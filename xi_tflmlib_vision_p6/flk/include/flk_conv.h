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
#ifndef _FLK_CONV_H_INCLUDED_
#define _FLK_CONV_H_INCLUDED_

#include "flk_common.h"
#include "xi_tile_manager.h"
#include "xi_tile3d_manager.h"

#include <stdio.h>

typedef enum
{
  FALSE = 0,
  TRUE  = 1
} BOOL_T;

typedef enum
{
  kkNone,
  /* DWH Kernel with aligned coefficients in NDWH format */
  kkCa2,
  /* DWH with 24 bit accumulators and quad multiplies. */
  kkQM24,
  /* DWH with 32 bit accumulators and quad multiplies. */
  kkQM32,
  /* Fully connected convolution kernel with coefficients in DWHN format */
  kkFC,
  /* 3x3 MOW kernel with Gather/Scatter on input and output */
  kkGS_MOW,
  /* 1x1 MOW kernel with Gather/Scatter on input and output */
  kkGS_MOW1x1,
  /* S2 MOW kernel with Gather/Scatter on input and output and input depth is 3*/
  kkGS_MOW_S2,
  /* Depthwise convolution */
  kkDepthwise,
  /* DWH with 24 bit accumulator and quad multiplies */
  /* Optimized for D=16/32, kernel=3x3 */
  kkInvQM24,
  /* Fully connected convolution kernel with fp16 data format */
  kkFC_FP16,
  /* kernel types to support symm per channel quantization */
  /* DWH with 24 bit accumulators and quad multiplies */
  kkVQ_QM24,
  /* DWH with 32 bit accumulators and quad multiplier */
  kkVQ_QM32,
  /* Fully connected convolution kernel with cofficients in DWHN format */
  kkVQ_FC,
  /* 3x3 MOW kernel with Gather / Scatter on input and output */
  kkVQ_GS_MOW,
  /* 1x1 MOW kernel with Gather / Scatter on input and output */
  kkVQ_GS_MOW1x1,
  /* S2 NOW kernel with Gather / Scatter on input and output and input depth is 3 */
  kkVQ_GS_MOW_S2,
  /* Depthwise convolution */
  kkVQ_Depthwise,
  /* DWH with 24 bit accumulator and quad multiplies */
  /* Optimized for D=16/32, kernel=3x3 */
  kkVQ_InvQM24,
  /* Fully connected convolution kernel with fp16 data format */
  kkVQ_FC_FP16,
  /* kernel types support for VQ7 */
  kkVQ7Opt,
} conv_kernel_kind_e;

#define QM32_ROW_PADDING     sizeof(int32_t)
#define QM32_FILTER_PADDING  (2 * sizeof(int32_t))
#define QM32_COL_PADDING     4

/*
 * Local memory configuration
 */

typedef struct PACKED
{
  /* Number of tiles in each dimension of output */
  uint32_t numTilesD;
  uint32_t numTilesW;
  uint32_t numTilesH;

  /* Tile size in bytes for each data object */
  uint32_t inpTileSize;
  uint32_t coeffTileSize;
  uint32_t outpTileSize;
  uint32_t biasTileSize;
  uint32_t fixupTileSize;
  uint32_t preluTileSize;
  uint32_t offsetTileSize;
  /* Per Channel quantization parameters */
  uint32_t outScaleSize;
  uint32_t outShiftSize;

  /* Double buffering flags */
  uint32_t doubleBuffInput;
  uint32_t doubleBuffOutput;
  uint32_t doubleBuffCoeff;

  /* Total bytes needed in local memory */
  uint32_t        totalSize;
  /* number of groups */
  uint32_t        numGroups;
  /* Device local memory description */
  localMem_info_t localMem;
} conv_mem_info_t;

/*
 * Convolution command descriptor
 */

/* Boolean: is ReLU enabled. Min/Max are specified as separate fields. */
#define CONV_FLAG_GET_RELU(flags)                      ((flags) & 1)
#define CONV_FLAG_SET_RELU(flags, value)               (flags) = ((flags) & ~1) | ((value) & 1)
/* Boolean: reload input. Prefer to reload input for every coefficients
   tile instead of reloading coefficients. */
#define CONV_FLAG_GET_RELOAD_INPUT(flags)              ((flags) & 2)
#define CONV_FLAG_SET_RELOAD_INPUT(flags, value)       (flags) = ((flags) & ~2) | (((value) & 1) << 1)
/* Which kernel to use: conv_kernel_kind_e enum. */
#define CONV_FLAG_GET_KERNEL_KIND(flags)               ((conv_kernel_kind_e) (((flags) >> 2) & 0x003F))
#define CONV_FLAG_SET_KERNEL_KIND(flags, value)        (flags) = ((flags) & ~0xFC) | (((value) & 63) << 2)

#define CONV_FLAG_GET_DOUBLE_BUFF_COEFF(flags)         ((flags) & (1 << 8))
#define CONV_FLAG_SET_DOUBLE_BUFF_COEFF(flags, value)  (flags) = ((flags) & ~(1 << 8)) | (((value) & 1) << 8)

