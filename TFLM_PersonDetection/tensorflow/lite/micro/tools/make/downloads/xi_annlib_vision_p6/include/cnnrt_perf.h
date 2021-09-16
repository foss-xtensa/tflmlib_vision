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
#ifndef _CNNRT_PERF_H_INCLUDED_
#define _CNNRT_PERF_H_INCLUDED_

#include <stdint.h>
//#include "cnnrt_log.h"

/* Don't measure time, disable all reports. */
#define CNNRT_PERF_LEVEL_NONE     0
/* Measure and report only total cycles per run */
#define CNNRT_PERF_LEVEL_SUMMARY  1
/* Measure and report per-layer and per-kernel cycles */
#define CNNRT_PERF_LEVEL_ALL      2

/* Default performance reporting to all info */
#ifndef CNNRT_PERF_LEVEL
#  define CNNRT_PERF_LEVEL CNNRT_PERF_LEVEL_ALL
#endif

/* Default DSP frequency to 1000 MHz */
#ifndef DSP_FREQUENCY
#  define DSP_FREQUENCY 1000.0
#endif

/* Default to reporting without batching */
#ifndef BATCH_ADJUSTMENT
#  define BATCH_ADJUSTMENT 1
#endif

/* Peak MACs per cycle */
#if XCHAL_HAVE_VISION == 1 && XCHAL_VISION_TYPE == 7
#  define PEAK_MACS 512 /* VisionQ7 */
#elif XCHAL_HAVE_VISION == 1 && XCHAL_VISION_TYPE == 6
#  define PEAK_MACS 256 /* VisionP6/Q6 */
#elif XCHAL_HAVE_VISIONC == 1
#  define PEAK_MACS 1024 /* VisionC5 */
#else
#  define PEAK_MACS 256 /* default */
#endif

/* Control Instruction Set Simulator profiling clients.
   On by default. */
#ifndef CNNRT_PERF_CONTROL_ISS
#  define CNNRT_PERF_CONTROL_ISS 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Layer printing.
 */

extern void _cnnrt_perf_print_exec_info_summary(uint32_t _compute_cycles, uint64_t frequency,  uint32_t batch, uint64_t _t_macs, uint32_t num_cores, uint32_t macs_per_cycle);

extern void _cnnrt_perf_print_layer_info_header();
extern void _cnnrt_perf_print_mc_layer_info_header(int mycore);
extern void _cnnrt_perf_print_layer_info(const char *_t_name, uint64_t LayerStart, uint64_t LayereEnd, uint64_t _t_cycles_layer, uint64_t _t_macs, uint32_t num_cores, uint32_t macs_per_cycle);
extern void _cnnrt_perf_print_exec_info(uint64_t frequency,  uint32_t batch, uint64_t _t_macs, uint32_t num_cores,  uint32_t macs_per_cycle);

/*

 Time stamp functions and profiler instrumentation.

*/

#if CNNRT_PERF_LEVEL != CNNRT_PERF_LEVEL_NONE

#include <stdio.h>

#  if defined(__XTENSA__)

#    include <xtensa/sim.h>
#    include <xtensa/simcall.h>

#  if CNNRT_PERF_CONTROL_ISS

#    define INST_PROFILE_CLIENTS_ENABLE()   xt_iss_client_command ("all", "enable")
#    define INST_PROFILE_CLIENTS_DISABLE()  xt_iss_client_command ("all", "disable")

#if DISABLE_ISS_SWITCH
    #define DISABLE_ISS_SWITCH_MODE  1
#else 
    #define DISABLE_ISS_SWITCH_MODE  0
#endif

#    define INST_PROFILE_INIT()                         \
    if (cnnrt_is_master()) {                            \
        if(!DISABLE_ISS_SWITCH_MODE) {                  \
            xt_iss_switch_mode (XT_ISS_FUNCTIONAL);     \
        }                                               \
        INST_PROFILE_CLIENTS_DISABLE();                 \
    }

#    define INST_PROFILE_BEGIN()                            \
    if (cnnrt_is_master()) {                                \
        if(!DISABLE_ISS_SWITCH_MODE) {                      \
            xt_iss_switch_mode(XT_ISS_CYCLE_ACCURATE);      \
        }                                                   \
        INST_PROFILE_CLIENTS_ENABLE();                      \
    }
#    define INST_PROFILE_END()                          \
    if (cnnrt_is_master()) {                            \
        INST_PROFILE_CLIENTS_DISABLE();                 \
        if(!DISABLE_ISS_SWITCH_MODE) {                  \
            xt_iss_switch_mode (XT_ISS_FUNCTIONAL);     \
        }                                               \
    }

