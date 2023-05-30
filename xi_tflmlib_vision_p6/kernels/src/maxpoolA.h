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
#undef MORPH_IDT_2Nx8
#undef MORPH_OFFSET_PTR_2NX8
#undef MORPH_OP_LOAD_2NX8_PP
#undef MORPH_OP_LOAD_2NX8_IP
#undef MORPH_OP_LOAD_2NX8_XP
#undef MORPH_MIN_VALUE
#undef MORPH_OP_UNPK2NX8_0
#undef MORPH_OP_UNPK2NX8_1
#undef MORPH_OP_MAX2NX8
#undef MORPH_OP_MIN2NX8
#undef MORPH_OP_MOV2NX8T
#undef MORPH_OP_MOV2NX8_FROMNX16
#undef MORPH_OP_SAV2NX8_XP
#undef MORPH_OP_FLUSH_SAPOS2NX8

#define MAKE_NAME_IMPL(name, MORPH_FNAME_SPECIFIER_IDT)  name ## _ ## MORPH_FNAME_SPECIFIER_IDT
#define MAKE_NAME(name)                                  MAKE_NAME_IMPL(name, U8_DWH)
#define MORPH_IDT_CHECK                     XI_CHECK_TILE3D_U8
#define MORPH_IDT_SCALAR                    uint8_t
#define MORPH_IDT_2Nx8                      xb_vec2Nx8U
#define MORPH_OFFSET_PTR_2NX8               OFFSET_PTR_2NX8U
#define MORPH_OP_LOAD_2NX8_PP               IVP_LA2NX8U_PP
#define MORPH_OP_LOAD_2NX8_IP               IVP_LA2NX8U_IP
#define MORPH_OP_LOAD_2NX8_XP               IVP_LA2NX8U_XP
#define MORPH_MIN_VALUE                     (xb_vec2Nx8U)0
#define MORPH_OP_UNPK2NX8_0                 IVP_UNPKU2NX8_0
#define MORPH_OP_UNPK2NX8_1                 IVP_UNPKU2NX8_1
#define MORPH_OP_MAX2NX8                    IVP_MAXU2NX8U
#define MORPH_OP_MIN2NX8                    IVP_MINU2NX8U
#define MORPH_OP_MOV2NX8T                   IVP_MOV2NX8UT
#define MORPH_OP_MOV2NX8_FROMNX16           IVP_MOV2NX8U_FROMNX16
#define MORPH_OP_SAV2NX8_XP                 IVP_SAV2NX8U_XP
#define MORPH_OP_FLUSH_SAPOS2NX8            IVP_SAPOS2NX8U_FP

#elif INPUT_DATA_TYPE == SIGNED8BIT

#undef MAKE_NAME_IMPL
#undef MAKE_NAME
#undef MORPH_IDT_CHECK
#undef MORPH_IDT_SCALAR
#undef MORPH_IDT_2Nx8
#undef MORPH_OFFSET_PTR_2NX8
#undef MORPH_OP_LOAD_2NX8_PP
#undef MORPH_OP_LOAD_2NX8_IP
#undef MORPH_OP_LOAD_2NX8_XP
#undef MORPH_MIN_VALUE
#undef MORPH_OP_UNPK2NX8_0
#undef MORPH_OP_UNPK2NX8_1
#undef MORPH_OP_MAX2NX8
#undef MORPH_OP_MIN2NX8
#undef MORPH_OP_MOV2NX8T
#undef MORPH_OP_MOV2NX8_FROMNX16
#undef MORPH_OP_SAV2NX8_XP
#undef MORPH_OP_FLUSH_SAPOS2NX8

#define MAKE_NAME_IMPL(name, MORPH_FNAME_SPECIFIER_IDT)  name ## _ ## MORPH_FNAME_SPECIFIER_IDT
#define MAKE_NAME(name)                                  MAKE_NAME_IMPL(name, S8_DWH)
#define MORPH_IDT_CHECK                     XI_CHECK_TILE3D_S8
#define MORPH_IDT_SCALAR                    int8_t
#define MORPH_IDT_2Nx8                      xb_vec2Nx8
#define MORPH_OFFSET_PTR_2NX8               OFFSET_PTR_2NX8
#define MORPH_OP_LOAD_2NX8_PP               IVP_LA2NX8_PP
#define MORPH_OP_LOAD_2NX8_IP               IVP_LA2NX8_IP
#define MORPH_OP_LOAD_2NX8_XP               IVP_LA2NX8_XP
#define MORPH_MIN_VALUE                     (xb_vec2Nx8)SCHAR_MIN
#define MORPH_OP_UNPK2NX8_0                 IVP_UNPKS2NX8_0
#define MORPH_OP_UNPK2NX8_1                 IVP_UNPKS2NX8_1
#define MORPH_OP_MAX2NX8                    IVP_MAX2NX8
#define MORPH_OP_MIN2NX8                    IVP_MIN2NX8
#define MORPH_OP_MOV2NX8T                   IVP_MOV2NX8T
#define MORPH_OP_MOV2NX8_FROMNX16           IVP_MOV2NX8_FROMNX16
#define MORPH_OP_SAV2NX8_XP                 IVP_SAV2NX8_XP
#define MORPH_OP_FLUSH_SAPOS2NX8            IVP_SAPOS2NX8_FP

#endif

