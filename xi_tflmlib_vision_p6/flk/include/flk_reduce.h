/*
 * Copyright (c) 2018 by Cadence Design Systems, Inc. ALL RIGHTS RESERVED. 
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of 
 * Cadence Design Systems Inc. Any rights to use, modify, and create 
 * derivative works of this file are set forth under the terms of your 
 * license agreement with Cadence Design Systems, Inc.
 */
 
#ifndef _FLK_REDUCE_H_INCLUDED_
#define _FLK_REDUCE_H_INCLUDED_

#include <flk_common.h>

#define XFL_REDUCE_MIN   1
#define XFL_REDUCE_MAX   2
#define XFL_REDUCE_ANY   3
#define XFL_REDUCE_ALL   4
#define XFL_REDUCE_MEAN  5
#define XFL_REDUCE_PROD  6
#define XFL_REDUCE_SUM   7

typedef struct PACKED {
    uint32_t structSize;
    /* Input dimensions. Layout is DWHB wih D is the fastest dimension. */
    dim4d_t input;
    dim4d_t output;
    /* Output tile dimensions. */
    dim4d_t tile;
    dim4d_t outTile;
    // Bank Index
    uint8_t bankInd;
    // Memory and tile info
    uint32_t tileSize;
    uint32_t outTileSize;
    uint32_t intermediateArrSize;
    uint32_t memSize;
    // params
    struct axis {
        uint8_t D;
        uint8_t W;
        uint8_t H;
        uint8_t N;
    } axis;
    uint8_t config;
    uint8_t type;
    uint8_t translationAxis;
    uint8_t combinedTile;
    uint8_t eleSize;
    DataType_t dataType;
    uint8_t doRequant;
    /* re-quantization parameters */
    uint8_t zero_pointInp;
    uint8_t zero_pointOut;
    int32_t multiplierOut;
    uint8_t shiftOut;
    uint8_t left_shift;
    uint8_t ScaleFlag;
} reduce_params_t;

typedef int32_t XI_ERR_TYPE;

XI_ERR_TYPE flk_reduce4D(const uint8_t* raw_params,
  struct XtensaOperationArgsIn* input,
  struct XtensaOperationArgsOut* output);

bool reduceTileSetup(reduce_params_t* params, const size_t memSize);
bool reduce4DTileSetup(reduce_params_t* params, const size_t memSize);
bool reduce4DTileSetupF16(reduce_params_t* params, const size_t memSize);


#endif /* _FLK_REDUCE_H_INCLUDED_ */
