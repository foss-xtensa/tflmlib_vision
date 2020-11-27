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
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "xi_tile3d_manager.h"
#include "flk_conv.h"
#include "xi_core_api.h"
#include "xi_cnn_api.h"
#include "xi_api_ref.h"
#include "xi_core.h"
#include "xi_cnn.h"

#define min(x,y) ((x) > (y) ? (y) : (x))
#define max(x,y) ((x) > (y) ? (x) : (y))

#if ((XCHAL_VISION_TYPE >= 6)) // && defined(INCLUDE_XI_CNN))
#define S24_MIN  (-(((int32_t) 1) << 23))
#define S24_MAX  ((((int32_t) 1) << 23) - 1)
#define CLAMP(v, min, max)                      ((v) < (min) ? (min) : (v) > (max) ? (max) : (v))
#define ROUND(x, s)                             (((s) == 0) ? (x) : (((x) + ((int64_t)1 << ((s) - 1))) >> (s)))
#define ROUND64B_ASYMM(x, s)                    (((s) == 0) ? (x) : \
                                                 (((x) + ((int64_t) 1 << ((s) - 1))) >> (s)))

#define ROUND_N_CLAMP64B_ASYMM(x, s, min, max)  (((s) == 0) ? (CLAMP(x, min, max)) : \
                                                 (CLAMP(ROUND64B_ASYMM(x, s), min, max)))

#define ROUND_N_CLAMP64B_SYMM(x, s, min, max)   (((s) == 0) ? (CLAMP(x, min, max)) : \
                                                 (CLAMP(ROUND64B_SYMM(x, s), min, max)))

#define ROUND64B_SYMM(x, s)                     (((s) == 0) ? (x) :                                         \
                                                 (((x) > 0) ? (((x) + (((int64_t) 1) << (s - 1))) >> (s)) : \
                                                  -(((-x) + (((int64_t) 1) << (s - 1))) >> (s))))

#define ROUND_N_CLAMP64B        ROUND_N_CLAMP64B_ASYMM
#ifdef ENABLE_CONV_ASYMMETRIC_ROUNDING
#define MORPH_ROUND64B          ROUND64B_ASYMM
#define MORPH_ROUND_N_CLAMP64B  ROUND_N_CLAMP64B_ASYMM
#else
#define MORPH_ROUND64B          ROUND64B_SYMM
#define MORPH_ROUND_N_CLAMP64B  ROUND_N_CLAMP64B_SYMM
#endif
#endif


