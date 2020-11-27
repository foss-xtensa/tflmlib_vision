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

#define CLAMP(v, min, max)                      ((v) < (min) ? (min) : (v) > (max) ? (max) : (v))

int32_t selpat[16] _XI_LOCAL_RAM0_ = { 0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7};
int16_t selpat1[32] _XI_LOCAL_RAM0_ = { 0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7};

int8_t dselpat[64] _XI_LOCAL_RAM0_ =
{
    0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,2,3,2,3,2,3,2,3,2,3,2,3,2,3,2,3,
    4,5,4,5,4,5,4,5,4,5,4,5,4,5,4,5,6,7,6,7,6,7,6,7,6,7,6,7,6,7,6,7
};

int16_t selpat4[32] _XI_LOCAL_RAM0_ =
{2,2,2,2,2,2,2,2,4,4,4,4,4,4,4,4,6,6,6,6,6,6,6,6,8,8,8,8,8,8,8,8};


int8_t dselpat1[64] _XI_LOCAL_RAM0_ =
{
    8,9,8,9,8,9,8,9,8,9,8,9,8,9,8,9,10,11,10,11,10,11,10,11,10,11,10,11,10,11,10,11,
    12,13,12,13,12,13,12,13,12,13,12,13,12,13,12,13,14,15,14,15,14,15,14,15,14,15,14,15,14,15,14,15
};

int8_t dselpat_lo_hi[64] _XI_LOCAL_RAM0_ =
{
    0,16,1,17,2,18,3,19,4,20,5,21,6,22,7,23,8,24,9,25,10,26,11,27,12,28,13,29,14,30,15,31,
    32,48,33,49,34,50,35,51,36,52,37,53,38,54,39,55,40,56,41,57,42,58,43,59,44,60,45,61,46,62,47,63,
};

int16_t selpat5[32] _XI_LOCAL_RAM0_ =
{10,10,10,10,10,10,10,10,12,12,12,12,12,12,12,12,14,14,14,14,14,14,14,14,16,16,16,16,16,16,16,16};


