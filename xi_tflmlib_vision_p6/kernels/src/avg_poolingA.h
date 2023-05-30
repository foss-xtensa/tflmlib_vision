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

#define MAX_KERNEL_HEIGHT  (16)

#if INPUT_DATA_TYPE == UNSIGNED8BIT

#undef MAKE_NAME_IMPL
#undef MAKE_NAME
#undef MORPH_IDT_CHECK
#undef MORPH_IDT_SCALAR
#undef MORPH_IDT_2Nx8
#undef MORPH_IDT_Nx16
#undef MORPH_IDT_N_2x32
#undef MORPH_OP_PRIME_2Nx8
#undef MORPH_OP_PRIME_2NX8_XU
#undef MORPH_OP_ALIGN_LOAD_2Nx8_IP
#undef MORPH_OP_ALIGN_LOAD_2Nx8
#undef MORPH_OP_ADD_WIDE_2Nx8
#undef MORPH_OP_ADD_WIDE_ACC_2NX8
#undef MORPH_OP_FLUSH_SAPOS2NX8
#undef MORPH_OP_ALIGN_STORE_2NX8_VARIABLE
#undef MORPH_OP_QUONX16
#undef MORPH_OP_DIVN_2X32X16
#undef MORPH_OP_MIN
#undef MORPH_OP_MAX
#undef MORPH_OP_MOV
#undef MORPH_OP_SYM_ROUND16
#undef MORPH_OP_SYM_ROUND32
#undef MORPH_OP_MULN_2X16X32_0
#undef MORPH_OP_MULN_2X16X32_1


#define MAKE_NAME_IMPL(name, MORPH_FNAME_SPECIFIER_IDT, suffix)  name ## _ ## MORPH_FNAME_SPECIFIER_IDT ## _ ## suffix
#define MAKE_NAME(name, suffix)                                  MAKE_NAME_IMPL(name, U8, suffix)
#define MORPH_IDT_CHECK                     XI_CHECK_TILE3D_U8
#define MORPH_IDT_SCALAR                    uint8_t
#define MORPH_IDT_2Nx8                      xb_vec2Nx8U
#define MORPH_IDT_Nx16                      xb_vecNx16U
#define MORPH_IDT_N_2x32                    xb_vecN_2x32Uv
#define MORPH_OP_PRIME_2Nx8                 IVP_LA2NX8U_PP
#define MORPH_OP_PRIME_2NX8_XU              IVP_LA2NX8U_PPXU
#define MORPH_OP_ALIGN_LOAD_2Nx8            IVP_LA2NX8U_XP
#define MORPH_OP_ALIGN_LOAD_2Nx8_IP         IVP_LA2NX8U_IP
#define MORPH_OP_ADD_WIDE_2Nx8              IVP_ADDWU2NX8
#define MORPH_OP_ADD_WIDE_ACC_2NX8          IVP_ADDWUA2NX8
#define MORPH_OP_ALIGN_STORE_2NX8_VARIABLE  IVP_SAV2NX8U_XP
#define MORPH_OP_FLUSH_SAPOS2NX8            IVP_SAPOS2NX8U_FP
#define MORPH_OP_QUONX16                    IVP_QUONX16U
#define MORPH_OP_DIVN_2X32X16               IVP_DIVN_2X32X16U
#define MORPH_OP_MIN                        IVP_MINU2NX8U
#define MORPH_OP_MAX                        IVP_MAXU2NX8U
#define MORPH_OP_MOV                        IVP_MOV2NX8U_FROMNX16
#define MORPH_OP_SYM_ROUND16(in , kSize)    (in) = IVP_ADDNX16U((in), (MORPH_IDT_Nx16)((kSize) >> 1));
#define MORPH_OP_SYM_ROUND32(in , kSize)    (in) = IVP_ADDN_2X32U((in), (MORPH_IDT_N_2x32)((kSize) >> 1));
#define MORPH_OP_MULN_2X16X32_0             IVP_MULUUN_2X16X32_0
#define MORPH_OP_MULN_2X16X32_1             IVP_MULUUN_2X16X32_1

#elif INPUT_DATA_TYPE == SIGNED8BIT

