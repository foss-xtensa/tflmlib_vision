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
#ifndef __XI_IMGPROC_API_H__
#define __XI_IMGPROC_API_H__

#include "xi_core_api.h"

/* helper macro */

/* temporary space requirement for xiResizeArea */
#define XI_RESIZE_AREA_U8_TMP_SIZE2(dst_width, dst_height, src_full_width, src_full_height) \
        {2 * (dst_width),  (src_full_height)}

#define XI_RESIZE_AREA_U8_TMP_CAPACITY2(dst_width, dst_height, src_full_width, src_full_height) \
        (size_t)(2 * (dst_width) * (src_full_height))

#define XI_RESIZE_AREA_U8Oa_TMP_SIZE2(dst_width, dst_height, src_full_width, src_full_height) \
        {2 * (XI_ALIGN_VALN(dst_width)),  (src_full_height)}

#define XI_RESIZE_AREA_U8Oa_TMP_CAPACITY2(dst_width, dst_height, src_full_width, src_full_height) \
        (size_t)(2 * (XI_ALIGN_VALN(dst_width)) * (src_full_height))

#define XI_RESIZE_AREA_S16_TMP_SIZE(dst_width, dst_height, src_full_width, src_full_height) \
        {(dst_width), (src_full_height)}

#define XI_RESIZE_AREA_S16_TMP_CAPACITY(dst_width, dst_height, src_full_width, src_full_height) \
        (size_t)(2 * (dst_width) * (src_full_height))

#define XI_RESIZE_AREA_S16Oa_TMP_SIZE(dst_width, dst_height, src_full_width, src_full_height) \
        {XI_ALIGN_VALN(dst_width), (src_full_height)}

#define XI_RESIZE_AREA_S16Oa_TMP_CAPACITY(dst_width, dst_height, src_full_width, src_full_height) \
        (size_t)(2 * (XI_ALIGN_VALN(dst_width)) * (src_full_height))


/* temporary space requirement for xiResizeBicubic */
#define XI_RESIZE_BICUBIC_U8_TMP_SIZE(dst_width, dst_height, src_full_width, src_full_height)           {0, 0}
#define XI_RESIZE_BICUBIC_U8Oa_TMP_SIZE(dst_width, dst_height, src_full_width, src_full_height)         {0, 0}
#define XI_RESIZE_BICUBIC_S16_TMP_SIZE(dst_width, dst_height, src_full_width, src_full_height)          {0, 0}
#define XI_RESIZE_BICUBIC_S16Oa_TMP_SIZE(dst_width, dst_height, src_full_width, src_full_height)        {0, 0}
#define XI_RESIZE_BICUBIC_U8_TMP_CAPACITY(dst_width, dst_height, src_full_width, src_full_height)       0
#define XI_RESIZE_BICUBIC_U8Oa_TMP_CAPACITY(dst_width, dst_height, src_full_width, src_full_height)     0
#define XI_RESIZE_BICUBIC_S16_TMP_CAPACITY(dst_width, dst_height, src_full_width, src_full_height)      0
#define XI_RESIZE_BICUBIC_S16Oa_TMP_CAPACITY(dst_width, dst_height, src_full_width, src_full_height)    0

#define XI_RESIZE_BICUBIC_U8_DST_SIZE(dst_width, dst_height, src_full_width, src_full_height)           {dst_width, dst_height}
#define XI_RESIZE_BICUBIC_U8Oa_DST_SIZE(dst_width, dst_height, src_full_width, src_full_height)         {dst_width, dst_height}
#define XI_RESIZE_BICUBIC_S16_DST_SIZE(dst_width, dst_height, src_full_width, src_full_height)          {dst_width, dst_height}
#define XI_RESIZE_BICUBIC_S16Oa_DST_SIZE(dst_width, dst_height, src_full_width, src_full_height)        {dst_width, dst_height}
#define XI_RESIZE_BICUBIC_U8_DST_CAPACITY(dst_width, dst_height, src_full_width, src_full_height)       \
    (size_t)((dst_height) * (dst_width))
#define XI_RESIZE_BICUBIC_U8Oa_DST_CAPACITY(dst_width, dst_height, src_full_width, src_full_height)     \
    (size_t)((dst_height) * XI_ALIGN_VALN(dst_width))
#define XI_RESIZE_BICUBIC_S16_DST_CAPACITY(dst_width, dst_height, src_full_width, src_full_height)      \
    (size_t)(2 * (dst_height) * (dst_width))
#define XI_RESIZE_BICUBIC_S16Oa_DST_CAPACITY(dst_width, dst_height, src_full_width, src_full_height)    \
    (size_t)(2 * (dst_height) * XI_ALIGN_VALN(dst_width))

/* temporary space requirement for xiCalcHist */
#if XCHAL_HAVE_GRIVPEP_HISTOGRAM  ||  XCHAL_HAVE_VISION_HISTOGRAM
// use vector registers only
#   define XI_CALCHIST_TMP_CAPACITY   0
#elif XCHAL_HAVE_VISION
// SIMD_WIDTH histograms by 256 bins
#   define XI_CALCHIST_TMP_CAPACITY   (XI_SIMD_WIDTH * 256 * sizeof(uint16_t))
#else
// 2 U16 histograms by 256 bins
#   define XI_CALCHIST_TMP_CAPACITY   (2 * 256 * sizeof(uint16_t))
#endif

