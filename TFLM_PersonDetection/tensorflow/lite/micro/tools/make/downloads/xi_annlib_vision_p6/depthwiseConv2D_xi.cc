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


#define ACC_INIT_BIAS_DM_2_TO_32(phvecBias1, phvecBias2, daccSum1, remCh1, remCh2)  { \
  valign vaBias;                                                                       \
  xb_vecN_2x32v hvecBias1 = 0, hvecBias2 = 0, hvecBias3 = 0, hvecBias4 = 0;            \
  vaBias = IVP_LAN_2X32_PP(phvecBias1);                                                \
  IVP_LAVN_2X32_XP(hvecBias1, vaBias, phvecBias1, remCh1);                             \
  IVP_LAVN_2X32_XP(hvecBias2, vaBias, phvecBias1, remCh2);                             \
  if (depthMultiplier == 32)                                                           \
  {                                                                                    \
    hvecBias3 = hvecBias1;                                                             \
    hvecBias4 = hvecBias2;                                                             \
    hvecScale3 = hvecScale1;                                                           \
    hvecScale4 = hvecScale2;                                                           \
    hvecLeftShift3 = hvecLeftShift1;                                                   \
    hvecLeftShift4 = hvecLeftShift2;                                                   \
  }                                                                                    \
  else if (depthMultiplier == 16)                                                      \
  {                                                                                    \
    hvecBias2 = hvecBias1;                                                             \
    hvecBias3 = hvecBias1;                                                             \
    hvecBias4 = hvecBias1;                                                             \
    hvecScale2 = hvecScale1;                                                           \
    hvecScale3 = hvecScale1;                                                           \
    hvecScale4 = hvecScale1;                                                           \
    hvecLeftShift2 = hvecLeftShift1;                                                   \
    hvecLeftShift3 = hvecLeftShift1;                                                   \
    hvecLeftShift4 = hvecLeftShift1;                                                   \
  }                                                                                    \
  else if (depthMultiplier <= 8)                                                       \
  {                                                                                    \
    hvecBias1 = IVP_SELN_2X32(hvecBias1, hvecBias1, hvecBiasSelLT16);                  \
    hvecScale1 = IVP_SELN_2X32(hvecScale1, hvecScale1, hvecBiasSelLT16);               \
    hvecLeftShift1 = IVP_SELN_2X32(hvecLeftShift1, hvecLeftShift1, hvecBiasSelLT16);   \
    hvecBias2 = hvecBias1;                                                             \
    hvecBias3 = hvecBias1;                                                             \
    hvecBias4 = hvecBias1;                                                             \
    hvecScale2 = hvecScale1;                                                           \
    hvecScale3 = hvecScale1;                                                           \
    hvecScale4 = hvecScale1;                                                           \
    hvecLeftShift2 = hvecLeftShift1;                                                   \
    hvecLeftShift3 = hvecLeftShift1;                                                   \
    hvecLeftShift4 = hvecLeftShift1;                                                   \
  }                                                                                    \
  daccSum1 = IVP_CVT24UNX32L(hvecBias2, hvecBias1);                                    \
  IVP_CVT24UNX32H(daccSum1, hvecBias4, hvecBias3);                                     \
}

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

int16_t selpat_lo_4[32] _XI_LOCAL_RAM0_ =
{ 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7 };



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
  uint8_t *__restrict pInData11;
  uint8_t *__restrict pInData12;
  uint8_t *__restrict pInData21;
  uint8_t *__restrict pInData22;
  xb_vecNx8U *__restrict pvecOutData;
  xb_vecNx8U *__restrict pvecFilter;
  xb_vec2Nx8U *__restrict pVecIn0;
  xb_vec2Nx8U *__restrict pVecIn1;
  xb_vecN_2x32v *__restrict phvecBias;
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

  xb_vecN_2x32v* __restrict phvecBias1;
  xb_vecNx8U* __restrict pvecCoeff;
  xb_vecNx8U* __restrict pvecData1;
  xb_vecNx8U* __restrict pvecData2;
  xb_vecNx8U* __restrict pvecData3;
  xb_vecNx8U* __restrict pvecData4;
  xb_vecNx8U* __restrict pvecData5;
  xb_vecNx8U* __restrict pvecData6;
  xb_vecNx8U* __restrict pdvecOut;
  xb_vecNx8U* __restrict pdvecOut1;

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

  xb_vecN_2x32v* __restrict phvecBias1;
  xb_vecNx8U* __restrict pvecCoeff;
  xb_vecNx8U* __restrict pvecData1;
  xb_vecNx8U* __restrict pvecData2;
  xb_vecNx8U* __restrict pvecData3;
  xb_vecNx8U* __restrict pvecData4;

  xb_vecNx8U* __restrict pdvecOut;
  xb_vecNx8U* __restrict pdvecOut1;

#ifdef MULP2NX8_FIXUP_OPT
  xb_vecN_2x32v* __restrict phvecBias2;
  xb_vec2Nx8U* __restrict pdvecCoeff;
  xb_vec2Nx8U* __restrict pdvecData1;
  xb_vec2Nx8U* __restrict pdvecData2;
  xb_vec2Nx8U* __restrict pdvecData3;
  xb_vec2Nx8U* __restrict pdvecData4;
  xb_vec2Nx8U* __restrict pdvecOut1;
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

#if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))

#ifdef IVP_PACKVNX48 /* Only available in Q7*/
#ifdef ENABLE_CONV_ASYMMETRIC_ROUNDING
#define CONV_PACK_AND_ROUNDING(hvecIn1, hvecIn2, vecRightShift, vecOut)      \
{                                                                            \
  /* Performs ASYMMETRIC ROUNDING Operation */                               \
  vecOut = IVP_PACKVNX48(IVP_CVT48SNX32(hvecIn2, hvecIn1), vecRightShift);   \
}
#else  /* #ifdef ENABLE_CONV_ASYMMETRIC_ROUNDING */
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

