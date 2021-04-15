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
#include "cnnrt.h"
#include <string>

// define 1 to use Google's reference implementation, 0 otherwise
#define FLK_USE_GOOGLE_REF 0

#if !(FLK_USE_GOOGLE_REF)
// define 1 to use reference implementation, 0 otherwise
#define REF_FLK_POOL             0
#define REF_FLK_CONV2D           0
#define REF_FLK_DEPTHWISE_CONV2D 0
#endif

#define ENABLE_OUTPUT_DUMP_FILES 0 // when set to 1, dumps output from each op layer

#define XTENSA_BUFFER_SIZE (216 * 1024) // sufficient to hold coeff+biases of every kernel instance

local_mem_info_t *getMeminfoContext(void);
int32_t InitXtensaContext(void);
int32_t getTotalCores(void);
int32_t getMyCore(void);
void *getScratchpadBuffer(void);
void xv_memset(int16_t * pMem, int16_t val, size_t size);
int dumpOutputToFile(std::string filename, int dim0, int dim1, int dim2, int dim3, uint8_t *pData);

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

