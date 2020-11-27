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
#ifndef __XI_CNN_API_H__
#define __XI_CNN_API_H__

#if defined(__XTENSA__) || defined(__GNUC__)
#else
#define ALIGN(x)  _declspec(align(x))
#define ALIGN16  _declspec(align(16))
#define ALIGN32  _declspec(align(32))
#define ALIGN64  _declspec(align(64))
#define __restrict
#endif
//#include "xi_config_api.h"
#include "xi_core_api.h"
#include "xi_tile3d_manager.h"
#include "xi_tile_manager.h"


#if 1// ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))
#if (XCHAL_HAVE_VISION_HP_VFPU == 1)
#include <xtensa/tie/xt_misc.h>
#include <math.h>
#endif

/***************************************************************************************/
/* log2 function is not defined in Visual Studio 2012 but available in higher versions */
/* _MSC_VER version number check to be performed for visual studio version             */
/* If _MSC_VER <= (Visual Studio 2012) version log2 function  is enabled               */
/* Visual Studio Version Information :                                                 */
/* MSVC++ 14.0 _MSC_VER == 1900 (Visual Studio 2015)                                   */
/* MSVC++ 12.0 _MSC_VER == 1800 (Visual Studio 2013)                                   */
/* MSVC++ 11.0 _MSC_VER == 1700 (Visual Studio 2012)                                   */
/* MSVC++ 10.0 _MSC_VER == 1600 (Visual Studio 2010)                                   */
/* MSVC++ 9.0  _MSC_VER == 1500 (Visual Studio 2008)                                   */
/* MSVC++ 8.0  _MSC_VER == 1400 (Visual Studio 2005)                                   */
/***************************************************************************************/

#if defined(_MSC_VER)
#if _MSC_VER <= 1700
#include "math.h"
static double log2(double number)
{
  /* Calculates log2 of number.  */
  return(log(number) / log(2.0));
}
#endif
#endif

#define CNN_CONV_FLAG_RELU          (1 << 0)
#define CNN_CONV_FLAG_LEFTEDGE      (1 << 1)
#define CNN_CONV_FLAG_TOPEDGE       (1 << 2)

#define CNN_POOLING_TOPEDGE_FLAG    (1 << 1)
#define CNN_POOLING_LEFTEDGE_FLAG   (1 << 0)

#define CNN_L2POOL2D_TOPEDGE_FLAG   (1 << 1)
#define CNN_L2POOL2D_LEFTEDGE_FLAG  (1 << 0)

#define CNN_MAXPOOLA_TOPEDGE_FLAG   (1 << 1)
#define CNN_MAXPOOLA_LEFTEDGE_FLAG  (1 << 0)

#define CNN_MINPOOLA_TOPEDGE_FLAG   (1 << 1)
#define CNN_MINPOOLA_LEFTEDGE_FLAG  (1 << 0)

#define CNN_AVGPOOLA_TOPEDGE_FLAG   (1 << 1)
#define CNN_AVGPOOLA_LEFTEDGE_FLAG  (1 << 0)

#define CNN_ENHANCED_LSTMA_FORGET_PEEPHOLE (1 << 0)
#define CNN_ENHANCED_LSTMA_INPUT_PEEPHOLE  (1 << 1)
#define CNN_ENHANCED_LSTMA_OUTPUT_PEEPHOLE (1 << 2)

#define CNN_ENHANCED_LSTMA_FORGET_LAYERNORM      (1 << 0)
#define CNN_ENHANCED_LSTMA_MODULATION_LAYERNORM  (1 << 1)
#define CNN_ENHANCED_LSTMA_INPUT_LAYERNORM       (1 << 2)
#define CNN_ENHANCED_LSTMA_OUTPUT_LAYERNORM      (1 << 3)

typedef struct
{
  uint8_t  stride;
  uint8_t  accumShift;             // Accumulator Shift - Shift to convert accumulator data to 16 bit
  uint16_t outputScale;            // Amount by which shifted data is scaled
  uint8_t  outputShift;            // Shift amount to convert the scaled data to 16 bit
  uint8_t  flags;                  // enableRelu, contiguous-data, edge flag;
  uint8_t  dilation;

  /*bit0 corresponds to Relu flag.
   * bit1 corresponds to leftEdgeFlag
   * bit2 corresponds to topEdgeFlag
   *
   *  --------------------------------------------------------------------------
   *  | bit7  | bit6 | bit5 | bit4 | bit3 | bit2       | bit1       | bit0     |
   *  |unused |unused|unused|unused|unused| topEdgeFlag|leftEdgeFlag|Relu Flag |
   *  --------------------------------------------------------------------------
   */
} xi_cnn_conv_params;

#define XI_CNN_CONV_GET_STRIDE(x)           ((x)->stride)
#define XI_CNN_CONV_SET_STRIDE(x, v)        ((x)->stride = (v))
#define XI_CNN_CONV_GET_ACCUM_SHIFT(x)      ((x)->accumShift)
#define XI_CNN_CONV_SET_ACCUM_SHIFT(x, v)   ((x)->accumShift = (v))
#define XI_CNN_CONV_GET_OUTPUT_SCALE(x)     ((x)->outputScale)
#define XI_CNN_CONV_SET_OUTPUT_SCALE(x, v)  ((x)->outputScale = (v))
#define XI_CNN_CONV_GET_OUTPUT_SHIFT(x)     ((x)->outputShift)
#define XI_CNN_CONV_SET_OUTPUT_SHIFT(x, v)  ((x)->outputShift = (v))
#define XI_CNN_CONV_GET_FLAGS(x)            ((x)->flags)
#define XI_CNN_CONV_SET_FLAGS(x, v)         ((x)->flags = (v))
#define XI_CNN_CONV_GET_FLAG_RELU(x)        ((x)->flags & CNN_CONV_FLAG_RELU)
#define XI_CNN_CONV_SET_FLAG_RELU(x)        ((x)->flags = ((x)->flags | CNN_CONV_FLAG_RELU))
#define XI_CNN_CONV_RESET_FLAG_RELU(x)      ((x)->flags = ((x)->flags & ~CNN_CONV_FLAG_RELU))
#define XI_CNN_CONV_GET_FLAG_LEFTEDGE(x)    ((x)->flags & CNN_CONV_FLAG_LEFTEDGE)
#define XI_CNN_CONV_SET_FLAG_LEFTEDGE(x)    ((x)->flags = ((x)->flags | CNN_CONV_FLAG_LEFTEDGE))
#define XI_CNN_CONV_RESET_FLAG_LEFTEDGE(x)  ((x)->flags = ((x)->flags & ~CNN_CONV_FLAG_LEFTEDGE))
#define XI_CNN_CONV_GET_FLAG_TOPEDGE(x)     ((x)->flags & CNN_CONV_FLAG_TOPEDGE)
#define XI_CNN_CONV_SET_FLAG_TOPEDGE(x)     ((x)->flags = ((x)->flags | CNN_CONV_FLAG_TOPEDGE))
#define XI_CNN_CONV_RESET_FLAG_TOPEDGE(x)   ((x)->flags = ((x)->flags & ~CNN_CONV_FLAG_TOPEDGE))
#define XI_CNN_CONV_GET_DILATION(x)         ((x)->dilation)
#define XI_CNN_CONV_SET_DILATION(x, v)      ((x)->dilation = (v))

typedef struct
{
  int32_t input_zero_point;             //offset for centering the u8 into signed ranged
  int32_t input_range_radius;           // defines the input radius for defining lower and upper limits
  int32_t input_multiplier;             // significand of scaleQ4.27
  int32_t input_left_shift;             // exponent part of scaleQ4.27
} xi_cnn_sigmoidA_params;

#define XI_CNN_SIGMOIDA_PARAMS_GET_ZERO_POINT(x)       ((x)->input_zero_point)
#define XI_CNN_SIGMOIDA_PARAMS_GET_RANGE_RADIUS(x)     ((x)->input_range_radius)
#define XI_CNN_SIGMOIDA_PARAMS_GET_MULTIPLIER(x)       ((x)->input_multiplier)
#define XI_CNN_SIGMOIDA_PARAMS_GET_LEFT_SHIFT(x)       ((x)->input_left_shift)
#define XI_CNN_SIGMOIDA_PARAMS_SET_ZERO_POINT(x, v)    ((x)->input_zero_point = (v))
#define XI_CNN_SIGMOIDA_PARAMS_SET_RANGE_RADIUS(x, v)  ((x)->input_range_radius = (v))
#define XI_CNN_SIGMOIDA_PARAMS_SET_MULTIPLIER(x, v)    ((x)->input_multiplier = (v))
#define XI_CNN_SIGMOIDA_PARAMS_SET_LEFT_SHIFT(x, v)    ((x)->input_left_shift = (v))

typedef struct
{
  int32_t input_zero_point;             //offset for centering the u8 into signed ranged
  int32_t input_range_radius;           // defines the input radius for defining lower and upper limits
  int32_t input_multiplier;             // significand of scaleQ4.27
  int32_t input_left_shift;             // exponent part of scaleQ4.27
} xi_cnn_tanhA_params;

#define XI_CNN_TANHA_PARAMS_GET_ZERO_POINT(x)       ((x)->input_zero_point)
#define XI_CNN_TANHA_PARAMS_GET_RANGE_RADIUS(x)     ((x)->input_range_radius)
#define XI_CNN_TANHA_PARAMS_GET_MULTIPLIER(x)       ((x)->input_multiplier)
#define XI_CNN_TANHA_PARAMS_GET_LEFT_SHIFT(x)       ((x)->input_left_shift)
#define XI_CNN_TANHA_PARAMS_SET_ZERO_POINT(x, v)    ((x)->input_zero_point = (v))
#define XI_CNN_TANHA_PARAMS_SET_RANGE_RADIUS(x, v)  ((x)->input_range_radius = (v))
#define XI_CNN_TANHA_PARAMS_SET_MULTIPLIER(x, v)    ((x)->input_multiplier = (v))
#define XI_CNN_TANHA_PARAMS_SET_LEFT_SHIFT(x, v)    ((x)->input_left_shift = (v))

typedef struct
{
  int32_t diff_min;                     //defines minimum difference with respect to the maximum value
  int32_t input_beta_multiplier;        //significand of BetaScaleQ5.26
  int32_t input_beta_left_shift;        //exponent of BetaScaleQ5.26
  int32_t axis;                         //axis along which softmax is to be computed
} xi_cnn_softmaxA_params;

#define XI_CNN_SOFTMAXA_PARAMS_GET_DIFF_MIN(x)       ((x)->diff_min)
#define XI_CNN_SOFTMAXA_PARAMS_GET_MULTIPLIER(x)     ((x)->input_beta_multiplier)
#define XI_CNN_SOFTMAXA_PARAMS_GET_LEFT_SHIFT(x)     ((x)->input_beta_left_shift)
#define XI_CNN_SOFTMAXA_PARAMS_GET_AXIS(x)           ((x)->axis)
#define XI_CNN_SOFTMAXA_PARAMS_SET_DIFF_MIN(x, v)    ((x)->diff_min = (v))
#define XI_CNN_SOFTMAXA_PARAMS_SET_MULTIPLIER(x, v)  ((x)->input_beta_multiplier = (v))
#define XI_CNN_SOFTMAXA_PARAMS_SET_LEFT_SHIFT(x, v)  ((x)->input_beta_left_shift = (v))
#define XI_CNN_SOFTMAXA_PARAMS_SET_AXIS(x, v)        ((x)->axis = (v))

#define XI_CNN_MEANA_DIM1     (0x1)
#define XI_CNN_MEANA_DIM2     (0x2)
#define XI_CNN_MEANA_DIM3     (0x4)
#define XI_CNN_MEANA_DIM4     (0x8)

#define XI_CNN_MEANA_DIM12    (XI_CNN_MEANA_DIM1 | XI_CNN_MEANA_DIM2)
#define XI_CNN_MEANA_DIM13    (XI_CNN_MEANA_DIM1 | XI_CNN_MEANA_DIM3)
#define XI_CNN_MEANA_DIM23    (XI_CNN_MEANA_DIM2 | XI_CNN_MEANA_DIM3)
#define XI_CNN_MEANA_DIM14    (XI_CNN_MEANA_DIM1 | XI_CNN_MEANA_DIM4)
#define XI_CNN_MEANA_DIM24    (XI_CNN_MEANA_DIM2 | XI_CNN_MEANA_DIM4)
#define XI_CNN_MEANA_DIM34    (XI_CNN_MEANA_DIM3 | XI_CNN_MEANA_DIM4)

#define XI_CNN_MEANA_DIM123   (XI_CNN_MEANA_DIM1 | XI_CNN_MEANA_DIM2 | XI_CNN_MEANA_DIM3)
#define XI_CNN_MEANA_DIM124   (XI_CNN_MEANA_DIM1 | XI_CNN_MEANA_DIM2 | XI_CNN_MEANA_DIM4)
#define XI_CNN_MEANA_DIM134   (XI_CNN_MEANA_DIM1 | XI_CNN_MEANA_DIM4 | XI_CNN_MEANA_DIM3)
#define XI_CNN_MEANA_DIM234   (XI_CNN_MEANA_DIM4 | XI_CNN_MEANA_DIM2 | XI_CNN_MEANA_DIM3)

#define XI_CNN_MEANA_DIM1234  (XI_CNN_MEANA_DIM1 | XI_CNN_MEANA_DIM2 | XI_CNN_MEANA_DIM3 | XI_CNN_MEANA_DIM4)

typedef struct
{
  int32_t frameDim1;
  int32_t frameDim2;
  int32_t frameDim3;
  int32_t frameDim4;
  int32_t config;
  int32_t outMultiplier;
  int32_t outShift;
  int32_t zeroPtInput;    // Zero point offset of input data
  int32_t zeroPtOutput;   // Zero point offset of output data
} xi_cnn_meanA_params;

#define XI_CNN_MEANA_PARAMS_GET_FRAMEDIM1(x)     ((x)->frameDim1)
#define XI_CNN_MEANA_PARAMS_GET_FRAMEDIM2(x)     ((x)->frameDim2)
#define XI_CNN_MEANA_PARAMS_GET_FRAMEDIM3(x)     ((x)->frameDim3)
#define XI_CNN_MEANA_PARAMS_GET_FRAMEDIM4(x)     ((x)->frameDim4)

#define XI_CNN_MEANA_PARAMS_GET_CONFIG(x)        ((x)->config)
#define XI_CNN_MEANA_PARAMS_SET_FRAMEDIM1(x, v)  ((x)->frameDim1 = (v))
#define XI_CNN_MEANA_PARAMS_SET_FRAMEDIM2(x, v)  ((x)->frameDim2 = (v))
#define XI_CNN_MEANA_PARAMS_SET_FRAMEDIM3(x, v)  ((x)->frameDim3 = (v))
#define XI_CNN_MEANA_PARAMS_SET_FRAMEDIM4(x, v)  ((x)->frameDim4 = (v))

#define XI_CNN_MEANA_PARAMS_SET_CONFIG(x, v)     ((x)->config = (v))

#define XI_CNN_MEANA_PARAMS_GET_MULTIPLIER_OUT(x)     ((x)->outMultiplier)
#define XI_CNN_MEANA_PARAMS_SET_MULTIPLIER_OUT(x, v)  ((x)->outMultiplier = (v))
#define XI_CNN_MEANA_PARAMS_GET_SHIFT_OUT(x)          ((x)->outShift)
#define XI_CNN_MEANA_PARAMS_SET_SHIFT_OUT(x, v)       ((x)->outShift = (v))

#define XI_CNN_MEANA_PARAMS_GET_ZEROPTINPUT(x)        ((x)->zeroPtInput)
#define XI_CNN_MEANA_PARAMS_SET_ZEROPTINPUT(x, v)     ((x)->zeroPtInput = (v))
#define XI_CNN_MEANA_PARAMS_GET_ZEROPTOUTPUT(x)       ((x)->zeroPtOutput)
#define XI_CNN_MEANA_PARAMS_SET_ZEROPTOUTPUT(x, v)    ((x)->zeroPtOutput = (v))

typedef struct
{
  int32_t frameDim1;
  int32_t frameDim2;
  int32_t frameDim3;
  int32_t starts[3];
  int32_t stops[3];
  int32_t strides[3];
} xi_cnn_stridedsliceA_params;

#define XI_CNN_STRIDEDSLICEA_PARAMS_GET_FRAMEDIM1(x)          ((x)->frameDim1)
#define XI_CNN_STRIDEDSLICEA_PARAMS_GET_FRAMEDIM2(x)          ((x)->frameDim2)
#define XI_CNN_STRIDEDSLICEA_PARAMS_GET_FRAMEDIM3(x)          ((x)->frameDim3)
#define XI_CNN_STRIDEDSLICEA_PARAMS_GET_STARTINDEX(x, y)      ((x)->starts[y])
#define XI_CNN_STRIDEDSLICEA_PARAMS_GET_STOPINDEX(x, y)       ((x)->stops[y])
#define XI_CNN_STRIDEDSLICEA_PARAMS_GET_STRIDEINDEX(x, y)     ((x)->strides[y])

#define XI_CNN_STRIDEDSLICEA_PARAMS_SET_FRAMEDIM1(x, v)       ((x)->frameDim1 = (v))
#define XI_CNN_STRIDEDSLICEA_PARAMS_SET_FRAMEDIM2(x, v)       ((x)->frameDim2 = (v))
#define XI_CNN_STRIDEDSLICEA_PARAMS_SET_FRAMEDIM3(x, v)       ((x)->frameDim3 = (v))
#define XI_CNN_STRIDEDSLICEA_PARAMS_SET_STARTINDEX(x, y, v)   ((x)->starts[y] = (v))
#define XI_CNN_STRIDEDSLICEA_PARAMS_SET_STOPINDEX(x, y, v)    ((x)->stops[y] = (v))
#define XI_CNN_STRIDEDSLICEA_PARAMS_SET_STRIDEINDEX(x, y, v)  ((x)->strides[y] = (v))

typedef struct
{
  int32_t weights_zero_point;
  int32_t accm_multiplier;
  int32_t accm_shift;
}xi_cnna_lstmA_params;


#define XI_CNN_LSTMA_PARAMS_GET_ZEROPOINT_WEIGHTS(x)     ((x)->weights_zero_point)
#define XI_CNN_LSTMA_PARAMS_GET_ACCMULTIPLIER(x)         ((x)->accm_multiplier)
#define XI_CNN_LSTMA_PARAMS_GET_ACCSHIFT(x)              ((x)->accm_shift)

#define XI_CNN_LSTMA_PARAMS_SET_ZEROPOINT_WEIGHTS(x, v)  ((x)->weights_zero_point = (v))
#define XI_CNN_LSTMA_PARAMS_SET_ACCMULTIPLIER(x, v)      ((x)->accm_multiplier = (v))
#define XI_CNN_LSTMA_PARAMS_SET_ACCSHIFT(x, v)           ((x)->accm_shift = (v))


typedef struct
{
  int32_t inputZeroPoint;
  int32_t filterZeroPoint;
  int32_t outputZeroPoint;
  int32_t outputMultiplier;
  int32_t outputShift;
  int32_t outputActivationMin;
  int32_t outputActivationMax;
  int32_t strideWidth;
  int32_t strideHeight;
  int32_t inputOffsetLeft;
  int32_t inputOffsetTop;
  int32_t stridedPadLeft;
  int32_t stridedPadTop;
  int32_t stridedPadRight;
  int32_t stridedPadBottom;
} xi_cnn_deconvolveA_params;

#define XI_CNN_DECONVOLVEA_PARAMS_GET_INPUT_ZERO_POINT(x)       ((x)->inputZeroPoint)
#define XI_CNN_DECONVOLVEA_PARAMS_GET_FILTER_ZERO_POINT(x)      ((x)->filterZeroPoint)
#define XI_CNN_DECONVOLVEA_PARAMS_GET_OUTPUT_ZERO_POINT(x)      ((x)->outputZeroPoint)
#define XI_CNN_DECONVOLVEA_PARAMS_GET_OUTPUT_MULTIPLIER(x)      ((x)->outputMultiplier)
#define XI_CNN_DECONVOLVEA_PARAMS_GET_OUTPUT_SHIFT(x)           ((x)->outputShift)
#define XI_CNN_DECONVOLVEA_PARAMS_GET_MIN_VAL(x)                ((x)->outputActivationMin)
#define XI_CNN_DECONVOLVEA_PARAMS_GET_MAX_VAL(x)                ((x)->outputActivationMax)
#define XI_CNN_DECONVOLVEA_PARAMS_GET_STRIDE_WIDTH(x)           ((x)->strideWidth)
#define XI_CNN_DECONVOLVEA_PARAMS_GET_STRIDE_HEIGHT(x)          ((x)->strideHeight)
#define XI_CNN_DECONVOLVEA_PARAMS_GET_INPUT_OFFSET_LEFT(x)      ((x)->inputOffsetLeft)
#define XI_CNN_DECONVOLVEA_PARAMS_GET_INPUT_OFFSET_TOP(x)       ((x)->inputOffsetTop)
#define XI_CNN_DECONVOLVEA_PARAMS_GET_STRIDED_PAD_LEFT(x)       ((x)->stridedPadLeft)
#define XI_CNN_DECONVOLVEA_PARAMS_GET_STRIDED_PAD_TOP(x)        ((x)->stridedPadTop)
#define XI_CNN_DECONVOLVEA_PARAMS_GET_STRIDED_PAD_RIGHT(x)      ((x)->stridedPadRight)
#define XI_CNN_DECONVOLVEA_PARAMS_GET_STRIDED_PAD_BOTTOM(x)     ((x)->stridedPadBottom)

#define XI_CNN_DECONVOLVEA_PARAMS_SET_INPUT_ZERO_POINT(x, y)    ((x)->inputZeroPoint = (y))
#define XI_CNN_DECONVOLVEA_PARAMS_SET_FILTER_ZERO_POINT(x, y)   ((x)->filterZeroPoint = (y))
#define XI_CNN_DECONVOLVEA_PARAMS_SET_OUTPUT_ZERO_POINT(x, y)   ((x)->outputZeroPoint = (y))
#define XI_CNN_DECONVOLVEA_PARAMS_SET_OUTPUT_MULTIPLIER(x, y)   ((x)->outputMultiplier = (y))
#define XI_CNN_DECONVOLVEA_PARAMS_SET_OUTPUT_SHIFT(x, y)        ((x)->outputShift = (y))
#define XI_CNN_DECONVOLVEA_PARAMS_SET_MIN_VAL(x, y)             ((x)->outputActivationMin = (y))
#define XI_CNN_DECONVOLVEA_PARAMS_SET_MAX_VAL(x, y)             ((x)->outputActivationMax = (y))
#define XI_CNN_DECONVOLVEA_PARAMS_SET_STRIDE_WIDTH(x, y)        ((x)->strideWidth = (y))
#define XI_CNN_DECONVOLVEA_PARAMS_SET_STRIDE_HEIGHT(x, y)       ((x)->strideHeight = (y))
#define XI_CNN_DECONVOLVEA_PARAMS_SET_INPUT_OFFSET_LEFT(x, y)   ((x)->inputOffsetLeft = (y))
#define XI_CNN_DECONVOLVEA_PARAMS_SET_INPUT_OFFSET_TOP(x, y)    ((x)->inputOffsetTop = (y))
#define XI_CNN_DECONVOLVEA_PARAMS_SET_STRIDED_PAD_LEFT(x, y)    ((x)->stridedPadLeft = (y))
#define XI_CNN_DECONVOLVEA_PARAMS_SET_STRIDED_PAD_TOP(x, y)     ((x)->stridedPadTop = (y))
#define XI_CNN_DECONVOLVEA_PARAMS_SET_STRIDED_PAD_RIGHT(x, y)   ((x)->stridedPadRight = (y))
#define XI_CNN_DECONVOLVEA_PARAMS_SET_STRIDED_PAD_BOTTOM(x, y)  ((x)->stridedPadBottom = (y))

typedef struct
{
  int32_t inZeroPoint;
  int32_t alphaZeroPoint;
  int32_t outZeroPoint;
  int32_t posOutMultiplier;
  int32_t posOutShift;
  int32_t negOutMultiplier;
  int32_t negOutShift;
  int32_t minVal;
  int32_t maxVal;
} xi_cnn_preluA_params;

#define XI_CNN_PRELUA_PARAMS_GET_IN_ZERO_POINT(x)                  ((x)->inZeroPoint)
#define XI_CNN_PRELUA_PARAMS_GET_ALPHA_ZERO_POINT(x)               ((x)->alphaZeroPoint)
#define XI_CNN_PRELUA_PARAMS_GET_OUT_ZERO_POINT(x)                 ((x)->outZeroPoint)
#define XI_CNN_PRELUA_PARAMS_GET_POSITIVE_OUTPUT_MULTIPLIER(x)     ((x)->posOutMultiplier)
#define XI_CNN_PRELUA_PARAMS_GET_POSITIVE_OUTPUT_SHIFT(x)          ((x)->posOutShift)
#define XI_CNN_PRELUA_PARAMS_GET_NEGATIVE_OUTPUT_MULTIPLIER(x)     ((x)->negOutMultiplier)
#define XI_CNN_PRELUA_PARAMS_GET_NEGATIVE_OUTPUT_SHIFT(x)          ((x)->negOutShift)
#define XI_CNN_PRELUA_PARAMS_GET_ZERO_POINT(x)                     ((x)->inZeroPoint)
#define XI_CNN_PRELUA_PARAMS_GET_MIN_VAL(x)                        ((x)->minVal)
#define XI_CNN_PRELUA_PARAMS_GET_MAX_VAL(x)                        ((x)->maxVal)

#define XI_CNN_PRELUA_PARAMS_SET_IN_ZERO_POINT(x, y)               ((x)->inZeroPoint = (y))
#define XI_CNN_PRELUA_PARAMS_SET_ALPHA_ZERO_POINT(x, y)            ((x)->alphaZeroPoint = (y))
#define XI_CNN_PRELUA_PARAMS_SET_OUT_ZERO_POINT(x, y)              ((x)->outZeroPoint = (y))
#define XI_CNN_PRELUA_PARAMS_SET_POSITIVE_OUTPUT_MULTIPLIER(x, y)  ((x)->posOutMultiplier = (y))
#define XI_CNN_PRELUA_PARAMS_SET_POSITIVE_OUTPUT_SHIFT(x, y)       ((x)->posOutShift = (y))
#define XI_CNN_PRELUA_PARAMS_SET_NEGATIVE_OUTPUT_MULTIPLIER(x, y)  ((x)->negOutMultiplier = (y))
#define XI_CNN_PRELUA_PARAMS_SET_NEGATIVE_OUTPUT_SHIFT(x, y)       ((x)->negOutShift = (y))
#define XI_CNN_PRELUA_PARAMS_SET_ZERO_POINT(x, y)                  (x)->inZeroPoint = (y); (x)->outZeroPoint = (y);
#define XI_CNN_PRELUA_PARAMS_SET_MIN_VAL(x, y)                     ((x)->minVal = (y))
#define XI_CNN_PRELUA_PARAMS_SET_MAX_VAL(x, y)                     ((x)->maxVal = (y))

typedef struct
{
  uint8_t strideX;
  uint8_t strideY;
  uint8_t dilationX;
  uint8_t dilationY;
  int32_t inputOffset;
  int32_t coeffOffset;
  int32_t outputOffset;
  int32_t quantizedScale;
  int32_t outputShift;
  uint8_t flags;
  /* bit0 corresponds to Relu flag.
   * bit1 corresponds to leftEdgeFlag
   * bit2 corresponds to topEdgeFlag
   *  ----------------------------------------------------
   *  | bit7 - 3 | bit2        | bit1         | bit0      |
   *  |   unused | topEdgeFlag | leftEdgeFlag | Relu Flag |
   *  ----------------------------------------------------
   */
  int16_t reluMin;
  int16_t reluMax;
} xi_cnna_dilatedConv_params;

#define XI_CNNA_DILATED_CONV_GET_STRIDEX(x)               ((x)->strideX)
#define XI_CNNA_DILATED_CONV_GET_STRIDEY(x)               ((x)->strideY)
#define XI_CNNA_DILATED_CONV_GET_DILATIONX(x)             ((x)->dilationX)
#define XI_CNNA_DILATED_CONV_GET_DILATIONY(x)             ((x)->dilationY)
#define XI_CNNA_DILATED_CONV_GET_INPUT_OFFSET(x)          ((x)->inputOffset)
#define XI_CNNA_DILATED_CONV_GET_COEFF_OFFSET(x)          ((x)->coeffOffset)
#define XI_CNNA_DILATED_CONV_GET_OUTPUT_OFFSET(x)         ((x)->outputOffset)
#define XI_CNNA_DILATED_CONV_GET_OUTPUT_MULTIPLIER(x)     ((x)->quantizedScale)
#define XI_CNNA_DILATED_CONV_GET_OUTPUT_SHIFT(x)          ((x)->outputShift)
#define XI_CNNA_DILATED_CONV_GET_RELU_MIN(x)              ((x)->reluMin)
#define XI_CNNA_DILATED_CONV_GET_RELU_MAX(x)              ((x)->reluMax)
#define XI_CNNA_DILATED_CONV_GET_FLAGS(x)                 ((x)->flags)
#define XI_CNNA_DILATED_CONV_GET_FLAG_RELU(x)             ((x)->flags & CNN_CONV_FLAG_RELU)
#define XI_CNNA_DILATED_CONV_GET_FLAG_LEFTEDGE(x)         ((x)->flags & CNN_CONV_FLAG_LEFTEDGE)
#define XI_CNNA_DILATED_CONV_GET_FLAG_TOPEDGE(x)          ((x)->flags & CNN_CONV_FLAG_TOPEDGE)
#define XI_CNNA_DILATED_CONV_SET_STRIDEX(x, v)            ((x)->strideX = (v))
#define XI_CNNA_DILATED_CONV_SET_STRIDEY(x, v)            ((x)->strideY = (v))
#define XI_CNNA_DILATED_CONV_SET_DILATIONX(x, v)          ((x)->dilationX = (v))
#define XI_CNNA_DILATED_CONV_SET_DILATIONY(x, v)          ((x)->dilationY = (v))
#define XI_CNNA_DILATED_CONV_SET_INPUT_OFFSET(x, v)       ((x)->inputOffset = (v))
#define XI_CNNA_DILATED_CONV_SET_COEFF_OFFSET(x, v)       ((x)->coeffOffset = (v))
#define XI_CNNA_DILATED_CONV_SET_OUTPUT_OFFSET(x, v)      ((x)->outputOffset = (v))
#define XI_CNNA_DILATED_CONV_SET_OUTPUT_MULTIPLIER(x, v)  ((x)->quantizedScale = (v))
#define XI_CNNA_DILATED_CONV_SET_OUTPUT_SHIFT(x, v)       ((x)->outputShift = (v))
#define XI_CNNA_DILATED_CONV_SET_RELU_MIN(x, v)           ((x)->reluMin = (v))
#define XI_CNNA_DILATED_CONV_SET_RELU_MAX(x, v)           ((x)->reluMax = (v))
#define XI_CNNA_DILATED_CONV_SET_FLAGS(x, v)              ((x)->flags = (v))
#define XI_CNNA_DILATED_CONV_SET_FLAG_RELU(x)             ((x)->flags = ((x)->flags | CNN_CONV_FLAG_RELU))
#define XI_CNNA_DILATED_CONV_SET_FLAG_LEFTEDGE(x)         ((x)->flags = ((x)->flags | CNN_CONV_FLAG_LEFTEDGE))
#define XI_CNNA_DILATED_CONV_SET_FLAG_TOPEDGE(x)          ((x)->flags = ((x)->flags | CNN_CONV_FLAG_TOPEDGE))
#define XI_CNNA_DILATED_CONV_RESET_FLAG_RELU(x)           ((x)->flags = ((x)->flags & ~CNN_CONV_FLAG_RELU))
#define XI_CNNA_DILATED_CONV_RESET_FLAG_LEFTEDGE(x)       ((x)->flags = ((x)->flags & ~CNN_CONV_FLAG_LEFTEDGE))
#define XI_CNNA_DILATED_CONV_RESET_FLAG_TOPEDGE(x)        ((x)->flags = ((x)->flags & ~CNN_CONV_FLAG_TOPEDGE))

typedef struct
{
  uint8_t strideX;
  uint8_t strideY;
  uint8_t dilationX;
  uint8_t dilationY;
  uint8_t depth_multiplier;
  int32_t inputOffset;
  int32_t coeffOffset;
  int32_t outputOffset;
  int32_t quantizedScale;
  int32_t outputShift;
  uint8_t flags;
  /* bit0 corresponds to Relu flag.
   * bit1 corresponds to leftEdgeFlag
   * bit2 corresponds to topEdgeFlag
   *  ----------------------------------------------------
   *  | bit7 - 3 | bit2        | bit1         | bit0      |
   *  |   unused | topEdgeFlag | leftEdgeFlag | Relu Flag |
   *  ----------------------------------------------------
   */
  int16_t reluMin;
  int16_t reluMax;
} xi_cnna_depthwiseDilatedConv_params;

#define XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEX(x)               ((x)->strideX)
#define XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEY(x)               ((x)->strideY)
#define XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(x)             ((x)->dilationX)
#define XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(x)             ((x)->dilationY)
#define XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DEPTH_MULTIPLIER(x)      ((x)->depth_multiplier)
#define XI_CNNA_DEPTHWISE_DILATED_CONV_GET_INPUT_OFFSET(x)          ((x)->inputOffset)
#define XI_CNNA_DEPTHWISE_DILATED_CONV_GET_COEFF_OFFSET(x)          ((x)->coeffOffset)
#define XI_CNNA_DEPTHWISE_DILATED_CONV_GET_OUTPUT_OFFSET(x)         ((x)->outputOffset)
#define XI_CNNA_DEPTHWISE_DILATED_CONV_GET_OUTPUT_MULTIPLIER(x)     ((x)->quantizedScale)
#define XI_CNNA_DEPTHWISE_DILATED_CONV_GET_OUTPUT_SHIFT(x)          ((x)->outputShift)
#define XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(x)              ((x)->reluMin)
#define XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(x)              ((x)->reluMax)
#define XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAGS(x)                 ((x)->flags)
#define XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_RELU(x)             ((x)->flags & CNN_CONV_FLAG_RELU)
#define XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_LEFTEDGE(x)         ((x)->flags & CNN_CONV_FLAG_LEFTEDGE)
#define XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_TOPEDGE(x)          ((x)->flags & CNN_CONV_FLAG_TOPEDGE)
#define XI_CNNA_DEPTHWISE_DILATED_CONV_SET_STRIDEX(x, v)            ((x)->strideX = (v))
#define XI_CNNA_DEPTHWISE_DILATED_CONV_SET_STRIDEY(x, v)            ((x)->strideY = (v))
#define XI_CNNA_DEPTHWISE_DILATED_CONV_SET_DILATIONX(x, v)          ((x)->dilationX = (v))
#define XI_CNNA_DEPTHWISE_DILATED_CONV_SET_DILATIONY(x, v)          ((x)->dilationY = (v))
#define XI_CNNA_DEPTHWISE_DILATED_CONV_SET_DEPTH_MULTIPLIER(x, v)   ((x)->depth_multiplier = (v))
#define XI_CNNA_DEPTHWISE_DILATED_CONV_SET_INPUT_OFFSET(x, v)       ((x)->inputOffset = (v))
#define XI_CNNA_DEPTHWISE_DILATED_CONV_SET_COEFF_OFFSET(x, v)       ((x)->coeffOffset = (v))
#define XI_CNNA_DEPTHWISE_DILATED_CONV_SET_OUTPUT_OFFSET(x, v)      ((x)->outputOffset = (v))
#define XI_CNNA_DEPTHWISE_DILATED_CONV_SET_OUTPUT_MULTIPLIER(x, v)  ((x)->quantizedScale = (v))
#define XI_CNNA_DEPTHWISE_DILATED_CONV_SET_OUTPUT_SHIFT(x, v)       ((x)->outputShift = (v))
#define XI_CNNA_DEPTHWISE_DILATED_CONV_SET_RELU_MIN(x, v)           ((x)->reluMin = (v))
#define XI_CNNA_DEPTHWISE_DILATED_CONV_SET_RELU_MAX(x, v)           ((x)->reluMax = (v))
#define XI_CNNA_DEPTHWISE_DILATED_CONV_SET_FLAGS(x, v)              ((x)->flags = (v))
#define XI_CNNA_DEPTHWISE_DILATED_CONV_SET_FLAG_RELU(x)             ((x)->flags = ((x)->flags | CNN_CONV_FLAG_RELU))
#define XI_CNNA_DEPTHWISE_DILATED_CONV_SET_FLAG_LEFTEDGE(x)         ((x)->flags = ((x)->flags | CNN_CONV_FLAG_LEFTEDGE))
#define XI_CNNA_DEPTHWISE_DILATED_CONV_SET_FLAG_TOPEDGE(x)          ((x)->flags = ((x)->flags | CNN_CONV_FLAG_TOPEDGE))
#define XI_CNNA_DEPTHWISE_DILATED_CONV_RESET_FLAG_RELU(x)           ((x)->flags = ((x)->flags & ~CNN_CONV_FLAG_RELU))
#define XI_CNNA_DEPTHWISE_DILATED_CONV_RESET_FLAG_LEFTEDGE(x)       ((x)->flags = ((x)->flags & ~CNN_CONV_FLAG_LEFTEDGE))
#define XI_CNNA_DEPTHWISE_DILATED_CONV_RESET_FLAG_TOPEDGE(x)        ((x)->flags = ((x)->flags & ~CNN_CONV_FLAG_TOPEDGE))

typedef struct
{
  uint8_t  kernelWidth;           // Normalization window width
  uint8_t  kernelHeight;          // Normalization window height
  uint32_t sigmaScale;            // Factor used to scale the sum of squares of data under the normalization window
  uint8_t  sigmaScaleShift;       // Shift to map the scaled sum of squares to LUT index
  uint8_t  outputShift;           // Output shift
} xi_cnn_lrn_spatial_params;

typedef struct
{
  uint8_t  kernelDepth;           // Normalization window depth
  uint32_t sigmaScale;            // Factor used to scale the sum of squares of data under the normalization window
  uint8_t  sigmaScaleShift;       // Shift to map the scaled sum of squares to LUT index
  uint8_t  outputShift;           // Output shift
} xi_cnn_lrn_depth_params;

#define XI_CNN_LRN_GET_KERNELWIDTH(x)         ((x)->kernelWidth)
#define XI_CNN_LRN_SET_KERNELWIDTH(x, v)      ((x)->kernelWidth = (v))
#define XI_CNN_LRN_GET_KERNELHEIGHT(x)        ((x)->kernelHeight)
#define XI_CNN_LRN_SET_KERNELHEIGHT(x, v)     ((x)->kernelHeight = (v))
#define XI_CNN_LRN_GET_KERNELDEPTH(x)         ((x)->kernelDepth)
#define XI_CNN_LRN_SET_KERNELDEPTH(x, v)      ((x)->kernelDepth = (v))
#define XI_CNN_LRN_GET_SIGMASCALE(x)          ((x)->sigmaScale)
#define XI_CNN_LRN_SET_SIGMASCALE(x, v)       ((x)->sigmaScale = (v))
#define XI_CNN_LRN_GET_SIGMASCALESHIFT(x)     ((x)->sigmaScaleShift)
#define XI_CNN_LRN_SET_SIGMASCALESHIFT(x, v)  ((x)->sigmaScaleShift = (v))
#define XI_CNN_LRN_GET_OUTPUTSHIFT(x)         ((x)->outputShift)
#define XI_CNN_LRN_SET_OUTPUTSHIFT(x, v)      ((x)->outputShift = (v))