/******************************** xiMaxPoolQuantizeA_U8_DWH *********************************/
/******************************** xiMaxPoolQuantizeA_S8_DWH *********************************/
/******************************** xiMaxPoolQuantizeA*****************************************/
/* Description  : P6 optimized implementation for MxN MaxPool for Android NN                */
/* Inputs       : Input Data Tile, CNN Pooling Parameters, Frame dimensions                 */
/* InOut        : Output Data Tile                                                          */
/* Output       : XI Error Code                                                             */
/* Assumptions  : Input and Output Data are U8/S8                                           */
/*                Input and Output are in DWH format                                        */
/*                Implementation do not have restrictions on kernel size and                */
/*                stride parameters                                                         */
/*                Number of channels in input and output are same                           */
/********************************************************************************************/
XI_ERR_TYPE MAKE_NAME(xiMaxPoolQuantizeA)(const xi_pTile3D inTile, xi_pTile3D outTile, const xi_cnn_maxpoolA_params *param)
{
  XI_ERROR_CHECKS()
  {
    XI_CHECK_TILE3D(inTile);
    XI_CHECK_TILE3D(outTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_POINTER(param);
    MORPH_IDT_CHECK(inTile);
    MORPH_IDT_CHECK(outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(inTile, outTile);
    XI_CHECK_TILE3D_DATA_ORDER(inTile, XI_DWH);
    XI_CHECK_TILE3D_DATA_ORDER(outTile, XI_DWH);
    XI_CHECK_ERROR((XI_CNN_MAXPOOLA_GET_SHIFT_OUT(param) > 0), XI_ERR_SCALE, \
                   "\nOutput shift = %d, Output shift should be positive", XI_CNN_MAXPOOLA_GET_SHIFT_OUT(param));
    XI_CHECK_ERROR((XI_CNN_MAXPOOLA_GET_KERNELWIDTH(param) > 0) && \
                   (XI_CNN_MAXPOOLA_GET_KERNELHEIGHT(param) > 0), XI_ERR_KSIZE, \
                   "\nKernel width = %hhu, kernel height = %hhu\nKernel height and width should be greater than 0", \
                   XI_CNN_MAXPOOLA_GET_KERNELWIDTH(param), XI_CNN_MAXPOOLA_GET_KERNELHEIGHT(param));
    XI_CHECK_ERROR((XI_CNN_MAXPOOLA_GET_STRIDEX(param) > 0) && \
                   (XI_CNN_MAXPOOLA_GET_STRIDEY(param) > 0), XI_ERR_BADARG, \
                   "\nStrideX = %hhu, StrideY = %hhu\nStrideX and StrideY should be greater than 0", \
                   XI_CNN_MAXPOOLA_GET_STRIDEX(param), XI_CNN_MAXPOOLA_GET_STRIDEY(param));
    XI_CHECK_CONSISTENCY_POOL_DWH(inTile, outTile, param);
  }

  /*  Read Tile Parameters */
  int32_t outW = XI_TILE3D_GET_DIM2(outTile);
  int32_t outH = XI_TILE3D_GET_DIM3(outTile);
  int32_t numCh = XI_TILE3D_GET_DIM1(outTile);
  int32_t inDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(inTile);
  int32_t inDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(inTile);

  int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);

  /* Read CNN Pooling Parameters */
  uint8_t kernelWidthU = XI_CNN_MAXPOOLA_GET_KERNELWIDTH(param);
  uint8_t kernelHeightU = XI_CNN_MAXPOOLA_GET_KERNELHEIGHT(param);
  uint8_t strideX = XI_CNN_MAXPOOLA_GET_STRIDEX(param);
  uint8_t strideY = XI_CNN_MAXPOOLA_GET_STRIDEY(param);
  uint8_t leftEdgeFlagU = XI_CNN_MAXPOOLA_GET_LEFTEDGE_FLAG(param);
  uint8_t topEdgeFlagU = XI_CNN_MAXPOOLA_GET_TOPEDGE_FLAG(param);

  /* Initialize Data Pointers for Input and Output Tiles */
  MORPH_IDT_SCALAR *pInData = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile);
  MORPH_IDT_SCALAR *pOutData = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(outTile);

  int32_t activation_min = XI_CNN_MAXPOOLA_GET_MIN_VAL(param);
  int32_t activation_max = XI_CNN_MAXPOOLA_GET_MAX_VAL(param);
  int32_t input_zero_point = XI_CNN_MAXPOOLA_GET_ZEROPTINPUT(param);
  int32_t output_zero_point = XI_CNN_MAXPOOLA_GET_ZEROPTOUTPUT(param);
  int32_t output_multiplier = XI_CNN_MAXPOOLA_GET_MULTIPLIER_OUT(param);
  int32_t output_shift = XI_CNN_MAXPOOLA_GET_SHIFT_OUT(param);
  int32_t left_shift = XI_CNN_MAXPOOLA_GET_LEFT_SHIFT(param);

  MORPH_IDT_2Nx8* restrict pdvecOut;
  MORPH_IDT_2Nx8* restrict pdvecIn;
  MORPH_IDT_2Nx8 dvecData1, dvecMax;
  xb_vecNx16 vecMax0, vecMax1;
  xb_vecN_2x32v hvecOut0, hvecOut1;
  xb_vecN_2x32v hvecInp1_0, hvecInp1_1;
  xb_vecNx16 vecOut0, vecOut1;
  /* Find the top and left edge based on egdeFlag */
  uint8_t minLeftEdgeU, minTopEdgeU;
  if (kernelWidthU % 2 != 0)
  {
    minLeftEdgeU = (kernelWidthU / 2);
  }
  else
  {
    minLeftEdgeU = leftEdgeFlagU ? (kernelWidthU / 2) : ((kernelWidthU / 2) - 1);
  }
  if (kernelHeightU % 2 != 0)
  {
    minTopEdgeU = (kernelHeightU / 2);
  }
  else
  {
    minTopEdgeU = topEdgeFlagU ? (kernelHeightU / 2) : ((kernelHeightU / 2) - 1);
  }

  /* Move pointer to the start of the data (including edge) */
  pInData = &pInData[-(minTopEdgeU * inDataPitch2 + minLeftEdgeU * inDataPitch1)];

  /* Loop Variables */
  int32_t x, y, ch, k;
  int32_t vectorizationWidth = 2 * XCHAL_IVPN_SIMD_WIDTH;
  for (ch = 0; ch < numCh; ch += vectorizationWidth)                   // Along channels
  {
    /* Handling cases where number of channels not a multiple of 64 */
    int remCh = XT_MIN(numCh - ch, vectorizationWidth);
    for (y = 0; y < outH; y += 1)               // Along output height
    {
      for (x = 0; x < outW; x += 1)                    // Along output width
      {
        /* Input Data Pointer */
        MORPH_IDT_SCALAR* pSrc = pInData + ch + y * inDataPitch2 * strideY + \
          x * inDataPitch1 * strideX;
        /* output Data Pointer */
        MORPH_IDT_SCALAR* pOut = pOutData + ch + y * outDataPitch2 + x * outDataPitch1;
        dvecMax = MORPH_MIN_VALUE;

        xb_vecN_2x32v hvecLaneIdxAddrOffKw = inDataPitch1;
        xb_vecN_2x32v hvecLaneIdxAddrOffKh = 0;
        int32_t index;
        /* outRow 0 , outCol 0 */
        pdvecIn = (MORPH_IDT_2Nx8 *)pSrc;
        valign vaInData;
        vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn);
        MORPH_OP_LOAD_2NX8_XP(dvecData1, vaInData, pdvecIn, inDataPitch1);
        dvecMax = MORPH_OP_MAX2NX8(dvecData1, dvecMax);

        /* First entry in the receptive field is already loaded.
        * Only the remaining kernelHeightU * kernelWidthU - 1 entries need to be loaded.
        */
        /* Finding the maximum of values in the receptive field  */
        for (k = 0; k < kernelHeightU * kernelWidthU - 1; k++)                         // Along kernelHeight * kernelWidth
        {
            /* Condition checks performed to get the Input Pointer Offsets      */
          /* after combining the Kernel Width and Height Loops                */
          vboolN_2 vbN_2 = IVP_EQN_2X32(hvecLaneIdxAddrOffKw, kernelWidthU * inDataPitch1);
          /* hvecLaneIdx will be reset to zero after every kWidth */
          hvecLaneIdxAddrOffKw = IVP_MOVN_2X32T(0, hvecLaneIdxAddrOffKw, vbN_2);
          /* Increment kH index every kW times */
          hvecLaneIdxAddrOffKh = IVP_MOVN_2X32T((inDataPitch2 - ((kernelWidthU - 1) * inDataPitch1)), inDataPitch1, vbN_2);
          /* Extract index */
          index = IVP_EXTRN_2X32(hvecLaneIdxAddrOffKh, 0);
          /* Extracting Input address offsets */
          hvecLaneIdxAddrOffKw = IVP_ADDN_2X32(hvecLaneIdxAddrOffKw, inDataPitch1);
          /* Pointers for Input Data Loads */
          pSrc = (pSrc + index);
          /* Loading input data and finding the max vector*/
          /* outRow 0 , outCol 0 */
          pdvecIn = (MORPH_IDT_2Nx8 *)pSrc;
          vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn);
          MORPH_OP_LOAD_2NX8_XP(dvecData1, vaInData, pdvecIn, inDataPitch1);
          dvecMax = MORPH_OP_MAX2NX8(dvecData1, dvecMax);
        }                           //end of (k = 0; k < kernelHeightU * kernelWidthU ; k++)
        vecMax0 = MORPH_OP_UNPK2NX8_0(dvecMax);
        vecMax1 = MORPH_OP_UNPK2NX8_1(dvecMax);
        //compute quantized multiply MAX values
        vecMax0 = vecMax0 - (xb_vecNx16)input_zero_point;
        vecMax1 = vecMax1 - (xb_vecNx16)input_zero_point;
        hvecInp1_0 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0(vecMax0, (xb_vecN_2x32v)output_multiplier), (31 - left_shift));
        hvecInp1_1 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_1(vecMax0, (xb_vecN_2x32v)output_multiplier), (31 - left_shift));
        hvecOut0 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, IVP_ABSN_2X32(hvecInp1_0)), output_shift);
        hvecOut0 = IVP_MULSGNN_2X32(hvecInp1_0, hvecOut0);
        hvecOut0 = hvecOut0 + (xb_vecN_2x32v)output_zero_point;
        hvecOut1 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, IVP_ABSN_2X32(hvecInp1_1)), output_shift);
        hvecOut1 = IVP_MULSGNN_2X32(hvecInp1_1, hvecOut1);
        hvecOut1 = hvecOut1 + (xb_vecN_2x32v)output_zero_point;
        hvecOut0 = IVP_MINN_2X32((xb_vecN_2x32v)activation_max, IVP_MAXN_2X32(hvecOut0, (xb_vecN_2x32v)activation_min));
        hvecOut1 = IVP_MINN_2X32((xb_vecN_2x32v)activation_max, IVP_MAXN_2X32(hvecOut1, (xb_vecN_2x32v)activation_min));
        vecOut0 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecOut1, hvecOut0, IVP_SELI_16B_INTERLEAVE_1_EVEN));

        hvecInp1_0 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0(vecMax1, (xb_vecN_2x32v)output_multiplier), (31 - left_shift));
        hvecInp1_1 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_1(vecMax1, (xb_vecN_2x32v)output_multiplier), (31 - left_shift));
        hvecOut0 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, IVP_ABSN_2X32(hvecInp1_0)), output_shift);
        hvecOut0 = IVP_MULSGNN_2X32(hvecInp1_0, hvecOut0);
        hvecOut0 = hvecOut0 + (xb_vecN_2x32v)output_zero_point;
        hvecOut1 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, IVP_ABSN_2X32(hvecInp1_1)), output_shift);
        hvecOut1 = IVP_MULSGNN_2X32(hvecInp1_1, hvecOut1);
        hvecOut1 = hvecOut1 + (xb_vecN_2x32v)output_zero_point;
        hvecOut0 = IVP_MINN_2X32((xb_vecN_2x32v)activation_max, IVP_MAXN_2X32(hvecOut0, (xb_vecN_2x32v)activation_min));
        hvecOut1 = IVP_MINN_2X32((xb_vecN_2x32v)activation_max, IVP_MAXN_2X32(hvecOut1, (xb_vecN_2x32v)activation_min));
        vecOut1 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecOut1, hvecOut0, IVP_SELI_16B_INTERLEAVE_1_EVEN));

        MORPH_IDT_2Nx8 dvecOut1 = IVP_SEL2NX8I(MORPH_OP_MOV2NX8_FROMNX16(vecOut1), MORPH_OP_MOV2NX8_FROMNX16(vecOut0), IVP_SELI_8B_INTERLEAVE_1_EVEN);

        /* Storing average value */
        pdvecOut = (MORPH_IDT_2Nx8 *)pOut;
        valign vaOutData = IVP_ZALIGN();
        MORPH_OP_SAV2NX8_XP(dvecOut1, vaOutData, pdvecOut, remCh);
        MORPH_OP_FLUSH_SAPOS2NX8(vaOutData, pdvecOut);
      }       //for (x = 0; x < outDataWidth; x += 1)
    }         //for (y = 0; y < outDataHeight; y += 1)
  }           //for (z = 0; z < numCh; z++)

  return(XI_ERROR_STATUS());
}

