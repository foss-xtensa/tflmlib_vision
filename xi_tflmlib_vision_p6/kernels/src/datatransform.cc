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

#include "xi_cnn.h"
#include "xi_intrin.h"
//#include "xi_imgproc.h"

#if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))
#if 0
#if defined(IVP_SAPOS2NX8_FPXP)
#define  FLUSH2NX8(vaOutData, pvecOut, pitch, remVal) { \
  IVP_SAPOS2NX8_FPXP(vaOutData, pvecOut, pitch - remVal);\
}
#define  FLUSHNX8U(vaOutData, pvecOut, pitch, remVal) { \
  IVP_SAPOSNX8U_FPXP(vaOutData, pvecOut, pitch - remVal);\
}
#else
#define  FLUSH2NX8(vaOutData, pvecOut, pitch, remVal) { \
  IVP_SAPOS2NX8_FP(vaOutData, pvecOut); \
   pvecOut = (xb_vec2Nx8 *)((int8_t *)pvecOut + pitch - remVal);\
}
#define  FLUSHNX8U(vaOutData, pvecOut, pitch, remVal) { \
  IVP_SAPOSNX8U_FP(vaOutData, pvecOut); \
   pvecOut = (xb_vecNx8U *)((int8_t *)pvecOut + pitch - remVal);\
}
#endif

#define CLAMP(v, min, max)             ((v) < (min) ? (min) : (v) > (max) ? (max) : (v))
#define ROUND(x, s)                    (((s) == 0) ? (x) : (((x) + (1 << ((s) - 1))) >> (s)))
#define ROUND_N_CLAMP(x, s, min, max)  (((s) == 0) ? (CLAMP(x, min, max)) :                 \
                                                              (CLAMP(ROUND(x, s), min, max)))
#define EDGE_CONST_I8_SCATTER_MIN_WIDTH  8
#define EDGE_CONST_I16_SCATTER_MIN_WIDTH  9

#define INPUT_DATA_TYPE  INTEGER8BIT
#include "fill_tile.h"
#undef INPUT_DATA_TYPE

#define INPUT_DATA_TYPE  INTEGER16BIT
#include "fill_tile.h"
#undef INPUT_DATA_TYPE

#define INPUT_DATA_TYPE  INTEGER8BIT
#include "extend_edge.h"
#undef INPUT_DATA_TYPE

#define INPUT_DATA_TYPE  INTEGER16BIT
#include "extend_edge.h"
#undef INPUT_DATA_TYPE

/*************************** xiFillTile3D ***********************************/
/* Description : General API for FillTile3D optimized implementation        */
/*               Calls one of the FillTile3D functions based                */
/*               on the parameters                                          */
/* Inputs      : constant value to fill, fill_edge_extension                */
/* Outputs     : XI Error Code                                              */
/* InOuts      : Destination Tile                                           */
/****************************************************************************/
XI_ERR_TYPE xiFillTile3D(xi_pTile3D dstTile,
                         const int32_t value,
                         xi_bool fill_edge_extension)
{
  if (!dstTile)
  {
    return(XI_ERR_NULLARG);
  }

  if (XI_TILE3D_GET_ELEMENT_SIZE(dstTile) == 1)
  {
    return (xiFillTile3D_I8(dstTile, value, fill_edge_extension));
  }
  else if (XI_TILE3D_GET_ELEMENT_SIZE(dstTile) == 2)
  {
    return (xiFillTile3D_I16(dstTile, value, fill_edge_extension));
  }
  return(XI_ERR_NO_VARIANT);
}

/************************* xiExtendEdgesConst3D *****************************/
/* Description : General API for ExtendEdgesConst3D optimized implementation*/
/*               Calls one of the ExtendEdgesConst3D functions based        */
/*               on the parameters                                          */
/* Inputs      : constant value to fill the edges                           */
/* Outputs     : XI Error Code                                              */
/* InOuts      : Destination Tile                                           */
/****************************************************************************/
XI_ERR_TYPE xiExtendEdgesConst3D(xi_pTile3D dstTile,
                                 const int32_t value,
                                 xi_size3D frame3DSize)
{
  if (!dstTile)
  {
    return(XI_ERR_NULLARG);
  }

  if (XI_TILE3D_GET_ELEMENT_SIZE(dstTile) == 1)
  {
    return (xiExtendEdgesConst3D_I8(dstTile, value, frame3DSize));
  }
  else if (XI_TILE3D_GET_ELEMENT_SIZE(dstTile) == 2)
  {
    return (xiExtendEdgesConst3D_I16(dstTile, value, frame3DSize));
  }
  return(XI_ERR_NO_VARIANT);
}

/*************************** extendEdgesConst3D_I8 **************************/
/* Description : P6 implementation for extending the edges of a 3D tile     */
/*               with a constant value. This function extends edges across  */
/*               dimension 1 & dimension2 of  a 3D tile                     */
/* Inputs      :  constant value to fill the edges                          */
/* Outputs     : XI Error Code                                              */
/* InOuts      : Destination Tile                                           */
/* Assumptions : dstData is signed/unsigned 8bit                            */
/****************************************************************************/
static void extendEdgesConst3D_I8(xi_pTile3D dstTile,
                                         const int32_t constValue,
                                         xi_size3D frame3DSize)
{
  /* Getting parameters from the tile structures */
  const int32_t dim1Size = XI_TILE3D_GET_DIM1(dstTile);
  const int32_t dim2Size = XI_TILE3D_GET_DIM2(dstTile);
  const int32_t dim1Edge1 = XI_TILE3D_GET_DIM1_EDGE1(dstTile);
  const int32_t dim1Edge2 = XI_TILE3D_GET_DIM1_EDGE2(dstTile);
  const int32_t dim2Edge1 = XI_TILE3D_GET_DIM2_EDGE1(dstTile);
  const int32_t dim2Edge2 = XI_TILE3D_GET_DIM2_EDGE2(dstTile);
  int32_t dim3Size =  XI_TILE3D_GET_DIM3(dstTile);

  const int32_t dstDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(dstTile);
  const int32_t dstDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(dstTile);
  int32_t frame_dim1 = frame3DSize.dim1Size;
  int32_t frame_dim2 = frame3DSize.dim2Size;

  int32_t start_x = XI_TILE3D_GET_DIM1_COORD(dstTile);
  int32_t start_y = XI_TILE3D_GET_DIM2_COORD(dstTile);

  int32_t ixmin = MAX2(start_x - dim1Edge1, 0);
  int32_t ixmax = MIN2(start_x + dim1Size + dim1Edge2 - 1, frame_dim1 - 1);
  int32_t iymin = MAX2(start_y - dim2Edge1, 0);
  int32_t iymax = MIN2(start_y + dim2Size + dim2Edge2 - 1, frame_dim2 - 1);
  const int8_t value = constValue;
  int x, y, z; /* Loop variables */
  valign vaOutData1 = IVP_ZALIGN();
  int32_t dim1ExtendEdgeSize = dim1Size + dim1Edge1 + dim1Edge2;
  int32_t dim2ExtendEdgeSize = (dim2Size + dim2Edge1 + dim2Edge2) * dstDataPitch1;

  int8_t *restrict pDst3D = (int8_t *)XI_TILE3D_GET_DATA_PTR(dstTile);


  // horizontal top
  int32_t horTopXcord = -dim1Edge1;
  int32_t horTopYcord = -dim2Edge1;
  int32_t horTopWidth = dim1Size + dim1Edge1 + dim1Edge2;
  int32_t horTopHeight = iymin - (start_y - dim2Edge1);

  // horizontal bottom
  int32_t horBottomXcord = -dim1Edge1;
  int32_t horBottomYcord = iymax + 1 - start_y;
  int32_t horBottomWidth = dim1Size + dim1Edge1 + dim1Edge2;
  int32_t horBottomHeight = start_y + dim2Size + dim2Edge2 - 1 - iymax;

  // vertical left
  int32_t  verLeftXcord = -dim1Edge1;
  int32_t  verLeftYcord = horTopYcord + horTopHeight;
  int32_t verLeftWidth = ixmin - (start_x - dim1Edge1);
  int32_t verLeftHeight = iymax - iymin + 1;

  // vertical right
  int32_t verRightXcord = ixmax + 1 - start_x;
  int32_t verRightYcord = horTopYcord + horTopHeight;
  int32_t verRightWidth = start_x + dim1Size + dim1Edge2 - 1 - ixmax;
  int32_t verRightHeight = iymax - iymin + 1;


  xb_vec2Nx8U *restrict pdvecOut1, *restrict pdvecOut2;
  uint8_t *restrict pDst1, *restrict pDst2;
  int8_t *restrict pDst;

  /* Most optimal case is when -
     i. dim1 (including edges) has no extra padding
     ii. Each plane, i.e. dim1 * dim2 (including edges in both dimensions) has no extra padding
  */
  if ((dstDataPitch1 == dim1ExtendEdgeSize) && (dstDataPitch2 == dim2ExtendEdgeSize))
  {
    int numIter =  horTopWidth * horTopHeight + horBottomWidth * horBottomHeight;

    // horizontal top first plane
    if (horTopHeight > 0)
    {
      pDst1 = (uint8_t *)pDst3D + \
        ((horTopYcord * dstDataPitch1) + horTopXcord);
      x = 0;
      pdvecOut1 = (xb_vec2Nx8U *)(pDst1);
      for (; x < horTopWidth * horTopHeight - (2 * XCHAL_IVPN_SIMD_WIDTH); \
        x += 2 * XCHAL_IVPN_SIMD_WIDTH)
      {
        IVP_SA2NX8U_IP(value, vaOutData1, pdvecOut1);
      }

      IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut1,
        sizeof(uint8_t) * (horTopWidth * horTopHeight - x));
      IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);
    } //if( horTopHeight > 0)

    z = 0;
    if(dim3Size > 1)
    {
      for(; z < dim3Size - 1; z++)
      {
        pDst1 = (uint8_t *)pDst3D + (z * dstDataPitch2) + \
          ((horBottomYcord * dstDataPitch1) + horBottomXcord);

        pdvecOut1 = (xb_vec2Nx8U *)(pDst1);
        for (x = 0; x < numIter - (2 * XCHAL_IVPN_SIMD_WIDTH); \
          x += 2 * XCHAL_IVPN_SIMD_WIDTH)
        {
          IVP_SA2NX8U_IP(value, vaOutData1, pdvecOut1);
        }
        IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut1,
          sizeof(uint8_t) * (numIter - x));
        IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);
      }
    }


    // horizontal bottom last plane
    if (horBottomHeight > 0)
    {
      pDst1 = (uint8_t *)pDst3D + (z * dstDataPitch2) + \
        ((horBottomYcord * dstDataPitch1) + horBottomXcord);
      x = 0;
      pdvecOut1 = (xb_vec2Nx8U *)(pDst1);
      for (; x < horBottomWidth * horBottomHeight - 2 * XCHAL_IVPN_SIMD_WIDTH; \
        x += 2 * XCHAL_IVPN_SIMD_WIDTH)
      {
        IVP_SA2NX8U_IP(value, vaOutData1, pdvecOut1);
      }
      IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut1,
        sizeof(uint8_t) * (horBottomWidth * horBottomHeight - x));
      IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);
    }
  }
  else
  {
    for(z = 0; z < dim3Size - 1; z+=2)
    {
      // horizontal top
      pDst1 = (uint8_t *)pDst3D + (z * dstDataPitch2) + \
        ((horTopYcord * dstDataPitch1) + horTopXcord);
      pDst2 = (uint8_t *)pDst3D + ((z+1) * dstDataPitch2) \
        + ((horTopYcord * dstDataPitch1) + horTopXcord);
      x = 0;
      if (horTopHeight > 0)
      {
        for (; x < horTopWidth - (2 * XCHAL_IVPN_SIMD_WIDTH); x += 4 * XCHAL_IVPN_SIMD_WIDTH)
        {
          for (y = 0; y < horTopHeight; y++)
          {
            pdvecOut1 = (xb_vec2Nx8U *)(pDst1 + (y * dstDataPitch1) + x);
            pdvecOut2 = (xb_vec2Nx8U *)(pDst2 + (y * dstDataPitch1) + x);
            IVP_SA2NX8U_IP(value, vaOutData1, pdvecOut1);
            IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut1,
              (horTopWidth - x - 2 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);

            IVP_SA2NX8U_IP(value, vaOutData1, pdvecOut2);
            IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut2,
              (horTopWidth - x - 2 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut2);
          }
        }
        if (x < horTopWidth)
        {
          for (y = 0; y < horTopHeight; y++)
          {
            pdvecOut1 = (xb_vec2Nx8U *)(pDst1 + (y * dstDataPitch1) + x);
            pdvecOut2 = (xb_vec2Nx8U *)(pDst2 + (y * dstDataPitch1) + x);
            IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut1, (horTopWidth - x));
            IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);

            IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut2, (horTopWidth - x));
            IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut2);
          }
        }
      } //if( horTopHeight > 0)

      // horizontal bottom
      pDst1 = (uint8_t *)pDst3D + (z * dstDataPitch2) + \
        ((horBottomYcord * dstDataPitch1) + horBottomXcord);
      pDst2 = (uint8_t *)pDst3D + ((z+1) * dstDataPitch2) + \
        ((horBottomYcord * dstDataPitch1) + horBottomXcord);
      x = 0;
      if (horBottomHeight > 0)
      {
        for (; x < horBottomWidth - (2 * XCHAL_IVPN_SIMD_WIDTH); x += 4 * XCHAL_IVPN_SIMD_WIDTH)
        {
          for (y = 0; y < horBottomHeight; y++)
          {
            pdvecOut1 = (xb_vec2Nx8U *)(pDst1 + (y * dstDataPitch1) + x);
            pdvecOut2 = (xb_vec2Nx8U *)(pDst2 + (y * dstDataPitch1) + x);
            IVP_SA2NX8U_IP(value, vaOutData1, pdvecOut1);
            IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut1,
              (horBottomWidth - x - 2 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);

            IVP_SA2NX8U_IP(value, vaOutData1, pdvecOut2);
            IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut2,
              (horBottomWidth - x - 2 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut2);
          }
        }
        if (x < horBottomWidth)
        {
          for (y = 0; y < horBottomHeight; y++)
          {
            pdvecOut1 = (xb_vec2Nx8U *)(pDst1 + (y * dstDataPitch1) + x);
            pdvecOut2 = (xb_vec2Nx8U *)(pDst2 + (y * dstDataPitch1) + x);
            IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut1, (horBottomWidth - x));
            IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);

            IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut2, (horBottomWidth - x));
            IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut2);
          }
        }
      }
    }
    if(z < dim3Size)
    {
      pDst = (int8_t *)XI_TILE3D_GET_DATA_PTR(dstTile) + (z * dstDataPitch2);

      // horizontal top
      pDst1 = (uint8_t *)pDst + ((horTopYcord * dstDataPitch1) + horTopXcord);
      x = 0;
      if (horTopHeight > 0)
      {
        for (; x < horTopWidth - (2 * XCHAL_IVPN_SIMD_WIDTH); x += 4 * XCHAL_IVPN_SIMD_WIDTH)
        {
          for (y = 0; y < horTopHeight; y++)
          {
            pdvecOut1 = (xb_vec2Nx8U *)(pDst1 + (y * dstDataPitch1) + x);
            IVP_SA2NX8U_IP(value, vaOutData1, pdvecOut1);
            IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut1,
              (horTopWidth - x - 2 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);
          }
        }
        if (x < horTopWidth)
        {
          for (y = 0; y < horTopHeight; y++)
          {
            pdvecOut1 = (xb_vec2Nx8U *)(pDst1 + (y * dstDataPitch1) + x);
            IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut1, (horTopWidth - x));
            IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);
          }
        }
      } //if( horTopHeight > 0)

      // horizontal bottom
      pDst1 = (uint8_t *)pDst + ((horBottomYcord * dstDataPitch1) + horBottomXcord);
      x = 0;
      if (horBottomHeight > 0)
      {
        for (; x < horBottomWidth - (2 * XCHAL_IVPN_SIMD_WIDTH); x += 4 * XCHAL_IVPN_SIMD_WIDTH)
        {
          for (y = 0; y < horBottomHeight; y++)
          {
            pdvecOut1 = (xb_vec2Nx8U *)(pDst1 + (y * dstDataPitch1) + x);
            IVP_SA2NX8U_IP(value, vaOutData1, pdvecOut1);
            IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut1,
              (horBottomWidth - x - 2 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);
          }
        }
        if (x < horBottomWidth)
        {
          for (y = 0; y < horBottomHeight; y++)
          {
            pdvecOut1 = (xb_vec2Nx8U *)(pDst1 + (y * dstDataPitch1) + x);
            IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut1, (horBottomWidth - x));
            IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);
          }
        }
      }
    }
  }
  for(z = 0; z < dim3Size - 1; z+=2)
  {
    // vertical left
    pDst1 = (uint8_t *)pDst3D + (z * dstDataPitch2) + \
      ((verLeftYcord * dstDataPitch1) + verLeftXcord);
    pDst2 = (uint8_t *)pDst3D + ((z+1) * dstDataPitch2) \
      + ((verLeftYcord * dstDataPitch1) + verLeftXcord);
    x = 0;
    for (; x < verLeftWidth; x += 2 * XCHAL_IVPN_SIMD_WIDTH)
    {
      for (y = 0; y < verLeftHeight; y++)
      {
        pdvecOut1 = (xb_vec2Nx8U *)(pDst1 + (y * dstDataPitch1) + x);
        pdvecOut2 = (xb_vec2Nx8U *)(pDst2 + (y * dstDataPitch1) + x);
        IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut1, (verLeftWidth - x));
        IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);
        IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut2, (verLeftWidth - x));
        IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut2);
      }
    }

    // vertical right
    pDst1 = (uint8_t *)pDst3D + (z * dstDataPitch2) + \
      ((verRightYcord * dstDataPitch1) + verRightXcord);
    pDst2 = (uint8_t *)pDst3D + ((z+1) * dstDataPitch2) + \
      ((verRightYcord * dstDataPitch1) + verRightXcord);
    x = 0;
    for (; x < verRightWidth; x += 2 * XCHAL_IVPN_SIMD_WIDTH)
    {
      for (y = 0; y < verRightHeight; y++)
      {
        pdvecOut1 = (xb_vec2Nx8U *)(pDst1 + (y * dstDataPitch1) + x);
        pdvecOut2 = (xb_vec2Nx8U *)(pDst2 + (y * dstDataPitch1) + x);
        IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut1, (verRightWidth - x));
        IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);
        IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut2, (verRightWidth - x));
        IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut2);
      }
    }
  }
  if(z < dim3Size)
  {
    pDst = (int8_t *)XI_TILE3D_GET_DATA_PTR(dstTile) + (z * dstDataPitch2);

    // vertical left
    pDst1 = (uint8_t *)pDst + ((verLeftYcord * dstDataPitch1) + verLeftXcord);
    x = 0;
    for (; x < verLeftWidth; x += 2 * XCHAL_IVPN_SIMD_WIDTH)
    {
      for (y = 0; y < verLeftHeight; y++)
      {
        pdvecOut1 = (xb_vec2Nx8U *)(pDst1 + (y * dstDataPitch1) + x);
        IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut1, (verLeftWidth - x));
        IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);
      }
    }

    // vertical right
    pDst1 = (uint8_t *)pDst + ((verRightYcord * dstDataPitch1) + verRightXcord);
    x = 0;
    for (; x < verRightWidth; x += 2 * XCHAL_IVPN_SIMD_WIDTH)
    {
      for (y = 0; y < verRightHeight; y++)
      {
        pdvecOut1 = (xb_vec2Nx8U *)(pDst1 + (y * dstDataPitch1) + x);
        IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut1, (verRightWidth - x));
        IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);
      }
    }
  }
}

/***********************   xiExtendEdges3D   ********************************/
/* Description : General API for ExtendEdges3D optimized implementation     */
/*               Calls one of the ExtendEdges3D functions based             */
/*               on the parameters                                          */
/* Inputs      : pArray, frame3DSize                                        */
/* Outputs     : XI Error Code                                              */
/* InOuts      : Input Tile                                                 */
/****************************************************************************/
XI_ERR_TYPE xiExtendEdges3D(xi_pTile3D dstTile,
                            const xi_pArray pArray,
                            xi_size3D frame3DSize)
{
  if (!dstTile)
  {
    return(XI_ERR_NULLARG);
  }
  if (XI_TILE3D_GET_ELEMENT_SIZE(dstTile) == 1)
  {
    return (xiExtendEdges3D_I8(dstTile, pArray, frame3DSize));
  }
  else if (XI_TILE3D_GET_ELEMENT_SIZE(dstTile) == 2)
  {
    return (xiExtendEdges3D_I16(dstTile, pArray, frame3DSize));
  }
  return(XI_ERR_NO_VARIANT);
}

