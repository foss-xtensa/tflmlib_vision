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
#ifndef __XI_CORE_H__
#define __XI_CORE_H__

#include "xi_core_api.h"

#if defined(_MSC_VER)
#define isfinite _finite
#define __func__ __FUNCTION__
#endif

// internal API
extern int XCHAL_IVPN_SIMD_WIDTH_;

/* Linear congruential generator */
#define RND_A 1103515245
#define RND_LOG_M 31
#define RND_C 12345
#define GET_NEXT_RND(x_pr) (((RND_A) * (x_pr) + (RND_C)) & ((unsigned int)(1 << (RND_LOG_M)) - 1))

/* return 0 on success or required memory size on failure */
_XI_EXTERN_C_ size_t xiFitArray_U8(const xi_pArray donor, xi_pArray rec, int width, int height, xi_bool aligned);
_XI_EXTERN_C_ size_t xiFitArray_U8S16(const xi_pArray donor, xi_pArray rec, int width, int height, xi_bool aligned);
_XI_EXTERN_C_ size_t xiFitArray_S16(const xi_pArray donor, xi_pArray rec, int width, int height, xi_bool aligned);
_XI_EXTERN_C_ size_t xiFitTile_U8(const xi_pTile donor, xi_pTile rec, int width, int height, xi_bool aligned);
_XI_EXTERN_C_ size_t xiFitTile_S16(const xi_pTile donor, xi_pTile rec, int width, int height, xi_bool aligned);

#define XI_FIT_ALIGNED 1
#define XI_FIT_ANY     0


// error check macro
#if XI_ERROR_LEVEL == XI_ERROR_LEVEL_PRINT_ON_ERROR || XI_ERROR_LEVEL == XI_ERROR_LEVEL_PRINT_AND_CONTINUE_ON_ERROR
#  include <stdio.h>
#endif

#if XI_ERROR_LEVEL == XI_ERROR_LEVEL_TERMINATE_ON_ERROR
#  include <stdlib.h>
#endif

#define MARK_VAR_AS_USED(var) (void)(var)

#if XI_ERROR_LEVEL != XI_ERROR_LEVEL_NO_ERROR
#  define XI_ERROR_CHECKS() XI_ERR_TYPE __xi_local_err_code = XI_ERR_OK;
#  define XI_ERROR_CHECKS_CONTINUE()
#  define XI_ERROR_STATUS() __xi_local_err_code
#else
#  define XI_ERROR_CHECKS() while (0)
#  define XI_ERROR_CHECKS_CONTINUE() while (0)
#  define XI_ERROR_STATUS() XI_ERR_OK
#endif

#if XI_ERROR_LEVEL == XI_ERROR_LEVEL_TERMINATE_ON_ERROR
#  define XI_CHECK_ERROR(condition, code, wide_description) \
    if (condition) {} else exit(-1)
#elif XI_ERROR_LEVEL == XI_ERROR_LEVEL_RETURN_ON_ERROR
#  define XI_CHECK_ERROR(condition, code, wide_description) \
    if (condition) {} else return (code)
#elif XI_ERROR_LEVEL == XI_ERROR_LEVEL_CONTINUE_ON_ERROR
#  define XI_CHECK_ERROR(condition, code, wide_description) \
    if (condition) {} else __xi_local_err_code = (code)
#elif XI_ERROR_LEVEL == XI_ERROR_LEVEL_PRINT_ON_ERROR
#  define XI_CHECK_ERROR(condition, code, wide_description) \
    do { if (!(condition)) { printf("%s:%d: Error #%d (%s) in function %s: %s\n", \
            __FILE__, __LINE__, (int)(code), xiErrStr(code), __func__, wide_description); fflush(stdout); return code; }} while(0)
#  define XI_CHECK_ERROR2(condition, code, printf_args) \
    do { if (!(condition)) { printf("%s:%d: Error #%d (%s) in function %s: ", __FILE__, __LINE__, (int)(code), xiErrStr(code), __func__); \
                             printf printf_args ; \
                             printf("\n"); \
                             fflush(stdout); return code; }} while(0)
#elif XI_ERROR_LEVEL == XI_ERROR_LEVEL_PRINT_AND_CONTINUE_ON_ERROR
#  define XI_CHECK_ERROR(condition, code, wide_description) \
    do { if (!(condition)) { printf("%s:%d: Error #%d (%s) in function %s: %s\n", \
            __FILE__, __LINE__, (int)(code), xiErrStr(code), __func__, wide_description); fflush(stdout); }} while(0)
#  define XI_CHECK_ERROR2(condition, code, printf_args) \
    do { if (!(condition)) { printf("%s:%d: Error #%d (%s) in function %s: ", __FILE__, __LINE__, (int)(code), xiErrStr(code), __func__); \
                             printf printf_args ; \
                             printf("\n"); \
                             fflush(stdout); }} while(0)
#else
#  define XI_CHECK_ERROR(condition, code, wide_description)
#endif

#ifndef XI_CHECK_ERROR2
#  define XI_CHECK_ERROR2(condition, code, printf_args) XI_CHECK_ERROR(condition, code, "")
#endif

// helper macro
#define XI_PTR_TO_ADDR(ptr) ((size_t)(ptr))
#define XI_ALIGN_PTR(ptr, alignment) ((void*)XI_ALIGN_VAL(XI_PTR_TO_ADDR((ptr)), (alignment)))
#define XI_ARRAY_USEFUL_CAPACITY(array, ptr) ((ptrdiff_t)XI_ARRAY_GET_BUFF_SIZE(array) - ((uint8_t *)(ptr) - (uint8_t *)XI_ARRAY_GET_BUFF_PTR(array)))

// macro for standard array/tile checks:

// check that array/tile data is placed in the DRAM
#if XI_EMULATE_LOCAL_RAM && __XTENSA__
#if XCHAL_NUM_DATARAM==2
#define XI_ARRAY_STARTS_IN_DRAM(t) \
    (XI_PTR_TO_ADDR(XI_ARRAY_GET_BUFF_PTR(t)) >= ((uint32_t)XCHAL_DATARAM0_VADDR) || \
     (XI_PTR_TO_ADDR(XI_ARRAY_GET_BUFF_PTR(t)) >= ((uint32_t)XCHAL_DATARAM1_VADDR)))
#define XI_ARRAY_ENDS_IN_DRAM(t) \
    (XI_PTR_TO_ADDR(XI_ARRAY_GET_BUFF_PTR(t)) + XI_ARRAY_GET_BUFF_SIZE(t) <= (((uint32_t)XCHAL_DATARAM0_VADDR) + ((uint32_t)XCHAL_DATARAM0_SIZE)) || \
     (XI_PTR_TO_ADDR(XI_ARRAY_GET_BUFF_PTR(t)) + XI_ARRAY_GET_BUFF_SIZE(t) <= (((uint32_t)XCHAL_DATARAM1_VADDR) + ((uint32_t)XCHAL_DATARAM1_SIZE))))