XI_ERR_TYPE xiConvolveA3D_S_MxN_U8_reff(const xi_pTile3D inTile,
                                        const xi_pTile4D coeffTile,
                                        const xi_pArray biasArray,
                                        xi_pTile3D outTile,
                                        const xi_cnna_conv_params *param,
                                        const int32_t numGroups,
                                        const int8_t edgeExtendLeft,
                                        const int8_t edgeExtendRight,
                                        const int8_t edgeExtendTop,
                                        const int8_t edgeExtendBottom,
                                        const xi_pArray edgeExtendValArray)
{
#if 1 // TODO
  /* Error Checks */
  /*  Input datatype can U8 */
  if (!xiTile3DIsValid_U8_ref(inTile))
  {
    return(XI_ERR_BADARG);
  }
  if (!xiTile4DIsValid_U8_ref(coeffTile))
  {
    return(XI_ERR_BADARG);
  }
  if (!xiTile3DIsValid_U8_ref(outTile))
  {
    return(XI_ERR_BADARG);
  }
  if (!param)
  {
    return(XI_ERR_NULLARG);
  }
  if (!xiArrayIsValid_S32_ref(biasArray))
  {
    return(XI_ERR_BADARG);
  }
#endif
  /* Supported Data Orders for
   * Input Tile       : WHD, DWH
   * Coefficient Tile : WHDN, NDWH, DWHN
   * Output Tile      : WHD, DWH
   */
  if (!((XI_TILE3D_GET_DATA_ORDER(inTile) == XI_WHD) || \
        (XI_TILE3D_GET_DATA_ORDER(inTile) == XI_DWH) || \
        (XI_TILE3D_GET_DATA_ORDER(inTile) == XI_ID16WH)))
  {
    return(XI_ERR_BADARG);
  }

  if (!((XI_TILE3D_GET_DATA_ORDER(outTile) == XI_WHD) || \
        (XI_TILE3D_GET_DATA_ORDER(outTile) == XI_DWH) || \
        (XI_TILE3D_GET_DATA_ORDER(inTile) == XI_ID16WH)))
  {
    return(XI_ERR_BADARG);
  }

  if (!((XI_TILE4D_GET_DATA_ORDER(coeffTile) == XI_WHDN) || \
        (XI_TILE4D_GET_DATA_ORDER(coeffTile) == XI_NDWH) || \
        (XI_TILE4D_GET_DATA_ORDER(coeffTile) == XI_DWHN)))
  {
    return(XI_ERR_BADARG);
  }

  /* Getting parameters from the tile structures */
  /* Assigning the pitch values to the common pitch variables
   * based on the Data Order of the tile */

  /******** Input Tile Parameters ********/
  int32_t inP1, inP2, inP3, inP4;

  int32_t inDataWidth;
  int32_t inDataHeight;
  int32_t inChanNum;
  int32_t inDataPitch1;
  int32_t inDataPitch2;

  if (XI_TILE3D_GET_DATA_ORDER(inTile) == XI_WHD)
  {
    inDataWidth  = XI_TILE3D_GET_DIM1(inTile);
    inDataHeight = XI_TILE3D_GET_DIM2(inTile);
    inChanNum    = XI_TILE3D_GET_DIM3(inTile);
    inDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(inTile);
    inDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(inTile);

    inP1 = 1;
    inP2 = inDataPitch1;
    inP3 = inDataPitch2;
    inP4 = inDataPitch2;
  }
  else if(XI_TILE3D_GET_DATA_ORDER(inTile) == XI_DWH)
  {
    inDataWidth  = XI_TILE3D_GET_DIM2(inTile);
    inDataHeight = XI_TILE3D_GET_DIM3(inTile);
    inChanNum    = XI_TILE3D_GET_DIM1(inTile);
    inDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(inTile);
    inDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(inTile);

    inP1 = inDataPitch1;
    inP2 = inDataPitch2;
    inP3 = 1;
    inP4 = 1;
  }
  else /* inTile data order : XI_ID16WH */
  {
    inDataWidth  = XI_TILE3D_GET_DIM1(inTile) >> 4;
    inDataHeight = XI_TILE3D_GET_DIM3(inTile);
    inChanNum    = XI_TILE3D_GET_DIM2(inTile) << 4;
    inDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(inTile);
    inDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(inTile);

    inP1 = 16;
    inP2 = inDataPitch2;
    inP3 = 1;
    inP4 = inDataPitch1 >> 4;
  }
  /******** Input Tile Parameters End ********/

  /******** Output Tile Parameters **********/
  int32_t outP1, outP2, outP3, outP4;

  int32_t outDataWidth;
  int32_t outDataHeight;
  int32_t outChanNum;
  int32_t outDataPitch1;
  int32_t outDataPitch2;

  if (XI_TILE3D_GET_DATA_ORDER(outTile) == XI_WHD)
  {
    outDataWidth  = XI_TILE3D_GET_DIM1(outTile);
    outDataHeight = XI_TILE3D_GET_DIM2(outTile);
    outChanNum    = XI_TILE3D_GET_DIM3(outTile);
    outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
    outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);

    outP1 = 1;
    outP2 = outDataPitch1;
    outP3 = outDataPitch2;
    outP4 = outDataPitch2;
  }
  else if (XI_TILE3D_GET_DATA_ORDER(outTile) == XI_DWH)
  {
    outDataWidth  = XI_TILE3D_GET_DIM2(outTile);
    outDataHeight = XI_TILE3D_GET_DIM3(outTile);
    outChanNum    = XI_TILE3D_GET_DIM1(outTile);
    outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
    outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);

    outP1 = outDataPitch1;
    outP2 = outDataPitch2;
    outP3 = 1;
    outP4 = 1;
  }
  else /* outTile data order :  XI_ID16WH */
  {
    outDataWidth = XI_TILE3D_GET_DIM1(outTile) >> 4;
    outDataHeight = XI_TILE3D_GET_DIM3(outTile);
    outChanNum = XI_TILE3D_GET_DIM2(outTile) << 4;
    outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
    outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);

    outP1 = 16;
    outP2 = outDataPitch2;
    outP3 = 1;
    outP4 = outDataPitch1 >> 4;
  }

  /******** Output Tile Parameters End **********/

  /******** Coeff Tile Parameters **********/
  int32_t coeffP1, coeffP2, coeffP3, coeffP4;
  int32_t kWidth, kHeight, numDepthPerKernel, numKernels;

  int32_t coeffDataPitch1 = XI_TILE4D_GET_DIM1_PITCH(coeffTile);
  int32_t coeffDataPitch2 = XI_TILE4D_GET_DIM2_PITCH(coeffTile);
  int32_t coeffDataPitch3 = XI_TILE4D_GET_DIM3_PITCH(coeffTile);

  if (XI_TILE4D_GET_DATA_ORDER(coeffTile) == XI_WHDN)
  {
    kWidth  = XI_TILE4D_GET_DIM1(coeffTile);
    kHeight = XI_TILE4D_GET_DIM2(coeffTile);
    numDepthPerKernel = XI_TILE4D_GET_DIM3(coeffTile);
    numKernels = XI_TILE4D_GET_DIM4(coeffTile);

    coeffP1 = 1;
    coeffP2 = coeffDataPitch1;
    coeffP3 = coeffDataPitch2;
    coeffP4 = coeffDataPitch3;
  }
  else if (XI_TILE4D_GET_DATA_ORDER(coeffTile) == XI_NDWH)
  {
    kWidth  = XI_TILE4D_GET_DIM3(coeffTile);
    kHeight = XI_TILE4D_GET_DIM4(coeffTile);
    numDepthPerKernel = XI_TILE4D_GET_DIM2(coeffTile);
    numKernels = XI_TILE4D_GET_DIM1(coeffTile);

    coeffP1 = coeffDataPitch2;
    coeffP2 = coeffDataPitch3;
    coeffP3 = coeffDataPitch1;
    coeffP4 = 1;
  }
  else
  {
    kWidth  = XI_TILE4D_GET_DIM2(coeffTile);
    kHeight = XI_TILE4D_GET_DIM3(coeffTile);
    numDepthPerKernel = XI_TILE4D_GET_DIM1(coeffTile);
    numKernels = XI_TILE4D_GET_DIM4(coeffTile);

    coeffP1 = coeffDataPitch1;
    coeffP2 = coeffDataPitch2;
    coeffP3 = 1;
    coeffP4 = coeffDataPitch3;
  }
  /******** Coeff Tile Parameters End **********/

  /* Number of Groups should be greater than or equal to 1
   * and less than or equal to number of Input channels and
   * the Number of Input / Output Channels should be a
   * multiple of number of groups.
   */
  if (!((numGroups >= 1) && (numGroups <= inChanNum)))
  {
    return(XI_ERR_BADARG);
  }
  if (!((inChanNum % numGroups) == 0))
  {
    return(XI_ERR_BADARG);
  }
  if (!((outChanNum % numGroups) == 0))
  {
    return(XI_ERR_BADARG);
  }
  /* Number of groups must divide number of kernels */
  if (!((numKernels % numGroups) == 0))
  {
    return(XI_ERR_BADARG);
  }

  int32_t inTileChanNumPerGroup = inChanNum / numGroups;
  int32_t outTileChanNumPerGroup = outChanNum / numGroups;

  /* Checks inChannel and outChannel per group are multiple of 16 for ID16WH data order */
  /* Check the consistency of number of depths between input, coefficient and output tiles */