/***********************   xiExtendEdges3D_I8   *****************************/
/* Description : General API for ExtendEdges3D optimized implementation     */
/*               Calls one of the ExtendEdges3D functions based             */
/*               on the parameters                                          */
/* Inputs      : pArray, frame3DSize                                        */
/* Outputs     : XI Error Code                                              */
/* InOuts      : Input Tile                                                 */
/****************************************************************************/
XI_ERR_TYPE xiExtendEdges3D_I8(xi_pTile3D dstTile,
                               const xi_pArray pArray,
                               xi_size3D frame3DSize)
{
  /* Error Checks */
  XI_ERROR_CHECKS()
  {
    XI_CHECK_TILE3D_I8(dstTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(dstTile);
    XI_CHECK_ARRAY_I8(pArray);
    XI_CHECK_ERROR(((XI_TILE3D_GET_DATA_ORDER(dstTile) == XI_WHD) || (XI_TILE3D_GET_DATA_ORDER(dstTile) == XI_DWH)\
      || (XI_TILE3D_GET_DATA_ORDER(dstTile) == XI_ID16WH)),
      XI_ERR_BADARG, "\nData Order should be WHD or DWH or ID16WH.");
    XI_CHECK_POINTER(pArray);
    XI_CHECK_ERROR((frame3DSize.dim1Size > 0) && (frame3DSize.dim2Size > 0) && (frame3DSize.dim3Size > 0), XI_ERR_DATASIZE, \
      "\nFrame dim1 = %d, dim2 = %d, dim3 = %d\nFrame dimensions should be greater than 0", \
      frame3DSize.dim1Size, frame3DSize.dim2Size, frame3DSize.dim3Size);
  }
  if (XI_TILE3D_GET_DATA_ORDER(dstTile) == XI_WHD)
  {
    XI_ERROR_CHECKS_CONTINUE()
    {
      XI_CHECK_ERROR(((XI_ARRAY_GET_WIDTH(pArray) >= (XI_TILE3D_GET_DIM3(dstTile) \
        + XI_TILE3D_GET_DIM3_EDGE1(dstTile) + XI_TILE3D_GET_DIM3_EDGE2(dstTile)))), XI_ERR_BADARG, \
        "\npArray width = %d, dstTile dim3 = %d, edge1 = %d, edge2 = %d\npArray width should be greater than or \
        equal to dstTile dim3 size + edge1 +edge2", XI_ARRAY_GET_WIDTH(pArray), XI_TILE3D_GET_DIM3(dstTile),\
        XI_TILE3D_GET_DIM3_EDGE1(dstTile), XI_TILE3D_GET_DIM3_EDGE2(dstTile));
    }
    extendEdges3D_I8_WHD(dstTile, pArray, frame3DSize);
  }
  else if (XI_TILE3D_GET_DATA_ORDER(dstTile) == XI_DWH)
  {
    XI_ERROR_CHECKS_CONTINUE()
    {
      XI_CHECK_ERROR(((XI_ARRAY_GET_WIDTH(pArray) >= (XI_TILE3D_GET_DIM1(dstTile) \
        + XI_TILE3D_GET_DIM1_EDGE1(dstTile) + XI_TILE3D_GET_DIM1_EDGE2(dstTile)))), XI_ERR_BADARG, \
        "\npArray width = %d, dstTile dim1 = %d, edge1 = %d, edge2 = %d\npArray width should be greater than or \
        equal to dstTile dim1 size + edge1 +edge2", XI_ARRAY_GET_WIDTH(pArray), XI_TILE3D_GET_DIM1(dstTile), \
        XI_TILE3D_GET_DIM1_EDGE1(dstTile), XI_TILE3D_GET_DIM1_EDGE2(dstTile));
    }
    extendEdges3D_I8_DWH(dstTile, pArray, frame3DSize);
  }
#if (XCHAL_VISION_TYPE >= 7)
  else
  {
    XI_ERROR_CHECKS_CONTINUE()
    {
      XI_CHECK_ERROR(((XI_ARRAY_GET_WIDTH(pArray) >= ((XI_TILE3D_GET_DIM2(dstTile) << 4) \
        + (XI_TILE3D_GET_DIM2_EDGE1(dstTile) << 4) + (XI_TILE3D_GET_DIM2_EDGE2(dstTile) << 4)))), XI_ERR_BADARG, \
        "\npArray width = %d, dstTile dim2 = %d, edge1 = %d, edge2 = %d\npArray width should be greater than or \
        equal to dstTile dim2 (size + edge1 +edge2)*16", XI_ARRAY_GET_WIDTH(pArray), XI_TILE3D_GET_DIM2(dstTile), \
        XI_TILE3D_GET_DIM2_EDGE1(dstTile), XI_TILE3D_GET_DIM2_EDGE2(dstTile));
      XI_CHECK_ERROR((XI_ALIGN_VAL(XI_TILE3D_GET_DIM1(dstTile), 16) == XI_TILE3D_GET_DIM1(dstTile)), XI_ERR_BADARG, \
        "\nInput tile dim1 = %d, Dimension 1 Size of the input tile should be multiple of 16", XI_TILE3D_GET_DIM1(dstTile));
      XI_CHECK_ERROR((XI_ALIGN_VAL(XI_TILE3D_GET_DIM1_EDGE1(dstTile), 16) == XI_TILE3D_GET_DIM1_EDGE1(dstTile)), XI_ERR_BADARG, \
        "\nInput tile dim1 edge1= %d, value should be multiple of 16", XI_TILE3D_GET_DIM1_EDGE1(dstTile));
      XI_CHECK_ERROR((XI_ALIGN_VAL(XI_TILE3D_GET_DIM1_EDGE2(dstTile), 16) == XI_TILE3D_GET_DIM1_EDGE2(dstTile)), XI_ERR_BADARG, \
        "\nInput tile dim1 edge2 = %d, value should be multiple of 16", XI_TILE3D_GET_DIM1_EDGE2(dstTile));
      XI_CHECK_ERROR((XI_ALIGN_VAL(frame3DSize.dim1Size, 16) == frame3DSize.dim1Size), XI_ERR_BADARG, \
        "\nDim1 Size of the Frame = %d, size of the Frame should be multiple of 16", frame3DSize.dim1Size);
      XI_CHECK_ERROR((XI_ALIGN_VAL(XI_TILE3D_GET_DIM1_COORD(dstTile), 16) == XI_TILE3D_GET_DIM1_COORD(dstTile)), XI_ERR_BADARG, \
        "\nDim1 Coordinate of the input tile = %d, value should be multiple of 16", XI_TILE3D_GET_DIM1_COORD(dstTile));
    }
    extendEdges3D_I8_ID16WH(dstTile, pArray, frame3DSize);
  }
#endif //(XCHAL_VISION_TYPE >= 7)
  return(XI_ERROR_STATUS());
}

/***********************   xiExtendEdges3D_I16   ****************************/
/* Description : General API for ExtendEdges3D optimized implementation     */
/*               Calls one of the ExtendEdges3D functions based             */
/*               on the parameters                                          */
/* Inputs      : pArray, frame3DSize                                        */
/* Outputs     : XI Error Code                                              */
/* InOuts      : Input Tile                                                 */
/****************************************************************************/
XI_ERR_TYPE xiExtendEdges3D_I16(xi_pTile3D dstTile,
                                const xi_pArray pArray,
                                xi_size3D frame3DSize)
{
  /* Error Checks */
  XI_ERROR_CHECKS()
  {
    XI_CHECK_TILE3D_I16(dstTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(dstTile);
    XI_CHECK_ARRAY_I16(pArray);
    XI_CHECK_ERROR(((XI_TILE3D_GET_DATA_ORDER(dstTile) == XI_WHD) || (XI_TILE3D_GET_DATA_ORDER(dstTile) == XI_DWH)\
      || (XI_TILE3D_GET_DATA_ORDER(dstTile) == XI_ID16WH)),
      XI_ERR_BADARG, "\nProvided Data Order not supported.");
    XI_CHECK_POINTER(pArray);
    XI_CHECK_ERROR((frame3DSize.dim1Size > 0) && (frame3DSize.dim2Size > 0) && (frame3DSize.dim3Size > 0), XI_ERR_DATASIZE, \
      "\nFrame dim1 = %d, dim2 = %d, dim3 = %d\nFrame dimensions should be greater than 0", \
      frame3DSize.dim1Size, frame3DSize.dim2Size, frame3DSize.dim3Size);
  }
  if (XI_TILE3D_GET_DATA_ORDER(dstTile) == XI_WHD)
  {
    XI_ERROR_CHECKS_CONTINUE()
    {
      XI_CHECK_ERROR(((XI_ARRAY_GET_WIDTH(pArray) >= (XI_TILE3D_GET_DIM3(dstTile) \
        + XI_TILE3D_GET_DIM3_EDGE1(dstTile) + XI_TILE3D_GET_DIM3_EDGE2(dstTile)))), XI_ERR_BADARG, \
        "pArray width and height parameters is not set as required");
    }
    extendEdges3D_I16_WHD(dstTile, pArray, frame3DSize);
  }
  else if (XI_TILE3D_GET_DATA_ORDER(dstTile) == XI_DWH)
  {
    XI_ERROR_CHECKS_CONTINUE()
    {
      XI_CHECK_ERROR(((XI_ARRAY_GET_WIDTH(pArray) >= (XI_TILE3D_GET_DIM1(dstTile) \
        + XI_TILE3D_GET_DIM1_EDGE1(dstTile) + XI_TILE3D_GET_DIM1_EDGE2(dstTile)))), XI_ERR_BADARG, \
        "\npArray width and height parameters is not set as required");
    }
    extendEdges3D_I16_DWH(dstTile, pArray, frame3DSize);
  }
  else if (XI_TILE3D_GET_DATA_ORDER(dstTile) == XI_ID16WH)
  {
    XI_ERROR_CHECKS_CONTINUE()
    {
      XI_CHECK_ERROR(((XI_ARRAY_GET_WIDTH(pArray) >= ((XI_TILE3D_GET_DIM2(dstTile) << 4) \
        + (XI_TILE3D_GET_DIM2_EDGE1(dstTile) << 4) + (XI_TILE3D_GET_DIM2_EDGE2(dstTile) << 4)))), XI_ERR_BADARG, \
        "\npArray width and height parameters is not set as required");
      XI_CHECK_ERROR((XI_ALIGN_VAL(XI_TILE3D_GET_DIM1(dstTile), 16) == XI_TILE3D_GET_DIM1(dstTile)), XI_ERR_BADARG, \
        "\nDim1 Size of the input tile = %d, value should be multiple of 16", XI_TILE3D_GET_DIM1(dstTile));
      XI_CHECK_ERROR((XI_ALIGN_VAL(XI_TILE3D_GET_DIM1_EDGE1(dstTile), 16) == XI_TILE3D_GET_DIM1_EDGE1(dstTile)), XI_ERR_BADARG, \
        "\nDim1 Edge1 of the input tile = %d, value should be multiple of 16", XI_TILE3D_GET_DIM1_EDGE1(dstTile));
      XI_CHECK_ERROR((XI_ALIGN_VAL(XI_TILE3D_GET_DIM1_EDGE2(dstTile), 16) == XI_TILE3D_GET_DIM1_EDGE2(dstTile)), XI_ERR_BADARG, \
        "\nDim1 Edge2 of the input tile = %d, value should be multiple of 16", XI_TILE3D_GET_DIM1_EDGE2(dstTile));
      XI_CHECK_ERROR((XI_ALIGN_VAL(frame3DSize.dim1Size, 16) == frame3DSize.dim1Size), XI_ERR_BADARG, \
        "\nDim1 size of the Frame = %d, value should be multiple of 16", frame3DSize.dim1Size);
      XI_CHECK_ERROR((XI_ALIGN_VAL(XI_TILE3D_GET_DIM1_COORD(dstTile), 16) == XI_TILE3D_GET_DIM1_COORD(dstTile)), XI_ERR_BADARG, \
        "\nDim1 Coordinate of the input tile = %d, value should be multiple of 16", XI_TILE3D_GET_DIM1_COORD(dstTile));
    }
#if (XCHAL_VISION_TYPE >= 7)
    extendEdges3D_I16_ID16WH(dstTile, pArray, frame3DSize);
#endif
  }
  return(XI_ERROR_STATUS());
}

/************************** extendWHEdges3D_I8 ******************************/
/* Description : P6 implementation for extending the edges of a 3D tile     */
/*               by filling different edge values for  different depths and */
/*               extends the edges along dimension 1(W) and dimension 2(H)  */
/*               of 3D tile                                                 */
/* Inputs      : pValue(array of edge values)                               */
/* Outputs     : XI Error Code                                              */
/* InOuts      : Destination Tile                                           */
/* Assumptions : dstData is signed/unsigned 8bit                            */
/****************************************************************************/

static void extendWHEdges3D_I8(xi_pTile3D dstTile,
                               const int8_t * pValue,
                               xi_size3D frame3DSize)
{
  /* Getting parameters from the tile structures */
  const int32_t dim1Size = XI_TILE3D_GET_DIM1(dstTile);
  const int32_t dim2Size = XI_TILE3D_GET_DIM2(dstTile);
  const int32_t dim1Edge1 = XI_TILE3D_GET_DIM1_EDGE1(dstTile);
  const int32_t dim1Edge2 = XI_TILE3D_GET_DIM1_EDGE2(dstTile);
  const int32_t dim2Edge1 = XI_TILE3D_GET_DIM2_EDGE1(dstTile);
  const int32_t dim2Edge2 = XI_TILE3D_GET_DIM2_EDGE2(dstTile);
  int32_t dim3Size =  XI_TILE3D_GET_DIM3(dstTile);

  const int32_t dstDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(dstTile);
  const int32_t dstDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(dstTile);
  int32_t frame_dim1 = frame3DSize.dim1Size;
  int32_t frame_dim2 = frame3DSize.dim2Size;

  int32_t start_x = XI_TILE3D_GET_DIM1_COORD(dstTile);
  int32_t start_y = XI_TILE3D_GET_DIM2_COORD(dstTile);

  int32_t ixmin = MAX2(start_x - dim1Edge1, 0);
  int32_t ixmax = MIN2(start_x + dim1Size + dim1Edge2 - 1, frame_dim1 - 1);
  int32_t iymin = MAX2(start_y - dim2Edge1, 0);
  int32_t iymax = MIN2(start_y + dim2Size + dim2Edge2 - 1, frame_dim2 - 1);

  int x, y, z; /* Loop variables */
  valign vaOutData1 = IVP_ZALIGN();
  valign vaOutData2 = IVP_ZALIGN();
  int32_t dim1ExtendEdgeSize = dim1Size + dim1Edge1 + dim1Edge2;

  int8_t *restrict pDst3D = (int8_t *)XI_TILE3D_GET_DATA_PTR(dstTile);

  // horizontal top
  int32_t horTopXcord = -dim1Edge1;
  int32_t horTopYcord = -dim2Edge1;
  int32_t horTopWidth = dim1Size + dim1Edge1 + dim1Edge2;
  int32_t horTopHeight = iymin - (start_y - dim2Edge1);


  // horizontal bottom
  int32_t horBottomXcord = -dim1Edge1;
  int32_t horBottomYcord = iymax + 1 - start_y;
  int32_t horBottomWidth = dim1Size + dim1Edge1 + dim1Edge2;
  int32_t horBottomHeight = start_y + dim2Size + dim2Edge2 - 1 - iymax;

  // vertical left
  int32_t  verLeftXcord = -dim1Edge1;
  int32_t  verLeftYcord = horTopYcord + horTopHeight;
  int32_t verLeftWidth = ixmin - (start_x - dim1Edge1);
  int32_t verLeftHeight = iymax - iymin + 1;

  // vertical right
  int32_t verRightXcord = ixmax + 1 - start_x;
  int32_t verRightYcord = horTopYcord + horTopHeight;
  int32_t verRightWidth = start_x + dim1Size + dim1Edge2 - 1 - ixmax;
  int32_t verRightHeight = iymax - iymin + 1;

  xb_vec2Nx8U *restrict pdvecOut1, *restrict pdvecOut2;
  uint8_t *restrict pDst1, *restrict pDst2;
  int8_t *restrict pDst;
  if(dstDataPitch1 == dim1ExtendEdgeSize)
  {
    for(z = 0; z < dim3Size - 1; z+=2)
    {
      const int8_t value1 = pValue[z];
      const int8_t value2 = pValue[z + 1];

      // horizontal top
      pDst1 = (uint8_t *)pDst3D + (z * dstDataPitch2) + \
        ((horTopYcord * dstDataPitch1) + horTopXcord);
      pDst2 = (uint8_t *)pDst3D + ((z+1) * dstDataPitch2) + \
        ((horTopYcord * dstDataPitch1) + horTopXcord);
      x = 0;
      if (horTopHeight > 0)
      {
        pdvecOut1 = (xb_vec2Nx8U *)(pDst1);
        pdvecOut2 = (xb_vec2Nx8U *)(pDst2);
        for (; x < horTopWidth * horTopHeight - (2 * XCHAL_IVPN_SIMD_WIDTH); x += \
          2 * XCHAL_IVPN_SIMD_WIDTH)
        {
          IVP_SA2NX8U_IP(value1, vaOutData1, pdvecOut1);
          IVP_SA2NX8U_IP(value2, vaOutData2, pdvecOut2);
        }
        IVP_SAV2NX8U_XP(value1, vaOutData1, pdvecOut1,
            sizeof(uint8_t) * (horTopWidth * horTopHeight - x));
        IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);

        IVP_SAV2NX8U_XP(value2, vaOutData2, pdvecOut2,
            sizeof(uint8_t) * (horTopWidth * horTopHeight - x));
        IVP_SAV2NX8UPOS_FP(vaOutData2, pdvecOut2);
      } //if( horTopHeight > 0)

      // horizontal bottom
      pDst1 = (uint8_t *)pDst3D + (z * dstDataPitch2) + \
        ((horBottomYcord * dstDataPitch1) + horBottomXcord);
      pDst2 = (uint8_t *)pDst3D + ((z+1) * dstDataPitch2) + \
        ((horBottomYcord * dstDataPitch1) + horBottomXcord);
      x = 0;
      if (horBottomHeight > 0)
      {
        pdvecOut1 = (xb_vec2Nx8U *)(pDst1);
        pdvecOut2 = (xb_vec2Nx8U *)(pDst2);
        for (; x < horBottomWidth * horBottomHeight - 2 * XCHAL_IVPN_SIMD_WIDTH; x += \
          2 * XCHAL_IVPN_SIMD_WIDTH)
        {
          IVP_SA2NX8U_IP(value1, vaOutData1, pdvecOut1);
          IVP_SA2NX8U_IP(value2, vaOutData2, pdvecOut2);
        }
        IVP_SAV2NX8U_XP(value1, vaOutData1, pdvecOut1,
            sizeof(uint8_t) * (horBottomWidth * horBottomHeight - x));
        IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);

        IVP_SAV2NX8U_XP(value2, vaOutData2, pdvecOut2,
            sizeof(uint8_t) * (horBottomWidth * horBottomHeight - x));
        IVP_SAV2NX8UPOS_FP(vaOutData2, pdvecOut2);
      }
    }
    if(z < dim3Size)
    {
      const int8_t value = pValue[z];
      pDst = (int8_t *)XI_TILE3D_GET_DATA_PTR(dstTile) + (z * dstDataPitch2);

      // horizontal top
      pDst1 = (uint8_t *)pDst + ((horTopYcord * dstDataPitch1) + horTopXcord);
      x = 0;
      if (horTopHeight > 0)
      {
        pdvecOut1 = (xb_vec2Nx8U *)(pDst1);
        for (; x < horTopWidth * horTopHeight - (2 * XCHAL_IVPN_SIMD_WIDTH); x += \
          2 * XCHAL_IVPN_SIMD_WIDTH)
        {
          IVP_SA2NX8U_IP(value, vaOutData1, pdvecOut1);
          IVP_SA2NX8UPOS_FP(vaOutData1, pdvecOut1);
        }
        for (; x < horTopWidth * horTopHeight; x += 2 * XCHAL_IVPN_SIMD_WIDTH)
        {
          IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut1,
            sizeof(uint8_t) * (horTopWidth * horTopHeight - x));
          IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);
        }
      } //if( horTopHeight > 0)

      // horizontal bottom
      pDst1 = (uint8_t *)pDst + ((horBottomYcord * dstDataPitch1) + horBottomXcord);
      x = 0;
      if (horBottomHeight > 0)
      {
        pdvecOut1 = (xb_vec2Nx8U *)(pDst1);
        for (; x < horBottomWidth * horBottomHeight - 2 * XCHAL_IVPN_SIMD_WIDTH; x += \
          2 * XCHAL_IVPN_SIMD_WIDTH)
        {
          IVP_SA2NX8U_IP(value, vaOutData1, pdvecOut1);
          IVP_SA2NX8UPOS_FP(vaOutData1, pdvecOut1);
        }
        for (; x < horBottomWidth * horBottomHeight; x += 2 * XCHAL_IVPN_SIMD_WIDTH)
        {
          IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut1,
            sizeof(uint8_t) * (horBottomWidth * horBottomHeight - x));
          IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);
        }
      }
    }
  }
  else
  {
    for(z = 0; z < dim3Size - 1; z+=2)
    {
      const int8_t value1 = pValue[z];
      const int8_t value2 = pValue[z + 1];
      // horizontal top
      pDst1 = (uint8_t *)pDst3D + (z * dstDataPitch2) + \
        ((horTopYcord * dstDataPitch1) + horTopXcord);
      pDst2 = (uint8_t *)pDst3D + ((z+1) * dstDataPitch2) + \
        ((horTopYcord * dstDataPitch1) + horTopXcord);
      x = 0;
      if (horTopHeight > 0)
      {
        for (; x < horTopWidth - (2 * XCHAL_IVPN_SIMD_WIDTH); x += 4 * XCHAL_IVPN_SIMD_WIDTH)
        {
          for (y = 0; y < horTopHeight; y++)
          {
            pdvecOut1 = (xb_vec2Nx8U *)(pDst1 + (y * dstDataPitch1) + x);
            pdvecOut2 = (xb_vec2Nx8U *)(pDst2 + (y * dstDataPitch1) + x);
            IVP_SA2NX8U_IP(value1, vaOutData1, pdvecOut1);
            IVP_SAV2NX8U_XP(value1, vaOutData1, pdvecOut1,
              (horTopWidth - x - 2 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);

            IVP_SA2NX8U_IP(value2, vaOutData1, pdvecOut2);
            IVP_SAV2NX8U_XP(value2, vaOutData1, pdvecOut2,
              (horTopWidth - x - 2 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut2);
          }
        }
        if (x < horTopWidth)
        {
          for (y = 0; y < horTopHeight; y++)
          {
            pdvecOut1 = (xb_vec2Nx8U *)(pDst1 + (y * dstDataPitch1) + x);
            pdvecOut2 = (xb_vec2Nx8U *)(pDst2 + (y * dstDataPitch1) + x);
            IVP_SAV2NX8U_XP(value1, vaOutData1, pdvecOut1, (horTopWidth - x));
            IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);

            IVP_SAV2NX8U_XP(value2, vaOutData1, pdvecOut2, (horTopWidth - x));
            IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut2);
          }
        }
      } //if( horTopHeight > 0)

      // horizontal bottom
      pDst1 = (uint8_t *)pDst3D + (z * dstDataPitch2) + \
        ((horBottomYcord * dstDataPitch1) + horBottomXcord);
      pDst2 = (uint8_t *)pDst3D + ((z+1) * dstDataPitch2) + \
        ((horBottomYcord * dstDataPitch1) + horBottomXcord);
      x = 0;
      if (horBottomHeight > 0)
      {
        for (; x < horBottomWidth - (2 * XCHAL_IVPN_SIMD_WIDTH); x += 4 * XCHAL_IVPN_SIMD_WIDTH)
        {
          for (y = 0; y < horBottomHeight; y++)
          {
            pdvecOut1 = (xb_vec2Nx8U *)(pDst1 + (y * dstDataPitch1) + x);
            pdvecOut2 = (xb_vec2Nx8U *)(pDst2 + (y * dstDataPitch1) + x);
            IVP_SA2NX8U_IP(value1, vaOutData1, pdvecOut1);
            IVP_SAV2NX8U_XP(value1, vaOutData1, pdvecOut1,
              (horBottomWidth - x - 2 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);

            IVP_SA2NX8U_IP(value2, vaOutData1, pdvecOut2);
            IVP_SAV2NX8U_XP(value2, vaOutData1, pdvecOut2,
              (horBottomWidth - x - 2 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut2);
          }
        }
        if (x < horBottomWidth)
        {
          for (y = 0; y < horBottomHeight; y++)
          {
            pdvecOut1 = (xb_vec2Nx8U *)(pDst1 + (y * dstDataPitch1) + x);
            pdvecOut2 = (xb_vec2Nx8U *)(pDst2 + (y * dstDataPitch1) + x);
            IVP_SAV2NX8U_XP(value1, vaOutData1, pdvecOut1, (horBottomWidth - x));
            IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);

            IVP_SAV2NX8U_XP(value2, vaOutData1, pdvecOut2, (horBottomWidth - x));
            IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut2);
          }
        }
      }
    }
    if(z < dim3Size)
    {
      pDst = (int8_t *)XI_TILE3D_GET_DATA_PTR(dstTile) + (z * dstDataPitch2);
      const int8_t value = pValue[z];

      // horizontal top
      pDst1 = (uint8_t *)pDst + ((horTopYcord * dstDataPitch1) + horTopXcord);
      x = 0;
      if (horTopHeight > 0)
      {
        for (; x < horTopWidth - (2 * XCHAL_IVPN_SIMD_WIDTH); x += 4 * XCHAL_IVPN_SIMD_WIDTH)
        {
          for (y = 0; y < horTopHeight; y++)
          {
            pdvecOut1 = (xb_vec2Nx8U *)(pDst1 + (y * dstDataPitch1) + x);
            IVP_SA2NX8U_IP(value, vaOutData1, pdvecOut1);
            IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut1,
              (horTopWidth - x - 2 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);
          }
        }
        if (x < horTopWidth)
        {
          for (y = 0; y < horTopHeight; y++)
          {
            pdvecOut1 = (xb_vec2Nx8U *)(pDst1 + (y * dstDataPitch1) + x);
            IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut1, (horTopWidth - x));
            IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);
          }
        }
      } //if( horTopHeight > 0)

      // horizontal bottom
      pDst1 = (uint8_t *)pDst + ((horBottomYcord * dstDataPitch1) + horBottomXcord);
      x = 0;
      if (horBottomHeight > 0)
      {
        for (; x < horBottomWidth - (2 * XCHAL_IVPN_SIMD_WIDTH); x += 4 * XCHAL_IVPN_SIMD_WIDTH)
        {
          for (y = 0; y < horBottomHeight; y++)
          {
            pdvecOut1 = (xb_vec2Nx8U *)(pDst1 + (y * dstDataPitch1) + x);
            IVP_SA2NX8U_IP(value, vaOutData1, pdvecOut1);
            IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut1,
              (horBottomWidth - x - 2 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);
          }
        }
        if (x < horBottomWidth)
        {
          for (y = 0; y < horBottomHeight; y++)
          {
            pdvecOut1 = (xb_vec2Nx8U *)(pDst1 + (y * dstDataPitch1) + x);
            IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut1, (horBottomWidth - x));
            IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);
          }
        }
      }
    }
  }

  for(z = 0; z < dim3Size - 1; z+=2)
  {
    const int8_t value1 = pValue[z];
    const int8_t value2 = pValue[z + 1];
    // vertical left
    pDst1 = (uint8_t *)pDst3D + (z * dstDataPitch2) + \
      ((verLeftYcord * dstDataPitch1) + verLeftXcord);
    pDst2 = (uint8_t *)pDst3D + ((z+1) * dstDataPitch2) + \
      ((verLeftYcord * dstDataPitch1) + verLeftXcord);
    x = 0;
    for (; x < verLeftWidth; x += 2 * XCHAL_IVPN_SIMD_WIDTH)
    {
      for (y = 0; y < verLeftHeight; y++)
      {
        pdvecOut1 = (xb_vec2Nx8U *)(pDst1 + (y * dstDataPitch1) + x);
        pdvecOut2 = (xb_vec2Nx8U *)(pDst2 + (y * dstDataPitch1) + x);
        IVP_SAV2NX8U_XP(value1, vaOutData1, pdvecOut1, (verLeftWidth - x));
        IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);
        IVP_SAV2NX8U_XP(value2, vaOutData1, pdvecOut2, (verLeftWidth - x));
        IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut2);
      }
    }

    // vertical right
    pDst1 = (uint8_t *)pDst3D + (z * dstDataPitch2) + \
      ((verRightYcord * dstDataPitch1) + verRightXcord);
    pDst2 = (uint8_t *)pDst3D + ((z+1) * dstDataPitch2) + \
      ((verRightYcord * dstDataPitch1) + verRightXcord);
    x = 0;
    for (; x < verRightWidth; x += 2 * XCHAL_IVPN_SIMD_WIDTH)
    {
      for (y = 0; y < verRightHeight; y++)
      {
        pdvecOut1 = (xb_vec2Nx8U *)(pDst1 + (y * dstDataPitch1) + x);
        pdvecOut2 = (xb_vec2Nx8U *)(pDst2 + (y * dstDataPitch1) + x);
        IVP_SAV2NX8U_XP(value1, vaOutData1, pdvecOut1, (verRightWidth - x));
        IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);
        IVP_SAV2NX8U_XP(value2, vaOutData1, pdvecOut2, (verRightWidth - x));
        IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut2);
      }
    }
  }
  if(z < dim3Size)
  {
    pDst = (int8_t *)XI_TILE3D_GET_DATA_PTR(dstTile) + (z * dstDataPitch2);
    const int8_t value = pValue[z];
    // vertical left
    pDst1 = (uint8_t *)pDst + ((verLeftYcord * dstDataPitch1) + verLeftXcord);
    x = 0;
    for (; x < verLeftWidth; x += 2 * XCHAL_IVPN_SIMD_WIDTH)
    {
      for (y = 0; y < verLeftHeight; y++)
      {
        pdvecOut1 = (xb_vec2Nx8U *)(pDst1 + (y * dstDataPitch1) + x);
        IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut1, (verLeftWidth - x));
        IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);
      }
    }

    // vertical right
    pDst1 = (uint8_t *)pDst + ((verRightYcord * dstDataPitch1) + verRightXcord);
    x = 0;
    for (; x < verRightWidth; x += 2 * XCHAL_IVPN_SIMD_WIDTH)
    {
      for (y = 0; y < verRightHeight; y++)
      {
        pdvecOut1 = (xb_vec2Nx8U *)(pDst1 + (y * dstDataPitch1) + x);
        IVP_SAV2NX8U_XP(value, vaOutData1, pdvecOut1, (verRightWidth - x));
        IVP_SAV2NX8UPOS_FP(vaOutData1, pdvecOut1);
      }
    }
  }
}

