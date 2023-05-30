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
#ifndef __XTENSA_UTILS_H__
#define __XTENSA_UTILS_H__
#include "cnnrt.h"
#include <string>

/* define 1 to place conv_params object in local memory.
We access elements/parms from conv object very often and 
placing this object in loacl memory (rather than external memory)
would save good number of cycles */
#define CONV_PARAMS_LOCAL_MEM (1)

local_mem_info_t *getMeminfoContext(void);
int32_t InitXtensaContext(void);
int32_t getTotalCores(void);
int32_t getMyCore(void);
void *getScratchpadBuffer(void);
void xv_memset(int16_t * pMem, int16_t val, size_t size);
int dumpOutputToFile(std::string filename, int dim0, int dim1, int dim2, int dim3, uint8_t *pData);
void PrintLog(const char *_t_name, int num_ops, void *ptr);
void print_layer_info_header();

typedef enum {
    TOT_CYC=0,
    KERNEL_CYC=1,
    EDGE_CYC=2,
    IDLE_CYC=3,
    MAC_PER_CYC=4,
    MAC_PERCENT=5,
    TOT_MAC=6,
    DMA_QUEUE=7,
    LAYER_START=8,
    LAYER_END=9,
    OP_IDX=10,
    LOG_END=11
} CNNRT_LOG;

#define TIME_STAMP(cyc_cnt)    \
  {                            \
    cyc_cnt = XT_RSR_CCOUNT(); \
  }

#define USER_DEFINED_HOOKS_START()                  \
  {                                                 \
    xt_iss_switch_mode(XT_ISS_CYCLE_ACCURATE);      \
    xt_iss_trace_level(6);                          \
    xt_iss_client_command("all", "enable");  \
  }

#define USER_DEFINED_HOOKS_STOP()                    \
  {                                                  \
    xt_iss_switch_mode(XT_ISS_FUNCTIONAL);           \
    xt_iss_trace_level(0);                           \
    xt_iss_client_command("all", "disable");  \
  }

#endif /* __XTENSA_UTILS_H__*/