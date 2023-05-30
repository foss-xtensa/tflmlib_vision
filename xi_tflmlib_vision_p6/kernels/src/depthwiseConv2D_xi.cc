/*
 * Copyright (c) 2022 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of
 * Cadence Design Systems Inc.  They may be adapted and modified by bona fide
 * purchasers for internal use, but neither the original nor any adapted
 * or modified version may be disclosed or distributed to third parties
 * in any manner, medium, or form, in whole or in part, without the prior
 * written consent of Cadence Design Systems Inc.  This software and its
 * derivatives are to be executed solely on products incorporating a Cadence
 * Design Systems processor.
 */

#include "xi_cnn.h"
#include "xi_intrin.h"

#if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))

#ifdef IVP_PACKVNX48 /* Only available in Q7*/
#ifdef ENABLE_CONV_ASYMMETRIC_ROUNDING
#define CONV_PACK_AND_ROUNDING(hvecIn1, hvecIn2, vecRightShift, vecOut)      \
{                                                                            \
  /* Performs ASYMMETRIC ROUNDING Operation */                               \
  vecOut = IVP_PACKVNX48(IVP_CVT48SNX32(hvecIn2, hvecIn1), vecRightShift);   \
}
#else  /* #ifdef ENABLE_CONV_ASYMMETRIC_ROUNDING */
#ifdef IVP_DECNEGWNX48
#define CONV_PACK_AND_ROUNDING(hvecIn1, hvecIn2, vecRightShift, vecOut)      \
{                                                                            \
  xb_vecNx16 vecAdjust = 0;                                                  \
  IVP_ADDNX16T(vecAdjust, vecAdjust, 1, IVP_EQNX16(vecRightShift, 0));       \
  /* Performs SYMMETRIC ROUNDING Operation */                                \
  xb_vecNx48 wvec48AB      = IVP_CVT48SNX32(hvecIn2, hvecIn1);               \
  IVP_DECNEGWNX48(wvec48AB);                                                 \
  vecOut = IVP_PACKVNX48(wvec48AB, vecRightShift);                           \
  vboolN vbA = IVP_LTNX16(vecOut, 0);                                        \
  IVP_ADDNX16T(vecOut, vecOut, vecAdjust, vbA);                              \
}
#else
#define CONV_PACK_AND_ROUNDING(hvecIn1, hvecIn2, vecRightShift, vecOut)      \
{                                                                            \
  /* Performs SYMMETRIC ROUNDING Operation */                                \
  vboolN_2 vbA = IVP_LTN_2X32(hvecIn1, 0);                                   \
  vboolN_2 vbB = IVP_LTN_2X32(hvecIn2, 0);                                   \
  xb_vecN_2x32v hvecAbs1 = IVP_ABSN_2X32(hvecIn1);                           \
  xb_vecN_2x32v hvecAbs2 = IVP_ABSN_2X32(hvecIn2);                           \
  xb_vecNx48 wvec48AB = IVP_CVT48UNX32(hvecAbs2, hvecAbs1);                  \
  vecOut = IVP_PACKVNX48(wvec48AB, vecRightShift);                           \
  vboolN vbAB = IVP_JOINBN_2(vbB, vbA);                                      \
  IVP_SUBNX16T(vecOut, 0, vecOut, vbAB);                                     \
}
#endif
#endif
#define PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
        hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut) \
{                                                                                                              \
  xb_vecNx16 vecOutL, vecOutH;                                                                                 \
  /* Move the 24 bit accumulated sum to 32 bit vec registers. */                                               \
  xb_vecN_2x32v hvecIn1 = IVP_CVT32S2NX24LL(daccSum);                                                          \
  xb_vecN_2x32v hvecIn2 = IVP_CVT32S2NX24LH(daccSum);                                                          \
  xb_vecN_2x32v hvecIn3 = IVP_CVT32S2NX24HL(daccSum);                                                          \
  xb_vecN_2x32v hvecIn4 = IVP_CVT32S2NX24HH(daccSum);                                                          \
  hvecIn1 = IVP_ADDN_2X32(hvecIn1,hvecBias1);                                                                  \
  hvecIn2 = IVP_ADDN_2X32(hvecIn2,hvecBias2);                                                                  \
  hvecIn3 = IVP_ADDN_2X32(hvecIn3,hvecBias3);                                                                  \
  hvecIn4 = IVP_ADDN_2X32(hvecIn4,hvecBias4);                                                                  \
  /* accumulated sum is multiplied with output scale value and Packed by 31 - Left shift */                    \
  hvecIn1 = IVP_PACKVN_2X64W(IVP_MULN_2X32(hvecIn1, hvecScale1), IVP_SUBN_2X32(31, hvecLeftShift1));           \
  hvecIn2 = IVP_PACKVN_2X64W(IVP_MULN_2X32(hvecIn2, hvecScale2), IVP_SUBN_2X32(31, hvecLeftShift2));           \
  hvecIn3 = IVP_PACKVN_2X64W(IVP_MULN_2X32(hvecIn3, hvecScale3), IVP_SUBN_2X32(31, hvecLeftShift3));           \
  hvecIn4 = IVP_PACKVN_2X64W(IVP_MULN_2X32(hvecIn4, hvecScale4), IVP_SUBN_2X32(31, hvecLeftShift4));           \
  /* Perform Pack and Rounding */                                                                              \
  CONV_PACK_AND_ROUNDING(hvecIn1, hvecIn2, vecRightShift1, vecOutL);                                           \
  CONV_PACK_AND_ROUNDING(hvecIn3, hvecIn4, vecRightShift2, vecOutH);                                           \
  /* Add zeroPtOut */                                                                                          \
  xb_vecNx16 vecOutAddZP_L = IVP_ADDSNX16(vecOutL, (xb_vecNx16) zeroPtOut);                                    \
  xb_vecNx16 vecOutAddZP_H = IVP_ADDSNX16(vecOutH, (xb_vecNx16) zeroPtOut);                                    \
  /* Result is saturate to minLim and maxLim */                                                                \
  vecOutL = (IVP_MAXNX16(IVP_MINNX16(vecOutAddZP_L, (xb_vecNx16) maxLim), (xb_vecNx16) minLim));               \
  vecOutH = (IVP_MAXNX16(IVP_MINNX16(vecOutAddZP_H, (xb_vecNx16) maxLim), (xb_vecNx16) minLim));               \
  dvecOut = IVP_SEL2NX8I(IVP_MOV2NX8_FROMNX16(vecOutH), IVP_MOV2NX8_FROMNX16(vecOutL), IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0); \
}

#else // #ifdef IVP_PACKVNX48
#ifdef ENABLE_CONV_ASYMMETRIC_ROUNDING
#define CONV_PACK_AND_ROUNDING(hvecIn1, hvecIn2, vecRightShift, vecOut)                   \
{                                                                                         \
  /* Performs ASYMMETRIC ROUNDING Operation */                                            \
  xb_vecNx16 vecShiftL, vecShiftH;                                                        \
  xb_vecN_2x32v hvecShiftL, hvecShiftH;                                                   \
  IVP_DSELNX16I(vecShiftH, vecShiftL, 0, vecRightShift, IVP_DSELI_INTERLEAVE_1);          \
  hvecShiftL = IVP_MOVN_2X32_FROMNX16(vecShiftL);                                         \
  hvecShiftH = IVP_MOVN_2X32_FROMNX16(vecShiftH);                                         \
  hvecIn1    = IVP_ADDN_2X32(hvecIn1, IVP_SLLN_2X32(1, IVP_SUBN_2X32(hvecShiftL, 1)));    \
  hvecIn2    = IVP_ADDN_2X32(hvecIn2, IVP_SLLN_2X32(1, IVP_SUBN_2X32(hvecShiftH, 1)));    \
  hvecIn1    = IVP_SRAN_2X32(hvecIn1, hvecShiftL);                                        \
  hvecIn2    = IVP_SRAN_2X32(hvecIn2, hvecShiftH);                                        \
  xb_vecNx48 wvec48AB = IVP_CVT48SNX32(hvecIn2, hvecIn1);                                 \
  vecOut = IVP_PACKVRNX48(wvec48AB, 0);                                                   \
}
#else  /* #ifdef ENABLE_CONV_ASYMMETRIC_ROUNDING */
#define CONV_PACK_AND_ROUNDING(hvecIn1, hvecIn2, vecRightShift, vecOut)                   \
{                                                                                         \
  /* Performs SYMMETRIC ROUNDING Operation */                                             \
  xb_vecNx16 vecShiftL, vecShiftH;                                                        \
  xb_vecN_2x32v hvecShiftL, hvecShiftH, hvecAbs1, hvecAbs2;                               \
  hvecAbs1 = IVP_ABSN_2X32(hvecIn1);                                                      \
  hvecAbs2 = IVP_ABSN_2X32(hvecIn2);                                                      \
  IVP_DSELNX16I(vecShiftH, vecShiftL, 0, vecRightShift, IVP_DSELI_INTERLEAVE_1);          \
  hvecShiftL = IVP_MOVN_2X32_FROMNX16(vecShiftL);                                         \
  hvecShiftH = IVP_MOVN_2X32_FROMNX16(vecShiftH);                                         \
  hvecAbs1   = IVP_ADDN_2X32(hvecAbs1, IVP_SLLN_2X32(1, IVP_SUBN_2X32(hvecShiftL, 1)));   \
  hvecAbs2   = IVP_ADDN_2X32(hvecAbs2, IVP_SLLN_2X32(1, IVP_SUBN_2X32(hvecShiftH, 1)));   \
  hvecAbs1   = IVP_SRAN_2X32(hvecAbs1, hvecShiftL);                                       \
  hvecAbs2   = IVP_SRAN_2X32(hvecAbs2, hvecShiftH);                                       \
  hvecAbs1   = IVP_MULSGNN_2X32(hvecIn1, hvecAbs1);                                       \
  hvecAbs2   = IVP_MULSGNN_2X32(hvecIn2, hvecAbs2);                                       \
  xb_vecNx48 wvec48AB = IVP_CVT48SNX32(hvecAbs2, hvecAbs1);                               \
  vecOut = IVP_PACKVRNX48(wvec48AB, 0);                                                   \
}
#endif

#define PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
        hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut) \
{                                                                                                              \
  xb_vecNx16 vecOutL, vecOutH;                                                                                 \
  /* Move the 24 bit accumulated sum to 32 bit vec registers. */                                               \
  xb_vecN_2x32v hvecIn1 = IVP_CVT32S2NX24LL(daccSum);                                                          \
  xb_vecN_2x32v hvecIn2 = IVP_CVT32S2NX24LH(daccSum);                                                          \
  xb_vecN_2x32v hvecIn3 = IVP_CVT32S2NX24HL(daccSum);                                                          \
  xb_vecN_2x32v hvecIn4 = IVP_CVT32S2NX24HH(daccSum);                                                          \
  hvecIn1 = IVP_ADDN_2X32(hvecIn1,hvecBias1);                                                                  \
  hvecIn2 = IVP_ADDN_2X32(hvecIn2,hvecBias2);                                                                  \
  hvecIn3 = IVP_ADDN_2X32(hvecIn3,hvecBias3);                                                                  \
  hvecIn4 = IVP_ADDN_2X32(hvecIn4,hvecBias4);                                                                  \
  /* accumulated sum is multiplied with output scale value and Packed by 31 - Left shift */                    \
  hvecIn1 = IVP_SLSN_2X32(hvecIn1, hvecLeftShift1);                                                            \
  hvecIn2 = IVP_SLSN_2X32(hvecIn2, hvecLeftShift2);                                                            \
  hvecIn3 = IVP_SLSN_2X32(hvecIn3, hvecLeftShift3);                                                            \
  hvecIn4 = IVP_SLSN_2X32(hvecIn4, hvecLeftShift4);                                                            \
  xb_vecNx16U vecIn1 = IVP_MOVNX16_FROMN_2X32(hvecIn1);                                                        \
  xb_vecNx16U vecIn2 = IVP_MOVNX16_FROMN_2X32(hvecIn2);                                                        \
  xb_vecNx16U vecIn3 = IVP_MOVNX16_FROMN_2X32(hvecIn3);                                                        \
  xb_vecNx16U vecIn4 = IVP_MOVNX16_FROMN_2X32(hvecIn4);                                                        \
  xb_vecN_2x64w haccScale1 = IVP_MULUSN_2X16X32_0(vecIn1, hvecScale1);                                         \
  xb_vecN_2x64w haccScale2 = IVP_MULUSN_2X16X32_0(vecIn2, hvecScale2);                                         \
  xb_vecN_2x64w haccScale3 = IVP_MULUSN_2X16X32_0(vecIn3, hvecScale3);                                         \
  xb_vecN_2x64w haccScale4 = IVP_MULUSN_2X16X32_0(vecIn4, hvecScale4);                                         \
  IVP_MULAHN_2X16X32_1(haccScale1, vecIn1, hvecScale1);                                                        \
  IVP_MULAHN_2X16X32_1(haccScale2, vecIn2, hvecScale2);                                                        \
  IVP_MULAHN_2X16X32_1(haccScale3, vecIn3, hvecScale3);                                                        \
  IVP_MULAHN_2X16X32_1(haccScale4, vecIn4, hvecScale4);                                                        \
  hvecIn1 = IVP_PACKVRN_2X64W(haccScale1, 31);                                                                 \
  hvecIn2 = IVP_PACKVRN_2X64W(haccScale2, 31);                                                                 \
  hvecIn3 = IVP_PACKVRN_2X64W(haccScale3, 31);                                                                 \
  hvecIn4 = IVP_PACKVRN_2X64W(haccScale4, 31);                                                                 \
  /* Perform Pack and Rounding */                                                                              \
  CONV_PACK_AND_ROUNDING(hvecIn1, hvecIn2, vecRightShift1, vecOutL);                                           \
  CONV_PACK_AND_ROUNDING(hvecIn3, hvecIn4, vecRightShift2, vecOutH);                                           \
  /* Add zeroPtOut */                                                                                          \
  xb_vecNx16 vecOutAddZP_L = IVP_ADDSNX16(vecOutL, (xb_vecNx16) zeroPtOut);                                    \
  xb_vecNx16 vecOutAddZP_H = IVP_ADDSNX16(vecOutH, (xb_vecNx16) zeroPtOut);                                    \
  /* Result is saturate to minLim and maxLim */                                                                \
  vecOutL = (IVP_MAXNX16(IVP_MINNX16(vecOutAddZP_L, (xb_vecNx16) maxLim), (xb_vecNx16) minLim));               \
  vecOutH = (IVP_MAXNX16(IVP_MINNX16(vecOutAddZP_H, (xb_vecNx16) maxLim), (xb_vecNx16) minLim));               \
  dvecOut = IVP_SEL2NX8I(IVP_MOV2NX8_FROMNX16(vecOutH), IVP_MOV2NX8_FROMNX16(vecOutL), IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0); \
}
#endif

#define BIAS_LOAD_FIXUP_SUBTRACT(phvecBias1, phvecBias2,hvecBias1, hvecBias2, hvecBias3, hvecBias4, remCh1, remCh2, remCh3, remCh4)      \
{                                                                                       \
  valign vaBias;                                                                        \
  vaBias = IVP_LAN_2X32_PP(phvecBias1);                                                 \
  IVP_LAVN_2X32_XP(hvecBias1, vaBias, phvecBias1, remCh1);                              \
  IVP_LAVN_2X32_XP(hvecBias2, vaBias, phvecBias1, remCh2);                              \
  hvecBias1 = IVP_SUBN_2X32(hvecBias1, hvecFixLL);                                      \
  hvecBias2 = IVP_SUBN_2X32(hvecBias2, hvecFixLH);                                      \
  vaBias   = IVP_LAN_2X32_PP(phvecBias2);                                               \
  IVP_LAVN_2X32_XP(hvecBias3, vaBias, phvecBias2, remCh3);                              \
  IVP_LAVN_2X32_XP(hvecBias4, vaBias, phvecBias2, remCh4);                              \
  hvecBias3 = IVP_SUBN_2X32(hvecBias3, hvecFixHL);                                      \
  hvecBias4 = IVP_SUBN_2X32(hvecBias4, hvecFixHH);                                      \
}

#if defined(IVP_SAPOS2NX8_FPXP)
#define  FLUSH2NX8(vaOutData, pdvecOut, outPitch1, remX, pOut)         \
{                                                                      \
  IVP_SAPOS2NX8_FPXP(vaOutData, pdvecOut, outPitch1 - remX);           \
}
#else
#define  FLUSH2NX8(vaOutData, pdvecOut, outPitch1, remX, pOut)         \
{                                                                      \
  IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);                               \
  pOut += outPitch1;                                                   \
  pdvecOut = (xb_vec2Nx8 *)(pOut);                                     \
}
#endif

#define CALCULATE_FIXUP(daccSum, kHeight, kWidth, pCoeff)              \
{                                                                      \
  int32_t kw, kh;                                                      \
  xb_vec2Nx8 dvecCoeff_1, dvecCoeff_2;                                 \
  for (kh = 0; kh < kHeight; kh++)                                     \
  {                                                                    \
    int8_t *pIn = pCoeff + kh * coeffPitch2;                           \
    pdvecCoeff    = (xb_vec2Nx8 *)pIn;                                 \
    for (kw = 0; kw < (kWidth - 1); kw+= 2)                            \
    {                                                                  \
      IVP_LV2NX8_XP(dvecCoeff_1, pdvecCoeff, coeffPitch1);             \
      IVP_LV2NX8_XP(dvecCoeff_2, pdvecCoeff, coeffPitch1);             \
      IVP_ADDWA2NX8(daccSum, dvecCoeff_1, dvecCoeff_2);                \
    }                                                                  \
    if(kw < kWidth)                                                    \
    {                                                                  \
      IVP_LV2NX8_XP(dvecCoeff_1, pdvecCoeff, coeffPitch1);             \
      IVP_ADDWA2NX8(daccSum, dvecCoeff_1, 0);                          \
    }                                                                  \
  }                                                                    \
}
/*****************************************************************************
* Stride 2 Sub-variant
* When input stride is 2 this function is called
*  **************************************************************************/
void depthwiseConvolveAVQ2D_S_3x3j2_S8Ca2_MOD_DWH(const xi_pTile3D inTile,
                                                const xi_pTile3D coeffTile,
                                                const xi_pArray biasArray,
                                                const xi_pArray outScaleArray,
                                                const xi_pArray outShiftArray,
                                                xi_pTile3D outTile,
                                                const xi_cnna_conv_params *param)
{
  /* Getting parameters from the tile structures */
  const int32_t outW          = XI_TILE3D_GET_DIM2(outTile);
  const int32_t outH          = XI_TILE3D_GET_DIM3(outTile);
  const int32_t numCh         = XI_TILE3D_GET_DIM1(inTile);
  const int32_t inDataPitch1  = XI_TILE3D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2  = XI_TILE3D_GET_DIM2_PITCH(inTile);
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
  const int32_t coeffPitch1   = XI_TILE3D_GET_DIM1_PITCH(coeffTile);
  const int32_t coeffPitch2   = XI_TILE3D_GET_DIM2_PITCH(coeffTile);

  /* Kernel Size (DWH) */
  const int32_t kSizeU = XI_TILE3D_GET_DIM2(coeffTile);

  /* Convolution params */
  const uint8_t enableReLu = XI_CNNA_CONV_GET_FLAG_RELU(param);
  const uint8_t stride     = XI_CNNA_CONV_GET_STRIDE(param);
  const int16_t zeroPtOut  = XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param);
  const int16_t zeroPtIn  = XI_CNNA_CONV_GET_ZEROPT_INPUT(param);
  const int16_t reluMin    = XI_CNNA_CONV_GET_RELUMIN(param);
  const int16_t reluMax    = XI_CNNA_CONV_GET_RELUMAX(param);

  /* Data Pointers of input, output, coefficient and bias data */
  int8_t *pInData    = (int8_t *) XI_TILE3D_GET_DATA_PTR(inTile);
  int8_t *pOutData   = (int8_t *) XI_TILE3D_GET_DATA_PTR(outTile);
  int8_t *pCoeffData = (int8_t *) XI_TILE3D_GET_DATA_PTR(coeffTile);
  int32_t *pBiasData = (int32_t *) XI_ARRAY_GET_DATA_PTR(biasArray);
  int32_t *pOutScale = (int32_t *) XI_ARRAY_GET_DATA_PTR(outScaleArray);
  int8_t *pOutShift  = (int8_t *) XI_ARRAY_GET_DATA_PTR(outShiftArray);

  /* Move pointer to the start of the data (including edge) */
  pInData = &pInData[-((kSizeU / 2) * inDataPitch1 + (kSizeU / 2) * inDataPitch2)];

  /* Setting the limits for output data according to ReLu is enabled or not*/
  const int16_t minLim = enableReLu ? reluMin : SCHAR_MIN;
  const int16_t maxLim = enableReLu ? reluMax : SCHAR_MAX;

  /* Variable Declarations */
  int32_t ch, x, y;
  valign vaOutData = IVP_ZALIGN();
  /* Input and Output data Pointers */
  xb_vecN_2x32v* restrict phvecBias1;
  xb_vecN_2x32v* restrict phvecBias2;
  xb_vecN_2x32v* restrict phvecOutScale1;
  xb_vecN_2x32v* restrict phvecOutScale2;
  xb_vec2Nx8* restrict pdvecOutShift;
  xb_vec2Nx8* restrict pdvecCoeff;
  xb_vec2Nx8* restrict pdvecData1;
  xb_vec2Nx8* restrict pdvecData2;
  xb_vec2Nx8* restrict pdvecData3;
  xb_vec2Nx8* restrict pdvecOut;

  /* Input and Output data vectors */
  xb_vec2Nx24 daccSum, daccSum1;
  xb_vecN_2x32v hvecScale1, hvecScale2, hvecScale3, hvecScale4;
  xb_vec2Nx8 dvecCoeff11, dvecCoeff12, dvecCoeff13;
  xb_vec2Nx8 dvecCoeff21, dvecCoeff22, dvecCoeff23;
  xb_vec2Nx8 dvecCoeff31, dvecCoeff32, dvecCoeff33;
  xb_vec2Nx8 dvecData11, dvecData12, dvecData13;
  xb_vec2Nx8 dvecData21, dvecData22, dvecData23;
  xb_vec2Nx8 dvecData31, dvecData32, dvecData33;
  xb_vec2Nx8 dvecOut;

  int32_t vectorizationWidth = 2 * XCHAL_IVPN_SIMD_WIDTH;

  /* Kernel height and width loops are completely unrolled */
  /* Loops Start */
  for (ch = 0; ch < numCh; ch += vectorizationWidth)
  { /* walk across the channels */
    /* To handle corner case when number of channels
     * is not a multiple of  2 * XCHAL_IVPN_SIMD_WIDTH
     */
    int32_t remainingCh = XT_MIN((numCh - ch), vectorizationWidth);
    int32_t remBiasLoad = (remainingCh > XCHAL_IVPN_SIMD_WIDTH) ? 1 : 0;

    int32_t remCh1 = XT_MIN(((numCh - ch) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh2 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH / 2) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh3 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh4 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH * 3 / 2) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);

    /* Load OutScale values */
    phvecOutScale1 = (xb_vecN_2x32v*)(pOutScale + ch);
    phvecOutScale2 = (xb_vecN_2x32v*)(pOutScale + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
    valign vaOutScale = IVP_LAN_2X32_PP(phvecOutScale1);
    IVP_LAVN_2X32_XP(hvecScale1, vaOutScale, phvecOutScale1, remCh1);
    IVP_LAVN_2X32_XP(hvecScale2, vaOutScale, phvecOutScale1, remCh2);
    vaOutScale = IVP_LAN_2X32_PP(phvecOutScale2);
    IVP_LAVN_2X32_XP(hvecScale3, vaOutScale, phvecOutScale2, remCh3);
    IVP_LAVN_2X32_XP(hvecScale4, vaOutScale, phvecOutScale2, remCh4);

    /* Load OutShift values */
    xb_vec2Nx8 dvecShift;
    pdvecOutShift = (xb_vec2Nx8*)(pOutShift + ch);
    valign vaOutShift = IVP_LA2NX8_PP(pdvecOutShift);
    IVP_LAV2NX8_XP(dvecShift, vaOutShift, pdvecOutShift, remainingCh);

    /* Calculate left shift and right shift values */
    vbool2N vb2N = IVP_LT2NX8(dvecShift, 0);
    xb_vec2Nx8 dvecRightShift = IVP_MOV2NX8T(0, dvecShift, vb2N);
    xb_vec2Nx8 dvecLeftShift = 0;
    IVP_SUB2NX8T(dvecLeftShift, 0, dvecShift, vb2N);

    xb_vec2Nx8 dvecRightShiftL, dvecRightShiftH;
    IVP_DSEL2NX8I(dvecRightShiftH, dvecRightShiftL, 0, dvecRightShift, IVP_DSELI_8B_INTERLEAVE_1);

    xb_vecNx16 vecRightShift1 = IVP_MOVNX16_FROM2NX8(dvecRightShiftL);
    xb_vecNx16 vecRightShift2 = IVP_MOVNX16_FROM2NX8(dvecRightShiftH);

    xb_vec2Nx8 dvecLeftShiftL, dvecLeftShiftH;
    IVP_DSEL2NX8I(dvecLeftShiftH, dvecLeftShiftL, 0, dvecLeftShift, IVP_DSELI_8B_INTERLEAVE_1);

    xb_vecNx16 vecLeftShiftLL, vecLeftShiftLH, vecLeftShiftHL, vecLeftShiftHH;
    IVP_DSELNX16I(vecLeftShiftLH, vecLeftShiftLL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftL), IVP_DSELI_INTERLEAVE_1);
    IVP_DSELNX16I(vecLeftShiftHH, vecLeftShiftHL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftH), IVP_DSELI_INTERLEAVE_1);

    xb_vecN_2x32v hvecLeftShift1 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLL);
    xb_vecN_2x32v hvecLeftShift2 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLH);
    xb_vecN_2x32v hvecLeftShift3 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHL);
    xb_vecN_2x32v hvecLeftShift4 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHH);

    /* Pointer for Coefficient Load */
    pdvecCoeff = (xb_vec2Nx8 *) (pCoeffData + ch);

    /* 9 Coefficient Loads */
    IVP_LV2NX8_XP(dvecCoeff11, pdvecCoeff, coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff12, pdvecCoeff, coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff13, pdvecCoeff, coeffPitch2 - 2 * coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff21, pdvecCoeff, coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff22, pdvecCoeff, coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff23, pdvecCoeff, coeffPitch2 - 2 * coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff31, pdvecCoeff, coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff32, pdvecCoeff, coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff33, pdvecCoeff, coeffPitch1);
    /* Calculate Fixup terms*/
    daccSum = IVP_ADDW2NX8(dvecCoeff11, dvecCoeff12);
    IVP_ADDWA2NX8(daccSum, dvecCoeff13, dvecCoeff21);
    IVP_ADDWA2NX8(daccSum, dvecCoeff22, dvecCoeff23);
    IVP_ADDWA2NX8(daccSum, dvecCoeff31, dvecCoeff32);
    IVP_ADDWA2NX8(daccSum, dvecCoeff33, 0);
    xb_vecNx16 vecSumL = IVP_CVT16S2NX24L(daccSum);
    xb_vecNx16 vecSumH = IVP_CVT16S2NX24H(daccSum);
    xb_vecNx48 accFixupL = IVP_MULNX16(vecSumL, (xb_vecNx16)zeroPtIn);
    xb_vecNx48 accFixupH = IVP_MULNX16(vecSumH, (xb_vecNx16)zeroPtIn);
    xb_vecN_2x32v hvecFixLL = IVP_CVT32SNX48L(accFixupL);
    xb_vecN_2x32v hvecFixLH = IVP_CVT32SNX48H(accFixupL);
    xb_vecN_2x32v hvecFixHL = IVP_CVT32SNX48L(accFixupH);
    xb_vecN_2x32v hvecFixHH = IVP_CVT32SNX48H(accFixupH);
    /* Initialize accumulators with bias values */
    phvecBias1 = (xb_vecN_2x32v*)(pBiasData + ch);
    phvecBias2 = (xb_vecN_2x32v*)(pBiasData + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
    xb_vecN_2x32v hvecBias1, hvecBias2, hvecBias3, hvecBias4;
    BIAS_LOAD_FIXUP_SUBTRACT(phvecBias1, phvecBias2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, remCh1, remCh2, remCh3, remCh4);
    for (y = 0; y < outH; y++)  /* along output Height */
    {
      /* Input Data Pointers */
      int8_t *pData = pInData + ch + y * stride * inDataPitch2;

      pdvecData1 = (xb_vec2Nx8 *) pData;
      pdvecData2 = (xb_vec2Nx8 *) (pData + inDataPitch1);
      pdvecData3 = (xb_vec2Nx8 *)(pData + 2 * inDataPitch1);
      /* Output Data Pointer */
      int8_t *pOut = pOutData + (y * outDataPitch2) + ch;
      pdvecOut = (xb_vec2Nx8 *)(pOut);

      /* Input loads*/
      /* ky = 0*/
      valign vaData = IVP_LA2NX8_PP(pdvecData1);
      IVP_LA2NX8_XP(dvecData11, vaData, pdvecData1, inDataPitch2);
      /* ky = 1*/
      vaData = IVP_LA2NX8_PP(pdvecData1);
      IVP_LA2NX8_XP(dvecData21, vaData, pdvecData1, inDataPitch2);
      /* ky = 2*/
      vaData = IVP_LA2NX8_PP(pdvecData1);
      IVP_LA2NX8_XP(dvecData31, vaData, pdvecData1, inDataPitch1);
      for (x = 0; x < outW; x++)  /* along output width */
      {
        /* Initialize accumulator with bias values */
        daccSum1 = 0;

        /* ky = 0*/
        valign vaData2 = IVP_LA2NX8_PP(pdvecData2);
        IVP_LA2NX8_XP(dvecData12, vaData2, pdvecData2, inDataPitch2);
        valign vaData3 = IVP_LA2NX8_PP(pdvecData3);
        IVP_LA2NX8_XP(dvecData13, vaData3, pdvecData3, inDataPitch2);

        /* ky = 1*/
        vaData2 = IVP_LA2NX8_PP(pdvecData2);
        IVP_LA2NX8_XP(dvecData22, vaData2, pdvecData2, inDataPitch2);
        vaData3 = IVP_LA2NX8_PP(pdvecData3);
        IVP_LA2NX8_XP(dvecData23, vaData3, pdvecData3, inDataPitch2);

        /* ky = 2*/
        vaData2 = IVP_LA2NX8_PP(pdvecData2);
        IVP_LA2NX8_XP(dvecData32, vaData2, pdvecData2, 2 * (inDataPitch1 - inDataPitch2));
        vaData3 = IVP_LA2NX8_PP(pdvecData3);
        IVP_LA2NX8_XP(dvecData33, vaData3, pdvecData3, 2 * (inDataPitch1 - inDataPitch2));

        /*Multiply and accumulate input data vector and coeff vector */
        IVP_MULPA2NX8(daccSum1, dvecCoeff11, dvecData11, dvecCoeff12, dvecData12);
        IVP_MULPA2NX8(daccSum1, dvecCoeff13, dvecData13, dvecCoeff21, dvecData21);
        IVP_MULPA2NX8(daccSum1, dvecCoeff22, dvecData22, dvecCoeff23, dvecData23);
        IVP_MULPA2NX8(daccSum1, dvecCoeff31, dvecData31, dvecCoeff32, dvecData32);
        IVP_MULA2NX8 (daccSum1, dvecCoeff33, dvecData33);

        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum1, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut);
        /* Store the output vecOut1 along the output depth */
        IVP_SAV2NX8_XP(dvecOut, vaOutData, pdvecOut, remainingCh);
        FLUSH2NX8(vaOutData, pdvecOut, outDataPitch1, remainingCh, pOut);

        dvecData11 = dvecData13;
        dvecData21 = dvecData23;
        dvecData31 = dvecData33;
      } /* End for (x = 0; x < outW; x ++) */
    }   /* End for (y = 0; y < outH; y ++) */
  }     /* End for (ch = 0; ch < numCh; ch += 2 * XCHAL_IVPN_SIMD_WIDTH) */
}

