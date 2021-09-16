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
#include "include/xi_tile3d_manager.h"
#include "include/flk_conv.h"
#include "include/xi_core_api.h"
#include "include/cnnrt.h"
#include "utils.h"

uint32_t xiDepthwiseConvGetMemReqd_Context(uint32_t *pContextSize)
{
	if(pContextSize) {
		*pContextSize = sizeof(conv_params_t);
		return 0;
	}
	else return 1;
}

uint32_t xiDepthwiseConvGetMemReqd_Coeff(uint8_t *pContext, uint32_t contextSize, uint32_t *pCoeffSize)
{
	if ( !pContext || (contextSize < sizeof(conv_params_t)) )
		return 1;
	conv_params_t *convParams = (conv_params_t *)pContext;
	*pCoeffSize = convParams->coeff_reord_size + sizeof(int32_t) * convParams->bias_reord_size;
	return 0;
}

static bool SetupDepthwise2(conv_params_t *params, conv_mem_info_t *mem_info)
{
	int H = params->output.H;
    int W = params->output.W;

#if (!REF_FLK_DEPTHWISE_CONV2D)
    CONV_FLAG_SET_KERNEL_KIND(params->flags, kkVQ_Depthwise);
#endif
    if (params->kernelW <= 16 && params->kernelH <= 16) {
        // Search for fast implementation
        for (unsigned int D = align_up(params->output.D, 32); D > 0; D -= 32) {
            params->tile.D = std::min(D, params->output.D);
            for (int i = W + H - 1; i > 0; i--) {
                if (i >= W) {
                    params->tile.W = W;
                    params->tile.H = i - W + 1;
                } else {
                    params->tile.W = i;
                    params->tile.H = 1;
                }
                return true;
            }
        }
    }
    return false;
}

static bool
ConvReorderCoefficients2(const uint8_t *coeff_ref, const int32_t* bias_ref, uint8_t *coeff, int32_t* bias, const conv_params_t *params)
{
	uint32_t depthMultiplier = params->output.D / params->input.D;
	uint32_t height = params->kernelH;
	uint32_t width = params->kernelW;
	uint32_t inDepth = params->input.D;
	/* Both Coeff and Coeff_ref share the same pitch */
	int32_t pitchW = params->output.D;
	int32_t pitchH = params->output.D * params->kernelW;

#if (REF_FLK_DEPTHWISE_CONV2D)
	switch (kkNone)
#else
	switch (CONV_FLAG_GET_KERNEL_KIND(params->flags))
#endif
	{
    case kkNone: { // need to convert coeff to S8 for XI-lib kernel
        for (int32_t h = 0; h < height; h++)
        {
          for (int32_t w = 0; w < width; w++)
          {
            for (int32_t dm = 0; dm < depthMultiplier; dm++)
            {
              for (int32_t d = 0; d < inDepth; d++)
              {
                int32_t srcIndex = dm + d * depthMultiplier + w *pitchW + h * pitchH;
                int32_t dstIndex = d + dm * inDepth + w * pitchW + h * pitchH; ;
               int8_t srcdata = (int8_t)coeff_ref[srcIndex];
                coeff[dstIndex] = coeff_ref[srcIndex];
              }
            }
          }
        }
        for(int32_t j=0; j< depthMultiplier; j++)
        {
      	for(int32_t i=0; i< inDepth; i++)
        	{
        		int32_t dstIndex = i + j * inDepth;
        		int32_t srcIndex = i * depthMultiplier + j;
        		bias[dstIndex] = bias_ref[srcIndex];
        	}
        }
        break;
	}
    case kkVQ_Depthwise: { // need to convert coeff to S8 for XI-lib kernel
		int offset = (params->tileType.dataType == XI_TILE3D_S8) ? 0 : 128;
		int tiles_count = (params->output.D + params->tile.D - 1) / params->tile.D;
        memcpy(bias, bias_ref, params->output.D * sizeof(int32_t));

        for (int tile = 0; tile < tiles_count; tile++) {
            int tile_depth = std::min(params->tile.D, params->output.D - tile * params->tile.D);
            int tile_depth64 = align_up(tile_depth, 64);
            for (unsigned int h = 0; h < params->kernelH; h++) {
                for (unsigned int w = 0; w < params->kernelW; w++) {
                    int d = 0;
                    for (; d < tile_depth; d++) {
                        *coeff++ = coeff_ref[(h * params->kernelW + w) * params->output.D + tile * params->tile.D + d];
                    }
                    for (; d < tile_depth64; d++) {
                        *coeff++ = 0;
                    }
                }
            }
        }
        break;
    }
    default:
      assert(0);
        return false;
    }
    return true;
}


uint32_t xiDepthwiseConvDoCoeffReorder(uint8_t *pContext, uint32_t contextSize,
		uint8_t *reordCoeffnBias, uint32_t reordCoeffnBiasSize, uint8_t *pFilter, int32_t *pBias)
{
	if ( !reordCoeffnBias || !pFilter || !pBias)
		return 1;
	if ( !pContext || (contextSize < sizeof(conv_params_t)) )
		return 1;
	conv_params_t *convParams = (conv_params_t *)pContext;
	if (reordCoeffnBiasSize < convParams->coeff_reord_size + sizeof(int32_t) * convParams->bias_reord_size)
		return 1;

	uint8_t *reordCoeff = reordCoeffnBias;
	int32_t *reordBias = (int32_t *)(reordCoeff + convParams->coeff_reord_size);
	return (uint32_t)(false == ConvReorderCoefficients2(pFilter, pBias, reordCoeff, reordBias, convParams));
}