typedef struct
{
  uint8_t kernelWidth;
  uint8_t kernelHeight;

  uint8_t strideX;    //The number of points by which the pooling window
                      //is shifted along X direction.
  uint8_t strideY;    //The number of points by which the pooling window
  uint8_t edgeFlag;   // edgeFlag is applicable only for pooling with even kernel sizes. Least significant bit(LSB)
                      // of the flag represents whether minimum left edge size required for pooling should be
                      // greater than the minimum right edge size required. The bit adjacent to LSB decides whether
                      // minimum top edge size required should be greater than minimum bottom edge size.
  int32_t min_val;
  int32_t max_val;
} xi_cnn_pooling_params;

#define XI_CNN_POOLING_GET_MIN_VAL(x)            ((x)->min_val)
#define XI_CNN_POOLING_SET_MIN_VAL(x, v)         ((x)->min_val = (v))
#define XI_CNN_POOLING_GET_MAX_VAL(x)            ((x)->max_val)
#define XI_CNN_POOLING_SET_MAX_VAL(x, v)         ((x)->max_val = (v))
#define XI_CNN_POOLING_GET_KERNELWIDTH(x)        ((x)->kernelWidth)
#define XI_CNN_POOLING_SET_KERNELWIDTH(x, v)     ((x)->kernelWidth = (v))
#define XI_CNN_POOLING_GET_KERNELHEIGHT(x)       ((x)->kernelHeight)
#define XI_CNN_POOLING_SET_KERNELHEIGHT(x, v)    ((x)->kernelHeight = (v))
#define XI_CNN_POOLING_GET_STRIDE(x)             ((x)->strideX)
#define XI_CNN_POOLING_SET_STRIDE(x, v)          (x)->strideX = (v); (x)->strideY = (v);
#define XI_CNN_POOLING_GET_STRIDEX(x)            ((x)->strideX)
#define XI_CNN_POOLING_GET_STRIDEY(x)            ((x)->strideY)
#define XI_CNN_POOLING_SET_STRIDE_XY(x, v1, v2)  (x)->strideX = (v1); (x)->strideY = (v2);
#define XI_CNN_POOLING_GET_TOPEDGE_FLAG(x)       ((x)->edgeFlag & CNN_POOLING_TOPEDGE_FLAG)
#define XI_CNN_POOLING_SET_TOPEDGE_FLAG(x)       ((x)->edgeFlag = ((x)->edgeFlag | CNN_POOLING_TOPEDGE_FLAG))
#define XI_CNN_POOLING_RESET_TOPEDGE_FLAG(x)     ((x)->edgeFlag = ((x)->edgeFlag & ~CNN_POOLING_TOPEDGE_FLAG))
#define XI_CNN_POOLING_GET_LEFTEDGE_FLAG(x)      ((x)->edgeFlag & CNN_POOLING_LEFTEDGE_FLAG)
#define XI_CNN_POOLING_SET_LEFTEDGE_FLAG(x)      ((x)->edgeFlag = ((x)->edgeFlag | CNN_POOLING_LEFTEDGE_FLAG))
#define XI_CNN_POOLING_RESET_LEFTEDGE_FLAG(x)    ((x)->edgeFlag = ((x)->edgeFlag & ~CNN_POOLING_LEFTEDGE_FLAG))

typedef struct
{
  int32_t zero_pointInp1;  /* Zero point (a 32 bit integer, in range [0, 255]) for input1 tile */
  int32_t zero_pointInp2;  /* Zero point (a 32 bit integer, in range [0, 255]) for input2 tile */
  int32_t zero_pointOut;   /* Zero point (a 32 bit integer, in range [0, 255]) for output tile */
  int32_t multiplierInp1;  /* The input1 scaling factor                                        */
  int32_t multiplierInp2;  /* The input2 scaling factor                                        */
  int32_t multiplierOut;   /* The output scaling factor                                        */
  int32_t shiftInp1;       /* The input1 shifting factor                                       */
  int32_t shiftInp2;       /* The input2 shifting factor                                       */
  int32_t shiftOut;        /* The output shifting factor                                       */
  int32_t left_shift;      /* Fixed amount required for fixed point math better precision      */
} xi_cnn_eltwisemaxA_params;

#define XI_CNN_ELTWISEMAXA_GET_ZERO_POINT_IN1(x)     ((x)->zero_pointInp1)
#define XI_CNN_ELTWISEMAXA_GET_ZERO_POINT_IN2(x)     ((x)->zero_pointInp2)
#define XI_CNN_ELTWISEMAXA_GET_ZERO_POINT_OUT(x)     ((x)->zero_pointOut)
#define XI_CNN_ELTWISEMAXA_GET_MULTIPLIER_IN1(x)     ((x)->multiplierInp1)
#define XI_CNN_ELTWISEMAXA_GET_MULTIPLIER_IN2(x)     ((x)->multiplierInp2)
#define XI_CNN_ELTWISEMAXA_GET_MULTIPLIER_OUT(x)     ((x)->multiplierOut)
#define XI_CNN_ELTWISEMAXA_GET_SHIFT_IN1(x)          ((x)->shiftInp1)
#define XI_CNN_ELTWISEMAXA_GET_SHIFT_IN2(x)          ((x)->shiftInp2)
#define XI_CNN_ELTWISEMAXA_GET_SHIFT_OUT(x)          ((x)->shiftOut)
#define XI_CNN_ELTWISEMAXA_GET_LEFT_SHIFT(x)         ((x)->left_shift)

#define XI_CNN_ELTWISEMAXA_SET_ZERO_POINT_IN1(x, v)  ((x)->zero_pointInp1 = (v))
#define XI_CNN_ELTWISEMAXA_SET_ZERO_POINT_IN2(x, v)  ((x)->zero_pointInp2 = (v))
#define XI_CNN_ELTWISEMAXA_SET_ZERO_POINT_OUT(x, v)  ((x)->zero_pointOut  = (v))
#define XI_CNN_ELTWISEMAXA_SET_MULTIPLIER_IN1(x, v)  ((x)->multiplierInp1 = (v))
#define XI_CNN_ELTWISEMAXA_SET_MULTIPLIER_IN2(x, v)  ((x)->multiplierInp2 = (v))
#define XI_CNN_ELTWISEMAXA_SET_MULTIPLIER_OUT(x, v)  ((x)->multiplierOut  = (v))
#define XI_CNN_ELTWISEMAXA_SET_SHIFT_IN1(x, v)       ((x)->shiftInp1      = (v))
#define XI_CNN_ELTWISEMAXA_SET_SHIFT_IN2(x, v)       ((x)->shiftInp2      = (v))
#define XI_CNN_ELTWISEMAXA_SET_SHIFT_OUT(x, v)       ((x)->shiftOut       = (v))
#define XI_CNN_ELTWISEMAXA_SET_LEFT_SHIFT(x, v)      ((x)->left_shift     = (v))

typedef struct
{
  int32_t zero_pointInp1;  /* Zero point (a 32 bit integer, in range [0, 255]) for input1 tile */
  int32_t zero_pointInp2;  /* Zero point (a 32 bit integer, in range [0, 255]) for input2 tile */
  int32_t zero_pointOut;   /* Zero point (a 32 bit integer, in range [0, 255]) for output tile */
  int32_t multiplierInp1;  /* The input1 scaling factor                                        */
  int32_t multiplierInp2;  /* The input2 scaling factor                                        */
  int32_t multiplierOut;   /* The output scaling factor                                        */
  int32_t shiftInp1;       /* The input1 shifting factor                                       */
  int32_t shiftInp2;       /* The input2 shifting factor                                       */
  int32_t shiftOut;        /* The output shifting factor                                       */
  int32_t left_shift;      /* Fixed amount required for fixed point math better precision      */
} xi_cnn_eltwiseminA_params;

#define XI_CNN_ELTWISEMINA_GET_ZERO_POINT_IN1(x)     ((x)->zero_pointInp1)
#define XI_CNN_ELTWISEMINA_GET_ZERO_POINT_IN2(x)     ((x)->zero_pointInp2)
#define XI_CNN_ELTWISEMINA_GET_ZERO_POINT_OUT(x)     ((x)->zero_pointOut)
#define XI_CNN_ELTWISEMINA_GET_MULTIPLIER_IN1(x)     ((x)->multiplierInp1)
#define XI_CNN_ELTWISEMINA_GET_MULTIPLIER_IN2(x)     ((x)->multiplierInp2)
#define XI_CNN_ELTWISEMINA_GET_MULTIPLIER_OUT(x)     ((x)->multiplierOut)
#define XI_CNN_ELTWISEMINA_GET_SHIFT_IN1(x)          ((x)->shiftInp1)
#define XI_CNN_ELTWISEMINA_GET_SHIFT_IN2(x)          ((x)->shiftInp2)
#define XI_CNN_ELTWISEMINA_GET_SHIFT_OUT(x)          ((x)->shiftOut)
#define XI_CNN_ELTWISEMINA_GET_LEFT_SHIFT(x)         ((x)->left_shift)

#define XI_CNN_ELTWISEMINA_SET_ZERO_POINT_IN1(x, v)  ((x)->zero_pointInp1 = (v))
#define XI_CNN_ELTWISEMINA_SET_ZERO_POINT_IN2(x, v)  ((x)->zero_pointInp2 = (v))
#define XI_CNN_ELTWISEMINA_SET_ZERO_POINT_OUT(x, v)  ((x)->zero_pointOut  = (v))
#define XI_CNN_ELTWISEMINA_SET_MULTIPLIER_IN1(x, v)  ((x)->multiplierInp1 = (v))
#define XI_CNN_ELTWISEMINA_SET_MULTIPLIER_IN2(x, v)  ((x)->multiplierInp2 = (v))
#define XI_CNN_ELTWISEMINA_SET_MULTIPLIER_OUT(x, v)  ((x)->multiplierOut  = (v))
#define XI_CNN_ELTWISEMINA_SET_SHIFT_IN1(x, v)       ((x)->shiftInp1      = (v))
#define XI_CNN_ELTWISEMINA_SET_SHIFT_IN2(x, v)       ((x)->shiftInp2      = (v))
#define XI_CNN_ELTWISEMINA_SET_SHIFT_OUT(x, v)       ((x)->shiftOut       = (v))
#define XI_CNN_ELTWISEMINA_SET_LEFT_SHIFT(x, v)      ((x)->left_shift     = (v))

typedef struct
{
  int32_t zero_pointInp1;
  int32_t zero_pointInp2;
  int32_t multiplierInp1;
  int32_t multiplierInp2;
  int32_t shiftInp1;
  int32_t shiftInp2;
} xi_cnn_eltwiseCompareAParams;
#define XI_CNN_ELTWISECOMPA_GET_ZERO_POINT_IN1(x)     ((x)->zero_pointInp1)
#define XI_CNN_ELTWISECOMPA_GET_ZERO_POINT_IN2(x)     ((x)->zero_pointInp2)
#define XI_CNN_ELTWISECOMPA_GET_MULTIPLIER_IN1(x)     ((x)->multiplierInp1)
#define XI_CNN_ELTWISECOMPA_GET_MULTIPLIER_IN2(x)     ((x)->multiplierInp2)
#define XI_CNN_ELTWISECOMPA_GET_SHIFT_IN1(x)          ((x)->shiftInp1)
#define XI_CNN_ELTWISECOMPA_GET_SHIFT_IN2(x)          ((x)->shiftInp2)
#define XI_CNN_ELTWISECOMPA_SET_ZERO_POINT_IN1(x, v)  ((x)->zero_pointInp1 = (v))
#define XI_CNN_ELTWISECOMPA_SET_ZERO_POINT_IN2(x, v)  ((x)->zero_pointInp2 = (v))
#define XI_CNN_ELTWISECOMPA_SET_MULTIPLIER_IN1(x, v)  ((x)->multiplierInp1 = (v))
#define XI_CNN_ELTWISECOMPA_SET_MULTIPLIER_IN2(x, v)  ((x)->multiplierInp2 = (v))
#define XI_CNN_ELTWISECOMPA_SET_SHIFT_IN1(x, v)       ((x)->shiftInp1 = (v))
#define XI_CNN_ELTWISECOMPA_SET_SHIFT_IN2(x, v)       ((x)->shiftInp2 = (v))

typedef struct
{
  uint8_t kernelWidth;
  uint8_t kernelHeight;
  int32_t zeroPtInput;    // Zero point offset of input data
  int32_t zeroPtOutput;   // Zero point offset of input data
  int32_t min_val;
  int32_t max_val;
  int32_t multiplierOut;        //output multiplier
  int32_t shiftOut;
  int32_t left_shift;
  uint8_t strideX;    //The number of points by which the pooling window
                      //is shifted along X direction.
  uint8_t strideY;    //The number of points by which the pooling window
  uint8_t edgeFlag;   // edgeFlag is applicable only for pooling with even kernel sizes. Least significant bit(LSB)
                      // of the flag represents whether minimum left edge size required for pooling should be
                      // greater than the minimum right edge size required. The bit adjacent to LSB decides whether
                      // minimum top edge size required should be greater than minimum bottom edge size.
} xi_cnn_avgpoolA_params;

#define XI_CNN_AVGPOOLA_GET_MIN_VAL(x)            ((x)->min_val)
#define XI_CNN_AVGPOOLA_SET_MIN_VAL(x, v)         ((x)->min_val = (v))
#define XI_CNN_AVGPOOLA_GET_MAX_VAL(x)            ((x)->max_val)
#define XI_CNN_AVGPOOLA_SET_MAX_VAL(x, v)         ((x)->max_val = (v))
#define XI_CNN_AVGPOOLA_GET_ZEROPTINPUT(x)        ((x)->zeroPtInput)
#define XI_CNN_AVGPOOLA_SET_ZEROPTINPUT(x, v)     ((x)->zeroPtInput = (v))
#define XI_CNN_AVGPOOLA_GET_ZEROPTOUTPUT(x)       ((x)->zeroPtOutput)
#define XI_CNN_AVGPOOLA_SET_ZEROPTOUTPUT(x, v)    ((x)->zeroPtOutput = (v))
#define XI_CNN_AVGPOOLA_GET_KERNELWIDTH(x)        ((x)->kernelWidth)
#define XI_CNN_AVGPOOLA_SET_KERNELWIDTH(x, v)     ((x)->kernelWidth = (v))
#define XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(x)       ((x)->kernelHeight)
#define XI_CNN_AVGPOOLA_SET_KERNELHEIGHT(x, v)    ((x)->kernelHeight = (v))
#define XI_CNN_AVGPOOLA_GET_STRIDE(x)             ((x)->strideX)
#define XI_CNN_AVGPOOLA_SET_STRIDE(x, v)          (x)->strideX = (v); (x)->strideY = (v);
#define XI_CNN_AVGPOOLA_GET_STRIDEX(x)            ((x)->strideX)
#define XI_CNN_AVGPOOLA_GET_STRIDEY(x)            ((x)->strideY)
#define XI_CNN_AVGPOOLA_SET_STRIDE_XY(x, v1, v2)  (x)->strideX = (v1); (x)->strideY = (v2);
#define XI_CNN_AVGPOOLA_GET_TOPEDGE_FLAG(x)       ((x)->edgeFlag & CNN_AVGPOOLA_TOPEDGE_FLAG)
#define XI_CNN_AVGPOOLA_SET_TOPEDGE_FLAG(x)       ((x)->edgeFlag = ((x)->edgeFlag | CNN_AVGPOOLA_TOPEDGE_FLAG))
#define XI_CNN_AVGPOOLA_RESET_TOPEDGE_FLAG(x)     ((x)->edgeFlag = ((x)->edgeFlag & ~CNN_AVGPOOLA_TOPEDGE_FLAG))
#define XI_CNN_AVGPOOLA_GET_LEFTEDGE_FLAG(x)      ((x)->edgeFlag & CNN_AVGPOOLA_LEFTEDGE_FLAG)
#define XI_CNN_AVGPOOLA_SET_LEFTEDGE_FLAG(x)      ((x)->edgeFlag = ((x)->edgeFlag | CNN_AVGPOOLA_LEFTEDGE_FLAG))
#define XI_CNN_AVGPOOLA_RESET_LEFTEDGE_FLAG(x)    ((x)->edgeFlag = ((x)->edgeFlag & ~CNN_AVGPOOLA_LEFTEDGE_FLAG))
#define XI_CNN_AVGPOOLA_SET_SHIFT_OUT(x, v)       ((x)->shiftOut = (v))
#define XI_CNN_AVGPOOLA_GET_SHIFT_OUT(x)          ((x)->shiftOut)
#define XI_CNN_AVGPOOLA_SET_LEFT_SHIFT(x, v)      ((x)->left_shift = (v))
#define XI_CNN_AVGPOOLA_GET_LEFT_SHIFT(x)         ((x)->left_shift)
#define XI_CNN_AVGPOOLA_SET_MULTIPLIER_OUT(x, v)  ((x)->multiplierOut = (v))
#define XI_CNN_AVGPOOLA_GET_MULTIPLIER_OUT(x)     ((x)->multiplierOut)


typedef struct
{
  uint8_t kernelWidth;
  uint8_t kernelHeight;
  int32_t zeroPtInput;    // Zero point offset of input data
  int32_t zeroPtOutput;   // Zero point offset of input data
  int32_t min_val;
  int32_t max_val;

  uint8_t strideX;     //The number of points by which the pooling window
                       //is shifted along X direction.
  uint8_t strideY;     //The number of points by which the pooling window
  int32_t frameWidth;  //total frame width
  int32_t frameHeight; //total frame height
  uint8_t edgeFlag;    // edgeFlag is applicable only for pooling with even kernel sizes. Least significant bit(LSB)
                       // of the flag represents whether minimum left edge size required for pooling should be
                       // greater than the minimum right edge size required. The bit adjacent to LSB decides whether
                       // minimum top edge size required should be greater than minimum bottom edge size.
  int32_t outMultiplier;
  int32_t outShift;
  int32_t leftShift;
} xi_cnn_l2pool2D_params;

#define XI_CNN_L2POOL2D_GET_MIN_VAL(x)            ((x)->min_val)
#define XI_CNN_L2POOL2D_SET_MIN_VAL(x, v)         ((x)->min_val = (v))
#define XI_CNN_L2POOL2D_GET_MAX_VAL(x)            ((x)->max_val)
#define XI_CNN_L2POOL2D_SET_MAX_VAL(x, v)         ((x)->max_val = (v))
#define XI_CNN_L2POOL2D_GET_ZEROPTINPUT(x)        ((x)->zeroPtInput)
#define XI_CNN_L2POOL2D_SET_ZEROPTINPUT(x, v)     ((x)->zeroPtInput = (v))
#define XI_CNN_L2POOL2D_GET_ZEROPTOUTPUT(x)       ((x)->zeroPtOutput)
#define XI_CNN_L2POOL2D_SET_ZEROPTOUTPUT(x, v)    ((x)->zeroPtOutput = (v))
#define XI_CNN_L2POOL2D_GET_KERNELWIDTH(x)        ((x)->kernelWidth)
#define XI_CNN_L2POOL2D_SET_KERNELWIDTH(x, v)     ((x)->kernelWidth = (v))
#define XI_CNN_L2POOL2D_GET_KERNELHEIGHT(x)       ((x)->kernelHeight)
#define XI_CNN_L2POOL2D_SET_KERNELHEIGHT(x, v)    ((x)->kernelHeight = (v))
#define XI_CNN_L2POOL2D_GET_STRIDE(x)             ((x)->strideX)
#define XI_CNN_L2POOL2D_SET_STRIDE(x, v)          (x)->strideX = (v); (x)->strideY = (v);
#define XI_CNN_L2POOL2D_GET_STRIDEX(x)            ((x)->strideX)
#define XI_CNN_L2POOL2D_GET_STRIDEY(x)            ((x)->strideY)
#define XI_CNN_L2POOL2D_SET_STRIDE_XY(x, v1, v2)  (x)->strideX = (v1); (x)->strideY = (v2);
#define XI_CNN_L2POOL2D_GET_TOPEDGE_FLAG(x)       ((x)->edgeFlag & CNN_L2POOL2D_TOPEDGE_FLAG)
#define XI_CNN_L2POOL2D_SET_TOPEDGE_FLAG(x)       ((x)->edgeFlag = ((x)->edgeFlag | CNN_L2POOL2D_TOPEDGE_FLAG))
#define XI_CNN_L2POOL2D_RESET_TOPEDGE_FLAG(x)     ((x)->edgeFlag = ((x)->edgeFlag & ~CNN_L2POOL2D_TOPEDGE_FLAG))
#define XI_CNN_L2POOL2D_GET_LEFTEDGE_FLAG(x)      ((x)->edgeFlag & CNN_L2POOL2D_LEFTEDGE_FLAG)
#define XI_CNN_L2POOL2D_SET_LEFTEDGE_FLAG(x)      ((x)->edgeFlag = ((x)->edgeFlag | CNN_L2POOL2D_LEFTEDGE_FLAG))
#define XI_CNN_L2POOL2D_RESET_LEFTEDGE_FLAG(x)    ((x)->edgeFlag = ((x)->edgeFlag & ~CNN_L2POOL2D_LEFTEDGE_FLAG))
#define XI_CNN_L2POOL2D_GET_EDGE_FLAG(x)          ((x)->edgeFlag)
#define XI_CNN_L2POOL2D_SET_EDGE_FLAG(x, v)       ((x)->edgeFlag = (v))
#define XI_CNN_L2POOL2D_GET_FRAME_WIDTH(x)        ((x)->frameWidth)
#define XI_CNN_L2POOL2D_GET_FRAME_HEIGHT(x)       ((x)->frameHeight)
#define XI_CNN_L2POOL2D_SET_FRAME_WIDTH(x, v)     ((x)->frameWidth = (v))
#define XI_CNN_L2POOL2D_SET_FRAME_HEIGHT(x, v)    ((x)->frameHeight = (v))
#define XI_CNN_L2POOL2D_GET_MULTIPLIER_OUT(x)     ((x)->outMultiplier)
#define XI_CNN_L2POOL2D_SET_MULTIPLIER_OUT(x, v)  ((x)->outMultiplier = (v))
#define XI_CNN_L2POOL2D_GET_SHIFT_OUT(x)          ((x)->outShift)
#define XI_CNN_L2POOL2D_SET_SHIFT_OUT(x, v)       ((x)->outShift = (v))
#define XI_CNN_L2POOL2D_GET_LEFT_SHIFT(x)         ((x)->leftShift)
#define XI_CNN_L2POOL2D_SET_LEFT_SHIFT(x, v)      ((x)->leftShift = (v))


typedef struct
{
  uint8_t kernelWidth;
  uint8_t kernelHeight;
  int32_t zeroPtInput;    // Zero point offset of input data
  int32_t zeroPtOutput;   // Zero point offset of input data
  int32_t min_val;
  int32_t max_val;
  int32_t multiplierOut;        //output multiplier
  int32_t shiftOut;
  int32_t left_shift;
  uint8_t strideX;    //The number of points by which the pooling window
                      //is shifted along X direction.
  uint8_t strideY;    //The number of points by which the pooling window
  uint8_t edgeFlag;   // edgeFlag is applicable only for pooling with even kernel sizes. Least significant bit(LSB)
                      // of the flag represents whether minimum left edge size required for pooling should be
                      // greater than the minimum right edge size required. The bit adjacent to LSB decides whether
                      // minimum top edge size required should be greater than minimum bottom edge size.
  int32_t frameWidth;
  int32_t frameHeight;
} xi_cnn_maxpoolA_params;

#define XI_CNN_MAXPOOLA_GET_MIN_VAL(x)            ((x)->min_val)
#define XI_CNN_MAXPOOLA_SET_MIN_VAL(x, v)         ((x)->min_val = (v))
#define XI_CNN_MAXPOOLA_GET_MAX_VAL(x)            ((x)->max_val)
#define XI_CNN_MAXPOOLA_SET_MAX_VAL(x, v)         ((x)->max_val = (v))
#define XI_CNN_MAXPOOLA_GET_ZEROPTINPUT(x)        ((x)->zeroPtInput)
#define XI_CNN_MAXPOOLA_SET_ZEROPTINPUT(x, v)     ((x)->zeroPtInput = (v))
#define XI_CNN_MAXPOOLA_GET_ZEROPTOUTPUT(x)       ((x)->zeroPtOutput)
#define XI_CNN_MAXPOOLA_SET_ZEROPTOUTPUT(x, v)    ((x)->zeroPtOutput = (v))
#define XI_CNN_MAXPOOLA_GET_KERNELWIDTH(x)        ((x)->kernelWidth)
#define XI_CNN_MAXPOOLA_SET_KERNELWIDTH(x, v)     ((x)->kernelWidth = (v))
#define XI_CNN_MAXPOOLA_GET_KERNELHEIGHT(x)       ((x)->kernelHeight)
#define XI_CNN_MAXPOOLA_SET_KERNELHEIGHT(x, v)    ((x)->kernelHeight = (v))
#define XI_CNN_MAXPOOLA_GET_STRIDE(x)             ((x)->strideX)
#define XI_CNN_MAXPOOLA_SET_STRIDE(x, v)          (x)->strideX = (v); (x)->strideY = (v);
#define XI_CNN_MAXPOOLA_GET_STRIDEX(x)            ((x)->strideX)
#define XI_CNN_MAXPOOLA_GET_STRIDEY(x)            ((x)->strideY)
#define XI_CNN_MAXPOOLA_SET_STRIDE_XY(x, v1, v2)  (x)->strideX = (v1); (x)->strideY = (v2);
#define XI_CNN_MAXPOOLA_GET_TOPEDGE_FLAG(x)       ((x)->edgeFlag & CNN_MAXPOOLA_TOPEDGE_FLAG)
#define XI_CNN_MAXPOOLA_SET_TOPEDGE_FLAG(x)       ((x)->edgeFlag = ((x)->edgeFlag | CNN_MAXPOOLA_TOPEDGE_FLAG))
#define XI_CNN_MAXPOOLA_RESET_TOPEDGE_FLAG(x)     ((x)->edgeFlag = ((x)->edgeFlag & ~CNN_MAXPOOLA_TOPEDGE_FLAG))
#define XI_CNN_MAXPOOLA_GET_LEFTEDGE_FLAG(x)      ((x)->edgeFlag & CNN_MAXPOOLA_LEFTEDGE_FLAG)
#define XI_CNN_MAXPOOLA_SET_LEFTEDGE_FLAG(x)      ((x)->edgeFlag = ((x)->edgeFlag | CNN_MAXPOOLA_LEFTEDGE_FLAG))
#define XI_CNN_MAXPOOLA_RESET_LEFTEDGE_FLAG(x)    ((x)->edgeFlag = ((x)->edgeFlag & ~CNN_MAXPOOLA_LEFTEDGE_FLAG))
#define XI_CNN_MAXPOOLA_SET_SHIFT_OUT(x, v)       ((x)->shiftOut = (v))
#define XI_CNN_MAXPOOLA_GET_SHIFT_OUT(x)          ((x)->shiftOut)
#define XI_CNN_MAXPOOLA_SET_LEFT_SHIFT(x, v)      ((x)->left_shift = (v))
#define XI_CNN_MAXPOOLA_GET_LEFT_SHIFT(x)         ((x)->left_shift)
#define XI_CNN_MAXPOOLA_SET_MULTIPLIER_OUT(x, v)  ((x)->multiplierOut = (v))
#define XI_CNN_MAXPOOLA_GET_MULTIPLIER_OUT(x)     ((x)->multiplierOut)
#define XI_CNN_MAXPOOLA_SET_FRAME_WIDTH(x, v)     ((x)->frameWidth = (v))
#define XI_CNN_MAXPOOLA_GET_FRAME_WIDTH(x)        ((x)->frameWidth)
#define XI_CNN_MAXPOOLA_SET_FRAME_HEIGHT(x, v)    ((x)->frameHeight = (v))
#define XI_CNN_MAXPOOLA_GET_FRAME_HEIGHT(x)       ((x)->frameHeight)

typedef struct
{
  uint8_t kernelWidth;    // width of pooling window
  uint8_t kernelHeight;   // height of pooling window
  int32_t zeroPtInput;    // Zero point offset of input data
  int32_t zeroPtOutput;   // Zero point offset of output data
  int32_t min_val;        // Output clamp boundary.
  int32_t max_val;        // Output clamp boundary.
  int32_t multiplierOut;  // Output multiplier
  int32_t shiftOut;   // Output shift parameter
  int32_t left_shift;
  uint8_t strideX;    //The number of points by which the pooling window
                      //is shifted along X direction.
  uint8_t strideY;    //The number of points by which the pooling window
  uint8_t edgeFlag;   // edgeFlag is applicable only for pooling with even kernel sizes. Least significant bit(LSB)
                      // of the flag represents whether minimum left edge size required for pooling should be
                      // greater than the minimum right edge size required. The bit adjacent to LSB decides whether
                      // minimum top edge size required should be greater than minimum bottom edge size.
  int32_t frameWidth;
  int32_t frameHeight;
} xi_cnn_minpoolA_params;

#define XI_CNN_MINPOOLA_GET_MIN_VAL(x)            ((x)->min_val)
#define XI_CNN_MINPOOLA_SET_MIN_VAL(x, v)         ((x)->min_val = (v))
#define XI_CNN_MINPOOLA_GET_MAX_VAL(x)            ((x)->max_val)
#define XI_CNN_MINPOOLA_SET_MAX_VAL(x, v)         ((x)->max_val = (v))
#define XI_CNN_MINPOOLA_GET_ZEROPTINPUT(x)        ((x)->zeroPtInput)
#define XI_CNN_MINPOOLA_SET_ZEROPTINPUT(x, v)     ((x)->zeroPtInput = (v))
#define XI_CNN_MINPOOLA_GET_ZEROPTOUTPUT(x)       ((x)->zeroPtOutput)
#define XI_CNN_MINPOOLA_SET_ZEROPTOUTPUT(x, v)    ((x)->zeroPtOutput = (v))
#define XI_CNN_MINPOOLA_GET_KERNELWIDTH(x)        ((x)->kernelWidth)
#define XI_CNN_MINPOOLA_SET_KERNELWIDTH(x, v)     ((x)->kernelWidth = (v))
#define XI_CNN_MINPOOLA_GET_KERNELHEIGHT(x)       ((x)->kernelHeight)
#define XI_CNN_MINPOOLA_SET_KERNELHEIGHT(x, v)    ((x)->kernelHeight = (v))
#define XI_CNN_MINPOOLA_GET_STRIDE(x)             ((x)->strideX)
#define XI_CNN_MINPOOLA_SET_STRIDE(x, v)          (x)->strideX = (v); (x)->strideY = (v);
#define XI_CNN_MINPOOLA_GET_STRIDEX(x)            ((x)->strideX)
#define XI_CNN_MINPOOLA_GET_STRIDEY(x)            ((x)->strideY)
#define XI_CNN_MINPOOLA_SET_STRIDE_XY(x, v1, v2)  (x)->strideX = (v1); (x)->strideY = (v2);
#define XI_CNN_MINPOOLA_GET_TOPEDGE_FLAG(x)       ((x)->edgeFlag & CNN_MINPOOLA_TOPEDGE_FLAG)
#define XI_CNN_MINPOOLA_SET_TOPEDGE_FLAG(x)       ((x)->edgeFlag = ((x)->edgeFlag | CNN_MINPOOLA_TOPEDGE_FLAG))
#define XI_CNN_MINPOOLA_RESET_TOPEDGE_FLAG(x)     ((x)->edgeFlag = ((x)->edgeFlag & ~CNN_MINPOOLA_TOPEDGE_FLAG))
#define XI_CNN_MINPOOLA_GET_LEFTEDGE_FLAG(x)      ((x)->edgeFlag & CNN_MINPOOLA_LEFTEDGE_FLAG)
#define XI_CNN_MINPOOLA_SET_LEFTEDGE_FLAG(x)      ((x)->edgeFlag = ((x)->edgeFlag | CNN_MINPOOLA_LEFTEDGE_FLAG))
#define XI_CNN_MINPOOLA_RESET_LEFTEDGE_FLAG(x)    ((x)->edgeFlag = ((x)->edgeFlag & ~CNN_MINPOOLA_LEFTEDGE_FLAG))
#define XI_CNN_MINPOOLA_SET_SHIFT_OUT(x, v)       ((x)->shiftOut = (v))
#define XI_CNN_MINPOOLA_GET_SHIFT_OUT(x)          ((x)->shiftOut)
#define XI_CNN_MINPOOLA_SET_LEFT_SHIFT(x, v)      ((x)->left_shift = (v))
#define XI_CNN_MINPOOLA_GET_LEFT_SHIFT(x)         ((x)->left_shift)
#define XI_CNN_MINPOOLA_SET_MULTIPLIER_OUT(x, v)  ((x)->multiplierOut = (v))
#define XI_CNN_MINPOOLA_GET_MULTIPLIER_OUT(x)     ((x)->multiplierOut)
#define XI_CNN_MINPOOLA_SET_FRAME_WIDTH(x, v)     ((x)->frameWidth = (v))
#define XI_CNN_MINPOOLA_GET_FRAME_WIDTH(x)        ((x)->frameWidth)
#define XI_CNN_MINPOOLA_SET_FRAME_HEIGHT(x, v)    ((x)->frameHeight = (v))
#define XI_CNN_MINPOOLA_GET_FRAME_HEIGHT(x)       ((x)->frameHeight)

typedef struct
{
  uint8_t outputShift;      /* No. of output bits to be right shifted. */
  uint8_t qFactorOutput;    /* No. of bits scaling applied to the reciprocal of the sum of exp(x)*/
} xi_cnn_softmax_params;

#define XI_CNN_SOFTMAX_GET_OUTPUTSHIFT(x)       ((x)->outputShift)
#define XI_CNN_SOFTMAX_SET_OUTPUTSHIFT(x, v)    ((x)->outputShift = (v))
#define XI_CNN_SOFTMAX_GET_QFACTOROUTPUT(x)     ((x)->qFactorOutput)
#define XI_CNN_SOFTMAX_SET_QFACTOROUTPUT(x, v)  ((x)->qFactorOutput = (v))


typedef struct
{
  uint16_t input1Scale;  /* Scaling factor for 1st input */
  uint16_t input2Scale;  /* Scaling factor for 2nd input */
  uint8_t  accumShift;   /* Accumulator Shift to bring data to 16b after scaling and addition */
  uint16_t outputScale;  /* Scaling factor for Output */
  uint8_t  outputShift;  /* Shift value to bring the final sum to 8b */
}xi_cnn_eltwise_params;

#define XI_CNN_ELTWISE_GET_INPUT1SCALE(x)     ((x)->input1Scale)
#define XI_CNN_ELTWISE_SET_INPUT1SCALE(x, v)  ((x)->input1Scale = (v))
#define XI_CNN_ELTWISE_GET_INPUT2SCALE(x)     ((x)->input2Scale)
#define XI_CNN_ELTWISE_SET_INPUT2SCALE(x, v)  ((x)->input2Scale = (v))
#define XI_CNN_ELTWISE_GET_ACCUMSHIFT(x)      ((x)->accumShift)
#define XI_CNN_ELTWISE_SET_ACCUMSHIFT(x, v)   ((x)->accumShift = (v))
#define XI_CNN_ELTWISE_GET_OUTPUTSCALE(x)     ((x)->outputScale)
#define XI_CNN_ELTWISE_SET_OUTPUTSCALE(x, v)  ((x)->outputScale = (v))
#define XI_CNN_ELTWISE_GET_OUTPUTSHIFT(x)     ((x)->outputShift)
#define XI_CNN_ELTWISE_SET_OUTPUTSHIFT(x, v)  ((x)->outputShift = (v))


typedef struct
{
  uint8_t strideX;         // Amount by which coefficient mask is shifted along width of input data.
  uint8_t strideY;         // Amount by which coefficient mask is shifted along height of input data.
  uint8_t dilationX;       // Dilation along width of coefficient data
  uint8_t dilationY;       // Dilation along height of coefficient data
  int16_t zeroPtInput;     // Zero point offset of input data
  int16_t zeroPtCoeff;     // Zero point offset of coefficient data
  int16_t zeroPtOutput;    // Zero point offset of output data
  int32_t quantizedScale;  // Quantized scale factor applied to the output
  int32_t outputShift;     // right shift bits which is associated with quantized scale factor applied to the output
  uint8_t flags;           // enableRelu, edge flag;
  /* bit0 corresponds to Relu flag.
   * bit1 corresponds to leftEdgeFlag
   * bit2 corresponds to topEdgeFlag
   *
   *  --------------------------------------------------------------------------
   *  | bit7  | bit6 | bit5 | bit4 | bit3 |bit2        | bit1       | bit0     |
   *  |unused |unused|unused|unused|unused| topEdgeFlag|leftEdgeFlag|Relu Flag |
   *  --------------------------------------------------------------------------
   */
  int16_t reluMin;        // minimum threshold value to control the output range to implement ReLU/ReLU1/ReLU6
  int16_t reluMax;        // maximum threshold value to control the output range to implement ReLU/ReLU1/ReLU6
} xi_cnna_conv_params;

#define XI_CNNA_CONV_GET_STRIDE(x)             ((x)->strideX)
#define XI_CNNA_CONV_SET_STRIDE(x, v)          (x)->strideX = (v); (x)->strideY = (v);
#define XI_CNNA_CONV_GET_STRIDEX(x)            ((x)->strideX)
#define XI_CNNA_CONV_SET_STRIDEX(x, v)         ((x)->strideX = (v))
#define XI_CNNA_CONV_GET_STRIDEY(x)            ((x)->strideY)
#define XI_CNNA_CONV_SET_STRIDEY(x, v)         ((x)->strideY = (v))
#define XI_CNNA_CONV_SET_STRIDE_XY(x, v1, v2)  (x)->strideX = (v1); (x)->strideY = (v2);
#define XI_CNNA_CONV_GET_DILATION(x)           ((x)->dilationX)
#define XI_CNNA_CONV_SET_DILATION(x, v)        (x)->dilationX = (v); (x)->dilationY = (v);
#define XI_CNNA_CONV_GET_DILATIONX(x)          ((x)->dilationX)
#define XI_CNNA_CONV_SET_DILATIONX(x, v)       ((x)->dilationX = (v))
#define XI_CNNA_CONV_GET_DILATIONY(x)          ((x)->dilationY)
#define XI_CNNA_CONV_SET_DILATIONY(x, v)       ((x)->dilationY = (v))
#define XI_CNNA_CONV_SET_DILATION_XY(x, v1, v2) (x)->dilationX = (v1); (x)->dilationY = (v2);
#define XI_CNNA_CONV_GET_ZEROPT_INPUT(x)      ((x)->zeroPtInput)
#define XI_CNNA_CONV_SET_ZEROPT_INPUT(x, v)   ((x)->zeroPtInput = (v))
#define XI_CNNA_CONV_GET_ZEROPT_COEFF(x)      ((x)->zeroPtCoeff)
#define XI_CNNA_CONV_SET_ZEROPT_COEFF(x, v)   ((x)->zeroPtCoeff = (v))
#define XI_CNNA_CONV_GET_ZEROPT_OUTPUT(x)     ((x)->zeroPtOutput)
#define XI_CNNA_CONV_SET_ZEROPT_OUTPUT(x, v)  ((x)->zeroPtOutput = (v))
#define XI_CNNA_CONV_GET_QUANT_SCALE(x)       ((x)->quantizedScale)
#define XI_CNNA_CONV_SET_QUANT_SCALE(x, v)    ((x)->quantizedScale = (v))
#define XI_CNNA_CONV_GET_OUTPUT_SHIFT(x)      ((x)->outputShift)
#define XI_CNNA_CONV_SET_OUTPUT_SHIFT(x, v)   ((x)->outputShift = (v))
#define XI_CNNA_CONV_GET_FLAGS(x)             ((x)->flags)
#define XI_CNNA_CONV_SET_FLAGS(x, v)          ((x)->flags = (v))
#define XI_CNNA_CONV_GET_FLAG_RELU(x)         ((x)->flags & CNN_CONV_FLAG_RELU)
#define XI_CNNA_CONV_SET_FLAG_RELU(x)         ((x)->flags = ((x)->flags | CNN_CONV_FLAG_RELU))
#define XI_CNNA_CONV_RESET_FLAG_RELU(x)       ((x)->flags = ((x)->flags & ~CNN_CONV_FLAG_RELU))
#define XI_CNNA_CONV_GET_FLAG_LEFTEDGE(x)     ((x)->flags & CNN_CONV_FLAG_LEFTEDGE)
#define XI_CNNA_CONV_SET_FLAG_LEFTEDGE(x)     ((x)->flags = ((x)->flags | CNN_CONV_FLAG_LEFTEDGE))
#define XI_CNNA_CONV_RESET_FLAG_LEFTEDGE(x)   ((x)->flags = ((x)->flags & ~CNN_CONV_FLAG_LEFTEDGE))
#define XI_CNNA_CONV_GET_FLAG_TOPEDGE(x)      ((x)->flags & CNN_CONV_FLAG_TOPEDGE)
#define XI_CNNA_CONV_SET_FLAG_TOPEDGE(x)      ((x)->flags = ((x)->flags | CNN_CONV_FLAG_TOPEDGE))
#define XI_CNNA_CONV_RESET_FLAG_TOPEDGE(x)    ((x)->flags = ((x)->flags & ~CNN_CONV_FLAG_TOPEDGE))
#define XI_CNNA_CONV_GET_RELUMIN(x)           ((x)->reluMin)
#define XI_CNNA_CONV_SET_RELUMIN(x, v)        ((x)->reluMin = (v))
#define XI_CNNA_CONV_GET_RELUMAX(x)           ((x)->reluMax)
#define XI_CNNA_CONV_SET_RELUMAX(x, v)        ((x)->reluMax = (v))


