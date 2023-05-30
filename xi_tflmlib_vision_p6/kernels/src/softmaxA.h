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

#if INPUT_DATA_TYPE == UNSIGNED8BIT

#undef MAKE_NAME_IMPL1
#undef MAKE_NAME_IMPL2
#undef MAKE_NAME1
#undef MAKE_NAME2
#undef MORPH_IDT_CHECK
#undef MORPH_CHECK_ARRAY
#undef MORPH_IDT_SCALAR
#undef MORPH_IDT_SCALAR_MIN
#undef MORPH_IDT_SCALAR_MAX
#undef MORPH_IDT_2Nx8
#undef MORPH_OP_PRIME_LOAD_2NX8_PP
#undef MORPH_OP_ALIGN_LOAD_2NX8_XP
#undef MORPH_OP_ALIGN_LOAD_2NX8_IP
#undef MORPH_OP_ALIGN_LOAD_2Nx8_VARIABLE
#undef MORPH_OP_ALIGN_STORE_2NX8_XP
#undef MORPH_OP_STORE_FLUSH
#undef MORPH_OP_STORE_FLUSH_XP
#undef MORPH_OP_SUB2NX8
#undef MORPH_OP_MAX2NX8
#undef MORPH_OP_UNPK_2NX8_EVEN
#undef MORPH_OP_UNPK_2NX8_ODD
#undef MORPH_OP_MOV_2NX8T
#undef MORPH_OP_ALIGN_STORE_2NX8_IP
#undef MORPH_OP_GATHERA_NX8
#undef MORPH_OP_GATHERA_NX8T
#undef MORPH_OP_GATHERD_NX8_L
#undef MORPH_OP_GATHERD_NX8_H
#undef MORPH_OP_SCATTER2NX8_L
#undef MORPH_OP_SCATTER2NX8_H
#undef MORPH_OP_SCATTER2NX8T_L
#undef MORPH_OP_SCATTER2NX8T_H

#define MAKE_NAME_IMPL1(name, MORPH_FNAME_SPECIFIER_IDT)         name##_##MORPH_FNAME_SPECIFIER_IDT
#define MAKE_NAME_IMPL2(name, MORPH_FNAME_SPECIFIER_IDT, suffix) name##_##MORPH_FNAME_SPECIFIER_IDT##_##suffix
#define MAKE_NAME1(name)                    MAKE_NAME_IMPL1(name, U8)
#define MAKE_NAME2(name, suffix)            MAKE_NAME_IMPL2(name, U8, suffix)
#define MORPH_IDT_CHECK                     XI_CHECK_TILE3D_U8
#define MORPH_CHECK_ARRAY                   XI_CHECK_ARRAY_U8
#define MORPH_IDT_SCALAR                    uint8_t
#define MORPH_IDT_SCALAR_MIN                0
#define MORPH_IDT_SCALAR_MAX                UCHAR_MAX
#define MORPH_IDT_2Nx8                      xb_vec2Nx8U
#define MORPH_OP_PRIME_LOAD_2NX8_PP         IVP_LA2NX8U_PP
#define MORPH_OP_ALIGN_LOAD_2NX8_XP         IVP_LA2NX8U_XP
#define MORPH_OP_ALIGN_LOAD_2NX8_IP         IVP_LA2NX8U_IP
#define MORPH_OP_ALIGN_LOAD_2Nx8_VARIABLE   IVP_LAV2NX8U_XP
#define MORPH_OP_ALIGN_STORE_2NX8_XP        IVP_SAV2NX8U_XP
#define MORPH_OP_STORE_FLUSH                IVP_SAPOS2NX8U_FP
#define MORPH_OP_SUB2NX8                    IVP_SUB2NX8U
#define MORPH_OP_MAX2NX8                    IVP_MAXU2NX8
#define MORPH_OP_UNPK_2NX8_EVEN             IVP_UNPKU2NX8_0
#define MORPH_OP_UNPK_2NX8_ODD              IVP_UNPKU2NX8_1
#define MORPH_OP_MOV_2NX8T                  IVP_MOV2NX8UT
#define MORPH_OP_ALIGN_STORE_2NX8_IP        IVP_SA2NX8U_IP
#define MORPH_OP_GATHERA_NX8                IVP_GATHERANX8U
#define MORPH_OP_GATHERA_NX8T               IVP_GATHERANX8UT
#define MORPH_OP_GATHERD_NX8_L              IVP_GATHERD2NX8U_L
#define MORPH_OP_GATHERD_NX8_H              IVP_GATHERD2NX8U_H
#define MORPH_OP_SCATTER2NX8_L              IVP_SCATTER2NX8U_L
#define MORPH_OP_SCATTER2NX8_H              IVP_SCATTER2NX8U_H
#define MORPH_OP_SCATTER2NX8T_L             IVP_SCATTER2NX8UT_L
#define MORPH_OP_SCATTER2NX8T_H             IVP_SCATTER2NX8UT_H

#elif INPUT_DATA_TYPE == SIGNED8BIT

#undef MAKE_NAME_IMPL1
#undef MAKE_NAME_IMPL2
#undef MAKE_NAME1
#undef MAKE_NAME2
#undef MORPH_IDT_CHECK
#undef MORPH_CHECK_ARRAY
#undef MORPH_IDT_SCALAR
#undef MORPH_IDT_SCALAR_MIN
#undef MORPH_IDT_SCALAR_MAX
#undef MORPH_IDT_2Nx8
#undef MORPH_OP_PRIME_LOAD_2NX8_PP
#undef MORPH_OP_ALIGN_LOAD_2NX8_XP
#undef MORPH_OP_ALIGN_LOAD_2NX8_IP
#undef MORPH_OP_ALIGN_LOAD_2Nx8_VARIABLE
#undef MORPH_OP_ALIGN_STORE_2NX8_XP
#undef MORPH_OP_STORE_FLUSH
#undef MORPH_OP_STORE_FLUSH_XP
#undef MORPH_OP_SUB2NX8
#undef MORPH_OP_MAX2NX8
#undef MORPH_OP_UNPK_2NX8_EVEN
#undef MORPH_OP_UNPK_2NX8_ODD
#undef MORPH_OP_MOV_2NX8T
#undef MORPH_OP_ALIGN_STORE_2NX8_IP
#undef MORPH_OP_GATHERA_NX8
#undef MORPH_OP_GATHERA_NX8T
#undef MORPH_OP_GATHERD_NX8_L
#undef MORPH_OP_GATHERD_NX8_H
#undef MORPH_OP_SCATTER2NX8_L
#undef MORPH_OP_SCATTER2NX8_H
#undef MORPH_OP_SCATTER2NX8T_L
#undef MORPH_OP_SCATTER2NX8T_H

#define MAKE_NAME_IMPL1(name, MORPH_FNAME_SPECIFIER_IDT)         name##_##MORPH_FNAME_SPECIFIER_IDT
#define MAKE_NAME_IMPL2(name, MORPH_FNAME_SPECIFIER_IDT, suffix) name##_##MORPH_FNAME_SPECIFIER_IDT##_##suffix
#define MAKE_NAME1(name)                    MAKE_NAME_IMPL1(name, S8)
#define MAKE_NAME2(name, suffix)            MAKE_NAME_IMPL2(name, S8, suffix)
#define MORPH_IDT_CHECK                     XI_CHECK_TILE3D_S8
#define MORPH_CHECK_ARRAY                   XI_CHECK_ARRAY_S8
#define MORPH_IDT_SCALAR                    int8_t
#define MORPH_IDT_SCALAR_MIN                SCHAR_MIN
#define MORPH_IDT_SCALAR_MAX                SCHAR_MAX
#define MORPH_IDT_2Nx8                      xb_vec2Nx8
#define MORPH_OP_PRIME_LOAD_2NX8_PP         IVP_LA2NX8_PP
#define MORPH_OP_ALIGN_LOAD_2NX8_XP         IVP_LA2NX8_XP
#define MORPH_OP_ALIGN_LOAD_2NX8_IP         IVP_LA2NX8_IP
#define MORPH_OP_ALIGN_LOAD_2Nx8_VARIABLE   IVP_LAV2NX8_XP
#define MORPH_OP_ALIGN_STORE_2NX8_XP        IVP_SAV2NX8_XP
#define MORPH_OP_STORE_FLUSH                IVP_SAPOS2NX8_FP
#define MORPH_OP_SUB2NX8                    IVP_SUB2NX8
#define MORPH_OP_MAX2NX8                    IVP_MAX2NX8
#define MORPH_OP_UNPK_2NX8_EVEN             IVP_UNPKS2NX8_0
#define MORPH_OP_UNPK_2NX8_ODD              IVP_UNPKS2NX8_1
#define MORPH_OP_MOV_2NX8T                  IVP_MOV2NX8T
#define MORPH_OP_ALIGN_STORE_2NX8_IP        IVP_SA2NX8_IP
#define MORPH_OP_GATHERA_NX8                IVP_GATHERANX8S
#define MORPH_OP_GATHERA_NX8T               IVP_GATHERANX8ST
#define MORPH_OP_GATHERD_NX8_L              IVP_GATHERD2NX8_L
#define MORPH_OP_GATHERD_NX8_H              IVP_GATHERD2NX8_H
#define MORPH_OP_SCATTER2NX8_L              IVP_SCATTER2NX8_L
#define MORPH_OP_SCATTER2NX8_H              IVP_SCATTER2NX8_H
#define MORPH_OP_SCATTER2NX8T_L             IVP_SCATTER2NX8T_L
#define MORPH_OP_SCATTER2NX8T_H             IVP_SCATTER2NX8T_H

#endif

#if INPUT_DATA_TYPE == UNSIGNED8BIT
#undef MORPH_OP_CLAMP
#define MORPH_OP_CLAMP(vecLow, vecHigh, dvecOut)                                                    \
{                                                                                                   \
  vecLow  = IVP_MAXNX16(IVP_MINNX16(vecLow, (xb_vecNx16)UCHAR_MAX), 0);                                             \
  vecHigh = IVP_MAXNX16(IVP_MINNX16(vecHigh, (xb_vecNx16)UCHAR_MAX), 0);                                            \
  dvecOut = IVP_MOV2NX8_FROMNX16(IVP_SELNX16I(vecRes1, vecRes0, IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0)); \
}
#elif INPUT_DATA_TYPE == SIGNED8BIT
#undef MORPH_OP_CLAMP
#define MORPH_OP_CLAMP(vecLow, vecHigh, dvecOut)                                                    \
{                                                                                                   \
  vecLow  = IVP_ADDNX16(vecLow, SCHAR_MIN);                                                         \
  vecHigh = IVP_ADDNX16(vecHigh, SCHAR_MIN);                                                        \
  vecLow  = IVP_MAXNX16(IVP_MINNX16(vecLow, (xb_vecNx16)SCHAR_MAX), (xb_vecNx16)SCHAR_MIN);         \
  vecHigh = IVP_MAXNX16(IVP_MINNX16(vecHigh, (xb_vecNx16)SCHAR_MAX), (xb_vecNx16)SCHAR_MIN);        \
  dvecOut = IVP_MOV2NX8_FROMNX16(IVP_SELNX16I(vecRes1, vecRes0, IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0)); \
}
#endif

static XI_ERR_TYPE MAKE_NAME2(softmaxA3D, DIM1_LUT)(const xi_pTile3D inTile,
                                                    const xi_pArray softmaxArray,
                                                    xi_pTile3D outTile,
                                                    const xi_cnn_softmaxA_params  * pparams)
{
  /* Error Checks */
  XI_ERROR_CHECKS()
  {
    MORPH_IDT_CHECK(inTile);
    MORPH_IDT_CHECK(outTile);
    XI_CHECK_ARRAY_S32(softmaxArray);
    XI_CHECK_TILE3D_SIZE_EQ(inTile, outTile);
    XI_CHECK_POINTER(pparams);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(softmaxArray) == 256, XI_ERR_BADARG, \
                   "\nsoftmaxArray LUT Size = %d, softmaxArray LUT Size should be equal to 256", XI_ARRAY_GET_WIDTH(softmaxArray));
    XI_CHECK_ERROR(XI_TILE3D_GET_DATA_ORDER(inTile) == XI_TILE3D_GET_DATA_ORDER(outTile), XI_ERR_BADARG, \
                   "\nData Order of InputTile = %d, OutputTile= %d, Data order of input and output tile should be same", \
                   XI_TILE3D_GET_DATA_ORDER(inTile), XI_TILE3D_GET_DATA_ORDER(outTile));
  }


  /* Getting parameters from the tile structures */
  const int32_t dim1Size      = XI_TILE3D_GET_DIM1(inTile);
  const int32_t dim2Size      = XI_TILE3D_GET_DIM2(inTile);
  const int32_t inDataPitch1  = XI_TILE3D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2  = XI_TILE3D_GET_DIM2_PITCH(inTile);
  const int32_t dim3Size      = XI_TILE3D_GET_DIM3(inTile);
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
  MORPH_IDT_SCALAR *pInput    = (MORPH_IDT_SCALAR *) XI_TILE3D_GET_DATA_PTR(inTile);
  MORPH_IDT_SCALAR *pOutput   = (MORPH_IDT_SCALAR *) XI_TILE3D_GET_DATA_PTR(outTile);
  int32_t x, y, z;

  static const int kAccumulationIntegerBits = 12;

  int32_t *LUTOut = (int32_t *) XI_ARRAY_GET_BUFF_PTR(softmaxArray);

  MORPH_IDT_SCALAR *pInData, *pOut;
  MORPH_IDT_2Nx8 *restrict pvecInp;
  valign va, vaStore;
  MORPH_IDT_2Nx8 dvecInp, dvecMax, dvecDiff, dvecRes;
  xb_vecNx16 vec0, vec1;

  xb_vecN_2x32v hvecSumExp0, hvecSumExp1, hvecSumExp2, hvecSumExp3, vecSumExp;

  xb_vecNx16 vecRes0, vecRes1;
  xb_vecN_2x64w wvec0, wvec1;
  MORPH_IDT_2Nx8 *restrict pvecOutput;;
  xb_vecN_2x64w vecProduct;

  vaStore = IVP_ZALIGN();
  xb_vec2Nx8 *restrict dvecLUTBase = (xb_vec2Nx8 *)LUTOut;
  valign va1                       = IVP_LA2NX8_PP(dvecLUTBase);
  xb_vec2Nx8 dvecLUT0LL, dvecLUT1LL, dvecLUT2LL, dvecLUT3LL, dvecLUT0LH, dvecLUT1LH, dvecLUT2LH, dvecLUT3LH;
  xb_vec2Nx8 dvecLUT0HL, dvecLUT1HL, dvecLUT2HL, dvecLUT3HL, dvecLUT0HH, dvecLUT1HH, dvecLUT2HH, dvecLUT3HH;
  xb_vec2Nx8 dvecLL0, dvecLL, dvecLH0, dvecLH, dvecHL0, dvecHL, dvecHH0, dvecHH;


  IVP_LA2NX8_IP(dvecLUT0LL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT1LL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT2LL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT3LL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT0LH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT1LH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT2LH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT3LH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT0HL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT1HL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT2HL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT3HL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT0HH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT1HH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT2HH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT3HH, va1, dvecLUTBase);


  int16_t max_in_row;
  int32_t sum_of_exps;               //Q12.19
  for (z = 0; z < dim3Size; z++)     /* along the image depth(WHD)/height(DWH) */
  {
    for (y = 0; y < dim2Size; y++)   /* along the image height(WHD)/width(DWH) */
    {
      max_in_row = MORPH_IDT_SCALAR_MIN;
      dvecMax    = MORPH_IDT_SCALAR_MIN;
      pInData    = (MORPH_IDT_SCALAR *) (pInput + ((z * inDataPitch2) + (y * inDataPitch1)));
      pOut       = pOutput + ((z * outDataPitch2) + (y * outDataPitch1));
      pvecInp    = (MORPH_IDT_2Nx8 *) (pInData);
      pvecOutput = (MORPH_IDT_2Nx8 *) (pOut);
      va         = MORPH_OP_PRIME_LOAD_2NX8_PP(pvecInp);
      for (x = 0; x <= dim1Size - (XCHAL_IVPN_SIMD_WIDTH * 2); x += (XCHAL_IVPN_SIMD_WIDTH * 2))
      {
        MORPH_OP_ALIGN_LOAD_2NX8_IP(dvecInp, va, pvecInp);
        dvecMax = MORPH_OP_MAX2NX8(dvecMax, dvecInp);
      }
      if (x < dim1Size)
      {
        MORPH_OP_ALIGN_LOAD_2Nx8_VARIABLE(dvecInp, va, pvecInp, dim1Size - x);
        dvecInp = MORPH_OP_MOV_2NX8T(dvecInp, MORPH_IDT_SCALAR_MIN, IVP_LTR2N(dim1Size - x));
        dvecMax = MORPH_OP_MAX2NX8(dvecMax, dvecInp);
      }

      vec0       = MORPH_OP_UNPK_2NX8_EVEN(dvecMax);
      vec1       = MORPH_OP_UNPK_2NX8_ODD(dvecMax);
      max_in_row = (int16_t)(IVP_RMAXNX16(IVP_MAXNX16(vec1, vec0)));

      // vector implementation of above code which is sum of exponents along the depth
      dvecMax     = (MORPH_IDT_2Nx8) max_in_row;
      sum_of_exps = 0;
      vecSumExp   = 0;

      pvecInp = (MORPH_IDT_2Nx8 *) (pInData);
      va      = MORPH_OP_PRIME_LOAD_2NX8_PP(pvecInp);
      for (x = 0; x < dim1Size - (XCHAL_IVPN_SIMD_WIDTH * 2); x += (XCHAL_IVPN_SIMD_WIDTH * 2))
      {
        MORPH_OP_ALIGN_LOAD_2Nx8_VARIABLE(dvecInp, va, pvecInp, dim1Size - x);
        dvecDiff = MORPH_OP_SUB2NX8(dvecMax, dvecInp);                         // equivalent of this int32 input_diff = (int32)(input_data[Offset(input_dims, c, x, y, b)]) -  max_in_row;

        // implements (input_diff >= diff_min) along with out of boundary condition
        //SEL based lookup table
        dvecLL = IVP_SEL2NX8(dvecLUT1LL, dvecLUT0LL, dvecDiff);
        IVP_SEL2NX8T(dvecLL, dvecLUT3LL, dvecLUT2LL, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

        dvecLH = IVP_SEL2NX8(dvecLUT1LH, dvecLUT0LH, dvecDiff);
        IVP_SEL2NX8T(dvecLH, dvecLUT3LH, dvecLUT2LH, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

        dvecHL = IVP_SEL2NX8(dvecLUT1HL, dvecLUT0HL, dvecDiff);
        IVP_SEL2NX8T(dvecHL, dvecLUT3HL, dvecLUT2HL, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

        dvecHH = IVP_SEL2NX8(dvecLUT1HH, dvecLUT0HH, dvecDiff);
        IVP_SEL2NX8T(dvecHH, dvecLUT3HH, dvecLUT2HH, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));


        dvecLL0 = IVP_SEL2NX8I(dvecLH, dvecLL, IVP_SELI_8B_INTERLEAVE_1_LO);
        dvecLH0 = IVP_SEL2NX8I(dvecLH, dvecLL, IVP_SELI_8B_INTERLEAVE_1_HI);
        dvecHL0 = IVP_SEL2NX8I(dvecHH, dvecHL, IVP_SELI_8B_INTERLEAVE_1_LO);
        dvecHH0 = IVP_SEL2NX8I(dvecHH, dvecHL, IVP_SELI_8B_INTERLEAVE_1_HI);


        dvecLL = IVP_SEL2NX8I(dvecHL0, dvecLL0, IVP_SELI_INTERLEAVE_1_LO);
        dvecHL = IVP_SEL2NX8I(dvecHL0, dvecLL0, IVP_SELI_INTERLEAVE_1_HI);
        dvecLH = IVP_SEL2NX8I(dvecHH0, dvecLH0, IVP_SELI_INTERLEAVE_1_LO);
        dvecHH = IVP_SEL2NX8I(dvecHH0, dvecLH0, IVP_SELI_INTERLEAVE_1_HI);


        hvecSumExp0 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecLL));
        hvecSumExp1 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecHL));
        hvecSumExp2 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecLH));
        hvecSumExp3 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecHH));
        //end of lookup table values

        /* sum of exponentials is right shifted by 12 */
        hvecSumExp0 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16) 1, hvecSumExp0), 12);
        hvecSumExp1 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16) 1, hvecSumExp1), 12);
        hvecSumExp2 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16) 1, hvecSumExp2), 12);
        hvecSumExp3 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16) 1, hvecSumExp3), 12);
        vecSumExp  += hvecSumExp0 + hvecSumExp1 + hvecSumExp2 + hvecSumExp3;
      }
      MORPH_OP_ALIGN_LOAD_2Nx8_VARIABLE(dvecInp, va, pvecInp, dim1Size - x);
      dvecDiff = MORPH_OP_SUB2NX8(dvecMax, dvecInp);
      int32_t numElements        = XT_MINU((dim1Size - x), 255);
      xb_vecN_2x32v hvecElements = (xb_vecN_2x32v) numElements;
      vboolN_2 hvbCheckLL, hvbCheckLH, hvbCheckHL, hvbCheckHH;

      hvbCheckLL = IVP_GTN_2X32(hvecElements, IVP_SEQN_2X32());
      hvbCheckLH = IVP_GTN_2X32(hvecElements - (xb_vecN_2x32v) (XCHAL_IVPN_SIMD_WIDTH / 2 ) , IVP_SEQN_2X32());
      hvbCheckHL = IVP_GTN_2X32(hvecElements - (xb_vecN_2x32v) (XCHAL_IVPN_SIMD_WIDTH ), IVP_SEQN_2X32());
      hvbCheckHH = IVP_GTN_2X32(hvecElements - (xb_vecN_2x32v) ((XCHAL_IVPN_SIMD_WIDTH * 3 ) / 2), IVP_SEQN_2X32());


      //SEL based lookup table

      dvecLL = IVP_SEL2NX8(dvecLUT1LL, dvecLUT0LL, dvecDiff);
      IVP_SEL2NX8T(dvecLL, dvecLUT3LL, dvecLUT2LL, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

      dvecLH = IVP_SEL2NX8(dvecLUT1LH, dvecLUT0LH, dvecDiff);
      IVP_SEL2NX8T(dvecLH, dvecLUT3LH, dvecLUT2LH, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

      dvecHL = IVP_SEL2NX8(dvecLUT1HL, dvecLUT0HL, dvecDiff);
      IVP_SEL2NX8T(dvecHL, dvecLUT3HL, dvecLUT2HL, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

      dvecHH = IVP_SEL2NX8(dvecLUT1HH, dvecLUT0HH, dvecDiff);
      IVP_SEL2NX8T(dvecHH, dvecLUT3HH, dvecLUT2HH, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));


      IVP_DSEL2NX8I(dvecLH, dvecLL, dvecLH, dvecLL, IVP_DSELI_8B_INTERLEAVE_1);
      IVP_DSEL2NX8I(dvecHH, dvecHL, dvecHH, dvecHL, IVP_DSELI_8B_INTERLEAVE_1);

      IVP_DSEL2NX8I(dvecHL, dvecLL, dvecHL, dvecLL, IVP_DSELI_INTERLEAVE_1);
      IVP_DSEL2NX8I(dvecHH, dvecLH, dvecHH, dvecLH, IVP_DSELI_INTERLEAVE_1);

      hvecSumExp0 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecLL));
      hvecSumExp1 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecHL));
      hvecSumExp2 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecLH));
      hvecSumExp3 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecHH));

      hvecSumExp0 = IVP_MOVN_2X32T(hvecSumExp0, IVP_ZERON_2X32(), hvbCheckLL);
      hvecSumExp1 = IVP_MOVN_2X32T(hvecSumExp1, IVP_ZERON_2X32(), hvbCheckLH);

      hvecSumExp2 = IVP_MOVN_2X32T(hvecSumExp2, IVP_ZERON_2X32(), hvbCheckHL);

      hvecSumExp3 = IVP_MOVN_2X32T(hvecSumExp3, IVP_ZERON_2X32(), hvbCheckHH);
      //end of lookup table values

      /* sum of exponentials is right shifted by 12 */
      hvecSumExp0 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16) 1, hvecSumExp0), 12);
      hvecSumExp1 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16) 1, hvecSumExp1), 12);
      hvecSumExp2 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16) 1, hvecSumExp2), 12);
      hvecSumExp3 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16) 1, hvecSumExp3), 12);
      vecSumExp  += hvecSumExp0 + hvecSumExp1 + hvecSumExp2 + hvecSumExp3;

      sum_of_exps = IVP_RADDN_2X32(vecSumExp);
      int32_t fixed_sum_of_exps = sum_of_exps;                  //sum_of_exps.raw();

      int headroom_plus_one = XT_NSAU((uint32_t) fixed_sum_of_exps);
      // This is the number of bits to the left of the binary point above 1.0.
      // Consider fixed_sum_of_exps=1.25.  In that case shifted_scale=0.8 and
      // no later adjustment will be needed.
      int num_bits_over_unit        = kAccumulationIntegerBits - headroom_plus_one;
      int32_t shifted_sum_minus_one = (int32_t) (((uint32_t) (fixed_sum_of_exps) << headroom_plus_one) - ((uint32_t) (1) << 31));
      // FixedPoint0 shifted_scale = gemmlowp::one_over_one_plus_x_for_x_in_0_1( FixedPoint0::FromRaw(shifted_sum_minus_one));
      // Strat of one_over_one_plus_x_for_x_in_0_1
      const int32_t constant_48_over_17     = 1515870810;                //Q2.29 format
      const int32_t constant_neg_32_over_17 = -1010580540;               //Q2.29 format

      vecProduct = IVP_MULN_2X16X32_0((xb_vecNx16) 1, (xb_vecN_2x32v) (ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct, (xb_vecNx16) 1, (xb_vecN_2x32v) (shifted_sum_minus_one + 1));
      xb_vecN_2x32v hvecHalfDenominator = IVP_PACKVRNRN_2X64W(vecProduct, 1);

      vecProduct = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v) constant_neg_32_over_17), hvecHalfDenominator);
      IVP_MULAHN_2X16X32_1(vecProduct, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v) constant_neg_32_over_17), hvecHalfDenominator);

      int x1 = (int ) (IVP_MOV32_FROMN_2X32(IVP_PACKVRN_2X64W(vecProduct, 31))) + constant_48_over_17;


      for (int i = 0; i < 3; i++)
      {
        vecProduct = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v) x1), hvecHalfDenominator);
        IVP_MULAHN_2X16X32_1(vecProduct, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v) x1), hvecHalfDenominator);
        xb_vecN_2x32v vecHalf_Denominator_times_x           = IVP_PACKVRN_2X64W(vecProduct, 31);
        xb_vecN_2x32v vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v) (1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v) x1), (xb_vecN_2x32v) (vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v) x1), (xb_vecN_2x32v) (vecOne_minus_half_denominator_times_x));
        int32_t delta_x = IVP_MOV32_FROMN_2X32(IVP_PACKVRN_2X64W(vecProduct, 31));
        x1 = x1 + (delta_x << 2);
      }
      int result     = x1 << 1;
      XT_MOVLTZ(result, 0x7FFFFFFF, ((1 << 30) - 1)-x1);
      XT_MOVLTZ(result, 0x7FFFFFFF, x1+((1 << 30) - 1));

       int shifted_scale = result;

      xb_vecN_2x32v hvecShiftedScale = (xb_vecN_2x32v) (shifted_scale);

      pvecInp = (MORPH_IDT_2Nx8 *) (pInData);
      va      = MORPH_OP_PRIME_LOAD_2NX8_PP(pvecInp);
      for (x = 0; x < dim1Size; x += (XCHAL_IVPN_SIMD_WIDTH * 2))
      {
        MORPH_OP_ALIGN_LOAD_2Nx8_VARIABLE(dvecInp, va, pvecInp, dim1Size - x);
        dvecDiff = MORPH_OP_SUB2NX8(dvecMax, dvecInp);
        dvecLL  = IVP_SEL2NX8(dvecLUT1LL, dvecLUT0LL, dvecDiff);
        IVP_SEL2NX8T(dvecLL, dvecLUT3LL, dvecLUT2LL, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

        dvecLH = IVP_SEL2NX8(dvecLUT1LH, dvecLUT0LH, dvecDiff);
        IVP_SEL2NX8T(dvecLH, dvecLUT3LH, dvecLUT2LH, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

        dvecHL = IVP_SEL2NX8(dvecLUT1HL, dvecLUT0HL, dvecDiff);
        IVP_SEL2NX8T(dvecHL, dvecLUT3HL, dvecLUT2HL, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

        dvecHH = IVP_SEL2NX8(dvecLUT1HH, dvecLUT0HH, dvecDiff);
        IVP_SEL2NX8T(dvecHH, dvecLUT3HH, dvecLUT2HH, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));


        dvecLL0 = IVP_SEL2NX8I(dvecLH, dvecLL, IVP_SELI_8B_INTERLEAVE_1_LO);
        dvecLH0 = IVP_SEL2NX8I(dvecLH, dvecLL, IVP_SELI_8B_INTERLEAVE_1_HI);
        dvecHL0 = IVP_SEL2NX8I(dvecHH, dvecHL, IVP_SELI_8B_INTERLEAVE_1_LO);
        dvecHH0 = IVP_SEL2NX8I(dvecHH, dvecHL, IVP_SELI_8B_INTERLEAVE_1_HI);


        dvecLL = IVP_SEL2NX8I(dvecHL0, dvecLL0, IVP_SELI_INTERLEAVE_1_LO);
        dvecHL = IVP_SEL2NX8I(dvecHL0, dvecLL0, IVP_SELI_INTERLEAVE_1_HI);
        dvecLH = IVP_SEL2NX8I(dvecHH0, dvecLH0, IVP_SELI_INTERLEAVE_1_LO);
        dvecHH = IVP_SEL2NX8I(dvecHH0, dvecLH0, IVP_SELI_INTERLEAVE_1_HI);


        hvecSumExp0 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecLL));
        hvecSumExp1 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecHL));
        hvecSumExp2 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecLH));
        hvecSumExp3 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecHH));

        // Start by multiplying with the reciprocal of SumOfExps
        wvec0 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecShiftedScale), hvecSumExp0);
        IVP_MULAHN_2X16X32_1(wvec0, IVP_MOVNX16_FROMN_2X32(hvecShiftedScale), hvecSumExp0);
        hvecSumExp0 = IVP_PACKVRN_2X64W(wvec0, 31);
        wvec0       = IVP_MULUSN_2X16X32_0((xb_vecNx16) 1, hvecSumExp0);
        hvecSumExp0 = IVP_PACKVRN_2X64W(wvec0, (31 - OUTPUT_BITS + num_bits_over_unit));

        wvec0 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecShiftedScale), hvecSumExp1);
        IVP_MULAHN_2X16X32_1(wvec0, IVP_MOVNX16_FROMN_2X32(hvecShiftedScale), hvecSumExp1);
        hvecSumExp1 = IVP_PACKVRN_2X64W(wvec0, 31);
        wvec0       = IVP_MULUSN_2X16X32_0((xb_vecNx16) 1, hvecSumExp1);
        hvecSumExp1 = IVP_PACKVRN_2X64W(wvec0, (31 - OUTPUT_BITS + num_bits_over_unit));

        //for 2 and 3
        wvec1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecShiftedScale), hvecSumExp2);
        IVP_MULAHN_2X16X32_1(wvec1, IVP_MOVNX16_FROMN_2X32(hvecShiftedScale), hvecSumExp2);
        hvecSumExp2 = IVP_PACKVRN_2X64W(wvec1, 31);
        wvec1       = IVP_MULUSN_2X16X32_0((xb_vecNx16) 1, hvecSumExp2);
        hvecSumExp2 = IVP_PACKVRN_2X64W(wvec1, (31 - OUTPUT_BITS + num_bits_over_unit));

        wvec1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecShiftedScale), hvecSumExp3);
        IVP_MULAHN_2X16X32_1(wvec1, IVP_MOVNX16_FROMN_2X32(hvecShiftedScale), hvecSumExp3);
        hvecSumExp3 = IVP_PACKVRNRN_2X64W(wvec1, 31);
        wvec1       = IVP_MULUSN_2X16X32_0((xb_vecNx16) 1, hvecSumExp3);
        hvecSumExp3 = IVP_PACKVRN_2X64W(wvec1, (31 - OUTPUT_BITS + num_bits_over_unit));

        vecRes0 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecSumExp1, hvecSumExp0, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));
        vecRes1 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecSumExp3, hvecSumExp2, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));
        MORPH_OP_CLAMP(vecRes0, vecRes1, dvecRes);

        MORPH_OP_ALIGN_STORE_2NX8_XP(dvecRes, vaStore, pvecOutput, dim1Size - x);
      }
      MORPH_OP_STORE_FLUSH(vaStore, pvecOutput);
    }
  }
  return XI_ERROR_STATUS();
}