/* PRELU is needed */
#define CONV_FLAG_GET_PRELU(flags)        ((flags) & (1 << 9))
#define CONV_FLAG_SET_PRELU(flags)        (flags) = ((flags) | (1 << 9))
#define CONV_FLAG_GET_LEAKY_PRELU(flags)  ((flags) & (1 << 10))
#define CONV_FLAG_SET_LEAKY_PRELU(flags)  (flags) = ((flags) | (1 << 10))

/* data format is FP16 */
#define CONV_FLAG_SET_FP16(flags)    ((flags) = (flags) | (1 << 11))
#define CONV_FLAG_RESET_FP16(flags)  ((flags) = (flags) & (~(1 << 11)))
#define CONV_FLAG_GET_FP16(flags)    (((flags) >> 11) & 0x1)

typedef struct PACKED
{
  /* Size of this structure (for consistency checking). */
  int32_t      structSize;
  /* Bank allocation layout */
  banks_info_t banks_info;
  /* Input dimensions. Layout is DWHB wih D is the fastest dimension. */
  dims3d_t     input;
  /* Output dimensions. Layout is DWHB wih D is the fastest dimension. */
  dims3d_t     output;
  /* Batch size for both input and output. */
  uint32_t     batch;
  float        F32_reluMin;
  float        F32_reluMax;
  /* Quantized scale factor applied to the output */
  int32_t      quantizedScale;
  /* right shift bits which is associated with quantized scale factor applied to the output */
  int32_t      outputShift;
  /* Output tile dimensions. */
  dims3d_t     tile;
  /* tile datatype */
  xi_dataType tileType;
  /* coeff tile datatype */
  xi_dataType coeffType;
  /* Convolution kernel width. */
  uint32_t     kernelW;
  /* Convolution kernel height. */
  uint32_t     kernelH;
  /* Stride, same in both W and H dimensions. */
  uint8_t      stride;
  /* Flag bits:
     0   : Enable ReLU
     1   : Reload coefficients
     2-7 : Kernel kind
     8   : Are coefficients double buffered or not
     9   : There is a fused PRELU operation
     10  : There is a fused leaky PRELU operation
     11  : Flag to indicate InvQM24 kernels (TODO: Integrate into kernel_kind)
   */
  uint32_t flags;
  /* Number of Groups */
  uint32_t numGroups;
  /* Offset to the first tile.
       Output is smaller than input and no padding outside
         of input data is needed if offsetX == kernelW/2.
       Output has same size as input and borders are zero padded
         when offsetX == 0.
       Same rules apply in H dimension.
   */
  int8_t  offsetX;
  int8_t  offsetY;
  /* Zero point offset of input data */
  int32_t zeroPtInput;
  /* Zero point offset of coefficient data */
  int32_t zeroPtCoeff;

  /* Zero point offset of output data */
  int32_t zeroPtOutput;
  /* minimum threshold value to control the output range to implement ReLU/ReLU1/ReLU6 */
  int32_t reluMin;
  /* maximum threshold value to control the output range to implement ReLU/ReLU1/ReLU6 */
  int32_t reluMax;

  // Fused PRELU parameters
  int32_t inZeroPoint;
  int32_t alphaZeroPoint;
  int32_t outZeroPoint;
  int32_t posOutMultiplier;
  int32_t posOutShift;
  int32_t negOutMultiplier;
  int32_t negOutShift;
  int32_t VQ7_512MAC_supported;
  int32_t isVQ7optimize;
  size_t coeff_reord_size;
  size_t bias_reord_size;
  size_t scale_size;
  size_t shift_size;
  // minVal == 0, maxVal = 255 ???
} conv_params_t;

typedef int32_t XI_ERR_TYPE;

void flk_conv_ref(const uint8_t *raw_params,
                     struct XtensaOperationArgsIn *input,
                     struct XtensaOperationArgsOut *output);

void flk_depthwise_conv_ref(const uint8_t* raw_params,
    struct XtensaOperationArgsIn* input,
    struct XtensaOperationArgsOut* output);

XI_ERR_TYPE flk_depthwise_conv(const uint8_t *raw_params,
                               struct XtensaOperationArgsIn *input,
                               struct XtensaOperationArgsOut *output);

XI_ERR_TYPE flk_conv(const uint8_t *raw_params,
                     struct XtensaOperationArgsIn *input,
                     struct XtensaOperationArgsOut *output);

// setup shape used for checks in isSUpported()
//
// Memory estimation functions shared between DSP and Host
//

