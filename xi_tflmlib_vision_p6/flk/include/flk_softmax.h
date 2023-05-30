/*
 * Copyright (c) 2018 by Cadence Design Systems, Inc. ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of
 * Cadence Design Systems Inc. Any rights to use, modify, and create
 * derivative works of this file are set forth under the terms of your
 * license agreement with Cadence Design Systems, Inc.
 */

#ifndef _FLK_SOFTMAX_H_INCLUDED_
#define _FLK_SOFTMAX_H_INCLUDED_

#include <flk_common.h>
#include <xi_tile_manager.h>
#include <xi_tile3d_manager.h>

typedef struct PACKED
{
  /* Size of this structure (for consistency checking). */
  int32_t  structSize;
  /* Input dimensions */
  dims3d_t input;
  /* Output dimensions */
  /* Batch size for both input and output. */
  uint32_t batch;
  dims3d_t outTile;
  // Memory and tile info
  uint8_t  doubleBuffer;
  uint32_t memTileSize;
  // Memory bank
  uint8_t  bankInd;
  /* Activation parameters */
  int32_t  diff_min;
  int32_t  input_beta_multiplier;
  int32_t  input_beta_left_shift;
  int32_t  axis;
  float    beta;
  int32_t  elemSize;
  //Field to store the quantized tensor sign
  xi_dataType tensorType;
  uint32_t intermediateArrSize;
} softmax_params_t;

typedef int32_t XI_ERR_TYPE;

XI_ERR_TYPE flk_softmax(const uint8_t* raw_params,
    struct XtensaOperationArgsIn* input,
    struct XtensaOperationArgsOut* output);

#if defined(__cplusplus)

// Kernel setup function
bool softmaxTileSetup(softmax_params_t *params, uint32_t bankSize, int32_t coreCount);

#endif /* __cplusplus__ */
#endif /* _FLK_SOFTMAX_H_INCLUDED_ */
