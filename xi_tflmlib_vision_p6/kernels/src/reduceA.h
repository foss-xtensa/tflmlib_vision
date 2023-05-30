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

#define UNSIGNED8BIT       1
#define SIGNED8BIT         2

#define CONVERT_AND_ADD_S8_1(dvecSum1, hvecSumVal1, accVal1)                                  {                                                  \
    /* Conversion from 2Nx24 to N_2x32 */                                                                                                        \
    xb_vecN_2x32v hvecSum1LL = IVP_CVT32S2NX24LL(dvecSum1);                                                                                      \
    xb_vecN_2x32v hvecSum1LH = IVP_CVT32S2NX24LH(dvecSum1);                                                                                      \
    xb_vecN_2x32v hvecSum1HL = IVP_CVT32S2NX24HL(dvecSum1);                                                                                      \
    xb_vecN_2x32v hvecSum1HH = IVP_CVT32S2NX24HH(dvecSum1);                                                                                      \
                                                                                                                                                 \
    hvecSumVal1 = (MORPH_ACC_TYPE) (IVP_RADDN_2X32(IVP_ADDN_2X32(IVP_ADDN_2X32                                                                   \
                                                                   (IVP_ADDN_2X32(hvecSum1LL, hvecSum1LH), hvecSum1HL), hvecSum1HH))) + accVal1; \
}

#define CONVERT_AND_ADD_U8_1(dvecSum1, hvecSumVal1, accVal1)                                  {                                                     \
    /* Conversion from 2Nx24 to N_2x32 */                                                                                                           \
    xb_vecN_2x32Uv hvecSum1LL = IVP_CVT32S2NX24LL(dvecSum1);                                                                                        \
    xb_vecN_2x32Uv hvecSum1LH = IVP_CVT32S2NX24LH(dvecSum1);                                                                                        \
    xb_vecN_2x32Uv hvecSum1HL = IVP_CVT32S2NX24HL(dvecSum1);                                                                                        \
    xb_vecN_2x32Uv hvecSum1HH = IVP_CVT32S2NX24HH(dvecSum1);                                                                                        \
                                                                                                                                                    \
    hvecSumVal1 = (MORPH_ACC_TYPE) (IVP_RADDN_2X32U(IVP_ADDN_2X32U(IVP_ADDN_2X32U                                                                   \
                                                                     (IVP_ADDN_2X32U(hvecSum1LL, hvecSum1LH), hvecSum1HL), hvecSum1HH))) + accVal1; \
}

#define CONVERT_AND_ADD_S8_2(dvecSum1, dvecSum2, hvecSumVal1, hvecSumVal2, accVal1, accVal2)  {                                                  \
    /* Conversion from 2Nx24 to N_2x32 */                                                                                                        \
    xb_vecN_2x32v hvecSum1LL = IVP_CVT32S2NX24LL(dvecSum1);                                                                                      \
    xb_vecN_2x32v hvecSum1LH = IVP_CVT32S2NX24LH(dvecSum1);                                                                                      \
    xb_vecN_2x32v hvecSum1HL = IVP_CVT32S2NX24HL(dvecSum1);                                                                                      \
    xb_vecN_2x32v hvecSum1HH = IVP_CVT32S2NX24HH(dvecSum1);                                                                                      \
                                                                                                                                                 \
    xb_vecN_2x32v hvecSum2LL = IVP_CVT32S2NX24LL(dvecSum2);                                                                                      \
    xb_vecN_2x32v hvecSum2LH = IVP_CVT32S2NX24LH(dvecSum2);                                                                                      \
    xb_vecN_2x32v hvecSum2HL = IVP_CVT32S2NX24HL(dvecSum2);                                                                                      \
    xb_vecN_2x32v hvecSum2HH = IVP_CVT32S2NX24HH(dvecSum2);                                                                                      \
                                                                                                                                                 \
    hvecSumVal1 = (MORPH_ACC_TYPE) (IVP_RADDN_2X32(IVP_ADDN_2X32(IVP_ADDN_2X32                                                                   \
                                                                   (IVP_ADDN_2X32(hvecSum1LL, hvecSum1LH), hvecSum1HL), hvecSum1HH))) + accVal1; \
    hvecSumVal2 = (MORPH_ACC_TYPE) (IVP_RADDN_2X32(IVP_ADDN_2X32(IVP_ADDN_2X32                                                                   \
                                                                   (IVP_ADDN_2X32(hvecSum2LL, hvecSum2LH), hvecSum2HL), hvecSum2HH))) + accVal2; \
}

#define CONVERT_AND_ADD_U8_2(dvecSum1, dvecSum2, hvecSumVal1, hvecSumVal2, accVal1, accVal2)  {                                                     \
    /* Conversion from 2Nx24 to N_2x32 */                                                                                                           \
    xb_vecN_2x32Uv hvecSum1LL = IVP_CVT32S2NX24LL(dvecSum1);                                                                                        \
    xb_vecN_2x32Uv hvecSum1LH = IVP_CVT32S2NX24LH(dvecSum1);                                                                                        \
    xb_vecN_2x32Uv hvecSum1HL = IVP_CVT32S2NX24HL(dvecSum1);                                                                                        \
    xb_vecN_2x32Uv hvecSum1HH = IVP_CVT32S2NX24HH(dvecSum1);                                                                                        \
                                                                                                                                                    \
    xb_vecN_2x32Uv hvecSum2LL = IVP_CVT32S2NX24LL(dvecSum2);                                                                                        \
    xb_vecN_2x32Uv hvecSum2LH = IVP_CVT32S2NX24LH(dvecSum2);                                                                                        \
    xb_vecN_2x32Uv hvecSum2HL = IVP_CVT32S2NX24HL(dvecSum2);                                                                                        \
    xb_vecN_2x32Uv hvecSum2HH = IVP_CVT32S2NX24HH(dvecSum2);                                                                                        \
                                                                                                                                                    \
    hvecSumVal1 = (MORPH_ACC_TYPE) (IVP_RADDN_2X32U(IVP_ADDN_2X32U(IVP_ADDN_2X32U                                                                   \
                                                                     (IVP_ADDN_2X32U(hvecSum1LL, hvecSum1LH), hvecSum1HL), hvecSum1HH))) + accVal1; \
    hvecSumVal2 = (MORPH_ACC_TYPE) (IVP_RADDN_2X32U(IVP_ADDN_2X32U(IVP_ADDN_2X32U                                                                   \
                                                                     (IVP_ADDN_2X32U(hvecSum2LL, hvecSum2LH), hvecSum2HL), hvecSum2HH))) + accVal2; \
}

#define CONVERT_AND_STORE_S8(dvecSum1, vaOut, phvecOut, varLen, tileFlag, phvecAcc)           {      \
    xb_vecN_2x32v hvecSum1LL = IVP_CVT32S2NX24LL(dvecSum1);                                          \
    xb_vecN_2x32v hvecSum1LH = IVP_CVT32S2NX24LH(dvecSum1);                                          \
    xb_vecN_2x32v hvecSum1HL = IVP_CVT32S2NX24HL(dvecSum1);                                          \
    xb_vecN_2x32v hvecSum1HH = IVP_CVT32S2NX24HH(dvecSum1);                                          \
                                                                                                     \
    xb_vecN_2x32v hvecSum1, hvecSum2, hvecSum3, hvecSum4;                                            \
    hvecSum1 = 0;                                                                                    \
    hvecSum2 = 0;                                                                                    \
    hvecSum3 = 0;                                                                                    \
    hvecSum4 = 0;                                                                                    \
                                                                                                     \
    if (tileFlag == XI_CNN_REDUCE_INTERMEDIATE_TILE || tileFlag == XI_CNN_REDUCE_LAST_TILE)          \
    {                                                                                                \
      valign vaOutData = IVP_LAN_2X32_PP(phvecOut);                                                  \
      IVP_LAVN_2X32_XP(hvecSum1, vaOutData, phvecAcc, 4 * (varLen));                                 \
      IVP_LAVN_2X32_XP(hvecSum2, vaOutData, phvecAcc, 4 * (varLen - XCHAL_IVPN_SIMD_WIDTH / 2));     \
      IVP_LAVN_2X32_XP(hvecSum3, vaOutData, phvecAcc, 4 * (varLen - XCHAL_IVPN_SIMD_WIDTH));         \
      IVP_LAVN_2X32_XP(hvecSum4, vaOutData, phvecAcc, 4 * (varLen - 3 * XCHAL_IVPN_SIMD_WIDTH / 2)); \
    }                                                                                                \
    hvecSum1LL = IVP_ADDN_2X32(hvecSum1LL, hvecSum1);                                                \
    hvecSum1LH = IVP_ADDN_2X32(hvecSum1LH, hvecSum2);                                                \
    hvecSum1HL = IVP_ADDN_2X32(hvecSum1HL, hvecSum3);                                                \
    hvecSum1HH = IVP_ADDN_2X32(hvecSum1HH, hvecSum4);                                                \
    /* Store the result */                                                                           \
    IVP_SAVN_2X32_XP(hvecSum1LL, vaOut, phvecOut, 4 * varLen);                                       \
    IVP_SAVN_2X32_XP(hvecSum1LH, vaOut, phvecOut, 4 * (varLen - XCHAL_IVPN_SIMD_WIDTH / 2));         \
    IVP_SAVN_2X32_XP(hvecSum1HL, vaOut, phvecOut, 4 * (varLen - XCHAL_IVPN_SIMD_WIDTH));             \
    IVP_SAVN_2X32_XP(hvecSum1HH, vaOut, phvecOut, 4 * (varLen - 3 * XCHAL_IVPN_SIMD_WIDTH / 2));     \
                                                                                                     \
    IVP_SAPOSN_2X32_FP(vaOut, phvecOut);                                                             \
}