static XI_ERR_TYPE MAKE_NAME2(softmaxA3D, DIM2_LUT)(const xi_pTile3D inTile,
                                                    const xi_pArray softmaxArray,
                                                    xi_pTile3D outTile,
                                                    const xi_cnn_softmaxA_params  * pparams)
{
  /* Error Checks */
  XI_ERROR_CHECKS()
  {
    MORPH_IDT_CHECK(inTile);
    MORPH_IDT_CHECK(outTile);
    XI_CHECK_ARRAY_S32(softmaxArray);
    XI_CHECK_TILE3D_SIZE_EQ(inTile, outTile);
    XI_CHECK_POINTER(pparams);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(softmaxArray) == 256, XI_ERR_BADARG, \
                   "\nsoftmaxArray LUT Size = %d, softmaxArray LUT Size should be equal to 256", XI_ARRAY_GET_WIDTH(softmaxArray));
    XI_CHECK_ERROR(XI_TILE3D_GET_DATA_ORDER(inTile) == XI_TILE3D_GET_DATA_ORDER(outTile), XI_ERR_BADARG, \
                   "\nData Order of InputTile = %d, OutputTile= %d\nData order of input and output tile should be same", \
                   XI_TILE3D_GET_DATA_ORDER(inTile), XI_TILE3D_GET_DATA_ORDER(outTile));
  }


  /* Getting parameters from the tile structures */
  const int32_t dim1Size      = XI_TILE3D_GET_DIM1(inTile);
  const int32_t dim2Size      = XI_TILE3D_GET_DIM2(inTile);
  const int32_t inDataPitch1  = XI_TILE3D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2  = XI_TILE3D_GET_DIM2_PITCH(inTile);
  const int32_t dim3Size      = XI_TILE3D_GET_DIM3(inTile);
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
  MORPH_IDT_SCALAR *pInput    = (MORPH_IDT_SCALAR *) XI_TILE3D_GET_DATA_PTR(inTile);
  MORPH_IDT_SCALAR *pOutput   = (MORPH_IDT_SCALAR *) XI_TILE3D_GET_DATA_PTR(outTile);
  int32_t x, y, z;

  static const int kAccumulationIntegerBits = 12;

  int32_t *LUTOut = (int32_t *) XI_ARRAY_GET_BUFF_PTR(softmaxArray);



  MORPH_IDT_SCALAR *pInData, *pOut;
  MORPH_IDT_2Nx8 *restrict pvecInp;
  valign va, vaStore;
  MORPH_IDT_2Nx8 dvecInp, dvecMax, dvecDiff, dvecRes;

  xb_vecN_2x32v hvecSumExp0, hvecSumExp1, hvecSumExp2, hvecSumExp3, hvecSum;
  xb_vecN_2x32v hvecSumExpInter0, hvecSumExpInter1, hvecSumExpInter2, hvecSumExpInter3;

  xb_vecN_2x32v hvecOne = 1;
  xb_vecN_2x32v hvecShift = 0;
  xb_vecNx16 vecRes0, vecRes1;
  xb_vecN_2x64w wvec0, wvec1;
  MORPH_IDT_2Nx8 *restrict pvecOutput;;
  xb_vecN_2x64w vecProduct1, vecProduct2, vecProduct3, vecProduct4;

  vaStore = IVP_ZALIGN();
  xb_vec2Nx8 *restrict dvecLUTBase = (xb_vec2Nx8 *)LUTOut;
  valign va1                       = IVP_LA2NX8_PP(dvecLUTBase);
  xb_vec2Nx8 dvecLUT0LL, dvecLUT1LL, dvecLUT2LL, dvecLUT3LL, dvecLUT0LH, dvecLUT1LH, dvecLUT2LH, dvecLUT3LH;
  xb_vec2Nx8 dvecLUT0HL, dvecLUT1HL, dvecLUT2HL, dvecLUT3HL, dvecLUT0HH, dvecLUT1HH, dvecLUT2HH, dvecLUT3HH;
  xb_vec2Nx8 dvecLL0, dvecLL, dvecLH0, dvecLH, dvecHL0, dvecHL, dvecHH0, dvecHH;


  IVP_LA2NX8_IP(dvecLUT0LL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT1LL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT2LL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT3LL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT0LH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT1LH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT2LH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT3LH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT0HL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT1HL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT2HL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT3HL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT0HH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT1HH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT2HH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT3HH, va1, dvecLUTBase);

  int32_t vectorizationWidth = XCHAL_IVPN_SIMD_WIDTH * 2;

  for (z = 0; z < dim3Size; z++)     /* along the image depth(WHD)/height(DWH) */
  {
    for (x = 0; x < dim1Size; x += vectorizationWidth)   /* along the image height(WHD)/width(DWH) */
    {
      dvecMax    = MORPH_IDT_SCALAR_MIN;
      pInData    = (MORPH_IDT_SCALAR *) (pInput + ((z * inDataPitch2) + x));
      pOut       = pOutput + ((z * outDataPitch2) + x);

      pvecInp = (MORPH_IDT_2Nx8 *)(pInData);
      for (y = 0; y < dim2Size; y++)
      {
        va = MORPH_OP_PRIME_LOAD_2NX8_PP(pvecInp);
        MORPH_OP_ALIGN_LOAD_2NX8_XP(dvecInp, va, pvecInp, inDataPitch1);
        dvecMax = MORPH_OP_MAX2NX8(dvecMax, dvecInp);
      }
        hvecSum = 0;

        hvecSumExp0 =  0;
        hvecSumExp1 =  0;
        hvecSumExp2 =  0;
        hvecSumExp3 =  0;

      pvecInp = (MORPH_IDT_2Nx8 *)(pInData);
      for (y = 0; y < dim2Size; y++)
      {
        va = MORPH_OP_PRIME_LOAD_2NX8_PP(pvecInp);
        MORPH_OP_ALIGN_LOAD_2NX8_XP(dvecInp, va, pvecInp, inDataPitch1);
        dvecDiff = MORPH_OP_SUB2NX8(dvecMax, dvecInp);                         // equivalent of this int32 input_diff = (int32)(input_data[Offset(input_dims, c, x, y, b)]) -  max_in_row;

        // implements (input_diff >= diff_min) along with out of boundary condition
        //SEL based lookup table
        dvecLL = IVP_SEL2NX8(dvecLUT1LL, dvecLUT0LL, dvecDiff);
        IVP_SEL2NX8T(dvecLL, dvecLUT3LL, dvecLUT2LL, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

        dvecLH = IVP_SEL2NX8(dvecLUT1LH, dvecLUT0LH, dvecDiff);
        IVP_SEL2NX8T(dvecLH, dvecLUT3LH, dvecLUT2LH, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

        dvecHL = IVP_SEL2NX8(dvecLUT1HL, dvecLUT0HL, dvecDiff);
        IVP_SEL2NX8T(dvecHL, dvecLUT3HL, dvecLUT2HL, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

        dvecHH = IVP_SEL2NX8(dvecLUT1HH, dvecLUT0HH, dvecDiff);
        IVP_SEL2NX8T(dvecHH, dvecLUT3HH, dvecLUT2HH, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));


        dvecLL0 = IVP_SEL2NX8I(dvecLH, dvecLL, IVP_SELI_8B_INTERLEAVE_1_LO);
        dvecLH0 = IVP_SEL2NX8I(dvecLH, dvecLL, IVP_SELI_8B_INTERLEAVE_1_HI);
        dvecHL0 = IVP_SEL2NX8I(dvecHH, dvecHL, IVP_SELI_8B_INTERLEAVE_1_LO);
        dvecHH0 = IVP_SEL2NX8I(dvecHH, dvecHL, IVP_SELI_8B_INTERLEAVE_1_HI);


        dvecLL = IVP_SEL2NX8I(dvecHL0, dvecLL0, IVP_SELI_INTERLEAVE_1_LO);
        dvecHL = IVP_SEL2NX8I(dvecHL0, dvecLL0, IVP_SELI_INTERLEAVE_1_HI);
        dvecLH = IVP_SEL2NX8I(dvecHH0, dvecLH0, IVP_SELI_INTERLEAVE_1_LO);
        dvecHH = IVP_SEL2NX8I(dvecHH0, dvecLH0, IVP_SELI_INTERLEAVE_1_HI);


        hvecSumExpInter0 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecLL));
        hvecSumExpInter1 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecHL));
        hvecSumExpInter2 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecLH));
        hvecSumExpInter3 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecHH));
        //end of lookup table values

        /* sum of exponentials is right shifted by 12 */
        hvecSumExp0 = IVP_ADDN_2X32(IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpInter0), 12), hvecSumExp0);
        hvecSumExp1 = IVP_ADDN_2X32(IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpInter1), 12), hvecSumExp1);
        hvecSumExp2 = IVP_ADDN_2X32(IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpInter2), 12), hvecSumExp2);
        hvecSumExp3 = IVP_ADDN_2X32(IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpInter3), 12), hvecSumExp3);
      }

      xb_vecN_2x32v hvecShiftedSumMinusOne0, hvecShiftedSumMinusOne1, hvecShiftedSumMinusOne2, hvecShiftedSumMinusOne3;
      xb_vecN_2x32v hvecHeadRoomPlusOne0, hvecHeadRoomPlusOne1, hvecHeadRoomPlusOne2, hvecHeadRoomPlusOne3;

      // This is the number of bits to the left of the binary point above 1.0.
      // Consider fixed_sum_of_exps=1.25.  In that case shifted_scale=0.8 and
      // no later adjustment will be needed.
      hvecHeadRoomPlusOne0 = IVP_NSAUN_2X32(hvecSumExp0);
      hvecHeadRoomPlusOne1 = IVP_NSAUN_2X32(hvecSumExp1);
      hvecHeadRoomPlusOne2 = IVP_NSAUN_2X32(hvecSumExp2);
      hvecHeadRoomPlusOne3 = IVP_NSAUN_2X32(hvecSumExp3);

      hvecShiftedSumMinusOne0 = IVP_SUBN_2X32U(IVP_SLAN_2X32(hvecSumExp0, hvecHeadRoomPlusOne0), (xb_vecN_2x32Uv)(1 << 31));
      hvecShiftedSumMinusOne1 = IVP_SUBN_2X32U(IVP_SLAN_2X32(hvecSumExp1, hvecHeadRoomPlusOne1), (xb_vecN_2x32Uv)(1 << 31));
      hvecShiftedSumMinusOne2 = IVP_SUBN_2X32U(IVP_SLAN_2X32(hvecSumExp2, hvecHeadRoomPlusOne2), (xb_vecN_2x32Uv)(1 << 31));
      hvecShiftedSumMinusOne3 = IVP_SUBN_2X32U(IVP_SLAN_2X32(hvecSumExp3, hvecHeadRoomPlusOne3), (xb_vecN_2x32Uv)(1 << 31));


      hvecHeadRoomPlusOne0 = IVP_SUBN_2X32((xb_vecN_2x32Uv)kAccumulationIntegerBits, hvecHeadRoomPlusOne0);
      hvecHeadRoomPlusOne1 = IVP_SUBN_2X32((xb_vecN_2x32Uv)kAccumulationIntegerBits, hvecHeadRoomPlusOne1);
      hvecHeadRoomPlusOne2 = IVP_SUBN_2X32((xb_vecN_2x32Uv)kAccumulationIntegerBits, hvecHeadRoomPlusOne2);
      hvecHeadRoomPlusOne3 = IVP_SUBN_2X32((xb_vecN_2x32Uv)kAccumulationIntegerBits, hvecHeadRoomPlusOne3);

      // FixedPoint0 shifted_scale = gemmlowp::one_over_one_plus_x_for_x_in_0_1( FixedPoint0::FromRaw(shifted_sum_minus_one));
      // Strat of one_over_one_plus_x_for_x_in_0_1
      const int32_t constant_48_over_17     = 1515870810;                //Q2.29 format
      const int32_t constant_neg_32_over_17 = -1010580540;               //Q2.29 format

      vecProduct1 = IVP_MULN_2X16X32_0((xb_vecNx16) 1, (xb_vecN_2x32v) (ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct1, (xb_vecNx16)1, IVP_ADDN_2X32(hvecShiftedSumMinusOne0, (xb_vecN_2x32v)1));
      xb_vecN_2x32v hvecHalfDenominator1 = IVP_PACKVRNRN_2X64W(vecProduct1, 1);

      vecProduct1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v) constant_neg_32_over_17), hvecHalfDenominator1);
      IVP_MULAHN_2X16X32_1(vecProduct1, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v) constant_neg_32_over_17), hvecHalfDenominator1);


      vecProduct2 = IVP_MULN_2X16X32_0((xb_vecNx16)1, (xb_vecN_2x32v)(ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct2, (xb_vecNx16)1, IVP_ADDN_2X32(hvecShiftedSumMinusOne1, (xb_vecN_2x32v)1));
      xb_vecN_2x32v hvecHalfDenominator2 = IVP_PACKVRNRN_2X64W(vecProduct2, 1);

      vecProduct2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator2);
      IVP_MULAHN_2X16X32_1(vecProduct2, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator2);

      vecProduct3 = IVP_MULN_2X16X32_0((xb_vecNx16)1, (xb_vecN_2x32v)(ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct3, (xb_vecNx16)1, IVP_ADDN_2X32(hvecShiftedSumMinusOne2, (xb_vecN_2x32v)1));
      xb_vecN_2x32v hvecHalfDenominator3 = IVP_PACKVRNRN_2X64W(vecProduct3, 1);

      vecProduct3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator3);
      IVP_MULAHN_2X16X32_1(vecProduct3, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator3);

      vecProduct4 = IVP_MULN_2X16X32_0((xb_vecNx16)1, (xb_vecN_2x32v)(ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct4, (xb_vecNx16)1, IVP_ADDN_2X32(hvecShiftedSumMinusOne3, (xb_vecN_2x32v)1));
      xb_vecN_2x32v hvecHalfDenominator4 = IVP_PACKVRNRN_2X64W(vecProduct4, 1);

      vecProduct4 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator4);
      IVP_MULAHN_2X16X32_1(vecProduct4, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator4);

      xb_vecN_2x32v x1 = IVP_ADDN_2X32((IVP_PACKVRN_2X64W(vecProduct1, 31)), (xb_vecN_2x32v)constant_48_over_17);

      xb_vecN_2x32v x2 = IVP_ADDN_2X32((IVP_PACKVRN_2X64W(vecProduct2, 31)), (xb_vecN_2x32v)constant_48_over_17);

      xb_vecN_2x32v x3 = IVP_ADDN_2X32((IVP_PACKVRN_2X64W(vecProduct3, 31)), (xb_vecN_2x32v)constant_48_over_17);

      xb_vecN_2x32v x4 = IVP_ADDN_2X32((IVP_PACKVRN_2X64W(vecProduct4, 31)), (xb_vecN_2x32v)constant_48_over_17);

      for (int i = 0; i < 3; i++)
      {
        vecProduct1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v) x1), hvecHalfDenominator1);
        IVP_MULAHN_2X16X32_1(vecProduct1, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v) x1), hvecHalfDenominator1);
        xb_vecN_2x32v vecHalf_Denominator_times_x           = IVP_PACKVRN_2X64W(vecProduct1, 31);
        xb_vecN_2x32v vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v) (1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v) x1), (xb_vecN_2x32v) (vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct1, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v) x1), (xb_vecN_2x32v) (vecOne_minus_half_denominator_times_x));
        xb_vecN_2x32v delta_x = IVP_PACKVRN_2X64W(vecProduct1, 31);
        x1 = IVP_ADDN_2X32(x1, IVP_SLAN_2X32(delta_x, 2));

        vecProduct2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x2), hvecHalfDenominator2);
        IVP_MULAHN_2X16X32_1(vecProduct2, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x2), hvecHalfDenominator2);
        vecHalf_Denominator_times_x = IVP_PACKVRN_2X64W(vecProduct2, 31);
        vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v)(1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x2), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct2, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x2), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        delta_x = IVP_PACKVRN_2X64W(vecProduct2, 31);
        x2 = IVP_ADDN_2X32(x2, IVP_SLAN_2X32(delta_x, 2));

        vecProduct3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x3), hvecHalfDenominator3);
        IVP_MULAHN_2X16X32_1(vecProduct3, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x3), hvecHalfDenominator3);
        vecHalf_Denominator_times_x = IVP_PACKVRN_2X64W(vecProduct3, 31);
        vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v)(1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct3= IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x3), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct3, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x3), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        delta_x = IVP_PACKVRN_2X64W(vecProduct3, 31);
        x3 = IVP_ADDN_2X32(x3, IVP_SLAN_2X32(delta_x, 2));

        vecProduct4 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x4), hvecHalfDenominator4);
        IVP_MULAHN_2X16X32_1(vecProduct4, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x4), hvecHalfDenominator4);
        vecHalf_Denominator_times_x = IVP_PACKVRN_2X64W(vecProduct4, 31);
        vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v)(1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct4 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x4), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct4, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x4), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        delta_x = IVP_PACKVRN_2X64W(vecProduct4, 31);
        x4 = IVP_ADDN_2X32(x4, IVP_SLAN_2X32(delta_x, 2));

      }
      x1 = IVP_SLAN_2X32(x1, 1);
      x2 = IVP_SLAN_2X32(x2, 1);
      x3 = IVP_SLAN_2X32(x3, 1);
      x4 = IVP_SLAN_2X32(x4, 1);

      vboolN_2 vbMaxRestrict = IVP_GTN_2X32(x1, (xb_vecN_2x32v)((1 << 30) - 1));
      x1 = IVP_MOVN_2X32T(x1, IVP_MAXN_2X32(x1, 0x7FFFFFFF), vbMaxRestrict);

      vboolN_2 vbMinRestrict = IVP_GTN_2X32(x1, (xb_vecN_2x32v)(-((1 << 30) - 1)));
      x1 = IVP_MOVN_2X32T(x1, IVP_MINN_2X32(x1, 0x80000000), vbMinRestrict);

      vbMaxRestrict = IVP_GTN_2X32(x2, (xb_vecN_2x32v)((1 << 30) - 1));
      x2 = IVP_MOVN_2X32T(x2, IVP_MAXN_2X32(x2, 0x7FFFFFFF), vbMaxRestrict);

      vbMinRestrict = IVP_GTN_2X32(x2, (xb_vecN_2x32v)(-((1 << 30) - 1)));
      x2 = IVP_MOVN_2X32T(x2, IVP_MINN_2X32(x2, 0x80000000), vbMinRestrict);

      vbMaxRestrict = IVP_GTN_2X32(x3, (xb_vecN_2x32v)((1 << 30) - 1));
      x3 = IVP_MOVN_2X32T(x3, IVP_MAXN_2X32(x3, 0x7FFFFFFF), vbMaxRestrict);

      vbMinRestrict = IVP_GTN_2X32(x3, (xb_vecN_2x32v)(-((1 << 30) - 1)));
      x3 = IVP_MOVN_2X32T(x3, IVP_MINN_2X32(x3, 0x80000000), vbMinRestrict);

      vbMaxRestrict = IVP_GTN_2X32(x4, (xb_vecN_2x32v)((1 << 30) - 1));
      x4 = IVP_MOVN_2X32T(x4, IVP_MAXN_2X32(x4, 0x7FFFFFFF), vbMaxRestrict);

      vbMinRestrict = IVP_GTN_2X32(x4, (xb_vecN_2x32v)(-((1 << 30) - 1)));
      x4 = IVP_MOVN_2X32T(x4, IVP_MINN_2X32(x4, 0x80000000), vbMinRestrict);

      pvecInp = (MORPH_IDT_2Nx8 *)(pInData);
      for (y = 0; y < dim2Size; y++)
      {
        va = MORPH_OP_PRIME_LOAD_2NX8_PP(pvecInp);
        pvecOutput = (MORPH_IDT_2Nx8 *)(pOut + (y * outDataPitch1));
        MORPH_OP_ALIGN_LOAD_2NX8_XP(dvecInp, va, pvecInp, inDataPitch1);
        dvecDiff = MORPH_OP_SUB2NX8(dvecMax, dvecInp);
        dvecLL  = IVP_SEL2NX8(dvecLUT1LL, dvecLUT0LL, dvecDiff);
        IVP_SEL2NX8T(dvecLL, dvecLUT3LL, dvecLUT2LL, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

        dvecLH = IVP_SEL2NX8(dvecLUT1LH, dvecLUT0LH, dvecDiff);
        IVP_SEL2NX8T(dvecLH, dvecLUT3LH, dvecLUT2LH, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

        dvecHL = IVP_SEL2NX8(dvecLUT1HL, dvecLUT0HL, dvecDiff);
        IVP_SEL2NX8T(dvecHL, dvecLUT3HL, dvecLUT2HL, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

        dvecHH = IVP_SEL2NX8(dvecLUT1HH, dvecLUT0HH, dvecDiff);
        IVP_SEL2NX8T(dvecHH, dvecLUT3HH, dvecLUT2HH, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));


        dvecLL0 = IVP_SEL2NX8I(dvecLH, dvecLL, IVP_SELI_8B_INTERLEAVE_1_LO);
        dvecLH0 = IVP_SEL2NX8I(dvecLH, dvecLL, IVP_SELI_8B_INTERLEAVE_1_HI);
        dvecHL0 = IVP_SEL2NX8I(dvecHH, dvecHL, IVP_SELI_8B_INTERLEAVE_1_LO);
        dvecHH0 = IVP_SEL2NX8I(dvecHH, dvecHL, IVP_SELI_8B_INTERLEAVE_1_HI);


        dvecLL = IVP_SEL2NX8I(dvecHL0, dvecLL0, IVP_SELI_INTERLEAVE_1_LO);
        dvecHL = IVP_SEL2NX8I(dvecHL0, dvecLL0, IVP_SELI_INTERLEAVE_1_HI);
        dvecLH = IVP_SEL2NX8I(dvecHH0, dvecLH0, IVP_SELI_INTERLEAVE_1_LO);
        dvecHH = IVP_SEL2NX8I(dvecHH0, dvecLH0, IVP_SELI_INTERLEAVE_1_HI);


        hvecSumExp0 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecLL));
        hvecSumExp1 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecHL));
        hvecSumExp2 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecLH));
        hvecSumExp3 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecHH));

        // Start by multiplying with the reciprocal of SumOfExps
        wvec0 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x1), hvecSumExp0);
        IVP_MULAHN_2X16X32_1(wvec0, IVP_MOVNX16_FROMN_2X32(x1), hvecSumExp0);
        hvecSumExp0 = IVP_PACKVRN_2X64W(wvec0, 31);
        hvecShift = IVP_ADDN_2X32(hvecHeadRoomPlusOne0, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
        hvecSum   = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift, hvecOne));
        hvecSumExp0 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExp0, hvecSum), hvecShift);

        wvec0 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x2), hvecSumExp1);
        IVP_MULAHN_2X16X32_1(wvec0, IVP_MOVNX16_FROMN_2X32(x2), hvecSumExp1);
        hvecSumExp1 = IVP_PACKVRN_2X64W(wvec0, 31);
        hvecShift   = IVP_ADDN_2X32(hvecHeadRoomPlusOne1, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
        hvecSum     = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift, hvecOne));
        hvecSumExp1 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExp1, hvecSum), hvecShift);

        //for 2 and 3
        wvec1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x3), hvecSumExp2);
        IVP_MULAHN_2X16X32_1(wvec1, IVP_MOVNX16_FROMN_2X32(x3), hvecSumExp2);
        hvecSumExp2 = IVP_PACKVRN_2X64W(wvec1, 31);
        hvecShift   = IVP_ADDN_2X32(hvecHeadRoomPlusOne2, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
        hvecSum     = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift, hvecOne));
        hvecSumExp2 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExp2, hvecSum), hvecShift);

        wvec1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x4), hvecSumExp3);
        IVP_MULAHN_2X16X32_1(wvec1, IVP_MOVNX16_FROMN_2X32(x4), hvecSumExp3);
        hvecSumExp3 = IVP_PACKVRNRN_2X64W(wvec1, 31);
        hvecShift   = IVP_ADDN_2X32(hvecHeadRoomPlusOne3, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
        hvecSum     = IVP_SLAN_2X32(hvecOne,IVP_SUBN_2X32(hvecShift, hvecOne));
        hvecSumExp3 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExp3, hvecSum), hvecShift);

        vecRes0 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecSumExp1, hvecSumExp0, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));
        vecRes1 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecSumExp3, hvecSumExp2, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));
        MORPH_OP_CLAMP(vecRes0, vecRes1, dvecRes);

        MORPH_OP_ALIGN_STORE_2NX8_XP(dvecRes, vaStore, pvecOutput, dim1Size - x);
        MORPH_OP_STORE_FLUSH(vaStore, pvecOutput);
      }
    }
  }
  return XI_ERROR_STATUS();
}