#undef MAKE_NAME_IMPL
#undef MAKE_NAME
#undef MORPH_IDT_CHECK
#undef MORPH_IDT_SCALAR
#undef MORPH_IDT_2Nx8
#undef MORPH_IDT_Nx16
#undef MORPH_IDT_N_2x32
#undef MORPH_OP_PRIME_2Nx8
#undef MORPH_OP_PRIME_2NX8_XU
#undef MORPH_OP_ALIGN_LOAD_2Nx8_IP
#undef MORPH_OP_ALIGN_LOAD_2Nx8
#undef MORPH_OP_ADD_WIDE_2Nx8
#undef MORPH_OP_ADD_WIDE_ACC_2NX8
#undef MORPH_OP_FLUSH_SAPOS2NX8
#undef MORPH_OP_ALIGN_STORE_2NX8_VARIABLE
#undef MORPH_OP_QUONX16
#undef MORPH_OP_DIVN_2X32X16
#undef MORPH_OP_MIN
#undef MORPH_OP_MAX
#undef MORPH_OP_MOV
#undef MORPH_OP_SYM_ROUND16
#undef MORPH_OP_SYM_ROUND32
#undef MORPH_OP_MULN_2X16X32_0
#undef MORPH_OP_MULN_2X16X32_1

#define MAKE_NAME_IMPL(name, MORPH_FNAME_SPECIFIER_IDT, suffix)  name ## _ ## MORPH_FNAME_SPECIFIER_IDT ## _ ## suffix
#define MAKE_NAME(name, suffix)                                  MAKE_NAME_IMPL(name, S8, suffix)
#define MORPH_IDT_CHECK                             XI_CHECK_TILE3D_S8
#define MORPH_IDT_SCALAR                            int8_t
#define MORPH_IDT_2Nx8                              xb_vec2Nx8
#define MORPH_IDT_Nx16                              xb_vecNx16
#define MORPH_IDT_N_2x32                            xb_vecN_2x32v
#define MORPH_OP_PRIME_2Nx8                         IVP_LA2NX8_PP
#define MORPH_OP_PRIME_2NX8_XU                      IVP_LA2NX8_PPXU
#define MORPH_OP_ALIGN_LOAD_2Nx8                    IVP_LA2NX8_XP
#define MORPH_OP_ALIGN_LOAD_2Nx8_IP                 IVP_LA2NX8_IP
#define MORPH_OP_ADD_WIDE_2Nx8                      IVP_ADDW2NX8
#define MORPH_OP_ADD_WIDE_ACC_2NX8                  IVP_ADDWA2NX8
#define MORPH_OP_ALIGN_STORE_2NX8_VARIABLE          IVP_SAV2NX8_XP
#define MORPH_OP_FLUSH_SAPOS2NX8                    IVP_SAPOS2NX8_FP
#define MORPH_OP_QUONX16                            IVP_QUONX16
#define MORPH_OP_DIVN_2X32X16                       IVP_DIVN_2X32X16
#define MORPH_OP_MIN                                IVP_MIN2NX8
#define MORPH_OP_MAX                                IVP_MAX2NX8
#define MORPH_OP_MOV                                IVP_MOV2NX8_FROMNX16
#define MORPH_OP_MULN_2X16X32_0                     IVP_MULSUN_2X16X32_0
#define MORPH_OP_MULN_2X16X32_1                     IVP_MULSUN_2X16X32_1
#define MORPH_OP_SYM_ROUND16(in , kSize)           {\
MORPH_IDT_Nx16 vecTemp;\
(vecTemp) = IVP_MOVNX16T(IVP_NEGNX16((MORPH_IDT_Nx16)((kSize) >> 1)), (MORPH_IDT_Nx16)((kSize) >> 1),IVP_LTNX16((in), 0));\
(in) = IVP_ADDSNX16((in), (vecTemp));\
}
#define MORPH_OP_SYM_ROUND32(in , kSize)           {\
MORPH_IDT_N_2x32 vecTemp;\
(vecTemp) = IVP_MOVN_2X32T(IVP_NEGN_2X32((MORPH_IDT_N_2x32)((kSize) >> 1)), (MORPH_IDT_N_2x32)((kSize) >> 1),IVP_LTN_2X32((in), 0));\
(in) = IVP_ADDN_2X32((in), (vecTemp));\
}
#endif