uint32_t xiDepthwiseConvSetContext(uint8_t *pContext, uint32_t contextSize, const uint32_t inputD, const uint32_t inputW, const uint32_t inputH,
		  const uint32_t outputD, const uint32_t outputW, const uint32_t outputH, const uint32_t filterW, const uint32_t filterH,
		  const uint32_t stride_width, const uint32_t zeroPtInput, const uint32_t zeroPtCoeff, const uint32_t zeroPtOutput,
		  const uint32_t quantizedScale, const uint32_t outputShift, const uint32_t reluMin, const uint32_t reluMax)
{
	if ( !pContext || (contextSize < sizeof(conv_params_t)) )
		return 1;
	conv_params_t *convParams = (conv_params_t *)pContext;
	memset(convParams, 0, sizeof(conv_params_t));

	convParams->structSize = sizeof(conv_params_t);
	convParams->input = { inputD, inputW, inputH };
	convParams->output = { outputD, outputW, outputH };
	convParams->batch = 1;
	convParams->kernelW = filterW;
	convParams->kernelH = filterH;
	convParams->stride = stride_width;
	if(stride_width > 1) {
		convParams->offsetX = 1;
		convParams->offsetY = 1;
    }
	else {
	    convParams->offsetX = 0;
	    convParams->offsetY = 0;
	}

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
	local_mem_info_t *mem_info = getMeminfoContext();
	size_t bank0Size = mem_info->bank[0].size;
	size_t bank1Size = mem_info->bank[1].size;
	uint32_t largeBank  = std::max(bank0Size, bank1Size);
	uint32_t smallBank  = std::min(bank0Size, bank1Size);

	mem_inf.localMem.banksNumber = arena_num_banks();
	mem_inf.localMem.bankSize[0] = bank0Size;
	mem_inf.localMem.bankSize[1] = bank1Size;
if (mem_inf.localMem.banksNumber < 2)
	  mem_inf.localMem.banksMode = 0;
else
	  mem_inf.localMem.banksMode = 2; // ((arena_contiguous_banks())? 1 : 2);

	SetupDepthwise2(convParams, &mem_inf);

	size_t tiles = (convParams->output.D + convParams->tile.D - 1) / convParams->tile.D;
	convParams->coeff_reord_size = convParams->kernelW * convParams->kernelH;
	convParams->coeff_reord_size *= align_up(convParams->tile.D, 64) * tiles;
	convParams->bias_reord_size = convParams->output.D;
	return 0;
}

uint32_t xiDepthwiseConv(uint8_t *pContext, uint32_t contextSize, int8_t * input, uint32_t inputSize, int8_t * output, uint32_t outputSize,
		int8_t *reordCoeffnBias, uint32_t reordCoeffnBiasSize, int32_t *outScale, int8_t *outShift, uint32_t num_channels,uint32_t paddingWidth, uint32_t paddingHeight)
{
	if ( !pContext || (contextSize < sizeof(conv_params_t)) )
		return 1;
	conv_params_t *convParams = (conv_params_t *)pContext;
	if (reordCoeffnBiasSize < convParams->coeff_reord_size + sizeof(int32_t) * convParams->bias_reord_size)
		return 1;

	int8_t *reordCoeff = reordCoeffnBias;
	int32_t *reordBias = (int32_t *)(reordCoeff + convParams->coeff_reord_size);

#if (REF_FLK_DEPTHWISE_CONV2D)
	XtensaOperationArgsIn inputs = {
	  7,
	  {input,
	  (int8_t *)reordCoeff,
      (int8_t*)reordBias,
      (int8_t*)outScale,
      (int8_t *)outShift,
	  (int8_t *)&paddingWidth,
	  (int8_t *)&paddingHeight},
	  {inputSize, convParams->coeff_reord_size, sizeof(int32_t) * convParams->bias_reord_size, num_channels, num_channels,sizeof(int32_t),sizeof(int32_t) }
	};
#else
	XtensaOperationArgsIn inputs = {
	  5,
	  {input,
	  (int8_t *)reordCoeff,
      (int8_t*)reordBias,
      (int8_t*)outScale,
      (int8_t *)outShift},
	  {inputSize, convParams->coeff_reord_size, sizeof(int32_t) * convParams->bias_reord_size, num_channels, num_channels }
	};
#endif
	struct XtensaOperationArgsOut outputs = {
	  1,
	  {output,},
	  {outputSize, }
	};

#if FLK_CYCLES
  int start = XT_RSR_CCOUNT();
#endif
#if (REF_FLK_DEPTHWISE_CONV2D)
  flk_depthwise_conv_ref((uint8_t*)convParams, &inputs, &outputs);
#else
  flk_depthwise_conv(reinterpret_cast<const uint8_t*>(convParams), &inputs, &outputs);
#endif
#if FLK_CYCLES
  int stop = XT_RSR_CCOUNT();
  printf("DepthwiseConv2D (including iDMA) =%d\n",stop-start);
#endif

#if !IS_MULTICHANNEL_DMA
	dma_barrier();
#endif

#if KERNEL_INFO
	printf("depth_conv2d:iW=%d,iH=%d,iD=%d \t kw=%d,kh=%d \t oW=%d,oH=%d,oD=%d \n", convParams->input.W, convParams->input.H, convParams->input.D, convParams->kernelW, convParams->kernelH, convParams->output.W, convParams->output.H, convParams->output.D );
#endif
	return 0;
}