static inline size_t FillMemInfoConv2D(const conv_params_t *params, conv_mem_info_t *info)
{
  BOOL_T FP16_flag;
  // Number of tiles in each dimension
  info->numTilesD = (params->output.D + params->tile.D - 1) / params->tile.D;
  info->numTilesW = (params->output.W + params->tile.W - 1) / params->tile.W;
  info->numTilesH = (params->output.H + params->tile.H - 1) / params->tile.H;

  // Max input tile dimensions
  int inpTileW = (params->tile.W - 1) * params->stride + 1;
  int inpTileH = (params->tile.H - 1) * params->stride + 1;

  FP16_flag = (BOOL_T) CONV_FLAG_GET_FP16(params->flags);
  // Tile size in bytes for each data object
  info->inpTileSize    = (params->input.D * (inpTileW + params->kernelW - 1) * (inpTileH + params->kernelH - 1)) << FP16_flag;
  info->outpTileSize   = (params->tile.D * params->tile.W * params->tile.H) << FP16_flag;
  info->biasTileSize   = (sizeof(int32_t) * (((params->output.D + 15) >> 4) << 4)) >> FP16_flag;
  info->coeffTileSize  = (params->kernelW * params->kernelH * params->input.D) << FP16_flag;
  info->offsetTileSize = 0;
  info->fixupTileSize  = 0;
  info->outScaleSize   = sizeof(int32_t) * params->output.D;
  info->outShiftSize   = sizeof(int8_t) * params->output.D;

  conv_kernel_kind_e kflag = CONV_FLAG_GET_KERNEL_KIND(params->flags);

  if (kflag == kkCa2)
  {
    // Do not interferre with FC
    if (params->tile.W == 1 && params->tile.H == 1)
    {
        //printf("failed at kernel kind: kkCa2, using FC\n");
        return(0);
    }
    // Pad in coefficients tile depth to 64.
    info->coeffTileSize *= align_up(params->tile.D, ALIGNMENT);
    info->fixupTileSize  = sizeof(int32_t) * params->tile.W * params->tile.H;
    info->biasTileSize   = (sizeof(int32_t) * params->output.D) >> FP16_flag;
  }
  else if (kflag == kkQM24 || kflag == kkQM32)
  {

    // >= QM32_COL_PADDING is artifitial constraint, kernel can handle
    // even 1x1 but it leads to less than 1% MAC utilization.
    if (params->tile.W * params->tile.H < QM32_COL_PADDING)
    {
      //printf("failed at kernel kind: kkQM24/kkQM32, using FC\n");
      return(0);
    }
    // Number of rows in spatial dimensions must be multiple of 4,
    // just reserving more space to write garbage there.
    info->outpTileSize = params->tile.D * align_up(params->tile.W * params->tile.H, QM32_COL_PADDING);
    int row_size = align_up(params->kernelW * params->input.D, QM32_ROW_PADDING);
    row_size = align_up(row_size * params->kernelH, QM32_FILTER_PADDING);
    // Coefficients are not padded
	// params->VQ7_512MAC_supported -> changing it to isVQ7 optimize
    if ((params->kernelW > 16) 
		|| (params->kernelH > 16) 
		|| (params->stride > 2) 
		|| ((kflag != kkQM24) && (kflag != kkQM32)))
    {
      info->coeffTileSize = row_size * params->tile.D;
      info->fixupTileSize = sizeof(int32_t) * align_up(params->tile.W * params->tile.H, QM32_COL_PADDING);
      if (params->kernelW == 1 && params->kernelH == 1 && params->input.D % QM32_FILTER_PADDING == 0)
      {
        // 1x1 kernel, fixup will be computed in-place but make sure to reserve two extra rows in input tile
        info->inpTileSize += 2 * row_size;
      }
      else
      {
        info->fixupTileSize += row_size * align_up(params->tile.W * params->tile.H, QM32_COL_PADDING);
      }
    }
    else
    {
      info->outpTileSize   = ((params->tile.D + 15) >> 4) * align_up((params->tile.W << 4) * (params->tile.H), QM32_COL_PADDING);
      info->inpTileSize    = ((inpTileW << 4) + ((params->kernelW - 1) << 4)) * ((params->input.D + 15) >> 4) * (inpTileH + ((params->kernelH - 1)));
      info->offsetTileSize = ((params->input.D + 15) >> 4) * params->kernelW * params->kernelH * sizeof(int32_t);
      info->coeffTileSize  = (align_up(align_up(params->kernelW * (((params->input.D + 15) & (~15)) << 4), QM32_ROW_PADDING) * params->kernelH, QM32_FILTER_PADDING) * ((params->tile.D + 15) >> 4));
      info->fixupTileSize  = sizeof(int32_t) * align_up(((params->tile.W - 1) * params->stride + params->kernelW) * ((params->tile.H - 1) * params->stride + params->kernelH), QM32_COL_PADDING);

	  //vmw      
//vmw      info->biasTileSize   = (sizeof(int32_t) * (((params->tile.D + 15) >> 4) << 4) * ((params->output.D + params->tile.D - 1) / params->tile.D)) >> FP16_flag;
      info->biasTileSize   = (sizeof(int32_t) *   params->output.D  ) >> FP16_flag;
    }
    info->biasTileSize   = (sizeof(int32_t) * params->output.D) >> FP16_flag;


  }
  else if (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkVQ_QM24
           || CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkVQ_QM32)
  {
    // 64 = @ * simd ALIGNMEENT REQUIREST FOR VQ convolutions
    //info->outpTileSize   = align_up(params->tile.D, ALIGNMENT) * params->tile.W * params->tile.H;
    info->coeffTileSize  = align_up(params->tile.D, ALIGNMENT) * params->input.D * params->kernelW * params->kernelH;
    info->biasTileSize   = sizeof(int32_t) * params->output.D; //align_up(params->tile.D, ALIGNMENT);
    info->outScaleSize   = sizeof(int32_t) * params->output.D;
    info->outShiftSize   = sizeof(int8_t) * params->output.D;

  }
  else if (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkInvQM24)
  {
    info->coeffTileSize *= params->tile.D;
    info->fixupTileSize  = sizeof(int32_t) * (params->input.D * params->kernelW * params->kernelH + 4) * align_up(params->tile.W * params->tile.H, 64);
    info->biasTileSize   = (sizeof(int32_t) * params->output.D) >> FP16_flag;
  }
  else if (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkVQ_InvQM24)
  {
    info->coeffTileSize *= params->tile.D;
    info->outScaleSize  = sizeof(int32_t) * params->output.D;
    info->outShiftSize  = sizeof(int8_t) * params->output.D;
    info->fixupTileSize = sizeof(int32_t) * align_up(params->tile.W * params->tile.H, 64) * params->kernelW * params->kernelH * params->input.D;
    info->biasTileSize   = (sizeof(int32_t) * params->output.D) >> FP16_flag;
  }
  else if (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkGS_MOW)
  {
    size_t input_pitch = params->input.D;
    // Make pitch odd to reduce sub-bank conflicts on Gather
    if (input_pitch % 2 == 0)
    {
      input_pitch++;
    }
    info->inpTileSize = input_pitch * (inpTileW + params->kernelW - 1) * (inpTileH + params->kernelH - 1);
    // Make sure gather/scatter doesn't overflow 16 bit offset
    if (31 * input_pitch >= 0x10000 || 31 * params->tile.D >= 0x10000)
    {
      return(0);
    }
    info->coeffTileSize *= params->tile.D;
    info->fixupTileSize  = sizeof(int32_t) * params->tile.W * params->tile.H +
                           params->input.D * (inpTileW + params->kernelW - 1) * (inpTileH + params->kernelH - 1);
    info->biasTileSize   = (sizeof(int32_t) * params->output.D) >> FP16_flag;
  }
  else if (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkVQ_GS_MOW)
  {
    size_t input_pitch = params->input.D;
    // Make pitch odd to reduce sub-bank conflicts on Gather
    if (input_pitch % 2 == 0)
    {
      input_pitch++;
    }
    info->inpTileSize = input_pitch * (inpTileW + params->kernelW - 1) * (inpTileH + params->kernelH - 1);
    // Make sure gather/scatter doesn't overflow 16 bit offset
    if (31 * input_pitch >= 0x10000 || 31 * params->tile.D >= 0x10000)
    {
      return(0);
    }
    info->coeffTileSize *= params->tile.D;
    //info->outScaleSize = sizeof(int32_t) * params->tile.D;
    //info->outShiftSize = sizeof(int8_t) * params->tile.D;
    info->fixupTileSize  = sizeof(int32_t) * params->tile.W * params->tile.H +
                           params->input.D * (inpTileW + params->kernelW - 1) * (inpTileH + params->kernelH - 1);
    info->biasTileSize   = (sizeof(int32_t) * params->output.D) >> FP16_flag;
  }
  else if (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkGS_MOW1x1)
  {
    size_t input_pitch = params->input.D;
    // Make pitch odd to reduce sub-bank conflicts on Gather
    if (input_pitch % 2 == 0)
    {
      input_pitch++;
    }
    info->inpTileSize = input_pitch * (inpTileW + params->kernelW - 1) * (inpTileH + params->kernelH - 1);
    // Make sure gather/scatter doesn't overflow 16 bit offset
    if (31 * input_pitch >= 0x10000 || 31 * params->tile.D >= 0x10000)
    {
      return(0);
    }
    info->coeffTileSize *= params->tile.D;
    info->fixupTileSize  = sizeof(int32_t) * params->tile.W * params->tile.H +
                           params->input.D * (inpTileW + params->kernelW - 1) * (inpTileH + params->kernelH - 1);
    info->biasTileSize   = (sizeof(int32_t) * params->output.D) >> FP16_flag;
  }
  else if (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkVQ_GS_MOW1x1)
  {
    size_t input_pitch = params->input.D;
    // Make pitch odd to reduce sub-bank conflicts on Gather
    if (input_pitch % 2 == 0)
    {
      input_pitch++;
    }
    info->inpTileSize = input_pitch * (inpTileW + params->kernelW - 1) * (inpTileH + params->kernelH - 1);
    // Make sure gather/scatter doesn't overflow 16 bit offset
    if (31 * input_pitch >= 0x10000 || 31 * params->tile.D >= 0x10000)
    {
      return(0);
    }
    info->coeffTileSize *= params->tile.D;
    //info->outScaleSize = sizeof(int32_t) * params->tile.D;
    //info->outShiftSize = sizeof(int8_t) * params->tile.D;
    info->fixupTileSize  = sizeof(int32_t) * params->tile.W * params->tile.H +
                           params->input.D * (inpTileW + params->kernelW - 1) * (inpTileH + params->kernelH - 1);
    info->biasTileSize   = (sizeof(int32_t) * params->output.D) >> FP16_flag;
  }
  else if (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkGS_MOW_S2)
  {
    size_t input_pitch = params->input.D;
    // Make pitch odd to reduce sub-bank conflicts on Gather
    if (input_pitch % 2 == 0)
    {
      input_pitch++;
    }
    info->inpTileSize = input_pitch * (inpTileW + params->kernelW - 1) * (inpTileH + params->kernelH - 1);
    // Make sure gather/scatter doesn't overflow 16 bit offset
    if (31 * input_pitch >= 0x10000 || 31 * params->tile.D >= 0x10000)
    {
      return(0);
    }
    info->coeffTileSize *= params->tile.D;
    info->fixupTileSize  = sizeof(int32_t) * params->tile.W * params->tile.H +
                           (params->input.D * (inpTileW + params->kernelW - 1) * (inpTileH + params->kernelH - 1));
    info->biasTileSize   = (sizeof(int32_t) * params->output.D) >> FP16_flag;
  }
  else if (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkVQ_GS_MOW_S2)
  {
    size_t input_pitch = params->input.D;
    // Make pitch odd to reduce sub-bank conflicts on Gather
    if (input_pitch % 2 == 0)
    {
      input_pitch++;
    }
    info->inpTileSize = input_pitch * (inpTileW + params->kernelW - 1) * (inpTileH + params->kernelH - 1);
    // Make sure gather/scatter doesn't overflow 16 bit offset
    if (31 * input_pitch >= 0x10000 || 31 * params->tile.D >= 0x10000)
    {
       printf("gather/scatter overflows 16bit offest\n");
       return(0);
    }
    info->coeffTileSize *= params->tile.D;
    //info->outScaleSize = sizeof(int32_t) * params->tile.D;
    //info->outShiftSize = sizeof(int8_t) * params->tile.D;
    info->fixupTileSize  = sizeof(int32_t) * params->tile.W * params->tile.H +
                           params->input.D * (inpTileW + params->kernelW - 1) * (inpTileH + params->kernelH - 1);
    info->biasTileSize   = (sizeof(int32_t) * params->output.D) >> FP16_flag;
  }
  else
  {
    info->coeffTileSize *= params->tile.D;
    info->fixupTileSize  = 0;
  }
  if (FP16_flag) {
    info->biasTileSize   = (sizeof(int32_t) * params->output.D) >> FP16_flag;
  }
  if (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkFC || CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkVQ_FC) {
    info->biasTileSize   = (sizeof(int32_t) * params->output.D) >> FP16_flag;
  }

  // Calculate total memory needed
  size_t memSize = align_up(info->biasTileSize, ALIGNMENT);
  memSize += align_up(info->inpTileSize, ALIGNMENT);
  memSize += align_up(info->outpTileSize, ALIGNMENT);
  memSize += align_up(info->coeffTileSize, ALIGNMENT);
  memSize += align_up(info->fixupTileSize, ALIGNMENT);
  memSize += align_up(info->offsetTileSize, ALIGNMENT);
  memSize += align_up(info->outScaleSize, ALIGNMENT);
  memSize += align_up(info->outShiftSize, ALIGNMENT);
  info->preluTileSize = 0;
  if (CONV_FLAG_GET_PRELU(params->flags))
  {
    info->preluTileSize = 1 << FP16_flag;     // leaky PRELU has one element tile
    if (!CONV_FLAG_GET_LEAKY_PRELU(params->flags))
    {
      info->preluTileSize = params->output.D << FP16_flag;
    }
    memSize += align_up(info->preluTileSize, ALIGNMENT);
  }

  // Double buffer if more than one tile
  info->doubleBuffInput  = params->batch != 1 || info->numTilesW != 1 || info->numTilesH != 1;
  info->doubleBuffCoeff  = (info->numTilesD > 1) && CONV_FLAG_GET_DOUBLE_BUFF_COEFF(params->flags);
  info->doubleBuffOutput = info->doubleBuffInput || (info->numTilesD > 1);

  // Account for double buffering
  memSize += info->doubleBuffInput * align_up(info->inpTileSize, ALIGNMENT);
  memSize += info->doubleBuffOutput * align_up(info->outpTileSize, ALIGNMENT);
  memSize += info->doubleBuffCoeff * align_up(info->coeffTileSize, ALIGNMENT);

  info->totalSize = memSize;
  return(memSize);
}