// TODO use from common headers
#define XI_ALIGN_VAL(val, pow2) (((val) + ((pow2) - 1)) & ~((pow2) - 1))
#define ALIGN(val, align)  XI_ALIGN_VAL(val, align)

  if (XI_TILE3D_GET_DATA_ORDER(inTile) == XI_ID16WH)
  {
    if (((inTileChanNumPerGroup % 16) != 0) || (inTileChanNumPerGroup != ALIGN(numDepthPerKernel, 16)))
    {
      return(XI_ERR_BADARG);
    }
    if ((XI_TILE3D_GET_DIM1(inTile) % 16) != 0)
    {
      return(XI_ERR_DATASIZE);
    }
    if ((XI_TILE3D_GET_DIM1_PITCH(inTile) % 16) != 0)
    {
      return(XI_ERR_DATASIZE);
    }
    if ((XI_TILE3D_GET_DIM1_EDGE1(inTile) % 16) != 0)
    {
      return(XI_ERR_EDGE);
    }
    if ((XI_TILE3D_GET_DIM1_EDGE2(inTile) % 16) != 0)
    {
      return(XI_ERR_EDGE);
    }
  }
  else
  {
    if (inTileChanNumPerGroup != numDepthPerKernel)
    {
      return(XI_ERR_DATASIZE);
    }
  }

  if (XI_TILE3D_GET_DATA_ORDER(outTile) == XI_ID16WH)
  {
    if (((outTileChanNumPerGroup % 16) != 0) || (outTileChanNumPerGroup != ALIGN(numKernels/numGroups, 16)))
    {
      return(XI_ERR_BADARG);
    }
    if ((XI_TILE3D_GET_DIM1(outTile) % 16) != 0)
    {
      return(XI_ERR_DATASIZE);
    }
    if ((XI_TILE3D_GET_DIM1_PITCH(outTile) % 16) != 0)
    {
      return(XI_ERR_DATASIZE);
    }
    if ((XI_TILE3D_GET_DIM1_EDGE1(outTile) % 16) != 0)
    {
      return(XI_ERR_EDGE);
    }
    if ((XI_TILE3D_GET_DIM1_EDGE2(outTile) % 16) != 0)
    {
      return(XI_ERR_EDGE);
    }
  }
  else
  {
    if (outChanNum != numKernels)
    {
      return(XI_ERR_DATASIZE);
    }
  }

  /* Check whether dilation = 1*/
  if (!(XI_CNNA_CONV_GET_DILATIONX(param) == 1 && XI_CNNA_CONV_GET_DILATIONY(param) == 1))
  {
    return(XI_ERR_BADARG);
  }
  if (!((edgeExtendLeft >= 0) && (edgeExtendRight >= 0) &&
        (edgeExtendTop >= 0) && (edgeExtendBottom >= 0)))
  {
    return(XI_ERR_EDGE);
  }

  /* Check whether the size of edgeExtendValArray= 1 or edgeExtendValArray= numinChanNumPerGroup,
  * if edgeExtendValArray is not a null pointer.
  */
  if (edgeExtendValArray)
  {
    if (!(XI_ARRAY_GET_WIDTH(edgeExtendValArray) == 1 || \
          XI_ARRAY_GET_WIDTH(edgeExtendValArray) == inChanNum))
    {
      return(XI_ERR_BADARG);
    }
    if (!(XI_ARRAY_GET_HEIGHT(edgeExtendValArray) > 0))
    {
      return(XI_ERR_BADARG);
    }
  }
  if (!(XI_ARRAY_GET_WIDTH(biasArray) >= numKernels))
  {
    return(XI_ERR_DATASIZE);
  }
  if (!(XI_ARRAY_GET_HEIGHT(biasArray) > 0))
  {
    return(XI_ERR_DATASIZE);
  }
  if (XI_CNNA_CONV_GET_FLAG_RELU(param))
  {
    if (!(XI_CNNA_CONV_GET_RELUMIN(param) <= XI_CNNA_CONV_GET_RELUMAX(param)))
    {
      return(XI_ERR_BADARG);
    }
    if (!((XI_CNNA_CONV_GET_RELUMIN(param) >= 0) && (XI_CNNA_CONV_GET_RELUMIN(param) <= UCHAR_MAX)))
    {
      return(XI_ERR_BADARG);
    }
    if (!((XI_CNNA_CONV_GET_RELUMAX(param) >= 0) && (XI_CNNA_CONV_GET_RELUMAX(param) <= UCHAR_MAX)))
    {
      return(XI_ERR_BADARG);
    }
  }
  if (!((XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param) >= 0) && (XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param) <= UCHAR_MAX)))
  {
    return(XI_ERR_BADARG);
  }
  if (!((XI_CNNA_CONV_GET_ZEROPT_INPUT(param) >= 0) && (XI_CNNA_CONV_GET_ZEROPT_INPUT(param) <= UCHAR_MAX)))
  {
    return(XI_ERR_BADARG);
  }

  /* The loops should run across the valid number of depths.
     Hence initialize them to values from coefficient tile  */
  int32_t inChanNumPerGroup = numDepthPerKernel;
  int32_t outChanNumPerGroup = numKernels / numGroups;

  /* Convolution params */
  const int32_t quantScale    = XI_CNNA_CONV_GET_QUANT_SCALE(param);
  const int32_t outShift      = XI_CNNA_CONV_GET_OUTPUT_SHIFT(param);
  const uint8_t enableReLu    = XI_CNNA_CONV_GET_FLAG_RELU(param);
  const uint8_t stride        = XI_CNNA_CONV_GET_STRIDE(param);
  const uint8_t zeroPtInputU  = (uint8_t)XI_CNNA_CONV_GET_ZEROPT_INPUT(param);
  const uint8_t zeroPtCoeffU  = XI_CNNA_CONV_GET_ZEROPT_COEFF(param);
  const uint8_t zeroPtOutputU = (uint8_t)XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param);
  const uint8_t reluMinU      = (uint8_t)XI_CNNA_CONV_GET_RELUMIN(param);
  const uint8_t reluMaxU      = (uint8_t)XI_CNNA_CONV_GET_RELUMAX(param);
  const uint8_t dilation      = XI_CNNA_CONV_GET_DILATION(param);

  /* Data Pointers of input, output, coefficient and bias data */
  uint8_t *pInput8b  = (uint8_t *) XI_TILE3D_GET_DATA_PTR(inTile);
  uint8_t *pOutput8b = (uint8_t *) XI_TILE3D_GET_DATA_PTR(outTile);
  uint8_t *pCoeff8b  = (uint8_t *) XI_TILE4D_GET_DATA_PTR(coeffTile);
  int32_t *pBias     = (int32_t *) XI_ARRAY_GET_DATA_PTR(biasArray);

  uint8_t *pEgdeVal8b = NULL;

  if (edgeExtendValArray)
  {
    pEgdeVal8b = (uint8_t *) XI_ARRAY_GET_DATA_PTR(edgeExtendValArray);
  }

  /* Negative output shift is equivalent to leftshift*/
  int32_t leftShift = outShift < 0 ? -outShift : 0;
  int32_t rightShift = outShift < 0 ? 0 : outShift;

  /* Setting the limits for output data according to ReLu is enabled or not*/
  const uint8_t minLimU = enableReLu ? reluMinU : 0;
  const uint8_t maxLimU = enableReLu ? reluMaxU : UCHAR_MAX;

  int32_t x, y, xk, yk, inCh, outCh, cIdx, dIdx, oIdx, n, inX, inY, kIn, kOut;
  int32_t isPosEdge;
  int32_t inpData;
  int32_t coeffData;
  int32_t egdeVal;
  int64_t acc64b;

  for (n = 0; n < numGroups; n++)                             /*  num of groups  */
  {
    for (outCh = 0; outCh < outChanNumPerGroup; outCh += 16)  /* output channels */
    {
      for (y = 0; y < outDataHeight; y++)             /* along the image height  */
      {
        for (x = 0; x < outDataWidth; x++)            /*  along the image width  */
        {
          int32_t remainingOutChIter = ((outChanNumPerGroup - outCh) > 16 ? 16 : (outChanNumPerGroup - outCh));
          for (kOut = 0; kOut < remainingOutChIter; kOut++)
          {
            acc64b = 0;
            for (inCh = 0; inCh < inChanNumPerGroup; inCh += 16)  /* input channels */
            {
              int32_t remainingInChIter = ((inChanNumPerGroup - inCh) > 16 ? 16 : (inChanNumPerGroup - inCh));
              for (kIn = 0; kIn < remainingInChIter; kIn++)
              {
                /* Selection of edgeVal
                * 1. if edgeExtendValArray is null, edges will be assigned with a value
                *    equal to zero point offset of input.
                * 2. else if width(edgeExtendValArray) =1 edges of all the WH planes
                *    will be assigned the value stored in edgeExtendValArray.
                * 3. else WH plane in each inchannel will be assigned with the value
                *    in index = inCh of edgeExtendValArray.
                */
                if (!edgeExtendValArray)
                {
                  egdeVal = zeroPtInputU;
                }
                else
                {
                  if (XI_ARRAY_GET_WIDTH(edgeExtendValArray) == 1)
                  {
                    egdeVal = (uint8_t)pEgdeVal8b[0];
                  }
                  else
                  {
                    egdeVal = (uint8_t)pEgdeVal8b[inCh + kIn + n*inChanNumPerGroup];
                  }
                }
                for (yk = 0; yk < kHeight; yk++)          /* along the kernel height */
                {
                  for (xk = 0; xk < kWidth; xk++)         /* along the kernel width  */
                  {
                    inX = x * stride + xk * dilation - edgeExtendLeft;
                    inY = y * stride + yk * dilation - edgeExtendTop;
                    cIdx = ((outCh + kOut + n * outChanNumPerGroup) * coeffP4) + ((inCh + kIn) * coeffP3) + \
                            (yk * coeffP2) + (xk * coeffP1);
                    dIdx = ((kIn)* inP3 + (inCh + n * inTileChanNumPerGroup) * inP4) + (inY * inP2) + \
                            (inX * inP1);

                    /*Checks if the input index is part of edge.*/
                    isPosEdge = (inY < 0) || (inY >= inDataHeight) || \
                                (inX < 0) || (inX >= inDataWidth);

                    /* Value of edge is decided by the edgeExtendValArray */
                    inpData = isPosEdge ? egdeVal : (uint8_t)pInput8b[dIdx];

                    coeffData = (uint8_t)pCoeff8b[cIdx];

                    acc64b += (((int64_t)(coeffData - zeroPtCoeffU)) * (inpData - zeroPtInputU));
                  } /* for (xk = 0; xk < kWidth; xk++) */
                }   /* for (yk = 0; yk < kHeight; yk++) */
              } /* for (kIn = 0; kIn < remainingInChIter; kIn++) */
            }  /* for (inCh = 0; inCh < inChanNumPerGroup; inCh += 16)*/
            /* Adding Bias to the sum */
            acc64b += pBias[outCh + kOut + n * outChanNumPerGroup];
            int32_t acc32b = (int32_t)CLAMP(acc64b, INT32_MIN, INT32_MAX);

            /* Accumulated value is multiplied with inputScale*coeffScale) /outputScale and
            * zeroPtOutputU is added to the result to get quantized output value.
            *
            * (inputScale*coeffScale) /outputScale = fractional part * 2^(outputShift)
            * quantized scale = fractional part in q31 format.
            *
            * Quantized scale and outputShift are passed to function as input.
            */

            acc64b = ROUND64B_ASYMM((((int64_t)acc32b) * quantScale), 31 - leftShift);
            acc32b = (int32_t)(MORPH_ROUND_N_CLAMP64B(acc64b, rightShift, SHRT_MIN, SHRT_MAX));

            /* Adding zero point of output */
            acc32b += zeroPtOutputU;

            /* Saturating between min and max value */
            uint8_t result = (uint8_t)(CLAMP(acc32b, minLimU, maxLimU));

            oIdx = kOut * outP3 + (outCh + n * outTileChanNumPerGroup) * outP4 + y * outP2 + x * outP1;
            pOutput8b[oIdx] = result;
          } /* for (kOut = 0; kOut < remainingOutChIter; kOut++) */

          /* If the output depth is not multiple of 16, set remaining output depths to zero in case of ID16WH */
          if (XI_TILE3D_GET_DATA_ORDER(outTile) == XI_ID16WH)
          {
            for (kOut = remainingOutChIter; kOut < 16; kOut++)
            {
              oIdx = kOut * outP3 + (outCh + n * outTileChanNumPerGroup) * outP4 + y * outP2 + x * outP1;
              pOutput8b[oIdx] = 0;
            } /*for (kOut = remainingOutChIter; kOut < 16; kOut++) */
          }   /* if (XI_TILE3D_GET_DATA_ORDER(outTile) == XI_ID16WH) */
        }     /* for (x = 0; x < outDataWidth; x++) */
      }       /* for (y = 0; y < outDataHeight; y++) */
    }         /* for (outCh = 0; outCh < outChanNumPerGroup; outCh += 16) */
  }           /* for (n = 0; n < numGroups; n++) */
  return(XI_ERR_OK);
}

