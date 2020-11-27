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
#include <stdint.h>
#include <stddef.h>
//#include "xi_config_api.h"
#include "xi_tile_manager.h"

#if 1 //((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))

#ifndef TILE3D_h
#define TILE3D_h

#if (XCHAL_HAVE_XNNE==1)
typedef enum { XI_WHD, XI_DWH, XI_ID16WH, XI_WHDN, XI_NWHD, XI_NDWH, XI_DWHN, XI_RMOD, XI_MTILE, XI_CMTILE, XI_UNKNOWN } xi_cnn_data_order;
#else
typedef enum { XI_WHD, XI_DWH, XI_ID16WH, XI_WHDN, XI_NWHD, XI_NDWH, XI_DWHN, XI_RMOD, XI_UNKNOWN } xi_cnn_data_order;
#endif

/******************************************************************************************************************
 *
 *                    3D definitions - extension of 2D definitions
 *
 * ****************************************************************************************************************/

typedef struct xi_frame3DStruct
{
  void     *pFrameBuff;
  uint32_t frameBuffSize;
  void     *pFrameData;
  int32_t  dim1Size;
  int32_t  dim2Size;
  int32_t  dim1Pitch; // pitch in width dimension
  uint8_t  pixelRes;  // in bits
  uint8_t  pixelPackFormat;  // not used in XI library
  uint16_t  dim1Edge1;
  uint16_t  dim1Edge2;
  uint16_t  dim2Edge1;
  uint16_t  dim2Edge2;
  uint16_t  dim3Edge1;
  uint16_t  dim3Edge2;
  uint8_t  paddingType;
  // new fields
  int32_t  dim2Pitch;
  int32_t  dim3Size;
  xi_cnn_data_order dataOrder; // WHD, DWH, etc.
} xi_frame3D, *xi_pFrame3D;

// new access macros
#define XI_FRAME3D_GET_DIM1(x)                ((x)->dim1Size)
#define XI_FRAME3D_SET_DIM1(x,v)              ((x)->dim1Size = (v))
#define XI_FRAME3D_GET_DIM1_PITCH(x)          ((x)->dim1Pitch)
#define XI_FRAME3D_SET_DIM1_PITCH(x,v)        ((x)->dim1Pitch = (v))
#define XI_FRAME3D_GET_DIM1_PITCH_IN_BYTES(x) ((x)->dim1Pitch*((x)->pixelRes/8 + ((x)->pixelRes&7 != 0)))
#define XI_FRAME3D_GET_DIM2(x)                ((x)->dim2Size)
#define XI_FRAME3D_SET_DIM2(x,v)              ((x)->dim2Size = (v))
#define XI_FRAME3D_GET_DIM2_PITCH(x)          ((x)->dim2Pitch)
#define XI_FRAME3D_SET_DIM2_PITCH(x,v)        ((x)->dim2Pitch = (v))
#define XI_FRAME3D_GET_DIM2_PITCH_IN_BYTES(x) ((x)->dim2Pitch*((x)->pixelRes/8 + ((x)->pixelRes&7 != 0)))
#define XI_FRAME3D_GET_DIM3(x)                ((x)->dim3Size)
#define XI_FRAME3D_SET_DIM3(x,v)              ((x)->dim3Size = (v))
#define XI_FRAME3D_GET_DIM1_EDGE1(x)          ((x)->dim1Edge1)
#define XI_FRAME3D_SET_DIM1_EDGE1(x,v)        ((x)->dim1Edge1 = (v))
#define XI_FRAME3D_GET_DIM1_EDGE2(x)          ((x)->dim1Edge2)
#define XI_FRAME3D_SET_DIM1_EDGE2(x,v)        ((x)->dim1Edge2 = (v))
#define XI_FRAME3D_GET_DIM2_EDGE1(x)          ((x)->dim2Edge1)
#define XI_FRAME3D_SET_DIM2_EDGE1(x,v)        ((x)->dim2Edge1 = (v))
#define XI_FRAME3D_GET_DIM2_EDGE2(x)          ((x)->dim2Edge2)
#define XI_FRAME3D_SET_DIM2_EDGE2(x,v)        ((x)->dim2Edge2 = (v))
#define XI_FRAME3D_GET_DIM3_EDGE1(x)          ((x)->dim3Edge1)
#define XI_FRAME3D_SET_DIM3_EDGE1(x,v)        ((x)->dim3Edge1 = (v))
#define XI_FRAME3D_GET_DIM3_EDGE2(x)          ((x)->dim3Edge2)
#define XI_FRAME3D_SET_DIM3_EDGE2(x,v)        ((x)->dim3Edge2 = (v))
#define XI_FRAME3D_GET_DATA_ORDER(x)          ((x)->dataOrder)
#define XI_FRAME3D_SET_DATA_ORDER(x,v)        ((x)->dataOrder = (v))

typedef struct {
    int32_t dim1Size;
    int32_t dim2Size;
    int32_t dim3Size;
} xi_size3D;

typedef struct {
    int32_t dim1Size;
    int32_t dim2Size;
    int32_t dim3Size;
    int32_t dim4Size;
} xi_size4D;

typedef struct {
  uint16_t dim1Edge1;
  uint16_t dim1Edge2;
  uint16_t dim2Edge1;
  uint16_t dim2Edge2;
  uint16_t dim3Edge1;
  uint16_t dim3Edge2;
} xi_edge3D;

typedef struct
{
  int32_t dataType;
} xi_dataType;

// 3D tile

typedef struct xi_tile3DStruct
{
  void       *pBuffer;
  uint32_t    bufferSize;
  void       *pData;
  int32_t     dim1Size;
  int32_t     dim1Pitch;
  uint32_t    status;    // Currently not used, planned to be obsolete
  uint16_t    type;
  int32_t     dim2Size;
  xi_frame3D *pFrame;    // changed to 3D frame
  int32_t     dim1Loc;   // dim1-loc of top-left active pixel in src frame
  int32_t     dim2Loc;   // dim2-loc of top-left active pixel in src frame
  uint16_t    dim1Edge1;
  uint16_t    dim2Edge1;
  uint16_t    dim1Edge2;
  uint16_t    dim2Edge2;
  // new fields
  int32_t       dim2Pitch;
  int32_t       dim3Size;
  xi_cnn_data_order dataOrder;
  int32_t       dim3Loc; // dim3-loc of top-left active pixel in src frame
  uint16_t      dim3Edge1;
  uint16_t      dim3Edge2;
#if (XCHAL_HAVE_XNNE==1)
  // Number of PTILES in MEMTILE along a particular dimension. Used for MEMTILES only
  int16_t    numPtilesDim1;
  int16_t    numPtilesDim2;
  int16_t    numPtilesDim3;
#endif
} xi_tile3D, *xi_pTile3D;


