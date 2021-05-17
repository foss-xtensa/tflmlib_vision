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
#include <string.h>
#include <assert.h>
#include "cnnrt.h"
#include "xi_tile3d_manager.h"
#include "flk_conv.h"
#include "utils.h"

#define max(a, b)                         ((a > b) ? a : b)
#define min(a, b)                         ((a > b) ? b : a)

uint8_t inputZeros[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

typedef XI_ERR_TYPE (*xiConvolvedA3D_f)(const xi_pTile3D inTile,
                                        const xi_pTile fixUpTile,
                                        const xi_pTile4D coeffTile,
                                        const xi_pArray biasArray,
                                        xi_pTile3D outTile,
                                        const xi_cnna_conv_params *param);

typedef XI_ERR_TYPE (*xiConvolvedFixupA3D_f)(xi_pTile3D inTile,
                                             const xi_pTile4D coeffTile,
                                             const xi_pTile3D outTile,
                                             xi_pTile fixupTile,
                                             const xi_cnna_conv_params *param);

typedef XI_ERR_TYPE (*xiConvolvedAVQ3D_f)(const xi_pTile3D inTile,
                                          const xi_pTile4D coeffTile,
                                          const xi_pArray biasArray,
                                          const xi_pArray outScaleArray,
                                          const xi_pArray outShiftArray,
                                          xi_pTile3D outTile,
                                          const xi_cnna_conv_params *param);

typedef XI_ERR_TYPE (*xiConvolvedA3D_VQ7_QM_f)(const xi_pTile3D inTile,
                                               const xi_pArray inPtrOffsetArr,
                                               const xi_pTile4D coeffTile,
                                               const xi_pArray biasArray,
                                               const xi_pTile fixUpTile,
                                               xi_pTile3D outTile,
                                               const xi_cnna_conv_params *param);

#if 0
/* Setup fixup tile parameters acording to output tile parameters */
INLINE void
setup_fixup_tile(const xi_pTile3D outp, xi_pTile fixup, const int32_t isVQ7optimize)
{
  XI_TILE_SET_WIDTH(fixup, XI_TILE3D_GET_DIM2(outp));
  XI_TILE_SET_HEIGHT(fixup, XI_TILE3D_GET_DIM3(outp));
  XI_TILE_SET_PITCH(fixup, XI_TILE3D_GET_DIM2(outp));

  if (isVQ7optimize) {
    XI_TILE_SET_WIDTH(fixup, XI_TILE3D_GET_DIM1(outp) >> 4);
    XI_TILE_SET_PITCH(fixup, XI_TILE3D_GET_DIM1(outp) >> 4);
  }
  else {
    XI_TILE_SET_WIDTH(fixup, XI_TILE3D_GET_DIM2(outp));
    XI_TILE_SET_PITCH(fixup, XI_TILE3D_GET_DIM2(outp));
  }
  XI_TILE_SET_HEIGHT(fixup, XI_TILE3D_GET_DIM3(outp));
}
#endif
#if 0
XI_ERR_TYPE xiConvolvedFixupA3D_GS_MOW1x1(xi_pTile3D inTile,
                                          const xi_pTile4D coeffTile,
                                          const xi_pTile3D outTile,
                                          xi_pTile fixupTile,
                                          const xi_cnna_conv_params *param)
{
  setup_fixup_tile(outTile, fixupTile, 0);
  return(XI_KERNEL_NAME (xiConvolvedFixupA3D_1x1_U8S32_DWH_WHD)(inTile, coeffTile, outTile, fixupTile, param));
}

XI_ERR_TYPE xiConvolvedFixupA3D_QM32(xi_pTile3D inTile,
                                     const xi_pTile4D coeffTile,
                                     const xi_pTile3D outTile,
                                     xi_pTile fixupTile,
                                     const xi_cnna_conv_params *param)
{
  setup_fixup_tile(outTile, fixupTile, 0);
  return(XI_KERNEL_NAME (xiConvolvedFixupA3D_QM32_MxN_U8S32)(inTile, coeffTile, outTile, fixupTile, param));
}
#endif
XI_ERR_TYPE xiConvolveAVQ3D_QM24_S8_DWH(const xi_pTile3D inTile,
                                         const xi_pTile4D coeffTile,
                                         const xi_pArray biasArray,
                                         const xi_pArray outScaleArray,
                                         const xi_pArray outShiftArray,
                                         const xi_pTile3D outTile,
                                         const xi_cnna_conv_params *param)
{
  //(void) fixUpTile;
  return(XI_KERNEL_NAME (xiConvolvedAVQ3D_QM24_S8_DWH)(inTile, coeffTile, biasArray, outScaleArray, outShiftArray, outTile, param));
}

XI_ERR_TYPE xiConvolveAVQ3D_QM32_S8_DWH(const xi_pTile3D inTile,
                                         const xi_pTile4D coeffTile,
                                         const xi_pArray biasArray,
                                         const xi_pArray outScaleArray,
                                         const xi_pArray outShiftArray,
                                         const xi_pTile3D outTile,
                                         const xi_cnna_conv_params *param)
{
  //(void) fixUpTile;
  return(XI_KERNEL_NAME (xiConvolvedAVQ3D_QM32_S8_DWH)(inTile, coeffTile, biasArray, outScaleArray, outShiftArray, outTile, param));
}
void SetTileType(xi_pTile3D tile3DInpA, xi_pTile3D tile3DInpB,
                 xi_pTile3D tile3DOutpA, xi_pTile3D tile3DOutpB,
                 xi_pArray parrPtroffset, xi_pArray parr1DBias,
                 xi_pArray parr1DOutScale, xi_pArray parr1DOutShift, const conv_params_t *params)
{
  if (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkFC_FP16)
  {
    XI_TILE3D_SET_TYPE(tile3DInpA, XI_TILE3D_F16);
    XI_TILE3D_SET_TYPE(tile3DInpB, XI_TILE3D_F16);
    XI_TILE3D_SET_TYPE(tile3DOutpA, XI_TILE3D_F16);
    XI_TILE3D_SET_TYPE(tile3DOutpB, XI_TILE3D_F16);
    XI_ARRAY_SET_TYPE(parr1DBias, XI_ARRAY_F16);
    XI_ARRAY_SET_TYPE(parrPtroffset, XI_ARRAY_F16);
  }
  else if((CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkFC)&&(params->tileType.dataType == XI_TILE3D_S8))
  {
    XI_TILE3D_SET_TYPE(tile3DInpA, XI_TILE3D_S8);
    XI_TILE3D_SET_TYPE(tile3DInpB, XI_TILE3D_S8);
    XI_TILE3D_SET_TYPE(tile3DOutpA, XI_TILE3D_S8);
    XI_TILE3D_SET_TYPE(tile3DOutpB, XI_TILE3D_S8);
    XI_ARRAY_SET_TYPE(parr1DBias, XI_ARRAY_S32);
    XI_ARRAY_SET_TYPE(parrPtroffset, XI_ARRAY_S32);
  }
  else if (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkVQ_QM32
          || CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkVQ_QM24) {
    XI_TILE3D_SET_TYPE(tile3DInpA, XI_TILE3D_S8);
    XI_TILE3D_SET_TYPE(tile3DInpB, XI_TILE3D_S8);
    XI_TILE3D_SET_TYPE(tile3DOutpA, XI_TILE3D_S8);
    XI_TILE3D_SET_TYPE(tile3DOutpB, XI_TILE3D_S8);
    XI_ARRAY_SET_TYPE(parr1DBias, XI_ARRAY_S32);
    XI_ARRAY_SET_TYPE(parrPtroffset, XI_ARRAY_S8);

    XI_TILE3D_SET_TYPE(parr1DOutScale, XI_ARRAY_S32);
    XI_TILE3D_SET_TYPE(parr1DOutShift, XI_ARRAY_S8);
  }
  else
  {
    XI_TILE3D_SET_TYPE(tile3DInpA, XI_TILE3D_U8);
    XI_TILE3D_SET_TYPE(tile3DInpB, XI_TILE3D_U8);
    XI_TILE3D_SET_TYPE(tile3DOutpA, XI_TILE3D_U8);
    XI_TILE3D_SET_TYPE(tile3DOutpB, XI_TILE3D_U8);
    XI_ARRAY_SET_TYPE(parr1DBias, XI_ARRAY_S32);
    XI_ARRAY_SET_TYPE(parrPtroffset, XI_ARRAY_S32);
  }
}

/* Setup output tile based on coordinates and params. */
static void
setup_outp_tile(int D, int X, int Y, xi_pTile3D outp, const conv_params_t *params)
{
  /* Set output tile coordinates */
  XI_TILE3D_SET_DIM1_COORD(outp, D);
  XI_TILE3D_SET_DIM2_COORD(outp, X);
  XI_TILE3D_SET_DIM3_COORD(outp, Y);

  if (params->isVQ7optimize)
  {
    int Depth  = min(D + params->tile.D, params->output.D) - D;
    int Width  = min(X + params->tile.W, params->output.W) - X;
    int Height = min(Y + params->tile.H, params->output.H) - Y;

    /* Compute output tile dimensions from intersection of output tile size with output data */
    XI_TILE3D_SET_DIM1(outp, Width << 4);
    XI_TILE3D_SET_DIM2(outp, (Depth + 15) >> 4);
    XI_TILE3D_SET_DIM3(outp, Height);
  }
  else
  {
    XI_TILE3D_SET_DIM1(outp, min(D + params->tile.D, params->output.D) - D);
    XI_TILE3D_SET_DIM2(outp, min(X + params->tile.W, params->output.W) - X);
    XI_TILE3D_SET_DIM3(outp, min(Y + params->tile.H, params->output.H) - Y);
  }

  /* Update output tile pitch according to tile size */
  XI_TILE3D_SET_DIM1_PITCH(outp, XI_TILE3D_GET_DIM1(outp));
  XI_TILE3D_SET_DIM2_PITCH(outp, XI_TILE3D_GET_DIM2(outp) * XI_TILE3D_GET_DIM1(outp));
}

/* Setup input tile coordinates/dimensions based on output tile coordinates/dimensions and params. */
static void
setup_inp_tile(const xi_pTile3D outp, xi_pTile3D inp, const conv_params_t *params)
{
  BOOL_T kernel_F16flag = (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkFC_FP16) ? TRUE : FALSE;
  int X                 = XI_TILE3D_GET_DIM2_COORD(outp);
  int Y                 = XI_TILE3D_GET_DIM3_COORD(outp);

  /* Compute input tile coordinates */

  XI_TILE3D_SET_DIM2_COORD(inp, X * params->stride + params->offsetX);
  XI_TILE3D_SET_DIM3_COORD(inp, Y * params->stride + params->offsetY);

  if (params->isVQ7optimize)
  {
    /* Compute input tile dimensions */
    XI_TILE3D_SET_DIM1(inp, (((XI_TILE3D_GET_DIM1(outp) >> 4) - 1) * params->stride + 1) << 4);
    XI_TILE3D_SET_DIM2(inp, ((params->input.D + 15) >> 4));
    XI_TILE3D_SET_DIM3(inp, (XI_TILE3D_GET_DIM3(outp) - 1) * params->stride + 1);

    /* Update input tile pitch according to tile size */

    XI_TILE3D_SET_DIM1_PITCH(inp, XI_TILE3D_GET_DIM1(inp) + XI_TILE3D_GET_DIM1_EDGE1(inp) + XI_TILE3D_GET_DIM1_EDGE2(inp));
    XI_TILE3D_SET_DIM2_PITCH(inp, (XI_TILE3D_GET_DIM2(inp) + XI_TILE3D_GET_DIM2_EDGE1(inp) + XI_TILE3D_GET_DIM2_EDGE2(inp)) * XI_TILE3D_GET_DIM1_PITCH(inp));
    XI_TILE3D_SET_DATA_PTR(inp, (uint8_t *) XI_TILE3D_GET_BUFF_PTR(inp)
                           + ((XI_TILE3D_GET_DIM1_PITCH(inp) * XI_TILE3D_GET_DIM2_EDGE1(inp)))
                           + ((XI_TILE3D_GET_DIM2_PITCH(inp) * XI_TILE3D_GET_DIM3_EDGE1(inp))) + XI_TILE3D_GET_DIM1_EDGE1(inp));
  }
  else
  {
    XI_TILE3D_SET_DIM2(inp, (XI_TILE3D_GET_DIM2(outp) - 1) * params->stride + 1);
    XI_TILE3D_SET_DIM3(inp, (XI_TILE3D_GET_DIM3(outp) - 1) * params->stride + 1);
    XI_TILE3D_SET_DIM2_PITCH(inp, (XI_TILE3D_GET_DIM2(inp) + XI_TILE3D_GET_DIM2_EDGE1(inp) + XI_TILE3D_GET_DIM2_EDGE2(inp)) * XI_TILE3D_GET_DIM1_PITCH(inp));
    XI_TILE3D_SET_DATA_PTR(inp, (uint8_t *) XI_TILE3D_GET_BUFF_PTR(inp)
                           + ((XI_TILE3D_GET_DIM1_PITCH(inp) * XI_TILE3D_GET_DIM2_EDGE1(inp)) << kernel_F16flag)
                           + ((XI_TILE3D_GET_DIM2_PITCH(inp) * XI_TILE3D_GET_DIM3_EDGE1(inp)) << kernel_F16flag));
  }
}

/* Setup coefficients tile coordinates/dimensions based on output tile coordinates/dimensions and params.
   Returns tile size in bytes. */
static unsigned
setup_coeff_tile(const xi_pTile3D outp, xi_pTile4D coeff, const conv_params_t *params)
{
  unsigned D    = XI_TILE3D_GET_DIM1(outp);
  unsigned size = 0;
  unsigned temp;
  switch (CONV_FLAG_GET_KERNEL_KIND(params->flags))
  {
  case kkCa2:   /* Data order XI_NDWH with padding to multiple of 64 in N. */
    XI_TILE4D_SET_DIM1_PITCH(coeff, align_up(D, ALIGNMENT));
    XI_TILE4D_SET_DIM2_PITCH(coeff, XI_TILE4D_GET_DIM1_PITCH(coeff) * params->input.D);
    XI_TILE4D_SET_DIM3_PITCH(coeff, XI_TILE4D_GET_DIM1_PITCH(coeff) * params->input.D * params->kernelW);
    XI_TILE4D_SET_DIM1(coeff, D);
    XI_TILE4D_SET_DIM1_COORD(coeff, XI_TILE3D_GET_DIM1_COORD(outp));
    size = XI_TILE4D_GET_DIM3_PITCH(coeff) * params->kernelH;
    break;
  case kkVQ_QM24:
  case kkVQ_QM32:
    XI_TILE4D_SET_DIM1_PITCH(coeff, align_up(D, ALIGNMENT));
    XI_TILE4D_SET_DIM2_PITCH(coeff, XI_TILE4D_GET_DIM1_PITCH(coeff) * params->input.D);
    XI_TILE4D_SET_DIM3_PITCH(coeff, XI_TILE4D_GET_DIM1_PITCH(coeff) * params->input.D * params->kernelW);

    XI_TILE4D_SET_DIM1(coeff, D);
    XI_TILE4D_SET_DIM1_COORD(coeff, XI_TILE3D_GET_DIM1_COORD(outp));
#if 0
    XI_TILE4D_SET_DIM1_PITCH(coeff, XI_TILE4D_GET_DIM1(coeff));
    XI_TILE4D_SET_DIM2_PITCH(coeff, XI_TILE4D_GET_DIM2(coeff) * XI_TILE4D_GET_DIM1_PITCH(coeff));
    XI_TILE4D_SET_DIM3_PITCH(coeff, XI_TILE4D_GET_DIM3(coeff) * XI_TILE4D_GET_DIM2_PITCH(coeff));
#endif
    size = XI_TILE4D_GET_DIM4(coeff) * XI_TILE4D_GET_DIM3_PITCH(coeff);
    break;
  case kkVQ7Opt:
    D    = XI_TILE3D_GET_DIM2(outp) << 4;
    temp = align_up(params->kernelW * (((params->input.D + 15) & (~15)) << 4), QM32_ROW_PADDING);
    XI_TILE4D_SET_DIM4(coeff, (D + 15) >> 4);
    XI_TILE4D_SET_DIM4_COORD(coeff, XI_TILE3D_GET_DIM1_COORD(outp));
    size = ((params->tile.D + 15) >> 4) * align_up(temp * params->kernelH, QM32_FILTER_PADDING);
    break;
  case kkQM24:
  case kkQM32:   /* Data order XI_NDWH. */
    if (!params->isVQ7optimize)
    {
      temp = align_up(params->kernelW * params->input.D, QM32_ROW_PADDING);
      XI_TILE4D_SET_DIM1_PITCH(coeff, D);
      XI_TILE4D_SET_DIM2_PITCH(coeff, XI_TILE4D_GET_DIM1_PITCH(coeff) * params->input.D);
      XI_TILE4D_SET_DIM3_PITCH(coeff, XI_TILE4D_GET_DIM1_PITCH(coeff) * temp);
      XI_TILE4D_SET_DIM1(coeff, D);
      XI_TILE4D_SET_DIM1_COORD(coeff, XI_TILE3D_GET_DIM1_COORD(outp));
      size = D * align_up(temp * params->kernelH, QM32_FILTER_PADDING);
    }
    else
    {
      D    = XI_TILE3D_GET_DIM2(outp) << 4;
      temp = align_up(params->kernelW * (((params->input.D + 15) & (~15)) << 4), QM32_ROW_PADDING);
      XI_TILE4D_SET_DIM4(coeff, (D + 15) >> 4);
      XI_TILE4D_SET_DIM4_COORD(coeff, XI_TILE3D_GET_DIM1_COORD(outp));
      size = ((params->tile.D + 15) >> 4) * align_up(temp * params->kernelH, QM32_FILTER_PADDING);
    }
    break;
  case kkInvQM24:   /* Data order XI DWHN */
    XI_TILE4D_SET_DIM1_PITCH(coeff, params->input.D);
    XI_TILE4D_SET_DIM2_PITCH(coeff, params->kernelW * XI_TILE4D_GET_DIM1_PITCH(coeff));
    XI_TILE4D_SET_DIM3_PITCH(coeff, params->kernelH * XI_TILE4D_GET_DIM2_PITCH(coeff));
    XI_TILE4D_SET_DIM4(coeff, D);
    size = D * XI_TILE4D_GET_DIM3_PITCH(coeff);
    break;
  case kkGS_MOW:   /* Data order XI_WHDN. */
    XI_TILE4D_SET_DIM4(coeff, D);
    XI_TILE4D_SET_DIM4_COORD(coeff, XI_TILE3D_GET_DIM1_COORD(outp));
    size = D * XI_TILE4D_GET_DIM3_PITCH(coeff);
    break;
  case kkGS_MOW1x1:   /* Data order XI_WHDN. */
    XI_TILE4D_SET_DIM4(coeff, D);
    XI_TILE4D_SET_DIM4_COORD(coeff, XI_TILE3D_GET_DIM1_COORD(outp));
    size = D * XI_TILE4D_GET_DIM3_PITCH(coeff);
    break;
  case kkGS_MOW_S2:       /* Data order XI_WHDN. */
    XI_TILE4D_SET_DIM4(coeff, D);
    XI_TILE4D_SET_DIM4_COORD(coeff, XI_TILE3D_GET_DIM1_COORD(outp));
    size = D * XI_TILE4D_GET_DIM3_PITCH(coeff);
    break;
  case kkFC:   /* Data order XI_DWHN, contiguous, no padding. */
    XI_TILE4D_SET_DIM4(coeff, D);
    size = D * XI_TILE4D_GET_DIM3_PITCH(coeff);
    break;
#if (XCHAL_HAVE_VISION_HP_VFPU == 1)
  case kkFC_FP16:
    XI_TILE4D_SET_DIM4(coeff, D);
    size = D * XI_TILE4D_GET_DIM3_PITCH(coeff) * 2;
    break;
#endif
  default:
    break;
  }
  return(size);
}

/* For input tile at (X, Y) of (W, H) with edges find intersection with input data and amount of padding needed.
   Schedules DMA transfer to bring valid data into tile.
   Sets tile status to non-zero if zero padding is needed. */
static void
transfer_input_tile(uint8_t *inputPtr, xi_pTile3D inp, const conv_params_t *params)
{
  if (params->isVQ7optimize)
  {
    int boundX = XI_TILE3D_GET_DIM2_COORD(inp) - (XI_TILE3D_GET_DIM1_EDGE1(inp) >> 4);
    int boundY = XI_TILE3D_GET_DIM3_COORD(inp) - XI_TILE3D_GET_DIM3_EDGE1(inp);
    int boundW = XI_TILE3D_GET_DIM2_COORD(inp) + (XI_TILE3D_GET_DIM1(inp) >> 4) + (XI_TILE3D_GET_DIM1_EDGE2(inp) >> 4);
    int boundH = XI_TILE3D_GET_DIM3_COORD(inp) + XI_TILE3D_GET_DIM3(inp) + XI_TILE3D_GET_DIM3_EDGE2(inp);

    int validX = max(0, boundX);
    int validY = max(0, boundY);
    int validW = min(boundW, params->input.W) - validX;
    int validH = min(boundH, params->input.H) - validY;


    //printf("valuid X Y %d %d and Valid W H %d %d \n", validX,validY,validW,validH);
    int pad_left   = validX - boundX;
    int pad_top    = validY - boundY;
    int pad_right  = boundW - validX - validW;
    int pad_bottom = boundH - validY - validH;


    int D = XI_TILE3D_GET_DIM2(inp) << 4;
    int W = XI_TILE3D_GET_DIM1(inp) >> 4;
    int H = XI_TILE3D_GET_DIM3(inp);

    int Wedge1 = XI_TILE3D_GET_DIM1_EDGE1(inp) >> 4;
    int Wedge2 = XI_TILE3D_GET_DIM1_EDGE2(inp) >> 4;
    int TotalW = W + Wedge1 + Wedge2;
    int Hedge1 = XI_TILE3D_GET_DIM3_EDGE1(inp);
    int Hedge2 = XI_TILE3D_GET_DIM3_EDGE2(inp);
    int TotalH = H + Hedge1 + Hedge2;

    //printf("padding value %d %d %d %d and TOtalw%d abnd TotalH %d \n", pad_left,pad_top,pad_right,pad_bottom,TotalW,TotalH);
    int inputDmul16   = (params->input.D / 16) * 16;
    int validData     = params->input.D - inputDmul16;
    int remainigZeros = 16 - validData;
    uint8_t *outPtr   = (uint8_t *) XI_TILE3D_GET_BUFF_PTR(inp);


        #if 01
    dma_4d_sys2loc_straddles(inputPtr + ((((validY * params->input.W) + validX) * params->input.D)), outPtr + ((pad_top * TotalW * D) + (pad_left * 16)),
                             16,
                             params->input.D /*d2_src_pitch*/, 16 /*d2_dst_pitch*/, validW /*d2_cnt*/,
                             (16) /*d3_src_pitch*/, TotalW * 16 /*d3_dst_pitch*/, (((D + 15) / 16) * 16) / 16 /*d3_cnt*/,
                             params->input.D * params->input.W /*d4_src_pitch*/, TotalW * (((D + 15) / 16) * 16) /*d4_dst_pitch*/, validH /*d4_cnt*/);
        #endif

    if ((pad_left | pad_top | pad_right | pad_bottom) != 0)
    {
#if (defined(IVP_DMULQA2N8QXR8))
      xi_size3D frame_size_tile3DInpDWH;
      frame_size_tile3DInpDWH.dim1Size = ((params->input.D + 15) >> 4) << 4;
      frame_size_tile3DInpDWH.dim2Size = (params->input.W);
      frame_size_tile3DInpDWH.dim3Size = params->input.H;
      (xiextendEdges3DA_ID16WH(inp, params->zeroPtInput, frame_size_tile3DInpDWH));
      //(xiExtendEdgesConst3D_I8(inp, params->zeroPtInput, frame_size_tile3DInpDWH));
#endif
    }

        #if 01
    if (validData)
    {
      dma_3d_sys2loc_straddles(/* src */ inputZeros,
                                         /* dst */ outPtr + ((inputDmul16) * (TotalW)) + validData,
                                         /* row size */ remainigZeros,
                                         /* src stride 2d */ 0,
                                         /* dst stride 2d */ 16,
                                         /* count 2d */ TotalW,
                                         /* src stride 3d */ 0,
                                         /* dst stride 3d */ TotalW * (((D + 15) / 16) * 16),
                                         /* count 3d */ TotalH);
    }
        #endif
  }
  else
  {
    BOOL_T kernel_F16flag = (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkFC_FP16) ? TRUE : FALSE;
    int boundX            = XI_TILE3D_GET_DIM2_COORD(inp) - XI_TILE3D_GET_DIM2_EDGE1(inp);
    int boundY            = XI_TILE3D_GET_DIM3_COORD(inp) - XI_TILE3D_GET_DIM3_EDGE1(inp);
    int boundW            = XI_TILE3D_GET_DIM2_COORD(inp) + XI_TILE3D_GET_DIM2(inp) + XI_TILE3D_GET_DIM2_EDGE2(inp);
    int boundH            = XI_TILE3D_GET_DIM3_COORD(inp) + XI_TILE3D_GET_DIM3(inp) + XI_TILE3D_GET_DIM3_EDGE2(inp);

    int validX = max(0, boundX);
    int validY = max(0, boundY);
    int validW = min(boundW, params->input.W) - validX;
    int validH = min(boundH, params->input.H) - validY;


    int pad_left   = validX - boundX;
    int pad_top    = validY - boundY;
    int pad_right  = boundW - validX - validW;
    int pad_bottom = boundH - validY - validH;

    /* Indicate that tile needs edge extension function called */
    XI_TILE3D_SET_STATUS_FLAGS(inp, (pad_left | pad_top | pad_right | pad_bottom) != 0);

    /* Don't issue DMA request if tile has no overlap with valid data */
    if (validW <= 0 || validH <= 0)
    {
      return;
    }

    uint8_t *tilePtr = (uint8_t *) XI_TILE3D_GET_BUFF_PTR(inp);
    dma_3d_sys2loc_dyn_straddles(/* src */ inputPtr + (((validY * params->input.W + validX) * params->input.D) << kernel_F16flag),
                                           /* dst */ tilePtr + ((pad_top * XI_TILE3D_GET_DIM2_PITCH(inp) + pad_left * XI_TILE3D_GET_DIM1_PITCH(inp)) << kernel_F16flag),                               //pad is 0 for FC_FP16
                                           /* row size */ (params->input.D) << kernel_F16flag,
                                           /* src stride 2d */ (params->input.D) << kernel_F16flag,
                                           /* dst stride 2d */ (XI_TILE3D_GET_DIM1_PITCH(inp)) << kernel_F16flag,
                                           /* count 2d */ validW,
                                           /* src stride 3d */ (params->input.D * params->input.W) << kernel_F16flag,
                                           /* dst stride 3d */ (XI_TILE3D_GET_DIM2_PITCH(inp)) << kernel_F16flag,
                                           /* count 3d */ validH);
  }
}

/* Schedule output tile DMA transfer */
static void
transfer_output_tile(uint8_t *outputPtr, xi_pTile3D outp, const conv_params_t *params)
{
  if (params->isVQ7optimize)
  {
    int D = XI_TILE3D_GET_DIM2(outp) << 4;
    int W = XI_TILE3D_GET_DIM1(outp) >> 4;
    int H = XI_TILE3D_GET_DIM3(outp);

    //printf("m here output %d %d %d \n", XI_TILE3D_GET_DIM3_COORD(outp),XI_TILE3D_GET_DIM2_COORD(outp),XI_TILE3D_GET_DIM1_COORD(outp));
    int validoutDepth = min(D, min(XI_TILE3D_GET_DIM1_COORD(outp) + params->tile.D, params->output.D) - XI_TILE3D_GET_DIM1_COORD(outp));

    int remainigZeros = validoutDepth - ((validoutDepth >> 4) << 4);

    uint8_t *inpPtr = (uint8_t *) XI_TILE3D_GET_BUFF_PTR(outp);


    dma_4d_loc2sys_straddles(inpPtr, outputPtr + (((XI_TILE3D_GET_DIM3_COORD(outp) * params->output.D * params->output.W) + (XI_TILE3D_GET_DIM2_COORD(outp) * params->output.D) + XI_TILE3D_GET_DIM1_COORD(outp))),
                             min(16, (validoutDepth / 16) * 16),
                             16 /*d2_src_pitch*/, params->output.D /*d2_dst_pitch*/, W /*d2_cnt*/,
                             W * 16 /*d3_src_pitch*/, (16) /*d3_dst_pitch*/, (validoutDepth / 16) /*d3_cnt*/,
                             W * (((D + 15) / 16) * 16) /*d4_src_pitch*/, params->output.W * params->output.D /*d4_dst_pitch*/, H /*d4_cnt*/);


    if (remainigZeros)
    {
      dma_4d_loc2sys_straddles(inpPtr + ((validoutDepth / 16) * 16 * W), outputPtr + ((validoutDepth / 16) * 16) + (((XI_TILE3D_GET_DIM3_COORD(outp) * params->output.D * params->output.W) + (XI_TILE3D_GET_DIM2_COORD(outp) * params->output.D) + XI_TILE3D_GET_DIM1_COORD(outp))),
                               remainigZeros,
                               16 /*d2_src_pitch*/, params->output.D /*d2_dst_pitch*/, W /*d2_cnt*/,
                               W * 16 /*d3_src_pitch*/, (16) /*d3_dst_pitch*/, 01 /*d3_cnt*/,
                               W * (((D + 15) / 16) * 16) /*d4_src_pitch*/, params->output.W * params->output.D /*d4_dst_pitch*/, H /*d4_cnt*/);
    }
  }
  else
  {
    BOOL_T kernel_F16flag = (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkFC_FP16) ? TRUE : FALSE;
    int D                 = XI_TILE3D_GET_DIM1_COORD(outp);
    int X                 = XI_TILE3D_GET_DIM2_COORD(outp);
    int Y                 = XI_TILE3D_GET_DIM3_COORD(outp);

    uint8_t *tilePtr = (uint8_t *) XI_TILE3D_GET_BUFF_PTR(outp);

    dma_3d_loc2sys_dyn_straddles(/* src */ tilePtr,
                                           /* dst */ outputPtr + ((Y * params->output.D * params->output.W + X * params->output.D + D) << kernel_F16flag),
                                           /* row size */ (XI_TILE3D_GET_DIM1(outp)) << kernel_F16flag,
                                           /* src stride 2d */ (XI_TILE3D_GET_DIM1_PITCH(outp)) << kernel_F16flag,
                                           /* dst stride 2d */ (params->output.D) << kernel_F16flag,
                                           /* count 2d */ XI_TILE3D_GET_DIM2(outp),
                                           /* src stride 3d */ (XI_TILE3D_GET_DIM2_PITCH(outp)) << kernel_F16flag,
                                           /* dst stride 3d */ (params->output.D * params->output.W) << kernel_F16flag,
                                           /* count 3d */ XI_TILE3D_GET_DIM3(outp));
  }
}

XI_ERR_TYPE kernel_func(const xi_pTile3D inTile,
                        const xi_pArray inPtrOffsetArr,
                        const xi_pTile4D coeffTile,
                        const xi_pArray biasArray,
                        const xi_pTile fixUpTile,
                        const xi_pArray outScaleArray,
                        const xi_pArray outShiftArray,
                        xi_pTile3D outTile,
                        const conv_params_t *params,
                        const xi_cnna_conv_params *xi_params,
                        xiConvolvedA3D_f func_ptr,
                        xiConvolvedAVQ3D_f func_vq_ptr,
                        xiConvolvedA3D_VQ7_QM_f VQ7_QM_func_ptr)
{
  XI_ERR_TYPE status;
  if (params->isVQ7optimize)
  {
    status = VQ7_QM_func_ptr(inTile, inPtrOffsetArr, coeffTile, biasArray, fixUpTile, outTile, (xi_cnna_conv_params *) xi_params);
    //XI_CHECK_RESULT(VQ7_QM_func_ptr(inTile, inPtrOffsetArr, coeffTile, biasArray, fixUpTile, outTile, (xi_cnna_conv_params *) xi_params));
  }
  else if (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkVQ_QM24
          || CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkVQ_QM32)
  {
    (void)inPtrOffsetArr;
    (void)fixUpTile;
    status = func_vq_ptr(inTile, coeffTile, biasArray, outScaleArray, outShiftArray, outTile, (xi_cnna_conv_params *) xi_params);
    //XI_CHECK_RESULT(func_vq_ptr(inTile, coeffTile, biasArray, outScaleArray, outShiftArray, outTile, (xi_cnna_conv_params *) xi_params));
  }
  else
  {
    status = func_ptr(inTile, fixUpTile, coeffTile, biasArray, outTile, (xi_cnna_conv_params *) xi_params);
    //XI_CHECK_RESULT(func_ptr(inTile, fixUpTile, coeffTile, biasArray, outTile, (xi_cnna_conv_params *) xi_params));
  }
  return(status);
}

XI_ERR_TYPE flk_conv(const uint8_t *raw_params,
                     struct XtensaOperationArgsIn *input,
                     struct XtensaOperationArgsOut *output)
{
  const conv_params_t *params = (const conv_params_t *) raw_params;

  XI_ERROR_CHECKS()
  {
    XI_RUN_TIME_CHECK(params != NULL,
                      "Params can be NULL", XI_ERR_BADARG);
    XI_RUN_TIME_CHECK(params->structSize == sizeof(*params),
                      "Params structure size is incorrect", XI_ERR_BADARG);
    XI_RUN_TIME_CHECK(input != NULL
                      && ((input->numArgs == 3) || (input->numArgs == 4) || (input->numArgs == 5)),
                      "Invalid number of input args", XI_ERR_BADARG);
    XI_RUN_TIME_CHECK(input->args[0] != NULL && input->argsSize[0] > 0
                      && input->args[1] != NULL && input->argsSize[1] > 0
                      && input->args[2] != NULL && input->argsSize[2] > 0,
                      "Invalid input args", XI_ERR_BADARG);
    if (input->numArgs == 4)
    {
      XI_RUN_TIME_CHECK(input->args[3] != NULL && input->argsSize[3] > 0,
                        "Invalid last input arg", XI_ERR_BADARG);
    }
    else if (input->numArgs == 5)
    {
      XI_RUN_TIME_CHECK(input->args[3] != NULL && input->argsSize[3] > 0,
                        "Invalid last input arg", XI_ERR_BADARG);
      XI_RUN_TIME_CHECK(input->args[4] != NULL && input->argsSize[4] > 0,
                        "Invalid last input arg", XI_ERR_BADARG);
    }
    XI_RUN_TIME_CHECK(output != NULL
                      && output->numArgs == 1
                      && output->args[0] != NULL && output->argsSize[0] > 0,
                      "Invalid output args", XI_ERR_BADARG);
    XI_RUN_TIME_CHECK(params->batch >= 1,
                      "Invalid batch size", XI_ERR_BADARG);
    XI_RUN_TIME_CHECK(params->stride >= 1,
                      "Invalid stride", XI_ERR_BADARG);
    XI_RUN_TIME_CHECK(params->output.D >= 1
                      && params->output.W >= 1
                      && params->output.H >= 1
                      && params->output.D * params->output.W * params->output.H * params->batch <= output->argsSize[0],
                      "Inconsistent output dimensions", XI_ERR_BADARG);
    XI_RUN_TIME_CHECK(params->input.D >= 1
                      && params->input.W >= 1
                      && params->input.H >= 1
                      && params->input.D * params->input.W * params->input.H * params->batch <= input->argsSize[0],
                      "Inconsistent input dimensions", XI_ERR_BADARG);
    XI_RUN_TIME_CHECK(params->tile.D >= 1 && params->output.D >= params->tile.D
                      && params->tile.W >= 1 && params->output.W >= params->tile.W
                      && params->tile.H >= 1 && params->output.H >= params->tile.H,
                      "Inconsistent output tile dimensions", XI_ERR_BADARG);
    XI_RUN_TIME_CHECK(params->kernelW >= 1 && params->kernelH >= 1,
                      "Inconsistent kernel dimensions", XI_ERR_BADARG);
    //XI_RUN_TIME_CHECK(params->output.D * params->kernelW * params->kernelH * params->input.D <= input->argsSize[1],
    //                  "Inconsistent coefficients array size", XI_ERR_BADARG);
    int sizeBias = sizeof(int32_t);
    (void) sizeBias;
#if (XCHAL_HAVE_VISION_HP_VFPU == 1)
    if (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkFC_FP16)
    {
      sizeBias = 2;       // sizeof(fp16) is equal to 2
    }
#endif
    XI_RUN_TIME_CHECK(params->output.D * sizeBias <= input->argsSize[2],
                      "Inconsistent bias array size", XI_ERR_BADARG);
  };

  conv_mem_info_t mem_info;
  int FC_FP16_flag;
  FC_FP16_flag = 0; // default value

  size_t total_size = FillMemInfoConv2D(params, &mem_info);
  (void) total_size;
  XI_RUN_TIME_CHECK(total_size > 0, "Inconsistent convolution parameters", XI_ERR_BADARG);

  uint8_t* buffOffset = NULL;
  uint8_t* buffBias = NULL;
  uint8_t* buffCoeffA = NULL;
  uint8_t* buffCoeffB = NULL;
  uint8_t* buffInputA = NULL;
  uint8_t* buffInputB = NULL;
  uint8_t* buffOutputA = NULL;
  uint8_t* buffOutputB    = NULL;
  uint8_t* buffFixup      = NULL;
  uint8_t* buffPreluSlope = NULL;
  uint8_t* buffOutScale   = NULL;
  uint8_t* buffOutShift   = NULL;

  struct
  {
    xi_tile3D tiles3D[4];
    xi_tile4D tiles4D[2];
    xi_tile   fixupTile;
    xi_tile3D preluSlope;
  } structs;

  xi_pTile3D tile3DInpA, tile3DInpB, temp3D;
  xi_pTile3D tile3DOutpA, tile3DOutpB;
  xi_pTile4D tile4DCoeffA, tile4DCoeffB, temp4D;
  xi_pTile3D tile3DPreluSlope;

  xi_size3D frame_size_tile3DInp;

  xi_array arr1DBias, arrPtrOffset;
  xi_array arr1DOutScale, arr1DOutShift;

  xiConvolvedA3D_f kernel = NULL;
  xiConvolvedAVQ3D_f kernelVQ = NULL;
  xiConvolvedA3D_VQ7_QM_f kernel_QMopt = NULL;
  xiConvolvedFixupA3D_f fixup = NULL;
#if KERNEL_CYCLES
  int fixupCy = 0;
#endif

  xi_cnna_conv_params xiparams;
  uint8_t *func_params_ptr = (uint8_t *) &xiparams;
  xv_memset((int16_t*) &xiparams, 0, sizeof(xiparams));
#if (XCHAL_HAVE_VISION_HP_VFPU == 1)
  xi_cnna_conv_F16_params xiparams_F16;
  xv_memset((int16_t*) &xiparams_F16, 0, sizeof(xiparams_F16));
#endif

  int needFixup = CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkCa2
                  || CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkGS_MOW
                  || CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkGS_MOW1x1
                  || CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkGS_MOW_S2;
  int needFixupTile = needFixup
                      || CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkQM24
                      || CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkInvQM24
                      || CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkQM32;

  /* Double buffer if more than one tile */
  int doubleBuffInput  = params->batch != 1 || mem_info.numTilesW != 1 || mem_info.numTilesH != 1;
  int doubleBuffCoeff  = (mem_info.numTilesD > 1) && CONV_FLAG_GET_DOUBLE_BUFF_COEFF(params->flags);
  int doubleBuffOutput = doubleBuffInput || (mem_info.numTilesD > 1);
  /* Reserve memory */
  switch (params->banks_info.banksMode)
  {
  case 0:   //  single bank
    XI_CHECK_RESULT(arena_init_one_bank(/* size */ mem_info.totalSize, /* max_alignment */ ALIGNMENT));
    break;
  case 1:   //  contiguous banks
    XI_CHECK_RESULT(arena_init_two_banks_contiguous(/* size */ mem_info.totalSize, /* max_alignment */ ALIGNMENT));
    break;
  case 2:   //  split banks
    XI_CHECK_RESULT(arena_init_two_banks_split(/* size0 */ params->banks_info.bank0total, /* max_alignment */ ALIGNMENT, /* size1 */ params->banks_info.bank1total, /* max_alignment */ ALIGNMENT));
    break;
  }

#if DEBUG_LEVEL_CONV > 1
  unsigned int dram0Total = 0, dram1Total = 0;
  XI_CHECK_RESULT(arena_bank_free_space_debug(0, &dram0Total));
  if (params->banks_info.banksMode != 0)
  {
    XI_CHECK_RESULT(arena_bank_free_space_debug(1, &dram1Total));
  }
#endif
  /* Allocate buffers */
  arena_alloc((void * *) (&buffCoeffA), /* bank */ (params->banks_info.bankAssignments & (1 << 5) ? 1 : 0), /* size */ mem_info.coeffTileSize, /* alignment */ ALIGNMENT);
  if (buffCoeffA == NULL)
   (arena_alloc((void * *) (&buffCoeffA), /* bank */ (params->banks_info.bankAssignments & (1 << 5) ? 0 : 1), /* size */ mem_info.coeffTileSize, /* alignment */ ALIGNMENT));
  if (doubleBuffCoeff)
  {
    XI_CHECK_RESULT(arena_alloc((void * *) (&buffCoeffB), /* bank */ (params->banks_info.bankAssignments & (1 << 6) ? 1 : 0), /* size */ mem_info.coeffTileSize, /* alignment */ ALIGNMENT));
  }
  else
  {
    buffCoeffB = buffCoeffA;
  }
  if(!buffCoeffA) printf("alloc failure in conv2D\n");

  arena_alloc((void * *) (&buffBias), /* bank */ (params->banks_info.bankAssignments & (1 << 0) ? 1 : 0), /* size */ mem_info.biasTileSize, /* alignment */ ALIGNMENT);
  if (buffBias == NULL)
   (arena_alloc((void * *) (&buffBias), /* bank */ (params->banks_info.bankAssignments & (1 << 0) ? 0 : 1), /* size */ mem_info.biasTileSize, /* alignment */ ALIGNMENT));
  if(!buffBias) printf("alloc failure in conv2D\n");

  arena_alloc((void * *) (&buffInputA), /* bank */ (params->banks_info.bankAssignments & (1 << 1) ? 1 : 0), /* size */ mem_info.inpTileSize, /* alignment */ ALIGNMENT);
  if (buffInputA == NULL)
    (arena_alloc((void * *) (&buffInputA), /* bank */ (params->banks_info.bankAssignments & (1 << 1) ? 0 : 1), /* size */ mem_info.inpTileSize, /* alignment */ ALIGNMENT));
  if (doubleBuffInput)
  {
    XI_CHECK_RESULT(arena_alloc((void * *) (&buffInputB), /* bank */ (params->banks_info.bankAssignments & (1 << 2) ? 1 : 0), /* size */ mem_info.inpTileSize, /* alignment */ ALIGNMENT));
  }
  if(!buffInputA) printf("alloc failure in conv2D\n");

  arena_alloc((void * *) (&buffOutputA), /* bank */ (params->banks_info.bankAssignments & (1 << 3) ? 1 : 0), /* size */ mem_info.outpTileSize, /* alignment */ ALIGNMENT);
  if (buffOutputA == NULL)
   (arena_alloc((void * *) (&buffOutputA), /* bank */ (params->banks_info.bankAssignments & (1 << 3) ? 0 : 1), /* size */ mem_info.outpTileSize, /* alignment */ ALIGNMENT));
  if (doubleBuffOutput)
  {
    XI_CHECK_RESULT(arena_alloc((void * *) (&buffOutputB), /* bank */ (params->banks_info.bankAssignments & (1 << 4) ? 1 : 0), /* size */ mem_info.outpTileSize, /* alignment */ ALIGNMENT));
  }
  if(!buffOutputA) printf("alloc failure in conv2D\n");

  if (needFixupTile)
  {
    arena_alloc((void * *) (&buffFixup), /* bank */ (params->banks_info.bankAssignments & (1 << 7) ? 1 : 0), /* size */ mem_info.fixupTileSize, /* alignment */ ALIGNMENT);
    if (buffFixup == NULL)
     (arena_alloc((void * *) (&buffFixup), /* bank */ (params->banks_info.bankAssignments & (1 << 7) ? 0 : 1), /* size */ mem_info.fixupTileSize, /* alignment */ ALIGNMENT));
    if(!buffFixup) printf("alloc failure in conv2D\n");
  }
  if (CONV_FLAG_GET_PRELU(params->flags))
  {
    arena_alloc((void * *) (&buffPreluSlope), /* bank */ (params->banks_info.bankAssignments & (1 << 8) ? 1 : 0), /* size */ mem_info.preluTileSize, /* alignment */ ALIGNMENT);
    if (buffPreluSlope == NULL)
     (arena_alloc((void * *) (&buffPreluSlope), /* bank */ (params->banks_info.bankAssignments & (1 << 8) ? 0 : 1), /* size */ mem_info.preluTileSize, /* alignment */ ALIGNMENT));
    if(!buffPreluSlope) printf("alloc failure in conv2D\n");
  }
  if (mem_info.offsetTileSize)
  {
    arena_alloc((void * *) (&buffOffset), /* bank */ (params->banks_info.bankAssignments & (1 << 9) ? 1 : 0), /* size */ mem_info.offsetTileSize /*((params->input.D + 15) >> 4) * params->kernelW * params->kernelH*sizeof(int32_t)*/, /* alignment */ ALIGNMENT);
    if (buffOffset == NULL)
     (arena_alloc((void * *) (&buffOffset), /* bank */ (params->banks_info.bankAssignments & (1 << 9) ? 0 : 1), /* size */ mem_info.offsetTileSize /*((params->input.D + 15) >> 4) * params->kernelW * params->kernelH*sizeof(int32_t)*/, /* alignment */ ALIGNMENT));
    if(!buffOffset) printf("alloc failure in conv2D\n");
  }
  if (mem_info.outScaleSize)
  {
    arena_alloc((void * *) (&buffOutScale), /* bank */ (params->banks_info.bankAssignments & (1 << 10) ? 1 : 0), /* size */ mem_info.outScaleSize, /* alignment */ ALIGNMENT);
    if (buffOutScale == NULL)
     (arena_alloc((void * *) (&buffOutScale), /* bank */ (params->banks_info.bankAssignments & (1 << 10) ? 0 : 1), /* size */ mem_info.outScaleSize, /* alignment */ ALIGNMENT));
    if(!buffOutScale) printf("alloc failure in conv2D\n");
  }
  if (mem_info.outShiftSize)
  {
    arena_alloc((void * *) (&buffOutShift), /* bank */ (params->banks_info.bankAssignments & (1 << 11) ? 1 : 0), /* size */ mem_info.outShiftSize, /* alignment */ ALIGNMENT);
    if (buffOutShift == NULL)
     (arena_alloc((void * *) (&buffOutShift), /* bank */ (params->banks_info.bankAssignments & (1 << 11) ? 0 : 1), /* size */ mem_info.outShiftSize, /* alignment */ ALIGNMENT));
    if(!buffOutShift) printf("alloc failure in conv2D\n");
  }


#if DEBUG_LEVEL_CONV > 1
  print_conv_params("CONV_2D", raw_params);
  print_conv_mem_info(&mem_info);
  printf("#   doubleBuff: I: %d, O: %d, C: %d\n", doubleBuffInput, doubleBuffOutput, doubleBuffCoeff);
  unsigned int dram0Free = 0, dram1Free = 0;
  XI_CHECK_RESULT(arena_bank_free_space_debug(0, &dram0Free));
  printf("#   dram0Free: %d\n", dram0Free);
  printf("#   dram0Total: %d\n", dram0Total);
  if (params->banks_info.banksMode != 0)
  {
    XI_CHECK_RESULT(arena_bank_free_space_debug(1, &dram1Free));
    printf("#   dram1Free: %d\n", dram1Free);
    printf("#   dram1Total: %d\n", dram1Total);
  }
  else
  {
    printf("#   dram1Free: -NA-\n");
    printf("#   dram1Total: -NA-\n");
  }
#endif

  uint8_t *inputPtr  = (uint8_t *) input->args[0];
  uint8_t *coeffPtr  = (uint8_t *) input->args[1];
  uint8_t *biasPtr   = (uint8_t *) input->args[2];
  uint8_t *outputPtr = (uint8_t *) output->args[0];
  int32_t *outScalePtr;
  int8_t *outShiftPtr;
  uint8_t *preluPtr;
  if (CONV_FLAG_GET_PRELU(params->flags) || CONV_FLAG_GET_LEAKY_PRELU(params->flags))
  {
    preluPtr = (uint8_t *) input->args[3];
  }
  // VQ version of convolution
  if (input->numArgs == 5) {
    outScalePtr = (int32_t *)input->args[3];
    outShiftPtr = (int8_t *)input->args[4];
    dma_1d_sys2loc_straddles(/* src */ (uint8_t *) outScalePtr, /* dst */ buffOutScale, /* row size */ mem_info.outScaleSize);
    dma_1d_sys2loc_straddles(/* src */ (uint8_t *) outShiftPtr, /* dst */ buffOutShift, /* row size */ mem_info.outShiftSize);
  }
  dma_1d_sys2loc_straddles(/* src */ biasPtr, /* dst */ buffBias, /* row size */ mem_info.biasTileSize);

  /* Input tile edges */
  unsigned edge_left   = params->kernelW / 2;
  unsigned edge_top    = params->kernelH / 2;
  unsigned edge_right  = params->kernelW - edge_left - 1;
  unsigned edge_bottom = params->kernelH - edge_top - 1;

  xv_memset((int16_t*)&structs, 0, sizeof(structs));

  tile3DInpA       = &structs.tiles3D[0];
  tile3DInpB       = &structs.tiles3D[1];
  tile3DOutpA      = &structs.tiles3D[2];
  tile3DOutpB      = &structs.tiles3D[3];
  tile4DCoeffA     = &structs.tiles4D[0];
  tile4DCoeffB     = &structs.tiles4D[1];
  tile3DPreluSlope = &structs.preluSlope;

  //arr1DOutScale    = &structs.arr1DOutScale;
  //arr1DOutShift    = &structs.arr1DOutShift;

  XI_TILE4D_SET_BUFF_SIZE(tile4DCoeffA, mem_info.coeffTileSize);
  XI_TILE4D_SET_BUFF_PTR(tile4DCoeffA, buffCoeffA);
  XI_TILE4D_SET_DATA_PTR(tile4DCoeffA, buffCoeffA);

  XI_TILE4D_SET_BUFF_SIZE(tile4DCoeffB, mem_info.coeffTileSize);
  XI_TILE4D_SET_BUFF_PTR(tile4DCoeffB, buffCoeffB);
  XI_TILE4D_SET_DATA_PTR(tile4DCoeffB, buffCoeffB);

  switch (CONV_FLAG_GET_KERNEL_KIND(params->flags))
  {
#if 0
  case kkGS_MOW1x1:
    XI_TILE4D_SET_DATA_ORDER(tile4DCoeffA, XI_WHDN);
    XI_TILE4D_SET_DIM1(tile4DCoeffA, params->kernelW);
    XI_TILE4D_SET_DIM2(tile4DCoeffA, params->kernelH);
    XI_TILE4D_SET_DIM3(tile4DCoeffA, params->input.D);
    XI_TILE4D_SET_DIM1_PITCH(tile4DCoeffA, params->kernelW);
    XI_TILE4D_SET_DIM2_PITCH(tile4DCoeffA, params->kernelW * params->kernelH);
    XI_TILE4D_SET_DIM3_PITCH(tile4DCoeffA, params->kernelW * params->kernelH * params->input.D);
    XI_TILE4D_SET_TYPE(tile4DCoeffA, XI_TILE4D_S8);

    XI_TILE4D_SET_DATA_ORDER(tile4DCoeffB, XI_WHDN);
    XI_TILE4D_SET_DIM1(tile4DCoeffB, params->kernelW);
    XI_TILE4D_SET_DIM2(tile4DCoeffB, params->kernelH);
    XI_TILE4D_SET_DIM3(tile4DCoeffB, params->input.D);
    XI_TILE4D_SET_DIM1_PITCH(tile4DCoeffB, params->kernelW);
    XI_TILE4D_SET_DIM2_PITCH(tile4DCoeffB, params->kernelW * params->kernelH);
    XI_TILE4D_SET_DIM3_PITCH(tile4DCoeffB, params->kernelW * params->kernelH * params->input.D);
    XI_TILE4D_SET_TYPE(tile4DCoeffB, XI_TILE4D_S8);

    kernel = XI_KERNEL_NAME(xiConvolvedA3D_1x1_U8S8U8j1d1_MOW_DWH);
#if DEBUG_LEVEL_CONV > 1
    printf("#   Conv type: xiConvolvedA3D_1x1_U8S8U8j1d1_MOW_DWH\n");
#endif
    fixup = xiConvolvedFixupA3D_GS_MOW1x1;
    break;

  case kkQM24:
  case kkQM32:

      XI_TILE4D_SET_DATA_ORDER(tile4DCoeffA, XI_NDWH);
      XI_TILE4D_SET_DIM2(tile4DCoeffA, params->input.D);
      XI_TILE4D_SET_DIM3(tile4DCoeffA, params->kernelW);
      XI_TILE4D_SET_DIM4(tile4DCoeffA, params->kernelH);
      XI_TILE4D_SET_TYPE(tile4DCoeffA, XI_TILE4D_S8);

      XI_TILE4D_SET_DATA_ORDER(tile4DCoeffB, XI_NDWH);
      XI_TILE4D_SET_DIM2(tile4DCoeffB, params->input.D);
      XI_TILE4D_SET_DIM3(tile4DCoeffB, params->kernelW);
      XI_TILE4D_SET_DIM4(tile4DCoeffB, params->kernelH);
      XI_TILE4D_SET_TYPE(tile4DCoeffB, XI_TILE4D_S8);

    if (params->isVQ7optimize == 0 && params->kernelW == 1 && params->kernelH == 1 && params->input.D % QM32_FILTER_PADDING == 0)
    {
      /* Fixup can be computed inplace but need to reserve two extra rows. */
      unsigned filter_size = align_up(params->input.D, QM32_FILTER_PADDING);
      buffInputA += 2 * filter_size;
      buffInputB += 2 * filter_size;
    }

    kernel = XI_KERNEL_NAME(xiConvolvedA3D_QM24_MxN_U8S8U8);
    fixup = xiConvolvedFixupA3D_QM32;
    break;
#endif
  case kkVQ_QM24:
  case kkVQ_QM32:
      XI_TILE4D_SET_DATA_ORDER(tile4DCoeffA, XI_NDWH);
      XI_TILE4D_SET_TYPE(tile4DCoeffA, XI_TILE4D_S8);
      //XI_TILE4D_SET_DIM1(tile4DCoeffA, params->tile.D);
      XI_TILE4D_SET_DIM2(tile4DCoeffA, params->input.D);
      XI_TILE4D_SET_DIM3(tile4DCoeffA, params->kernelW);
      XI_TILE4D_SET_DIM4(tile4DCoeffA, params->kernelH);
      //XI_TILE4D_SET_DIM1_PITCH(tile4DCoeffA, params->tile.D);
      //XI_TILE4D_SET_DIM2_PITCH(tile4DCoeffA, params->input.D * XI_TILE4D_GET_DIM1_PITCH(tile4DCoeffA));
      //XI_TILE4D_SET_DIM3_PITCH(tile4DCoeffA, params->kernelW * XI_TILE4D_GET_DIM2_PITCH(tile4DCoeffA));

      XI_TILE4D_SET_DATA_ORDER(tile4DCoeffB, XI_NDWH);
      XI_TILE4D_SET_TYPE(tile4DCoeffB, XI_TILE4D_S8);
      //XI_TILE4D_SET_DIM1(tile4DCoeffB, params->tile.D);
      XI_TILE4D_SET_DIM2(tile4DCoeffB, params->input.D);
      XI_TILE4D_SET_DIM3(tile4DCoeffB, params->kernelW);
      XI_TILE4D_SET_DIM4(tile4DCoeffB, params->kernelH);
      //XI_TILE4D_SET_DIM1_PITCH(tile4DCoeffA, params->tile.D);
      //XI_TILE4D_SET_DIM2_PITCH(tile4DCoeffA, params->input.D * XI_TILE4D_GET_DIM1_PITCH(tile4DCoeffA));
      //XI_TILE4D_SET_DIM3_PITCH(tile4DCoeffA, params->kernelW * XI_TILE4D_GET_DIM2_PITCH(tile4DCoeffA));
      if (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkVQ_QM24)
      {
          kernelVQ = XI_KERNEL_NAME(xiConvolveAVQ3D_QM24_S8_DWH);
#if DEBUG_LEVEL_CONV > 1
          printf("#   Conv type: xiConvolveAVQ3D_QM24_S8_DWH\n");
#endif
      }
      else
      {
          kernelVQ = XI_KERNEL_NAME(xiConvolveAVQ3D_QM32_S8_DWH);
#if DEBUG_LEVEL_CONV > 1
          printf("#   Conv type: xiConvolveAVQ3D_QM32_S8_DWH\n");
#endif
      }
      break;
  default:
    return(XI_ERR_BADARG);
  }

  XI_TILE3D_SET_BUFF_SIZE(tile3DOutpA, mem_info.outpTileSize);
  XI_TILE3D_SET_BUFF_PTR(tile3DOutpA, buffOutputA);
  XI_TILE3D_SET_DATA_PTR(tile3DOutpA, buffOutputA);


  XI_TILE3D_SET_BUFF_SIZE(tile3DOutpB, mem_info.outpTileSize);
  XI_TILE3D_SET_BUFF_PTR(tile3DOutpB, buffOutputB);
  XI_TILE3D_SET_DATA_PTR(tile3DOutpB, buffOutputB);
  if (params->isVQ7optimize == 1)
  {
    XI_TILE3D_SET_DATA_ORDER(tile3DOutpA, XI_ID16WH);
    XI_TILE3D_SET_DATA_ORDER(tile3DOutpB, XI_ID16WH);
  }
  else
  {
    XI_TILE3D_SET_DATA_ORDER(tile3DOutpA, XI_DWH);
    XI_TILE3D_SET_DATA_ORDER(tile3DOutpB, XI_DWH);
  }

  /* Make pitch odd for gather to reduce bank conflicts */
  unsigned int inp_depth_pitch = params->input.D;
  if (inp_depth_pitch % 2 == 0 && ((CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkGS_MOW) || (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkGS_MOW1x1) || (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkGS_MOW_S2)))
  {
    inp_depth_pitch++;
  }

  SetTileType(tile3DInpA, tile3DInpB, tile3DOutpA, tile3DOutpB, &arrPtrOffset, &arr1DBias, &arr1DOutScale, &arr1DOutShift, params);

  XI_TILE3D_SET_BUFF_SIZE(tile3DInpA, mem_info.inpTileSize);
  XI_TILE3D_SET_BUFF_PTR(tile3DInpA, buffInputA);
  if (params->isVQ7optimize == 1)
  {
    XI_TILE3D_SET_DATA_ORDER(tile3DInpA, XI_ID16WH);
    XI_TILE3D_SET_DIM2(tile3DInpA, ((params->input.D + 15) >> 4));
    XI_TILE3D_SET_DIM1_EDGE1(tile3DInpA, edge_left << 4);
    XI_TILE3D_SET_DIM1_EDGE2(tile3DInpA, edge_right << 4);
    XI_TILE3D_SET_DIM3_EDGE1(tile3DInpA, edge_top);
    XI_TILE3D_SET_DIM3_EDGE2(tile3DInpA, edge_bottom);
  }
  else
  {
    XI_TILE3D_SET_DATA_ORDER(tile3DInpA, XI_DWH);
    XI_TILE3D_SET_DIM1(tile3DInpA, params->input.D);
    XI_TILE3D_SET_DIM1_PITCH(tile3DInpA, inp_depth_pitch);
    XI_TILE3D_SET_DIM2_EDGE1(tile3DInpA, edge_left);
    XI_TILE3D_SET_DIM2_EDGE2(tile3DInpA, edge_right);
    XI_TILE3D_SET_DIM3_EDGE1(tile3DInpA, edge_top);
    XI_TILE3D_SET_DIM3_EDGE2(tile3DInpA, edge_bottom);
  }

  XI_TILE3D_SET_BUFF_SIZE(tile3DInpB, mem_info.inpTileSize);
  XI_TILE3D_SET_BUFF_PTR(tile3DInpB, buffInputB);
  if (params->isVQ7optimize == 1)
  {
    XI_TILE3D_SET_DATA_ORDER(tile3DInpB, XI_ID16WH);
    XI_TILE3D_SET_DIM2(tile3DInpB, ((params->input.D + 15) >> 4));
    XI_TILE3D_SET_DIM1_EDGE1(tile3DInpB, edge_left << 4);
    XI_TILE3D_SET_DIM1_EDGE2(tile3DInpB, edge_right << 4);
    XI_TILE3D_SET_DIM3_EDGE1(tile3DInpB, edge_top);
    XI_TILE3D_SET_DIM3_EDGE2(tile3DInpB, edge_bottom);
  }
  else
  {
    XI_TILE3D_SET_DATA_ORDER(tile3DInpB, XI_DWH);
    XI_TILE3D_SET_DIM1(tile3DInpB, params->input.D);
    XI_TILE3D_SET_DIM1_PITCH(tile3DInpB, inp_depth_pitch);
    XI_TILE3D_SET_DIM2_EDGE1(tile3DInpB, edge_left);
    XI_TILE3D_SET_DIM2_EDGE2(tile3DInpB, edge_right);
    XI_TILE3D_SET_DIM3_EDGE1(tile3DInpB, edge_top);
    XI_TILE3D_SET_DIM3_EDGE2(tile3DInpB, edge_bottom);
  }

  XI_TILE_SET_BUFF_SIZE(&structs.fixupTile, mem_info.fixupTileSize);
  XI_TILE_SET_BUFF_PTR(&structs.fixupTile, buffFixup);
  XI_TILE_SET_DATA_PTR(&structs.fixupTile, buffFixup);
  XI_TILE_SET_TYPE(&structs.fixupTile, XI_TILE_S32);

  frame_size_tile3DInp.dim1Size = params->input.D;
  frame_size_tile3DInp.dim2Size = params->input.W;
  frame_size_tile3DInp.dim3Size = params->input.H;

  XI_ARRAY_SET_BUFF_SIZE(&(arr1DBias), mem_info.biasTileSize);
  XI_ARRAY_SET_BUFF_PTR(&(arr1DBias), buffBias);
  XI_ARRAY_SET_HEIGHT(&(arr1DBias), 1);
  XI_ARRAY_SET_BUFF_SIZE(&(arrPtrOffset), ((params->input.D + 15) >> 4) * params->kernelW * params->kernelH * sizeof(int32_t));
  XI_ARRAY_SET_BUFF_PTR(&(arrPtrOffset), buffOffset);
  XI_ARRAY_SET_DATA_PTR(&arrPtrOffset, buffOffset);
  XI_ARRAY_SET_HEIGHT(&(arrPtrOffset), 1);

  //XI_ARRAY_SET_BUFF_SIZE(&(arr1DOutScale), mem_info.outScaleSize);
  XI_ARRAY_SET_BUFF_SIZE(&(arr1DOutScale), params->output.D * sizeof(int32_t));
  XI_ARRAY_SET_BUFF_PTR(&(arr1DOutScale), buffOutScale);
  XI_ARRAY_SET_DATA_PTR(&(arr1DOutScale), buffOutScale);
  XI_ARRAY_SET_WIDTH(&(arr1DOutScale), params->tile.D);
  XI_ARRAY_SET_PITCH(&(arr1DOutScale), params->tile.D);
  XI_ARRAY_SET_HEIGHT(&(arr1DOutScale), 1);

  //XI_ARRAY_SET_BUFF_SIZE(&(arr1DOutShift), mem_info.outShiftSize);
  XI_ARRAY_SET_BUFF_SIZE(&(arr1DOutShift), params->output.D);
  XI_ARRAY_SET_BUFF_PTR(&(arr1DOutShift), buffOutShift);
  XI_ARRAY_SET_DATA_PTR(&(arr1DOutShift), buffOutShift);
  XI_ARRAY_SET_WIDTH(&(arr1DOutShift), params->tile.D);
  XI_ARRAY_SET_PITCH(&(arr1DOutShift), params->tile.D);
  XI_ARRAY_SET_HEIGHT(&(arr1DOutShift), 1);


  XI_CNNA_CONV_SET_STRIDE(&xiparams, params->stride);
  XI_CNNA_CONV_SET_DILATION(&xiparams, 1);
  XI_CNNA_CONV_SET_ZEROPT_INPUT(&xiparams, params->zeroPtInput);
  XI_CNNA_CONV_SET_ZEROPT_COEFF(&xiparams, params->zeroPtCoeff);
  XI_CNNA_CONV_SET_ZEROPT_OUTPUT(&xiparams, params->zeroPtOutput);
  XI_CNNA_CONV_SET_QUANT_SCALE(&xiparams, params->quantizedScale);
  XI_CNNA_CONV_SET_OUTPUT_SHIFT(&xiparams, params->outputShift);
  XI_CNNA_CONV_SET_FLAGS(&xiparams, 0);
  XI_CNNA_CONV_SET_RELUMIN(&xiparams, params->reluMin);
  XI_CNNA_CONV_SET_RELUMAX(&xiparams, params->reluMax);

  if (CONV_FLAG_GET_RELU(params->flags))
  {
    XI_CNNA_CONV_SET_FLAG_RELU(&xiparams);
  }
  if (params->kernelW % 2 == 0)
  {
    XI_CNNA_CONV_SET_FLAG_LEFTEDGE(&xiparams);
  }
  if (params->kernelH % 2 == 0)
  {
    XI_CNNA_CONV_SET_FLAG_TOPEDGE(&xiparams);
  }


  if (CONV_FLAG_GET_PRELU(params->flags))
  {
    XI_TILE3D_SET_BUFF_SIZE(tile3DPreluSlope, mem_info.preluTileSize);
    XI_TILE3D_SET_BUFF_PTR(tile3DPreluSlope, buffPreluSlope);
    XI_TILE3D_SET_DATA_PTR(tile3DPreluSlope, buffPreluSlope);
    XI_TILE3D_SET_DATA_ORDER(tile3DPreluSlope, XI_DWH);
    XI_TILE3D_SET_TYPE(tile3DPreluSlope, XI_TILE3D_U8);
    XI_TILE3D_SET_DIM1(tile3DPreluSlope, params->output.D);
    XI_TILE3D_SET_DIM1_PITCH(tile3DPreluSlope, params->output.D);
    XI_TILE3D_SET_DIM2(tile3DPreluSlope, 1);
    XI_TILE3D_SET_DIM3(tile3DPreluSlope, 1);
    XI_TILE3D_SET_DIM2_PITCH(tile3DPreluSlope, XI_TILE3D_GET_DIM1_PITCH(tile3DPreluSlope));
    if (CONV_FLAG_GET_LEAKY_PRELU(params->flags))
    {
      XI_TILE3D_SET_DIM1(tile3DPreluSlope, 1);
      XI_TILE3D_SET_DIM1_PITCH(tile3DPreluSlope, 1);
      XI_TILE3D_SET_DIM2_PITCH(tile3DPreluSlope, 1);
    }
    /* Loading prelu slope data */
    dma_1d_sys2loc_straddles(/* src */ preluPtr, /* dst */ buffPreluSlope, /* row size */ mem_info.preluTileSize);
  }

  xi_cnn_preluA_params prelu_params;
  if (CONV_FLAG_GET_PRELU(params->flags) || CONV_FLAG_GET_LEAKY_PRELU(params->flags))
  {
    XI_CNN_PRELUA_PARAMS_SET_IN_ZERO_POINT(&prelu_params, params->inZeroPoint);
    XI_CNN_PRELUA_PARAMS_SET_ALPHA_ZERO_POINT(&prelu_params, params->alphaZeroPoint);
    XI_CNN_PRELUA_PARAMS_SET_OUT_ZERO_POINT(&prelu_params, params->outZeroPoint);
    XI_CNN_PRELUA_PARAMS_SET_POSITIVE_OUTPUT_MULTIPLIER(&prelu_params, params->posOutMultiplier);
    XI_CNN_PRELUA_PARAMS_SET_POSITIVE_OUTPUT_SHIFT(&prelu_params, params->posOutShift);
    XI_CNN_PRELUA_PARAMS_SET_NEGATIVE_OUTPUT_MULTIPLIER(&prelu_params, params->negOutMultiplier);
    XI_CNN_PRELUA_PARAMS_SET_NEGATIVE_OUTPUT_SHIFT(&prelu_params, params->negOutShift);
    XI_CNN_PRELUA_PARAMS_SET_MIN_VAL(&prelu_params, 0);
    XI_CNN_PRELUA_PARAMS_SET_MAX_VAL(&prelu_params, 255);
  }

  unsigned int batchSize = params->batch;

  /* Enforce coefficent reload variant if we can't split input tensor over all cores along D */
  if (!CONV_FLAG_GET_RELOAD_INPUT(params->flags) || !doubleBuffInput || (mem_info.numTilesD < (getTotalCores() * 2)))
  {
    /* Prefer to reload coefficients to reduce overall bandwidth */

    uint32_t currTile = 0;
    uint32_t nB, nW, nH;

    /* Find first tile coordinates, incrementing loop variables with wraparound and carry over */
    int exceed = inc_variable_iter(&nW, 0, mem_info.numTilesW,
                      inc_variable_iter(&nB, 0, batchSize,
                            inc_variable_iter(&nH, 0, mem_info.numTilesH, getMyCore())));
    if (exceed)
        return XI_ERROR_STATUS();

    /* Prepare first output tile */
    setup_outp_tile(0, nW * params->tile.W, nH * params->tile.H, tile3DOutpA, params);

    /* Load first input tile */
    setup_inp_tile(tile3DOutpA, tile3DInpA, params);
    transfer_input_tile(&inputPtr[(nB * params->input.D * params->input.W * params->input.H) << FC_FP16_flag], tile3DInpA, params);

    /* Load first coefficients tile */
    uint32_t tileSize = setup_coeff_tile(tile3DOutpA, tile4DCoeffA, params);
    //dma_1d_sys2loc_straddles(/* src */ coeffPtr, /* dst */ buffCoeffA, /* row size */ mem_info.coeffTileSize);
    dma_1d_sys2loc_straddles(/* src */ coeffPtr, /* dst */ buffCoeffA, /* row size */ tileSize);

    /* Wait for first tiles to arrive */
    XI_CHECK_RESULT(dma_barrier());



    // TODO - squash 3 into one, using "next tile cooridnates" calculation to exact pinpoint next tile (nW,nB,nH and D=0)
    for (unsigned int W = 0; W < mem_info.numTilesW; W++)
    {
      for (unsigned int B = 0; B < batchSize; B++)
      {
        for (unsigned int H = 0; H < mem_info.numTilesH; H++)
        {
          /* skip tiles which are processed by other cores */
          if (((currTile++ - getMyCore()) % getTotalCores()) != 0)
              continue;

          /* Find next tile coordinates, incrementing loop variables with wraparound and carry over */
          int lastSpatial;
          lastSpatial =   inc_variable_iter(&nW, W, mem_info.numTilesW,
                              inc_variable_iter(&nB, B, batchSize,
                                  inc_variable_iter(&nH, H, mem_info.numTilesH, getTotalCores())));
          /* Request next input transfer if it's not the last iteration */
          if (!lastSpatial)
          {

            setup_outp_tile(0, nW * params->tile.W, nH * params->tile.H, tile3DOutpB, params);
            setup_inp_tile(tile3DOutpB, tile3DInpB, params);

            transfer_input_tile(&inputPtr[(nB * params->input.D * params->input.W * params->input.H) << FC_FP16_flag], tile3DInpB, params);
          }
          /* Extend edge on the current tile */
          if (params->isVQ7optimize != 1)
          {
            if (XI_TILE3D_GET_STATUS_FLAGS(tile3DInpA))
            {
              INST_EDGE_EXTENSION_BEGIN();
              XI_TILE3D_SET_STATUS_FLAGS(tile3DInpA, 0);
              XI_CHECK_RESULT(_proto_xiExtendEdgesConstWH3D_DWH_IX(tile3DInpA, params->zeroPtInput, frame_size_tile3DInp));
              INST_EDGE_EXTENSION_END();
            }
          }
          /* Prepare fixup tile */
          if (fixup)
          {

            if (params->isVQ7optimize)
            {
              XI_TILE_SET_WIDTH(&structs.fixupTile, XI_TILE3D_GET_DIM1(tile3DOutpA) >> 4);
              XI_TILE_SET_PITCH(&structs.fixupTile, XI_TILE3D_GET_DIM1(tile3DOutpA) >> 4);
            }
            else
            {
              XI_TILE_SET_WIDTH(&structs.fixupTile, XI_TILE3D_GET_DIM2(tile3DOutpA));
              XI_TILE_SET_PITCH(&structs.fixupTile, XI_TILE3D_GET_DIM2(tile3DOutpA));
            }
            XI_TILE_SET_HEIGHT(&structs.fixupTile, XI_TILE3D_GET_DIM3(tile3DOutpA));

            //setup_fixup_tile(tile3DOutpA, &structs.fixupTile, params->isVQ7optimize);
            // adjust zero pof of coefficient
            if (params->coeffType.dataType == XI_TILE4D_S8) {
                XI_CNNA_CONV_SET_ZEROPT_COEFF(&xiparams, params->zeroPtCoeff + 128);
            }
            INST_KERNEL_BEGIN();
#if KERNEL_CYCLES
			int start = XT_RSR_CCOUNT();
#endif
            XI_CHECK_RESULT(fixup(tile3DInpA, tile4DCoeffA, tile3DOutpA, &structs.fixupTile, &xiparams));
#if KERNEL_CYCLES
			fixupCy = XT_RSR_CCOUNT() - start;
#endif
            INST_KERNEL_END();
            XI_CNNA_CONV_SET_ZEROPT_COEFF(&xiparams, params->zeroPtCoeff);
          }
          if (params->isVQ7optimize)
          {
            INST_KERNEL_BEGIN();
            XI_ARRAY_SET_BUFF_SIZE(&(arrPtrOffset), ((XI_TILE3D_GET_DIM2(tile3DInpA))) * params->kernelW * params->kernelH * sizeof(int32_t));
            XI_ARRAY_SET_WIDTH(&arrPtrOffset, ((XI_TILE3D_GET_DIM2(tile3DInpA))) * params->kernelW * params->kernelH);
            XI_ARRAY_SET_PITCH(&arrPtrOffset, ((XI_TILE3D_GET_DIM2(tile3DInpA))) * params->kernelW * params->kernelH);
#if (defined(IVP_DMULQA2N8QXR8))
            XI_CHECK_RESULT(xiComputeOffset_Convd3D_MOD(tile3DInpA, &arrPtrOffset, xiparams.dilationX, xiparams.dilationY, xiparams.strideX, xiparams.strideY, params->kernelW, params->kernelH));
#endif
            INST_KERNEL_END();
          }
          for (unsigned int D = 0; D < mem_info.numTilesD; D++)
          {
             tileSize = min(params->output.D - D * params->tile.D, params->tile.D);
             //XI_ARRAY_SET_BUFF_SIZE(&arr1DOutScale, tileSize * sizeof(int32_t));
             XI_ARRAY_SET_DATA_PTR(&arr1DOutScale, (int32_t *) XI_ARRAY_GET_BUFF_PTR(&arr1DOutScale) + D * params->tile.D);
             XI_ARRAY_SET_WIDTH(&(arr1DOutScale), tileSize);
             XI_ARRAY_SET_PITCH(&(arr1DOutScale), tileSize);

             //XI_ARRAY_SET_BUFF_SIZE(&arr1DOutShift, tileSize);
             XI_ARRAY_SET_DATA_PTR(&arr1DOutShift,  (int8_t *) XI_ARRAY_GET_BUFF_PTR(&arr1DOutShift) + D * params->tile.D);
             XI_ARRAY_SET_WIDTH(&(arr1DOutShift), tileSize);
             XI_ARRAY_SET_PITCH(&(arr1DOutShift), tileSize);


            /* Request next coefficients transfer if it's not the last iteration.
               This loop must circle over coefficients in the case when number of
               coefficient tiles is not 1. */
            int loadCoefficients = mem_info.numTilesD != 1 && (!lastSpatial || D != mem_info.numTilesD - 1);
            if (loadCoefficients && doubleBuffCoeff)
            {
              int nD, nW, nH, nB;
              /* Find next tile coordinates, incrementing loop variables with wraparound and carry over */
              inc_iter_to_temp(&nW, W, mem_info.numTilesW,
                               inc_iter_to_temp(&nB, B, batchSize,
                                                inc_iter_to_temp(&nH, H, mem_info.numTilesH,
                                                                 inc_iter_to_temp(&nD, D, mem_info.numTilesD, 1))));
              setup_outp_tile(nD * params->tile.D, nW * params->tile.W, nH * params->tile.H, tile3DOutpB, params);
              unsigned tileSize = setup_coeff_tile(tile3DOutpB, tile4DCoeffB, params);
              dma_1d_sys2loc_straddles(/* src */ coeffPtr + nD * mem_info.coeffTileSize, /* dst */ XI_TILE4D_GET_BUFF_PTR(tile4DCoeffB), /* row size */ tileSize);
              //dma_1d_sys2loc_straddles(/* src */ coeffPtr + nD * mem_info.coeffTileSize, /* dst */ XI_TILE4D_GET_BUFF_PTR(tile4DCoeffB), /* row size */ tileSize);
            }
            if (FC_FP16_flag)
            {
              XI_ARRAY_SET_DATA_PTR(&arr1DBias, (int16_t *) XI_ARRAY_GET_BUFF_PTR(&arr1DBias) + D * params->tile.D);
            }


            if (params->isVQ7optimize)
            {
              XI_ARRAY_SET_WIDTH(&arr1DBias, XI_TILE3D_GET_DIM2(tile3DOutpA) << 4);
              XI_ARRAY_SET_CAPACITY(&arr1DBias, XI_TILE3D_GET_DIM2(tile3DOutpA) << 4);
              XI_ARRAY_SET_DATA_PTR(&arr1DBias, (int32_t *) XI_ARRAY_GET_BUFF_PTR(&arr1DBias) + D * params->tile.D);
            }
            else
            {
              XI_ARRAY_SET_WIDTH(&arr1DBias, XI_TILE3D_GET_DIM1(tile3DOutpA));
              XI_ARRAY_SET_CAPACITY(&arr1DBias, XI_TILE3D_GET_DIM1(tile3DOutpA));
              XI_ARRAY_SET_DATA_PTR(&arr1DBias, (int32_t *) XI_ARRAY_GET_BUFF_PTR(&arr1DBias) + D * params->tile.D);
            }

            /*
            print_tile3D_U8("tile3DInpA (U8)", tile3DInpA);
            print_tile4D_S8("tile4DCoeffA", tile4DCoeffA);
            print_tile4D_S8("tile4DCoeffB", tile4DCoeffB);
            printf("Debug Print:\n");
            print_tile4D_S8("tile4DCoeffA", tile4DCoeffA);
            print_array_S32("arr1DBias", &arr1DBias);
            print_array_S32("arr1DOutScale", &arr1DOutScale);
            print_array_S8("arr1DOutShift", &arr1DOutShift);
            */
            /* Call XI kernel */
            INST_KERNEL_BEGIN();
#if KERNEL_CYCLES
			int start = XT_RSR_CCOUNT();
#endif

            //convolvedA3D_func(tile3DInpA, &arrPtrOffset, tile4DCoeffA, &arr1DBias, &structs.fixupTile, tile3DOutpA, (xi_cnna_conv_params *) func_params_ptr, kernel, kernel_QMopt, params->isVQ7optimize);
            kernel_func(tile3DInpA, &arrPtrOffset, tile4DCoeffA, &arr1DBias, &structs.fixupTile, &arr1DOutScale, &arr1DOutShift, tile3DOutpA, params, (xi_cnna_conv_params *) func_params_ptr, kernel, kernelVQ, kernel_QMopt);
#if KERNEL_CYCLES
			int stop = XT_RSR_CCOUNT();
			printf("Conv2D=%d\n",fixupCy + stop-start);
#endif

            INST_KERNEL_END();
            // PRELU Activation
            if (CONV_FLAG_GET_PRELU(params->flags) || CONV_FLAG_GET_LEAKY_PRELU(params->flags))
            {
              INST_KERNEL_BEGIN();
              XI_CHECK_RESULT(xiPreluA3D_U8U8U8_DWH(tile3DOutpA, tile3DPreluSlope, tile3DOutpA, &prelu_params));
              INST_KERNEL_END();
            }
            if (loadCoefficients && !doubleBuffCoeff)
            {
              int nD, nW, nH, nB;
              /* Find next tile coordinates, incrementing loop variables with wraparound and carry over */
              inc_iter_to_temp(&nW, W, mem_info.numTilesW,
                               inc_iter_to_temp(&nB, B, batchSize,
                                                inc_iter_to_temp(&nH, H, mem_info.numTilesH,
                                                                 inc_iter_to_temp(&nD, D, mem_info.numTilesD, 1))));
              setup_outp_tile(nD * params->tile.D, nW * params->tile.W, nH * params->tile.H, tile3DOutpB, params);
              unsigned tileSize = setup_coeff_tile(tile3DOutpB, tile4DCoeffB, params);
              dma_1d_sys2loc_straddles(/* src */ coeffPtr + nD * mem_info.coeffTileSize, /* dst */ XI_TILE4D_GET_BUFF_PTR(tile4DCoeffB), /* row size */ tileSize);
            }
            /* Wait for overlapping DMA transfers */
            XI_CHECK_RESULT(dma_barrier());
#if 0
            printf("Debug Print:\n");
            print_tile3D_U8("tile3DInpA (U8)", tile3DInpA);
            print_tile3D_S8("tile3DInpA (S8)", tile3DInpA);
            print_tile4D_S8("tile4DCoeffA", tile4DCoeffA);
            print_array_S32("arr1DBias", &arr1DBias);
            print_array_S32("arr1DOutScale", &arr1DOutScale);
            print_array_S8("arr1DOutShift", &arr1DOutShift);
            print_tile3D_S8("tile3DOutpA", tile3DOutpA);
#endif

            transfer_output_tile(&outputPtr[(B * params->output.D * params->output.W * params->output.H) << FC_FP16_flag], tile3DOutpA, params);

            /* Wait for overlapping DMA transfers */
            //XI_CHECK_RESULT(dma_barrier());
            /* Swap double buffered pointers */
            temp3D = tile3DOutpA; tile3DOutpA = tile3DOutpB; tile3DOutpB = temp3D;
            if (mem_info.numTilesD > 1)
            {
              temp4D = tile4DCoeffA; tile4DCoeffA = tile4DCoeffB; tile4DCoeffB = temp4D;
            }
          }
          temp3D = tile3DInpA; tile3DInpA = tile3DInpB; tile3DInpB = temp3D;
        }
      }
    }
  }
  else
  {
    /* Prefer to reload inputs to reduce overall bandwidth */

    /* Split D dimension on equal parts for each core (besides a last one which may be less)
       and select section of D dimension which is assigned to this core (myCore)
     */
    uint32_t Dcount = (mem_info.numTilesD + getTotalCores() - 1)/getTotalCores();
    uint32_t Dstart = Dcount * getMyCore();
    uint32_t Dend = min(mem_info.numTilesD, (Dstart + Dcount));
    if (Dstart >= Dend)
        return XI_ERROR_STATUS();

    /* Prepare first output tile */
    setup_outp_tile(Dstart, 0, 0, tile3DOutpA, params);

    /* Load first coefficients tile */
    uint32_t tileSize = setup_coeff_tile(tile3DOutpA, tile4DCoeffA, params);

    dma_1d_sys2loc_straddles(/* src */ coeffPtr + Dstart * mem_info.coeffTileSize, /* dst */ buffCoeffA, /* row size */ tileSize);

    /* Load first input tile */
    setup_inp_tile(tile3DOutpA, tile3DInpA, params);
    transfer_input_tile(inputPtr, tile3DInpA, params);

    /* Wait for first tiles to arrive */
    XI_CHECK_RESULT(dma_barrier());

    /* process the section of D assigned to this core */
    for (unsigned int D=Dstart; D < Dend; D++)
    {
      tileSize = min(params->output.D - D * params->tile.D, params->tile.D);
      //XI_ARRAY_SET_BUFF_SIZE(&arr1DOutScale, tileSize * sizeof(int32_t));
      XI_ARRAY_SET_DATA_PTR(&arr1DOutScale, (int32_t *) XI_ARRAY_GET_BUFF_PTR(&arr1DOutScale) + D * params->tile.D);
      XI_ARRAY_SET_WIDTH(&(arr1DOutScale), tileSize);
      XI_ARRAY_SET_PITCH(&(arr1DOutScale), tileSize);

      //XI_ARRAY_SET_BUFF_SIZE(&arr1DOutShift, tileSize);
      XI_ARRAY_SET_DATA_PTR(&arr1DOutShift,  (int8_t *) XI_ARRAY_GET_BUFF_PTR(&arr1DOutShift) + D * params->tile.D);
      XI_ARRAY_SET_WIDTH(&(arr1DOutShift), tileSize);
      XI_ARRAY_SET_PITCH(&(arr1DOutShift), tileSize);

      /* Request next coefficients tile */
      int lastCoeff = D == Dend - 1;

      if (!lastCoeff)
      {
        int nD;
        /* Find next tile coordinates, incrementing loop variables with wraparound and carry over */
        inc_iter_to_temp(&nD, D, mem_info.numTilesD, 1);
        setup_outp_tile(nD * params->tile.D, 0, 0, tile3DOutpB, params);
        unsigned tileSize = setup_coeff_tile(tile3DOutpB, tile4DCoeffB, params);
        if (doubleBuffCoeff)
        {
           //printf("before nD: %d, %d\n", nD, nD * mem_info.coeffTileSize);
           dma_1d_sys2loc_straddles(/* src */ coeffPtr + nD * mem_info.coeffTileSize, /* dst */ XI_TILE4D_GET_BUFF_PTR(tile4DCoeffB), /* row size */ tileSize);
           //XI_CHECK_RESULT(dma_barrier());
        }
      }
      if (FC_FP16_flag)
      {
        XI_ARRAY_SET_DATA_PTR(&arr1DBias, (int16_t *) XI_ARRAY_GET_BUFF_PTR(&arr1DBias) + D * params->tile.D);
      }
      else
      {
        XI_ARRAY_SET_DATA_PTR(&arr1DBias, (int32_t *) XI_ARRAY_GET_BUFF_PTR(&arr1DBias) + D * params->tile.D);
      }

      if (params->isVQ7optimize)
      {
        XI_ARRAY_SET_WIDTH(&arr1DBias, (XI_TILE3D_GET_DIM2(tile3DOutpA)) << 4);
        XI_ARRAY_SET_CAPACITY(&arr1DBias, (XI_TILE3D_GET_DIM2(tile3DOutpA)) << 4);
      }
      else
      {
        XI_ARRAY_SET_WIDTH(&arr1DBias, XI_TILE3D_GET_DIM1(tile3DOutpA));
        XI_ARRAY_SET_CAPACITY(&arr1DBias, XI_TILE3D_GET_DIM1(tile3DOutpA));
      }
      //printf("entered here with number W tiles %d and height %d \n", mem_info.numTilesW, mem_info.numTilesH);

      for (unsigned int W = 0; W < mem_info.numTilesW; W++)
      {
        int lastW = W == (mem_info.numTilesW - 1);
        for (unsigned int B = 0; B < batchSize; B++)
        {
          int lastB = lastW && B == (batchSize - 1);
          for (unsigned int H = 0; H < mem_info.numTilesH; H++)
          {
            int lastSpatial = lastB && H == (mem_info.numTilesH - 1);
            /* Request next input transfer if it's not the last iteration */
            if (doubleBuffInput && (!lastSpatial || !lastCoeff))
            {
              int nD, nW, nH, nB;
              /* Find next tile coordinates, incrementing loop variables with wraparound and carry over */
              inc_iter_to_temp(&nD, D, mem_info.numTilesD,
                               inc_iter_to_temp(&nW, W, mem_info.numTilesW,
                                                inc_iter_to_temp(&nB, B, batchSize,
                                                                 inc_iter_to_temp(&nH, H, mem_info.numTilesH, 1))));
              setup_outp_tile(nD * params->tile.D, nW * params->tile.W, nH * params->tile.H, tile3DOutpB, params);
              setup_inp_tile(tile3DOutpB, tile3DInpB, params);
              transfer_input_tile(&inputPtr[(nB * params->input.D * params->input.W * params->input.H) << FC_FP16_flag], tile3DInpB, params);
            }
            /* Extend edge on the current tile */
            if (params->isVQ7optimize != 1)
            {
              if (XI_TILE3D_GET_STATUS_FLAGS(tile3DInpA))
              {
                INST_EDGE_EXTENSION_BEGIN();
                XI_TILE3D_SET_STATUS_FLAGS(tile3DInpA, 0);
                XI_CHECK_RESULT(_proto_xiExtendEdgesConstWH3D_DWH_IX(tile3DInpA, params->zeroPtInput, frame_size_tile3DInp));
                INST_EDGE_EXTENSION_END();
              }
            }
            INST_KERNEL_BEGIN();
            if (fixup)
            {

              if (params->isVQ7optimize)
              {
                XI_TILE_SET_WIDTH(&structs.fixupTile, XI_TILE3D_GET_DIM1(tile3DOutpA) >> 4);
                XI_TILE_SET_PITCH(&structs.fixupTile, XI_TILE3D_GET_DIM1(tile3DOutpA) >> 4);
              }
              else
              {
                XI_TILE_SET_WIDTH(&structs.fixupTile, XI_TILE3D_GET_DIM2(tile3DOutpA));
                XI_TILE_SET_PITCH(&structs.fixupTile, XI_TILE3D_GET_DIM2(tile3DOutpA));
              }
              XI_TILE_SET_HEIGHT(&structs.fixupTile, XI_TILE3D_GET_DIM3(tile3DOutpA));

              //setup_fixup_tile(tile3DOutpA, &structs.fixupTile, params->isVQ7optimize);
              // adjust zero pof of coefficient
              if (params->coeffType.dataType == XI_TILE4D_S8) {
                XI_CNNA_CONV_SET_ZEROPT_COEFF(&xiparams, params->zeroPtCoeff + 128);
              }
              XI_CHECK_RESULT(fixup(tile3DInpA, tile4DCoeffA, tile3DOutpA, &structs.fixupTile, &xiparams));
              XI_CNNA_CONV_SET_ZEROPT_COEFF(&xiparams, params->zeroPtCoeff);
            }

            if (params->isVQ7optimize)
            {
              XI_ARRAY_SET_BUFF_SIZE(&(arrPtrOffset), ((XI_TILE3D_GET_DIM2(tile3DInpA))) * params->kernelW * params->kernelH * sizeof(int32_t));
              XI_ARRAY_SET_WIDTH(&arrPtrOffset, ((XI_TILE3D_GET_DIM2(tile3DInpA))) * params->kernelW * params->kernelH);
              XI_ARRAY_SET_PITCH(&arrPtrOffset, ((XI_TILE3D_GET_DIM2(tile3DInpA))) * params->kernelW * params->kernelH);
#if (defined(IVP_DMULQA2N8QXR8))
              XI_CHECK_RESULT(xiComputeOffset_Convd3D_MOD(tile3DInpA, &arrPtrOffset, xiparams.dilationX, xiparams.dilationY, xiparams.strideX, xiparams.strideY, params->kernelW, params->kernelH));
#endif
            }

            /* Wait for overlapping DMA transfers */
            //XI_CHECK_RESULT(dma_barrier());
            /*
            print_tile3D_U8("tile3DInpA (U8)", tile3DInpA);
            print_tile4D_S8("tile4DCoeffA", tile4DCoeffA);
            print_tile4D_S8("tile4DCoeffB", tile4DCoeffB);
            printf("Debug Print:\n");
            print_array_S32("arr1DBias", &arr1DBias);
            print_array_S32("arr1DOutScale", &arr1DOutScale);
            print_array_S8("arr1DOutShift", &arr1DOutShift);
            */
            //convolvedA3D_func(tile3DInpA, &arrPtrOffset, tile4DCoeffA, &arr1DBias, &structs.fixupTile, tile3DOutpA, (xi_cnna_conv_params *) func_params_ptr, kernel, kernel_QMopt, params->isVQ7optimize);
            kernel_func(tile3DInpA, &arrPtrOffset, tile4DCoeffA, &arr1DBias, &structs.fixupTile, &arr1DOutScale, &arr1DOutShift, tile3DOutpA, params, (xi_cnna_conv_params *) func_params_ptr, kernel, kernelVQ, kernel_QMopt);
            INST_KERNEL_END();
            // PRELU Activation
            if (CONV_FLAG_GET_PRELU(params->flags) || CONV_FLAG_GET_LEAKY_PRELU(params->flags))
            {
              INST_KERNEL_BEGIN();
              XI_CHECK_RESULT(xiPreluA3D_U8U8U8_DWH(tile3DOutpA, tile3DPreluSlope, tile3DOutpA, &prelu_params));
              INST_KERNEL_END();
            }
            if (!doubleBuffCoeff && mem_info.numTilesD > 1 && lastSpatial && !lastCoeff)
            {
              unsigned tileSize = setup_coeff_tile(tile3DOutpB, tile4DCoeffB, params);
              dma_1d_sys2loc_straddles(/* src */ coeffPtr + (D + 1) * mem_info.coeffTileSize, /* dst */ XI_TILE4D_GET_BUFF_PTR(tile4DCoeffB), /* row size */ tileSize);
            }
            /* Wait for overlapping DMA transfers */
            XI_CHECK_RESULT(dma_barrier());
#if 0
            printf("Debug Print:\n");
            print_tile3D_U8("tile3DInpA (U8)", tile3DInpA);
            print_tile3D_S8("tile3DInpA (S8)", tile3DInpA);
            print_tile4D_S8("tile4DCoeffA", tile4DCoeffA);
            print_array_S32("arr1DBias", &arr1DBias);
            print_array_S32("arr1DOutScale", &arr1DOutScale);
            print_array_S8("arr1DOutShift", &arr1DOutShift);
            print_tile3D_S8("tile3DOutpA", tile3DOutpA);
#endif

            /* Async transfer output tile */
            transfer_output_tile(&outputPtr[(B * params->output.D * params->output.W * params->output.H) << FC_FP16_flag], tile3DOutpA, params);
            /* Wait for overlapping DMA transfers */
            //XI_CHECK_RESULT(dma_barrier())

            /* Swap double buffered pointers */
            temp3D = tile3DOutpA; tile3DOutpA = tile3DOutpB; tile3DOutpB = temp3D;
            temp3D = tile3DInpA; tile3DInpA = tile3DInpB; tile3DInpB = temp3D;
          }
        }
      }
      temp4D = tile4DCoeffA; tile4DCoeffA = tile4DCoeffB; tile4DCoeffB = temp4D;
    }
  }

  XI_CHECK_RESULT(dma_barrier());
#if IS_MULTICHANNEL_DMA
  /* Wait for the last transfer */
  XI_CHECK_RESULT(dma_barrier());
  /* Don't reset arena if last dma_barrier is not executed as  DMA transfer is still ongoing */
  XI_CHECK_RESULT(arena_reset());
#endif

  return(XI_ERROR_STATUS());
}
