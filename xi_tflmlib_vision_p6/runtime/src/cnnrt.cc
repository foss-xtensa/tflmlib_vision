/* ------------------------------------------------------------------------ */
/* Copyright (c) 2016 by Cadence Design Systems, Inc. ALL RIGHTS RESERVED.  */
/* These coded instructions, statements, and computer programs ("Cadence    */
/* Libraries") are the copyrighted works of Cadence Design Systems Inc.     */
/* Cadence IP is licensed for use with Cadence processor cores only and     */
/* must not be used for any other processors and platforms. Your use of the */
/* Cadence Libraries is subject to the terms of the license agreement you   */
/* have entered into with Cadence Design Systems, or a sublicense granted   */
/* to you by a direct Cadence licensee.                                     */
/* ------------------------------------------------------------------------ */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#define IDMA_ERROR_CHECKING 0
#define IDMA_BUSY_CHECKING  0
#define IDMA_USE_INTR 0

// Since idma-xtos library is used, IDMA_APP_USE_XTOS must be defined prior including idma.h
#if !_MSC_VER
#include <xtensa/idma.h>
#else
#ifndef XCHAL_IDMA_NUM_CHANNELS
# define XCHAL_IDMA_NUM_CHANNELS        1
#endif
#endif
#include "cnnrt.h"

/* iDMA initialization variables scope. Static by default. */
#ifndef DMA_DEFS_API
#  define DMA_DEFS_API const static
#endif

#if !CSTUB_BUILD
#  if XCHAL_VISION_TYPE >= 7
#    define IDMA_USE_64B_DESC 1
#  endif

#endif  // CSTIB_BUILD

#if IDMA_USE_64B_DESC 
#define IDMA_USE_3D_DESC 1
#else
#define IDMA_USE_3D_DESC 0
#endif

//#define IDMA_USE_3D_DESC 0

#ifdef __XCC__

#  if defined(XTENSA_SWVERSION_RG_2018_9) && XTENSA_SWVERSION >= XTENSA_SWVERSION_RG_2018_9 && XCHAL_NUM_DATARAM>1 && XCHAL_DATARAM1_PADDR < XCHAL_DATARAM0_PADDR
#    define IDMA_USE_DRAM1
#  endif



#  if XCHAL_IDMA_NUM_CHANNELS > 1
#    define cnnrt_idma_schedule_desc(ch, to_schedule) \
    idma_schedule_desc(ch, to_schedule)
#    define cnnrt_idma_buffer_status(ch) \
    idma_buffer_status(ch)
#    define cnnrt_idma_init(ch, flags, blocks, pif, ticks_per_cycle, ticks_timeout, cb) \
    idma_init(ch, flags, blocks, pif, ticks_per_cycle, ticks_timeout, cb)
#    define cnnrt_idma_init_loop(ch, buff, flags, queue_size, cb_data, cb_func) \
    idma_init_loop(ch, buff, flags, queue_size, cb_data, cb_func)
#  else
#    define cnnrt_idma_schedule_desc(ch, to_schedule) \
    idma_schedule_desc(to_schedule)
#    define cnnrt_idma_buffer_status(ch) \
    idma_buffer_status()
#    define cnnrt_idma_init(ch, flags, blocks, pif, ticks_per_cycle, ticks_timeout, cb) \
    idma_init(flags, blocks, pif, ticks_per_cycle, ticks_timeout, cb)
#    define cnnrt_idma_init_loop(ch, buff, flags, queue_size, cb_data, cb_func) \
    idma_init_loop(buff, flags, queue_size, cb_data, cb_func)
#  endif

#else // __XCC__

int _idmaCopyOnAdd = 1;

typedef int32_t idma_ticks_cyc_t;
typedef int32_t idma_status_t;

typedef struct {
    idma_status_t status;
} idma_error_details_t;

typedef struct {
    int      ch;
    void    *src;
    void    *dst;
    size_t   size;
    int      src_pitch;
    int      dst_pitch;
    size_t   rows;
#if (IDMA_USE_3D_DESC == 1)
    int      src_tile_pitch;
    int      dst_tile_pitch;
    size_t   ntiles;
#endif
} idma_desc_t;

typedef idma_desc_t idma_buffer_t;
typedef void (*idma_err_callback_fn)(idma_error_details_t* data);
typedef void (*idma_callback_fn)();

#  define IDMA_OK            0
#  define IDMA_ERR_BAD_DESC -1
#  define TICK_CYCLES_2      0
#  define DESC_IDMA_PRIOR_H  0
#  define IDMA_BUFFER_SIZE(cnt, dummy) ((cnt)*sizeof(idma_desc_t))

/* Queue indexes */
static unsigned dma_desc_free[XCHAL_IDMA_NUM_CHANNELS];
static unsigned dma_desc_last[XCHAL_IDMA_NUM_CHANNELS];

static idma_status_t idma_add_2d_desc(idma_buffer_t *buff, void *dst, void *src, size_t d1_cnt, int dummy, size_t d2_cnt, int src_pitch, int dst_pitch);

//TODO: Need to check for 64B flag in Q7
#if (IDMA_USE_3D_DESC == 1)
static idma_status_t idma_add_2d_desc64(idma_buffer_t *buff, void *dst, void *src, size_t d1_cnt, int dummy, size_t d2_cnt, int src_pitch, int dst_pitch);
static idma_status_t idma_add_3d_desc64(idma_buffer_t *buff, void *dst, void *src, int dummy/*optional*/, size_t d1_cnt, int d2_cnt, int d3_cnt, int src_row_pitch, int dst_row_pitch, int src_tile_pitch, int dst_tile_pitch); 
#endif

static void do_memcpy(int ch, int number);
static idma_status_t cnnrt_idma_schedule_desc(int ch, unsigned to_schedule);
static idma_status_t cnnrt_idma_buffer_status(int ch);

/* Non XCC end */
#endif

/*
 * Defines for both CSTUBS and XCC
 */

/*
 * Global variables.
 */

typedef struct {
    /* Pointer at DMA descriptors queue alocated from arena. */
    idma_buffer_t *_idmaObjBuff[XCHAL_IDMA_NUM_CHANNELS];
    /* Number of DMA descriptors allocated */
    unsigned _idmaDescrTotal[XCHAL_IDMA_NUM_CHANNELS];
    /* Descriptors counter to avoid overflows of idmaObjBuff */
    unsigned _idmaDescrCount[XCHAL_IDMA_NUM_CHANNELS];
    /* Boolean indicator if iDMA is in error state */
    uint32_t _idmaStatus[XCHAL_IDMA_NUM_CHANNELS];
} cnnrt_tls;

#ifdef XOS_BUILD
uint32_t cnnrt_tls_key;
cnnrt_tls *cnnrt_tls_get(void) {
    return (cnnrt_tls *)xos_tls_get(cnnrt_tls_key);
}
#else
cnnrt_tls cnnrtLocalStorage _LOCAL_RAM_;
#define cnnrt_tls_get() (&cnnrtLocalStorage)
#endif

#ifdef XCHAL_IDMA_MAX_OUTSTANDING_REQ
#  define _IDMA_MAX_OUTSTANDING_REQ XCHAL_IDMA_MAX_OUTSTANDING_REQ
#else
/* On older tools use fixed max number */
#  define _IDMA_MAX_OUTSTANDING_REQ 64
#endif

/* Configuration global variables used by dma_init() */
DMA_DEFS_API int32_t                 _idmaInitFlags   = 0;
DMA_DEFS_API idma_ticks_cyc_t        _idmaTicksPerCyc = TICK_CYCLES_2;
DMA_DEFS_API int32_t                 _idmaTimeoutTicks= 0;
DMA_DEFS_API int32_t                 _idmaMaxPIF      = _IDMA_MAX_OUTSTANDING_REQ;
DMA_DEFS_API idma_err_callback_fn    _idmaCBError     = NULL;
#if defined(XOS_BUILD) || (defined(IDMA_USE_INTR) && (IDMA_USE_INTR == 1))
static void *                        _idmaCBData      = NULL;
DMA_DEFS_API idma_callback_fn        _idmaCBFunc      = NULL;
#else
#  ifdef __XCC__
static void *           _idmaCBData      = NULL;
static idma_callback_fn _idmaCBFunc      = NULL;
#  endif
#endif