#define CONVERT_AND_STORE_U8(dvecSum1, vaOut, phvecOut, varLen, tileFlag, phvecAcc)           {       \
    xb_vecN_2x32Uv hvecSum1LL = IVP_CVT32S2NX24LL(dvecSum1);                                          \
    xb_vecN_2x32Uv hvecSum1LH = IVP_CVT32S2NX24LH(dvecSum1);                                          \
    xb_vecN_2x32Uv hvecSum1HL = IVP_CVT32S2NX24HL(dvecSum1);                                          \
    xb_vecN_2x32Uv hvecSum1HH = IVP_CVT32S2NX24HH(dvecSum1);                                          \
                                                                                                      \
    xb_vecN_2x32Uv hvecSum1, hvecSum2, hvecSum3, hvecSum4;                                            \
    hvecSum1 = 0;                                                                                     \
    hvecSum2 = 0;                                                                                     \
    hvecSum3 = 0;                                                                                     \
    hvecSum4 = 0;                                                                                     \
                                                                                                      \
    if (tileFlag == XI_CNN_REDUCE_INTERMEDIATE_TILE || tileFlag == XI_CNN_REDUCE_LAST_TILE)          \
    {                                                                                                 \
      valign vaOutData = IVP_LAN_2X32U_PP(phvecAcc);                                                  \
      IVP_LAVN_2X32U_XP(hvecSum1, vaOutData, phvecAcc, 4 * varLen);                                   \
      IVP_LAVN_2X32U_XP(hvecSum2, vaOutData, phvecAcc, 4 * (varLen - XCHAL_IVPN_SIMD_WIDTH / 2));     \
      IVP_LAVN_2X32U_XP(hvecSum3, vaOutData, phvecAcc, 4 * (varLen - XCHAL_IVPN_SIMD_WIDTH));         \
      IVP_LAVN_2X32U_XP(hvecSum4, vaOutData, phvecAcc, 4 * (varLen - 3 * XCHAL_IVPN_SIMD_WIDTH / 2)); \
    }                                                                                                 \
    hvecSum1LL = IVP_ADDN_2X32U(hvecSum1LL, hvecSum1);                                                 \
    hvecSum1LH = IVP_ADDN_2X32U(hvecSum1LH, hvecSum2);                                                 \
    hvecSum1HL = IVP_ADDN_2X32U(hvecSum1HL, hvecSum3);                                                 \
    hvecSum1HH = IVP_ADDN_2X32U(hvecSum1HH, hvecSum4);                                                 \
                                                                                                      \
    /* Store the result */                                                                            \
    IVP_SAVN_2X32U_XP(hvecSum1LL, vaOut, phvecOut, 4 * varLen);                                       \
    IVP_SAVN_2X32U_XP(hvecSum1LH, vaOut, phvecOut, 4 * (varLen - XCHAL_IVPN_SIMD_WIDTH / 2));         \
    IVP_SAVN_2X32U_XP(hvecSum1HL, vaOut, phvecOut, 4 * (varLen - XCHAL_IVPN_SIMD_WIDTH));             \
    IVP_SAVN_2X32U_XP(hvecSum1HH, vaOut, phvecOut, 4 * (varLen - 3 * XCHAL_IVPN_SIMD_WIDTH / 2));     \
                                                                                                      \
    IVP_SAPOSN_2X32U_FP(vaOut, phvecOut);                                                             \
}

#define MAKE_NAME_IMPL(name, MORPH_FNAME_SPECIFIER_IDT)  name## _ ##MORPH_FNAME_SPECIFIER_IDT

#undef MAKE_NAME_1
#undef MAKE_NAME_2
#undef MORPH_IDT_CHECK
#undef MORPH_ODT_CHECK
#undef MORPH_IDT_4D_CHECK
#undef MORPH_ODT_4D_CHECK
#undef MORPH_IDT_SCALAR
#undef MORPH_ODT_SCALAR
#undef MORPH_IDT_2Nx8
#undef MORPH_ODT_VECTOR
#undef MORPH_IDT_WIDEVECTOR
#undef MORPH_ACC_TYPE
#undef MORPH_INT16
#undef MORPH_LA2NX8_PP
#undef MORPH_LA2NX8_IP
#undef MORPH_LA2NX8_XP
#undef MORPH_LAV2NX8_XP
#undef MORPH_MAX2NX8
#undef MORPH_OR2NX8
#undef MORPH_EQ2NX8
#undef MORPH_NEQ2NX8
#undef MORPH_RADD2NX8
#undef MORPH_MIN2NX8
#undef MORPH_MOV2NX8T
#undef MORPH_SAV2NX8_XP
#undef MORPH_SAPOS2NX8_FP
#undef MORPH_RMAX2NX8
#undef MORPH_RMIN2NX8
#undef MORPH_MINVALUE
#undef MORPH_MAXVALUE
#undef MORPH_CALCULATE_MAX
#undef MORPH_OP_BYTES_PER_PIXEL
#undef MORPH_OP_ADDWA
#undef MORPH_OP_CONVERT_ADD_1
#undef MORPH_OP_CONVERT_ADD_2
#undef MORPH_OP_VAR_STORE
#undef MORPH_OP_STORE_FLUSH
#undef MORPH_OP_CONVERT_STORE

#if INPUT_DATA_TYPE == UNSIGNED8BIT
#define MAKE_NAME_1(name)                                MAKE_NAME_IMPL(name, U8)
#define MAKE_NAME_2(name)                                MAKE_NAME_IMPL(name, U8U32)
#define MORPH_IDT_CHECK                                  XI_CHECK_TILE3D_U8
#define MORPH_ODT_CHECK                                  XI_CHECK_TILE3D_U32
#define MORPH_IDT_4D_CHECK                               XI_CHECK_TILE4D_U8
#define MORPH_ODT_4D_CHECK                               XI_CHECK_TILE4D_U32
#define MORPH_IDT_SCALAR                                 uint8_t
#define MORPH_ODT_SCALAR                                 uint32_t
#define MORPH_INT16                                      uint16_t
#define MORPH_IDT_2Nx8                                   xb_vec2Nx8U
#define MORPH_ODT_VECTOR                                 xb_vecN_2x32Uv
#define MORPH_IDT_WIDEVECTOR                             xb_vec2Nx24
#define MORPH_ACC_TYPE                                   int32_t
#define MORPH_LA2NX8_PP                                  IVP_LA2NX8U_PP
#define MORPH_LA2NX8_IP                                  IVP_LA2NX8U_IP
#define MORPH_LA2NX8_XP                                  IVP_LA2NX8U_XP
#define MORPH_LAV2NX8_XP                                 IVP_LAV2NX8U_XP
#define MORPH_MAX2NX8                                    IVP_MAXU2NX8U
#define MORPH_MIN2NX8                                    IVP_MINU2NX8U
#define MORPH_OR2NX8                                     IVP_OR2NX8U
#define MORPH_EQ2NX8                                     IVP_EQ2NX8U
#define MORPH_NEQ2NX8                                    IVP_NEQ2NX8U
#define MORPH_RADD2NX8                                   IVP_RADDU2NX8
#define MORPH_MOV2NX8T                                   IVP_MOV2NX8UT
#define MORPH_SAV2NX8_XP                                 IVP_SAV2NX8U_XP
#define MORPH_SAPOS2NX8_FP                               IVP_SAPOS2NX8U_FP
#define MORPH_MINVALUE                                   0
#define MORPH_MAXVALUE                                   UCHAR_MAX
#define MORPH_OP_BYTES_PER_PIXEL                         1
#define MORPH_OP_ADDWA                                   IVP_ADDWUA2NX8U
#define MORPH_OP_CONVERT_ADD_1                           CONVERT_AND_ADD_U8_1
#define MORPH_OP_CONVERT_ADD_2                           CONVERT_AND_ADD_U8_2
#define MORPH_OP_VAR_STORE                               IVP_SAVN_2X32U_XP
#define MORPH_OP_STORE_FLUSH                             IVP_SAPOSN_2X32U_FP
#define MORPH_OP_CONVERT_STORE                           CONVERT_AND_STORE_U8

#ifdef IVP_RMAXU2NX8
#define MORPH_RMAX2NX8(dvecIn, maxOut)                                         \
  maxOut = IVP_RMAXU2NX8(dvecIn);