/***********************************************************************************/
/* Description  : P6 implementation for 2x2 MaxPool DWH                            */
/* Inputs       : Input Data Tile, CNN Pooling Parameters                          */
/* Inout        : Output Data Tile                                                 */
/* Output       : XI Error Code                                                    */
/* Assumptions  : Input and Output Data are U8/S8....                              */
/*                Input and output are in DWH format                               */
/*                Implementation do not have restrictions on kernel size and       */
/*                stride parameters                                                */
/*                Number of channels in input and output are same                  */
/***********************************************************************************/
//xiMaxPoolA3D_2x2_U8_DWH
//xiMaxPoolA3D_2x2_S8_DWH
void MAKE_NAME(maxPoolA3D_2x2)(const xi_pTile3D inTile, xi_pTile3D outTile, const xi_cnn_maxpoolA_params *param)
{
  /*  Read Tile Parameters */
  int32_t outW = XI_TILE3D_GET_DIM2(outTile);
  int32_t outH = XI_TILE3D_GET_DIM3(outTile);
  int32_t numCh = XI_TILE3D_GET_DIM1(outTile);

  int32_t inDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(inTile);
  int32_t inDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(inTile);

  int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);

  /* Read CNN Pooling Parameters */
  uint8_t kernelWidthU = 2;
  uint8_t kernelHeightU = 2;
  uint8_t strideU = XI_CNN_MAXPOOLA_GET_STRIDE(param);
  uint8_t leftEdgeFlagU = XI_CNN_MAXPOOLA_GET_LEFTEDGE_FLAG(param);
  uint8_t topEdgeFlagU = XI_CNN_MAXPOOLA_GET_TOPEDGE_FLAG(param);

  /* Min - Max values */
  int32_t activation_min = XI_CNN_MAXPOOLA_GET_MIN_VAL(param);
  int32_t activation_max = XI_CNN_MAXPOOLA_GET_MAX_VAL(param);

  /* Initialize Data Pointers for Input and Output Tiles */
  MORPH_IDT_SCALAR *pInData = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile);
  MORPH_IDT_SCALAR *pOutData = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(outTile);

  MORPH_IDT_2Nx8* restrict pdvecOut;
  MORPH_IDT_2Nx8* restrict pdvecIn1;
  MORPH_IDT_2Nx8* restrict pdvecIn2;
  MORPH_IDT_2Nx8 dvecMax1, dvecMax2, dvecMax00, dvecMax10;
  MORPH_IDT_2Nx8 dvecData1, dvecData2;
  valign vaOutData = IVP_ZALIGN();

  /* Find the top and left edge based on egdeFlag */
  uint8_t minLeftEdgeU = leftEdgeFlagU ? (kernelWidthU / 2) : ((kernelWidthU / 2) - 1);
  uint8_t minTopEdgeU = topEdgeFlagU ? (kernelHeightU / 2) : ((kernelHeightU / 2) - 1);

  /* Move pointer to the start of the data (including edge) */
  pInData = &pInData[-(minTopEdgeU * inDataPitch2 + minLeftEdgeU * inDataPitch1)];

  /* Loop Variables */
  int32_t x, y, ch;
  int32_t remY;
  int32_t vectorizationWidth = 2 * XCHAL_IVPN_SIMD_WIDTH;

  /* This implementation is specific to kernel size = 2 and stride = 2      */
  /* Here vectorization is done along channels. Output height loop is       */
  /* unrolled by a factor of 2. Kernel width and kernel height loops        */
  /* are completely unrolled                                                */

  for (ch = 0; ch < numCh; ch += vectorizationWidth)             // Along channels
  {
    /* Handling cases where number of channels not a multiple of 64 */
    int remCh = XT_MIN(numCh - ch, vectorizationWidth);
    for (y = 0; y < outH; y += 2)   // Along output height
    {
      /* For handling odd number of rows */
      remY = XT_MIN(2, outH - y) - 1;

      /* Input Data Pointer */
      MORPH_IDT_SCALAR* pSrc = pInData + ch + y * inDataPitch2 * strideU;
      pdvecIn1 = (MORPH_IDT_2Nx8 *)pSrc;
      pdvecIn2 = MORPH_OFFSET_PTR_2NX8(pdvecIn1, strideU, inDataPitch2 * remY, 0);

      /* Output Data Pointer */
      MORPH_IDT_SCALAR* pOut = pOutData + ch + y * outDataPitch2;

      for (x = 0; x < outW; x++)   // Along output width
      {
        /* Loading input data and comparing it with maximum value            */
        /* Single iteration produces two output vectors.                     */
        /* 4 vectors( (ky,kx) = (0,0),(0,1),(1,0) and(1,1)) corresponding    */
        /* to each output vector are loaded and each vector is compared      */
        /*  with dvecMax                                                     */

        /* ky = 0 */
        /* outRow 0 , outCol 0 */
        valign vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn1);
        MORPH_OP_LOAD_2NX8_XP(dvecMax1, vaInData, pdvecIn1, inDataPitch1);
        vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn1);
        MORPH_OP_LOAD_2NX8_XP(dvecData1, vaInData, pdvecIn1, (inDataPitch2 - inDataPitch1));
        dvecMax1 = MORPH_OP_MAX2NX8(dvecMax1, dvecData1);

        /* outRow 1 , outCol 0 */
        vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn2);
        MORPH_OP_LOAD_2NX8_XP(dvecMax2, vaInData, pdvecIn2, inDataPitch1);
        vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn2);
        MORPH_OP_LOAD_2NX8_XP(dvecData2, vaInData, pdvecIn2, (inDataPitch2 - inDataPitch1));
        dvecMax2 = MORPH_OP_MAX2NX8(dvecMax2, dvecData2);


        /* ky = 1 */
        /* outRow 0, outCol 0 */
        vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn1);
        MORPH_OP_LOAD_2NX8_XP(dvecData1, vaInData, pdvecIn1, inDataPitch1);
        dvecMax1 = MORPH_OP_MAX2NX8(dvecMax1, dvecData1);
        vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn1);
        MORPH_OP_LOAD_2NX8_XP(dvecData1, vaInData, pdvecIn1, ((inDataPitch1 * (strideU - 1)) - inDataPitch2));
        dvecMax1 = MORPH_OP_MAX2NX8(dvecMax1, dvecData1);

        /* outRow 1 , outCol 0  */
        vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn2);
        MORPH_OP_LOAD_2NX8_XP(dvecData2, vaInData, pdvecIn2, inDataPitch1);
        dvecMax2 = MORPH_OP_MAX2NX8(dvecMax2, dvecData2);
        vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn2);
        MORPH_OP_LOAD_2NX8_XP(dvecData2, vaInData, pdvecIn2, ((inDataPitch1 * (strideU - 1)) - inDataPitch2));
        dvecMax2 = MORPH_OP_MAX2NX8(dvecMax2, dvecData2);


        /* Storing maximum value */
        /* outRow 0 , outCol 0 */
        dvecMax00 = MORPH_OP_MIN2NX8((MORPH_IDT_2Nx8)activation_max, MORPH_OP_MAX2NX8(dvecMax1, (MORPH_IDT_2Nx8)activation_min));
        pdvecOut = (MORPH_IDT_2Nx8 *)pOut;
        MORPH_OP_SAV2NX8_XP(dvecMax00, vaOutData, pdvecOut, remCh);
        MORPH_OP_FLUSH_SAPOS2NX8(vaOutData, pdvecOut);

        /* outRow 1 , outCol 0 */
        dvecMax10 = MORPH_OP_MIN2NX8((MORPH_IDT_2Nx8)activation_max, MORPH_OP_MAX2NX8(dvecMax2, (MORPH_IDT_2Nx8)activation_min));
        pdvecOut = (MORPH_IDT_2Nx8 *)(pOut + outDataPitch2 * remY);
        MORPH_OP_SAV2NX8_XP(dvecMax10, vaOutData, pdvecOut, remCh * remY);
        MORPH_OP_FLUSH_SAPOS2NX8(vaOutData, pdvecOut);

        /*Updating output pointer */
        pOut += outDataPitch1;
      } //for (x = 0; x < outW ; x++)
    }   //for (y = 0; y < outH; y += 2)
  }     //for (ch = 0; ch < numCh; ch += vectorizationWidth)
}