static XI_ERR_TYPE MAKE_NAME2(softmaxA3D, DIM3_LUT)(const xi_pTile3D inTile,
                                                    const xi_pArray softmaxArray,
                                                    xi_pTile3D outTile,
                                                    const xi_cnn_softmaxA_params  * pparams)
{
  /* Error Checks */
 XI_ERROR_CHECKS()
 {
   MORPH_IDT_CHECK(inTile);
   MORPH_IDT_CHECK(outTile);
   XI_CHECK_ARRAY_S32(softmaxArray);
   XI_CHECK_TILE3D_SIZE_EQ(inTile, outTile);
   XI_CHECK_POINTER(pparams);
   XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
   XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
   XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(softmaxArray) == 256, XI_ERR_BADARG, \
                   "\nsoftmaxArray LUT Size = %d, softmaxArray LUT Size should be equal to 256", XI_ARRAY_GET_WIDTH(softmaxArray));
   XI_CHECK_ERROR(XI_TILE3D_GET_DATA_ORDER(inTile) == XI_TILE3D_GET_DATA_ORDER(outTile), XI_ERR_BADARG, \
                  "\nData Order of InputTile = %d, OutputTile= %d\nData order of input and output tile should be same", \
                  XI_TILE3D_GET_DATA_ORDER(inTile), XI_TILE3D_GET_DATA_ORDER(outTile));
 }


  /* Getting parameters from the tile structures */
  const int32_t dim1Size      = XI_TILE3D_GET_DIM1(inTile);
  const int32_t dim2Size      = XI_TILE3D_GET_DIM2(inTile);
  const int32_t inDataPitch1  = XI_TILE3D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2  = XI_TILE3D_GET_DIM2_PITCH(inTile);
  const int32_t dim3Size      = XI_TILE3D_GET_DIM3(inTile);
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
  MORPH_IDT_SCALAR *pInput    = (MORPH_IDT_SCALAR *) XI_TILE3D_GET_DATA_PTR(inTile);
  MORPH_IDT_SCALAR *pOutput   = (MORPH_IDT_SCALAR *) XI_TILE3D_GET_DATA_PTR(outTile);
  int32_t x, y, z;

  static const int kAccumulationIntegerBits = 12;

  int32_t *LUTOut = (int32_t *) XI_ARRAY_GET_BUFF_PTR(softmaxArray);



  MORPH_IDT_SCALAR *pInData, *pOut;
  MORPH_IDT_2Nx8 *restrict pvecInp;
  valign va, vaStore;
  MORPH_IDT_2Nx8 dvecInp, dvecMax, dvecDiff, dvecRes;

  xb_vecN_2x32v hvecSumExp0, hvecSumExp1, hvecSumExp2, hvecSumExp3, hvecSum;
  xb_vecN_2x32v hvecSumExpInter0, hvecSumExpInter1, hvecSumExpInter2, hvecSumExpInter3;

  xb_vecN_2x32v hvecOne = 1;
  xb_vecN_2x32v hvecShift = 0;
  xb_vecNx16 vecRes0, vecRes1;
  xb_vecN_2x64w wvec0, wvec1;
  MORPH_IDT_2Nx8 *restrict pvecOutput;;
  xb_vecN_2x64w vecProduct1, vecProduct2, vecProduct3, vecProduct4;

  vaStore = IVP_ZALIGN();
  xb_vec2Nx8 *restrict dvecLUTBase = (xb_vec2Nx8 *)LUTOut;
  valign va1                       = IVP_LA2NX8_PP(dvecLUTBase);
  xb_vec2Nx8 dvecLUT0LL, dvecLUT1LL, dvecLUT2LL, dvecLUT3LL, dvecLUT0LH, dvecLUT1LH, dvecLUT2LH, dvecLUT3LH;
  xb_vec2Nx8 dvecLUT0HL, dvecLUT1HL, dvecLUT2HL, dvecLUT3HL, dvecLUT0HH, dvecLUT1HH, dvecLUT2HH, dvecLUT3HH;
  xb_vec2Nx8 dvecLL0, dvecLL, dvecLH0, dvecLH, dvecHL0, dvecHL, dvecHH0, dvecHH;


  IVP_LA2NX8_IP(dvecLUT0LL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT1LL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT2LL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT3LL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT0LH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT1LH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT2LH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT3LH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT0HL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT1HL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT2HL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT3HL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT0HH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT1HH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT2HH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT3HH, va1, dvecLUTBase);

  int32_t vectorizationWidth = XCHAL_IVPN_SIMD_WIDTH * 2;

  for (y = 0; y < dim2Size; y++)     /* along the image depth(WHD)/height(DWH) */
  {
    for (x = 0; x < dim1Size; x += vectorizationWidth)   /* along the image height(WHD)/width(DWH) */
    {

      dvecMax    = MORPH_IDT_SCALAR_MIN;
      pInData    = (MORPH_IDT_SCALAR *) (pInput + ((y * inDataPitch1) + x));
      pOut       = pOutput + ((y * outDataPitch1) + x);

      pvecInp = (MORPH_IDT_2Nx8 *)(pInData);
      for (z = 0; z < dim3Size; z++)
      {
        va = MORPH_OP_PRIME_LOAD_2NX8_PP(pvecInp);
        MORPH_OP_ALIGN_LOAD_2NX8_XP(dvecInp, va, pvecInp, inDataPitch2);
        dvecMax = MORPH_OP_MAX2NX8(dvecMax, dvecInp);
      }

        hvecSum = 0;

        hvecSumExp0 =  0;
        hvecSumExp1 =  0;
        hvecSumExp2 =  0;
        hvecSumExp3 =  0;

      pvecInp = (MORPH_IDT_2Nx8 *)(pInData);
      for (z = 0; z < dim3Size; z++)
      {
        va = MORPH_OP_PRIME_LOAD_2NX8_PP(pvecInp);
        MORPH_OP_ALIGN_LOAD_2NX8_XP(dvecInp, va, pvecInp, inDataPitch2);
        dvecDiff = MORPH_OP_SUB2NX8(dvecMax, dvecInp);                         // equivalent of this int32 input_diff = (int32)(input_data[Offset(input_dims, c, x, y, b)]) -  max_in_row;

        // implements (input_diff >= diff_min) along with out of boundary condition
        //SEL based lookup table
        dvecLL = IVP_SEL2NX8(dvecLUT1LL, dvecLUT0LL, dvecDiff);
        IVP_SEL2NX8T(dvecLL, dvecLUT3LL, dvecLUT2LL, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

        dvecLH = IVP_SEL2NX8(dvecLUT1LH, dvecLUT0LH, dvecDiff);
        IVP_SEL2NX8T(dvecLH, dvecLUT3LH, dvecLUT2LH, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

        dvecHL = IVP_SEL2NX8(dvecLUT1HL, dvecLUT0HL, dvecDiff);
        IVP_SEL2NX8T(dvecHL, dvecLUT3HL, dvecLUT2HL, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

        dvecHH = IVP_SEL2NX8(dvecLUT1HH, dvecLUT0HH, dvecDiff);
        IVP_SEL2NX8T(dvecHH, dvecLUT3HH, dvecLUT2HH, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));


        dvecLL0 = IVP_SEL2NX8I(dvecLH, dvecLL, IVP_SELI_8B_INTERLEAVE_1_LO);
        dvecLH0 = IVP_SEL2NX8I(dvecLH, dvecLL, IVP_SELI_8B_INTERLEAVE_1_HI);
        dvecHL0 = IVP_SEL2NX8I(dvecHH, dvecHL, IVP_SELI_8B_INTERLEAVE_1_LO);
        dvecHH0 = IVP_SEL2NX8I(dvecHH, dvecHL, IVP_SELI_8B_INTERLEAVE_1_HI);


        dvecLL = IVP_SEL2NX8I(dvecHL0, dvecLL0, IVP_SELI_INTERLEAVE_1_LO);
        dvecHL = IVP_SEL2NX8I(dvecHL0, dvecLL0, IVP_SELI_INTERLEAVE_1_HI);
        dvecLH = IVP_SEL2NX8I(dvecHH0, dvecLH0, IVP_SELI_INTERLEAVE_1_LO);
        dvecHH = IVP_SEL2NX8I(dvecHH0, dvecLH0, IVP_SELI_INTERLEAVE_1_HI);


        hvecSumExpInter0 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecLL));
        hvecSumExpInter1 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecHL));
        hvecSumExpInter2 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecLH));
        hvecSumExpInter3 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecHH));
        //end of lookup table values

        /* sum of exponentials is right shifted by 12 */
        hvecSumExp0 = IVP_ADDN_2X32(IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpInter0), 12), hvecSumExp0);
        hvecSumExp1 = IVP_ADDN_2X32(IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpInter1), 12), hvecSumExp1);
        hvecSumExp2 = IVP_ADDN_2X32(IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpInter2), 12), hvecSumExp2);
        hvecSumExp3 = IVP_ADDN_2X32(IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpInter3), 12), hvecSumExp3);
      }

      xb_vecN_2x32v hvecShiftedSumMinusOne0, hvecShiftedSumMinusOne1, hvecShiftedSumMinusOne2, hvecShiftedSumMinusOne3;
      xb_vecN_2x32v hvecHeadRoomPlusOne0, hvecHeadRoomPlusOne1, hvecHeadRoomPlusOne2, hvecHeadRoomPlusOne3;

      // This is the number of bits to the left of the binary point above 1.0.
      // Consider fixed_sum_of_exps=1.25.  In that case shifted_scale=0.8 and
      // no later adjustment will be needed.
      hvecHeadRoomPlusOne0 = IVP_NSAUN_2X32(hvecSumExp0);
      hvecHeadRoomPlusOne1 = IVP_NSAUN_2X32(hvecSumExp1);
      hvecHeadRoomPlusOne2 = IVP_NSAUN_2X32(hvecSumExp2);
      hvecHeadRoomPlusOne3 = IVP_NSAUN_2X32(hvecSumExp3);

      hvecShiftedSumMinusOne0 = IVP_SUBN_2X32U(IVP_SLAN_2X32(hvecSumExp0, hvecHeadRoomPlusOne0), (xb_vecN_2x32Uv)(1 << 31));
      hvecShiftedSumMinusOne1 = IVP_SUBN_2X32U(IVP_SLAN_2X32(hvecSumExp1, hvecHeadRoomPlusOne1), (xb_vecN_2x32Uv)(1 << 31));
      hvecShiftedSumMinusOne2 = IVP_SUBN_2X32U(IVP_SLAN_2X32(hvecSumExp2, hvecHeadRoomPlusOne2), (xb_vecN_2x32Uv)(1 << 31));
      hvecShiftedSumMinusOne3 = IVP_SUBN_2X32U(IVP_SLAN_2X32(hvecSumExp3, hvecHeadRoomPlusOne3), (xb_vecN_2x32Uv)(1 << 31));


      hvecHeadRoomPlusOne0 = IVP_SUBN_2X32((xb_vecN_2x32Uv)kAccumulationIntegerBits, hvecHeadRoomPlusOne0);
      hvecHeadRoomPlusOne1 = IVP_SUBN_2X32((xb_vecN_2x32Uv)kAccumulationIntegerBits, hvecHeadRoomPlusOne1);
      hvecHeadRoomPlusOne2 = IVP_SUBN_2X32((xb_vecN_2x32Uv)kAccumulationIntegerBits, hvecHeadRoomPlusOne2);
      hvecHeadRoomPlusOne3 = IVP_SUBN_2X32((xb_vecN_2x32Uv)kAccumulationIntegerBits, hvecHeadRoomPlusOne3);

      // FixedPoint0 shifted_scale = gemmlowp::one_over_one_plus_x_for_x_in_0_1( FixedPoint0::FromRaw(shifted_sum_minus_one));
      // Strat of one_over_one_plus_x_for_x_in_0_1
      const int32_t constant_48_over_17     = 1515870810;                //Q2.29 format
      const int32_t constant_neg_32_over_17 = -1010580540;               //Q2.29 format

      vecProduct1 = IVP_MULN_2X16X32_0((xb_vecNx16) 1, (xb_vecN_2x32v) (ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct1, (xb_vecNx16)1, IVP_ADDN_2X32(hvecShiftedSumMinusOne0, (xb_vecN_2x32v)1));
      xb_vecN_2x32v hvecHalfDenominator1 = IVP_PACKVRNRN_2X64W(vecProduct1, 1);

      vecProduct1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v) constant_neg_32_over_17), hvecHalfDenominator1);
      IVP_MULAHN_2X16X32_1(vecProduct1, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v) constant_neg_32_over_17), hvecHalfDenominator1);


      vecProduct2 = IVP_MULN_2X16X32_0((xb_vecNx16)1, (xb_vecN_2x32v)(ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct2, (xb_vecNx16)1, IVP_ADDN_2X32(hvecShiftedSumMinusOne1, (xb_vecN_2x32v)1));
      xb_vecN_2x32v hvecHalfDenominator2 = IVP_PACKVRNRN_2X64W(vecProduct2, 1);

      vecProduct2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator2);
      IVP_MULAHN_2X16X32_1(vecProduct2, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator2);

      vecProduct3 = IVP_MULN_2X16X32_0((xb_vecNx16)1, (xb_vecN_2x32v)(ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct3, (xb_vecNx16)1, IVP_ADDN_2X32(hvecShiftedSumMinusOne2, (xb_vecN_2x32v)1));
      xb_vecN_2x32v hvecHalfDenominator3 = IVP_PACKVRNRN_2X64W(vecProduct3, 1);

      vecProduct3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator3);
      IVP_MULAHN_2X16X32_1(vecProduct3, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator3);

      vecProduct4 = IVP_MULN_2X16X32_0((xb_vecNx16)1, (xb_vecN_2x32v)(ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct4, (xb_vecNx16)1, IVP_ADDN_2X32(hvecShiftedSumMinusOne3, (xb_vecN_2x32v)1));
      xb_vecN_2x32v hvecHalfDenominator4 = IVP_PACKVRNRN_2X64W(vecProduct4, 1);

      vecProduct4 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator4);
      IVP_MULAHN_2X16X32_1(vecProduct4, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator4);

      xb_vecN_2x32v x1 = IVP_ADDN_2X32((IVP_PACKVRN_2X64W(vecProduct1, 31)), (xb_vecN_2x32v)constant_48_over_17);

      xb_vecN_2x32v x2 = IVP_ADDN_2X32((IVP_PACKVRN_2X64W(vecProduct2, 31)), (xb_vecN_2x32v)constant_48_over_17);

      xb_vecN_2x32v x3 = IVP_ADDN_2X32((IVP_PACKVRN_2X64W(vecProduct3, 31)), (xb_vecN_2x32v)constant_48_over_17);

      xb_vecN_2x32v x4 = IVP_ADDN_2X32((IVP_PACKVRN_2X64W(vecProduct4, 31)), (xb_vecN_2x32v)constant_48_over_17);

      for (int i = 0; i < 3; i++)
      {
        vecProduct1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v) x1), hvecHalfDenominator1);
        IVP_MULAHN_2X16X32_1(vecProduct1, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v) x1), hvecHalfDenominator1);
        xb_vecN_2x32v vecHalf_Denominator_times_x           = IVP_PACKVRN_2X64W(vecProduct1, 31);
        xb_vecN_2x32v vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v) (1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v) x1), (xb_vecN_2x32v) (vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct1, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v) x1), (xb_vecN_2x32v) (vecOne_minus_half_denominator_times_x));
        xb_vecN_2x32v delta_x = IVP_PACKVRN_2X64W(vecProduct1, 31);
        x1 = IVP_ADDN_2X32(x1, IVP_SLAN_2X32(delta_x, 2));

        vecProduct2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x2), hvecHalfDenominator2);
        IVP_MULAHN_2X16X32_1(vecProduct2, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x2), hvecHalfDenominator2);
        vecHalf_Denominator_times_x = IVP_PACKVRN_2X64W(vecProduct2, 31);
        vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v)(1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x2), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct2, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x2), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        delta_x = IVP_PACKVRN_2X64W(vecProduct2, 31);
        x2 = IVP_ADDN_2X32(x2, IVP_SLAN_2X32(delta_x, 2));

        vecProduct3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x3), hvecHalfDenominator3);
        IVP_MULAHN_2X16X32_1(vecProduct3, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x3), hvecHalfDenominator3);
        vecHalf_Denominator_times_x = IVP_PACKVRN_2X64W(vecProduct3, 31);
        vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v)(1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct3= IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x3), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct3, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x3), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        delta_x = IVP_PACKVRN_2X64W(vecProduct3, 31);
        x3 = IVP_ADDN_2X32(x3, IVP_SLAN_2X32(delta_x, 2));

        vecProduct4 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x4), hvecHalfDenominator4);
        IVP_MULAHN_2X16X32_1(vecProduct4, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x4), hvecHalfDenominator4);
        vecHalf_Denominator_times_x = IVP_PACKVRN_2X64W(vecProduct4, 31);
        vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v)(1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct4 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x4), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct4, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x4), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        delta_x = IVP_PACKVRN_2X64W(vecProduct4, 31);
        x4 = IVP_ADDN_2X32(x4, IVP_SLAN_2X32(delta_x, 2));

      }
      x1 = IVP_SLAN_2X32(x1, 1);
      x2 = IVP_SLAN_2X32(x2, 1);
      x3 = IVP_SLAN_2X32(x3, 1);
      x4 = IVP_SLAN_2X32(x4, 1);

      vboolN_2 vbMaxRestrict = IVP_GTN_2X32(x1, (xb_vecN_2x32v)((1 << 30) - 1));
      x1 = IVP_MOVN_2X32T(x1, IVP_MAXN_2X32(x1, 0x7FFFFFFF), vbMaxRestrict);

      vboolN_2 vbMinRestrict = IVP_GTN_2X32(x1, (xb_vecN_2x32v)(-((1 << 30) - 1)));
      x1 = IVP_MOVN_2X32T(x1, IVP_MINN_2X32(x1, 0x80000000), vbMinRestrict);

      vbMaxRestrict = IVP_GTN_2X32(x2, (xb_vecN_2x32v)((1 << 30) - 1));
      x2 = IVP_MOVN_2X32T(x2, IVP_MAXN_2X32(x2, 0x7FFFFFFF), vbMaxRestrict);

      vbMinRestrict = IVP_GTN_2X32(x2, (xb_vecN_2x32v)(-((1 << 30) - 1)));
      x2 = IVP_MOVN_2X32T(x2, IVP_MINN_2X32(x2, 0x80000000), vbMinRestrict);

      vbMaxRestrict = IVP_GTN_2X32(x3, (xb_vecN_2x32v)((1 << 30) - 1));
      x3 = IVP_MOVN_2X32T(x3, IVP_MAXN_2X32(x3, 0x7FFFFFFF), vbMaxRestrict);

      vbMinRestrict = IVP_GTN_2X32(x3, (xb_vecN_2x32v)(-((1 << 30) - 1)));
      x3 = IVP_MOVN_2X32T(x3, IVP_MINN_2X32(x3, 0x80000000), vbMinRestrict);

      vbMaxRestrict = IVP_GTN_2X32(x4, (xb_vecN_2x32v)((1 << 30) - 1));
      x4 = IVP_MOVN_2X32T(x4, IVP_MAXN_2X32(x4, 0x7FFFFFFF), vbMaxRestrict);

      vbMinRestrict = IVP_GTN_2X32(x4, (xb_vecN_2x32v)(-((1 << 30) - 1)));
      x4 = IVP_MOVN_2X32T(x4, IVP_MINN_2X32(x4, 0x80000000), vbMinRestrict);

      pvecInp = (MORPH_IDT_2Nx8 *)(pInData);
      for (z = 0; z < dim3Size; z++)
      {
        va = MORPH_OP_PRIME_LOAD_2NX8_PP(pvecInp);
        pvecOutput = (MORPH_IDT_2Nx8 *)(pOut + (z * outDataPitch2));
        MORPH_OP_ALIGN_LOAD_2NX8_XP(dvecInp, va, pvecInp, inDataPitch2);
        dvecDiff = MORPH_OP_SUB2NX8(dvecMax, dvecInp);
        dvecLL  = IVP_SEL2NX8(dvecLUT1LL, dvecLUT0LL, dvecDiff);
        IVP_SEL2NX8T(dvecLL, dvecLUT3LL, dvecLUT2LL, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

        dvecLH = IVP_SEL2NX8(dvecLUT1LH, dvecLUT0LH, dvecDiff);
        IVP_SEL2NX8T(dvecLH, dvecLUT3LH, dvecLUT2LH, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

        dvecHL = IVP_SEL2NX8(dvecLUT1HL, dvecLUT0HL, dvecDiff);
        IVP_SEL2NX8T(dvecHL, dvecLUT3HL, dvecLUT2HL, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

        dvecHH = IVP_SEL2NX8(dvecLUT1HH, dvecLUT0HH, dvecDiff);
        IVP_SEL2NX8T(dvecHH, dvecLUT3HH, dvecLUT2HH, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));


        dvecLL0 = IVP_SEL2NX8I(dvecLH, dvecLL, IVP_SELI_8B_INTERLEAVE_1_LO);
        dvecLH0 = IVP_SEL2NX8I(dvecLH, dvecLL, IVP_SELI_8B_INTERLEAVE_1_HI);
        dvecHL0 = IVP_SEL2NX8I(dvecHH, dvecHL, IVP_SELI_8B_INTERLEAVE_1_LO);
        dvecHH0 = IVP_SEL2NX8I(dvecHH, dvecHL, IVP_SELI_8B_INTERLEAVE_1_HI);


        dvecLL = IVP_SEL2NX8I(dvecHL0, dvecLL0, IVP_SELI_INTERLEAVE_1_LO);
        dvecHL = IVP_SEL2NX8I(dvecHL0, dvecLL0, IVP_SELI_INTERLEAVE_1_HI);
        dvecLH = IVP_SEL2NX8I(dvecHH0, dvecLH0, IVP_SELI_INTERLEAVE_1_LO);
        dvecHH = IVP_SEL2NX8I(dvecHH0, dvecLH0, IVP_SELI_INTERLEAVE_1_HI);


        hvecSumExp0 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecLL));
        hvecSumExp1 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecHL));
        hvecSumExp2 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecLH));
        hvecSumExp3 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecHH));

        // Start by multiplying with the reciprocal of SumOfExps
        wvec0 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x1), hvecSumExp0);
        IVP_MULAHN_2X16X32_1(wvec0, IVP_MOVNX16_FROMN_2X32(x1), hvecSumExp0);
        hvecSumExp0 = IVP_PACKVRN_2X64W(wvec0, 31);
        hvecShift = IVP_ADDN_2X32(hvecHeadRoomPlusOne0, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
        hvecSum   = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift, hvecOne));
        hvecSumExp0 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExp0, hvecSum), hvecShift);

        wvec0 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x2), hvecSumExp1);
        IVP_MULAHN_2X16X32_1(wvec0, IVP_MOVNX16_FROMN_2X32(x2), hvecSumExp1);
        hvecSumExp1 = IVP_PACKVRN_2X64W(wvec0, 31);
        hvecShift   = IVP_ADDN_2X32(hvecHeadRoomPlusOne1, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
        hvecSum     = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift, hvecOne));
        hvecSumExp1 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExp1, hvecSum), hvecShift);

        //for 2 and 3
        wvec1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x3), hvecSumExp2);
        IVP_MULAHN_2X16X32_1(wvec1, IVP_MOVNX16_FROMN_2X32(x3), hvecSumExp2);
        hvecSumExp2 = IVP_PACKVRN_2X64W(wvec1, 31);
        hvecShift   = IVP_ADDN_2X32(hvecHeadRoomPlusOne2, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
        hvecSum     = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift, hvecOne));
        hvecSumExp2 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExp2, hvecSum), hvecShift);

        wvec1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x4), hvecSumExp3);
        IVP_MULAHN_2X16X32_1(wvec1, IVP_MOVNX16_FROMN_2X32(x4), hvecSumExp3);
        hvecSumExp3 = IVP_PACKVRNRN_2X64W(wvec1, 31);
        hvecShift   = IVP_ADDN_2X32(hvecHeadRoomPlusOne3, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
        hvecSum     = IVP_SLAN_2X32(hvecOne,IVP_SUBN_2X32(hvecShift, hvecOne));
        hvecSumExp3 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExp3, hvecSum), hvecShift);

        vecRes0 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecSumExp1, hvecSumExp0, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));
        vecRes1 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecSumExp3, hvecSumExp2, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));
        MORPH_OP_CLAMP(vecRes0, vecRes1, dvecRes);

        MORPH_OP_ALIGN_STORE_2NX8_XP(dvecRes, vaStore, pvecOutput, dim1Size - x);
        MORPH_OP_STORE_FLUSH(vaStore, pvecOutput);
      }
    }
  }
  return XI_ERROR_STATUS();
}