typedef struct
{
  int32_t zero_pointInp1;                      //Zero point for input 1 in quantized scale
  int32_t zero_pointInp2;                      //Zero point for input 2 in quantized scale
  int32_t zero_pointOut;                       //Zero point for output in quantized scale
  int32_t multiplierInp1;                      //Scale multiplier for input 1
  int32_t multiplierInp2;                      //Scale multiplier for input 2
  int32_t multiplierOut;                       //Scale multiplier for output
  int32_t shiftInp1;                           //Shift value as per multiplier for input 1
  int32_t shiftInp2;                           //Shift value as per multiplier for input 2
  int32_t shiftOut;                            //Shift value as per multiplier for output
  int32_t min_val;                             //Min value of clamp limit
  int32_t max_val;                             //Max value of clamp limit
  int32_t left_shift;                          //Shift value for scaled fixed point computation
} xi_cnn_addA_params;

#define XI_CNN_ADDA_GET_ZERO_POINT_IN1(x)     ((x)->zero_pointInp1)
#define XI_CNN_ADDA_GET_ZERO_POINT_IN2(x)     ((x)->zero_pointInp2)
#define XI_CNN_ADDA_GET_ZERO_POINT_OUT(x)     ((x)->zero_pointOut)
#define XI_CNN_ADDA_GET_MULTIPLIER_IN1(x)     ((x)->multiplierInp1)
#define XI_CNN_ADDA_GET_MULTIPLIER_IN2(x)     ((x)->multiplierInp2)
#define XI_CNN_ADDA_GET_MULTIPLIER_OUT(x)     ((x)->multiplierOut)
#define XI_CNN_ADDA_GET_SHIFT_IN1(x)          ((x)->shiftInp1)
#define XI_CNN_ADDA_GET_SHIFT_IN2(x)          ((x)->shiftInp2)
#define XI_CNN_ADDA_GET_SHIFT_OUT(x)          ((x)->shiftOut)
#define XI_CNN_ADDA_GET_MIN_VAL(x)            ((x)->min_val)
#define XI_CNN_ADDA_GET_MAX_VAL(x)            ((x)->max_val)
#define XI_CNN_ADDA_GET_LEFT_SHIFT(x)         ((x)->left_shift)

#define XI_CNN_ADDA_SET_ZERO_POINT_IN1(x, v)  ((x)->zero_pointInp1 = (v))
#define XI_CNN_ADDA_SET_ZERO_POINT_IN2(x, v)  ((x)->zero_pointInp2 = (v))
#define XI_CNN_ADDA_SET_ZERO_POINT_OUT(x, v)  ((x)->zero_pointOut = (v))
#define XI_CNN_ADDA_SET_MULTIPLIER_IN1(x, v)  ((x)->multiplierInp1 = (v))
#define XI_CNN_ADDA_SET_MULTIPLIER_IN2(x, v)  ((x)->multiplierInp2 = (v))
#define XI_CNN_ADDA_SET_MULTIPLIER_OUT(x, v)  ((x)->multiplierOut = (v))
#define XI_CNN_ADDA_SET_SHIFT_IN1(x, v)       ((x)->shiftInp1 = (v))
#define XI_CNN_ADDA_SET_SHIFT_IN2(x, v)       ((x)->shiftInp2 = (v))
#define XI_CNN_ADDA_SET_SHIFT_OUT(x, v)       ((x)->shiftOut = (v))
#define XI_CNN_ADDA_SET_MIN_VAL(x, v)         ((x)->min_val = (v))
#define XI_CNN_ADDA_SET_MAX_VAL(x, v)         ((x)->max_val = (v))
#define XI_CNN_ADDA_SET_LEFT_SHIFT(x, v)      ((x)->left_shift = (v))


typedef struct
{
  int32_t zero_pointInp1;                       //Zero point for input 1 in quantized scale
  int32_t zero_pointInp2;                       //Zero point for input 2 in quantized scale
  int32_t zero_pointOut;                        //Zero point for output in quantized scale
  int32_t multiplierInp1;                       //Scale multiplier for input 1
  int32_t multiplierInp2;                       //Scale multiplier for input 2
  int32_t multiplierOut;                        //Scale multiplier for output
  int32_t shiftInp1;                            //Shift value as per multiplier for input 1
  int32_t shiftInp2;                            //Shift value as per multiplier for input 2
  int32_t shiftOut;                             //Shift value as per multiplier for output
  int32_t min_val;                              //Min value of clamp limit
  int32_t max_val;                              //Max value of clamp limit
  int32_t left_shift;                           //Shift value for scaled fixed point computation
} xi_cnn_subA_params;

#define XI_CNN_SUBA_GET_ZERO_POINT_IN1(x)     ((x)->zero_pointInp1)
#define XI_CNN_SUBA_GET_ZERO_POINT_IN2(x)     ((x)->zero_pointInp2)
#define XI_CNN_SUBA_GET_ZERO_POINT_OUT(x)     ((x)->zero_pointOut)
#define XI_CNN_SUBA_GET_MULTIPLIER_IN1(x)     ((x)->multiplierInp1)
#define XI_CNN_SUBA_GET_MULTIPLIER_IN2(x)     ((x)->multiplierInp2)
#define XI_CNN_SUBA_GET_MULTIPLIER_OUT(x)     ((x)->multiplierOut)
#define XI_CNN_SUBA_GET_SHIFT_IN1(x)          ((x)->shiftInp1)
#define XI_CNN_SUBA_GET_SHIFT_IN2(x)          ((x)->shiftInp2)
#define XI_CNN_SUBA_GET_SHIFT_OUT(x)          ((x)->shiftOut)
#define XI_CNN_SUBA_GET_MIN_VAL(x)            ((x)->min_val)
#define XI_CNN_SUBA_GET_MAX_VAL(x)            ((x)->max_val)
#define XI_CNN_SUBA_GET_LEFT_SHIFT(x)         ((x)->left_shift)

#define XI_CNN_SUBA_SET_ZERO_POINT_IN1(x, v)  ((x)->zero_pointInp1 = (v))
#define XI_CNN_SUBA_SET_ZERO_POINT_IN2(x, v)  ((x)->zero_pointInp2 = (v))
#define XI_CNN_SUBA_SET_ZERO_POINT_OUT(x, v)  ((x)->zero_pointOut = (v))
#define XI_CNN_SUBA_SET_MULTIPLIER_IN1(x, v)  ((x)->multiplierInp1 = (v))
#define XI_CNN_SUBA_SET_MULTIPLIER_IN2(x, v)  ((x)->multiplierInp2 = (v))
#define XI_CNN_SUBA_SET_MULTIPLIER_OUT(x, v)  ((x)->multiplierOut = (v))
#define XI_CNN_SUBA_SET_SHIFT_IN1(x, v)       ((x)->shiftInp1 = (v))
#define XI_CNN_SUBA_SET_SHIFT_IN2(x, v)       ((x)->shiftInp2 = (v))
#define XI_CNN_SUBA_SET_SHIFT_OUT(x, v)       ((x)->shiftOut = (v))
#define XI_CNN_SUBA_SET_MIN_VAL(x, v)         ((x)->min_val = (v))
#define XI_CNN_SUBA_SET_MAX_VAL(x, v)         ((x)->max_val = (v))
#define XI_CNN_SUBA_SET_LEFT_SHIFT(x, v)     ((x)->left_shift = (v))


typedef struct
{
  int32_t zero_pointInp1;
  int32_t zero_pointInp2;
  int32_t zero_pointOut;
  int32_t multiplierOut;
  int32_t shiftOut;
  int32_t min_val;
  int32_t max_val;
} xi_cnn_mulA_params;


#define XI_CNN_MULA_GET_ZERO_POINT_IN1(x)     ((x)->zero_pointInp1)
#define XI_CNN_MULA_GET_ZERO_POINT_IN2(x)     ((x)->zero_pointInp2)
#define XI_CNN_MULA_GET_ZERO_POINT_OUT(x)     ((x)->zero_pointOut)
#define XI_CNN_MULA_GET_MULTIPLIER_OUT(x)     ((x)->multiplierOut)
#define XI_CNN_MULA_GET_SHIFT_OUT(x)          ((x)->shiftOut)
#define XI_CNN_MULA_GET_MIN_VAL(x)            ((x)->min_val)
#define XI_CNN_MULA_GET_MAX_VAL(x)            ((x)->max_val)

#define XI_CNN_MULA_SET_ZERO_POINT_IN1(x, v)  ((x)->zero_pointInp1 = (v))
#define XI_CNN_MULA_SET_ZERO_POINT_IN2(x, v)  ((x)->zero_pointInp2 = (v))
#define XI_CNN_MULA_SET_ZERO_POINT_OUT(x, v)  ((x)->zero_pointOut = (v))
#define XI_CNN_MULA_SET_MULTIPLIER_OUT(x, v)  ((x)->multiplierOut = (v))
#define XI_CNN_MULA_SET_SHIFT_OUT(x, v)       ((x)->shiftOut = (v))
#define XI_CNN_MULA_SET_MIN_VAL(x, v)         ((x)->min_val = (v))
#define XI_CNN_MULA_SET_MAX_VAL(x, v)         ((x)->max_val = (v))

typedef struct
{
  int32_t order1;
  int32_t order2;
  int32_t order3;
  //Each of the order takes either of 0, 1, 2 values without repetition.
  //(order1, order2, order3) = (0, 1, 2) indicates the output order is same as that of input order
  //(order1, order2, order3) = (1, 0, 2) indicates the first and second dimensions of input tile will be flipped (or transposed) in the output tile.
} xi_cnn_permuteA3D_params;

#define XI_CNN_PERMUTEA3D_GET_ORDER1(x)     ((x)->order1)
#define XI_CNN_PERMUTEA3D_GET_ORDER2(x)     ((x)->order2)
#define XI_CNN_PERMUTEA3D_GET_ORDER3(x)     ((x)->order3)
#define XI_CNN_PERMUTEA3D_SET_ORDER1(x, v)  ((x)->order1 = (v))
#define XI_CNN_PERMUTEA3D_SET_ORDER2(x, v)  ((x)->order2 = (v))
#define XI_CNN_PERMUTEA3D_SET_ORDER3(x, v)  ((x)->order3 = (v))

typedef struct
{
  int32_t order1;
  int32_t order2;
  int32_t order3;
  int32_t order4;
} xi_cnn_permuteA4D_params;

#define XI_CNN_PERMUTEA4D_GET_ORDER1(x)     ((x)->order1)
#define XI_CNN_PERMUTEA4D_GET_ORDER2(x)     ((x)->order2)
#define XI_CNN_PERMUTEA4D_GET_ORDER3(x)     ((x)->order3)
#define XI_CNN_PERMUTEA4D_GET_ORDER4(x)     ((x)->order4)
#define XI_CNN_PERMUTEA4D_SET_ORDER1(x, v)  ((x)->order1 = (v))
#define XI_CNN_PERMUTEA4D_SET_ORDER2(x, v)  ((x)->order2 = (v))
#define XI_CNN_PERMUTEA4D_SET_ORDER3(x, v)  ((x)->order3 = (v))
#define XI_CNN_PERMUTEA4D_SET_ORDER4(x, v)  ((x)->order4 = (v))

typedef struct
{
  int32_t zero_pointInp;
  int32_t zero_pointOut;
  int32_t multiplierOut;
  int32_t shiftOut;
  int32_t left_shift;
  int32_t min_val;
  int32_t max_val;
  uint8_t ScaleFlag;
} xi_cnn_reQuantizeA_params;

#define XI_CNN_REQUANTIZEA_GET_ZERO_POINT_IN(x)     ((x)->zero_pointInp)
#define XI_CNN_REQUANTIZEA_GET_ZERO_POINT_OUT(x)    ((x)->zero_pointOut)
#define XI_CNN_REQUANTIZEA_GET_MULTIPLIER_OUT(x)    ((x)->multiplierOut)
#define XI_CNN_REQUANTIZEA_GET_SHIFT_OUT(x)         ((x)->shiftOut)
#define XI_CNN_REQUANTIZEA_GET_LEFT_SHIFT(x)        ((x)->left_shift)
#define XI_CNN_REQUANTIZEA_GET_MIN_VAL(x)           ((x)->min_val)
#define XI_CNN_REQUANTIZEA_GET_MAX_VAL(x)           ((x)->max_val)
#define XI_CNN_REQUANTIZEA_GET_SCALE_FLAG(x)        ((x)->ScaleFlag)

#define XI_CNN_REQUANTIZEA_SET_ZERO_POINT_IN(x, v)  ((x)->zero_pointInp = (v))
#define XI_CNN_REQUANTIZEA_SET_ZERO_POINT_OUT(x, v) ((x)->zero_pointOut = (v))
#define XI_CNN_REQUANTIZEA_SET_MULTIPLIER_OUT(x, v) ((x)->multiplierOut = (v))
#define XI_CNN_REQUANTIZEA_SET_SHIFT_OUT(x, v)      ((x)->shiftOut = (v))
#define XI_CNN_REQUANTIZEA_SET_LEFT_SHIFT(x, v)     ((x)->left_shift = (v))
#define XI_CNN_REQUANTIZEA_SET_MIN_VAL(x, v)        ((x)->min_val = (v))
#define XI_CNN_REQUANTIZEA_SET_MAX_VAL(x, v)        ((x)->max_val = (v))
#define XI_CNN_REQUANTIZEA_SET_SCALE_FLAG(x,v)      ((x)->ScaleFlag = (v))

typedef struct
{
  uint32_t widthScale;             // Multiplicative spatial scale factor to translate ROI coords from their
                                    // input scale to the scale used when pooling.Represented in Q31
  uint32_t heightScale;
  int32_t  sampling_ratioX;         //Number of sampling points used to compute the output, set to 0 for an
                                    // adaptive number of ceil(roi_width / out_width)
  int32_t  sampling_ratioY;         //Number of sampling points used to compute the output, set to 0 for
                                    // an adaptive number of ceil(roi_height / out_height)
  uint32_t oneByPooledHeightScale;  //Reciprocal of pooledHeight represented in U32 range
  uint32_t oneByPooledWidthScale;   //Reciprocal of pooledWidth represented in U32 range
  int32_t  zero_pointIn;            //Zero point for input 1 in quantized scale
  int32_t  zero_pointOut;           //Zero point for output in quantized scale
  int32_t  multiplierScale;         //Scale multiplier for output
  int32_t  multiplierShift;         //Shift value as per multiplier for output
} xi_cnn_roi_align_params;

#define XI_CNN_ROI_ALIGN_GET_ZERO_POINT_IN(x)                   ((x)->zero_pointIn)
#define XI_CNN_ROI_ALIGN_GET_ZERO_POINT_OUT(x)                  ((x)->zero_pointOut)
#define XI_CNN_ROI_ALIGN_GET_MULTIPLIER_OUT(x)                  ((x)->multiplierScale)
#define XI_CNN_ROI_ALIGN_GET_SHIFT_IN1(x)                       ((x)->shiftInp1)

#define XI_CNN_ROI_ALIGN_GET_SHIFT_OUT(x)                       ((x)->multiplierShift)
#define XI_CNN_ROI_ALIGN_SET_ZERO_POINT_IN(x, v)                ((x)->zero_pointIn = (v))
#define XI_CNN_ROI_ALIGN_SET_ZERO_POINT_OUT(x, v)               ((x)->zero_pointOut = (v))

#define XI_CNN_ROI_ALIGN_SET_MULTIPLIER_OUT(x, v)               ((x)->multiplierScale = (v))
#define XI_CNN_ROI_ALIGN_SET_SHIFT_OUT(x, v)                    ((x)->multiplierShift = (v))
#define XI_CNN_ROI_ALIGN_GET_WIDTH_SCALE(x)                     ((x)->widthScale)
#define XI_CNN_ROI_ALIGN_SET_WIDTH_SCALE(x, v)                  ((x)->widthScale = (v))
#define XI_CNN_ROI_ALIGN_GET_HEIGHT_SCALE(x)                    ((x)->heightScale)
#define XI_CNN_ROI_ALIGN_SET_HEIGHT_SCALE(x, v)                 ((x)->heightScale = (v))
#define XI_CNN_ROI_ALIGN_GET_ONE_BY_POOLED_WIDTH_SCALE(x)       ((x)->oneByPooledWidthScale)
#define XI_CNN_ROI_ALIGN_SET_ONE_BY_POOLED_WIDTH_SCALE(x, v)    ((x)->oneByPooledWidthScale = (v))
#define XI_CNN_ROI_ALIGN_GET_ONE_BY_POOLED_HEIGHT_SCALE(x)      ((x)->oneByPooledHeightScale)
#define XI_CNN_ROI_ALIGN_SET_ONE_BY_POOLED_HEIGHT_SCALE(x, v)   ((x)->oneByPooledHeightScale = (v))
#define XI_CNN_ROI_ALIGN_GET_SAMPLING_RATIO_X(x)                ((x)->sampling_ratioX)
#define XI_CNN_ROI_ALIGN_SET_SAMPLING_RATIO_X(x, v)             ((x)->sampling_ratioX = (v))
#define XI_CNN_ROI_ALIGN_GET_SAMPLING_RATIO_Y(x)                ((x)->sampling_ratioY)
#define XI_CNN_ROI_ALIGN_SET_SAMPLING_RATIO_Y(x, v)             ((x)->sampling_ratioY = (v))

typedef struct
{
  int32_t zero_pointInp;  //Zero Point of input
  int32_t zero_pointOut;  //Zero Point of output
  int32_t multiplierOut;  //Output multiplier
  int32_t shiftOut;       //Output Shift
  int32_t left_shift;     //The upscale parameter to ensure multiplierOut is always less than 1.
  int32_t min_val;        //Output clamp lower boundary.
  int32_t max_val;        //Output clamp upper boundary.
} xi_cnn_AbsA_params;

#define XI_CNN_ABSA_GET_ZERO_POINT_IN(x)     ((x)->zero_pointInp)
#define XI_CNN_ABSA_GET_ZERO_POINT_OUT(x)    ((x)->zero_pointOut)
#define XI_CNN_ABSA_GET_MULTIPLIER_OUT(x)    ((x)->multiplierOut)
#define XI_CNN_ABSA_GET_SHIFT_OUT(x)         ((x)->shiftOut)
#define XI_CNN_ABSA_GET_LEFT_SHIFT(x)        ((x)->left_shift)
#define XI_CNN_ABSA_GET_MIN_VAL(x)           ((x)->min_val)
#define XI_CNN_ABSA_GET_MAX_VAL(x)           ((x)->max_val)
#define XI_CNN_ABSA_SET_ZERO_POINT_IN(x, v)  ((x)->zero_pointInp = (v))
#define XI_CNN_ABSA_SET_ZERO_POINT_OUT(x, v) ((x)->zero_pointOut = (v))
#define XI_CNN_ABSA_SET_MULTIPLIER_OUT(x, v) ((x)->multiplierOut = (v))
#define XI_CNN_ABSA_SET_SHIFT_OUT(x, v)      ((x)->shiftOut = (v))
#define XI_CNN_ABSA_SET_LEFT_SHIFT(x, v)     ((x)->left_shift = (v))
#define XI_CNN_ABSA_SET_MIN_VAL(x, v)        ((x)->min_val = (v))
#define XI_CNN_ABSA_SET_MAX_VAL(x, v)        ((x)->max_val = (v))

typedef struct
{
  uint32_t groups;       /* Input Groups */
  uint8_t config; /* The direction along which shuffle operation needs to be performed,
                  can take values 1,2,3,4. 1 -> Operation along dimension 1, 2 -> Operation
                  along dimension 2, 3 -> Operation along dimension 3, 4 -> Operation along dimension 4*/
}xi_cnn_shuffleA_params;

#define XI_CNN_SHUFFLE_GET_INTERLEAVEGROUPS(x)      ((x)->groups)
#define XI_CNN_SHUFFLE_SET_INTERLEAVEGROUPS(x, v)   ((x)->groups = (v))
#define XI_CNN_SHUFFLEA_GET_CONFIG(x)     ((x)->config)
#define XI_CNN_SHUFFLEA_SET_CONFIG(x,v)   ((x)->config = (v))
#define XI_CNN_SHUFFLEA_DIM1 1
#define XI_CNN_SHUFFLEA_DIM2 2
#define XI_CNN_SHUFFLEA_DIM3 3
#define XI_CNN_SHUFFLEA_DIM4 4

typedef struct
{
  int32_t inputZeroPoint;            //offset for centering the u8 into signed ranged
  int32_t inputRangeRadius;          // defines the input radius for defining lower and upper limits
  int32_t inputMultiplier;           // significand
  int32_t inputLeftShift;            // exponent part
  int32_t outputZeroPoint;           //offset for centering the u8 into signed ranged
  int32_t posOutputMultiplier;       // significand
  int32_t posOutputShift;            // exponent part
  int32_t negOutputMultiplier;       // significand
  int32_t negOutputShift;            // exponent part
  int32_t leftShift;
} xi_cnn_ELUA_params;

#define XI_CNN_ELUA_PARAMS_GET_INPUT_ZERO_POINT(x)         ((x)->inputZeroPoint)
#define XI_CNN_ELUA_PARAMS_GET_INPUT_RANGE_RADIUS(x)       ((x)->inputRangeRadius)
#define XI_CNN_ELUA_PARAMS_GET_INPUT_MULTIPLIER(x)         ((x)->inputMultiplier)
#define XI_CNN_ELUA_PARAMS_GET_INPUT_LEFT_SHIFT(x)         ((x)->inputLeftShift)
#define XI_CNN_ELUA_PARAMS_GET_OUTPUT_ZERO_POINT(x)        ((x)->outputZeroPoint)
#define XI_CNN_ELUA_PARAMS_GET_POS_OUTPUT_MULTIPLIER(x)    ((x)->posOutputMultiplier)
#define XI_CNN_ELUA_PARAMS_GET_POS_OUTPUT_SHIFT(x)         ((x)->posOutputShift)
#define XI_CNN_ELUA_PARAMS_GET_NEG_OUTPUT_MULTIPLIER(x)    ((x)->negOutputMultiplier)
#define XI_CNN_ELUA_PARAMS_GET_NEG_OUTPUT_SHIFT(x)         ((x)->negOutputShift)
#define XI_CNN_ELUA_PARAMS_GET_LEFT_SHIFT(x)               ((x)->leftShift)

#define XI_CNN_ELUA_PARAMS_SET_INPUT_ZERO_POINT(x, v)      ((x)->inputZeroPoint = (v))
#define XI_CNN_ELUA_PARAMS_SET_INPUT_RANGE_RADIUS(x, v)    ((x)->inputRangeRadius = (v))
#define XI_CNN_ELUA_PARAMS_SET_INPUT_MULTIPLIER(x, v)      ((x)->inputMultiplier = (v))
#define XI_CNN_ELUA_PARAMS_SET_INPUT_LEFT_SHIFT(x, v)      ((x)->inputLeftShift = (v))
#define XI_CNN_ELUA_PARAMS_SET_OUTPUT_ZERO_POINT(x, v)     ((x)->outputZeroPoint = (v))
#define XI_CNN_ELUA_PARAMS_SET_POS_OUTPUT_MULTIPLIER(x, v) ((x)->posOutputMultiplier = (v))
#define XI_CNN_ELUA_PARAMS_SET_POS_OUTPUT_SHIFT(x, v)      ((x)->posOutputShift = (v))
#define XI_CNN_ELUA_PARAMS_SET_NEG_OUTPUT_MULTIPLIER(x, v) ((x)->negOutputMultiplier = (v))
#define XI_CNN_ELUA_PARAMS_SET_NEG_OUTPUT_SHIFT(x, v)      ((x)->negOutputShift = (v))
#define XI_CNN_ELUA_PARAMS_SET_LEFT_SHIFT(x, v)            ((x)->leftShift = (v))

typedef struct
{
  uint8_t config; /* The direction along which Max/Min need to be computed, can take values 1,2,3,4.
                  * 1 -> Operation along dimension 1, 2 -> Operation along dimension 2,
                    3 -> Operation along dimension 3, 4 -> Operation along dimension 4.*/
}xi_cnn_ArgMinMaxA_params;

#define XI_CNN_ARGMINMAXA_GET_CONFIG(x)     ((x)->config)
#define XI_CNN_ARGMINMAXA_SET_CONFIG(x,v)   ((x)->config = (v))

#define XI_CNN_ARGMINMAXA_DIM1     (0x1)
#define XI_CNN_ARGMINMAXA_DIM2     (0x2)
#define XI_CNN_ARGMINMAXA_DIM3     (0x3)
#define XI_CNN_ARGMINMAXA_DIM4     (0x4)

typedef struct
{
  uint8_t config; /* The direction along which gather operation needs to be performed, can take values 1,2,3.
                   * 1 -> Operation along dimension 1, 2 -> Operation along dimension 2, 3 -> Operation along dimension 3 */
}xi_cnn_GatherA_params;

#define XI_CNN_GATHERA_GET_CONFIG(x)     ((x)->config)
#define XI_CNN_GATHERA_SET_CONFIG(x,v)   ((x)->config = (v))

#define XI_CNN_GATHERA_DIM1     (0x1)
#define XI_CNN_GATHERA_DIM2     (0x2)
#define XI_CNN_GATHERA_DIM3     (0x3)

typedef struct
{
  uint8_t config;                    // Determines reduction across particular dimensions
  uint8_t tileFlag;                  // Determines whether the given tile is first tile or not
                                     // 1-> first/first-last tile, 0-> intermediate or final tile
}xi_cnn_reduceA_params;

#define XI_CNN_REDUCE_GET_CONFIG(x)                     ((x)->config)
#define XI_CNN_REDUCE_GET_TILEFLAG(x)                   ((x)->tileFlag)

#define XI_CNN_REDUCE_SET_CONFIG(x,v)                   ((x)->config = v)
#define XI_CNN_REDUCE_SET_TILEFLAG(x,v)                 ((x)->tileFlag = v)

#define XI_CNN_REDUCE_DIM1     (0x1)
#define XI_CNN_REDUCE_DIM2     (0x2)
#define XI_CNN_REDUCE_DIM3     (0x4)
#define XI_CNN_REDUCE_DIM4     (0x8)

#define XI_CNN_REDUCE_DIM12    (XI_CNN_REDUCE_DIM1 | XI_CNN_REDUCE_DIM2)
#define XI_CNN_REDUCE_DIM13    (XI_CNN_REDUCE_DIM1 | XI_CNN_REDUCE_DIM3)
#define XI_CNN_REDUCE_DIM14    (XI_CNN_REDUCE_DIM1 | XI_CNN_REDUCE_DIM4)

#define XI_CNN_REDUCE_DIM23    (XI_CNN_REDUCE_DIM2 | XI_CNN_REDUCE_DIM3)
#define XI_CNN_REDUCE_DIM24    (XI_CNN_REDUCE_DIM2 | XI_CNN_REDUCE_DIM4)

#define XI_CNN_REDUCE_DIM34    (XI_CNN_REDUCE_DIM3 | XI_CNN_REDUCE_DIM4)

#define XI_CNN_REDUCE_DIM123   (XI_CNN_REDUCE_DIM1 | XI_CNN_REDUCE_DIM2 | XI_CNN_REDUCE_DIM3)
#define XI_CNN_REDUCE_DIM124   (XI_CNN_REDUCE_DIM1 | XI_CNN_REDUCE_DIM2 | XI_CNN_REDUCE_DIM4)
#define XI_CNN_REDUCE_DIM134   (XI_CNN_REDUCE_DIM1 | XI_CNN_REDUCE_DIM3 | XI_CNN_REDUCE_DIM4)

#define XI_CNN_REDUCE_DIM234   (XI_CNN_REDUCE_DIM2 | XI_CNN_REDUCE_DIM3 | XI_CNN_REDUCE_DIM4)

#define XI_CNN_REDUCE_DIM1234   (XI_CNN_REDUCE_DIM1 | XI_CNN_REDUCE_DIM2 | XI_CNN_REDUCE_DIM3 | XI_CNN_REDUCE_DIM4)

#define XI_CNN_REDUCE_FIRST_TILE  1

typedef struct
{
  uint8_t config;               // Determines reduction across particular dimensions
  uint8_t tileFlag;             // Determines whether the given tile is first tile or not
                                // 0-> intermediate tile, 1-> first tile, 2-> final tile and 3-> first-last tile
  int32_t zeroPtInput;          // Zero point offset of input data
  int32_t zeroPtOutput;         // Zero point offset of output data
  int32_t multiplierOut;        // Output multiplier
  int32_t shiftOut;             // Output shift parameter
  int32_t left_shift;           //The upscale parameter to ensure multiplierOut is always less than 1.
  uint32_t min_val;             //Min value of clamp limit
  uint32_t max_val;             //Max value of clamp limit
}xi_cnn_reduceProdA_params;

#define XI_CNN_REDUCE_PROD_GET_CONFIG(x)                     ((x)->config)
#define XI_CNN_REDUCE_PROD_GET_TILEFLAG(x)                   ((x)->tileFlag)
#define XI_CNN_REDUCE_PROD_GET_ZEROPTINPUT(x)                ((x)->zeroPtInput)
#define XI_CNN_REDUCE_PROD_GET_ZEROPTOUTPUT(x)               ((x)->zeroPtOutput)
#define XI_CNN_REDUCE_PROD_GET_MULTIPLIER_OUT(x)             ((x)->multiplierOut)
#define XI_CNN_REDUCE_PROD_GET_SHIFT_OUT(x)                  ((x)->shiftOut)
#define XI_CNN_REDUCE_PROD_GET_LEFT_SHIFT(x)                 ((x)->left_shift)
#define XI_CNN_REDUCE_PROD_GET_MIN_VAL(x)                    ((x)->min_val)
#define XI_CNN_REDUCE_PROD_GET_MAX_VAL(x)                    ((x)->max_val)

#define XI_CNN_REDUCE_PROD_SET_CONFIG(x,v)                   ((x)->config = v)
#define XI_CNN_REDUCE_PROD_SET_TILEFLAG(x,v)                 ((x)->tileFlag = v)
#define XI_CNN_REDUCE_PROD_SET_ZEROPTINPUT(x,v)              ((x)->zeroPtInput = v)
#define XI_CNN_REDUCE_PROD_SET_ZEROPTOUTPUT(x,v)             ((x)->zeroPtOutput = v)
#define XI_CNN_REDUCE_PROD_SET_MULTIPLIER_OUT(x,v)           ((x)->multiplierOut = v)
#define XI_CNN_REDUCE_PROD_SET_SHIFT_OUT(x,v)                ((x)->shiftOut = v)
#define XI_CNN_REDUCE_PROD_SET_LEFT_SHIFT(x,v)               ((x)->left_shift = v)
#define XI_CNN_REDUCE_PROD_SET_MIN_VAL(x, v)                 ((x)->min_val = (v))
#define XI_CNN_REDUCE_PROD_SET_MAX_VAL(x, v)                 ((x)->max_val = (v))

#define XI_CNN_REDUCE_PROD_DIM1     (0x1)
#define XI_CNN_REDUCE_PROD_DIM2     (0x2)
#define XI_CNN_REDUCE_PROD_DIM3     (0x4)
#define XI_CNN_REDUCE_PROD_DIM4     (0x8)

#define XI_CNN_REDUCE_PROD_DIM12    (XI_CNN_REDUCE_PROD_DIM1 | XI_CNN_REDUCE_PROD_DIM2)
#define XI_CNN_REDUCE_PROD_DIM13    (XI_CNN_REDUCE_PROD_DIM1 | XI_CNN_REDUCE_PROD_DIM3)
#define XI_CNN_REDUCE_PROD_DIM14    (XI_CNN_REDUCE_PROD_DIM1 | XI_CNN_REDUCE_PROD_DIM4)

#define XI_CNN_REDUCE_PROD_DIM23    (XI_CNN_REDUCE_PROD_DIM2 | XI_CNN_REDUCE_PROD_DIM3)
#define XI_CNN_REDUCE_PROD_DIM24    (XI_CNN_REDUCE_PROD_DIM2 | XI_CNN_REDUCE_PROD_DIM4)

#define XI_CNN_REDUCE_PROD_DIM34    (XI_CNN_REDUCE_PROD_DIM3 | XI_CNN_REDUCE_PROD_DIM4)

#define XI_CNN_REDUCE_PROD_DIM123   (XI_CNN_REDUCE_PROD_DIM1 | XI_CNN_REDUCE_PROD_DIM2 | XI_CNN_REDUCE_PROD_DIM3)
#define XI_CNN_REDUCE_PROD_DIM124   (XI_CNN_REDUCE_PROD_DIM1 | XI_CNN_REDUCE_PROD_DIM2 | XI_CNN_REDUCE_PROD_DIM4)
#define XI_CNN_REDUCE_PROD_DIM134   (XI_CNN_REDUCE_PROD_DIM1 | XI_CNN_REDUCE_PROD_DIM3 | XI_CNN_REDUCE_PROD_DIM4)

#define XI_CNN_REDUCE_PROD_DIM234   (XI_CNN_REDUCE_PROD_DIM2 | XI_CNN_REDUCE_PROD_DIM3 | XI_CNN_REDUCE_PROD_DIM4)

#define XI_CNN_REDUCE_PROD_DIM1234  (XI_CNN_REDUCE_PROD_DIM1 | XI_CNN_REDUCE_PROD_DIM2 | XI_CNN_REDUCE_PROD_DIM3 | XI_CNN_REDUCE_PROD_DIM4)
#define XI_CNN_REDUCE_FIRST_LAST_TILE  3

typedef struct
{
  int32_t heatMapZeroPt;         // Zero point offset for heatmap
  int32_t outScoreZeroPt;        // zero point for output score tile
  int32_t outScoreMultiplier;    // Output multiplier corresponding to outScore
  int32_t outScoreShift;         // Output Shift corresponding to outScore
  int32_t oneByHeatmapSizeScale; // Reciprocal of heatmap size represented in U15 range
  int32_t oneByHeatmapSizeShift; // Shift value applied to normalize after operating with oneByHeatmapSizeScale
} xi_cnn_heatmap_max_keypointA3D_params;

#define XI_CNN_HEATMAPMAX_PARAMS_GET_HEATMAP_ZERO_POINT(x)           ((x)->heatMapZeroPt)
#define XI_CNN_HEATMAPMAX_PARAMS_GET_OUTSCORE_ZERO_POINT(x)          ((x)->outScoreZeroPt)
#define XI_CNN_HEATMAPMAX_PARAMS_GET_OUTSCORE_MULTIPLIER(x)          ((x)->outScoreMultiplier)
#define XI_CNN_HEATMAPMAX_PARAMS_GET_OUTSCORE_SHIFT(x)               ((x)->outScoreShift)
#define XI_CNN_HEATMAPMAX_PARAMS_GET_ONE_BY_HEATMAPSIZE_SCALE(x)     ((x)->oneByHeatmapSizeScale)
#define XI_CNN_HEATMAPMAX_PARAMS_GET_ONE_BY_HEATMAPSIZE_SHIFT(x)     ((x)->oneByHeatmapSizeShift)
#define XI_CNN_HEATMAPMAX_PARAMS_SET_HEATMAP_ZERO_POINT(x, v)        ((x)->heatMapZeroPt = (v))
#define XI_CNN_HEATMAPMAX_PARAMS_SET_OUTSCORE_ZERO_POINT(x, v)       ((x)->outScoreZeroPt = (v))
#define XI_CNN_HEATMAPMAX_PARAMS_SET_OUTSCORE_MULTIPLIER(x, v)       ((x)->outScoreMultiplier = (v))
#define XI_CNN_HEATMAPMAX_PARAMS_SET_OUTSCORE_SHIFT(x, v)            ((x)->outScoreShift = (v))
#define XI_CNN_HEATMAPMAX_PARAMS_SET_ONE_BY_HEATMAPSIZE_SCALE(x, v)  ((x)->oneByHeatmapSizeScale = (v))
#define XI_CNN_HEATMAPMAX_PARAMS_SET_ONE_BY_HEATMAPSIZE_SHIFT(x, v)  ((x)->oneByHeatmapSizeShift = (v))


typedef struct
{
  int32_t thresholdScore;      /* Score threshold is represented as a signed 32 bit integer.      */
  int32_t thresholdNms;        /* NMS threshold is represented as a signed 32 bit integer.        */
  int32_t thresholdIou;        /* IOU threshold is represented as a signed 32 bit integer.        */
  int32_t detectionsPerImage;  /* max number of proposals to keep per image.                      */
  int32_t softNmsKernel;       /* It can take values of 0, 1 and 2                                */
  int32_t oneBySigma;          /* Applicable when softNmsKernel is set to 2, represented in Q8.23 */
  int32_t scoreZeroPoint;      /* zero point for score tile                                       */
} xi_cnn_box_with_NMS_limitA3D_params;

#define XI_CNN_BOX_WITH_NMS_LIMIT_GET_THRESHOLD_SCORE(x)         ((x)->thresholdScore)
#define XI_CNN_BOX_WITH_NMS_LIMIT_GET_THRESHOLD_NMS(x)           ((x)->thresholdNms)
#define XI_CNN_BOX_WITH_NMS_LIMIT_GET_THRESHOLD_IOU(x)           ((x)->thresholdIou)
#define XI_CNN_BOX_WITH_NMS_LIMIT_GET_DETECTIONS_PER_IMAGE(x)    ((x)->detectionsPerImage)
#define XI_CNN_BOX_WITH_NMS_LIMIT_GET_SOFT_NMS_KERNEL(x)         ((x)->softNmsKernel)
#define XI_CNN_BOX_WITH_NMS_LIMIT_GET_ONE_BY_SIGMA(x)            ((x)->oneBySigma)
#define XI_CNN_BOX_WITH_NMS_LIMIT_GET_SCORE_ZEROPOINT(x)         ((x)->scoreZeroPoint)

