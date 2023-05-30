/*
 * Copyright (c) 2020 by Cadence Design Systems, Inc. ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of
 * Cadence Design Systems Inc. Any rights to use, modify, and create
 * derivative works of this file are set forth under the terms of your
 * license agreement with Cadence Design Systems, Inc.
 */

#include <algorithm>
#include "xi_tile3d_manager.h"
#include "flk_conv.h"
#include "xi_core_api.h"
#include "utils.h"


size_t
ConvCoefficientsBufferSize(const conv_params_t& params)
{
  size_t coeff_size = params.kernelW * params.kernelH;
  switch (CONV_FLAG_GET_KERNEL_KIND(params.flags))
  {
  case kkCa2:
    {
        // Pad in coefficients tile depth to ALIGNMENT.
        size_t tiles = (params.output.D + params.tile.D - 1) / params.tile.D;
        coeff_size *= align_up(params.tile.D, ALIGNMENT) * params.input.D * tiles;
        break;
    }

  case kkQM24:
  case kkQM32:
    {
        {
          coeff_size = align_up(params.kernelW * params.input.D, QM32_ROW_PADDING);
          coeff_size = params.output.D * align_up(coeff_size * params.kernelH, QM32_FILTER_PADDING);
        }
        break;
    }

  case kkVQ_QM24:
  case kkVQ_QM32:
    {
        {
          size_t tiles = (params.output.D + params.tile.D - 1) / params.tile.D;
        coeff_size *= align_up(params.tile.D, ALIGNMENT) * params.input.D * tiles;
        }
        //coeff_size *= params.input.D * params.output.D;
        break;
    }

  case kkInvQM24:
  case kkVQ_InvQM24:
    {
        coeff_size = coeff_size * params.input.D * params.output.D;
        break;
    }

  case kkGS_MOW:
  case kkVQ_GS_MOW:
    {
        coeff_size = coeff_size * params.input.D * params.output.D;
        break;
    }

  case kkGS_MOW1x1:
  case kkVQ_GS_MOW1x1:
    {
        coeff_size = coeff_size * params.input.D * params.output.D;
        break;
    }

  case kkGS_MOW_S2:
  case kkVQ_GS_MOW_S2:
    {
        coeff_size = coeff_size * params.input.D * params.output.D;
        break;
    }

  case kkDepthwise:
  case kkVQ_Depthwise:
    {
        int outputD;
        int tileD;

        outputD = params.output.D;
        tileD   = params.tile.D;
        // Pad in coefficients tile depth to ALIGNMENT.
        size_t tiles = (outputD + tileD - 1) / tileD;
        coeff_size *= align_up(tileD, ALIGNMENT) * tiles;
        break;
    }

  default:
    {
        // No padding needed.
        coeff_size *= params.output.D * params.input.D;
        break;
    }
  }

  return(coeff_size);
}

int setupDeviceMemoryLayout(uint32_t* sizes,
  int arraysNum, localMem_info_t* localMem,
  const banks_info_t* banks_info)
{
  uint32_t    sizeA;
  uint32_t    sizeB;
  uint32_t    retAssign;
  int         i, j;
  int         reference[XTENSA_OPERATION_MAX_NUM_OPERANDS];
  uint32_t    lsizes[XTENSA_OPERATION_MAX_NUM_OPERANDS];
  banks_info_t* banksInfo = (banks_info_t*)banks_info;

  // Try to fill split banks first
  if (localMem->banksNumber == 2) {
    sizeA = 0;
    sizeB = 0;
    retAssign = 0;

    //  create a reference array and sort length array
    for (i = 0; i < arraysNum; i++) {
      // fill reference array
      reference[i] = i;
      lsizes[i] = sizes[i];
    }

    // use an insertion sort, descending order
    for (i = 1; i < arraysNum; i++) {
      uint32_t t = lsizes[i];
      uint32_t t2 = reference[i];
      for (j = i; j >= 1 && t > lsizes[j - 1]; j--) {
        lsizes[j] = lsizes[j - 1];
        reference[j] = reference[j - 1];
      }
      lsizes[j] = t;
      reference[j] = t2;
    }

    // fill banks with biggest array 1st, then next size etc
    for (i = 0; i < arraysNum; i++) {
      if (!lsizes[i])
        break;  // all zero-length (absent) arrays are at the end
      if ((sizeA + lsizes[i]) > localMem->bankSize[0]) {
        if ((sizeB + lsizes[i]) > localMem->bankSize[1])
          goto contiguous;    // failed, both banks can't fit array
        sizeB += lsizes[i];
        // set bank1 with original array index
        retAssign |= (1 << reference[i]);
        continue;
      }
      sizeA += lsizes[i];
    }
    banksInfo->banksMode = 2;   // allocation is OK, force split mode in device
    banksInfo->bankAssignments = retAssign;
    banksInfo->bank0total = sizeA;
    banksInfo->bank1total = sizeB;
    return 1;
  }

contiguous:
  // last resort... if banks are contiguous or just a single bank
  if (localMem->banksMode < 2) {
    uint32_t totalSize;
    uint32_t pastSize;
    sizeA = 0;
    retAssign = 0;
    totalSize = localMem->bankSize[0] + localMem->bankSize[1];
    for (i = 0; i < arraysNum; i++) {
      if (!sizes[i])
        continue;
      pastSize = sizeA;
      sizeA += *(sizes + i);
      if (sizeA >= totalSize) {
        return 0;   // BAD
      }
      if (pastSize >= localMem->bankSize[0])
        retAssign |= (1 << i);
    }
    banksInfo->banksMode = localMem->banksMode;
    banksInfo->bankAssignments = retAssign;
    banksInfo->bank0total = sizeA;
    return 1;
  }

  return 0;   // BAD
}

// Convolution kernel cost functions.
// These are not cycles but reflect kernel performance.

static float kernel_model_1x1(const conv_params_t& params)
{
    float result;
    int oD = (params.tile.D + 63) / 64;
    int oW = (params.tile.W + 1) / 2;
    int oH = (params.tile.H + 1) / 2;
    int iD = params.input.D / 4;

    if (params.input.D % 4 == 0)
    {
        result = (((4.56608672 * iD + 100.66974692 + 39) * oW + 28.02605354) * oH + 82.13547351) * oD + 53.6597693;
    }
    else
    {
        result = (((4.58488786 * iD + 118.17603777 + 39) * oW + 33.32729354) * oH + 74.5587789) * oD + 56.5491165;
    }

    // Take into account fixup calculations
    result += (2 * (params.input.D + 2 * IVPN_SIMD_WIDTH - 1) / (2 * IVPN_SIMD_WIDTH) + 35) * params.tile.W * params.tile.H;
    return(result);
}

static float kernel_model_MxN(const conv_params_t& params)
{
    float result;
    int oD  = (params.tile.D + 63) / 64;
    int oW  = (params.tile.W + 1) / 2;
    int oH  = (params.tile.H + 1) / 2;
    int its = (params.input.D * params.kernelW) / 4;

    if ((params.input.D * params.kernelW) % 4 == 0)
    {
        result = ((((4.26073724 * its + 50.45233397) * params.kernelH + 62.05437559) * oW + 7.04195927) * oH + 1.2839801) * oD + 2.11535784;
    }
    else
    {
        result = ((((4.50286598 * its + 67.50409869) * params.kernelH + 74.24374647) * oW + 7.4981325) * oH + 1.32380411) * oD + 2.1639086;
    }

    // Take into account fixup calculations
    result += (((params.input.D * params.kernelW + 2 * IVPN_SIMD_WIDTH - 1) / (2 * IVPN_SIMD_WIDTH) + 9) * params.kernelH + 15) * params.tile.W * params.tile.H;
    return(result);
}