/* xiSoftmaxA3D_U8_DWH_LUTReused */
/* xiSoftmaxA3D_S8_DWH_LUTReused */
XI_ERR_TYPE  MAKE_NAME2(xiSoftmaxA3D, DWH_LUTReused)(const xi_pTile3D inTile,
                                                     const xi_pArray softmaxArray,
                                                     xi_pTile3D outTile,
                                                     const xi_cnn_softmaxA_params  * pparams,
                                                     xi_pArray tmp,
                                                     xi_pArray scratchArray)
{
  /* Error Checks */
  XI_ERROR_CHECKS()
  {
    MORPH_IDT_CHECK(inTile);
    MORPH_IDT_CHECK(outTile);
    XI_CHECK_TILE3D_SIZE_EQ(inTile, outTile);
    XI_CHECK_ARRAY_S32(softmaxArray);
    XI_CHECK_ARRAY_S32(tmp);
    XI_CHECK_POINTER(pparams);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(softmaxArray) == 256, XI_ERR_BADARG, \
                   "\nsoftmaxArray LUT Size = %d, softmaxArray LUT Size should be equal to 256", XI_ARRAY_GET_WIDTH(softmaxArray));
    XI_CHECK_TILE3D_DATA_ORDER(inTile, XI_DWH);
    XI_CHECK_TILE3D_DATA_ORDER(outTile, XI_DWH);
    XI_CHECK_ERROR(XI_ARRAY_IS_PTR_ALIGNED_2NX8(tmp), XI_ERR_IALIGNMENT, \
                   "\narray tmp pointer should be %d aligned", XCHAL_IVPN_SIMD_WIDTH * 2);
  }


  /* Getting parameters from the tile structures */
  const int32_t dim1Size      = XI_TILE3D_GET_DIM1(inTile);
  const int32_t dim2Size      = XI_TILE3D_GET_DIM2(inTile);
  const int32_t inDataPitch1  = XI_TILE3D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2  = XI_TILE3D_GET_DIM2_PITCH(inTile);
  const int32_t dim3Size      = XI_TILE3D_GET_DIM3(inTile);
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
  MORPH_IDT_SCALAR *pInput    = (MORPH_IDT_SCALAR *) XI_TILE3D_GET_DATA_PTR(inTile);
  MORPH_IDT_SCALAR *pOutput   = (MORPH_IDT_SCALAR *) XI_TILE3D_GET_DATA_PTR(outTile);
  int32_t x, y, z;

  static const int kAccumulationIntegerBits = 12;

  int32_t *LUTOut = (int32_t *) XI_ARRAY_GET_BUFF_PTR(softmaxArray);

  int32_t *ptrScratch = (int32_t *) XI_ARRAY_GET_DATA_PTR(tmp);
  xb_vecN_2x32v *__restrict phvecTemp;

  MORPH_IDT_SCALAR *pInData, *pOut;
  MORPH_IDT_2Nx8 *__restrict pvecInp;
  valign va, vaStore;
  MORPH_IDT_2Nx8 dvecInp, dvecMax, dvecDiff, dvecRes;
  xb_vecNx16 vec0, vec1;

  xb_vecN_2x32v hvecSumExp0, hvecSumExp1, hvecSumExp2, hvecSumExp3, vecSumExp;

  xb_vecNx16 vecRes0, vecRes1;
  xb_vecN_2x64w wvec0, wvec1;
  MORPH_IDT_2Nx8 *__restrict pvecOutput;
  xb_vecN_2x64w vecProduct;
  vaStore = IVP_ZALIGN();
  xb_vec2Nx8 *__restrict dvecLUTBase = (xb_vec2Nx8 *) LUTOut;
  valign va1                         = IVP_LA2NX8_PP(dvecLUTBase);
  xb_vec2Nx8 dvecLUT0LL, dvecLUT1LL, dvecLUT2LL, dvecLUT3LL, dvecLUT0LH, dvecLUT1LH, dvecLUT2LH, dvecLUT3LH;
  xb_vec2Nx8 dvecLUT0HL, dvecLUT1HL, dvecLUT2HL, dvecLUT3HL, dvecLUT0HH, dvecLUT1HH, dvecLUT2HH, dvecLUT3HH;
  xb_vec2Nx8 dvecLL0, dvecLL, dvecLH0, dvecLH, dvecHL0, dvecHL, dvecHH0, dvecHH;


  IVP_LA2NX8_IP(dvecLUT0LL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT1LL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT2LL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT3LL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT0LH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT1LH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT2LH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT3LH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT0HL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT1HL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT2HL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT3HL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT0HH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT1HH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT2HH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT3HH, va1, dvecLUTBase);


  int16_t max_in_row;
  int32_t sum_of_exps;               //Q12.19
  for (z = 0; z < dim3Size; z++)     /* along the image depth(WHD)/height(DWH) */
  {
    for (y = 0; y < dim2Size; y++)   /* along the image height(WHD)/width(DWH) */
    {
      max_in_row = MORPH_IDT_SCALAR_MIN;

      dvecMax    = MORPH_IDT_SCALAR_MIN;
      pInData    = (MORPH_IDT_SCALAR *) (pInput + ((z * inDataPitch2) + (y * inDataPitch1) + 0));
      pOut       = pOutput + ((z * outDataPitch2) + (y * outDataPitch1) + 0);
      pvecInp    = (MORPH_IDT_2Nx8 *) (pInData);
      pvecOutput = (MORPH_IDT_2Nx8 *) (pOut);
      va         = MORPH_OP_PRIME_LOAD_2NX8_PP(pvecInp);
      for (x = 0; x < dim1Size - (XCHAL_IVPN_SIMD_WIDTH * 2); x += (XCHAL_IVPN_SIMD_WIDTH * 2))
      {
        MORPH_OP_ALIGN_LOAD_2NX8_IP(dvecInp, va, pvecInp);
        dvecMax = MORPH_OP_MAX2NX8(dvecMax, dvecInp);
      }
      if (x < dim1Size)
      {
        MORPH_OP_ALIGN_LOAD_2Nx8_VARIABLE(dvecInp, va, pvecInp, dim1Size - x);
        vbool2N remXBool = IVP_LT2NX8(IVP_SEQ2NX8(), dim1Size - x);
        MORPH_IDT_2Nx8 dvecInpVar = MORPH_OP_MOV_2NX8T(dvecInp, MORPH_IDT_SCALAR_MIN, remXBool);
        dvecMax = MORPH_OP_MAX2NX8(dvecMax, dvecInpVar);
      }
      vec0       = MORPH_OP_UNPK_2NX8_EVEN(dvecMax);
      vec1       = MORPH_OP_UNPK_2NX8_ODD(dvecMax);
      max_in_row = (int16_t) (IVP_RMAXNX16(IVP_MAXNX16(vec1, vec0)));


      // vector implementation of above code which is sum of exponents along the depth
      dvecMax     = (MORPH_IDT_2Nx8) max_in_row;
      sum_of_exps = 0;
      vecSumExp   = 0;

      pvecInp   = (MORPH_IDT_2Nx8 *) (pInData);
      va        = MORPH_OP_PRIME_LOAD_2NX8_PP(pvecInp);
      phvecTemp = (xb_vecN_2x32v *) ptrScratch;
      for (x = 0; x < dim1Size - (XCHAL_IVPN_SIMD_WIDTH * 2); x += (XCHAL_IVPN_SIMD_WIDTH * 2))
      {
        MORPH_OP_ALIGN_LOAD_2Nx8_VARIABLE(dvecInp, va, pvecInp, dim1Size - x);
        dvecDiff = MORPH_OP_SUB2NX8(dvecMax, dvecInp);                         // equivalent of this int32 input_diff = (int32)(input_data[Offset(input_dims, c, x, y, b)]) -  max_in_row;

        // implements (input_diff >= diff_min) along with out of boundary condition
        //SEL based lookup table
        dvecLL = IVP_SEL2NX8(dvecLUT1LL, dvecLUT0LL, dvecDiff);
        IVP_SEL2NX8T(dvecLL, dvecLUT3LL, dvecLUT2LL, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

        dvecLH = IVP_SEL2NX8(dvecLUT1LH, dvecLUT0LH, dvecDiff);
        IVP_SEL2NX8T(dvecLH, dvecLUT3LH, dvecLUT2LH, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

        dvecHL = IVP_SEL2NX8(dvecLUT1HL, dvecLUT0HL, dvecDiff);
        IVP_SEL2NX8T(dvecHL, dvecLUT3HL, dvecLUT2HL, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

        dvecHH = IVP_SEL2NX8(dvecLUT1HH, dvecLUT0HH, dvecDiff);
        IVP_SEL2NX8T(dvecHH, dvecLUT3HH, dvecLUT2HH, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));


        dvecLL0 = IVP_SEL2NX8I(dvecLH, dvecLL, IVP_SELI_8B_INTERLEAVE_1_LO);
        dvecLH0 = IVP_SEL2NX8I(dvecLH, dvecLL, IVP_SELI_8B_INTERLEAVE_1_HI);
        dvecHL0 = IVP_SEL2NX8I(dvecHH, dvecHL, IVP_SELI_8B_INTERLEAVE_1_LO);
        dvecHH0 = IVP_SEL2NX8I(dvecHH, dvecHL, IVP_SELI_8B_INTERLEAVE_1_HI);


        dvecLL = IVP_SEL2NX8I(dvecHL0, dvecLL0, IVP_SELI_INTERLEAVE_1_LO);
        dvecHL = IVP_SEL2NX8I(dvecHL0, dvecLL0, IVP_SELI_INTERLEAVE_1_HI);
        dvecLH = IVP_SEL2NX8I(dvecHH0, dvecLH0, IVP_SELI_INTERLEAVE_1_LO);
        dvecHH = IVP_SEL2NX8I(dvecHH0, dvecLH0, IVP_SELI_INTERLEAVE_1_HI);


        hvecSumExp0 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecLL));
        hvecSumExp1 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecHL));
        hvecSumExp2 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecLH));
        hvecSumExp3 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecHH));
        //end of lookup table values
        IVP_SVN_2X32_IP(hvecSumExp0, phvecTemp, (XCHAL_IVPN_SIMD_WIDTH * 2));
        IVP_SVN_2X32_IP(hvecSumExp1, phvecTemp, (XCHAL_IVPN_SIMD_WIDTH * 2));
        IVP_SVN_2X32_IP(hvecSumExp2, phvecTemp, (XCHAL_IVPN_SIMD_WIDTH * 2));
        IVP_SVN_2X32_IP(hvecSumExp3, phvecTemp, (XCHAL_IVPN_SIMD_WIDTH * 2));

        /* sum of exponentials is right shifted by 12 */
        hvecSumExp0 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16) 1, hvecSumExp0), 12);
        hvecSumExp1 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16) 1, hvecSumExp1), 12);
        hvecSumExp2 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16) 1, hvecSumExp2), 12);
        hvecSumExp3 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16) 1, hvecSumExp3), 12);
        vecSumExp  += hvecSumExp0 + hvecSumExp1 + hvecSumExp2 + hvecSumExp3;
      }
      MORPH_OP_ALIGN_LOAD_2Nx8_VARIABLE(dvecInp, va, pvecInp, dim1Size - x);
      dvecDiff = MORPH_OP_SUB2NX8(dvecMax, dvecInp);
      int32_t numElements        = XT_MINU((dim1Size - x), 255);
      xb_vecN_2x32v hvecElements = (xb_vecN_2x32v) numElements;
      vboolN_2 hvbCheckLL, hvbCheckLH, hvbCheckHL, hvbCheckHH;

      hvbCheckLL = IVP_GTN_2X32(hvecElements, IVP_SEQN_2X32());
      hvbCheckLH = IVP_GTN_2X32(hvecElements - (xb_vecN_2x32v) (XCHAL_IVPN_SIMD_WIDTH / 2 ) , IVP_SEQN_2X32());
      hvbCheckHL = IVP_GTN_2X32(hvecElements - (xb_vecN_2x32v) (XCHAL_IVPN_SIMD_WIDTH ), IVP_SEQN_2X32());
      hvbCheckHH = IVP_GTN_2X32(hvecElements - (xb_vecN_2x32v) ((XCHAL_IVPN_SIMD_WIDTH * 3 ) / 2), IVP_SEQN_2X32());


      //SEL based lookup table
      dvecLL = IVP_SEL2NX8(dvecLUT1LL, dvecLUT0LL, dvecDiff);
      IVP_SEL2NX8T(dvecLL, dvecLUT3LL, dvecLUT2LL, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

      dvecLH = IVP_SEL2NX8(dvecLUT1LH, dvecLUT0LH, dvecDiff);
      IVP_SEL2NX8T(dvecLH, dvecLUT3LH, dvecLUT2LH, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

      dvecHL = IVP_SEL2NX8(dvecLUT1HL, dvecLUT0HL, dvecDiff);
      IVP_SEL2NX8T(dvecHL, dvecLUT3HL, dvecLUT2HL, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));

      dvecHH = IVP_SEL2NX8(dvecLUT1HH, dvecLUT0HH, dvecDiff);
      IVP_SEL2NX8T(dvecHH, dvecLUT3HH, dvecLUT2HH, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U) 128));


      IVP_DSEL2NX8I(dvecLH, dvecLL, dvecLH, dvecLL, IVP_DSELI_8B_INTERLEAVE_1);
      IVP_DSEL2NX8I(dvecHH, dvecHL, dvecHH, dvecHL, IVP_DSELI_8B_INTERLEAVE_1);

      IVP_DSEL2NX8I(dvecHL, dvecLL, dvecHL, dvecLL, IVP_DSELI_INTERLEAVE_1);
      IVP_DSEL2NX8I(dvecHH, dvecLH, dvecHH, dvecLH, IVP_DSELI_INTERLEAVE_1);

      hvecSumExp0 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecLL));
      hvecSumExp1 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecHL));
      hvecSumExp2 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecLH));
      hvecSumExp3 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecHH));

      hvecSumExp0 = IVP_MOVN_2X32T(hvecSumExp0, IVP_ZERON_2X32(), hvbCheckLL);
      hvecSumExp1 = IVP_MOVN_2X32T(hvecSumExp1, IVP_ZERON_2X32(), hvbCheckLH);

      hvecSumExp2 = IVP_MOVN_2X32T(hvecSumExp2, IVP_ZERON_2X32(), hvbCheckHL);

      hvecSumExp3 = IVP_MOVN_2X32T(hvecSumExp3, IVP_ZERON_2X32(), hvbCheckHH);
      //end of lookup table values

      IVP_SVN_2X32_IP(hvecSumExp0, phvecTemp, (XCHAL_IVPN_SIMD_WIDTH * 2));
      IVP_SVN_2X32_IP(hvecSumExp1, phvecTemp, (XCHAL_IVPN_SIMD_WIDTH * 2));
      IVP_SVN_2X32_IP(hvecSumExp2, phvecTemp, (XCHAL_IVPN_SIMD_WIDTH * 2));
      IVP_SVN_2X32_IP(hvecSumExp3, phvecTemp, (XCHAL_IVPN_SIMD_WIDTH * 2));

      /* sum of exponentials is right shifted by 12 */
      hvecSumExp0 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16) 1, hvecSumExp0), 12);
      hvecSumExp1 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16) 1, hvecSumExp1), 12);
      hvecSumExp2 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16) 1, hvecSumExp2), 12);
      hvecSumExp3 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16) 1, hvecSumExp3), 12);
      vecSumExp  += hvecSumExp0 + hvecSumExp1 + hvecSumExp2 + hvecSumExp3;

      sum_of_exps = IVP_RADDN_2X32(vecSumExp);

      int32_t fixed_sum_of_exps = sum_of_exps;                  //sum_of_exps.raw();

      int headroom_plus_one = XT_NSAU((uint32_t) fixed_sum_of_exps);
      // This is the number of bits to the left of the binary point above 1.0.
      // Consider fixed_sum_of_exps=1.25.  In that case shifted_scale=0.8 and
      // no later adjustment will be needed.
      int num_bits_over_unit        = kAccumulationIntegerBits - headroom_plus_one;
      int32_t shifted_sum_minus_one = (int32_t) (((uint32_t) (fixed_sum_of_exps) << headroom_plus_one) - ((uint32_t) (1) << 31));
      // FixedPoint0 shifted_scale = gemmlowp::one_over_one_plus_x_for_x_in_0_1( FixedPoint0::FromRaw(shifted_sum_minus_one));
      // Strat of one_over_one_plus_x_for_x_in_0_1
      const int32_t constant_48_over_17     = 1515870810;                //Q2.29 format
      const int32_t constant_neg_32_over_17 = -1010580540;               //Q2.29 format
      vecProduct = IVP_MULN_2X16X32_0((xb_vecNx16) 1, (xb_vecN_2x32v) (ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct, (xb_vecNx16) 1, (xb_vecN_2x32v) (shifted_sum_minus_one + 1));
      xb_vecN_2x32v hvecHalfDenominator = IVP_PACKVRNRN_2X64W(vecProduct, 1);

      vecProduct = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v) constant_neg_32_over_17), hvecHalfDenominator);
      IVP_MULAHN_2X16X32_1(vecProduct, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v) constant_neg_32_over_17), hvecHalfDenominator);

      int x1 = (int ) (IVP_MOV32_FROMN_2X32(IVP_PACKVRN_2X64W(vecProduct, 31))) + constant_48_over_17;


      for (int i = 0; i < 3; i++)
      {
        vecProduct = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v) x1), hvecHalfDenominator);
        IVP_MULAHN_2X16X32_1(vecProduct, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v) x1), hvecHalfDenominator);
        xb_vecN_2x32v vecHalf_Denominator_times_x           = IVP_PACKVRN_2X64W(vecProduct, 31);
        xb_vecN_2x32v vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v) (1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v) x1), (xb_vecN_2x32v) (vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v) x1), (xb_vecN_2x32v) (vecOne_minus_half_denominator_times_x));
        int32_t delta_x = IVP_MOV32_FROMN_2X32(IVP_PACKVRN_2X64W(vecProduct, 31));
        x1 = x1 + (delta_x << 2);
      }
      int result     = x1 << 1;
      XT_MOVLTZ(result, 0x7FFFFFFF, ((1 << 30) - 1)-x1);
      XT_MOVLTZ(result, 0x7FFFFFFF, x1+((1 << 30) - 1));
       int shifted_scale = result;


      xb_vecN_2x32v hvecShiftedScale = (xb_vecN_2x32v) (shifted_scale);
      phvecTemp = (xb_vecN_2x32v *) ptrScratch;
      pvecInp   = (MORPH_IDT_2Nx8 *) (pInData);
      va        = MORPH_OP_PRIME_LOAD_2NX8_PP(pvecInp);
      for (x = 0; x < dim1Size; x += (XCHAL_IVPN_SIMD_WIDTH * 2))
      {
        IVP_LVN_2X32_IP(hvecSumExp0, phvecTemp, (XCHAL_IVPN_SIMD_WIDTH * 2));
        IVP_LVN_2X32_IP(hvecSumExp1, phvecTemp, (XCHAL_IVPN_SIMD_WIDTH * 2));
        IVP_LVN_2X32_IP(hvecSumExp2, phvecTemp, (XCHAL_IVPN_SIMD_WIDTH * 2));
        IVP_LVN_2X32_IP(hvecSumExp3, phvecTemp, (XCHAL_IVPN_SIMD_WIDTH * 2));

        // Start by multiplying with the reciprocal of SumOfExps
        wvec0 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecShiftedScale), hvecSumExp0);
        IVP_MULAHN_2X16X32_1(wvec0, IVP_MOVNX16_FROMN_2X32(hvecShiftedScale), hvecSumExp0);
        hvecSumExp0 = IVP_PACKVRN_2X64W(wvec0, 31);
        wvec0       = IVP_MULUSN_2X16X32_0((xb_vecNx16) 1, hvecSumExp0);
        hvecSumExp0 = IVP_PACKVRN_2X64W(wvec0, (31 - OUTPUT_BITS + num_bits_over_unit));

        wvec0 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecShiftedScale), hvecSumExp1);
        IVP_MULAHN_2X16X32_1(wvec0, IVP_MOVNX16_FROMN_2X32(hvecShiftedScale), hvecSumExp1);
        hvecSumExp1 = IVP_PACKVRN_2X64W(wvec0, 31);
        wvec0       = IVP_MULUSN_2X16X32_0((xb_vecNx16) 1, hvecSumExp1);
        hvecSumExp1 = IVP_PACKVRN_2X64W(wvec0, (31 - OUTPUT_BITS + num_bits_over_unit));

        //for 2 and 3
        wvec1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecShiftedScale), hvecSumExp2);
        IVP_MULAHN_2X16X32_1(wvec1, IVP_MOVNX16_FROMN_2X32(hvecShiftedScale), hvecSumExp2);
        hvecSumExp2 = IVP_PACKVRN_2X64W(wvec1, 31);
        wvec1       = IVP_MULUSN_2X16X32_0((xb_vecNx16) 1, hvecSumExp2);
        hvecSumExp2 = IVP_PACKVRN_2X64W(wvec1, (31 - OUTPUT_BITS + num_bits_over_unit));

        wvec1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecShiftedScale), hvecSumExp3);
        IVP_MULAHN_2X16X32_1(wvec1, IVP_MOVNX16_FROMN_2X32(hvecShiftedScale), hvecSumExp3);
        hvecSumExp3 = IVP_PACKVRNRN_2X64W(wvec1, 31);
        wvec1       = IVP_MULUSN_2X16X32_0((xb_vecNx16) 1, hvecSumExp3);
        hvecSumExp3 = IVP_PACKVRN_2X64W(wvec1, (31 - OUTPUT_BITS + num_bits_over_unit));

        vecRes0 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecSumExp1, hvecSumExp0, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));
        vecRes1 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecSumExp3, hvecSumExp2, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));
        MORPH_OP_CLAMP(vecRes0, vecRes1, dvecRes);

        MORPH_OP_ALIGN_STORE_2NX8_XP(dvecRes, vaStore, pvecOutput, dim1Size - x);
      }
      MORPH_OP_STORE_FLUSH(vaStore, pvecOutput);
    }
  }
  return XI_ERROR_STATUS();
}
#define SOFTMAX_APPLY_LUT(dvecDiff, hvecSumExp0, hvecSumExp1, hvecSumExp2,hvecSumExp3) \
{\
  xb_vec2Nx8 dvecLL0, dvecLL, dvecLH0, dvecLH, dvecHL0, dvecHL, dvecHH0, dvecHH;\
  /*SEL based lookup table*/   \
  dvecLL = IVP_SEL2NX8(dvecLUT1LL, dvecLUT0LL, dvecDiff);\
  IVP_SEL2NX8T(dvecLL, dvecLUT3LL, dvecLUT2LL, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U)128));\