/*****************************************************************************
* Stride 4 Sub-variant
* When input stride is 4 this function is called
*  **************************************************************************/
void depthwiseConvolveAVQ2D_S_3x3j4_S8Ca2_MOD_DWH(const xi_pTile3D inTile,
                                                const xi_pTile3D coeffTile,
                                                const xi_pArray biasArray,
                                                const xi_pArray outScaleArray,
                                                const xi_pArray outShiftArray,
                                                xi_pTile3D outTile,
                                                const xi_cnna_conv_params *param)
{
  /* Getting parameters from the tile structures */
  const int32_t outW          = XI_TILE3D_GET_DIM2(outTile);
  const int32_t outH          = XI_TILE3D_GET_DIM3(outTile);
  const int32_t numCh         = XI_TILE3D_GET_DIM1(inTile);
  const int32_t inDataPitch1  = XI_TILE3D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2  = XI_TILE3D_GET_DIM2_PITCH(inTile);
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
  const int32_t coeffPitch1   = XI_TILE3D_GET_DIM1_PITCH(coeffTile);
  const int32_t coeffPitch2   = XI_TILE3D_GET_DIM2_PITCH(coeffTile);

  /* Kernel Size (DWH) */
  const int32_t kSizeU = XI_TILE3D_GET_DIM2(coeffTile);

  /* Convolution params */
  const uint8_t enableReLu = XI_CNNA_CONV_GET_FLAG_RELU(param);
  const uint8_t stride     = XI_CNNA_CONV_GET_STRIDE(param);
  const int16_t zeroPtOut  = XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param);
  const int16_t zeroPtIn   = XI_CNNA_CONV_GET_ZEROPT_INPUT(param);
  const int16_t reluMin    = XI_CNNA_CONV_GET_RELUMIN(param);
  const int16_t reluMax    = XI_CNNA_CONV_GET_RELUMAX(param);

  /* Data Pointers of input, output, coefficient and bias data */
  int8_t *pInData    = (int8_t *) XI_TILE3D_GET_DATA_PTR(inTile);
  int8_t *pOutData   = (int8_t *) XI_TILE3D_GET_DATA_PTR(outTile);
  int8_t *pCoeffData = (int8_t *) XI_TILE3D_GET_DATA_PTR(coeffTile);
  int32_t *pBiasData = (int32_t *) XI_ARRAY_GET_DATA_PTR(biasArray);
  int32_t *pOutScale = (int32_t *) XI_ARRAY_GET_DATA_PTR(outScaleArray);
  int8_t *pOutShift  = (int8_t *) XI_ARRAY_GET_DATA_PTR(outShiftArray);

  /* Move pointer to the start of the data (including edge) */
  pInData = &pInData[-((kSizeU / 2) * inDataPitch1 + (kSizeU / 2) * inDataPitch2)];

  /* Setting the limits for output data according to ReLu is enabled or not*/
  const int16_t minLim = enableReLu ? reluMin : SCHAR_MIN;
  const int16_t maxLim = enableReLu ? reluMax : SCHAR_MAX;

  /* Variable Declarations */
  int32_t ch, x, y;
  valign vaOutData = IVP_ZALIGN();
  /* Input and Output data Pointers */
  xb_vecN_2x32v* restrict phvecBias1;
  xb_vecN_2x32v* restrict phvecBias2;
  xb_vecN_2x32v* restrict phvecOutScale1;
  xb_vecN_2x32v* restrict phvecOutScale2;
  xb_vec2Nx8* restrict pdvecOutShift;
  xb_vec2Nx8* restrict pdvecCoeff;
  xb_vec2Nx8* restrict pdvecData1;
  xb_vec2Nx8* restrict pdvecData2;
  xb_vec2Nx8* restrict pdvecData3;
  xb_vec2Nx8* restrict pdvecOut;

  /* Input and Output data Vectors */
  xb_vec2Nx24 daccSum1;
  xb_vec2Nx8 dvecCoeff11, dvecCoeff12, dvecCoeff13;
  xb_vec2Nx8 dvecCoeff21, dvecCoeff22, dvecCoeff23;
  xb_vec2Nx8 dvecCoeff31, dvecCoeff32, dvecCoeff33;
  xb_vec2Nx8 dvecData11, dvecData12, dvecData13;
  xb_vec2Nx8 dvecData21, dvecData22, dvecData23;
  xb_vec2Nx8 dvecData31, dvecData32, dvecData33;
  xb_vec2Nx8 dvecOut;
  xb_vecN_2x32v hvecScale1, hvecScale2, hvecScale3, hvecScale4;
  int32_t vectorizationWidth = 2 * XCHAL_IVPN_SIMD_WIDTH;

  /* Kernel height and width loops are completely unrolled */
  /* Loops Start */
  for (ch = 0; ch < numCh; ch += vectorizationWidth)/* Along Output Channel */
  { /* walk across the channels */
    /* To handle corner case when number of channels
     * is not a multiple of  2 * XCHAL_IVPN_SIMD_WIDTH*/
    int32_t remainingCh = XT_MIN((numCh - ch), vectorizationWidth);
    int32_t remBiasLoad = (remainingCh > XCHAL_IVPN_SIMD_WIDTH) ? 1 : 0;

    int32_t remCh1 = XT_MIN(((numCh - ch) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh2 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH / 2) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh3 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh4 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH * 3 / 2) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);

    /* Load OutScale values */
    phvecOutScale1 = (xb_vecN_2x32v*)(pOutScale + ch);
    phvecOutScale2 = (xb_vecN_2x32v*)(pOutScale + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
    valign vaOutScale = IVP_LAN_2X32_PP(phvecOutScale1);
    IVP_LAVN_2X32_XP(hvecScale1, vaOutScale, phvecOutScale1, remCh1);
    IVP_LAVN_2X32_XP(hvecScale2, vaOutScale, phvecOutScale1, remCh2);
    vaOutScale = IVP_LAN_2X32_PP(phvecOutScale2);
    IVP_LAVN_2X32_XP(hvecScale3, vaOutScale, phvecOutScale2, remCh3);
    IVP_LAVN_2X32_XP(hvecScale4, vaOutScale, phvecOutScale2, remCh4);

    /* Load OutShift values */
    xb_vec2Nx8 dvecShift;
    pdvecOutShift = (xb_vec2Nx8*)(pOutShift + ch);
    valign vaOutShift = IVP_LA2NX8_PP(pdvecOutShift);
    IVP_LAV2NX8_XP(dvecShift, vaOutShift, pdvecOutShift, remainingCh);

    /* Calculate left shift and right shift values */
    vbool2N vb2N = IVP_LT2NX8(dvecShift, 0);
    xb_vec2Nx8 dvecRightShift = IVP_MOV2NX8T(0, dvecShift, vb2N);
    xb_vec2Nx8 dvecLeftShift = 0;
    IVP_SUB2NX8T(dvecLeftShift, 0, dvecShift, vb2N);

    xb_vec2Nx8 dvecRightShiftL, dvecRightShiftH;
    IVP_DSEL2NX8I(dvecRightShiftH, dvecRightShiftL, 0, dvecRightShift, IVP_DSELI_8B_INTERLEAVE_1);

    xb_vecNx16 vecRightShift1 = IVP_MOVNX16_FROM2NX8(dvecRightShiftL);
    xb_vecNx16 vecRightShift2 = IVP_MOVNX16_FROM2NX8(dvecRightShiftH);

    xb_vec2Nx8 dvecLeftShiftL, dvecLeftShiftH;
    IVP_DSEL2NX8I(dvecLeftShiftH, dvecLeftShiftL, 0, dvecLeftShift, IVP_DSELI_8B_INTERLEAVE_1);

    xb_vecNx16 vecLeftShiftLL, vecLeftShiftLH, vecLeftShiftHL, vecLeftShiftHH;
    IVP_DSELNX16I(vecLeftShiftLH, vecLeftShiftLL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftL), IVP_DSELI_INTERLEAVE_1);
    IVP_DSELNX16I(vecLeftShiftHH, vecLeftShiftHL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftH), IVP_DSELI_INTERLEAVE_1);

    xb_vecN_2x32v hvecLeftShift1 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLL);
    xb_vecN_2x32v hvecLeftShift2 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLH);
    xb_vecN_2x32v hvecLeftShift3 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHL);
    xb_vecN_2x32v hvecLeftShift4 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHH);

    /* Pointer for Coefficient Load */
    pdvecCoeff = (xb_vec2Nx8 *) (pCoeffData + ch);

    /* 9 Coefficient Loads */
    IVP_LV2NX8_XP(dvecCoeff11, pdvecCoeff, coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff12, pdvecCoeff, coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff13, pdvecCoeff, coeffPitch2 - 2 * coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff21, pdvecCoeff, coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff22, pdvecCoeff, coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff23, pdvecCoeff, coeffPitch2 - 2 * coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff31, pdvecCoeff, coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff32, pdvecCoeff, coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff33, pdvecCoeff, coeffPitch1);
    /* Calculate Fixup terms*/
    xb_vec2Nx24 daccSum0;
    daccSum0 = IVP_ADDW2NX8(dvecCoeff11, dvecCoeff12);
    IVP_ADDWA2NX8(daccSum0, dvecCoeff13, dvecCoeff21);
    IVP_ADDWA2NX8(daccSum0, dvecCoeff22, dvecCoeff23);
    IVP_ADDWA2NX8(daccSum0, dvecCoeff31, dvecCoeff32);
    IVP_ADDWA2NX8(daccSum0, dvecCoeff33, 0);
    xb_vecNx16 vecSumL = IVP_CVT16S2NX24L(daccSum0);
    xb_vecNx16 vecSumH = IVP_CVT16S2NX24H(daccSum0);
    xb_vecNx48 accFixupL = IVP_MULNX16(vecSumL, (xb_vecNx16)zeroPtIn);
    xb_vecNx48 accFixupH = IVP_MULNX16(vecSumH, (xb_vecNx16)zeroPtIn);
    xb_vecN_2x32v hvecFixLL = IVP_CVT32SNX48L(accFixupL);
    xb_vecN_2x32v hvecFixLH = IVP_CVT32SNX48H(accFixupL);
    xb_vecN_2x32v hvecFixHL = IVP_CVT32SNX48L(accFixupH);
    xb_vecN_2x32v hvecFixHH = IVP_CVT32SNX48H(accFixupH);
    /* Initialize accumulators with bias values */
    phvecBias1 = (xb_vecN_2x32v*)(pBiasData + ch);
    phvecBias2 = (xb_vecN_2x32v*)(pBiasData + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);

    xb_vecN_2x32v hvecBias1, hvecBias2, hvecBias3, hvecBias4;
    BIAS_LOAD_FIXUP_SUBTRACT(phvecBias1, phvecBias2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, remCh1, remCh2, remCh3, remCh4);

    for (y = 0; y < outH; y++) /* Along Output Height */
    {
      /* Input Data Pointers */
      int8_t *pData = pInData + ch + y * stride * inDataPitch2;
      pdvecData1 = (xb_vec2Nx8 *) (pData + 0 * inDataPitch2);
      pdvecData2 = (xb_vec2Nx8 *) (pData + 1 * inDataPitch2);
      pdvecData3 = (xb_vec2Nx8 *) (pData + 2 * inDataPitch2);
      /* Output Data pointer */
      int8_t *pOut = pOutData + (y * outDataPitch2 + ch);
      pdvecOut = (xb_vec2Nx8 *)(pOut);
      for (x = 0; x < outW; x++) /* Along Output Width */
      {
        /* Initialize accumulator with bias values */
        daccSum1 = 0;
        /* Input loads*/
        /* ky = 0*/
        valign vaData1 = IVP_LA2NX8_PP(pdvecData1);
        IVP_LA2NX8_XP(dvecData11, vaData1, pdvecData1, inDataPitch1);
        vaData1 = IVP_LA2NX8_PP(pdvecData1);
        IVP_LA2NX8_XP(dvecData12, vaData1, pdvecData1, inDataPitch1);
        vaData1 = IVP_LA2NX8_PP(pdvecData1);
        IVP_LA2NX8_XP(dvecData13, vaData1, pdvecData1, 2*inDataPitch1);

        /* ky = 1*/
        valign vaData2 = IVP_LA2NX8_PP(pdvecData2);
        IVP_LA2NX8_XP(dvecData21, vaData2, pdvecData2, inDataPitch1);
        vaData2 = IVP_LA2NX8_PP(pdvecData2);
        IVP_LA2NX8_XP(dvecData22, vaData2, pdvecData2, inDataPitch1);
        vaData2 = IVP_LA2NX8_PP(pdvecData2);
        IVP_LA2NX8_XP(dvecData23, vaData2, pdvecData2, 2*inDataPitch1);

        /* ky = 2*/
        valign vaData3 = IVP_LA2NX8_PP(pdvecData3);
        IVP_LA2NX8_XP(dvecData31, vaData3, pdvecData3, inDataPitch1);
        vaData3 = IVP_LA2NX8_PP(pdvecData3);
        IVP_LA2NX8_XP(dvecData32, vaData3, pdvecData3, inDataPitch1);
        vaData3 = IVP_LA2NX8_PP(pdvecData3);
        IVP_LA2NX8_XP(dvecData33, vaData3, pdvecData3, 2*inDataPitch1);

        /*Multiply and accumulate input data vector and coeff vector */
        IVP_MULPA2NX8(daccSum1, dvecCoeff11, dvecData11, dvecCoeff12, dvecData12);
        IVP_MULPA2NX8(daccSum1, dvecCoeff13, dvecData13, dvecCoeff21, dvecData21);
        IVP_MULPA2NX8(daccSum1, dvecCoeff22, dvecData22, dvecCoeff23, dvecData23);
        IVP_MULPA2NX8(daccSum1, dvecCoeff31, dvecData31, dvecCoeff32, dvecData32);
        IVP_MULA2NX8(daccSum1, dvecCoeff33, dvecData33);

        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum1, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut);
        /* Store the output vecOut1 along the output depth */
        IVP_SAV2NX8_XP(dvecOut, vaOutData, pdvecOut, remainingCh);
        FLUSH2NX8(vaOutData, pdvecOut, outDataPitch1, remainingCh, pOut);
      } /* End for (x = 0; x < outW; x ++) */
    }   /* End for (y = 0; y < outH; y ++) */
  }     /* End for (ch = 0; ch < numCh; ch += 2 * XCHAL_IVPN_SIMD_WIDTH) */
}

/*****************************************************************************
* Stride 1 FOLD Sub-variant
* When input depth <= 32 this function is called
*  **************************************************************************/
void depthwiseConvolveAVQ2D_S_3x3_S8Ca2_MOD_DWH_FOLD(const xi_pTile3D inTile,
                                                     const xi_pTile3D coeffTile,
                                                     const xi_pArray biasArray,
                                                     const xi_pArray outScaleArray,
                                                     const xi_pArray outShiftArray,
                                                     xi_pTile3D outTile,
                                                     const xi_cnna_conv_params *param)
{
  /* Getting parameters from the tile structures */
  const int32_t numCh         = XI_TILE3D_GET_DIM1(inTile);
  const int32_t outW          = XI_TILE3D_GET_DIM2(outTile);
  const int32_t outH          = XI_TILE3D_GET_DIM3(outTile);
  const int32_t inDataPitch1  = XI_TILE3D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2  = XI_TILE3D_GET_DIM2_PITCH(inTile);
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
  const int32_t coeffPitch1   = XI_TILE3D_GET_DIM1_PITCH(coeffTile);
  const int32_t coeffPitch2   = XI_TILE3D_GET_DIM2_PITCH(coeffTile);

  /* Convolution params */
  const uint8_t enableReLu    = XI_CNNA_CONV_GET_FLAG_RELU(param);
  const int16_t zeroPtOut     = XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param);
  const int16_t zeroPtIn      = XI_CNNA_CONV_GET_ZEROPT_INPUT(param);
  const int16_t reluMin       = XI_CNNA_CONV_GET_RELUMIN(param);
  const int16_t reluMax       = XI_CNNA_CONV_GET_RELUMAX(param);

  /* Setting the limits for output data according to ReLu is enabled or not*/
  const int16_t minLim = enableReLu ? reluMin : SCHAR_MIN;
  const int16_t maxLim = enableReLu ? reluMax : SCHAR_MAX;
  /* Kernel Size (DWH) */
  const int32_t kSizeU = XI_TILE3D_GET_DIM2(coeffTile);

  /* Data Pointers of input, output, coefficient and bias data */
  int8_t *pInData    = (int8_t *) XI_TILE3D_GET_DATA_PTR(inTile);
  int8_t *pOutData   = (int8_t *) XI_TILE3D_GET_DATA_PTR(outTile);
  int8_t *pCoeffData = (int8_t *) XI_TILE3D_GET_DATA_PTR(coeffTile);
  int32_t *pBiasData = (int32_t *)XI_ARRAY_GET_DATA_PTR(biasArray);
  int32_t *pOutScale = (int32_t *)XI_ARRAY_GET_DATA_PTR(outScaleArray);
  int8_t *pOutShift  = (int8_t *)XI_ARRAY_GET_DATA_PTR(outShiftArray);

  /* Move pointer to the start of the data (including edge) */
  pInData = &pInData[-((kSizeU / 2) * inDataPitch1 + (kSizeU / 2) * inDataPitch2)];

  /* Input and Output data Pointers */
  xb_vecN_2x32v* restrict phvecBias;
  xb_vecN_2x32v* restrict phvecOutScale;
  xb_vec2Nx8* restrict pdvecOutShift;
  xb_vec2Nx8* restrict pdvecCoeff;
  xb_vec2Nx8* restrict pdvecData1;
  xb_vec2Nx8* restrict pdvecData2;
  xb_vec2Nx8* restrict pdvecData3;
  xb_vec2Nx8* restrict pdvecOut;
  /* Input and Output data Pointers */
  xb_vec2Nx8 dvecData11, dvecData12, dvecData13;
  xb_vec2Nx8 dvecData21, dvecData22, dvecData23;
  xb_vec2Nx8 dvecData31, dvecData32, dvecData33;
  xb_vec2Nx8 dvecOut;
  xb_vec2Nx8 dvecCoeff11, dvecCoeff12, dvecCoeff13;
  xb_vec2Nx8 dvecCoeff21, dvecCoeff22, dvecCoeff23;
  xb_vec2Nx8 dvecCoeff31, dvecCoeff32, dvecCoeff33;
  xb_vec2Nx24 daccSum1;

  /* Variable Declarations */
  int32_t x, y;
  int32_t vectorizationWidth = 2 * XCHAL_IVPN_SIMD_WIDTH;
  /* Sequence for select Pattern */
  xb_vec2Nx8 dvecSeq1 = IVP_ADD2NX8(IVP_SEQ2NX8(), numCh);
  vbool2N vbl1 = IVP_GE2NX8(dvecSeq1, numCh * 2);
  IVP_SUB2NX8T(dvecSeq1, dvecSeq1, numCh * 2, vbl1);
  IVP_ADD2NX8T(dvecSeq1, dvecSeq1, XCHAL_IVPN_SIMD_WIDTH * 2, vbl1);

  xb_vec2Nx8 dvecSeq2 = IVP_SEQ2NX8();
  vbl1 = IVP_GE2NX8(dvecSeq2, numCh);
  IVP_SUB2NX8T(dvecSeq2, dvecSeq2, numCh, vbl1);

  valign vaOutData = IVP_ZALIGN();
  if ((numCh == 1) && (inDataPitch1 == 1) && (outDataPitch1 == 1))
  {
    /* Pointer for Coefficient Load */
    pdvecCoeff = (xb_vec2Nx8 *) (pCoeffData);
    /* 9 Coefficient Loads */
    dvecCoeff11 = (xb_vec2Nx8) pCoeffData[0 * coeffPitch2 + 0 * coeffPitch1];
    dvecCoeff12 = (xb_vec2Nx8) pCoeffData[0 * coeffPitch2 + 1 * coeffPitch1];
    dvecCoeff13 = (xb_vec2Nx8) pCoeffData[0 * coeffPitch2 + 2 * coeffPitch1];
    dvecCoeff21 = (xb_vec2Nx8) pCoeffData[1 * coeffPitch2 + 0 * coeffPitch1];
    dvecCoeff22 = (xb_vec2Nx8) pCoeffData[1 * coeffPitch2 + 1 * coeffPitch1];
    dvecCoeff23 = (xb_vec2Nx8) pCoeffData[1 * coeffPitch2 + 2 * coeffPitch1];
    dvecCoeff31 = (xb_vec2Nx8) pCoeffData[2 * coeffPitch2 + 0 * coeffPitch1];
    dvecCoeff32 = (xb_vec2Nx8) pCoeffData[2 * coeffPitch2 + 1 * coeffPitch1];
    dvecCoeff33 = (xb_vec2Nx8)pCoeffData[2 * coeffPitch2 + 2 * coeffPitch1];
    /* Calculate Fixup terms*/
    xb_vec2Nx24 daccSum0;
    daccSum0 = IVP_ADDW2NX8(dvecCoeff11, dvecCoeff12);
    IVP_ADDWA2NX8(daccSum0, dvecCoeff13, dvecCoeff21);
    IVP_ADDWA2NX8(daccSum0, dvecCoeff22, dvecCoeff23);
    IVP_ADDWA2NX8(daccSum0, dvecCoeff31, dvecCoeff32);
    IVP_ADDWA2NX8(daccSum0, dvecCoeff33, 0);
    xb_vecNx16 vecSum   = IVP_CVT16S2NX24L(daccSum0);
    xb_vecNx48 accFixup = IVP_MULNX16(vecSum, (xb_vecNx16)zeroPtIn);
    xb_vecN_2x32v hvecFixUp = IVP_CVT32SNX48L(accFixup);
    /* Load Bias Value */
    xb_vecN_2x32v hvecBias = pBiasData[0];
    hvecBias = IVP_SUBN_2X32(hvecBias, hvecFixUp);

    xb_vecN_2x32v hvecBias1 = hvecBias; xb_vecN_2x32v hvecBias2 = hvecBias;
    xb_vecN_2x32v hvecBias3 = hvecBias; xb_vecN_2x32v hvecBias4 = hvecBias;

    /* Load Scale Value */
    xb_vecN_2x32v hvecScale1 = pOutScale[0];
    xb_vecN_2x32v hvecScale2 = pOutScale[0];
    xb_vecN_2x32v hvecScale3 = pOutScale[0];
    xb_vecN_2x32v hvecScale4 = pOutScale[0];
    /* Load Shift Value */
    int32_t outShift   = pOutShift[0];
    int32_t leftShift = outShift < 0 ? -outShift : 0;
    int32_t rightShift = outShift < 0 ? 0 : outShift;
    /* Calculate Right Shift Value */
    xb_vecNx16 vecRightShift1 = (xb_vecNx16)rightShift;
    xb_vecNx16 vecRightShift2 = (xb_vecNx16)rightShift;
    /* Calculate Left Shift Value */
    xb_vecN_2x32v hvecLeftShift1 = (xb_vecN_2x32v)leftShift;
    xb_vecN_2x32v hvecLeftShift2 = (xb_vecN_2x32v)leftShift;
    xb_vecN_2x32v hvecLeftShift3 = (xb_vecN_2x32v)leftShift;
    xb_vecN_2x32v hvecLeftShift4 = (xb_vecN_2x32v)leftShift;
    for (x = 0; x < outW; x += vectorizationWidth)    /* Along output Width */
    {
      int32_t remX = XT_MIN((outW - x), vectorizationWidth);
      /* Input Data Pointers */
      int8_t *pData = (pInData + x);
      pdvecData1 = (xb_vec2Nx8 *) pData;
      pdvecData2 = (xb_vec2Nx8 *) (pData + inDataPitch2);
      /* load first line */
      valign vaData1 = IVP_LA2NX8_PP(pdvecData1);
      xb_vec2Nx8 dvecData10, dvecData1N;
      IVP_LAV2NX8_XP(dvecData10, vaData1, pdvecData1, inDataPitch2 - x);
      IVP_LAV2NX8_XP(dvecData1N, vaData1, pdvecData1, inDataPitch2 - x - vectorizationWidth);

      /* load second line */
      valign vaData2 = IVP_LA2NX8_PP(pdvecData2);
      xb_vec2Nx8 dvecData20, dvecData2N;
      IVP_LAV2NX8_XP(dvecData20, vaData2, pdvecData2, inDataPitch2 - x);
      IVP_LAV2NX8_XP(dvecData2N, vaData2, pdvecData2, inDataPitch2 - x - vectorizationWidth);
      /* Select input data */
      dvecData11 = dvecData10;
      dvecData12 = IVP_SEL2NX8I(dvecData1N, dvecData10, IVP_SELI_8B_ROTATE_RIGHT_1);
      dvecData13 = IVP_SEL2NX8I(dvecData1N, dvecData10, IVP_SELI_8B_ROTATE_RIGHT_2);
      /* Select input data */
      dvecData21 = dvecData20;
      dvecData22 = IVP_SEL2NX8I(dvecData2N, dvecData20, IVP_SELI_8B_ROTATE_RIGHT_1);
      dvecData23 = IVP_SEL2NX8I(dvecData2N, dvecData20, IVP_SELI_8B_ROTATE_RIGHT_2);

      xb_vec2Nx8 dvecData30, dvecData3N;
      /* Output Data Pointer */
      int8_t *pOut = pOutData + x * outDataPitch1;
      pdvecOut = (xb_vec2Nx8 *)(pOut);
      for (y = 0; y < outH; y++)  /* Along output Height */
      {
        /* load current line */
        pdvecData3 = (xb_vec2Nx8 *) (pData + ((y + 2) * inDataPitch2));
        valign vaData3 = IVP_LA2NX8_PP(pdvecData3);
        IVP_LAV2NX8_XP(dvecData30, vaData3, pdvecData3, inDataPitch2 - x);
        IVP_LAV2NX8_XP(dvecData3N, vaData3, pdvecData3, inDataPitch2 - x - vectorizationWidth);
        dvecData31 = dvecData30;
        dvecData32 = IVP_SEL2NX8I(dvecData3N, dvecData30, IVP_SELI_8B_ROTATE_RIGHT_1);
        dvecData33 = IVP_SEL2NX8I(dvecData3N, dvecData30, IVP_SELI_8B_ROTATE_RIGHT_2);

        /* Initialize accumulator */
        daccSum1 = 0;

        /* Multiply Input with Coefficient Value */
        IVP_MULPA2NX8(daccSum1, dvecCoeff11, dvecData11, dvecCoeff12, dvecData12);
        IVP_MULPA2NX8(daccSum1, dvecCoeff13, dvecData13, dvecCoeff21, dvecData21);
        IVP_MULPA2NX8(daccSum1, dvecCoeff22, dvecData22, dvecCoeff23, dvecData23);
        IVP_MULPA2NX8(daccSum1, dvecCoeff31, dvecData31, dvecCoeff32, dvecData32);
        IVP_MULA2NX8(daccSum1,  dvecCoeff33, dvecData33);

        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum1, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut);
        /* Store the output vecOut1 along the output depth */
        IVP_SAV2NX8_XP(dvecOut, vaOutData, pdvecOut, remX);
        FLUSH2NX8(vaOutData, pdvecOut, outDataPitch2, remX, pOut);
        /* Initialize Next  set of Input values */
        dvecData11 = dvecData21;
        dvecData12 = dvecData22;
        dvecData13 = dvecData23;

        dvecData21 = dvecData31;
        dvecData22 = dvecData32;
        dvecData23 = dvecData33;
      } // for (y = 0; y < outH; y++)
    } // for (x = 0; x < outW; x += vectorizationWidth)
  } // if ((numCh == 1) && (inDataPitch1 == 1) && (outDataPitch1 == 1))
  else
  {
    /* Initialize accumulators with bias values
     * copy lower 32 elements values in higher 32 bytes
     */
    phvecBias = (xb_vecN_2x32v*) (pBiasData);
    xb_vecN_2x32v hvecBias1, hvecBias2, hvecBias3, hvecBias4;
    valign vaBias = IVP_LAN_2X32_PP(phvecBias);
    IVP_LAVN_2X32_XP(hvecBias1, vaBias, phvecBias, numCh << 2);
    IVP_LAVN_2X32_XP(hvecBias2, vaBias, phvecBias, (numCh - XCHAL_IVPN_SIMD_WIDTH / 2) << 2);
    /* Load Scale values */
    phvecOutScale = (xb_vecN_2x32v *)(pOutScale);
    xb_vecN_2x32v hvecScale1, hvecScale2, hvecScale3, hvecScale4;
    valign vaOutScale = IVP_LAN_2X32_PP(phvecOutScale);
    IVP_LAVN_2X32_XP(hvecScale1, vaOutScale, phvecOutScale,numCh<<2 );
    IVP_LAVN_2X32_XP(hvecScale2, vaOutScale, phvecOutScale, (numCh - XCHAL_IVPN_SIMD_WIDTH / 2) << 2);
    /* Load Shift values */
    pdvecOutShift = (xb_vec2Nx8 *)pOutShift;
    xb_vec2Nx8 dvecShift;
    valign vaShift = IVP_LA2NX8_PP(pdvecOutShift);
    IVP_LA2NX8_IP(dvecShift, vaShift, pdvecOutShift);
    dvecShift = IVP_SHFL2NX8(dvecShift, dvecSeq2);

    /* Calculate left shift and right shift values */
    vbool2N vb2N = IVP_LT2NX8(dvecShift, 0);
    xb_vec2Nx8 dvecRightShift = IVP_MOV2NX8T(0, dvecShift, vb2N);
    xb_vec2Nx8 dvecLeftShift = 0;
    IVP_SUB2NX8T(dvecLeftShift, 0, dvecShift, vb2N);
    /* Calculate right shift values */
    xb_vec2Nx8 dvecRightShiftL, dvecRightShiftH;
    IVP_DSEL2NX8I(dvecRightShiftH, dvecRightShiftL, 0, dvecRightShift, IVP_DSELI_8B_INTERLEAVE_1);
    xb_vecNx16 vecRightShift1 = IVP_MOVNX16_FROM2NX8(dvecRightShiftL);
    xb_vecNx16 vecRightShift2 = IVP_MOVNX16_FROM2NX8(dvecRightShiftH);
    /* Calculate left shift values */
    xb_vec2Nx8 dvecLeftShiftL, dvecLeftShiftH;
    IVP_DSEL2NX8I(dvecLeftShiftH, dvecLeftShiftL, 0, dvecLeftShift, IVP_DSELI_8B_INTERLEAVE_1);
    xb_vecNx16 vecLeftShiftLL, vecLeftShiftLH, vecLeftShiftHL, vecLeftShiftHH;
    IVP_DSELNX16I(vecLeftShiftLH, vecLeftShiftLL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftL), IVP_DSELI_INTERLEAVE_1);
    IVP_DSELNX16I(vecLeftShiftHH, vecLeftShiftHL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftH), IVP_DSELI_INTERLEAVE_1);
    /* Left Shift Values */
    xb_vecN_2x32v hvecLeftShift1 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLL);
    xb_vecN_2x32v hvecLeftShift2 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLH);
    xb_vecN_2x32v hvecLeftShift3 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHL);
    xb_vecN_2x32v hvecLeftShift4 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHH);

    /* Pointer for Coefficient Load */
    pdvecCoeff = (xb_vec2Nx8 *)(pCoeffData);

    /* 9 Coefficient Loads */
    IVP_LV2NX8_XP(dvecCoeff11, pdvecCoeff, coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff12, pdvecCoeff, coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff13, pdvecCoeff, coeffPitch2 - 2 * coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff21, pdvecCoeff, coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff22, pdvecCoeff, coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff23, pdvecCoeff, coeffPitch2 - 2 * coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff31, pdvecCoeff, coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff32, pdvecCoeff, coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff33, pdvecCoeff, coeffPitch1);

    /* shuffle coefficient data */
    dvecCoeff11 = IVP_SHFL2NX8(dvecCoeff11, dvecSeq2);
    dvecCoeff12 = IVP_SHFL2NX8(dvecCoeff12, dvecSeq2);
    dvecCoeff13 = IVP_SHFL2NX8(dvecCoeff13, dvecSeq2);
    dvecCoeff21 = IVP_SHFL2NX8(dvecCoeff21, dvecSeq2);
    dvecCoeff22 = IVP_SHFL2NX8(dvecCoeff22, dvecSeq2);
    dvecCoeff23 = IVP_SHFL2NX8(dvecCoeff23, dvecSeq2);
    dvecCoeff31 = IVP_SHFL2NX8(dvecCoeff31, dvecSeq2);
    dvecCoeff32 = IVP_SHFL2NX8(dvecCoeff32, dvecSeq2);
    dvecCoeff33 = IVP_SHFL2NX8(dvecCoeff33, dvecSeq2);
    /* Calculate Fixup terms*/
    xb_vec2Nx24 daccSum0;
    daccSum0 = IVP_ADDW2NX8(dvecCoeff11, dvecCoeff12);
    IVP_ADDWA2NX8(daccSum0, dvecCoeff13, dvecCoeff21);
    IVP_ADDWA2NX8(daccSum0, dvecCoeff22, dvecCoeff23);
    IVP_ADDWA2NX8(daccSum0, dvecCoeff31, dvecCoeff32);
    IVP_ADDWA2NX8(daccSum0, dvecCoeff33, 0);
    xb_vecNx16 vecSumL = IVP_CVT16S2NX24L(daccSum0);
    xb_vecNx16 vecSumH = IVP_CVT16S2NX24H(daccSum0);
    xb_vecNx48 accFixupL = IVP_MULNX16(vecSumL, (xb_vecNx16)zeroPtIn);
    xb_vecNx48 accFixupH = IVP_MULNX16(vecSumH, (xb_vecNx16)zeroPtIn);
    xb_vecN_2x32v hvecFixLL = IVP_CVT32SNX48L(accFixupL);
    xb_vecN_2x32v hvecFixLH = IVP_CVT32SNX48H(accFixupL);
    xb_vecN_2x32v hvecFixHL = IVP_CVT32SNX48L(accFixupH);
    xb_vecN_2x32v hvecFixHH = IVP_CVT32SNX48H(accFixupH);
    /*Taking width 1 output and width2 output operations in a contiguous format*/
    if (numCh <= XCHAL_IVPN_SIMD_WIDTH / 2)
    {
      /*For eg; if d == 15, the sequence in accumulator is
      0 1 2 ....13 14 0 1 2 ....13 14*/
      xb_vecN_2x32v hvecSeq1 = IVP_SEQN_2X32();
      vboolN_2 vbl11 = IVP_GEN_2X32(hvecSeq1, numCh);
      IVP_SUBN_2X32T(hvecSeq1, hvecSeq1, numCh, vbl11);
      xb_vecN_2x32v hvecSeq2 = IVP_ADDN_2X32(IVP_SEQN_2X32(), XCHAL_IVPN_SIMD_WIDTH / 2 - numCh);
      hvecBias1 = IVP_SELN_2X32(hvecBias1, hvecBias1, hvecSeq1);
      hvecBias2 = IVP_SELN_2X32(hvecBias2, hvecBias1, hvecSeq2);
      hvecBias1 = IVP_SUBN_2X32(hvecBias1, hvecFixLL);
      hvecBias2 = IVP_SUBN_2X32(hvecBias2, hvecFixLH);
      hvecBias3 = hvecBias1;
      hvecBias4 = hvecBias2;
      /* OutScale Values */
      hvecScale1 = IVP_SELN_2X32(hvecScale1, hvecScale1, hvecSeq1);
      hvecScale2 = IVP_SELN_2X32(hvecScale2, hvecScale1, hvecSeq2);
      hvecScale3 = hvecScale1;
      hvecScale4 = hvecScale2;
    }
    else
    {
      /*For eg; if d == 31, the sequence in accumulator is
      0 1 2 ....29 30 0 1 2 ....29 30*/
      xb_vecN_2x32v hvecSeq1 = IVP_ADDN_2X32(IVP_SEQN_2X32(), XCHAL_IVPN_SIMD_WIDTH / 2);
      vboolN_2 vbl11 = IVP_GEN_2X32(hvecSeq1, numCh);
      IVP_SUBN_2X32T(hvecSeq1, hvecSeq1, numCh, vbl11);
      xb_vecN_2x32v hvecSeq2 = IVP_ADDN_2X32(IVP_SEQN_2X32(), XCHAL_IVPN_SIMD_WIDTH - numCh);
      xb_vecN_2x32v hvecBias2A = IVP_SELN_2X32(hvecBias2, hvecBias1, hvecSeq1);
      xb_vecN_2x32v hvecBias3A = IVP_SELN_2X32(hvecBias2, hvecBias1, hvecSeq2);
      xb_vecN_2x32v hvecBias4A = IVP_SELN_2X32(hvecBias2, hvecBias2, hvecSeq2);
      hvecBias1  = IVP_SUBN_2X32(hvecBias1, hvecFixLL);
      hvecBias2A = IVP_SUBN_2X32(hvecBias2A, hvecFixLH);
      hvecBias3A = IVP_SUBN_2X32(hvecBias3A, hvecFixHL);
      hvecBias4A = IVP_SUBN_2X32(hvecBias4A, hvecFixHH);
      /* OutScale Values */
      xb_vecN_2x32v hvecScale2A = IVP_SELN_2X32(hvecScale2, hvecScale1, hvecSeq1);
      hvecScale3 = IVP_SELN_2X32(hvecScale2, hvecScale1, hvecSeq2);
      hvecScale4 = IVP_SELN_2X32(hvecScale2, hvecScale2, hvecSeq2);
      hvecScale2 = hvecScale2A;
      hvecBias2 = hvecBias2A;
      hvecBias3 = hvecBias3A;
      hvecBias4 = hvecBias4A;
    }

    for (y = 0; y < outH; y++)  /* Along Output Height */
    {
      /* Input Data Pointers */
      int8_t *pData = pInData + y * inDataPitch2;
      /* Output Data Pointers */
      int8_t *pOut = pOutData + y * outDataPitch2;
      pdvecOut = (xb_vec2Nx8 *)(pOut);

      /* Input Data Pointers */
      pdvecData1 = (xb_vec2Nx8 *) (pData);
      pdvecData2 = (xb_vec2Nx8 *) (pData + 1*inDataPitch2);
      pdvecData3 = (xb_vec2Nx8 *) (pData + 2*inDataPitch2);
      /* Input loads*/
      /* ky = 0*/
      /* load data from first 2 input rows */
      valign vaData1 = IVP_LA2NX8_PP(pdvecData1);
      IVP_LA2NX8_XP(dvecData11, vaData1, pdvecData1, 2*inDataPitch1);
      /* ky = 1*/
      vaData1 = IVP_LA2NX8_PP(pdvecData2);
      IVP_LA2NX8_XP(dvecData21, vaData1, pdvecData2, 2*inDataPitch1);
      /* ky = 2*/
      vaData1 = IVP_LA2NX8_PP(pdvecData3);
      IVP_LA2NX8_XP(dvecData31, vaData1, pdvecData3, 2*inDataPitch1);

      valign vaData2 = IVP_LA2NX8_PP(pdvecData1);
      valign vaData3 = IVP_LA2NX8_PP(pdvecData2);
      valign vaData4 = IVP_LA2NX8_PP(pdvecData3);
      for (x = 0; x < (outW - 1); x+= 2)   /* Along Output Width */
      {
        daccSum1 = 0;
        /* ky = 0*/
        /* load data from second 2 input rows */
        IVP_LA2NX8_XP(dvecData13, vaData2, pdvecData1, 2*inDataPitch1);
        /* ky = 1*/
        IVP_LA2NX8_XP(dvecData23, vaData3, pdvecData2, 2*inDataPitch1);
        /* ky = 2*/
        IVP_LA2NX8_XP(dvecData33, vaData4, pdvecData3, 2*inDataPitch1);
        /* Select Input Data */
        dvecData12 = IVP_SEL2NX8(dvecData13, dvecData11, dvecSeq1);
        dvecData22 = IVP_SEL2NX8(dvecData23, dvecData21, dvecSeq1);
        dvecData32 = IVP_SEL2NX8(dvecData33, dvecData31, dvecSeq1);

        /*Multiply and accumulate input data vector and coeff vector */
        IVP_MULPA2NX8(daccSum1, dvecCoeff11, dvecData11, dvecCoeff12, dvecData12);
        IVP_MULPA2NX8(daccSum1, dvecCoeff13, dvecData13, dvecCoeff21, dvecData21);
        IVP_MULPA2NX8(daccSum1, dvecCoeff22, dvecData22, dvecCoeff23, dvecData23);
        IVP_MULPA2NX8(daccSum1, dvecCoeff31, dvecData31, dvecCoeff32, dvecData32);
        IVP_MULA2NX8 (daccSum1, dvecCoeff33, dvecData33);

        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum1, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut);
        /* Storing the first and second row */
        IVP_SAV2NX8_XP(dvecOut, vaOutData, pdvecOut, numCh *2);
        FLUSH2NX8(vaOutData, pdvecOut, outDataPitch1 * 2, numCh * 2, pOut);
        dvecData11 = dvecData13;
        dvecData21 = dvecData23;
        dvecData31 = dvecData33;
      }/* End for (x = 0; x < outW-1; x +=2) */
      if(x < outW)
      {
        daccSum1 = 0;
        /* ky = 0*/
        /* load data from second 2 input rows */
        IVP_LA2NX8_XP(dvecData13, vaData2, pdvecData1, inDataPitch1);
        /* ky = 1*/
        IVP_LA2NX8_XP(dvecData23, vaData3, pdvecData2, inDataPitch1);
        /* ky = 2*/
        IVP_LA2NX8_XP(dvecData33, vaData4, pdvecData3, inDataPitch1);

        dvecData12 = IVP_SEL2NX8(dvecData13, dvecData11, dvecSeq1);
        dvecData22 = IVP_SEL2NX8(dvecData23, dvecData21, dvecSeq1);
        dvecData32 = IVP_SEL2NX8(dvecData33, dvecData31, dvecSeq1);

        /*Multiply and accumulate input data vector and coeff vector */
        IVP_MULPA2NX8(daccSum1, dvecCoeff11, dvecData11, dvecCoeff12, dvecData12);
        IVP_MULPA2NX8(daccSum1, dvecCoeff13, dvecData13, dvecCoeff21, dvecData21);
        IVP_MULPA2NX8(daccSum1, dvecCoeff22, dvecData22, dvecCoeff23, dvecData23);
        IVP_MULPA2NX8(daccSum1, dvecCoeff31, dvecData31, dvecCoeff32, dvecData32);
        IVP_MULA2NX8(daccSum1, dvecCoeff33, dvecData33);

        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum1, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut);
        /* Storing the first row */
        IVP_SAV2NX8_XP(dvecOut, vaOutData, pdvecOut, numCh);
        FLUSH2NX8(vaOutData, pdvecOut, outDataPitch1, numCh, pOut);
      } // if(outW < x)
    } /* End for (y = 0; y < outH; y ++) */
  } //else
}