#define XI_CNN_BOX_WITH_NMS_LIMIT_SET_THRESHOLD_SCORE(x, v)         ((x)->thresholdScore = (v))
#define XI_CNN_BOX_WITH_NMS_LIMIT_SET_THRESHOLD_NMS(x, v)           ((x)->thresholdNms = (v))
#define XI_CNN_BOX_WITH_NMS_LIMIT_SET_THRESHOLD_IOU(x, v)           ((x)->thresholdIou = (v))
#define XI_CNN_BOX_WITH_NMS_LIMIT_SET_DETECTIONS_PER_IMAGE(x, v)    ((x)->detectionsPerImage = (v))
#define XI_CNN_BOX_WITH_NMS_LIMIT_SET_SOFT_NMS_KERNEL(x, v)         ((x)->softNmsKernel = (v))
#define XI_CNN_BOX_WITH_NMS_LIMIT_SET_ONE_BY_SIGMA(x, v)            ((x)->oneBySigma = (v))
#define XI_CNN_BOX_WITH_NMS_LIMIT_SET_SCORE_ZEROPOINT(x,v)          ((x)->scoreZeroPoint = (v))

typedef struct
{
  int16_t bboxDeltaZeroPt; // zero-point, scale, shift for BBoxDelta
  int32_t bboxDeltaScale;
  int16_t bboxDeltaShift;
  uint16_t imageWidth;
  uint16_t imageHeight;
} xi_cnn_axis_aligned_bbox_transformA3D_params;

#define XI_CNN_AXIS_ALIGNED_BBOX_TRANSFORM_GET_ZERO_POINT_DELTA(x)            ((x)->bboxDeltaZeroPt)
#define XI_CNN_AXIS_ALIGNED_BBOX_TRANSFORM_GET_SCALE_DELTA(x)                 ((x)->bboxDeltaScale)
#define XI_CNN_AXIS_ALIGNED_BBOX_TRANSFORM_GET_SHIFT_DELTA(x)                 ((x)->bboxDeltaShift)
#define XI_CNN_AXIS_ALIGNED_BBOX_TRANSFORM_GET_IMAGE_WIDTH(x)                 ((x)->imageWidth)
#define XI_CNN_AXIS_ALIGNED_BBOX_TRANSFORM_GET_IMAGE_HEIGHT(x)                ((x)->imageHeight)

#define XI_CNN_AXIS_ALIGNED_BBOX_TRANSFORM_SET_ZERO_POINT_DELTA(x, v)         ((x)->bboxDeltaZeroPt = (v))
#define XI_CNN_AXIS_ALIGNED_BBOX_TRANSFORM_SET_SCALE_DELTA(x, v)              ((x)->bboxDeltaScale = (v))
#define XI_CNN_AXIS_ALIGNED_BBOX_TRANSFORM_SET_SHIFT_DELTA(x, v)              ((x)->bboxDeltaShift = (v))
#define XI_CNN_AXIS_ALIGNED_BBOX_TRANSFORM_SET_IMAGE_WIDTH(x, v)              ((x)->imageWidth = (v))
#define XI_CNN_AXIS_ALIGNED_BBOX_TRANSFORM_SET_IMAGE_HEIGHT(x, v)             ((x)->imageHeight = (v))

typedef struct
{
  int32_t spatialScaleX; // ratio from the width of original image to the height of feature map
  int16_t spatialShiftX;
  int32_t spatialScaleY; // ratio from the height of original image to the height of feature map
  int16_t spatialShiftY;
  float minSize;       // minimum threshold for the width/height of the output ROI bounding boxes.
  int32_t preNMSTopN;    // max number of proposals to keep before applying NMS
  int32_t postNMSTopN;   // max number of proposals to keep after applying NMS
  int16_t thresholdNMS;  // threshold of IoU for suppressing overlapping proposals.
  int16_t bboxDeltaZeroPt; // zero-point, scale, shift for BBoxDelta
  int32_t bboxDeltaScale;
  int16_t bboxDeltaShift;
  uint16_t imageWidth;
  uint16_t imageHeight;
  int32_t anchorZeroPt;
  int32_t anchorShift;
  int32_t outBBoxShift;
} xi_cnn_generate_proposalsA3D_params;

#define XI_CNN_GENERATE_PROPOSALS_GET_SPATIAL_SCALEX(x)         ((x)->spatialScaleX)
#define XI_CNN_GENERATE_PROPOSALS_GET_SPATIAL_SHIFTX(x)         ((x)->spatialShiftX)
#define XI_CNN_GENERATE_PROPOSALS_GET_SPATIAL_SCALEY(x)         ((x)->spatialScaleY)
#define XI_CNN_GENERATE_PROPOSALS_GET_SPATIAL_SHIFTY(x)         ((x)->spatialShiftY)

#define XI_CNN_GENERATE_PROPOSALS_GET_MINIMUM_SIZE(x)           ((x)->minSize)
#define XI_CNN_GENERATE_PROPOSALS_GET_PRE_NMS_TOPN(x)           ((x)->preNMSTopN)
#define XI_CNN_GENERATE_PROPOSALS_GET_POST_NMS_TOPN(x)          ((x)->postNMSTopN)
#define XI_CNN_GENERATE_PROPOSALS_GET_THRESHOLD_NMS(x)          ((x)->thresholdNMS)

#define XI_CNN_GENERATE_PROPOSALS_GET_ZERO_POINT_DELTA(x)       ((x)->bboxDeltaZeroPt)
#define XI_CNN_GENERATE_PROPOSALS_GET_SCALE_DELTA(x)            ((x)->bboxDeltaScale)
#define XI_CNN_GENERATE_PROPOSALS_GET_SHIFT_DELTA(x)            ((x)->bboxDeltaShift)

#define XI_CNN_GENERATE_PROPOSALS_GET_IMAGE_WIDTH(x)            ((x)->imageWidth)
#define XI_CNN_GENERATE_PROPOSALS_GET_IMAGE_HEIGHT(x)           ((x)->imageHeight)

#define XI_CNN_GENERATE_PROPOSALS_GET_ZERO_POINT_ANCHOR(x)      ((x)->anchorZeroPt)
#define XI_CNN_GENERATE_PROPOSALS_GET_SHIFT_ANCHOR(x)           ((x)->anchorShift)

#define XI_CNN_GENERATE_PROPOSALS_GET_SHIFT_OUT_BBOX(x)         ((x)->outBBoxShift)

#define XI_CNN_GENERATE_PROPOSALS_SET_SPATIAL_SCALEX(x, v)         ((x)->spatialScaleX = (v))
#define XI_CNN_GENERATE_PROPOSALS_SET_SPATIAL_SHIFTX(x, v)         ((x)->spatialShiftX = (v))
#define XI_CNN_GENERATE_PROPOSALS_SET_SPATIAL_SCALEY(x, v)         ((x)->spatialScaleY = (v))
#define XI_CNN_GENERATE_PROPOSALS_SET_SPATIAL_SHIFTY(x, v)         ((x)->spatialShiftY = (v))

#define XI_CNN_GENERATE_PROPOSALS_SET_MINIMUM_SIZE(x, v)           ((x)->minSize = (v))
#define XI_CNN_GENERATE_PROPOSALS_SET_PRE_NMS_TOPN(x, v)           ((x)->preNMSTopN = (v))
#define XI_CNN_GENERATE_PROPOSALS_SET_POST_NMS_TOPN(x, v)          ((x)->postNMSTopN = (v))
#define XI_CNN_GENERATE_PROPOSALS_SET_THRESHOLD_NMS(x, v)          ((x)->thresholdNMS = (v))

#define XI_CNN_GENERATE_PROPOSALS_SET_ZERO_POINT_DELTA(x, v)       ((x)->bboxDeltaZeroPt = (v))
#define XI_CNN_GENERATE_PROPOSALS_SET_SCALE_DELTA(x, v)            ((x)->bboxDeltaScale = (v))
#define XI_CNN_GENERATE_PROPOSALS_SET_SHIFT_DELTA(x, v)            ((x)->bboxDeltaShift = (v))

#define XI_CNN_GENERATE_PROPOSALS_SET_IMAGE_WIDTH(x, v)            ((x)->imageWidth = (v))
#define XI_CNN_GENERATE_PROPOSALS_SET_IMAGE_HEIGHT(x, v)           ((x)->imageHeight = (v))

typedef struct
{
  int32_t qWidthStride;  // quantized ratio of stride along width to anchorScale value
  int32_t qHeightStride; // quantized ratio of stride along height to anchorScale value
  uint16_t qMinSize;     // quantized minimum threshold for the width/height of the output ROI bounding boxes.
  int32_t preNMSTopN;    // max number of proposals to keep before applying NMS
  int32_t postNMSTopN;   // max number of proposals to keep after applying NMS
  int32_t qIouThreshold;  // threshold of IoU for suppressing overlapping proposals.
  int16_t bboxDeltaZeroPt; // zero-point, scale, shift for BBoxDelta
  int32_t bboxDeltaScale;
  int16_t bboxDeltaShift;
  uint16_t imageWidth;
  uint16_t imageHeight;
  int32_t outBBoxShift;
} xi_cnn_generate_proposalsA3D_I8_params;

#define XI_CNN_GENERATE_PROPOSALS_I8_GET_QUANT_WIDTH_STRIDE(x)     ((x)->qWidthStride)
#define XI_CNN_GENERATE_PROPOSALS_I8_GET_QUANT_HEIGHT_STRIDE(x)    ((x)->qHeightStride)

#define XI_CNN_GENERATE_PROPOSALS_I8_GET_QUANT_MINIMUM_SIZE(x)     ((x)->qMinSize)
#define XI_CNN_GENERATE_PROPOSALS_I8_GET_PRE_NMS_TOPN(x)           ((x)->preNMSTopN)
#define XI_CNN_GENERATE_PROPOSALS_I8_GET_POST_NMS_TOPN(x)          ((x)->postNMSTopN)
#define XI_CNN_GENERATE_PROPOSALS_I8_GET_QUANT_IOU_THRESHOLD(x)    ((x)->qIouThreshold)

#define XI_CNN_GENERATE_PROPOSALS_I8_GET_ZERO_POINT_DELTA(x)       ((x)->bboxDeltaZeroPt)
#define XI_CNN_GENERATE_PROPOSALS_I8_GET_SCALE_DELTA(x)            ((x)->bboxDeltaScale)
#define XI_CNN_GENERATE_PROPOSALS_I8_GET_SHIFT_DELTA(x)            ((x)->bboxDeltaShift)

#define XI_CNN_GENERATE_PROPOSALS_I8_GET_IMAGE_WIDTH(x)            ((x)->imageWidth)
#define XI_CNN_GENERATE_PROPOSALS_I8_GET_IMAGE_HEIGHT(x)           ((x)->imageHeight)

#define XI_CNN_GENERATE_PROPOSALS_I8_GET_SHIFT_OUT_BBOX(x)         ((x)->outBBoxShift)

#define XI_CNN_GENERATE_PROPOSALS_I8_SET_QUANT_WIDTH_STRIDE(x, v)     ((x)->qWidthStride = (v))
#define XI_CNN_GENERATE_PROPOSALS_I8_SET_QUANT_HEIGHT_STRIDE(x, v)    ((x)->qHeightStride = (v))

#define XI_CNN_GENERATE_PROPOSALS_I8_SET_QUANT_MINIMUM_SIZE(x, v)     ((x)->qMinSize = (v))
#define XI_CNN_GENERATE_PROPOSALS_I8_SET_PRE_NMS_TOPN(x, v)           ((x)->preNMSTopN = (v))
#define XI_CNN_GENERATE_PROPOSALS_I8_SET_POST_NMS_TOPN(x, v)          ((x)->postNMSTopN = (v))
#define XI_CNN_GENERATE_PROPOSALS_I8_SET_QUANT_IOU_THRESHOLD(x, v)    ((x)->qIouThreshold = (v))

#define XI_CNN_GENERATE_PROPOSALS_I8_SET_ZERO_POINT_DELTA(x, v)       ((x)->bboxDeltaZeroPt = (v))
#define XI_CNN_GENERATE_PROPOSALS_I8_SET_SCALE_DELTA(x, v)            ((x)->bboxDeltaScale = (v))
#define XI_CNN_GENERATE_PROPOSALS_I8_SET_SHIFT_DELTA(x, v)            ((x)->bboxDeltaShift = (v))

#define XI_CNN_GENERATE_PROPOSALS_I8_SET_IMAGE_WIDTH(x, v)            ((x)->imageWidth = (v))
#define XI_CNN_GENERATE_PROPOSALS_I8_SET_IMAGE_HEIGHT(x, v)           ((x)->imageHeight = (v))

#define XI_CNN_GENERATE_PROPOSALS_I8_SET_SHIFT_OUT_BBOX(x, v)         ((x)->outBBoxShift = (v))

typedef struct
{
  int32_t zero_pointInp1; /* Zero point (a 32 bit integer, in range [0, 255]) for input1 tile */
  int32_t zero_pointInp2; /* Zero point (a 32 bit integer, in range [0, 255]) for input2 tile */
  int32_t zero_pointOut; /* Zero point (a 32 bit integer, in range [0, 255]) for output tile */
  int32_t multiplierInp1; /* The input1 scaling factor */
  int32_t multiplierInp2; /* The input2 scaling factor */
  int32_t multiplierOut; /* The output scaling factor */
  int32_t shiftInp1; /* The input1 shifting factor */
  int32_t shiftInp2; /* The input2 shifting factor */
  int32_t shiftOut; /* The output shifting factor */
  int32_t left_shift; /*Fixed amount required for fixed point math better precision */
} xi_cnn_selectA_params;

#define XI_CNN_SELECTA_GET_ZERO_POINT_IN1(x)     ((x)->zero_pointInp1)
#define XI_CNN_SELECTA_GET_ZERO_POINT_IN2(x)     ((x)->zero_pointInp2)
#define XI_CNN_SELECTA_GET_ZERO_POINT_OUT(x)     ((x)->zero_pointOut)
#define XI_CNN_SELECTA_GET_MULTIPLIER_IN1(x)     ((x)->multiplierInp1)
#define XI_CNN_SELECTA_GET_MULTIPLIER_IN2(x)     ((x)->multiplierInp2)
#define XI_CNN_SELECTA_GET_MULTIPLIER_OUT(x)     ((x)->multiplierOut)
#define XI_CNN_SELECTA_GET_SHIFT_IN1(x)          ((x)->shiftInp1)
#define XI_CNN_SELECTA_GET_SHIFT_IN2(x)          ((x)->shiftInp2)
#define XI_CNN_SELECTA_GET_SHIFT_OUT(x)          ((x)->shiftOut)
#define XI_CNN_SELECTA_GET_LEFT_SHIFT(x)         ((x)->left_shift)

#define XI_CNN_SELECTA_SET_ZERO_POINT_IN1(x, v)  ((x)->zero_pointInp1 = (v))
#define XI_CNN_SELECTA_SET_ZERO_POINT_IN2(x, v)  ((x)->zero_pointInp2 = (v))
#define XI_CNN_SELECTA_SET_ZERO_POINT_OUT(x, v)  ((x)->zero_pointOut = (v))
#define XI_CNN_SELECTA_SET_MULTIPLIER_IN1(x, v)  ((x)->multiplierInp1 = (v))
#define XI_CNN_SELECTA_SET_MULTIPLIER_IN2(x, v)  ((x)->multiplierInp2 = (v))
#define XI_CNN_SELECTA_SET_MULTIPLIER_OUT(x, v)  ((x)->multiplierOut = (v))
#define XI_CNN_SELECTA_SET_SHIFT_IN1(x, v)       ((x)->shiftInp1 = (v))
#define XI_CNN_SELECTA_SET_SHIFT_IN2(x, v)       ((x)->shiftInp2 = (v))
#define XI_CNN_SELECTA_SET_SHIFT_OUT(x, v)       ((x)->shiftOut = (v))
#define XI_CNN_SELECTA_SET_LEFT_SHIFT(x, v)      ((x)->left_shift = (v))
#define XI_CNN_GENERATE_PROPOSALS_SET_ZERO_POINT_ANCHOR(x, v)      ((x)->anchorZeroPt = (v))
#define XI_CNN_GENERATE_PROPOSALS_SET_SHIFT_ANCHOR(x, v)           ((x)->anchorShift = (v))

#define XI_CNN_GENERATE_PROPOSALS_SET_SHIFT_OUT_BBOX(x, v)         ((x)->outBBoxShift = (v))

typedef struct {
  XI_Q15_16 widthScale;  //Q15.16 format
  XI_Q15_16 heightScale;  //Q15.16 format
  int32_t srcStartW;
  int32_t srcStartH;
  int32_t dstStartW;
  int32_t dstStartH;
  int8_t alignCorners;
  int8_t halfPixelCenters;
} xi_cnn_resizeA3D_params;


#define XI_CNN_RESIZE3D_GET_WIDTHSCALE(x)                ((x)->widthScale)
#define XI_CNN_RESIZE3D_GET_HEIGHTSCALE(x)               ((x)->heightScale)
#define XI_CNN_RESIZE3D_GET_SRC_STARTW(x)                ((x)->srcStartW)
#define XI_CNN_RESIZE3D_GET_SRC_STARTH(x)                ((x)->srcStartH)
#define XI_CNN_RESIZE3D_GET_DST_STARTW(x)                ((x)->dstStartW)
#define XI_CNN_RESIZE3D_GET_DST_STARTH(x)                ((x)->dstStartH)
#define XI_CNN_RESIZE3D_GET_FLAG_ALIGN_CORNERS(x)        ((x)->alignCorners)
#define XI_CNN_RESIZE3D_GET_FLAG_HALF_PIXEL_CENTERS(x)   ((x)->halfPixelCenters)

#define XI_CNN_RESIZE3D_SET_WIDTHSCALE(x, v)  ((x)->widthScale = (v))
#define XI_CNN_RESIZE3D_SET_HEIGHTSCALE(x, v) ((x)->heightScale = (v))
#define XI_CNN_RESIZE3D_SET_SRC_STARTW(x,v)   ((x)->srcStartW  = (v))
#define XI_CNN_RESIZE3D_SET_SRC_STARTH(x,v)   ((x)->srcStartH  = (v))
#define XI_CNN_RESIZE3D_SET_DST_STARTW(x,v)   ((x)->dstStartW  = (v))
#define XI_CNN_RESIZE3D_SET_DST_STARTH(x,v)   ((x)->dstStartH  = (v))
#define XI_CNN_RESIZE3D_SET_FLAG_ALIGN_CORNERS(x,v)        ((x)->alignCorners = v)
#define XI_CNN_RESIZE3D_SET_FLAG_HALF_PIXEL_CENTERS(x,v)   ((x)->halfPixelCenters = v)

typedef struct
{
  uint8_t config;        /* The dimension along which L2Norm operation needs to be performed, can take values 1,2,3.
                          * 1 -> Operation along dimension 1,
                          * 2 -> Operation along dimension 2,
                          * 3 -> Operation along dimension 3
                          */
  int32_t zeroPtInput;   /* Zero point offset of input data */
} xi_cnn_l2NormA_params;

#define XI_CNN_L2NORMA_GET_CONFIG(x)            ((x)->config)
#define XI_CNN_L2NORMA_GET_ZERO_POINT_IN(x)     ((x)->zeroPtInput)

#define XI_CNN_L2NORMA_SET_CONFIG(x, v)         ((x)->config = (v))
#define XI_CNN_L2NORMA_SET_ZERO_POINT_IN(x, v)  ((x)->zeroPtInput = (v))


#define XI_CNN_L2NORMA_DIM1     (0x1)
#define XI_CNN_L2NORMA_DIM2     (0x2)
#define XI_CNN_L2NORMA_DIM3     (0x3)

typedef struct
{
  int32_t zeroPtInput;
  int32_t zeroPtOutput;
  int16_t outMultiplier;
  int16_t reluishMultiplier;
  int32_t outShift;
  int32_t reluishShift;
  int32_t activationMin;
  int32_t activationMax;
}xi_cnn_hardSwishA3D_params;

#define XI_CNN_HARDSWISHA3D_GET_ZERO_POINT_INPUT(x)      ((x)->zeroPtInput)
#define XI_CNN_HARDSWISHA3D_GET_ZERO_POINT_OUTPUT(x)     ((x)->zeroPtOutput)
#define XI_CNN_HARDSWISHA3D_GET_OUT_MULTIPLIER(x)        ((x)->outMultiplier)
#define XI_CNN_HARDSWISHA3D_GET_RELUISH_MULTIPLIER(x)    ((x)->reluishMultiplier)
#define XI_CNN_HARDSWISHA3D_GET_OUT_SHIFT(x)             ((x)->outShift)
#define XI_CNN_HARDSWISHA3D_GET_RELUISH_SHIFT(x)         ((x)->reluishShift)
#define XI_CNN_HARDSWISHA3D_GET_ACTIVATION_MIN(x)        ((x)->activationMin)
#define XI_CNN_HARDSWISHA3D_GET_ACTIVATION_MAX(x)        ((x)->activationMax)

#define XI_CNN_HARDSWISHA3D_SET_ZERO_POINT_INPUT(x, v)      ((x)->zeroPtInput = (v))
#define XI_CNN_HARDSWISHA3D_SET_ZERO_POINT_OUTPUT(x, v)     ((x)->zeroPtOutput = (v))
#define XI_CNN_HARDSWISHA3D_SET_OUT_MULTIPLIER(x, v)        ((x)->outMultiplier = (v))
#define XI_CNN_HARDSWISHA3D_SET_RELUISH_MULTIPLIER(x, v)    ((x)->reluishMultiplier = (v))
#define XI_CNN_HARDSWISHA3D_SET_OUT_SHIFT(x, v)             ((x)->outShift = (v))
#define XI_CNN_HARDSWISHA3D_SET_RELUISH_SHIFT(x, v)         ((x)->reluishShift = (v))
#define XI_CNN_HARDSWISHA3D_SET_ACTIVATION_MIN(x, v)        ((x)->activationMin = (v))
#define XI_CNN_HARDSWISHA3D_SET_ACTIVATION_MAX(x, v)        ((x)->activationMax = (v))

typedef struct
{
  // Previus Output State zero point
  int32_t prevOutStateZeroPoint;
  // Input zero point
  int32_t inputZeroPoint;
  // Hidden State zero point
  int32_t hiddenStateZeroPoint;

  // Forget Gate Params
  int32_t inputToForgetMultiplier;
  int32_t inputToForgetShift;
  int32_t recurrentToForgetMultiplier;
  int32_t recurrentToForgetShift;
  int32_t cellToForgetMultiplier;
  int32_t cellToForgetShift;
  int32_t forgetLayerNormMultiplier;
  int32_t forgetLayerNormShift;

  // Modulation Gate Params
  int32_t inputToCellMultiplier;
  int32_t inputToCellShift;
  int32_t recurrentToCellMultiplier;
  int32_t recurrentToCellShift;
  int32_t cellLayerNormMultiplier;
  int32_t cellLayerNormShift;

  // Input Gate Params
  int32_t inputToInputMultiplier;
  int32_t inputToInputShift;
  int32_t recurrentToInputMultiplier;
  int32_t recurrentToInputShift;
  int32_t cellToInputMultiplier;
  int32_t cellToInputShift;
  int32_t inputLayerNormMultiplier;
  int32_t inputLayerNormShift;

  // Output Gate Params
  int32_t inputToOutputMultiplier;
  int32_t inputToOutputShift;
  int32_t recurrentToOutputMultiplier;
  int32_t recurrentToOutputShift;
  int32_t cellToOutputMultiplier;
  int32_t cellToOutputShift;
  int32_t outputLayerNormMultiplier;
  int32_t outputLayerNormShift;

  // Hidden Layer Params
  int32_t hiddenStateMultiplier;
  int32_t hiddenStateShift;

  // Projection Layer Params
  int32_t projectionMultiplier;
  int32_t projectionShift;

  // Quantized Clipping Values
  int32_t quantizedProjectionClip;
  int32_t quantizedCellClip;

  // Obtained from prevCellState scale
  int32_t cellShift;

  // inverse large value params for Layer Norm
  int32_t forgetVarianceLimit;
  int32_t inputVarianceLimit;
  int32_t cellVarianceLimit;
  int32_t outputVarianceLimit;

  // Different variation options for QLSTM
  uint8_t useCifg;    // Coupled input forget gate parameter, can be 0 or 1
  uint8_t usePeephole;
  /*bit0 corresponds to peephole flag for forget gate
  * bit1 corresponds to peephole flag for input gate
  * bit2 corresponds to peephole flag for output gate
  *
  *  ------------------------------------------------------------------------
  *  | bit7  | bit6 | bit5 | bit4 | bit3 | bit2     | bit1       | bit0     |
  *  |unused |unused|unused|unused|unused|  Output  |  Input     | Forget   |
  *  ------------------------------------------------------------------------
  */
  uint8_t useLayerNorm;
  /*bit0 corresponds to layer normalization flag for forget gate
  * bit1 corresponds to layer normalization flag for modulation gate
  * bit2 corresponds to layer normalization flag for input gate
  * bit2 corresponds to layer normalization flag for output gate
  *
  *  --------------------------------------------------------------------------
  *  | bit7  | bit6 | bit5 | bit4 | bit3   | bit2     | bit1       | bit0     |
  *  |unused |unused|unused|unused|Output  |  Input   | Modulation | Forget   |
  *  --------------------------------------------------------------------------
  */
  uint8_t useProjection; // Projection Layer parameter, can be 0 or 1

}xi_cnn_enhanced_LSTMA_params;

/* Previous and input State zero points */
#define XI_CNN_ENHANCED_LSTMA_GET_PREV_OUT_ZERO_POINT(x)            ((x)->prevOutStateZeroPoint)
#define XI_CNN_ENHANCED_LSTMA_SET_PREV_OUT_ZERO_POINT(x, v)         ((x)->prevOutStateZeroPoint = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_INPUT_ZERO_POINT(x)               ((x)->inputZeroPoint)
#define XI_CNN_ENHANCED_LSTMA_SET_INPUT_ZERO_POINT(x, v)            ((x)->inputZeroPoint = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_HIDDEN_STATE_ZERO_POINT(x)        ((x)->hiddenStateZeroPoint)
#define XI_CNN_ENHANCED_LSTMA_SET_HIDDEN_STATE_ZERO_POINT(x, v)     ((x)->hiddenStateZeroPoint = (v))

/* Forget Gate Params */
#define XI_CNN_ENHANCED_LSTMA_GET_INPUT_TO_FORGET_MULTIPLIER(x)     ((x)->inputToForgetMultiplier)
#define XI_CNN_ENHANCED_LSTMA_SET_INPUT_TO_FORGET_MULTIPLIER(x, v)  ((x)->inputToForgetMultiplier = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_INPUT_TO_FORGET_SHIFT(x)          ((x)->inputToForgetShift)
#define XI_CNN_ENHANCED_LSTMA_SET_INPUT_TO_FORGET_SHIFT(x, v)       ((x)->inputToForgetShift = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_RECURRENT_TO_FORGET_MULTIPLIER(x) ((x)->recurrentToForgetMultiplier)
#define XI_CNN_ENHANCED_LSTMA_SET_RECURRENT_TO_FORGET_MULTIPLIER(x, v)  ((x)->recurrentToForgetMultiplier = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_RECURRENT_TO_FORGET_SHIFT(x)      ((x)->recurrentToForgetShift)
#define XI_CNN_ENHANCED_LSTMA_SET_RECURRENT_TO_FORGET_SHIFT(x, v)   ((x)->recurrentToForgetShift = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_CELL_TO_FORGET_MULTIPLIER(x)      ((x)->cellToForgetMultiplier)
#define XI_CNN_ENHANCED_LSTMA_SET_CELL_TO_FORGET_MULTIPLIER(x, v)   ((x)->cellToForgetMultiplier = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_CELL_TO_FORGET_SHIFT(x)           ((x)->cellToForgetShift)
#define XI_CNN_ENHANCED_LSTMA_SET_CELL_TO_FORGET_SHIFT(x, v)        ((x)->cellToForgetShift = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_LAYER_NORM_FORGET_MULTIPLIER(x)   ((x)->forgetLayerNormMultiplier)
#define XI_CNN_ENHANCED_LSTMA_SET_LAYER_NORM_FORGET_MULTIPLIER(x, v)  ((x)->forgetLayerNormMultiplier = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_LAYER_NORM_FORGET_SHIFT(x)        ((x)->forgetLayerNormShift)
#define XI_CNN_ENHANCED_LSTMA_SET_LAYER_NORM_FORGET_SHIFT(x, v)     ((x)->forgetLayerNormShift = (v))

/* Modulation Gate Params */
#define XI_CNN_ENHANCED_LSTMA_GET_INPUT_TO_CELL_MULTIPLIER(x)       ((x)->inputToCellMultiplier)
#define XI_CNN_ENHANCED_LSTMA_SET_INPUT_TO_CELL_MULTIPLIER(x, v)    ((x)->inputToCellMultiplier = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_INPUT_TO_CELL_SHIFT(x)            ((x)->inputToCellShift)
#define XI_CNN_ENHANCED_LSTMA_SET_INPUT_TO_CELL_SHIFT(x, v)         ((x)->inputToCellShift = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_RECURRENT_TO_CELL_MULTIPLIER(x)   ((x)->recurrentToCellMultiplier)
#define XI_CNN_ENHANCED_LSTMA_SET_RECURRENT_TO_CELL_MULTIPLIER(x, v)  ((x)->recurrentToCellMultiplier = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_RECURRENT_TO_CELL_SHIFT(x)        ((x)->recurrentToCellShift)
#define XI_CNN_ENHANCED_LSTMA_SET_RECURRENT_TO_CELL_SHIFT(x, v)     ((x)->recurrentToCellShift = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_LAYER_NORM_CELL_MULTIPLIER(x)     ((x)->cellLayerNormMultiplier)
#define XI_CNN_ENHANCED_LSTMA_SET_LAYER_NORM_CELL_MULTIPLIER(x, v)  ((x)->cellLayerNormMultiplier = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_LAYER_NORM_CELL_SHIFT(x)          ((x)->cellLayerNormShift)
#define XI_CNN_ENHANCED_LSTMA_SET_LAYER_NORM_CELL_SHIFT(x, v)       ((x)->cellLayerNormShift = (v))

/* Input Gate Params */
#define XI_CNN_ENHANCED_LSTMA_GET_INPUT_TO_INPUT_MULTIPLIER(x)      ((x)->inputToInputMultiplier)
#define XI_CNN_ENHANCED_LSTMA_SET_INPUT_TO_INPUT_MULTIPLIER(x, v)   ((x)->inputToInputMultiplier = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_INPUT_TO_INPUT_SHIFT(x)           ((x)->inputToInputShift)
#define XI_CNN_ENHANCED_LSTMA_SET_INPUT_TO_INPUT_SHIFT(x, v)        ((x)->inputToInputShift = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_RECURRENT_TO_INPUT_MULTIPLIER(x)  ((x)->recurrentToInputMultiplier)
#define XI_CNN_ENHANCED_LSTMA_SET_RECURRENT_TO_INPUT_MULTIPLIER(x, v)  ((x)->recurrentToInputMultiplier = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_RECURRENT_TO_INPUT_SHIFT(x)       ((x)->recurrentToInputShift)
#define XI_CNN_ENHANCED_LSTMA_SET_RECURRENT_TO_INPUT_SHIFT(x, v)    ((x)->recurrentToInputShift = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_CELL_TO_INPUT_MULTIPLIER(x)       ((x)->cellToInputMultiplier)
#define XI_CNN_ENHANCED_LSTMA_SET_CELL_TO_INPUT_MULTIPLIER(x, v)    ((x)->cellToInputMultiplier = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_CELL_TO_INPUT_SHIFT(x)            ((x)->cellToInputShift)
#define XI_CNN_ENHANCED_LSTMA_SET_CELL_TO_INPUT_SHIFT(x, v)         ((x)->cellToInputShift = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_LAYER_NORM_INPUT_MULTIPLIER(x)    ((x)->inputLayerNormMultiplier)
#define XI_CNN_ENHANCED_LSTMA_SET_LAYER_NORM_INPUT_MULTIPLIER(x, v) ((x)->inputLayerNormMultiplier = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_LAYER_NORM_INPUT_SHIFT(x)         ((x)->inputLayerNormShift)
#define XI_CNN_ENHANCED_LSTMA_SET_LAYER_NORM_INPUT_SHIFT(x, v)      ((x)->inputLayerNormShift = (v))


/* Output Gate Params */
#define XI_CNN_ENHANCED_LSTMA_GET_INPUT_TO_OUTPUT_MULTIPLIER(x)     ((x)->inputToOutputMultiplier)
#define XI_CNN_ENHANCED_LSTMA_SET_INPUT_TO_OUTPUT_MULTIPLIER(x, v)  ((x)->inputToOutputMultiplier = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_INPUT_TO_OUTPUT_SHIFT(x)          ((x)->inputToOutputShift)
#define XI_CNN_ENHANCED_LSTMA_SET_INPUT_TO_OUTPUT_SHIFT(x, v)       ((x)->inputToOutputShift = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_RECURRENT_TO_OUTPUT_MULTIPLIER(x) ((x)->recurrentToOutputMultiplier)
#define XI_CNN_ENHANCED_LSTMA_SET_RECURRENT_TO_OUTPUT_MULTIPLIER(x, v)  ((x)->recurrentToOutputMultiplier = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_RECURRENT_TO_OUTPUT_SHIFT(x)      ((x)->recurrentToOutputShift)
#define XI_CNN_ENHANCED_LSTMA_SET_RECURRENT_TO_OUTPUT_SHIFT(x, v)   ((x)->recurrentToOutputShift = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_CELL_TO_OUTPUT_MULTIPLIER(x)      ((x)->cellToOutputMultiplier)
#define XI_CNN_ENHANCED_LSTMA_SET_CELL_TO_OUTPUT_MULTIPLIER(x, v)   ((x)->cellToOutputMultiplier = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_CELL_TO_OUTPUT_SHIFT(x)           ((x)->cellToOutputShift)
#define XI_CNN_ENHANCED_LSTMA_SET_CELL_TO_OUTPUT_SHIFT(x, v)        ((x)->cellToOutputShift = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_LAYER_NORM_OUTPUT_MULTIPLIER(x)   ((x)->outputLayerNormMultiplier)
#define XI_CNN_ENHANCED_LSTMA_SET_LAYER_NORM_OUTPUT_MULTIPLIER(x, v)  ((x)->outputLayerNormMultiplier = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_LAYER_NORM_OUTPUT_SHIFT(x)        ((x)->outputLayerNormShift)
#define XI_CNN_ENHANCED_LSTMA_SET_LAYER_NORM_OUTPUT_SHIFT(x, v)     ((x)->outputLayerNormShift = (v))

/* Hidden Layer Params */
#define XI_CNN_ENHANCED_LSTMA_GET_HIDDEN_STATE_MULTIPLIER(x)        ((x)->hiddenStateMultiplier)
#define XI_CNN_ENHANCED_LSTMA_SET_HIDDEN_STATE_MULTIPLIER(x, v)     ((x)->hiddenStateMultiplier = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_HIDDEN_STATE_SHIFT(x)             ((x)->hiddenStateShift)
#define XI_CNN_ENHANCED_LSTMA_SET_HIDDEN_STATE_SHIFT(x, v)          ((x)->hiddenStateShift = (v))

/* Projection Layer Params */
#define XI_CNN_ENHANCED_LSTMA_GET_PROJECTION_STATE_MULTIPLIER(x)    ((x)->projectionMultiplier)
#define XI_CNN_ENHANCED_LSTMA_SET_PROJECTION_STATE_MULTIPLIER(x, v) ((x)->projectionMultiplier = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_PROJECTION_STATE_SHIFT(x)         ((x)->projectionShift)
#define XI_CNN_ENHANCED_LSTMA_SET_PROJECTION_STATE_SHIFT(x, v)      ((x)->projectionShift = (v))

/* Quantized Clipping Values */
#define XI_CNN_ENHANCED_LSTMA_GET_QUANT_PROJECTION_CLIP(x)          ((x)->quantizedProjectionClip)
#define XI_CNN_ENHANCED_LSTMA_SET_QUANT_PROJECTION_CLIP(x, v)       ((x)->quantizedProjectionClip = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_QUANT_CELL_CLIP(x)                ((x)->quantizedCellClip)
#define XI_CNN_ENHANCED_LSTMA_SET_QUANT_CELL_CLIP(x, v)             ((x)->quantizedCellClip = (v))

#define XI_CNN_ENHANCED_LSTMA_GET_CELL_SHIFT(x)                     ((x)->cellShift)
#define XI_CNN_ENHANCED_LSTMA_SET_CELL_SHIFT(x, v)                  ((x)->cellShift = (v))

/* inverse large value params for Layer Norm */
#define XI_CNN_ENHANCED_LSTMA_GET_FORGET_VARLIMIT(x)                ((x)->forgetVarianceLimit)
#define XI_CNN_ENHANCED_LSTMA_SET_FORGET_VARLIMIT(x, v)             ((x)->forgetVarianceLimit = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_INPUT_VARLIMIT(x)                 ((x)->inputVarianceLimit)
#define XI_CNN_ENHANCED_LSTMA_SET_INPUT_VARLIMIT(x, v)              ((x)->inputVarianceLimit = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_CELL_VARLIMIT(x)                  ((x)->cellVarianceLimit)
#define XI_CNN_ENHANCED_LSTMA_SET_CELL_VARLIMIT(x, v)               ((x)->cellVarianceLimit = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_OUTPUT_VARLIMIT(x)                ((x)->outputVarianceLimit)
#define XI_CNN_ENHANCED_LSTMA_SET_OUTPUT_VARLIMIT(x, v)             ((x)->outputVarianceLimit = (v))

/* Flags */
#define XI_CNN_ENHANCED_LSTMA_GET_USE_CIFG(x)                       ((x)->useCifg)
#define XI_CNN_ENHANCED_LSTMA_SET_USE_CIFG(x, v)                    ((x)->useCifg = (v))

#define XI_CNN_ENHANCED_LSTMA_GET_USE_PEEPHOLE(x)                   ((x)->usePeephole)
#define XI_CNN_ENHANCED_LSTMA_SET_USE_PEEPHOLE(x, v)                ((x)->usePeephole = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_USE_PEEPHOLE_FORGET(x)            ((x)->usePeephole & CNN_ENHANCED_LSTMA_FORGET_PEEPHOLE)
#define XI_CNN_ENHANCED_LSTMA_SET_USE_PEEPHOLE_FORGET(x)            ((x)->usePeephole = \
                                                                     ((x)->usePeephole | CNN_ENHANCED_LSTMA_FORGET_PEEPHOLE))
#define XI_CNN_ENHANCED_LSTMA_RESET_USE_PEEPHOLE_FORGET(x)          ((x)->usePeephole = \
                                                                     ((x)->usePeephole & ~CNN_ENHANCED_LSTMA_FORGET_PEEPHOLE))