/* flip around vertical axe */
_XI_API_ XI_ERR_TYPE xiFlipHorizontal_I8   (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiFlipHorizontal_I8A  (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiFlipHorizontal_I8A2 (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiFlipHorizontal_I8Oa2(const xi_pArray src, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiFlipHorizontal_I16  (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiFlipHorizontal_I16A (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiFlipHorizontal_I16Oa(const xi_pArray src, xi_pArray dst);


/* flip around horizontal axe */
_XI_API_ XI_ERR_TYPE xiFlipVertical_I8  (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiFlipVertical_I8A2(const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiFlipVertical_I16 (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiFlipVertical_I16A(const xi_pArray src, xi_pArray dst);


/* clockwise rotation (by 90 degrees) */
_XI_API_ XI_ERR_TYPE xiRotateClockwise_I8    (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiRotateClockwise_I8A   (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiRotateClockwise_I8MSIa(const xi_pArray src, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiRotateClockwise_I16    (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiRotateClockwise_I16A   (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiRotateClockwise_I16MSIa(const xi_pArray src, xi_pArray dst);


/* GetRectSubPix */
_XI_API_ XI_ERR_TYPE xiGetRectSubPix_U8_Q16_15   (const xi_pTile src, xi_pArray dst, XI_Q16_15 cx, XI_Q16_15 cy);
_XI_API_ XI_ERR_TYPE xiGetRectSubPix_U8Oa2_Q16_15(const xi_pTile src, xi_pArray dst, XI_Q16_15 cx, XI_Q16_15 cy);
_XI_API_ XI_ERR_TYPE xiGetRectSubPix_S16_Q16_15  (const xi_pTile src, xi_pArray dst, XI_Q16_15 cx, XI_Q16_15 cy);
_XI_API_ XI_ERR_TYPE xiGetRectSubPix_S16Oa_Q16_15(const xi_pTile src, xi_pArray dst, XI_Q16_15 cx, XI_Q16_15 cy);


/* threshold */
_XI_API_ XI_ERR_TYPE xiThreshold2_U8  (const xi_pArray src, xi_pArray dst, int threshold, int minval, int maxval);
_XI_API_ XI_ERR_TYPE xiThreshold2_U8A2(const xi_pArray src, xi_pArray dst, int threshold, int minval, int maxval);
_XI_API_ XI_ERR_TYPE xiThreshold2_S16 (const xi_pArray src, xi_pArray dst, int threshold, int minval, int maxval);
_XI_API_ XI_ERR_TYPE xiThreshold2_S16A(const xi_pArray src, xi_pArray dst, int threshold, int minval, int maxval);
_XI_API_ XI_ERR_TYPE xiThreshold2_S16U8    (const xi_pArray src, xi_pArray dst, int threshold, int minval, int maxval);
_XI_API_ XI_ERR_TYPE xiThreshold2_S16U8A   (const xi_pArray src, xi_pArray dst, int threshold, int minval, int maxval);

#define xiThreshold_U8(src, dst, threshold, maxval)         xiThreshold2_U8(src, dst, threshold, 0, maxval)
#define xiThreshold_U8A2(src, dst, threshold, maxval)       xiThreshold2_U8A2(src, dst, threshold, 0, maxval)
#define xiThreshold_S16(src, dst, threshold, maxval)        xiThreshold2_S16(src, dst, threshold, 0, maxval)
#define xiThreshold_S16A(src, dst, threshold, maxval)       xiThreshold2_S16A(src, dst, threshold, 0, maxval)

#define xiThresholdInv_U8(src, dst, threshold, maxval)      xiThreshold2_U8(src, dst, threshold, maxval, 0)
#define xiThresholdInv_U8A2(src, dst, threshold, maxval)    xiThreshold2_U8A2(src, dst, threshold, maxval, 0)
#define xiThresholdInv_S16(src, dst, threshold, maxval)     xiThreshold2_S16(src, dst, threshold, maxval, 0)
#define xiThresholdInv_S16A(src, dst, threshold, maxval)    xiThreshold2_S16A(src, dst, threshold, maxval, 0)

#define xiThreshold_S16U8(src, dst, threshold, maxval)      xiThreshold2_S16U8(src, dst, threshold, 0, maxval)
#define xiThreshold_S16U8A(src, dst, threshold, maxval)     xiThreshold2_S16U8A(src, dst, threshold, 0, maxval)
#define xiThresholdInv_S16U8(src, dst, threshold, maxval)   xiThreshold2_S16U8(src, dst, threshold, maxval, 0)
#define xiThresholdInv_S16U8A(src, dst, threshold, maxval)  xiThreshold2_S16U8A(src, dst, threshold, maxval, 0)

#define xiThresholdTrunc_U8(src, dst, threshold)   xiMinScalar_U8  ((src), (dst), (threshold))
#define xiThresholdTrunc_U8A2(src, dst, threshold) xiMinScalar_U8A2((src), (dst), (threshold))
#define xiThresholdTrunc_S16(src, dst, threshold)  xiMinScalar_S16 ((src), (dst), (threshold))
#define xiThresholdTrunc_S16A(src, dst, threshold) xiMinScalar_S16A((src), (dst), (threshold))

_XI_API_ XI_ERR_TYPE xiThresholdToZero_U8  (const xi_pArray src, xi_pArray dst, int threshold);
_XI_API_ XI_ERR_TYPE xiThresholdToZero_U8A2(const xi_pArray src, xi_pArray dst, int threshold);
_XI_API_ XI_ERR_TYPE xiThresholdToZero_S16 (const xi_pArray src, xi_pArray dst, int threshold);
_XI_API_ XI_ERR_TYPE xiThresholdToZero_S16A(const xi_pArray src, xi_pArray dst, int threshold);

_XI_API_ XI_ERR_TYPE xiThresholdToZeroInv_U8  (const xi_pArray src, xi_pArray dst, int threshold);
_XI_API_ XI_ERR_TYPE xiThresholdToZeroInv_U8A2(const xi_pArray src, xi_pArray dst, int threshold);
_XI_API_ XI_ERR_TYPE xiThresholdToZeroInv_S16 (const xi_pArray src, xi_pArray dst, int threshold);
_XI_API_ XI_ERR_TYPE xiThresholdToZeroInv_S16A(const xi_pArray src, xi_pArray dst, int threshold);


/* transpose */
_XI_API_ XI_ERR_TYPE xiTranspose_I8    (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiTranspose_I8A   (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiTranspose_I8MSOa(const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiTranspose_I8MSIa(const xi_pArray src, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiTranspose_I16    (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiTranspose_I16A   (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiTranspose_I16MSOa(const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiTranspose_I16MSIa(const xi_pArray src, xi_pArray dst);


/* affine transform helpers */
_XI_API_ XI_ERR_TYPE xiInverseAffine(const xi_affine* affine, xi_affine* inverse);
_XI_API_ XI_ERR_TYPE xiPrepareAffineFpt(const xi_affine* affine, xi_affine_fpt* affine_fpt, xi_bool inverse);
_XI_API_ XI_ERR_TYPE xiProjectRectAffineNearest (const xi_affine_fpt* affine, xi_point pos, xi_size size, xi_point* projected_pos, xi_size* projected_size);
_XI_API_ XI_ERR_TYPE xiProjectRectAffineBilinear(const xi_affine_fpt* affine, xi_point pos, xi_size size, xi_point* projected_pos, xi_size* projected_size);


/* affine transform */
_XI_API_ XI_ERR_TYPE xiWarpAffineBilinear_U8_Q13_18 (const xi_pTile src, xi_pTile dst, const xi_affine_fpt* affine);
_XI_API_ XI_ERR_TYPE xiWarpAffineBilinear_S16_Q13_18(const xi_pTile src, xi_pTile dst, const xi_affine_fpt* affine);
_XI_API_ XI_ERR_TYPE xiWarpAffineNearest_I8_Q13_18  (const xi_pTile src, xi_pTile dst, const xi_affine_fpt* affine);
_XI_API_ XI_ERR_TYPE xiWarpAffineNearest_I16_Q13_18 (const xi_pTile src, xi_pTile dst, const xi_affine_fpt* affine);


/* bi-linear image resize */
_XI_API_ XI_ERR_TYPE xiResizeBilinear2_U8_Q13_18   (const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift XI_DEFAULT(0), XI_Q13_18 yshift XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiResizeBilinear2_U8Oa2_Q13_18(const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift XI_DEFAULT(0), XI_Q13_18 yshift XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiResizeBilinear2_S16_Q13_18  (const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift XI_DEFAULT(0), XI_Q13_18 yshift XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiResizeBilinear2_S16Oa_Q13_18(const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift XI_DEFAULT(0), XI_Q13_18 yshift XI_DEFAULT(0));


/* bi-cubic image resize */
_XI_API_ XI_ERR_TYPE xiResizeBicubic2_U8_Q13_18   (const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift XI_DEFAULT(0), XI_Q13_18 yshift XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiResizeBicubic2_U8Oa_Q13_18 (const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift XI_DEFAULT(0), XI_Q13_18 yshift XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiResizeBicubic2_S16_Q13_18  (const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift XI_DEFAULT(0), XI_Q13_18 yshift XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiResizeBicubic2_S16Oa_Q13_18(const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift XI_DEFAULT(0), XI_Q13_18 yshift XI_DEFAULT(0));

_XI_API_ XI_ERR_TYPE xiResizeBicubic_U8_Q13_18   (const xi_pTile src, xi_pTile dst, xi_pArray tmpbuf, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift XI_DEFAULT(0), XI_Q13_18 yshift XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiResizeBicubic_U8Oa_Q13_18 (const xi_pTile src, xi_pTile dst, xi_pArray tmpbuf, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift XI_DEFAULT(0), XI_Q13_18 yshift XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiResizeBicubic_S16_Q13_18  (const xi_pTile src, xi_pTile dst, xi_pArray tmpbuf, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift XI_DEFAULT(0), XI_Q13_18 yshift XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiResizeBicubic_S16Oa_Q13_18(const xi_pTile src, xi_pTile dst, xi_pArray tmpbuf, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift XI_DEFAULT(0), XI_Q13_18 yshift XI_DEFAULT(0));


/* nearest-neighbor image resize */
_XI_API_ XI_ERR_TYPE xiResizeNearest_I8_Q13_18   (const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift XI_DEFAULT(0), XI_Q13_18 yshift XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiResizeNearest_I8Oa2_Q13_18(const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift XI_DEFAULT(0), XI_Q13_18 yshift XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiResizeNearest_S16_Q13_18  (const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift XI_DEFAULT(0), XI_Q13_18 yshift XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiResizeNearest_S16Oa_Q13_18(const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift XI_DEFAULT(0), XI_Q13_18 yshift XI_DEFAULT(0));


/* area image resize */
_XI_API_ XI_ERR_TYPE xiResizeArea_U8_Q13_18   (const xi_pTile src, xi_pTile dst, xi_pArray tmpbuf, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift XI_DEFAULT(0), XI_Q13_18 yshift XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiResizeArea_U8Oa_Q13_18 (const xi_pTile src, xi_pTile dst, xi_pArray tmpbuf, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift XI_DEFAULT(0), XI_Q13_18 yshift XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiResizeArea_S16_Q13_18  (const xi_pTile src, xi_pTile dst, xi_pArray tmpbuf, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift XI_DEFAULT(0), XI_Q13_18 yshift XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiResizeArea_S16Oa_Q13_18(const xi_pTile src, xi_pTile dst, xi_pArray tmpbuf, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift XI_DEFAULT(0), XI_Q13_18 yshift XI_DEFAULT(0));


/* Canny */
_XI_API_ XI_ERR_TYPE xiCanny_3x3_U8_L1(const xi_pTile src, xi_pArray dst, XI_Q28_3 low_threshold, XI_Q28_3 high_threshold, xi_bool trace_edges);
_XI_API_ XI_ERR_TYPE xiCanny_3x3_U8_L2(const xi_pTile src, xi_pArray dst, unsigned low_threshold, unsigned high_threshold, xi_bool trace_edges);

_XI_API_ XI_ERR_TYPE xiCanny_3x3_S16U8_L1(const xi_pTile src, xi_pArray dst, unsigned low_threshold, unsigned high_threshold, xi_bool trace_edges);
_XI_API_ XI_ERR_TYPE xiCanny_3x3_S16U8_L2(const xi_pTile src, xi_pArray dst, unsigned low_threshold, unsigned high_threshold, xi_bool trace_edges);

_XI_API_ XI_ERR_TYPE xiCanny_5x5_U8_L1(const xi_pTile src, xi_pArray dst, unsigned low_threshold, unsigned high_threshold, xi_bool trace_edges);
_XI_API_ XI_ERR_TYPE xiCanny_5x5_U8_L2(const xi_pTile src, xi_pArray dst, unsigned low_threshold, unsigned high_threshold, xi_bool trace_edges);

_XI_API_ XI_ERR_TYPE xiCanny_5x5_S16U8_L1(const xi_pTile src, xi_pArray dst, unsigned low_threshold, unsigned high_threshold, xi_bool trace_edges);
_XI_API_ XI_ERR_TYPE xiCanny_5x5_S16U8_L2(const xi_pTile src, xi_pArray dst, unsigned low_threshold, unsigned high_threshold, xi_bool trace_edges);

_XI_API_ XI_ERR_TYPE xiCanny_7x7_U8_L1(const xi_pTile src, xi_pArray dst, unsigned low_threshold, unsigned high_threshold, xi_bool trace_edges);
_XI_API_ XI_ERR_TYPE xiCanny_7x7_U8_L2(const xi_pTile src, xi_pArray dst, unsigned low_threshold, unsigned high_threshold, xi_bool trace_edges);

_XI_API_ XI_ERR_TYPE xiCanny_7x7_S16U8_L1(const xi_pTile src, xi_pArray dst, unsigned low_threshold, unsigned high_threshold, xi_bool trace_edges);
_XI_API_ XI_ERR_TYPE xiCanny_7x7_S16U8_L2(const xi_pTile src, xi_pArray dst, unsigned low_threshold, unsigned high_threshold, xi_bool trace_edges);


/* Laplacian filters */
_XI_API_ XI_ERR_TYPE xiLaplacian_3x3_U8S16  (const xi_pTile src, xi_pArray dst, xi_bool normalize XI_DEFAULT(false));
_XI_API_ XI_ERR_TYPE xiLaplacian_3x3_U8S16Oa(const xi_pTile src, xi_pArray dst, xi_bool normalize XI_DEFAULT(false));
_XI_API_ XI_ERR_TYPE xiLaplacian_3x3_S16    (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiLaplacian_3x3_S16Oa  (const xi_pTile src, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiLaplacian2_3x3_U8S16  (const xi_pTile src, xi_pArray dst, xi_bool normalize XI_DEFAULT(false));
_XI_API_ XI_ERR_TYPE xiLaplacian2_3x3_U8S16Oa(const xi_pTile src, xi_pArray dst, xi_bool normalize XI_DEFAULT(false));
_XI_API_ XI_ERR_TYPE xiLaplacian2_3x3_S16    (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiLaplacian2_3x3_S16Oa  (const xi_pTile src, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiLaplacian3_3x3_U8S16  (const xi_pTile src, xi_pArray dst, xi_bool normalize XI_DEFAULT(false));
_XI_API_ XI_ERR_TYPE xiLaplacian3_3x3_U8S16Oa(const xi_pTile src, xi_pArray dst, xi_bool normalize XI_DEFAULT(false));
_XI_API_ XI_ERR_TYPE xiLaplacian3_3x3_S16    (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiLaplacian3_3x3_S16Oa  (const xi_pTile src, xi_pArray dst);


/* Scharr gradients */
_XI_API_ XI_ERR_TYPE xiScharr_3x3_U8S16  (const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy, xi_bool normalize XI_DEFAULT(false));
_XI_API_ XI_ERR_TYPE xiScharr_3x3_U8S16Oa(const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy, xi_bool normalize XI_DEFAULT(false));
_XI_API_ XI_ERR_TYPE xiScharr_3x3_S16    (const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy);
_XI_API_ XI_ERR_TYPE xiScharr_3x3_S16Oa  (const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy);


/* Sobel gradients */
_XI_API_ XI_ERR_TYPE xiSobelX_3x3_U8S16   (const xi_pTile src, xi_pArray dst_dx, xi_bool normalize XI_DEFAULT(false));
_XI_API_ XI_ERR_TYPE xiSobelX_3x3_U8S16Oa (const xi_pTile src, xi_pArray dst_dx, xi_bool normalize XI_DEFAULT(false));
_XI_API_ XI_ERR_TYPE xiSobelX_3x3_S16     (const xi_pTile src, xi_pArray dst_dx);
_XI_API_ XI_ERR_TYPE xiSobelX_3x3_S16Oa   (const xi_pTile src, xi_pArray dst_dx);
_XI_API_ XI_ERR_TYPE xiSobelX_3x3_S16S32  (const xi_pTile src, xi_pArray dst_dx, xi_bool normalize XI_DEFAULT(false));
_XI_API_ XI_ERR_TYPE xiSobelX_3x3_S16S32Oa(const xi_pTile src, xi_pArray dst_dx, xi_bool normalize XI_DEFAULT(false));

_XI_API_ XI_ERR_TYPE xiSobelY_3x3_U8S16   (const xi_pTile src, xi_pArray dst_dy, xi_bool normalize XI_DEFAULT(false));
_XI_API_ XI_ERR_TYPE xiSobelY_3x3_U8S16Oa (const xi_pTile src, xi_pArray dst_dy, xi_bool normalize XI_DEFAULT(false));
_XI_API_ XI_ERR_TYPE xiSobelY_3x3_S16     (const xi_pTile src, xi_pArray dst_dy);
_XI_API_ XI_ERR_TYPE xiSobelY_3x3_S16Oa   (const xi_pTile src, xi_pArray dst_dy);
_XI_API_ XI_ERR_TYPE xiSobelY_3x3_S16S32  (const xi_pTile src, xi_pArray dst_dy, xi_bool normalize XI_DEFAULT(false));
_XI_API_ XI_ERR_TYPE xiSobelY_3x3_S16S32Oa(const xi_pTile src, xi_pArray dst_dy, xi_bool normalize XI_DEFAULT(false));

_XI_API_ XI_ERR_TYPE xiSobel_3x3_U8S16   (const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy, xi_bool normalize XI_DEFAULT(false));
_XI_API_ XI_ERR_TYPE xiSobel_3x3_U8S16Oa (const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy, xi_bool normalize XI_DEFAULT(false));
_XI_API_ XI_ERR_TYPE xiSobel_3x3_S16     (const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy);
_XI_API_ XI_ERR_TYPE xiSobel_3x3_S16Oa   (const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy);
_XI_API_ XI_ERR_TYPE xiSobel_3x3_S16S32  (const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy, xi_bool normalize);
_XI_API_ XI_ERR_TYPE xiSobel_3x3_S16S32Oa(const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy, xi_bool normalize);

_XI_API_ XI_ERR_TYPE xiSobel_5x5_U8S16  (const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy, xi_bool normalize XI_DEFAULT(false));
_XI_API_ XI_ERR_TYPE xiSobel_5x5_U8S16Oa(const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy, xi_bool normalize XI_DEFAULT(false));
_XI_API_ XI_ERR_TYPE xiSobel_5x5_S16    (const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy);


/* Filter2D */
_XI_API_ int xiFilter2DComputeLshift(const XI_Q15* kernel, int N, int rshift, int bias);

_XI_API_ XI_ERR_TYPE xiFilter2D_R_U8   (const xi_pTile src, xi_pArray dst, xi_pArray tmp, const XI_Q15* kernel, int ksize, int lshift XI_DEFAULT(-1), int rshift XI_DEFAULT(0), int bias XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiFilter2D_R_S16  (const xi_pTile src, xi_pArray dst, xi_pArray tmp, const XI_Q15* kernel, int ksize, int rshift XI_DEFAULT(0), int bias XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiFilter2D_R_S16Oa(const xi_pTile src, xi_pArray dst, const XI_Q15* kernel, int ksize, int rshift XI_DEFAULT(0), int bias XI_DEFAULT(0));

_XI_API_ XI_ERR_TYPE xiFilter2D_D_U8   (const xi_pTile src, xi_pArray dst, xi_pArray tmp, const XI_Q15* kernel, int ksize, int lshift XI_DEFAULT(-1), int divisor XI_DEFAULT(1), int bias XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiFilter2D_D_S16  (const xi_pTile src, xi_pArray dst, xi_pArray tmp, const XI_Q15* kernel, int ksize, int divisor XI_DEFAULT(1), int bias XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiFilter2D_D_S16Oa(const xi_pTile src, xi_pArray dst, const XI_Q15* kernel, int ksize, int divisor XI_DEFAULT(1), int bias XI_DEFAULT(0));


/* Separable Filter2D */
_XI_API_ int xiSepFilter2DComputeLshift(const XI_Q15* xkernel, const XI_Q15* ykernel, int N, int rshift, int bias);

_XI_API_ XI_ERR_TYPE xiSepFilter2D_R_U8 (const xi_pTile src, xi_pArray dst, xi_pArray tmp, const XI_Q15* xkernel, const XI_Q15* ykernel, int ksize, int lshift XI_DEFAULT(-1), int rshift XI_DEFAULT(0), int bias XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiSepFilter2D_R_S16(const xi_pTile src, xi_pArray dst, xi_pArray tmp, const XI_Q15* xkernel, const XI_Q15* ykernel, int ksize, int rshift XI_DEFAULT(0), int bias XI_DEFAULT(0));

_XI_API_ XI_ERR_TYPE xiSepFilter2D_D_U8 (const xi_pTile src, xi_pArray dst, xi_pArray tmp, const XI_Q15* xkernel, const XI_Q15* ykernel, int ksize, int lshift XI_DEFAULT(-1), int divisor XI_DEFAULT(1), int bias XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiSepFilter2D_D_S16(const xi_pTile src, xi_pArray dst, xi_pArray tmp, const XI_Q15* xkernel, const XI_Q15* ykernel, int ksize, int divisor XI_DEFAULT(1), int bias XI_DEFAULT(0));


/* OpenVX-style custom convolution */
_XI_API_ XI_ERR_TYPE xiConvolve_RS_U8    (const xi_pTile src, xi_pArray dst, const int16_t* kernel, int kwidth, int kheight, int shift);
_XI_API_ XI_ERR_TYPE xiConvolve_RS_U8S16 (const xi_pTile src, xi_pArray dst, const int16_t* kernel, int kwidth, int kheight, int shift);
_XI_API_ XI_ERR_TYPE xiConvolve_RS_S16   (const xi_pTile src, xi_pArray dst, const int16_t* kernel, int kwidth, int kheight, int shift);
_XI_API_ XI_ERR_TYPE xiConvolve_RS_S16S32(const xi_pTile src, xi_pArray dst, const int16_t* kernel, int kwidth, int kheight, int shift);

_XI_API_ XI_ERR_TYPE xiConvolve2_RS_U8(const xi_pTile src, xi_pArray dst, const int16_t* kernel, int kwidth, int kheight, int shift);


/* bilateral filter 5x5 */
_XI_API_ XI_ERR_TYPE xiGetBilateralFilterLUT64_U8 (XI_Q5_10* range_lut, float range_sigma);
_XI_API_ XI_ERR_TYPE xiGetBilateralFilterLUT64_S16(XI_Q15* range_lut, float range_sigma);

_XI_API_ XI_ERR_TYPE xiBilateralFilter_5x5_U8_lut64 (const xi_pTile src, xi_pArray dst, xi_pArray tmp, const XI_Q5_10* range_lut);
_XI_API_ XI_ERR_TYPE xiBilateralFilter_5x5_S16_lut64(const xi_pTile src, xi_pArray dst, const XI_Q15* range_lut);

_XI_API_ XI_ERR_TYPE xiBilateralFilter_9x9_U8_lut64 (const xi_pTile src, xi_pArray dst, const XI_Q5_10* range_lut);
_XI_API_ XI_ERR_TYPE xiBilateralFilter_9x9_S16_lut64(const xi_pTile src, xi_pArray dst, const XI_Q15* range_lut);


/* normalized box filter */
_XI_API_ XI_ERR_TYPE xiBoxFilter_3x3_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiBoxFilter_3x3_U8Oa (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiBoxFilter_3x3_S16  (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiBoxFilter_3x3_S16Oa(const xi_pTile src, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiBoxFilter_5x5_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiBoxFilter_5x5_U8Oa (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiBoxFilter_5x5_S16  (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiBoxFilter_5x5_S16Oa(const xi_pTile src, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiBoxFilter_U8   (const xi_pTile src, xi_pArray dst, xi_pArray tmp, int ksize);
_XI_API_ XI_ERR_TYPE xiBoxFilter_U8Oa (const xi_pTile src, xi_pArray dst, xi_pArray tmp, int ksize);
_XI_API_ XI_ERR_TYPE xiBoxFilter_S16  (const xi_pTile src, xi_pArray dst, xi_pArray tmp, int ksize);
_XI_API_ XI_ERR_TYPE xiBoxFilter_S16Oa(const xi_pTile src, xi_pArray dst, xi_pArray tmp, int ksize);


/* box filter */
_XI_API_ XI_ERR_TYPE xiBoxFilter_3x3_U8U16   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiBoxFilter_3x3_U8U16Oa (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiBoxFilter_3x3_S16S32  (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiBoxFilter_3x3_S16S32Oa(const xi_pTile src, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiBoxFilter_5x5_U8U16   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiBoxFilter_5x5_U8U16Oa (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiBoxFilter_5x5_S16S32  (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiBoxFilter_5x5_S16S32Oa(const xi_pTile src, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiBoxFilter_U8U16   (const xi_pTile src, xi_pArray dst, xi_pArray tmp, int ksize);
_XI_API_ XI_ERR_TYPE xiBoxFilter_U8U16Oa (const xi_pTile src, xi_pArray dst, int ksize);
_XI_API_ XI_ERR_TYPE xiBoxFilter_S16S32  (const xi_pTile src, xi_pArray dst, xi_pArray tmp, int ksize);
_XI_API_ XI_ERR_TYPE xiBoxFilter_S16S32Oa(const xi_pTile src, xi_pArray dst, int ksize);


/* erode (min filter) */
_XI_API_ XI_ERR_TYPE xiErode_1x2_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErode_1x3_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErode_1x4_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErode_1x5_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErode_1x6_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErode_1x7_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErode_1x8_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErode_1x9_U8   (const xi_pTile src, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiErode_2x1_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErode_3x1_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErode_4x1_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErode_5x1_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErode_6x1_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErode_7x1_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErode_8x1_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErode_9x1_U8   (const xi_pTile src, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiErode_3x3_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErode_3x3_U8Oa2(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErode_3x3_S16  (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErode_3x3_S16Oa(const xi_pTile src, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiErode_5x5_U8   (const xi_pTile src, xi_pArray dst, xi_pArray tmp);
_XI_API_ XI_ERR_TYPE xiErode_5x5_U8Oa2(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErode_5x5_S16  (const xi_pTile src, xi_pArray dst, xi_pArray tmp);
_XI_API_ XI_ERR_TYPE xiErode_5x5_S16Oa(const xi_pTile src, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiErode_U8    (const xi_pTile src, xi_pArray dst, xi_pArray tmp, int ksize);
_XI_API_ XI_ERR_TYPE xiErode_NxM_U8(const xi_pTile src, xi_pArray dst, xi_pArray tmp, int kwidth, int kheight);
_XI_API_ XI_ERR_TYPE xiErode_S16   (const xi_pTile src, xi_pArray dst, xi_pArray tmp, int ksize);

/* erode pattern*/
_XI_API_ XI_ERR_TYPE xiErodePatternCross_3x3_U8Oa2(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErodePatternCross_5x5_U8Oa2(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErodePatternCross_7x7_U8Oa2(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErodePatternCross_3x3_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErodePatternCross_5x5_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErodePatternCross_7x7_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErodePatternCross_9x9_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErodePatternDisk_5x5_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErodePatternDisk_5x5_U8Oa2(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErodePatternDisk_7x7_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErodePatternDisk_7x7_U8Oa2(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErodePatternDisk_9x9_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErodePatternDisk_9x9_U8Oa2(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiErodePattern_3x3_U8(const xi_pTile src, xi_pArray dst, const uint8_t *mask);
_XI_API_ XI_ERR_TYPE xiErodePattern_NxM_U8(const xi_pTile src, xi_pArray dst, const uint8_t * mask, int kwidth, int kheight);

/* dilate (max filter) */
_XI_API_ XI_ERR_TYPE xiDilate_1x2_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilate_1x3_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilate_1x4_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilate_1x5_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilate_1x6_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilate_1x7_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilate_1x8_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilate_1x9_U8   (const xi_pTile src, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiDilate_2x1_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilate_3x1_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilate_4x1_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilate_5x1_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilate_6x1_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilate_7x1_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilate_8x1_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilate_9x1_U8   (const xi_pTile src, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiDilate_3x3_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilate_3x3_U8Oa2(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilate_3x3_S16  (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilate_3x3_S16Oa(const xi_pTile src, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiDilate_5x5_U8   (const xi_pTile src, xi_pArray dst, xi_pArray tmp);
_XI_API_ XI_ERR_TYPE xiDilate_5x5_U8Oa2(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilate_5x5_S16  (const xi_pTile src, xi_pArray dst, xi_pArray tmp);
_XI_API_ XI_ERR_TYPE xiDilate_5x5_S16Oa(const xi_pTile src, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiDilate_U8    (const xi_pTile src, xi_pArray dst, xi_pArray tmp, int ksize);
_XI_API_ XI_ERR_TYPE xiDilate_NxM_U8(const xi_pTile src, xi_pArray dst, xi_pArray tmp, int kwidth, int kheight);
_XI_API_ XI_ERR_TYPE xiDilate_S16   (const xi_pTile src, xi_pArray dst, xi_pArray tmp, int ksize);

/* dilate pattern*/
_XI_API_ XI_ERR_TYPE xiDilatePatternCross_3x3_U8Oa2(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilatePatternCross_5x5_U8Oa2(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilatePatternCross_7x7_U8Oa2(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilatePatternCross_3x3_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilatePatternCross_5x5_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilatePatternCross_7x7_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilatePatternCross_9x9_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilatePatternDisk_5x5_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilatePatternDisk_5x5_U8Oa2(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilatePatternDisk_7x7_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilatePatternDisk_7x7_U8Oa2(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilatePatternDisk_9x9_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilatePatternDisk_9x9_U8Oa2(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDilatePattern_3x3_U8(const xi_pTile src, xi_pArray dst, const uint8_t *mask);
_XI_API_ XI_ERR_TYPE xiDilatePattern_NxM_U8(const xi_pTile src, xi_pArray dst, const uint8_t * mask, int kwidth, int kheight);


/* non maxima suppression */
_XI_API_ XI_ERR_TYPE xiNonMaximaSuppression2D_3x3_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiNonMaximaSuppression2D_3x3_U8Oa2(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiNonMaximaSuppression2D_3x3_S16  (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiNonMaximaSuppression2D_3x3_U16  (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiNonMaximaSuppression2D_3x3_S16Oa(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiNonMaximaSuppression2D_3x3_U16Oa(const xi_pTile src, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiNonMaximaSuppressionAsym2D_3x3_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiNonMaximaSuppressionAsym2D_3x3_U8Oa2(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiNonMaximaSuppressionAsym2D_3x3_S16  (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiNonMaximaSuppressionAsym2D_3x3_S16Oa(const xi_pTile src, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiNonMaximaSuppression3D_3x3x3_U8   (const xi_pTile prev, const xi_pTile curr, const xi_pTile next, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiNonMaximaSuppression3D_3x3x3_U8Oa2(const xi_pTile prev, const xi_pTile curr, const xi_pTile next, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiNonMaximaSuppression3D_3x3x3_S16  (const xi_pTile prev, const xi_pTile curr, const xi_pTile next, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiNonMaximaSuppression3D_3x3x3_S16Oa(const xi_pTile prev, const xi_pTile curr, const xi_pTile next, xi_pArray dst);


/* integral image */
_XI_API_ XI_ERR_TYPE xiIntegral_U8U16 (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiIntegral_U8U32 (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiIntegralTopLeftZero_U8U32 (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiIntegral_I16   (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiIntegral_S16S32(const xi_pArray src, xi_pArray dst);


/* integral image of squared values*/
_XI_API_ XI_ERR_TYPE xiIntegralSqr_U8U32 (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiIntegralSqr_S16U32(const xi_pArray src, xi_pArray dst);


/* OpenVX-style Integral Image */
_XI_API_ XI_ERR_TYPE xiIntegralImage_U8U32(const xi_pArray src, const xi_pArray dst_left, const xi_pArray dst_top, xi_pArray dst);


/* 45-degree rotated integral image */
_XI_API_ XI_ERR_TYPE xiIntegral45_U8U16  (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiIntegral45_U8U16A (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiIntegral45_U8U32  (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiIntegral45_U8U32A (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiIntegral45_I16    (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiIntegral45_I16A   (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiIntegral45_S16S32 (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiIntegral45_S16S32A(const xi_pArray src, xi_pArray dst);


/* Gaussian Blur */
_XI_API_ XI_ERR_TYPE xiGetGaussianKernel(XI_Q15* kernel, int ksize, float sigma);

_XI_API_ XI_ERR_TYPE xiGaussianBlur_3x3_U8     (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiGaussianBlur_R_3x3_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiGaussianBlur_3x3_U8Oa2  (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiGaussianBlur_R_3x3_U8Oa2(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiGaussianBlur_3x3_S16    (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiGaussianBlur_R_3x3_S16  (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiGaussianBlur_R_3x3_S16Oa(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiGaussianBlur_3x3_S16Oa  (const xi_pTile src, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiGaussianBlur_5x5_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiGaussianBlur_5x5_S16  (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiGaussianBlur_5x5_S16Oa(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiGaussianBlur_5x5_S16_high_precision  (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiGaussianBlur_5x5_S16Oa_high_precision(const xi_pTile src, xi_pArray dst);


/* Blur image and downsample it */
_XI_API_ XI_ERR_TYPE xiPyrDown_3x3_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiPyrDown_3x3_U8Oa (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiPyrDown_3x3_S16  (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiPyrDown_3x3_S16Oa(const xi_pTile src, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiPyrDown_U8    (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiPyrDown_R_U8  (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiPyrDown_U8Oa  (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiPyrDown_R_U8Oa(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiPyrDown_S16   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiPyrDown_S16Oa (const xi_pTile src, xi_pArray dst);


/* Fused construction step for OpenVX Laplacian pyramid */
_XI_API_ XI_ERR_TYPE xiVXLaplacianPyrDown_U8   (const xi_pTile src, xi_pArray dst, xi_pArray dst_next);
_XI_API_ XI_ERR_TYPE xiVXLaplacianPyrDown_U8Oa2(const xi_pTile src, xi_pArray dst, xi_pArray dst_next);


/* Fused reconstruction step for OpenVX Laplacian pyramid */
/* dst = upsample(src + laplacian) */
_XI_API_ XI_ERR_TYPE xiVXLaplacianReconstruct1_S16 (const xi_pArray src, const xi_pArray laplacian, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiVXLaplacianReconstruct1_S16A(const xi_pArray src, const xi_pArray laplacian, xi_pArray dst);
/* dst = src + upsample(laplacian) */
_XI_API_ XI_ERR_TYPE xiVXLaplacianReconstruct2_S16 (const xi_pArray src, const xi_pArray laplacian, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiVXLaplacianReconstruct2_S16A(const xi_pArray src, const xi_pArray laplacian, xi_pArray dst);


/* Median Blur filtering */
_XI_API_ XI_ERR_TYPE xiMedianBlur_3x3_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMedianBlur_3x3_U8Oa2(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMedianBlur_3x3_S16(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMedianBlur_3x3_S16Oa(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMedianBlur_5x5_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMedianBlurPatternCross_3x3_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMedianBlurPatternCross_3x3_U8Oa2(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMedianBlurPatternCross_5x5_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMedianBlurPatternCross_7x7_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMedianBlurPatternCross_9x9_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMedianBlurPatternDisk_5x5_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMedianBlurPattern_3x3_U8(const xi_pTile src, xi_pArray dst, const uint8_t* mask);
_XI_API_ XI_ERR_TYPE xiMedianBlurPattern_MxN_U8(const xi_pTile src, xi_pArray dst, const uint8_t* mask, int width_mask, int height_mask);

#define xiMedianBlur_2x1_U8 xiErode_2x1_U8
_XI_API_ XI_ERR_TYPE xiMedianBlur_3x1_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMedianBlur_4x1_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMedianBlur_5x1_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMedianBlur_6x1_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMedianBlur_7x1_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMedianBlur_8x1_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMedianBlur_9x1_U8(const xi_pTile src, xi_pArray dst);

#define xiMedianBlur_1x2_U8 xiErode_1x2_U8
_XI_API_ XI_ERR_TYPE xiMedianBlur_1x3_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMedianBlur_1x4_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMedianBlur_1x5_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMedianBlur_1x6_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMedianBlur_1x7_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMedianBlur_1x8_U8(const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMedianBlur_1x9_U8(const xi_pTile src, xi_pArray dst);


/* Upsample image and blur it */
_XI_API_ XI_ERR_TYPE xiPyrUp_U8   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiPyrUp_S16  (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiPyrUp_S16Oa(const xi_pTile src, xi_pArray dst);


/* Upsample2x */
_XI_API_ XI_ERR_TYPE xiUpsample2XNearest_I8  (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiUpsample2XNearest_I8A2(const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiUpsample2XNearest_I16 (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiUpsample2XNearest_I16Oa(const xi_pArray src, xi_pArray dst);


/* Generates distribution from an image */
_XI_API_ XI_ERR_TYPE xiCalcHist_U8U16(const xi_pArray src, xi_pArray histogram /*inout*/, xi_pArray tmp);
_XI_API_ XI_ERR_TYPE xiCalcHist_U8U32(const xi_pArray src, xi_pArray histogram /*inout*/, xi_pArray tmp);


/* Converts histogram into lookup with equalization */
_XI_API_ XI_ERR_TYPE xiEqualizeHist_U16U8(const xi_pArray histogram, xi_pArray lut);
_XI_API_ XI_ERR_TYPE xiEqualizeHist_U32U8(const xi_pArray histogram, xi_pArray lut);


/* Reduces number of bins in distribution */
_XI_API_ XI_ERR_TYPE xiShrinkHist_U32(const xi_pArray hist_in, xi_pArray hist_out, int offset_in, int range_in);


/* Unpack IYUV to YUV4 BT709 convert functions */
_XI_API_ XI_ERR_TYPE xiCvtColor_UnpackUV_U8  (const xi_pArray uv, xi_pArray u, xi_pArray v);
_XI_API_ XI_ERR_TYPE xiCvtColor_UnpackUV_U8A2(const xi_pArray uv, xi_pArray u, xi_pArray v);


/* RGB to Gray convert functions */
_XI_API_ XI_ERR_TYPE xiCvtColor_U8_RGB2Gray  (const xi_pArray r, const xi_pArray g, const xi_pArray b, xi_pArray y);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A2_RGB2Gray(const xi_pArray r, const xi_pArray g, const xi_pArray b, xi_pArray y);
_XI_API_ XI_ERR_TYPE xiCvtColor_S16_RGB2Gray (const xi_pArray r, const xi_pArray g, const xi_pArray b, xi_pArray y);
_XI_API_ XI_ERR_TYPE xiCvtColor_S16A_RGB2Gray(const xi_pArray r, const xi_pArray g, const xi_pArray b, xi_pArray y);


/* RGB to/from RGBX conversion */
_XI_API_ XI_ERR_TYPE xiCvtColor_U8_RGB2RGBX  (const xi_pArray rgb, xi_pArray rgbx);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A2_RGB2RGBX(const xi_pArray rgb, xi_pArray rgbx);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8_RGBX2RGB  (const xi_pArray rgbx, xi_pArray rgb);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A2_RGBX2RGB(const xi_pArray rgbx, xi_pArray rgb);


/* RGB and RGBX to YUV444 BT709 conversion */
_XI_API_ XI_ERR_TYPE xiCvtColor_U8_RGB2YUV_BT709   (const xi_pArray rgb,  xi_pArray y, xi_pArray u, xi_pArray v);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A2_RGB2YUV_BT709 (const xi_pArray rgb,  xi_pArray y, xi_pArray u, xi_pArray v);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8_RGBX2YUV_BT709  (const xi_pArray rgbx, xi_pArray y, xi_pArray u, xi_pArray v);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A2_RGBX2YUV_BT709(const xi_pArray rgbx, xi_pArray y, xi_pArray u, xi_pArray v);


/* RGB and RGBX to/from YUV422 BT709 conversion */
_XI_API_ XI_ERR_TYPE xiCvtColor_U8_YUV2RGB_422_UYVY_BT709  (const xi_pArray uyvy, xi_pArray rgb);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A2_YUV2RGB_422_UYVY_BT709(const xi_pArray uyvy, xi_pArray rgb);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8_YUV2RGB_422_YUYV_BT709  (const xi_pArray yuyv, xi_pArray rgb);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A2_YUV2RGB_422_YUYV_BT709(const xi_pArray yuyv, xi_pArray rgb);

#if !(XCHAL_HAVE_VISION && XCHAL_VISION_TYPE >= 6)
#define xiCvtColor_U8A2_YUV2RGB_422_UYVY_BT709 xiCvtColor_U8_YUV2RGB_422_UYVY_BT709
#define xiCvtColor_U8A2_YUV2RGB_422_YUYV_BT709 xiCvtColor_U8_YUV2RGB_422_YUYV_BT709
#endif

_XI_API_ XI_ERR_TYPE xiCvtColor_U8_YUV2RGBX_422_UYVY_BT709  (const xi_pArray uyvy, xi_pArray rgbx);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A2_YUV2RGBX_422_UYVY_BT709(const xi_pArray uyvy, xi_pArray rgbx);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8_YUV2RGBX_422_YUYV_BT709  (const xi_pArray yuyv, xi_pArray rgbx);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A2_YUV2RGBX_422_YUYV_BT709(const xi_pArray yuyv, xi_pArray rgbx);

#if !(XCHAL_HAVE_VISION && XCHAL_VISION_TYPE >= 6)
#define xiCvtColor_U8A2_YUV2RGBX_422_UYVY_BT709 xiCvtColor_U8_YUV2RGBX_422_UYVY_BT709
#define xiCvtColor_U8A2_YUV2RGBX_422_YUYV_BT709 xiCvtColor_U8_YUV2RGBX_422_YUYV_BT709
#endif

/* RGB and RGBX to/from YUV420 BT709 conversion */
_XI_API_ XI_ERR_TYPE xiCvtColor_U8_RGB2YUV_420_BT709   (const xi_pArray rgb, xi_pArray y, xi_pArray u, xi_pArray v);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A2_RGB2YUV_420_BT709 (const xi_pArray rgb, xi_pArray y, xi_pArray u, xi_pArray v);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8_RGBX2YUV_420_BT709  (const xi_pArray rgbx, xi_pArray y, xi_pArray u, xi_pArray v);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A2_RGBX2YUV_420_BT709(const xi_pArray rgbx, xi_pArray y, xi_pArray u, xi_pArray v);

_XI_API_ XI_ERR_TYPE xiCvtColor_U8_YUV2RGB_420_BT709   (const xi_pArray y, const xi_pArray u, const xi_pArray v, xi_pArray rgb);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A2_YUV2RGB_420_BT709 (const xi_pArray y, const xi_pArray u, const xi_pArray v, xi_pArray rgb);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8_YUV2RGBX_420_BT709  (const xi_pArray y, const xi_pArray u, const xi_pArray v, xi_pArray rgbx);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A2_YUV2RGBX_420_BT709(const xi_pArray y, const xi_pArray u, const xi_pArray v, xi_pArray rgbx);

_XI_API_ XI_ERR_TYPE xiCvtColor_U8_RGB2YUV_420_NV12_BT709   (const xi_pArray rgb, xi_pArray y, xi_pArray uv);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A2_RGB2YUV_420_NV12_BT709 (const xi_pArray rgb, xi_pArray y, xi_pArray uv);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8_RGBX2YUV_420_NV12_BT709  (const xi_pArray rgbx, xi_pArray y, xi_pArray uv);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A2_RGBX2YUV_420_NV12_BT709(const xi_pArray rgbx, xi_pArray y, xi_pArray uv);

_XI_API_ XI_ERR_TYPE xiCvtColor_U8_YUV2RGB_420_NV12_BT709   (const xi_pArray y, const xi_pArray uv, xi_pArray rgb);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A2_YUV2RGB_420_NV12_BT709 (const xi_pArray y, const xi_pArray uv, xi_pArray rgb);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8_YUV2RGBX_420_NV12_BT709  (const xi_pArray y, const xi_pArray uv, xi_pArray rgbx);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A2_YUV2RGBX_420_NV12_BT709(const xi_pArray y, const xi_pArray uv, xi_pArray rgbx);

_XI_API_ XI_ERR_TYPE xiCvtColor_U8_YUV2RGB_420_NV21_BT709   (const xi_pArray y, const xi_pArray uv, xi_pArray rgb);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A2_YUV2RGB_420_NV21_BT709 (const xi_pArray y, const xi_pArray uv, xi_pArray rgb);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8_YUV2RGBX_420_NV21_BT709  (const xi_pArray y, const xi_pArray uv, xi_pArray rgbx);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A2_YUV2RGBX_420_NV21_BT709(const xi_pArray y, const xi_pArray uv, xi_pArray rgbx);


/* YUV422 to YUV420 conversion */
_XI_API_ XI_ERR_TYPE xiCvtColor_U8_YUYV_NV12  (const xi_pArray yuyv, xi_pArray y, xi_pArray uv);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A2_YUYV_NV12(const xi_pArray yuyv, xi_pArray y, xi_pArray uv);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8_UYVY_NV12  (const xi_pArray uyvy, xi_pArray y, xi_pArray uv);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A2_UYVY_NV12(const xi_pArray uyvy, xi_pArray y, xi_pArray uv);

_XI_API_ XI_ERR_TYPE xiCvtColor_U8_YUYV_IYUV  (const xi_pArray yuyv, xi_pArray y, xi_pArray u, xi_pArray v);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A2_YUYV_IYUV(const xi_pArray yuyv, xi_pArray y, xi_pArray u, xi_pArray v);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8_UYVY_IYUV  (const xi_pArray uyvy, xi_pArray y, xi_pArray u, xi_pArray v);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A2_UYVY_IYUV(const xi_pArray uyvy, xi_pArray y, xi_pArray u, xi_pArray v);


/* RGB to YCrCb conversion */
_XI_API_ XI_ERR_TYPE xiCvtColor_U8_RGB2YCrCb  (const xi_pArray r, const xi_pArray g, const xi_pArray b, xi_pArray y, xi_pArray cr, xi_pArray cb);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A2_RGB2YCrCb(const xi_pArray r, const xi_pArray g, const xi_pArray b, xi_pArray y, xi_pArray cr, xi_pArray cb);
_XI_API_ XI_ERR_TYPE xiCvtColor_S16_RGB2YCrCb (const xi_pArray r, const xi_pArray g, const xi_pArray b, xi_pArray y, xi_pArray cr, xi_pArray cb);
_XI_API_ XI_ERR_TYPE xiCvtColor_S16A_RGB2YCrCb(const xi_pArray r, const xi_pArray g, const xi_pArray b, xi_pArray y, xi_pArray cr, xi_pArray cb);


/* YCrCb to RGB conversion */
_XI_API_ XI_ERR_TYPE xiCvtColor_U8_YCrCb2RGB  (const xi_pArray y, const xi_pArray cr, const xi_pArray cb, xi_pArray r, xi_pArray g, xi_pArray b);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A_YCrCb2RGB (const xi_pArray y, const xi_pArray cr, const xi_pArray cb, xi_pArray r, xi_pArray g, xi_pArray b);
_XI_API_ XI_ERR_TYPE xiCvtColor_S16_YCrCb2RGB (const xi_pArray y, const xi_pArray cr, const xi_pArray cb, xi_pArray r, xi_pArray g, xi_pArray b);
_XI_API_ XI_ERR_TYPE xiCvtColor_S16A_YCrCb2RGB(const xi_pArray y, const xi_pArray cr, const xi_pArray cb, xi_pArray r, xi_pArray g, xi_pArray b);


/* RGB to HSV conversion */
_XI_API_ XI_ERR_TYPE xiCvtColor_U8_RGB2HSV  (const xi_pArray r, const xi_pArray g, const xi_pArray b, xi_pArray h, xi_pArray s, xi_pArray v);
_XI_API_ XI_ERR_TYPE xiCvtColor_U8A_RGB2HSV (const xi_pArray r, const xi_pArray g, const xi_pArray b, xi_pArray h, xi_pArray s, xi_pArray v);
_XI_API_ XI_ERR_TYPE xiCvtColor_S16_RGB2HSV (const xi_pArray r, const xi_pArray g, const xi_pArray b, xi_pArray h, xi_pArray s, xi_pArray v);
_XI_API_ XI_ERR_TYPE xiCvtColor_S16A_RGB2HSV(const xi_pArray r, const xi_pArray g, const xi_pArray b, xi_pArray h, xi_pArray s, xi_pArray v);


/* NCC */
_XI_API_ XI_ERR_TYPE xiNCC_U8Q16(const xi_pArray src, const xi_pArray templ, xi_pArray dst);

/* ZNCC */
_XI_API_ XI_ERR_TYPE xiCompareCcoeffNorm_U8Q15(const xi_pArray src, const xi_pArray templ, xi_pArray temptempl, xi_pArray tempsrc, xi_pArray dst);

/* Hamming */
_XI_API_ XI_ERR_TYPE xiCompareHamming_U8(const xi_pArray src, const xi_pArray templ, xi_pArray dst);

/* L1 */
_XI_API_ XI_ERR_TYPE xiCompareL1_U8(const xi_pArray src, const xi_pArray templ, xi_pArray dst);

/* L2 */
_XI_API_ XI_ERR_TYPE xiCompareL2_U8(const xi_pArray src, const xi_pArray templ, xi_pArray dst);

/*Cross correlation*/
_XI_API_ XI_ERR_TYPE xiCompareCcorr_U8(const xi_pArray src, const xi_pArray templ, xi_pArray dst);

/* L2 Norm*/
_XI_API_ XI_ERR_TYPE xiCompareL2Norm_U8Q16(const xi_pArray src, const xi_pArray templ, xi_pArray dst);

/* Cross correlation Norm*/
_XI_API_ XI_ERR_TYPE xiCompareCcorrNorm_U8Q16(const xi_pArray src, const xi_pArray templ, xi_pArray tempsrcsqr, xi_pArray temptemplsqr, xi_pArray dst);


/* SQDiff */
_XI_API_ XI_ERR_TYPE xiSQDiff_U8Q16(const xi_pArray src, const xi_pArray templ, xi_pArray dst);


/* Census transform */
_XI_API_ XI_ERR_TYPE xiCensusTransform_U8     (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiCensusTransform_U8Oa2  (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiCensusTransform_S16U8  (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiCensusTransform_S16U8Oa(const xi_pTile src, xi_pArray dst);


/* Modified census transform (MCT) */
_XI_API_ XI_ERR_TYPE xiMCT_U8U16   (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMCT_U8U16Oa (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMCT_S16U16  (const xi_pTile src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMCT_S16U16Oa(const xi_pTile src, xi_pArray dst);


/* Revised modified census transform (RMCT) */
_XI_API_ XI_ERR_TYPE xiRMCT_U8U16   (const xi_pTile src, xi_pArray dst, int r);
_XI_API_ XI_ERR_TYPE xiRMCT_U8U16Oa (const xi_pTile src, xi_pArray dst, int r);
_XI_API_ XI_ERR_TYPE xiRMCT_S16U16  (const xi_pTile src, xi_pArray dst, int r);
_XI_API_ XI_ERR_TYPE xiRMCT_S16U16Oa(const xi_pTile src, xi_pArray dst, int r);


/* Perspective transform */
_XI_API_ XI_ERR_TYPE xiPreparePerspective(const xi_perspective* perspective, xi_perspective_fpt* perspective_fpt, xi_bool inverse);
_XI_API_ XI_ERR_TYPE xiProjectRectPerspective(const xi_perspective_fpt* perspective, xi_point pos, xi_size size, xi_point* projected_pos, xi_size* projected_size);

_XI_API_ XI_ERR_TYPE xiWarpPerspective_U8_Q13_18  (const xi_pTile src, xi_pTile dst, const xi_perspective_fpt* perspective);
_XI_API_ XI_ERR_TYPE xiWarpPerspective_U8Oa_Q13_18(const xi_pTile src, xi_pTile dst, const xi_perspective_fpt* perspective);

_XI_API_ XI_ERR_TYPE xiWarpPerspective_S16_Q13_18  (const xi_pTile src, xi_pTile dst, const xi_perspective_fpt* perspective);

_XI_API_ XI_ERR_TYPE xiWarpPerspective_S16Oa_Q13_18(const xi_pTile src, xi_pTile dst, const xi_perspective_fpt* perspective);

#if XCHAL_HAVE_VISION && XCHAL_VISION_TYPE >= 6
#define xiWarpPerspective_S16Oa_Q13_18 xiWarpPerspective_S16_Q13_18
#endif

_XI_API_ XI_ERR_TYPE xiWarpPerspectiveNearest_I8_Q13_18 (const xi_pTile src, xi_pTile dst, const xi_perspective_fpt* perspective);
_XI_API_ XI_ERR_TYPE xiWarpPerspectiveNearest_I16_Q13_18(const xi_pTile src, xi_pTile dst, const xi_perspective_fpt* perspective);


/* Applies generic geometric transformation */
_XI_API_ XI_ERR_TYPE xiRemapNearest_I8(const xi_pTile src, xi_pArray dst, const xi_pArray remap_int);
_XI_API_ XI_ERR_TYPE xiRemapNearest_I16(const xi_pTile src, xi_pArray dst, const xi_pArray remap_int);

_XI_API_ XI_ERR_TYPE xiRemapBilinear_U8(const xi_pTile src, xi_pArray dst, const xi_pArray remap_int, const xi_pArray remap_frac);
_XI_API_ XI_ERR_TYPE xiRemapBilinear_S16(const xi_pTile src, xi_pArray dst, const xi_pArray remap_int, const xi_pArray remap_frac);


/* Spatial moments of raster image: M00, M01, M10, M11, M20, M02 */
_XI_API_ XI_ERR_TYPE xiMoments_U8  (const xi_pArray src, xi_moments* moments);
_XI_API_ XI_ERR_TYPE xiMoments_U8A (const xi_pArray src, xi_moments* moments);
_XI_API_ XI_ERR_TYPE xiMoments_S16 (const xi_pArray src, xi_moments* moments);
_XI_API_ XI_ERR_TYPE xiMoments_S16A(const xi_pArray src, xi_moments* moments);


/* Connection component labeling functions */
_XI_API_ XI_ERR_TYPE xiConnectedComponents4_I8U16 (const xi_pArray src, xi_pArray dst, xi_pArray buf);
_XI_API_ XI_ERR_TYPE xiConnectedComponents4_I8U16A(const xi_pArray src, xi_pArray dst, xi_pArray buf);

_XI_API_ XI_ERR_TYPE xiConnectedComponents8_I8U16 (const xi_pArray src, xi_pArray dst, xi_pArray buf);
_XI_API_ XI_ERR_TYPE xiConnectedComponents8_I8U16A(const xi_pArray src, xi_pArray dst, xi_pArray buf);


/* Ellipse fitting */
#define XI_FIT_MODE_ACCURATE    0
#define XI_FIT_MODE_APPROXIMATE 1

_XI_API_ XI_ERR_TYPE xiFitEllipse_U8(const xi_pArray ptx, const xi_pArray pty, xi_rotated_rect_f* ellipse, int mode XI_DEFAULT(XI_FIT_MODE_ACCURATE));


/* Adaptive threshold */
_XI_API_ XI_ERR_TYPE xiAdaptiveThreshold_3x3_U8   (const xi_pTile src, xi_pArray dst, int maxval XI_DEFAULT(255), XI_Q15_16 c XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiAdaptiveThreshold_3x3_S16U8(const xi_pTile src, xi_pArray dst, int maxval XI_DEFAULT(255), int c XI_DEFAULT(0));

_XI_API_ XI_ERR_TYPE xiAdaptiveThreshold_5x5_U8   (const xi_pTile src, xi_pArray dst, int maxval XI_DEFAULT(255), XI_Q15_16 c XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiAdaptiveThreshold_5x5_S16U8(const xi_pTile src, xi_pArray dst, int maxval XI_DEFAULT(255), int c XI_DEFAULT(0));

_XI_API_ XI_ERR_TYPE xiAdaptiveThreshold_U8   (const xi_pTile src, xi_pArray dst, xi_pArray tmp, int ksize, int maxval XI_DEFAULT(255), XI_Q15_16 c XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiAdaptiveThreshold_S16U8(const xi_pTile src, xi_pArray dst, xi_pArray tmp, int ksize, int maxval XI_DEFAULT(255), int c XI_DEFAULT(0));


/* Applies the standard Hough transform to an image tile */
_XI_API_ XI_ERR_TYPE xiPrepareHoughLines(xi_size size_img, xi_size size_map, xi_pArray buff);
_XI_API_ XI_ERR_TYPE xiHoughLinesTransform_U16(const xi_pArray locations, xi_pTile response_map /*inout*/, xi_pArray buff);
_XI_API_ XI_ERR_TYPE xiHoughLinesExtract_U16(const xi_pTile response_map, int threshold, xi_pArray lines, int* count, xi_pArray buff);
_XI_API_ XI_ERR_TYPE xiHoughLinesExtractNext_U16(const xi_pTile response_map, xi_point start_loc, int threshold, xi_pArray lines, int* count, xi_pArray buff);


/* Hough circles transform */
_XI_API_ XI_ERR_TYPE xiHoughCirclesGetEdgePoints_U8S16S16(const xi_pTile src_canny, const xi_pArray src_sobel_dx, const xi_pArray src_sobel_dy, xi_pArray edge, xi_pArray gradient, int* count);
_XI_API_ XI_ERR_TYPE xiHoughCirclesGetNextEdgePoints_U8S16S16(const xi_pTile src_canny, const xi_pArray src_sobel_dx, const xi_pArray src_sobel_dy, xi_point start_loc, xi_pArray edge, xi_pArray gradient, int* count);
_XI_API_ XI_ERR_TYPE xiHoughCirclesTransform_U16(const xi_pArray edge, const xi_pArray gradient, xi_pTile response_map /*inout*/, int min_radius, int max_radius);
_XI_API_ XI_ERR_TYPE xiHoughCirclesFindRadius(const xi_pArray edge, xi_point center, int* radius, xi_pArray tmp, int threshold, int min_radius, int max_radius);

/*Standard LBP*/
_XI_API_ XI_ERR_TYPE xiLBP_3x3_U8 (const xi_pTile src, xi_pTile dst);
_XI_API_ XI_ERR_TYPE xiLBP_5x5_U8 (const xi_pTile src, xi_pTile dst);

/*Modified LBP*/
_XI_API_ XI_ERR_TYPE xiMLBP_3x3_U8 (const xi_pTile src, xi_pTile dst);
_XI_API_ XI_ERR_TYPE xiMLBP_5x5_U8 (const xi_pTile src, xi_pTile dst);

/*Uniform LBP*/
_XI_API_ XI_ERR_TYPE xiULBP_3x3_U8 (const xi_pTile src, xi_pTile dst);
_XI_API_ XI_ERR_TYPE xiULBP_5x5_U8 (const xi_pTile src, xi_pTile dst);

#endif