\
  dvecLH = IVP_SEL2NX8(dvecLUT1LH, dvecLUT0LH, dvecDiff);\
  IVP_SEL2NX8T(dvecLH, dvecLUT3LH, dvecLUT2LH, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U)128));\
\
  dvecHL = IVP_SEL2NX8(dvecLUT1HL, dvecLUT0HL, dvecDiff);\
  IVP_SEL2NX8T(dvecHL, dvecLUT3HL, dvecLUT2HL, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U)128));\
\
  dvecHH = IVP_SEL2NX8(dvecLUT1HH, dvecLUT0HH, dvecDiff);\
  IVP_SEL2NX8T(dvecHH, dvecLUT3HH, dvecLUT2HH, dvecDiff, IVP_GEU2NX8U(dvecDiff, (xb_vec2Nx8U)128));\
\
  dvecLL0 = IVP_SEL2NX8I(dvecLH, dvecLL, IVP_SELI_8B_INTERLEAVE_1_LO);\
  dvecLH0 = IVP_SEL2NX8I(dvecLH, dvecLL, IVP_SELI_8B_INTERLEAVE_1_HI);\
  dvecHL0 = IVP_SEL2NX8I(dvecHH, dvecHL, IVP_SELI_8B_INTERLEAVE_1_LO);\
  dvecHH0 = IVP_SEL2NX8I(dvecHH, dvecHL, IVP_SELI_8B_INTERLEAVE_1_HI);\
\
  dvecLL = IVP_SEL2NX8I(dvecHL0, dvecLL0, IVP_SELI_INTERLEAVE_1_LO);\
  dvecHL = IVP_SEL2NX8I(dvecHL0, dvecLL0, IVP_SELI_INTERLEAVE_1_HI);\
  dvecLH = IVP_SEL2NX8I(dvecHH0, dvecLH0, IVP_SELI_INTERLEAVE_1_LO);\
  dvecHH = IVP_SEL2NX8I(dvecHH0, dvecLH0, IVP_SELI_INTERLEAVE_1_HI);\
  hvecSumExp0 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecLL));\
  hvecSumExp1 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecHL));\
  hvecSumExp2 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecLH));\
  hvecSumExp3 = IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecHH));\
}

