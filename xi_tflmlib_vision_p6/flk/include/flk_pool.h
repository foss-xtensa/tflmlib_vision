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
#ifndef _FLK_POOL_H_INCLUDED_
#define _FLK_POOL_H_INCLUDED_

#include "flk_common.h"

typedef uint8_t POOL_TYPE;

#define MAX_POOLING 0
#define AVG_POOLING 1
#define L2_POOLING  2

#define ACTIVATION_RELU_NONE 0
#define ACTIVATION_RELU_0 1
#define ACTIVATION_RELU_1 2
#define ACTIVATION_RELU_6 3

typedef struct PACKED {
    /* Size of this structure (for consistency checking). */
    int32_t  structSize;
    /* Input dimensions. Layout is DWHB wih D is the fastest dimension. */
    dims3d_t input;
    /* Output dimensions. Layout is DWHB wih D is the fastest dimension. */
    dims3d_t output;
    /* Output tile dimensions. */
    dims3d_t tile;
    /* Batch size for both input and output. */
    uint32_t batch;
    /* Convolution kernel width. */
    uint8_t  kernelW;
    /* Convolution kernel height. */
    uint8_t  kernelH;
    /* Stride, same in both W and H dimensions. */
    uint8_t  strideW;
    uint8_t  strideH;
    /* Offset to the first tile.
         Output is smaller than input and no padding outside
           of input data is needed if offsetX == kernelW/2.
         Output has same size as input and borders are zero padded
           when offsetX == 0.
         Same rules apply in H dimension.
    */
    int8_t   offsetX;
    int8_t   offsetY;
    /* Zero point offset of input data */
    int32_t  zeroPtInput;
    /* Zero point offset of output data */
    int32_t  zeroPtOutput;
    /* Type of pool : Max/Avg. pooling */
    POOL_TYPE type;
    /* Activation Layer */
    uint8_t activation;
    int32_t reluMin, reluMax; // Changed data type from uint8_t to int16_t
    /* output scale parameters */
    uint8_t qFlag;
    int32_t multiplierOut, shiftOut, left_shift;
    /* Memory Info */
    uint8_t largeInd;
    /* Tile size and memory info */
    uint32_t memInpTile, memOutpTile;
    // Field to capture quantized tensor signed.
    // Set structure's dataType field to XI_S8 if ASYMM_SIGNED or XI_U8 if ASYMM
    xi_dataType  quantTensorSign;
} pool_params_t;

typedef int32_t XI_ERR_TYPE;

XI_ERR_TYPE flk_pool(const uint8_t *raw_params,
                     struct XtensaOperationArgsIn *input,
                     struct XtensaOperationArgsOut *output);


#if defined(__cplusplus)

/* Setup pool descriptor:
   - select kernel
   - select tile size

   Returns:
   -  zero on success;
   - non-zero if failed for given parameters and local memory size.
*/
bool poolSetup(pool_params_t* params, const size_t largeBank, const size_t smallBank, int32_t coreCount);

#endif /* __cplusplus */

#endif /* _FLK_POOL_H_INCLUDED_ */