#define PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum, dvecOut)                                                              \
{                                                                                                              \
  xb_vecNx16 vecOutL, vecOutH;                                                                                 \
  /* Move the 24 bit accumulated sum to 32 bit vec registers. */                                               \
  xb_vecN_2x32v hvecIn1 = IVP_CVT32S2NX24LL(daccSum);                                                          \
  xb_vecN_2x32v hvecIn2 = IVP_CVT32S2NX24LH(daccSum);                                                          \
  xb_vecN_2x32v hvecIn3 = IVP_CVT32S2NX24HL(daccSum);                                                          \
  xb_vecN_2x32v hvecIn4 = IVP_CVT32S2NX24HH(daccSum);                                                          \
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
#define PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum, dvecOut)                                                              \
{                                                                                                              \
  xb_vecNx16 vecOutL, vecOutH;                                                                                 \
  /* Move the 24 bit accumulated sum to 32 bit vec registers. */                                               \
  xb_vecN_2x32v hvecIn1 = IVP_CVT32S2NX24LL(daccSum);                                                          \
  xb_vecN_2x32v hvecIn2 = IVP_CVT32S2NX24LH(daccSum);                                                          \
  xb_vecN_2x32v hvecIn3 = IVP_CVT32S2NX24HL(daccSum);                                                          \
  xb_vecN_2x32v hvecIn4 = IVP_CVT32S2NX24HH(daccSum);                                                          \
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

#define PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ_FOLD4(accSum, vecOutput)                              \
  {                                                                                                \
    /* Move the 24 bit accumulated sum to 32 bit vec registers. */                                 \
    xb_vecN_2x32v hvecIn1 = IVP_CVT32SNX48L(accSum);                                               \
    xb_vecN_2x32v hvecIn2 = IVP_CVT32SNX48H(accSum);                                               \
                                                                                                   \
    /* accumulated sum is multiplied with output scale value and Packed by 31 - Left shift */      \
    hvecIn1 = IVP_SLSN_2X32(hvecIn1, hvecLeftShift1);                                              \
    hvecIn2 = IVP_SLSN_2X32(hvecIn2, hvecLeftShift2);                                              \
    xb_vecNx16U vecIn1       = IVP_MOVNX16_FROMN_2X32(hvecIn1);                                    \
    xb_vecNx16U vecIn2       = IVP_MOVNX16_FROMN_2X32(hvecIn2);                                    \
    xb_vecN_2x64w haccScale1 = IVP_MULUSN_2X16X32_0(vecIn1, hvecScale1);                           \
    xb_vecN_2x64w haccScale2 = IVP_MULUSN_2X16X32_0(vecIn2, hvecScale2);                           \
    IVP_MULAHN_2X16X32_1(haccScale1, vecIn1, hvecScale1);                                          \
    IVP_MULAHN_2X16X32_1(haccScale2, vecIn2, hvecScale2);                                          \
    hvecIn1 = IVP_PACKVRN_2X64W(haccScale1, 31);                                                   \
    hvecIn2 = IVP_PACKVRN_2X64W(haccScale2, 31);                                                   \
    /* Perform Pack and Rounding */                                                                \
    CONV_PACK_AND_ROUNDING(hvecIn1, hvecIn2, vecRightShift, vecOutput);                            \
    /* Add zeroPtOut */                                                                            \
    xb_vecNx16 vecOutAddZP = IVP_ADDSNX16(vecOutput, (xb_vecNx16) zeroPtOut);                      \
    /* Result is saturate to minLim and maxLim */                                                  \
    vecOutput = (IVP_MAXNX16(IVP_MINNX16(vecOutAddZP, (xb_vecNx16) maxLim), (xb_vecNx16) minLim)); \
  }
#endif

#define BIAS_LOAD(phvecBias1, phvecBias2, daccSum, remCh1, remCh2, remCh3, remCh4)      \
{                                                                                       \
  valign vaBias;                                                                        \
  xb_vecN_2x32v hvecBias1, hvecBias2;                                                   \
  vaBias = IVP_LAN_2X32_PP(phvecBias1);                                                 \
  IVP_LAVN_2X32_XP(hvecBias1, vaBias, phvecBias1, remCh1);                              \
  IVP_LAVN_2X32_XP(hvecBias2, vaBias, phvecBias1, remCh2);                              \
  hvecBias1 = IVP_SUBN_2X32(hvecBias1, hvecFixLL);                                      \
  hvecBias2 = IVP_SUBN_2X32(hvecBias2, hvecFixLH);                                      \
  daccSum  = IVP_CVT24UNX32L(hvecBias2, hvecBias1);                                     \
  vaBias   = IVP_LAN_2X32_PP(phvecBias2);                                               \
  IVP_LAVN_2X32_XP(hvecBias1, vaBias, phvecBias2, remCh3);                              \
  IVP_LAVN_2X32_XP(hvecBias2, vaBias, phvecBias2, remCh4);                              \
  hvecBias1 = IVP_SUBN_2X32(hvecBias1, hvecFixHL);                                      \
  hvecBias2 = IVP_SUBN_2X32(hvecBias2, hvecFixHH);                                      \
  IVP_CVT24UNX32H(daccSum, hvecBias2, hvecBias1);                                       \
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
#endif
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
  xb_vecN_2x32v* __restrict phvecBias1;
  xb_vecN_2x32v* __restrict phvecBias2;
  xb_vecN_2x32v* __restrict phvecOutScale1;
  xb_vecN_2x32v* __restrict phvecOutScale2;
  xb_vec2Nx8* __restrict pdvecOutShift;
  xb_vec2Nx8* __restrict pdvecCoeff;
  xb_vec2Nx8* __restrict pdvecData1;
  xb_vec2Nx8* __restrict pdvecData2;
  xb_vec2Nx8* __restrict pdvecData3;
  xb_vec2Nx8* __restrict pdvecOut;

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

    int32_t remCh1 = XT_MIN(((numCh - ch) << 2), 64);
    int32_t remCh2 = XT_MIN((((numCh - ch) - 16) << 2), 64);
    int32_t remCh3 = XT_MIN((((numCh - ch) - 32) << 2), 64);
    int32_t remCh4 = XT_MIN((((numCh - ch) - 48) << 2), 64);

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
    BIAS_LOAD(phvecBias1, phvecBias2, daccSum, remCh1, remCh2, remCh3, remCh4);

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
        daccSum1 = daccSum;

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
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum1, dvecOut);
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

void depthwiseConvolveAVQ2D_S_3x3j2_S8Ca2_MOD_DWH_FOLD2(const xi_pTile3D inTile,
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
  valign vaOutData  = IVP_ZALIGN();
  valign vaOutData1 = IVP_ZALIGN();
  /* Input and Output data Pointers */
  xb_vecN_2x32v* __restrict phvecBias;
  xb_vecN_2x32v* __restrict phvecOutScale1;
  xb_vecN_2x32v* __restrict phvecOutScale2;
  xb_vecNx8* __restrict pvecOutShift;
  xb_vecNx8* __restrict pvecCoeff;
  xb_vecNx8* __restrict pvecData1;
  xb_vecNx8* __restrict pvecData2;
  xb_vecNx8* __restrict pvecData3;
  xb_vecNx8* __restrict pvecData4;
  xb_vecNx8* __restrict pvecData5;
  xb_vecNx8* __restrict pvecData6;
  xb_vecNx8* __restrict pvecOut;
  xb_vecNx8* __restrict pvecOut1;

  /* Input and Output data vectors */
  xb_vecNx48 accSum, accSum1, accSum2;
  xb_vecN_2x32v hvecScale1, hvecScale2, hvecScale3, hvecScale4;
  xb_vecNx16 vecCoeff11, vecCoeff12, vecCoeff13;
  xb_vecNx16 vecCoeff21, vecCoeff22, vecCoeff23;
  xb_vecNx16 vecCoeff31, vecCoeff32, vecCoeff33;
  xb_vecNx16 vecData11, vecData12, vecData13;
  xb_vecNx16 vecData21, vecData22, vecData23;
  xb_vecNx16 vecData31, vecData32, vecData33;
  xb_vecNx16 vecOut, vecOut1;

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

    int32_t remCh1 = XT_MIN(((numCh - ch) << 2), 64);
    int32_t remCh2 = XT_MIN((((numCh - ch) - 16) << 2), 64);
    int32_t remCh3 = XT_MIN((((numCh - ch) - 32) << 2), 64);
    int32_t remCh4 = XT_MIN((((numCh - ch) - 48) << 2), 64);

    phvecBias = (xb_vecN_2x32v *) (pBiasData);
    xb_vecN_2x32v hvecBias1, hvecBias2;
    valign vaBias = IVP_LAN_2X32_PP(phvecBias);
    IVP_LAVN_2X32_XP(hvecBias1, vaBias, phvecBias, numCh << 2);
    hvecBias2 = hvecBias1;


    /* Load OutScale values */
    phvecOutScale1 = (xb_vecN_2x32v *) (pOutScale + ch);
    phvecOutScale2 = (xb_vecN_2x32v *) (pOutScale + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
    valign vaOutScale = IVP_LAN_2X32_PP(phvecOutScale1);
    IVP_LAVN_2X32_XP(hvecScale1, vaOutScale, phvecOutScale1, remCh1);
    hvecScale2 = hvecScale1;

    /* Load OutShift values */
    xb_vecNx16 vecShift;
    pvecOutShift = (xb_vecNx8 *) (pOutShift + ch);
    valign vaOutShift = IVP_LANX8S_PP(pvecOutShift);
    IVP_LAVNX8S_XP(vecShift, vaOutShift, pvecOutShift, remainingCh);
    vecShift = IVP_SELNX16I(vecShift, vecShift, IVP_SELI_16B_EXTRACT_LO_HALVES);

    /* Calculate left shift and right shift values */
    vboolN vbN               = IVP_LTNX16(vecShift, 0);
    xb_vecNx16 vecRightShift = IVP_MOVNX16T(0, vecShift, vbN);
    xb_vecNx16 vecLeftShift  = 0;
    IVP_SUBNX16T(vecLeftShift, 0, vecShift, vbN);

    xb_vecNx16 vecLeftShiftL, vecLeftShiftH;
    xb_vecN_2x32v hvecLeftShift1 = IVP_UNPKSNX16_L(vecLeftShift);
    xb_vecN_2x32v hvecLeftShift2 = IVP_UNPKSNX16_H(vecLeftShift);


    /* Pointer for Coefficient Load */
    pvecCoeff = (xb_vecNx8 *) (pCoeffData);

    /* 9 Coefficient Loads */
    IVP_LVNX8S_XP(vecCoeff11, pvecCoeff, coeffPitch1);
    IVP_LVNX8S_XP(vecCoeff12, pvecCoeff, coeffPitch1);
    IVP_LVNX8S_XP(vecCoeff13, pvecCoeff, coeffPitch2 - 2 * coeffPitch1);
    IVP_LVNX8S_XP(vecCoeff21, pvecCoeff, coeffPitch1);
    IVP_LVNX8S_XP(vecCoeff22, pvecCoeff, coeffPitch1);
    IVP_LVNX8S_XP(vecCoeff23, pvecCoeff, coeffPitch2 - 2 * coeffPitch1);
    IVP_LVNX8S_XP(vecCoeff31, pvecCoeff, coeffPitch1);
    IVP_LVNX8S_XP(vecCoeff32, pvecCoeff, coeffPitch1);
    IVP_LVNX8S_XP(vecCoeff33, pvecCoeff, coeffPitch1);

    /* shuffle coefficient data */
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
    /* Calculate Fixup terms*/
    xb_vecNx16 vecSum;
    vecSum  = IVP_ADDNX16(vecCoeff11, vecCoeff12);
    vecSum += IVP_ADDNX16(vecCoeff13, vecCoeff21);
    vecSum += IVP_ADDNX16(vecCoeff22, vecCoeff23);
    vecSum += IVP_ADDNX16(vecCoeff31, vecCoeff32);
    vecSum += IVP_ADDNX16(vecCoeff33, 0);

    xb_vecNx48 accFixup    = IVP_MULNX16(vecSum, (xb_vecNx16) zeroPtIn);
    xb_vecN_2x32v hvecFixL = IVP_CVT32SNX48L(accFixup);
    xb_vecN_2x32v hvecFixH = IVP_CVT32SNX48H(accFixup);

    hvecBias1 = IVP_SUBN_2X32(hvecBias1, hvecFixL);
    hvecBias2 = IVP_SUBN_2X32(hvecBias2, hvecFixH);
    accSum    = IVP_CVT48SNX32(hvecBias2, hvecBias1);

    for (y = 0; y < outH; y += 2)  /* along output Height */
    {
      /* Input Data Pointers */
      int8_t *pData  = pInData + ch + y * stride * inDataPitch2;
      int8_t *pData1 = pInData + ch + (y + 1) * stride * inDataPitch2;


      pvecData1 = (xb_vecNx8 *)  pData;
      pvecData2 = (xb_vecNx8 *) (pData + inDataPitch2);
      pvecData3 = (xb_vecNx8 *) (pData + 2 * inDataPitch2);
      pvecData4 = (xb_vecNx8 *)  pData1;
      pvecData5 = (xb_vecNx8 *) (pData1 + inDataPitch2);
      pvecData6 = (xb_vecNx8 *) (pData1 + 2 * inDataPitch2);


      /* Input loads*/
      /* ky = 0*/
      valign vaData1 = IVP_LANX8S_PP(pvecData1);
      xb_vecNx16 vecData11; IVP_LANX8S_XP(vecData11, vaData1, pvecData1, 2 * inDataPitch1);
      /* ky = 1*/
      valign vaData2 = IVP_LANX8S_PP(pvecData2);
      xb_vecNx16 vecData21; IVP_LANX8S_XP(vecData21, vaData2, pvecData2, 2 * inDataPitch1);
      /* ky = 2*/
      valign vaData3 = IVP_LANX8S_PP(pvecData3);
      xb_vecNx16 vecData31; IVP_LANX8S_XP(vecData31, vaData3, pvecData3, 2 * inDataPitch1);

      vaData1 = IVP_LANX8S_PP(pvecData4);
      xb_vecNx16 vecData11_2; IVP_LANX8S_XP(vecData11_2, vaData1, pvecData4, 2 * inDataPitch1);
      vaData2 = IVP_LANX8S_PP(pvecData5);
      xb_vecNx16 vecData21_2; IVP_LANX8S_XP(vecData21_2, vaData2, pvecData5, 2 * inDataPitch1);
      vaData3 = IVP_LANX8S_PP(pvecData6);
      xb_vecNx16 vecData31_2; IVP_LANX8S_XP(vecData31_2, vaData3, pvecData6, 2 * inDataPitch1);

      for (x = 0; x < outW; x += 2) /* along output width */
      {
        int8_t *pOut  = pOutData + (x * outDataPitch1 + y * outDataPitch2);
        int8_t *pOut1 = pOutData + (x * outDataPitch1 + (y + 1) * outDataPitch2);

        /* Initialize accumulator with bias values */
        accSum1 = accSum;
        accSum2 = accSum;

        /* ky = 0*/
        vaData1 = IVP_LANX8S_PP(pvecData1);
        xb_vecNx16 vecData12; IVP_LANX8S_XP(vecData12, vaData1, pvecData1, 2 * inDataPitch1);
        xb_vecNx16 vecData13; IVP_LANX8S_XP(vecData13, vaData1, pvecData1, 2 * inDataPitch1);
        /* ky = 1*/
        vaData2 = IVP_LANX8S_PP(pvecData2);
        xb_vecNx16 vecData22; IVP_LANX8S_XP(vecData22, vaData2, pvecData2, 2 * inDataPitch1);
        xb_vecNx16 vecData23; IVP_LANX8S_XP(vecData23, vaData2, pvecData2, 2 * inDataPitch1);
        /* ky = 2*/
        vaData3 = IVP_LANX8S_PP(pvecData3);
        xb_vecNx16 vecData32; IVP_LANX8S_XP(vecData32, vaData3, pvecData3, 2 * inDataPitch1);
        xb_vecNx16 vecData33; IVP_LANX8S_XP(vecData33, vaData3, pvecData3, 2 * inDataPitch1);
        vaData1 = IVP_LANX8S_PP(pvecData4);
        xb_vecNx16 vecData12_2; IVP_LANX8S_XP(vecData12_2, vaData1, pvecData4, 2 * inDataPitch1);
        xb_vecNx16 vecData13_2; IVP_LANX8S_XP(vecData13_2, vaData1, pvecData4, 2 * inDataPitch1);
        vaData2 = IVP_LANX8S_PP(pvecData5);
        xb_vecNx16 vecData22_2; IVP_LANX8S_XP(vecData22_2, vaData2, pvecData5, 2 * inDataPitch1);
        xb_vecNx16 vecData23_2; IVP_LANX8S_XP(vecData23_2, vaData2, pvecData5, 2 * inDataPitch1);
        vaData3 = IVP_LANX8S_PP(pvecData6);
        xb_vecNx16 vecData32_2; IVP_LANX8S_XP(vecData32_2, vaData3, pvecData6, 2 * inDataPitch1);
        xb_vecNx16 vecData33_2; IVP_LANX8S_XP(vecData33_2, vaData3, pvecData6, 2 * inDataPitch1);
        /* form new */                 // 0,1 , 2,3, 4,5 <==> 0,2,1,3,2,4
        xb_vecNx16 t11, t12;
        IVP_DSELNX16(t12, t11, vecData12, vecData11, *(xb_vec2Nx8 *) &dselpat_lo_hi);
        xb_vecNx16 t13 = IVP_SELNX16I(vecData13, vecData12, IVP_SELI_16B_EXTRACT_LO_HALVES);

        xb_vecNx16 t21, t22;
        IVP_DSELNX16(t22, t21, vecData22, vecData21, *(xb_vec2Nx8 *) &dselpat_lo_hi);
        xb_vecNx16 t23 = IVP_SELNX16I(vecData23, vecData22, IVP_SELI_16B_EXTRACT_LO_HALVES);

        xb_vecNx16 t31, t32;
        IVP_DSELNX16(t32, t31, vecData32, vecData31, *(xb_vec2Nx8 *) &dselpat_lo_hi);
        xb_vecNx16 t33 = IVP_SELNX16I(vecData33, vecData32, IVP_SELI_16B_EXTRACT_LO_HALVES);

        IVP_MULPANX16(accSum1, vecCoeff11, t11, vecCoeff12, t12);
        IVP_MULPANX16(accSum1, vecCoeff13, t13, vecCoeff21, t21);
        IVP_MULPANX16(accSum1, vecCoeff22, t22, vecCoeff23, t23);
        IVP_MULPANX16(accSum1, vecCoeff31, t31, vecCoeff32, t32);
        IVP_MULANX16(accSum1, vecCoeff33, t33);

        IVP_DSELNX16(t12, t11, vecData12_2, vecData11_2, *(xb_vec2Nx8 *) &dselpat_lo_hi);
        t13 = IVP_SELNX16I(vecData13_2, vecData12_2, IVP_SELI_16B_EXTRACT_LO_HALVES);
        IVP_DSELNX16(t22, t21, vecData22_2, vecData21_2, *(xb_vec2Nx8 *) &dselpat_lo_hi);
        t23 = IVP_SELNX16I(vecData23_2, vecData22_2, IVP_SELI_16B_EXTRACT_LO_HALVES);
        IVP_DSELNX16(t32, t31, vecData32_2, vecData31_2, *(xb_vec2Nx8 *) &dselpat_lo_hi);
        t33 = IVP_SELNX16I(vecData33_2, vecData32_2, IVP_SELI_16B_EXTRACT_LO_HALVES);
        IVP_MULPANX16(accSum2, vecCoeff11, t11, vecCoeff12, t12);
        IVP_MULPANX16(accSum2, vecCoeff13, t13, vecCoeff21, t21);
        IVP_MULPANX16(accSum2, vecCoeff22, t22, vecCoeff23, t23);
        IVP_MULPANX16(accSum2, vecCoeff31, t31, vecCoeff32, t32);
        IVP_MULANX16(accSum2, vecCoeff33, t33);

        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ_FOLD4(accSum1, vecOut);
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ_FOLD4(accSum2, vecOut1);
        /* Store the output vecOut1 along the output depth */
        pvecOut = (xb_vecNx8 *) (pOut);
        IVP_SAVNX8S_XP(vecOut, vaOutData, pvecOut, 2 * remainingCh);
        IVP_SAPOSNX8S_FP(vaOutData, pvecOut);
        pvecOut1 = (xb_vecNx8 *) (pOut1);
        IVP_SAVNX8S_XP(vecOut1, vaOutData1, pvecOut1, 2 * remainingCh);
        IVP_SAPOSNX8S_FP(vaOutData1, pvecOut1);
        vecData11   = vecData13;
        vecData21   = vecData23;
        vecData31   = vecData33;
        vecData11_2 = vecData13_2;
        vecData21_2 = vecData23_2;
        vecData31_2 = vecData33_2;
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
  xb_vecN_2x32v* __restrict phvecBias1;
  xb_vecN_2x32v* __restrict phvecBias2;
  xb_vecN_2x32v* __restrict phvecOutScale1;
  xb_vecN_2x32v* __restrict phvecOutScale2;
  xb_vec2Nx8* __restrict pdvecOutShift;
  xb_vec2Nx8* __restrict pdvecCoeff;
  xb_vec2Nx8* __restrict pdvecData1;
  xb_vec2Nx8* __restrict pdvecData2;
  xb_vec2Nx8* __restrict pdvecData3;
  xb_vec2Nx8* __restrict pdvecOut;

  /* Input and Output data Vectors */
  xb_vec2Nx24 daccSum, daccSum1;
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

    int32_t remCh1 = XT_MIN(((numCh - ch) << 2), 64);
    int32_t remCh2 = XT_MIN((((numCh - ch) - 16) << 2), 64);
    int32_t remCh3 = XT_MIN((((numCh - ch) - 32) << 2), 64);
    int32_t remCh4 = XT_MIN((((numCh - ch) - 48) << 2), 64);

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
    BIAS_LOAD(phvecBias1, phvecBias2, daccSum, remCh1, remCh2, remCh3, remCh4);
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
        daccSum1 = daccSum;
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
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum1, dvecOut);
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
  xb_vecN_2x32v* __restrict phvecBias;
  xb_vecN_2x32v* __restrict phvecOutScale;
  xb_vec2Nx8* __restrict pdvecOutShift;
  xb_vec2Nx8* __restrict pdvecCoeff;
  xb_vec2Nx8* __restrict pdvecData1;
  xb_vec2Nx8* __restrict pdvecData2;
  xb_vec2Nx8* __restrict pdvecData3;
  xb_vec2Nx8* __restrict pdvecOut;
  /* Input and Output data Pointers */
  xb_vec2Nx8 dvecData11, dvecData12, dvecData13;
  xb_vec2Nx8 dvecData21, dvecData22, dvecData23;
  xb_vec2Nx8 dvecData31, dvecData32, dvecData33;
  xb_vec2Nx8 dvecOut;
  xb_vec2Nx8 dvecCoeff11, dvecCoeff12, dvecCoeff13;
  xb_vec2Nx8 dvecCoeff21, dvecCoeff22, dvecCoeff23;
  xb_vec2Nx8 dvecCoeff31, dvecCoeff32, dvecCoeff33;
  xb_vec2Nx24 daccSum, daccSum1;

  /* Variable Declarations */
  int32_t x, y;
  int32_t vectorizationWidth = 2 * XCHAL_IVPN_SIMD_WIDTH;
  /* Sequence for select Pattern */
  xb_vec2Nx8 dvecSeq1 = IVP_ADD2NX8(IVP_SEQ2NX8(), numCh);
  vbool2N vbl1 = IVP_GE2NX8(dvecSeq1, numCh * 2);
  IVP_SUB2NX8T(dvecSeq1, dvecSeq1, numCh * 2, vbl1);
  IVP_ADD2NX8T(dvecSeq1, dvecSeq1, 64, vbl1);

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
    /* Initialize accumulator with bias values */
    daccSum  = IVP_CVT24UNX32L(hvecBias, hvecBias);
    IVP_CVT24UNX32H(daccSum, hvecBias, hvecBias);
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

        /* Initialize accumulator with bias values */
        daccSum1 = daccSum;
        /* Multiply Input with Coefficient Value */
        IVP_MULPA2NX8(daccSum1, dvecCoeff11, dvecData11, dvecCoeff12, dvecData12);
        IVP_MULPA2NX8(daccSum1, dvecCoeff13, dvecData13, dvecCoeff21, dvecData21);
        IVP_MULPA2NX8(daccSum1, dvecCoeff22, dvecData22, dvecCoeff23, dvecData23);
        IVP_MULPA2NX8(daccSum1, dvecCoeff31, dvecData31, dvecCoeff32, dvecData32);
        IVP_MULA2NX8(daccSum1,  dvecCoeff33, dvecData33);

        /* Pack, Output Scale, Output Shift and clamping */
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum1, dvecOut);
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
    xb_vecN_2x32v hvecBias1, hvecBias2;
    valign vaBias = IVP_LAN_2X32_PP(phvecBias);
    IVP_LAVN_2X32_XP(hvecBias1, vaBias, phvecBias, numCh << 2);
    IVP_LAVN_2X32_XP(hvecBias2, vaBias, phvecBias, (numCh - 16) << 2);
    /* Load Scale values */
    phvecOutScale = (xb_vecN_2x32v *)(pOutScale);
    xb_vecN_2x32v hvecScale1, hvecScale2, hvecScale3, hvecScale4;
    valign vaOutScale = IVP_LAN_2X32_PP(phvecOutScale);
    IVP_LAVN_2X32_XP(hvecScale1, vaOutScale, phvecOutScale,numCh<<2 );
    IVP_LAVN_2X32_XP(hvecScale2, vaOutScale, phvecOutScale, (numCh - 16) << 2);
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
    if (numCh <= 16)
    {
      /*For eg; if d == 15, the sequence in accumulator is
      0 1 2 ....13 14 0 1 2 ....13 14*/
      xb_vecN_2x32v hvecSeq1 = IVP_SEQN_2X32();
      vboolN_2 vbl1 = IVP_GEN_2X32(hvecSeq1, numCh);
      IVP_SUBN_2X32T(hvecSeq1, hvecSeq1, numCh, vbl1);
      xb_vecN_2x32v hvecSeq2 = IVP_ADDN_2X32(IVP_SEQN_2X32(), 16 - numCh);
      hvecBias1 = IVP_SELN_2X32(hvecBias1, hvecBias1, hvecSeq1);
      hvecBias2 = IVP_SELN_2X32(hvecBias2, hvecBias1, hvecSeq2);
      hvecBias1 = IVP_SUBN_2X32(hvecBias1, hvecFixLL);
      hvecBias2 = IVP_SUBN_2X32(hvecBias2, hvecFixLH);
      daccSum = IVP_CVT24UNX32L(hvecBias2, hvecBias1);
      IVP_CVT24UNX32H(daccSum, hvecBias2, hvecBias1);
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
      xb_vecN_2x32v hvecSeq1 = IVP_ADDN_2X32(IVP_SEQN_2X32(), 16);
      vboolN_2 vbl1 = IVP_GEN_2X32(hvecSeq1, numCh);
      IVP_SUBN_2X32T(hvecSeq1, hvecSeq1, numCh, vbl1);
      xb_vecN_2x32v hvecSeq2 = IVP_ADDN_2X32(IVP_SEQN_2X32(), 32 - numCh);
      xb_vecN_2x32v hvecBias2A = IVP_SELN_2X32(hvecBias2, hvecBias1, hvecSeq1);
      xb_vecN_2x32v hvecBias3A = IVP_SELN_2X32(hvecBias2, hvecBias1, hvecSeq2);
      xb_vecN_2x32v hvecBias4A = IVP_SELN_2X32(hvecBias2, hvecBias2, hvecSeq2);
      hvecBias1  = IVP_SUBN_2X32(hvecBias1, hvecFixLL);
      hvecBias2A = IVP_SUBN_2X32(hvecBias2A, hvecFixLH);
      hvecBias3A = IVP_SUBN_2X32(hvecBias3A, hvecFixHL);
      hvecBias4A = IVP_SUBN_2X32(hvecBias4A, hvecFixHH);
      daccSum = IVP_CVT24UNX32L(hvecBias2A, hvecBias1);
      IVP_CVT24UNX32H(daccSum, hvecBias4A, hvecBias3A);
      /* OutScale Values */
      xb_vecN_2x32v hvecScale2A = IVP_SELN_2X32(hvecScale2, hvecScale1, hvecSeq1);
      hvecScale3 = IVP_SELN_2X32(hvecScale2, hvecScale1, hvecSeq2);
      hvecScale4 = IVP_SELN_2X32(hvecScale2, hvecScale2, hvecSeq2);
      hvecScale2 = hvecScale2A;
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
        /* Initialize accumulator with bias values */
        daccSum1 = daccSum;
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
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum1, dvecOut);
        /* Storing the first and second row */
        IVP_SAV2NX8_XP(dvecOut, vaOutData, pdvecOut, numCh *2);
        FLUSH2NX8(vaOutData, pdvecOut, outDataPitch1 * 2, numCh * 2, pOut);
        dvecData11 = dvecData13;
        dvecData21 = dvecData23;
        dvecData31 = dvecData33;
      }/* End for (x = 0; x < outW-1; x +=2) */
      if(x < outW)
      {
        /* Initialize accumulator with bias values */
        daccSum1 = daccSum;
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
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum1, dvecOut);
        /* Storing the first row */
        IVP_SAV2NX8_XP(dvecOut, vaOutData, pdvecOut, numCh);
        FLUSH2NX8(vaOutData, pdvecOut, outDataPitch1, numCh, pOut);
      } // if(outW < x)
    } /* End for (y = 0; y < outH; y ++) */
  } //else
}