XI_ERR_TYPE MAKE_NAME2(softmaxA3D, DIM1_LUT_DEPTH2)(const xi_pTile3D inTile,
                                                           const xi_pArray softmaxArray,
                                                           xi_pTile3D outTile,
                                                           const xi_cnn_softmaxA_params  * pparams)
{
  /* Error Checks */
  XI_ERROR_CHECKS()
  {
    MORPH_IDT_CHECK(inTile);
    MORPH_IDT_CHECK(outTile);
    XI_CHECK_ARRAY_S32(softmaxArray);
    XI_CHECK_TILE3D_SIZE_EQ(inTile, outTile);
    XI_CHECK_POINTER(pparams);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(inTile) == 2, XI_ERR_BADARG, \
      "\ninTile dim1 size = %d, inTile dim1 Size should be equal to 2", XI_TILE3D_GET_DIM1(inTile));
    XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(softmaxArray) == 256, XI_ERR_BADARG, \
      "\nsoftmaxArray LUT Size = %d, softmaxArray LUT Size should be equal to 256", XI_ARRAY_GET_WIDTH(softmaxArray));
    XI_CHECK_ERROR(XI_TILE3D_GET_DATA_ORDER(inTile) == XI_TILE3D_GET_DATA_ORDER(outTile), XI_ERR_BADARG, \
      "\nData Order of InputTile = %d, OutputTile= %d, Data order of input and output tile should be same", \
      XI_TILE3D_GET_DATA_ORDER(inTile), XI_TILE3D_GET_DATA_ORDER(outTile));
  }

  /* Getting parameters from the tile structures */
  const int32_t dim2Size = XI_TILE3D_GET_DIM2(inTile);
  const int32_t inDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(inTile);
  const int32_t dim3Size = XI_TILE3D_GET_DIM3(inTile);
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
  MORPH_IDT_SCALAR *pInput = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile);
  MORPH_IDT_SCALAR *pOutput = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(outTile);
  int32_t  y, z;

  static const int kAccumulationIntegerBits = 12;

  int32_t *LUTOut = (int32_t *)XI_ARRAY_GET_BUFF_PTR(softmaxArray);

  MORPH_IDT_SCALAR *pInData, *pOut;
  MORPH_IDT_2Nx8 *restrict pvecInp;
  valign va, vaStore;

  xb_vecN_2x32v hvecSumExp0, hvecSumExp1, hvecSumExp2, hvecSumExp3, vecSumExp;
  //
  MORPH_IDT_2Nx8 *restrict pvecOutput;;

  vaStore = IVP_ZALIGN();
  xb_vec2Nx8 *restrict dvecLUTBase = (xb_vec2Nx8 *)LUTOut;
  valign va1 = IVP_LA2NX8_PP(dvecLUTBase);
  xb_vec2Nx8 dvecLUT0LL, dvecLUT1LL, dvecLUT2LL, dvecLUT3LL, dvecLUT0LH, dvecLUT1LH, dvecLUT2LH, dvecLUT3LH;
  xb_vec2Nx8 dvecLUT0HL, dvecLUT1HL, dvecLUT2HL, dvecLUT3HL, dvecLUT0HH, dvecLUT1HH, dvecLUT2HH, dvecLUT3HH;
  //  xb_vec2Nx8 dvecLL0, dvecLL, dvecLH0, dvecLH, dvecHL0, dvecHL, dvecHH0, dvecHH;


  IVP_LA2NX8_IP(dvecLUT0LL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT1LL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT2LL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT3LL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT0LH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT1LH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT2LH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT3LH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT0HL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT1HL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT2HL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT3HL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT0HH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT1HH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT2HH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT3HH, va1, dvecLUTBase);


  MORPH_IDT_2Nx8 xvec1, xvec2;
  MORPH_IDT_2Nx8 xvec3, xvec4;
  MORPH_IDT_2Nx8 xvec_max;
  MORPH_IDT_2Nx8 dvec_diff1, dvec_diff2;

  xb_vecN_2x32v hvecSumExpA0, hvecSumExpA1, hvecSumExpA2, hvecSumExpA3;
  xb_vecN_2x32v hvecSumExpB0, hvecSumExpB1, hvecSumExpB2, hvecSumExpB3;

  xb_vecN_2x64w vecProduct1, vecProduct2, vecProduct3, vecProduct4;
  xb_vecN_2x64w wvecA0, wvecA1, wvecA2, wvecA3;
  xb_vecN_2x64w wvecB0, wvecB1, wvecB2, wvecB3;
  xb_vecN_2x32v hvecShift0, hvecShift1, hvecShift2, hvecShift3;

  xb_vecNx16 vecRes0, vecRes1;
  xb_vec2Nx8U dvecResA, dvecResB;

  for (z = 0; z < dim3Size; z++)     /* along the image depth(WHD)/height(DWH) */
  {
    for (y = 0; y <= dim2Size - (2 * XCHAL_IVPN_SIMD_WIDTH); y += (2 * XCHAL_IVPN_SIMD_WIDTH))   /* along the image height(WHD)/width(DWH) */
    {
      pInData = (MORPH_IDT_SCALAR *)(pInput + ((z * inDataPitch2) + (y * inDataPitch1)));
      pOut = pOutput + ((z * outDataPitch2) + (y * outDataPitch1));
      pvecInp = (MORPH_IDT_2Nx8 *)(pInData);
      pvecOutput = (MORPH_IDT_2Nx8 *)(pOut);
      va = MORPH_OP_PRIME_LOAD_2NX8_PP(pvecInp);

      MORPH_OP_ALIGN_LOAD_2NX8_IP(xvec1, va, pvecInp);
      MORPH_OP_ALIGN_LOAD_2NX8_IP(xvec2, va, pvecInp);

      IVP_DSEL2NX8I(xvec4, xvec3, xvec2, xvec1, IVP_DSELI_8B_DEINTERLEAVE_1);

      xvec_max = MORPH_OP_MAX2NX8(xvec4, xvec3);
      vecSumExp = 0;

      dvec_diff1 = MORPH_OP_SUB2NX8(xvec_max, xvec3);        // equivalent of this int32 input_diff = (int32)(input_data[Offset(input_dims, c, x, y, b)]) -  max_in_row;
      dvec_diff2 = MORPH_OP_SUB2NX8(xvec_max, xvec4);
      // implements (input_diff >= diff_min) along with out of boundary condition
      SOFTMAX_APPLY_LUT(dvec_diff1, hvecSumExpA0, hvecSumExpA1, hvecSumExpA2, hvecSumExpA3);
      SOFTMAX_APPLY_LUT(dvec_diff2, hvecSumExpB0, hvecSumExpB1, hvecSumExpB2, hvecSumExpB3);

      hvecSumExp0 = IVP_ADDN_2X32(IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpA0), 12),
        IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpB0), 12));
      hvecSumExp1 = IVP_ADDN_2X32(IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpA1), 12)
        , IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpB1), 12));
      hvecSumExp2 = IVP_ADDN_2X32(IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpA2), 12)
        , IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpB2), 12));
      hvecSumExp3 = IVP_ADDN_2X32(IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpA3), 12),
        IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpB3), 12));

      xb_vecN_2x32v hvecShiftedSumMinusOne0, hvecShiftedSumMinusOne1, hvecShiftedSumMinusOne2, hvecShiftedSumMinusOne3;
      xb_vecN_2x32v hvecHeadRoomPlusOne0, hvecHeadRoomPlusOne1, hvecHeadRoomPlusOne2, hvecHeadRoomPlusOne3;

      //int headroom_plus_one = XT_NSAU((uint32_t)fixed_sum_of_exps);
      hvecHeadRoomPlusOne0 = IVP_NSAUN_2X32(hvecSumExp0);
      hvecHeadRoomPlusOne1 = IVP_NSAUN_2X32(hvecSumExp1);
      hvecHeadRoomPlusOne2 = IVP_NSAUN_2X32(hvecSumExp2);
      hvecHeadRoomPlusOne3 = IVP_NSAUN_2X32(hvecSumExp3);

      //int num_bits_over_unit = kAccumulationIntegerBits - headroom_plus_one;
      hvecShiftedSumMinusOne0 = IVP_SUBN_2X32U(IVP_SLAN_2X32(hvecSumExp0, hvecHeadRoomPlusOne0), (xb_vecN_2x32Uv)(1 << 31));
      hvecShiftedSumMinusOne1 = IVP_SUBN_2X32U(IVP_SLAN_2X32(hvecSumExp1, hvecHeadRoomPlusOne1), (xb_vecN_2x32Uv)(1 << 31));
      hvecShiftedSumMinusOne2 = IVP_SUBN_2X32U(IVP_SLAN_2X32(hvecSumExp2, hvecHeadRoomPlusOne2), (xb_vecN_2x32Uv)(1 << 31));
      hvecShiftedSumMinusOne3 = IVP_SUBN_2X32U(IVP_SLAN_2X32(hvecSumExp3, hvecHeadRoomPlusOne3), (xb_vecN_2x32Uv)(1 << 31));

      // int32_t shifted_sum_minus_one = (int32_t)(((uint32_t)(fixed_sum_of_exps) << headroom_plus_one) - ((uint32_t)(1) << 31));
      hvecHeadRoomPlusOne0 = IVP_SUBN_2X32((xb_vecN_2x32Uv)kAccumulationIntegerBits, hvecHeadRoomPlusOne0);
      hvecHeadRoomPlusOne1 = IVP_SUBN_2X32((xb_vecN_2x32Uv)kAccumulationIntegerBits, hvecHeadRoomPlusOne1);
      hvecHeadRoomPlusOne2 = IVP_SUBN_2X32((xb_vecN_2x32Uv)kAccumulationIntegerBits, hvecHeadRoomPlusOne2);
      hvecHeadRoomPlusOne3 = IVP_SUBN_2X32((xb_vecN_2x32Uv)kAccumulationIntegerBits, hvecHeadRoomPlusOne3);


      // This is the number of bits to the left of the binary point above 1.0.
      // Consider fixed_sum_of_exps=1.25.  In that case shifted_scale=0.8 and
      // no later adjustment will be needed.


      // FixedPoint0 shifted_scale = gemmlowp::one_over_one_plus_x_for_x_in_0_1( FixedPoint0::FromRaw(shifted_sum_minus_one));
      // Strat of one_over_one_plus_x_for_x_in_0_1
      const int32_t constant_48_over_17 = 1515870810;                //Q2.29 format
      const int32_t constant_neg_32_over_17 = -1010580540;               //Q2.29 format

      vecProduct1 = IVP_MULN_2X16X32_0((xb_vecNx16)1, (xb_vecN_2x32v)(ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct1, (xb_vecNx16)1, IVP_ADDN_2X32(hvecShiftedSumMinusOne0, (xb_vecN_2x32v)1));
      xb_vecN_2x32v hvecHalfDenominator1 = IVP_PACKVRNRN_2X64W(vecProduct1, 1);

      vecProduct1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator1);
      IVP_MULAHN_2X16X32_1(vecProduct1, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator1);


      vecProduct2 = IVP_MULN_2X16X32_0((xb_vecNx16)1, (xb_vecN_2x32v)(ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct2, (xb_vecNx16)1, IVP_ADDN_2X32(hvecShiftedSumMinusOne1, (xb_vecN_2x32v)1));
      xb_vecN_2x32v hvecHalfDenominator2 = IVP_PACKVRNRN_2X64W(vecProduct2, 1);

      vecProduct2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator2);
      IVP_MULAHN_2X16X32_1(vecProduct2, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator2);

      vecProduct3 = IVP_MULN_2X16X32_0((xb_vecNx16)1, (xb_vecN_2x32v)(ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct3, (xb_vecNx16)1, IVP_ADDN_2X32(hvecShiftedSumMinusOne2, (xb_vecN_2x32v)1));
      xb_vecN_2x32v hvecHalfDenominator3 = IVP_PACKVRNRN_2X64W(vecProduct3, 1);

      vecProduct3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator3);
      IVP_MULAHN_2X16X32_1(vecProduct3, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator3);

      vecProduct4 = IVP_MULN_2X16X32_0((xb_vecNx16)1, (xb_vecN_2x32v)(ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct4, (xb_vecNx16)1, IVP_ADDN_2X32(hvecShiftedSumMinusOne3, (xb_vecN_2x32v)1));
      xb_vecN_2x32v hvecHalfDenominator4 = IVP_PACKVRNRN_2X64W(vecProduct4, 1);

      vecProduct4 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator4);
      IVP_MULAHN_2X16X32_1(vecProduct4, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator4);

      xb_vecN_2x32v x1 = IVP_ADDN_2X32((IVP_PACKVRN_2X64W(vecProduct1, 31)), (xb_vecN_2x32v)constant_48_over_17);
      xb_vecN_2x32v x2 = IVP_ADDN_2X32((IVP_PACKVRN_2X64W(vecProduct2, 31)), (xb_vecN_2x32v)constant_48_over_17);
      xb_vecN_2x32v x3 = IVP_ADDN_2X32((IVP_PACKVRN_2X64W(vecProduct3, 31)), (xb_vecN_2x32v)constant_48_over_17);
      xb_vecN_2x32v x4 = IVP_ADDN_2X32((IVP_PACKVRN_2X64W(vecProduct4, 31)), (xb_vecN_2x32v)constant_48_over_17);

      for (int i = 0; i < 3; i++)
      {
        vecProduct1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x1), hvecHalfDenominator1);
        IVP_MULAHN_2X16X32_1(vecProduct1, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x1), hvecHalfDenominator1);
        xb_vecN_2x32v vecHalf_Denominator_times_x = IVP_PACKVRN_2X64W(vecProduct1, 31);
        xb_vecN_2x32v vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v)(1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x1), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct1, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x1), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        xb_vecN_2x32v delta_x = IVP_PACKVRN_2X64W(vecProduct1, 31);
        x1 = IVP_ADDN_2X32(x1, IVP_SLAN_2X32(delta_x, 2));

        vecProduct2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x2), hvecHalfDenominator2);
        IVP_MULAHN_2X16X32_1(vecProduct2, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x2), hvecHalfDenominator2);
        vecHalf_Denominator_times_x = IVP_PACKVRN_2X64W(vecProduct2, 31);
        vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v)(1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x2), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct2, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x2), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        delta_x = IVP_PACKVRN_2X64W(vecProduct2, 31);
        x2 = IVP_ADDN_2X32(x2, IVP_SLAN_2X32(delta_x, 2));

        vecProduct3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x3), hvecHalfDenominator3);
        IVP_MULAHN_2X16X32_1(vecProduct3, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x3), hvecHalfDenominator3);
        vecHalf_Denominator_times_x = IVP_PACKVRN_2X64W(vecProduct3, 31);
        vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v)(1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x3), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct3, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x3), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        delta_x = IVP_PACKVRN_2X64W(vecProduct3, 31);
        x3 = IVP_ADDN_2X32(x3, IVP_SLAN_2X32(delta_x, 2));

        vecProduct4 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x4), hvecHalfDenominator4);
        IVP_MULAHN_2X16X32_1(vecProduct4, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x4), hvecHalfDenominator4);
        vecHalf_Denominator_times_x = IVP_PACKVRN_2X64W(vecProduct4, 31);
        vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v)(1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct4 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x4), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct4, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x4), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        delta_x = IVP_PACKVRN_2X64W(vecProduct4, 31);
        x4 = IVP_ADDN_2X32(x4, IVP_SLAN_2X32(delta_x, 2));

      }
      x1 = IVP_SLAN_2X32(x1, 1);
      x2 = IVP_SLAN_2X32(x2, 1);
      x3 = IVP_SLAN_2X32(x3, 1);
      x4 = IVP_SLAN_2X32(x4, 1);

      vboolN_2 vbMaxRestrict = IVP_GTN_2X32(x1, (xb_vecN_2x32v)((1 << 30) - 1));
      x1 = IVP_MOVN_2X32T(x1, IVP_MAXN_2X32(x1, 0x7FFFFFFF), vbMaxRestrict);

      vboolN_2 vbMinRestrict = IVP_GTN_2X32(x1, (xb_vecN_2x32v)(-((1 << 30) - 1)));
      x1 = IVP_MOVN_2X32T(x1, IVP_MINN_2X32(x1, 0x80000000), vbMinRestrict);

      vbMaxRestrict = IVP_GTN_2X32(x2, (xb_vecN_2x32v)((1 << 30) - 1));
      x2 = IVP_MOVN_2X32T(x2, IVP_MAXN_2X32(x2, 0x7FFFFFFF), vbMaxRestrict);

      vbMinRestrict = IVP_GTN_2X32(x2, (xb_vecN_2x32v)(-((1 << 30) - 1)));
      x2 = IVP_MOVN_2X32T(x2, IVP_MINN_2X32(x2, 0x80000000), vbMinRestrict);

      vbMaxRestrict = IVP_GTN_2X32(x3, (xb_vecN_2x32v)((1 << 30) - 1));
      x3 = IVP_MOVN_2X32T(x3, IVP_MAXN_2X32(x3, 0x7FFFFFFF), vbMaxRestrict);

      vbMinRestrict = IVP_GTN_2X32(x3, (xb_vecN_2x32v)(-((1 << 30) - 1)));
      x3 = IVP_MOVN_2X32T(x3, IVP_MINN_2X32(x3, 0x80000000), vbMinRestrict);

      vbMaxRestrict = IVP_GTN_2X32(x4, (xb_vecN_2x32v)((1 << 30) - 1));
      x4 = IVP_MOVN_2X32T(x4, IVP_MAXN_2X32(x4, 0x7FFFFFFF), vbMaxRestrict);

      vbMinRestrict = IVP_GTN_2X32(x4, (xb_vecN_2x32v)(-((1 << 30) - 1)));
      x4 = IVP_MOVN_2X32T(x4, IVP_MINN_2X32(x4, 0x80000000), vbMinRestrict);

      xb_vecN_2x32v hvecSum0, hvecSum1, hvecSum2, hvecSum3;
      xb_vecN_2x32v hvecOne = 1;
      // Part A
      // Start by multiplying with the reciprocal of SumOfExps

      wvecA0 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x1), hvecSumExpA0);
      IVP_MULAHN_2X16X32_1(wvecA0, IVP_MOVNX16_FROMN_2X32(x1), hvecSumExpA0);
      hvecSumExpA0 = IVP_PACKVRN_2X64W(wvecA0, 31);
      hvecShift0 = IVP_ADDN_2X32(hvecHeadRoomPlusOne0, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
      hvecSum0 = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift0, hvecOne));
      hvecSumExpA0 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExpA0, hvecSum0), hvecShift0);

      wvecA1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x2), hvecSumExpA1);
      IVP_MULAHN_2X16X32_1(wvecA1, IVP_MOVNX16_FROMN_2X32(x2), hvecSumExpA1);
      hvecSumExpA1 = IVP_PACKVRN_2X64W(wvecA1, 31);
      hvecShift1 = IVP_ADDN_2X32(hvecHeadRoomPlusOne1, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
      hvecSum1 = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift1, hvecOne));
      hvecSumExpA1 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExpA1, hvecSum1), hvecShift1);

      //for 2 and 3
      wvecA2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x3), hvecSumExpA2);
      IVP_MULAHN_2X16X32_1(wvecA2, IVP_MOVNX16_FROMN_2X32(x3), hvecSumExpA2);
      hvecSumExpA2 = IVP_PACKVRN_2X64W(wvecA2, 31);
      hvecShift2 = IVP_ADDN_2X32(hvecHeadRoomPlusOne2, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
      hvecSum2 = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift2, hvecOne));
      hvecSumExpA2 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExpA2, hvecSum2), hvecShift2);

      wvecA3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x4), hvecSumExpA3);
      IVP_MULAHN_2X16X32_1(wvecA3, IVP_MOVNX16_FROMN_2X32(x4), hvecSumExpA3);
      hvecSumExpA3 = IVP_PACKVRNRN_2X64W(wvecA3, 31);
      hvecShift3 = IVP_ADDN_2X32(hvecHeadRoomPlusOne3, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
      hvecSum3 = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift3, hvecOne));
      hvecSumExpA3 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExpA3, hvecSum3), hvecShift3);


      // Part B
      // Start by multiplying with the reciprocal of SumOfExps
      wvecB0 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x1), hvecSumExpB0);
      IVP_MULAHN_2X16X32_1(wvecB0, IVP_MOVNX16_FROMN_2X32(x1), hvecSumExpB0);
      hvecSumExpB0 = IVP_PACKVRN_2X64W(wvecB0, 31);
      hvecShift0 = IVP_ADDN_2X32(hvecHeadRoomPlusOne0, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
      hvecSum0 = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift0, hvecOne));
      hvecSumExpB0 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExpB0, hvecSum0), hvecShift0);

      wvecB1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x2), hvecSumExpB1);
      IVP_MULAHN_2X16X32_1(wvecB1, IVP_MOVNX16_FROMN_2X32(x2), hvecSumExpB1);
      hvecSumExpB1 = IVP_PACKVRN_2X64W(wvecB1, 31);
      hvecShift1 = IVP_ADDN_2X32(hvecHeadRoomPlusOne1, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
      hvecSum1 = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift1, hvecOne));
      hvecSumExpB1 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExpB1, hvecSum1), hvecShift1);


      //for 2 and 3
      wvecB2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x3), hvecSumExpB2);
      IVP_MULAHN_2X16X32_1(wvecB2, IVP_MOVNX16_FROMN_2X32(x3), hvecSumExpB2);
      hvecSumExpB2 = IVP_PACKVRN_2X64W(wvecB2, 31);
      hvecShift2 = IVP_ADDN_2X32(hvecHeadRoomPlusOne2, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
      hvecSum2 = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift2, hvecOne));
      hvecSumExpB2 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExpB2, hvecSum2), hvecShift2);

      wvecB3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x4), hvecSumExpB3);
      IVP_MULAHN_2X16X32_1(wvecB3, IVP_MOVNX16_FROMN_2X32(x4), hvecSumExpB3);
      hvecSumExpB3 = IVP_PACKVRNRN_2X64W(wvecB3, 31);
      hvecShift3 = IVP_ADDN_2X32(hvecHeadRoomPlusOne3, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
      hvecSum3 = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift3, hvecOne));
      hvecSumExpB3 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExpB3, hvecSum3), hvecShift3);

      vecRes0 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecSumExpA1, hvecSumExpA0, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));
      vecRes1 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecSumExpA3, hvecSumExpA2, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));
      MORPH_OP_CLAMP(vecRes0, vecRes1, dvecResA);

      vecRes0 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecSumExpB1, hvecSumExpB0, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));
      vecRes1 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecSumExpB3, hvecSumExpB2, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));

      MORPH_OP_CLAMP(vecRes0, vecRes1, dvecResB);

      IVP_DSEL2NX8I(dvecResB, dvecResA, dvecResB, dvecResA, IVP_DSELI_8B_INTERLEAVE_1);

      MORPH_OP_ALIGN_STORE_2NX8_IP(dvecResA, vaStore, pvecOutput);
      MORPH_OP_ALIGN_STORE_2NX8_IP(dvecResB, vaStore, pvecOutput);

      MORPH_OP_STORE_FLUSH(vaStore, pvecOutput);
    }
    if (y < dim2Size)   /* along the image height(WHD)/width(DWH) */
    {
      pInData = (MORPH_IDT_SCALAR *)(pInput + ((z * inDataPitch2) + (y * inDataPitch1)));
      pOut = pOutput + ((z * outDataPitch2) + (y * outDataPitch1));
      pvecInp = (MORPH_IDT_2Nx8 *)(pInData);
      pvecOutput = (MORPH_IDT_2Nx8 *)(pOut);
      va = MORPH_OP_PRIME_LOAD_2NX8_PP(pvecInp);

      MORPH_OP_ALIGN_LOAD_2Nx8_VARIABLE(xvec1, va, pvecInp, 2 * (dim2Size - y));
      MORPH_OP_ALIGN_LOAD_2Nx8_VARIABLE(xvec2, va, pvecInp, 2 * (dim2Size - y - XCHAL_IVPN_SIMD_WIDTH));

      IVP_DSEL2NX8I(xvec4, xvec3, xvec2, xvec1, IVP_DSELI_8B_DEINTERLEAVE_1);

      xvec_max = MORPH_OP_MAX2NX8(xvec4, xvec3);
      vecSumExp = 0;

      dvec_diff1 = MORPH_OP_SUB2NX8(xvec_max, xvec3);        // equivalent of this int32 input_diff = (int32)(input_data[Offset(input_dims, c, x, y, b)]) -  max_in_row;
      dvec_diff2 = MORPH_OP_SUB2NX8(xvec_max, xvec4);
      // implements (input_diff >= diff_min) along with out of boundary condition
      SOFTMAX_APPLY_LUT(dvec_diff1, hvecSumExpA0, hvecSumExpA1, hvecSumExpA2, hvecSumExpA3);
      SOFTMAX_APPLY_LUT(dvec_diff2, hvecSumExpB0, hvecSumExpB1, hvecSumExpB2, hvecSumExpB3);

      hvecSumExp0 = IVP_ADDN_2X32(IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpA0), 12),
        IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpB0), 12));
      hvecSumExp1 = IVP_ADDN_2X32(IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpA1), 12)
        , IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpB1), 12));
      hvecSumExp2 = IVP_ADDN_2X32(IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpA2), 12)
        , IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpB2), 12));
      hvecSumExp3 = IVP_ADDN_2X32(IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpA3), 12),
        IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpB3), 12));

      xb_vecN_2x32v hvecShiftedSumMinusOne0, hvecShiftedSumMinusOne1, hvecShiftedSumMinusOne2, hvecShiftedSumMinusOne3;
      xb_vecN_2x32v hvecHeadRoomPlusOne0, hvecHeadRoomPlusOne1, hvecHeadRoomPlusOne2, hvecHeadRoomPlusOne3;

      //int headroom_plus_one = XT_NSAU((uint32_t)fixed_sum_of_exps);
      hvecHeadRoomPlusOne0 = IVP_NSAUN_2X32(hvecSumExp0);
      hvecHeadRoomPlusOne1 = IVP_NSAUN_2X32(hvecSumExp1);
      hvecHeadRoomPlusOne2 = IVP_NSAUN_2X32(hvecSumExp2);
      hvecHeadRoomPlusOne3 = IVP_NSAUN_2X32(hvecSumExp3);

      //int num_bits_over_unit = kAccumulationIntegerBits - headroom_plus_one;
      hvecShiftedSumMinusOne0 = IVP_SUBN_2X32U(IVP_SLAN_2X32(hvecSumExp0, hvecHeadRoomPlusOne0), (xb_vecN_2x32Uv)(1 << 31));
      hvecShiftedSumMinusOne1 = IVP_SUBN_2X32U(IVP_SLAN_2X32(hvecSumExp1, hvecHeadRoomPlusOne1), (xb_vecN_2x32Uv)(1 << 31));
      hvecShiftedSumMinusOne2 = IVP_SUBN_2X32U(IVP_SLAN_2X32(hvecSumExp2, hvecHeadRoomPlusOne2), (xb_vecN_2x32Uv)(1 << 31));
      hvecShiftedSumMinusOne3 = IVP_SUBN_2X32U(IVP_SLAN_2X32(hvecSumExp3, hvecHeadRoomPlusOne3), (xb_vecN_2x32Uv)(1 << 31));

      // int32_t shifted_sum_minus_one = (int32_t)(((uint32_t)(fixed_sum_of_exps) << headroom_plus_one) - ((uint32_t)(1) << 31));
      hvecHeadRoomPlusOne0 = IVP_SUBN_2X32((xb_vecN_2x32Uv)kAccumulationIntegerBits, hvecHeadRoomPlusOne0);
      hvecHeadRoomPlusOne1 = IVP_SUBN_2X32((xb_vecN_2x32Uv)kAccumulationIntegerBits, hvecHeadRoomPlusOne1);
      hvecHeadRoomPlusOne2 = IVP_SUBN_2X32((xb_vecN_2x32Uv)kAccumulationIntegerBits, hvecHeadRoomPlusOne2);
      hvecHeadRoomPlusOne3 = IVP_SUBN_2X32((xb_vecN_2x32Uv)kAccumulationIntegerBits, hvecHeadRoomPlusOne3);


      // This is the number of bits to the left of the binary point above 1.0.
      // Consider fixed_sum_of_exps=1.25.  In that case shifted_scale=0.8 and
      // no later adjustment will be needed.


      // FixedPoint0 shifted_scale = gemmlowp::one_over_one_plus_x_for_x_in_0_1( FixedPoint0::FromRaw(shifted_sum_minus_one));
      // Strat of one_over_one_plus_x_for_x_in_0_1
      const int32_t constant_48_over_17 = 1515870810;                //Q2.29 format
      const int32_t constant_neg_32_over_17 = -1010580540;               //Q2.29 format

      vecProduct1 = IVP_MULN_2X16X32_0((xb_vecNx16)1, (xb_vecN_2x32v)(ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct1, (xb_vecNx16)1, IVP_ADDN_2X32(hvecShiftedSumMinusOne0, (xb_vecN_2x32v)1));
      xb_vecN_2x32v hvecHalfDenominator1 = IVP_PACKVRNRN_2X64W(vecProduct1, 1);

      vecProduct1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator1);
      IVP_MULAHN_2X16X32_1(vecProduct1, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator1);


      vecProduct2 = IVP_MULN_2X16X32_0((xb_vecNx16)1, (xb_vecN_2x32v)(ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct2, (xb_vecNx16)1, IVP_ADDN_2X32(hvecShiftedSumMinusOne1, (xb_vecN_2x32v)1));
      xb_vecN_2x32v hvecHalfDenominator2 = IVP_PACKVRNRN_2X64W(vecProduct2, 1);

      vecProduct2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator2);
      IVP_MULAHN_2X16X32_1(vecProduct2, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator2);

      vecProduct3 = IVP_MULN_2X16X32_0((xb_vecNx16)1, (xb_vecN_2x32v)(ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct3, (xb_vecNx16)1, IVP_ADDN_2X32(hvecShiftedSumMinusOne2, (xb_vecN_2x32v)1));
      xb_vecN_2x32v hvecHalfDenominator3 = IVP_PACKVRNRN_2X64W(vecProduct3, 1);

      vecProduct3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator3);
      IVP_MULAHN_2X16X32_1(vecProduct3, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator3);

      vecProduct4 = IVP_MULN_2X16X32_0((xb_vecNx16)1, (xb_vecN_2x32v)(ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct4, (xb_vecNx16)1, IVP_ADDN_2X32(hvecShiftedSumMinusOne3, (xb_vecN_2x32v)1));
      xb_vecN_2x32v hvecHalfDenominator4 = IVP_PACKVRNRN_2X64W(vecProduct4, 1);

      vecProduct4 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator4);
      IVP_MULAHN_2X16X32_1(vecProduct4, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator4);

      xb_vecN_2x32v x1 = IVP_ADDN_2X32((IVP_PACKVRN_2X64W(vecProduct1, 31)), (xb_vecN_2x32v)constant_48_over_17);
      xb_vecN_2x32v x2 = IVP_ADDN_2X32((IVP_PACKVRN_2X64W(vecProduct2, 31)), (xb_vecN_2x32v)constant_48_over_17);
      xb_vecN_2x32v x3 = IVP_ADDN_2X32((IVP_PACKVRN_2X64W(vecProduct3, 31)), (xb_vecN_2x32v)constant_48_over_17);
      xb_vecN_2x32v x4 = IVP_ADDN_2X32((IVP_PACKVRN_2X64W(vecProduct4, 31)), (xb_vecN_2x32v)constant_48_over_17);

      for (int i = 0; i < 3; i++)
      {
        vecProduct1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x1), hvecHalfDenominator1);
        IVP_MULAHN_2X16X32_1(vecProduct1, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x1), hvecHalfDenominator1);
        xb_vecN_2x32v vecHalf_Denominator_times_x = IVP_PACKVRN_2X64W(vecProduct1, 31);
        xb_vecN_2x32v vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v)(1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x1), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct1, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x1), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        xb_vecN_2x32v delta_x = IVP_PACKVRN_2X64W(vecProduct1, 31);
        x1 = IVP_ADDN_2X32(x1, IVP_SLAN_2X32(delta_x, 2));

        vecProduct2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x2), hvecHalfDenominator2);
        IVP_MULAHN_2X16X32_1(vecProduct2, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x2), hvecHalfDenominator2);
        vecHalf_Denominator_times_x = IVP_PACKVRN_2X64W(vecProduct2, 31);
        vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v)(1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x2), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct2, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x2), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        delta_x = IVP_PACKVRN_2X64W(vecProduct2, 31);
        x2 = IVP_ADDN_2X32(x2, IVP_SLAN_2X32(delta_x, 2));

        vecProduct3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x3), hvecHalfDenominator3);
        IVP_MULAHN_2X16X32_1(vecProduct3, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x3), hvecHalfDenominator3);
        vecHalf_Denominator_times_x = IVP_PACKVRN_2X64W(vecProduct3, 31);
        vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v)(1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x3), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct3, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x3), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        delta_x = IVP_PACKVRN_2X64W(vecProduct3, 31);
        x3 = IVP_ADDN_2X32(x3, IVP_SLAN_2X32(delta_x, 2));

        vecProduct4 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x4), hvecHalfDenominator4);
        IVP_MULAHN_2X16X32_1(vecProduct4, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x4), hvecHalfDenominator4);
        vecHalf_Denominator_times_x = IVP_PACKVRN_2X64W(vecProduct4, 31);
        vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v)(1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct4 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x4), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct4, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x4), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        delta_x = IVP_PACKVRN_2X64W(vecProduct4, 31);
        x4 = IVP_ADDN_2X32(x4, IVP_SLAN_2X32(delta_x, 2));

      }
      x1 = IVP_SLAN_2X32(x1, 1);
      x2 = IVP_SLAN_2X32(x2, 1);
      x3 = IVP_SLAN_2X32(x3, 1);
      x4 = IVP_SLAN_2X32(x4, 1);

      vboolN_2 vbMaxRestrict = IVP_GTN_2X32(x1, (xb_vecN_2x32v)((1 << 30) - 1));
      x1 = IVP_MOVN_2X32T(x1, IVP_MAXN_2X32(x1, 0x7FFFFFFF), vbMaxRestrict);

      vboolN_2 vbMinRestrict = IVP_GTN_2X32(x1, (xb_vecN_2x32v)(-((1 << 30) - 1)));
      x1 = IVP_MOVN_2X32T(x1, IVP_MINN_2X32(x1, 0x80000000), vbMinRestrict);

      vbMaxRestrict = IVP_GTN_2X32(x2, (xb_vecN_2x32v)((1 << 30) - 1));
      x2 = IVP_MOVN_2X32T(x2, IVP_MAXN_2X32(x2, 0x7FFFFFFF), vbMaxRestrict);

      vbMinRestrict = IVP_GTN_2X32(x2, (xb_vecN_2x32v)(-((1 << 30) - 1)));
      x2 = IVP_MOVN_2X32T(x2, IVP_MINN_2X32(x2, 0x80000000), vbMinRestrict);

      vbMaxRestrict = IVP_GTN_2X32(x3, (xb_vecN_2x32v)((1 << 30) - 1));
      x3 = IVP_MOVN_2X32T(x3, IVP_MAXN_2X32(x3, 0x7FFFFFFF), vbMaxRestrict);

      vbMinRestrict = IVP_GTN_2X32(x3, (xb_vecN_2x32v)(-((1 << 30) - 1)));
      x3 = IVP_MOVN_2X32T(x3, IVP_MINN_2X32(x3, 0x80000000), vbMinRestrict);

      vbMaxRestrict = IVP_GTN_2X32(x4, (xb_vecN_2x32v)((1 << 30) - 1));
      x4 = IVP_MOVN_2X32T(x4, IVP_MAXN_2X32(x4, 0x7FFFFFFF), vbMaxRestrict);

      vbMinRestrict = IVP_GTN_2X32(x4, (xb_vecN_2x32v)(-((1 << 30) - 1)));
      x4 = IVP_MOVN_2X32T(x4, IVP_MINN_2X32(x4, 0x80000000), vbMinRestrict);

      xb_vecN_2x32v hvecSum0, hvecSum1, hvecSum2, hvecSum3;
      xb_vecN_2x32v hvecOne = 1;
      // Part A
      // Start by multiplying with the reciprocal of SumOfExps

      wvecA0 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x1), hvecSumExpA0);
      IVP_MULAHN_2X16X32_1(wvecA0, IVP_MOVNX16_FROMN_2X32(x1), hvecSumExpA0);
      hvecSumExpA0 = IVP_PACKVRN_2X64W(wvecA0, 31);
      hvecShift0 = IVP_ADDN_2X32(hvecHeadRoomPlusOne0, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
      hvecSum0 = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift0, hvecOne));
      hvecSumExpA0 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExpA0, hvecSum0), hvecShift0);

      wvecA1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x2), hvecSumExpA1);
      IVP_MULAHN_2X16X32_1(wvecA1, IVP_MOVNX16_FROMN_2X32(x2), hvecSumExpA1);
      hvecSumExpA1 = IVP_PACKVRN_2X64W(wvecA1, 31);
      hvecShift1 = IVP_ADDN_2X32(hvecHeadRoomPlusOne1, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
      hvecSum1 = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift1, hvecOne));
      hvecSumExpA1 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExpA1, hvecSum1), hvecShift1);

      //for 2 and 3
      wvecA2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x3), hvecSumExpA2);
      IVP_MULAHN_2X16X32_1(wvecA2, IVP_MOVNX16_FROMN_2X32(x3), hvecSumExpA2);
      hvecSumExpA2 = IVP_PACKVRN_2X64W(wvecA2, 31);
      hvecShift2 = IVP_ADDN_2X32(hvecHeadRoomPlusOne2, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
      hvecSum2 = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift2, hvecOne));
      hvecSumExpA2 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExpA2, hvecSum2), hvecShift2);

      wvecA3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x4), hvecSumExpA3);
      IVP_MULAHN_2X16X32_1(wvecA3, IVP_MOVNX16_FROMN_2X32(x4), hvecSumExpA3);
      hvecSumExpA3 = IVP_PACKVRNRN_2X64W(wvecA3, 31);
      hvecShift3 = IVP_ADDN_2X32(hvecHeadRoomPlusOne3, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
      hvecSum3 = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift3, hvecOne));
      hvecSumExpA3 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExpA3, hvecSum3), hvecShift3);


      // Part B
      // Start by multiplying with the reciprocal of SumOfExps
      wvecB0 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x1), hvecSumExpB0);
      IVP_MULAHN_2X16X32_1(wvecB0, IVP_MOVNX16_FROMN_2X32(x1), hvecSumExpB0);
      hvecSumExpB0 = IVP_PACKVRN_2X64W(wvecB0, 31);
      hvecShift0 = IVP_ADDN_2X32(hvecHeadRoomPlusOne0, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
      hvecSum0 = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift0, hvecOne));
      hvecSumExpB0 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExpB0, hvecSum0), hvecShift0);

      wvecB1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x2), hvecSumExpB1);
      IVP_MULAHN_2X16X32_1(wvecB1, IVP_MOVNX16_FROMN_2X32(x2), hvecSumExpB1);
      hvecSumExpB1 = IVP_PACKVRN_2X64W(wvecB1, 31);
      hvecShift1 = IVP_ADDN_2X32(hvecHeadRoomPlusOne1, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
      hvecSum1 = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift1, hvecOne));
      hvecSumExpB1 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExpB1, hvecSum1), hvecShift1);


      //for 2 and 3
      wvecB2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x3), hvecSumExpB2);
      IVP_MULAHN_2X16X32_1(wvecB2, IVP_MOVNX16_FROMN_2X32(x3), hvecSumExpB2);
      hvecSumExpB2 = IVP_PACKVRN_2X64W(wvecB2, 31);
      hvecShift2 = IVP_ADDN_2X32(hvecHeadRoomPlusOne2, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
      hvecSum2 = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift2, hvecOne));
      hvecSumExpB2 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExpB2, hvecSum2), hvecShift2);

      wvecB3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x4), hvecSumExpB3);
      IVP_MULAHN_2X16X32_1(wvecB3, IVP_MOVNX16_FROMN_2X32(x4), hvecSumExpB3);
      hvecSumExpB3 = IVP_PACKVRNRN_2X64W(wvecB3, 31);
      hvecShift3 = IVP_ADDN_2X32(hvecHeadRoomPlusOne3, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
      hvecSum3 = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift3, hvecOne));
      hvecSumExpB3 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExpB3, hvecSum3), hvecShift3);

      vecRes0 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecSumExpA1, hvecSumExpA0, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));
      vecRes1 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecSumExpA3, hvecSumExpA2, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));
      MORPH_OP_CLAMP(vecRes0, vecRes1, dvecResA);

      vecRes0 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecSumExpB1, hvecSumExpB0, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));
      vecRes1 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecSumExpB3, hvecSumExpB2, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));

      MORPH_OP_CLAMP(vecRes0, vecRes1, dvecResB);

      IVP_DSEL2NX8I(dvecResB, dvecResA, dvecResB, dvecResA, IVP_DSELI_8B_INTERLEAVE_1);

      MORPH_OP_ALIGN_STORE_2NX8_XP(dvecResA, vaStore, pvecOutput, 2 * (dim2Size - y));
      MORPH_OP_ALIGN_STORE_2NX8_XP(dvecResB, vaStore, pvecOutput, 2 * (dim2Size - y - XCHAL_IVPN_SIMD_WIDTH));

      MORPH_OP_STORE_FLUSH(vaStore, pvecOutput);
    }
  }
  return XI_ERROR_STATUS();
}