static float kernel_model_qm24(const conv_params_t& params)
{
    int rows        = (params.tile.W * params.tile.H + QM32_COL_PADDING - 1) / QM32_COL_PADDING;
    int channels    = (params.tile.D + 2 * IVPN_SIMD_WIDTH - 1) / (2 * IVPN_SIMD_WIDTH);
    int filter_size = align_up(align_up(params.input.D * params.kernelW, QM32_ROW_PADDING) * params.kernelH, QM32_FILTER_PADDING) / sizeof(int32_t);
    int fixup       = QM32_COL_PADDING * (14 + 3 * params.kernelH + (sizeof(int32_t) * filter_size + 2 * IVPN_SIMD_WIDTH - 1) / (2 * IVPN_SIMD_WIDTH));
    return((float) ((8.2 * (filter_size + 1) / 2 + 61) * channels + fixup + 115) * rows + 125);
}

//Todo: UPDATE VALID MODEL
static float kernel_model_vq_qm24(const conv_params_t& params)
{
    int rows        = (params.tile.W * params.tile.H + QM32_COL_PADDING - 1) / QM32_COL_PADDING;
    int channels    = (params.tile.D + 2 * IVPN_SIMD_WIDTH - 1) / (2 * IVPN_SIMD_WIDTH);
    int filter_size = align_up(align_up(params.input.D * params.kernelW, QM32_ROW_PADDING) * params.kernelH, QM32_FILTER_PADDING) / sizeof(int32_t);
    return((float) ((8.2 * (filter_size + 1) / 2 + 61) * channels + 115) * rows + 125);
}

//TODO: Update calcuation for invQM24
static float kernel_model_inv_qm24(const conv_params_t& params)
{
    int rows        = (params.tile.W * params.tile.H + QM32_COL_PADDING - 1) / QM32_COL_PADDING;
    int channels    = (params.tile.D + 2 * IVPN_SIMD_WIDTH - 1) / (2 * IVPN_SIMD_WIDTH);
    int filter_size = align_up(align_up(params.input.D * params.kernelW, QM32_ROW_PADDING) * params.kernelH, QM32_FILTER_PADDING) / sizeof(int32_t);
    int fixup       = QM32_COL_PADDING * (14 + 3 * params.kernelH + (sizeof(int32_t) * filter_size + 2 * IVPN_SIMD_WIDTH - 1) / (2 * IVPN_SIMD_WIDTH));
    return((float) ((8.2 * (filter_size + 1) / 2 + 61) * channels + fixup + 115) * rows + 125);
}

//TODO: update cacluations for vq invQM24
static float kernel_model_vq_inv_qm24(const conv_params_t& params)
{
    int rows        = (params.tile.W * params.tile.H + QM32_COL_PADDING - 1) / QM32_COL_PADDING;
    int channels    = (params.tile.D + 2 * IVPN_SIMD_WIDTH - 1) / (2 * IVPN_SIMD_WIDTH);
    int filter_size = align_up(align_up(params.input.D * params.kernelW, QM32_ROW_PADDING) * params.kernelH, QM32_FILTER_PADDING) / sizeof(int32_t);
    return((float) ((8.2 * (filter_size + 1) / 2 + 61) * channels + 115) * rows + 125);
}

static float kernel_model_qm32(const conv_params_t& params)
{
    int rows        = (params.tile.W * params.tile.H + QM32_COL_PADDING - 1) / QM32_COL_PADDING;
    int channels    = (params.tile.D + 2 * IVPN_SIMD_WIDTH - 1) / (2 * IVPN_SIMD_WIDTH);
    int filter_size = align_up(align_up(params.input.D * params.kernelW, QM32_ROW_PADDING) * params.kernelH, QM32_FILTER_PADDING) / sizeof(int32_t);
    int level1      = (filter_size + 61) / 62;
    int level2      = (filter_size + 1) / 2;
    int fixup       = QM32_COL_PADDING * (14 + 3 * params.kernelH + (sizeof(int32_t) * filter_size + 2 * IVPN_SIMD_WIDTH - 1) / (2 * IVPN_SIMD_WIDTH));
    return((float) ((8.2 * level2 + 28 * level1 + 123) * channels + fixup + 115) * rows + 125);
}

//TODO: update with valid model
static float kernel_model_vq_qm32(const conv_params_t& params)
{
    int rows        = (params.tile.W * params.tile.H + QM32_COL_PADDING - 1) / QM32_COL_PADDING;
    int channels    = (params.tile.D + 2 * IVPN_SIMD_WIDTH - 1) / (2 * IVPN_SIMD_WIDTH);
    int filter_size = align_up(align_up(params.input.D * params.kernelW, QM32_ROW_PADDING) * params.kernelH, QM32_FILTER_PADDING) / sizeof(int32_t);
    int level1      = (filter_size + 61) / 62;
    int level2      = (filter_size + 1) / 2;
    return((float) ((8.2 * level2 + 28 * level1 + 123) * channels + 115) * rows + 125);
}

static float kernel_model_GS_MOW(const conv_params_t& params)
{
    float result;
    int oD = (params.tile.D + 1) / 2;
    int oW = (params.tile.W + 61) / 62;
    int oH = (params.tile.H + 1) / 2;
    result = (((15 * params.input.D + 185) * oD + 8) * oH + 10) * oW + 100;
    // Take into account fixup calculations
    result += ((4 * params.input.D + 308) * params.tile.H + 110) * ((params.tile.W + 63) / 64);
    return(result);
}

//TODO: update with a valid model
static float kernel_model_VQ_GS_MOW(const conv_params_t& params)
{
    float result;
    int oD = (params.tile.D + 1) / 2;
    int oW = (params.tile.W + 61) / 62;
    int oH = (params.tile.H + 1) / 2;
    result = (((15 * params.input.D + 185) * oD + 8) * oH + 10) * oW + 100;
    return(result);
}

static float kernel_model_GS_MOW1x1(const conv_params_t& params)
{
    float result;
    int oD = (params.tile.D + 1) / 2;
    int oW = (params.tile.W + 63) / 64;
    int oH = (params.tile.H + 1) / 2;
    result = (((4.3 * params.input.D + 234) * oD + 1.9) * oH + 12) * oW + 46;
    // Take into account fixup calculations
    result += ((58 * params.input.D + 1.4) * params.tile.H + 48) * oW;
    return(result);
}

//todo: UPDATE WITH A VALID MODEL
static float kernel_model_VQ_GS_MOW1x1(const conv_params_t& params)
{
    float result;
    int oD = (params.tile.D + 1) / 2;
    int oW = (params.tile.W + 63) / 64;
    int oH = (params.tile.H + 1) / 2;
    result = (((4.3 * params.input.D + 234) * oD + 1.9) * oH + 12) * oW + 46;
    return(result);
}

typedef float (*kernel_model_f)(const conv_params_t& params);