#else
#define MORPH_RMAX2NX8(dvecIn, maxOut)                                         \
{                                                                              \
  xb_vec2Nx8U dvecMaxL, dvecMaxH;                                              \
  IVP_DSEL2NX8UI(dvecMaxH, dvecMaxL, 0, dvecIn, IVP_DSELI_8B_INTERLEAVE_1);    \
  xb_vec2Nx8U dvecMax = IVP_MAXU2NX8U(dvecMaxH, dvecMaxL);                     \
  uint16_t maxValue   = IVP_RMAXUNX16(IVP_MOVNX16_FROM2NX8U(dvecMax));         \
  maxOut              = (uint8_t)maxValue;                                     \
  }
#endif

#ifdef IVP_RMINU2NX8
#define MORPH_RMIN2NX8(dvecIn, minOut)                                         \
  minOut = IVP_RMINU2NX8(dvecIn);
#else
#define MORPH_RMIN2NX8(dvecIn, minOut)                                         \
{                                                                              \
  xb_vec2Nx8U dvecMinL, dvecMinH;                                              \
  IVP_DSEL2NX8UI(dvecMinH, dvecMinL, 0, dvecIn, IVP_DSELI_8B_INTERLEAVE_1);    \
  xb_vec2Nx8U dvecMin = IVP_MINU2NX8U(dvecMinH, dvecMinL);                     \
  uint16_t minValue   = IVP_RMINUNX16(IVP_MOVNX16_FROM2NX8U(dvecMin));         \
  minOut              = (uint8_t)minValue;                                     \
}
#endif

#define MORPH_CALCULATE_MAX(pdvecIn1, pdvecIn2, dvecMax1, dvecMax2, loopCount)  \
{                                                                               \
  for (x = loopCount; x > 0; x -= vectorizationWidth)                           \
  {                                                                             \
    /* Load the data */                                                         \
    MORPH_LAV2NX8_XP(dvecInData1, vaInData1, pdvecIn1, x);                      \
    MORPH_LAV2NX8_XP(dvecInData2, vaInData2, pdvecIn2, x);                      \
    /* Find the Maximum value */                                                \
    dvecMax1 = MORPH_MAX2NX8(dvecMax1, dvecInData1);                            \
    dvecMax2 = MORPH_MAX2NX8(dvecMax2, dvecInData2);                            \
  }                                                                             \
}

#elif INPUT_DATA_TYPE == SIGNED8BIT
#define MAKE_NAME_1(name)                                MAKE_NAME_IMPL(name, S8)
#define MAKE_NAME_2(name)                                MAKE_NAME_IMPL(name, S8S32)
#define MORPH_IDT_CHECK                                  XI_CHECK_TILE3D_S8
#define MORPH_ODT_CHECK                                  XI_CHECK_TILE3D_S32
#define MORPH_IDT_4D_CHECK                               XI_CHECK_TILE4D_S8
#define MORPH_ODT_4D_CHECK                               XI_CHECK_TILE4D_S32
#define MORPH_IDT_SCALAR                                 int8_t
#define MORPH_ODT_SCALAR                                 int32_t
#define MORPH_INT16                                      int16_t
#define MORPH_IDT_2Nx8                                   xb_vec2Nx8
#define MORPH_ODT_VECTOR                                 xb_vecN_2x32v
#define MORPH_IDT_WIDEVECTOR                             xb_vec2Nx24
#define MORPH_ACC_TYPE                                   int32_t
#define MORPH_LA2NX8_PP                                  IVP_LA2NX8_PP
#define MORPH_LA2NX8_IP                                  IVP_LA2NX8_IP
#define MORPH_LA2NX8_XP                                  IVP_LA2NX8_XP
#define MORPH_LAV2NX8_XP                                 IVP_LAV2NX8_XP
#define MORPH_MAX2NX8                                    IVP_MAX2NX8
#define MORPH_MIN2NX8                                    IVP_MIN2NX8
#define MORPH_OR2NX8                                     IVP_OR2NX8
#define MORPH_EQ2NX8                                     IVP_EQ2NX8U
#define MORPH_NEQ2NX8                                    IVP_NEQ2NX8U
#define MORPH_RADD2NX8                                   IVP_RADD2NX8
#define MORPH_MOV2NX8T                                   IVP_MOV2NX8T
#define MORPH_SAV2NX8_XP                                 IVP_SAV2NX8_XP
#define MORPH_SAPOS2NX8_FP                               IVP_SAPOS2NX8_FP
#define MORPH_MINVALUE                                   SCHAR_MIN
#define MORPH_MAXVALUE                                   SCHAR_MAX
#define MORPH_OP_BYTES_PER_PIXEL                         1
#define MORPH_OP_ADDWA                                   IVP_ADDWA2NX8
#define MORPH_OP_CONVERT_ADD_1                           CONVERT_AND_ADD_S8_1
#define MORPH_OP_CONVERT_ADD_2                           CONVERT_AND_ADD_S8_2
#define MORPH_OP_VAR_STORE                               IVP_SAVN_2X32_XP
#define MORPH_OP_STORE_FLUSH                             IVP_SAPOSN_2X32_FP
#define MORPH_OP_CONVERT_STORE                           CONVERT_AND_STORE_S8

#ifdef IVP_RMAX2NX8
#define MORPH_RMAX2NX8(dvecIn, maxOut)                                         \
  maxOut = IVP_RMAX2NX8(dvecIn);
#else
#define MORPH_RMAX2NX8(dvecIn, maxOut)                                         \
{                                                                              \
  xb_vecNx16 vecMaxEven, vecMaxOdd;                                            \
  vecMaxEven = IVP_UNPKS2NX8_0(dvecIn);                                        \
  vecMaxOdd  = IVP_UNPKS2NX8_1(dvecIn);                                        \
  xb_vecNx16 vecMax = IVP_MAXNX16(vecMaxOdd, vecMaxEven);                      \
  int16_t maxValue  = IVP_RMAXNX16(vecMax);                                    \
  maxOut            = (int8_t)maxValue;                                        \
}
#endif

#ifdef IVP_RMIN2NX8
#define MORPH_RMIN2NX8(dvecIn, minOut)                                         \
  minOut = IVP_RMIN2NX8(dvecIn);
#else
#define MORPH_RMIN2NX8(dvecIn, minOut)                                         \
{                                                                              \
  xb_vecNx16 vecMinEven, vecMinOdd;                                            \
  vecMinEven = IVP_UNPKS2NX8_0(dvecIn);                                        \
  vecMinOdd  = IVP_UNPKS2NX8_1(dvecIn);                                        \
  xb_vecNx16 vecMin = IVP_MINNX16(vecMinOdd, vecMinEven);                      \
  int16_t minValue  = IVP_RMINNX16(vecMin);                                    \
  minOut            = (int8_t)minValue;                                        \
}
#endif

#define MORPH_CALCULATE_MAX(pdvecIn1, pdvecIn2, dvecMax1, dvecMax2, loopCount)  \
{                                                                               \
  for (x = 0; x <= (loopCount - vectorizationWidth); x += vectorizationWidth)   \
  {                                                                             \
    /* Load the data */                                                         \
    MORPH_LA2NX8_IP(dvecInData1, vaInData1, pdvecIn1);                          \
    MORPH_LA2NX8_IP(dvecInData2, vaInData2, pdvecIn2);                          \
    /* Find the Maximum value */                                                \
    dvecMax1 = MORPH_MAX2NX8(dvecMax1, dvecInData1);                            \
    dvecMax2 = MORPH_MAX2NX8(dvecMax2, dvecInData2);                            \
  }                                                                             \
  if (x < loopCount)                                                            \
  {                                                                             \
    /* Mask generation for valid elements */                                    \
    int32_t remX = (loopCount - x);                                             \
    vbool2N vbMask = IVP_LTU2NX8U(IVP_SEQ2NX8(), remX);                         \
    /* Load the data */                                                         \
    MORPH_LA2NX8_IP(dvecInData1, vaInData1, pdvecIn1);                          \
    MORPH_LA2NX8_IP(dvecInData2, vaInData2, pdvecIn2);                          \
    dvecInData1 = MORPH_MOV2NX8T(dvecInData1, MORPH_MINVALUE, vbMask);          \
    dvecInData2 = MORPH_MOV2NX8T(dvecInData2, MORPH_MINVALUE, vbMask);          \
    /* Find the Maximum value */                                                \
    dvecMax1 = MORPH_MAX2NX8(dvecMax1, dvecInData1);                            \
    dvecMax2 = MORPH_MAX2NX8(dvecMax2, dvecInData2);                            \
  }                                                                             \
}
#endif