/*****************************************************************************
* Generic variant for 3x3 DepthwiseConvolveAVQ2D_MOD_DWH
*  **************************************************************************/
void depthwiseConvolveAVQ2D_S_3x3_S8Ca2_MOD_DWH(const xi_pTile3D inTile,
                                                const xi_pTile3D coeffTile,
                                                const xi_pArray biasArray,
                                                const xi_pArray outScaleArray,
                                                const xi_pArray outShiftArray,
                                                xi_pTile3D outTile,
                                                const xi_cnna_conv_params *param)
{
  /* Getting parameters from the tile structures */
  const int32_t numCh         = XI_TILE3D_GET_DIM1(inTile);
  const int32_t outW          = XI_TILE3D_GET_DIM2(outTile);
  const int32_t outH          = XI_TILE3D_GET_DIM3(outTile);
  const int32_t inDataPitch1  = XI_TILE3D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2  = XI_TILE3D_GET_DIM2_PITCH(inTile);
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
  const int32_t coeffPitch1   = XI_TILE3D_GET_DIM1_PITCH(coeffTile);
  const int32_t coeffPitch2   = XI_TILE3D_GET_DIM2_PITCH(coeffTile);
  /* Convolution params */
  const uint8_t enableReLu    = XI_CNNA_CONV_GET_FLAG_RELU(param);
  const uint8_t stride        = XI_CNNA_CONV_GET_STRIDE(param);
  const int16_t zeroPtOut     = XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param);
  const int16_t zeroPtIn      = XI_CNNA_CONV_GET_ZEROPT_INPUT(param);
  const int16_t reluMin       = XI_CNNA_CONV_GET_RELUMIN(param);
  const int16_t reluMax       = XI_CNNA_CONV_GET_RELUMAX(param);

  /* Setting the limits for output data according to ReLu is enabled or not*/
  const int16_t minLim = enableReLu ? reluMin : SCHAR_MIN;
  const int16_t maxLim = enableReLu ? reluMax : SCHAR_MAX;
  /* Kernel Size (DWH) */
  const int32_t kSizeU = XI_TILE3D_GET_DIM2(coeffTile);

  /* Data Pointers of input, output, coefficient and bias data */
  int8_t *pInData    = (int8_t *) XI_TILE3D_GET_DATA_PTR(inTile);
  int8_t *pOutData   = (int8_t *) XI_TILE3D_GET_DATA_PTR(outTile);
  int8_t *pCoeffData = (int8_t *) XI_TILE3D_GET_DATA_PTR(coeffTile);
  int32_t *pBiasData = (int32_t *)XI_ARRAY_GET_DATA_PTR(biasArray);
  int32_t *pOutScale = (int32_t *)XI_ARRAY_GET_DATA_PTR(outScaleArray);
  int8_t *pOutShift  = (int8_t *)XI_ARRAY_GET_DATA_PTR(outShiftArray);

  /* Move pointer to the start of the data (including edge) */
  pInData = &pInData[-((kSizeU / 2) * inDataPitch1 + (kSizeU / 2) * inDataPitch2)];

  /* Variable Declarations */
  int32_t ch, x, y;
  int32_t vectorizationWidth = 2 * XCHAL_IVPN_SIMD_WIDTH;
  valign vaOutData = IVP_ZALIGN();
  /* Input and Output data Pointers */
  xb_vecN_2x32v* restrict phvecBias1;
  xb_vecN_2x32v* restrict phvecBias2;
  xb_vecN_2x32v* restrict phvecOutScale1;
  xb_vecN_2x32v* restrict phvecOutScale2;
  xb_vec2Nx8* restrict pdvecOutShift;
  xb_vec2Nx8* restrict pdvecCoeff;
  xb_vec2Nx8* restrict pdvecData1;
  xb_vec2Nx8* restrict pdvecData2;
  xb_vec2Nx8* restrict pdvecData3;
  xb_vec2Nx8* restrict pdvecOut;
  /* Input and Output data Pointers */
  xb_vec2Nx8 dvecData11, dvecData12, dvecData13;
  xb_vec2Nx8 dvecData21, dvecData22, dvecData23;
  xb_vec2Nx8 dvecData31, dvecData32, dvecData33;
  xb_vec2Nx8 dvecOut;
  xb_vecN_2x32v hvecScale1, hvecScale2, hvecScale3, hvecScale4;
  xb_vec2Nx24 daccSum;
  for (ch = 0; ch < numCh; ch += vectorizationWidth) /* Along Output Channel */
  {
    /* To handle corner case when number of output channels
     * is not a multiple of  2 * XCHAL_IVPN_SIMD_WIDTH*/
    int32_t remainingOutCh = XT_MIN(vectorizationWidth, numCh - ch);
    int32_t remBiasLoad = (remainingOutCh > XCHAL_IVPN_SIMD_WIDTH) ? 1 : 0;

    int32_t remCh1 = XT_MIN(((numCh - ch) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh2 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH / 2) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh3 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh4 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH * 3 / 2) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);

    /* Load OutScale values */
    phvecOutScale1 = (xb_vecN_2x32v*)(pOutScale + ch);
    phvecOutScale2 = (xb_vecN_2x32v*)(pOutScale + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
    valign vaOutScale = IVP_LAN_2X32_PP(phvecOutScale1);
    IVP_LAVN_2X32_XP(hvecScale1, vaOutScale, phvecOutScale1, remCh1);
    IVP_LAVN_2X32_XP(hvecScale2, vaOutScale, phvecOutScale1, remCh2);
    vaOutScale = IVP_LAN_2X32_PP(phvecOutScale2);
    IVP_LAVN_2X32_XP(hvecScale3, vaOutScale, phvecOutScale2, remCh3);
    IVP_LAVN_2X32_XP(hvecScale4, vaOutScale, phvecOutScale2, remCh4);

    /* Load OutShift values */
    xb_vec2Nx8 dvecShift;
    pdvecOutShift = (xb_vec2Nx8*)(pOutShift + ch);
    valign vaOutShift = IVP_LA2NX8_PP(pdvecOutShift);
    IVP_LAV2NX8_XP(dvecShift, vaOutShift, pdvecOutShift, remainingOutCh);

    /* Calculate left shift and right shift values */
    vbool2N vb2N = IVP_LT2NX8(dvecShift, 0);
    xb_vec2Nx8 dvecRightShift = IVP_MOV2NX8T(0, dvecShift, vb2N);
    xb_vec2Nx8 dvecLeftShift = 0;
    IVP_SUB2NX8T(dvecLeftShift, 0, dvecShift, vb2N);

    xb_vec2Nx8 dvecRightShiftL, dvecRightShiftH;
    IVP_DSEL2NX8I(dvecRightShiftH, dvecRightShiftL, 0, dvecRightShift, IVP_DSELI_8B_INTERLEAVE_1);

    xb_vecNx16 vecRightShift1 = IVP_MOVNX16_FROM2NX8(dvecRightShiftL);
    xb_vecNx16 vecRightShift2 = IVP_MOVNX16_FROM2NX8(dvecRightShiftH);

    xb_vec2Nx8 dvecLeftShiftL, dvecLeftShiftH;
    IVP_DSEL2NX8I(dvecLeftShiftH, dvecLeftShiftL, 0, dvecLeftShift, IVP_DSELI_8B_INTERLEAVE_1);

    xb_vecNx16 vecLeftShiftLL, vecLeftShiftLH, vecLeftShiftHL, vecLeftShiftHH;
    IVP_DSELNX16I(vecLeftShiftLH, vecLeftShiftLL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftL), IVP_DSELI_INTERLEAVE_1);
    IVP_DSELNX16I(vecLeftShiftHH, vecLeftShiftHL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftH), IVP_DSELI_INTERLEAVE_1);

    xb_vecN_2x32v hvecLeftShift1 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLL);
    xb_vecN_2x32v hvecLeftShift2 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLH);
    xb_vecN_2x32v hvecLeftShift3 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHL);
    xb_vecN_2x32v hvecLeftShift4 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHH);

    /* Pointer for Coefficient Load */
    pdvecCoeff = (xb_vec2Nx8 *)(pCoeffData + ch);
    /* 9 Coefficient Loads */
    xb_vec2Nx8 dvecCoeff11, dvecCoeff12, dvecCoeff13;
    xb_vec2Nx8 dvecCoeff21, dvecCoeff22, dvecCoeff23;
    xb_vec2Nx8 dvecCoeff31, dvecCoeff32, dvecCoeff33;
    IVP_LV2NX8_XP(dvecCoeff11, pdvecCoeff, coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff12, pdvecCoeff, coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff13, pdvecCoeff, coeffPitch2 - 2 * coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff21, pdvecCoeff, coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff22, pdvecCoeff, coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff23, pdvecCoeff, coeffPitch2 - 2 * coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff31, pdvecCoeff, coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff32, pdvecCoeff, coeffPitch1);
    IVP_LV2NX8_XP(dvecCoeff33, pdvecCoeff, coeffPitch2 - 2 * coeffPitch1);
    /* Calculate Fixup terms */
    daccSum = IVP_ADDW2NX8(dvecCoeff11, dvecCoeff12);
    IVP_ADDWA2NX8(daccSum, dvecCoeff13, dvecCoeff21);
    IVP_ADDWA2NX8(daccSum, dvecCoeff22, dvecCoeff23);
    IVP_ADDWA2NX8(daccSum, dvecCoeff31, dvecCoeff32);
    IVP_ADDWA2NX8(daccSum, dvecCoeff33, 0);
    xb_vecNx16 vecSumL = IVP_CVT16S2NX24L(daccSum);
    xb_vecNx16 vecSumH = IVP_CVT16S2NX24H(daccSum);
    xb_vecNx48 accFixupL = IVP_MULNX16(vecSumL, (xb_vecNx16)zeroPtIn);
    xb_vecNx48 accFixupH = IVP_MULNX16(vecSumH, (xb_vecNx16)zeroPtIn);
    xb_vecN_2x32v hvecFixLL = IVP_CVT32SNX48L(accFixupL);
    xb_vecN_2x32v hvecFixLH = IVP_CVT32SNX48H(accFixupL);
    xb_vecN_2x32v hvecFixHL = IVP_CVT32SNX48L(accFixupH);
    xb_vecN_2x32v hvecFixHH = IVP_CVT32SNX48H(accFixupH);
    /* Initialize accumulators with bias values */
    phvecBias1 = (xb_vecN_2x32v*)(pBiasData + ch);
    phvecBias2 = (xb_vecN_2x32v*)(pBiasData + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
    xb_vecN_2x32v hvecBias1, hvecBias2, hvecBias3, hvecBias4;
    BIAS_LOAD_FIXUP_SUBTRACT(phvecBias1, phvecBias2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, remCh1, remCh2, remCh3, remCh4);
    for (y = 0; y < outH; y++)  /* Along Output Height */
    {
      /* Input Data Pointer */
      int8_t *pData = (pInData + ch + (y * stride * inDataPitch2));

      pdvecData1 = (xb_vec2Nx8 *)(pData);
      pdvecData2 = (xb_vec2Nx8 *)(pData + 1 * inDataPitch2);
      pdvecData3 = (xb_vec2Nx8 *)(pData + 2 * inDataPitch2);

      /* Output data Pointer */
      int8_t *pOut = pOutData + y * outDataPitch2 + ch;
      pdvecOut = (xb_vec2Nx8 *)(pOut);
      /* Input loads*/
      /* ky = 0*/
      valign vaData1 = IVP_LA2NX8_PP(pdvecData1);
      IVP_LA2NX8_XP(dvecData11, vaData1, pdvecData1, inDataPitch1);
      vaData1 = IVP_LA2NX8_PP(pdvecData1);
      IVP_LA2NX8_XP(dvecData12, vaData1, pdvecData1, inDataPitch1);

      /* ky = 1*/
      vaData1 = IVP_LA2NX8_PP(pdvecData2);
      IVP_LA2NX8_XP(dvecData21, vaData1, pdvecData2, inDataPitch1);
      vaData1 = IVP_LA2NX8_PP(pdvecData2);
      IVP_LA2NX8_XP(dvecData22, vaData1, pdvecData2, inDataPitch1);

      /* ky = 2*/
      vaData1 = IVP_LA2NX8_PP(pdvecData3);
      IVP_LA2NX8_XP(dvecData31, vaData1, pdvecData3, inDataPitch1);
      vaData1 = IVP_LA2NX8_PP(pdvecData3);
      IVP_LA2NX8_XP(dvecData32, vaData1, pdvecData3, inDataPitch1);

      for (x = 0; x < outW; x++)/* Along Output Width */
      {
        daccSum = 0;
        /* ky = 0*/
        valign vaData2 = IVP_LA2NX8_PP(pdvecData1);
        IVP_LA2NX8_XP(dvecData13, vaData2, pdvecData1, inDataPitch1);
        /* ky = 1*/
        valign vaData3 = IVP_LA2NX8_PP(pdvecData2);
        IVP_LA2NX8_XP(dvecData23, vaData3, pdvecData2, inDataPitch1);
        /* ky = 2*/
        valign vaData4 = IVP_LA2NX8_PP(pdvecData3);
        IVP_LA2NX8_XP(dvecData33, vaData4, pdvecData3, inDataPitch1);

        /*Multiply and accumulate input data vector and coeff vector */
        IVP_MULPA2NX8(daccSum, dvecCoeff11, dvecData11, dvecCoeff12, dvecData12);
        IVP_MULPA2NX8(daccSum, dvecCoeff13, dvecData13, dvecCoeff21, dvecData21);
        IVP_MULPA2NX8(daccSum, dvecCoeff22, dvecData22, dvecCoeff23, dvecData23);
        IVP_MULPA2NX8(daccSum, dvecCoeff31, dvecData31, dvecCoeff32, dvecData32);
        IVP_MULA2NX8(daccSum, dvecCoeff33, dvecData33);

        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut);
        /* Store the output vecOut1 along the output depth */
        IVP_SAV2NX8_XP(dvecOut, vaOutData, pdvecOut, remainingOutCh);
        FLUSH2NX8(vaOutData, pdvecOut, outDataPitch1, remainingOutCh, pOut);

        dvecData11 = dvecData12;
        dvecData12 = dvecData13;
        dvecData21 = dvecData22;
        dvecData22 = dvecData23;
        dvecData31 = dvecData32;
        dvecData32 = dvecData33;
      }  /* for (x = 0; x < outW; x ++) */
    }    /* for (y = 0; y < outH; y++) */
  }      /* for (ch = 0; ch < numCh; ch += vectorizationWidth) */
}

/**************************************************************************************/
/***************** xiDepthwiseConvolveAVQ2D_S_3x3_S8Ca2_MOD_DWH ***********************/
/**************************************************************************************/
/**************************************************************************************/
/* Description : Optimized implementation of Symmetric 3x3 MOD_DWH                    */
/*               depthwise convolution for Android NN.                                */
/* Inputs      : Input Data Tile, Coeff Data Tile, Bias Array, Output Scale Array,    */
/*               Output Shift Array, CNNA convolution params structure.               */
/* Outputs     : XI Error Code                                                        */
/* InOuts      : Output Tile                                                          */
/* Assumptions : InData, CoeffData are S8                                             */
/*               biasArray is signed 32b, OutData is S8                               */
/*               Kernel Size is 3x3.                                                  */
/*               Input and Output are in DWH format                                   */
/*               Coeff is in DWH format                                               */
/*               CoeffDim1Pitch is aligned to 2N (Ca2)                                */
/**************************************************************************************/
XI_ERR_TYPE xiDepthwiseConvolveAVQ2D_S_3x3_S8Ca2_MOD_DWH(const xi_pTile3D inTile,
                                                         const xi_pTile3D coeffTile,
                                                         const xi_pArray biasArray,
                                                         const xi_pArray outScaleArray,
                                                         const xi_pArray outShiftArray,
                                                         xi_pTile3D outTile,
                                                         const xi_cnna_conv_params *param)
{
  /* Error Checks */
  XI_ERROR_CHECKS()
  {
    XI_CHECK_TILE3D_S8(inTile);
    XI_CHECK_TILE3D_S8(outTile);
    XI_CHECK_TILE3D_S8(coeffTile);
    XI_CHECK_ARRAY_S32(biasArray);
    XI_CHECK_ARRAY_S32(outScaleArray);
    XI_CHECK_ARRAY_S8(outShiftArray);
    XI_CHECK_POINTER(param);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(coeffTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(inTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(coeffTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(biasArray, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(outScaleArray, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(outShiftArray, outTile);
    XI_CHECK_TILE3D_DATA_ORDER(inTile, XI_DWH);
    XI_CHECK_TILE3D_DATA_ORDER(outTile, XI_DWH);
    XI_CHECK_TILE3D_DATA_ORDER(coeffTile, XI_DWH);
    XI_CHECK_KERNEL_SIZE_DEPTHWISE(coeffTile, 3);
    XI_CHECK_TILE3D_EDGE(inTile, 1);
    XI_CHECK_ERROR(((XI_CNNA_CONV_GET_STRIDEX(param) == 1) || (XI_CNNA_CONV_GET_STRIDEX(param) == 2) || (XI_CNNA_CONV_GET_STRIDEX(param) == 4)) && \
                   ((XI_CNNA_CONV_GET_STRIDEY(param) == 1) || (XI_CNNA_CONV_GET_STRIDEY(param) == 2) || (XI_CNNA_CONV_GET_STRIDEY(param) == 4)), \
                   XI_ERR_BADARG, "\nStrideX = %hhu, StrideY = %hhu\nStride value should be equal to 1, 2 or 4", \
                   XI_CNNA_CONV_GET_STRIDEX(param), XI_CNNA_CONV_GET_STRIDEY(param));
    XI_CHECK_ERROR(XI_CNNA_CONV_GET_STRIDEX(param) == XI_CNNA_CONV_GET_STRIDEY(param), XI_ERR_BADARG, \
                   "\nStrideX = %hhu, StrideY = %hhu\nStrideX and StrideY must be same", \
                   XI_CNNA_CONV_GET_STRIDEX(param), XI_CNNA_CONV_GET_STRIDEY(param));
    XI_CHECK_TILE3D_IALIGNMENT_2NX8(coeffTile);
    XI_CHECK_CONSISTENCYA_DEPTHWISE_MOD_DWH(inTile, coeffTile, biasArray, outTile, param);
    XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(outScaleArray) >= XI_TILE3D_GET_DIM1(outTile), XI_ERR_DATASIZE, \
      "\nWidth of Output Scale Array = %d, Number of output channels = %d\nWidth of Output Scale Array should be \
      greater than or equal to Number of output channels", XI_ARRAY_GET_WIDTH(outScaleArray), XI_TILE3D_GET_DIM1(outTile));
    XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(outShiftArray) >= XI_TILE3D_GET_DIM1(outTile), XI_ERR_DATASIZE, \
      "\nWidth of Output Shift Array = %d, Number of output channels = %d\nWidth of Output Shift Array should be \
      greater than or equal to Number of output channels", XI_ARRAY_GET_WIDTH(outShiftArray), XI_TILE3D_GET_DIM1(outTile));
    if (XI_CNNA_CONV_GET_FLAG_RELU(param))
    {
      XI_CHECK_ERROR(XI_CNNA_CONV_GET_RELUMIN(param) <= XI_CNNA_CONV_GET_RELUMAX(param), XI_ERR_BADARG, \
        "\nRelu min = %hi, Relu max = %hi\nRelu max should be greater than or equal to Relu min", \
        XI_CNNA_CONV_GET_RELUMIN(param), XI_CNNA_CONV_GET_RELUMAX(param));
      XI_CHECK_ERROR((XI_CNNA_CONV_GET_RELUMIN(param) >= SCHAR_MIN), XI_ERR_BADARG, \
        "\nRelu min = %hi, value should be greater than or equal to -128", XI_CNNA_CONV_GET_RELUMIN(param));
      XI_CHECK_ERROR((XI_CNNA_CONV_GET_RELUMAX(param) <= SCHAR_MAX), XI_ERR_BADARG, \
        "\nRelu max = %hi, value should be less than or equal to 127", XI_CNNA_CONV_GET_RELUMAX(param));
    }
    XI_CHECK_ERROR(((XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param) >= SCHAR_MIN) && \
                    (XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param) <= SCHAR_MAX)), XI_ERR_BADARG,\
      "\nZero point output = %hi, value should be in the range -128 to 127", XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param));
    XI_CHECK_ERROR(((XI_CNNA_CONV_GET_ZEROPT_INPUT(param) >= SCHAR_MIN) && \
                    (XI_CNNA_CONV_GET_ZEROPT_INPUT(param) <= SCHAR_MAX)), XI_ERR_BADARG,\
      "\nZero point input = %hi, value should be in the range -128 to 127", XI_CNNA_CONV_GET_ZEROPT_INPUT(param));
    XI_CHECK_ERROR((XI_CNNA_CONV_GET_ZEROPT_COEFF(param) == 0), XI_ERR_BADARG,\
      "\nZero point coefficient = %hi, value should be equal to zero", XI_CNNA_CONV_GET_ZEROPT_COEFF(param));
  }
  if (XI_CNNA_CONV_GET_STRIDE(param) == 2)
  {
    depthwiseConvolveAVQ2D_S_3x3j2_S8Ca2_MOD_DWH(inTile, coeffTile, biasArray, outScaleArray, outShiftArray, outTile, param);
  }
  else if (XI_CNNA_CONV_GET_STRIDE(param) == 4)
  {
    depthwiseConvolveAVQ2D_S_3x3j4_S8Ca2_MOD_DWH(inTile, coeffTile, biasArray, outScaleArray, outShiftArray, outTile, param);
  }
  else if((XI_TILE3D_GET_DIM1_PITCH(inTile) <= XCHAL_IVPN_SIMD_WIDTH) && \
          (XI_TILE3D_GET_DIM1_PITCH(inTile) == XI_TILE3D_GET_DIM1(inTile)))
  {
    depthwiseConvolveAVQ2D_S_3x3_S8Ca2_MOD_DWH_FOLD(inTile, coeffTile, biasArray, outScaleArray, outShiftArray, outTile, param);
  }
  else
  {
    depthwiseConvolveAVQ2D_S_3x3_S8Ca2_MOD_DWH(inTile, coeffTile, biasArray, outScaleArray, outShiftArray, outTile, param);
  }

  return(XI_ERROR_STATUS());
}

/*****************************************************************************
* Stride 2 Sub-variant
* When input stride is 2 this function is called
*  **************************************************************************/
void depthwiseConvolveAVQ2D_S_5x5j2_S8Ca2_MOD_DWH(const xi_pTile3D inTile,
                                                  const xi_pTile3D coeffTile,
                                                  const xi_pArray biasArray,
                                                  const xi_pArray outScaleArray,
                                                  const xi_pArray outShiftArray,
                                                  xi_pTile3D outTile,
                                                  const xi_cnna_conv_params *param)
{
  /* Getting parameters from the tile structures */
  const int32_t numCh         = XI_TILE3D_GET_DIM1(inTile);
  const int32_t outW          = XI_TILE3D_GET_DIM2(outTile);
  const int32_t outH          = XI_TILE3D_GET_DIM3(outTile);
  const int32_t inDataPitch1  = XI_TILE3D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2  = XI_TILE3D_GET_DIM2_PITCH(inTile);
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
  const int32_t coeffPitch1   = XI_TILE3D_GET_DIM1_PITCH(coeffTile);
  const int32_t coeffPitch2   = XI_TILE3D_GET_DIM2_PITCH(coeffTile);
  /* Convolution params */
  const uint8_t enableReLu = XI_CNNA_CONV_GET_FLAG_RELU(param);
  const uint8_t stride = XI_CNNA_CONV_GET_STRIDE(param);
  const int16_t zeroPtOut = XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param);
  const int16_t zeroPtIn = XI_CNNA_CONV_GET_ZEROPT_INPUT(param);
  const int16_t reluMin = XI_CNNA_CONV_GET_RELUMIN(param);
  const int16_t reluMax = XI_CNNA_CONV_GET_RELUMAX(param);

  /* Setting the limits for output data according to ReLu is enabled or not*/
  const int16_t minLim = enableReLu ? reluMin : SCHAR_MIN;
  const int16_t maxLim = enableReLu ? reluMax : SCHAR_MAX;
  /* Kernel Size (DWH) */
  const int32_t kSizeU = XI_TILE3D_GET_DIM2(coeffTile);

  /* Data Pointers of input, output, coefficient and bias data */
  int8_t *pInData    = (int8_t *)XI_TILE3D_GET_DATA_PTR(inTile);
  int8_t *pOutData   = (int8_t *)XI_TILE3D_GET_DATA_PTR(outTile);
  int8_t *pCoeffData = (int8_t *)XI_TILE3D_GET_DATA_PTR(coeffTile);
  int32_t *pBiasData = (int32_t *)XI_ARRAY_GET_DATA_PTR(biasArray);
  int32_t *pOutScale = (int32_t *)XI_ARRAY_GET_DATA_PTR(outScaleArray);
  int8_t *pOutShift  = (int8_t *)XI_ARRAY_GET_DATA_PTR(outShiftArray);

  /* Move pointer to the start of the data (including edge) */
  pInData = &pInData[-((kSizeU / 2) * inDataPitch1 + (kSizeU / 2) * inDataPitch2)];

  /* Variable Declarations */
  int32_t ch, x, y, ky;
  int32_t vectorizationWidth = 2 * XCHAL_IVPN_SIMD_WIDTH;
  valign vaOutData = IVP_ZALIGN();
  /* Input and Output data Pointers */
  xb_vecN_2x32v* restrict phvecBias1;
  xb_vecN_2x32v* restrict phvecBias2;
  xb_vecN_2x32v* restrict phvecOutScale1;
  xb_vecN_2x32v* restrict phvecOutScale2;
  xb_vec2Nx8* restrict pdvecOutShift;
  xb_vec2Nx8* restrict pdvecCoeff;
  xb_vec2Nx8* restrict pdvecData1;
  xb_vec2Nx8* restrict pdvecData2;
  xb_vec2Nx8* restrict pdvecData3;
  xb_vec2Nx8* restrict pdvecData4;
  xb_vec2Nx8* restrict pdvecOut;
  /* Accumulator vectors */
  xb_vec2Nx24 daccSum1, daccSum2, daccSum3, daccSum4;

  for (ch = 0; ch < numCh; ch += vectorizationWidth) /* Along Output Channel */
  {
    /* To handle corner case when number of output channels
    * is not a multiple of  2 * XCHAL_IVPN_SIMD_WIDTH*/
    int32_t remainingOutCh = XT_MIN(vectorizationWidth, numCh - ch);
    int32_t remBiasLoad = (remainingOutCh > XCHAL_IVPN_SIMD_WIDTH) ? 1 : 0;

    int32_t remCh1 = XT_MIN(((numCh - ch) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh2 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH / 2) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh3 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh4 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH * 3 / 2) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);

    /* Load OutScale values */
    xb_vecN_2x32v hvecScale1, hvecScale2, hvecScale3, hvecScale4;
    phvecOutScale1 = (xb_vecN_2x32v*)(pOutScale + ch);
    phvecOutScale2 = (xb_vecN_2x32v*)(pOutScale + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
    valign vaOutScale = IVP_LAN_2X32_PP(phvecOutScale1);
    IVP_LAVN_2X32_XP(hvecScale1, vaOutScale, phvecOutScale1, remCh1);
    IVP_LAVN_2X32_XP(hvecScale2, vaOutScale, phvecOutScale1, remCh2);
    vaOutScale = IVP_LAN_2X32_PP(phvecOutScale2);
    IVP_LAVN_2X32_XP(hvecScale3, vaOutScale, phvecOutScale2, remCh3);
    IVP_LAVN_2X32_XP(hvecScale4, vaOutScale, phvecOutScale2, remCh4);

    /* Load OutShift values */
    xb_vec2Nx8 dvecShift;
    pdvecOutShift = (xb_vec2Nx8*)(pOutShift + ch);
    valign vaOutShift = IVP_LA2NX8_PP(pdvecOutShift);
    IVP_LAV2NX8_XP(dvecShift, vaOutShift, pdvecOutShift, remainingOutCh);

    /* Calculate left shift and right shift values */
    vbool2N vb2N = IVP_LT2NX8(dvecShift, 0);
    xb_vec2Nx8 dvecRightShift = IVP_MOV2NX8T(0, dvecShift, vb2N);
    xb_vec2Nx8 dvecLeftShift = 0;
    IVP_SUB2NX8T(dvecLeftShift, 0, dvecShift, vb2N);

    xb_vec2Nx8 dvecRightShiftL, dvecRightShiftH;
    IVP_DSEL2NX8I(dvecRightShiftH, dvecRightShiftL, 0, dvecRightShift, IVP_DSELI_8B_INTERLEAVE_1);

    xb_vecNx16 vecRightShift1 = IVP_MOVNX16_FROM2NX8(dvecRightShiftL);
    xb_vecNx16 vecRightShift2 = IVP_MOVNX16_FROM2NX8(dvecRightShiftH);

    xb_vec2Nx8 dvecLeftShiftL, dvecLeftShiftH;
    IVP_DSEL2NX8I(dvecLeftShiftH, dvecLeftShiftL, 0, dvecLeftShift, IVP_DSELI_8B_INTERLEAVE_1);

    xb_vecNx16 vecLeftShiftLL, vecLeftShiftLH, vecLeftShiftHL, vecLeftShiftHH;
    IVP_DSELNX16I(vecLeftShiftLH, vecLeftShiftLL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftL), IVP_DSELI_INTERLEAVE_1);
    IVP_DSELNX16I(vecLeftShiftHH, vecLeftShiftHL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftH), IVP_DSELI_INTERLEAVE_1);

    xb_vecN_2x32v hvecLeftShift1 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLL);
    xb_vecN_2x32v hvecLeftShift2 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLH);
    xb_vecN_2x32v hvecLeftShift3 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHL);
    xb_vecN_2x32v hvecLeftShift4 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHH);
    /* Fixup calculation */
    daccSum1 = 0;
    int8_t *pCoeff = (pCoeffData + ch);
    CALCULATE_FIXUP(daccSum1, kSizeU, kSizeU, pCoeff);
    /* Fixup term */
    xb_vecNx16 vecSumL = IVP_CVT16S2NX24L(daccSum1);
    xb_vecNx16 vecSumH = IVP_CVT16S2NX24H(daccSum1);
    xb_vecNx16 vecSumOdd, vecSumEven;
    IVP_DSELNX16I(vecSumOdd, vecSumEven, vecSumH, vecSumL, IVP_DSELI_DEINTERLEAVE_1);
    daccSum1 = IVP_MULI2NX8X16((xb_vec2Nx8)zeroPtIn, vecSumOdd, vecSumEven);
    xb_vecN_2x32v hvecFixLL = IVP_CVT32S2NX24LL(daccSum1);
    xb_vecN_2x32v hvecFixLH = IVP_CVT32S2NX24LH(daccSum1);
    xb_vecN_2x32v hvecFixHL = IVP_CVT32S2NX24HL(daccSum1);
    xb_vecN_2x32v hvecFixHH = IVP_CVT32S2NX24HH(daccSum1);
    for (y = 0; y < outH; y++)  /* Along Output Height */
    {
      for (x = 0; x < (outW - 3); x += 4) /* Along Output Width */
      {
        /* Initialize accumulators with bias values */
        phvecBias1 = (xb_vecN_2x32v*)(pBiasData + ch);
        phvecBias2 = (xb_vecN_2x32v*)(pBiasData + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
        xb_vecN_2x32v hvecBias1, hvecBias2, hvecBias3, hvecBias4;
        BIAS_LOAD_FIXUP_SUBTRACT(phvecBias1, phvecBias2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, remCh1, remCh2, remCh3, remCh4);
        daccSum2 = 0; daccSum1 = 0;
        daccSum3 = 0;
        daccSum4 = 0;
        /* Input Data Pointer */
        int8_t *pData = (pInData + ch + (x * stride * inDataPitch1) + (y * stride * inDataPitch2));
        pdvecData1 = (xb_vec2Nx8 *)(pData);
        pdvecData2 = (xb_vec2Nx8 *)(pData + 3 * inDataPitch1);
        pdvecData3 = (xb_vec2Nx8 *)(pData + 6 * inDataPitch1);
        pdvecData4 = (xb_vec2Nx8 *)(pData + 9 * inDataPitch1);
        /* Output data Pointer */
        int8_t *pOut = pOutData + y * outDataPitch2 + x * outDataPitch1 + ch;
        /* Pointer for Coefficient Load */
        pdvecCoeff = (xb_vec2Nx8 *)(pCoeffData + ch);
        for (ky = 0; ky < kSizeU; ky++) /* Kernel Height */
        {
          /* Input and Coeff Data vectors */
          xb_vec2Nx8 dvecCoeff1, dvecCoeff2, dvecCoeff3, dvecCoeff4, dvecCoeff5;
          xb_vec2Nx8 dvecData1, dvecData2, dvecData3, dvecData4;
          xb_vec2Nx8 dvecData5, dvecData6, dvecData7, dvecData8;
          xb_vec2Nx8 dvecData9, dvecData10, dvecData11;
          /* Load Coefficient values */
          IVP_LV2NX8_XP(dvecCoeff1, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff2, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff3, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff4, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff5, pdvecCoeff, (coeffPitch2 - 4 * coeffPitch1));
          /* To produce 4 output rows along width with stride 2,
          * a total of (OutputWidth - 1)*stride + KernelWidth  i.e
          * (4 - 1) * 2 + 5 = 11 input loads across input width are required*/
          /* Input loads*/
          valign vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData1, vaData1, pdvecData1, inDataPitch1);
          vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData2, vaData1, pdvecData1, inDataPitch1);
          vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData3, vaData1, pdvecData1, (inDataPitch2 - (2 * inDataPitch1)));
          valign vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData4, vaData2, pdvecData2, inDataPitch1);
          vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData5, vaData2, pdvecData2, inDataPitch1);
          vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData6, vaData2, pdvecData2, (inDataPitch2 - (2 * inDataPitch1)));
          valign vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData7, vaData3, pdvecData3, inDataPitch1);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData8, vaData3, pdvecData3, inDataPitch1);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData9, vaData3, pdvecData3, (inDataPitch2 - (2 * inDataPitch1)));
          valign vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData10, vaData4, pdvecData4, inDataPitch1);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData11, vaData4, pdvecData4, (inDataPitch2 - inDataPitch1));
          /*Multiply and accumulate dvecData1 to dvecData5 for 1st output row*/
          IVP_MULPA2NX8(daccSum1, dvecCoeff1, dvecData1, dvecCoeff2, dvecData2);
          IVP_MULPA2NX8(daccSum1, dvecCoeff3, dvecData3, dvecCoeff4, dvecData4);
          IVP_MULA2NX8(daccSum1, dvecCoeff5, dvecData5);
          /*Multiply and accumulate dvecData3 to dvecData7 for 2nd output row*/
          IVP_MULPA2NX8(daccSum2, dvecCoeff1, dvecData3, dvecCoeff2, dvecData4);
          IVP_MULPA2NX8(daccSum2, dvecCoeff3, dvecData5, dvecCoeff4, dvecData6);
          IVP_MULA2NX8(daccSum2, dvecCoeff5, dvecData7);
          /*Multiply and accumulate dvecData5 to dvecData9 for 3rd output row*/
          IVP_MULPA2NX8(daccSum3, dvecCoeff1, dvecData5, dvecCoeff2, dvecData6);
          IVP_MULPA2NX8(daccSum3, dvecCoeff3, dvecData7, dvecCoeff4, dvecData8);
          IVP_MULA2NX8(daccSum3, dvecCoeff5, dvecData9);
          /*Multiply and accumulate dvecData7 to dvecData11 for 4th output row*/
          IVP_MULPA2NX8(daccSum4, dvecCoeff1, dvecData7, dvecCoeff2, dvecData8);
          IVP_MULPA2NX8(daccSum4, dvecCoeff3, dvecData9, dvecCoeff4, dvecData10);
          IVP_MULA2NX8(daccSum4, dvecCoeff5, dvecData11);
        }
        xb_vec2Nx8 dvecOut1, dvecOut2, dvecOut3, dvecOut4;
        /* Store the result for 1st output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum1, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut1);
        /* Store the output vecOut1 along the output depth */
        pdvecOut = (xb_vec2Nx8 *)(pOut);
        IVP_SAV2NX8_XP(dvecOut1, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 2nd output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut2);
        pdvecOut = (xb_vec2Nx8 *)(pOut + outDataPitch1);
        IVP_SAV2NX8_XP(dvecOut2, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 3rd output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum3, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut3);
        pdvecOut = (xb_vec2Nx8 *)(pOut + (2 * outDataPitch1));
        IVP_SAV2NX8_XP(dvecOut3, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 4th output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum4, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut4);
        pdvecOut = (xb_vec2Nx8 *)(pOut + (3 * outDataPitch1));
        IVP_SAV2NX8_XP(dvecOut4, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
      }  /* for (x = 0; x < outW - 3; x += 4) */
      if (x < outW)
      {
        int32_t numX1 = XT_SALT(1, outW - x);
        int32_t numX2 = XT_SALT(2, outW - x);
        /* Initialize accumulators with bias values */
        phvecBias1 = (xb_vecN_2x32v*)(pBiasData + ch);
        phvecBias2 = (xb_vecN_2x32v*)(pBiasData + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
        xb_vecN_2x32v hvecBias1, hvecBias2, hvecBias3, hvecBias4;
        BIAS_LOAD_FIXUP_SUBTRACT(phvecBias1, phvecBias2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, remCh1, remCh2, remCh3, remCh4);
        daccSum2 = 0; daccSum1 = 0;
        daccSum3 = 0;
        /* Input Data Pointer */
        int8_t *pData = (pInData + ch + (x * stride * inDataPitch1) + (y * stride * inDataPitch2));
        pdvecData1 = (xb_vec2Nx8 *)(pData);
        pdvecData2 = (xb_vec2Nx8 *)(pData + 2 * inDataPitch1);
        pdvecData3 = (xb_vec2Nx8 *)(pData + 4 * inDataPitch1);
        /* Output data Pointer */
        int8_t *pOut = pOutData + y * outDataPitch2 + x * outDataPitch1 + ch;
        /* Pointer for Coefficient Load */
        pdvecCoeff = (xb_vec2Nx8 *)(pCoeffData + ch);

        for (ky = 0; ky < kSizeU; ky++) /* Kernel Height */
        {
          xb_vec2Nx8 dvecCoeff1, dvecCoeff2, dvecCoeff3, dvecCoeff4, dvecCoeff5;
          xb_vec2Nx8 dvecData1, dvecData2, dvecData3, dvecData4;
          xb_vec2Nx8 dvecData5, dvecData6, dvecData7, dvecData8;
          xb_vec2Nx8 dvecData9;
          /* Load Coefficient values */
          IVP_LV2NX8_XP(dvecCoeff1, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff2, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff3, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff4, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff5, pdvecCoeff, (coeffPitch2 - 4 * coeffPitch1));
          /* To produce 3 output rows along width with stride 2,
          * a total of (OutputWidth - 1)*stride + KernelWidth  i.e
          * (3 - 1) * 2 + 5 = 9 input loads across input width are required*/
          /* Input loads*/
          valign vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData1, vaData1, pdvecData1, inDataPitch1);
          vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData2, vaData1, pdvecData1, inDataPitch2 - inDataPitch1);
          valign vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData3, vaData2, pdvecData2, inDataPitch1);
          vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData4, vaData2, pdvecData2, inDataPitch2 - inDataPitch1);
          valign vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData5, vaData3, pdvecData3, inDataPitch1 * numX1);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData6, vaData3, pdvecData3, inDataPitch1 * numX1);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData7, vaData3, pdvecData3, inDataPitch1 * numX2);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData8, vaData3, pdvecData3, inDataPitch1 * numX2);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData9, vaData3, pdvecData3, inDataPitch2 - \
                                            (2 * (numX1 + numX2)) * inDataPitch1);
          /*Multiply and accumulate dvecData1 to dvecData5 for 1st output row*/
          IVP_MULPA2NX8(daccSum1, dvecCoeff1, dvecData1, dvecCoeff2, dvecData2);
          IVP_MULPA2NX8(daccSum1, dvecCoeff3, dvecData3, dvecCoeff4, dvecData4);
          IVP_MULA2NX8(daccSum1, dvecCoeff5, dvecData5);
          /*Multiply and accumulate dvecData3 to dvecData7 for 2nd output row*/
          IVP_MULPA2NX8(daccSum2, dvecCoeff1, dvecData3, dvecCoeff2, dvecData4);
          IVP_MULPA2NX8(daccSum2, dvecCoeff3, dvecData5, dvecCoeff4, dvecData6);
          IVP_MULA2NX8(daccSum2, dvecCoeff5, dvecData7);
          /*Multiply and accumulate dvecData5 to dvecData9 for 3rd output row*/
          IVP_MULPA2NX8(daccSum3, dvecCoeff1, dvecData5, dvecCoeff2, dvecData6);
          IVP_MULPA2NX8(daccSum3, dvecCoeff3, dvecData7, dvecCoeff4, dvecData8);
          IVP_MULA2NX8(daccSum3, dvecCoeff5, dvecData9);
        }/* for (ky = 0; ky < kSizeU; ky++) */
        xb_vec2Nx8 dvecOut1, dvecOut2, dvecOut3;
        /* Store the result for 1st output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum1, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut1);
        /* Store the output vecOut1 along the output depth */
        pdvecOut = (xb_vec2Nx8 *)(pOut);
        IVP_SAV2NX8_XP(dvecOut1, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 2nd output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut2);
        pdvecOut = (xb_vec2Nx8 *)(pOut + outDataPitch1 * numX1);
        IVP_SAV2NX8_XP(dvecOut2, vaOutData, pdvecOut, remainingOutCh * numX1);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 3rd output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum3, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut3);
        pdvecOut = (xb_vec2Nx8 *)(pOut + (2 * outDataPitch1* numX2));
        IVP_SAV2NX8_XP(dvecOut3, vaOutData, pdvecOut, remainingOutCh* numX2);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
      }  /* if(x < outW) */
    }    /* for (y = 0; y < outH; y++) */
  }      /* for (ch = 0; ch < numCh; ch += vectorizationWidth) */
}