static bool
searchTileDims(conv_params_t& params, conv_kernel_kind_e kernel_kind, int depth_step,
               kernel_model_f model, conv_mem_info_t *mem_info, float *loss = NULL, int32_t coreCount = 1, int32_t batch=1)
{
    // Best tile size and optimization metric
    const float MAX_METRIC = 10e30;

    int H = params.output.H;
    int W = params.output.W;

    // Search for fast implementation
    CONV_FLAG_SET_KERNEL_KIND(params.flags, kernel_kind);
    CONV_FLAG_SET_DOUBLE_BUFF_COEFF(params.flags, 1);

    // See if it fits and skip search if so.
    params.tile.D = params.output.D;
    params.tile.W = params.output.W;
    params.tile.H = params.output.H;

    size_t mem_needed = AllocateLocalMemConv2D(&params, mem_info);
    if (mem_needed)
    {
        if (loss != NULL)
        {
          *loss = model(params);
        }

        return(true);
    }

    int best_W               = 0;
    int best_H               = 0;
    int best_D               = 0;
    int HWBbest_W            = 0;
    int HWBbest_H            = 0;
    int HWBbest_D            = 0;
    int best_double_buff     = 1;
    int HWBbest_double_buff  = 1;

    float best_metric        = MAX_METRIC;
    float HWBbest_metric     = MAX_METRIC; // Two variables to capture which way to go about in tiling if using either reload input or reload output.
    int filter_size = params.input.D * params.kernelW * params.kernelH;

    for (int double_buff = 1; double_buff >= 0; double_buff--)
    {
        CONV_FLAG_SET_DOUBLE_BUFF_COEFF(params.flags, double_buff);
        for (unsigned int D = depth_step; D <= params.output.D + depth_step - 1; D += depth_step)
        {
            params.tile.D = std::min(D, params.output.D);
            // Throw in coefficients transfer cycles, just some estimate
            float coeff_transfer  = (double_buff) ? (256 + (params.tile.D * filter_size + 15) / 16) : 0;
            float tiling_overhead = 1276;
            coeff_transfer *= (params.output.D + params.tile.D - 1) / params.tile.D;
            for (int i = W + H - 1; i > 0; i--)
            {
                if (i >= W)
                {
                  params.tile.W = W;
                  params.tile.H = i - W + 1;
                }
                else
                {
                  params.tile.W = i;
                  params.tile.H = 1;
                }

                mem_needed = AllocateLocalMemConv2D(&params, mem_info);
                if (mem_needed)
                {
                    float metric = mem_info->numTilesD * mem_info->numTilesW
                                   * mem_info->numTilesH * (tiling_overhead + model(params)) + coeff_transfer;
                    if (metric < best_metric)
                    {
                        best_metric      = metric;
                        best_double_buff = double_buff;
                        best_W           = params.tile.W;
                        best_H           = params.tile.H;
                        best_D           = params.tile.D;
                    }

                     if(!((mem_info->numTilesW * mem_info->numTilesH * batch) < (uint32_t) coreCount))
                     {
                        if(metric < HWBbest_metric)
                        {
                            HWBbest_metric = metric;
                            HWBbest_double_buff = double_buff;
                            HWBbest_W = params.tile.W;
                            HWBbest_H = params.tile.H;
                            HWBbest_D = params.tile.D;
                        }
                    }
                }
            }
        }
    }

    if (best_metric < MAX_METRIC)
    {
        CONV_FLAG_SET_DOUBLE_BUFF_COEFF(params.flags, best_double_buff);
        params.tile.W = best_W;
        params.tile.H = best_H;
        params.tile.D = best_D;
        if (loss != NULL)
            *loss = best_metric;

        // restore local memory layout with selected params.tile/double buffering
        mem_needed = AllocateLocalMemConv2D(&params, mem_info);

        int64_t totalSpatialReloads  = (mem_info->numTilesH * mem_info->numTilesW * batch);
        int64_t inputSize = (mem_info->numTilesD - 1) * (totalSpatialReloads) * mem_info->inpTileSize;
        int64_t coeffSize = mem_info->numTilesD * (totalSpatialReloads-1) *  mem_info->coeffTileSize;

        if(inputSize <= coeffSize) // Reloading inputs
        {
                CONV_FLAG_SET_DOUBLE_BUFF_COEFF(params.flags, best_double_buff);
                params.tile.W = best_W;
                params.tile.H = best_H;
                params.tile.D = best_D;
                if (loss != NULL)
                   *loss = best_metric;

                // restore local memory layout with selected params.tile/double buffering
                mem_needed = AllocateLocalMemConv2D(&params, mem_info);
                return true;
        }
        else
        {
	  if (HWBbest_metric < MAX_METRIC) {
		CONV_FLAG_SET_DOUBLE_BUFF_COEFF(params.flags, HWBbest_double_buff);
		params.tile.W = HWBbest_W;
		params.tile.H = HWBbest_H;
		params.tile.D = HWBbest_D;
                if (loss != NULL)
		    *loss = best_metric;

		// restore local memory layout with selected params.tile/double buffering
		mem_needed = AllocateLocalMemConv2D(&params, mem_info);
		return true;
	    }
      	    CONV_FLAG_SET_DOUBLE_BUFF_COEFF(params.flags, best_double_buff);
	    params.tile.W = best_W;
	    params.tile.H = best_H;
	    params.tile.D = best_D;
            if (loss != NULL)
                *loss = best_metric;

	    // restore local memory layout with selected params.tile/double buffering
	    mem_needed = AllocateLocalMemConv2D(&params, mem_info);
	    return(true);

        }
    }

    // Not selected
    return(false);
}

#if _TUNE_PERF_
#include <stdio.h>
#endif

// Estimate returns zero if couldn't find sutable solution.
// params and mem_info updated regardless from outcome.
static float
getQM24Estimate(conv_params_t& params, conv_mem_info_t *mem_info, int32_t coreCount, int32_t batch)
{
    float qm24_loss = 0;
    int depth_step = 1;
    if (searchTileDims(params, kkQM24, depth_step, kernel_model_qm24, mem_info, &qm24_loss, coreCount, batch))
    {
#if _TUNE_PERF_
        printf("BEST QM24: %f\n", qm24_loss);
#endif
    }
    return(qm24_loss);
}

static float
getVQQM24Estimate(conv_params_t& params, conv_mem_info_t *mem_info, int32_t coreCount, int32_t batch)
{
    float vq_qm24_loss = 0;
    int depth_step = 1;
    if (searchTileDims(params, kkVQ_QM24, depth_step, kernel_model_vq_qm24, mem_info, &vq_qm24_loss, coreCount, batch))
    {
#if _TUNE_PERF_
        printf("BEST QM24: %f\n", vq_qm24_loss);
#endif
    }
    return(vq_qm24_loss);
}

static float
getInvQM24Estimate(conv_params_t& params, conv_mem_info_t *mem_info, int32_t coreCount, int32_t batch)
{
    float inv_qm24_loss = 0;
    if (params.kernelW == 3 && params.kernelH == 3
        && (params.stride == 1 || params.stride == 2)
        && (params.input.D == 16 || params.input.D == 32))
    {
        int saved_mode = mem_info->localMem.banksMode;
        mem_info->localMem.banksMode = 2;
        if (searchTileDims(params, kkInvQM24, 1, kernel_model_inv_qm24, mem_info, &inv_qm24_loss, coreCount, batch))
        {
#if _TUNE_PERF_
            printf("BEST INV QM24: %f\n", inv_qm24_loss);
#endif
        }
        mem_info->localMem.banksMode = saved_mode;
    }
    return(inv_qm24_loss);
}

static float
getVQInvQM24Estimate(conv_params_t& params, conv_mem_info_t *mem_info, int32_t coreCount, int32_t batch)
{
    float vq_inv_qm24_loss = 0;
    if (params.kernelW == 3 && params.kernelH == 3
        && (params.stride == 1 || params.stride == 2)
        && (params.input.D == 16 || params.input.D == 32))
    {
        int saved_mode = mem_info->localMem.banksMode;
        mem_info->localMem.banksMode = 2;
        if (searchTileDims(params, kkVQ_InvQM24, 1, kernel_model_vq_inv_qm24, mem_info, &vq_inv_qm24_loss, coreCount, batch))
        {
#if _TUNE_PERF_
            printf("BEST VQ INV QM24: %f\n", vq_inv_qm24_loss);
#endif
        }
        mem_info->localMem.banksMode = saved_mode;
    }
    return(vq_inv_qm24_loss);
}