#    define INST_PROFILE_DUMP()                 \
    xt_iss_client_command("profile", "dump");   \
    xt_iss_client_command("summary", "dump");   \
    xt_iss_client_command("stackuse", "dump");

#    if XCHAL_HAVE_XNNE == 1
       /* These simcalls are availabale only with XNNE susbsystem */
#      define INST_XTSC_LAYER_START(index)   xt_iss_simcall(/* layer start */ 1, index, 0, 0, 0, 0)
#      define INST_XTSC_LAYER_END(index)     xt_iss_simcall(/* layer end */ 2, index, 0, 0, 0, 0)
#    else
#      define INST_XTSC_LAYER_START(index)
#      define INST_XTSC_LAYER_END(index)
#    endif

#  else

#    define INST_PROFILE_CLIENTS_ENABLE()
#    define INST_PROFILE_CLIENTS_DISABLE()

#    define INST_PROFILE_INIT()
#    define INST_PROFILE_BEGIN()
#    define INST_PROFILE_END()
#    define INST_PROFILE_DUMP()

#    define INST_XTSC_LAYER_START(index)
#    define INST_XTSC_LAYER_END(index)

#  endif

static inline uint32_t _time_stamp()
{
    uint32_t cyc_cnt;
    __asm__ __volatile__ ("rsr.ccount %0"
                          : "=a" (cyc_cnt) :: "memory"
                          );
    return cyc_cnt;
}

#  elif defined(__linux__) || defined(__ANDROID__) || defined(__QNX__) || defined(__CYGWIN__)

#    define INST_PROFILE_CLIENTS_ENABLE()
#    define INST_PROFILE_CLIENTS_DISABLE()

#    define INST_PROFILE_INIT()
#    define INST_PROFILE_BEGIN()
#    define INST_PROFILE_END()
#    define INST_PROFILE_DUMP()

#    define INST_XTSC_LAYER_START(index)
#    define INST_XTSC_LAYER_END(index)

/* On this platform just some number based on current time is returned, not a real cycles. */
#    include <time.h>
#    include <sys/time.h>

static inline uint32_t _time_stamp()
{
    struct timespec _t;
    clock_gettime(CLOCK_MONOTONIC, &_t);
    return (uint32_t)((uint64_t)_t.tv_nsec/1000 +_t.tv_sec*1000000UL);
}
#elif defined( _WIN32)
static int clock_gettime(int, struct timespec* spec)
{
  return 0;
}
static inline uint32_t _time_stamp()
{
  //struct timespec _t;
  //clock_gettime(1, &_t);
  return 0;//(uint32_t)((uint64_t)_t.tv_nsec / 1000 + _t.tv_sec * 1000000UL);
}
#  define INST_PROFILE_INIT()
#  define INST_PROFILE_BEGIN()
#  define INST_PROFILE_END()
#  define INST_PROFILE_DUMP()

#  else
#    error "implement _time_stamp()"
#  endif

#else /* CNNRT_PERF_LEVEL != CNNRT_PERF_LEVEL_NONE */

#  define INST_PROFILE_INIT()
#  define INST_PROFILE_BEGIN()
#  define INST_PROFILE_END()
#  define INST_PROFILE_DUMP()

#endif /* CNNRT_PERF_LEVEL != CNNRT_PERF_LEVEL_NONE */



/*
 * Indicator if reference XI kernels are used instead of optimized.
 */

#if ENABLE_REF_KERNELS
#  define INST_REFERENCE_STR " (reference kernels)"
#else
#  define INST_REFERENCE_STR ""
#endif

/*
 * Instrumentation macro
 */

#if CNNRT_PERF_LEVEL == CNNRT_PERF_LEVEL_NONE


/* Instrumentation macro for collecting network performance data. */
#  define INST_COMPUTE_BEGIN(name, myCore, ptr)
#  define INST_COMPUTE_END(name, macs, num_cores, macs_per_cycle, myCore, ptr)

/* Instrumentation macro for collecting per-layer performance data. */
#  define INST_LAYER_BEGIN(index, name)
#  define INST_LAYER_END(index, name, macs, num_cores, macs_per_cycle, myCore, ptr)
#  define INSERT_DUMMY_LAYER(myCore,ptr)
#  define RECORD_LAYER_OPTYPE(optype, ptr)

/* Instrumentation macro for collecting kernel call performance data. */
#  define INST_KERNEL_BEGIN()
#  define INST_KERNEL_END()

