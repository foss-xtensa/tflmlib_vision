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
#define INCLUDE_XI_CNN
#include "xi_cnn.h"
#include "xi_intrin.h"
#include "xi_cnn_api.h"

#define MAX(a, b)  ((b) > (a) ? (b) : (a))
#define min(a, b)  ((a) > (b) ? (b) : (a))
#define MUL_PACK_SCALE_SHIFT_ADD_ZPT_CLAMP_LIMITS(vecOut, vecIn, slope, m1, nm1, neg_output_multiplier,    \
                                                 pos_output_multiplier, neg_output_shift, pos_output_shift,\
                                                 output_zero_point, minVal, maxVal)                        \
{                                                                                                          \
  xb_vecNx16 vecMul_0;                                                                                     \
  xb_vecN_2x32Uv hvecMul_1L, hvecMul_1H;                                                                   \
  vboolN vbN;                                                                                              \
  vboolN_2 vbN_2_L, vbN_2_H;                                                                               \
  vbN = IVP_LTNX16(vecIn, (xb_vecNx16) 0);                                                                 \
  IVP_EXTRACTBN(vbN_2_H, vbN_2_L, vbN);                                                                    \
  xb_vecNx16  vecTemp;                                                                                     \
  vecTemp = IVP_SLLNX16(slope, (xb_vecNx16)nm1);                                                           \
  vecMul_0  = IVP_MOVNX16T(vecTemp, m1, vbN);                                                              \
  hvecMul_1L = IVP_MOVN_2X32UT(neg_output_multiplier, pos_output_multiplier, vbN_2_L);                     \
  hvecMul_1H = IVP_MOVN_2X32UT(neg_output_multiplier, pos_output_multiplier, vbN_2_H);                     \
                                                                                                           \
  xb_vecNx48 acc = IVP_MULNX16(vecIn, vecMul_0);                                                           \
                                                                                                           \
  xb_vecN_2x32v hvecOutL, hvecOutH, hvecOutLB, hvecOutHB;                                                  \
  hvecOutL = IVP_CVT32SNX48L(acc);                                                                         \
  hvecOutH = IVP_CVT32SNX48H(acc);                                                                         \
                                                                                                           \
  xb_vecN_2x64w haccScaledL, haccScaledH;                                                                  \
  haccScaledL = IVP_MULUUN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecOutL), hvecMul_1L);                        \
  IVP_MULSUAHN_2X16X32_1(haccScaledL, IVP_MOVNX16_FROMN_2X32(hvecOutL), hvecMul_1L);                       \
  haccScaledH = IVP_MULUUN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecOutH), hvecMul_1H);                        \
  IVP_MULSUAHN_2X16X32_1(haccScaledH, IVP_MOVNX16_FROMN_2X32(hvecOutH), hvecMul_1H);                       \
                                                                                                           \
  hvecOutL = IVP_PACKVRN_2X64W(haccScaledL, 31);                                                           \
  hvecOutH = IVP_PACKVRN_2X64W(haccScaledH, 31);                                                           \
                                                                                                           \
  hvecOutLB = hvecOutL;                                                                                    \
  hvecOutHB = hvecOutH;                                                                                    \
                                                                                                           \
  xb_vecNx16 vecOutA, vecOutB, vecOutC;                                                                    \
  /*Pack the result to 16 bits and  saturate to short min and max*/                                        \
  MORPH_IVP_PACKVRNX48(vecOutA, hvecOutH, hvecOutL, pos_output_shift);                                     \
  MORPH_IVP_PACKVRNX48(vecOutB, hvecOutHB, hvecOutLB, neg_output_shift);                                   \
  vecOutC = IVP_MOVNX16T(vecOutB, vecOutA, vbN);                                                           \
  vecOutC = IVP_ADDSNX16(vecOutC, output_zero_point);                                                      \
  vecOut  = IVP_MAXNX16(IVP_MINNX16(vecOutC, (xb_vecNx16) maxVal), (xb_vecNx16) minVal);                   \
}


/***************************** xiPreluA3D_U8_DWH_ref ******************************************/
/* Description : VP6 implementation of parameterized RELU which retains                       */
/*               positive values of input and applies a slope to                              */
/*               input values which are less than 0.                                          */
/*               If slope is equal to 0 given function behaves as basic RELU                  */
/*               If dimension of slope is 1x1x1 the function will behave as leakyRelu         */
/* Inputs      : Input tile, slope applied to negative values of input                        */
/* Outputs     : XI Error Code                                                                */
/* InOuts      : Output Tile                                                                  */
/* Assumptions : InData, outData, slope is unsigned 8bit                                      */
/*               min-max values are unsigned 8bit                                             */
/**********************************************************************************************/
XI_ERR_TYPE xiPreluA3D_U8U8U8_DWH(const xi_pTile3D inTile,
                                  const xi_pTile3D slopeTile,
                                  xi_pTile3D outTile,
                                  const xi_cnn_preluA_params *params)
{
  /* Error Checks */
  XI_ERROR_CHECKS()
  {
    XI_CHECK_TILE3D_U8(inTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE3D_DATA_ORDER(inTile, XI_DWH);
    XI_CHECK_TILE3D_U8(outTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_TILE3D_DATA_ORDER(outTile, XI_DWH);
    XI_CHECK_TILE3D_U8(slopeTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(slopeTile);
    XI_CHECK_TILE3D_DATA_ORDER(slopeTile, XI_DWH);
    XI_CHECK_POINTER(params);
    XI_CHECK_ERROR(!((XI_TILE3D_GET_DIM1(inTile) != XI_TILE3D_GET_DIM1(slopeTile)) && ((XI_TILE3D_GET_DIM1(inTile) != 1) && (XI_TILE3D_GET_DIM1(slopeTile) != 1))), \
      XI_ERR_DATASIZE, "If the size of a first dimension in both input tiles are not equal, then one of them must be equal to 1");
    XI_CHECK_ERROR(!((XI_TILE3D_GET_DIM2(inTile) != XI_TILE3D_GET_DIM2(slopeTile)) && ((XI_TILE3D_GET_DIM2(inTile) != 1) && (XI_TILE3D_GET_DIM2(slopeTile) != 1))), \
      XI_ERR_DATASIZE, "If the size of a second dimension in both input tiles are not equal, then one of them must be equal to 1");
    XI_CHECK_ERROR(!((XI_TILE3D_GET_DIM3(inTile) != XI_TILE3D_GET_DIM3(slopeTile)) && ((XI_TILE3D_GET_DIM3(inTile) != 1) && (XI_TILE3D_GET_DIM3(slopeTile) != 1))), \
      XI_ERR_DATASIZE, "If the sizes of a third dimension in both input tiles are not equal, then one of them must be equal to 1");
    XI_CHECK_ERROR(!(MAX(XI_TILE3D_GET_DIM1(inTile), XI_TILE3D_GET_DIM1(slopeTile)) != XI_TILE3D_GET_DIM1(outTile)), \
      XI_ERR_DATASIZE, "The size of the first dimension in output tile must be equal to the maximum of sizes of first dimension in both input tiles");
    XI_CHECK_ERROR(!(MAX(XI_TILE3D_GET_DIM2(inTile), XI_TILE3D_GET_DIM2(slopeTile)) != XI_TILE3D_GET_DIM2(outTile)), \
      XI_ERR_DATASIZE, "The size of the second dimension in output tile must be equal to the maximum of sizes of second dimension in both input tiles");
    XI_CHECK_ERROR(!(MAX(XI_TILE3D_GET_DIM3(inTile), XI_TILE3D_GET_DIM3(slopeTile)) != XI_TILE3D_GET_DIM3(outTile)), \
      XI_ERR_DATASIZE, "The size of the third dimension in output tile must be equal to the maximum of sizes of third dimension in both input tiles");
    XI_CHECK_ERROR((!((XI_CNN_PRELUA_PARAMS_GET_IN_ZERO_POINT(params) < 0) || (XI_CNN_PRELUA_PARAMS_GET_IN_ZERO_POINT(params) > 255))), \
      XI_ERR_BADARG, "The input zero point value is invalid");
    XI_CHECK_ERROR((!((XI_CNN_PRELUA_PARAMS_GET_OUT_ZERO_POINT(params) < 0) || (XI_CNN_PRELUA_PARAMS_GET_OUT_ZERO_POINT(params) > 255))), \
      XI_ERR_BADARG, "The output zero point value is invalid");
    XI_CHECK_ERROR((!((XI_CNN_PRELUA_PARAMS_GET_ALPHA_ZERO_POINT(params) < 0) || (XI_CNN_PRELUA_PARAMS_GET_ALPHA_ZERO_POINT(params) > 255))), \
      XI_ERR_BADARG, "The alpha zero point value is invalid");
    XI_CHECK_ERROR(!((XI_CNN_PRELUA_PARAMS_GET_MIN_VAL(params) > XI_CNN_PRELUA_PARAMS_GET_MAX_VAL(params))), \
      XI_ERR_BADARG, "The min val is greater than max val");
    XI_CHECK_ERROR(((XI_CNN_PRELUA_PARAMS_GET_NEGATIVE_OUTPUT_MULTIPLIER(params) >= 0) && (XI_CNN_PRELUA_PARAMS_GET_POSITIVE_OUTPUT_MULTIPLIER(params) >= 0)), \
      XI_ERR_BADARG, "The output multipliers values are invalid");
    XI_CHECK_ERROR(((XI_CNN_PRELUA_PARAMS_GET_POSITIVE_OUTPUT_SHIFT(params) < 32) && (XI_CNN_PRELUA_PARAMS_GET_POSITIVE_OUTPUT_SHIFT(params) > -32) && (XI_CNN_PRELUA_PARAMS_GET_NEGATIVE_OUTPUT_SHIFT(params) <= 31)), \
      XI_ERR_BADARG, "The at least one of the output shift values is invalid");
  }

  /* Getting parameters from the tile structures */
  const int32_t outDepth = XI_TILE3D_GET_DIM1(outTile);
  const int32_t outWidth = XI_TILE3D_GET_DIM2(outTile);
  const int32_t outHeight = XI_TILE3D_GET_DIM3(outTile);
  int32_t inDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(inTile);
  int32_t inDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(inTile);
  int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
  int32_t slopeDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(slopeTile);
  int32_t slopeDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(slopeTile);

  uint8_t *pInput = (uint8_t *)XI_TILE3D_GET_DATA_PTR(inTile);
  uint8_t *pOutput = (uint8_t *)XI_TILE3D_GET_DATA_PTR(outTile);
  uint8_t *pSlope = (uint8_t *)XI_TILE3D_GET_DATA_PTR(slopeTile);

  int32_t input_zero_point = XI_CNN_PRELUA_PARAMS_GET_IN_ZERO_POINT(params);
  int32_t alpha_zero_point = XI_CNN_PRELUA_PARAMS_GET_ALPHA_ZERO_POINT(params);
  int32_t output_zero_point = XI_CNN_PRELUA_PARAMS_GET_OUT_ZERO_POINT(params);
  int32_t pos_output_multiplier = XI_CNN_PRELUA_PARAMS_GET_POSITIVE_OUTPUT_MULTIPLIER(params);
  int32_t pos_output_shift = XI_CNN_PRELUA_PARAMS_GET_POSITIVE_OUTPUT_SHIFT(params);
  int32_t neg_output_multiplier = XI_CNN_PRELUA_PARAMS_GET_NEGATIVE_OUTPUT_MULTIPLIER(params);
  int32_t neg_output_shift = XI_CNN_PRELUA_PARAMS_GET_NEGATIVE_OUTPUT_SHIFT(params);
  int32_t minVal = XI_CNN_PRELUA_PARAMS_GET_MIN_VAL(params);
  int32_t maxVal = XI_CNN_PRELUA_PARAMS_GET_MAX_VAL(params);

  int32_t leakyReluCase = (XI_TILE3D_GET_DIM1(slopeTile) == 1) ? 1 : 0;

  int32_t d, w, h;

  int32_t pm1, nm1;
  int32_t ps_, ns_;

  // Merge equations of all three cases of positive multiplier and shift
  if (pos_output_multiplier == 0 && pos_output_shift == 0)
  {
    pm1 = 1;
    ps_ = 0;
  }
  else if (pos_output_shift <= 0)
  {
    pm1 = 1;
    ps_ = (-1) * pos_output_shift;
  }
  else
  {
    pm1 = (1 << pos_output_shift);
    ps_ = 0;
  }

  // Merge equations of all three cases of positive multiplier and shift
  if (neg_output_multiplier == 0 && neg_output_shift == 0)
  {
    nm1 = 0;
    ns_ = 0;
  }
  else if (neg_output_shift <= 0)
  {
    nm1 = 0;
    ns_ = (-1) * neg_output_shift;
  }
  else
  {
    nm1 = neg_output_shift;
    ns_ = 0;
  }

  if (XI_TILE3D_GET_DIM2(inTile) == 1)
  {
    inDataPitch1 = 0;
  }
  if (XI_TILE3D_GET_DIM3(inTile) == 1)
  {
    inDataPitch2 = 0;
  }

  if (XI_TILE3D_GET_DIM2(slopeTile) == 1)
  {
    slopeDataPitch1 = 0;
  }
  if (XI_TILE3D_GET_DIM3(slopeTile) == 1)
  {
    slopeDataPitch2 = 0;
  }

  xb_vecNx8U* restrict pvecIn = (xb_vecNx8U *)pInput;
  xb_vecNx8U* restrict pvecOut = (xb_vecNx8U *)pOutput;
  xb_vecNx8U* restrict pvecSlope = (xb_vecNx8U *)pSlope;

  int32_t numElem;
  xb_vecNx16 vecIn, vecOut, vecSlope;
  valign valSlope, valIn, valOut;

  /*
  The code is conditionally executed based on following conditions:
  1. LeakyReLU
     [1.a] slopeTile = {1, 1, 1} and input, output and slope tile edges are zero enabling contiguous load/store.
     [1.b] slopeTile = {1, dim2, dim3}, input and output may or may not have edges.

  2. ReLU
     [2.a] indim1 > 1, slopeDim1 > 1, dim2 and dim3 of input and slope >=1
     [2.b] indim1 ==1, slopeDim1 > 1, dim2 and dim3 of input and slope >=1
  */

  if (leakyReluCase)
  {
    /* Leaky ReLU case where the input, ouput and slope tiles do not have edges*/
    if ((outDepth == inDataPitch1) && (outDepth == outDataPitch1) && ((outWidth * outDepth) == inDataPitch2) && (inDataPitch2 == outDataPitch2))
    {
      if ((XI_TILE3D_GET_DIM1(slopeTile) == 1) && (XI_TILE3D_GET_DIM2(slopeTile) == 1) && (XI_TILE3D_GET_DIM3(slopeTile) == 1))
      {
        /*Estimate data count to process */
        int32_t total_count = outDepth * outWidth * outHeight;
        /*Prime the input*/
        valIn = IVP_LANX8U_PP(pvecIn);
        valOut = IVP_ZALIGN();
        /*Load slope value*/
        valSlope = IVP_LANX8U_PP(pvecSlope);
        IVP_LAVNX8U_XP(vecSlope, valSlope, pvecSlope, 1);
        vecSlope = IVP_REPNX16(vecSlope, 0);
        vecSlope = IVP_SUBNX16(vecSlope, alpha_zero_point);

        for (d = 0; d < total_count; d += XCHAL_IVPN_SIMD_WIDTH)
        {
          numElem = total_count - d;
          IVP_LAVNX8U_XP(vecIn, valIn, pvecIn, numElem);
          vecIn = IVP_SUBNX16(vecIn, input_zero_point);

          MUL_PACK_SCALE_SHIFT_ADD_ZPT_CLAMP_LIMITS(vecOut, vecIn, vecSlope, pm1, nm1, neg_output_multiplier, pos_output_multiplier, \
            ns_, ps_, output_zero_point, minVal, maxVal);

          IVP_SAVNX8U_XP(vecOut, valOut, pvecOut, numElem);
        }
        IVP_SAPOSNX8U_FP(valOut, pvecOut);
        return(XI_ERROR_STATUS());
      }
    }
    if ((XI_TILE3D_GET_DIM2(slopeTile) >= 1) && (XI_TILE3D_GET_DIM3(slopeTile) >= 1)) /* generalized leaky ReLU case*/
    {
      for (h = 0; h < outHeight; ++h)     // along the image height(DWH)
      {
        for (w = 0; w < outWidth; ++w) // along the image width(DWH)
        {
          /* Initialize input, output and slope pointers */
          pvecIn = (xb_vecNx8U *)(pInput + h * inDataPitch2 + w * inDataPitch1);
          pvecOut = (xb_vecNx8U *)(pOutput + h * outDataPitch2 + w * outDataPitch1);
          pvecSlope = (xb_vecNx8U *)(pSlope + h * slopeDataPitch2 + w * slopeDataPitch1);
          valIn = IVP_LANX8U_PP(pvecIn);
          valSlope = IVP_LANX8U_PP(pvecSlope);
          /* Load slope values prior to inner loop, as the slope data remains same across dim1 */
          IVP_LAVNX8U_XP(vecSlope, valSlope, pvecSlope, 1);
          vecSlope = IVP_REPNX16(vecSlope, 0);
          vecSlope = IVP_SUBNX16(vecSlope, alpha_zero_point);
          valOut = IVP_ZALIGN();
          for (d = 0; d < (outDepth - XCHAL_IVPN_SIMD_WIDTH); d += XCHAL_IVPN_SIMD_WIDTH)
          {
            IVP_LAVNX8U_XP(vecIn, valIn, pvecIn, outDepth-d);
            vecIn = IVP_SUBNX16(vecIn, input_zero_point);

            MUL_PACK_SCALE_SHIFT_ADD_ZPT_CLAMP_LIMITS(vecOut, vecIn, vecSlope, pm1, nm1, neg_output_multiplier, pos_output_multiplier, \
              ns_, ps_, output_zero_point, minVal, maxVal);

            IVP_SAVNX8U_XP(vecOut, valOut, pvecOut, outDepth-d);
          }
          IVP_SAPOSNX8U_FP(valOut, pvecOut);

          if (d < outDepth) /* Handle corner case*/
          {
            IVP_LAVNX8U_XP(vecIn, valIn, pvecIn, outDepth - d);
            vecIn = IVP_SUBNX16(vecIn, input_zero_point);
            MUL_PACK_SCALE_SHIFT_ADD_ZPT_CLAMP_LIMITS(vecOut, vecIn, vecSlope, pm1, nm1, neg_output_multiplier, pos_output_multiplier, \
              ns_, ps_, output_zero_point, minVal, maxVal);
            IVP_SAVNX8U_XP(vecOut, valOut, pvecOut, outDepth - d);
            IVP_SAPOSNX8U_FP(valOut, pvecOut);
          }
        }
      }
      return(XI_ERROR_STATUS());
    }
  }
  else /* pReLU case (non-leaky ReLU)*/
  {
    if (XI_TILE3D_GET_DIM1(inTile) > 1) /* inDim == slopeDim1 */
    {
      for (h = 0; h < outHeight; ++h)     // along the image height(DWH)
      {
        for (w = 0; w < outWidth; ++w)     // along the image width(DWH)
        {
          /* Initialize input, output and slope pointers */
          pvecIn = (xb_vecNx8U *)(pInput + h * inDataPitch2 + w * inDataPitch1);
          pvecOut = (xb_vecNx8U *)(pOutput + h * outDataPitch2 + w * outDataPitch1);
          pvecSlope = (xb_vecNx8U *)(pSlope + h * slopeDataPitch2 + w * slopeDataPitch1);

          valSlope = IVP_LANX8U_PP(pvecSlope);
          valIn = IVP_LANX8U_PP(pvecIn);
          valOut = IVP_ZALIGN();
          for (d = 0; d < outDepth - XCHAL_IVPN_SIMD_WIDTH; d += XCHAL_IVPN_SIMD_WIDTH)
          {
            IVP_LAVNX8U_XP(vecIn, valIn, pvecIn, outDepth-d);
            IVP_LAVNX8U_XP(vecSlope, valSlope, pvecSlope, outDepth-d);

            vecSlope = IVP_SUBNX16(vecSlope, alpha_zero_point);
            vecIn = IVP_SUBNX16(vecIn, input_zero_point);

            MUL_PACK_SCALE_SHIFT_ADD_ZPT_CLAMP_LIMITS(vecOut, vecIn, vecSlope, pm1, nm1, neg_output_multiplier, pos_output_multiplier, \
              ns_, ps_, output_zero_point, minVal, maxVal);

            IVP_SAVNX8U_XP(vecOut, valOut, pvecOut, outDepth-d);
          }
          IVP_SAPOSNX8U_FP(valOut, pvecOut);
          if (d < outDepth) /* Handle corner case across depth */
          {
            IVP_LAVNX8U_XP(vecIn, valIn, pvecIn, outDepth-d);
            IVP_LAVNX8U_XP(vecSlope, valSlope, pvecSlope, outDepth-d);

            vecSlope = IVP_SUBNX16(vecSlope, alpha_zero_point);
            vecIn = IVP_SUBNX16(vecIn, input_zero_point);

            MUL_PACK_SCALE_SHIFT_ADD_ZPT_CLAMP_LIMITS(vecOut, vecIn, vecSlope, pm1, nm1, neg_output_multiplier, pos_output_multiplier, \
              ns_, ps_, output_zero_point, minVal, maxVal);

            IVP_SAVNX8U_XP(vecOut, valOut, pvecOut, outDepth-d);
            IVP_SAPOSNX8U_FP(valOut, pvecOut);
          }
        }
      }
    }
    else /* (if XI_TILE3D_GET_DIM1(inTile) == 1) */
    {
      int32_t numElemIn = 1;
      for (h = 0; h < outHeight; ++h)     // along the image height(DWH)
      {
        for (w = 0; w < outWidth; ++w)     // along the image width(DWH)
        {
          /* Initialize input pointers */
          pvecIn = (xb_vecNx8U *)(pInput + h * inDataPitch2 + w * inDataPitch1);
          pvecOut = (xb_vecNx8U *)(pOutput + h * outDataPitch2 + w * outDataPitch1);
          pvecSlope = (xb_vecNx8U *)(pSlope + h * slopeDataPitch2 + w * slopeDataPitch1);
          valSlope = IVP_LANX8U_PP(pvecSlope);
          valIn = IVP_LANX8U_PP(pvecIn);

          /* Load input data outside the inner loop as the data remains same across dim1 */
          IVP_LAVNX8U_XP(vecIn, valIn, pvecIn, numElemIn);
          vecIn = IVP_REPNX16(vecIn, 0);
          vecIn = IVP_SUBNX16(vecIn, input_zero_point);
          valOut = IVP_ZALIGN();
          for (d = 0; d < outDepth - XCHAL_IVPN_SIMD_WIDTH; d += XCHAL_IVPN_SIMD_WIDTH)
          {
            IVP_LAVNX8U_XP(vecSlope, valSlope, pvecSlope, outDepth-d);

            vecSlope = IVP_SUBNX16(vecSlope, alpha_zero_point);

            MUL_PACK_SCALE_SHIFT_ADD_ZPT_CLAMP_LIMITS(vecOut, vecIn, vecSlope, pm1, nm1, neg_output_multiplier, pos_output_multiplier, \
              ns_, ps_, output_zero_point, minVal, maxVal);

            IVP_SAVNX8U_XP(vecOut, valOut, pvecOut, outDepth-d);
          }
          IVP_SAPOSNX8U_FP(valOut, pvecOut);
          if (d < outDepth) /* Handle corner cases */
          {
            IVP_LAVNX8U_XP(vecSlope, valSlope, pvecSlope, outDepth-d);

            vecSlope = IVP_SUBNX16(vecSlope, alpha_zero_point);

            MUL_PACK_SCALE_SHIFT_ADD_ZPT_CLAMP_LIMITS(vecOut, vecIn, vecSlope, pm1, nm1, neg_output_multiplier, pos_output_multiplier, \
              ns_, ps_, output_zero_point, minVal, maxVal);

            IVP_SAVNX8U_XP(vecOut, valOut, pvecOut, outDepth-d);
            IVP_SAPOSNX8U_FP(valOut, pvecOut);
          }
        }
      }
    }
  }
  return(XI_ERROR_STATUS());
}

#define ADD_FIXUP_BIAS_ZPT_PACK_SCALE_SHIFT_CLAMP_LIMITS(vec16H1W1CD, vec16H1W1AB, daccSum1,                           \
                                                         hvecFixUp1, hvecFixUp2, hvecFixUp3, hvecFixUp4,               \
                                                         hvecBias, zeroPtOutput, quantScale, leftShift,rightShift, minLim, maxLim) \
  {                                                                                                                    \
    xb_vecN_2x32v hvecOutH1W1A, hvecOutH1W1B, hvecOutH1W1C, hvecOutH1W1D;                                              \
    xb_vecN_2x64w dProd1, dProd2, dProd3, dProd4;                                                                      \
    hvecOutH1W1A = IVP_CVT32S2NX24LL(daccSum1);                                                                        \
    hvecOutH1W1B = IVP_CVT32S2NX24LH(daccSum1);                                                                        \
    hvecOutH1W1C = IVP_CVT32S2NX24HL(daccSum1);                                                                        \
    hvecOutH1W1D = IVP_CVT32S2NX24HH(daccSum1);                                                                        \
    hvecOutH1W1A = IVP_ADDN_2X32(hvecOutH1W1A, hvecFixUp1);                                                            \
    hvecOutH1W1B = IVP_ADDN_2X32(hvecOutH1W1B, hvecFixUp2);                                                            \
    hvecOutH1W1C = IVP_ADDN_2X32(hvecOutH1W1C, hvecFixUp3);                                                            \
    hvecOutH1W1D = IVP_ADDN_2X32(hvecOutH1W1D, hvecFixUp4);                                                            \
    hvecOutH1W1A = IVP_ADDN_2X32(hvecOutH1W1A, hvecBias);                                                              \
    hvecOutH1W1B = IVP_ADDN_2X32(hvecOutH1W1B, hvecBias);                                                              \
    hvecOutH1W1C = IVP_ADDN_2X32(hvecOutH1W1C, hvecBias);                                                              \
    hvecOutH1W1D = IVP_ADDN_2X32(hvecOutH1W1D, hvecBias);                                                              \
    dProd1       = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecOutH1W1A),                                          \
                                        quantScale);                                                                   \
    IVP_MULAHN_2X16X32_1(dProd1, IVP_MOVNX16_FROMN_2X32(hvecOutH1W1A), quantScale);                                    \
    dProd2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecOutH1W1B),                                                \
                                  quantScale);                                                                         \
    IVP_MULAHN_2X16X32_1(dProd2, IVP_MOVNX16_FROMN_2X32(hvecOutH1W1B), quantScale);                                    \
    dProd3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecOutH1W1C),                                                \
                                  quantScale);                                                                         \
    IVP_MULAHN_2X16X32_1(dProd3, IVP_MOVNX16_FROMN_2X32(hvecOutH1W1C), quantScale);                                    \
    dProd4 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecOutH1W1D),                                                \
                                  quantScale);                                                                         \
    IVP_MULAHN_2X16X32_1(dProd4, IVP_MOVNX16_FROMN_2X32(hvecOutH1W1D), quantScale);                                    \
    hvecOutH1W1A = IVP_PACKVRN_2X64W(dProd1, 31 - leftShift);                                                          \
    hvecOutH1W1B = IVP_PACKVRN_2X64W(dProd2, 31 - leftShift);                                                          \
    hvecOutH1W1C = IVP_PACKVRN_2X64W(dProd3, 31 - leftShift);                                                          \
    hvecOutH1W1D = IVP_PACKVRN_2X64W(dProd4, 31 - leftShift);                                                          \
    MORPH_IVP_PACKVRNX48(vec16H1W1AB, hvecOutH1W1B, hvecOutH1W1A, rightShift);                                         \
    MORPH_IVP_PACKVRNX48(vec16H1W1CD, hvecOutH1W1D, hvecOutH1W1C, rightShift);                                         \
    vec16H1W1AB = IVP_ADDSNX16(vec16H1W1AB, zeroPtOutput);                                                             \
    vec16H1W1CD = IVP_ADDSNX16(vec16H1W1CD, zeroPtOutput);                                                             \
    vec16H1W1AB = IVP_MAXNX16(IVP_MINNX16(vec16H1W1AB, (xb_vecNx16) maxLim),                                           \
                              (xb_vecNx16) minLim);                                                                    \
    vec16H1W1CD = IVP_MAXNX16(IVP_MINNX16(vec16H1W1CD, (xb_vecNx16) maxLim),                                           \
                              (xb_vecNx16) minLim);                                                                    \
  }