#define XI_CNN_ENHANCED_LSTMA_GET_USE_PEEPHOLE_INPUT(x)             ((x)->usePeephole & CNN_ENHANCED_LSTMA_INPUT_PEEPHOLE)
#define XI_CNN_ENHANCED_LSTMA_SET_USE_PEEPHOLE_INPUT(x)             ((x)->usePeephole = \
                                                                     ((x)->usePeephole | CNN_ENHANCED_LSTMA_INPUT_PEEPHOLE))
#define XI_CNN_ENHANCED_LSTMA_RESET_USE_PEEPHOLE_INPUT(x)           ((x)->usePeephole = \
                                                                     ((x)->usePeephole & ~CNN_ENHANCED_LSTMA_INPUT_PEEPHOLE))
#define XI_CNN_ENHANCED_LSTMA_GET_USE_PEEPHOLE_OUTPUT(x)            ((x)->usePeephole & CNN_ENHANCED_LSTMA_OUTPUT_PEEPHOLE)
#define XI_CNN_ENHANCED_LSTMA_SET_USE_PEEPHOLE_OUTPUT(x)            ((x)->usePeephole = \
                                                                     ((x)->usePeephole | CNN_ENHANCED_LSTMA_OUTPUT_PEEPHOLE))
#define XI_CNN_ENHANCED_LSTMA_RESET_USE_PEEPHOLE_OUTPUT(x)          ((x)->usePeephole = \
                                                                     ((x)->usePeephole & ~CNN_ENHANCED_LSTMA_OUTPUT_PEEPHOLE))

#define XI_CNN_ENHANCED_LSTMA_GET_USE_LAYER_NORM(x)                 ((x)->useLayerNorm)
#define XI_CNN_ENHANCED_LSTMA_SET_USE_LAYER_NORM(x, v)              ((x)->useLayerNorm = (v))
#define XI_CNN_ENHANCED_LSTMA_GET_USE_LAYERNORM_FORGET(x)           ((x)->useLayerNorm & CNN_ENHANCED_LSTMA_FORGET_LAYERNORM)
#define XI_CNN_ENHANCED_LSTMA_SET_USE_LAYERNORM_FORGET(x)           ((x)->useLayerNorm = \
                                                                     ((x)->useLayerNorm | CNN_ENHANCED_LSTMA_FORGET_LAYERNORM))
#define XI_CNN_ENHANCED_LSTMA_RESET_USE_LAYERNORM_FORGET(x)         ((x)->useLayerNorm = \
                                                                     ((x)->useLayerNorm & ~CNN_ENHANCED_LSTMA_FORGET_LAYERNORM))
#define XI_CNN_ENHANCED_LSTMA_GET_USE_LAYERNORM_MODULATION(x)       ((x)->useLayerNorm & CNN_ENHANCED_LSTMA_MODULATION_LAYERNORM)
#define XI_CNN_ENHANCED_LSTMA_SET_USE_LAYERNORM_MODULATION(x)       ((x)->useLayerNorm = \
                                                                     ((x)->useLayerNorm | CNN_ENHANCED_LSTMA_MODULATION_LAYERNORM))
#define XI_CNN_ENHANCED_LSTMA_RESET_USE_LAYERNORM_MODULATION(x)     ((x)->useLayerNorm = \
                                                                     ((x)->useLayerNorm & ~CNN_ENHANCED_LSTMA_MODULATION_LAYERNORM))
#define XI_CNN_ENHANCED_LSTMA_GET_USE_LAYERNORM_INPUT(x)            ((x)->useLayerNorm & CNN_ENHANCED_LSTMA_INPUT_LAYERNORM)
#define XI_CNN_ENHANCED_LSTMA_SET_USE_LAYERNORM_INPUT(x)            ((x)->useLayerNorm = \
                                                                    ((x)->useLayerNorm | CNN_ENHANCED_LSTMA_INPUT_LAYERNORM))
#define XI_CNN_ENHANCED_LSTMA_RESET_USE_LAYERNORM_INPUT(x)          ((x)->useLayerNorm = \
                                                                    ((x)->useLayerNorm & ~CNN_ENHANCED_LSTMA_INPUT_LAYERNORM))
#define XI_CNN_ENHANCED_LSTMA_GET_USE_LAYERNORM_OUTPUT(x)           ((x)->useLayerNorm & CNN_ENHANCED_LSTMA_OUTPUT_LAYERNORM)
#define XI_CNN_ENHANCED_LSTMA_SET_USE_LAYERNORM_OUTPUT(x)           ((x)->useLayerNorm = \
                                                                     ((x)->useLayerNorm | CNN_ENHANCED_LSTMA_OUTPUT_LAYERNORM))
#define XI_CNN_ENHANCED_LSTMA_RESET_USE_LAYERNORM_OUTPUT(x)         ((x)->useLayerNorm = \
                                                                     ((x)->useLayerNorm & ~CNN_ENHANCED_LSTMA_OUTPUT_LAYERNORM))

#define XI_CNN_ENHANCED_LSTMA_GET_USE_PROJECTION(x)                 ((x)->useProjection)
#define XI_CNN_ENHANCED_LSTMA_SET_USE_PROJECTION(x, v)              ((x)->useProjection = (v))



