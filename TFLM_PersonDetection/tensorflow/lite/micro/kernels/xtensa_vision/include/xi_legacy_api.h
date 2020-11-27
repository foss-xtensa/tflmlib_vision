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
#ifndef __XI_LEGACY_API_H__
#define __XI_LEGACY_API_H__

#include "xi_core_api.h"

/* temporary space requirement for xiResizeBilinear */
#define XI_RESIZE_BILINEAR_U8_TMP_CAPACITY(dst_width, dst_height, src_full_width, src_full_height) 32
#define XI_RESIZE_BILINEAR_U8_TMP_SIZE(dst_width, dst_height, src_full_width, src_full_height)     (xi_size){32, 1}
#define XI_RESIZE_BILINEAR_U8_DST_CAPACITY(dst_width, dst_height, src_full_width, src_full_height) ((dst_width)*(dst_height))
#define XI_RESIZE_BILINEAR_U8_DST_SIZE(dst_width, dst_height, src_full_width, src_full_height)     (xi_size){(dst_width), (dst_height)}

#define XI_RESIZE_BILINEAR_U8Oa_TMP_CAPACITY(dst_width, dst_height, src_full_width, src_full_height) 32
#define XI_RESIZE_BILINEAR_U8Oa_TMP_SIZE(dst_width, dst_height, src_full_width, src_full_height)     (xi_size){32, 1}
#define XI_RESIZE_BILINEAR_U8Oa_DST_CAPACITY(dst_width, dst_height, src_full_width, src_full_height) (XI_ALIGN_VALN(dst_width)*(dst_height))
#define XI_RESIZE_BILINEAR_U8Oa_DST_SIZE(dst_width, dst_height, src_full_width, src_full_height)     (xi_size){(dst_width), (dst_height)}

#define XI_RESIZE_BILINEAR_S16_TMP_CAPACITY(dst_width, dst_height, src_full_width, src_full_height) 64
#define XI_RESIZE_BILINEAR_S16_TMP_SIZE(dst_width, dst_height, src_full_width, src_full_height)     (xi_size){32, 1}
#define XI_RESIZE_BILINEAR_S16_DST_CAPACITY(dst_width, dst_height, src_full_width, src_full_height) 2 * ((dst_width)*(dst_height))
#define XI_RESIZE_BILINEAR_S16_DST_SIZE(dst_width, dst_height, src_full_width, src_full_height)     (xi_size){(dst_width), (dst_height)}

#define XI_RESIZE_BILINEAR_S16Oa_TMP_CAPACITY(dst_width, dst_height, src_full_width, src_full_height) 64
#define XI_RESIZE_BILINEAR_S16Oa_TMP_SIZE(dst_width, dst_height, src_full_width, src_full_height)     (xi_size){32, 1}
#define XI_RESIZE_BILINEAR_S16Oa_DST_CAPACITY(dst_width, dst_height, src_full_width, src_full_height) 2 * (XI_ALIGN_VALN(dst_width)*(dst_height))
#define XI_RESIZE_BILINEAR_S16Oa_DST_SIZE(dst_width, dst_height, src_full_width, src_full_height)     (xi_size){(dst_width), (dst_height)}

/* temporary space requirement for xiResizeArea */
#define XI_RESIZE_AREA_U8_DST_SIZE(dst_height, src_full_width) \
        {(src_full_width),  (dst_height) + 1}

#define XI_RESIZE_AREA_U8_TMP_SIZE(dst_height, src_full_width) \
        {2 * (src_full_width),  (dst_height) + 1}

#define XI_RESIZE_AREA_U8_DST_CAPACITY(dst_height, src_full_width) \
        (size_t){(src_full_width) * (dst_height) + 1}

#define XI_RESIZE_AREA_U8_TMP_CAPACITY(dst_height, src_full_width) \
        (size_t){2 * (src_full_width) * (dst_height) + 1}


#define XI_RESIZE_AREA_U8Oa_DST_SIZE(dst_height, src_full_width) \
        {(XI_ALIGN_VALN(src_full_width)),  (dst_height)}

#define XI_RESIZE_AREA_U8Oa_TMP_SIZE(dst_height, src_full_width) \
        {2 * (XI_ALIGN_VALN(src_full_width)),  XI_ALIGN_VALN(dst_height) + 1}

#define XI_RESIZE_AREA_U8Oa_DST_CAPACITY(dst_height, src_full_width) \
        (size_t){(XI_ALIGN_VALN(src_full_width)) * (dst_height)}