XI_ERR_TYPE MAKE_NAME2(softmaxA3D, DIM1_LUT_DEPTH_LESSTHAN16)(const xi_pTile3D inTile,
                                                                     const xi_pArray softmaxArray,
                                                                     xi_pTile3D outTile,
                                                                     const xi_cnn_softmaxA_params  * pparams,
                                                                     xi_pArray scratchArray)

{
  /* Error Checks */
  XI_ERROR_CHECKS()
  {
    MORPH_IDT_CHECK(inTile);
    MORPH_IDT_CHECK(outTile);
    XI_CHECK_ARRAY_S32(softmaxArray);
    MORPH_CHECK_ARRAY(scratchArray);
    XI_CHECK_TILE3D_SIZE_EQ(inTile, outTile);
    XI_CHECK_POINTER(pparams);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(inTile) <= 16, XI_ERR_BADARG, \
      "\ninTile dim1 size = %d, inTile dim1 Size should be less than or equal to 16", XI_TILE3D_GET_DIM1(inTile));
    XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(softmaxArray) == 256, XI_ERR_BADARG, \
      "\nsoftmaxArray LUT Size = %d, softmaxArray LUT Size should be equal to 256", XI_ARRAY_GET_WIDTH(softmaxArray));
    XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(softmaxArray) == 256, XI_ERR_BADARG, \
      "\nsoftmaxArray LUT Size = %d, softmaxArray LUT Size should be equal to 256", XI_ARRAY_GET_WIDTH(softmaxArray));
    XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(scratchArray) == XI_TILE3D_GET_DIM1(inTile) * 64, XI_ERR_BADARG, \
      "\nscratchArray Size = %d, scratchArray Size should be equal to %d",XI_ARRAY_GET_WIDTH(scratchArray), XI_TILE3D_GET_DIM1(inTile) * 64);
    XI_CHECK_ERROR(XI_TILE3D_GET_DATA_ORDER(inTile) == XI_TILE3D_GET_DATA_ORDER(outTile), XI_ERR_BADARG, \
      "\nData Order of InputTile = %d, OutputTile= %d, Data order of input and output tile should be same", \
      XI_TILE3D_GET_DATA_ORDER(inTile), XI_TILE3D_GET_DATA_ORDER(outTile));
  }

  /* Getting parameters from the tile structures */
  const int32_t dim1Size = XI_TILE3D_GET_DIM1(inTile);
  const int32_t dim2Size = XI_TILE3D_GET_DIM2(inTile);
  const int32_t inDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(inTile);
  const int32_t dim3Size = XI_TILE3D_GET_DIM3(inTile);
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
  MORPH_IDT_SCALAR *pInput = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile);
  MORPH_IDT_SCALAR *pOutput = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(outTile);
  int32_t  y, z;

  static const int kAccumulationIntegerBits = 12;

  int32_t *LUTOut = (int32_t *)XI_ARRAY_GET_BUFF_PTR(softmaxArray);

  MORPH_IDT_SCALAR *pInData, *pOut;
  MORPH_IDT_2Nx8 * pvecInp;
  valign va, vaStore;
  MORPH_IDT_2Nx8  dvecMax, dvecRes;

  xb_vecN_2x32v hvecSumExp0, hvecSumExp1, hvecSumExp2, hvecSumExp3, vecSumExp;

  MORPH_IDT_2Nx8 * pvecOutput;;
  vaStore = IVP_ZALIGN();
  xb_vec2Nx8 * dvecLUTBase = (xb_vec2Nx8 *)LUTOut;
  valign va1 = IVP_LA2NX8_PP(dvecLUTBase);
  xb_vec2Nx8 dvecLUT0LL, dvecLUT1LL, dvecLUT2LL, dvecLUT3LL, dvecLUT0LH, dvecLUT1LH, dvecLUT2LH, dvecLUT3LH;
  xb_vec2Nx8 dvecLUT0HL, dvecLUT1HL, dvecLUT2HL, dvecLUT3HL, dvecLUT0HH, dvecLUT1HH, dvecLUT2HH, dvecLUT3HH;


  IVP_LA2NX8_IP(dvecLUT0LL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT1LL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT2LL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT3LL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT0LH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT1LH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT2LH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT3LH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT0HL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT1HL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT2HL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT3HL, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT0HH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT1HH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT2HH, va1, dvecLUTBase);
  IVP_LA2NX8_IP(dvecLUT3HH, va1, dvecLUTBase);


  MORPH_IDT_2Nx8 xvec_max, xvec1;
  MORPH_IDT_2Nx8 dvec_diff1;

  xb_vecN_2x32v hvecSumExpA0, hvecSumExpA1, hvecSumExpA2, hvecSumExpA3;

  xb_vecN_2x64w vecProduct1, vecProduct2, vecProduct3, vecProduct4;
  xb_vecN_2x64w wvecA0, wvecA1, wvecA2, wvecA3;
  xb_vecN_2x32v hvecShift0, hvecShift1, hvecShift2, hvecShift3;


  xb_vecNx16 vecRes0, vecRes1;

#ifdef __XCC__
  XT_MEMW(); /* Adding Memory Wait as Gather and Normal Load/Stores are not synchronized */