/*************************** extendEdgesConst3D_I16 *************************/
/* Description : P6 implementation for extending the edges of a 3D tile     */
/*               with a constant value. This function extends edges across  */
/*               dimension 1 & dimension2 of  a 3D tile                     */
/* Inputs      : constant value to fill the edges                           */
/* Outputs     : XI Error Code                                              */
/* InOuts      : Destination Tile                                           */
/* Assumptions : dstData is signed/unsigned 16bit                            */
/****************************************************************************/
static void extendEdgesConst3D_I16(xi_pTile3D dstTile,
                                   const int32_t constValue,
                                   xi_size3D frame3DSize)
{
  /* Getting parameters from the tile structures */
  const int32_t dim1Size = XI_TILE3D_GET_DIM1(dstTile);
  const int32_t dim2Size = XI_TILE3D_GET_DIM2(dstTile);
  const int32_t dim1Edge1 = XI_TILE3D_GET_DIM1_EDGE1(dstTile);
  const int32_t dim1Edge2 = XI_TILE3D_GET_DIM1_EDGE2(dstTile);
  const int32_t dim2Edge1 = XI_TILE3D_GET_DIM2_EDGE1(dstTile);
  const int32_t dim2Edge2 = XI_TILE3D_GET_DIM2_EDGE2(dstTile);
  int32_t dim3Size =  XI_TILE3D_GET_DIM3(dstTile);

  const int32_t dstDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(dstTile);
  const int32_t dstDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(dstTile);
  int32_t frame_dim1 = frame3DSize.dim1Size;
  int32_t frame_dim2 = frame3DSize.dim2Size;
  int32_t dim1ExtendEdgeSize = dim1Size + dim1Edge1 + dim1Edge2;
  int32_t dim2ExtendEdgeSize = (dim2Size + dim2Edge1 + dim2Edge2) * dstDataPitch1;

  int32_t start_x = XI_TILE3D_GET_DIM1_COORD(dstTile);
  int32_t start_y = XI_TILE3D_GET_DIM2_COORD(dstTile);

  int16_t *restrict pDst3D = (int16_t *)XI_TILE3D_GET_DATA_PTR(dstTile);
  int32_t ixmin = MAX2(start_x - dim1Edge1, 0);
  int32_t ixmax = MIN2(start_x + dim1Size + dim1Edge2 - 1, frame_dim1 - 1);
  int32_t iymin = MAX2(start_y - dim2Edge1, 0);
  int32_t iymax = MIN2(start_y + dim2Size + dim2Edge2 - 1, frame_dim2 - 1);


  int x, y, z; /* Loop variables */
  const int16_t value = constValue;
  // horizontal top
  int32_t horTopXcord = -dim1Edge1;
  int32_t horTopYcord = -dim2Edge1;
  int32_t horTopWidth = dim1Size + dim1Edge1 + dim1Edge2;
  int32_t horTopHeight = iymin - (start_y - dim2Edge1);

  // horizontal bottom
  int32_t horBottomXcord = -dim1Edge1;
  int32_t horBottomYcord = iymax + 1 - start_y;
  int32_t horBottomWidth = dim1Size + dim1Edge1 + dim1Edge2;
  int32_t horBottomHeight = start_y + dim2Size + dim2Edge2 - 1 - iymax;

  // vertical left
  int32_t  verLeftXcord = -dim1Edge1;
  int32_t  verLeftYcord = horTopYcord + horTopHeight;
  int32_t verLeftWidth = ixmin - (start_x - dim1Edge1);
  int32_t verLeftHeight = iymax - iymin + 1;

  // vertical right
  int32_t verRightXcord = ixmax + 1 - start_x;
  int32_t verRightYcord = horTopYcord + horTopHeight;
  int32_t verRightWidth = start_x + dim1Size + dim1Edge2 - 1 - ixmax;
  int32_t verRightHeight = iymax - iymin + 1;

  valign vaOutData1 = IVP_ZALIGN();

  xb_vecNx16 *restrict pdvecOut1, *restrict pdvecOut2;
  int16_t *restrict pDst, *restrict pDst1, *restrict pDst2;
  /* Most optimal case is when -
     i. dim1 (including edges) has no extra padding
     ii. Each plane, i.e. dim1 * dim2 (including edges in both dimensions) has no extra padding
  */
  if ((dstDataPitch1 == dim1ExtendEdgeSize) && (dstDataPitch2 == dim2ExtendEdgeSize))
  {
    int numIter =  horTopWidth * horTopHeight + horBottomWidth * horBottomHeight;

    // horizontal top first plane
    if (horTopHeight > 0)
    {
      pDst1 = (int16_t *)pDst3D + \
        ((horTopYcord * dstDataPitch1) + horTopXcord);
      x = 0;
      pdvecOut1 = (xb_vecNx16 *)(pDst1);
      for (; x < horTopWidth * horTopHeight - XCHAL_IVPN_SIMD_WIDTH; \
        x += XCHAL_IVPN_SIMD_WIDTH)
      {
        IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
      }

      IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
        sizeof(int16_t) * (horTopWidth * horTopHeight - x));
      IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);
    } //if( horTopHeight > 0)

    z = 0;
    if(dim3Size > 1)
    {
      for(; z < dim3Size - 1; z++)
      {
        pDst1 = (int16_t *)pDst3D + (z * dstDataPitch2) + \
          ((horBottomYcord * dstDataPitch1) + horBottomXcord);

        pdvecOut1 = (xb_vecNx16 *)(pDst1);
        for (x = 0; x < numIter - XCHAL_IVPN_SIMD_WIDTH; \
          x += XCHAL_IVPN_SIMD_WIDTH)
        {
          IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
        }
        IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
          sizeof(int16_t) * (numIter - x));
        IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);
      }
    }

    // horizontal bottom last plane
    if (horBottomHeight > 0)
    {
      pDst1 = (int16_t *)pDst3D + (z * dstDataPitch2) + \
        ((horBottomYcord * dstDataPitch1) + horBottomXcord);
      x = 0;
      pdvecOut1 = (xb_vecNx16 *)(pDst1);
      for (; x < horBottomWidth * horBottomHeight - XCHAL_IVPN_SIMD_WIDTH; \
        x += XCHAL_IVPN_SIMD_WIDTH)
      {
        IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
      }
      IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
        sizeof(int16_t) * (horBottomWidth * horBottomHeight - x));
      IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);
    }
  }
  else
  {
    for(z = 0; z < dim3Size - 1; z+=2)
    {
      // horizontal top
      pDst1 = (int16_t *)pDst3D + (z * dstDataPitch2) + \
        ((horTopYcord * dstDataPitch1) + horTopXcord);
      pDst2 = (int16_t *)pDst3D + ((z+1) * dstDataPitch2) + \
        ((horTopYcord * dstDataPitch1) + horTopXcord);
      x = 0;
      if (horTopHeight > 0)
      {
        for (; x < horTopWidth - (3 * XCHAL_IVPN_SIMD_WIDTH); x += 4 * XCHAL_IVPN_SIMD_WIDTH)
        {
          for (y = 0; y < horTopHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            pdvecOut2 = (xb_vecNx16 *)(pDst2 + (y * dstDataPitch1) + x);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horTopWidth - x - 3 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);

            IVP_SANX16_IP(value, vaOutData1, pdvecOut2);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut2);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut2);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut2,
              sizeof(int16_t) * (horTopWidth - x - 3 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut2);
          }
        }
        if (x < (horTopWidth - 2 * XCHAL_IVPN_SIMD_WIDTH))
        {
          for (y = 0; y < horTopHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            pdvecOut2 = (xb_vecNx16 *)(pDst2 + (y * dstDataPitch1) + x);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horTopWidth - x - 2 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);

            IVP_SANX16_IP(value, vaOutData1, pdvecOut2);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut2);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut2,
              sizeof(int16_t) * (horTopWidth - x - 2 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut2);
          }
        }
        if (x < (horTopWidth - XCHAL_IVPN_SIMD_WIDTH))
        {
          for (y = 0; y < horTopHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            pdvecOut2 = (xb_vecNx16 *)(pDst2 + (y * dstDataPitch1) + x);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horTopWidth - x - XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);

            IVP_SANX16_IP(value, vaOutData1, pdvecOut2);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut2,
              sizeof(int16_t) * (horTopWidth - x - XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut2);
          }
        }
        if (x < horTopWidth)
        {
          for (y = 0; y < horTopHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            pdvecOut2 = (xb_vecNx16 *)(pDst2 + (y * dstDataPitch1) + x);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horTopWidth - x));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);

            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut2,
              sizeof(int16_t) * (horTopWidth - x));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut2);
          }
        }
      } //if( horTopHeight > 0)

      // horizontal bottom
      pDst1 = (int16_t *)pDst3D + (z * dstDataPitch2) + \
        ((horBottomYcord * dstDataPitch1) + horBottomXcord);
      pDst2 = (int16_t *)pDst3D + ((z+1) * dstDataPitch2) + \
        ((horBottomYcord * dstDataPitch1) + horBottomXcord);
      x = 0;
      if (horBottomHeight > 0)
      {
        for (; x < horBottomWidth - (3 * XCHAL_IVPN_SIMD_WIDTH); x += 4 * XCHAL_IVPN_SIMD_WIDTH)
        {
          for (y = 0; y < horBottomHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            pdvecOut2 = (xb_vecNx16 *)(pDst2 + (y * dstDataPitch1) + x);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horBottomWidth - x - 3 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);

            IVP_SANX16_IP(value, vaOutData1, pdvecOut2);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut2);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut2);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut2,
              sizeof(int16_t) * (horBottomWidth - x - 3 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut2);
          }
        }
        if (x < (horBottomWidth - 2 * XCHAL_IVPN_SIMD_WIDTH))
        {
          for (y = 0; y < horBottomHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            pdvecOut2 = (xb_vecNx16 *)(pDst2 + (y * dstDataPitch1) + x);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horBottomWidth - x - 2 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);

            IVP_SANX16_IP(value, vaOutData1, pdvecOut2);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut2);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut2,
              sizeof(int16_t) * (horBottomWidth - x - 2 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut2);
          }
        }
        if (x < (horBottomWidth - XCHAL_IVPN_SIMD_WIDTH))
        {
          for (y = 0; y < horBottomHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            pdvecOut2 = (xb_vecNx16 *)(pDst2 + (y * dstDataPitch1) + x);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horBottomWidth - x - XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);

            IVP_SANX16_IP(value, vaOutData1, pdvecOut2);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut2,
              sizeof(int16_t) * (horBottomWidth - x - XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut2);
          }
        }
        if (x < horBottomWidth)
        {
          for (y = 0; y < horBottomHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            pdvecOut2 = (xb_vecNx16 *)(pDst2 + (y * dstDataPitch1) + x);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horBottomWidth - x));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);

            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut2,
              sizeof(int16_t) * (horBottomWidth - x));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut2);
          }
        }
      }
    }
    if(z < dim3Size)
    {
      pDst = pDst3D + (z * dstDataPitch2);

      // horizontal top
      pDst1 = pDst + ((horTopYcord * dstDataPitch1) + horTopXcord);
      x = 0;
      if (horTopHeight > 0)
      {
        for (; x < horTopWidth - (3 * XCHAL_IVPN_SIMD_WIDTH); x += 4 * XCHAL_IVPN_SIMD_WIDTH)
        {
          for (y = 0; y < horTopHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horTopWidth - x - 3 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);
          }
        }
        if (x < (horTopWidth - 2 * XCHAL_IVPN_SIMD_WIDTH))
        {
          for (y = 0; y < horTopHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horTopWidth - x - 2 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);
          }
        }
        if (x < (horTopWidth - XCHAL_IVPN_SIMD_WIDTH))
        {
          for (y = 0; y < horTopHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horTopWidth - x - XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);
          }
        }
        if (x < horTopWidth)
        {
          for (y = 0; y < horTopHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horTopWidth - x));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);
          }
        }
      } //if( horTopHeight > 0)

      // horizontal bottom
      pDst1 = (int16_t *)pDst + ((horBottomYcord * dstDataPitch1) + horBottomXcord);
      x = 0;
      if (horBottomHeight > 0)
      {
        for (; x < horBottomWidth - (3 * XCHAL_IVPN_SIMD_WIDTH); x += 4 * XCHAL_IVPN_SIMD_WIDTH)
        {
          for (y = 0; y < horBottomHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horBottomWidth - x - 3 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);
          }
        }
        if (x < (horBottomWidth - 2 * XCHAL_IVPN_SIMD_WIDTH))
        {
          for (y = 0; y < horBottomHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horBottomWidth - x - 2 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);
          }
        }
        if (x < (horBottomWidth - XCHAL_IVPN_SIMD_WIDTH))
        {
          for (y = 0; y < horBottomHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horBottomWidth - x - XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);
          }
        }
        if (x < horBottomWidth)
        {
          for (y = 0; y < horBottomHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horBottomWidth - x));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);
          }
        }
      }
    }
  }
  for(z = 0; z < dim3Size - 1; z+=2)
  {
    // vertical left
    pDst1 = (int16_t *)pDst3D + (z * dstDataPitch2) + \
      ((verLeftYcord * dstDataPitch1) + verLeftXcord);
    pDst2 = (int16_t *)pDst3D + ((z+1) * dstDataPitch2) + \
      ((verLeftYcord * dstDataPitch1) + verLeftXcord);
    x = 0;
    for (; x < verLeftWidth; x += XCHAL_IVPN_SIMD_WIDTH)
    {
      for (y = 0; y < verLeftHeight; y++)
      {
        pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
        pdvecOut2 = (xb_vecNx16 *)(pDst2 + (y * dstDataPitch1) + x);
        IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1, sizeof(int16_t) * (verLeftWidth - x));
        IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);

        IVP_SAVNX16_XP(value, vaOutData1, pdvecOut2, sizeof(int16_t) * (verLeftWidth - x));
        IVP_SAVNX16POS_FP(vaOutData1, pdvecOut2);
      }
    }

    // vertical right

    pDst1 = (int16_t *)pDst3D + (z * dstDataPitch2) + \
      ((verRightYcord * dstDataPitch1) + verRightXcord);
    pDst2 = (int16_t *)pDst3D + ((z+1) * dstDataPitch2) + \
      ((verRightYcord * dstDataPitch1) + verRightXcord);
    x = 0;
    for (; x < verRightWidth; x += XCHAL_IVPN_SIMD_WIDTH)
    {
      for (y = 0; y < verRightHeight; y++)
      {
        pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
        pdvecOut2 = (xb_vecNx16 *)(pDst2 + (y * dstDataPitch1) + x);
        IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1, sizeof(int16_t) * (verRightWidth - x));
        IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);

        IVP_SAVNX16_XP(value, vaOutData1, pdvecOut2, sizeof(int16_t) * (verRightWidth - x));
        IVP_SAVNX16POS_FP(vaOutData1, pdvecOut2);
      }
    }
  }
  if(z < dim3Size)
  {
    pDst = pDst3D + (z * dstDataPitch2);
    // vertical left
    pDst1 = (int16_t *)pDst + ((verLeftYcord * dstDataPitch1) + verLeftXcord);
    x = 0;
    for (; x < verLeftWidth; x += XCHAL_IVPN_SIMD_WIDTH)
    {
      for (y = 0; y < verLeftHeight; y++)
      {
        pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
        IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1, sizeof(int16_t) * (verLeftWidth - x));
        IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);
      }
    }

    // vertical right

    pDst1 = pDst + ((verRightYcord * dstDataPitch1) + verRightXcord);
    x = 0;
    for (; x < verRightWidth; x += XCHAL_IVPN_SIMD_WIDTH)
    {
      for (y = 0; y < verRightHeight; y++)
      {
        pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
        IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1, sizeof(int16_t) * (verRightWidth - x));
        IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);
      }
    }
  }
}

