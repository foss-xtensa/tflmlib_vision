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
#include <assert.h>

#if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))


static void extendEdgesConst3D_I8(xi_pTile3D dstTile,
                                  const int32_t constValue,
                                  xi_size3D frame3DSize);

static void extendEdgesConst3D_I16(xi_pTile3D dstTile,
                                   const int32_t constValue,
                                   xi_size3D frame3DSize);

static void extendWHEdges3D_I8(xi_pTile3D dstTile,
                               const int8_t * pValue,
                               xi_size3D frame3DSize);

static void extendWHEdges3D_I16(xi_pTile3D dstTile,
                                const int16_t * pValue,
                                xi_size3D frame3DSize);

#define INTEGER8BIT  1
#define INTEGER16BIT 2

#undef MAKE_NAME_IMPL
#undef MAKE_NAME

#define MAKE_NAME_IMPL(name, MORPH_FNAME_SPECIFIER_IDT) name##_##MORPH_FNAME_SPECIFIER_IDT
#define MAKE_NAME_IMPL_1(name, MORPH_FNAME_SPECIFIER_IDT, dataOrder) name##_##MORPH_FNAME_SPECIFIER_IDT##_##dataOrder

#if INPUT_DATA_TYPE == INTEGER8BIT

#undef MAKE_NAME
#undef MAKE_NAME_1
#undef MORPH_OP_FUNCTION
#undef MORPH_OP_FUNCTION_CONST
#undef MORPH_IDT_CHECK
#undef MORPH_ADT_CHECK
#undef MORPH_IDT_SCALAR
#undef MORPH_IDT_FILLTILE
#undef MORPH_OP_LOAD
#undef MORPH_OP_AND
#undef MORPH_OP_SEQ
#undef MORPH_OP_SEL
#undef MORPH_OP_STORE
#undef MORPH_OP_PRIME
#undef MORPH_IDT_VEC
#undef MORPH_OP_FLUSH
#undef MORPH_VECTORIZATIONWIDTH

#define MORPH_OP_FUNCTION       extendWHEdges3D_I8
#define MORPH_OP_FUNCTION_CONST extendEdgesConst3D_I8
#define MAKE_NAME(name)         MAKE_NAME_IMPL(name, I8)
#define MAKE_NAME_1(name, dataOrder) MAKE_NAME_IMPL_1(name, I8, dataOrder)
#define MORPH_IDT_CHECK         XI_CHECK_TILE3D_I8
#define MORPH_ADT_CHECK         XI_CHECK_ARRAY_I8
#define MORPH_IDT_SCALAR        int8_t
#define MORPH_IDT_FILLTILE      xiFillTile3D_I8
#define MORPH_OP_LOAD           IVP_LAV2NX8_XP
#define MORPH_OP_AND            IVP_AND2NX8
#define MORPH_OP_SEQ            IVP_SEQ2NX8
#define MORPH_OP_SEL            IVP_SEL2NX8
#define MORPH_OP_STORE          IVP_SAV2NX8_XP
#define MORPH_OP_PRIME          IVP_LA2NX8_PP
#define MORPH_IDT_VEC           xb_vec2Nx8
#define MORPH_OP_FLUSH          IVP_SAPOS2NX8_FP
#define MORPH_VECTORIZATIONWIDTH 2 * XCHAL_IVPN_SIMD_WIDTH

#elif INPUT_DATA_TYPE == INTEGER16BIT

#undef MAKE_NAME
#undef MAKE_NAME_1
#undef MORPH_OP_FUNCTION
#undef MORPH_OP_FUNCTION_CONST
#undef MORPH_IDT_CHECK
#undef MORPH_ADT_CHECK
#undef MORPH_IDT_SCALAR
#undef MORPH_IDT_FILLTILE
#undef MORPH_OP_LOAD
#undef MORPH_OP_AND
#undef MORPH_OP_SEQ
#undef MORPH_OP_SEL
#undef MORPH_OP_STORE
#undef MORPH_OP_PRIME
#undef MORPH_IDT_VEC
#undef MORPH_OP_FLUSH
#undef MORPH_VECTORIZATIONWIDTH

#define MAKE_NAME(name)         MAKE_NAME_IMPL(name, I16)
#define MAKE_NAME_1(name, dataOrder) MAKE_NAME_IMPL_1(name, I16, dataOrder)
#define MORPH_OP_FUNCTION       extendWHEdges3D_I16
#define MORPH_OP_FUNCTION_CONST extendEdgesConst3D_I16
#define MORPH_IDT_CHECK         XI_CHECK_TILE3D_I16
#define MORPH_ADT_CHECK         XI_CHECK_ARRAY_I16
#define MORPH_IDT_SCALAR        int16_t
#define MORPH_IDT_FILLTILE      xiFillTile3D_I16
#define MORPH_OP_LOAD           IVP_LAVNX16_XP
#define MORPH_OP_AND            IVP_ANDNX16
#define MORPH_OP_SEQ            IVP_SEQNX16
#define MORPH_OP_SEL            IVP_SELNX16
#define MORPH_OP_STORE          IVP_SAVNX16_XP
#define MORPH_OP_PRIME          IVP_LANX16_PP
#define MORPH_IDT_VEC           xb_vecNx16
#define MORPH_OP_FLUSH          IVP_SAPOSNX16_FP
#define MORPH_VECTORIZATIONWIDTH XCHAL_IVPN_SIMD_WIDTH

#endif


/****************************   xiExtendEdgesConst3D  ****************************/
/* Description : P6 optimized generic implementation of xiExtendEdgesConst 3D    */
/*               function. Based on MORPH pre-processor specifiers, code         */
/*               implementation is generated during preprocessing stage. This    */
/*               method implements xiExtendEdgesConst_I8 and                     */
/*               and xiExtendEdgesConst_I16 functionality                        */
/* Inputs      : constant value to fill the edges                                */
/* Outputs     : XI Error Code                                                   */
/* InOuts      : Destination Tile                                                */
/* Assumptions : OutData is signed 8 / 16 bit Interger based on MORPH specifier  */
/*********************************************************************************/

/************************** xiExtendEdgesConst3D_I8 ******************************/
/************************** xiExtendEdgesConst3D_I16 *****************************/

