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
#ifndef _CNNRT_XI_H_INCLUDED_
#define _CNNRT_XI_H_INCLUDED_

#ifndef INCLUDE_XI_CNN
#  define INCLUDE_XI_CNN
#endif

#include <limits>
#include "xi_api.h"
#include "xi_cnn_api.h"
#include "xi_tile_manager.h"
#include "xi_tile3d_manager.h"

/* Error codes in addition to defined in xi_core_api.h */
#define XI_ERR_SBLK_COMPARE   (XI_ERR_LAST + 1)
#define XI_ERR_CNNRT_LAST     (XI_ERR_SBLK_COMPARE)

#include "cnnrt_debug.h"

#if XI_ERROR_LEVEL == XI_ERROR_LEVEL_PRINT_ON_ERROR || XI_ERROR_LEVEL == XI_ERROR_LEVEL_PRINT_AND_CONTINUE_ON_ERROR
#  include <stdio.h>
#endif

#if XI_ERROR_LEVEL != XI_ERROR_LEVEL_NO_ERROR
#  define XI_ERROR_CHECKS() XI_ERR_TYPE __xi_local_err_code = XI_ERR_OK;
#  define XI_ERROR_STATUS() __xi_local_err_code
#else
#  define XI_ERROR_CHECKS() while (0)
#  define XI_ERROR_STATUS() XI_ERR_OK
#endif

#if XI_ERROR_LEVEL == XI_ERROR_LEVEL_NO_ERROR

#  define XI_CHECK_RESULT(expr)  if ((expr) != XI_ERR_OK) {};
#  define XI_RUN_TIME_CHECK(expr, msg, retval) {}

#elif XI_ERROR_LEVEL == XI_ERROR_LEVEL_TERMINATE_ON_ERROR

#  define XI_CHECK_RESULT(expr)                                          \
    {                                                                   \
        if ((expr) != XI_ERR_OK) {                                      \
            HINT_NEVER;                                                 \
            exit(-1);                                                   \
        }                                                               \
    }
#  define XI_RUN_TIME_CHECK(expr, msg, retval)                          \
    {                                                                   \
        if (!(expr)) {                                                  \
            HINT_NEVER;                                                 \
            exit(-1);                                                   \
        }                                                               \
    }

#elif XI_ERROR_LEVEL == XI_ERROR_LEVEL_RETURN_ON_ERROR

#  define XI_CHECK_RESULT(expr)                                          \
    {                                                                   \
        XI_ERR_TYPE _t_status = expr;                                   \
        if (_t_status != XI_ERR_OK) {                                   \
            HINT_NEVER;                                                 \
            return _t_status;                                           \
        }                                                               \
    }
#  define XI_RUN_TIME_CHECK(expr, msg, retval)                          \
    {                                                                   \
        if (!(expr)) {                                                  \
            HINT_NEVER;                                                 \
            return retval;                                              \
        }                                                               \
    }

#elif XI_ERROR_LEVEL == XI_ERROR_LEVEL_CONTINUE_ON_ERROR

#  define XI_CHECK_RESULT(expr)                                          \
    {                                                                   \
        XI_ERR_TYPE _t_status = expr;                                   \
        if (_t_status != XI_ERR_OK) {                                   \
            HINT_NEVER;                                                 \
            __xi_local_err_code = _t_status;                            \
        }                                                               \
    }
#  define XI_RUN_TIME_CHECK(expr, msg, retval)                          \
    {                                                                   \
        if (!(expr)) {                                                  \
            HINT_NEVER;                                                 \
            __xi_local_err_code = retval;                               \
        }                                                               \
    }

#elif XI_ERROR_LEVEL == XI_ERROR_LEVEL_PRINT_ON_ERROR

#  define XI_CHECK_RESULT(expr)                                          \
    {                                                                   \
        XI_ERR_TYPE _t_status = expr;                                   \
        if (_t_status != XI_ERR_OK) {                                   \
            HINT_NEVER;                                                 \
            return _t_status;                                           \
        }                                                               \
    }
#  define XI_RUN_TIME_CHECK(expr, msg, retval)                          \
    {                                                                   \
        if (!(expr)) {                                                  \
            HINT_NEVER;                                                 \
            printf("%s:%d: error %s in function %s\n",                  \
                   __FILE__, __LINE__, msg, __func__);                  \
            fflush(stdout);                                             \
            return retval;                                              \
        }                                                               \
    }

#elif XI_ERROR_LEVEL == XI_ERROR_LEVEL_PRINT_AND_CONTINUE_ON_ERROR

#  define XI_CHECK_RESULT(expr)                                          \
    {                                                                   \
        XI_ERR_TYPE _t_status = expr;                                   \
        if (_t_status != XI_ERR_OK) {                                   \
            HINT_NEVER;                                                 \
            __xi_local_err_code = _t_status;                            \
        }                                                               \
    }