/************************** extendWHEdges3D_I16 *****************************/
/* Description : P6 implementation for extending the edges of a 3D tile     */
/*               by filling different edge values for  different depths and */
/*               extends the edges along dimension 1(W) and dimension 2(H)  */
/*               3D tile                                                    */
/* Inputs      : pValue(array of edge values)                               */
/* Outputs     : XI Error Code                                              */
/* InOuts      : Destination Tile                                           */
/* Assumptions : dstData is signed/unsigned 16bit                           */
/****************************************************************************/
static void extendWHEdges3D_I16(xi_pTile3D dstTile,
                                const int16_t * pValue,
                                xi_size3D frame3DSize)
{
  /* Getting parameters from the tile structures */
  const int32_t dim1Size = XI_TILE3D_GET_DIM1(dstTile);
  const int32_t dim2Size = XI_TILE3D_GET_DIM2(dstTile);
  const int32_t dim1Edge1 = XI_TILE3D_GET_DIM1_EDGE1(dstTile);
  const int32_t dim1Edge2 = XI_TILE3D_GET_DIM1_EDGE2(dstTile);
  const int32_t dim2Edge1 = XI_TILE3D_GET_DIM2_EDGE1(dstTile);
  const int32_t dim2Edge2 = XI_TILE3D_GET_DIM2_EDGE2(dstTile);
  int32_t dim3Size =  XI_TILE3D_GET_DIM3(dstTile);

  const int32_t dstDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(dstTile);
  const int32_t dstDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(dstTile);
  int32_t frame_dim1 = frame3DSize.dim1Size;
  int32_t frame_dim2 = frame3DSize.dim2Size;
  int32_t dim1ExtendEdgeSize = dim1Size + dim1Edge1 + dim1Edge2;

  int32_t start_x = XI_TILE3D_GET_DIM1_COORD(dstTile);
  int32_t start_y = XI_TILE3D_GET_DIM2_COORD(dstTile);

  int16_t *restrict pDst3D = (int16_t *)XI_TILE3D_GET_DATA_PTR(dstTile);
  int32_t ixmin = MAX2(start_x - dim1Edge1, 0);
  int32_t ixmax = MIN2(start_x + dim1Size + dim1Edge2 - 1, frame_dim1 - 1);
  int32_t iymin = MAX2(start_y - dim2Edge1, 0);
  int32_t iymax = MIN2(start_y + dim2Size + dim2Edge2 - 1, frame_dim2 - 1);

  int x, y, z; /* Loop variables */

  // horizontal top
  int32_t horTopXcord = -dim1Edge1;
  int32_t horTopYcord = -dim2Edge1;
  int32_t horTopWidth = dim1Size + dim1Edge1 + dim1Edge2;
  int32_t horTopHeight = iymin - (start_y - dim2Edge1);

  // horizontal bottom
  int32_t horBottomXcord = -dim1Edge1;
  int32_t horBottomYcord = iymax + 1 - start_y;
  int32_t horBottomWidth = dim1Size + dim1Edge1 + dim1Edge2;
  int32_t horBottomHeight = start_y + dim2Size + dim2Edge2 - 1 - iymax;

  // vertical left
  int32_t  verLeftXcord = -dim1Edge1;
  int32_t  verLeftYcord = horTopYcord + horTopHeight;
  int32_t verLeftWidth = ixmin - (start_x - dim1Edge1);
  int32_t verLeftHeight = iymax - iymin + 1;

  // vertical right
  int32_t verRightXcord = ixmax + 1 - start_x;
  int32_t verRightYcord = horTopYcord + horTopHeight;
  int32_t verRightWidth = start_x + dim1Size + dim1Edge2 - 1 - ixmax;
  int32_t verRightHeight = iymax - iymin + 1;

  valign vaOutData1 = IVP_ZALIGN();
  valign vaOutData2 = IVP_ZALIGN();

  xb_vecNx16 *restrict pdvecOut1, *restrict pdvecOut2;
  int16_t *restrict pDst, *restrict pDst1, *restrict pDst2;

  if(dstDataPitch1 == dim1ExtendEdgeSize)
  {
    for(z = 0; z < dim3Size - 1; z+=2)
    {
      const int16_t value1 = pValue[z];
      const int16_t value2 = pValue[z + 1];

      // horizontal top
      pDst1 = (int16_t *)pDst3D + (z * dstDataPitch2) + \
        ((horTopYcord * dstDataPitch1) + horTopXcord);
      pDst2 = (int16_t *)pDst3D + ((z+1) * dstDataPitch2) + \
        ((horTopYcord * dstDataPitch1) + horTopXcord);
      x = 0;
      if (horTopHeight > 0)
      {
        pdvecOut1 = (xb_vecNx16 *)(pDst1);
        pdvecOut2 = (xb_vecNx16 *)(pDst2);
        for (; x < horTopWidth * horTopHeight - (XCHAL_IVPN_SIMD_WIDTH); x += XCHAL_IVPN_SIMD_WIDTH)
        {
          IVP_SANX16_IP(value1, vaOutData1, pdvecOut1);
          IVP_SANX16_IP(value2, vaOutData2, pdvecOut2);
        }
        IVP_SAVNX16_XP(value1, vaOutData1, pdvecOut1,
            sizeof(int16_t) * (horTopWidth * horTopHeight - x));
        IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);

        IVP_SAVNX16_XP(value2, vaOutData2, pdvecOut2,
            sizeof(int16_t) * (horTopWidth * horTopHeight - x));
        IVP_SAVNX16POS_FP(vaOutData2, pdvecOut2);
      } //if( horTopHeight > 0)

      // horizontal bottom
      pDst1 = (int16_t *)pDst3D + (z * dstDataPitch2) + \
        ((horBottomYcord * dstDataPitch1) + horBottomXcord);
      pDst2 = (int16_t *)pDst3D + ((z+1) * dstDataPitch2) + \
        ((horBottomYcord * dstDataPitch1) + horBottomXcord);
      x = 0;
      if (horBottomHeight > 0)
      {
        pdvecOut1 = (xb_vecNx16 *)(pDst1);
        pdvecOut2 = (xb_vecNx16 *)(pDst2);
        for (; x < horBottomWidth * horBottomHeight - XCHAL_IVPN_SIMD_WIDTH; \
          x += XCHAL_IVPN_SIMD_WIDTH)
        {
          IVP_SANX16_IP(value1, vaOutData1, pdvecOut1);
          IVP_SANX16_IP(value2, vaOutData2, pdvecOut2);
        }
        IVP_SAVNX16_XP(value1, vaOutData1, pdvecOut1,
            sizeof(int16_t) * (horBottomWidth * horBottomHeight - x));
        IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);

        IVP_SAVNX16_XP(value2, vaOutData2, pdvecOut2,
            sizeof(int16_t) * (horBottomWidth * horBottomHeight - x));
        IVP_SAVNX16POS_FP(vaOutData2, pdvecOut2);
      }
    }
    if(z < dim3Size)
    {
      pDst = pDst3D + (z * dstDataPitch2);
      const int16_t value =  pValue[z];

      // horizontal top
      pDst1 = pDst + ((horTopYcord * dstDataPitch1) + horTopXcord);
      x = 0;
      if (horTopHeight > 0)
      {
        pdvecOut1 = (xb_vecNx16 *)(pDst1);
        for (; x < horTopWidth * horTopHeight - (XCHAL_IVPN_SIMD_WIDTH); \
          x += XCHAL_IVPN_SIMD_WIDTH)
        {
          IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
          IVP_SANX16POS_FP(vaOutData1, pdvecOut1);
        }
        for (; x < horTopWidth * horTopHeight; x += XCHAL_IVPN_SIMD_WIDTH)
        {
          IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
            sizeof(int16_t) * (horTopWidth * horTopHeight - x));
          IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);
        }
      } //if( horTopHeight > 0)

      // horizontal bottom
      pDst1 = (int16_t *)pDst + ((horBottomYcord * dstDataPitch1) + horBottomXcord);
      x = 0;
      if (horBottomHeight > 0)
      {
        pdvecOut1 = (xb_vecNx16 *)(pDst1);
        for (; x < horBottomWidth * horBottomHeight - XCHAL_IVPN_SIMD_WIDTH; \
          x += XCHAL_IVPN_SIMD_WIDTH)
        {
          IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
          IVP_SANX16POS_FP(vaOutData1, pdvecOut1);
        }
        for (; x < horBottomWidth * horBottomHeight; x += XCHAL_IVPN_SIMD_WIDTH)
        {
          IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
            sizeof(int16_t) * (horBottomWidth * horBottomHeight - x));
          IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);
        }
      }
    }
  }
  else
  {
    for(z = 0; z < dim3Size - 1; z+=2)
    {
      const int16_t value1 = pValue[z];
      const int16_t value2 = pValue[z + 1];

      // horizontal top
      pDst1 = (int16_t *)pDst3D + (z * dstDataPitch2) + \
        ((horTopYcord * dstDataPitch1) + horTopXcord);
      pDst2 = (int16_t *)pDst3D + ((z+1) * dstDataPitch2) + \
        ((horTopYcord * dstDataPitch1) + horTopXcord);
      x = 0;
      if (horTopHeight > 0)
      {
        for (; x < horTopWidth - (3 * XCHAL_IVPN_SIMD_WIDTH); x += 4 * XCHAL_IVPN_SIMD_WIDTH)
        {
          for (y = 0; y < horTopHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            pdvecOut2 = (xb_vecNx16 *)(pDst2 + (y * dstDataPitch1) + x);
            IVP_SANX16_IP(value1, vaOutData1, pdvecOut1);
            IVP_SANX16_IP(value1, vaOutData1, pdvecOut1);
            IVP_SANX16_IP(value1, vaOutData1, pdvecOut1);
            IVP_SAVNX16_XP(value1, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horTopWidth - x - 3 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);

            IVP_SANX16_IP(value2, vaOutData1, pdvecOut2);
            IVP_SANX16_IP(value2, vaOutData1, pdvecOut2);
            IVP_SANX16_IP(value2, vaOutData1, pdvecOut2);
            IVP_SAVNX16_XP(value2, vaOutData1, pdvecOut2,
              sizeof(int16_t) * (horTopWidth - x - 3 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut2);
          }
        }
        if (x < (horTopWidth - 2 * XCHAL_IVPN_SIMD_WIDTH))
        {
          for (y = 0; y < horTopHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            pdvecOut2 = (xb_vecNx16 *)(pDst2 + (y * dstDataPitch1) + x);
            IVP_SANX16_IP(value1, vaOutData1, pdvecOut1);
            IVP_SANX16_IP(value1, vaOutData1, pdvecOut1);
            IVP_SAVNX16_XP(value1, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horTopWidth - x - 2 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);

            IVP_SANX16_IP(value2, vaOutData1, pdvecOut2);
            IVP_SANX16_IP(value2, vaOutData1, pdvecOut2);
            IVP_SAVNX16_XP(value2, vaOutData1, pdvecOut2,
              sizeof(int16_t) * (horTopWidth - x - 2 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut2);
          }
        }
        if (x < (horTopWidth - XCHAL_IVPN_SIMD_WIDTH))
        {
          for (y = 0; y < horTopHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            pdvecOut2 = (xb_vecNx16 *)(pDst2 + (y * dstDataPitch1) + x);
            IVP_SANX16_IP(value1, vaOutData1, pdvecOut1);
            IVP_SAVNX16_XP(value1, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horTopWidth - x - XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);

            IVP_SANX16_IP(value2, vaOutData1, pdvecOut2);
            IVP_SAVNX16_XP(value2, vaOutData1, pdvecOut2,
              sizeof(int16_t) * (horTopWidth - x - XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut2);
          }
        }
        if (x < horTopWidth)
        {
          for (y = 0; y < horTopHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            pdvecOut2 = (xb_vecNx16 *)(pDst2 + (y * dstDataPitch1) + x);
            IVP_SAVNX16_XP(value1, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horTopWidth - x));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);

            IVP_SAVNX16_XP(value2, vaOutData1, pdvecOut2,
              sizeof(int16_t) * (horTopWidth - x));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut2);
          }
        }
      } //if( horTopHeight > 0)

      // horizontal bottom
      pDst1 = (int16_t *)pDst3D + (z * dstDataPitch2) + \
        ((horBottomYcord * dstDataPitch1) + horBottomXcord);
      pDst2 = (int16_t *)pDst3D + ((z+1) * dstDataPitch2) + \
        ((horBottomYcord * dstDataPitch1) + horBottomXcord);
      x = 0;
      if (horBottomHeight > 0)
      {
        for (; x < horBottomWidth - (3 * XCHAL_IVPN_SIMD_WIDTH); x += 4 * XCHAL_IVPN_SIMD_WIDTH)
        {
          for (y = 0; y < horBottomHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            pdvecOut2 = (xb_vecNx16 *)(pDst2 + (y * dstDataPitch1) + x);
            IVP_SANX16_IP(value1, vaOutData1, pdvecOut1);
            IVP_SANX16_IP(value1, vaOutData1, pdvecOut1);
            IVP_SANX16_IP(value1, vaOutData1, pdvecOut1);
            IVP_SAVNX16_XP(value1, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horBottomWidth - x - 3 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);

            IVP_SANX16_IP(value2, vaOutData1, pdvecOut2);
            IVP_SANX16_IP(value2, vaOutData1, pdvecOut2);
            IVP_SANX16_IP(value2, vaOutData1, pdvecOut2);
            IVP_SAVNX16_XP(value2, vaOutData1, pdvecOut2,
              sizeof(int16_t) * (horBottomWidth - x - 3 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut2);
          }
        }
        if (x < (horBottomWidth - 2 * XCHAL_IVPN_SIMD_WIDTH))
        {
          for (y = 0; y < horBottomHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            pdvecOut2 = (xb_vecNx16 *)(pDst2 + (y * dstDataPitch1) + x);
            IVP_SANX16_IP(value1, vaOutData1, pdvecOut1);
            IVP_SANX16_IP(value1, vaOutData1, pdvecOut1);
            IVP_SAVNX16_XP(value1, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horBottomWidth - x - 2 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);

            IVP_SANX16_IP(value2, vaOutData1, pdvecOut2);
            IVP_SANX16_IP(value2, vaOutData1, pdvecOut2);
            IVP_SAVNX16_XP(value2, vaOutData1, pdvecOut2,
              sizeof(int16_t) * (horBottomWidth - x - 2 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut2);
          }
        }
        if (x < (horBottomWidth - XCHAL_IVPN_SIMD_WIDTH))
        {
          for (y = 0; y < horBottomHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            pdvecOut2 = (xb_vecNx16 *)(pDst2 + (y * dstDataPitch1) + x);
            IVP_SANX16_IP(value1, vaOutData1, pdvecOut1);
            IVP_SAVNX16_XP(value1, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horBottomWidth - x - XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);

            IVP_SANX16_IP(value2, vaOutData1, pdvecOut2);
            IVP_SAVNX16_XP(value2, vaOutData1, pdvecOut2,
              sizeof(int16_t) * (horBottomWidth - x - XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut2);
          }
        }
        if (x < horBottomWidth)
        {
          for (y = 0; y < horBottomHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            pdvecOut2 = (xb_vecNx16 *)(pDst2 + (y * dstDataPitch1) + x);
            IVP_SAVNX16_XP(value1, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horBottomWidth - x));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);

            IVP_SAVNX16_XP(value2, vaOutData1, pdvecOut2,
              sizeof(int16_t) * (horBottomWidth - x));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut2);
          }
        }
      }
    }
    if(z < dim3Size)
    {
      pDst = pDst3D + (z * dstDataPitch2);
      const int16_t value = pValue[z];

      // horizontal top
      pDst1 = pDst + ((horTopYcord * dstDataPitch1) + horTopXcord);
      x = 0;
      if (horTopHeight > 0)
      {
        for (; x < horTopWidth - (3 * XCHAL_IVPN_SIMD_WIDTH); x += 4 * XCHAL_IVPN_SIMD_WIDTH)
        {
          for (y = 0; y < horTopHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horTopWidth - x - 3 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);
          }
        }
        if (x < (horTopWidth - 2 * XCHAL_IVPN_SIMD_WIDTH))
        {
          for (y = 0; y < horTopHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horTopWidth - x - 2 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);
          }
        }
        if (x < (horTopWidth - XCHAL_IVPN_SIMD_WIDTH))
        {
          for (y = 0; y < horTopHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horTopWidth - x - XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);
          }
        }
        if (x < horTopWidth)
        {
          for (y = 0; y < horTopHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horTopWidth - x));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);
          }
        }
      } //if( horTopHeight > 0)

      // horizontal bottom
      pDst1 = (int16_t *)pDst + ((horBottomYcord * dstDataPitch1) + horBottomXcord);
      x = 0;
      if (horBottomHeight > 0)
      {
        for (; x < horBottomWidth - (3 * XCHAL_IVPN_SIMD_WIDTH); x += 4 * XCHAL_IVPN_SIMD_WIDTH)
        {
          for (y = 0; y < horBottomHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horBottomWidth - x - 3 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);
          }
        }
        if (x < (horBottomWidth - 2 * XCHAL_IVPN_SIMD_WIDTH))
        {
          for (y = 0; y < horBottomHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horBottomWidth - x - 2 * XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);
          }
        }
        if (x < (horBottomWidth - XCHAL_IVPN_SIMD_WIDTH))
        {
          for (y = 0; y < horBottomHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            IVP_SANX16_IP(value, vaOutData1, pdvecOut1);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horBottomWidth - x - XCHAL_IVPN_SIMD_WIDTH));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);
          }
        }
        if (x < horBottomWidth)
        {
          for (y = 0; y < horBottomHeight; y++)
          {
            pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
            IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1,
              sizeof(int16_t) * (horBottomWidth - x));
            IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);
          }
        }
      }
    }
  }
  for(z = 0; z < dim3Size - 1; z+=2)
  {
    const int16_t value1 = pValue[z];
    const int16_t value2 = pValue[z + 1];

    // vertical left
    pDst1 = (int16_t *)pDst3D + (z * dstDataPitch2) + \
      ((verLeftYcord * dstDataPitch1) + verLeftXcord);
    pDst2 = (int16_t *)pDst3D + ((z+1) * dstDataPitch2) + \
      ((verLeftYcord * dstDataPitch1) + verLeftXcord);
    x = 0;
    for (; x < verLeftWidth; x += XCHAL_IVPN_SIMD_WIDTH)
    {
      for (y = 0; y < verLeftHeight; y++)
      {
        pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
        pdvecOut2 = (xb_vecNx16 *)(pDst2 + (y * dstDataPitch1) + x);
        IVP_SAVNX16_XP(value1, vaOutData1, pdvecOut1, sizeof(int16_t) * (verLeftWidth - x));
        IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);

        IVP_SAVNX16_XP(value2, vaOutData1, pdvecOut2, sizeof(int16_t) * (verLeftWidth - x));
        IVP_SAVNX16POS_FP(vaOutData1, pdvecOut2);
      }
    }

    // vertical right
    pDst1 = (int16_t *)pDst3D + (z * dstDataPitch2) + \
      ((verRightYcord * dstDataPitch1) + verRightXcord);
    pDst2 = (int16_t *)pDst3D + ((z+1) * dstDataPitch2) + \
      ((verRightYcord * dstDataPitch1) + verRightXcord);
    x = 0;
    for (; x < verRightWidth; x += XCHAL_IVPN_SIMD_WIDTH)
    {
      for (y = 0; y < verRightHeight; y++)
      {
        pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
        pdvecOut2 = (xb_vecNx16 *)(pDst2 + (y * dstDataPitch1) + x);
        IVP_SAVNX16_XP(value1, vaOutData1, pdvecOut1, sizeof(int16_t) * (verRightWidth - x));
        IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);

        IVP_SAVNX16_XP(value2, vaOutData1, pdvecOut2, sizeof(int16_t) * (verRightWidth - x));
        IVP_SAVNX16POS_FP(vaOutData1, pdvecOut2);
      }
    }
  }
  if(z < dim3Size)
  {
    pDst = pDst3D + (z * dstDataPitch2);
    const int16_t value =  pValue[z];

    // vertical left
    pDst1 = (int16_t *)pDst + ((verLeftYcord * dstDataPitch1) + verLeftXcord);
    x = 0;
    for (; x < verLeftWidth; x += XCHAL_IVPN_SIMD_WIDTH)
    {
      for (y = 0; y < verLeftHeight; y++)
      {
        pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
        IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1, sizeof(int16_t) * (verLeftWidth - x));
        IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);
      }
    }

    // vertical right
    pDst1 = pDst + ((verRightYcord * dstDataPitch1) + verRightXcord);
    x = 0;
    for (; x < verRightWidth; x += XCHAL_IVPN_SIMD_WIDTH)
    {
      for (y = 0; y < verRightHeight; y++)
      {
        pdvecOut1 = (xb_vecNx16 *)(pDst1 + (y * dstDataPitch1) + x);
        IVP_SAVNX16_XP(value, vaOutData1, pdvecOut1, sizeof(int16_t) * (verRightWidth - x));
        IVP_SAVNX16POS_FP(vaOutData1, pdvecOut1);
      }
    }
  }
}


/************************** xiCopyTile3D  ***********************************/
/* Description : P6 optimized implementation for copying the contents of a  */
/*               3D tile to another 3D tile. This function supports copying */
/*               of 8bit and 16 bit input tile data based on data type of   */
/*               tile data elements. copy_edge_extension flag is used to    */
/*               control copy of edges. If edge sizes are different, then   */
/*               minimum of input & output edge size number of elements is  */
/*               copied from edges.                                         */
/* Inputs      : Input Tile data, copy_edge_extension,                      */
/* Outputs     : XI Error Code                                              */
/* InOuts      : Output Tile                                                */
/* Assumptions : InData is integer 8bit / 16 bit value                      */
/*               Active data size of input & output tiles are the same      */
/****************************************************************************/

