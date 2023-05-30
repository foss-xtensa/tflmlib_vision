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
#if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))

#define UNSIGNED8BIT       1
#define SIGNED8BIT         2


#if INPUT_DATA_TYPE == UNSIGNED8BIT

#undef MAKE_NAME_IMPL
#undef MAKE_NAME
#undef MORPH_IDT_CHECK
#undef MORPH_IDT_SCALAR
#undef MORPH_IDT_2Nx8
#undef MORPH_OP_PRIME_2Nx8
#undef MORPH_OP_PRIME_2NX8_XU
#undef MORPH_OP_ALIGN_LOAD_2Nx8_IP
#undef MORPH_OP_ALIGN_LOAD_2Nx8
#undef MORPH_OP_FLUSH_SAPOS2NX8
#undef MORPH_OP_ALIGN_STORE_2NX8_VARIABLE
#undef MORPH_OP_LOAD_2Nx8_VARIABLE
#undef MORPH_OP_SUB_WIDE_2Nx8
#undef MORPH_OP_SUB_WIDEACC_2Nx8

#define MAKE_NAME_IMPL(name, MORPH_FNAME_SPECIFIER_IDT, suffix)  name ## _ ## MORPH_FNAME_SPECIFIER_IDT ## _ ## suffix
#define MAKE_NAME(name, suffix)                                  MAKE_NAME_IMPL(name, U8, suffix)
#define MORPH_IDT_CHECK                     XI_CHECK_TILE3D_U8
#define MORPH_IDT_SCALAR                    uint8_t
#define MORPH_IDT_2Nx8                      xb_vec2Nx8U
#define MORPH_OP_PRIME_2Nx8                 IVP_LA2NX8U_PP
#define MORPH_OP_PRIME_2NX8_XU              IVP_LA2NX8U_PPXU
#define MORPH_OP_ALIGN_LOAD_2Nx8            IVP_LA2NX8U_XP
#define MORPH_OP_ALIGN_LOAD_2Nx8_IP         IVP_LA2NX8U_IP
#define MORPH_OP_ALIGN_STORE_2NX8_VARIABLE  IVP_SAV2NX8U_XP
#define MORPH_OP_FLUSH_SAPOS2NX8            IVP_SAPOS2NX8U_FP
#define MORPH_OP_LOAD_2Nx8_VARIABLE         IVP_LAV2NX8U_XP
#define MORPH_OP_SUB_WIDE_2Nx8              IVP_SUBWU2NX8U
#define MORPH_OP_SUB_WIDEACC_2Nx8           IVP_SUBWUA2NX8U

#elif INPUT_DATA_TYPE == SIGNED8BIT

#undef MAKE_NAME_IMPL
#undef MAKE_NAME
#undef MORPH_IDT_CHECK
#undef MORPH_IDT_SCALAR
#undef MORPH_IDT_2Nx8
#undef MORPH_OP_PRIME_2Nx8
#undef MORPH_OP_PRIME_2NX8_XU
#undef MORPH_OP_ALIGN_LOAD_2Nx8_IP
#undef MORPH_OP_ALIGN_LOAD_2Nx8
#undef MORPH_OP_FLUSH_SAPOS2NX8
#undef MORPH_OP_ALIGN_STORE_2NX8_VARIABLE
#undef MORPH_OP_LOAD_2Nx8_VARIABLE
#undef MORPH_OP_SUB_WIDE_2Nx8
#undef MORPH_OP_SUB_WIDEACC_2Nx8

#define MAKE_NAME_IMPL(name, MORPH_FNAME_SPECIFIER_IDT, suffix)  name ## _ ## MORPH_FNAME_SPECIFIER_IDT ## _ ## suffix
#define MAKE_NAME(name, suffix)                     MAKE_NAME_IMPL(name, S8, suffix)
#define MORPH_IDT_CHECK                             XI_CHECK_TILE3D_S8
#define MORPH_IDT_SCALAR                            int8_t
#define MORPH_IDT_2Nx8                              xb_vec2Nx8
#define MORPH_OP_PRIME_2Nx8                         IVP_LA2NX8_PP
#define MORPH_OP_PRIME_2NX8_XU                      IVP_LA2NX8_PPXU
#define MORPH_OP_ALIGN_LOAD_2Nx8                    IVP_LA2NX8_XP
#define MORPH_OP_ALIGN_LOAD_2Nx8_IP                 IVP_LA2NX8_IP
#define MORPH_OP_ALIGN_STORE_2NX8_VARIABLE          IVP_SAV2NX8_XP
#define MORPH_OP_FLUSH_SAPOS2NX8                    IVP_SAPOS2NX8_FP
#define MORPH_OP_SUB_WIDE_2Nx8                      IVP_SUBW2NX8
#define MORPH_OP_LOAD_2Nx8_VARIABLE                 IVP_LAV2NX8_XP
#define MORPH_OP_SUB_WIDEACC_2Nx8                   IVP_SUBWA2NX8
#endif