/*
 * End of global variables
 */

#if CNNRT_PERF_LEVEL != CNNRT_PERF_LEVEL_NONE

#define STATS_WAIT(code)                                                \
    {                                                                   \
        uint32_t _t_cycles = _time_stamp();                             \
        { code }                                                        \
        _cnnrt_perf_idleWaitCycles += _time_stamp() - _t_cycles;        \
    }

#define STATS_QUEUE_SIZE(size)    _cnnrt_perf_idmaMaxQueueSize = _cnnrt_perf_idmaMaxQueueSize >= (size) ? _cnnrt_perf_idmaMaxQueueSize : (size);
#define STATS_QUEUE_OVERFLOWED()  _cnnrt_perf_idmaQueueOverflow = 1;

#else

#define STATS_WAIT(code)          { code }
#define STATS_QUEUE_SIZE(size)
#define STATS_QUEUE_OVERFLOWED()

#endif


/* Increment number of descriptors in the DMA queue.
   Waits for previous DMA transfer completion if queue is full
   to make sure free slot is available in the queue.

   to_schedule is the number of added but not scheduled descriptors.
   
   Returns number of scheduled DMA transfers.

*/
INLINE int inc_desc_counter (int ch, 
                             unsigned to_schedule)
{
    int result = 0;

    cnnrt_tls *lptr = cnnrt_tls_get();

    if (lptr->_idmaDescrCount[ch] >= lptr->_idmaDescrTotal[ch]) {
        /* Descriptors queue is full */
        HINT_NEVER;
        /* Record that queue overflowed if compiled with stat instrumentation */
        STATS_QUEUE_OVERFLOWED();

        if (to_schedule >= lptr->_idmaDescrTotal[ch]) {
            HINT_NEVER;
            if (lptr->_idmaStatus[ch]) {
                /* Already in error state */
                HINT_NEVER;
                return 0;
            }
            /* Schedule added descriptors */
            if (to_schedule > 0 && cnnrt_idma_schedule_desc(ch, to_schedule) < 0) {
                HINT_NEVER;
                lptr->_idmaStatus[ch] |= cnnrt_idma_buffer_status(ch);
                return 0;
            }
            /* Return number of scheduled descriptors */
            result = to_schedule;
        }
        /* Wait for completion of scheduled descriptors */
        if (dma_barrier_ch( ch ) != XI_ERR_OK) {
            HINT_NEVER;
            return 0;
        }
        /* Barrier resets number of descriptors based on assumption
           that all added descriptors are scheduled. Restoring
           number of added descriptors here. */
        lptr->_idmaDescrCount[ch] = to_schedule;
    }
    lptr->_idmaDescrCount[ch]++;
    /* Record max queue size */
    STATS_QUEUE_SIZE(lptr->_idmaDescrCount[ch]);
    return result;
}

XI_ERR_TYPE cnnrt_dma_init(void)
{
    XI_ERROR_CHECKS() {}

    for (int i=0; i<XCHAL_IDMA_NUM_CHANNELS; i++) {
#ifdef __XCC__
        uint32_t Status = cnnrt_idma_init(i, _idmaInitFlags, MAX_BLOCK_16, _idmaMaxPIF, _idmaTicksPerCyc, _idmaTimeoutTicks, _idmaCBError);
        (void) Status;
        XI_RUN_TIME_CHECK(Status == IDMA_OK, "Failed to initialize iDMA library", XI_ERR_BADARG);
#else // __XCC__
        dma_desc_free[i] = 0;
        dma_desc_last[i] = 0;
#endif
    }

    return XI_ERROR_STATUS();
}

XI_ERR_TYPE cnnrt_dma_init_loops(const unsigned queue_size)
{
    XI_ERROR_CHECKS() {
        XI_RUN_TIME_CHECK(queue_size >= 8 && (queue_size & (queue_size - 1)) == 0,
                          "Invalid queue size", XI_ERR_BADARG);
    }

#ifdef XOS_BUILD
    cnnrt_tls *lptr;
    // Allocate space for thread mutable variables and store it in TLS
    arena_static_alloc((void **)&lptr, sizeof(cnnrt_tls), sizeof(unsigned long long));
    xos_tls_set(cnnrt_tls_key, (void*)lptr);
#else
    cnnrt_tls *lptr = cnnrt_tls_get();
#endif

    for (int i=0; i<XCHAL_IDMA_NUM_CHANNELS; i++) {
#if (IDMA_USE_3D_DESC == 1)
        XI_CHECK_RESULT(arena_static_alloc((void**)&(lptr->_idmaObjBuff[i]), IDMA_BUFFER_SIZE(queue_size, IDMA_64B_DESC), 64));
#else
        XI_CHECK_RESULT(arena_static_alloc((void**)&(lptr->_idmaObjBuff[i]), IDMA_BUFFER_SIZE(queue_size, IDMA_2D_DESC), 64));
#endif
#ifdef __XCC__
#if (IDMA_USE_3D_DESC == 1)
        lptr->_idmaStatus[i] = cnnrt_idma_init_loop(i, lptr->_idmaObjBuff[i], IDMA_64B_DESC, queue_size, _idmaCBData, _idmaCBFunc);
#else
        lptr->_idmaStatus[i] = cnnrt_idma_init_loop(i, lptr->_idmaObjBuff[i], IDMA_2D_DESC, queue_size, _idmaCBData, _idmaCBFunc);
#endif
        XI_RUN_TIME_CHECK(lptr->_idmaStatus[i] == IDMA_OK, "Failed to initialize iDMA buffer", XI_ERR_BADARG);
#endif
        lptr->_idmaDescrCount[i] = 0;
        lptr->_idmaDescrTotal[i] = queue_size;
    }

    return XI_ERROR_STATUS();
}