static float
getCa2Estimate(conv_params_t& params, conv_mem_info_t *mem_info, int32_t coreCount, int32_t batch)
{
    float ca2_loss = 0;
    if (params.kernelW <= 16 && params.kernelH <= 16)
    {
        // Kernel performance model
        kernel_model_f model;
        if (params.kernelW == 1 && params.kernelH == 1)
        {
          model = kernel_model_1x1;
        }
        else
          model = kernel_model_MxN;

        if (searchTileDims(params, kkCa2, 64, model, mem_info, &ca2_loss, coreCount, batch))
        {
#if _TUNE_PERF_
            printf("BEST CA2: %f\n", ca2_loss);
#endif
        }
    }
    return(ca2_loss);
}

static float
getGS_MOWEstimate(conv_params_t& params, conv_mem_info_t *mem_info, int32_t coreCount, int32_t batch)
{
    float loss = 0;
    if (params.kernelW == 3
        && params.kernelH == 3
        && params.stride == 1
        && params.tile.D <= 48)
    {
        int saved_mode = mem_info->localMem.banksMode;
        // Setting to "split banks" mode as gather/scatter is used and it
        // cannot cross DRAM boundary.
        mem_info->localMem.banksMode = 2;
        if (searchTileDims(params, kkGS_MOW, 1, kernel_model_GS_MOW, mem_info, &loss, coreCount, batch))
        {
#if _TUNE_PERF_
            printf("BEST GS_MOW: %f\n", loss);
#endif
        }
        mem_info->localMem.banksMode = saved_mode;
    }
    return(loss);
}

static float
getVQ_GS_MOWEstimate(conv_params_t& params, conv_mem_info_t *mem_info, int32_t coreCount, int32_t batch)
{
    float loss = 0;
    if (params.kernelW == 3
        && params.kernelH == 3
        && params.stride == 1
        && params.tile.D <= 48)
    {
        int saved_mode = mem_info->localMem.banksMode;
        // Setting to "split banks" mode as gather/scatter is used and it
        // cannot cross DRAM boundary.
        mem_info->localMem.banksMode = 2;
        if (searchTileDims(params, kkVQ_GS_MOW, 1, kernel_model_VQ_GS_MOW, mem_info, &loss, coreCount, batch))
        {
#if _TUNE_PERF_
            printf("BEST VQ_GS_MOW: %f\n", loss);
#endif
        }
        mem_info->localMem.banksMode = saved_mode;
    }
    return(loss);
}

static float
getGS_MOWS2Estimate(conv_params_t& params, conv_mem_info_t *mem_info, int32_t coreCount, int32_t batch)
{
    float loss = 0;
    if (params.kernelW == 3
        && params.kernelH == 3
        && params.stride == 2
        && params.input.D == 3)
    {
        int saved_mode = mem_info->localMem.banksMode;
        // Setting to "split banks" mode as gather/scatter is used and it
        // cannot cross DRAM boundary.
        mem_info->localMem.banksMode = 2;
        if (searchTileDims(params, kkGS_MOW_S2, 1, kernel_model_GS_MOW, mem_info, &loss, coreCount, batch))
        {
#if _TUNE_PERF_
#endif
        }
        mem_info->localMem.banksMode = saved_mode;
    }
    else if (params.kernelW == 7
             && params.kernelH == 7
             && params.stride == 2
             && params.output.D >= 64
             && params.input.D == 3)
    {
        int saved_mode = mem_info->localMem.banksMode;
        // Setting to "split banks" mode as gather/scatter is used and it
        // cannot cross DRAM boundary.
        mem_info->localMem.banksMode = 2;
        if (searchTileDims(params, kkGS_MOW_S2, 1, kernel_model_GS_MOW, mem_info, &loss, coreCount, batch))
        {
#if _TUNE_PERF_
#endif
        }
        mem_info->localMem.banksMode = saved_mode;
    }
    return(loss);
}

static float
getVQ_GS_MOWS2Estimate(conv_params_t& params, conv_mem_info_t *mem_info, int32_t coreCount, int32_t batch)
{
    float loss = 0;
    if (params.kernelW == 3
        && params.kernelH == 3
        && params.stride == 2
        && params.input.D == 3)
    {
        int saved_mode = mem_info->localMem.banksMode;
        // Setting to "split banks" mode as gather/scatter is used and it
        // cannot cross DRAM boundary.
        mem_info->localMem.banksMode = 2;
        if (searchTileDims(params, kkVQ_GS_MOW_S2, 1, kernel_model_VQ_GS_MOW, mem_info, &loss, coreCount, batch))
        {
#if _TUNE_PERF_
#endif
        }
        mem_info->localMem.banksMode = saved_mode;
    }
    else if (params.kernelW == 7
             && params.kernelH == 7
             && params.stride == 2
             && params.output.D >= 64
             && params.input.D == 3)
    {
        int saved_mode = mem_info->localMem.banksMode;
        // Setting to "split banks" mode as gather/scatter is used and it
        // cannot cross DRAM boundary.
        mem_info->localMem.banksMode = 2;
        if (searchTileDims(params, kkVQ_GS_MOW_S2, 1, kernel_model_VQ_GS_MOW, mem_info, &loss, coreCount, batch))
        {
#if _TUNE_PERF_
#endif
        }
        mem_info->localMem.banksMode = saved_mode;
    }
    return(loss);
}

static float
getGS_MOW1x1Estimate(conv_params_t& params, conv_mem_info_t *mem_info, int32_t coreCount, int32_t batch)
{
    float loss = 0;
    if (params.kernelW == 1
        && params.kernelH == 1
        && params.stride == 1
        && params.tile.D < 32
        && params.input.D < 32)
    {
        int saved_mode = mem_info->localMem.banksMode;
        // Setting to "split banks" mode as gather/scatter is used and it
        // cannot cross DRAM boundary.
        mem_info->localMem.banksMode = 2;
        if (searchTileDims(params, kkGS_MOW1x1, 1, kernel_model_GS_MOW1x1, mem_info, &loss, coreCount, batch))
        {
#if _TUNE_PERF_
            printf("BEST GS_MOW1x1: %f\n", loss);
#endif
        }
        mem_info->localMem.banksMode = saved_mode;
    }
    return(loss);
}

static float
getVQ_GS_MOW1x1Estimate(conv_params_t& params, conv_mem_info_t *mem_info, int32_t coreCount, int32_t batch)
{
    float loss = 0;
    if (params.kernelW == 1
        && params.kernelH == 1
        && params.stride == 1
        && params.tile.D < 32
        && params.input.D < 32)
    {
        int saved_mode = mem_info->localMem.banksMode;
        // Setting to "split banks" mode as gather/scatter is used and it
        // cannot cross DRAM boundary.
        mem_info->localMem.banksMode = 2;
        if (searchTileDims(params, kkVQ_GS_MOW1x1, 1, kernel_model_VQ_GS_MOW1x1, mem_info, &loss, coreCount, batch))
        {
#if _TUNE_PERF_
            printf("BEST VQ_GS_MOW1x1: %f\n", loss);
#endif
        }
        mem_info->localMem.banksMode = saved_mode;
    }
    return(loss);
}