XI_ERR_TYPE xiCopyTile3D(const xi_pTile3D inTile,
                            xi_pTile3D outTile,
                            xi_bool copy_edge_extension)
{
  /* Error Checks */
  XI_ERROR_CHECKS()
  {
    XI_CHECK_TILE3D(inTile);
    XI_CHECK_TILE3D(outTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(inTile, outTile);
    XI_CHECK_ERROR(((XI_TILE3D_GET_ELEMENT_SIZE(inTile) == 1) || (XI_TILE3D_GET_ELEMENT_SIZE(inTile) == 2)),
                     XI_ERR_DATATYPE, "\nThe argument of input tile has unsupported data type");
    XI_CHECK_TILE3D_ELEMENT_SIZE_EQ(inTile, outTile);
    XI_CHECK_TILE3D_EQUAL(inTile, outTile);
    XI_CHECK_ERROR(XI_TILE3D_GET_DATA_ORDER(inTile) == XI_TILE3D_GET_DATA_ORDER(outTile), XI_ERR_BADARG,\
                    "\nData Order of InputTile = %d OutputTile = %d\nData Order of InputTile and OutputTile should be same", \
                    XI_TILE3D_GET_DATA_ORDER(inTile), XI_TILE3D_GET_DATA_ORDER(outTile));
  }

  /* Getting parameters from the tile structures                               */
  /* Tile size across first dimension of input tile and output tile is scaled  */
  /* based on input data type of tile data elements                            */

  const int32_t element_size  = XI_TILE3D_GET_ELEMENT_SIZE(inTile);
  const int32_t dim1Size      = XI_TILE3D_GET_DIM1(inTile) * element_size;
  const int32_t inDim1Edge1   = XI_TILE3D_GET_DIM1_EDGE1(inTile) * element_size;
  const int32_t inDim1Edge2   = XI_TILE3D_GET_DIM1_EDGE2(inTile) * element_size;
  const int32_t outDim1Edge1  = XI_TILE3D_GET_DIM1_EDGE1(outTile) * element_size;
  const int32_t outDim1Edge2  = XI_TILE3D_GET_DIM1_EDGE2(outTile) * element_size;
  const int32_t inDataPitch1  = XI_TILE3D_GET_DIM1_PITCH(inTile) * element_size;
  const int32_t inDataPitch2  = XI_TILE3D_GET_DIM2_PITCH(inTile) * element_size;
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile) * element_size;
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile) * element_size;
  const int32_t dim2Size      = XI_TILE3D_GET_DIM2(inTile);
  const int32_t dim3Size      = XI_TILE3D_GET_DIM3(inTile);
  const int32_t inDim2Edge1   = XI_TILE3D_GET_DIM2_EDGE1(inTile);
  const int32_t inDim2Edge2   = XI_TILE3D_GET_DIM2_EDGE2(inTile);
  const int32_t inDim3Edge1   = XI_TILE3D_GET_DIM3_EDGE1(inTile);
  const int32_t inDim3Edge2   = XI_TILE3D_GET_DIM3_EDGE2(inTile);
  const int32_t outDim2Edge1  = XI_TILE3D_GET_DIM2_EDGE1(outTile);
  const int32_t outDim2Edge2  = XI_TILE3D_GET_DIM2_EDGE2(outTile);
  const int32_t outDim3Edge1  = XI_TILE3D_GET_DIM3_EDGE1(outTile);
  const int32_t outDim3Edge2  = XI_TILE3D_GET_DIM3_EDGE2(outTile);
  /* Vectorization for xiCopyTile3D function is always done across the first dimension */
  int32_t vectorizationWidth  = 2 * XCHAL_IVPN_SIMD_WIDTH;
  int32_t vectorizationWidth2X = vectorizationWidth * 2;
  int32_t vectorizationWidth3X = vectorizationWidth * 3;
  int32_t vectorizationWidth4X = vectorizationWidth * 4;

  int8_t *pInput  = (int8_t *) XI_TILE3D_GET_DATA_PTR(inTile);
  int8_t *pOutput = (int8_t *) XI_TILE3D_GET_DATA_PTR(outTile);

  int32_t z, x, y;
  int32_t dim1CopySize = dim1Size;
  int32_t dim2CopySize = dim2Size;
  int32_t dim3CopySize = dim3Size;
  int32_t dim1CopyEdge1Size;
  int32_t dim2CopyEdge1Size;
  int32_t dim3CopyEdge1Size;
  int32_t dim1CopyEdge2Size;
  int32_t dim2CopyEdge2Size;
  int32_t dim3CopyEdge2Size;
  int32_t maxLoopCount;
  valign vaInData;
  valign vaOutData = IVP_ZALIGN();
  xb_vec2Nx8* restrict pdvecIn;
  xb_vec2Nx8* restrict pdvecOut;
  xb_vec2Nx8 vecValue;

  /* If copy_edge_extension flag is enabled update input and output data pointer  */
  /* and data copy size across all 3 dimensions.                                 */

  if (copy_edge_extension)
  {
    dim1CopyEdge1Size = XT_MIN(inDim1Edge1, outDim1Edge1);
    dim2CopyEdge1Size = XT_MIN(inDim2Edge1, outDim2Edge1);
    dim3CopyEdge1Size = XT_MIN(inDim3Edge1, outDim3Edge1);
    dim1CopyEdge2Size = XT_MIN(inDim1Edge2, outDim1Edge2);
    dim2CopyEdge2Size = XT_MIN(inDim2Edge2, outDim2Edge2);
    dim3CopyEdge2Size = XT_MIN(inDim3Edge2, outDim3Edge2);
    dim1CopySize      = dim1Size + dim1CopyEdge1Size + dim1CopyEdge2Size;
    dim2CopySize      = dim2Size + dim2CopyEdge1Size + dim2CopyEdge2Size;
    dim3CopySize      = dim3Size + dim3CopyEdge1Size + dim3CopyEdge2Size;
    pInput            = &pInput[-dim1CopyEdge1Size + ((-dim2CopyEdge1Size) * inDataPitch1) \
      + ((-dim3CopyEdge1Size) * inDataPitch2)];
    pOutput           = &pOutput[-dim1CopyEdge1Size + ((-dim2CopyEdge1Size) * outDataPitch1) \
      + ((-dim3CopyEdge1Size) * outDataPitch2)];
  }

  /******************************************************************************/
  /* The overall design approach is split into 2 parts                          */
  /* 1. When output tile pitch is equal to output tile copy size.               */
  /*    - If above condition holds good, memory location to be copied           */
  /*      from inTile to outTile is contiguous. Hence vectorization can be      */
  /*      utilized effectively                                                  */
  /* 2. When output tile pitch is greater than output tile copy size.           */
  /*    - If above condition holds good, memory location to be copied           */
  /*      from inTile to outTile is contiguous. In order to do                  */
  /*      vectorization across first dimension, output data pointers            */
  /*      need to be updated based on output tile copy size and                 */
  /*      output tile pitch                                                     */
  /******************************************************************************/

  if ((inDataPitch1 == dim1CopySize) && (inDataPitch1 == outDataPitch1))
  {
    /* Data to be copied exist in contiguous memory location with respect to */
    /* first dimension                                                       */

    /* Initialize MAX2 loop counter */
    int32_t maxdim3LoopCount = dim3CopySize;
    maxLoopCount = dim1CopySize * dim2CopySize;

    if ((inDataPitch2 == maxLoopCount) && (inDataPitch2 == outDataPitch2))
    {
      /* Data to be filled exist in contiguous memory location with respect to */
      /* first and second dimension                                            */

      /* Update MAX2 loop counter */
      maxdim3LoopCount = 1;
      maxLoopCount    *= dim3CopySize;
    }
    for (z = 0; z < maxdim3LoopCount; z++)
    {
      /* initialize input and output data pointer */
      pdvecIn  = (xb_vec2Nx8 *) (pInput + (z * inDataPitch2));
      pdvecOut = (xb_vec2Nx8 *) (pOutput + (z * outDataPitch2));
      vaInData = IVP_LA2NX8_PP(pdvecIn);
      for (x = 0; x < maxLoopCount - vectorizationWidth; x += vectorizationWidth)
      {
        /* Read vector input data */
        IVP_LA2NX8_IP(vecValue, vaInData, pdvecIn);
        /* Store vector output data */
        IVP_SA2NX8_IP(vecValue, vaOutData, pdvecOut);
      }

      IVP_LAV2NX8_XP(vecValue, vaInData, pdvecIn, maxLoopCount - x);
      IVP_SAV2NX8_XP(vecValue, vaOutData, pdvecOut, maxLoopCount - x);
      IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
    }
  }
  else
  {
    /* else block execute, if output tile pitch is  greater than output tile copy size   */
    /* or input tile pitch in not equal to output tile pitch                             */

    for (z = 0; z < dim3CopySize; z++) /* Loop across dim3 */
    {
      x = 0;
      /* Loop across dimension 1 */

      /* Condition check added to maximize vectorization across dimension 1*/
      /* Loop across dim1 */
      for (; x < (dim1CopySize - vectorizationWidth3X); x += vectorizationWidth4X)
      {
        /* initialize input and output data pointer */
        int8_t *pInput1  = pInput + x + (z * inDataPitch2);
        int8_t *pOutput1 = pOutput + x + (z * outDataPitch2);
        int32_t varLen = dim1CopySize - (x + vectorizationWidth3X);

        for (y = 0; y < dim2CopySize; y++)
        {
          pdvecIn  = (xb_vec2Nx8 *) (pInput1 + (y * inDataPitch1));
          pdvecOut = (xb_vec2Nx8 *) (pOutput1 + (y * outDataPitch1));
          vaInData = IVP_LA2NX8_PP(pdvecIn);

          /* Read vector data from inTile and copy vector data to outTile */
          IVP_LA2NX8_IP(vecValue, vaInData, pdvecIn);
          IVP_SA2NX8_IP(vecValue, vaOutData, pdvecOut);
          IVP_LA2NX8_IP(vecValue, vaInData, pdvecIn);
          IVP_SA2NX8_IP(vecValue, vaOutData, pdvecOut);
          IVP_LA2NX8_IP(vecValue, vaInData, pdvecIn);
          IVP_SA2NX8_IP(vecValue, vaOutData, pdvecOut);
          IVP_LAV2NX8_XP(vecValue, vaInData, pdvecIn, varLen);
          IVP_SAV2NX8_XP(vecValue, vaOutData, pdvecOut, varLen);
          IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        }
      }
      if (x < (dim1CopySize - vectorizationWidth2X)) /* Loop unrolling across dim2 */
      {
        /* initialize input and output data pointer */
        int8_t *pInput1  = pInput + x + (z * inDataPitch2);
        int8_t *pOutput1 = pOutput + x + (z * outDataPitch2);
        int32_t varLen = dim1CopySize - (x + vectorizationWidth2X);
        for (y = 0; y < dim2CopySize; y++)
        {
          pdvecIn  = (xb_vec2Nx8 *) (pInput1 + (y * inDataPitch1));
          pdvecOut = (xb_vec2Nx8 *) (pOutput1 + (y * outDataPitch1));
          vaInData = IVP_LA2NX8_PP(pdvecIn);

          /* Read vector data from inTile and copy vector data to outTile */
          IVP_LA2NX8_IP(vecValue, vaInData, pdvecIn);
          IVP_SA2NX8_IP(vecValue, vaOutData, pdvecOut);
          IVP_LA2NX8_IP(vecValue, vaInData, pdvecIn);
          IVP_SA2NX8_IP(vecValue, vaOutData, pdvecOut);
          IVP_LAV2NX8_XP(vecValue, vaInData, pdvecIn, varLen);
          IVP_SAV2NX8_XP(vecValue, vaOutData, pdvecOut, varLen);
          IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        }
      }
      else if (x < (dim1CopySize - vectorizationWidth))
      {
        /* initialize input and output data pointer */
        int8_t *pInput1  = pInput + x + (z * inDataPitch2);
        int8_t *pOutput1 = pOutput + x + (z * outDataPitch2);
        int32_t varLen = dim1CopySize - (x + vectorizationWidth);
        for (y = 0; y < dim2CopySize; y++)
        {
          pdvecIn  = (xb_vec2Nx8 *) (pInput1 + (y * inDataPitch1));
          pdvecOut = (xb_vec2Nx8 *) (pOutput1 + (y * outDataPitch1));
          vaInData = IVP_LA2NX8_PP(pdvecIn);

          /* Read vector data from inTile and copy vector data to outTile */
          IVP_LA2NX8_IP(vecValue, vaInData, pdvecIn);
          IVP_SA2NX8_IP(vecValue, vaOutData, pdvecOut);
          IVP_LAV2NX8_XP(vecValue, vaInData, pdvecIn, varLen);
          IVP_SAV2NX8_XP(vecValue, vaOutData, pdvecOut, varLen);
          IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        }
      }
      else if (x < dim1CopySize)
      {
        /* initialize input and output data pointer */
        int8_t *pInput1  = pInput + x + (z * inDataPitch2);
        int8_t *pOutput1 = pOutput + x + (z * outDataPitch2);
        int32_t varLen = dim1CopySize - x;
        for (y = 0; y < dim2CopySize; y++)
        {
          pdvecIn  = (xb_vec2Nx8 *) (pInput1 + (y * inDataPitch1));
          pdvecOut = (xb_vec2Nx8 *) (pOutput1 + (y * outDataPitch1));
          vaInData = IVP_LA2NX8_PP(pdvecIn);

          /* Read vector data from inTile and copy vector data */
          IVP_LAV2NX8_XP(vecValue, vaInData, pdvecIn, varLen);
          IVP_SAV2NX8_XP(vecValue, vaOutData, pdvecOut, varLen);
          IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        }
      }
    }
  }
  return(XI_ERROR_STATUS());
}

/***************************** xiTranspose3D ********************************/
/* Description : General API for Transpose3D optimized implementation       */
/*               Calls one of the Transpose3D functions based               */
/*               on the parameters                                          */
/* Inputs      : Input Tile data                                            */
/* Outputs     : XI Error Code                                              */
/* InOuts      : Output Tile                                                */
/****************************************************************************/
XI_ERR_TYPE xiTranspose3D(const xi_pTile3D inTile,
                          xi_pTile3D outTile,
                          const uint8_t typeConvertVar)
{
  if (!inTile)
  {
    return(XI_ERR_NULLARG);
  }
  if (XI_TILE3D_GET_DATA_ORDER(inTile) == XI_WHD)
  {
    return (xiTranspose3D_I8_WHD_DWH(inTile, outTile, typeConvertVar));
  }
  else if (XI_TILE3D_GET_DATA_ORDER(inTile) == XI_DWH)
  {
    return (xiTranspose3D_I8_DWH_WHD(inTile, outTile, typeConvertVar));
  }
  return(XI_ERR_NO_VARIANT);
}

/************************* xiTranspose3D_I8_WHD_DWH *************************/
/* Description : Vision P6 implementation for getting the transpose of a 3D  */
/*               tile from  WHD to DWH format.                              */
/* Inputs      : Input Tile data                                            */
/* Outputs     : XI Error Code                                              */
/* InOuts      : Output Tile                                                */
/* Assumptions : InData is integer 8bit value                               */
/*               InTile is in WHD format & outTile in in DWH format         */
/****************************************************************************/
XI_ERR_TYPE xiTranspose3D_I8_WHD_DWH(const xi_pTile3D inTile,
                                     xi_pTile3D outTile,
                                     const uint8_t typeConvertVar)
{
  /* Error Checks */
  XI_ERROR_CHECKS()
  {
    XI_CHECK_TILE3D_I8(inTile);
    XI_CHECK_TILE3D_I8(outTile);
    XI_CHECK_TILE3D_FITS_IN_SINGLE_DRAM(inTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(inTile, outTile);
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(inTile) == XI_TILE3D_GET_DIM2(outTile) &&\
        XI_TILE3D_GET_DIM2(inTile) == XI_TILE3D_GET_DIM3(outTile) &&\
        XI_TILE3D_GET_DIM3(inTile) == XI_TILE3D_GET_DIM1(outTile), XI_ERR_DATASIZE, \
        "Input and output tiles size are not valid");
    XI_CHECK_ERROR(XI_TILE3D_GET_DATA_ORDER(inTile) == XI_WHD, XI_ERR_BADARG, \
        "Input tile should be in WHD order");
    XI_CHECK_ERROR(XI_TILE3D_GET_DATA_ORDER(outTile) == XI_DWH, XI_ERR_BADARG, \
        "Output tile should be in DWH order");
    XI_CHECK_ERROR(((XI_TILE3D_GET_DIM2_PITCH(inTile) * 
      (MIN2(XI_TILE3D_GET_DIM1(outTile), (XCHAL_IVPN_SIMD_WIDTH >> 2))) - 1) + 3)  < (64 * 1024), \
      XI_ERR_DATASIZE, "Tile size too big for gather operation");
    XI_CHECK_ERROR(typeConvertVar == 0 || typeConvertVar == 128, XI_ERR_BADARG, \
      "\ntypeConverterVar = %hhu, value should be either 0 or 128", typeConvertVar);
  }

  /* Getting parameters from the tile structures */
  const int32_t dim1Size       = XI_TILE3D_GET_DIM1(inTile);
  const int32_t dim2Size       = XI_TILE3D_GET_DIM2(inTile);

  const int32_t outWidth       = XI_TILE3D_GET_DIM1(outTile);

  const int32_t inDataPitch1  = XI_TILE3D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2  = XI_TILE3D_GET_DIM2_PITCH(inTile);
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);

  uint8_t* restrict pInput  = (uint8_t*)XI_TILE3D_GET_DATA_PTR(inTile);
  uint8_t* restrict pOutput = (uint8_t*)XI_TILE3D_GET_DATA_PTR(outTile);
  uint8_t* restrict psrc;
  uint8_t* restrict pdst;
  xb_vecNx8U* restrict vdst_ptr0;
  xb_vecNx8U* restrict vdst_ptr1;
  xb_vecNx8U* restrict vdst_ptr2;
  xb_vecNx8U* restrict vdst_ptr3;

  const xb_vecNx16 vecScalar = (xb_vecNx16)typeConvertVar;

  int sstride = inDataPitch2;
  int dstride = outDataPitch1;
  int swidth = dim1Size;
  int dwidth = outWidth;
  
  valign a0 = IVP_ZALIGN();
  valign a1 = IVP_ZALIGN();
  valign a2 = IVP_ZALIGN();
  valign a3 = IVP_ZALIGN();
  
#ifdef __XCC__
    XT_MEMW(); /* Adding Memory Wait as Gather and Normal Load/Stores are not synchronized */