void flk_conv_ref(const uint8_t* raw_params,
    struct XtensaOperationArgsIn* input,
    struct XtensaOperationArgsOut* output)
{
    XI_ERROR_CHECKS()
    conv_params_t* params = (conv_params_t*)raw_params;
    xi_cnna_conv_params xiparams;

    xi_tile3D inTile;
    xi_tile4D coeffTile;
    xi_array biasArray;
    xi_tile3D outTile;
    //xi_array outScaleArray;
    //xi_array outShiftArray;

    memset(&xiparams, 0, sizeof(xiparams));
    memset(&inTile, 0, sizeof(inTile));
    memset(&coeffTile, 0, sizeof(coeffTile));
    memset(&biasArray, 0, sizeof(biasArray));
    memset(&outTile, 0, sizeof(outTile));
#if 0
    memset(&outScaleArray, 0, sizeof(outScaleArray));
    memset(&outShiftArray, 0, sizeof(outShiftArray));
#endif
    uint8_t *inputPtr  = (uint8_t *) input->args[0];
    uint8_t *coeffPtr  = (uint8_t *) input->args[1];
    uint8_t *biasPtr   = (uint8_t *) input->args[2];
    uint8_t *outputPtr = (uint8_t *) output->args[0];

    XI_CNNA_CONV_SET_STRIDE(&xiparams, params->stride);
    XI_CNNA_CONV_SET_DILATION(&xiparams, 1);
    XI_CNNA_CONV_SET_ZEROPT_INPUT(&xiparams, params->zeroPtInput);
    XI_CNNA_CONV_SET_ZEROPT_COEFF(&xiparams, params->zeroPtCoeff);
    XI_CNNA_CONV_SET_ZEROPT_OUTPUT(&xiparams, params->zeroPtOutput);
    XI_CNNA_CONV_SET_QUANT_SCALE(&xiparams, params->quantizedScale);
    XI_CNNA_CONV_SET_OUTPUT_SHIFT(&xiparams, params->outputShift);
    XI_CNNA_CONV_SET_FLAGS(&xiparams, 0);
    XI_CNNA_CONV_SET_RELUMIN(&xiparams, params->reluMin);
    XI_CNNA_CONV_SET_RELUMAX(&xiparams, params->reluMax);