static inline size_t AllocateLocalMemConv2D(const conv_params_t *params, conv_mem_info_t *info)
{
  uint32_t ret;

  if (!FillMemInfoConv2D(params, info))
  {
    return(0);
  }

  // Calculate local memory bank assignments for each buffer in 2 banks case
  // Each "preference" bit corresponds to a specific buffer in XRP local memory
  // The sequence is predefined by call sequence of arena_alloc in XRP

  uint32_t allocArray[XTENSA_OPERATION_MAX_NUM_OPERANDS] = { align_up(info->biasTileSize, ALIGNMENT),
                                                             align_up(info->inpTileSize,  ALIGNMENT) };

  if (info->doubleBuffInput)
  {
    allocArray[2] = align_up(info->inpTileSize, ALIGNMENT);
  }
  allocArray[3] = align_up(info->outpTileSize, ALIGNMENT);
  if (info->doubleBuffOutput)
  {
    allocArray[4] = align_up(info->outpTileSize, ALIGNMENT);
  }
  allocArray[5] = align_up(info->coeffTileSize, ALIGNMENT);
  if (info->doubleBuffCoeff)
  {
    allocArray[6] = align_up(info->coeffTileSize, ALIGNMENT);
  }
  // optional array
  if (info->fixupTileSize)
  {
    allocArray[7] = align_up(info->fixupTileSize, ALIGNMENT);
  }
  if (info->preluTileSize)
  {
    allocArray[8] = align_up(info->preluTileSize, ALIGNMENT);
  }
  if (info->offsetTileSize)
  {
    allocArray[9] = align_up(info->offsetTileSize, ALIGNMENT);
  }
  if (info->outScaleSize)
  {
    allocArray[10] = align_up(info->outScaleSize, ALIGNMENT);
  }
  if (info->outShiftSize)
  {
    allocArray[11] = align_up(info->outShiftSize, ALIGNMENT);
  }
  ret = setupDeviceMemoryLayout(&allocArray[0], 12, &(info->localMem), &(params->banks_info));
  return(ret);
}