bool
SetupConv2D(conv_params_t& params, bool allow_acc24, bool channel_qflag, conv_kernel_kind_e force_kernel, conv_mem_info_t *mem_info, int32_t coreCount, int32_t batch)
{
	// NEED REFACTORING
    if (allow_acc24 && (!channel_qflag))
    {
        conv_params_t best_params     = params;
        conv_mem_info_t best_mem_info = *mem_info;
        float best_loss               = 0;
        if (force_kernel == kkNone || force_kernel == kkQM24)
        {
            float qm24_loss = getQM24Estimate(params, mem_info, coreCount, batch);
            if (best_loss == 0 || (qm24_loss > 0 && qm24_loss < best_loss))
            {
              best_loss     = qm24_loss;
              best_params   = params;
              best_mem_info = *mem_info;
            }
        }
        if (IVPN_SIMD_WIDTH != 64) /* Temporarily disabling for Q8 as the implementation is not yet available*/
        {
            // Special case of QM24 for porformance improvement
            if (force_kernel == kkNone || force_kernel == kkInvQM24)
            {
                float inv_qm24_loss = getInvQM24Estimate(params, mem_info, coreCount, batch);
                if (best_loss == 0 || (inv_qm24_loss > 0 && inv_qm24_loss < best_loss))
                {
                    best_loss = inv_qm24_loss;
                    best_params = params;
                    best_mem_info = *mem_info;
                }
            }
        }
        if (force_kernel == kkNone || force_kernel == kkCa2)
        {
            float ca2_loss = getCa2Estimate(params, mem_info, coreCount, batch);
            if (best_loss == 0 || (ca2_loss > 0 && ca2_loss < best_loss))
            {
              best_loss     = ca2_loss;
              best_params   = params;
              best_mem_info = *mem_info;
            }
        }

        if (force_kernel == kkNone || force_kernel == kkGS_MOW)
        {
            float mow_loss = getGS_MOWEstimate(params, mem_info, coreCount, batch);
            if (best_loss == 0 || (mow_loss > 0 && mow_loss < best_loss))
            {
              best_loss     = mow_loss;
              best_params   = params;
              best_mem_info = *mem_info;
            }
        }

        if (force_kernel == kkNone || force_kernel == kkGS_MOW1x1)
        {
            float mow_loss1x1 = getGS_MOW1x1Estimate(params, mem_info, coreCount, batch);
            if (best_loss == 0 || (mow_loss1x1 > 0 && mow_loss1x1 < best_loss))
            {
              best_loss     = mow_loss1x1;
              best_params   = params;
              best_mem_info = *mem_info;
            }
        }

        if (force_kernel == kkNone || force_kernel == kkGS_MOW_S2)
        {
            float mow_lossS2 = getGS_MOWS2Estimate(params, mem_info, coreCount, batch);
            if (best_loss == 0 || (mow_lossS2 > 0 && mow_lossS2 < best_loss))
            {
              best_loss     = mow_lossS2;
              best_params   = params;
              best_mem_info = *mem_info;
            }
        }

        if (best_loss > 0)
        {
            params    = best_params;
            *mem_info = best_mem_info;

            return(true);
        }
    }
    else if (allow_acc24 && channel_qflag)
    {
        conv_params_t best_params     = params;
        conv_mem_info_t best_mem_info = *mem_info;
        float best_loss               = 0;
        if (force_kernel == kkNone || force_kernel == kkVQ_QM24)
        {
            float vq_qm24_loss = getVQQM24Estimate(params, mem_info, coreCount, batch);
            if (best_loss == 0 || (vq_qm24_loss > 0 && vq_qm24_loss < best_loss))
            {
              best_loss     = vq_qm24_loss;
              best_params   = params;
              best_mem_info = *mem_info;
            }
        }

        if (IVPN_SIMD_WIDTH != 64) /* Temporarily disabling for Q8 as the implementation is not yet available*/
        {
            // Special case of QM24 for performance improvement
            if (force_kernel == kkNone || force_kernel == kkVQ_InvQM24)
            {
                float vq_inv_qm24_loss = getVQInvQM24Estimate(params, mem_info, coreCount, batch);
                if (best_loss == 0 || (vq_inv_qm24_loss > 0 && vq_inv_qm24_loss < best_loss))
                {
                  best_loss     = vq_inv_qm24_loss;
                  best_params   = params;
                  best_mem_info = *mem_info;
                }
            }
        }
        if (force_kernel == kkNone || force_kernel == kkVQ_GS_MOW)
        {
            float vq_mow_loss = getVQ_GS_MOWEstimate(params, mem_info, coreCount, batch);
            if (best_loss == 0 || (vq_mow_loss > 0 && vq_mow_loss < best_loss))
            {
              best_loss     = vq_mow_loss;
              best_params   = params;
              best_mem_info = *mem_info;
            }
        }

        if (force_kernel == kkNone || force_kernel == kkVQ_GS_MOW1x1)
        {
            float vq_mow_loss1x1 = getVQ_GS_MOW1x1Estimate(params, mem_info, coreCount, batch);
            if (best_loss == 0 || (vq_mow_loss1x1 > 0 && vq_mow_loss1x1 < best_loss))
            {
              best_loss     = vq_mow_loss1x1;
              best_params   = params;
              best_mem_info = *mem_info;
            }
        }

        if (force_kernel == kkNone || force_kernel == kkVQ_GS_MOW_S2)
        {
            float vq_mow_lossS2 = getVQ_GS_MOWS2Estimate(params, mem_info, coreCount, batch);
            if (best_loss == 0 || (vq_mow_lossS2 > 0 && vq_mow_lossS2 < best_loss))
            {
              best_loss     = vq_mow_lossS2;
              best_params   = params;
              best_mem_info = *mem_info;
            }
        }

        if (best_loss > 0)
        {
            params    = best_params;
            *mem_info = best_mem_info;

            return(true);
        }
    }

    if (channel_qflag)
    {
        if (force_kernel == kkNone || force_kernel == kkVQ_QM32)
        {
            float vq_qm32_loss = 0;
            int depth_step = 1;
            if (searchTileDims(params, kkVQ_QM32, depth_step, kernel_model_vq_qm32, mem_info, &vq_qm32_loss, coreCount, batch))
            {
#if _TUNE_PERF_
                printf("BEST VQ QM32: %f\n", vq_qm32_loss);
#endif
                return(true);
            }
            // Failed kkVQ_32 try kkVQ_FC
            force_kernel = kkVQ_FC;
        }
    }
    else
    {
        if (force_kernel == kkNone || force_kernel == kkQM32)
        {
            float qm32_loss = 0;
            if (searchTileDims(params, kkQM32, 1, kernel_model_qm32, mem_info, &qm32_loss, coreCount, batch))
            {
#if _TUNE_PERF_
                printf("BEST QM32: %f\n", qm32_loss);
#endif
              return(true);
            }
        }
    }

    if (channel_qflag)
    {
        if (force_kernel == kkNone || force_kernel == kkVQ_FC)
        {
            // No luck, try fully connected layer
            params.tile.W = 1;
            params.tile.H = 1;

            if (CONV_FLAG_GET_FP16(params.flags))
            {
                CONV_FLAG_SET_KERNEL_KIND(params.flags, kkVQ_FC_FP16);
            }
            else
            {
                CONV_FLAG_SET_KERNEL_KIND(params.flags, kkVQ_FC);
            }

            // Allow double buffering of coefficients
            CONV_FLAG_SET_DOUBLE_BUFF_COEFF(params.flags, 1);

            for (int D = params.output.D; D > 0; D--)
            {
                params.tile.D = D;
                size_t mem_needed = AllocateLocalMemConv2D(&params, mem_info);
                if (mem_needed)
                    return(true);
            }
        }
    }
    else
    {
        if (force_kernel == kkNone || force_kernel == kkFC)
        {
            // No luck, try fully connected layer
            params.tile.W = 1;
            params.tile.H = 1;

            if (CONV_FLAG_GET_FP16(params.flags))
            {
                CONV_FLAG_SET_KERNEL_KIND(params.flags, kkFC_FP16);
            }
            else
            {
                CONV_FLAG_SET_KERNEL_KIND(params.flags, kkFC);
            }

            // Allow double buffering of coefficients
            CONV_FLAG_SET_DOUBLE_BUFF_COEFF(params.flags, 1);

            for (int D = params.output.D; D > 0; D--)
            {
                params.tile.D = D;
                size_t mem_needed = AllocateLocalMemConv2D(&params, mem_info);
                if (mem_needed)
                {
                  return(true);
                }
            }
        }
    }

    return(false);
}

