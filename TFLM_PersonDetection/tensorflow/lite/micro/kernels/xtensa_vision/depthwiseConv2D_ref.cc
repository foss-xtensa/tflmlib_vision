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
#include <vector>
#include <stdio.h>
#include <assert.h>
#include "xi_tile3d_manager.h"
#include "flk_pool.h"
#include "flk_conv.h"
#include "xi_core_api.h"
#include "xi_cnn_api.h"
#include "xi_api_ref.h"
#include "xi_core.h"

#define min(x,y) ((x) > (y) ? (y) : (x))
#define max(x,y) ((x) > (y) ? (x) : (y))

#if ((XCHAL_VISION_TYPE >= 6)) // && defined(INCLUDE_XI_CNN))
#define S24_MIN  (-(((int32_t) 1) << 23))
#define S24_MAX  ((((int32_t) 1) << 23) - 1)
#define CLAMP(v, min, max)                      ((v) < (min) ? (min) : (v) > (max) ? (max) : (v))
#define ROUND(x, s)                             (((s) == 0) ? (x) : (((x) + ((int64_t)1 << ((s) - 1))) >> (s)))
#define ROUND64B_ASYMM(x, s)                    (((s) == 0) ? (x) : \
                                                 (((x) + ((int64_t) 1 << ((s) - 1))) >> (s)))

#define ROUND_N_CLAMP64B_ASYMM(x, s, min, max)  (((s) == 0) ? (CLAMP(x, min, max)) : \
                                                 (CLAMP(ROUND64B_ASYMM(x, s), min, max)))

#define ROUND_N_CLAMP64B_SYMM(x, s, min, max)   (((s) == 0) ? (CLAMP(x, min, max)) : \
                                                 (CLAMP(ROUND64B_SYMM(x, s), min, max)))

#define ROUND64B_SYMM(x, s)                     (((s) == 0) ? (x) :                                         \
                                                 (((x) > 0) ? (((x) + (((int64_t) 1) << (s - 1))) >> (s)) : \
                                                  -(((-x) + (((int64_t) 1) << (s - 1))) >> (s))))

#define ROUND_N_CLAMP64B        ROUND_N_CLAMP64B_ASYMM
#ifdef ENABLE_CONV_ASYMMETRIC_ROUNDING
#define MORPH_ROUND64B          ROUND64B_ASYMM
#define MORPH_ROUND_N_CLAMP64B  ROUND_N_CLAMP64B_ASYMM
#else
#define MORPH_ROUND64B          ROUND64B_SYMM
#define MORPH_ROUND_N_CLAMP64B  ROUND_N_CLAMP64B_SYMM
#endif
#endif