/*****************************************************************************
* Stride 4 Sub-variant
* When input stride is 4 this function is called
*  **************************************************************************/
void depthwiseConvolveAVQ2D_S_5x5j4_S8Ca2_MOD_DWH(const xi_pTile3D inTile,
                                                  const xi_pTile3D coeffTile,
                                                  const xi_pArray biasArray,
                                                  const xi_pArray outScaleArray,
                                                  const xi_pArray outShiftArray,
                                                  xi_pTile3D outTile,
                                                  const xi_cnna_conv_params *param)
{
  /* Getting parameters from the tile structures */
  const int32_t numCh         = XI_TILE3D_GET_DIM1(inTile);
  const int32_t outW          = XI_TILE3D_GET_DIM2(outTile);
  const int32_t outH          = XI_TILE3D_GET_DIM3(outTile);
  const int32_t inDataPitch1  = XI_TILE3D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2  = XI_TILE3D_GET_DIM2_PITCH(inTile);
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
  const int32_t coeffPitch1   = XI_TILE3D_GET_DIM1_PITCH(coeffTile);
  const int32_t coeffPitch2   = XI_TILE3D_GET_DIM2_PITCH(coeffTile);
  /* Convolution params */
  const uint8_t enableReLu = XI_CNNA_CONV_GET_FLAG_RELU(param);
  const uint8_t stride     = XI_CNNA_CONV_GET_STRIDE(param);
  const int16_t zeroPtOut  = XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param);
  const int16_t zeroPtIn   = XI_CNNA_CONV_GET_ZEROPT_INPUT(param);
  const int16_t reluMin    = XI_CNNA_CONV_GET_RELUMIN(param);
  const int16_t reluMax    = XI_CNNA_CONV_GET_RELUMAX(param);

  /* Setting the limits for output data according to ReLu is enabled or not*/
  const int16_t minLim = enableReLu ? reluMin : SCHAR_MIN;
  const int16_t maxLim = enableReLu ? reluMax : SCHAR_MAX;
  /* Kernel Size (DWH) */
  const int32_t kSizeU = XI_TILE3D_GET_DIM2(coeffTile);

  /* Data Pointers of input, output, coefficient and bias data */
  int8_t *pInData    = (int8_t *)XI_TILE3D_GET_DATA_PTR(inTile);
  int8_t *pOutData   = (int8_t *)XI_TILE3D_GET_DATA_PTR(outTile);
  int8_t *pCoeffData = (int8_t *)XI_TILE3D_GET_DATA_PTR(coeffTile);
  int32_t *pBiasData = (int32_t *)XI_ARRAY_GET_DATA_PTR(biasArray);
  int32_t *pOutScale = (int32_t *)XI_ARRAY_GET_DATA_PTR(outScaleArray);
  int8_t *pOutShift  = (int8_t *)XI_ARRAY_GET_DATA_PTR(outShiftArray);

  /* Move pointer to the start of the data (including edge) */
  pInData = &pInData[-((kSizeU / 2) * inDataPitch1 + (kSizeU / 2) * inDataPitch2)];

  /* Variable Declarations */
  int32_t ch, x, y, ky;
  int32_t vectorizationWidth = 2 * XCHAL_IVPN_SIMD_WIDTH;
  valign vaOutData = IVP_ZALIGN();
  /* Input and Output data Pointers */
  xb_vecN_2x32v* restrict phvecBias1;
  xb_vecN_2x32v* restrict phvecBias2;
  xb_vecN_2x32v* restrict phvecOutScale1;
  xb_vecN_2x32v* restrict phvecOutScale2;
  xb_vec2Nx8* restrict pdvecOutShift;
  xb_vec2Nx8* restrict pdvecCoeff;
  xb_vec2Nx8* restrict pdvecData1;
  xb_vec2Nx8* restrict pdvecData2;
  xb_vec2Nx8* restrict pdvecData3;
  xb_vec2Nx8* restrict pdvecData4;
  xb_vec2Nx8* restrict pdvecOut;
  /* Accumulator vectors */
  xb_vec2Nx24 daccSum1, daccSum2, daccSum3, daccSum4;

  for (ch = 0; ch < numCh; ch += vectorizationWidth) /* Along Output Channel */
  {
    /* To handle corner case when number of output channels
    * is not a multiple of  2 * XCHAL_IVPN_SIMD_WIDTH*/
    int32_t remainingOutCh = XT_MIN(vectorizationWidth, numCh - ch);
    int32_t remBiasLoad = (remainingOutCh > XCHAL_IVPN_SIMD_WIDTH) ? 1 : 0;

    int32_t remCh1 = XT_MIN(((numCh - ch) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh2 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH / 2) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh3 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh4 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH * 3 / 2) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);

    /* Load OutScale values */
    xb_vecN_2x32v hvecScale1, hvecScale2, hvecScale3, hvecScale4;
    phvecOutScale1 = (xb_vecN_2x32v*)(pOutScale + ch);
    phvecOutScale2 = (xb_vecN_2x32v*)(pOutScale + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
    valign vaOutScale = IVP_LAN_2X32_PP(phvecOutScale1);
    IVP_LAVN_2X32_XP(hvecScale1, vaOutScale, phvecOutScale1, remCh1);
    IVP_LAVN_2X32_XP(hvecScale2, vaOutScale, phvecOutScale1, remCh2);
    vaOutScale = IVP_LAN_2X32_PP(phvecOutScale2);
    IVP_LAVN_2X32_XP(hvecScale3, vaOutScale, phvecOutScale2, remCh3);
    IVP_LAVN_2X32_XP(hvecScale4, vaOutScale, phvecOutScale2, remCh4);

    /* Load OutShift values */
    xb_vec2Nx8 dvecShift;
    pdvecOutShift = (xb_vec2Nx8*)(pOutShift + ch);
    valign vaOutShift = IVP_LA2NX8_PP(pdvecOutShift);
    IVP_LAV2NX8_XP(dvecShift, vaOutShift, pdvecOutShift, remainingOutCh);

    /* Calculate left shift and right shift values */
    vbool2N vb2N = IVP_LT2NX8(dvecShift, 0);
    xb_vec2Nx8 dvecRightShift = IVP_MOV2NX8T(0, dvecShift, vb2N);
    xb_vec2Nx8 dvecLeftShift = 0;
    IVP_SUB2NX8T(dvecLeftShift, 0, dvecShift, vb2N);

    xb_vec2Nx8 dvecRightShiftL, dvecRightShiftH;
    IVP_DSEL2NX8I(dvecRightShiftH, dvecRightShiftL, 0, dvecRightShift, IVP_DSELI_8B_INTERLEAVE_1);

    xb_vecNx16 vecRightShift1 = IVP_MOVNX16_FROM2NX8(dvecRightShiftL);
    xb_vecNx16 vecRightShift2 = IVP_MOVNX16_FROM2NX8(dvecRightShiftH);

    xb_vec2Nx8 dvecLeftShiftL, dvecLeftShiftH;
    IVP_DSEL2NX8I(dvecLeftShiftH, dvecLeftShiftL, 0, dvecLeftShift, IVP_DSELI_8B_INTERLEAVE_1);

    xb_vecNx16 vecLeftShiftLL, vecLeftShiftLH, vecLeftShiftHL, vecLeftShiftHH;
    IVP_DSELNX16I(vecLeftShiftLH, vecLeftShiftLL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftL), IVP_DSELI_INTERLEAVE_1);
    IVP_DSELNX16I(vecLeftShiftHH, vecLeftShiftHL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftH), IVP_DSELI_INTERLEAVE_1);

    xb_vecN_2x32v hvecLeftShift1 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLL);
    xb_vecN_2x32v hvecLeftShift2 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLH);
    xb_vecN_2x32v hvecLeftShift3 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHL);
    xb_vecN_2x32v hvecLeftShift4 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHH);
    /* Fixup calculation */
    daccSum1 = 0;
    int8_t *pCoeff = (pCoeffData + ch);
    CALCULATE_FIXUP(daccSum1, kSizeU, kSizeU, pCoeff);
    /* Fixup term */
    xb_vecNx16 vecSumL = IVP_CVT16S2NX24L(daccSum1);
    xb_vecNx16 vecSumH = IVP_CVT16S2NX24H(daccSum1);
    xb_vecNx16 vecSumOdd, vecSumEven;
    IVP_DSELNX16I(vecSumOdd, vecSumEven, vecSumH, vecSumL, IVP_DSELI_DEINTERLEAVE_1);
    daccSum1 = IVP_MULI2NX8X16((xb_vec2Nx8)zeroPtIn, vecSumOdd, vecSumEven);
    xb_vecN_2x32v hvecFixLL = IVP_CVT32S2NX24LL(daccSum1);
    xb_vecN_2x32v hvecFixLH = IVP_CVT32S2NX24LH(daccSum1);
    xb_vecN_2x32v hvecFixHL = IVP_CVT32S2NX24HL(daccSum1);
    xb_vecN_2x32v hvecFixHH = IVP_CVT32S2NX24HH(daccSum1);
    for (y = 0; y < outH; y++)  /* Along Output Height */
    {
      for (x = 0; x < (outW - 3); x += 4) /* Along Output Width */
      {
        /* Initialize accumulators with bias values */
        phvecBias1 = (xb_vecN_2x32v*)(pBiasData + ch);
        phvecBias2 = (xb_vecN_2x32v*)(pBiasData + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
        xb_vecN_2x32v hvecBias1, hvecBias2, hvecBias3, hvecBias4;
        BIAS_LOAD_FIXUP_SUBTRACT(phvecBias1, phvecBias2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, remCh1, remCh2, remCh3, remCh4);
        daccSum1 = 0;
        daccSum2 = daccSum1;
        daccSum3 = daccSum1;
        daccSum4 = daccSum1;
        /* Input Data Pointer */
        int8_t *pData = (pInData + ch + (x * stride * inDataPitch1) + (y * stride * inDataPitch2));
        pdvecData1 = (xb_vec2Nx8 *)(pData);
        pdvecData2 = (xb_vec2Nx8 *)(pData + 4 * inDataPitch1);
        pdvecData3 = (xb_vec2Nx8 *)(pData + 8 * inDataPitch1);
        pdvecData4 = (xb_vec2Nx8 *)(pData + 12 * inDataPitch1);
        /* Output data Pointer */
        int8_t *pOut = pOutData + y * outDataPitch2 + x * outDataPitch1 + ch;
        /* Pointer for Coefficient Load */
        pdvecCoeff = (xb_vec2Nx8 *)(pCoeffData + ch);
        for (ky = 0; ky < kSizeU; ky++) /* Kernel Height */
        {
          /* Input and Coeff Data vectors */
          xb_vec2Nx8 dvecCoeff1, dvecCoeff2, dvecCoeff3, dvecCoeff4, dvecCoeff5;
          xb_vec2Nx8 dvecData1, dvecData2, dvecData3, dvecData4;
          xb_vec2Nx8 dvecData5, dvecData6, dvecData7, dvecData8, dvecData9;
          xb_vec2Nx8 dvecData10, dvecData11, dvecData12, dvecData13;
          xb_vec2Nx8 dvecData14, dvecData15, dvecData16, dvecData17;
          /* Load Coefficient values */
          IVP_LV2NX8_XP(dvecCoeff1, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff2, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff3, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff4, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff5, pdvecCoeff, (coeffPitch2 - 4 * coeffPitch1));
          /* To produce 4 output rows along width with stride 4,
          * a total of (OutputWidth - 1)*stride + KernelWidth  i.e
          * (4 - 1) * 4 + 5 = 17 input loads across input width are required*/
          /* Input loads*/
          valign vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData1, vaData1, pdvecData1, inDataPitch1);
          vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData2, vaData1, pdvecData1, inDataPitch1);
          vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData3, vaData1, pdvecData1, inDataPitch1);
          vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData4, vaData1, pdvecData1, (inDataPitch2 - (3 * inDataPitch1)));
          valign vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData5, vaData2, pdvecData2, inDataPitch1);
          vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData6, vaData2, pdvecData2, inDataPitch1);
          vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData7, vaData2, pdvecData2, inDataPitch1);
          vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData8, vaData2, pdvecData2, (inDataPitch2 - (3 * inDataPitch1)));
          valign vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData9, vaData3, pdvecData3, inDataPitch1);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData10, vaData3, pdvecData3, inDataPitch1);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData11, vaData3, pdvecData3, inDataPitch1);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData12, vaData3, pdvecData3, (inDataPitch2 - (3 * inDataPitch1)));
          valign vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData13, vaData4, pdvecData4, inDataPitch1);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData14, vaData4, pdvecData4, inDataPitch1);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData15, vaData4, pdvecData4, inDataPitch1);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData16, vaData4, pdvecData4, inDataPitch1);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData17, vaData4, pdvecData4, (inDataPitch2 - (4 * inDataPitch1)));
          /*Multiply and accumulate dvecData1 to dvecData5 for 1st output row*/
          IVP_MULPA2NX8(daccSum1, dvecCoeff1, dvecData1, dvecCoeff2, dvecData2);
          IVP_MULPA2NX8(daccSum1, dvecCoeff3, dvecData3, dvecCoeff4, dvecData4);
          IVP_MULA2NX8(daccSum1, dvecCoeff5, dvecData5);
          /*Multiply and accumulate dvecData5 to dvecData9 for 2nd output row*/
          IVP_MULPA2NX8(daccSum2, dvecCoeff1, dvecData5, dvecCoeff2, dvecData6);
          IVP_MULPA2NX8(daccSum2, dvecCoeff3, dvecData7, dvecCoeff4, dvecData8);
          IVP_MULA2NX8(daccSum2, dvecCoeff5, dvecData9);
          /*Multiply and accumulate dvecData9 to dvecData13 for 3rd output row*/
          IVP_MULPA2NX8(daccSum3, dvecCoeff1, dvecData9, dvecCoeff2, dvecData10);
          IVP_MULPA2NX8(daccSum3, dvecCoeff3, dvecData11, dvecCoeff4, dvecData12);
          IVP_MULA2NX8(daccSum3, dvecCoeff5, dvecData13);
          /*Multiply and accumulate dvecData13 to dvecData17 for 4th output row*/
          IVP_MULPA2NX8(daccSum4, dvecCoeff1, dvecData13, dvecCoeff2, dvecData14);
          IVP_MULPA2NX8(daccSum4, dvecCoeff3, dvecData15, dvecCoeff4, dvecData16);
          IVP_MULA2NX8(daccSum4, dvecCoeff5, dvecData17);
        }
        xb_vec2Nx8 dvecOut1, dvecOut2, dvecOut3, dvecOut4;
        /* Store the result for 1st output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum1, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut1);
        /* Store the output vecOut1 along the output depth */
        pdvecOut = (xb_vec2Nx8 *)(pOut);
        IVP_SAV2NX8_XP(dvecOut1, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 2nd output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut2);
        pdvecOut = (xb_vec2Nx8 *)(pOut + outDataPitch1);
        IVP_SAV2NX8_XP(dvecOut2, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 3rd output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum3, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut3);
        pdvecOut = (xb_vec2Nx8 *)(pOut + (2 * outDataPitch1));
        IVP_SAV2NX8_XP(dvecOut3, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 4th output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum4, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut4);
        pdvecOut = (xb_vec2Nx8 *)(pOut + (3 * outDataPitch1));
        IVP_SAV2NX8_XP(dvecOut4, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
      }  /* for (x = 0; x < outW - 3; x += 4) */
      if (x < outW)
      {
        int32_t numX1 = XT_SALT(1, outW - x);
        int32_t numX2 = XT_SALT(2, outW - x);
        /* Initialize accumulators with bias values */
        phvecBias1 = (xb_vecN_2x32v*)(pBiasData + ch);
        phvecBias2 = (xb_vecN_2x32v*)(pBiasData + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
        xb_vecN_2x32v hvecBias1, hvecBias2, hvecBias3, hvecBias4;
        BIAS_LOAD_FIXUP_SUBTRACT(phvecBias1, phvecBias2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, remCh1, remCh2, remCh3, remCh4);
        daccSum1 = 0;
        daccSum2 = daccSum1;
        daccSum3 = daccSum1;
        /* Input Data Pointer */
        int8_t *pData = (pInData + ch + (x * stride * inDataPitch1) + (y * stride * inDataPitch2));
        pdvecData1 = (xb_vec2Nx8 *)(pData);
        pdvecData2 = (xb_vec2Nx8 *)(pData + (4 * inDataPitch1));
        pdvecData3 = (xb_vec2Nx8 *)(pData + (8 * inDataPitch1 * numX1));
        /* Output data Pointer */
        int8_t *pOut = pOutData + y * outDataPitch2 + x * outDataPitch1 + ch;
        /* Pointer for Coefficient Load */
        pdvecCoeff = (xb_vec2Nx8 *)(pCoeffData + ch);

        for (ky = 0; ky < kSizeU; ky++) /* Kernel Height */
        {
          xb_vec2Nx8 dvecCoeff1, dvecCoeff2, dvecCoeff3, dvecCoeff4, dvecCoeff5;
          xb_vec2Nx8 dvecData1, dvecData2, dvecData3, dvecData4, dvecData5;
          xb_vec2Nx8 dvecData6, dvecData7, dvecData8, dvecData9;
          xb_vec2Nx8 dvecData10, dvecData11, dvecData12, dvecData13;
          /* Load Coefficient values */
          IVP_LV2NX8_XP(dvecCoeff1, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff2, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff3, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff4, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff5, pdvecCoeff, (coeffPitch2 - 4 * coeffPitch1));
          /* To produce 3 output rows along width with stride 4,
           * a total of (OutputWidth - 1)*stride + KernelWidth  i.e
           * (3 - 1) * 4 + 5 = 13 input loads across input width are required*/
          /* Input loads*/
          valign vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData1, vaData1, pdvecData1, inDataPitch1);
          vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData2, vaData1, pdvecData1, inDataPitch1);
          vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData3, vaData1, pdvecData1, inDataPitch1);
          vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData4, vaData1, pdvecData1, inDataPitch2 - (3 * inDataPitch1));
          valign vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData5, vaData2, pdvecData2, inDataPitch1 * numX1);
          vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData6, vaData2, pdvecData2, inDataPitch1 * numX1);
          vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData7, vaData2, pdvecData2, inDataPitch1 * numX1);
          vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData8, vaData2, pdvecData2, inDataPitch2 - \
                                                           (3 * numX1 * inDataPitch1));
          valign vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData9, vaData3, pdvecData3,  inDataPitch1 * numX2);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData10, vaData3, pdvecData3, inDataPitch1 * numX2);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData11, vaData3, pdvecData3, inDataPitch1 * numX2);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData12, vaData3, pdvecData3, inDataPitch1 * numX2);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData13, vaData3, pdvecData3, inDataPitch2 - \
                                                  (4 * numX2 * inDataPitch1));
          /*Multiply and accumulate dvecData1 to dvecData5 for 1st output row*/
          IVP_MULPA2NX8(daccSum1, dvecCoeff1, dvecData1, dvecCoeff2, dvecData2);
          IVP_MULPA2NX8(daccSum1, dvecCoeff3, dvecData3, dvecCoeff4, dvecData4);
          IVP_MULA2NX8(daccSum1, dvecCoeff5, dvecData5);
          /*Multiply and accumulate dvecData5 to dvecData9 for 2nd output row*/
          IVP_MULPA2NX8(daccSum2, dvecCoeff1, dvecData5, dvecCoeff2, dvecData6);
          IVP_MULPA2NX8(daccSum2, dvecCoeff3, dvecData7, dvecCoeff4, dvecData8);
          IVP_MULA2NX8(daccSum2, dvecCoeff5, dvecData9);
          /*Multiply and accumulate dvecData9 to dvecData13 for 3rd output row*/
          IVP_MULPA2NX8(daccSum3, dvecCoeff1, dvecData9, dvecCoeff2, dvecData10);
          IVP_MULPA2NX8(daccSum3, dvecCoeff3, dvecData11, dvecCoeff4, dvecData12);
          IVP_MULA2NX8(daccSum3, dvecCoeff5, dvecData13);
        }/* for (ky = 0; ky < kSizeU; ky++) */
        xb_vec2Nx8 dvecOut1, dvecOut2, dvecOut3;
        /* Store the result for 1st output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum1, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut1);
        /* Store the output vecOut1 along the output depth */
        pdvecOut = (xb_vec2Nx8 *)(pOut);
        IVP_SAV2NX8_XP(dvecOut1, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 2nd output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut2);
        pdvecOut = (xb_vec2Nx8 *)(pOut + outDataPitch1 * numX1);
        IVP_SAV2NX8_XP(dvecOut2, vaOutData, pdvecOut, remainingOutCh * numX1);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 3rd output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum3, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut3);
        pdvecOut = (xb_vec2Nx8 *)(pOut + (2 * outDataPitch1 * numX2));
        IVP_SAV2NX8_XP(dvecOut3, vaOutData, pdvecOut, remainingOutCh* numX2);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
      }  /* if(x < outW) */
    }    /* for (y = 0; y < outH; y++) */
  }      /* for (ch = 0; ch < numCh; ch += vectorizationWidth) */
}

/*****************************************************************************
* Generic Sub-variant for 5x5 DepthwiseConvolveAVQ2D_S_S8Ca2_MOD_DWH
*  **************************************************************************/
void depthwiseConvolveAVQ2D_S_5x5_S8Ca2_MOD_DWH(const xi_pTile3D inTile,
                                                const xi_pTile3D coeffTile,
                                                const xi_pArray biasArray,
                                                const xi_pArray outScaleArray,
                                                const xi_pArray outShiftArray,
                                                xi_pTile3D outTile,
                                                const xi_cnna_conv_params *param)
{

  /* Getting parameters from the tile structures */
  const int32_t numCh         = XI_TILE3D_GET_DIM1(inTile);
  const int32_t outW          = XI_TILE3D_GET_DIM2(outTile);
  const int32_t outH          = XI_TILE3D_GET_DIM3(outTile);
  const int32_t inDataPitch1  = XI_TILE3D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2  = XI_TILE3D_GET_DIM2_PITCH(inTile);
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
  const int32_t coeffPitch1   = XI_TILE3D_GET_DIM1_PITCH(coeffTile);
  const int32_t coeffPitch2   = XI_TILE3D_GET_DIM2_PITCH(coeffTile);
  /* Convolution params */
  const uint8_t enableReLu    = XI_CNNA_CONV_GET_FLAG_RELU(param);
  const uint8_t stride        = XI_CNNA_CONV_GET_STRIDE(param);
  const int16_t zeroPtOut     = XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param);
  const int16_t zeroPtIn      = XI_CNNA_CONV_GET_ZEROPT_INPUT(param);
  const int16_t reluMin       = XI_CNNA_CONV_GET_RELUMIN(param);
  const int16_t reluMax       = XI_CNNA_CONV_GET_RELUMAX(param);

  /* Setting the limits for output data according to ReLu is enabled or not*/
  const int16_t minLim = enableReLu ? reluMin : SCHAR_MIN;
  const int16_t maxLim = enableReLu ? reluMax : SCHAR_MAX;
  /* Kernel Size (DWH) */
  const int32_t kSizeU = XI_TILE3D_GET_DIM2(coeffTile);

  /* Data Pointers of input, output, coefficient and bias data */
  int8_t *pInData    = (int8_t *) XI_TILE3D_GET_DATA_PTR(inTile);
  int8_t *pOutData   = (int8_t *) XI_TILE3D_GET_DATA_PTR(outTile);
  int8_t *pCoeffData = (int8_t *) XI_TILE3D_GET_DATA_PTR(coeffTile);
  int32_t *pBiasData = (int32_t *)XI_ARRAY_GET_DATA_PTR(biasArray);
  int32_t *pOutScale = (int32_t *)XI_ARRAY_GET_DATA_PTR(outScaleArray);
  int8_t *pOutShift  = (int8_t *)XI_ARRAY_GET_DATA_PTR(outShiftArray);

  /* Move pointer to the start of the data (including edge) */
  pInData = &pInData[-((kSizeU / 2) * inDataPitch1 + (kSizeU / 2) * inDataPitch2)];

  /* Variable Declarations */
  int32_t ch, x, y, ky;
  int32_t vectorizationWidth = 2 * XCHAL_IVPN_SIMD_WIDTH;
  valign vaOutData = IVP_ZALIGN();
  /* Input and Output data Pointers */
  xb_vecN_2x32v* restrict phvecBias1;
  xb_vecN_2x32v* restrict phvecBias2;
  xb_vecN_2x32v* restrict phvecOutScale1;
  xb_vecN_2x32v* restrict phvecOutScale2;
  xb_vec2Nx8* restrict pdvecOutShift;
  xb_vec2Nx8* restrict pdvecCoeff;
  xb_vec2Nx8* restrict pdvecData1;
  xb_vec2Nx8* restrict pdvecData2;
  xb_vec2Nx8* restrict pdvecData3;
  xb_vec2Nx8* restrict pdvecData4;
  xb_vec2Nx8* restrict pdvecOut;
  /* Input and Output data Pointers */
  xb_vec2Nx8 dvecData1, dvecData2, dvecData3, dvecData4;
  xb_vec2Nx8 dvecData5, dvecData6, dvecData7, dvecData8;
  xb_vecN_2x32v hvecScale1, hvecScale2, hvecScale3, hvecScale4;
  xb_vec2Nx24 daccSum1, daccSum2, daccSum3, daccSum4;

  for (ch = 0; ch < numCh; ch += vectorizationWidth) /* Along Output Channel */
  {
    /* To handle corner case when number of output channels
     * is not a multiple of  2 * XCHAL_IVPN_SIMD_WIDTH*/
    int32_t remainingOutCh = XT_MIN(vectorizationWidth, numCh - ch);
    int32_t remBiasLoad = (remainingOutCh > XCHAL_IVPN_SIMD_WIDTH) ? 1 : 0;

    int32_t remCh1 = XT_MIN(((numCh - ch) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh2 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH / 2) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh3 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh4 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH * 3 / 2) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);

    /* Load OutScale values */
    phvecOutScale1 = (xb_vecN_2x32v*)(pOutScale + ch);
    phvecOutScale2 = (xb_vecN_2x32v*)(pOutScale + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
    valign vaOutScale = IVP_LAN_2X32_PP(phvecOutScale1);
    IVP_LAVN_2X32_XP(hvecScale1, vaOutScale, phvecOutScale1, remCh1);
    IVP_LAVN_2X32_XP(hvecScale2, vaOutScale, phvecOutScale1, remCh2);
    vaOutScale = IVP_LAN_2X32_PP(phvecOutScale2);
    IVP_LAVN_2X32_XP(hvecScale3, vaOutScale, phvecOutScale2, remCh3);
    IVP_LAVN_2X32_XP(hvecScale4, vaOutScale, phvecOutScale2, remCh4);

    /* Load OutShift values */
    xb_vec2Nx8 dvecShift;
    pdvecOutShift = (xb_vec2Nx8*)(pOutShift + ch);
    valign vaOutShift = IVP_LA2NX8_PP(pdvecOutShift);
    IVP_LAV2NX8_XP(dvecShift, vaOutShift, pdvecOutShift, remainingOutCh);

    /* Calculate left shift and right shift values */
    vbool2N vb2N = IVP_LT2NX8(dvecShift, 0);
    xb_vec2Nx8 dvecRightShift = IVP_MOV2NX8T(0, dvecShift, vb2N);
    xb_vec2Nx8 dvecLeftShift = 0;
    IVP_SUB2NX8T(dvecLeftShift, 0, dvecShift, vb2N);

    xb_vec2Nx8 dvecRightShiftL, dvecRightShiftH;
    IVP_DSEL2NX8I(dvecRightShiftH, dvecRightShiftL, 0, dvecRightShift, IVP_DSELI_8B_INTERLEAVE_1);

    xb_vecNx16 vecRightShift1 = IVP_MOVNX16_FROM2NX8(dvecRightShiftL);
    xb_vecNx16 vecRightShift2 = IVP_MOVNX16_FROM2NX8(dvecRightShiftH);

    xb_vec2Nx8 dvecLeftShiftL, dvecLeftShiftH;
    IVP_DSEL2NX8I(dvecLeftShiftH, dvecLeftShiftL, 0, dvecLeftShift, IVP_DSELI_8B_INTERLEAVE_1);

    xb_vecNx16 vecLeftShiftLL, vecLeftShiftLH, vecLeftShiftHL, vecLeftShiftHH;
    IVP_DSELNX16I(vecLeftShiftLH, vecLeftShiftLL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftL), IVP_DSELI_INTERLEAVE_1);
    IVP_DSELNX16I(vecLeftShiftHH, vecLeftShiftHL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftH), IVP_DSELI_INTERLEAVE_1);

    xb_vecN_2x32v hvecLeftShift1 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLL);
    xb_vecN_2x32v hvecLeftShift2 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLH);
    xb_vecN_2x32v hvecLeftShift3 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHL);
    xb_vecN_2x32v hvecLeftShift4 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHH);
    /* Fixup calculation */
    daccSum1 = 0;
    int8_t *pCoeff = (pCoeffData + ch);
    CALCULATE_FIXUP(daccSum1, kSizeU, kSizeU, pCoeff);
    /* Fixup term */
    xb_vecNx16 vecSumL = IVP_CVT16S2NX24L(daccSum1);
    xb_vecNx16 vecSumH = IVP_CVT16S2NX24H(daccSum1);
    xb_vecNx16 vecSumOdd, vecSumEven;
    IVP_DSELNX16I(vecSumOdd, vecSumEven, vecSumH, vecSumL, IVP_DSELI_DEINTERLEAVE_1);
    daccSum1 = IVP_MULI2NX8X16((xb_vec2Nx8)zeroPtIn, vecSumOdd, vecSumEven);
    xb_vecN_2x32v hvecFixLL = IVP_CVT32S2NX24LL(daccSum1);
    xb_vecN_2x32v hvecFixLH = IVP_CVT32S2NX24LH(daccSum1);
    xb_vecN_2x32v hvecFixHL = IVP_CVT32S2NX24HL(daccSum1);
    xb_vecN_2x32v hvecFixHH = IVP_CVT32S2NX24HH(daccSum1);
    for (y = 0; y < outH; y++)  /* Along Output Height */
    {
      for (x = 0; x < (outW - 3); x += 4) /* Along Output Width */
      {
        /* Initialize accumulators with bias values */
        phvecBias1 = (xb_vecN_2x32v*)(pBiasData + ch);
        phvecBias2 = (xb_vecN_2x32v*)(pBiasData + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
        xb_vecN_2x32v hvecBias1, hvecBias2, hvecBias3, hvecBias4;
        BIAS_LOAD_FIXUP_SUBTRACT(phvecBias1, phvecBias2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, remCh1, remCh2, remCh3, remCh4);
        daccSum1 = 0;
        daccSum2 = daccSum1;
        daccSum3 = daccSum1;
        daccSum4 = daccSum1;
        /* Input Data Pointer */
        int8_t *pData = (pInData + ch + (x * inDataPitch1) + (y * stride * inDataPitch2));
        pdvecData1 = (xb_vec2Nx8 *)(pData);
        pdvecData2 = (xb_vec2Nx8 *)(pData + 2 * inDataPitch1);
        pdvecData3 = (xb_vec2Nx8 *)(pData + 4 * inDataPitch1);
        pdvecData4 = (xb_vec2Nx8 *)(pData + 6 * inDataPitch1);
        /* Output data Pointer */
        int8_t *pOut = pOutData + y * outDataPitch2 + x * outDataPitch1 + ch;
        /* Pointer for Coefficient Load */
        pdvecCoeff = (xb_vec2Nx8 *)(pCoeffData + ch);
        for (ky = 0; ky < kSizeU; ky++) /* Kernel Height */
        {
          xb_vec2Nx8 dvecCoeff1, dvecCoeff2, dvecCoeff3, dvecCoeff4, dvecCoeff5;
          /* Load Coefficient values */
          IVP_LV2NX8_XP(dvecCoeff1, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff2, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff3, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff4, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff5, pdvecCoeff, (coeffPitch2 - 4 * coeffPitch1));
          /* To produce 4 output rows along width with stride 1,
           * a total of (OutputWidth - 1)*stride + KernelWidth  i.e
           * (4 -1)*1 + 5 = 8 input loads across input width are required*/
          /* Input loads*/
          valign vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData1, vaData1, pdvecData1, inDataPitch1);
          vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData2, vaData1, pdvecData1, (inDataPitch2 - inDataPitch1));
          valign vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData3, vaData2, pdvecData2, inDataPitch1);
          vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData4, vaData2, pdvecData2, (inDataPitch2 - inDataPitch1));
          valign vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData5, vaData3, pdvecData3, inDataPitch1);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData6, vaData3, pdvecData3, (inDataPitch2 - inDataPitch1));
          valign vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData7, vaData4, pdvecData4, inDataPitch1);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData8, vaData4, pdvecData4, (inDataPitch2 - inDataPitch1));
          /*Multiply and accumulate dvecData1 to dvecData5 for 1st output row*/
          IVP_MULPA2NX8(daccSum1, dvecCoeff1, dvecData1, dvecCoeff2, dvecData2);
          IVP_MULPA2NX8(daccSum1, dvecCoeff3, dvecData3, dvecCoeff4, dvecData4);
          IVP_MULA2NX8(daccSum1, dvecCoeff5, dvecData5);
          /*Multiply and accumulate dvecData2 to dvecData6 for 2nd output row*/
          IVP_MULPA2NX8(daccSum2, dvecCoeff1, dvecData2, dvecCoeff2, dvecData3);
          IVP_MULPA2NX8(daccSum2, dvecCoeff3, dvecData4, dvecCoeff4, dvecData5);
          IVP_MULA2NX8(daccSum2, dvecCoeff5, dvecData6);
          /*Multiply and accumulate dvecData3 to dvecData7 for 3rd output row*/
          IVP_MULPA2NX8(daccSum3, dvecCoeff1, dvecData3, dvecCoeff2, dvecData4);
          IVP_MULPA2NX8(daccSum3, dvecCoeff3, dvecData5, dvecCoeff4, dvecData6);
          IVP_MULA2NX8(daccSum3, dvecCoeff5, dvecData7);
          /*Multiply and accumulate dvecData4 to dvecData8 for 4th output row*/
          IVP_MULPA2NX8(daccSum4, dvecCoeff1, dvecData4, dvecCoeff2, dvecData5);
          IVP_MULPA2NX8(daccSum4, dvecCoeff3, dvecData6, dvecCoeff4, dvecData7);
          IVP_MULA2NX8(daccSum4, dvecCoeff5, dvecData8);
        }
        xb_vec2Nx8 dvecOut1, dvecOut2, dvecOut3, dvecOut4;
        /* Store the result for 1st output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum1, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut1);
        /* Store the output vecOut1 along the output depth */
        pdvecOut = (xb_vec2Nx8 *)(pOut);
        IVP_SAV2NX8_XP(dvecOut1, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 2nd output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut2);
        pdvecOut = (xb_vec2Nx8 *)(pOut + outDataPitch1);
        IVP_SAV2NX8_XP(dvecOut2, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 3rd output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum3, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut3);
        pdvecOut = (xb_vec2Nx8 *)(pOut + (2 * outDataPitch1));
        IVP_SAV2NX8_XP(dvecOut3, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 4th output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum4, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut4);
        pdvecOut = (xb_vec2Nx8 *)(pOut + (3 * outDataPitch1));
        IVP_SAV2NX8_XP(dvecOut4, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
      }  /* for (x = 0; x < outW - 3; x += 4) */
      if (x < outW)
      {
        int32_t numX1 = XT_SALT(1, outW - x);
        int32_t numX2 = XT_SALT(2, outW - x);
        /* Initialize accumulators with bias values */
        phvecBias1 = (xb_vecN_2x32v*)(pBiasData + ch);
        phvecBias2 = (xb_vecN_2x32v*)(pBiasData + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
        xb_vecN_2x32v hvecBias1, hvecBias2, hvecBias3, hvecBias4;
        BIAS_LOAD_FIXUP_SUBTRACT(phvecBias1, phvecBias2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, remCh1, remCh2, remCh3, remCh4);
        daccSum1 = 0;
        daccSum2 = daccSum1;
        daccSum3 = daccSum1;
        /* Input Data Pointer */
        int8_t *pData = (pInData + ch + (x * inDataPitch1) + (y * stride * inDataPitch2));
        pdvecData1 = (xb_vec2Nx8 *)(pData);
        pdvecData2 = (xb_vec2Nx8 *)(pData + 2 * inDataPitch1);
        pdvecData3 = (xb_vec2Nx8 *)(pData + 4 * inDataPitch1);
        /* Output data Pointer */
        int8_t *pOut = pOutData + y * outDataPitch2 + x * outDataPitch1 + ch;
        /* Pointer for Coefficient Load */
        pdvecCoeff = (xb_vec2Nx8 *)(pCoeffData + ch);

        for (ky = 0; ky < kSizeU; ky++) /* Kernel Height */
        {
          xb_vec2Nx8 dvecCoeff1, dvecCoeff2, dvecCoeff3, dvecCoeff4, dvecCoeff5;
          /* Load Coefficient values */
          IVP_LV2NX8_XP(dvecCoeff1, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff2, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff3, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff4, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff5, pdvecCoeff, (coeffPitch2 - 4 * coeffPitch1));
          /* To produce 3 output rows along width with stride 1,
          * a total of (OutputWidth - 1)*stride + KernelWidth  i.e
          * (3 -1)*1 + 5 = 7 input loads across input width are required*/
          /* Input loads*/
          valign vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData1, vaData1, pdvecData1, inDataPitch1);
          vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData2, vaData1, pdvecData1, (inDataPitch2 - inDataPitch1));
          valign vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData3, vaData2, pdvecData2, inDataPitch1);
          vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData4, vaData2, pdvecData2, (inDataPitch2 - inDataPitch1));
          valign vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData5, vaData3, pdvecData3, inDataPitch1 * numX1);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData6, vaData3, pdvecData3, inDataPitch1 * numX2);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData7, vaData3, pdvecData3, inDataPitch2 - \
                                    (numX1 + numX2) * inDataPitch1);
          /*Multiply and accumulate dvecData1 to dvecData5 for 1st output row*/
          IVP_MULPA2NX8(daccSum1, dvecCoeff1, dvecData1, dvecCoeff2, dvecData2);
          IVP_MULPA2NX8(daccSum1, dvecCoeff3, dvecData3, dvecCoeff4, dvecData4);
          IVP_MULA2NX8(daccSum1, dvecCoeff5, dvecData5);
          /*Multiply and accumulate dvecData2 to dvecData6 for 2nd output row*/
          IVP_MULPA2NX8(daccSum2, dvecCoeff1, dvecData2, dvecCoeff2, dvecData3);
          IVP_MULPA2NX8(daccSum2, dvecCoeff3, dvecData4, dvecCoeff4, dvecData5);
          IVP_MULA2NX8(daccSum2, dvecCoeff5, dvecData6);
          /*Multiply and accumulate dvecData3 to dvecData7 for 3rd output row*/
          IVP_MULPA2NX8(daccSum3, dvecCoeff1, dvecData3, dvecCoeff2, dvecData4);
          IVP_MULPA2NX8(daccSum3, dvecCoeff3, dvecData5, dvecCoeff4, dvecData6);
          IVP_MULA2NX8(daccSum3, dvecCoeff5, dvecData7);
        }/* for (ky = 0; ky < kSizeU; ky++) */
        xb_vec2Nx8 dvecOut1, dvecOut2, dvecOut3;
        /* Store the result for 1st output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum1, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut1);
        /* Store the output vecOut1 along the output depth */
        pdvecOut = (xb_vec2Nx8 *)(pOut);
        IVP_SAV2NX8_XP(dvecOut1, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 2nd output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut2);
        pdvecOut = (xb_vec2Nx8 *)(pOut + outDataPitch1 * numX1);
        IVP_SAV2NX8_XP(dvecOut2, vaOutData, pdvecOut, remainingOutCh * numX1);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 3rd output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum3, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut3);
        pdvecOut = (xb_vec2Nx8 *)(pOut + (2 * outDataPitch1 * numX2));
        IVP_SAV2NX8_XP(dvecOut3, vaOutData, pdvecOut, remainingOutCh* numX2);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
      }  /* if(x < outW) */
    }    /* for (y = 0; y < outH; y++) */
  }      /* for (ch = 0; ch < numCh; ch += vectorizationWidth) */

}

