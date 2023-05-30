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

#undef MAKE_NAME_IMPL
#undef MAKE_NAME
#undef MORPH_IDT_CHECK
#undef MORPH_IDT_SCALAR
#undef MORPH_IDT_Nx8
#undef MORPH_IDT_Nx16
#undef MORPH_OP_LOAD_NX8_PP
#undef MORPH_OP_ALIGN_LOAD_NX8_XP
#undef MORPH_OP_ALIGN_LOAD_Nx8_IP
#undef MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE
#undef MORPH_OP_ADDNX16
#undef MORPH_OP_FLUSH_SAPOSNX8
#undef MORPH_OP_IVP_SANX8_IP
#undef MORPH_OP_ALIGN_STORE_NX8_VARIABLE

#define MAKE_NAME_IMPL(name, MORPH_FNAME_SPECIFIER_IDT)  name ## _ ## MORPH_FNAME_SPECIFIER_IDT
#define MAKE_NAME(name)                                  MAKE_NAME_IMPL(name, U8)
#define MORPH_IDT_CHECK                     XI_CHECK_TILE3D_U8
#define MORPH_IDT_SCALAR                    uint8_t
#define MORPH_IDT_Nx8                       xb_vecNx8U
#define MORPH_IDT_Nx16                      xb_vecNx16U
#define MORPH_OP_LOAD_NX8_PP                IVP_LANX8U_PP
#define MORPH_OP_ALIGN_LOAD_NX8_XP          IVP_LANX8U_XP
#define MORPH_OP_ALIGN_LOAD_Nx8_IP          IVP_LANX8U_IP
#define MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE    IVP_LAVNX8U_XP
#define MORPH_OP_ADDNX16                    IVP_ADDNX16U
#define MORPH_OP_ALIGN_STORE_NX8_VARIABLE   IVP_SAVNX8U_XP
#define MORPH_OP_IVP_SANX8_IP               IVP_SANX8U_IP
#define MORPH_OP_FLUSH_SAPOSNX8             IVP_SAPOSNX8U_FP

#elif INPUT_DATA_TYPE == SIGNED8BIT

#undef MAKE_NAME_IMPL
#undef MAKE_NAME
#undef MORPH_IDT_CHECK
#undef MORPH_IDT_SCALAR
#undef MORPH_IDT_Nx8
#undef MORPH_IDT_Nx16
#undef MORPH_OP_LOAD_NX8_PP
#undef MORPH_OP_ALIGN_LOAD_NX8_XP
#undef MORPH_OP_ALIGN_LOAD_Nx8_IP
#undef MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE
#undef MORPH_OP_ADDNX16
#undef MORPH_OP_FLUSH_SAPOSNX8
#undef MORPH_OP_IVP_SANX8_IP
#undef MORPH_OP_ALIGN_STORE_NX8_VARIABLE

#define MAKE_NAME_IMPL(name, MORPH_FNAME_SPECIFIER_IDT)  name ## _ ## MORPH_FNAME_SPECIFIER_IDT
#define MAKE_NAME(name)                                  MAKE_NAME_IMPL(name, S8)
#define MORPH_IDT_CHECK                     XI_CHECK_TILE3D_S8
#define MORPH_IDT_SCALAR                    int8_t
#define MORPH_IDT_Nx8                       xb_vecNx8
#define MORPH_IDT_Nx16                      xb_vecNx16
#define MORPH_OP_LOAD_NX8_PP                IVP_LANX8S_PP
#define MORPH_OP_ALIGN_LOAD_NX8_XP          IVP_LANX8S_XP
#define MORPH_OP_ALIGN_LOAD_Nx8_IP          IVP_LANX8S_IP
#define MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE    IVP_LAVNX8S_XP
#define MORPH_OP_ADDNX16                    IVP_ADDSNX16
#define MORPH_OP_ALIGN_STORE_NX8_VARIABLE   IVP_SAVNX8S_XP
#define MORPH_OP_IVP_SANX8_IP               IVP_SANX8S_IP
#define MORPH_OP_FLUSH_SAPOSNX8             IVP_SAPOSNX8S_FP

#endif

#ifdef IVP_MULUSN_2X32
#define ADD32VALUES(vecInp1,vecInp2,vecOut)                                                                                               \
{                                                                                                                                         \
  xb_vecN_2x32v hvecScaledabs1_0,hvecScaledabs1_1, hvecScaledabs2_0, hvecScaledabs2_1, hvecInp1_0, hvecInp1_1;                            \
  xb_vecN_2x32v hvecInp2_0, hvecInp2_1, hvecScaled1_0, hvecScaled1_1, hvecScaled2_0, hvecScaled2_1, hvecOut0;                             \
  xb_vecN_2x32v hvecOut1, hvecSum0, hvecSum1, hvecSumH, hvecSumL, hvecInp1L, hvecInp1H, hvecInp2L, hvecInp2H;                             \
  xb_vecN_2x64w wvec0, wvec1;                                                                                                             \
  xb_vecNx48 wvecTemp0, wvecTemp1;                                                                                                        \
  hvecInp1_0 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0(vecInp1, (xb_vecN_2x32v)input1_multiplier),(31-left_shift));                          \
  hvecInp1_1 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_1(vecInp1, (xb_vecN_2x32v)input1_multiplier),(31-left_shift));                          \
  hvecInp2_0 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0(vecInp2, (xb_vecN_2x32v)input2_multiplier),(31-left_shift));                          \
  hvecInp2_1 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_1(vecInp2, (xb_vecN_2x32v)input2_multiplier),(31-left_shift));                          \
  IVP_DSELN_2X32I(hvecInp1H,hvecInp1L,hvecInp1_1,hvecInp1_0,IVP_DSELI_INTERLEAVE_2);                                                      \
  IVP_DSELN_2X32I(hvecInp2H,hvecInp2L,hvecInp2_1,hvecInp2_0,IVP_DSELI_INTERLEAVE_2);                                                      \
  wvecTemp0        = IVP_CVT48UNX32(IVP_ABSN_2X32(hvecInp1H),IVP_ABSN_2X32(hvecInp1L));                                                   \
  wvecTemp1        = IVP_CVT48UNX32(IVP_ABSN_2X32(hvecInp2H),IVP_ABSN_2X32(hvecInp2L));                                                   \
  hvecScaledabs1_0 = IVP_PACKVRNX48_0(wvecTemp0, input1_shift);                                                                           \
  hvecScaledabs2_0 = IVP_PACKVRNX48_0(wvecTemp1, input2_shift);                                                                           \
  hvecScaled1_0    = IVP_MULSGNN_2X32(hvecInp1_0, hvecScaledabs1_0);                                                                      \
  hvecScaled2_0    = IVP_MULSGNN_2X32(hvecInp2_0, hvecScaledabs2_0);                                                                      \
  hvecScaledabs1_1 = IVP_PACKVRNX48_1(wvecTemp0, input1_shift);                                                                           \
  hvecScaledabs2_1 = IVP_PACKVRNX48_1(wvecTemp1, input2_shift);                                                                           \
  hvecScaled1_1    = IVP_MULSGNN_2X32(hvecInp1_1, hvecScaledabs1_1);                                                                      \
  hvecScaled2_1    = IVP_MULSGNN_2X32(hvecInp2_1, hvecScaledabs2_1);                                                                      \
  hvecSum0         = IVP_ADDN_2X32(hvecScaled1_0, hvecScaled2_0);                                                                         \
  hvecSum1         = IVP_ADDN_2X32(hvecScaled1_1, hvecScaled2_1);                                                                         \
  wvec0            = IVP_MULUSN_2X32((xb_vecN_2x32v)output_multiplier, hvecSum0);                                                         \
  wvec1            = IVP_MULUSN_2X32((xb_vecN_2x32v)output_multiplier, hvecSum1);                                                         \
  hvecSum0         = IVP_PACKVRN_2X64W(wvec0, 31);                                                                                            \
  hvecSum1         = IVP_PACKVRN_2X64W(wvec1, 31);                                                                                        \
  IVP_DSELN_2X32I(hvecSumH,hvecSumL,hvecSum1,hvecSum0,IVP_DSELI_INTERLEAVE_2);                                                            \
  wvecTemp0        = IVP_CVT48UNX32(IVP_ABSN_2X32(hvecSumH),IVP_ABSN_2X32(hvecSumL));                                                     \
  hvecOut0         = IVP_PACKVRNX48_0(wvecTemp0, output_shift);                                                                           \
  hvecOut1         = IVP_PACKVRNX48_1(wvecTemp0, output_shift);                                                                           \
  hvecOut0         = IVP_MULSGNN_2X32(hvecSum0, hvecOut0);                                                                                \
  hvecOut0         = IVP_ADDN_2X32(hvecOut0, (xb_vecN_2x32v)output_offset);                                                               \
  hvecOut1         = IVP_MULSGNN_2X32(hvecSum1, hvecOut1);                                                                                \
  hvecOut1         = IVP_ADDN_2X32(hvecOut1, (xb_vecN_2x32v)output_offset);                                                               \
  hvecOut0         = IVP_MINN_2X32((xb_vecN_2x32v)output_activation_max, IVP_MAXN_2X32(hvecOut0, (xb_vecN_2x32v)output_activation_min));  \
  hvecOut1         = IVP_MINN_2X32((xb_vecN_2x32v)output_activation_max, IVP_MAXN_2X32(hvecOut1, (xb_vecN_2x32v)output_activation_min));  \
  vecOut           = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecOut1,hvecOut0, IVP_SELI_16B_INTERLEAVE_1_EVEN));                           \
}
#else
#define ADD32VALUES(vecInp1,vecInp2,vecOut)                                                                                               \
{                                                                                                                                         \
  xb_vecN_2x32v hvecScaledabs1_0,hvecScaledabs1_1, hvecScaledabs2_0, hvecScaledabs2_1, hvecInp1_0, hvecInp1_1;                            \
  xb_vecN_2x32v hvecInp2_0, hvecInp2_1, hvecScaled1_0, hvecScaled1_1, hvecScaled2_0, hvecScaled2_1, hvecOut0;                             \
  xb_vecN_2x32v hvecOut1,hvecSum0,hvecSum1,hvecSumH,hvecSumL,hvecInp1L,hvecInp1H,hvecInp2L,hvecInp2H;                                     \
  xb_vecN_2x64w wvec0, wvec1;                                                                                                             \
  xb_vecNx48 wvecTemp0, wvecTemp1;                                                                                                        \
  hvecInp1_0 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0(vecInp1, (xb_vecN_2x32v)input1_multiplier),(31-left_shift));                          \
  hvecInp1_1 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_1(vecInp1, (xb_vecN_2x32v)input1_multiplier),(31-left_shift));                          \
  hvecInp2_0 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0(vecInp2, (xb_vecN_2x32v)input2_multiplier),(31-left_shift));                          \
  hvecInp2_1 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_1(vecInp2, (xb_vecN_2x32v)input2_multiplier),(31-left_shift));                          \
  IVP_DSELN_2X32I(hvecInp1H,hvecInp1L,hvecInp1_1,hvecInp1_0,IVP_DSELI_INTERLEAVE_2);                                                      \
  IVP_DSELN_2X32I(hvecInp2H,hvecInp2L,hvecInp2_1,hvecInp2_0,IVP_DSELI_INTERLEAVE_2);                                                      \
  wvecTemp0        = IVP_CVT48UNX32(IVP_ABSN_2X32(hvecInp1H),IVP_ABSN_2X32(hvecInp1L));                                                   \
  wvecTemp1        = IVP_CVT48UNX32(IVP_ABSN_2X32(hvecInp2H),IVP_ABSN_2X32(hvecInp2L));                                                   \
  hvecScaledabs1_0 = IVP_PACKVRNX48_0(wvecTemp0, input1_shift);                                                                           \
  hvecScaledabs2_0 = IVP_PACKVRNX48_0(wvecTemp1, input2_shift);                                                                           \
  hvecScaled1_0    = IVP_MULSGNN_2X32(hvecInp1_0, hvecScaledabs1_0);                                                                      \
  hvecScaled2_0    = IVP_MULSGNN_2X32(hvecInp2_0, hvecScaledabs2_0);                                                                      \
  hvecScaledabs1_1 = IVP_PACKVRNX48_1(wvecTemp0, input1_shift);                                                                           \
  hvecScaledabs2_1 = IVP_PACKVRNX48_1(wvecTemp1, input2_shift);                                                                           \
  hvecScaled1_1    = IVP_MULSGNN_2X32(hvecInp1_1, hvecScaledabs1_1);                                                                      \
  hvecScaled2_1    = IVP_MULSGNN_2X32(hvecInp2_1, hvecScaledabs2_1);                                                                      \
  hvecSum0         = IVP_ADDN_2X32(hvecScaled1_0, hvecScaled2_0);                                                                         \
  hvecSum1         = IVP_ADDN_2X32(hvecScaled1_1, hvecScaled2_1);                                                                         \
  wvec0            = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)output_multiplier), hvecSum0);                            \
  IVP_MULUSAHN_2X16X32_1(wvec0, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)output_multiplier), hvecSum0);                                      \
  wvec1            = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)output_multiplier), hvecSum1);                            \
  IVP_MULUSAHN_2X16X32_1(wvec1, IVP_MOVNX16_FROMN_2X32((xb_vecN_2x32v)output_multiplier), hvecSum1);                                      \
  hvecSum0         = IVP_PACKVRN_2X64W(wvec0, 31);                                                                                        \
  hvecSum1         = IVP_PACKVRN_2X64W(wvec1, 31);                                                                                        \
  IVP_DSELN_2X32I(hvecSumH,hvecSumL,hvecSum1,hvecSum0,IVP_DSELI_INTERLEAVE_2);                                                            \
  wvecTemp0        = IVP_CVT48UNX32(IVP_ABSN_2X32(hvecSumH),IVP_ABSN_2X32(hvecSumL));                                                     \
  hvecOut0         = IVP_PACKVRNX48_0(wvecTemp0, output_shift);                                                                           \
  hvecOut1         = IVP_PACKVRNX48_1(wvecTemp0, output_shift);                                                                           \
  hvecOut0         = IVP_MULSGNN_2X32(hvecSum0, hvecOut0);                                                                                \
  hvecOut0         = IVP_ADDN_2X32(hvecOut0, (xb_vecN_2x32v)output_offset);                                                               \
  hvecOut1         = IVP_MULSGNN_2X32(hvecSum1, hvecOut1);                                                                                \
  hvecOut1         = IVP_ADDN_2X32(hvecOut1,(xb_vecN_2x32v)output_offset);                                                                \
  hvecOut0         = IVP_MINN_2X32((xb_vecN_2x32v)output_activation_max, IVP_MAXN_2X32(hvecOut0, (xb_vecN_2x32v)output_activation_min));  \
  hvecOut1         = IVP_MINN_2X32((xb_vecN_2x32v)output_activation_max, IVP_MAXN_2X32(hvecOut1, (xb_vecN_2x32v)output_activation_min));  \
  vecOut           = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecOut1,hvecOut0, IVP_SELI_16B_INTERLEAVE_1_EVEN));                           \
}
#endif