/* Instrumentation macro for collecting edge extension performance data. */
#  define INST_EDGE_EXTENSION_BEGIN()
#  define INST_EDGE_EXTENSION_END()


#elif CNNRT_PERF_LEVEL == CNNRT_PERF_LEVEL_SUMMARY


/* Instrumentation macro for collecting network performance data. */
#  define INST_COMPUTE_BEGIN(name, myCore, ptr)                              \
    uint32_t _compute_cycles = 0;                               \
    if (cnnrt_is_master()) {                                    \
        INST_PROFILE_CLIENTS_DISABLE();                         \
        printf ("RUNNING GRAPH%s...core = %d\n", INST_REFERENCE_STR, XT_RSR_PRID());    \
        _cnnrt_perf_idleWaitCycles = 0;                         \
        _cnnrt_perf_idmaMaxQueueSize = 0;                       \
        _cnnrt_perf_idmaQueueOverflow = 0;                      \
        _cnnrt_perf_totalKernelCycles = 0;                      \
        _cnnrt_perf_totalEdgeExtensionCycles = 0;               \
        _cnnrt_perf_totalCycles = 0;                            \
        INST_PROFILE_CLIENTS_ENABLE();                          \
        _compute_cycles = _time_stamp();                        \
    }

#  define INST_COMPUTE_END(name, macs, num_cores, macs_per_cycle, myCore, ptr)       \
    if (cnnrt_is_master()) {                                            \
        _compute_cycles = _time_stamp() - _compute_cycles;              \
        INST_PROFILE_CLIENTS_DISABLE();                                 \
        if (!ptr)                                                        \
            _cnnrt_perf_print_exec_info_summary(_compute_cycles, DSP_FREQUENCY, BATCH_ADJUSTMENT, (macs), num_cores, macs_per_cycle); \
        INST_PROFILE_CLIENTS_ENABLE();                                  \
    }

/* Instrumentation macro for collecting per-layer performance data. */
#  define INST_LAYER_BEGIN(index, name) \
  if (cnnrt_is_master()) {                                              \
      INST_XTSC_LAYER_START(index);                                     \
  }                                                                     \
  {                                                                     \
    uint32_t _t_cycles_layer = _time_stamp();                           \


#  define INST_LAYER_END(index, name, macs, num_cores, macs_per_cycle, myCore,ptr) \
    if (cnnrt_is_master()) {                                            \
        _cnnrt_wait_all();                                              \
        if (ptr) {                                                      \
            *(ptr+LAYER_START)=_t_cycles_layer;                         \
            *(ptr+LAYER_END)=_time_stamp();                             \
        }                                                               \
        _t_cycles_layer = _time_stamp() - _t_cycles_layer;              \
        INST_XTSC_LAYER_END(index);                                     \
        _cnnrt_perf_totalCycles += _t_cycles_layer;                     \
    }                                                                   \
  }

#define INSERT_DUMMY_LAYER(myCore,ptr) \
    if (ptr && cnnrt_is_master()) {                                     \
        unsigned long long _t_tmp_cyc = _time_stamp();                  \
        *(ptr+LAYER_START)=_t_tmp_cyc;                              \
        *(ptr+LAYER_END)=_t_tmp_cyc;                                \
        _t_tmp_cyc = 0;                                                 \
        _cnnrt_perf_totalCycles += _t_tmp_cyc;                          \
        *(ptr+TOT_CYC) = _t_tmp_cyc;                                \
        *(ptr+KERNEL_CYC) =  0;                                     \
        *(ptr+EDGE_CYC) =  0;                                       \
        *(ptr+IDLE_CYC) =   0;                                      \
        *((double *)(ptr+MAC_PER_CYC)) = 0;                         \
        *((double *)(ptr+MAC_PERCENT))= 0;                          \
        *(ptr+TOT_MAC) = 0;                                         \
        *(ptr+DMA_QUEUE) = 0;                                       \
    }

#  define RECORD_LAYER_OPTYPE(optype, ptr)  \
    if (ptr && cnnrt_is_master())                                    \
        *(ptr+OP_IDX) = optype;

/* Instrumentation macro for collecting kernel call performance data. */
#  define INST_KERNEL_BEGIN()                   \
    if (cnnrt_is_master()) {                    \
        _cnnrt_perf_temp = _time_stamp();       \
    }

#  define INST_KERNEL_END()                                             \
    if (cnnrt_is_master()) {                                         \
        _cnnrt_perf_totalKernelCycles += _time_stamp() - _cnnrt_perf_temp; \
    }

