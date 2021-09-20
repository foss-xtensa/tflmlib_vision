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
#ifndef ANDROID_ML_NN_XTENSA_DRIVER_XTENSA_OP_H
#define ANDROID_ML_NN_XTENSA_DRIVER_XTENSA_OP_H

#include <stdint.h>

// Maximum number of input and output across all xtensa operations
//      ***    IF CHANGE - CHANGE XTENSA_ANN_VERSION below  ****
#define XTENSA_OPERATION_MAX_NUM_INPUTS 36
#define XTENSA_OPERATION_MAX_NUM_OUTPUTS 31

// Define the max size of parameters in bytes of each xtensa operation
#define XTENSA_OPERATION_MAX_PARAMS_SIZE 512

// Define max supported local mem banks on device
#define XTENSA_DEVICE_MAX_NUM_LOCAL_MEM_BANKS 2

// Define Xtensa ANN version
#define XTENSA_ANN_VERSION "XTENSA_ANN_FP16_512MAC_V7 "
#ifdef  __ANDROID__
#ifndef __ANDROID_API_Q__
#error  "Minimum Android Q API is required for this version of Xtensa ANN"
#endif
#endif

// Type of operation corresponding to the NN op to be executed on the target
// DSP.
enum XtensaOperationType {
    XFL_CONV_2D           = 0,
    XFL_DEPTHWISE_CONV_2D = 1,
    XFL_POOL_2D           = 2,
    XFL_LOGISTIC          = 3,
    XFL_RELU              = 4,
    XFL_SOFTMAX           = 5,
    XFL_CONCATENATION     = 6,
    XFL_ELEMENT_WISE_OP   = 7,
    XFL_L2NORM            = 8,
    XFL_LOOKUP            = 9,
    XFL_DEPTH_N_SPACE     = 10,
    XFL_OEM_OP            = 11,
    XFL_BATCH_TO_SPACE_ND = 12,
    XFL_MEAN              = 13,
    XFL_PAD               = 14,
    XFL_SPACE_TO_BATCH_ND = 15,
    XFL_SQUEEZE           = 16,
    XFL_STRIDED_SLICE     = 17,
    XFL_TRANSPOSE         = 18,
    XFL_TANH              = 19,
    XFL_PRELU             = 20,
    XFL_CHANNELSHUFFLE    = 21,
    XFL_GROUPED_CONV_2D   = 22,
    XFL_TRANSPOSE_CONV_2D = 23,
    XFL_QUANTIZED_16BIT_LSTM = 24,
    XFL_ARGMIN            = 25,
    XFL_ARGMAX            = 26,
    XFL_TOPK_V2           = 27,
    XFL_GATHER            = 28,
    XFL_CAST              = 29,
    XFL_HEATMAP_MAX_KEYPOINT = 30,
    XFL_REDUCE              = 31,
    XFL_GENERATE_PROPOSALS = 32,
    XFL_SELECT            = 33,
    XFL_BBOX_TRANSFORM    = 34,
    XFL_BOX_WITH_NMS_LIMIT   = 35,
    XFL_TILE              = 36,
    XFL_SPLIT             = 37,
    XFL_LOGSOFTMAX        = 38,
    XFL_DETECTION_PP      = 39,
    XFL_QUANTIZE          = 40,
    XFL_ROIALIGN          = 41,
    XFL_DEQUANTIZE        = 42,
    XFL_RESIZE_NEAREST_NEIGHBOR   = 43,
    XFL_RESIZE_BILINEAR   = 44,
    XFL_TENSORARITHMETIC_OP = 45,
    XFL_GREATER           = 46,
    XFL_GREATER_EQUAL     = 47,
    XFL_LESS              = 48,
    XFL_LESS_EQUAL        = 49,
    XFL_EQUAL             = 50,
    XFL_NOT_EQUAL         = 51,
    XFL_HARDSWISH         = 52,
    XFL_QUANTIZED_LSTM = 53,
    // All Android NN ops should come before this
    XFL_NUM_OPS,
    // Internal operations
    XFL_LOCAL_MEM_INFO = XFL_NUM_OPS,
    XFL_XTENSA_ANN_VERSION,
    XFL_MCHEAD,
    XFL_NUM_ALL_OPS,
};

enum XtensaOptControl {
    XTENSA_OPT_NONE = 0,
    XTENSA_OPT_SKIP,
    XTENSA_OPT_DELETE,
    XTENSA_OPT_DELETE_NEXT,
    XTENSA_OPT_FAILED,
};