static inline size_t FillMemInfoDepthwise(const conv_params_t *params, conv_mem_info_t *info)
{
  int outputD, outputW;
  int inputD, inputW;
  int tileD, tileW;

  outputD = params->output.D; outputW = params->output.W;
  inputD = params->input.D; inputW = params->input.W;
  tileD = params->tile.D; tileW = params->tile.W;


  // Number of tiles in each dimension
  info->numTilesD = (params->output.D + params->tile.D - 1) / params->tile.D;
  info->numTilesW = (params->output.W + params->tile.W - 1) / params->tile.W;
  info->numTilesH = (params->output.H + params->tile.H - 1) / params->tile.H;

  // Max input tile dimensions
  int inpTileW;
  int inpTileH;


  inpTileW = (params->tile.W - 1) * params->stride + 1;
  inpTileH = (params->tile.H - 1) * params->stride + 1;

  // Assume it is VQ7 Optimize


  // Tile size in bytes for each data object
  {
	info->inpTileSize   = params->tile.D * (inpTileW + params->kernelW - 1) * (inpTileH + params->kernelH - 1);
	info->outpTileSize  = params->tile.D * params->tile.W * params->tile.H;
	info->biasTileSize  = sizeof(int32_t) * outputD;
	info->coeffTileSize = params->kernelW * params->kernelH * align_up(tileD, ALIGNMENT);
	info->fixupTileSize = 0;
	info->outScaleSize  = sizeof(int32_t) * outputD;
	info->outShiftSize  = sizeof(int8_t) * outputD;	
  }


  // Calculate total memory needed
  size_t memSize = align_up(info->biasTileSize, ALIGNMENT);
  memSize += align_up(info->inpTileSize, ALIGNMENT);
  memSize += align_up(info->outpTileSize, ALIGNMENT);
  memSize += align_up(info->coeffTileSize, ALIGNMENT);
  memSize += align_up(info->outScaleSize, ALIGNMENT);
  memSize += align_up(info->outShiftSize, ALIGNMENT);

  // Double buffer if more
  info->doubleBuffCoeff = info->numTilesD > 1;
  info->doubleBuffInput = info->doubleBuffCoeff || params->batch != 1 || info->numTilesW != 1 || info->numTilesH != 1;

  
  // Account for double buffering
  memSize += info->doubleBuffInput * (align_up(info->inpTileSize, ALIGNMENT)
                                      + align_up(info->outpTileSize, ALIGNMENT));
  memSize += info->doubleBuffCoeff * align_up(info->coeffTileSize, ALIGNMENT);

  info->totalSize = memSize;

  return(memSize);
}