#  define XI_RUN_TIME_CHECK(expr, msg, retval)                          \
    {                                                                   \
        if (!(expr)) {                                                  \
            HINT_NEVER;                                                 \
            printf("%s:%d: error %s in function %s\n",                  \
                   __FILE__, __LINE__, msg, __func__);                  \
            fflush(stdout);                                             \
            __xi_local_err_code = retval;                               \
        }                                                               \
    }

#else
#  error "Unexpected XI_ERROR_LEVEL value"
#endif

/* Find max value for tile element data type */
INLINE int xiTile3DTypeMaxValue(xi_pTile3D tile)
{
    unsigned type = XI_TILE3D_GET_ELEMENT_TYPE(tile);
    int result = 0;
    if (XI_TYPE_IS_SIGNED(type)) {
        switch(XI_TYPE_ELEMENT_SIZE(type)) {
        case 1:
            result = SCHAR_MAX;
            break;
        case 2:
            result = SHRT_MAX;
            break;
        default:
            result = INT_MAX;
            break;
        }
    } else {
        switch(XI_TYPE_ELEMENT_SIZE(type)) {
        case 1:
            result = UCHAR_MAX;
            break;
        case 2:
            result = USHRT_MAX;
            break;
        default:
            result = UINT_MAX;
            break;
        }
    }
    return result;
}

/* Find min value for tile element data type */
INLINE int xiTile3DTypeMinValue(xi_pTile3D tile)
{
    unsigned type = XI_TILE3D_GET_ELEMENT_TYPE(tile);
    int result = 0;
    if (XI_TYPE_IS_SIGNED(type)) {
        switch(XI_TYPE_ELEMENT_SIZE(type)) {
        case 1:
            result = SCHAR_MIN;
            break;
        case 2:
            result = SHRT_MIN;
            break;
        default:
            result = INT_MIN;
            break;
        }
    }
    /* Min value for unsigned types is 0 */
    return result;
}

#if XCHAL_HAVE_VISION == 1 && XCHAL_VISION_TYPE >= 6
#  include "cnnrt_xi_p6.h"
#else
#  error "Unsupported target"
#endif

INLINE void XI_TILE3D_ADJUST_EDGES(xi_pTile3D pTile,
                                   int dim1_edge1, int dim1_edge2,
                                   int dim2_edge1, int dim2_edge2,
                                   int dim3_edge1, int dim3_edge2)
{
    int elt_size = XI_TYPE_ELEMENT_SIZE(XI_TILE3D_GET_ELEMENT_TYPE(pTile));
    XI_TILE3D_SET_DATA_PTR(pTile, (uint8_t*)XI_TILE3D_GET_DATA_PTR(pTile)
                           - elt_size * (dim3_edge1 * XI_TILE3D_GET_DIM2_PITCH(pTile)
                                         + dim2_edge1 * XI_TILE3D_GET_DIM1_PITCH(pTile)
                                         + dim1_edge1));
    XI_TILE3D_SET_DIM1(pTile, XI_TILE3D_GET_DIM1(pTile) + dim1_edge1 + dim1_edge2);
    XI_TILE3D_SET_DIM1_COORD(pTile, XI_TILE3D_GET_DIM1_COORD(pTile) - dim1_edge1);
    XI_TILE3D_SET_DIM1_EDGE1(pTile, XI_TILE3D_GET_DIM1_EDGE1(pTile) - dim1_edge1);
    XI_TILE3D_SET_DIM1_EDGE2(pTile, XI_TILE3D_GET_DIM1_EDGE2(pTile) - dim1_edge2);
    XI_TILE3D_SET_DIM2(pTile, XI_TILE3D_GET_DIM2(pTile) + dim2_edge1 + dim2_edge2);
    XI_TILE3D_SET_DIM2_COORD(pTile, XI_TILE3D_GET_DIM2_COORD(pTile) - dim2_edge1);
    XI_TILE3D_SET_DIM2_EDGE1(pTile, XI_TILE3D_GET_DIM2_EDGE1(pTile) - dim2_edge1);
    XI_TILE3D_SET_DIM2_EDGE2(pTile, XI_TILE3D_GET_DIM2_EDGE2(pTile) - dim2_edge2);
    XI_TILE3D_SET_DIM3(pTile, XI_TILE3D_GET_DIM3(pTile) + dim3_edge1 + dim3_edge2);
    XI_TILE3D_SET_DIM3_COORD(pTile, XI_TILE3D_GET_DIM3_COORD(pTile) - dim3_edge1);
    XI_TILE3D_SET_DIM3_EDGE1(pTile, XI_TILE3D_GET_DIM3_EDGE1(pTile) - dim3_edge1);
    XI_TILE3D_SET_DIM3_EDGE2(pTile, XI_TILE3D_GET_DIM3_EDGE2(pTile) - dim3_edge2);
}

#endif /* _CNNRT_XI_H_INCLUDED_ */