    if (CONV_FLAG_GET_RELU(params->flags))
        XI_CNNA_CONV_SET_FLAG_RELU(&xiparams);
    if (params->kernelW % 2 == 0)
        XI_CNNA_CONV_SET_FLAG_LEFTEDGE(&xiparams);
    if (params->kernelH % 2 == 0)
        XI_CNNA_CONV_SET_FLAG_TOPEDGE(&xiparams);

    XI_TILE4D_SET_BUFF_PTR(&coeffTile, coeffPtr);
    XI_TILE4D_SET_BUFF_SIZE(&coeffTile, params->output.D * params->input.D * params->kernelW * params->kernelH);
    XI_TILE4D_SET_DATA_PTR(&coeffTile, coeffPtr);
    XI_TILE4D_SET_DATA_ORDER(&coeffTile, XI_DWHN);
    XI_TILE4D_SET_TYPE(&coeffTile, params->tileType.dataType);
    //XI_TILE4D_SET_TYPE(&coeffTile, XI_TILE4D_S8);
    XI_TILE4D_SET_DIM1_PITCH(&coeffTile, params->input.D);
    XI_TILE4D_SET_DIM2_PITCH(&coeffTile, params->input.D * params->kernelW);
    XI_TILE4D_SET_DIM3_PITCH(&coeffTile, params->input.D * params->kernelW * params->kernelH);
    XI_TILE4D_SET_DIM1(&coeffTile, params->input.D);
    XI_TILE4D_SET_DIM2(&coeffTile, params->kernelW);
    XI_TILE4D_SET_DIM3(&coeffTile, params->kernelH);
    XI_TILE4D_SET_DIM4(&coeffTile, params->output.D);