static inline size_t AllocateLocalMemDepthwise(const conv_params_t *params, conv_mem_info_t *info)
{
  uint32_t ret;

  if (!FillMemInfoDepthwise(params, info))
  {
    return(0);
  }

  // Calculate local memory bank assignments for each buffer in 2 banks case
  // Each "preference" bit corresponds to a specific buffer in XRP local memory
  // The sequence is predefined by call sequence of arena_alloc in XRP

  uint32_t allocArray[XTENSA_OPERATION_MAX_NUM_OPERANDS] = { align_up(info->biasTileSize, ALIGNMENT),
                                                             align_up(info->inpTileSize,  ALIGNMENT) };

  if (info->doubleBuffInput)
  {
    allocArray[2] = align_up(info->inpTileSize, ALIGNMENT);
  }
  allocArray[3] = align_up(info->outpTileSize, ALIGNMENT);
  if (info->doubleBuffInput)
  {
    allocArray[4] = align_up(info->outpTileSize, ALIGNMENT);
  }
  allocArray[5] = align_up(info->coeffTileSize, ALIGNMENT);
  if (info->doubleBuffCoeff)
  {
    allocArray[6] = align_up(info->coeffTileSize, ALIGNMENT);
  }
  if (info->outScaleSize)
  {
     allocArray[7] = align_up(info->outScaleSize, ALIGNMENT);
  }
  if (info->outShiftSize)
  {
    allocArray[8] = align_up(info->outShiftSize, ALIGNMENT);
  }

  ret = setupDeviceMemoryLayout(&allocArray[0], 9, &(info->localMem), &(params->banks_info));
  return(ret);
}

