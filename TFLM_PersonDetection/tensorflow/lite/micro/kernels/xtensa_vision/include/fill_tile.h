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

#define INTEGER8BIT  1
#define INTEGER16BIT 2

#undef MAKE_NAME_IMPL
#undef MAKE_NAME
#undef MORPH_OP_PRIME
#undef MORPH_IDT_CHECK
#undef MORPH_IDT_SCALAR
#undef MORPH_OP_FLUSH

#define MAKE_NAME_IMPL(name, MORPH_FNAME_SPECIFIER_IDT) name##_##MORPH_FNAME_SPECIFIER_IDT

#if INPUT_DATA_TYPE == INTEGER8BIT

#define MAKE_NAME(name) MAKE_NAME_IMPL(name, I8)
#define MORPH_IDT_CHECK XI_CHECK_TILE3D_I8
#define MORPH_IDT_SCALAR int8_t
#define MORPH_IDT_VECTOR xb_vec2Nx8
#define MORPH_VECTORIZATION_WIDTH (2 * XCHAL_IVPN_SIMD_WIDTH)
#define MORPH_OP_STORE_IP IVP_SA2NX8_IP
#define MORPH_OP_STORE_IP IVP_SA2NX8_IP
#define MORPH_OP_VAR_STORE_XP IVP_SAV2NX8_XP
#define MORPH_OP_PRIME IVP_LA2NX8_PP
#define MORPH_OP_FLUSH IVP_SAPOS2NX8_FP
#define MORPH_BYTES_PER_PIXEL 1

#elif INPUT_DATA_TYPE == INTEGER16BIT

#undef MAKE_NAME
#undef MORPH_IDT_CHECK
#undef MORPH_IDT_SCALAR
#undef MORPH_IDT_VECTOR
#undef MORPH_VECTORIZATION_WIDTH
#undef MORPH_OP_STORE_IP
#undef MORPH_OP_VAR_STORE_XP
#undef MORPH_OP_PRIME
#undef MORPH_OP_FLUSH
#undef MORPH_BYTES_PER_PIXEL

#define MAKE_NAME(name) MAKE_NAME_IMPL(name, I16)
#define MORPH_IDT_CHECK XI_CHECK_TILE3D_I16
#define MORPH_IDT_SCALAR int16_t
#define MORPH_IDT_VECTOR xb_vecNx16
#define MORPH_VECTORIZATION_WIDTH (XCHAL_IVPN_SIMD_WIDTH)
#define MORPH_OP_STORE_IP IVP_SANX16_IP
#define MORPH_OP_VAR_STORE_XP IVP_SAVNX16_XP
#define MORPH_OP_PRIME IVP_LANX16_PP
#define MORPH_OP_FLUSH IVP_SAPOSNX16_FP
#define MORPH_BYTES_PER_PIXEL 2

#endif

/******************************************************************************************
*                                 MAKE_NAME(xiFillTile3D)
******************************************************************************************/

/*******************************   xiFillTile3D  ********************************/
/* Description : P6 optimized generic implementation of FillTile 3D function.   */
/*               Based on MORPH pre-processor specifiers, code implementation   */
/*               is generated during preprocessing stage. This method           */
/*               implements xiFillTile3D_I8 and xiFillTile3D_I16 functionality  */
/* Inputs      : Constant value to fill, fill_edge_extension                    */
/* Outputs     : XI Error Code                                                  */
/* InOuts      : Output Tile                                                    */
/* Assumptions : OutData is signed 8 / 16 bit Interger based on MORPH specifier */
/********************************************************************************/

