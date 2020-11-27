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
#ifndef _CNNRT_DEBUG_H_INCLUDED_
#define _CNNRT_DEBUG_H_INCLUDED_

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* XI kernel function name wrapper.

   The purpose of this wrapper is to redirect calls to reference library if
   ENABLE_REF_KERNELS is defined and non zero.
*/

#if ENABLE_REF_KERNELS
#  define XI_KERNEL_NAME(function_name) function_name ## _ref
#else
#  define XI_KERNEL_NAME(function_name) function_name
#endif

/* Debug printing of output arrays.

   name: layer name
   buffer: buffer pointer
   dim: dimension size in elements
   stride: stride between dimension elements in elements
*/

#if ENABLE_TRACE_OUTPUTS
#  if ENABLE_TRACE_STATS
#    define INST_LAYER_OUTPUT_1D_U8(name, buffer, dim0, stride0) \
      if (cnnrt_is_master()) { print_stats_U8((#name ":" #buffer), (buffer), (dim0), (stride0), 1, 0, 1, 0, 1, 0); }
#    define INST_LAYER_OUTPUT_2D_U8(name, buffer, dim0, stride0, dim1, stride1) \
      if (cnnrt_is_master()) { print_stats_U8((#name ":" #buffer), (buffer), (dim0), (stride0), (dim1), (stride1), 1, 0, 1, 0); }
#    define INST_LAYER_OUTPUT_3D_U8(name, buffer, dim0, stride0, dim1, stride1, dim2, stride2) \
      if (cnnrt_is_master()) { print_stats_U8((#name ":" #buffer), (buffer), (dim0), (stride0), (dim1), (stride1), (dim2), (stride2), 1, 0); }
#    define INST_LAYER_OUTPUT_4D_U8(name, buffer, dim0, stride0, dim1, stride1, dim2, stride2, dim3, stride3) \
      if (cnnrt_is_master()) { print_stats_U8((#name ":" #buffer), (buffer), (dim0), (stride0), (dim1), (stride1), (dim2), (stride2), (dim3), (stride3)); }

#    define INST_LAYER_OUTPUT_1D_U16(name, buffer, dim0, stride0) \
      if (cnnrt_is_master()) { print_stats_U16((#name ":" #buffer), (buffer), (dim0), (stride0), 1, 0, 1, 0, 1, 0); }
#    define INST_LAYER_OUTPUT_2D_U16(name, buffer, dim0, stride0, dim1, stride1) \
      if (cnnrt_is_master()) { print_stats_U16((#name ":" #buffer), (buffer), (dim0), (stride0), (dim1), (stride1), 1, 0, 1, 0); }
#    define INST_LAYER_OUTPUT_3D_U16(name, buffer, dim0, stride0, dim1, stride1, dim2, stride2) \
      if (cnnrt_is_master()) { print_stats_U16((#name ":" #buffer), (buffer), (dim0), (stride0), (dim1), (stride1), (dim2), (stride2), 1, 0); }
#    define INST_LAYER_OUTPUT_4D_U16(name, buffer, dim0, stride0, dim1, stride1, dim2, stride2, dim3, stride3) \
      if (cnnrt_is_master()) { print_stats_U16((#name ":" #buffer), (buffer), (dim0), (stride0), (dim1), (stride1), (dim2), (stride2), (dim3), (stride3)); }

#    define INST_LAYER_OUTPUT_1D_U32(name, buffer, dim0, stride0) \
      if (cnnrt_is_master()) { print_stats_U32((#name ":" #buffer), (buffer), (dim0), (stride0), 1, 0, 1, 0, 1, 0); }
#    define INST_LAYER_OUTPUT_2D_U32(name, buffer, dim0, stride0, dim1, stride1) \
      if (cnnrt_is_master()) { print_stats_U32((#name ":" #buffer), (buffer), (dim0), (stride0), (dim1), (stride1), 1, 0, 1, 0); }
#    define INST_LAYER_OUTPUT_3D_U32(name, buffer, dim0, stride0, dim1, stride1, dim2, stride2) \
      if (cnnrt_is_master()) { print_stats_U32((#name ":" #buffer), (buffer), (dim0), (stride0), (dim1), (stride1), (dim2), (stride2), 1, 0); }
#    define INST_LAYER_OUTPUT_4D_U32(name, buffer, dim0, stride0, dim1, stride1, dim2, stride2, dim3, stride3) \
      if (cnnrt_is_master()) { print_stats_U32((#name ":" #buffer), (buffer), (dim0), (stride0), (dim1), (stride1), (dim2), (stride2), (dim3), (stride3)); }

#    define INST_LAYER_OUTPUT_1D_F32(name, buffer, dim0, stride0) \
      if (cnnrt_is_master()) { print_stats_FLOAT((#name ":" #buffer), (buffer), (dim0), (stride0), 1, 0, 1, 0, 1, 0); }
#    define INST_LAYER_OUTPUT_2D_F32(name, buffer, dim0, stride0, dim1, stride1) \
      if (cnnrt_is_master()) { print_stats_FLOAT((#name ":" #buffer), (buffer), (dim0), (stride0), (dim1), (stride1), 1, 0, 1, 0); }
#    define INST_LAYER_OUTPUT_3D_F32(name, buffer, dim0, stride0, dim1, stride1, dim2, stride2) \
      if (cnnrt_is_master()) { print_stats_FLOAT((#name ":" #buffer), (buffer), (dim0), (stride0), (dim1), (stride1), (dim2), (stride2), 1, 0); }
#    define INST_LAYER_OUTPUT_4D_F32(name, buffer, dim0, stride0, dim1, stride1, dim2, stride2, dim3, stride3) \
      if (cnnrt_is_master()) { print_stats_FLOAT((#name ":" #buffer), (buffer), (dim0), (stride0), (dim1), (stride1), (dim2), (stride2), (dim3), (stride3)); }
#  else
#    define INST_LAYER_OUTPUT_1D_U8(name, buffer, dim0, stride0) \
      if (cnnrt_is_master()) { dump_buffer_U8(#name, #buffer, (buffer), (dim0), (stride0), 1, 0, 1, 0, 1, 0); }
#    define INST_LAYER_OUTPUT_2D_U8(name, buffer, dim0, stride0, dim1, stride1) \
      if (cnnrt_is_master()) { dump_buffer_U8(#name, #buffer, (buffer), (dim0), (stride0), (dim1), (stride1), 1, 0, 1, 0); }
#    define INST_LAYER_OUTPUT_3D_U8(name, buffer, dim0, stride0, dim1, stride1, dim2, stride2) \
      if (cnnrt_is_master()) { dump_buffer_U8(#name, #buffer, (buffer), (dim0), (stride0), (dim1), (stride1), (dim2), (stride2), 1, 0); }
#    define INST_LAYER_OUTPUT_4D_U8(name, buffer, dim0, stride0, dim1, stride1, dim2, stride2, dim3, stride3) \
      if (cnnrt_is_master()) { dump_buffer_U8(#name, #buffer, (buffer), (dim0), (stride0), (dim1), (stride1), (dim2), (stride2), (dim3), (stride3)); }

#    define INST_LAYER_OUTPUT_1D_U16(name, buffer, dim0, stride0) \
      if (cnnrt_is_master()) { dump_buffer_U16(#name, #buffer, (buffer), (dim0), (stride0), 1, 0, 1, 0, 1, 0); }
#    define INST_LAYER_OUTPUT_2D_U16(name, buffer, dim0, stride0, dim1, stride1) \
      if (cnnrt_is_master()) { dump_buffer_U16(#name, #buffer, (buffer), (dim0), (stride0), (dim1), (stride1), 1, 0, 1, 0); }
#    define INST_LAYER_OUTPUT_3D_U16(name, buffer, dim0, stride0, dim1, stride1, dim2, stride2) \
      if (cnnrt_is_master()) { dump_buffer_U16(#name, #buffer, (buffer), (dim0), (stride0), (dim1), (stride1), (dim2), (stride2), 1, 0); }
#    define INST_LAYER_OUTPUT_4D_U16(name, buffer, dim0, stride0, dim1, stride1, dim2, stride2, dim3, stride3) \
      if (cnnrt_is_master()) { dump_buffer_U16(#name, #buffer, (buffer), (dim0), (stride0), (dim1), (stride1), (dim2), (stride2), (dim3), (stride3)); }

#    define INST_LAYER_OUTPUT_1D_U32(name, buffer, dim0, stride0) \
      if (cnnrt_is_master()) { dump_buffer_U32(#name, #buffer, (buffer), (dim0), (stride0), 1, 0, 1, 0, 1, 0); }
#    define INST_LAYER_OUTPUT_2D_U32(name, buffer, dim0, stride0, dim1, stride1) \
      if (cnnrt_is_master()) { dump_buffer_U32(#name, #buffer, (buffer), (dim0), (stride0), (dim1), (stride1), 1, 0, 1, 0); }
#    define INST_LAYER_OUTPUT_3D_U32(name, buffer, dim0, stride0, dim1, stride1, dim2, stride2) \
      if (cnnrt_is_master()) { dump_buffer_U32(#name, #buffer, (buffer), (dim0), (stride0), (dim1), (stride1), (dim2), (stride2), 1, 0); }
#    define INST_LAYER_OUTPUT_4D_U32(name, buffer, dim0, stride0, dim1, stride1, dim2, stride2, dim3, stride3) \
      if (cnnrt_is_master()) { dump_buffer_U32(#name, #buffer, (buffer), (dim0), (stride0), (dim1), (stride1), (dim2), (stride2), (dim3), (stride3)); }

#    define INST_LAYER_OUTPUT_1D_F32(name, buffer, dim0, stride0) \
      if (cnnrt_is_master()) { dump_buffer_FLOAT(#name, #buffer, (buffer), (dim0), (stride0), 1, 0, 1, 0, 1, 0); }
#    define INST_LAYER_OUTPUT_2D_F32(name, buffer, dim0, stride0, dim1, stride1) \
      if (cnnrt_is_master()) { dump_buffer_FLOAT(#name, #buffer, (buffer), (dim0), (stride0), (dim1), (stride1), 1, 0, 1, 0); }
#    define INST_LAYER_OUTPUT_3D_F32(name, buffer, dim0, stride0, dim1, stride1, dim2, stride2) \
      if (cnnrt_is_master()) { dump_buffer_FLOAT(#name, #buffer, (buffer), (dim0), (stride0), (dim1), (stride1), (dim2), (stride2), 1, 0); }
#    define INST_LAYER_OUTPUT_4D_F32(name, buffer, dim0, stride0, dim1, stride1, dim2, stride2, dim3, stride3) \
      if (cnnrt_is_master()) { dump_buffer_FLOAT(#name, #buffer, (buffer), (dim0), (stride0), (dim1), (stride1), (dim2), (stride2), (dim3), (stride3)); }
#  endif

/* Macro wrapper for switching into fast functional simulation if dumping is enabled
   and switching back to cycle accurate mode. Used only inside of optimized compute function.
*/
#  define INST_COMPUTE_TURBO_ISS(stmts)         \
    INST_PROFILE_END();                         \
    stmts;                                      \
    INST_PROFILE_BEGIN();

#  define INST_COMPUTE_TURBO_ISS2(stmts)        \
    if (ENABLE_TRACE_OUTPUTS > 1) {             \
        INST_PROFILE_END();                     \
        stmts;                                  \
        INST_PROFILE_BEGIN();                   \
    }

#define IF_MAINTAIN_MEM_CONSISTENCY(stmts)                              \
    if (CNNRT_PERF_LEVEL == CNNRT_PERF_LEVEL_ALL || ENABLE_TRACE_OUTPUTS > 1) { \
        stmts;                                                          \
    }

#else
#  define INST_LAYER_OUTPUT_1D_U8(name, buffer, dim0, stride0)
#  define INST_LAYER_OUTPUT_2D_U8(name, buffer, dim0, stride0, dim1, stride1)
#  define INST_LAYER_OUTPUT_3D_U8(name, buffer, dim0, stride0, dim1, stride1, dim2, stride2)
#  define INST_LAYER_OUTPUT_4D_U8(name, buffer, dim0, stride0, dim1, stride1, dim2, stride2, dim3, stride3)

#  define INST_LAYER_OUTPUT_1D_U16(name, buffer, dim0, stride0)
#  define INST_LAYER_OUTPUT_2D_U16(name, buffer, dim0, stride0, dim1, stride1)
#  define INST_LAYER_OUTPUT_3D_U16(name, buffer, dim0, stride0, dim1, stride1, dim2, stride2)
#  define INST_LAYER_OUTPUT_4D_U16(name, buffer, dim0, stride0, dim1, stride1, dim2, stride2, dim3, stride3)

#  define INST_LAYER_OUTPUT_1D_U32(name, buffer, dim0, stride0)
#  define INST_LAYER_OUTPUT_2D_U32(name, buffer, dim0, stride0, dim1, stride1)
#  define INST_LAYER_OUTPUT_3D_U32(name, buffer, dim0, stride0, dim1, stride1, dim2, stride2)
#  define INST_LAYER_OUTPUT_4D_U32(name, buffer, dim0, stride0, dim1, stride1, dim2, stride2, dim3, stride3)

#  define INST_LAYER_OUTPUT_1D_F32(name, buffer, dim0, stride0)
#  define INST_LAYER_OUTPUT_2D_F32(name, buffer, dim0, stride0, dim1, stride1)
#  define INST_LAYER_OUTPUT_3D_F32(name, buffer, dim0, stride0, dim1, stride1, dim2, stride2)
#  define INST_LAYER_OUTPUT_4D_F32(name, buffer, dim0, stride0, dim1, stride1, dim2, stride2, dim3, stride3)

#  define INST_COMPUTE_TURBO_ISS(stmts) stmts
#  define INST_COMPUTE_TURBO_ISS2(stmts) stmts

#define IF_MAINTAIN_MEM_CONSISTENCY(stmts)                              \
    if (CNNRT_PERF_LEVEL == CNNRT_PERF_LEVEL_ALL) {                     \
        stmts;                                                          \
    }

#endif

#define INST_LAYER_OUTPUT_1D_S8 INST_LAYER_OUTPUT_1D_U8
#define INST_LAYER_OUTPUT_2D_S8 INST_LAYER_OUTPUT_2D_U8
#define INST_LAYER_OUTPUT_3D_S8 INST_LAYER_OUTPUT_3D_U8
#define INST_LAYER_OUTPUT_4D_S8 INST_LAYER_OUTPUT_4D_U8

#define INST_LAYER_OUTPUT_1D_S16 INST_LAYER_OUTPUT_1D_U16
#define INST_LAYER_OUTPUT_2D_S16 INST_LAYER_OUTPUT_2D_U16
#define INST_LAYER_OUTPUT_3D_S16 INST_LAYER_OUTPUT_3D_U16
#define INST_LAYER_OUTPUT_4D_S16 INST_LAYER_OUTPUT_4D_U16

#define INST_LAYER_OUTPUT_1D_S32 INST_LAYER_OUTPUT_1D_U32
#define INST_LAYER_OUTPUT_2D_S32 INST_LAYER_OUTPUT_2D_U32
#define INST_LAYER_OUTPUT_3D_S32 INST_LAYER_OUTPUT_3D_U32
#define INST_LAYER_OUTPUT_4D_S32 INST_LAYER_OUTPUT_4D_U32


/* Print buffer content in hex.

   Args:
       name: buffer name to print in the header. Function accepts NULL.
       buffer: pointer to data.
       size: buffer size in bytes.
*/

void dump_buffer(const char *name, const void *buffer, unsigned size);

#define DUMP_ARRAY(name) \
    dump_buffer(#name, (const void *)name, sizeof(name))

/* Print multidimensional U8 tensor in hex.

   Element is located as ((uint8_t*)buffer)[coord0*stride0 + coord1*stride1 + ...]
   where 0 <= coord0 < dim0, 0 <= coord1 < dim1 etc.

   Args:
       name: buffer name to print in the header. Function accepts NULL.
       suffix: additional buffer name text to print.
       buffer: pointer to data.
       dim: dimension size in elements.
       stride: stride in associated dimension in elements.
*/

void dump_buffer_U8(const char *name, const char *suffix, const void *buffer, unsigned dim0, unsigned stride0,
                    unsigned dim1, unsigned stride1, unsigned dim2, unsigned stride2,
                    unsigned dim3, unsigned stride3);

/* Print multidimensional U16 tensor in hex.

   Element is located as ((uint16_t*)buffer)[coord0*stride0 + coord1*stride1 + ...]
   where 0 <= coord0 < dim0, 0 <= coord1 < dim1 etc.

   Args:
       name: buffer name to print in the header. Function accepts NULL.
       suffix: additional buffer name text to print.
       buffer: pointer to data.
       dim: dimension size in elements.
       stride: stride in associated dimension in elements.
*/

void dump_buffer_U16(const char *name, const char *suffix, const void *buffer, unsigned dim0, unsigned stride0,
                     unsigned dim1, unsigned stride1, unsigned dim2, unsigned stride2,
                     unsigned dim3, unsigned stride3);

/* Print multidimensional U32 tensor in hex.

   Element is located as ((uint32_t*)buffer)[coord0*stride0 + coord1*stride1 + ...]
   where 0 <= coord0 < dim0, 0 <= coord1 < dim1 etc.

   Args:
       name: buffer name to print in the header. Function accepts NULL.
       suffix: additional buffer name text to print.
       buffer: pointer to data.
       dim: dimension size in elements.
       stride: stride in associated dimension in elements.
*/

void dump_buffer_U32(const char *name, const char *suffix, const void *buffer, unsigned dim0, unsigned stride0,
                     unsigned dim1, unsigned stride1, unsigned dim2, unsigned stride2,
                     unsigned dim3, unsigned stride3);

/* Print multidimensional FLOAT tensor.

   Element is located as ((float*)buffer)[coord0*stride0 + coord1*stride1 + ...]
   where 0 <= coord0 < dim0, 0 <= coord1 < dim1 etc.

   Args:
       name: buffer name to print in the header. Function accepts NULL.
       suffix: additional buffer name text to print.
       buffer: pointer to data.
       dim: dimension size in elements.
       stride: stride in associated dimension in elements.
*/

void dump_buffer_FLOAT(const char *name, const char *suffix, const void *buffer, unsigned dim0, unsigned stride0,
                     unsigned dim1, unsigned stride1, unsigned dim2, unsigned stride2,
                     unsigned dim3, unsigned stride3);

/* Print multidimensional U8 tensor statistics

   Element is located as ((uint8_t*)buffer)[coord0*stride0 + coord1*stride1 + ...]
   where 0 <= coord0 < dim0, 0 <= coord1 < dim1 etc.

   Args:
       name: buffer name to print in the header. Function accepts NULL.
       buffer: pointer to data.
       dim: dimension size in elements.
       stride: stride in associated dimension in elements.
*/
void print_stats_U8(const char *name, const void *buffer, unsigned dim0, unsigned stride0,
                    unsigned dim1, unsigned stride1, unsigned dim2, unsigned stride2,
                    unsigned dim3, unsigned stride3);

/* Print multidimensional U16 tensor statistcs.

   Element is located as ((uint16_t*)buffer)[coord0*stride0 + coord1*stride1 + ...]
   where 0 <= coord0 < dim0, 0 <= coord1 < dim1 etc.

   Args:
       name: buffer name to print in the header. Function accepts NULL.
       buffer: pointer to data.
       dim: dimension size in elements.
       stride: stride in associated dimension in elements.
*/
void print_stats_U16(const char *name, const void *buffer, unsigned dim0, unsigned stride0,
                     unsigned dim1, unsigned stride1, unsigned dim2, unsigned stride2,
                     unsigned dim3, unsigned stride3);

/* Print multidimensional U32 tensor statistcs.

   Element is located as ((uint32_t*)buffer)[coord0*stride0 + coord1*stride1 + ...]
   where 0 <= coord0 < dim0, 0 <= coord1 < dim1 etc.

   Args:
       name: buffer name to print in the header. Function accepts NULL.
       buffer: pointer to data.
       dim: dimension size in elements.
       stride: stride in associated dimension in elements.
*/
void print_stats_U32(const char *name, const void *buffer, unsigned dim0, unsigned stride0,
                     unsigned dim1, unsigned stride1, unsigned dim2, unsigned stride2,
                     unsigned dim3, unsigned stride3);

/* Print multidimensional FLOAT tensor statistcs.

   Element is located as ((float*)buffer)[coord0*stride0 + coord1*stride1 + ...]
   where 0 <= coord0 < dim0, 0 <= coord1 < dim1 etc.

   Args:
       name: buffer name to print in the header. Function accepts NULL.
       buffer: pointer to data.
       dim: dimension size in elements.
       stride: stride in associated dimension in elements.
*/
void print_stats_FLOAT(const char *name, const void *buffer, unsigned dim0, unsigned stride0,
                     unsigned dim1, unsigned stride1, unsigned dim2, unsigned stride2,
                     unsigned dim3, unsigned stride3);

/* Debug printing of statistics of layer outputs
   name: layer name
   buffer: buffer pointer
   dim: dimension size in elements
   stride: stride between dimension elements in elements
*/


/* randomize array

   Args:
       dst: pointer to the array
       size: size of the array in bytes
*/
void randomize_array(void *dst, size_t size);

uint8_t  random_U8();
int8_t   random_S8();
uint16_t random_U16();
int16_t  random_S16();
uint32_t random_U32();
int32_t  random_S32();
float  random_F32();

void debug_jam(const xi_array *in, int copy_first, xi_array *out);

int cnnCmpTiles(const char *net, const char *param, void *tileA, void *tileB);

#if defined(RTL_DIAG) || defined(POWER_DIAG) || defined(XTSC_DIAG)
#include <xtensa/config/core.h>
#include <xtensa/config/system.h>
#endif

#if defined(RTL_DIAG) || defined(POWER_DIAG)
#include <xtensa/hal.h>
#include <xtensa/xt_reftb.h>

#define weak __attribute__((weak))

#define TB_SLAVEMEMDUMP_OFFSET 16
#ifndef TB_SLAVEMEMDUMP_LOCATION
#define TB_SLAVEMEMDUMP_LOCATION (XSHAL_MAGIC_EXIT + TB_SLAVEMEMDUMP_OFFSET)
#endif

#define TB_SLAVEMEMLOAD_OFFSET 20
#ifndef TB_SLAVEMEMLOAD_LOCATION
#define TB_SLAVEMEMLOAD_LOCATION (XSHAL_MAGIC_EXIT + TB_SLAVEMEMLOAD_OFFSET)
#endif

// Write to TB_SLAVEMEMDUMP_LOCATION the address pair (Source, Destination) to dump the region
volatile weak unsigned int *_reftb_slavememdump = (unsigned int*) TB_SLAVEMEMDUMP_LOCATION;

// Write to TB_SLAVEMEMLOAD_LOCATION the address pair (Source, Destination) to load the region
volatile weak unsigned int *_reftb_slavememload = (unsigned int*) TB_SLAVEMEMLOAD_LOCATION;

//
// Dump Slave System Memory for HW simulation
// Monitors.v will detect this and dump the memory.

__attribute__ ((always_inline))
static inline
void set_memdump_address_region(char* SA, unsigned int size)
{
#ifdef __XTENSA__
  *_reftb_slavememdump = (unsigned int)SA;
  XT_MEMW();
  *_reftb_slavememdump = (unsigned int)SA + (size * sizeof(char));
  XT_MEMW();
#endif
}

__attribute__ ((always_inline))
static inline
void set_memload_address_region(char* SA, unsigned int size)
{
#ifdef __XTENSA__
  *_reftb_slavememload = (unsigned int)SA;
  XT_MEMW();
  *_reftb_slavememload = (unsigned int)SA + (size * sizeof(char));
  XT_MEMW();
#endif
}

__attribute__ ((always_inline))
static inline
unsigned int get_memload_status(void)
{
#ifdef __XTENSA__
  XT_MEMW();
  return (*_reftb_slavememdump);
#endif
}
#endif // RTL_DIAG

#if !defined(GOLDEN_DIAG) && defined(ENABLE_XNNE_PERF_COUNTER) && (ENABLE_XNNE_PERF_COUNTER)
void print_perf_counters();
void release_perf_counters();
void setup_perf_counters();
void read_perf_counters();
#endif// ENABLE_XNNE_PERF_COUNTER

#ifdef __cplusplus
}
#endif

#endif /* _CNNRT_DEBUG_H_INCLUDED_ */