#define XI_TILE3D_GET_DIM1(x)                ((x)->dim1Size)
#define XI_TILE3D_SET_DIM1(x,v)              ((x)->dim1Size = (v))
#define XI_TILE3D_GET_DIM1_PITCH(x)          ((x)->dim1Pitch)
#define XI_TILE3D_SET_DIM1_PITCH(x,v)        ((x)->dim1Pitch = (v))
#define XI_TILE3D_GET_DIM2(x)                ((x)->dim2Size)
#define XI_TILE3D_SET_DIM2(x,v)              ((x)->dim2Size = (v))
#define XI_TILE3D_GET_DIM2_PITCH(x)          ((x)->dim2Pitch)
#define XI_TILE3D_SET_DIM2_PITCH(x,v)        ((x)->dim2Pitch = (v))
#define XI_TILE3D_GET_DIM3(x)                ((x)->dim3Size)
#define XI_TILE3D_SET_DIM3(x,v)              ((x)->dim3Size = (v))
#define XI_TILE3D_GET_DATA_ORDER(x)          ((x)->dataOrder)
#define XI_TILE3D_SET_DATA_ORDER(x,v)        ((x)->dataOrder = (v))
#define XI_TILE3D_GET_DIM1_COORD(x)          ((x)->dim1Loc)
#define XI_TILE3D_SET_DIM1_COORD(x,v)        ((x)->dim1Loc = (v))
#define XI_TILE3D_GET_DIM2_COORD(x)          ((x)->dim2Loc)
#define XI_TILE3D_SET_DIM2_COORD(x,v)        ((x)->dim2Loc = (v))
#define XI_TILE3D_GET_DIM3_COORD(x)          ((x)->dim3Loc)
#define XI_TILE3D_SET_DIM3_COORD(x,v)        ((x)->dim3Loc = (v))
#define XI_TILE3D_GET_DIM1_EDGE1(x)          ((x)->dim1Edge1)
#define XI_TILE3D_SET_DIM1_EDGE1(x,v)        ((x)->dim1Edge1 = (v))
#define XI_TILE3D_GET_DIM1_EDGE2(x)          ((x)->dim1Edge2)
#define XI_TILE3D_SET_DIM1_EDGE2(x,v)        ((x)->dim1Edge2 = (v))
#define XI_TILE3D_GET_DIM2_EDGE1(x)          ((x)->dim2Edge1)
#define XI_TILE3D_SET_DIM2_EDGE1(x,v)        ((x)->dim2Edge1 = (v))
#define XI_TILE3D_GET_DIM2_EDGE2(x)          ((x)->dim2Edge2)
#define XI_TILE3D_SET_DIM2_EDGE2(x,v)        ((x)->dim2Edge2 = (v))
#define XI_TILE3D_GET_DIM3_EDGE1(x)          ((x)->dim3Edge1)
#define XI_TILE3D_SET_DIM3_EDGE1(x,v)        ((x)->dim3Edge1 = (v))
#define XI_TILE3D_GET_DIM3_EDGE2(x)          ((x)->dim3Edge2)
#define XI_TILE3D_SET_DIM3_EDGE2(x,v)        ((x)->dim3Edge2 = (v))


/*****************************************
*   Data type definitions
*****************************************/

// added new bit to indicate 3D tile
#define XI_TYPE_TILE3D_BIT        (1 << 13)

#define XI_TYPE_IS_TILE3D(type)   ((type) & (XI_TYPE_TILE3D_BIT))

#define XI_TILE3D_U8    (XI_U8    | XI_TYPE_TILE3D_BIT | XI_TYPE_TILE_BIT)
#define XI_TILE3D_U16   (XI_U16   | XI_TYPE_TILE3D_BIT | XI_TYPE_TILE_BIT)
#define XI_TILE3D_U32   (XI_U32   | XI_TYPE_TILE3D_BIT | XI_TYPE_TILE_BIT)
#define XI_TILE3D_S8    (XI_S8    | XI_TYPE_TILE3D_BIT | XI_TYPE_TILE_BIT)
#define XI_TILE3D_S16   (XI_S16   | XI_TYPE_TILE3D_BIT | XI_TYPE_TILE_BIT)
#define XI_TILE3D_S32   (XI_S32   | XI_TYPE_TILE3D_BIT | XI_TYPE_TILE_BIT)
#define XI_TILE3D_F16   (XI_F16   | XI_TYPE_TILE3D_BIT | XI_TYPE_TILE_BIT)
#define XI_TILE3D_F32   (XI_F32   | XI_TYPE_TILE3D_BIT | XI_TYPE_TILE_BIT)
/*****************************************
*                   3D Frame Access Macros
*****************************************/

#define XI_FRAME3D_GET_BUFF_PTR     XI_FRAME_GET_BUFF_PTR
#define XI_FRAME3D_SET_BUFF_PTR     XI_FRAME_SET_BUFF_PTR

#define XI_FRAME3D_GET_BUFF_SIZE    XI_FRAME_GET_BUFF_SIZE
#define XI_FRAME3D_SET_BUFF_SIZE    XI_FRAME_SET_BUFF_SIZE

#define XI_FRAME3D_GET_DATA_PTR     XI_FRAME_GET_DATA_PTR
#define XI_FRAME3D_SET_DATA_PTR     XI_FRAME_SET_DATA_PTR

#define XI_FRAME3D_GET_PIXEL_RES    XI_FRAME_GET_PIXEL_RES
#define XI_FRAME3D_SET_PIXEL_RES    XI_FRAME_SET_PIXEL_RES

#define XI_FRAME3D_GET_PIXEL_FORMAT XI_FRAME_GET_PIXEL_FORMAT
#define XI_FRAME3D_SET_PIXEL_FORMAT XI_FRAME_SET_PIXEL_FORMAT

#define XI_FRAME3D_GET_PADDING_TYPE XI_FRAME_GET_PADDING_TYPE
#define XI_FRAME3D_SET_PADDING_TYPE XI_FRAME_SET_PADDING_TYPE

/*****************************************
*                   3D Tile Access Macros
*****************************************/

#define XI_TILE3D_GET_BUFF_PTR      XI_TILE_GET_BUFF_PTR
#define XI_TILE3D_SET_BUFF_PTR      XI_TILE_SET_BUFF_PTR

#define XI_TILE3D_GET_BUFF_SIZE     XI_TILE_GET_BUFF_SIZE
#define XI_TILE3D_SET_BUFF_SIZE     XI_TILE_SET_BUFF_SIZE

#define XI_TILE3D_GET_DATA_PTR      XI_TILE_GET_DATA_PTR
#define XI_TILE3D_SET_DATA_PTR      XI_TILE_SET_DATA_PTR

#define XI_TILE3D_GET_STATUS_FLAGS  XI_TILE_GET_STATUS_FLAGS
#define XI_TILE3D_SET_STATUS_FLAGS  XI_TILE_SET_STATUS_FLAGS

#define XI_TILE3D_GET_TYPE          XI_TILE_GET_TYPE
#define XI_TILE3D_SET_TYPE          XI_TILE_SET_TYPE

#define XI_TILE3D_GET_ELEMENT_TYPE  XI_TILE_GET_ELEMENT_TYPE
#define XI_TILE3D_GET_ELEMENT_SIZE  XI_TILE_GET_ELEMENT_SIZE
#define XI_TILE3D_IS_TILE           XI_TILE_IS_TILE

#define XI_TILE3D_GET_FRAME_PTR(pTile3D)           ((pTile3D)->pFrame)
#define XI_TILE3D_SET_FRAME_PTR(pTile3D, ptrFrame) (pTile3D)->pFrame = ((xi_pFrame3D) (ptrFrame))

#define XI_TILE3D_CHECK_STATUS_FLAGS_DMA_ONGOING         XI_TILE_CHECK_STATUS_FLAGS_DMA_ONGOING

/***********************************
*              Other Marcos
***********************************/

#define XI_TILE3D_CHECK_VIRTUAL_FRAME  XI_TILE_CHECK_VIRTUAL_FRAME
#define XI_FRAME3D_CHECK_VIRTUAL_FRAME XI_FRAME_CHECK_VIRTUAL_FRAME