/************************** addA3DNoQuant_S8 ****************************/
/************************** addA3DNoQuant_U8 ****************************/
/************************** addA3DNoQuant *******************************/
/* Description : Optimized implementation of add functionality          */
/*               Here there is no quantization applied as the input and */
/*               output scales are assumed to be the same               */
/* Inputs      : Input tile and AddA_params                             */
/* Outputs     : XI Error Code                                          */
/* InOuts      : Output Tile                                            */
/* Assumptions : InData, outData is U8/S8                               */
/************************************************************************/
static void MAKE_NAME(addA3DNoQuant) (const xi_pTile3D inTile1,
                                      const xi_pTile3D inTile2,
                                      xi_pTile3D outTile,
                                      const xi_cnn_addA_params *pparams
                                      )
{
  /* Getting parameters from the tile structures */
  const int32_t dim1Size = XI_TILE3D_GET_DIM1(inTile1);
  const int32_t dim2Size = XI_TILE3D_GET_DIM2(inTile1);
  const int32_t dim3Size = XI_TILE3D_GET_DIM3(inTile1);
  const int32_t inData1Pitch1 = XI_TILE3D_GET_DIM1_PITCH(inTile1);
  const int32_t inData1Pitch2 = XI_TILE3D_GET_DIM2_PITCH(inTile1);
  const int32_t inData2Pitch1 = XI_TILE3D_GET_DIM1_PITCH(inTile2);
  const int32_t inData2Pitch2 = XI_TILE3D_GET_DIM2_PITCH(inTile2);
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
  /*Get data pointers */
  MORPH_IDT_SCALAR *pInput1 = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile1);
  MORPH_IDT_SCALAR *pInput2 = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile2);
  MORPH_IDT_SCALAR *pOutput = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(outTile);

  /* Get add struct parameters*/
  int32_t input1_offset = XI_CNN_ADDA_GET_ZERO_POINT_IN1(pparams);
  int32_t input2_offset = XI_CNN_ADDA_GET_ZERO_POINT_IN2(pparams);
  int32_t output_offset = XI_CNN_ADDA_GET_ZERO_POINT_OUT(pparams);
  int32_t output_activation_min = XI_CNN_ADDA_GET_MIN_VAL(pparams);
  int32_t output_activation_max = XI_CNN_ADDA_GET_MAX_VAL(pparams);

  int32_t x, y, z;
  valign va1, va2, vaStore;
  /* Input and output data pointers */
  MORPH_IDT_Nx8 *restrict pvecInp1;
  MORPH_IDT_Nx8 *restrict pvecInp2;
  MORPH_IDT_Nx8 *restrict pvecOut;
  MORPH_IDT_SCALAR    *restrict outPtr;
  MORPH_IDT_Nx16 vecInp2, vecInp1, vecOut;
  MORPH_IDT_Nx16 vecOffset = input1_offset + input2_offset + output_offset;
  int32_t tripcount1, tripcount2;
  vaStore = IVP_ZALIGN();
  int32_t vectorizationWidth = XCHAL_IVPN_SIMD_WIDTH;

  tripcount1 = dim3Size * dim2Size * dim1Size;
  tripcount2 = dim2Size * dim1Size;

  //if the data is contiguous for input1 , input2 and output along all the directions
  if ((dim2Size * inData1Pitch1 == inData1Pitch2) && (dim1Size == inData1Pitch1) && \
    (dim2Size * inData2Pitch1 == inData2Pitch2) && (dim1Size == inData2Pitch1) && \
    (dim2Size * outDataPitch1 == outDataPitch2) && (dim1Size == outDataPitch1))   // data is continuous across all dimensions
  {
    /* Input and Output data pointers */
    pvecInp1 = (MORPH_IDT_Nx8 *)(pInput1);
    va1 = MORPH_OP_LOAD_NX8_PP(pvecInp1);

    pvecInp2 = (MORPH_IDT_Nx8 *)(pInput2);
    va2 = MORPH_OP_LOAD_NX8_PP(pvecInp2);

    pvecOut = (MORPH_IDT_Nx8 *)(pOutput);

    for (x = 0; x <= (tripcount1 - vectorizationWidth); x += vectorizationWidth)
    {
      /* Load Input data */
      MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va1, pvecInp1);
      MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va2, pvecInp2);

      /* Add input data and pre-computed offset */
      MORPH_IDT_Nx16 vecInp = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(vecInp2));
      vecOut = MORPH_OP_ADDNX16(vecInp, vecOffset);
      vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

      /*Store the value */
      MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
    }
    if (x < tripcount1)
    {
      /* Load data */
      MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp1, va1, pvecInp1, (tripcount1 - x));
      MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp2, va2, pvecInp2, (tripcount1 - x));

      /* Add input data and pre-computed offset */
      MORPH_IDT_Nx16 vecInp = MORPH_OP_ADDNX16(vecInp1, vecInp2);
      vecOut = MORPH_OP_ADDNX16(vecInp, vecOffset);
      vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

      /*Store the value */
      MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (tripcount1 - x));
    }
    MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
  }
  //if the data is contiguous for input1 , input2 and output along two outer most dimensions
  else if ((dim1Size == inData1Pitch1) && \
    (dim1Size == outDataPitch1) && \
    (dim1Size == inData2Pitch1))       // data is continuous along dimensions1 and dimensions2
  {
    for (z = 0; z < dim3Size; z++)
    {
      /* Input and Output data pointers */
      pvecInp1 = (MORPH_IDT_Nx8 *)(pInput1 + z * inData1Pitch2);
      va1 = MORPH_OP_LOAD_NX8_PP(pvecInp1);
      pvecInp2 = (MORPH_IDT_Nx8 *)(pInput2 + z * inData2Pitch2);
      va2 = MORPH_OP_LOAD_NX8_PP(pvecInp2);
      pvecOut = (MORPH_IDT_Nx8 *)(pOutput + z * outDataPitch2);

      for (x = 0; x <= (tripcount2 - vectorizationWidth); x += vectorizationWidth)
      {
        /* Load Input data */
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va1, pvecInp1);
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va2, pvecInp2);

        /* Add input data and pre-computed offset */
        MORPH_IDT_Nx16 vecInp = MORPH_OP_ADDNX16(vecInp1, vecInp2);
        vecOut = MORPH_OP_ADDNX16(vecInp, vecOffset);
        vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

        /*Store the value */
        MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
      }
      if (x < tripcount2)
      {
        /* Load Input data */
        MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp1, va1, pvecInp1, (tripcount2 - x));
        MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp2, va2, pvecInp2, (tripcount2 - x));

        /* Add input data and pre-computed offset */
        MORPH_IDT_Nx16 vecInp = MORPH_OP_ADDNX16(vecInp1, vecInp2);
        vecOut = MORPH_OP_ADDNX16(vecInp, vecOffset);
        vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

        /*Store the value */
        MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (tripcount2 - x));
      }
      MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
    }
  }
  else       // data is not continuous along innermost dimension
  {
    for (x = 0; x < dim1Size; x += vectorizationWidth)
    {
      for (z = 0; z < dim3Size; z++)
      {
        /* Input and Output data pointers */
        pvecInp1 = (MORPH_IDT_Nx8 *)(pInput1 + z * inData1Pitch2 + x);
        pvecInp2 = (MORPH_IDT_Nx8 *)(pInput2 + z * inData2Pitch2 + x);

        outPtr = (pOutput + z * outDataPitch2 + x);

        for (y = 0; y < dim2Size; y++)
        {
          /* Load Input data */
          va1 = MORPH_OP_LOAD_NX8_PP(pvecInp1);
          MORPH_OP_ALIGN_LOAD_NX8_XP(vecInp1, va1, pvecInp1, inData1Pitch1);

          va2 = MORPH_OP_LOAD_NX8_PP(pvecInp2);
          MORPH_OP_ALIGN_LOAD_NX8_XP(vecInp2, va2, pvecInp2, inData2Pitch1);

          /* Add input data and pre-computed offset */
          MORPH_IDT_Nx16 vecInp = MORPH_OP_ADDNX16(vecInp1, vecInp2);
          vecOut = MORPH_OP_ADDNX16(vecInp, vecOffset);
          vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

          /* Store the data */
          pvecOut = (MORPH_IDT_Nx8 *)outPtr;
          MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (dim1Size - x));
          MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
          outPtr += outDataPitch1;
        }
      }
    }
  }

  return;
}
/******************************* xiAddA3D_S8 ****************************/
/******************************* xiAddA3D_U8 ****************************/
/******************************* xiAddA3D *******************************/
/* Description : Optimized implementation of add functionality          */
/* Inputs      : Input tile and AddA_params                             */
/* Outputs     : XI Error Code                                          */
/* InOuts      : Output Tile                                            */
/* Assumptions : InData, outData is U8/S8                               */
/************************************************************************/
XI_ERR_TYPE MAKE_NAME(xiAddA3D) (
  const xi_pTile3D inTile1,
  const xi_pTile3D inTile2,
  xi_pTile3D outTile,
  const xi_cnn_addA_params *pparams
  )
{
  XI_ERROR_CHECKS()
  {
    MORPH_IDT_CHECK(inTile1);
    MORPH_IDT_CHECK(inTile2);
    MORPH_IDT_CHECK(outTile);
    XI_CHECK_TILE3D_SIZE_EQ(inTile1, inTile2);
    XI_CHECK_TILE3D_SIZE_EQ(inTile1, outTile);
    XI_CHECK_POINTER(pparams);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile1);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile2);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_ERROR((XI_CNN_ADDA_GET_MIN_VAL(pparams) <= XI_CNN_ADDA_GET_MAX_VAL(pparams)), XI_ERR_BADARG,\
                   "\nmax limit = %d, min limit= %d\nmax limit is less than min limit", XI_CNN_ADDA_GET_MIN_VAL(pparams), XI_CNN_ADDA_GET_MAX_VAL(pparams));
    XI_CHECK_ERROR((XI_TILE3D_GET_DATA_ORDER(inTile1) == XI_TILE3D_GET_DATA_ORDER(outTile)) && (XI_TILE3D_GET_DATA_ORDER(inTile2) == XI_TILE3D_GET_DATA_ORDER(outTile)),
                   XI_ERR_BADARG, "Data Order of InputTiles and OutputTile should be same");
    XI_CHECK_ERROR((XI_CNN_ADDA_GET_SHIFT_IN1(pparams) >= 0), XI_ERR_NORM, \
                   "\nInput1 shift = %d, Input1 shift should be greater than or equal to zero", XI_CNN_ADDA_GET_SHIFT_IN1(pparams));
    XI_CHECK_ERROR((XI_CNN_ADDA_GET_SHIFT_IN1(pparams) < 32), XI_ERR_NORM, \
                   "\nInput1 shift = %d, Input1 shift should be less than 32", XI_CNN_ADDA_GET_SHIFT_IN1(pparams));
    XI_CHECK_ERROR((XI_CNN_ADDA_GET_SHIFT_IN2(pparams) >= 0), XI_ERR_NORM, \
                   "\nInput2 shift = %d, Input2 shift should be greater than or equal to zero", XI_CNN_ADDA_GET_SHIFT_IN2(pparams));
    XI_CHECK_ERROR((XI_CNN_ADDA_GET_SHIFT_IN2(pparams) < 32), XI_ERR_NORM, \
                   "\nInput2 shift = %d, Input2 shift should be less than 32", XI_CNN_ADDA_GET_SHIFT_IN2(pparams));
    XI_CHECK_ERROR((XI_CNN_ADDA_GET_SHIFT_OUT(pparams) >= 0), XI_ERR_NORM, \
                   "\nOutput shift = %d, Output shift should be greater than or equal to zero", XI_CNN_ADDA_GET_SHIFT_OUT(pparams));
    XI_CHECK_ERROR((XI_CNN_ADDA_GET_SHIFT_OUT(pparams) < 32), XI_ERR_NORM, \
                   "\nOutput shift = %d, Output shift should be less than 32", XI_CNN_ADDA_GET_SHIFT_OUT(pparams));
    XI_CHECK_ERROR((XI_CNN_ADDA_GET_LEFT_SHIFT(pparams) >= 0), XI_ERR_NORM, \
                   "\nLeft shift = %d, Left shift should be greater than or equal to zero", XI_CNN_ADDA_GET_LEFT_SHIFT(pparams));
    XI_CHECK_ERROR((XI_CNN_ADDA_GET_LEFT_SHIFT(pparams) < 24), XI_ERR_NORM, \
                   "\nLeft shift = %d, Left shift should be less than 24", XI_CNN_ADDA_GET_LEFT_SHIFT(pparams));
  }

  if (!(XI_CNN_ADDA_GET_QUANTIZATION_FLAG(pparams)))
  {
    MAKE_NAME(addA3DNoQuant)(inTile1, inTile2, outTile, pparams);
    return XI_ERROR_STATUS();
  }

  /* Getting parameters from the tile structures */
  const int32_t dim1Size       = XI_TILE3D_GET_DIM1(inTile1);
  const int32_t dim2Size       = XI_TILE3D_GET_DIM2(inTile1);
  const int32_t dim3Size       = XI_TILE3D_GET_DIM3(inTile1);
  const int32_t inData1Pitch1  = XI_TILE3D_GET_DIM1_PITCH(inTile1);
  const int32_t inData1Pitch2  = XI_TILE3D_GET_DIM2_PITCH(inTile1);
  const int32_t inData2Pitch1  = XI_TILE3D_GET_DIM1_PITCH(inTile2);
  const int32_t inData2Pitch2  = XI_TILE3D_GET_DIM2_PITCH(inTile2);
  const int32_t outDataPitch1  = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2  = XI_TILE3D_GET_DIM2_PITCH(outTile);
  /*Get data pointers */
  MORPH_IDT_SCALAR *pInput1 = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile1);
  MORPH_IDT_SCALAR *pInput2 = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile2);
  MORPH_IDT_SCALAR *pOutput = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(outTile);

  /* Get add struct parameters*/
  int32_t input1_offset         = XI_CNN_ADDA_GET_ZERO_POINT_IN1(pparams);
  int32_t input2_offset         = XI_CNN_ADDA_GET_ZERO_POINT_IN2(pparams);
  int32_t output_offset         = XI_CNN_ADDA_GET_ZERO_POINT_OUT(pparams);
  int32_t input1_multiplier     = XI_CNN_ADDA_GET_MULTIPLIER_IN1(pparams);
  int32_t input2_multiplier     = XI_CNN_ADDA_GET_MULTIPLIER_IN2(pparams);
  int32_t output_multiplier     = XI_CNN_ADDA_GET_MULTIPLIER_OUT(pparams);
  int32_t input1_shift          = XI_CNN_ADDA_GET_SHIFT_IN1(pparams);
  int32_t input2_shift          = XI_CNN_ADDA_GET_SHIFT_IN2(pparams);
  int32_t output_shift          = XI_CNN_ADDA_GET_SHIFT_OUT(pparams);
  int32_t output_activation_min = XI_CNN_ADDA_GET_MIN_VAL(pparams);
  int32_t output_activation_max = XI_CNN_ADDA_GET_MAX_VAL(pparams);
  const int32_t left_shift      = XI_CNN_ADDA_GET_LEFT_SHIFT(pparams);

  int32_t x, y, z;
  valign va1, va2, vaStore;
  /* Input and output data pointers */
  MORPH_IDT_Nx8 *restrict pvecInp1;
  MORPH_IDT_Nx8 *restrict pvecInp2;
  MORPH_IDT_Nx8 *restrict pvecOut;
  MORPH_IDT_SCALAR    *restrict outPtr;
  MORPH_IDT_Nx16 vecInp2, vecInp1, vecOut;

  int32_t tripcount1, tripcount2;
  vaStore = IVP_ZALIGN();
  int32_t vectorizationWidth = XCHAL_IVPN_SIMD_WIDTH;

  tripcount1 = dim3Size * dim2Size * dim1Size;
  tripcount2 = dim2Size * dim1Size;

  //if the data is contiguous for input1 , input2 and output along all the directions
  if ((dim2Size * inData1Pitch1 == inData1Pitch2) && (dim1Size == inData1Pitch1) && \
      (dim2Size * inData2Pitch1 == inData2Pitch2) && (dim1Size == inData2Pitch1) && \
      (dim2Size * outDataPitch1 == outDataPitch2) && (dim1Size == outDataPitch1))   // data is continuous across all dimensions
  {
    /* Input and Output data pointers */
    pvecInp1 = (MORPH_IDT_Nx8 *)(pInput1);
    va1 = MORPH_OP_LOAD_NX8_PP(pvecInp1);

    pvecInp2 = (MORPH_IDT_Nx8 *)(pInput2);
    va2 = MORPH_OP_LOAD_NX8_PP(pvecInp2);

    pvecOut = (MORPH_IDT_Nx8 *)(pOutput);

    for (x = 0; x <= (tripcount1 - vectorizationWidth); x += vectorizationWidth)
    {
      /* Load Input data */
      MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va1, pvecInp1);
      MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va2, pvecInp2);
      /* Zero point addition */
      vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
      vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
      /* Add operation */
      ADD32VALUES(vecInp1, vecInp2, vecOut);
      /*Store the value */
      MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
    }
    if(x < tripcount1)
    {
      /* Load data */
      MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp1, va1, pvecInp1, (tripcount1 - x));
      MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp2, va2, pvecInp2, (tripcount1 - x));
      /* Zero point addition */
      vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
      vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
      /* Add operation */
      ADD32VALUES(vecInp1, vecInp2, vecOut);
      /*Store the value */
      MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (tripcount1 - x));
    }
    MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
  }
  //if the data is contiguous for input1 , input2 and output along two outer most dimensions
  else if ((dim1Size == inData1Pitch1) && \
           (dim1Size == outDataPitch1) && \
           (dim1Size == inData2Pitch1))       // data is continuous along dimensions1 and dimensions2
  {
    for (z = 0; z < dim3Size; z++)
    {
      /* Input and Output data pointers */
      pvecInp1 = (MORPH_IDT_Nx8 *)(pInput1 + z * inData1Pitch2);
      va1 = MORPH_OP_LOAD_NX8_PP(pvecInp1);
      pvecInp2 = (MORPH_IDT_Nx8 *)(pInput2 + z * inData2Pitch2);
      va2 = MORPH_OP_LOAD_NX8_PP(pvecInp2);
      pvecOut = (MORPH_IDT_Nx8 *)(pOutput + z * outDataPitch2);

      for (x = 0; x <= (tripcount2 - vectorizationWidth); x += vectorizationWidth)
      {
        /* Load Input data */
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va1, pvecInp1);
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va2, pvecInp2);
        /* Zero point addition */
        vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
        vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
        /* Add operation */
        ADD32VALUES(vecInp1, vecInp2, vecOut);
        /*Store the value */
        MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
      }
      if(x < tripcount2)
      {
        /* Load Input data */
        MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp1, va1, pvecInp1, (tripcount2 - x));
        MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp2, va2, pvecInp2, (tripcount2 - x));
        /* Zero point addition */
        vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
        vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
        /* Add operation */
        ADD32VALUES(vecInp1, vecInp2, vecOut);
        /*Store the value */
        MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (tripcount2 - x));
      }
      MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
    }
  }
  else       // data is not continuous along innermost dimension
  {
    for (x = 0; x < dim1Size; x+= vectorizationWidth)
    {
      for (z = 0; z < dim3Size; z++)
      {
        /* Input and Output data pointers */
        pvecInp1 = (MORPH_IDT_Nx8 *)(pInput1 + z * inData1Pitch2 + x);
        pvecInp2 = (MORPH_IDT_Nx8 *)(pInput2 + z * inData2Pitch2 + x);

        outPtr   = (pOutput + z * outDataPitch2 + x);

        for (y = 0; y < dim2Size; y++)
        {
          /* Load Input data */
          va1 = MORPH_OP_LOAD_NX8_PP(pvecInp1);
          MORPH_OP_ALIGN_LOAD_NX8_XP(vecInp1, va1, pvecInp1, inData1Pitch1);

          va2 = MORPH_OP_LOAD_NX8_PP(pvecInp2);
          MORPH_OP_ALIGN_LOAD_NX8_XP(vecInp2, va2, pvecInp2, inData2Pitch1);

          /* Zero point addition */
          vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
          vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
          /* Add operation */
          ADD32VALUES(vecInp1, vecInp2, vecOut);
          /* Store the data */
          pvecOut = (MORPH_IDT_Nx8 *)outPtr;
          MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (dim1Size - x));
          MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
          outPtr += outDataPitch1;
        }
      }
    }
  }
  return XI_ERROR_STATUS();
}

