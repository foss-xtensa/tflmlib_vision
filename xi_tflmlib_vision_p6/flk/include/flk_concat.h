/*
 * Copyright (c) 2018 by Cadence Design Systems, Inc. ALL RIGHTS RESERVED. 
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of 
 * Cadence Design Systems Inc. Any rights to use, modify, and create 
 * derivative works of this file are set forth under the terms of your 
 * license agreement with Cadence Design Systems, Inc.
 */
 
#ifndef _FLK_CONCAT_H_INCLUDED_
#define _FLK_CONCAT_H_INCLUDED_

#include <flk_common.h>
#include <xi_tile_manager.h>
#include <xi_tile3d_manager.h>


#define MAX_FLK_CONCAT_INPUTS   2

typedef struct PACKED {
    /* Size of this structure (for consistency checking). */
    int32_t  structSize;
    /* Input dimensions, array of 4d dimensions,
       fastest dimension goes first in the array of four.*/
    uint32_t input[MAX_FLK_CONCAT_INPUTS][4];
    /* Output padding at the beginning of each dimension (except in axis dimension) */
    uint16_t output_pad_low[4];
    /* Output padding at the end of each dimension (except in axis dimension) */
    uint16_t output_pad_high[4];
    /* Padding value */
    uint32_t padValue;
    /* Number of inputs */
    uint8_t numInputs;
    /* Dimension index along which to concatenate */
    uint8_t  axis;
    // input output sclaes
    uint8_t reQuantFlags[MAX_FLK_CONCAT_INPUTS];
    int32_t zeroPtInput[MAX_FLK_CONCAT_INPUTS];
    int32_t zeroPtOutput;
    int32_t multiplierInp[MAX_FLK_CONCAT_INPUTS];
    int32_t shiftInp[MAX_FLK_CONCAT_INPUTS];
    int32_t left_shift;
    uint8_t Min, Max;
    uint8_t ScaleFlag[MAX_FLK_CONCAT_INPUTS];   // if 1,take scale > 1 path  //vinayak
    xi_dataType quantTensorSign;
    xi_dataType tensorType;
    uint8_t doRequant;
    uint8_t elemSize;
} concat_params_t;

typedef int32_t XI_ERR_TYPE;

XI_ERR_TYPE flk_concat(const uint8_t *raw_params,
                       struct XtensaOperationArgsIn  *input,
                       struct XtensaOperationArgsOut *output);

#endif /* _FLK_CONCAT_H_INCLUDED_ */