#define SETUP_TILE3D(type, pTile, pBuf, pFrame, bufSize, dim1Size, dim2Size, dim3Size, dim1Pitch, dim2Pitch,  \
      dim1Edge1, dim1Edge2, dim2Edge1, dim2Edge2, dim3Edge1, dim3Edge2, dim1Loc, dim2Loc, dim3Loc, dataOrder, \
      alignType)                                                                                              \
  {                                                                                                           \
    XI_TILE3D_SET_TYPE(pTile, type);                                                                          \
    XI_TILE3D_SET_FRAME_PTR(pTile, pFrame);                                                                   \
    XI_TILE3D_SET_BUFF_PTR(pTile, pBuf);                                                                      \
    XI_TILE3D_SET_BUFF_SIZE(pTile, bufSize);                                                                  \
    XI_TILE3D_SET_DIM1(pTile, dim1Size);                                                                      \
    XI_TILE3D_SET_DIM2(pTile, dim2Size);                                                                      \
    XI_TILE3D_SET_DIM3(pTile, dim3Size);                                                                      \
    XI_TILE3D_SET_DIM1_PITCH(pTile, dim1Pitch);                                                               \
    XI_TILE3D_SET_DIM2_PITCH(pTile, dim2Pitch);                                                               \
    uint8_t *edgePtr = (uint8_t *) pBuf, *dataPtr;                                                            \
    int32_t alignment    = 63;                                                                                \
    if ((alignType == EDGE_ALIGNED_32) || (alignType == DATA_ALIGNED_32)) { alignment = 31; }                 \
    if ((alignType == EDGE_ALIGNED_32) || (alignType == EDGE_ALIGNED_64))                                     \
    {                                                                                                         \
      edgePtr = (uint8_t *) (((long) (pBuf) + alignment) & (~alignment));                                     \
    }                                                                                                         \
    XI_TILE3D_SET_DATA_PTR(pTile, edgePtr + ((dim3Edge1) * (dim2Pitch) +                                      \
                     (dim2Edge1) * (dim1Pitch) + (dim1Edge1)) * XI_TILE3D_GET_ELEMENT_SIZE(pTile));           \
    if ((alignType == DATA_ALIGNED_32) || (alignType == DATA_ALIGNED_64))                                     \
    {                                                                                                         \
      dataPtr = (uint8_t *) XI_TILE3D_GET_DATA_PTR(pTile);                                                    \
      dataPtr = (uint8_t *) (((long) (dataPtr) + alignment) & (~alignment));                                  \
      XI_TILE3D_SET_DATA_PTR(pTile, dataPtr);                                                                 \
    }                                                                                                         \
    XI_TILE3D_SET_DIM1_EDGE1(pTile, dim1Edge1);                                                               \
    XI_TILE3D_SET_DIM1_EDGE2(pTile, dim1Edge2);                                                               \
    XI_TILE3D_SET_DIM2_EDGE1(pTile, dim2Edge1);                                                               \
    XI_TILE3D_SET_DIM2_EDGE2(pTile, dim2Edge2);                                                               \
    XI_TILE3D_SET_DIM3_EDGE1(pTile, dim3Edge1);                                                               \
    XI_TILE3D_SET_DIM3_EDGE2(pTile, dim3Edge2);                                                               \
    XI_TILE3D_SET_DIM1_COORD(pTile, dim1Loc);                                                                 \
    XI_TILE3D_SET_DIM2_COORD(pTile, dim2Loc);                                                                 \
    XI_TILE3D_SET_DIM3_COORD(pTile, dim3Loc);                                                                 \
    XI_TILE3D_SET_DATA_ORDER(pTile, dataOrder);                                                               \
  }

#define SETUP_FRAME3D(pFrame, pFrameBuffer, bufSize, dim1Size, dim2Size, dim3Size, dim1Pitch, dim2Pitch,         \
    dim1Edge1, dim1Edge2, dim2Edge1, dim2Edge2, dim3Edge1, dim3Edge2, pixRes, pixPackFormat, paddingType,        \
    dataOrder)                                                                                                   \
  {                                                                                                              \
    XI_FRAME3D_SET_BUFF_PTR(pFrame, pFrameBuffer);                                                               \
    XI_FRAME3D_SET_BUFF_SIZE(pFrame, bufSize);                                                                   \
    XI_FRAME3D_SET_DIM1(pFrame, dim1Size);                                                                       \
    XI_FRAME3D_SET_DIM2(pFrame, dim2Size);                                                                       \
    XI_FRAME3D_SET_DIM3(pFrame, dim3Size);                                                                       \
    XI_FRAME3D_SET_DIM1_PITCH(pFrame, dim1Pitch);                                                                \
    XI_FRAME3D_SET_DIM2_PITCH(pFrame, dim2Pitch);                                                                \
    XI_FRAME3D_SET_DATA_PTR(pFrame, pFrameBuffer + ((dim3Edge1) * (dim2Pitch) +                                  \
                                                    (dim2Edge1) * (dim1Pitch) + (dim1Edge1))* pixRes);           \
    XI_FRAME3D_SET_DIM1_EDGE1(pFrame, dim1Edge1);                                                                \
    XI_FRAME3D_SET_DIM1_EDGE2(pFrame, dim1Edge2);                                                                \
    XI_FRAME3D_SET_DIM2_EDGE1(pFrame, dim2Edge1);                                                                \
    XI_FRAME3D_SET_DIM2_EDGE2(pFrame, dim2Edge2);                                                                \
    XI_FRAME3D_SET_DIM3_EDGE1(pFrame, dim3Edge1);                                                                \
    XI_FRAME3D_SET_DIM3_EDGE2(pFrame, dim3Edge2);                                                                \
    XI_FRAME3D_SET_PIXEL_RES(pFrame, pixRes);                                                                    \
    XI_FRAME3D_SET_PIXEL_FORMAT(pFrame, pixPackFormat);                                                          \
    XI_FRAME3D_SET_PADDING_TYPE(pFrame, paddingType);                                                            \
    XI_FRAME3D_SET_DATA_ORDER(pFrame, dataOrder);                                                                \
  }

#define WAIT_FOR_TILE3D WAIT_FOR_TILE

#define XI_COPY_FRAME3D_TO_TILE3D(frame, tile) {                      \
    XI_TILE3D_SET_DIM1(tile, XI_FRAME3D_GET_DIM1(frame));             \
    XI_TILE3D_SET_DIM1_PITCH(tile, XI_FRAME3D_GET_DIM1_PITCH(frame)); \
    XI_TILE3D_SET_DIM1_EDGE1(tile, XI_FRAME3D_GET_DIM1_EDGE1(frame)); \
    XI_TILE3D_SET_DIM1_EDGE2(tile, XI_FRAME3D_GET_DIM1_EDGE2(frame)); \
    XI_TILE3D_SET_DIM2(tile, XI_FRAME3D_GET_DIM2(frame));             \
    XI_TILE3D_SET_DIM2_PITCH(tile, XI_FRAME3D_GET_DIM2_PITCH(frame)); \
    XI_TILE3D_SET_DIM2_EDGE1(tile, XI_FRAME3D_GET_DIM2_EDGE1(frame)); \
    XI_TILE3D_SET_DIM2_EDGE2(tile, XI_FRAME3D_GET_DIM2_EDGE2(frame)); \
    XI_TILE3D_SET_DIM3(tile, XI_FRAME3D_GET_DIM3(frame));             \
    XI_TILE3D_SET_DIM3_EDGE1(tile, XI_FRAME3D_GET_DIM3_EDGE1(frame)); \
    XI_TILE3D_SET_DIM3_EDGE2(tile, XI_FRAME3D_GET_DIM3_EDGE2(frame)); \
    XI_TILE3D_SET_DATA_PTR(tile, XI_FRAME3D_GET_DATA_PTR(frame));     \
    XI_TILE3D_SET_DATA_ORDER(tile, XI_FRAME3D_GET_DATA_ORDER(frame)); \
}

#define XI_COPY_FRAME3D_TO_FRAME3D(frameIn, frameOut) {                      \
    XI_FRAME3D_SET_DIM1(frameOut, XI_FRAME3D_GET_DIM1(frameIn));             \
    XI_FRAME3D_SET_DIM1_PITCH(frameOut, XI_FRAME3D_GET_DIM1_PITCH(frameIn)); \
    XI_FRAME3D_SET_DIM1_EDGE1(frameOut, XI_FRAME3D_GET_DIM1_EDGE1(frameIn)); \
    XI_FRAME3D_SET_DIM1_EDGE2(frameOut, XI_FRAME3D_GET_DIM1_EDGE2(frameIn)); \
    XI_FRAME3D_SET_DIM2(frameOut, XI_FRAME3D_GET_DIM2(frameIn));             \
    XI_FRAME3D_SET_DIM2_PITCH(frameOut, XI_FRAME3D_GET_DIM2_PITCH(frameIn)); \
    XI_FRAME3D_SET_DIM2_EDGE1(frameOut, XI_FRAME3D_GET_DIM2_EDGE1(frameIn)); \
    XI_FRAME3D_SET_DIM2_EDGE2(frameOut, XI_FRAME3D_GET_DIM2_EDGE2(frameIn)); \
    XI_FRAME3D_SET_DIM3(frameOut, XI_FRAME3D_GET_DIM3(frameIn));             \
    XI_FRAME3D_SET_DIM3_EDGE1(frameOut, XI_FRAME3D_GET_DIM2_EDGE1(frameIn)); \
    XI_FRAME3D_SET_DIM3_EDGE2(frameOut, XI_FRAME3D_GET_DIM2_EDGE2(frameIn)); \
    XI_FRAME3D_SET_DATA_PTR(frameOut, XI_FRAME3D_GET_DATA_PTR(frameIn));     \
    XI_FRAME3D_SET_DATA_ORDER(frameOut, XI_FRAME3D_GET_DATA_ORDER(frameIn)); \
    XI_FRAME3D_SET_PIXEL_RES(frameOut, XI_FRAME3D_GET_PIXEL_RES(frameIn));   \
}