XI_ERR_TYPE MAKE_NAME(xiFillTile3D)(xi_pTile3D dstTile,
  const int32_t value,
  xi_bool fill_edge_extension)
{
  /* Error Checks */
  XI_ERROR_CHECKS()
  {
    XI_CHECK_TILE3D(dstTile);
    XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(dstTile);
    MORPH_IDT_CHECK(dstTile);
  }

  /* Getting parameters from the tile structures */
  const int32_t dim1Size = XI_TILE3D_GET_DIM1(dstTile);
  const int32_t dim2Size = XI_TILE3D_GET_DIM2(dstTile);
  const int32_t dim1Edge1 = XI_TILE3D_GET_DIM1_EDGE1(dstTile);
  const int32_t dim1Edge2 = XI_TILE3D_GET_DIM1_EDGE2(dstTile);
  const int32_t dim2Edge1 = XI_TILE3D_GET_DIM2_EDGE1(dstTile);
  const int32_t dim2Edge2 = XI_TILE3D_GET_DIM2_EDGE2(dstTile);
  const int32_t dim3Edge1 = XI_TILE3D_GET_DIM3_EDGE1(dstTile);
  const int32_t dim3Edge2 = XI_TILE3D_GET_DIM3_EDGE2(dstTile);
  const int32_t dstDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(dstTile);
  const int32_t dstDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(dstTile);
  const int32_t dim3Size = XI_TILE3D_GET_DIM3(dstTile);
  MORPH_IDT_SCALAR *pDst = (MORPH_IDT_SCALAR *)XI_TILE3D_GET_DATA_PTR(dstTile);

  int32_t z, x, y;
  /* Vectorization for xiFillTile3D function is always done across the first dimension */
  int32_t vectorizationWidth = MORPH_VECTORIZATION_WIDTH;
  int32_t dim1FillSize = dim1Size;
  int32_t dim2FillSize = dim2Size;
  int32_t dim3FillSize = dim3Size;
  int32_t maxLoopCount;

  MORPH_IDT_VECTOR* __restrict pdvecOut;
  valign vaOutData = IVP_ZALIGN();
  MORPH_IDT_VECTOR vecValue = value;

  /* If fill_edge_extension flag is enabled update destination data pointer  */
  /* and data fill size across all 3 dimensions.                             */

  if (fill_edge_extension)
  {
    dim1FillSize = dim1Size + dim1Edge1 + dim1Edge2;
    dim2FillSize = dim2Size + dim2Edge1 + dim2Edge2;
    dim3FillSize = dim3Size + dim3Edge1 + dim3Edge2;
    pDst = &pDst[-dim1Edge1 + ((-dim2Edge1) * dstDataPitch1) + ((-dim3Edge1) * dstDataPitch2)];
  }

  /******************************************************************************/
  /* The overall design approach is split into 2 parts                          */
  /* 1. When destination tile pitch is equal to destination tile fill size.     */
  /*    - If above condition holds good, memory location to be filled           */
  /*      with constant value is contiguous. Hence vectorization can be         */
  /*      utilized effectively                                                  */
  /* 2. When destination tile pitch is greater than destination tile fill size. */
  /*    - If above condition holds good, memory location to be filled           */
  /*      with constant value is not contiguous. In order to do                 */
  /*      vectorization across first dimension, destination data pointers       */
  /*      need to be updated based on destination tile fill size and            */
  /*      destination tile pitch                                                */
  /******************************************************************************/
  if (dstDataPitch1 == dim1FillSize)
  {
    /* Data to be filled exist in contiguous memory location with respect to */
    /* first dimension                                                       */

    /* Initialize max loop counter */
    int32_t dim3MaxLoopCount = dim3FillSize;
    maxLoopCount = dim1FillSize * dim2FillSize;
    if (dstDataPitch2 == maxLoopCount)
    {
      /* Data to be filled exist in contiguous memory location with respect to */
      /* first and second dimension                                            */

      /* Update max loop counter */
      maxLoopCount *= dim3FillSize;
      dim3MaxLoopCount = 1;
    }
    for (z = 0; z < dim3MaxLoopCount; z++)
    {
      /* initialize destination data pointer */
      pdvecOut = (MORPH_IDT_VECTOR *)(pDst + (z * dstDataPitch2));
      for (x = 0; x < maxLoopCount - vectorizationWidth; x += vectorizationWidth)
      {
        MORPH_OP_STORE_IP(vecValue, vaOutData, pdvecOut);
      }

      MORPH_OP_VAR_STORE_XP(vecValue, vaOutData, pdvecOut,
                            (maxLoopCount - x)* MORPH_BYTES_PER_PIXEL);
      MORPH_OP_FLUSH(vaOutData, pdvecOut);
    }
  }
  else
  {
    /* else block execute if destination tile pitch is */
    /* greater than destination tile fill size         */
    for (z = 0; z < dim3FillSize; z++) /* Loop across dim3 */
    {
      x = 0;
      /* Loop across dimension 1 */
      /* Condition check added to maximize vectorization across dimension 1*/
      /* Loop across dim1 */
      for (; x < (dim1FillSize - 3 * vectorizationWidth); x += 4 * vectorizationWidth)
      {
        /* initialize destination data pointer */
        MORPH_IDT_SCALAR *pDst1 = pDst + x + (z * dstDataPitch2);
        for (y = 0; y < dim2FillSize; y++) /* Loop across dim2 */
        {
          pdvecOut = (MORPH_IDT_VECTOR *)(pDst1 + (y * dstDataPitch1));
          MORPH_OP_STORE_IP(vecValue, vaOutData, pdvecOut);
          MORPH_OP_STORE_IP(vecValue, vaOutData, pdvecOut);
          MORPH_OP_STORE_IP(vecValue, vaOutData, pdvecOut);
          MORPH_OP_VAR_STORE_XP(vecValue, vaOutData, pdvecOut,
                            (dim1FillSize - (x + 3 * vectorizationWidth))* MORPH_BYTES_PER_PIXEL);
          MORPH_OP_FLUSH(vaOutData, pdvecOut);
        }
      }
      if (x < (dim1FillSize - 2 * vectorizationWidth))
      {
        /* initialize destination data pointer */
        MORPH_IDT_SCALAR *pDst1 = pDst + x + (z * dstDataPitch2);
        for (y = 0; y < dim2FillSize; y++) /* Loop across dim2 */
        {
          pdvecOut = (MORPH_IDT_VECTOR *)(pDst1 + (y * dstDataPitch1));
          MORPH_OP_STORE_IP(vecValue, vaOutData, pdvecOut);
          MORPH_OP_STORE_IP(vecValue, vaOutData, pdvecOut);
          MORPH_OP_VAR_STORE_XP(vecValue, vaOutData, pdvecOut,
                            (dim1FillSize - (x + 2 * vectorizationWidth))* MORPH_BYTES_PER_PIXEL);
          MORPH_OP_FLUSH(vaOutData, pdvecOut);
        }
      }
      else if (x < (dim1FillSize - vectorizationWidth))
      {
        /* initialize destination data pointer */
        MORPH_IDT_SCALAR *pDst1 = pDst + x + (z * dstDataPitch2);
        for (y = 0; y < dim2FillSize; y++) /* Loop across dim2 */
        {
          pdvecOut = (MORPH_IDT_VECTOR *)(pDst1 + (y * dstDataPitch1));
          MORPH_OP_STORE_IP(vecValue, vaOutData, pdvecOut);
          MORPH_OP_VAR_STORE_XP(vecValue, vaOutData, pdvecOut,
                                (dim1FillSize - (x + vectorizationWidth))* MORPH_BYTES_PER_PIXEL);
          MORPH_OP_FLUSH(vaOutData, pdvecOut);
        }
      }
      else if (x < dim1FillSize)
      {
        /* initialize destination data pointer */
        MORPH_IDT_SCALAR *pDst1 = pDst + x + (z * dstDataPitch2);
        for (y = 0; y < dim2FillSize; y++) /* Loop across dim2 */
        {
          pdvecOut = (MORPH_IDT_VECTOR *)(pDst1 + (y * dstDataPitch1));
          MORPH_OP_VAR_STORE_XP(vecValue, vaOutData, pdvecOut,
                                (dim1FillSize - x)* MORPH_BYTES_PER_PIXEL);
          MORPH_OP_FLUSH(vaOutData, pdvecOut);
        }
      }
    }
  }
  return(XI_ERROR_STATUS());
}

#endif //if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))