/***************** xiDepthwiseConvolveAVQ2D_S_5x5_S8Ca2_MOD_DWH ***********************/
/**************************************************************************************/
/**************************************************************************************/
/* Description : Optimized implementation of Symmetric 5x5 MOD_DWH                    */
/*               depthwise convolution for Android NN.                                */
/* Inputs      : Input Data Tile, Coeff Data Tile, Bias Array, Output Scale Array,    */
/*               Output Shift Array, CNNA convolution params structure.               */
/* Outputs     : XI Error Code                                                        */
/* InOuts      : Output Tile                                                          */
/* Assumptions : InData, CoeffData are S8                                             */
/*               biasArray is signed 32b, OutData is S8                               */
/*               Kernel Size is 5x5.                                                  */
/*               Input and Output are in DWH format                                   */
/*               Coeff is in DWH format                                               */
/*               CoeffDim1Pitch is aligned to 2N (Ca2)                                */
/**************************************************************************************/
XI_ERR_TYPE xiDepthwiseConvolveAVQ2D_S_5x5_S8Ca2_MOD_DWH(const xi_pTile3D inTile,
                                                         const xi_pTile3D coeffTile,
                                                         const xi_pArray biasArray,
                                                         const xi_pArray outScaleArray,
                                                         const xi_pArray outShiftArray,
                                                         xi_pTile3D outTile,
                                                         const xi_cnna_conv_params *param)
{
  /* Error Checks */
  XI_ERROR_CHECKS()
  {
    XI_CHECK_TILE3D_S8(inTile);
    XI_CHECK_TILE3D_S8(outTile);
    XI_CHECK_TILE3D_S8(coeffTile);
    XI_CHECK_ARRAY_S32(biasArray);
    XI_CHECK_ARRAY_S32(outScaleArray);
    XI_CHECK_ARRAY_S8(outShiftArray);
    XI_CHECK_POINTER(param);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(coeffTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(inTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(coeffTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(biasArray, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(outScaleArray, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(outShiftArray, outTile);
    XI_CHECK_TILE3D_DATA_ORDER(inTile, XI_DWH);
    XI_CHECK_TILE3D_DATA_ORDER(outTile, XI_DWH);
    XI_CHECK_TILE3D_DATA_ORDER(coeffTile, XI_DWH);
    XI_CHECK_KERNEL_SIZE_DEPTHWISE(coeffTile, 5);
    XI_CHECK_TILE3D_EDGE(inTile, 2);
    XI_CHECK_ERROR(((XI_CNNA_CONV_GET_STRIDEX(param) == 1) || (XI_CNNA_CONV_GET_STRIDEX(param) == 2) || (XI_CNNA_CONV_GET_STRIDEX(param) == 4)) && \
                   ((XI_CNNA_CONV_GET_STRIDEY(param) == 1) || (XI_CNNA_CONV_GET_STRIDEY(param) == 2) || (XI_CNNA_CONV_GET_STRIDEY(param) == 4)), \
                   XI_ERR_BADARG, "\nStrideX = %hhu, StrideY = %hhu\nStride value should be equal to 1, 2 or 4", \
                   XI_CNNA_CONV_GET_STRIDEX(param), XI_CNNA_CONV_GET_STRIDEY(param));
    XI_CHECK_ERROR(XI_CNNA_CONV_GET_STRIDEX(param) == XI_CNNA_CONV_GET_STRIDEY(param), XI_ERR_BADARG, \
                   "\nStrideX = %hhu, StrideY = %hhu\nStrideX and StrideY must be same", \
                   XI_CNNA_CONV_GET_STRIDEX(param), XI_CNNA_CONV_GET_STRIDEY(param));
    XI_CHECK_TILE3D_IALIGNMENT_2NX8(coeffTile);
    XI_CHECK_CONSISTENCYA_DEPTHWISE_MOD_DWH(inTile, coeffTile, biasArray, outTile, param);
    XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(outScaleArray) >= XI_TILE3D_GET_DIM1(outTile), XI_ERR_DATASIZE, \
      "\nWidth of Output Scale Array = %d, Number of output channels = %d\nWidth of Output Scale Array should be \
      greater than or equal to Number of output channels", XI_ARRAY_GET_WIDTH(outScaleArray), XI_TILE3D_GET_DIM1(outTile));
    XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(outShiftArray) >= XI_TILE3D_GET_DIM1(outTile), XI_ERR_DATASIZE, \
      "\nWidth of Output Shift Array = %d, Number of output channels = %d\nWidth of Output Shift Array should be \
      greater than or equal to Number of output channels", XI_ARRAY_GET_WIDTH(outShiftArray), XI_TILE3D_GET_DIM1(outTile));
    if (XI_CNNA_CONV_GET_FLAG_RELU(param))
    {
      XI_CHECK_ERROR(XI_CNNA_CONV_GET_RELUMIN(param) <= XI_CNNA_CONV_GET_RELUMAX(param), XI_ERR_BADARG, \
        "\nRelu min = %hi, Relu max = %hi\nRelu max should be greater than or equal to Relu min", \
        XI_CNNA_CONV_GET_RELUMIN(param), XI_CNNA_CONV_GET_RELUMAX(param));
      XI_CHECK_ERROR((XI_CNNA_CONV_GET_RELUMIN(param) >= SCHAR_MIN), XI_ERR_BADARG, \
        "\nRelu min = %hi, value should be greater than or equal to -128", XI_CNNA_CONV_GET_RELUMIN(param));
      XI_CHECK_ERROR((XI_CNNA_CONV_GET_RELUMAX(param) <= SCHAR_MAX), XI_ERR_BADARG, \
        "\nRelu max = %hi, value should be less than or equal to 127", XI_CNNA_CONV_GET_RELUMAX(param));
    }
    XI_CHECK_ERROR(((XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param) >= SCHAR_MIN) && \
                    (XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param) <= SCHAR_MAX)), XI_ERR_BADARG,\
      "\nZero point output = %hi, value should be in the range -128 to 127", XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param));
    XI_CHECK_ERROR(((XI_CNNA_CONV_GET_ZEROPT_INPUT(param) >= SCHAR_MIN) && \
                    (XI_CNNA_CONV_GET_ZEROPT_INPUT(param) <= SCHAR_MAX)), XI_ERR_BADARG,\
      "\nZero point input = %hi, value should be in the range -128 to 127", XI_CNNA_CONV_GET_ZEROPT_INPUT(param));
    XI_CHECK_ERROR((XI_CNNA_CONV_GET_ZEROPT_COEFF(param) == 0), XI_ERR_BADARG,\
      "\nZero point coefficient = %hi, value should be equal to zero", XI_CNNA_CONV_GET_ZEROPT_COEFF(param));
  }

  if (XI_CNNA_CONV_GET_STRIDE(param) == 2)
  {
    depthwiseConvolveAVQ2D_S_5x5j2_S8Ca2_MOD_DWH(inTile, coeffTile, biasArray, outScaleArray, outShiftArray, outTile, param);
  }
  else if (XI_CNNA_CONV_GET_STRIDE(param) == 4)
  {
    depthwiseConvolveAVQ2D_S_5x5j4_S8Ca2_MOD_DWH(inTile, coeffTile, biasArray, outScaleArray, outShiftArray, outTile, param);
  }
  else
  {
    depthwiseConvolveAVQ2D_S_5x5_S8Ca2_MOD_DWH(inTile, coeffTile, biasArray, outScaleArray, outShiftArray, outTile, param);
  }

  return(XI_ERROR_STATUS());
}

/*****************************************************************************
* Stride 2 Sub-variant
* When input stride is 2 this function is called
*  **************************************************************************/

void depthwiseConvolveAVQ2D_S_7x7j2_S8Ca2_MOD_DWH(const xi_pTile3D inTile,
                                                  const xi_pTile3D coeffTile,
                                                  const xi_pArray biasArray,
                                                  const xi_pArray outScaleArray,
                                                  const xi_pArray outShiftArray,
                                                  xi_pTile3D outTile,
                                                  const xi_cnna_conv_params *param)
{
  /* Getting parameters from the tile structures */
  const int32_t numCh         = XI_TILE3D_GET_DIM1(inTile);
  const int32_t outW          = XI_TILE3D_GET_DIM2(outTile);
  const int32_t outH          = XI_TILE3D_GET_DIM3(outTile);
  const int32_t inDataPitch1  = XI_TILE3D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2  = XI_TILE3D_GET_DIM2_PITCH(inTile);
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
  const int32_t coeffPitch1   = XI_TILE3D_GET_DIM1_PITCH(coeffTile);
  const int32_t coeffPitch2   = XI_TILE3D_GET_DIM2_PITCH(coeffTile);
  /* Convolution params */
  const uint8_t enableReLu = XI_CNNA_CONV_GET_FLAG_RELU(param);
  const uint8_t stride     = XI_CNNA_CONV_GET_STRIDE(param);
  const int16_t zeroPtOut  = XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param);
  const int16_t zeroPtIn   = XI_CNNA_CONV_GET_ZEROPT_INPUT(param);
  const int16_t reluMin    = XI_CNNA_CONV_GET_RELUMIN(param);
  const int16_t reluMax    = XI_CNNA_CONV_GET_RELUMAX(param);

  /* Setting the limits for output data according to ReLu is enabled or not*/
  const int16_t minLim = enableReLu ? reluMin : SCHAR_MIN;
  const int16_t maxLim = enableReLu ? reluMax : SCHAR_MAX;
  /* Kernel Size (DWH) */
  const int32_t kSizeU = XI_TILE3D_GET_DIM2(coeffTile);

  /* Data Pointers of input, output, coefficient and bias data */
  int8_t *pInData    = (int8_t *)XI_TILE3D_GET_DATA_PTR(inTile);
  int8_t *pOutData   = (int8_t *)XI_TILE3D_GET_DATA_PTR(outTile);
  int8_t *pCoeffData = (int8_t *)XI_TILE3D_GET_DATA_PTR(coeffTile);
  int32_t *pBiasData = (int32_t *)XI_ARRAY_GET_DATA_PTR(biasArray);
  int32_t *pOutScale = (int32_t *)XI_ARRAY_GET_DATA_PTR(outScaleArray);
  int8_t *pOutShift  = (int8_t *)XI_ARRAY_GET_DATA_PTR(outShiftArray);

  /* Move pointer to the start of the data (including edge) */
  pInData = &pInData[-((kSizeU / 2) * inDataPitch1 + (kSizeU / 2) * inDataPitch2)];

  /* Variable Declarations */
  int32_t ch, x, y, ky;
  int32_t vectorizationWidth = 2 * XCHAL_IVPN_SIMD_WIDTH;
  valign vaOutData = IVP_ZALIGN();
  /* Input and Output data Pointers */
  xb_vecN_2x32v* restrict phvecBias1;
  xb_vecN_2x32v* restrict phvecBias2;
  xb_vecN_2x32v* restrict phvecOutScale1;
  xb_vecN_2x32v* restrict phvecOutScale2;
  xb_vec2Nx8* restrict pdvecOutShift;
  xb_vec2Nx8* restrict pdvecCoeff;
  xb_vec2Nx8* restrict pdvecData1;
  xb_vec2Nx8* restrict pdvecData2;
  xb_vec2Nx8* restrict pdvecData3;
  xb_vec2Nx8* restrict pdvecData4;
  xb_vec2Nx8* restrict pdvecOut;
  /* Accumulator vectors */
  xb_vec2Nx24 daccSum1, daccSum2, daccSum3, daccSum4;

  for (ch = 0; ch < numCh; ch += vectorizationWidth) /* Along Output Channel */
  {
    /* To handle corner case when number of output channels
    * is not a multiple of  2 * XCHAL_IVPN_SIMD_WIDTH*/
    int32_t remainingOutCh = XT_MIN(vectorizationWidth, numCh - ch);
    int32_t remBiasLoad = (remainingOutCh > XCHAL_IVPN_SIMD_WIDTH) ? 1 : 0;

    int32_t remCh1 = XT_MIN(((numCh - ch) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh2 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH / 2) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh3 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh4 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH * 3 / 2) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);

    /* Load OutScale values */
    xb_vecN_2x32v hvecScale1, hvecScale2, hvecScale3, hvecScale4;
    phvecOutScale1 = (xb_vecN_2x32v*)(pOutScale + ch);
    phvecOutScale2 = (xb_vecN_2x32v*)(pOutScale + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
    valign vaOutScale = IVP_LAN_2X32_PP(phvecOutScale1);
    IVP_LAVN_2X32_XP(hvecScale1, vaOutScale, phvecOutScale1, remCh1);
    IVP_LAVN_2X32_XP(hvecScale2, vaOutScale, phvecOutScale1, remCh2);
    vaOutScale = IVP_LAN_2X32_PP(phvecOutScale2);
    IVP_LAVN_2X32_XP(hvecScale3, vaOutScale, phvecOutScale2, remCh3);
    IVP_LAVN_2X32_XP(hvecScale4, vaOutScale, phvecOutScale2, remCh4);


    /* Load OutShift values */
    xb_vec2Nx8 dvecShift;
    pdvecOutShift = (xb_vec2Nx8*)(pOutShift + ch);
    valign vaOutShift = IVP_LA2NX8_PP(pdvecOutShift);
    IVP_LAV2NX8_XP(dvecShift, vaOutShift, pdvecOutShift, remainingOutCh);

    /* Calculate left shift and right shift values */
    vbool2N vb2N = IVP_LT2NX8(dvecShift, 0);
    xb_vec2Nx8 dvecRightShift = IVP_MOV2NX8T(0, dvecShift, vb2N);
    xb_vec2Nx8 dvecLeftShift  = 0;
    IVP_SUB2NX8T(dvecLeftShift, 0, dvecShift, vb2N);

    xb_vec2Nx8 dvecRightShiftL, dvecRightShiftH;
    IVP_DSEL2NX8I(dvecRightShiftH, dvecRightShiftL, 0, dvecRightShift, IVP_DSELI_8B_INTERLEAVE_1);

    xb_vecNx16 vecRightShift1 = IVP_MOVNX16_FROM2NX8(dvecRightShiftL);
    xb_vecNx16 vecRightShift2 = IVP_MOVNX16_FROM2NX8(dvecRightShiftH);

    xb_vec2Nx8 dvecLeftShiftL, dvecLeftShiftH;
    IVP_DSEL2NX8I(dvecLeftShiftH, dvecLeftShiftL, 0, dvecLeftShift, IVP_DSELI_8B_INTERLEAVE_1);

    xb_vecNx16 vecLeftShiftLL, vecLeftShiftLH, vecLeftShiftHL, vecLeftShiftHH;
    IVP_DSELNX16I(vecLeftShiftLH, vecLeftShiftLL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftL), IVP_DSELI_INTERLEAVE_1);
    IVP_DSELNX16I(vecLeftShiftHH, vecLeftShiftHL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftH), IVP_DSELI_INTERLEAVE_1);

    xb_vecN_2x32v hvecLeftShift1 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLL);
    xb_vecN_2x32v hvecLeftShift2 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLH);
    xb_vecN_2x32v hvecLeftShift3 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHL);
    xb_vecN_2x32v hvecLeftShift4 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHH);
    /* Fixup calculation */
    daccSum1 = 0;
    int8_t *pCoeff = (pCoeffData + ch);
    CALCULATE_FIXUP(daccSum1, kSizeU, kSizeU, pCoeff);
    /* Fixup term */
    xb_vecNx16 vecSumL = IVP_CVT16S2NX24L(daccSum1);
    xb_vecNx16 vecSumH = IVP_CVT16S2NX24H(daccSum1);
    xb_vecNx16 vecSumOdd, vecSumEven;
    IVP_DSELNX16I(vecSumOdd, vecSumEven, vecSumH, vecSumL, IVP_DSELI_DEINTERLEAVE_1);
    daccSum1 = IVP_MULI2NX8X16((xb_vec2Nx8)zeroPtIn, vecSumOdd, vecSumEven);
    xb_vecN_2x32v hvecFixLL = IVP_CVT32S2NX24LL(daccSum1);
    xb_vecN_2x32v hvecFixLH = IVP_CVT32S2NX24LH(daccSum1);
    xb_vecN_2x32v hvecFixHL = IVP_CVT32S2NX24HL(daccSum1);
    xb_vecN_2x32v hvecFixHH = IVP_CVT32S2NX24HH(daccSum1);
    for (y = 0; y < outH; y++)  /* Along Output Height */
    {
      for (x = 0; x < (outW - 3); x += 4) /* Along Output Width */
      {
        /* Initialize accumulators with bias values */
        phvecBias1 = (xb_vecN_2x32v*)(pBiasData + ch);
        phvecBias2 = (xb_vecN_2x32v*)(pBiasData + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
        xb_vecN_2x32v hvecBias1, hvecBias2, hvecBias3, hvecBias4;
        BIAS_LOAD_FIXUP_SUBTRACT(phvecBias1, phvecBias2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, remCh1, remCh2, remCh3, remCh4);
        daccSum1 = 0;
        daccSum2 = daccSum1;
        daccSum3 = daccSum1;
        daccSum4 = daccSum1;
        /* Input Data Pointer */
        int8_t *pData = (pInData + ch + (x * stride * inDataPitch1) + (y * stride * inDataPitch2));
        pdvecData1 = (xb_vec2Nx8 *)(pData);
        pdvecData2 = (xb_vec2Nx8 *)(pData + 3 * inDataPitch1);
        pdvecData3 = (xb_vec2Nx8 *)(pData + 6 * inDataPitch1);
        pdvecData4 = (xb_vec2Nx8 *)(pData + 9 * inDataPitch1);
        /* Output data Pointer */
        int8_t *pOut = pOutData + y * outDataPitch2 + x * outDataPitch1 + ch;
        /* Pointer for Coefficient Load */
        pdvecCoeff = (xb_vec2Nx8 *)(pCoeffData + ch);
        for (ky = 0; ky < kSizeU; ky++) /* Kernel Height */
        {
          /* Input and Coeff Data vectors */
          xb_vec2Nx8 dvecCoeff1, dvecCoeff2, dvecCoeff3, dvecCoeff4;
          xb_vec2Nx8 dvecCoeff5, dvecCoeff6, dvecCoeff7;
          xb_vec2Nx8 dvecData1, dvecData2, dvecData3, dvecData4;
          xb_vec2Nx8 dvecData5, dvecData6, dvecData7, dvecData8;
          xb_vec2Nx8 dvecData9, dvecData10, dvecData11, dvecData12, dvecData13;
          /* Load Coefficient values */
          IVP_LV2NX8_XP(dvecCoeff1, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff2, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff3, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff4, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff5, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff6, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff7, pdvecCoeff, (coeffPitch2 - 6 * coeffPitch1));
          /* To produce 4 output rows along width with stride 2,
           * a total of (OutputWidth - 1)*stride + KernelWidth  i.e
           * (4 - 1) * 2 + 7 = 13 input loads across input width are required*/
          /* Input loads*/
          valign vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData1, vaData1, pdvecData1, inDataPitch1);
          vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData2, vaData1, pdvecData1, inDataPitch1);
          vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData3, vaData1, pdvecData1, (inDataPitch2 - (2 * inDataPitch1)));
          valign vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData4, vaData2, pdvecData2, inDataPitch1);
          vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData5, vaData2, pdvecData2, inDataPitch1);
          vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData6, vaData2, pdvecData2, (inDataPitch2 - (2 * inDataPitch1)));
          valign vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData7, vaData3, pdvecData3, inDataPitch1);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData8, vaData3, pdvecData3, inDataPitch1);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData9, vaData3, pdvecData3, (inDataPitch2 - (2 * inDataPitch1)));
          valign vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData10, vaData4, pdvecData4, inDataPitch1);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData11, vaData4, pdvecData4, inDataPitch1);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData12, vaData4, pdvecData4, inDataPitch1);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData13, vaData4, pdvecData4, (inDataPitch2 - (3 * inDataPitch1)));
          /*Multiply and accumulate dvecData1 to dvecData7 for 1st output row*/
          IVP_MULPA2NX8(daccSum1, dvecCoeff1, dvecData1, dvecCoeff2, dvecData2);
          IVP_MULPA2NX8(daccSum1, dvecCoeff3, dvecData3, dvecCoeff4, dvecData4);
          IVP_MULPA2NX8(daccSum1, dvecCoeff5, dvecData5, dvecCoeff6, dvecData6);
          IVP_MULA2NX8(daccSum1, dvecCoeff7, dvecData7);
          /*Multiply and accumulate dvecData3 to dvecData9 for 2nd output row*/
          IVP_MULPA2NX8(daccSum2, dvecCoeff1, dvecData3, dvecCoeff2, dvecData4);
          IVP_MULPA2NX8(daccSum2, dvecCoeff3, dvecData5, dvecCoeff4, dvecData6);
          IVP_MULPA2NX8(daccSum2, dvecCoeff5, dvecData7, dvecCoeff6, dvecData8);
          IVP_MULA2NX8(daccSum2, dvecCoeff7, dvecData9);
          /*Multiply and accumulate dvecData5 to dvecData11 for 3rd output row*/
          IVP_MULPA2NX8(daccSum3, dvecCoeff1, dvecData5, dvecCoeff2, dvecData6);
          IVP_MULPA2NX8(daccSum3, dvecCoeff3, dvecData7, dvecCoeff4, dvecData8);
          IVP_MULPA2NX8(daccSum3, dvecCoeff5, dvecData9, dvecCoeff6, dvecData10);
          IVP_MULA2NX8(daccSum3, dvecCoeff7, dvecData11);
          /*Multiply and accumulate dvecData7 to dvecData13 for 4th output row*/
          IVP_MULPA2NX8(daccSum4, dvecCoeff1, dvecData7, dvecCoeff2, dvecData8);
          IVP_MULPA2NX8(daccSum4, dvecCoeff3, dvecData9, dvecCoeff4, dvecData10);
          IVP_MULPA2NX8(daccSum4, dvecCoeff5, dvecData11, dvecCoeff6, dvecData12);
          IVP_MULA2NX8(daccSum4, dvecCoeff7, dvecData13);
        }
        xb_vec2Nx8 dvecOut1, dvecOut2, dvecOut3, dvecOut4;
        /* Store the result for 1st output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum1, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut1);
        /* Store the output vecOut1 along the output depth */
        pdvecOut = (xb_vec2Nx8 *)(pOut);
        IVP_SAV2NX8_XP(dvecOut1, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 2nd output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut2);
        pdvecOut = (xb_vec2Nx8 *)(pOut + outDataPitch1);
        IVP_SAV2NX8_XP(dvecOut2, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 3rd output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum3, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut3);
        pdvecOut = (xb_vec2Nx8 *)(pOut + (2 * outDataPitch1));
        IVP_SAV2NX8_XP(dvecOut3, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 4th output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum4, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut4);
        pdvecOut = (xb_vec2Nx8 *)(pOut + (3 * outDataPitch1));
        IVP_SAV2NX8_XP(dvecOut4, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
      }  /* for (x = 0; x < outW - 3; x += 4) */
      if (x < outW)
      {
        int32_t numX1 = XT_SALT(1, outW - x);
        int32_t numX2 = XT_SALT(2, outW - x);
        /* Initialize accumulators with bias values */
        phvecBias1 = (xb_vecN_2x32v*)(pBiasData + ch);
        phvecBias2 = (xb_vecN_2x32v*)(pBiasData + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
        xb_vecN_2x32v hvecBias1, hvecBias2, hvecBias3, hvecBias4;
        BIAS_LOAD_FIXUP_SUBTRACT(phvecBias1, phvecBias2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, remCh1, remCh2, remCh3, remCh4);
        daccSum1 = 0;
        daccSum2 = daccSum1;
        daccSum3 = daccSum1;
        /* Input Data Pointer */
        int8_t *pData = (pInData + ch + (x * stride * inDataPitch1) + (y * stride * inDataPitch2));
        pdvecData1 = (xb_vec2Nx8 *)(pData);
        pdvecData2 = (xb_vec2Nx8 *)(pData + 2 * inDataPitch1);
        pdvecData3 = (xb_vec2Nx8 *)(pData + 4 * inDataPitch1);
        pdvecData4 = (xb_vec2Nx8 *)(pData + 6 * inDataPitch1);
        /* Output data Pointer */
        int8_t *pOut = pOutData + y * outDataPitch2 + x * outDataPitch1 + ch;
        /* Pointer for Coefficient Load */
        pdvecCoeff = (xb_vec2Nx8 *)(pCoeffData + ch);

        for (ky = 0; ky < kSizeU; ky++) /* Kernel Height */
        {
          xb_vec2Nx8 dvecCoeff1, dvecCoeff2, dvecCoeff3, dvecCoeff4;
          xb_vec2Nx8 dvecCoeff5, dvecCoeff6, dvecCoeff7;
          xb_vec2Nx8 dvecData1, dvecData2, dvecData3, dvecData4;
          xb_vec2Nx8 dvecData5, dvecData6, dvecData7, dvecData8;
          xb_vec2Nx8 dvecData9, dvecData10, dvecData11;
          /* Load Coefficient values */
          IVP_LV2NX8_XP(dvecCoeff1, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff2, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff3, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff4, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff5, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff6, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff7, pdvecCoeff, (coeffPitch2 - 6 * coeffPitch1));
          /* To produce 3 output rows along width with stride 2,
          * a total of (OutputWidth - 1)*stride + KernelWidth  i.e
          * (3 - 1) * 2 + 7 = 11 input loads across input width are required*/
          /* Input loads*/
          valign vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData1, vaData1, pdvecData1, inDataPitch1);
          vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData2, vaData1, pdvecData1, inDataPitch2 - inDataPitch1);
          valign vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData3, vaData2, pdvecData2, inDataPitch1);
          vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData4, vaData2, pdvecData2, inDataPitch2 - inDataPitch1);
          valign vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData5, vaData3, pdvecData3, inDataPitch1);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData6, vaData3, pdvecData3, inDataPitch2 - inDataPitch1);
          valign vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData7, vaData4, pdvecData4, inDataPitch1 * numX1);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData8, vaData4, pdvecData4, inDataPitch1 * numX1);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData9, vaData4, pdvecData4, inDataPitch1 * numX2);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData10, vaData4, pdvecData4, inDataPitch1 * numX2);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData11, vaData4, pdvecData4, inDataPitch2 - \
                                            (2 * (numX1 + numX2)) * inDataPitch1);
          /*Multiply and accumulate dvecData1 to dvecData7 for 1st output row*/
          IVP_MULPA2NX8(daccSum1, dvecCoeff1, dvecData1, dvecCoeff2, dvecData2);
          IVP_MULPA2NX8(daccSum1, dvecCoeff3, dvecData3, dvecCoeff4, dvecData4);
          IVP_MULPA2NX8(daccSum1, dvecCoeff5, dvecData5, dvecCoeff6, dvecData6);
          IVP_MULA2NX8(daccSum1, dvecCoeff7, dvecData7);
          /*Multiply and accumulate dvecData3 to dvecData9 for 2nd output row*/
          IVP_MULPA2NX8(daccSum2, dvecCoeff1, dvecData3, dvecCoeff2, dvecData4);
          IVP_MULPA2NX8(daccSum2, dvecCoeff3, dvecData5, dvecCoeff4, dvecData6);
          IVP_MULPA2NX8(daccSum2, dvecCoeff5, dvecData7, dvecCoeff6, dvecData8);
          IVP_MULA2NX8(daccSum2, dvecCoeff7, dvecData9);
          /*Multiply and accumulate dvecData5 to dvecData11 for 3rd output row*/
          IVP_MULPA2NX8(daccSum3, dvecCoeff1, dvecData5, dvecCoeff2, dvecData6);
          IVP_MULPA2NX8(daccSum3, dvecCoeff3, dvecData7, dvecCoeff4, dvecData8);
          IVP_MULPA2NX8(daccSum3, dvecCoeff5, dvecData9, dvecCoeff6, dvecData10);
          IVP_MULA2NX8(daccSum3, dvecCoeff7, dvecData11);
        }/* for (ky = 0; ky < kSizeU; ky++) */
        xb_vec2Nx8 dvecOut1, dvecOut2, dvecOut3;
        /* Store the result for 1st output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum1, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut1);
        /* Store the output vecOut1 along the output depth */
        pdvecOut = (xb_vec2Nx8 *)(pOut);
        IVP_SAV2NX8_XP(dvecOut1, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 2nd output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut2);
        pdvecOut = (xb_vec2Nx8 *)(pOut + outDataPitch1 * numX1);
        IVP_SAV2NX8_XP(dvecOut2, vaOutData, pdvecOut, remainingOutCh * numX1);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 3rd output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum3, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut3);
        pdvecOut = (xb_vec2Nx8 *)(pOut + (2 * outDataPitch1 * numX2));
        IVP_SAV2NX8_XP(dvecOut3, vaOutData, pdvecOut, remainingOutCh* numX2);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
      }  /* if(x < outW) */
    }    /* for (y = 0; y < outH; y++) */
  }      /* for (ch = 0; ch < numCh; ch += vectorizationWidth) */
}