#define XI_COPY_TILE3D_TO_TILE3D(tileIn, tileOut) {                      \
    XI_TILE3D_SET_DIM1(tileOut, XI_TILE3D_GET_DIM1(tileIn));             \
    XI_TILE3D_SET_DIM1_PITCH(tileOut, XI_TILE3D_GET_DIM1_PITCH(tileIn)); \
    XI_TILE3D_SET_DIM1_EDGE1(tileOut, XI_TILE3D_GET_DIM1_EDGE1(tileIn)); \
    XI_TILE3D_SET_DIM1_EDGE2(tileOut, XI_TILE3D_GET_DIM1_EDGE2(tileIn)); \
    XI_TILE3D_SET_DIM2(tileOut, XI_TILE3D_GET_DIM2(tileIn));             \
    XI_TILE3D_SET_DIM2_PITCH(tileOut, XI_TILE3D_GET_DIM2_PITCH(tileIn)); \
    XI_TILE3D_SET_DIM2_EDGE1(tileOut, XI_TILE3D_GET_DIM2_EDGE1(tileIn)); \
    XI_TILE3D_SET_DIM2_EDGE2(tileOut, XI_TILE3D_GET_DIM2_EDGE2(tileIn)); \
    XI_TILE3D_SET_DIM3(tileOut, XI_TILE3D_GET_DIM3(tileIn));             \
    XI_TILE3D_SET_DIM3_EDGE1(tileOut, XI_TILE3D_GET_DIM3_EDGE1(tileIn)); \
    XI_TILE3D_SET_DIM3_EDGE2(tileOut, XI_TILE3D_GET_DIM3_EDGE2(tileIn)); \
    XI_TILE3D_SET_DATA_PTR(tileOut, XI_TILE3D_GET_DATA_PTR(tileIn));     \
    XI_TILE3D_SET_DATA_ORDER(tileOut, XI_TILE3D_GET_DATA_ORDER(tileIn)); \
}

// Assumes 8 bit pixRes and Edge1 = Edge2
#define XI_TILE3D_UPDATE_EDGE_DIM1(pTile, newEdgeSize)                     \
  {                                                                        \
    uint16_t currEdgeSize  = (uint16_t) XI_TILE3D_GET_DIM1_EDGE1(pTile);   \
    uint32_t dim1Pitch     = (uint32_t) XI_TILE3D_GET_DIM1_PITCH(pTile);   \
    uint32_t dataU32       = (uint32_t) XI_TILE3D_GET_DATA_PTR(pTile);     \
    dataU32 = dataU32 + newEdgeSize - currEdgeSize;                        \
    XI_TILE3D_SET_DATA_PTR(pTile, (void *) dataU32);                       \
    XI_TILE3D_SET_DIM1_EDGE1(pTile, newEdgeSize);                          \
    XI_TILE3D_SET_DIM1_EDGE2(pTile, newEdgeSize);                          \
    XI_TILE3D_SET_DIM1(pTile, dim1Pitch - 2 * newEdgeSize);                \
  }

// Assumes 8 bit pixRes and Edge1 = Edge2
#define XI_TILE3D_UPDATE_EDGE_DIM2(pTile, newEdgeSize)                     \
  {                                                                        \
    uint16_t currEdgeSize  = (uint16_t) XI_TILE3D_GET_DIM2_EDGE1(pTile);   \
    uint32_t dim1Pitch     = (uint32_t) XI_TILE3D_GET_DIM1_PITCH(pTile);   \
    uint16_t dim2Size      = (uint16_t) XI_TILE3D_GET_DIM2(pTile);         \
    uint32_t dataU32       = (uint32_t) XI_TILE3D_GET_DATA_PTR(pTile);     \
    dataU32 = dataU32 + dim1Pitch * (newEdgeSize - currEdgeSize);          \
    XI_TILE3D_SET_DATA_PTR(pTile, (void *) dataU32);                       \
    XI_TILE3D_SET_DIM2_EDGE1(pTile, newEdgeSize);                          \
    XI_TILE3D_SET_DIM2_EDGE2(pTile, newEdgeSize);                          \
    XI_TILE3D_SET_DIM2(pTile, dim2Size + 2 * (currEdgeSize-newEdgeSize));  \
  }

// Assumes 8 bit pixRes and Edge1 = Edge2
#define XI_TILE3D_UPDATE_EDGE_DIM3(pTile, newEdgeSize)                     \
  {                                                                        \
    uint16_t currEdgeSize  = (uint16_t) XI_TILE3D_GET_DIM3_EDGE1(pTile);   \
    uint32_t dim2Pitch     = (uint32_t) XI_TILE3D_GET_DIM2_PITCH(pTile);   \
    uint16_t dim3Size      = (uint16_t) XI_TILE3D_GET_DIM3(pTile);         \
    uint32_t dataU32       = (uint32_t) XI_TILE3D_GET_DATA_PTR(pTile);     \
    dataU32 = dataU32 + dim2Pitch * (newEdgeSize - currEdgeSize);          \
    XI_TILE3D_SET_DATA_PTR(pTile, (void *) dataU32);                       \
    XI_TILE3D_SET_DIM3_EDGE1(pTile, newEdgeSize);                          \
    XI_TILE3D_SET_DIM3_EDGE2(pTile, newEdgeSize);                          \
    XI_TILE3D_SET_DIM3(pTile, dim3Size + 2 * (currEdgeSize-newEdgeSize));  \
  }

#define XI_TILE3D_UPDATE_DIMENSIONS(pTile, dim1Loc, dim2Loc, dim3Loc, dim1Size, dim2Size, dim3Size,  \
                dim1Pitch, dim2Pitch)                                                                \
  {                                                                                                  \
    XI_TILE3D_SET_DIM1_COORD(pTile, dim1Loc);                                                        \
    XI_TILE3D_SET_DIM2_COORD(pTile, dim2Loc);                                                        \
    XI_TILE3D_SET_DIM3_COORD(pTile, dim3Loc);                                                        \
    XI_TILE3D_SET_DIM1(pTile, dim1Size);                                                             \
    XI_TILE3D_SET_DIM2(pTile, dim2Size);                                                             \
    XI_TILE3D_SET_DIM3(pTile, dim3Size);                                                             \
    XI_TILE3D_SET_DIM1_PITCH(pTile, dim1Pitch);                                                      \
    XI_TILE3D_SET_DIM2_PITCH(pTile, dim2Pitch);                                                      \
  }

/******************************************************************************************************************
 *
 *                    4D definitions - extension of 3D definitions
 *
 * ****************************************************************************************************************/

typedef struct xi_frame4DStruct
{
  void     *pFrameBuff;
  uint32_t frameBuffSize;
  void     *pFrameData;
  int32_t  dim1Size;
  int32_t  dim2Size;
  int32_t  dim1Pitch; // pitch in width dimension
  uint8_t  pixelRes;  // in bits
  uint8_t  pixelPackFormat;
  uint16_t  dim1Edge1;
  uint16_t  dim1Edge2;
  uint16_t  dim2Edge1;
  uint16_t  dim2Edge2;
  uint16_t  dim3Edge1;
  uint16_t  dim3Edge2;
  uint8_t  paddingType;
  // new fields
  int32_t  dim2Pitch;
  int32_t  dim3Size;
  xi_cnn_data_order dataOrder; // WHD, DWH, WHDN, NWHD, etc.
  // new fields
  int32_t  dim3Pitch;
  int32_t  dim4Size;
} xi_frame4D, *xi_pFrame4D;

