/*
 * Copyright (c) 2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
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

#if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))

#define UNSIGNED8BIT             1
#define SIGNED8BIT               2
#define UNSIGNED8BITSIGNED8BIT   3
#define MAKE_NAME_IMPL(name, MORPH_FNAME_SPECIFIER_IDT)  name ## _ ## MORPH_FNAME_SPECIFIER_IDT

#undef MAKE_NAME
#undef MORPH_TILE3D_CHECK
#undef MORPH_TILE4D_CHECK
#undef MORPH_IDT_SCALAR
#undef MORPH_IDT_2Nx8
#undef MORPH_IDT_Nx8
#undef MORPH_IDT_Nx16
#undef MORPH_OP_PRIME_2Nx8
#undef MORPH_OP_LOAD_2Nx8_IP
#undef MORPH_OP_LOAD_2Nx8_VAR
#undef MORPH_LV2NX8_I
#undef MORPH_LV2NX8_IP
#undef MORPH_OP_PRIME_NX8
#undef MORPH_LOAD_Nx8_IP
#undef MORPH_LOAD_NX8_VAR
#undef MORPH_LVNX8_I
#undef MORPH_LVNX8_XP
#undef MORPH_LVNX8_IP
#undef MORPH_MULPA2N8XR16
#undef MORPH_MULPA2NX8
#undef MORPH_MULA2NX8
#undef MORPH_MULA2N8XR16
#undef MORPH_SAVNX8_XP
#undef MORPH_SAPOSNX8_FP
#undef MORPH_MIN_VALUE
#undef MORPH_MAX_VALUE
#undef MORPH_MAX_SIZE
#undef MORPH_GATHERANX8
#undef MORPH_GATHERDNX8
#undef MORPH_IDT_SCALAR_COEFF
#undef MORPH_IDT_2Nx8_COEFF
#undef MORPH_IDT_Nx8_COEFF
#undef MORPH_IDT_Nx16_COEFF
#undef MORPH_OP_PRIME_2Nx8_COEFF
#undef MORPH_OP_LOAD_2Nx8_IP_COEFF
#undef MORPH_OP_LOAD_2Nx8_VAR_COEFF
#undef MORPH_OP_PRIME_NX8_COEFF
#undef MORPH_LOAD_Nx8_IP_COEFF
#undef MORPH_LOAD_NX8_VAR_COEFF
#undef MORPH_MULPA2N8XR16_COEFF
#undef MORPH_MIN_VALUE_COEFF
#undef MORPH_MAX_VALUE_COEFF

#if INPUT_DATA_TYPE == UNSIGNED8BIT

#define MAKE_NAME(name)               MAKE_NAME_IMPL(name, U8)
#define MORPH_TILE3D_CHECK            XI_CHECK_TILE3D_U8
#define MORPH_TILE4D_CHECK            XI_CHECK_TILE4D_U8
#define MORPH_IDT_SCALAR              uint8_t
#define MORPH_IDT_2Nx8                xb_vec2Nx8U
#define MORPH_IDT_Nx8                 xb_vecNx8U
#define MORPH_IDT_Nx16                xb_vecNx16U
#define MORPH_OP_PRIME_2Nx8           IVP_LA2NX8U_PP
#define MORPH_OP_LOAD_2Nx8_IP         IVP_LA2NX8U_IP
#define MORPH_OP_LOAD_2Nx8_VAR        IVP_LAV2NX8U_XP
#define MORPH_LV2NX8_I                IVP_LV2NX8U_I
#define MORPH_LV2NX8_IP               IVP_LV2NX8U_IP
#define MORPH_OP_PRIME_NX8            IVP_LANX8U_PP
#define MORPH_LOAD_Nx8_IP             IVP_LANX8U_IP
#define MORPH_LOAD_NX8_VAR            IVP_LAVNX8U_XP
#define MORPH_LVNX8_I                 IVP_LVNX8U_I
#define MORPH_LVNX8_XP                IVP_LVNX8U_XP
#define MORPH_LVNX8_IP                IVP_LVNX8U_IP
#define MORPH_MULPA2N8XR16            IVP_MULUSPA2N8XR16
#define MORPH_MULPA2NX8               IVP_MULUUPA2NX8
#define MORPH_MULA2NX8                IVP_MULUUA2NX8U
#define MORPH_MULA2N8XR16             IVP_MULUSA2N8XR16
#define MORPH_SAVNX8_XP               IVP_SAVNX8U_XP
#define MORPH_SAPOSNX8_FP             IVP_SAPOSNX8U_FP
#define MORPH_MIN_VALUE               0
#define MORPH_MAX_VALUE               UCHAR_MAX
#define MORPH_MAX_SIZE                8256
#define MORPH_GATHERANX8              IVP_GATHERANX8U
#define MORPH_GATHERDNX8              IVP_GATHERDNX8U
#define MORPH_IDT_SCALAR_COEFF        uint8_t
#define MORPH_IDT_2Nx8_COEFF          xb_vec2Nx8U
#define MORPH_IDT_Nx8_COEFF           xb_vecNx8U
#define MORPH_IDT_Nx16_COEFF          xb_vecNx16U
#define MORPH_OP_PRIME_2Nx8_COEFF     IVP_LA2NX8U_PP
#define MORPH_OP_LOAD_2Nx8_IP_COEFF   IVP_LA2NX8U_IP
#define MORPH_OP_LOAD_2Nx8_VAR_COEFF  IVP_LAV2NX8U_XP
#define MORPH_OP_PRIME_NX8_COEFF      IVP_LANX8U_PP
#define MORPH_LOAD_Nx8_IP_COEFF       IVP_LANX8U_IP
#define MORPH_LOAD_NX8_VAR_COEFF      IVP_LAVNX8U_XP
#define MORPH_MULPA2N8XR16_COEFF      IVP_MULUSPA2N8XR16
#define MORPH_MIN_VALUE_COEFF         0
#define MORPH_MAX_VALUE_COEFF         UCHAR_MAX

#elif INPUT_DATA_TYPE == SIGNED8BIT

#define MAKE_NAME(name)               MAKE_NAME_IMPL(name, S8)
#define MORPH_TILE3D_CHECK            XI_CHECK_TILE3D_S8
#define MORPH_TILE4D_CHECK            XI_CHECK_TILE4D_S8
#define MORPH_IDT_SCALAR              int8_t
#define MORPH_IDT_2Nx8                xb_vec2Nx8
#define MORPH_IDT_Nx8                 xb_vecNx8
#define MORPH_IDT_Nx16                xb_vecNx16
#define MORPH_OP_PRIME_2Nx8           IVP_LA2NX8_PP
#define MORPH_OP_LOAD_2Nx8_IP         IVP_LA2NX8_IP
#define MORPH_OP_LOAD_2Nx8_VAR        IVP_LAV2NX8_XP
#define MORPH_LV2NX8_I                IVP_LV2NX8_I
#define MORPH_LV2NX8_IP               IVP_LV2NX8_IP
#define MORPH_OP_PRIME_NX8            IVP_LANX8S_PP
#define MORPH_LOAD_Nx8_IP             IVP_LANX8S_IP
#define MORPH_LOAD_NX8_VAR            IVP_LAVNX8S_XP
#define MORPH_LVNX8_I                 IVP_LVNX8S_I
#define MORPH_LVNX8_XP                IVP_LVNX8S_XP
#define MORPH_LVNX8_IP                IVP_LVNX8S_IP
#define MORPH_MULPA2N8XR16            IVP_MULPA2N8XR16
#define MORPH_MULPA2NX8               IVP_MULPA2NX8
#define MORPH_MULA2NX8                IVP_MULA2NX8
#define MORPH_MULA2N8XR16             IVP_MULA2N8XR16
#define MORPH_SAVNX8_XP               IVP_SAVNX8S_XP
#define MORPH_SAPOSNX8_FP             IVP_SAPOSNX8S_FP
#define MORPH_MIN_VALUE               SCHAR_MIN
#define MORPH_MAX_VALUE               SCHAR_MAX
#define MORPH_MAX_SIZE                16448
#define MORPH_GATHERANX8              IVP_GATHERANX8S
#define MORPH_GATHERDNX8              IVP_GATHERDNX8S
#define MORPH_IDT_SCALAR_COEFF        int8_t
#define MORPH_IDT_2Nx8_COEFF          xb_vec2Nx8
#define MORPH_IDT_Nx8_COEFF           xb_vecNx8
#define MORPH_IDT_Nx16_COEFF          xb_vecNx16
#define MORPH_OP_PRIME_2Nx8_COEFF     IVP_LA2NX8_PP
#define MORPH_OP_LOAD_2Nx8_IP_COEFF   IVP_LA2NX8_IP
#define MORPH_OP_LOAD_2Nx8_VAR_COEFF  IVP_LAV2NX8_XP
#define MORPH_OP_PRIME_NX8_COEFF      IVP_LANX8S_PP
#define MORPH_LOAD_Nx8_IP_COEFF       IVP_LANX8S_IP
#define MORPH_LOAD_NX8_VAR_COEFF      IVP_LAVNX8S_XP
#define MORPH_MULPA2N8XR16_COEFF      IVP_MULPA2N8XR16
#define MORPH_MIN_VALUE_COEFF         SCHAR_MIN
#define MORPH_MAX_VALUE_COEFF         SCHAR_MAX

#elif INPUT_DATA_TYPE == UNSIGNED8BITSIGNED8BIT

#define MAKE_NAME(name)               MAKE_NAME_IMPL(name, U8S8U8)
#define MORPH_TILE3D_CHECK            XI_CHECK_TILE3D_U8
#define MORPH_TILE4D_CHECK            XI_CHECK_TILE4D_S8
#define MORPH_IDT_SCALAR              uint8_t
#define MORPH_IDT_2Nx8                xb_vec2Nx8U
#define MORPH_IDT_Nx8                 xb_vecNx8U
#define MORPH_IDT_Nx16                xb_vecNx16U
#define MORPH_OP_PRIME_2Nx8           IVP_LA2NX8U_PP
#define MORPH_OP_LOAD_2Nx8_IP         IVP_LA2NX8U_IP
#define MORPH_OP_LOAD_2Nx8_VAR        IVP_LAV2NX8U_XP
#define MORPH_LV2NX8_I                IVP_LV2NX8_I
#define MORPH_LV2NX8_IP               IVP_LV2NX8_IP
#define MORPH_OP_PRIME_NX8            IVP_LANX8U_PP
#define MORPH_LOAD_Nx8_IP             IVP_LANX8U_IP
#define MORPH_LOAD_NX8_VAR            IVP_LAVNX8U_XP
#define MORPH_LVNX8_I                 IVP_LVNX8S_I
#define MORPH_LVNX8_XP                IVP_LVNX8S_XP
#define MORPH_LVNX8_IP                IVP_LVNX8S_IP
#define MORPH_MULPA2N8XR16            IVP_MULUSPA2N8XR16
#define MORPH_MULPA2NX8               IVP_MULUSPA2NX8
#define MORPH_MULA2NX8                IVP_MULUSA2NX8
#define MORPH_MULA2N8XR16             IVP_MULA2N8XR16
#define MORPH_SAVNX8_XP               IVP_SAVNX8U_XP
#define MORPH_SAPOSNX8_FP             IVP_SAPOSNX8U_FP
#define MORPH_MIN_VALUE               0
#define MORPH_MAX_VALUE               UCHAR_MAX
#define MORPH_MAX_SIZE                8256
#define MORPH_GATHERANX8              IVP_GATHERANX8S
#define MORPH_GATHERDNX8              IVP_GATHERDNX8S
#define MORPH_IDT_SCALAR_COEFF        int8_t
#define MORPH_IDT_2Nx8_COEFF          xb_vec2Nx8
#define MORPH_IDT_Nx8_COEFF           xb_vecNx8
#define MORPH_IDT_Nx16_COEFF          xb_vecNx16
#define MORPH_OP_PRIME_2Nx8_COEFF     IVP_LA2NX8_PP
#define MORPH_OP_LOAD_2Nx8_IP_COEFF   IVP_LA2NX8_IP
#define MORPH_OP_LOAD_2Nx8_VAR_COEFF  IVP_LAV2NX8_XP
#define MORPH_OP_PRIME_NX8_COEFF      IVP_LANX8S_PP
#define MORPH_LOAD_Nx8_IP_COEFF       IVP_LANX8S_IP
#define MORPH_LOAD_NX8_VAR_COEFF      IVP_LAVNX8S_XP
#define MORPH_MULPA2N8XR16_COEFF      IVP_MULPA2N8XR16
#define MORPH_MIN_VALUE_COEFF         SCHAR_MIN
#define MORPH_MAX_VALUE_COEFF         SCHAR_MAX
#endif

#define FIXUP_CALCULATION(pdvecInData, dotprodLength, hvecFixup, outOffset)                       \
{                                                                                                 \
  pdvecInData = (MORPH_IDT_2Nx8 *) pInData;                                                       \
  valign vaIn = MORPH_OP_PRIME_2Nx8(pdvecInData);                                                 \
  int32_t dimVal;                                                                                 \
  xb_vec2Nx24 daccSum = 0;                                                                        \
  for(dimVal = 0; dimVal <= dotprodLength - vectorizationWidth4x; dimVal +=vectorizationWidth4x)  \
  {                                                                                               \
    MORPH_IDT_2Nx8 dvecIn1, dvecIn2;                                                              \
    MORPH_OP_LOAD_2Nx8_IP(dvecIn1, vaIn, pdvecInData);                                            \
    MORPH_OP_LOAD_2Nx8_IP(dvecIn2, vaIn, pdvecInData);                                            \
    MORPH_MULPA2N8XR16(daccSum, dvecIn2, dvecIn1, zeroPCoeffOffset);                              \
  }                                                                                               \
  if(dimVal < dotprodLength)                                                                      \
  {                                                                                               \
    MORPH_IDT_2Nx8 dvecIn1, dvecIn2;                                                              \
    int32_t remX = dotprodLength - dimVal;                                                        \
    MORPH_OP_LOAD_2Nx8_VAR(dvecIn1, vaIn, pdvecInData, remX);                                     \
    MORPH_OP_LOAD_2Nx8_VAR(dvecIn2, vaIn, pdvecInData, (remX - 2*XCHAL_IVPN_SIMD_WIDTH));         \
    MORPH_MULPA2N8XR16(daccSum, dvecIn2, dvecIn1, zeroPCoeffOffset);                              \
  }                                                                                               \
  xb_vecN_2x32v hvecSum = IVP_ADDN_2X32(IVP_CVT32S2NX24LL(daccSum), IVP_CVT32S2NX24LH(daccSum));  \
  hvecSum   = IVP_ADDN_2X32(hvecSum, IVP_CVT32S2NX24HL(daccSum));                                 \
  hvecSum   = IVP_ADDN_2X32(hvecSum, IVP_CVT32S2NX24HH(daccSum));                                 \
  int32_t fixupTerm = IVP_RADDN_2X32(hvecSum);                                                    \
  hvecFixup = IVP_ADDN_2X32((xb_vecN_2x32v)outOffset, (xb_vecN_2x32v)fixupTerm);                  \
}


/* optimized FC for input depth of 32. (for specific customer use case) */
void MAKE_NAME(fullyConnectedA3D_S_QM24_MODGS)(const xi_pTile3D inTile,
                                         const xi_pTile4D coeffTile,
                                         const xi_pArray biasArray,
                                         xi_pTile3D outTile,
                                         const xi_cnna_conv_params *param)
{
  const int32_t quantScale   = XI_CNNA_CONV_GET_QUANT_SCALE(param);
  const int32_t outShift     = XI_CNNA_CONV_GET_OUTPUT_SHIFT(param);
  const int16_t zeroPtInput  = XI_CNNA_CONV_GET_ZEROPT_INPUT(param);
  const int16_t zeroPtCoeff  = XI_CNNA_CONV_GET_ZEROPT_COEFF(param);
  const int16_t zeroPtOutput = XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param);
  const uint8_t enableReLu   = XI_CNNA_CONV_GET_FLAG_RELU(param);
  const int16_t reluMin      = XI_CNNA_CONV_GET_RELUMIN(param);
  const int16_t reluMax      = XI_CNNA_CONV_GET_RELUMAX(param);
  const int16_t minLim       = enableReLu ? reluMin : MORPH_MIN_VALUE;
  const int16_t maxLim       = enableReLu ? reluMax : MORPH_MAX_VALUE;
  int32_t leftShift          = outShift < 0 ? -outShift : 0;
  int32_t rightShift         = outShift < 0 ? 0 : outShift;
  const int32_t inDim1       = XI_TILE3D_GET_DIM1(inTile);
  const int32_t inDim2       = XI_TILE3D_GET_DIM2(inTile);
  const int32_t inDim3       = XI_TILE3D_GET_DIM3(inTile);
  const int32_t numOut       = XI_TILE4D_GET_DIM4(coeffTile);
  const int32_t coeffPitch3  = XI_TILE4D_GET_DIM3_PITCH(coeffTile);
  MORPH_IDT_SCALAR *pInData          = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile);
  MORPH_IDT_SCALAR *pOutData         = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(outTile);
  MORPH_IDT_SCALAR_COEFF *pCoeffData = (MORPH_IDT_SCALAR_COEFF *)XI_TILE4D_GET_DATA_PTR(coeffTile);
  int32_t *pBiasData                 = (int32_t *) XI_ARRAY_GET_DATA_PTR(biasArray);
  int32_t outN, k;
  int32_t dotprodLength    = inDim1 * inDim2 * inDim3;
  MORPH_IDT_Nx8*  restrict pvecOut;
  xb_vecN_2x32v*  restrict phvecBias;
  MORPH_IDT_Nx8 * restrict pvecIn;
  pvecOut = (MORPH_IDT_Nx8 *)(pOutData);
  valign vaOutData = IVP_ZALIGN();
  xb_vecN_2x32v hvecBias1,hvecBias2;
  xb_vecNx16  vecinput1,vecinput2;
  xb_vecNx48 accSum1 = 0;
  xb_vecNx16U vecGatherIdx, vecGatherIdx1;

  pvecIn = (MORPH_IDT_Nx8 *)pInData;
  valign vaIn = MORPH_OP_PRIME_NX8(pvecIn);
  MORPH_IDT_Nx16 vecIn32;
  MORPH_LOAD_NX8_VAR(vecIn32, vaIn, pvecIn, 32);
  vecIn32 = IVP_SUBNX16(vecIn32, zeroPtInput);
  uint16_t maxCoeffLoc = numOut * coeffPitch3 - 1;
  xb_vec2Nx8 dvecSeq = IVP_AND2NX8(IVP_SEQ2NX8(), 1);
  for (outN = 0; outN < numOut; outN += 32)
  {
    phvecBias = (xb_vecN_2x32v *)(pBiasData + outN);
    valign vaBias = IVP_LAN_2X32_PP(phvecBias);
    IVP_LAVN_2X32_XP(hvecBias1, vaBias, phvecBias , 4*(numOut - outN) );
    IVP_LAVN_2X32_XP(hvecBias2, vaBias, phvecBias , 4*(numOut - outN - 16) );
    xb_vecNx16U vecSeqMax = IVP_MINUNX16(IVP_SEQNX16U(), (numOut - outN - 1));
    vecGatherIdx = IVP_MULNX16UPACKL(vecSeqMax + (xb_vecNx16U)outN, (xb_vecNx16U)coeffPitch3);
    vecGatherIdx1 = IVP_ADDNX16U(vecGatherIdx, 1);
    xb_vecNx16U vecSeq1 = IVP_SELNX16I(vecGatherIdx1, vecGatherIdx, IVP_SELI_INTERLEAVE_1_LO);
    xb_vecNx16U vecSeq2 = IVP_ADDNX16U(vecSeq1, coeffPitch3 << 4);
    accSum1 = IVP_CVT48SNX32(hvecBias2, hvecBias1);
    vecGatherIdx = IVP_MOVNX16UT(0, vecSeq1, IVP_GTUNX16U(vecSeq1, maxCoeffLoc));
    vecGatherIdx1 = IVP_MOVNX16UT(0, vecSeq2, IVP_GTUNX16U(vecSeq2, maxCoeffLoc));
    for (k = 0; k < (dotprodLength); k+=2)
    {
      IVP_DSELNX16U(vecinput2, vecinput1, vecIn32, vecIn32, dvecSeq);
      xb_gsr gatherReg1 = MORPH_GATHERANX8(pCoeffData, vecGatherIdx);
      xb_vecNx16  vecCoef1 = MORPH_GATHERDNX8(gatherReg1);
      xb_gsr gatherReg2 = MORPH_GATHERANX8(pCoeffData, vecGatherIdx1);
      xb_vecNx16  vecCoef2 = MORPH_GATHERDNX8(gatherReg2);
      vecCoef1 = IVP_SUBNX16(vecCoef1, zeroPtCoeff);
      vecCoef2 = IVP_SUBNX16(vecCoef2, zeroPtCoeff);
      IVP_DSELNX16I(vecCoef2, vecCoef1, vecCoef2, vecCoef1, IVP_DSELI_DEINTERLEAVE_1);
      IVP_MULPANX16(accSum1, vecCoef1, vecinput1, vecCoef2, vecinput2);
      vecGatherIdx = IVP_ADDNX16U(vecGatherIdx, 2);
      vecGatherIdx1 = IVP_ADDNX16U(vecGatherIdx1, 2);
      dvecSeq = IVP_ADD2NX8(dvecSeq, 2);
    }
    xb_vecN_2x32v  hvecSum1 = IVP_CVT32UNX48L(accSum1);
    xb_vecN_2x32v  hvecSum2 = IVP_CVT32UNX48H(accSum1);
    MORPH_IDT_Nx16 vecOutData1,vecOutData2;
    SCALE_SHIFT_ADDZP_CLAMP_LIMITS_FC(vecOutData1, hvecSum1, quantScale, leftShift,rightShift, zeroPtOutput, minLim, maxLim);
    SCALE_SHIFT_ADDZP_CLAMP_LIMITS_FC(vecOutData2, hvecSum2, quantScale, leftShift,rightShift, zeroPtOutput, minLim, maxLim);
    MORPH_IDT_Nx16 vecOutData = IVP_SELNX16I(vecOutData2, vecOutData1, IVP_SELI_16B_EXTRACT_LO_HALVES);
    MORPH_SAVNX8_XP(vecOutData, vaOutData, pvecOut, (numOut - outN));
    MORPH_SAPOSNX8_FP(vaOutData, pvecOut);
  }
}