/* Instrumentation macro for collecting edge extension performance data. */
#  define INST_EDGE_EXTENSION_BEGIN()           \
    if (cnnrt_is_master()) {                    \
        _cnnrt_perf_temp = _time_stamp();       \
    }

#  define INST_EDGE_EXTENSION_END()                                     \
    if (cnnrt_is_master()) {                                            \
        _cnnrt_perf_totalEdgeExtensionCycles += _time_stamp() - _cnnrt_perf_temp; \
    }


#elif CNNRT_PERF_LEVEL == CNNRT_PERF_LEVEL_ALL


/* Instrumentation macro for collecting network performance data. */
#  define INST_COMPUTE_BEGIN(name, myCore, ptr)                              \
    if (cnnrt_is_master()) {                                    \
        INST_PROFILE_CLIENTS_DISABLE();                         \
        _cnnrt_perf_totalCycles = 0;                            \
        _cnnrt_perf_totalKernelCycles = 0;                      \
        _cnnrt_perf_totalEdgeExtensionCycles = 0;               \
        _cnnrt_perf_totalidleWaitCycles = 0;                    \
        printf ("RUNNING GRAPH%s...core = %d\n", INST_REFERENCE_STR, XT_RSR_PRID());    \
        if (!ptr)                                               \
            _cnnrt_perf_print_mc_layer_info_header(myCore);        \
        INST_PROFILE_CLIENTS_ENABLE();                          \
    }

#  define INST_COMPUTE_END(name, macs, num_cores, macs_per_cycle, myCore, ptr)       \
    if (cnnrt_is_master()) {                                            \
        INST_PROFILE_CLIENTS_DISABLE();                                 \
        if (!ptr)                                                        \
            _cnnrt_perf_print_exec_info(DSP_FREQUENCY, BATCH_ADJUSTMENT, macs, num_cores, macs_per_cycle);  \
        INST_PROFILE_CLIENTS_ENABLE();                                  \
    }

/* Instrumentation macro for collecting per-layer performance data. */
#  define INST_LAYER_BEGIN(index, name)                      \
    if (cnnrt_is_master()) {                                \
        INST_PROFILE_CLIENTS_DISABLE();                     \
        _cnnrt_perf_currentEdgeExtensionCycles = 0;         \
        _cnnrt_perf_currentKernelCycles = 0;                \
        _cnnrt_perf_idleWaitCycles = 0;                     \
        _cnnrt_perf_idmaMaxQueueSize = 0;                   \
        _cnnrt_perf_idmaQueueOverflow = 0;                  \
        INST_PROFILE_CLIENTS_ENABLE();                      \
        INST_XTSC_LAYER_START(index);                       \
    }                                                       \
    {                                                       \
    uint32_t _t_cycles_layer = _time_stamp();               \


#  define INST_LAYER_END(index, name, macs, num_cores, macs_per_cycle, myCore, ptr)  \
    if (cnnrt_is_master()) {                                            \
        _cnnrt_wait_all();                                              \
        unsigned long long tmp_cyc = _time_stamp();                  \
        unsigned long long LayerStart = _t_cycles_layer;             \
        if (ptr) {                                                      \
            *(ptr+LAYER_START) = _t_cycles_layer;                             \
            *(ptr+LAYER_END)   = tmp_cyc;                                \
        }                                                               \
        _t_cycles_layer = tmp_cyc - _t_cycles_layer;              \
        INST_XTSC_LAYER_END(index);                                     \
        INST_PROFILE_CLIENTS_DISABLE();                                 \
        _cnnrt_perf_totalCycles += _t_cycles_layer;                     \
        _cnnrt_perf_totalEdgeExtensionCycles += _cnnrt_perf_currentEdgeExtensionCycles; \
        _cnnrt_perf_totalKernelCycles += _cnnrt_perf_currentKernelCycles; \
        _cnnrt_perf_totalidleWaitCycles += _cnnrt_perf_idleWaitCycles;  \
        if (ptr) {                                                      \
            *(ptr+TOT_CYC) = _t_cycles_layer;                       \
            *(ptr+KERNEL_CYC) =  (unsigned long long)_cnnrt_perf_currentKernelCycles;  \
            *(ptr+EDGE_CYC) =  (unsigned long long)_cnnrt_perf_currentEdgeExtensionCycles; \
            *(ptr+IDLE_CYC) =   (unsigned long long)_cnnrt_perf_idleWaitCycles;         \
            *((double *)(ptr+MAC_PER_CYC)) = (double) macs/(_t_cycles_layer?_t_cycles_layer:1); \
            *((double *)(ptr+MAC_PERCENT))= (double)((double)(macs)/((macs_per_cycle ? macs_per_cycle : PEAK_MACS)*num_cores))*100/(_t_cycles_layer ? _t_cycles_layer : 1);                   \
            *(ptr+TOT_MAC) = macs;                                          \
            *(ptr+DMA_QUEUE) = (unsigned long) _cnnrt_perf_idmaMaxQueueSize;     \
        } else                                                          \
            _cnnrt_perf_print_layer_info(name, LayerStart, tmp_cyc, _t_cycles_layer, macs, num_cores, macs_per_cycle);   \
        INST_PROFILE_CLIENTS_ENABLE();                                  \
    }}