// new access macros
#define XI_FRAME4D_GET_DIM1                XI_FRAME3D_GET_DIM1
#define XI_FRAME4D_SET_DIM1                XI_FRAME3D_SET_DIM1
#define XI_FRAME4D_GET_DIM1_PITCH          XI_FRAME3D_GET_DIM1_PITCH
#define XI_FRAME4D_SET_DIM1_PITCH          XI_FRAME3D_SET_DIM1_PITCH
#define XI_FRAME4D_GET_DIM1_PITCH_IN_BYTES XI_FRAME3D_GET_DIM1_PITCH_IN_BYTES
#define XI_FRAME4D_GET_DIM2                XI_FRAME3D_GET_DIM2
#define XI_FRAME4D_SET_DIM2                XI_FRAME3D_SET_DIM2
#define XI_FRAME4D_GET_DIM2_PITCH          XI_FRAME3D_GET_DIM2_PITCH
#define XI_FRAME4D_SET_DIM2_PITCH          XI_FRAME3D_SET_DIM2_PITCH
#define XI_FRAME4D_GET_DIM2_PITCH_IN_BYTES XI_FRAME3D_GET_DIM2_PITCH_IN_BYTES
#define XI_FRAME4D_GET_DIM3                XI_FRAME3D_GET_DIM3
#define XI_FRAME4D_SET_DIM3                XI_FRAME3D_SET_DIM3
#define XI_FRAME4D_GET_DATA_ORDER          XI_FRAME3D_GET_DATA_ORDER
#define XI_FRAME4D_SET_DATA_ORDER          XI_FRAME3D_SET_DATA_ORDER
#define XI_FRAME4D_GET_DIM1_EDGE1          XI_FRAME3D_GET_DIM1_EDGE1
#define XI_FRAME4D_SET_DIM1_EDGE1          XI_FRAME3D_SET_DIM1_EDGE1
#define XI_FRAME4D_GET_DIM1_EDGE2          XI_FRAME3D_GET_DIM1_EDGE2
#define XI_FRAME4D_SET_DIM1_EDGE2          XI_FRAME3D_SET_DIM1_EDGE2
#define XI_FRAME4D_GET_DIM2_EDGE1          XI_FRAME3D_GET_DIM2_EDGE1
#define XI_FRAME4D_SET_DIM2_EDGE1          XI_FRAME3D_SET_DIM2_EDGE1
#define XI_FRAME4D_GET_DIM2_EDGE2          XI_FRAME3D_GET_DIM2_EDGE2
#define XI_FRAME4D_SET_DIM2_EDGE2          XI_FRAME3D_SET_DIM2_EDGE2
#define XI_FRAME4D_GET_DIM4(x)             ((x)->dim4Size)
#define XI_FRAME4D_SET_DIM4(x,v)           ((x)->dim4Size = (v))
#define XI_FRAME4D_GET_DIM3_PITCH(x)       ((x)->dim3Pitch)
#define XI_FRAME4D_SET_DIM3_PITCH(x,v)     ((x)->dim3Pitch = (v))
#define XI_FRAME4D_GET_DIM3_EDGE1(x)       ((x)->dim3Edge1)
#define XI_FRAME4D_SET_DIM3_EDGE1(x,v)     ((x)->dim3Edge1 = (v))
#define XI_FRAME4D_GET_DIM3_EDGE2(x)       ((x)->dim3Edge2)
#define XI_FRAME4D_SET_DIM3_EDGE2(x,v)     ((x)->dim3Edge2 = (v))

// 4D tile

typedef struct xi_tile4DStruct
{
  void       *pBuffer;
  uint32_t    bufferSize;
  void       *pData;
  int32_t     dim1Size;
  int32_t     dim1Pitch;
  uint32_t    status;  //Currently not used, planned to be obsolete
  uint16_t    type;
  int32_t     dim2Size;
  xi_frame4D *pFrame;
  int32_t     dim1Loc; // dim1-loc of top-left active pixel in src frame
  int32_t     dim2Loc; // dim2-loc of top-left active pixel in src frame
  uint16_t    dim1Edge1;
  uint16_t    dim2Edge1;
  uint16_t    dim1Edge2;
  uint16_t    dim2Edge2;
  // new fields
  int32_t       dim2Pitch;
  int32_t       dim3Size;
  xi_cnn_data_order dataOrder;
  int32_t       dim3Loc;   // dim3-loc of top-left active pixel in src frame
  uint16_t      dim3Edge1;
  uint16_t      dim3Edge2;
  // new fields
  int32_t       dim3Pitch;
  int32_t       dim4Size;  // 4th dimension is num for lack of better term
  int32_t       dim4Loc;   // dim4-loc of top-left active pixel in src frame
} xi_tile4D, *xi_pTile4D;

#define XI_TILE4D_GET_DIM1                XI_TILE3D_GET_DIM1
#define XI_TILE4D_SET_DIM1                XI_TILE3D_SET_DIM1
#define XI_TILE4D_GET_DIM1_PITCH          XI_TILE3D_GET_DIM1_PITCH
#define XI_TILE4D_SET_DIM1_PITCH          XI_TILE3D_SET_DIM1_PITCH
#define XI_TILE4D_GET_DIM2                XI_TILE3D_GET_DIM2
#define XI_TILE4D_SET_DIM2                XI_TILE3D_SET_DIM2
#define XI_TILE4D_GET_DIM2_PITCH          XI_TILE3D_GET_DIM2_PITCH
#define XI_TILE4D_SET_DIM2_PITCH          XI_TILE3D_SET_DIM2_PITCH
#define XI_TILE4D_GET_DIM3                XI_TILE3D_GET_DIM3
#define XI_TILE4D_SET_DIM3                XI_TILE3D_SET_DIM3
#define XI_TILE4D_GET_DIM3_PITCH(x)       ((x)->dim3Pitch)
#define XI_TILE4D_SET_DIM3_PITCH(x,v)     ((x)->dim3Pitch = (v))
#define XI_TILE4D_GET_DIM4(x)             ((x)->dim4Size)
#define XI_TILE4D_SET_DIM4(x,v)           ((x)->dim4Size = (v))
#define XI_TILE4D_GET_DIM1_EDGE1          XI_TILE3D_GET_DIM1_EDGE1
#define XI_TILE4D_SET_DIM1_EDGE1          XI_TILE3D_SET_DIM1_EDGE1
#define XI_TILE4D_GET_DIM1_EDGE2          XI_TILE3D_GET_DIM1_EDGE2
#define XI_TILE4D_SET_DIM1_EDGE2          XI_TILE3D_SET_DIM1_EDGE2
#define XI_TILE4D_GET_DIM2_EDGE1          XI_TILE3D_GET_DIM2_EDGE1
#define XI_TILE4D_SET_DIM2_EDGE1          XI_TILE3D_SET_DIM2_EDGE1
#define XI_TILE4D_GET_DIM2_EDGE2          XI_TILE3D_GET_DIM2_EDGE2
#define XI_TILE4D_SET_DIM2_EDGE2          XI_TILE3D_SET_DIM2_EDGE2
#define XI_TILE4D_GET_DIM3_EDGE1          XI_TILE3D_GET_DIM3_EDGE1
#define XI_TILE4D_SET_DIM3_EDGE1          XI_TILE3D_SET_DIM3_EDGE1
#define XI_TILE4D_GET_DIM3_EDGE2          XI_TILE3D_GET_DIM3_EDGE2
#define XI_TILE4D_SET_DIM3_EDGE2          XI_TILE3D_SET_DIM3_EDGE2
#define XI_TILE4D_GET_DATA_ORDER          XI_TILE3D_GET_DATA_ORDER
#define XI_TILE4D_SET_DATA_ORDER          XI_TILE3D_SET_DATA_ORDER
#define XI_TILE4D_GET_DIM1_COORD          XI_TILE3D_GET_DIM1_COORD
#define XI_TILE4D_SET_DIM1_COORD          XI_TILE3D_SET_DIM1_COORD
#define XI_TILE4D_GET_DIM2_COORD          XI_TILE3D_GET_DIM2_COORD
#define XI_TILE4D_SET_DIM2_COORD          XI_TILE3D_SET_DIM2_COORD
#define XI_TILE4D_GET_DIM3_COORD          XI_TILE3D_GET_DIM3_COORD
#define XI_TILE4D_SET_DIM3_COORD          XI_TILE3D_SET_DIM3_COORD
#define XI_TILE4D_GET_DIM4_COORD(x)       ((x)->dim4Loc)
#define XI_TILE4D_SET_DIM4_COORD(x,v)     ((x)->dim4Loc = (v))