#define XI_RESIZE_AREA_U8Oa_TMP_CAPACITY(dst_height, src_full_width) \
        (size_t){2 * (XI_ALIGN_VALN(src_full_width)) * XI_ALIGN_VALN(dst_height) + (2 * XI_SIMD_WIDTH - 1)}


/* bilinear resize */
#define xiResizeBilinear_U8_Q13_18(src, dst, tmp, ...) xiResizeBilinear2_U8_Q13_18(src, dst, __VA_ARGS__)
#define xiResizeBilinear_U8Oa_Q13_18 xiResizeBilinear_U8_Q13_18

#define xiResizeBilinear_S16_Q13_18(src, dst, tmp, ...) xiResizeBilinear2_S16_Q13_18(src, dst, __VA_ARGS__)
#define xiResizeBilinear_S16Oa_Q13_18(src, dst, tmp, ...) xiResizeBilinear2_S16Oa_Q13_18(src, dst, __VA_ARGS__)


/* equalize histogram */
#define xiEqualizeHist_U16U8A xiEqualizeHist_U16U8


/* Sobel gradients */
#define xiSobel_5x5_S16Oa xiSobel_5x5_S16


/* separable afffine transform */
typedef struct {
    XI_Q13_18 a1;
    XI_Q13_18 b1;
    XI_Q13_18 c1;
    XI_Q13_18 a2;
    XI_Q13_18 b2;
    XI_Q13_18 c2;
    XI_Q13_18 xscale;
    XI_Q13_18 yscale;
    uint32_t flags;
    uint8_t pixel_res;
} xi_separable_affine;

_XI_API_ XI_ERR_TYPE xiReduceAffineScale(const xi_affine* input, xi_affine* scaled, float* scale_factor, float max_scale XI_DEFAULT(2.0f));
_XI_API_ XI_ERR_TYPE xiPrepareAffine(const xi_affine* affine, xi_size dst_size, int pixel_res, xi_bool aligned, xi_separable_affine* decomposed, unsigned* tmp_buf_size, unsigned* dst_buf_size);
_XI_API_ XI_ERR_TYPE xiProjectRectAffine(const xi_separable_affine* inverse_affine, xi_point pos, xi_size size, xi_point* projected_pos, xi_size* projected_size);

_XI_API_ XI_ERR_TYPE xiWarpAffine_U8_Q13_18  (const xi_pTile src, xi_pTile dst, xi_pArray tmpbuf, const xi_separable_affine* decomposed);
_XI_API_ XI_ERR_TYPE xiWarpAffine_U8A_Q13_18 (const xi_pTile src, xi_pTile dst, xi_pArray tmpbuf, const xi_separable_affine* decomposed);
_XI_API_ XI_ERR_TYPE xiWarpAffine_S16_Q13_18 (const xi_pTile src, xi_pTile dst, xi_pArray tmpbuf, const xi_separable_affine* decomposed);
_XI_API_ XI_ERR_TYPE xiWarpAffine_S16A_Q13_18(const xi_pTile src, xi_pTile dst, xi_pArray tmpbuf, const xi_separable_affine* decomposed);


/* BRIEF descriptor */
#define XI_BRIEF_PATTERN_SIZE 5375
_XI_API_ XI_ERR_TYPE xiBRIEF_48x48_InitializePattern(xi_pArray pattern);
#define xiBRIEF16_48x48_S16U8(src, pattern, ...)   xiBRIEF16_48x48_S16U8_2(src, __VA_ARGS__)
#define xiBRIEF16_48x48_U16U8(src, pattern, ...)   xiBRIEF16_48x48_U16U8_2(src, __VA_ARGS__)
#define xiBRIEF32_48x48_S16U8(src, pattern, ...)   xiBRIEF32_48x48_S16U8_2(src, __VA_ARGS__)
#define xiBRIEF32_48x48_U16U8(src, pattern, ...)   xiBRIEF32_48x48_U16U8_2(src, __VA_ARGS__)
#define xiBRIEF64_48x48_S16U8(src, pattern, ...)   xiBRIEF64_48x48_S16U8_2(src, __VA_ARGS__)
#define xiBRIEF64_48x48_U16U8(src, pattern, ...)   xiBRIEF64_48x48_U16U8_2(src, __VA_ARGS__)


/* Maximally stable extremal region extraction */
typedef struct
{
    int delta;
    int min_area;
    int max_area;
    XI_Q15 max_variation;
    XI_Q15 min_diversity;
} xi_mser_params;
#define XI_MSER_PARAMS_INIT {5, 60, 14400, (XI_Q15)((0.25) * (1 << 15) + 0.5), (XI_Q15)((0.20) * (1 << 15) + 0.5)}