/**************** xiDepthwiseMultiplierConvolvedA3D_U8_DWH_ref *****************/
/* Description : Reference implementation of 3D depthwise dilated              */
/*               convolution for Android NN                                    */
/* Inputs      : Input Data Tile, Coeff Data Tile, Bias Array,                 */
/*               CNNA depthwise dilated convolution params structure.          */
/* Outputs     : XI Error Code                                                 */
/* InOuts      : Output Tile                                                   */
/* Assumptions : InData supported is U8                                        */
/*               CoeffData supported is U8                                     */
/*               OutData supported is U8                                       */
/*               Input and Output are in DWH format.                           */
/*               Coeff is in DWH format.                                       */
/*******************************************************************************/
XI_ERR_TYPE xiDepthwiseMultiplierConvolvedA3D_U8_DWH_ref(const xi_pTile3D inTile,
                                                         const xi_pTile3D coeffTile,
                                                         const xi_pArray biasArray,
                                                         xi_pTile3D outTile,
                                                         const xi_cnna_depthwiseDilatedConv_params *param)
{
#if 1
	/* Error Checks */
  if (!xiTile3DIsValid_U8_ref(inTile))
  {
    return(XI_ERR_BADARG);
  }
  if (!xiTile3DIsValid_U8_ref(coeffTile))
  {
    return(XI_ERR_BADARG);
  }
  if (!xiTile3DIsValid_U8_ref(outTile))
  {
    return(XI_ERR_BADARG);
  }
  if (!xiArrayIsValid_S32_ref(biasArray))
  {
    return(XI_ERR_BADARG);
  }
  if (!param)
  {
    return(XI_ERR_NULLARG);
  }
  if (!((XI_TILE3D_GET_DATA_ORDER(inTile)    == XI_DWH) &&  \
        (XI_TILE3D_GET_DATA_ORDER(outTile)   == XI_DWH) && \
        (XI_TILE3D_GET_DATA_ORDER(coeffTile) == XI_DWH)))
  {
    return(XI_ERR_BADARG);
  }
  if (!(XI_TILE3D_GET_DIM1(inTile) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DEPTH_MULTIPLIER(param) == XI_TILE3D_GET_DIM1(outTile)))
  {
    return(XI_ERR_DATASIZE);
  }
  if (!(XI_TILE3D_GET_DIM1(outTile) == XI_TILE3D_GET_DIM1(coeffTile)))
  {
    return(XI_ERR_DATASIZE);
  }
  if (!(XI_ARRAY_GET_WIDTH(biasArray) >= XI_TILE3D_GET_DIM1(outTile)))
  {
    return(XI_ERR_DATASIZE);
  }
  if (!(XI_ARRAY_GET_HEIGHT(biasArray) > 0))
  {
    return(XI_ERR_DATASIZE);
  }
  //Array overlap check
  if (!(XI_TILE3D_GET_DATA_PTR(inTile) != XI_TILE3D_GET_DATA_PTR(outTile)))
  {
    return(XI_ERR_INPLACE);
  }
  if (!(XI_TILE4D_GET_DATA_PTR(coeffTile) != XI_TILE3D_GET_DATA_PTR(outTile)))
  {
    return(XI_ERR_INPLACE);
  }
  if (!((XI_TILE4D_GET_DIM2(coeffTile) <= 8) && (XI_TILE4D_GET_DIM3(coeffTile) <= 8)))
  {
    return(XI_ERR_KSIZE);
  }
  if (!((XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) <= 36) && (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) <= 36)))
  {
    return(XI_ERR_BADARG);
  }
  if (!((XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEX(param) <= 4) && (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEY(param) <= 4)))
  {
    return(XI_ERR_BADARG);
  }
  //Check for coefficient dimension and input edges.
  if (!(((XI_TILE4D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) % 2 == 0))
  {
    // Odd filter dimensions
    // edgeLeft >= dilatedKernelWidth / 2
    if (!(XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (((XI_TILE4D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2)))
    {
      return(XI_ERR_EDGE);
    }
    if (!(XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (((XI_TILE4D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2)))
    {
     return(XI_ERR_EDGE);
    }
    // inWidth >= (outWidth - 1) * strideWidth + 1
    if (!(XI_TILE3D_GET_DIM2(inTile) >= (XI_TILE3D_GET_DIM2(outTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEX(param) + 1))
    {
      return(XI_ERR_BADARG);
    }
  }
  else
  {
    // Even filter dimensions
    if (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_LEFTEDGE(param))
    {
      // edgeLeft >= dilatedKernelWidth / 2
      if (!(XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (((XI_TILE4D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2)))
      {
        return(XI_ERR_EDGE);
      }
      if (!(XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((((XI_TILE4D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2) - 1)))
      {
       return(XI_ERR_EDGE);
      }
    }
    else
    {
      // edgeLeft >= dilatedKernelWidth / 2 - 1
      if (!(XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((((XI_TILE4D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2) - 1)))
      {
        return(XI_ERR_EDGE);
      }
      if (!(XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((((XI_TILE4D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2))))
      {
       return(XI_ERR_EDGE);
      }
    }
    // inWidth >= (outWidth - 1) * strideWidth + 1
    if (!(XI_TILE3D_GET_DIM2(inTile) >= (XI_TILE3D_GET_DIM2(outTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEX(param) + 1))
    {
      return(XI_ERR_BADARG);
    }
  }
  if (!(((XI_TILE4D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) % 2 == 0))
  {
    // Odd filter dimensions
    // edgeRight >= dilatedKernelHeight / 2
    if (!(XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (((XI_TILE4D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2)))
    {
      return(XI_ERR_EDGE);
    }
    if (!(XI_TILE3D_GET_DIM3_EDGE2(inTile) >= (((XI_TILE4D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2)))
    {
     return(XI_ERR_EDGE);
    }
    // inHeight >= (outHeight - 1) * strideHeight + 1
    if (!(XI_TILE3D_GET_DIM3(inTile) >= (XI_TILE3D_GET_DIM3(outTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEY(param) + 1))
    {
      return(XI_ERR_BADARG);
    }
  }
  else
  {
    // Even filter dimensions
    if (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_TOPEDGE(param))
    {
      // edgeTop >= dilatedKernelHeight / 2
      if (!(XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (((XI_TILE4D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2)))
      {
        return(XI_ERR_EDGE);
      }
      if (!(XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((((XI_TILE4D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2) - 1)))
      {
       return(XI_ERR_EDGE);
      }
    }
    else
    {
      // edgeTop >= dilatedKernelHeight / 2 - 1
      if (!(XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((((XI_TILE4D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2) - 1)))
      {
        return(XI_ERR_EDGE);
      }
      if (!(XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((((XI_TILE4D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2))))
      {
       return(XI_ERR_EDGE);
      }
    }
    // inHeight >= (outHeight - 1) * strideHeight + 1
    if (!(XI_TILE3D_GET_DIM3(inTile) >= (XI_TILE3D_GET_DIM3(outTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEY(param) + 1))
    {
      return(XI_ERR_BADARG);
    }
  }

  if (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_RELU(param))
  {
    if (((XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(param) < XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(param))))
    {
     return(XI_ERR_BADARG);
    }
    if (!((XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(param) >= 0) && (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(param) <= UCHAR_MAX)))
    {
     return(XI_ERR_BADARG);
    }
    if (!((XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(param) >= 0) && (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(param) <= UCHAR_MAX)))
    {
     return(XI_ERR_BADARG);
    }
  }
#endif
  /* Get Tile Parameters */
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
  int32_t filterPitch2        = XI_TILE3D_GET_DIM2_PITCH(coeffTile);
  uint8_t *pInput             = (uint8_t *) XI_TILE3D_GET_DATA_PTR(inTile);
  uint8_t *pOutput            = (uint8_t *) XI_TILE3D_GET_DATA_PTR(outTile);
  uint8_t *pFilter            = (uint8_t *) XI_TILE4D_GET_DATA_PTR(coeffTile);
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
  int32_t rightShift  = outShift < 0 ? 0 : outShift;

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

  for (int32_t outY = 0; outY < outHeight; ++outY)
  {
    for (int32_t outX = 0; outX < outWidth; ++outX)
    {
      for (int32_t ic = 0; ic < inDepth; ++ic)
      {
        for (int32_t m = 0; m < depthMultiplier; m++)
        {
          int32_t fc        = m + ic * depthMultiplier;
          int32_t oc        = m + ic * depthMultiplier;
          int32_t inXorigin = (outX * strideWidth);
          int32_t inYorigin = (outY * strideHeight);
          int32_t acc = pBias[fc];
          for (int32_t filterY = 0; filterY < filterHeight; ++filterY)
          {
            for (int32_t filterX = 0; filterX < filterWidth; ++filterX)
            {
              int32_t inX         = inXorigin + dilationX * filterX;
              int32_t inY         = inYorigin + dilationY * filterY;
              int32_t inputIndex  = ic + inX * inPitch1 + inY * inPitch2;
              int32_t filterIndex = fc + filterX * filterPitch1 + filterY * filterPitch2;
              int32_t inputVal    = pInput[inputIndex];
              int32_t filterVal   = pFilter[filterIndex];
              acc += (filterVal + filterOffset) * (inputVal + inputOffset);
            }
          }
          int64_t acc64  = ROUND((((int64_t) acc) * outMultiplier), 31 - leftShift);
          acc = (int32_t)(MORPH_ROUND_N_CLAMP64B(acc64, rightShift, SHRT_MIN, SHRT_MAX));
          acc += outputOffset;
          uint8_t result = CLAMP(acc, minLim, maxLim);
          int32_t outputIndex = oc + outX * outPitch1 + outY * outPitch2;
          pOutput[outputIndex] = result;
        }
      }
    }
  }
  return(XI_ERR_OK);
}

/* For input tile at (X, Y) of (W, H) with edges find intersection with input data and amount of padding needed.
   Schedules DMA transfer to bring valid data into tile.
   Sets tile status to non-zero if zero padding is needed. */
static inline void
transfer_input_tile(uint8_t* inputPtr, xi_pTile3D inp, const conv_params_t* params)
{
    int boundX = XI_TILE3D_GET_DIM2_COORD(inp) - XI_TILE3D_GET_DIM2_EDGE1(inp);
    int boundY = XI_TILE3D_GET_DIM3_COORD(inp) - XI_TILE3D_GET_DIM3_EDGE1(inp);
    int boundW = XI_TILE3D_GET_DIM2_COORD(inp) + XI_TILE3D_GET_DIM2(inp) + XI_TILE3D_GET_DIM2_EDGE2(inp);
    int boundH = XI_TILE3D_GET_DIM3_COORD(inp) + XI_TILE3D_GET_DIM3(inp) + XI_TILE3D_GET_DIM3_EDGE2(inp);

    int validX = max(0, boundX);
    int validY = max(0, boundY);
    int validW = min(boundW, params->input.W) - validX;
    int validH = min(boundH, params->input.H) - validY;

    int pad_left = validX - boundX;
    int pad_top = validY - boundY;
    int pad_right = boundW - validX - validW;
    int pad_bottom = boundH - validY - validH;

    /* Indicate that tile needs edge extension function called */
    XI_TILE3D_SET_STATUS_FLAGS(inp, (pad_left | pad_top | pad_right | pad_bottom) != 0);

    uint8_t* tilePtr = (uint8_t*)XI_TILE3D_GET_BUFF_PTR(inp);
    uint8_t* src = inputPtr + (validY * params->input.W + validX) * params->input.D + XI_TILE3D_GET_DIM1_COORD(inp);
    uint8_t* dst = tilePtr + pad_top * XI_TILE3D_GET_DIM2_PITCH(inp) + pad_left * XI_TILE3D_GET_DIM1_PITCH(inp);
    int rowSize = XI_TILE3D_GET_DIM1(inp);
    int srcStride2d = params->input.D;
    int dstStride2d = XI_TILE3D_GET_DIM1_PITCH(inp);
    int count2d = validW;
    int srcStride3d = params->input.D * params->input.W;
    int dstStride3d = XI_TILE3D_GET_DIM2_PITCH(inp);
    int count3d = validH;

    for (int d3 = 0; d3 < count3d; d3++)
    {
        uint8_t* src1 = src;
        uint8_t* dst1 = dst;
        for (int d2 = 0; d2 < count2d; d2++)
        {
            for (int d1 = 0; d1 < rowSize; d1++)
            {
                dst1[d1] = src1[d1];
            }
            dst1 += dstStride2d;
            src1 += srcStride2d;
        }
        dst += dstStride3d;
        src += srcStride3d;
    }
}

void flk_depthwise_conv_ref(const uint8_t* raw_params,
    struct XtensaOperationArgsIn* input,
    struct XtensaOperationArgsOut* output)
{
    XI_ERROR_CHECKS()
	const conv_params_t* params = (const conv_params_t*)raw_params;
    int zeroPtInput = - params->zeroPtInput ;
    int zeroPtCoeff = - params->zeroPtCoeff ;

    /* Input tile edges */
    unsigned edge_left = params->kernelW / 2;
    unsigned edge_top = params->kernelH / 2;
    unsigned edge_right = params->kernelW - edge_left - 1;
    unsigned edge_bottom = params->kernelH - edge_top - 1;

    // Max input tile dimensions, assume whole output in one tile
    int inpTileW = (params->output.W - 1) * params->stride + 1;
    int inpTileH = (params->output.H - 1) * params->stride + 1;

    // Tile size in bytes for each data object
    int depthMultiplier = params->output.D / params->input.D;
    int inpTileSize = (params->tile.D / depthMultiplier) * (inpTileW + params->kernelW - 1) * (inpTileH + params->kernelH - 1);
    unsigned paddedWidth = edge_left + inpTileW + edge_right;
    unsigned paddedheight = edge_top + inpTileH + edge_bottom;

    std::vector<uint8_t> mInputPadded;
    mInputPadded.resize(inpTileSize);
    memset(reinterpret_cast<uint8_t*>(&mInputPadded[0]), zeroPtInput, mInputPadded.size());

    struct {
        xi_tile3D tiles3D[3];
        xi_array  arr1DBias;
        xi_cnna_depthwiseDilatedConv_params xiparams_dm;
    } structs;

    xi_pTile3D tile3DInp = &structs.tiles3D[0];
    xi_pTile3D tile3DOutp = &structs.tiles3D[1];
    xi_pTile3D tile3DCoeff = &structs.tiles3D[2];
    xi_cnna_depthwiseDilatedConv_params* xiparams = &structs.xiparams_dm;
    memset(&structs, 0, sizeof(structs));

    // output
    XI_TILE3D_SET_DATA_ORDER(tile3DOutp, XI_DWH);
    XI_TILE3D_SET_TYPE(tile3DOutp, XI_TILE3D_U8);
    /* Set output tile coordinates */
    XI_TILE3D_SET_DIM1_COORD(tile3DOutp, 0);
    XI_TILE3D_SET_DIM2_COORD(tile3DOutp, 0);
    XI_TILE3D_SET_DIM3_COORD(tile3DOutp, 0);
    XI_TILE3D_SET_DIM1(tile3DOutp, params->output.D);
    XI_TILE3D_SET_DIM2(tile3DOutp, params->output.W);
    XI_TILE3D_SET_DIM3(tile3DOutp, params->output.H);
    /* Update output tile pitch according to tile size */
    XI_TILE3D_SET_DIM1_PITCH(tile3DOutp, XI_TILE3D_GET_DIM1(tile3DOutp));
    XI_TILE3D_SET_DIM2_PITCH(tile3DOutp, XI_TILE3D_GET_DIM2(tile3DOutp)* XI_TILE3D_GET_DIM1(tile3DOutp));

    // input
    XI_TILE3D_SET_BUFF_SIZE(tile3DInp, mInputPadded.size());
    XI_TILE3D_SET_BUFF_PTR(tile3DInp, reinterpret_cast<uint8_t*>(&mInputPadded[0]));
    XI_TILE3D_SET_DATA_ORDER(tile3DInp, XI_DWH);
    XI_TILE3D_SET_TYPE(tile3DInp, XI_TILE3D_U8);
    XI_TILE3D_SET_DIM1(tile3DInp, params->input.D);
    XI_TILE3D_SET_DIM1_PITCH(tile3DInp, params->input.D);
    XI_TILE3D_SET_DIM2_EDGE1(tile3DInp, edge_left);
    XI_TILE3D_SET_DIM2_EDGE2(tile3DInp, edge_right);
    XI_TILE3D_SET_DIM3_EDGE1(tile3DInp, edge_top);
    XI_TILE3D_SET_DIM3_EDGE2(tile3DInp, edge_bottom);
    /* Compute input tile coordinates */
    XI_TILE3D_SET_DIM1_COORD(tile3DInp, 0);
    XI_TILE3D_SET_DIM2_COORD(tile3DInp, params->offsetX);
    XI_TILE3D_SET_DIM3_COORD(tile3DInp, params->offsetY);
    /* Compute input tile dimensions */
    XI_TILE3D_SET_DIM1(tile3DInp, params->input.D);
    XI_TILE3D_SET_DIM2(tile3DInp, (XI_TILE3D_GET_DIM2(tile3DOutp) - 1)* params->stride + 1);
    XI_TILE3D_SET_DIM3(tile3DInp, (XI_TILE3D_GET_DIM3(tile3DOutp) - 1)* params->stride + 1);
    /* Update input tile pitch according to tile size */
    XI_TILE3D_SET_DIM1_PITCH(tile3DInp, XI_TILE3D_GET_DIM1(tile3DInp));
    XI_TILE3D_SET_DIM2_PITCH(tile3DInp, (XI_TILE3D_GET_DIM2(tile3DInp) + XI_TILE3D_GET_DIM2_EDGE1(tile3DInp) + XI_TILE3D_GET_DIM2_EDGE2(tile3DInp))* XI_TILE3D_GET_DIM1_PITCH(tile3DInp));
    /* Update data pointer, edge in depth is always 0 */
    XI_TILE3D_SET_DATA_PTR(tile3DInp, (uint8_t*)XI_TILE3D_GET_BUFF_PTR(tile3DInp)
        + XI_TILE3D_GET_DIM1_PITCH(tile3DInp) * XI_TILE3D_GET_DIM2_EDGE1(tile3DInp)
        + XI_TILE3D_GET_DIM2_PITCH(tile3DInp) * XI_TILE3D_GET_DIM3_EDGE1(tile3DInp));

    // coeff
    XI_TILE3D_SET_DIM1_PITCH(tile3DCoeff, align_up(params->output.D, ALIGNMENT));
    XI_TILE3D_SET_DIM2_PITCH(tile3DCoeff, XI_TILE3D_GET_DIM1_PITCH(tile3DCoeff)* params->kernelW);
    XI_TILE3D_SET_DIM1(tile3DCoeff, params->output.D);
    XI_TILE3D_SET_DIM1_COORD(tile3DCoeff, XI_TILE3D_GET_DIM1_COORD(tile3DOutp));
    XI_TILE3D_SET_BUFF_SIZE(tile3DCoeff, input->argsSize[1]);
    XI_TILE3D_SET_BUFF_PTR(tile3DCoeff, (uint8_t*)input->args[1]);
    XI_TILE3D_SET_DATA_PTR(tile3DCoeff, (uint8_t*)input->args[1]);
    XI_TILE3D_SET_DATA_ORDER(tile3DCoeff, XI_DWH);
    XI_TILE3D_SET_DIM2(tile3DCoeff, params->kernelW);
    XI_TILE3D_SET_DIM3(tile3DCoeff, params->kernelH);
    XI_TILE3D_SET_TYPE(tile3DCoeff, XI_TILE3D_U8);

    // xi_params
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_STRIDEX(xiparams, params->stride);
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_STRIDEY(xiparams, params->stride);
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_DILATIONX(xiparams, 1);
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_DILATIONY(xiparams, 1);
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_DEPTH_MULTIPLIER(xiparams, params->output.D / params->input.D);
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_INPUT_OFFSET(xiparams, zeroPtInput);
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_COEFF_OFFSET(xiparams, zeroPtCoeff);
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_OUTPUT_OFFSET(xiparams, params->zeroPtOutput);
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_OUTPUT_MULTIPLIER(xiparams, params->quantizedScale);
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_OUTPUT_SHIFT(xiparams, params->outputShift);
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_RELU_MIN(xiparams, params->reluMin);
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_RELU_MAX(xiparams, params->reluMax);
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_FLAGS(xiparams, 0);
    if (CONV_FLAG_GET_RELU(params->flags))
        XI_CNNA_DEPTHWISE_DILATED_CONV_SET_FLAG_RELU(xiparams);
    if (params->kernelW % 2 == 0)
        XI_CNNA_DEPTHWISE_DILATED_CONV_SET_FLAG_LEFTEDGE(xiparams);
    if (params->kernelH % 2 == 0)
        XI_CNNA_DEPTHWISE_DILATED_CONV_SET_FLAG_TOPEDGE(xiparams);

    // bias
    XI_ARRAY_SET_BUFF_SIZE(&structs.arr1DBias, (uint8_t*)input->argsSize[2]);
    XI_ARRAY_SET_BUFF_PTR(&structs.arr1DBias, (uint8_t*)input->args[2]);
    XI_ARRAY_SET_DATA_PTR(&structs.arr1DBias, (uint8_t*)input->args[2]);
    XI_ARRAY_SET_HEIGHT(&structs.arr1DBias, 1);
    XI_ARRAY_SET_TYPE(&structs.arr1DBias, XI_ARRAY_S32);
    XI_ARRAY_SET_WIDTH(&structs.arr1DBias, XI_TILE3D_GET_DIM1(tile3DOutp));
    XI_ARRAY_SET_CAPACITY(&structs.arr1DBias, XI_TILE3D_GET_DIM1(tile3DOutp));

    for (unsigned int B = 0; B < params->batch; B++)
    {
        transfer_input_tile((uint8_t*)input->args[0] + B * params->input.D * params->input.H * params->input.W, tile3DInp, params);

        int outputSizeperBatch = params->output.D * params->output.H * params->output.W;
        XI_TILE3D_SET_BUFF_SIZE(tile3DOutp, outputSizeperBatch);
        XI_TILE3D_SET_BUFF_PTR(tile3DOutp, (uint8_t*)output->args[0] + B * outputSizeperBatch);
        XI_TILE3D_SET_DATA_PTR(tile3DOutp, (uint8_t*)output->args[0] + B * outputSizeperBatch);

        xiDepthwiseMultiplierConvolvedA3D_U8_DWH_ref(tile3DInp, tile3DCoeff,
            &structs.arr1DBias, tile3DOutp, xiparams);
    }

    return ;
}