/*****************************************
*   Data type definitions
*****************************************/

// added new bit to indicate 4D tile
#define XI_TYPE_TILE4D_BIT         (1 << 12)

#define XI_TYPE_IS_TILE4D(type)    ((type) & (XI_TYPE_TILE4D_BIT))

#define XI_TILE4D_U8  (XI_U8  | XI_TYPE_TILE4D_BIT | XI_TYPE_TILE3D_BIT | XI_TYPE_TILE_BIT)
#define XI_TILE4D_U16 (XI_U16 | XI_TYPE_TILE4D_BIT | XI_TYPE_TILE3D_BIT | XI_TYPE_TILE_BIT)
#define XI_TILE4D_U32 (XI_U32 | XI_TYPE_TILE4D_BIT | XI_TYPE_TILE3D_BIT | XI_TYPE_TILE_BIT)
#define XI_TILE4D_S8  (XI_S8  | XI_TYPE_TILE4D_BIT | XI_TYPE_TILE3D_BIT | XI_TYPE_TILE_BIT)
#define XI_TILE4D_S16 (XI_S16 | XI_TYPE_TILE4D_BIT | XI_TYPE_TILE3D_BIT | XI_TYPE_TILE_BIT)
#define XI_TILE4D_S32 (XI_S32 | XI_TYPE_TILE4D_BIT | XI_TYPE_TILE3D_BIT | XI_TYPE_TILE_BIT)
#define XI_TILE4D_F16 (XI_F16 | XI_TYPE_TILE4D_BIT | XI_TYPE_TILE3D_BIT | XI_TYPE_TILE_BIT)
#define XI_TILE4D_F32 (XI_F32 | XI_TYPE_TILE4D_BIT | XI_TYPE_TILE3D_BIT | XI_TYPE_TILE_BIT)
/*****************************************
*                   4D Frame Access Macros
*****************************************/

#define XI_FRAME4D_GET_BUFF_PTR     XI_FRAME_GET_BUFF_PTR
#define XI_FRAME4D_SET_BUFF_PTR     XI_FRAME_SET_BUFF_PTR

#define XI_FRAME4D_GET_BUFF_SIZE    XI_FRAME_GET_BUFF_SIZE
#define XI_FRAME4D_SET_BUFF_SIZE    XI_FRAME_SET_BUFF_SIZE

#define XI_FRAME4D_GET_DATA_PTR     XI_FRAME_GET_DATA_PTR
#define XI_FRAME4D_SET_DATA_PTR     XI_FRAME_SET_DATA_PTR

#define XI_FRAME4D_GET_PIXEL_RES    XI_FRAME_GET_PIXEL_RES
#define XI_FRAME4D_SET_PIXEL_RES    XI_FRAME_SET_PIXEL_RES

#define XI_FRAME4D_GET_PIXEL_FORMAT XI_FRAME_GET_PIXEL_FORMAT
#define XI_FRAME4D_SET_PIXEL_FORMAT XI_FRAME_SET_PIXEL_FORMAT

#define XI_FRAME4D_GET_PADDING_TYPE XI_FRAME_GET_PADDING_TYPE
#define XI_FRAME4D_SET_PADDING_TYPE XI_FRAME_SET_PADDING_TYPE

/*****************************************
*                   4D Tile Access Macros
*****************************************/

#define XI_TILE4D_GET_BUFF_PTR      XI_TILE_GET_BUFF_PTR
#define XI_TILE4D_SET_BUFF_PTR      XI_TILE_SET_BUFF_PTR

#define XI_TILE4D_GET_BUFF_SIZE     XI_TILE_GET_BUFF_SIZE
#define XI_TILE4D_SET_BUFF_SIZE     XI_TILE_SET_BUFF_SIZE

#define XI_TILE4D_GET_DATA_PTR      XI_TILE_GET_DATA_PTR
#define XI_TILE4D_SET_DATA_PTR      XI_TILE_SET_DATA_PTR

#define XI_TILE4D_GET_STATUS_FLAGS  XI_TILE_GET_STATUS_FLAGS
#define XI_TILE4D_SET_STATUS_FLAGS  XI_TILE_SET_STATUS_FLAGS

#define XI_TILE4D_GET_TYPE          XI_TILE_GET_TYPE
#define XI_TILE4D_SET_TYPE          XI_TILE_SET_TYPE

#define XI_TILE4D_GET_ELEMENT_TYPE  XI_TILE_GET_ELEMENT_TYPE
#define XI_TILE4D_GET_ELEMENT_SIZE  XI_TILE_GET_ELEMENT_SIZE
#define XI_TILE4D_IS_TILE           XI_TILE_IS_TILE

#define XI_TILE4D_GET_FRAME_PTR(pTile4D)           ((pTile4D)->pFrame)
#define XI_TILE4D_SET_FRAME_PTR(pTile4D, ptrFrame) (pTile4D)->pFrame = ((xi_pFrame4D) (ptrFrame))

#define XI_TILE4D_CHECK_STATUS_FLAGS_DMA_ONGOING         XI_TILE_CHECK_STATUS_FLAGS_DMA_ONGOING
#define XI_TILE4D_CHECK_STATUS_FLAGS_EDGE_PADDING_NEEDED XI_TILE_CHECK_STATUS_FLAGS_EDGE_PADDING_NEEDED

/***********************************
*              Other Marcos
***********************************/

#define XI_TILE4D_CHECK_VIRTUAL_FRAME  XI_TILE_CHECK_VIRTUAL_FRAME
#define XI_FRAME4D_CHECK_VIRTUAL_FRAME XI_FRAME_CHECK_VIRTUAL_FRAME

#define SETUP_TILE4D(type, pTile, pBuf, pFrame, bufSize, dim1Size, dim2Size, dim3Size, dim4Size, dim1Pitch, dim2Pitch,   \
          dim3Pitch, dim1Edge1, dim1Edge2, dim2Edge1, dim2Edge2, dim3Edge1, dim3Edge2,                                   \
          dim1Loc, dim2Loc, dim3Loc, dim4Loc, dataOrder, alignType)                                                      \
  {                                                                                                                      \
    XI_TILE4D_SET_TYPE(pTile, type);                                                                                     \
    XI_TILE4D_SET_FRAME_PTR(pTile, pFrame);                                                                              \
    XI_TILE4D_SET_BUFF_PTR(pTile, pBuf);                                                                                 \
    XI_TILE4D_SET_BUFF_SIZE(pTile, bufSize);                                                                             \
    XI_TILE4D_SET_DIM1(pTile, dim1Size);                                                                                 \
    XI_TILE4D_SET_DIM2(pTile, dim2Size);                                                                                 \
    XI_TILE4D_SET_DIM3(pTile, dim3Size);                                                                                 \
    XI_TILE4D_SET_DIM4(pTile, dim4Size);                                                                                 \
    XI_TILE4D_SET_DIM1_PITCH(pTile, dim1Pitch);                                                                          \
    XI_TILE4D_SET_DIM2_PITCH(pTile, dim2Pitch);                                                                          \
    XI_TILE4D_SET_DIM3_PITCH(pTile, dim3Pitch);                                                                          \
    uint8_t *edgePtr = (uint8_t *) pBuf, *dataPtr;                                                                       \
    int32_t alignment    = 63;                                                                                           \
    if ((alignType == EDGE_ALIGNED_32) || (alignType == DATA_ALIGNED_32)) { alignment = 31; }                            \
    if ((alignType == EDGE_ALIGNED_32) || (alignType == EDGE_ALIGNED_64))                                                \
    {                                                                                                                    \
      edgePtr = (uint8_t *) (((long) (pBuf) + alignment) & (~alignment));                                                \
    }                                                                                                                    \
    XI_TILE4D_SET_DATA_PTR(pTile, edgePtr + ((dim3Edge1) * (dim2Pitch) + (dim2Edge1) * (dim1Pitch) + (dim1Edge1))        \
                                    *  XI_TILE4D_GET_ELEMENT_SIZE(pTile));                                               \
    if ((alignType == DATA_ALIGNED_32) || (alignType == DATA_ALIGNED_64))                                                \
    {                                                                                                                    \
      dataPtr = (uint8_t *) XI_TILE4D_GET_DATA_PTR(pTile);                                                               \
      dataPtr = (uint8_t *) (((long) (dataPtr) + alignment) & (~alignment));                                             \
      XI_TILE4D_SET_DATA_PTR(pTile, dataPtr);                                                                            \
    }                                                                                                                    \
    XI_TILE4D_SET_DIM1_EDGE1(pTile, dim1Edge1);                                                                          \
    XI_TILE4D_SET_DIM1_EDGE2(pTile, dim1Edge2);                                                                          \
    XI_TILE4D_SET_DIM2_EDGE1(pTile, dim2Edge1);                                                                          \
    XI_TILE4D_SET_DIM2_EDGE2(pTile, dim2Edge2);                                                                          \
    XI_TILE4D_SET_DIM3_EDGE1(pTile, dim3Edge1);                                                                          \
    XI_TILE4D_SET_DIM3_EDGE2(pTile, dim3Edge2);                                                                          \
    XI_TILE4D_SET_DIM1_COORD(pTile, dim1Loc);                                                                            \
    XI_TILE4D_SET_DIM2_COORD(pTile, dim2Loc);                                                                            \
    XI_TILE4D_SET_DIM3_COORD(pTile, dim3Loc);                                                                            \
    XI_TILE4D_SET_DIM4_COORD(pTile, dim4Loc);                                                                            \
    XI_TILE4D_SET_DATA_ORDER(pTile, dataOrder);                                                                          \
  }