/*****************************************************************************
* Stride 4 Sub-variant
* When input stride is 4 this function is called
*  **************************************************************************/
void depthwiseConvolveAVQ2D_S_7x7j4_S8Ca2_MOD_DWH(const xi_pTile3D inTile,
                                                  const xi_pTile3D coeffTile,
                                                  const xi_pArray biasArray,
                                                  const xi_pArray outScaleArray,
                                                  const xi_pArray outShiftArray,
                                                  xi_pTile3D outTile,
                                                  const xi_cnna_conv_params *param)
{
  /* Getting parameters from the tile structures */
  const int32_t numCh         = XI_TILE3D_GET_DIM1(inTile);
  const int32_t outW          = XI_TILE3D_GET_DIM2(outTile);
  const int32_t outH          = XI_TILE3D_GET_DIM3(outTile);
  const int32_t inDataPitch1  = XI_TILE3D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2  = XI_TILE3D_GET_DIM2_PITCH(inTile);
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
  const int32_t coeffPitch1   = XI_TILE3D_GET_DIM1_PITCH(coeffTile);
  const int32_t coeffPitch2   = XI_TILE3D_GET_DIM2_PITCH(coeffTile);
  /* Convolution params */
  const uint8_t enableReLu = XI_CNNA_CONV_GET_FLAG_RELU(param);
  const uint8_t stride     = XI_CNNA_CONV_GET_STRIDE(param);
  const int16_t zeroPtOut  = XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param);
  const int16_t zeroPtIn   = XI_CNNA_CONV_GET_ZEROPT_INPUT(param);
  const int16_t reluMin    = XI_CNNA_CONV_GET_RELUMIN(param);
  const int16_t reluMax    = XI_CNNA_CONV_GET_RELUMAX(param);

  /* Setting the limits for output data according to ReLu is enabled or not*/
  const int16_t minLim = enableReLu ? reluMin : SCHAR_MIN;
  const int16_t maxLim = enableReLu ? reluMax : SCHAR_MAX;
  /* Kernel Size (DWH) */
  const int32_t kSizeU = XI_TILE3D_GET_DIM2(coeffTile);

  /* Data Pointers of input, output, coefficient and bias data */
  int8_t *pInData    = (int8_t *)XI_TILE3D_GET_DATA_PTR(inTile);
  int8_t *pOutData   = (int8_t *)XI_TILE3D_GET_DATA_PTR(outTile);
  int8_t *pCoeffData = (int8_t *)XI_TILE3D_GET_DATA_PTR(coeffTile);
  int32_t *pBiasData = (int32_t *)XI_ARRAY_GET_DATA_PTR(biasArray);
  int32_t *pOutScale = (int32_t *)XI_ARRAY_GET_DATA_PTR(outScaleArray);
  int8_t *pOutShift  = (int8_t *)XI_ARRAY_GET_DATA_PTR(outShiftArray);

  /* Move pointer to the start of the data (including edge) */
  pInData = &pInData[-((kSizeU / 2) * inDataPitch1 + (kSizeU / 2) * inDataPitch2)];

  /* Variable Declarations */
  int32_t ch, x, y, ky;
  int32_t vectorizationWidth = 2 * XCHAL_IVPN_SIMD_WIDTH;
  valign vaOutData = IVP_ZALIGN();
  /* Input and Output data Pointers */
  xb_vecN_2x32v* restrict phvecBias1;
  xb_vecN_2x32v* restrict phvecBias2;
  xb_vecN_2x32v* restrict phvecOutScale1;
  xb_vecN_2x32v* restrict phvecOutScale2;
  xb_vec2Nx8* restrict pdvecOutShift;
  xb_vec2Nx8* restrict pdvecCoeff;
  xb_vec2Nx8* restrict pdvecData1;
  xb_vec2Nx8* restrict pdvecData2;
  xb_vec2Nx8* restrict pdvecData3;
  xb_vec2Nx8* restrict pdvecData4;
  xb_vec2Nx8* restrict pdvecOut;
  /* Accumulator vectors */
  xb_vec2Nx24 daccSum1, daccSum2, daccSum3, daccSum4;

  for (ch = 0; ch < numCh; ch += vectorizationWidth) /* Along Output Channel */
  {
    /* To handle corner case when number of output channels
     * is not a multiple of  2 * XCHAL_IVPN_SIMD_WIDTH*/
    int32_t remainingOutCh = XT_MIN(vectorizationWidth, numCh - ch);
    int32_t remBiasLoad = (remainingOutCh > XCHAL_IVPN_SIMD_WIDTH) ? 1 : 0;

    int32_t remCh1 = XT_MIN(((numCh - ch) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh2 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH / 2) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh3 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh4 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH * 3 / 2) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);

    /* Load OutScale values */
    xb_vecN_2x32v hvecScale1, hvecScale2, hvecScale3, hvecScale4;
    phvecOutScale1 = (xb_vecN_2x32v*)(pOutScale + ch);
    phvecOutScale2 = (xb_vecN_2x32v*)(pOutScale + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
    valign vaOutScale = IVP_LAN_2X32_PP(phvecOutScale1);
    IVP_LAVN_2X32_XP(hvecScale1, vaOutScale, phvecOutScale1, remCh1);
    IVP_LAVN_2X32_XP(hvecScale2, vaOutScale, phvecOutScale1, remCh2);
    vaOutScale = IVP_LAN_2X32_PP(phvecOutScale2);
    IVP_LAVN_2X32_XP(hvecScale3, vaOutScale, phvecOutScale2, remCh3);
    IVP_LAVN_2X32_XP(hvecScale4, vaOutScale, phvecOutScale2, remCh4);


    /* Load OutShift values */
    xb_vec2Nx8 dvecShift;
    pdvecOutShift = (xb_vec2Nx8*)(pOutShift + ch);
    valign vaOutShift = IVP_LA2NX8_PP(pdvecOutShift);
    IVP_LAV2NX8_XP(dvecShift, vaOutShift, pdvecOutShift, remainingOutCh);

    /* Calculate left shift and right shift values */
    vbool2N vb2N = IVP_LT2NX8(dvecShift, 0);
    xb_vec2Nx8 dvecRightShift = IVP_MOV2NX8T(0, dvecShift, vb2N);
    xb_vec2Nx8 dvecLeftShift = 0;
    IVP_SUB2NX8T(dvecLeftShift, 0, dvecShift, vb2N);

    xb_vec2Nx8 dvecRightShiftL, dvecRightShiftH;
    IVP_DSEL2NX8I(dvecRightShiftH, dvecRightShiftL, 0, dvecRightShift, IVP_DSELI_8B_INTERLEAVE_1);

    xb_vecNx16 vecRightShift1 = IVP_MOVNX16_FROM2NX8(dvecRightShiftL);
    xb_vecNx16 vecRightShift2 = IVP_MOVNX16_FROM2NX8(dvecRightShiftH);

    xb_vec2Nx8 dvecLeftShiftL, dvecLeftShiftH;
    IVP_DSEL2NX8I(dvecLeftShiftH, dvecLeftShiftL, 0, dvecLeftShift, IVP_DSELI_8B_INTERLEAVE_1);

    xb_vecNx16 vecLeftShiftLL, vecLeftShiftLH, vecLeftShiftHL, vecLeftShiftHH;
    IVP_DSELNX16I(vecLeftShiftLH, vecLeftShiftLL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftL), IVP_DSELI_INTERLEAVE_1);
    IVP_DSELNX16I(vecLeftShiftHH, vecLeftShiftHL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftH), IVP_DSELI_INTERLEAVE_1);

    xb_vecN_2x32v hvecLeftShift1 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLL);
    xb_vecN_2x32v hvecLeftShift2 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLH);
    xb_vecN_2x32v hvecLeftShift3 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHL);
    xb_vecN_2x32v hvecLeftShift4 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHH);
    /* Fixup calculation */
    daccSum1 = 0;
    int8_t *pCoeff = (pCoeffData + ch);
    CALCULATE_FIXUP(daccSum1, kSizeU, kSizeU, pCoeff);
    /* Fixup term */
    xb_vecNx16 vecSumL = IVP_CVT16S2NX24L(daccSum1);
    xb_vecNx16 vecSumH = IVP_CVT16S2NX24H(daccSum1);
    xb_vecNx16 vecSumOdd, vecSumEven;
    IVP_DSELNX16I(vecSumOdd, vecSumEven, vecSumH, vecSumL, IVP_DSELI_DEINTERLEAVE_1);
    daccSum1 = IVP_MULI2NX8X16((xb_vec2Nx8)zeroPtIn, vecSumOdd, vecSumEven);
    xb_vecN_2x32v hvecFixLL = IVP_CVT32S2NX24LL(daccSum1);
    xb_vecN_2x32v hvecFixLH = IVP_CVT32S2NX24LH(daccSum1);
    xb_vecN_2x32v hvecFixHL = IVP_CVT32S2NX24HL(daccSum1);
    xb_vecN_2x32v hvecFixHH = IVP_CVT32S2NX24HH(daccSum1);
    for (y = 0; y < outH; y++)  /* Along Output Height */
    {
      for (x = 0; x < (outW - 3); x += 4) /* Along Output Width */
      {
        /* Initialize accumulators with bias values */
        phvecBias1 = (xb_vecN_2x32v*)(pBiasData + ch);
        phvecBias2 = (xb_vecN_2x32v*)(pBiasData + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
        xb_vecN_2x32v hvecBias1, hvecBias2, hvecBias3, hvecBias4;
        BIAS_LOAD_FIXUP_SUBTRACT(phvecBias1, phvecBias2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, remCh1, remCh2, remCh3, remCh4);
        daccSum1 = 0;
        daccSum2 = daccSum1;
        daccSum3 = daccSum1;
        daccSum4 = daccSum1;
        /* Input Data Pointer */
        int8_t *pData = (pInData + ch + (x * stride * inDataPitch1) + (y * stride * inDataPitch2));
        pdvecData1 = (xb_vec2Nx8 *)(pData);
        pdvecData2 = (xb_vec2Nx8 *)(pData + 5 * inDataPitch1);
        pdvecData3 = (xb_vec2Nx8 *)(pData + 10 * inDataPitch1);
        pdvecData4 = (xb_vec2Nx8 *)(pData + 15 * inDataPitch1);
        /* Output data Pointer */
        int8_t *pOut = pOutData + y * outDataPitch2 + x * outDataPitch1 + ch;
        /* Pointer for Coefficient Load */
        pdvecCoeff = (xb_vec2Nx8 *)(pCoeffData + ch);
        for (ky = 0; ky < kSizeU; ky++) /* Kernel Height */
        {
          /* Input and Coeff Data vectors */
          xb_vec2Nx8 dvecCoeff1, dvecCoeff2, dvecCoeff3, dvecCoeff4;
          xb_vec2Nx8 dvecCoeff5, dvecCoeff6, dvecCoeff7;
          xb_vec2Nx8 dvecData1, dvecData2, dvecData3, dvecData4, dvecData5;
          xb_vec2Nx8 dvecData6, dvecData7, dvecData8, dvecData9, dvecData10;
          xb_vec2Nx8 dvecData11, dvecData12, dvecData13, dvecData14, dvecData15;
          xb_vec2Nx8 dvecData16, dvecData17, dvecData18, dvecData19;
          /* Load Coefficient values */
          IVP_LV2NX8_XP(dvecCoeff1, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff2, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff3, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff4, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff5, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff6, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff7, pdvecCoeff, (coeffPitch2 - 6 * coeffPitch1));
          /* To produce 4 output rows along width with stride 4,
          * a total of (OutputWidth - 1)*stride + KernelWidth  i.e
          * (4 - 1) * 4 + 7 = 19 input loads across input width are required*/
          /* Input loads*/
          valign vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData1, vaData1, pdvecData1, inDataPitch1);
          vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData2, vaData1, pdvecData1, inDataPitch1);
          vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData3, vaData1, pdvecData1, inDataPitch1);
          vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData4, vaData1, pdvecData1, inDataPitch1);
          vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData5, vaData1, pdvecData1, (inDataPitch2 - (4 * inDataPitch1)));
          valign vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData6, vaData2, pdvecData2, inDataPitch1);
          vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData7, vaData2, pdvecData2, inDataPitch1);
          vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData8, vaData2, pdvecData2, inDataPitch1);
          vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData9, vaData2, pdvecData2, inDataPitch1);
          vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData10, vaData2, pdvecData2, (inDataPitch2 - (4 * inDataPitch1)));
          valign vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData11, vaData3, pdvecData3, inDataPitch1);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData12, vaData3, pdvecData3, inDataPitch1);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData13, vaData3, pdvecData3, inDataPitch1);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData14, vaData3, pdvecData3, inDataPitch1);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData15, vaData3, pdvecData3, (inDataPitch2 - (4 * inDataPitch1)));
          valign vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData16, vaData4, pdvecData4, inDataPitch1);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData17, vaData4, pdvecData4, inDataPitch1);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData18, vaData4, pdvecData4, inDataPitch1);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData19, vaData4, pdvecData4, (inDataPitch2 - (3 * inDataPitch1)));
          /*Multiply and accumulate dvecData1 to dvecData7 for 1st output row*/
          IVP_MULPA2NX8(daccSum1, dvecCoeff1, dvecData1, dvecCoeff2, dvecData2);
          IVP_MULPA2NX8(daccSum1, dvecCoeff3, dvecData3, dvecCoeff4, dvecData4);
          IVP_MULPA2NX8(daccSum1, dvecCoeff5, dvecData5, dvecCoeff6, dvecData6);
          IVP_MULA2NX8(daccSum1, dvecCoeff7, dvecData7);
          /*Multiply and accumulate dvecData5 to dvecData11 for 2nd output row*/
          IVP_MULPA2NX8(daccSum2, dvecCoeff1, dvecData5, dvecCoeff2, dvecData6);
          IVP_MULPA2NX8(daccSum2, dvecCoeff3, dvecData7, dvecCoeff4, dvecData8);
          IVP_MULPA2NX8(daccSum2, dvecCoeff5, dvecData9, dvecCoeff6, dvecData10);
          IVP_MULA2NX8(daccSum2, dvecCoeff7, dvecData11);
          /*Multiply and accumulate dvecData9 to dvecData15 for 3rd output row*/
          IVP_MULPA2NX8(daccSum3, dvecCoeff1, dvecData9, dvecCoeff2, dvecData10);
          IVP_MULPA2NX8(daccSum3, dvecCoeff3, dvecData11, dvecCoeff4, dvecData12);
          IVP_MULPA2NX8(daccSum3, dvecCoeff5, dvecData13, dvecCoeff6, dvecData14);
          IVP_MULA2NX8(daccSum3, dvecCoeff7, dvecData15);
          /*Multiply and accumulate dvecData13 to dvecData19 for 4th output row*/
          IVP_MULPA2NX8(daccSum4, dvecCoeff1, dvecData13, dvecCoeff2, dvecData14);
          IVP_MULPA2NX8(daccSum4, dvecCoeff3, dvecData15, dvecCoeff4, dvecData16);
          IVP_MULPA2NX8(daccSum4, dvecCoeff5, dvecData17, dvecCoeff6, dvecData18);
          IVP_MULA2NX8(daccSum4, dvecCoeff7, dvecData19);
        }
        xb_vec2Nx8 dvecOut1, dvecOut2, dvecOut3, dvecOut4;
        /* Store the result for 1st output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum1, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut1);
        /* Store the output vecOut1 along the output depth */
        pdvecOut = (xb_vec2Nx8 *)(pOut);
        IVP_SAV2NX8_XP(dvecOut1, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 2nd output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut2);
        pdvecOut = (xb_vec2Nx8 *)(pOut + outDataPitch1);
        IVP_SAV2NX8_XP(dvecOut2, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 3rd output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum3, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut3);
        pdvecOut = (xb_vec2Nx8 *)(pOut + (2 * outDataPitch1));
        IVP_SAV2NX8_XP(dvecOut3, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 4th output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum4, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut4);
        pdvecOut = (xb_vec2Nx8 *)(pOut + (3 * outDataPitch1));
        IVP_SAV2NX8_XP(dvecOut4, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
      }  /* for (x = 0; x < outW - 3; x += 4) */
      if (x < outW)
      {
        int32_t numX1 = XT_SALT(1, outW - x);
        int32_t numX2 = XT_SALT(2, outW - x);
        /* Initialize accumulators with bias values */
        phvecBias1 = (xb_vecN_2x32v*)(pBiasData + ch);
        phvecBias2 = (xb_vecN_2x32v*)(pBiasData + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
        xb_vecN_2x32v hvecBias1, hvecBias2, hvecBias3, hvecBias4;
        BIAS_LOAD_FIXUP_SUBTRACT(phvecBias1, phvecBias2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, remCh1, remCh2, remCh3, remCh4);
        daccSum1 = 0;
        daccSum2 = daccSum1;
        daccSum3 = daccSum1;
        /* Input Data Pointer */
        int8_t *pData = (pInData + ch + (x * stride * inDataPitch1) + (y * stride * inDataPitch2));
        pdvecData1 = (xb_vec2Nx8 *)(pData);
        pdvecData2 = (xb_vec2Nx8 *)(pData + (2 * inDataPitch1));
        pdvecData3 = (xb_vec2Nx8 *)(pData + (4 * inDataPitch1));
        pdvecData4 = (xb_vec2Nx8 *)(pData + (6 * inDataPitch1));
        /* Output data Pointer */
        int8_t *pOut = pOutData + y * outDataPitch2 + x * outDataPitch1 + ch;
        /* Pointer for Coefficient Load */
        pdvecCoeff = (xb_vec2Nx8 *)(pCoeffData + ch);
        for (ky = 0; ky < kSizeU; ky++) /* Kernel Height */
        {
          xb_vec2Nx8 dvecCoeff1, dvecCoeff2, dvecCoeff3, dvecCoeff4;
          xb_vec2Nx8 dvecCoeff5, dvecCoeff6, dvecCoeff7;
          xb_vec2Nx8 dvecData1, dvecData2, dvecData3, dvecData4, dvecData5;
          xb_vec2Nx8 dvecData6, dvecData7, dvecData8, dvecData9, dvecData10;
          xb_vec2Nx8 dvecData11, dvecData12, dvecData13, dvecData14, dvecData15;
          /* Load Coefficient values */
          IVP_LV2NX8_XP(dvecCoeff1, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff2, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff3, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff4, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff5, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff6, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff7, pdvecCoeff, (coeffPitch2 - 6 * coeffPitch1));
          /* To produce 3 output rows along width with stride 4,
           * a total of (OutputWidth - 1)*stride + KernelWidth  i.e
           * (3 - 1) * 4 + 7 = 15 input loads across input width are required*/
          /* Input loads*/
          valign vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData1, vaData1, pdvecData1, inDataPitch1);
          vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData2, vaData1, pdvecData1, inDataPitch2 - inDataPitch1);
          valign vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData3, vaData2, pdvecData2, inDataPitch1);
          vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData4, vaData2, pdvecData2, inDataPitch2 - inDataPitch1);
          valign vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData5, vaData3, pdvecData3, inDataPitch1);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData6, vaData3, pdvecData3, inDataPitch2 - inDataPitch1);
          valign vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData7, vaData4, pdvecData4, inDataPitch1 * numX1);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData8, vaData4, pdvecData4, inDataPitch1 * numX1);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData9, vaData4, pdvecData4, inDataPitch1 * numX1);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData10, vaData4, pdvecData4, inDataPitch1 * numX1);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData11, vaData4, pdvecData4, inDataPitch1 * numX2);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData12, vaData4, pdvecData4, inDataPitch1 * numX2);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData13, vaData4, pdvecData4, inDataPitch1 * numX2);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData14, vaData4, pdvecData4, inDataPitch1 * numX2);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData15, vaData4, pdvecData4, inDataPitch2 - \
                                                  (4 *(numX1 + numX2) * inDataPitch1));
          /*Multiply and accumulate dvecData1 to dvecData7 for 1st output row*/
          IVP_MULPA2NX8(daccSum1, dvecCoeff1, dvecData1, dvecCoeff2, dvecData2);
          IVP_MULPA2NX8(daccSum1, dvecCoeff3, dvecData3, dvecCoeff4, dvecData4);
          IVP_MULPA2NX8(daccSum1, dvecCoeff5, dvecData5, dvecCoeff6, dvecData6);
          IVP_MULA2NX8(daccSum1, dvecCoeff7, dvecData7);
          /*Multiply and accumulate dvecData5 to dvecData11 for 2nd output row*/
          IVP_MULPA2NX8(daccSum2, dvecCoeff1, dvecData5, dvecCoeff2, dvecData6);
          IVP_MULPA2NX8(daccSum2, dvecCoeff3, dvecData7, dvecCoeff4, dvecData8);
          IVP_MULPA2NX8(daccSum2, dvecCoeff5, dvecData9, dvecCoeff6, dvecData10);
          IVP_MULA2NX8(daccSum2, dvecCoeff7, dvecData11);
          /*Multiply and accumulate dvecData9 to dvecData15 for 3rd output row*/
          IVP_MULPA2NX8(daccSum3, dvecCoeff1, dvecData9, dvecCoeff2, dvecData10);
          IVP_MULPA2NX8(daccSum3, dvecCoeff3, dvecData11, dvecCoeff4, dvecData12);
          IVP_MULPA2NX8(daccSum3, dvecCoeff5, dvecData13, dvecCoeff6, dvecData14);
          IVP_MULA2NX8(daccSum3, dvecCoeff7, dvecData15);
        }/* for (ky = 0; ky < kSizeU; ky++) */
        xb_vec2Nx8 dvecOut1, dvecOut2, dvecOut3;
        /* Store the result for 1st output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum1, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut1);
        /* Store the output vecOut1 along the output depth */
        pdvecOut = (xb_vec2Nx8 *)(pOut);
        IVP_SAV2NX8_XP(dvecOut1, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 2nd output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut2);
        pdvecOut = (xb_vec2Nx8 *)(pOut + outDataPitch1 * numX1);
        IVP_SAV2NX8_XP(dvecOut2, vaOutData, pdvecOut, remainingOutCh * numX1);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 3rd output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum3, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut3);
        pdvecOut = (xb_vec2Nx8 *)(pOut + (2 * outDataPitch1  * numX2));
        IVP_SAV2NX8_XP(dvecOut3, vaOutData, pdvecOut, remainingOutCh* numX2);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
      }  /* if(x < outW) */
    }    /* for (y = 0; y < outH; y++) */
  }      /* for (ch = 0; ch < numCh; ch += vectorizationWidth) */
}

/*****************************************************************************
* Generic Sub-variant for 7x7 DepthwiseConvolveAVQ2D_S_S8Ca2_MOD_DWH
*  **************************************************************************/
void depthwiseConvolveAVQ2D_S_7x7_S8Ca2_MOD_DWH(const xi_pTile3D inTile,
                                                const xi_pTile3D coeffTile,
                                                const xi_pArray biasArray,
                                                const xi_pArray outScaleArray,
                                                const xi_pArray outShiftArray,
                                                xi_pTile3D outTile,
                                                const xi_cnna_conv_params *param)
{

  /* Getting parameters from the tile structures */
  const int32_t numCh         = XI_TILE3D_GET_DIM1(inTile);
  const int32_t outW          = XI_TILE3D_GET_DIM2(outTile);
  const int32_t outH          = XI_TILE3D_GET_DIM3(outTile);
  const int32_t inDataPitch1  = XI_TILE3D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2  = XI_TILE3D_GET_DIM2_PITCH(inTile);
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
  const int32_t coeffPitch1   = XI_TILE3D_GET_DIM1_PITCH(coeffTile);
  const int32_t coeffPitch2   = XI_TILE3D_GET_DIM2_PITCH(coeffTile);
  /* Convolution params */
  const uint8_t enableReLu    = XI_CNNA_CONV_GET_FLAG_RELU(param);
  const uint8_t stride        = XI_CNNA_CONV_GET_STRIDE(param);
  const int16_t zeroPtOut     = XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param);
  const int16_t zeroPtIn      = XI_CNNA_CONV_GET_ZEROPT_INPUT(param);
  const int16_t reluMin       = XI_CNNA_CONV_GET_RELUMIN(param);
  const int16_t reluMax       = XI_CNNA_CONV_GET_RELUMAX(param);

  /* Setting the limits for output data according to ReLu is enabled or not*/
  const int16_t minLim = enableReLu ? reluMin : SCHAR_MIN;
  const int16_t maxLim = enableReLu ? reluMax : SCHAR_MAX;
  /* Kernel Size (DWH) */
  const int32_t kSizeU = XI_TILE3D_GET_DIM2(coeffTile);

  /* Data Pointers of input, output, coefficient and bias data */
  int8_t *pInData    = (int8_t *) XI_TILE3D_GET_DATA_PTR(inTile);
  int8_t *pOutData   = (int8_t *) XI_TILE3D_GET_DATA_PTR(outTile);
  int8_t *pCoeffData = (int8_t *) XI_TILE3D_GET_DATA_PTR(coeffTile);
  int32_t *pBiasData = (int32_t *)XI_ARRAY_GET_DATA_PTR(biasArray);
  int32_t *pOutScale = (int32_t *)XI_ARRAY_GET_DATA_PTR(outScaleArray);
  int8_t *pOutShift  = (int8_t *)XI_ARRAY_GET_DATA_PTR(outShiftArray);

  /* Move pointer to the start of the data (including edge) */
  pInData = &pInData[-((kSizeU / 2) * inDataPitch1 + (kSizeU / 2) * inDataPitch2)];

  /* Variable Declarations */
  int32_t ch, x, y, ky;
  int32_t vectorizationWidth = 2 * XCHAL_IVPN_SIMD_WIDTH;
  valign vaOutData = IVP_ZALIGN();
  /* Input and Output data Pointers */
  xb_vecN_2x32v* restrict phvecBias1;
  xb_vecN_2x32v* restrict phvecBias2;
  xb_vecN_2x32v* restrict phvecOutScale1;
  xb_vecN_2x32v* restrict phvecOutScale2;
  xb_vec2Nx8* restrict pdvecOutShift;
  xb_vec2Nx8* restrict pdvecCoeff;
  xb_vec2Nx8* restrict pdvecData1;
  xb_vec2Nx8* restrict pdvecData2;
  xb_vec2Nx8* restrict pdvecData3;
  xb_vec2Nx8* restrict pdvecData4;
  xb_vec2Nx8* restrict pdvecOut;
  /* Input and Output data Pointers */
  xb_vec2Nx8 dvecData1, dvecData2, dvecData3, dvecData4, dvecData5;
  xb_vec2Nx8 dvecData6, dvecData7, dvecData8, dvecData9, dvecData10;
  xb_vecN_2x32v hvecScale1, hvecScale2, hvecScale3, hvecScale4;
  xb_vec2Nx24 daccSum1, daccSum2, daccSum3, daccSum4;

  for (ch = 0; ch < numCh; ch += vectorizationWidth) /* Along Output Channel */
  {
    /* To handle corner case when number of output channels
     * is not a multiple of  2 * XCHAL_IVPN_SIMD_WIDTH*/
    int32_t remainingOutCh = XT_MIN(vectorizationWidth, numCh - ch);
    int32_t remBiasLoad = (remainingOutCh > XCHAL_IVPN_SIMD_WIDTH) ? 1 : 0;

    int32_t remCh1 = XT_MIN(((numCh - ch) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh2 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH / 2) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh3 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh4 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH * 3 / 2) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);

    /* Load OutScale values */
    phvecOutScale1 = (xb_vecN_2x32v*)(pOutScale + ch);
    phvecOutScale2 = (xb_vecN_2x32v*)(pOutScale + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
    valign vaOutScale = IVP_LAN_2X32_PP(phvecOutScale1);
    IVP_LAVN_2X32_XP(hvecScale1, vaOutScale, phvecOutScale1, remCh1);
    IVP_LAVN_2X32_XP(hvecScale2, vaOutScale, phvecOutScale1, remCh2);
    vaOutScale = IVP_LAN_2X32_PP(phvecOutScale2);
    IVP_LAVN_2X32_XP(hvecScale3, vaOutScale, phvecOutScale2, remCh3);
    IVP_LAVN_2X32_XP(hvecScale4, vaOutScale, phvecOutScale2, remCh4);

    /* Load OutShift values */
    xb_vec2Nx8 dvecShift;
    pdvecOutShift = (xb_vec2Nx8*)(pOutShift + ch);
    valign vaOutShift = IVP_LA2NX8_PP(pdvecOutShift);
    IVP_LAV2NX8_XP(dvecShift, vaOutShift, pdvecOutShift, remainingOutCh);

    /* Calculate left shift and right shift values */
    vbool2N vb2N = IVP_LT2NX8(dvecShift, 0);
    xb_vec2Nx8 dvecRightShift = IVP_MOV2NX8T(0, dvecShift, vb2N);
    xb_vec2Nx8 dvecLeftShift = 0;
    IVP_SUB2NX8T(dvecLeftShift, 0, dvecShift, vb2N);

    xb_vec2Nx8 dvecRightShiftL, dvecRightShiftH;
    IVP_DSEL2NX8I(dvecRightShiftH, dvecRightShiftL, 0, dvecRightShift, IVP_DSELI_8B_INTERLEAVE_1);

    xb_vecNx16 vecRightShift1 = IVP_MOVNX16_FROM2NX8(dvecRightShiftL);
    xb_vecNx16 vecRightShift2 = IVP_MOVNX16_FROM2NX8(dvecRightShiftH);

    xb_vec2Nx8 dvecLeftShiftL, dvecLeftShiftH;
    IVP_DSEL2NX8I(dvecLeftShiftH, dvecLeftShiftL, 0, dvecLeftShift, IVP_DSELI_8B_INTERLEAVE_1);

    xb_vecNx16 vecLeftShiftLL, vecLeftShiftLH, vecLeftShiftHL, vecLeftShiftHH;
    IVP_DSELNX16I(vecLeftShiftLH, vecLeftShiftLL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftL), IVP_DSELI_INTERLEAVE_1);
    IVP_DSELNX16I(vecLeftShiftHH, vecLeftShiftHL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftH), IVP_DSELI_INTERLEAVE_1);

    xb_vecN_2x32v hvecLeftShift1 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLL);
    xb_vecN_2x32v hvecLeftShift2 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLH);
    xb_vecN_2x32v hvecLeftShift3 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHL);
    xb_vecN_2x32v hvecLeftShift4 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHH);
    /* Fixup calculation */
    daccSum1 = 0;
    int8_t *pCoeff = (pCoeffData + ch);
    CALCULATE_FIXUP(daccSum1, kSizeU, kSizeU, pCoeff);
    /* Fixup term */
    xb_vecNx16 vecSumL = IVP_CVT16S2NX24L(daccSum1);
    xb_vecNx16 vecSumH = IVP_CVT16S2NX24H(daccSum1);
    xb_vecNx16 vecSumOdd, vecSumEven;
    IVP_DSELNX16I(vecSumOdd, vecSumEven, vecSumH, vecSumL, IVP_DSELI_DEINTERLEAVE_1);
    daccSum1 = IVP_MULI2NX8X16((xb_vec2Nx8)zeroPtIn, vecSumOdd, vecSumEven);
    xb_vecN_2x32v hvecFixLL = IVP_CVT32S2NX24LL(daccSum1);
    xb_vecN_2x32v hvecFixLH = IVP_CVT32S2NX24LH(daccSum1);
    xb_vecN_2x32v hvecFixHL = IVP_CVT32S2NX24HL(daccSum1);
    xb_vecN_2x32v hvecFixHH = IVP_CVT32S2NX24HH(daccSum1);
    for (y = 0; y < outH; y++)  /* Along Output Height */
    {
      for (x = 0; x < (outW - 3); x += 4) /* Along Output Width */
      {
        /* Initialize accumulators with bias values */
        phvecBias1 = (xb_vecN_2x32v*)(pBiasData + ch);
        phvecBias2 = (xb_vecN_2x32v*)(pBiasData + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
        xb_vecN_2x32v hvecBias1, hvecBias2, hvecBias3, hvecBias4;
        BIAS_LOAD_FIXUP_SUBTRACT(phvecBias1, phvecBias2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, remCh1, remCh2, remCh3, remCh4);
        daccSum1 = 0;
        daccSum2 = daccSum1;
        daccSum3 = daccSum1;
        daccSum4 = daccSum1;
        /* Input Data Pointer */
        int8_t *pData = (pInData + ch + (x * stride * inDataPitch1) + (y * stride * inDataPitch2));
        pdvecData1 = (xb_vec2Nx8 *)(pData);
        pdvecData2 = (xb_vec2Nx8 *)(pData + 2 * inDataPitch1);
        pdvecData3 = (xb_vec2Nx8 *)(pData + 4 * inDataPitch1);
        pdvecData4 = (xb_vec2Nx8 *)(pData + 7 * inDataPitch1);
        /* Output data Pointer */
        int8_t *pOut = pOutData + y * outDataPitch2 + x * outDataPitch1 + ch;
        /* Pointer for Coefficient Load */
        pdvecCoeff = (xb_vec2Nx8 *)(pCoeffData + ch);
        for (ky = 0; ky < kSizeU; ky++) /* Kernel Height */
        {
          xb_vec2Nx8 dvecCoeff1, dvecCoeff2, dvecCoeff3, dvecCoeff4;
          xb_vec2Nx8 dvecCoeff5, dvecCoeff6, dvecCoeff7;
          /* Load Coefficient values */
          IVP_LV2NX8_XP(dvecCoeff1, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff2, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff3, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff4, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff5, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff6, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff7, pdvecCoeff, (coeffPitch2 - 6 * coeffPitch1));
          /* To produce 4 output rows along width with stride 1,
           * a total of (OutputWidth - 1)*stride + KernelWidth  i.e
           * (4 - 1)*1 + 7 = 10 input loads across input width are required*/
          /* Input loads*/
          valign vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData1, vaData1, pdvecData1, inDataPitch1);
          vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData2, vaData1, pdvecData1, (inDataPitch2 - inDataPitch1));
          valign vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData3, vaData2, pdvecData2, inDataPitch1);
          vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData4, vaData2, pdvecData2, (inDataPitch2 - inDataPitch1));
          valign vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData5, vaData3, pdvecData3, inDataPitch1);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData6, vaData3, pdvecData3, inDataPitch1);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData7, vaData3, pdvecData3, (inDataPitch2 - (2 * inDataPitch1)));
          valign vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData8, vaData4, pdvecData4, inDataPitch1);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData9, vaData4, pdvecData4, inDataPitch1);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData10, vaData4, pdvecData4, (inDataPitch2 - (2 * inDataPitch1)));
          /*Multiply and accumulate dvecData1 to dvecData7 for 1st output row*/
          IVP_MULPA2NX8(daccSum1, dvecCoeff1, dvecData1, dvecCoeff2, dvecData2);
          IVP_MULPA2NX8(daccSum1, dvecCoeff3, dvecData3, dvecCoeff4, dvecData4);
          IVP_MULPA2NX8(daccSum1, dvecCoeff5, dvecData5, dvecCoeff6, dvecData6);
          IVP_MULA2NX8(daccSum1, dvecCoeff7, dvecData7);
          /*Multiply and accumulate dvecData2 to dvecData8 for 2nd output row*/
          IVP_MULPA2NX8(daccSum2, dvecCoeff1, dvecData2, dvecCoeff2, dvecData3);
          IVP_MULPA2NX8(daccSum2, dvecCoeff3, dvecData4, dvecCoeff4, dvecData5);
          IVP_MULPA2NX8(daccSum2, dvecCoeff5, dvecData6, dvecCoeff6, dvecData7);
          IVP_MULA2NX8(daccSum2, dvecCoeff7, dvecData8);
          /*Multiply and accumulate dvecData3 to dvecData9 for 3rd output row*/
          IVP_MULPA2NX8(daccSum3, dvecCoeff1, dvecData3, dvecCoeff2, dvecData4);
          IVP_MULPA2NX8(daccSum3, dvecCoeff3, dvecData5, dvecCoeff4, dvecData6);
          IVP_MULPA2NX8(daccSum3, dvecCoeff5, dvecData7, dvecCoeff6, dvecData8);
          IVP_MULA2NX8(daccSum3, dvecCoeff7, dvecData9);
          /*Multiply and accumulate dvecData4 to dvecData10 for 4th output row*/
          IVP_MULPA2NX8(daccSum4, dvecCoeff1, dvecData4, dvecCoeff2, dvecData5);
          IVP_MULPA2NX8(daccSum4, dvecCoeff3, dvecData6, dvecCoeff4, dvecData7);
          IVP_MULPA2NX8(daccSum4, dvecCoeff5, dvecData8, dvecCoeff6, dvecData9);
          IVP_MULA2NX8(daccSum4, dvecCoeff7, dvecData10);
        }
        xb_vec2Nx8 dvecOut1, dvecOut2, dvecOut3, dvecOut4;
        /* Store the result for 1st output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum1, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut1);
        /* Store the output vecOut1 along the output depth */
        pdvecOut = (xb_vec2Nx8 *)(pOut);
        IVP_SAV2NX8_XP(dvecOut1, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 2nd output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut2);
        pdvecOut = (xb_vec2Nx8 *)(pOut + outDataPitch1);
        IVP_SAV2NX8_XP(dvecOut2, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 3rd output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum3, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut3);
        pdvecOut = (xb_vec2Nx8 *)(pOut + (2 * outDataPitch1));
        IVP_SAV2NX8_XP(dvecOut3, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 4th output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum4, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut4);
        pdvecOut = (xb_vec2Nx8 *)(pOut + (3 * outDataPitch1));
        IVP_SAV2NX8_XP(dvecOut4, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
      }  /* for (x = 0; x < outW - 3; x += 4) */
      if (x < outW)
      {
        int32_t numX1 = XT_SALT(1, outW - x);
        int32_t numX2 = XT_SALT(2, outW - x);
        /* Initialize accumulators with bias values */
        phvecBias1 = (xb_vecN_2x32v*)(pBiasData + ch);
        phvecBias2 = (xb_vecN_2x32v*)(pBiasData + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
        xb_vecN_2x32v hvecBias1, hvecBias2, hvecBias3, hvecBias4;
        BIAS_LOAD_FIXUP_SUBTRACT(phvecBias1, phvecBias2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, remCh1, remCh2, remCh3, remCh4);
        daccSum1 = 0;
        daccSum2 = daccSum1;
        daccSum3 = daccSum1;
        /* Input Data Pointer */
        int8_t *pData = (pInData + ch + (x * stride * inDataPitch1) + (y * stride * inDataPitch2));
        pdvecData1 = (xb_vec2Nx8 *)(pData);
        pdvecData2 = (xb_vec2Nx8 *)(pData + 2 * inDataPitch1);
        pdvecData3 = (xb_vec2Nx8 *)(pData + 4 * inDataPitch1);
        pdvecData4 = (xb_vec2Nx8 *)(pData + 6 * inDataPitch1);
        /* Output data Pointer */
        int8_t *pOut = pOutData + y * outDataPitch2 + x * outDataPitch1 + ch;
        /* Pointer for Coefficient Load */
        pdvecCoeff = (xb_vec2Nx8 *)(pCoeffData + ch);

        for (ky = 0; ky < kSizeU; ky++) /* Kernel Height */
        {
          xb_vec2Nx8 dvecCoeff1, dvecCoeff2, dvecCoeff3, dvecCoeff4;
          xb_vec2Nx8 dvecCoeff5, dvecCoeff6, dvecCoeff7;
          /* Load Coefficient values */
          IVP_LV2NX8_XP(dvecCoeff1, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff2, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff3, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff4, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff5, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff6, pdvecCoeff, coeffPitch1);
          IVP_LV2NX8_XP(dvecCoeff7, pdvecCoeff, (coeffPitch2 - 6 * coeffPitch1));
          /* To produce 3 output rows along width with stride 1,
          * a total of (OutputWidth - 1)*stride + KernelWidth  i.e
          * (3 -1)*1 + 7 = 9 input loads across input width are required*/
          /* Input loads*/
          valign vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData1, vaData1, pdvecData1, inDataPitch1);
          vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData2, vaData1, pdvecData1, (inDataPitch2 - inDataPitch1));
          valign vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData3, vaData2, pdvecData2, inDataPitch1);
          vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData4, vaData2, pdvecData2, (inDataPitch2 - inDataPitch1));
          valign vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData5, vaData3, pdvecData3, inDataPitch1);
          vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData6, vaData3, pdvecData3, (inDataPitch2 - inDataPitch1));
          valign vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData7, vaData4, pdvecData4, inDataPitch1 * numX1);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData8, vaData4, pdvecData4, inDataPitch1 * numX2);
          vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData9, vaData4, pdvecData4, inDataPitch2 - \
                                         (numX1 + numX2) * inDataPitch1);
          /*Multiply and accumulate dvecData1 to dvecData7 for 1st output row*/
          IVP_MULPA2NX8(daccSum1, dvecCoeff1, dvecData1, dvecCoeff2, dvecData2);
          IVP_MULPA2NX8(daccSum1, dvecCoeff3, dvecData3, dvecCoeff4, dvecData4);
          IVP_MULPA2NX8(daccSum1, dvecCoeff5, dvecData5, dvecCoeff6, dvecData6);
          IVP_MULA2NX8(daccSum1, dvecCoeff7, dvecData7);
          /*Multiply and accumulate dvecData2 to dvecData8 for 2nd output row*/
          IVP_MULPA2NX8(daccSum2, dvecCoeff1, dvecData2, dvecCoeff2, dvecData3);
          IVP_MULPA2NX8(daccSum2, dvecCoeff3, dvecData4, dvecCoeff4, dvecData5);
          IVP_MULPA2NX8(daccSum2, dvecCoeff5, dvecData6, dvecCoeff6, dvecData7);
          IVP_MULA2NX8(daccSum2, dvecCoeff7, dvecData8);
          /*Multiply and accumulate dvecData3 to dvecData9 for 3rd output row*/
          IVP_MULPA2NX8(daccSum3, dvecCoeff1, dvecData3, dvecCoeff2, dvecData4);
          IVP_MULPA2NX8(daccSum3, dvecCoeff3, dvecData5, dvecCoeff4, dvecData6);
          IVP_MULPA2NX8(daccSum3, dvecCoeff5, dvecData7, dvecCoeff6, dvecData8);
          IVP_MULA2NX8(daccSum3, dvecCoeff7, dvecData9);
        }/* for (ky = 0; ky < kSizeU; ky++) */
        xb_vec2Nx8 dvecOut1, dvecOut2, dvecOut3;
        /* Store the result for 1st output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum1, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut1);
        /* Store the output vecOut1 along the output depth */
        pdvecOut = (xb_vec2Nx8 *)(pOut);
        IVP_SAV2NX8_XP(dvecOut1, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 2nd output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut2);
        pdvecOut = (xb_vec2Nx8 *)(pOut + outDataPitch1 * numX1);
        IVP_SAV2NX8_XP(dvecOut2, vaOutData, pdvecOut, remainingOutCh * numX1);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for 3rd output row */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum3, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut3);
        pdvecOut = (xb_vec2Nx8 *)(pOut + (2 * outDataPitch1  * numX2));
        IVP_SAV2NX8_XP(dvecOut3, vaOutData, pdvecOut, remainingOutCh* numX2);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
      }  /* if(x < outW) */
    }    /* for (y = 0; y < outH; y++) */
  }      /* for (ch = 0; ch < numCh; ch += vectorizationWidth) */
}