#if (XCHAL_VISION_TYPE >= 7)
/******************* broadCastDims1NoQuant_ID16WH *************************/
/* Description : Optimized implementation of Broadcast and add            */
/*               functionality across 1st dimension for ID16WH            */
/*               Here there is no quantization applied as the input and   */
/*               output scales are assumed to be the same                 */
/* Inputs      : Input tile and AddA_params                               */
/* Outputs     : XI Error Code                                            */
/* InOuts      : Output Tile                                              */
/* Assumptions : InData, outData is U8/S8                                 */
/**************************************************************************/
void MAKE_NAME(broadCastDims1NoQuant_ID16WH)(const xi_pTile3D inTile1,
                                             const xi_pTile3D inTile2,
                                             xi_pTile3D outTile,
                                             const xi_cnn_addA_params *pparams,
                                             int32_t inData1Pitch0,
                                             int32_t inData2Pitch0,
                                             int32_t inData1Pitch1,
                                             int32_t inData2Pitch1,
                                             int32_t inData1Pitch2,
                                             int32_t inData2Pitch2)
{
  /* Getting parameters from the tile structures */
  const int32_t  inTile1dim1Size = XI_TILE3D_GET_DIM1(inTile1);
  const int32_t  inTile1dim2Size = XI_TILE3D_GET_DIM2(inTile1);
  const int32_t  inTile2dim1Size = XI_TILE3D_GET_DIM1(inTile2);
  const int32_t  inTile2dim2Size = XI_TILE3D_GET_DIM2(inTile2);
  const int32_t  outTiledim1Size = XI_TILE3D_GET_DIM1(outTile);
  const int32_t  outTiledim2Size = XI_TILE3D_GET_DIM2(outTile);
  const int32_t  outTiledim3Size = XI_TILE3D_GET_DIM3(outTile);
  const int32_t  outDataPitch1   = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t  outDataPitch2   = XI_TILE3D_GET_DIM2_PITCH(outTile);

  /*Get Data Pointers */
  MORPH_IDT_SCALAR *pInput1 = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile1);
  MORPH_IDT_SCALAR *pInput2 = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile2);
  MORPH_IDT_SCALAR *pOutput = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(outTile);

  /* Get add struct parameters*/
  int32_t input1_offset         = XI_CNN_ADDA_GET_ZERO_POINT_IN1(pparams);
  int32_t input2_offset         = XI_CNN_ADDA_GET_ZERO_POINT_IN2(pparams);
  int32_t output_offset         = XI_CNN_ADDA_GET_ZERO_POINT_OUT(pparams);
  int32_t output_activation_min = XI_CNN_ADDA_GET_MIN_VAL(pparams);
  int32_t output_activation_max = XI_CNN_ADDA_GET_MAX_VAL(pparams);

  /* Input and Output data pointers */
  MORPH_IDT_SCALAR *restrict inp1Ptr;
  MORPH_IDT_SCALAR *restrict inp2Ptr;
  MORPH_IDT_SCALAR *restrict outPtr;

  MORPH_IDT_SCALAR *restrict inp1Ptr2;
  MORPH_IDT_SCALAR *restrict inp2Ptr2;
  MORPH_IDT_SCALAR *restrict outPtr2;

  /* Input pointer and data vector */
  MORPH_IDT_Nx8 *restrict pvecInp1;
  MORPH_IDT_Nx8 *restrict pvecInp2;
  MORPH_IDT_Nx8 *restrict pvecInp3;
  MORPH_IDT_Nx8 *restrict pvecInp4;

  MORPH_IDT_Nx16 vecInp2, vecInp1, vecInp;
  MORPH_IDT_Nx16 vecInp3, vecInp4;
  /* Output pointer and data vector */
  MORPH_IDT_Nx8 *restrict pvecOut;
  MORPH_IDT_Nx8 *restrict pvecOut2;
  MORPH_IDT_Nx16 vecOut;
  MORPH_IDT_Nx16 vecOut2;

  valign va1, va2;
  valign va3, va4;
  valign vaStore2 = IVP_ZALIGN();
  valign vaStore  = IVP_ZALIGN();
  int32_t vectorizationWidth = XCHAL_IVPN_SIMD_WIDTH;
  int32_t x, y, z;

  /* Create sequence {0, 32, 1, 33,...15, 47, 0, 32, 1, 33,...15, 47} */
  xb_vec2Nx8 dvecPattern1 = IVP_AND2NX8(IVP_SEQ2NX8(), 15);
  xb_vec2Nx8 dvecPattern2 = IVP_ADD2NX8(dvecPattern1, XCHAL_IVPN_SIMD_WIDTH);
  xb_vec2Nx8 dvecPattern = IVP_SEL2NX8I(dvecPattern2, dvecPattern1, IVP_SELI_8B_INTERLEAVE_1_LO);

  int32_t tripCount  = outTiledim1Size * outTiledim2Size * outTiledim3Size;
  int32_t tripcount1 = outTiledim1Size * outTiledim2Size;
  MORPH_IDT_Nx16 vecOffset = input1_offset + input2_offset + output_offset;

  //input2 value broadcast case
  if (inData2Pitch0 == 0)
  {
    /* if the data is contiguous among all the directions for input1 , input2 and output
     * A special case wherein inTile1 and intile2 will have a depth of 16 is handled with
     * inData2Pitch1 == 16) || (inData2Pitch1 == 0)
     */
    if (((inData2Pitch1 == 16) || (inData2Pitch1 == 0)) && (inData2Pitch2 == 0) && (inTile2dim2Size == 1) && \
         (inTile1dim2Size * inData1Pitch1 == inData1Pitch2) && (inTile1dim1Size == inData1Pitch1)         && \
         (outTiledim2Size * outDataPitch1 == outDataPitch2) && (outTiledim1Size == outDataPitch1))
    {
      /* Input and Output data pointers */
      inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1);
      inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2);
      outPtr  = (MORPH_IDT_SCALAR *)(pOutput);
      /* vector and pointer to load and store values*/
      pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
      va1      = MORPH_OP_LOAD_NX8_PP(pvecInp1);

      /*Load Input 2*/
      pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr);
      va2      = MORPH_OP_LOAD_NX8_PP(pvecInp2);
      MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp, va2, pvecInp2);
      vecInp2 = IVP_SELNX16(vecInp, vecInp, IVP_ANDNX16(IVP_SEQNX16(), 15));
      /* Add input data and pre-computed offset */
      vecInp2  = MORPH_OP_ADDNX16(vecInp2, vecOffset);
      /* Output data pointer */
      pvecOut  = (MORPH_IDT_Nx8 *)(outPtr);

      for (x = 0; x <= (tripCount - vectorizationWidth); x += vectorizationWidth)
      {
        /* Here as the second input is broadcasted, it is loaded outside the innermost
           loop and and repeated and compared with first input*/
        /*Load Input 1*/
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va1, pvecInp1);

        /* Add operation */
        vecOut = MORPH_OP_ADDNX16(vecInp1, vecInp2);

        vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

        /* Store the data */
        MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
      }
      if(x < tripCount)
      {
        /*Load Input 1*/
        MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp1, va1, pvecInp1, (tripCount - x));

        vecOut = MORPH_OP_ADDNX16(vecInp1, vecInp2);
        vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

        /* Store the data */
        MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (tripCount - x));
      }
      MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
    }
    else if(((inData2Pitch1 == 16) || (inData2Pitch1 == 0)) && (inTile2dim2Size == 1) && \
             (inTile1dim1Size == inData1Pitch1) && \
             (outTiledim1Size == outDataPitch1))
    {
      // if the data is contiguous with respect to first dimension for input1, input2 and output
      for (z = 0; z < outTiledim3Size; z += 2)
      {
        int32_t remZ = XT_MIN(2, (outTiledim3Size - z)) - 1;
        /* Input and Output data pointers */
        inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1 + z * inData1Pitch2);
        inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2 + z * inData2Pitch2);
        outPtr  = (MORPH_IDT_SCALAR *)(pOutput + z * outDataPitch2);

        inp1Ptr2 = (MORPH_IDT_SCALAR *)(pInput1 + (z + remZ) * inData1Pitch2);
        inp2Ptr2 = (MORPH_IDT_SCALAR *)(pInput2 + (z + remZ) * inData2Pitch2);
        outPtr2  = (MORPH_IDT_SCALAR *)(pOutput + (z + remZ) * outDataPitch2);

        /* vector and pointer to load and store values*/
        pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
        va1      = MORPH_OP_LOAD_NX8_PP(pvecInp1);

        pvecInp3 = (MORPH_IDT_Nx8 *)(inp1Ptr2);
        va3      = MORPH_OP_LOAD_NX8_PP(pvecInp3);

        /*Load Input 2*/
        pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr);
        va2      = MORPH_OP_LOAD_NX8_PP(pvecInp2);
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp, va2, pvecInp2);

        pvecInp4 = (MORPH_IDT_Nx8 *)(inp2Ptr2);
        va4      = MORPH_OP_LOAD_NX8_PP(pvecInp4);
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va4, pvecInp4);

        IVP_DSELNX16(vecInp4, vecInp2, vecInp1, vecInp, dvecPattern);

        /*Zero point addition*/
        vecInp2 = MORPH_OP_ADDNX16(vecInp2, vecOffset);
        vecInp4 = MORPH_OP_ADDNX16(vecInp4, vecOffset);
        /* Output data pointer */
        pvecOut  = (MORPH_IDT_Nx8 *)(outPtr);
        pvecOut2 = (MORPH_IDT_Nx8 *)(outPtr2);

        for (x = 0; x <= (tripcount1 - XCHAL_IVPN_SIMD_WIDTH); x += XCHAL_IVPN_SIMD_WIDTH)
        {
          /* Load Input 1 */
          MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va1, pvecInp1);
          MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp3, va3, pvecInp3);

          vecOut = MORPH_OP_ADDNX16(vecInp1, vecInp2);
          vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

          vecOut2 = MORPH_OP_ADDNX16(vecInp3, vecInp4);
          vecOut2 = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut2, (xb_vecNx16)output_activation_min));

          /* Store the data */
          MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
          MORPH_OP_IVP_SANX8_IP(vecOut2, vaStore2, pvecOut2);
        }
        if(x < tripcount1)
        {
          /* Load Input 1 */
          MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp1, va1, pvecInp1, (tripcount1 - x));
          MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp3, va3, pvecInp3, (tripcount1 - x));
          /* Add operation */
          vecOut = MORPH_OP_ADDNX16(vecInp1, vecInp2);
          vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

          /* Add operation */
          vecOut2 = MORPH_OP_ADDNX16(vecInp4, vecInp3);
          vecOut2 = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut2, (xb_vecNx16)output_activation_min));

          /* Store the data */
          MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (tripcount1 - x));
          MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut2, vaStore2, pvecOut2, (tripcount1 - x) * remZ);
        }
        MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
        MORPH_OP_FLUSH_SAPOSNX8(vaStore2, pvecOut2);
      }
    }
    else
    { // if the data is not contiguous along inner most dimension
      for (z = 0; z < outTiledim3Size; z += 2)
      {
        int32_t remZ = XT_MIN(2, outTiledim3Size - z) - 1;
        for (y = 0; y < outTiledim2Size; y++)
        {
          /* Input and Output data pointers */
          inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1 + z * inData1Pitch2 + y * inData1Pitch1);
          inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2 + z * inData2Pitch2 + y * inData2Pitch1);
          outPtr  = (MORPH_IDT_SCALAR *)(pOutput + z * outDataPitch2 + y * outDataPitch1);

          inp1Ptr2 = (MORPH_IDT_SCALAR *)(pInput1 + (z + remZ) * inData1Pitch2 + y * inData1Pitch1);
          inp2Ptr2 = (MORPH_IDT_SCALAR *)(pInput2 + (z + remZ) * inData2Pitch2 + y * inData2Pitch1);
          outPtr2  = (MORPH_IDT_SCALAR *)(pOutput + (z + remZ) * outDataPitch2 + y * outDataPitch1);

          /* vector and pointer to load input1*/
          pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
          va1      = MORPH_OP_LOAD_NX8_PP(pvecInp1);

          pvecInp3 = (MORPH_IDT_Nx8 *)(inp1Ptr2);
          va3      = MORPH_OP_LOAD_NX8_PP(pvecInp3);

          /*Load Input 2*/
          pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr);
          va2      = MORPH_OP_LOAD_NX8_PP(pvecInp2);
          MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp, va2, pvecInp2);

          pvecInp4 = (MORPH_IDT_Nx8 *)(inp2Ptr2);
          va4      = MORPH_OP_LOAD_NX8_PP(pvecInp4);
          MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va4, pvecInp4);

          IVP_DSELNX16(vecInp4, vecInp2, vecInp1, vecInp, dvecPattern);

          /* Add input data and pre-computed offset */
          vecInp2 = MORPH_OP_ADDNX16(vecInp2, vecOffset);
          vecInp4 = MORPH_OP_ADDNX16(vecInp4, vecOffset);
          /* Output data pointer */
          pvecOut  = (MORPH_IDT_Nx8 *)(outPtr);
          pvecOut2 = (MORPH_IDT_Nx8 *)(outPtr2);

          for (x = 0; x <= (outTiledim1Size - vectorizationWidth); x += vectorizationWidth)
          {
            /* Here as the second input is broadcasted, it is loaded outside the innermost
               loop and and repeated and compared with first input*/
            /*Load Input 1*/
            MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va1, pvecInp1);
            MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp3, va3, pvecInp3);

            /* Add operation */
            vecOut = MORPH_OP_ADDNX16(vecInp1, vecInp2);
            vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));
            /* Add operation */
            vecOut2 = MORPH_OP_ADDNX16(vecInp4, vecInp3);
            vecOut2 = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut2, (xb_vecNx16)output_activation_min));
            /* Store the data */
            MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
            MORPH_OP_IVP_SANX8_IP(vecOut2, vaStore2, pvecOut2);
          }
          if(x < outTiledim1Size)
          {
            /*Load Input 1*/
            MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp1, va1, pvecInp1, (outTiledim1Size - x));
            MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp3, va3, pvecInp3, (outTiledim1Size - x));

            /* Add operation */
            vecOut = MORPH_OP_ADDNX16(vecInp1, vecInp2);
            vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));
            /* Add operation */
            vecOut2 = MORPH_OP_ADDNX16(vecInp3, vecInp4);
            vecOut2 = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut2, (xb_vecNx16)output_activation_min));

            /* Store the data */
            MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (outTiledim1Size - x));
            MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut2, vaStore2, pvecOut2, (outTiledim1Size - x) * remZ);
          }
          MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
          MORPH_OP_FLUSH_SAPOSNX8(vaStore2, pvecOut2);
        }
      }
    }
  }
  //input1 value broadcast case
  else if (inData1Pitch0 == 0)
  {
    /* if the data is contiguous among all the directions for input1 , input2 and output
     * A special case wherein inTile1 and intile2 will have a depth of 16 is handled with
     * (inData1Pitch1 == 16) || (inData1Pitch1 == 0)
     */
    if (((inData1Pitch1 == 16) || (inData1Pitch1 == 0)) && (inData1Pitch2 == 0) && (inTile1dim2Size == 1) && \
         (inTile2dim2Size * inData2Pitch1 == inData2Pitch2) && (inTile2dim1Size == inData2Pitch1) && \
         (outTiledim2Size * outDataPitch1 == outDataPitch2) && (outTiledim1Size == outDataPitch1))
    {
      /* Input and Output data pointers */
      inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1);
      inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2);
      outPtr = (MORPH_IDT_SCALAR *)(pOutput);
      /* vector and pointer to load and store values*/
      pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr);
      va2      = MORPH_OP_LOAD_NX8_PP(pvecInp2);

      /*Load Input 1*/
      pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
      va1      = MORPH_OP_LOAD_NX8_PP(pvecInp1);
      MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp, va1, pvecInp1);
      vecInp1 = IVP_SELNX16(vecInp, vecInp, IVP_ANDNX16(IVP_SEQNX16(), 15));
      /*Zero point addition*/
      vecInp1  = MORPH_OP_ADDNX16(vecInp1, vecOffset);
      /* Output data pointer */
      pvecOut  = (MORPH_IDT_Nx8 *)(outPtr);

      for (x = 0; x <= (tripCount - vectorizationWidth); x += vectorizationWidth)
      {
        /* Here as the second input is broadcasted, it is loaded outside the innermost
           loop and and repeated and compared with first input*/
        /*Load Input 2*/
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va2, pvecInp2);

        /* Add operation */
        vecOut = MORPH_OP_ADDNX16(vecInp1, vecInp2);
        vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

        /* Store the data */
        MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
      }
      if(x < tripCount)
      {
        /*Load Input 2*/
        MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp2, va2, pvecInp2, (tripCount - x));

        /* Add operation */
        vecOut = MORPH_OP_ADDNX16(vecInp1, vecInp2);
        vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));
        /* Store the data */
        MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (tripCount - x));
      }
      MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
    }
    else if(((inData1Pitch1 == 16) || (inData1Pitch1 == 0)) && (inTile1dim2Size == 1) && \
             (inTile2dim1Size == inData2Pitch1) && \
             (outTiledim1Size == outDataPitch1))
    {
      // if the data is contiguous with respect to first dimension for input1, input2 and output
      for (z = 0; z < outTiledim3Size; z += 2)
      {
        int32_t remZ = XT_MIN(2, (outTiledim3Size - z)) - 1;
        /* Input and Output data pointers */
        inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1 + z * inData1Pitch2);
        inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2 + z * inData2Pitch2);
        outPtr  = (MORPH_IDT_SCALAR *)(pOutput + z * outDataPitch2);

        inp1Ptr2 = (MORPH_IDT_SCALAR *)(pInput1 + (z + remZ) * inData1Pitch2);
        inp2Ptr2 = (MORPH_IDT_SCALAR *)(pInput2 + (z + remZ) * inData2Pitch2);
        outPtr2  = (MORPH_IDT_SCALAR *)(pOutput + (z + remZ) * outDataPitch2);

        /* vector and pointer to load and store values*/
        pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr);
        va2      = MORPH_OP_LOAD_NX8_PP(pvecInp2);

        pvecInp4 = (MORPH_IDT_Nx8 *)(inp2Ptr2);
        va4      = MORPH_OP_LOAD_NX8_PP(pvecInp4);

        /*Load Input 1*/
        pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
        va1      = MORPH_OP_LOAD_NX8_PP(pvecInp1);
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp, va1, pvecInp1);

        pvecInp3 = (MORPH_IDT_Nx8 *)(inp1Ptr2);
        va3      = MORPH_OP_LOAD_NX8_PP(pvecInp3);
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va3, pvecInp3);

        IVP_DSELNX16(vecInp3, vecInp1, vecInp2, vecInp, dvecPattern);

        /*Zero point addition*/
        vecInp1 = MORPH_OP_ADDNX16(vecInp1, vecOffset);
        vecInp3 = MORPH_OP_ADDNX16(vecInp3, vecOffset);
        /* Output data pointer */
        pvecOut  = (MORPH_IDT_Nx8 *)(outPtr);
        pvecOut2 = (MORPH_IDT_Nx8 *)(outPtr2);

        for (x = 0; x <= (tripcount1 - XCHAL_IVPN_SIMD_WIDTH); x += XCHAL_IVPN_SIMD_WIDTH)
        {
          /* Load Input 2 */
          MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va2, pvecInp2);
          MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp4, va4, pvecInp4);

          /* Add operation */
          vecOut = MORPH_OP_ADDNX16(vecInp1, vecInp2);
          vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

          /* Add operation */
          vecOut2 = MORPH_OP_ADDNX16(vecInp3, vecInp4);
          vecOut2 = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut2, (xb_vecNx16)output_activation_min));
          /* Store the data */
          MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
          MORPH_OP_IVP_SANX8_IP(vecOut2, vaStore2, pvecOut2);
        }
        if(x < tripcount1)
        {
          /* Load Input 2 */
          MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp2, va2, pvecInp2, (tripcount1 - x));
          MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp4, va4, pvecInp4, (tripcount1 - x));

          /* Add operation */
          vecOut = MORPH_OP_ADDNX16(vecInp1, vecInp2);
          vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

          /* Add operation */
          vecOut2 = MORPH_OP_ADDNX16(vecInp3, vecInp4);
          vecOut2 = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut2, (xb_vecNx16)output_activation_min));
          /* Store the data */
          MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (tripcount1 - x));
          MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut2, vaStore2, pvecOut2, (tripcount1 - x) * remZ);
        }
        MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
        MORPH_OP_FLUSH_SAPOSNX8(vaStore2, pvecOut2);
      }
    }
    else
    { // if the data is not contiguous along inner most dimension
      for (z = 0; z < outTiledim3Size; z += 2)
      {
        int32_t remZ = XT_MIN(2, outTiledim3Size - z) - 1;
        for (y = 0; y < outTiledim2Size; y++)
        {
          /* Input and Output data pointers */
          inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1 + z * inData1Pitch2 + y * inData1Pitch1);
          inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2 + z * inData2Pitch2 + y * inData2Pitch1);
          outPtr = (MORPH_IDT_SCALAR *)(pOutput + z * outDataPitch2 + y * outDataPitch1);

          inp1Ptr2 = (MORPH_IDT_SCALAR *)(pInput1 + (z + remZ) * inData1Pitch2 + y * inData1Pitch1);
          inp2Ptr2 = (MORPH_IDT_SCALAR *)(pInput2 + (z + remZ) * inData2Pitch2 + y * inData2Pitch1);
          outPtr2 = (MORPH_IDT_SCALAR *)(pOutput + (z + remZ) * outDataPitch2 + y * outDataPitch1);

          /* vector and pointer to load input1*/
          pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr);
          va2 = MORPH_OP_LOAD_NX8_PP(pvecInp2);

          pvecInp4 = (MORPH_IDT_Nx8 *)(inp2Ptr2);
          va4 = MORPH_OP_LOAD_NX8_PP(pvecInp4);

          /*Load Input 1*/
          pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
          va1 = MORPH_OP_LOAD_NX8_PP(pvecInp1);
          MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp, va1, pvecInp1);

          pvecInp3 = (MORPH_IDT_Nx8 *)(inp1Ptr2);
          va3 = MORPH_OP_LOAD_NX8_PP(pvecInp3);
          MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va3, pvecInp3);

          IVP_DSELNX16(vecInp3, vecInp1, vecInp2, vecInp, dvecPattern);
          /*Zero point addition*/
          vecInp1 = MORPH_OP_ADDNX16(vecInp1, vecOffset);
          vecInp3 = MORPH_OP_ADDNX16(vecInp3, vecOffset);
          /* Output data pointer */
          pvecOut = (MORPH_IDT_Nx8 *)(outPtr);
          pvecOut2 = (MORPH_IDT_Nx8 *)(outPtr2);

          for (x = 0; x <= (outTiledim1Size - vectorizationWidth); x += vectorizationWidth)
          {
            /* Here as the second input is broadcasted, it is loaded outside the innermost
               loop and and repeated and compared with first input*/
            /*Load Input 2*/
            MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va2, pvecInp2);
            MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp4, va4, pvecInp4);

            /* Add operation */
            vecOut = MORPH_OP_ADDNX16(vecInp1, vecInp2);
            vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

            /* Add operation */
            vecOut2 = MORPH_OP_ADDNX16(vecInp3, vecInp4);
            vecOut2 = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut2, (xb_vecNx16)output_activation_min));
            /* Store the data */
            MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
            MORPH_OP_IVP_SANX8_IP(vecOut2, vaStore2, pvecOut2);
          }
          if(x < outTiledim1Size)
          {
            /*Load Input 1*/
            MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp2, va2, pvecInp2, (outTiledim1Size - x));
            MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp4, va4, pvecInp4, (outTiledim1Size - x));

            /* Add operation */
            vecOut = MORPH_OP_ADDNX16(vecInp1, vecInp2);
            vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

            /* Add operation */
            vecOut2 = MORPH_OP_ADDNX16(vecInp3, vecInp4);
            vecOut2 = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut2, (xb_vecNx16)output_activation_min));

            /* Store the data */
            MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (outTiledim1Size - x));
            MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut2, vaStore2, pvecOut2, (outTiledim1Size - x) * remZ);
          }
          MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
          MORPH_OP_FLUSH_SAPOSNX8(vaStore2, pvecOut2);
        }
      }
    }
  }
}