    XI_ARRAY_SET_BUFF_PTR(&biasArray, biasPtr);
    XI_ARRAY_SET_BUFF_SIZE(&biasArray, params->output.D * sizeof(int32_t));
    XI_ARRAY_SET_DATA_PTR(&biasArray, biasPtr);
    XI_ARRAY_SET_WIDTH(&biasArray, params->output.D);
    XI_ARRAY_SET_HEIGHT(&biasArray, 1);
    XI_ARRAY_SET_TYPE(&biasArray, XI_ARRAY_S32);
    XI_ARRAY_SET_CAPACITY(&biasArray, params->output.D);

    XI_TILE3D_SET_BUFF_SIZE(&inTile, params->input.D * params->input.W * params->input.H);
    XI_TILE3D_SET_DATA_ORDER(&inTile, XI_DWH);
    XI_TILE3D_SET_TYPE(&inTile, params->tileType.dataType);
    //XI_TILE3D_SET_TYPE(&inTile, XI_TILE3D_S8);
    XI_TILE3D_SET_DIM1_PITCH(&inTile, params->input.D);
    XI_TILE3D_SET_DIM2_PITCH(&inTile, params->input.D * params->input.W);
    XI_TILE3D_SET_DIM1(&inTile, params->input.D);
    XI_TILE3D_SET_DIM2(&inTile, params->input.W);
    XI_TILE3D_SET_DIM3(&inTile, params->input.H);