_XI_API_ XI_ERR_TYPE xiMSER_U8(const xi_pArray src, xi_pArray dst, xi_pArray tmp, const xi_mser_params* params);


/* Haar */
_XI_API_ XI_ERR_TYPE xiHaarStump_U16U8_xstep1(const xi_pArray integral_image, const xi_pArray stdDev, int16_t* cascade, xi_pArray detect_map, int ystep, xi_size window, int numStages, int *pskipSum);
_XI_API_ XI_ERR_TYPE xiHaarStump_U16U8_xstep2(const xi_pArray integral_image, const xi_pArray stdDev, int16_t* cascade, xi_pArray detect_map, int ystep, xi_size window, int numStages, int *pskipSum);


/* HOG Histogram */
_XI_API_ XI_ERR_TYPE xiHOGWeighted_S16U32_Q16_15(const xi_pTile src_mag, const xi_pArray src_ang_Q3_12, const xi_pArray weight_Q15, xi_pArray hist, xi_point pt, xi_size cellSize, int nbins);


/* pre-v6.0.0 function names */
#define xiAccumulate_S16 xiAccumulate_I16
#define xiAccumulate_S16A xiAccumulate_I16A
#define xiAccumulate_U8S16 xiAccumulate_U8U16
#define xiAccumulate_U8S16A xiAccumulate_U8U16A
#define xiAccumulateProduct_S16 xiAccumulateProduct_I16
#define xiAccumulateProduct_S16A xiAccumulateProduct_I16A
#define xiAccumulateSquare_S16 xiAccumulateSquare_I16
#define xiAccumulateSquare_S16A xiAccumulateSquare_I16A
#define xiAccumulateSquare_U8S16 xiAccumulateSquare_U8U16
#define xiAccumulateSquare_U8S16A xiAccumulateSquare_U8U16A
#define xiAdd_S16 xiAdd_I16
#define xiAdd_S16A xiAdd_I16A
#define xiAdd_U8 xiAdd_I8
#define xiAdd_U8A2 xiAdd_I8A2
#define xiAddScalar_S16 xiAddScalar_I16
#define xiAddScalar_S16A xiAddScalar_I16A
#define xiAddScalar_U8 xiAddScalar_I8
#define xiAddScalar_U8A2 xiAddScalar_I8A2
#define xiBitwiseAnd_S16 xiBitwiseAnd
#define xiBitwiseAnd_S16A xiBitwiseAnd_A2
#define xiBitwiseAnd_U8 xiBitwiseAnd
#define xiBitwiseAnd_U8A2 xiBitwiseAnd_A2
#define xiBitwiseAndScalar_S16 xiBitwiseAndScalar_I16
#define xiBitwiseAndScalar_S16A xiBitwiseAndScalar_I16A
#define xiBitwiseAndScalar_U8 xiBitwiseAndScalar_I8
#define xiBitwiseAndScalar_U8A2 xiBitwiseAndScalar_I8A2
#define xiBitwiseNot_S16 xiBitwiseNot
#define xiBitwiseNot_S16A xiBitwiseNot_A2
#define xiBitwiseNot_U8 xiBitwiseNot
#define xiBitwiseNot_U8A2 xiBitwiseNot_A2
#define xiBitwiseOr_S16 xiBitwiseOr
#define xiBitwiseOr_S16A xiBitwiseOr_A2
#define xiBitwiseOr_U8 xiBitwiseOr
#define xiBitwiseOr_U8A2 xiBitwiseOr_A2
#define xiBitwiseOrScalar_S16 xiBitwiseOrScalar_I16
#define xiBitwiseOrScalar_S16A xiBitwiseOrScalar_I16A
#define xiBitwiseOrScalar_U8 xiBitwiseOrScalar_I8
#define xiBitwiseOrScalar_U8A2 xiBitwiseOrScalar_I8A2
#define xiBitwiseXor_S16 xiBitwiseXor
#define xiBitwiseXor_S16A xiBitwiseXor_A2
#define xiBitwiseXor_U8 xiBitwiseXor
#define xiBitwiseXor_U8A2 xiBitwiseXor_A2
#define xiBitwiseXorScalar_S16 xiBitwiseXorScalar_I16
#define xiBitwiseXorScalar_S16A xiBitwiseXorScalar_I16A
#define xiBitwiseXorScalar_U8 xiBitwiseXorScalar_I8
#define xiBitwiseXorScalar_U8A2 xiBitwiseXorScalar_I8A2
#define xiCircle_S16 xiCircle_I16
#define xiCircle_S16Sa xiCircle_I16Sa
#define xiCircle_U8 xiCircle_I8
#define xiCircle_U8Sa xiCircle_I8Sa
#define xiCompareEQ_S16U8 xiCompareEQ_I16U8
#define xiCompareEQ_S16U8A xiCompareEQ_I16U8A
#define xiCompareEQ_U8 xiCompareEQ_I8U8
#define xiCompareEQ_U8A2 xiCompareEQ_I8U8A2
#define xiCompareNE_S16U8 xiCompareNE_I16U8
#define xiCompareNE_S16U8A xiCompareNE_I16U8A
#define xiCompareNE_U8 xiCompareNE_I8U8
#define xiCompareNE_U8A2 xiCompareNE_I8U8A2
#define xiCompareScalarEQ_S16U8 xiCompareScalarEQ_I16U8
#define xiCompareScalarEQ_S16U8A xiCompareScalarEQ_I16U8A
#define xiCompareScalarEQ_U8 xiCompareScalarEQ_I8U8
#define xiCompareScalarEQ_U8A2 xiCompareScalarEQ_I8U8A2
#define xiCompareScalarNE_S16U8 xiCompareScalarNE_I16U8
#define xiCompareScalarNE_S16U8A xiCompareScalarNE_I16U8A
#define xiCompareScalarNE_U8 xiCompareScalarNE_I8U8
#define xiCompareScalarNE_U8A2 xiCompareScalarNE_I8U8A2
#define xiConnectedComponents4_U8U16 xiConnectedComponents4_I8U16
#define xiConnectedComponents4_U8U16A xiConnectedComponents4_I8U16A
#define xiConnectedComponents8_U8U16 xiConnectedComponents8_I8U16
#define xiConnectedComponents8_U8U16A xiConnectedComponents8_I8U16A
#define xiCopyTile_S16 xiCopyTile
#define xiCopyTile_U8 xiCopyTile
#define xiCountEqual_S16 xiCountEqual_I16
#define xiCountEqual_S16A xiCountEqual_I16A
#define xiCountEqual_U8 xiCountEqual_I8
#define xiCountEqual_U8A2 xiCountEqual_I8A2
#define xiCountNonZero_S16 xiCountNonZero_I16
#define xiCountNonZero_S16A xiCountNonZero_I16A
#define xiCountNonZero_U8 xiCountNonZero_I8
#define xiCountNonZero_U8A2 xiCountNonZero_I8A2
#define xiExtendEdgesConst_S16 xiExtendEdgesConst_I16
#define xiExtendEdgesConst_U8 xiExtendEdgesConst_I8
#define xiExtendEdgesReflect101_S16 xiExtendEdgesReflect101_I16
#define xiExtendEdgesReflect101_U8 xiExtendEdgesReflect101_I8
#define xiExtendEdgesReplicate_S16 xiExtendEdgesReplicate_I16
#define xiExtendEdgesReplicate_U8 xiExtendEdgesReplicate_I8
#define xiExtractChannel2_U8 xiExtractChannel2_I8
#define xiExtractChannel2_U8A2 xiExtractChannel2_I8A2
#define xiExtractChannel3_U8 xiExtractChannel3_I8
#define xiExtractChannel3_U8A2 xiExtractChannel3_I8A2
#define xiExtractChannel4_U8 xiExtractChannel4_I8
#define xiExtractChannel4_U8A2 xiExtractChannel4_I8A2
#define xiFillTile_S16 xiFillTile_I16
#define xiFillTile_U8 xiFillTile_I8
#define xiFlipHorizontal_S16 xiFlipHorizontal_I16
#define xiFlipHorizontal_S16A xiFlipHorizontal_I16A
#define xiFlipHorizontal_S16Oa xiFlipHorizontal_I16Oa
#define xiFlipHorizontal_U8 xiFlipHorizontal_I8
#define xiFlipHorizontal_U8A xiFlipHorizontal_I8A
#define xiFlipHorizontal_U8A2 xiFlipHorizontal_I8A2
#define xiFlipHorizontal_U8Oa2 xiFlipHorizontal_I8Oa2
#define xiFlipVertical_S16 xiFlipVertical_I16
#define xiFlipVertical_S16A xiFlipVertical_I16A
#define xiFlipVertical_U8 xiFlipVertical_I8
#define xiFlipVertical_U8A2 xiFlipVertical_I8A2
#define xiGatherLocationsEQ_S16 xiGatherLocationsEQ_I16
#define xiGatherLocationsEQ_U8 xiGatherLocationsEQ_I8
#define xiGatherNextLocationsEQ_S16 xiGatherNextLocationsEQ_I16
#define xiGatherNextLocationsEQ_U8 xiGatherNextLocationsEQ_I8
#define xiIntegral45_S16 xiIntegral45_I16
#define xiIntegral45_S16A xiIntegral45_I16A
#define xiIntegral_S16 xiIntegral_I16
#define xiLine_S16 xiLine_I16
#define xiLine_U8 xiLine_I8
#define xiMerge2_S16 xiMerge2_I16
#define xiMerge2_S16A xiMerge2_I16A
#define xiMerge2_U8 xiMerge2_I8
#define xiMerge2_U8A2 xiMerge2_I8A2
#define xiMerge3_S16 xiMerge3_I16
#define xiMerge3_S16A xiMerge3_I16A
#define xiMerge3_U8 xiMerge3_I8
#define xiMerge3_U8A2 xiMerge3_I8A2
#define xiMerge4_S16 xiMerge4_I16
#define xiMerge4_S16A xiMerge4_I16A
#define xiMerge4_U8 xiMerge4_I8
#define xiMerge4_U8A2 xiMerge4_I8A2
#define xiMultiply_S16 xiMultiply_I16
#define xiMultiply_S16A xiMultiply_I16A
#define xiMultiply_U8 xiMultiply_I8
#define xiMultiply_U8A2 xiMultiply_I8A2
#define xiMultiplyScalar_S16 xiMultiplyScalar_I16
#define xiMultiplyScalar_S16A xiMultiplyScalar_I16A
#define xiMultiplyScalar_U8 xiMultiplyScalar_I8
#define xiMultiplyScalar_U8A2 xiMultiplyScalar_I8A2
#define xiRectangle_S16 xiRectangle_I16
#define xiRectangle_U8 xiRectangle_I8
#define xiRemapNearest_U8 xiRemapNearest_I8
#define xiResizeNearest_U8_Q13_18 xiResizeNearest_I8_Q13_18
#define xiResizeNearest_U8Oa2_Q13_18 xiResizeNearest_I8Oa2_Q13_18
#define xiRotateClockwise_S16 xiRotateClockwise_I16
#define xiRotateClockwise_S16A xiRotateClockwise_I16A
#define xiRotateClockwise_S16MSIa xiRotateClockwise_I16MSIa
#define xiRotateClockwise_U8 xiRotateClockwise_I8
#define xiRotateClockwise_U8A xiRotateClockwise_I8A
#define xiRotateClockwise_U8MSIa xiRotateClockwise_I8MSIa
#define xiShiftLeft_U32 xiShiftLeft_I32
#define xiShiftLeft_U32A xiShiftLeft_I32A
#define xiSplit2_S16 xiSplit2_I16
#define xiSplit2_S16A xiSplit2_I16A
#define xiSplit2_U8 xiSplit2_I8
#define xiSplit2_U8A2 xiSplit2_I8A2
#define xiSplit3_S16 xiSplit3_I16
#define xiSplit3_S16A xiSplit3_I16A
#define xiSplit3_U8 xiSplit3_I8
#define xiSplit3_U8A2 xiSplit3_I8A2
#define xiSplit4_S16 xiSplit4_I16
#define xiSplit4_S16A xiSplit4_I16A
#define xiSplit4_U8 xiSplit4_I8
#define xiSplit4_U8A2 xiSplit4_I8A2
#define xiSubtract_S16 xiSubtract_I16
#define xiSubtract_S16A xiSubtract_I16A
#define xiSubtract_U8 xiSubtract_I8
#define xiSubtract_U8A2 xiSubtract_I8A2
#define xiSubtractScalar_S16 xiSubtractScalar_I16
#define xiSubtractScalar_S16A xiSubtractScalar_I16A
#define xiSubtractScalar_U8 xiSubtractScalar_I8
#define xiSubtractScalar_U8A2 xiSubtractScalar_I8A2
#define xiTranspose_S16 xiTranspose_I16
#define xiTranspose_S16A xiTranspose_I16A
#define xiTranspose_S16MSIa xiTranspose_I16MSIa
#define xiTranspose_S16MSOa xiTranspose_I16MSOa
#define xiTranspose_U8 xiTranspose_I8
#define xiTranspose_U8A xiTranspose_I8A
#define xiTranspose_U8MSIa xiTranspose_I8MSIa
#define xiTranspose_U8MSOa xiTranspose_I8MSOa
#define xiUpsample2XNearest_U8 xiUpsample2XNearest_I8
#define xiUpsample2XNearest_U8A2 xiUpsample2XNearest_I8A2
#define xiWarpAffineNearest_U8_Q13_18 xiWarpAffineNearest_I8_Q13_18
#define xiWarpPerspectiveNearest_U8_Q13_18 xiWarpPerspectiveNearest_I8_Q13_18

#endif