/******************* broadCastDims1_ID16WH ********************************/
/* Description : Optimized implementation of Broadcast and add            */
/*               functionality across 1st dimension for ID16WH            */
/* Inputs      : Input tile and AddA_params                               */
/* Outputs     : XI Error Code                                            */
/* InOuts      : Output Tile                                              */
/* Assumptions : InData, outData is U8/S8                                 */
/**************************************************************************/
void MAKE_NAME(broadCastDims1_ID16WH)(const xi_pTile3D inTile1,
                                      const xi_pTile3D inTile2,
                                      xi_pTile3D outTile,
                                      const xi_cnn_addA_params *pparams,
                                      int32_t inData1Pitch0,
                                      int32_t inData2Pitch0,
                                      int32_t inData1Pitch1,
                                      int32_t inData2Pitch1,
                                      int32_t inData1Pitch2,
                                      int32_t inData2Pitch2)
{
  /* Getting parameters from the tile structures */
  const int32_t  inTile1dim1Size = XI_TILE3D_GET_DIM1(inTile1);
  const int32_t  inTile1dim2Size = XI_TILE3D_GET_DIM2(inTile1);
  const int32_t  inTile2dim1Size = XI_TILE3D_GET_DIM1(inTile2);
  const int32_t  inTile2dim2Size = XI_TILE3D_GET_DIM2(inTile2);
  const int32_t  outTiledim1Size = XI_TILE3D_GET_DIM1(outTile);
  const int32_t  outTiledim2Size = XI_TILE3D_GET_DIM2(outTile);
  const int32_t  outTiledim3Size = XI_TILE3D_GET_DIM3(outTile);
  const int32_t  outDataPitch1   = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t  outDataPitch2   = XI_TILE3D_GET_DIM2_PITCH(outTile);

  /*Get Data Pointers */
  MORPH_IDT_SCALAR *pInput1 = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile1);
  MORPH_IDT_SCALAR *pInput2 = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile2);
  MORPH_IDT_SCALAR *pOutput = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(outTile);

  /* Get add struct parameters*/
  int32_t input1_offset         = XI_CNN_ADDA_GET_ZERO_POINT_IN1(pparams);
  int32_t input2_offset         = XI_CNN_ADDA_GET_ZERO_POINT_IN2(pparams);
  int32_t output_offset         = XI_CNN_ADDA_GET_ZERO_POINT_OUT(pparams);
  int32_t input1_multiplier     = XI_CNN_ADDA_GET_MULTIPLIER_IN1(pparams);
  int32_t input2_multiplier     = XI_CNN_ADDA_GET_MULTIPLIER_IN2(pparams);
  int32_t output_multiplier     = XI_CNN_ADDA_GET_MULTIPLIER_OUT(pparams);
  int32_t input1_shift          = XI_CNN_ADDA_GET_SHIFT_IN1(pparams);
  int32_t input2_shift          = XI_CNN_ADDA_GET_SHIFT_IN2(pparams);
  int32_t output_shift          = XI_CNN_ADDA_GET_SHIFT_OUT(pparams);
  int32_t output_activation_min = XI_CNN_ADDA_GET_MIN_VAL(pparams);
  int32_t output_activation_max = XI_CNN_ADDA_GET_MAX_VAL(pparams);
  const int32_t left_shift      = XI_CNN_ADDA_GET_LEFT_SHIFT(pparams);

  /* Input and Output data pointers */
  MORPH_IDT_SCALAR *restrict inp1Ptr;
  MORPH_IDT_SCALAR *restrict inp2Ptr;
  MORPH_IDT_SCALAR *restrict outPtr;

  MORPH_IDT_SCALAR *restrict inp1Ptr2;
  MORPH_IDT_SCALAR *restrict inp2Ptr2;
  MORPH_IDT_SCALAR *restrict outPtr2;

  /* Input pointer and data vector */
  MORPH_IDT_Nx8 *restrict pvecInp1;
  MORPH_IDT_Nx8 *restrict pvecInp2;
  MORPH_IDT_Nx8 *restrict pvecInp3;
  MORPH_IDT_Nx8 *restrict pvecInp4;

  MORPH_IDT_Nx16 vecInp2, vecInp1, vecInp;
  MORPH_IDT_Nx16 vecInp3, vecInp4;
  /* Output pointer and data vector */
  MORPH_IDT_Nx8 *restrict pvecOut;
  MORPH_IDT_Nx8 *restrict pvecOut2;
  MORPH_IDT_Nx16 vecOut;
  MORPH_IDT_Nx16 vecOut2;

  valign va1, va2;
  valign va3, va4;
  valign vaStore2 = IVP_ZALIGN();
  valign vaStore  = IVP_ZALIGN();
  int32_t vectorizationWidth = XCHAL_IVPN_SIMD_WIDTH;
  int32_t x, y, z;

  /* Create sequence {0, 32, 1, 33,...15, 47, 0, 32, 1, 33,...15, 47} */
  xb_vec2Nx8 dvecPattern1 = IVP_AND2NX8(IVP_SEQ2NX8(), 15);
  xb_vec2Nx8 dvecPattern2 = IVP_ADD2NX8(dvecPattern1, XCHAL_IVPN_SIMD_WIDTH);
  xb_vec2Nx8 dvecPattern = IVP_SEL2NX8I(dvecPattern2, dvecPattern1, IVP_SELI_8B_INTERLEAVE_1_LO);

  int32_t tripCount  = outTiledim1Size * outTiledim2Size * outTiledim3Size;
  int32_t tripcount1 = outTiledim1Size * outTiledim2Size;

  //input2 value broadcast case
  if (inData2Pitch0 == 0)
  {
    /* if the data is contiguous among all the directions for input1 , input2 and output
     * A special case wherein inTile1 and intile2 will have a depth of 16 is handled with
     * inData2Pitch1 == 16) || (inData2Pitch1 == 0)
     */
    if (((inData2Pitch1 == 16) || (inData2Pitch1 == 0)) && (inData2Pitch2 == 0) && (inTile2dim2Size == 1) && \
         (inTile1dim2Size * inData1Pitch1 == inData1Pitch2) && (inTile1dim1Size == inData1Pitch1)         && \
         (outTiledim2Size * outDataPitch1 == outDataPitch2) && (outTiledim1Size == outDataPitch1))
    {
      /* Input and Output data pointers */
      inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1);
      inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2);
      outPtr  = (MORPH_IDT_SCALAR *)(pOutput);
      /* vector and pointer to load and store values*/
      pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
      va1      = MORPH_OP_LOAD_NX8_PP(pvecInp1);

      /*Load Input 2*/
      pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr);
      va2      = MORPH_OP_LOAD_NX8_PP(pvecInp2);
      MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp, va2, pvecInp2);
      vecInp2 = IVP_SELNX16(vecInp, vecInp, IVP_ANDNX16(IVP_SEQNX16(), 15));
      /*Zero point addition*/
      vecInp2  = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
      /* Output data pointer */
      pvecOut  = (MORPH_IDT_Nx8 *)(outPtr);

      for (x = 0; x <= (tripCount - vectorizationWidth); x += vectorizationWidth)
      {
        /* Here as the second input is broadcasted, it is loaded outside the innermost
           loop and and repeated and compared with first input*/
        /*Load Input 1*/
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va1, pvecInp1);
        /*Zero point addition*/
        vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
        /* Add operation */
        ADD32VALUES(vecInp1, vecInp2, vecOut);
        /* Store the data */
        MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
      }
      if(x < tripCount)
      {
        /*Load Input 1*/
        MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp1, va1, pvecInp1, (tripCount - x));
        /*Zero point addition*/
        vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
        /* Add operation */
        ADD32VALUES(vecInp1, vecInp2, vecOut);
        /* Store the data */
        MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (tripCount - x));
      }
      MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
    }
    else if(((inData2Pitch1 == 16) || (inData2Pitch1 == 0)) && (inTile2dim2Size == 1) && \
             (inTile1dim1Size == inData1Pitch1) && \
             (outTiledim1Size == outDataPitch1))
    {
      // if the data is contiguous with respect to first dimension for input1, input2 and output
      for (z = 0; z < outTiledim3Size; z += 2)
      {
        int32_t remZ = XT_MIN(2, (outTiledim3Size - z)) - 1;
        /* Input and Output data pointers */
        inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1 + z * inData1Pitch2);
        inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2 + z * inData2Pitch2);
        outPtr  = (MORPH_IDT_SCALAR *)(pOutput + z * outDataPitch2);

        inp1Ptr2 = (MORPH_IDT_SCALAR *)(pInput1 + (z + remZ) * inData1Pitch2);
        inp2Ptr2 = (MORPH_IDT_SCALAR *)(pInput2 + (z + remZ) * inData2Pitch2);
        outPtr2  = (MORPH_IDT_SCALAR *)(pOutput + (z + remZ) * outDataPitch2);

        /* vector and pointer to load and store values*/
        pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
        va1      = MORPH_OP_LOAD_NX8_PP(pvecInp1);

        pvecInp3 = (MORPH_IDT_Nx8 *)(inp1Ptr2);
        va3      = MORPH_OP_LOAD_NX8_PP(pvecInp3);

        /*Load Input 2*/
        pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr);
        va2      = MORPH_OP_LOAD_NX8_PP(pvecInp2);
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp, va2, pvecInp2);

        pvecInp4 = (MORPH_IDT_Nx8 *)(inp2Ptr2);
        va4      = MORPH_OP_LOAD_NX8_PP(pvecInp4);
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va4, pvecInp4);

        IVP_DSELNX16(vecInp4, vecInp2, vecInp1, vecInp, dvecPattern);

        /*Zero point addition*/
        vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
        vecInp4 = MORPH_OP_ADDNX16(vecInp4, (MORPH_IDT_Nx16)(input2_offset));
        /* Output data pointer */
        pvecOut  = (MORPH_IDT_Nx8 *)(outPtr);
        pvecOut2 = (MORPH_IDT_Nx8 *)(outPtr2);

        for (x = 0; x <= (tripcount1 - XCHAL_IVPN_SIMD_WIDTH); x += XCHAL_IVPN_SIMD_WIDTH)
        {
          /* Load Input 1 */
          MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va1, pvecInp1);
          MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp3, va3, pvecInp3);
          /*Zero point addition*/
          vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
          vecInp3 = MORPH_OP_ADDNX16(vecInp3, (MORPH_IDT_Nx16)(input1_offset));
          /* Add operation */
          ADD32VALUES(vecInp1, vecInp2, vecOut);
          ADD32VALUES(vecInp3, vecInp4, vecOut2);
          /* Store the data */
          MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
          MORPH_OP_IVP_SANX8_IP(vecOut2, vaStore2, pvecOut2);
        }
        if(x < tripcount1)
        {
          /* Load Input 1 */
          MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp1, va1, pvecInp1, (tripcount1 - x));
          MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp3, va3, pvecInp3, (tripcount1 - x));
          /*Zero point addition*/
          vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
          vecInp3 = MORPH_OP_ADDNX16(vecInp3, (MORPH_IDT_Nx16)(input1_offset));
          /* Add operation */
          ADD32VALUES(vecInp1, vecInp2, vecOut);
          ADD32VALUES(vecInp3, vecInp4, vecOut2);
          /* Store the data */
          MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (tripcount1 - x));
          MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut2, vaStore2, pvecOut2, (tripcount1 - x) * remZ);
        }
        MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
        MORPH_OP_FLUSH_SAPOSNX8(vaStore2, pvecOut2);
      }
    }
    else
    { // if the data is not contiguous along inner most dimension
      for (z = 0; z < outTiledim3Size; z += 2)
      {
        int32_t remZ = XT_MIN(2, outTiledim3Size - z) - 1;
        for (y = 0; y < outTiledim2Size; y++)
        {
          /* Input and Output data pointers */
          inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1 + z * inData1Pitch2 + y * inData1Pitch1);
          inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2 + z * inData2Pitch2 + y * inData2Pitch1);
          outPtr  = (MORPH_IDT_SCALAR *)(pOutput + z * outDataPitch2 + y * outDataPitch1);

          inp1Ptr2 = (MORPH_IDT_SCALAR *)(pInput1 + (z + remZ) * inData1Pitch2 + y * inData1Pitch1);
          inp2Ptr2 = (MORPH_IDT_SCALAR *)(pInput2 + (z + remZ) * inData2Pitch2 + y * inData2Pitch1);
          outPtr2  = (MORPH_IDT_SCALAR *)(pOutput + (z + remZ) * outDataPitch2 + y * outDataPitch1);

          /* vector and pointer to load input1*/
          pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
          va1      = MORPH_OP_LOAD_NX8_PP(pvecInp1);

          pvecInp3 = (MORPH_IDT_Nx8 *)(inp1Ptr2);
          va3      = MORPH_OP_LOAD_NX8_PP(pvecInp3);

          /*Load Input 2*/
          pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr);
          va2      = MORPH_OP_LOAD_NX8_PP(pvecInp2);
          MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp, va2, pvecInp2);

          pvecInp4 = (MORPH_IDT_Nx8 *)(inp2Ptr2);
          va4      = MORPH_OP_LOAD_NX8_PP(pvecInp4);
          MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va4, pvecInp4);

          IVP_DSELNX16(vecInp4, vecInp2, vecInp1, vecInp, dvecPattern);

          /*Zero point addition*/
          vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
          vecInp4 = MORPH_OP_ADDNX16(vecInp4, (MORPH_IDT_Nx16)(input2_offset));
          /* Output data pointer */
          pvecOut  = (MORPH_IDT_Nx8 *)(outPtr);
          pvecOut2 = (MORPH_IDT_Nx8 *)(outPtr2);

          for (x = 0; x <= (outTiledim1Size - vectorizationWidth); x += vectorizationWidth)
          {
            /* Here as the second input is broadcasted, it is loaded outside the innermost
               loop and and repeated and compared with first input*/
            /*Load Input 1*/
            MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va1, pvecInp1);
            MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp3, va3, pvecInp3);
            /*Zero point addition*/
            vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
            vecInp3 = MORPH_OP_ADDNX16(vecInp3, (MORPH_IDT_Nx16)(input1_offset));
            /* Add operation */
            ADD32VALUES(vecInp1, vecInp2, vecOut);
            ADD32VALUES(vecInp3, vecInp4, vecOut2);
            /* Store the data */
            MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
            MORPH_OP_IVP_SANX8_IP(vecOut2, vaStore2, pvecOut2);
          }
          if(x < outTiledim1Size)
          {
            /*Load Input 1*/
            MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp1, va1, pvecInp1, (outTiledim1Size - x));
            MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp3, va3, pvecInp3, (outTiledim1Size - x));
            /*Zero point addition*/
            vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
            vecInp3 = MORPH_OP_ADDNX16(vecInp3, (MORPH_IDT_Nx16)(input1_offset));
            /* Add operation */
            ADD32VALUES(vecInp1, vecInp2, vecOut);
            ADD32VALUES(vecInp3, vecInp4, vecOut2);
            /* Store the data */
            MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (outTiledim1Size - x));
            MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut2, vaStore2, pvecOut2, (outTiledim1Size - x) * remZ);
          }
          MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
          MORPH_OP_FLUSH_SAPOSNX8(vaStore2, pvecOut2);
        }
      }
    }
  }
  //input1 value broadcast case
  else if (inData1Pitch0 == 0)
  {
    /* if the data is contiguous among all the directions for input1 , input2 and output
     * A special case wherein inTile1 and intile2 will have a depth of 16 is handled with
     * (inData1Pitch1 == 16) || (inData1Pitch1 == 0)
     */
    if (((inData1Pitch1 == 16) || (inData1Pitch1 == 0)) && (inData1Pitch2 == 0) && (inTile1dim2Size == 1) && \
         (inTile2dim2Size * inData2Pitch1 == inData2Pitch2) && (inTile2dim1Size == inData2Pitch1) && \
         (outTiledim2Size * outDataPitch1 == outDataPitch2) && (outTiledim1Size == outDataPitch1))
    {
      /* Input and Output data pointers */
      inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1);
      inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2);
      outPtr = (MORPH_IDT_SCALAR *)(pOutput);
      /* vector and pointer to load and store values*/
      pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr);
      va2      = MORPH_OP_LOAD_NX8_PP(pvecInp2);

      /*Load Input 1*/
      pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
      va1      = MORPH_OP_LOAD_NX8_PP(pvecInp1);
      MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp, va1, pvecInp1);
      vecInp1 = IVP_SELNX16(vecInp, vecInp, IVP_ANDNX16(IVP_SEQNX16(), 15));
      /*Zero point addition*/
      vecInp1  = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
      /* Output data pointer */
      pvecOut  = (MORPH_IDT_Nx8 *)(outPtr);

      for (x = 0; x <= (tripCount - vectorizationWidth); x += vectorizationWidth)
      {
        /* Here as the second input is broadcasted, it is loaded outside the innermost
           loop and and repeated and compared with first input*/
        /*Load Input 2*/
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va2, pvecInp2);
        /*Zero point addition*/
        vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
        /* Add operation */
        ADD32VALUES(vecInp1, vecInp2, vecOut);
        /* Store the data */
        MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
      }
      if(x < tripCount)
      {
        /*Load Input 2*/
        MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp2, va2, pvecInp2, (tripCount - x));
        /*Zero point addition*/
        vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
        /* Add operation */
        ADD32VALUES(vecInp1, vecInp2, vecOut);
        /* Store the data */
        MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (tripCount - x));
      }
      MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
    }
    else if(((inData1Pitch1 == 16) || (inData1Pitch1 == 0)) && (inTile1dim2Size == 1) && \
             (inTile2dim1Size == inData2Pitch1) && \
             (outTiledim1Size == outDataPitch1))
    {
      // if the data is contiguous with respect to first dimension for input1, input2 and output
      for (z = 0; z < outTiledim3Size; z += 2)
      {
        int32_t remZ = XT_MIN(2, (outTiledim3Size - z)) - 1;
        /* Input and Output data pointers */
        inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1 + z * inData1Pitch2);
        inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2 + z * inData2Pitch2);
        outPtr  = (MORPH_IDT_SCALAR *)(pOutput + z * outDataPitch2);

        inp1Ptr2 = (MORPH_IDT_SCALAR *)(pInput1 + (z + remZ) * inData1Pitch2);
        inp2Ptr2 = (MORPH_IDT_SCALAR *)(pInput2 + (z + remZ) * inData2Pitch2);
        outPtr2  = (MORPH_IDT_SCALAR *)(pOutput + (z + remZ) * outDataPitch2);

        /* vector and pointer to load and store values*/
        pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr);
        va2      = MORPH_OP_LOAD_NX8_PP(pvecInp2);

        pvecInp4 = (MORPH_IDT_Nx8 *)(inp2Ptr2);
        va4      = MORPH_OP_LOAD_NX8_PP(pvecInp4);

        /*Load Input 1*/
        pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
        va1      = MORPH_OP_LOAD_NX8_PP(pvecInp1);
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp, va1, pvecInp1);

        pvecInp3 = (MORPH_IDT_Nx8 *)(inp1Ptr2);
        va3      = MORPH_OP_LOAD_NX8_PP(pvecInp3);
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va3, pvecInp3);

        IVP_DSELNX16(vecInp3, vecInp1, vecInp2, vecInp, dvecPattern);

        /*Zero point addition*/
        vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
        vecInp3 = MORPH_OP_ADDNX16(vecInp3, (MORPH_IDT_Nx16)(input1_offset));
        /* Output data pointer */
        pvecOut  = (MORPH_IDT_Nx8 *)(outPtr);
        pvecOut2 = (MORPH_IDT_Nx8 *)(outPtr2);

        for (x = 0; x <= (tripcount1 - XCHAL_IVPN_SIMD_WIDTH); x += XCHAL_IVPN_SIMD_WIDTH)
        {
          /* Load Input 2 */
          MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va2, pvecInp2);
          MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp4, va4, pvecInp4);
          /*Zero point addition*/
          vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
          vecInp4 = MORPH_OP_ADDNX16(vecInp4, (MORPH_IDT_Nx16)(input2_offset));
          /* Add operation */
          ADD32VALUES(vecInp1, vecInp2, vecOut);
          ADD32VALUES(vecInp3, vecInp4, vecOut2);
          /* Store the data */
          MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
          MORPH_OP_IVP_SANX8_IP(vecOut2, vaStore2, pvecOut2);
        }
        if(x < tripcount1)
        {
          /* Load Input 2 */
          MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp2, va2, pvecInp2, (tripcount1 - x));
          MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp4, va4, pvecInp4, (tripcount1 - x));
          /*Zero point addition*/
          vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
          vecInp4 = MORPH_OP_ADDNX16(vecInp4, (MORPH_IDT_Nx16)(input2_offset));
          /* Add operation */
          ADD32VALUES(vecInp1, vecInp2, vecOut);
          ADD32VALUES(vecInp3, vecInp4, vecOut2);
          /* Store the data */
          MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (tripcount1 - x));
          MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut2, vaStore2, pvecOut2, (tripcount1 - x) * remZ);
        }
        MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
        MORPH_OP_FLUSH_SAPOSNX8(vaStore2, pvecOut2);
      }
    }
    else
    { // if the data is not contiguous along inner most dimension
      for (z = 0; z < outTiledim3Size; z += 2)
      {
        int32_t remZ = XT_MIN(2, outTiledim3Size - z) - 1;
        for (y = 0; y < outTiledim2Size; y++)
        {
          /* Input and Output data pointers */
          inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1 + z * inData1Pitch2 + y * inData1Pitch1);
          inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2 + z * inData2Pitch2 + y * inData2Pitch1);
          outPtr = (MORPH_IDT_SCALAR *)(pOutput + z * outDataPitch2 + y * outDataPitch1);

          inp1Ptr2 = (MORPH_IDT_SCALAR *)(pInput1 + (z + remZ) * inData1Pitch2 + y * inData1Pitch1);
          inp2Ptr2 = (MORPH_IDT_SCALAR *)(pInput2 + (z + remZ) * inData2Pitch2 + y * inData2Pitch1);
          outPtr2 = (MORPH_IDT_SCALAR *)(pOutput + (z + remZ) * outDataPitch2 + y * outDataPitch1);

          /* vector and pointer to load input1*/
          pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr);
          va2 = MORPH_OP_LOAD_NX8_PP(pvecInp2);

          pvecInp4 = (MORPH_IDT_Nx8 *)(inp2Ptr2);
          va4 = MORPH_OP_LOAD_NX8_PP(pvecInp4);

          /*Load Input 1*/
          pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
          va1 = MORPH_OP_LOAD_NX8_PP(pvecInp1);
          MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp, va1, pvecInp1);

          pvecInp3 = (MORPH_IDT_Nx8 *)(inp1Ptr2);
          va3 = MORPH_OP_LOAD_NX8_PP(pvecInp3);
          MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va3, pvecInp3);

          IVP_DSELNX16(vecInp3, vecInp1, vecInp2, vecInp, dvecPattern);
          /*Zero point addition*/
          vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
          vecInp3 = MORPH_OP_ADDNX16(vecInp3, (MORPH_IDT_Nx16)(input1_offset));
          /* Output data pointer */
          pvecOut = (MORPH_IDT_Nx8 *)(outPtr);
          pvecOut2 = (MORPH_IDT_Nx8 *)(outPtr2);

          for (x = 0; x <= (outTiledim1Size - vectorizationWidth); x += vectorizationWidth)
          {
            /* Here as the second input is broadcasted, it is loaded outside the innermost
               loop and and repeated and compared with first input*/
            /*Load Input 2*/
            MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va2, pvecInp2);
            MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp4, va4, pvecInp4);
            /*Zero point addition*/
            vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
            vecInp4 = MORPH_OP_ADDNX16(vecInp4, (MORPH_IDT_Nx16)(input2_offset));
            /* Add operation */
            ADD32VALUES(vecInp1, vecInp2, vecOut);
            ADD32VALUES(vecInp3, vecInp4, vecOut2);
            /* Store the data */
            MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
            MORPH_OP_IVP_SANX8_IP(vecOut2, vaStore2, pvecOut2);
          }
          if(x < outTiledim1Size)
          {
            /*Load Input 1*/
            MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp2, va2, pvecInp2, (outTiledim1Size - x));
            MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp4, va4, pvecInp4, (outTiledim1Size - x));
            /*Zero point addition*/
            vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
            vecInp4 = MORPH_OP_ADDNX16(vecInp4, (MORPH_IDT_Nx16)(input2_offset));
            /* Add operation */
            ADD32VALUES(vecInp1, vecInp2, vecOut);
            ADD32VALUES(vecInp3, vecInp4, vecOut2);
            /* Store the data */
            MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (outTiledim1Size - x));
            MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut2, vaStore2, pvecOut2, (outTiledim1Size - x) * remZ);
          }
          MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
          MORPH_OP_FLUSH_SAPOSNX8(vaStore2, pvecOut2);
        }
      }
    }
  }
}