/***************** xiConvolvedFixupA3D_1x1_U8S32_DWH_WHD *************************/
/* Description :Implementation which                                             */
/*                 a) Computes fixup for 1x1 DWH tiles for Android NN            */
/*                 b) Transposes the input from DWH to WHD format and stores     */
/*                    them in fixupTile after fixupData.                         */
/*                 c) Should be used with xiConvolvedA3D_1x1_U8S8U8j1d1_MOW_DWH  */
/*                                                                               */
/* Inputs      : Input data tile, Coeff data Tile,                               */
/*               CNNA convolution params structure                               */
/* InOuts      : outTile, Fixup Tile                                             */
/* Assumptions : Kernel width = 1 and Kernel height = 1                          */
/*               Input data is U8                                                */
/*               Coeff data is S8                                                */
/*               Output data is U8                                               */
/*               Fixup data is S32                                               */
/*********************************************************************************/
XI_ERR_TYPE xiConvolvedFixupA3D_1x1_U8S32_DWH_WHD(
  const xi_pTile3D inTile,
  const xi_pTile4D coeffTile,
  const xi_pTile3D outTile,
  xi_pTile fixupTile,
  const xi_cnna_conv_params *param
  )
{
  XI_ERROR_CHECKS()
  {
    XI_CHECK_TILE3D_U8(inTile);
    XI_CHECK_TILE4D_S8(coeffTile);
    XI_CHECK_TILE3D_U8(outTile);
    XI_CHECK_TILE_S32(fixupTile);
    XI_CHECK_POINTER(param);
    XI_CHECK_TILE3D_FITS_IN_SINGLE_DRAM(inTile);
    XI_CHECK_TILE4D_IN_DRAM_BOUNDARY(coeffTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(fixupTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(inTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(fixupTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(coeffTile, outTile);
    XI_CHECK_TILE3D_DATA_ORDER(inTile, XI_DWH);
    XI_CHECK_TILE4D_DATA_ORDER(coeffTile, XI_WHDN);
    XI_CHECK_TILE3D_DATA_ORDER(outTile, XI_DWH);
    XI_CHECK_DILATION_CNNA(param, 1, 1);
    XI_CHECK_STRIDE_CNNA(param, 1, 1);
    XI_CHECK_COEFFTILE_CONTIGUOUS(coeffTile, param);
    XI_CHECK_ERROR((31 * XI_TILE3D_GET_DIM1_PITCH(inTile)) < (64 * 1024), XI_ERR_DATASIZE, "Tile size too big for Gather operation");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM2(inTile) == XI_TILE3D_GET_DIM2(outTile), XI_ERR_DATASIZE, "Input and Output should have same width.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM3(inTile) == XI_TILE3D_GET_DIM3(outTile), XI_ERR_DATASIZE, "Input and Output should have same height.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM2(inTile) == XI_TILE_GET_WIDTH(fixupTile), XI_ERR_DATASIZE, "Input and Fixup should have same width.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM3(inTile) == XI_TILE_GET_HEIGHT(fixupTile), XI_ERR_DATASIZE, "Input and Fixup should have same height.");
    XI_CHECK_ERROR((XI_CNNA_CONV_GET_STRIDE(param) == 1), XI_ERR_BADARG, \
                   "Provided stride value is not supported.");
    XI_CHECK_KERNEL_SIZE(coeffTile, 1);
    XI_CHECK_ERROR(XI_TILE_GET_BUFF_SIZE(fixupTile) >= \
                   sizeof(int) * XI_TILE3D_GET_DIM2(outTile) * XI_TILE3D_GET_DIM3(outTile) +
                   XI_TILE3D_GET_DIM1(inTile) * XI_TILE3D_GET_DIM2(inTile) * XI_TILE3D_GET_DIM3(inTile), \
                   XI_ERR_DATASIZE, "Fixup tile size insufficient");
  }

  const int32_t fixupWidth  = XI_TILE_GET_WIDTH(fixupTile);
  const int32_t fixupHeight = XI_TILE_GET_HEIGHT(fixupTile);
  const int32_t fixupPitch  = XI_TILE_GET_PITCH(fixupTile);
  const int32_t inPitch1    = XI_TILE3D_GET_DIM1_PITCH(inTile);
  const int32_t inPitch2    = XI_TILE3D_GET_DIM2_PITCH(inTile);
  const int32_t inPitch1R   = XI_TILE3D_GET_DIM2(inTile);       // pitch1 = width + 2 = inner most dimension for transposed data
  const int32_t inPitch2R   = XI_TILE3D_GET_DIM2(inTile) * XI_TILE3D_GET_DIM3(inTile);

  const int32_t inDepth        = XI_TILE3D_GET_DIM1(inTile);
  const int32_t inWidth        = XI_TILE3D_GET_DIM2(inTile);
  const int32_t coeffZeroPoint = XI_CNNA_CONV_GET_ZEROPT_COEFF(param);
  uint8_t* restrict pInput     = (uint8_t *) XI_TILE3D_GET_DATA_PTR(inTile);
  int32_t* restrict pFixup     = (int32_t *) XI_TILE_GET_DATA_PTR(fixupTile);
  // Transposed input is merged with fixup tile. This is done to be compatible with previous fixups
  uint8_t* restrict pInputR = (uint8_t *) (pFixup + fixupPitch * fixupHeight);    // Transposed input buffer pointer
  xb_vecNx16U vecOffsetT;
  xb_vecNx16U vecOffset = IVP_SEQNX16U() * (xb_vecNx16U) inPitch1;
  uint8_t* restrict pIn;
  xb_vec2Nx8U* restrict pvecInR;
  xb_vec2Nx8U dvec0, dvec1;
  xb_vec2Nx24 dacc1;
  dvec1 = 0;
  int32_t predLanes;
  uint32_t inPtr;

  valign vas1 = IVP_ZALIGN();

  for (int w = 0; w < fixupWidth; w += (XCHAL_IVPN_SIMD_WIDTH << 1))
  {
    for (int h = 0; h < fixupHeight; h++)
    {
      xb_vecN_2x32v* phvecFixup = (xb_vecN_2x32v *) (pFixup + w + h * fixupPitch);
      dacc1 = 0;
      for (int d = 0; d < inDepth; d++)
      {
        pIn       = (uint8_t *) (pInput + d + w * inPitch1 + h * inPitch2);
        predLanes = inWidth - w;
        vboolN vbGatherMask = IVP_LTNX16(IVP_SEQNX16(), predLanes);
        inPtr = (uint32_t) pIn;
        XT_MOVLTZ(inPtr, (uint32_t) pInput, predLanes - 1);
        pIn        = (uint8_t *) inPtr;
        vecOffsetT = IVP_MOVNX16UT(vecOffset, 0, vbGatherMask);
        xb_gsr vg1 = IVP_GATHERANX8UT(pIn, vecOffsetT, vbGatherMask);
        pIn          = (uint8_t *) (pInput + d + (w + XCHAL_IVPN_SIMD_WIDTH) * inPitch1 + h * inPitch2);
        predLanes    = inWidth - w - XCHAL_IVPN_SIMD_WIDTH;
        vbGatherMask = IVP_LTNX16(IVP_SEQNX16(), predLanes);
        inPtr        = (uint32_t) pIn;
        XT_MOVLTZ(inPtr, (uint32_t) pInput, predLanes - 1);
        pIn        = (uint8_t *) inPtr;
        vecOffsetT = IVP_MOVNX16UT(vecOffset, 0, vbGatherMask);
        xb_gsr vg2          = IVP_GATHERANX8UT(pIn, vecOffsetT, vbGatherMask);
        xb_vecNx16 vecData0 = IVP_GATHERDNX8U(vg1);
        xb_vecNx16 vecData1 = IVP_GATHERDNX8U(vg2);
        dvec0 = IVP_SEL2NX8UI(IVP_MOV2NX8_FROMNX16(vecData1), IVP_MOV2NX8_FROMNX16(vecData0), IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0);
        IVP_ADDWUA2NX8(dacc1, dvec0, dvec1);

        pvecInR = (xb_vec2Nx8U *) (pInputR + w + h * inPitch1R + d * inPitch2R);
        IVP_SAV2NX8U_XP(dvec0, vas1, pvecInR, inWidth - w);
        IVP_SAPOS2NX8U_FP(vas1, pvecInR);
      }
      xb_vecN_2x32v hvecOutLL = IVP_CVT32S2NX24LL(dacc1);
      xb_vecN_2x32v hvecOutLH = IVP_CVT32S2NX24LH(dacc1);
      xb_vecN_2x32v hvecOutHL = IVP_CVT32S2NX24HL(dacc1);
      xb_vecN_2x32v hvecOutHH = IVP_CVT32S2NX24HH(dacc1);
      xb_vecN_2x64w dProd1, dProd2, dProd3, dProd4;
      dProd1    = IVP_MULN_2X16X32_0((128 - coeffZeroPoint), hvecOutLL);
      dProd2    = IVP_MULN_2X16X32_0((128 - coeffZeroPoint), hvecOutLH);
      dProd3    = IVP_MULN_2X16X32_0((128 - coeffZeroPoint), hvecOutHL);
      dProd4    = IVP_MULN_2X16X32_0((128 - coeffZeroPoint), hvecOutHH);
      hvecOutLL = IVP_PACKLN_2X64W(dProd1);
      hvecOutLH = IVP_PACKLN_2X64W(dProd2);
      hvecOutHL = IVP_PACKLN_2X64W(dProd3);
      hvecOutHH = IVP_PACKLN_2X64W(dProd4);
      IVP_SAVN_2X32U_XP(hvecOutLL, vas1, phvecFixup, (fixupWidth - w) << 2);
      IVP_SAVN_2X32U_XP(hvecOutLH, vas1, phvecFixup, (fixupWidth - w - (XCHAL_IVPN_SIMD_WIDTH >> 1)) << 2);
      IVP_SAVN_2X32U_XP(hvecOutHL, vas1, phvecFixup, (fixupWidth - w - XCHAL_IVPN_SIMD_WIDTH) << 2);
      IVP_SAVN_2X32U_XP(hvecOutHH, vas1, phvecFixup, (fixupWidth - w - 3 * (XCHAL_IVPN_SIMD_WIDTH >> 1)) << 2);
      IVP_SAPOSN_2X32_FP(vas1, phvecFixup);
    }
  }
  return(XI_ERROR_STATUS());
}

/********* xiConvolvedA3D_1x1_U8S8U8j1d1_MOW_DWH **********************************/
/* Description :Implementation which                                              */
/*                 a) Computes convolution for 1x1 tiles for Android NN           */
/*                 b) Scatters the output to DWH format to store them along width */
/*                 c) Should be used with xiConvolvedFixupA3D_1x1_U8S32_DWH_WHD   */
/*                                                                                */
/* Inputs      : Input data tile, Coeff data Tile,                                */
/*               biasArray, Fixup Tile, CNNA convolution params structure         */
/* InOuts      : outTile                                                          */
/* Assumptions : Kernel width = 1 and Kernel height = 1                           */
/*               Input data is U8                                                 */
/*               Coeff data is S8                                                 */
/*               Output data is U8                                                */
/*               Fixup data is S32                                                */
/*               biasArray is S32                                                 */
/**********************************************************************************/
XI_ERR_TYPE xiConvolvedA3D_1x1_U8S8U8j1d1_MOW_DWH(
  const xi_pTile3D inTile, xi_pTile fixupTile,
  const xi_pTile4D coeffTile, const xi_pArray biasArray,
  xi_pTile3D outTile, const xi_cnna_conv_params *param)
{
  /* Error Checks */
  XI_ERROR_CHECKS()
  {
    XI_CHECK_TILE3D_U8(inTile);
    XI_CHECK_TILE3D_U8(outTile);
    XI_CHECK_TILE4D_S8(coeffTile);
    XI_CHECK_TILE_S32(fixupTile);
    XI_CHECK_ARRAY_S32(biasArray);
    XI_CHECK_TILE3D_FITS_IN_SINGLE_DRAM(outTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE4D_IN_DRAM_BOUNDARY(coeffTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(fixupTile);
    XI_CHECK_POINTER(param);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(inTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(fixupTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(coeffTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(biasArray, outTile);
    XI_CHECK_KERNEL_SIZE(coeffTile, 1);
    XI_CHECK_TILE3D_DATA_ORDER(inTile, XI_DWH);
    XI_CHECK_TILE3D_DATA_ORDER(outTile, XI_DWH);
    XI_CHECK_TILE4D_DATA_ORDER(coeffTile, XI_WHDN);
    XI_CHECK_COEFFTILE_CONTIGUOUS(coeffTile, param);
    XI_CHECK_ERROR((31 * XI_TILE3D_GET_DIM1_PITCH(outTile)) < (64 * 1024), XI_ERR_DATASIZE, "Tile size too big for Scatter operation");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM2(inTile) == XI_TILE3D_GET_DIM2(outTile), XI_ERR_DATASIZE, "Input and Output should have same width.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM3(inTile) == XI_TILE3D_GET_DIM3(outTile), XI_ERR_DATASIZE, "Input and Output should have same height.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM2(inTile) == XI_TILE_GET_WIDTH(fixupTile), XI_ERR_DATASIZE, "Input and Fixup should have same width.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM3(inTile) == XI_TILE_GET_HEIGHT(fixupTile), XI_ERR_DATASIZE, "Input and Fixup should have same height.");
    XI_CHECK_ERROR((XI_CNNA_CONV_GET_DILATIONX(param) == 1 && XI_CNNA_CONV_GET_DILATIONY(param) == 1), XI_ERR_BADARG, "Provided dilation not supported.");
    XI_CHECK_ERROR((XI_CNNA_CONV_GET_STRIDEX(param) == 1 && XI_CNNA_CONV_GET_STRIDEY(param) == 1), XI_ERR_BADARG, "Provided stride value is not supported.");
    XI_CHECK_ERROR(((XI_CNNA_CONV_GET_OUTPUT_SHIFT(param) < 32) && (XI_CNNA_CONV_GET_OUTPUT_SHIFT(param) > -32)), XI_ERR_BADARG, "Provided output shift value is not supported.");
    XI_CHECK_ERROR(XI_TILE_GET_BUFF_SIZE(fixupTile) >= \
                   sizeof(int) * XI_TILE3D_GET_DIM2(outTile) * XI_TILE3D_GET_DIM3(outTile) +
                   XI_TILE3D_GET_DIM1(inTile) * XI_TILE3D_GET_DIM2(inTile) * XI_TILE3D_GET_DIM3(inTile), \
                   XI_ERR_DATASIZE, "Fixup tile size insufficient");
    XI_CHECK_ERROR(((XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param) >= 0) && \
      (XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param) <= UCHAR_MAX)), XI_ERR_BADARG,
      "Zero point output value should be between 0 to 255 for U8 variants.");
    XI_CHECK_ERROR(((XI_CNNA_CONV_GET_ZEROPT_INPUT(param) >= 0) && \
      (XI_CNNA_CONV_GET_ZEROPT_INPUT(param) <= UCHAR_MAX)), XI_ERR_BADARG,
      "Zero point input value should be between 0 to 255 for U8 variants.");
  }

  /* Getting parameters from the tile structures */
  const int32_t numOutCh = XI_TILE3D_GET_DIM1(outTile);
  const int32_t outW     = XI_TILE3D_GET_DIM2(outTile);
  const int32_t outH     = XI_TILE3D_GET_DIM3(outTile);
  const int32_t numInCh  = XI_TILE3D_GET_DIM1(inTile);

  /* Pitches of Coefficient Data (WHDN) in dim1 and dim3 */
  const int32_t coeffPitch3 = XI_TILE4D_GET_DIM3_PITCH(coeffTile);

  const int32_t inDataPitch1 = XI_TILE3D_GET_DIM2(inTile);
  const int32_t inDataPitch2 = XI_TILE3D_GET_DIM2(inTile) * XI_TILE3D_GET_DIM3(inTile);

  /* Pitch of Output Data in dim1 and dim2 */
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);

  /* Kernel Size (WHDN)*/
  const int32_t kSizeU = XI_TILE4D_GET_DIM1(coeffTile);

  /* CNN convolution parameters */
  const int32_t outScale        = XI_CNNA_CONV_GET_QUANT_SCALE(param);
  const int16_t outShift        = XI_CNNA_CONV_GET_OUTPUT_SHIFT(param);
  const uint8_t enableReLu      = XI_CNNA_CONV_GET_FLAG_RELU(param);
  const int32_t outputZeroPoint = (int32_t)XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param);

  int32_t leftShift = outShift < 0 ? -outShift : 0;
  int32_t rightShift = outShift < 0 ? 0 : outShift;


  /* Data Pointers of input, output, coefficient and bias data */
  uint8_t* restrict pOutData   = (uint8_t *) XI_TILE3D_GET_DATA_PTR(outTile);
  int32_t* restrict pBiasData  = (int32_t *) XI_ARRAY_GET_DATA_PTR(biasArray);
  int32_t* restrict pFixupData = (int32_t *) XI_TILE_GET_DATA_PTR(fixupTile);

  int32_t fixupPitch          = XI_TILE_GET_PITCH(fixupTile);
  int32_t fixupHeight         = XI_TILE_GET_HEIGHT(fixupTile);
  int8_t* restrict pCoeffData = (int8_t *) XI_TILE4D_GET_DATA_PTR(coeffTile);
  uint8_t* restrict pInData   = (uint8_t *) (pFixupData + fixupPitch * fixupHeight);

  /* Setting the limits for output data according to ReLu Flag and outTileType */
  const int16_t reluMin = enableReLu ? XI_CNNA_CONV_GET_RELUMIN(param) : 0;
  const int16_t reluMax = enableReLu ? XI_CNNA_CONV_GET_RELUMAX(param) : 255;
  xb_vec2Nx8U* restrict pdvecIn;
  xb_vec2Nx8* restrict pdvecCoeff1;
  xb_vecNx16U vecOffsetT1, vecOffsetT2;
  xb_vecNx16U vecScatterOffset = IVP_SEQNX16U() * (xb_vecNx16U) outDataPitch1;
  xb_vecNx16U vecSeqU          = IVP_SEQNX16U();

  int32_t predLanes1;
  int32_t predLanes2;
  uint32_t outPtr1;
  uint32_t outPtr2;

  /* Variable Declarations */
  int32_t inCh, outCh, x, y;
  /* In order to make the loop multiply-bound we are reducing the vectorization width
     by extra values required for the kernel */
  const int32_t vectorizationWidth = ((2 * XCHAL_IVPN_SIMD_WIDTH) - kSizeU) + 1;

  /* loop across output channels and output height are unrolled twice
   * to produce four output vectors in 1 iteration
   */

  if( outW == 1)
  {
	  x = 0;
	  {
		y = 0;
		{
		  int32_t *pFixup    = &pFixupData[y * fixupPitch + x];

		  /* initialize input data pointer */
		  uint8_t *pInput = &pInData[inDataPitch1 * y + x];

		  /* initialize coeff and Bias data pointer */
		  int8_t* pCoeff = &pCoeffData[0];
		  int32_t* pBias = &pBiasData[0];

		  for (outCh = 0; outCh < numOutCh; outCh += 1)    /* Loop across Output depth */
		  {
			/* wide vectors(accumulators) initialized with bias */
			xb_vec2Nx24 dacc11, dacc12, dacc13, dacc14;
			dacc11 = dacc12 = dacc13 = dacc14 = 0;

			/* priming of coeff load is done outside the innermost loop*/
			pdvecCoeff1 = (xb_vec2Nx8 *) (pCoeff);
			valign vaCoeffData1; vaCoeffData1 = IVP_LA2NX8_PP(pdvecCoeff1);
			for (inCh = 0; inCh < numInCh; inCh++)    /* Loop across input channels */
			{
			  xb_vec2Nx8 dvecCoeffData1;
			  xb_vec2Nx8U dvecInData1, dvecInData2, dvecInData3, dvecInData4;
			  pdvecIn = (xb_vec2Nx8U *) (pInput + inCh * inDataPitch2);

			  /* load data from first input row */
			  IVP_L2U2NX8_XP(dvecInData1, pdvecIn, inDataPitch1);
			  /* load all the 1x1 coefficients for 2 output depths*/
			  IVP_LAV2NX8_XP(dvecCoeffData1, vaCoeffData1, pdvecCoeff1, 1);
			  dvecCoeffData1 = IVP_REP2NX8(dvecCoeffData1, 0);
			  IVP_MULUSA2NX8(dacc11, dvecInData1, dvecCoeffData1);
			 // IVP_MULUSA2NX8(dacc12, dvecInData2, dvecCoeffData1);
			 // IVP_MULUSA2NX8(dacc13, dvecInData3, dvecCoeffData1);
			 // IVP_MULUSA2NX8(dacc14, dvecInData4, dvecCoeffData1);
			} // end of for (inCh = 0; inCh < numInCh; inCh++)

			/* Bias, Fixup, Pack, Output Scale, Output Shift, Ouput zero point and clamping */
			xb_vecN_2x32v hvecBias1; IVP_LSRN_2X32_XP(hvecBias1, pBias, 4);

			xb_vecN_2x32v hvecFixup11, hvecFixup12, hvecFixup13, hvecFixup14;

			xb_vecN_2x32v* phvecFixup = (xb_vecN_2x32v *) pFixup;
			valign vaFixup            = IVP_LAN_2X32_PP(phvecFixup);
			IVP_LAVN_2X32_XP(hvecFixup11, vaFixup, phvecFixup, (outW - x) << 2);

			xb_vecNx16U vecOut10, vecOut20, vecOut30, vecOut40;
			xb_vecNx16U vecOut11, vecOut21, vecOut31, vecOut41;

			ADD_FIXUP_BIAS_ZPT_PACK_SCALE_SHIFT_CLAMP_LIMITS(vecOut11, vecOut10, dacc11, hvecFixup11, hvecFixup12, hvecFixup13, hvecFixup14,
															 hvecBias1, outputZeroPoint, outScale, leftShift,rightShift, reluMin, reluMax);

			uint8_t* pOutput = &pOutData[y * outDataPitch2 + x * outDataPitch1 + outCh];
			int32_t varLen   = XT_MIN(vectorizationWidth, outW - x);

			vboolN vb1, vb2;

			predLanes1 = varLen;
			outPtr1 = (uint32_t) pOutput;
			vb1         = IVP_LTNX16(vecSeqU, predLanes1);
			vecOffsetT1 = IVP_MOVNX16UT(vecScatterOffset, 0, vb1);
			XT_MOVLTZ(outPtr1, (uint32_t) pOutput, predLanes1 - 1);
			IVP_SCATTERNX8UT(vecOut10, (uint8_t *)outPtr1, vecOffsetT1, vb1);

			pCoeff += 1 * coeffPitch3;
		  }  // end of for (outCh = 0; outCh < numOutCh; outCh += 2)
		}    // end of for (y = 0; y < outH; y += 2)
	  }      // end of for (x = 0; x < outW; x += vectorizationWidth)
	  IVP_SCATTERW();
  }
  else
  {
  for (x = 0; x < outW; x += vectorizationWidth) /* Loop across output width */
  {
    for (y = 0; y < outH; y += 4)   /* Loop across output height */
    {
      int32_t enable2Row = XT_SALT(y, outH - 1);
      int32_t enable3Row = XT_SALT(y, outH - 2);
      int32_t enable4Row = XT_SALT(y, outH - 3);
      int32_t *pFixup    = &pFixupData[y * fixupPitch + x];

      /* initialize input data pointer */
      uint8_t *pInput = &pInData[inDataPitch1 * y + x];

      /* initialize coeff and Bias data pointer */
      int8_t* pCoeff = &pCoeffData[0];
      int32_t* pBias = &pBiasData[0];

      for (outCh = 0; outCh < numOutCh; outCh += 1)    /* Loop across Output depth */
      {
        /* wide vectors(accumulators) initialized with bias */
        xb_vec2Nx24 dacc11, dacc12, dacc13, dacc14;
        dacc11 = dacc12 = dacc13 = dacc14 = 0;

        /* priming of coeff load is done outside the innermost loop*/
        pdvecCoeff1 = (xb_vec2Nx8 *) (pCoeff);
        valign vaCoeffData1; vaCoeffData1 = IVP_LA2NX8_PP(pdvecCoeff1);
        for (inCh = 0; inCh < numInCh; inCh++)    /* Loop across input channels */
        {
          xb_vec2Nx8 dvecCoeffData1;
          xb_vec2Nx8U dvecInData1, dvecInData2, dvecInData3, dvecInData4;
          pdvecIn = (xb_vec2Nx8U *) (pInput + inCh * inDataPitch2);

          /* load data from first input row */
          IVP_L2U2NX8_XP(dvecInData1, pdvecIn, inDataPitch1);
          /* load data from 2nd input row */
          IVP_L2U2NX8_XP(dvecInData2, pdvecIn, inDataPitch1 * enable2Row);
          /* load data from 3rd input row */
          IVP_L2U2NX8_XP(dvecInData3, pdvecIn, inDataPitch1 * enable3Row);
          /* load data from 4th input row */
          IVP_L2U2NX8_XP(dvecInData4, pdvecIn, inDataPitch1 * enable4Row);

          /* load all the 1x1 coefficients for 2 output depths*/
          IVP_LAV2NX8_XP(dvecCoeffData1, vaCoeffData1, pdvecCoeff1, 1);
          dvecCoeffData1 = IVP_REP2NX8(dvecCoeffData1, 0);
          IVP_MULUSA2NX8(dacc11, dvecInData1, dvecCoeffData1);
          IVP_MULUSA2NX8(dacc12, dvecInData2, dvecCoeffData1);
          IVP_MULUSA2NX8(dacc13, dvecInData3, dvecCoeffData1);
          IVP_MULUSA2NX8(dacc14, dvecInData4, dvecCoeffData1);
        } // end of for (inCh = 0; inCh < numInCh; inCh++)

        /* Bias, Fixup, Pack, Output Scale, Output Shift, Ouput zero point and clamping */
        xb_vecN_2x32v hvecBias1; IVP_LSRN_2X32_XP(hvecBias1, pBias, 4);

        xb_vecN_2x32v hvecFixup11, hvecFixup12, hvecFixup13, hvecFixup14;
        xb_vecN_2x32v hvecFixup21, hvecFixup22, hvecFixup23, hvecFixup24;
        xb_vecN_2x32v hvecFixup31, hvecFixup32, hvecFixup33, hvecFixup34;
        xb_vecN_2x32v hvecFixup41, hvecFixup42, hvecFixup43, hvecFixup44;

        xb_vecN_2x32v* phvecFixup = (xb_vecN_2x32v *) pFixup;
        valign vaFixup            = IVP_LAN_2X32_PP(phvecFixup);
        IVP_LAVN_2X32_XP(hvecFixup11, vaFixup, phvecFixup, (outW - x) << 2);
        IVP_LAVN_2X32_XP(hvecFixup12, vaFixup, phvecFixup, (outW - x - (XCHAL_IVPN_SIMD_WIDTH >> 1)) << 2);
        IVP_LAVN_2X32_XP(hvecFixup13, vaFixup, phvecFixup, (outW - x - (XCHAL_IVPN_SIMD_WIDTH >> 2)) << 2);
        IVP_LAVN_2X32_XP(hvecFixup14, vaFixup, phvecFixup, (outW - x - (XCHAL_IVPN_SIMD_WIDTH >> 1) - (XCHAL_IVPN_SIMD_WIDTH >> 2)) << 2);

        phvecFixup = (xb_vecN_2x32v *) (pFixup + enable2Row * fixupPitch);
        vaFixup    = IVP_LAN_2X32_PP(phvecFixup);
        IVP_LAVN_2X32_XP(hvecFixup21, vaFixup, phvecFixup, (outW - x) << 2);
        IVP_LAVN_2X32_XP(hvecFixup22, vaFixup, phvecFixup, (outW - x - (XCHAL_IVPN_SIMD_WIDTH >> 1)) << 2);
        IVP_LAVN_2X32_XP(hvecFixup23, vaFixup, phvecFixup, (outW - x - (XCHAL_IVPN_SIMD_WIDTH >> 2)) << 2);
        IVP_LAVN_2X32_XP(hvecFixup24, vaFixup, phvecFixup, (outW - x - (XCHAL_IVPN_SIMD_WIDTH >> 1) - (XCHAL_IVPN_SIMD_WIDTH >> 2)) << 2);

        phvecFixup = (xb_vecN_2x32v *) (pFixup + enable3Row * fixupPitch * 2);
        vaFixup    = IVP_LAN_2X32_PP(phvecFixup);
        IVP_LAVN_2X32_XP(hvecFixup31, vaFixup, phvecFixup, (outW - x) << 2);
        IVP_LAVN_2X32_XP(hvecFixup32, vaFixup, phvecFixup, (outW - x - (XCHAL_IVPN_SIMD_WIDTH >> 1)) << 2);
        IVP_LAVN_2X32_XP(hvecFixup33, vaFixup, phvecFixup, (outW - x - (XCHAL_IVPN_SIMD_WIDTH >> 2)) << 2);
        IVP_LAVN_2X32_XP(hvecFixup34, vaFixup, phvecFixup, (outW - x - (XCHAL_IVPN_SIMD_WIDTH >> 1) - (XCHAL_IVPN_SIMD_WIDTH >> 2)) << 2);

        phvecFixup = (xb_vecN_2x32v *) (pFixup + enable4Row * fixupPitch * 3);
        vaFixup    = IVP_LAN_2X32_PP(phvecFixup);
        IVP_LAVN_2X32_XP(hvecFixup41, vaFixup, phvecFixup, (outW - x) << 2);
        IVP_LAVN_2X32_XP(hvecFixup42, vaFixup, phvecFixup, (outW - x - (XCHAL_IVPN_SIMD_WIDTH >> 1)) << 2);
        IVP_LAVN_2X32_XP(hvecFixup43, vaFixup, phvecFixup, (outW - x - (XCHAL_IVPN_SIMD_WIDTH >> 2)) << 2);
        IVP_LAVN_2X32_XP(hvecFixup44, vaFixup, phvecFixup, (outW - x - (XCHAL_IVPN_SIMD_WIDTH >> 1) - (XCHAL_IVPN_SIMD_WIDTH >> 2)) << 2);
        xb_vecNx16U vecOut10, vecOut20, vecOut30, vecOut40;
        xb_vecNx16U vecOut11, vecOut21, vecOut31, vecOut41;
        ADD_FIXUP_BIAS_ZPT_PACK_SCALE_SHIFT_CLAMP_LIMITS(vecOut11, vecOut10, dacc11, hvecFixup11, hvecFixup12, hvecFixup13, hvecFixup14,
                                                         hvecBias1, outputZeroPoint, outScale, leftShift,rightShift, reluMin, reluMax);
        ADD_FIXUP_BIAS_ZPT_PACK_SCALE_SHIFT_CLAMP_LIMITS(vecOut21, vecOut20, dacc12, hvecFixup21, hvecFixup22, hvecFixup23, hvecFixup24,
                                                         hvecBias1, outputZeroPoint, outScale, leftShift,rightShift, reluMin, reluMax);
        ADD_FIXUP_BIAS_ZPT_PACK_SCALE_SHIFT_CLAMP_LIMITS(vecOut31, vecOut30, dacc13, hvecFixup31, hvecFixup32, hvecFixup33, hvecFixup34,
                                                         hvecBias1, outputZeroPoint, outScale, leftShift,rightShift, reluMin, reluMax);
        ADD_FIXUP_BIAS_ZPT_PACK_SCALE_SHIFT_CLAMP_LIMITS(vecOut41, vecOut40, dacc14, hvecFixup41, hvecFixup42, hvecFixup43, hvecFixup44,
                                                         hvecBias1, outputZeroPoint, outScale, leftShift,rightShift, reluMin, reluMax);

        uint8_t* pOutput = &pOutData[y * outDataPitch2 + x * outDataPitch1 + outCh];
        int32_t varLen   = XT_MIN(vectorizationWidth, outW - x);

        vboolN vb1, vb2;

        predLanes1 = varLen;
        predLanes2 = varLen - XCHAL_IVPN_SIMD_WIDTH;
        outPtr1 = (uint32_t) pOutput;
        outPtr2 = (uint32_t) (pOutput + outDataPitch1 * XCHAL_IVPN_SIMD_WIDTH);
        vb1         = IVP_LTNX16(vecSeqU, predLanes1);
        vb2         = IVP_LTNX16(vecSeqU, predLanes2);
        vecOffsetT1 = IVP_MOVNX16UT(vecScatterOffset, 0, vb1);
        vecOffsetT2 = IVP_MOVNX16UT(vecScatterOffset, 0, vb2);
        XT_MOVLTZ(outPtr1, (uint32_t) pOutput, predLanes1 - 1);
        XT_MOVLTZ(outPtr2, (uint32_t) pOutput, predLanes2 - 1);
        IVP_SCATTERNX8UT(vecOut10, (uint8_t *)outPtr1, vecOffsetT1, vb1);
        IVP_SCATTERNX8UT(vecOut11, (uint8_t *)outPtr2, vecOffsetT2, vb2);

        predLanes1 = varLen * enable2Row;
        predLanes2 = (varLen - XCHAL_IVPN_SIMD_WIDTH) * enable2Row;
        outPtr1 = (uint32_t) (pOutput + outDataPitch2);
        outPtr2 = (uint32_t) (pOutput + outDataPitch2 + outDataPitch1 * XCHAL_IVPN_SIMD_WIDTH);
        vb1         = IVP_LTNX16(vecSeqU, predLanes1);
        vb2         = IVP_LTNX16(vecSeqU, predLanes2);
        vecOffsetT1 = IVP_MOVNX16UT(vecScatterOffset, 0, vb1);
        vecOffsetT2 = IVP_MOVNX16UT(vecScatterOffset, 0, vb2);
        XT_MOVLTZ(outPtr1, (uint32_t) pOutput, predLanes1 - 1);
        XT_MOVLTZ(outPtr2, (uint32_t) pOutput, predLanes2 - 1);
        IVP_SCATTERNX8UT(vecOut20, (uint8_t *)outPtr1, vecOffsetT1, vb1);
        IVP_SCATTERNX8UT(vecOut21, (uint8_t *)outPtr2, vecOffsetT2, vb2);

        predLanes1 = varLen * enable3Row;
        predLanes2 = (varLen - XCHAL_IVPN_SIMD_WIDTH) * enable3Row;
        outPtr1 = (uint32_t) (pOutput + outDataPitch2 * 2);
        outPtr2 = (uint32_t) (pOutput + outDataPitch2 * 2 + outDataPitch1 * XCHAL_IVPN_SIMD_WIDTH);
        vb1         = IVP_LTNX16(vecSeqU, predLanes1);
        vb2         = IVP_LTNX16(vecSeqU, predLanes2);
        vecOffsetT1 = IVP_MOVNX16UT(vecScatterOffset, 0, vb1);
        vecOffsetT2 = IVP_MOVNX16UT(vecScatterOffset, 0, vb2);
        XT_MOVLTZ(outPtr1, (uint32_t) pOutput, predLanes1 - 1);
        XT_MOVLTZ(outPtr2, (uint32_t) pOutput, predLanes2 - 1);
        IVP_SCATTERNX8UT(vecOut30, (uint8_t *)outPtr1, vecOffsetT1, vb1);
        IVP_SCATTERNX8UT(vecOut31, (uint8_t *)outPtr2, vecOffsetT2, vb2);

        predLanes1 = varLen * enable4Row;
        predLanes2 = (varLen - XCHAL_IVPN_SIMD_WIDTH) * enable4Row;
        outPtr1 = (uint32_t) (pOutput + outDataPitch2 * 3);
        outPtr2 = (uint32_t) (pOutput + outDataPitch2 * 3 + outDataPitch1 * XCHAL_IVPN_SIMD_WIDTH);
        vb1         = IVP_LTNX16(vecSeqU, predLanes1);
        vb2         = IVP_LTNX16(vecSeqU, predLanes2);
        vecOffsetT1 = IVP_MOVNX16UT(vecScatterOffset, 0, vb1);
        vecOffsetT2 = IVP_MOVNX16UT(vecScatterOffset, 0, vb2);
        XT_MOVLTZ(outPtr1, (uint32_t) pOutput, predLanes1 - 1);
        XT_MOVLTZ(outPtr2, (uint32_t) pOutput, predLanes2 - 1);
        IVP_SCATTERNX8UT(vecOut40, (uint8_t *)outPtr1, vecOffsetT1, vb1);
        IVP_SCATTERNX8UT(vecOut41, (uint8_t *)outPtr2, vecOffsetT2, vb2);

        pCoeff += 1 * coeffPitch3;
      }  // end of for (outCh = 0; outCh < numOutCh; outCh += 2)
    }    // end of for (y = 0; y < outH; y += 2)
  }      // end of for (x = 0; x < outW; x += vectorizationWidth)
  IVP_SCATTERW();
  }
  return(XI_ERROR_STATUS());
}

/* Place static data to the local memory with lowest address. */
#ifdef __XCC__
#  define INLINE  static inline __attribute__((always_inline))
#else
#  define INLINE  static inline
#endif


#define QM32_ROW_PADDING     sizeof(int32_t)
#define QM32_FILTER_PADDING  (2 * sizeof(int32_t))
#define QM32_COL_PADDING     4

INLINE unsigned align_up(unsigned value, unsigned alignment)
{
  return((value + alignment - 1) - ((value + alignment - 1) & (alignment - 1)));
}

#define fixup_update_qm32(sum0, sum1, vaSrc0, pSrc0, vaSrc1, pSrc1, vaDst, pDst, srcCount, dstCountL, dstCountH) \
  {                                                                                                              \
    xb_vec2Nx8U d0, d1, v0, v1;                                                                                  \
    const xb_vec2Nx8U c128 = 128;                                                                                \
    IVP_LAV2NX8U_XP(d0, vaSrc0, pSrc0, srcCount);                                                                \
    IVP_LAV2NX8U_XP(d1, vaSrc1, pSrc1, srcCount);                                                                \
    v0 = IVP_SEL2NX8UI(d1 - c128, d0 - c128, IVP_SELI_INTERLEAVE_2_LO);                                          \
    v1 = IVP_SEL2NX8UI(d1 - c128, d0 - c128, IVP_SELI_INTERLEAVE_2_HI);                                          \
    IVP_SAV2NX8U_XP(v0, vaDst, pDst, dstCountL);                                                                 \
    IVP_SAV2NX8U_XP(v1, vaDst, pDst, dstCountH);                                                                 \
    IVP_ADDWUA2NX8U(sum0, d0, 0);                                                                                \
    IVP_ADDWUA2NX8U(sum1, d1, 0);                                                                                \
  }

#ifdef  IVP_MULN_2X32
#define MULN_2X32(haccScaled, hvecAcc, scaleValue)                   \
{                                                                    \
  haccScaled = IVP_MULN_2X32(hvecAcc, scaleValue);                   \
}
#else
#define MULN_2X32(haccScaled, hvecAcc, scaleValue)                                  \
{                                                                                   \
  haccScaled = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecAcc), scaleValue);   \
  IVP_MULAHN_2X16X32_1(haccScaled, IVP_MOVNX16_FROMN_2X32(hvecAcc), scaleValue);    \
}
#endif

/* Update <x0, y0> and <x1, y1> coordinates by incrementing X by two and overflowing
   to the next Y row in the case of stepping beyond output_width.
   Y is not updated if no more rows left, it ends up pointing into valid data so that
   user has a safe place to read from. */

INLINE void next_pair_of_rows(int *x0, int *y0, int *x1, int *y1, int output_width, int output_height)
{
  int inc;

  inc = 0;

  *x0 = *x1 + 1;
  // Decide on increment for Y
  XT_MOVEQZ(inc, 1, *x0 - output_width);
  // Set *x0 to 0 if reached output_width after increment
  XT_MOVEQZ(*x0, 0, *x0 - output_width);
  // Stay in bounds
  *y0 = XT_MIN(*y1 + inc, output_height - 1);

  inc = 0;

  *x1 = *x0 + 1;
  // Decide on increment for Y
  XT_MOVEQZ(inc, 1, *x1 - output_width);
  // Set *x1 to 0 if reached output_width after increment
  XT_MOVEQZ(*x1, 0, *x1 - output_width);
  // Stay in bounds
  *y1 = XT_MIN(*y0 + inc, output_height - 1);
}

/* compute_fixup_qm32 inline function added as macro to resolve VS2013 build issues */
#define compute_fixup_qm32(sum0, sum1, fixupScale, fixupQM32)                                   \
{                                                                                               \
  xb_vecN_2x32v scalarAcc0 = (int32_t) IVP_RADDN_2X32U(IVP_CVT32S2NX24LL(sum0)                  \
                                                       + IVP_CVT32S2NX24LH(sum0)                \
                                                       + IVP_CVT32S2NX24HL(sum0)                \
                                                       + IVP_CVT32S2NX24HH(sum0));              \
  xb_vecN_2x32v scalarAcc1 = (int32_t) IVP_RADDN_2X32U(IVP_CVT32S2NX24LL(sum1)                  \
                                                       + IVP_CVT32S2NX24LH(sum1)                \
                                                       + IVP_CVT32S2NX24HL(sum1)                \
                                                       + IVP_CVT32S2NX24HH(sum1));              \
                                                                                                \
  xb_vecN_2x32v hvecTemp = 0;                                                                   \
  hvecTemp = IVP_MOVN_2X32T(scalarAcc1, hvecTemp, IVP_LTRN_2I(2));                              \
  hvecTemp = IVP_MOVN_2X32T(scalarAcc0, hvecTemp, IVP_LTRN_2I(1));                              \
                                                                                                \
  /* Multiply with (128 - zeroPointCoeff) and clamp within 32bits */                            \
  xb_vecN_2x64w haccOut;                                                                        \
  MULN_2X32(haccOut, hvecTemp, fixupScale);                                                     \
  fixupQM32 = (IVP_PACKVRN_2X64W(haccOut, 0));                                                  \
}


/******************* xiConvolvedFixupA3D_QM32_MxN_U8S32 *******************************/
/* Description :Implementation which                                                  */
/*                 a) Computes fixup term that is used by                             */
/*                    xiConvolvedA3D_QM24_MxN_U8S8U8 or                               */
/*                    xiConvolvedA3D_QM32_MxN_U8S8U8                                  */
/* Inputs      : Input data tile, Coeff data Tile,                                    */
/*               biasArray, Fixup Tile, CNNA convolution params structure             */
/* InOuts      : outTile                                                              */
/* Assumptions : Input data is U8                                                     */
/*               Coeff data is S8                                                     */
/*               Output data is U8                                                    */
/*               Fixup data is S32                                                    */
/*               biasArray is S32                                                     */
/**************************************************************************************/

uint8_t selpattern[64] _XI_LOCAL_RAM0_ =
{
0,1,2,3,16,17,18,19,4,5,6,7,20,21,22,23,
8,9,10,11,24,25,26,27,12,13,14,15,28,29,30,31,
32,33,34,35,48,49,50,51,36,37,38,39,52,53,54,55,
40,41,42,43,56,57,58,59,44,45,46,47,60,61,62,63
};
uint8_t selpattern1[64] _XI_LOCAL_RAM0_ =
{
0,1,2,3,32,33,34,35,4,5,6,7,36,37,38,39,
8,9,10,11,40,41,42,43,12,13,14,15,44,45,46,47,
16,17,18,19,48,49,50,51,20,21,22,23,52,53,54,55,
24,25,26,27,56,57,58,59,28,29,30,31,60,61,62,63
};

XI_ERR_TYPE xiConvolvedFixupA3D_QM32_MxN_U8S32(const xi_pTile3D inTile,
                                               const xi_pTile4D coeffTile, const xi_pTile3D outTile,
                                               xi_pTile fixUpTile, const xi_cnna_conv_params *param)
{
  XI_ERROR_CHECKS()
  {
    XI_CHECK_TILE3D_U8(inTile);
    XI_CHECK_TILE4D_S8(coeffTile);
    XI_CHECK_TILE3D_U8(outTile);
    XI_CHECK_TILE_S32(fixUpTile);
    XI_CHECK_POINTER(param);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE4D_IN_DRAM_BOUNDARY(coeffTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(fixUpTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(inTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(fixUpTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(coeffTile, outTile);
    XI_CHECK_TILE3D_DATA_ORDER(inTile, XI_DWH);
    XI_CHECK_TILE4D_DATA_ORDER(coeffTile, XI_NDWH);
    XI_CHECK_TILE3D_DATA_ORDER(outTile, XI_DWH);
    XI_CHECK_DILATION_CNNA(param, 1, 1);
    XI_CHECK_COEFFTILE_CONTIGUOUS(coeffTile, param);
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(outTile) == XI_TILE4D_GET_DIM1(coeffTile),
                   XI_ERR_KSIZE, "Output depth Should be same as coefficient's first dimension size.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(inTile) == XI_TILE4D_GET_DIM2(coeffTile),
                   XI_ERR_KSIZE, "Input depth Should be same as coefficient's second dimension size.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM2(outTile) == XI_TILE_GET_WIDTH(fixUpTile),
                   XI_ERR_DATASIZE, "Output and Fixup should have same width.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM3(outTile) == XI_TILE_GET_HEIGHT(fixUpTile),
                   XI_ERR_DATASIZE, "Output and Fixup should have same height.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_EDGE1(inTile) == XI_TILE4D_GET_DIM3(coeffTile) >> 1,
                   XI_ERR_KSIZE, "Coefficient dimensions inconsistent with edges.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM3_EDGE1(inTile) == XI_TILE4D_GET_DIM4(coeffTile) >> 1,
                   XI_ERR_KSIZE, "Coefficient dimensions inconsistent with edges.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_EDGE2(inTile) == XI_TILE4D_GET_DIM3(coeffTile) - XI_TILE3D_GET_DIM2_EDGE1(inTile) - 1,
                   XI_ERR_KSIZE, "Coefficient dimensions inconsistent with edges.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM3_EDGE2(inTile) == XI_TILE4D_GET_DIM4(coeffTile) - XI_TILE3D_GET_DIM3_EDGE1(inTile) - 1,
                   XI_ERR_KSIZE, "Coefficient dimensions inconsistent with edges.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM2(inTile) == (XI_TILE3D_GET_DIM2(outTile) - 1) * XI_CNNA_CONV_GET_STRIDE(param) + 1,
                   XI_ERR_DATASIZE, "outTile-inTile dimensions inconsistent along second dimension.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM3(inTile) == (XI_TILE3D_GET_DIM3(outTile) - 1) * XI_CNNA_CONV_GET_STRIDE(param) + 1,
                   XI_ERR_DATASIZE, "outTile-inTile dimensions inconsistent along third dimension.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_PITCH(outTile) == XI_TILE3D_GET_DIM1(outTile) * XI_TILE3D_GET_DIM2(outTile),
                   XI_ERR_DATASIZE, "outTile should not have edges along dim1 and dim2.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(inTile) == XI_TILE3D_GET_DIM1_PITCH(inTile),
                   XI_ERR_DATASIZE, "inTile should not have edges in innermost dimension.");
    XI_CHECK_ERROR(XI_TILE_GET_WIDTH(fixUpTile) == XI_TILE_GET_PITCH(fixUpTile),
                   XI_ERR_DATASIZE, "Fixup width is not same as fixup pitch.");
    XI_CHECK_ERROR(XI_TILE4D_GET_BUFF_SIZE(coeffTile) >=
                   align_up(align_up(XI_TILE4D_GET_DIM3(coeffTile) * XI_TILE4D_GET_DIM2(coeffTile), QM32_ROW_PADDING) *
                            XI_TILE4D_GET_DIM4(coeffTile), QM32_FILTER_PADDING) * XI_TILE4D_GET_DIM1(coeffTile),
                   XI_ERR_KSIZE, "coeffTileSize not sufficient.");
    XI_CHECK_ERROR(XI_TILE3D_GET_BUFF_SIZE(outTile) >= XI_TILE3D_GET_DIM1(outTile) *
                   align_up(XI_TILE3D_GET_DIM2(outTile) * XI_TILE3D_GET_DIM3(outTile), QM32_COL_PADDING),
                   XI_ERR_DATASIZE, "outTileSize not sufficient.");
    if ((XI_TILE4D_GET_DIM3(coeffTile) == 1) && (XI_TILE4D_GET_DIM4(coeffTile) == 1) &&
        (XI_TILE4D_GET_DIM2(coeffTile) % QM32_FILTER_PADDING == 0))
    {
      XI_CHECK_ERROR(XI_TILE_GET_BUFF_SIZE(fixUpTile) >= sizeof(int32_t) *
                     align_up(XI_TILE3D_GET_DIM2(outTile) * XI_TILE3D_GET_DIM3(outTile), QM32_COL_PADDING),
                     XI_ERR_DATASIZE, "Fixup tile size insufficient");
    }
    else
    {
      XI_CHECK_ERROR(XI_TILE_GET_BUFF_SIZE(fixUpTile) >= sizeof(int32_t) *
                     align_up(XI_TILE3D_GET_DIM2(outTile) * XI_TILE3D_GET_DIM3(outTile), QM32_COL_PADDING) +
                     align_up(XI_TILE4D_GET_DIM2(coeffTile), QM32_FILTER_PADDING) *
                     align_up(XI_TILE3D_GET_DIM2(outTile) * XI_TILE3D_GET_DIM3(outTile), QM32_COL_PADDING),
                     XI_ERR_DATASIZE, "Fixup tile size insufficient");
    }
  }

  /* CNNA convolution parameters */
  const int32_t fixupScale = 128 - XI_CNNA_CONV_GET_ZEROPT_COEFF(param);

  int output_width  = XI_TILE3D_GET_DIM2(outTile);
  int output_height = XI_TILE3D_GET_DIM3(outTile);
  int kernel_depth  = XI_TILE4D_GET_DIM2(coeffTile);
  int kernel_width  = XI_TILE4D_GET_DIM3(coeffTile);
  int kernel_height = XI_TILE4D_GET_DIM4(coeffTile);

  int its         = kernel_depth * kernel_width;
  int filter_size = align_up(its, QM32_ROW_PADDING) * kernel_height;
  int stride_x    = XI_TILE3D_GET_DIM1_PITCH(inTile) * XI_CNNA_CONV_GET_STRIDE(param);
  int stride_y    = XI_TILE3D_GET_DIM2_PITCH(inTile) * XI_CNNA_CONV_GET_STRIDE(param);
  int pitch_y     = XI_TILE3D_GET_DIM2_PITCH(inTile);

  uint8_t *pData = (uint8_t *) XI_TILE3D_GET_DATA_PTR(inTile)
                   - XI_TILE3D_GET_DIM2_EDGE1(inTile) * XI_TILE3D_GET_DIM1_PITCH(inTile)
                   - XI_TILE3D_GET_DIM3_EDGE1(inTile) * XI_TILE3D_GET_DIM2_PITCH(inTile);

  int rowIts    = (its - 1) / (2 * XCHAL_IVPN_SIMD_WIDTH);
  int rowRem    = 1 + (its - 1) % (2 * XCHAL_IVPN_SIMD_WIDTH);
  int outSize   = 2 * align_up(rowRem, QM32_ROW_PADDING);
  int filterPad = align_up(filter_size, QM32_FILTER_PADDING) - filter_size;
  int rowRemL;
  int rowRemH;
  if (outSize <= 2 * XCHAL_IVPN_SIMD_WIDTH)
  {
    rowRemL = outSize;
    rowRemH = 0;
  }
  else
  {
    rowRemL = 2 * XCHAL_IVPN_SIMD_WIDTH;
    rowRemH = outSize - 2 * XCHAL_IVPN_SIMD_WIDTH;
  }

  int fixup_items = align_up(output_height * output_width, QM32_COL_PADDING);

  xb_vec2Nx8U * restrict pSrc0;
  xb_vec2Nx8U * restrict pSrc1;
  xb_vec2Nx8U * restrict pDst     = (xb_vec2Nx8U *) ((int32_t *) XI_TILE_GET_DATA_PTR(fixUpTile) + fixup_items);
  xb_vecN_2x32v * restrict pFixup = (xb_vecN_2x32v *) XI_TILE_GET_DATA_PTR(fixUpTile);

  valign vaDst   = IVP_ZALIGN();
  valign vaFixup = IVP_ZALIGN();

  int total_rows = output_height * output_width;
  /* Pick two rows. When total number of rows is odd,
     just picking any valid row for (x1, y1) which is
     total_rows + 1. */
  int y0 = 0;
  int x0 = 0;
  int y1 = y0 + (x0 == output_width - 1 && total_rows > 1);
  int x1 = (x0 == output_width - 1) ? 0 : (x0 + 1);

  /* Various specializations of a general case below */

  if (kernel_width == 1 && kernel_height == 1 && kernel_depth % QM32_FILTER_PADDING == 0)
  {
    /* Can compute fixup inplace, assuming extra rows available at the start of buffer */
    pDst = (xb_vec2Nx8U *) &pData[ -2 * align_up(filter_size, QM32_FILTER_PADDING)];
#ifdef __XCC__
#pragma loop_count min=1
#endif
   if(kernel_depth == 16) //depth = 16
   {
		int inc;
		for (int row = 0; row < total_rows; row+=4)
		{
		  xb_vec2Nx24 sum0 = 0;
		  xb_vecN_2x32v fixupQM32;
		  pSrc0 = (xb_vec2Nx8U *) &pData[y0 * stride_y + x0 * stride_x];
		  valign vaSrc0 = IVP_LA2NX8U_PP(pSrc0);
		  xb_vec2Nx8U d0, d1, v0, v1;
		  const xb_vec2Nx8U c128 = 128;
		  IVP_LAV2NX8U_XP(d0, vaSrc0, pSrc0, 4*rowRem);
		  v0 = IVP_SEL2NX8U(d0 - c128, d0 - c128,*(xb_vec2Nx8U *)&selpattern);
		  IVP_SAV2NX8U_XP(v0, vaDst, pDst, 64);
		  IVP_ADDWUA2NX8U(sum0, d0, 0);
		  x0 = x0 + 4;
		  inc = 0;
		  XT_MOVEQZ(inc, 1, x0 - output_width);
		  XT_MOVEQZ(x0, 0, x0 - output_width);// Set *x0 to 0 if reached output_width after increment
		  y0 = XT_MIN(y0 + inc, output_height - 1);// Stay in bounds
		  IVP_SAV2NX8U_XP(0, vaDst, pDst, 8 * filterPad);
		  xb_vecN_2x32v scalarAcc0 = (int32_t )IVP_RADDN_2X32U(IVP_CVT32S2NX24LL(sum0));
		  xb_vecN_2x32v scalarAcc1 = (int32_t )IVP_RADDN_2X32U(IVP_CVT32S2NX24LH(sum0));
		  xb_vecN_2x32v scalarAcc2 = (int32_t )IVP_RADDN_2X32U(IVP_CVT32S2NX24HL(sum0));
		  xb_vecN_2x32v scalarAcc3 = (int32_t )IVP_RADDN_2X32U(IVP_CVT32S2NX24HH(sum0));
		  xb_vecN_2x32v hvecTemp = 0;
		  hvecTemp = IVP_MOVN_2X32T(scalarAcc3, hvecTemp, IVP_LTRN_2I(4));
		  hvecTemp = IVP_MOVN_2X32T(scalarAcc2, hvecTemp, IVP_LTRN_2I(3));
		  hvecTemp = IVP_MOVN_2X32T(scalarAcc1, hvecTemp, IVP_LTRN_2I(2));
		  hvecTemp = IVP_MOVN_2X32T(scalarAcc0, hvecTemp, IVP_LTRN_2I(1));
		  xb_vecN_2x64w haccOut;
		  MULN_2X32(haccOut, hvecTemp, fixupScale);
		  fixupQM32 = (IVP_PACKVRN_2X64W(haccOut, 0));
		  IVP_SAVN_2X32_XP(fixupQM32, vaFixup, pFixup, 4 * 4);
		}
   }
   else if(kernel_depth==32) //depth = 32
   {
    for (int row = 0; row < total_rows; row += 2)
    {
      xb_vec2Nx24 sum0 = 0;
		  xb_vecN_2x32v fixupQM32;
		  pSrc0 = (xb_vec2Nx8U *) &pData[y0 * stride_y + x0 * stride_x];
		  valign vaSrc0 = IVP_LA2NX8U_PP(pSrc0);
		  xb_vec2Nx8U d0, d1, v0, v1;
		  const xb_vec2Nx8U c128 = 128;
		  IVP_LAV2NX8U_XP(d0, vaSrc0, pSrc0, 64);
		  v0 = IVP_SEL2NX8U(d1 - c128, d0 - c128, *(xb_vec2Nx8U *)&selpattern1);
		  IVP_SAV2NX8U_XP(v0, vaDst, pDst, 2*XCHAL_IVPN_SIMD_WIDTH);
		  IVP_ADDWUA2NX8U(sum0, d0, 0);
		  x0 = x0 + 2;
		  int inc = 0;
		  XT_MOVEQZ(inc, 1, x0 - output_width);
		  XT_MOVEQZ(x0, 0, x0 - output_width);// Set *x0 to 0 if reached output_width after increment
		  y0 = XT_MIN(y0 + inc, output_height - 1);// Stay in bounds
		  xb_vecN_2x32v scalarAcc0 = (int32_t )IVP_RADDN_2X32U(IVP_CVT32S2NX24LL(sum0));
		  xb_vecN_2x32v scalarAcc1 = (int32_t )IVP_RADDN_2X32U(IVP_CVT32S2NX24LH(sum0));
		  xb_vecN_2x32v scalarAcc2 = (int32_t )IVP_RADDN_2X32U(IVP_CVT32S2NX24HL(sum0));
		  xb_vecN_2x32v scalarAcc3 = (int32_t )IVP_RADDN_2X32U(IVP_CVT32S2NX24HH(sum0));
		  scalarAcc0 = scalarAcc0 + scalarAcc1;
		  scalarAcc2 = scalarAcc2 + scalarAcc3;
		  xb_vecN_2x32v hvecTemp = 0;
		  hvecTemp = IVP_MOVN_2X32T(scalarAcc2, hvecTemp, IVP_LTRN_2I(2));
		  hvecTemp = IVP_MOVN_2X32T(scalarAcc0, hvecTemp, IVP_LTRN_2I(1));
		  xb_vecN_2x64w haccOut;
		  MULN_2X32(haccOut, hvecTemp, fixupScale);
		  fixupQM32 = (IVP_PACKVRN_2X64W(haccOut, 0));
		  IVP_SAVN_2X32_XP(fixupQM32, vaFixup, pFixup, 2 * 4);
		}
   }
   else //multiples of 64
   {
	    for (int row = 0; row < total_rows; row += 2)
		{
		  xb_vec2Nx24 sum0 = 0;
      xb_vec2Nx24 sum1 = 0;
      xb_vecN_2x32v fixupQM32;
		  pSrc0 = (xb_vec2Nx8U *) &pData[y0 * stride_y + x0 * stride_x];
		  pSrc1 = (xb_vec2Nx8U *) &pData[y1 * stride_y + x1 * stride_x];
		  valign vaSrc0 = IVP_LA2NX8U_PP(pSrc0);
		  valign vaSrc1 = IVP_LA2NX8U_PP(pSrc1);
		  for (int i = 0; i < rowIts+1; i++)
		  {
			fixup_update_qm32(sum0, sum1, vaSrc0, pSrc0, vaSrc1, pSrc1, vaDst, pDst,
							  2 * XCHAL_IVPN_SIMD_WIDTH, 2 * XCHAL_IVPN_SIMD_WIDTH, 2 * XCHAL_IVPN_SIMD_WIDTH);
		  }
		  next_pair_of_rows(&x0, &y0, &x1, &y1, output_width, output_height);
		  IVP_SAV2NX8U_XP(0, vaDst, pDst, 2 * filterPad);
		  compute_fixup_qm32(sum0, sum1, fixupScale, fixupQM32);
		  IVP_SAVN_2X32_XP(fixupQM32, vaFixup, pFixup, 2 * 4);
		}
   }
#if 0
    for (int row = 0; row < total_rows; row += 2)
    {
      xb_vec2Nx24 sum0 = 0;
      xb_vec2Nx24 sum1 = 0;
      xb_vecN_2x32v fixupQM32;

      pSrc0 = (xb_vec2Nx8U *) &pData[y0 * stride_y + x0 * stride_x];
      pSrc1 = (xb_vec2Nx8U *) &pData[y1 * stride_y + x1 * stride_x];
      valign vaSrc0 = IVP_LA2NX8U_PP(pSrc0);
      valign vaSrc1 = IVP_LA2NX8U_PP(pSrc1);
      for (int i = 0; i < rowIts; i++)
      {
        fixup_update_qm32(sum0, sum1, vaSrc0, pSrc0, vaSrc1, pSrc1, vaDst, pDst,
                          2 * XCHAL_IVPN_SIMD_WIDTH, 2 * XCHAL_IVPN_SIMD_WIDTH, 2 * XCHAL_IVPN_SIMD_WIDTH);
      }
      fixup_update_qm32(sum0, sum1, vaSrc0, pSrc0, vaSrc1, pSrc1, vaDst, pDst, rowRem, rowRemL, rowRemH);
      next_pair_of_rows(&x0, &y0, &x1, &y1, output_width, output_height);
      IVP_SAV2NX8U_XP(0, vaDst, pDst, 2 * filterPad);
      compute_fixup_qm32(sum0, sum1, fixupScale, fixupQM32);
      IVP_SAVN_2X32_XP(fixupQM32, vaFixup, pFixup, 2 * 4);
    }
#endif
  }
  else if (rowIts == 0)
  {
#ifdef __XCC__
#pragma loop_count min=1
#endif
    for (int row = 0; row < total_rows; row += 2)
    {
      xb_vec2Nx24 sum0 = 0;
      xb_vec2Nx24 sum1 = 0;
      xb_vecN_2x32v fixupQM32;

#ifdef __XCC__
#pragma loop_count min=1
#endif
      for (int ky = 0; ky < kernel_height; ky++)
      {
        pSrc0 = (xb_vec2Nx8U *) &pData[y0 * stride_y + x0 * stride_x + ky * pitch_y ];
        pSrc1 = (xb_vec2Nx8U *) &pData[y1 * stride_y + x1 * stride_x + ky * pitch_y ];
        valign vaSrc0 = IVP_LA2NX8U_PP(pSrc0);
        valign vaSrc1 = IVP_LA2NX8U_PP(pSrc1);
        fixup_update_qm32(sum0, sum1, vaSrc0, pSrc0, vaSrc1, pSrc1, vaDst, pDst, rowRem, rowRemL, rowRemH);
      }
      next_pair_of_rows(&x0, &y0, &x1, &y1, output_width, output_height);
      IVP_SAV2NX8U_XP(0, vaDst, pDst, 2 * filterPad);
      compute_fixup_qm32(sum0, sum1, fixupScale, fixupQM32);
      IVP_SAVN_2X32_XP(fixupQM32, vaFixup, pFixup, 2 * 4);
    }
  }
  else if (rowIts == 1)
  {
#ifdef __XCC__
#pragma loop_count min=1
#endif
    for (int row = 0; row < total_rows; row += 2)
    {
      xb_vec2Nx24 sum0 = 0;
      xb_vec2Nx24 sum1 = 0;
      xb_vecN_2x32v fixupQM32;

#ifdef __XCC__
#pragma loop_count min=1
#endif
      for (int ky = 0; ky < kernel_height; ky++)
      {
        pSrc0 = (xb_vec2Nx8U *) &pData[y0 * stride_y + x0 * stride_x + ky * pitch_y ];
        pSrc1 = (xb_vec2Nx8U *) &pData[y1 * stride_y + x1 * stride_x + ky * pitch_y ];
        valign vaSrc0 = IVP_LA2NX8U_PP(pSrc0);
        valign vaSrc1 = IVP_LA2NX8U_PP(pSrc1);
        fixup_update_qm32(sum0, sum1, vaSrc0, pSrc0, vaSrc1, pSrc1, vaDst, pDst,
                          2 * XCHAL_IVPN_SIMD_WIDTH, 2 * XCHAL_IVPN_SIMD_WIDTH, 2 * XCHAL_IVPN_SIMD_WIDTH);
        fixup_update_qm32(sum0, sum1, vaSrc0, pSrc0, vaSrc1, pSrc1, vaDst, pDst, rowRem, rowRemL, rowRemH);
      }
      next_pair_of_rows(&x0, &y0, &x1, &y1, output_width, output_height);
      IVP_SAV2NX8U_XP(0, vaDst, pDst, 2 * filterPad);
      compute_fixup_qm32(sum0, sum1, fixupScale, fixupQM32);
      IVP_SAVN_2X32_XP(fixupQM32, vaFixup, pFixup, 2 * 4);
    }
  }
  else if (rowIts == 2)
  {
#ifdef __XCC__
#pragma loop_count min=1
#endif
    for (int row = 0; row < total_rows; row += 2)
    {
      xb_vec2Nx24 sum0 = 0;
      xb_vec2Nx24 sum1 = 0;
      xb_vecN_2x32v fixupQM32;

#ifdef __XCC__
#pragma loop_count min=1
#endif
      for (int ky = 0; ky < kernel_height; ky++)
      {
        pSrc0 = (xb_vec2Nx8U *) &pData[y0 * stride_y + x0 * stride_x + ky * pitch_y ];
        pSrc1 = (xb_vec2Nx8U *) &pData[y1 * stride_y + x1 * stride_x + ky * pitch_y ];
        valign vaSrc0 = IVP_LA2NX8U_PP(pSrc0);
        valign vaSrc1 = IVP_LA2NX8U_PP(pSrc1);
        fixup_update_qm32(sum0, sum1, vaSrc0, pSrc0, vaSrc1, pSrc1, vaDst, pDst,
                          2 * XCHAL_IVPN_SIMD_WIDTH, 2 * XCHAL_IVPN_SIMD_WIDTH, 2 * XCHAL_IVPN_SIMD_WIDTH);
        fixup_update_qm32(sum0, sum1, vaSrc0, pSrc0, vaSrc1, pSrc1, vaDst, pDst,
                          2 * XCHAL_IVPN_SIMD_WIDTH, 2 * XCHAL_IVPN_SIMD_WIDTH, 2 * XCHAL_IVPN_SIMD_WIDTH);
        fixup_update_qm32(sum0, sum1, vaSrc0, pSrc0, vaSrc1, pSrc1, vaDst, pDst, rowRem, rowRemL, rowRemH);
      }
      next_pair_of_rows(&x0, &y0, &x1, &y1, output_width, output_height);
      IVP_SAV2NX8U_XP(0, vaDst, pDst, 2 * filterPad);
      compute_fixup_qm32(sum0, sum1, fixupScale, fixupQM32);
      IVP_SAVN_2X32_XP(fixupQM32, vaFixup, pFixup, 2 * 4);
    }
  }
  else if (rowRem == 2 * XCHAL_IVPN_SIMD_WIDTH)
  {
#ifdef __XCC__
#pragma loop_count min=1
#endif
    for (int row = 0; row < total_rows; row += 2)
    {
      xb_vec2Nx24 sum0 = 0;
      xb_vec2Nx24 sum1 = 0;
      xb_vecN_2x32v fixupQM32;
#ifdef __XCC__
#pragma loop_count min=1
#endif
      for (int ky = 0; ky < kernel_height; ky++)
      {
        pSrc0 = (xb_vec2Nx8U *) &pData[y0 * stride_y + x0 * stride_x + ky * pitch_y ];
        pSrc1 = (xb_vec2Nx8U *) &pData[y1 * stride_y + x1 * stride_x + ky * pitch_y ];
        valign vaSrc0 = IVP_LA2NX8U_PP(pSrc0);
        valign vaSrc1 = IVP_LA2NX8U_PP(pSrc1);
#ifdef __XCC__
#pragma loop_count min=4
#endif
        for (int i = 0; i < rowIts + 1; i++)
        {
          fixup_update_qm32(sum0, sum1, vaSrc0, pSrc0, vaSrc1, pSrc1, vaDst, pDst,
                            2 * XCHAL_IVPN_SIMD_WIDTH, 2 * XCHAL_IVPN_SIMD_WIDTH, 2 * XCHAL_IVPN_SIMD_WIDTH);
        }
      }
      next_pair_of_rows(&x0, &y0, &x1, &y1, output_width, output_height);
      IVP_SAV2NX8U_XP(0, vaDst, pDst, 2 * filterPad);
      compute_fixup_qm32(sum0, sum1, fixupScale, fixupQM32);
      IVP_SAVN_2X32_XP(fixupQM32, vaFixup, pFixup, 2 * 4);
    }
  }
  else
  {
    /* General case */
#ifdef __XCC__
#pragma loop_count min=1
#endif
    for (int row = 0; row < total_rows; row += 2)
    {
      xb_vec2Nx24 sum0 = 0;
      xb_vec2Nx24 sum1 = 0;
      xb_vecN_2x32v fixupQM32;
#ifdef __XCC__
#pragma loop_count min=1
#endif
      for (int ky = 0; ky < kernel_height; ky++)
      {
        pSrc0 = (xb_vec2Nx8U *) &pData[y0 * stride_y + x0 * stride_x + ky * pitch_y ];
        pSrc1 = (xb_vec2Nx8U *) &pData[y1 * stride_y + x1 * stride_x + ky * pitch_y ];
        valign vaSrc0 = IVP_LA2NX8U_PP(pSrc0);
        valign vaSrc1 = IVP_LA2NX8U_PP(pSrc1);
#ifdef __XCC__
#pragma loop_count min=3
#endif
        for (int i = 0; i < rowIts; i++)
        {
          fixup_update_qm32(sum0, sum1, vaSrc0, pSrc0, vaSrc1, pSrc1, vaDst, pDst,
                            2 * XCHAL_IVPN_SIMD_WIDTH, 2 * XCHAL_IVPN_SIMD_WIDTH, 2 * XCHAL_IVPN_SIMD_WIDTH);
        }
        fixup_update_qm32(sum0, sum1, vaSrc0, pSrc0, vaSrc1, pSrc1, vaDst, pDst, rowRem, rowRemL, rowRemH);
      }
      next_pair_of_rows(&x0, &y0, &x1, &y1, output_width, output_height);
      IVP_SAV2NX8U_XP(0, vaDst, pDst, 2 * filterPad);
      compute_fixup_qm32(sum0, sum1, fixupScale, fixupQM32);
      IVP_SAVN_2X32_XP(fixupQM32, vaFixup, pFixup, 2 * 4);
    }
  }
  IVP_SAPOSN_2X32_FP(vaFixup, pFixup);
  IVP_SAPOS2NX8U_FP(vaDst, pDst);
  return(XI_ERROR_STATUS());
}


#define extrN_2x32(v, index)  IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(v)), index)

/* add64x32 inline function added as macro to resolve VS2013 build issues */
#define add64x32(acc64x32, acc64x24)           \
{                                              \
  acc64x32[0] += IVP_CVT32S2NX24LL(acc64x24);  \
  acc64x32[1] += IVP_CVT32S2NX24LH(acc64x24);  \
  acc64x32[2] += IVP_CVT32S2NX24HL(acc64x24);  \
  acc64x32[3] += IVP_CVT32S2NX24HH(acc64x24);  \
}

#define quant_acc32x32(accL, accH, quantScale, leftShift, rightShift, zeroPtOut, minLim, maxLim, quantOutValue)     \
{                                                                                                      \
  xb_vecNx16 vecOutShifted;                                                                            \
  /* Scale, Shift, add output zeropoint and Clamp the accumulator output */                            \
  xb_vecN_2x64w haccScaledL, haccScaledH;                                                              \
  MULN_2X32(haccScaledL, accL, quantScale);                                                            \
  MULN_2X32(haccScaledH, accH, quantScale);                                                            \
  xb_vecN_2x32v packedH = IVP_PACKVRN_2X64W(haccScaledH, 31 - leftShift);                              \
  xb_vecN_2x32v packedL = IVP_PACKVRN_2X64W(haccScaledL, 31 - leftShift);                              \
  CONV_PACK_AND_ROUNDING(packedH, packedL, rightShift, vecOutShifted);                                 \
  xb_vecNx16 vecOutAddZP = IVP_ADDSNX16(vecOutShifted, (xb_vecNx16) zeroPtOut);                        \
  quantOutValue = (IVP_MAXNX16(IVP_MINNX16(vecOutAddZP, (xb_vecNx16) maxLim), (xb_vecNx16) minLim));   \
}

#if 1
#define CONV_PACK_AND_ROUNDING(packedH, packedL, outShift, vecOutShifted)                \
  /* Performs ASYMMETRIC ROUNDING Operation */                                           \
  vecOutShifted = IVP_PACKVRNX48(IVP_CVT48SNX32(packedH, packedL), outShift)
#else
#define CONV_PACK_AND_ROUNDING(packedH, packedL, outShift, vecOutShifted)                \
{                                                                                        \
  /* Performs SYMMETRIC ROUNDING Operation */                                            \
  vboolN_2 vbA = IVP_LTN_2X32(packedL, 0);                                               \
  vboolN_2 vbB = IVP_LTN_2X32(packedH, 0);                                               \
  packedL = IVP_ABSN_2X32(packedL);                                                      \
  packedH = IVP_ABSN_2X32(packedH);                                                      \
  xb_vecNx48 wvec48AB      = IVP_CVT48SNX32(packedH, packedL);                           \
  vecOutShifted = IVP_PACKVRNX48(wvec48AB, outShift);                                    \
  vboolN vbAB              = IVP_JOINBN_2(vbB, vbA);                                     \
  IVP_SUBNX16T(vecOutShifted, 0, vecOutShifted, vbAB);                                   \
}
#endif

INLINE xb_vec2Nx8 quant_acc64x32(xb_vecN_2x32v *acc64x32,
                                 int32_t quantScale,
                                 int32_t leftShift,
                                 int32_t rightShift,
                                 uint8_t zeroPtOut,
                                 uint8_t minLim,
                                 uint8_t maxLim)
{

  xb_vecNx16U L;
  xb_vecNx16U H;

  quant_acc32x32(acc64x32[0], acc64x32[1], quantScale, leftShift, rightShift, zeroPtOut, minLim, maxLim, L);
  quant_acc32x32(acc64x32[2], acc64x32[3], quantScale, leftShift, rightShift, zeroPtOut, minLim, maxLim, H);

  return(IVP_SEL2NX8I(IVP_MOV2NX8_FROMNX16(H), IVP_MOV2NX8_FROMNX16(L), IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0));
}

INLINE void load64x32(const int count, const int32_t *value, xb_vecN_2x32v *bias64x32)
{
  xb_vecN_2x32v *pv = (xb_vecN_2x32v *) value;

  int ld0 = min(2 * XCHAL_IVPN_SIMD_WIDTH, count * sizeof(int32_t));
  int ld1 = min(2 * XCHAL_IVPN_SIMD_WIDTH, count * sizeof(int32_t) - ld0);
  int ld2 = min(2 * XCHAL_IVPN_SIMD_WIDTH, count * sizeof(int32_t) - ld0 - ld1);
  int ld3 = min(2 * XCHAL_IVPN_SIMD_WIDTH, count * sizeof(int32_t) - ld0 - ld1 - ld2);

  valign va = IVP_LAN_2X32_PP(pv);

  IVP_LAVN_2X32_XP(bias64x32[0], va, pv, ld0);
  IVP_LAVN_2X32_XP(bias64x32[1], va, pv, ld1);
  IVP_LAVN_2X32_XP(bias64x32[2], va, pv, ld2);
  IVP_LAVN_2X32_XP(bias64x32[3], va, pv, ld3);
}

INLINE void load64x32_depth32(const int count, const int32_t *value, xb_vecN_2x32v *bias64x32)
{
  xb_vecN_2x32v *pv = (xb_vecN_2x32v *) value;
  int ld0 = min(2 * XCHAL_IVPN_SIMD_WIDTH, count * sizeof(int32_t));
  int ld1 = min(2 * XCHAL_IVPN_SIMD_WIDTH, count * sizeof(int32_t) - ld0);
  valign va = IVP_LAN_2X32_PP(pv);
  IVP_LAVN_2X32_XP(bias64x32[0], va, pv, ld0);
  IVP_LAVN_2X32_XP(bias64x32[1], va, pv, ld1);
  bias64x32[2] = bias64x32[0];
  bias64x32[3] = bias64x32[1];
}

INLINE void init64x32_depth32(xb_vecN_2x32v *acc64x32, xb_vecN_2x32v *bias64x32, int32_t *broadcast)
{
  xb_vecN_2x32v vec = IVP_LSRN_2X32_I(broadcast, 0);
  xb_vecN_2x32v vec1 = IVP_LSRN_2X32_I(broadcast, 4);

  acc64x32[0] = bias64x32[0] + vec;
  acc64x32[1] = bias64x32[1] + vec;
  acc64x32[2] = bias64x32[2] + vec1;
  acc64x32[3] = bias64x32[3] + vec1;
}

INLINE void init64x32(xb_vecN_2x32v *acc64x32, xb_vecN_2x32v *bias64x32, int32_t *broadcast)
{
  xb_vecN_2x32v vec = IVP_LSRN_2X32_I(broadcast, 0);

  acc64x32[0] = bias64x32[0] + vec;
  acc64x32[1] = bias64x32[1] + vec;
  acc64x32[2] = bias64x32[2] + vec;
  acc64x32[3] = bias64x32[3] + vec;
}


/******************* xiConvolvedA3D_QM24_MxN_U8S8U8 *******************************/
/* Description :Implementation which                                              */
/*                 a) Computes convolution when used with                         */
/*                    xiConvolvedFixupA3D_QM32_MxN_U8S32                          */
/*                 b) It uses 24 bit accumulator for computations.                */
/*                    It must be used when it is guaranteed that 24 bit           */
/*                    accumulator wont overflow                                   */
/* Inputs      : Input data tile, Coeff data Tile,                                */
/*               biasArray, Fixup Tile, CNNA convolution params structure         */
/* InOuts      : outTile                                                          */
/* Assumptions : Input data is U8                                                 */
/*               Coeff data is U8                                                 */
/*               Output data is U8                                                */
/*               Fixup data is S32                                                */
/*               biasArray is S32                                                 */
/**********************************************************************************/

uint8_t sel[64] _XI_LOCAL_RAM0_ = {0,0,4,4,1,1,5,5,2,2,6,6,3,3,7,7,
 								   8,8,12,12,9,9,13,13,10,10,14,14,11,11,15,15,
 								   32,32,36,36,33,33,37,37,34,34,38,38,35,35,39,39,
 								   40,40,44,44,41,41,45,45,42,42,46,46,43,43,47,47
							  	  };
uint8_t  dsel_0_4_1_5[64] _XI_LOCAL_RAM0_ =
{0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,0,2,
 1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3
};

uint8_t  dsel_2_6_3_7[64] _XI_LOCAL_RAM0_ =
{4,6,4,6,4,6,4,6,4,6,4,6,4,6,4,6,4,6,4,6,4,6,4,6,4,6,4,6,4,6,4,6,
 5,7,5,7,5,7,5,7,5,7,5,7,5,7,5,7,5,7,5,7,5,7,5,7,5,7,5,7,5,7,5,7
};

XI_ERR_TYPE xiConvolvedA3D_QM24_MxN_U8S8U8(const xi_pTile3D inTile,
                                           const xi_pTile fixUpTile, const xi_pTile4D coeffTile,
                                           const xi_pArray biasArray, xi_pTile3D outTile,
                                           const xi_cnna_conv_params *param)
{
  XI_ERROR_CHECKS()
  {
    XI_CHECK_TILE3D_U8(inTile);
    XI_CHECK_TILE4D_S8(coeffTile);
    XI_CHECK_TILE3D_U8(outTile);
    XI_CHECK_TILE_S32(fixUpTile);
    XI_CHECK_ARRAY_S32(biasArray);
    XI_CHECK_POINTER(param);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE4D_IN_DRAM_BOUNDARY(coeffTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(fixUpTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(inTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(fixUpTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(coeffTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(biasArray, outTile);
    XI_CHECK_TILE3D_DATA_ORDER(inTile, XI_DWH);
    XI_CHECK_TILE4D_DATA_ORDER(coeffTile, XI_NDWH);
    XI_CHECK_TILE3D_DATA_ORDER(outTile, XI_DWH);
    XI_CHECK_DILATION_CNNA(param, 1, 1);
    XI_CHECK_COEFFTILE_CONTIGUOUS(coeffTile, param);
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(outTile) == XI_TILE4D_GET_DIM1(coeffTile),
                   XI_ERR_KSIZE, "Output depth should be same as coefficient's first dimension size.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(inTile) == XI_TILE4D_GET_DIM2(coeffTile),
                   XI_ERR_KSIZE, "Input depth should be same as coefficient's second dimension size.");
    XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(biasArray) >= XI_TILE3D_GET_DIM1(outTile),
                   XI_ERR_DATASIZE, "Width of Bias Array is less than number of output channel.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM2(outTile) == XI_TILE_GET_WIDTH(fixUpTile),
                   XI_ERR_DATASIZE, "Output and Fixup should have same width.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM3(outTile) == XI_TILE_GET_HEIGHT(fixUpTile),
                   XI_ERR_DATASIZE, "Output and Fixup should have same height.");
    XI_CHECK_ERROR((XI_CNNA_CONV_GET_OUTPUT_SHIFT(param) < 32) && (XI_CNNA_CONV_GET_OUTPUT_SHIFT(param) > -32),
                    XI_ERR_BADARG, "Provided output shift value is not supported.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_EDGE1(inTile) == XI_TILE4D_GET_DIM3(coeffTile) >> 1,
                   XI_ERR_KSIZE, "Coefficient dimensions inconsistent with edges.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM3_EDGE1(inTile) == XI_TILE4D_GET_DIM4(coeffTile) >> 1,
                   XI_ERR_KSIZE, "Coefficient dimensions inconsistent with edges.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_EDGE2(inTile) == XI_TILE4D_GET_DIM3(coeffTile) - XI_TILE3D_GET_DIM2_EDGE1(inTile) - 1,
                   XI_ERR_KSIZE, "Coefficient dimensions inconsistent with edges.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM3_EDGE2(inTile) == XI_TILE4D_GET_DIM4(coeffTile) - XI_TILE3D_GET_DIM3_EDGE1(inTile) - 1,
                   XI_ERR_KSIZE, "Coefficient dimensions inconsistent with edges.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM2(inTile) == (XI_TILE3D_GET_DIM2(outTile) - 1) * XI_CNNA_CONV_GET_STRIDE(param) + 1,
                   XI_ERR_DATASIZE, "outTile-inTile dimensions inconsistent along second dimension.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM3(inTile) == (XI_TILE3D_GET_DIM3(outTile) - 1) * XI_CNNA_CONV_GET_STRIDE(param) + 1,
                   XI_ERR_DATASIZE, "outTile-inTile dimensions inconsistent along third dimension.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_PITCH(outTile) == XI_TILE3D_GET_DIM1(outTile) * XI_TILE3D_GET_DIM2(outTile),
                   XI_ERR_DATASIZE, "outTile should not have edges along dim1 and dim2.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(inTile) == XI_TILE3D_GET_DIM1_PITCH(inTile),
                   XI_ERR_DATASIZE, "inTile should not have edges in innermost dimension.");
    XI_CHECK_ERROR(XI_TILE_GET_WIDTH(fixUpTile) == XI_TILE_GET_PITCH(fixUpTile),
                   XI_ERR_DATASIZE, "Fixup width is not same as fixup pitch.");
    XI_CHECK_ERROR(XI_TILE4D_GET_BUFF_SIZE(coeffTile) >=
                   align_up(align_up(XI_TILE4D_GET_DIM3(coeffTile) * XI_TILE4D_GET_DIM2(coeffTile), QM32_ROW_PADDING) *
                            XI_TILE4D_GET_DIM4(coeffTile), QM32_FILTER_PADDING) * XI_TILE4D_GET_DIM1(coeffTile),
                   XI_ERR_KSIZE, "coeffTileSize not sufficient.");
    XI_CHECK_ERROR(XI_TILE3D_GET_BUFF_SIZE(outTile) >= XI_TILE3D_GET_DIM1(outTile) *
                   align_up(XI_TILE3D_GET_DIM2(outTile) * XI_TILE3D_GET_DIM3(outTile), QM32_COL_PADDING),
                   XI_ERR_DATASIZE, "outTileSize not sufficient.");
    if ((XI_TILE4D_GET_DIM3(coeffTile) == 1) && (XI_TILE4D_GET_DIM4(coeffTile) == 1) &&
        (XI_TILE4D_GET_DIM2(coeffTile) % QM32_FILTER_PADDING == 0))
    {
      XI_CHECK_ERROR(XI_TILE_GET_BUFF_SIZE(fixUpTile) >= sizeof(int32_t) *
                     align_up(XI_TILE3D_GET_DIM2(outTile) * XI_TILE3D_GET_DIM3(outTile), QM32_COL_PADDING),
                     XI_ERR_DATASIZE, "Fixup tile size insufficient");
    }
    else
    {
      XI_CHECK_ERROR(XI_TILE_GET_BUFF_SIZE(fixUpTile) >= sizeof(int32_t) *
                     align_up(XI_TILE3D_GET_DIM2(outTile) * XI_TILE3D_GET_DIM3(outTile), QM32_COL_PADDING) +
                     align_up(XI_TILE4D_GET_DIM2(coeffTile), QM32_FILTER_PADDING) *
                     align_up(XI_TILE3D_GET_DIM2(outTile) * XI_TILE3D_GET_DIM3(outTile), QM32_COL_PADDING),
                     XI_ERR_DATASIZE, "Fixup tile size insufficient");
    }
    if (XI_CNNA_CONV_GET_FLAG_RELU(param))
    {
      XI_CHECK_ERROR(XI_CNNA_CONV_GET_RELUMIN(param) <= XI_CNNA_CONV_GET_RELUMAX(param),
        XI_ERR_BADARG, "Relu max is less than Relu min");
      XI_CHECK_ERROR((XI_CNNA_CONV_GET_RELUMIN(param) >= 0) && \
                     (XI_CNNA_CONV_GET_RELUMIN(param) <= UCHAR_MAX),
        XI_ERR_BADARG, "Relu min value is invalid");
      XI_CHECK_ERROR((XI_CNNA_CONV_GET_RELUMAX(param) >= 0) && \
                     (XI_CNNA_CONV_GET_RELUMAX(param) <= UCHAR_MAX),
        XI_ERR_BADARG, "Relu max value is invalid");
    }
    XI_CHECK_ERROR(((XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param) >= 0) && \
      (XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param) <= UCHAR_MAX)), XI_ERR_BADARG,
      "Zero point output value should be between 0 to 255 for U8 variants.");
    XI_CHECK_ERROR(((XI_CNNA_CONV_GET_ZEROPT_INPUT(param) >= 0) && \
      (XI_CNNA_CONV_GET_ZEROPT_INPUT(param) <= UCHAR_MAX)), XI_ERR_BADARG,
      "Zero point input value should be between 0 to 255 for U8 variants.");
  }
  /* CNNA convolution parameters */
  const int32_t quantScale = XI_CNNA_CONV_GET_QUANT_SCALE(param);
  const int32_t outShift   = XI_CNNA_CONV_GET_OUTPUT_SHIFT(param);
  const uint8_t zeroPtOut = (uint8_t)XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param);
  const uint8_t enableReLu = XI_CNNA_CONV_GET_FLAG_RELU(param);
  const uint8_t reluMinU   = (uint8_t)XI_CNNA_CONV_GET_RELUMIN(param);
  const uint8_t reluMaxU   = (uint8_t)XI_CNNA_CONV_GET_RELUMAX(param);

  /* Negative output shift is equivalent to leftshift*/
  int32_t leftShift = outShift < 0 ? -outShift : 0;
  int32_t rightShift = outShift < 0 ? 0 : outShift;

  /* Setting the limits for output data according to ReLu is enabled or not*/
  const uint8_t minLim = enableReLu ? reluMinU : 0;
  const uint8_t maxLim = enableReLu ? reluMaxU : UCHAR_MAX;

  int output_depth  = XI_TILE3D_GET_DIM1(outTile);
  int output_width  = XI_TILE3D_GET_DIM2(outTile);
  int output_height = XI_TILE3D_GET_DIM3(outTile);
  int kernel_depth  = XI_TILE4D_GET_DIM2(coeffTile);
  int kernel_width  = XI_TILE4D_GET_DIM3(coeffTile);
  int kernel_height = XI_TILE4D_GET_DIM4(coeffTile);

  xb_vec2Nx8U* restrict pvOut0;
  xb_vec2Nx8U* restrict pvOut1;
  xb_vec2Nx8U* restrict pvOut2;
  xb_vec2Nx8U* restrict pvOut3;
  xb_vec2Nx8* restrict pC0;
  xb_vec2Nx8* restrict pC1;

  int filter_size      = align_up(align_up(kernel_depth * kernel_width, QM32_ROW_PADDING) * kernel_height, QM32_FILTER_PADDING);
  int output_rows      = align_up(output_width * output_height, QM32_COL_PADDING);
  int input_row_dwords = filter_size / sizeof(int32_t);

  int32_t * restrict pData  = (int32_t *) XI_TILE_GET_DATA_PTR(fixUpTile) + output_rows;
  int32_t * restrict pFixup = (int32_t *) XI_TILE_GET_DATA_PTR(fixUpTile);
  int8_t * restrict pCoeff  = (int8_t *) XI_TILE3D_GET_DATA_PTR(coeffTile);
  int32_t * restrict pBias  = (int32_t *) XI_ARRAY_GET_DATA_PTR(biasArray);
  uint8_t * restrict pOut   = (uint8_t *) XI_TILE3D_GET_DATA_PTR(outTile);

  xb_vec2Nx8 * pRow0;
  xb_vec2Nx8 * pRow1;
  xb_vec2Nx8 * pRow2;
  xb_vec2Nx8 * pRow3;


  if (kernel_width == 1 && kernel_height == 1 && kernel_depth % QM32_FILTER_PADDING == 0)
  {
    /* Fixup was computed inplace, assuming extra rows available at the start of buffer */
    pData = (int32_t *) ((int8_t *) XI_TILE3D_GET_DATA_PTR(inTile) - 2 * filter_size);
  }

   if(output_depth == 32)
   {
	      for (int row = 0; row < output_rows; row += 8)
		  {
			valign vaOut0 = IVP_ZALIGN();
			valign vaOut1 = IVP_ZALIGN();
			valign vaOut2 = IVP_ZALIGN();
			valign vaOut3 = IVP_ZALIGN();
			pvOut0 = (xb_vec2Nx8U *) &pOut[(row) * output_depth];
			pvOut1 = (xb_vec2Nx8U *) &pOut[(row + 2) * output_depth];
			pvOut2 = (xb_vec2Nx8U *) &pOut[(row + 4) * output_depth];
			pvOut3 = (xb_vec2Nx8U *) &pOut[(row + 6) * output_depth];
		#ifdef __XCC__
		#pragma loop_count min=1
		#endif
			int ch = 0;
			//for (int ch = 0; ch < output_depth; ch += 2 * XCHAL_IVPN_SIMD_WIDTH)
			{
			  int depth = min(2 * XCHAL_IVPN_SIMD_WIDTH, output_depth - ch);

			  pC0 = (xb_vec2Nx8 *) &pCoeff[ch * filter_size];
			  pC1 = (xb_vec2Nx8 *) &pCoeff[ch * filter_size + depth * (filter_size / 2)];

			  valign vaC0 = IVP_LA2NX8_PP(pC0);
			  valign vaC1 = IVP_LA2NX8_PP(pC1);

			  pRow0 = (xb_vec2Nx8 *) &pData[row * input_row_dwords ];
			  pRow1 = (xb_vec2Nx8 *) &pData[row * input_row_dwords + 2 * input_row_dwords];
			  pRow2 = (xb_vec2Nx8 *) &pData[row * input_row_dwords + 4 * input_row_dwords];
			  pRow3 = (xb_vec2Nx8 *) &pData[row * input_row_dwords + 6 * input_row_dwords];

			  valign vaRow0 = IVP_LA2NX8_PP(pRow0);
			  valign vaRow1 = IVP_LA2NX8_PP(pRow1);
			  valign vaRow2 = IVP_LA2NX8_PP(pRow2);
			  valign vaRow3 = IVP_LA2NX8_PP(pRow3);

			  xb_vec2Nx24 sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
#ifdef __XCC__
#pragma loop_count min=1
#endif
			  xb_vec2Nx8 dv,dv1,dv2,dv3;
			  IVP_LAV2NX8_XP(dv, vaRow0,  pRow0, input_row_dwords*8);  // 4 rows of 16 data
			  IVP_LAV2NX8_XP(dv1, vaRow1, pRow1, input_row_dwords*8);  // 4 rows of 16 data
			  IVP_LAV2NX8_XP(dv2, vaRow2, pRow2, input_row_dwords*8); // 4 rows of 16 data
			  IVP_LAV2NX8_XP(dv3, vaRow3, pRow3, input_row_dwords*8); // 4 rows of 16 data
			  for (int j = 0; j < input_row_dwords; j += 2)
			  {
				//re-arrange the data
				xb_vecNx16 vec_insel  = IVP_MOVNX16_FROM2NX8(IVP_SHFL2NX8(dv,*(xb_vec2Nx8 *)& sel));
				xb_vecNx16 vec_insel1  = IVP_MOVNX16_FROM2NX8(IVP_SHFL2NX8(dv1,*(xb_vec2Nx8 *)& sel));
				xb_vecNx16 vec_insel2 = IVP_MOVNX16_FROM2NX8(IVP_SHFL2NX8(dv2,*(xb_vec2Nx8 *)& sel));
				xb_vecNx16 vec_insel3 = IVP_MOVNX16_FROM2NX8(IVP_SHFL2NX8(dv3,*(xb_vec2Nx8 *)& sel));
				// load coefficients
				xb_vec2Nx8 c0, c1, c2, c3;
				IVP_LAV2NX8_XP(c0, vaC0, pC0, 64); //c0 and c1
				IVP_LAV2NX8_XP(c2, vaC1, pC1, 64); //c2 and c3
				//re-arrange coeffdata
				xb_vec2Nx8 vecTmp0 = IVP_SEL2NX8I(c0, c0, IVP_SELI_8B_EXTRACT_LO_HALVES);
				xb_vec2Nx8 vecTmp1 = IVP_SEL2NX8I(c0, c0, IVP_SELI_8B_EXTRACT_HI_HALVES);
				xb_vec2Nx8 vecTmp2 = IVP_SEL2NX8I(c2, c2, IVP_SELI_8B_EXTRACT_LO_HALVES);
				xb_vec2Nx8 vecTmp3 = IVP_SEL2NX8I(c2, c2, IVP_SELI_8B_EXTRACT_HI_HALVES);
				//repeat inputdata
				xb_vecNx16 vecInTmp0, vecInTmp1, vecInTmp2, vecInTmp3;
				IVP_DSELNX16( (vecInTmp1),  (vecInTmp0),
						(vec_insel) , (vec_insel) , *(xb_vec2Nx8 *)&dsel_0_4_1_5);
				IVP_DSELNX16( (vecInTmp3),  (vecInTmp2),
							(vec_insel) , (vec_insel) , *(xb_vec2Nx8 *)&dsel_2_6_3_7);

				IVP_MULPA2NX8(sum0,vecTmp0,IVP_MOV2NX8_FROMNX16(vecInTmp0),
							  vecTmp1,IVP_MOV2NX8_FROMNX16(vecInTmp1));
				IVP_MULPA2NX8(sum0,vecTmp2,IVP_MOV2NX8_FROMNX16(vecInTmp2),
							vecTmp3,IVP_MOV2NX8_FROMNX16(vecInTmp3));

				xb_vecNx16 vecInTmp4, vecInTmp5, vecInTmp6, vecInTmp7;
				IVP_DSELNX16( (vecInTmp5),  (vecInTmp4),
						(vec_insel1) , (vec_insel1) , *(xb_vec2Nx8 *)&dsel_0_4_1_5);
				IVP_DSELNX16( (vecInTmp7),  (vecInTmp6),
							(vec_insel1) , (vec_insel1) , *(xb_vec2Nx8 *)&dsel_2_6_3_7);

				IVP_MULPA2NX8(sum1,vecTmp0,IVP_MOV2NX8_FROMNX16(vecInTmp4),vecTmp1,IVP_MOV2NX8_FROMNX16(vecInTmp5));
				IVP_MULPA2NX8(sum1,vecTmp2,IVP_MOV2NX8_FROMNX16(vecInTmp6),vecTmp3,IVP_MOV2NX8_FROMNX16(vecInTmp7));

				xb_vecNx16 vecInTmp8, vecInTmp9, vecInTmp10, vecInTmp11;
				IVP_DSELNX16( (vecInTmp9),  (vecInTmp8),
						(vec_insel2) , (vec_insel2) , *(xb_vec2Nx8 *)&dsel_0_4_1_5);
				IVP_DSELNX16( (vecInTmp11),  (vecInTmp10),
							(vec_insel2) , (vec_insel2) , *(xb_vec2Nx8 *)&dsel_2_6_3_7);

				IVP_MULPA2NX8(sum2,vecTmp0,IVP_MOV2NX8_FROMNX16(vecInTmp8),vecTmp1,IVP_MOV2NX8_FROMNX16(vecInTmp9));
				IVP_MULPA2NX8(sum2,vecTmp2,IVP_MOV2NX8_FROMNX16(vecInTmp10),vecTmp3,IVP_MOV2NX8_FROMNX16(vecInTmp11));

				xb_vecNx16 vecInTmp12, vecInTmp13, vecInTmp14, vecInTmp15;
				IVP_DSELNX16( (vecInTmp13),  (vecInTmp12),
						(vec_insel3) , (vec_insel3) , *(xb_vec2Nx8 *)&dsel_0_4_1_5);
				IVP_DSELNX16( (vecInTmp15),  (vecInTmp14),
							(vec_insel3) , (vec_insel3) , *(xb_vec2Nx8 *)&dsel_2_6_3_7);

				IVP_MULPA2NX8(sum3,vecTmp0,IVP_MOV2NX8_FROMNX16(vecInTmp12),vecTmp1,IVP_MOV2NX8_FROMNX16(vecInTmp13));
				IVP_MULPA2NX8(sum3,vecTmp2,IVP_MOV2NX8_FROMNX16(vecInTmp14),vecTmp3,IVP_MOV2NX8_FROMNX16(vecInTmp15));

				IVP_LAV2NX8_XP(c0, vaC0, pC0, 2*depth); //c0 and c1
				IVP_LAV2NX8_XP(c2, vaC1, pC1, 2*depth); //c2 and c3
				//re-arrange the coeffdata
				vecTmp0 = IVP_SEL2NX8I(c0, c0, IVP_SELI_8B_EXTRACT_LO_HALVES);
				vecTmp1 = IVP_SEL2NX8I(c0, c0, IVP_SELI_8B_EXTRACT_HI_HALVES);
				vecTmp2 = IVP_SEL2NX8I(c2, c2, IVP_SELI_8B_EXTRACT_LO_HALVES);
				vecTmp3 = IVP_SEL2NX8I(c2, c2, IVP_SELI_8B_EXTRACT_HI_HALVES);

				xb_vecNx16 vecTmp_insel = IVP_SELNX16I(0, vec_insel, IVP_SELI_16B_ROTATE_RIGHT_8);
				xb_vecNx16 vecTmp_insel1 = IVP_SELNX16I(0, vec_insel1, IVP_SELI_16B_ROTATE_RIGHT_8);
				xb_vecNx16 vecTmp_insel2 = IVP_SELNX16I(0, vec_insel2, IVP_SELI_16B_ROTATE_RIGHT_8);
				xb_vecNx16 vecTmp_insel3 = IVP_SELNX16I(0, vec_insel3, IVP_SELI_16B_ROTATE_RIGHT_8);

				IVP_DSELNX16( (vecInTmp1),  (vecInTmp0),
						(vecTmp_insel) , (vecTmp_insel) , *(xb_vec2Nx8 *)&dsel_0_4_1_5);
				IVP_DSELNX16( (vecInTmp3),  (vecInTmp2),
							(vecTmp_insel) , (vecTmp_insel) , *(xb_vec2Nx8 *)&dsel_2_6_3_7);

				IVP_MULPA2NX8(sum0,vecTmp0,IVP_MOV2NX8_FROMNX16(vecInTmp0),
							  vecTmp1,IVP_MOV2NX8_FROMNX16(vecInTmp1));
				IVP_MULPA2NX8(sum0,vecTmp2,IVP_MOV2NX8_FROMNX16(vecInTmp2),
							vecTmp3,IVP_MOV2NX8_FROMNX16(vecInTmp3));

				IVP_DSELNX16( (vecInTmp5),  (vecInTmp4),
						(vecTmp_insel1) , (vecTmp_insel1) , *(xb_vec2Nx8 *)&dsel_0_4_1_5);
				IVP_DSELNX16( (vecInTmp7),  (vecInTmp6),
							(vecTmp_insel1) , (vecTmp_insel1) , *(xb_vec2Nx8 *)&dsel_2_6_3_7);

				IVP_MULPA2NX8(sum1,vecTmp0,IVP_MOV2NX8_FROMNX16(vecInTmp4),vecTmp1,IVP_MOV2NX8_FROMNX16(vecInTmp5));
				IVP_MULPA2NX8(sum1,vecTmp2,IVP_MOV2NX8_FROMNX16(vecInTmp6),vecTmp3,IVP_MOV2NX8_FROMNX16(vecInTmp7));

				IVP_DSELNX16( (vecInTmp9),  (vecInTmp8),
						(vecTmp_insel2) , (vecTmp_insel2) , *(xb_vec2Nx8 *)&dsel_0_4_1_5);
				IVP_DSELNX16( (vecInTmp11),  (vecInTmp10),
							(vecTmp_insel2) , (vecTmp_insel2) , *(xb_vec2Nx8 *)&dsel_2_6_3_7);

				IVP_MULPA2NX8(sum2,vecTmp0,IVP_MOV2NX8_FROMNX16(vecInTmp8),vecTmp1,IVP_MOV2NX8_FROMNX16(vecInTmp9));
				IVP_MULPA2NX8(sum2,vecTmp2,IVP_MOV2NX8_FROMNX16(vecInTmp10),vecTmp3,IVP_MOV2NX8_FROMNX16(vecInTmp11));


				IVP_DSELNX16( (vecInTmp13),  (vecInTmp12),
						(vecTmp_insel3) , (vecTmp_insel3) , *(xb_vec2Nx8 *)&dsel_0_4_1_5);
				IVP_DSELNX16( (vecInTmp15),  (vecInTmp14),
							(vecTmp_insel3) , (vecTmp_insel3) , *(xb_vec2Nx8 *)&dsel_2_6_3_7);

				IVP_MULPA2NX8(sum3,vecTmp0,IVP_MOV2NX8_FROMNX16(vecInTmp12),vecTmp1,IVP_MOV2NX8_FROMNX16(vecInTmp13));
				IVP_MULPA2NX8(sum3,vecTmp2,IVP_MOV2NX8_FROMNX16(vecInTmp14),vecTmp3,IVP_MOV2NX8_FROMNX16(vecInTmp15));

				dv  = IVP_SEL2NX8I(0, dv, IVP_SELI_8B_ROTATE_RIGHT_16);
				dv1  = IVP_SEL2NX8I(0, dv1, IVP_SELI_8B_ROTATE_RIGHT_16);
				dv2 = IVP_SEL2NX8I(0, dv2, IVP_SELI_8B_ROTATE_RIGHT_16);
				dv3 = IVP_SEL2NX8I(0, dv3, IVP_SELI_8B_ROTATE_RIGHT_16);
			  }

			  xb_vecN_2x32v acc64x32_0[4], acc64x32_1[4], acc64x32_2[4], acc64x32_3[4], bias64x32[4];

			  load64x32_depth32(depth, pBias + ch, bias64x32);
			  init64x32_depth32(acc64x32_0, bias64x32, pFixup + row);
			  init64x32_depth32(acc64x32_1, bias64x32, pFixup + row + 2);
			  init64x32_depth32(acc64x32_2, bias64x32, pFixup + row + 4);
			  init64x32_depth32(acc64x32_3, bias64x32, pFixup + row + 6);

			  add64x32(acc64x32_0, sum0);
			  add64x32(acc64x32_1, sum1);
			  add64x32(acc64x32_2, sum2);
			  add64x32(acc64x32_3, sum3);

			  xb_vec2Nx8U out0 = quant_acc64x32(acc64x32_0, quantScale, leftShift, rightShift, zeroPtOut, minLim, maxLim);
			  xb_vec2Nx8U out1 = quant_acc64x32(acc64x32_1, quantScale, leftShift, rightShift, zeroPtOut, minLim, maxLim);
			  xb_vec2Nx8U out2 = quant_acc64x32(acc64x32_2, quantScale, leftShift, rightShift, zeroPtOut, minLim, maxLim);
			  xb_vec2Nx8U out3 = quant_acc64x32(acc64x32_3, quantScale, leftShift, rightShift, zeroPtOut, minLim, maxLim);

			  IVP_SAV2NX8U_XP(out0, vaOut0, pvOut0, 2*depth);
			  IVP_SAV2NX8U_XP(out1, vaOut1, pvOut1, 2*depth);
			  IVP_SAV2NX8U_XP(out2, vaOut2, pvOut2, 2*depth);
			  IVP_SAV2NX8U_XP(out3, vaOut3, pvOut3, 2*depth);
			}

			IVP_SAPOS2NX8U_FP(vaOut0, pvOut0);
			IVP_SAPOS2NX8U_FP(vaOut1, pvOut1);
			IVP_SAPOS2NX8U_FP(vaOut2, pvOut2);
			IVP_SAPOS2NX8U_FP(vaOut3, pvOut3);
		  }
   }
   else
   {
  for (int row = 0; row < output_rows; row += 4)
  {
    valign vaOut0 = IVP_ZALIGN();
    valign vaOut1 = IVP_ZALIGN();
    valign vaOut2 = IVP_ZALIGN();
    valign vaOut3 = IVP_ZALIGN();
    pvOut0 = (xb_vec2Nx8U *) &pOut[(row) * output_depth];
    pvOut1 = (xb_vec2Nx8U *) &pOut[(row + 1) * output_depth];
    pvOut2 = (xb_vec2Nx8U *) &pOut[(row + 2) * output_depth];
    pvOut3 = (xb_vec2Nx8U *) &pOut[(row + 3) * output_depth];
#ifdef __XCC__
#pragma loop_count min=1
#endif
    for (int ch = 0; ch < output_depth; ch += 2 * XCHAL_IVPN_SIMD_WIDTH)
    {
      int depth = min(2 * XCHAL_IVPN_SIMD_WIDTH, output_depth - ch);

      pC0 = (xb_vec2Nx8 *) &pCoeff[ch * filter_size];
      pC1 = (xb_vec2Nx8 *) &pCoeff[ch * filter_size + depth * (filter_size / 2)];

      valign vaC0 = IVP_LA2NX8_PP(pC0);
      valign vaC1 = IVP_LA2NX8_PP(pC1);

      pRow0 = (xb_vec2Nx8 *) &pData[row * input_row_dwords];
		  pRow1 = (xb_vec2Nx8 *) &pData[row * input_row_dwords + 2 * input_row_dwords];

      valign vaRow0 = IVP_LA2NX8_PP(pRow0);
		  valign vaRow1 = IVP_LA2NX8_PP(pRow1);
      xb_vec2Nx24 sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
#ifdef __XCC__
#pragma loop_count min=1
#endif
      for (int j = 0; j < input_row_dwords; j += 2)
      {
			xb_vec2Nx8 c0, c1, c2, c3, dv,dv1;
			xb_vec2Nx8 c4,c5,c6,c7;
        IVP_LAV2NX8_XP(dv, vaRow0, pRow0, 16);
			IVP_LAV2NX8_XP(dv1, vaRow1, pRow1, 16);
        IVP_LAV2NX8_XP(c0, vaC0, pC0, depth);
        IVP_LAV2NX8_XP(c1, vaC0, pC0, depth);
        IVP_LAV2NX8_XP(c2, vaC1, pC1, depth);
        IVP_LAV2NX8_XP(c3, vaC1, pC1, depth);
			IVP_LAV2NX8_XP(c4, vaC0, pC0, depth);
			IVP_LAV2NX8_XP(c5, vaC0, pC0, depth);
			IVP_LAV2NX8_XP(c6, vaC1, pC1, depth);
			IVP_LAV2NX8_XP(c7, vaC1, pC1, depth);
        IVP_MULQA2N8XR8(sum0, c3, c2, c1, c0, extrN_2x32(dv, 0));
        IVP_MULQA2N8XR8(sum1, c3, c2, c1, c0, extrN_2x32(dv, 1));
			IVP_MULQA2N8XR8(sum2, c3, c2, c1, c0, extrN_2x32(dv1, 0));
			IVP_MULQA2N8XR8(sum3, c3, c2, c1, c0, extrN_2x32(dv1, 1));
			IVP_MULQA2N8XR8(sum0, c7, c6, c5, c4, extrN_2x32(dv, 2));
			IVP_MULQA2N8XR8(sum1, c7, c6, c5, c4, extrN_2x32(dv, 3));
			IVP_MULQA2N8XR8(sum2, c7, c6, c5, c4, extrN_2x32(dv1, 2));
			IVP_MULQA2N8XR8(sum3, c7, c6, c5, c4, extrN_2x32(dv1, 3));
		  }

		  xb_vecN_2x32v acc64x32_0[4], acc64x32_1[4], acc64x32_2[4], acc64x32_3[4], bias64x32[4];

		  load64x32(depth, pBias + ch, bias64x32);
		  init64x32(acc64x32_0, bias64x32, pFixup + row);
		  init64x32(acc64x32_1, bias64x32, pFixup + row + 1);
		  init64x32(acc64x32_2, bias64x32, pFixup + row + 2);
		  init64x32(acc64x32_3, bias64x32, pFixup + row + 3);

		  add64x32(acc64x32_0, sum0);
		  add64x32(acc64x32_1, sum1);
		  add64x32(acc64x32_2, sum2);
		  add64x32(acc64x32_3, sum3);

		  xb_vec2Nx8U out0 = quant_acc64x32(acc64x32_0, quantScale, leftShift, rightShift, zeroPtOut, minLim, maxLim);
		  xb_vec2Nx8U out1 = quant_acc64x32(acc64x32_1, quantScale, leftShift, rightShift, zeroPtOut, minLim, maxLim);
		  xb_vec2Nx8U out2 = quant_acc64x32(acc64x32_2, quantScale, leftShift, rightShift, zeroPtOut, minLim, maxLim);
		  xb_vec2Nx8U out3 = quant_acc64x32(acc64x32_3, quantScale, leftShift, rightShift, zeroPtOut, minLim, maxLim);

		  IVP_SAV2NX8U_XP(out0, vaOut0, pvOut0, depth);
		  IVP_SAV2NX8U_XP(out1, vaOut1, pvOut1, depth);
		  IVP_SAV2NX8U_XP(out2, vaOut2, pvOut2, depth);
		  IVP_SAV2NX8U_XP(out3, vaOut3, pvOut3, depth);
		}

		IVP_SAPOS2NX8U_FP(vaOut0, pvOut0);
		IVP_SAPOS2NX8U_FP(vaOut1, pvOut1);
		IVP_SAPOS2NX8U_FP(vaOut2, pvOut2);
		IVP_SAPOS2NX8U_FP(vaOut3, pvOut3);
	  }
   }

  return(XI_ERROR_STATUS());
}