/***********************************************************************************/
/* Description  : P6 implementation for 3x3 MaxPool DWH with stride = 1,2          */
/* Inputs       : Input Data Tile, CNN Pooling Parameters                          */
/* Inout        : Output Data Tile                                                 */
/* Output       : XI Error Code                                                    */
/* Assumptions  : Input and Output Data are U8/S8                                  */
/*                Kernel size is 3x3                                               */
/*                Input and output are in DWH format                               */
/*                Number of channels in input and output are same                  */
/***********************************************************************************/
//xiMaxPoolA3D_3x3_U8_DWH
//xiMaxPoolA3D_3x3_S8_DWH
void MAKE_NAME(maxPoolA3D_3x3)(const xi_pTile3D inTile, xi_pTile3D outTile, const xi_cnn_maxpoolA_params *param)
{
  /*  Read Tile Parameters */
  int32_t outW = XI_TILE3D_GET_DIM2(outTile);
  int32_t outH = XI_TILE3D_GET_DIM3(outTile);
  int32_t numCh = XI_TILE3D_GET_DIM1(outTile);

  int32_t inDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(inTile);
  int32_t inDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(inTile);

  int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);

  /* Read CNN Pooling Parameters */
  uint8_t kernelWidthU = 3;
  uint8_t kernelHeightU = 3;
  uint8_t strideU = XI_CNN_MAXPOOLA_GET_STRIDE(param);

  /* Min - Max values */
  int32_t activation_min = XI_CNN_MAXPOOLA_GET_MIN_VAL(param);
  int32_t activation_max = XI_CNN_MAXPOOLA_GET_MAX_VAL(param);

  /* Initialize Data Pointers for Input and Output Tiles */
  MORPH_IDT_SCALAR *pInData = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile);
  MORPH_IDT_SCALAR *pOutData = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(outTile);

  MORPH_IDT_2Nx8* restrict pdvecOut;
  MORPH_IDT_2Nx8* restrict pdvecIn;
  MORPH_IDT_2Nx8* restrict pdvecIn1;
  MORPH_IDT_2Nx8* restrict pdvecIn2;
  MORPH_IDT_2Nx8 dvecMax1, dvecMax2, dvecMax00, dvecMax10;
  MORPH_IDT_2Nx8 dvecData1, dvecData2;
  valign vaOutData = IVP_ZALIGN();

  /* Move pointer to the start of the data (including edge) */
  pInData = &pInData[-((kernelHeightU / 2) * inDataPitch2 + (kernelWidthU / 2) * inDataPitch1)];

  /* Loop Variables */
  int32_t x, y, ch;
  int32_t remY;
  int32_t vectorizationWidth = 2 * XCHAL_IVPN_SIMD_WIDTH;

  /* This implementation is specific to kernel size = 3 and stride= 1 or 2  */
  /* Here vectorization is done along channels. Output height loop is       */
  /* unrolled by a factor of 2. Kernel width and kernel height loops        */
  /* are completely unrolled                                                */

  /*To handle load operations in the presence of stride */
  vbool2N dvbStride1 = IVP_LT2NX8(strideU, 2); //All 1s if stride = 1 otherwise all 0s
  int32_t remKy = strideU - 1;            // Scalar to handle the ky = 4 load for stride1 case

  for (ch = 0; ch < numCh; ch += vectorizationWidth)             // Along channels
  {
    /* Handling cases where number of channels not a multiple of 64 */
    int remCh = XT_MIN(numCh - ch, vectorizationWidth);
    for (y = 0; y < outH; y += 2)   // Along output height
    {
      /* For handling odd number of rows */
      remY = XT_MIN(2, outH - y) - 1;

      /* Input Data Pointer */
      MORPH_IDT_SCALAR* pSrc = pInData + ch + y * inDataPitch2 * strideU;
      pdvecIn = (MORPH_IDT_2Nx8 *)pSrc;
      pdvecIn1 = pdvecIn;
      pdvecIn2 = MORPH_OFFSET_PTR_2NX8(pdvecIn1, kernelHeightU, inDataPitch2 * remY, 0);
      /* Output Data Pointer */
      MORPH_IDT_SCALAR* pOut = pOutData + ch + y * outDataPitch2;

      for (x = 0; x < outW; x++)   // Along output width
      {
        /* Loading input data and comparing it with maximum value.                  */
        /* In a single iteration of loop two outputs corresponding to               */
        /* (outRow 0 , outCol 0) and (outRow 1, outCol 0 ) are generated.           */
        /* 5 rows (3 loads each) of data are loaded ky = 0, 1, 2, 3, 4.             */
        /* For stride = 1, ky = 0 , 1 ,2 contributes to (outRow 0 , outCol 0) and   */
        /* ky = 1, 2, 3 contributes to (outRow 1 , outCol 0). Common loads are ky = 1, 2.  */
        /* For stride = 2, ky = 0 ,1 ,2 contributes to (outRow 0 , outCol 0) and           */
        /* ky = 2, 3, 4 contributes to (outRow 1 , outCol 0) . Common loads are  ky = 2.   */
        /* For stride 1 case, ky = 4 is not needed this case is handled using the          */
        /* boolean vector dvbStride1. Also For stride 2 case, ky=1 does not contribute to  */
        /* (outRow 1 , outCol 0). This case is also handled by boolean vector dvbStride1.  */

        /* ky = 0 *//* outRow 0 , outCol 0 */
        valign vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn1);
        MORPH_OP_LOAD_2NX8_XP(dvecMax1, vaInData, pdvecIn1, inDataPitch1);
        vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn1);
        MORPH_OP_LOAD_2NX8_XP(dvecData1, vaInData, pdvecIn1, inDataPitch1);
        dvecMax1 = MORPH_OP_MAX2NX8(dvecMax1, dvecData1);
        vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn1);
        MORPH_OP_LOAD_2NX8_XP(dvecData1, vaInData, pdvecIn1, (inDataPitch2 - 2 * inDataPitch1));
        dvecMax1 = MORPH_OP_MAX2NX8(dvecMax1, dvecData1);

        /*ky = 1 */ /* outRow 0, outCol 0(stride =1,2)  and outRow 1 , outCol 0(stride =1)*/
        vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn1);
        MORPH_OP_LOAD_2NX8_XP(dvecData1, vaInData, pdvecIn1, inDataPitch1);
        dvecMax1 = MORPH_OP_MAX2NX8(dvecMax1, dvecData1);
        dvecMax2 = dvecData1;
        vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn1);
        MORPH_OP_LOAD_2NX8_XP(dvecData1, vaInData, pdvecIn1, inDataPitch1);
        dvecMax1 = MORPH_OP_MAX2NX8(dvecMax1, dvecData1);
        dvecMax2 = MORPH_OP_MAX2NX8(dvecMax2, dvecData1);
        vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn1);
        MORPH_OP_LOAD_2NX8_XP(dvecData1, vaInData, pdvecIn1, (inDataPitch2 - 2 * inDataPitch1));
        dvecMax1 = MORPH_OP_MAX2NX8(dvecMax1, dvecData1);
        dvecMax2 = MORPH_OP_MAX2NX8(dvecMax2, dvecData1);

        /*ky = 2 */ /*Common input loads for outRow 0 , outCol 0 and outRow 1 , outCol 0 */
        vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn1);
        MORPH_OP_LOAD_2NX8_XP(dvecData1, vaInData, pdvecIn1, inDataPitch1);
        dvecMax1 = MORPH_OP_MAX2NX8(dvecMax1, dvecData1);
        dvecMax2 = MORPH_OP_MOV2NX8T(dvecMax2, dvecData1, dvbStride1);
        dvecMax2 = MORPH_OP_MAX2NX8(dvecMax2, dvecData1);
        vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn1);
        MORPH_OP_LOAD_2NX8_XP(dvecData1, vaInData, pdvecIn1, inDataPitch1);
        dvecMax1 = MORPH_OP_MAX2NX8(dvecMax1, dvecData1);
        dvecMax2 = MORPH_OP_MAX2NX8(dvecMax2, dvecData1);
        vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn1);
        MORPH_OP_LOAD_2NX8_IP(dvecData1, vaInData, pdvecIn1);
        dvecMax1 = MORPH_OP_MAX2NX8(dvecMax1, dvecData1);
        dvecMax2 = MORPH_OP_MAX2NX8(dvecMax2, dvecData1);

        /*ky = 3*/ /* outRow 1 , outCol 0 */
        vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn2);
        MORPH_OP_LOAD_2NX8_XP(dvecData1, vaInData, pdvecIn2, inDataPitch1);
        dvecMax2 = MORPH_OP_MAX2NX8(dvecMax2, dvecData1);
        vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn2);
        MORPH_OP_LOAD_2NX8_XP(dvecData1, vaInData, pdvecIn2, inDataPitch1);
        dvecMax2 = MORPH_OP_MAX2NX8(dvecMax2, dvecData1);
        vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn2);
        MORPH_OP_LOAD_2NX8_XP(dvecData1, vaInData, pdvecIn2, (inDataPitch2 * remKy - 2 * inDataPitch1));
        dvecMax2 = MORPH_OP_MAX2NX8(dvecMax2, dvecData1);

        /*ky = 4*/ /* outRow 1 , outCol 0 (stride =2)*/
        vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn2);
        MORPH_OP_LOAD_2NX8_XP(dvecData2, vaInData, pdvecIn2, inDataPitch1 * remKy);
        dvecMax2 = MORPH_OP_MAX2NX8(dvecMax2, dvecData2);
        vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn2);
        MORPH_OP_LOAD_2NX8_XP(dvecData2, vaInData, pdvecIn2, inDataPitch1 * remKy);
        dvecMax2 = MORPH_OP_MAX2NX8(dvecMax2, dvecData2);
        vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn2);
        MORPH_OP_LOAD_2NX8_IP(dvecData2, vaInData, pdvecIn2);
        dvecMax2 = MORPH_OP_MAX2NX8(dvecMax2, dvecData2);

        /* Updating Input vectors */
        pdvecIn = MORPH_OFFSET_PTR_2NX8(pdvecIn, inDataPitch1, strideU, 0);
        pdvecIn1 = pdvecIn;
        pdvecIn2 = MORPH_OFFSET_PTR_2NX8(pdvecIn1, kernelHeightU, inDataPitch2 * remY, 0);

        /* Storing maximum value */
        /* outRow 0 , outCol 0 */
        dvecMax00 = MORPH_OP_MIN2NX8((MORPH_IDT_2Nx8)activation_max, MORPH_OP_MAX2NX8(dvecMax1, (MORPH_IDT_2Nx8)activation_min));
        pdvecOut = (MORPH_IDT_2Nx8 *)pOut;
        MORPH_OP_SAV2NX8_XP(dvecMax00, vaOutData, pdvecOut, remCh);
        MORPH_OP_FLUSH_SAPOS2NX8(vaOutData, pdvecOut);

        /* outRow 1 , outCol 0 */
        dvecMax10 = MORPH_OP_MIN2NX8((MORPH_IDT_2Nx8)activation_max, MORPH_OP_MAX2NX8(dvecMax2, (MORPH_IDT_2Nx8)activation_min));
        pdvecOut = (MORPH_IDT_2Nx8 *)(pOut + outDataPitch2 * remY);
        MORPH_OP_SAV2NX8_XP(dvecMax10, vaOutData, pdvecOut, remCh * remY);
        MORPH_OP_FLUSH_SAPOS2NX8(vaOutData, pdvecOut);

        /*Updating output pointer */
        pOut += outDataPitch1;
      } //for (x = 0; x < outW ; x++)
    }   //for (y = 0; y < outH; y += 2)
  }     //for (ch = 0; ch < numCh; ch += vectorizationWidth)
}