/******************* broadCastDims2and3NoQuant_ID16WH *********************/
/* Description : Optimized implementation of Broadcast and add            */
/*               functionality across 2nd and 3rd dimension for ID16WH    */
/*               Here there is no quantization applied as the input and   */
/*               output scales are assumed to be the same                 */
/* Inputs      : Input tile and AddA_params                               */
/* Outputs     : XI Error Code                                            */
/* InOuts      : Output Tile                                              */
/* Assumptions : InData, outData is U8/S8                                 */
/**************************************************************************/
void MAKE_NAME(broadCastDims2and3NoQuant_ID16WH)(const xi_pTile3D inTile1,
                                                 const xi_pTile3D inTile2,
                                                 xi_pTile3D outTile,
                                                 const xi_cnn_addA_params *pparams,
                                                 int32_t inData1Pitch0,
                                                 int32_t inData2Pitch0,
                                                 int32_t inData1Pitch1,
                                                 int32_t inData2Pitch1,
                                                 int32_t inData1Pitch2,
                                                 int32_t inData2Pitch2)
{
  /* Getting parameters from the tile structures */
  const int32_t  outTiledim1Size   = XI_TILE3D_GET_DIM1(outTile);
  const int32_t  outTiledim2Size   = XI_TILE3D_GET_DIM2(outTile);
  const int32_t  outTiledim3Size   = XI_TILE3D_GET_DIM3(outTile);
  const int32_t  outDataPitch1     = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t  outDataPitch2     = XI_TILE3D_GET_DIM2_PITCH(outTile);

  /* Get Data Pointers */
  MORPH_IDT_SCALAR *pInput1 = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile1);
  MORPH_IDT_SCALAR *pInput2 = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile2);
  MORPH_IDT_SCALAR *pOutput = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(outTile);

  /* Get add struct parameters*/
  int32_t input1_offset         = XI_CNN_ADDA_GET_ZERO_POINT_IN1(pparams);
  int32_t input2_offset         = XI_CNN_ADDA_GET_ZERO_POINT_IN2(pparams);
  int32_t output_offset         = XI_CNN_ADDA_GET_ZERO_POINT_OUT(pparams);
  int32_t output_activation_min = XI_CNN_ADDA_GET_MIN_VAL(pparams);
  int32_t output_activation_max = XI_CNN_ADDA_GET_MAX_VAL(pparams);

  /* Input and Output data pointers */
  MORPH_IDT_SCALAR *restrict inp1Ptr;
  MORPH_IDT_SCALAR *restrict inp2Ptr;
  MORPH_IDT_SCALAR *restrict outPtr;

  /* Input pointer and data vector */
  MORPH_IDT_Nx8 *restrict pvecInp1;
  MORPH_IDT_Nx8 *restrict pvecInp2;
  MORPH_IDT_Nx16 vecInp2, vecInp1;

  /* Output pointer and data vector */
  MORPH_IDT_Nx8 *restrict pvecOut;
  MORPH_IDT_Nx16 vecOut;

  int32_t x, y, z;
  valign va1, va2, vaStore;

  int32_t tripCount = outTiledim1Size * outTiledim2Size;

  int32_t vectorizationWidth   = XCHAL_IVPN_SIMD_WIDTH;
  vaStore = IVP_ZALIGN();
  MORPH_IDT_Nx16 vecOffset = input1_offset + input2_offset + output_offset;

  if ((outTiledim1Size == inData1Pitch1) && \
      (outTiledim1Size == inData2Pitch1) && \
      (outTiledim1Size == outDataPitch1))
  {
    for (z = 0; z < outTiledim3Size; z++)
    {
      /* input and output data pointers */
      inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1 + z * inData1Pitch2);
      inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2 + z * inData2Pitch2);
      outPtr = (MORPH_IDT_SCALAR *)(pOutput + z * outDataPitch2);

      /* vector and pointer to load and store values*/
      pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
      va1 = MORPH_OP_LOAD_NX8_PP(pvecInp1);

      pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr);
      va2 = MORPH_OP_LOAD_NX8_PP(pvecInp2);

      pvecOut = (MORPH_IDT_Nx8 *)(outPtr);

      for (x = 0; x <= (tripCount - vectorizationWidth); x += vectorizationWidth)
      {
        /*Loading inputs */
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va1, pvecInp1);
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va2, pvecInp2);

        /* Add operation */
        MORPH_IDT_Nx16 vecInp = MORPH_OP_ADDNX16(vecInp1, vecOffset);
        vecOut = MORPH_OP_ADDNX16(vecInp, vecInp2);
        vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

        /* Store the data */
        MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
      }
      if( x < tripCount)
      {
        /*Loading inputs */
        MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp1, va1, pvecInp1, (tripCount - x));
        MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp2, va2, pvecInp2, (tripCount - x));

        /* Add operation */
        MORPH_IDT_Nx16 vecInp = MORPH_OP_ADDNX16(vecInp1, vecOffset);
        vecOut = MORPH_OP_ADDNX16(vecInp, vecInp2);
        vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));
        /* Store the data */
        MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (tripCount - x));
      }
      MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
    }
  }
  else
  {
    for (z = 0; z < outTiledim3Size; z++)
    {
      for (y = 0; y < outTiledim2Size; y++)
      {
        /* input and output data pointers */
        inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1 + z * inData1Pitch2 + y * inData1Pitch1);
        inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2 + z * inData2Pitch2 + y * inData2Pitch1);
        outPtr = (MORPH_IDT_SCALAR *)(pOutput + z * outDataPitch2 + y * outDataPitch1);

        /* vector and pointer to load and store values*/
        pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
        va1 = MORPH_OP_LOAD_NX8_PP(pvecInp1);

        pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr);
        va2 = MORPH_OP_LOAD_NX8_PP(pvecInp2);

        pvecOut = (MORPH_IDT_Nx8 *)(outPtr);

        for (x = 0; x <= (outTiledim1Size - vectorizationWidth); x += vectorizationWidth)
        {
          /*Loading inputs */
          MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va1, pvecInp1);
          MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va2, pvecInp2);

          /* Add operation */
          MORPH_IDT_Nx16 vecInp = MORPH_OP_ADDNX16(vecInp1, vecOffset);
          vecOut = MORPH_OP_ADDNX16(vecInp, vecInp2);
          vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

          /* Store the data */
          MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
        }
        if(x < outTiledim1Size)
        {
          /*Loading inputs */
          MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp1, va1, pvecInp1, (outTiledim1Size - x));
          MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp2, va2, pvecInp2, (outTiledim1Size - x));

          /* Add operation */
          MORPH_IDT_Nx16 vecInp = MORPH_OP_ADDNX16(vecInp1, vecOffset);
          vecOut = MORPH_OP_ADDNX16(vecInp, vecInp2);
          vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

          /* Store the data */
          MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (outTiledim1Size - x));
        }
        MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
      }
    }
  }
}

/******************* broadCastDims2and3_ID16WH ****************************/
/* Description : Optimized implementation of Broadcast and add            */
/*               functionality across 2nd and 3rd dimension for ID16WH    */
/* Inputs      : Input tile and AddA_params                               */
/* Outputs     : XI Error Code                                            */
/* InOuts      : Output Tile                                              */
/* Assumptions : InData, outData is U8/S8                                 */
/**************************************************************************/
void MAKE_NAME(broadCastDims2and3_ID16WH)(const xi_pTile3D inTile1,
                                          const xi_pTile3D inTile2,
                                          xi_pTile3D outTile,
                                          const xi_cnn_addA_params *pparams,
                                          int32_t inData1Pitch0,
                                          int32_t inData2Pitch0,
                                          int32_t inData1Pitch1,
                                          int32_t inData2Pitch1,
                                          int32_t inData1Pitch2,
                                          int32_t inData2Pitch2)
{
  /* Getting parameters from the tile structures */
  const int32_t  outTiledim1Size   = XI_TILE3D_GET_DIM1(outTile);
  const int32_t  outTiledim2Size   = XI_TILE3D_GET_DIM2(outTile);
  const int32_t  outTiledim3Size   = XI_TILE3D_GET_DIM3(outTile);
  const int32_t  outDataPitch1     = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t  outDataPitch2     = XI_TILE3D_GET_DIM2_PITCH(outTile);

  /* Get Data Pointers */
  MORPH_IDT_SCALAR *pInput1 = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile1);
  MORPH_IDT_SCALAR *pInput2 = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile2);
  MORPH_IDT_SCALAR *pOutput = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(outTile);

  /* Get add struct parameters*/
  int32_t input1_offset         = XI_CNN_ADDA_GET_ZERO_POINT_IN1(pparams);
  int32_t input2_offset         = XI_CNN_ADDA_GET_ZERO_POINT_IN2(pparams);
  int32_t output_offset         = XI_CNN_ADDA_GET_ZERO_POINT_OUT(pparams);
  int32_t input1_multiplier     = XI_CNN_ADDA_GET_MULTIPLIER_IN1(pparams);
  int32_t input2_multiplier     = XI_CNN_ADDA_GET_MULTIPLIER_IN2(pparams);
  int32_t output_multiplier     = XI_CNN_ADDA_GET_MULTIPLIER_OUT(pparams);
  int32_t input1_shift          = XI_CNN_ADDA_GET_SHIFT_IN1(pparams);
  int32_t input2_shift          = XI_CNN_ADDA_GET_SHIFT_IN2(pparams);
  int32_t output_shift          = XI_CNN_ADDA_GET_SHIFT_OUT(pparams);
  int32_t output_activation_min = XI_CNN_ADDA_GET_MIN_VAL(pparams);
  int32_t output_activation_max = XI_CNN_ADDA_GET_MAX_VAL(pparams);
  const int32_t left_shift      = XI_CNN_ADDA_GET_LEFT_SHIFT(pparams);

  /* Input and Output data pointers */
  MORPH_IDT_SCALAR *restrict inp1Ptr;
  MORPH_IDT_SCALAR *restrict inp2Ptr;
  MORPH_IDT_SCALAR *restrict outPtr;

  /* Input pointer and data vector */
  MORPH_IDT_Nx8 *restrict pvecInp1;
  MORPH_IDT_Nx8 *restrict pvecInp2;
  MORPH_IDT_Nx16 vecInp2, vecInp1;

  /* Output pointer and data vector */
  MORPH_IDT_Nx8 *restrict pvecOut;
  MORPH_IDT_Nx16 vecOut;

  int32_t x, y, z;
  valign va1, va2, vaStore;

  int32_t tripCount = outTiledim1Size * outTiledim2Size;

  int32_t vectorizationWidth   = XCHAL_IVPN_SIMD_WIDTH;
  vaStore = IVP_ZALIGN();

  if ((outTiledim1Size == inData1Pitch1) && \
      (outTiledim1Size == inData2Pitch1) && \
      (outTiledim1Size == outDataPitch1))
  {
    for (z = 0; z < outTiledim3Size; z++)
    {
      /* input and output data pointers */
      inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1 + z * inData1Pitch2);
      inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2 + z * inData2Pitch2);
      outPtr = (MORPH_IDT_SCALAR *)(pOutput + z * outDataPitch2);

      /* vector and pointer to load and store values*/
      pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
      va1 = MORPH_OP_LOAD_NX8_PP(pvecInp1);

      pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr);
      va2 = MORPH_OP_LOAD_NX8_PP(pvecInp2);

      pvecOut = (MORPH_IDT_Nx8 *)(outPtr);

      for (x = 0; x <= (tripCount - vectorizationWidth); x += vectorizationWidth)
      {
        /*Loading inputs */
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va1, pvecInp1);
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va2, pvecInp2);
        /*Zero point addition*/
        vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
        vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
        /* Add operation */
        ADD32VALUES(vecInp1, vecInp2, vecOut);
        /* Store the data */
        MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
      }
      if( x < tripCount)
      {
        /*Loading inputs */
        MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp1, va1, pvecInp1, (tripCount - x));
        MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp2, va2, pvecInp2, (tripCount - x));
        /*Zero point addition*/
        vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
        vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
        /* Add operation*/
        ADD32VALUES(vecInp1, vecInp2, vecOut);
        /* Store the data */
        MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (tripCount - x));
      }
      MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
    }
  }
  else
  {
    for (z = 0; z < outTiledim3Size; z++)
    {
      for (y = 0; y < outTiledim2Size; y++)
      {
        /* input and output data pointers */
        inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1 + z * inData1Pitch2 + y * inData1Pitch1);
        inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2 + z * inData2Pitch2 + y * inData2Pitch1);
        outPtr = (MORPH_IDT_SCALAR *)(pOutput + z * outDataPitch2 + y * outDataPitch1);

        /* vector and pointer to load and store values*/
        pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
        va1 = MORPH_OP_LOAD_NX8_PP(pvecInp1);

        pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr);
        va2 = MORPH_OP_LOAD_NX8_PP(pvecInp2);

        pvecOut = (MORPH_IDT_Nx8 *)(outPtr);

        for (x = 0; x <= (outTiledim1Size - vectorizationWidth); x += vectorizationWidth)
        {
          /*Loading inputs */
          MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va1, pvecInp1);
          MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va2, pvecInp2);
          /*Zero point addition*/
          vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
          vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
          /* Add operation */
          ADD32VALUES(vecInp1, vecInp2, vecOut);
          /* Store the data */
          MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
        }
        if(x < outTiledim1Size)
        {
          /*Loading inputs */
          MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp1, va1, pvecInp1, (outTiledim1Size - x));
          MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp2, va2, pvecInp2, (outTiledim1Size - x));
          /*Zero point addition*/
          vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
          vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
          /* Add operation*/
          ADD32VALUES(vecInp1, vecInp2, vecOut);
          /* Store the data */
          MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (outTiledim1Size - x));
        }
        MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
      }
    }
  }
}

/******************* broadcastAddA3D_ID16WH *******************************/
/* Description : Optimized implementation of  broadcast add functionality */
/*               for ID16WH                                               */
/* Inputs      : Input tile and AddA_params                               */
/* Outputs     : XI Error Code                                            */
/* InOuts      : Output Tile                                              */
/* Assumptions : InData, outData is U8/S8                                 */
/**************************************************************************/
void MAKE_NAME(broadcastAddA3D_ID16WH)(const xi_pTile3D inTile1,
                                       const xi_pTile3D inTile2,
                                       xi_pTile3D outTile,
                                       const xi_cnn_addA_params *pparams)
{

    /* Getting parameters from the tile structures */
  const int32_t  inTile1dim1Size      = XI_TILE3D_GET_DIM1(inTile1);
  const int32_t  inTile1dim2Size      = XI_TILE3D_GET_DIM2(inTile1);
  const int32_t  inTile1dim3Size      = XI_TILE3D_GET_DIM3(inTile1);
  const int32_t  inTile2dim1Size      = XI_TILE3D_GET_DIM1(inTile2);
  const int32_t  inTile2dim2Size      = XI_TILE3D_GET_DIM2(inTile2);
  const int32_t  inTile2dim3Size      = XI_TILE3D_GET_DIM3(inTile2);

  /* Setting pitches of the input tiles */
  int32_t inData1Pitch0 = 16;
  int32_t inData1Pitch1 = XI_TILE3D_GET_DIM1_PITCH(inTile1);
  int32_t inData1Pitch2 = XI_TILE3D_GET_DIM2_PITCH(inTile1);
  int32_t inData2Pitch0 = 16;
  int32_t inData2Pitch1 = XI_TILE3D_GET_DIM1_PITCH(inTile2);
  int32_t inData2Pitch2 = XI_TILE3D_GET_DIM2_PITCH(inTile2);
  uint8_t qFlagVal = XI_CNN_ADDA_GET_QUANTIZATION_FLAG(pparams);

  /*Broadcast function across 2nd and 3rd dimension happens when the corresponding dimesion size is 1*/
  /*Broadcast function across 1st dimension happens when the corresponding dimesion size is 16 which */
  /* is the minimal size requirement for ID16WH variant.  The 16 values in an ideal scenario is      */
  /* expected to be same except when the original tile size was 16.                                  */
  int broadCastDims = 0;

  if (inTile1dim3Size != inTile2dim3Size)
  {
    if (inTile1dim3Size == 1)
    {
      inData1Pitch2 = 0;
    }
    else
    {
      inData2Pitch2 = 0;
    }
    broadCastDims = 3;
  }
  if (inTile1dim2Size != inTile2dim2Size)
  {
    if (inTile1dim2Size == 1)
    {
      inData1Pitch1 = 0;
    }
    else
    {
      inData2Pitch1 = 0;
    }
    broadCastDims = 2;
  }

  if (inTile1dim1Size != inTile2dim1Size)
  {
    if ((inTile1dim1Size >> 4) == 1)
    {
      inData1Pitch0 = 0;
    }
    else
    {
      inData2Pitch0 = 0;
    }
    broadCastDims = 1;
  }

  switch (broadCastDims)
  {
  case 1: 
    if (qFlagVal == 0)
    {
      MAKE_NAME(broadCastDims1NoQuant_ID16WH)(inTile1, inTile2, outTile, pparams, inData1Pitch0, \
        inData2Pitch0, inData1Pitch1, inData2Pitch1, inData1Pitch2, inData2Pitch2);
    }
    else
    {
      MAKE_NAME(broadCastDims1_ID16WH)(inTile1, inTile2, outTile, pparams, inData1Pitch0, \
        inData2Pitch0, inData1Pitch1, inData2Pitch1, inData1Pitch2, inData2Pitch2);
    }
    break;
  case 2:
  case 3:
    if (qFlagVal == 0)
    {
      MAKE_NAME(broadCastDims2and3NoQuant_ID16WH)(inTile1, inTile2, outTile, pparams, inData1Pitch0, \
        inData2Pitch0, inData1Pitch1, inData2Pitch1, inData1Pitch2, inData2Pitch2);
    }
    else
    {
      MAKE_NAME(broadCastDims2and3_ID16WH)(inTile1, inTile2, outTile, pparams, inData1Pitch0, \
        inData2Pitch0, inData1Pitch1, inData2Pitch1, inData1Pitch2, inData2Pitch2);
    }
    break;
  default: MAKE_NAME(xiAddA3D)(inTile1, inTile2, outTile, pparams);
    break;
  }
}
#endif

