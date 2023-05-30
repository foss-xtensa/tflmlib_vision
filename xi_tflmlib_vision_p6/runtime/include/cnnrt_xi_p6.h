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
#ifndef _CNNRT_XI_P6_H_INCLUDED_
#define _CNNRT_XI_P6_H_INCLUDED_

#include <xtensa/tie/xt_ivpn.h>
#include <xtensa/tie/xt_misc.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#ifdef __cplusplus
extern "C" {
#endif

/* P6 XI-CNN library xi_cnn_conv_params doesn't have kernel dimensions.
The following macros are added to be compatible with C5 */
#define XI_CNN_CONV_SET_KERNEL_WIDTH(x, v)
#define XI_CNN_CONV_SET_KERNEL_HEIGHT(x, v)
#define XI_CNN_DEPTHWISE_DILATED_CONV_SET_KERNEL_WIDTH(x, v)
#define XI_CNN_DEPTHWISE_DILATED_CONV_SET_KERNEL_HEIGHT(x, v)

/* Sanitized normalization macros */

#define XI_CNN_NORMALIZE3D_SET_NORM_TYPE(x, v)     ((x)->normType = (v))
#define XI_CNN_NORMALIZE3D_GET_NORM_TYPE(x)        ((x)->normType)
#define XI_CNN_NORMALIZE3D_SET_NORM_AXIS(x, v)     ((x)->normAxis = (v))
#define XI_CNN_NORMALIZE3D_GET_NORM_AXIS(x)        ((x)->normAxis)
#define XI_CNN_NORMALIZE3D_SET_CHANNEL_SHARE(x, v) ((v) ? XI_CNN_NORMALIZE3D_SET_CHANNEL_SHARE_FLAG(x) : XI_CNN_NORMALIZE3D_RESET_CHANNEL_SHARE_FLAG(x))
#define XI_CNN_NORMALIZE3D_GET_CHANNEL_SHARE(x)    (XI_CNN_NORMALIZE3D_GET_CHANNEL_SHARE_FLAG(x))
#define XI_CNN_NORMALIZE3D_SET_FIRST_TILE(x, v)    ((v) ? XI_CNN_NORMALIZE3D_SET_FIRST_TILE_FLAG(x) : XI_CNN_NORMALIZE3D_RESET_FIRST_TILE_FLAG(x))
#define XI_CNN_NORMALIZE3D_GET_FIRST_TILE(x)       (XI_CNN_NORMALIZE3D_GET_FIRST_TILE_FLAG(x))
#define XI_CNN_NORMALIZE3D_SET_LAST_TILE(x, v)     ((v) ? XI_CNN_NORMALIZE3D_SET_LAST_TILE_FLAG(x) : XI_CNN_NORMALIZE3D_RESET_LAST_TILE_FLAG(x))
#define XI_CNN_NORMALIZE3D_GET_LAST_TILE(x)        (XI_CNN_NORMALIZE3D_GET_LAST_TILE_FLAG(x))
#define XI_CNN_NORMALIZE3D_SET_EPSILON_USAGE(x, v) ((v) ? XI_CNN_NORMALIZE3D_SET_EPSILON_USAGE_FLAG(x) : XI_CNN_NORMALIZE3D_RESET_EPSILON_USAGE_FLAG(x))
#define XI_CNN_NORMALIZE3D_GET_EPSILON_USAGE(x)    (XI_CNN_NORMALIZE3D_GET_EPSILON_USAGE_FLAG(x))


/* Tiled Functions */

#define xiExtendEdgesConst3D_U8   xiExtendEdgesConst3D_I8
#define xiExtendEdgesConst3D_S8   xiExtendEdgesConst3D_I8
#define xiExtendEdgesConst3D_U16  xiExtendEdgesConst3D_I16
#define xiExtendEdgesConst3D_S16  xiExtendEdgesConst3D_I16
#define xiExtendEdges3D_U8        xiExtendEdges3D_I8
#define xiExtendEdges3D_S8        xiExtendEdges3D_I8
#define xiExtendEdges3D_U16       xiExtendEdges3D_I16
#define xiExtendEdges3D_S16       xiExtendEdges3D_I16

#define xiExtendEdgesConst3D_U8_ref   xiExtendEdgesConst3D_I8_ref
#define xiExtendEdgesConst3D_S8_ref   xiExtendEdgesConst3D_I8_ref
#define xiExtendEdgesConst3D_U16_ref  xiExtendEdgesConst3D_I16_ref
#define xiExtendEdgesConst3D_S16_ref  xiExtendEdgesConst3D_I16_ref
#define xiExtendEdges3D_U8_ref        xiExtendEdges3D_I8_ref
#define xiExtendEdges3D_S8_ref        xiExtendEdges3D_I8_ref
#define xiExtendEdges3D_U16_ref       xiExtendEdges3D_I16_ref
#define xiExtendEdges3D_S16_ref       xiExtendEdges3D_I16_ref

#ifndef _CNNRT_XI_DNA100_H_INCLUDED_
#  define _proto_xiRenorm3D_reff     xiRenorm3D_ref
#  define _proto_xiCopyTile3D_reff   xiCopyTile3D_ref
#endif
#if 0
INLINE XI_ERR_TYPE _proto_xiCopyTile4D_reff(const xi_pTile4D inTile,
                                            xi_pTile4D outTile,
                                            xi_bool copy_edge_extension)
{
    XI_ERROR_CHECKS() {}
    xi_tile4D tmpInTile;
    xi_tile4D tmpOutTile;
    int i;

    tmpInTile = *inTile;
    tmpOutTile = *outTile;

    for (i = 0; i < XI_TILE4D_GET_DIM4(inTile); i++) {
        XI_TILE4D_SET_DATA_PTR(&tmpInTile, (unsigned char*)XI_TILE4D_GET_DATA_PTR(inTile) + XI_TILE4D_GET_DIM3_PITCH(inTile)*XI_TILE4D_GET_ELEMENT_SIZE(inTile)*i);
        XI_TILE4D_SET_DATA_PTR(&tmpOutTile, (unsigned char*)XI_TILE4D_GET_DATA_PTR(outTile) + XI_TILE4D_GET_DIM3_PITCH(outTile)*XI_TILE4D_GET_ELEMENT_SIZE(outTile)*i);
        XI_CHECK_RESULT(_proto_xiCopyTile3D_reff((const xi_pTile3D)&tmpInTile, (xi_pTile3D)&tmpOutTile, copy_edge_extension));
    }
    return XI_ERROR_STATUS();
}

/* Q7 and above have more data formats for transposition */
#if !(defined(XI_CNN_LIBRARY_DSP_PROCESSOR) && XI_CNN_LIBRARY_DSP_PROCESSOR == Q)

INLINE XI_ERR_TYPE _proto_xiTranspose3D_reff(const xi_pTile3D inTile,
                                             xi_pTile3D outTile)
{
#if XCHAL_HAVE_XNNE == 1
    if (XI_TILE3D_GET_DATA_ORDER(inTile) == XI_MTILE
        && (XI_TILE3D_GET_DATA_ORDER(outTile) == XI_DWH || XI_TILE3D_GET_DATA_ORDER(outTile) == XI_WHD)) {
        return copy_fromMTILES_toTile3D_ref(inTile, outTile);
    } else if ((XI_TILE3D_GET_DATA_ORDER(inTile) == XI_DWH || XI_TILE3D_GET_DATA_ORDER(inTile) == XI_WHD)
               && XI_TILE3D_GET_DATA_ORDER(outTile) == XI_MTILE) {
        return copy_fromTile3D_toMTILES_ref(inTile, outTile);
    } else if (XI_TILE3D_GET_DATA_ORDER(inTile) == XI_DWH && XI_TILE3D_GET_DATA_ORDER(outTile) == XI_DWH) {
        return xiCopyTile3D_ref(inTile, outTile, 1);
    }
#endif
    return xiTranspose3D_ref(inTile, outTile, 0);
}
#endif // !(defined(XI_CNN_LIBRARY_DSP_PROCESSOR) && XI_CNN_LIBRARY_DSP_PROCESSOR == Q)
#endif


#if _CNNRT_ANN_COMPATIBILITY_EXCLUDE_
#ifndef _CNNRT_XI_DNA100_H_INCLUDED_
INLINE XI_ERR_TYPE _proto_xiRenormVQ3D_reff(const xi_pTile3D inTile,
                                            const xi_pArray scaleArray,
                                            xi_pTile3D outTile,
                                            const uint8_t renormShift)

{
    if (XI_TILE3D_GET_DATA_ORDER(inTile) == XI_DWH)
        return xiRenormVQ3D_S16_DWH_ref(inTile, scaleArray, outTile, renormShift);
    else
        return xiRenormVQ3D_S16_WHD_ref(inTile, scaleArray, outTile, renormShift);
}
#endif // _CNNRT_XI_DNA100_H_INCLUDED_

#ifndef _CNNRT_XI_DNA100_H_INCLUDED_
INLINE XI_ERR_TYPE _proto_xiLeakyRELU_reff(const xi_pTile3D inTile,
                                           xi_pTile3D outTile,
                                           const int32_t slope,
                                           const int16_t scale,
                                           const uint8_t shift)
{
    if (scale == 1 && shift == 0) {
        if (slope == 0 &&
            XI_TILE3D_GET_TYPE(inTile) == XI_TILE3D_U8 &&
            XI_TILE3D_GET_TYPE(outTile) == XI_TILE3D_U8)
        {
            return xiRELU_ref(inTile, outTile, 0, 255);
        }
        else
        {
            return xiLeakyRELU_ref(inTile, outTile, slope);
        }
    } else {
        if (slope == 0) {
            if (XI_TILE3D_GET_TYPE(inTile) == XI_TILE3D_S8 &&
                XI_TILE3D_GET_TYPE(outTile) == XI_TILE3D_U8) {
                return xiRELUScale_S8U8_ref(inTile, outTile, scale, shift, 0, 255);
            } else {
                return XI_ERR_NO_VARIANT;
            }
        } else {
            return XI_ERR_NO_VARIANT;
        }
    }
}
#endif // _CNNRT_XI_DNA100_H_INCLUDED_

/* Data conversion full-layer reference function (to/from floating-point) */
INLINE XI_ERR_TYPE _proto_xiDataConversionFloat3D_reff(const xi_pTile3D inTile,
                                                  xi_pTile3D outTile,
                                                  const float scale)
{
    XI_ERROR_CHECKS() {}
    switch(XI_TILE3D_GET_TYPE(inTile)) {
    case XI_TILE3D_S8:
    case XI_TILE3D_U8:
    case XI_TILE3D_S16:
    case XI_TILE3D_U16:
        XI_CHECK_RESULT(xiDataConversion3D_IXFLOAT_ref(inTile, outTile, scale));
        break;
    default:
        XI_CHECK_RESULT(xiDataConversion3D_FLOATIX_ref(inTile, outTile, scale));
        break;
    }
    return XI_ERROR_STATUS();
}

/* fused resize downsample element wise add full layer reference */

INLINE XI_ERR_TYPE _proto_xiFusedResizeDownsampleEltwiseAdd3D_reff(const xi_pTile3D in0Tile,
                                                                   const xi_pTile3D in1Tile,
                                                                   xi_pTile3D outTile,
                                                                   xi_cnn_eltwise_params *params)
{
    switch(XI_TILE3D_GET_DATA_ORDER(in0Tile)) {
    case XI_DWH:
        switch(XI_TILE_GET_ELEMENT_TYPE(in0Tile)) {
        case XI_S8:  return xiEltwiseAdd3D_j1j2_S8I8_DWH_ref(in0Tile, in1Tile, outTile, params);
        case XI_U8:  return xiEltwiseAdd3D_j1j2_U8_DWH_ref(in0Tile, in1Tile, outTile, params);
        case XI_S16: return xiEltwiseAdd3D_j1j2_S16I16_DWH_ref(in0Tile, in1Tile, outTile, params);
        default:     break;
        }
        break;
    default:
        break;
    }
    return XI_ERR_NO_VARIANT;
}
#endif

INLINE void _copy_3Dto4D(xi_pTile4D dst, const xi_pTile3D src)
{
    XI_TILE4D_SET_BUFF_PTR(dst, XI_TILE3D_GET_BUFF_PTR(src));
    XI_TILE4D_SET_BUFF_SIZE(dst, XI_TILE3D_GET_BUFF_SIZE(src));
    XI_TILE4D_SET_DATA_PTR(dst, XI_TILE3D_GET_DATA_PTR(src));
    XI_TILE4D_SET_DATA_ORDER(dst, XI_TILE3D_GET_DATA_ORDER(src));
    XI_TILE4D_SET_TYPE(dst, XI_TILE3D_GET_TYPE(src) | XI_TYPE_TILE4D_BIT);
    XI_TILE4D_SET_FRAME_PTR(dst, 0);
    XI_TILE4D_SET_STATUS_FLAGS(dst, 0);
    XI_TILE4D_SET_DIM1_PITCH(dst, XI_TILE3D_GET_DIM1_PITCH(src));
    XI_TILE4D_SET_DIM2_PITCH(dst, XI_TILE3D_GET_DIM2_PITCH(src));
    XI_TILE4D_SET_DIM1(dst, XI_TILE3D_GET_DIM1(src));
    XI_TILE4D_SET_DIM1_COORD(dst, XI_TILE3D_GET_DIM1_COORD(src));
    XI_TILE4D_SET_DIM1_EDGE1(dst, XI_TILE3D_GET_DIM1_EDGE1(src));
    XI_TILE4D_SET_DIM1_EDGE2(dst, XI_TILE3D_GET_DIM1_EDGE2(src));
    XI_TILE4D_SET_DIM2(dst, XI_TILE3D_GET_DIM2(src));
    XI_TILE4D_SET_DIM2_COORD(dst, XI_TILE3D_GET_DIM2_COORD(src));
    XI_TILE4D_SET_DIM2_EDGE1(dst, XI_TILE3D_GET_DIM2_EDGE1(src));
    XI_TILE4D_SET_DIM2_EDGE2(dst, XI_TILE3D_GET_DIM2_EDGE2(src));
    XI_TILE4D_SET_DIM3(dst, XI_TILE3D_GET_DIM3(src));
    XI_TILE4D_SET_DIM3_COORD(dst, XI_TILE3D_GET_DIM3_COORD(src));
    XI_TILE4D_SET_DIM3_EDGE1(dst, XI_TILE3D_GET_DIM3_EDGE1(src));
    XI_TILE4D_SET_DIM3_EDGE2(dst, XI_TILE3D_GET_DIM3_EDGE2(src));
}

#if _CNNRT_ANN_COMPATIBILITY_EXCLUDE_
INLINE XI_ERR_TYPE xiPermute3D_ref(const xi_pTile3D inTile,
                                   xi_pTile3D outTile,
                                   xi_cnn_permute4D_params *params)
{
    xi_tile4D in4D;
    xi_tile4D out4D;

    _copy_3Dto4D(&in4D, inTile);
    XI_TILE4D_SET_DIM4(&in4D, 1);
    XI_TILE4D_SET_DIM4_COORD(&in4D, 0);
    XI_TILE4D_SET_DIM3_PITCH(&in4D, (XI_TILE4D_GET_DIM3_EDGE1(inTile) + XI_TILE4D_GET_DIM3(inTile) + XI_TILE4D_GET_DIM3_EDGE2(inTile)) * XI_TILE4D_GET_DIM2_PITCH(inTile));
    _copy_3Dto4D(&out4D, outTile);
    XI_TILE4D_SET_DIM4(&out4D, 1);
    XI_TILE4D_SET_DIM4_COORD(&out4D, 0);
    XI_TILE4D_SET_DIM3_PITCH(&out4D, (XI_TILE4D_GET_DIM3_EDGE1(outTile) + XI_TILE4D_GET_DIM3(outTile) + XI_TILE4D_GET_DIM3_EDGE2(outTile)) * XI_TILE4D_GET_DIM2_PITCH(outTile));
    return xiPermute4D_ref(&in4D, &out4D, params);
}
#endif

#define xiSoftmax1D_S16U16_ref xiSoftmax1D_S16U16

static inline XI_ERR_TYPE xiSoftmax1D_S16U16(const xi_pTile3D inTile,
                                             const xi_pArray lutArray,
                                             xi_pTile3D outTile,
                                             const xi_cnn_softmax_params *params)
{
    XI_ERROR_CHECKS() {
        XI_RUN_TIME_CHECK(XI_TILE3D_GET_ELEMENT_TYPE(inTile) == XI_S16,
                          "Input tile type is not S16", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(XI_TILE3D_GET_ELEMENT_TYPE(outTile) == XI_U16,
                          "Output tile type is not U16", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK((XI_TILE3D_GET_DIM3(inTile) == 1
                           || XI_TILE3D_GET_DIM1(inTile) * XI_TILE3D_GET_DIM2(inTile) == XI_TILE3D_GET_DIM2_PITCH(inTile))
                          && (XI_TILE3D_GET_DIM2(inTile) == 1
                              || XI_TILE3D_GET_DIM1(inTile) == XI_TILE3D_GET_DIM1_PITCH(inTile))
                          && (XI_TILE3D_GET_DIM3(outTile) == 1
                              || XI_TILE3D_GET_DIM1(outTile) * XI_TILE3D_GET_DIM2(outTile) == XI_TILE3D_GET_DIM2_PITCH(outTile))
                          && (XI_TILE3D_GET_DIM2(outTile) == 1
                              || XI_TILE3D_GET_DIM1(outTile) == XI_TILE3D_GET_DIM1_PITCH(outTile)),
                          "Tiles must be contiguous", XI_ERR_BADARG);
    }
    xi_array inp;
    xi_array outp;
    XI_ARRAY_SET_BUFF_PTR(&inp, XI_TILE3D_GET_BUFF_PTR(inTile));
    XI_ARRAY_SET_BUFF_SIZE(&inp, XI_TILE3D_GET_BUFF_SIZE(inTile));
    XI_ARRAY_SET_DATA_PTR(&inp, XI_TILE3D_GET_DATA_PTR(inTile));
    XI_ARRAY_SET_CAPACITY(&inp, XI_TILE3D_GET_DIM1(inTile) * XI_TILE3D_GET_DIM2(inTile) * XI_TILE3D_GET_DIM3(inTile));
    XI_ARRAY_SET_WIDTH(&inp, XI_ARRAY_GET_CAPACITY(&inp));
    XI_ARRAY_SET_HEIGHT(&inp, 1);
    XI_ARRAY_SET_TYPE(&inp, XI_ARRAY_S16);

    XI_ARRAY_SET_BUFF_PTR(&outp, XI_TILE3D_GET_BUFF_PTR(outTile));
    XI_ARRAY_SET_BUFF_SIZE(&outp, XI_TILE3D_GET_BUFF_SIZE(outTile));
    XI_ARRAY_SET_DATA_PTR(&outp, XI_TILE3D_GET_DATA_PTR(outTile));
    XI_ARRAY_SET_CAPACITY(&outp, XI_TILE3D_GET_DIM1(outTile) * XI_TILE3D_GET_DIM2(outTile) * XI_TILE3D_GET_DIM3(outTile));
    XI_ARRAY_SET_WIDTH(&outp, XI_ARRAY_GET_CAPACITY(&outp));
    XI_ARRAY_SET_HEIGHT(&outp, 1);
    XI_ARRAY_SET_TYPE(&outp, XI_ARRAY_U16);

    XI_CHECK_RESULT(XI_KERNEL_NAME(xiSoftmax_S16U16)(&inp, lutArray, &outp, params));
    return XI_ERROR_STATUS();
}

#if _CNNRT_ANN_COMPATIBILITY_EXCLUDE_
/* 3D-softmax full-layer reference */
INLINE XI_ERR_TYPE _proto_xiSoftmax3D_S16U16_reff(const xi_pTile3D inTile,
                                                  const xi_pArray lutArray,
                                                  xi_pTile3D outTile,
                                                  const xi_cnn_softmax_params *params,
                                                  int axis)
{
    XI_ERROR_CHECKS() {
        XI_RUN_TIME_CHECK(XI_TILE3D_GET_DATA_ORDER(inTile) == XI_WHD || XI_TILE3D_GET_DATA_ORDER(inTile) == XI_DWH,
                          "expect input order: WHD or DWH", XI_ERR_BADARG);
    }

    int axis_index = 0;
    if (XI_TILE3D_GET_DATA_ORDER(inTile) == XI_WHD) {
        switch(axis) {
        case 1: /* W */ axis_index = 1; break;
        case 2: /* H */ axis_index = 2; break;
        case 3: /* D */ axis_index = 3; break;
        }
    } else {
        /* XI_DWH layout */
        switch(axis) {
        case 1: /* W */ axis_index = 2; break;
        case 2: /* H */ axis_index = 3; break;
        case 3: /* D */ axis_index = 1; break;
        }
    }

    if (axis_index == 3) {
        XI_CHECK_RESULT(xiSoftmax3D_S16U16_Dim3_ref(inTile, lutArray, outTile, params));
    } else if (axis_index == 2) {
        XI_CHECK_RESULT(xiSoftmax3D_S16U16_Dim2_ref(inTile, lutArray, outTile, params));
    } else if (axis_index == 1) {
        XI_CHECK_RESULT(xiSoftmax3D_S16U16_Dim1_ref(inTile, lutArray, outTile, params));
    } else {
        XI_CHECK_RESULT(XI_ERR_BADARG);
    }
    return XI_ERROR_STATUS();
}


/* 3D Argmax full-layer reference */
INLINE XI_ERR_TYPE _proto_xiArgmax3D_reff(const xi_pTile3D inTile,
                                          xi_pTile3D outIndexTile,
                                          xi_pTile3D outValueTile,
                                          uint16_t numLargestVal,
                                          int axis)
{
     XI_ERROR_CHECKS() {
        XI_RUN_TIME_CHECK(XI_TILE3D_GET_DATA_ORDER(inTile) == XI_WHD || XI_TILE3D_GET_DATA_ORDER(inTile) == XI_DWH,
                          "expect input order: WHD or DWH", XI_ERR_BADARG);
    }
    xi_array buffArray;
    uint32_t main_dim_size, buff_size;
    void *buffPtr;
    int axis_index = 0;
    if (XI_TILE3D_GET_DATA_ORDER(inTile) == XI_WHD) {
        switch(axis) {
        case 1: /* W */ axis_index = 1; break;
        case 2: /* H */ axis_index = 2; break;
        case 3: /* D */ axis_index = 3; break;
        }
    } else {
        /* XI_DWH layout */
        switch(axis) {
        case 1: /* W */ axis_index = 2; break;
        case 2: /* H */ axis_index = 3; break;
        case 3: /* D */ axis_index = 1; break;
        }
    }

    if (axis_index == 3) {
        main_dim_size = XI_TILE3D_GET_DIM3(inTile);
        buff_size = main_dim_size > 1024 ? main_dim_size : main_dim_size * 64;
        buffPtr = malloc(XI_TILE3D_GET_ELEMENT_SIZE(inTile)*buff_size);
        XI_CHECK_RESULT((buffPtr) ? XI_ERR_OK : XI_ERR_TMPSIZE);
        XI_ARRAY_SET_BUFF_PTR(&buffArray, buffPtr);
        XI_ARRAY_SET_BUFF_SIZE(&buffArray, XI_TILE3D_GET_ELEMENT_SIZE(inTile)*buff_size);
        XI_ARRAY_SET_DATA_PTR(&buffArray, buffPtr);
        XI_ARRAY_SET_CAPACITY(&buffArray, buff_size);
        XI_ARRAY_SET_WIDTH(&buffArray, buff_size);
        XI_ARRAY_SET_PITCH(&buffArray, buff_size);
        XI_ARRAY_SET_HEIGHT(&buffArray, 1);
        XI_ARRAY_SET_TYPE(&buffArray, XI_TILE3D_GET_ELEMENT_TYPE(inTile));
        XI_CHECK_RESULT(xiArgmax3D_dim3_ref(inTile, &buffArray, outIndexTile, outValueTile, numLargestVal));
        free(buffPtr);
    } else if (axis_index == 2) {
        main_dim_size = XI_TILE3D_GET_DIM2(inTile);
        buff_size = main_dim_size > 1024 ? main_dim_size : main_dim_size * 64;
        buffPtr = malloc(XI_TILE3D_GET_ELEMENT_SIZE(inTile)*buff_size);
        XI_CHECK_RESULT((buffPtr) ? XI_ERR_OK : XI_ERR_TMPSIZE);
        XI_ARRAY_SET_BUFF_PTR(&buffArray, buffPtr);
        XI_ARRAY_SET_BUFF_SIZE(&buffArray, XI_TILE3D_GET_ELEMENT_SIZE(inTile)*buff_size);
        XI_ARRAY_SET_DATA_PTR(&buffArray, buffPtr);
        XI_ARRAY_SET_CAPACITY(&buffArray, buff_size);
        XI_ARRAY_SET_WIDTH(&buffArray, buff_size);
        XI_ARRAY_SET_PITCH(&buffArray, buff_size);
        XI_ARRAY_SET_HEIGHT(&buffArray, 1);
        XI_ARRAY_SET_TYPE(&buffArray, XI_TILE3D_GET_ELEMENT_TYPE(inTile));
        XI_CHECK_RESULT(xiArgmax3D_dim2_ref(inTile, &buffArray, outIndexTile, outValueTile, numLargestVal));
        free(buffPtr);
    } else if (axis_index == 1) {
        buffPtr = malloc(XI_TILE3D_GET_ELEMENT_SIZE(inTile)*XI_TILE3D_GET_DIM1(inTile));
        XI_CHECK_RESULT((buffPtr) ? XI_ERR_OK : XI_ERR_TMPSIZE);
        XI_ARRAY_SET_BUFF_PTR(&buffArray, buffPtr);
        XI_ARRAY_SET_BUFF_SIZE(&buffArray, XI_TILE3D_GET_ELEMENT_SIZE(inTile)*XI_TILE3D_GET_DIM1(inTile));
        XI_ARRAY_SET_DATA_PTR(&buffArray, buffPtr);
        XI_ARRAY_SET_CAPACITY(&buffArray, XI_TILE3D_GET_DIM1(inTile));
        XI_ARRAY_SET_WIDTH(&buffArray, XI_TILE3D_GET_DIM1(inTile));
        XI_ARRAY_SET_PITCH(&buffArray, XI_TILE3D_GET_DIM1(inTile));
        XI_ARRAY_SET_HEIGHT(&buffArray, 1);
        XI_ARRAY_SET_TYPE(&buffArray, XI_TILE3D_GET_ELEMENT_TYPE(inTile));
        XI_CHECK_RESULT(xiArgmax3D_dim1_ref(inTile, &buffArray, outIndexTile, outValueTile, numLargestVal));
        free(buffPtr);
    } else {
        XI_CHECK_RESULT(XI_ERR_BADARG);
    }
    return XI_ERROR_STATUS();
}


/* CalcMaxval full-layer reference*/
INLINE XI_ERR_TYPE _proto_xiCalcMaxval3D_S16_reff(const xi_pTile3D inTile,
                                                  xi_pArray maxval,
                                                  xi_cnn_maxval_params *params)
{
    XI_ERROR_CHECKS() {
        XI_RUN_TIME_CHECK(XI_TILE3D_GET_DATA_ORDER(inTile) == XI_WHD || XI_TILE3D_GET_DATA_ORDER(inTile) == XI_DWH,
                          "expect input order: WHD or DWH", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(XI_ARRAY_GET_WIDTH(maxval) == 1 && XI_ARRAY_GET_HEIGHT(maxval) == 1,
                          "expect maxval dimension 1x1", XI_ERR_BADARG);
    }

    unsigned char scratch[64];
    xi_array tile_scratch;
    XI_ARRAY_SET_BUFF_SIZE(&tile_scratch, 64);
    XI_ARRAY_SET_WIDTH(&tile_scratch, 32);
    XI_ARRAY_SET_HEIGHT(&tile_scratch, 1);
    XI_ARRAY_SET_TYPE(&tile_scratch, XI_ARRAY_S16);
    XI_ARRAY_SET_CAPACITY(&tile_scratch, 32);
    XI_ARRAY_SET_BUFF_PTR(&tile_scratch, scratch);
    XI_ARRAY_SET_DATA_PTR(&tile_scratch, scratch);

    XI_CNN_MAXVAL_SET_TILEFLAG(params, 3); /* TILEFLAG=3: first and last tile */
    XI_CHECK_RESULT(xiCalcMaxval3D_S16_ref(inTile, &tile_scratch, params));
    ((short*)XI_ARRAY_GET_DATA_PTR(maxval))[0] = XI_CNN_MAXVAL_GET_MAXVAL(params);
    return XI_ERROR_STATUS();
}

/* 3D-softmax with global max full-layer reference*/
INLINE XI_ERR_TYPE _proto_xiSoftmax3D_gMax_S16U16_reff(const xi_pTile3D inTile,
                                                  const xi_pArray lutArray,
                                                  const xi_pArray maxval,
                                                  xi_pTile3D outTile,
                                                  xi_cnn_softmax_params *params,
                                                  int axis)
{
    XI_ERROR_CHECKS() {
        XI_RUN_TIME_CHECK(XI_TILE3D_GET_DATA_ORDER(inTile) == XI_WHD || XI_TILE3D_GET_DATA_ORDER(inTile) == XI_DWH,
                          "expect input order: WHD or DWH", XI_ERR_BADARG);
    }

    int axis_index = 0;
    if (XI_TILE3D_GET_DATA_ORDER(inTile) == XI_WHD) {
        switch(axis) {
        case 1: /* W */ axis_index = 1; break;
        case 2: /* H */ axis_index = 2; break;
        case 3: /* D */ axis_index = 3; break;
        }
    } else {
        /* XI_DWH layout */
        switch(axis) {
        case 1: /* W */ axis_index = 2; break;
        case 2: /* H */ axis_index = 3; break;
        case 3: /* D */ axis_index = 1; break;
        }
    }

    XI_CNN_SOFTMAX_SET_MAXVAL(params, ((short*)XI_ARRAY_GET_DATA_PTR(maxval))[0]); /* set maxval as parameter */
    if (axis_index == 3) {
        XI_CHECK_RESULT(xiSoftmax3D_gMax_S16U16_Dim3_ref(inTile, lutArray, outTile, params));
    } else if (axis_index == 2) {
        XI_CHECK_RESULT(xiSoftmax3D_gMax_S16U16_Dim2_ref(inTile, lutArray, outTile, params));
    } else if (axis_index == 1) {
        XI_CHECK_RESULT(xiSoftmax3D_gMax_S16U16_Dim1_ref(inTile, lutArray, outTile, params));
    } else {
        XI_CHECK_RESULT(XI_ERR_BADARG);
    }
    return XI_ERROR_STATUS();
}

/* Fully-connected layer proto helper function to check tiles and adjust input tile by including edge in data area */
INLINE INLINE_RESTRICT XI_ERR_TYPE _fc_check_adjust_tiles(
    const xi_pTile3D inTile,
    xi_pTile3D outTile,
    xi_pTile3D fullTile)
{
    int dim1 = XI_TILE3D_GET_DIM1(inTile) + XI_TILE3D_GET_DIM1_EDGE1(inTile) + XI_TILE3D_GET_DIM1_EDGE2(inTile);
    int dim2 = XI_TILE3D_GET_DIM2(inTile) + XI_TILE3D_GET_DIM2_EDGE1(inTile) + XI_TILE3D_GET_DIM2_EDGE2(inTile);
    int dim3 = XI_TILE3D_GET_DIM3(inTile) + XI_TILE3D_GET_DIM3_EDGE1(inTile) + XI_TILE3D_GET_DIM3_EDGE2(inTile);

    XI_ERROR_CHECKS() {
        XI_RUN_TIME_CHECK((dim2 == 1 || dim1 == XI_TILE3D_GET_DIM1_PITCH(inTile))
                          && (dim3 == 1 || dim1 * dim2 == XI_TILE3D_GET_DIM2_PITCH(inTile)),
                          "contiguous input data is expected in fully connected kernel", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK((XI_TILE3D_GET_DIM2(outTile) == 1
                           || XI_TILE3D_GET_DIM1(outTile) == XI_TILE3D_GET_DIM1_PITCH(outTile))
                          && (XI_TILE3D_GET_DIM3(outTile) == 1
                              || XI_TILE3D_GET_DIM1(outTile) * XI_TILE3D_GET_DIM2(outTile) == XI_TILE3D_GET_DIM2_PITCH(outTile)),
                          "contiguous output data is expected in fully connected kernel", XI_ERR_BADARG);
    }

    XI_TILE3D_SET_BUFF_PTR(fullTile, XI_TILE3D_GET_BUFF_PTR(inTile));
    XI_TILE3D_SET_BUFF_SIZE(fullTile, XI_TILE3D_GET_BUFF_SIZE(inTile));
    XI_TILE3D_SET_DATA_ORDER(fullTile, XI_TILE3D_GET_DATA_ORDER(inTile));
    XI_TILE3D_SET_TYPE(fullTile, XI_TILE3D_GET_TYPE(inTile));
    XI_TILE3D_SET_DATA_PTR(fullTile, (uint8_t*)XI_TILE3D_GET_DATA_PTR(inTile)
                           - XI_TILE3D_GET_DIM3_EDGE1(inTile) * XI_TILE3D_GET_DIM2_PITCH(inTile)
                           - XI_TILE3D_GET_DIM2_EDGE1(inTile) * XI_TILE3D_GET_DIM1_PITCH(inTile)
                           - XI_TILE3D_GET_DIM1_EDGE1(inTile));
    XI_TILE3D_SET_DIM1(fullTile, dim1);
    XI_TILE3D_SET_DIM1_COORD(fullTile, 0);
    XI_TILE3D_SET_DIM1_EDGE1(fullTile, 0);
    XI_TILE3D_SET_DIM1_EDGE2(fullTile, 0);
    XI_TILE3D_SET_DIM1_PITCH(fullTile, dim1);
    XI_TILE3D_SET_DIM2(fullTile, dim2);
    XI_TILE3D_SET_DIM2_COORD(fullTile, 0);
    XI_TILE3D_SET_DIM2_EDGE1(fullTile, 0);
    XI_TILE3D_SET_DIM2_EDGE2(fullTile, 0);
    XI_TILE3D_SET_DIM2_PITCH(fullTile, dim1 * dim2);
    XI_TILE3D_SET_DIM3(fullTile, dim3);
    XI_TILE3D_SET_DIM3_COORD(fullTile, 0);
    XI_TILE3D_SET_DIM3_EDGE1(fullTile, 0);
    XI_TILE3D_SET_DIM3_EDGE2(fullTile, 0);
    return XI_ERROR_STATUS();
}


#define _proto_xiFullyConnected3D_S_I8S8IX_ref _proto_xiFullyConnected3D_S_I8S8IX

INLINE INLINE_RESTRICT XI_ERR_TYPE _proto_xiFullyConnected3D_S_I8S8IX(const xi_pTile3D inTile,
                                                                      const xi_pTile4D coeffTile,
                                                                      const xi_pArray biasArray,
                                                                      xi_pTile3D outTile,
                                                                      const xi_cnn_conv_params *param)
{
    XI_ERROR_CHECKS(){}
    xi_tile3D fullTile;
    _fc_check_adjust_tiles(inTile, outTile, &fullTile);
    RESTRICT_BARRIER();
    if (XI_TYPE_IS_SIGNED(XI_TILE3D_GET_ELEMENT_TYPE(inTile))) {
        XI_CHECK_RESULT(XI_KERNEL_NAME(xiFullyConnected3D_S_S8S8IX)(&fullTile, coeffTile, biasArray, outTile, param));
    } else {
        XI_CHECK_RESULT(XI_KERNEL_NAME(xiFullyConnected3D_S_U8S8IX)(&fullTile, coeffTile, biasArray, outTile, param));
    }
    RESTRICT_BARRIER();
    return XI_ERROR_STATUS();
}

#define _proto_xiFullyConnectedVQ3D_S_I8S8IX_ref _proto_xiFullyConnectedVQ3D_S_I8S8IX

INLINE INLINE_RESTRICT XI_ERR_TYPE _proto_xiFullyConnectedVQ3D_S_I8S8IX(const xi_pTile3D inTile,
                                                                      const xi_pTile4D coeffTile,
                                                                      const xi_pArray biasArray,
                                                                      const xi_pArray vecOutScale,
                                                                      xi_pTile3D outTile,
                                                                      const xi_cnn_conv_params *param)
{
    XI_ERROR_CHECKS(){}
    xi_tile3D fullTile;
    _fc_check_adjust_tiles(inTile, outTile, &fullTile);

    RESTRICT_BARRIER();
    if (XI_TYPE_IS_SIGNED(XI_TILE3D_GET_ELEMENT_TYPE(inTile))) {
        XI_CHECK_RESULT(XI_KERNEL_NAME(xiFullyConnectedVQ3D_S_S8S8IX)(&fullTile, coeffTile, biasArray, vecOutScale, outTile, param));
    } else {
        XI_CHECK_RESULT(XI_KERNEL_NAME(xiFullyConnectedVQ3D_S_U8S8IX)(&fullTile, coeffTile, biasArray, vecOutScale, outTile, param));
    }
    RESTRICT_BARRIER();
    return XI_ERROR_STATUS();
}

#define _proto_xiFullyConnected3D_WithBatching_S_S8S8IXCa2_ref _proto_xiFullyConnected3D_WithBatching_S_S8S8IXCa2

INLINE INLINE_RESTRICT XI_ERR_TYPE _proto_xiFullyConnected3D_WithBatching_S_S8S8IXCa2(const xi_pTile4D inTile,
                                                                                      const xi_pTile4D coeffTile,
                                                                                      const xi_pArray biasArray,
                                                                                      xi_pArray accArray,
                                                                                      xi_pTile4D outTile,
                                                                                      int depth_index,
                                                                                      int depth_range,
                                                                                      xi_cnn_conv_params *param)
{
    if (depth_index == 0)
        XI_CNN_CONV_SET_FLAG_INPUT(param);
    else
        XI_CNN_CONV_RESET_FLAG_INPUT(param);
    if (depth_index == depth_range - 1)
        XI_CNN_CONV_SET_FLAG_OUTPUT(param);
    else
        XI_CNN_CONV_RESET_FLAG_OUTPUT(param);
    XI_ARRAY_SET_DATA_PTR(accArray, (int32_t*)XI_ARRAY_GET_BUFF_PTR(accArray) + XI_TILE4D_GET_DIM4_COORD(inTile) * XI_TILE4D_GET_DIM1(coeffTile));
    XI_ARRAY_SET_WIDTH(accArray, XI_TILE4D_GET_DIM4(inTile) * XI_TILE4D_GET_DIM1(coeffTile));
    RESTRICT_BARRIER();
    XI_ERR_TYPE status = XI_KERNEL_NAME(xiFullyConnected3DWithBatching_S_S8S8IXCa2)(inTile, coeffTile, biasArray, accArray, outTile, param);
    RESTRICT_BARRIER();
    return status;
}

#define _proto_xiFullyConnectedVQ3D_WithBatching_S_S8S8IXCa2_ref _proto_xiFullyConnectedVQ3D_WithBatching_S_S8S8IXCa2

INLINE INLINE_RESTRICT XI_ERR_TYPE _proto_xiFullyConnectedVQ3D_WithBatching_S_S8S8IXCa2(const xi_pTile4D inTile,
                                                                                        const xi_pTile4D coeffTile,
                                                                                        const xi_pArray biasArray,
                                                                                        const xi_pArray outputScaleArray,
                                                                                        xi_pArray accArray,
                                                                                        xi_pTile4D outTile,
                                                                                        int depth_index,
                                                                                        int depth_range,
                                                                                        xi_cnn_conv_params *param)
{
    if (depth_index == 0)
        XI_CNN_CONV_SET_FLAG_INPUT(param);
    else
        XI_CNN_CONV_RESET_FLAG_INPUT(param);
    if (depth_index == depth_range - 1)
        XI_CNN_CONV_SET_FLAG_OUTPUT(param);
    else
        XI_CNN_CONV_RESET_FLAG_OUTPUT(param);
    XI_ARRAY_SET_DATA_PTR(accArray, (int32_t*)XI_ARRAY_GET_BUFF_PTR(accArray) + XI_TILE4D_GET_DIM4_COORD(inTile) * XI_TILE4D_GET_DIM1(coeffTile));
    XI_ARRAY_SET_WIDTH(accArray, XI_TILE4D_GET_DIM4(inTile) * XI_TILE4D_GET_DIM1(coeffTile));
    RESTRICT_BARRIER();
    XI_ERR_TYPE status = XI_KERNEL_NAME(xiFullyConnectedVQ3DWithBatching_S_S8S8IXCa2)(inTile, coeffTile, biasArray, accArray, outputScaleArray, outTile, param);
    RESTRICT_BARRIER();
    return status;
}

INLINE xi_pTile3D _setup_xiPartialConvolved3D(xi_pTile3D tempTile,
                                              xi_pTile3D accTile,
                                              xi_pTile3D outTile,
                                              int depth_index,
                                              int depth_range,
                                              int adjust_accTile,
                                              xi_cnn_conv_params *param)
{
    if (depth_index == 0)
        XI_CNN_CONV_SET_FLAG_INPUT(param);
    else
        XI_CNN_CONV_RESET_FLAG_INPUT(param);
    if (depth_index == depth_range - 1)
        XI_CNN_CONV_SET_FLAG_OUTPUT(param);
    else
        XI_CNN_CONV_RESET_FLAG_OUTPUT(param);
    if (adjust_accTile) {

        XI_TILE3D_SET_BUFF_PTR(tempTile, XI_TILE3D_GET_BUFF_PTR(accTile));
        XI_TILE3D_SET_BUFF_SIZE(tempTile, XI_TILE3D_GET_BUFF_SIZE(accTile));
        XI_TILE3D_SET_DATA_PTR(tempTile, (uint32_t*)XI_TILE3D_GET_BUFF_PTR(accTile)
                               + XI_TILE3D_GET_DIM2_COORD(outTile) * XI_TILE3D_GET_DIM1_PITCH(accTile)
                               + XI_TILE3D_GET_DIM3_COORD(outTile) * XI_TILE3D_GET_DIM2_PITCH(accTile));
        XI_TILE3D_SET_DATA_ORDER(tempTile, XI_TILE3D_GET_DATA_ORDER(accTile));
        XI_TILE3D_SET_TYPE(tempTile, XI_TILE3D_GET_TYPE(accTile));
        XI_TILE3D_SET_FRAME_PTR(tempTile, 0);
        XI_TILE3D_SET_STATUS_FLAGS(tempTile, 0);
        XI_TILE3D_SET_DIM1_PITCH(tempTile, XI_TILE3D_GET_DIM1_PITCH(accTile));
        XI_TILE3D_SET_DIM2_PITCH(tempTile, XI_TILE3D_GET_DIM2_PITCH(accTile));
        XI_TILE3D_SET_DIM1(tempTile, XI_TILE3D_GET_DIM1(accTile));
        XI_TILE3D_SET_DIM1_COORD(tempTile, 0);
        XI_TILE3D_SET_DIM1_EDGE1(tempTile, 0);
        XI_TILE3D_SET_DIM1_EDGE2(tempTile, 0);
        XI_TILE3D_SET_DIM2(tempTile, XI_TILE3D_GET_DIM2(outTile));
        XI_TILE3D_SET_DIM2_COORD(tempTile, 0);
        XI_TILE3D_SET_DIM2_EDGE1(tempTile, 0);
        XI_TILE3D_SET_DIM2_EDGE2(tempTile, 0);
        XI_TILE3D_SET_DIM3(tempTile, XI_TILE3D_GET_DIM3(outTile));
        XI_TILE3D_SET_DIM3_COORD(tempTile, 0);
        XI_TILE3D_SET_DIM3_EDGE1(tempTile, 0);
        XI_TILE3D_SET_DIM3_EDGE2(tempTile, 0);

        return tempTile;
    }
    return accTile;
}

#define _proto_xiPartialConvolved3D_S_S8S8IXCa2_MOD_DWH_ref _proto_xiPartialConvolved3D_S_S8S8IXCa2_MOD_DWH

INLINE INLINE_RESTRICT XI_ERR_TYPE _proto_xiPartialConvolved3D_S_S8S8IXCa2_MOD_DWH(const xi_pTile3D inTile,
                                                                                   const xi_pTile4D coeffTile,
                                                                                   const xi_pArray biasArray,
                                                                                   xi_pTile3D accTile,
                                                                                   xi_pTile3D outTile,
                                                                                   int depth_index,
                                                                                   int depth_range,
                                                                                   int adjust_accTile,
                                                                                   xi_cnn_conv_params *param)
{
    xi_tile3D temp;
    accTile = _setup_xiPartialConvolved3D(&temp, accTile, outTile, depth_index, depth_range, adjust_accTile, param);
    return XI_KERNEL_NAME(xiPartialConvolved3D_S_MxN_S8S8IXCa2_MOD_DWH)(inTile, coeffTile, biasArray, accTile, outTile, param);
}

#define _proto_xiPartialConvolvedVQ3D_S_S8S8IXCa2_MOD_DWH_ref _proto_xiPartialConvolvedVQ3D_S_S8S8IXCa2_MOD_DWH

INLINE INLINE_RESTRICT XI_ERR_TYPE _proto_xiPartialConvolvedVQ3D_S_S8S8IXCa2_MOD_DWH(const xi_pTile3D inTile,
                                                                                     const xi_pTile4D coeffTile,
                                                                                     const xi_pArray biasArray,
                                                                                     const xi_pArray outputScaleArray,
                                                                                     xi_pTile3D accTile,
                                                                                     xi_pTile3D outTile,
                                                                                     int depth_index,
                                                                                     int depth_range,
                                                                                     int adjust_accTile,
                                                                                     xi_cnn_conv_params *param)
{
    xi_tile3D temp;
    accTile = _setup_xiPartialConvolved3D(&temp, accTile, outTile, depth_index, depth_range, adjust_accTile, param);
    return XI_KERNEL_NAME(xiPartialConvolvedVQ3D_S_MxN_S8S8IXCa2_MOD_DWH)(inTile, coeffTile, biasArray, outputScaleArray, accTile, outTile, param);
}

#define _proto_xiPartialConvolved3D_S_U8S8IXCa2_MOD_DWH_ref _proto_xiPartialConvolved3D_S_U8S8IXCa2_MOD_DWH

INLINE INLINE_RESTRICT XI_ERR_TYPE _proto_xiPartialConvolved3D_S_U8S8IXCa2_MOD_DWH(const xi_pTile3D inTile,
                                                                                   const xi_pTile4D coeffTile,
                                                                                   xi_pArray biasArray,
                                                                                   xi_pTile3D accTile,
                                                                                   xi_pTile3D outTile,
                                                                                   int depth_index,
                                                                                   int depth_range,
                                                                                   int adjust_accTile,
                                                                                   xi_cnn_conv_params *param)
{
    xi_tile3D temp;
    accTile = _setup_xiPartialConvolved3D(&temp, accTile, outTile, depth_index, depth_range, adjust_accTile, param);
    return XI_KERNEL_NAME(xiPartialConvolved3D_S_MxN_U8S8IXCa2_MOD_DWH)(inTile, coeffTile, biasArray, accTile, outTile, param);
}

#define _proto_xiPartialConvolvedVQ3D_S_U8S8IXCa2_MOD_DWH_ref _proto_xiPartialConvolvedVQ3D_S_U8S8IXCa2_MOD_DWH

INLINE INLINE_RESTRICT XI_ERR_TYPE _proto_xiPartialConvolvedVQ3D_S_U8S8IXCa2_MOD_DWH(const xi_pTile3D inTile,
                                                                                     const xi_pTile4D coeffTile,
                                                                                     const xi_pArray biasArray,
                                                                                     const xi_pArray outputScaleArray,
                                                                                     xi_pTile3D accTile,
                                                                                     xi_pTile3D outTile,
                                                                                     int depth_index,
                                                                                     int depth_range,
                                                                                     int adjust_accTile,
                                                                                     xi_cnn_conv_params *param)
{
    xi_tile3D temp;
    accTile = _setup_xiPartialConvolved3D(&temp, accTile, outTile, depth_index, depth_range, adjust_accTile, param);
    return XI_KERNEL_NAME(xiPartialConvolvedVQ3D_S_MxN_U8S8IXCa2_MOD_DWH)(inTile, coeffTile, biasArray, outputScaleArray, accTile, outTile, param);
}
#endif

/*
 * Edge extension functions
 */

/* Extend W and H edge for a 3D tile in DWH format */

#if _CNNRT_ANN_COMPATIBILITY_EXCLUDE_
INLINE INLINE_RESTRICT XI_ERR_TYPE _proto_xiExtendEdgesConstWH3D_DWH_IX(xi_pTile3D inTile,
                                                                        const int32_t value,
                                                                        xi_size3D frame3DSize,
                                                                        int elt_size)
#else
INLINE INLINE_RESTRICT XI_ERR_TYPE _proto_xiExtendEdgesConstWH3D_DWH_IX(xi_pTile3D inTile,
                                                                        const int32_t value,
                                                                        xi_size3D frame3DSize)
#endif
{
    XI_ERROR_CHECKS() {
        XI_RUN_TIME_CHECK(XI_TILE3D_GET_DATA_ORDER(inTile) == XI_DWH,
                          "Tile order is not DWH", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(XI_TILE3D_GET_DIM1_EDGE1(inTile) == 0 && XI_TILE3D_GET_DIM1_EDGE2(inTile) == 0,
                          "Edge in Depth dimension must be 0", XI_ERR_EDGE);
        XI_RUN_TIME_CHECK(XI_TILE3D_GET_DIM1_COORD(inTile) >= 0
                          && XI_TILE3D_GET_DIM1_COORD(inTile) + XI_TILE3D_GET_DIM1(inTile) <= frame3DSize.dim1Size,
                          "Tile needs padding in depth passed to function that expects no padding in depth", XI_ERR_BADARG);
    }
    xi_tile3D tile;
    xi_size3D newSize;
    unsigned pitch = XI_TILE3D_GET_DIM1_PITCH(inTile);
#if !_CNNRT_ANN_COMPATIBILITY_EXCLUDE_
    unsigned type = XI_TYPE_ELEMENT_TYPE(XI_TILE3D_GET_TYPE(inTile));
    int elt_size = XI_TYPE_ELEMENT_SIZE(type);
#endif

    XI_TILE3D_SET_BUFF_PTR(&tile, XI_TILE3D_GET_BUFF_PTR(inTile));
    XI_TILE3D_SET_BUFF_SIZE(&tile, XI_TILE3D_GET_BUFF_SIZE(inTile));
    XI_TILE3D_SET_DATA_PTR(&tile, XI_TILE3D_GET_DATA_PTR(inTile));
    XI_TILE3D_SET_DATA_ORDER(&tile, XI_WHD);
    XI_TILE3D_SET_TYPE(&tile, XI_TILE3D_GET_TYPE(inTile));
    XI_TILE3D_SET_FRAME_PTR(&tile, 0);
    XI_TILE3D_SET_STATUS_FLAGS(&tile, 0);
    XI_TILE3D_SET_DIM1_PITCH(&tile, XI_TILE3D_GET_DIM2_PITCH(inTile));
    XI_TILE3D_SET_DIM2_PITCH(&tile, XI_TILE3D_GET_DIM3(inTile) * XI_TILE3D_GET_DIM2_PITCH(inTile));

    XI_TILE3D_SET_DIM1(&tile, pitch * XI_TILE3D_GET_DIM2(inTile));
    XI_TILE3D_SET_DIM1_COORD(&tile, pitch * XI_TILE3D_GET_DIM2_COORD(inTile));
    XI_TILE3D_SET_DIM1_EDGE1(&tile, pitch * XI_TILE3D_GET_DIM2_EDGE1(inTile));
    XI_TILE3D_SET_DIM1_EDGE2(&tile, pitch * XI_TILE3D_GET_DIM2_EDGE2(inTile));

    XI_TILE3D_SET_DIM2(&tile, XI_TILE3D_GET_DIM3(inTile));
    XI_TILE3D_SET_DIM2_COORD(&tile, XI_TILE3D_GET_DIM3_COORD(inTile));
    XI_TILE3D_SET_DIM2_EDGE1(&tile, XI_TILE3D_GET_DIM3_EDGE1(inTile));
    XI_TILE3D_SET_DIM2_EDGE2(&tile, XI_TILE3D_GET_DIM3_EDGE2(inTile));

    XI_TILE3D_SET_DIM3(&tile, 1);
    XI_TILE3D_SET_DIM3_COORD(&tile, 0);
    XI_TILE3D_SET_DIM3_EDGE1(&tile, 0);
    XI_TILE3D_SET_DIM3_EDGE2(&tile, 0);

    newSize.dim1Size = pitch * frame3DSize.dim2Size;
    newSize.dim2Size = frame3DSize.dim3Size;
    newSize.dim3Size = 1;

    RESTRICT_BARRIER();
    if (elt_size == 1) {
        XI_CHECK_RESULT(XI_KERNEL_NAME(xiExtendEdgesConst3D_I8)(&tile, value, newSize));
    } else {
    	assert(0);
        //XI_CHECK_RESULT(XI_KERNEL_NAME(xiExtendEdgesConst3D_I16)(&tile, value, newSize));
    }
    RESTRICT_BARRIER();
    return XI_ERROR_STATUS();
}

/* Extend D edge for a 3D tile in DWH format */

#if _CNNRT_ANN_COMPATIBILITY_EXCLUDE_
INLINE INLINE_RESTRICT XI_ERR_TYPE _proto_xiExtendEdgesConstD3D_DWH_IX(xi_pTile3D inTile,
                                                                       const int32_t value,
                                                                       xi_size3D frame3DSize,
                                                                       int elt_size)
#else
INLINE INLINE_RESTRICT XI_ERR_TYPE _proto_xiExtendEdgesConstD3D_DWH_IX(xi_pTile3D inTile,
                                                                       const int32_t value,
                                                                       xi_size3D frame3DSize)
#endif
{
    XI_ERROR_CHECKS() {
        XI_RUN_TIME_CHECK(XI_TILE3D_GET_DATA_ORDER(inTile) == XI_DWH,
                          "Tile order is not DWH", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(XI_TILE3D_GET_DIM2_EDGE1(inTile) == 0 && XI_TILE3D_GET_DIM2_EDGE2(inTile) == 0
                          && XI_TILE3D_GET_DIM3_EDGE1(inTile) == 0 && XI_TILE3D_GET_DIM3_EDGE2(inTile) == 0,
                          "Edge in Width and Height dimensions must be 0", XI_ERR_EDGE);
        XI_RUN_TIME_CHECK(XI_TILE3D_GET_DIM2_PITCH(inTile) == XI_TILE3D_GET_DIM2(inTile) * XI_TILE3D_GET_DIM1_PITCH(inTile),
                          "Tile width must be full pitch", XI_ERR_EDGE);
        XI_RUN_TIME_CHECK(XI_TILE3D_GET_DIM2_COORD(inTile) >= 0
                          && XI_TILE3D_GET_DIM2_COORD(inTile) + XI_TILE3D_GET_DIM2(inTile) <= frame3DSize.dim2Size
                          && XI_TILE3D_GET_DIM3_COORD(inTile) >= 0
                          && XI_TILE3D_GET_DIM3_COORD(inTile) + XI_TILE3D_GET_DIM3(inTile) <= frame3DSize.dim3Size,
                          "Tile needs padding in width/height passed to function that expects no padding in width/height", XI_ERR_BADARG);
    }
    xi_tile3D tile;
    xi_size3D newSize;
#if !_CNNRT_ANN_COMPATIBILITY_EXCLUDE_
    unsigned type = XI_TYPE_ELEMENT_TYPE(XI_TILE3D_GET_TYPE(inTile));
    int elt_size = XI_TYPE_ELEMENT_SIZE(type);
#endif

    XI_TILE3D_SET_BUFF_PTR(&tile, XI_TILE3D_GET_BUFF_PTR(inTile));
    XI_TILE3D_SET_BUFF_SIZE(&tile, XI_TILE3D_GET_BUFF_SIZE(inTile));
    XI_TILE3D_SET_DATA_PTR(&tile, XI_TILE3D_GET_DATA_PTR(inTile));
    XI_TILE3D_SET_DATA_ORDER(&tile, XI_WHD);
    XI_TILE3D_SET_TYPE(&tile, XI_TILE3D_GET_TYPE(inTile));
    XI_TILE3D_SET_FRAME_PTR(&tile, 0);
    XI_TILE3D_SET_STATUS_FLAGS(&tile, 0);
    XI_TILE3D_SET_DIM1_PITCH(&tile, XI_TILE3D_GET_DIM1_PITCH(inTile));
    XI_TILE3D_SET_DIM2_PITCH(&tile, XI_TILE3D_GET_DIM2_PITCH(inTile) * XI_TILE3D_GET_DIM3(inTile));

    XI_TILE3D_SET_DIM1(&tile, XI_TILE3D_GET_DIM1(inTile));
    XI_TILE3D_SET_DIM1_COORD(&tile, XI_TILE3D_GET_DIM1_COORD(inTile));
    XI_TILE3D_SET_DIM1_EDGE1(&tile, XI_TILE3D_GET_DIM1_EDGE1(inTile));
    XI_TILE3D_SET_DIM1_EDGE2(&tile, XI_TILE3D_GET_DIM1_EDGE2(inTile));

    XI_TILE3D_SET_DIM2(&tile, XI_TILE3D_GET_DIM2(inTile) * XI_TILE3D_GET_DIM3(inTile));
    XI_TILE3D_SET_DIM2_COORD(&tile, 0);
    XI_TILE3D_SET_DIM2_EDGE1(&tile, 0);
    XI_TILE3D_SET_DIM2_EDGE2(&tile, 0);

    XI_TILE3D_SET_DIM3(&tile, 1);
    XI_TILE3D_SET_DIM3_COORD(&tile, 0);
    XI_TILE3D_SET_DIM3_EDGE1(&tile, 0);
    XI_TILE3D_SET_DIM3_EDGE2(&tile, 0);

    newSize.dim1Size = frame3DSize.dim1Size;
    newSize.dim2Size = frame3DSize.dim2Size * frame3DSize.dim3Size;
    newSize.dim3Size = 1;

    RESTRICT_BARRIER();
    if (elt_size == 1) {
        XI_CHECK_RESULT(XI_KERNEL_NAME(xiExtendEdgesConst3D_I8)(&tile, value, newSize));
    } else {
    	assert(0);
        //XI_CHECK_RESULT(XI_KERNEL_NAME(xiExtendEdgesConst3D_I16)(&tile, value, newSize));
    }
    RESTRICT_BARRIER();
    return XI_ERROR_STATUS();
}

/* Edge extend D dimension for a 3D tile in WHD format */

INLINE INLINE_RESTRICT XI_ERR_TYPE _proto_xiExtendEdgesConstD3D_WHD_IX(xi_pTile3D inTile,
                                                                       const int32_t value,
                                                                       xi_size3D frame3DSize,
                                                                       int elt_size)
{
    XI_ERROR_CHECKS() {
        XI_RUN_TIME_CHECK(XI_TILE3D_GET_DATA_ORDER(inTile) == XI_WHD,
                          "Tile order is not WHD", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(XI_TILE3D_GET_DIM1_EDGE1(inTile) == 0 && XI_TILE3D_GET_DIM1_EDGE2(inTile) == 0
                          && XI_TILE3D_GET_DIM2_EDGE1(inTile) == 0 && XI_TILE3D_GET_DIM2_EDGE2(inTile) == 0,
                          "Edge in Width and Height dimensions must be 0", XI_ERR_EDGE);
        XI_RUN_TIME_CHECK(XI_TILE3D_GET_DIM1_COORD(inTile) >= 0
                          && XI_TILE3D_GET_DIM1_COORD(inTile) + XI_TILE3D_GET_DIM1(inTile) <= frame3DSize.dim1Size
                          && XI_TILE3D_GET_DIM2_COORD(inTile) >= 0
                          && XI_TILE3D_GET_DIM2_COORD(inTile) + XI_TILE3D_GET_DIM2(inTile) <= frame3DSize.dim2Size,
                          "Tile needs padding in width/height passed to function that expects no padding in width/height", XI_ERR_BADARG);
    }

    xi_tile3D tile;
    xi_size3D newSize;

    XI_TILE3D_SET_BUFF_PTR(&tile, XI_TILE3D_GET_BUFF_PTR(inTile));
    XI_TILE3D_SET_BUFF_SIZE(&tile, XI_TILE3D_GET_BUFF_SIZE(inTile));
    XI_TILE3D_SET_DATA_PTR(&tile, XI_TILE3D_GET_DATA_PTR(inTile));
    XI_TILE3D_SET_DATA_ORDER(&tile, XI_WHD);
    XI_TILE3D_SET_TYPE(&tile, XI_TILE3D_GET_TYPE(inTile));
    XI_TILE3D_SET_FRAME_PTR(&tile, 0);
    XI_TILE3D_SET_STATUS_FLAGS(&tile, 0);
    XI_TILE3D_SET_DIM1_PITCH(&tile, XI_TILE3D_GET_DIM2_PITCH(inTile));
    XI_TILE3D_SET_DIM2_PITCH(&tile, XI_TILE3D_GET_DIM2_PITCH(inTile) * XI_TILE3D_GET_DIM3(inTile));

    XI_TILE3D_SET_DIM1(&tile, XI_TILE3D_GET_DIM1_PITCH(inTile) * XI_TILE3D_GET_DIM2(inTile));
    XI_TILE3D_SET_DIM1_COORD(&tile, 0);
    XI_TILE3D_SET_DIM1_EDGE1(&tile, 0);
    XI_TILE3D_SET_DIM1_EDGE2(&tile, 0);

    XI_TILE3D_SET_DIM2(&tile, XI_TILE3D_GET_DIM3(inTile));
    XI_TILE3D_SET_DIM2_COORD(&tile, XI_TILE3D_GET_DIM3_COORD(inTile));
    XI_TILE3D_SET_DIM2_EDGE1(&tile, XI_TILE3D_GET_DIM3_EDGE1(inTile));
    XI_TILE3D_SET_DIM2_EDGE2(&tile, XI_TILE3D_GET_DIM3_EDGE2(inTile));

    XI_TILE3D_SET_DIM3(&tile, 1);
    XI_TILE3D_SET_DIM3_COORD(&tile, 0);
    XI_TILE3D_SET_DIM3_EDGE1(&tile, 0);
    XI_TILE3D_SET_DIM3_EDGE2(&tile, 0);

    newSize.dim1Size = frame3DSize.dim1Size * frame3DSize.dim2Size;
    newSize.dim2Size = frame3DSize.dim3Size;
    newSize.dim3Size = 1;

    RESTRICT_BARRIER();
    if (elt_size == 1) {
        XI_CHECK_RESULT(XI_KERNEL_NAME(xiExtendEdgesConst3D_I8)(&tile, value, newSize));
    } else {
    	assert(0);
        //XI_CHECK_RESULT(XI_KERNEL_NAME(xiExtendEdgesConst3D_I16)(&tile, value, newSize));
    }
    RESTRICT_BARRIER();
    return XI_ERROR_STATUS();
}

/*
   Fill a buffer, essentially memset.
   Buffer size is in bytes regardless of data type.
*/

#define _proto_FillBuffer_S8 _proto_FillBuffer_I8
#define _proto_FillBuffer_U8 _proto_FillBuffer_I8

INLINE INLINE_RESTRICT XI_ERR_TYPE _proto_FillBuffer_I8(void* buff, int val, unsigned size) {
    XI_ERROR_CHECKS() {}
    unsigned its = size / (2 * XCHAL_IVPN_SIMD_WIDTH);
    unsigned rem = size % (2 * XCHAL_IVPN_SIMD_WIDTH);
    xb_vec2Nx8 *pDst = (xb_vec2Nx8 *)buff;
    valign vaDst = IVP_ZALIGN();
    xb_vec2Nx8 pattern = IVP_LSR2NX8_I((int8_t *)&val, 0);
    for (unsigned i=0; i < its; i++) {
        IVP_SAV2NX8_XP(pattern, vaDst, pDst, 2 * XCHAL_IVPN_SIMD_WIDTH);
    }
    IVP_SAV2NX8_XP(pattern, vaDst, pDst, rem);
    IVP_SAPOS2NX8_FP(vaDst, pDst);
    return XI_ERROR_STATUS();
}

#define _proto_FillBuffer_S16 _proto_FillBuffer_I16
#define _proto_FillBuffer_U16 _proto_FillBuffer_I16

INLINE INLINE_RESTRICT XI_ERR_TYPE _proto_FillBuffer_I16(void* buff, int val, unsigned size) {
    XI_ERROR_CHECKS() {}
    unsigned its = size / (2 * XCHAL_IVPN_SIMD_WIDTH);
    unsigned rem = size % (2 * XCHAL_IVPN_SIMD_WIDTH);
    xb_vecNx16 *pDst = (xb_vecNx16 *)buff;
    valign vaDst = IVP_ZALIGN();
    xb_vecNx16 pattern = IVP_LSRNX16_I((int16_t *)&val, 0);
    for (unsigned i=0; i < its; i++) {
        IVP_SAVNX16_XP(pattern, vaDst, pDst, 2 * XCHAL_IVPN_SIMD_WIDTH);
    }
    IVP_SAVNX16_XP(pattern, vaDst, pDst, rem);
    IVP_SAPOSNX16_FP(vaDst, pDst);
    return XI_ERROR_STATUS();
}

/*
 * Deconvolution intelreave wrappers
 */

#if _CNNRT_ANN_COMPATIBILITY_EXCLUDE_
static inline XI_ERR_TYPE _proto_xiDeconvInterleave3D_IX_reff(const xi_pTile3D tiles[],
                                                              xi_pTile3D outTile,
                                                              const xi_cnn_conv_params *params)
{
    if (XI_TYPE_ELEMENT_SIZE(XI_TYPE_ELEMENT_TYPE(XI_TILE3D_GET_TYPE(outTile))) == 1)
        return xiDeConvInterleave3D_I8_WHD_ref(tiles, outTile, params);
    else
        return xiDeConvInterleave3D_I16_WHD_ref(tiles, outTile, params);
}

INLINE XI_ERR_TYPE _proto_xiDeconvolvedVQ3D_S_MxN_IXIXIX_reff(const xi_pTile3D inTile,
                                                              const xi_pTile4D coeffTile,
                                                              const xi_pArray biasArray,
                                                              const xi_pArray vecOutScales,
                                                              xi_pTile3D outTile,
                                                              const xi_cnn_conv_params *param)
{
    XI_ERROR_CHECKS() {}
    xi_array array;
    XI_ERR_TYPE status;
    size_t buffer_size = sizeof(int64_t) * XI_TILE3D_GET_DIM1(outTile) * XI_TILE3D_GET_DIM2(outTile) * XI_TILE3D_GET_DIM3(outTile);
    void *buffer = malloc(buffer_size);
    XI_CHECK_RESULT((buffer) ? XI_ERR_OK : XI_ERR_TMPSIZE);
    memset(buffer, 0, buffer_size);
    XI_ARRAY_SET_BUFF_PTR(&array, buffer);
    XI_ARRAY_SET_BUFF_SIZE(&array, buffer_size);
    XI_ARRAY_SET_DATA_PTR(&array, buffer);
    XI_ARRAY_SET_WIDTH(&array, buffer_size);
    XI_ARRAY_SET_HEIGHT(&array, 1);
    XI_ARRAY_SET_TYPE(&array, XI_ARRAY_S8);
    if (vecOutScales)
        status = xiDeConvolvedVQ3D_S_MxN_IXIXIX_reff(inTile, coeffTile, biasArray, vecOutScales, &array, outTile, param, 0, 0, 0, 0);
    else
        status = xiDeConvolved3D_S_MxN_IXIXIX_reff(inTile, coeffTile, biasArray, &array, outTile, param, 0, 0, 0, 0);
    free(buffer);
    XI_CHECK_RESULT(status);
    return XI_ERROR_STATUS();
}

INLINE XI_ERR_TYPE _proto_xiDeconvolved3D_S_MxN_IXIXIX_reff(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_conv_params *param)
{
    return _proto_xiDeconvolvedVQ3D_S_MxN_IXIXIX_reff(inTile, coeffTile, biasArray, NULL, outTile, param);
}

INLINE XI_ERR_TYPE _proto_xiRoiMaxPool3D_IX_reff(const xi_pTile3D inTile,
                                                 const xi_pTile3D RoIParam,
                                                 xi_pTile4D outTile,
                                                 const xi_cnn_roi_pooling_params *param)
{
    XI_ERROR_CHECKS() {
        XI_RUN_TIME_CHECK(XI_TILE3D_GET_DATA_ORDER(RoIParam) == XI_WHD,
                          "RoIParam must be in WHD format", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(XI_TILE3D_GET_DIM1(RoIParam) == 1,
                          "RoIParam dimension 1 must be 1", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(XI_TILE3D_GET_ELEMENT_TYPE(RoIParam) == XI_S16,
                          "RoIParam tile type is not S16", XI_ERR_BADARG);
    }
    xi_array rois;
    XI_ARRAY_SET_BUFF_PTR(&rois, XI_TILE3D_GET_BUFF_PTR(RoIParam));
    XI_ARRAY_SET_BUFF_SIZE(&rois, XI_TILE3D_GET_BUFF_SIZE(RoIParam));
    XI_ARRAY_SET_DATA_PTR(&rois, XI_TILE3D_GET_DATA_PTR(RoIParam));
    XI_ARRAY_SET_CAPACITY(&rois, XI_TILE3D_GET_DIM2(RoIParam)); // array capacity is pitch actually
    XI_ARRAY_SET_WIDTH(&rois, XI_TILE3D_GET_DIM2(RoIParam));
    XI_ARRAY_SET_HEIGHT(&rois, XI_TILE3D_GET_DIM3(RoIParam));
    XI_ARRAY_SET_TYPE(&rois, XI_ARRAY_S16);

    XI_CHECK_RESULT(xiRoiMaxPool3D_IX_reff(inTile, &rois, outTile, param));
    return XI_ERROR_STATUS();
}


INLINE XI_ERR_TYPE _proto_xiCropResize3D_IX_reff(const xi_pTile3D inTile,
                                                 const xi_pTile3D RoIParam,
                                                 xi_pTile4D outTile,
                                                 const xi_cnn_cropResize3D_params *param)
{
    XI_ERROR_CHECKS() {
        XI_RUN_TIME_CHECK(XI_TILE3D_GET_DATA_ORDER(RoIParam) == XI_WHD,
                          "RoIParam must be in WHD format", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(XI_TILE3D_GET_DIM1(RoIParam) == 1,
                          "RoIParam dimension 1 must be 1", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(XI_TILE3D_GET_ELEMENT_TYPE(RoIParam) == XI_S16,
                          "RoIParam tile type is not S16", XI_ERR_BADARG);
    }
    xi_array rois;
    XI_ARRAY_SET_BUFF_PTR(&rois, XI_TILE3D_GET_BUFF_PTR(RoIParam));
    XI_ARRAY_SET_BUFF_SIZE(&rois, XI_TILE3D_GET_BUFF_SIZE(RoIParam));
    XI_ARRAY_SET_DATA_PTR(&rois, XI_TILE3D_GET_DATA_PTR(RoIParam));
    XI_ARRAY_SET_CAPACITY(&rois, XI_TILE3D_GET_DIM2(RoIParam)); // array capacity is pitch actually
    XI_ARRAY_SET_WIDTH(&rois, XI_TILE3D_GET_DIM2(RoIParam));
    XI_ARRAY_SET_HEIGHT(&rois, XI_TILE3D_GET_DIM3(RoIParam));
    XI_ARRAY_SET_TYPE(&rois, XI_ARRAY_S16);

    XI_CHECK_RESULT(xiCropResize3D_ref(inTile, &rois, outTile, param));
    return XI_ERROR_STATUS();
}

/* Some wrappers for normalize 3D */

INLINE XI_ERR_TYPE xiCalcNormalizeFactor3D_ref(const xi_pTile3D pInTile,
                                               const xi_pArray rSqrtTable,
                                               const xi_pArray recipTable,
                                               xi_pArray buffArray,
                                               xi_pArray buffArray2,
                                               xi_pArray pNormScaleArr,
                                               const xi_cnn_normalize3D_params *params) {
    XI_ERROR_CHECKS() {}
    if (XI_TILE3D_GET_DATA_ORDER(pInTile) == XI_WHD) {
        switch(XI_TILE3D_GET_TYPE(pInTile)) {
        case XI_TILE3D_S8: return xiCalcNormalizeFactor3D_S8_WHD_ref(pInTile, rSqrtTable, recipTable, buffArray, buffArray2, pNormScaleArr, params);
        case XI_TILE3D_U8: return xiCalcNormalizeFactor3D_U8_WHD_ref(pInTile, rSqrtTable, recipTable, buffArray, buffArray2, pNormScaleArr, params);
        default: return XI_ERR_DATATYPE;
        }
    } else if (XI_TILE3D_GET_DATA_ORDER(pInTile) == XI_DWH) {
        switch(XI_TILE3D_GET_TYPE(pInTile)) {
        case XI_TILE3D_S8: return xiCalcNormalizeFactor3D_S8_DWH_ref(pInTile, rSqrtTable, recipTable, buffArray, buffArray2, pNormScaleArr, params);
        case XI_TILE3D_U8: return xiCalcNormalizeFactor3D_U8_DWH_ref(pInTile, rSqrtTable, recipTable, buffArray, buffArray2, pNormScaleArr, params);
        default: return XI_ERR_DATATYPE;
        }
    } else {
        return XI_ERR_NO_VARIANT;
    }
    return XI_ERROR_STATUS();
}

INLINE XI_ERR_TYPE xiApplyScale3D_ref(const xi_pTile3D pInTile,
                                      const xi_pArray pNormScaleArr,
                                      const xi_pArray  pQuantScaleTable,
                                      xi_pTile3D pOutTile,
                                      const xi_cnn_normalize3D_params *params) {
    XI_ERROR_CHECKS() {}
    if (XI_TILE3D_GET_DATA_ORDER(pInTile) == XI_WHD) {
        switch(XI_TILE3D_GET_TYPE(pInTile)) {
        case XI_TILE3D_S8: return xiApplyScale3D_S8_WHD_ref(pInTile, pNormScaleArr, pQuantScaleTable, pOutTile, params);
        case XI_TILE3D_U8: return xiApplyScale3D_U8_WHD_ref(pInTile, pNormScaleArr, pQuantScaleTable, pOutTile, params);
        default: return XI_ERR_DATATYPE;
        }
    } else if (XI_TILE3D_GET_DATA_ORDER(pInTile) == XI_DWH) {
        switch(XI_TILE3D_GET_TYPE(pInTile)) {
        case XI_TILE3D_S8: return xiApplyScale3D_S8_DWH_ref(pInTile, pNormScaleArr, pQuantScaleTable, pOutTile, params);
        case XI_TILE3D_U8: return xiApplyScale3D_U8_DWH_ref(pInTile, pNormScaleArr, pQuantScaleTable, pOutTile, params);
        default: return XI_ERR_DATATYPE;
        }
    } else {
        return XI_ERR_NO_VARIANT;
    }
    return XI_ERROR_STATUS();
}

INLINE XI_ERR_TYPE _proto_xiDivide3D_reff(const xi_pTile3D pIn0Tile,
                                          const xi_pTile3D pIn1Tile,
                                          xi_pTile3D pOutTile,
                                          xi_cnn_divide3D_params *params)
{
    XI_ERROR_CHECKS() {}
    xi_array In1Array;
    int dim;

    dim = XI_TILE3D_GET_DIM1(pIn1Tile) * XI_TILE3D_GET_DIM2(pIn1Tile) * XI_TILE3D_GET_DIM3(pIn1Tile);
    XI_ARRAY_SET_BUFF_PTR(&In1Array, XI_TILE3D_GET_BUFF_PTR(pIn1Tile));
    XI_ARRAY_SET_BUFF_SIZE(&In1Array, XI_TILE3D_GET_BUFF_SIZE(pIn1Tile));
    XI_ARRAY_SET_DATA_PTR(&In1Array, XI_TILE3D_GET_DATA_PTR(pIn1Tile));
    XI_ARRAY_SET_CAPACITY(&In1Array, dim); // array capacity is pitch actually
    XI_ARRAY_SET_WIDTH(&In1Array, dim);
    XI_ARRAY_SET_HEIGHT(&In1Array, 1);
    XI_ARRAY_SET_TYPE(&In1Array, XI_TILE3D_GET_ELEMENT_TYPE(pIn1Tile));
    XI_CHECK_RESULT(xiDivide3D_ref(pIn0Tile, &In1Array, pOutTile, params));
    return XI_ERROR_STATUS();
}

INLINE XI_ERR_TYPE _proto_xiEltwiseAdd3D_reff(const xi_pTile3D inTile1,
                                              const xi_pTile3D inTile2,
                                              xi_pTile3D outTile,
                                              xi_cnn_eltwise_params *param)
{
    if (XI_CNN_ELTWISE_GET_RELUFLAG(param)) {
        XI_CNN_ELTWISE_SET_RELUFLAG(param, 1);
        XI_CNN_ELTWISE_SET_MAX_VAL(param, xiTile3DTypeMaxValue(outTile));
    }
    return xiEltwiseAdd3D_ref(inTile1, inTile2, outTile, param);
}

INLINE XI_ERR_TYPE _proto_xiEltwiseMul3D_reff(const xi_pTile3D inTile1,
                                              const xi_pTile3D inTile2,
                                              xi_pTile3D outTile,
                                              xi_cnn_eltwiseMul_params *param)
{
    if (XI_CNN_ELTWISE_MUL_GET_RELUFLAG(param)) {
        XI_CNN_ELTWISE_MUL_SET_RELUFLAG(param, 1);
        XI_CNN_ELTWISE_MUL_SET_MAX_VAL(param, xiTile3DTypeMaxValue(outTile));
    }
    return xiEltwiseMul3D_ref(inTile1, inTile2, outTile, param);
}
#endif

#ifdef __cplusplus
}
#endif


#endif /* _CNNRT_XI_P6_H_INCLUDED_ */