static inline size_t FillMemInfoGroupedConv2D(const conv_params_t *params, conv_mem_info_t *info)
{
  // Number of tiles in each dimension
  info->numTilesD = ((params->output.D / params->numGroups) + params->tile.D - 1) / params->tile.D;
  info->numTilesW = (params->output.W + params->tile.W - 1) / params->tile.W;
  info->numTilesH = (params->output.H + params->tile.H - 1) / params->tile.H;
  info->numGroups = params->numGroups;

  // Max input tile dimensions
  int inpTileW = (params->tile.W - 1) * params->stride + 1;
  int inpTileH = (params->tile.H - 1) * params->stride + 1;

  // Tile size in bytes for each data object
  info->inpTileSize   = (params->input.D / params->numGroups) * (inpTileW + params->kernelW - 1) * (inpTileH + params->kernelH - 1);
  info->outpTileSize  = params->tile.D * params->tile.W * params->tile.H;
  info->biasTileSize  = sizeof(int32_t) * (params->output.D);
  info->coeffTileSize = params->kernelW * params->kernelH * (params->input.D / params->numGroups);
  info->preluTileSize = 0;
  info->outScaleSize  = sizeof(int32_t) * params->output.D;
  info->outShiftSize  = sizeof(int8_t) * params->output.D;

  if (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkQM24
      || CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkQM32)
  {
    // >= QM32_COL_PADDING is artifitial constraint, kernel can handle
    // even 1x1 but it leads to less than 1% MAC utilization.
    /*
       if (params->tile.W * params->tile.H < QM32_COL_PADDING)
        return 0;
     */
    // Number of rows in spatial dimensions must be multiple of 4,
    // just reserving more space to write garbage there.
    info->outpTileSize = params->tile.D * align_up(params->tile.W * params->tile.H, QM32_COL_PADDING);
    int row_size = align_up(params->kernelW * (params->input.D / params->numGroups), QM32_ROW_PADDING);
    row_size = align_up(row_size * params->kernelH, QM32_FILTER_PADDING);
    // Coefficients are not padded
    info->coeffTileSize = row_size * params->tile.D;
    info->fixupTileSize = sizeof(int32_t) * align_up(params->tile.W * params->tile.H, QM32_COL_PADDING);
    if (params->kernelW == 1 && params->kernelH == 1 && (params->input.D / params->numGroups) % QM32_FILTER_PADDING == 0)
    {
      // 1x1 kernel, fixup will be computed in-place but make sure to reserve two extra rows in input tile
      info->inpTileSize += 2 * row_size;
    }
    else
    {
      info->fixupTileSize += row_size * align_up(params->tile.W * params->tile.H, QM32_COL_PADDING);
    }
  }
  else if (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkVQ_QM24
           || CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkVQ_QM32)
  {
    // >= QM32_COL_PADDING is artifitial constraint, kernel can handle
    // even 1x1 but it leads to less than 1% MAC utilization.
    /*
       if (params->tile.W * params->tile.H < QM32_COL_PADDING)
        return 0;
     */
    // Number of rows in spatial dimensions must be multiple of 4,
    // just reserving more space to write garbage there.
    info->outpTileSize = params->tile.D * params->tile.W * params->tile.H;
    int row_size = params->kernelW * (params->input.D / params->numGroups); ;
    row_size = row_size * params->kernelH;
    // Coefficients are not padded
    info->coeffTileSize = row_size * align_up(params->tile.D, ALIGNMENT);
    if (params->kernelW == 1 && params->kernelH == 1 && (params->input.D / params->numGroups) % QM32_FILTER_PADDING == 0)
    {
      // 1x1 kernel, fixup will be computed in-place but make sure to reserve two extra rows in input tile
      info->inpTileSize += 2 * row_size;
    }

    info->fixupTileSize  = 0;
  }
  else
  {
    info->coeffTileSize *= params->tile.D;
    info->fixupTileSize  = 0;
  }

  // Calculate total memory needed
  size_t memSize = align_up(info->biasTileSize, ALIGNMENT);
  memSize += align_up(info->inpTileSize, ALIGNMENT);
  memSize += align_up(info->outpTileSize, ALIGNMENT);
  memSize += align_up(info->coeffTileSize, ALIGNMENT);
  memSize += align_up(info->fixupTileSize, ALIGNMENT);
  memSize += align_up(info->outScaleSize, ALIGNMENT);
  memSize += align_up(info->outShiftSize, ALIGNMENT);

  info->preluTileSize = 0;
  if (CONV_FLAG_GET_PRELU(params->flags))
  {
    info->preluTileSize = 1;     // leaky PRELU has one element tile
  }
  if (!CONV_FLAG_GET_LEAKY_PRELU(params->flags))
  {
    info->preluTileSize = params->output.D;
    memSize            += align_up(info->preluTileSize, ALIGNMENT);
  }

  // Double buffer if more than one tile
  info->doubleBuffInput  = params->batch != 1 || info->numTilesW != 1 || info->numTilesH != 1 || info->numTilesD != 1 || info->numGroups != 1;
  info->doubleBuffCoeff  = ((info->numTilesD > 1) || (info->numGroups > 1)) && CONV_FLAG_GET_DOUBLE_BUFF_COEFF(params->flags);
  info->doubleBuffOutput = info->doubleBuffInput || (info->numTilesD > 1) || (info->numGroups > 1);
  // Account for double buffering
  memSize += info->doubleBuffInput * align_up(info->inpTileSize, ALIGNMENT);
  memSize += info->doubleBuffOutput * align_up(info->outpTileSize, ALIGNMENT);
  memSize += info->doubleBuffCoeff * align_up(info->coeffTileSize, ALIGNMENT);

  info->totalSize = memSize;

  return(memSize);
}