/********************* BroadCastDims1NoQuant ******************************/
/* Description : Optimized implementation of Broadcast and add            */
/*               functionality across first dimension.                    */
/*               Here there is no quantization applied as the input and   */
/*               output scales are assumed to be the same                 */
/* Inputs      : Input tile and AddA_params                               */
/* Outputs     : XI Error Code                                            */
/* InOuts      : Output Tile                                              */
/* Assumptions : InData, outData is U8/S8                                 */
/**************************************************************************/
static void MAKE_NAME(BroadCastDims1NoQuant)(const xi_pTile3D inTile1,
                                      const xi_pTile3D inTile2,
                                      xi_pTile3D outTile,
                                      const xi_cnn_addA_params *pparams,
                                      int32_t inData1Pitch0,
                                      int32_t inData2Pitch0,
                                      int32_t inData1Pitch1,
                                      int32_t inData2Pitch1,
                                      int32_t inData1Pitch2,
                                      int32_t inData2Pitch2)
{
  /* Getting parameters from the tile structures */
  const int32_t  outTiledim1Size      = XI_TILE3D_GET_DIM1(outTile);
  const int32_t  outTiledim2Size      = XI_TILE3D_GET_DIM2(outTile);
  const int32_t  outTiledim3Size      = XI_TILE3D_GET_DIM3(outTile);
  const int32_t outDataPitch1         = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2         = XI_TILE3D_GET_DIM2_PITCH(outTile);
  /* Get Data Pointers */
  MORPH_IDT_SCALAR *pInput1 = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile1);
  MORPH_IDT_SCALAR *pInput2 = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile2);
  MORPH_IDT_SCALAR *pOutput = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(outTile);
  /* Get add struct parameters*/
  int32_t input1_offset         = XI_CNN_ADDA_GET_ZERO_POINT_IN1(pparams);
  int32_t input2_offset         = XI_CNN_ADDA_GET_ZERO_POINT_IN2(pparams);
  int32_t output_offset         = XI_CNN_ADDA_GET_ZERO_POINT_OUT(pparams);
  int32_t output_activation_min =  XI_CNN_ADDA_GET_MIN_VAL(pparams);
  int32_t output_activation_max =  XI_CNN_ADDA_GET_MAX_VAL(pparams);


  /* Input and Output data pointers */
  MORPH_IDT_SCALAR *restrict inp1Ptr, *restrict inp1Ptr2;
  MORPH_IDT_SCALAR *restrict inp2Ptr, *restrict inp2Ptr2;
  MORPH_IDT_SCALAR *restrict outPtr, *restrict outPtr2;

  MORPH_IDT_Nx8 *restrict pvecInp1;
  MORPH_IDT_Nx8 *restrict pvecInp2;
  MORPH_IDT_Nx8 *restrict pvecOut;
  MORPH_IDT_Nx8 *restrict pvecOut2;

  /* Input and Output data vectors */
  MORPH_IDT_Nx16 vecInp2, vecInp1, vecOut;
  MORPH_IDT_Nx16 vecInp3, vecInp4, vecOut2;

  int32_t tripcount  = outTiledim1Size * outTiledim2Size * outTiledim3Size;
  int32_t tripcount1 = outTiledim1Size * outTiledim2Size;

  int32_t x, y, z;
  valign va1, va2;
  valign vaStore  = IVP_ZALIGN();
  valign vaStore2 = IVP_ZALIGN();
  MORPH_IDT_Nx16 vecOffset = input1_offset + input2_offset + output_offset;

  //input2 value broadcast case
  if(inData2Pitch0 == 0 )
  {
    // if the data is contiguous among all the directions for input1 , input2 and output
    if ((inData2Pitch1 == 0) && (inData2Pitch2 == 0) && \
        (outTiledim2Size * inData1Pitch1 == inData1Pitch2) && (outTiledim1Size == inData1Pitch1) && \
        (outTiledim2Size * outDataPitch1 == outDataPitch2) && (outTiledim1Size == outDataPitch1))
    {
      /* Input and Output data pointers */
      inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1);
      inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2);
      outPtr  = (MORPH_IDT_SCALAR *)(pOutput);
      /* vector and pointer to load and store values*/
      pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
      va1      = MORPH_OP_LOAD_NX8_PP(pvecInp1);
      pvecOut  = (MORPH_IDT_Nx8 *)(outPtr);

      /* Load Input 2*/
      vecInp2 = (MORPH_IDT_Nx8)(inp2Ptr[0]);
      /* Zero point addition */
      vecInp2 = MORPH_OP_ADDNX16(vecInp2, vecOffset);

      for ( x = 0; x <= (tripcount - XCHAL_IVPN_SIMD_WIDTH); x += XCHAL_IVPN_SIMD_WIDTH)
      {
        /* load the data */
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va1, pvecInp1);

        /* Add operation */
        vecOut = MORPH_OP_ADDNX16(vecInp1, vecInp2);
        vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

        /* Store the data */
        MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
      }
      if(x < tripcount)
      {
        /* load the data */
        MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp1, va1, pvecInp1, (tripcount - x));

        /* Add operation */
        vecOut = MORPH_OP_ADDNX16(vecInp1, vecInp2);
        vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

        /* Store the data */
        MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (tripcount - x));
      }
      MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
    }
    else if((inData2Pitch1 == 0) && \
            (outTiledim1Size == inData1Pitch1) && \
            (outTiledim1Size == outDataPitch1))
    {
      for (z = 0; z < outTiledim3Size; z++)
      {
        /* Input and Output data pointers */
        inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1 + z * inData1Pitch2);
        inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2 + z * inData2Pitch2);
        outPtr = (MORPH_IDT_SCALAR *)(pOutput + z * outDataPitch2);

        pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
        va1 = MORPH_OP_LOAD_NX8_PP(pvecInp1);
        pvecOut = (MORPH_IDT_Nx8 *)(outPtr);

        /* Load Input 2*/
        vecInp2 = (MORPH_IDT_Nx8)(inp2Ptr[0]);
        /* Add input data and pre-computed offset */
        vecInp2 = MORPH_OP_ADDNX16(vecInp2, vecOffset);

        for (x = 0; x <= (tripcount1 - XCHAL_IVPN_SIMD_WIDTH); x += XCHAL_IVPN_SIMD_WIDTH)
        {
          /* Load Input 1 */
          MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va1, pvecInp1);

          /* Add operation */
          vecOut = MORPH_OP_ADDNX16(vecInp1, vecInp2);
          vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

          /* Store the data */
          MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
        }
        if(x < tripcount1)
        {
          /* Load Input 1 */
          MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp1, va1, pvecInp1, tripcount1 - x);

          /* Add operation */
          vecOut = MORPH_OP_ADDNX16(vecInp1, vecInp2);
          vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

          /* Store the data */
          MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, tripcount1 - x);
        }
        MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
      }
    }
    else  // if the data is not contiguous along inner most dimension
    {
      for (z = 0; z < outTiledim3Size; z += 2)
      {
        int32_t remZ = XT_MIN(2, (outTiledim3Size - z)) - 1;

        for (y = 0; y < outTiledim2Size; y++)
        {
          /* Input and Output data pointers */
          inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1 + z * inData1Pitch2 + y * inData1Pitch1);
          inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2 + z * inData2Pitch2 + y * inData2Pitch1);
          outPtr = (MORPH_IDT_SCALAR *)(pOutput + z * outDataPitch2 + y * outDataPitch1);

          inp1Ptr2 = (MORPH_IDT_SCALAR *)(pInput1 + (z + remZ) * inData1Pitch2 + y * inData1Pitch1);
          inp2Ptr2 = (MORPH_IDT_SCALAR *)(pInput2 + (z + remZ) * inData2Pitch2 + y * inData2Pitch1);
          outPtr2 = (MORPH_IDT_SCALAR *)(pOutput + (z + remZ) * outDataPitch2 + y * outDataPitch1);

          /* vector and pointer to load and store values*/
          pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
          va1 = MORPH_OP_LOAD_NX8_PP(pvecInp1);

          pvecInp2 = (MORPH_IDT_Nx8 *)(inp1Ptr2);
          va2 = MORPH_OP_LOAD_NX8_PP(pvecInp2);

          pvecOut = (MORPH_IDT_Nx8 *)(outPtr);
          pvecOut2 = (MORPH_IDT_Nx8 *)(outPtr2);

          /* load the data 2 */
          vecInp2 = (MORPH_IDT_Nx8)(inp2Ptr[0]);
          vecInp4 = (MORPH_IDT_Nx8)(inp2Ptr2[0]);
          /* Zero point addition */
          vecInp2 = MORPH_OP_ADDNX16(vecInp2, vecOffset);
          vecInp4 = MORPH_OP_ADDNX16(vecInp4, vecOffset);

          for (x = 0; x <= (outTiledim1Size - XCHAL_IVPN_SIMD_WIDTH); x += XCHAL_IVPN_SIMD_WIDTH)
          {
            /* load the data 1 */
            MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va1, pvecInp1);
            MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp3, va2, pvecInp2);

            /* Add operation */
            vecOut = MORPH_OP_ADDNX16(vecInp1, vecInp2);
            vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

            /* Add operation */
            vecOut2 = MORPH_OP_ADDNX16(vecInp3, vecInp4);
            vecOut2 = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut2, (xb_vecNx16)output_activation_min));

            /* Store the data */
            MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
            MORPH_OP_IVP_SANX8_IP(vecOut2, vaStore2, pvecOut2);
          }
          if(x < outTiledim1Size)
          {
            /* load the data 1 */
            MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp1, va1, pvecInp1, (outTiledim1Size - x));
            MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp3, va2, pvecInp2, (outTiledim1Size - x));

            /* Add operation */
            vecOut = MORPH_OP_ADDNX16(vecInp1, vecInp2);
            vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

            /* Add operation */
            vecOut2 = MORPH_OP_ADDNX16(vecInp3, vecInp4);
            vecOut2 = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut2, (xb_vecNx16)output_activation_min));

            /* Store the data */
            MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (outTiledim1Size - x));
            MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut2, vaStore2, pvecOut2, (outTiledim1Size - x) * remZ);
          }
          MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
          MORPH_OP_FLUSH_SAPOSNX8(vaStore2, pvecOut2);
        }
      }
    }
  }
  //input1 value broadcast case
  else if(inData1Pitch0 == 0 )
  {
    // if the data is contiguous among all the directions for input1 , input2 and output
    if ((inData1Pitch1 == 0)&&(inData1Pitch2 == 0) && \
        (outTiledim2Size * inData2Pitch1 == inData2Pitch2) && (outTiledim1Size == inData2Pitch1) && \
        (outTiledim2Size * outDataPitch1 == outDataPitch2) && (outTiledim1Size == outDataPitch1))
    {
      /* Input and Output data pointers */
      inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1);
      inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2);
      outPtr = (MORPH_IDT_SCALAR *)(pOutput);

      pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr);
      va2 = MORPH_OP_LOAD_NX8_PP(pvecInp2);
      pvecOut = (MORPH_IDT_Nx8 *)(outPtr);

      /* Load Input 1 */
      vecInp1 = (MORPH_IDT_Nx8)(inp1Ptr[0]);
      /* Add input data and pre-computed offset */
      vecInp1 = MORPH_OP_ADDNX16(vecInp1, vecOffset);

      for (x = 0; x <= (tripcount - XCHAL_IVPN_SIMD_WIDTH); x += XCHAL_IVPN_SIMD_WIDTH)
      {
        /* Load Input 2 */
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va2, pvecInp2);

        /* Add operation */
        vecOut = MORPH_OP_ADDNX16(vecInp1, vecInp2);
        vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

        /* Store the data */
        MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
      }
      if(x < tripcount)
      {
        /* Load Input 2*/
        MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp2, va2, pvecInp2, (tripcount - x));
        /* Add operation */
        vecOut = MORPH_OP_ADDNX16(vecInp1, vecInp2);
        vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

        /* Store the data */
        MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (tripcount - x));
      }
      MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
    }
    else if((inData1Pitch1 == 0) && \
            (outTiledim1Size == inData2Pitch1) && \
            (outTiledim1Size == outDataPitch1))
    {
      for (z = 0; z < outTiledim3Size; z++)
      {
        /* Input and Output data pointers */
        inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1 + z * inData1Pitch2);
        inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2 + z * inData2Pitch2);
        outPtr = (MORPH_IDT_SCALAR *)(pOutput + z * outDataPitch2);

        pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr);
        va2 = MORPH_OP_LOAD_NX8_PP(pvecInp2);
        pvecOut = (MORPH_IDT_Nx8 *)(outPtr);

        /* Load Input 1*/
        vecInp1 = (MORPH_IDT_Nx8)(inp1Ptr[0]);
        /* Add input data and pre-computed offset */
        vecInp1 = MORPH_OP_ADDNX16(vecInp1, vecOffset);

        for (x = 0; x <= (tripcount1 - XCHAL_IVPN_SIMD_WIDTH); x += XCHAL_IVPN_SIMD_WIDTH)
        {
          /* Load Input 2 */
          MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va2, pvecInp2);

          /* add operation */
          vecOut = MORPH_OP_ADDNX16(vecInp1, vecInp2);
          vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

          /* Store the data */
          MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
        }
        if(x < tripcount1)
        {
          /* Load Input 2 */
          MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp2, va2, pvecInp2, tripcount1 - x);
          /* add operation */
          vecOut = MORPH_OP_ADDNX16(vecInp1, vecInp2);
          vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

          /* Store the data */
          MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, tripcount1 - x);
        }
        MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
      }
    }
    else
    {  // if the data is not contiguous along inner most dimension
      for (z = 0; z < outTiledim3Size; z += 2)
      {
        int32_t remZ = XT_MIN(2, (outTiledim3Size - z)) - 1;
        for (y = 0; y < outTiledim2Size; y++)
        {
          /* Input and Output data pointers */
          inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1 + z * inData1Pitch2 + y * inData1Pitch1);
          inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2 + z * inData2Pitch2 + y * inData2Pitch1);
          outPtr = (MORPH_IDT_SCALAR *)(pOutput + z * outDataPitch2 + y * outDataPitch1);

          inp1Ptr2 = (MORPH_IDT_SCALAR *)(pInput1 + (z + remZ) * inData1Pitch2 + y * inData1Pitch1);
          inp2Ptr2 = (MORPH_IDT_SCALAR *)(pInput2 + (z + remZ) * inData2Pitch2 + y * inData2Pitch1);
          outPtr2 = (MORPH_IDT_SCALAR *)(pOutput + (z + remZ) * outDataPitch2 + y * outDataPitch1);

          pvecInp1 = (MORPH_IDT_Nx8 *)(inp2Ptr);
          va1 = MORPH_OP_LOAD_NX8_PP(pvecInp1);

          pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr2);
          va2 = MORPH_OP_LOAD_NX8_PP(pvecInp2);

          pvecOut = (MORPH_IDT_Nx8 *)(outPtr);
          pvecOut2 = (MORPH_IDT_Nx8 *)(outPtr2);

          /* load the data 1 */
          vecInp1 = (MORPH_IDT_Nx8)(inp1Ptr[0]);
          vecInp3 = (MORPH_IDT_Nx8)(inp1Ptr2[0]);
          /* Add input data and pre-computed offset */
          vecInp1 = MORPH_OP_ADDNX16(vecInp1, vecOffset);
          vecInp3 = MORPH_OP_ADDNX16(vecInp3, vecOffset);

          for (x = 0; x <= (outTiledim1Size - XCHAL_IVPN_SIMD_WIDTH); x += XCHAL_IVPN_SIMD_WIDTH)
          {
            /* load the data 2 */
            MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va1, pvecInp1);
            MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp4, va2, pvecInp2);
            /* add operation */
            vecOut = MORPH_OP_ADDNX16(vecInp1, vecInp2);
            vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

            vecOut2 = MORPH_OP_ADDNX16(vecInp3, vecInp4);
            vecOut2 = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut2, (xb_vecNx16)output_activation_min));
            /* Store the data */
            MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
            MORPH_OP_IVP_SANX8_IP(vecOut2, vaStore2, pvecOut2);
          }
          if(x < outTiledim1Size)
          {
            /* load the data 2 */
            MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp2, va1, pvecInp1, (outTiledim1Size - x));
            MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp4, va2, pvecInp2, (outTiledim1Size - x));
            /* add operation */
            vecOut = MORPH_OP_ADDNX16(vecInp1, vecInp2);
            vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

            vecOut2 = MORPH_OP_ADDNX16(vecInp3, vecInp4);
            vecOut2 = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut2, (xb_vecNx16)output_activation_min));

            /* Store the data */
            MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (outTiledim1Size - x));
            MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut2, vaStore2, pvecOut2, (outTiledim1Size - x) * remZ);
          }
          MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
          MORPH_OP_FLUSH_SAPOSNX8(vaStore2, pvecOut2);
        }
      }
    }
  }
}