#define XI_TILE_STARTS_IN_DRAM(t) \
    (XI_PTR_TO_ADDR(XI_TILE_GET_BUFF_PTR(t)) >= ((uint32_t)XCHAL_DATARAM0_VADDR) || \
     (XI_PTR_TO_ADDR(XI_TILE_GET_BUFF_PTR(t)) >= ((uint32_t)XCHAL_DATARAM1_VADDR)))
#define XI_TILE_ENDS_IN_DRAM(t) \
    (XI_PTR_TO_ADDR(XI_TILE_GET_BUFF_PTR(t)) + XI_TILE_GET_BUFF_SIZE(t) <= (((uint32_t)XCHAL_DATARAM0_VADDR) + ((uint32_t)XCHAL_DATARAM0_SIZE)) || \
     (XI_PTR_TO_ADDR(XI_TILE_GET_BUFF_PTR(t)) + XI_TILE_GET_BUFF_SIZE(t) <= (((uint32_t)XCHAL_DATARAM1_VADDR) + ((uint32_t)XCHAL_DATARAM1_SIZE))))
#elif XCHAL_NUM_DATARAM==1
#define XI_ARRAY_STARTS_IN_DRAM(t) \
    (XI_PTR_TO_ADDR(XI_ARRAY_GET_BUFF_PTR(t)) >= ((uint32_t)XCHAL_DATARAM0_VADDR))
#define XI_ARRAY_ENDS_IN_DRAM(t) \
    (XI_PTR_TO_ADDR(XI_ARRAY_GET_BUFF_PTR(t)) + XI_ARRAY_GET_BUFF_SIZE(t) <= (((uint32_t)XCHAL_DATARAM0_VADDR) + ((uint32_t)XCHAL_DATARAM0_SIZE)))
#define XI_TILE_STARTS_IN_DRAM(t) \
    (XI_PTR_TO_ADDR(XI_TILE_GET_BUFF_PTR(t)) >= ((uint32_t)XCHAL_DATARAM0_VADDR))
#define XI_TILE_ENDS_IN_DRAM(t) \
    (XI_PTR_TO_ADDR(XI_TILE_GET_BUFF_PTR(t)) + XI_TILE_GET_BUFF_SIZE(t) <= (((uint32_t)XCHAL_DATARAM0_VADDR) + ((uint32_t)XCHAL_DATARAM0_SIZE)))
#endif

#else //#XI_EMULATE_LOCAL_RAM && __XTENSA__
#define XI_ARRAY_STARTS_IN_DRAM(t) 1
#define XI_ARRAY_ENDS_IN_DRAM(t) 1
#define XI_TILE_STARTS_IN_DRAM(t) 1
#define XI_TILE_ENDS_IN_DRAM(t) 1
#endif //#XI_EMULATE_LOCAL_RAM && __XTENSA__

// check the minimal alignment requirements
#define XI_ARRAY_IS_WIDTH_ALIGNED(t)      ((XI_ARRAY_GET_WIDTH(t) & (XCHAL_IVPN_SIMD_WIDTH - 1)) == 0)
#define XI_ARRAY_IS_WIDTH_ALIGNED2(t)     ((XI_ARRAY_GET_WIDTH(t) & (2 * XCHAL_IVPN_SIMD_WIDTH - 1)) == 0)
#define XI_ARRAY_IS_WIDTH_ALIGNED_2(t)    ((XI_ARRAY_GET_WIDTH(t) & (XCHAL_IVPN_SIMD_WIDTH/2 - 1)) == 0)
#define XI_ARRAY_IS_STRIDE_ALIGNED(t)     ((XI_ARRAY_GET_PITCH(t) & (XCHAL_IVPN_SIMD_WIDTH - 1)) == 0)
#define XI_ARRAY_IS_STRIDE_ALIGNED2(t)    ((XI_ARRAY_GET_PITCH(t) & (2 * XCHAL_IVPN_SIMD_WIDTH - 1)) == 0)
#define XI_ARRAY_IS_STRIDE_ALIGNED_2(t)   ((XI_ARRAY_GET_PITCH(t) & (XCHAL_IVPN_SIMD_WIDTH/2 - 1)) == 0)
#define XI_ARRAY_IS_PTR_ALIGNED_NX8(t)    ((XI_PTR_TO_ADDR(XI_ARRAY_GET_DATA_PTR(t)) & (XCHAL_IVPN_SIMD_WIDTH - 1)) == 0)
#define XI_ARRAY_IS_PTR_ALIGNED_2NX8(t)   ((XI_PTR_TO_ADDR(XI_ARRAY_GET_DATA_PTR(t)) & (2 * XCHAL_IVPN_SIMD_WIDTH - 1)) == 0)
#define XI_ARRAY_IS_PTR_ALIGNED_NX16(t)   ((XI_PTR_TO_ADDR(XI_ARRAY_GET_DATA_PTR(t)) & (2 * XCHAL_IVPN_SIMD_WIDTH - 1)) == 0)
#define XI_ARRAY_IS_PTR_ALIGNED_N_2X32(t) ((XI_PTR_TO_ADDR(XI_ARRAY_GET_DATA_PTR(t)) & (2 * XCHAL_IVPN_SIMD_WIDTH - 1)) == 0)

#define XI_ARRAY_IS_ALIGNED_NX8(t)    (XI_ARRAY_IS_PTR_ALIGNED_NX8(t)    && XI_ARRAY_IS_WIDTH_ALIGNED(t)   && XI_ARRAY_IS_STRIDE_ALIGNED(t))
#define XI_ARRAY_IS_ALIGNED_2NX8(t)   (XI_ARRAY_IS_PTR_ALIGNED_2NX8(t)   && XI_ARRAY_IS_WIDTH_ALIGNED2(t)  && XI_ARRAY_IS_STRIDE_ALIGNED2(t))
#define XI_ARRAY_IS_ALIGNED_NX16(t)   (XI_ARRAY_IS_PTR_ALIGNED_NX16(t)   && XI_ARRAY_IS_WIDTH_ALIGNED(t)   && XI_ARRAY_IS_STRIDE_ALIGNED(t))
#define XI_ARRAY_IS_ALIGNED_N_2X32(t) (XI_ARRAY_IS_PTR_ALIGNED_N_2X32(t) && XI_ARRAY_IS_WIDTH_ALIGNED_2(t) && XI_ARRAY_IS_STRIDE_ALIGNED_2(t))

#define XI_TILE_IS_WIDTH_ALIGNED(t)      XI_ARRAY_IS_WIDTH_ALIGNED(t)
#define XI_TILE_IS_WIDTH_ALIGNED2(t)     XI_ARRAY_IS_WIDTH_ALIGNED2(t)
#define XI_TILE_IS_WIDTH_ALIGNED_2(t)    XI_ARRAY_IS_WIDTH_ALIGNED_2(t)
#define XI_TILE_IS_STRIDE_ALIGNED(t)     XI_ARRAY_IS_STRIDE_ALIGNED(t)
#define XI_TILE_IS_STRIDE_ALIGNED2(t)    XI_ARRAY_IS_STRIDE_ALIGNED2(t)
#define XI_TILE_IS_STRIDE_ALIGNED_2(t)   XI_ARRAY_IS_STRIDE_ALIGNED_2(t)
#define XI_TILE_IS_PTR_ALIGNED_NX8(t)    XI_ARRAY_IS_PTR_ALIGNED_NX8(t)
#define XI_TILE_IS_PTR_ALIGNED_2NX8(t)   XI_ARRAY_IS_PTR_ALIGNED_2NX8(t)
#define XI_TILE_IS_PTR_ALIGNED_NX16(t)   XI_ARRAY_IS_PTR_ALIGNED_NX16(t)
#define XI_TILE_IS_PTR_ALIGNED_N_2X32(t) XI_ARRAY_IS_PTR_ALIGNED_N_2X32(t)