_XI_API_ XI_ERR_TYPE xiConvolve3D_S_1x1j1_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_1x1j1_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_3x3j1_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_3x3j1_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_5x5j1_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_5x5j1_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_7x7j1_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_7x7j1_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_MxNj1_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_MxNj1_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_1x1j2_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_1x1j2_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                          const xi_pTile4D coeffTile,
                                                          const xi_pArray biasArray,
                                                          xi_pTile3D outTile,
                                                          xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_3x3j2_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_3x3j2_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_5x5j2_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_5x5j2_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_7x7j2_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_7x7j2_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_MxNj2_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_MxNj2_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_1x1j4_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_1x1j4_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_3x3j4_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_3x3j4_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_5x5j4_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_5x5j4_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_7x7j4_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_7x7j4_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_MxNj4_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_MxNj4_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_1x1_S8S8IXCa2_MOD_WHD_DWH(const xi_pTile3D inTile,
                                                              const xi_pTile4D coeffTile,
                                                              const xi_pArray biasArray,
                                                              xi_pTile3D outTile,
                                                              xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_3x3_S8S8IXCa2_MOD_WHD_DWH(const xi_pTile3D inTile,
                                                              const xi_pTile4D coeffTile,
                                                              const xi_pArray biasArray,
                                                              xi_pTile3D outTile,
                                                              xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_5x5_S8S8IXCa2_MOD_WHD_DWH(const xi_pTile3D inTile,
                                                              const xi_pTile4D coeffTile,
                                                              const xi_pArray biasArray,
                                                              xi_pTile3D outTile,
                                                              xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_7x7_S8S8IXCa2_MOD_WHD_DWH(const xi_pTile3D inTile,
                                                              const xi_pTile4D coeffTile,
                                                              const xi_pArray biasArray,
                                                              xi_pTile3D outTile,
                                                              xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_MxN_S8S8IXCa2_MOD_WHD_DWH(const xi_pTile3D inTile,
                                                              const xi_pTile4D coeffTile,
                                                              const xi_pArray biasArray,
                                                              xi_pTile3D outTile,
                                                              xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_1x1_S8S8IXCa2_MOD_DWH(const xi_pTile3D inTile,
                                                          const xi_pTile4D coeffTile,
                                                          const xi_pArray biasArray,
                                                          xi_pTile3D outTile,
                                                          xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_3x3_S8S8IXCa2_MOD_DWH(const xi_pTile3D inTile,
                                                          const xi_pTile4D coeffTile,
                                                          const xi_pArray biasArray,
                                                          xi_pTile3D outTile,
                                                          xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_5x5_S8S8IXCa2_MOD_DWH(const xi_pTile3D inTile,
                                                          const xi_pTile4D coeffTile,
                                                          const xi_pArray biasArray,
                                                          xi_pTile3D outTile,
                                                          xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_7x7_S8S8IXCa2_MOD_DWH(const xi_pTile3D inTile,
                                                          const xi_pTile4D coeffTile,
                                                          const xi_pArray biasArray,
                                                          xi_pTile3D outTile,
                                                          xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_MxN_S8S8IXCa2_MOD_DWH(const xi_pTile3D inTile,
                                                          const xi_pTile4D coeffTile,
                                                          const xi_pArray biasArray,
                                                          xi_pTile3D outTile,
                                                          xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_MxN_S8S8IX_SO_DWH(const xi_pTile3D inTile,
                                                      const xi_pTile4D coeffTile,
                                                      const xi_pArray biasArray,
                                                      xi_pTile3D outTile,
                                                      xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D_S_MxN_U8S8IX_SO_DWH(const xi_pTile3D inTile,
                                                      const xi_pTile4D coeffTile,
                                                      const xi_pArray biasArray,
                                                      xi_pTile3D outTile,
                                                      xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolve3D(const xi_pTile3D inTile,
                                  const xi_pTile4D coeffTile,
                                  const xi_pArray biasArray,
                                  xi_pTile3D outTile,
                                  xi_cnn_conv_params *param);

_XI_API_ void *xiGetConvolve3DVariant(const xi_pTile3D inTile,
                                      const xi_pTile4D coeffTile,
                                      const xi_pArray biasArray,
                                      xi_pTile3D outTile,
                                      xi_cnn_conv_params *param);

/* Dilation variants */
_XI_API_ XI_ERR_TYPE xiConvolved3D_S_1x1j1d1_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_1x1j1d1_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_3x3j1d1_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_3x3j1d1_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_5x5j1d1_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_5x5j1d1_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_7x7j1d1_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_7x7j1d1_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_MxNj1d1_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_MxNj1d1_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_1x1j2d1_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_1x1j2d1_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_3x3j2d1_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_3x3j2d1_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_5x5j2d1_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_5x5j2d1_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_7x7j2d1_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_7x7j2d1_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_MxNj2d1_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_MxNj2d1_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_1x1j4d1_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_1x1j4d1_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_3x3j4d1_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_3x3j4d1_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_5x5j4d1_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_5x5j4d1_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_7x7j4d1_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_7x7j4d1_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_MxNj4d1_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_MxNj4d1_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_3x3j1d2_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_3x3j1d2_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_3x3j1d4_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_3x3j1d4_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_5x5j1d2_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_5x5j1d2_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_5x5j1d4_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_5x5j1d4_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_7x7j1d2_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_7x7j1d2_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_7x7j1d4_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_7x7j1d4_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_MxNj1d2_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_MxNj1d2_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_MxNj1d4_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);
_XI_API_ XI_ERR_TYPE xiConvolved3D_S_MxNj1d4_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_1x1_S8S8IXCa2_MOD_WHD_DWH(const xi_pTile3D inTile,
                                                               const xi_pTile4D coeffTile,
                                                               const xi_pArray biasArray,
                                                               xi_pTile3D outTile,
                                                               const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_3x3_S8S8IXCa2_MOD_WHD_DWH(const xi_pTile3D inTile,
                                                               const xi_pTile4D coeffTile,
                                                               const xi_pArray biasArray,
                                                               xi_pTile3D outTile,
                                                               const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_5x5_S8S8IXCa2_MOD_WHD_DWH(const xi_pTile3D inTile,
                                                               const xi_pTile4D coeffTile,
                                                               const xi_pArray biasArray,
                                                               xi_pTile3D outTile,
                                                               const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_7x7_S8S8IXCa2_MOD_WHD_DWH(const xi_pTile3D inTile,
                                                               const xi_pTile4D coeffTile,
                                                               const xi_pArray biasArray,
                                                               xi_pTile3D outTile,
                                                               const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_MxN_S8S8IXCa2_MOD_WHD_DWH(const xi_pTile3D inTile,
                                                               const xi_pTile4D coeffTile,
                                                               const xi_pArray biasArray,
                                                               xi_pTile3D outTile,
                                                               const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_1x1_S8S8IXCa2_MOD_DWH(const xi_pTile3D inTile,
                                                           const xi_pTile4D coeffTile,
                                                           const xi_pArray biasArray,
                                                           xi_pTile3D outTile,
                                                           const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_3x3_S8S8IXCa2_MOD_DWH(const xi_pTile3D inTile,
                                                           const xi_pTile4D coeffTile,
                                                           const xi_pArray biasArray,
                                                           xi_pTile3D outTile,
                                                           const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_5x5_S8S8IXCa2_MOD_DWH(const xi_pTile3D inTile,
                                                           const xi_pTile4D coeffTile,
                                                           const xi_pArray biasArray,
                                                           xi_pTile3D outTile,
                                                           const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_7x7_S8S8IXCa2_MOD_DWH(const xi_pTile3D inTile,
                                                           const xi_pTile4D coeffTile,
                                                           const xi_pArray biasArray,
                                                           xi_pTile3D outTile,
                                                           const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_MxN_S8S8IXCa2_MOD_DWH(const xi_pTile3D inTile,
                                                           const xi_pTile4D coeffTile,
                                                           const xi_pArray biasArray,
                                                           xi_pTile3D outTile,
                                                           const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedA3D_S_1x1_S8S8U8Ca2_MOD_DWH(const xi_pTile3D inTile,
                                                            const xi_pTile fixUpTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_MxN_S8S8IX_SO_DWH(const xi_pTile3D inTile,
                                                       const xi_pTile4D coeffTile,
                                                       const xi_pArray biasArray,
                                                       xi_pTile3D outTile,
                                                       const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D_S_MxN_U8S8IX_SO_DWH(const xi_pTile3D inTile,
                                                       const xi_pTile4D coeffTile,
                                                       const xi_pArray biasArray,
                                                       xi_pTile3D outTile,
                                                       const xi_cnn_conv_params *param);


_XI_API_ XI_ERR_TYPE xiConvolvedFixupSubtract128A3D_1x1_U8S8S32_DWH(const xi_pTile3D inTile,
                                                                    const xi_pTile4D coeffTile,
                                                                    xi_pTile3D outTile,
                                                                    xi_pTile fixupTile,
                                                                    const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedFixupSubtract128A3D_1x1_U8S8S32Ia2_DWH(const xi_pTile3D inTile,
                                                                       const xi_pTile4D coeffTile,
                                                                       xi_pTile3D outTile,
                                                                       xi_pTile fixupTile,
                                                                       const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedFixupSubtract128A3D_1x1(const xi_pTile3D inTile,
                                                        const xi_pTile4D coeffTile,
                                                        xi_pTile3D outTile,
                                                        xi_pTile fixupTile,
                                                        const xi_cnna_conv_params *param);

_XI_API_ void *xiGetConvolvedFixupSubtract128A3D_1x1_Variant(const xi_pTile3D inTile,
                                                             const xi_pTile4D coeffTile,
                                                             xi_pTile3D outTile,
                                                             xi_pTile fixupTile,
                                                             const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedFixupA3D_MxN_U8S32_DWH(const xi_pTile3D inTile,
                                                       const xi_pTile4D coeffTile,
                                                       xi_pTile fixupTile,
                                                       const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedA3D_S_MxN_S8S8U8Ca2_MOD_DWH(const xi_pTile3D inTile,
                                                            const xi_pTile fixUpTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolved3D(const xi_pTile3D inTile,
                                   const xi_pTile4D coeffTile,
                                   const xi_pArray biasArray,
                                   xi_pTile3D outTile,
                                   const xi_cnn_conv_params *param);

_XI_API_ void *xiGetConvolvedA3DVariant(const xi_pTile3D inTile,
                                        const xi_pTile fixUpTile,
                                        const xi_pTile4D coeffTile,
                                        const xi_pArray biasArray,
                                        xi_pTile3D outTile,
                                        const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedA3D(const xi_pTile3D inTile,
                                    const xi_pTile fixUpTile,
                                    const xi_pTile4D coeffTile,
                                    const xi_pArray biasArray,
                                    xi_pTile3D outTile,
                                    const xi_cnna_conv_params *param);

_XI_API_ void *xiGetConvolved3DVariant(const xi_pTile3D inTile,
                                       const xi_pTile4D coeffTile,
                                       const xi_pArray biasArray,
                                       xi_pTile3D outTile,
                                       const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiFullyConnected3D(const xi_pTile3D inTile,
                                        const xi_pTile4D coeffTile,
                                        const xi_pArray biasArray,
                                        xi_pTile3D outTile,
                                        const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiFullyConnected3D_S_S8S8IX(const xi_pTile3D inTile,
                                                 const xi_pTile4D coeffTile,
                                                 const xi_pArray biasArray,
                                                 xi_pTile3D outTile,
                                                 const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiFullyConnected3D_S_U8S8IX(const xi_pTile3D inTile,
                                                 const xi_pTile4D coeffTile,
                                                 const xi_pArray biasArray,
                                                 xi_pTile3D outTile,
                                                 const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiLRNDepth3D(const xi_pTile3D inTile,
                                  const xi_pArray lutArray,
                                  xi_pTile3D outTile,
                                  const xi_cnn_lrn_depth_params *param);

_XI_API_ XI_ERR_TYPE xiLRNSpatial3D(const xi_pTile3D inTile,
                                    const xi_pArray lutArray,
                                    xi_pTile3D outTile,
                                    const xi_cnn_lrn_spatial_params *param);

_XI_API_ XI_ERR_TYPE xiLRNDepth3D_S_3_U8S8_WHD(const xi_pTile3D inTile,
                                               const xi_pArray lutArray,
                                               xi_pTile3D outTile,
                                               const xi_cnn_lrn_depth_params *param);

_XI_API_ XI_ERR_TYPE xiLRNDepth3D_S_5_U8S8_WHD(const xi_pTile3D inTile,
                                               const xi_pArray lutArray,
                                               xi_pTile3D outTile,
                                               const xi_cnn_lrn_depth_params *param);


_XI_API_ XI_ERR_TYPE xiLRNDepth3D_S_3_U8S8_DWH(const xi_pTile3D inTile,
                                               const xi_pArray lutArray,
                                               xi_pTile3D outTile,
                                               const xi_cnn_lrn_depth_params *param);

_XI_API_ XI_ERR_TYPE xiLRNDepth3D_S_5_U8S8_DWH(const xi_pTile3D inTile,
                                               const xi_pArray lutArray,
                                               xi_pTile3D outTile,
                                               const xi_cnn_lrn_depth_params *param);
_XI_API_ XI_ERR_TYPE xiLRNSpatial3D_S_3x3_U8S8_WHD(const xi_pTile3D inTile,
                                                   const xi_pArray lutArray,
                                                   xi_pTile3D outTile,
                                                   const xi_cnn_lrn_spatial_params *param);

_XI_API_ XI_ERR_TYPE xiLRNSpatial3D_S_5x5_U8S8_WHD(const xi_pTile3D inTile,
                                                   const xi_pArray lutArray,
                                                   xi_pTile3D outTile,
                                                   const xi_cnn_lrn_spatial_params *param);

_XI_API_ XI_ERR_TYPE xiLRNSpatial3D_S_3x3_U8S8_DWH(const xi_pTile3D inTile,
                                                   const xi_pArray lutArray,
                                                   xi_pTile3D outTile,
                                                   const xi_cnn_lrn_spatial_params *param);

_XI_API_ XI_ERR_TYPE xiLRNSpatial3D_S_5x5_U8S8_DWH(const xi_pTile3D inTile,
                                                   const xi_pArray lutArray,
                                                   xi_pTile3D outTile,
                                                   const xi_cnn_lrn_spatial_params *param);

_XI_API_ XI_ERR_TYPE xiLRNSpatial3D_generateLut(xi_pArray lutArray,
                                                xi_cnn_lrn_spatial_params *params,
                                                float alpha,
                                                float beta,
                                                float kValue,
                                                int32_t maxSumOfSquares,
                                                float qIn,
                                                float qOut);

_XI_API_ XI_ERR_TYPE xiLRNDepth3D_generateLut(xi_pArray lutArray,
                                              xi_cnn_lrn_depth_params *params,
                                              float alpha,
                                              float beta,
                                              float kValue,
                                              int32_t maxSumOfSquares,
                                              float qIn,
                                              float qOut);


_XI_API_ XI_ERR_TYPE xiMaxPool3D_MxNj1_S8_WHD(const xi_pTile3D inTile,
                                              xi_pTile3D outTile,
                                              const xi_cnn_pooling_params *param);

_XI_API_ XI_ERR_TYPE xiMaxPool3D_MxNj1_U8_WHD(const xi_pTile3D inTile,
                                              xi_pTile3D outTile,
                                              const xi_cnn_pooling_params *param);

_XI_API_ XI_ERR_TYPE xiMaxPool3D_MxNj1_S16_WHD(const xi_pTile3D inTile,
                                               xi_pTile3D outTile,
                                               const xi_cnn_pooling_params *param);

_XI_API_ XI_ERR_TYPE xiMaxPool3D_MxN_S8_DWH(const xi_pTile3D inTile,
                                            xi_pTile3D outTile,
                                            const xi_cnn_pooling_params *param);

_XI_API_ XI_ERR_TYPE xiMaxPool3D_MxN_U8_DWH(const xi_pTile3D inTile,
                                            xi_pTile3D outTile,
                                            const xi_cnn_pooling_params *param);

_XI_API_ XI_ERR_TYPE xiMaxPool3D_MxN_S16_DWH(const xi_pTile3D inTile,
                                             xi_pTile3D outTile,
                                             const xi_cnn_pooling_params *param);


_XI_API_ XI_ERR_TYPE xiMaxPool3D_MxNj2_S8_WHD(const xi_pTile3D inTile,
                                              xi_pTile3D outTile,
                                              const xi_cnn_pooling_params *param);

_XI_API_ XI_ERR_TYPE xiMaxPool3D_MxNj2_U8_WHD(const xi_pTile3D inTile,
                                              xi_pTile3D outTile,
                                              const xi_cnn_pooling_params *param);

_XI_API_ XI_ERR_TYPE xiMaxPool3D_MxNj2_S16_WHD(const xi_pTile3D inTile,
                                               xi_pTile3D outTile,
                                               const xi_cnn_pooling_params *param);

_XI_API_ XI_ERR_TYPE xiMaxPool3D(const xi_pTile3D inTile,
                                 xi_pTile3D outTile,
                                 const xi_cnn_pooling_params *param);

_XI_API_ XI_ERR_TYPE xiMaxPool3D_MxN_S8_WHD(const xi_pTile3D inTile,
                                            xi_pTile3D outTile,
                                            const xi_cnn_pooling_params *param);

_XI_API_ XI_ERR_TYPE xiMaxPool3D_MxN_U8_WHD(const xi_pTile3D inTile,
                                            xi_pTile3D outTile,
                                            const xi_cnn_pooling_params *param);

_XI_API_ XI_ERR_TYPE xiMaxPool3D_MxN_S16_WHD(const xi_pTile3D inTile,
                                             xi_pTile3D outTile,
                                             const xi_cnn_pooling_params *param);

_XI_API_ XI_ERR_TYPE xiAvgPool3D_MxNj1_S8_WHD(const xi_pTile3D inTile,
                                              xi_pArray bufArray,
                                              xi_pTile3D outTile,
                                              const xi_cnn_pooling_params *param);

_XI_API_ XI_ERR_TYPE xiAvgPool3D_MxNj1_U8_WHD(const xi_pTile3D inTile,
                                              xi_pArray bufArray,
                                              xi_pTile3D outTile,
                                              const xi_cnn_pooling_params *param);

_XI_API_ XI_ERR_TYPE xiAvgPool3D_MxN_S8_DWH(const xi_pTile3D inTile,
                                            xi_pTile3D outTile,
                                            const xi_cnn_pooling_params *param);

_XI_API_ XI_ERR_TYPE xiAvgPool3D_MxN_U8_DWH(const xi_pTile3D inTile,
                                            xi_pTile3D outTile,
                                            const xi_cnn_pooling_params *param);

_XI_API_ XI_ERR_TYPE xiAvgPoolA3D_MxN_U8_DWH(const xi_pTile3D inTile,
                                             xi_pTile3D outTile,
                                             const xi_cnn_pooling_params *param,
                                             const xi_size3D frame3DSize);

_XI_API_ XI_ERR_TYPE xiAvgPoolA3D_MxN_S8_DWH(const xi_pTile3D inTile,
                                             xi_pTile3D outTile,
                                             const xi_cnn_pooling_params *param,
                                             const xi_size3D frame3DSize);

_XI_API_ XI_ERR_TYPE xiAvgPool3D_MxNj2_S8_WHD(const xi_pTile3D inTile,
                                              xi_pTile3D outTile,
                                              const xi_cnn_pooling_params *param);

_XI_API_ XI_ERR_TYPE xiAvgPool3D_MxNj2_U8_WHD(const xi_pTile3D inTile,
                                              xi_pTile3D outTile,
                                              const xi_cnn_pooling_params *param);

_XI_API_ XI_ERR_TYPE xiAvgPool3D_MxNj1_S16_WHD(const xi_pTile3D inTile,
                                               xi_pArray bufArray,
                                               xi_pTile3D outTile,
                                               const xi_cnn_pooling_params *param);

_XI_API_ XI_ERR_TYPE xiAvgPool3D_MxNj2_S16_WHD(const xi_pTile3D inTile,
                                               xi_pTile3D outTile,
                                               const xi_cnn_pooling_params *param);

_XI_API_ XI_ERR_TYPE xiAvgPool3D_MxN_S16_DWH(const xi_pTile3D inTile,
                                             xi_pTile3D outTile,
                                             const xi_cnn_pooling_params *param);

_XI_API_ XI_ERR_TYPE xiAvgPool3D(const xi_pTile3D inTile,
                                 xi_pArray bufArray,
                                 xi_pTile3D outTile,
                                 const xi_cnn_pooling_params *param);

_XI_API_ XI_ERR_TYPE xiROIAlignQuantizeA3D_S8_DWH(const xi_pTile3D inTile,
                                                  const xi_pArray RoIParam,
                                                  const xi_pArray batchSplitData,
                                                  xi_pTile4D outTile,
                                                  const xi_cnn_roi_align_params *pparams,
                                                  xi_pArray posWeightArr);

_XI_API_ XI_ERR_TYPE xiROIAlignQuantizeA3D_U8_DWH(const xi_pTile3D inTile,
                                                  const xi_pArray RoIParam,
                                                  const xi_pArray batchSplitData,
                                                  xi_pTile4D outTile,
                                                  const xi_cnn_roi_align_params *pparams,
                                                  xi_pArray posWeightArr);

_XI_API_ XI_ERR_TYPE xiTanh_generateLut(xi_pArray lutArray,
                                        const int32_t inpDataType,
                                        const uint8_t lutQfactor,
                                        const float qIn);

_XI_API_ XI_ERR_TYPE xiSigmoid_generateLut(xi_pArray lutArray,
                                           const int32_t inpDataType,
                                           const uint8_t lutQfactor,
                                           const float qIn);

_XI_API_ XI_ERR_TYPE xiSoftmax_generateLut(xi_pArray lutArray,
                                           xi_cnn_softmax_params *params,
                                           const uint16_t qFactorLUT,
                                           const float qin);

_XI_API_ XI_ERR_TYPE xiLUT3D_S8I16(const xi_pTile3D inTile,
                                   const xi_pArray lutArray,
                                   xi_pTile3D outTile);

_XI_API_ XI_ERR_TYPE xiFillTile3D(xi_pTile3D dstTile,
                                  const int32_t value,
                                  xi_bool fill_edge_extension);

_XI_API_ XI_ERR_TYPE xiFillTile3D_I8(xi_pTile3D dstTile,
                                     const int32_t value,
                                     xi_bool fill_edge_extension);

_XI_API_ XI_ERR_TYPE xiFillTile3D_I16(xi_pTile3D dstTile,
                                      const int32_t value,
                                      xi_bool fill_edge_extension);

_XI_API_ XI_ERR_TYPE xiExtendEdgesConst3D(xi_pTile3D dstTile,
                                          const int32_t value,
                                          xi_size3D frame3DSize);

_XI_API_ XI_ERR_TYPE xiExtendEdgesConst3D_I8(xi_pTile3D dstTile,
                                             const int32_t value,
                                             xi_size3D frame3DSize);

_XI_API_ XI_ERR_TYPE xiExtendEdgesConst3D_I16(xi_pTile3D dstTile,
                                              const int32_t value,
                                              xi_size3D frame3DSize);

_XI_API_ XI_ERR_TYPE xiExtendEdges3D(xi_pTile3D dstTile,
                                     const xi_pArray pArray,
                                     xi_size3D frame3DSize);

_XI_API_ XI_ERR_TYPE xiExtendEdges3D_I8(xi_pTile3D dstTile,
                                        const xi_pArray pArray,
                                        xi_size3D frame3DSize);

_XI_API_ XI_ERR_TYPE xiExtendEdges3D_I16(xi_pTile3D dstTile,
                                         const xi_pArray pArray,
                                         xi_size3D frame3DSize);

_XI_API_ XI_ERR_TYPE xiCopyTile3D(const xi_pTile3D inTile,
                                  xi_pTile3D outTile,
                                  xi_bool copy_edge_extension);

_XI_API_ XI_ERR_TYPE xiTranspose3D(const xi_pTile3D inTile,
                                   xi_pTile3D outTile);

_XI_API_ XI_ERR_TYPE xiTranspose3D_I8_WHD_DWH(const xi_pTile3D inTile,
                                              xi_pTile3D outTile);

_XI_API_ XI_ERR_TYPE xiTranspose3D_I8_DWH_WHD(const xi_pTile3D inTile,
                                              xi_pTile3D outTile);

_XI_API_ XI_ERR_TYPE xiUnsignedToSigned3D_U8S8(xi_pTile3D inTile,
                                               xi_pTile3D outTile);

_XI_API_ XI_ERR_TYPE xiLeakyRELU(const xi_pTile3D inTile,
                                 xi_pTile3D outTile,
                                 const XI_Q15 slope);

_XI_API_ XI_ERR_TYPE xiLeakyRELU_S8(const xi_pTile3D inTile,
                                    xi_pTile3D outTile,
                                    const XI_Q15 slope);

_XI_API_ XI_ERR_TYPE xiLeakyRELU_S16(const xi_pTile3D inTile,
                                     xi_pTile3D outTile,
                                     const XI_Q15 slope);

_XI_API_ XI_ERR_TYPE xiRELUA_U8(const xi_pTile3D inTile,
                                xi_pTile3D outTile,
                                const uint8_t minVal,
                                const uint8_t maxVal);

_XI_API_ XI_ERR_TYPE xiRELUA_S8(const xi_pTile3D inTile,
                                xi_pTile3D outTile,
                                const int8_t minVal,
                                const int8_t maxVal);

_XI_API_ XI_ERR_TYPE xiArgmax_S8(const xi_pTile3D inTile,
                                 xi_pTile3D outTileIdx,
                                 xi_pTile3D outTileVal,
                                 xi_pTile extraValCnt,
                                 xi_pArray sortedIdxArr,
                                 xi_pArray sortedValArr,
                                 const uint16_t numLargestVal);

_XI_API_ XI_ERR_TYPE xiSoftmax_S16U16(const xi_pArray input,
                                      const xi_pArray lutArray,
                                      xi_pArray output,
                                      const xi_cnn_softmax_params *params);

_XI_API_ XI_ERR_TYPE xiEltwiseAdd3D(const xi_pTile3D inTile1,
                                    const xi_pTile3D inTile2,
                                    xi_pTile3D outTile,
                                    const xi_cnn_eltwise_params *param);

_XI_API_ XI_ERR_TYPE xiEltwiseAdd3D_S8(const xi_pTile3D inTile1,
                                       const xi_pTile3D inTile2,
                                       xi_pTile3D outTile,
                                       const xi_cnn_eltwise_params *param);

_XI_API_ XI_ERR_TYPE xiEltwiseAdd3D_S16(const xi_pTile3D inTile1,
                                        const xi_pTile3D inTile2,
                                        xi_pTile3D outTile,
                                        const xi_cnn_eltwise_params *param);

_XI_API_ XI_ERR_TYPE xiRenorm3D(const xi_pTile3D inTile,
                                xi_pTile3D outTile,
                                const uint16_t renormScale,
                                const uint8_t renormShift);

_XI_API_ XI_ERR_TYPE xiRenorm3D_U8(const xi_pTile3D inTile,
                                   xi_pTile3D outTile,
                                   const uint16_t renormScale,
                                   const uint8_t renormShift);

_XI_API_ XI_ERR_TYPE xiRenorm3D_S8(const xi_pTile3D inTile,
                                   xi_pTile3D outTile,
                                   const uint16_t renormScale,
                                   const uint8_t renormShift);

_XI_API_ XI_ERR_TYPE xiDataConversion3D(const xi_pTile3D inTile,
                                        xi_pTile3D outTile,
                                        const uint16_t scale,
                                        const uint8_t shift);

_XI_API_ XI_ERR_TYPE xiDataConversion3D_S16I8(const xi_pTile3D inTile,
                                              xi_pTile3D outTile,
                                              const uint16_t scale,
                                              const uint8_t shift);

_XI_API_ XI_ERR_TYPE xiDataConversion3D_S8S16(const xi_pTile3D inTile,
                                              xi_pTile3D outTile,
                                              const uint16_t scale,
                                              const uint8_t shift);

_XI_API_ XI_ERR_TYPE xiDataConversion3D_U8S16(const xi_pTile3D inTile,
                                              xi_pTile3D outTile,
                                              const uint16_t scale,
                                              const uint8_t shift);

_XI_API_ XI_ERR_TYPE xiMaxout3D_S8_WHD(const xi_pTile3D inTile,
                                       xi_pTile3D outTile,
                                       const uint16_t kSize);

_XI_API_ XI_ERR_TYPE xiMaxout3D_S8_DWH(const xi_pTile3D inTile,
                                       xi_pTile3D outTile,
                                       const uint16_t kSize);

_XI_API_ XI_ERR_TYPE xiMaxout3D(const xi_pTile3D inTile,
                                xi_pTile3D outTile,
                                const uint16_t kSize);

_XI_API_ XI_ERR_TYPE xiMeanSubtraction3D_U8S8(const xi_pTile3D inTile,
                                              xi_pTile3D outTile,
                                              const uint8_t mean,
                                              const uint16_t scale,
                                              const uint8_t shift);

_XI_API_ XI_ERR_TYPE xiWrapper3D_TYPE_1(const xi_pTile3D inTile,
                                        xi_pTile3D outTile,
                                        void *function2DPtr);

_XI_API_ XI_ERR_TYPE xiWrapper3D_TYPE_2(const xi_pTile3D inTile,
                                        xi_pTile3D outTile,
                                        int32_t value,
                                        void *function2DPtr);

_XI_API_ XI_ERR_TYPE xiWrapper3D_TYPE_3(const xi_pTile3D inTile,
                                        xi_pTile3D outTile,
                                        xi_pArray pArray,
                                        void *function2DPtr);

_XI_API_ XI_ERR_TYPE xiWrapper3D_TYPE_4(const xi_pTile3D inTile0,
                                        const xi_pTile3D inTile1,
                                        xi_pTile3D outTile,
                                        void *function2DPtr);

_XI_API_ XI_ERR_TYPE xiWrapper3D_TYPE_5(const xi_pTile3D inTile0,
                                        const xi_pTile3D inTile1,
                                        xi_pTile3D outTile,
                                        int32_t value,
                                        void *function2DPtr);

_XI_API_ XI_ERR_TYPE xiWrapper3D_TYPE_6(const xi_pTile3D inTile,
                                        xi_pTile3D outTile,
                                        xi_pTile tmpTile,
                                        int32_t value,
                                        void *function2DPtr);

_XI_API_ XI_ERR_TYPE xiWrapper3D_TYPE_7(const xi_pTile3D inTile,
                                         int32_t *counter,
                                         void *function2DPtr);

_XI_API_ XI_ERR_TYPE xiWrapper3D_TYPE_8(const xi_pTile3D inTile,
                                        int32_t value,
                                        int32_t *counter,
                                        void *function2DPtr);

_XI_API_ XI_ERR_TYPE xiWrapper3D_TYPE_9(const xi_pTile3D inTile,
                                        xi_pTile3D outTile,
                                        XI_Q13_18 xscale,
                                        XI_Q13_18 yscale,
                                        XI_Q13_18 xshift,
                                        XI_Q13_18 yshift,
                                        void *function2DPtr);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_3x3j1_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_3x3j1_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_5x5j1_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);


_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_5x5j1_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_7x7j1_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_7x7j1_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_MxNj1_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_MxNj1_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);


_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_3x3j2_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_3x3j2_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_5x5j2_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_5x5j2_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);


_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_7x7j2_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);


_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_7x7j2_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);


_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_MxNj2_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);


_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_MxNj2_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);



_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_3x3j4_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_3x3j4_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);


_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_5x5j4_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_5x5j4_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_7x7j4_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);


_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_7x7j4_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_MxNj4_S8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);


_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_MxNj4_U8S8IX_MOW_WHD(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_3x3_S8S8IXCa2_MOD_DWH(const xi_pTile3D inTile,
                                                                   const xi_pTile3D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_5x5_S8S8IXCa2_MOD_DWH(const xi_pTile3D inTile,
                                                                   const xi_pTile3D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_7x7_S8S8IXCa2_MOD_DWH(const xi_pTile3D inTile,
                                                                   const xi_pTile3D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_MxN_S8S8IXCa2_MOD_DWH(const xi_pTile3D inTile,
                                                                   const xi_pTile3D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolve2D(const xi_pTile3D inTile,
                                           const xi_pTile3D coeffTile,
                                           const xi_pArray biasArray,
                                           xi_pTile3D outTile,
                                           const xi_cnn_conv_params *param);

_XI_API_ void *xiGetDepthwiseConvolveA2DVariant(const xi_pTile3D inTile,
                                                const xi_pTile3D coeffTile,
                                                const xi_pArray biasArray,
                                                xi_pTile3D outTile,
                                                const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolveA2D(const xi_pTile3D inTile,
                                            const xi_pTile3D coeffTile,
                                            const xi_pArray biasArray,
                                            xi_pTile3D outTile,
                                            const xi_cnna_conv_params *param);

_XI_API_ void *xiGetDepthwiseConvolve2DVariant(const xi_pTile3D inTile,
                                               const xi_pTile3D coeffTile,
                                               const xi_pArray biasArray,
                                               xi_pTile3D outTile,
                                               const xi_cnn_conv_params *param);

// Sigmoid API definitions

// API for functions meant to be compiled and run on host processor

// Creates fixed point sigmoid parameters for sigmoid parameters input_scale and offset
_XI_API_ XI_ERR_TYPE xiSigmoidA_createParams_c(const float input_scale, const int32_t offset, xi_cnn_sigmoidA_params *pparams);

// Creates fixed point tanh parameters for tanh parameters input_scale and offset
_XI_API_ XI_ERR_TYPE xiTanhA_createParams_c(const float input_scale, const int32_t offset, xi_cnn_tanhA_params *pparams);

// Generates 256 entry lookup table sigmoidArray with 8 bit entries, for use by Sigmoid function using sigmoid parameters pparams
_XI_API_ XI_ERR_TYPE xiSigmoidA_generateLUT_I8_c(xi_pArray sigmoidArray, const xi_cnn_sigmoidA_params * pparams);

// Generates 256 entry lookup table tanhArray with 8 bit entries, for use by tanh function using tanh parameters pparams
_XI_API_ XI_ERR_TYPE xiTanhA_generateLUT_I8_c(xi_pArray tanhArray, const xi_cnn_tanhA_params * pparams);

// API for functions compiled and run on IVP core

_XI_API_ XI_ERR_TYPE xiSigmoidA3D_U8_LUT(const xi_pTile3D inTile, const xi_pArray sigmoidArray, xi_pTile3D outTile);
_XI_API_ XI_ERR_TYPE xiSigmoidA3D_S8_LUT(const xi_pTile3D inTile, const xi_pArray sigmoidArray, xi_pTile3D outTile);
// Uses 256 entry lookup table sigmoidArray to get 8bit Sigmoid output tile for 8bit input tile

_XI_API_ XI_ERR_TYPE xiSigmoidA_generateLUT_U8(xi_pArray sigmoidArray, const xi_cnn_sigmoidA_params * pparams);
_XI_API_ XI_ERR_TYPE xiSigmoidA_generateLUT_S8(xi_pArray sigmoidArray, const xi_cnn_sigmoidA_params * pparams);
// Generates 256 entry lookup table sigmoidArray with 8 bit entries, for use by Sigmoid function using sigmoid parameters pparams using IVP specific code, meant to be compiled and run
// only on IVP core

_XI_API_ XI_ERR_TYPE xiSigmoidA3D_U8(const xi_pTile3D inTile, const xi_pArray sigmoidArray, xi_pTile3D outTile, const xi_cnn_sigmoidA_params* pparams);
_XI_API_ XI_ERR_TYPE xiSigmoidA3D_S8(const xi_pTile3D inTile, const xi_pArray sigmoidArray, xi_pTile3D outTile, const xi_cnn_sigmoidA_params* pparams);
// Implements single pass sigmoid by generating lookup table on the fly and using the lookup table in a single function

_XI_API_ XI_ERR_TYPE xiTanhA3D_U8_LUT(const xi_pTile3D inTile, const xi_pArray tanhArray, xi_pTile3D outTile);
_XI_API_ XI_ERR_TYPE xiTanhA3D_S8_LUT(const xi_pTile3D inTile, const xi_pArray tanhArray, xi_pTile3D outTile);
// Uses 256 entry lookup table tanhArray to get 8bit Tanh output tile for 8bit input tile

_XI_API_ XI_ERR_TYPE xiTanhA_generateLUT_U8(xi_pArray tanhArray, const xi_cnn_tanhA_params * pparams);
_XI_API_ XI_ERR_TYPE xiTanhA_generateLUT_S8(xi_pArray tanhArray, const xi_cnn_tanhA_params * pparams);
// Generates 256 entry lookup table tanhArray with 8 bit entries, for use by Tanh function using Tanh parameters pparams using IVP specific code, meant to be compiled and run
// only on IVP core

_XI_API_ XI_ERR_TYPE xiTanhA3D_U8(const xi_pTile3D inTile, const xi_pArray tanhArray, xi_pTile3D outTile, const xi_cnn_tanhA_params* pparams);
_XI_API_ XI_ERR_TYPE xiTanhA3D_S8(const xi_pTile3D inTile, const xi_pArray tanhArray, xi_pTile3D outTile, const xi_cnn_tanhA_params* pparams);
// Implements single pass tanh by generating lookup table on the fly and using the lookup table in a single function

//Softmax API definitions
// API for functions meant to be compiled and run on host processor

_XI_API_ XI_ERR_TYPE xiSoftmaxA_createParams_c(const float input_scale, const float beta, const int32_t axis, xi_cnn_softmaxA_params *pparams);
// Creates fixed point softmax parameters pparams for parameters input_scale and beta


_XI_API_ XI_ERR_TYPE xiSoftmaxA_generateLUT_ref(xi_pArray softmaxArray, const xi_cnn_softmaxA_params * pparams);
// Generates 256 entry lookup table softmaxArray with 32bit entries for use by Softmax function using softmax parameters pparams


// API for functions compiled and run on IVP core
_XI_API_ XI_ERR_TYPE xiSoftmaxA3D_U8(const xi_pTile3D inTile, const xi_pArray softmaxArray, xi_pTile3D outTile, const xi_cnn_softmaxA_params * pparams);

_XI_API_ XI_ERR_TYPE xiSoftmaxA3D_S8(const xi_pTile3D inTile, const xi_pArray softmaxArray, xi_pTile3D outTile, const xi_cnn_softmaxA_params * pparams);

_XI_API_ XI_ERR_TYPE xiSoftmaxA3D_U8_LUT(const xi_pTile3D inTile, const xi_pArray softmaxArray, xi_pTile3D outTile, const xi_cnn_softmaxA_params * pparams);

_XI_API_ XI_ERR_TYPE xiSoftmaxA3D_S8_LUT(const xi_pTile3D inTile, const xi_pArray softmaxArray, xi_pTile3D outTile, const xi_cnn_softmaxA_params * pparams);
// Uses 256 entry lookup table softmaxArray to get 8bit Softmax output tile for 8bit input tile for given softmax-parameter

_XI_API_ XI_ERR_TYPE xiSoftmaxA3D_U8_DWH_LUTReused(const xi_pTile3D inTile, const xi_pArray softmaxArray, xi_pTile3D outTile, const xi_cnn_softmaxA_params  * pparams, xi_pArray tmp);

_XI_API_ XI_ERR_TYPE xiSoftmaxA3D_S8_DWH_LUTReused(const xi_pTile3D inTile, const xi_pArray softmaxArray, xi_pTile3D outTile, const xi_cnn_softmaxA_params  * pparams, xi_pArray tmp);
// A more efficient implementation of xiSoftmaxA3D_U8_DWH_LUT which reuses lookedup values using a temporary array tmp.
// The temporary array must of size 4*depth

_XI_API_ XI_ERR_TYPE xiSoftmaxA_generateLUT(xi_pArray softmaxArray, const xi_cnn_softmaxA_params * pparams);
// Generates 256 entry lookup table softmaxArray with 32 bit entries, for use by Softmax function using softmax parameters pparams using IVP specific code, meant to be compiled and run
// only on IVP core

// Exponential Linear unit

// Creates fixed point ELU parameters for ELU parameters input_scale and offset
_XI_API_ XI_ERR_TYPE xiELUA_createParams_c(const float input_scale, const float output_scale, const int32_t inputoffset, const int32_t outputoffset, const int32_t leftShift, xi_cnn_ELUA_params *pparams);

_XI_API_ XI_ERR_TYPE xiELUA3D_U8_LUT(const xi_pTile3D inTile, const xi_pArray ELUArray, xi_pTile3D outTile);
_XI_API_ XI_ERR_TYPE xiELUA3D_S8_LUT(const xi_pTile3D inTile, const xi_pArray ELUArray, xi_pTile3D outTile);
//Uses 256 entry lookup table ELUArray to get 8bit ELU output tile for 8bit input tile

_XI_API_ XI_ERR_TYPE xiELUA_generateLUT_I8_c(xi_pArray eluArray, const xi_cnn_ELUA_params * pparams);
// Generates 256 entry lookup table ELUArray with 8 bit entries, for use by ELU function using ELU parameters pparams using IVP specific code, meant to be compiled and run
// only on IVP core

_XI_API_ XI_ERR_TYPE xiELUA_generateLUT_U8(xi_pArray eluArray, const xi_cnn_ELUA_params * pparams);
_XI_API_ XI_ERR_TYPE xiELUA_generateLUT_S8(xi_pArray eluArray, const xi_cnn_ELUA_params * pparams);
// Generates 256 entry lookup table eluArray with 8 bit entries, for use by elu function using elu parameters pparams using IVP specific code, meant to be compiled and run
// only on IVP core

_XI_API_ XI_ERR_TYPE xiELUA3D_U8(const xi_pTile3D inTile, const xi_pArray ELUArray, xi_pTile3D outTile, const xi_cnn_ELUA_params* pparams);
_XI_API_ XI_ERR_TYPE xiELUA3D_S8(const xi_pTile3D inTile, const xi_pArray ELUArray, xi_pTile3D outTile, const xi_cnn_ELUA_params* pparams);
// Implements single pass elu by generating lookup table on the fly and using the lookup table in a single function

// L2 Norm
_XI_API_ XI_ERR_TYPE xiL2NormA3D_U8_DWH(const xi_pTile3D inTile,
                                        xi_pTile3D outTile,
                                        const xi_cnn_l2NormA_params* pparams);

_XI_API_ XI_ERR_TYPE xiL2NormA3D_S8_DWH(const xi_pTile3D inTile,
                                        xi_pTile3D outTile,
                                        const xi_cnn_l2NormA_params* pparams);

_XI_API_ XI_ERR_TYPE xiDeconvolveA_generateFilter_c(const xi_size3D inFrame3DSize, xi_tile4D *coeffTile,
                                                    const xi_size3D outFrame3DSize, xi_tile4D *newCoeffTile,
                                                    xi_array *constTermArray, xi_cnn_deconvolveA_params *params);

_XI_API_ XI_ERR_TYPE xiDeconvolveA3D_MxN_U8U8U8_MOD_DWH(xi_tile3D *inTile, xi_pTile fixUpTile, xi_array *biasArray,
                                                        xi_tile3D *outTile, xi_cnn_deconvolveA_params *params,
                                                        xi_tile4D *newCoeffTile, xi_array *constTermArray);


_XI_API_ XI_ERR_TYPE xiDeconvolveAVQ3D_MxN_S8_MOD_DWH(xi_tile3D *inTile,
                                                             xi_array *biasArray,
                                                             const xi_pArray outScaleArray,
                                                             const xi_pArray outShiftArray,
                                                             xi_tile3D *outTile,
                                                             xi_cnn_deconvolveA_params *params,
                                                             xi_tile4D *newCoeffTile,
                                                             xi_array *constTermArray);

_XI_API_ XI_ERR_TYPE xiTransposeConv2DAGetDim4D_NDWH(const xi_pTile4D coeffTile,
                                                     xi_pTile4D subCoeffInfo[],
                                                     xi_pTile4D superCoeffInfo[],
                                                     uint16_t *numSubKernels,
                                                     uint16_t *numSuperKernels,
                                                     uint8_t strideX,
                                                     uint8_t strideY,
                                                     uint8_t getNumKernelsFlag);

_XI_API_ XI_ERR_TYPE xiTransposeConv2DAReOrder4D_I8_NDWH(const xi_pTile4D inTile,
                                                         xi_pTile4D subCoeffs[],
                                                         xi_pTile4D superCoeffs[],
                                                         const xi_cnna_conv_params *param,
                                                         const uint8_t transposeCoeffsFlag);

_XI_API_ XI_ERR_TYPE xiBiasExtend_S32_MOD(const xi_pArray inBiasArray,
                                          xi_pArray outBiasArray);

_XI_API_ XI_ERR_TYPE xiPreluA3D_U8_DWH(const xi_pTile3D inTile, xi_array *slopeArray,
                                       xi_pTile3D outTile, const xi_cnn_preluA_params *params,
                                       xi_array *scratchBuffArray);

_XI_API_ XI_ERR_TYPE xiPreluA3D_S8_DWH(const xi_pTile3D inTile,
                                       xi_array *slopeArray,
                                       xi_pTile3D outTile,
                                       const xi_cnn_preluA_params *params,
                                       xi_array *scratchBuffArray);

_XI_API_ XI_ERR_TYPE xiPreluA3D_U8U8U8_DWH(const xi_pTile3D inTile,
                                           const xi_pTile3D slopeTile,
                                           xi_pTile3D outTile,
                                           const xi_cnn_preluA_params *param);


_XI_API_ XI_ERR_TYPE xiPreluQuantizeA3D_S8_DWH(const xi_pTile3D inTile,
                                               const xi_pTile3D slopeTile,
                                               xi_pTile3D outTile,
                                               const xi_cnn_preluA_params *param);

_XI_API_ XI_ERR_TYPE xiHardSwishA3D_setinfo_c(const float inScale, const float outScale,
                                              const int32_t inZP, const int32_t outZP,
                                              xi_cnn_hardSwishA3D_params *params);

_XI_API_ XI_ERR_TYPE xiHardSwishA3D_U8(const xi_pTile3D inTile,
                                       xi_pTile3D outTile,
                                       const xi_cnn_hardSwishA3D_params *params);

_XI_API_ XI_ERR_TYPE xiHardSwishA3D_S8(const xi_pTile3D inTile,
                                       xi_pTile3D outTile,
                                       const xi_cnn_hardSwishA3D_params *params);

_XI_API_ XI_ERR_TYPE xiComputeGateEnhancedLSTMA_U8(const xi_pArray inputArray,
                                                   const xi_pArray inputToInputWeightsArray,
                                                   const xi_pArray recurrentToInputWeightsArray,
                                                   const xi_pArray inputToForgetWeightsArray,
                                                   const xi_pArray recurrentToForgetWeightsArray,
                                                   const xi_pArray inputToCellWeightsArray,
                                                   const xi_pArray recurrentToCellWeightsArray,
                                                   const xi_pArray inputToOutputWeightsArray,
                                                   const xi_pArray recurrentToOutputWeightsArray,
                                                   const xi_pArray prevOutputArray,
                                                   const xi_pArray inputToInputEffectiveBiasArray,
                                                   const xi_pArray recurrentToInputEffectiveBiasArray,
                                                   const xi_pArray inputToForgetEffectiveBiasArray,
                                                   const xi_pArray recurrentToForgetEffectiveBiasArray,
                                                   const xi_pArray inputToCellEffectiveBiasArray,
                                                   const xi_pArray recurrentToCellEffectiveBiasArray,
                                                   const xi_pArray inputToOutputEffectiveBiasArray,
                                                   const xi_pArray recurrentToOutputEffectiveBiasArray,
                                                   xi_pArray inputGateBuffer,
                                                   xi_pArray forgetGateBuffer,
                                                   xi_pArray cellGateBuffer,
                                                   xi_pArray outputGateBuffer,
                                                   xi_cnn_enhanced_LSTMA_params* params);

_XI_API_ XI_ERR_TYPE xiComputeStateEnhancedLSTMA_U8(const xi_pArray inputLayerNormArray,
                                                    const xi_pArray forgetLayerNormArray,
                                                    const xi_pArray cellLayerNormArray,
                                                    const xi_pArray outputLayerNormArray,
                                                    const xi_pArray prevCellStateArray,
                                                    const xi_pArray inputBiasArray,
                                                    const xi_pArray forgetBiasArray,
                                                    const xi_pArray cellBiasArray,
                                                    const xi_pArray outputBiasArray,
                                                    const xi_pArray cellToInputArray,
                                                    const xi_pArray cellToForgetArray,
                                                    const xi_pArray cellToOutputArray,
                                                    xi_pArray inputGateBuffer,
                                                    xi_pArray forgetGateBuffer,
                                                    xi_pArray cellGateBuffer,
                                                    xi_pArray outputGateBuffer,
                                                    xi_pArray outputBuffer8bit,
                                                    xi_pArray cellStateArray,
                                                    xi_cnn_enhanced_LSTMA_params* params);

_XI_API_ XI_ERR_TYPE xiProjectionEnhancedLSTMA_U8(xi_pArray projectionInputArray,
                                                  xi_pArray projectionEffectiveBiasArray,
                                                  xi_pArray projectionWeightsArray,
                                                  xi_pArray outputArray,
                                                  xi_pArray outputStateArray,
                                                  xi_cnn_enhanced_LSTMA_params* params);

_XI_API_ XI_ERR_TYPE xiPrecomputeBiasEnhancedLSTM(const xi_pArray inputToInputWeightsArray,
                                                  const xi_pArray recurrentToInputWeightsArray,
                                                  const xi_pArray inputToForgetWeightsArray,
                                                  const xi_pArray recurrentToForgetWeightsArray,
                                                  const xi_pArray inputToCellWeightsArray,
                                                  const xi_pArray recurrentToCellWeightsArray,
                                                  const xi_pArray inputToOutputWeightsArray,
                                                  const xi_pArray recurrentToOutputWeightsArray,
                                                  xi_pArray inputToInputEffectiveBiasArray,
                                                  xi_pArray recurrentToInputEffectiveBiasArray,
                                                  xi_pArray inputToForgetEffectiveBiasArray,
                                                  xi_pArray recurrentToForgetEffectiveBiasArray,
                                                  xi_pArray inputToCellEffectiveBiasArray,
                                                  xi_pArray recurrentToCellEffectiveBiasArray,
                                                  xi_pArray inputToOutputEffectiveBiasArray,
                                                  xi_pArray recurrentToOutputEffectiveBiasArray,
                                                  xi_cnn_enhanced_LSTMA_params* params);

_XI_API_ XI_ERR_TYPE xiPrecomputeBiasProjectionEnhancedLSTM(const xi_pArray projectionWeightsArray,
                                                            const xi_pArray projectionBiasArray,
                                                            xi_pArray projectionEffectiveBiasArray,
                                                            xi_cnn_enhanced_LSTMA_params* params);

_XI_API_ XI_ERR_TYPE xiConvolvedA_MOW_reoderCoeffs(xi_tile4D *srcTile, xi_tile4D *dstTile, xi_array *biasArray, int32_t inZP, int32_t coeffZP);

_XI_API_ XI_ERR_TYPE xiConvolvedA_QM_reorderCoeffs(xi_tile4D *srcTile, xi_tile4D *dstTile, xi_array *biasArrayRef, xi_array *biasArray, int32_t inZP, int32_t coeffZP);

_XI_API_ XI_ERR_TYPE xiConvolvedA_InvQM_reorderCoeffs(xi_tile4D *srcTile, xi_tile4D *dstTile, xi_array *biasArray, int32_t inZP, int32_t coeffZP);

_XI_API_ XI_ERR_TYPE xiDilatedConvA_reorderCoeffBuffers(uint8_t *srcData, uint8_t *dstData, int32_t depth, int32_t width, int32_t height, int32_t outChannel,
                                                        int32_t srcPitch1, int32_t srcPitch2, int32_t srcPitch3,
                                                        int32_t dstPitch1, int32_t dstPitch2, int32_t dstPitch3);

_XI_API_ XI_ERR_TYPE xiDilatedConvA_reorderCoeffs(xi_tile4D *srcTile, xi_tile4D *dstTile);

_XI_API_ XI_ERR_TYPE xiDepthwiseDilatedConvA_reorderCoeffBuffers(uint8_t *srcData, uint8_t *dstData, int32_t depth, int32_t width, int32_t height,
                                                                 int32_t srcPitch1, int32_t srcPitch2, int32_t dstPitch1, int32_t dstPitch2,
                                                                 int32_t *bias, int32_t *biasReorder, int32_t inDepth, int32_t depthMultiplier);

_XI_API_ XI_ERR_TYPE xiDepthwiseDilatedConvA_reorderCoeffs(xi_tile3D *srcTile, xi_tile3D *dstTile, xi_array *biasArray, xi_array *biasArrayReorder, int32_t inDepth, int32_t depthMultiplier);

_XI_API_ XI_ERR_TYPE xiPReluQuantizedA3D_I8_SetInfo_c(const float inScale, const float outScale, const float slopeScale, xi_cnn_preluA_params * pparams);

_XI_API_ XI_ERR_TYPE xiAddA3D_I8_SetInfo_c(const int32_t left_shift,
                                           const float scale1, const float scale2, const float scaleOut,
                                           const int32_t offset1, const int32_t offset2, const int32_t offsetOut,
                                           xi_cnn_addA_params * pparams);

_XI_API_ XI_ERR_TYPE xiSubA3D_I8_SetInfo_c(const int32_t left_shift,
                                           const float scale1, const float scale2, const float scaleOut,
                                           const int32_t offset1, const int32_t offset2, const int32_t offsetOut,
                                           xi_cnn_subA_params * pparams);

_XI_API_ XI_ERR_TYPE xiROIAlignQuantizeA3D_I8_DWH_SetInfo_c(const float scaleIn,
                                                            const float scaleOut, const float spatialScaleX,
                                                            const float spatialScaleY, const int32_t zeroPtInput,
                                                            const int32_t zeroPtOutput,
                                                            const int32_t pooledHeight, const int32_t pooledWidth,
                                                            xi_cnn_roi_align_params * pparams);
_XI_API_ XI_ERR_TYPE xiEltwiseMaxA3D_I8_SetInfo_c(const int32_t left_shift,
                                                  const float scale1, const float scale2, const float scaleOut,
                                                  const int32_t offset1, const int32_t offset2, const int32_t offsetOut,
                                                  xi_cnn_eltwisemaxA_params * pparams);

_XI_API_ XI_ERR_TYPE xiEltwiseMinA3D_I8_SetInfo_c(const int32_t left_shift,
                                                  const float scale1,
                                                  const float scale2,
                                                  const float scaleOut,
                                                  const int32_t offset1,
                                                  const int32_t offset2,
                                                  const int32_t offsetOut,
                                                  xi_cnn_eltwiseminA_params * pparams);

_XI_API_ XI_ERR_TYPE xiEltwiseCompareA3D_I8_SetInfo_c(const float scale1,
                                                      const float scale2,
                                                      const int32_t offset1,
                                                      const int32_t offset2,
                                                      xi_cnn_eltwiseCompareAParams * pparams);

_XI_API_ XI_ERR_TYPE xiMaxpoolQuantizeA3D_I8_SetInfo_c(const int32_t left_shift, const float scaleIn, const float scaleOut,
                                                       const int32_t zeroPtInput, const int32_t zeroPtOutput,
                                                       xi_cnn_maxpoolA_params * pparams);

_XI_API_ XI_ERR_TYPE xiMaxPoolA3D_MxN_U8_DWH(const xi_pTile3D inTile, xi_pTile3D outTile, const xi_cnn_maxpoolA_params *param);

_XI_API_ XI_ERR_TYPE xiMaxPoolA3D_MxN_S8_DWH(const xi_pTile3D inTile, xi_pTile3D outTile, const xi_cnn_maxpoolA_params *param);

_XI_API_ XI_ERR_TYPE xiAvgpoolQuantizeA3D_I8_SetInfo_c(const int32_t left_shift,
                                                       const float scaleIn, const float scaleOut,
                                                       const int32_t zeroPtInput, const int32_t zeroPtOutput,
                                                       xi_cnn_avgpoolA_params * pparams);

_XI_API_ XI_ERR_TYPE xiL2Pool2DQuantizeA3D_I8_SetInfo_c(const int32_t left_shift, const float scaleIn, const float scaleOut,
                                                        xi_cnn_l2pool2D_params * pparams);

_XI_API_ XI_ERR_TYPE xiMeanQuantizeA3D_I8_SetInfo_c(const float scaleIn, const float scaleOut,
                                                        xi_cnn_meanA_params * pparams);

_XI_API_ XI_ERR_TYPE xiAvgPoolQuantizeA_U8_DWH(const xi_pTile3D inTile,
                                               xi_pTile3D outTile,
                                               const xi_cnn_avgpoolA_params *param,
                                               const xi_size3D frame3DSize);

_XI_API_ XI_ERR_TYPE xiAvgPoolQuantizeA_S8_DWH(const xi_pTile3D inTile,
                                               xi_pTile3D outTile,
                                               const xi_cnn_avgpoolA_params *param,
                                               const xi_size3D frame3DSize);

_XI_API_ XI_ERR_TYPE xiMulA3D_I8_SetInfo_c(const float scale1, const float scale2, const float scaleOut,
                                           const int32_t offset1, const int32_t offset2, const int32_t offsetOut,
                                           xi_cnn_mulA_params * pparams);

_XI_API_ XI_ERR_TYPE xiEnhancedLSTMA_SetInfo_c(const float forget_inputWeightScale,  // forget gate scales
                                               const float forget_recurrentWeightScale,
                                               const float forget_cellWeightScale,
                                               const float forget_intermediateScale,
                                               const float forget_layerNormScale,
                                               const float mod_inputWeightScale,  // modulation gate scales
                                               const float mod_recurrentWeightScale,
                                               const float mod_intermediateScale,
                                               const float mod_layerNormScale,
                                               const float input_inputWeightScale,  // input gate scales
                                               const float input_recurrentWeightScale,
                                               const float input_cellWeightScale,
                                               const float input_intermediateScale,
                                               const float input_layerNormScale,
                                               const float output_inputWeightScale,  // output gate scales
                                               const float output_recurrentWeightScale,
                                               const float output_cellWeightScale,
                                               const float output_intermediateScale,
                                               const float output_layerNormScale,
                                               const int32_t input_zeroPoint,  // input quantization parameters
                                               const float input_scale,
                                               const int32_t prev_out_zeroPoint,  // previous output quantization parameters
                                               const float prev_out_scale,
                                               const int32_t hidden_zeroPoint, // hidden layer quantization parameters
                                               const float hidden_scale,
                                               const float cellClipVal,  // cell clip params
                                               const float cellClipScale,
                                               const uint8_t useCifg,  // coupled input forget gate param
                                               const uint8_t usePeephole, // should these be different params for different gates???
                                               const uint8_t useLayerNorm,
                                               xi_cnn_enhanced_LSTMA_params * pparams);

_XI_API_ XI_ERR_TYPE xiProjectionEnhancedLSTMA_SetInfo_c(const float projectionClipVal, // projection clip params
                                                         const float projectionClipScale,
                                                         const int32_t hidden_zeroPoint,  // hidden layer quantization parameters
                                                         const float hidden_scale,
                                                         const int32_t prev_out_zeroPoint,  // previous output quantization parameters
                                                         const float prev_out_scale,
                                                         const uint8_t useProjection,
                                                         xi_cnn_enhanced_LSTMA_params * pparams);

_XI_API_ XI_ERR_TYPE xiReQuantizeA3D_I8_SetInfo_c(const int32_t left_shift, const float scaleIn, const float scaleOut, const int32_t offsetIn,
                                                  const int32_t offsetOut, xi_cnn_reQuantizeA_params * pparams);

_XI_API_ XI_ERR_TYPE xiHeatmapMaxKeypointA3D_I8_SetInfo_c(const float heatMapScaleIn, const float outScoreScale,
                                                          const int32_t heatmapZeroPtInput, const int32_t outScoreZeroPt,
                                                          const int32_t heatmapSize,
                                                          xi_cnn_heatmap_max_keypointA3D_params *pparams);

_XI_API_ XI_ERR_TYPE xiBoxWithNMSLimitA3D_U8_SetInfo_c(const float scoreScale, const float scoreThreshold,
                                                       const float nmsThreshold, const float iouThreshold,
                                                       const float sigma, xi_cnn_box_with_NMS_limitA3D_params *pparams);

_XI_API_ XI_ERR_TYPE xiBoxWithNMSLimitA3D_S8_SetInfo_c(const float scoreScale, const float scoreThreshold,
                                                       const float nmsThreshold, const float iouThreshold,
                                                       const float sigma, xi_cnn_box_with_NMS_limitA3D_params *pparams);

_XI_API_ XI_ERR_TYPE xiHeatmapMaxKeypointA3D_U8(const xi_pTile4D heatmapTile,
                                                const xi_pArray inBBBoxArray,
                                                xi_pArray outScoreArray,
                                                xi_pTile3D outKeypointTile,
                                                const xi_cnn_heatmap_max_keypointA3D_params * pparams);

_XI_API_ XI_ERR_TYPE xiHeatmapMaxKeypointA3D_S8(const xi_pTile4D heatmapTile,
                                                const xi_pArray inBBBoxArray,
                                                xi_pArray outScoreArray,
                                                xi_pTile3D outKeypointTile,
                                                const xi_cnn_heatmap_max_keypointA3D_params * pparams);


_XI_API_ XI_ERR_TYPE xiHashtableLookupA3D_IX(const xi_pArray lookupArray, const xi_pArray keysArray, const xi_pTile3D inTile, xi_pTile3D outTile, xi_pArray outHitsArray);

_XI_API_ XI_ERR_TYPE xiAddA3D_U8(const xi_pTile3D inTile1, const xi_pTile3D inTile2, xi_pTile3D outTile, const xi_cnn_addA_params *pparams);

_XI_API_ XI_ERR_TYPE xiAddA3D_S8(const xi_pTile3D inTile1, const xi_pTile3D inTile2, xi_pTile3D outTile, const xi_cnn_addA_params *pparams);

_XI_API_ XI_ERR_TYPE xiReQuantizeA3D_U8(const xi_pTile3D inTile, xi_pTile3D outTile, const xi_cnn_reQuantizeA_params *pparams);

_XI_API_ XI_ERR_TYPE xiReQuantizeA3D_S8(const xi_pTile3D inTile, xi_pTile3D outTile, const xi_cnn_reQuantizeA_params *pparams);

_XI_API_ XI_ERR_TYPE xiReQuantizeA4D_U8(const xi_pTile4D inTile, xi_pTile4D outTile, const xi_cnn_reQuantizeA_params *pparams);

_XI_API_ XI_ERR_TYPE xiReQuantizeA4D_S8(const xi_pTile4D inTile, xi_pTile4D outTile, const xi_cnn_reQuantizeA_params *pparams);

_XI_API_ XI_ERR_TYPE xiSubA3D_U8(const xi_pTile3D inTile1, const xi_pTile3D inTile2, xi_pTile3D outTile, const xi_cnn_subA_params *pparams);

_XI_API_ XI_ERR_TYPE xiSubA3D_S8(const xi_pTile3D inTile1, const xi_pTile3D inTile2, xi_pTile3D outTile, const xi_cnn_subA_params *pparams);

_XI_API_ XI_ERR_TYPE xiBroadcastAddA3D_U8(const xi_pTile3D inTile1, const xi_pTile3D inTile2, xi_pTile3D outTile, const xi_cnn_addA_params *pparams);

_XI_API_ XI_ERR_TYPE xiBroadcastAddA3D_S8(const xi_pTile3D inTile1, const xi_pTile3D inTile2, xi_pTile3D outTile, const xi_cnn_addA_params *pparams);

_XI_API_ XI_ERR_TYPE xiBroadcastSubA3D_U8(const xi_pTile3D inTile1, const xi_pTile3D inTile2, xi_pTile3D outTile, const xi_cnn_subA_params *pparams);

_XI_API_ XI_ERR_TYPE xiBroadcastSubA3D_S8(const xi_pTile3D inTile1, const xi_pTile3D inTile2, xi_pTile3D outTile, const xi_cnn_subA_params *pparams);

_XI_API_ XI_ERR_TYPE xiMulA3D_U8(const xi_pTile3D inTile1, const xi_pTile3D inTile2, xi_pTile3D outTile, const xi_cnn_mulA_params *pparams);

_XI_API_ XI_ERR_TYPE xiMulA3D_S8(const xi_pTile3D inTile1, const xi_pTile3D inTile2, xi_pTile3D outTile, const xi_cnn_mulA_params *pparams);

_XI_API_ XI_ERR_TYPE xiBroadcastMulA3D_U8(const xi_pTile3D inTile1, const xi_pTile3D inTile2, xi_pTile3D outTile, const xi_cnn_mulA_params *pparams);

_XI_API_ XI_ERR_TYPE xiBroadcastMulA3D_S8(const xi_pTile3D inTile1, const xi_pTile3D inTile2, xi_pTile3D outTile, const xi_cnn_mulA_params *pparams);

_XI_API_ XI_ERR_TYPE xiEltwiseMaxA3D_U8(const xi_pTile3D inTile1, const xi_pTile3D inTile2, xi_pTile3D outTile, xi_pTile3D outmaskTile, const xi_cnn_eltwisemaxA_params *pparams);

_XI_API_ XI_ERR_TYPE xiEltwiseMaxA3D_S8(const xi_pTile3D inTile1, const xi_pTile3D inTile2, xi_pTile3D outTile, xi_pTile3D outmaskTile, const xi_cnn_eltwisemaxA_params *pparams);

_XI_API_ XI_ERR_TYPE xiBroadcastEltwiseMaxA3D_S8(const xi_pTile3D inTile1, const xi_pTile3D inTile2, xi_pTile3D outTile, xi_pTile3D outmaskTile, const xi_cnn_eltwisemaxA_params *pparams);

_XI_API_ XI_ERR_TYPE xiEltwiseGreaterA3D_U8(const xi_pTile3D inTile1,
                                            const xi_pTile3D inTile2,
                                            xi_pTile3D outBool,
                                            const xi_cnn_eltwiseCompareAParams *pparams);

_XI_API_ XI_ERR_TYPE xiEltwiseGreaterOrEqualA3D_U8(const xi_pTile3D inTile1,
                                                   const xi_pTile3D inTile2,
                                                   xi_pTile3D outBool,
                                                   const xi_cnn_eltwiseCompareAParams *pparams);

_XI_API_ XI_ERR_TYPE xiEltwiseLesserA3D_U8(const xi_pTile3D inTile1,
                                           const xi_pTile3D inTile2,
                                           xi_pTile3D outBool,
                                           const xi_cnn_eltwiseCompareAParams *pparams);

_XI_API_ XI_ERR_TYPE xiEltwiseLesserOrEqualA3D_U8(const xi_pTile3D inTile1,
                                                  const xi_pTile3D inTile2,
                                                  xi_pTile3D outBool,
                                                  const xi_cnn_eltwiseCompareAParams *pparams);

_XI_API_ XI_ERR_TYPE xiBroadcastEltwiseGreaterA3D_U8(const xi_pTile3D inTile1,
                                                     const xi_pTile3D inTile2,
                                                     xi_pTile3D outBool,
                                                     const xi_cnn_eltwiseCompareAParams *pparams);

_XI_API_ XI_ERR_TYPE xiBroadcastEltwiseGreaterOrEqualA3D_U8(const xi_pTile3D inTile1,
                                                     const xi_pTile3D inTile2,
                                                     xi_pTile3D outBool,
                                                     const xi_cnn_eltwiseCompareAParams *pparams);

_XI_API_ XI_ERR_TYPE xiBroadcastEltwiseLesserA3D_U8(const xi_pTile3D inTile1,
                                                     const xi_pTile3D inTile2,
                                                     xi_pTile3D outBool,
                                                     const xi_cnn_eltwiseCompareAParams *pparams);

_XI_API_ XI_ERR_TYPE xiBroadcastEltwiseLesserOrEqualA3D_U8(const xi_pTile3D inTile1,
                                                     const xi_pTile3D inTile2,
                                                     xi_pTile3D outBool,
                                                     const xi_cnn_eltwiseCompareAParams *pparams);

_XI_API_ XI_ERR_TYPE xiEltwiseGreaterA3D_S8(const xi_pTile3D inTile1,
                                            const xi_pTile3D inTile2,
                                            xi_pTile3D outBool,
                                            const xi_cnn_eltwiseCompareAParams *pparams);

_XI_API_ XI_ERR_TYPE xiEltwiseGreaterOrEqualA3D_S8(const xi_pTile3D inTile1,
                                                   const xi_pTile3D inTile2,
                                                   xi_pTile3D outBool,
                                                   const xi_cnn_eltwiseCompareAParams *pparams);

_XI_API_ XI_ERR_TYPE xiEltwiseLesserA3D_S8(const xi_pTile3D inTile1,
                                           const xi_pTile3D inTile2,
                                           xi_pTile3D outBool,
                                           const xi_cnn_eltwiseCompareAParams *pparams);

_XI_API_ XI_ERR_TYPE xiEltwiseEqualA3D_U8(const xi_pTile3D inTile1,
                                          const xi_pTile3D inTile2,
                                          xi_pTile3D outBool,
                                          const xi_cnn_eltwiseCompareAParams *pparams);

_XI_API_ XI_ERR_TYPE xiEltwiseEqualA3D_S8(const xi_pTile3D inTile1,
                                          const xi_pTile3D inTile2,
                                          xi_pTile3D outBool,
                                          const xi_cnn_eltwiseCompareAParams *pparams);

_XI_API_ XI_ERR_TYPE xiEltwiseNotEqualA3D_U8(const xi_pTile3D inTile1,
                                             const xi_pTile3D inTile2,
                                             xi_pTile3D outBool,
                                             const xi_cnn_eltwiseCompareAParams *pparams);

_XI_API_ XI_ERR_TYPE xiEltwiseNotEqualA3D_S8(const xi_pTile3D inTile1,
                                             const xi_pTile3D inTile2,
                                             xi_pTile3D outBool,
                                             const xi_cnn_eltwiseCompareAParams *pparams);

_XI_API_ XI_ERR_TYPE xiEltwiseLesserOrEqualA3D_S8(const xi_pTile3D inTile1,
                                                  const xi_pTile3D inTile2,
                                                  xi_pTile3D outBool,
                                                  const xi_cnn_eltwiseCompareAParams *pparams);

_XI_API_ XI_ERR_TYPE xiBroadcastEltwiseGreaterA3D_S8(const xi_pTile3D inTile1,
                                                     const xi_pTile3D inTile2,
                                                     xi_pTile3D outBool,
                                                     const xi_cnn_eltwiseCompareAParams *pparams);

_XI_API_ XI_ERR_TYPE xiBroadcastEltwiseGreaterOrEqualA3D_S8(const xi_pTile3D inTile1,
                                                            const xi_pTile3D inTile2,
                                                            xi_pTile3D outBool,
                                                            const xi_cnn_eltwiseCompareAParams *pparams);

_XI_API_ XI_ERR_TYPE xiBroadcastEltwiseLesserA3D_S8(const xi_pTile3D inTile1,
                                                    const xi_pTile3D inTile2,
                                                    xi_pTile3D outBool,
                                                    const xi_cnn_eltwiseCompareAParams *pparams);

_XI_API_ XI_ERR_TYPE xiBroadcastEltwiseLesserOrEqualA3D_S8(const xi_pTile3D inTile1,
                                                           const xi_pTile3D inTile2,
                                                           xi_pTile3D outBool,
                                                           const xi_cnn_eltwiseCompareAParams *pparams);

_XI_API_ XI_ERR_TYPE xiBroadcastEltwiseEqualA3D_U8(const xi_pTile3D inTile1,
                                                   const xi_pTile3D inTile2,
                                                   xi_pTile3D outBool,
                                                   const xi_cnn_eltwiseCompareAParams *pparams);

_XI_API_ XI_ERR_TYPE xiBroadcastEltwiseEqualA3D_S8(const xi_pTile3D inTile1,
                                                   const xi_pTile3D inTile2,
                                                   xi_pTile3D outBool,
                                                   const xi_cnn_eltwiseCompareAParams *pparams);

_XI_API_ XI_ERR_TYPE xiBroadcastEltwiseNotEqualA3D_U8(const xi_pTile3D inTile1,
                                                      const xi_pTile3D inTile2,
                                                      xi_pTile3D outBool,
                                                      const xi_cnn_eltwiseCompareAParams *pparams);

_XI_API_ XI_ERR_TYPE xiBroadcastEltwiseNotEqualA3D_S8(const xi_pTile3D inTile1,
                                                      const xi_pTile3D inTile2,
                                                      xi_pTile3D outBool,
                                                      const xi_cnn_eltwiseCompareAParams *pparams);

_XI_API_ XI_ERR_TYPE xiEltwiseGreaterA3D_S32(const xi_pTile3D inTile1,
                                             const xi_pTile3D inTile2,
                                             xi_pTile3D outBool);

_XI_API_ XI_ERR_TYPE xiEltwiseGreaterOrEqualA3D_S32(const xi_pTile3D inTile1,
                                                    const xi_pTile3D inTile2,
                                                    xi_pTile3D outBool);

_XI_API_ XI_ERR_TYPE xiEltwiseLesserA3D_S32(const xi_pTile3D inTile1,
                                            const xi_pTile3D inTile2,
                                            xi_pTile3D outBool);

_XI_API_ XI_ERR_TYPE xiEltwiseLesserOrEqualA3D_S32(const xi_pTile3D inTile1,
                                                   const xi_pTile3D inTile2,
                                                   xi_pTile3D outBool);

_XI_API_ XI_ERR_TYPE xiEltwiseEqualA3D_S32(const xi_pTile3D inTile1,
                                           const xi_pTile3D inTile2,
                                           xi_pTile3D outBool);

_XI_API_ XI_ERR_TYPE xiEltwiseNotEqualA3D_S32(const xi_pTile3D inTile1,
                                             const xi_pTile3D inTile2,
                                             xi_pTile3D outBool);

_XI_API_ XI_ERR_TYPE xiBroadcastEltwiseGreaterA3D_S32(const xi_pTile3D inTile1,
                                                      const xi_pTile3D inTile2,
                                                      xi_pTile3D outBool);

_XI_API_ XI_ERR_TYPE xiBroadcastEltwiseGreaterOrEqualA3D_S32(const xi_pTile3D inTile1,
                                                             const xi_pTile3D inTile2,
                                                             xi_pTile3D outBool);

_XI_API_ XI_ERR_TYPE xiBroadcastEltwiseLesserA3D_S32(const xi_pTile3D inTile1,
                                                     const xi_pTile3D inTile2,
                                                     xi_pTile3D outBool);

_XI_API_ XI_ERR_TYPE xiBroadcastEltwiseLesserOrEqualA3D_S32(const xi_pTile3D inTile1,
                                                            const xi_pTile3D inTile2,
                                                            xi_pTile3D outBool);

_XI_API_ XI_ERR_TYPE xiBroadcastEltwiseEqualA3D_S32(const xi_pTile3D inTile1,
                                                    const xi_pTile3D inTile2,
                                                    xi_pTile3D outBool);

_XI_API_ XI_ERR_TYPE xiBroadcastEltwiseNotEqualA3D_S32(const xi_pTile3D inTile1,
                                                      const xi_pTile3D inTile2,
                                                      xi_pTile3D outBool);

_XI_API_ XI_ERR_TYPE xiBroadcastEltwiseMaxA3D_U8(const xi_pTile3D inTile1,
                                                 const xi_pTile3D inTile2,
                                                 xi_pTile3D outTile,
                                                 xi_pTile3D outmaskTile,
                                                 const xi_cnn_eltwisemaxA_params *pparams);

_XI_API_ XI_ERR_TYPE xiBroadcastEltwiseMinA3D_U8(const xi_pTile3D inTile1,
                                                 const xi_pTile3D inTile2,
                                                 xi_pTile3D outTile,
                                                 xi_pTile3D outmaskTile,
                                                 const xi_cnn_eltwiseminA_params *pparams);

_XI_API_ XI_ERR_TYPE xiBroadcastEltwiseMinA3D_S8(const xi_pTile3D inTile1,
                                                 const xi_pTile3D inTile2,
                                                 xi_pTile3D outTile,
                                                 xi_pTile3D outmaskTile,
                                                 const xi_cnn_eltwiseminA_params *pparams);

_XI_API_ XI_ERR_TYPE xiBroadcastLogicalAndA3D_I8(const xi_pTile3D inBoolTile1,
                                                 const xi_pTile3D inBoolTile2,
                                                 xi_pTile3D outBoolTile);

_XI_API_ XI_ERR_TYPE xiBroadcastLogicalOrA3D_I8(const xi_pTile3D inBoolTile1,
                                                const xi_pTile3D inBoolTile2,
                                                xi_pTile3D outBoolTile);

_XI_API_ XI_ERR_TYPE xiL2Pool2DA_U8_DWH(const xi_pTile3D inTile, xi_pTile3D outTile, const xi_cnn_l2pool2D_params *param);

_XI_API_ XI_ERR_TYPE xiL2Pool2DA_S8_DWH(const xi_pTile3D inTile, xi_pTile3D outTile, const xi_cnn_l2pool2D_params *param);

_XI_API_ XI_ERR_TYPE xiL2Pool2DQuantizeA_U8_DWH(const xi_pTile3D inTile, xi_pTile3D outTile, const xi_cnn_l2pool2D_params *param);

_XI_API_ XI_ERR_TYPE xiL2Pool2DQuantizeA_S8_DWH(const xi_pTile3D inTile, xi_pTile3D outTile, const xi_cnn_l2pool2D_params *param);

_XI_API_ XI_ERR_TYPE xiMaxPoolQuantizeA_U8_DWH(const xi_pTile3D inTile, xi_pTile3D outTile, const xi_cnn_maxpoolA_params *param);

_XI_API_ XI_ERR_TYPE xiMaxPoolQuantizeA_S8_DWH(const xi_pTile3D inTile, xi_pTile3D outTile, const xi_cnn_maxpoolA_params *param);

_XI_API_ XI_ERR_TYPE xiMeanA_U8(const xi_pTile3D inTile, xi_pTile3D outTile, xi_pArray accSumArray, const xi_cnn_meanA_params *param);

_XI_API_ XI_ERR_TYPE xiMeanA_S8(const xi_pTile3D inTile, xi_pTile3D outTile, xi_pArray accSumArray, const xi_cnn_meanA_params *param);

_XI_API_ XI_ERR_TYPE xiMeanQuantizeA_U8(const xi_pTile3D inTile, xi_pTile3D outTile, xi_pArray accSumArray, const xi_cnn_meanA_params *param);

_XI_API_ XI_ERR_TYPE xiMeanQuantizeA_S8(const xi_pTile3D inTile, xi_pTile3D outTile, xi_pArray accSumArray, const xi_cnn_meanA_params *param);

_XI_API_ XI_ERR_TYPE xiMean4DA_U8(const xi_pTile4D inTile, xi_pTile4D outTile, xi_pArray accSumArray, const xi_cnn_meanA_params *pparams);

_XI_API_ XI_ERR_TYPE xiMean4DA_S8(const xi_pTile4D inTile, xi_pTile4D outTile, xi_pArray accSumArray, const xi_cnn_meanA_params *pparams);

_XI_API_ XI_ERR_TYPE xiMean4DQuantizeA_U8(const xi_pTile4D inTile, xi_pTile4D outTile, xi_pArray accSumArray, const xi_cnn_meanA_params *param);

_XI_API_ XI_ERR_TYPE xiMean4DQuantizeA_S8(const xi_pTile4D inTile, xi_pTile4D outTile, xi_pArray accSumArray, const xi_cnn_meanA_params *param);

_XI_API_ XI_ERR_TYPE xiReduceProdA_U8_SetInfo_c(const int32_t left_shift,
                                                  const float scaleIn,
                                                  const float scaleOut,
                                                  const int32_t zeroPtInput,
                                                  const int32_t zeroPtOutput,
                                                  xi_cnn_reduceProdA_params * params);

_XI_API_ XI_ERR_TYPE xiReduceMaxA3D_U8(const xi_pTile3D inTile,
                                       xi_pTile3D outTile,
                                       const xi_cnn_reduceA_params *params);

_XI_API_ XI_ERR_TYPE xiReduceMaxA3D_S8(const xi_pTile3D inTile,
                                       xi_pTile3D outTile,
                                       const xi_cnn_reduceA_params *params);

_XI_API_ XI_ERR_TYPE xiReduceMaxA4D_U8(const xi_pTile4D inTile,
                                       xi_pTile4D outTile,
                                       const xi_cnn_reduceA_params *params);

_XI_API_ XI_ERR_TYPE xiReduceMaxA4D_S8(const xi_pTile4D inTile,
                                       xi_pTile4D outTile,
                                       const xi_cnn_reduceA_params *params);

_XI_API_ XI_ERR_TYPE xiReduceMinA3D_U8(const xi_pTile3D inTile,
                                       xi_pTile3D outTile,
                                       const xi_cnn_reduceA_params *params);

_XI_API_ XI_ERR_TYPE xiReduceMinA3D_S8(const xi_pTile3D inTile,
                                       xi_pTile3D outTile,
                                       const xi_cnn_reduceA_params *params);

_XI_API_ XI_ERR_TYPE xiReduceMinA4D_U8(const xi_pTile4D inTile,
                                       xi_pTile4D outTile,
                                       const xi_cnn_reduceA_params *params);

_XI_API_ XI_ERR_TYPE xiReduceMinA4D_S8(const xi_pTile4D inTile,
                                       xi_pTile4D outTile,
                                       const xi_cnn_reduceA_params *params);

_XI_API_ XI_ERR_TYPE xiReduceAnyA3D_U8(const xi_pTile3D inTile,
                                       xi_pTile3D outTile,
                                       const xi_cnn_reduceA_params *params);

_XI_API_ XI_ERR_TYPE xiReduceAnyA3D_S8(const xi_pTile3D inTile,
                                       xi_pTile3D outTile,
                                       const xi_cnn_reduceA_params *params);

_XI_API_ XI_ERR_TYPE xiReduceAnyA4D_U8(const xi_pTile4D inTile,
                                       xi_pTile4D outTile,
                                       const xi_cnn_reduceA_params *params);

_XI_API_ XI_ERR_TYPE xiReduceAnyA4D_S8(const xi_pTile4D inTile,
                                       xi_pTile4D outTile,
                                       const xi_cnn_reduceA_params *params);

_XI_API_ XI_ERR_TYPE xiStridedSliceA_U8(const xi_pTile3D inTile, xi_pTile3D outTile, const xi_cnn_stridedsliceA_params *pparams);

_XI_API_ XI_ERR_TYPE xiStridedSliceA_S8(const xi_pTile3D inTile, xi_pTile3D outTile, const xi_cnn_stridedsliceA_params *pparams);

_XI_API_ XI_ERR_TYPE xiLSTMA_U8(const xi_pTile4D inTile, const xi_pTile4D outActiveTile, const xi_pTile4D prevActiveTile,                                    \
        const xi_pTile4D weightsTile, const xi_pArray biasArray, const xi_pTile4D prevStateInTile, const xi_pTile4D outputStateTile, \
        xi_pTile4D concatTile, xi_pTile4D scratchTile, const xi_cnna_lstmA_params *param);

_XI_API_ XI_ERR_TYPE xiLSTMA_S8(const xi_pTile4D inTile, const xi_pTile4D outActiveTile, const xi_pTile4D prevActiveTile,                                    \
        const xi_pTile4D weightsTile, const xi_pArray biasArray, const xi_pTile4D prevStateInTile, const xi_pTile4D outputStateTile, \
        xi_pTile4D concatTile, xi_pTile4D scratchTile, const xi_cnna_lstmA_params *param);


_XI_API_ XI_ERR_TYPE xiLSTMA(const xi_pTile4D inTile, const xi_pTile4D outActiveTile, const xi_pTile4D prevActiveTile,                                    \
        const xi_pTile4D weightsTile, const xi_pArray biasArray, const xi_pTile4D prevStateInTile, const xi_pTile4D outputStateTile, \
        xi_pTile4D concatTile, xi_pTile4D scratchTile, const xi_cnna_lstmA_params *param);

_XI_API_ XI_ERR_TYPE xiPermuteA3D_I8(const xi_pTile3D inTile, xi_pTile3D outTile, const xi_cnn_permuteA3D_params * pparams);

_XI_API_ XI_ERR_TYPE xiPermuteA4D_I8(const xi_pTile4D inTile, xi_pTile4D outTile, const xi_cnn_permuteA4D_params * pparams);

_XI_API_ XI_ERR_TYPE xiCropA3D_I8(const xi_pTile3D inTile, xi_pTile3D outTile, const uint32_t start0, const uint32_t start1, const uint32_t start2);

_XI_API_ XI_ERR_TYPE xiConvolvedFixupA3D_QM32_MxN_U8S32(const xi_pTile3D inTile,
                                                        const xi_pTile4D coeffTile, const xi_pTile3D outTile,
                                                        xi_pTile fixUpTile, const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedA3D_QM24_MxN_U8S8U8(const xi_pTile3D inTile,
                                                    const xi_pTile fixUpTile, const xi_pTile4D coeffTile,
                                                    const xi_pArray biasArray, xi_pTile3D outTile,
                                                    const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedAVQ3D_InvQM24_3x3_S8_DWH(const xi_pTile3D inTile,
                                                         xi_pTile reorderTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         const xi_pArray outScaleArray,
                                                         const xi_pArray outShiftArray,
                                                         xi_pTile3D outTile,
                                                         const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedFixupA3D_InvQM24_3x3_U8S32_DWH(const xi_pTile3D inTile, const xi_pTile4D coeffTile,
                                                               const xi_pTile3D outTile, xi_pTile fixupTile,
                                                               const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedA3D_QM32_MxN_U8S8U8(const xi_pTile3D inTile,
                                                    const xi_pTile fixUpTile, const xi_pTile4D coeffTile,
                                                    const xi_pArray biasArray, xi_pTile3D outTile,
                                                    const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedA3D_QM24_U8U8U8(xi_tile3D *src_t, xi_tile *fixupTile, xi_tile4D *coeffTile,
                                                xi_array *biasArray, xi_tile3D *dst_t, xi_cnna_conv_params *convParams);

_XI_API_ XI_ERR_TYPE xiConvolvedAVQ3D(const xi_pTile3D inTile,
                                      const xi_pTile fixUpTile,
                                      const xi_pTile4D coeffTile,
                                      const xi_pArray biasArray,
                                      const xi_pArray outScaleArray,
                                      const xi_pArray outShiftArray,
                                      xi_pTile3D outTile,
                                      const xi_cnna_conv_params *param);

_XI_API_ void *xiGetConvolvedAVQ3DVariant(const xi_pTile3D inTile,
                                          const xi_pTile fixUpTile,
                                          const xi_pTile4D coeffTile,
                                          const xi_pArray biasArray,
                                          const xi_pArray outScaleArray,
                                          const xi_pArray outShiftArray,
                                          xi_pTile3D outTile,
                                          const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedAVQ3D_QM24_S8_DWH(const xi_pTile3D inTile,
                                                  const xi_pTile4D coeffTile,
                                                  const xi_pArray biasArray,
                                                  const xi_pArray outScaleArray,
                                                  const xi_pArray outShiftArray,
                                                  xi_pTile3D outTile,
                                                  const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedAVQ3D_QM32_S8_DWH(const xi_pTile3D inTile,
                                                  const xi_pTile4D coeffTile,
                                                  const xi_pArray biasArray,
                                                  const xi_pArray outScaleArray,
                                                  const xi_pArray outShiftArray,
                                                  xi_pTile3D outTile,
                                                  const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedAVQ3D_S_MxN_S8_SO_DWH(const xi_pTile3D inTile,
                                                      const xi_pTile4D coeffTile,
                                                      const xi_pArray biasArray,
                                                      const xi_pArray outScaleArray,
                                                      const xi_pArray outShiftArray,
                                                      xi_pTile3D outTile,
                                                      const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedA3D_InvQM24_3x3_U8S8U8(xi_tile3D *src_t, xi_tile *fixupTile, xi_tile4D *coeffTile,
                                                       xi_array *biasArray, xi_tile3D *dst_t, const xi_cnna_conv_params *convParams);

_XI_API_ XI_ERR_TYPE xiConvolvedA3D_QM32_U8U8U8(xi_tile3D *src_t, xi_tile *fixupTile, xi_tile4D *coeffTile,
                                                xi_array *biasArray, xi_tile3D *dst_t, xi_cnna_conv_params *convParams);

_XI_API_ XI_ERR_TYPE xiDilatedConvolvedA3D(const xi_pTile3D inTile, const xi_pTile4D coeffTile,
                                           const xi_pArray biasArray, xi_pTile3D outTile,
                                           xi_cnna_dilatedConv_params *param);

_XI_API_ XI_ERR_TYPE xiDilatedConvolvedAVQ3D_S8_DWH(const xi_pTile3D inTile,
                                                    const xi_pTile4D coeffTile,
                                                    const xi_pArray biasArray,
                                                    const xi_pArray scaleArray,
                                                    const xi_pArray shiftArray,
                                                    xi_pTile3D outTile,
                                                    xi_cnna_dilatedConv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseDilatedConvolvedA3D(const xi_pTile3D inTile, const xi_pTile3D coeffTile,
                                                    const xi_pArray biasArray, xi_pTile3D outTile,
                                                    const xi_cnna_depthwiseDilatedConv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseMultiplierConvolvedA3D_U8_DWH(const xi_pTile3D inTile,
                                                              const xi_pTile3D coeffTile,
                                                              const xi_pArray biasArray,
                                                              xi_pTile3D outTile,
                                                              const xi_cnna_depthwiseDilatedConv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedA3D_3x3_U8S8U8j1d1_MOW(xi_tile3D *src_t, xi_tile *fixupTile, xi_tile4D *coeffTile,
                                                       xi_array *biasArray, xi_tile3D *dst_t, xi_cnna_conv_params *convParams);

_XI_API_ XI_ERR_TYPE xiConvolvedA3D_3x3_U8S8U8j2d1_MOW_DEPTH3(xi_tile3D *src_t, xi_tile *fixupTile, xi_tile4D *coeffTile,
                                                       xi_array *biasArray, xi_tile3D *dst_t, xi_cnna_conv_params *convParams);

_XI_API_ XI_ERR_TYPE xiConvolvedA3D_7x7_U8S8U8j2d1_MOW_DEPTH3(xi_tile3D *src_t, xi_tile *fixupTile, xi_tile4D *coeffTile,
                                                              xi_array *biasArray, xi_tile3D *dst_t, xi_cnna_conv_params *convParams);

_XI_API_ XI_ERR_TYPE xiConvolvedA3D_1x1_U8S8U8j1d1_MOW(xi_tile3D *src_t, xi_tile *fixupTile, xi_tile4D *coeffTile,
                                                       xi_array *biasArray, xi_tile3D *dst_t, xi_cnna_conv_params *convParams);

_XI_API_ XI_ERR_TYPE xiConvolvedA3D_3x3_U8S8U8j1d1_MOW_DWH(const xi_pTile3D inTile, xi_pTile fixupTile,
                                                           const xi_pTile4D coeffTile, const xi_pArray biasArray,
                                                           xi_pTile3D outTile, const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedA3D_3x3_U8S8U8j2d1_MOW_DWH_DEPTH3(const xi_pTile3D inTile, xi_pTile fixupTile,
                                                           const xi_pTile4D coeffTile, const xi_pArray biasArray,
                                                           xi_pTile3D outTile, const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedA3D_7x7_U8S8U8j2d1_MOW_DWH_DEPTH3(const xi_pTile3D inTile, xi_pTile fixupTile,
                                                                  const xi_pTile4D coeffTile, const xi_pArray biasArray,
                                                                  xi_pTile3D outTile, const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedA3D_1x1_U8S8U8j1d1_MOW_DWH(const xi_pTile3D inTile, xi_pTile fixupTile,
                                                           const xi_pTile4D coeffTile, const xi_pArray biasArray,
                                                           xi_pTile3D outTile, const xi_cnna_conv_params *param);


_XI_API_ XI_ERR_TYPE xiConvolvedAVQ3D_1x1_S8j1d1_MOW_DWH(const xi_pTile3D inTile, xi_pTile transposeTile,
                                                             const xi_pTile4D coeffTile, const xi_pArray biasArray,
                                                             const xi_pArray outputScaleArray, const xi_pArray outputShiftArray,
                                                             xi_pTile3D outTile, const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedAVQ3D_3x3_S8j1d1_MOW_DWH(const xi_pTile3D inTile, xi_pTile transposeTile,
                                                             const xi_pTile4D coeffTile, const xi_pArray biasArray,
                                                             const xi_pArray outputScaleArray, const xi_pArray outputShiftArray,
                                                             xi_pTile3D outTile, const xi_cnna_conv_params *param);


_XI_API_ XI_ERR_TYPE xiConvolvedAVQ3D_3x3_S8j2d1_MOW_DWH_DEPTH3(const xi_pTile3D inTile,
                                                                xi_pTile transposeTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                const xi_pArray outputScaleArray,
                                                                const xi_pArray outputShiftArray,
                                                                xi_pTile3D outTile,
                                                                const xi_cnna_conv_params *param);


_XI_API_ XI_ERR_TYPE xiConvolvedAVQ3D_7x7_S8j2d1_MOW_DWH_DEPTH3(const xi_pTile3D inTile,
                                                                xi_pTile transposeTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                const xi_pArray outputScaleArray,
                                                                const xi_pArray outputShiftArray,
                                                                xi_pTile3D outTile,
                                                                const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedFixupA3D_3x3_U8S32_DWH_WHD(const xi_pTile3D inTile, const xi_pTile4D coeffTile,
                                                           const xi_pTile3D outTile, xi_pTile fixupTile, const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedFixupA3D_3x3_U8S32j2_DWH_WHD_DEPTH3(const xi_pTile3D inTile, const xi_pTile4D coeffTile,
                                                           const xi_pTile3D outTile, xi_pTile fixupTile, const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedFixupA3D_7x7_U8S32j2_DWH_WHD_DEPTH3(const xi_pTile3D inTile,
                                                                    const xi_pTile4D coeffTile,
                                                                    const xi_pTile3D outTile,
                                                                    xi_pTile fixupTile,
                                                                    const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedFixupA3D_1x1_U8S32_DWH_WHD(const xi_pTile3D inTile, const xi_pTile4D coeffTile,
                                                           const xi_pTile3D outTile, xi_pTile fixupTile, const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolveA2D_S_3x3_U8Ca2_MOD_DWH(const xi_pTile3D inTile,
                                                                const xi_pTile3D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolveAVQ2D(const xi_pTile3D inTile,
                                              const xi_pTile3D coeffTile,
                                              const xi_pArray biasArray,
                                              const xi_pArray outScaleArray,
                                              const xi_pArray outShiftArray,
                                              xi_pTile3D outTile,
                                              const xi_cnna_conv_params *param);

_XI_API_ void *xiGetDepthwiseConvolveAVQ2DVariant(const xi_pTile3D inTile,
                                                  const xi_pTile3D coeffTile,
                                                  const xi_pArray biasArray,
                                                  const xi_pArray outScaleArray,
                                                  const xi_pArray outShiftArray,
                                                  xi_pTile3D outTile,
                                                  const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolveAVQ2D_S_3x3_S8Ca2_MOD_DWH(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  const xi_pArray outScaleArray,
                                                                  const xi_pArray outShiftArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolveA2D_S_5x5_U8Ca2_MOD_DWH(const xi_pTile3D inTile,
                                                                const xi_pTile3D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolveAVQ2D_S_5x5_S8Ca2_MOD_DWH(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  const xi_pArray outScaleArray,
                                                                  const xi_pArray outShiftArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolveA2D_S_7x7_U8Ca2_MOD_DWH(const xi_pTile3D inTile,
                                                                const xi_pTile3D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolveAVQ2D_S_7x7_S8Ca2_MOD_DWH(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  const xi_pArray outScaleArray,
                                                                  const xi_pArray outShiftArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolveA2D_S_MxN_U8Ca2_MOD_DWH(const xi_pTile3D inTile,
                                                                const xi_pTile3D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolveAVQ2D_S_MxN_S8Ca2_MOD_DWH(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  const xi_pArray outScaleArray,
                                                                  const xi_pArray outShiftArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiFullyConnectedAVQ3D_S_S8(const xi_pTile3D inTile,
                                                    const xi_pTile4D coeffTile,
                                                    const xi_pArray biasArray,
                                                    const xi_pArray outScaleArray,
                                                    const xi_pArray outShiftArray,
                                                    xi_pTile3D outTile,
                                                    const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseDilatedConvolvedAVQ3D_S8_DWH(const xi_pTile3D inTile,
                                                             const xi_pTile3D coeffTile,
                                                             const xi_pArray biasArray,
                                                             const xi_pArray scaleArray,
                                                             const xi_pArray shiftArray,
                                                             xi_pTile3D outTile,
                                                             const xi_cnna_depthwiseDilatedConv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseMultiplierConvolvedAVQ3D_S8_DWH(const xi_pTile3D inTile,
                                                                const xi_pTile3D coeffTile,
                                                                const xi_pArray biasArray,
                                                                const xi_pArray scaleArray,
                                                                const xi_pArray shiftArray,
                                                                xi_pTile3D outTile,
                                                                const xi_cnna_depthwiseDilatedConv_params *param);

_XI_API_ XI_ERR_TYPE xiFullyConnectedA3D_S_U8(const xi_pTile3D inTile,
                                              const xi_pTile4D coeffTile,
                                              const xi_pArray biasArray,
                                              xi_pTile3D outTile,
                                              const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiFullyConnectedA3D_S_S8(const xi_pTile3D inTile,
                                              const xi_pTile4D coeffTile,
                                              const xi_pArray biasArray,
                                              xi_pTile3D outTile,
                                              const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedA3D_S_MxN_U8S8U8_SO_DWH(const xi_pTile3D inTile,
                                                        const xi_pTile fixUpTile,
                                                        const xi_pTile4D coeffTile,
                                                        const xi_pArray biasArray,
                                                        xi_pTile3D outTile,
                                                        const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiSubtractA3D_const128_U8S8(const xi_pTile3D inTile,
                                                 xi_pTile3D outTile);

_XI_API_ XI_ERR_TYPE xiSubtractA3D_const128_I8(const xi_pTile3D inTile,
                                                 xi_pTile3D outTile);

_XI_API_ XI_ERR_TYPE xiEmbeddingLookupA3D_I8_Dim1(const xi_pTile3D inTile,
                                                  const xi_pArray indxArray,
                                                  xi_pTile3D outTile);

_XI_API_ XI_ERR_TYPE xiEmbeddingLookupA3D_I8_Dim2(const xi_pTile3D inTile,
                                                  const xi_pArray indxArray,
                                                  xi_pTile3D outTile);

_XI_API_ XI_ERR_TYPE xiEmbeddingLookupA3D_I8_Dim3(const xi_pTile3D inTile,
                                                  const xi_pArray indxArray,
                                                  xi_pTile3D outTile);

_XI_API_ XI_ERR_TYPE xiGatherA3D_I8(const xi_pTile3D inTile,
                                    const xi_pArray indxArray,
                                    xi_pArray outArray);

_XI_API_ XI_ERR_TYPE xiConvertIndicesA3D_WHD_DWH(const xi_pTile3D inTileWHD,
                                                 const xi_pTile3D inTileDWH,
                                                 const xi_pArray idxWHDArray,
                                                 xi_pArray idxDWHArray);

_XI_API_ XI_ERR_TYPE xiGatherConvertIndicesA3D(const xi_pTile3D inTile,
                                                const xi_pArray inIndxArray,
                                                xi_pArray outIndxArray,
                                                const xi_cnn_GatherA_params *params);

_XI_API_ XI_ERR_TYPE xiCastA3D(const xi_pTile3D inTile,
                               xi_pTile3D outTile);

_XI_API_ XI_ERR_TYPE xiShuffleA3D_I8_DWH(const xi_pTile3D inTile,
                                         xi_pTile3D outTile,
                                         const xi_cnn_shuffleA_params *shuffParams);

_XI_API_ XI_ERR_TYPE xiShuffleA4D_I8(const xi_pTile4D inTile,
                                     xi_pTile4D outTile,
                                     const xi_cnn_shuffleA_params *shuffParams);

_XI_API_ XI_ERR_TYPE xiEltwiseMinA3D_U8(const xi_pTile3D inTile1,
                                        const xi_pTile3D inTile2,
                                        xi_pTile3D outTile,
                                        xi_pTile3D outmaskTile,
                                        const xi_cnn_eltwiseminA_params *pparams);

_XI_API_ XI_ERR_TYPE xiEltwiseMinA3D_S8(const xi_pTile3D inTile1,
                                        const xi_pTile3D inTile2,
                                        xi_pTile3D outTile,
                                        xi_pTile3D outmaskTile,
                                        const xi_cnn_eltwiseminA_params *pparams);

_XI_API_ XI_ERR_TYPE xiArgmaxA3D_U8U32(const xi_pTile3D inTile,
                                       xi_pTile3D outTile,
                                       const xi_cnn_ArgMinMaxA_params *params);

_XI_API_ XI_ERR_TYPE xiArgmaxA3D_S8U32(const xi_pTile3D inTile,
                                       xi_pTile3D outTile,
                                       const xi_cnn_ArgMinMaxA_params *params);

_XI_API_ XI_ERR_TYPE xiArgmaxA4D_U8U32(const xi_pTile4D inTile,
                                       xi_pTile4D outTile,
                                       const xi_cnn_ArgMinMaxA_params *params);

_XI_API_ XI_ERR_TYPE xiArgmaxA4D_S8U32(const xi_pTile4D inTile,
                                       xi_pTile4D outTile,
                                       const xi_cnn_ArgMinMaxA_params *params);

_XI_API_ XI_ERR_TYPE xiArgminA3D_U8U32(const xi_pTile3D inTile,
                                       xi_pTile3D outTile,
                                       const xi_cnn_ArgMinMaxA_params *params);

_XI_API_ XI_ERR_TYPE xiArgminA3D_S8U32(const xi_pTile3D inTile,
                                       xi_pTile3D outTile,
                                       const xi_cnn_ArgMinMaxA_params *params);

_XI_API_ XI_ERR_TYPE xiArgminA4D_U8U32(const xi_pTile4D inTile,
                                       xi_pTile4D outTile,
                                       const xi_cnn_ArgMinMaxA_params *params);

_XI_API_ XI_ERR_TYPE xiArgminA4D_S8U32(const xi_pTile4D inTile,
                                       xi_pTile4D outTile,
                                       const xi_cnn_ArgMinMaxA_params *params);

_XI_API_ XI_ERR_TYPE xiLogicalAndA3D_I8(const xi_pTile3D inBoolTile1,
                                        const xi_pTile3D inBoolTile2,
                                        xi_pTile3D outBoolTile);

_XI_API_ XI_ERR_TYPE xiLogicalOrA3D_I8(const xi_pTile3D inBoolTile1,
                                       const xi_pTile3D inBoolTile2,
                                       xi_pTile3D outBoolTile);

_XI_API_ XI_ERR_TYPE xiLogicalNotA3D_I8(const xi_pTile3D inBoolTile,
                                        xi_pTile3D outBoolTile);

_XI_API_ XI_ERR_TYPE xiMinpoolQuantizeA3D_I8_SetInfo_c(const int32_t left_shift,
                                                       const float scaleIn,
                                                       const float scaleOut,
                                                       const int32_t zeroPtInput,
                                                       const int32_t zeroPtOutput,
                                                       xi_cnn_minpoolA_params * pparams);

_XI_API_ XI_ERR_TYPE xiMinPoolQuantizeA_U8_DWH(const xi_pTile3D inTile,
                                               xi_pTile3D outTile,
                                               const xi_cnn_minpoolA_params *param);

_XI_API_ XI_ERR_TYPE xiMinPoolQuantizeA_S8_DWH(const xi_pTile3D inTile,
                                               xi_pTile3D outTile,
                                               const xi_cnn_minpoolA_params *param);

_XI_API_ XI_ERR_TYPE xiAbsA3D_I8_SetInfo_c(const int32_t left_shift,
                                           const float scaleIn,
                                           const float scaleOut,
                                           const int32_t zeroPtInput,
                                           const int32_t zeroPtOutput,
                                           xi_cnn_AbsA_params * pparams);

_XI_API_ XI_ERR_TYPE xiAbsA3D_U8(const xi_pTile3D inTile,
                                 xi_pTile3D outTile,
                                 const xi_cnn_AbsA_params *params);

_XI_API_ XI_ERR_TYPE xiAbsA3D_S8(const xi_pTile3D inTile,
                                 xi_pTile3D outTile,
                                 const xi_cnn_AbsA_params *params);


_XI_API_ XI_ERR_TYPE xiTopKV2A3D_U8_DWH(const xi_pTile3D inTile,
                                        xi_pTile3D outTileIdx,
                                        xi_pTile3D outTileVal,
                                        xi_pTile extraValCnt,
                                        xi_pArray sortedIdxArr,
                                        xi_pArray sortedValArr,
                                        const uint16_t numLargestVal);

_XI_API_ XI_ERR_TYPE xiTopKV2A3D_S8_DWH(const xi_pTile3D inTile,
                                        xi_pTile3D outTileIdx,
                                        xi_pTile3D outTileVal,
                                        xi_pTile extraValCnt,
                                        xi_pArray sortedIdxArr,
                                        xi_pArray sortedValArr,
                                        const uint16_t numLargestVal);

_XI_API_ XI_ERR_TYPE xiSelectA3D_I8_SetInfo_c(const int32_t left_shift,
                                              const float scale1,
                                              const float scale2,
                                              const float scaleOut,
                                              const int32_t offset1,
                                              const int32_t offset2,
                                              const int32_t offsetOut,
                                              xi_cnn_selectA_params * pparams);

_XI_API_ XI_ERR_TYPE xiSelectA3D_U8(const xi_pTile3D inTile1,
                                    const xi_pTile3D inTile2,
                                    const xi_pTile3D boolMask,
                                    xi_pTile3D outTile,
                                    const xi_cnn_selectA_params * pparams);

_XI_API_ XI_ERR_TYPE xiSelectA3D_S8(const xi_pTile3D inTile1,
                                    const xi_pTile3D inTile2,
                                    const xi_pTile3D boolMask,
                                    xi_pTile3D outTile,
                                    const xi_cnn_selectA_params * pparams);

// Generates 256 entry lookup table bBoxArray with 32 bit entries, for use by bBoxTransform function using bBox parameters pparams
_XI_API_ XI_ERR_TYPE xiExp_generateLut_U8_c(xi_pArray bBoxTransformLUTArray,
                                      const xi_cnn_axis_aligned_bbox_transformA3D_params * pparams);

_XI_API_ XI_ERR_TYPE xiExp_generateLut_S8_c(xi_pArray bBoxTransformLUTArray,
                                      const xi_cnn_axis_aligned_bbox_transformA3D_params * pparams);

_XI_API_ XI_ERR_TYPE xiExp_generateLut_U8(xi_pArray bBoxTransformLUTArray,
                                      const xi_cnn_axis_aligned_bbox_transformA3D_params * pparams);

_XI_API_ XI_ERR_TYPE xiExp_generateLut_S8(xi_pArray bBoxTransformLUTArray,
                                      const xi_cnn_axis_aligned_bbox_transformA3D_params * pparams);

_XI_API_ XI_ERR_TYPE xiAxisAlignedBboxTransformA3D_I8_DWH_SetInfo_c(const double bboxDeltaScale,
                                                                    const int16_t bboxDeltaZeroPt,
                                                                    const uint16_t imageWidth,
                                                                    const uint16_t imageHeight,
                                                                    xi_cnn_axis_aligned_bbox_transformA3D_params* params);

_XI_API_ XI_ERR_TYPE xiAxisAlignedBboxTransformA3D_U8(const xi_pArray RoIParam,
                                                      const xi_pTile3D BBoxDeltaTile,
                                                      const xi_pArray batchSplitData,
                                                      xi_pArray lutArray,
                                                      xi_pTile3D outBoxTile,
                                                      const xi_cnn_axis_aligned_bbox_transformA3D_params * pparams);

_XI_API_ XI_ERR_TYPE xiAxisAlignedBboxTransformA3D_U8_LUT(const xi_pArray RoIParam,
                                                          const xi_pTile3D BBoxDeltaTile,
                                                          const xi_pArray batchSplitData,
                                                          const xi_pArray lutArray,
                                                          xi_pTile3D outBoxTile,
                                                          const xi_cnn_axis_aligned_bbox_transformA3D_params * pparams);

_XI_API_ XI_ERR_TYPE xiAxisAlignedBboxTransformA3D_S8(const xi_pArray RoIParam,
                                                      const xi_pTile3D BBoxDeltaTile,
                                                      const xi_pArray batchSplitData,
                                                      xi_pArray lutArray,
                                                      xi_pTile3D outBoxTile,
                                                      const xi_cnn_axis_aligned_bbox_transformA3D_params * pparams);

_XI_API_ XI_ERR_TYPE xiAxisAlignedBboxTransformA3D_S8_LUT(const xi_pArray RoIParam,
                                                          const xi_pTile3D BBoxDeltaTile,
                                                          const xi_pArray batchSplitData,
                                                          const xi_pArray lutArray,
                                                          xi_pTile3D outBoxTile,
                                                          const xi_cnn_axis_aligned_bbox_transformA3D_params * pparams);

_XI_API_ XI_ERR_TYPE xiGenerateProposalsA3D_I8_SetInfo_c(const double anchorScale,
                                                         const double bboxDeltaScale,
                                                         const int16_t bboxDeltaZeroPt,
                                                         const uint16_t imageWidth,
                                                         const uint16_t imageHeight,
                                                         const float spatialScaleX,
                                                         const float spatialScaleY,
                                                         const float minSize,
                                                         const float iouThreshold,
                                                         xi_cnn_axis_aligned_bbox_transformA3D_params* bboxParams,
                                                         xi_cnn_generate_proposalsA3D_I8_params* pparams);

_XI_API_ XI_ERR_TYPE xiGenerateProposalsA3D_U8(const xi_pTile3D inScoreTile,
                                               const xi_pTile4D bBoxDeltaTile,
                                               xi_pArray batchSplitData,
                                               const xi_pArray anchorArray,
                                               xi_pArray RoIParam,
                                               xi_pTile3D intermediateBBoxTile,
                                               xi_pArray sortedIdxArray,
                                               xi_pArray outBBoxArray,
                                               xi_pArray outScoreArray,
                                               const xi_cnn_generate_proposalsA3D_I8_params* pparams);

_XI_API_ XI_ERR_TYPE xiGenerateProposalsA3D_S8(const xi_pTile3D inScoreTile,
                                               const xi_pTile4D bBoxDeltaTile,
                                               xi_pArray batchSplitData,
                                               const xi_pArray anchorArray,
                                               xi_pArray RoIParam,
                                               xi_pTile3D intermediateBBoxTile,
                                               xi_pArray sortedIdxArray,
                                               xi_pArray outBBoxArray,
                                               xi_pArray outScoreArray,
                                               const xi_cnn_generate_proposalsA3D_I8_params* pparams);

_XI_API_ XI_ERR_TYPE xiBoxWithNMSLimitA3D_U8(const xi_pTile3D inBBoxTile,
                                             const xi_pArray inBatchSplitData,
                                             xi_pArray inScoreArray,
                                             xi_pArray intermediateScoreArray,
                                             xi_pArray sortedIdxArray,
                                             xi_pArray outBatchSplitData,
                                             xi_pArray outScoreArray,
                                             xi_pArray outBBoxArray,
                                             xi_pArray outClassArray,
                                             const xi_cnn_box_with_NMS_limitA3D_params * pparams);

_XI_API_ XI_ERR_TYPE xiBoxWithNMSLimitA3D_S8(const xi_pTile3D inBBoxTile,
                                             const xi_pArray inBatchSplitData,
                                             xi_pArray inScoreArray,
                                             xi_pArray intermediateScoreArray,
                                             xi_pArray sortedIdxArray,
                                             xi_pArray outBatchSplitData,
                                             xi_pArray outScoreArray,
                                             xi_pArray outBBoxArray,
                                             xi_pArray outClassArray,
                                             const xi_cnn_box_with_NMS_limitA3D_params * pparams);

_XI_API_ XI_ERR_TYPE xiResizeNearestNeighborA3D_I8_DWH(const xi_pTile3D inTile,
                                                       xi_pTile3D outTile,
                                                       const xi_cnn_resizeA3D_params * pparams);

_XI_API_ XI_ERR_TYPE xiResizeBilinearA3D_U8_DWH(const xi_pTile3D inTile,
                                                xi_pTile3D outTile,
                                                const xi_cnn_resizeA3D_params * pparams);

_XI_API_ XI_ERR_TYPE xiResizeBilinearA3D_S8_DWH(const xi_pTile3D inTile,
                                                xi_pTile3D outTile,
                                                const xi_cnn_resizeA3D_params * pparams);

#endif

/*Float half precision (16bit) optimized variants*/
#if (XCHAL_HAVE_VISION_HP_VFPU == 1)


 typedef struct
{
  int32_t axis;                  //axis along which softmax is to be computed
  xb_f16 beta;                   //multiplication factor
} xi_cnn_softmaxA3D_F16_params;


#define XI_CNN_SOFTMAXAF16_PARAMS_GET_AXIS(x)           ((x)->axis)
#define XI_CNN_SOFTMAXAF16_PARAMS_GET_BETA(x)           ((x)->beta)
#define XI_CNN_SOFTMAXAF16_PARAMS_SET_AXIS(x, v)        ((x)->axis = (v))
#define XI_CNN_SOFTMAXAF16_PARAMS_SET_BETA(x, v)        ((x)->beta = (v))

typedef struct
{
  xb_f16 fpAtol; // Absolute tolerance
  xb_f16 fpRtol; // Relative tolerance
} xi_cnn_heatmap_max_keypointA3D_F16_params;

#define XI_CNN_HEATMAPMAX_F16_PARAMS_GET_ABSTOL(x)           ((x)->fpAtol)
#define XI_CNN_HEATMAPMAX_F16_PARAMS_GET_RELTOL(x)           ((x)->fpRtol)
#define XI_CNN_HEATMAPMAX_F16_PARAMS_SET_ABSTOL(x, v)        ((x)->fpAtol = (v))
#define XI_CNN_HEATMAPMAX_F16_PARAMS_SET_RELTOL(x, v)        ((x)->fpRtol = (v))

typedef struct
{
  xb_f16 thresholdScore;        // Score threshold is represented as half precision floating point value
  xb_f16 thresholdNms;          //NMS threshold is represented as half precision floating point value
  xb_f16 thresholdIou;          //IOU threshold is represented as half precision floating point value
  int32_t detectionsPerImage;   // max number of proposals to keep per image.
  int32_t softNmsKernel;        // It can take values of 0, 1 and 2
  xb_f16 oneBySigma;            // Applicable when softNmsKernel is set to 2
} xi_cnn_box_with_NMS_limitA3D_F16_params;

#define XI_CNN_BOX_WITH_NMS_LIMIT_GET_THRESHOLD_SCORE_FP16(x)         ((x)->thresholdScore)
#define XI_CNN_BOX_WITH_NMS_LIMIT_GET_THRESHOLD_NMS_FP16(x)           ((x)->thresholdNms)
#define XI_CNN_BOX_WITH_NMS_LIMIT_GET_THRESHOLD_IOU_FP16(x)           ((x)->thresholdIou)
#define XI_CNN_BOX_WITH_NMS_LIMIT_GET_DETECTIONS_PER_IMAGE_FP16(x)    ((x)->detectionsPerImage)
#define XI_CNN_BOX_WITH_NMS_LIMIT_GET_SOFT_NMS_KERNEL_FP16(x)         ((x)->softNmsKernel)
#define XI_CNN_BOX_WITH_NMS_LIMIT_GET_ONE_BY_SIGMA_FP16(x)            ((x)->oneBySigma)

#define XI_CNN_BOX_WITH_NMS_LIMIT_SET_THRESHOLD_SCORE_FP16(x, v)         ((x)->thresholdScore = (v))
#define XI_CNN_BOX_WITH_NMS_LIMIT_SET_THRESHOLD_NMS_FP16(x, v)           ((x)->thresholdNms = (v))
#define XI_CNN_BOX_WITH_NMS_LIMIT_SET_THRESHOLD_IOU_FP16(x, v)           ((x)->thresholdIou = (v))
#define XI_CNN_BOX_WITH_NMS_LIMIT_SET_DETECTIONS_PER_IMAGE_FP16(x, v)    ((x)->detectionsPerImage = (v))
#define XI_CNN_BOX_WITH_NMS_LIMIT_SET_SOFT_NMS_KERNEL_FP16(x, v)         ((x)->softNmsKernel = (v))
#define XI_CNN_BOX_WITH_NMS_LIMIT_SET_ONE_BY_SIGMA_FP16(x, v)            ((x)->oneBySigma = (v))

typedef struct
{
  xb_f16 imageWidth;
  xb_f16 imageHeight;
} xi_cnn_axis_aligned_bbox_transformA3D_F16_params;

#define XI_CNN_AXIS_ALIGNED_BBOX_TRANSFORM_F16_GET_IMAGE_WIDTH(x)                 ((x)->imageWidth)
#define XI_CNN_AXIS_ALIGNED_BBOX_TRANSFORM_F16_GET_IMAGE_HEIGHT(x)                ((x)->imageHeight)

#define XI_CNN_AXIS_ALIGNED_BBOX_TRANSFORM_F16_SET_IMAGE_WIDTH(x, v)              ((x)->imageWidth = (v))
#define XI_CNN_AXIS_ALIGNED_BBOX_TRANSFORM_F16_SET_IMAGE_HEIGHT(x, v)             ((x)->imageHeight = (v))

typedef struct
{
  xb_f16 scaleX; // scale value for Bounding Box deltaX
  xb_f16 scaleY; // scale value for Bounding Box deltaY
  xb_f16 scaleW; // scale value for Bounding Box deltaW
  xb_f16 scaleH; // scale value for Bounding Box deltaH
  xi_bool  useRegularNms; //set to true to use regular multi-class NMS algorithm, set to false for faster NMS algorithm
  int32_t maxNumDetections; //maximum number of boxes for the output
  int32_t maxNumClassesPerDetection; //used when UseRegularNms is false, specifies the maximum number of classes per detection.
  int32_t maxNumDetectionsPerClass; //used when UseRegularNms is true, specifies the maximum number of detections per class.
  xb_f16 scoreThreshold; //threshold for input scores
  xb_f16 iouThreshold; //threshold for hard NMS
  xi_bool isBackgroundInLabelMap; //set to true to include background class in the list of label map for the output
}xi_cnn_detection_postprocessA3D_F16_params;

#define XI_CNN_DETECTIONS_POSTPROCESS_GET_SCALEX_FP16(x)                    ((x)->scaleX)
#define XI_CNN_DETECTIONS_POSTPROCESS_GET_SCALEY_FP16(x)                    ((x)->scaleY)
#define XI_CNN_DETECTIONS_POSTPROCESS_GET_SCALEW_FP16(x)                    ((x)->scaleW)
#define XI_CNN_DETECTIONS_POSTPROCESS_GET_SCALEH_FP16(x)                    ((x)->scaleH)
#define XI_CNN_DETECTIONS_POSTPROCESS_GET_USE_REG_NMS_FP16(x)               ((x)->useRegularNms)
#define XI_CNN_DETECTIONS_POSTPROCESS_GET_MAX_DETECTIONS_FP16(x)            ((x)->maxNumDetections)
#define XI_CNN_DETECTIONS_POSTPROCESS_GET_MAX_CLASS_PER_DETECTION_FP16(x)   ((x)->maxNumClassesPerDetection)
#define XI_CNN_DETECTIONS_POSTPROCESS_GET_MAX_DETECTIONS_PER_CLASS_FP16(x)  ((x)->maxNumDetectionsPerClass)
#define XI_CNN_DETECTIONS_POSTPROCESS_GET_THRESHOLD_SCORE_FP16(x)           ((x)->scoreThreshold)
#define XI_CNN_DETECTIONS_POSTPROCESS_GET_THRESHOLD_NMS_FP16(x)             ((x)->iouThreshold)
#define XI_CNN_DETECTIONS_POSTPROCESS_GET_IS_BG_LABEL_FP16(x)               ((x)->isBackgroundInLabelMap)

#define XI_CNN_DETECTIONS_POSTPROCESS_SET_SCALEX_FP16(x, v)                    ((x)->scaleX = (v))
#define XI_CNN_DETECTIONS_POSTPROCESS_SET_SCALEY_FP16(x, v)                    ((x)->scaleY = (v))
#define XI_CNN_DETECTIONS_POSTPROCESS_SET_SCALEW_FP16(x, v)                    ((x)->scaleW = (v))
#define XI_CNN_DETECTIONS_POSTPROCESS_SET_SCALEH_FP16(x, v)                    ((x)->scaleH = (v))
#define XI_CNN_DETECTIONS_POSTPROCESS_SET_USE_REG_NMS_FP16(x, v)               ((x)->useRegularNms = (v))
#define XI_CNN_DETECTIONS_POSTPROCESS_SET_MAX_DETECTIONS_FP16(x, v)            ((x)->maxNumDetections = (v))
#define XI_CNN_DETECTIONS_POSTPROCESS_SET_MAX_CLASS_PER_DETECTION_FP16(x, v)   ((x)->maxNumClassesPerDetection = (v))
#define XI_CNN_DETECTIONS_POSTPROCESS_SET_MAX_DETECTIONS_PER_CLASS_FP16(x, v)  ((x)->maxNumDetectionsPerClass = (v))
#define XI_CNN_DETECTIONS_POSTPROCESS_SET_THRESHOLD_SCORE_FP16(x, v)           ((x)->scoreThreshold = (v))
#define XI_CNN_DETECTIONS_POSTPROCESS_SET_THRESHOLD_NMS_FP16(x, v)             ((x)->iouThreshold = (v))
#define XI_CNN_DETECTIONS_POSTPROCESS_SET_IS_BG_LABEL_FP16(x, v)               ((x)->isBackgroundInLabelMap = (v))


typedef struct
{
  xb_f16 spatialScaleX; // ratio of the width of original image to the height of feature map
  xb_f16 spatialScaleY; // ratio of the height of original image to the height of feature map
  float_t minSize;       // minimum threshold for the width/height of the output ROI bounding boxes.
  int32_t preNMSTopN;    // max number of proposals to keep before applying NMS
  int32_t postNMSTopN;   // max number of proposals to keep after applying NMS
  xb_f16 thresholdNMS;    // threshold of IoU for suppressing overlapping proposals.
  xb_f16 imageWidth;
  xb_f16 imageHeight;
} xi_cnn_generate_proposalsA3D_F16_params;

#define XI_CNN_GENERATE_PROPOSALS_F16_GET_SPATIAL_SCALEX(x)         ((x)->spatialScaleX)
#define XI_CNN_GENERATE_PROPOSALS_F16_GET_SPATIAL_SCALEY(x)         ((x)->spatialScaleY)
#define XI_CNN_GENERATE_PROPOSALS_F16_GET_MINIMUM_SIZE(x)           ((x)->minSize)
#define XI_CNN_GENERATE_PROPOSALS_F16_GET_PRE_NMS_TOPN(x)           ((x)->preNMSTopN)
#define XI_CNN_GENERATE_PROPOSALS_F16_GET_POST_NMS_TOPN(x)          ((x)->postNMSTopN)
#define XI_CNN_GENERATE_PROPOSALS_F16_GET_THRESHOLD_NMS(x)          ((x)->thresholdNMS)
#define XI_CNN_GENERATE_PROPOSALS_F16_GET_IMAGE_WIDTH(x)            ((x)->imageWidth)
#define XI_CNN_GENERATE_PROPOSALS_F16_GET_IMAGE_HEIGHT(x)           ((x)->imageHeight)

#define XI_CNN_GENERATE_PROPOSALS_F16_SET_SPATIAL_SCALEX(x, v)      ((x)->spatialScaleX = (v))
#define XI_CNN_GENERATE_PROPOSALS_F16_SET_SPATIAL_SCALEY(x, v)      ((x)->spatialScaleY = (v))
#define XI_CNN_GENERATE_PROPOSALS_F16_SET_MINIMUM_SIZE(x, v)        ((x)->minSize = (v))
#define XI_CNN_GENERATE_PROPOSALS_F16_SET_PRE_NMS_TOPN(x, v)        ((x)->preNMSTopN = (v))
#define XI_CNN_GENERATE_PROPOSALS_F16_SET_POST_NMS_TOPN(x, v)       ((x)->postNMSTopN = (v))
#define XI_CNN_GENERATE_PROPOSALS_F16_SET_THRESHOLD_NMS(x, v)       ((x)->thresholdNMS = (v))
#define XI_CNN_GENERATE_PROPOSALS_F16_SET_IMAGE_WIDTH(x, v)         ((x)->imageWidth = (v))
#define XI_CNN_GENERATE_PROPOSALS_F16_SET_IMAGE_HEIGHT(x, v)        ((x)->imageHeight = (v))

#define F16_MIN (-65504.0f)
#define F16_MAX (65504.0f)

#define CNN_ADDA_FLAG_RELU          1
typedef struct
{
  uint8_t  flags;                  // enableRelu

  /*bit0 corresponds to Relu flag.
   *  ------------------------------------------------------------------------
   *  | bit7  | bit6 | bit5 | bit4 | bit3 | bit2  | bit1 | bit0     |
   *  |unused |unused|unused|unused|unused| unused|unused|Relu Flag |
   *  ------------------------------------------------------------------------
   */
  xb_f16 reluMin;
  xb_f16 reluMax;
} xi_cnn_addA_F16_params;

#define XI_CNN_ADDA_GET_RELU_MIN(x)         ((x)->reluMin)
#define XI_CNN_ADDA_GET_RELU_MAX(x)        ((x)->reluMax)
#define XI_CNN_ADDA_GET_FLAGS(x)            ((x)->flags)
#define XI_CNN_ADDA_GET_FLAG_RELU(x)        ((x)->flags & CNN_ADDA_FLAG_RELU)
#define XI_CNN_ADDA_SET_RELU_MIN(x, v)      ((x)->reluMin = (v))
#define XI_CNN_ADDA_SET_RELU_MAX(x, v)      ((x)->reluMax = (v))
#define XI_CNN_ADDA_SET_FLAGS(x, v)         ((x)->flags = (v))
#define XI_CNN_ADDA_SET_FLAG_RELU(x)        ((x)->flags = ((x)->flags | CNN_ADDA_FLAG_RELU))
#define XI_CNN_ADDA_RESET_FLAG_RELU(x)      ((x)->flags = ((x)->flags & ~CNN_ADDA_FLAG_RELU))

#define CNN_MULA_FLAG_RELU          1
typedef struct
{
  uint8_t  flags;                  // enableRelu

  /*bit0 corresponds to Relu flag.
   *  ------------------------------------------------------------------------
   *  | bit7  | bit6 | bit5 | bit4 | bit3 | bit2  | bit1 | bit0     |
   *  |unused |unused|unused|unused|unused| unused|unused|Relu Flag |
   *  ------------------------------------------------------------------------
   */
  xb_f16 reluMin;
  xb_f16 reluMax;
} xi_cnn_mulA_F16_params;

#define XI_CNN_MULA_GET_RELU_MIN(x)         ((x)->reluMin)
#define XI_CNN_MULA_GET_RELU_MAX(x)        ((x)->reluMax)
#define XI_CNN_MULA_GET_FLAGS(x)            ((x)->flags)
#define XI_CNN_MULA_GET_FLAG_RELU(x)        ((x)->flags & CNN_MULA_FLAG_RELU)
#define XI_CNN_MULA_SET_RELU_MIN(x, v)      ((x)->reluMin = (v))
#define XI_CNN_MULA_SET_RELU_MAX(x, v)      ((x)->reluMax = (v))
#define XI_CNN_MULA_SET_FLAGS(x, v)         ((x)->flags = (v))
#define XI_CNN_MULA_SET_FLAG_RELU(x)        ((x)->flags = ((x)->flags | CNN_MULA_FLAG_RELU))
#define XI_CNN_MULA_RESET_FLAG_RELU(x)      ((x)->flags = ((x)->flags & ~CNN_MULA_FLAG_RELU))

typedef struct
{
  xb_f16 activationMin;
  xb_f16 activationMax;
} xi_cnn_divA_F16_params;

#define XI_CNN_DIVA_GET_ACTIVATION_MIN(x)        ((x)->activationMin)
#define XI_CNN_DIVA_GET_ACTIVATION_MAX(x)        ((x)->activationMax)
#define XI_CNN_DIVA_SET_ACTIVATION_MIN(x, v)     ((x)->activationMin = (v))
#define XI_CNN_DIVA_SET_ACTIVATION_MAX(x, v)     ((x)->activationMax = (v))


#define CNNA_CONV_F16_FLAG_RELU          1
#define CNNA_CONV_F16_FLAG_LEFTEDGE      (1 << 1)
#define CNNA_CONV_F16_FLAG_TOPEDGE       (1 << 2)
typedef struct
{
  uint8_t  stride;
  uint8_t  flags;                  // enableRelu, contiguous-data, edge flag;
  uint8_t  dilation;
  /*bit0 corresponds to Relu flag.
   * bit1 corresponds to leftEdgeFlag
   * bit2 corresponds to topEdgeFlag
   *  --------------------------------------------------------------------------
   *  | bit7  | bit6 | bit5 | bit4 | bit3 | bit2       | bit1       | bit0     |
   *  |unused |unused|unused|unused|unused| topEdgeFlag|leftEdgeFlag|Relu Flag |
   *  --------------------------------------------------------------------------
   */
  xb_f16 reluMin;
  xb_f16 reluMax;
}xi_cnna_conv_F16_params;

#define XI_CNNA_CONV_F16_GET_STRIDE(x)           ((x)->stride)
#define XI_CNNA_CONV_F16_SET_STRIDE(x, v)        ((x)->stride = (v))
#define XI_CNNA_CONV_F16_GET_RELU_MIN(x)         ((x)->reluMin)
#define XI_CNNA_CONV_F16_GET_RELU_MAX(x)         ((x)->reluMax)
#define XI_CNNA_CONV_F16_GET_FLAGS(x)            ((x)->flags)
#define XI_CNNA_CONV_F16_GET_FLAG_RELU(x)        ((x)->flags & CNNA_CONV_F16_FLAG_RELU)
#define XI_CNNA_CONV_F16_SET_RELU_MIN(x, v)      ((x)->reluMin = (v))
#define XI_CNNA_CONV_F16_SET_RELU_MAX(x, v)      ((x)->reluMax = (v))
#define XI_CNNA_CONV_F16_SET_FLAGS(x, v)         ((x)->flags = (v))
#define XI_CNNA_CONV_F16_SET_FLAG_RELU(x)        ((x)->flags = ((x)->flags | CNNA_CONV_F16_FLAG_RELU))
#define XI_CNNA_CONV_F16_RESET_FLAG_RELU(x)      ((x)->flags = ((x)->flags & ~CNNA_CONV_F16_FLAG_RELU))
#define XI_CNNA_CONV_F16_GET_FLAG_LEFTEDGE(x)    ((x)->flags & CNNA_CONV_F16_FLAG_LEFTEDGE)
#define XI_CNNA_CONV_F16_SET_FLAG_LEFTEDGE(x)    ((x)->flags = ((x)->flags | CNNA_CONV_F16_FLAG_LEFTEDGE))
#define XI_CNNA_CONV_F16_RESET_FLAG_LEFTEDGE(x)  ((x)->flags = ((x)->flags & ~CNNA_CONV_F16_FLAG_LEFTEDGE))
#define XI_CNNA_CONV_F16_GET_FLAG_TOPEDGE(x)     ((x)->flags & CNNA_CONV_F16_FLAG_TOPEDGE)
#define XI_CNNA_CONV_F16_SET_FLAG_TOPEDGE(x)     ((x)->flags = ((x)->flags | CNNA_CONV_F16_FLAG_TOPEDGE))
#define XI_CNNA_CONV_F16_RESET_FLAG_TOPEDGE(x)   ((x)->flags = ((x)->flags & ~CNNA_CONV_F16_FLAG_TOPEDGE))
#define XI_CNNA_CONV_F16_GET_DILATION(x)         ((x)->dilation)
#define XI_CNNA_CONV_F16_SET_DILATION(x, v)      ((x)->dilation = (v))


typedef struct
{
  float_t scale;
  int32_t offset;
} xi_cnn_quantDequantA_params;

#define XI_CNN_QUANT_DEQUANT_GET_SCALE(x)     ((x)->scale)
#define XI_CNN_QUANT_DEQUANT_SET_SCALE(x, v)  ((x)->scale = (v))

#define XI_CNN_QUANT_DEQUANT_GET_OFFSET(x)    ((x)->offset)
#define XI_CNN_QUANT_DEQUANT_SET_OFFSET(x, v) ((x)->offset = (v))

typedef struct
{
  float widthStride;                 // Multiplicative Inverse of  Stride factor to translate ROI coords from their
                                     // input scale to the scale used when pooling
  float heightStride;
  int32_t  widthSamplingRatio;       //Number of sampling points used to compute the output, set to <= 0 for an
                                     //adaptive number of ceil(roi_width / out_width)
  int32_t  heightSamplingRatio;      //Number of sampling points used to compute the
                                     //output, set to <= 0 for an adaptive number of ceil(roi_height / out_height)
} xi_cnn_roi_align_F16_params;

#define XI_CNN_ROI_ALIGN_F16_GET_WIDTH_STRIDE(x)                  ((x)->widthStride)
#define XI_CNN_ROI_ALIGN_F16_SET_WIDTH_STRIDE(x, v)               ((x)->widthStride = (v))
#define XI_CNN_ROI_ALIGN_F16_GET_HEIGHT_STRIDE(x)                  ((x)->heightStride)
#define XI_CNN_ROI_ALIGN_F16_SET_HEIGHT_STRIDE(x, v)               ((x)->heightStride = (v))

#define XI_CNN_ROI_ALIGN_F16_GET_SAMPLING_WIDTH_RATIO(x)                  ((x)->widthSamplingRatio)
#define XI_CNN_ROI_ALIGN_F16_SET_SAMPLING_WIDTH_RATIO(x, v)               ((x)->widthSamplingRatio = (v))
#define XI_CNN_ROI_ALIGN_F16_GET_SAMPLING_HEIGHT_RATIO(x)                  ((x)->heightSamplingRatio)
#define XI_CNN_ROI_ALIGN_F16_SET_SAMPLING_HEIGHT_RATIO(x, v)               ((x)->heightSamplingRatio = (v))


_XI_API_ XI_ERR_TYPE xiBroadcastAddA3D_F16(const xi_pTile3D inTile1,
                                           const xi_pTile3D inTile2,
                                           xi_pTile3D outTile,
                                           const xi_cnn_addA_F16_params * params);

_XI_API_ XI_ERR_TYPE xiBroadcastEltwiseEqualA3D_F16(const xi_pTile3D inTile1,
                                                    const xi_pTile3D inTile2,
                                                    xi_pTile3D outTile);

_XI_API_ XI_ERR_TYPE xiBroadcastEltwiseNotEqualA3D_F16(const xi_pTile3D inTile1,
                                                       const xi_pTile3D inTile2,
                                                       xi_pTile3D outTile);

_XI_API_ XI_ERR_TYPE xiBroadcastMulA3D_F16(const xi_pTile3D inTile1,
                                           const xi_pTile3D inTile2,
                                           xi_pTile3D outTile,
                                           const xi_cnn_mulA_F16_params * params);

_XI_API_ XI_ERR_TYPE xiFullyConnectedA3D_F16(const xi_pTile3D inTile,
                                             const xi_pTile4D coeffTile,
                                             const xi_pArray biasArray,
                                             xi_pTile3D outTile,
                                             const xi_cnna_conv_F16_params * params);

_XI_API_ XI_ERR_TYPE xiConvolved3D_MXN_F16_MOD_DWH(const xi_pTile3D inTile,
                                           const xi_pTile4D coeffTile,
                                           const xi_pArray biasArray,
                                           xi_pTile3D outTile,
                                           const xi_cnna_conv_F16_params *params);

_XI_API_ XI_ERR_TYPE xiPermuteA4D_F16(const xi_pTile4D inTile,
                                      xi_pTile4D outTile,
                                      const xi_cnn_permuteA4D_params *pparams);

_XI_API_ XI_ERR_TYPE xiSoftMaxA3D_F16(const xi_pTile3D inTile,
                                      xi_pTile3D outTile,
                                      xi_cnn_softmaxA3D_F16_params * params);

_XI_API_ XI_ERR_TYPE xiLogSoftMaxA3D_F16(const xi_pTile3D inTile,
                                         xi_pTile3D outTile,
                                         xi_cnn_softmaxA3D_F16_params * params);

_XI_API_ XI_ERR_TYPE xiHeatmapMaxKeypointA3D_F16(const xi_pTile4D heatmapTile,
                                                 const xi_pArray inBBBoxArray,
                                                 xi_pArray outScoreArray,
                                                 xi_pTile3D outKeypointTile,
                                                 xi_cnn_heatmap_max_keypointA3D_F16_params *pparams);

_XI_API_ XI_ERR_TYPE xiBoxWithNMSLimitA3D_F16(const xi_pTile3D inBBoxTile,
                                              const xi_pArray inBatchSplitData,
                                              xi_pArray inScoreArray,
                                              xi_pArray sortedIdxArray,
                                              xi_pArray outBatchSplitData,
                                              xi_pArray outScoreArray,
                                              xi_pArray outBBoxArray,
                                              xi_pArray outClassArray,
                                              const xi_cnn_box_with_NMS_limitA3D_F16_params * pparams);

_XI_API_ XI_ERR_TYPE xiAxisAlignedBboxTransformA3D_F16(const xi_pArray RoIParam,
                                             const xi_pTile3D BBoxDeltaTile,
                                             const xi_pArray batchSplitData,
                                             xi_pTile3D outBoxTile,
                                             const xi_cnn_axis_aligned_bbox_transformA3D_F16_params * pparams);

_XI_API_ XI_ERR_TYPE xiGenerateProposalsA3D_F16(const xi_pTile3D inScoreTile,
                                                const xi_pTile4D BBoxDeltaTile,
                                                xi_pArray batchSplitData,
                                                const xi_pArray anchorArray,
                                                xi_pArray RoIParam,
                                                xi_pTile3D intermediateBBoxTile,
                                                xi_pArray sortedIdxArray,
                                                xi_pArray outBBoxArray,
                                                xi_pArray outScoreArray,
                                                const xi_cnn_generate_proposalsA3D_F16_params* pparams);

_XI_API_ XI_ERR_TYPE xiDetectionPostprocessA3D_F16(const xi_pArray inScoreArray,
                                                  const xi_pArray inBBoxDeltaArray,
                                                  const xi_pArray anchorArray,
                                                  xi_pArray RoIParam,
                                                  xi_pArray maxScoreArray,
                                                  xi_pArray sortedIdxArray,
                                                  xi_pArray outScoreArray,
                                                  xi_pArray outBBoxArray,
                                                  xi_pArray outClassArray,
                                                  int32_t *numDetections,
                                                  const xi_cnn_detection_postprocessA3D_F16_params * pparams);

_XI_API_ XI_ERR_TYPE xiROIAlignA3D_F16_DWH(const xi_pTile3D inTile,
                                           const xi_pArray RoIParam,
                                           const xi_pArray batchSplitData,
                                           xi_pTile4D outTile,
                                           const xi_cnn_roi_align_F16_params *pparams,
                                           xi_pArray posWtsArr);


_XI_API_ XI_ERR_TYPE xiQuantizeA3D_F16U8(const xi_pTile3D inTile,
                                         xi_pTile3D outTile,
                                         const xi_cnn_quantDequantA_params *pparams);

_XI_API_ XI_ERR_TYPE xiQuantizeA3D_F16S8(const xi_pTile3D inTile,
                                         xi_pTile3D outTile,
                                         const xi_cnn_quantDequantA_params *pparams);

_XI_API_ XI_ERR_TYPE xiQuantizeA4D_F16U8(const xi_pTile4D inTile,
                                         xi_pTile4D outTile,
                                         const xi_cnn_quantDequantA_params *pparams);

_XI_API_ XI_ERR_TYPE xiQuantizeA4D_F16S8(const xi_pTile4D inTile,
                                         xi_pTile4D outTile,
                                         const xi_cnn_quantDequantA_params *pparams);

_XI_API_ XI_ERR_TYPE xiDeQuantizeA3D_U8F16(const xi_pTile3D inTile,
                                           xi_pTile3D outTile,
                                           xi_pArray  lut,
                                           const xi_cnn_quantDequantA_params *pparams);

_XI_API_ XI_ERR_TYPE xiDeQuantizeA3D_S8F16(const xi_pTile3D inTile,
                                           xi_pTile3D outTile,
                                           xi_pArray  lut,
                                           const xi_cnn_quantDequantA_params *pparams);

_XI_API_ XI_ERR_TYPE xiDeQuantizeA4D_U8F16(const xi_pTile4D inTile,
                                           xi_pTile4D outTile,
                                           xi_pArray  lut,
                                           const xi_cnn_quantDequantA_params *pparams);

_XI_API_ XI_ERR_TYPE xiDeQuantizeA4D_S8F16(const xi_pTile4D inTile,
                                           xi_pTile4D outTile,
                                           xi_pArray  lut,
                                           const xi_cnn_quantDequantA_params *pparams);

_XI_API_ XI_ERR_TYPE xiSqrtA3D_F16(const xi_pTile3D inTile,
                                   xi_pTile3D outTile);

_XI_API_ XI_ERR_TYPE xiRSqrtA3D_F16(const xi_pTile3D inTile,
                                    xi_pTile3D outTile);

_XI_API_ XI_ERR_TYPE xiDivA3D_F16(const xi_pTile3D numeratorTile,
                                  const xi_pTile3D denominatorTile,
                                  xi_pTile3D outTile,
                                  const xi_cnn_divA_F16_params *params);

_XI_API_ XI_ERR_TYPE xiBroadcastDivA3D_F16(const xi_pTile3D numeratorTile,
                                           const xi_pTile3D denominatorTile,
                                           xi_pTile3D outTile,
                                           const xi_cnn_divA_F16_params *params);

#endif //#if (XCHAL_HAVE_VISION_HP_VFPU == 1)
/* Q7 function variants */

_XI_API_ XI_ERR_TYPE xiGetDim3D_DWH_ID16WH(const xi_pTile3D inTile,
                                           xi_pTile3D outTile);

_XI_API_ XI_ERR_TYPE xiGetDim3D_WHD_ID16WH(const xi_pTile3D inTile,
                                                  xi_pTile3D outTile);

_XI_API_ XI_ERR_TYPE xiTranspose3D_I8_WHD_ID16WH(const xi_pTile3D inTile,
                                                 xi_pTile3D outTile);

_XI_API_ XI_ERR_TYPE xiTranspose3D_I8_ID16WH_WHD(const xi_pTile3D inTile,
                                                 xi_pTile3D outTile);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolveA2D_S_3x3_U8_MOD_ID16WH(const xi_pTile3D inTile,
                                                                const xi_pTile3D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolveA2D_S_MxN_U8_MOD_ID16WH(const xi_pTile3D inTile,
                                                                const xi_pTile3D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolveAVQ2D_S_3x3_S8_MOD_ID16WH(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  const xi_pArray outScaleArray,
                                                                  const xi_pArray outShiftArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiDepthwiseConvolveAVQ2D_S_MxN_S8_MOD_ID16WH(const xi_pTile3D inTile,
                                                                  const xi_pTile3D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  const xi_pArray outScaleArray,
                                                                  const xi_pArray outShiftArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnna_conv_params *param);

#if 1 //(defined(IVP_DMULQA2N8QXR8))
_XI_API_ XI_ERR_TYPE xiConvolvedA3D_S_MxN_U8S8U8_DWH_QM24_Q7(const xi_pTile3D inTile,
                                                             const xi_pArray inPtrOffsetArr,
                                                             const xi_pTile4D coeffTile,
                                                             const xi_pArray biasArray,
                                                             xi_pTile fixUpTile,
                                                             xi_pTile3D outTile,
                                                             const xi_cnna_conv_params *convParams);

_XI_API_ XI_ERR_TYPE xiConvolvedA3D_S_MxN_U8S8U8_ID16WH_QM24_Q7(const xi_pTile3D inTile,
                                                                const xi_pArray inPtrOffsetArr,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile fixUpTile,
                                                                xi_pTile3D outTile,
                                                                const xi_cnna_conv_params *convParams);

_XI_API_ XI_ERR_TYPE xiConvolvedA3D_S_MxN_U8S8U8_ID16WH_QM32_Q7(const xi_pTile3D inTile,
                                                                const xi_pArray inPtrOffsetArr,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile fixUpTile,
                                                                xi_pTile3D outTile,
                                                                const xi_cnna_conv_params *convParams);

_XI_API_ XI_ERR_TYPE xiConvolvedA3D_S_MxN_U8S8U8_DWH_QM32_Q7(const xi_pTile3D inTile,
                                                             const xi_pArray inPtrOffsetArr,
                                                             const xi_pTile4D coeffTile,
                                                             const xi_pArray biasArray,
                                                             xi_pTile fixUpTile,
                                                             xi_pTile3D outTile,
                                                             const xi_cnna_conv_params *convParams);

_XI_API_ XI_ERR_TYPE xiConvolvedFixupA3D_MxN_U8S32_ID16WH(const xi_pTile3D inTile,
                                                          const xi_pTile4D coeffTile,
                                                          xi_pArray bufArray,
                                                          xi_pTile fixUpTile,
                                                          const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiConvolvedA3D_S_MxN_U8S8U8Ca2_MOD_ID16WH_DWH_QM24(const xi_pTile3D inTile,
                                                                        const xi_pArray inPtrOffsetArr,
                                                                        const xi_pTile4D coeffTile,
                                                                        const xi_pArray biasArray,
                                                                        const xi_pTile fixUpTile,
                                                                        xi_pTile3D outTile,
                                                                        const xi_cnna_conv_params *convParams);

_XI_API_ XI_ERR_TYPE xiConvolvedA3D_S_MxN_U8S8U8Ca2_MOD_ID16WH_DWH_QM32(const xi_pTile3D inTile,
                                                                        const xi_pArray inPtrOffsetArr,
                                                                        const xi_pTile4D coeffTile,
                                                                        const xi_pArray biasArray,
                                                                        const xi_pTile fixUpTile,
                                                                        xi_pTile3D outTile,
                                                                        const xi_cnna_conv_params *convParams);

_XI_API_ XI_ERR_TYPE xiConvolvedAVQ3D_S_MxN_S8_ID16WH_QM24_Q7(const xi_pTile3D inTile,
                                                              const xi_pArray inPtrOffsetArr,
                                                              const xi_pTile4D coeffTile,
                                                              const xi_pArray biasArray,
                                                              const xi_pArray outScaleArray,
                                                              const xi_pArray outShiftArray,
                                                              xi_pTile3D outTile,
                                                              const xi_cnna_conv_params *convParams);

_XI_API_ XI_ERR_TYPE xiConvolvedAVQ3D_S_MxN_S8_ID16WH_QM32_Q7(const xi_pTile3D inTile,
                                                              const xi_pArray inPtrOffsetArr,
                                                              const xi_pTile4D coeffTile,
                                                              const xi_pArray biasArray,
                                                              const xi_pArray outScaleArray,
                                                              const xi_pArray outShiftArray,
                                                              xi_pTile3D outTile,
                                                              const xi_cnna_conv_params *convParams);

_XI_API_ XI_ERR_TYPE xiConvolvedAVQ3D_S_MxN_U8S8U8_ID16WH_QM24_Q7(const xi_pTile3D inTile,
                                                                  const xi_pArray inPtrOffsetArr,
                                                                  const xi_pTile4D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  const xi_pArray outScaleArray,
                                                                  const xi_pArray outShiftArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnna_conv_params *convParams);

_XI_API_ XI_ERR_TYPE xiConvolvedAVQ3D_S_MxN_U8S8U8_ID16WH_QM32_Q7(const xi_pTile3D inTile,
                                                                  const xi_pArray inPtrOffsetArr,
                                                                  const xi_pTile4D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  const xi_pArray outScaleArray,
                                                                  const xi_pArray outShiftArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnna_conv_params *convParams);

_XI_API_ XI_ERR_TYPE xiConvolvedAVQ3D_S_MxN_S8_DWH_QM24_Q7(const xi_pTile3D inTile,
                                                           const xi_pArray inPtrOffsetArr,
                                                           const xi_pTile4D coeffTile,
                                                           const xi_pArray biasArray,
                                                           const xi_pArray outScaleArray,
                                                           const xi_pArray outShiftArray,
                                                           xi_pTile fixUpTile,
                                                           xi_pTile3D outTile,
                                                           const xi_cnna_conv_params *convParams);

_XI_API_ XI_ERR_TYPE xiConvolvedAVQ3D_S_MxN_S8_DWH_QM32_Q7(const xi_pTile3D inTile,
                                                           const xi_pArray inPtrOffsetArr,
                                                           const xi_pTile4D coeffTile,
                                                           const xi_pArray biasArray,
                                                           const xi_pArray outScaleArray,
                                                           const xi_pArray outShiftArray,
                                                           xi_pTile fixUpTile,
                                                           xi_pTile3D outTile,
                                                           const xi_cnna_conv_params *convParams);




_XI_API_ XI_ERR_TYPE xiconvolvedA3D_S_MxN_U8S8U8_ID16WH_DWH_QM32(const xi_pTile3D inTile,
                                               const xi_pArray inPtrOffsetArr,
                                               const xi_pTile4D coeffTile,
                                               const xi_pArray biasArray,
                                               const xi_pTile fixUpTile,
                                               xi_pTile3D outTile,
                                               const xi_cnna_conv_params *convParams);

_XI_API_ XI_ERR_TYPE xiconvolvedA3D_S_MxN_U8S8U8_ID16WH_DWH_QM24(const xi_pTile3D inTile,
                                               const xi_pArray inPtrOffsetArr,
                                               const xi_pTile4D coeffTile,
                                               const xi_pArray biasArray,
                                               const xi_pTile fixUpTile,
                                               xi_pTile3D outTile,
                                               const xi_cnna_conv_params *param);
_XI_API_ XI_ERR_TYPE xiconvolvedFixupA3D_MxN_U8S32_DWH_ID16WH_QM24(const xi_pTile3D inTile,
                                                 const xi_pTile4D coeffTile,
                                                 const xi_pTile3D outTile,
                                                 xi_pTile fixUpTile,
                                                 const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiconvolvedA3D_S_MxN_U8S8U8_ID16WH_QM32(const xi_pTile3D inTile,
                                                             const xi_pArray inPtrOffsetArr,
                                                             const xi_pTile4D coeffTile,
                                                             const xi_pArray biasArray,
                                                             const xi_pTile fixUpTile,
                                                             xi_pTile3D outTile,
                                                             const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiconvolvedA3D_S_MxN_U8S8U8_ID16WH_QM24(const xi_pTile3D inTile,
                                                             const xi_pArray inPtrOffsetArr,
                                                             const xi_pTile4D coeffTile,
                                                             const xi_pArray biasArray,
                                                             const xi_pTile fixUpTile,
                                                             xi_pTile3D outTile,
                                                             const xi_cnna_conv_params *param);

_XI_API_ XI_ERR_TYPE xiconvolvedFixupA3D_MxN_U8S32_ID16WH_QM24(const xi_pTile3D inTile,
                                                               const xi_pTile4D coeffTile,
                                                               const xi_pTile3D outTile,
                                                               xi_pTile fixUpTile,
                                                               const xi_cnna_conv_params *param);
_XI_API_ XI_ERR_TYPE xiextendEdges3DA_ID16WH(xi_pTile3D outTile,
                                             const int value,
                                             xi_size3D frame3DSize);
#endif

_XI_API_ XI_ERR_TYPE xiComputeOffset_Convd3D_MOD(const xi_pTile3D inTile,
                                                 xi_pArray  inPtrOffsetArr,
                                                 const uint8_t dilationX,
                                                 const uint8_t dilationY,
                                                 const uint8_t strideX,
                                                 const uint8_t strideY,
                                                 const uint8_t kWidth,
                                                 const uint8_t kHeight);

_XI_API_ XI_ERR_TYPE xiReOrder3D_I8_DWH_ID16WH(const xi_pTile3D inTile,
                                               xi_pTile3D outTile);

_XI_API_ XI_ERR_TYPE xiReOrder3D_I8_ID16WH_DWH(const xi_pTile3D inTile,
                                               xi_pTile3D outTile);

#endif //if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))

