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
#include "include/xi_tile3d_manager.h"
#include "include/flk_conv.h"
#include "include/xi_core_api.h"
#include "utils.h"

uint32_t xiConvGetMemReqd_Context(uint32_t *pContextSize)
{
	if(pContextSize) {
		*pContextSize = sizeof(conv_params_t);
		return 0;
	}
	else return 1;
}

uint32_t xiConvGetMemReqd_Coeff(uint8_t *pContext, uint32_t contextSize, uint32_t *pCoeffSize)
{
	if ( !pContext || (contextSize < sizeof(conv_params_t)) )
		return 1;
	conv_params_t *convParams = (conv_params_t *)pContext;
	*pCoeffSize = convParams->coeff_reord_size + sizeof(int32_t) * convParams->bias_reord_size;
	return 0;
}

static size_t ConvCoefficientsBufferSize2(const conv_params_t *params)
{
    size_t coeff_size = params->kernelW * params->kernelH;
    switch (CONV_FLAG_GET_KERNEL_KIND(params->flags)) {
    //switch (kkQM24) {
    case kkQM24:
    case kkQM32: {
        coeff_size = align_up(params->kernelW * params->input.D, QM32_ROW_PADDING);
        coeff_size = params->output.D * align_up(coeff_size * params->kernelH, QM32_FILTER_PADDING);
        break;
    }
    case kkVQ_QM24:
    case kkVQ_QM32: {
        size_t tiles = (params->output.D + params->tile.D - 1) / params->tile.D;
        coeff_size *= align_up(params->tile.D, 64) * params->input.D * tiles;
        break;
    }
    case kkGS_MOW1x1: {
        coeff_size = coeff_size * params->input.D * params->output.D;
        break;
    }
    case kkNone : {
        // No padding needed.
        coeff_size *= params->output.D * params->input.D;
        break;
    }
    default: {
        // No padding needed.
        printf("invalid case. Abort...\n");
        assert(0);
        break;
    }
    }
    return coeff_size;
}