/******************************** xiAvgPoolQuantizeA_U8_DWH *********************************/
/******************************** xiAvgPoolQuantizeA_S8_DWH *********************************/
/* Description  : P6 optimized implementation for MxN AvgPool for Android NN                */
/* Inputs       : Input Data Tile, CNN Pooling Parameters, Frame dimensions                 */
/* InOut        : Output Data Tile                                                          */
/* Output       : XI Error Code                                                             */
/* Assumptions  : Input and Output Data are U8                                              */
/*                Input and Output are in DWH format                                        */
/*                Kernel size is MxN and supports odd even combinations                     */
/*                Number of channels in input and output are same                           */
/*                Kernel size supported is upto 129*255(The product of Kernel width and     */
/*                height is restricted based on the accumulator capacity.                   */
/*                i.e. (kw * kh * (input-zeropoint) needs to be accumulated in 24 bit signed*/
/*                accumulator                                                               */
/********************************************************************************************/
XI_ERR_TYPE MAKE_NAME(xiAvgPoolQuantizeA, DWH)(const xi_pTile3D inTile,
                                               xi_pTile3D outTile,
                                               const xi_cnn_avgpoolA_params *param,
                                               const xi_size3D frame3DSize)
{
  XI_ERROR_CHECKS()
  {
    MORPH_IDT_CHECK(inTile);
    MORPH_IDT_CHECK(outTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_POINTER(param);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(inTile, outTile);
    XI_CHECK_TILE3D_DATA_ORDER(inTile, XI_DWH);
    XI_CHECK_TILE3D_DATA_ORDER(outTile, XI_DWH);
    XI_CHECK_ERROR((XI_CNN_AVGPOOLA_GET_STRIDEX(param) > 0 && \
                   XI_CNN_AVGPOOLA_GET_STRIDEY(param) > 0 ), XI_ERR_BADARG, \
                   "\nStrideX = %hhu, StrideY = %hhu,\nStrideX and StrideY provided should be greater than 0, ", XI_CNN_AVGPOOLA_GET_STRIDEX(param),
                   XI_CNN_AVGPOOLA_GET_STRIDEY(param));
    XI_CHECK_ERROR((XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) > 0) && (XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param) > 0), XI_ERR_KSIZE, \
                   "\nKernel width = %hhu, height = %hhu\nKernel height and width should be greater than 0", \
                   XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param), XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param));
    XI_CHECK_ERROR((XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) * XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param)) <= 255 * 129, XI_ERR_KSIZE, \
      "\nKernel width = %hhu, height = %hhu\nThe product of Kernel height and width should be less than or equal to 255 * 129", \
      XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param), XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param));
    XI_CHECK_ERROR((XI_CNN_AVGPOOLA_GET_SHIFT_OUT(param) >= 0 && XI_CNN_AVGPOOLA_GET_SHIFT_OUT(param) < 32), XI_ERR_NORM, \
                   "\nOutput shift  = %d, Output shift is not in between 0 and 32", XI_CNN_AVGPOOLA_GET_SHIFT_OUT(param));
    XI_CHECK_ERROR((XI_CNN_AVGPOOLA_GET_LEFT_SHIFT(param) >= 0 && XI_CNN_AVGPOOLA_GET_LEFT_SHIFT(param) < 24), XI_ERR_NORM, \
                   "\nLeft shift = %d, Left shift is not in between 0 and 24", XI_CNN_AVGPOOLA_GET_LEFT_SHIFT(param));
    XI_CHECK_ZEROPOINT_AVGPOOLAQUANT(inTile, param);
    XI_CHECK_CONSISTENCY_POOL_DWH(inTile, outTile, param);
    XI_CHECK_ERROR((frame3DSize.dim1Size > 0) && (frame3DSize.dim2Size > 0) && (frame3DSize.dim3Size) > 0, XI_ERR_DATASIZE, \
                   "\nFrame dim1 = %d, dim2 = %d, dim3 = %d\nFrame Dimensions should be greater than 0", \
                   frame3DSize.dim1Size, frame3DSize.dim2Size, frame3DSize.dim3Size);
    XI_CHECK_MINMAX_AVGPOOLAQUANT(inTile, param);
    if ((XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) % 2 != 0) || (XI_CNN_AVGPOOLA_GET_LEFTEDGE_FLAG(param) != 0))
    {
      XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_COORD(inTile) - (XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) / 2) + (XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) - 1) >= 0,
                     XI_ERR_COORD, "\ntileCoordX = %d, minLeftEdgeU =%d, kernelWidth = %hhu\nAt least one pixel in the first window should be valid. tileCoordX - minLeftEdgeU + (kernelWidth - 1) >= 0", \
                     XI_TILE3D_GET_DIM2_COORD(inTile), XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) / 2, XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param));
      XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_COORD(inTile) - (XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) / 2) + (XI_TILE3D_GET_DIM2(outTile) - 1) * XI_CNN_AVGPOOLA_GET_STRIDEX(param) < frame3DSize.dim2Size,
                     XI_ERR_COORD, "\ntileCoordX = %d, minLeftEdgeU =%d, outWidth = %d, stride = %hhu, frame dim3 = %d\nAt least one pixel in the last window should be valid.\
                     tileCoordX - minLeftEdgeU + (outWidth - 1)*stride < frameWidth", XI_TILE3D_GET_DIM2_COORD(inTile), XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) / 2, \
                     XI_TILE3D_GET_DIM2(outTile), XI_CNN_AVGPOOLA_GET_STRIDEX(param), frame3DSize.dim2Size);
    }
    else
    {
      XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_COORD(inTile) - (XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) / 2 - 1) + (XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) - 1) >= 0,
                     XI_ERR_COORD, "\ntileCoordX = %d, minLeftEdgeU =%d, kernelWidth = %hhu\nAt least one pixel in the first window should be valid. tileCoordX - minLeftEdgeU + (kernelWidth - 1) >= 0", \
                     XI_TILE3D_GET_DIM2_COORD(inTile), XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) / 2, XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param));
      XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_COORD(inTile) - (XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) / 2 - 1) + (XI_TILE3D_GET_DIM2(outTile) - 1) * XI_CNN_AVGPOOLA_GET_STRIDEX(param) < frame3DSize.dim2Size,
                     XI_ERR_COORD, "\ntileCoordX = %d, minLeftEdgeU =%d, outWidth = %d, stride = %hhu, frame dim3 = %d\nAt least one pixel in the last window should be valid. \
                     tileCoordX - minLeftEdgeU + (outWidth - 1)*stride < frameWidth", XI_TILE3D_GET_DIM2_COORD(inTile), XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) / 2, \
                     XI_TILE3D_GET_DIM2(outTile), XI_CNN_AVGPOOLA_GET_STRIDEX(param), frame3DSize.dim2Size);
    }
    if ((XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param) % 2 != 0) || (XI_CNN_AVGPOOLA_GET_TOPEDGE_FLAG(param) != 0))
    {
      XI_CHECK_ERROR(XI_TILE3D_GET_DIM3_COORD(inTile) - (XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param) / 2) + (XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param) - 1) >= 0,
                     XI_ERR_COORD, "\ntileCoordY = %d, minTopEdgeU =%d, kernelHeight = %hhu\nAt least one pixel in the first window should be valid. tileCoordY - minTopEdgeU + (kernelHeight - 1) >= 0", \
                     XI_TILE3D_GET_DIM3_COORD(inTile), XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param) / 2, XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param));
      XI_CHECK_ERROR(XI_TILE3D_GET_DIM3_COORD(inTile) - (XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param) / 2) + (XI_TILE3D_GET_DIM3(outTile) - 1) * XI_CNN_AVGPOOLA_GET_STRIDEY(param) < frame3DSize.dim3Size,
                     XI_ERR_COORD, "\ntileCoordY = %d, minTopEdgeU =%d, outHeight = %d, stride = %hhu, frame dim3 = %d\nAt least one pixel in the last window should be valid. \
                     tileCoordY - minTopEdgeU + (outHeight - 1)*stride < frameHeight", XI_TILE3D_GET_DIM3_COORD(inTile), XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param) / 2, \
                     XI_TILE3D_GET_DIM3(outTile), XI_CNN_AVGPOOLA_GET_STRIDEY(param), frame3DSize.dim3Size);
    }
    else
    {
      XI_CHECK_ERROR(XI_TILE3D_GET_DIM3_COORD(inTile) - (XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param) / 2 - 1) + (XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param) - 1) >= 0,
                     XI_ERR_COORD, "\ntileCoordY = %d, minTopEdgeU =%d, kernelHeight = %hhu\nAt least one pixel in the first window should be valid. tileCoordY - minTopEdgeU + (kernelHeight - 1) >= 0", \
                     XI_TILE3D_GET_DIM3_COORD(inTile), XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param) / 2, XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param));
      XI_CHECK_ERROR(XI_TILE3D_GET_DIM3_COORD(inTile) - (XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param) / 2 - 1) + (XI_TILE3D_GET_DIM3(outTile) - 1) * XI_CNN_AVGPOOLA_GET_STRIDEY(param) < frame3DSize.dim3Size,
                     XI_ERR_COORD, "\ntileCoordY = %d, minTopEdgeU =%d, outHeight = %d, stride = %hhu, frame dim3 = %d\nAt least one pixel in the last window should be valid. \
                     tileCoordY - minTopEdgeU + (outHeight - 1)*stride < frameHeight", XI_TILE3D_GET_DIM3_COORD(inTile), XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param) / 2, \
                     XI_TILE3D_GET_DIM3(outTile), XI_CNN_AVGPOOLA_GET_STRIDEY(param), frame3DSize.dim3Size);
    }
  }

  /*  Read Tile Parameters */
  int32_t outW = XI_TILE3D_GET_DIM2(outTile);
  int32_t outH = XI_TILE3D_GET_DIM3(outTile);
  int32_t numCh = XI_TILE3D_GET_DIM1(outTile);
  const int32_t inDataWidth = XI_TILE3D_GET_DIM2(inTile);
  const int32_t inDataHeight = XI_TILE3D_GET_DIM3(inTile);

  int32_t inDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(inTile);
  int32_t inDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(inTile);

  int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);

  /* Read CNN Pooling Parameters */
  uint8_t kernelWidthU = XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param);
  uint8_t kernelHeightU = XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param);
  uint8_t strideX = XI_CNN_AVGPOOLA_GET_STRIDEX(param);
  uint8_t strideY = XI_CNN_AVGPOOLA_GET_STRIDEY(param);
  uint8_t leftEdgeFlagU = XI_CNN_AVGPOOLA_GET_LEFTEDGE_FLAG(param);
  uint8_t topEdgeFlagU = XI_CNN_AVGPOOLA_GET_TOPEDGE_FLAG(param);
  int32_t activation_min = XI_CNN_AVGPOOLA_GET_MIN_VAL(param);
  int32_t activation_max = XI_CNN_AVGPOOLA_GET_MAX_VAL(param);
  /* Initialize Data Pointers for Input and Output Tiles */
  MORPH_IDT_SCALAR *pInData = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(inTile);
  MORPH_IDT_SCALAR *pOutData = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(outTile);

  int32_t input_zero_point = XI_CNN_AVGPOOLA_GET_ZEROPTINPUT(param);
  int32_t output_zero_point = XI_CNN_AVGPOOLA_GET_ZEROPTOUTPUT(param);
  int32_t left_shift = XI_CNN_AVGPOOLA_GET_LEFT_SHIFT(param);
  int32_t output_multiplier = XI_CNN_AVGPOOLA_GET_MULTIPLIER_OUT(param);
  int32_t output_shift = XI_CNN_AVGPOOLA_GET_SHIFT_OUT(param);

  MORPH_IDT_2Nx8* restrict pdvecOut;
  MORPH_IDT_2Nx8* restrict pdvecIn;
  xb_vec2Nx24 daccSum1, daccSum2, daccSum3, daccSum4;
  MORPH_IDT_2Nx8 dvecData1, dvecData2, dvecData3, dvecData4;

  /* qFormat :- number of bits required to represent
  * Qf = 1/(kernelHeightU*kernelWidthU) for accurate calculation of average.
  * Minimum 18 bits are required to represent Qf for accurate average calculation .
  * Since in this P6 implementation Qf value is stored in 32 bit
  * vector it is the maximum possible qFactor value.
  */

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


  uint8_t minRightEdgeU = (kernelWidthU - 1) - minLeftEdgeU;
  uint8_t minBottomEdgeU = (kernelHeightU - 1) - minTopEdgeU;
  /* Move pointer to the start of the data (including edge) */
  pInData = &pInData[-(minTopEdgeU * inDataPitch2 + minLeftEdgeU * inDataPitch1)];

  /* Frame dimensions */
  int32_t frame_dim2 = frame3DSize.dim2Size;
  int32_t frame_dim3 = frame3DSize.dim3Size;

  /*Location of tile in the frame */
  int32_t start_x = XI_TILE3D_GET_DIM2_COORD(inTile);
  int32_t start_y = XI_TILE3D_GET_DIM3_COORD(inTile);

  int32_t end_x = start_x + inDataWidth - 1;
  int32_t end_y = start_y + inDataHeight - 1;

  int32_t ixmin = XT_MAX(start_x - minLeftEdgeU, 0);
  int32_t ixmax = XT_MIN(start_x + inDataWidth + minRightEdgeU - 1, frame_dim2 - 1);
  int32_t iymin = XT_MAX(start_y - minTopEdgeU, 0);
  int32_t iymax = XT_MIN(start_y + inDataHeight + minBottomEdgeU - 1, frame_dim3 - 1);

  /* Loop Variables */
  int32_t x, y, ch, k;
  int32_t remX, remY;
  int32_t vectorizationWidth = 2 * XCHAL_IVPN_SIMD_WIDTH;
  int32_t kSizeU1, kSizeU2, kSizeU3, kSizeU4;

  //below implementation is for depth equal to 1 . We assume inout,output depth padding is zero
  if ((numCh == 1) && (strideX == 2)  && (inDataPitch1 == 1) && (outDataPitch1 == 1))
  {
    for (y = 0; y < outH; y += 1)               // Along output height
    {
      /* output Data Pointer */
      MORPH_IDT_SCALAR* pOut = pOutData + y * outDataPitch2;
      pdvecOut = (MORPH_IDT_2Nx8 *)pOut;
      valign vaOutData = IVP_ZALIGN();
      for (x = 0; x < outW; x += vectorizationWidth)                    // Along output width
      {
        /* Input Data Pointer */
        MORPH_IDT_SCALAR* pSrc = pInData + y * inDataPitch2 * strideY + \
          x * inDataPitch1 * strideX;

        xb_vecNx16 vecSeqXL = (IVP_SEQNX16() + (xb_vecNx16)x) << 1;
        xb_vecNx16 vecSeqXH = (IVP_SEQNX16() + (xb_vecNx16)(x + (vectorizationWidth >> 1))) << 1;
        xb_vecNx16 vecXstartL = IVP_MAXNX16(IVP_ZERONX16(), (xb_vecNx16)(minLeftEdgeU - start_x) - vecSeqXL);
        xb_vecNx16 vecXstartH = IVP_MAXNX16(IVP_ZERONX16(), (xb_vecNx16)(minLeftEdgeU - start_x) - vecSeqXH);
        xb_vecNx16 vecXendL = IVP_MINNX16((xb_vecNx16)kernelWidthU, (xb_vecNx16)(frame_dim2 + minLeftEdgeU - start_x) - vecSeqXL);
        xb_vecNx16 vecXendH = IVP_MINNX16((xb_vecNx16)kernelWidthU, (xb_vecNx16)(frame_dim2 + minLeftEdgeU - start_x) - vecSeqXH);

        xb_vecNx16 vecYstartL = IVP_MAXNX16(IVP_ZERONX16(), (xb_vecNx16)(minTopEdgeU - start_y) - (xb_vecNx16)(y * strideY));
        xb_vecNx16 vecYstartH = IVP_MAXNX16(IVP_ZERONX16(), (xb_vecNx16)(minTopEdgeU - start_y) - (xb_vecNx16)(y * strideY));
        xb_vecNx16 vecYendL = IVP_MINNX16((xb_vecNx16)kernelHeightU, (xb_vecNx16)(frame_dim3 + minTopEdgeU - start_y) - (xb_vecNx16)(y * strideY));
        xb_vecNx16 vecYendH = IVP_MINNX16((xb_vecNx16)kernelHeightU, (xb_vecNx16)(frame_dim3 + minTopEdgeU - start_y) - (xb_vecNx16)(y * strideY));

        xb_vecNx16 veckSizeUL = IVP_PACKLNX48(IVP_MULNX16(IVP_SUBNX16(vecXendL, vecXstartL), IVP_SUBNX16(vecYendL, vecYstartL)));
        xb_vecNx16 veckSizeUH = IVP_PACKLNX48(IVP_MULNX16(IVP_SUBNX16(vecXendH, vecXstartH), IVP_SUBNX16(vecYendH, vecYstartH)));
        xb_vecNx16 veckSizeUeven, veckSizeUodd;
        IVP_DSELNX16I(veckSizeUodd, veckSizeUeven, veckSizeUH, veckSizeUL, IVP_DSELI_DEINTERLEAVE_1);

        valign vaInData;
        daccSum1 = 0;

        int Idx = kernelWidthU;
        int inAddrOff = (inDataPitch1);

        /* Finding the sum of values in the receptive field  */
#ifdef __XCC__
#pragma loop_count min=1
#endif
        for (k = 0; k < kernelHeightU * kernelWidthU; k++)
        {
          /* Loading input data and accumulating the sum vector*/
          /* outRow 0 , outCol 0 */
          pdvecIn = (MORPH_IDT_2Nx8 *)pSrc;
          vaInData = MORPH_OP_PRIME_2Nx8(pdvecIn);
          MORPH_OP_LOAD_2Nx8_VARIABLE(dvecData1, vaInData, pdvecIn, inDataWidth - (x * strideX));
          MORPH_OP_LOAD_2Nx8_VARIABLE(dvecData2, vaInData, pdvecIn, inDataWidth - (x * strideX) - vectorizationWidth);
          dvecData1 = IVP_SEL2NX8UI(dvecData2, dvecData1, IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0);
          MORPH_OP_SUB_WIDEACC_2Nx8(daccSum1, dvecData1, input_zero_point);

          Idx--;
          XT_MOVEQZ(inAddrOff, (inDataPitch2 - ((kernelWidthU - 1) * inDataPitch1)), Idx);
          /* Pointers for Input Data Loads */
          pSrc = (pSrc + inAddrOff);
          XT_MOVEQZ(inAddrOff, inDataPitch1, Idx);
          XT_MOVEQZ(Idx, kernelWidthU, Idx);

        } //end of (k = 0; k < kernelHeightU * kernelWidthU ; k++)

        /* Finding the average value */
        xb_vecNx16 vecEvens, vecOdds, roundOffEvens, roundOffOdds;
        xb_vec2Nx8 dvecRqOut1;

        if (kernelHeightU * kernelWidthU < 128)
        {
          /* outRow 0 , outCol 0 */
          vecEvens = IVP_PACKL2NX24_0(daccSum1);
          vecOdds = IVP_PACKL2NX24_1(daccSum1);
          roundOffEvens = IVP_MOVNX16T(-(veckSizeUeven >> 1), veckSizeUeven >> 1, IVP_LTNX16(vecEvens, 0));
          roundOffOdds = IVP_MOVNX16T(-(veckSizeUodd >> 1), veckSizeUodd >> 1, IVP_LTNX16(vecOdds, 0));
          vecEvens = IVP_QUONX16(IVP_ADDNX16(vecEvens, roundOffEvens), veckSizeUeven);
          vecOdds = IVP_QUONX16(IVP_ADDNX16(vecOdds, roundOffOdds), veckSizeUodd);
          REQUANTIZE_POOLING_OUTPUT(vecOdds, vecEvens, dvecRqOut1, output_zero_point, output_multiplier, output_shift, left_shift, activation_min, activation_max)
        }
        else
        {
          xb_vecN_2x32v hvecQuoLL, hvecQuoLH, hvecQuoHL, hvecQuoHH, hvecRemTemp;
          xb_vecNx16 vec1, vec0;
          xb_vecN_2x32v hvecLL, hvecLH, hvecHL, hvecHH;
          xb_vecN_2x32v roundOffLL, roundOffLH, roundOffHL, roundOffHH;
          xb_vecN_2x32v hveckSizeULL, hveckSizeULH, hveckSizeUHL, hveckSizeUHH;

          hveckSizeULL = IVP_UNPKSNX16_L(veckSizeUL);
          hveckSizeULH = IVP_UNPKSNX16_H(veckSizeUL);
          hveckSizeUHL = IVP_UNPKSNX16_L(veckSizeUH);
          hveckSizeUHH = IVP_UNPKSNX16_H(veckSizeUH);

          /* outRow 0, outCol 0 */
          hvecLL = IVP_CVT32S2NX24LL(daccSum1);
          hvecLH = IVP_CVT32S2NX24LH(daccSum1);
          hvecHL = IVP_CVT32S2NX24HL(daccSum1);
          hvecHH = IVP_CVT32S2NX24HH(daccSum1);

          roundOffLL = IVP_MOVN_2X32T(-(hveckSizeULL >> 1), hveckSizeULL >> 1, IVP_LTN_2X32(hvecLL, 0));
          roundOffLH = IVP_MOVN_2X32T(-(hveckSizeULH >> 1), hveckSizeULH >> 1, IVP_LTN_2X32(hvecLH, 0));
          roundOffHL = IVP_MOVN_2X32T(-(hveckSizeUHL >> 1), hveckSizeUHL >> 1, IVP_LTN_2X32(hvecHL, 0));
          roundOffHH = IVP_MOVN_2X32T(-(hveckSizeUHH >> 1), hveckSizeUHH >> 1, IVP_LTN_2X32(hvecHH, 0));

          IVP_DIVN_2X32X16(hvecQuoLL, hvecRemTemp, IVP_ADDN_2X32(hvecLL, roundOffLL), IVP_MOVNX16_FROMN_2X32(hveckSizeULL), 0);
          IVP_DIVN_2X32X16(hvecQuoLH, hvecRemTemp, IVP_ADDN_2X32(hvecLH, roundOffLH), IVP_MOVNX16_FROMN_2X32(hveckSizeULH), 0);
          IVP_DIVN_2X32X16(hvecQuoHL, hvecRemTemp, IVP_ADDN_2X32(hvecHL, roundOffHL), IVP_MOVNX16_FROMN_2X32(hveckSizeUHL), 0);
          IVP_DIVN_2X32X16(hvecQuoHH, hvecRemTemp, IVP_ADDN_2X32(hvecHH, roundOffHH), IVP_MOVNX16_FROMN_2X32(hveckSizeUHH), 0);

          vec0 = IVP_SELNX16I(IVP_MOVNX16_FROMN_2X32(hvecQuoLH), IVP_MOVNX16_FROMN_2X32(hvecQuoLL), IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0);
          vec1 = IVP_SELNX16I(IVP_MOVNX16_FROMN_2X32(hvecQuoHH), IVP_MOVNX16_FROMN_2X32(hvecQuoHL), IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0);

          IVP_DSELNX16I(vecOdds, vecEvens, vec1, vec0, IVP_DSELI_DEINTERLEAVE_1);
          REQUANTIZE_POOLING_OUTPUT(vecOdds, vecEvens, dvecRqOut1, output_zero_point, output_multiplier, output_shift, left_shift, activation_min, activation_max)
        }

        /* Storing average value */
        /* outRow 0 , outCol 0 */
        MORPH_OP_ALIGN_STORE_2NX8_VARIABLE(dvecRqOut1, vaOutData, pdvecOut, outW - x);
      }                   //for (x = 0; x < outDataWidth; x += 2)
      MORPH_OP_FLUSH_SAPOS2NX8(vaOutData, pdvecOut);
    }               //for (y = 0; y < outDataHeight; y += 2)
  }
  else
  {
    int8_t find_normalizer = 0;
    if ((start_x - minLeftEdgeU < 0) || (start_x + inDataWidth + minRightEdgeU - 1 > frame_dim2 - 1)
      || (start_y - minTopEdgeU < 0) || (start_y + inDataHeight + minBottomEdgeU - 1 > frame_dim3 - 1))
    {
      find_normalizer = 1;
    }
    uint8_t find_kSize_startx = 0, find_kSize_starty = 0, find_kSize_startxy = 0;
    uint8_t find_kSize_endx = 0, find_kSize_endy = 0, find_kSize_endxy = 0;
    uint8_t find_kSize_startx_endy = 0, find_kSize_endx_starty = 0;

    if (find_normalizer)
    {
      if (start_x - minLeftEdgeU < 0)             // Edges to be considered along left of inputTile
      {
        find_kSize_startx = 1;
      }
      if (start_y - minTopEdgeU < 0)             //Edges to be considered along top of inputTile
      {
        find_kSize_starty = 1;
      }
      if (end_x + minRightEdgeU > frame_dim2 - 1)             //Edges to be considered along right of inputTile
      {
        find_kSize_endx = 1;
      }
      if (end_y + minBottomEdgeU > frame_dim3 - 1)             //Edges to be considered along bottom of inputTile
      {
        find_kSize_endy = 1;
      }
      //Edges to be considered along top-left corner of inputTile
      if ((start_x - minLeftEdgeU < 0) && (start_y - minTopEdgeU < 0))
      {
        find_kSize_startxy = 1;
      }
      //Edges to be considered along bottom left corner of inputTile
      if ((start_x - minLeftEdgeU < 0) && (end_y + minBottomEdgeU > frame_dim3 - 1))
      {
        find_kSize_startx_endy = 1;
      }
      //Edges to be considered along bottom right corner of inputTile
      if ((end_x + minRightEdgeU > frame_dim2 - 1) && (end_y + minBottomEdgeU > frame_dim3 - 1))
      {
        find_kSize_endxy = 1;
      }
      //Edges to be considered along top right corner of inputTile
      if ((end_x + minRightEdgeU > frame_dim2 - 1) && (start_y - minTopEdgeU < 0))
      {
        find_kSize_endx_starty = 1;
      }
    }

    /* The loop across kernel width and kernel height can be combined. In this         */
    /* case the address offsets for input  need to be derived from vector registers.   */
    for (ch = 0; ch < numCh; ch += vectorizationWidth)                         // Along channels
    {
      /* Handling cases where number of channels not a multiple of 64 */
      int remCh = XT_MIN(numCh - ch, vectorizationWidth);
      for (y = 0; y < outH; y += 2)                     // Along output height
      {
        /* For handling odd number of rows */
        remY = XT_MIN(2, outH - y) - 1;

        for (x = 0; x < outW; x += 2)                          // Along output width
        {
          /* For handling odd number of columns */
          remX = XT_MIN(2, outW - x) - 1;

          /* calculating start index of kernel w.r.t output pixel position*/
          int32_t start_kx = x * strideX + start_x - minLeftEdgeU;
          int32_t start_kx1 = start_kx + strideX;
          int32_t start_ky = y * strideY + start_y - minTopEdgeU;
          int32_t start_ky1 = start_ky + strideY;

          /* calculating end index of kernel w.r.t output pixel position*/
          int32_t end_kx = start_kx + kernelWidthU - 1;
          int32_t end_kx1 = end_kx + strideX;
          int32_t end_ky = start_ky + kernelHeightU - 1;
          int32_t end_ky1 = end_ky + strideY;

          /* Initializing the active pooling window equal to the kernel size */
          kSizeU1 = kSizeU2 = kSizeU3 = kSizeU4 = kernelHeightU * kernelWidthU;
          if (find_normalizer)
          {
            /* Adjusting the active pooling window to avoid the edges for\
            *  outRow 0, outRow 1, outCol 0 and outCol 1 */

            /*Excluding the edges to the left of input from the pooling window*/
            if (find_kSize_startx)
            {
              /* outRow 0 outCol 0*/
              kSizeU1 = kSizeU1 + (start_kx * kernelHeightU * (start_kx < ixmin));
              /* outRow 0 outCol 1*/
              kSizeU2 = kSizeU2 + (start_kx1 * kernelHeightU * (start_kx1 < ixmin) * remX);
              /* outRow 1 outCol 0*/
              kSizeU3 = kSizeU3 + (start_kx * kernelHeightU * (start_kx < ixmin) * remY);
              /* outRow 1 outCol 1*/
              kSizeU4 = kSizeU4 + (start_kx1 * kernelHeightU * (start_kx1 < ixmin) * remX * remY);
            }
            /*Excluding the edges to the top of input from the pooling window*/
            if (find_kSize_starty)
            {
              kSizeU1 = kSizeU1 + (start_ky * kernelWidthU * (start_ky < iymin));
              kSizeU2 = kSizeU2 + (start_ky * kernelWidthU * (start_ky < iymin) * remX);
              kSizeU3 = kSizeU3 + (start_ky1 * kernelWidthU * (start_ky1 < iymin) * remY);
              kSizeU4 = kSizeU4 + (start_ky1 * kernelWidthU * (start_ky1 < iymin) * remX * remY);
            }
            /*Excluding the edges to the right of input from the pooling window*/
            if (find_kSize_endx)
            {
              kSizeU1 = kSizeU1 - ((end_kx - ixmax) * kernelHeightU * ((end_kx) > ixmax));
              kSizeU2 = kSizeU2 - ((end_kx1 - ixmax) * kernelHeightU * ((end_kx1) > ixmax) * remX);
              kSizeU3 = kSizeU3 - ((end_kx - ixmax) * kernelHeightU * ((end_kx) > ixmax) * remY);
              kSizeU4 = kSizeU4 - ((end_kx1 - ixmax) * kernelHeightU * ((end_kx1) > ixmax) * remX * remY);
            }
            /*Excluding the edges to the bottom of input from the pooling window*/
            if (find_kSize_endy)
            {
              kSizeU1 = kSizeU1 - ((end_ky - iymax) * kernelWidthU * ((end_ky) > iymax));
              kSizeU2 = kSizeU2 - ((end_ky - iymax) * kernelWidthU * ((end_ky) > iymax) * remX);
              kSizeU3 = kSizeU3 - ((end_ky1 - iymax) * kernelWidthU * ((end_ky1) > iymax) * remY);
              kSizeU4 = kSizeU4 - ((end_ky1 - iymax) * kernelWidthU * ((end_ky1) > iymax) * remX * remY);
            }
            /*Excluding the edges to the top-left corner of input from the pooling window*/
            if (find_kSize_startxy)
            {
              kSizeU1 = kSizeU1 + (start_kx * start_ky) * \
                ((start_kx < ixmin) && (start_ky < iymin));
              kSizeU2 = kSizeU2 + (start_kx1 * start_ky) * \
                ((start_kx1 < ixmin) && (start_ky < iymin)) * remX;
              kSizeU3 = kSizeU3 + (start_kx * start_ky1) * \
                ((start_kx < ixmin) && (start_ky1 < iymin)) * remY;
              kSizeU4 = kSizeU4 + (start_kx1 * start_ky1) * \
                ((start_kx1 < ixmin) && (start_ky1 < iymin)) * remX * remY;
            }
            /*Excluding the edges to the bottom-left corner of input from the pooling window*/
            if (find_kSize_startx_endy)
            {
              kSizeU1 = kSizeU1 - (start_kx * (end_ky - iymax)) * \
                ((start_kx < ixmin) && (end_ky > iymax));
              kSizeU2 = kSizeU2 - (start_kx1 * (end_ky - iymax)) * \
                ((start_kx1 < ixmin) && (end_ky > iymax)) * remX;
              kSizeU3 = kSizeU3 - (start_kx * (end_ky1 - iymax)) * \
                ((start_kx < ixmin) && (end_ky1 > iymax)) * remY;
              kSizeU4 = kSizeU4 - (start_kx1 * (end_ky1 - iymax)) * \
                ((start_kx1 < ixmin) && (end_ky1 > iymax)) * remX * remY;
            }
            /*Excluding the edges to the bottom-right corner of input from the pooling window*/
            if (find_kSize_endxy)
            {
              kSizeU1 = kSizeU1 + ((end_kx - ixmax) * (end_ky - iymax)) * \
                ((end_kx > ixmax) && (end_ky > iymax));
              kSizeU2 = kSizeU2 + ((end_kx1 - ixmax) * (end_ky - iymax)) * \
                ((end_kx1 > ixmax) && (end_ky > iymax)) * remX;
              kSizeU3 = kSizeU3 + ((end_kx - ixmax) * (end_ky1 - iymax)) * \
                ((end_kx > ixmax) && (end_ky1 > iymax)) * remY;
              kSizeU4 = kSizeU4 + ((end_kx1 - ixmax) * (end_ky1 - iymax)) * \
                ((end_kx1 > ixmax) && (end_ky1 > iymax)) * remX * remY;
            }
            /*Excluding the edges to the top-right corner of input from the pooling window*/
            if (find_kSize_endx_starty)
            {
              kSizeU1 = kSizeU1 - ((end_kx - ixmax) * start_ky) * \
                ((end_kx > ixmax) && (start_ky < iymin));
              kSizeU2 = kSizeU2 - ((end_kx1 - ixmax) * start_ky) * \
                ((end_kx1 > ixmax) && (start_ky < iymin)) * remX;
              kSizeU3 = kSizeU3 - ((end_kx - ixmax) * start_ky1) * \
                ((end_kx > ixmax) && (start_ky1 < iymin)) * remY;
              kSizeU4 = kSizeU4 - ((end_kx1 - ixmax) * start_ky1) * \
                ((end_kx1 > ixmax) && (start_ky1 < iymin)) * remX * remY;
            }
          }

          /* Input Data Pointer */
          MORPH_IDT_SCALAR* pSrc = pInData + ch + y * inDataPitch2 * strideY + \
            x * inDataPitch1 * strideX;

          /* output Data Pointer */
          MORPH_IDT_SCALAR* pOut = pOutData + ch + y * outDataPitch2 + x * outDataPitch1;

          valign vaInData;
          daccSum1 = 0;
          daccSum2 = 0;
          daccSum3 = 0;
          daccSum4 = 0;

          xb_vecN_2x32v hvecLaneIdxAddrOffKw = 0;
          xb_vecN_2x32v hvecLaneIdxAddrOffKh = 0;
          int32_t index, inAddrOff;

          /* Finding the sum of values in the receptive field  */
#ifdef __XCC__
#pragma loop_count min=1
#endif
          for (k = 0; k < kernelHeightU * kernelWidthU; k++)
          {
            /* Condition checks performed to get the Input Pointer Offsets      */
            /* after combining the Kernel Width and Height Loops                */
            vboolN_2 vbN_2 = IVP_EQN_2X32(hvecLaneIdxAddrOffKw, kernelWidthU * inDataPitch1);

            /* hvecLaneIdx will be reset to zero after every kWidth */
            hvecLaneIdxAddrOffKw = IVP_MOVN_2X32T(0, hvecLaneIdxAddrOffKw, vbN_2);

            /* Increment kH index every kW times */
            IVP_ADDN_2X32T(hvecLaneIdxAddrOffKh, hvecLaneIdxAddrOffKh, inDataPitch2, vbN_2);

            /*Get final combined address offset */
            inAddrOff = IVP_ADDN_2X32(hvecLaneIdxAddrOffKw, hvecLaneIdxAddrOffKh);
            /* Extract index */
            index = IVP_EXTRN_2X32(inAddrOff, 0);

            /* Extracting Input address offsets */
            hvecLaneIdxAddrOffKw = IVP_ADDN_2X32(hvecLaneIdxAddrOffKw, inDataPitch1);

            /* Loading input data and accumulating the sum vector*/
            /* outRow 0 , outCol 0 */
#ifdef IVP_LA2NX8U_PPXU
            pdvecIn = (MORPH_IDT_2Nx8 *)pSrc;
            MORPH_OP_PRIME_2NX8_XU(vaInData, pdvecIn, index);
#else
            MORPH_IDT_SCALAR *pSrc1 = (pSrc + index);
            pdvecIn = (MORPH_IDT_2Nx8 *)pSrc1;
            vaInData = MORPH_OP_PRIME_2Nx8(pdvecIn);
#endif
            MORPH_OP_ALIGN_LOAD_2Nx8(dvecData1, vaInData, pdvecIn, strideX * inDataPitch1 * remX);
            MORPH_OP_SUB_WIDEACC_2Nx8(daccSum1, dvecData1, input_zero_point);

            /* outRow 0 , outCol 1 */
            vaInData = MORPH_OP_PRIME_2Nx8(pdvecIn);
            MORPH_OP_ALIGN_LOAD_2Nx8(dvecData2, vaInData, pdvecIn,
              (strideY *inDataPitch2 * remY - strideX * inDataPitch1 * remX));
            MORPH_OP_SUB_WIDEACC_2Nx8(daccSum2, dvecData2, input_zero_point);

            /* outRow 1 , outCol 0 */
            vaInData = MORPH_OP_PRIME_2Nx8(pdvecIn);
            MORPH_OP_ALIGN_LOAD_2Nx8(dvecData3, vaInData, pdvecIn, strideX * inDataPitch1 * remX);
            MORPH_OP_SUB_WIDEACC_2Nx8(daccSum3, dvecData3, input_zero_point);

            /* outRow 1 , outCol 1 */
            vaInData = MORPH_OP_PRIME_2Nx8(pdvecIn);
            MORPH_OP_ALIGN_LOAD_2Nx8_IP(dvecData4, vaInData, pdvecIn);
            MORPH_OP_SUB_WIDEACC_2Nx8(daccSum4, dvecData4, input_zero_point);
          } //end of (k = 0; k < kernelHeightU * kernelWidthU ; k++)

          /* Finding the average value */
          xb_vecNx16 vecEvens, vecOdds, roundOffEvens, roundOffOdds;
          xb_vec2Nx8 dvecRqOut1, dvecRqOut2, dvecRqOut3, dvecRqOut4;

          if (kernelHeightU * kernelWidthU < 128)
          {
            /* outRow 0 , outCol 0 */
            vecEvens = IVP_PACKL2NX24_0(daccSum1);
            vecOdds = IVP_PACKL2NX24_1(daccSum1);
            roundOffEvens = IVP_MOVNX16T(-(kSizeU1 >> 1), kSizeU1 >> 1, IVP_LTNX16(vecEvens, 0));
            roundOffOdds = IVP_MOVNX16T(-(kSizeU1 >> 1), kSizeU1 >> 1, IVP_LTNX16(vecOdds, 0));
            vecEvens = IVP_QUONX16(IVP_ADDNX16(vecEvens, roundOffEvens), (xb_vecNx16)kSizeU1);
            vecOdds = IVP_QUONX16(IVP_ADDNX16(vecOdds, roundOffOdds), (xb_vecNx16)kSizeU1);
            REQUANTIZE_POOLING_OUTPUT(vecOdds, vecEvens, dvecRqOut1, output_zero_point, output_multiplier, output_shift, left_shift, activation_min, activation_max)

              /* outRow 0 , outCol 1 */
            vecEvens = IVP_PACKL2NX24_0(daccSum2);
            vecOdds = IVP_PACKL2NX24_1(daccSum2);
            roundOffEvens = IVP_MOVNX16T(-(kSizeU2 >> 1), kSizeU2 >> 1, IVP_LTNX16(vecEvens, 0));
            roundOffOdds = IVP_MOVNX16T(-(kSizeU2 >> 1), kSizeU2 >> 1, IVP_LTNX16(vecOdds, 0));
            vecEvens = IVP_QUONX16(IVP_ADDNX16(vecEvens, roundOffEvens), (xb_vecNx16)kSizeU2);
            vecOdds = IVP_QUONX16(IVP_ADDNX16(vecOdds, roundOffOdds), (xb_vecNx16)kSizeU2);
            REQUANTIZE_POOLING_OUTPUT(vecOdds, vecEvens, dvecRqOut2, output_zero_point, output_multiplier, output_shift, left_shift, activation_min, activation_max)

              /* outRow 1 , outCol 0 */
            vecEvens = IVP_PACKL2NX24_0(daccSum3);
            vecOdds = IVP_PACKL2NX24_1(daccSum3);
            roundOffEvens = IVP_MOVNX16T(-(kSizeU3 >> 1), kSizeU3 >> 1, IVP_LTNX16(vecEvens, 0));
            roundOffOdds = IVP_MOVNX16T(-(kSizeU3 >> 1), kSizeU3 >> 1, IVP_LTNX16(vecOdds, 0));
            vecEvens = IVP_QUONX16(IVP_ADDNX16(vecEvens, roundOffEvens), (xb_vecNx16)kSizeU3);
            vecOdds = IVP_QUONX16(IVP_ADDNX16(vecOdds, roundOffOdds), (xb_vecNx16)kSizeU3);
            REQUANTIZE_POOLING_OUTPUT(vecOdds, vecEvens, dvecRqOut3, output_zero_point, output_multiplier, output_shift, left_shift, activation_min, activation_max)

              /* outRow 1 , outCol 1 */
            vecEvens = IVP_PACKL2NX24_0(daccSum4);
            vecOdds = IVP_PACKL2NX24_1(daccSum4);
            roundOffEvens = IVP_MOVNX16T(-(kSizeU4 >> 1), kSizeU4 >> 1, IVP_LTNX16(vecEvens, 0));
            roundOffOdds = IVP_MOVNX16T(-(kSizeU4 >> 1), kSizeU4 >> 1, IVP_LTNX16(vecOdds, 0));
            vecEvens = IVP_QUONX16(IVP_ADDNX16(vecEvens, roundOffEvens), (xb_vecNx16)kSizeU4);
            vecOdds = IVP_QUONX16(IVP_ADDNX16(vecOdds, roundOffOdds), (xb_vecNx16)kSizeU4);
            REQUANTIZE_POOLING_OUTPUT(vecOdds, vecEvens, dvecRqOut4, output_zero_point, output_multiplier, output_shift, left_shift, activation_min, activation_max)
          }
          else
          {
            xb_vecN_2x32v hvecQuoLL, hvecQuoLH, hvecQuoHL, hvecQuoHH, hvecRemTemp;
            xb_vecNx16 vec1, vec0;
            xb_vecN_2x32v hvecLL, hvecLH, hvecHL, hvecHH;
            xb_vecN_2x32v roundOffLL, roundOffLH, roundOffHL, roundOffHH;

            /* outRow 0, outCol 0 */
            hvecLL = IVP_CVT32S2NX24LL(daccSum1);
            hvecLH = IVP_CVT32S2NX24LH(daccSum1);
            hvecHL = IVP_CVT32S2NX24HL(daccSum1);
            hvecHH = IVP_CVT32S2NX24HH(daccSum1);

            roundOffLL = IVP_MOVN_2X32T(-(kSizeU1 >> 1), kSizeU1 >> 1, IVP_LTN_2X32(hvecLL, 0));
            roundOffLH = IVP_MOVN_2X32T(-(kSizeU1 >> 1), kSizeU1 >> 1, IVP_LTN_2X32(hvecLH, 0));
            roundOffHL = IVP_MOVN_2X32T(-(kSizeU1 >> 1), kSizeU1 >> 1, IVP_LTN_2X32(hvecHL, 0));
            roundOffHH = IVP_MOVN_2X32T(-(kSizeU1 >> 1), kSizeU1 >> 1, IVP_LTN_2X32(hvecHH, 0));

            IVP_DIVN_2X32X16(hvecQuoLL, hvecRemTemp, IVP_ADDN_2X32(hvecLL, roundOffLL), kSizeU1, 0);
            IVP_DIVN_2X32X16(hvecQuoLH, hvecRemTemp, IVP_ADDN_2X32(hvecLH, roundOffLH), kSizeU1, 0);
            IVP_DIVN_2X32X16(hvecQuoHL, hvecRemTemp, IVP_ADDN_2X32(hvecHL, roundOffHL), kSizeU1, 0);
            IVP_DIVN_2X32X16(hvecQuoHH, hvecRemTemp, IVP_ADDN_2X32(hvecHH, roundOffHH), kSizeU1, 0);

            vec0 = IVP_SELNX16I(IVP_MOVNX16_FROMN_2X32(hvecQuoLH), IVP_MOVNX16_FROMN_2X32(hvecQuoLL), IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0);
            vec1 = IVP_SELNX16I(IVP_MOVNX16_FROMN_2X32(hvecQuoHH), IVP_MOVNX16_FROMN_2X32(hvecQuoHL), IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0);

            IVP_DSELNX16I(vecOdds, vecEvens, vec1, vec0, IVP_DSELI_DEINTERLEAVE_1);
            REQUANTIZE_POOLING_OUTPUT(vecOdds, vecEvens, dvecRqOut1, output_zero_point, output_multiplier, output_shift, left_shift, activation_min, activation_max)

            /* outRow 0, outCol 1 */
            hvecLL = IVP_CVT32S2NX24LL(daccSum2);
            hvecLH = IVP_CVT32S2NX24LH(daccSum2);
            hvecHL = IVP_CVT32S2NX24HL(daccSum2);
            hvecHH = IVP_CVT32S2NX24HH(daccSum2);

            roundOffLL = IVP_MOVN_2X32T(-(kSizeU2 >> 1), kSizeU2 >> 1, IVP_LTN_2X32(hvecLL, 0));
            roundOffLH = IVP_MOVN_2X32T(-(kSizeU2 >> 1), kSizeU2 >> 1, IVP_LTN_2X32(hvecLH, 0));
            roundOffHL = IVP_MOVN_2X32T(-(kSizeU2 >> 1), kSizeU2 >> 1, IVP_LTN_2X32(hvecHL, 0));
            roundOffHH = IVP_MOVN_2X32T(-(kSizeU2 >> 1), kSizeU2 >> 1, IVP_LTN_2X32(hvecHH, 0));

            IVP_DIVN_2X32X16(hvecQuoLL, hvecRemTemp, IVP_ADDN_2X32(hvecLL, roundOffLL), kSizeU2, 0);
            IVP_DIVN_2X32X16(hvecQuoLH, hvecRemTemp, IVP_ADDN_2X32(hvecLH, roundOffLH), kSizeU2, 0);
            IVP_DIVN_2X32X16(hvecQuoHL, hvecRemTemp, IVP_ADDN_2X32(hvecHL, roundOffHL), kSizeU2, 0);
            IVP_DIVN_2X32X16(hvecQuoHH, hvecRemTemp, IVP_ADDN_2X32(hvecHH, roundOffHH), kSizeU2, 0);

            vec0 = IVP_SELNX16I(IVP_MOVNX16_FROMN_2X32(hvecQuoLH), IVP_MOVNX16_FROMN_2X32(hvecQuoLL), IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0);
            vec1 = IVP_SELNX16I(IVP_MOVNX16_FROMN_2X32(hvecQuoHH), IVP_MOVNX16_FROMN_2X32(hvecQuoHL), IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0);

            IVP_DSELNX16I(vecOdds, vecEvens, vec1, vec0, IVP_DSELI_DEINTERLEAVE_1);
            REQUANTIZE_POOLING_OUTPUT(vecOdds, vecEvens, dvecRqOut2, output_zero_point, output_multiplier, output_shift, left_shift, activation_min, activation_max)

            /* outRow 1, outCol 0 */
            hvecLL = IVP_CVT32S2NX24LL(daccSum3);
            hvecLH = IVP_CVT32S2NX24LH(daccSum3);
            hvecHL = IVP_CVT32S2NX24HL(daccSum3);
            hvecHH = IVP_CVT32S2NX24HH(daccSum3);

            roundOffLL = IVP_MOVN_2X32T(-(kSizeU3 >> 1), kSizeU3 >> 1, IVP_LTN_2X32(hvecLL, 0));
            roundOffLH = IVP_MOVN_2X32T(-(kSizeU3 >> 1), kSizeU3 >> 1, IVP_LTN_2X32(hvecLH, 0));
            roundOffHL = IVP_MOVN_2X32T(-(kSizeU3 >> 1), kSizeU3 >> 1, IVP_LTN_2X32(hvecHL, 0));
            roundOffHH = IVP_MOVN_2X32T(-(kSizeU3 >> 1), kSizeU3 >> 1, IVP_LTN_2X32(hvecHH, 0));

            IVP_DIVN_2X32X16(hvecQuoLL, hvecRemTemp, IVP_ADDN_2X32(hvecLL, roundOffLL), kSizeU3, 0);
            IVP_DIVN_2X32X16(hvecQuoLH, hvecRemTemp, IVP_ADDN_2X32(hvecLH, roundOffLH), kSizeU3, 0);
            IVP_DIVN_2X32X16(hvecQuoHL, hvecRemTemp, IVP_ADDN_2X32(hvecHL, roundOffHL), kSizeU3, 0);
            IVP_DIVN_2X32X16(hvecQuoHH, hvecRemTemp, IVP_ADDN_2X32(hvecHH, roundOffHH), kSizeU3, 0);

            vec0 = IVP_SELNX16I(IVP_MOVNX16_FROMN_2X32(hvecQuoLH), IVP_MOVNX16_FROMN_2X32(hvecQuoLL), IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0);
            vec1 = IVP_SELNX16I(IVP_MOVNX16_FROMN_2X32(hvecQuoHH), IVP_MOVNX16_FROMN_2X32(hvecQuoHL), IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0);

            IVP_DSELNX16I(vecOdds, vecEvens, vec1, vec0, IVP_DSELI_DEINTERLEAVE_1);
            REQUANTIZE_POOLING_OUTPUT(vecOdds, vecEvens, dvecRqOut3, output_zero_point, output_multiplier, output_shift, left_shift, activation_min, activation_max)

            /* outRow 1, outCol 1 */
            hvecLL = IVP_CVT32S2NX24LL(daccSum4);
            hvecLH = IVP_CVT32S2NX24LH(daccSum4);
            hvecHL = IVP_CVT32S2NX24HL(daccSum4);
            hvecHH = IVP_CVT32S2NX24HH(daccSum4);

            roundOffLL = IVP_MOVN_2X32T(-(kSizeU4 >> 1), kSizeU4 >> 1, IVP_LTN_2X32(hvecLL, 0));
            roundOffLH = IVP_MOVN_2X32T(-(kSizeU4 >> 1), kSizeU4 >> 1, IVP_LTN_2X32(hvecLH, 0));
            roundOffHL = IVP_MOVN_2X32T(-(kSizeU4 >> 1), kSizeU4 >> 1, IVP_LTN_2X32(hvecHL, 0));
            roundOffHH = IVP_MOVN_2X32T(-(kSizeU4 >> 1), kSizeU4 >> 1, IVP_LTN_2X32(hvecHH, 0));

            IVP_DIVN_2X32X16(hvecQuoLL, hvecRemTemp, IVP_ADDN_2X32(hvecLL, roundOffLL), kSizeU4, 0);
            IVP_DIVN_2X32X16(hvecQuoLH, hvecRemTemp, IVP_ADDN_2X32(hvecLH, roundOffLH), kSizeU4, 0);
            IVP_DIVN_2X32X16(hvecQuoHL, hvecRemTemp, IVP_ADDN_2X32(hvecHL, roundOffHL), kSizeU4, 0);
            IVP_DIVN_2X32X16(hvecQuoHH, hvecRemTemp, IVP_ADDN_2X32(hvecHH, roundOffHH), kSizeU4, 0);

            vec0 = IVP_SELNX16I(IVP_MOVNX16_FROMN_2X32(hvecQuoLH), IVP_MOVNX16_FROMN_2X32(hvecQuoLL), IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0);
            vec1 = IVP_SELNX16I(IVP_MOVNX16_FROMN_2X32(hvecQuoHH), IVP_MOVNX16_FROMN_2X32(hvecQuoHL), IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0);

            IVP_DSELNX16I(vecOdds, vecEvens, vec1, vec0, IVP_DSELI_DEINTERLEAVE_1);
            REQUANTIZE_POOLING_OUTPUT(vecOdds, vecEvens, dvecRqOut4, output_zero_point, output_multiplier, output_shift, left_shift, activation_min, activation_max)
          }

          /* Storing average value */
          /* outRow 0 , outCol 0 */
          pdvecOut = (MORPH_IDT_2Nx8 *)pOut;
          valign vaOutData = IVP_ZALIGN();
          MORPH_OP_ALIGN_STORE_2NX8_VARIABLE(dvecRqOut1, vaOutData, pdvecOut, remCh);
          MORPH_OP_FLUSH_SAPOS2NX8(vaOutData, pdvecOut);

          /* outRow 0 , outCol 1 */
          pdvecOut = (MORPH_IDT_2Nx8 *)(pOut + outDataPitch1 * remX);
          MORPH_OP_ALIGN_STORE_2NX8_VARIABLE(dvecRqOut2, vaOutData, pdvecOut, remCh * remX);
          MORPH_OP_FLUSH_SAPOS2NX8(vaOutData, pdvecOut);

          /* outRow 1 , outCol 0 */
          pdvecOut = (MORPH_IDT_2Nx8 *)(pOut + outDataPitch2 * remY);
          MORPH_OP_ALIGN_STORE_2NX8_VARIABLE(dvecRqOut3, vaOutData, pdvecOut, remCh * remY);
          MORPH_OP_FLUSH_SAPOS2NX8(vaOutData, pdvecOut);

          /* outRow 1 , outCol 1 */
          pdvecOut = (MORPH_IDT_2Nx8 *)(pOut + outDataPitch2 * remY + outDataPitch1 * remX);
          MORPH_OP_ALIGN_STORE_2NX8_VARIABLE(dvecRqOut4, vaOutData, pdvecOut, remCh * remX * remY);
          MORPH_OP_FLUSH_SAPOS2NX8(vaOutData, pdvecOut);
        }             //for (x = 0; x < outDataWidth; x += 2)
      }               //for (y = 0; y < outDataHeight; y += 2)
    }                 //for (z = 0; z < numCh; z++)
  }

  return(XI_ERROR_STATUS());
}
#endif //if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))
