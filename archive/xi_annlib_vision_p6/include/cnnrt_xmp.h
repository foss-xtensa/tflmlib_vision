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
#ifndef _CNNRT_XMP_H_INCLUDED_
#define _CNNRT_XMP_H_INCLUDED_


#ifdef __cplusplus
extern "C" {
#endif

#if 1 > 1

/* Multicore XI run-time macros */
#  define XI_ERROR_CHECKS_MULTICORE()       XI_ERR_TYPE _local_status = XI_ERR_OK;
#  define XI_ERROR_STATUS_MULTICORE()       _local_status
#  define XI_UPDATE_STATUS_MULTICORE(expr)  _local_status = expr;

#  ifdef __XCC__
#    include <xtensa/system/xmp_subsystem.h>
#    include <xtensa/tie/xt_core.h>
#    include <xtensa/config/core-isa.h>
#    include "xi_platform.h"
#    include <xtensa/tie/xt_core.h>
#    include <xtensa/hal.h>
#    include <xtensa/xipc/xipc.h>
#    include <xi_api.h>
#    include "cnnrt_perf.h"
#    include "cnnrt_helpers.h"
#    include "xmp_shared.h"
#    include "xmem.h"
#    include "xmp_mem_pool.h"

#    define cnnrt_core_id()     XT_RSR_PRID()
#    define cnnrt_core_num()    XMP_NUM_PROCS
#    define cnnrt_is_master()   (XT_RSR_PRID() == XMP_MASTER_PID)

#    define FOR_EACH_CORE(statement) statement

    /* __XCC__*/

#  else

    /* CSTUBs */

#    include <sys/types.h>
#    include "cnnrt_helpers.h"
#    include <xi_api.h>
#    include "xmem.h"
#    include "xmp_shared.h"
#    include "xmp_mem_pool.h"

#    define FOR_EACH_CORE(statement)            \
    {                                           \
        pid_t _pid = _cnnrt_cstub_fork();       \
        statement;                              \
        if (_pid) {                             \
            _cnnrt_cstub_wait_children();       \
        }                                       \
    }

extern pid_t _cnnrt_cstub_fork();
extern void _cnnrt_cstub_wait_children();

extern unsigned int cnnrt_core_id();
extern unsigned int cnnrt_core_num();

#    define cnnrt_is_master()    (cnnrt_core_id() == 0)

#  endif /* CSTUBs */

/* Use customized heap manager to manage shared memory pool */

#  define shared_malloc(s)             xmem_alloc(_cnnrt_mem_mgr, s, 8, &_cnnrt_mem_error_code) /* aligned to 64-bit, otherwise cstub will have issues */
#  define shared_memalign(align, s)    xmem_alloc(_cnnrt_mem_mgr, s, align, &_cnnrt_mem_error_code)
#  define shared_free(p, s)            xmem_free(_cnnrt_mem_mgr, p)

extern xmem_mgr_t *_cnnrt_mem_mgr;                     /* shared memory manager */
extern xmem_status_t _cnnrt_mem_error_code;            /* shared memory manager error code */

#define SHARED_MEM_BLOCK_NUM 1024                      /* Max number of allocated blocks in shared memory */
#define SHARED_MEM_HEADER_SIZE XMEM_HEAP3_CACHE_ALIGNED_HEADER_SIZE(SHARED_MEM_BLOCK_NUM)

/* Initialization of multicore subsystem.
   Synchronization of cores, initialization of shared memory,
   communication channels. */

extern void cnnrt_init_multicore(unsigned int num_cores);

/* Multicore barrier.
   Status argument is propagated to all cores.
   Returns XI_ERR_OK only if all cores passed XI_ERR_OK local status.
*/
extern XI_ERR_TYPE cnnrt_barrier(XI_ERR_TYPE local_status);

/* Helper functions for IPC in example applications. */
extern void master_broadcast_task(const mail_packet_t *t);
extern void slave_receive_task(mail_packet_t* t);

#else
    /* Single Core */
#  define cnnrt_core_id()       0
#  define cnnrt_core_num()      1
#  define cnnrt_barrier(status)
#  define cnnrt_is_master()     1

#endif

#ifdef __cplusplus
}
#endif


#endif /* _CNNRT_XMP_H_INCLUDED_ */

