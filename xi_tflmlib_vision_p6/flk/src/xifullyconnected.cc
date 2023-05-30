/*******************************************************************************
* Copyright (c) 2019 Cadence Design Systems, Inc.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to use this Software with Cadence processor cores only and
* not with any other processors and platforms, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/
#include <algorithm>
#include "xi_tile3d_manager.h"
#include "flk_conv.h"
#include "xi_core_api.h"
#include "utils.h"

uint32_t xiFullyConnectedGetMemReqd_Context(uint32_t *pContextSize)
{
	if(pContextSize) {
		*pContextSize = sizeof(conv_params_t);
		return 0;
	}
	else return 1;
}

uint32_t xiFullyConnectedGetMemReqd_Coeff(uint8_t *pContext, uint32_t contextSize, uint32_t *pCoeffSize)
{
	if ( !pContext || (contextSize < sizeof(conv_params_t)) )
		return 1;
	conv_params_t *convParams = (conv_params_t *)pContext;
	*pCoeffSize = convParams->coeff_reord_size + sizeof(int32_t) * convParams->bias_reord_size;
	return 0;
}
uint32_t xiFullyConnectedSetContext(uint8_t *pContext, uint32_t contextSize, const uint32_t *inDims,
		  const uint32_t *outDims, const uint32_t *filterDims,
		  const uint32_t stride_width, const uint32_t zeroPtInput, const uint32_t zeroPtCoeff, const uint32_t zeroPtOutput,
		  const uint32_t quantizedScale, const uint32_t outputShift, const uint32_t reluMin, const uint32_t reluMax, uint8_t* pFilter)
{
  if ( !pContext || (contextSize < sizeof(conv_params_t)) )
	return 1;
  conv_params_t *convParams = (conv_params_t *)pContext;
  memset(convParams, 0, sizeof(conv_params_t));
  convParams->structSize = sizeof(conv_params_t);
  convParams->input = { inDims[0], inDims[1], inDims[2] };
  convParams->output = { outDims[0], outDims[1], outDims[2] };
  convParams->batch = inDims[3];
  convParams->kernelW = filterDims[1];
  convParams->kernelH = filterDims[2];
  convParams->stride = stride_width;
  uint32_t filterW = filterDims[1];
  uint32_t filterH = filterDims[2];
  convParams->offsetX = filterW / 2;
  convParams->offsetY = filterH / 2;

  convParams->tileType.dataType = XI_TILE3D_S8;
  convParams->zeroPtInput = zeroPtInput;
  convParams->zeroPtCoeff = zeroPtCoeff;
  convParams->zeroPtOutput = zeroPtOutput;
  convParams->quantizedScale = quantizedScale;
  convParams->outputShift = (uint32_t)(-(int32_t)outputShift);
  convParams->reluMin = reluMin;
  convParams->reluMax = reluMax;
  convParams->VQ7_512MAC_supported = 0;
#define CONV_FLAG_SET_RELU(flags, value)               (flags) = ((flags) & ~1) | ((value) & 1)
  CONV_FLAG_SET_RELU(convParams->flags, 1 != 1);
#define CONV_FLAG_RESET_FP16(flags)  ((flags) = (flags) & (~(1 << 11)))
  CONV_FLAG_RESET_FP16(convParams->flags);

  conv_mem_info_t mem_inf;

  convParams->batch = outDims[3];
  convParams->input.D = (inDims[0] * inDims[1] * inDims[2] * inDims[3]) / convParams->batch;
  convParams->input.W = 1;
  convParams->input.H = 1;

  conv_kernel_kind_e kernel_kind;
#if 1
  mem_inf.localMem.banksNumber = arena_num_banks();
  uint32_t free_space = 0;
  arena_bank_free_space(0, &free_space);
  mem_inf.localMem.bankSize[0] = free_space;
  free_space = 0;
  arena_bank_free_space(1, &free_space);
  mem_inf.localMem.bankSize[1] = free_space;
//  if (mem_inf.localMem.banksNumber < 2)
//  mem_inf.localMem.banksMode = 0;
//  else
//  mem_inf.localMem.banksMode = 2; // ((arena_contiguous_banks())? 1 : 2);
#endif
  convParams->isVQ7optimize = 0;
  convParams->tile.D = convParams->output.D;
  convParams->tile.W = convParams->output.W;
  convParams->tile.H = convParams->output.H;


  kernel_kind = kkFC;
  bool allow_acc24 = true;
  bool ofl_flag = MayOverflowAccumulator24(pFilter, filterW * filterH * inDims[0], outDims[0]);

  if (allow_acc24 && ofl_flag)
  {
    allow_acc24 = false;
  }
  CONV_FLAG_SET_KERNEL_KIND(convParams->flags, kernel_kind);
  SetupConv2D(*convParams, allow_acc24, 0, kernel_kind, &mem_inf, 1, convParams->batch);

  // Choose which input to reload if it's inevitable
  int64_t totalSpatialTiles = mem_inf.numTilesH * mem_inf.numTilesW * convParams->batch;
  int64_t reloadInput = (mem_inf.numTilesD - 1) * totalSpatialTiles * mem_inf.inpTileSize;
  int64_t reloadCoeff = mem_inf.numTilesD * (totalSpatialTiles - 1) * mem_inf.coeffTileSize;
  CONV_FLAG_SET_RELOAD_INPUT(convParams->flags, reloadInput <= reloadCoeff);

  bool kernel_F16flag = (CONV_FLAG_GET_KERNEL_KIND(convParams->flags) == kkFC_FP16);
  convParams->coeff_reord_size = (ConvCoefficientsBufferSize(*convParams) ) << kernel_F16flag;
  convParams->bias_reord_size = (convParams->output.D ) << kernel_F16flag;
  convParams->scale_size = align_up((convParams->output.D *sizeof(int32_t)), sizeof(int32_t)) << kernel_F16flag;
  convParams->shift_size = (convParams->output.D) << kernel_F16flag;

  return 0;
}

uint32_t xiFullyConnected(uint8_t *pContext, uint32_t contextSize, int8_t * input, uint32_t inputSize, int8_t * output, uint32_t outputSize,
		int8_t *reordCoeffnBias, uint32_t reordCoeffnBiasSize, int32_t* outScale, int8_t* outShift, uint32_t num_channels)
{
	if ( !pContext || (contextSize < sizeof(conv_params_t)) )
		return 1;
	conv_params_t *convParams = (conv_params_t *)pContext;
	if (reordCoeffnBiasSize < convParams->coeff_reord_size + sizeof(int32_t) * convParams->bias_reord_size)
		return 1;

	int8_t *reordCoeff = reordCoeffnBias;
	int32_t *reordBias = (int32_t *)(reordCoeff + convParams->coeff_reord_size);

	XtensaOperationArgsIn inputs = {
    3,
    {input,
    (int8_t*)reordCoeff,
      (int8_t*)reordBias,
    },
    {inputSize, convParams->coeff_reord_size, sizeof(int32_t) * convParams->bias_reord_size}
  };

	struct XtensaOperationArgsOut outputs = {
	  1,
	  {output,},
	  {outputSize, }
	};

	flk_conv(reinterpret_cast<const uint8_t*>(convParams), &inputs, &outputs);

#if !IS_MULTICHANNEL_DMA
	dma_barrier();
#endif
	return 0;
}

uint32_t xiFullyConnectedDoCoeffReorder(uint8_t *pContext, uint32_t contextSize,
		uint8_t *reordCoeffnBias, uint32_t reordCoeffnBiasSize, uint8_t *pFilter, int32_t *pBias)
{
	if ( !reordCoeffnBias || !pFilter )
		return 1;
	if ( !pContext || (contextSize < sizeof(conv_params_t)) )
		return 1;
	conv_params_t *convParams = (conv_params_t *)pContext;
	if (reordCoeffnBiasSize < convParams->coeff_reord_size + sizeof(int32_t) * convParams->bias_reord_size)
		return 1;

	uint8_t *reordCoeff = reordCoeffnBias;
	int32_t *reordBias = (int32_t *)(reordCoeff + convParams->coeff_reord_size);
	return (uint32_t)(false == ConvReorderCoefficients(pFilter, pBias, reordCoeff, reordBias, *convParams));
}