#endif
  /* Offsets to be used for Gather operation */
  xb_vecNx16 offsets = IVP_ADDNX16(IVP_MULNX16PACKL(IVP_SRLINX16(IVP_SEQNX16(), 2), sstride), IVP_ANDNX16(3, IVP_SEQNX16()));
  int i, j = 0;
  for (i = 0; i < (dwidth & -XCHAL_IVPN_SIMD_WIDTH); i += XCHAL_IVPN_SIMD_WIDTH)
  {
    for (int32_t idx = 0; idx < dim2Size; idx++)
    {
      psrc = (uint8_t*)&pInput[idx * inDataPitch1];
      pdst = (uint8_t*)&pOutput[idx * outDataPitch2];

      vdst_ptr0 = (xb_vecNx8U*)(pdst + i);
      vdst_ptr1 = (xb_vecNx8U*)(pdst + i + dstride);
      vdst_ptr2 = (xb_vecNx8U*)(pdst + i + 2 * dstride);
      vdst_ptr3 = (xb_vecNx8U*)(pdst + i + 3 * dstride);
      
      for (j = 0; j < (swidth & ~3); j += 4)
      {
        /* gathering by 4 16-bit elements per row, total 8 rows per gather */
        uint8_t*  srcPtr0 = (uint8_t*)((psrc)+j + (i + 0 * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        uint8_t*  srcPtr1 = (uint8_t*)((psrc)+j + (i + 1 * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        uint8_t*  srcPtr2 = (uint8_t*)((psrc)+j + (i + 2 * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        uint8_t*  srcPtr3 = (uint8_t*)((psrc)+j + (i + 3 * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));

        xb_vecNx16 v0 = IVP_GATHERNX8U_V(srcPtr0, offsets, 15);
        xb_vecNx16 v1 = IVP_GATHERNX8U_V(srcPtr1, offsets, 15);
        xb_vecNx16 v2 = IVP_GATHERNX8U_V(srcPtr2, offsets, 15);
        xb_vecNx16 v3 = IVP_GATHERNX8U_V(srcPtr3, offsets, 15);

        v0 = IVP_XORNX16(v0, vecScalar);
        v1 = IVP_XORNX16(v1, vecScalar);
        v2 = IVP_XORNX16(v2, vecScalar);
        v3 = IVP_XORNX16(v3, vecScalar);

        IVP_DSELNX16I(v1, v0, v1, v0, IVP_DSELI_DEINTERLEAVE_2);
        IVP_DSELNX16I(v3, v2, v3, v2, IVP_DSELI_DEINTERLEAVE_2);
        IVP_DSELNX16I(v2, v0, v2, v0, IVP_DSELI_DEINTERLEAVE_1);
        IVP_DSELNX16I(v3, v1, v3, v1, IVP_DSELI_DEINTERLEAVE_1);

        IVP_SANX8U_IP(v0, a0, vdst_ptr0);
        FLUSHNX8U(a0, vdst_ptr0, 4 * dstride, XCHAL_IVPN_SIMD_WIDTH);

        IVP_SANX8U_IP(v2, a1, vdst_ptr1);
        FLUSHNX8U(a1, vdst_ptr1, 4 * dstride, XCHAL_IVPN_SIMD_WIDTH);

        IVP_SANX8U_IP(v1, a2, vdst_ptr2);
        FLUSHNX8U(a2, vdst_ptr2, 4 * dstride, XCHAL_IVPN_SIMD_WIDTH);

        IVP_SANX8U_IP(v3, a3, vdst_ptr3);
        FLUSHNX8U(a3, vdst_ptr3, 4 * dstride, XCHAL_IVPN_SIMD_WIDTH);
      }
      if (j < swidth)
      {
        int num1 = j < (swidth - 1) ? XCHAL_IVPN_SIMD_WIDTH : 0;
        int num2 = j < (swidth - 2) ? XCHAL_IVPN_SIMD_WIDTH : 0;

        /* gathering by 4 16-bit elements per row, total 8 rows per gather */
        uint8_t*  srcPtr0 = (uint8_t*)((psrc)+j + (i + 0 * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        uint8_t*  srcPtr1 = (uint8_t*)((psrc)+j + (i + 1 * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        uint8_t*  srcPtr2 = (uint8_t*)((psrc)+j + (i + 2 * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        uint8_t*  srcPtr3 = (uint8_t*)((psrc)+j + (i + 3 * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        xb_vecNx16 v0 = IVP_GATHERNX8U_V(srcPtr0, offsets, 15);
        xb_vecNx16 v1 = IVP_GATHERNX8U_V(srcPtr1, offsets, 15);
        xb_vecNx16 v2 = IVP_GATHERNX8U_V(srcPtr2, offsets, 15);
        xb_vecNx16 v3 = IVP_GATHERNX8U_V(srcPtr3, offsets, 15);

        v0 = IVP_XORNX16(v0, vecScalar);
        v1 = IVP_XORNX16(v1, vecScalar);
        v2 = IVP_XORNX16(v2, vecScalar);
        v3 = IVP_XORNX16(v3, vecScalar);

        IVP_DSELNX16I(v1, v0, v1, v0, IVP_DSELI_DEINTERLEAVE_2);
        IVP_DSELNX16I(v3, v2, v3, v2, IVP_DSELI_DEINTERLEAVE_2);
        IVP_DSELNX16I(v2, v0, v2, v0, IVP_DSELI_DEINTERLEAVE_1);
        IVP_DSELNX16I(v3, v1, v3, v1, IVP_DSELI_DEINTERLEAVE_1);

        IVP_SAVNX8U_XP(v0, a0, vdst_ptr0, XCHAL_IVPN_SIMD_WIDTH);
        IVP_SAVNX8UPOS_FP(a0, vdst_ptr0);

        IVP_SAVNX8U_XP(v2, a1, vdst_ptr1, num1);
        IVP_SAVNX8UPOS_FP(a1, vdst_ptr1);

        IVP_SAVNX8U_XP(v1, a2, vdst_ptr2, num2);
        IVP_SAVNX8UPOS_FP(a2, vdst_ptr2);
      }
    }
  }
  if (i < dwidth)
  {
    for (int32_t idx = 0; idx < dim2Size; idx++)
    {
      psrc = (uint8_t*)&pInput[idx * inDataPitch1];
      pdst = (uint8_t*)&pOutput[idx * outDataPitch2];

      vboolN h_mask0 = IVP_LTNX16(IVP_SRLINX16(IVP_SEQNX16(), 2), dwidth - i);
      vboolN h_mask1 = IVP_LTNX16(IVP_SRLINX16(IVP_SEQNX16(), 2), dwidth - i - (XCHAL_IVPN_SIMD_WIDTH >> 2));
      vboolN h_mask2 = IVP_LTNX16(IVP_SRLINX16(IVP_SEQNX16(), 2), dwidth - i - 2 * (XCHAL_IVPN_SIMD_WIDTH >> 2));
      vboolN h_mask3 = IVP_LTNX16(IVP_SRLINX16(IVP_SEQNX16(), 2), dwidth - i - 3 * (XCHAL_IVPN_SIMD_WIDTH >> 2));

      int rowIsValid0 = XT_MIN(XT_MAX(dwidth - i - (XCHAL_IVPN_SIMD_WIDTH >> 2), 0), 1);
      int rowIsValid1 = XT_MIN(XT_MAX(dwidth - i - 2 * (XCHAL_IVPN_SIMD_WIDTH >> 2), 0), 1);
      int rowIsValid2 = XT_MIN(XT_MAX(dwidth - i - 3 * (XCHAL_IVPN_SIMD_WIDTH >> 2), 0), 1);

      xb_vecNx16U offsets1T = IVP_MOVNX16UT(offsets, 0, h_mask0);
      xb_vecNx16U offsets2T = IVP_MOVNX16UT(offsets, 0, h_mask1);
      xb_vecNx16U offsets3T = IVP_MOVNX16UT(offsets, 0, h_mask2);
      xb_vecNx16U offsets4T = IVP_MOVNX16UT(offsets, 0, h_mask3);

      vdst_ptr0 = (xb_vecNx8U*)(pdst + i);
      vdst_ptr1 = (xb_vecNx8U*)(pdst + i + dstride);
      vdst_ptr2 = (xb_vecNx8U*)(pdst + i + 2 * dstride);
      vdst_ptr3 = (xb_vecNx8U*)(pdst + i + 3 * dstride);
      int32_t remVal = XT_MIN(dwidth - i, XCHAL_IVPN_SIMD_WIDTH);
      for (j = 0; j < (swidth & ~3); j += 4)
      {
        /* gathering up to 4 16-bit elements per row, total 8 rows per gather */
        uint8_t*  srcPtr0 = (uint8_t*)((psrc)+j + (i + 0 * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        uint8_t*  srcPtr1 = (uint8_t*)((psrc)+j + (i + (1 * rowIsValid0) * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        uint8_t*  srcPtr2 = (uint8_t*)((psrc)+j + (i + (2 * rowIsValid1) * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        uint8_t*  srcPtr3 = (uint8_t*)((psrc)+j + (i + (3 * rowIsValid2) * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        xb_vecNx16 v0 = IVP_GATHERNX8U_V(srcPtr0, offsets1T, 15);
        xb_vecNx16 v1 = IVP_GATHERNX8U_V(srcPtr1, offsets2T, 15);
        xb_vecNx16 v2 = IVP_GATHERNX8U_V(srcPtr2, offsets3T, 15);
        xb_vecNx16 v3 = IVP_GATHERNX8U_V(srcPtr3, offsets4T, 15);

        v0 = IVP_XORNX16(v0, vecScalar);
        v1 = IVP_XORNX16(v1, vecScalar);
        v2 = IVP_XORNX16(v2, vecScalar);
        v3 = IVP_XORNX16(v3, vecScalar);

        IVP_DSELNX16I(v1, v0, v1, v0, IVP_DSELI_DEINTERLEAVE_2);
        IVP_DSELNX16I(v3, v2, v3, v2, IVP_DSELI_DEINTERLEAVE_2);
        IVP_DSELNX16I(v2, v0, v2, v0, IVP_DSELI_DEINTERLEAVE_1);
        IVP_DSELNX16I(v3, v1, v3, v1, IVP_DSELI_DEINTERLEAVE_1);

        IVP_SAVNX8U_XP(v0, a0, vdst_ptr0, remVal);
        FLUSHNX8U(a0, vdst_ptr0, 4 * dstride, remVal);

        IVP_SAVNX8U_XP(v2, a1, vdst_ptr1, remVal);
        FLUSHNX8U(a1, vdst_ptr1, 4 * dstride, remVal);

        IVP_SAVNX8U_XP(v1, a2, vdst_ptr2, remVal);
        FLUSHNX8U(a2, vdst_ptr2, 4 * dstride, remVal);

        IVP_SAVNX8U_XP(v3, a3, vdst_ptr3, remVal);
        FLUSHNX8U(a3, vdst_ptr3, 4 * dstride, remVal);
      }

      if (j < swidth)
      {
        vboolN h_mask = IVP_LENX16(IVP_ANDNX16(3, IVP_SEQNX16()), (swidth - 1) & 3);
        vboolN h_mask0 = IVP_ANDBN(h_mask, IVP_LTNX16(IVP_SRLINX16(IVP_SEQNX16(), 2), dwidth - i));
        vboolN h_mask1 = IVP_ANDBN(h_mask, IVP_LTNX16(IVP_SRLINX16(IVP_SEQNX16(), 2), dwidth - i - (XCHAL_IVPN_SIMD_WIDTH >> 2)));
        vboolN h_mask2 = IVP_ANDBN(h_mask, IVP_LTNX16(IVP_SRLINX16(IVP_SEQNX16(), 2), dwidth - i - 2 * (XCHAL_IVPN_SIMD_WIDTH >> 2)));
        vboolN h_mask3 = IVP_ANDBN(h_mask, IVP_LTNX16(IVP_SRLINX16(IVP_SEQNX16(), 2), dwidth - i - 3 * (XCHAL_IVPN_SIMD_WIDTH >> 2)));
        int num1 = j < (swidth - 1) ? dwidth - i : 0;
        int num2 = j < (swidth - 2) ? dwidth - i : 0;

        /* gathering by 4 16-bit elements per row, total 8 rows per gather */
        uint8_t*  srcPtr0 = (uint8_t*)((psrc)+j + (i + 0 * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        uint8_t*  srcPtr1 = (uint8_t*)((psrc)+j + (i + (1 * rowIsValid0) * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        uint8_t*  srcPtr2 = (uint8_t*)((psrc)+j + (i + (2 * rowIsValid1) * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        uint8_t*  srcPtr3 = (uint8_t*)((psrc)+j + (i + (3 * rowIsValid2) * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        xb_vecNx16 v0 = IVP_GATHERNX8U_V(srcPtr0, IVP_MOVNX16UT(offsets, 0, h_mask0), 15);
        xb_vecNx16 v1 = IVP_GATHERNX8U_V(srcPtr1, IVP_MOVNX16UT(offsets, 0, h_mask1), 15);
        xb_vecNx16 v2 = IVP_GATHERNX8U_V(srcPtr2, IVP_MOVNX16UT(offsets, 0, h_mask2), 15);
        xb_vecNx16 v3 = IVP_GATHERNX8U_V(srcPtr3, IVP_MOVNX16UT(offsets, 0, h_mask3), 15);

        v0 = IVP_XORNX16(v0, vecScalar);
        v1 = IVP_XORNX16(v1, vecScalar);
        v2 = IVP_XORNX16(v2, vecScalar);
        v3 = IVP_XORNX16(v3, vecScalar);

        IVP_DSELNX16I(v1, v0, v1, v0, IVP_DSELI_DEINTERLEAVE_2);
        IVP_DSELNX16I(v3, v2, v3, v2, IVP_DSELI_DEINTERLEAVE_2);
        IVP_DSELNX16I(v2, v0, v2, v0, IVP_DSELI_DEINTERLEAVE_1);
        IVP_DSELNX16I(v3, v1, v3, v1, IVP_DSELI_DEINTERLEAVE_1);

        IVP_SAVNX8U_XP(v0, a0, vdst_ptr0, dwidth - i);
        IVP_SAVNX8UPOS_FP(a0, vdst_ptr0);

        IVP_SAVNX8U_XP(v2, a1, vdst_ptr1, num1);
        IVP_SAVNX8UPOS_FP(a1, vdst_ptr1);

        IVP_SAVNX8U_XP(v1, a2, vdst_ptr2, num2);
        IVP_SAVNX8UPOS_FP(a2, vdst_ptr2);
      }
    }
  }
  return(XI_ERROR_STATUS());
}

/************************* xiTranspose3D_I8_DWH_WHD *************************/
/* Description : Vision P6 implementation for getting the transpose of a 3D */
/*               tile from  DWH to WHD format.                              */
/* Inputs      : Input Tile data                                            */
/* Outputs     : XI Error Code                                              */
/* InOuts      : Output Tile                                                */
/* Assumptions : InData is integer 8bit value                               */
/*               InTile is in DWH format and outTile is in WHD format       */
/****************************************************************************/

XI_ERR_TYPE xiTranspose3D_I8_DWH_WHD(const xi_pTile3D inTile,
                                     xi_pTile3D outTile,
                                     const uint8_t typeConvertVar)
{
  XI_ERROR_CHECKS()
  {
    XI_CHECK_TILE3D_I8(inTile);
    XI_CHECK_TILE3D_I8(outTile);
    XI_CHECK_TILE3D_FITS_IN_SINGLE_DRAM(inTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_TILES3D_ARE_NOT_OVERLAP(inTile, outTile);
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(inTile) == XI_TILE3D_GET_DIM3(outTile) &&\
        XI_TILE3D_GET_DIM2(inTile) == XI_TILE3D_GET_DIM1(outTile) &&\
        XI_TILE3D_GET_DIM3(inTile) == XI_TILE3D_GET_DIM2(outTile), XI_ERR_DATASIZE, \
        "Input and output tiles size are not valid");
    XI_CHECK_ERROR(XI_TILE3D_GET_DATA_ORDER(inTile) == XI_DWH, XI_ERR_BADARG, \
        "Input tile should be in DWH order");
    XI_CHECK_ERROR(XI_TILE3D_GET_DATA_ORDER(outTile) == XI_WHD, XI_ERR_BADARG, \
        "Output tile should be in WHD order");
    XI_CHECK_ERROR(((XI_TILE3D_GET_DIM1_PITCH(inTile) *      \
      (MIN2(XI_TILE3D_GET_DIM1(outTile), (XCHAL_IVPN_SIMD_WIDTH >> 2))) - 1) + 3)  < (64 * 1024), \
      XI_ERR_DATASIZE, "Tile size too big for gather operation");
    XI_CHECK_ERROR(typeConvertVar == 0 || typeConvertVar == 128, XI_ERR_BADARG, \
      "\ntypeConverterVar = %hhu, value should be either 0 or 128", typeConvertVar);
  }

  /* Getting parameters from the tile structures */
  const int32_t dim1Size       = XI_TILE3D_GET_DIM1(inTile);
  const int32_t dim3Size       = XI_TILE3D_GET_DIM3(inTile);

  const int32_t outWidth       = XI_TILE3D_GET_DIM1(outTile);

  const int32_t inDataPitch1  = XI_TILE3D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2  = XI_TILE3D_GET_DIM2_PITCH(inTile);
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);

  uint8_t* restrict pInput = (uint8_t*)XI_TILE3D_GET_DATA_PTR(inTile);
  uint8_t* restrict pOutput = (uint8_t*)XI_TILE3D_GET_DATA_PTR(outTile);

  uint8_t* restrict psrc;
  uint8_t* restrict pdst;
  xb_vecNx8U* restrict vdst_ptr0;
  xb_vecNx8U* restrict vdst_ptr1;
  xb_vecNx8U* restrict vdst_ptr2;
  xb_vecNx8U* restrict vdst_ptr3;

  const xb_vecNx16 vecScalar = (xb_vecNx16)typeConvertVar;

  int sstride = inDataPitch1;
  int dstride = outDataPitch2;
  int swidth = dim1Size;
  int dwidth = outWidth;
  
  valign a0 = IVP_ZALIGN();
  valign a1 = IVP_ZALIGN();
  valign a2 = IVP_ZALIGN();
  valign a3 = IVP_ZALIGN();
#ifdef __XCC__
    XT_MEMW(); /* Adding Memory Wait as Gather and Normal Load/Stores are not synchronized */
#endif
  int i, j = 0;
  /* Offset used for gather operation */
  xb_vecNx16 offsets = IVP_ADDNX16(IVP_MULNX16PACKL(IVP_SRLINX16(IVP_SEQNX16(), 2), sstride), IVP_ANDNX16(3, IVP_SEQNX16()));

  for (i = 0; i < (dwidth & -XCHAL_IVPN_SIMD_WIDTH); i += XCHAL_IVPN_SIMD_WIDTH)
  {
    for (int32_t idx = 0; idx < dim3Size; idx++)
    {
      psrc = (uint8_t*)&pInput[idx * inDataPitch2];
      pdst = (uint8_t*)&pOutput[idx * outDataPitch1];

      vdst_ptr0 = (xb_vecNx8U*)(pdst + i);
      vdst_ptr1 = (xb_vecNx8U*)(pdst + i + dstride);
      vdst_ptr2 = (xb_vecNx8U*)(pdst + i + 2 * dstride);
      vdst_ptr3 = (xb_vecNx8U*)(pdst + i + 3 *dstride);

      for (j = 0; j < (swidth & ~3); j += 4)
      {
        /* gathering by 4 16-bit elements per row, total 8 rows per gather */
        uint8_t*  srcPtr0 = (uint8_t*)((psrc)+j + (i + 0 * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        uint8_t*  srcPtr1 = (uint8_t*)((psrc)+j + (i + 1 * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        uint8_t*  srcPtr2 = (uint8_t*)((psrc)+j + (i + 2 * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        uint8_t*  srcPtr3 = (uint8_t*)((psrc)+j + (i + 3 * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        xb_vecNx16 v0 = IVP_GATHERNX8U_V(srcPtr0, offsets, 15);
        xb_vecNx16 v1 = IVP_GATHERNX8U_V(srcPtr1, offsets, 15);
        xb_vecNx16 v2 = IVP_GATHERNX8U_V(srcPtr2, offsets, 15);
        xb_vecNx16 v3 = IVP_GATHERNX8U_V(srcPtr3, offsets, 15);

        v0 = IVP_XORNX16(v0, vecScalar);
        v1 = IVP_XORNX16(v1, vecScalar);
        v2 = IVP_XORNX16(v2, vecScalar);
        v3 = IVP_XORNX16(v3, vecScalar);

        IVP_DSELNX16I(v1, v0, v1, v0, IVP_DSELI_DEINTERLEAVE_2);
        IVP_DSELNX16I(v3, v2, v3, v2, IVP_DSELI_DEINTERLEAVE_2);
        IVP_DSELNX16I(v2, v0, v2, v0, IVP_DSELI_DEINTERLEAVE_1);
        IVP_DSELNX16I(v3, v1, v3, v1, IVP_DSELI_DEINTERLEAVE_1);

        IVP_SANX8U_IP(v0, a0, vdst_ptr0);
        FLUSHNX8U(a0, vdst_ptr0, 4 * dstride, XCHAL_IVPN_SIMD_WIDTH);

        IVP_SANX8U_IP(v2, a1, vdst_ptr1);
        FLUSHNX8U(a1, vdst_ptr1, 4 * dstride, XCHAL_IVPN_SIMD_WIDTH);

        IVP_SANX8U_IP(v1, a2, vdst_ptr2);
        FLUSHNX8U(a2, vdst_ptr2, 4 * dstride, XCHAL_IVPN_SIMD_WIDTH);

        IVP_SANX8U_IP(v3, a3, vdst_ptr3);
        FLUSHNX8U(a3, vdst_ptr3, 4 * dstride, XCHAL_IVPN_SIMD_WIDTH);
      }
      if (j < swidth)
      {
        int num1 = j < (swidth - 1) ? XCHAL_IVPN_SIMD_WIDTH : 0;
        int num2 = j < (swidth - 2) ? XCHAL_IVPN_SIMD_WIDTH : 0;

        /* gathering by 4 16-bit elements per row, total 8 rows per gather */
        uint8_t*  srcPtr0 = (uint8_t*)((psrc)+j + (i + 0 * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        uint8_t*  srcPtr1 = (uint8_t*)((psrc)+j + (i + 1 * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        uint8_t*  srcPtr2 = (uint8_t*)((psrc)+j + (i + 2 * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        uint8_t*  srcPtr3 = (uint8_t*)((psrc)+j + (i + 3 * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        
        xb_vecNx16 v0 = IVP_GATHERNX8U_V(srcPtr0, offsets, 15);
        xb_vecNx16 v1 = IVP_GATHERNX8U_V(srcPtr1, offsets, 15);
        xb_vecNx16 v2 = IVP_GATHERNX8U_V(srcPtr2, offsets, 15);
        xb_vecNx16 v3 = IVP_GATHERNX8U_V(srcPtr3, offsets, 15);

        v0 = IVP_XORNX16(v0, vecScalar);
        v1 = IVP_XORNX16(v1, vecScalar);
        v2 = IVP_XORNX16(v2, vecScalar);
        v3 = IVP_XORNX16(v3, vecScalar);

        IVP_DSELNX16I(v1, v0, v1, v0, IVP_DSELI_DEINTERLEAVE_2);
        IVP_DSELNX16I(v3, v2, v3, v2, IVP_DSELI_DEINTERLEAVE_2);
        IVP_DSELNX16I(v2, v0, v2, v0, IVP_DSELI_DEINTERLEAVE_1);
        IVP_DSELNX16I(v3, v1, v3, v1, IVP_DSELI_DEINTERLEAVE_1);

        IVP_SAVNX8U_XP(v0, a0, vdst_ptr0, XCHAL_IVPN_SIMD_WIDTH);
        IVP_SAVNX8UPOS_FP(a0, vdst_ptr0);

        IVP_SAVNX8U_XP(v2, a1, vdst_ptr1, num1);
        IVP_SAVNX8UPOS_FP(a1, vdst_ptr1);

        IVP_SAVNX8U_XP(v1, a2, vdst_ptr2, num2);
        IVP_SAVNX8UPOS_FP(a2, vdst_ptr2);
      }
    }
  }
  if (i < dwidth)
  {
    for (int32_t idx = 0; idx < dim3Size; idx++)
    {
      psrc = (uint8_t*)&pInput[idx * inDataPitch2];
      pdst = (uint8_t*)&pOutput[idx * outDataPitch1];

      vboolN h_mask0 = IVP_LTNX16(IVP_SRLINX16(IVP_SEQNX16(), 2), dwidth - i);
      vboolN h_mask1 = IVP_LTNX16(IVP_SRLINX16(IVP_SEQNX16(), 2), dwidth - i - (XCHAL_IVPN_SIMD_WIDTH >> 2));
      vboolN h_mask2 = IVP_LTNX16(IVP_SRLINX16(IVP_SEQNX16(), 2), dwidth - i - 2 * (XCHAL_IVPN_SIMD_WIDTH >> 2));
      vboolN h_mask3 = IVP_LTNX16(IVP_SRLINX16(IVP_SEQNX16(), 2), dwidth - i - 3 * (XCHAL_IVPN_SIMD_WIDTH >> 2));

      int rowIsValid0 = XT_MIN(XT_MAX(dwidth - i - (XCHAL_IVPN_SIMD_WIDTH >> 2), 0), 1);
      int rowIsValid1 = XT_MIN(XT_MAX(dwidth - i - 2 * (XCHAL_IVPN_SIMD_WIDTH >> 2), 0), 1);
      int rowIsValid2 = XT_MIN(XT_MAX(dwidth - i - 3 * (XCHAL_IVPN_SIMD_WIDTH >> 2), 0), 1);

      xb_vecNx16U offsets1T = IVP_MOVNX16UT(offsets, 0, h_mask0);
      xb_vecNx16U offsets2T = IVP_MOVNX16UT(offsets, 0, h_mask1);
      xb_vecNx16U offsets3T = IVP_MOVNX16UT(offsets, 0, h_mask2);
      xb_vecNx16U offsets4T = IVP_MOVNX16UT(offsets, 0, h_mask3);

      vdst_ptr0 = (xb_vecNx8U*)(pdst + i);
      vdst_ptr1 = (xb_vecNx8U*)(pdst + i + dstride);
      vdst_ptr2 = (xb_vecNx8U*)(pdst + i + 2 * dstride);
      vdst_ptr3 = (xb_vecNx8U*)(pdst + i + 3 * dstride);
      
      int32_t remVal = XT_MIN(dwidth - i, XCHAL_IVPN_SIMD_WIDTH);
      for (j = 0; j < (swidth & ~3); j += 4)
      {
        /* gathering up to 4 16-bit elements per row, total 8 rows per gather */
        uint8_t*  srcPtr0 = (uint8_t*)((psrc)+j + (i + 0 * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        uint8_t*  srcPtr1 = (uint8_t*)((psrc)+j + (i + (1 * rowIsValid0) * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        uint8_t*  srcPtr2 = (uint8_t*)((psrc)+j + (i + (2 * rowIsValid1) * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        uint8_t*  srcPtr3 = (uint8_t*)((psrc)+j + (i + (3 * rowIsValid2) * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        xb_vecNx16 v0 = IVP_GATHERNX8U_V(srcPtr0, offsets1T, 15);
        xb_vecNx16 v1 = IVP_GATHERNX8U_V(srcPtr1, offsets2T, 15);
        xb_vecNx16 v2 = IVP_GATHERNX8U_V(srcPtr2, offsets3T, 15);
        xb_vecNx16 v3 = IVP_GATHERNX8U_V(srcPtr3, offsets4T, 15);

        v0 = IVP_XORNX16(v0, vecScalar);
        v1 = IVP_XORNX16(v1, vecScalar);
        v2 = IVP_XORNX16(v2, vecScalar);
        v3 = IVP_XORNX16(v3, vecScalar);

        IVP_DSELNX16I(v1, v0, v1, v0, IVP_DSELI_DEINTERLEAVE_2);
        IVP_DSELNX16I(v3, v2, v3, v2, IVP_DSELI_DEINTERLEAVE_2);
        IVP_DSELNX16I(v2, v0, v2, v0, IVP_DSELI_DEINTERLEAVE_1);
        IVP_DSELNX16I(v3, v1, v3, v1, IVP_DSELI_DEINTERLEAVE_1);

        IVP_SAVNX8U_XP(v0, a0, vdst_ptr0, remVal);
        FLUSHNX8U(a0, vdst_ptr0, 4 * dstride, remVal);

        IVP_SAVNX8U_XP(v2, a1, vdst_ptr1, remVal);
        FLUSHNX8U(a1, vdst_ptr1, 4 * dstride, remVal);

        IVP_SAVNX8U_XP(v1, a2, vdst_ptr2, remVal);
        FLUSHNX8U(a2, vdst_ptr2, 4 * dstride, remVal);

        IVP_SAVNX8U_XP(v3, a3, vdst_ptr3, remVal);
        FLUSHNX8U(a3, vdst_ptr3, 4 * dstride, remVal);
      }

      if (j < swidth)
      {
        vboolN h_mask = IVP_LENX16(IVP_ANDNX16(3, IVP_SEQNX16()), (swidth - 1) & 3);
        vboolN h_mask0 = IVP_ANDBN(h_mask, IVP_LTNX16(IVP_SRLINX16(IVP_SEQNX16(), 2), dwidth - i));
        vboolN h_mask1 = IVP_ANDBN(h_mask, IVP_LTNX16(IVP_SRLINX16(IVP_SEQNX16(), 2), dwidth - i - (XCHAL_IVPN_SIMD_WIDTH >> 2)));
        vboolN h_mask2 = IVP_ANDBN(h_mask, IVP_LTNX16(IVP_SRLINX16(IVP_SEQNX16(), 2), dwidth - i - 2 * (XCHAL_IVPN_SIMD_WIDTH >> 2)));
        vboolN h_mask3 = IVP_ANDBN(h_mask, IVP_LTNX16(IVP_SRLINX16(IVP_SEQNX16(), 2), dwidth - i - 3 * (XCHAL_IVPN_SIMD_WIDTH >> 2)));
        int num1 = j < (swidth - 1) ? dwidth - i : 0;
        int num2 = j < (swidth - 2) ? dwidth - i : 0;

        /* gathering by 4 16-bit elements per row, total 8 rows per gather */
        uint8_t*  srcPtr0 = (uint8_t*)((psrc)+j + (i + 0 * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        uint8_t*  srcPtr1 = (uint8_t*)((psrc)+j + (i + (1 * rowIsValid0) * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        uint8_t*  srcPtr2 = (uint8_t*)((psrc)+j + (i + (2 * rowIsValid1) * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        uint8_t*  srcPtr3 = (uint8_t*)((psrc)+j + (i + (3 * rowIsValid2) * XCHAL_IVPN_SIMD_WIDTH / 4)*(sstride));
        xb_vecNx16 v0 = IVP_GATHERNX8U_V(srcPtr0, IVP_MOVNX16UT(offsets, 0, h_mask0), 15);
        xb_vecNx16 v1 = IVP_GATHERNX8U_V(srcPtr1, IVP_MOVNX16UT(offsets, 0, h_mask1), 15);
        xb_vecNx16 v2 = IVP_GATHERNX8U_V(srcPtr2, IVP_MOVNX16UT(offsets, 0, h_mask2), 15);
        xb_vecNx16 v3 = IVP_GATHERNX8U_V(srcPtr3, IVP_MOVNX16UT(offsets, 0, h_mask3), 15);

        v0 = IVP_XORNX16(v0, vecScalar);
        v1 = IVP_XORNX16(v1, vecScalar);
        v2 = IVP_XORNX16(v2, vecScalar);
        v3 = IVP_XORNX16(v3, vecScalar);

        IVP_DSELNX16I(v1, v0, v1, v0, IVP_DSELI_DEINTERLEAVE_2);
        IVP_DSELNX16I(v3, v2, v3, v2, IVP_DSELI_DEINTERLEAVE_2);
        IVP_DSELNX16I(v2, v0, v2, v0, IVP_DSELI_DEINTERLEAVE_1);
        IVP_DSELNX16I(v3, v1, v3, v1, IVP_DSELI_DEINTERLEAVE_1);

        IVP_SAVNX8U_XP(v0, a0, vdst_ptr0, dwidth - i);
        IVP_SAVNX8UPOS_FP(a0, vdst_ptr0);

        IVP_SAVNX8U_XP(v2, a1, vdst_ptr1, num1);
        IVP_SAVNX8UPOS_FP(a1, vdst_ptr1);

        IVP_SAVNX8U_XP(v1, a2, vdst_ptr2, num2);
        IVP_SAVNX8UPOS_FP(a2, vdst_ptr2);
      }
    }
  }
  return(XI_ERROR_STATUS());
}

/************************ xiUnsignedToSigned3D_U8S8 ******************************/
/* Description : P6 optimized implementation for converting the tile data from   */
/*               unsigned 8bit to signed 8bit. This function can operate         */
/*               in-place. Applications needing this function to operate         */
/*               in-place can provide the same Input and Output Tiles.           */
/* Inputs      : Input Tile                                                      */
/* Outputs     : XI Error Code                                                   */
/* InOuts      : Output Tile                                                     */
/* Assumptions : InData is unsigned 8bit                                         */
/*               Unsigned to Signed 8bit conversion not performed on tile edges  */
/*********************************************************************************/
XI_ERR_TYPE xiUnsignedToSigned3D_U8S8(xi_pTile3D inTile, xi_pTile3D outTile)
{
  /* Error Checks */
  XI_ERROR_CHECKS()
  {
    XI_CHECK_TILE3D(inTile);
    XI_CHECK_TILE3D(outTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_TILE3D_U8(inTile);
    XI_CHECK_TILE3D_S8(outTile);
    XI_CHECK_TILE3D_EQUAL(inTile, outTile);
    XI_CHECK_ERROR(XI_TILE3D_GET_DATA_ORDER(inTile) == XI_TILE3D_GET_DATA_ORDER(outTile), XI_ERR_BADARG,\
      "\nData Order of InputTile = %d OutputTile = %d\nData Order of InputTile and OutputTile should be same", \
      XI_TILE3D_GET_DATA_ORDER(inTile), XI_TILE3D_GET_DATA_ORDER(outTile));
  }

  /* Getting parameters from the tile structures */
  const int32_t dim1Size   = XI_TILE3D_GET_DIM1(inTile);
  const int32_t dim2Size   = XI_TILE3D_GET_DIM2(inTile);
  const int32_t inDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(inTile);
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
  const int32_t dim3Size   = XI_TILE3D_GET_DIM3(inTile);

  /* Input and Output Data Pointers */
  uint8_t *pInput = (uint8_t *) XI_TILE3D_GET_DATA_PTR(inTile);
  int8_t *pOutput = (int8_t *) XI_TILE3D_GET_DATA_PTR(outTile);
  int32_t maxLoopCount;

  /*  xiUnsignedToSigned3D_U8S8 function support in-place unsigned to signed 8bit */
  /*  conversion. In a such a scenario inTile and outTile will be overlapping.    */
  /*  Hence restrict keyword is not used for input and output data pointers       */
  xb_vec2Nx8U* restrict pdvecIn;
  xb_vec2Nx8* restrict pdvecOut;
  valign vaInData;
  valign vaOutData = IVP_ZALIGN();
  xb_vec2Nx8U vecValue1, vecValue2, vecValue3, vecValue4;
  xb_vec2Nx8 vecValueSigned1, vecValueSigned2, vecValueSigned3, vecValueSigned4;
  const xb_vec2Nx8 signedCharMax = SCHAR_MAX;

  /* Vectorization for xiUnsignedToSigned3D_U8S8 function */
  /* is always done across the first dimension            */
  int32_t vectorizationWidth = 2 * XCHAL_IVPN_SIMD_WIDTH;
  int32_t vectorizationWidth2X = 2 * vectorizationWidth;
  int32_t vectorizationWidth3X = 3 * vectorizationWidth;
  int32_t vectorizationWidth4X = 4 * vectorizationWidth;
  int32_t x, y, z;

  /******************************************************************************/
  /* The overall design approach is split into 2 parts                          */
  /* 1. When input tile pitch is equal to input tile width and input tile pitch */
  /*    is equal to output tile pitch                                           */
  /*    - If above condition holds good, data elements for which unsigned       */
  /*      8 bit to signed 8 bit conversion need to done present in contiguous   */
  /*      memory location. Hence vectorization can be utilized effectively      */
  /*                                                                            */
  /* 2. When input tile pitch is not equal to input tile size or input tile     */
  /*    pitch is not equal to output tile pitch                                 */
  /*    - If above condition holds good, data elements for which unsigned       */
  /*      8 bit to signed 8 bit conversion need to done exist in non-contiguous */
  /*      memory location. In order to do vectorization across first dimension, */
  /*      output data pointers need to be updated based on output tile size     */
  /*      and output tile pitch                                                 */
  /******************************************************************************/

  if ((inDataPitch1 == dim1Size) && (inDataPitch1 == outDataPitch1))
  {
    /******************************************************************************/
    /* Data exist in contiguous memory location with respect to first dimension   */
    /******************************************************************************/

    /* Initialize MAX2 loop counter */
    int32_t dim3MaxLoopCount = dim3Size;
    maxLoopCount = dim1Size * dim2Size;

    /* Updated Loop count based on tile dimension configuration */
    if ((inDataPitch2 == maxLoopCount) && (inDataPitch2 == outDataPitch2))
    {
      /**********************************************************************/
      /* Data exist in contiguous memory location with respect to first and */
      /* second dimension                                                   */
      /**********************************************************************/
      dim3MaxLoopCount = 1;       /* Update MAX2 loop counter */
      maxLoopCount *= dim3Size;
    }
    for (z = 0; z < dim3MaxLoopCount; z++)
    {
      /* initialize input data pointer */
      pdvecIn  = (xb_vec2Nx8U *) (pInput + (z * inDataPitch2));
      /* initialize output data pointer */
      pdvecOut = (xb_vec2Nx8 *) (pOutput + (z * outDataPitch2));
      vaInData = IVP_LA2NX8U_PP(pdvecIn);

      for (x = 0; x < maxLoopCount - vectorizationWidth4X; x += vectorizationWidth4X)
      {
        /* Load Data */
        IVP_LA2NX8U_IP(vecValue1, vaInData, pdvecIn);
        IVP_LA2NX8U_IP(vecValue2, vaInData, pdvecIn);
        IVP_LA2NX8U_IP(vecValue3, vaInData, pdvecIn);
        IVP_LA2NX8U_IP(vecValue4, vaInData, pdvecIn);

        /* Perform unsigned to signed conversion and rounding off operation */
        vecValue1 = IVP_AVGRU2NX8(vecValue1, 0);
        vecValue2 = IVP_AVGRU2NX8(vecValue2, 0);
        vecValue3 = IVP_AVGRU2NX8(vecValue3, 0);
        vecValue4 = IVP_AVGRU2NX8(vecValue4, 0);

        /* Perform saturation of signed MAX2 value */
        vecValueSigned1 = IVP_MINU2NX8U(signedCharMax, vecValue1);
        vecValueSigned2 = IVP_MINU2NX8U(signedCharMax, vecValue2);
        vecValueSigned3 = IVP_MINU2NX8U(signedCharMax, vecValue3);
        vecValueSigned4 = IVP_MINU2NX8U(signedCharMax, vecValue4);

        /* Store Data */
        IVP_SA2NX8_IP(vecValueSigned1, vaOutData, pdvecOut);
        IVP_SA2NX8_IP(vecValueSigned2, vaOutData, pdvecOut);
        IVP_SA2NX8_IP(vecValueSigned3, vaOutData, pdvecOut);
        IVP_SA2NX8_IP(vecValueSigned4, vaOutData, pdvecOut);
      }
      /* Load remaining data */
      IVP_LAV2NX8U_XP(vecValue1, vaInData, pdvecIn, maxLoopCount - (x + vectorizationWidth3X));
      IVP_LAV2NX8U_XP(vecValue2, vaInData, pdvecIn, maxLoopCount - (x + vectorizationWidth2X));
      IVP_LAV2NX8U_XP(vecValue3, vaInData, pdvecIn, maxLoopCount - (x + vectorizationWidth));
      IVP_LAV2NX8U_XP(vecValue4, vaInData, pdvecIn, maxLoopCount - x);

      /* Perform unsigned to signed conversion and rounding off operation */
      vecValue1 = IVP_AVGRU2NX8(vecValue1, 0);
      vecValue2 = IVP_AVGRU2NX8(vecValue2, 0);
      vecValue3 = IVP_AVGRU2NX8(vecValue3, 0);
      vecValue4 = IVP_AVGRU2NX8(vecValue4, 0);

      /* Perform saturation of signed MAX2 value */
      vecValueSigned1 = IVP_MINU2NX8U(signedCharMax, vecValue1);
      vecValueSigned2 = IVP_MINU2NX8U(signedCharMax, vecValue2);
      vecValueSigned3 = IVP_MINU2NX8U(signedCharMax, vecValue3);
      vecValueSigned4 = IVP_MINU2NX8U(signedCharMax, vecValue4);

      /* Variable stores */
      IVP_SAV2NX8_XP(vecValueSigned1, vaOutData, pdvecOut,
                     maxLoopCount - (x + vectorizationWidth3X));
      IVP_SAV2NX8_XP(vecValueSigned2, vaOutData, pdvecOut,
                     maxLoopCount - (x + vectorizationWidth2X));
      IVP_SAV2NX8_XP(vecValueSigned3, vaOutData, pdvecOut, maxLoopCount - (x + vectorizationWidth));
      IVP_SAV2NX8_XP(vecValueSigned4, vaOutData, pdvecOut, maxLoopCount - x);
      IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
    }
  }
  else
  {
    /* else block is executed if input tile pitch is not equal to input tile width or input tile */
    /* pitch is not equal to output tile pitch                                                   */

    for (z = 0; z < dim3Size; z++) /* Loop across dim3 */
    {
      x = 0;
      /* Loop across dimension 1 */
      /* Condition check added to maximize vectorization across dimension 1*/
      /* Loop across dim1 */
      for (; x < (dim1Size - 3 * vectorizationWidth); x += 4 * vectorizationWidth)
      {
        /* initialize input and output data pointer */
        uint8_t *pInput1  = pInput + x + (z * inDataPitch2);
        int8_t *pOutput1 = pOutput + x + (z * outDataPitch2);
        int32_t varLen = dim1Size - (x + 3 * vectorizationWidth);

        for (y = 0; y < dim2Size; y++)
        {
          pdvecIn  = (xb_vec2Nx8U *) (pInput1 + (y * inDataPitch1));
          pdvecOut = (xb_vec2Nx8 *) (pOutput1 + (y * outDataPitch1));
          vaInData = IVP_LA2NX8U_PP(pdvecIn);

          /* Load Input Data */
          IVP_LA2NX8U_IP(vecValue1, vaInData, pdvecIn);
          IVP_LA2NX8U_IP(vecValue2, vaInData, pdvecIn);
          IVP_LA2NX8U_IP(vecValue3, vaInData, pdvecIn);
          IVP_LAV2NX8U_XP(vecValue4, vaInData, pdvecIn, varLen);

          /* Perform unsigned to signed conversion and rounding off operation */
          vecValue1 = IVP_AVGRU2NX8(vecValue1, 0);
          vecValue2 = IVP_AVGRU2NX8(vecValue2, 0);
          vecValue3 = IVP_AVGRU2NX8(vecValue3, 0);
          vecValue4 = IVP_AVGRU2NX8(vecValue4, 0);

          /* Perform saturation of signed MAX2 value */
          vecValueSigned1 = IVP_MINU2NX8U(signedCharMax, vecValue1);
          vecValueSigned2 = IVP_MINU2NX8U(signedCharMax, vecValue2);
          vecValueSigned3 = IVP_MINU2NX8U(signedCharMax, vecValue3);
          vecValueSigned4 = IVP_MINU2NX8U(signedCharMax, vecValue4);

          /* Store */
          IVP_SA2NX8_IP(vecValueSigned1, vaOutData, pdvecOut);
          IVP_SA2NX8_IP(vecValueSigned2, vaOutData, pdvecOut);
          IVP_SA2NX8_IP(vecValueSigned3, vaOutData, pdvecOut);
          IVP_SAV2NX8_XP(vecValueSigned4, vaOutData, pdvecOut, varLen);
          IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        }
      }
      if (x < (dim1Size - 2 * vectorizationWidth)) /* Loop unrolling across dim2 */
      {
        /* initialize input and output data pointer */
        uint8_t *pInput1  = pInput + x + (z * inDataPitch2);
        int8_t *pOutput1 = pOutput + x + (z * outDataPitch2);
        int32_t varLen = dim1Size - (x + 2 * vectorizationWidth);

        for (y = 0; y < dim2Size; y++)
        {
          pdvecIn  = (xb_vec2Nx8U *) (pInput1 + (y * inDataPitch1));
          pdvecOut = (xb_vec2Nx8 *) (pOutput1 + (y * outDataPitch1));
          vaInData = IVP_LA2NX8U_PP(pdvecIn);

          /* Load Input Data */
          IVP_LA2NX8U_IP(vecValue1, vaInData, pdvecIn);
          IVP_LA2NX8U_IP(vecValue2, vaInData, pdvecIn);
          IVP_LAV2NX8U_XP(vecValue3, vaInData, pdvecIn, varLen);

          /* Perform unsigned to signed conversion and rounding off operation */
          vecValue1 = IVP_AVGRU2NX8(vecValue1, 0);
          vecValue2 = IVP_AVGRU2NX8(vecValue2, 0);
          vecValue3 = IVP_AVGRU2NX8(vecValue3, 0);

          /* Perform saturation of signed MAX2 value */
          vecValueSigned1 = IVP_MINU2NX8U(signedCharMax, vecValue1);
          vecValueSigned2 = IVP_MINU2NX8U(signedCharMax, vecValue2);
          vecValueSigned3 = IVP_MINU2NX8U(signedCharMax, vecValue3);

          /* Store */
          IVP_SA2NX8_IP(vecValueSigned1, vaOutData, pdvecOut);
          IVP_SA2NX8_IP(vecValueSigned2, vaOutData, pdvecOut);
          IVP_SAV2NX8_XP(vecValueSigned3, vaOutData, pdvecOut, varLen);
          IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        }
      }
      else if (x < (dim1Size - vectorizationWidth))
      {
        /* initialize input and output data pointer */
        uint8_t *pInput1  = pInput + x + (z * inDataPitch2);
        int8_t *pOutput1 = pOutput + x + (z * outDataPitch2);
        int32_t varLen = dim1Size - (x + vectorizationWidth);

        for (y = 0; y < dim2Size; y++)
        {
          pdvecIn  = (xb_vec2Nx8U *) (pInput1 + (y * inDataPitch1));
          pdvecOut = (xb_vec2Nx8 *) (pOutput1 + (y * outDataPitch1));
          vaInData = IVP_LA2NX8U_PP(pdvecIn);

          /* Load Input Data */
          IVP_LA2NX8U_IP(vecValue1, vaInData, pdvecIn);
          IVP_LAV2NX8U_XP(vecValue2, vaInData, pdvecIn, varLen);

          /* Perform unsigned to signed conversion and rounding off operation */
          vecValue1 = IVP_AVGRU2NX8(vecValue1, 0);
          vecValue2 = IVP_AVGRU2NX8(vecValue2, 0);

          /* Perform saturation of signed MAX2 value */
          vecValueSigned1 = IVP_MINU2NX8U(signedCharMax, vecValue1);
          vecValueSigned2 = IVP_MINU2NX8U(signedCharMax, vecValue2);

          /* Store */
          IVP_SA2NX8_IP(vecValueSigned1, vaOutData, pdvecOut);
          IVP_SAV2NX8_XP(vecValueSigned2, vaOutData, pdvecOut, dim1Size - (x + vectorizationWidth));
          IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        }
      }
      else if (x < dim1Size)
      {
        /* initialize input and output data pointer */
        uint8_t *pInput1  = pInput + x + (z * inDataPitch2);
        int8_t *pOutput1 = pOutput + x + (z * outDataPitch2);
        int32_t varLen = dim1Size - x;

        for (y = 0; y < dim2Size; y++)
        {
          pdvecIn  = (xb_vec2Nx8U *) (pInput1 + (y * inDataPitch1));
          pdvecOut = (xb_vec2Nx8 *) (pOutput1 + (y * outDataPitch1));
          vaInData = IVP_LA2NX8U_PP(pdvecIn);

          /* Load Input Data */
          IVP_LAV2NX8U_XP(vecValue1, vaInData, pdvecIn, varLen);

          /* Perform unsigned to signed conversion and rounding off operation */
          vecValue1 = IVP_AVGRU2NX8(vecValue1, 0);

          /* Perform saturation of signed MAX2 value */
          vecValueSigned1 = IVP_MINU2NX8U(signedCharMax, vecValue1);

          /* Store */
          IVP_SAV2NX8_XP(vecValueSigned1, vaOutData, pdvecOut, varLen);
          IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        }
      }
    }
  }
  return(XI_ERROR_STATUS());
}
/***************************** XI_CNNA Variants *********************************/

/************************* xiSubtractA3D_const128_U8S8 **************************/
/* Description : Reference implementation of xiSubtractA3D_const128_U8S8 for    */
/*               Android NN which subtracts constant value 128 from the input.  */
/*               This function supports in-place operation, if the same tile is */
/*               provided as both input and output.                             */
/* Inputs      : Input 3D tile                                                  */
/* Outputs     : XI Error Code                                                  */
/* InOuts      : Output 3D Tile                                                 */
/* Assumptions : Input tile is U8                                               */
/*               Output tile is S8                                              */
/********************************************************************************/
#endif

XI_ERR_TYPE xiSubtractA3D_const128_U8S8(const xi_pTile3D inTile,
                                        xi_pTile3D outTile)
{
  /* Error Checks */
  XI_ERROR_CHECKS()
  {
    XI_CHECK_TILE3D(inTile);
    XI_CHECK_TILE3D(outTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_TILE3D_U8(inTile);
    XI_CHECK_TILE3D_S8(outTile);
    XI_CHECK_TILE3D_EQUAL(inTile, outTile);
    XI_CHECK_TILE3D_EDGES_EQUAL(inTile, outTile);
    XI_CHECK_ERROR(XI_TILE3D_GET_DATA_ORDER(inTile) == XI_TILE3D_GET_DATA_ORDER(outTile), XI_ERR_BADARG,\
      "\nData Order of InputTile = %d OutputTile = %d\nData Order of InputTile and OutputTile should be same", \
      XI_TILE3D_GET_DATA_ORDER(inTile), XI_TILE3D_GET_DATA_ORDER(outTile));
  }

  /* Getting parameters from the tile structures */
  const int32_t dim1Size      = XI_TILE3D_GET_DIM1(inTile);
  const int32_t dim2Size      = XI_TILE3D_GET_DIM2(inTile);
  const int32_t dim1Edge1     = XI_TILE3D_GET_DIM1_EDGE1(inTile);
  const int32_t dim1Edge2     = XI_TILE3D_GET_DIM1_EDGE2(inTile);
  const int32_t dim2Edge1     = XI_TILE3D_GET_DIM2_EDGE1(inTile);
  const int32_t dim2Edge2     = XI_TILE3D_GET_DIM2_EDGE2(inTile);
  const int32_t dim3Edge1     = XI_TILE3D_GET_DIM3_EDGE1(inTile);
  const int32_t dim3Edge2     = XI_TILE3D_GET_DIM3_EDGE2(inTile);
  const int32_t inDataPitch1  = XI_TILE3D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2  = XI_TILE3D_GET_DIM2_PITCH(inTile);
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
  const int32_t dim3Size      = XI_TILE3D_GET_DIM3(inTile);

  /* Input and Output Data Pointers */
  uint8_t *pInput = (uint8_t *) XI_TILE3D_GET_DATA_PTR(inTile);
  int8_t *pOutput = (int8_t *) XI_TILE3D_GET_DATA_PTR(outTile);

  /* Move pointer to include edges */
  pInput = &pInput[-(dim3Edge1 * inDataPitch2 + dim2Edge1 * inDataPitch1 + dim1Edge1)];
  pOutput = &pOutput[-(dim3Edge1 * outDataPitch2 + dim2Edge1 * outDataPitch1 + dim1Edge1)];

  xb_vec2Nx8U* restrict pdvecIn;
  xb_vec2Nx8* restrict pdvecOut;

  valign vaInData;
  valign vaOutData = IVP_ZALIGN();
  xb_vec2Nx8U dvecValue1, dvecValue2, dvecValue3, dvecValue4;
  xb_vec2Nx8 dvecValueSigned1, dvecValueSigned2, dvecValueSigned3, dvecValueSigned4;
  const xb_vec2Nx8U dvecScalar128 = (xb_vec2Nx8U)(128);

  /* In the implementation, vectorization is */
  /* always done across the first dimension  */
  int32_t vectorizationWidth = 2 * XCHAL_IVPN_SIMD_WIDTH;
  int32_t vectorizationWidth2X = 2 * vectorizationWidth;
  int32_t vectorizationWidth3X = 3 * vectorizationWidth;
  int32_t vectorizationWidth4X = 4 * vectorizationWidth;
  int32_t x, y, z;

  /******************************************************************************/
  /* The overall design approach is split into 2 parts                          */
  /* 1. When input and output pitches are equal                                 */
  /*    and pitch = dimEgde1 + dimSize + dimEgde2                               */
  /*    - If above condition holds good, data elements for which unsigned       */
  /*      8 bit to signed 8 bit conversion need to done present in contiguous   */
  /*      memory location. Hence vectorization can be utilized effectively      */
  /*                                                                            */
  /* 2. When input and output pitches are unequal                               */
  /*    or pitch != dimEgde1 + dimSize + dimEgde2                               */
  /******************************************************************************/

  if ((inDataPitch2 == outDataPitch2) && (inDataPitch1 == outDataPitch1) &&
       (inDataPitch1 == dim1Edge1 + dim1Size + dim1Edge2) &&
       (inDataPitch2 == (dim2Edge1 + dim2Size + dim2Edge2) * inDataPitch1))
  {

    int32_t totalSize = inDataPitch2 * (dim3Edge1 + dim3Size + dim3Edge2);
    pdvecIn = (xb_vec2Nx8U*)(pInput);
    pdvecOut = (xb_vec2Nx8*)(pOutput);
    vaInData = IVP_LA2NX8U_PP(pdvecIn);
    for (x = 0; x < totalSize - vectorizationWidth4X; x += vectorizationWidth4X)
    {
      /* Load Data */
      IVP_LA2NX8U_IP(dvecValue1, vaInData, pdvecIn);
      IVP_LA2NX8U_IP(dvecValue2, vaInData, pdvecIn);
      IVP_LA2NX8U_IP(dvecValue3, vaInData, pdvecIn);
      IVP_LA2NX8U_IP(dvecValue4, vaInData, pdvecIn);

      /* Subtract the scalar value 128 from the input vector */
      dvecValueSigned1 = IVP_SUB2NX8U(dvecValue1, dvecScalar128);
      dvecValueSigned2 = IVP_SUB2NX8U(dvecValue2, dvecScalar128);
      dvecValueSigned3 = IVP_SUB2NX8U(dvecValue3, dvecScalar128);
      dvecValueSigned4 = IVP_SUB2NX8U(dvecValue4, dvecScalar128);

      /* Store Data */
      IVP_SA2NX8_IP(dvecValueSigned1, vaOutData, pdvecOut);
      IVP_SA2NX8_IP(dvecValueSigned2, vaOutData, pdvecOut);
      IVP_SA2NX8_IP(dvecValueSigned3, vaOutData, pdvecOut);
      IVP_SA2NX8_IP(dvecValueSigned4, vaOutData, pdvecOut);
    }
    int32_t varLen = totalSize - x;

    /* Load remaining data */
    IVP_LAV2NX8U_XP(dvecValue1, vaInData, pdvecIn, varLen);
    IVP_LAV2NX8U_XP(dvecValue2, vaInData, pdvecIn, varLen - vectorizationWidth);
    IVP_LAV2NX8U_XP(dvecValue3, vaInData, pdvecIn, varLen - vectorizationWidth2X);
    IVP_LAV2NX8U_XP(dvecValue4, vaInData, pdvecIn, varLen - vectorizationWidth3X);

    /* Subtract the scalar value 128 from the input vector */
    dvecValueSigned1 = IVP_SUB2NX8U(dvecValue1, dvecScalar128);
    dvecValueSigned2 = IVP_SUB2NX8U(dvecValue2, dvecScalar128);
    dvecValueSigned3 = IVP_SUB2NX8U(dvecValue3, dvecScalar128);
    dvecValueSigned4 = IVP_SUB2NX8U(dvecValue4, dvecScalar128);

    /* Variable stores */
    IVP_SAV2NX8_XP(dvecValueSigned1, vaOutData, pdvecOut, varLen);
    IVP_SAV2NX8_XP(dvecValueSigned2, vaOutData, pdvecOut, varLen - vectorizationWidth);
    IVP_SAV2NX8_XP(dvecValueSigned3, vaOutData, pdvecOut, varLen - vectorizationWidth2X);
    IVP_SAV2NX8_XP(dvecValueSigned4, vaOutData, pdvecOut, varLen - vectorizationWidth3X);
    IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
  }
  else
  {
    /* else block is executed if input tile pitch is not equal to input tile width or input tile */
    /* pitch is not equal to output tile pitch                                                   */

    for (z = 0; z < dim3Edge1 + dim3Size + dim3Edge2; z++) /* Loop across dim3 */
    {
      /* Loop across dimension 1 */
      for (x = 0; x < (dim1Edge1 + dim1Size + dim1Edge2 - vectorizationWidth); x += vectorizationWidth2X)
      {
        /* initialize input and output data pointer */
        uint8_t *pInput1  = pInput + x + (z * inDataPitch2);
        int8_t *pOutput1 = pOutput + x + (z * outDataPitch2);
        int32_t varLen = dim1Edge1 + dim1Size + dim1Edge2 - (x + vectorizationWidth);

        pdvecIn  = (xb_vec2Nx8U *)pInput1;

        for (y = 0; y < dim2Edge1 + dim2Size + dim2Edge2; y++)
        {
          pdvecOut = (xb_vec2Nx8 *) (pOutput1 + (y * outDataPitch1));
          vaInData = IVP_LA2NX8U_PP(pdvecIn);

          /* Load Input Data */
          IVP_LA2NX8U_IP(dvecValue1, vaInData, pdvecIn);
          IVP_LA2NX8U_XP(dvecValue2, vaInData, pdvecIn, inDataPitch1 - vectorizationWidth);

          /* Subtract the scalar value 128 from the input vector */
          dvecValueSigned1 = IVP_SUB2NX8U(dvecValue1, dvecScalar128);
          dvecValueSigned2 = IVP_SUB2NX8U(dvecValue2, dvecScalar128);

          /* Store */
          IVP_SA2NX8_IP(dvecValueSigned1, vaOutData, pdvecOut);
          IVP_SAV2NX8_XP(dvecValueSigned2, vaOutData, pdvecOut, varLen);
          IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        }
      }
      /* dim1Edge1 + dim1Size + dim1Edge2 - x <= vectorizationWidth */
      if (x < dim1Edge1 + dim1Size + dim1Edge2)
      {
        /* initialize input and output data pointer */
        uint8_t *pInput1  = pInput + x + (z * inDataPitch2);
        int8_t *pOutput1 = pOutput + x + (z * outDataPitch2);
        int32_t varLen = dim1Edge1 + dim1Size + dim1Edge2 - x;

        pdvecIn  = (xb_vec2Nx8U *)pInput1;

        for (y = 0; y < dim2Edge1 + dim2Size + dim2Edge2; y++)
        {
          pdvecOut = (xb_vec2Nx8 *) (pOutput1 + (y * outDataPitch1));
          vaInData = IVP_LA2NX8U_PP(pdvecIn);

          /* Load Input Data */
          IVP_LA2NX8U_XP(dvecValue1, vaInData, pdvecIn, inDataPitch1);

          /* Subtract the scalar value 128 from the input vector */
          dvecValueSigned1 = IVP_SUB2NX8U(dvecValue1, dvecScalar128);

          /* Store */
          IVP_SAV2NX8_XP(dvecValueSigned1, vaOutData, pdvecOut, varLen);
          IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
        }
      }
    }
  }
  return(XI_ERROR_STATUS());
}

#if 0
/************************* xiSubtractA3D_const128_I8 ****************************/
/* Description : Reference implementation of xiSubtractA3D_const128_I8 for      */
/*               Android NN which subtracts constant value 128 from the input.  */
/*               This function supports in-place operation, if the same tile is */
/*               provided as both input and output.                             */
/* Inputs      : Input 3D tile                                                  */
/* Outputs     : XI Error Code                                                  */
/* InOuts      : Output 3D Tile                                                 */
/* Assumptions : Input tile is U8/S8                                            */
/*               Output tile is S8/U8                                           */
/********************************************************************************/

XI_ERR_TYPE xiSubtractA3D_const128_I8(const xi_pTile3D inTile,
                                        xi_pTile3D outTile)
{
  /* Error Checks */
  XI_ERROR_CHECKS()
  {
    XI_CHECK_TILE3D(inTile);
    XI_CHECK_TILE3D(outTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_TILE3D_I8(inTile);
    XI_CHECK_TILE3D_I8(outTile);
    XI_CHECK_TILE3D_EQUAL(inTile, outTile);
    XI_CHECK_ERROR(XI_TILE3D_GET_DATA_ORDER(inTile) == XI_TILE3D_GET_DATA_ORDER(outTile), XI_ERR_BADARG,\
      "\nData Order of InputTile = %d OutputTile = %d\nData Order of InputTile and OutputTile should be same", \
      XI_TILE3D_GET_DATA_ORDER(inTile), XI_TILE3D_GET_DATA_ORDER(outTile));
  }

  /* Getting parameters from the tile structures */
  const int32_t dim1Size      = XI_TILE3D_GET_DIM1(inTile);
  const int32_t dim2Size      = XI_TILE3D_GET_DIM2(inTile);
  const int32_t inDataPitch1  = XI_TILE3D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2  = XI_TILE3D_GET_DIM2_PITCH(inTile);
  const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
  const int32_t dim3Size      = XI_TILE3D_GET_DIM3(inTile);

  /* Input and Output Data Pointers */
  int8_t *pInput = (int8_t *) XI_TILE3D_GET_DATA_PTR(inTile);
  int8_t *pOutput = (int8_t *) XI_TILE3D_GET_DATA_PTR(outTile);

  valign vaInData;
  valign vaOutData = IVP_ZALIGN();
  xb_vec2Nx8 dvecValue1, dvecValue2, dvecValue3, dvecValue4;
  xb_vec2Nx8 dvecValueSigned1, dvecValueSigned2, dvecValueSigned3, dvecValueSigned4;
  const xb_vec2Nx8U dvecScalar128 = (xb_vec2Nx8U)(128);

  /* In the implementation, vectorization is */
  /* always done across the first dimension  */
  int32_t vectorizationWidth = 2 * XCHAL_IVPN_SIMD_WIDTH;
  int32_t vectorizationWidth2X = 2 * vectorizationWidth;
  int32_t vectorizationWidth3X = 3 * vectorizationWidth;
  int32_t vectorizationWidth4X = 4 * vectorizationWidth;
  int32_t x, y, z;
  xb_vec2Nx8* pdvecIn1, *pdvecOut1, *pdvecIn2, *pdvecOut2;
  valign vaInData1, vaOutData1;
  vaOutData1 = IVP_ZALIGN();
  /******************************************************************************/
  /* The overall design approach is split into 2 parts                          */
  /* 1. When input and output pitches are equal in first and second dimension   */
  /*    - If above condition holds good, data elements for which unsigned       */
  /*      8 bit to signed 8 bit conversion need to done present in contiguous   */
  /*      memory location. Hence vectorization can be utilized effectively      */
  /*                                                                            */
  /* 2. When input and output pitches are unequal                               */
  /******************************************************************************/

  if ((inDataPitch1 == dim1Size) && (inDataPitch1 == outDataPitch1))
  {
    /******************************************************************************/
    /* Data exist in contiguous memory location with respect to first dimension   */
    /******************************************************************************/

    /* Initialize MAX2 loop counter */
    int32_t dim3MaxLoopCount = dim3Size;
    int32_t maxLoopCount = dim1Size * dim2Size;

    /* Updated Loop count based on tile dimension configuration */
    if ((inDataPitch2 == maxLoopCount) && (inDataPitch2 == outDataPitch2))
    {
      /**********************************************************************/
      /* Data exist in contiguous memory location with respect to first and */
      /* second dimension                                                   */
      /**********************************************************************/
      dim3MaxLoopCount = 1;       /* Update MAX2 loop counter */
      maxLoopCount *= dim3Size;
    }
    for (z = 0; z < dim3MaxLoopCount; z++)
    {
      pdvecIn1 = (xb_vec2Nx8*)(pInput + (z * inDataPitch2));
      pdvecOut1 = (xb_vec2Nx8*)(pOutput + (z * outDataPitch2));
      vaInData = IVP_LA2NX8_PP(pdvecIn1);
      for (x = 0; x < maxLoopCount - vectorizationWidth4X; x += vectorizationWidth4X)
      {
        /* Load Data */
        IVP_LA2NX8U_IP(dvecValue1, vaInData, pdvecIn1);
        IVP_LA2NX8U_IP(dvecValue2, vaInData, pdvecIn1);
        IVP_LA2NX8U_IP(dvecValue3, vaInData, pdvecIn1);
        IVP_LA2NX8U_IP(dvecValue4, vaInData, pdvecIn1);

        /* Subtract the scalar value 128 from the input vector */
        dvecValueSigned1 = IVP_XOR2NX8(dvecValue1, dvecScalar128);
        dvecValueSigned2 = IVP_XOR2NX8(dvecValue2, dvecScalar128);
        dvecValueSigned3 = IVP_XOR2NX8(dvecValue3, dvecScalar128);
        dvecValueSigned4 = IVP_XOR2NX8(dvecValue4, dvecScalar128);

        /* Store Data */
        IVP_SA2NX8_IP(dvecValueSigned1, vaOutData, pdvecOut1);
        IVP_SA2NX8_IP(dvecValueSigned2, vaOutData, pdvecOut1);
        IVP_SA2NX8_IP(dvecValueSigned3, vaOutData, pdvecOut1);
        IVP_SA2NX8_IP(dvecValueSigned4, vaOutData, pdvecOut1);
      }
      int32_t varLen = maxLoopCount - x;

      /* Load remaining data */
      IVP_LAV2NX8_XP(dvecValue1, vaInData, pdvecIn1, varLen);
      IVP_LAV2NX8_XP(dvecValue2, vaInData, pdvecIn1, varLen - vectorizationWidth);
      IVP_LAV2NX8_XP(dvecValue3, vaInData, pdvecIn1, varLen - vectorizationWidth2X);
      IVP_LAV2NX8_XP(dvecValue4, vaInData, pdvecIn1, varLen - vectorizationWidth3X);

      /* Subtract the scalar value 128 from the input vector */
      dvecValueSigned1 = IVP_XOR2NX8(dvecValue1, dvecScalar128);
      dvecValueSigned2 = IVP_XOR2NX8(dvecValue2, dvecScalar128);
      dvecValueSigned3 = IVP_XOR2NX8(dvecValue3, dvecScalar128);
      dvecValueSigned4 = IVP_XOR2NX8(dvecValue4, dvecScalar128);

      /* Variable stores */
      IVP_SAV2NX8_XP(dvecValueSigned1, vaOutData, pdvecOut1, varLen);
      IVP_SAV2NX8_XP(dvecValueSigned2, vaOutData, pdvecOut1, varLen - vectorizationWidth);
      IVP_SAV2NX8_XP(dvecValueSigned3, vaOutData, pdvecOut1, varLen - vectorizationWidth2X);
      IVP_SAV2NX8_XP(dvecValueSigned4, vaOutData, pdvecOut1, varLen - vectorizationWidth3X);
      IVP_SAPOS2NX8_FP(vaOutData, pdvecOut1);
    }
  }
  else
  {
    /* else block is executed if input tile pitch is not equal to input tile width or input tile */
    /* pitch is not equal to output tile pitch                                                   */

    /* Loop across dimension 1 */
    for (x = 0; x < dim1Size; x += vectorizationWidth)
    {
      int32_t remVal = XT_MIN(dim1Size - x, vectorizationWidth);
      for (z = 0; z < dim3Size; z+=2) /* Loop across dim3 */
      {
        int32_t remZ = XT_MIN(2, (dim3Size - z)) - 1;
        /* initialize input and output data pointer */
        int8_t *pInput1  = pInput + x + (z * inDataPitch2);
        int8_t *pInput2  = pInput + x + (z + remZ) * inDataPitch2;
        int8_t *pOutput1 = pOutput + x + (z * outDataPitch2);
        int8_t *pOutput2 = pOutput + x + (z + remZ) * outDataPitch2;

        pdvecIn1  = (xb_vec2Nx8 *)pInput1;
        pdvecIn2 = (xb_vec2Nx8 *)pInput2;

        pdvecOut1 = (xb_vec2Nx8 *)(pOutput1);
        pdvecOut2 = (xb_vec2Nx8 *)(pOutput2);

        for (y = 0; y < dim2Size ; y++)
        {
          /* Load Input Data */
          vaInData  = IVP_LA2NX8_PP(pdvecIn1);
          vaInData1 = IVP_LA2NX8_PP(pdvecIn2);

          IVP_LA2NX8U_XP(dvecValue1, vaInData, pdvecIn1, inDataPitch1);
          IVP_LA2NX8U_XP(dvecValue2, vaInData1, pdvecIn2, inDataPitch1);

          /* Subtract the scalar value 128 from the input vector */
          dvecValueSigned1 = IVP_XOR2NX8(dvecValue1, dvecScalar128);
          dvecValueSigned2 = IVP_XOR2NX8(dvecValue2, dvecScalar128);

          /* Store */
          IVP_SAV2NX8_XP(dvecValueSigned1, vaOutData, pdvecOut1, remVal);
          FLUSH2NX8(vaOutData, pdvecOut1, outDataPitch1, remVal);
          IVP_SAV2NX8_XP(dvecValueSigned2, vaOutData1, pdvecOut2, remVal * remZ);
          FLUSH2NX8(vaOutData1, pdvecOut2, outDataPitch1, remVal * remZ);
        }
      }
    }
  }
  return(XI_ERROR_STATUS());
}


/************************* xiSubtractA4D_const128_I8 ****************************/
/* Description : Reference implementation of xiSubtractA4D_const128_I8 for      */
/*               Android NN which subtracts constant value 128 from the input.  */
/*               This function supports in-place operation, if the same tile is */
/*               provided as both input and output.                             */
/* Inputs      : Input 4D tile                                                  */
/* Outputs     : XI Error Code                                                  */
/* InOuts      : Output 4D Tile                                                 */
/* Assumptions : Input tile is U8/S8                                            */
/*               Output tile is S8/U8                                           */
/********************************************************************************/

XI_ERR_TYPE xiSubtractA4D_const128_I8(const xi_pTile4D inTile,
                                      xi_pTile4D outTile)
{
  /* Error Checks */
  XI_ERROR_CHECKS()
  {
    XI_CHECK_TILE4D(inTile);
    XI_CHECK_TILE4D(outTile);
    XI_CHECK_TILE4D_IN_DRAM_BOUNDARY(inTile);
    XI_CHECK_TILE4D_IN_DRAM_BOUNDARY(outTile);
    XI_CHECK_TILE4D_I8(inTile);
    XI_CHECK_TILE4D_I8(outTile);
    XI_CHECK_TILE4D_EQUAL(inTile, outTile);
    XI_CHECK_ERROR(XI_TILE4D_GET_DATA_ORDER(inTile) == XI_TILE4D_GET_DATA_ORDER(outTile), XI_ERR_BADARG,\
      "\nData Order of InputTile = %d OutputTile = %d\nData Order of InputTile and OutputTile should be same", \
      XI_TILE3D_GET_DATA_ORDER(inTile), XI_TILE3D_GET_DATA_ORDER(outTile));
  }

  /* Getting parameters from the tile structures */
  const int32_t dim1Size      = XI_TILE4D_GET_DIM1(inTile);
  const int32_t dim2Size      = XI_TILE4D_GET_DIM2(inTile);
  const int32_t dim3Size      = XI_TILE4D_GET_DIM3(inTile);
  const int32_t dim4Size      = XI_TILE4D_GET_DIM4(inTile);
  const int32_t inDataPitch1  = XI_TILE4D_GET_DIM1_PITCH(inTile);
  const int32_t inDataPitch2  = XI_TILE4D_GET_DIM2_PITCH(inTile);
  const int32_t inDataPitch3  = XI_TILE4D_GET_DIM3_PITCH(inTile);
  const int32_t outDataPitch1 = XI_TILE4D_GET_DIM1_PITCH(outTile);
  const int32_t outDataPitch2 = XI_TILE4D_GET_DIM2_PITCH(outTile);
  const int32_t outDataPitch3 = XI_TILE4D_GET_DIM3_PITCH(outTile);

  /* Input and Output Data Pointers */
  int8_t *pInput = (int8_t *) XI_TILE4D_GET_DATA_PTR(inTile);
  int8_t *pOutput = (int8_t *) XI_TILE4D_GET_DATA_PTR(outTile);

  valign vaInData;
  valign vaOutData = IVP_ZALIGN();
  xb_vec2Nx8 dvecValue1, dvecValue2, dvecValue3, dvecValue4;
  xb_vec2Nx8 dvecValueSigned1, dvecValueSigned2, dvecValueSigned3, dvecValueSigned4;
  const xb_vec2Nx8U dvecScalar128 = (xb_vec2Nx8U)(128);

  /* In the implementation, vectorization is */
  /* always done across the first dimension  */
  int32_t vectorizationWidth = 2 * XCHAL_IVPN_SIMD_WIDTH;
  int32_t vectorizationWidth2X = 2 * vectorizationWidth;
  int32_t vectorizationWidth3X = 3 * vectorizationWidth;
  int32_t vectorizationWidth4X = 4 * vectorizationWidth;
  int32_t x, y, z, n;
  xb_vec2Nx8* pdvecIn1, *pdvecOut1, *pdvecIn2, *pdvecOut2;
  valign vaInData1, vaOutData1;
  vaOutData1 = IVP_ZALIGN();
  /******************************************************************************/
  /* The overall design approach is split into 2 parts                          */
  /* 1. When input and output pitches are equal in first and second dimension   */
  /*    - If above condition holds good, data elements for which unsigned       */
  /*      8 bit to signed 8 bit conversion need to done present in contiguous   */
  /*      memory location. Hence vectorization can be utilized effectively      */
  /*                                                                            */
  /* 2. When input and output pitches are unequal                               */
  /******************************************************************************/

  if ((inDataPitch1 == dim1Size) && (inDataPitch1 == outDataPitch1))
  {
    /******************************************************************************/
    /* Data exist in contiguous memory location with respect to first dimension   */
    /******************************************************************************/

    /* Initialize max loop counter */
    int32_t dim4MaxLoopCount = dim4Size;
    int32_t dim3MaxLoopCount = dim3Size;
    int32_t maxLoopCount = dim1Size * dim2Size;

    /* Updated Loop count based on tile dimension configuration */
    if ((inDataPitch2 == maxLoopCount) && (inDataPitch2 == outDataPitch2))
    {
      /**********************************************************************/
      /* Data exist in contiguous memory location with respect to first and */
      /* second dimension                                                   */
      /**********************************************************************/
      dim3MaxLoopCount = 1;       /* Update max loop counter */
      maxLoopCount *= dim3Size;
      if ((inDataPitch3 == maxLoopCount) && (inDataPitch3 == outDataPitch3))
      {
        /**********************************************************************/
        /* Data exist in contiguous memory location with respect to first,    */
        /* second and third dimensions                                        */
        /**********************************************************************/
        dim4MaxLoopCount = 1;
        maxLoopCount *= dim4Size;
      }
    }
    for (n = 0; n < dim4MaxLoopCount; n++)
    {
      for (z = 0; z < dim3MaxLoopCount; z++)
      {
        pdvecIn1 = (xb_vec2Nx8*)(pInput + (z * inDataPitch2) + (n * inDataPitch3));
        pdvecOut1 = (xb_vec2Nx8*)(pOutput + (z * outDataPitch2) + (n * outDataPitch3));
        vaInData = IVP_LA2NX8_PP(pdvecIn1);
        for (x = 0; x < maxLoopCount - vectorizationWidth4X; x += vectorizationWidth4X)
        {
          /* Load Data */
          IVP_LA2NX8U_IP(dvecValue1, vaInData, pdvecIn1);
          IVP_LA2NX8U_IP(dvecValue2, vaInData, pdvecIn1);
          IVP_LA2NX8U_IP(dvecValue3, vaInData, pdvecIn1);
          IVP_LA2NX8U_IP(dvecValue4, vaInData, pdvecIn1);

          /* Subtract the scalar value 128 from the input vector */
          dvecValueSigned1 = IVP_XOR2NX8(dvecValue1, dvecScalar128);
          dvecValueSigned2 = IVP_XOR2NX8(dvecValue2, dvecScalar128);
          dvecValueSigned3 = IVP_XOR2NX8(dvecValue3, dvecScalar128);
          dvecValueSigned4 = IVP_XOR2NX8(dvecValue4, dvecScalar128);

          /* Store Data */
          IVP_SA2NX8_IP(dvecValueSigned1, vaOutData, pdvecOut1);
          IVP_SA2NX8_IP(dvecValueSigned2, vaOutData, pdvecOut1);
          IVP_SA2NX8_IP(dvecValueSigned3, vaOutData, pdvecOut1);
          IVP_SA2NX8_IP(dvecValueSigned4, vaOutData, pdvecOut1);
        }
        int32_t varLen = maxLoopCount - x;

        /* Load remaining data */
        IVP_LAV2NX8_XP(dvecValue1, vaInData, pdvecIn1, varLen);
        IVP_LAV2NX8_XP(dvecValue2, vaInData, pdvecIn1, varLen - vectorizationWidth);
        IVP_LAV2NX8_XP(dvecValue3, vaInData, pdvecIn1, varLen - vectorizationWidth2X);
        IVP_LAV2NX8_XP(dvecValue4, vaInData, pdvecIn1, varLen - vectorizationWidth3X);

        /* Subtract the scalar value 128 from the input vector */
        dvecValueSigned1 = IVP_XOR2NX8(dvecValue1, dvecScalar128);
        dvecValueSigned2 = IVP_XOR2NX8(dvecValue2, dvecScalar128);
        dvecValueSigned3 = IVP_XOR2NX8(dvecValue3, dvecScalar128);
        dvecValueSigned4 = IVP_XOR2NX8(dvecValue4, dvecScalar128);

        /* Variable stores */
        IVP_SAV2NX8_XP(dvecValueSigned1, vaOutData, pdvecOut1, varLen);
        IVP_SAV2NX8_XP(dvecValueSigned2, vaOutData, pdvecOut1, varLen - vectorizationWidth);
        IVP_SAV2NX8_XP(dvecValueSigned3, vaOutData, pdvecOut1, varLen - vectorizationWidth2X);
        IVP_SAV2NX8_XP(dvecValueSigned4, vaOutData, pdvecOut1, varLen - vectorizationWidth3X);
        IVP_SAPOS2NX8_FP(vaOutData, pdvecOut1);
      }
    }
  }
  else
  {
    /* else block is executed if input tile pitch is not equal to input tile width or input tile */
    /* pitch is not equal to output tile pitch                                                   */

    /* Loop across dimension 1 */
    for (x = 0; x < dim1Size; x += vectorizationWidth)
    {
      int32_t remVal = XT_MIN(dim1Size - x, vectorizationWidth);
      for (n = 0; n < dim4Size; n++)
      {
        for (z = 0; z < dim3Size; z += 2) /* Loop across dim3 */
        {
          int32_t remZ = XT_MIN(2, (dim3Size - z)) - 1;
          /* initialize input and output data pointer */
          int8_t *pInput1 = pInput + x + (z * inDataPitch2) + (n * inDataPitch3);
          int8_t *pInput2 = pInput + x + (z + remZ) * inDataPitch2 + (n * inDataPitch3);
          int8_t *pOutput1 = pOutput + x + (z * outDataPitch2) + (n * outDataPitch3);
          int8_t *pOutput2 = pOutput + x + (z + remZ) * outDataPitch2 + (n * outDataPitch3);

          pdvecIn1 = (xb_vec2Nx8 *)pInput1;
          pdvecIn2 = (xb_vec2Nx8 *)pInput2;

          pdvecOut1 = (xb_vec2Nx8 *)(pOutput1);
          pdvecOut2 = (xb_vec2Nx8 *)(pOutput2);

          for (y = 0; y < dim2Size; y++)
          {
            /* Load Input Data */
            vaInData = IVP_LA2NX8_PP(pdvecIn1);
            vaInData1 = IVP_LA2NX8_PP(pdvecIn2);

            IVP_LA2NX8U_XP(dvecValue1, vaInData, pdvecIn1, inDataPitch1);
            IVP_LA2NX8U_XP(dvecValue2, vaInData1, pdvecIn2, inDataPitch1);

            /* Subtract the scalar value 128 from the input vector */
            dvecValueSigned1 = IVP_XOR2NX8(dvecValue1, dvecScalar128);
            dvecValueSigned2 = IVP_XOR2NX8(dvecValue2, dvecScalar128);

            /* Store */
            IVP_SAV2NX8_XP(dvecValueSigned1, vaOutData, pdvecOut1, remVal);
            FLUSH2NX8(vaOutData, pdvecOut1, outDataPitch1, remVal);
            IVP_SAV2NX8_XP(dvecValueSigned2, vaOutData1, pdvecOut2, remVal * remZ);
            FLUSH2NX8(vaOutData1, pdvecOut2, outDataPitch1, remVal * remZ);
          }
        }
      }
    }
  }
  return(XI_ERROR_STATUS());
}
#endif
#endif //if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))