void MAKE_NAME(fullyConnectedA3D_S_QM24)(const xi_pTile3D inTile,
                                         const xi_pTile4D coeffTile,
                                         const xi_pArray biasArray,
                                         xi_pTile3D outTile,
                                         const xi_cnna_conv_params *param)
{
  /* CNNA convolution parameters */
  const int32_t quantScale   = XI_CNNA_CONV_GET_QUANT_SCALE(param);
  const int32_t outShift     = XI_CNNA_CONV_GET_OUTPUT_SHIFT(param);
  const int16_t zeroPtInput  = XI_CNNA_CONV_GET_ZEROPT_INPUT(param);
  const int16_t zeroPtCoeff  = XI_CNNA_CONV_GET_ZEROPT_COEFF(param);
  const int16_t zeroPtOutput = XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param);
  const uint8_t enableReLu   = XI_CNNA_CONV_GET_FLAG_RELU(param);
  const int16_t reluMin      = XI_CNNA_CONV_GET_RELUMIN(param);
  const int16_t reluMax      = XI_CNNA_CONV_GET_RELUMAX(param);

  /* Setting the limits for output data according to ReLu is enabled or not*/
  const int16_t minLim = enableReLu ? reluMin : MORPH_MIN_VALUE;
  const int16_t maxLim = enableReLu ? reluMax : MORPH_MAX_VALUE;
  /* Left and Right shift values */
  int32_t leftShift  = outShift < 0 ? -outShift : 0;
  int32_t rightShift = outShift < 0 ? 0 : outShift;
  /* Getting parameters from the tile structures */
  const int32_t inDim1      = XI_TILE3D_GET_DIM1(inTile);
  const int32_t inDim2      = XI_TILE3D_GET_DIM2(inTile);
  const int32_t inDim3      = XI_TILE3D_GET_DIM3(inTile);
  const int32_t numOut      = XI_TILE4D_GET_DIM4(coeffTile);
  const int32_t coeffPitch3 = XI_TILE4D_GET_DIM3_PITCH(coeffTile);

  /* Data Pointers of input, output, coefficient and bias data */
  MORPH_IDT_SCALAR *pInData          = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile);
  MORPH_IDT_SCALAR *pOutData         = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(outTile);
  MORPH_IDT_SCALAR_COEFF *pCoeffData = (MORPH_IDT_SCALAR_COEFF *)XI_TILE4D_GET_DATA_PTR(coeffTile);
  int32_t *pBiasData                 = (int32_t *) XI_ARRAY_GET_DATA_PTR(biasArray);
  /* Loop Variables */
  int32_t outN, k;
  int32_t dotprodLength    = inDim1 * inDim2 * inDim3;
  int16_t zeroPInS         = ((int16_t)zeroPtInput * -1);
  int16_t zeroPCoeffS      = ((int16_t)zeroPtCoeff * -1);
  int32_t zeroPInOffset    = (((uint32_t)((uint16_t)zeroPInS) << 16) + ((uint32_t)((uint16_t)zeroPInS)));
  int32_t zeroPCoeffOffset = (((uint32_t)((uint16_t)zeroPCoeffS) << 16) + ((uint32_t)((uint16_t)zeroPCoeffS)));
  int32_t outOffset        = (int32_t)(zeroPtCoeff * zeroPtInput * dotprodLength);
  /* Input and output data pointers */
  MORPH_IDT_2Nx8* restrict pdvecInData;
  MORPH_IDT_2Nx8_COEFF* restrict pdvecCoeffData1;
  MORPH_IDT_2Nx8_COEFF* restrict pdvecCoeffData2;
  MORPH_IDT_2Nx8_COEFF* restrict pdvecCoeffData3;
  MORPH_IDT_2Nx8_COEFF* restrict pdvecCoeffData4;
  MORPH_IDT_Nx8*  restrict pvecOut;
  xb_vecN_2x32v*  restrict phvecBias;
  /* Initialize output data pointer */
  pvecOut = (MORPH_IDT_Nx8 *)(pOutData);
  valign vaOutData = IVP_ZALIGN();
  /* Vectorization Width */
  int32_t vectorizationWidth4x = (4 * XCHAL_IVPN_SIMD_WIDTH);
  /* Calculate fixup term */
  /* fixupTerm = input * (-zeroPtCoeffU) + zeroPtCoeffU *  zeroPtInputU */
  xb_vecN_2x32v hvecFixup;
  FIXUP_CALCULATION(pdvecInData, dotprodLength, hvecFixup, outOffset);

  /* There are 2 implementations of Fully Connected Convolution.
   * First implementation is used, when the coeff Data pointer is aligned to SIMD width and
   * dot product length is a multiple of to SIMD width. In this implementation, all the accumulators.
   * are utilized thereby generating 4 outputs at a time. Here dotprodLength loop
   * is unrolled by 4 * XCHAL_IVPN_SIMD_WIDTH.
   * In the second implementation,2 outputs are generated at a time.
   * dotprodLength loop is unrolled by 2 * XCHAL_IVPN_SIMD_WIDTH
   */
  if (XI_TILE3D_IS_PTR_ALIGNED_2NX8(coeffTile) && ((dotprodLength & (2 * XCHAL_IVPN_SIMD_WIDTH -1)) == 0))
  {
    /* The loop gives 4 outputs per iteration */
    for (outN = 0; outN < numOut - 3; outN += 4)
    {
      /* Load bias to the first element in accumulators */
      xb_vecN_2x32v hvecBias;
      phvecBias = (xb_vecN_2x32v *)(pBiasData + outN);
      valign vaBias = IVP_LAN_2X32_PP(phvecBias);
      IVP_LAVN_2X32_XP(hvecBias, vaBias, phvecBias, 4 * 4);
      /* Add fixup term with bias value */
      IVP_ADDN_2X32T(hvecBias, hvecFixup, hvecBias, IVP_LTRN_2I(4));
      /* Initialize accumulators */
      xb_vec2Nx24 daccSum1 = 0;
      xb_vec2Nx24 daccSum2 = 0;
      xb_vec2Nx24 daccSum3 = 0;
      xb_vec2Nx24 daccSum4 = 0;
      /* Input Data and Coefficient Pointers */
      pdvecInData     = (MORPH_IDT_2Nx8 *) pInData;
      pdvecCoeffData1 = (MORPH_IDT_2Nx8_COEFF *)(pCoeffData + outN * coeffPitch3);
      pdvecCoeffData2 = (MORPH_IDT_2Nx8_COEFF *)(pCoeffData + (outN + 1) * coeffPitch3);
      pdvecCoeffData3 = (MORPH_IDT_2Nx8_COEFF *)(pCoeffData + (outN + 2) * coeffPitch3);
      pdvecCoeffData4 = (MORPH_IDT_2Nx8_COEFF *)(pCoeffData + (outN + 3) * coeffPitch3);
      /* Priming for Input Data */
      valign vaInData = MORPH_OP_PRIME_2Nx8(pdvecInData);
      /* Multiplying and Accumulating 128 bytes at a time */
      for (k = 0; k <= (dotprodLength - vectorizationWidth4x); k += vectorizationWidth4x)
      { /* (inDim1 * inDim2 * inDim3) Loop */
        /* Input Data Load */
        MORPH_IDT_2Nx8 dvecInData1; MORPH_OP_LOAD_2Nx8_IP(dvecInData1, vaInData, pdvecInData);
        MORPH_IDT_2Nx8 dvecInData2; MORPH_OP_LOAD_2Nx8_IP(dvecInData2, vaInData, pdvecInData);
        /* Coefficient Data Load */
        MORPH_IDT_2Nx8_COEFF dvecCoeffData11, dvecCoeffData21, dvecCoeffData31, dvecCoeffData41;
        MORPH_IDT_2Nx8_COEFF dvecCoeffData12, dvecCoeffData22, dvecCoeffData32, dvecCoeffData42;
        dvecCoeffData12 = MORPH_LV2NX8_I(pdvecCoeffData1, 2 * XCHAL_IVPN_SIMD_WIDTH);
        MORPH_LV2NX8_IP(dvecCoeffData11, pdvecCoeffData1, 4 * XCHAL_IVPN_SIMD_WIDTH);
        dvecCoeffData22 = MORPH_LV2NX8_I(pdvecCoeffData2, 2 * XCHAL_IVPN_SIMD_WIDTH);
        MORPH_LV2NX8_IP(dvecCoeffData21, pdvecCoeffData2, 4 * XCHAL_IVPN_SIMD_WIDTH);
        dvecCoeffData32 = MORPH_LV2NX8_I(pdvecCoeffData3, 2 * XCHAL_IVPN_SIMD_WIDTH);
        MORPH_LV2NX8_IP(dvecCoeffData31, pdvecCoeffData3, 4 * XCHAL_IVPN_SIMD_WIDTH);
        dvecCoeffData42 = MORPH_LV2NX8_I(pdvecCoeffData4, 2 * XCHAL_IVPN_SIMD_WIDTH);
        MORPH_LV2NX8_IP(dvecCoeffData41, pdvecCoeffData4, 4 * XCHAL_IVPN_SIMD_WIDTH);
        /* Pair Multiply and Accumulates */
        MORPH_MULPA2NX8(daccSum1, dvecInData1, dvecCoeffData11, dvecInData2, dvecCoeffData12);
        MORPH_MULPA2NX8(daccSum2, dvecInData1, dvecCoeffData21, dvecInData2, dvecCoeffData22);
        MORPH_MULPA2NX8(daccSum3, dvecInData1, dvecCoeffData31, dvecInData2, dvecCoeffData32);
        MORPH_MULPA2NX8(daccSum4, dvecInData1, dvecCoeffData41, dvecInData2, dvecCoeffData42);
        MORPH_MULPA2N8XR16_COEFF(daccSum1, dvecCoeffData12, dvecCoeffData11, zeroPInOffset);
        MORPH_MULPA2N8XR16_COEFF(daccSum2, dvecCoeffData22, dvecCoeffData21, zeroPInOffset);
        MORPH_MULPA2N8XR16_COEFF(daccSum3, dvecCoeffData32, dvecCoeffData31, zeroPInOffset);
        MORPH_MULPA2N8XR16_COEFF(daccSum4, dvecCoeffData42, dvecCoeffData41, zeroPInOffset);
      }
      /* Handling of corner cases */
      if (k < dotprodLength)
      {
        /* Input Data Load */
        MORPH_IDT_2Nx8 dvecInData; MORPH_OP_LOAD_2Nx8_IP(dvecInData, vaInData, pdvecInData);
        /* Coefficient Load */
        MORPH_IDT_2Nx8_COEFF dvecCoeffData1, dvecCoeffData2, dvecCoeffData3, dvecCoeffData4;
        MORPH_LV2NX8_IP(dvecCoeffData1, pdvecCoeffData1, 2 * XCHAL_IVPN_SIMD_WIDTH);
        MORPH_LV2NX8_IP(dvecCoeffData2, pdvecCoeffData2, 2 * XCHAL_IVPN_SIMD_WIDTH);
        MORPH_LV2NX8_IP(dvecCoeffData3, pdvecCoeffData3, 2 * XCHAL_IVPN_SIMD_WIDTH);
        MORPH_LV2NX8_IP(dvecCoeffData4, pdvecCoeffData4, 2 * XCHAL_IVPN_SIMD_WIDTH);
        /* Pair Multiply and Accumulates */
        MORPH_MULA2NX8(daccSum1, dvecInData, dvecCoeffData1);
        MORPH_MULA2NX8(daccSum2, dvecInData, dvecCoeffData2);
        MORPH_MULA2NX8(daccSum3, dvecInData, dvecCoeffData3);
        MORPH_MULA2NX8(daccSum4, dvecInData, dvecCoeffData4);
        MORPH_MULA2N8XR16(daccSum1, dvecCoeffData1, zeroPInS);
        MORPH_MULA2N8XR16(daccSum2, dvecCoeffData2, zeroPInS);
        MORPH_MULA2N8XR16(daccSum3, dvecCoeffData3, zeroPInS);
        MORPH_MULA2N8XR16(daccSum4, dvecCoeffData4, zeroPInS);
      }
      /* Reduction Addition and Bias Addition */
      xb_vecN_2x32v hvecSum1  = IVP_ADDN_2X32(IVP_CVT32S2NX24LL(daccSum1), \
                                IVP_CVT32S2NX24LH(daccSum1));
      hvecSum1 = IVP_ADDN_2X32(hvecSum1, IVP_CVT32S2NX24HL(daccSum1));
      hvecSum1 = IVP_ADDN_2X32(hvecSum1, IVP_CVT32S2NX24HH(daccSum1));
      int32_t sum1 = IVP_RADDN_2X32(hvecSum1);
      /* Reduction Addition and Bias Addition */
      xb_vecN_2x32v hvecSum2 = IVP_ADDN_2X32(IVP_CVT32S2NX24LL(daccSum2), \
                                             IVP_CVT32S2NX24LH(daccSum2));
      hvecSum2 = IVP_ADDN_2X32(hvecSum2, IVP_CVT32S2NX24HL(daccSum2));
      hvecSum2 = IVP_ADDN_2X32(hvecSum2, IVP_CVT32S2NX24HH(daccSum2));
      int32_t sum2 = IVP_RADDN_2X32(hvecSum2);
      /* Reduction Addition and Bias Addition */
      xb_vecN_2x32v hvecSum3 = IVP_ADDN_2X32(IVP_CVT32S2NX24LL(daccSum3), \
                                             IVP_CVT32S2NX24LH(daccSum3));
      hvecSum3 = IVP_ADDN_2X32(hvecSum3, IVP_CVT32S2NX24HL(daccSum3));
      hvecSum3 = IVP_ADDN_2X32(hvecSum3, IVP_CVT32S2NX24HH(daccSum3));
      int32_t sum3 = IVP_RADDN_2X32(hvecSum3);
      /* Reduction Addition and Bias Addition */
      xb_vecN_2x32v hvecSum4 = IVP_ADDN_2X32(IVP_CVT32S2NX24LL(daccSum4), \
                                             IVP_CVT32S2NX24LH(daccSum4));
      hvecSum4 = IVP_ADDN_2X32(hvecSum4, IVP_CVT32S2NX24HL(daccSum4));
      hvecSum4 = IVP_ADDN_2X32(hvecSum4, IVP_CVT32S2NX24HH(daccSum4));
      int32_t sum4 = IVP_RADDN_2X32(hvecSum4);
      /* Moving all the scalar sums to a 32-bit vector */
      xb_vecN_2x32v hvecOut = 0;
      hvecOut = IVP_MOVN_2X32T((xb_vecN_2x32v)sum4, hvecOut, IVP_LTRN_2I(4));
      hvecOut = IVP_MOVN_2X32T((xb_vecN_2x32v)sum3, hvecOut, IVP_LTRN_2I(3));
      hvecOut = IVP_MOVN_2X32T((xb_vecN_2x32v)sum2, hvecOut, IVP_LTRN_2I(2));
      hvecOut = IVP_MOVN_2X32T((xb_vecN_2x32v)sum1, hvecOut, IVP_LTRN_2I(1));
      /* add Bias term */
      hvecOut = IVP_ADDN_2X32(hvecOut, hvecBias);
      /* Scale, Shift, add output zeropoint and Clamp the accumulator output */
      MORPH_IDT_Nx16 vecOutData;
      SCALE_SHIFT_ADDZP_CLAMP_LIMITS_FC(vecOutData, hvecOut, quantScale, leftShift,rightShift, zeroPtOutput, minLim, maxLim);
      /* Store the output values */
      MORPH_SAVNX8_XP(vecOutData, vaOutData, pvecOut, 4);
      MORPH_SAPOSNX8_FP(vaOutData, pvecOut);
    }
    /* Loop for remaining Outputs */
    for (; outN < numOut; outN++) // Corner case for (k = 0; k < dotprodLength - 4 * XCHAL_IVPN_SIMD_WIDTH + 1; k += 4 * XCHAL_IVPN_SIMD_WIDTH)
    {
      /* Load bias to the first element in accumulators */
      xb_vecN_2x32v hvecBias;
      phvecBias = (xb_vecN_2x32v *)(pBiasData + outN);
      valign vaBias = IVP_LAN_2X32_PP(phvecBias);
      IVP_LAVN_2X32_XP(hvecBias, vaBias, phvecBias, 4);
      /* Add fixup term with bias value */
      IVP_ADDN_2X32T(hvecBias, hvecFixup, hvecBias, IVP_LTRN_2I(1));
      /* Initialize accumulators */
      xb_vec2Nx24 daccSum = 0;
      /* Input Data and Coefficient Pointers */
      pdvecInData     = (MORPH_IDT_2Nx8 *)pInData;
      pdvecCoeffData1 = (MORPH_IDT_2Nx8_COEFF *)(pCoeffData + outN * coeffPitch3);
      /* Priming for Input Data */
      valign vaInData = MORPH_OP_PRIME_2Nx8(pdvecInData);
      for (k = 0; k <= (dotprodLength - vectorizationWidth4x); k += vectorizationWidth4x)
      { /* (inDim1 * inDim2 * inDim3) Loop */
        /* Input Data Load */
        MORPH_IDT_2Nx8 dvecInData1; MORPH_OP_LOAD_2Nx8_IP(dvecInData1, vaInData, pdvecInData);
        MORPH_IDT_2Nx8 dvecInData2; MORPH_OP_LOAD_2Nx8_IP(dvecInData2, vaInData, pdvecInData);
        /* Coefficient Data Load */
        MORPH_IDT_2Nx8_COEFF dvecCoeffData1, dvecCoeffData2;
        dvecCoeffData2 = MORPH_LV2NX8_I(pdvecCoeffData1, 2 * XCHAL_IVPN_SIMD_WIDTH);
        MORPH_LV2NX8_IP(dvecCoeffData1, pdvecCoeffData1, 4 * XCHAL_IVPN_SIMD_WIDTH);
        /* Pair Multiply and Accumulates */
        MORPH_MULPA2NX8(daccSum, dvecInData1, dvecCoeffData1, dvecInData2, dvecCoeffData2);
        MORPH_MULPA2N8XR16_COEFF(daccSum, dvecCoeffData2, dvecCoeffData1, zeroPInOffset);
      }
      if (k < dotprodLength)
      {
        /* Input Data Load */
        MORPH_IDT_2Nx8 dvecInData; MORPH_OP_LOAD_2Nx8_IP(dvecInData, vaInData, pdvecInData);
        /* Coefficient Load */
        MORPH_IDT_2Nx8_COEFF dvecCoeffData;
        MORPH_LV2NX8_IP(dvecCoeffData, pdvecCoeffData1, 2 * XCHAL_IVPN_SIMD_WIDTH);
        /* Pair Multiply and Accumulates */
        MORPH_MULA2NX8(daccSum, dvecInData, dvecCoeffData);
        MORPH_MULA2N8XR16(daccSum, dvecCoeffData, zeroPInS);
      }
      /* Reduction Addition and Bias Addition */
      xb_vecN_2x32v hvecSum = IVP_ADDN_2X32(IVP_CVT32S2NX24LL(daccSum), \
                                             IVP_CVT32S2NX24LH(daccSum));
      hvecSum = IVP_ADDN_2X32(hvecSum, IVP_CVT32S2NX24HL(daccSum));
      hvecSum = IVP_ADDN_2X32(hvecSum, IVP_CVT32S2NX24HH(daccSum));
      int32_t sum = IVP_RADDN_2X32(hvecSum);
      /* Moving all the scalar sums to a 32-bit vector */
      xb_vecN_2x32v hvecOut = 0;
      hvecOut = IVP_MOVN_2X32T((xb_vecN_2x32v)sum, hvecOut, IVP_LTRN_2I(1));
      /* add Bias term */
      hvecOut = IVP_ADDN_2X32(hvecOut, hvecBias);
      /* Scale, Shift, add output zeropoint and Clamp the accumulator output */
      MORPH_IDT_Nx16 vecOutData;
      SCALE_SHIFT_ADDZP_CLAMP_LIMITS_FC(vecOutData, hvecOut, quantScale, leftShift, rightShift, zeroPtOutput, minLim, maxLim);
      /* Store the output values */
      MORPH_SAVNX8_XP(vecOutData, vaOutData, pvecOut, 1);
      MORPH_SAPOSNX8_FP(vaOutData, pvecOut);
    }
  }
  else // else part of if(XI_TILE3D_IS_PTR_ALIGNED_2NX8(coeffTile))
  {
    /* The loop gives 2 outputs per iteration */
    for (outN = 0; outN < numOut - 1; outN += 2)
    {
      /* Load bias to the first element in accumulators */
      xb_vecN_2x32v hvecBias;
      phvecBias = (xb_vecN_2x32v *)(pBiasData + outN);
      valign vaBias = IVP_LAN_2X32_PP(phvecBias);
      IVP_LAVN_2X32_XP(hvecBias, vaBias, phvecBias, 4 * 2);
      /* Add fixup term with bias value */
      IVP_ADDN_2X32T(hvecBias, hvecFixup, hvecBias, IVP_LTRN_2I(2));
      /* Initialize accumulators */
      xb_vec2Nx24 daccSum1 = 0;
      xb_vec2Nx24 daccSum2 = 0;

      /* Input Data and Coefficient Pointers */
      pdvecInData     = (MORPH_IDT_2Nx8 *) pInData;
      pdvecCoeffData1 = (MORPH_IDT_2Nx8_COEFF *)(pCoeffData + outN * coeffPitch3);
      pdvecCoeffData2 = (MORPH_IDT_2Nx8_COEFF *)(pCoeffData + (outN + 1) * coeffPitch3);

      /* Priming for Input Data and Coefficient Data */
      valign vaInData     = MORPH_OP_PRIME_2Nx8(pdvecInData);
      valign vaCoeffData1 = MORPH_OP_PRIME_2Nx8_COEFF(pdvecCoeffData1);
      valign vaCoeffData2 = MORPH_OP_PRIME_2Nx8_COEFF(pdvecCoeffData2);

      /* Multiplying and Accumulating 64 bytes at a time */
      for (k = 0; k <= (dotprodLength - vectorizationWidth4x); k += vectorizationWidth4x)
      { /* (inDim1 * inDim2 * inDim3) Loop */
        /* Input Data Load */
        MORPH_IDT_2Nx8 dvecInData1; MORPH_OP_LOAD_2Nx8_IP(dvecInData1, vaInData, pdvecInData);
        MORPH_IDT_2Nx8 dvecInData2; MORPH_OP_LOAD_2Nx8_IP(dvecInData2, vaInData, pdvecInData);
        /* Coefficient Data Load */
        MORPH_IDT_2Nx8_COEFF dvecCoeffData11, dvecCoeffData12;
        MORPH_OP_LOAD_2Nx8_IP_COEFF(dvecCoeffData11, vaCoeffData1, pdvecCoeffData1);
        MORPH_OP_LOAD_2Nx8_IP_COEFF(dvecCoeffData12, vaCoeffData1, pdvecCoeffData1);
        MORPH_IDT_2Nx8_COEFF dvecCoeffData21, dvecCoeffData22;
        MORPH_OP_LOAD_2Nx8_IP_COEFF(dvecCoeffData21, vaCoeffData2, pdvecCoeffData2);
        MORPH_OP_LOAD_2Nx8_IP_COEFF(dvecCoeffData22, vaCoeffData2, pdvecCoeffData2);
        /* Pair Multiply and Accumulates */
        MORPH_MULPA2NX8(daccSum1, dvecInData1, dvecCoeffData11, dvecInData2, dvecCoeffData12);
        MORPH_MULPA2NX8(daccSum2, dvecInData1, dvecCoeffData21, dvecInData2, dvecCoeffData22);
        MORPH_MULPA2N8XR16_COEFF(daccSum1, dvecCoeffData12, dvecCoeffData11, zeroPInOffset);
        MORPH_MULPA2N8XR16_COEFF(daccSum2, dvecCoeffData22, dvecCoeffData21, zeroPInOffset);
      }
      if (k < dotprodLength )
      {
        /* Corner case handling if dotprodLength is not a multiple of 64 */
        int32_t remLength = dotprodLength - k;
        /* Input Data Load */
        MORPH_IDT_2Nx8 dvecInData1, dvecInData2;
        MORPH_OP_LOAD_2Nx8_VAR(dvecInData1, vaInData, pdvecInData, remLength);
        MORPH_OP_LOAD_2Nx8_VAR(dvecInData2, vaInData, pdvecInData, remLength - 64);
        /* Coefficient Data Load */
        MORPH_IDT_2Nx8_COEFF dvecCoeffData11, dvecCoeffData12;
        MORPH_OP_LOAD_2Nx8_VAR_COEFF(dvecCoeffData11, vaCoeffData1, pdvecCoeffData1, remLength);
        MORPH_OP_LOAD_2Nx8_VAR_COEFF(dvecCoeffData12, vaCoeffData1, pdvecCoeffData1, remLength - 64);
        MORPH_IDT_2Nx8_COEFF dvecCoeffData21, dvecCoeffData22;
        MORPH_OP_LOAD_2Nx8_VAR_COEFF(dvecCoeffData21, vaCoeffData2, pdvecCoeffData2, remLength);
        MORPH_OP_LOAD_2Nx8_VAR_COEFF(dvecCoeffData22, vaCoeffData2, pdvecCoeffData2, remLength - 64);
        /* Pair Multiply and Accumulates */
        MORPH_MULPA2NX8(daccSum1, dvecInData1, dvecCoeffData11, dvecInData2, dvecCoeffData12);
        MORPH_MULPA2NX8(daccSum2, dvecInData1, dvecCoeffData21, dvecInData2, dvecCoeffData22);
        MORPH_MULPA2N8XR16_COEFF(daccSum1, dvecCoeffData12, dvecCoeffData11, zeroPInOffset);
        MORPH_MULPA2N8XR16_COEFF(daccSum2, dvecCoeffData22, dvecCoeffData21, zeroPInOffset);
      }
      /* Reduction Addition and Bias Addition */
     // xb_vecN_2x32v hvecSum1 = IVP_ADDN_2X32(IVP_CVT32S2NX24LL(daccSum1), \
     //                                        IVP_CVT32S2NX24LH(daccSum1));
      xb_vecN_2x32v hvecSum1 = IVP_CVT32S2NX24LL(daccSum1);
      xb_vecN_2x32v hvecSumT = IVP_CVT32S2NX24LH(daccSum1);
      hvecSum1 = IVP_ADDN_2X32(hvecSum1, hvecSumT);

      hvecSum1 = IVP_ADDN_2X32(hvecSum1, IVP_CVT32S2NX24HL(daccSum1));
      hvecSum1 = IVP_ADDN_2X32(hvecSum1, IVP_CVT32S2NX24HH(daccSum1));
      int32_t sum1           = IVP_RADDN_2X32(hvecSum1);
      /* Reduction Addition and Bias Addition */
      xb_vecN_2x32v hvecSum2 = IVP_ADDN_2X32(IVP_CVT32S2NX24LL(daccSum2), \
                                             IVP_CVT32S2NX24LH(daccSum2));
      hvecSum2 = IVP_ADDN_2X32(hvecSum2, IVP_CVT32S2NX24HL(daccSum2));
      hvecSum2 = IVP_ADDN_2X32(hvecSum2, IVP_CVT32S2NX24HH(daccSum2));
      int32_t sum2            = IVP_RADDN_2X32(hvecSum2);
      /* Moving all the scalar sums to a 32-bit vector */
      xb_vecN_2x32v hvecOut = 0;
      hvecOut = IVP_MOVN_2X32T((xb_vecN_2x32v)sum2, hvecOut, IVP_LTRN_2I(2));
      hvecOut = IVP_MOVN_2X32T((xb_vecN_2x32v)sum1, hvecOut, IVP_LTRN_2I(1));
      /* add Bias term */
      hvecOut = IVP_ADDN_2X32(hvecOut, hvecBias);
      /* Scale, Shift, Add output zeropoint and Clamp the accumulator output */
      MORPH_IDT_Nx16 vecOutData;
      SCALE_SHIFT_ADDZP_CLAMP_LIMITS_FC(vecOutData, hvecOut, quantScale, leftShift, rightShift, zeroPtOutput, minLim, maxLim);
      /* Store the output values */
      MORPH_SAVNX8_XP(vecOutData, vaOutData, pvecOut, 2);
      MORPH_SAPOSNX8_FP(vaOutData, pvecOut);
    }
    if (outN < numOut) // Corner case of for (k = 0; k < dotprodLength - 2 * XCHAL_IVPN_SIMD_WIDTH + 1; k += 2 * XCHAL_IVPN_SIMD_WIDTH)
    {
      /* Load bias to the first element in accumulators */
      xb_vecN_2x32v hvecBias;
      phvecBias = (xb_vecN_2x32v *)(pBiasData + outN);
      valign vaBias = IVP_LAN_2X32_PP(phvecBias);
      IVP_LAVN_2X32_XP(hvecBias, vaBias, phvecBias, 4);
      /* Add fixup term with bias value */
      IVP_ADDN_2X32T(hvecBias, hvecFixup, hvecBias, IVP_LTRN_2I(1));
      /* Initialize accumulators */
      xb_vec2Nx24 daccSum = 0;
      /* Input Data and Coefficient Pointers */
      pdvecInData     = (MORPH_IDT_2Nx8 *) pInData;
      pdvecCoeffData1 = (MORPH_IDT_2Nx8_COEFF *)(pCoeffData + outN * coeffPitch3);
      /* Priming for Input Data and Coefficient Data */
      valign vaInData    = MORPH_OP_PRIME_2Nx8(pdvecInData);
      valign vaCoeffData = MORPH_OP_PRIME_2Nx8_COEFF(pdvecCoeffData1);
      /* Multiplying and Accumulating 128 bytes at a time */
      for (k = 0; k <= (dotprodLength - vectorizationWidth4x); k += vectorizationWidth4x)
      { /* (inDim1 * inDim2 * inDim3) Loop */
        /* Input Data Load */
        MORPH_IDT_2Nx8 dvecInData1; MORPH_OP_LOAD_2Nx8_IP(dvecInData1, vaInData, pdvecInData);
        MORPH_IDT_2Nx8 dvecInData2; MORPH_OP_LOAD_2Nx8_IP(dvecInData2, vaInData, pdvecInData);
        /* Coefficient Data Load */
        MORPH_IDT_2Nx8_COEFF dvecCoeffData1; MORPH_OP_LOAD_2Nx8_IP_COEFF(dvecCoeffData1, vaCoeffData, pdvecCoeffData1);
        MORPH_IDT_2Nx8_COEFF dvecCoeffData2; MORPH_OP_LOAD_2Nx8_IP_COEFF(dvecCoeffData2, vaCoeffData, pdvecCoeffData1);
        /* Pair Multiply and Accumulates */
        MORPH_MULPA2NX8(daccSum, dvecInData1, dvecCoeffData1, dvecInData2, dvecCoeffData2);
        MORPH_MULPA2N8XR16_COEFF(daccSum, dvecCoeffData2, dvecCoeffData1, zeroPInOffset);
      }
      if (k < dotprodLength)
      {
        /* Corner case handling if dotprodLength is not a multiple of 64 */
        int32_t remLength = dotprodLength - k;
        /* Input Data Load */
        MORPH_IDT_2Nx8 dvecInData1; MORPH_OP_LOAD_2Nx8_VAR(dvecInData1, vaInData, pdvecInData, remLength);
        MORPH_IDT_2Nx8 dvecInData2; MORPH_OP_LOAD_2Nx8_VAR(dvecInData2, vaInData, pdvecInData, (remLength - 64));
        /* Coefficient Data Load */
        MORPH_IDT_2Nx8_COEFF dvecCoeffData1, dvecCoeffData2;
        MORPH_OP_LOAD_2Nx8_VAR_COEFF(dvecCoeffData1, vaCoeffData, pdvecCoeffData1, remLength);
        MORPH_OP_LOAD_2Nx8_VAR_COEFF(dvecCoeffData2, vaCoeffData, pdvecCoeffData1, (remLength - 64));
        /* Pair Multiply and Accumulates */
        MORPH_MULPA2NX8(daccSum, dvecInData1, dvecCoeffData1, dvecInData2, dvecCoeffData2);
        MORPH_MULPA2N8XR16_COEFF(daccSum, dvecCoeffData2, dvecCoeffData1, zeroPInOffset);
      }
      /* Reduction Addition and Bias Addition */
      xb_vecN_2x32v hvecSum =  IVP_ADDN_2X32(IVP_CVT32S2NX24LL(daccSum), \
                                             IVP_CVT32S2NX24LH(daccSum));
      hvecSum = IVP_ADDN_2X32(hvecSum, IVP_CVT32S2NX24HL(daccSum));
      hvecSum = IVP_ADDN_2X32(hvecSum, IVP_CVT32S2NX24HH(daccSum));
      int32_t sum           = IVP_RADDN_2X32(hvecSum);
      /* Moving all the scalar sums to a 32-bit vector */
      xb_vecN_2x32v hvecOut = 0;
      hvecOut = IVP_MOVN_2X32T((xb_vecN_2x32v)sum, hvecOut, IVP_LTRN_2I(1));
      /* add Bias term */
      hvecOut = IVP_ADDN_2X32(hvecOut, hvecBias);
      /* Scale, Shift, Add output zeropoint and Clamp the accumulator output */
      MORPH_IDT_Nx16 vecOutData;
      SCALE_SHIFT_ADDZP_CLAMP_LIMITS_FC(vecOutData, hvecOut, quantScale, leftShift, rightShift, zeroPtOutput, minLim, maxLim);
      /* Store the output values */
      MORPH_SAVNX8_XP(vecOutData, vaOutData, pvecOut, 1);
      MORPH_SAPOSNX8_FP(vaOutData, pvecOut);
    }
  }
}
/******* xiFullyConnectedA3D_S_U8 / xiFullyConnectedA3D_S_S8 ***********/
/* Description : P6 implementation for Fully Connected Layer for       */
/*               Android CNN                                           */
/* Inputs      : Input Data Tile, Coeff Data Tile, Bias Array,         */
/*               CNNA convolution params structure                     */
/* Outputs     : XI Error Code                                         */
/* InOuts      : Output Tile                                           */
/* Assumptions : InData, CoeffData and OutData are U8/S8               */
/*               Width, Height and Depth of coefficient tile are same  */
/*               as that of the input tile.                            */
/*               Input is in WHD or DWH format                         */
/*               Coeff is in WHDN or DWHN format                       */
/***********************************************************************/
XI_ERR_TYPE MAKE_NAME(xiFullyConnectedA3D_S)(const xi_pTile3D inTile,
                                             const xi_pTile4D coeffTile,
                                             const xi_pArray biasArray,
                                             xi_pTile3D outTile,
                                             const xi_cnna_conv_params *param)
{
  /* Error Checks */
  XI_ERROR_CHECKS()
  {
    MORPH_TILE3D_CHECK(inTile);
    MORPH_TILE3D_CHECK(outTile);
    MORPH_TILE4D_CHECK(coeffTile);
    XI_CHECK_ARRAY_S32(biasArray);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_TILE4D_IN_DRAM_BOUNDARY(coeffTile);
    XI_CHECK_POINTER(param);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(inTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(coeffTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(biasArray, outTile);
    if (XI_TILE3D_GET_DATA_ORDER(outTile) == XI_WHD)
    {
      XI_CHECK_ERROR(XI_TILE4D_GET_DIM4(coeffTile) == XI_TILE3D_GET_DIM3(outTile), XI_ERR_DATASIZE,\
                     "\nNumber of Outputs = %d, number of Kernels = %d\nNumber of Outputs should be equal to the number of Kernels", \
                     XI_TILE3D_GET_DIM3(outTile), XI_TILE4D_GET_DIM4(coeffTile));

      XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(biasArray) >= XI_TILE4D_GET_DIM4(coeffTile), XI_ERR_DATASIZE,\
                     "\nWidth of Bias Array = %d,number of Kernels = %d\nWidth of Bias Array should be >= number of Kernels", \
                     XI_ARRAY_GET_WIDTH(biasArray), XI_TILE4D_GET_DIM4(coeffTile));
    }
    else if (XI_TILE3D_GET_DATA_ORDER(outTile) == XI_DWH)
    {
      XI_CHECK_ERROR(XI_TILE4D_GET_DIM4(coeffTile) == XI_TILE3D_GET_DIM1(outTile), XI_ERR_DATASIZE,\
                     "\nNumber of Outputs = %d, number of Kernels = %d\nNumber of Outputs should be equal to the number of Kernels", \
                     XI_TILE3D_GET_DIM1(outTile), XI_TILE4D_GET_DIM4(coeffTile));

      XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(biasArray) >= XI_TILE4D_GET_DIM4(coeffTile), XI_ERR_DATASIZE,\
                     "\nWidth of Bias Array = %d,number of Kernels = %d\nWidth of Bias Array should be >= number of Kernels", \
                     XI_ARRAY_GET_WIDTH(biasArray), XI_TILE4D_GET_DIM4(coeffTile));
    }
    XI_CHECK_ERROR(((XI_TILE3D_GET_DIM1_PITCH(inTile) == XI_TILE3D_GET_DIM1(inTile)) && \
                   (XI_TILE3D_GET_DIM2_PITCH(inTile) == XI_TILE3D_GET_DIM1(inTile) * XI_TILE3D_GET_DIM2(inTile))), XI_ERR_DATASIZE, \
                   "\nInput tile dim1 size = %d, dim1 pitch = %d, dim2 size = %d, dim2 pitch = %d\nData Pitch not equal to width for input tile", \
                   XI_TILE3D_GET_DIM1(inTile), XI_TILE3D_GET_DIM1_PITCH(inTile), XI_TILE3D_GET_DIM2(inTile), XI_TILE3D_GET_DIM2_PITCH(inTile));

    XI_CHECK_ERROR(((XI_TILE3D_GET_DIM1(inTile) * XI_TILE3D_GET_DIM2(inTile) * XI_TILE3D_GET_DIM3(inTile)) == \
                   (XI_TILE4D_GET_DIM1(coeffTile) * XI_TILE4D_GET_DIM2(coeffTile) * XI_TILE4D_GET_DIM3(coeffTile))), XI_ERR_DATASIZE, \
                   "\ninTile dim1 = %d, dim2 = %d, dim3 = %d\ncoeffTile dim1 = %d, dim2 = %d, dim3 = %d\ninTile Volume and kernel Volume should be equal.", \
                   XI_TILE3D_GET_DIM1(inTile), XI_TILE3D_GET_DIM2(inTile), XI_TILE3D_GET_DIM3(inTile), XI_TILE4D_GET_DIM1(coeffTile), XI_TILE4D_GET_DIM2(coeffTile), XI_TILE4D_GET_DIM3(coeffTile));

    XI_CHECK_ERROR(((XI_TILE4D_GET_DIM1_PITCH(coeffTile) == XI_TILE4D_GET_DIM1(coeffTile)) && (XI_TILE4D_GET_DIM2_PITCH(coeffTile) == \
                   XI_TILE4D_GET_DIM1_PITCH(coeffTile) * XI_TILE4D_GET_DIM2(coeffTile)) && (XI_TILE4D_GET_DIM3_PITCH(coeffTile) == \
                   XI_TILE4D_GET_DIM2_PITCH(coeffTile) * XI_TILE4D_GET_DIM3(coeffTile))), XI_ERR_DATASIZE,\
                   "\nCoefficient tile dim1 size = %d, dim1 pitch = %d, dim2 size = %d, dim2 pitch = %d\nData Pitch not equal to width for coefficient tile", \
                   XI_TILE3D_GET_DIM1(coeffTile), XI_TILE3D_GET_DIM1_PITCH(coeffTile), XI_TILE3D_GET_DIM2(coeffTile), XI_TILE3D_GET_DIM2_PITCH(coeffTile));

    if (XI_CNNA_CONV_GET_FLAG_RELU(param))
    {
      XI_CHECK_ERROR(XI_CNNA_CONV_GET_RELUMIN(param) <= XI_CNNA_CONV_GET_RELUMAX(param),
                     XI_ERR_BADARG, "\nRelu min = %d, Relu max = %d\nRelu max should be less than or equal to Relu min", \
                     XI_CNNA_CONV_GET_RELUMIN(param), XI_CNNA_CONV_GET_RELUMAX(param));
      XI_CHECK_ERROR((XI_CNNA_CONV_GET_RELUMIN(param) >= MORPH_MIN_VALUE),
                     XI_ERR_BADARG, "\nRelu min = %d, Relu min value should be >= %d", XI_CNNA_CONV_GET_RELUMIN(param), MORPH_MIN_VALUE);
      XI_CHECK_ERROR((XI_CNNA_CONV_GET_RELUMAX(param) <= MORPH_MAX_VALUE),
                     XI_ERR_BADARG, "\nRelu max = %d, Relu max value should be <= %d", XI_CNNA_CONV_GET_RELUMAX(param), MORPH_MAX_VALUE);
    }
    XI_CHECK_ERROR(((XI_TILE3D_GET_DATA_ORDER(inTile) == XI_WHD) && (XI_TILE4D_GET_DATA_ORDER(coeffTile) == XI_WHDN) && \
                   (XI_TILE3D_GET_DATA_ORDER(outTile)== XI_WHD)) || ((XI_TILE3D_GET_DATA_ORDER(inTile) == XI_DWH) && \
                   (XI_TILE4D_GET_DATA_ORDER(coeffTile) == XI_DWHN) && (XI_TILE3D_GET_DATA_ORDER(outTile) == XI_DWH)), XI_ERR_BADARG,\
                   "\nData Order of the (input tile, coefficient tile, output tile) = (%d, %d, %d)\nData Order of the (input tile, coefficient tile, output tile) \
                   should be either (XI_WHD, XI_WHDN, XI_WHD) or (XI_DWH, XI_DWHN, XI_DWH)", XI_TILE3D_GET_DATA_ORDER(inTile), XI_TILE3D_GET_DATA_ORDER(coeffTile), XI_TILE3D_GET_DATA_ORDER(outTile));
    XI_CHECK_ERROR(((XI_CNNA_CONV_GET_OUTPUT_SHIFT(param) < 32) && \
                   (XI_CNNA_CONV_GET_OUTPUT_SHIFT(param) > -32)), XI_ERR_NORM, \
                   "\nOutput shift value = %d, The output shift value is should be between -32 and 32 ", XI_CNNA_CONV_GET_OUTPUT_SHIFT(param));
    XI_CHECK_ERROR(((XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param) >= MORPH_MIN_VALUE) && \
                   (XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param) <= MORPH_MAX_VALUE)), XI_ERR_BADARG,
                   "\nOutput zero point = %d, Zero point output value should be between %d to %d", XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param), MORPH_MIN_VALUE, MORPH_MAX_VALUE);
    XI_CHECK_ERROR(((XI_CNNA_CONV_GET_ZEROPT_INPUT(param) >= MORPH_MIN_VALUE) && \
                   (XI_CNNA_CONV_GET_ZEROPT_INPUT(param) <= MORPH_MAX_VALUE)), XI_ERR_BADARG,
                   "\nInput zero point = %d, Zero Point input value should be between %d to %d", XI_CNNA_CONV_GET_ZEROPT_INPUT(param), MORPH_MIN_VALUE, MORPH_MAX_VALUE);
    XI_CHECK_ERROR(((XI_CNNA_CONV_GET_ZEROPT_COEFF(param) >= MORPH_MIN_VALUE_COEFF) && \
                   (XI_CNNA_CONV_GET_ZEROPT_COEFF(param) <= MORPH_MAX_VALUE_COEFF)), XI_ERR_BADARG,
                   "\nZero Point coefficient = %d, Zero Point coefficient should be between %d to %d", XI_CNNA_CONV_GET_ZEROPT_COEFF(param), MORPH_MIN_VALUE_COEFF, MORPH_MAX_VALUE_COEFF);
  }
  /* CNNA convolution parameters */
  const int32_t quantScale   = XI_CNNA_CONV_GET_QUANT_SCALE(param);
  const int32_t outShift     = XI_CNNA_CONV_GET_OUTPUT_SHIFT(param);
  const int16_t zeroPtInput  = XI_CNNA_CONV_GET_ZEROPT_INPUT(param);
  const int16_t zeroPtCoeff  = XI_CNNA_CONV_GET_ZEROPT_COEFF(param);
  const int16_t zeroPtOutput = XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param);
  const uint8_t enableReLu   = XI_CNNA_CONV_GET_FLAG_RELU(param);
  const int16_t reluMin      = XI_CNNA_CONV_GET_RELUMIN(param);
  const int16_t reluMax      = XI_CNNA_CONV_GET_RELUMAX(param);
  /* Setting the limits for output data according to ReLu is enabled or not*/
  const int16_t minLim = enableReLu ? reluMin : MORPH_MIN_VALUE;
  const int16_t maxLim = enableReLu ? reluMax : MORPH_MAX_VALUE;
  /* Left and right shift value */
  int32_t leftShift  = outShift < 0 ? -outShift : 0;
  int32_t rightShift = outShift < 0 ? 0 : outShift;
  /* If quantization scale = 0 , output tile is filled with
   * CLAMP(zeroPtOutput, minLim, maxLim)
   */
  int16_t tileFillValue;
  if (quantScale == 0)
  {
    tileFillValue = ((zeroPtOutput <  minLim) ? minLim : (zeroPtOutput > maxLim) ? maxLim : zeroPtOutput);
    return(xiFillTile3D_I8(outTile, tileFillValue, 0));
  }

  /* Consider the total size of input (dim1 * dim2* dim3) is N. In QM24 implementaion, we are calculating  */
  /* (input x coeff - inzp x coeff) within the inner most loop. In U8 case, the maximum sum per vector     */
  /* lane will obtain when the Input zero point is 0 and all the input and coefficient values are 255.     */
  /* The maximum sum per vector lane is = (255*255*N/64). This value should not cross beyond S24 limit.    */
  /* So the maximum tile size possible in U8 case is 8256.                                                 */
  /* In S8 case, if Input zero point is 127 and all the input and coefficient values are -128, the maximum */
  /* sum per vector lane is (128*128 + 127*128)*N/64. The maximum tile size possible in S8 case is 16448.  */

  if ((XI_TILE3D_GET_DIM1(inTile)* XI_TILE3D_GET_DIM2(inTile) *XI_TILE3D_GET_DIM3(inTile)) <= MORPH_MAX_SIZE)
  {
    if (((XI_TILE3D_GET_DIM1(inTile)*XI_TILE3D_GET_DIM2(inTile)*XI_TILE3D_GET_DIM3(inTile) ) == 32) && ((XI_TILE4D_GET_DIM4(coeffTile) ) >= 64))
    {
      MAKE_NAME(fullyConnectedA3D_S_QM24_MODGS)(inTile, coeffTile, biasArray, outTile, param);
    }
    else
    {
    MAKE_NAME(fullyConnectedA3D_S_QM24)(inTile, coeffTile, biasArray, outTile, param);
    }
    return(XI_ERROR_STATUS());
  }
  /* Getting parameters from the tile structures */
  const int32_t inDim1      = XI_TILE3D_GET_DIM1(inTile);
  const int32_t inDim2      = XI_TILE3D_GET_DIM2(inTile);
  const int32_t inDim3      = XI_TILE3D_GET_DIM3(inTile);
  const int32_t numOut      = XI_TILE4D_GET_DIM4(coeffTile);
  const int32_t coeffPitch3 = XI_TILE4D_GET_DIM3_PITCH(coeffTile);

  /* Data Pointers of input, output, coefficient and bias data */
  MORPH_IDT_SCALAR *pInData          = (MORPH_IDT_SCALAR *) XI_TILE3D_GET_DATA_PTR(inTile);
  MORPH_IDT_SCALAR *pOutData         = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(outTile);
  MORPH_IDT_SCALAR_COEFF *pCoeffData = (MORPH_IDT_SCALAR_COEFF *)XI_TILE4D_GET_DATA_PTR(coeffTile);
  int32_t *pBiasData                 = (int32_t *) XI_ARRAY_GET_DATA_PTR(biasArray);
  /* Loop Variables */
  int32_t outN, k;
  int32_t dotprodLength    = inDim1 * inDim2 * inDim3;
  /* Input and Output data pointers */
  MORPH_IDT_Nx8* restrict pvecInData;
  MORPH_IDT_Nx8_COEFF* restrict pvecCoeffData1;
  MORPH_IDT_Nx8_COEFF* restrict pvecCoeffData2;
  MORPH_IDT_Nx8_COEFF* restrict pvecCoeffData3;
  MORPH_IDT_Nx8_COEFF* restrict pvecCoeffData4;
  MORPH_IDT_Nx8* restrict pvecOut;
  xb_vec2Nx8* restrict pdvecBias;
  /* Initialize output data pointer */
  pvecOut = (MORPH_IDT_Nx8 *)(pOutData);
  valign vaOutData = IVP_ZALIGN();

  /* There are 2 implementations of Fully Connected Convolution.
   * First implementation is used, when the coeff Data pointer is aligned to SIMD width and
   * dot product length is a multiple of to SIMD width. In this implementation, all the accumulators.
   * are utilized thereby generating 4 outputs at a time. Here dotprodLength loop
   * is unrolled by 4 * XCHAL_IVPN_SIMD_WIDTH.
   * In the second implementation,2 outputs are generated at a time.
   * dotprodLength loop is unrolled by 2 * XCHAL_IVPN_SIMD_WIDTH
   */
  if (XI_TILE3D_IS_PTR_ALIGNED_NX8(coeffTile) && ((dotprodLength & (XCHAL_IVPN_SIMD_WIDTH -1)) == 0))
  {
    /* The loop gives 4 outputs per iteration */
    for (outN = 0; outN < numOut - 3; outN += 4)
    {
      int32_t remLength;
      /* Load bias to the first element in accumulators */
      xb_vec2Nx8 dvecBias;
      pdvecBias = (xb_vec2Nx8 *)((int8_t *)(pBiasData + outN));
      valign vaBias = IVP_LA2NX8_PP(pdvecBias);
      IVP_LAV2NX8_XP(dvecBias, vaBias, pdvecBias, 4);
      xb_vecNx48 accSum1 = IVP_CVT48SNX32(0, IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecBias)));
      IVP_LAV2NX8_XP(dvecBias, vaBias, pdvecBias, 4);
      xb_vecNx48 accSum2 = IVP_CVT48SNX32(0, IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecBias)));
      IVP_LAV2NX8_XP(dvecBias, vaBias, pdvecBias, 4);
      xb_vecNx48 accSum3 = IVP_CVT48SNX32(0, IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecBias)));
      IVP_LAV2NX8_XP(dvecBias, vaBias, pdvecBias, 4);
      xb_vecNx48 accSum4 = IVP_CVT48SNX32(0, IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecBias)));
      /* Input Data and Coefficient Pointers */
      pvecInData     = (MORPH_IDT_Nx8 *) pInData;
      pvecCoeffData1 = (MORPH_IDT_Nx8_COEFF *)(pCoeffData + outN * coeffPitch3);
      pvecCoeffData2 = (MORPH_IDT_Nx8_COEFF *)(pCoeffData + (outN + 1) * coeffPitch3);
      pvecCoeffData3 = (MORPH_IDT_Nx8_COEFF *)(pCoeffData + (outN + 2) * coeffPitch3);
      pvecCoeffData4 = (MORPH_IDT_Nx8_COEFF *)(pCoeffData + (outN + 3) * coeffPitch3);

      /* Priming for Input Data */
      valign vaInData = MORPH_OP_PRIME_NX8(pvecInData);

      /* Multiplying and Accumulating 128 bytes at a time */
      for (k = 0; k < dotprodLength - 4 * XCHAL_IVPN_SIMD_WIDTH + 1; k += 4 * XCHAL_IVPN_SIMD_WIDTH)
      { /* (inDim1 * inDim2 * inDim3) Loop */
        /* Input Data Load */
        MORPH_IDT_Nx16 vecInData1; MORPH_LOAD_Nx8_IP(vecInData1, vaInData, pvecInData);
        MORPH_IDT_Nx16 vecInData2; MORPH_LOAD_Nx8_IP(vecInData2, vaInData, pvecInData);
        MORPH_IDT_Nx16 vecInData3; MORPH_LOAD_Nx8_IP(vecInData3, vaInData, pvecInData);
        MORPH_IDT_Nx16 vecInData4; MORPH_LOAD_Nx8_IP(vecInData4, vaInData, pvecInData);

        /* Coefficient Data Load */
        MORPH_IDT_Nx16_COEFF vecCoeffData11, vecCoeffData12, vecCoeffData13, vecCoeffData14;
        MORPH_IDT_Nx16_COEFF vecCoeffData21, vecCoeffData22, vecCoeffData23, vecCoeffData24;
        MORPH_IDT_Nx16_COEFF vecCoeffData31, vecCoeffData32, vecCoeffData33, vecCoeffData34;
        MORPH_IDT_Nx16_COEFF vecCoeffData41, vecCoeffData42, vecCoeffData43, vecCoeffData44;

        vecCoeffData12 = MORPH_LVNX8_I(pvecCoeffData1, XCHAL_IVPN_SIMD_WIDTH);
        vecCoeffData13 = MORPH_LVNX8_I(pvecCoeffData1, 2 * XCHAL_IVPN_SIMD_WIDTH);
        vecCoeffData14 = MORPH_LVNX8_I(pvecCoeffData1, 3 * XCHAL_IVPN_SIMD_WIDTH);
        MORPH_LVNX8_XP(vecCoeffData11, pvecCoeffData1, 4 * XCHAL_IVPN_SIMD_WIDTH);
        vecCoeffData22 = MORPH_LVNX8_I(pvecCoeffData2, XCHAL_IVPN_SIMD_WIDTH);
        vecCoeffData23 = MORPH_LVNX8_I(pvecCoeffData2, 2 * XCHAL_IVPN_SIMD_WIDTH);
        vecCoeffData24 = MORPH_LVNX8_I(pvecCoeffData2, 3 * XCHAL_IVPN_SIMD_WIDTH);
        MORPH_LVNX8_XP(vecCoeffData21, pvecCoeffData2, 4 * XCHAL_IVPN_SIMD_WIDTH);
        vecCoeffData32 = MORPH_LVNX8_I(pvecCoeffData3, XCHAL_IVPN_SIMD_WIDTH);
        vecCoeffData33 = MORPH_LVNX8_I(pvecCoeffData3, 2 * XCHAL_IVPN_SIMD_WIDTH);
        vecCoeffData34 = MORPH_LVNX8_I(pvecCoeffData3, 3 * XCHAL_IVPN_SIMD_WIDTH);
        MORPH_LVNX8_XP(vecCoeffData31, pvecCoeffData3, 4 * XCHAL_IVPN_SIMD_WIDTH);
        vecCoeffData42 = MORPH_LVNX8_I(pvecCoeffData4, XCHAL_IVPN_SIMD_WIDTH);
        vecCoeffData43 = MORPH_LVNX8_I(pvecCoeffData4, 2 * XCHAL_IVPN_SIMD_WIDTH);
        vecCoeffData44 = MORPH_LVNX8_I(pvecCoeffData4, 3 * XCHAL_IVPN_SIMD_WIDTH);
        MORPH_LVNX8_XP(vecCoeffData41, pvecCoeffData4, 4 * XCHAL_IVPN_SIMD_WIDTH);

        /* Subtract corresponding zeropoint from input vector*/
        vecInData1= IVP_SUBNX16(vecInData1, zeroPtInput);
        vecInData2= IVP_SUBNX16(vecInData2, zeroPtInput);
        vecInData3= IVP_SUBNX16(vecInData3, zeroPtInput);
        vecInData4= IVP_SUBNX16(vecInData4, zeroPtInput);

        /* Subtract corresponding zeropoint from coeff vector */
        vecCoeffData11 = IVP_SUBNX16(vecCoeffData11, zeroPtCoeff);
        vecCoeffData12 = IVP_SUBNX16(vecCoeffData12, zeroPtCoeff);
        vecCoeffData13 = IVP_SUBNX16(vecCoeffData13, zeroPtCoeff);
        vecCoeffData14 = IVP_SUBNX16(vecCoeffData14, zeroPtCoeff);
        vecCoeffData21 = IVP_SUBNX16(vecCoeffData21, zeroPtCoeff);
        vecCoeffData22 = IVP_SUBNX16(vecCoeffData22, zeroPtCoeff);
        vecCoeffData23 = IVP_SUBNX16(vecCoeffData23, zeroPtCoeff);
        vecCoeffData24 = IVP_SUBNX16(vecCoeffData24, zeroPtCoeff);
        vecCoeffData31 = IVP_SUBNX16(vecCoeffData31, zeroPtCoeff);
        vecCoeffData32 = IVP_SUBNX16(vecCoeffData32, zeroPtCoeff);
        vecCoeffData33 = IVP_SUBNX16(vecCoeffData33, zeroPtCoeff);
        vecCoeffData34 = IVP_SUBNX16(vecCoeffData34, zeroPtCoeff);
        vecCoeffData41 = IVP_SUBNX16(vecCoeffData41, zeroPtCoeff);
        vecCoeffData42 = IVP_SUBNX16(vecCoeffData42, zeroPtCoeff);
        vecCoeffData43 = IVP_SUBNX16(vecCoeffData43, zeroPtCoeff);
        vecCoeffData44 = IVP_SUBNX16(vecCoeffData44, zeroPtCoeff);
        /* Pair Multiply and Accumulates */
        IVP_MULPANX16(accSum1, vecInData2, vecCoeffData12, vecInData1, vecCoeffData11);
        IVP_MULPANX16(accSum2, vecInData2, vecCoeffData22, vecInData1, vecCoeffData21);
        IVP_MULPANX16(accSum3, vecInData2, vecCoeffData32, vecInData1, vecCoeffData31);
        IVP_MULPANX16(accSum4, vecInData2, vecCoeffData42, vecInData1, vecCoeffData41);

        IVP_MULPANX16(accSum1, vecInData3, vecCoeffData13, vecInData4, vecCoeffData14);
        IVP_MULPANX16(accSum2, vecInData3, vecCoeffData23, vecInData4, vecCoeffData24);
        IVP_MULPANX16(accSum3, vecInData3, vecCoeffData33, vecInData4, vecCoeffData34);
        IVP_MULPANX16(accSum4, vecInData3, vecCoeffData43, vecInData4, vecCoeffData44);
      }
      /* Handling if remLength is zero, 32, 64 or 96 */
      for (remLength = dotprodLength - k;remLength > 0; \
           remLength -= XCHAL_IVPN_SIMD_WIDTH)
      {
        /* Input Data Load */
        MORPH_IDT_Nx16 vecInData1;
        MORPH_LOAD_Nx8_IP(vecInData1, vaInData, pvecInData);
        /* Coeff Data Load */
        MORPH_IDT_Nx16_COEFF vecCoeffData1;
        MORPH_LVNX8_IP(vecCoeffData1, pvecCoeffData1, XCHAL_IVPN_SIMD_WIDTH);
        MORPH_IDT_Nx16_COEFF vecCoeffData2;
        MORPH_LVNX8_IP(vecCoeffData2, pvecCoeffData2, XCHAL_IVPN_SIMD_WIDTH);
        MORPH_IDT_Nx16_COEFF vecCoeffData3;
        MORPH_LVNX8_IP(vecCoeffData3, pvecCoeffData3, XCHAL_IVPN_SIMD_WIDTH);
        MORPH_IDT_Nx16_COEFF vecCoeffData4;
        MORPH_LVNX8_IP(vecCoeffData4, pvecCoeffData4, XCHAL_IVPN_SIMD_WIDTH);

        /* Subtract corresponding zeropoint from input vector */
        vecInData1= IVP_SUBNX16(vecInData1, zeroPtInput);

        /* Subtract corresponding zeropoint from coeff vector */
        vecCoeffData1 = IVP_SUBNX16(vecCoeffData1, zeroPtCoeff);
        vecCoeffData2 = IVP_SUBNX16(vecCoeffData2, zeroPtCoeff);
        vecCoeffData3 = IVP_SUBNX16(vecCoeffData3, zeroPtCoeff);
        vecCoeffData4 = IVP_SUBNX16(vecCoeffData4, zeroPtCoeff);

        /* Multiply and Accumulates */
        IVP_MULANX16(accSum1, vecInData1, vecCoeffData1);
        IVP_MULANX16(accSum2, vecInData1, vecCoeffData2);
        IVP_MULANX16(accSum3, vecInData1, vecCoeffData3);
        IVP_MULANX16(accSum4, vecInData1, vecCoeffData4);
      }
      /* Reduction Addition and Bias Addition */
      xb_vecN_2x32v hvecSum1 = IVP_ADDN_2X32(IVP_CVT32UNX48L(accSum1), \
                                             IVP_CVT32UNX48H(accSum1));
      int32_t sum1           = IVP_RADDN_2X32(hvecSum1);

      /* Reduction Addition and Bias Addition */
      xb_vecN_2x32v hvecSum2 = IVP_ADDN_2X32(IVP_CVT32UNX48L(accSum2), \
                                             IVP_CVT32UNX48H(accSum2));
      int32_t sum2           = IVP_RADDN_2X32(hvecSum2);

      /* Reduction Addition and Bias Addition */
      xb_vecN_2x32v hvecSum3 = IVP_ADDN_2X32(IVP_CVT32UNX48L(accSum3), \
                                             IVP_CVT32UNX48H(accSum3));
      int32_t sum3           = IVP_RADDN_2X32(hvecSum3);

      /* Reduction Addition and Bias Addition */
      xb_vecN_2x32v hvecSum4 = IVP_ADDN_2X32(IVP_CVT32UNX48L(accSum4), \
                                             IVP_CVT32UNX48H(accSum4));
      int32_t sum4           = IVP_RADDN_2X32(hvecSum4);

      /* Moving all the scalar sums to a 32-bit vector */
      xb_vecN_2x32v hvecOut = 0;
      hvecOut = IVP_MOVN_2X32T((xb_vecN_2x32v)sum4, hvecOut, IVP_LTRN_2I(4));
      hvecOut = IVP_MOVN_2X32T((xb_vecN_2x32v)sum3, hvecOut, IVP_LTRN_2I(3));
      hvecOut = IVP_MOVN_2X32T((xb_vecN_2x32v)sum2, hvecOut, IVP_LTRN_2I(2));
      hvecOut = IVP_MOVN_2X32T((xb_vecN_2x32v)sum1, hvecOut, IVP_LTRN_2I(1));

      /* Scale, Shift, add output zeropoint and Clamp the accumulator output */
      MORPH_IDT_Nx16 vecOutData;
      SCALE_SHIFT_ADDZP_CLAMP_LIMITS_FC(vecOutData, hvecOut, quantScale, leftShift,rightShift, zeroPtOutput, minLim, maxLim);

      /* Store the output values */
      MORPH_SAVNX8_XP(vecOutData, vaOutData, pvecOut, 4);
      MORPH_SAPOSNX8_FP(vaOutData, pvecOut);
    }
    /* Loop for remaining Outputs */
    for (; outN < numOut; outN++) // Corner case for (k = 0; k < dotprodLength - 4 * XCHAL_IVPN_SIMD_WIDTH + 1; k += 4 * XCHAL_IVPN_SIMD_WIDTH)
    {
      int32_t remLength;

      /* Load bias to the first element in accumulators */
      xb_vec2Nx8 dvecBias;
      pdvecBias = (xb_vec2Nx8 *)((int8_t *)(pBiasData + outN));
      valign vaBias = IVP_LA2NX8_PP(pdvecBias);
      IVP_LAV2NX8_XP(dvecBias, vaBias, pdvecBias, 4);
      xb_vecNx48 accSum = IVP_CVT48SNX32(0, IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecBias)));
      /* Input and Coeff data pointers */
      pvecInData     = (MORPH_IDT_Nx8 *)pInData;
      pvecCoeffData1 = (MORPH_IDT_Nx8_COEFF *)(pCoeffData + outN * coeffPitch3);

      valign vaInData = MORPH_OP_PRIME_NX8(pvecInData);

      for (k = 0; k < dotprodLength - 4 * XCHAL_IVPN_SIMD_WIDTH + 1; k += 4 * XCHAL_IVPN_SIMD_WIDTH)
      { /* (inDim1 * inDim2 * inDim3) Loop */
        /* Input Data Load */
        MORPH_IDT_Nx16 vecInData1; MORPH_LOAD_Nx8_IP(vecInData1, vaInData, pvecInData);
        MORPH_IDT_Nx16 vecInData2; MORPH_LOAD_Nx8_IP(vecInData2, vaInData, pvecInData);
        MORPH_IDT_Nx16 vecInData3; MORPH_LOAD_Nx8_IP(vecInData3, vaInData, pvecInData);
        MORPH_IDT_Nx16 vecInData4; MORPH_LOAD_Nx8_IP(vecInData4, vaInData, pvecInData);

        /* Coefficient Data Load */
        MORPH_IDT_Nx16_COEFF vecCoeffData11, vecCoeffData12, vecCoeffData13, vecCoeffData14;

        vecCoeffData12 = MORPH_LVNX8_I(pvecCoeffData1, XCHAL_IVPN_SIMD_WIDTH);
        vecCoeffData13 = MORPH_LVNX8_I(pvecCoeffData1, 2 * XCHAL_IVPN_SIMD_WIDTH);
        vecCoeffData14 = MORPH_LVNX8_I(pvecCoeffData1, 3 * XCHAL_IVPN_SIMD_WIDTH);
        MORPH_LVNX8_XP(vecCoeffData11, pvecCoeffData1, 4 * XCHAL_IVPN_SIMD_WIDTH);

        /* Subtract corresponding zeropoint from input vector */
        vecInData1= IVP_SUBNX16(vecInData1, zeroPtInput);
        vecInData2= IVP_SUBNX16(vecInData2, zeroPtInput);
        vecInData3= IVP_SUBNX16(vecInData3, zeroPtInput);
        vecInData4= IVP_SUBNX16(vecInData4, zeroPtInput);

        /* Subtract corresponding zeropoint from coeff vector */
        vecCoeffData11 = IVP_SUBNX16(vecCoeffData11, zeroPtCoeff);
        vecCoeffData12 = IVP_SUBNX16(vecCoeffData12, zeroPtCoeff);
        vecCoeffData13 = IVP_SUBNX16(vecCoeffData13, zeroPtCoeff);
        vecCoeffData14 = IVP_SUBNX16(vecCoeffData14, zeroPtCoeff);

        /* Pair Multiply and Accumulates */
        IVP_MULPANX16(accSum, vecInData2, vecCoeffData12, vecInData1, vecCoeffData11);
        IVP_MULPANX16(accSum, vecInData3, vecCoeffData13, vecInData4, vecCoeffData14);
      }
      /* Handling if remLength is zero, 32, 64 or 96 */
      for (remLength = dotprodLength - k;remLength > 0; \
           remLength -= XCHAL_IVPN_SIMD_WIDTH)
      {
        /* Input Data Load */
        MORPH_IDT_Nx16 vecInData1;
        MORPH_LOAD_Nx8_IP(vecInData1, vaInData, pvecInData);

        /* Coefficient Data Load */
        MORPH_IDT_Nx16_COEFF vecCoeffData1;
        MORPH_LVNX8_IP(vecCoeffData1, pvecCoeffData1, XCHAL_IVPN_SIMD_WIDTH);

        /* Subtract corresponding zeropoint from input vector */
        vecInData1= IVP_SUBNX16(vecInData1, zeroPtInput);
        /* Subtract corresponding zeropoint from coeff vector */
        vecCoeffData1 = IVP_SUBNX16(vecCoeffData1, zeroPtCoeff);

        /* Multiply and Accumulates */
        IVP_MULANX16(accSum, vecInData1, vecCoeffData1);
      }
      /* Reduction Addition and Bias Addition */
      xb_vecN_2x32v hvecSum = IVP_ADDN_2X32(IVP_CVT32UNX48L(accSum), \
                                            IVP_CVT32UNX48H(accSum));
      int32_t sum = IVP_RADDN_2X32(hvecSum);

      /* Moving all the scalar sums to a 32-bit vector */
      xb_vecN_2x32v hvecOut = 0;
      hvecOut = IVP_MOVN_2X32T((xb_vecN_2x32v)sum, hvecOut, IVP_LTRN_2I(1));

      /* Scale, Shift, add output zeropoint and Clamp the accumulator output */
      MORPH_IDT_Nx16 vecOutData;
      SCALE_SHIFT_ADDZP_CLAMP_LIMITS_FC(vecOutData, hvecOut, quantScale, leftShift, rightShift, zeroPtOutput, minLim, maxLim);

      /* Store the output values */
      MORPH_SAVNX8_XP(vecOutData, vaOutData, pvecOut, 1);
      MORPH_SAPOSNX8_FP(vaOutData, pvecOut);
    }
  }
  else // else part of if(XI_TILE3D_IS_PTR_ALIGNED_NX8(coeffTile) && ((dotprodLength & 31) == 0))
  {
    /* The loop gives 2 outputs per iteration */
    for (outN = 0; outN < numOut - 1; outN += 2)
    {
      /* Load bias to the first element in accumulators */
      xb_vec2Nx8 dvecBias;
      pdvecBias = (xb_vec2Nx8 *)((int8_t *)(pBiasData + outN));
      valign vaBias     = IVP_LA2NX8_PP(pdvecBias);
      IVP_LAV2NX8_XP(dvecBias, vaBias, pdvecBias, 4);
      xb_vecNx48 accSum1 = IVP_CVT48SNX32(0, IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecBias)));
      IVP_LAV2NX8_XP(dvecBias, vaBias, pdvecBias, 4);
      xb_vecNx48 accSum2 = IVP_CVT48SNX32(0, IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecBias)));

      /* Input Data and Coefficient Pointers */
      pvecInData     = (MORPH_IDT_Nx8 *) pInData;
      pvecCoeffData1 = (MORPH_IDT_Nx8_COEFF *)(pCoeffData + outN * coeffPitch3);
      pvecCoeffData2 = (MORPH_IDT_Nx8_COEFF *)(pCoeffData + (outN + 1) * coeffPitch3);

      /* Priming for Input Data and Coefficient Data */
      valign vaInData     = MORPH_OP_PRIME_NX8(pvecInData);
      valign vaCoeffData1 = MORPH_OP_PRIME_NX8_COEFF(pvecCoeffData1);
      valign vaCoeffData2 = MORPH_OP_PRIME_NX8_COEFF(pvecCoeffData2);

      /* Multiplying and Accumulating 64 bytes at a time */
      for (k = 0; k < dotprodLength - 2 * XCHAL_IVPN_SIMD_WIDTH + 1; k += 2 * XCHAL_IVPN_SIMD_WIDTH)
      { /* (inDim1 * inDim2 * inDim3) Loop */
        /* Input Data Load */
        MORPH_IDT_Nx16 vecInData1; MORPH_LOAD_Nx8_IP(vecInData1, vaInData, pvecInData);
        MORPH_IDT_Nx16 vecInData2; MORPH_LOAD_Nx8_IP(vecInData2, vaInData, pvecInData);

        /* Coefficient Data Load */
        MORPH_IDT_Nx16_COEFF vecCoeffData11; MORPH_LOAD_Nx8_IP_COEFF(vecCoeffData11, vaCoeffData1, pvecCoeffData1);
        MORPH_IDT_Nx16_COEFF vecCoeffData12; MORPH_LOAD_Nx8_IP_COEFF(vecCoeffData12, vaCoeffData1, pvecCoeffData1);
        MORPH_IDT_Nx16_COEFF vecCoeffData21; MORPH_LOAD_Nx8_IP_COEFF(vecCoeffData21, vaCoeffData2, pvecCoeffData2);
        MORPH_IDT_Nx16_COEFF vecCoeffData22; MORPH_LOAD_Nx8_IP_COEFF(vecCoeffData22, vaCoeffData2, pvecCoeffData2);

        /* Subtract corresponding zeropoint from input vector */
        vecInData1= IVP_SUBNX16(vecInData1, zeroPtInput);
        vecInData2= IVP_SUBNX16(vecInData2, zeroPtInput);

        /* Subtract corresponding zeropoint from coeff vector */
        vecCoeffData11= IVP_SUBNX16(vecCoeffData11, zeroPtCoeff);
        vecCoeffData12= IVP_SUBNX16(vecCoeffData12, zeroPtCoeff);
        vecCoeffData21= IVP_SUBNX16(vecCoeffData21, zeroPtCoeff);
        vecCoeffData22= IVP_SUBNX16(vecCoeffData22, zeroPtCoeff);

        /* Pair Multiply and Accumulates */
        IVP_MULPANX16(accSum1, vecInData2, vecCoeffData12, vecInData1, vecCoeffData11);
        IVP_MULPANX16(accSum2, vecInData2, vecCoeffData22, vecInData1, vecCoeffData21);
      }
      if (k < dotprodLength )
      {
        /* Corner case handling if dotprodLength is not a multiple of 2 * XCHAL_IVPN_SIMD_WIDTH  */
        int32_t remLength = dotprodLength - k;

        /* Mask is used in the predicated subtract instruction to
         * ensure zero point is subtracted from valid data
         */
        xb_vecNx16U vecSeq = IVP_SEQNX16U();
        vboolN vbRemLengthMask1 = IVP_LTNX16(vecSeq, remLength);
        vboolN vbRemLengthMask2 = IVP_LTNX16(vecSeq, remLength - XCHAL_IVPN_SIMD_WIDTH);

        /* Input Data Load */
        MORPH_IDT_Nx16 vecInData1;
        MORPH_LOAD_NX8_VAR(vecInData1, vaInData, pvecInData, remLength);
        MORPH_IDT_Nx16 vecInData2;
        MORPH_LOAD_NX8_VAR(vecInData2, vaInData, pvecInData, \
                                                      remLength - XCHAL_IVPN_SIMD_WIDTH);
        /* Coefficient Data Load */
        MORPH_IDT_Nx16_COEFF vecCoeffData11, vecCoeffData12;
        MORPH_IDT_Nx16_COEFF vecCoeffData21, vecCoeffData22;
        MORPH_LOAD_NX8_VAR_COEFF(vecCoeffData11, vaCoeffData1, pvecCoeffData1, remLength);
        MORPH_LOAD_NX8_VAR_COEFF(vecCoeffData12, vaCoeffData1, pvecCoeffData1, \
                                                      remLength - XCHAL_IVPN_SIMD_WIDTH);
        MORPH_LOAD_NX8_VAR_COEFF(vecCoeffData21, vaCoeffData2, pvecCoeffData2, remLength);
        MORPH_LOAD_NX8_VAR_COEFF(vecCoeffData22, vaCoeffData2, pvecCoeffData2, \
                                                      remLength - XCHAL_IVPN_SIMD_WIDTH);

        /* Subtract corresponding zeropoints from input vector */
        IVP_SUBNX16T(vecInData1, vecInData1, zeroPtInput, vbRemLengthMask1);
        IVP_SUBNX16T(vecInData2, vecInData2, zeroPtInput, vbRemLengthMask2);

        /* Subtract corresponding zeropoint from coeff vector */
        IVP_SUBNX16T(vecCoeffData11, vecCoeffData11, zeroPtCoeff, vbRemLengthMask1);
        IVP_SUBNX16T(vecCoeffData12, vecCoeffData12, zeroPtCoeff, vbRemLengthMask2);
        IVP_SUBNX16T(vecCoeffData21, vecCoeffData21, zeroPtCoeff, vbRemLengthMask1);
        IVP_SUBNX16T(vecCoeffData22, vecCoeffData22, zeroPtCoeff, vbRemLengthMask2);
        /* Pair Multiply and Accumulates */
        IVP_MULPANX16(accSum1, vecInData2, vecCoeffData12, vecInData1, vecCoeffData11);
        IVP_MULPANX16(accSum2, vecInData2, vecCoeffData22, vecInData1, vecCoeffData21);
      }
      /* Reduction Addition and Bias Addition */
      xb_vecN_2x32v hvecSum1 = IVP_ADDN_2X32(IVP_CVT32UNX48L(accSum1), \
                                             IVP_CVT32UNX48H(accSum1));
      int32_t sum1           = IVP_RADDN_2X32(hvecSum1);

      /* Reduction Addition and Bias Addition */
      xb_vecN_2x32v hvecSum2 = IVP_ADDN_2X32(IVP_CVT32UNX48L(accSum2), \
                                             IVP_CVT32UNX48H(accSum2));
      int32_t sum2           = IVP_RADDN_2X32(hvecSum2);

      /* Moving all the scalar sums to a 32-bit vector */
      xb_vecN_2x32v hvecOut = 0;
      hvecOut = IVP_MOVN_2X32T((xb_vecN_2x32v)sum2, hvecOut, IVP_LTRN_2I(2));
      hvecOut = IVP_MOVN_2X32T((xb_vecN_2x32v)sum1, hvecOut, IVP_LTRN_2I(1));

      /* Scale, Shift, Add output zeropoint and Clamp the accumulator output */
      MORPH_IDT_Nx16 vecOutData;
      SCALE_SHIFT_ADDZP_CLAMP_LIMITS_FC(vecOutData, hvecOut, quantScale, leftShift, rightShift, zeroPtOutput, minLim, maxLim);

      /* Store the output values */
      MORPH_SAVNX8_XP(vecOutData, vaOutData, pvecOut, 2);
      MORPH_SAPOSNX8_FP(vaOutData, pvecOut);
    }
    if (outN < numOut) // Corner case of for (k = 0; k < dotprodLength - 2 * XCHAL_IVPN_SIMD_WIDTH + 1; k += 2 * XCHAL_IVPN_SIMD_WIDTH)
    {
      /* Load bias to the first element in accumulator */
      xb_vec2Nx8 dvecBias;
      pdvecBias = (xb_vec2Nx8 *)((int8_t *)(pBiasData + outN));
      valign vaBias = IVP_LA2NX8_PP(pdvecBias);
      IVP_LAV2NX8_XP(dvecBias, vaBias, pdvecBias, 4);
      xb_vecNx48 accSum1 = IVP_CVT48SNX32(0, IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecBias)));

      /* Input Data and Coefficient Pointers */
      pvecInData     = (MORPH_IDT_Nx8 *) pInData;
      pvecCoeffData1 = (MORPH_IDT_Nx8_COEFF *)(pCoeffData + outN * coeffPitch3);

      /* Priming for Input Data and Coefficient Data */
      valign vaInData     = MORPH_OP_PRIME_NX8(pvecInData);
      valign vaCoeffData1 = MORPH_OP_PRIME_NX8_COEFF(pvecCoeffData1);

      /* Multiplying and Accumulating 128 bytes at a time */
      for (k = 0; k < dotprodLength - 2 * XCHAL_IVPN_SIMD_WIDTH + 1; k += 2 * XCHAL_IVPN_SIMD_WIDTH)
      { /* (inDim1 * inDim2 * inDim3) Loop */
        /* Input Data Load */
        MORPH_IDT_Nx16 vecInData1; MORPH_LOAD_Nx8_IP(vecInData1, vaInData, pvecInData);
        MORPH_IDT_Nx16 vecInData2; MORPH_LOAD_Nx8_IP(vecInData2, vaInData, pvecInData);

        /* Coefficient Data Load */
        MORPH_IDT_Nx16_COEFF vecCoeffData11; MORPH_LOAD_Nx8_IP_COEFF(vecCoeffData11, vaCoeffData1, pvecCoeffData1);
        MORPH_IDT_Nx16_COEFF vecCoeffData12; MORPH_LOAD_Nx8_IP_COEFF(vecCoeffData12, vaCoeffData1, pvecCoeffData1);

        /* Subtract corresponding zeropoints from input vector */
        vecInData1= IVP_SUBNX16(vecInData1, zeroPtInput);
        vecInData2= IVP_SUBNX16(vecInData2, zeroPtInput);

        /* Subtract corresponding zeropoint from coeff vector */
        vecCoeffData11= IVP_SUBNX16(vecCoeffData11, zeroPtCoeff);
        vecCoeffData12= IVP_SUBNX16(vecCoeffData12, zeroPtCoeff);

        /* Pair Multiply and Accumulates */
        IVP_MULPANX16(accSum1, vecInData2, vecCoeffData12, vecInData1, vecCoeffData11);
      }
      if (k < dotprodLength)
      {
        /* Corner case handling if dotprodLength is not a multiple of 2*XCHAL_IVPN_SIMD_WIDTH */
        int32_t remLength = dotprodLength - k;

        /* Mask is used in the predicated subtract instruction to
         * ensure zero point is subtracted from valid data
         */
        xb_vecNx16U vecSeq = IVP_SEQNX16U();
        vboolN vbRemLengthMask1 = IVP_LTNX16(vecSeq, remLength);
        vboolN vbRemLengthMask2 = IVP_LTNX16(vecSeq, remLength - XCHAL_IVPN_SIMD_WIDTH);

        /* Input Data Load */
        MORPH_IDT_Nx16 vecInData1;
        MORPH_LOAD_NX8_VAR(vecInData1, vaInData, pvecInData, remLength);
        MORPH_IDT_Nx16 vecInData2;
        MORPH_LOAD_NX8_VAR(vecInData2, vaInData, pvecInData, \
                                                      remLength - XCHAL_IVPN_SIMD_WIDTH);
        /* Coefficient Data Load */
        MORPH_IDT_Nx16_COEFF vecCoeffData11, vecCoeffData12;
        MORPH_LOAD_NX8_VAR_COEFF(vecCoeffData11, vaCoeffData1, pvecCoeffData1, remLength);
        MORPH_LOAD_NX8_VAR_COEFF(vecCoeffData12, vaCoeffData1, pvecCoeffData1, \
                                                      remLength - XCHAL_IVPN_SIMD_WIDTH);

        /* Subtract corresponding zeropoints from coeff and input */
        IVP_SUBNX16T(vecInData1, vecInData1, zeroPtInput, vbRemLengthMask1);
        IVP_SUBNX16T(vecInData2, vecInData2, zeroPtInput, vbRemLengthMask2);

        IVP_SUBNX16T(vecCoeffData11, vecCoeffData11, zeroPtCoeff, vbRemLengthMask1);
        IVP_SUBNX16T(vecCoeffData12, vecCoeffData12, zeroPtCoeff, vbRemLengthMask2);

        /* Pair Multiply and Accumulates */
        IVP_MULPANX16(accSum1, vecInData2, vecCoeffData12, vecInData1, vecCoeffData11);

      }
        /* Reduction Addition and Bias Addition */
        xb_vecN_2x32v hvecSum1 = IVP_ADDN_2X32(IVP_CVT32UNX48L(accSum1), \
                                               IVP_CVT32UNX48H(accSum1));
        int32_t sum1           = IVP_RADDN_2X32(hvecSum1);

        /* Moving all the scalar sums to a 32-bit vector */
        xb_vecN_2x32v hvecOut = 0;
        hvecOut = IVP_MOVN_2X32T((xb_vecN_2x32v)sum1, hvecOut, IVP_LTRN_2I(1));

        /* Scale, Shift, Add output zeropoint and Clamp the accumulator output */
        MORPH_IDT_Nx16 vecOutData;
        SCALE_SHIFT_ADDZP_CLAMP_LIMITS_FC(vecOutData, hvecOut, quantScale, leftShift, rightShift, zeroPtOutput, minLim, maxLim);

        /* Store the output values */
        MORPH_SAVNX8_XP(vecOutData, vaOutData, pvecOut, 1);
        MORPH_SAPOSNX8_FP(vaOutData, pvecOut);
    }
  }
  return(XI_ERROR_STATUS());
}

#endif //if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))
