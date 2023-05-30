/*
 * Copyright (c) 2018 by Cadence Design Systems, Inc. ALL RIGHTS RESERVED. 
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of 
 * Cadence Design Systems Inc. Any rights to use, modify, and create 
 * derivative works of this file are set forth under the terms of your 
 * license agreement with Cadence Design Systems, Inc.
 */
 
#ifndef _FLK_ELTWISE_OP_H_INCLUDED_
#define _FLK_ELTWISE_OP_H_INCLUDED_

#include <flk_common.h>
#include <xi_tile_manager.h>
#include <xi_tile3d_manager.h>

#define ELEMENT_WISE_MUL 0
#define ELEMENT_WISE_ADD 1
//#define ELEMENT_WISE_MAX 2
#define ELEMENT_WISE_SUB 3
#define ELEMENT_WISE_MAXIMUM    4
#define ELEMENT_WISE_MINIMUM    5
#define ELEMENT_WISE_MUL_FP16 6
#define ELEMENT_WISE_ADD_FP16 7
#define ELEMENT_WISE_DIV_FP16 8
#define ELEMENT_WISE_EXP_FP16 9
#define ELEMENT_WISE_POW_FP16 10

#define ACTIVATION_RELU_NONE 0
#define ACTIVATION_RELU_0    1
#define ACTIVATION_RELU_1    2    
#define ACTIVATION_RELU_6    3

typedef struct PACKED {
    // Size of this structure (for consistency checking). 
    int32_t  structSize;
    // Input dimensions 
    dims3d_t inputA;
    dims3d_t inputB;
    // Output dimensions 
    dims3d_t output;
    dims3d_t outpTile;
    // Batch size for both input and output. 
    uint32_t batchA, batchB;
    uint32_t batch;
    // Memory and tile info
    uint32_t memTileSize;
    // Memory bank Index
    uint8_t bankInd;
    // Type of operations : ADD / MUL
    uint8_t type;
    uint8_t elemSize;
    uint8_t activation;
    // Q8 specific values
    // Zero point
    int32_t zeroPtInputA;
    int32_t zeroPtInputB;
    int32_t zeroPtOutput;
    /* Multiply parameters */
    int32_t multiplierInpA;
    int32_t multiplierInpB;
    int32_t multiplierOut;
    int32_t shiftInpA;
    int32_t shiftInpB;
    int32_t shiftOut;
    int32_t minVal;
    int32_t maxVal;
    float minVal_f32;
    float maxVal_f32;
    int32_t left_shift;
    // Parameter to capture what type of quantized tensor. Field value will be 1 for
    // ASYMM_SIGNED or 0 for ASYMM
    xi_dataType tensorType;
    int32_t qSkipFlag;
} eltwiseOp_params_t;

typedef struct PACKED {
    // Size of this structure (for consistency checking). 
    int32_t  structSize;
    // Input dimensions 
    dims3d_t inputA;
    dims3d_t inputB;
    // Output dimensions 
    dims3d_t output;
    dims3d_t outpTile;
    // Batch size for both input and output. 
    uint32_t batchA, batchB;
    uint32_t batch;
    // Memory and tile info
    uint32_t memTileSize;
    // Memory bank Index
    uint8_t bankInd;
    // Type of operations : ADD / MUL
    uint8_t type;
    uint8_t elemSize;
    uint8_t activation;
    // FP16 specific values
    float minVal;
    float maxVal;
} eltwiseOp_fp16_params_t;

typedef int32_t XI_ERR_TYPE;

XI_ERR_TYPE flk_eltwiseOp(const uint8_t* raw_params,
    struct XtensaOperationArgsIn* input,
    struct XtensaOperationArgsOut* output);

#if defined(__cplusplus)
    // Kernel setup function
    bool eltwiseOpTileSetup(eltwiseOp_params_t *params, uint32_t bankLarge, uint32_t bankSmall, int32_t coreCount);
    bool eltwiseOpf16TileSetup(eltwiseOp_fp16_params_t *params, uint32_t bankLarge, uint32_t bankSmall);

#endif /* __cplusplus__ */

#endif /* _FLK_ELTWISE_OP_H_INCLUDED_ */