typedef struct PACKED {
    /* Size of this structure (for consistency checking). */
    int32_t  structSize;
    int32_t totalCores;
    int32_t myCore;
} MCHead_params_t;

// Defines an operation parameters space for kernelSetup
struct XtensaOperation {
    enum XtensaOperationType opType;
    uint8_t params[XTENSA_OPERATION_MAX_PARAMS_SIZE];
};

// Defines an operation in the XRP command buffer
struct XtensaVisionOperation {
    struct XtensaOperation xtensaOp;
    uint16_t inputIndexes[XTENSA_OPERATION_MAX_NUM_INPUTS];
    uint16_t outputIndexes[XTENSA_OPERATION_MAX_NUM_OUTPUTS];
};

#if 0 //defined(__ANDROID__) || DEBUG_LEVEL > 0 || CNNRT_PERF_LEVEL == CNNRT_PERF_LEVEL_ALL
// Note, the order of the names should match the XtensaOperationType enum
static const char* XtensaOperationName[] __attribute__ ((unused)) = {
    "XFL_CONV_2D",
    "XFL_DEPTHWISE_CONV_2D",
    "XFL_POOL_2D",
    "XFL_LOGISTIC",
    "XFL_RELU",
    "XFL_SOFTMAX",
    "XFL_CONCATENATION",
    "XFL_ELEMENT_WISE_OP",
    "XFL_L2NORM",
    "XFL_LOOKUP",
    "XFL_DEPTH_N_SPACE",
    "XFL_OEM_OP",
    "XFL_BATCH_TO_SPACE_ND",
    "XFL_MEAN",
    "XFL_PAD",
    "XFL_SPACE_TO_BATCH_ND",
    "XFL_SQUEEZE",
    "XFL_STRIDED_SLICE",
    "XFL_TRANSPOSE",
    "XFL_TANH",
    "XFL_PRELU",
    "XFL_CHANNELSHUFFLE",
    "XFL_GROUPED_CONV_2D",
    "XFL_TRANSPOSE_CONV_2D",
    "XFL_QUANTIZED_16BIT_LSTM",
    "XFL_ARGMIN",
    "XFL_ARGMAX",
    "XFL_TOPK_V2",
    "XFL_GATHER",
    "XFL_CAST",
    "XFL_HEATMAP_MAX_KEYPOINT",
    "XFL_REDUCE",
    "XFL_GENERATE_PROPOSALS",
    "XFL_SELECT",
    "XFL_BBOX_TRANSFORM",
    "XFL_BOX_WITH_NMS_LIMIT",
    "XFL_TILE",
    "XFL_SPLIT",
    "XFL_LOGSOFTMAX",
    "XFL_DETECTION_PP",
    "XFL_QUANTIZE",
    "XFL_ROIALIGN",
    "XFL_DEQUANTIZE",
    "XFL_RESIZE_NEAREST_NEIGHBOR",
    "XFL_RESIZE_BILINEAR",
    "XFL_TENSORARITHMETIC_OP",
    "XFL_GREATER",
    "XFL_GREATER_EQUAL",
    "XFL_LESS",
    "XFL_LESS_EQUAL",
    "XFL_EQUAL",
    "XFL_NOT_EQUAL",
    "XFL_HARDSWISH",
	"XFL_QUANTIZED_LSTM",
    // All Android NN ops should come before this
    "XFL_LOCAL_MEM_INFO",
    "XFL_XTENSA_ANN_VERSION",
    "XFL_MCHEAD",
};
#endif

#if defined(__ANDROID__)
static const char* XtensaOptName[] = {
    "XTENSA_OPT_NONE",
    "XTENSA_OPT_SKIP",
    "XTENSA_OPT_DELETE",
    "XTENSA_OPT_DELETE_NEXT",
    "XTENSA_OPT_FAILED",
};
#endif

// Defines local memories on the DSP
struct XtensaDeviceLocalMem {
    uint32_t numBanks;
    uint32_t areBanksContiguous;
    uint32_t freeSpace[XTENSA_DEVICE_MAX_NUM_LOCAL_MEM_BANKS];
    uint32_t dna_mblk;
    uint32_t dna_ubuf_size;
};

#endif // ANDROID_ML_NN_XTENSA_DRIVER_XTENSA_OP_H