uint32_t xiConvSetContext(uint8_t *pContext, uint32_t contextSize, const uint32_t inputD, const uint32_t inputW, const uint32_t inputH,
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
  convParams->offsetX = 0;
  convParams->offsetY = 0;

  convParams->tileType.dataType = XI_TILE4D_U8;
  convParams->zeroPtInput = zeroPtInput;
  convParams->zeroPtCoeff = zeroPtCoeff;
  convParams->zeroPtOutput = zeroPtOutput;
  convParams->quantizedScale = quantizedScale;
  convParams->outputShift = outputShift;
  convParams->reluMin = reluMin;
  convParams->reluMax = reluMax;
#if (XCHAL_VISION_TYPE >= 7) && (defined(IVP_DMULQA2N8QXR8))
  convParams->VQ7_512MAC_supported = 1;
#else
  convParams->VQ7_512MAC_supported = 0;
#endif
#define CONV_FLAG_SET_RELU(flags, value)               (flags) = ((flags) & ~1) | ((value) & 1)
  CONV_FLAG_SET_RELU(convParams->flags, 1 != 1);
#define CONV_FLAG_RESET_FP16(flags)  ((flags) = (flags) & (~(1 << 11)))
  CONV_FLAG_RESET_FP16(convParams->flags);

  conv_mem_info_t mem_inf;
  local_mem_info_t *mem_info = getMeminfoContext();

  size_t bank0Size = mem_info->bank[0].size;
  size_t bank1Size = mem_info->bank[1].size;
 // uint32_t largeBank  = std::max(bank0Size, bank1Size);
 // uint32_t smallBank  = std::min(bank0Size, bank1Size);

  mem_inf.localMem.banksNumber = arena_num_banks();
  mem_inf.localMem.bankSize[0] = bank0Size;
  mem_inf.localMem.bankSize[1] = bank1Size;
  if (mem_inf.localMem.banksNumber < 2)
  mem_inf.localMem.banksMode = 0;
  else
  mem_inf.localMem.banksMode = 2; // ((arena_contiguous_banks())? 1 : 2);

  convParams->isVQ7optimize = 0;
  // TODO : current 'person detection' usecase can fit whole output in one tile.
  // add assert checks otherwise
  convParams->tile.D = convParams->output.D;
  convParams->tile.W = convParams->output.W;
  convParams->tile.H = convParams->output.H;
#if !(REF_FLK_CONV2D)
#if 0
  if ( (convParams->input.H == 1 && convParams->input.W == 1)
		  || (convParams->input.D <= 8) )
	  CONV_FLAG_SET_KERNEL_KIND(convParams->flags, kkGS_MOW1x1);
  else
	  CONV_FLAG_SET_KERNEL_KIND(convParams->flags, kkQM24);
#endif
  CONV_FLAG_SET_KERNEL_KIND(convParams->flags, kkVQ_QM24);
#endif

  // Choose which input to reload if it's inevitable
  int64_t totalSpatialTiles = mem_inf.numTilesH * mem_inf.numTilesW * convParams->batch;
  int64_t reloadInput = (mem_inf.numTilesD - 1) * totalSpatialTiles * mem_inf.inpTileSize;
  int64_t reloadCoeff = mem_inf.numTilesD * (totalSpatialTiles - 1) * mem_inf.coeffTileSize;
  CONV_FLAG_SET_RELOAD_INPUT(convParams->flags, reloadInput <= reloadCoeff);

  bool kernel_F16flag = (CONV_FLAG_GET_KERNEL_KIND(convParams->flags) == kkFC_FP16);
  convParams->coeff_reord_size = (ConvCoefficientsBufferSize2(convParams) ) << kernel_F16flag;
  convParams->bias_reord_size = (convParams->output.D ) << kernel_F16flag;
  convParams->scale_size = align_up((convParams->output.D *sizeof(int32_t)), sizeof(int32_t)) << kernel_F16flag;
  convParams->shift_size = (convParams->output.D) << kernel_F16flag;

  return 0;
}