// check array invariants
#define XI_ARRAY_IS_1D(t) (XI_ARRAY_GET_HEIGHT(t) == 1)

#define XI_ARRAY_CHECK_TYPE(a, type) (XI_TYPE_ELEMENT_TYPE(XI_ARRAY_GET_TYPE(a)) == type )

#define XI_ARRAY_CHECK_ELEMENT_SIZE(a, size) (XI_ARRAY_GET_ELEMENT_SIZE(a) == (size))

#define XI_ARRAY_SIZE_EQ(t1, t2) (XI_ARRAY_GET_WIDTH(t1) == XI_ARRAY_GET_WIDTH(t2) && XI_ARRAY_GET_HEIGHT(t1) == XI_ARRAY_GET_HEIGHT(t2))

#define XI_ARRAY_SIZE_GEQ(t1, t2) (XI_ARRAY_GET_WIDTH(t1) >= XI_ARRAY_GET_WIDTH(t2) && XI_ARRAY_GET_HEIGHT(t1) >= XI_ARRAY_GET_HEIGHT(t2))

#define XI_ARRAYS_ARE_NOT_OVERLAP(t1, t2) (XI_ARRAY_GET_DATA_PTR(t1) != XI_ARRAY_GET_DATA_PTR(t2))

#define XI_ARRAY_IS_CONSISTENT(a) \
    ((XI_ARRAY_GET_PITCH(a) >= XI_ARRAY_GET_WIDTH(a)) && \
    ((uint8_t*)XI_ARRAY_GET_DATA_PTR(a) >= (uint8_t*)XI_ARRAY_GET_BUFF_PTR(a)) && \
    ((uint8_t*)XI_ARRAY_GET_DATA_PTR(a) + (XI_ARRAY_GET_PITCH(a) * (XI_ARRAY_GET_HEIGHT(a) - 1) + XI_ARRAY_GET_WIDTH(a)) * XI_ARRAY_GET_ELEMENT_SIZE(a) \
                    <= (uint8_t*)XI_ARRAY_GET_BUFF_PTR(a) + XI_ARRAY_GET_BUFF_SIZE(a)))