    XI_TILE3D_SET_BUFF_SIZE(&outTile, params->output.D * params->output.W * params->output.H);
    XI_TILE3D_SET_DATA_ORDER(&outTile, XI_DWH);
    XI_TILE3D_SET_TYPE(&outTile, params->tileType.dataType);
    //XI_TILE3D_SET_TYPE(&outTile, XI_TILE3D_S8);
    XI_TILE3D_SET_DIM1_PITCH(&outTile, params->output.D);
    XI_TILE3D_SET_DIM2_PITCH(&outTile, params->output.D * params->output.W);
    XI_TILE3D_SET_DIM1(&outTile, params->output.D);
    XI_TILE3D_SET_DIM2(&outTile, params->output.W);
    XI_TILE3D_SET_DIM3(&outTile, params->output.H);

#if 0
    XI_ARRAY_SET_BUFF_PTR(&outScaleArray, &mOutScaleArray[0]);
    XI_ARRAY_SET_BUFF_SIZE(&outScaleArray, params->output.D * sizeof(int32_t));
    XI_ARRAY_SET_DATA_PTR(&outScaleArray, &mOutScaleArray[0]);
    XI_ARRAY_SET_WIDTH(&outScaleArray, params->output.D);
    XI_ARRAY_SET_PITCH(&outScaleArray, params->output.D);
    XI_ARRAY_SET_HEIGHT(&outScaleArray, 1);
    XI_ARRAY_SET_TYPE(&outScaleArray, XI_ARRAY_S32);
    XI_ARRAY_SET_CAPACITY(&outScaleArray, params->output.D);

