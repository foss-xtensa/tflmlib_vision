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
#ifndef __XI_TILE_MANAGER_H__
#define __XI_TILE_MANAGER_H__

#include <stdint.h>

// Define the flag XI_XV_TILE_COMPATIBILITY if we want to use the tile manager
// for handling the DMA transfers. In this case the xi_tile and xi_frame structures
// must be expaded to reflect the extra structre elements added to xvFrame and
// xvTile structures.

#ifdef XI_XV_TILE_COMPATIBILITY
#include "tileManager.h"
#endif

typedef struct xiFrameStruct
{
  void     *pFrameBuff;
  uint32_t frameBuffSize;
  void     *pFrameData;
  int32_t  frameWidth;
  int32_t  frameHeight;
  int32_t  framePitch;
  uint8_t  pixelRes;
  uint8_t  pixelPackFormat;
#ifdef XI_XV_TILE_COMPATIBILITY
  uint8_t  reserved1;
  uint8_t  reserved2;
  uint8_t  reserved3;
  uint8_t  reserved4;
  uint8_t  reserved5;
  uint8_t  reserved6;
#endif
} xi_frame, *xi_pFrame;

#define XI_ARRAY_FIELDS \
  void     *pBuffer;    \
  uint32_t bufferSize;  \
  void *pData;          \
  int32_t width;        \
  int32_t pitch;        \
  uint32_t status;      \
  uint16_t type;        \
  int32_t  height;


typedef struct xiArrayStruct
{
  XI_ARRAY_FIELDS
} xi_array, *xi_pArray;


typedef struct xiTileStruct
{
  XI_ARRAY_FIELDS
  xi_frame *pFrame;
  int32_t  x;
  int32_t  y;
  uint16_t edgeWidth;
  uint16_t edgeHeight;
#ifdef XI_XV_TILE_COMPATIBILITY
  uint16_t reserved1;
  uint16_t reserved2;
  int32_t  reserved3;
  int32_t  reserved4;
  int32_t  reserved5;
#endif
} xi_tile, *xi_pTile;


/*****************************************
*   Data type definitions
*****************************************/

#define XI_TYPE_SIGNED_BIT         (1 << 15)
#define XI_TYPE_TILE_BIT           (1 << 14)

#define XI_TYPE_FLOAT_BIT          (1 << 11)

#define XI_TYPE_ELEMENT_SIZE_BITS  9
#define XI_TYPE_ELEMENT_SIZE_MASK  ((1 << XI_TYPE_ELEMENT_SIZE_BITS) - 1)
#define XI_TYPE_CHANNELS_BITS      2
#define XI_TYPE_CHANNELS_MASK      (((1 << XI_TYPE_CHANNELS_BITS) - 1) << XI_TYPE_ELEMENT_SIZE_BITS)

#define XI_MAKETYPE(flags, depth, channels)  (((depth) * (channels)) | (((channels) - 1) << XI_TYPE_ELEMENT_SIZE_BITS) | (flags))
#define XI_CUSTOMTYPE(type)                  XI_MAKETYPE(0, sizeof(type), 1)

#define XI_TYPE_ELEMENT_SIZE(type)           ((type) & (XI_TYPE_ELEMENT_SIZE_MASK))
#define XI_TYPE_ELEMENT_TYPE(type)           ((type) & (XI_TYPE_SIGNED_BIT | XI_TYPE_CHANNELS_MASK | XI_TYPE_ELEMENT_SIZE_MASK | XI_TYPE_FLOAT_BIT))
#define XI_TYPE_IS_TILE(type)                ((type) & (XI_TYPE_TILE_BIT))
#define XI_TYPE_IS_SIGNED(type)              ((type) & (XI_TYPE_SIGNED_BIT))
#define XI_TYPE_CHANNELS(type)               ((((type) & (XI_TYPE_CHANNELS_MASK)) >> (XI_TYPE_ELEMENT_SIZE_BITS)) + 1)

// XV_MAKETYPE accepts 3 parameters
// 1: Denotes whether the entity is a tile (XV_TYPE_TILE_BIT is set) or an array (XV_TYPE_TILE_BIT is unset),
//    and also if the data is a signed or unsigned.
// 2: Denotes number of bytes per pel.
//    1 implies the data is 8bit, 2 implies the data is 16bit and 4 implies the data is 32bit.
// 3: Denotes number of channels.
//    1 implies gray scale, 3 implies RGB,...

#define XI_U8         XI_MAKETYPE(0, 1, 1)
#define XI_U16        XI_MAKETYPE(0, 2, 1)
#define XI_U32        XI_MAKETYPE(0, 4, 1)
#define XI_U64        XI_MAKETYPE(0, 8, 1)