/******************* BroadCastDims1 ***************************************/
/* Description : Optimized implementation of Broadcast and add            */
/*               functionality across first dimension                     */
/* Inputs      : Input tile and AddA_params                               */
/* Outputs     : XI Error Code                                            */
/* InOuts      : Output Tile                                              */
/* Assumptions : InData, outData is U8/S8                                 */
/**************************************************************************/
static void MAKE_NAME(BroadCastDims1)(const xi_pTile3D inTile1,
                                      const xi_pTile3D inTile2,
                                      xi_pTile3D outTile,
                                      const xi_cnn_addA_params *pparams,
                                      int32_t inData1Pitch0,
                                      int32_t inData2Pitch0,
                                      int32_t inData1Pitch1,
                                      int32_t inData2Pitch1,
                                      int32_t inData1Pitch2,
                                      int32_t inData2Pitch2)
{
  /* Getting parameters from the tile structures */
  const int32_t  outTiledim1Size      = XI_TILE3D_GET_DIM1(outTile);
  const int32_t  outTiledim2Size      = XI_TILE3D_GET_DIM2(outTile);
  const int32_t  outTiledim3Size      = XI_TILE3D_GET_DIM3(outTile);
  const int32_t outDataPitch1         = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2         = XI_TILE3D_GET_DIM2_PITCH(outTile);
  /* Get Data Pointers */
  MORPH_IDT_SCALAR *pInput1 = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile1);
  MORPH_IDT_SCALAR *pInput2 = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile2);
  MORPH_IDT_SCALAR *pOutput = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(outTile);
  /* Get add struct parameters*/
  int32_t input1_offset         = XI_CNN_ADDA_GET_ZERO_POINT_IN1(pparams);
  int32_t input2_offset         = XI_CNN_ADDA_GET_ZERO_POINT_IN2(pparams);
  int32_t output_offset         = XI_CNN_ADDA_GET_ZERO_POINT_OUT(pparams);
  int32_t input1_multiplier     = XI_CNN_ADDA_GET_MULTIPLIER_IN1(pparams);
  int32_t input2_multiplier     = XI_CNN_ADDA_GET_MULTIPLIER_IN2(pparams);
  int32_t output_multiplier     = XI_CNN_ADDA_GET_MULTIPLIER_OUT(pparams);
  int32_t input1_shift          = XI_CNN_ADDA_GET_SHIFT_IN1(pparams);
  int32_t input2_shift          = XI_CNN_ADDA_GET_SHIFT_IN2(pparams);
  int32_t output_shift          = XI_CNN_ADDA_GET_SHIFT_OUT(pparams);
  int32_t output_activation_min =  XI_CNN_ADDA_GET_MIN_VAL(pparams);
  int32_t output_activation_max =  XI_CNN_ADDA_GET_MAX_VAL(pparams);
  const int32_t left_shift      = XI_CNN_ADDA_GET_LEFT_SHIFT(pparams);

  /* Input and Output data pointers */
  MORPH_IDT_SCALAR *restrict inp1Ptr, *restrict inp1Ptr2;
  MORPH_IDT_SCALAR *restrict inp2Ptr, *restrict inp2Ptr2;
  MORPH_IDT_SCALAR *restrict outPtr, *restrict outPtr2;

  MORPH_IDT_Nx8 *restrict pvecInp1;
  MORPH_IDT_Nx8 *restrict pvecInp2;
  MORPH_IDT_Nx8 *restrict pvecOut;
  MORPH_IDT_Nx8 *restrict pvecOut2;

  /* Input and Output data vectors */
  MORPH_IDT_Nx16 vecInp2, vecInp1, vecOut;
  MORPH_IDT_Nx16 vecInp3, vecInp4, vecOut2;

  int32_t tripcount  = outTiledim1Size * outTiledim2Size * outTiledim3Size;
  int32_t tripcount1 = outTiledim1Size * outTiledim2Size;

  int32_t x, y, z;
  valign va1, va2;
  valign vaStore  = IVP_ZALIGN();
  valign vaStore2 = IVP_ZALIGN();

  //input2 value broadcast case
  if(inData2Pitch0 == 0 )
  {
    // if the data is contiguous among all the directions for input1 , input2 and output
    if ((inData2Pitch1 == 0) && (inData2Pitch2 == 0) && \
        (outTiledim2Size * inData1Pitch1 == inData1Pitch2) && (outTiledim1Size == inData1Pitch1) && \
        (outTiledim2Size * outDataPitch1 == outDataPitch2) && (outTiledim1Size == outDataPitch1))
    {
      /* Input and Output data pointers */
      inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1);
      inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2);
      outPtr  = (MORPH_IDT_SCALAR *)(pOutput);
      /* vector and pointer to load and store values*/
      pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
      va1      = MORPH_OP_LOAD_NX8_PP(pvecInp1);
      pvecOut  = (MORPH_IDT_Nx8 *)(outPtr);

      /* Load Input 2*/
      vecInp2 = (MORPH_IDT_Nx8)(inp2Ptr[0]);
      /* Zero point addition */
      vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));

      for ( x = 0; x <= (tripcount - XCHAL_IVPN_SIMD_WIDTH); x += XCHAL_IVPN_SIMD_WIDTH)
      {
        /* load the data */
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va1, pvecInp1);
        /* Zero point addition */
        vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
        /* add operation */
        ADD32VALUES(vecInp1, vecInp2, vecOut);
        /* Store the data */
        MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
      }
      if(x < tripcount)
      {
        /* load the data */
        MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp1, va1, pvecInp1, (tripcount - x));
        /* Zero point addition */
        vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
        /* add operation */
        ADD32VALUES(vecInp1, vecInp2, vecOut);
        /* Store the data */
        MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (tripcount - x));
      }
      MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
    }
    else if((inData2Pitch1 == 0) && \
            (outTiledim1Size == inData1Pitch1) && \
            (outTiledim1Size == outDataPitch1))
    {
      for (z = 0; z < outTiledim3Size; z++)
      {
        /* Input and Output data pointers */
        inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1 + z * inData1Pitch2);
        inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2 + z * inData2Pitch2);
        outPtr = (MORPH_IDT_SCALAR *)(pOutput + z * outDataPitch2);

        pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
        va1 = MORPH_OP_LOAD_NX8_PP(pvecInp1);
        pvecOut = (MORPH_IDT_Nx8 *)(outPtr);

        /* Load Input 2*/
        vecInp2 = (MORPH_IDT_Nx8)(inp2Ptr[0]);
        /* Zero point addition */
        vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));

        for (x = 0; x <= (tripcount1 - XCHAL_IVPN_SIMD_WIDTH); x += XCHAL_IVPN_SIMD_WIDTH)
        {
          /* Load Input 1 */
          MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va1, pvecInp1);
          /* Zero point addition */
          vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
          /* add operation */
          ADD32VALUES(vecInp1, vecInp2, vecOut);
          /* Store the data */
          MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
        }
        if(x < tripcount1)
        {
          /* Load Input 1 */
          MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp1, va1, pvecInp1, tripcount1 - x);
          /* Zero point addition */
          vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
          /* add operation */
          ADD32VALUES(vecInp1, vecInp2, vecOut);
          /* Store the data */
          MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, tripcount1 - x);
        }
        MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
      }
    }
    else  // if the data is not contiguous along inner most dimension
    {
      for (z = 0; z < outTiledim3Size; z += 2)
      {
        int32_t remZ = XT_MIN(2, (outTiledim3Size - z)) - 1;

        for (y = 0; y < outTiledim2Size; y++)
        {
          /* Input and Output data pointers */
          inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1 + z * inData1Pitch2 + y * inData1Pitch1);
          inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2 + z * inData2Pitch2 + y * inData2Pitch1);
          outPtr = (MORPH_IDT_SCALAR *)(pOutput + z * outDataPitch2 + y * outDataPitch1);

          inp1Ptr2 = (MORPH_IDT_SCALAR *)(pInput1 + (z + remZ) * inData1Pitch2 + y * inData1Pitch1);
          inp2Ptr2 = (MORPH_IDT_SCALAR *)(pInput2 + (z + remZ) * inData2Pitch2 + y * inData2Pitch1);
          outPtr2 = (MORPH_IDT_SCALAR *)(pOutput + (z + remZ) * outDataPitch2 + y * outDataPitch1);

          /* vector and pointer to load and store values*/
          pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
          va1 = MORPH_OP_LOAD_NX8_PP(pvecInp1);

          pvecInp2 = (MORPH_IDT_Nx8 *)(inp1Ptr2);
          va2 = MORPH_OP_LOAD_NX8_PP(pvecInp2);

          pvecOut = (MORPH_IDT_Nx8 *)(outPtr);
          pvecOut2 = (MORPH_IDT_Nx8 *)(outPtr2);

          /* load the data 2 */
          vecInp2 = (MORPH_IDT_Nx8)(inp2Ptr[0]);
          vecInp4 = (MORPH_IDT_Nx8)(inp2Ptr2[0]);
          /* Zero point addition */
          vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
          vecInp4 = MORPH_OP_ADDNX16(vecInp4, (MORPH_IDT_Nx16)(input2_offset));

          for (x = 0; x <= (outTiledim1Size - XCHAL_IVPN_SIMD_WIDTH); x += XCHAL_IVPN_SIMD_WIDTH)
          {
            /* load the data 1 */
            MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va1, pvecInp1);
            MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp3, va2, pvecInp2);
            /* Zero point addition */
            vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
            vecInp3 = MORPH_OP_ADDNX16(vecInp3, (MORPH_IDT_Nx16)(input1_offset));
            /* add operation */
            ADD32VALUES(vecInp1, vecInp2, vecOut);
            ADD32VALUES(vecInp3, vecInp4, vecOut2);
            /* Store the data */
            MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
            MORPH_OP_IVP_SANX8_IP(vecOut2, vaStore2, pvecOut2);
          }
          if(x < outTiledim1Size)
          {
            /* load the data 1 */
            MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp1, va1, pvecInp1, (outTiledim1Size - x));
            MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp3, va2, pvecInp2, (outTiledim1Size - x));
            /* Zero point addition */
            vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
            vecInp3 = MORPH_OP_ADDNX16(vecInp3, (MORPH_IDT_Nx16)(input1_offset));
            /* add operation */
            ADD32VALUES(vecInp1, vecInp2, vecOut);
            ADD32VALUES(vecInp3, vecInp4, vecOut2);
            /* Store the data */
            MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (outTiledim1Size - x));
            MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut2, vaStore2, pvecOut2, (outTiledim1Size - x) * remZ);
          }
          MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
          MORPH_OP_FLUSH_SAPOSNX8(vaStore2, pvecOut2);
        }
      }
    }
  }
  //input1 value broadcast case
  else if(inData1Pitch0 == 0 )
  {
    // if the data is contiguous among all the directions for input1 , input2 and output
    if ((inData1Pitch1 == 0)&&(inData1Pitch2 == 0) && \
        (outTiledim2Size * inData2Pitch1 == inData2Pitch2) && (outTiledim1Size == inData2Pitch1) && \
        (outTiledim2Size * outDataPitch1 == outDataPitch2) && (outTiledim1Size == outDataPitch1))
    {
      /* Input and Output data pointers */
      inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1);
      inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2);
      outPtr = (MORPH_IDT_SCALAR *)(pOutput);

      pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr);
      va2 = MORPH_OP_LOAD_NX8_PP(pvecInp2);
      pvecOut = (MORPH_IDT_Nx8 *)(outPtr);

      /* Load Input 1 */
      vecInp1 = (MORPH_IDT_Nx8)(inp1Ptr[0]);
      /* Zero point addition */
      vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));

      for (x = 0; x <= (tripcount - XCHAL_IVPN_SIMD_WIDTH); x += XCHAL_IVPN_SIMD_WIDTH)
      {
        /* Load Input 2 */
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va2, pvecInp2);
        /* Zero point addition */
        vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
        /* add operation */
        ADD32VALUES(vecInp1, vecInp2, vecOut);
        /* Store the data */
        MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
      }
      if(x < tripcount)
      {
        /* Load Input 2*/
        MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp2, va2, pvecInp2, (tripcount - x));
        /* Zero point addition */
        vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
        /* add operation */
        ADD32VALUES(vecInp1, vecInp2, vecOut);
        /* Store the data */
        MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (tripcount - x));
      }
      MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
    }
    else if((inData1Pitch1 == 0) && \
            (outTiledim1Size == inData2Pitch1) && \
            (outTiledim1Size == outDataPitch1))
    {
      for (z = 0; z < outTiledim3Size; z++)
      {
        /* Input and Output data pointers */
        inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1 + z * inData1Pitch2);
        inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2 + z * inData2Pitch2);
        outPtr = (MORPH_IDT_SCALAR *)(pOutput + z * outDataPitch2);

        pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr);
        va2 = MORPH_OP_LOAD_NX8_PP(pvecInp2);
        pvecOut = (MORPH_IDT_Nx8 *)(outPtr);

        /* Load Input 1*/
        vecInp1 = (MORPH_IDT_Nx8)(inp1Ptr[0]);
        /* Zero point addition */
        vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));

        for (x = 0; x <= (tripcount1 - XCHAL_IVPN_SIMD_WIDTH); x += XCHAL_IVPN_SIMD_WIDTH)
        {
          /* Load Input 2 */
          MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va2, pvecInp2);
          /* Zero point addition */
          vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
          /* add operation */
          ADD32VALUES(vecInp1, vecInp2, vecOut);
          /* Store the data */
          MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
        }
        if(x < tripcount1)
        {
          /* Load Input 2 */
          MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp2, va2, pvecInp2, tripcount1 - x);
          /* Zero point addition */
          vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
          /* add operation */
          ADD32VALUES(vecInp1, vecInp2, vecOut);
          /* Store the data */
          MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, tripcount1 - x);
        }
        MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
      }
    }
    else
    {  // if the data is not contiguous along inner most dimension
      for (z = 0; z < outTiledim3Size; z += 2)
      {
        int32_t remZ = XT_MIN(2, (outTiledim3Size - z)) - 1;
        for (y = 0; y < outTiledim2Size; y++)
        {
          /* Input and Output data pointers */
          inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1 + z * inData1Pitch2 + y * inData1Pitch1);
          inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2 + z * inData2Pitch2 + y * inData2Pitch1);
          outPtr = (MORPH_IDT_SCALAR *)(pOutput + z * outDataPitch2 + y * outDataPitch1);

          inp1Ptr2 = (MORPH_IDT_SCALAR *)(pInput1 + (z + remZ) * inData1Pitch2 + y * inData1Pitch1);
          inp2Ptr2 = (MORPH_IDT_SCALAR *)(pInput2 + (z + remZ) * inData2Pitch2 + y * inData2Pitch1);
          outPtr2 = (MORPH_IDT_SCALAR *)(pOutput + (z + remZ) * outDataPitch2 + y * outDataPitch1);

          pvecInp1 = (MORPH_IDT_Nx8 *)(inp2Ptr);
          va1 = MORPH_OP_LOAD_NX8_PP(pvecInp1);

          pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr2);
          va2 = MORPH_OP_LOAD_NX8_PP(pvecInp2);

          pvecOut = (MORPH_IDT_Nx8 *)(outPtr);
          pvecOut2 = (MORPH_IDT_Nx8 *)(outPtr2);

          /* load the data 1 */
          vecInp1 = (MORPH_IDT_Nx8)(inp1Ptr[0]);
          vecInp3 = (MORPH_IDT_Nx8)(inp1Ptr2[0]);
          /* Zero point addition */
          vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
          vecInp3 = MORPH_OP_ADDNX16(vecInp3, (MORPH_IDT_Nx16)(input1_offset));

          for (x = 0; x <= (outTiledim1Size - XCHAL_IVPN_SIMD_WIDTH); x += XCHAL_IVPN_SIMD_WIDTH)
          {
            /* load the data 2 */
            MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va1, pvecInp1);
            MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp4, va2, pvecInp2);
            /* Zero point addition */
            vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
            vecInp4 = MORPH_OP_ADDNX16(vecInp4, (MORPH_IDT_Nx16)(input2_offset));
            /* add operation */
            ADD32VALUES(vecInp1, vecInp2, vecOut);
            ADD32VALUES(vecInp3, vecInp4, vecOut2);
            /* Store the data */
            MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
            MORPH_OP_IVP_SANX8_IP(vecOut2, vaStore2, pvecOut2);
          }
          if(x < outTiledim1Size)
          {
            /* load the data 2 */
            MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp2, va1, pvecInp1, (outTiledim1Size - x));
            MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp4, va2, pvecInp2, (outTiledim1Size - x));
            /* Zero point addition */
            vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
            vecInp4 = MORPH_OP_ADDNX16(vecInp4, (MORPH_IDT_Nx16)(input2_offset));
            /* add operation */
            ADD32VALUES(vecInp1, vecInp2, vecOut);
            ADD32VALUES(vecInp3, vecInp4, vecOut2);
            /* Store the data */
            MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (outTiledim1Size - x));
            MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut2, vaStore2, pvecOut2, (outTiledim1Size - x) * remZ);
          }
          MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
          MORPH_OP_FLUSH_SAPOSNX8(vaStore2, pvecOut2);
        }
      }
    }
  }
}

/******************* BroadCastDims2and3NoQuant ****************************/
/* Description : Optimized implementation of Broadcast and add            */
/*               functionality across 2nd and 3rd dimesion                */
/*               Here there is no quantization applied as the input and   */
/*               output scales are assumed to be the same                 */
/* Inputs      : Input tile and AddA_params                               */
/* Outputs     : XI Error Code                                            */
/* InOuts      : Output Tile                                              */
/* Assumptions : InData, outData is U8/S8                                 */
/**************************************************************************/
static void MAKE_NAME(BroadCastDims2and3NoQuant)(const xi_pTile3D inTile1,
                                          const xi_pTile3D inTile2,
                                          xi_pTile3D outTile,
                                          const xi_cnn_addA_params *pparams,
                                          int32_t inData1Pitch1,
                                          int32_t inData2Pitch1,
                                          int32_t inData1Pitch2,
                                          int32_t inData2Pitch2)
{
  /* Getting parameters from the tile structures */
  const int32_t  outTiledim1Size = XI_TILE3D_GET_DIM1(outTile);
  const int32_t  outTiledim2Size = XI_TILE3D_GET_DIM2(outTile);
  const int32_t  outTiledim3Size = XI_TILE3D_GET_DIM3(outTile);
  const int32_t  outDataPitch1   = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t  outDataPitch2   = XI_TILE3D_GET_DIM2_PITCH(outTile);
  /* Get Data Pointers */
  MORPH_IDT_SCALAR *pInput1 = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile1);
  MORPH_IDT_SCALAR *pInput2 = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile2);
  MORPH_IDT_SCALAR *pOutput = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(outTile);

  /* Get add struct parameters*/
  int32_t input1_offset         = XI_CNN_ADDA_GET_ZERO_POINT_IN1(pparams);
  int32_t input2_offset         = XI_CNN_ADDA_GET_ZERO_POINT_IN2(pparams);
  int32_t output_offset         = XI_CNN_ADDA_GET_ZERO_POINT_OUT(pparams);
  int32_t output_activation_min =  XI_CNN_ADDA_GET_MIN_VAL(pparams);
  int32_t output_activation_max =  XI_CNN_ADDA_GET_MAX_VAL(pparams);

  /* Input and Output data pointers */
  MORPH_IDT_SCALAR *restrict inp1Ptr;
  MORPH_IDT_SCALAR *restrict inp2Ptr;
  MORPH_IDT_SCALAR *restrict outPtr;
  MORPH_IDT_Nx8 *restrict pvecInp1;
  MORPH_IDT_Nx8 *restrict pvecInp2;
  MORPH_IDT_Nx8 *restrict pvecOut;
  MORPH_IDT_Nx16 vecInp2, vecInp1, vecOut;

  int32_t x, y, z;
  valign va1, va2, vaStore;
  vaStore = IVP_ZALIGN();
  MORPH_IDT_Nx16 vecOffset = input1_offset + input2_offset + output_offset;
  int32_t tripcount = outTiledim1Size * outTiledim2Size;

  if ((outTiledim1Size == inData1Pitch1) && \
      (outTiledim1Size == inData2Pitch1) && \
      (outTiledim1Size == outDataPitch1))
  {
    for (z = 0; z < outTiledim3Size; z++)
    {
      /* Input and Output data pointers */
      inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1 + z * inData1Pitch2);
      inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2 + z * inData2Pitch2);
      outPtr = (pOutput + z * outDataPitch2);

      /* vector and pointer to load and store values*/
      pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
      va1 = MORPH_OP_LOAD_NX8_PP(pvecInp1);

      pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr);
      va2 = MORPH_OP_LOAD_NX8_PP(pvecInp2);

      pvecOut = (MORPH_IDT_Nx8 *)(outPtr);

      for (x = 0; x <= (tripcount - XCHAL_IVPN_SIMD_WIDTH); x += XCHAL_IVPN_SIMD_WIDTH)
      {
        /* load the data */
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va1, pvecInp1);
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va2, pvecInp2);

        /* Add input data and pre-computed offset */
        MORPH_IDT_Nx16 vecInp = MORPH_OP_ADDNX16(vecInp1, vecInp2);
        vecOut = MORPH_OP_ADDNX16(vecInp, vecOffset);
        vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

        /* store the result */
        MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
      }
      if(x < tripcount)
      {
        /* load the data */
        MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp1, va1, pvecInp1, (tripcount - x));
        MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp2, va2, pvecInp2, (tripcount - x));

        /* Add input data and pre-computed offset */
        MORPH_IDT_Nx16 vecInp = MORPH_OP_ADDNX16(vecInp1, vecInp2);
        vecOut = MORPH_OP_ADDNX16(vecInp, vecOffset);
        vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

        /* store the result */
        MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (tripcount - x));
      }
      MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
    }
  }
  else   // data is not continuous along innermost dimension
  {
    for (x = 0; x < outTiledim1Size; x += XCHAL_IVPN_SIMD_WIDTH)
    {
      for (z = 0; z < outTiledim3Size; z++)
      {
        /* Input and Output data pointers */
        inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1 + z * inData1Pitch2 + x);
        inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2 + z * inData2Pitch2 + x);
        outPtr = (MORPH_IDT_SCALAR *)(pOutput + z * outDataPitch2 + x);

        /* vector and pointer to load and store values*/
        pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
        pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr);

        for (y = 0; y < outTiledim2Size; y++)
        {
          /* Load the data 1*/
          va1 = MORPH_OP_LOAD_NX8_PP(pvecInp1);
          MORPH_OP_ALIGN_LOAD_NX8_XP(vecInp1, va1, pvecInp1, inData1Pitch1);
          /* Load the data 2 */
          va2 = MORPH_OP_LOAD_NX8_PP(pvecInp2);
          MORPH_OP_ALIGN_LOAD_NX8_XP(vecInp2, va2, pvecInp2, inData2Pitch1);

          /* Add input data and pre-computed offset */
          MORPH_IDT_Nx16 vecInp = MORPH_OP_ADDNX16(vecInp1, vecInp2);
          vecOut = MORPH_OP_ADDNX16(vecInp, vecOffset);
          vecOut = IVP_MINNX16((xb_vecNx16)output_activation_max, IVP_MAXNX16(vecOut, (xb_vecNx16)output_activation_min));

          /* Store the result */
          pvecOut = (MORPH_IDT_Nx8 *)(outPtr);
          MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (outTiledim1Size - x));
          MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
          outPtr += outDataPitch1;
        }
      }
    }
  }
}