bool
SetupDepthwise(conv_params_t& params, conv_kernel_kind_e force_kernel, conv_mem_info_t *mem_info, int32_t coreCount)
{
    int H = params.output.H;
    int W = params.output.W;
    int depthPerCore;
    CONV_FLAG_SET_KERNEL_KIND(params.flags, force_kernel);

    {
        if (params.kernelW <= 16 && params.kernelH <= 16)
        {
            depthPerCore = (params.output.D / coreCount) ? (params.output.D / coreCount) : params.output.D;
            for (unsigned int D = align_up(depthPerCore, 32); D > 0; D -= 32)
            {
                params.tile.D = std::min(D, params.output.D);
                for (int i = W + H - 1; i > 0; i--)
                {
                    if (i >= W)
                    {
                        params.tile.W = W;
                        params.tile.H = i - W + 1;
                    }
                    else
                    {
                        params.tile.W = i;
                        params.tile.H = 1;
                    }

                    size_t mem_needed = AllocateLocalMemDepthwise(&params, mem_info);
                    if (mem_needed)
                        return(true);
                }
            }
        }
    }
    return(false);
}

bool MayOverflowAccumulator24(const uint8_t *filters_data,
                              size_t filter_size,
                              size_t count)
{
    bool overflow_possible = false;

    for (size_t filter = 0; filter < count; filter++)
    {
        uint64_t sum = 0;
        for (size_t c = 0; c < filter_size; c++)
        {
            int s8 = filters_data[filter * filter_size + c] - 128;
            sum += (s8 < 0) ?  (-128 * s8) : (128 * s8);
        }
        if (sum > 0x7FFFFF)
        {
            overflow_possible = true;
            break;
        }
    }
    return(overflow_possible);
}