/************************* xiReduceMaxA4D_S8/U8 ******************************************/
/* Description  : Optimized implementation of xiReduceMaxA4D  function                   */
/* Inputs       : Input Data Tile and CNN Reduce Parameters                              */
/* Inout        : Output Data Tile                                                       */
/* Output       : XI Error Code                                                          */
/* Assumptions  : Input  and Output data is S8/U8                                        */
/*****************************************************************************************/
/*************************** xiReduceMaxA4D_S8 *******************************************/
/*************************** xiReduceMaxA4D_U8 *******************************************/
XI_ERR_TYPE MAKE_NAME_1(xiReduceMaxA4D)(const xi_pTile4D inTile,
                                      xi_pTile4D outTile,
                                      const xi_cnn_reduceA_params *params)
{

  /* Error Checks */
  XI_ERROR_CHECKS()
  {
    MORPH_IDT_4D_CHECK(inTile);
    MORPH_IDT_4D_CHECK(outTile);
    XI_CHECK_POINTER(params);
    XI_CHECK_TILE4D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE4D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_TILES4D_ARE_NOT_OVERLAP(inTile, outTile);
    XI_CHECK_ERROR(XI_CNN_REDUCE_GET_CONFIG(params) >= XI_CNN_REDUCE_DIM1, XI_ERR_BADARG, \
                   "\nConfig = %hhu, Incorrect Config Flag", XI_CNN_REDUCE_GET_CONFIG(params));
    XI_CHECK_ERROR(XI_CNN_REDUCE_GET_CONFIG(params) <= XI_CNN_REDUCE_DIM1234, XI_ERR_BADARG, \
                   "\nConfig = %hhu, Incorrect Config Flag", XI_CNN_REDUCE_GET_CONFIG(params));

    if ((XI_CNN_REDUCE_GET_CONFIG(params) & XI_CNN_REDUCE_DIM1) != XI_CNN_REDUCE_DIM1)
    {
      XI_CHECK_ERROR(XI_TILE4D_GET_DIM1(inTile) == XI_TILE4D_GET_DIM1(outTile), XI_ERR_DATASIZE, \
                     "\nInput tile dim1size = %d, Output tile dim1size = %d\nInequality in first dimension", \
                     XI_TILE4D_GET_DIM1(inTile), XI_TILE4D_GET_DIM1(outTile));
    }
    else
    {
      XI_CHECK_ERROR(XI_TILE4D_GET_DIM1(outTile) == 1, XI_ERR_DATASIZE, \
                     "\nOutput tile dim1size = %d, output first dimension should be 1", XI_TILE4D_GET_DIM1(outTile));
    }
    if ((XI_CNN_REDUCE_GET_CONFIG(params) & XI_CNN_REDUCE_DIM2) != XI_CNN_REDUCE_DIM2)
    {
      XI_CHECK_ERROR(XI_TILE4D_GET_DIM2(inTile) == XI_TILE4D_GET_DIM2(outTile), XI_ERR_DATASIZE, \
                     "\nInput tile dim2size = %d, Output tile dim2size = %d\nInequality in first dimension", \
                     XI_TILE4D_GET_DIM2(inTile), XI_TILE4D_GET_DIM2(outTile));
    }
    else
    {
      XI_CHECK_ERROR(XI_TILE4D_GET_DIM2(outTile) == 1, XI_ERR_DATASIZE, \
                     "\nOutput tile dim2size = %d, output second dimension should be 1", XI_TILE4D_GET_DIM2(outTile));
    }
    if ((XI_CNN_REDUCE_GET_CONFIG(params) & XI_CNN_REDUCE_DIM3) != XI_CNN_REDUCE_DIM3)
    {
      XI_CHECK_ERROR(XI_TILE4D_GET_DIM3(inTile) == XI_TILE4D_GET_DIM3(outTile), XI_ERR_DATASIZE, \
                     "\nInput tile dim3size = %d, Output tile dim3size = %d\nInequality in third dimension", \
                     XI_TILE4D_GET_DIM3(inTile), XI_TILE4D_GET_DIM3(outTile));
    }
    else
    {
      XI_CHECK_ERROR(XI_TILE4D_GET_DIM3(outTile) == 1, XI_ERR_DATASIZE, \
                     "\nOutput tile dim3size = %d, output third dimension should be 1", XI_TILE4D_GET_DIM3(outTile));
    }
    if ((XI_CNN_REDUCE_GET_CONFIG(params) & XI_CNN_REDUCE_DIM4) != XI_CNN_REDUCE_DIM4)
    {
      XI_CHECK_ERROR(XI_TILE4D_GET_DIM4(inTile) == XI_TILE4D_GET_DIM4(outTile), XI_ERR_DATASIZE, \
                     "\nInput tile dim4size = %d, Output tile dim4size = %d\nInequality in fourth dimension", \
                     XI_TILE4D_GET_DIM4(inTile), XI_TILE4D_GET_DIM4(outTile));
    }
    else
    {
      XI_CHECK_ERROR(XI_TILE4D_GET_DIM4(outTile) == 1, XI_ERR_DATASIZE, \
                     "\nOutput tile dim3size = %d, output fourth dimension should be 1", XI_TILE4D_GET_DIM4(outTile));
    }
    XI_CHECK_ERROR(XI_CNN_REDUCE_GET_TILEFLAG(params) <= 1, XI_ERR_BADARG, \
                   "\nTile Flag = %hhu, Incorrect Tile Flag", XI_CNN_REDUCE_GET_TILEFLAG(params));
  }
  /*Tile Parameters*/
  const int32_t inputDim1     = XI_TILE4D_GET_DIM1(inTile);
  const int32_t inputDim2     = XI_TILE4D_GET_DIM2(inTile);
  const int32_t inputDim3     = XI_TILE4D_GET_DIM3(inTile);
  const int32_t inputDim4     = XI_TILE4D_GET_DIM4(inTile);
  const int32_t outDataPitch1 = XI_TILE4D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE4D_GET_DIM2_PITCH(outTile);
  const int32_t outDataPitch3 = XI_TILE4D_GET_DIM3_PITCH(outTile);
  const int32_t inDataPitch1  = XI_TILE4D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2  = XI_TILE4D_GET_DIM2_PITCH(inTile);
  const int32_t inDataPitch3  = XI_TILE4D_GET_DIM3_PITCH(inTile);

  /*Initialize Data Pointers for Input and Output Tiles*/
  MORPH_IDT_SCALAR *pInData  = (MORPH_IDT_SCALAR *)XI_TILE4D_GET_DATA_PTR(inTile);
  MORPH_IDT_SCALAR *pOutData = (MORPH_IDT_SCALAR *)XI_TILE4D_GET_DATA_PTR(outTile);

  /*Reduce parameter*/
  const int32_t config   = XI_CNN_REDUCE_GET_CONFIG(params);
  const uint8_t tileFlag = XI_CNN_REDUCE_GET_TILEFLAG(params); // 1-> first/first-last tile, 0-> intermediate or final tile

  /* vectorization width */
  const int32_t vectorizationWidth = 2 * XCHAL_IVPN_SIMD_WIDTH;

  /* Input and Output data pointers */
  MORPH_IDT_2Nx8 * restrict pdvecIn1;
  MORPH_IDT_2Nx8 * restrict pdvecIn2;
  MORPH_IDT_2Nx8 * restrict pdvecOut1;
  MORPH_IDT_2Nx8 * restrict pdvecOut2;

  /* Input and Output data vectors */
  MORPH_IDT_2Nx8 dvecInData1;
  MORPH_IDT_2Nx8 dvecInData2;
  MORPH_IDT_2Nx8 dvecMax1;
  MORPH_IDT_2Nx8 dvecMax2;

  /*Loop indices*/
  int32_t n, z, y, x;
  int32_t tileSize1, tileSize2, tileSize3;
  int32_t inP1, inP2, inP3;
  int32_t outP2, outP3;
  MORPH_IDT_SCALAR max1, max2;

  valign vaOut1  = IVP_ZALIGN();
  valign vaOut2 = IVP_ZALIGN();

  switch(config)
  {
    case XI_CNN_REDUCE_DIM2  : tileSize1 = inputDim2;
                               tileSize2 = inputDim3;
                               tileSize3 = inputDim4;
                               inP1      = inDataPitch1;
                               inP2      = inDataPitch2;
                               inP3      = inDataPitch3;
                               outP2     = outDataPitch2;
                               outP3     = outDataPitch3;
                               break;
    case XI_CNN_REDUCE_DIM3  : tileSize1 = inputDim3;
                               tileSize2 = inputDim2;
                               tileSize3 = inputDim4;
                               inP1      = inDataPitch2;
                               inP2      = inDataPitch1;
                               inP3      = inDataPitch3;
                               outP2     = outDataPitch1;
                               outP3     = outDataPitch3;
                               break;
    case XI_CNN_REDUCE_DIM4  : tileSize1 = inputDim4;
                               tileSize2 = inputDim2;
                               tileSize3 = inputDim3;
                               inP1      = inDataPitch3;
                               inP2      = inDataPitch1;
                               inP3      = inDataPitch2;
                               outP2     = outDataPitch1;
                               outP3     = outDataPitch2;
                               break;
    case XI_CNN_REDUCE_DIM13 : tileSize1 = inputDim3;
                               tileSize2 = inputDim2;
                               tileSize3 = inputDim4;
                               inP1      = inDataPitch2;
                               inP2      = inDataPitch1;
                               inP3      = inDataPitch3;
                               outP2     = outDataPitch1;
                               outP3     = outDataPitch3;
                               break;
    case XI_CNN_REDUCE_DIM14 : tileSize1 = inputDim4;
                               tileSize2 = inputDim2;
                               tileSize3 = inputDim3;
                               inP1      = inDataPitch3;
                               inP2      = inDataPitch1;
                               inP3      = inDataPitch2;
                               outP2     = outDataPitch1;
                               outP3     = outDataPitch2;
                               break;
    case XI_CNN_REDUCE_DIM23 : tileSize1 = inputDim2;
                               tileSize2 = inputDim3;
                               tileSize3 = inputDim4;
                               inP1      = inDataPitch1;
                               inP2      = inDataPitch2;
                               inP3      = inDataPitch3;
                               outP2     = 0;
                               outP3     = outDataPitch3;
                               break;
    case XI_CNN_REDUCE_DIM24 : tileSize1 = inputDim2;
                               tileSize2 = inputDim4;
                               tileSize3 = inputDim3;
                               inP1      = inDataPitch1;
                               inP2      = inDataPitch3;
                               inP3      = inDataPitch2;
                               outP2     = 0;
                               outP3     = outDataPitch2;
                               break;
    case XI_CNN_REDUCE_DIM34 : tileSize1 = inputDim3;
                               tileSize2 = inputDim4;
                               tileSize3 = inputDim2;
                               inP1      = inDataPitch2;
                               inP2      = inDataPitch3;
                               inP3      = inDataPitch1;
                               outP2     = 0;
                               outP3     = outDataPitch1;
                               break;
    default: tileSize1 = 0;
             tileSize2 = 0;
             tileSize3 = 0;
             inP1      = 0;
             inP2      = 0;
             inP3      = 0;
             outP2     = 0;
             outP3     = 0;
             break;
  }

  if(config == XI_CNN_REDUCE_DIM1)
  {
    for(n = 0; n < inputDim4; n++)
    {
      for(z = 0; z < inputDim3; z++)
      {
        for(y = 0; y < inputDim2; y += 2)
        {
          int32_t remY = XT_MIN(inputDim2 - y, 2) - 1; // handling corner cases
          if (tileFlag == XI_CNN_REDUCE_FIRST_TILE)
          {
            dvecMax1  = MORPH_MINVALUE;
            dvecMax2  = MORPH_MINVALUE;
          }
          else
          {
            dvecMax1 = pOutData[y * outDataPitch1 + z * outDataPitch2 + n * outDataPitch3];
            dvecMax2 = pOutData[(y + remY) * outDataPitch1 + z * outDataPitch2 + n * outDataPitch3];
          }
          /* input data pointers */
          pdvecIn1 = (MORPH_IDT_2Nx8 *)(pInData + y * inDataPitch1 + z * inDataPitch2 + n * inDataPitch3);
          pdvecIn2 = (MORPH_IDT_2Nx8 *)(pInData + (y + remY) * inDataPitch1 + z * inDataPitch2 + n * inDataPitch3);
          valign vaInData1 = MORPH_LA2NX8_PP(pdvecIn1);
          valign vaInData2 = MORPH_LA2NX8_PP(pdvecIn2);
          /* Macro for Loading the input and finding the maximum value */
          MORPH_CALCULATE_MAX(pdvecIn1, pdvecIn2, dvecMax1, dvecMax2, inputDim1);
          /* Find the maximum value */
          MORPH_RMAX2NX8(dvecMax1, max1);
          MORPH_RMAX2NX8(dvecMax2, max2);
          /* Store the result */
          pOutData[y * outDataPitch1 + z * outDataPitch2 + n * outDataPitch3] = max1;
          pOutData[(y + remY) * outDataPitch1 + z * outDataPitch2 + n * outDataPitch3] = max2;
        }
      }
    }
  }
  else if((config == XI_CNN_REDUCE_DIM2) || (config == XI_CNN_REDUCE_DIM3) || (config == XI_CNN_REDUCE_DIM4))
  {
    for( x = 0; x < inputDim1; x+= vectorizationWidth)
    {
      for(n = 0; n < tileSize3; n++)
      {
        for(z = 0; z < tileSize2; z+= 2)
        {
          int32_t remZ = XT_MIN(2, (tileSize2 - z)) - 1;
          /* Input and Output data pointers */
          pdvecIn1  = (MORPH_IDT_2Nx8 *)(pInData  + x + z * inP2  + n * inP3);
          pdvecIn2  = (MORPH_IDT_2Nx8 *)(pInData  + x + (z + remZ) * inP2  + n * inP3);
          pdvecOut1 = (MORPH_IDT_2Nx8 *)(pOutData + x + z * outP2 + n * outP3);
          pdvecOut2 = (MORPH_IDT_2Nx8 *)(pOutData + x + (z + remZ) * outP2 + n * outP3);

          if (tileFlag == XI_CNN_REDUCE_FIRST_TILE)
          {
            dvecMax1 = MORPH_MINVALUE;
            dvecMax2 = MORPH_MINVALUE;
          }
          else
          {
            valign vaOutData1 = MORPH_LA2NX8_PP(pdvecOut1);
            valign vaOutData2 = MORPH_LA2NX8_PP(pdvecOut2);
            MORPH_LA2NX8_IP(dvecMax1, vaOutData1, pdvecOut1);
            MORPH_LA2NX8_IP(dvecMax2, vaOutData2, pdvecOut2);
          }
          pdvecOut1 = (MORPH_IDT_2Nx8 *)(pOutData + x + z * outP2 + n * outP3);
          pdvecOut2 = (MORPH_IDT_2Nx8 *)(pOutData + x + (z + remZ) * outP2 + n * outP3);

          for(y = 0; y < tileSize1; y++)
          {
            /* Load the data */
            valign vaInData1 = MORPH_LA2NX8_PP(pdvecIn1);
            MORPH_LA2NX8_XP(dvecInData1, vaInData1, pdvecIn1, inP1);
            /* Load the data */
            valign vaInData2  = MORPH_LA2NX8_PP(pdvecIn2);
            MORPH_LA2NX8_XP(dvecInData2, vaInData2, pdvecIn2, inP1);
            /* Find the maximum value */
            dvecMax1 = MORPH_MAX2NX8(dvecMax1, dvecInData1);
            dvecMax2 = MORPH_MAX2NX8(dvecMax2, dvecInData2);
          }
          /* Store the result */
          MORPH_SAV2NX8_XP(dvecMax1, vaOut1, pdvecOut1, (inputDim1 - x));
          MORPH_SAPOS2NX8_FP(vaOut1, pdvecOut1);
          MORPH_SAV2NX8_XP(dvecMax2, vaOut2, pdvecOut2, (inputDim1 - x)* remZ);
          MORPH_SAPOS2NX8_FP(vaOut2, pdvecOut2);
        }
      }
    }
  }
  else if(config == XI_CNN_REDUCE_DIM12)
  {
    /******************************************************************************/
    /* Data exist in contiguous memory location with respect to first dimension   */
    /******************************************************************************/
    if (inDataPitch1 == inputDim1)
    {
      int32_t maxLoopCount = inputDim1 * inputDim2;
      for(n = 0; n < inputDim4; n++)
      {
        for(z = 0; z < inputDim3; z++)
        {
          if (tileFlag == XI_CNN_REDUCE_FIRST_TILE)
          {
            dvecMax1 = MORPH_MINVALUE;
          }
          else
          {
            dvecMax1 = pOutData[z * outDataPitch2 + n * outDataPitch3];
          }
          /* Input data pointer */
          pdvecIn1  = (MORPH_IDT_2Nx8 *)(pInData + z * inDataPitch2 + n * inDataPitch3 );
          valign vaInData  = MORPH_LA2NX8_PP(pdvecIn1);

          for (x = 0; x <= (maxLoopCount - vectorizationWidth); x += vectorizationWidth)
          {
            /* Load the data */
            MORPH_LA2NX8_IP(dvecInData1, vaInData, pdvecIn1);
            /* Find the maximum value */
            dvecMax1 = MORPH_MAX2NX8(dvecMax1, dvecInData1);
          }
          if (x < maxLoopCount)
          {
            int32_t remX = maxLoopCount - x;
            vbool2N vbMask = IVP_LTU2NX8U(IVP_SEQ2NX8(), remX);
            /* Load the data */
            MORPH_LAV2NX8_XP(dvecInData1, vaInData, pdvecIn1, remX);
            dvecInData1 = MORPH_MOV2NX8T(dvecInData1, MORPH_MINVALUE, vbMask);
            /* Find the maximum value */
            dvecMax1 = MORPH_MAX2NX8(dvecMax1, dvecInData1);
          }
          /* Find the maximum value */
          MORPH_RMAX2NX8(dvecMax1, max1);
          /* Store the result */
          pOutData[z * outDataPitch2 + n * outDataPitch3] = max1;
        }
      }
    }
    else
    {
      for(n = 0; n < inputDim4; n++)
      {
        for (z = 0; z < inputDim3; z+= 2)
        {
          int32_t remZ = XT_MIN(2, (inputDim3 - z)) - 1;
          if (tileFlag == XI_CNN_REDUCE_FIRST_TILE)
          {
            dvecMax1 = MORPH_MINVALUE;
            dvecMax2 = MORPH_MINVALUE;
          }
          else
          {
            dvecMax1 = pOutData[z * outDataPitch2 + n * outDataPitch3];
            dvecMax2 = pOutData[(z + remZ) * outDataPitch2 + n * outDataPitch3];
          }
          for (x = 0; x < inputDim1; x += vectorizationWidth)
          {
            /* Input data pointer */
            pdvecIn1 = (MORPH_IDT_2Nx8 *)(pInData + x + z * inDataPitch2 + n * inDataPitch3);
            pdvecIn2 = (MORPH_IDT_2Nx8 *)(pInData + x + (z + remZ) * inDataPitch2 + n * inDataPitch3);

            int32_t remX = XT_MIN(vectorizationWidth, (inputDim1 - x));
            vbool2N vbMask = IVP_LTU2NX8U(IVP_SEQ2NX8(), remX);
            for (y = 0; y < inputDim2; y++)
            {
              /* Load the data */
              valign vaInData1 = MORPH_LA2NX8_PP(pdvecIn1);
              MORPH_LA2NX8_XP(dvecInData1, vaInData1, pdvecIn1, inDataPitch1);
              valign vaInData2 = MORPH_LA2NX8_PP(pdvecIn2);
              MORPH_LA2NX8_XP(dvecInData2, vaInData2, pdvecIn2, inDataPitch1);
              dvecInData1 = MORPH_MOV2NX8T(dvecInData1, MORPH_MINVALUE, vbMask);
              dvecInData2 = MORPH_MOV2NX8T(dvecInData2, MORPH_MINVALUE, vbMask);
              /* Find the maximum value */
              dvecMax1 = MORPH_MAX2NX8(dvecMax1, dvecInData1);
              dvecMax2 = MORPH_MAX2NX8(dvecMax2, dvecInData2);
            }
          }
          /* Find the maximum value */
          MORPH_RMAX2NX8(dvecMax1, max1);
          MORPH_RMAX2NX8(dvecMax2, max2);
          /* Store the result */
          pOutData[z * outDataPitch2 + n * outDataPitch3] = max1;
          pOutData[(z + remZ) * outDataPitch2 + n * outDataPitch3] = max2;
        }
      }
    }
  }
  else if((config == XI_CNN_REDUCE_DIM13) || (config == XI_CNN_REDUCE_DIM14))
  {
    for(n = 0; n < tileSize3; n++)
    {
      for(z = 0; z < tileSize2; z+= 2)
      {
        int32_t remZ = XT_MIN(2, (tileSize2 - z)) - 1;
        if (tileFlag == XI_CNN_REDUCE_FIRST_TILE)
        {
          dvecMax1 = MORPH_MINVALUE;
          dvecMax2 = MORPH_MINVALUE;
        }
        else
        {
          dvecMax1 = pOutData[z * outP2 + n * outP3];
          dvecMax2 = pOutData[(z + remZ) * outP2 + n * outP3];
        }
        for( x = 0; x < inputDim1; x+= vectorizationWidth)
        {
          /* Input and Output data pointers */
          pdvecIn1 = (MORPH_IDT_2Nx8 *)(pInData  + x + z * inP2  + n * inP3);
          pdvecIn2 = (MORPH_IDT_2Nx8 *)(pInData  + x + (z + remZ) * inP2  + n * inP3);
          int32_t remX   = XT_MIN(vectorizationWidth, (inputDim1 - x));
          vbool2N vbMask = IVP_LTU2NX8U(IVP_SEQ2NX8(), remX);
          for(y = 0; y < tileSize1; y++)
          {
            /* Load the data */
            valign vaInData1 = MORPH_LA2NX8_PP(pdvecIn1);
            MORPH_LA2NX8_XP(dvecInData1, vaInData1, pdvecIn1, inP1);
            valign vaInData2 = MORPH_LA2NX8_PP(pdvecIn2);
            MORPH_LA2NX8_XP(dvecInData2, vaInData2, pdvecIn2, inP1);
            dvecInData1 = MORPH_MOV2NX8T(dvecInData1, MORPH_MINVALUE, vbMask);
            dvecInData2 = MORPH_MOV2NX8T(dvecInData2, MORPH_MINVALUE, vbMask);
            /* Find the maximum value */
            dvecMax1 = MORPH_MAX2NX8(dvecMax1, dvecInData1);
            dvecMax2 = MORPH_MAX2NX8(dvecMax2, dvecInData2);
          }
        }
        /* Find the maximum value */
        MORPH_RMAX2NX8(dvecMax1, max1);
        MORPH_RMAX2NX8(dvecMax2, max2);
        /* store the result */
        pOutData[z * outP2 + n * outP3] = max1;
        pOutData[(z + remZ) * outP2 + n * outP3] = max2;
      }
    }
  }
  else if((config == XI_CNN_REDUCE_DIM23) || (config == XI_CNN_REDUCE_DIM24)|| (config == XI_CNN_REDUCE_DIM34))
  {
    for(x = 0; x < inputDim1; x+= vectorizationWidth)
    {
      for(n = 0; n < tileSize3; n+= 2)
      {
        int32_t remN =  XT_MIN(2, (tileSize3 - n)) - 1;

        /* Output data pointers */
        pdvecOut1 = (MORPH_IDT_2Nx8 *)(pOutData + x + n * outP3);
        pdvecOut2 = (MORPH_IDT_2Nx8 *)(pOutData + x + (n + remN) * outP3);
        if (tileFlag == XI_CNN_REDUCE_FIRST_TILE)
        {
          dvecMax1 = MORPH_MINVALUE;
          dvecMax2 = MORPH_MINVALUE;
        }
        else
        {
          valign vaOutData1 = MORPH_LA2NX8_PP(pdvecOut1);
          MORPH_LA2NX8_IP(dvecMax1, vaOutData1, pdvecOut1);
          valign vaOutData2 = MORPH_LA2NX8_PP(pdvecOut2);
          MORPH_LA2NX8_IP(dvecMax2, vaOutData2, pdvecOut2);
        }
        pdvecOut1 = (MORPH_IDT_2Nx8 *)(pOutData + x + n * outP3);
        pdvecOut2 = (MORPH_IDT_2Nx8 *)(pOutData + x + (n + remN) * outP3);
        for(z = 0; z < tileSize2; z++)
        {
          /* Input data pointers */
          pdvecIn1 = (MORPH_IDT_2Nx8 *)(pInData  + x + z * inP2  + n * inP3);
          pdvecIn2 = (MORPH_IDT_2Nx8 *)(pInData  + x + z * inP2  + (n + remN) * inP3);
          for(y = 0; y < tileSize1; y++)
          {
            /* Load the data */
            valign vaInData1 = MORPH_LA2NX8_PP(pdvecIn1);
            MORPH_LA2NX8_XP(dvecInData1, vaInData1, pdvecIn1, inP1);
            valign vaInData2 = MORPH_LA2NX8_PP(pdvecIn2);
            MORPH_LA2NX8_XP(dvecInData2, vaInData2, pdvecIn2, inP1);
            /* Find the maximum value */
            dvecMax1 = MORPH_MAX2NX8(dvecMax1, dvecInData1);
            dvecMax2 = MORPH_MAX2NX8(dvecMax2, dvecInData2);
          }
        }
        /* store the result */
        MORPH_SAV2NX8_XP(dvecMax1, vaOut1, pdvecOut1, (inputDim1 - x));
        MORPH_SAPOS2NX8_FP(vaOut1, pdvecOut1);
        MORPH_SAV2NX8_XP(dvecMax2, vaOut2, pdvecOut2, (inputDim1 - x));
        MORPH_SAPOS2NX8_FP(vaOut2, pdvecOut2);
      }
    }
  }
  else if(config == XI_CNN_REDUCE_DIM123)
  {
    /******************************************************************************/
    /* Data exist in contiguous memory location with respect to first dimension   */
    /******************************************************************************/
    if (inDataPitch1 == inputDim1)
    {
      /* Initialize max loop counter */
      int32_t maxLoopCount = inputDim1 * inputDim2;
      int32_t dim3Size = inputDim3;
      /* Updated Loop count based on tile dimension configuration */
      if (inDataPitch2 == maxLoopCount)
      {
        /**********************************************************************/
        /* Data exist in contiguous memory location with respect to first and */
        /* second dimension                                                   */
        /**********************************************************************/
        dim3Size = 1;       /* Update max loop counter */
        maxLoopCount *= inputDim3;
      }
      for(n = 0; n < inputDim4; n++)
      {
        if (tileFlag == XI_CNN_REDUCE_FIRST_TILE)
        {
          dvecMax1 = MORPH_MINVALUE;
        }
        else
        {
          dvecMax1 = pOutData[n * outDataPitch3];
        }
        for(z = 0; z < dim3Size; z++)
        {
          /* Input data pointer */
          pdvecIn1 = (MORPH_IDT_2Nx8 *)(pInData + z * inDataPitch2 + n * inDataPitch3);
          valign vaInData = MORPH_LA2NX8_PP(pdvecIn1);
          for (x = 0; x <= (maxLoopCount - vectorizationWidth); x += vectorizationWidth)
          {
            /* Load the data */
            MORPH_LA2NX8_IP(dvecInData1, vaInData, pdvecIn1);
            dvecMax1 = MORPH_MAX2NX8(dvecMax1, dvecInData1);
          }
          if (x < maxLoopCount)
          {
            int32_t remX = (maxLoopCount - x);
            vbool2N vbMask = IVP_LTU2NX8U(IVP_SEQ2NX8(), remX);
            /* Load the data */
            MORPH_LAV2NX8_XP(dvecInData1, vaInData, pdvecIn1, remX);
            dvecInData1 = MORPH_MOV2NX8T(dvecInData1, MORPH_MINVALUE, vbMask);
            /* Find the maximum value */
            dvecMax1 = MORPH_MAX2NX8(dvecMax1, dvecInData1);
          }
        }
        /* Find the maximum value */
        MORPH_RMAX2NX8(dvecMax1, max1);
        /* Store the result */
        pOutData[n * outDataPitch3] = max1;
      }
    }
    else
    {
      for(n = 0; n < inputDim4; n+= 2)
      {
        int32_t remN = XT_MIN(2, (inputDim4 - n)) - 1;
        if (tileFlag == XI_CNN_REDUCE_FIRST_TILE)
        {
          dvecMax1 = MORPH_MINVALUE;
          dvecMax2 = MORPH_MINVALUE;
        }
        else
        {
          dvecMax1 = pOutData[n * outDataPitch3];
          dvecMax2 = pOutData[(n + remN) * outDataPitch3];
        }
        for (x = 0; x < inputDim1; x += vectorizationWidth)
        {
          int32_t remX   = XT_MIN(vectorizationWidth, (inputDim1 - x));
          vbool2N vbMask = IVP_LTU2NX8U(IVP_SEQ2NX8(), remX);
          for (z = 0; z < inputDim3; z++)
          {
            /* Input data pointer */
            pdvecIn1 = (MORPH_IDT_2Nx8 *)(pInData + x + z * inDataPitch2 + n * inDataPitch3);
            pdvecIn2 = (MORPH_IDT_2Nx8 *)(pInData + x + z * inDataPitch2 + (n + remN)* inDataPitch3);
            for (y = 0; y < inputDim2; y++)
            {
              /* Load the data */
              valign vaInData1 = MORPH_LA2NX8_PP(pdvecIn1);
              MORPH_LA2NX8_XP(dvecInData1, vaInData1, pdvecIn1, inDataPitch1);
              valign vaInData2 = MORPH_LA2NX8_PP(pdvecIn2);
              MORPH_LA2NX8_XP(dvecInData2, vaInData2, pdvecIn2, inDataPitch1);
              dvecInData1 = MORPH_MOV2NX8T(dvecInData1, MORPH_MINVALUE, vbMask);
              dvecInData2 = MORPH_MOV2NX8T(dvecInData2, MORPH_MINVALUE, vbMask);
              dvecMax1 = MORPH_MAX2NX8(dvecMax1, dvecInData1);
              dvecMax2 = MORPH_MAX2NX8(dvecMax2, dvecInData2);
            }
          }
        }
        /* Find the maximum value */
        MORPH_RMAX2NX8(dvecMax1, max1);
        MORPH_RMAX2NX8(dvecMax2, max2);
        /* Store the result */
        pOutData[n * outDataPitch3] = max1;
        pOutData[(n + remN) * outDataPitch3] = max2;
      }
    }
  }
  else if(config == XI_CNN_REDUCE_DIM124)
  {
    /******************************************************************************/
    /* Data exist in contiguous memory location with respect to first dimension   */
    /******************************************************************************/
    if (inDataPitch1 == inputDim1)
    {
      /* Initialize max loop counter */
      int32_t maxLoopCount = inputDim1 * inputDim2;
      for(z = 0; z < inputDim3; z++)
      {
        if (tileFlag == XI_CNN_REDUCE_FIRST_TILE)
        {
          dvecMax1 = MORPH_MINVALUE;
        }
        else
        {
          dvecMax1 = pOutData[z * outDataPitch2];
        }
        for(n = 0; n < inputDim4; n++)
        {
          /* Input data pointer */
          pdvecIn1 = (MORPH_IDT_2Nx8 *)(pInData + z * inDataPitch2 + n * inDataPitch3);
          valign vaInData  = MORPH_LA2NX8_PP(pdvecIn1);
          for (x = 0; x <= (maxLoopCount - vectorizationWidth); x += vectorizationWidth)
          {
            /* Load the data */
            MORPH_LA2NX8_IP(dvecInData1, vaInData, pdvecIn1);
            dvecMax1 = MORPH_MAX2NX8(dvecMax1, dvecInData1);
          }
          if (x < maxLoopCount)
          {
            int32_t remX = (maxLoopCount - x);
            vbool2N vbMask = IVP_LTU2NX8U(IVP_SEQ2NX8(), remX);
            /* Load the data */
            MORPH_LAV2NX8_XP(dvecInData1, vaInData, pdvecIn1, remX);
            dvecInData1 = MORPH_MOV2NX8T(dvecInData1, MORPH_MINVALUE, vbMask);
            /* Find the maximum value */
            dvecMax1 = MORPH_MAX2NX8(dvecMax1, dvecInData1);
          }
        }
        /* Find the maximum value */
        MORPH_RMAX2NX8(dvecMax1, max1);
        /* Store the result */
        pOutData[z * outDataPitch2] = max1;
      }
    }
    else
    {
      for(z = 0; z < inputDim3; z+= 2)
      {
        int32_t remZ = XT_MIN(2, (inputDim3 - z)) - 1;
        if (tileFlag == XI_CNN_REDUCE_FIRST_TILE)
        {
          dvecMax1 = MORPH_MINVALUE;
          dvecMax2 = MORPH_MINVALUE;
        }
        else
        {
          dvecMax1 = pOutData[z * outDataPitch2];
          dvecMax2 = pOutData[(z + remZ) * outDataPitch2];
        }
        for(x = 0; x < inputDim1; x+= vectorizationWidth)
        {
          int32_t remX   = XT_MIN(vectorizationWidth, (inputDim1 - x));
          vbool2N vbMask = IVP_LTU2NX8U(IVP_SEQ2NX8(), remX);
          for(n = 0; n < inputDim4; n++)
          {
            /* Input data pointer */
            pdvecIn1 = (MORPH_IDT_2Nx8 *)(pInData + x + z * inDataPitch2 + n * inDataPitch3);
            pdvecIn2 = (MORPH_IDT_2Nx8 *)(pInData + x + (z + remZ) * inDataPitch2 + n * inDataPitch3);
            for(y = 0; y < inputDim2; y++)
            {
              /* Load the data */
              valign vaInData1 = MORPH_LA2NX8_PP(pdvecIn1);
              MORPH_LA2NX8_XP(dvecInData1, vaInData1, pdvecIn1, inDataPitch1);
              valign vaInData2 = MORPH_LA2NX8_PP(pdvecIn2);
              MORPH_LA2NX8_XP(dvecInData2, vaInData2, pdvecIn2, inDataPitch1);
              /* Find the maximum value */
              dvecInData1 = MORPH_MOV2NX8T(dvecInData1, MORPH_MINVALUE, vbMask);
              dvecInData2 = MORPH_MOV2NX8T(dvecInData2, MORPH_MINVALUE, vbMask);
              dvecMax1    = MORPH_MAX2NX8(dvecMax1, dvecInData1);
              dvecMax2    = MORPH_MAX2NX8(dvecMax2, dvecInData2);
            }
          }
        }
        /* Find the maximum value */
        MORPH_RMAX2NX8(dvecMax1, max1);
        MORPH_RMAX2NX8(dvecMax2, max2);
        /* Store the result */
        pOutData[z * outDataPitch2] = max1;
        pOutData[(z + remZ) * outDataPitch2] = max2;
      }
    }
  }
  else if(config == XI_CNN_REDUCE_DIM134)
  {
    for(y = 0; y < inputDim2; y+= 2)
    {
      int32_t remY =  XT_MIN(2, (inputDim2 - y)) - 1;
      if (tileFlag == XI_CNN_REDUCE_FIRST_TILE)
      {
        dvecMax1 = MORPH_MINVALUE;
        dvecMax2 = MORPH_MINVALUE;
      }
      else
      {
        dvecMax1 = pOutData[y * outDataPitch1];
        dvecMax2 = pOutData[(y + remY) * outDataPitch1];
      }
      for(x = 0; x < inputDim1; x+= vectorizationWidth)
      {
        int32_t remX   = XT_MIN(vectorizationWidth, (inputDim1 - x));
        vbool2N vbMask = IVP_LTU2NX8U(IVP_SEQ2NX8(), remX);
        for(n = 0; n < inputDim4; n++)
        {
          /* Input data pointer */
          pdvecIn1 = (MORPH_IDT_2Nx8 *)(pInData + x + y * inDataPitch1 + n * inDataPitch3);
          pdvecIn2 = (MORPH_IDT_2Nx8 *)(pInData + x + (y + remY) * inDataPitch1 + n * inDataPitch3);
          for(z = 0; z < inputDim3; z++)
          {
            /* Load the data */
            valign vaInData1  = MORPH_LA2NX8_PP(pdvecIn1);
            MORPH_LA2NX8_XP(dvecInData1, vaInData1, pdvecIn1, inDataPitch2);
            valign vaInData2 = MORPH_LA2NX8_PP(pdvecIn2);
            MORPH_LA2NX8_XP(dvecInData2, vaInData2, pdvecIn2, inDataPitch2);
            dvecInData1 = MORPH_MOV2NX8T(dvecInData1, MORPH_MINVALUE, vbMask);
            dvecInData2 = MORPH_MOV2NX8T(dvecInData2, MORPH_MINVALUE, vbMask);
            /* Find the maximum */
            dvecMax1 = MORPH_MAX2NX8(dvecMax1, dvecInData1);
            dvecMax2 = MORPH_MAX2NX8(dvecMax2, dvecInData2);
          }
        }
      }
      /* Find the maximum value */
      MORPH_RMAX2NX8(dvecMax1, max1);
      MORPH_RMAX2NX8(dvecMax2, max2);
      /* Store the result */
      pOutData[y * outDataPitch1] = max1;
      pOutData[(y + remY) * outDataPitch1] = max2;
    }
  }
  else if(config == XI_CNN_REDUCE_DIM234)
  {
    for(x = 0; x < inputDim1; x+= vectorizationWidth)
    {
      /* Input and Output data pointers */
      pdvecOut1 = (MORPH_IDT_2Nx8 *)(pOutData + x );
      if (tileFlag == XI_CNN_REDUCE_FIRST_TILE)
      {
        dvecMax1 = MORPH_MINVALUE;
      }
      else
      {
        valign vaOutData = MORPH_LA2NX8_PP(pdvecOut1);
        MORPH_LA2NX8_IP(dvecMax1, vaOutData, pdvecOut1);
      }
      pdvecOut1 = (MORPH_IDT_2Nx8 *)(pOutData + x);
      dvecMax2 = dvecMax1;
      for(n = 0; n < inputDim4; n++)
      {
        for(z = 0; z < inputDim3; z+= 2)
        {
          int32_t remZ = XT_MIN(2, (inputDim3 - z)) - 1;
          /* Input data pointer */
          pdvecIn1 = (MORPH_IDT_2Nx8 *)(pInData + x + z * inDataPitch2 + n * inDataPitch3);
          pdvecIn2 = (MORPH_IDT_2Nx8 *)(pInData + x + (z + remZ) * inDataPitch2 + n * inDataPitch3);
          for(y = 0; y < inputDim2; y++)
          {
            /* Load the data */
            valign vaInData1  = MORPH_LA2NX8_PP(pdvecIn1);
            MORPH_LA2NX8_XP(dvecInData1, vaInData1, pdvecIn1, inDataPitch1);
            valign vaInData2  = MORPH_LA2NX8_PP(pdvecIn2);
            MORPH_LA2NX8_XP(dvecInData2, vaInData2, pdvecIn2, inDataPitch1);
            /* Find the maximum value */
            dvecMax1 = MORPH_MAX2NX8(dvecMax1, dvecInData1);
            dvecMax2 = MORPH_MAX2NX8(dvecMax2, dvecInData2);
          }
        }
      }
      /* store the result */
      dvecMax1 = MORPH_MAX2NX8(dvecMax1, dvecMax2);
      MORPH_SAV2NX8_XP(dvecMax1, vaOut1, pdvecOut1, (inputDim1 - x));
      MORPH_SAPOS2NX8_FP(vaOut1, pdvecOut1);
    }
  }
  else if(config == XI_CNN_REDUCE_DIM1234)
  {
    if (tileFlag == XI_CNN_REDUCE_FIRST_TILE)
    {
      dvecMax1 = MORPH_MINVALUE;
    }
    else
    {
      dvecMax1 = pOutData[0];
    }
    /******************************************************************************/
    /* Data exist in contiguous memory location with respect to first dimension   */
    /******************************************************************************/
    if (inDataPitch1 == inputDim1)
    {
      /* Initialize max loop counter */
      int32_t dim3Size = inputDim3;
      int32_t dim4Size = inputDim4;
      int32_t maxLoopCount = inputDim2*inputDim1;
      /* Updated Loop count based on tile dimension configuration */
      if (inDataPitch2 == maxLoopCount)
      {
        /**********************************************************************/
        /* Data exist in contiguous memory location with respect to first and */
        /* second dimension                                                   */
        /**********************************************************************/
        dim3Size = 1;       /* Update max loop counter */
        maxLoopCount *= inputDim3;
      }
      if (inDataPitch3 == maxLoopCount)
      {
        /**********************************************************************/
        /* Data exist in contiguous memory location with respect to first,    */
        /* second and third dimension                                         */
        /**********************************************************************/
        dim4Size = 1;       /* Update max loop counter */
        maxLoopCount *= inputDim4;
      }
      for (n = 0; n < dim4Size; n++)
      {
        for (z = 0; z < dim3Size; z++)
        {
          /* Input data pointer */
          pdvecIn1 = (MORPH_IDT_2Nx8 *)(pInData + z * inDataPitch2 + n * inDataPitch3);
          valign vaInData = MORPH_LA2NX8_PP(pdvecIn1);
          for (x = 0; x <= (maxLoopCount - vectorizationWidth); x += vectorizationWidth)
          {
            /* Load the data */
            MORPH_LA2NX8_IP(dvecInData1, vaInData, pdvecIn1);
            /* Find the maximum value */
            dvecMax1 = MORPH_MAX2NX8(dvecInData1, dvecMax1);
          }
          if (x < maxLoopCount)
          {
            int32_t remX = (maxLoopCount - x);
            vbool2N vbMask = IVP_LTU2NX8U(IVP_SEQ2NX8(), remX);
            /* Load the data */
            MORPH_LAV2NX8_XP(dvecInData1, vaInData, pdvecIn1, remX);
            dvecInData1 = MORPH_MOV2NX8T(dvecInData1, MORPH_MINVALUE, vbMask);
            /* Find the maximum value */
            dvecMax1 = MORPH_MAX2NX8(dvecMax1, dvecInData1);
          }
        }
      }
    }
    else
    {
       dvecMax2  = dvecMax1;
      for (x = 0; x < inputDim1; x += vectorizationWidth)
      {
        int32_t remX   = XT_MIN(vectorizationWidth, (inputDim1 - x));
        vbool2N vbMask = IVP_LTU2NX8U(IVP_SEQ2NX8(), remX);
        for(n = 0; n < inputDim4; n++)
        {
          for (z = 0; z < inputDim3; z+= 2)
          {
            int32_t remZ = XT_MIN(2, (inputDim3 - z)) - 1;
            /* Input data pointer */
            pdvecIn1 = (MORPH_IDT_2Nx8 *)(pInData + x + z * inDataPitch2 + n * inDataPitch3);
            pdvecIn2 = (MORPH_IDT_2Nx8 *)(pInData + x + (z + remZ) * inDataPitch2 + n * inDataPitch3);
            for (y = 0; y < inputDim2; y++)
            {
              /* Load the data */
              valign vaInData1 = MORPH_LA2NX8_PP(pdvecIn1);
              valign vaInData2 = MORPH_LA2NX8_PP(pdvecIn2);
              MORPH_LA2NX8_XP(dvecInData1, vaInData1, pdvecIn1, inDataPitch1);
              MORPH_LA2NX8_XP(dvecInData2, vaInData2, pdvecIn2, inDataPitch1);
              dvecInData1 = MORPH_MOV2NX8T(dvecInData1, MORPH_MINVALUE, vbMask);
              dvecInData2 = MORPH_MOV2NX8T(dvecInData2, MORPH_MINVALUE, vbMask);
              /* Find the maximum value */
              dvecMax1 = MORPH_MAX2NX8(dvecMax1, dvecInData1);
              dvecMax2 = MORPH_MAX2NX8(dvecMax2, dvecInData2);
            }
          }
        }
      }
      dvecMax1  = MORPH_MAX2NX8(dvecMax1, dvecMax2);
    }
    /* Find the maximum value */
    MORPH_RMAX2NX8(dvecMax1, max1);
    /* Store the result */
    pOutData[0] = max1;
  }
  return(XI_ERROR_STATUS());
}

#endif //if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))