/****************************** xiMaxPoolA3D_MxN_S8_DWH ****************************/
/****************************** xiMaxPoolA3D_MxN_U8_DWH ****************************/
/* Description  : P6 implementation for MxN MaxPool DWH                            */
/*                Calls the general implementation of MxN MaxPool                  */
/* Inputs       : Input Data Tile, CNN Pooling Parameters                          */
/* Inout        : Output Data Tile                                                 */
/* Output       : XI Error Code                                                    */
/* Assumptions  : Input and Output Data are U8/S8                                  */
/*                Implementation do not have restrictions on kernel size           */
/*                and stride parameters                                            */
/*                Input and output are in DWH format                               */
/*                Number of channels in input and output are same                  */
/***********************************************************************************/
XI_ERR_TYPE MAKE_NAME(xiMaxPoolA3D_MxN)(const xi_pTile3D inTile, xi_pTile3D outTile, const xi_cnn_maxpoolA_params *param)
{
  XI_ERROR_CHECKS()
  {
    XI_CHECK_TILE3D(inTile);
    XI_CHECK_TILE3D(outTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_POINTER(param);
    MORPH_IDT_CHECK(inTile);
    MORPH_IDT_CHECK(outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(inTile, outTile);
    XI_CHECK_TILE3D_DATA_ORDER(inTile, XI_DWH);
    XI_CHECK_TILE3D_DATA_ORDER(outTile, XI_DWH);
    XI_CHECK_ERROR(((XI_CNN_MAXPOOLA_GET_STRIDEX(param) > 0) && \
                   XI_CNN_MAXPOOLA_GET_STRIDEY(param) > 0), XI_ERR_BADARG, \
                   "\nStrideX= %hhu, StrideY = %hhu\nStride along the height and width should be greater than 0", \
                   XI_CNN_MAXPOOLA_GET_STRIDEX(param), XI_CNN_MAXPOOLA_GET_STRIDEY(param));
    XI_CHECK_ERROR((XI_CNN_MAXPOOLA_GET_KERNELWIDTH(param) > 0) && \
                   (XI_CNN_MAXPOOLA_GET_KERNELHEIGHT(param) > 0), XI_ERR_KSIZE, \
                   "\nKernel width = %hhu, Kernel height = %hhu\nKernel height and width should be greater than 0", \
                   XI_CNN_MAXPOOLA_GET_KERNELWIDTH(param), XI_CNN_MAXPOOLA_GET_KERNELHEIGHT(param));
    XI_CHECK_CONSISTENCY_POOL_DWH(inTile, outTile, param);
  }

  /* Stride */
  uint8_t strideX = XI_CNN_MAXPOOLA_GET_STRIDEX(param);
  uint8_t strideY = XI_CNN_MAXPOOLA_GET_STRIDEY(param);
  uint8_t kernelWidth = XI_CNN_MAXPOOLA_GET_KERNELWIDTH(param);
  uint8_t kernelHeight = XI_CNN_MAXPOOLA_GET_KERNELHEIGHT(param);

  MORPH_IDT_2Nx8* restrict pdvecIn;
  MORPH_IDT_2Nx8* restrict pdvecIn1;
  MORPH_IDT_2Nx8* restrict pdvecIn2;
  MORPH_IDT_2Nx8* restrict pdvecIn3;
  MORPH_IDT_2Nx8* restrict pdvecIn4;
  MORPH_IDT_2Nx8* restrict pdvecOut;

  /* Better optimization for small size kernels */
  if ((strideX == strideY) && (kernelWidth <= 16) && (kernelHeight <= 16))
  {
    /* Calling the maxPool3D function specific to 2x2 kernel or 3x3 kernel  */
    if (XI_CNN_MAXPOOLA_GET_KERNELWIDTH(param) == 2 && \
      XI_CNN_MAXPOOLA_GET_KERNELHEIGHT(param) == 2)
    {
      MAKE_NAME(maxPoolA3D_2x2)(inTile, outTile, param);
      return(XI_ERROR_STATUS());
    }
    else if (XI_CNN_MAXPOOLA_GET_KERNELWIDTH(param) == 3 && \
      XI_CNN_MAXPOOLA_GET_KERNELHEIGHT(param) == 3)
    {
      MAKE_NAME(maxPoolA3D_3x3)(inTile, outTile, param);
      return(XI_ERROR_STATUS());
    }

    /*  Read Tile Parameters */
    int32_t outW = XI_TILE3D_GET_DIM2(outTile);
    int32_t outH = XI_TILE3D_GET_DIM3(outTile);
    int32_t numCh = XI_TILE3D_GET_DIM1(outTile);

    int32_t inDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(inTile);
    int32_t inDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(inTile);

    int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
    int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);

    /* Read CNN Pooling Parameters */
    uint8_t strideU = strideX;
    uint8_t leftEdgeFlagU = XI_CNN_MAXPOOLA_GET_LEFTEDGE_FLAG(param);
    uint8_t topEdgeFlagU = XI_CNN_MAXPOOLA_GET_TOPEDGE_FLAG(param);

    /* Min - Max values */
    int32_t activation_min = XI_CNN_MAXPOOLA_GET_MIN_VAL(param);
    int32_t activation_max = XI_CNN_MAXPOOLA_GET_MAX_VAL(param);

    /* Initialize Data Pointers for Input and Output Tiles */
    MORPH_IDT_SCALAR *pInData = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile);
    MORPH_IDT_SCALAR *pOutData = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(outTile);
    MORPH_IDT_2Nx8 dvecMax1, dvecMax2, dvecMax3, dvecMax4;
    MORPH_IDT_2Nx8 dvecMax00, dvecMax01, dvecMax10, dvecMax11;
    MORPH_IDT_2Nx8 dvecData1, dvecData2, dvecData3, dvecData4;
    valign vaOutData = IVP_ZALIGN();

    /* Find the top and left edge based on egdeFlag and kernelwidth/height in pooling_param */
    uint8_t minLeftEdgeU, minTopEdgeU;
    if (kernelWidth % 2 != 0)
    {
      minLeftEdgeU = (kernelWidth / 2);
    }
    else
    {
      minLeftEdgeU = leftEdgeFlagU ? (kernelWidth / 2) : ((kernelWidth / 2) - 1);
    }
    if (kernelHeight % 2 != 0)
    {
      minTopEdgeU = (kernelHeight / 2);
    }
    else
    {
      minTopEdgeU = topEdgeFlagU ? (kernelHeight / 2) : ((kernelHeight / 2) - 1);
    }
    /* Move pointer to the start of the data (including edge) */
    pInData = &pInData[-(minTopEdgeU * inDataPitch2 + minLeftEdgeU * inDataPitch1)];

    /* Loop Variables */
    int32_t x, y, ch, k;
    int32_t remX, remY;
    int32_t vectorizationWidth = 2 * XCHAL_IVPN_SIMD_WIDTH;

    /* Vectorization is along channels. Output width and output height are unrolled    */
    /* by 2. Kernel height and width and height loops are combined together            */
    /* In this case the address offsets for input  need to be derived from             */
    /* vector registers. These vector registers are initialized as follows             */

    xb_vecN_2x32v hvecInAddrOffInit = IVP_PACKVRNRN_2X64W(IVP_MULHN_2X16X32_1 \
      ((xb_vecNx16)inDataPitch1, IVP_SEQN_2X32()), 16);

    for (ch = 0; ch < numCh; ch += vectorizationWidth)             // Along channels
    {
      /* Handling cases where number of channels not a multiple of 64 */
      int remCh = XT_MIN(numCh - ch, vectorizationWidth);
      for (y = 0; y < outH; y += 2)   // Along output height
      {
        /* For handling odd number of rows */
        remY = XT_MIN(2, outH - y) - 1;
        for (x = 0; x < outW; x += 2)  // Along output width
        {
          /* For handling odd number of columns */
          remX = XT_MIN(2, outW - x) - 1;

          /* Input Data Pointer */
          MORPH_IDT_SCALAR* pSrc = pInData + ch + y * inDataPitch2 * strideU + x * inDataPitch1 * strideU;

          /* output Data Pointer */
          MORPH_IDT_SCALAR* pOut = pOutData + ch + y * outDataPitch2 + x * outDataPitch1;

          /*  Initializing the maxVal with the first entry in the receptive field */
          /* outRow 0 , outCol 0 */
          pdvecIn = (MORPH_IDT_2Nx8 *)pSrc;
          valign vaInData; vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn);
          MORPH_OP_LOAD_2NX8_XP(dvecMax1, vaInData, pdvecIn, strideU * inDataPitch1 * remX);

          /* outRow 0 , outCol 1 */
          vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn);
          MORPH_OP_LOAD_2NX8_XP(dvecMax2, vaInData, pdvecIn, strideU * (inDataPitch2 * remY - inDataPitch1 * remX));

          /* outRow 1 , outCol 0 */
          vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn);
          MORPH_OP_LOAD_2NX8_XP(dvecMax3, vaInData, pdvecIn, strideU * inDataPitch1 * remX);

          /* outRow 1 , outCol 1 */
          vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn);
          MORPH_OP_LOAD_2NX8_IP(dvecMax4, vaInData, pdvecIn);


          /* First entry in the receptive field is already loaded.
          * Only the remaining kernelHeightU * kernelWidthU - 1 entries need to be loaded.
          * So hvecLaneIdx is initialized as 1 */
          xb_vecN_2x32v hvecLaneIdx = 1;
          xb_vecN_2x32v hvecInAddrOff = hvecInAddrOffInit;
          int32_t index, inAddrOff;

          /* Finding the maximum value in the receptive field  */
          for (k = 0; k < kernelHeight * kernelWidth - 1; k++)     // Along kernelHeight * kernelWidth
          {
            /* Condition checks performed to get the Input Pointer Offsets      */
            /* after combining the Kernel Width and Height Loops                */
            vboolN_2 vbN_2 = IVP_EQN_2X32(hvecLaneIdx, kernelWidth);
            /* hvecLaneIdx will be reset to zero after every kWidth */
            hvecLaneIdx = IVP_MOVN_2X32T(0, hvecLaneIdx, vbN_2);
            /* InPitch added after every kWidth */
            IVP_ADDN_2X32T(hvecInAddrOff, hvecInAddrOff, inDataPitch2, vbN_2);
            index = IVP_EXTRN_2X32(hvecLaneIdx, 0);
            /* Extracting Input address offsets */
            inAddrOff = IVP_EXTRVRN_2X32(hvecInAddrOff, 4 * index);
            hvecLaneIdx = IVP_ADDN_2X32(hvecLaneIdx, 1);

            /* Pointers for Input Data Loads */
            MORPH_IDT_SCALAR *pSrc1 = (pSrc + inAddrOff);

            /* Loading input data and comparing it with maximum value */
            /* outRow 0 , outCol 0 */
            pdvecIn = (MORPH_IDT_2Nx8 *)pSrc1;
            vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn);
            MORPH_OP_LOAD_2NX8_XP(dvecData1, vaInData, pdvecIn, strideU * inDataPitch1 * remX);
            dvecMax1 = MORPH_OP_MAX2NX8(dvecMax1, dvecData1);

            /* outRow 0 , outCol 1 */
            vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn);
            MORPH_OP_LOAD_2NX8_XP(dvecData2, vaInData, pdvecIn, strideU * (inDataPitch2 * remY - inDataPitch1 * remX));
            dvecMax2 = MORPH_OP_MAX2NX8(dvecMax2, dvecData2);

            /* outRow 1 , outCol 0 */
            vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn);
            MORPH_OP_LOAD_2NX8_XP(dvecData3, vaInData, pdvecIn, strideU * inDataPitch1 * remX);
            dvecMax3 = MORPH_OP_MAX2NX8(dvecMax3, dvecData3);

            /* outRow 1 , outCol 1 */
            vaInData = MORPH_OP_LOAD_2NX8_PP(pdvecIn);
            MORPH_OP_LOAD_2NX8_IP(dvecData4, vaInData, pdvecIn);
            dvecMax4 = MORPH_OP_MAX2NX8(dvecMax4, dvecData4);
          } //end of for(k = 0; k < kernelHeightU * kernelWidthU - 1 ; k++)

          /* Storing maximum value */
          /* outRow 0 , outCol 0 */
          dvecMax00 = MORPH_OP_MIN2NX8((MORPH_IDT_2Nx8)activation_max, MORPH_OP_MAX2NX8(dvecMax1, (MORPH_IDT_2Nx8)activation_min));
          pdvecOut = (MORPH_IDT_2Nx8 *)pOut;
          MORPH_OP_SAV2NX8_XP(dvecMax00, vaOutData, pdvecOut, remCh);
          MORPH_OP_FLUSH_SAPOS2NX8(vaOutData, pdvecOut);

          /* outRow 0 , outCol 1 */
          dvecMax01 = MORPH_OP_MIN2NX8((MORPH_IDT_2Nx8)activation_max, MORPH_OP_MAX2NX8(dvecMax2, (MORPH_IDT_2Nx8)activation_min));
          pdvecOut = (MORPH_IDT_2Nx8 *)(pOut + outDataPitch1 * remX);
          MORPH_OP_SAV2NX8_XP(dvecMax01, vaOutData, pdvecOut, remCh * remX);
          MORPH_OP_FLUSH_SAPOS2NX8(vaOutData, pdvecOut);

          /* outRow 1 , outCol 0 */
          dvecMax10 = MORPH_OP_MIN2NX8((MORPH_IDT_2Nx8)activation_max, MORPH_OP_MAX2NX8(dvecMax3, (MORPH_IDT_2Nx8)activation_min));
          pdvecOut = (MORPH_IDT_2Nx8 *)(pOut + outDataPitch2 * remY);
          MORPH_OP_SAV2NX8_XP(dvecMax10, vaOutData, pdvecOut, remCh * remY);
          MORPH_OP_FLUSH_SAPOS2NX8(vaOutData, pdvecOut);

          /* outRow 1 , outCol 1 */
          dvecMax11 = MORPH_OP_MIN2NX8((MORPH_IDT_2Nx8)activation_max, MORPH_OP_MAX2NX8(dvecMax4, (MORPH_IDT_2Nx8)activation_min));
          pdvecOut = (MORPH_IDT_2Nx8 *)(pOut + outDataPitch2 * remY + outDataPitch1 * remX);
          MORPH_OP_SAV2NX8_XP(dvecMax11, vaOutData, pdvecOut, remCh * remX * remY);
          MORPH_OP_FLUSH_SAPOS2NX8(vaOutData, pdvecOut);
        } //for (x = 0; x < outW ; x += 2)
      }   //for (y = 0; y < outH; y += 2)
    }     //for (ch = 0; ch < numCh; ch += vectorizationWidth)
  }
  else
  {
    /*  Read Tile Parameters */
    int32_t outW = XI_TILE3D_GET_DIM2(outTile);
    int32_t outH = XI_TILE3D_GET_DIM3(outTile);
    int32_t numCh = XI_TILE3D_GET_DIM1(outTile);

    int32_t inDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(inTile);
    int32_t inDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(inTile);

    int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
    int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);

    /* Read CNN Pooling Parameters */
    uint8_t leftEdgeFlagU = XI_CNN_MAXPOOLA_GET_LEFTEDGE_FLAG(param);
    uint8_t topEdgeFlagU = XI_CNN_MAXPOOLA_GET_TOPEDGE_FLAG(param);

    /* Min - Max values */
    int32_t activation_min = XI_CNN_MAXPOOLA_GET_MIN_VAL(param);
    int32_t activation_max = XI_CNN_MAXPOOLA_GET_MAX_VAL(param);

    /* Initialize Data Pointers for Input and Output Tiles */
    MORPH_IDT_SCALAR *pInData = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile);
    MORPH_IDT_SCALAR *pOutData = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(outTile);
    MORPH_IDT_2Nx8 dvecMax1, dvecMax2, dvecMax3, dvecMax4;
    MORPH_IDT_2Nx8 dvecMax00, dvecMax01, dvecMax10, dvecMax11;
    MORPH_IDT_2Nx8 dvecData1, dvecData2, dvecData3, dvecData4;
    valign vaOutData = IVP_ZALIGN();
    valign vaInData1;
    valign vaInData2;
    valign vaInData3;
    valign vaInData4;

    /* Loop variables */
    int32_t x, y, ch, kx, ky;
    int32_t remX, remY;

    /* Find the top and left edge based on egdeFlag and kernelwidth/height in pooling_param */
    uint8_t minLeftEdgeU, minTopEdgeU;
    if (kernelWidth % 2 != 0)
    {
      minLeftEdgeU = (kernelWidth / 2);
    }
    else
    {
      minLeftEdgeU = leftEdgeFlagU ? (kernelWidth / 2) : ((kernelWidth / 2) - 1);
    }
    if (kernelHeight % 2 != 0)
    {
      minTopEdgeU = (kernelHeight / 2);
    }
    else
    {
      minTopEdgeU = topEdgeFlagU ? (kernelHeight / 2) : ((kernelHeight / 2) - 1);
    }
    /* Move pointer to the start of the data (including edge) */
    pInData = &pInData[-(minTopEdgeU * inDataPitch2 + minLeftEdgeU * inDataPitch1)];

    int32_t vectorizationWidth = 2 * XCHAL_IVPN_SIMD_WIDTH;

    /* Vectorization is along channels. Output width and output height are unrolled by 2 */

    for (ch = 0; ch < numCh; ch += vectorizationWidth)             // Along channels
    {
      /* Handling cases where number of channels not a multiple of 64 */
      int remCh = XT_MIN(numCh - ch, vectorizationWidth);
      for (y = 0; y < outH; y += 2)   // Along output height
      {
        /* For handling odd number of rows */
        remY = XT_MIN(2, outH - y) - 1;

        for (x = 0; x < outW; x += 2)  // Along output width
        {
          /* For handling odd number of columns */
          remX = XT_MIN(2, outW - x) - 1;

          /* output Data Pointer */
          MORPH_IDT_SCALAR* pOut = pOutData + ch + y * outDataPitch2 + x * outDataPitch1;

          /* Input Data Pointer */
          MORPH_IDT_SCALAR* pSrc1 = pInData + ch + y * inDataPitch2 * strideY + \
            x * inDataPitch1 * strideX;
          MORPH_IDT_SCALAR* pSrc2 = (pSrc1 + (strideX * inDataPitch1 * remX));
          MORPH_IDT_SCALAR* pSrc3 = (pSrc1 + (strideY * inDataPitch2 * remY));
          MORPH_IDT_SCALAR* pSrc4 = (pSrc1 + (strideX * inDataPitch1 * remX) + \
            (strideY * inDataPitch2 * remY));

          dvecMax1 = MORPH_MIN_VALUE;
          dvecMax2 = MORPH_MIN_VALUE;
          dvecMax3 = MORPH_MIN_VALUE;
          dvecMax4 = MORPH_MIN_VALUE;

          for (ky = 0; ky < kernelHeight; ky++)
          {
            pdvecIn1 = (MORPH_IDT_2Nx8 *)(pSrc1 + (ky * inDataPitch2));
            pdvecIn2 = (MORPH_IDT_2Nx8 *)(pSrc2 + (ky * inDataPitch2));
            pdvecIn3 = (MORPH_IDT_2Nx8 *)(pSrc3 + (ky * inDataPitch2));
            pdvecIn4 = (MORPH_IDT_2Nx8 *)(pSrc4 + (ky * inDataPitch2));
            for (kx = 0; kx < kernelWidth; kx++)
            {
              /* Loading input data and comparing it with maximum value */
              /* outRow 0 , outCol 0 */
              vaInData1 = MORPH_OP_LOAD_2NX8_PP(pdvecIn1);
              MORPH_OP_LOAD_2NX8_XP(dvecData1, vaInData1, pdvecIn1, inDataPitch1);
              dvecMax1 = MORPH_OP_MAX2NX8(dvecMax1, dvecData1);

              /* outRow 0 , outCol 1 */
              vaInData2 = MORPH_OP_LOAD_2NX8_PP(pdvecIn2);
              MORPH_OP_LOAD_2NX8_XP(dvecData2, vaInData2, pdvecIn2, inDataPitch1);
              dvecMax2 = MORPH_OP_MAX2NX8(dvecMax2, dvecData2);

              /* outRow 1 , outCol 0 */
              vaInData3 = MORPH_OP_LOAD_2NX8_PP(pdvecIn3);
              MORPH_OP_LOAD_2NX8_XP(dvecData3, vaInData3, pdvecIn3, inDataPitch1);
              dvecMax3 = MORPH_OP_MAX2NX8(dvecMax3, dvecData3);

              /* outRow 1 , outCol 1 */
              vaInData4 = MORPH_OP_LOAD_2NX8_PP(pdvecIn4);
              MORPH_OP_LOAD_2NX8_XP(dvecData4, vaInData4, pdvecIn4, inDataPitch1);
              dvecMax4 = MORPH_OP_MAX2NX8(dvecMax4, dvecData4);
            } //end of for (kx = 0; kx < kernelWidthU; kx++)
          }   //end of for (ky = 0; ky < kernelHeightU; ky++)

          /* Storing maximum value */
          /* outRow 0 , outCol 0 */
          dvecMax00 = MORPH_OP_MIN2NX8((MORPH_IDT_2Nx8)activation_max, MORPH_OP_MAX2NX8(dvecMax1, (MORPH_IDT_2Nx8)activation_min));
          pdvecOut = (MORPH_IDT_2Nx8 *)pOut;
          MORPH_OP_SAV2NX8_XP(dvecMax00, vaOutData, pdvecOut, remCh);
          MORPH_OP_FLUSH_SAPOS2NX8(vaOutData, pdvecOut);

          /* outRow 0 , outCol 1 */
          dvecMax01 = MORPH_OP_MIN2NX8((MORPH_IDT_2Nx8)activation_max, MORPH_OP_MAX2NX8(dvecMax2, (MORPH_IDT_2Nx8)activation_min));
          pdvecOut = (MORPH_IDT_2Nx8 *)(pOut + outDataPitch1 * remX);
          MORPH_OP_SAV2NX8_XP(dvecMax01, vaOutData, pdvecOut, remCh * remX);
          MORPH_OP_FLUSH_SAPOS2NX8(vaOutData, pdvecOut);

          /* outRow 1 , outCol 0 */
          dvecMax10 = MORPH_OP_MIN2NX8((MORPH_IDT_2Nx8)activation_max, MORPH_OP_MAX2NX8(dvecMax3, (MORPH_IDT_2Nx8)activation_min));
          pdvecOut = (MORPH_IDT_2Nx8 *)(pOut + outDataPitch2 * remY);
          MORPH_OP_SAV2NX8_XP(dvecMax10, vaOutData, pdvecOut, remCh * remY);
          MORPH_OP_FLUSH_SAPOS2NX8(vaOutData, pdvecOut);

          /* outRow 1 , outCol 1 */
          dvecMax11 = MORPH_OP_MIN2NX8((MORPH_IDT_2Nx8)activation_max, MORPH_OP_MAX2NX8(dvecMax4, (MORPH_IDT_2Nx8)activation_min));
          pdvecOut = (MORPH_IDT_2Nx8 *)(pOut + outDataPitch2 * remY + outDataPitch1 * remX);
          MORPH_OP_SAV2NX8_XP(dvecMax11, vaOutData, pdvecOut, remCh * remX * remY);
          MORPH_OP_FLUSH_SAPOS2NX8(vaOutData, pdvecOut);
        } //for (x = 0; x < outW ; x += 2)
      }   //for (y = 0; y < outH; y += 2)
    }     //for (ch = 0; ch < numCh; ch += vectorizationWidth)
  }
  return(XI_ERROR_STATUS());
}


#endif //if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))