bool
ConvReorderCoefficients(const uint8_t *coeff_ref, const int32_t* bias_ref, uint8_t *coeff, int32_t* bias, const conv_params_t& params)
{
    int8_t *coeff_ref_s8 = (int8_t *) coeff_ref;
    int32_t idx, coeff_val;

    int offset      = (params.coeffType.dataType == XI_TILE4D_S8) ? 0 : 128;
    int tiles_count = (params.output.D + params.tile.D - 1) / params.tile.D;

    switch (CONV_FLAG_GET_KERNEL_KIND(params.flags))
    {
    case kkCa2:
      {
        int8_t *coeff_s8 = (int8_t *) coeff;

        for (int tile = 0; tile < tiles_count; tile++)
        {
            int tile_depth   = std::min(params.tile.D, params.output.D - tile * params.tile.D);
            int tile_depth64 = align_up(tile_depth, ALIGNMENT);
            for (unsigned int h = 0; h < params.kernelH; h++)
            {
                for (unsigned int w = 0; w < params.kernelW; w++)
                {
                    for (unsigned int ch = 0; ch < params.input.D; ch++)
                    {
                        int d = 0;

                        for (; d < tile_depth; d++)
                        {
                          *coeff_s8++ = (int8_t) (coeff_ref[(((tile * params.tile.D + d) * params.kernelH + h) * params.kernelW + w) * params.input.D + ch] - offset);
                        }

                        for (; d < tile_depth64; d++)
                        {
                          *coeff_s8++ = 0;
                        }
                    }
                }
            }
            // Compute fixup and copy bias in one pass
            unsigned int kernel_volume = params.kernelH * params.kernelW * params.input.D;
            int const_fixup            = (params.zeroPtInput * params.zeroPtCoeff - offset * offset) * kernel_volume;
            for (int d = 0; d < tile_depth; d++)
            {
                int32_t fixup = 0;
                for (unsigned int i = 0; i < kernel_volume; i++)
                {
                    idx       = (tile * params.tile.D + d) * kernel_volume + i;
                    coeff_val = (offset == 0) ? coeff_ref_s8[idx] : coeff_ref[idx];
                    fixup    += coeff_val;        //coeff_ref[(tile * params.tile.D + d) * kernel_volume + i];
                }
                *bias++ = *bias_ref++ + (128 - params.zeroPtInput) * fixup + const_fixup;
            }
        }

        break;
      }

    case kkVQ7Opt:
      {
        unsigned int kIn;
        int8_t *coeff_s8       = (int8_t *) coeff;
        size_t src_filter_size = params.kernelH * params.kernelW * params.input.D;
        size_t numInChAligned  = align_up(params.input.D, 16);
        uint8_t *pInput;
        for (int tile = 0; tile < tiles_count; tile++)
        {
            uint8_t *coeff_ref_Dout = (uint8_t *) coeff_ref + (params.input.D * params.kernelW * params.kernelH * tile * params.tile.D);
            unsigned int tile_depth = std::min(params.tile.D, params.output.D - tile * params.tile.D);
            for (unsigned int outCh = 0; outCh < tile_depth; outCh += 16)
            {
                size_t remOutCh = std::min((unsigned int) 16, tile_depth - outCh);
                for (unsigned int y = 0; y < params.kernelH; y++)
                {
                    for (unsigned int x = 0; x < params.kernelW; x++)
                    {
                        pInput = (uint8_t *) coeff_ref_Dout + (outCh * (params.input.D * params.kernelW * params.kernelH)) + (y * (params.input.D * params.kernelW)) + (x * params.input.D);
                        for (unsigned int inCh = 0; inCh < numInChAligned; inCh += 4)
                        {
                            for (kIn = 0; kIn < remOutCh; kIn++)
                            {
                              *coeff_s8++ = (int8_t) ((int32_t) pInput[inCh + kIn * (params.input.D * params.kernelW * params.kernelH)] - offset);
                              *coeff_s8++ = ((inCh + 1) < params.input.D) ? (int8_t) ((int32_t) pInput[(inCh + 1) + kIn * (params.input.D * params.kernelW * params.kernelH) ] - offset) : 0;
                              *coeff_s8++ = ((inCh + 2) < params.input.D) ? (int8_t) ((int32_t) pInput[(inCh + 2) + kIn * (params.input.D * params.kernelW * params.kernelH) ] - offset) : 0;
                              *coeff_s8++ = ((inCh + 3) < params.input.D) ? (int8_t) ((int32_t) pInput[(inCh + 3) + kIn * (params.input.D * params.kernelW * params.kernelH) ] - offset) : 0;
                            }

                            for (; kIn < 16; kIn++)
                            {
                              *coeff_s8++ = 0;
                              *coeff_s8++ = 0;
                              *coeff_s8++ = 0;
                              *coeff_s8++ = 0;
                            }
                        }
                    }
                }
            }
            int const_fixup = (params.zeroPtInput * params.zeroPtCoeff - 0 * 0) * src_filter_size;
            for (unsigned int d = 0; d < tile_depth; d++)
            {
                int32_t fixup = 0;
                for (unsigned int i = 0; i < src_filter_size; i++)
                {
                    idx       = (tile * params.tile.D + d) * src_filter_size + i;
                    coeff_val = (offset == 0) ? coeff_ref_s8[idx] : coeff_ref[idx];
                    fixup    += coeff_val;        //coeff_ref[(tile * params.tile.D + d) * src_filter_size + i];
                }
                *bias++ = (*bias_ref++) + (0 - params.zeroPtInput) * fixup + const_fixup;
            }
        }

        break;
      }

    case kkQM24:
    case kkQM32:
      {

        {
            int8_t *coeff_s8           = (int8_t *) coeff;
            size_t src_filter_size     = params.kernelH * params.kernelW * params.input.D;
            size_t dst_filter_row_size = align_up(params.kernelW * params.input.D, QM32_ROW_PADDING);
            size_t dst_filter_size     = align_up(params.kernelH * dst_filter_row_size, QM32_FILTER_PADDING);
            size_t pad_h               = dst_filter_row_size - params.kernelW * params.input.D;
            size_t pad_filter          = dst_filter_size - params.kernelH * dst_filter_row_size;
            //assert(QM32_ROW_PADDING >= 4, "Assuming that row size is at least multiple of 4");
            for (int tile = 0; tile < tiles_count; tile++)
            {
                unsigned int tile_depth = std::min(params.tile.D, params.output.D - tile * params.tile.D);
                for (unsigned int band = 0; band < tile_depth; band += 2 * IVPN_SIMD_WIDTH)
                {
                    size_t band_size = std::min((unsigned int) 2 * IVPN_SIMD_WIDTH, tile_depth - band);
                    // Split even and odd filter dwords into two contiguous streams.
                    for (size_t half = 0; half < 2; half++)
                    {
                        for (unsigned int h = 0; h < params.kernelH; h++)
                        {
                            for (unsigned int w = 0; w < params.kernelW; w++)
                            {
                                for (unsigned int ch = 0; ch < params.input.D; ch++)
                                {
                                    size_t src_start = h * dst_filter_row_size + w * params.input.D + ch;
                                    if ((src_start / 2) % 2 == half)
                                    {
                                        for (unsigned int d = 0; d < band_size; d++)
                                        {
                                          size_t elt = tile * params.tile.D + band + d;
                                          elt         = elt * params.kernelH + h;
                                          elt         = elt * params.kernelW + w;
                                          elt         = elt * params.input.D + ch;
                                          *coeff_s8++ = (int8_t) (coeff_ref[elt] - offset);
                                        }
                                    }
                                }
                            }
                            // Zero pad tail of each row
                            for (unsigned int i = 0; i < pad_h; i++)
                            {
                                size_t src_start = h * dst_filter_row_size + params.kernelW * params.input.D + i;
                                if ((src_start / 2) % 2 == half)
                                {
                                    for (unsigned int d = 0; d < band_size; d++)
                                    {
                                      *coeff_s8++ = 0;
                                    }
                                }
                            }
                        }
                        // Zero pad filter to reflect unrolling by two in the kernel
                        for (unsigned int i = 0; i < pad_filter / 2; i++)
                        {
                            for (unsigned int d = 0; d < band_size; d++)
                            {
                              *coeff_s8++ = 0;
                            }
                        }
                    }
                }
                // Compute fixup and copiy bias in one pass
                int const_fixup = (params.zeroPtInput * params.zeroPtCoeff - offset * offset) * src_filter_size;
                for (unsigned int d = 0; d < tile_depth; d++)
                {
                    int32_t fixup = 0;
                    for (unsigned int i = 0; i < src_filter_size; i++)
                    {
                      idx       = (tile * params.tile.D + d) * src_filter_size + i;
                      coeff_val = (offset == 0) ? coeff_ref_s8[idx] : coeff_ref[idx];
                      fixup    += coeff_val;      //coeff_ref[(tile * params.tile.D + d) * src_filter_size + i];
                    }
                    *bias++ = *bias_ref++ + (128 - params.zeroPtInput) * fixup + const_fixup;
                }
            }
        }

        break;
      }

    case kkVQ_QM24:
    case kkVQ_QM32:
      {
        {
        int8_t *coeff_s8 = (int8_t *) coeff;
        for (int tile = 0; tile < tiles_count; tile++)
        {
            unsigned int tile_depth   = std::min(params.tile.D, params.output.D - tile * params.tile.D);
            unsigned int tile_depth64 = align_up(tile_depth, ALIGNMENT);

            //for (unsigned int n = 0; n < tile_depth; n++) {
            for (unsigned int h = 0; h < params.kernelH; h++)
            {
                for (unsigned int w = 0; w < params.kernelW; w++)
                {
                    for (unsigned int ch = 0; ch < params.input.D; ch++)
                    {
                        unsigned int n = 0;
                        for (; n < tile_depth; n++)
                        {
                          idx = (((tile * params.tile.D + n) * params.kernelH + h) * params.kernelW + w) * params.input.D + ch;
                          *coeff_s8++ = (int8_t) coeff_ref[idx];
                        }

                        for (; n < tile_depth64; n++)
                        {
                          *coeff_s8++ = 0;
                        }
                    }
                }
            }

            // copy bias in one pass
            uint32_t src_filter_size = params.kernelH * params.kernelW * params.input.D;
            for (unsigned int d = 0; d < tile_depth; d++)
            {
                int fixup = 0;
                for (unsigned i = 0; i < src_filter_size; i++)
                {
                  fixup += (int8_t) coeff_ref[(tile * params.tile.D + d) * src_filter_size + i];
                }
                *bias++ = *bias_ref++ - (params.zeroPtInput * fixup);
            }
            }
        }

        break;
      }

    case kkInvQM24:
    case kkVQ_InvQM24:
      {
        int8_t *coeff_s8       = (int8_t *) coeff;
        size_t src_filter_size = params.kernelH * params.kernelW * params.input.D;
        for (int tile = 0; tile < tiles_count; tile++)
        {
            unsigned int tile_depth = std::min(params.tile.D, params.output.D - tile * params.tile.D);
            for (unsigned int d = 0; d < tile_depth; d++)
            {
                for (unsigned int h = 0; h < params.kernelH; h++)
                {
                    for (unsigned int w = 0; w < params.kernelW; w++)
                    {
                        for (unsigned int ch = 0; ch < params.input.D; ch++)
                        {
                          *coeff_s8++ = (int8_t) (coeff_ref[(((tile * params.tile.D + d) * params.kernelH + h) * params.kernelW + w) * params.input.D + ch] - offset);
                        }
                    }
                }
            }
            // Compute fixup and copy bias in one pass
            int const_fixup = params.zeroPtInput * params.zeroPtCoeff * src_filter_size;
            for (unsigned int d = 0; d < tile_depth; d++)
            {
                int32_t fixup = 0;
                for (unsigned int i = 0; i < src_filter_size; i++)
                {
                    idx       = (tile * params.tile.D + d) * src_filter_size + i;
                    coeff_val = (offset == 0) ? coeff_ref_s8[idx] : coeff_ref[idx];
                    fixup    += coeff_val;        //coeff_ref[(tile * params.tile.D + d) * src_filter_size + i];
                }
                *bias++ = *bias_ref++ - params.zeroPtInput * fixup + const_fixup;
            }
        }

        break;
      }

    case kkGS_MOW:
    case kkVQ_GS_MOW:
      {
        int8_t *coeff_s8 = (int8_t *) coeff;

        for (int tile = 0; tile < tiles_count; tile++)
        {
            // Reorder tile to WHDN format (width fastest dimension)
            unsigned int tile_depth = std::min(params.tile.D, params.output.D - tile * params.tile.D);
            for (unsigned int d = 0; d < tile_depth; d++)
            {
                for (unsigned int ch = 0; ch < params.input.D; ch++)
                {
                    for (unsigned int h = 0; h < params.kernelH; h++)
                    {
                        for (unsigned int w = 0; w < params.kernelW; w++)
                        {
                          *coeff_s8++ = (int8_t) (coeff_ref[(((tile * params.tile.D + d) * params.kernelH + h) * params.kernelW + w) * params.input.D + ch] - offset);
                        }
                    }
                }
            }
            // Compute fixup and copy bias in one pass
            unsigned int kernel_volume = params.kernelH * params.kernelW * params.input.D;
            int const_fixup            = params.zeroPtInput * params.zeroPtCoeff * kernel_volume;
            for (unsigned int d = 0; d < tile_depth; d++)
            {
                int32_t fixup = 0;
                for (unsigned int i = 0; i < kernel_volume; i++)
                {
                    idx       = (tile * params.tile.D + d) * kernel_volume + i;
                    coeff_val = (offset == 0) ? coeff_ref_s8[idx] : coeff_ref[idx];
                    fixup    += coeff_val;        //coeff_ref[(tile * params.tile.D + d) * kernel_volume + i];
                }
                *bias++ = *bias_ref++ - params.zeroPtInput * fixup + const_fixup;
            }
        }
        break;
      }

    case kkGS_MOW1x1:
    case kkVQ_GS_MOW1x1:
      {
        int8_t *coeff_s8 = (int8_t *) coeff;

        for (int tile = 0; tile < tiles_count; tile++)
        {
            // Reorder tile to WHDN format (width fastest dimension)
            unsigned int tile_depth = std::min(params.tile.D, params.output.D - tile * params.tile.D);
            for (unsigned int d = 0; d < tile_depth; d++)
            {
                for (unsigned int ch = 0; ch < params.input.D; ch++)
                {
                    for (unsigned int h = 0; h < params.kernelH; h++)
                    {
                        for (unsigned int w = 0; w < params.kernelW; w++)
                        {
                          *coeff_s8++ = (int8_t) (coeff_ref[(((tile * params.tile.D + d) * params.kernelH + h) * params.kernelW + w) * params.input.D + ch] - offset);
                        }
                    }
                }
            }
            // Compute fixup and copy bias in one pass
            unsigned int kernel_volume = params.kernelH * params.kernelW * params.input.D;
            int const_fixup            = params.zeroPtInput * params.zeroPtCoeff * kernel_volume;
            for (unsigned int d = 0; d < tile_depth; d++)
            {
                int32_t fixup = 0;
                for (unsigned int i = 0; i < kernel_volume; i++)
                {
                    idx       = (tile * params.tile.D + d) * kernel_volume + i;
                    coeff_val = (offset == 0) ? coeff_ref_s8[idx] : coeff_ref[idx];
                    fixup    += coeff_val;        //coeff_ref[(tile * params.tile.D + d) * kernel_volume + i];
                }
                *bias++ = *bias_ref++ - params.zeroPtInput * fixup + const_fixup;
            }
        }

        break;
      }

    case kkGS_MOW_S2:
    case kkVQ_GS_MOW_S2:
      {
        int8_t *coeff_s8 = (int8_t *) coeff;

        for (int tile = 0; tile < tiles_count; tile++)
        {
            // Reorder tile to WHDN format (width fastest dimension)
            unsigned int tile_depth = std::min(params.tile.D, params.output.D - tile * params.tile.D);
            for (unsigned int d = 0; d < tile_depth; d++)
            {
                for (unsigned int ch = 0; ch < params.input.D; ch++)
                {
                    for (unsigned int h = 0; h < params.kernelH; h++)
                    {
                        for (unsigned int w = 0; w < params.kernelW; w++)
                        {
                          *coeff_s8++ = (int8_t) (coeff_ref[(((tile * params.tile.D + d) * params.kernelH + h) * params.kernelW + w) * params.input.D + ch] - offset);
                        }
                    }
                }
            }
            // Compute fixup and copy bias in one pass
            unsigned int kernel_volume = params.kernelH * params.kernelW * params.input.D;
            int const_fixup            = params.zeroPtInput * params.zeroPtCoeff * kernel_volume;
            for (unsigned int d = 0; d < tile_depth; d++)
            {
                int32_t fixup = 0;
                for (unsigned int i = 0; i < kernel_volume; i++)
                {
                  idx       = (tile * params.tile.D + d) * kernel_volume + i;
                  coeff_val = (offset == 0) ? coeff_ref_s8[idx] : coeff_ref[idx];
                  fixup    += coeff_val;        //coeff_ref[(tile * params.tile.D + d) * kernel_volume + i];
                }
                *bias++ = *bias_ref++ - params.zeroPtInput * fixup + const_fixup;
            }
        }
        break;
      }

    case kkFC:
    case kkVQ_FC:
      {
        memcpy(coeff, coeff_ref, params.output.D * params.kernelH * params.kernelW * params.input.D);
        if (bias_ref)
          memcpy(bias, bias_ref, params.output.D * sizeof(int32_t));
        else
          memset(bias, 0, params.output.D * sizeof(int32_t));

        break;
      }

    case kkFC_FP16:
      {
        memcpy(coeff, coeff_ref, params.output.D * params.kernelH * params.kernelW * params.input.D * 2); // 2 = sizeof (F16)
        memcpy(bias, bias_ref, params.output.D * 2);                                                      // 2 = sizeof (F16)

        break;
      }

    case kkDepthwise:
      {   // need to convert coeff to U8 for XI-lib kernel
        int outputD;
        int tileD;
        outputD = params.output.D;
        tileD   = params.tile.D;
        tiles_count = (outputD + tileD - 1) / tileD;
        offset      = (params.coeffType.dataType == XI_TILE4D_S8) ? 128 : 0;
        memcpy(bias, bias_ref, outputD * sizeof(int32_t));

        for (int tile = 0; tile < tiles_count; tile++)
        {
            int tile_depth   = std::min(tileD, outputD - tile * tileD);
            int tile_depth64 = align_up(tile_depth, ALIGNMENT);
            for (unsigned int h = 0; h < params.kernelH; h++)
            {
                for (unsigned int w = 0; w < params.kernelW; w++)
                {
                    int d = 0;
                    for (; d < tile_depth; d++)
                    {
                      *coeff++ = coeff_ref[(h * params.kernelW + w) * outputD + tile * tileD + d] + offset;
                    }

                    for (; d < tile_depth64; d++)
                    {
                      *coeff++ = 0;
                    }
                }
            }
        }
        break;
      }

    case kkVQ_Depthwise:
      {   // need to convert coeff to S8 for XI-lib kernel
        offset = (params.coeffType.dataType == XI_TILE4D_S8) ? 0 : 128;
        memcpy(bias, bias_ref, params.output.D * sizeof(int32_t));
        for (int tile = 0; tile < tiles_count; tile++)
        {
            int tile_depth   = std::min(params.tile.D, params.output.D - tile * params.tile.D);
            int tile_depth64 = align_up(tile_depth, ALIGNMENT);
            for (unsigned int h = 0; h < params.kernelH; h++)
            {
                for (unsigned int w = 0; w < params.kernelW; w++)
                {
                    int d = 0;
                    for (; d < tile_depth; d++)
                    {
                      *coeff++ = coeff_ref[(h * params.kernelW + w) * params.output.D + tile * params.tile.D + d] + offset;
                    }

                    for (; d < tile_depth64; d++)
                    {
                      *coeff++ = 0;
                    }
                }
            }
        }
        break;
      }

    default:
      return(false);
    }

    return(true);
}