#endif
  MORPH_IDT_2Nx8* pvecOutputTemp;
  MORPH_IDT_2Nx8* pvecInpTemp;

  int depth = dim1Size;
  xb_vecNx16U vecOffset1 = IVP_MULNX16UPACKL(IVP_SEQNX16U(), (xb_vecNx16U)depth);
  xb_vecNx16U vecOffset2 = IVP_MULNX16UPACKL(IVP_ADDNX16U(IVP_SEQNX16U(), 32), (xb_vecNx16U)depth);

  int16_t max_in_row;
  for (z = 0; z < dim3Size; z++)     /* along the image depth(WHD)/height(DWH) */
  {
    for (y = 0; y <= dim2Size - (2 * XCHAL_IVPN_SIMD_WIDTH); y += (2 * XCHAL_IVPN_SIMD_WIDTH))   /* along the image height(WHD)/width(DWH) */
    {
      max_in_row = MORPH_IDT_SCALAR_MIN;
      dvecMax = MORPH_IDT_SCALAR_MIN;

      pInData = (MORPH_IDT_SCALAR *)(pInput + ((z * inDataPitch2) + (y * inDataPitch1)));
      pOut = pOutput + ((z * outDataPitch2) + (y * outDataPitch1));

      pvecInp = (MORPH_IDT_2Nx8 *)(pInData);
      pvecInpTemp = (MORPH_IDT_2Nx8 *)(pInData);
      pvecOutput = (MORPH_IDT_2Nx8 *)(pOut);
      pvecOutputTemp = (MORPH_IDT_2Nx8 *)(pOut);

      va = MORPH_OP_PRIME_LOAD_2NX8_PP(pvecInp);
      vaStore = IVP_ZALIGN();

      // store one row transpose to outbuffer
      // find max along depth
      xvec_max = (MORPH_IDT_2Nx8)max_in_row;
      for (int x = 0; x < depth; ++x)
      {
        xb_vecNx16U vOffset1 = IVP_ADDNX16U(vecOffset1, (xb_vecNx16U)x);
        xb_vecNx16U vOffset2 = IVP_ADDNX16U(vecOffset2, (xb_vecNx16U)x);
        MORPH_IDT_2Nx8 dvec1;

        xb_gsr vg1 = MORPH_OP_GATHERA_NX8(pInData, vOffset1);
        dvec1 = MORPH_OP_GATHERD_NX8_L(vg1);

        xb_gsr vg2 = MORPH_OP_GATHERA_NX8(pInData, vOffset2);
        MORPH_OP_GATHERD_NX8_H(dvec1, vg2);

        xvec_max = MORPH_OP_MAX2NX8(xvec_max, dvec1);

        MORPH_OP_ALIGN_STORE_2NX8_IP(dvec1, vaStore, pvecOutputTemp);
      }
      // FPXP
      MORPH_OP_STORE_FLUSH(vaStore, pvecOutputTemp);

      vecSumExp = 0;
      hvecSumExp0 = 0;
      hvecSumExp1 = 0;
      hvecSumExp2 = 0;
      hvecSumExp3 = 0;

      pvecInpTemp = pvecOutput; // point to output
      pvecOutputTemp = (MORPH_IDT_2Nx8 *)XI_ARRAY_GET_BUFF_PTR(scratchArray); // write to input 1st row

      valign vaOut = IVP_ZALIGN();
      vaStore = IVP_ZALIGN();
      // sum along depth
      va = MORPH_OP_PRIME_LOAD_2NX8_PP(pvecInpTemp);
      for (int i = 0; i < depth; ++i)
      {
        MORPH_OP_ALIGN_LOAD_2NX8_IP(xvec1, va, pvecInpTemp);

        dvec_diff1 = MORPH_OP_SUB2NX8(xvec_max, xvec1);

        SOFTMAX_APPLY_LUT(dvec_diff1, hvecSumExpA0, hvecSumExpA1, hvecSumExpA2, hvecSumExpA3);

        hvecSumExp0 = IVP_ADDN_2X32(IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpA0), 12),
          hvecSumExp0);
        hvecSumExp1 = IVP_ADDN_2X32(IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpA1), 12)
          , hvecSumExp1);
        hvecSumExp2 = IVP_ADDN_2X32(IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpA2), 12)
          , hvecSumExp2);
        hvecSumExp3 = IVP_ADDN_2X32(IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpA3), 12),
          hvecSumExp3);

        MORPH_OP_ALIGN_STORE_2NX8_IP(dvec_diff1, vaOut, pvecOutputTemp);
      }
      MORPH_OP_STORE_FLUSH(vaOut, pvecOutputTemp);

      xb_vecN_2x32v hvecShiftedSumMinusOne0, hvecShiftedSumMinusOne1, hvecShiftedSumMinusOne2, hvecShiftedSumMinusOne3;
      xb_vecN_2x32v hvecHeadRoomPlusOne0, hvecHeadRoomPlusOne1, hvecHeadRoomPlusOne2, hvecHeadRoomPlusOne3;

      //int headroom_plus_one = XT_NSAU((uint32_t)fixed_sum_of_exps);
      hvecHeadRoomPlusOne0 = IVP_NSAUN_2X32(hvecSumExp0);
      hvecHeadRoomPlusOne1 = IVP_NSAUN_2X32(hvecSumExp1);
      hvecHeadRoomPlusOne2 = IVP_NSAUN_2X32(hvecSumExp2);
      hvecHeadRoomPlusOne3 = IVP_NSAUN_2X32(hvecSumExp3);

      //int num_bits_over_unit = kAccumulationIntegerBits - headroom_plus_one;
      hvecShiftedSumMinusOne0 = IVP_SUBN_2X32U(IVP_SLAN_2X32(hvecSumExp0, hvecHeadRoomPlusOne0), (xb_vecN_2x32Uv)(1 << 31));
      hvecShiftedSumMinusOne1 = IVP_SUBN_2X32U(IVP_SLAN_2X32(hvecSumExp1, hvecHeadRoomPlusOne1), (xb_vecN_2x32Uv)(1 << 31));
      hvecShiftedSumMinusOne2 = IVP_SUBN_2X32U(IVP_SLAN_2X32(hvecSumExp2, hvecHeadRoomPlusOne2), (xb_vecN_2x32Uv)(1 << 31));
      hvecShiftedSumMinusOne3 = IVP_SUBN_2X32U(IVP_SLAN_2X32(hvecSumExp3, hvecHeadRoomPlusOne3), (xb_vecN_2x32Uv)(1 << 31));

      // int32_t shifted_sum_minus_one = (int32_t)(((uint32_t)(fixed_sum_of_exps) << headroom_plus_one) - ((uint32_t)(1) << 31));
      hvecHeadRoomPlusOne0 = IVP_SUBN_2X32((xb_vecN_2x32Uv)kAccumulationIntegerBits, hvecHeadRoomPlusOne0);
      hvecHeadRoomPlusOne1 = IVP_SUBN_2X32((xb_vecN_2x32Uv)kAccumulationIntegerBits, hvecHeadRoomPlusOne1);
      hvecHeadRoomPlusOne2 = IVP_SUBN_2X32((xb_vecN_2x32Uv)kAccumulationIntegerBits, hvecHeadRoomPlusOne2);
      hvecHeadRoomPlusOne3 = IVP_SUBN_2X32((xb_vecN_2x32Uv)kAccumulationIntegerBits, hvecHeadRoomPlusOne3);

      // FixedPoint0 shifted_scale = gemmlowp::one_over_one_plus_x_for_x_in_0_1( FixedPoint0::FromRaw(shifted_sum_minus_one));
      // Strat of one_over_one_plus_x_for_x_in_0_1
      const int32_t constant_48_over_17 = 1515870810;                //Q2.29 format
      const int32_t constant_neg_32_over_17 = -1010580540;               //Q2.29 format

      vecProduct1 = IVP_MULN_2X16X32_0((xb_vecNx16)1, (xb_vecN_2x32v)(ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct1, (xb_vecNx16)1, IVP_ADDN_2X32(hvecShiftedSumMinusOne0, (xb_vecN_2x32v)1));
      xb_vecN_2x32v hvecHalfDenominator1 = IVP_PACKVRNRN_2X64W(vecProduct1, 1);

      vecProduct1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator1);
      IVP_MULAHN_2X16X32_1(vecProduct1, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator1);

      vecProduct2 = IVP_MULN_2X16X32_0((xb_vecNx16)1, (xb_vecN_2x32v)(ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct2, (xb_vecNx16)1, IVP_ADDN_2X32(hvecShiftedSumMinusOne1, (xb_vecN_2x32v)1));
      xb_vecN_2x32v hvecHalfDenominator2 = IVP_PACKVRNRN_2X64W(vecProduct2, 1);

      vecProduct2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator2);
      IVP_MULAHN_2X16X32_1(vecProduct2, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator2);

      vecProduct3 = IVP_MULN_2X16X32_0((xb_vecNx16)1, (xb_vecN_2x32v)(ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct3, (xb_vecNx16)1, IVP_ADDN_2X32(hvecShiftedSumMinusOne2, (xb_vecN_2x32v)1));
      xb_vecN_2x32v hvecHalfDenominator3 = IVP_PACKVRNRN_2X64W(vecProduct3, 1);

      vecProduct3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator3);
      IVP_MULAHN_2X16X32_1(vecProduct3, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator3);

      vecProduct4 = IVP_MULN_2X16X32_0((xb_vecNx16)1, (xb_vecN_2x32v)(ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct4, (xb_vecNx16)1, IVP_ADDN_2X32(hvecShiftedSumMinusOne3, (xb_vecN_2x32v)1));
      xb_vecN_2x32v hvecHalfDenominator4 = IVP_PACKVRNRN_2X64W(vecProduct4, 1);

      vecProduct4 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator4);
      IVP_MULAHN_2X16X32_1(vecProduct4, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator4);

      xb_vecN_2x32v x1 = IVP_ADDN_2X32((IVP_PACKVRN_2X64W(vecProduct1, 31)), (xb_vecN_2x32v)constant_48_over_17);
      xb_vecN_2x32v x2 = IVP_ADDN_2X32((IVP_PACKVRN_2X64W(vecProduct2, 31)), (xb_vecN_2x32v)constant_48_over_17);
      xb_vecN_2x32v x3 = IVP_ADDN_2X32((IVP_PACKVRN_2X64W(vecProduct3, 31)), (xb_vecN_2x32v)constant_48_over_17);
      xb_vecN_2x32v x4 = IVP_ADDN_2X32((IVP_PACKVRN_2X64W(vecProduct4, 31)), (xb_vecN_2x32v)constant_48_over_17);

      for (int i = 0; i < 3; i++)
      {
        vecProduct1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x1), hvecHalfDenominator1);
        IVP_MULAHN_2X16X32_1(vecProduct1, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x1), hvecHalfDenominator1);
        xb_vecN_2x32v vecHalf_Denominator_times_x = IVP_PACKVRN_2X64W(vecProduct1, 31);
        xb_vecN_2x32v vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v)(1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x1), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct1, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x1), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        xb_vecN_2x32v delta_x = IVP_PACKVRN_2X64W(vecProduct1, 31);
        x1 = IVP_ADDN_2X32(x1, IVP_SLAN_2X32(delta_x, 2));

        vecProduct2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x2), hvecHalfDenominator2);
        IVP_MULAHN_2X16X32_1(vecProduct2, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x2), hvecHalfDenominator2);
        vecHalf_Denominator_times_x = IVP_PACKVRN_2X64W(vecProduct2, 31);
        vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v)(1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x2), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct2, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x2), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        delta_x = IVP_PACKVRN_2X64W(vecProduct2, 31);
        x2 = IVP_ADDN_2X32(x2, IVP_SLAN_2X32(delta_x, 2));

        vecProduct3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x3), hvecHalfDenominator3);
        IVP_MULAHN_2X16X32_1(vecProduct3, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x3), hvecHalfDenominator3);
        vecHalf_Denominator_times_x = IVP_PACKVRN_2X64W(vecProduct3, 31);
        vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v)(1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x3), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct3, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x3), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        delta_x = IVP_PACKVRN_2X64W(vecProduct3, 31);
        x3 = IVP_ADDN_2X32(x3, IVP_SLAN_2X32(delta_x, 2));

        vecProduct4 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x4), hvecHalfDenominator4);
        IVP_MULAHN_2X16X32_1(vecProduct4, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x4), hvecHalfDenominator4);
        vecHalf_Denominator_times_x = IVP_PACKVRN_2X64W(vecProduct4, 31);
        vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v)(1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct4 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x4), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct4, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x4), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        delta_x = IVP_PACKVRN_2X64W(vecProduct4, 31);
        x4 = IVP_ADDN_2X32(x4, IVP_SLAN_2X32(delta_x, 2));

      }
      x1 = IVP_SLAN_2X32(x1, 1);
      x2 = IVP_SLAN_2X32(x2, 1);
      x3 = IVP_SLAN_2X32(x3, 1);
      x4 = IVP_SLAN_2X32(x4, 1);

      vboolN_2 vbMaxRestrict = IVP_GTN_2X32(x1, (xb_vecN_2x32v)((1 << 30) - 1));
      x1 = IVP_MOVN_2X32T(x1, IVP_MAXN_2X32(x1, 0x7FFFFFFF), vbMaxRestrict);

      vboolN_2 vbMinRestrict = IVP_GTN_2X32(x1, (xb_vecN_2x32v)(-((1 << 30) - 1)));
      x1 = IVP_MOVN_2X32T(x1, IVP_MINN_2X32(x1, 0x80000000), vbMinRestrict);

      vbMaxRestrict = IVP_GTN_2X32(x2, (xb_vecN_2x32v)((1 << 30) - 1));
      x2 = IVP_MOVN_2X32T(x2, IVP_MAXN_2X32(x2, 0x7FFFFFFF), vbMaxRestrict);

      vbMinRestrict = IVP_GTN_2X32(x2, (xb_vecN_2x32v)(-((1 << 30) - 1)));
      x2 = IVP_MOVN_2X32T(x2, IVP_MINN_2X32(x2, 0x80000000), vbMinRestrict);

      vbMaxRestrict = IVP_GTN_2X32(x3, (xb_vecN_2x32v)((1 << 30) - 1));
      x3 = IVP_MOVN_2X32T(x3, IVP_MAXN_2X32(x3, 0x7FFFFFFF), vbMaxRestrict);

      vbMinRestrict = IVP_GTN_2X32(x3, (xb_vecN_2x32v)(-((1 << 30) - 1)));
      x3 = IVP_MOVN_2X32T(x3, IVP_MINN_2X32(x3, 0x80000000), vbMinRestrict);

      vbMaxRestrict = IVP_GTN_2X32(x4, (xb_vecN_2x32v)((1 << 30) - 1));
      x4 = IVP_MOVN_2X32T(x4, IVP_MAXN_2X32(x4, 0x7FFFFFFF), vbMaxRestrict);

      vbMinRestrict = IVP_GTN_2X32(x4, (xb_vecN_2x32v)(-((1 << 30) - 1)));
      x4 = IVP_MOVN_2X32T(x4, IVP_MINN_2X32(x4, 0x80000000), vbMinRestrict);

      // Part A
      // Start by multiplying with the reciprocal of SumOfExps
      xb_vecN_2x32v hvecSum0, hvecSum1, hvecSum2, hvecSum3;
      xb_vecN_2x32v hvecOne = 1;

      pvecInp = (MORPH_IDT_2Nx8 *)XI_ARRAY_GET_BUFF_PTR(scratchArray); // write to input 1st row
      va = MORPH_OP_PRIME_LOAD_2NX8_PP(pvecInp);
      // sum along depth

      for (int x = 0; x < depth; ++x) {

        MORPH_OP_ALIGN_LOAD_2NX8_IP(dvec_diff1, va, pvecInp);

        SOFTMAX_APPLY_LUT(dvec_diff1, hvecSumExpA0, hvecSumExpA1, hvecSumExpA2, hvecSumExpA3);

        wvecA0 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x1), hvecSumExpA0);
        IVP_MULAHN_2X16X32_1(wvecA0, IVP_MOVNX16_FROMN_2X32(x1), hvecSumExpA0);
        hvecSumExpA0 = IVP_PACKVRN_2X64W(wvecA0, 31);
        hvecShift0 = IVP_ADDN_2X32(hvecHeadRoomPlusOne0, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
        hvecSum0 = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift0, hvecOne));
        hvecSumExpA0 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExpA0, hvecSum0), hvecShift0);

        wvecA1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x2), hvecSumExpA1);
        IVP_MULAHN_2X16X32_1(wvecA1, IVP_MOVNX16_FROMN_2X32(x2), hvecSumExpA1);
        hvecSumExpA1 = IVP_PACKVRN_2X64W(wvecA1, 31);
        hvecShift1 = IVP_ADDN_2X32(hvecHeadRoomPlusOne1, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
        hvecSum1 = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift1, hvecOne));
        hvecSumExpA1 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExpA1, hvecSum1), hvecShift1);

        //for 2 and 3
        wvecA2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x3), hvecSumExpA2);
        IVP_MULAHN_2X16X32_1(wvecA2, IVP_MOVNX16_FROMN_2X32(x3), hvecSumExpA2);
        hvecSumExpA2 = IVP_PACKVRN_2X64W(wvecA2, 31);
        hvecShift2 = IVP_ADDN_2X32(hvecHeadRoomPlusOne2, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
        hvecSum2 = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift2, hvecOne));
        hvecSumExpA2 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExpA2, hvecSum2), hvecShift2);

        wvecA3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x4), hvecSumExpA3);
        IVP_MULAHN_2X16X32_1(wvecA3, IVP_MOVNX16_FROMN_2X32(x4), hvecSumExpA3);
        hvecSumExpA3 = IVP_PACKVRNRN_2X64W(wvecA3, 31);
        hvecShift3 = IVP_ADDN_2X32(hvecHeadRoomPlusOne3, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
        hvecSum3 = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift3, hvecOne));
        hvecSumExpA3 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExpA3, hvecSum3), hvecShift3);

        vecRes0 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecSumExpA1, hvecSumExpA0, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));
        vecRes1 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecSumExpA3, hvecSumExpA2, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));

        MORPH_OP_CLAMP(vecRes0, vecRes1, dvecRes);

        xb_vecNx16U vOffset1 = IVP_ADDNX16U(vecOffset1, (xb_vecNx16U)x);
        xb_vecNx16U vOffset2 = IVP_ADDNX16U(vecOffset2, (xb_vecNx16U)x);

        MORPH_OP_SCATTER2NX8_L(dvecRes, pOut, vOffset1);
        MORPH_OP_SCATTER2NX8_H(dvecRes, pOut, vOffset2);
      }
    }
    if (y < dim2Size)   /* along the image height(WHD)/width(DWH) */
    {
      max_in_row = MORPH_IDT_SCALAR_MIN;
      dvecMax = MORPH_IDT_SCALAR_MIN;

      pInData = (MORPH_IDT_SCALAR *)(pInput + ((z * inDataPitch2) + (y * inDataPitch1)));
      pOut = pOutput + ((z * outDataPitch2) + (y * outDataPitch1));

      pvecInp = (MORPH_IDT_2Nx8 *)(pInData);
      pvecInpTemp = (MORPH_IDT_2Nx8 *)(pInData);
      pvecOutput = (MORPH_IDT_2Nx8 *)(pOut);
      pvecOutputTemp = (MORPH_IDT_2Nx8 *)(pOut);

      va = MORPH_OP_PRIME_LOAD_2NX8_PP(pvecInp);
      vaStore = IVP_ZALIGN();

      // store one row transpose to outbuffer
      // find max along depth
      vboolN vboolMsk1 = IVP_LTUNX16(IVP_SEQNX16U(), dim2Size - y);
      vboolN vboolMsk2 = IVP_LTUNX16(IVP_ADDNX16U(IVP_SEQNX16U(), 32), dim2Size - y);
      xvec_max = (MORPH_IDT_2Nx8)max_in_row;
      for (int x = 0; x < depth; x++)
      {
        xb_vecNx16U vOffset1 = IVP_ADDNX16U(vecOffset1, (xb_vecNx16U)x);
        xb_vecNx16U vOffset2 = IVP_ADDNX16U(vecOffset2, (xb_vecNx16U)x);

        vOffset1 = IVP_MOVNX16UT(vOffset1, (xb_vecNx16U)0, vboolMsk1);
        vOffset2 = IVP_MOVNX16UT(vOffset2, (xb_vecNx16U)0, vboolMsk2);
        MORPH_IDT_2Nx8 dvec1;

        xb_gsr vg1 = MORPH_OP_GATHERA_NX8T(pInData, vOffset1, vboolMsk1);
        dvec1 = MORPH_OP_GATHERD_NX8_L(vg1);

        xb_gsr vg2 = MORPH_OP_GATHERA_NX8T(pInData, vOffset2, vboolMsk2);
        MORPH_OP_GATHERD_NX8_H(dvec1, vg2);

        xvec_max = MORPH_OP_MAX2NX8(xvec_max, dvec1);

        MORPH_OP_ALIGN_STORE_2NX8_XP(dvec1, vaStore, pvecOutputTemp, (dim2Size - y));
      }
      // FPXP
      MORPH_OP_STORE_FLUSH(vaStore, pvecOutputTemp);

      vecSumExp = 0;
      hvecSumExp0 = 0;
      hvecSumExp1 = 0;
      hvecSumExp2 = 0;
      hvecSumExp3 = 0;

      pvecInpTemp = pvecOutput; // point to output
      pvecOutputTemp = (MORPH_IDT_2Nx8 *)XI_ARRAY_GET_BUFF_PTR(scratchArray); // write to input 1st row

      valign vaOut = IVP_ZALIGN();
      vaStore = IVP_ZALIGN();
      // sum along depth
      va = MORPH_OP_PRIME_LOAD_2NX8_PP(pvecInpTemp);
      for (int i = 0; i < depth; i++)
      {
        MORPH_OP_ALIGN_LOAD_2Nx8_VARIABLE(xvec1, va, pvecInpTemp, (dim2Size - y));

        dvec_diff1 = MORPH_OP_SUB2NX8(xvec_max, xvec1);

        SOFTMAX_APPLY_LUT(dvec_diff1, hvecSumExpA0, hvecSumExpA1, hvecSumExpA2, hvecSumExpA3);

        hvecSumExp0 = IVP_ADDN_2X32(IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpA0), 12),
          hvecSumExp0);
        hvecSumExp1 = IVP_ADDN_2X32(IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpA1), 12)
          , hvecSumExp1);
        hvecSumExp2 = IVP_ADDN_2X32(IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpA2), 12)
          , hvecSumExp2);
        hvecSumExp3 = IVP_ADDN_2X32(IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, hvecSumExpA3), 12),
          hvecSumExp3);

        MORPH_OP_ALIGN_STORE_2NX8_XP(dvec_diff1, vaOut, pvecOutputTemp, (dim2Size - y));
      }
      MORPH_OP_STORE_FLUSH(vaOut, pvecOutputTemp);

      xb_vecN_2x32v hvecShiftedSumMinusOne0, hvecShiftedSumMinusOne1, hvecShiftedSumMinusOne2, hvecShiftedSumMinusOne3;
      xb_vecN_2x32v hvecHeadRoomPlusOne0, hvecHeadRoomPlusOne1, hvecHeadRoomPlusOne2, hvecHeadRoomPlusOne3;

      //int headroom_plus_one = XT_NSAU((uint32_t)fixed_sum_of_exps);
      hvecHeadRoomPlusOne0 = IVP_NSAUN_2X32(hvecSumExp0);
      hvecHeadRoomPlusOne1 = IVP_NSAUN_2X32(hvecSumExp1);
      hvecHeadRoomPlusOne2 = IVP_NSAUN_2X32(hvecSumExp2);
      hvecHeadRoomPlusOne3 = IVP_NSAUN_2X32(hvecSumExp3);

      //int num_bits_over_unit = kAccumulationIntegerBits - headroom_plus_one;
      hvecShiftedSumMinusOne0 = IVP_SUBN_2X32U(IVP_SLAN_2X32(hvecSumExp0, hvecHeadRoomPlusOne0), (xb_vecN_2x32Uv)(1 << 31));
      hvecShiftedSumMinusOne1 = IVP_SUBN_2X32U(IVP_SLAN_2X32(hvecSumExp1, hvecHeadRoomPlusOne1), (xb_vecN_2x32Uv)(1 << 31));
      hvecShiftedSumMinusOne2 = IVP_SUBN_2X32U(IVP_SLAN_2X32(hvecSumExp2, hvecHeadRoomPlusOne2), (xb_vecN_2x32Uv)(1 << 31));
      hvecShiftedSumMinusOne3 = IVP_SUBN_2X32U(IVP_SLAN_2X32(hvecSumExp3, hvecHeadRoomPlusOne3), (xb_vecN_2x32Uv)(1 << 31));

      // int32_t shifted_sum_minus_one = (int32_t)(((uint32_t)(fixed_sum_of_exps) << headroom_plus_one) - ((uint32_t)(1) << 31));
      hvecHeadRoomPlusOne0 = IVP_SUBN_2X32((xb_vecN_2x32Uv)kAccumulationIntegerBits, hvecHeadRoomPlusOne0);
      hvecHeadRoomPlusOne1 = IVP_SUBN_2X32((xb_vecN_2x32Uv)kAccumulationIntegerBits, hvecHeadRoomPlusOne1);
      hvecHeadRoomPlusOne2 = IVP_SUBN_2X32((xb_vecN_2x32Uv)kAccumulationIntegerBits, hvecHeadRoomPlusOne2);
      hvecHeadRoomPlusOne3 = IVP_SUBN_2X32((xb_vecN_2x32Uv)kAccumulationIntegerBits, hvecHeadRoomPlusOne3);

      // FixedPoint0 shifted_scale = gemmlowp::one_over_one_plus_x_for_x_in_0_1( FixedPoint0::FromRaw(shifted_sum_minus_one));
      // Strat of one_over_one_plus_x_for_x_in_0_1
      const int32_t constant_48_over_17 = 1515870810;                //Q2.29 format
      const int32_t constant_neg_32_over_17 = -1010580540;               //Q2.29 format

      vecProduct1 = IVP_MULN_2X16X32_0((xb_vecNx16)1, (xb_vecN_2x32v)(ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct1, (xb_vecNx16)1, IVP_ADDN_2X32(hvecShiftedSumMinusOne0, (xb_vecN_2x32v)1));
      xb_vecN_2x32v hvecHalfDenominator1 = IVP_PACKVRNRN_2X64W(vecProduct1, 1);

      vecProduct1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator1);
      IVP_MULAHN_2X16X32_1(vecProduct1, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator1);

      vecProduct2 = IVP_MULN_2X16X32_0((xb_vecNx16)1, (xb_vecN_2x32v)(ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct2, (xb_vecNx16)1, IVP_ADDN_2X32(hvecShiftedSumMinusOne1, (xb_vecN_2x32v)1));
      xb_vecN_2x32v hvecHalfDenominator2 = IVP_PACKVRNRN_2X64W(vecProduct2, 1);

      vecProduct2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator2);
      IVP_MULAHN_2X16X32_1(vecProduct2, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator2);

      vecProduct3 = IVP_MULN_2X16X32_0((xb_vecNx16)1, (xb_vecN_2x32v)(ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct3, (xb_vecNx16)1, IVP_ADDN_2X32(hvecShiftedSumMinusOne2, (xb_vecN_2x32v)1));
      xb_vecN_2x32v hvecHalfDenominator3 = IVP_PACKVRNRN_2X64W(vecProduct3, 1);

      vecProduct3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator3);
      IVP_MULAHN_2X16X32_1(vecProduct3, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator3);

      vecProduct4 = IVP_MULN_2X16X32_0((xb_vecNx16)1, (xb_vecN_2x32v)(ONE_Q31));
      IVP_MULAN_2X16X32_0(vecProduct4, (xb_vecNx16)1, IVP_ADDN_2X32(hvecShiftedSumMinusOne3, (xb_vecN_2x32v)1));
      xb_vecN_2x32v hvecHalfDenominator4 = IVP_PACKVRNRN_2X64W(vecProduct4, 1);

      vecProduct4 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator4);
      IVP_MULAHN_2X16X32_1(vecProduct4, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)constant_neg_32_over_17), hvecHalfDenominator4);

      xb_vecN_2x32v x1 = IVP_ADDN_2X32((IVP_PACKVRN_2X64W(vecProduct1, 31)), (xb_vecN_2x32v)constant_48_over_17);
      xb_vecN_2x32v x2 = IVP_ADDN_2X32((IVP_PACKVRN_2X64W(vecProduct2, 31)), (xb_vecN_2x32v)constant_48_over_17);
      xb_vecN_2x32v x3 = IVP_ADDN_2X32((IVP_PACKVRN_2X64W(vecProduct3, 31)), (xb_vecN_2x32v)constant_48_over_17);
      xb_vecN_2x32v x4 = IVP_ADDN_2X32((IVP_PACKVRN_2X64W(vecProduct4, 31)), (xb_vecN_2x32v)constant_48_over_17);

      for (int i = 0; i < 3; i++)
      {
        vecProduct1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x1), hvecHalfDenominator1);
        IVP_MULAHN_2X16X32_1(vecProduct1, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x1), hvecHalfDenominator1);
        xb_vecN_2x32v vecHalf_Denominator_times_x = IVP_PACKVRN_2X64W(vecProduct1, 31);
        xb_vecN_2x32v vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v)(1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x1), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct1, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x1), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        xb_vecN_2x32v delta_x = IVP_PACKVRN_2X64W(vecProduct1, 31);
        x1 = IVP_ADDN_2X32(x1, IVP_SLAN_2X32(delta_x, 2));

        vecProduct2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x2), hvecHalfDenominator2);
        IVP_MULAHN_2X16X32_1(vecProduct2, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x2), hvecHalfDenominator2);
        vecHalf_Denominator_times_x = IVP_PACKVRN_2X64W(vecProduct2, 31);
        vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v)(1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x2), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct2, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x2), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        delta_x = IVP_PACKVRN_2X64W(vecProduct2, 31);
        x2 = IVP_ADDN_2X32(x2, IVP_SLAN_2X32(delta_x, 2));

        vecProduct3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x3), hvecHalfDenominator3);
        IVP_MULAHN_2X16X32_1(vecProduct3, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x3), hvecHalfDenominator3);
        vecHalf_Denominator_times_x = IVP_PACKVRN_2X64W(vecProduct3, 31);
        vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v)(1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x3), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct3, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x3), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        delta_x = IVP_PACKVRN_2X64W(vecProduct3, 31);
        x3 = IVP_ADDN_2X32(x3, IVP_SLAN_2X32(delta_x, 2));

        vecProduct4 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x4), hvecHalfDenominator4);
        IVP_MULAHN_2X16X32_1(vecProduct4, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x4), hvecHalfDenominator4);
        vecHalf_Denominator_times_x = IVP_PACKVRN_2X64W(vecProduct4, 31);
        vecOne_minus_half_denominator_times_x = (xb_vecN_2x32v)(1 << (31 - 2)) - vecHalf_Denominator_times_x;

        vecProduct4 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x4), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        IVP_MULAHN_2X16X32_1(vecProduct4, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)x4), (xb_vecN_2x32v)(vecOne_minus_half_denominator_times_x));
        delta_x = IVP_PACKVRN_2X64W(vecProduct4, 31);
        x4 = IVP_ADDN_2X32(x4, IVP_SLAN_2X32(delta_x, 2));

      }
      x1 = IVP_SLAN_2X32(x1, 1);
      x2 = IVP_SLAN_2X32(x2, 1);
      x3 = IVP_SLAN_2X32(x3, 1);
      x4 = IVP_SLAN_2X32(x4, 1);

      vboolN_2 vbMaxRestrict = IVP_GTN_2X32(x1, (xb_vecN_2x32v)((1 << 30) - 1));
      x1 = IVP_MOVN_2X32T(x1, IVP_MAXN_2X32(x1, 0x7FFFFFFF), vbMaxRestrict);

      vboolN_2 vbMinRestrict = IVP_GTN_2X32(x1, (xb_vecN_2x32v)(-((1 << 30) - 1)));
      x1 = IVP_MOVN_2X32T(x1, IVP_MINN_2X32(x1, 0x80000000), vbMinRestrict);

      vbMaxRestrict = IVP_GTN_2X32(x2, (xb_vecN_2x32v)((1 << 30) - 1));
      x2 = IVP_MOVN_2X32T(x2, IVP_MAXN_2X32(x2, 0x7FFFFFFF), vbMaxRestrict);

      vbMinRestrict = IVP_GTN_2X32(x2, (xb_vecN_2x32v)(-((1 << 30) - 1)));
      x2 = IVP_MOVN_2X32T(x2, IVP_MINN_2X32(x2, 0x80000000), vbMinRestrict);

      vbMaxRestrict = IVP_GTN_2X32(x3, (xb_vecN_2x32v)((1 << 30) - 1));
      x3 = IVP_MOVN_2X32T(x3, IVP_MAXN_2X32(x3, 0x7FFFFFFF), vbMaxRestrict);

      vbMinRestrict = IVP_GTN_2X32(x3, (xb_vecN_2x32v)(-((1 << 30) - 1)));
      x3 = IVP_MOVN_2X32T(x3, IVP_MINN_2X32(x3, 0x80000000), vbMinRestrict);

      vbMaxRestrict = IVP_GTN_2X32(x4, (xb_vecN_2x32v)((1 << 30) - 1));
      x4 = IVP_MOVN_2X32T(x4, IVP_MAXN_2X32(x4, 0x7FFFFFFF), vbMaxRestrict);

      vbMinRestrict = IVP_GTN_2X32(x4, (xb_vecN_2x32v)(-((1 << 30) - 1)));
      x4 = IVP_MOVN_2X32T(x4, IVP_MINN_2X32(x4, 0x80000000), vbMinRestrict);

      // Part A
      // Start by multiplying with the reciprocal of SumOfExps
      xb_vecN_2x32v hvecSum0, hvecSum1, hvecSum2, hvecSum3;
      xb_vecN_2x32v hvecOne = 1;

      pvecInp = (MORPH_IDT_2Nx8 *)XI_ARRAY_GET_BUFF_PTR(scratchArray); // write to input 1st row
      va = MORPH_OP_PRIME_LOAD_2NX8_PP(pvecInp);

      vbool2N vbMsk = IVP_LTU2NX8(IVP_SEQ2NX8U(), dim2Size - y);
      // sum along depth

      for (int x = 0; x < depth; x++)
      {
        MORPH_OP_ALIGN_LOAD_2Nx8_VARIABLE(dvec_diff1, va, pvecInp, (dim2Size - y));

        SOFTMAX_APPLY_LUT(dvec_diff1, hvecSumExpA0, hvecSumExpA1, hvecSumExpA2, hvecSumExpA3);

        wvecA0 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x1), hvecSumExpA0);
        IVP_MULAHN_2X16X32_1(wvecA0, IVP_MOVNX16_FROMN_2X32(x1), hvecSumExpA0);
        hvecSumExpA0 = IVP_PACKVRN_2X64W(wvecA0, 31);
        hvecShift0 = IVP_ADDN_2X32(hvecHeadRoomPlusOne0, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
        hvecSum0 = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift0, hvecOne));
        hvecSumExpA0 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExpA0, hvecSum0), hvecShift0);

        wvecA1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x2), hvecSumExpA1);
        IVP_MULAHN_2X16X32_1(wvecA1, IVP_MOVNX16_FROMN_2X32(x2), hvecSumExpA1);
        hvecSumExpA1 = IVP_PACKVRN_2X64W(wvecA1, 31);
        hvecShift1 = IVP_ADDN_2X32(hvecHeadRoomPlusOne1, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
        hvecSum1 = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift1, hvecOne));
        hvecSumExpA1 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExpA1, hvecSum1), hvecShift1);

        //for 2 and 3
        wvecA2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x3), hvecSumExpA2);
        IVP_MULAHN_2X16X32_1(wvecA2, IVP_MOVNX16_FROMN_2X32(x3), hvecSumExpA2);
        hvecSumExpA2 = IVP_PACKVRN_2X64W(wvecA2, 31);
        hvecShift2 = IVP_ADDN_2X32(hvecHeadRoomPlusOne2, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
        hvecSum2 = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift2, hvecOne));
        hvecSumExpA2 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExpA2, hvecSum2), hvecShift2);

        wvecA3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(x4), hvecSumExpA3);
        IVP_MULAHN_2X16X32_1(wvecA3, IVP_MOVNX16_FROMN_2X32(x4), hvecSumExpA3);
        hvecSumExpA3 = IVP_PACKVRNRN_2X64W(wvecA3, 31);
        hvecShift3 = IVP_ADDN_2X32(hvecHeadRoomPlusOne3, (xb_vecN_2x32v)(31 - OUTPUT_BITS));
        hvecSum3 = IVP_SLAN_2X32(hvecOne, IVP_SUBN_2X32(hvecShift3, hvecOne));
        hvecSumExpA3 = IVP_SRLN_2X32U(IVP_ADDN_2X32U(hvecSumExpA3, hvecSum3), hvecShift3);

        vecRes0 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecSumExpA1, hvecSumExpA0, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));
        vecRes1 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecSumExpA3, hvecSumExpA2, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));

        MORPH_OP_CLAMP(vecRes0, vecRes1, dvecRes);

        xb_vecNx16U vOffset1 = IVP_ADDNX16U(vecOffset1, (xb_vecNx16U)x);
        xb_vecNx16U vOffset2 = IVP_ADDNX16U(vecOffset2, (xb_vecNx16U)x);

        vOffset1 = IVP_MOVNX16UT(vOffset1, (xb_vecNx16U)0, vboolMsk1);
        vOffset2 = IVP_MOVNX16UT(vOffset2, (xb_vecNx16U)0, vboolMsk2);

        MORPH_OP_SCATTER2NX8T_L(dvecRes, pOut, vOffset1, vbMsk);
        MORPH_OP_SCATTER2NX8T_H(dvecRes, pOut, vOffset2, vbMsk);
      }
    }
  }
  return XI_ERROR_STATUS();
}

/**************************** xiSoftmaxA3D_U8_LUT ***********************/
/**************************** xiSoftmaxA3D_S8_LUT ***********************/
/****************************** xiSoftmaxA3D ****************************/
/* Description : Optimized implementation of softmax functionality      */
/* Inputs      : Input tile, softmaxArray softmaxA_params               */
/* Outputs     : XI Error Code                                          */
/* InOuts      : Output Tile                                            */
/* Assumptions : InData, outData is signed/unsigned 8bit                */
/************************************************************************/
//Fast Softmax implementation using pre-generated Lookup table
XI_ERR_TYPE MAKE_NAME2(xiSoftmaxA3D, LUT)(const xi_pTile3D inTile,
                                          const xi_pArray softmaxArray,
                                          xi_pTile3D outTile,
                                          const xi_cnn_softmaxA_params* pparams,
                                          xi_pArray scratchArray)
{
  /* Error Checks */
  if (pparams == NULL)
    return XI_ERR_NULLARG;
  if (inTile == NULL)
    return XI_ERR_NULLARG;

  int32_t axis = XI_CNN_SOFTMAXA_PARAMS_GET_AXIS(pparams);
  XI_ERR_TYPE errType;
  if (axis == 1)
  {
    if (inTile->dim1Edge1 == 0 && inTile->dim1Edge2 == 0 && outTile->dim1Edge1 == 0 && outTile->dim1Edge2 == 0 && inTile->dim1Size <= 16)
    {
      if (inTile->dim1Size == 2)
      {
        errType = MAKE_NAME2(softmaxA3D, DIM1_LUT_DEPTH2)(inTile, softmaxArray, outTile, pparams);
      }
      else
      {
        // uses extra [64*depth] bytes of memory in DRAM
        errType = MAKE_NAME2(softmaxA3D, DIM1_LUT_DEPTH_LESSTHAN16)(inTile, softmaxArray, outTile, pparams, scratchArray);
      }
    }
    else 
    {
      errType = MAKE_NAME2(softmaxA3D, DIM1_LUT)(inTile, softmaxArray, outTile, pparams);
    }
  }
  else if (axis == 2)
  {
    errType = MAKE_NAME2(softmaxA3D, DIM2_LUT)(inTile, softmaxArray, outTile, pparams);
  }
  else if (axis == 3)
  {
    errType = MAKE_NAME2(softmaxA3D, DIM3_LUT)(inTile, softmaxArray, outTile, pparams);
  }
  else
  {
    errType = (XI_ERR_NO_VARIANT); // axis mentioned is wrong
  }
  return errType;
}

/**************************** xiSoftmaxA3D_U8 ***************************/
/**************************** xiSoftmaxA3D_S8 ***************************/
/**************************** xiSoftmaxA3D ******************************/
/* Description : Optimized implementation of softmax functionality      */
/* Inputs      : Input tile and softmaxA_params                         */
/* Outputs     : XI Error Code                                          */
/* InOuts      : Output Tile                                            */
/* Assumptions : InData, outData is signed/unsigned 8bit                */
/************************************************************************/
//Softmax implementation which generates lookup table for given parameters and uses the same for given input tile
XI_ERR_TYPE MAKE_NAME1(xiSoftmaxA3D)(const xi_pTile3D inTile,
                                     const xi_pArray softmaxArray,
                                     xi_pTile3D outTile,
                                     const xi_cnn_softmaxA_params* pparams,
                                     xi_pArray scratchArray)
{
  XI_ERR_TYPE errType = xiSoftmaxA_generateLUT(softmaxArray, pparams);

  if (errType == XI_ERR_OK)
  {
    errType = MAKE_NAME2(xiSoftmaxA3D,LUT)(inTile, softmaxArray, outTile, pparams, scratchArray);
  }
  return errType;
}
#endif //if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))