XI_ERR_TYPE dma_barrier_ch(int ch)
{
    XI_ERROR_CHECKS() {}

    XI_RUN_TIME_CHECK(ch >=0 && ch < XCHAL_IDMA_NUM_CHANNELS, "Invalid iDMA channel index", XI_ERR_BADARG);

    cnnrt_tls *lptr = cnnrt_tls_get();

    /* Reset descriptors counter */
    lptr->_idmaDescrCount[ch] = 0;

    XI_RUN_TIME_CHECK(lptr->_idmaStatus[ch] == XI_ERR_OK, "iDMA engine is in error state", XI_ERR_BADARG);

#ifdef __XCC__

#ifdef  XOS_BUILD
    uint32_t status;

    while ((status = idma_buffer_status_i(ch)) > 0) {
      STATS_WAIT(idma_sleep_i(ch););
    }
    if (status == IDMA_CANT_SLEEP)
        status = XI_ERR_OK;
    lptr->_idmaStatus[ch] |= status;
    XI_RUN_TIME_CHECK(lptr->_idmaStatus[ch] == XI_ERR_OK, "iDMA engine is in error state", XI_ERR_BADARG);

#else // XOS_BUILD
#  if XCHAL_IDMA_NUM_CHANNELS > 1
    if (IDMA_HW_NUM_OUTSTANDING( ch ) > 0) {
        HINT_NEVER;
        /* Measure wait cycles if compiled with stats instrumentation */
        STATS_WAIT (IDMA_HW_WAIT_ALL ( ch ););
    }
#  else
    if (IDMA_HW_NUM_OUTSTANDING() > 0) {
        HINT_NEVER;
        /* Measure wait cycles if compiled with stats instrumentation */
        STATS_WAIT (IDMA_HW_WAIT_ALL (););
    }
#  endif // MULTICHANNEL

    /* Check for errors */
#  if defined(READ_IDMA_REG) || (defined(XTENSA_SWVERSION_RI_2019_1) && XTENSA_SWVERSION >= XTENSA_SWVERSION_RI_2019_1)
    if ((READ_IDMA_REG(ch, IDMA_REG_STATUS) & IDMA_STATE_MASK) == IDMA_STATE_ERROR) {
#  else
    if ((XT_RER(idmareg_base + IDMA_REG_STATUS) & IDMA_STATE_MASK) == IDMA_STATE_ERROR) {
#  endif
        HINT_NEVER;
        lptr->_idmaStatus[ch] |= cnnrt_idma_buffer_status(ch);
        XI_RUN_TIME_CHECK(lptr->_idmaStatus[ch] == XI_ERR_OK, "iDMA engine is in error state", XI_ERR_BADARG);
    }
#endif // XOS_BUILD

#else
    /* CSTUBs */
    if (!_idmaCopyOnAdd) {
        /* Copy all requests on wait */
        do_memcpy(ch, /*to_schedule=*/-1);
    }
#endif
    return XI_ERROR_STATUS();
}

XI_ERR_TYPE dma_barrier() {
     XI_ERROR_CHECKS() {}
     for (int i=0; i<XCHAL_IDMA_NUM_CHANNELS; i++) {
         XI_CHECK_RESULT(dma_barrier_ch(i));
     }
    return XI_ERROR_STATUS();
}


#if 0 && CHECK_DMA_ACROSS_STACK
extern void __stack;
#endif

INLINE idma_status_t add_single_2d_desc(int ch,
                                        void *src, void *dst, size_t d1_cnt,
                                        int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                                        unsigned *added, unsigned *scheduled)
{
#if 0 && CHECK_DMA_ACROSS_STACK
    char *stack_top = (char*)&__stack;
    char *stack_bottom = stack_top - 12*1024;
    char *dst_low = (char*)dst;
    char *dst_high = (char*)dst + d2_dst_pitch * (d2_cnt - 1) + d1_cnt;
    if (stack_top > dst_low && stack_bottom < dst_high) {
        /* DMA request intersects with STACK */
        return IDMA_ERR_BAD_DESC;
    }
#endif

    *scheduled += inc_desc_counter (ch, *added - *scheduled);
    *added += 1;

    cnnrt_tls *lptr = cnnrt_tls_get();

    return idma_add_2d_desc(lptr->_idmaObjBuff[ch], dst, src, d1_cnt, DESC_IDMA_PRIOR_H,
                            d2_cnt, d2_src_pitch, d2_dst_pitch);
}

#if (IDMA_USE_3D_DESC == 1)
INLINE idma_status_t add_single_2d_desc64(int ch,
                                          void *src, void *dst, size_t d1_cnt,
                                          int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                                          unsigned *added, unsigned *scheduled)
{
    *scheduled += inc_desc_counter (ch, *added - *scheduled);
    *added += 1;

    cnnrt_tls *lptr = cnnrt_tls_get();

    return idma_add_2d_desc64(lptr->_idmaObjBuff[ch], &dst, &src, d1_cnt, DESC_IDMA_PRIOR_H,
                                                   d2_cnt, d2_src_pitch, d2_dst_pitch);
}

INLINE idma_status_t add_single_3d_desc64(int ch,
                                          void *src, void *dst, size_t d1_cnt,
                                          int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                                          int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt,
                                          unsigned *added, unsigned *scheduled)
{

    *scheduled += inc_desc_counter (ch, *added - *scheduled);
    *added += 1;

    cnnrt_tls *lptr = cnnrt_tls_get();

    idma_status_t idma_status =  idma_add_3d_desc64(lptr->_idmaObjBuff[ch], &dst, &src, DESC_IDMA_PRIOR_H, d1_cnt,
                                  d2_cnt, d3_cnt, d2_src_pitch, d2_dst_pitch, d3_src_pitch, d3_dst_pitch);

#if (XTENSA_SWVERSION <= XTENSA_SWVERSION_RI_2019_2)
    //TODO: Remove af bug fix in idma library
    IDMA_DISABLE_INTS();
    idma_buf_t* buf = (uint8_t *)&(lptr->_idmaObjBuff[ch][0]);
    update_next_add_ptr(buf);
    IDMA_ENABLE_INTS();
#endif 

    return idma_status;
}
#endif

INLINE int add_2d_desc_proxy(int ch,
                       void *src, void *dst, size_t d1_cnt,
                       int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                       unsigned added, unsigned isSys2Loc, unsigned straddles)
{
    idma_status_t idma_status = IDMA_OK;
    /* Number of scheduled descriptors */
    unsigned scheduled = 0;
#if XCHAL_NUM_DATARAM>1 && \
    (((XCHAL_DATARAM0_PADDR + XCHAL_DATARAM0_SIZE) == XCHAL_DATARAM1_PADDR) || \
     ((XCHAL_DATARAM1_PADDR + XCHAL_DATARAM1_SIZE) == XCHAL_DATARAM0_PADDR))
    /* Check if request crosses DRAM boundary and split into multiple requests if it does. */
    uint8_t *first;
    uint8_t *last;
    uint8_t *boundary;
    int pitch;

#  if XCHAL_NUM_DATARAM>2
#    warning "XI CNN supports up to two local memories, DMA subsystem is not designed for crossing more memories."
#  endif

    boundary = (uint8_t*)((XCHAL_DATARAM0_PADDR < XCHAL_DATARAM1_PADDR) ? XCHAL_DATARAM1_PADDR : XCHAL_DATARAM0_PADDR);

    /* Look at local memory side */
    if (isSys2Loc) {
        pitch = d2_dst_pitch;
        first = (uint8_t*)dst;
    } else {
        pitch = d2_src_pitch;
        first = (uint8_t*)src;
    }
    last = first + pitch * (d2_cnt - 1) + d1_cnt - 1;
    if (straddles && first < boundary && last >= boundary)
    {
        HINT_NEVER;
        size_t rows = (boundary - first) / pitch;
        size_t rem = (boundary - first) - rows * pitch;
        /* Add one more row if DRAM boundary goes between rows */
        rows += rem >= d1_cnt;
        if (rows > 0) {
            /* Schedule full row transfer to DRAM0 */
#if (IDMA_USE_3D_DESC == 1)
            idma_status |= add_single_2d_desc64(ch, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, rows, &added, &scheduled);
#else
            idma_status = (idma_status_t)((int)idma_status | (int) add_single_2d_desc(ch, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, rows, &added, &scheduled));
#endif
            src = (uint8_t*)src + rows * d2_src_pitch;
            dst = (uint8_t*)dst + rows * d2_dst_pitch;
            d2_cnt -= rows;
        }
        if (rem != 0 && rem < d1_cnt) {
            /* Need also to split one row into two transfers */
#if (IDMA_USE_3D_DESC == 1)
            idma_status |= add_single_2d_desc64 (ch, src, dst, rem, d2_src_pitch, d2_dst_pitch, 1, &added, &scheduled);
            idma_status |= add_single_2d_desc64 (ch, src+rem, dst+rem, d1_cnt-rem, d2_src_pitch, d2_dst_pitch, 1, &added, &scheduled);
#else
            idma_status =(idma_status_t)((int)idma_status | (int) add_single_2d_desc (ch, src, dst, rem, d2_src_pitch, d2_dst_pitch, 1, &added, &scheduled));
            idma_status = (idma_status_t)((int)idma_status | (int)add_single_2d_desc (ch, (char*)src+rem, (char*)dst+rem, d1_cnt-rem, d2_src_pitch, d2_dst_pitch, 1, &added, &scheduled));
#endif
            src = (uint8_t*)src + d2_src_pitch;
            dst = (uint8_t*)dst + d2_dst_pitch;
            d2_cnt --;
        }
        if (d2_cnt == 0)
            return added - scheduled;
        /* Fall through to perform last transform */
    }
#endif

    cnnrt_tls *lptr = cnnrt_tls_get();

#if (IDMA_USE_3D_DESC == 1)
    lptr->_idmaStatus[ch] |= idma_status | add_single_2d_desc64(ch, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, &added, &scheduled);
#else
    lptr->_idmaStatus[ch] |= idma_status | add_single_2d_desc(ch, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, &added, &scheduled);
#endif
    return added - scheduled;
}

INLINE int add_2d_desc(int ch, void *src, void *dst, size_t d1_cnt,
                       int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                       unsigned added, unsigned isSys2Loc, unsigned straddles)
{
#if defined(XCHAL_HAVE_NX) && (XCHAL_HAVE_NX == 1) && defined(XCHAL_HAVE_VISION) && (XCHAL_HAVE_VISION == 1) && (XCHAL_VISION_TYPE == 6)
    /* Workaround Vision Q6 limitation on NUM_ROWS size */
    size_t num_rows;
    for (; d2_cnt > 0xffffU; d2_cnt -= 0xffffU) {
        num_rows = min(0xffffU, d2_cnt);
        added = add_2d_desc_proxy(ch, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, num_rows, added, isSys2Loc, straddles);
        src = (uint8_t*)src + num_rows * d2_src_pitch;
        dst = (uint8_t*)dst + num_rows * d2_dst_pitch;
    }
#endif
    added = add_2d_desc_proxy(ch, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, added, isSys2Loc, straddles);
    return added;
}


#if (IDMA_USE_3D_DESC==1)
INLINE int add_3d_desc64(int ch, void *src, void *dst, size_t d1_cnt,
                         int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                         int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt, 
                         unsigned added, unsigned isSys2Loc, unsigned straddles)
{
    idma_status_t idma_status = IDMA_OK;
    /* Number of scheduled descriptors */
    unsigned scheduled = 0;
#if XCHAL_NUM_DATARAM>1 && \
    (((XCHAL_DATARAM0_PADDR + XCHAL_DATARAM0_SIZE) == XCHAL_DATARAM1_PADDR) || \
     ((XCHAL_DATARAM1_PADDR + XCHAL_DATARAM1_SIZE) == XCHAL_DATARAM0_PADDR))
    /* Check if request crosses DRAM boundary and split into multiple requests if it does. */
    uint8_t *first;
    uint8_t *last;
    uint8_t *boundary;
    int d2_pitch, d3_pitch;

#  if XCHAL_NUM_DATARAM>2
#    warning "XI CNN supports up to two local memories, DMA subsystem is not designed for crossing more memories."
#  endif

    boundary = (uint8_t*)((XCHAL_DATARAM0_PADDR < XCHAL_DATARAM1_PADDR) ? XCHAL_DATARAM1_PADDR : XCHAL_DATARAM0_PADDR);

    /* Look at local memory side */
    if (isSys2Loc) {
        d2_pitch = d2_dst_pitch;
        d3_pitch = d3_dst_pitch;
        first = (uint8_t*)dst;
    } else {
        d2_pitch = d2_src_pitch;
        d3_pitch = d3_src_pitch;
        first = (uint8_t*)src;
    }
    last = first + d3_pitch * (d3_cnt - 1) + d2_pitch * (d2_cnt - 1) + d1_cnt - 1;
    if (straddles && first < boundary && last >= boundary)
    {
        HINT_NEVER;
        //size_t rows  = (boundary - first) / d2_pitch;
        size_t tiles    = (boundary - first) / d3_pitch;
        size_t rows_rem = ((boundary - first) - tiles * d3_pitch) / d2_pitch;  
        size_t rem      = (boundary - first) - rows_rem * d2_pitch - tiles * d3_pitch;
        /* Add one more row if DRAM boundary goes between rows */
        tiles += rows_rem >= d2_cnt;
        //rows  += rem >= d1_cnt;
        if (tiles > 0) {
            /* Schedule full row transfer to DRAM0 */
            idma_status |= add_single_3d_desc64(ch, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, tiles, &added, &scheduled);
            src = (uint8_t*)src + tiles * d3_src_pitch;
            dst = (uint8_t*)dst + tiles * d3_dst_pitch;
            //d2_cnt -= rows;
            d3_cnt -= tiles;
        }
        if (rows_rem != 0 && rows_rem < d2_cnt) {
            /* Need also to split one row into two transfers */
            idma_status |= add_single_3d_desc64(ch, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, rows_rem, d3_src_pitch, d3_dst_pitch, 1, &added, &scheduled);
            src = (uint8_t*)src + rows_rem * d2_src_pitch;
            dst = (uint8_t*)dst + rows_rem * d2_dst_pitch;
            if (rem != 0 && rem < d1_cnt) {
                idma_status |= add_single_3d_desc64(ch, src, dst, rem, d2_src_pitch, d2_dst_pitch, 1, d3_src_pitch, d3_dst_pitch, 1, &added, &scheduled);
                idma_status |= add_single_3d_desc64(ch, src + rem, dst + rem, d1_cnt - rem, d2_src_pitch, d2_dst_pitch, 1, d3_src_pitch, d3_dst_pitch, 1, &added, &scheduled);
                src = (uint8_t*)src + d2_src_pitch;
                dst = (uint8_t*)dst + d2_dst_pitch;
                d2_cnt --; 
            }
            if (d2_cnt == 0)
                return added - scheduled;

            idma_status |= add_single_3d_desc64(ch, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt - rows_rem, d3_src_pitch, d3_dst_pitch, 1, &added, &scheduled);
			src = (uint8_t*)src - (rows_rem * d2_src_pitch) + d3_src_pitch;
            dst = (uint8_t*)dst - (rows_rem * d2_dst_pitch) + d3_dst_pitch;
			//move pointer back to original place . one Width height plane is completely done 
            if (rem != 0 && rem < d1_cnt) {
                src = (uint8_t*)src - d2_src_pitch ;
                dst = (uint8_t*)dst - d2_dst_pitch ;
                d2_cnt++;			
           }			
            d3_cnt --;
        }

        if (d3_cnt == 0)
            return added - scheduled;
        /* Fall through to perform last transform */
    }
#endif
    cnnrt_tls *lptr = cnnrt_tls_get();

    lptr->_idmaStatus[ch] |= idma_status | add_single_3d_desc64(ch, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt, &added, &scheduled);
    return added - scheduled;
}
#endif

INLINE void schedule_descriptors(int ch,
                                 unsigned added)
{
    cnnrt_tls *lptr = cnnrt_tls_get();

    if (!lptr->_idmaStatus[ch] && cnnrt_idma_schedule_desc (ch, added) < 0) {
        HINT_NEVER;
        lptr->_idmaStatus[ch] |= cnnrt_idma_buffer_status( ch );
    }
}

void dma_1d_loc2sys_ch(int ch,
                       void *src, void *dst, size_t d1_cnt)
{
    unsigned added = add_2d_desc(ch,
                                 src, dst, d1_cnt,
                                 d1_cnt, d1_cnt, 1,
                                 /*added*/ 0, /*isSys2Loc*/ 0, /*straddles*/ 0);
    schedule_descriptors(ch,
                         added);
}

void dma_1d_sys2loc_ch(int ch,
                       void *src, void *dst, size_t d1_cnt)
{
    unsigned added = add_2d_desc(ch,
                                 src, dst, d1_cnt,
                                 d1_cnt, d1_cnt, 1,
                                 /*added*/ 0, /*isSys2Loc*/ 1, /*straddles*/ 0);
    schedule_descriptors(ch,
                         added);
}

void dma_2d_loc2sys_ch(int ch,
                       void *src, void *dst, size_t d1_cnt, int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt)
{
    unsigned added = add_2d_desc(ch,
                                 src, dst, d1_cnt,
                                 d2_src_pitch, d2_dst_pitch, d2_cnt,
                                 /*added*/ 0, /*isSys2Loc*/ 0, /*straddles*/ 0);
    schedule_descriptors(ch,
                         added);
}

void dma_2d_sys2loc_ch(int ch,
                       void *src, void *dst, size_t d1_cnt, int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt)
{
    unsigned added = add_2d_desc(ch,
                                 src, dst, d1_cnt,
                                 d2_src_pitch, d2_dst_pitch, d2_cnt,
                                 /*added*/ 0, /*isSys2Loc*/ 1, /*straddles*/ 0);
    schedule_descriptors(ch,
                         added);
}

void dma_2d_loc2sys_dyn_ch(int ch,
                           void *src, void *dst, size_t d1_cnt, int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt)
{
    unsigned added;
    if ((int)d1_cnt == d2_src_pitch && (int)d1_cnt == d2_dst_pitch) {
        added = add_2d_desc(ch,
                            src, dst, d1_cnt * d2_cnt, d1_cnt * d2_cnt, d1_cnt * d2_cnt, 1,
                            /*added*/ 0, /*isSys2Loc*/ 0, /*straddles*/ 0);
    } else {
        added = add_2d_desc(ch,
                            src, dst, d1_cnt,
                            d2_src_pitch, d2_dst_pitch, d2_cnt,
                            /*added*/ 0, /*isSys2Loc*/ 0, /*straddles*/ 0);
    }
    schedule_descriptors(ch,
                         added);
}

void dma_2d_sys2loc_dyn_ch(int ch,
                           void *src, void *dst, size_t d1_cnt, int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt)
{
    unsigned added;
    if ((int)d1_cnt == d2_src_pitch && (int)d1_cnt == d2_dst_pitch) {
        added = add_2d_desc(ch,
                            src, dst, d1_cnt * d2_cnt, d1_cnt * d2_cnt, d1_cnt * d2_cnt, 1,
                            /*added*/ 0, /*isSys2Loc*/ 1, /*straddles*/ 0);
    } else {
        added = add_2d_desc(ch,
                            src, dst, d1_cnt,
                            d2_src_pitch, d2_dst_pitch, d2_cnt,
                            /*added*/ 0, /*isSys2Loc*/ 1, /*straddles*/ 0);
    }
    schedule_descriptors(ch,
                         added);
}

void dma_3d_loc2sys_ch(int ch,
                       void *src, void *dst, size_t d1_cnt,
                       int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                       int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt)
{
    unsigned added = 0;
#if (IDMA_USE_3D_DESC==1)    
    added = add_3d_desc64(ch, 
                          (uint8_t*)src, 
                          (uint8_t*)dst,
                          d1_cnt,
                          d2_src_pitch,
                          d2_dst_pitch,
                          d2_cnt,
                          d3_src_pitch,
                          d3_dst_pitch,
                          d3_cnt,
                          added,
                          /*isSys2Loc*/0, /*straddles*/0);
#else
    for (unsigned i = 0; i < d3_cnt; ++i) {
        added = add_2d_desc(ch,
                            (uint8_t*)src + i*d3_src_pitch,
                            (uint8_t*)dst + i*d3_dst_pitch,
                            d1_cnt,
                            d2_src_pitch,
                            d2_dst_pitch,
                            d2_cnt,
                            added,
                            /*isSys2Loc*/ 0, /*straddles*/ 0);
    }
#endif
    schedule_descriptors(ch,
                         added);
}

void dma_3d_sys2loc_ch(int ch,
                       void *src, void *dst, size_t d1_cnt,
                       int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                       int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt)
{
    unsigned added = 0;
#if (IDMA_USE_3D_DESC==1)    
    added = add_3d_desc64(ch,
                          (uint8_t*)src, 
                          (uint8_t*)dst,
                          d1_cnt,
                          d2_src_pitch,
                          d2_dst_pitch,
                          d2_cnt,
                          d3_src_pitch,
                          d3_dst_pitch,
                          d3_cnt,
                          added,
                          /*isSys2Loc*/1, /*straddles*/0);
#else
    for (unsigned i = 0; i < d3_cnt; ++i) {
        added = add_2d_desc(ch,
                            (uint8_t*)src + i*d3_src_pitch,
                            (uint8_t*)dst + i*d3_dst_pitch,
                            d1_cnt,
                            d2_src_pitch,
                            d2_dst_pitch,
                            d2_cnt,
                            added,
                            /*isSys2Loc*/ 1, /*straddles*/ 0);
    }
#endif
    schedule_descriptors(ch,
                         added);
}

void dma_3d_loc2sys_dyn_ch(int ch,
                           void *src, void *dst, size_t d1_cnt,
                           int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                           int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt)
{
    /* Fold dimensions if possible */
    if ((int)d1_cnt == d2_src_pitch
        && (int)d1_cnt == d2_dst_pitch) {
        dma_2d_loc2sys_dyn_ch(ch,
                              src, dst, d1_cnt * d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt);
    } else if ((int)d2_cnt * d2_src_pitch == d3_src_pitch
               && (int)d2_cnt * d2_dst_pitch == d3_dst_pitch) {
        dma_2d_loc2sys_dyn_ch(ch,
                              src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt * d3_cnt);
    } else {
        dma_3d_loc2sys_ch(ch,
                          src, dst, d1_cnt,
                          d2_src_pitch, d2_dst_pitch, d2_cnt,
                          d3_src_pitch, d3_dst_pitch, d3_cnt);
    }
}

void dma_3d_sys2loc_dyn_ch(int ch,
                           void *src, void *dst, size_t d1_cnt,
                           int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                           int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt)
{
    /* Fold dimensions if possible */
    if ((int)d1_cnt == d2_src_pitch
        && (int)d1_cnt == d2_dst_pitch) {
        dma_2d_sys2loc_dyn_ch(ch,
                              src, dst, d1_cnt * d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt);
    } else if ((int)d2_cnt * d2_src_pitch == d3_src_pitch
               && (int)d2_cnt * d2_dst_pitch == d3_dst_pitch) {
        dma_2d_sys2loc_dyn_ch(ch,
                              src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt * d3_cnt);
    } else {
        dma_3d_sys2loc_ch(ch,
                          src, dst, d1_cnt,
                          d2_src_pitch, d2_dst_pitch, d2_cnt,
                          d3_src_pitch, d3_dst_pitch, d3_cnt);
    }
}

void dma_4d_loc2sys_ch(int ch,
                       void *src, void *dst, size_t d1_cnt,
                       int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                       int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt,
                       int d4_src_pitch, int d4_dst_pitch, size_t d4_cnt)
{
    unsigned added = 0;
    for (unsigned j = 0; j < d4_cnt; ++j) {
#if (IDMA_USE_3D_DESC==1)    
        added = add_3d_desc64(ch,
                              (uint8_t*)src + j * d4_src_pitch, 
                              (uint8_t*)dst + j * d4_dst_pitch,
                              d1_cnt,
                              d2_src_pitch,
                              d2_dst_pitch,
                              d2_cnt,
                              d3_src_pitch,
                              d3_dst_pitch,
                              d3_cnt,
                              added,
                              /*isSys2Loc*/0, /*straddles*/0);
#else
        for (unsigned i = 0; i < d3_cnt; ++i) {
            added = add_2d_desc(ch,
                                (uint8_t*)src + i*d3_src_pitch + j*d4_src_pitch,
                                (uint8_t*)dst + i*d3_dst_pitch + j*d4_dst_pitch,
                                d1_cnt,
                                d2_src_pitch,
                                d2_dst_pitch,
                                d2_cnt,
                                added,
                                /*isSys2Loc*/ 0, /*straddles*/ 0);
        }
#endif
    }
    schedule_descriptors(ch,
                         added);
}

void dma_4d_sys2loc_ch(int ch,
                       void *src, void *dst, size_t d1_cnt,
                       int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                       int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt,
                       int d4_src_pitch, int d4_dst_pitch, size_t d4_cnt)
{
    unsigned added = 0;
    for (unsigned j = 0; j < d4_cnt; ++j) {
#if (IDMA_USE_3D_DESC==1)    
        added = add_3d_desc64(ch,
                              (uint8_t*)src + j * d4_src_pitch, 
                              (uint8_t*)dst + j * d4_dst_pitch,
                              d1_cnt,
                              d2_src_pitch,
                              d2_dst_pitch,
                              d2_cnt,
                              d3_src_pitch,
                              d3_dst_pitch,
                              d3_cnt,
                              added,
                              /*isSys2Loc*/1, /*straddles*/0);
#else
        for (unsigned i = 0; i < d3_cnt; ++i) {
            added = add_2d_desc(ch,
                                (uint8_t*)src + i*d3_src_pitch + j*d4_src_pitch,
                                (uint8_t*)dst + i*d3_dst_pitch + j*d4_dst_pitch,
                                d1_cnt,
                                d2_src_pitch,
                                d2_dst_pitch,
                                d2_cnt,
                                added,
                                /*isSys2Loc*/ 1, /*straddles*/ 0);
        }
#endif
    }
    schedule_descriptors(ch,
                         added);
}

void dma_4d_loc2sys_dyn_ch(int ch, void *src, void *dst, size_t d1_cnt, 
                        int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                        int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt,
                        int d4_src_pitch, int d4_dst_pitch, size_t d4_cnt)
{
    for (unsigned j = 0; j < d4_cnt; ++j) {
        dma_3d_loc2sys_dyn_ch(ch,       
                           (uint8_t*)src + j*d4_src_pitch,
                           (uint8_t*)dst + j*d4_dst_pitch,
                           d1_cnt,                         
                           d2_src_pitch,                  
                           d2_dst_pitch,                  
                           d2_cnt,
                           d3_src_pitch,                   
                           d3_dst_pitch,               
                           d3_cnt);                    
    }                     
}

void dma_4d_sys2loc_dyn_ch(int ch, void *src, void *dst, size_t d1_cnt, 
                        int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                        int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt,
                        int d4_src_pitch, int d4_dst_pitch, size_t d4_cnt)
{
    for (unsigned j = 0; j < d4_cnt; ++j) {
        dma_3d_sys2loc_dyn_ch(ch,
                           (uint8_t*)src + j*d4_src_pitch,
                           (uint8_t*)dst + j*d4_dst_pitch, 
                           d1_cnt,
                           d2_src_pitch,                   
                           d2_dst_pitch,                   
                           d2_cnt,                         
                           d3_src_pitch,
                           d3_dst_pitch,                   
                           d3_cnt);                        
    }
} 

void dma_1d_loc2sys_straddles_ch(int ch,
                                 void *src, void *dst, size_t d1_cnt)
{
    unsigned added = add_2d_desc(ch,
                                 src, dst, d1_cnt,
                                 d1_cnt, d1_cnt, 1,
                                 /*added*/ 0, /*isSys2Loc*/ 0, /*straddles*/ 1);
    schedule_descriptors(ch,
                         added);
}

void dma_1d_sys2loc_straddles_ch(int ch,
                                 void *src, void *dst, size_t d1_cnt)
{
    unsigned added = add_2d_desc(ch,
                                 src, dst, d1_cnt,
                                 d1_cnt, d1_cnt, 1,
                                 /*added*/ 0, /*isSys2Loc*/ 1, /*straddles*/ 1);
    schedule_descriptors(ch,
                         added);
}

void dma_2d_loc2sys_straddles_ch(int ch,
                                 void *src, void *dst, size_t d1_cnt, int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt)
{
    unsigned added = add_2d_desc(ch,
                                 src, dst, d1_cnt,
                                 d2_src_pitch, d2_dst_pitch, d2_cnt,
                                 /*added*/ 0, /*isSys2Loc*/ 0, /*straddles*/ 1);
    schedule_descriptors(ch,
                         added);
}

void dma_2d_sys2loc_straddles_ch(int ch,
                                 void *src, void *dst, size_t d1_cnt, int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt)
{
    unsigned added = add_2d_desc(ch,
                                 src, dst, d1_cnt,
                                 d2_src_pitch, d2_dst_pitch, d2_cnt,
                                 /*added*/ 0, /*isSys2Loc*/ 1, /*straddles*/ 1);
    schedule_descriptors(ch,
                         added);
}

void dma_2d_loc2sys_dyn_straddles_ch(int ch,
                                     void *src, void *dst, size_t d1_cnt, int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt)
{
    unsigned added;
    if ((int)d1_cnt == d2_src_pitch && (int)d1_cnt == d2_dst_pitch) {
        added = add_2d_desc(ch,
                            src, dst, d1_cnt * d2_cnt, d1_cnt * d2_cnt, d1_cnt * d2_cnt, 1,
                            /*added*/ 0, /*isSys2Loc*/ 0, /*straddles*/ 1);
    } else {
        added = add_2d_desc(ch,
                            src, dst, d1_cnt,
                            d2_src_pitch, d2_dst_pitch, d2_cnt,
                            /*added*/ 0, /*isSys2Loc*/ 0, /*straddles*/ 1);
    }
    schedule_descriptors(ch,
                         added);
}

void dma_2d_sys2loc_dyn_straddles_ch(int ch,
                                     void *src, void *dst, size_t d1_cnt, int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt)
{
    unsigned added;
    if ((int)d1_cnt == d2_src_pitch && (int)d1_cnt == d2_dst_pitch) {
        added = add_2d_desc(ch,
                            src, dst, d1_cnt * d2_cnt, d1_cnt * d2_cnt, d1_cnt * d2_cnt, 1,
                            /*added*/ 0, /*isSys2Loc*/ 1, /*straddles*/ 1);
    } else {
        added = add_2d_desc(ch,
                            src, dst, d1_cnt,
                            d2_src_pitch, d2_dst_pitch, d2_cnt,
                            /*added*/ 0, /*isSys2Loc*/ 1, /*straddles*/ 1);
    }
    schedule_descriptors(ch,
                         added);
}

void dma_3d_loc2sys_straddles_ch(int ch,
                                 void *src, void *dst, size_t d1_cnt,
                                 int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                                 int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt)
{
    unsigned added = 0;
#if (IDMA_USE_3D_DESC==1)    
    added = add_3d_desc64(ch, 
                          (uint8_t*)src, 
                          (uint8_t*)dst,
                          d1_cnt,
                          d2_src_pitch,
                          d2_dst_pitch,
                          d2_cnt,
                          d3_src_pitch,
                          d3_dst_pitch,
                          d3_cnt,
                          added,
                          /*isSys2Loc*/0, /*straddles*/1);
#else
    for (unsigned i = 0; i < d3_cnt; ++i) {
        added = add_2d_desc(ch,
                            (uint8_t*)src + i*d3_src_pitch,
                            (uint8_t*)dst + i*d3_dst_pitch,
                            d1_cnt,
                            d2_src_pitch,
                            d2_dst_pitch,
                            d2_cnt,
                            added,
                            /*isSys2Loc*/ 0, /*straddles*/ 1);
    }
#endif
    schedule_descriptors(ch,
                         added);
}

void dma_3d_sys2loc_straddles_ch(int ch,
                                 void *src, void *dst, size_t d1_cnt,
                                 int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                                 int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt)
{
    unsigned added = 0;
#if (IDMA_USE_3D_DESC == 1)    
    added = add_3d_desc64(ch,
                          (uint8_t*)src, 
                          (uint8_t*)dst,
                          d1_cnt,
                          d2_src_pitch,
                          d2_dst_pitch,
                          d2_cnt,
                          d3_src_pitch,
                          d3_dst_pitch,
                          d3_cnt,
                          added,
                          /*isSys2Loc*/1, /*straddles*/1);
#else
    for (unsigned i = 0; i < d3_cnt; ++i) {
        added = add_2d_desc(ch,
                            (uint8_t*)src + i*d3_src_pitch,
                            (uint8_t*)dst + i*d3_dst_pitch,
                            d1_cnt,
                            d2_src_pitch,
                            d2_dst_pitch,
                            d2_cnt,
                            added,
                            /*isSys2Loc*/ 1, /*straddles*/ 1);
    }
#endif
    schedule_descriptors(ch,
                         added);
}

void dma_3d_loc2sys_dyn_straddles_ch(int ch,
                                     void *src, void *dst, size_t d1_cnt,
                                     int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                                     int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt)
{
    /* Fold dimensions if possible */
    if ((int)d1_cnt == d2_src_pitch
        && (int)d1_cnt == d2_dst_pitch) {
        dma_2d_loc2sys_dyn_straddles_ch(ch,
                                        src, dst, d1_cnt * d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt);
    } else if ((int)d2_cnt * d2_src_pitch == d3_src_pitch
               && (int)d2_cnt * d2_dst_pitch == d3_dst_pitch) {
        dma_2d_loc2sys_dyn_straddles_ch(ch,
                                        src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt * d3_cnt);
    } else {
        dma_3d_loc2sys_straddles_ch(ch,
                                    src, dst, d1_cnt,
                                    d2_src_pitch, d2_dst_pitch, d2_cnt,
                                    d3_src_pitch, d3_dst_pitch, d3_cnt);
    }
}

void dma_3d_sys2loc_dyn_straddles_ch(int ch,
                                     void *src, void *dst, size_t d1_cnt,
                                     int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                                     int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt)
{
    /* Fold dimensions if possible */
    if ((int)d1_cnt == d2_src_pitch
        && (int)d1_cnt == d2_dst_pitch) {
        dma_2d_sys2loc_dyn_straddles_ch(ch,
                                        src, dst, d1_cnt * d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt);
    } else if ((int)d2_cnt * d2_src_pitch == d3_src_pitch
               && (int)d2_cnt * d2_dst_pitch == d3_dst_pitch) {
        dma_2d_sys2loc_dyn_straddles_ch(ch,
                                        src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt * d3_cnt);
    } else {
        dma_3d_sys2loc_straddles_ch(ch,
                                    src, dst, d1_cnt,
                                    d2_src_pitch, d2_dst_pitch, d2_cnt,
                                    d3_src_pitch, d3_dst_pitch, d3_cnt);
    }
}

void dma_4d_loc2sys_straddles_ch(int ch,
                                 void *src, void *dst, size_t d1_cnt,
                                 int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                                 int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt,
                                 int d4_src_pitch, int d4_dst_pitch, size_t d4_cnt)
{
    unsigned added = 0;
    for (unsigned j = 0; j < d4_cnt; ++j) {
#if (IDMA_USE_3D_DESC == 1)    
        added = add_3d_desc64(ch,
                              (uint8_t*)src + j * d4_src_pitch, 
                              (uint8_t*)dst + j * d4_dst_pitch,
                              d1_cnt,
                              d2_src_pitch,
                              d2_dst_pitch,
                              d2_cnt,
                              d3_src_pitch,
                              d3_dst_pitch,
                              d3_cnt,
                              added,
                              /*isSys2Loc*/0, /*straddles*/1);
#else
        for (unsigned i = 0; i < d3_cnt; ++i) {
            added = add_2d_desc(ch,
                                (uint8_t*)src + i*d3_src_pitch + j*d4_src_pitch,
                                (uint8_t*)dst + i*d3_dst_pitch + j*d4_dst_pitch,
                                d1_cnt,
                                d2_src_pitch,
                                d2_dst_pitch,
                                d2_cnt,
                                added,
                                /*isSys2Loc*/ 0, /*straddles*/ 1);
        }
#endif
    }
    schedule_descriptors(ch,
                         added);
}

void dma_4d_sys2loc_straddles_ch(int ch,
                                 void *src, void *dst, size_t d1_cnt,
                                 int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                                 int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt,
                                 int d4_src_pitch, int d4_dst_pitch, size_t d4_cnt)
{
    unsigned added = 0;
    for (unsigned j = 0; j < d4_cnt; ++j) {
#if (IDMA_USE_3D_DESC == 1)    
        added = add_3d_desc64(ch,
                              (uint8_t*)src + j * d4_src_pitch, 
                              (uint8_t*)dst + j * d4_dst_pitch,
                              d1_cnt,
                              d2_src_pitch,
                              d2_dst_pitch,
                              d2_cnt,
                              d3_src_pitch,
                              d3_dst_pitch,
                              d3_cnt,
                              added,
                              /*isSys2Loc*/1, /*straddles*/1);
#else
        for (unsigned i = 0; i < d3_cnt; ++i) {
            added = add_2d_desc(ch,
                                (uint8_t*)src + i*d3_src_pitch + j*d4_src_pitch,
                                (uint8_t*)dst + i*d3_dst_pitch + j*d4_dst_pitch,
                                d1_cnt,
                                d2_src_pitch,
                                d2_dst_pitch,
                                d2_cnt,
                                added,
                                /*isSys2Loc*/ 1, /*straddles*/ 1);
        }
#endif
    }
    schedule_descriptors(ch,
                         added);
}

void compressed_dma_1d_sys2loc_ch(int ch,
                                  void *src, void *dst, const void *header, unsigned tile_idx, size_t dst_size)
{
    size_t compressed_data_size;
    void *compressed_src, *compressed_dst;
    compressed_transfer_info(src, dst, header, tile_idx, dst_size,
                             &compressed_src, &compressed_dst, &compressed_data_size);
    dma_1d_sys2loc_ch(ch,
                      compressed_src, compressed_dst, compressed_data_size);
}

void compressed_dma_1d_sys2loc_straddles_ch(int ch,
                                            void *src, void *dst, const void *header, unsigned tile_idx, size_t dst_size)
{
    size_t compressed_data_size;
    void *compressed_src, *compressed_dst;
    compressed_transfer_info(src, dst, header, tile_idx, dst_size,
                             &compressed_src, &compressed_dst, &compressed_data_size);
    dma_1d_sys2loc_straddles_ch(ch,
                                compressed_src, compressed_dst, compressed_data_size);
}

void dir_dma_1d_sys2loc_ch(int ch,
                           void *src, void *dst, const void *header, unsigned tile_idx, size_t size)
{
    void *sys_src;
    size_t sys_size;

    sys_src = (void *)dir_addr((const dir_entry_t *)header, src, tile_idx);
    sys_size = dir_size((const dir_entry_t *)header, tile_idx);

    dma_1d_sys2loc_ch(ch,
                      sys_src, dst, sys_size);
}

void dir_dma_1d_sys2loc_straddles_ch(int ch,
                                     void *src, void *dst, const void *header, unsigned tile_idx, size_t size)
{
    void *sys_src;
    size_t sys_size;

    sys_src = (void *)dir_addr((const dir_entry_t *)header, src, tile_idx);
    sys_size = dir_size((const dir_entry_t *)header, tile_idx);

    dma_1d_sys2loc_straddles_ch(ch,
                                sys_src, dst, sys_size);
}

void dir_dma_1d_loc2sys_ch(int ch,
                           void *src, void *dst, const void *header, unsigned tile_idx, size_t size)
{
    void *sys_dst;
    size_t sys_size;

    sys_dst = (void *)dir_addr((const dir_entry_t *)header, dst, tile_idx);
    sys_size = dir_size((const dir_entry_t *)header, tile_idx);

    dma_1d_sys2loc_ch(ch,
                      src, sys_dst, sys_size);
}

void dir_dma_1d_loc2sys_straddles_ch(int ch,
                                     void *src, void *dst, const void *header, unsigned tile_idx, size_t size)
{
    void *sys_dst;
    size_t sys_size;

    sys_dst = (void *)dir_addr((const dir_entry_t *)header, dst, tile_idx);
    sys_size = dir_size((const dir_entry_t *)header, tile_idx);

    dma_1d_sys2loc_straddles_ch(ch,
                                src, sys_dst, sys_size);
}


#if CSTUB_BUILD

static idma_status_t idma_add_2d_desc(idma_buffer_t *buff, void *dst, void *src, size_t d1_cnt, int dummy, size_t d2_cnt, int src_pitch, int dst_pitch)
{
    int ch = 0;

    cnnrt_tls *lptr = cnnrt_tls_get();

    for (; ch < XCHAL_IDMA_NUM_CHANNELS; ch++) {
        if (buff == lptr->_idmaObjBuff[ch])
            break;
    }
    if (ch == XCHAL_IDMA_NUM_CHANNELS) {
        return IDMA_ERR_BAD_DESC;
    }
    idma_desc_t *d = &buff[(dma_desc_free[ch]++) & (lptr->_idmaDescrTotal[ch]-1)];
    d->ch = ch;
    d->src = src;
    d->dst = dst;
    d->size = d1_cnt;
    d->rows = d2_cnt;
    d->src_pitch = src_pitch;
    d->dst_pitch = dst_pitch;
    return IDMA_OK;
}

static void do_memcpy(int ch, 
                      int number)
{
    if (number < 0)
        number = dma_desc_free[ch] - dma_desc_last[ch];

    cnnrt_tls *lptr = cnnrt_tls_get();

    while (number > 0 && dma_desc_last[ch] < dma_desc_free[ch]) {
        size_t i;
        idma_desc_t *d = &(lptr->_idmaObjBuff[ch] [(dma_desc_last[ch]++) & (lptr->_idmaDescrTotal[ch]-1)]);
#if (IDMA_USE_3D_DESC == 1)
        for (i=0; i<d->ntiles;i++) {
            for (j=0; j<d->rows; j++) {
                memcpy((uint8_t*)d->dst + i*d->dst_tile_pitch + j*d->dst_pitch, (uint8_t*)d->src + i*d->src_tile_pitch + j*d->src_pitch, d->size);
            }
        }
#else
        for (i=0; i<d->rows; i++)
            memcpy((uint8_t*)d->dst + i*d->dst_pitch, (uint8_t*)d->src + i*d->src_pitch, d->size);
#endif
    }
}

static idma_status_t cnnrt_idma_schedule_desc (int ch,
                                               unsigned to_schedule)
{
    if (_idmaCopyOnAdd)
        do_memcpy (ch,
                   to_schedule);
    return IDMA_OK;
}

static idma_status_t cnnrt_idma_buffer_status (int ch)
{
    return IDMA_OK;
}


void cnnrt_dma_mode(int mode)
{
    switch(mode) {
    case CNNRT_DMA_SLOW:
        _idmaCopyOnAdd = 0;
        break;
    case CNNRT_DMA_FAST:
        _idmaCopyOnAdd = 1;
        break;
    default:
        assert(0 && "Unexpected DMA mode");
    }
}

#endif /* #if CSTUB_BUILD */


XI_ERR_TYPE _cnnrt_wait_all()
{
    XI_ERROR_CHECKS() {}
#if XCHAL_HAVE_XNNE == 1
    XI_CHECK_RESULT(xnne_wait_all(_cnnrt_xnne_state));
#endif // XCHAL_HAVE_XNNE == 1
    XI_CHECK_RESULT(dma_barrier());
    return XI_ERROR_STATUS();
}


#if defined(__XTENSA__) && XCHAL_HAVE_XNNE == 1

static XI_ERR_TYPE _cnnrt_load_vpu()
{
    XI_ERROR_CHECKS() {}

#if !defined(RTL_DIAG) && !defined(POWER_DIAG)

    void *fw_data = NULL;
    void *fw_text = NULL;

    if (arena_init_two_banks_split(_cnnrt_firmware_data_size, sizeof(uint32_t), _cnnrt_firmware_text_size, sizeof(uint32_t)) == XI_ERR_OK
        && arena_alloc(&fw_data, /*bank*/ 0, /*size*/ _cnnrt_firmware_data_size, /*alignment*/ sizeof(uint32_t)) == XI_ERR_OK
        && arena_alloc(&fw_text, /*bank*/ 1, /*size*/ _cnnrt_firmware_text_size, /*alignment*/ sizeof(uint32_t)) == XI_ERR_OK)
    {
        dma_1d_sys2loc(_cnnrt_firmware_data, fw_data, _cnnrt_firmware_data_size);
        dma_1d_sys2loc(_cnnrt_firmware_text, fw_text, _cnnrt_firmware_text_size);
        XI_CHECK_RESULT(dma_barrier());
    } else {
        fw_data = _cnnrt_firmware_data;
        fw_text = _cnnrt_firmware_text;
    }

    /* Load VPU firmware and take it out of reset and runstall to get simulation going.
       XTSC deadlocks when VPU is in RunStall and other core tries to switch
       simulation between Turbo<->CA. */
    for (int j=0; j<XCHAL_XNNE_NUM_SBLKS; ++j) {
        xthal_xnne_load(j,
                        fw_text,
                        _cnnrt_firmware_text_size,
                        fw_data,
                        _cnnrt_firmware_data_size);
    }

    XI_CHECK_RESULT(arena_reset());

#else /* RTL_DIAG || POWER_DIAG */
    for (int j=0; j<XCHAL_XNNE_NUM_SBLKS; ++j) {

        // Current VPU Reset Sequence for RTL RefTB (Firmware is preloaded to vpu iram/dram at time=0)
        // VPUCtrl: De-assert both Reset & RunStall
        XNNE_WR(XNNE_TIEQ_VPU_CTRL(j), 0x0);

        // Ideal Reset Sequence (See xthal_xnne_load)
        // VPUCtrl: De-assert reset and assert RunStall
        //XNNE_WR(XNNE_TIEQ_VPU_CTRL(j), XNNE_VPU_CTRL_RUNSTALL);

        //VP6 loads firmware to VPU memory

        // VPUCtrl: De-assert RunStall
        //XNNE_WR(XNNE_TIEQ_VPU_CTRL(j), 0x0);
    }
#endif /* RTL_DIAG || POWER_DIAG */

#if XCHAL_XNNE_VERSION > 1
    uint32_t version;
	printf("_cnnrt_load_vpu(): loading\n");
    for (int j=0; j<XCHAL_XNNE_NUM_SBLKS; ++j) {
        uint32_t expected_version = XFW_FW_VERSION;
        /* Wait for VPU ready */
        do {
            version = XNNE_READ_FW_VERSION(j);
            XI_RUN_TIME_CHECK(!XNNE_READ_ERRORSTATUS(j), "XNNE is in error state", XI_ERR_BADARG);
        } while (version == 0);
        XI_RUN_TIME_CHECK(version == expected_version, "VPU firmware version mismatch", XI_ERR_BADARG);
    }
	printf("_cnnrt_load_vpu(): FW_VERSION=0x%x\n", version);
#endif

    return XI_ERROR_STATUS();
}
#endif /* __XTENSA__ && XCHAL_HAVE_XNNE */


#if XCHAL_HAVE_XNNE == 1
static XI_ERR_TYPE xnne_init()
{
    XI_ERROR_CHECKS() {}

#if defined(__XTENSA__)
    /* Make sure all SBLKs are enabled before we initialize them. */
    for (int i = 0; i < XCHAL_XNNE_NUM_SBLKS; ++i) {
        XNNE_WRITE_DISABLE(i, 0);
    }
    XI_CHECK_RESULT(_cnnrt_load_vpu());
#endif

    XI_CHECK_RESULT(arena_static_alloc((void**)&_cnnrt_xnne_state, sizeof(xnne_state_t), 64));
    xnne_state_init(_cnnrt_xnne_state);

#if defined(__XTENSA__)
    /* Check initialization status */
    for (int i = 0; i < XCHAL_XNNE_NUM_SBLKS; ++i) {
		XI_RUN_TIME_CHECK(!XNNE_READ_ERRORSTATUS(i), "XNNE is in error state", XI_ERR_BADARG);
    }
#endif

    return XI_ERROR_STATUS();
}
#endif

XI_ERR_TYPE cnnrt_init(const unsigned int dma_queue_size, const local_mem_info_t *mem_info)
{
    (void)dma_queue_size;

    XI_ERROR_CHECKS() {}
#if CSTUB_BUILD && CNNRT_PERF_LEVEL != CNNRT_PERF_LEVEL_NONE
    printf("Running CSTUBs simulation.\n");
#endif
    XI_CHECK_RESULT(arena_init(mem_info));

#ifdef XOS_BUILD
    // create TLS key for cnnrt mutable variable ptr
    xos_tls_create(&cnnrt_tls_key, NULL);
#endif
    XI_CHECK_RESULT(cnnrt_dma_init());
#ifndef XOS_BUILD
    // it is needed here because of XNNE use IDMA during initialization below.
    // TODO: multithreading XNNE variant.
    XI_CHECK_RESULT(cnnrt_dma_init_loops(dma_queue_size));
#endif

#if XCHAL_HAVE_XNNE == 1
    XI_CHECK_RESULT(xnne_init());
#endif // XCHAL_HAVE_XNNE == 1
    return XI_ERROR_STATUS();
}


void cnnrt_deinit()
{
    _cnnrt_wait_all();
#ifdef XOS_BUILD
    xos_tls_delete(cnnrt_tls_key);
#endif
    arena_deinit();
#if CSTUB_BUILD
    cnnrt_tls *lptr = cnnrt_tls_get();

    for (int i = 0; i < XCHAL_IDMA_NUM_CHANNELS; i++) {
#ifndef _MSC_VER
        free(lptr->_idmaObjBuff[i]);
#else //_MSC_VER
        _aligned_free(lptr->_idmaObjBuff[i]);
#endif //_MSC_VER
    }
#endif // CSTUB_BUILD
}