XI_ERR_TYPE xiDepthwiseMultiplierConvolvedA3D_U8_DWH(const xi_pTile3D inTile,
                                                     const xi_pTile3D coeffTile,
                                                     const xi_pArray biasArray,
                                                     xi_pTile3D outTile,
                                                     const xi_cnna_depthwiseDilatedConv_params *param)
{
  XI_ERROR_CHECKS()
  {
    XI_CHECK_TILE3D_U8(inTile);
    XI_CHECK_TILE3D_U8(coeffTile);
    XI_CHECK_TILE3D_U8(outTile);
    XI_CHECK_ARRAY_S32(biasArray);
    XI_CHECK_POINTER(param);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(coeffTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(inTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(coeffTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(biasArray, outTile);
    XI_CHECK_TILE3D_DATA_ORDER(inTile, XI_DWH);
    XI_CHECK_TILE3D_DATA_ORDER(coeffTile, XI_DWH);
    XI_CHECK_TILE3D_DATA_ORDER(outTile, XI_DWH);
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(coeffTile) <= 8) && (XI_TILE3D_GET_DIM3(coeffTile) <= 8),
                   XI_ERR_KSIZE, "Coefficient sizes up to 8x8 supported.");
    XI_CHECK_ERROR((XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) <= 36) && (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) <= 36),
                    XI_ERR_BADARG, "Dilation up to 36x36 supported.");
    XI_CHECK_ERROR((XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEX(param) <= 4) && (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEY(param) <= 4),
                   XI_ERR_BADARG, "Stride up to 4x4 supported.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(outTile) == XI_TILE3D_GET_DIM1(coeffTile),
                   XI_ERR_DATASIZE, "Output depth should be same as coefficient's first dimension size.");
    XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(biasArray) >= XI_TILE3D_GET_DIM1(outTile),
                   XI_ERR_DATASIZE, "Width of Bias Array is less than number of output channel.");
    XI_CHECK_ERROR(XI_ARRAY_GET_HEIGHT(biasArray) > 0,
                   XI_ERR_DATASIZE, "Height of Bias Array should be greater than zero.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM2(inTile) >= (XI_TILE3D_GET_DIM2(outTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEX(param) + 1,
                   XI_ERR_DATASIZE, "inWidth >= (outWidth - 1) * strideWidth + 1.");
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM3(inTile) >= (XI_TILE3D_GET_DIM3(outTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEY(param) + 1,
                   XI_ERR_DATASIZE, "inHeight >= (outHeight - 1) * strideHeight + 1.");
    XI_CHECK_ERROR(((XI_CNNA_DEPTHWISE_DILATED_CONV_GET_OUTPUT_SHIFT(param) < 32) && (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_OUTPUT_SHIFT(param) > -32)),\
                     XI_ERR_NORM, "Provided output shift value is not supported.");
    if (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_RELU(param))
    {
      XI_CHECK_ERROR(XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(param) <= \
                     XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(param),
                     XI_ERR_BADARG, "Relu max is less than Relu min");
      XI_CHECK_ERROR((XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(param) >= 0) && \
                     (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(param) <= UCHAR_MAX),
                      XI_ERR_BADARG, "Relu min value is invalid");
      XI_CHECK_ERROR((XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(param) >= 0) && \
                     (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(param) <= UCHAR_MAX),
                      XI_ERR_BADARG, "Relu max value is invalid");
    }
    if (!(((XI_TILE3D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) % 2 == 0))
    {
      XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (((XI_TILE3D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2),
                     XI_ERR_EDGE, "edgeLeft >= dilatedKernelWidth / 2.");
      XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (((XI_TILE3D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2),
                     XI_ERR_EDGE, "edgeRight >= dilatedKernelWidth / 2.");
    }
    else
    {
      if (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_LEFTEDGE(param))
      {
        XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (((XI_TILE3D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2),
                       XI_ERR_EDGE, "edgeLeft >= dilatedKernelWidth / 2.");
        XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((((XI_TILE3D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2) - 1),
                       XI_ERR_EDGE, "edgeRight >= dilatedKernelWidth / 2.");
      }
      else
      {
        XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((((XI_TILE3D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2) - 1),
                       XI_ERR_EDGE, "edgeLeft >= dilatedKernelWidth / 2 - 1.");
        XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((((XI_TILE3D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2)),
                       XI_ERR_EDGE, "edgeRight >= dilatedKernelWidth / 2.");
      }
    }
    if (!(((XI_TILE3D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) % 2 == 0))
    {
      XI_CHECK_ERROR(XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (((XI_TILE3D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2),
                     XI_ERR_EDGE, "edgeRight >= dilatedKernelHeight / 2.");
      XI_CHECK_ERROR(XI_TILE3D_GET_DIM3_EDGE2(inTile) >= (((XI_TILE3D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2),
                     XI_ERR_EDGE, "edgeLeft >= dilatedKernelHeight / 2.");
    }
    else
    {
      if (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_TOPEDGE(param))
      {
        XI_CHECK_ERROR(XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (((XI_TILE3D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2),
                       XI_ERR_EDGE, "edgeTop >= dilatedKernelHeight / 2.");
        XI_CHECK_ERROR(XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((((XI_TILE3D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2) - 1),
                       XI_ERR_EDGE, "edgeBottom >= dilatedKernelHeight / 2 - 1.");
      }
      else
      {
        XI_CHECK_ERROR(XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((((XI_TILE3D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2) - 1),
                       XI_ERR_EDGE, "edgeTop >= dilatedKernelHeight / 2 - 1.");
        XI_CHECK_ERROR(XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((((XI_TILE3D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2)),
                       XI_ERR_EDGE, "edgeTop >= dilatedKernelHeight / 2.");
      }
    }
  }
  int32_t inDepth             = XI_TILE3D_GET_DIM1(inTile);
  int32_t inPitch1            = XI_TILE3D_GET_DIM1_PITCH(inTile);
  int32_t inPitch2            = XI_TILE3D_GET_DIM2_PITCH(inTile);
  int32_t outWidth            = XI_TILE3D_GET_DIM2(outTile);
  int32_t outHeight           = XI_TILE3D_GET_DIM3(outTile);
  int32_t outPitch1           = XI_TILE3D_GET_DIM1_PITCH(outTile);
  int32_t outPitch2           = XI_TILE3D_GET_DIM2_PITCH(outTile);
  int32_t filterWidth         = XI_TILE3D_GET_DIM2(coeffTile);
  int32_t filterHeight        = XI_TILE3D_GET_DIM3(coeffTile);
  int32_t filterPitch1        = XI_TILE3D_GET_DIM1_PITCH(coeffTile);
  uint8_t *pInput             = (uint8_t *) XI_TILE3D_GET_DATA_PTR(inTile);
  uint8_t *pOutData           = (uint8_t *) XI_TILE3D_GET_DATA_PTR(outTile);
  uint8_t *pFilter            = (uint8_t *) XI_TILE3D_GET_DATA_PTR(coeffTile);
  int32_t *pBias              = (int32_t *) XI_ARRAY_GET_DATA_PTR(biasArray);
  int32_t strideWidth         = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEX(param);
  int32_t strideHeight        = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEY(param);
  int32_t dilationX           = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param);
  int32_t dilationY           = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param);
  int32_t depthMultiplier     = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DEPTH_MULTIPLIER(param);
  int32_t inputOffset         = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_INPUT_OFFSET(param);
  int32_t filterOffset        = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_COEFF_OFFSET(param);
  int32_t outputOffset        = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_OUTPUT_OFFSET(param);
  int32_t outMultiplier       = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_OUTPUT_MULTIPLIER(param);
  int32_t outShift            = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_OUTPUT_SHIFT(param);
  int32_t reluMin             = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(param);
  int32_t reluMax             = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(param);
  uint8_t leftEdgeFlag        = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_LEFTEDGE(param);
  uint8_t topEdgeFlag         = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_TOPEDGE(param);
  int32_t dilatedFilterWidth  = (filterWidth - 1) * dilationX + 1;
  int32_t dilatedFilterHeight = (filterHeight - 1) * dilationY + 1;
  const int8_t enableReLu = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_RELU(param);
  const uint8_t minLim = enableReLu ? reluMin : 0;
  const uint8_t maxLim = enableReLu ? reluMax : UCHAR_MAX;
  int32_t leftShift = outShift < 0 ? -outShift : 0;
  int32_t rightShift = outShift < 0 ? 0 : outShift;
  uint8_t *restrict pInData11;
  uint8_t *restrict pInData12;
  uint8_t *restrict pInData21;
  uint8_t *restrict pInData22;
  xb_vecNx8U *restrict pvecOutData;
  xb_vecNx8U *restrict pvecFilter;
  xb_vec2Nx8U *restrict pVecIn0;
  xb_vec2Nx8U *restrict pVecIn1;
  xb_vecN_2x32v *restrict phvecBias;
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
  xb_vecN_2x32Uv hvecInAddrOffInit = inPitch1 * dilationX;
  vboolN_2 vbN_2 = IVP_EQN_2X32(IVP_SEQN_2X32(), filterWidth - 1);
  hvecInAddrOffInit = IVP_MOVN_2X32T(((inPitch2 * dilationY) - ((filterWidth - 1) * inPitch1 * dilationX)), hvecInAddrOffInit, vbN_2);
  int32_t outputDepth = inDepth * depthMultiplier;
  xb_vecNx16 vecInOffset = (xb_vecNx16)inputOffset;
  xb_vecNx16 vecFiltOffset = (xb_vecNx16)filterOffset;
  valign vaOut = IVP_ZALIGN();

  for (int32_t outY = 0; outY < outHeight; outY += 2)
     {
        int32_t enable2Row    = XT_SALT(outY, outHeight - 1);
        for (int32_t outX = 0; outX < outWidth; outX += 8) // process 4 width at once
        {
            int32_t enable2Col    = XT_SALT(outX, outWidth - 1);
            int32_t enable2RowCol  = enable2Col * enable2Row;
            for (int32_t ic = 0; ic < inDepth; ic++) // Input depth index
            {
                for (int32_t m = 0; m < depthMultiplier; m += XCHAL_IVPN_SIMD_WIDTH) // DepthMultiplier used to move about outputDepth
                {
                    xb_vecNx48 dacc11, dacc12, dacc21, dacc22;
                    int32_t filterD = m + ic * depthMultiplier; // filter depth index
                    int32_t biasD   = filterD;
                    xb_vecNx16 vecOut11, vecOut12, vecOut21, vecOut22;
                    phvecBias = (xb_vecN_2x32v *) &pBias[biasD];
                    xb_vecN_2x32v hvecBias1, hvecBias2;
                    valign valBias = IVP_LAN_2X32_PP(phvecBias);
                    IVP_LAVN_2X32_XP(hvecBias1, valBias, phvecBias, (outputDepth - biasD) << 2);
                    IVP_LAVN_2X32_XP(hvecBias2, valBias, phvecBias, (outputDepth - biasD - (XCHAL_IVPN_SIMD_WIDTH >> 1)) << 2);
                    hvecBias1 = IVP_SHFLN_2X32(hvecBias1, *(xb_vecN_2x32v *)&selpat);
                    hvecBias2 = hvecBias1; //replicate lo to hi
                    dacc11 = IVP_CVT48SNX32(hvecBias2, hvecBias1);
                    dacc12 = dacc11;
                    dacc21 = dacc11;
                    dacc22 = dacc11;

                    uint8_t* pOut = pOutData + ic * depthMultiplier + m + outX * outPitch1 + outY * outPitch2;
                    // 2 heights
                    uint8_t *pData = pInput + ic + outX * strideWidth * inPitch1 + outY * strideHeight * inPitch2;
                    pVecIn0 = (xb_vec2Nx8U *)(pData);
                    pVecIn1 = (xb_vec2Nx8U *)(pData + strideHeight * inPitch2 * enable2Row);
                    valign valignIn0, valignIn1;

                    // Load the coefficients
                    uint8_t *pCoeff = pFilter + filterD;
                    pvecFilter = (xb_vecNx8U *)(pCoeff);
                    valign valFilter = IVP_LANX8U_PP(pvecFilter);
                    //load coeff
                    xb_vecNx16 vecFilter00, vecFilter01, vecFilter02;
                    for (int32_t k = 0; k < filterHeight ; k++) /* Kernel Height */
                    {
                        xb_vec2Nx8U vecInData0, vecInData1;
                        valignIn0 = IVP_LA2NX8U_PP(pVecIn0);
                        IVP_LA2NX8U_XP(vecInData0, valignIn0, pVecIn0, inPitch2); // take 1st 9 values
                        valignIn1 = IVP_LA2NX8U_PP(pVecIn1);
                        IVP_LA2NX8U_XP(vecInData1, valignIn1, pVecIn1, inPitch2); // take 1st 9 values

                        xb_vecNx16 vecInData11 = IVP_MOVNX16_FROM2NX8U(IVP_SEL2NX8UI(0, vecInData0, IVP_SELI_8B_INTERLEAVE_1_LO));
                        xb_vecNx16 vecInData12 = IVP_MOVNX16_FROM2NX8U(IVP_SEL2NX8UI(0, vecInData1, IVP_SELI_8B_INTERLEAVE_1_LO));
                        // 00
                        IVP_LANX8U_XP(vecFilter00, valFilter, pvecFilter, filterPitch1);vecFilter00 = IVP_ADDNX16(vecFilter00, vecFiltOffset);
                        vecFilter00 = IVP_SHFLNX16(vecFilter00, *(xb_vecNx16 *)&selpat1);
                        //01
                        IVP_LANX8U_XP(vecFilter01, valFilter, pvecFilter, filterPitch1);vecFilter01 = IVP_ADDNX16(vecFilter01, vecFiltOffset);
                        vecFilter01 = IVP_SHFLNX16(vecFilter01, *(xb_vecNx16 *)&selpat1);
                        //02
                        IVP_LANX8U_XP(vecFilter02, valFilter, pvecFilter, filterPitch1);vecFilter02 = IVP_ADDNX16(vecFilter02, vecFiltOffset);
                        vecFilter02 = IVP_SHFLNX16(vecFilter02, *(xb_vecNx16 *)&selpat1);
                        // can use DSELs
                        xb_vecNx16 vech00, vech01,vech02,vech03,vech04,vech05 ;
                        IVP_DSELNX16(vech01, vech00, vecInData11, vecInData11, *(xb_vec2Nx8 *)&dselpat);
                        vech02 = IVP_SHFLNX16(vecInData11, *(xb_vecNx16 *)&selpat4);//2,4,6,8
                        IVP_DSELNX16(vech04, vech03, vecInData11, vecInData11, *(xb_vec2Nx8 *)&dselpat1);
                        vech05 = IVP_SHFLNX16(vecInData11, *(xb_vecNx16 *)&selpat5);//2,4,6,8

                        xb_vecNx16 vech10, vech11, vech12, vech13, vech14, vech15 ;
                        IVP_DSELNX16(vech11, vech10, vecInData12, vecInData12, *(xb_vec2Nx8 *)&dselpat);
                        vech12 = IVP_SHFLNX16(vecInData12, *(xb_vecNx16 *)&selpat4);//2,4,6,8
                        IVP_DSELNX16(vech14, vech13, vecInData12, vecInData12, *(xb_vec2Nx8 *)&dselpat1);
                        vech15 = IVP_SHFLNX16(vecInData12, *(xb_vecNx16 *)&selpat5);//2,4,6,8

                        IVP_MULPANX16(dacc11, vech00, vecFilter00, vecInOffset, vecFilter00);
                        IVP_MULPANX16(dacc11, vech01, vecFilter01, vecInOffset, vecFilter01);
                        IVP_MULPANX16(dacc11, vech02, vecFilter02, vecInOffset, vecFilter02);
                        IVP_MULPANX16(dacc12, vech03, vecFilter00, vecInOffset, vecFilter00);
                        IVP_MULPANX16(dacc12, vech04, vecFilter01, vecInOffset, vecFilter01);
                        IVP_MULPANX16(dacc12, vech05, vecFilter02, vecInOffset, vecFilter02);

                        IVP_MULPANX16(dacc21, vech10, vecFilter00, vecInOffset, vecFilter00);
                        IVP_MULPANX16(dacc21, vech11, vecFilter01, vecInOffset, vecFilter01);
                        IVP_MULPANX16(dacc21, vech12, vecFilter02, vecInOffset, vecFilter02);
                        IVP_MULPANX16(dacc22, vech13, vecFilter00, vecInOffset, vecFilter00);
                        IVP_MULPANX16(dacc22, vech14, vecFilter01, vecInOffset, vecFilter01);
                        IVP_MULPANX16(dacc22, vech15, vecFilter02, vecInOffset, vecFilter02);

                    }
                     ADD48_ZPT_PACK_SCALE_SHIFT_CLAMP_LIMITS(vecOut11, dacc11, outputOffset, leftShift, rightShift, outMultiplier, maxLim, minLim);
                     ADD48_ZPT_PACK_SCALE_SHIFT_CLAMP_LIMITS(vecOut12, dacc12, outputOffset, leftShift, rightShift, outMultiplier, maxLim, minLim);
                     ADD48_ZPT_PACK_SCALE_SHIFT_CLAMP_LIMITS(vecOut21, dacc21, outputOffset, leftShift, rightShift, outMultiplier, maxLim, minLim);
                     ADD48_ZPT_PACK_SCALE_SHIFT_CLAMP_LIMITS(vecOut22, dacc22, outputOffset, leftShift, rightShift, outMultiplier, maxLim, minLim);

                     int32_t varLen   = XT_MIN(XCHAL_IVPN_SIMD_WIDTH, depthMultiplier - m);
                     pvecOutData = (xb_vecNx8U *)(pOut);
                     IVP_SAVNX8U_XP(vecOut11, vaOut, pvecOutData, 4*varLen);
                     IVP_SAVNX8U_XP(vecOut12, vaOut, pvecOutData, 4*varLen);
                     IVP_SAPOSNX8U_FP(vaOut, pvecOutData);
                     pvecOutData = (xb_vecNx8U *)(pOut + outPitch2);
                     IVP_SAVNX8U_XP(vecOut21, vaOut, pvecOutData,4*varLen * enable2Row);
                     IVP_SAVNX8U_XP(vecOut22, vaOut, pvecOutData,4*varLen * enable2Row);
                     IVP_SAPOSNX8U_FP(vaOut, pvecOutData);
                }
            }
        }
     }

  return(XI_ERROR_STATUS());
}

/*****************************************************************************
*  Functions optimized for CNNA
*  **************************************************************************/
/*****************************************************************************
*  xiDepthwiseConvolveA2D_S_3x3_U8Ca2_MOD_DWH
*  **************************************************************************/

/****************************************************************************/
/* Description : P6 optimized implementation of depthwise convolution       */
/*               for  for Android NN.                                       */
/* Inputs      : Input Data Tile, Coeff Data Tile, Bias Array,              */
/*               CNNA convolution params structure                          */
/* Outputs     : XI Error Code                                              */
/* InOuts      : Output Tile                                                */
/* Assumptions : InData, CoeffData are U8                                   */
/*               biasArray is signed 32b, OutData is U8                     */
/*               Kernel Size is 3x3.                                        */
/*               Input and Output are in DWH format                         */
/*               Coeff is in DWH format                                     */
/*               CoeffDim1Pitch is aligned to 2N (Ca2)                      */
/****************************************************************************/

/*****************************************************************************
* Stride 2 Sub-variant
* When input stride is 2 this function is called
*  **************************************************************************/

void depthwiseConvolveA2D_S_3x3j2_U8Ca2_MOD_DWH(
  const xi_pTile3D inTile,
  const xi_pTile3D coeffTile,
  const xi_pArray biasArray,
  xi_pTile3D outTile,
  const xi_cnna_conv_params *param
  )
{
  /* Getting parameters from the tile structures */
  const int32_t outW  = XI_TILE3D_GET_DIM2(outTile);
  const int32_t outH  = XI_TILE3D_GET_DIM3(outTile);
  const int32_t numCh = XI_TILE3D_GET_DIM1(inTile);

  /* Kernel Size (DWH) */
  const int32_t kSizeU = XI_TILE3D_GET_DIM2(coeffTile);

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

  int32_t leftShift = outShift < 0 ? -outShift : 0;
  int32_t rightShift = outShift < 0 ? 0 : outShift;

  /* Data Pointers of input, output, coefficient and bias data */
  int8_t *pInData    = (int8_t *) XI_TILE3D_GET_DATA_PTR(inTile);
  int8_t *pOutData   = (int8_t *) XI_TILE3D_GET_DATA_PTR(outTile);
  int8_t *pCoeffData = (int8_t *) XI_TILE3D_GET_DATA_PTR(coeffTile);
  int32_t *pBiasData = (int32_t *) XI_ARRAY_GET_DATA_PTR(biasArray);

  /* Pitches of Coefficient Data (DWH) in dim1 and dim2 */
  const int32_t coeffPitch1 = XI_TILE3D_GET_DIM1_PITCH(coeffTile);
  const int32_t coeffPitch2 = XI_TILE3D_GET_DIM2_PITCH(coeffTile);

  /* Pitches of Input Data (DWH) in dim1 and dim2 */
  const int32_t inDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(inTile);

  /* Pitch of Output Data (DWH) in dim1 and dim2 */
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);


  /* Move pointer to the start of the data (including edge) */
  pInData = &pInData[-((kSizeU / 2) * inDataPitch1 + (kSizeU / 2) * inDataPitch2)];

  /* Setting the limits for output data according to ReLu is enabled or not*/
  const uint8_t minLimU = enableReLu ? reluMinU : 0;
  const uint8_t maxLimU = enableReLu ? reluMaxU : UCHAR_MAX;

  /* Variable Declarations */
  int32_t ch, x, y;
  valign vaOutData = IVP_ZALIGN();
  valign vaOutData1 = IVP_ZALIGN();

  xb_vecN_2x32v* restrict phvecBias1;
  xb_vecNx8U* restrict pvecCoeff;
  xb_vecNx8U* restrict pvecData1;
  xb_vecNx8U* restrict pvecData2;
  xb_vecNx8U* restrict pvecData3;
  xb_vecNx8U* restrict pvecData4;
  xb_vecNx8U* restrict pvecData5;
  xb_vecNx8U* restrict pvecData6;
  xb_vecNx8U* restrict pdvecOut;
  xb_vecNx8U* restrict pdvecOut1;

 if(numCh!=16)
 {
      for (ch = 0; ch < numCh; ch += XCHAL_IVPN_SIMD_WIDTH)
      { /* walk across the channels */
        /* To handle corner case when number of channels
         * is not a multiple of  2 * XCHAL_IVPN_SIMD_WIDTH*/
        int32_t remainingCh = numCh - ch;

        /* Initialize accumulators with bias values */
         xb_vecNx48 accSum, accSum1,accSum2;
        phvecBias1 = (xb_vecN_2x32v *) (pBiasData + ch);
        valign vaBias;
        xb_vecN_2x32v hvecBias1, hvecBias2;
        vaBias = IVP_LAN_2X32_PP(phvecBias1);
        IVP_L2ANX32_IP(hvecBias2, hvecBias1, vaBias, phvecBias1);
        accSum = IVP_CVT48SNX32(hvecBias2, hvecBias1);

        /* Pointer for Coefficient Load */
        pvecCoeff = (xb_vecNx8U *) (pCoeffData + ch);

        /* 9 Coefficient Loads */
        xb_vecNx16U vecCoeff11U; IVP_LVNX8U_XP(vecCoeff11U, pvecCoeff, coeffPitch1);
        xb_vecNx16U vecCoeff12U; IVP_LVNX8U_XP(vecCoeff12U, pvecCoeff, coeffPitch1);
        xb_vecNx16U vecCoeff13U; IVP_LVNX8U_XP(vecCoeff13U, pvecCoeff, coeffPitch2 - 2 * coeffPitch1);
        xb_vecNx16U vecCoeff21U; IVP_LVNX8U_XP(vecCoeff21U, pvecCoeff, coeffPitch1);
        xb_vecNx16U vecCoeff22U; IVP_LVNX8U_XP(vecCoeff22U, pvecCoeff, coeffPitch1);
        xb_vecNx16U vecCoeff23U; IVP_LVNX8U_XP(vecCoeff23U, pvecCoeff, coeffPitch2 - 2 * coeffPitch1);
        xb_vecNx16U vecCoeff31U; IVP_LVNX8U_XP(vecCoeff31U, pvecCoeff, coeffPitch1);
        xb_vecNx16U vecCoeff32U; IVP_LVNX8U_XP(vecCoeff32U, pvecCoeff, coeffPitch1);
        xb_vecNx16U vecCoeff33U; IVP_LVNX8U_XP(vecCoeff33U, pvecCoeff, coeffPitch1);

        /* Subtract corresponding zeropoint from coeff vector */
        xb_vecNx16 vecCoeff11 = IVP_SUBNX16U(vecCoeff11U, zeroPtCoeffU);
        xb_vecNx16 vecCoeff12 = IVP_SUBNX16U(vecCoeff12U, zeroPtCoeffU);
        xb_vecNx16 vecCoeff13 = IVP_SUBNX16U(vecCoeff13U, zeroPtCoeffU);
        xb_vecNx16 vecCoeff21 = IVP_SUBNX16U(vecCoeff21U, zeroPtCoeffU);
        xb_vecNx16 vecCoeff22 = IVP_SUBNX16U(vecCoeff22U, zeroPtCoeffU);
        xb_vecNx16 vecCoeff23 = IVP_SUBNX16U(vecCoeff23U, zeroPtCoeffU);
        xb_vecNx16 vecCoeff31 = IVP_SUBNX16U(vecCoeff31U, zeroPtCoeffU);
        xb_vecNx16 vecCoeff32 = IVP_SUBNX16U(vecCoeff32U, zeroPtCoeffU);
        xb_vecNx16 vecCoeff33 = IVP_SUBNX16U(vecCoeff33U, zeroPtCoeffU);

         for (y = 0; y < outH; y+=2)
        { /* walk down the rows */
          /* Input Data Pointers */
          int8_t *pData = pInData + ch + y * stride * inDataPitch2;
          int8_t *pData1 = pInData + ch + ((y+1) * stride * inDataPitch2);
          int32_t enable2Row    = XT_SALT((y+1), outH );

           pvecData1 = (xb_vecNx8U *) pData;
           pvecData2 = (xb_vecNx8U *) (pData + inDataPitch1);
           pvecData3 = (xb_vecNx8U *) (pData + inDataPitch2 + inDataPitch1);

           pvecData4 = (xb_vecNx8U *) pData1;
           pvecData5 = (xb_vecNx8U *) (pData1 + inDataPitch1);
           pvecData6 = (xb_vecNx8U *) (pData1 + inDataPitch2 + inDataPitch1);
           /* Input loads*/
           /* ky = 0*/
           valign vaData1 = IVP_LANX8U_PP(pvecData1);
           xb_vecNx16U vecData11U; IVP_LANX8U_XP(vecData11U, vaData1, pvecData1, inDataPitch2);

           /* ky = 1*/
           vaData1 = IVP_LANX8U_PP(pvecData1);
           xb_vecNx16U vecData21U; IVP_LANX8U_XP(vecData21U, vaData1, pvecData1, inDataPitch2);

           /* ky = 2*/
           vaData1 = IVP_LANX8U_PP(pvecData1);
           xb_vecNx16U vecData31U; IVP_LANX8U_XP(vecData31U, vaData1, pvecData1, inDataPitch2);

           vaData1 = IVP_LANX8U_PP(pvecData4);
           xb_vecNx16U vecData11_2U; IVP_LANX8U_XP(vecData11_2U, vaData1, pvecData4, inDataPitch2);
           vaData1 = IVP_LANX8U_PP(pvecData4);
           xb_vecNx16U vecData21_2U; IVP_LANX8U_XP(vecData21_2U, vaData1, pvecData4, inDataPitch2);
           vaData1 = IVP_LANX8U_PP(pvecData4);
           xb_vecNx16U vecData31_2U; IVP_LANX8U_XP(vecData31_2U, vaData1, pvecData4, inDataPitch2);
           /* Subtract corresponding zeropoint from input vector*/
           xb_vecNx16 vecData11 = IVP_SUBNX16U(vecData11U, zeroPtInputU);
           xb_vecNx16 vecData21 = IVP_SUBNX16U(vecData21U, zeroPtInputU);
           xb_vecNx16 vecData31 = IVP_SUBNX16U(vecData31U, zeroPtInputU);

           xb_vecNx16 vecData11_2 = IVP_SUBNX16U(vecData11_2U, zeroPtInputU);
           xb_vecNx16 vecData21_2 = IVP_SUBNX16U(vecData21_2U, zeroPtInputU);
           xb_vecNx16 vecData31_2 = IVP_SUBNX16U(vecData31_2U, zeroPtInputU);
          for (x = 0; x < outW; x++)
          { /* walk across the columns */
            /* Output Data pointer */
            int8_t *pOut = pOutData + (x * outDataPitch1 + y * outDataPitch2);
             int8_t *pOut1 = pOutData + (x * outDataPitch1 + (y+1) * outDataPitch2);

            /* Initialize accumulator with bias values */
            accSum1 = accSum;
             accSum2 = accSum;

            /* ky = 0*/
            valign vaData2 = IVP_LANX8U_PP(pvecData2);
            xb_vecNx16U vecData12U; IVP_LANX8U_XP(vecData12U, vaData2, pvecData2, inDataPitch1);
            vaData2 = IVP_LANX8U_PP(pvecData2);
            xb_vecNx16U vecData13U; IVP_LANX8U_XP(vecData13U, vaData2, pvecData2, inDataPitch1);

            /* ky = 1*/
            valign vaData3 = IVP_LANX8U_PP(pvecData3);
            xb_vecNx16U vecData22U; IVP_LANX8U_XP(vecData22U, vaData3, pvecData3, inDataPitch1);
            vaData3 = IVP_LANX8U_PP(pvecData3);
            xb_vecNx16U vecData23U; IVP_LANX8U_XP(vecData23U, vaData3, pvecData3, inDataPitch2 \
                                                  - inDataPitch1);

            /* ky = 2*/
            vaData3 = IVP_LANX8U_PP(pvecData3);
            xb_vecNx16U vecData32U; IVP_LANX8U_XP(vecData32U, vaData3, pvecData3, inDataPitch1);
            vaData3 = IVP_LANX8U_PP(pvecData3);
            xb_vecNx16U vecData33U; IVP_LANX8U_XP(vecData33U, vaData3, pvecData3, -inDataPitch2 \
                                                  + inDataPitch1);

              vaData2 = IVP_LANX8U_PP(pvecData5);
              xb_vecNx16U vecData12_2U; IVP_LANX8U_XP(vecData12_2U, vaData2, pvecData5, inDataPitch1);
              vaData2 = IVP_LANX8U_PP(pvecData5);
              xb_vecNx16U vecData13_2U; IVP_LANX8U_XP(vecData13_2U, vaData2, pvecData5, inDataPitch1);
              vaData3 = IVP_LANX8U_PP(pvecData6);
              xb_vecNx16U vecData22_2U; IVP_LANX8U_XP(vecData22_2U, vaData3, pvecData6, inDataPitch1);
              vaData3 = IVP_LANX8U_PP(pvecData6);
              xb_vecNx16U vecData23_2U; IVP_LANX8U_XP(vecData23_2U, vaData3, pvecData6, inDataPitch2 \
                                                    - inDataPitch1);
              vaData3 = IVP_LANX8U_PP(pvecData6);
              xb_vecNx16U vecData32_2U; IVP_LANX8U_XP(vecData32_2U, vaData3, pvecData6, inDataPitch1);
              vaData3 = IVP_LANX8U_PP(pvecData6);
              xb_vecNx16U vecData33_2U; IVP_LANX8U_XP(vecData33_2U, vaData3, pvecData6, -inDataPitch2 \
                                                    + inDataPitch1);
            /* Subtract corresponding zeropoint from input vector*/
            xb_vecNx16 vecData12 = IVP_SUBNX16U(vecData12U, zeroPtInputU);
            xb_vecNx16 vecData22 = IVP_SUBNX16U(vecData22U, zeroPtInputU);
            xb_vecNx16 vecData32 = IVP_SUBNX16U(vecData32U, zeroPtInputU);

            xb_vecNx16 vecData13 = IVP_SUBNX16U(vecData13U, zeroPtInputU);
            xb_vecNx16 vecData23 = IVP_SUBNX16U(vecData23U, zeroPtInputU);
            xb_vecNx16 vecData33 = IVP_SUBNX16U(vecData33U, zeroPtInputU);

             xb_vecNx16 vecData12_2 = IVP_SUBNX16U(vecData12_2U, zeroPtInputU);
            xb_vecNx16 vecData22_2 = IVP_SUBNX16U(vecData22_2U, zeroPtInputU);
            xb_vecNx16 vecData32_2 = IVP_SUBNX16U(vecData32_2U, zeroPtInputU);
            xb_vecNx16 vecData13_2 = IVP_SUBNX16U(vecData13_2U, zeroPtInputU);
            xb_vecNx16 vecData23_2 = IVP_SUBNX16U(vecData23_2U, zeroPtInputU);
            xb_vecNx16 vecData33_2 = IVP_SUBNX16U(vecData33_2U, zeroPtInputU);
            /*Multiply and accumulate input data vector and coeff vector */
            IVP_MULPANX16(accSum1, vecCoeff11, vecData11, vecCoeff12, vecData12);
            IVP_MULPANX16(accSum1, vecCoeff13, vecData13, vecCoeff21, vecData21);
            IVP_MULPANX16(accSum1, vecCoeff22, vecData22, vecCoeff23, vecData23);
            IVP_MULPANX16(accSum1, vecCoeff31, vecData31, vecCoeff32, vecData32);
            IVP_MULANX16(accSum1, vecCoeff33, vecData33);

            IVP_MULPANX16(accSum2, vecCoeff11, vecData11_2, vecCoeff12, vecData12_2);
            IVP_MULPANX16(accSum2, vecCoeff13, vecData13_2, vecCoeff21, vecData21_2);
            IVP_MULPANX16(accSum2, vecCoeff22, vecData22_2, vecCoeff23, vecData23_2);
            IVP_MULPANX16(accSum2, vecCoeff31, vecData31_2, vecCoeff32, vecData32_2);
            IVP_MULANX16(accSum2, vecCoeff33, vecData33_2);

            /* Pack, Output Scale, Output Shift and clamping */
            xb_vecNx16 vec16H1W1AB;
            ADD48_ZPT_PACK_SCALE_SHIFT_CLAMP_LIMITS(vec16H1W1AB, accSum1, \
                                                    zeroPtOutputU, leftShift,rightShift, quantScale, maxLimU, minLimU);
             xb_vecNx16 vec16H1W1AB1;
              ADD48_ZPT_PACK_SCALE_SHIFT_CLAMP_LIMITS(vec16H1W1AB1, accSum2, \
                            zeroPtOutputU, leftShift,rightShift, quantScale, maxLimU, minLimU);

            /* Store the output vecOut1 along the output depth */
            pdvecOut = (xb_vecNx8U *) (pOut + ch);
            IVP_SAVNX8U_XP(vec16H1W1AB, vaOutData, pdvecOut, remainingCh);
            IVP_SAPOSNX8U_FP(vaOutData, pdvecOut);

             pdvecOut1 = (xb_vecNx8U *) (pOut1 + ch);
            IVP_SAVNX8U_XP(vec16H1W1AB1, vaOutData1, pdvecOut1, remainingCh*enable2Row);
            IVP_SAPOSNX8U_FP(vaOutData1, pdvecOut1);
            vecData11 = vecData13;
            vecData21 = vecData23;
            vecData31 = vecData33;
             vecData11_2 = vecData13_2;
             vecData21_2 = vecData23_2;
             vecData31_2 = vecData33_2;
          } /* End for (x = 0; x < outW; x ++) */
        }   /* End for (y = 0; y < outH; y ++) */
      }     /* End for (ch = 0; ch < numCh; ch += 2 * XCHAL_IVPN_SIMD_WIDTH) */
   }
   else //16 case
   {
       for (ch = 0; ch < numCh; ch += XCHAL_IVPN_SIMD_WIDTH)
      { /* walk across the channels */
        /* To handle corner case when number of channels
         * is not a multiple of  2 * XCHAL_IVPN_SIMD_WIDTH*/
        int32_t remainingCh = numCh - ch;

        /* Initialize accumulators with bias values */
        xb_vecNx48 accSum, accSum1, accSum2;
        phvecBias1 = (xb_vecN_2x32v *) (pBiasData + ch);
        valign vaBias;
        xb_vecN_2x32v hvecBias1, hvecBias2;
        vaBias = IVP_LAN_2X32_PP(phvecBias1);
        IVP_L2ANX32_IP(hvecBias2, hvecBias1, vaBias, phvecBias1);
        hvecBias2 = hvecBias1 ; // copy 1 to 2
        accSum = IVP_CVT48SNX32(hvecBias2, hvecBias1);

        /* Pointer for Coefficient Load */
        pvecCoeff = (xb_vecNx8U *) (pCoeffData + ch);

        /* 9 Coefficient Loads */
        xb_vecNx16U vecCoeff11U; IVP_LVNX8U_XP(vecCoeff11U, pvecCoeff, coeffPitch1);
        xb_vecNx16U vecCoeff12U; IVP_LVNX8U_XP(vecCoeff12U, pvecCoeff, coeffPitch1);
        xb_vecNx16U vecCoeff13U; IVP_LVNX8U_XP(vecCoeff13U, pvecCoeff, coeffPitch2 - 2 * coeffPitch1);
        xb_vecNx16U vecCoeff21U; IVP_LVNX8U_XP(vecCoeff21U, pvecCoeff, coeffPitch1);
        xb_vecNx16U vecCoeff22U; IVP_LVNX8U_XP(vecCoeff22U, pvecCoeff, coeffPitch1);
        xb_vecNx16U vecCoeff23U; IVP_LVNX8U_XP(vecCoeff23U, pvecCoeff, coeffPitch2 - 2 * coeffPitch1);
        xb_vecNx16U vecCoeff31U; IVP_LVNX8U_XP(vecCoeff31U, pvecCoeff, coeffPitch1);
        xb_vecNx16U vecCoeff32U; IVP_LVNX8U_XP(vecCoeff32U, pvecCoeff, coeffPitch1);
        xb_vecNx16U vecCoeff33U; IVP_LVNX8U_XP(vecCoeff33U, pvecCoeff, coeffPitch1);

        /* Subtract corresponding zeropoint from coeff vector */
        xb_vecNx16 vecCoeff11 = IVP_SUBNX16U(vecCoeff11U, zeroPtCoeffU);
        xb_vecNx16 vecCoeff12 = IVP_SUBNX16U(vecCoeff12U, zeroPtCoeffU);
        xb_vecNx16 vecCoeff13 = IVP_SUBNX16U(vecCoeff13U, zeroPtCoeffU);
        xb_vecNx16 vecCoeff21 = IVP_SUBNX16U(vecCoeff21U, zeroPtCoeffU);
        xb_vecNx16 vecCoeff22 = IVP_SUBNX16U(vecCoeff22U, zeroPtCoeffU);
        xb_vecNx16 vecCoeff23 = IVP_SUBNX16U(vecCoeff23U, zeroPtCoeffU);
        xb_vecNx16 vecCoeff31 = IVP_SUBNX16U(vecCoeff31U, zeroPtCoeffU);
        xb_vecNx16 vecCoeff32 = IVP_SUBNX16U(vecCoeff32U, zeroPtCoeffU);
        xb_vecNx16 vecCoeff33 = IVP_SUBNX16U(vecCoeff33U, zeroPtCoeffU);

        // REPLICATE higher half with lower half
        vecCoeff11 = IVP_SELNX16I(vecCoeff11, vecCoeff11, IVP_SELI_16B_EXTRACT_LO_HALVES);
        vecCoeff12 = IVP_SELNX16I(vecCoeff12, vecCoeff12, IVP_SELI_16B_EXTRACT_LO_HALVES);
        vecCoeff13 = IVP_SELNX16I(vecCoeff13, vecCoeff13, IVP_SELI_16B_EXTRACT_LO_HALVES);
        vecCoeff21 = IVP_SELNX16I(vecCoeff21, vecCoeff21, IVP_SELI_16B_EXTRACT_LO_HALVES);
        vecCoeff22 = IVP_SELNX16I(vecCoeff22, vecCoeff22, IVP_SELI_16B_EXTRACT_LO_HALVES);
        vecCoeff23 = IVP_SELNX16I(vecCoeff23, vecCoeff23, IVP_SELI_16B_EXTRACT_LO_HALVES);
        vecCoeff31 = IVP_SELNX16I(vecCoeff31, vecCoeff31, IVP_SELI_16B_EXTRACT_LO_HALVES);
        vecCoeff32 = IVP_SELNX16I(vecCoeff32, vecCoeff32, IVP_SELI_16B_EXTRACT_LO_HALVES);
        vecCoeff33 = IVP_SELNX16I(vecCoeff33, vecCoeff33, IVP_SELI_16B_EXTRACT_LO_HALVES);

        for (y = 0; y < outH; y+=2)
        { /* walk down the rows */
          /* Input Data Pointers */
          int8_t *pData = pInData + ch + y * stride * inDataPitch2;
          int8_t *pData1 = pInData + ch + (y+1) * stride * inDataPitch2;

          pvecData1 = (xb_vecNx8U *) (pData);
          pvecData2 = (xb_vecNx8U *) (pData + inDataPitch2);
          pvecData3 = (xb_vecNx8U *) (pData + 2*inDataPitch2);
          pvecData4 = (xb_vecNx8U *) (pData1);
          pvecData5 = (xb_vecNx8U *) (pData1 + inDataPitch2);
          pvecData6 = (xb_vecNx8U *) (pData1 + 2*inDataPitch2);

          /* Input loads*/
          /* ky = 0*/
          valign vaData1 = IVP_LANX8U_PP(pvecData1);
          xb_vecNx16U vecData11U; IVP_LANX8U_XP(vecData11U, vaData1, pvecData1, 2*inDataPitch1);
          /* ky = 1*/
          valign vaData2 = IVP_LANX8U_PP(pvecData2);
          xb_vecNx16U vecData21U; IVP_LANX8U_XP(vecData21U, vaData2, pvecData2, 2*inDataPitch1);
          /* ky = 2*/
          valign vaData3 = IVP_LANX8U_PP(pvecData3);
          xb_vecNx16U vecData31U; IVP_LANX8U_XP(vecData31U, vaData3, pvecData3, 2*inDataPitch1);

          vaData1 = IVP_LANX8U_PP(pvecData4);
          xb_vecNx16U vecData11_2U; IVP_LANX8U_XP(vecData11_2U, vaData1, pvecData4, 2*inDataPitch1);
          vaData2 = IVP_LANX8U_PP(pvecData5);
          xb_vecNx16U vecData21_2U; IVP_LANX8U_XP(vecData21_2U, vaData2, pvecData5, 2*inDataPitch1);
          vaData3 = IVP_LANX8U_PP(pvecData6);
          xb_vecNx16U vecData31_2U; IVP_LANX8U_XP(vecData31_2U, vaData3, pvecData6, 2*inDataPitch1);
          /* Subtract corresponding zeropoint from input vector*/
          xb_vecNx16 vecData11 = IVP_SUBNX16U(vecData11U, zeroPtInputU);
          xb_vecNx16 vecData21 = IVP_SUBNX16U(vecData21U, zeroPtInputU);
          xb_vecNx16 vecData31 = IVP_SUBNX16U(vecData31U, zeroPtInputU);

          xb_vecNx16 vecData11_2 = IVP_SUBNX16U(vecData11_2U, zeroPtInputU);
          xb_vecNx16 vecData21_2 = IVP_SUBNX16U(vecData21_2U, zeroPtInputU);
          xb_vecNx16 vecData31_2 = IVP_SUBNX16U(vecData31_2U, zeroPtInputU);
          for (x = 0; x < outW; x+=2)
          { /* walk across the columns */
            /* Output Data pointer */
            int8_t *pOut = pOutData + (x * outDataPitch1 + y * outDataPitch2);
            int8_t *pOut1 = pOutData + (x * outDataPitch1 + (y+1) * outDataPitch2);

            /* Initialize accumulator with bias values */
            accSum1 = accSum;
            accSum2 = accSum;
            /* ky = 0*/
            vaData1 = IVP_LANX8U_PP(pvecData1);
            xb_vecNx16U vecData12U; IVP_LANX8U_XP(vecData12U, vaData1, pvecData1, 2*inDataPitch1);
            xb_vecNx16U vecData13U; IVP_LANX8U_XP(vecData13U, vaData1, pvecData1, 2*inDataPitch1);
            /* ky = 1*/
            vaData2 = IVP_LANX8U_PP(pvecData2);
            xb_vecNx16U vecData22U; IVP_LANX8U_XP(vecData22U, vaData2, pvecData2, 2*inDataPitch1);
            xb_vecNx16U vecData23U; IVP_LANX8U_XP(vecData23U, vaData2, pvecData2, 2*inDataPitch1);
            /* ky = 2*/
            vaData3 = IVP_LANX8U_PP(pvecData3);
            xb_vecNx16U vecData32U; IVP_LANX8U_XP(vecData32U, vaData3, pvecData3, 2*inDataPitch1);
            xb_vecNx16U vecData33U; IVP_LANX8U_XP(vecData33U, vaData3, pvecData3, 2*inDataPitch1);
            vaData1 = IVP_LANX8U_PP(pvecData4);
            xb_vecNx16U vecData12_2U; IVP_LANX8U_XP(vecData12_2U, vaData1, pvecData4, 2*inDataPitch1);
            xb_vecNx16U vecData13_2U; IVP_LANX8U_XP(vecData13_2U, vaData1, pvecData4, 2*inDataPitch1);
            vaData2 = IVP_LANX8U_PP(pvecData5);
            xb_vecNx16U vecData22_2U; IVP_LANX8U_XP(vecData22_2U, vaData2, pvecData5, 2*inDataPitch1);
            xb_vecNx16U vecData23_2U; IVP_LANX8U_XP(vecData23_2U, vaData2, pvecData5, 2*inDataPitch1);
            vaData3 = IVP_LANX8U_PP(pvecData6);
            xb_vecNx16U vecData32_2U; IVP_LANX8U_XP(vecData32_2U, vaData3, pvecData6, 2*inDataPitch1);
            xb_vecNx16U vecData33_2U; IVP_LANX8U_XP(vecData33_2U, vaData3, pvecData6, 2*inDataPitch1);
            /* Subtract corresponding zeropoint from input vector*/
            xb_vecNx16 vecData12 = IVP_SUBNX16U(vecData12U, zeroPtInputU);
            xb_vecNx16 vecData22 = IVP_SUBNX16U(vecData22U, zeroPtInputU);
            xb_vecNx16 vecData32 = IVP_SUBNX16U(vecData32U, zeroPtInputU);

            xb_vecNx16 vecData13 = IVP_SUBNX16U(vecData13U, zeroPtInputU);
            xb_vecNx16 vecData23 = IVP_SUBNX16U(vecData23U, zeroPtInputU);
            xb_vecNx16 vecData33 = IVP_SUBNX16U(vecData33U, zeroPtInputU);
            xb_vecNx16 vecData12_2 = IVP_SUBNX16U(vecData12_2U, zeroPtInputU);
            xb_vecNx16 vecData22_2 = IVP_SUBNX16U(vecData22_2U, zeroPtInputU);
            xb_vecNx16 vecData32_2 = IVP_SUBNX16U(vecData32_2U, zeroPtInputU);
            xb_vecNx16 vecData13_2 = IVP_SUBNX16U(vecData13_2U, zeroPtInputU);
            xb_vecNx16 vecData23_2 = IVP_SUBNX16U(vecData23_2U, zeroPtInputU);
            xb_vecNx16 vecData33_2 = IVP_SUBNX16U(vecData33_2U, zeroPtInputU);

            /* form new */ // 0,1 , 2,3, 4,5 <==> 0,2,1,3,2,4
            xb_vecNx16 t11, t12;
            IVP_DSELNX16(t12,t11, vecData12,vecData11,*(xb_vec2Nx8 *)&dselpat_lo_hi);
            xb_vecNx16 t13 = IVP_SELNX16I(vecData13, vecData12, IVP_SELI_16B_EXTRACT_LO_HALVES);

            xb_vecNx16 t21, t22;
            IVP_DSELNX16(t22,t21, vecData22,vecData21,*(xb_vec2Nx8 *)&dselpat_lo_hi);
            xb_vecNx16 t23 = IVP_SELNX16I(vecData23, vecData22, IVP_SELI_16B_EXTRACT_LO_HALVES);

            xb_vecNx16 t31, t32;
            IVP_DSELNX16(t32,t31, vecData32,vecData31,*(xb_vec2Nx8 *)&dselpat_lo_hi);
            xb_vecNx16 t33 = IVP_SELNX16I(vecData33, vecData32, IVP_SELI_16B_EXTRACT_LO_HALVES);

            IVP_MULPANX16(accSum1, vecCoeff11, t11, vecCoeff12, t12);
            IVP_MULPANX16(accSum1, vecCoeff13, t13, vecCoeff21, t21);
            IVP_MULPANX16(accSum1, vecCoeff22, t22, vecCoeff23, t23);
            IVP_MULPANX16(accSum1, vecCoeff31, t31, vecCoeff32, t32);
            IVP_MULANX16(accSum1, vecCoeff33,  t33);

            IVP_DSELNX16(t12,t11, vecData12_2,vecData11_2,*(xb_vec2Nx8 *)&dselpat_lo_hi);
            t13 = IVP_SELNX16I(vecData13_2, vecData12_2, IVP_SELI_16B_EXTRACT_LO_HALVES);
            IVP_DSELNX16(t22,t21, vecData22_2,vecData21_2,*(xb_vec2Nx8 *)&dselpat_lo_hi);
            t23 = IVP_SELNX16I(vecData23_2, vecData22_2, IVP_SELI_16B_EXTRACT_LO_HALVES);
            IVP_DSELNX16(t32,t31, vecData32_2,vecData31_2,*(xb_vec2Nx8 *)&dselpat_lo_hi);
            t33 = IVP_SELNX16I(vecData33_2, vecData32_2, IVP_SELI_16B_EXTRACT_LO_HALVES);
            IVP_MULPANX16(accSum2, vecCoeff11, t11, vecCoeff12, t12);
            IVP_MULPANX16(accSum2, vecCoeff13, t13, vecCoeff21, t21);
            IVP_MULPANX16(accSum2, vecCoeff22, t22, vecCoeff23, t23);
            IVP_MULPANX16(accSum2, vecCoeff31, t31, vecCoeff32, t32);
            IVP_MULANX16(accSum2, vecCoeff33,  t33);
            /* Pack, Output Scale, Output Shift and clamping */
            xb_vecNx16 vec16H1W1AB;
            ADD48_ZPT_PACK_SCALE_SHIFT_CLAMP_LIMITS(vec16H1W1AB, accSum1, \
                                                    zeroPtOutputU, leftShift,rightShift, quantScale, maxLimU, minLimU);

            xb_vecNx16 vec16H1W1AB1;
            ADD48_ZPT_PACK_SCALE_SHIFT_CLAMP_LIMITS(vec16H1W1AB1, accSum2, \
                                                    zeroPtOutputU, leftShift,rightShift, quantScale, maxLimU, minLimU);
            /* Store the output vecOut1 along the output depth */
            pdvecOut = (xb_vecNx8U *) (pOut + ch);
            IVP_SAVNX8U_XP(vec16H1W1AB, vaOutData, pdvecOut, 2*remainingCh);
            IVP_SAPOSNX8U_FP(vaOutData, pdvecOut);
            pdvecOut1 = (xb_vecNx8U *) (pOut1 + ch);
            IVP_SAVNX8U_XP(vec16H1W1AB1, vaOutData1, pdvecOut1, 2*remainingCh);
            IVP_SAPOSNX8U_FP(vaOutData1, pdvecOut1);

            vecData11 = vecData13;
            vecData21 = vecData23;
            vecData31 = vecData33;
            vecData11_2 = vecData13_2;
            vecData21_2 = vecData23_2;
            vecData31_2 = vecData33_2;
          } /* End for (x = 0; x < outW; x ++) */
        }   /* End for (y = 0; y < outH; y ++) */
      }     /* End for (ch = 0; ch < numCh; ch += 2 * XCHAL_IVPN_SIMD_WIDTH) */
   }
}

XI_ERR_TYPE xiDepthwiseConvolveA2D_S_3x3_U8Ca2_MOD_DWH(
  const xi_pTile3D inTile,
  const xi_pTile3D coeffTile,
  const xi_pArray biasArray,
  xi_pTile3D outTile,
  const xi_cnna_conv_params *param
  )
{
  /* Error Checks */
  XI_ERROR_CHECKS()
  {
    XI_CHECK_TILE3D(inTile);
    XI_CHECK_TILE3D(outTile);
    XI_CHECK_TILE3D(coeffTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_TILE4D_IN_DRAM_BOUNDARY(coeffTile);
    XI_CHECK_POINTER(param);
    XI_CHECK_TILE3D_U8(inTile);
    XI_CHECK_TILE3D_U8(outTile);
    XI_CHECK_TILE3D_U8(coeffTile);
    XI_CHECK_ARRAY_S32(biasArray);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(inTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(coeffTile, outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(biasArray, outTile);
    XI_CHECK_TILE3D_DATA_ORDER(inTile, XI_DWH);
    XI_CHECK_TILE3D_DATA_ORDER(outTile, XI_DWH);
    XI_CHECK_TILE3D_DATA_ORDER(coeffTile, XI_DWH);
    XI_CHECK_KERNEL_SIZE_DEPTHWISE(coeffTile, 3);
    XI_CHECK_TILE3D_EDGE(inTile, 1);
    XI_CHECK_ERROR(((XI_CNNA_CONV_GET_STRIDEX(param) == 1) || (XI_CNNA_CONV_GET_STRIDEX(param) == 2) || (XI_CNNA_CONV_GET_STRIDEX(param) == 4)) && \
                   ((XI_CNNA_CONV_GET_STRIDEY(param) == 1) || (XI_CNNA_CONV_GET_STRIDEY(param) == 2) || (XI_CNNA_CONV_GET_STRIDEY(param) == 4)), \
                   XI_ERR_BADARG, "Provided stride value is not supported.");

    XI_CHECK_ERROR(XI_CNNA_CONV_GET_STRIDEX(param) == XI_CNNA_CONV_GET_STRIDEY(param), \
                       XI_ERR_BADARG, "StrideX and StrideY must be same");
    XI_CHECK_TILE3D_IALIGNMENT_2NX8(coeffTile);
    XI_CHECK_CONSISTENCYA_DEPTHWISE_MOD_DWH(inTile, coeffTile, biasArray, outTile, param);
    XI_CHECK_ERROR((XI_CNNA_CONV_GET_OUTPUT_SHIFT(param) < 32) && (XI_CNNA_CONV_GET_OUTPUT_SHIFT(param) > -32), \
                   XI_ERR_NORM, "The output shift value is invalid");
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

  /* Setting the limits for output data according to ReLu is enabled or not*/
  const uint8_t minLimU = enableReLu ? reluMinU : 0;
  const uint8_t maxLimU = enableReLu ? reluMaxU : UCHAR_MAX;

  int32_t leftShift = outShift < 0 ? -outShift : 0;
  int32_t rightShift = outShift < 0 ? 0 : outShift;

  /* If quantization scale = 0 , output tile is filled with
   * CLAMP(zeroPtOutputU, minLimU, maxLimU)
   */
  uint8_t tileFillValue;
  if (quantScale == 0)
  {
    tileFillValue = CLAMP(zeroPtOutputU, minLimU, maxLimU);
    return(xiFillTile3D_I8(outTile, tileFillValue, 0));
  }

  /*Check input stride, if it is equal to 2 call stride 2 sub-variant
   * If it is equal to 4 call stride 4 sub-variant
   * else continue for stride 1
   * */

  if (XI_CNNA_CONV_GET_STRIDE(param) == 2)
  {
    depthwiseConvolveA2D_S_3x3j2_U8Ca2_MOD_DWH(inTile, coeffTile, biasArray, outTile, param);
    return(XI_ERROR_STATUS());
  }
  else if (XI_CNNA_CONV_GET_STRIDE(param) != 1)
  {
    return(XI_ERROR_STATUS());
  }

  /* Getting parameters from the tile structures */
  const int32_t outW  = XI_TILE3D_GET_DIM2(outTile);
  const int32_t outH  = XI_TILE3D_GET_DIM3(outTile);
  const int32_t numCh = XI_TILE3D_GET_DIM1(inTile);

  /* Kernel Size (DWH) */
  const int32_t kSizeU = XI_TILE3D_GET_DIM2(coeffTile);

  /* Data Pointers of input, output, coefficient and bias data */
  int8_t *pInData    = (int8_t *) XI_TILE3D_GET_DATA_PTR(inTile);
  int8_t *pOutData   = (int8_t *) XI_TILE3D_GET_DATA_PTR(outTile);
  int8_t *pCoeffData = (int8_t *) XI_TILE3D_GET_DATA_PTR(coeffTile);
  int32_t *pBiasData = (int32_t *) XI_ARRAY_GET_DATA_PTR(biasArray);

  /* Pitches of Coefficient Data (DWH) in dim1 and dim2 */
  const int32_t coeffPitch1 = XI_TILE3D_GET_DIM1_PITCH(coeffTile);
  const int32_t coeffPitch2 = XI_TILE3D_GET_DIM2_PITCH(coeffTile);

  /* Pitches of Input Data (DWH) in dim1 and dim2 */
  const int32_t inDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(inTile);

  /* Pitch of Output Data (DWH) in dim1 and dim2 */
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);


  /* Move pointer to the start of the data (including edge) */
  pInData = &pInData[-((kSizeU / 2) * inDataPitch1 + (kSizeU / 2) * inDataPitch2)];

  /* Variable Declarations */
  int32_t ch, x, y;
  valign vaOutData = IVP_ZALIGN();
  valign vaOutData1 = IVP_ZALIGN();

  xb_vecN_2x32v* restrict phvecBias1;
  xb_vecNx8U* restrict pvecCoeff;
  xb_vecNx8U* restrict pvecData1;
  xb_vecNx8U* restrict pvecData2;
  xb_vecNx8U* restrict pvecData3;
  xb_vecNx8U* restrict pvecData4;

  xb_vecNx8U* restrict pdvecOut;
  xb_vecNx8U* restrict pdvecOut1;

#ifdef MULP2NX8_FIXUP_OPT
  xb_vecN_2x32v* restrict phvecBias2;
  xb_vec2Nx8U* restrict pdvecCoeff;
  xb_vec2Nx8U* restrict pdvecData1;
  xb_vec2Nx8U* restrict pdvecData2;
  xb_vec2Nx8U* restrict pdvecData3;
  xb_vec2Nx8U* restrict pdvecData4;
  xb_vec2Nx8U* restrict pdvecOut1;
#endif
  if ((numCh == 1) && (inDataPitch1 == 1) && (outDataPitch1 == 1))
  {
    /* Pointer for Coefficient Load */
    pvecCoeff = (xb_vecNx8U *) (pCoeffData);
    /* 9 Coefficient Loads */
    xb_vecNx16U vecCoeff11U; vecCoeff11U = (xb_vecNx16U) ((uint8_t ) pCoeffData[0 * coeffPitch2 + 0 * coeffPitch1]);
    xb_vecNx16U vecCoeff12U; vecCoeff12U = (xb_vecNx16U) ((uint8_t ) pCoeffData[0 * coeffPitch2 + 1 * coeffPitch1]);
    xb_vecNx16U vecCoeff13U; vecCoeff13U = (xb_vecNx16U) ((uint8_t ) pCoeffData[0 * coeffPitch2 + 2 * coeffPitch1]);
    xb_vecNx16U vecCoeff21U; vecCoeff21U = (xb_vecNx16U) ((uint8_t ) pCoeffData[1 * coeffPitch2 + 0 * coeffPitch1]);
    xb_vecNx16U vecCoeff22U; vecCoeff22U = (xb_vecNx16U) ((uint8_t ) pCoeffData[1 * coeffPitch2 + 1 * coeffPitch1]);
    xb_vecNx16U vecCoeff23U; vecCoeff23U = (xb_vecNx16U) ((uint8_t ) pCoeffData[1 * coeffPitch2 + 2 * coeffPitch1]);
    xb_vecNx16U vecCoeff31U; vecCoeff31U = (xb_vecNx16U) ((uint8_t ) pCoeffData[2 * coeffPitch2 + 0 * coeffPitch1]);
    xb_vecNx16U vecCoeff32U; vecCoeff32U = (xb_vecNx16U) ((uint8_t ) pCoeffData[2 * coeffPitch2 + 1 * coeffPitch1]);
    xb_vecNx16U vecCoeff33U; vecCoeff33U = (xb_vecNx16U) ((uint8_t ) pCoeffData[2 * coeffPitch2 + 2 * coeffPitch1]);

    /* Subtract corresponding zeropoint from coeff vector */
    xb_vecNx16 vecCoeff11 = IVP_SUBNX16U(vecCoeff11U, zeroPtCoeffU);
    xb_vecNx16 vecCoeff12 = IVP_SUBNX16U(vecCoeff12U, zeroPtCoeffU);
    xb_vecNx16 vecCoeff13 = IVP_SUBNX16U(vecCoeff13U, zeroPtCoeffU);
    xb_vecNx16 vecCoeff21 = IVP_SUBNX16U(vecCoeff21U, zeroPtCoeffU);
    xb_vecNx16 vecCoeff22 = IVP_SUBNX16U(vecCoeff22U, zeroPtCoeffU);
    xb_vecNx16 vecCoeff23 = IVP_SUBNX16U(vecCoeff23U, zeroPtCoeffU);
    xb_vecNx16 vecCoeff31 = IVP_SUBNX16U(vecCoeff31U, zeroPtCoeffU);
    xb_vecNx16 vecCoeff32 = IVP_SUBNX16U(vecCoeff32U, zeroPtCoeffU);
    xb_vecNx16 vecCoeff33 = IVP_SUBNX16U(vecCoeff33U, zeroPtCoeffU);
    xb_vecNx48 accSum1;

    xb_vecN_2x32v hvecBias = pBiasData[0];
    for (x = 0; x < outW; x += XCHAL_IVPN_SIMD_WIDTH)
    {             /* walk across the columns */
      int8_t *pData = (pInData + x);
      pvecData1 = (xb_vecNx8U *) pData;
      pvecData2 = (xb_vecNx8U *) (pData + inDataPitch2);
      //load first line
      valign vaData1 = IVP_LANX8U_PP(pvecData1);
      xb_vecNx16U vecData10U, vecData1NU;
      IVP_LAVNX8U_XP(vecData10U, vaData1, pvecData1, inDataPitch2 - x);
      IVP_LAVNX8U_XP(vecData1NU, vaData1, pvecData1, inDataPitch2 - x - XCHAL_IVPN_SIMD_WIDTH);

      //load second line
      valign vaData2 = IVP_LANX8U_PP(pvecData2);
      xb_vecNx16U vecData20U, vecData2NU;
      IVP_LAVNX8U_XP(vecData20U, vaData2, pvecData2, inDataPitch2 - x);
      IVP_LAVNX8U_XP(vecData2NU, vaData2, pvecData2, inDataPitch2 - x - XCHAL_IVPN_SIMD_WIDTH);

       /* Subtract corresponding zeropoint from input vector*/
      vecData10U = IVP_SUBNX16U(vecData10U, zeroPtInputU);
      vecData1NU = IVP_SUBNX16U(vecData1NU, zeroPtInputU);
      vecData20U = IVP_SUBNX16U(vecData20U, zeroPtInputU);
      vecData2NU = IVP_SUBNX16U(vecData2NU, zeroPtInputU);

      xb_vecNx16 vecData11 = vecData10U;
      xb_vecNx16 vecData12 = IVP_SELNX16I(vecData1NU, vecData10U, IVP_SELI_16B_ROTATE_RIGHT_1);
      xb_vecNx16 vecData13 = IVP_SELNX16I(vecData1NU, vecData10U, IVP_SELI_16B_ROTATE_RIGHT_2);

      xb_vecNx16 vecData21 = vecData20U;
      xb_vecNx16 vecData22 = IVP_SELNX16I(vecData2NU, vecData20U, IVP_SELI_16B_ROTATE_RIGHT_1);
      xb_vecNx16 vecData23 = IVP_SELNX16I(vecData2NU, vecData20U, IVP_SELI_16B_ROTATE_RIGHT_2);

      for (y = 0; y < outH; y++)
      {                   /* walk down the rows */
        //load current line
        pvecData3 = (xb_vecNx8U *) (pData + ((y + 2) * inDataPitch2));
        valign vaData3 = IVP_LANX8U_PP(pvecData3);
        xb_vecNx16U vecData30U, vecData3NU, vecData31U, vecData32U, vecData33U;
        IVP_LAVNX8U_XP(vecData30U, vaData3, pvecData3, inDataPitch2 - x);
        IVP_LAVNX8U_XP(vecData3NU, vaData3, pvecData3, inDataPitch2 - x - XCHAL_IVPN_SIMD_WIDTH);
        vecData31U = vecData30U;
        vecData32U = IVP_SELNX16I(vecData3NU, vecData30U, IVP_SELI_16B_ROTATE_RIGHT_1);
        vecData33U = IVP_SELNX16I(vecData3NU, vecData30U, IVP_SELI_16B_ROTATE_RIGHT_2);

        xb_vecNx16 vecData31 = IVP_SUBNX16U(vecData31U, zeroPtInputU);
        xb_vecNx16 vecData32 = IVP_SUBNX16U(vecData32U, zeroPtInputU);
        xb_vecNx16 vecData33 = IVP_SUBNX16U(vecData33U, zeroPtInputU);

        /* Initialize accumulator with bias values */
        accSum1 = IVP_CVT48SNX32(hvecBias, hvecBias);

        IVP_MULPANX16(accSum1, vecCoeff11, vecData11, vecCoeff12, vecData12);
        IVP_MULPANX16(accSum1, vecCoeff13, vecData13, vecCoeff21, vecData21);
        IVP_MULPANX16(accSum1, vecCoeff22, vecData22, vecCoeff23, vecData23);
        IVP_MULPANX16(accSum1, vecCoeff31, vecData31, vecCoeff32, vecData32);
        IVP_MULANX16(accSum1, vecCoeff33, vecData33);

        /* Pack, Output Scale, Output Shift and clamping */
        xb_vecNx16 vec16H1W1AB;

        ADD48_ZPT_PACK_SCALE_SHIFT_CLAMP_LIMITS(vec16H1W1AB, accSum1, \
                                                zeroPtOutputU, leftShift,rightShift, quantScale, maxLimU, minLimU);

        int8_t *pOut = pOutData + (x * outDataPitch1 + y * outDataPitch2);
        pdvecOut = (xb_vecNx8U *) (pOut);
        /* Store the output vecOut1 along the output depth */

        IVP_SAVNX8U_XP(vec16H1W1AB, vaOutData, pdvecOut, outW - x);
        IVP_SAPOSNX8U_FP(vaOutData, pdvecOut);

        vecData11 = vecData21;
        vecData12 = vecData22;
        vecData13 = vecData23;

        vecData21 = vecData31;
        vecData22 = vecData32;
        vecData23 = vecData33;
      }
    }
  }
  else
#ifdef MULP2NX8_FIXUP_OPT
  {
    /* Kernel height and width loops are completely unrolled */
    /* Loops Start */
    for (ch = 0; ch < numCh; ch += XCHAL_IVPN_SIMD_WIDTH << 1)
    {             /* walk across the channels */
      /* To handle corner case when number of channels
      * is not a multiple of  2 * XCHAL_IVPN_SIMD_WIDTH*/
      int32_t remainingCh = (numCh - ch) < (XCHAL_IVPN_SIMD_WIDTH << 1) ? \
        (numCh - ch) : (XCHAL_IVPN_SIMD_WIDTH << 1);
      int32_t remBiasLoad = (remainingCh > XCHAL_IVPN_SIMD_WIDTH) ? 1 : 0;
      /* Initialize accumulators with bias values */
      xb_vec2Nx24 daccSum, daccSum1;
      phvecBias1 = (xb_vecN_2x32v *)(pBiasData + ch);
      phvecBias2 = (xb_vecN_2x32v*)(pBiasData + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);

      {
        valign vaBias;
        xb_vecN_2x32v hvecBias1, hvecBias2;
        vaBias = IVP_LAN_2X32_PP(phvecBias1);
        IVP_L2ANX32_IP(hvecBias2, hvecBias1, vaBias, phvecBias1);
        int32_t zdxzc = zeroPtCoeffU * zeroPtInputU * 9;
        hvecBias1 = IVP_ADDN_2X32(hvecBias1, zdxzc);
        hvecBias2 = IVP_ADDN_2X32(hvecBias2, zdxzc);
        daccSum = IVP_CVT24UNX32L(hvecBias2, hvecBias1);
        vaBias = IVP_LAN_2X32_PP(phvecBias2);
        IVP_L2ANX32_IP(hvecBias2, hvecBias1, vaBias, phvecBias2);
        hvecBias1 = IVP_ADDN_2X32(hvecBias1, zdxzc);
        hvecBias2 = IVP_ADDN_2X32(hvecBias2, zdxzc);
        IVP_CVT24UNX32H(daccSum, hvecBias2, hvecBias1);
      }


      /* Pointer for Coefficient Load */
      pdvecCoeff = (xb_vec2Nx8U *)(pCoeffData + ch);

      /* 9 Coefficient Loads */
      xb_vec2Nx8U dvecCoeff11U; IVP_LV2NX8U_XP(dvecCoeff11U, pdvecCoeff, coeffPitch1);
      xb_vec2Nx8U dvecCoeff12U; IVP_LV2NX8U_XP(dvecCoeff12U, pdvecCoeff, coeffPitch1);
      xb_vec2Nx8U dvecCoeff13U; IVP_LV2NX8U_XP(dvecCoeff13U, pdvecCoeff, coeffPitch2 \
        - 2 * coeffPitch1);/* Subtract corresponding zeropoint from coeff vector */
      xb_vec2Nx8U dvecCoeff21U; IVP_LV2NX8U_XP(dvecCoeff21U, pdvecCoeff, coeffPitch1);
      xb_vec2Nx8U dvecCoeff22U; IVP_LV2NX8U_XP(dvecCoeff22U, pdvecCoeff, coeffPitch1);
      xb_vec2Nx8U dvecCoeff23U; IVP_LV2NX8U_XP(dvecCoeff23U, pdvecCoeff, coeffPitch2 \
        - 2 * coeffPitch1);
      xb_vec2Nx8U dvecCoeff31U; IVP_LV2NX8U_XP(dvecCoeff31U, pdvecCoeff, coeffPitch1);
      xb_vec2Nx8U dvecCoeff32U; IVP_LV2NX8U_XP(dvecCoeff32U, pdvecCoeff, coeffPitch1);
      xb_vec2Nx8U dvecCoeff33U; IVP_LV2NX8U_XP(dvecCoeff33U, pdvecCoeff, coeffPitch1);

      /*zeroPtCoeffU * coefficient*/
      IVP_MULUUPS2NX8(daccSum, dvecCoeff11U, zeroPtInputU, dvecCoeff12U, zeroPtInputU);
      IVP_MULUUPS2NX8(daccSum, dvecCoeff13U, zeroPtInputU, dvecCoeff21U, zeroPtInputU);
      IVP_MULUUPS2NX8(daccSum, dvecCoeff22U, zeroPtInputU, dvecCoeff23U, zeroPtInputU);
      IVP_MULUUPS2NX8(daccSum, dvecCoeff31U, zeroPtInputU, dvecCoeff32U, zeroPtInputU);
      IVP_MULUUS2NX8(daccSum, dvecCoeff33U, zeroPtInputU);

      for (y = 0; y < outH; y++)
      {                   /* walk down the rows */
        /* Input Data Pointers */
        int8_t *pData = pInData + ch + y * stride * inDataPitch2;

        pdvecData1 = (xb_vec2Nx8U *)pData;
        pdvecData2 = (xb_vec2Nx8U *)(pData + 2 * inDataPitch1);
        pdvecData3 = (xb_vec2Nx8U *)(pData + inDataPitch2 + 2 * inDataPitch1);
        pdvecData4 = (xb_vec2Nx8U *)(pData + 2 * inDataPitch2 + 2 * inDataPitch1);

        /* Input loads*/
        /* ky = 0*/
        valign vaData2 = IVP_LA2NX8U_PP(pdvecData1);
        xb_vec2Nx8U dvecData11U; IVP_LA2NX8U_XP(dvecData11U, vaData2, pdvecData1, inDataPitch1);
        vaData2 = IVP_LA2NX8U_PP(pdvecData1);
        xb_vec2Nx8U dvecData12U;
        IVP_LA2NX8U_XP(dvecData12U, vaData2, pdvecData1, -inDataPitch1 + inDataPitch2);

        /* ky = 1*/
        vaData2 = IVP_LA2NX8U_PP(pdvecData1);
        xb_vec2Nx8U dvecData21U; IVP_LA2NX8U_XP(dvecData21U, vaData2, pdvecData1, inDataPitch1);
        vaData2 = IVP_LA2NX8U_PP(pdvecData1);
        xb_vec2Nx8U dvecData22U;
        IVP_LA2NX8U_XP(dvecData22U, vaData2, pdvecData1, -inDataPitch1 + inDataPitch2);

        /* ky = 2*/
        vaData2 = IVP_LA2NX8U_PP(pdvecData1);
        xb_vec2Nx8U dvecData31U; IVP_LA2NX8U_XP(dvecData31U, vaData2, pdvecData1, inDataPitch1);
        vaData2 = IVP_LA2NX8U_PP(pdvecData1);
        xb_vec2Nx8U dvecData32U;
        IVP_LA2NX8U_XP(dvecData32U, vaData2, pdvecData1, -inDataPitch1 + inDataPitch2);

        /* Output Data pointer */
        int8_t *pOut = pOutData + (y * outDataPitch2 + ch);
        pdvecOut1 = (xb_vec2Nx8U *)(pOut);
        for (x = 0; x < outW; x++)
        {                         /* walk across the columns */

          /* Initialize accumulator with bias values */
          daccSum1 = daccSum;

          /* ky = 0*/
          valign vaData5 = IVP_LA2NX8U_PP(pdvecData2);
          xb_vec2Nx8U dvecData13U; IVP_LA2NX8U_XP(dvecData13U, vaData5, pdvecData2, inDataPitch1);
          /* ky = 1*/
          valign vaData6 = IVP_LA2NX8U_PP(pdvecData3);
          xb_vec2Nx8U dvecData23U; IVP_LA2NX8U_XP(dvecData23U, vaData6, pdvecData3, inDataPitch1);
          /* ky = 2*/
          valign vaData7 = IVP_LA2NX8U_PP(pdvecData4);
          xb_vec2Nx8U dvecData33U; IVP_LA2NX8U_XP(dvecData33U, vaData7, pdvecData4, inDataPitch1);
          /*Multiply and accumulate input data vector and coeff vector */
          IVP_MULUUPA2NX8(daccSum1, dvecCoeff11U, dvecData11U, dvecCoeff12U, dvecData12U);
          IVP_MULUUPA2NX8(daccSum1, dvecCoeff13U, dvecData13U, dvecCoeff21U, dvecData21U);
          IVP_MULUUPA2NX8(daccSum1, dvecCoeff22U, dvecData22U, dvecCoeff23U, dvecData23U);
          IVP_MULUUPA2NX8(daccSum1, dvecCoeff31U, dvecData31U, dvecCoeff32U, dvecData32U);
          IVP_MULUUPA2NX8(daccSum1, dvecCoeff33U, dvecData33U, 0, 0);

          /* Multiply and subtract input data vector and zero point of coeff  */
          IVP_MULUUPS2NX8(daccSum1, zeroPtCoeffU, dvecData11U, zeroPtCoeffU, dvecData12U);
          IVP_MULUUPS2NX8(daccSum1, zeroPtCoeffU, dvecData13U, zeroPtCoeffU, dvecData21U);
          IVP_MULUUPS2NX8(daccSum1, zeroPtCoeffU, dvecData22U, zeroPtCoeffU, dvecData23U);
          IVP_MULUUPS2NX8(daccSum1, zeroPtCoeffU, dvecData31U, zeroPtCoeffU, dvecData32U);
          IVP_MULUUPS2NX8(daccSum1, zeroPtCoeffU, dvecData33U, 0, 0);

          xb_vec2Nx8 dvecOut;
          /* The accumulated sum which is now in 24 bits wvec registers is moved to 32 bit  */
          /*  vec registers to handle the addition of bias and fixup values to it. */
          /* H -height W-width */
          /* Moving accumlated sum H1-W1 to normal vec registers */
          xb_vecN_2x32v hvecOutH1W1A, hvecOutH1W1B, hvecOutH1W1C, hvecOutH1W1D;
          xb_vecN_2x64w dProd1, dProd2, dProd3, dProd4;
          hvecOutH1W1A = IVP_CVT32S2NX24LL(daccSum1);
          hvecOutH1W1B = IVP_CVT32S2NX24LH(daccSum1);
          hvecOutH1W1C = IVP_CVT32S2NX24HL(daccSum1);
          hvecOutH1W1D = IVP_CVT32S2NX24HH(daccSum1);
          /* Pack, Output Scale, Output Shift and clamping */
          /* Handling the 32x32 bit multiplication of accumulated sum with quantScale */
          dProd1 = IVP_MULN_2X32(hvecOutH1W1A, \
            quantScale);
          dProd2 = IVP_MULN_2X32(hvecOutH1W1B, \
            quantScale);
          dProd3 = IVP_MULN_2X32(hvecOutH1W1C, \
            quantScale);
          dProd4 = IVP_MULN_2X32(hvecOutH1W1D, \
            quantScale);

          hvecOutH1W1A = IVP_PACKVRN_2X64W(dProd1, 31 - leftShift);
          hvecOutH1W1B = IVP_PACKVRN_2X64W(dProd2, 31 - leftShift);
          hvecOutH1W1C = IVP_PACKVRN_2X64W(dProd3, 31 - leftShift);
          hvecOutH1W1D = IVP_PACKVRN_2X64W(dProd4, 31 - leftShift);

          /*Pack the result to 16 bits and  saturate to short min and max*/
          vboolN_2 vbA = IVP_LTN_2X32(hvecOutH1W1B, 0);
          hvecOutH1W1B = IVP_ABSN_2X32(hvecOutH1W1B);
          vboolN_2 vbB = IVP_LTN_2X32(hvecOutH1W1A, 0);
          hvecOutH1W1A = IVP_ABSN_2X32(hvecOutH1W1A);
          xb_vecNx16 vecOut0 = IVP_PACKVRNX48(IVP_CVT48SNX32(hvecOutH1W1B, hvecOutH1W1A), rightShift);
          vboolN vbAB = IVP_JOINBN_2(vbA, vbB);
          vecOut0 = IVP_MOVNX16T(IVP_SUBNX16(0, vecOut0), vecOut0, vbAB);

          /* Handling symmetrical rounding */
          vbA = IVP_LTN_2X32(hvecOutH1W1D, 0);
          hvecOutH1W1D = IVP_ABSN_2X32(hvecOutH1W1D);
          vbB = IVP_LTN_2X32(hvecOutH1W1C, 0);
          hvecOutH1W1C = IVP_ABSN_2X32(hvecOutH1W1C);
          xb_vecNx16 vecOut1 = IVP_PACKVRNX48(IVP_CVT48SNX32(hvecOutH1W1D, hvecOutH1W1C), rightShift);
          vbAB = IVP_JOINBN_2(vbA, vbB);
          vecOut1 = IVP_MOVNX16T(IVP_SUBNX16(0, vecOut1), vecOut1, vbAB);

          vecOut0 = IVP_ADDSNX16(vecOut0, zeroPtOutputU);
          vecOut1 = IVP_ADDSNX16(vecOut1, zeroPtOutputU);
          vecOut0 = IVP_MAXNX16(IVP_MINNX16(vecOut0, (xb_vecNx16)maxLimU), \
            (xb_vecNx16)minLimU);
          vecOut1 = IVP_MAXNX16(IVP_MINNX16(vecOut1, (xb_vecNx16)maxLimU), \
            (xb_vecNx16)minLimU);
          dvecOut = IVP_SEL2NX8I(IVP_MOV2NX8_FROMNX16(vecOut1), IVP_MOV2NX8_FROMNX16(vecOut0), \
            IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0);


          /* Store the output vecOut1 along the output depth */
          IVP_SAV2NX8U_XP(dvecOut, vaOutData, pdvecOut1, remainingCh);
          IVP_SAPOS2NX8U_FPXP(vaOutData, pdvecOut1 /*inout*/, outDataPitch1 - (remainingCh));

          /* Reuse input data */
          dvecData11U = dvecData12U;
          dvecData12U = dvecData13U;

          dvecData21U = dvecData22U;
          dvecData22U = dvecData23U;

          dvecData31U = dvecData32U;
          dvecData32U = dvecData33U;
        }         /* End for (x = 0; x < outW; x ++) */
      }           /* End for (y = 0; y < outH; y ++) */
    }             /* End for (ch = 0; ch < numCh; ch += 2 * XCHAL_IVPN_SIMD_WIDTH) */
  }
#else
  {
    /* Kernel height and width loops are completely unrolled */
    /* Loops Start */

     if(numCh!=8)
     {
    for (ch = 0; ch < numCh; ch += XCHAL_IVPN_SIMD_WIDTH)
    {             /* walk across the channels */
      /* To handle corner case when number of channels
       * is not a multiple of  2 * XCHAL_IVPN_SIMD_WIDTH*/
      int32_t remainingCh = XT_MIN((numCh - ch), XCHAL_IVPN_SIMD_WIDTH);

      /* Initialize accumulators with bias values */
            xb_vecNx48 accSum, accSum1,accSum2;
      phvecBias1 = (xb_vecN_2x32v *) (pBiasData + ch);
      valign vaBias;
      xb_vecN_2x32v hvecBias1, hvecBias2;
      vaBias = IVP_LAN_2X32_PP(phvecBias1);
      IVP_L2ANX32_IP(hvecBias2, hvecBias1, vaBias, phvecBias1);
      accSum = IVP_CVT48SNX32(hvecBias2, hvecBias1);

      /* Pointer for Coefficient Load */
      pvecCoeff = (xb_vecNx8U *) (pCoeffData + ch);

      /* 9 Coefficient Loads */
      xb_vecNx16U vecCoeff11U; IVP_LVNX8U_XP(vecCoeff11U, pvecCoeff, coeffPitch1);
      xb_vecNx16U vecCoeff12U; IVP_LVNX8U_XP(vecCoeff12U, pvecCoeff, coeffPitch1);
      xb_vecNx16U vecCoeff13U; IVP_LVNX8U_XP(vecCoeff13U, pvecCoeff, coeffPitch2 - 2 * coeffPitch1);
      xb_vecNx16U vecCoeff21U; IVP_LVNX8U_XP(vecCoeff21U, pvecCoeff, coeffPitch1);
      xb_vecNx16U vecCoeff22U; IVP_LVNX8U_XP(vecCoeff22U, pvecCoeff, coeffPitch1);
      xb_vecNx16U vecCoeff23U; IVP_LVNX8U_XP(vecCoeff23U, pvecCoeff, coeffPitch2 - 2 * coeffPitch1);
      xb_vecNx16U vecCoeff31U; IVP_LVNX8U_XP(vecCoeff31U, pvecCoeff, coeffPitch1);
      xb_vecNx16U vecCoeff32U; IVP_LVNX8U_XP(vecCoeff32U, pvecCoeff, coeffPitch1);
      xb_vecNx16U vecCoeff33U; IVP_LVNX8U_XP(vecCoeff33U, pvecCoeff, coeffPitch1);

      /* Subtract corresponding zeropoint from coeff vector */
      xb_vecNx16 vecCoeff11 = IVP_SUBNX16U(vecCoeff11U, zeroPtCoeffU);
      xb_vecNx16 vecCoeff12 = IVP_SUBNX16U(vecCoeff12U, zeroPtCoeffU);
      xb_vecNx16 vecCoeff13 = IVP_SUBNX16U(vecCoeff13U, zeroPtCoeffU);
      xb_vecNx16 vecCoeff21 = IVP_SUBNX16U(vecCoeff21U, zeroPtCoeffU);
      xb_vecNx16 vecCoeff22 = IVP_SUBNX16U(vecCoeff22U, zeroPtCoeffU);
      xb_vecNx16 vecCoeff23 = IVP_SUBNX16U(vecCoeff23U, zeroPtCoeffU);
      xb_vecNx16 vecCoeff31 = IVP_SUBNX16U(vecCoeff31U, zeroPtCoeffU);
      xb_vecNx16 vecCoeff32 = IVP_SUBNX16U(vecCoeff32U, zeroPtCoeffU);
      xb_vecNx16 vecCoeff33 = IVP_SUBNX16U(vecCoeff33U, zeroPtCoeffU);

            for (y = 0; y < outH; y+=2)
      {                   /* walk down the rows */

              int32_t enable2Row    = XT_SALT(y, outH - 1);
        /* Input Data Pointers */
        int8_t *pData = pInData + ch + y * stride * inDataPitch2;

        pvecData1 = (xb_vecNx8U *) (pData + 0*inDataPitch2);
        pvecData2 = (xb_vecNx8U *) (pData + 1*inDataPitch2);
        pvecData3 = (xb_vecNx8U *) (pData + 2*inDataPitch2);
              pvecData4 = (xb_vecNx8U *) (pData + 3*inDataPitch2);

        /* Input loads*/
        /* ky = 0*/
        valign vaData1 = IVP_LANX8U_PP(pvecData1);
        xb_vecNx16U vecData11U; IVP_LANX8U_XP(vecData11U, vaData1, pvecData1, inDataPitch1);
        vaData1 = IVP_LANX8U_PP(pvecData1);
        xb_vecNx16U vecData12U; IVP_LANX8U_XP(vecData12U, vaData1, pvecData1, inDataPitch1);

        /* ky = 1*/
        vaData1 = IVP_LANX8U_PP(pvecData2);
        xb_vecNx16U vecData21U; IVP_LANX8U_XP(vecData21U, vaData1, pvecData2, inDataPitch1);
        vaData1 = IVP_LANX8U_PP(pvecData2);
        xb_vecNx16U vecData22U; IVP_LANX8U_XP(vecData22U, vaData1, pvecData2, inDataPitch1);

        /* ky = 2*/
        vaData1 = IVP_LANX8U_PP(pvecData3);
        xb_vecNx16U vecData31U; IVP_LANX8U_XP(vecData31U, vaData1, pvecData3, inDataPitch1);
        vaData1 = IVP_LANX8U_PP(pvecData3);
        xb_vecNx16U vecData32U; IVP_LANX8U_XP(vecData32U, vaData1, pvecData3, inDataPitch1);

              vaData1 = IVP_LANX8U_PP(pvecData4);
              xb_vecNx16U vecData41U; IVP_LANX8U_XP(vecData41U, vaData1, pvecData4, inDataPitch1);
              vaData1 = IVP_LANX8U_PP(pvecData4);
              xb_vecNx16U vecData42U; IVP_LANX8U_XP(vecData42U, vaData1, pvecData4, inDataPitch1);

        /* Subtract corresponding zeropoint from input vector*/
        xb_vecNx16 vecData11 = IVP_SUBNX16U(vecData11U, zeroPtInputU);
        xb_vecNx16 vecData12 = IVP_SUBNX16U(vecData12U, zeroPtInputU);

        xb_vecNx16 vecData21 = IVP_SUBNX16U(vecData21U, zeroPtInputU);
        xb_vecNx16 vecData22 = IVP_SUBNX16U(vecData22U, zeroPtInputU);

        xb_vecNx16 vecData31 = IVP_SUBNX16U(vecData31U, zeroPtInputU);
        xb_vecNx16 vecData32 = IVP_SUBNX16U(vecData32U, zeroPtInputU);

              xb_vecNx16 vecData41 = IVP_SUBNX16U(vecData41U, zeroPtInputU);
              xb_vecNx16 vecData42 = IVP_SUBNX16U(vecData42U, zeroPtInputU);

        int8_t *pOut = pOutData + y * outDataPitch2 + ch;
              int8_t *pOut1 = pOutData + (y+1) * outDataPitch2 + ch;

        for (x = 0; x < outW; x++)
        {                         /* walk across the columns */

          /* Initialize accumulator with bias values */
          accSum1 = accSum;
                accSum2 = accSum;

          /* ky = 0*/
          valign vaData2 = IVP_LANX8U_PP(pvecData1);
          xb_vecNx16U vecData13U; IVP_LANX8U_XP(vecData13U, vaData2, pvecData1, inDataPitch1);
          /* ky = 1*/
          valign vaData3 = IVP_LANX8U_PP(pvecData2);
          xb_vecNx16U vecData23U; IVP_LANX8U_XP(vecData23U, vaData3, pvecData2, inDataPitch1);
          /* ky = 2*/
          valign vaData4 = IVP_LANX8U_PP(pvecData3);
          xb_vecNx16U vecData33U; IVP_LANX8U_XP(vecData33U, vaData4, pvecData3, inDataPitch1);

                valign vaData5 = IVP_LANX8U_PP(pvecData4);
                xb_vecNx16U vecData43U; IVP_LANX8U_XP(vecData43U, vaData5, pvecData4, inDataPitch1);

          /* Subtract corresponding zeropoint from input vector*/
          xb_vecNx16 vecData13 = IVP_SUBNX16U(vecData13U, zeroPtInputU);
          xb_vecNx16 vecData23 = IVP_SUBNX16U(vecData23U, zeroPtInputU);
          xb_vecNx16 vecData33 = IVP_SUBNX16U(vecData33U, zeroPtInputU);
                xb_vecNx16 vecData43 = IVP_SUBNX16U(vecData43U, zeroPtInputU);

          /*Multiply and accumulate input data vector and coeff vector */
          IVP_MULPANX16(accSum1, vecCoeff11, vecData11, vecCoeff12, vecData12);
          IVP_MULPANX16(accSum1, vecCoeff13, vecData13, vecCoeff21, vecData21);
          IVP_MULPANX16(accSum1, vecCoeff22, vecData22, vecCoeff23, vecData23);
          IVP_MULPANX16(accSum1, vecCoeff31, vecData31, vecCoeff32, vecData32);
          IVP_MULANX16(accSum1, vecCoeff33, vecData33);

                IVP_MULPANX16(accSum2, vecCoeff11, vecData21, vecCoeff12, vecData22);
                IVP_MULPANX16(accSum2, vecCoeff13, vecData23, vecCoeff21, vecData31);
                IVP_MULPANX16(accSum2, vecCoeff22, vecData32, vecCoeff23, vecData33);
                IVP_MULPANX16(accSum2, vecCoeff31, vecData41, vecCoeff32, vecData42);
                IVP_MULANX16(accSum2, vecCoeff33,  vecData43);

          /* Pack, Output Scale, Output Shift and clamping */
          xb_vecNx16 vec16H1W1AB;
          ADD48_ZPT_PACK_SCALE_SHIFT_CLAMP_LIMITS(vec16H1W1AB, accSum1, \
                                                  zeroPtOutputU, leftShift,rightShift, quantScale, maxLimU, minLimU);

                xb_vecNx16 vec16H1W1AB1;
                            ADD48_ZPT_PACK_SCALE_SHIFT_CLAMP_LIMITS(vec16H1W1AB1, accSum2, \
                                                                    zeroPtOutputU, leftShift,rightShift, quantScale, maxLimU, minLimU);
                pdvecOut = (xb_vecNx8U *)(pOut);
                pdvecOut1 = (xb_vecNx8U *)(pOut1);

                /* Store the output vecOut1 along the output depth */
                IVP_SAVNX8U_XP(vec16H1W1AB, vaOutData, pdvecOut, remainingCh);
                IVP_SAPOSNX8U_FP(vaOutData, pdvecOut /*inout*/);
                pOut += outDataPitch1;

                IVP_SAVNX8U_XP(vec16H1W1AB1, vaOutData1, pdvecOut1, remainingCh*enable2Row);
                IVP_SAPOSNX8U_FP(vaOutData1, pdvecOut1 /*inout*/);
                pOut1 += outDataPitch1;

                vecData11 = vecData12;
                vecData12 = vecData13;

                vecData21 = vecData22;
                vecData22 = vecData23;

                vecData31 = vecData32;
                vecData32 = vecData33;

                vecData41 = vecData42;
                vecData42 = vecData43;
              }         /* End for (x = 0; x < outW; x ++) */
            }           /* End for (y = 0; y < outH; y ++) */
          }
     }
     else // special case handle numCh == 8
     {
          for (ch = 0; ch < numCh; ch += XCHAL_IVPN_SIMD_WIDTH)
          {             /* walk across the channels */
               /* To handle corner case when number of channels
                * is not a multiple of  2 * XCHAL_IVPN_SIMD_WIDTH*/
               int32_t remainingCh = XT_MIN((numCh - ch), XCHAL_IVPN_SIMD_WIDTH);

               /* Initialize accumulators with bias values */
               xb_vecNx48 accSum, accSum1,accSum2;
               phvecBias1 = (xb_vecN_2x32v *) (pBiasData + ch);
               valign vaBias;
               xb_vecN_2x32v hvecBias1, hvecBias2;
               vaBias = IVP_LAN_2X32_PP(phvecBias1);
               IVP_L2ANX32_IP(hvecBias2, hvecBias1, vaBias, phvecBias1);

               hvecBias1 = IVP_SHFLN_2X32(hvecBias1,*(xb_vecN_2x32v *) &selpat);
               hvecBias2 = hvecBias1; // copy 1 to 2)

               accSum = IVP_CVT48SNX32(hvecBias2, hvecBias1);
               /* Pointer for Coefficient Load */
               pvecCoeff = (xb_vecNx8U *) (pCoeffData + ch);

               /* 9 Coefficient Loads */
               xb_vecNx16U vecCoeff11U; IVP_LVNX8U_XP(vecCoeff11U, pvecCoeff, coeffPitch1);
               xb_vecNx16U vecCoeff12U; IVP_LVNX8U_XP(vecCoeff12U, pvecCoeff, coeffPitch1);
               xb_vecNx16U vecCoeff13U; IVP_LVNX8U_XP(vecCoeff13U, pvecCoeff, coeffPitch2 - 2 * coeffPitch1);
               xb_vecNx16U vecCoeff21U; IVP_LVNX8U_XP(vecCoeff21U, pvecCoeff, coeffPitch1);
               xb_vecNx16U vecCoeff22U; IVP_LVNX8U_XP(vecCoeff22U, pvecCoeff, coeffPitch1);
               xb_vecNx16U vecCoeff23U; IVP_LVNX8U_XP(vecCoeff23U, pvecCoeff, coeffPitch2 - 2 * coeffPitch1);
               xb_vecNx16U vecCoeff31U; IVP_LVNX8U_XP(vecCoeff31U, pvecCoeff, coeffPitch1);
               xb_vecNx16U vecCoeff32U; IVP_LVNX8U_XP(vecCoeff32U, pvecCoeff, coeffPitch1);
               xb_vecNx16U vecCoeff33U; IVP_LVNX8U_XP(vecCoeff33U, pvecCoeff, coeffPitch1);


               //rep lo to all 4 quarter lanes
               /* Subtract corresponding zeropoint from coeff vector */
               xb_vecNx16 vecCoeff11 = IVP_SUBNX16U(vecCoeff11U, zeroPtCoeffU);
               xb_vecNx16 vecCoeff12 = IVP_SUBNX16U(vecCoeff12U, zeroPtCoeffU);
               xb_vecNx16 vecCoeff13 = IVP_SUBNX16U(vecCoeff13U, zeroPtCoeffU);
               xb_vecNx16 vecCoeff21 = IVP_SUBNX16U(vecCoeff21U, zeroPtCoeffU);
               xb_vecNx16 vecCoeff22 = IVP_SUBNX16U(vecCoeff22U, zeroPtCoeffU);
               xb_vecNx16 vecCoeff23 = IVP_SUBNX16U(vecCoeff23U, zeroPtCoeffU);
               xb_vecNx16 vecCoeff31 = IVP_SUBNX16U(vecCoeff31U, zeroPtCoeffU);
               xb_vecNx16 vecCoeff32 = IVP_SUBNX16U(vecCoeff32U, zeroPtCoeffU);
               xb_vecNx16 vecCoeff33 = IVP_SUBNX16U(vecCoeff33U, zeroPtCoeffU);

               vecCoeff11 = IVP_SHFLNX16(vecCoeff11,*(xb_vecNx16 *)&selpat1);
               vecCoeff12 = IVP_SHFLNX16(vecCoeff12,*(xb_vecNx16 *)&selpat1);
               vecCoeff13 = IVP_SHFLNX16(vecCoeff13,*(xb_vecNx16 *)&selpat1);
               vecCoeff21 = IVP_SHFLNX16(vecCoeff21,*(xb_vecNx16 *)&selpat1);
               vecCoeff22 = IVP_SHFLNX16(vecCoeff22,*(xb_vecNx16 *)&selpat1);
               vecCoeff23 = IVP_SHFLNX16(vecCoeff23,*(xb_vecNx16 *)&selpat1);
               vecCoeff31 = IVP_SHFLNX16(vecCoeff31,*(xb_vecNx16 *)&selpat1);
               vecCoeff32 = IVP_SHFLNX16(vecCoeff32,*(xb_vecNx16 *)&selpat1);
               vecCoeff33 = IVP_SHFLNX16(vecCoeff33,*(xb_vecNx16 *)&selpat1);
               for (y = 0; y < outH; y+=2)
               {
                    /* Input Data Pointers */
                    int8_t *pData = pInData + ch + y * stride * inDataPitch2;

                    pvecData1 = (xb_vecNx8U *) (pData + 0*inDataPitch2);
                    pvecData2 = (xb_vecNx8U *) (pData + 1*inDataPitch2);
                    pvecData3 = (xb_vecNx8U *) (pData + 2*inDataPitch2);
                    pvecData4 = (xb_vecNx8U *) (pData + 3*inDataPitch2);
                   int8_t *pOut = pOutData + y * outDataPitch2 + ch;
                   int8_t *pOut1 = pOutData + (y+1) * outDataPitch2 + ch;
                 for (x = 0; x < outW; x+=4)
                 {
                    /* Initialize accumulator with bias values */
                    accSum1 = accSum;
                    accSum2 = accSum;

                    xb_vecNx16U vecData11U,vecData12U,vecData13U;
                    xb_vecNx16U vecData21U,vecData22U,vecData23U;
                    xb_vecNx16U vecData31U,vecData32U,vecData33U;
                    xb_vecNx16U vecData41U,vecData42U,vecData43U;
                    valign vaData1, vaData2, vaData3, vaData4;

                    vaData1 = IVP_LANX8U_PP(pvecData1);
                    IVP_LANX8U_XP(vecData11U, vaData1, pvecData1, inDataPitch1);//  y = 0, x=0,1,2,3
                    IVP_LANX8U_XP(vecData12U, vaData1, pvecData1, inDataPitch1);//  y = 0, x=1,2,3,4
                    IVP_LANX8U_XP(vecData13U, vaData1, pvecData1, 2*inDataPitch1);//y = 0, x=2,3,4,5

                    vaData2 = IVP_LANX8U_PP(pvecData2);
                    IVP_LANX8U_XP(vecData21U, vaData2, pvecData2, inDataPitch1);//  y = 1, x=0,1,2,3
                    IVP_LANX8U_XP(vecData22U, vaData2, pvecData2, inDataPitch1);//  y = 1, x=1,2,3,4
                    IVP_LANX8U_XP(vecData23U, vaData2, pvecData2, 2*inDataPitch1);//y = 1, x=2,3,4,5

                    vaData3 = IVP_LANX8U_PP(pvecData3);
                    IVP_LANX8U_XP(vecData31U, vaData3, pvecData3, inDataPitch1);//  y = 2, x=0,1,2,3
                    IVP_LANX8U_XP(vecData32U, vaData3, pvecData3, inDataPitch1);//  y = 2, x=1,2,3,4
                    IVP_LANX8U_XP(vecData33U, vaData3, pvecData3, 2*inDataPitch1);//y = 2, x=2,3,4,5

                    vaData4 = IVP_LANX8U_PP(pvecData4);
                    IVP_LANX8U_XP(vecData41U, vaData4, pvecData4, inDataPitch1);//  y = 2, x=0,1,2,3
                    IVP_LANX8U_XP(vecData42U, vaData4, pvecData4, inDataPitch1);//  y = 2, x=1,2,3,4
                    IVP_LANX8U_XP(vecData43U, vaData4, pvecData4, 2*inDataPitch1);//y = 2, x=2,3,4,5

                    xb_vecNx16 vecData11 = IVP_SUBNX16U(vecData11U, zeroPtInputU);
                    xb_vecNx16 vecData12 = IVP_SUBNX16U(vecData12U, zeroPtInputU);
                    xb_vecNx16 vecData13 = IVP_SUBNX16U(vecData13U, zeroPtInputU);

                    xb_vecNx16 vecData21 = IVP_SUBNX16U(vecData21U, zeroPtInputU);
                    xb_vecNx16 vecData22 = IVP_SUBNX16U(vecData22U, zeroPtInputU);
                    xb_vecNx16 vecData23 = IVP_SUBNX16U(vecData23U, zeroPtInputU);

                    xb_vecNx16 vecData31 = IVP_SUBNX16U(vecData31U, zeroPtInputU);
                    xb_vecNx16 vecData32 = IVP_SUBNX16U(vecData32U, zeroPtInputU);
                    xb_vecNx16 vecData33 = IVP_SUBNX16U(vecData33U, zeroPtInputU);

                    xb_vecNx16 vecData41 = IVP_SUBNX16U(vecData41U, zeroPtInputU);
                    xb_vecNx16 vecData42 = IVP_SUBNX16U(vecData42U, zeroPtInputU);
                    xb_vecNx16 vecData43 = IVP_SUBNX16U(vecData43U, zeroPtInputU);

                    /*Multiply and accumulate input data vector and coeff vector */
                   IVP_MULPANX16(accSum1, vecCoeff11, vecData11, vecCoeff12, vecData12);
                   IVP_MULPANX16(accSum1, vecCoeff13, vecData13, vecCoeff21, vecData21);
                   IVP_MULPANX16(accSum1, vecCoeff22, vecData22, vecCoeff23, vecData23);
                   IVP_MULPANX16(accSum1, vecCoeff31, vecData31, vecCoeff32, vecData32);
                   IVP_MULANX16(accSum1, vecCoeff33, vecData33);

                   /*Multiply and accumulate input data vector and coeff vector */
                   IVP_MULPANX16(accSum2, vecCoeff11, vecData21, vecCoeff12, vecData22);
                   IVP_MULPANX16(accSum2, vecCoeff13, vecData23, vecCoeff21, vecData31);
                   IVP_MULPANX16(accSum2, vecCoeff22, vecData32, vecCoeff23, vecData33);
                   IVP_MULPANX16(accSum2, vecCoeff31, vecData41, vecCoeff32, vecData42);
                   IVP_MULANX16(accSum2, vecCoeff33, vecData43);

                   xb_vecNx16 vec16H1W1AB;
                       ADD48_ZPT_PACK_SCALE_SHIFT_CLAMP_LIMITS(vec16H1W1AB, accSum1, \
                                                       zeroPtOutputU, leftShift,rightShift, quantScale, maxLimU, minLimU);
                       xb_vecNx16 vec16H1W1AB1;
                   ADD48_ZPT_PACK_SCALE_SHIFT_CLAMP_LIMITS(vec16H1W1AB1, accSum2, \
                                                                       zeroPtOutputU, leftShift,rightShift, quantScale, maxLimU, minLimU);
#ifndef IVP_SAPOSNX8U_FPXP
          pdvecOut = (xb_vecNx8U *)(pOut);
                    pdvecOut1 = (xb_vecNx8U *)(pOut1);
#endif
          /* Store the output vecOut1 along the output depth */
                    IVP_SAVNX8U_XP(vec16H1W1AB, vaOutData,  pdvecOut, 4*remainingCh);
                    IVP_SAVNX8U_XP(vec16H1W1AB1, vaOutData1, pdvecOut1, 4*remainingCh);
#ifdef IVP_SAPOSNX8U_FPXP
          IVP_SAPOSNX8U_FPXP(vaOutData, pdvecOut /*inout*/, outDataPitch1 - (remainingCh));
#else
          IVP_SAPOSNX8U_FP(vaOutData, pdvecOut /*inout*/);
          IVP_SAPOSNX8U_FP(vaOutData1, pdvecOut1 /*inout*/);
          pOut  += 4*outDataPitch1;pOut1 += 4*outDataPitch1;
      #endif
                 }
               }
          }
     }
  }
#endif
  return(XI_ERROR_STATUS());
}
