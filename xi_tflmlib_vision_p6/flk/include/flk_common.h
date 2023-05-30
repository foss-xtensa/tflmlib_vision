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
#ifndef _FLK_COMMON_H_INCLUDED_
#define _FLK_COMMON_H_INCLUDED_

#include <stddef.h>
#include <stdint.h>

#include "XtensaOp.h"

/* Packing attribute for shared DSP<->Host data structures. */
#ifndef PACKED
#if defined(__XTENSA__) || defined(__GNUC__)
#define PACKED     __attribute__((packed))
#else
#define PACKED
#endif
#endif

/* Vision vector width in 16-bit elements. */
#define IVPN_SIMD_WIDTH 32

/* Alignment required by DSP */
#define ALIGNMENT    (2*IVPN_SIMD_WIDTH)

/* Maximum number of arrays in device local memory: sum of MAX INPUT and OUTPUT operands */
#define XTENSA_OPERATION_MAX_NUM_OPERANDS    (XTENSA_OPERATION_MAX_NUM_INPUTS + XTENSA_OPERATION_MAX_NUM_OUTPUTS)

typedef struct PACKED {
    uint32_t D;
    uint32_t W;
    uint32_t H;
} dims3d_t;

typedef struct PACKED {
    uint32_t D;
    uint32_t W;
    uint32_t H;
    uint32_t N;
} dim4d_t;


/*  Device banks layout */
typedef struct PACKED {
    /* Local memory bank mode initialization, single:0/contiguous:1/split:2 */
    uint32_t banksMode;
    /* Local memory banks number - 1 or 2 */
    uint32_t banksNumber;
    /* Local memory banks available sizes */
    uint32_t bankSize[2];
} localMem_info_t;

/*  Banks allocation layout */
typedef struct PACKED {
    /* Local memory bank mode initialization, single:0/contiguous:1/split:2 */
    uint32_t banksMode;
    /* Local memory bank assignments, bit-by-buffer, NN operation dependent */
    uint32_t bankAssignments;
    /* Local memory bank0 allocated size total */
    uint32_t bank0total;
    /* Local memory bank1 allocated size total */
    uint32_t bank1total;
} banks_info_t;

typedef enum {
  U8,
  S8,
  INT32,
  BOOL8,
  FP16
} DataType_t;

typedef enum {
    SET_BUF,
    SET_IN_DMA,
    CAST,
    TOPKV2,
    GATHER,
    QUANTIZE,
    DEQUANTIZE,
    RESIZE,
    REORDER,
    REDUCE,
    SET_OUT_DMA,
    WAIT_FOR_INDATA,
    TILE_DONE,
} OP_STATE_T;

/* Align value up to a multiple of alignment.
   Alignment must be a power of two.
*/
static inline unsigned align_up(unsigned value, unsigned alignment)
{
    return (value + alignment - 1) - ((value + alignment - 1) & (alignment - 1));
}

#define FLK_INCR_NEXT_X(nextX, numTilesX, axisX, allSet)        \
    {   if ((axisX) && !(allSet)) {                             \
            if (++nextX < (numTilesX))                          \
                allSet = 1;                                     \
            else                                                \
                nextX = 0;                                      \
        }                                                       \
    }

int setupDeviceMemoryLayout(uint32_t *sizes,
                            int arraysNum, localMem_info_t *localMem,
                            const banks_info_t *banksInfo);

struct XtensaOperationArgsIn {
  uint32_t numArgs;
  int8_t *args[XTENSA_OPERATION_MAX_NUM_INPUTS];
  uint32_t argsSize[XTENSA_OPERATION_MAX_NUM_INPUTS];
};

struct XtensaOperationArgsOut {
  uint32_t numArgs;
  int8_t *args[XTENSA_OPERATION_MAX_NUM_OUTPUTS];
  uint32_t argsSize[XTENSA_OPERATION_MAX_NUM_OUTPUTS];
};


#endif /* _FLK_COMMON_H_INCLUDED_ */