// common array error checks
#define XI_CHECK_POINTER(pointer)                                                                                             \
    XI_CHECK_ERROR(pointer != 0,                     XI_ERR_NULLARG,    "The pointer (" #pointer ") is NULL")

#define XI_CHECK_BUFFER(array)                                                                                                \
    XI_CHECK_POINTER(array);                                                                                                  \
    XI_CHECK_ERROR(XI_ARRAY_STARTS_IN_DRAM(array), XI_ERR_MEMLOCAL,   "The argument (" #array ") data does not start in DRAM"); \
    XI_CHECK_ERROR(XI_ARRAY_ENDS_IN_DRAM(array),   XI_ERR_MEMLOCAL,   "Complete data for the argument  (" #array ")  does not lie in DRAM")

#define XI_CHECK_ARRAY(array)                                                                                                 \
    XI_CHECK_BUFFER(array);                                                                                                   \
    XI_CHECK_ERROR(XI_ARRAY_IS_CONSISTENT(array),    XI_ERR_BADARG,     "The argument (" #array ") is invalid")

#define XI_CHECK_ARRAY_I(array, element_size)                                                                                 \
    XI_CHECK_ARRAY(array);                                                                                                    \
    XI_CHECK_ERROR(XI_ARRAY_CHECK_ELEMENT_SIZE(array, element_size),                                                          \
                                                     XI_ERR_DATATYPE,   "The argument (" #array ") has wrong type")

#define XI_CHECK_ARRAY_T(array, type)                                                                                         \
    XI_CHECK_ARRAY(array);                                                                                                    \
    XI_CHECK_ERROR(XI_ARRAY_CHECK_TYPE(array, type), XI_ERR_DATATYPE,   "The argument (" #array ") has wrong type")

#define XI_CHECK_ARRAY_I8(array)  XI_CHECK_ARRAY_I(array, sizeof(int8_t))
#define XI_CHECK_ARRAY_I16(array) XI_CHECK_ARRAY_I(array, sizeof(int16_t))
#define XI_CHECK_ARRAY_I32(array) XI_CHECK_ARRAY_I(array, sizeof(int32_t))

#define XI_CHECK_ARRAY_U8(array)  XI_CHECK_ARRAY_T(array, XI_U8)
#define XI_CHECK_ARRAY_U16(array) XI_CHECK_ARRAY_T(array, XI_U16)
#define XI_CHECK_ARRAY_U32(array) XI_CHECK_ARRAY_T(array, XI_U32)
#define XI_CHECK_ARRAY_S8(array)  XI_CHECK_ARRAY_T(array, XI_S8)
#define XI_CHECK_ARRAY_S16(array) XI_CHECK_ARRAY_T(array, XI_S16)
#define XI_CHECK_ARRAY_S32(array) XI_CHECK_ARRAY_T(array, XI_S32)
#define XI_CHECK_ARRAY_F16(array) XI_CHECK_ARRAY_T(array, XI_F16)
#define XI_CHECK_ARRAY_F32(array) XI_CHECK_ARRAY_T(array, XI_F32)

#define XI_CHECK_ARRAY_IS_1D(array)\
    XI_CHECK_ERROR(XI_ARRAY_GET_HEIGHT(array)==1, XI_ERR_BADARG, "The argument (" #array ") must be a 1D array")

#define XI_CHECK_ARRAYS_ARE_NOT_OVERLAP(array0, array1)\
    XI_CHECK_ERROR(XI_ARRAYS_ARE_NOT_OVERLAP(array0, array1), XI_ERR_INPLACE, "Inplace operation is not supported")

#define XI_CHECK_ARRAY_ELEMENT_SIZE_EQ(array0, array1)\
    XI_CHECK_ERROR(XI_ARRAY_GET_ELEMENT_SIZE(array0) == XI_ARRAY_GET_ELEMENT_SIZE(array1),\
        XI_ERR_DATATYPE, "The (" #array0 ") element size must be equal to the (" #array1 ") element size")

#define XI_CHECK_ARRAY_SIZE_EQ(array0, array1)\
    XI_CHECK_ERROR(XI_ARRAY_SIZE_EQ(array0, array1), XI_ERR_DATASIZE, "The (" #array0 ") argument size is not equal to the (" #array1 ") argument size")

#define XI_CHECK_ARRAY_SIZE_GEQ(array0, array1)\
    XI_CHECK_ERROR(XI_ARRAY_SIZE_GEQ(array0, array1), XI_ERR_DATASIZE, "The (" #array0 ") argument size is not equal to OR greater than the (" #array1 ") argument size")

#define XI_CHECK_ARRAY_ALIGNMENT(array, DEPTH, ERR)\
    XI_CHECK_ERROR(XI_ARRAY_IS_ALIGNED_##DEPTH(array), XI_ERR_##ERR, "The argument (" #array ") is not fully aligned")

#define XI_CHECK_ARRAY_IALIGNMENT_NX8(array)    XI_CHECK_ARRAY_ALIGNMENT(array, NX8, IALIGNMENT)
#define XI_CHECK_ARRAY_IALIGNMENT_2NX8(array)   XI_CHECK_ARRAY_ALIGNMENT(array, 2NX8, IALIGNMENT)
#define XI_CHECK_ARRAY_IALIGNMENT_NX16(array)   XI_CHECK_ARRAY_ALIGNMENT(array, NX16, IALIGNMENT)
#define XI_CHECK_ARRAY_IALIGNMENT_N_2X32(array) XI_CHECK_ARRAY_ALIGNMENT(array, N_2X32, IALIGNMENT)
#define XI_CHECK_ARRAY_OALIGNMENT_NX8(array)    XI_CHECK_ARRAY_ALIGNMENT(array, NX8, OALIGNMENT)
#define XI_CHECK_ARRAY_OALIGNMENT_2NX8(array)   XI_CHECK_ARRAY_ALIGNMENT(array, 2NX8, OALIGNMENT)
#define XI_CHECK_ARRAY_OALIGNMENT_NX16(array)   XI_CHECK_ARRAY_ALIGNMENT(array, NX16, OALIGNMENT)
#define XI_CHECK_ARRAY_OALIGNMENT_N_2X32(array) XI_CHECK_ARRAY_ALIGNMENT(array, N_2X32, OALIGNMENT)


// check tile invariants
#define XI_TILE_IS_CONSISTENT(t) \
    ((XI_TILE_GET_PITCH(t) >= XI_TILE_GET_WIDTH(t) + XI_TILE_GET_EDGE_WIDTH(t)*2) && \
    ((uint8_t*)XI_TILE_GET_DATA_PTR(t) - (XI_TILE_GET_EDGE_WIDTH(t) + XI_TILE_GET_PITCH(t) * XI_TILE_GET_EDGE_HEIGHT(t)) * XI_TILE_GET_ELEMENT_SIZE(t) \
                    >= (uint8_t*)XI_TILE_GET_BUFF_PTR(t)) && \
    ((uint8_t*)XI_TILE_GET_DATA_PTR(t) + (XI_TILE_GET_PITCH(t) * (XI_TILE_GET_HEIGHT(t) + XI_TILE_GET_EDGE_HEIGHT(t) - 1) + XI_TILE_GET_WIDTH(t) + XI_TILE_GET_EDGE_WIDTH(t)) * XI_TILE_GET_ELEMENT_SIZE(t) \
                    <= (uint8_t*)XI_TILE_GET_BUFF_PTR(t) + XI_TILE_GET_BUFF_SIZE(t)))

// common tile error checks
#define XI_CHECK_TILE(tile)                                                                                                 \
    XI_CHECK_POINTER(tile);                                                                                                 \
    XI_CHECK_ERROR(XI_TILE_IS_CONSISTENT(tile),     XI_ERR_BADARG,     "The argument (" #tile ") is invalid");              \
    XI_CHECK_ERROR(XI_TILE_IS_TILE(tile),           XI_ERR_BADARG,     "The argument (" #tile ") is not a tile");           \
    XI_CHECK_ERROR(XI_TILE_STARTS_IN_DRAM(tile),    XI_ERR_MEMLOCAL,   "The argument (" #tile ") data does not start in DRAM"); \
    XI_CHECK_ERROR(XI_TILE_ENDS_IN_DRAM(tile),      XI_ERR_MEMLOCAL,   "Complete data for the argument  (" #tile ")  does not lie in DRAM")

#define XI_TILE_CHECK_TYPE(a, type)                                        \
((XI_TYPE_ELEMENT_TYPE(XI_TILE_GET_TYPE(a)) == type ) && (XI_TILE_IS_TILE(a)))

#define XI_CHECK_TILE_I(tile, element_size)                                                                                 \
    XI_CHECK_TILE(tile);                                                                                                    \
    XI_CHECK_ERROR(XI_ARRAY_CHECK_ELEMENT_SIZE(tile, element_size),                                                         \
                                                    XI_ERR_DATATYPE,   "The argument (" #tile ") has wrong type")

#define XI_CHECK_TILE_T(tile, type)                                                                                         \
    XI_CHECK_TILE(tile);                                                                                                    \
    XI_CHECK_ERROR(XI_TILE_CHECK_TYPE(tile, type),  XI_ERR_DATATYPE,   "The argument (" #tile ") has wrong type")

#define XI_CHECK_TILE_I8(array)  XI_CHECK_TILE_I(array, sizeof(int8_t))
#define XI_CHECK_TILE_I16(array) XI_CHECK_TILE_I(array, sizeof(int16_t))
#define XI_CHECK_TILE_I32(array) XI_CHECK_TILE_I(array, sizeof(int32_t))

#define XI_CHECK_TILE_U8(array)  XI_CHECK_TILE_T(array, XI_U8)
#define XI_CHECK_TILE_U16(array) XI_CHECK_TILE_T(array, XI_U16)
#define XI_CHECK_TILE_U32(array) XI_CHECK_TILE_T(array, XI_U32)
#define XI_CHECK_TILE_S8(array)  XI_CHECK_TILE_T(array, XI_S8)
#define XI_CHECK_TILE_S16(array) XI_CHECK_TILE_T(array, XI_S16)
#define XI_CHECK_TILE_S32(array) XI_CHECK_TILE_T(array, XI_S32)
#define XI_CHECK_TILE_F16(array) XI_CHECK_TILE_T(array, XI_F16)
#define XI_CHECK_TILE_F32(array) XI_CHECK_TILE_T(array, XI_F32)

#define XI_CHECK_TILE_EDGE(tile, edge)                                                                                      \
    XI_CHECK_ERROR(XI_TILE_GET_EDGE_WIDTH(tile) >= edge && XI_TILE_GET_EDGE_HEIGHT(tile) >= edge,                           \
                                      XI_ERR_EDGE, "The (" #tile ") tile must have at least " #edge "-pixel edge extension")

#define XI_CHECK_TILES_ARE_NOT_OVERLAP(tile0, tile1) XI_CHECK_ARRAYS_ARE_NOT_OVERLAP(tile0, tile1)

#define XI_CHECK_TILE_IALIGNMENT_NX8(tile)    XI_CHECK_ARRAY_IALIGNMENT_NX8(tile)
#define XI_CHECK_TILE_IALIGNMENT_2NX8(tile)   XI_CHECK_ARRAY_IALIGNMENT_2NX8(tile)
#define XI_CHECK_TILE_IALIGNMENT_NX16(tile)   XI_CHECK_ARRAY_IALIGNMENT_NX16(tile)
#define XI_CHECK_TILE_IALIGNMENT_N_2X32(tile) XI_CHECK_ARRAY_IALIGNMENT_N_2X32(tile)
#define XI_CHECK_TILE_OALIGNMENT_NX8(tile)    XI_CHECK_ARRAY_OALIGNMENT_NX8(tile)
#define XI_CHECK_TILE_OALIGNMENT_2NX8(tile)   XI_CHECK_ARRAY_OALIGNMENT_2NX8(tile)
#define XI_CHECK_TILE_OALIGNMENT_NX16(tile)   XI_CHECK_ARRAY_OALIGNMENT_NX16(tile)
#define XI_CHECK_TILE_OALIGNMENT_N_2X32(tile) XI_CHECK_ARRAY_OALIGNMENT_N_2X32(tile)

// Checks for confinement of 3D and 4D tiles in single DRAM
#if XI_EMULATE_LOCAL_RAM && __XTENSA__&& !SYS_MEM_TESTING
#if XCHAL_NUM_DATARAM==2
#define XI_ARRAY_START_AND_END_IN_SINGLE_DRAM(t)                                         \
  ((XI_PTR_TO_ADDR(XI_ARRAY_GET_BUFF_PTR(t)) >= ((uint32_t)XCHAL_DATARAM0_VADDR) &&      \
  XI_PTR_TO_ADDR(XI_ARRAY_GET_BUFF_PTR(t)) + XI_ARRAY_GET_BUFF_SIZE(t)                   \
   <= (((uint32_t)XCHAL_DATARAM0_VADDR) + ((uint32_t)XCHAL_DATARAM0_SIZE))) ||           \
  ((XI_PTR_TO_ADDR(XI_ARRAY_GET_BUFF_PTR(t)) >= ((uint32_t)XCHAL_DATARAM1_VADDR)) &&     \
    (XI_PTR_TO_ADDR(XI_ARRAY_GET_BUFF_PTR(t)) + XI_ARRAY_GET_BUFF_SIZE(t)                \
    <= (((uint32_t)XCHAL_DATARAM1_VADDR) + ((uint32_t)XCHAL_DATARAM1_SIZE)))))
#elif XCHAL_NUM_DATARAM==1
#define XI_ARRAY_START_AND_END_IN_SINGLE_DRAM(t) \
  (XI_PTR_TO_ADDR(XI_ARRAY_GET_BUFF_PTR(t)) >= ((uint32_t)XCHAL_DATARAM0_VADDR) &&       \
  XI_PTR_TO_ADDR(XI_ARRAY_GET_BUFF_PTR(t)) + XI_ARRAY_GET_BUFF_SIZE(t)                   \
   <= (((uint32_t)XCHAL_DATARAM0_VADDR) + ((uint32_t)XCHAL_DATARAM0_SIZE)))
#endif


#define XI_TILE3D_START_AND_END_IN_SINGLE_DRAM(t) XI_ARRAY_START_AND_END_IN_SINGLE_DRAM(t)
#define XI_TILE4D_START_AND_END_IN_SINGLE_DRAM(t) XI_ARRAY_START_AND_END_IN_SINGLE_DRAM(t)
#else
#define XI_TILE3D_START_AND_END_IN_SINGLE_DRAM(t) 1
#define XI_TILE4D_START_AND_END_IN_SINGLE_DRAM(t) 1
#define XI_ARRAY_START_AND_END_IN_SINGLE_DRAM(t) 1
#endif

#define XI_CHECK_TILE3D_FITS_IN_SINGLE_DRAM(t)                           \
  XI_CHECK_ERROR(XI_TILE3D_START_AND_END_IN_SINGLE_DRAM(t), XI_ERR_MEMLOCAL, \
  "Complete data for the argument  (" #t ")  does not fit in single DRAM");

#define XI_CHECK_TILE4D_FITS_IN_SINGLE_DRAM(t)                           \
  XI_CHECK_ERROR(XI_TILE4D_START_AND_END_IN_SINGLE_DRAM(t), XI_ERR_MEMLOCAL, \
  "Complete data for the argument  (" #t ")  does not fit in single DRAM");

#define XI_CHECK_ARRAY_FITS_IN_SINGLE_DRAM(parray)                           \
  XI_CHECK_ERROR(XI_ARRAY_START_AND_END_IN_SINGLE_DRAM(parray), XI_ERR_MEMLOCAL, \
  "Complete data for the argument  (" #parray ")  does not fit in single DRAM");

#ifndef SYS_MEM_TESTING
#define XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(tile)                                                                                              \
  XI_CHECK_ERROR(XI_TILE_STARTS_IN_DRAM(tile), XI_ERR_MEMLOCAL, "The argument (" #tile ") data does not start in DRAM"); \
  XI_CHECK_ERROR(XI_TILE_ENDS_IN_DRAM(tile), XI_ERR_MEMLOCAL, "Complete data for the argument  (" #tile ")  does not lie in DRAM");
#else
#define XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(tile)
#endif

#define XI_CHECK_TILE4D_IN_DRAM_BOUNDARY(tile)  XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(tile)

// Checks for tile consistency
#define XI_TILE3D_IS_CONSISTENT(t)                                                                                                              \
   ((uint8_t*)XI_TILE3D_GET_DATA_PTR(t) - (XI_TILE3D_GET_DIM1_EDGE1(t) + XI_TILE3D_GET_DIM1_PITCH(t) * XI_TILE3D_GET_DIM2_EDGE1(t)              \
        + XI_TILE3D_GET_DIM2_PITCH(t) * XI_TILE3D_GET_DIM3_EDGE1(t)) * XI_TILE3D_GET_ELEMENT_SIZE(t) >= (uint8_t*)XI_TILE3D_GET_BUFF_PTR(t)) && \
   ((uint8_t*)XI_TILE3D_GET_DATA_PTR(t) + (XI_TILE3D_GET_DIM2_PITCH(t) * (XI_TILE3D_GET_DIM3(t) + XI_TILE3D_GET_DIM3_EDGE2(t) - 1)              \
        + XI_TILE3D_GET_DIM1_PITCH(t) * (XI_TILE3D_GET_DIM2(t) + XI_TILE3D_GET_DIM2_EDGE2(t) - 1)                                               \
        + XI_TILE3D_GET_DIM1(t) + XI_TILE3D_GET_DIM1_EDGE2(t)) * XI_TILE3D_GET_ELEMENT_SIZE(t)                                                  \
                    <= (uint8_t*)XI_TILE3D_GET_BUFF_PTR(t) + XI_TILE3D_GET_BUFF_SIZE(t))  &&                                                    \
   (XI_TILE3D_GET_BUFF_SIZE(t) != 0)  &&                                                                                                        \
   (XI_TILE3D_GET_DIM1(t) > 0) && (XI_TILE3D_GET_DIM2(t) > 0) && (XI_TILE3D_GET_DIM3(t) > 0) &&                                                 \
   (XI_TILE3D_GET_DIM1_PITCH(t) >= XI_TILE3D_GET_DIM1(t) + XI_TILE3D_GET_DIM1_EDGE1(t) + XI_TILE3D_GET_DIM1_EDGE2(t))

#define XI_TILE4D_IS_CONSISTENT(t)                                                                                                               \
   ((uint8_t*)XI_TILE4D_GET_DATA_PTR(t) - (XI_TILE4D_GET_DIM1_EDGE1(t) + XI_TILE4D_GET_DIM1_PITCH(t) * XI_TILE4D_GET_DIM2_EDGE1(t)               \
        + XI_TILE4D_GET_DIM2_PITCH(t) * XI_TILE4D_GET_DIM3_EDGE1(t)) * XI_TILE4D_GET_ELEMENT_SIZE(t) >= (uint8_t*)XI_TILE4D_GET_BUFF_PTR(t)) &&  \
   ((uint8_t*)XI_TILE4D_GET_DATA_PTR(t) + (XI_TILE4D_GET_DIM3_PITCH(t) * (XI_TILE4D_GET_DIM4(t) - 1)                                             \
        + XI_TILE4D_GET_DIM2_PITCH(t) * (XI_TILE4D_GET_DIM3(t) + XI_TILE4D_GET_DIM3_EDGE2(t) - 1)                                                \
        + XI_TILE4D_GET_DIM1_PITCH(t) * (XI_TILE4D_GET_DIM2(t) + XI_TILE4D_GET_DIM2_EDGE2(t) - 1)                                                \
        + XI_TILE4D_GET_DIM1(t) + XI_TILE4D_GET_DIM1_EDGE2(t)) * XI_TILE4D_GET_ELEMENT_SIZE(t)                                                   \
                    <= (uint8_t*)XI_TILE4D_GET_BUFF_PTR(t) + XI_TILE4D_GET_BUFF_SIZE(t))  &&                                                     \
   (XI_TILE4D_GET_BUFF_SIZE(t) != 0)  &&                                                                                                         \
   (XI_TILE4D_GET_DIM1(t) > 0) && (XI_TILE4D_GET_DIM2(t) > 0) && (XI_TILE4D_GET_DIM3(t) > 0) && (XI_TILE4D_GET_DIM4(t) > 0) &&                   \
   (XI_TILE4D_GET_DIM1_PITCH(t) >= XI_TILE4D_GET_DIM1(t) + XI_TILE4D_GET_DIM1_EDGE1(t) + XI_TILE4D_GET_DIM1_EDGE2(t))

#define XI_TILE3D_SIZE_EQ(t1, t2)                                                                          \
  (XI_TILE3D_GET_DIM1(t1) == XI_TILE3D_GET_DIM1(t2) && XI_TILE3D_GET_DIM2(t1) == XI_TILE3D_GET_DIM2(t2) && \
   XI_TILE3D_GET_DIM3(t1) == XI_TILE3D_GET_DIM3(t2))

// common tile error checks
#define XI_CHECK_TILE3D(tile)                                                                                              \
  XI_CHECK_POINTER(tile);                                                                                                  \
  XI_CHECK_ERROR(XI_TILE3D_IS_CONSISTENT(tile), XI_ERR_BADARG, "The argument (" #tile ") is invalid");                     \
  XI_CHECK_ERROR(XI_TYPE_IS_TILE3D(XI_TILE3D_GET_TYPE(tile)), XI_ERR_BADARG, "The argument (" #tile ") is not a tile");


#define XI_TILE3D_CHECK_TYPE(a, type)                                        \
((XI_TYPE_ELEMENT_TYPE(XI_TILE3D_GET_TYPE(a)) == type ) && (XI_TYPE_IS_TILE3D(XI_TILE3D_GET_TYPE(a))))

#define XI_TILE3D_CHECK_ELEMENT_SIZE(a, size)  (XI_ARRAY_GET_ELEMENT_SIZE(a) == (size))

#define XI_CHECK_TILE3D_SIZE_EQ(t1, t2) \
  XI_CHECK_ERROR(XI_TILE3D_SIZE_EQ(t1, t2), XI_ERR_DATASIZE, "Size of the ("#t1 ") and ("#t2 ") is not same")

#define XI_CHECK_TILE3D_I(tile, element_size)                      \
  XI_CHECK_TILE3D(tile);                                           \
  XI_CHECK_ERROR(XI_TILE3D_CHECK_ELEMENT_SIZE(tile, element_size), \
                 XI_ERR_DATATYPE, "The argument (" #tile ") has wrong type")

#define XI_CHECK_TILE3D_T(tile, type) \
  XI_CHECK_TILE3D(tile);              \
  XI_CHECK_ERROR(XI_TILE3D_CHECK_TYPE(tile, type), XI_ERR_DATATYPE, "The argument (" #tile ") has wrong type")

#define XI_CHECK_TILE3D_I8(array)   XI_CHECK_TILE3D_I(array, sizeof(int8_t))
#define XI_CHECK_TILE3D_I16(array)  XI_CHECK_TILE3D_I(array, sizeof(int16_t))
#define XI_CHECK_TILE3D_I32(array)  XI_CHECK_TILE3D_I(array, sizeof(int32_t))

#define XI_CHECK_TILE3D_U8(array)   XI_CHECK_TILE3D_T(array, XI_U8)
#define XI_CHECK_TILE3D_U16(array)  XI_CHECK_TILE3D_T(array, XI_U16)
#define XI_CHECK_TILE3D_U32(array)  XI_CHECK_TILE3D_T(array, XI_U32)
#define XI_CHECK_TILE3D_S8(array)   XI_CHECK_TILE3D_T(array, XI_S8)
#define XI_CHECK_TILE3D_S16(array)  XI_CHECK_TILE3D_T(array, XI_S16)
#define XI_CHECK_TILE3D_S32(array)  XI_CHECK_TILE3D_T(array, XI_S32)
#define XI_CHECK_TILE3D_F16(array)  XI_CHECK_TILE3D_T(array, XI_F16)
#define XI_CHECK_TILE3D_F32(array)  XI_CHECK_TILE3D_T(array, XI_F32)

// checks for 4D tiles
#define XI_CHECK_TILE4D(tile)                                                                                              \
  XI_CHECK_POINTER(tile);                                                                                                  \
  XI_CHECK_ERROR(XI_TILE4D_IS_CONSISTENT(tile), XI_ERR_BADARG, "The argument (" #tile ") is invalid");                     \
  XI_CHECK_ERROR(XI_TYPE_IS_TILE4D(XI_TILE4D_GET_TYPE(tile)), XI_ERR_BADARG, "The argument (" #tile ") is not a tile");


#define XI_TILE4D_CHECK_TYPE(a, type)                                        \
((XI_TYPE_ELEMENT_TYPE(XI_TILE4D_GET_TYPE(a)) == type ) && (XI_TYPE_IS_TILE4D(XI_TILE4D_GET_TYPE(a))))

#define XI_TILE4D_CHECK_ELEMENT_SIZE(a, size)  (XI_ARRAY_GET_ELEMENT_SIZE(a) == (size))

#define XI_CHECK_TILE4D_I(tile, element_size)                      \
  XI_CHECK_TILE4D(tile);                                           \
  XI_CHECK_ERROR(XI_TILE4D_CHECK_ELEMENT_SIZE(tile, element_size), \
                 XI_ERR_DATATYPE, "The argument (" #tile ") has wrong type")

#define XI_CHECK_TILE4D_T(tile, type) \
  XI_CHECK_TILE4D(tile);              \
  XI_CHECK_ERROR(XI_TILE4D_CHECK_TYPE(tile, type), XI_ERR_DATATYPE, "The argument (" #tile ") has wrong type")

#define XI_CHECK_TILE4D_I8(array)   XI_CHECK_TILE4D_I(array, sizeof(int8_t))
#define XI_CHECK_TILE4D_I16(array)  XI_CHECK_TILE4D_I(array, sizeof(int16_t))
#define XI_CHECK_TILE4D_I32(array)  XI_CHECK_TILE4D_I(array, sizeof(int32_t))

#define XI_CHECK_TILE4D_U8(array)   XI_CHECK_TILE4D_T(array, XI_U8)
#define XI_CHECK_TILE4D_U16(array)  XI_CHECK_TILE4D_T(array, XI_U16)
#define XI_CHECK_TILE4D_U32(array)  XI_CHECK_TILE4D_T(array, XI_U32)
#define XI_CHECK_TILE4D_S8(array)   XI_CHECK_TILE4D_T(array, XI_S8)
#define XI_CHECK_TILE4D_S16(array)  XI_CHECK_TILE4D_T(array, XI_S16)
#define XI_CHECK_TILE4D_S32(array)  XI_CHECK_TILE4D_T(array, XI_S32)
#define XI_CHECK_TILE4D_F16(array)  XI_CHECK_TILE4D_T(array, XI_F16)
#define XI_CHECK_TILE4D_F32(array)  XI_CHECK_TILE4D_T(array, XI_F32)

// check the minimal alignment requirements for 3D tile
#define XI_TILE3D_IS_STRIDE_ALIGNED(t)      ((XI_TILE3D_GET_DIM1_PITCH(t) & (XCHAL_IVPN_SIMD_WIDTH - 1)) == 0)
#define XI_TILE3D_IS_STRIDE_ALIGNED2(t)     ((XI_TILE3D_GET_DIM1_PITCH(t) & (2 * XCHAL_IVPN_SIMD_WIDTH - 1)) == 0)
#define XI_TILE3D_IS_STRIDE_ALIGNED_2(t)    ((XI_TILE3D_GET_DIM1_PITCH(t) & (XCHAL_IVPN_SIMD_WIDTH / 2 - 1)) == 0)
#define XI_TILE3D_IS_PTR_ALIGNED_NX8(t)     ((XI_PTR_TO_ADDR(XI_TILE3D_GET_DATA_PTR(t)) & (XCHAL_IVPN_SIMD_WIDTH - 1)) == 0)
#define XI_TILE3D_IS_PTR_ALIGNED_2NX8(t)    ((XI_PTR_TO_ADDR(XI_TILE3D_GET_DATA_PTR(t)) & (2 * XCHAL_IVPN_SIMD_WIDTH - 1)) == 0)
#define XI_TILE3D_IS_PTR_ALIGNED_NX16(t)    ((XI_PTR_TO_ADDR(XI_TILE3D_GET_DATA_PTR(t)) & (2 * XCHAL_IVPN_SIMD_WIDTH - 1)) == 0)
#define XI_TILE3D_IS_PTR_ALIGNED_N_2X32(t)  ((XI_PTR_TO_ADDR(XI_TILE3D_GET_DATA_PTR(t)) & (2 * XCHAL_IVPN_SIMD_WIDTH - 1)) == 0)

#define XI_TILE3D_IS_ALIGNED_NX8(t)         (XI_TILE3D_IS_PTR_ALIGNED_NX8(t) && XI_TILE3D_IS_STRIDE_ALIGNED(t))
#define XI_TILE3D_IS_ALIGNED_2NX8(t)        (XI_TILE3D_IS_PTR_ALIGNED_2NX8(t) && XI_TILE3D_IS_STRIDE_ALIGNED2(t))
#define XI_TILE3D_IS_ALIGNED_NX16(t)        (XI_TILE3D_IS_PTR_ALIGNED_NX16(t) && XI_TILE3D_IS_STRIDE_ALIGNED(t))
#define XI_TILE3D_IS_ALIGNED_N_2X32(t)      (XI_TILE3D_IS_PTR_ALIGNED_N_2X32(t) && XI_TILE3D_IS_STRIDE_ALIGNED_2(t))

#define XI_CHECK_TILE3D_ALIGNMENT(array, DEPTH, ERR) \
  XI_CHECK_ERROR(XI_TILE3D_IS_ALIGNED_ ## DEPTH(array), XI_ERR_ ## ERR, "The argument (" #array ") is not fully aligned")

#define XI_CHECK_TILE3D_IALIGNMENT_NX8(array)     XI_CHECK_TILE3D_ALIGNMENT(array, NX8, IALIGNMENT)
#define XI_CHECK_TILE3D_IALIGNMENT_2NX8(array)    XI_CHECK_TILE3D_ALIGNMENT(array, 2NX8, IALIGNMENT)
#define XI_CHECK_TILE3D_IALIGNMENT_NX16(array)    XI_CHECK_TILE3D_ALIGNMENT(array, NX16, IALIGNMENT)
#define XI_CHECK_TILE3D_IALIGNMENT_N_2X32(array)  XI_CHECK_TILE3D_ALIGNMENT(array, N_2X32, IALIGNMENT)
#define XI_CHECK_TILE3D_OALIGNMENT_NX8(array)     XI_CHECK_TILE3D_ALIGNMENT(array, NX8, OALIGNMENT)
#define XI_CHECK_TILE3D_OALIGNMENT_2NX8(array)    XI_CHECK_TILE3D_ALIGNMENT(array, 2NX8, OALIGNMENT)
#define XI_CHECK_TILE3D_OALIGNMENT_NX16(array)    XI_CHECK_TILE3D_ALIGNMENT(array, NX16, OALIGNMENT)
#define XI_CHECK_TILE3D_OALIGNMENT_N_2X32(array)  XI_CHECK_TILE3D_ALIGNMENT(array, N_2X32, OALIGNMENT)
#define XI_CHECK_TILE3D_CALIGNMENT_NX8(array)     XI_CHECK_TILE3D_ALIGNMENT(array, NX8, IALIGNMENT)
#define XI_CHECK_TILE3D_CALIGNMENT_2NX8(array)    XI_CHECK_TILE3D_ALIGNMENT(array, 2NX8, IALIGNMENT)
#define XI_CHECK_TILE3D_CALIGNMENT_NX16(array)    XI_CHECK_TILE3D_ALIGNMENT(array, NX16, IALIGNMENT)
#define XI_CHECK_TILE3D_CALIGNMENT_N_2X32(array)  XI_CHECK_TILE3D_ALIGNMENT(array, N_2X32, IALIGNMENT)

// check the minimal alignment requirements for 4D tile
#define XI_TILE4D_IS_STRIDE_ALIGNED(t)      ((XI_TILE4D_GET_DIM1_PITCH(t) & (XCHAL_IVPN_SIMD_WIDTH - 1)) == 0)
#define XI_TILE4D_IS_STRIDE_ALIGNED2(t)     ((XI_TILE4D_GET_DIM1_PITCH(t) & (2 * XCHAL_IVPN_SIMD_WIDTH - 1)) == 0)
#define XI_TILE4D_IS_STRIDE_ALIGNED_2(t)    ((XI_TILE4D_GET_DIM1_PITCH(t) & (XCHAL_IVPN_SIMD_WIDTH / 2 - 1)) == 0)
#define XI_TILE4D_IS_PTR_ALIGNED_NX8(t)     ((XI_PTR_TO_ADDR(XI_TILE4D_GET_DATA_PTR(t)) & (XCHAL_IVPN_SIMD_WIDTH - 1)) == 0)
#define XI_TILE4D_IS_PTR_ALIGNED_2NX8(t)    ((XI_PTR_TO_ADDR(XI_TILE4D_GET_DATA_PTR(t)) & (2 * XCHAL_IVPN_SIMD_WIDTH - 1)) == 0)
#define XI_TILE4D_IS_PTR_ALIGNED_NX16(t)    ((XI_PTR_TO_ADDR(XI_TILE4D_GET_DATA_PTR(t)) & (2 * XCHAL_IVPN_SIMD_WIDTH - 1)) == 0)
#define XI_TILE4D_IS_PTR_ALIGNED_N_2X32(t)  ((XI_PTR_TO_ADDR(XI_TILE4D_GET_DATA_PTR(t)) & (2 * XCHAL_IVPN_SIMD_WIDTH - 1)) == 0)

#define XI_TILE4D_IS_ALIGNED_NX8(t)         (XI_TILE4D_IS_PTR_ALIGNED_NX8(t) && XI_TILE4D_IS_STRIDE_ALIGNED(t))
#define XI_TILE4D_IS_ALIGNED_2NX8(t)        (XI_TILE4D_IS_PTR_ALIGNED_2NX8(t) && XI_TILE4D_IS_STRIDE_ALIGNED2(t))
#define XI_TILE4D_IS_ALIGNED_NX16(t)        (XI_TILE4D_IS_PTR_ALIGNED_NX16(t) && XI_TILE4D_IS_STRIDE_ALIGNED(t))
#define XI_TILE4D_IS_ALIGNED_N_2X32(t)      (XI_TILE4D_IS_PTR_ALIGNED_N_2X32(t) && XI_TILE4D_IS_STRIDE_ALIGNED_2(t))

#define XI_CHECK_TILE4D_ALIGNMENT(array, DEPTH, ERR) \
  XI_CHECK_ERROR(XI_TILE4D_IS_ALIGNED_ ## DEPTH(array), XI_ERR_ ## ERR, "The argument (" #array ") is not fully aligned")

#define XI_CHECK_TILE4D_IALIGNMENT_NX8(array)     XI_CHECK_TILE4D_ALIGNMENT(array, NX8, IALIGNMENT)
#define XI_CHECK_TILE4D_IALIGNMENT_2NX8(array)    XI_CHECK_TILE4D_ALIGNMENT(array, 2NX8, IALIGNMENT)
#define XI_CHECK_TILE4D_IALIGNMENT_NX16(array)    XI_CHECK_TILE4D_ALIGNMENT(array, NX16, IALIGNMENT)
#define XI_CHECK_TILE4D_IALIGNMENT_N_2X32(array)  XI_CHECK_TILE4D_ALIGNMENT(array, N_2X32, IALIGNMENT)
#define XI_CHECK_TILE4D_OALIGNMENT_NX8(array)     XI_CHECK_TILE4D_ALIGNMENT(array, NX8, OALIGNMENT)
#define XI_CHECK_TILE4D_OALIGNMENT_2NX8(array)    XI_CHECK_TILE4D_ALIGNMENT(array, 2NX8, OALIGNMENT)
#define XI_CHECK_TILE4D_OALIGNMENT_NX16(array)    XI_CHECK_TILE4D_ALIGNMENT(array, NX16, OALIGNMENT)
#define XI_CHECK_TILE4D_OALIGNMENT_N_2X32(array)  XI_CHECK_TILE4D_ALIGNMENT(array, N_2X32, OALIGNMENT)

#define XI_CHECK_TILES3D_ARE_NOT_OVERLAP(tile0, tile1)  XI_CHECK_ARRAYS_ARE_NOT_OVERLAP(tile0, tile1)
#define XI_CHECK_TILES4D_ARE_NOT_OVERLAP(tile0, tile1)  XI_CHECK_TILES3D_ARE_NOT_OVERLAP(tile0, tile1)

#define XI_CHECK_TILE3D_EQUAL(tile1, tile2)\
  XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(tile1) == XI_TILE3D_GET_DIM1(tile2) && \
                 XI_TILE3D_GET_DIM2(tile1) == XI_TILE3D_GET_DIM2(tile2) && \
                 XI_TILE3D_GET_DIM3(tile1) == XI_TILE3D_GET_DIM3(tile2), XI_ERR_DATASIZE, \
                 "Tiles sizes are not equal.");

#define XI_CHECK_TILE4D_EQUAL(tile1, tile2)\
  XI_CHECK_ERROR(XI_TILE4D_GET_DIM1(tile1) == XI_TILE4D_GET_DIM1(tile2) && \
                 XI_TILE4D_GET_DIM2(tile1) == XI_TILE4D_GET_DIM2(tile2) && \
                 XI_TILE4D_GET_DIM3(tile1) == XI_TILE4D_GET_DIM3(tile2) && \
                 XI_TILE4D_GET_DIM4(tile1) == XI_TILE4D_GET_DIM4(tile2), XI_ERR_DATASIZE, \
                 "Tiles sizes are not equal.");

#define XI_CHECK_TILE3D_ELEMENT_SIZE_EQ(inT, outT)\
    XI_CHECK_ERROR(XI_TILE3D_GET_ELEMENT_SIZE(inT) == XI_TILE3D_GET_ELEMENT_SIZE(outT),\
        XI_ERR_DATATYPE, "Input tile element element size must be equal to output tile element size")

// other marcos
#define XI_TO_Q15(val) ((int16_t)((val) * (1 << 15) + 0.5))
#define XI_TO_Q1_14(val) ((int16_t)((val) * (1 << 14) + 0.5))
#define XI_TO_Q2_13(val) ((int16_t)((val) * (1 << 13) + 0.5))
#define XI_TO_Q3_12(val) ((int16_t)((val) * (1 << 12) + 0.5))
#define XI_TO_Q4_11(val) ((int16_t)((val) * (1 << 11) + 0.5))
#define XI_TO_Q5_10(val) ((int16_t)((val) * (1 << 10) + 0.5))
#define XI_TO_Q13_18(val) ((int)((val) * (1 << 18) + 0.5))
#define XI_Q0_16_HALF 0x8000

#endif