/******************* BroadCastDims2and3 ***********************************/
/* Description : Optimized implementation of Broadcast and add            */
/*               functionality across 2nd and 3rd dimesion                */
/* Inputs      : Input tile and AddA_params                               */
/* Outputs     : XI Error Code                                            */
/* InOuts      : Output Tile                                              */
/* Assumptions : InData, outData is U8/S8                                 */
/**************************************************************************/
static void MAKE_NAME(BroadCastDims2and3)(const xi_pTile3D inTile1,
                                          const xi_pTile3D inTile2,
                                          xi_pTile3D outTile,
                                          const xi_cnn_addA_params *pparams,
                                          int32_t inData1Pitch1,
                                          int32_t inData2Pitch1,
                                          int32_t inData1Pitch2,
                                          int32_t inData2Pitch2)
{
  /* Getting parameters from the tile structures */
  const int32_t  outTiledim1Size = XI_TILE3D_GET_DIM1(outTile);
  const int32_t  outTiledim2Size = XI_TILE3D_GET_DIM2(outTile);
  const int32_t  outTiledim3Size = XI_TILE3D_GET_DIM3(outTile);
  const int32_t  outDataPitch1   = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t  outDataPitch2   = XI_TILE3D_GET_DIM2_PITCH(outTile);
  /* Get Data Pointers */
  MORPH_IDT_SCALAR *pInput1 = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile1);
  MORPH_IDT_SCALAR *pInput2 = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile2);
  MORPH_IDT_SCALAR *pOutput = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(outTile);

  /* Get add struct parameters*/
  int32_t input1_offset         = XI_CNN_ADDA_GET_ZERO_POINT_IN1(pparams);
  int32_t input2_offset         = XI_CNN_ADDA_GET_ZERO_POINT_IN2(pparams);
  int32_t output_offset         = XI_CNN_ADDA_GET_ZERO_POINT_OUT(pparams);
  int32_t input1_multiplier     = XI_CNN_ADDA_GET_MULTIPLIER_IN1(pparams);
  int32_t input2_multiplier     = XI_CNN_ADDA_GET_MULTIPLIER_IN2(pparams);
  int32_t output_multiplier     = XI_CNN_ADDA_GET_MULTIPLIER_OUT(pparams);
  int32_t input1_shift          = XI_CNN_ADDA_GET_SHIFT_IN1(pparams);
  int32_t input2_shift          = XI_CNN_ADDA_GET_SHIFT_IN2(pparams);
  int32_t output_shift          = XI_CNN_ADDA_GET_SHIFT_OUT(pparams);
  int32_t output_activation_min =  XI_CNN_ADDA_GET_MIN_VAL(pparams);
  int32_t output_activation_max =  XI_CNN_ADDA_GET_MAX_VAL(pparams);
  const int32_t left_shift      = XI_CNN_ADDA_GET_LEFT_SHIFT(pparams);

  /* Input and Output data pointers */
  MORPH_IDT_SCALAR *restrict inp1Ptr;
  MORPH_IDT_SCALAR *restrict inp2Ptr;
  MORPH_IDT_SCALAR *restrict outPtr;
  MORPH_IDT_Nx8 *restrict pvecInp1;
  MORPH_IDT_Nx8 *restrict pvecInp2;
  MORPH_IDT_Nx8 *restrict pvecOut;
  MORPH_IDT_Nx16 vecInp2, vecInp1, vecOut;

  int32_t x, y, z;
  valign va1, va2, vaStore;
  vaStore = IVP_ZALIGN();

  int32_t tripcount = outTiledim1Size * outTiledim2Size;

  if ((outTiledim1Size == inData1Pitch1) && \
      (outTiledim1Size == inData2Pitch1) && \
      (outTiledim1Size == outDataPitch1))
  {
    for (z = 0; z < outTiledim3Size; z++)
    {
      /* Input and Output data pointers */
      inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1 + z * inData1Pitch2);
      inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2 + z * inData2Pitch2);
      outPtr = (pOutput + z * outDataPitch2);

      /* vector and pointer to load and store values*/
      pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
      va1 = MORPH_OP_LOAD_NX8_PP(pvecInp1);

      pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr);
      va2 = MORPH_OP_LOAD_NX8_PP(pvecInp2);

      pvecOut = (MORPH_IDT_Nx8 *)(outPtr);

      for (x = 0; x <= (tripcount - XCHAL_IVPN_SIMD_WIDTH); x += XCHAL_IVPN_SIMD_WIDTH)
      {
        /* load the data */
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp1, va1, pvecInp1);
        MORPH_OP_ALIGN_LOAD_Nx8_IP(vecInp2, va2, pvecInp2);
        /*Zero point addition*/
        vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
        vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
        /* add operation */
        ADD32VALUES(vecInp1, vecInp2, vecOut);
        /* store the result */
        MORPH_OP_IVP_SANX8_IP(vecOut, vaStore, pvecOut);
      }
      if(x < tripcount)
      {
        /* load the data */
        MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp1, va1, pvecInp1, (tripcount - x));
        MORPH_OP_ALIGN_LOAD_Nx8_VARIABLE(vecInp2, va2, pvecInp2, (tripcount - x));
        /*Zero point addition*/
        vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
        vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
        /* add operation */
        ADD32VALUES(vecInp1, vecInp2, vecOut);
        /* store the result */
        MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (tripcount - x));
      }
      MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
    }
  }
  else   // data is not continuous along innermost dimension
  {
    for (x = 0; x < outTiledim1Size; x += XCHAL_IVPN_SIMD_WIDTH)
    {
      for (z = 0; z < outTiledim3Size; z++)
      {
        /* Input and Output data pointers */
        inp1Ptr = (MORPH_IDT_SCALAR *)(pInput1 + z * inData1Pitch2 + x);
        inp2Ptr = (MORPH_IDT_SCALAR *)(pInput2 + z * inData2Pitch2 + x);
        outPtr = (MORPH_IDT_SCALAR *)(pOutput + z * outDataPitch2 + x);

        /* vector and pointer to load and store values*/
        pvecInp1 = (MORPH_IDT_Nx8 *)(inp1Ptr);
        pvecInp2 = (MORPH_IDT_Nx8 *)(inp2Ptr);

        for (y = 0; y < outTiledim2Size; y++)
        {
          /* Load the data 1*/
          va1 = MORPH_OP_LOAD_NX8_PP(pvecInp1);
          MORPH_OP_ALIGN_LOAD_NX8_XP(vecInp1, va1, pvecInp1, inData1Pitch1);
          /* Load the data 2 */
          va2 = MORPH_OP_LOAD_NX8_PP(pvecInp2);
          MORPH_OP_ALIGN_LOAD_NX8_XP(vecInp2, va2, pvecInp2, inData2Pitch1);
          /*Zero point addition*/
          vecInp1 = MORPH_OP_ADDNX16(vecInp1, (MORPH_IDT_Nx16)(input1_offset));
          vecInp2 = MORPH_OP_ADDNX16(vecInp2, (MORPH_IDT_Nx16)(input2_offset));
          /* Add operation */
          ADD32VALUES(vecInp1, vecInp2, vecOut);
          /* Store the result */
          pvecOut = (MORPH_IDT_Nx8 *)(outPtr);
          MORPH_OP_ALIGN_STORE_NX8_VARIABLE(vecOut, vaStore, pvecOut, (outTiledim1Size - x));
          MORPH_OP_FLUSH_SAPOSNX8(vaStore, pvecOut);
          outPtr += outDataPitch1;
        }
      }
    }
  }
}

/******************* xiBroadcastAddA3D_U8 *********************************/
/******************* xiBroadcastAddA3D_S8 *********************************/
/******************* xiBroadcastAddA3D ************************************/
/* Description : Optimized implementation of Broadcast and add            */
/*               functionality                                            */
/* Inputs      : Input tile and AddA_params                               */
/* Outputs     : XI Error Code                                            */
/* InOuts      : Output Tile                                              */
/* Assumptions : InData, outData is U8/S8                                 */
/**************************************************************************/
XI_ERR_TYPE MAKE_NAME(xiBroadcastAddA3D)(const xi_pTile3D inTile1,
                                         const xi_pTile3D inTile2,
                                         xi_pTile3D outTile,
                                         const xi_cnn_addA_params *pparams)
{
  XI_ERROR_CHECKS()
  {
    MORPH_IDT_CHECK(inTile1);
    MORPH_IDT_CHECK(inTile2);
    MORPH_IDT_CHECK(outTile);
    XI_CHECK_POINTER(pparams);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile1);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile2);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
#if (XCHAL_VISION_TYPE >= 7)
    if (XI_TILE3D_GET_DATA_ORDER(inTile1) == XI_ID16WH)
    {
      XI_CHECK_ERROR(!((XI_TILE3D_GET_DIM1(inTile1) != XI_TILE3D_GET_DIM1(inTile2)) && \
                     ((XI_TILE3D_GET_DIM1(inTile1) >> 4 != 1) && (XI_TILE3D_GET_DIM1(inTile2) >> 4 != 1))), XI_ERR_DATASIZE, \
                     "\nInput tile1 dim1= %d, Input tile2 dim1 = %d\nIf the size of a first dimension in both input tiles are not equal,\
                     then one of them must be equal to 16", XI_TILE3D_GET_DIM1(inTile1), XI_TILE3D_GET_DIM1(inTile2));
    }
    else
    {
      XI_CHECK_ERROR(!((XI_TILE3D_GET_DIM1(inTile1) != XI_TILE3D_GET_DIM1(inTile2)) && \
                     ((XI_TILE3D_GET_DIM1(inTile1) != 1) && (XI_TILE3D_GET_DIM1(inTile2) != 1))), XI_ERR_DATASIZE, \
                     "\nInput tile1 dim1= %d, Input tile2 dim1 = %d\nIf the size of a first dimension in both input tiles are not equal,\
                     then one of them must be equal to 1", XI_TILE3D_GET_DIM1(inTile1), XI_TILE3D_GET_DIM1(inTile2));
    }
#else
    XI_CHECK_ERROR(!((XI_TILE3D_GET_DIM1(inTile1) != XI_TILE3D_GET_DIM1(inTile2)) && \
                     ((XI_TILE3D_GET_DIM1(inTile1) != 1) && (XI_TILE3D_GET_DIM1(inTile2) != 1))), XI_ERR_DATASIZE, \
                     "\nInput tile1 dim1= %d, Input tile2 dim1 = %d\nIf the size of a first dimension in both input tiles are not equal,\
                     then one of them must be equal to 1", XI_TILE3D_GET_DIM1(inTile1), XI_TILE3D_GET_DIM1(inTile2));
#endif
    XI_CHECK_ERROR(!((XI_TILE3D_GET_DIM2(inTile1) != XI_TILE3D_GET_DIM2(inTile2)) && \
                   ((XI_TILE3D_GET_DIM2(inTile1) != 1) && (XI_TILE3D_GET_DIM2(inTile2) != 1))), XI_ERR_DATASIZE,\
                   "\nInput tile1 dim2= %d, Input tile2 dim2 = %d\nIf the size of a second dimension in both input tiles are not equal,\
                   then one of them must be equal to 1", XI_TILE3D_GET_DIM2(inTile1), XI_TILE3D_GET_DIM2(inTile2));
    XI_CHECK_ERROR(!((XI_TILE3D_GET_DIM3(inTile1) != XI_TILE3D_GET_DIM3(inTile2)) && \
                   ((XI_TILE3D_GET_DIM3(inTile1) != 1) && (XI_TILE3D_GET_DIM3(inTile2) != 1))), XI_ERR_DATASIZE,\
                   "Input tile1 dim3= %d, Input tile2 dim3 = %d\nIf the sizes of a third dimension in both input tiles are not equal,\
                   then one of them must be equal to 1", XI_TILE3D_GET_DIM3(inTile1), XI_TILE3D_GET_DIM3(inTile2));
    XI_CHECK_ERROR(!(MAX2(XI_TILE3D_GET_DIM1(inTile1), XI_TILE3D_GET_DIM1(inTile2)) != XI_TILE3D_GET_DIM1(outTile)), XI_ERR_DATASIZE,\
                   "\nMax dim1 size of input tiles = %d, Output tile dim1 = %d\nThe size of the first dimension in output tile must be equal to the maximum of sizes of \
                   first dimension in both input tiles", MAX2(XI_TILE3D_GET_DIM1(inTile1), XI_TILE3D_GET_DIM1(inTile2)), XI_TILE3D_GET_DIM1(outTile));
    XI_CHECK_ERROR(!(MAX2(XI_TILE3D_GET_DIM2(inTile1), XI_TILE3D_GET_DIM2(inTile2)) != XI_TILE3D_GET_DIM2(outTile)), XI_ERR_DATASIZE, \
                   "\nMax dim2 size of input tiles = %d, Output tile dim2 = %d\nThe size of the second dimension in output tile must be equal to the maximum of sizes of\
                   second dimension in both input tiles", MAX2(XI_TILE3D_GET_DIM2(inTile1), XI_TILE3D_GET_DIM2(inTile2)), XI_TILE3D_GET_DIM2(outTile));
    XI_CHECK_ERROR(!(MAX2(XI_TILE3D_GET_DIM3(inTile1), XI_TILE3D_GET_DIM3(inTile2)) != XI_TILE3D_GET_DIM3(outTile)), XI_ERR_DATASIZE, \
                   "\nMax dim3 size of input tiles = %d, Output tile dim3 = %d\nThe size of the third dimension in output tile must be equal to the maximum of sizes of\
                   third dimension in both input tiles", MAX2(XI_TILE3D_GET_DIM3(inTile1), XI_TILE3D_GET_DIM3(inTile2)), XI_TILE3D_GET_DIM3(outTile));
    XI_CHECK_ERROR((XI_CNN_ADDA_GET_MIN_VAL(pparams) <= XI_CNN_ADDA_GET_MAX_VAL(pparams)), XI_ERR_BADARG, \
                   "\nmin limit = %d, max limit = %d\nMax limit is less than min limit", XI_CNN_ADDA_GET_MIN_VAL(pparams), XI_CNN_ADDA_GET_MAX_VAL(pparams));
    XI_CHECK_ERROR((XI_TILE3D_GET_DATA_ORDER(inTile1) == XI_TILE3D_GET_DATA_ORDER(outTile)) && (XI_TILE3D_GET_DATA_ORDER(inTile2) == XI_TILE3D_GET_DATA_ORDER(outTile)),
                   XI_ERR_BADARG, "\nData order of inputTile1 = %d, Data order of inputTile2 = %d, OutputTile order = %d\nData Order of InputTiles and OutputTile should be same",\
                   XI_TILE3D_GET_DATA_ORDER(inTile1), XI_TILE3D_GET_DATA_ORDER(inTile2), XI_TILE3D_GET_DATA_ORDER(outTile));
    XI_CHECK_ERROR((XI_CNN_ADDA_GET_SHIFT_IN1(pparams) >= 0), XI_ERR_NORM,\
                   "\nInput1 shift = %d, Input1 shift should be greater than or equal to zero", XI_CNN_ADDA_GET_SHIFT_IN1(pparams));
    XI_CHECK_ERROR((XI_CNN_ADDA_GET_SHIFT_IN1(pparams) < 32), XI_ERR_NORM, \
                   "\nInput1 shift = %d, Input1 shift should be less than 32", XI_CNN_ADDA_GET_SHIFT_IN1(pparams));
    XI_CHECK_ERROR((XI_CNN_ADDA_GET_SHIFT_IN2(pparams) >= 0), XI_ERR_NORM, \
                   "\nInput2 shift = %d, Input2 shift should be greater than or equal to zero", XI_CNN_ADDA_GET_SHIFT_IN2(pparams));
    XI_CHECK_ERROR((XI_CNN_ADDA_GET_SHIFT_IN2(pparams) < 32), XI_ERR_NORM, \
                   "\nInput2 shift = %d, Input2 shift should be less than 32", XI_CNN_ADDA_GET_SHIFT_IN2(pparams));
    XI_CHECK_ERROR((XI_CNN_ADDA_GET_SHIFT_OUT(pparams) >= 0), XI_ERR_NORM, \
                   "\nOutput shift = %d, Output shift should be greater than or equal to zero", XI_CNN_ADDA_GET_SHIFT_OUT(pparams));
    XI_CHECK_ERROR((XI_CNN_ADDA_GET_SHIFT_OUT(pparams) < 32), XI_ERR_NORM, \
                    "\nOutput shift = %d, Output shift should be less than 32", XI_CNN_ADDA_GET_SHIFT_OUT(pparams));
    XI_CHECK_ERROR((XI_CNN_ADDA_GET_LEFT_SHIFT(pparams) >= 0), XI_ERR_NORM, \
                   "\nLeft shift = %d, Left shift should be greater than or equal to zero", XI_CNN_ADDA_GET_LEFT_SHIFT(pparams));
    XI_CHECK_ERROR((XI_CNN_ADDA_GET_LEFT_SHIFT(pparams) < 24), XI_ERR_NORM, \
                   "\nLeft shift = %d, Left shift should be less than 24", XI_CNN_ADDA_GET_LEFT_SHIFT(pparams));
  }
#if (XCHAL_VISION_TYPE >= 7)
  if (XI_TILE3D_GET_DATA_ORDER(inTile1) == XI_ID16WH)
  {
    MAKE_NAME(broadcastAddA3D_ID16WH)(inTile1, inTile2, outTile, pparams);
    return XI_ERROR_STATUS();
  }
#endif

  /* Getting parameters from the tile structures */
  const int32_t  inTile1dim1Size = XI_TILE3D_GET_DIM1(inTile1);
  const int32_t  inTile1dim2Size = XI_TILE3D_GET_DIM2(inTile1);
  const int32_t  inTile1dim3Size = XI_TILE3D_GET_DIM3(inTile1);
  const int32_t  inTile2dim1Size = XI_TILE3D_GET_DIM1(inTile2);
  const int32_t  inTile2dim2Size = XI_TILE3D_GET_DIM2(inTile2);
  const int32_t  inTile2dim3Size = XI_TILE3D_GET_DIM3(inTile2);
  int32_t inData1Pitch0 = 1;
  int32_t inData1Pitch1 = XI_TILE3D_GET_DIM1_PITCH(inTile1);
  int32_t inData1Pitch2 = XI_TILE3D_GET_DIM2_PITCH(inTile1);
  int32_t inData2Pitch0 = 1;
  int32_t inData2Pitch1 = XI_TILE3D_GET_DIM1_PITCH(inTile2);
  int32_t inData2Pitch2 = XI_TILE3D_GET_DIM2_PITCH(inTile2);
  uint8_t qflagVal = XI_CNN_ADDA_GET_QUANTIZATION_FLAG(pparams);

  int BroadCastDims = 0;


  if(inTile1dim2Size != inTile2dim2Size)
  {
    if(inTile1dim2Size == 1)
    {
      inData1Pitch1 = 0;
    }
    else
    {
      inData2Pitch1 = 0;
    }
    BroadCastDims = 2;
  }

  if(inTile1dim3Size != inTile2dim3Size)
  {
    if(inTile1dim3Size == 1)
    {
      inData1Pitch2 = 0;
    }
    else
    {
      inData2Pitch2 = 0;
    }
    BroadCastDims = 3;
  }

  if(inTile1dim1Size != inTile2dim1Size)
  {
    if(inTile1dim1Size == 1)
    {
      inData1Pitch0 = 0;
    }
    else
    {
      inData2Pitch0 = 0;
    }
    BroadCastDims = 1;
  }

  switch(BroadCastDims)
  {
  case 1:
    if (qflagVal == 0)
    {
      MAKE_NAME(BroadCastDims1NoQuant)(inTile1, inTile2, outTile, pparams, inData1Pitch0, inData2Pitch0, inData1Pitch1, inData2Pitch1, inData1Pitch2, inData2Pitch2);
    }
    else
    {
      MAKE_NAME(BroadCastDims1)(inTile1, inTile2, outTile, pparams, inData1Pitch0, inData2Pitch0, inData1Pitch1, inData2Pitch1, inData1Pitch2, inData2Pitch2);
    }
   break;
  case 2:
  case 3:
    if (qflagVal == 0)
    {
      MAKE_NAME(BroadCastDims2and3NoQuant)(inTile1, inTile2, outTile, pparams, inData1Pitch1, inData2Pitch1, inData1Pitch2, inData2Pitch2);
    }
    else
    {
      MAKE_NAME(BroadCastDims2and3)(inTile1, inTile2, outTile, pparams, inData1Pitch1, inData2Pitch1, inData1Pitch2, inData2Pitch2);
    }
    break;
  default: MAKE_NAME(xiAddA3D)(inTile1, inTile2, outTile, pparams);
    break;
  }
  return XI_ERROR_STATUS();
}

#endif //if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))