/***************** xiDepthwiseConvolveAVQ2D_S_7x7_S8Ca2_MOD_DWH ***********************/
/**************************************************************************************/
/**************************************************************************************/
/* Description : Optimized implementation of Symmetric 7x7 MOD_DWH                    */
/*               depthwise convolution for Android NN.                                */
/* Inputs      : Input Data Tile, Coeff Data Tile, Bias Array, Output Scale Array,    */
/*               Output Shift Array, CNNA convolution params structure.               */
/* Outputs     : XI Error Code                                                        */
/* InOuts      : Output Tile                                                          */
/* Assumptions : InData, CoeffData are S8                                             */
/*               biasArray is signed 32b, OutData is S8                               */
/*               Kernel Size is 7x7.                                                  */
/*               Input and Output are in DWH format                                   */
/*               Coeff is in DWH format                                               */
/*               CoeffDim1Pitch is aligned to 2N (Ca2)                                */
/**************************************************************************************/
XI_ERR_TYPE xiDepthwiseConvolveAVQ2D_S_7x7_S8Ca2_MOD_DWH(const xi_pTile3D inTile,
                                                         const xi_pTile3D coeffTile,
                                                         const xi_pArray biasArray,
                                                         const xi_pArray outScaleArray,
                                                         const xi_pArray outShiftArray,
                                                         xi_pTile3D outTile,
                                                         const xi_cnna_conv_params *param)
{
  /* Error Checks */
  XI_ERROR_CHECKS()
  {
    XI_CHECK_TILE3D_S8(inTile);
    XI_CHECK_TILE3D_S8(outTile);
    XI_CHECK_TILE3D_S8(coeffTile);
    XI_CHECK_ARRAY_S32(biasArray);
    XI_CHECK_ARRAY_S32(outScaleArray);
    XI_CHECK_ARRAY_S8(outShiftArray);
    XI_CHECK_POINTER(param);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(coeffTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(inTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(coeffTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(biasArray, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(outScaleArray, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(outShiftArray, outTile);
    XI_CHECK_TILE3D_DATA_ORDER(inTile, XI_DWH);
    XI_CHECK_TILE3D_DATA_ORDER(outTile, XI_DWH);
    XI_CHECK_TILE3D_DATA_ORDER(coeffTile, XI_DWH);
    XI_CHECK_KERNEL_SIZE_DEPTHWISE(coeffTile, 7);
    XI_CHECK_TILE3D_EDGE(inTile, 3);
    XI_CHECK_ERROR(((XI_CNNA_CONV_GET_STRIDEX(param) == 1) || (XI_CNNA_CONV_GET_STRIDEX(param) == 2) || (XI_CNNA_CONV_GET_STRIDEX(param) == 4)) && \
                   ((XI_CNNA_CONV_GET_STRIDEY(param) == 1) || (XI_CNNA_CONV_GET_STRIDEY(param) == 2) || (XI_CNNA_CONV_GET_STRIDEY(param) == 4)), \
                   XI_ERR_BADARG, "\nStrideX = %hhu, StrideY = %hhu\nStride value should be equal to 1, 2 or 4", \
                   XI_CNNA_CONV_GET_STRIDEX(param), XI_CNNA_CONV_GET_STRIDEY(param));
    XI_CHECK_ERROR(XI_CNNA_CONV_GET_STRIDEX(param) == XI_CNNA_CONV_GET_STRIDEY(param), XI_ERR_BADARG, \
                   "\nStrideX = %hhu, StrideY = %hhu\nStrideX and StrideY must be same", \
                   XI_CNNA_CONV_GET_STRIDEX(param), XI_CNNA_CONV_GET_STRIDEY(param));
    XI_CHECK_TILE3D_IALIGNMENT_2NX8(coeffTile);
    XI_CHECK_CONSISTENCYA_DEPTHWISE_MOD_DWH(inTile, coeffTile, biasArray, outTile, param);
    XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(outScaleArray) >= XI_TILE3D_GET_DIM1(outTile), XI_ERR_DATASIZE, \
      "\nWidth of Output Scale Array = %d, Number of output channels = %d\nWidth of Output Scale Array should be \
      greater than or equal to Number of output channels", XI_ARRAY_GET_WIDTH(outScaleArray), XI_TILE3D_GET_DIM1(outTile));
    XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(outShiftArray) >= XI_TILE3D_GET_DIM1(outTile), XI_ERR_DATASIZE, \
      "\nWidth of Output Shift Array = %d, Number of output channels = %d\nWidth of Output Shift Array should be \
      greater than or equal to Number of output channels", XI_ARRAY_GET_WIDTH(outShiftArray), XI_TILE3D_GET_DIM1(outTile));
    if (XI_CNNA_CONV_GET_FLAG_RELU(param))
    {
      XI_CHECK_ERROR(XI_CNNA_CONV_GET_RELUMIN(param) <= XI_CNNA_CONV_GET_RELUMAX(param), XI_ERR_BADARG, \
        "\nRelu min = %hi, Relu max = %hi\nRelu max should be greater than or equal to Relu min", \
        XI_CNNA_CONV_GET_RELUMIN(param), XI_CNNA_CONV_GET_RELUMAX(param));
      XI_CHECK_ERROR((XI_CNNA_CONV_GET_RELUMIN(param) >= SCHAR_MIN), XI_ERR_BADARG, \
        "\nRelu min = %hi, value should be greater than or equal to -128", XI_CNNA_CONV_GET_RELUMIN(param));
      XI_CHECK_ERROR((XI_CNNA_CONV_GET_RELUMAX(param) <= SCHAR_MAX), XI_ERR_BADARG, \
        "\nRelu max = %hi, value should be less than or equal to 127", XI_CNNA_CONV_GET_RELUMAX(param));
    }
    XI_CHECK_ERROR(((XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param) >= SCHAR_MIN) && \
                    (XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param) <= SCHAR_MAX)), XI_ERR_BADARG,\
      "\nZero point output = %hi, value should be in the range -128 to 127", XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param));
    XI_CHECK_ERROR(((XI_CNNA_CONV_GET_ZEROPT_INPUT(param) >= SCHAR_MIN) && \
                    (XI_CNNA_CONV_GET_ZEROPT_INPUT(param) <= SCHAR_MAX)), XI_ERR_BADARG,\
      "\nZero point input = %hi, value should be in the range -128 to 127", XI_CNNA_CONV_GET_ZEROPT_INPUT(param));
    XI_CHECK_ERROR((XI_CNNA_CONV_GET_ZEROPT_COEFF(param) == 0), XI_ERR_BADARG,\
      "\nZero point coefficient = %hi, value should be equal to zero", XI_CNNA_CONV_GET_ZEROPT_COEFF(param));
  }

  if (XI_CNNA_CONV_GET_STRIDE(param) == 2)
  {
    depthwiseConvolveAVQ2D_S_7x7j2_S8Ca2_MOD_DWH(inTile, coeffTile, biasArray, outScaleArray, outShiftArray, outTile, param);
  }
  else if (XI_CNNA_CONV_GET_STRIDE(param) == 4)
  {
    depthwiseConvolveAVQ2D_S_7x7j4_S8Ca2_MOD_DWH(inTile, coeffTile, biasArray, outScaleArray, outShiftArray, outTile, param);
  }
  else
  {
    depthwiseConvolveAVQ2D_S_7x7_S8Ca2_MOD_DWH(inTile, coeffTile, biasArray, outScaleArray, outShiftArray, outTile, param);
  }

  return(XI_ERROR_STATUS());
}

/**************************************************************************************/
/***************** xiDepthwiseConvolveAVQ2D_S_MxN_S8Ca2_MOD_DWH ***********************/
/**************************************************************************************/
/**************************************************************************************/
/* Description : Optimized implementation of Symmetric MxN MOD_DWH                    */
/*               depthwise convolution for Android NN.                                */
/* Inputs      : Input Data Tile, Coeff Data Tile, Bias Array, Output Scale Array,    */
/*               Output Shift Array, CNNA convolution params structure.               */
/* Outputs     : XI Error Code                                                        */
/* InOuts      : Output Tile                                                          */
/* Assumptions : InData, CoeffData are S8                                             */
/*               biasArray is signed 32b, OutData is S8                               */
/*               Kernel Size is MxN.                                                  */
/*               Input and Output are in DWH format                                   */
/*               Coeff is in DWH format                                               */
/*               CoeffDim1Pitch is aligned to 2N (Ca2)                                */
/**************************************************************************************/
XI_ERR_TYPE xiDepthwiseConvolveAVQ2D_S_MxN_S8Ca2_MOD_DWH(const xi_pTile3D inTile,
                                                         const xi_pTile3D coeffTile,
                                                         const xi_pArray biasArray,
                                                         const xi_pArray outScaleArray,
                                                         const xi_pArray outShiftArray,
                                                         xi_pTile3D outTile,
                                                         const xi_cnna_conv_params *param)
{
  /* Error Checks */
  XI_ERROR_CHECKS()
  {
    XI_CHECK_TILE3D_S8(inTile);
    XI_CHECK_TILE3D_S8(outTile);
    XI_CHECK_TILE3D_S8(coeffTile);
    XI_CHECK_ARRAY_S32(biasArray);
    XI_CHECK_ARRAY_S32(outScaleArray);
    XI_CHECK_ARRAY_S8(outShiftArray);
    XI_CHECK_POINTER(param);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_TILE4D_IN_DRAM_BOUNDARY(coeffTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(inTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(coeffTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(biasArray, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(outScaleArray, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(outShiftArray, outTile);
    XI_CHECK_TILE3D_DATA_ORDER(inTile, XI_DWH);
    XI_CHECK_TILE3D_DATA_ORDER(outTile, XI_DWH);
    XI_CHECK_TILE3D_DATA_ORDER(coeffTile, XI_DWH);
    XI_CHECK_ERROR(((XI_TILE3D_GET_DIM2(coeffTile) <= 16) && (XI_TILE3D_GET_DIM3(coeffTile) <= 16)), XI_ERR_KSIZE, \
                   "\nKernel width = %d, height = %d\nKernel width and height should be less than equal to 16", \
                   XI_TILE3D_GET_DIM2(coeffTile), XI_TILE3D_GET_DIM3(coeffTile));
    XI_CHECK_EDGESA_DEPTHWISE_MOD_DWH(inTile, coeffTile, param);
    XI_CHECK_ERROR(((XI_CNNA_CONV_GET_STRIDEX(param) == 1) || (XI_CNNA_CONV_GET_STRIDEX(param) == 2) || (XI_CNNA_CONV_GET_STRIDEX(param) == 4)) && \
                   ((XI_CNNA_CONV_GET_STRIDEY(param) == 1) || (XI_CNNA_CONV_GET_STRIDEY(param) == 2) || (XI_CNNA_CONV_GET_STRIDEY(param) == 4)), \
                   XI_ERR_BADARG, "\nStrideX = %hhu, StrideY = %hhu\nStride value should be equal to 1, 2 or 4", \
                   XI_CNNA_CONV_GET_STRIDEX(param), XI_CNNA_CONV_GET_STRIDEY(param));
    XI_CHECK_ERROR(XI_CNNA_CONV_GET_STRIDEX(param) == XI_CNNA_CONV_GET_STRIDEY(param), XI_ERR_BADARG, \
                   "\nStrideX = %hhu, StrideY = %hhu\nStrideX and StrideY must be same", \
                   XI_CNNA_CONV_GET_STRIDEX(param), XI_CNNA_CONV_GET_STRIDEY(param));
    XI_CHECK_TILE3D_IALIGNMENT_2NX8(coeffTile);
    XI_CHECK_CONSISTENCYA_DEPTHWISE_MOD_DWH(inTile, coeffTile, biasArray, outTile, param);
    XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(outScaleArray) >= XI_TILE3D_GET_DIM1(outTile), XI_ERR_DATASIZE, \
      "\nWidth of Output Scale Array = %d, Number of output channels = %d\nWidth of Output Scale Array should be \
      greater than or equal to Number of output channels", XI_ARRAY_GET_WIDTH(outScaleArray), XI_TILE3D_GET_DIM1(outTile));
    XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(outShiftArray) >= XI_TILE3D_GET_DIM1(outTile), XI_ERR_DATASIZE, \
      "\nWidth of Output Shift Array = %d, Number of output channels = %d\nWidth of Output Shift Array should be \
      greater than or equal to Number of output channels", XI_ARRAY_GET_WIDTH(outShiftArray), XI_TILE3D_GET_DIM1(outTile));
    if (XI_CNNA_CONV_GET_FLAG_RELU(param))
    {
      XI_CHECK_ERROR(XI_CNNA_CONV_GET_RELUMIN(param) <= XI_CNNA_CONV_GET_RELUMAX(param), XI_ERR_BADARG, \
        "\nRelu min = %hi, Relu max = %hi\nRelu max should be greater than or equal to Relu min", \
        XI_CNNA_CONV_GET_RELUMIN(param), XI_CNNA_CONV_GET_RELUMAX(param));
      XI_CHECK_ERROR((XI_CNNA_CONV_GET_RELUMIN(param) >= SCHAR_MIN), XI_ERR_BADARG, \
        "\nRelu min = %hi, value should be greater than or equal to -128", XI_CNNA_CONV_GET_RELUMIN(param));
      XI_CHECK_ERROR((XI_CNNA_CONV_GET_RELUMAX(param) <= SCHAR_MAX), XI_ERR_BADARG, \
        "\nRelu max = %hi, value should be less than or equal to 127", XI_CNNA_CONV_GET_RELUMAX(param));
    }
    XI_CHECK_ERROR(((XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param) >= SCHAR_MIN) && \
                    (XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param) <= SCHAR_MAX)), XI_ERR_BADARG,\
      "\nZero point output = %hi, value should be in the range -128 to 127", XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param));
    XI_CHECK_ERROR(((XI_CNNA_CONV_GET_ZEROPT_INPUT(param) >= SCHAR_MIN) && \
                    (XI_CNNA_CONV_GET_ZEROPT_INPUT(param) <= SCHAR_MAX)), XI_ERR_BADARG,\
      "\nZero point input = %hi, value should be in the range -128 to 127", XI_CNNA_CONV_GET_ZEROPT_INPUT(param));
    XI_CHECK_ERROR((XI_CNNA_CONV_GET_ZEROPT_COEFF(param) == 0), XI_ERR_BADARG,\
      "\nZero point coefficient = %hi, value should be equal to zero", XI_CNNA_CONV_GET_ZEROPT_COEFF(param));
  }

  /* Getting parameters from the tile structures */
  const int32_t numCh         = XI_TILE3D_GET_DIM1(inTile);
  const int32_t outW          = XI_TILE3D_GET_DIM2(outTile);
  const int32_t outH          = XI_TILE3D_GET_DIM3(outTile);
  const int32_t inDataPitch1  = XI_TILE3D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2  = XI_TILE3D_GET_DIM2_PITCH(inTile);
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
  const int32_t coeffPitch1   = XI_TILE3D_GET_DIM1_PITCH(coeffTile);
  const int32_t coeffPitch2   = XI_TILE3D_GET_DIM2_PITCH(coeffTile);
  /* Convolution params */
  const uint8_t enableReLu    = XI_CNNA_CONV_GET_FLAG_RELU(param);
  const uint8_t stride        = XI_CNNA_CONV_GET_STRIDE(param);
  const int16_t zeroPtOut     = XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param);
  const int16_t zeroPtIn      = XI_CNNA_CONV_GET_ZEROPT_INPUT(param);
  const uint8_t leftEdgeFlag  = XI_CNNA_CONV_GET_FLAG_LEFTEDGE(param);
  const uint8_t topEdgeFlag   = XI_CNNA_CONV_GET_FLAG_TOPEDGE(param);
  const int16_t reluMin       = XI_CNNA_CONV_GET_RELUMIN(param);
  const int16_t reluMax       = XI_CNNA_CONV_GET_RELUMAX(param);

  /* Setting the limits for output data according to ReLu is enabled or not*/
  const int16_t minLim = enableReLu ? reluMin : SCHAR_MIN;
  const int16_t maxLim = enableReLu ? reluMax : SCHAR_MAX;
  /* Kernel Size (DWH) */
  const int32_t kWidthU  = XI_TILE3D_GET_DIM2(coeffTile);
  const int32_t kHeightU = XI_TILE3D_GET_DIM3(coeffTile);

  /* Data Pointers of input, output, coefficient and bias data */
  int8_t *pInData    = (int8_t *) XI_TILE3D_GET_DATA_PTR(inTile);
  int8_t *pOutData   = (int8_t *) XI_TILE3D_GET_DATA_PTR(outTile);
  int8_t *pCoeffData = (int8_t *) XI_TILE3D_GET_DATA_PTR(coeffTile);
  int32_t *pBiasData = (int32_t *)XI_ARRAY_GET_DATA_PTR(biasArray);
  int32_t *pOutScale = (int32_t *)XI_ARRAY_GET_DATA_PTR(outScaleArray);
  int8_t *pOutShift  = (int8_t *)XI_ARRAY_GET_DATA_PTR(outShiftArray);

  int32_t leftEdge, topEdge;
  if ((kWidthU % 2) != 0)
  {
    leftEdge = kWidthU / 2;
  }
  else
  {
    leftEdge = leftEdgeFlag ? (kWidthU / 2) : ((kWidthU / 2) - 1);
  }

  if ((kHeightU % 2) != 0)
  {
    topEdge = kHeightU / 2;
  }
  else
  {
    topEdge = topEdgeFlag ? (kHeightU / 2) : ((kHeightU / 2) - 1);
  }

  /* Move pointer to the start of the data (including edge) */
  pInData = &pInData[-((leftEdge) * inDataPitch1 + (topEdge) * inDataPitch2)];

  /* Variable Declarations */
  int32_t ch, x, y, k;
  int32_t vectorizationWidth = 2 * XCHAL_IVPN_SIMD_WIDTH;
  valign vaOutData = IVP_ZALIGN();
  /* Input and Output data Pointers */
  xb_vecN_2x32v* restrict phvecBias1;
  xb_vecN_2x32v* restrict phvecBias2;
  xb_vecN_2x32v* restrict phvecOutScale1;
  xb_vecN_2x32v* restrict phvecOutScale2;
  xb_vec2Nx8* restrict pdvecOutShift;
  xb_vec2Nx8* restrict pdvecCoeff;
  xb_vec2Nx8* restrict pdvecData1;
  xb_vec2Nx8* restrict pdvecData2;
  xb_vec2Nx8* restrict pdvecData3;
  xb_vec2Nx8* restrict pdvecData4;
  xb_vec2Nx8* restrict pdvecOut;
  /* Input and Output data Pointers */
  xb_vec2Nx8 dvecData11, dvecData12;
  xb_vec2Nx8 dvecData21, dvecData22;
  xb_vecN_2x32v hvecScale1, hvecScale2, hvecScale3, hvecScale4;
  xb_vec2Nx24 daccSum1, daccSum2, daccSum3, daccSum4;

  /* Vectorization is along channels. Output width and output height are unrolled    */
  /* by 2. Kernel height and width and height loops are combined together            */
  /* In this case the address offsets for input  need to be derived from             */
  /* vector registers. These vector registers are initialized as follows             */
  xb_vecN_2x32Uv hvecInAddrOffInit = inDataPitch1;
  int32_t * pVecOffset = (int32_t *)&hvecInAddrOffInit;
  /*Creating offset pattern with inP1, inP1...inP2*/
  /*inputPitch2 will be inserted for kWidthU-th position*/
  pVecOffset[kWidthU - 1] = (inDataPitch2 - (kWidthU - 1) * inDataPitch1);

  for (ch = 0; ch < numCh; ch += vectorizationWidth) /* Along Output Channel */
  {
    /* To handle corner case when number of output channels
     * is not a multiple of  2 * XCHAL_IVPN_SIMD_WIDTH*/
    int32_t remainingOutCh = XT_MIN(vectorizationWidth, numCh - ch);
    int32_t remBiasLoad = (remainingOutCh > XCHAL_IVPN_SIMD_WIDTH) ? 1 : 0;

    int32_t remCh1 = XT_MIN(((numCh - ch) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh2 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH / 2) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh3 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
    int32_t remCh4 = XT_MIN((((numCh - ch) - XCHAL_IVPN_SIMD_WIDTH * 3 / 2) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);

    /* Load OutScale values */
    phvecOutScale1 = (xb_vecN_2x32v*)(pOutScale + ch);
    phvecOutScale2 = (xb_vecN_2x32v*)(pOutScale + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
    valign vaOutScale = IVP_LAN_2X32_PP(phvecOutScale1);
    IVP_LAVN_2X32_XP(hvecScale1, vaOutScale, phvecOutScale1, remCh1);
    IVP_LAVN_2X32_XP(hvecScale2, vaOutScale, phvecOutScale1, remCh2);
    vaOutScale = IVP_LAN_2X32_PP(phvecOutScale2);
    IVP_LAVN_2X32_XP(hvecScale3, vaOutScale, phvecOutScale2, remCh3);
    IVP_LAVN_2X32_XP(hvecScale4, vaOutScale, phvecOutScale2, remCh4);

    /* Load OutShift values */
    xb_vec2Nx8 dvecShift;
    pdvecOutShift = (xb_vec2Nx8*)(pOutShift + ch);
    valign vaOutShift = IVP_LA2NX8_PP(pdvecOutShift);
    IVP_LAV2NX8_XP(dvecShift, vaOutShift, pdvecOutShift, remainingOutCh);

    /* Calculate left shift and right shift values */
    vbool2N vb2N = IVP_LT2NX8(dvecShift, 0);
    xb_vec2Nx8 dvecRightShift = IVP_MOV2NX8T(0, dvecShift, vb2N);
    xb_vec2Nx8 dvecLeftShift = 0;
    IVP_SUB2NX8T(dvecLeftShift, 0, dvecShift, vb2N);

    xb_vec2Nx8 dvecRightShiftL, dvecRightShiftH;
    IVP_DSEL2NX8I(dvecRightShiftH, dvecRightShiftL, 0, dvecRightShift, IVP_DSELI_8B_INTERLEAVE_1);

    xb_vecNx16 vecRightShift1 = IVP_MOVNX16_FROM2NX8(dvecRightShiftL);
    xb_vecNx16 vecRightShift2 = IVP_MOVNX16_FROM2NX8(dvecRightShiftH);

    xb_vec2Nx8 dvecLeftShiftL, dvecLeftShiftH;
    IVP_DSEL2NX8I(dvecLeftShiftH, dvecLeftShiftL, 0, dvecLeftShift, IVP_DSELI_8B_INTERLEAVE_1);

    xb_vecNx16 vecLeftShiftLL, vecLeftShiftLH, vecLeftShiftHL, vecLeftShiftHH;
    IVP_DSELNX16I(vecLeftShiftLH, vecLeftShiftLL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftL), IVP_DSELI_INTERLEAVE_1);
    IVP_DSELNX16I(vecLeftShiftHH, vecLeftShiftHL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftH), IVP_DSELI_INTERLEAVE_1);

    xb_vecN_2x32v hvecLeftShift1 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLL);
    xb_vecN_2x32v hvecLeftShift2 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLH);
    xb_vecN_2x32v hvecLeftShift3 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHL);
    xb_vecN_2x32v hvecLeftShift4 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHH);
    /* Fixup calculation */
    daccSum1 = 0;
    int8_t *pCoeff = (pCoeffData + ch);
    CALCULATE_FIXUP(daccSum1, kHeightU, kWidthU, pCoeff);
    /* Fixup term */
    xb_vecNx16 vecSumL = IVP_CVT16S2NX24L(daccSum1);
    xb_vecNx16 vecSumH = IVP_CVT16S2NX24H(daccSum1);
    xb_vecNx16 vecSumOdd, vecSumEven;
    IVP_DSELNX16I(vecSumOdd, vecSumEven, vecSumH, vecSumL, IVP_DSELI_DEINTERLEAVE_1);
    daccSum1 = IVP_MULI2NX8X16((xb_vec2Nx8)zeroPtIn, vecSumOdd, vecSumEven);
    xb_vecN_2x32v hvecFixLL = IVP_CVT32S2NX24LL(daccSum1);
    xb_vecN_2x32v hvecFixLH = IVP_CVT32S2NX24LH(daccSum1);
    xb_vecN_2x32v hvecFixHL = IVP_CVT32S2NX24HL(daccSum1);
    xb_vecN_2x32v hvecFixHH = IVP_CVT32S2NX24HH(daccSum1);
    for (y = 0; y < outH; y+= 2)  /* Along Output Height */
    {
      /* Variable to handle corner case when height is odd */
      int32_t numY = XT_MIN(1, outH - y - 1);
      for (x = 0; x < outW; x += 2)  /* Along Output Width */
      {
        /* Variable to handle corner case when width is odd */
        int32_t numX = XT_MIN(1, outW - x - 1);
        /* Output Data pointer */
        int8_t *pOut = pOutData + ch + (x * outDataPitch1 + y * outDataPitch2) ;
        /* Initialize accumulators with bias values */
        phvecBias1 = (xb_vecN_2x32v*)(pBiasData + ch);
        phvecBias2 = (xb_vecN_2x32v*)(pBiasData + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
        xb_vecN_2x32v hvecBias1, hvecBias2, hvecBias3, hvecBias4;
        BIAS_LOAD_FIXUP_SUBTRACT(phvecBias1, phvecBias2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, remCh1, remCh2, remCh3, remCh4);
        daccSum1 = 0;
        daccSum2 = daccSum1;
        daccSum3 = daccSum1;
        daccSum4 = daccSum1;

        /* Pointer for Coefficient Load */
        pdvecCoeff = (xb_vec2Nx8 *)(pCoeffData + ch);

        /* Input Data Pointers */
        int8_t *pData = pInData + ch + x * stride * inDataPitch1 + y * stride * inDataPitch2;
        /* Pointers for Input Data Loads */
        pdvecData1 = (xb_vec2Nx8 *)(pData);
        pdvecData2 = (xb_vec2Nx8 *)(pData + stride * inDataPitch1 * numX);
        pdvecData3 = (xb_vec2Nx8 *)(pData + stride * inDataPitch2 * numY);
        pdvecData4 = (xb_vec2Nx8 *)(pData + (stride * inDataPitch1 + stride * inDataPitch2) * numX * numY);

        int32_t i = 0;
        for (k = 0; k < kHeightU * kWidthU; k++, i++) /* Kernel Height */
        {
          int32_t addrOffset = IVP_EXTRVRN_2X32(hvecInAddrOffInit, 4 * i);
          /* After every kWidthU no: of iterations, the offset, should start from
          0th position in the offset vector*/
          /*Making i = -1 when k = multiple of kWidthU, so that i++ will give i = 0
          for next iteration */
          i |= ((kWidthU - 2) - i) >> 31;
          /*Input loads corresponding to 4 output rows */
          /* H1W1 */
          valign vaData1 = IVP_LA2NX8_PP(pdvecData1);
          IVP_LA2NX8_XP(dvecData11, vaData1, pdvecData1, addrOffset);
          /* H1W2 */
          valign vaData2 = IVP_LA2NX8_PP(pdvecData2);
          IVP_LA2NX8_XP(dvecData12, vaData2, pdvecData2, addrOffset);
          /* H2W1 */
          valign vaData3 = IVP_LA2NX8_PP(pdvecData3);
          IVP_LA2NX8_XP(dvecData21, vaData3, pdvecData3, addrOffset);
          /* H2W1 */
          valign vaData4 = IVP_LA2NX8_PP(pdvecData4);
          IVP_LA2NX8_XP(dvecData22, vaData4, pdvecData4, addrOffset);
          /* Coefficient Load */
          xb_vec2Nx8 dvecCoeff; IVP_LV2NX8_XP(dvecCoeff, pdvecCoeff, coeffPitch1);
          /*Multiply and accumulate for 4 output rows*/
          IVP_MULA2NX8(daccSum1, dvecCoeff, dvecData11);
          IVP_MULA2NX8(daccSum2, dvecCoeff, dvecData12);
          IVP_MULA2NX8(daccSum3, dvecCoeff, dvecData21);
          IVP_MULA2NX8(daccSum4, dvecCoeff, dvecData22);
        } /* for (k = 0; k < kHeightU * kWidthU; k++) */
        xb_vec2Nx8 dvecOut1, dvecOut2, dvecOut3, dvecOut4;
        /* Store the result for output depth for H1-W1*/
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum1, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut1);
        /* Store the output vecOut1 along the output depth */
        pdvecOut = (xb_vec2Nx8 *)(pOut);
        IVP_SAV2NX8_XP(dvecOut1, vaOutData, pdvecOut, remainingOutCh);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for output depth for H1-W2 */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut2);
        pdvecOut = (xb_vec2Nx8 *)(pOut + outDataPitch1 * numX);
        IVP_SAV2NX8_XP(dvecOut2, vaOutData, pdvecOut, remainingOutCh * numX);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for output depth for H2-W1  */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum3, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut3);
        pdvecOut = (xb_vec2Nx8 *)(pOut + outDataPitch2 * numY);
        IVP_SAV2NX8_XP(dvecOut3, vaOutData, pdvecOut, remainingOutCh * numY);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        /* Store the result for output depth for H2-W2  */
        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum4, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecScale1, hvecScale2, \
          hvecScale3, hvecScale4, hvecLeftShift1, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, maxLim, minLim, dvecOut4);
        pdvecOut = (xb_vec2Nx8 *)(pOut + (outDataPitch1 * numX + outDataPitch2 * numY));
        IVP_SAV2NX8_XP(dvecOut4, vaOutData, pdvecOut, remainingOutCh* numX * numY);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
      }  /* for (x = 0; x < outW; x += 2) */
    }    /* for (y = 0; y < outH; y += 2) */
  }      /* for (ch = 0; ch < numCh; ch += vectorizationWidth) */
  return(XI_ERROR_STATUS());
}