XI_ERR_TYPE MAKE_NAME(xiExtendEdgesConst3D)(xi_pTile3D dstTile,
                                            const int32_t value,
                                            xi_size3D frame3DSize)
{
  /* Error Checks */
  XI_ERROR_CHECKS()
  {
    MORPH_IDT_CHECK(dstTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(dstTile);
    XI_CHECK_ERROR((frame3DSize.dim1Size > 0) && (frame3DSize.dim2Size > 0) && \
      (frame3DSize.dim3Size > 0), XI_ERR_DATASIZE, \
      "Frame dimensions cannot be 0");
  }

#if 1

  /* Getting parameters from the tile structures */
  const int32_t dim1Size = XI_TILE3D_GET_DIM1(dstTile);
  const int32_t dim2Size = XI_TILE3D_GET_DIM2(dstTile);
  const int32_t dim3Size = XI_TILE3D_GET_DIM3(dstTile);
  const int32_t dim1Edge1 = XI_TILE3D_GET_DIM1_EDGE1(dstTile);
  const int32_t dim1Edge2 = XI_TILE3D_GET_DIM1_EDGE2(dstTile);
  const int32_t dim2Edge1 = XI_TILE3D_GET_DIM2_EDGE1(dstTile);
  const int32_t dim2Edge2 = XI_TILE3D_GET_DIM2_EDGE2(dstTile);
  const int32_t dim3Edge1 = XI_TILE3D_GET_DIM3_EDGE1(dstTile);
  const int32_t dim3Edge2 = XI_TILE3D_GET_DIM3_EDGE2(dstTile);
  const int32_t dstDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(dstTile);
  const int32_t dstDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(dstTile);

  MORPH_IDT_SCALAR *pDst = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(dstTile);

  int32_t frame_dim1 = frame3DSize.dim1Size;
  int32_t frame_dim2 = frame3DSize.dim2Size;
  int32_t frame_dim3 = frame3DSize.dim3Size;
  int32_t start_x = XI_TILE3D_GET_DIM1_COORD(dstTile);
  int32_t start_y = XI_TILE3D_GET_DIM2_COORD(dstTile);
  int32_t start_z = XI_TILE3D_GET_DIM3_COORD(dstTile);

  int32_t ixmin = max(start_x - dim1Edge1, 0);
  int32_t ixmax = min(start_x + dim1Size + dim1Edge2 - 1, frame_dim1 - 1);
  int32_t iymin = max(start_y - dim2Edge1, 0);
  int32_t iymax = min(start_y + dim2Size + dim2Edge2 - 1, frame_dim2 - 1);
  int32_t izmin = max(start_z - dim3Edge1, 0);
  int32_t izmax = min(start_z + dim3Size + dim3Edge2 - 1, frame_dim3 - 1);

  /* nothing to extend, because tile and frame intersection is empty */
  if ((ixmin > ixmax) || (iymin > iymax) || (izmin > izmax))
  {
    return(MORPH_IDT_FILLTILE(dstTile, value, 1));
  }

  /*******************************************************************************/
  /* P6 implementation of xiExtendEdgesConst3D is split into 3 parts.            */
  /* If pitch is equal to stride, memory location to be updated across 3rd       */
  /* dimension edges is contiguous. Hence processing across edge can be          */
  /* implemented using FillTile3D functionality. Processing across 3rd dimension */
  /* is split as front end and rear end processing. Processing across 3rd        */
  /* dimension excluding the edge is implemented similar to 2D implementation of */
  /* ExtendEdges functionality.                                                  */
  /*******************************************************************************/

  MORPH_IDT_SCALAR *pDst1;

  /* Number of 2D tiles to be processed across edge1 3rd dimension */
  int32_t dim3SizeFrontEnd = izmin - (start_z - dim3Edge1);
  /* Offset calculation for Extend Edge across 3rd dimension excluding edges */
  int32_t dim3CordMiddle = izmin - start_z;
  /* Number of 2D tiles to be processed across 3rd dimension excluding edges */
  int32_t dim3SizeMiddle = izmax - izmin + 1;
  /* Offset calculation for Extend Edge across edge 2 3rd */
  int32_t dim3CordRearEnd = izmax + 1 - start_z;
  /* Number of 2D tiles processing to Extend Edge across 3rd edge2 dimension */
  int32_t dim3SizeRearEnd = start_z + dim3Size + dim3Edge2 - 1 - izmax;

  /* Update local 3D tile structure with dstTile structure parameters. Local   */
  /* 3D tile structure is used as parameter to implement fillTile functionality */
  xi_tile3D dst_t;
  /* Update parameters for local 3D tile */
  XI_TILE3D_SET_DIM1(&dst_t, dim1Size);
  XI_TILE3D_SET_DIM1_PITCH(&dst_t, dstDataPitch1);
  XI_TILE3D_SET_DIM1_EDGE1(&dst_t, dim1Edge1);
  XI_TILE3D_SET_DIM1_EDGE2(&dst_t, dim1Edge2);
  XI_TILE3D_SET_DIM2(&dst_t, dim2Size);
  XI_TILE3D_SET_DIM2_PITCH(&dst_t, dstDataPitch2);
  XI_TILE3D_SET_DIM2_EDGE1(&dst_t, dim2Edge1);
  XI_TILE3D_SET_DIM2_EDGE2(&dst_t, dim2Edge2);
  XI_TILE3D_SET_DIM3_EDGE1(&dst_t, 0);
  XI_TILE3D_SET_DIM3_EDGE2(&dst_t, 0);
  XI_TILE3D_SET_DIM1_COORD(&dst_t, start_x);
  XI_TILE3D_SET_DIM2_COORD(&dst_t, start_y);
  XI_TILE3D_SET_DIM3_COORD(&dst_t, start_z);
  XI_TILE3D_SET_BUFF_PTR(&dst_t, XI_TILE3D_GET_BUFF_PTR(dstTile));
  XI_TILE3D_SET_BUFF_SIZE(&dst_t, XI_TILE3D_GET_BUFF_SIZE(dstTile));
  XI_TILE3D_SET_TYPE(&dst_t, XI_TILE3D_GET_TYPE(dstTile));

  /***********************************************************************************/
  /* Processing across the 3rd dimension edges (edge1 and edge2)                     */
  /* Processing across 3rd dimension edge 1 is referred as Front End Processing      */
  /* Processing across 3rd dimension edge 2 is referred as Rear End Processing       */
  /* Local copy of 3D tile is declared and updated with destination tile parameters. */
  /* Size parameter across third dimension is updated based on number of 2D tiles    */
  /* to be processed across front and read end. In order to effectively use the      */
  /* SIMD capabilities xiFillTile3D implementation is utilized.                      */
  /***********************************************************************************/
  if (dim3SizeFrontEnd > 0)
  {
    /***********************************************************************************/
    /* Front end processing : Processing along the 3rd dimension edge 1.               */
    /***********************************************************************************/

    /* update destination data pointer */
    pDst1 = &pDst[((-dim3Edge1) * dstDataPitch2)];
    XI_TILE3D_SET_DATA_PTR(&dst_t, pDst1);
    XI_TILE3D_SET_DIM3(&dst_t, dim3SizeFrontEnd);
    MORPH_IDT_FILLTILE(&dst_t, value, 1);
  }
  if (dim3SizeRearEnd > 0)
  {
    /***********************************************************************************/
    /* Rear end processing : Processing along the 3rd dimension edge 2.                */
    /***********************************************************************************/

    /* update destination data pointer */
    pDst1 = &pDst[dim3CordRearEnd * dstDataPitch2];
    XI_TILE3D_SET_DATA_PTR(&dst_t, pDst1);
    XI_TILE3D_SET_DIM3(&dst_t, dim3SizeRearEnd);
    MORPH_IDT_FILLTILE(&dst_t, value, 1);
  }

  /* Update destination data pointer */
  pDst1 = &pDst[(dim3CordMiddle * dstDataPitch2)];
  XI_TILE3D_SET_DIM3(&dst_t, dim3SizeMiddle);
  XI_TILE3D_SET_DATA_PTR(&dst_t, pDst1);

  //MORPH_OP_FUNCTION_CONST(&dst_t, value, frame3DSize);
  // TODO : defined function in function. This is body of extendEdgesConst3D_I8()
  // need to move out to seperate function
  {
	  xi_pTile3D dstTile = &dst_t;
	  const int32_t constValue = value;
      //xi_size3D frame3DSize

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

    int32_t ixmin = max(start_x - dim1Edge1, 0);
    int32_t ixmax = min(start_x + dim1Size + dim1Edge2 - 1, frame_dim1 - 1);
    int32_t iymin = max(start_y - dim2Edge1, 0);
    int32_t iymax = min(start_y + dim2Size + dim2Edge2 - 1, frame_dim2 - 1);
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

#endif
  return(XI_ERROR_STATUS());
}

/***********************   extendEdges3D_I8(I16)_WHD  ****************************/
/* Description : P6 optimized generic implementation of xiExtendEdgesConst 3D    */
/*               function. Based on MORPH pre-processor specifiers, code         */
/*               implementation is generated during preprocessing stage. This    */
/*               method implements extendEdges3D_I8_WHD and extendEdges3D_I16_WHD*/
/*               functionality                                                   */
/* Inputs      : constant value to fill the edges                                */
/* Outputs     : XI Error Code                                                   */
/* InOuts      : Destination Tile                                                */
/* Assumptions : OutData is signed/unsigned 8 / 16 bit Interger                  */
/*               based on MORPH specifier                                        */
/*********************************************************************************/

/***************************** extendEdges3D_I8_WHD ******************************/
/***************************** extendEdges3D_I16_WHD *****************************/

void MAKE_NAME_1(extendEdges3D, WHD)(xi_pTile3D dstTile,
                                     const xi_pArray pArray,
                                     xi_size3D frame3DSize)
{
  assert(0);
#if 0

  /* Getting parameters from the tile structures */
  const int32_t dim1Size = XI_TILE3D_GET_DIM1(dstTile);
  const int32_t dim2Size = XI_TILE3D_GET_DIM2(dstTile);
  const int32_t dim3Size = XI_TILE3D_GET_DIM3(dstTile);
  const int32_t dim1Edge1 = XI_TILE3D_GET_DIM1_EDGE1(dstTile);
  const int32_t dim1Edge2 = XI_TILE3D_GET_DIM1_EDGE2(dstTile);
  const int32_t dim2Edge1 = XI_TILE3D_GET_DIM2_EDGE1(dstTile);
  const int32_t dim2Edge2 = XI_TILE3D_GET_DIM2_EDGE2(dstTile);
  const int32_t dim3Edge1 = XI_TILE3D_GET_DIM3_EDGE1(dstTile);
  const int32_t dim3Edge2 = XI_TILE3D_GET_DIM3_EDGE2(dstTile);
  const int32_t dstDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(dstTile);
  const int32_t dstDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(dstTile);

  int32_t frame_dim1 = frame3DSize.dim1Size;
  int32_t frame_dim2 = frame3DSize.dim2Size;
  int32_t frame_dim3 = frame3DSize.dim3Size;
  int32_t start_x = XI_TILE3D_GET_DIM1_COORD(dstTile);
  int32_t start_y = XI_TILE3D_GET_DIM2_COORD(dstTile);
  int32_t start_z = XI_TILE3D_GET_DIM3_COORD(dstTile);

  int32_t ixmin = max(start_x - dim1Edge1, 0);
  int32_t ixmax = min(start_x + dim1Size + dim1Edge2 - 1, frame_dim1 - 1);
  int32_t iymin = max(start_y - dim2Edge1, 0);
  int32_t iymax = min(start_y + dim2Size + dim2Edge2 - 1, frame_dim2 - 1);
  int32_t izmin = max(start_z - dim3Edge1, 0);
  int32_t izmax = min(start_z + dim3Size + dim3Edge2 - 1, frame_dim3 - 1);

  /* Update local 3D tile structure with dstTile structure parameters. Local   */
  /* 3D tile structure is used as parameter to implement fillTile functionality */
  xi_tile3D dst_t;
  XI_TILE3D_SET_DIM1(&dst_t, dim1Size);
  XI_TILE3D_SET_DIM1_PITCH(&dst_t, dstDataPitch1);
  XI_TILE3D_SET_DIM1_EDGE1(&dst_t, dim1Edge1);
  XI_TILE3D_SET_DIM1_EDGE2(&dst_t, dim1Edge2);
  XI_TILE3D_SET_DIM2(&dst_t, dim2Size);
  XI_TILE3D_SET_DIM2_PITCH(&dst_t, dstDataPitch2);
  XI_TILE3D_SET_DIM2_EDGE1(&dst_t, dim2Edge1);
  XI_TILE3D_SET_DIM2_EDGE2(&dst_t, dim2Edge2);
  XI_TILE3D_SET_DIM3_EDGE1(&dst_t, 0);
  XI_TILE3D_SET_DIM3_EDGE2(&dst_t, 0);
  XI_TILE3D_SET_DIM1_COORD(&dst_t, start_x);
  XI_TILE3D_SET_DIM2_COORD(&dst_t, start_y);
  XI_TILE3D_SET_DIM3_COORD(&dst_t, start_z);
  XI_TILE3D_SET_BUFF_PTR(&dst_t, XI_TILE3D_GET_BUFF_PTR(dstTile));
  XI_TILE3D_SET_BUFF_SIZE(&dst_t, XI_TILE3D_GET_BUFF_SIZE(dstTile));
  XI_TILE3D_SET_TYPE(&dst_t, XI_TILE3D_GET_TYPE(dstTile));

  MORPH_IDT_SCALAR *pDst = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(dstTile);
  const MORPH_IDT_SCALAR *pValue = (MORPH_IDT_SCALAR *)XI_ARRAY_GET_DATA_PTR(pArray);
  int32_t z; /* Loop variable */
  MORPH_IDT_SCALAR *pDst1;
  MORPH_IDT_SCALAR value;

  /* Validation for Tile and Frame intersection */
  int32_t frameIntersectionFlag = ((ixmin > ixmax) || (iymin > iymax) || (izmin > izmax));

  /*********************************************************************************/
  /* P6 implementation of xiExtendEdges3D is similar to xiExtendEdgesConst3D       */
  /* implementation. In ExtendEdges functionality a unique value is used to        */
  /* xiExtendEdges, in xiExtendEdges3D implementation each 2D tile is filled       */
  /* with a value from xi_array, index by the co-ordinate position across third    */
  /* dimension. In xiExtendEdges3D implementation processing across 3rd            */
  /* dimension edges, extendEdges need to perform for the entire 2D tile.          */
  /* xiExtendEdges3D processing is split into 3 parts. ExtendEdges processing      */
  /* across 3rd dimension edges is split as front end and rear end processing.     */
  /* Processing across 3rd dimension excluding the edge is implemented similar to  */
  /* 2D implementation of extendEdges functionality.                               */
  /*********************************************************************************/

  if (frameIntersectionFlag)
  {
    /* If frameIntersectionFlag is enabled the tile exists outside frame boundary */
    /* and ExtendEdges need to be done on the entire 3D tile.                     */

    const int32_t dim3FillSize = dim3Size + dim3Edge1 + dim3Edge2;
    pDst1 = &pDst[((-dim3Edge1) * dstDataPitch2)];
    for (z = 0; z < dim3FillSize; z++) /* Loop across dim3 */
    {
      value = pValue[z];
      /* update destination data pointer */
      MORPH_IDT_SCALAR *pDst2 = pDst1 + (z * dstDataPitch2);
      XI_TILE3D_SET_DATA_PTR(&dst_t, pDst2);
      XI_TILE3D_SET_DIM3(&dst_t, 1);
      MORPH_IDT_FILLTILE(&dst_t, value, 1);
    }
    return;
  }

  /* Number of 2D tiles to be processed across edge1 3rd dimension */
  int32_t dim3SizeFrontEnd = izmin - (start_z - dim3Edge1);
  /* Offset calculation for Extend Edge across 3rd dimension excluding edges */
  int32_t dim3CordMiddle = izmin - start_z;
  /* Number of 2D tiles to be processed across 3rd dimension excluding edges */
  int32_t dim3SizeMiddle = izmax - izmin + 1;
  /* Offset calculation for Extend Edge across edge 2 3rd */
  int32_t dim3CordRearEnd = izmax + 1 - start_z;
  /* Number of 2D tiles processing to Extend Edge across 3rd edge2 dimension */
  int32_t dim3SizeRearEnd = start_z + dim3Size + dim3Edge2 - 1 - izmax;

  /***********************************************************************************/
  /* Processing across the 3rd dimension edges (edge1 and edge2)                     */
  /* Processing across 3rd dimension edge 1 is referred as Front End Processing      */
  /* Processing across 3rd dimension edge 2 is referred as Rear End Processing       */
  /* Local copy of 3D tile is declared and updated with destination tile parameters. */
  /* Size parameter across third dimension is updated based on number of 2D tiles    */
  /* to be processed across front and read end. In order to effectively use the      */
  /* SIMD capabilities xiFillTile3D implementation is utilized.                      */
  /***********************************************************************************/

  if (dim3SizeFrontEnd > 0)
  {
    /***********************************************************************************/
    /* Front end processing : Processing along the 3rd dimension edge 1.               */
    /***********************************************************************************/

    /* Update destination data pointer */
    pDst1 = &pDst[((-dim3Edge1) * dstDataPitch2)];
    XI_TILE3D_SET_DIM3(&dst_t, 1);
    for (z = 0; z < dim3SizeFrontEnd; z++) /* Loop across dim3 */
    {
      value = pValue[z];
      /* update destination data pointer */
      MORPH_IDT_SCALAR *pDst2 = pDst1 + (z * dstDataPitch2);
      XI_TILE3D_SET_DATA_PTR(&dst_t, pDst2);
      MORPH_IDT_FILLTILE(&dst_t, value, 1);
    }
  }
  if (dim3SizeRearEnd > 0)
  {
    /***********************************************************************************/
    /* Rear end processing : Processing along the 3rd dimension edge 2.                */
    /***********************************************************************************/

    /* Update destination data pointer */
    pDst1 = &pDst[(dim3CordRearEnd * dstDataPitch2)];
    XI_TILE3D_SET_DIM3(&dst_t, 1);
    for (z = 0; z < dim3SizeRearEnd; z++) /* Loop across dim3 */
    {
      /* update destination data pointer */
      MORPH_IDT_SCALAR *pDst2 = pDst1 + (z * dstDataPitch2);
      value = pValue[z + dim3CordRearEnd + dim3Edge1];
      XI_TILE3D_SET_DATA_PTR(&dst_t, pDst2);
      MORPH_IDT_FILLTILE(&dst_t, value, 1);
    }
  }

  /* Update destination data pointer */
  pDst1 = &pDst[(dim3CordMiddle * dstDataPitch2)];
  XI_TILE3D_SET_DIM3(&dst_t, dim3SizeMiddle);

  XI_TILE3D_SET_DATA_PTR(&dst_t, pDst1);
  MORPH_OP_FUNCTION(&dst_t, pValue + dim3CordMiddle + dim3Edge1, frame3DSize);
#endif
}

/***********************   extendEdges3D_I8(I16)_DWH  ****************************/
/* Description : C5 optimized generic implementation of xiExtendEdgesConst 3D    */
/*               function. Based on MORPH pre-processor specifiers, code         */
/*               implementation is generated during preprocessing stage. This    */
/*               method implements extendEdges3D_I8_DWH and extendEdges3D_I16_DWH*/
/*               functionality                                                   */
/* Inputs      : constant value to fill the edges                                */
/* Outputs     : XI Error Code                                                   */
/* InOuts      : Destination Tile                                                */
/* Assumptions : OutData is signed/unsigned 8 / 16 bit Interger                  */
/*               based on MORPH specifier                                        */
/*********************************************************************************/

/*************************** extendEdges3D_I8_DWH ********************************/
/*************************** extendEdges3D_I16_DWH *******************************/

void MAKE_NAME_1(extendEdges3D, DWH)(xi_pTile3D dstTile,
                                     const xi_pArray pArray,
                                     xi_size3D frame3DSize)
{
  /* Getting parameters from the tile structures */
  const int32_t dim1Size = XI_TILE3D_GET_DIM1(dstTile);
  const int32_t dim2Size = XI_TILE3D_GET_DIM2(dstTile);
  const int32_t dim3Size = XI_TILE3D_GET_DIM3(dstTile);
  const int32_t dim1Edge1 = XI_TILE3D_GET_DIM1_EDGE1(dstTile);
  const int32_t dim1Edge2 = XI_TILE3D_GET_DIM1_EDGE2(dstTile);
  const int32_t dim2Edge1 = XI_TILE3D_GET_DIM2_EDGE1(dstTile);
  const int32_t dim2Edge2 = XI_TILE3D_GET_DIM2_EDGE2(dstTile);
  const int32_t dim3Edge1 = XI_TILE3D_GET_DIM3_EDGE1(dstTile);
  const int32_t dim3Edge2 = XI_TILE3D_GET_DIM3_EDGE2(dstTile);
  const int32_t dstDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(dstTile);
  const int32_t dstDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(dstTile);
  const int32_t bytesPerPixel = XI_TILE3D_GET_ELEMENT_SIZE(dstTile);

  int32_t frame_dim1 = frame3DSize.dim1Size;
  int32_t frame_dim2 = frame3DSize.dim2Size;
  int32_t frame_dim3 = frame3DSize.dim3Size;
  int32_t start_x = XI_TILE3D_GET_DIM1_COORD(dstTile); // along Depth
  int32_t start_y = XI_TILE3D_GET_DIM2_COORD(dstTile); // along Width
  int32_t start_z = XI_TILE3D_GET_DIM3_COORD(dstTile); // along Height

  int32_t ixmin = max(start_x - dim1Edge1, 0);
  int32_t ixmax = min(start_x + dim1Size + dim1Edge2 - 1, frame_dim1 - 1);
  int32_t iymin = max(start_y - dim2Edge1, 0);
  int32_t iymax = min(start_y + dim2Size + dim2Edge2 - 1, frame_dim2 - 1);
  int32_t izmin = max(start_z - dim3Edge1, 0);
  int32_t izmax = min(start_z + dim3Size + dim3Edge2 - 1, frame_dim3 - 1);

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

  // front
  int32_t frontXcord = -dim1Edge1;
  int32_t frontYcord = horTopYcord + horTopHeight;
  int32_t frontZcord = -dim3Edge1;
  int32_t frontDepth = izmin - (start_z - dim3Edge1);
  int32_t frontWidth = horTopWidth;
  int32_t frontHeight = iymax - iymin + 1;

  // rear
  int32_t rearXcord = -dim1Edge1;
  int32_t rearYcord = horTopYcord + horTopHeight;
  int32_t rearZcord = izmax + 1 - start_z;
  int32_t rearDepth = start_z + dim3Size + dim3Edge2 - 1 - izmax;
  int32_t rearWidth = horTopWidth;
  int32_t rearHeight = iymax - iymin + 1;

  int x, y, z; /* Loop variables */
  valign vaOutData = IVP_ZALIGN();
  valign vaArray;
  int32_t vectorizationWidth = MORPH_VECTORIZATIONWIDTH;

  MORPH_IDT_SCALAR *restrict pDst3D = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(dstTile);
  MORPH_IDT_SCALAR *restrict pArr = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(pArray) + dim1Edge1;

  MORPH_IDT_VEC *restrict pdvecArr, *restrict pdvecDst;
  MORPH_IDT_VEC dvecArrData;

  /* Tile and frame intersection is empty,fill entire tile with edge values */
  if ((ixmin > ixmax) || (iymin > iymax) || (izmin > izmax))
  {
    pdvecArr = (MORPH_IDT_VEC *)(pArr - dim1Edge1);

    /* priming of pArray */
    vaArray = MORPH_OP_PRIME(pdvecArr);

    for (x = 0; x < (dim1Size + dim1Edge1 + dim1Edge2); x += vectorizationWidth)
    {
      /* Load pArray */
      MORPH_OP_LOAD(dvecArrData, vaArray, pdvecArr, (dim1Size + dim1Edge1 + dim1Edge2 - x) * bytesPerPixel);

      for (z = 0; z < (dim3Size + dim3Edge1 + dim3Edge2); z++)
      {
        for (y = 0; y < (dim2Size + dim2Edge1 + dim2Edge2); y++)
        {
          pdvecDst = (MORPH_IDT_VEC *)(pDst3D + (z - dim3Edge1) * dstDataPitch2 + \
            (y - dim2Edge1) * dstDataPitch1 + (-dim1Edge1) + x);

          /* store array value in destination */
          MORPH_OP_STORE(dvecArrData, vaOutData, pdvecDst, (dim1Size + dim1Edge1 + dim1Edge2 - x) * bytesPerPixel);

          MORPH_OP_FLUSH(vaOutData, pdvecDst);
        }
      }
    }
  }
  else
  {
    /* Front Height Edge */
    if (frontDepth > 0)
    {
      pdvecArr = (MORPH_IDT_VEC *)(pArr + frontXcord);

      /* priming of pArray */
      vaArray = MORPH_OP_PRIME(pdvecArr);

      for (x = 0; x < frontWidth; x += vectorizationWidth)
      {
        /* Load pArray */
        MORPH_OP_LOAD(dvecArrData, vaArray, pdvecArr, (frontWidth - x) * bytesPerPixel);

        for (z = 0; z < frontDepth; z++)
        {
          for (y = 0; y < frontHeight; y++)
          {
            pdvecDst = (MORPH_IDT_VEC *)(pDst3D + (frontZcord + z) * dstDataPitch2 + \
              (y + frontYcord) * dstDataPitch1 + frontXcord + x);

            /* store array value in destination */
            MORPH_OP_STORE(dvecArrData, vaOutData, pdvecDst, (frontWidth - x) * bytesPerPixel);

            MORPH_OP_FLUSH(vaOutData, pdvecDst);
          }
        }
      }
    }

    /* Rear Height Edge */
    if (rearDepth > 0)
    {
      pdvecArr = (MORPH_IDT_VEC *)(pArr + rearXcord);

      /* priming of pArray */
      vaArray = MORPH_OP_PRIME(pdvecArr);

      for (x = 0; x < rearWidth; x += vectorizationWidth)
      {
        /* Load pArray */
        MORPH_OP_LOAD(dvecArrData, vaArray, pdvecArr, (rearWidth - x) * bytesPerPixel);

        for (z = 0; z < rearDepth; z++)
        {
          for (y = 0; y < rearHeight; y++)
          {
            pdvecDst = (MORPH_IDT_VEC *)(pDst3D + (rearZcord + z) * dstDataPitch2 + \
              (y + rearYcord) * dstDataPitch1 + rearXcord + x);

            /* store array value in destination */
            MORPH_OP_STORE(dvecArrData, vaOutData, pdvecDst, (rearWidth - x) * bytesPerPixel);

            MORPH_OP_FLUSH(vaOutData, pdvecDst);
          }
        }
      }
    }

    /* Top Width Edge */
    if (horTopHeight > 0)
    {
      pdvecArr = (MORPH_IDT_VEC *)(pArr + horTopXcord);

      /* priming of pArray */
      vaArray = MORPH_OP_PRIME(pdvecArr);

      for (x = 0; x < horTopWidth; x += vectorizationWidth)
      {
        /* Load pArray */
        MORPH_OP_LOAD(dvecArrData, vaArray, pdvecArr, (horTopWidth - x) * bytesPerPixel);

        for (z = 0; z < (dim3Size + dim3Edge1 + dim3Edge2); z++)
        {
          for (y = 0; y < horTopHeight; y++)
          {
            pdvecDst = (MORPH_IDT_VEC *)(pDst3D + (z - dim3Edge1) * dstDataPitch2 + \
              (horTopYcord + y) * dstDataPitch1 + horTopXcord + x);

            /* store array value in destination */
            MORPH_OP_STORE(dvecArrData, vaOutData, pdvecDst, (horTopWidth - x) * bytesPerPixel);

            MORPH_OP_FLUSH(vaOutData, pdvecDst);
          }
        }
      }
    }

    /* Bottom Width Edge */
    if (horBottomHeight > 0)
    {
      pdvecArr = (MORPH_IDT_VEC *)(pArr + horBottomXcord);

      /* priming of pArray */
      vaArray = MORPH_OP_PRIME(pdvecArr);

      for (x = 0; x < horBottomWidth; x += vectorizationWidth)
      {
        /* Load pArray */
        MORPH_OP_LOAD(dvecArrData, vaArray, pdvecArr, (horBottomWidth - x) * bytesPerPixel);

        for (z = 0; z < (dim3Size + dim3Edge1 + dim3Edge2); z++)
        {
          for (y = 0; y < horBottomHeight; y++)
          {
            pdvecDst = (MORPH_IDT_VEC *)(pDst3D + (z - dim3Edge1) * dstDataPitch2 + \
              (horBottomYcord + y) * dstDataPitch1 + horBottomXcord + x);

            /* store array value in destination */
            MORPH_OP_STORE(dvecArrData, vaOutData, pdvecDst, (horBottomWidth - x) * bytesPerPixel);

            MORPH_OP_FLUSH(vaOutData, pdvecDst);
          }
        }
      }
    }

    /* Left Depth Edge */
    if (verLeftWidth > 0)
    {
      pdvecArr = (MORPH_IDT_VEC *)(pArr + verLeftXcord);

      /* priming of pArray */
      vaArray = MORPH_OP_PRIME(pdvecArr);

      for (x = 0; x < verLeftWidth; x += vectorizationWidth)
      {
        /* Load pArray */
        MORPH_OP_LOAD(dvecArrData, vaArray, pdvecArr, (verLeftWidth - x) * bytesPerPixel);

        for (z = 0; z < (dim3Size + dim3Edge1 + dim3Edge2); z++)
        {
          for (y = 0; y < verLeftHeight; y++)
          {
            pdvecDst = (MORPH_IDT_VEC *)(pDst3D + (z - dim3Edge1) * dstDataPitch2 + \
              (verLeftYcord + y) * dstDataPitch1 + verLeftXcord + x);

            /* store array value in destination */
            MORPH_OP_STORE(dvecArrData, vaOutData, pdvecDst, (verLeftWidth - x) * bytesPerPixel);

            MORPH_OP_FLUSH(vaOutData, pdvecDst);
          }
        }
      }
    }

    /* Right Depth Edge */
    if (verRightWidth > 0)
    {
      pdvecArr = (MORPH_IDT_VEC *)(pArr + verRightXcord);

      /* priming of pArray */
      vaArray = MORPH_OP_PRIME(pdvecArr);

      for (x = 0; x < verRightWidth; x += vectorizationWidth)
      {
        /* Load pArray */
        MORPH_OP_LOAD(dvecArrData, vaArray, pdvecArr, (verRightWidth - x) * bytesPerPixel);

        for (z = 0; z < (dim3Size + dim3Edge1 + dim3Edge2); z++)
        {
          for (y = 0; y < verRightHeight; y++)
          {
            pdvecDst = (MORPH_IDT_VEC *)(pDst3D + (z - dim3Edge1) * dstDataPitch2 + \
              (verRightYcord + y) * dstDataPitch1 + verRightXcord + x);

            /* store array value in destination */
            MORPH_OP_STORE(dvecArrData, vaOutData, pdvecDst, (verRightWidth - x) * bytesPerPixel);

            MORPH_OP_FLUSH(vaOutData, pdvecDst);
          }
        }
      }
    }
  }
}

/***********************   extendEdges3D_I8(I16)_ID16WH  *************************/
/* Description : P6 optimized generic implementation of xiExtendEdgesConst 3D    */
/*               function. Based on MORPH pre-processor specifiers, code         */
/*               implementation is generated during preprocessing stage. This    */
/*               method implements extendEdges3D_I8_ID16WH and                   */
/*               extendEdges3D_I16_ID16WH                                        */
/*               functionality                                                   */
/* Inputs      : constant value to fill the edges                                */
/* Outputs     : XI Error Code                                                   */
/* InOuts      : Destination Tile                                                */
/* Assumptions : OutData is signed/unsigned 8 / 16 bit Interger                  */
/*               based on MORPH specifier                                        */
/*********************************************************************************/
void MAKE_NAME_1(extendEdges3D, ID16WH)(xi_pTile3D dstTile,
                                        const xi_pArray pArray,
                                        xi_size3D frame3DSize)
{
  /* Getting parameters from the tile structures */
  const int32_t dim1Size = XI_TILE3D_GET_DIM1(dstTile);
  const int32_t dim2Size = XI_TILE3D_GET_DIM2(dstTile);
  const int32_t dim3Size = XI_TILE3D_GET_DIM3(dstTile);
  const int32_t dim1Edge1 = XI_TILE3D_GET_DIM1_EDGE1(dstTile);
  const int32_t dim1Edge2 = XI_TILE3D_GET_DIM1_EDGE2(dstTile);
  const int32_t dim2Edge1 = XI_TILE3D_GET_DIM2_EDGE1(dstTile);
  const int32_t dim2Edge2 = XI_TILE3D_GET_DIM2_EDGE2(dstTile);
  const int32_t dim3Edge1 = XI_TILE3D_GET_DIM3_EDGE1(dstTile);
  const int32_t dim3Edge2 = XI_TILE3D_GET_DIM3_EDGE2(dstTile);
  const int32_t dstDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(dstTile);
  const int32_t dstDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(dstTile);
  const int32_t bytesPerPixel = XI_TILE3D_GET_ELEMENT_SIZE(dstTile);

  int32_t frame_dim1 = frame3DSize.dim1Size;
  int32_t frame_dim2 = frame3DSize.dim2Size;
  int32_t frame_dim3 = frame3DSize.dim3Size;
  int32_t start_x = XI_TILE3D_GET_DIM1_COORD(dstTile); // along Depth
  int32_t start_y = XI_TILE3D_GET_DIM2_COORD(dstTile); // along Width
  int32_t start_z = XI_TILE3D_GET_DIM3_COORD(dstTile); // along Height

  int32_t ixmin = max(start_x - dim1Edge1, 0);
  int32_t ixmax = min(start_x + dim1Size + dim1Edge2 - 1, frame_dim1 - 1);
  int32_t iymin = max(start_y - dim2Edge1, 0);
  int32_t iymax = min(start_y + dim2Size + dim2Edge2 - 1, frame_dim2 - 1);
  int32_t izmin = max(start_z - dim3Edge1, 0);
  int32_t izmax = min(start_z + dim3Size + dim3Edge2 - 1, frame_dim3 - 1);

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

  // front
  int32_t frontXcord = -dim1Edge1;
  int32_t frontYcord = horTopYcord + horTopHeight;
  int32_t frontZcord = -dim3Edge1;
  int32_t frontDepth = izmin - (start_z - dim3Edge1);
  int32_t frontWidth = horTopWidth;
  int32_t frontHeight = iymax - iymin + 1;

  // rear
  int32_t rearXcord = -dim1Edge1;
  int32_t rearYcord = horTopYcord + horTopHeight;
  int32_t rearZcord = izmax + 1 - start_z;
  int32_t rearDepth = start_z + dim3Size + dim3Edge2 - 1 - izmax;
  int32_t rearWidth = horTopWidth;
  int32_t rearHeight = iymax - iymin + 1;

  //vecDataSeq: 0, 1, 2,...15, 0 , 1, 2,...15, 0 , 1, 2, ...15, 0, 1, 2, ....15
  MORPH_IDT_VEC dvecSeq = MORPH_OP_AND(MORPH_OP_SEQ(), (MORPH_IDT_VEC)(15));

  int x, y, z; /* Loop variables */
  valign vaOutData = IVP_ZALIGN();
  valign vaArray;
  int32_t vectorizationWidth =  XCHAL_IVPN_SIMD_WIDTH;

  MORPH_IDT_SCALAR *restrict pDst3D = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(dstTile);
  MORPH_IDT_SCALAR *restrict pArr = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(pArray) + (dim2Edge1 << 4);

  MORPH_IDT_VEC *restrict pdvecArr, *restrict pdvecDst;
  MORPH_IDT_VEC dvecArrData;

  /* Tile and frame intersection is empty,fill entire tile with edge values */
  if ((ixmin > ixmax) || (iymin > iymax) || (izmin > izmax))
  {
    for (z = 0; z < (dim3Size + dim3Edge1 + dim3Edge2); z++)
    {
      pdvecArr = (MORPH_IDT_VEC *)(pArr - dim2Edge1 * 16);

      /* priming of pArray */
      vaArray = MORPH_OP_PRIME(pdvecArr);

      for (y = 0; y < (dim2Size + dim2Edge1 + dim2Edge2); y++)
      {
        pdvecDst = (MORPH_IDT_VEC *)(pDst3D + (z - dim3Edge1) * dstDataPitch2 + \
          (y - dim2Edge1) * dstDataPitch1 + (-dim1Edge1));

        /* Load pArray */
        MORPH_OP_LOAD(dvecArrData, vaArray, pdvecArr, (vectorizationWidth >> 1) * bytesPerPixel);
        MORPH_IDT_VEC dvecRepliArrData = MORPH_OP_SEL(dvecArrData, dvecArrData, dvecSeq);

        int xLimit = dim1Size + dim1Edge1 + dim1Edge2;
        for (x = 0; x < xLimit ; x += vectorizationWidth)
        {
          int32_t varLength = XT_MIN(xLimit - x, vectorizationWidth);

          /* store array value in destination */
          MORPH_OP_STORE(dvecRepliArrData, vaOutData, pdvecDst, varLength * bytesPerPixel);
        }
        MORPH_OP_FLUSH(vaOutData, pdvecDst);
      }
    }
  }
  else
  {
    /* Front Height Edge */
    for (z = 0; z < frontDepth; z++)
    {
      pdvecArr = (MORPH_IDT_VEC *)(pArr + frontYcord * 16);

      /* priming of pArray */
      vaArray = MORPH_OP_PRIME(pdvecArr);

      for (y = 0; y < frontHeight; y++)
      {
        pdvecDst = (MORPH_IDT_VEC *)(pDst3D + (frontZcord + z) * dstDataPitch2 + \
          (y + frontYcord) * dstDataPitch1 + frontXcord);

        /* Load pArray */
        MORPH_OP_LOAD(dvecArrData, vaArray, pdvecArr, (vectorizationWidth >> 1) * bytesPerPixel);
        MORPH_IDT_VEC dvecRepliArrData = MORPH_OP_SEL(dvecArrData, dvecArrData, dvecSeq);

        for (x = 0; x < frontWidth; x += vectorizationWidth)
        {
          int32_t varLength = XT_MIN(frontWidth - x, vectorizationWidth);
          /* store array value in destination */
          MORPH_OP_STORE(dvecRepliArrData, vaOutData, pdvecDst, varLength * bytesPerPixel);
        }
        MORPH_OP_FLUSH(vaOutData, pdvecDst);
      }
    }

    /* Rear Height Edge */
    for (z = 0; z < rearDepth; z++)
    {
      pdvecArr = (MORPH_IDT_VEC *)(pArr + rearYcord * 16);

      /* priming of pArray */
      vaArray = MORPH_OP_PRIME(pdvecArr);

      for (y = 0; y < rearHeight; y++)
      {
        pdvecDst = (MORPH_IDT_VEC *)(pDst3D + (rearZcord + z) * dstDataPitch2 + \
          (y + rearYcord) * dstDataPitch1 + rearXcord);

        /* Load pArray */
        MORPH_OP_LOAD(dvecArrData, vaArray, pdvecArr, (vectorizationWidth >> 1) * bytesPerPixel);
        MORPH_IDT_VEC dvecRepliArrData = MORPH_OP_SEL(dvecArrData, dvecArrData, dvecSeq);

        for (x = 0; x < rearWidth; x += vectorizationWidth)
        {
          int32_t varLength = XT_MIN(rearWidth - x, vectorizationWidth);
          /* store array value in destination */
          MORPH_OP_STORE(dvecRepliArrData, vaOutData, pdvecDst, varLength * bytesPerPixel);
        }
        MORPH_OP_FLUSH(vaOutData, pdvecDst);
      }
    }

    /* Top Width Edge */
    if (horTopHeight > 0)
    {
      for (z = 0; z < (dim3Size + dim3Edge1 + dim3Edge2); z++)
      {
        pdvecArr = (MORPH_IDT_VEC *)(pArr + horTopYcord * 16);

        /* priming of pArray */
        vaArray = MORPH_OP_PRIME(pdvecArr);

        for (y = 0; y < horTopHeight; y++)
        {
          /* Load pArray */
          MORPH_OP_LOAD(dvecArrData, vaArray, pdvecArr, (vectorizationWidth >> 1) * bytesPerPixel);
          MORPH_IDT_VEC dvecRepliArrData = MORPH_OP_SEL(dvecArrData, dvecArrData, dvecSeq);

          pdvecDst = (MORPH_IDT_VEC *)(pDst3D + (z - dim3Edge1) * dstDataPitch2 + \
            (horTopYcord + y) * dstDataPitch1 + horTopXcord);

          for (x = 0; x < horTopWidth; x += vectorizationWidth)
          {
            int32_t varLength = XT_MIN(horTopWidth - x, vectorizationWidth);
            /* store array value in destination */
            MORPH_OP_STORE(dvecRepliArrData, vaOutData, pdvecDst, varLength * bytesPerPixel);
          }
          MORPH_OP_FLUSH(vaOutData, pdvecDst);
        }
      }
    }

    /* Bottom Width Edge */
    if (horBottomHeight > 0)
    {
      for (z = 0; z < (dim3Size + dim3Edge1 + dim3Edge2); z++)
      {
        pdvecArr = (MORPH_IDT_VEC *)(pArr + horBottomYcord * 16);

        /* priming of pArray */
        vaArray = MORPH_OP_PRIME(pdvecArr);

        for (y = 0; y < horBottomHeight; y++)
        {
          pdvecDst = (MORPH_IDT_VEC *)(pDst3D + (z - dim3Edge1) * dstDataPitch2 + \
            (horBottomYcord + y) * dstDataPitch1 + horBottomXcord);

          /* Load pArray */
          MORPH_OP_LOAD(dvecArrData, vaArray, pdvecArr, (vectorizationWidth >> 1) * bytesPerPixel);
          MORPH_IDT_VEC dvecRepliArrData = MORPH_OP_SEL(dvecArrData, dvecArrData, dvecSeq);

          for (x = 0; x < horBottomWidth; x += vectorizationWidth)
          {
            int32_t varLength = XT_MIN(horBottomWidth - x, vectorizationWidth);
            /* store array value in destination */
            MORPH_OP_STORE(dvecRepliArrData, vaOutData, pdvecDst, varLength * bytesPerPixel);
          }
          MORPH_OP_FLUSH(vaOutData, pdvecDst);
        }
      }
    }

    /* Left Depth Edge */
    if (verLeftWidth > 0)
    {
      for (z = 0; z < (dim3Size + dim3Edge1 + dim3Edge2); z++)
      {
        pdvecArr = (MORPH_IDT_VEC *)(pArr + verLeftYcord * 16);

        /* priming of pArray */
        vaArray = MORPH_OP_PRIME(pdvecArr);
        for (y = 0; y < verLeftHeight; y++)
        {
          pdvecDst = (MORPH_IDT_VEC *)(pDst3D + (z - dim3Edge1) * dstDataPitch2 + \
            (verLeftYcord + y) * dstDataPitch1 + verLeftXcord);

          /* Load pArray */
          MORPH_OP_LOAD(dvecArrData, vaArray, pdvecArr, (vectorizationWidth >> 1) * bytesPerPixel);
          MORPH_IDT_VEC dvecRepliArrData = MORPH_OP_SEL(dvecArrData, dvecArrData, dvecSeq);

          for (x = 0; x < verLeftWidth; x += vectorizationWidth)
          {
            int32_t varLength = XT_MIN(verLeftWidth - x, vectorizationWidth);
            /* store array value in destination */
            MORPH_OP_STORE(dvecRepliArrData, vaOutData, pdvecDst, varLength * bytesPerPixel);
          }
          MORPH_OP_FLUSH(vaOutData, pdvecDst);
        }
      }
    }

    /* Right Depth Edge */
    if (verRightWidth > 0)
    {
      for (z = 0; z < (dim3Size + dim3Edge1 + dim3Edge2); z++)
      {
        pdvecArr = (MORPH_IDT_VEC *)(pArr + verRightYcord * 16);

        /* priming of pArray */
        vaArray = MORPH_OP_PRIME(pdvecArr);

        for (y = 0; y < verRightHeight; y++)
        {
          pdvecDst = (MORPH_IDT_VEC *)(pDst3D + (z - dim3Edge1) * dstDataPitch2 + \
            (verRightYcord + y) * dstDataPitch1 + verRightXcord);

          /* Load pArray */
          MORPH_OP_LOAD(dvecArrData, vaArray, pdvecArr, (vectorizationWidth >> 1) * bytesPerPixel);
          MORPH_IDT_VEC dvecRepliArrData = MORPH_OP_SEL(dvecArrData, dvecArrData, dvecSeq);

          for (x = 0; x < verRightWidth; x += vectorizationWidth)
          {
            int32_t varLength = XT_MIN(verRightWidth - x, vectorizationWidth);
            /* store array value in destination */
            MORPH_OP_STORE(dvecRepliArrData, vaOutData, pdvecDst, varLength * bytesPerPixel);
          }
          MORPH_OP_FLUSH(vaOutData, pdvecDst);
        }
      }
    }
  }
}

#endif //if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))