#define XI_S8         XI_MAKETYPE(XI_TYPE_SIGNED_BIT, 1, 1)
#define XI_S16        XI_MAKETYPE(XI_TYPE_SIGNED_BIT, 2, 1)
#define XI_S32        XI_MAKETYPE(XI_TYPE_SIGNED_BIT, 4, 1)
#define XI_S64        XI_MAKETYPE(XI_TYPE_SIGNED_BIT, 8, 1)

#define XI_F16        (XI_MAKETYPE(XI_TYPE_SIGNED_BIT | XI_TYPE_FLOAT_BIT, 2, 1))
#define XI_F32        (XI_MAKETYPE(XI_TYPE_SIGNED_BIT | XI_TYPE_FLOAT_BIT, 4, 1))

#define XI_ARRAY_U8   XI_U8
#define XI_ARRAY_U16  XI_U16
#define XI_ARRAY_U32  XI_U32
#define XI_ARRAY_U64  XI_U64

#define XI_ARRAY_S8   XI_S8
#define XI_ARRAY_S16  XI_S16
#define XI_ARRAY_S32  XI_S32
#define XI_ARRAY_S64  XI_S64

#define XI_ARRAY_F16  XI_F16
#define XI_ARRAY_F32  XI_F32

#define XI_TILE_U8    (XI_U8 | XI_TYPE_TILE_BIT)
#define XI_TILE_U16   (XI_U16 | XI_TYPE_TILE_BIT)
#define XI_TILE_U32   (XI_U32 | XI_TYPE_TILE_BIT)
#define XI_TILE_U64   (XI_U64 | XI_TYPE_TILE_BIT)

#define XI_TILE_S8    (XI_S8 | XI_TYPE_TILE_BIT)
#define XI_TILE_S16   (XI_S16 | XI_TYPE_TILE_BIT)
#define XI_TILE_S32   (XI_S32 | XI_TYPE_TILE_BIT)
#define XI_TILE_S64   (XI_S64 | XI_TYPE_TILE_BIT)

#define XI_TILE_F16   (XI_F16 | XI_TYPE_TILE_BIT)
#define XI_TILE_F32   (XI_F32 | XI_TYPE_TILE_BIT)


/*****************************************
*                   Frame Access Macros
*****************************************/

#define XI_FRAME_GET_BUFF_PTR(pFrame)                   ((pFrame)->pFrameBuff)
#define XI_FRAME_SET_BUFF_PTR(pFrame, pBuff)            (pFrame)->pFrameBuff = ((void *) (pBuff))

#define XI_FRAME_GET_BUFF_SIZE(pFrame)                  ((pFrame)->frameBuffSize)
#define XI_FRAME_SET_BUFF_SIZE(pFrame, buffSize)        (pFrame)->frameBuffSize = ((uint32_t) (buffSize))

#define XI_FRAME_GET_DATA_PTR(pFrame)                   ((pFrame)->pFrameData)
#define XI_FRAME_SET_DATA_PTR(pFrame, pData)            (pFrame)->pFrameData = ((void *) (pData))

#define XI_FRAME_GET_WIDTH(pFrame)                      ((pFrame)->frameWidth)
#define XI_FRAME_SET_WIDTH(pFrame, width)               (pFrame)->frameWidth = ((int32_t) (width))

#define XI_FRAME_GET_HEIGHT(pFrame)                     ((pFrame)->frameHeight)
#define XI_FRAME_SET_HEIGHT(pFrame, height)             (pFrame)->frameHeight = ((int32_t) (height))

#define XI_FRAME_GET_PITCH(pFrame)                      ((pFrame)->framePitch)
#define XI_FRAME_SET_PITCH(pFrame, pitch)               (pFrame)->framePitch = ((int32_t) (pitch))

#define XI_FRAME_GET_PIXEL_RES(pFrame)                  ((pFrame)->pixelRes)
#define XI_FRAME_SET_PIXEL_RES(pFrame, pixRes)          (pFrame)->pixelRes = ((uint8_t) (pixRes))

#define XI_FRAME_GET_PIXEL_FORMAT(pFrame)               ((pFrame)->pixelPackFormat)
#define XI_FRAME_SET_PIXEL_FORMAT(pFrame, pixelFormat)  (pFrame)->pixelPackFormat = ((uint8_t) (pixelFormat))


/*****************************************
*                   Array Access Macros
*****************************************/

#define XI_ARRAY_GET_BUFF_PTR(pArray)              ((pArray)->pBuffer)
#define XI_ARRAY_SET_BUFF_PTR(pArray, pBuff)       (pArray)->pBuffer = ((void *) (pBuff))

#define XI_ARRAY_GET_BUFF_SIZE(pArray)             ((pArray)->bufferSize)
#define XI_ARRAY_SET_BUFF_SIZE(pArray, buffSize)   (pArray)->bufferSize = ((uint32_t) (buffSize))