static bool
ConvReorderCoefficients2(const uint8_t *coeff_ref, const int32_t* bias_ref, uint8_t *coeff, int32_t* bias, const conv_params_t *params)
{
    int offset = (params->tileType.dataType == XI_TILE3D_S8) ? 0 : 128;
    int8_t *coeff_ref_s8 = (int8_t *)coeff_ref;
    (void)coeff_ref_s8;
    int tiles_count = (params->output.D + params->tile.D - 1) / params->tile.D;

    // TODO
#if (REF_FLK_CONV2D)
    switch (kkFC) {
#else
    switch (CONV_FLAG_GET_KERNEL_KIND(params->flags)) {
#endif //(REF_FLK_CONV2D)

    case kkQM24:
    case kkQM32:
    {
        //printf("non VQ7optimize version, offset: %d\n", offset);
        int8_t *coeff_s8 = (int8_t *)coeff;
        size_t src_filter_size = params->kernelH * params->kernelW * params->input.D;
        size_t dst_filter_row_size = align_up(params->kernelW * params->input.D, QM32_ROW_PADDING);
        size_t dst_filter_size = align_up(params->kernelH * dst_filter_row_size, QM32_FILTER_PADDING);
        size_t pad_h = dst_filter_row_size - params->kernelW * params->input.D;
        size_t pad_filter = dst_filter_size - params->kernelH * dst_filter_row_size;
        //assert(QM32_ROW_PADDING >= 4, "Assuming that row size is at least multiple of 4");
        for (int tile = 0; tile < tiles_count; tile++) {
            unsigned int tile_depth = std::min(params->tile.D, params->output.D - tile * params->tile.D);
            for (unsigned int band = 0; band < tile_depth; band += 2 * IVPN_SIMD_WIDTH) {
                size_t band_size = std::min((unsigned int) 2 * IVPN_SIMD_WIDTH, tile_depth - band);
                // Split even and odd filter dwords into two contiguous streams.
                for (size_t half = 0; half < 2; half++) {
                    for (unsigned int h = 0; h < params->kernelH; h++) {
                        for (unsigned int w = 0; w < params->kernelW; w++) {
                            for (unsigned int ch = 0; ch < params->input.D; ch++) {
                                size_t src_start = h * dst_filter_row_size + w * params->input.D + ch;
                                if ((src_start / 2) % 2 == half) {
                                    for (unsigned int d = 0; d < band_size; d++) {
                                        size_t elt = tile * params->tile.D + band + d;
                                        elt = elt * params->kernelH + h;
                                        elt = elt * params->kernelW + w;
                                        elt = elt * params->input.D + ch;
                                        *coeff_s8++ = (int8_t)(coeff_ref[elt] - offset);
                                    }
                                }
                            }
                        }
                        // Zero pad tail of each row
                        for (unsigned int i = 0; i < pad_h; i++) {
                            size_t src_start = h * dst_filter_row_size + params->kernelW * params->input.D + i;
                            if ((src_start / 2) % 2 == half) {
                                for (unsigned int d = 0; d < band_size; d++) {
                                    *coeff_s8++ = 0;
                                }
                            }
                        }
                    }
                    // Zero pad filter to reflect unrolling by two in the kernel
                    for (unsigned int i = 0; i < pad_filter / 2; i++) {
                        for (unsigned int d = 0; d < band_size; d++) {
                            *coeff_s8++ = 0;
                        }
                    }
                }
            }
            // Compute fixup and copy bias in one pass
            int const_fixup = (params->zeroPtInput * params->zeroPtCoeff - offset * offset) * src_filter_size;
            for (unsigned int d = 0; d < tile_depth; d++) {
                int32_t fixup = 0;
                for (unsigned int i = 0; i < src_filter_size; i++) {
                    fixup += coeff_ref[(tile * params->tile.D + d) * src_filter_size + i];
                }
                *bias++ = *bias_ref++ + (offset - params->zeroPtInput) * fixup + const_fixup;
            }
        }
        break;
    }
    case kkVQ_QM24:
    case kkVQ_QM32:{
        int8_t *coeff_s8 = (int8_t *)coeff;
        for (int tile = 0; tile < tiles_count; tile++) {
            unsigned int tile_depth = std::min(params->tile.D, params->output.D - tile * params->tile.D);
            unsigned int tile_depth64 = align_up(tile_depth, 64);
            //for (unsigned int n = 0; n < tile_depth; n++) {
                for (unsigned int h = 0; h < params->kernelH; h++) {
                    for (unsigned int w = 0; w < params->kernelW; w++) {
                        for (unsigned int ch = 0; ch < params->input.D; ch++) {
                            unsigned int n = 0;
                            for (; n < tile_depth; n++) {
                                int32_t idx = (((tile * params->tile.D + n)* params->kernelH + h) * params->kernelW + w) * params->input.D + ch;
                                *coeff_s8++ = (int8_t)coeff_ref[idx];
                            }
                            for (; n < tile_depth64; n++) {
                                *coeff_s8++ = 0;
                            }
                        }
                    }
                }
            //}
            // copy bias in one pass
            uint32_t src_filter_size = params->kernelH * params->kernelW * params->input.D;
            for (unsigned int d = 0; d < tile_depth; d++) {
                int fixup = 0;
                for (unsigned i = 0; i < src_filter_size; i++) {
                    fixup +=(int8_t)coeff_ref[(tile * params->tile.D  + d) * src_filter_size + i];
                }
                *bias++ = *bias_ref++ - (params->zeroPtInput * fixup);
            }
        }
        break;
    }
    case kkFC: {
        memcpy(coeff, coeff_ref, params->output.D * params->kernelH * params->kernelW * params->input.D);
        memcpy(bias, bias_ref, params->output.D * sizeof(int32_t));
        break;
    }
    case kkGS_MOW1x1: {
        int8_t *coeff_s8 = (int8_t *)coeff;

        for (int tile = 0; tile < tiles_count; tile++) {
            // Reorder tile to WHDN format (width fastest dimension)
            unsigned int tile_depth = std::min(params->tile.D, params->output.D - tile * params->tile.D);
            for (unsigned int d = 0; d < tile_depth; d++) {
                for (unsigned int ch = 0; ch < params->input.D; ch++) {
                    for (unsigned int h = 0; h < params->kernelH; h++) {
                        for (unsigned int w = 0; w < params->kernelW; w++) {
                            *coeff_s8++ = (int8_t)(coeff_ref[(((tile * params->tile.D + d) * params->kernelH + h) * params->kernelW + w) * params->input.D + ch] - offset);
                        }
                    }
                }
            }
            // Compute fixup and copy bias in one pass
            unsigned int kernel_volume = params->kernelH * params->kernelW * params->input.D;
            int const_fixup = params->zeroPtInput * params->zeroPtCoeff * kernel_volume;
            for (unsigned int d = 0; d < tile_depth; d++) {
                int32_t fixup = 0;
                for (unsigned int i = 0; i < kernel_volume; i++) {
                    fixup += coeff_ref[(tile * params->tile.D + d) * kernel_volume + i];
                }
                *bias++ = *bias_ref++ - params->zeroPtInput * fixup + const_fixup;
            }
        }
        break;
    }
    default:
        return false;
    }
    return true;
}

uint32_t xiConv(uint8_t *pContext, uint32_t contextSize, int8_t * input, uint32_t inputSize, int8_t * output, uint32_t outputSize,
		int8_t *reordCoeffnBias, uint32_t reordCoeffnBiasSize, int32_t *outScale, int8_t *outShift, uint32_t num_channels)
{
	if ( !pContext || (contextSize < sizeof(conv_params_t)) )
		return 1;
	conv_params_t *convParams = (conv_params_t *)pContext;
	if (reordCoeffnBiasSize < convParams->coeff_reord_size + sizeof(int32_t) * convParams->bias_reord_size)
		return 1;

	int8_t *reordCoeff = reordCoeffnBias;
	int32_t *reordBias = (int32_t *)(reordCoeff + convParams->coeff_reord_size);

	XtensaOperationArgsIn inputs = {
	  5,
	  {input,
	  (int8_t *)reordCoeff,
      (int8_t*)reordBias,
      (int8_t*)outScale,
      (int8_t *)outShift, },
	  {inputSize, convParams->coeff_reord_size, sizeof(int32_t) * convParams->bias_reord_size, num_channels, num_channels}
	};

	struct XtensaOperationArgsOut outputs = {
	  1,
	  {output,},
	  {outputSize, }
	};

#if FLK_CYCLES
    int start = XT_RSR_CCOUNT();
#endif
#if (REF_FLK_CONV2D)
	flk_conv_ref(reinterpret_cast<const uint8_t*>(convParams), &inputs, &outputs);
#else
	flk_conv(reinterpret_cast<const uint8_t*>(convParams), &inputs, &outputs);
#endif //(REF_FLK_CONV2D)
#if FLK_CYCLES
    int stop = XT_RSR_CCOUNT();
    printf("Conv2D (including iDMA) =%d\n",stop-start);
#endif

#if !IS_MULTICHANNEL_DMA
	dma_barrier();

#if KERNEL_INFO
	printf("Conv2D:iW=%d,iH=%d,iD=%d \t kw=%d,kh=%d \t oW=%d,oH=%d,oD=%d \n", convParams->input.W, convParams->input.H, convParams->input.D, convParams->kernelW, convParams->kernelH, convParams->output.W, convParams->output.H, convParams->output.D );

#endif
#endif
	return 0;
}

uint32_t xiConvDoCoeffReorder(uint8_t *pContext, uint32_t contextSize,
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

conv_kernel_kind_e kernelKind(const conv_params_t &params) { return kkNone; }
int  allowAccumulator24() { return true; }