#define INSERT_DUMMY_LAYER(myCore,ptr) \
    if (ptr && cnnrt_is_master()) {                                     \
        unsigned long long _t_tmp_cyc = _time_stamp();                               \
        *(ptr+LAYER_START)=_t_tmp_cyc;                            \
        *(ptr+LAYER_END)=_t_tmp_cyc;                              \
        _t_tmp_cyc = 0;                                           \
        _cnnrt_perf_totalCycles += _t_tmp_cyc;                    \
        *(ptr+TOT_CYC) = _t_tmp_cyc;                              \
        *(ptr+KERNEL_CYC) =  0;                                        \
        *(ptr+EDGE_CYC) =  0;                                          \
        *(ptr+IDLE_CYC) =   0;                                         \
        *((double *)(ptr+MAC_PER_CYC)) = 0;                            \
        *((double *)(ptr+MAC_PERCENT))= 0;\
        *(ptr+TOT_MAC) = 0;                                          \
        *(ptr+DMA_QUEUE) = 0;     \
    }

#  define RECORD_LAYER_OPTYPE(optype, ptr)  \
    if (ptr && cnnrt_is_master())                                    \
        *(ptr+OP_IDX) = optype;

/* Instrumentation macro for collecting kernel call performance data. */
#  define INST_KERNEL_BEGIN()                   \
    if (cnnrt_is_master()) {                 \
        _cnnrt_perf_temp = _time_stamp();       \
    }

#  define INST_KERNEL_END()                                             \
    if (cnnrt_is_master()) {                                         \
        _cnnrt_perf_currentKernelCycles += _time_stamp() - _cnnrt_perf_temp; \
    }

/* Instrumentation macro for collecting edge extension performance data. */
#  define INST_EDGE_EXTENSION_BEGIN()           \
    if (cnnrt_is_master()) {                 \
        _cnnrt_perf_temp = _time_stamp();       \
    }

#  define INST_EDGE_EXTENSION_END()                                     \
    if (cnnrt_is_master()) {                                         \
        _cnnrt_perf_currentEdgeExtensionCycles += _time_stamp() - _cnnrt_perf_temp; \
    }

#endif /* CNNRT_PERF_LEVEL dispatch */

/* Accumulator for layer cycles, contauns sum of cycles for all layers. */
extern uint64_t _cnnrt_perf_totalCycles;
/* Accumulator for kernel cycles over all layers, contauns sum of cycles
   spent in kernel functions without DMA and tiling harness. */
extern uint64_t _cnnrt_perf_totalKernelCycles;
/* Accumulator for cycles spent in edge extension function in all layers. */
extern uint64_t _cnnrt_perf_totalEdgeExtensionCycles;
/* Accumulator for cycles spent in kernel in current layer. */
extern uint64_t _cnnrt_perf_currentKernelCycles;
/* Accumulator for cycles spent in edge extension function in current layer. */
extern uint64_t _cnnrt_perf_currentEdgeExtensionCycles;
/* Temp placeholder for cycle counter. */
extern uint32_t _cnnrt_perf_temp;

/* Accumulator for full network cycles spent in WAIT for DMA/XNNE completion */
extern uint64_t _cnnrt_perf_totalidleWaitCycles;
/* Number of cycles spent in WAIT for DMA/XNNE completion */
extern uint64_t _cnnrt_perf_idleWaitCycles;
/* Max queue size observed. */
extern uint8_t _cnnrt_perf_idmaMaxQueueSize;
/* Was queue overflow detected or not */
extern uint8_t _cnnrt_perf_idmaQueueOverflow;

#ifdef __cplusplus
}
#endif

#endif /* _CNNRT_PERF_H_INCLUDED_ */