#define SETUP_FRAME4D(pFrame, pFrameBuffer, bufSize, dim1Size, dim2Size, dim3Size, dim4Size, dim1Pitch, dim2Pitch, dim3Pitch,  \
  dim1Edge1, dim1Edge2, dim2Edge1, dim2Edge2, dim3Edge1, dim3Edge2, pixRes, pixPackFormat, paddingType, dataOrder)             \
  {                                                                                                                            \
    XI_FRAME4D_SET_BUFF_PTR(pFrame, pFrameBuffer);                                                                             \
    XI_FRAME4D_SET_BUFF_SIZE(pFrame, bufSize);                                                                                 \
    XI_FRAME4D_SET_DIM1(pFrame, dim1Size);                                                                                     \
    XI_FRAME4D_SET_DIM2(pFrame, dim2Size);                                                                                     \
    XI_FRAME4D_SET_DIM3(pFrame, dim3Size);                                                                                     \
    XI_FRAME4D_SET_DIM4(pFrame, dim4Size);                                                                                     \
    XI_FRAME4D_SET_DIM1_PITCH(pFrame, dim1Pitch);                                                                              \
    XI_FRAME4D_SET_DIM2_PITCH(pFrame, dim2Pitch);                                                                              \
    XI_FRAME4D_SET_DIM3_PITCH(pFrame, dim3Pitch);                                                                              \
    XI_FRAME4D_SET_DATA_PTR(pFrame, pFrameBuffer + ((dim3Edge1) * (dim2Pitch) + (dim2Edge1) * (dim1Pitch) +                    \
                                                    (dim1Edge1)) * pixRes);                                                    \
    XI_FRAME4D_SET_DIM1_EDGE1(pFrame, dim1Edge1);                                                                              \
    XI_FRAME4D_SET_DIM1_EDGE2(pFrame, dim1Edge2);                                                                              \
    XI_FRAME4D_SET_DIM2_EDGE1(pFrame, dim2Edge1);                                                                              \
    XI_FRAME4D_SET_DIM2_EDGE2(pFrame, dim2Edge2);                                                                              \
    XI_FRAME4D_SET_DIM3_EDGE1(pFrame, dim3Edge1);                                                                              \
    XI_FRAME4D_SET_DIM3_EDGE2(pFrame, dim3Edge2);                                                                              \
    XI_FRAME4D_SET_PIXEL_RES(pFrame, pixRes);                                                                                  \
    XI_FRAME4D_SET_PIXEL_FORMAT(pFrame, pixPackFormat);                                                                        \
    XI_FRAME4D_SET_PADDING_TYPE(pFrame, paddingType);                                                                          \
    XI_FRAME4D_SET_DATA_ORDER(pTile, dataOrder);                                                                               \
  }

#define WAIT_FOR_TILE4D WAIT_FOR_TILE

#define XI_COPY_FRAME4D_TO_TILE4D(frame, tile) {                      \
    XI_TILE4D_SET_DIM1(tile, XI_FRAME4D_GET_DIM1(frame));             \
    XI_TILE4D_SET_DIM1_PITCH(tile, XI_FRAME4D_GET_DIM1_PITCH(frame)); \
    XI_TILE4D_SET_DIM1_EDGE1(tile, XI_FRAME4D_GET_DIM1_EDGE1(frame)); \
    XI_TILE4D_SET_DIM1_EDGE2(tile, XI_FRAME4D_GET_DIM1_EDGE2(frame)); \
    XI_TILE4D_SET_DIM2(tile, XI_FRAME4D_GET_DIM2(frame));             \
    XI_TILE4D_SET_DIM2_PITCH(tile, XI_FRAME4D_GET_DIM2_PITCH(frame)); \
    XI_TILE4D_SET_DIM2_EDGE1(tile, XI_FRAME4D_GET_DIM2_EDGE1(frame)); \
    XI_TILE4D_SET_DIM2_EDGE2(tile, XI_FRAME4D_GET_DIM2_EDGE2(frame)); \
    XI_TILE4D_SET_DIM3(tile, XI_FRAME4D_GET_DIM3(frame));             \
    XI_TILE4D_SET_DIM3_PITCH(tile, XI_FRAME4D_GET_DIM3_PITCH(frame)); \
    XI_TILE4D_SET_DIM3_EDGE1(tile, XI_FRAME4D_GET_DIM3_EDGE1(frame)); \
    XI_TILE4D_SET_DIM3_EDGE2(tile, XI_FRAME4D_GET_DIM3_EDGE2(frame)); \
    XI_TILE4D_SET_DIM4(tile, XI_FRAME4D_GET_DIM4(frame));             \
    XI_TILE4D_SET_DATA_PTR(tile, XI_FRAME4D_GET_DATA_PTR(frame));     \
    XI_TILE4D_SET_DATA_ORDER(tile, XI_FRAME4D_GET_DATA_ORDER(frame)); \
}

#define XI_COPY_FRAME4D_TO_FRAME4D(frameIn, frameOut) {                      \
    XI_FRAME4D_SET_DIM1(frameOut, XI_FRAME4D_GET_DIM1(frameIn));             \
    XI_FRAME4D_SET_DIM1_PITCH(frameOut, XI_FRAME4D_GET_DIM1_PITCH(frameIn)); \
    XI_FRAME4D_SET_DIM1_EDGE1(frameOut, XI_FRAME4D_GET_DIM1_EDGE1(frameIn)); \
    XI_FRAME4D_SET_DIM1_EDGE2(frameOut, XI_FRAME4D_GET_DIM1_EDGE2(frameIn)); \
    XI_FRAME4D_SET_DIM2(frameOut, XI_FRAME4D_GET_DIM2(frameIn));             \
    XI_FRAME4D_SET_DIM2_PITCH(frameOut, XI_FRAME4D_GET_DIM2_PITCH(frameIn)); \
    XI_FRAME4D_SET_DIM2_EDGE1(frameOut, XI_FRAME4D_GET_DIM2_EDGE1(frameIn)); \
    XI_FRAME4D_SET_DIM2_EDGE2(frameOut, XI_FRAME4D_GET_DIM2_EDGE2(frameIn)); \
    XI_FRAME4D_SET_DIM3(frameOut, XI_FRAME4D_GET_DIM3(frameIn));             \
    XI_FRAME4D_SET_DIM3_PITCH(frameOut, XI_FRAME4D_GET_DIM3_PITCH(frameIn)); \
    XI_FRAME4D_SET_DIM3_EDGE1(frameOut, XI_FRAME4D_GET_DIM3_EDGE1(frameIn)); \
    XI_FRAME4D_SET_DIM3_EDGE2(frameOut, XI_FRAME4D_GET_DIM3_EDGE2(frameIn)); \
    XI_FRAME4D_SET_DIM4(frameOut, XI_FRAME4D_GET_DIM4(frameIn));             \
    XI_FRAME4D_SET_DATA_PTR(frameOut, XI_FRAME4D_GET_DATA_PTR(frameIn));     \
    XI_FRAME4D_SET_DATA_ORDER(frameOut, XI_FRAME4D_GET_DATA_ORDER(frameIn)); \
    XI_FRAME4D_SET_PIXEL_RES(frameOut, XI_FRAME4D_GET_PIXEL_RES(frameIn));   \
}