#define DEPTHWISE_DILATED_BIAS_ADD_PACK_SCALE_AND_ROUNDING_VQ(daccSum,hvecBias1,hvecBias2,hvecBias3,hvecBias4, hvecLeftShift1 \
   ,hvecScale1,hvecScale2,hvecScale3,hvecScale4, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4,vecRightShift1,vecRightShift2, \
    minLim, maxLim, vecOutL, vecOutH)                                                     \
{                                                                                                              \
  /* Move the 24 bit accumulated sum to 32 bit vec registers. */                                               \
  xb_vecN_2x32v hvecIn1 = IVP_CVT32S2NX24LL(daccSum);                                                          \
  xb_vecN_2x32v hvecIn2 = IVP_CVT32S2NX24LH(daccSum);                                                          \
  xb_vecN_2x32v hvecIn3 = IVP_CVT32S2NX24HL(daccSum);                                                          \
  xb_vecN_2x32v hvecIn4 = IVP_CVT32S2NX24HH(daccSum);                                                          \
  /* Adding bias values */                                                                                     \
  hvecIn1 = IVP_ADDN_2X32(hvecIn1,hvecBias1);                                                                  \
  hvecIn2 = IVP_ADDN_2X32(hvecIn2,hvecBias2);                                                                  \
  hvecIn3 = IVP_ADDN_2X32(hvecIn3,hvecBias3);                                                                  \
  hvecIn4 = IVP_ADDN_2X32(hvecIn4,hvecBias4);                                                                  \
  /* accumulated sum is multiplied with output scale value and Packed by 31 - Left shift */                    \
  hvecIn1 = IVP_SLSN_2X32(hvecIn1, hvecLeftShift1);                                                            \
  hvecIn2 = IVP_SLSN_2X32(hvecIn2, hvecLeftShift2);                                                            \
  hvecIn3 = IVP_SLSN_2X32(hvecIn3, hvecLeftShift3);                                                            \
  hvecIn4 = IVP_SLSN_2X32(hvecIn4, hvecLeftShift4);                                                            \
  xb_vecNx16U vecIn1 = IVP_MOVNX16_FROMN_2X32(hvecIn1);                                                        \
  xb_vecNx16U vecIn2 = IVP_MOVNX16_FROMN_2X32(hvecIn2);                                                        \
  xb_vecNx16U vecIn3 = IVP_MOVNX16_FROMN_2X32(hvecIn3);                                                        \
  xb_vecNx16U vecIn4 = IVP_MOVNX16_FROMN_2X32(hvecIn4);                                                        \
  xb_vecN_2x64w haccScale1 = IVP_MULUSN_2X16X32_0(vecIn1, hvecScale1);                                         \
  xb_vecN_2x64w haccScale2 = IVP_MULUSN_2X16X32_0(vecIn2, hvecScale2);                                         \
  xb_vecN_2x64w haccScale3 = IVP_MULUSN_2X16X32_0(vecIn3, hvecScale3);                                         \
  xb_vecN_2x64w haccScale4 = IVP_MULUSN_2X16X32_0(vecIn4, hvecScale4);                                         \
  IVP_MULAHN_2X16X32_1(haccScale1, vecIn1, hvecScale1);                                                        \
  IVP_MULAHN_2X16X32_1(haccScale2, vecIn2, hvecScale2);                                                        \
  IVP_MULAHN_2X16X32_1(haccScale3, vecIn3, hvecScale3);                                                        \
  IVP_MULAHN_2X16X32_1(haccScale4, vecIn4, hvecScale4);                                                        \
  hvecIn1 = IVP_PACKVRN_2X64W(haccScale1, 31);                                                                 \
  hvecIn2 = IVP_PACKVRN_2X64W(haccScale2, 31);                                                                 \
  hvecIn3 = IVP_PACKVRN_2X64W(haccScale3, 31);                                                                 \
  hvecIn4 = IVP_PACKVRN_2X64W(haccScale4, 31);                                                                 \
  /* Perform Pack and Rounding */                                                                              \
  CONV_PACK_AND_ROUNDING(hvecIn1, hvecIn2, vecRightShift1, vecOutL);                                           \
  CONV_PACK_AND_ROUNDING(hvecIn3, hvecIn4, vecRightShift2, vecOutH);                                           \
  /* Add zeroPtOut */                                                                                          \
  xb_vecNx16 vecOutAddZP_L = IVP_ADDSNX16(vecOutL, (xb_vecNx16) outputOffset);                                    \
  xb_vecNx16 vecOutAddZP_H = IVP_ADDSNX16(vecOutH, (xb_vecNx16) outputOffset);                                    \
  /* Result is saturate to minLim and maxLim */                                                                \
  vecOutL = (IVP_MAXNX16(IVP_MINNX16(vecOutAddZP_L, (xb_vecNx16) maxLim), (xb_vecNx16) minLim));               \
  vecOutH = (IVP_MAXNX16(IVP_MINNX16(vecOutAddZP_H, (xb_vecNx16) maxLim), (xb_vecNx16) minLim));               \
}


/****************************  xiDepthwiseMultiplierConvolvedAVQ3D_S8_DWH *****************************/
/* Description : Optimized implementation of 3D depthwise multiplier dilated                          */
/*               convolution with symmetric quantization support.                                     */
/* Inputs      : Input Data Tile, Coeff Data Tile, Bias Array, ScaleArray                             */
/*               Shift Array,CNNA depthwise dilated convolution params structure.                     */
/* Outputs     : XI Error Code                                                                        */
/* InOuts      : Output Tile                                                                          */
/* Assumptions : InData supported is S8                                                               */
/*               CoeffData supported is S8                                                            */
/*               OutData supported is S8                                                              */
/*               Input and Output are in DWH format.                                                  */
/*               Coeff is in DWH  format.                                                             */
/******************************************************************************************************/
XI_ERR_TYPE xiDepthwiseMultiplierConvolvedAVQ3D_S8_DWH(const xi_pTile3D inTile,
  const xi_pTile3D coeffTile,
  const xi_pArray biasArray,
  const xi_pArray scaleArray,
  const xi_pArray shiftArray,
  xi_pTile3D outTile,
  const xi_cnna_depthwiseDilatedConv_params* param)
{
  XI_ERROR_CHECKS()
  {
    XI_CHECK_TILE3D_S8(inTile);
    XI_CHECK_TILE3D_S8(coeffTile);
    XI_CHECK_TILE3D_S8(outTile);
    XI_CHECK_ARRAY_S32(biasArray);
    XI_CHECK_POINTER(param);
    XI_CHECK_ARRAY_S8(shiftArray);
    XI_CHECK_ARRAY_S32(scaleArray);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(coeffTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(inTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(coeffTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(biasArray, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(scaleArray, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(shiftArray, outTile);
    XI_CHECK_TILE3D_DATA_ORDER(inTile, XI_DWH);
    XI_CHECK_TILE3D_DATA_ORDER(coeffTile, XI_DWH);
    XI_CHECK_TILE3D_DATA_ORDER(outTile, XI_DWH);
    XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(scaleArray) >= XI_TILE3D_GET_DIM1(outTile), XI_ERR_DATASIZE, \
      "\nscaleArray width = %d, Output depth = %d\nscaleArray width should be >= output depth", \
      XI_ARRAY_GET_WIDTH(scaleArray), XI_TILE3D_GET_DIM1(outTile));
    XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(shiftArray) >= XI_TILE3D_GET_DIM1(outTile), XI_ERR_DATASIZE, \
      "\nshiftArray width = %d, Output depth = %d\nshiftArray width should be >= output depth", \
      XI_ARRAY_GET_WIDTH(shiftArray), XI_TILE3D_GET_DIM1(outTile));
    XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(biasArray) == XI_TILE3D_GET_DIM1(outTile), XI_ERR_DATASIZE, \
      "\nbiasArray width = %d, Output depth = %d\nbiasArray width should be >= output depth", \
      XI_ARRAY_GET_WIDTH(biasArray), XI_TILE3D_GET_DIM1(outTile));
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(coeffTile) <= 16) && (XI_TILE3D_GET_DIM3(coeffTile) <= 16), XI_ERR_KSIZE, \
      "\nKernel width = %d, Kernel height = %d\nKernel width and height should be less than or equal to 8", \
      XI_TILE3D_GET_DIM2(coeffTile), XI_TILE3D_GET_DIM3(coeffTile));
    XI_CHECK_ERROR((XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) <= 36) && \
      (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) <= 36), XI_ERR_BADARG, \
      "\nDilationX = %hhu, DilationY = %hhu\nDilationX and DilationY should be less than or equal to 36", \
      XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param), XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param));
    XI_CHECK_ERROR((XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEX(param) <= 4) && \
      (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEY(param) <= 4), XI_ERR_BADARG, \
      "\nStrideX = %hhu, StrideY = %hhu\nStrideX and StrideY should be less than or equal to 4", \
      XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEX(param), XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEY(param));
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(outTile) == XI_TILE3D_GET_DIM1(coeffTile), XI_ERR_DATASIZE, \
      "\nOutput depth = %d, Coefficient tile dim1 = %d\nOutput depth should be same as coefficient tile dim1", \
      XI_TILE3D_GET_DIM1(outTile), XI_TILE3D_GET_DIM1(coeffTile));
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM2(inTile) >= (XI_TILE3D_GET_DIM2(outTile) - 1) * \
      XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEX(param) + 1, XI_ERR_DATASIZE, "\ninWidth = %d, outWidth = %d,\
                    strideWidth = %hhu\ninWidth should be >= (outWidth - 1) * strideWidth + 1", XI_TILE3D_GET_DIM2(inTile), \
      XI_TILE3D_GET_DIM2(outTile), XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEX(param));
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM3(inTile) >= (XI_TILE3D_GET_DIM3(outTile) - 1) * \
      XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEY(param) + 1, XI_ERR_DATASIZE, "\ninHeight = %d, outHeight = %d,\
                    strideHeight = %hhu\ninHeight should be >= (outHeight - 1) * strideHeight + 1", \
      XI_TILE3D_GET_DIM3(inTile), XI_TILE3D_GET_DIM3(outTile), XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEY(param));
    if (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_RELU(param))
    {
      XI_CHECK_ERROR(XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(param) <= XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(param), \
        XI_ERR_BADARG, "\nRelu max = %hi, Relu min = %hi\nRelu max should be greater than or equal to Relu min", \
        XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(param), XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(param));
      XI_CHECK_ERROR((XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(param) >= SCHAR_MIN), XI_ERR_BADARG, \
        "\nRelu min = %hi, value should be greater than or equal to -128", \
        XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(param));
      XI_CHECK_ERROR((XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(param) <= SCHAR_MAX), XI_ERR_BADARG, \
        "\nRelu max = %hi, value must be less than or equal to 127", \
        XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(param));
    }
    XI_CHECK_ERROR((XI_CNNA_DEPTHWISE_DILATED_CONV_GET_COEFF_OFFSET(param) == 0), XI_ERR_BADARG, \
      "\nZero Point coefficient = %d, value should be equal to 0", \
      XI_CNNA_DEPTHWISE_DILATED_CONV_GET_COEFF_OFFSET(param));
    //Check for coefficient dimension and input edges.
    if (!(((XI_TILE3D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) % 2 == 0))
    {
      // Odd filter dimensions
      XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (((XI_TILE3D_GET_DIM2(coeffTile) - 1) * \
        XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2), XI_ERR_EDGE, "\nedgeLeft = %hu, \
                     value should be greater than or equal to %d(((XI_TILE3D_GET_DIM2(coeffTile) - 1) * \
                     XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2)", XI_TILE3D_GET_DIM2_EDGE1(inTile), \
        (((XI_TILE3D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2));
      XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (((XI_TILE3D_GET_DIM2(coeffTile) - 1) * \
        XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2), XI_ERR_EDGE, "\nedgeRight = %hu, \
                     value should be greater than or equal to %d(((XI_TILE3D_GET_DIM2(coeffTile) - 1) * \
                     XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2)", XI_TILE3D_GET_DIM2_EDGE2(inTile), \
        (((XI_TILE3D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2));
    }
    else
    {
      // Even filter dimensions
      if (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_LEFTEDGE(param))
      {
        XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (((XI_TILE3D_GET_DIM2(coeffTile) - 1) * \
          XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2), XI_ERR_EDGE, "\nedgeLeft = %hu, \
                       value should be greater than or equal to %d(((XI_TILE3D_GET_DIM2(coeffTile) - 1) * \
                       XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2)", XI_TILE3D_GET_DIM2_EDGE1(inTile), \
          (((XI_TILE3D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2));
        XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((((XI_TILE3D_GET_DIM2(coeffTile) - 1) * \
          XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2) - 1), XI_ERR_EDGE, "\nedgeRight = %hu, \
                       value should be greater than or equal to %d((((XI_TILE3D_GET_DIM2(coeffTile) - 1) * \
                       XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2) - 1)", XI_TILE3D_GET_DIM2_EDGE2(inTile), \
          ((((XI_TILE3D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2) - 1));
      }
      else
      {
        // edgeLeft >= dilatedKernelWidth / 2 - 1
        XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((((XI_TILE3D_GET_DIM2(coeffTile) - 1) * \
          XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2) - 1), XI_ERR_EDGE, "\nedgeLeft = %hu, \
                       value should be greater than or equal to %d((((XI_TILE3D_GET_DIM2(coeffTile) - 1) * \
                       XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2) - 1)", XI_TILE3D_GET_DIM2_EDGE1(inTile), \
          ((((XI_TILE3D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2) - 1));
        XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((((XI_TILE3D_GET_DIM2(coeffTile) - 1) * \
          XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2)), XI_ERR_EDGE, "\nedgeRight = %hu, \
                       value should be greater than or equal to %d((((XI_TILE3D_GET_DIM2(coeffTile) - 1) * \
                       XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2))", XI_TILE3D_GET_DIM2_EDGE2(inTile), \
          ((((XI_TILE3D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2)));
      }
    }
    if (!(((XI_TILE3D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) % 2 == 0))
    {
      // Odd filter dimensions
      XI_CHECK_ERROR(XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (((XI_TILE3D_GET_DIM3(coeffTile) - 1) * \
        XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2), XI_ERR_EDGE, "\nedgeTop = %hu, \
                     value should be greater than or equal to %d(((XI_TILE3D_GET_DIM3(coeffTile) - 1) * \
                     XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2)", XI_TILE3D_GET_DIM3_EDGE1(inTile), \
        (((XI_TILE3D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2));
      XI_CHECK_ERROR(XI_TILE3D_GET_DIM3_EDGE2(inTile) >= (((XI_TILE3D_GET_DIM3(coeffTile) - 1) * \
        XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2), XI_ERR_EDGE, "\nedgeBottom = %hu, \
                     value should be greater than or equal to %d(((XI_TILE3D_GET_DIM3(coeffTile) - 1) * \
                     XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2)", XI_TILE3D_GET_DIM3_EDGE2(inTile), \
        (((XI_TILE3D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2));
    }
    else
    {
      // Even filter dimensions
      if (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_TOPEDGE(param))
      {
        // edgeTop >= dilatedKernelHeight / 2
        XI_CHECK_ERROR(XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (((XI_TILE3D_GET_DIM3(coeffTile) - 1) * \
          XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2), XI_ERR_EDGE, "\nedgeTop = %hu, \
                       value should be greater than or equal to %d(((XI_TILE3D_GET_DIM3(coeffTile) - 1) * \
                       XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2)", XI_TILE3D_GET_DIM3_EDGE1(inTile), \
          (((XI_TILE3D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2));
        XI_CHECK_ERROR(XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((((XI_TILE3D_GET_DIM3(coeffTile) - 1) * \
          XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2) - 1), XI_ERR_EDGE, "\nedgeBottom = %hu, \
                       value should be greater than or equal to %d(((XI_TILE3D_GET_DIM3(coeffTile) - 1) * \
                       XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2)", XI_TILE3D_GET_DIM3_EDGE2(inTile), \
          (((XI_TILE3D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2));
      }
      else
      {
        // edgeTop >= dilatedKernelHeight / 2 - 1
        XI_CHECK_ERROR(XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((((XI_TILE3D_GET_DIM3(coeffTile) - 1) * \
          XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2) - 1), XI_ERR_EDGE, "\nedgeTop = %hu, \
                       value should be greater than or equal to %d((((XI_TILE3D_GET_DIM3(coeffTile) - 1) * \
                       XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2) - 1)", XI_TILE3D_GET_DIM3_EDGE1(inTile), \
          ((((XI_TILE3D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2) - 1));
        XI_CHECK_ERROR(XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((((XI_TILE3D_GET_DIM3(coeffTile) - 1) * \
          XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2)), XI_ERR_EDGE, "\nedgeBottom = %hu, \
                       value should be greater than or equal to %d((((XI_TILE3D_GET_DIM3(coeffTile) - 1) * \
                       XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2))", XI_TILE3D_GET_DIM3_EDGE2(inTile), \
          ((((XI_TILE3D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2)));
      }
    }
  }

  /* Get Tile Parameters */
  int32_t inDepth = XI_TILE3D_GET_DIM1(inTile);
  int32_t inPitch1 = XI_TILE3D_GET_DIM1_PITCH(inTile);
  int32_t inPitch2 = XI_TILE3D_GET_DIM2_PITCH(inTile);
  int32_t outWidth = XI_TILE3D_GET_DIM2(outTile);
  int32_t outHeight = XI_TILE3D_GET_DIM3(outTile);
  int32_t outPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  int32_t outPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
  int32_t filterWidth = XI_TILE3D_GET_DIM2(coeffTile);
  int32_t filterHeight = XI_TILE3D_GET_DIM3(coeffTile);
  int32_t filterPitch1 = XI_TILE3D_GET_DIM1_PITCH(coeffTile);
  /* Input and Output data pointers */
  int8_t* pInput = (int8_t*)XI_TILE3D_GET_DATA_PTR(inTile);
  int8_t* pOutData = (int8_t*)XI_TILE3D_GET_DATA_PTR(outTile);
  int8_t* pFilter = (int8_t*)XI_TILE3D_GET_DATA_PTR(coeffTile);
  int32_t* pBias = (int32_t*)XI_ARRAY_GET_DATA_PTR(biasArray);
  int32_t* pOutScale = (int32_t*)XI_ARRAY_GET_DATA_PTR(scaleArray);
  int8_t* pOutShift = (int8_t*)XI_ARRAY_GET_DATA_PTR(shiftArray);
  /* Read Depthwise dilated conv parameters */
  int32_t strideWidth = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEX(param);
  int32_t strideHeight = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEY(param);
  int32_t dilationX = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param);
  int32_t dilationY = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param);
  int32_t depthMultiplier = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DEPTH_MULTIPLIER(param);
  int32_t inputOffset = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_INPUT_OFFSET(param);
  int32_t outputOffset = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_OUTPUT_OFFSET(param);
  int32_t reluMin = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(param);
  int32_t reluMax = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(param);
  uint8_t leftEdgeFlag = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_LEFTEDGE(param);
  uint8_t topEdgeFlag = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_TOPEDGE(param);

  /* Setting the limits for output data according to ReLu is enabled or not*/
  const int8_t enableReLu = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_RELU(param);
  const int16_t minLim = enableReLu ? reluMin : SCHAR_MIN;
  const int16_t maxLim = enableReLu ? reluMax : SCHAR_MAX;

  /* Input and output data pointers */
  int8_t* restrict pInData11;
  int8_t* restrict pInData12;
  int8_t* restrict pInData21;
  int8_t* restrict pInData22;
  xb_vec2Nx8* restrict pdvecOutData;
  xb_vec2Nx8* restrict pdvecFilter;
  xb_vec2Nx8* restrict pdvecOutShift;
  xb_vecN_2x32v* restrict phvecOutScale1;
  xb_vecN_2x32v* restrict phvecOutScale2;
  xb_vecN_2x32v* restrict phvecBias1;
  xb_vecN_2x32v* restrict phvecBias2;
  xb_vec2Nx8 dvecOut1, dvecOut2, dvecOut3, dvecOut4;
  valign vaOutData = IVP_ZALIGN();

  /* calculate edge values */
  int32_t dilatedFilterWidth = (filterWidth - 1) * dilationX + 1;
  int32_t dilatedFilterHeight = (filterHeight - 1) * dilationY + 1;
  int32_t leftEdge, topEdge;
  if ((dilatedFilterWidth % 2) != 0)
  {
    leftEdge = dilatedFilterWidth / 2;
  }
  else
  {
    leftEdge = leftEdgeFlag ? (dilatedFilterWidth / 2) : ((dilatedFilterWidth / 2) - 1);
  }

  if ((dilatedFilterHeight % 2) != 0)
  {
    topEdge = dilatedFilterHeight / 2;
  }
  else
  {
    topEdge = topEdgeFlag ? (dilatedFilterHeight / 2) : ((dilatedFilterHeight / 2) - 1);
  }
  pInput = &pInput[-(topEdge * inPitch2 + leftEdge * inPitch1)];

  int32_t vectorizationWidth = (2 * XCHAL_IVPN_SIMD_WIDTH);

  /* Vectorization is along channels. Output width and output height are unrolled    */
  /* by 2. Kernel height and width and height loops are combined together            */
  /* In this case the address offsets for input  need to be derived from             */
  /* vector registers. These vector registers are initialized as follows             */
  xb_vecN_2x32Uv hvecInAddrOffInit = inPitch1 * dilationX;
  vboolN_2 vbN_2 = IVP_EQN_2X32(IVP_SEQN_2X32(), filterWidth - 1);
  hvecInAddrOffInit = IVP_MOVN_2X32T(((inPitch2 * dilationY) - ((filterWidth - 1) * inPitch1 * dilationX)), hvecInAddrOffInit, vbN_2);

  for (int32_t outY = 0; outY < outHeight; outY += 2) /* along output Height */
  {
    int32_t enable2Row = XT_SALT(outY, outHeight - 1);

    for (int32_t outX = 0; outX < outWidth; outX += 2)  /* along output Width */
    {
      int32_t enable2Col = XT_SALT(outX, outWidth - 1);
      int32_t enable2RowCol = enable2Col * enable2Row;
      for (int32_t ic = 0; ic < inDepth; ic++) /* Input depth index */
      {
        /* Loop index */
        int32_t arrayOffset = ic * depthMultiplier;
        for (int32_t m = 0; m < depthMultiplier; m += vectorizationWidth) // DepthMultiplier used to move about outputDepth
        {
          /* To handle corner case when number of output channels
           * is not a multiple of  2 * XCHAL_IVPN_SIMD_WIDTH*/
          int32_t remainingOutCh = XT_MIN(vectorizationWidth, depthMultiplier - m);
          int32_t remBiasLoad = (remainingOutCh > XCHAL_IVPN_SIMD_WIDTH) ? 1 : 0;

          int32_t remCh1 = XT_MIN(((depthMultiplier - m) << 2), 2 * XCHAL_IVPN_SIMD_WIDTH);
          int32_t remCh2 = XT_MIN(((depthMultiplier - m) << 2) - 2 * XCHAL_IVPN_SIMD_WIDTH, 2 * XCHAL_IVPN_SIMD_WIDTH);
          int32_t remCh3 = XT_MIN(((depthMultiplier - m) << 2) - 4 * XCHAL_IVPN_SIMD_WIDTH, 2 * XCHAL_IVPN_SIMD_WIDTH);
          int32_t remCh4 = XT_MIN(((depthMultiplier - m) << 2) - 6 * XCHAL_IVPN_SIMD_WIDTH, 2 * XCHAL_IVPN_SIMD_WIDTH);

          /* Load OutScale values */
          xb_vecN_2x32v hvecScale1, hvecScale2, hvecScale3, hvecScale4;
          phvecOutScale1 = (xb_vecN_2x32v*)(pOutScale + arrayOffset + m);
          phvecOutScale2 = (xb_vecN_2x32v*)(pOutScale + arrayOffset + m + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
          valign vaOutScale = IVP_LAN_2X32_PP(phvecOutScale1);
          IVP_LAVN_2X32_XP(hvecScale1, vaOutScale, phvecOutScale1, remCh1);
          IVP_LAVN_2X32_XP(hvecScale2, vaOutScale, phvecOutScale1, remCh2);
          vaOutScale = IVP_LAN_2X32_PP(phvecOutScale2);
          IVP_LAVN_2X32_XP(hvecScale3, vaOutScale, phvecOutScale2, remCh3);
          IVP_LAVN_2X32_XP(hvecScale4, vaOutScale, phvecOutScale2, remCh4);

          /* Load OutShift values */
          xb_vec2Nx8 dvecShift;
          pdvecOutShift = (xb_vec2Nx8*)(pOutShift + arrayOffset + m);
          valign vaOutShift = IVP_LA2NX8_PP(pdvecOutShift);
          IVP_LAV2NX8_XP(dvecShift, vaOutShift, pdvecOutShift, remainingOutCh);
          /* Calculate left shift and right shift values */
          vbool2N vb2N = IVP_LT2NX8(dvecShift, 0);
          xb_vec2Nx8 dvecRightShift = IVP_MOV2NX8T(0, dvecShift, vb2N);
          xb_vec2Nx8 dvecLeftShift = 0;
          IVP_SUB2NX8T(dvecLeftShift, 0, dvecShift, vb2N);
          xb_vec2Nx8 dvecRightShiftL, dvecRightShiftH;
          IVP_DSEL2NX8I(dvecRightShiftH, dvecRightShiftL, 0, dvecRightShift, IVP_DSELI_8B_INTERLEAVE_1);
          xb_vecNx16 vecRightShift1 = IVP_MOVNX16_FROM2NX8(dvecRightShiftL);
          xb_vecNx16 vecRightShift2 = IVP_MOVNX16_FROM2NX8(dvecRightShiftH);

          xb_vec2Nx8 dvecLeftShiftL, dvecLeftShiftH;
          IVP_DSEL2NX8I(dvecLeftShiftH, dvecLeftShiftL, 0, dvecLeftShift, IVP_DSELI_8B_INTERLEAVE_1);

          xb_vecNx16 vecLeftShiftLL, vecLeftShiftLH, vecLeftShiftHL, vecLeftShiftHH;
          IVP_DSELNX16I(vecLeftShiftLH, vecLeftShiftLL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftL), IVP_DSELI_INTERLEAVE_1);
          IVP_DSELNX16I(vecLeftShiftHH, vecLeftShiftHL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftH), IVP_DSELI_INTERLEAVE_1);

          xb_vecN_2x32v hvecLeftShift1 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLL);
          xb_vecN_2x32v hvecLeftShift2 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLH);
          xb_vecN_2x32v hvecLeftShift3 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHL);
          xb_vecN_2x32v hvecLeftShift4 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHH);

          /* Initialize accumulators with bias values */
          xb_vec2Nx24 daccSum1 = 0, daccSum2 = 0, daccSum3 = 0, daccSum4 = 0;
          xb_vecN_2x32v hvecBias1, hvecBias2, hvecBias3, hvecBias4;
          phvecBias1 = (xb_vecN_2x32v*)(pBias + arrayOffset + m);
          phvecBias2 = (xb_vecN_2x32v*)(pBias + arrayOffset + m + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);

          valign vaBias = IVP_LAN_2X32_PP(phvecBias1);
          IVP_LAVN_2X32_XP(hvecBias1, vaBias, phvecBias1, remCh1);
          IVP_LAVN_2X32_XP(hvecBias2, vaBias, phvecBias1, remCh2);
          vaBias = IVP_LAN_2X32_PP(phvecBias2);
          IVP_LAVN_2X32_XP(hvecBias3, vaBias, phvecBias2, remCh3);
          IVP_LAVN_2X32_XP(hvecBias4, vaBias, phvecBias2, remCh4);

          /* Input Data Pointers */
          int8_t* pData = pInput + ic + outX * strideWidth * inPitch1 + outY * strideHeight * inPitch2;
          /* Pointers for Input Data Loads */
          pInData11 = (int8_t*)(pData);
          pInData12 = (int8_t*)(pData + strideWidth * inPitch1 * enable2Col);
          pInData21 = (int8_t*)(pData + strideHeight * inPitch2 * enable2Row);
          pInData22 = (int8_t*)(pData + (strideWidth * inPitch1 + strideHeight * inPitch2) * enable2RowCol);

          /* Pointer for Coefficient Load */
          int8_t* pCoeff = pFilter + arrayOffset + m;
          pdvecFilter = (xb_vec2Nx8*)(pCoeff);
          int32_t i = 0;
          int32_t zeroPointIn = ((int32_t)inputOffset << 16) | ((int32_t)inputOffset & 0XFFFF);
          /* Pointer for Output Load */
          int8_t* pOut = pOutData + arrayOffset + m + outX * outPitch1 + outY * outPitch2;
          int32_t k;
          for (k = 0; k < (filterHeight * filterWidth) - 1; k += 2, i++) // along kernelWidth*kernelHeight
          {
            int32_t inAddrOffset = IVP_EXTRVRN_2X32(hvecInAddrOffInit, 4 * i);
            /* After every filterWidth no: of iterations, the offset, should start from
            0th position in the offset vector*/
            /* Making i = -1 when k = multiple of filterWidth, so that i++ will give i = 0
               for next iteration */
            i |= ((filterWidth - 2) - i) >> 31;
            xb_vec2Nx8 dvecFilter, dvecInData11, dvecInData12, dvecInData21, dvecInData22;
            xb_vec2Nx8 dvecFilterA, dvecInData11A, dvecInData12A, dvecInData21A, dvecInData22A;
            /* Load coefficient value*/
            valign valFilter = IVP_LA2NX8_PP(pdvecFilter);
            IVP_LA2NX8_XP(dvecFilter, valFilter, pdvecFilter, filterPitch1);
            /* Load Input data */
            IVP_LSR2NX8_XP(dvecInData11, pInData11, inAddrOffset);
            IVP_LSR2NX8_XP(dvecInData12, pInData12, inAddrOffset);
            IVP_LSR2NX8_XP(dvecInData21, pInData21, inAddrOffset);
            IVP_LSR2NX8_XP(dvecInData22, pInData22, inAddrOffset);

            i++;
            inAddrOffset = IVP_EXTRVRN_2X32(hvecInAddrOffInit, 4 * i);
            /* After every filterWidth no: of iterations, the offset, should start from
            0th position in the offset vector*/
            /* Making i = -1 when k = multiple of filterWidth, so that i++ will give i = 0
            for next iteration */
            i |= ((filterWidth - 2) - i) >> 31;
            /* Load coefficient value*/
            valFilter = IVP_LA2NX8_PP(pdvecFilter);
            IVP_LA2NX8_XP(dvecFilterA, valFilter, pdvecFilter, filterPitch1);
            /* Load Input data */
            IVP_LSR2NX8_XP(dvecInData11A, pInData11, inAddrOffset);
            IVP_LSR2NX8_XP(dvecInData12A, pInData12, inAddrOffset);
            IVP_LSR2NX8_XP(dvecInData21A, pInData21, inAddrOffset);
            IVP_LSR2NX8_XP(dvecInData22A, pInData22, inAddrOffset);

            IVP_MULPA2NX8(daccSum1, dvecInData11, dvecFilter, dvecInData11A, dvecFilterA);
            IVP_MULPA2NX8(daccSum2, dvecInData12, dvecFilter, dvecInData12A, dvecFilterA);
            IVP_MULPA2NX8(daccSum3, dvecInData21, dvecFilter, dvecInData21A, dvecFilterA);
            IVP_MULPA2NX8(daccSum4, dvecInData22, dvecFilter, dvecInData22A, dvecFilterA);
            IVP_MULPA2N8XR16(daccSum1, dvecFilter, dvecFilterA, zeroPointIn);
            IVP_MULPA2N8XR16(daccSum2, dvecFilter, dvecFilterA, zeroPointIn);
            IVP_MULPA2N8XR16(daccSum3, dvecFilter, dvecFilterA, zeroPointIn);
            IVP_MULPA2N8XR16(daccSum4, dvecFilter, dvecFilterA, zeroPointIn);
          }
          if (k < (filterHeight * filterWidth))
          {
            xb_vec2Nx8 dvecFilter, dvecInData11, dvecInData12, dvecInData21, dvecInData22;
            /* Load coefficient value*/
            valign valFilter = IVP_LA2NX8_PP(pdvecFilter);
            IVP_LA2NX8_XP(dvecFilter, valFilter, pdvecFilter, filterPitch1);
            /* Load Input data */
            IVP_LSR2NX8_XP(dvecInData11, pInData11, 1);
            IVP_LSR2NX8_XP(dvecInData12, pInData12, 1);
            IVP_LSR2NX8_XP(dvecInData21, pInData21, 1);
            IVP_LSR2NX8_XP(dvecInData22, pInData22, 1);

            IVP_MULA2NX8(daccSum1, dvecInData11, dvecFilter);
            IVP_MULA2NX8(daccSum2, dvecInData12, dvecFilter);
            IVP_MULA2NX8(daccSum3, dvecInData21, dvecFilter);
            IVP_MULA2NX8(daccSum4, dvecInData22, dvecFilter);
            IVP_MULA2N8XR16(daccSum1, dvecFilter, zeroPointIn);
            IVP_MULA2N8XR16(daccSum2, dvecFilter, zeroPointIn);
            IVP_MULA2N8XR16(daccSum3, dvecFilter, zeroPointIn);
            IVP_MULA2N8XR16(daccSum4, dvecFilter, zeroPointIn);
          }
          xb_vecNx16 vecOut11, vecOut12, vecOut21, vecOut22;
          xb_vecNx16 vecOut31, vecOut32, vecOut41, vecOut42;
          DEPTHWISE_DILATED_BIAS_ADD_PACK_SCALE_AND_ROUNDING_VQ(daccSum1, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecLeftShift1 \
            , hvecScale1, hvecScale2, hvecScale3, hvecScale4, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, \
            minLim, maxLim, vecOut11, vecOut12);
          DEPTHWISE_DILATED_BIAS_ADD_PACK_SCALE_AND_ROUNDING_VQ(daccSum2, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecLeftShift1 \
            , hvecScale1, hvecScale2, hvecScale3, hvecScale4, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, \
            minLim, maxLim, vecOut21, vecOut22);
          DEPTHWISE_DILATED_BIAS_ADD_PACK_SCALE_AND_ROUNDING_VQ(daccSum3, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecLeftShift1 \
            , hvecScale1, hvecScale2, hvecScale3, hvecScale4, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, \
            minLim, maxLim, vecOut31, vecOut32);
          DEPTHWISE_DILATED_BIAS_ADD_PACK_SCALE_AND_ROUNDING_VQ(daccSum4, hvecBias1, hvecBias2, hvecBias3, hvecBias4, hvecLeftShift1 \
            , hvecScale1, hvecScale2, hvecScale3, hvecScale4, hvecLeftShift2, hvecLeftShift3, hvecLeftShift4, vecRightShift1, vecRightShift2, \
            minLim, maxLim, vecOut41, vecOut42);

          /* Store output */
          dvecOut1 = IVP_SEL2NX8I(IVP_MOV2NX8_FROMNX16(vecOut12), IVP_MOV2NX8_FROMNX16(vecOut11), IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0);
          dvecOut2 = IVP_SEL2NX8I(IVP_MOV2NX8_FROMNX16(vecOut22), IVP_MOV2NX8_FROMNX16(vecOut21), IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0);
          dvecOut3 = IVP_SEL2NX8I(IVP_MOV2NX8_FROMNX16(vecOut32), IVP_MOV2NX8_FROMNX16(vecOut31), IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0);
          dvecOut4 = IVP_SEL2NX8I(IVP_MOV2NX8_FROMNX16(vecOut42), IVP_MOV2NX8_FROMNX16(vecOut41), IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0);

          pdvecOutData = (xb_vec2Nx8*)(pOut);
          IVP_SAV2NX8_XP(dvecOut1, vaOutData, pdvecOutData, remainingOutCh);
          IVP_SAPOS2NX8_FP(vaOutData, pdvecOutData);

          pdvecOutData = (xb_vec2Nx8*)(pOut + outPitch1 * enable2Col);
          IVP_SAV2NX8_XP(dvecOut2, vaOutData, pdvecOutData, remainingOutCh * enable2Col);
          IVP_SAPOS2NX8_FP(vaOutData, pdvecOutData);

          pdvecOutData = (xb_vec2Nx8*)(pOut + outPitch2 * enable2Row);
          IVP_SAV2NX8_XP(dvecOut3, vaOutData, pdvecOutData, remainingOutCh * enable2Row);
          IVP_SAPOS2NX8_FP(vaOutData, pdvecOutData);

          pdvecOutData = (xb_vec2Nx8*)(pOut + outPitch1 * enable2Col + outPitch2 * enable2Row);
          IVP_SAV2NX8_XP(dvecOut4, vaOutData, pdvecOutData, remainingOutCh * enable2Col * enable2Row);
          IVP_SAPOS2NX8_FP(vaOutData, pdvecOutData);
        }
      }
    }
  }
  return(XI_ERROR_STATUS());
}

#endif //if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))