#define XI_ARRAY_GET_DATA_PTR(pArray)              ((pArray)->pData)
#define XI_ARRAY_SET_DATA_PTR(pArray, pArrayData)  (pArray)->pData = ((void *) (pArrayData))

#define XI_ARRAY_GET_WIDTH(pArray)                 ((pArray)->width)
#define XI_ARRAY_SET_WIDTH(pArray, value)          (pArray)->width = ((int32_t) (value))

#define XI_ARRAY_GET_PITCH(pArray)                 ((pArray)->pitch)
#define XI_ARRAY_SET_PITCH(pArray, value)          (pArray)->pitch = ((int32_t) (value))

#define XI_ARRAY_GET_HEIGHT(pArray)                ((pArray)->height)
#define XI_ARRAY_SET_HEIGHT(pArray, value)         (pArray)->height = ((uint16_t) (value))

#define XI_ARRAY_GET_STATUS_FLAGS(pArray)          ((pArray)->status)
#define XI_ARRAY_SET_STATUS_FLAGS(pArray, value)   (pArray)->status = ((uint8_t) (value))

#define XI_ARRAY_GET_TYPE(pArray)                  ((pArray)->type)
#define XI_ARRAY_SET_TYPE(pArray, value)           (pArray)->type = ((uint16_t) (value))

#define XI_ARRAY_GET_CAPACITY(pArray)              ((pArray)->pitch)
#define XI_ARRAY_SET_CAPACITY(pArray, value)       (pArray)->pitch = ((int32_t) (value))

#define XI_ARRAY_GET_ELEMENT_TYPE(pArray)          (XI_TYPE_ELEMENT_TYPE(XI_ARRAY_GET_TYPE(pArray)))
#define XI_ARRAY_GET_ELEMENT_SIZE(pArray)          ((XI_ARRAY_GET_TYPE(pArray)) & (XI_TYPE_ELEMENT_SIZE_MASK))
#define XI_ARRAY_IS_TILE(pArray)                   ((XI_ARRAY_GET_TYPE(pArray)) & (XI_TYPE_TILE_BIT))

#define XI_ARRAY_GET_AREA(pArray)                  (((pArray)->width) * ((int32_t) (pArray)->height))

/*****************************************
*                   Tile Access Macros
*****************************************/

#define XI_TILE_GET_BUFF_PTR      XI_ARRAY_GET_BUFF_PTR
#define XI_TILE_SET_BUFF_PTR      XI_ARRAY_SET_BUFF_PTR

#define XI_TILE_GET_BUFF_SIZE     XI_ARRAY_GET_BUFF_SIZE
#define XI_TILE_SET_BUFF_SIZE     XI_ARRAY_SET_BUFF_SIZE

#define XI_TILE_GET_DATA_PTR      XI_ARRAY_GET_DATA_PTR
#define XI_TILE_SET_DATA_PTR      XI_ARRAY_SET_DATA_PTR

#define XI_TILE_GET_WIDTH         XI_ARRAY_GET_WIDTH
#define XI_TILE_SET_WIDTH         XI_ARRAY_SET_WIDTH

#define XI_TILE_GET_PITCH         XI_ARRAY_GET_PITCH
#define XI_TILE_SET_PITCH         XI_ARRAY_SET_PITCH

#define XI_TILE_GET_HEIGHT        XI_ARRAY_GET_HEIGHT
#define XI_TILE_SET_HEIGHT        XI_ARRAY_SET_HEIGHT

#define XI_TILE_GET_STATUS_FLAGS  XI_ARRAY_GET_STATUS_FLAGS
#define XI_TILE_SET_STATUS_FLAGS  XI_ARRAY_SET_STATUS_FLAGS

#define XI_TILE_GET_TYPE          XI_ARRAY_GET_TYPE
#define XI_TILE_SET_TYPE          XI_ARRAY_SET_TYPE

#define XI_TILE_GET_ELEMENT_TYPE  XI_ARRAY_GET_ELEMENT_TYPE
#define XI_TILE_GET_ELEMENT_SIZE  XI_ARRAY_GET_ELEMENT_SIZE
#define XI_TILE_IS_TILE           XI_ARRAY_IS_TILE

#define XI_TILE_GET_FRAME_PTR(pTile)            ((pTile)->pFrame)
#define XI_TILE_SET_FRAME_PTR(pTile, ptrFrame)  (pTile)->pFrame = ((xi_frame *) (ptrFrame))

#define XI_TILE_GET_X_COORD(pTile)              ((pTile)->x)
#define XI_TILE_SET_X_COORD(pTile, xcoord)      (pTile)->x = ((int32_t) (xcoord))

#define XI_TILE_GET_Y_COORD(pTile)              ((pTile)->y)
#define XI_TILE_SET_Y_COORD(pTile, ycoord)      (pTile)->y = ((int32_t) (ycoord))