    //print_array_S32_ref("outScaleArray", &outScaleArray);

    XI_ARRAY_SET_BUFF_PTR(&outShiftArray, &mOutShiftArray[0]);
    XI_ARRAY_SET_BUFF_SIZE(&outShiftArray, params->output.D);
    XI_ARRAY_SET_DATA_PTR(&outShiftArray, &mOutShiftArray[0]);
    XI_ARRAY_SET_WIDTH(&outShiftArray, params->output.D);
    XI_ARRAY_SET_PITCH(&outShiftArray, params->output.D);
    XI_ARRAY_SET_HEIGHT(&outShiftArray, 1);
    XI_ARRAY_SET_TYPE(&outShiftArray, XI_ARRAY_S8);
    XI_ARRAY_SET_CAPACITY(&outShiftArray, params->output.D);
#endif

    for (unsigned int i=0; i<params->batch; i++) {
        XI_TILE3D_SET_BUFF_PTR(&inTile, &inputPtr[i * XI_TILE3D_GET_BUFF_SIZE(&inTile)]);
        XI_TILE3D_SET_DATA_PTR(&inTile, &inputPtr[i * XI_TILE3D_GET_BUFF_SIZE(&inTile)]);
        XI_TILE3D_SET_BUFF_PTR(&outTile, &outputPtr[i * XI_TILE3D_GET_BUFF_SIZE(&outTile)]);
        XI_TILE3D_SET_DATA_PTR(&outTile, &outputPtr[i * XI_TILE3D_GET_BUFF_SIZE(&outTile)]);
        if (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkVQ_QM24
            || CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkVQ_QM32) {
            printf("kernel type: kkVQ_QM32 / kkVQ_QM32 NOT supported...\n");
            exit(0);
#if 0
            ASSERT_EQ(xiConvolveAVQ3D_S_MxN_I8S8I8_reff(&inTile,
                                                        &coeffTile,
                                                        &biasArray,
                                                        &outScaleArray,
                                                        &outShiftArray,
                                                        &outTile,
                                                        &xiparams,
                                                        /*numGroups*/ 1,
                                                        /*edgeExtendLeft*/ params->kernelW / 2 - params->offsetX,
                                                        /*edgeExtendRight*/ params->kernelW / 2 - params->offsetX,
                                                        /*edgeExtendTop*/ params->kernelH / 2 - params->offsetY,
                                                        /*edgeExtendBottom*/ params->kernelH / 2 - params->offsetY,
                                                        /*edgeExtendValArray*/ NULL),
                        XI_ERR_OK);
#endif
        }
        else {
            xiConvolveA3D_S_MxN_U8_reff(&inTile,
                                                  &coeffTile,
                                                  &biasArray,
                                                  &outTile,
                                                  &xiparams,
                                                  /*numGroups*/ 1,
                                                  /*edgeExtendLeft*/ params->kernelW / 2 - params->offsetX,
                                                  /*edgeExtendRight*/ params->kernelW / 2 - params->offsetX,
                                                  /*edgeExtendTop*/ params->kernelH / 2 - params->offsetY,
                                                  /*edgeExtendBottom*/ params->kernelH / 2 - params->offsetY,
                                                  /*edgeExtendValArray*/ NULL);
        }
    }
    return ;
}