#define XI_COPY_TILE4D_TO_TILE4D(tileIn, tileOut) {                      \
    XI_TILE4D_SET_DIM1(tileOut, XI_TILE4D_GET_DIM1(tileIn));             \
    XI_TILE4D_SET_DIM1_PITCH(tileOut, XI_TILE4D_GET_DIM1_PITCH(tileIn)); \
    XI_TILE4D_SET_DIM1_EDGE1(tileOut, XI_TILE4D_GET_DIM1_EDGE1(tileIn)); \
    XI_TILE4D_SET_DIM1_EDGE2(tileOut, XI_TILE4D_GET_DIM1_EDGE2(tileIn)); \
    XI_TILE4D_SET_DIM2(tileOut, XI_TILE4D_GET_DIM2(tileIn));             \
    XI_TILE4D_SET_DIM2_PITCH(tileOut, XI_TILE4D_GET_DIM2_PITCH(tileIn)); \
    XI_TILE4D_SET_DIM2_EDGE1(tileOut, XI_TILE4D_GET_DIM2_EDGE1(tileIn)); \
    XI_TILE4D_SET_DIM2_EDGE2(tileOut, XI_TILE4D_GET_DIM2_EDGE2(tileIn)); \
    XI_TILE4D_SET_DIM3(tileOut, XI_TILE4D_GET_DIM3(tileIn));             \
    XI_TILE4D_SET_DIM3_PITCH(tileOut, XI_TILE4D_GET_DIM3_PITCH(tileIn)); \
    XI_TILE4D_SET_DIM3_EDGE1(tileOut, XI_TILE4D_GET_DIM3_EDGE1(tileIn)); \
    XI_TILE4D_SET_DIM3_EDGE2(tileOut, XI_TILE4D_GET_DIM3_EDGE2(tileIn)); \
    XI_TILE4D_SET_DIM4(tileOut, XI_TILE4D_GET_DIM4(tileIn));             \
    XI_TILE4D_SET_DATA_PTR(tileOut, XI_TILE4D_GET_DATA_PTR(tileIn));     \
    XI_TILE4D_SET_DATA_ORDER(tileOut, XI_TILE4D_GET_DATA_ORDER(tileIn)); \
}

// Assumes 8 bit pixRes and Edge1 = Edge2
#define XI_TILE4D_UPDATE_EDGE_DIM1(pTile, newEdgeSize)                     \
  {                                                                        \
    uint16_t currEdgeSize  = (uint16_t) XI_TILE4D_GET_DIM1_EDGE1(pTile);   \
    uint32_t dim1Pitch     = (uint32_t) XI_TILE4D_GET_DIM1_PITCH(pTile);   \
    uint32_t dataU32       = (uint32_t) XI_TILE4D_GET_DATA_PTR(pTile);     \
    dataU32 = dataU32 + newEdgeSize - currEdgeSize;                        \
    XI_TILE4D_SET_DATA_PTR(pTile, (void *) dataU32);                       \
    XI_TILE4D_SET_DIM1_EDGE1(pTile, newEdgeSize);                          \
    XI_TILE4D_SET_DIM1_EDGE2(pTile, newEdgeSize);                          \
    XI_TILE4D_SET_DIM1(pTile, dim1Pitch - 2 * newEdgeSize);                \
  }

// Assumes 8 bit pixRes and Edge1 = Edge2
#define XI_TILE4D_UPDATE_EDGE_DIM2(pTile, newEdgeSize)                     \
  {                                                                        \
    uint16_t currEdgeSize  = (uint16_t) XI_TILE4D_GET_DIM2_EDGE1(pTile);   \
    uint32_t dim1Pitch     = (uint32_t) XI_TILE4D_GET_DIM1_PITCH(pTile);   \
    uint16_t dim2Size      = (uint16_t) XI_TILE4D_GET_DIM2(pTile);         \
    uint32_t dataU32       = (uint32_t) XI_TILE4D_GET_DATA_PTR(pTile);     \
    dataU32 = dataU32 + dim1Pitch * (newEdgeSize - currEdgeSize);          \
    XI_TILE4D_SET_DATA_PTR(pTile, (void *) dataU32);                       \
    XI_TILE4D_SET_DIM2_EDGE1(pTile, newEdgeSize);                          \
    XI_TILE4D_SET_DIM2_EDGE2(pTile, newEdgeSize);                          \
    XI_TILE4D_SET_DIM2(pTile, dim2Size + 2 * (currEdgeSize-newEdgeSize));  \
  }

// Assumes 8 bit pixRes and Edge1 = Edge2
#define XI_TILE4D_UPDATE_EDGE_DIM3(pTile, newEdgeSize)                     \
  {                                                                        \
    uint16_t currEdgeSize  = (uint16_t) XI_TILE4D_GET_DIM3_EDGE1(pTile);   \
    uint32_t dim2Pitch     = (uint32_t) XI_TILE4D_GET_DIM2_PITCH(pTile);   \
    uint16_t dim3Size      = (uint16_t) XI_TILE4D_GET_DIM3(pTile);         \
    uint32_t dataU32       = (uint32_t) XI_TILE4D_GET_DATA_PTR(pTile);     \
    dataU32 = dataU32 + dim2Pitch * (newEdgeSize - currEdgeSize);          \
    XI_TILE4D_SET_DATA_PTR(pTile, (void *) dataU32);                       \
    XI_TILE4D_SET_DIM3_EDGE1(pTile, newEdgeSize);                          \
    XI_TILE4D_SET_DIM3_EDGE2(pTile, newEdgeSize);                          \
    XI_TILE4D_SET_DIM3(pTile, dim3Size + 2 * (currEdgeSize-newEdgeSize));  \
  }

#define XI_TILE4D_UPDATE_DIMENSIONS(pTile, dim1Loc, dim2Loc, dim3Loc, dim4Loc, dim1Size, dim2Size, dim3Size, dim4Size,  \
         dim1Pitch, dim2Pitch, dim3Pitch)                                                                               \
  {                                                                                                                     \
    XI_TILE4D_SET_DIM1_COORD(pTile, dim1Loc);                                                                           \
    XI_TILE4D_SET_DIM2_COORD(pTile, dim2Loc);                                                                           \
    XI_TILE4D_SET_DIM3_COORD(pTile, dim3Loc);                                                                           \
    XI_TILE4D_SET_DIM4_COORD(pTile, dim4Loc);                                                                           \
    XI_TILE4D_SET_DIM1(pTile, dim1Size);                                                                                \
    XI_TILE4D_SET_DIM2(pTile, dim2Size);                                                                                \
    XI_TILE4D_SET_DIM3(pTile, dim3Size);                                                                                \
    XI_TILE4D_SET_DIM4(pTile, dim4Size);                                                                                \
    XI_TILE4D_SET_DIM1_PITCH(pTile, dim1Pitch);                                                                         \
    XI_TILE4D_SET_DIM2_PITCH(pTile, dim2Pitch);                                                                         \
    XI_TILE4D_SET_DIM3_PITCH(pTile, dim3Pitch);                                                                         \
  }

#endif /*  */

#endif //if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))
