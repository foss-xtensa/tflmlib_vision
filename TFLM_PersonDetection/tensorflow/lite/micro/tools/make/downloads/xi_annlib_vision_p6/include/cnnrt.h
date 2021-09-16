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
#ifndef _CNNRT_H_INCLUDED_
#define _CNNRT_H_INCLUDED_

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#if defined(__XCC__) || CSTUB_BUILD
#  include <xtensa/config/core-isa.h>
#include "xi_platform.h"
#endif


#ifdef __XCC__
#define STACK_SIZE (6*1024)
/* Place static data to the local memory with higest address. Stack shall also be in 
highest address dram*/
#  if XCHAL_NUM_DATARAM>1 && XCHAL_DATARAM1_PADDR < XCHAL_DATARAM0_PADDR
#    define _LOCAL_RAM_      __attribute__((section(".dram1.data")))
#  else
#    define _LOCAL_RAM_      __attribute__((section(".dram0.data")))
#  endif
#  define HINT_NEVER        _Pragma("frequency_hint NEVER");
#  define HINT_FREQUENT     _Pragma("frequency_hint FREQUENT");
#  define INLINE            static inline __attribute__((always_inline))
#  define NO_INLINE         __attribute__((noinline))
#  define ALIGN(bytes)      __attribute__((aligned(bytes)))

#  if INLINE_XI
#    define RESTRICT_BARRIER()   _Pragma("no_reorder")
#    define INLINE_RESTRICT      __attribute__((allow_inline_restrict))
#  else
#    define RESTRICT_BARRIER()
#    define INLINE_RESTRICT
#  endif

#else
#define STACK_SIZE (6*1024)
#  define _LOCAL_RAM_
#  define HINT_NEVER
#  define HINT_FREQUENT
#  define INLINE             static inline
#  define NO_INLINE          __attribute__((noinline))
#  define ALIGN(bytes)       __attribute__((aligned(bytes)))
#  define RESTRICT_BARRIER()
#  define INLINE_RESTRICT

#endif

#ifndef XCHAL_IDMA_NUM_CHANNELS
#  define XCHAL_IDMA_NUM_CHANNELS 1
#endif

#if XCHAL_IDMA_NUM_CHANNELS > 1
   /* If hw has more than one channel, use it. */
#  define IDMA_USE_MULTICHANNEL 1
#endif // XCHAL_IDMA_NUM_CHANNELS


#include "cnnrt_perf.h"
#include "cnnrt_xi.h"
#include "cnnrt_debug.h"

#if XCHAL_HAVE_XNNE == 1
#  include "cnnrt_dna100.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum number of memory banks supported */
#define CNNRT_MAX_BANKS_COUNT  2

typedef struct {
    /* Number of memory banks in the bank[] array */
    int banks_count;
    struct {
        /* Memory bank free space start address */
        void   *start;
        /* Memory bank free space size */
        size_t  size;
    } bank[CNNRT_MAX_BANKS_COUNT];
} local_mem_info_t;

/* Get local memory configuration from Linker Support Package symbols.
   On success function returns XI_ERR_OK and fills out mem_info structure
   with information about free space in local memory. Function makes sure
   that DSP stack of stack_size bytes is excluded in reported mem_info.
 */
XI_ERR_TYPE cnnrt_get_local_mem_info(const size_t stack_size, local_mem_info_t *mem_info);

/* Print memory configuration */
void cnnrt_print_local_mem_info(const size_t stack_size, local_mem_info_t *mem_info);

/* Runtime initialization, initialized DMA engine and arena allocator.
   Arena memory manager takes mem_info memory regions under control.

   Args:
     dma_queue_size: max number of items in DMA queue.
     mem_info: memory for arena allocator.
*/
XI_ERR_TYPE cnnrt_dma_init(void);
XI_ERR_TYPE cnnrt_dma_init_loops(unsigned queue_size);
XI_ERR_TYPE cnnrt_init(const unsigned int dma_queue_size, const local_mem_info_t *mem_info);

#if XCHAL_HAVE_XNNE == 1

/* Enable/disable SBLK Compare feature of DNA.

   Args:
     enable: boolean
*/

XI_ERR_TYPE cnnrt_sblk_compare(unsigned int enable);

#endif

/* Runtime de-initialization. */
void cnnrt_deinit();

/* Get static error string for error code */
const char *cnnrt_err_str(XI_ERR_TYPE error_code);


#if CSTUB_BUILD
/* DMA emulation mode in CSTUBs build. Testing with both modes helps to
   detect race conditions on buffers used for DMA transfers.

   Two extreme modes supported:
     slow: for all transfers memory copy is performed on dma_barrier()
           as if all DMA transfers are slow.
     fast: for all transfers memory copy is performed instantly when
           dma transfer is requested as if all DMA transfers are fast.
*/
#  define  CNNRT_DMA_SLOW  1
#  define  CNNRT_DMA_FAST  2
void cnnrt_dma_mode(int mode);
#endif

XI_ERR_TYPE _cnnrt_wait_all();

#ifdef __cplusplus
}
#endif

#include "cnnrt_dma.h"
#include "cnnrt_arena.h"
#include "cnnrt_helpers.h"
#include "cnnrt_compression.h"
#include "cnnrt_compression_support.h"

/* definitions from cnnrt_xmp.h */
#  define cnnrt_core_id()       0
#  define cnnrt_core_num()      1
#  define cnnrt_barrier(status)
#  define cnnrt_is_master()     1

#endif /* _CNNRT_H_INCLUDED_ */