void depthwiseConvolveAVQ2D_S_3x3_S8Ca2_MOD_DWH_FOLD4(const xi_pTile3D inTile,
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
  int8_t *pInData    = (int8_t *) XI_TILE3D_GET_DATA_PTR(inTile);
  int8_t *pOutData   = (int8_t *) XI_TILE3D_GET_DATA_PTR(outTile);
  int8_t *pCoeffData = (int8_t *) XI_TILE3D_GET_DATA_PTR(coeffTile);
  int32_t *pBiasData = (int32_t *) XI_ARRAY_GET_DATA_PTR(biasArray);
  int32_t *pOutScale = (int32_t *) XI_ARRAY_GET_DATA_PTR(outScaleArray);
  int8_t *pOutShift  = (int8_t *) XI_ARRAY_GET_DATA_PTR(outShiftArray);

  /* Move pointer to the start of the data (including edge) */
  pInData = &pInData[-((kSizeU / 2) * inDataPitch1 + (kSizeU / 2) * inDataPitch2)];

  /* Input and Output data Pointers */
  xb_vecN_2x32v* __restrict phvecBias;
  xb_vecN_2x32v* __restrict phvecOutScale;
  xb_vecNx8* __restrict pvecOutShift;
  xb_vecNx8* __restrict pvecCoeff;
  xb_vecNx8* __restrict pvecData1;
  xb_vecNx8* __restrict pvecData2;
  xb_vecNx8* __restrict pvecData3;
  xb_vecNx8* __restrict pvecData4;
  xb_vecNx8* __restrict pvecOut;
  xb_vecNx8* __restrict pvecOut1;
  /* Input and Output data Pointers */
  xb_vecNx16 vecData11, vecData12, vecData13;
  xb_vecNx16 vecData21, vecData22, vecData23;
  xb_vecNx16 vecData31, vecData32, vecData33;
  xb_vecNx16 vecOut, vecOut1;
  xb_vecNx16 vecCoeff11, vecCoeff12, vecCoeff13;
  xb_vecNx16 vecCoeff21, vecCoeff22, vecCoeff23;
  xb_vecNx16 vecCoeff31, vecCoeff32, vecCoeff33;
  xb_vecNx48 accSum, accSum1, accSum2;

  /* Variable Declarations */
  int32_t x, y;
  int32_t vectorizationWidth = 2 * XCHAL_IVPN_SIMD_WIDTH;
  /* Sequence for select Pattern */
  xb_vec2Nx8 dvecSeq1 = IVP_ADD2NX8(IVP_SEQ2NX8(), numCh);
  vbool2N vbl1        = IVP_GE2NX8(dvecSeq1, numCh * 2);
  IVP_SUB2NX8T(dvecSeq1, dvecSeq1, numCh * 2, vbl1);
  IVP_ADD2NX8T(dvecSeq1, dvecSeq1, 64, vbl1);

  xb_vec2Nx8 dvecSeq2 = IVP_SEQ2NX8();
  vbl1 = IVP_GE2NX8(dvecSeq2, numCh);
  IVP_SUB2NX8T(dvecSeq2, dvecSeq2, numCh, vbl1);

  valign vaOutData  = IVP_ZALIGN();
  valign vaOutData1 = IVP_ZALIGN();

  /* Initialize accumulators with bias values
   * copy lower 32 elements values in higher 32 bytes
   */
  phvecBias = (xb_vecN_2x32v *) (pBiasData);
  xb_vecN_2x32v hvecBias1, hvecBias2;
  valign vaBias = IVP_LAN_2X32_PP(phvecBias);
  IVP_LAVN_2X32_XP(hvecBias1, vaBias, phvecBias, numCh << 2);
  hvecBias1 = IVP_SELN_2X32I(hvecBias1, hvecBias1, IVP_SELI_32B_EXTRACT_LO_HALVES);
  hvecBias2 = hvecBias1;

  /* Load Scale values */
  phvecOutScale = (xb_vecN_2x32v *) (pOutScale);
  xb_vecN_2x32v hvecScale1, hvecScale2, hvecScale3, hvecScale4;
  valign vaOutScale = IVP_LAN_2X32_PP(phvecOutScale);
  IVP_LAVN_2X32_XP(hvecScale1, vaOutScale, phvecOutScale, numCh << 2);
  hvecScale1 = IVP_SELN_2X32I(hvecScale1, hvecScale1, IVP_SELI_32B_EXTRACT_LO_HALVES);
  hvecScale2 = hvecScale1;
  /* Load Shift values */
  pvecOutShift = (xb_vecNx8 *) pOutShift;
  xb_vecNx16 vecShift;
  valign vaShift = IVP_LANX8S_PP(pvecOutShift);
  IVP_LAVNX8S_XP(vecShift, vaShift, pvecOutShift, numCh);
  vecShift = IVP_SHFLNX16(vecShift, *(xb_vecNx16 *) &selpat_lo_4);

  vboolN vbN               = IVP_LTNX16(vecShift, 0);
  xb_vecNx16 vecRightShift = IVP_MOVNX16T(0, vecShift, vbN);
  xb_vecNx16 vecLeftShift  = 0;
  IVP_SUBNX16T(vecLeftShift, 0, vecShift, vbN);

  /* Calculate left shift values */
  xb_vecNx16 vecLeftShiftL, vecLeftShiftH;
  xb_vecN_2x32v hvecLeftShift1 = IVP_UNPKSNX16_L(vecLeftShift);
  xb_vecN_2x32v hvecLeftShift2 = IVP_UNPKSNX16_H(vecLeftShift);


  /* Pointer for Coefficient Load */
  pvecCoeff = (xb_vecNx8 *) (pCoeffData);

  /* 9 Coefficient Loads */
  IVP_LVNX8S_XP(vecCoeff11, pvecCoeff, coeffPitch1);
  IVP_LVNX8S_XP(vecCoeff12, pvecCoeff, coeffPitch1);
  IVP_LVNX8S_XP(vecCoeff13, pvecCoeff, coeffPitch2 - 2 * coeffPitch1);
  IVP_LVNX8S_XP(vecCoeff21, pvecCoeff, coeffPitch1);
  IVP_LVNX8S_XP(vecCoeff22, pvecCoeff, coeffPitch1);
  IVP_LVNX8S_XP(vecCoeff23, pvecCoeff, coeffPitch2 - 2 * coeffPitch1);
  IVP_LVNX8S_XP(vecCoeff31, pvecCoeff, coeffPitch1);
  IVP_LVNX8S_XP(vecCoeff32, pvecCoeff, coeffPitch1);
  IVP_LVNX8S_XP(vecCoeff33, pvecCoeff, coeffPitch1);

  /* shuffle coefficient data */
  vecCoeff11 = IVP_SHFLNX16(vecCoeff11, *(xb_vecNx16 *) &selpat1);
  vecCoeff12 = IVP_SHFLNX16(vecCoeff12, *(xb_vecNx16 *) &selpat1);
  vecCoeff13 = IVP_SHFLNX16(vecCoeff13, *(xb_vecNx16 *) &selpat1);
  vecCoeff21 = IVP_SHFLNX16(vecCoeff21, *(xb_vecNx16 *) &selpat1);
  vecCoeff22 = IVP_SHFLNX16(vecCoeff22, *(xb_vecNx16 *) &selpat1);
  vecCoeff23 = IVP_SHFLNX16(vecCoeff23, *(xb_vecNx16 *) &selpat1);
  vecCoeff31 = IVP_SHFLNX16(vecCoeff31, *(xb_vecNx16 *) &selpat1);
  vecCoeff32 = IVP_SHFLNX16(vecCoeff32, *(xb_vecNx16 *) &selpat1);
  vecCoeff33 = IVP_SHFLNX16(vecCoeff33, *(xb_vecNx16 *) &selpat1);
  /* Calculate Fixup terms*/
  xb_vecNx16 vecSum;
  vecSum  = IVP_ADDNX16(vecCoeff11, vecCoeff12);
  vecSum += IVP_ADDNX16(vecCoeff13, vecCoeff21);
  vecSum += IVP_ADDNX16(vecCoeff22, vecCoeff23);
  vecSum += IVP_ADDNX16(vecCoeff31, vecCoeff32);
  vecSum += IVP_ADDNX16(vecCoeff33, 0);

  xb_vecNx48 accFixup    = IVP_MULNX16(vecSum, (xb_vecNx16) zeroPtIn);
  xb_vecN_2x32v hvecFixL = IVP_CVT32SNX48L(accFixup);
  xb_vecN_2x32v hvecFixH = IVP_CVT32SNX48H(accFixup);

  hvecBias1 = IVP_SUBN_2X32(hvecBias1, hvecFixL);
  hvecBias2 = IVP_SUBN_2X32(hvecBias2, hvecFixH);
  accSum    = IVP_CVT48SNX32(hvecBias2, hvecBias1);

  for (y = 0; y < outH; y += 2)  /* Along Output Height */
  {
    /* Input Data Pointers */
    int8_t *pData = pInData + y * inDataPitch2;
    /* Output Data Pointers */
    int8_t *pOut  = pOutData + y * outDataPitch2;
    int8_t *pOut1 = pOutData + (y + 1) * outDataPitch2;

    /* Input Data Pointers */
    pvecData1 = (xb_vecNx8 *) (pData);
    pvecData2 = (xb_vecNx8 *) (pData + 1 * inDataPitch2);
    pvecData3 = (xb_vecNx8 *) (pData + 2 * inDataPitch2);
    pvecData4 = (xb_vecNx8 *) (pData + 3 * inDataPitch2);
    /* Input loads*/
    for (x = 0; x < (outW - 1); x += 4)        /* Along Output Width */
    {
      /* Initialize accumulator with bias values */
      accSum1 = accSum;
      accSum2 = accSum;
      xb_vecNx16 vecData11, vecData12, vecData13;
      xb_vecNx16 vecData21, vecData22, vecData23;
      xb_vecNx16 vecData31, vecData32, vecData33;
      xb_vecNx16 vecData41, vecData42, vecData43;
      valign vaData1, vaData2, vaData3, vaData4;
      /* ky = 0*/
      /* load data from second 2 input rows */

      vaData1 = IVP_LANX8S_PP(pvecData1);
      IVP_LANX8S_XP(vecData11, vaData1, pvecData1, inDataPitch1);                  //  y = 0, x=0,1,2,3
      IVP_LANX8S_XP(vecData12, vaData1, pvecData1, inDataPitch1);                  //  y = 0, x=1,2,3,4
      IVP_LANX8S_XP(vecData13, vaData1, pvecData1, 2 * inDataPitch1);              //y = 0, x=2,3,4,5

      vaData2 = IVP_LANX8S_PP(pvecData2);
      IVP_LANX8S_XP(vecData21, vaData2, pvecData2, inDataPitch1);                  //  y = 1, x=0,1,2,3
      IVP_LANX8S_XP(vecData22, vaData2, pvecData2, inDataPitch1);                  //  y = 1, x=1,2,3,4
      IVP_LANX8S_XP(vecData23, vaData2, pvecData2, 2 * inDataPitch1);              //y = 1, x=2,3,4,5

      vaData3 = IVP_LANX8S_PP(pvecData3);
      IVP_LANX8S_XP(vecData31, vaData3, pvecData3, inDataPitch1);                  //  y = 2, x=0,1,2,3
      IVP_LANX8S_XP(vecData32, vaData3, pvecData3, inDataPitch1);                  //  y = 2, x=1,2,3,4
      IVP_LANX8S_XP(vecData33, vaData3, pvecData3, 2 * inDataPitch1);              //y = 2, x=2,3,4,5

      vaData4 = IVP_LANX8S_PP(pvecData4);
      IVP_LANX8S_XP(vecData41, vaData4, pvecData4, inDataPitch1);                  //  y = 2, x=0,1,2,3
      IVP_LANX8S_XP(vecData42, vaData4, pvecData4, inDataPitch1);                  //  y = 2, x=1,2,3,4
      IVP_LANX8S_XP(vecData43, vaData4, pvecData4, 2 * inDataPitch1);              //y = 2, x=2,3,4,5

      /*Multiply and accumulate input data vector and coeff vector */
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

      /* Pack, Output Scale, Output Shift and clamping */
      PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ_FOLD4(accSum1, vecOut);
      PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ_FOLD4(accSum2, vecOut1);

      pvecOut  = (xb_vecNx8 *) (pOut);
      pvecOut1 = (xb_vecNx8 *) (pOut1);

      /* Storing the first and second row */
      IVP_SAVNX8S_XP(vecOut, vaOutData, pvecOut, numCh * 4);
      //FLUSHNX8(vaOutData, pdvecOut, outDataPitch1 * 4, numCh * 4, pOut);
      IVP_SAVNX8S_XP(vecOut1, vaOutData1, pvecOut1, numCh * 4);
      //FLUSHNX8(vaOutData1, pdvecOut1, outDataPitch1 * 4, numCh * 4, pOut1);
      pOut += 4 * outDataPitch1; pOut1 += 4 * outDataPitch1;
    }     /* End for (x = 0; x < outW-1; x +=2) */
  }       /* End for (y = 0; y < outH; y ++) */
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
  xb_vecN_2x32v* __restrict phvecBias1;
  xb_vecN_2x32v* __restrict phvecBias2;
  xb_vecN_2x32v* __restrict phvecOutScale1;
  xb_vecN_2x32v* __restrict phvecOutScale2;
  xb_vec2Nx8* __restrict pdvecOutShift;
  xb_vec2Nx8* __restrict pdvecCoeff;
  xb_vec2Nx8* __restrict pdvecData1;
  xb_vec2Nx8* __restrict pdvecData2;
  xb_vec2Nx8* __restrict pdvecData3;
  xb_vec2Nx8* __restrict pdvecOut;
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

    int32_t remCh1 = XT_MIN(((numCh - ch) << 2), 64);
    int32_t remCh2 = XT_MIN((((numCh - ch) - 16) << 2), 64);
    int32_t remCh3 = XT_MIN((((numCh - ch) - 32) << 2), 64);
    int32_t remCh4 = XT_MIN((((numCh - ch) - 48) << 2), 64);

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
    xb_vec2Nx24 daccSum0;
    phvecBias1 = (xb_vecN_2x32v*)(pBiasData + ch);
    phvecBias2 = (xb_vecN_2x32v*)(pBiasData + ch + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
    BIAS_LOAD(phvecBias1, phvecBias2, daccSum0, remCh1, remCh2, remCh3, remCh4);

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
        /* Initialize accumulator with bias values */
        daccSum = daccSum0;
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

        /* Pack, Output Scale, Output Shift and clamping */;
        PACK_SCALE_AND_ROUNDING_DEPTHWISE_VQ(daccSum, dvecOut);
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
  if (XI_CNNA_CONV_GET_STRIDE(param) == 2 && (XI_TILE3D_GET_DIM1(inTile) == 16) && (XI_TILE3D_GET_DIM1_PITCH(inTile) == XI_TILE3D_GET_DIM1(inTile)))
  {
    depthwiseConvolveAVQ2D_S_3x3j2_S8Ca2_MOD_DWH_FOLD2(inTile, coeffTile, biasArray, outScaleArray, outShiftArray, outTile, param);
  }
  else if (XI_CNNA_CONV_GET_STRIDE(param) == 2)
  {
    depthwiseConvolveAVQ2D_S_3x3j2_S8Ca2_MOD_DWH(inTile, coeffTile, biasArray, outScaleArray, outShiftArray, outTile, param);
  }
  else if (XI_CNNA_CONV_GET_STRIDE(param) == 4)
  {
    depthwiseConvolveAVQ2D_S_3x3j4_S8Ca2_MOD_DWH(inTile, coeffTile, biasArray, outScaleArray, outShiftArray, outTile, param);
  }
  else if ((XI_TILE3D_GET_DIM1(inTile) == 8) && \
           (XI_TILE3D_GET_DIM1_PITCH(inTile) == XI_TILE3D_GET_DIM1(inTile)))
  {
    depthwiseConvolveAVQ2D_S_3x3_S8Ca2_MOD_DWH_FOLD4(inTile, coeffTile, biasArray, outScaleArray, outShiftArray, outTile, param);
  }
  else if((XI_TILE3D_GET_DIM1_PITCH(inTile) <= 32) && \
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

#ifdef IVP_PACKVNX48 /* Only available in Q7*/
#ifdef ENABLE_CONV_ASYMMETRIC_ROUNDING
#define CONV_PACK_AND_ROUNDING(hvecIn1, hvecIn2, vecRightShift, vecOut)      \
{                                                                            \
  /* Performs ASYMMETRIC ROUNDING Operation */                               \
  vecOut = IVP_PACKVNX48(IVP_CVT48SNX32(hvecIn2, hvecIn1), vecRightShift);   \
}
#else  /* #ifdef ENABLE_CONV_ASYMMETRIC_ROUNDING */
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

/* uses 24 bit accumulator for computations. Move the 24 bit accumulated sum      */
/* to 32 bit vec registers. Accumulated value is multiplied with OutScale value and packed by (31 - Left shift)*/
/* value. Then, pack the result to 16 bits. zeroPtOutput is added to the result to get quantized output value. */
/* The result is saturate to minLim and maxLim.                                                                */
#define DEPTHWISE_DILATED_PACK_SCALE_AND_ROUNDING_VQ(daccSum, vecOutL, vecOutH)                                                     \
{                                                                                                              \
  /* Move the 24 bit accumulated sum to 32 bit vec registers. */                                               \
  xb_vecN_2x32v hvecIn1 = IVP_CVT32S2NX24LL(daccSum);                                                          \
  xb_vecN_2x32v hvecIn2 = IVP_CVT32S2NX24LH(daccSum);                                                          \
  xb_vecN_2x32v hvecIn3 = IVP_CVT32S2NX24HL(daccSum);                                                          \
  xb_vecN_2x32v hvecIn4 = IVP_CVT32S2NX24HH(daccSum);                                                          \
  /* accumulated sum is multiplied with output scale value and Packed by 31 - Left shift */                    \
  hvecIn1 = IVP_PACKVN_2X64W(IVP_MULN_2X32(hvecIn1, hvecScale1), IVP_SUBN_2X32(31, hvecLeftShift1));           \
  hvecIn2 = IVP_PACKVN_2X64W(IVP_MULN_2X32(hvecIn2, hvecScale2), IVP_SUBN_2X32(31, hvecLeftShift2));           \
  hvecIn3 = IVP_PACKVN_2X64W(IVP_MULN_2X32(hvecIn3, hvecScale3), IVP_SUBN_2X32(31, hvecLeftShift3));           \
  hvecIn4 = IVP_PACKVN_2X64W(IVP_MULN_2X32(hvecIn4, hvecScale4), IVP_SUBN_2X32(31, hvecLeftShift4));           \
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

#define DEPTHWISE_DILATED_PACK_SCALE_AND_ROUNDING_VQ(daccSum, vecOutL, vecOutH)                                                     \
{                                                                                                              \
  /* Move the 24 bit accumulated sum to 32 bit vec registers. */                                               \
  xb_vecN_2x32v hvecIn1 = IVP_CVT32S2NX24LL(daccSum);                                                          \
  xb_vecN_2x32v hvecIn2 = IVP_CVT32S2NX24LH(daccSum);                                                          \
  xb_vecN_2x32v hvecIn3 = IVP_CVT32S2NX24HL(daccSum);                                                          \
  xb_vecN_2x32v hvecIn4 = IVP_CVT32S2NX24HH(daccSum);                                                          \
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
#endif

#ifdef IVP_PACKVN_2X64W
#define PACK_SCALE_AND_ROUNDING_VQ(vec16H1W1AB, accSum1,                                     \
                                   zeroPtOutputU, vecleftShiftL,vecleftShiftH,               \
                                   vecRightShift, vecScaleL,vecScaleH, maxLimU, minLimU)  {  \
    /* The accumulated sum which is now in 48 bits wvec registers is moved to 32 bit  */     \
    /*  vec registers to handle the addition of bias and fixup values to it. */              \
    /* H -height W-width */                                                                  \
    /* Moving accumlated sum H1-W1 to normal vec registers */                                \
    xb_vecN_2x32v hvecOutH1W1A, hvecOutH1W1B;                                                \
    xb_vecN_2x64w dProd1, dProd2;                                                            \
    hvecOutH1W1A = IVP_CVT32SNX48L(accSum1);                                                 \
    hvecOutH1W1B = IVP_CVT32SNX48H(accSum1);                                                 \
    /* Accumulated value is multiplied with (inputScale*coeffScale) /outputScale;*/          \
    /* zeroPtOutputU is added to the result to get quantized output value. */                \
    /* (inputScale*coeffScale) /outputScale = fractional part * 2^(outputShift) */           \
    /* quantized scale = fractional part in q31 format. */                                   \
    /* H1- W1 */                                                                             \
    /* Handling the 32x32 bit multiplication of accumulated sum with quantScale */           \
    MORPH_IVP_MUL32_QUANTSCALE(dProd1, hvecOutH1W1A, vecScaleL);                             \
    MORPH_IVP_MUL32_QUANTSCALE(dProd2, hvecOutH1W1B, vecScaleH);                             \
    hvecOutH1W1A = IVP_PACKVN_2X64W(dProd1,IVP_SUBN_2X32(31 , vecleftShiftL));               \
    hvecOutH1W1B = IVP_PACKVN_2X64W(dProd2,IVP_SUBN_2X32(31 , vecleftShiftH));               \
    /*Pack the result to 16 bits and  saturate to short min and max*/                        \
    MORPH_IVP_PACKVRNX48_VQ(vec16H1W1AB, hvecOutH1W1B, hvecOutH1W1A, vecRightShift);         \
    vec16H1W1AB = IVP_ADDSNX16(vec16H1W1AB, zeroPtOutputU);                                  \
    vec16H1W1AB = IVP_MAXNX16(IVP_MINNX16(vec16H1W1AB, (xb_vecNx16) maxLimU),                \
                              (xb_vecNx16) minLimU);                                         \
}
#else

#define PACK_SCALE_AND_ROUNDING_VQ(vec16H1W1AB, accSum1,                                     \
                                   zeroPtOutputU, vecleftShiftL,vecleftShiftH,               \
                                   vecRightShift, vecScaleL,vecScaleH, maxLimU, minLimU)  {  \
    /* The accumulated sum which is now in 48 bits wvec registers is moved to 32 bit  */     \
    /*  vec registers to handle the addition of bias and fixup values to it. */              \
    /* H -height W-width */                                                                  \
    /* Moving accumlated sum H1-W1 to normal vec registers */                                \
    xb_vecN_2x32v hvecOutH1W1A, hvecOutH1W1B;                                                \
    xb_vecN_2x64w dProd1, dProd2;                                                            \
    hvecOutH1W1A = IVP_CVT32SNX48L(accSum1);                                                 \
    hvecOutH1W1B = IVP_CVT32SNX48H(accSum1);                                                 \
    hvecOutH1W1A = IVP_SLSN_2X32(hvecOutH1W1A,vecleftShiftL);                                \
    hvecOutH1W1B = IVP_SLSN_2X32(hvecOutH1W1B,vecleftShiftH);                                \
    /* Accumulated value is multiplied with (inputScale*coeffScale) /outputScale;*/          \
    /* zeroPtOutputU is added to the result to get quantized output value. */                \
    /* (inputScale*coeffScale) /outputScale = fractional part * 2^(outputShift) */           \
    /* quantized scale = fractional part in q31 format. */                                   \
    /* H1- W1 */                                                                             \
    /* Handling the 32x32 bit multiplication of accumulated sum with quantScale */           \
    MORPH_IVP_MUL32_QUANTSCALE(dProd1, hvecOutH1W1A, vecScaleL);                             \
    MORPH_IVP_MUL32_QUANTSCALE(dProd2, hvecOutH1W1B, vecScaleH);                             \
    hvecOutH1W1A = IVP_PACKVRN_2X64W(dProd1, 31);                                            \
    hvecOutH1W1B = IVP_PACKVRN_2X64W(dProd2, 31);                                            \
    /*Pack the result to 16 bits and  saturate to short min and max*/                        \
    MORPH_IVP_PACKVRNX48_VQ(vec16H1W1AB, hvecOutH1W1B, hvecOutH1W1A, vecRightShift);         \
    vec16H1W1AB = IVP_ADDSNX16(vec16H1W1AB, zeroPtOutputU);                                  \
    vec16H1W1AB = IVP_MAXNX16(IVP_MINNX16(vec16H1W1AB, (xb_vecNx16) maxLimU),                \
                              (xb_vecNx16) minLimU);                                         \
}
#endif

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
static void depthwiseMultiplierConvolvedAVQ3D_S8_DWH_DM_LTE32(const xi_pTile3D inTile,
                                                              const xi_pTile3D coeffTile,
                                                              const xi_pArray biasArray,
                                                              const xi_pArray scaleArray,
                                                              const xi_pArray shiftArray,
                                                              xi_pTile3D outTile,
                                                              const xi_cnna_depthwiseDilatedConv_params *param)
{
  /* Get Tile Parameters */
  int32_t inDepth      = XI_TILE3D_GET_DIM1(inTile);
  int32_t inPitch1     = XI_TILE3D_GET_DIM1_PITCH(inTile);
  int32_t inPitch2     = XI_TILE3D_GET_DIM2_PITCH(inTile);
  int32_t outWidth     = XI_TILE3D_GET_DIM2(outTile);
  int32_t outHeight    = XI_TILE3D_GET_DIM3(outTile);
  int32_t outPitch1    = XI_TILE3D_GET_DIM1_PITCH(outTile);
  int32_t outPitch2    = XI_TILE3D_GET_DIM2_PITCH(outTile);
  int32_t filterWidth  = XI_TILE3D_GET_DIM2(coeffTile);
  int32_t filterHeight = XI_TILE3D_GET_DIM3(coeffTile);
  int32_t filterPitch1 = XI_TILE3D_GET_DIM1_PITCH(coeffTile);
  /* Input and Output data pointers */
  int8_t *pInput     = (int8_t *)XI_TILE3D_GET_DATA_PTR(inTile);
  int8_t *pOutData   = (int8_t *)XI_TILE3D_GET_DATA_PTR(outTile);
  int8_t *pFilter    = (int8_t *)XI_TILE3D_GET_DATA_PTR(coeffTile);
  int32_t *pBias     = (int32_t *)XI_ARRAY_GET_DATA_PTR(biasArray);
  int32_t *pOutScale = (int32_t *)XI_ARRAY_GET_DATA_PTR(scaleArray);
  int8_t *pOutShift  = (int8_t *)XI_ARRAY_GET_DATA_PTR(shiftArray);
  /* Read Depthwise dilated conv parameters */
  int32_t strideWidth     = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEX(param);
  int32_t strideHeight    = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEY(param);
  int32_t dilationX       = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param);
  int32_t dilationY       = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param);
  int32_t depthMultiplier = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DEPTH_MULTIPLIER(param);
  int32_t inputOffset     = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_INPUT_OFFSET(param);
  int32_t outputOffset    = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_OUTPUT_OFFSET(param);
  int32_t reluMin         = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(param);
  int32_t reluMax         = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(param);
  uint8_t leftEdgeFlag    = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_LEFTEDGE(param);
  uint8_t topEdgeFlag     = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_TOPEDGE(param);

  /* Setting the limits for output data according to ReLu is enabled or not*/
  const int8_t enableReLu = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_RELU(param);
  const int16_t minLim = enableReLu ? reluMin : SCHAR_MIN;
  const int16_t maxLim = enableReLu ? reluMax : SCHAR_MAX;

  /* Input and output data pointers */
  int8_t *restrict pInData11;
  int8_t *restrict pInData21;
  xb_vec2Nx8 *restrict pdvecFilter;
  xb_vec2Nx8 *restrict pdvecOutShift;
  xb_vecN_2x32v *restrict phvecOutScale1;
  xb_vecN_2x32v *restrict phvecOutScale2;
  xb_vecN_2x32v *restrict phvecBias1;
  xb_vecN_2x32v *restrict phvecBias2;

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

  /*Support input offset value  =128*/
  xb_vec2Nx8 dvecOffsetDummy = inputOffset == 128 ? 1 : 0;
  xb_vec2Nx8 dvecInOffset = inputOffset == 128 ? (xb_vec2Nx8)(inputOffset - 1) : (xb_vec2Nx8)inputOffset;
  int32_t vectorizationWidth = (2 * XCHAL_IVPN_SIMD_WIDTH);

  int32_t numWinUnroll, numHinUnroll;
  /*Unrolling factor across width. Dynamic in nature based on depthMultiplier*/
  numWinUnroll = (vectorizationWidth / depthMultiplier) / 2;
  /*Unrolling factor across height = 2 as of now*/
  numHinUnroll = 2;
  int32_t shiftVal = 30 - (int32_t)IVP_NSA32(depthMultiplier);
  /*Assume depthMultiplier = 8. FInal pattern has to be 0,1,2..8,0,1,2 ...8, 0, 1, 2..8  No:of folds = 8*/
  xb_vecNx16 gOffset = IVP_SRLNX16(IVP_SEQNX16(), shiftVal);
  xb_vecNx16 gOffsetW = IVP_MULNX16PACKL(gOffset, strideWidth * inPitch1);
  xb_vecNx16 stOffsetW = IVP_MULNX16PACKL(gOffset, outPitch1);
  stOffsetW = IVP_ADDNX16(IVP_ANDNX16(IVP_SEQNX16(), depthMultiplier - 1), stOffsetW);
  xb_vec2Nx8 dvecFiltSel = IVP_AND2NX8(IVP_SEQ2NX8(), depthMultiplier - 1);
  xb_vecN_2x32v hvecBiasSelLT16 = IVP_ANDN_2X32(IVP_SEQN_2X32(), depthMultiplier - 1);

  /* Vectorization is along depthmultiplier. Output width will be unrolled dynamically */
  /* and output height is  unrolled by 2 and is packed in the vector to compensate for */
  /* lesser depth multiplier values                                                    */
  /* by 2. Kernel height and width and height loops are combined together              */
  /* In this case the address offsets for input  need to be derived from               */
  /* vector registers. These vector registers are initialized as follows               */
  xb_vecN_2x32Uv hvecInAddrOffInit = inPitch1 * dilationX;
  vboolN_2 vbN_2 = IVP_EQN_2X32(IVP_SEQN_2X32(), filterWidth - 1);
  hvecInAddrOffInit = IVP_MOVN_2X32T(((inPitch2 * dilationY) - ((filterWidth - 1) * inPitch1 * dilationX)), hvecInAddrOffInit, vbN_2);

  for (int32_t outY = 0; outY < outHeight; outY += numHinUnroll) /* along output Height */
  {
    /*To handle Odd cases for output height when unrolled by 2*/
    int32_t enable2Row = XT_SALT(outY, outHeight - 1);
    for (int32_t outX = 0; outX < outWidth; outX += numWinUnroll)  /* along output Width */
    {
      int32_t widthRem = XT_MIN(outWidth - outX, numWinUnroll);
      int32_t widthMaxLim = (widthRem - 1) * strideWidth * inPitch1;
      xb_vecNx16 gOffsetWRem = IVP_MINNX16(gOffsetW, widthMaxLim);
      /*Masks for to avoid outof bound storing*/
      vboolN vbWMask = IVP_LENX16(gOffsetW, widthMaxLim);
      vboolN vbHMask = IVP_LENX16(gOffsetW, widthMaxLim * enable2Row);
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

          int32_t remCh1 = XT_MIN(((depthMultiplier - m) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
          int32_t remCh2 = XT_MIN((((depthMultiplier - m) - (XCHAL_IVPN_SIMD_WIDTH >> 1)) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);

          /* Load OutScale values */
          xb_vecN_2x32v hvecScale1, hvecScale2, hvecScale3, hvecScale4;
          phvecOutScale1 = (xb_vecN_2x32v*)(pOutScale + arrayOffset + m);
          phvecOutScale2 = (xb_vecN_2x32v*)(pOutScale + arrayOffset + m + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
          valign vaOutScale = IVP_LAN_2X32_PP(phvecOutScale1);
          IVP_LAVN_2X32_XP(hvecScale1, vaOutScale, phvecOutScale1, remCh1);
          IVP_LAVN_2X32_XP(hvecScale2, vaOutScale, phvecOutScale1, remCh2);

          /* Load OutShift values */
          xb_vec2Nx8 dvecShift;
          pdvecOutShift = (xb_vec2Nx8*)(pOutShift + arrayOffset + m);
          valign vaOutShift = IVP_LA2NX8_PP(pdvecOutShift);
          IVP_LAV2NX8_XP(dvecShift, vaOutShift, pdvecOutShift, remainingOutCh);

          dvecShift = IVP_SEL2NX8(dvecShift, dvecShift, dvecFiltSel);

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
          xb_vec2Nx24 daccSum1;
          phvecBias1 = (xb_vecN_2x32v*)(pBias + arrayOffset + m);
          phvecBias2 = (xb_vecN_2x32v*)(pBias + arrayOffset + m + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
          ACC_INIT_BIAS_DM_2_TO_32(phvecBias1, phvecBias2, daccSum1, remCh1, remCh2);

          /* Input Data Pointers */
          int8_t *pData = pInput + ic + outX * strideWidth * inPitch1 + outY * strideHeight * inPitch2;
          /* Pointers for Input Data Loads */
          pInData11 = (int8_t *)(pData);
          pInData21 = (int8_t *)(pData + strideHeight * inPitch2 * enable2Row);

          /* Pointer for Coefficient Load */
          int8_t *pCoeff = pFilter + arrayOffset + m;
          pdvecFilter = (xb_vec2Nx8 *)(pCoeff);
          int32_t i = 0;
          /* Pointer for Output Load */
          uint8_t* pOut = (uint8_t *)(pOutData + arrayOffset + m + outX * outPitch1 + outY * outPitch2);
          for (int32_t k = 0; k < (filterHeight * filterWidth); k++, i++) // along kernelWidth*kernelHeight
          {
            int32_t inAddrOffset = IVP_EXTRVRN_2X32(hvecInAddrOffInit, 4 * i);
            /* After every filterWidth no: of iterations, the offset, should start from
            0th position in the offset vector*/
            /* Making i = -1 when k = multiple of filterWidth, so that i++ will give i = 0
            for next iteration */
            i |= ((filterWidth - 2) - i) >> 31;
            xb_vec2Nx8 dvecFilter;
            xb_gsr gatherReg1 = IVP_GATHERANX8S(pInData11, gOffsetWRem);
            xb_vecNx16  vecIn1 = IVP_GATHERDNX8S(gatherReg1);
            xb_gsr gatherReg2 = IVP_GATHERANX8S(pInData21, gOffsetWRem);
            xb_vecNx16  vecIn2 = IVP_GATHERDNX8S(gatherReg2);
            xb_vec2Nx8 dvecIn = IVP_SEL2NX8I(IVP_MOV2NX8_FROMNX16(vecIn2), IVP_MOV2NX8_FROMNX16(vecIn1), IVP_SELI_8B_DEINTERLEAVE_1_EVEN);
            /* Load coefficient value*/
            valign valFilter = IVP_LA2NX8_PP(pdvecFilter);
            IVP_LA2NX8_XP(dvecFilter, valFilter, pdvecFilter, filterPitch1);
            dvecFilter = IVP_SEL2NX8(dvecFilter, dvecFilter, dvecFiltSel);

            IVP_MULPA2NX8(daccSum1, dvecIn, dvecFilter, dvecInOffset, dvecFilter);
            IVP_MULA2NX8(daccSum1, dvecOffsetDummy, dvecFilter);
            pInData11 += inAddrOffset;
            pInData21 += inAddrOffset;
          }
          xb_vecNx16 vecOut11, vecOut12;
          DEPTHWISE_DILATED_PACK_SCALE_AND_ROUNDING_VQ(daccSum1, vecOut11, vecOut12);
          /* Store output */
          IVP_SCATTERNX8UT(vecOut11, pOut, stOffsetW, vbWMask);
          uint8_t *  pOut1 = (pOut + outPitch2 * enable2Row);
          IVP_SCATTERNX8UT(vecOut12, pOut1, stOffsetW, vbHMask);
        }
      }
    }
  }
  IVP_SCATTERW();
}

XI_ERR_TYPE xiDepthwiseMultiplierConvolvedAVQ3D_S8_DWH(const xi_pTile3D inTile,
                                                       const xi_pTile3D coeffTile,
                                                       const xi_pArray biasArray,
                                                       const xi_pArray scaleArray,
                                                       const xi_pArray shiftArray,
                                                       xi_pTile3D outTile,
                                                       const xi_cnna_depthwiseDilatedConv_params *param)
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
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(coeffTile) <= 8) && (XI_TILE3D_GET_DIM3(coeffTile) <= 8), XI_ERR_KSIZE, \
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
      XI_CHECK_ERROR(XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(param) <= XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(param),\
                      XI_ERR_BADARG, "\nRelu max = %hi, Relu min = %hi\nRelu max should be greater than or equal to Relu min",\
                      XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(param), XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(param));
      XI_CHECK_ERROR((XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(param) >= SCHAR_MIN), XI_ERR_BADARG, \
                     "\nRelu min = %hi, value should be greater than or equal to -128", \
                     XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(param));
      XI_CHECK_ERROR((XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(param) <= SCHAR_MAX), XI_ERR_BADARG, \
                     "\nRelu max = %hi, value must be less than or equal to 127", \
                     XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(param));
    }
    XI_CHECK_ERROR((XI_CNNA_DEPTHWISE_DILATED_CONV_GET_COEFF_OFFSET(param) == 0), XI_ERR_BADARG,\
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
  /*Depth multiplier should be <= 32 & >= 2 and should be a power of 2*/
  /*no: of folds x strideW x inpitch1 + depth multiplier - 1 should be with in gather limit */
  /*no:of folds x outpitch1  + + depth multiplier - 1 should be with in scatter limits */
  int32_t depthMultiplier = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DEPTH_MULTIPLIER(param);
  if ((depthMultiplier == XCHAL_IVPN_SIMD_WIDTH || depthMultiplier == XCHAL_IVPN_SIMD_WIDTH >> 1 || depthMultiplier == XCHAL_IVPN_SIMD_WIDTH >> 2 || \
    depthMultiplier == XCHAL_IVPN_SIMD_WIDTH >> 4 || depthMultiplier == XCHAL_IVPN_SIMD_WIDTH >> 8) && \
    (((XCHAL_IVPN_SIMD_WIDTH / depthMultiplier - 1)  * XI_TILE3D_GET_DIM1_PITCH(inTile) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEX(param)  \
    + (depthMultiplier - 1)) < 65535) && \
    (((XCHAL_IVPN_SIMD_WIDTH / depthMultiplier - 1)  * XI_TILE3D_GET_DIM1_PITCH(outTile)  \
    + (depthMultiplier - 1)) < 65535))
  {
    depthwiseMultiplierConvolvedAVQ3D_S8_DWH_DM_LTE32(inTile, coeffTile, biasArray, \
      scaleArray, shiftArray, outTile, param);
    return(XI_ERR_OK);
  }

  /* Get Tile Parameters */
  int32_t inDepth      = XI_TILE3D_GET_DIM1(inTile);
  int32_t inPitch1     = XI_TILE3D_GET_DIM1_PITCH(inTile);
  int32_t inPitch2     = XI_TILE3D_GET_DIM2_PITCH(inTile);
  int32_t outWidth     = XI_TILE3D_GET_DIM2(outTile);
  int32_t outHeight    = XI_TILE3D_GET_DIM3(outTile);
  int32_t outPitch1    = XI_TILE3D_GET_DIM1_PITCH(outTile);
  int32_t outPitch2    = XI_TILE3D_GET_DIM2_PITCH(outTile);
  int32_t filterWidth  = XI_TILE3D_GET_DIM2(coeffTile);
  int32_t filterHeight = XI_TILE3D_GET_DIM3(coeffTile);
  int32_t filterPitch1 = XI_TILE3D_GET_DIM1_PITCH(coeffTile);
  /* Input and Output data pointers */
  int8_t *pInput       = (int8_t *)XI_TILE3D_GET_DATA_PTR(inTile);
  int8_t *pOutData     = (int8_t *)XI_TILE3D_GET_DATA_PTR(outTile);
  int8_t *pFilter      = (int8_t *)XI_TILE3D_GET_DATA_PTR(coeffTile);
  int32_t *pBias       = (int32_t *)XI_ARRAY_GET_DATA_PTR(biasArray);
  int32_t *pOutScale   = (int32_t *)XI_ARRAY_GET_DATA_PTR(scaleArray);
  int8_t *pOutShift    = (int8_t *)XI_ARRAY_GET_DATA_PTR(shiftArray);
  /* Read Depthwise dilated conv parameters */
  int32_t strideWidth         = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEX(param);
  int32_t strideHeight        = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEY(param);
  int32_t dilationX           = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param);
  int32_t dilationY           = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param);
  int32_t inputOffset         = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_INPUT_OFFSET(param);
  int32_t outputOffset        = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_OUTPUT_OFFSET(param);
  int32_t reluMin             = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(param);
  int32_t reluMax             = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(param);
  uint8_t leftEdgeFlag        = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_LEFTEDGE(param);
  uint8_t topEdgeFlag         = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_TOPEDGE(param);

  /* Setting the limits for output data according to ReLu is enabled or not*/
  const int8_t enableReLu = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_RELU(param);
  const int16_t minLim = enableReLu ? reluMin : SCHAR_MIN;
  const int16_t maxLim = enableReLu ? reluMax : SCHAR_MAX;

  /* Input and output data pointers */
  int8_t *restrict pInData11;
  int8_t *restrict pInData12;
  int8_t *restrict pInData21;
  int8_t *restrict pInData22;
  xb_vec2Nx8 *restrict pdvecOutData;
  xb_vec2Nx8 *restrict pdvecFilter;
  xb_vec2Nx8 *restrict pdvecOutShift;
  xb_vecN_2x32v *restrict phvecOutScale1;
  xb_vecN_2x32v *restrict phvecOutScale2;
  xb_vecN_2x32v *restrict phvecBias1;
  xb_vecN_2x32v *restrict phvecBias2;
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

          int32_t remCh1 = XT_MIN(((depthMultiplier - m) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
          int32_t remCh2 = XT_MIN((((depthMultiplier - m) - (XCHAL_IVPN_SIMD_WIDTH >> 1)) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
          int32_t remCh3 = XT_MIN((((depthMultiplier - m) - XCHAL_IVPN_SIMD_WIDTH) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);
          int32_t remCh4 = XT_MIN((((depthMultiplier - m) - ((XCHAL_IVPN_SIMD_WIDTH * 3 ) >> 1)) << 2), XCHAL_IVPN_SIMD_WIDTH * 2);

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
          xb_vec2Nx24 daccSum1, daccSum2, daccSum3, daccSum4;
          phvecBias1 = (xb_vecN_2x32v*)(pBias + arrayOffset + m);
          phvecBias2 = (xb_vecN_2x32v*)(pBias + arrayOffset + m + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
          ACC_INIT_BIAS(phvecBias1, phvecBias2, daccSum1, daccSum2, daccSum3, daccSum4, remCh1, \
            remCh2, remCh3, remCh4);
          /* Input Data Pointers */
          int8_t *pData = pInput + ic + outX * strideWidth * inPitch1 + outY * strideHeight * inPitch2;
          /* Pointers for Input Data Loads */
          pInData11 = (int8_t *)(pData);
          pInData12 = (int8_t *)(pData + strideWidth * inPitch1 * enable2Col);
          pInData21 = (int8_t *)(pData + strideHeight * inPitch2 * enable2Row);
          pInData22 = (int8_t *)(pData + (strideWidth * inPitch1 + strideHeight * inPitch2) * enable2RowCol);

          /* Pointer for Coefficient Load */
          int8_t *pCoeff = pFilter + arrayOffset + m;
          pdvecFilter = (xb_vec2Nx8 *)(pCoeff);
          int32_t i = 0;
          int32_t zeroPointIn = ((int32_t)inputOffset << 16) | ((int32_t)inputOffset & 0XFFFF);
          /* Pointer for Output Load */
          int8_t* pOut = pOutData + arrayOffset + m + outX * outPitch1 + outY * outPitch2;
          int32_t k;
          for (k = 0; k < (filterHeight * filterWidth) -1; k+=2, i++) // along kernelWidth*kernelHeight
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
          DEPTHWISE_DILATED_PACK_SCALE_AND_ROUNDING_VQ(daccSum1, vecOut11, vecOut12);
          DEPTHWISE_DILATED_PACK_SCALE_AND_ROUNDING_VQ(daccSum2, vecOut21, vecOut22);
          DEPTHWISE_DILATED_PACK_SCALE_AND_ROUNDING_VQ(daccSum3, vecOut31, vecOut32);
          DEPTHWISE_DILATED_PACK_SCALE_AND_ROUNDING_VQ(daccSum4, vecOut41, vecOut42);

          /* Store output */
          dvecOut1 = IVP_SEL2NX8I(IVP_MOV2NX8_FROMNX16(vecOut12), IVP_MOV2NX8_FROMNX16(vecOut11), IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0);
          dvecOut2 = IVP_SEL2NX8I(IVP_MOV2NX8_FROMNX16(vecOut22), IVP_MOV2NX8_FROMNX16(vecOut21), IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0);
          dvecOut3 = IVP_SEL2NX8I(IVP_MOV2NX8_FROMNX16(vecOut32), IVP_MOV2NX8_FROMNX16(vecOut31), IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0);
          dvecOut4 = IVP_SEL2NX8I(IVP_MOV2NX8_FROMNX16(vecOut42), IVP_MOV2NX8_FROMNX16(vecOut41), IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0);

          pdvecOutData = (xb_vec2Nx8 *)(pOut);
          IVP_SAV2NX8_XP(dvecOut1, vaOutData, pdvecOutData, remainingOutCh);
          IVP_SAPOS2NX8_FP(vaOutData, pdvecOutData);

          pdvecOutData = (xb_vec2Nx8 *)(pOut + outPitch1 * enable2Col);
          IVP_SAV2NX8_XP(dvecOut2, vaOutData, pdvecOutData, remainingOutCh * enable2Col);
          IVP_SAPOS2NX8_FP(vaOutData, pdvecOutData);

          pdvecOutData = (xb_vec2Nx8 *)(pOut + outPitch2 * enable2Row);
          IVP_SAV2NX8_XP(dvecOut3, vaOutData, pdvecOutData, remainingOutCh * enable2Row);
          IVP_SAPOS2NX8_FP(vaOutData, pdvecOutData);

          pdvecOutData = (xb_vec2Nx8 *)(pOut + outPitch1  * enable2Col + outPitch2 * enable2Row);
          IVP_SAV2NX8_XP(dvecOut4, vaOutData, pdvecOutData, remainingOutCh * enable2Col * enable2Row);
          IVP_SAPOS2NX8_FP(vaOutData, pdvecOutData);
        }
      }
    }
  }
  return(XI_ERROR_STATUS());
}

