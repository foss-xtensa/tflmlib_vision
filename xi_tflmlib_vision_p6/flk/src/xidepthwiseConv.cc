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
#include <assert.h>
#include <algorithm>
#include "xi_tile3d_manager.h"
#include "flk_conv.h"
#include "xi_core_api.h"
#include "cnnrt.h"
#include "utils.h"

uint32_t xiDepthwiseConvGetMemReqd_Context(uint32_t* pContextSize)
{
  if (pContextSize) {
    *pContextSize = sizeof(conv_params_t);
    return 0;
  }
  else return 1;
}

uint32_t xiDepthwiseConvGetMemReqd_Coeff(uint8_t* pContext, uint32_t contextSize, uint32_t* pCoeffSize)
{
  if (!pContext || (contextSize < sizeof(conv_params_t)))
    return 1;
  conv_params_t* convParams = (conv_params_t*)pContext;
  *pCoeffSize = convParams->coeff_reord_size + sizeof(int32_t) * convParams->bias_reord_size;
  return 0;
}

uint32_t xiDepthwiseConvDoCoeffReorder(uint8_t* pContext, uint32_t contextSize,
  uint8_t* reordCoeffnBias, uint32_t reordCoeffnBiasSize, uint8_t* pFilter, int32_t* pBias)
{
  if (!reordCoeffnBias || !pFilter || !pBias)
    return 1;
  if (!pContext || (contextSize < sizeof(conv_params_t)))
    return 1;
  conv_params_t* convParams = (conv_params_t*)pContext;
  if (reordCoeffnBiasSize < convParams->coeff_reord_size + sizeof(int32_t) * convParams->bias_reord_size)
    return 1;

  uint8_t* reordCoeff = reordCoeffnBias;
  int32_t* reordBias = (int32_t*)(reordCoeff + convParams->coeff_reord_size);
  return (uint32_t)(false == ConvReorderCoefficients(pFilter, pBias, reordCoeff, reordBias, *convParams));
}

uint32_t xiDepthwiseConvSetContext(uint8_t* pContext, uint32_t contextSize, const uint32_t inputD, const uint32_t inputW, const uint32_t inputH,
  const uint32_t outputD, const uint32_t outputW, const uint32_t outputH, const uint32_t filterW, const uint32_t filterH,
  const uint32_t stride_width, const uint32_t zeroPtInput, const uint32_t zeroPtCoeff, const uint32_t zeroPtOutput,
  const uint32_t quantizedScale, const uint32_t outputShift, const uint32_t reluMin, const uint32_t reluMax, const uint32_t paddingWidth, const uint32_t paddingHeight)
{
  if (!pContext || (contextSize < sizeof(conv_params_t)))
    return 1;
  conv_params_t* convParams = (conv_params_t*)pContext;
  memset(convParams, 0, sizeof(conv_params_t));

  convParams->structSize = sizeof(conv_params_t);
  convParams->input = { inputD, inputW, inputH };
  convParams->output = { outputD, outputW, outputH };
  convParams->batch = 1;
  convParams->kernelW = filterW;
  convParams->kernelH = filterH;
  convParams->stride = stride_width;

  if (paddingWidth) {
    convParams->offsetX = 0;
  }
  else  {
    convParams->offsetX = filterW / 2;
  }

  if (paddingHeight) {
    convParams->offsetY = 0;
  }
  else {
    convParams->offsetY = filterH / 2;
  }


  convParams->tileType.dataType = XI_TILE3D_S8;
  convParams->coeffType.dataType = XI_TILE4D_S8;
  convParams->zeroPtInput = zeroPtInput;
  convParams->zeroPtCoeff = zeroPtCoeff;
  convParams->zeroPtOutput = zeroPtOutput;
  convParams->quantizedScale = quantizedScale;
  convParams->outputShift = outputShift;
  convParams->reluMin = reluMin;
  convParams->reluMax = reluMax;
#define CONV_FLAG_SET_RELU(flags, value)               (flags) = ((flags) & ~1) | ((value) & 1)
  CONV_FLAG_SET_RELU(convParams->flags, 1 != 1);

  conv_mem_info_t mem_inf;
  local_mem_info_t* mem_info = getMeminfoContext();
  size_t bank0Size = mem_info->bank[0].size;
  size_t bank1Size = mem_info->bank[1].size;
  //uint32_t largeBank  = std::max(bank0Size, bank1Size);
  //uint32_t smallBank  = std::min(bank0Size, bank1Size);

  mem_inf.localMem.banksNumber = arena_num_banks();
  mem_inf.localMem.bankSize[0] = bank0Size;
  mem_inf.localMem.bankSize[1] = bank1Size;
  if (mem_inf.localMem.banksNumber < 2)
    mem_inf.localMem.banksMode = 0;
  else
    mem_inf.localMem.banksMode = 2; // ((arena_contiguous_banks())? 1 : 2);

  SetupDepthwise(*convParams, kkVQ_Depthwise, &mem_inf, 1);

  size_t tiles = (convParams->output.D + convParams->tile.D - 1) / convParams->tile.D;
  convParams->coeff_reord_size = convParams->kernelW * convParams->kernelH;
  convParams->coeff_reord_size *= align_up(convParams->tile.D, ALIGNMENT) * tiles;
  convParams->bias_reord_size = convParams->output.D;
  return 0;
}

uint32_t xiDepthwiseConv(uint8_t* pContext, uint32_t contextSize, int8_t* input, uint32_t inputSize, int8_t* output, uint32_t outputSize,
  int8_t* reordCoeffnBias, uint32_t reordCoeffnBiasSize, int32_t* outScale, int8_t* outShift, uint32_t num_channels, uint32_t paddingWidth, uint32_t paddingHeight)
{
  if (!pContext || (contextSize < sizeof(conv_params_t)))
    return 1;
  conv_params_t* convParams = (conv_params_t*)pContext;
  if (reordCoeffnBiasSize < convParams->coeff_reord_size + sizeof(int32_t) * convParams->bias_reord_size)
    return 1;

  int8_t* reordCoeff = reordCoeffnBias;
  int32_t* reordBias = (int32_t*)(reordCoeff + convParams->coeff_reord_size);

  XtensaOperationArgsIn inputs = {
    5,
    {input,
    (int8_t*)reordCoeff,
      (int8_t*)reordBias,
      (int8_t*)outScale,
      (int8_t*)outShift},
    {inputSize, convParams->coeff_reord_size, sizeof(int32_t) * convParams->bias_reord_size, num_channels, num_channels }
  };
  struct XtensaOperationArgsOut outputs = {
    1,
    {output,},
    {outputSize, }
  };

  flk_depthwise_conv(reinterpret_cast<const uint8_t*>(convParams), &inputs, &outputs);

#if !IS_MULTICHANNEL_DMA
  dma_barrier();
#endif

  return 0;
}