static inline size_t AllocateLocalMemGroupedConv2D(const conv_params_t *params, conv_mem_info_t *info)
{
  uint32_t ret;
  if (!FillMemInfoGroupedConv2D(params, info))
  {
    return(0);
  }
  // Calculate local memory bank assignments for each buffer in 2 banks case
  // Each "preference" bit corresponds to a specific buffer in XRP local memory
  // The sequence is predefined by call sequence of arena_alloc in XRP

  uint32_t allocArray[XTENSA_OPERATION_MAX_NUM_OPERANDS] = { align_up(info->biasTileSize, ALIGNMENT),
                                                             align_up(info->inpTileSize,  ALIGNMENT) };

  if (info->doubleBuffInput)
  {
    allocArray[2] = align_up(info->inpTileSize, ALIGNMENT);
  }
  allocArray[3] = align_up(info->outpTileSize, ALIGNMENT);
  if (info->doubleBuffOutput)
  {
    allocArray[4] = align_up(info->outpTileSize, ALIGNMENT);
  }
  allocArray[5] = align_up(info->coeffTileSize, ALIGNMENT);
  if (info->doubleBuffCoeff)
  {
    allocArray[6] = align_up(info->coeffTileSize, ALIGNMENT);
  }
  // optional array
  if (info->fixupTileSize)
  {
    allocArray[7] = align_up(info->fixupTileSize, ALIGNMENT);
  }
  if (info->preluTileSize)
  {
    allocArray[8] = align_up(info->preluTileSize, ALIGNMENT);
  }
  if (info->outScaleSize)
  {
    allocArray[9] = align_up(info->outScaleSize, ALIGNMENT);
  }
  if (info->outShiftSize)
  {
    allocArray[10] = align_up(info->outShiftSize, ALIGNMENT);
  }

  ret = setupDeviceMemoryLayout(&allocArray[0], 11, &(info->localMem), &(params->banks_info));

  return(ret);
}

/* Setup convolution descriptor:
   - select kernel
   - select tile size
   - fill out mem info

   Returns:
   - true on success.
   - false if failed and unable to initialize descriptor
     for given parameters and local memory size.
 */

bool SetupConv2D(conv_params_t& params, bool allow_acc24, bool channel_qflag, conv_kernel_kind_e force_kernel, conv_mem_info_t *mem_info,int32_t coreCount, int32_t batch);
bool SetupDepthwise(conv_params_t& params, conv_kernel_kind_e force_kernel, conv_mem_info_t *mem_info, int32_t coreCount);
bool SetupGroupedConv2D(conv_params_t& params, bool allow_acc24, bool channel_qflag, conv_kernel_kind_e force_kernel, conv_mem_info_t *mem_info);

// Calculate coefficients buffer size
size_t ConvCoefficientsBufferSize(const conv_params_t& params);
size_t GroupedConvCoefficientsBufferSize(const conv_params_t& params);

// Check <count> filters of <filter_size> size each in
// <filters_data> for possible overflow of 24 bit accumulator.
bool MayOverflowAccumulator24(const uint8_t *filters_data,
                              size_t filter_size,
                              size_t count);

bool MayOverflowAcc24GroupedConv2D(const uint8_t *filters_data,
                                   const conv_params_t& params);

// Reorder and fixup coefficients for selected kernel
bool ConvReorderCoefficients(const uint8_t *coeff_ref,
                             const int32_t* bias_ref,
                             uint8_t *coeff,
                             int32_t* bias,
                             const conv_params_t& params);

bool GroupedConvReorderCoefficients(const uint8_t *coeff_ref,
                                    const int32_t* bias_ref,
                                    uint8_t *coeff,
                                    int32_t* bias,
                                    const conv_params_t& params);

#endif /* _FLK_CONV_H_INCLUDED_ */