/******************************** xiAvgPoolA3D_MxN_U8_DWH *************************************/
/******************************** xiAvgPoolA3D_MxN_S8_DWH *************************************/
/* Description  : P6 optimized implementation for MxN Average Pool for Android NN             */
/* Inputs       : Input Data Tile, CNN Pooling Parameters, Frame dimensions                   */
/* InOut        : Output Data Tile                                                            */
/* Output       : XI Error Code                                                               */
/* Assumptions  : Input and Output Data are U8/S8                                             */
/*                Input and Output are in DWH format                                          */
/*                Implementation do not have restrictions on kernel size for S8 input and     */
/*                stride parameters                                                           */
/*                Kernel size supported is upto 129*255 for U8 input (The product of Kernel   */
/*                width and height is restricted based on the accumulator capacity.           */
/*                i.e. (kw * kh * input needs to be accumulated in 24 bit signed              */
/*                accumulator                                                                 */
/*                Number of channels in input and output are same                             */
/**********************************************************************************************/
XI_ERR_TYPE MAKE_NAME(xiAvgPoolA3D_MxN, DWH)(const xi_pTile3D inTile,
                                             xi_pTile3D outTile,
                                             const xi_cnn_avgpoolA_params *param,
                                             const xi_size3D frame3DSize)
{
  /* Error Checks */
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
    XI_CHECK_ERROR((XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) > 0) && (XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param) > 0), XI_ERR_KSIZE,\
                   "\nKernel width = %hhu, height = %hhu\nKernel height and width should be greater than 0", \
                   XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param), XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param));
    if (XI_TILE3D_CHECK_TYPE(inTile, XI_U8))
    {
      XI_CHECK_ERROR((XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) * XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param)) <= 255 * 129, XI_ERR_KSIZE, \
        "\nKernel width = %hhu, height = %hhu\nThe product of Kernel height and width should be less than or equal to 255 * 129", \
        XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param), XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param));
    }
    XI_CHECK_ERROR((XI_CNN_AVGPOOLA_GET_STRIDEX(param) > 0) && (XI_CNN_AVGPOOLA_GET_STRIDEY(param) > 0), XI_ERR_BADARG, \
                   "\nStrideX = %hhu, StrideY = %hhu\nStrideX and StrideY should be greater than 0", \
                   XI_CNN_AVGPOOLA_GET_STRIDEX(param), XI_CNN_AVGPOOLA_GET_STRIDEY(param));
    XI_CHECK_CONSISTENCY_POOL_DWH(inTile, outTile, param);
    XI_CHECK_ERROR((frame3DSize.dim1Size > 0) && (frame3DSize.dim2Size > 0) && (frame3DSize.dim3Size) > 0, XI_ERR_DATASIZE, \
                   "\nFrame dim1 = %d, dim2 = %d, dim3 = %d\nFrame Dimensions should be greater than 0", \
                   frame3DSize.dim1Size, frame3DSize.dim2Size, frame3DSize.dim3Size);
    XI_CHECK_MINMAX_AVGPOOLA(inTile, param);
    if ((XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) % 2 != 0) || (XI_CNN_AVGPOOLA_GET_LEFTEDGE_FLAG(param) != 0))
    {
      XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_COORD(inTile) - (XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) / 2) + (XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) - 1) >= 0,
                     XI_ERR_COORD, "\ntileCoordX = %d, minLeftEdgeU =%d, kernelWidth = %hhu\nAt least one pixel in the first window should be valid. tileCoordX - minLeftEdgeU + (kernelWidth - 1) >= 0", \
                     XI_TILE3D_GET_DIM2_COORD(inTile), XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) / 2, XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param));
      XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_COORD(inTile) - (XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) / 2) + (XI_TILE3D_GET_DIM2(outTile) - 1) * XI_CNN_AVGPOOLA_GET_STRIDEX(param) < frame3DSize.dim2Size,
                     XI_ERR_COORD, "\ntileCoordX = %d, minLeftEdgeU =%d, outWidth = %d, strideX = %hhu, frame dim2 = %d\nAt least one pixel in the last window should be valid. \
                     tileCoordX - minLeftEdgeU + (outWidth - 1)*strideX < frameWidth", XI_TILE3D_GET_DIM2_COORD(inTile), XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) / 2, \
                     XI_TILE3D_GET_DIM2(outTile), XI_CNN_AVGPOOLA_GET_STRIDEX(param), frame3DSize.dim2Size);
    }
    else
    {
      XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_COORD(inTile) - (XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) / 2 - 1) + (XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) - 1) >= 0,
                     XI_ERR_COORD, "\ntileCoordX = %d, minLeftEdgeU =%d, kernelWidth = %hhu\nAt least one pixel in the first window should be valid. tileCoordX - minLeftEdgeU + (kernelWidth - 1) >= 0", \
                     XI_TILE3D_GET_DIM2_COORD(inTile), XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) / 2, XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param));
      XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_COORD(inTile) - (XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) / 2 - 1) + (XI_TILE3D_GET_DIM2(outTile) - 1) * XI_CNN_AVGPOOLA_GET_STRIDEX(param) < frame3DSize.dim2Size,
                     XI_ERR_COORD, "\ntileCoordX = %d, minLeftEdgeU =%d, outWidth = %d, strideX = %hhu, frame dim2 = %d\nAt least one pixel in the last window should be valid. \
                     tileCoordX - minLeftEdgeU + (outWidth - 1)*strideX < frameWidth", XI_TILE3D_GET_DIM2_COORD(inTile), XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) / 2, \
                     XI_TILE3D_GET_DIM2(outTile), XI_CNN_AVGPOOLA_GET_STRIDEX(param), frame3DSize.dim2Size);
    }
    if ((XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param) % 2 != 0) || (XI_CNN_AVGPOOLA_GET_TOPEDGE_FLAG(param) != 0))
    {
      XI_CHECK_ERROR(XI_TILE3D_GET_DIM3_COORD(inTile) - (XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param) / 2) + (XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param) - 1) >= 0,
                     XI_ERR_COORD, "\ntileCoordY = %d, minTopEdgeU =%d, kernelHeight = %hhu\nAt least one pixel in the first window should be valid. tileCoordY - minTopEdgeU + (kernelHeight - 1) >= 0", \
                     XI_TILE3D_GET_DIM3_COORD(inTile), XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param) / 2, XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param));
      XI_CHECK_ERROR(XI_TILE3D_GET_DIM3_COORD(inTile) - (XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param) / 2) + (XI_TILE3D_GET_DIM3(outTile) - 1) * XI_CNN_AVGPOOLA_GET_STRIDEY(param) < frame3DSize.dim3Size,
                     XI_ERR_COORD, "\ntileCoordY = %d, minTopEdgeU =%d, outHeight = %d, strideY = %hhu, frame dim3 = %d\nAt least one pixel in the last window should be valid. \
                     tileCoordY - minTopEdgeU + (outHeight - 1)*strideY < frameHeight", XI_TILE3D_GET_DIM3_COORD(inTile), XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param) / 2, \
                     XI_TILE3D_GET_DIM3(outTile), XI_CNN_AVGPOOLA_GET_STRIDEY(param), frame3DSize.dim3Size);
    }
    else
    {
      XI_CHECK_ERROR(XI_TILE3D_GET_DIM3_COORD(inTile) - (XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param) / 2 - 1) + (XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param) - 1) >= 0,
                     XI_ERR_COORD, "\ntileCoordY = %d, minTopEdgeU =%d, kernelHeight = %hhu\nAt least one pixel in the first window should be valid. tileCoordY - minTopEdgeU + (kernelHeight - 1) >= 0", \
                     XI_TILE3D_GET_DIM3_COORD(inTile), XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param) / 2, XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param));
      XI_CHECK_ERROR(XI_TILE3D_GET_DIM3_COORD(inTile) - (XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param) / 2 - 1) + (XI_TILE3D_GET_DIM3(outTile) - 1) * XI_CNN_AVGPOOLA_GET_STRIDEY(param) < frame3DSize.dim3Size,
                     XI_ERR_COORD, "\ntileCoordY = %d, minTopEdgeU =%d, outHeight = %d, strideY = %hhu, frame dim3 = %d\nAt least one pixel in the last window should be valid. \
                     tileCoordY - minTopEdgeU + (outHeight - 1)*strideY < frameHeight", XI_TILE3D_GET_DIM3_COORD(inTile), XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param) / 2, \
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

  MORPH_IDT_2Nx8* restrict pdvecOut;
  MORPH_IDT_2Nx8* restrict pdvecIn;
  xb_vec2Nx24 daccSum1, daccSum2, daccSum3, daccSum4;
  MORPH_IDT_2Nx8 dvecData1, dvecData2, dvecData3, dvecData4;

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

  int8_t find_normalizer = 0;
  if ((start_x - minLeftEdgeU < 0) || (start_x + inDataWidth + minRightEdgeU - 1 > frame_dim2 - 1)
    || (start_y - minTopEdgeU < 0) || (start_y + inDataHeight + minBottomEdgeU - 1 > frame_dim3 - 1))
  {
    find_normalizer = 1;
  }
  uint8_t find_kSize_startx = 0, find_kSize_starty = 0, find_kSize_startxy = 0;
  uint8_t find_kSize_endx = 0, find_kSize_endy = 0, find_kSize_endxy = 0;
  uint8_t find_kSize_startx_endy = 0, find_kSize_endx_starty = 0;

  if (start_x - minLeftEdgeU < 0) // Edges to be considered along left of inputTile
  {
    find_kSize_startx = 1;
  }
  if (start_y - minTopEdgeU < 0) //Edges to be considered along top of inputTile
  {
    find_kSize_starty = 1;
  }
  if (end_x + minRightEdgeU > frame_dim2 - 1) //Edges to be considered along right of inputTile
  {
    find_kSize_endx = 1;
  }
  if (end_y + minBottomEdgeU > frame_dim3 - 1) //Edges to be considered along bottom of inputTile
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
        for (k = 0; k < kernelHeightU * kernelWidthU; k++) // Along kernelHeight * kernelWidth
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
          vaInData = MORPH_OP_PRIME_2Nx8(pdvecIn);
          MORPH_OP_ALIGN_LOAD_2Nx8(dvecData1, vaInData, pdvecIn, strideX * inDataPitch1 * remX);
          MORPH_OP_ADD_WIDE_ACC_2NX8(daccSum1, dvecData1, 0);

          /* outRow 0 , outCol 1 */
          vaInData = MORPH_OP_PRIME_2Nx8(pdvecIn);
          MORPH_OP_ALIGN_LOAD_2Nx8(dvecData2, vaInData, pdvecIn,
            (strideY *inDataPitch2 * remY - strideX * inDataPitch1 * remX));
          MORPH_OP_ADD_WIDE_ACC_2NX8(daccSum2, dvecData2, 0);

          /* outRow 1 , outCol 0 */
          vaInData = MORPH_OP_PRIME_2Nx8(pdvecIn);
          MORPH_OP_ALIGN_LOAD_2Nx8(dvecData3, vaInData, pdvecIn, strideX * inDataPitch1 * remX);
          MORPH_OP_ADD_WIDE_ACC_2NX8(daccSum3, dvecData3, 0);

          /* outRow 1 , outCol 1 */
          vaInData = MORPH_OP_PRIME_2Nx8(pdvecIn);
          MORPH_OP_ALIGN_LOAD_2Nx8_IP(dvecData4, vaInData, pdvecIn);
          MORPH_OP_ADD_WIDE_ACC_2NX8(daccSum4, dvecData4, 0);
        }   //end of (k = 0; k < kernelHeightU * kernelWidthU ; k++)

        MORPH_IDT_2Nx8 dvecOut1, dvecOut2, dvecOut3, dvecOut4;

        /* Finding the average value */

        if (kernelHeightU * kernelWidthU < 128)
        {
          /* outRow 0 , outCol 0 */
          //high accurate output
          MORPH_IDT_Nx16 vecEvens = IVP_PACKL2NX24_0(daccSum1);
          MORPH_IDT_Nx16 vecOdds = IVP_PACKL2NX24_1(daccSum1);

          //kSizeU1
          MORPH_OP_SYM_ROUND16(vecEvens, kSizeU1);
          vecEvens = MORPH_OP_QUONX16(vecEvens, (MORPH_IDT_Nx16)kSizeU1);
          MORPH_OP_SYM_ROUND16(vecOdds, kSizeU1);
          vecOdds = MORPH_OP_QUONX16(vecOdds, (MORPH_IDT_Nx16)kSizeU1);
          dvecOut1 = IVP_SEL2NX8I(MORPH_OP_MOV(vecOdds), \
            MORPH_OP_MOV(vecEvens), IVP_SELI_8B_INTERLEAVE_1_EVEN);
          dvecOut1 = MORPH_OP_MIN((MORPH_IDT_2Nx8)activation_max, MORPH_OP_MAX(dvecOut1, (MORPH_IDT_2Nx8)activation_min));

          /* outRow 0 , outCol 1 */
          vecEvens = IVP_PACKL2NX24_0(daccSum2);
          vecOdds = IVP_PACKL2NX24_1(daccSum2);
          MORPH_OP_SYM_ROUND16(vecEvens, kSizeU2);
          vecEvens = MORPH_OP_QUONX16(vecEvens, (MORPH_IDT_Nx16)kSizeU2);
          MORPH_OP_SYM_ROUND16(vecOdds, kSizeU2);
          vecOdds = MORPH_OP_QUONX16(vecOdds, (MORPH_IDT_Nx16)kSizeU2);

          dvecOut2 = IVP_SEL2NX8I(MORPH_OP_MOV(vecOdds), \
            MORPH_OP_MOV(vecEvens), IVP_SELI_8B_INTERLEAVE_1_EVEN);
          dvecOut2 = MORPH_OP_MIN((MORPH_IDT_2Nx8)activation_max, MORPH_OP_MAX(dvecOut2, (MORPH_IDT_2Nx8)activation_min));
          /* outRow 1 , outCol 0 */
          MORPH_IDT_Nx16 vecEvens1 = IVP_PACKL2NX24_0(daccSum3);
          MORPH_IDT_Nx16 vecOdds1 = IVP_PACKL2NX24_1(daccSum3);
          MORPH_OP_SYM_ROUND16(vecEvens1, kSizeU3);
          vecEvens1 = MORPH_OP_QUONX16(vecEvens1, (MORPH_IDT_Nx16)kSizeU3);
          MORPH_OP_SYM_ROUND16(vecOdds1, kSizeU3);
          vecOdds1 = MORPH_OP_QUONX16(vecOdds1, (MORPH_IDT_Nx16)kSizeU3);

          dvecOut3 = IVP_SEL2NX8I(MORPH_OP_MOV(vecOdds1), \
            MORPH_OP_MOV(vecEvens1), IVP_SELI_8B_INTERLEAVE_1_EVEN);
          dvecOut3 = MORPH_OP_MIN((MORPH_IDT_2Nx8)activation_max, MORPH_OP_MAX(dvecOut3, (MORPH_IDT_2Nx8)activation_min));
          /* outRow 1 , outCol 1 */
          vecEvens1 = IVP_PACKL2NX24_0(daccSum4);
          vecOdds1 = IVP_PACKL2NX24_1(daccSum4);
          MORPH_OP_SYM_ROUND16(vecEvens1, kSizeU4);
          vecEvens1 = MORPH_OP_QUONX16(vecEvens1, (MORPH_IDT_Nx16)kSizeU4);
          MORPH_OP_SYM_ROUND16(vecOdds1, kSizeU4);
          vecOdds1 = MORPH_OP_QUONX16(vecOdds1, (MORPH_IDT_Nx16)kSizeU4);
          dvecOut4 = IVP_SEL2NX8I(MORPH_OP_MOV(vecOdds1), \
            MORPH_OP_MOV(vecEvens1), IVP_SELI_8B_INTERLEAVE_1_EVEN);
          dvecOut4 = MORPH_OP_MIN((MORPH_IDT_2Nx8)activation_max, MORPH_OP_MAX(dvecOut4, (MORPH_IDT_2Nx8)activation_min));
        }
        else
        {
          /* outRow 0 , outCol 0 */
          MORPH_IDT_N_2x32 hvec0 = IVP_CVT32S2NX24LL(daccSum1);
          MORPH_IDT_N_2x32 hvec1 = IVP_CVT32S2NX24LH(daccSum1);
          MORPH_IDT_N_2x32 hvec2 = IVP_CVT32S2NX24HL(daccSum1);
          MORPH_IDT_N_2x32 hvec3 = IVP_CVT32S2NX24HH(daccSum1);

          MORPH_OP_SYM_ROUND32(hvec0, kSizeU1);
          MORPH_OP_SYM_ROUND32(hvec1, kSizeU1);
          MORPH_OP_SYM_ROUND32(hvec2, kSizeU1);
          MORPH_OP_SYM_ROUND32(hvec3, kSizeU1);

          MORPH_IDT_N_2x32 remDummyVar;
          MORPH_OP_DIVN_2X32X16(hvec0, remDummyVar, hvec0, (MORPH_IDT_Nx16)kSizeU1, 0);
          MORPH_OP_DIVN_2X32X16(hvec1, remDummyVar, hvec1, (MORPH_IDT_Nx16)kSizeU1, 0);
          MORPH_OP_DIVN_2X32X16(hvec2, remDummyVar, hvec2, (MORPH_IDT_Nx16)kSizeU1, 0);
          MORPH_OP_DIVN_2X32X16(hvec3, remDummyVar, hvec3, (MORPH_IDT_Nx16)kSizeU1, 0);
          
          MORPH_IDT_Nx16 vec0 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvec1, hvec0, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));
          MORPH_IDT_Nx16 vec1 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvec3, hvec2, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));
          dvecOut1 = IVP_SEL2NX8I(MORPH_OP_MOV(vec1), MORPH_OP_MOV(vec0), IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0);
          dvecOut1 = MORPH_OP_MIN((MORPH_IDT_2Nx8)activation_max, MORPH_OP_MAX(dvecOut1, (MORPH_IDT_2Nx8)activation_min));

          /* outRow 0 , outCol 1 */
          hvec0 = IVP_CVT32S2NX24LL(daccSum2);
          hvec1 = IVP_CVT32S2NX24LH(daccSum2);
          hvec2 = IVP_CVT32S2NX24HL(daccSum2);
          hvec3 = IVP_CVT32S2NX24HH(daccSum2);

          MORPH_OP_SYM_ROUND32(hvec0, kSizeU2);
          MORPH_OP_SYM_ROUND32(hvec1, kSizeU2);
          MORPH_OP_SYM_ROUND32(hvec2, kSizeU2);
          MORPH_OP_SYM_ROUND32(hvec3, kSizeU2);

          MORPH_OP_DIVN_2X32X16(hvec0, remDummyVar, hvec0, (MORPH_IDT_Nx16)kSizeU2, 0);
          MORPH_OP_DIVN_2X32X16(hvec1, remDummyVar, hvec1, (MORPH_IDT_Nx16)kSizeU2, 0);
          MORPH_OP_DIVN_2X32X16(hvec2, remDummyVar, hvec2, (MORPH_IDT_Nx16)kSizeU2, 0);
          MORPH_OP_DIVN_2X32X16(hvec3, remDummyVar, hvec3, (MORPH_IDT_Nx16)kSizeU2, 0);

          vec0 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvec1, hvec0, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));
          vec1 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvec3, hvec2, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));
          dvecOut2 = IVP_SEL2NX8I(MORPH_OP_MOV(vec1), MORPH_OP_MOV(vec0), IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0);
          dvecOut2 = MORPH_OP_MIN((MORPH_IDT_2Nx8)activation_max, MORPH_OP_MAX(dvecOut2, (MORPH_IDT_2Nx8)activation_min));

          /* outRow 1 , outCol 0 */
          hvec0 = IVP_CVT32S2NX24LL(daccSum3);
          hvec1 = IVP_CVT32S2NX24LH(daccSum3);
          hvec2 = IVP_CVT32S2NX24HL(daccSum3);
          hvec3 = IVP_CVT32S2NX24HH(daccSum3);

          MORPH_OP_SYM_ROUND32(hvec0, kSizeU3);
          MORPH_OP_SYM_ROUND32(hvec1, kSizeU3);
          MORPH_OP_SYM_ROUND32(hvec2, kSizeU3);
          MORPH_OP_SYM_ROUND32(hvec3, kSizeU3);

          MORPH_OP_DIVN_2X32X16(hvec0, remDummyVar, hvec0, (MORPH_IDT_Nx16)kSizeU3, 0);
          MORPH_OP_DIVN_2X32X16(hvec1, remDummyVar, hvec1, (MORPH_IDT_Nx16)kSizeU3, 0);
          MORPH_OP_DIVN_2X32X16(hvec2, remDummyVar, hvec2, (MORPH_IDT_Nx16)kSizeU3, 0);
          MORPH_OP_DIVN_2X32X16(hvec3, remDummyVar, hvec3, (MORPH_IDT_Nx16)kSizeU3, 0);

          vec0 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvec1, hvec0, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));
          vec1 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvec3, hvec2, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));
          dvecOut3 = IVP_SEL2NX8I(MORPH_OP_MOV(vec1), MORPH_OP_MOV(vec0), IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0);
          dvecOut3 = MORPH_OP_MIN((MORPH_IDT_2Nx8)activation_max, MORPH_OP_MAX(dvecOut3, (MORPH_IDT_2Nx8)activation_min));

          /* outRow 1 , outCol 1 */
          hvec0 = IVP_CVT32S2NX24LL(daccSum4);
          hvec1 = IVP_CVT32S2NX24LH(daccSum4);
          hvec2 = IVP_CVT32S2NX24HL(daccSum4);
          hvec3 = IVP_CVT32S2NX24HH(daccSum4);

          MORPH_OP_SYM_ROUND32(hvec0, kSizeU4);
          MORPH_OP_SYM_ROUND32(hvec1, kSizeU4);
          MORPH_OP_SYM_ROUND32(hvec2, kSizeU4);
          MORPH_OP_SYM_ROUND32(hvec3, kSizeU4);

          MORPH_OP_DIVN_2X32X16(hvec0, remDummyVar, hvec0, (MORPH_IDT_Nx16)kSizeU4, 0);
          MORPH_OP_DIVN_2X32X16(hvec1, remDummyVar, hvec1, (MORPH_IDT_Nx16)kSizeU4, 0);
          MORPH_OP_DIVN_2X32X16(hvec2, remDummyVar, hvec2, (MORPH_IDT_Nx16)kSizeU4, 0);
          MORPH_OP_DIVN_2X32X16(hvec3, remDummyVar, hvec3, (MORPH_IDT_Nx16)kSizeU4, 0);

          vec0 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvec1, hvec0, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));
          vec1 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvec3, hvec2, IVP_SELI_16B_EXTRACT_1_OF_2_OFF_0));
          dvecOut4 = IVP_SEL2NX8I(MORPH_OP_MOV(vec1), MORPH_OP_MOV(vec0), IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0);
          dvecOut4 = MORPH_OP_MIN((MORPH_IDT_2Nx8)activation_max, MORPH_OP_MAX(dvecOut4, (MORPH_IDT_2Nx8)activation_min));
        }

        /* Storing average value */
        /* outRow 0 , outCol 0 */
        pdvecOut = (MORPH_IDT_2Nx8 *)pOut;
        valign vaOutData = IVP_ZALIGN();
        MORPH_OP_ALIGN_STORE_2NX8_VARIABLE(dvecOut1, vaOutData, pdvecOut, remCh);
        MORPH_OP_FLUSH_SAPOS2NX8(vaOutData, pdvecOut);

        /* outRow 0 , outCol 1 */
        pdvecOut = (MORPH_IDT_2Nx8 *)(pOut + outDataPitch1);
        MORPH_OP_ALIGN_STORE_2NX8_VARIABLE(dvecOut2, vaOutData, pdvecOut, remCh * remX);
        MORPH_OP_FLUSH_SAPOS2NX8(vaOutData, pdvecOut);

        /* outRow 1 , outCol 0 */
        pdvecOut = (MORPH_IDT_2Nx8 *)(pOut + outDataPitch2);
        MORPH_OP_ALIGN_STORE_2NX8_VARIABLE(dvecOut3, vaOutData, pdvecOut, remCh * remY);
        MORPH_OP_FLUSH_SAPOS2NX8(vaOutData, pdvecOut);

        /* outRow 1 , outCol 1 */
        pdvecOut = (MORPH_IDT_2Nx8 *)(pOut + outDataPitch2 + outDataPitch1);
        MORPH_OP_ALIGN_STORE_2NX8_VARIABLE(dvecOut4, vaOutData, pdvecOut, remCh * remX * remY);
        MORPH_OP_FLUSH_SAPOS2NX8(vaOutData, pdvecOut);
      } //for (x = 0; x < outDataWidth; x += 2)
    }   //for (y = 0; y < outDataHeight; y += 2)
  }     //for (z = 0; z < numCh; z++)

  return(XI_ERROR_STATUS());
}
#endif //if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))