#define XI_TILE_GET_EDGE_WIDTH(pTile)           ((pTile)->edgeWidth)
#ifdef XI_XV_TILE_COMPATIBILITY
#define XI_TILE_SET_EDGE_WIDTH(pTile, eWidth)   XV_TILE_SET_EDGE_WIDTH((xvpTile) pTile, eWidth)
#else
#define XI_TILE_SET_EDGE_WIDTH(pTile, eWidth)   ((pTile)->edgeWidth = (uint16_t) eWidth)
#endif

#define XI_TILE_GET_EDGE_HEIGHT(pTile)           ((pTile)->edgeHeight)
#ifdef XI_XV_TILE_COMPATIBILITY
#define XI_TILE_SET_EDGE_HEIGHT(pTile, eHeight)  XV_TILE_SET_EDGE_HEIGHT((xvpTile) pTile, eHeight)
#else
#define XI_TILE_SET_EDGE_HEIGHT(pTile, eHeight)  ((pTile)->edgeHeight = (uint16_t) eHeight)
#endif

/***********************************
*              Other Marcos
***********************************/

#define XI_TILE_CHECK_VIRTUAL_FRAME(pTile)    ((pTile)->pFrame->pFrameBuff == NULL)
#define XI_FRAME_CHECK_VIRTUAL_FRAME(pFrame)  ((pFrame)->pFrameBuff == NULL)

#ifndef XI_XV_TILE_COMPATIBILITY

typedef enum
{
  TILE_UNALIGNED,
  EDGE_ALIGNED_32,
  DATA_ALIGNED_32,
  EDGE_ALIGNED_64,
  DATA_ALIGNED_64,
} buffer_align_type_t;

// If type is zero, bytes per pixel is eight bits and number of channels is one
#define SETUP_TILE(pTile, pBuf, bufSize, pFrame, width, height, pitch, type, edgeWidth, edgeHeight, x, y, alignType) \
  {                                                                                                                  \
    int32_t tileType, bytesPerPixel, channels, bytesPerPel;                                                          \
    uint8_t *edgePtr = (uint8_t *) pBuf, *dataPtr;                                                                   \
    int alignment    = 63;                                                                                           \
    if ((alignType == EDGE_ALIGNED_32) || (alignType == DATA_ALIGNED_32)) { alignment = 31; }                        \
    tileType      = (type) ? (type) : XI_U8;                                                                         \
    bytesPerPixel = XI_TYPE_ELEMENT_SIZE(tileType);                                                                  \
    channels      = XI_TYPE_CHANNELS(tileType);                                                                      \
    bytesPerPel   = bytesPerPixel / channels;                                                                        \
    XI_TILE_SET_FRAME_PTR(pTile, (pFrame));                                                                          \
    XI_TILE_SET_BUFF_PTR(pTile, (pBuf));                                                                             \
    XI_TILE_SET_BUFF_SIZE(pTile, (bufSize));                                                                         \
    if ((alignType == EDGE_ALIGNED_32) || (alignType == EDGE_ALIGNED_64))                                            \
    {                                                                                                                \
      edgePtr = (uint8_t *) (((long) (pBuf) + alignment) & (~alignment));                                            \
    }                                                                                                                \
    XI_TILE_SET_DATA_PTR(pTile, edgePtr + ((edgeHeight) * (pitch) * bytesPerPel + (edgeWidth) * bytesPerPixel));     \
    if ((alignType == DATA_ALIGNED_32) || (alignType == DATA_ALIGNED_64))                                            \
    {                                                                                                                \
      dataPtr = (uint8_t *) XI_TILE_GET_DATA_PTR(pTile);                                                             \
      dataPtr = (uint8_t *) (((long) (dataPtr) + alignment) & (~alignment));                                         \
      XI_TILE_SET_DATA_PTR(pTile, dataPtr);                                                                          \
    }                                                                                                                \
    XI_TILE_SET_WIDTH(pTile, (width));                                                                               \
    XI_TILE_SET_HEIGHT(pTile, (height));                                                                             \
    XI_TILE_SET_PITCH(pTile, (pitch));                                                                               \
    XI_TILE_SET_TYPE(pTile, (tileType | XI_TYPE_TILE_BIT));                                                          \
    XI_TILE_SET_EDGE_WIDTH(pTile, (edgeWidth));                                                                      \
    XI_TILE_SET_EDGE_HEIGHT(pTile, (edgeHeight));                                                                    \
    XI_TILE_SET_X_COORD(pTile, (x));                                                                                 \
    XI_TILE_SET_Y_COORD(pTile, (y));                                                                                 \
    XI_TILE_SET_STATUS_FLAGS(pTile, 0);                                                                              \
  }

#endif

#endif
