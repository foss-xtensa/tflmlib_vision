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
#ifndef __XI_API_REF_H__
#define __XI_API_REF_H__

#include "xi_core_api.h"
#include "xi_legacy_api.h"


#if defined(_MSC_VER) && defined(XI_REF_CREATE_SHARED_LIBRARY)
#  define _XI_EXPORTSREF_ __declspec(dllexport)
#  define _XI_APIREF_ _XI_EXTERN_C_ _XI_EXPORTSREF_
#else
#  define _XI_APIREF_ _XI_API_
#  define _XI_EXPORTSREF_ _XI_EXPORTS_
#endif

/* macros from stdint.h to fix issues when bit-excatness is enabled*/
#ifdef XI_ENABLE_BIT_EXACT_CREF
    #ifndef INT16_MAX
        #define INT16_MAX (32767)
    #endif
    #ifndef INT16_MIN
        #define INT16_MIN (-32767 - 1)
    #endif
    #ifndef UINT32_MAX
        #define UINT32_MAX 0xffffffff
    #endif
    #ifndef UINT8_MAX
        #define UINT8_MAX 0xff
    #endif
#endif

#ifndef TMPSTRIDE
#define TMPSTRIDE 2*XCHAL_IVPN_SIMD_WIDTH
#endif

#define NO_OF_DERIVATIVES 2

/* helper functions used by reference implementation */
_XI_APIREF_ xi_bool xiArrayCheckType_ref(const xi_pArray p, uint16_t type);

_XI_APIREF_ xi_bool xiTileIsValid_ref(const xi_pTile p, int bpp);
_XI_APIREF_ xi_bool xiTileIsValid_S8_ref(const xi_pTile t);
_XI_APIREF_ xi_bool xiTileIsValid_U8_ref  (const xi_pTile t);
_XI_APIREF_ xi_bool xiTileIsValid_U16_ref(const xi_pTile t);
_XI_APIREF_ xi_bool xiTileIsValid_S16_ref (const xi_pTile t);
_XI_APIREF_ xi_bool xiTileIsValid_S32_ref (const xi_pTile t);
_XI_APIREF_ xi_bool xiArrayIsValid_ref    (const xi_pArray p, int bpp);
_XI_APIREF_ xi_bool xiArrayIsValid_S8_ref(const xi_pArray a);
_XI_APIREF_ xi_bool xiArrayIsValid_U8_ref(const xi_pArray a);
_XI_APIREF_ xi_bool xiArrayIsValid_I8_ref(const xi_pArray a);
_XI_APIREF_ xi_bool xiArrayIsValid_S16_ref(const xi_pArray a);
_XI_APIREF_ xi_bool xiArrayIsValid_U16_ref(const xi_pArray a);
_XI_APIREF_ xi_bool xiArrayIsValid_I16_ref(const xi_pArray a);
_XI_APIREF_ xi_bool xiArrayIsValid_U16_ref(const xi_pArray a);
_XI_APIREF_ xi_bool xiArrayIsValid_U32_ref(const xi_pArray a);
_XI_APIREF_ xi_bool xiArrayIsValid_S32_ref(const xi_pArray a);
_XI_APIREF_ xi_bool xiArrayIsValid_F16_ref(const xi_pArray a);
_XI_APIREF_ xi_bool xiArrayIsValid_F32_ref(const xi_pArray a);
_XI_APIREF_ xi_bool xiArraysHaveSameSize_ref(const xi_pArray a, const xi_pArray b);
#define xiTilesHaveSameSize_ref(a, b) xiArraysHaveSameSize_ref((xi_pArray)(a), (xi_pArray)(b))

_XI_APIREF_ xi_bool xiTile3DCheckType_ref(const xi_pTile3D p, uint16_t type);
_XI_APIREF_ xi_bool xiTile3DIsValid_S8_ref(const xi_pTile3D p);
_XI_APIREF_ xi_bool xiTile3DIsValid_U8_ref(const xi_pTile3D p);
_XI_APIREF_ xi_bool xiTile3DIsValid_S16_ref(const xi_pTile3D p);
_XI_APIREF_ xi_bool xiTile3DIsValid_U16_ref(const xi_pTile3D p);
_XI_APIREF_ xi_bool xiTile3DIsValid_U32_ref(const xi_pTile3D p);
_XI_APIREF_ xi_bool xiTile3DIsValid_S32_ref(const xi_pTile3D p);
_XI_APIREF_ xi_bool xiTile3DIsValid_F16_ref(const xi_pTile3D p);
_XI_APIREF_ xi_bool xiTile3DIsValid_F32_ref(const xi_pTile3D p);
_XI_APIREF_ xi_bool xiTile3DsHaveSameSize_ref(xi_pTile3D t1, xi_pTile3D t2);
_XI_APIREF_ xi_bool xiTile3DIsValid_I8_ref(const xi_pTile3D p);
_XI_APIREF_ xi_bool xiTile3DIsValid_I16_ref(const xi_pTile3D p);
_XI_APIREF_ xi_bool xiTile3DIsValid_I32_ref(const xi_pTile3D p);
_XI_APIREF_ xi_bool xiTile3DIsValid_IX_ref(const xi_pTile3D p);
_XI_APIREF_ xi_bool xiTile3DIsValid_IIX_ref(const xi_pTile3D p);

_XI_APIREF_ xi_bool xiTile4DCheckType_ref(const xi_pTile4D p, uint16_t type);
_XI_APIREF_ xi_bool xiTile4DIsValid_S8_ref(const xi_pTile4D p);
_XI_APIREF_ xi_bool xiTile4DIsValid_U8_ref(const xi_pTile4D p);
_XI_APIREF_ xi_bool xiTile4DIsValid_S16_ref(const xi_pTile4D p);
_XI_APIREF_ xi_bool xiTile4DIsValid_U16_ref(const xi_pTile4D p);
_XI_APIREF_ xi_bool xiTile4DIsValid_U32_ref(const xi_pTile4D p);
_XI_APIREF_ xi_bool xiTile4DIsValid_F16_ref(const xi_pTile4D p);
_XI_APIREF_ xi_bool xiTile4DIsValid_F32_ref(const xi_pTile4D p);
_XI_APIREF_ xi_bool xiTile4DIsValid_I8_ref(const xi_pTile4D p);
_XI_APIREF_ xi_bool xiTile4DIsValid_IX_ref(const xi_pTile4D p);
_XI_APIREF_ xi_bool xiTile4DsHaveSameSize_ref(xi_pTile4D t1, xi_pTile4D t2);

_XI_APIREF_ xi_bool xiTile3DCheckType_ref(const xi_pTile3D p, uint16_t type);
_XI_APIREF_ xi_bool xiTile4DCheckType_ref(const xi_pTile4D p, uint16_t type);


/* setting tile to constant value */
_XI_APIREF_ XI_ERR_TYPE xiFillTile_U8_ref (xi_pArray dst, unsigned value, xi_bool fill_edge_extension);
_XI_APIREF_ XI_ERR_TYPE xiFillTile_S16_ref(xi_pArray dst, int value, xi_bool fill_edge_extension);
_XI_APIREF_ XI_ERR_TYPE xiFillTile_I16_ref(xi_pArray dst, int value, xi_bool fill_edge_extension);


/* copying one tile to another  */
_XI_APIREF_ XI_ERR_TYPE xiCopyTile_ref(const xi_pArray src, xi_pArray dst, int bpp, xi_bool copy_edge_extension);
#define xiCopyTile_U8_ref(s,d,e)  xiCopyTile_ref((xi_pArray)(s), (xi_pArray)(d), sizeof(uint8_t), e)
#define xiCopyTile_S16_ref(s,d,e) xiCopyTile_ref((xi_pArray)(s), (xi_pArray)(d), sizeof(int16_t), e)
#define xiCopyTile_S32_ref(s,d,e) xiCopyTile_ref((xi_pArray)(s), (xi_pArray)(d), sizeof(int32_t), e)


_XI_APIREF_ XI_ERR_TYPE xiDepthwiseMultiplierConvolvedAVQ3D_S8_DWH_ref(const xi_pTile3D inTile,
                                                                       const xi_pTile3D coeffTile,
                                                                       const xi_pArray biasArray,
                                                                       const xi_pArray scaleArray,
                                                                       const xi_pArray shiftArray,
                                                                       xi_pTile3D outTile,
                                                                       const xi_cnna_depthwiseDilatedConv_params *param);

/* exchange tile data  */
_XI_APIREF_ XI_ERR_TYPE xiSwapTiles_ref(xi_pArray t1, xi_pArray t2, int element_size, xi_bool with_edge_extension);


/* GetRectSubPix */
_XI_APIREF_ XI_ERR_TYPE xiGetRectSubPix_U8_Q16_15_ref     (const xi_pTile src, xi_pArray dst, XI_Q16_15 x, XI_Q16_15 y);
_XI_APIREF_ XI_ERR_TYPE xiGetRectSubPix_S16_Q16_15_ref    (const xi_pTile src, xi_pArray dst, XI_Q16_15 x, XI_Q16_15 y);

_XI_APIREF_ XI_ERR_TYPE xiGetRectSubPix_U8Oa2_Q16_15_ref (const xi_pTile src, xi_pArray dst, XI_Q16_15 x, XI_Q16_15 y);
_XI_APIREF_ XI_ERR_TYPE xiGetRectSubPix_S16Oa_Q16_15_ref(const xi_pTile src, xi_pArray dst, XI_Q16_15 x, XI_Q16_15 y);
/* magnitude */
_XI_APIREF_ XI_ERR_TYPE xiMagnitude_U8U16_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMagnitude_S_S16_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMagnitude_U8U16A_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMagnitude_S_S16A_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
/* square root*/
_XI_APIREF_ XI_ERR_TYPE xiSquareRoot_U32U16_ref(const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiSquareRoot_U16_ref(const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiSquareRoot_U32U16A_ref(const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiSquareRoot_U16A_ref(const xi_pArray src, xi_pArray dst);
/* log */
_XI_APIREF_ XI_ERR_TYPE xiLog_S16_Q3_12_ref(const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiLog_S16_Q3_12_2_ref(const xi_pArray src, xi_pArray dst, const xi_pArray lut);
_XI_APIREF_ XI_ERR_TYPE xiLog_S16A_Q3_12_2_ref(const xi_pArray src, xi_pArray dst, const xi_pArray lut);


/* check range */
_XI_APIREF_ XI_ERR_TYPE xiCheckRange_U8_ref (const xi_pArray src, xi_bool* result, xi_point* pos, int min_value, int max_value);
_XI_APIREF_ XI_ERR_TYPE xiCheckRange_U8A2_ref (const xi_pArray src, xi_bool* result, xi_point* pos, int min_value, int max_value);
_XI_APIREF_ XI_ERR_TYPE xiCheckRange_S16_ref(const xi_pArray src, xi_bool* result, xi_point* pos, int min_value, int max_value);
_XI_APIREF_ XI_ERR_TYPE xiCheckRange_S16A_ref(const xi_pArray src, xi_bool* result, xi_point* pos, int min_value, int max_value);

/* finding min and max value and their locations */
_XI_APIREF_ XI_ERR_TYPE xiMinMaxLoc_U8_ref (const xi_pArray src, unsigned* min_value, unsigned* max_value, xi_point* min_loc, xi_point* max_loc);
_XI_APIREF_ XI_ERR_TYPE xiMinMaxLoc_S16_ref(const xi_pArray src, int* min_value, int* max_value, xi_point* min_loc, xi_point* max_loc);
_XI_APIREF_ XI_ERR_TYPE xiMinLoc_U8A2_ref (const xi_pArray src, unsigned* min_value, xi_point* min_loc);
_XI_APIREF_ XI_ERR_TYPE xiMinLoc_S16A_ref(const xi_pArray src, int* min_value, xi_point* min_loc);
_XI_APIREF_ XI_ERR_TYPE xiMinLoc_U8_ref (const xi_pArray src, unsigned* min_value, xi_point* min_loc);
_XI_APIREF_ XI_ERR_TYPE xiMinLoc_S16_ref(const xi_pArray src, int* min_value, xi_point* min_loc);
_XI_APIREF_ XI_ERR_TYPE xiMinMaxLoc_U8A2_ref (const xi_pArray src, unsigned* min_value, unsigned* max_value, xi_point* min_loc, xi_point* max_loc);
_XI_APIREF_ XI_ERR_TYPE xiMinMaxLoc_S16A_ref(const xi_pArray src, int* min_value, int* max_value, xi_point* min_loc, xi_point* max_loc);
_XI_APIREF_ XI_ERR_TYPE xiMaxLoc_U8A2_ref (const xi_pArray src, unsigned* max_value, xi_point* max_loc);
_XI_APIREF_ XI_ERR_TYPE xiMaxLoc_S16A_ref(const xi_pArray src, int* max_value, xi_point* max_loc);
_XI_APIREF_ XI_ERR_TYPE xiMaxLoc_U8_ref (const xi_pArray src, unsigned* max_value, xi_point* max_loc);
_XI_APIREF_ XI_ERR_TYPE xiMaxLoc_S16_ref(const xi_pArray src, int* max_value, xi_point* max_loc);
/* find min and max value */
_XI_APIREF_ XI_ERR_TYPE xiRMinMax_U8_ref (const xi_pArray src, unsigned* minval, unsigned* maxval);
_XI_APIREF_ XI_ERR_TYPE xiRMinMax_S16_ref(const xi_pArray src, int* minval, int* maxval);
_XI_APIREF_ XI_ERR_TYPE xiRMinMax_U8A2_ref (const xi_pArray src, unsigned* minval, unsigned* maxval);
_XI_APIREF_ XI_ERR_TYPE xiRMinMax_S16A_ref(const xi_pArray src, int* minval, int* maxval);
_XI_APIREF_ XI_ERR_TYPE xiRMinMax2_S16A_ref(const xi_pArray src, int* minval0, int* minval1, int* maxval0, int* maxval1);
_XI_APIREF_ XI_ERR_TYPE xiRMinMax2_S16_ref(const xi_pArray src, int* minval0, int* minval1, int* maxval0, int* maxval1);

/* gather locations of the specific value */
_XI_APIREF_ XI_ERR_TYPE xiGatherLocationsEQ_U8_ref     (const xi_pTile src, xi_point32* locations, int max_locations, int* count, int value);
_XI_APIREF_ XI_ERR_TYPE xiGatherLocationsEQ_S16_ref    (const xi_pTile src, xi_point32* locations, int max_locations, int* count, int value);
_XI_APIREF_ XI_ERR_TYPE xiGatherNextLocationsEQ_U8_ref (const xi_pTile src, xi_point32 start_loc, xi_point32* locations, int max_locations, int* count, int value);
_XI_APIREF_ XI_ERR_TYPE xiGatherNextLocationsEQ_S16_ref(const xi_pTile src, xi_point32 start_loc, xi_point32* locations, int max_locations, int* count, int value);
_XI_APIREF_ XI_ERR_TYPE xiGatherNextLocationsEQ_I8_ref (const xi_pTile src, xi_point32 start_loc, xi_point32* locations, int max_locations, int* count, int value);
_XI_APIREF_ XI_ERR_TYPE xiGatherNextLocationsEQ_I16_ref(const xi_pTile src, xi_point32 start_loc, xi_point32* locations, int max_locations, int* count, int value);
_XI_APIREF_ XI_ERR_TYPE xiGatherLocationsEQ_I8_ref     (const xi_pTile src, xi_point32* locations, int max_locations, int* count, int value);
_XI_APIREF_ XI_ERR_TYPE xiGatherLocationsEQ_I16_ref    (const xi_pTile src, xi_point32* locations, int max_locations, int* count, int value);

/* For each line of query tile searches nearest neighbour(s) among lines of train */
_XI_APIREF_ XI_ERR_TYPE xiKnnSearch1_L1_U8_ref      (const xi_pArray query, const xi_pArray train, xi_pArray matches, xi_pArray distance);
_XI_APIREF_ XI_ERR_TYPE xiKnnSearch1_L2S_U8_ref     (const xi_pArray query, const xi_pArray train, xi_pArray matches, xi_pArray distance);
_XI_APIREF_ XI_ERR_TYPE xiKnnSearch1_Hamming_U8_ref (const xi_pArray query, const xi_pArray train, xi_pArray matches, xi_pArray distance);
_XI_APIREF_ XI_ERR_TYPE xiKnnSearch1_L1_S16_ref     (const xi_pArray query, const xi_pArray train, xi_pArray matches, xi_pArray distance);
_XI_APIREF_ XI_ERR_TYPE xiKnnSearch1_L2S_S16_ref    (const xi_pArray query, const xi_pArray train, xi_pArray matches, xi_pArray distance);

_XI_APIREF_ XI_ERR_TYPE xiKnnSearch2_L1_U8_ref      (const xi_pArray query, const xi_pArray train, xi_pArray matches, xi_pArray distance);
_XI_APIREF_ XI_ERR_TYPE xiKnnSearch2_L2S_U8_ref     (const xi_pArray query, const xi_pArray train, xi_pArray matches, xi_pArray distance);
_XI_APIREF_ XI_ERR_TYPE xiKnnSearch2_Hamming_U8_ref (const xi_pArray query, const xi_pArray train, xi_pArray matches, xi_pArray distance);
_XI_APIREF_ XI_ERR_TYPE xiKnnSearch2_L1_S16_ref     (const xi_pArray query, const xi_pArray train, xi_pArray matches, xi_pArray distance);
_XI_APIREF_ XI_ERR_TYPE xiKnnSearch2_L2S_S16_ref    (const xi_pArray query, const xi_pArray train, xi_pArray matches, xi_pArray distance);


/* mean and standard deviation */
_XI_APIREF_ XI_ERR_TYPE xiMeanStdDev_U8_ref (const xi_pArray src, XI_Q8_7   * mean, float    * stddev);
_XI_APIREF_ XI_ERR_TYPE xiMeanStdDev_S16_ref(const xi_pArray src, XI_Q15_16 * mean, float    * stddev);
_XI_APIREF_ XI_ERR_TYPE xiSumSquares_U8_ref (const xi_pArray src, uint32_t  * sum,  uint32_t * sqsum );
_XI_APIREF_ XI_ERR_TYPE xiSumSquares_S16_ref(const xi_pArray src, int64_t   * sum,  uint64_t * sqsum );


/* threshold */
_XI_APIREF_ XI_ERR_TYPE xiThreshold2_U8_ref          (const xi_pArray src, xi_pArray dst, int threshold, int minval, int maxval);
_XI_APIREF_ XI_ERR_TYPE xiThreshold2_S16_ref         (const xi_pArray src, xi_pArray dst, int threshold, int minval, int maxval);
_XI_APIREF_ XI_ERR_TYPE xiThreshold2_S16U8_ref       (const xi_pArray src, xi_pArray dst, int threshold, int minval, int maxval);

_XI_APIREF_ XI_ERR_TYPE xiThresholdTrunc_U8_ref     (const xi_pArray src, xi_pArray dst, int threshold, int maxval);
_XI_APIREF_ XI_ERR_TYPE xiThresholdTrunc_S16_ref    (const xi_pArray src, xi_pArray dst, int threshold, int maxval);
_XI_APIREF_ XI_ERR_TYPE xiThresholdToZero_U8_ref    (const xi_pArray src, xi_pArray dst, int threshold, int maxval);
_XI_APIREF_ XI_ERR_TYPE xiThresholdToZero_S16_ref   (const xi_pArray src, xi_pArray dst, int threshold, int maxval);
_XI_APIREF_ XI_ERR_TYPE xiThresholdToZeroInv_U8_ref (const xi_pArray src, xi_pArray dst, int threshold, int maxval);
_XI_APIREF_ XI_ERR_TYPE xiThresholdToZeroInv_S16_ref(const xi_pArray src, xi_pArray dst, int threshold, int maxval);

#define xiThreshold_U8_ref(src, dst, threshold, maxval)          xiThreshold2_U8_ref(src, dst, threshold, 0, maxval)
#define xiThreshold_S16_ref(src, dst, threshold, maxval)         xiThreshold2_S16_ref(src, dst, threshold, 0, maxval)
#define xiThresholdInv_U8_ref(src, dst, threshold, maxval)       xiThreshold2_U8_ref(src, dst, threshold, maxval, 0)
#define xiThresholdInv_S16_ref(src, dst, threshold, maxval)      xiThreshold2_S16_ref(src, dst, threshold, maxval, 0)
#define xiThreshold_S16U8_ref(src, dst, threshold, maxval)       xiThreshold2_S16U8_ref(src, dst, threshold, 0, maxval)
#define xiThresholdInv_S16U8_ref(src, dst, threshold, maxval)    xiThreshold2_S16U8_ref(src, dst, threshold, maxval, 0)


/* bi-linear image resize */
_XI_APIREF_ XI_ERR_TYPE xiResizeBilinear_U8_Q13_18_ref (const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift, XI_Q13_18 yshift);
_XI_APIREF_ XI_ERR_TYPE xiResizeBilinear_S16_Q13_18_ref(const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift, XI_Q13_18 yshift);
_XI_APIREF_ XI_ERR_TYPE xiResizeBilinear_S16Oa_Q13_18_ref(const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift, XI_Q13_18 yshift);
_XI_APIREF_ XI_ERR_TYPE xiResizeBilinear_U8Oa_Q13_18_ref(const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift, XI_Q13_18 yshift);
_XI_APIREF_ XI_ERR_TYPE xiResizeBilinear2_U8Oa2_Q13_18_ref(const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift, XI_Q13_18 yshift);


/* bi-cubic image resize */
_XI_APIREF_ XI_ERR_TYPE xiResizeBicubic_U8_Q13_18_ref (const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift, XI_Q13_18 yshift);
_XI_APIREF_ XI_ERR_TYPE xiResizeBicubic_S16_Q13_18_ref(const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift, XI_Q13_18 yshift);
_XI_APIREF_ XI_ERR_TYPE xiResizeBicubic2_U8_Q13_18_ref (const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift, XI_Q13_18 yshift);
_XI_APIREF_ XI_ERR_TYPE xiResizeBicubic2_S16_Q13_18_ref(const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift, XI_Q13_18 yshift);
_XI_APIREF_ XI_ERR_TYPE xiResizeBicubic2_U8Oa_Q13_18_ref (const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift, XI_Q13_18 yshift);
_XI_APIREF_ XI_ERR_TYPE xiResizeBicubic2_S16Oa_Q13_18_ref(const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift, XI_Q13_18 yshift);


/* nearest-neighbor image resize */
_XI_APIREF_ XI_ERR_TYPE xiResizeNearest_U8_Q13_18_ref (const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift, XI_Q13_18 yshift);
_XI_APIREF_ XI_ERR_TYPE xiResizeNearest_S16_Q13_18_ref(const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift, XI_Q13_18 yshift);
_XI_APIREF_ XI_ERR_TYPE xiResizeNearest_I8_Q13_18_ref (const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift, XI_Q13_18 yshift);
_XI_APIREF_ XI_ERR_TYPE xiResizeNearest_I8Oa2_Q13_18_ref (const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift, XI_Q13_18 yshift);
_XI_APIREF_ XI_ERR_TYPE xiResizeNearest_S16Oa_Q13_18_ref(const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift, XI_Q13_18 yshift);


/* area image resize */
_XI_APIREF_ XI_ERR_TYPE xiResizeArea_U8_Q13_18_ref (const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift, XI_Q13_18 yshift);
_XI_APIREF_ XI_ERR_TYPE xiResizeArea_S16_Q13_18_ref(const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift, XI_Q13_18 yshift);
_XI_APIREF_ XI_ERR_TYPE xiResizeArea_S16_Q13_18_gthr_ref(const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift, XI_Q13_18 yshift);
_XI_APIREF_ XI_ERR_TYPE xiResizeArea_S16_Q13_18_int_ref(const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift, XI_Q13_18 yshift);
_XI_APIREF_ XI_ERR_TYPE xiResizeArea_S16_Q13_18_default_ref(const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift, XI_Q13_18 yshift);
_XI_APIREF_ XI_ERR_TYPE xiResizeArea_S16Oa_Q13_18_ref(const xi_pTile src, xi_pTile dst, XI_Q13_18 xscale, XI_Q13_18 yscale, XI_Q13_18 xshift, XI_Q13_18 yshift);
#define xiResizeArea_U8Oa_Q13_18_ref(src, dst, xscale, yscale, xshift, yshift) xiResizeArea_U8_Q13_18_ref(src, dst, xscale, yscale, xshift, yshift)


/* Affine transform */
_XI_APIREF_ XI_ERR_TYPE xiWarpAffine_U8_Q13_18_ref (const xi_pTile src, xi_pTile dst, const xi_affine* affine);
_XI_APIREF_ XI_ERR_TYPE xiWarpAffine_U8A_Q13_18_ref (const xi_pTile src, xi_pTile dst, const xi_affine* affine);
_XI_APIREF_ XI_ERR_TYPE xiWarpAffine_S16_Q13_18_ref(const xi_pTile src, xi_pTile dst, const xi_affine* affine);
_XI_APIREF_ XI_ERR_TYPE xiWarpAffine_S16A_Q13_18_ref(const xi_pTile src, xi_pTile dst, const xi_affine* affine);

_XI_APIREF_ XI_ERR_TYPE xiWarpAffineBilinear_U8_Q13_18_ref (const xi_pTile src, xi_pTile dst, const xi_affine_fpt* affine);
_XI_APIREF_ XI_ERR_TYPE xiWarpAffineBilinear_S16_Q13_18_ref(const xi_pTile src, xi_pTile dst, const xi_affine_fpt* affine);

_XI_APIREF_ XI_ERR_TYPE xiWarpAffineNearest_U8_Q13_18_ref (const xi_pTile src, xi_pTile dst, const xi_affine_fpt* affine);
_XI_APIREF_ XI_ERR_TYPE xiWarpAffineNearest_S16_Q13_18_ref(const xi_pTile src, xi_pTile dst, const xi_affine_fpt* affine);
_XI_APIREF_ XI_ERR_TYPE xiWarpAffineNearest_I8_Q13_18_ref (const xi_pTile src, xi_pTile dst, const xi_affine_fpt* affine);
_XI_APIREF_ XI_ERR_TYPE xiWarpAffineNearest_I16_Q13_18_ref(const xi_pTile src, xi_pTile dst, const xi_affine_fpt* affine);


/* Canny */
_XI_APIREF_ XI_ERR_TYPE xiCanny_3x3_U8_L1_ref(const xi_pTile src, xi_pArray dst, XI_Q28_3 low_threshold, XI_Q28_3 high_threshold, xi_bool trace_edges);
_XI_APIREF_ XI_ERR_TYPE xiCanny_3x3_U8_L2_ref(const xi_pTile src, xi_pArray dst, unsigned low_threshold, unsigned high_threshold, xi_bool trace_edges);

_XI_APIREF_ XI_ERR_TYPE xiCanny_3x3_S16U8_L1_ref(const xi_pTile src, xi_pArray dst, unsigned low_threshold, unsigned high_threshold, xi_bool trace_edges);
_XI_APIREF_ XI_ERR_TYPE xiCanny_3x3_S16U8_L2_ref(const xi_pTile src, xi_pArray dst, unsigned low_threshold, unsigned high_threshold, xi_bool trace_edges);

_XI_APIREF_ XI_ERR_TYPE xiCanny_5x5_U8_L1_ref(const xi_pTile src, xi_pArray dst, unsigned low_threshold, unsigned high_threshold, xi_bool trace_edges);
_XI_APIREF_ XI_ERR_TYPE xiCanny_5x5_U8_L2_ref(const xi_pTile src, xi_pArray dst, unsigned low_threshold, unsigned high_threshold, xi_bool trace_edges);

_XI_APIREF_ XI_ERR_TYPE xiCanny_5x5_S16U8_L1_ref(const xi_pTile src, xi_pArray dst, unsigned low_threshold, unsigned high_threshold, xi_bool trace_edges);
_XI_APIREF_ XI_ERR_TYPE xiCanny_5x5_S16U8_L2_ref(const xi_pTile src, xi_pArray dst, unsigned low_threshold, unsigned high_threshold, xi_bool trace_edges);

_XI_APIREF_ XI_ERR_TYPE xiCanny_7x7_U8_L1_ref(const xi_pTile src, xi_pArray dst, unsigned low_threshold, unsigned high_threshold, xi_bool trace_edges);
_XI_APIREF_ XI_ERR_TYPE xiCanny_7x7_U8_L2_ref(const xi_pTile src, xi_pArray dst, unsigned low_threshold, unsigned high_threshold, xi_bool trace_edges);

_XI_APIREF_ XI_ERR_TYPE xiCanny_7x7_S16U8_L1_ref(const xi_pTile src, xi_pArray dst, unsigned low_threshold, unsigned high_threshold, xi_bool trace_edges);
_XI_APIREF_ XI_ERR_TYPE xiCanny_7x7_S16U8_L2_ref(const xi_pTile src, xi_pArray dst, unsigned low_threshold, unsigned high_threshold, xi_bool trace_edges);


/* Tile rotations */
_XI_APIREF_ XI_ERR_TYPE xiFlipHorizontal_U8_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiFlipHorizontal_S16_ref(const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiFlipHorizontal_I8_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiFlipHorizontal_I8A_ref(const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiFlipHorizontal_I8A2_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiFlipHorizontal_I8Oa2_ref(const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiFlipHorizontal_I16_ref(const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiFlipHorizontal_I16A_ref(const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiFlipHorizontal_I16Oa_ref(const xi_pArray src, xi_pArray dst);

_XI_APIREF_ XI_ERR_TYPE xiFlipVertical_U8_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiFlipVertical_S16_ref(const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiFlipVertical_I8_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiFlipVertical_I16_ref(const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiFlipVertical_I8A2_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiFlipVertical_I16A_ref(const xi_pArray src, xi_pArray dst);

_XI_APIREF_ XI_ERR_TYPE xiTranspose_U8_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiTranspose_S16_ref(const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiTranspose_I8_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiTranspose_I16_ref(const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiTranspose_I8A_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiTranspose_I16A_ref(const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiTranspose_I8MSOa_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiTranspose_I16MSOa_ref(const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiTranspose_I8MSIa_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiTranspose_I16MSIa_ref(const xi_pArray src, xi_pArray dst);

_XI_APIREF_ XI_ERR_TYPE xiRotateClockwise_U8_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiRotateClockwise_S16_ref(const xi_pArray src, xi_pArray dst);

_XI_APIREF_ XI_ERR_TYPE xiRotateClockwise_I8_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiRotateClockwise_I16_ref(const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiRotateClockwise_I8A_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiRotateClockwise_I16A_ref(const xi_pArray src, xi_pArray dst);
/* Extend Edges */
_XI_APIREF_ XI_ERR_TYPE xiExtendEdgesConst_U8_ref (xi_pTile tile, int frame_width, int frame_height, int value);
_XI_APIREF_ XI_ERR_TYPE xiExtendEdgesConst_S16_ref(xi_pTile tile, int frame_width, int frame_height, int value);
_XI_APIREF_ XI_ERR_TYPE xiExtendEdgesConst_I8_ref (xi_pTile tile, int frame_width, int frame_height, int value);
_XI_APIREF_ XI_ERR_TYPE xiExtendEdgesConst_I16_ref(xi_pTile tile, int frame_width, int frame_height, int value);
_XI_APIREF_ XI_ERR_TYPE xiExtendEdgesReplicate_U8_ref (xi_pTile tile, int frame_width, int frame_height);
_XI_APIREF_ XI_ERR_TYPE xiExtendEdgesReplicate_S16_ref(xi_pTile tile, int frame_width, int frame_height);
_XI_APIREF_ XI_ERR_TYPE xiExtendEdgesReplicate_I8_ref (xi_pTile tile, int frame_width, int frame_height);
_XI_APIREF_ XI_ERR_TYPE xiExtendEdgesReplicate_I16_ref(xi_pTile tile, int frame_width, int frame_height);
_XI_APIREF_ XI_ERR_TYPE xiExtendEdgesReflect101_U8_ref (xi_pTile tile, int frame_width, int frame_height);
_XI_APIREF_ XI_ERR_TYPE xiExtendEdgesReflect101_S16_ref(xi_pTile tile, int frame_width, int frame_height);
_XI_APIREF_ XI_ERR_TYPE xiExtendEdgesReflect101_I8_ref (xi_pTile tile, int frame_width, int frame_height);
_XI_APIREF_ XI_ERR_TYPE xiExtendEdgesReflect101_I16_ref(xi_pTile tile, int frame_width, int frame_height);

/* Filter2D functions*/
_XI_APIREF_ XI_ERR_TYPE xiFilter2D_NxN_U8S16_Q5_10_R_ref  (const xi_pTile src, xi_pArray dst, const XI_Q15* kernel, int ksize, int rsht, int bias);
_XI_APIREF_ XI_ERR_TYPE xiFilter2D_NxN_U8S16_Q5_10_Div_ref(const xi_pTile src, xi_pArray dst, const XI_Q15* kernel, int ksize, int divisor, int bias);

_XI_APIREF_ XI_ERR_TYPE xiFilter2D_U8_ref (const xi_pTile src, xi_pArray dst, const XI_Q15* kernel, int ksize, int divisor, int bias);
_XI_APIREF_ XI_ERR_TYPE xiFilter2D_S16_ref(const xi_pTile src, xi_pArray dst, const XI_Q15* kernel, int ksize, int divisor, int bias);

_XI_APIREF_ XI_ERR_TYPE xiFilter2D_R_S16_ref(const xi_pTile src, xi_pArray dst, const XI_Q15* kernel, int ksize, int rshift, int bias);
_XI_APIREF_ XI_ERR_TYPE xiFilter2D_R_U8_ref (const xi_pTile src, xi_pArray dst, const XI_Q15* kernel, int ksize, int lshift, int norm, int bias);
_XI_APIREF_ XI_ERR_TYPE xiFilter2D_D_S16_ref(const xi_pTile src, xi_pArray dst, const XI_Q15* kernel, int ksize, int divisor, int bias);
_XI_APIREF_ XI_ERR_TYPE xiFilter2D_D_U8_ref (const xi_pTile src, xi_pArray dst, const XI_Q15* kernel, int ksize, int lshift, int divisor, int bias);
_XI_APIREF_ XI_ERR_TYPE xiFilter2D_R_S16Oa_ref(const xi_pTile src, xi_pArray dst, const XI_Q15* kernel, int ksize, int divisor, int bias);
#ifdef XI_ENABLE_BIT_EXACT_CREF
_XI_APIREF_ XI_ERR_TYPE xiFilter2D_D_S16Oa_ref(const xi_pTile src, xi_pArray dst, const XI_Q15* kernel, int ksize, int divisor, int bias);
_XI_APIREF_ XI_ERR_TYPE xiFilter2D_D_S16_3x3_ref(const xi_pTile src, xi_pArray dst, const XI_Q15* kernel, int ksize, int divisor, int bias);
_XI_APIREF_ XI_ERR_TYPE xiFilter2D_D_S16Oa_3x3_ref(const xi_pTile src, xi_pArray dst, const XI_Q15* kernel, int ksize, int divisor, int bias);
_XI_APIREF_ XI_ERR_TYPE xiFilter2D_R_S16_NxN_ref(const xi_pTile src, xi_pArray dst, const XI_Q15* kernel, int ksize, int divisor, int bias);
_XI_APIREF_ XI_ERR_TYPE xiFilter2D_R_S16Oa_NxN_ref(const xi_pTile src, xi_pArray dst, const XI_Q15* kernel, int ksize, int divisor, int bias);
_XI_APIREF_ XI_ERR_TYPE xiFilter2D_D_U8_NxN_ref (const xi_pTile src, xi_pArray dst, const XI_Q15* kernel, int ksize, int divisor, int bias);
_XI_APIREF_ XI_ERR_TYPE xiFilter2D_S16_laplacian_ref(const xi_pTile src, xi_pArray dst, const XI_Q15* kernel, int ksize, int divisor, int bias);
_XI_APIREF_ XI_ERR_TYPE xiFilter2D_S16_laplacian2_ref(const xi_pTile src, xi_pArray dst, const XI_Q15* kernel, int ksize, int divisor, int bias);
_XI_APIREF_ XI_ERR_TYPE xiFilter2D_S16_laplacian3_ref(const xi_pTile src, xi_pArray dst, const XI_Q15* kernel, int ksize, int divisor, int bias);
#endif

/* OpenVX-style custom convolution */
_XI_APIREF_ XI_ERR_TYPE xiConvolve_RS_U8_ref(const xi_pTile src, xi_pArray dst, const int16_t* kernel, int kwidth, int kheight, int shift);
_XI_APIREF_ XI_ERR_TYPE xiConvolve_RS_S16_ref(const xi_pTile src, xi_pArray dst, const int16_t* kernel, int kwidth, int kheight, int shift);
_XI_APIREF_ XI_ERR_TYPE xiConvolve_RS_U8S16_ref(const xi_pTile src, xi_pArray dst, const int16_t* kernel, int kwidth, int kheight, int shift);
_XI_APIREF_ XI_ERR_TYPE xiConvolve_RS_S16S32_ref(const xi_pTile src, xi_pArray dst, const int16_t* kernel, int kwidth, int kheight, int shift);


/* Separable Filter2D functions*/
_XI_APIREF_ XI_ERR_TYPE xiSepFilter2D_U8_ref (const xi_pTile src, xi_pArray dst, const XI_Q15* kernelX, const XI_Q15* kernelY, int ksize, int divisor, int bias);
_XI_APIREF_ XI_ERR_TYPE xiSepFilter2D_R_U8_ref(const xi_pTile src, xi_pArray dst, const XI_Q15* kernelX, const XI_Q15* kernelY, int ksize, int divisor, int bias);
_XI_APIREF_ XI_ERR_TYPE xiSepFilter2D_D_U8_ref(const xi_pTile src, xi_pArray dst, const XI_Q15* kernelX, const XI_Q15* kernelY, int ksize, int divisor, int bias);
_XI_APIREF_ XI_ERR_TYPE xiSepFilter2D_S16_ref(const xi_pTile src, xi_pArray dst, const XI_Q15* kernelX, const XI_Q15* kernelY, int ksize, int divisor, int bias);
_XI_APIREF_ XI_ERR_TYPE xiSepFilter2D_R_S16_ref(const xi_pTile src, xi_pArray dst, const XI_Q15* kernelX, const XI_Q15* kernelY, int ksize, int divisor, int bias);
_XI_APIREF_ XI_ERR_TYPE xiSepFilter2D_D_S16_ref(const xi_pTile src, xi_pArray dst, const XI_Q15* kernelX, const XI_Q15* kernelY, int ksize, int divisor, int bias);


#ifdef XI_ENABLE_BIT_EXACT_CREF
_XI_APIREF_ XI_ERR_TYPE xiScharr_S16_dx_ref(const xi_pTile src, xi_pArray dst, const XI_Q15* kernelX, const XI_Q15* kernelY, int ksize, int divisor, int bias);
_XI_APIREF_ XI_ERR_TYPE xiScharr_S16_dy_ref(const xi_pTile src, xi_pArray dst, const XI_Q15* kernelX, const XI_Q15* kernelY, int ksize, int divisor, int bias);
_XI_APIREF_ XI_ERR_TYPE xiSepFilter2D_Sobel_X_S16_ref(const xi_pTile src, xi_pArray dst, const XI_Q15* kernelX, const XI_Q15* kernelY, int ksize, int divisor, int bias);
_XI_APIREF_ XI_ERR_TYPE xiSepFilter2D_Sobel_Y_S16_ref(const xi_pTile src, xi_pArray dst, const XI_Q15* kernelX, const XI_Q15* kernelY, int ksize, int divisor, int bias);
_XI_APIREF_ XI_ERR_TYPE xiSepFilter2D_GaussianBlur_3x3_S16_ref(const xi_pTile src, xi_pArray dst, const XI_Q15* kernelX, const XI_Q15* kernelY, int ksize, int divisor, int bias);
_XI_APIREF_ XI_ERR_TYPE xiSepFilter2D_GaussianBlur_5x5_S16_ref(const xi_pTile src, xi_pArray dst, const XI_Q15* kernelX, const XI_Q15* kernelY, int ksize, int divisor, int bias);
_XI_APIREF_ XI_ERR_TYPE xiSepFilter2D_GaussianBlur_5x5_S16Oa_ref(const xi_pTile src, xi_pArray dst, const XI_Q15* kernelX, const XI_Q15* kernelY, int ksize, int divisor, int bias);
#endif

/* Accumulate functions */
_XI_APIREF_ XI_ERR_TYPE xiAccumulate_U8S16_ref(const xi_pArray src, xi_pArray dst, const xi_pArray msk);
_XI_APIREF_ XI_ERR_TYPE xiAccumulate_S16_ref  (const xi_pArray src, xi_pArray dst, const xi_pArray msk);


/* AccumulateSquare functions */
_XI_APIREF_ XI_ERR_TYPE xiAccumulateSquare_U8S16_ref(const xi_pArray src, xi_pArray dst, const xi_pArray msk);
_XI_APIREF_ XI_ERR_TYPE xiAccumulateSquare_S16_ref  (const xi_pArray src, xi_pArray dst, const xi_pArray msk);


/* AccumulateSquareImage functions */
_XI_APIREF_ XI_ERR_TYPE xiAccumulateSquareImage_S_U8S16_ref (const xi_pArray src, xi_pArray dst, int shift);
_XI_APIREF_ XI_ERR_TYPE xiAccumulateSquareImage_S_S16S32_ref(const xi_pArray src, xi_pArray dst, int shift);
_XI_APIREF_ XI_ERR_TYPE xiAccumulateSquareImage_S_U8S16A_ref (const xi_pArray src, xi_pArray dst, int shift);
_XI_APIREF_ XI_ERR_TYPE xiAccumulateSquareImage_S_S16S32A_ref(const xi_pArray src, xi_pArray dst, int shift);
/* AccumulateWeighted functions */
_XI_APIREF_ XI_ERR_TYPE xiAccumulateWeighted_U8_ref (const xi_pArray src, xi_pArray dst, XI_Q15 alpha, const xi_pArray msk);
_XI_APIREF_ XI_ERR_TYPE xiAccumulateWeighted_S16_ref(const xi_pArray src, xi_pArray dst, XI_Q15 alpha, const xi_pArray msk);
_XI_APIREF_ XI_ERR_TYPE xiAccumulateWeighted_U8A2_ref (const xi_pArray src, xi_pArray dst, XI_Q15 alpha, const xi_pArray msk);
_XI_APIREF_ XI_ERR_TYPE xiAccumulateWeighted_S16A_ref(const xi_pArray src, xi_pArray dst, XI_Q15 alpha, const xi_pArray msk);


/* AccumulateWeightedImage functions */
_XI_APIREF_ XI_ERR_TYPE xiAccumulateWeightedImage_U8_ref (const xi_pArray src, xi_pArray dst, XI_Q15 alpha);
_XI_APIREF_ XI_ERR_TYPE xiAccumulateWeightedImage_S16_ref(const xi_pArray src, xi_pArray dst, XI_Q15 alpha);
_XI_APIREF_ XI_ERR_TYPE xiAccumulateWeightedImage_U8A2_ref(const xi_pArray src, xi_pArray dst, XI_Q15 alpha);
_XI_APIREF_ XI_ERR_TYPE xiAccumulateWeightedImage_S16A_ref(const xi_pArray src, xi_pArray dst, XI_Q15 alpha);


/* AccumulateWeightedImage functions */
_XI_APIREF_ XI_ERR_TYPE xiAccumulateWeightedScalar_U8_ref(xi_pArray dst, int val, XI_Q15 alpha);


/* AccumulateProduct functions */
_XI_APIREF_ XI_ERR_TYPE xiAccumulateProduct_U8S16_ref(const xi_pArray src1, const xi_pArray src2, xi_pArray dst, const xi_pArray msk);
_XI_APIREF_ XI_ERR_TYPE xiAccumulateProduct_S16_ref  (const xi_pArray src1, const xi_pArray src2, xi_pArray dst, const xi_pArray msk);


/* AccumulateImage functions */
_XI_APIREF_ XI_ERR_TYPE xiAccumulateImage_S_U8S16_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiAccumulateImage_S_S16S32_ref(const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiAccumulateScalar_S_S16_ref  (xi_pArray dst,  int val);


/* MeanShift functions */
_XI_APIREF_ XI_ERR_TYPE xiMeanShift_U8_ref (const xi_pTile src, xi_rect* window, int max_steps, int eps);
_XI_APIREF_ XI_ERR_TYPE xiMeanShift_S16_ref(const xi_pTile src, xi_rect* window, int max_steps, int eps);


/* CamShift functions */
_XI_APIREF_ XI_ERR_TYPE xiCamShift_U8_ref (const xi_pTile src, xi_rect* start_wnd, xi_rotated_rect* r_wnd, int max_steps, int eps2);
_XI_APIREF_ XI_ERR_TYPE xiCamShift_S16_ref(const xi_pTile src, xi_rect* start_wnd, xi_rotated_rect* r_wnd, int max_steps, int eps2);


/* Sum functions */
_XI_APIREF_ XI_ERR_TYPE xiSum_U8_ref (const xi_pArray src, unsigned* sum);
_XI_APIREF_ XI_ERR_TYPE xiSum_S16_ref(const xi_pArray src, int* sum);
_XI_APIREF_ XI_ERR_TYPE xiSum_U8A2_ref (const xi_pArray src, unsigned* sum);
_XI_APIREF_ XI_ERR_TYPE xiSum_S16A_ref(const xi_pArray src, int* sum);

/* Element counting functions */
_XI_APIREF_ XI_ERR_TYPE xiCountNonZero_U8_ref (const xi_pTile src, int* count);
_XI_APIREF_ XI_ERR_TYPE xiCountNonZero_I8_ref (const xi_pTile src, int* count);
_XI_APIREF_ XI_ERR_TYPE xiCountNonZero_I8A2_ref (const xi_pTile src, int* count);

_XI_APIREF_ XI_ERR_TYPE xiCountNonZero_S16_ref(const xi_pTile src, int* count);
_XI_APIREF_ XI_ERR_TYPE xiCountNonZero_I16_ref(const xi_pTile src, int* count);
_XI_APIREF_ XI_ERR_TYPE xiCountNonZero_I16A_ref(const xi_pTile src, int* count);

_XI_APIREF_ XI_ERR_TYPE xiCountEqual_U8_ref (const xi_pTile src, unsigned value, int * count);
_XI_APIREF_ XI_ERR_TYPE xiCountEqual_I8_ref (const xi_pTile src, unsigned value, int * count);
_XI_APIREF_ XI_ERR_TYPE xiCountEqual_I8A2_ref (const xi_pTile src, unsigned value, int * count);

_XI_APIREF_ XI_ERR_TYPE xiCountEqual_S16_ref(const xi_pTile src, int      value, int * count);
_XI_APIREF_ XI_ERR_TYPE xiCountEqual_I16_ref(const xi_pTile src, int      value, int * count);
_XI_APIREF_ XI_ERR_TYPE xiCountEqual_I16A_ref(const xi_pTile src, int      value, int * count);
_XI_APIREF_ XI_ERR_TYPE xiCountNonZero_I8_ref(const xi_pTile src, int * count);
_XI_APIREF_ XI_ERR_TYPE xiCountEqual_I8_ref(const xi_pTile src, unsigned int value, int * count);
_XI_APIREF_ XI_ERR_TYPE xiCountGreater_U8_ref (const xi_pTile src, unsigned value, int * count);
_XI_APIREF_ XI_ERR_TYPE xiCountGreater_U8A2_ref (const xi_pTile src, unsigned value, int * count);
_XI_APIREF_ XI_ERR_TYPE xiCountGreater_S16_ref(const xi_pTile src, int      value, int * count);
_XI_APIREF_ XI_ERR_TYPE xiCountGreater_S16A_ref(const xi_pTile src, int      value, int * count);
_XI_APIREF_ XI_ERR_TYPE xiCountGreater_U16_ref(const xi_pTile src, unsigned value, int * count);
_XI_APIREF_ XI_ERR_TYPE xiCountGreater_U16A_ref(const xi_pTile src, unsigned value, int * count);

/* L1 norm */
_XI_APIREF_ XI_ERR_TYPE xiNormL1_U8_ref (const xi_pTile src1, const xi_pTile src2, unsigned *norm);
_XI_APIREF_ XI_ERR_TYPE xiNormL1_S16_ref(const xi_pTile src1, const xi_pTile src2, unsigned *norm);
_XI_APIREF_ XI_ERR_TYPE xiNormL1_U8A2_ref (const xi_pTile src1, const xi_pTile src2, unsigned *norm);
_XI_APIREF_ XI_ERR_TYPE xiNormL1_S16A_ref(const xi_pTile src1, const xi_pTile src2, unsigned *norm);

/* L2 norm */
_XI_APIREF_ XI_ERR_TYPE xiNormL2_U8_ref (const xi_pTile src1, const xi_pTile src2, unsigned* norm);
_XI_APIREF_ XI_ERR_TYPE xiNormL2S_U8_ref(const xi_pTile src1, const xi_pTile src2, unsigned* norm);
_XI_APIREF_ XI_ERR_TYPE xiNormL2_S16_ref(const xi_pTile src1, const xi_pTile src2, unsigned* norm);
_XI_APIREF_ XI_ERR_TYPE xiNormL2_U8A2_ref (const xi_pTile src1, const xi_pTile src2, unsigned* norm);
_XI_APIREF_ XI_ERR_TYPE xiNormL2S_U8A2_ref(const xi_pTile src1, const xi_pTile src2, unsigned* norm);
_XI_APIREF_ XI_ERR_TYPE xiNormL2_S16A_ref(const xi_pTile src1, const xi_pTile src2, unsigned* norm);


/* infinite norm */
_XI_APIREF_ XI_ERR_TYPE xiNormInf_U8_ref (const xi_pTile src1, const xi_pTile src2, unsigned* norm);
_XI_APIREF_ XI_ERR_TYPE xiNormInf_S16_ref(const xi_pTile src1, const xi_pTile src2, unsigned* norm);
_XI_APIREF_ XI_ERR_TYPE xiNormInf_U8A2_ref (const xi_pTile src1, const xi_pTile src2, unsigned* norm);
_XI_APIREF_ XI_ERR_TYPE xiNormInf_S16A_ref(const xi_pTile src1, const xi_pTile src2, unsigned* norm);

/* Harring norm */
_XI_APIREF_ XI_ERR_TYPE xiNormHamming_U8_ref(const xi_pTile src1, const xi_pTile src2, unsigned* norm);
_XI_APIREF_ XI_ERR_TYPE xiNormHamming_U8A2_ref(const xi_pTile src1, const xi_pTile src2, unsigned* norm);

/* Spatial moments of raster image: M00, M01, M10, M11, M20, M02 */
_XI_APIREF_ XI_ERR_TYPE xiMoments_U8_ref (const xi_pTile src, xi_moments* moments);
_XI_APIREF_ XI_ERR_TYPE xiMoments_U8A_ref (const xi_pTile src, xi_moments* moments);
_XI_APIREF_ XI_ERR_TYPE xiMoments_S16_ref(const xi_pTile src, xi_moments* moments);


/* clip to specific range */
_XI_APIREF_ XI_ERR_TYPE xiClip_U8_ref (const xi_pArray src, xi_pArray dst, int low, int high);
_XI_APIREF_ XI_ERR_TYPE xiClip_S16_ref(const xi_pArray src, xi_pArray dst, int low, int high);


/* generate mask for specific range */
_XI_APIREF_ XI_ERR_TYPE xiInRange2_U8_ref    (const xi_pArray src, xi_pArray dst, int low, int high, int trueval, int falseval);
_XI_APIREF_ XI_ERR_TYPE xiInRange2_S16U8_ref (const xi_pArray src, xi_pArray dst, int low, int high, int trueval, int falseval);
_XI_APIREF_ XI_ERR_TYPE xiInRange2_S16U16_ref(const xi_pArray src, xi_pArray dst, int low, int high, int trueval, int falseval);
_XI_APIREF_ XI_ERR_TYPE xiInRange2_U8A2_ref    (const xi_pArray src, xi_pArray dst, int low, int high, int trueval, int falseval);
_XI_APIREF_ XI_ERR_TYPE xiInRange2_S16U8A2_ref (const xi_pArray src, xi_pArray dst, int low, int high, int trueval, int falseval);
_XI_APIREF_ XI_ERR_TYPE xiInRange2_S16U16A_ref(const xi_pArray src, xi_pArray dst, int low, int high, int trueval, int falseval);
#define xiInRange_U8_ref(src, dst, low, high)  xiInRange2_U8_ref(src, dst, low, high, 255 ,0)
#define xiInRange_S16U8_ref(src, dst, low, high)  xiInRange2_S16U8_ref(src, dst, low, high, 255 ,0)
#define xiInRange_S16U16_ref(src, dst, low, high) xiInRange2_S16U16_ref(src, dst, low, high, 65535 ,0)
#define xiInRange_U8A2_ref(src, dst, low, high)   xiInRange2_U8_ref(src, dst, low, high, 255 ,0)
#define xiInRange_S16U8A2_ref(src, dst, low, high) xiInRange2_S16U8_ref(src, dst, low, high, 255 ,0)
#define xiInRange_S16U16A_ref(src, dst, low, high) xiInRange2_S16U16_ref(src, dst, low, high, 65535 ,0)
/* data depth conversion */
_XI_APIREF_ XI_ERR_TYPE xiConvertBitDepth_U8S16_ref  (const xi_pArray src, xi_pArray dst, unsigned r);
_XI_APIREF_ XI_ERR_TYPE xiConvertBitDepth_U8S16A_ref  (const xi_pArray src, xi_pArray dst, unsigned r);
_XI_APIREF_ XI_ERR_TYPE xiConvertBitDepth_S16U8_ref  (const xi_pArray src, xi_pArray dst, unsigned r);
_XI_APIREF_ XI_ERR_TYPE xiConvertBitDepth_S16U8A_ref  (const xi_pArray src, xi_pArray dst, unsigned r);
_XI_APIREF_ XI_ERR_TYPE xiConvertBitDepth_S_S16U8_ref(const xi_pArray src, xi_pArray dst, unsigned r);
_XI_APIREF_ XI_ERR_TYPE xiConvertBitDepth_S_S16U8A_ref(const xi_pArray src, xi_pArray dst, unsigned r);

/* look-up */
_XI_APIREF_ XI_ERR_TYPE xiLUT_U8_ref(const xi_pTile src, xi_pTile dst, uint8_t* lut, int size);
_XI_APIREF_ XI_ERR_TYPE xiLUT_S16_ref(const xi_pArray src, xi_pArray dst, int16_t *lut, int lutsz, int lutoffs);


/* merge channels */
_XI_APIREF_ XI_ERR_TYPE xiMerge2_U8_ref    (const xi_pArray src0,  const xi_pArray src1,  xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMerge2_S16_ref   (const xi_pArray src0,  const xi_pArray src1,  xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMerge3_U8_ref    (const xi_pArray src0,  const xi_pArray src1,  const xi_pArray src2,  xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMerge3_S16_ref   (const xi_pArray src0,  const xi_pArray src1,  const xi_pArray src2,  xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMerge4_U8_ref    (const xi_pArray src0,  const xi_pArray src1,  const xi_pArray src2,  const xi_pArray src3, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMerge4_S16_ref   (const xi_pArray src0,  const xi_pArray src1,  const xi_pArray src2,  const xi_pArray src3, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMerge_U8_YUYV_ref(const xi_pArray src_y, const xi_pArray src_u, const xi_pArray src_v, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMerge_U8_UYVY_ref(const xi_pArray src_y, const xi_pArray src_u, const xi_pArray src_v, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMerge2_I8_ref    (const xi_pArray src0,  const xi_pArray src1,  xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMerge2_I8A2_ref    (const xi_pArray src0,  const xi_pArray src1,  xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMerge2_I16_ref   (const xi_pArray src0,  const xi_pArray src1,  xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMerge2_I16A_ref   (const xi_pArray src0,  const xi_pArray src1,  xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMerge3_I8_ref    (const xi_pArray src0,  const xi_pArray src1,  const xi_pArray src2,  xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMerge3_I8A2_ref    (const xi_pArray src0,  const xi_pArray src1,  const xi_pArray src2,  xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMerge3_I16A_ref   (const xi_pArray src0,  const xi_pArray src1,  const xi_pArray src2,  xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMerge3_I16_ref   (const xi_pArray src0,  const xi_pArray src1,  const xi_pArray src2,  xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMerge4_I8_ref    (const xi_pArray src0,  const xi_pArray src1,  const xi_pArray src2,  const xi_pArray src3, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMerge4_I8A2_ref    (const xi_pArray src0,  const xi_pArray src1,  const xi_pArray src2,  const xi_pArray src3, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMerge4_I16_ref   (const xi_pArray src0,  const xi_pArray src1,  const xi_pArray src2,  const xi_pArray src3, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMerge4_I16A_ref   (const xi_pArray src0,  const xi_pArray src1,  const xi_pArray src2,  const xi_pArray src3, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMerge_U8A2_YUYV_ref(const xi_pArray src_y, const xi_pArray src_u, const xi_pArray src_v, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMerge_U8A2_UYVY_ref(const xi_pArray src_y, const xi_pArray src_u, const xi_pArray src_v, xi_pArray dst);


/* split channels */
_XI_APIREF_ XI_ERR_TYPE xiSplit2_U8_ref (const xi_pArray src, xi_pArray dst0, xi_pArray dst1);
_XI_APIREF_ XI_ERR_TYPE xiSplit2_S16_ref(const xi_pArray src, xi_pArray dst0, xi_pArray dst1);
_XI_APIREF_ XI_ERR_TYPE xiSplit2_I8_ref (const xi_pArray src, xi_pArray dst0, xi_pArray dst1);
_XI_APIREF_ XI_ERR_TYPE xiSplit2_I16_ref(const xi_pArray src, xi_pArray dst0, xi_pArray dst1);
_XI_APIREF_ XI_ERR_TYPE xiSplit2_I8A2_ref (const xi_pArray src, xi_pArray dst0, xi_pArray dst1);
_XI_APIREF_ XI_ERR_TYPE xiSplit2_I16A_ref(const xi_pArray src, xi_pArray dst0, xi_pArray dst1);

_XI_APIREF_ XI_ERR_TYPE xiSplit3_U8_ref (const xi_pArray src, xi_pArray dst0, xi_pArray dst1, xi_pArray dst2);
_XI_APIREF_ XI_ERR_TYPE xiSplit3_S16_ref(const xi_pArray src, xi_pArray dst0, xi_pArray dst1, xi_pArray dst2);
_XI_APIREF_ XI_ERR_TYPE xiSplit3_I8_ref (const xi_pArray src, xi_pArray dst0, xi_pArray dst1, xi_pArray dst2);
_XI_APIREF_ XI_ERR_TYPE xiSplit3_I16_ref(const xi_pArray src, xi_pArray dst0, xi_pArray dst1, xi_pArray dst2);
_XI_APIREF_ XI_ERR_TYPE xiSplit3_I8A2_ref (const xi_pArray src, xi_pArray dst0, xi_pArray dst1, xi_pArray dst2);
_XI_APIREF_ XI_ERR_TYPE xiSplit3_I16A_ref(const xi_pArray src, xi_pArray dst0, xi_pArray dst1, xi_pArray dst2);

_XI_APIREF_ XI_ERR_TYPE xiSplit4_U8_ref (const xi_pArray src, xi_pArray dst0, xi_pArray dst1, xi_pArray dst2, xi_pArray dst3);
_XI_APIREF_ XI_ERR_TYPE xiSplit4_S16_ref(const xi_pArray src, xi_pArray dst0, xi_pArray dst1, xi_pArray dst2, xi_pArray dst3);
_XI_APIREF_ XI_ERR_TYPE xiSplit4_I8_ref (const xi_pArray src, xi_pArray dst0, xi_pArray dst1, xi_pArray dst2, xi_pArray dst3);
_XI_APIREF_ XI_ERR_TYPE xiSplit4_I16_ref(const xi_pArray src, xi_pArray dst0, xi_pArray dst1, xi_pArray dst2, xi_pArray dst3);
_XI_APIREF_ XI_ERR_TYPE xiSplit4_I8A2_ref (const xi_pArray src, xi_pArray dst0, xi_pArray dst1, xi_pArray dst2, xi_pArray dst3);
_XI_APIREF_ XI_ERR_TYPE xiSplit4_I16A_ref(const xi_pArray src, xi_pArray dst0, xi_pArray dst1, xi_pArray dst2, xi_pArray dst3);


/* extract channel */
_XI_APIREF_ XI_ERR_TYPE xiExtractChannel_U8_ref(const xi_pTile src, xi_pTile dst, int src_channels, int channel);
_XI_APIREF_ XI_ERR_TYPE xiExtractChannel2_I8_ref(const xi_pTile src, xi_pTile dst, int src_channels, int channel);
_XI_APIREF_ XI_ERR_TYPE xiExtractChannel3_I8_ref(const xi_pTile src, xi_pTile dst, int src_channels, int channel);
_XI_APIREF_ XI_ERR_TYPE xiExtractChannel4_I8_ref(const xi_pTile src, xi_pTile dst, int src_channels, int channel);
_XI_APIREF_ XI_ERR_TYPE xiExtractChannel2_I8A2_ref(const xi_pTile src, xi_pTile dst, int src_channels, int channel);
_XI_APIREF_ XI_ERR_TYPE xiExtractChannel3_I8A2_ref(const xi_pTile src, xi_pTile dst, int src_channels, int channel);
_XI_APIREF_ XI_ERR_TYPE xiExtractChannel4_I8A2_ref(const xi_pTile src, xi_pTile dst, int src_channels, int channel);
/* extract keypoint */
_XI_APIREF_ XI_ERR_TYPE xiExtractKeypoints_U8_ref    (const xi_pTile responce_map, xi_keypoint* keypoints, int max_points, int* count);
_XI_APIREF_ XI_ERR_TYPE xiExtractNextKeypoints_U8_ref(const xi_pTile responce_map, xi_point32 start_loc, xi_keypoint* keypoints, int max_points, int* count);
_XI_APIREF_ XI_ERR_TYPE xiExtractKeypoints_U8A_ref    (const xi_pTile responce_map, xi_keypoint* keypoints, int max_points, int* count);
_XI_APIREF_ XI_ERR_TYPE xiExtractNextKeypoints_U8A_ref(const xi_pTile responce_map, xi_point32 start_loc, xi_keypoint* keypoints, int max_points, int* count);


/* phase */
_XI_APIREF_ XI_ERR_TYPE xiPhase_U8_ref   (const xi_pArray tileX, const xi_pArray tileY, xi_pArray tileAngle_Q3_12);
_XI_APIREF_ XI_ERR_TYPE xiPhase_S16_ref  (const xi_pArray tileX, const xi_pArray tileY, xi_pArray tileAngle_Q3_12);
_XI_APIREF_ XI_ERR_TYPE xiPhase_S16U8_ref(const xi_pArray tileX, const xi_pArray tileY, xi_pArray tileAngle);
_XI_APIREF_ XI_ERR_TYPE xiPhase_U8S16_Q3_12_2_ref(const xi_pArray tileX, const xi_pArray tileY, xi_pArray tileAngle_Q3_12, const xi_pArray atan_lut);
_XI_APIREF_ XI_ERR_TYPE xiPhase_S16_Q3_12_2_ref(const xi_pArray tileX, const xi_pArray tileY, xi_pArray tileAngle_Q3_12, const xi_pArray atan_lut);
_XI_APIREF_ XI_ERR_TYPE xiPhase_S16U8_2_ref(const xi_pArray tileX, const xi_pArray tileY, xi_pArray tileAngle_Q3_12, const xi_pArray atan_lut);
_XI_APIREF_ XI_ERR_TYPE xiPhase_U8S16A_Q3_12_2_ref(const xi_pArray tileX, const xi_pArray tileY, xi_pArray tileAngle_Q3_12, const xi_pArray atan_lut);
_XI_APIREF_ XI_ERR_TYPE xiPhase_S16A_Q3_12_2_ref(const xi_pArray tileX, const xi_pArray tileY, xi_pArray tileAngle_Q3_12, const xi_pArray atan_lut);
_XI_APIREF_ XI_ERR_TYPE xiPhase_S16U8A_2_ref(const xi_pArray tileX, const xi_pArray tileY, xi_pArray tileAngle_Q3_12, const xi_pArray atan_lut);


/*  B = max(A, a) */
_XI_APIREF_ XI_ERR_TYPE xiMaxScalar_U8_ref (const xi_pArray src, xi_pArray dst, unsigned a);
_XI_APIREF_ XI_ERR_TYPE xiMaxScalar_S16_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiMaxScalar_U8A2_ref (const xi_pArray src, xi_pArray dst, unsigned a);
_XI_APIREF_ XI_ERR_TYPE xiMaxScalar_S16A_ref(const xi_pArray src, xi_pArray dst, int a);


/*  B = min(A, a) */
_XI_APIREF_ XI_ERR_TYPE xiMinScalar_U8_ref (const xi_pArray src, xi_pArray dst, unsigned a);
_XI_APIREF_ XI_ERR_TYPE xiMinScalar_S16_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiMinScalar_U8A2_ref (const xi_pArray src, xi_pArray dst, unsigned a);
_XI_APIREF_ XI_ERR_TYPE xiMinScalar_S16A_ref(const xi_pArray src, xi_pArray dst, int a);

/* B = A * a + b */
_XI_APIREF_ XI_ERR_TYPE xiConvertScale_U8_ref (const xi_pArray src, xi_pArray dst, int a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScale_U8A_ref (const xi_pArray src, xi_pArray dst, int a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScale_S16_ref(const xi_pArray src, xi_pArray dst, int a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScale_S16A_ref(const xi_pArray src, xi_pArray dst, int a, int b);

_XI_APIREF_ XI_ERR_TYPE xiConvertScale_U8_Q15_ref (const xi_pArray src, xi_pArray dst, XI_Q15 a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScale_U8A_Q15_ref (const xi_pArray src, xi_pArray dst, XI_Q15 a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScale_S16_Q15_ref(const xi_pArray src, xi_pArray dst, XI_Q15 a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScale_S16A_Q15_ref(const xi_pArray src, xi_pArray dst, XI_Q15 a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScale_U8_Q5_10_ref (const xi_pArray src, xi_pArray dst, XI_Q5_10 a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScale_U8A_Q5_10_ref (const xi_pArray src, xi_pArray dst, XI_Q5_10 a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScale_S16_Q5_10_ref(const xi_pArray src, xi_pArray dst, XI_Q5_10 a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScale_S16A_Q5_10_ref(const xi_pArray src, xi_pArray dst, XI_Q5_10 a, int b);

_XI_APIREF_ XI_ERR_TYPE xiConvertScale_S_U8_ref (const xi_pArray src, xi_pArray dst, int a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScale_S_U8A2_ref (const xi_pArray src, xi_pArray dst, int a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScale_S_S16_ref(const xi_pArray src, xi_pArray dst, int a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScale_S_S16A_ref(const xi_pArray src, xi_pArray dst, int a, int b);

_XI_APIREF_ XI_ERR_TYPE xiConvertScale_S_U8_Q15_ref (const xi_pArray src, xi_pArray dst, XI_Q15 a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScale_S_U8A_Q15_ref (const xi_pArray src, xi_pArray dst, XI_Q15 a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScale_S_S16_Q15_ref(const xi_pArray src, xi_pArray dst, XI_Q15 a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScale_S_S16A_Q15_ref(const xi_pArray src, xi_pArray dst, XI_Q15 a, int b);

_XI_APIREF_ XI_ERR_TYPE xiConvertScale_S_U8_Q5_10_ref (const xi_pArray src, xi_pArray dst, XI_Q5_10 a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScale_S_U8A2_Q5_10_ref (const xi_pArray src, xi_pArray dst, XI_Q5_10 a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScale_S_S16_Q5_10_ref(const xi_pArray src, xi_pArray dst, XI_Q5_10 a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScale_S_S16A_Q5_10_ref(const xi_pArray src, xi_pArray dst, XI_Q5_10 a, int b);

/* B = |A * a + b| */
_XI_APIREF_ XI_ERR_TYPE xiConvertScaleAbs_U8_ref (const xi_pArray src, xi_pArray dst, int a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScaleAbs_U8A_ref (const xi_pArray src, xi_pArray dst, int a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScaleAbs_S16_ref(const xi_pArray src, xi_pArray dst, int a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScaleAbs_S16A_ref(const xi_pArray src, xi_pArray dst, int a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScaleAbs_U8_Q15_ref (const xi_pArray src, xi_pArray dst, XI_Q15 a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScaleAbs_U8A_Q15_ref (const xi_pArray src, xi_pArray dst, XI_Q15 a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScaleAbs_S16_Q15_ref(const xi_pArray src, xi_pArray dst, XI_Q15 a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScaleAbs_S16A_Q15_ref(const xi_pArray src, xi_pArray dst, XI_Q15 a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScaleAbs_U8A_Q5_10_ref (const xi_pArray src, xi_pArray dst, XI_Q5_10 a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScaleAbs_U8_Q5_10_ref (const xi_pArray src, xi_pArray dst, XI_Q5_10 a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScaleAbs_S16A_Q5_10_ref(const xi_pArray src, xi_pArray dst, XI_Q5_10 a, int b);
_XI_APIREF_ XI_ERR_TYPE xiConvertScaleAbs_S16_Q5_10_ref(const xi_pArray src, xi_pArray dst, XI_Q5_10 a, int b);


/* bitwise scalar and, or, xor */
_XI_APIREF_ XI_ERR_TYPE xiBitwiseAndScalar_U8_ref (const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiBitwiseAndScalar_S16_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiBitwiseAndScalar_I8A2_ref (const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiBitwiseAndScalar_I16A_ref (const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiBitwiseAndScalar_I8_ref (const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiBitwiseAndScalar_I16_ref(const xi_pArray src, xi_pArray dst, int a);

_XI_APIREF_ XI_ERR_TYPE xiBitwiseOrScalar_U8_ref (const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiBitwiseOrScalar_S16_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiBitwiseOrScalar_I8A2_ref (const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiBitwiseOrScalar_I16A_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiBitwiseOrScalar_I8_ref (const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiBitwiseOrScalar_I16_ref(const xi_pArray src, xi_pArray dst, int a);

_XI_APIREF_ XI_ERR_TYPE xiBitwiseXorScalar_U8_ref (const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiBitwiseXorScalar_S16_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiBitwiseXorScalar_I8_ref (const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiBitwiseXorScalar_I16_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiBitwiseXorScalar_I8A2_ref (const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiBitwiseXorScalar_I16A_ref(const xi_pArray src, xi_pArray dst, int a);


/* B = a * A */
#define xiMultiplyScalar_U8_ref(src, dst, a)  xiConvertScale_U8_ref (src, dst, a, 0)
#define xiMultiplyScalar_I8A2_ref(src, dst, a)   xiConvertScale_U8_ref (src, dst, a, 0)
#define xiMultiplyScalar_I8_ref(src, dst, a)  xiConvertScale_U8_ref (src, dst, a, 0)
#define xiMultiplyScalar_S16_ref(src, dst, a) xiConvertScale_S16_ref(src, dst, a, 0)
#define xiMultiplyScalar_I16A_ref(src, dst, a) xiConvertScale_S16_ref(src, dst, a, 0)


#define xiMultiplyScalar_U8_Q15_ref(src, dst, a)  xiConvertScale_U8_Q15_ref (src, dst, a, 0)
#define xiMultiplyScalar_S16_Q15_ref(src, dst, a) xiConvertScale_S16_Q15_ref(src, dst, a, 0)
#define xiMultiplyScalar_U8A_Q15_ref(src, dst, a)  xiConvertScale_U8_Q15_ref (src, dst, a, 0)
#define xiMultiplyScalar_S16A_Q15_ref(src, dst, a) xiConvertScale_S16_Q15_ref(src, dst, a, 0)

#define xiMultiplyScalar_U8_Q5_10_ref(src, dst, a)  xiConvertScale_U8_Q5_10_ref (src, dst, a, 0)
#define xiMultiplyScalar_S16_Q5_10_ref(src, dst, a) xiConvertScale_S16_Q5_10_ref(src, dst, a, 0)
#define xiMultiplyScalar_U8A_Q5_10_ref(src, dst, a)  xiConvertScale_U8_Q5_10_ref (src, dst, a, 0)
#define xiMultiplyScalar_S16A_Q5_10_ref(src, dst, a) xiConvertScale_S16_Q5_10_ref(src, dst, a, 0)
_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalar_S_U8_ref (const xi_pArray src, xi_pArray dst, int a, int bits);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalar_S_S16_ref(const xi_pArray src, xi_pArray dst, int a, int bits);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalar_S_U8A2_ref (const xi_pArray src, xi_pArray dst, int a, int bits);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalar_S_S16A_ref(const xi_pArray src, xi_pArray dst, int a, int bits);


/* B = A + a */
#define xiAddScalar_U8_ref(src, dst, a)  xiConvertScale_U8_ref (src, dst, 1, a)
#define xiAddScalar_S16_ref(src, dst, a) xiConvertScale_S16_ref(src, dst, 1, a)

_XI_APIREF_ XI_ERR_TYPE xiAddScalar_S_U8_ref (const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiAddScalar_S_S16_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiAddScalar_S_U8S16_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiAddScalar_U8S16_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiAddScalar_S_U8A_ref (const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiAddScalar_S_S16A_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiAddScalar_U8S16A_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiAddScalar_I8_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiAddScalar_I16_ref(const xi_pArray src, xi_pArray dst, int a);
/* B = a - A */
#define xiSubtractScalar_U8_ref(src, dst, a)  xiConvertScale_U8_ref (src, dst, -1, a)
#define xiSubtractScalar_I8_ref(src, dst, a)  xiConvertScale_U8_ref (src, dst, -1, a)
#define xiSubtractScalar_I8A2_ref(src, dst, a)  xiConvertScale_U8_ref (src, dst, -1, a)
#define xiSubtractScalar_S16_ref(src, dst, a) xiConvertScale_S16_ref(src, dst, -1, a)
#define xiSubtractScalar_I16_ref(src, dst, a) xiConvertScale_S16_ref(src, dst, -1, a)
#define xiSubtractScalar_I16A_ref(src, dst, a) xiConvertScale_S16_ref(src, dst, -1, a)

_XI_APIREF_ XI_ERR_TYPE xiSubtractScalar_S_U8_ref   (const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiSubtractScalar_S_S16_ref  (const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiSubtractScalar_U8S16_ref  (const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiSubtractScalar_S_U8S16_ref(const xi_pArray src, xi_pArray dst, int a);

/* B = A - a */
_XI_APIREF_ XI_ERR_TYPE xiSubtractScalar2_S_U8_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiSubtractScalar2_S_S16_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiSubtractScalar2_S_U8S16_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiSubtractScalar_S_U8A_ref   (const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiSubtractScalar_S_S16A_ref  (const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiSubtractScalar_U8S16A_ref  (const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiSubtractScalar_S_U8S16A_ref(const xi_pArray src, xi_pArray dst, int a);
/* B = |a - A| */
#define xiAbsdiffScalar_U8_ref(src, dst, a)  xiConvertScaleAbs_U8_ref (src, dst, -1, (uint8_t)a)
#define xiAbsdiffScalar_S16_ref(src, dst, a) xiConvertScaleAbs_S16_ref(src, dst, -1, (int16_t)a)

_XI_APIREF_ XI_ERR_TYPE xiAbsdiffScalar_S_U8_ref (const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiAbsdiffScalar_S_S16_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiAbsdiffScalar_S_S16A_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiAbsdiffScalar_S_U8A_ref (const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiAbsdiffScalar_S16A_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiAbsdiffScalar_U8A_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiAbsdiffScalar_U8A2_ref(const xi_pArray src, xi_pArray dst, int a);
/* B = a / A */
_XI_APIREF_ XI_ERR_TYPE xiDivideScalar_U8_ref (const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiDivideScalar_S16_ref(const xi_pArray src, xi_pArray dst, int a);

_XI_APIREF_ XI_ERR_TYPE xiDivideScalar_U8_Q_ref   (const xi_pArray src, xi_pArray dst, unsigned a);
_XI_APIREF_ XI_ERR_TYPE xiDivideScalar_U8U16_Q_ref(const xi_pArray src, xi_pArray dst, unsigned a);
_XI_APIREF_ XI_ERR_TYPE xiDivideScalar_U16_Q_ref  (const xi_pArray src, xi_pArray dst, unsigned a);
_XI_APIREF_ XI_ERR_TYPE xiDivideScalar_U8A_ref (const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiDivideScalar_S16A_ref(const xi_pArray src, xi_pArray dst, int a);

_XI_APIREF_ XI_ERR_TYPE xiDivideScalar_U8A_Q_ref   (const xi_pArray src, xi_pArray dst, unsigned a);
_XI_APIREF_ XI_ERR_TYPE xiDivideScalar_U8U16A_Q_ref(const xi_pArray src, xi_pArray dst, unsigned a);
_XI_APIREF_ XI_ERR_TYPE xiDivideScalar_U16A_Q_ref  (const xi_pArray src, xi_pArray dst, unsigned a);

/* B = A ? a (==, !=, >, <, >=, <=) */
_XI_APIREF_ XI_ERR_TYPE xiCompareScalarEQ_U8_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiCompareScalarNE_U8_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiCompareScalarGT_U8_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiCompareScalarLT_U8_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiCompareScalarGE_U8_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiCompareScalarLE_U8_ref(const xi_pArray src, xi_pArray dst, int a);

_XI_APIREF_ XI_ERR_TYPE xiCompareScalarEQ_S16U8_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiCompareScalarNE_S16U8_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiCompareScalarGT_S16U8_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiCompareScalarLT_S16U8_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiCompareScalarGE_S16U8_ref(const xi_pArray src, xi_pArray dst, int a);
_XI_APIREF_ XI_ERR_TYPE xiCompareScalarLE_S16U8_ref(const xi_pArray src, xi_pArray dst, int a);


/* C = max(A, B) */
_XI_APIREF_ XI_ERR_TYPE xiMax_U8_ref (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMax_S16_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);


/* C = min(A, B) */
_XI_APIREF_ XI_ERR_TYPE xiMin_U8_ref (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMin_S16_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);


/* C = a * A + b * B + c */
_XI_APIREF_ XI_ERR_TYPE xiAddWeighted_U8_ref (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int a, int b, int c);
_XI_APIREF_ XI_ERR_TYPE xiAddWeighted_S16_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int a, int b, int c);

_XI_APIREF_ XI_ERR_TYPE xiAddWeighted_U8_Q15_ref (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q15 a, XI_Q15 b, int c);
_XI_APIREF_ XI_ERR_TYPE xiAddWeighted_S16_Q15_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q15 a, XI_Q15 b, int c);

_XI_APIREF_ XI_ERR_TYPE xiAddWeighted_U8_Q5_10_ref (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q5_10 a, XI_Q5_10 b, int c);
_XI_APIREF_ XI_ERR_TYPE xiAddWeighted_S16_Q5_10_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q5_10 a, XI_Q5_10 b, int c);

_XI_APIREF_ XI_ERR_TYPE xiAddWeighted_S_U8_ref (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int a, int b, int c, int bits);
_XI_APIREF_ XI_ERR_TYPE xiAddWeighted_S_S16_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int a, int b, int c, int bits);


/* C = A + B */
#define xiAdd_U8_ref(src0, src1, dst)  xiAddWeighted_U8_ref (src0, src1, dst, 1, 1, 0)
#define xiAdd_S16_ref(src0, src1, dst) xiAddWeighted_S16_ref(src0, src1, dst, 1, 1, 0)

_XI_APIREF_ XI_ERR_TYPE xiAdd_S_U8_ref (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiAdd_S_S16_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

_XI_APIREF_ XI_ERR_TYPE xiAdd_U8S16_ref     (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiAdd_U8S16S16_ref  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiAdd_S_U8S16S16_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

_XI_APIREF_ XI_ERR_TYPE xiAdd_U8S16S32_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiAdd_S16S32_ref  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

_XI_APIREF_ XI_ERR_TYPE xiAdd_S_S16U8_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

/* C = A - B */
#define xiSubtract_U8_ref(src0, src1, dst)  xiAddWeighted_U8_ref (src0, src1, dst, 1, -1, 0)
#define xiSubtract_S16_ref(src0, src1, dst) xiAddWeighted_S16_ref(src0, src1, dst, 1, -1, 0)

_XI_APIREF_ XI_ERR_TYPE xiSubtract_U8S16_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiSubtract_S_U8_ref (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiSubtract_S_S16_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

_XI_APIREF_ XI_ERR_TYPE xiSubtract_S16U8S16_ref  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiSubtract_U8S16S16_ref  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiSubtract_S_S16U8S16_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiSubtract_S_U8S16S16_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

_XI_APIREF_ XI_ERR_TYPE xiSubtract_U8S16S32_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiSubtract_S16U8S32_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiSubtract_S16S32_ref  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

/* C = |A - B| */
_XI_APIREF_ XI_ERR_TYPE xiAbsdiff_U8_ref (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiAbsdiff_S16_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

_XI_APIREF_ XI_ERR_TYPE xiAbsdiff_S_S16_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);


/* C = A * B */
_XI_APIREF_ XI_ERR_TYPE xiMultiply_U8_ref   (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMultiply_U8U16_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMultiply_S16_ref  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

_XI_APIREF_ XI_ERR_TYPE xiMultiply_S_U8_ref (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int bits);
_XI_APIREF_ XI_ERR_TYPE xiMultiply_S_S16_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int bits);


/* C = trunc((A * B) >> s) */
_XI_APIREF_ XI_ERR_TYPE xiMultiplyTrunc_R_U8_ref      (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int shift);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyTrunc_R_U8S16_ref   (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int shift);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyTrunc_R_U8S16S16_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int shift);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyTrunc_R_S16_ref     (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int shift);

_XI_APIREF_ XI_ERR_TYPE xiMultiplyTrunc_SR_U8_ref      (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int shift);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyTrunc_SR_U8S16_ref   (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int shift);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyTrunc_SR_U8S16S16_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int shift);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyTrunc_SR_S16_ref     (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int shift);


/* dst = trunc((src * scalar) >> shift) */
_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalarTrunc_R_U8_ref       (const xi_pArray src, xi_pArray dst, int scalar, int shift);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalarTrunc_R_U8S16_ref    (const xi_pArray src, xi_pArray dst, int scalar, int shift);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalarTrunc_R_U8S16S16_ref (const xi_pArray src, xi_pArray dst, int scalar, int shift);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalarTrunc_R_S16_ref      (const xi_pArray src, xi_pArray dst, int scalar, int shift);

_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalarTrunc_SR_U8_ref      (const xi_pArray src, xi_pArray dst, int scalar, int shift);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalarTrunc_SR_U8S16_ref   (const xi_pArray src, xi_pArray dst, int scalar, int shift);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalarTrunc_SR_U8S16S16_ref(const xi_pArray src, xi_pArray dst, int scalar, int shift);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalarTrunc_SR_S16_ref     (const xi_pArray src, xi_pArray dst, int scalar, int shift);


/* C = round((A * B) / d) */
_XI_APIREF_ XI_ERR_TYPE xiMultiplyRound_D_U8_ref      (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, unsigned divisor);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyRound_D_U8S16_ref   (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, unsigned divisor);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyRound_D_U8S16S16_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst, unsigned divisor);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyRound_D_S16_ref     (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, unsigned divisor);

_XI_APIREF_ XI_ERR_TYPE xiMultiplyRound_SD_U8_ref      (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, unsigned divisor);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyRound_SD_U8S16_ref   (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, unsigned divisor);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyRound_SD_U8S16S16_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst, unsigned divisor);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyRound_SD_S16_ref     (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, unsigned divisor);


/* dst = round((src * scalar) / divisor) */
_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalarRound_D_U8_ref       (const xi_pArray src, xi_pArray dst, int scalar, unsigned int divisor);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalarRound_D_U8S16_ref    (const xi_pArray src, xi_pArray dst, int scalar, unsigned int divisor);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalarRound_D_U8S16S16_ref (const xi_pArray src, xi_pArray dst, int scalar, unsigned int divisor);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalarRound_D_S16_ref      (const xi_pArray src, xi_pArray dst, int scalar, unsigned int divisor);

_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalarRound_SD_U8_ref      (const xi_pArray src, xi_pArray dst, int scalar, unsigned int divisor);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalarRound_SD_U8S16_ref   (const xi_pArray src, xi_pArray dst, int scalar, unsigned int divisor);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalarRound_SD_U8S16S16_ref(const xi_pArray src, xi_pArray dst, int scalar, unsigned int divisor);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalarRound_SD_S16_ref     (const xi_pArray src, xi_pArray dst, int scalar, unsigned int divisor);


/* C = round((A * B) * s) */
_XI_APIREF_ XI_ERR_TYPE xiMultiplyRound_Q_U8_ref      (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q0_16 scale);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyRound_Q_U8S16_ref   (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q0_16 scale);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyRound_Q_U8S16S16_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q0_16 scale);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyRound_Q_S16_ref     (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q0_16 scale);

_XI_APIREF_ XI_ERR_TYPE xiMultiplyRound_Q_S_U8_ref      (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q0_16 scale);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyRound_Q_S_U8S16_ref   (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q0_16 scale);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyRound_Q_S_U8S16S16_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q0_16 scale);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyRound_Q_S_S16_ref     (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q0_16 scale);


/* dst = round((src * scalar) * scale) */
_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalarRound_Q_U8_ref        (const xi_pArray src, xi_pArray dst, int scalar, XI_Q0_16 scale);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalarRound_Q_U8S16_ref     (const xi_pArray src, xi_pArray dst, int scalar, XI_Q0_16 scale);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalarRound_Q_U8S16S16_ref  (const xi_pArray src, xi_pArray dst, int scalar, XI_Q0_16 scale);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalarRound_Q_S16_ref       (const xi_pArray src, xi_pArray dst, int scalar, XI_Q0_16 scale);

_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalarRound_Q_S_U8_ref      (const xi_pArray src, xi_pArray dst, int scalar, XI_Q0_16 scale);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalarRound_Q_S_U8S16_ref   (const xi_pArray src, xi_pArray dst, int scalar, XI_Q0_16 scale);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalarRound_Q_S_U8S16S16_ref(const xi_pArray src, xi_pArray dst, int scalar, XI_Q0_16 scale);
_XI_APIREF_ XI_ERR_TYPE xiMultiplyScalarRound_Q_S_S16_ref     (const xi_pArray src, xi_pArray dst, int scalar, XI_Q0_16 scale);


/* C = A / B */
_XI_APIREF_ XI_ERR_TYPE xiDivide_U8_ref (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiDivide_S16_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

_XI_APIREF_ XI_ERR_TYPE xiDivide_U8_Q_ref   (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiDivide_U8U16_Q_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiDivide_U16_Q_ref  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);


/* C = A ? B (==, !=, >, <, >=, <=) */
_XI_APIREF_ XI_ERR_TYPE xiCompareEQ_U8_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiCompareNE_U8_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiCompareGT_U8_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiCompareGE_U8_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

_XI_APIREF_ XI_ERR_TYPE xiCompareEQ_S16U8_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiCompareNE_S16U8_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiCompareGT_S16U8_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiCompareGE_S16U8_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

#define xiCompareLT_U8_ref(src0, src1, dst)    xiCompareGT_U8_ref(src1, src0, dst)
#define xiCompareLE_U8_ref(src0, src1, dst)    xiCompareGE_U8_ref(src1, src0, dst)
#define xiCompareLT_S16U8_ref(src0, src1, dst) xiCompareGT_S16U8_ref(src1, src0, dst)
#define xiCompareLE_S16U8_ref(src0, src1, dst) xiCompareGE_S16U8_ref(src1, src0, dst)


/* bitwise and, or, xor */
_XI_APIREF_ XI_ERR_TYPE xiBitwiseAnd_U8_ref (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiBitwiseAnd_S16_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

_XI_APIREF_ XI_ERR_TYPE xiBitwiseOr_U8_ref  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiBitwiseOr_S16_ref (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

_XI_APIREF_ XI_ERR_TYPE xiBitwiseXor_U8_ref (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiBitwiseXor_S16_ref(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);


/* B = ~A */
_XI_APIREF_ XI_ERR_TYPE xiBitwiseNot_U8_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiBitwiseNot_S16_ref(const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiBitwiseNot_I8_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiBitwiseNot_I16_ref(const xi_pArray src, xi_pArray dst);

/* B = -A */
#define xiNeg_S16_ref(src, dst)   xiSubtractScalar_S16_ref(src, dst, 0)
#define xiNeg_S_S16_ref(src, dst) xiSubtractScalar_S_S16_ref(src, dst, 0)
_XI_APIREF_ XI_ERR_TYPE xiNeg_U8S16_ref(const xi_pArray src, xi_pArray dst);


/* B = |A| */
#define xiAbs_S16_ref(src, dst)   xiAbsdiffScalar_S16_ref(src, dst, 0)
#define xiAbs_S_S16_ref(src, dst) xiAbsdiffScalar_S_S16_ref(src, dst, 0)


/* box filter */
_XI_APIREF_ XI_ERR_TYPE xiBoxFilter_U8_ref    (const xi_pTile src, xi_pTile dst, int ksize);
_XI_APIREF_ XI_ERR_TYPE xiBoxFilter_S16_ref   (const xi_pTile src, xi_pTile dst, int ksize);
_XI_APIREF_ XI_ERR_TYPE xiBoxFilter_U8U16_ref (const xi_pTile src, xi_pTile dst, int ksize);
_XI_APIREF_ XI_ERR_TYPE xiBoxFilter_S16S32_ref(const xi_pTile src, xi_pTile dst, int ksize);
_XI_APIREF_ XI_ERR_TYPE xiBoxFilter_3x3_U8_ref(const xi_pTile src, xi_pTile dst);
_XI_APIREF_ XI_ERR_TYPE xiBoxFilter_3x3_S16_ref(const xi_pTile src, xi_pTile dst);
_XI_APIREF_ XI_ERR_TYPE xiBoxFilter_3x3_S16Oa_ref(const xi_pTile src, xi_pTile dst);
_XI_APIREF_ XI_ERR_TYPE xiBoxFilter_5x5_S16Oa_ref(const xi_pTile src, xi_pTile dst);
_XI_APIREF_ XI_ERR_TYPE xiBoxFilter_3x3_U8U16_ref(const xi_pTile src, xi_pTile dst);
_XI_APIREF_ XI_ERR_TYPE xiBoxFilter_3x3_U8U16Oa_ref(const xi_pTile src, xi_pTile dst);
_XI_APIREF_ XI_ERR_TYPE xiBoxFilter_3x3_S16S32_ref(const xi_pTile src, xi_pTile dst);
_XI_APIREF_ XI_ERR_TYPE xiBoxFilter_3x3_S16S32Oa_ref(const xi_pTile src, xi_pTile dst);
_XI_APIREF_ XI_ERR_TYPE xiBoxFilter_5x5_U8_ref(const xi_pTile src, xi_pTile dst);
_XI_APIREF_ XI_ERR_TYPE xiBoxFilter_5x5_U8Oa_ref(const xi_pTile src, xi_pTile dst);
_XI_APIREF_ XI_ERR_TYPE xiBoxFilter_5x5_U8U16_ref(const xi_pTile src, xi_pTile dst);
_XI_APIREF_ XI_ERR_TYPE xiBoxFilter_5x5_U8U16Oa_ref(const xi_pTile src, xi_pTile dst);
_XI_APIREF_ XI_ERR_TYPE xiBoxFilter_5x5_S16S32_ref(const xi_pTile src, xi_pTile dst);
_XI_APIREF_ XI_ERR_TYPE xiBoxFilter_5x5_S16S32Oa_ref(const xi_pTile src, xi_pTile dst);
_XI_APIREF_ XI_ERR_TYPE xiBoxFilter_5x5_S16_ref(const xi_pTile src, xi_pTile dst);
/* erode */
_XI_APIREF_ XI_ERR_TYPE xiErode_U8_ref    (const xi_pTile src, xi_pArray dst, int ksize);
_XI_APIREF_ XI_ERR_TYPE xiErode_NxM_U8_ref(const xi_pTile src, xi_pArray dst, int kwidth, int kheight);
_XI_APIREF_ XI_ERR_TYPE xiErode_S16_ref   (const xi_pTile src, xi_pArray dst, int ksize);
_XI_APIREF_ XI_ERR_TYPE xiErode_5x5_U8_ref (const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiErode_3x3_S16_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiErode_5x5_U8Oa2_ref (const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiErode_5x5_S16_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiErode_3x3_U8_ref (const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiErode_5x5_S16Oa_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiErode_3x3_U8Oa2_ref (const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiErode_3x3_S16Oa_ref(const xi_pTile src, xi_pArray dst);

/*erode pattern */
_XI_APIREF_ XI_ERR_TYPE xiErodePattern_NxM_U8_ref(const xi_pTile src, xi_pArray dst, const uint8_t* mask, int width_mask, int height_mask);

_XI_APIREF_ XI_ERR_TYPE xiErodePatternCross_3x3_U8_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiErodePatternCross_5x5_U8_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiErodePatternCross_7x7_U8_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiErodePatternCross_9x9_U8_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiErodePatternDisk_5x5_U8_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiErodePatternDisk_7x7_U8_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiErodePatternDisk_9x9_U8_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiErodePatternCross_3x3_U8Oa2_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiErodePatternCross_5x5_U8Oa2_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiErodePatternDisk_5x5_U8Oa2_ref(const xi_pTile src, xi_pArray dst);

/* dilate */
_XI_APIREF_ XI_ERR_TYPE xiDilate_U8_ref    (const xi_pTile src, xi_pArray dst, int ksize);
_XI_APIREF_ XI_ERR_TYPE xiDilate_NxM_U8_ref(const xi_pTile src, xi_pArray dst, int kwidth, int kheight);
_XI_APIREF_ XI_ERR_TYPE xiDilate_S16_ref   (const xi_pTile src, xi_pArray dst, int ksize);
_XI_APIREF_ XI_ERR_TYPE xiDilate_3x3_U8_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiDilate_3x3_S16_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiDilate_5x5_U8_ref (const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiDilate_5x5_S16_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiDilate_5x5_U8Oa2_ref (const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiDilate_5x5_S16Oa_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiDilate_3x3_U8Oa2_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiDilate_3x3_S16Oa_ref(const xi_pTile src, xi_pArray dst);

/*dilate pattern */
_XI_APIREF_  XI_ERR_TYPE xiDilatePattern_NxM_U8_ref(const xi_pTile src, xi_pArray dst, const uint8_t* mask, int width_mask, int height_mask);

_XI_APIREF_ XI_ERR_TYPE xiDilatePatternCross_3x3_U8_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiDilatePatternCross_5x5_U8_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiDilatePatternCross_7x7_U8_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiDilatePatternCross_9x9_U8_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiDilatePatternDisk_5x5_U8_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiDilatePatternDisk_7x7_U8_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiDilatePatternDisk_9x9_U8_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiDilatePatternDisk_5x5_U8Oa2_ref(const xi_pTile src, xi_pArray dst);

/* non maxima suppression */
_XI_APIREF_ XI_ERR_TYPE xiNonMaximaSuppression2D_3x3_U8_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiNonMaximaSuppression2D_3x3_U8Oa2_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiNonMaximaSuppressionAsym2D_3x3_U8_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiNonMaximaSuppressionAsym2D_3x3_U8Oa2_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiNonMaximaSuppression2D_3x3_S16_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiNonMaximaSuppression2D_3x3_S16Oa_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiNonMaximaSuppression2D_3x3_U16_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiNonMaximaSuppression2D_3x3_U16Oa_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiNonMaximaSuppressionAsym2D_3x3_S16_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiNonMaximaSuppressionAsym2D_3x3_S16Oa_ref(const xi_pTile src, xi_pArray dst);

_XI_APIREF_ XI_ERR_TYPE xiNonMaximaSuppression3D_3x3x3_U8_ref (const xi_pTile prev, const xi_pTile curr, const xi_pTile next, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiNonMaximaSuppression3D_3x3x3_U8Oa2_ref (const xi_pTile prev, const xi_pTile curr, const xi_pTile next, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiNonMaximaSuppression3D_3x3x3_S16_ref(const xi_pTile prev, const xi_pTile curr, const xi_pTile next, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiNonMaximaSuppression3D_3x3x3_S16Oa_ref(const xi_pTile prev, const xi_pTile curr, const xi_pTile next, xi_pArray dst);


/* laplacian filters */
_XI_APIREF_ XI_ERR_TYPE xiLaplacian_3x3_U8S16_ref (const xi_pTile src, xi_pArray dst, xi_bool normalize);
_XI_APIREF_ XI_ERR_TYPE xiLaplacian_3x3_U8S16Oa_ref (const xi_pTile src, xi_pArray dst, xi_bool normalize);

_XI_APIREF_ XI_ERR_TYPE xiLaplacian_3x3_S16_ref   (const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiLaplacian_3x3_S16Oa_ref   (const xi_pTile src, xi_pArray dst);

_XI_APIREF_ XI_ERR_TYPE xiLaplacian2_3x3_U8S16_ref(const xi_pTile src, xi_pArray dst, xi_bool normalize);
_XI_APIREF_ XI_ERR_TYPE xiLaplacian2_3x3_U8S16Oa_ref(const xi_pTile src, xi_pArray dst, xi_bool normalize);
_XI_APIREF_ XI_ERR_TYPE xiLaplacian2_3x3_S16_ref  (const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiLaplacian2_3x3_S16Oa_ref  (const xi_pTile src, xi_pArray dst);

_XI_APIREF_ XI_ERR_TYPE xiLaplacian3_3x3_U8S16_ref(const xi_pTile src, xi_pArray dst, xi_bool normalize);
_XI_APIREF_ XI_ERR_TYPE xiLaplacian3_3x3_U8S16Oa_ref(const xi_pTile src, xi_pArray dst, xi_bool normalize);
_XI_APIREF_ XI_ERR_TYPE xiLaplacian3_3x3_S16_ref  (const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiLaplacian3_3x3_S16Oa_ref  (const xi_pTile src, xi_pArray dst);


/* Scharr gradients */
_XI_APIREF_ XI_ERR_TYPE xiScharr_3x3_U8S16_ref(const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy, xi_bool normalize);
_XI_APIREF_ XI_ERR_TYPE xiScharr_3x3_U8S16Oa_ref(const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy, xi_bool normalize);
_XI_APIREF_ XI_ERR_TYPE xiScharr_3x3_S16_ref  (const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy);
_XI_APIREF_ XI_ERR_TYPE xiScharr_3x3_S16Oa_ref(const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy);


/* Sobel gradients */
_XI_APIREF_ XI_ERR_TYPE xiSobel_3x3_U8S16_ref  (const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy, xi_bool normalize);
_XI_APIREF_ XI_ERR_TYPE xiSobel_3x3_U8S16Oa_ref  (const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy, xi_bool normalize);
_XI_APIREF_ XI_ERR_TYPE xiSobel_3x3_S16_ref    (const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy);
_XI_APIREF_ XI_ERR_TYPE xiSobel_3x3_S16Oa_ref    (const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy);

_XI_APIREF_ XI_ERR_TYPE xiSobel_3x3_S16S32_ref (const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy, xi_bool normalize);
_XI_APIREF_ XI_ERR_TYPE xiSobel_3x3_S16S32Oa_ref (const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy, xi_bool normalize);

_XI_APIREF_ XI_ERR_TYPE xiSobelX_3x3_U8S16_ref (const xi_pTile src, xi_pArray dst_dx, xi_bool normalize);
_XI_APIREF_ XI_ERR_TYPE xiSobelX_3x3_U8S16Oa_ref (const xi_pTile src, xi_pArray dst_dx, xi_bool normalize);
_XI_APIREF_ XI_ERR_TYPE xiSobelY_3x3_U8S16_ref (const xi_pTile src, xi_pArray dst_dy, xi_bool normalize);
_XI_APIREF_ XI_ERR_TYPE xiSobelY_3x3_U8S16Oa_ref (const xi_pTile src, xi_pArray dst_dy, xi_bool normalize);
_XI_APIREF_ XI_ERR_TYPE xiSobelX_3x3_S16_ref   (const xi_pTile src, xi_pArray dst_dx);
_XI_APIREF_ XI_ERR_TYPE xiSobelX_3x3_S16Oa_ref   (const xi_pTile src, xi_pArray dst_dx);

_XI_APIREF_ XI_ERR_TYPE xiSobelY_3x3_S16_ref   (const xi_pTile src, xi_pArray dst_dy);
_XI_APIREF_ XI_ERR_TYPE xiSobelY_3x3_S16Oa_ref   (const xi_pTile src, xi_pArray dst_dy);

_XI_APIREF_ XI_ERR_TYPE xiSobelX_3x3_S16S32_ref(const xi_pTile src, xi_pArray dst_dx, xi_bool normalize);
_XI_APIREF_ XI_ERR_TYPE xiSobelX_3x3_S16S32Oa_ref(const xi_pTile src, xi_pArray dst_dx, xi_bool normalize);
_XI_APIREF_ XI_ERR_TYPE xiSobelY_3x3_S16S32_ref(const xi_pTile src, xi_pArray dst_dx, xi_bool normalize);
_XI_APIREF_ XI_ERR_TYPE xiSobelY_3x3_S16S32Oa_ref(const xi_pTile src, xi_pArray dst_dx, xi_bool normalize);

_XI_APIREF_ XI_ERR_TYPE xiSobel_5x5_U8S16_ref(const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy, xi_bool normalize);
_XI_APIREF_ XI_ERR_TYPE xiSobel_5x5_U8S16Oa_ref(const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy, xi_bool normalize);

_XI_APIREF_ XI_ERR_TYPE xiSobel_5x5_S16_ref  (const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy);
_XI_APIREF_ XI_ERR_TYPE xiSobel_5x5_S16Oa_ref  (const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy);


_XI_APIREF_ XI_ERR_TYPE xiSobel_7x7_U8S16_ref(const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy);


/* integral image */
_XI_APIREF_ XI_ERR_TYPE xiIntegral_U8U16_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiIntegral_U8U32_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiIntegralTopLeftZero_U8U32_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiIntegral_S16_ref   (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiIntegral_S16S32_ref(const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiIntegral_I16_ref   (const xi_pArray src, xi_pArray dst);

/* integral image of squared values */
_XI_APIREF_ XI_ERR_TYPE xiIntegralSqr_U8U32_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiIntegralSqr_S16U32_ref(const xi_pArray src, xi_pArray dst);


/* IntegralImage */
_XI_APIREF_ XI_ERR_TYPE xiIntegralImage_U8U32_ref(const xi_pArray src, const xi_pArray dst_left, const xi_pArray dst_top, xi_pArray dst);


/* 45-degree rotated integral image */
_XI_APIREF_ XI_ERR_TYPE xiIntegral45_U8U16_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiIntegral45_U8U16A_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiIntegral45_S16S32_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiIntegral45_U8U32_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiIntegral45_U8U32A_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiIntegral45_S16_ref   (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiIntegral45_I16_ref   (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiIntegral45_I16A_ref   (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiIntegral45_S16S32_ref(const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiIntegral45_S16S32A_ref(const xi_pArray src, xi_pArray dst);

/* Gaussian Blur */
_XI_APIREF_ XI_ERR_TYPE xiGaussianBlur_3x3_U8_ref  (const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiGaussianBlur_3x3_U8Oa2_ref  (const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiGaussianBlur_R_3x3_U8_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiGaussianBlur_R_3x3_U8Oa2_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiGaussianBlur_3x3_S16_ref (const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiGaussianBlur_3x3_S16Oa_ref (const xi_pTile src, xi_pArray dst);

_XI_APIREF_ XI_ERR_TYPE xiGaussianBlur_R_3x3_S16_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiGaussianBlur_R_3x3_S16Oa_ref(const xi_pTile src, xi_pArray dst);

_XI_APIREF_ XI_ERR_TYPE xiGaussianBlur_5x5_U8_ref (const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiGaussianBlur_5x5_S16_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiGaussianBlur_5x5_S16_high_precision_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiGaussianBlur_5x5_S16Oa_high_precision_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiGaussianBlur_5x5_S16Oa_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiGaussianBlur_5x5_S16Oa_ref(const xi_pTile src, xi_pArray dst);



/* Median Blur */
_XI_APIREF_ XI_ERR_TYPE xiMedianBlurPattern_MxN_U8_ref(const xi_pTile src, xi_pArray dst, const uint8_t* mask, int width_mask, int height_mask);
_XI_APIREF_ XI_ERR_TYPE xiMedianBlurPattern_MxN_S16_ref(const xi_pTile src, xi_pArray dst, const uint8_t* mask, int width_mask, int height_mask);

_XI_APIREF_ XI_ERR_TYPE xiMedianBlur_3x3_U8_ref (const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMedianBlur_3x3_S16_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMedianBlur_5x5_U8_ref (const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMedianBlurPatternCross_3x3_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMedianBlurPatternCross_5x5_U8_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMedianBlurPatternCross_7x7_U8_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMedianBlurPatternCross_9x9_U8_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMedianBlurPatternDisk_5x5_U8_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMedianBlur_3x3_U8Oa2_ref (const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMedianBlur_3x3_S16Oa_ref(const xi_pTile src, xi_pArray dst);

/* bilateral filter */
_XI_APIREF_ XI_ERR_TYPE xiBilateralFilter_5x5_U8_ref    (const xi_pTile src, const xi_pTile dst, float color_sigma);
_XI_APIREF_ XI_ERR_TYPE xiBilateralFilter_5x5_U8_fpt_ref(const xi_pTile src, const xi_pTile dst, float color_sigma);

_XI_APIREF_ XI_ERR_TYPE xiBilateralFilter_5x5_S16_ref    (const xi_pTile src, const xi_pTile dst, float color_sigma);
_XI_APIREF_ XI_ERR_TYPE xiBilateralFilter_5x5_S16_fpt_ref(const xi_pTile src, const xi_pTile dst, float color_sigma);

_XI_APIREF_ XI_ERR_TYPE xiSepBilateralFilter_5x5_U8_fpt_ref(const xi_pTile src, const xi_pTile dst, const xi_pTile tmp, float color_sigma);

_XI_APIREF_ XI_ERR_TYPE xiBilateralFilter_9x9_U8_ref (const xi_pTile src, const xi_pTile dst, float color_sigma);
_XI_APIREF_ XI_ERR_TYPE xiBilateralFilter_9x9_S16_ref(const xi_pTile src, const xi_pTile dst, float color_sigma);

#ifdef XI_ENABLE_BIT_EXACT_CREF
_XI_APIREF_ XI_ERR_TYPE xiBilateralFilter_5x5_U8_lut64_ref (const xi_pTile src, const xi_pTile dst, const XI_Q5_10* range_lut);
_XI_APIREF_ XI_ERR_TYPE xiBilateralFilter_5x5_S16_lut64_ref(const xi_pTile src, const xi_pTile dst, const XI_Q5_10* range_lut);
_XI_APIREF_ XI_ERR_TYPE xiBilateralFilter_9x9_U8_lut64_ref (const xi_pTile src, const xi_pTile dst, const XI_Q5_10* range_lut);
_XI_APIREF_ XI_ERR_TYPE xiBilateralFilter_9x9_S16_lut64_ref(const xi_pTile src, const xi_pTile dst, const XI_Q5_10* range_lut);
#endif

/* Blur image and downsample it */
_XI_APIREF_ XI_ERR_TYPE xiPyrDown_3x3_U8_ref (const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiPyrDown_U8_ref     (const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiPyrDown_R_U8_ref   (const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiPyrDown_S16_ref    (const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiPyrDown_3x3_S16_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiPyrDown_3x3_U8Oa_ref (const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiPyrDown_U8Oa_ref (const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiPyrDown_3x3_S16Oa_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiPyrDown_S16Oa_ref(const xi_pTile src, xi_pArray dst);


/* Laplacian pyramid construction step for OpenVX */
_XI_APIREF_ XI_ERR_TYPE xiVXLaplacianPyrDown_U8_ref(const xi_pTile src, xi_pArray dst, xi_pArray dst_next);
_XI_APIREF_ XI_ERR_TYPE xiVXLaplacianPyrDown_U8Oa2_ref(const xi_pTile src, xi_pArray dst, xi_pArray dst_next);
/* Laplacian pyramid reconstruction step for OpenVX */
_XI_APIREF_ XI_ERR_TYPE xiVXLaplacianReconstruct1_S16_ref(const xi_pArray src, const xi_pArray laplacian, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiVXLaplacianReconstruct2_S16_ref(const xi_pArray src, const xi_pArray laplacian, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiVXLaplacianReconstruct2_S16A_ref(const xi_pArray src, const xi_pArray laplacian, xi_pArray dst);

/* Upsample image and blur it */
_XI_APIREF_ XI_ERR_TYPE xiPyrUp_U8_ref (const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiPyrUp_S16_ref(const xi_pTile src, xi_pArray dst);


/* Generates distribution from an image */
_XI_APIREF_ XI_ERR_TYPE xiHistogram_U8U16_ref(const xi_pArray src, uint16_t* histogram);
_XI_APIREF_ XI_ERR_TYPE xiHistogram_U8U32_ref(const xi_pArray src, uint32_t* histogram);


/* Converts histogram into lookup with equalization */
_XI_APIREF_ XI_ERR_TYPE xiEqualizeHist_U16U8_ref(const uint16_t* histogram, uint8_t* lut);
_XI_APIREF_ XI_ERR_TYPE xiEqualizeHist_U32U8_ref(const uint32_t* histogram, uint8_t* lut);


/* Polar coordinates into Cartesian; x = magnitude * cos(angle), y = magnitude * sin(angle) */
_XI_APIREF_ XI_ERR_TYPE xiPolarToCart_S16_ref    (const xi_pArray magnitude, const xi_pArray angle, xi_pArray x, xi_pArray y);
_XI_APIREF_ XI_ERR_TYPE xiPolarToCart_S_S16U8_ref(const xi_pArray magnitude, const xi_pArray angle, xi_pArray x, xi_pArray y);
_XI_APIREF_ XI_ERR_TYPE xiPolarToCart_S16A_ref    (const xi_pArray magnitude, const xi_pArray angle, xi_pArray x, xi_pArray y);
_XI_APIREF_ XI_ERR_TYPE xiPolarToCart_S_S16U8A_ref(const xi_pArray magnitude, const xi_pArray angle, xi_pArray x, xi_pArray y);


/* Cartesian coordinates into polar; angle = atan(y/x), magnitude = sqrt(x^2 + y^2) */
_XI_APIREF_ XI_ERR_TYPE xiCartToPolar_U8S16_ref(const xi_pArray x, const xi_pArray y, xi_pArray magnitude, xi_pArray angle);
_XI_APIREF_ XI_ERR_TYPE xiCartToPolar_U8S16_2_ref(const xi_pArray x, const xi_pArray y, xi_pArray magnitude, xi_pArray angle, const xi_pArray atan_lut);
_XI_APIREF_ XI_ERR_TYPE xiCartToPolar_U8S16A_2_ref(const xi_pArray x, const xi_pArray y, xi_pArray magnitude, xi_pArray angle, const xi_pArray atan_lut);


/* BRIEF descriptor */
_XI_APIREF_ XI_ERR_TYPE xiBRIEF16_48x48_S16U8_ref(const xi_pTile src, xi_point keypoint, uint8_t* descriptor);
_XI_APIREF_ XI_ERR_TYPE xiBRIEF16_48x48_U16U8_ref(const xi_pTile src, xi_point keypoint, uint8_t* descriptor);
_XI_APIREF_ XI_ERR_TYPE xiBRIEF32_48x48_S16U8_ref(const xi_pTile src, xi_point keypoint, uint8_t* descriptor);
_XI_APIREF_ XI_ERR_TYPE xiBRIEF32_48x48_U16U8_ref(const xi_pTile src, xi_point keypoint, uint8_t* descriptor);
_XI_APIREF_ XI_ERR_TYPE xiBRIEF64_48x48_S16U8_ref(const xi_pTile src, xi_point keypoint, uint8_t* descriptor);
_XI_APIREF_ XI_ERR_TYPE xiBRIEF64_48x48_U16U8_ref(const xi_pTile src, xi_point keypoint, uint8_t* descriptor);
_XI_APIREF_ XI_ERR_TYPE xiBRIEF16_48x48_S16U8_3_ref(const xi_pTile src, xi_point keypoint, uint8_t* descriptor,const xi_pArray pattern);
_XI_APIREF_ XI_ERR_TYPE xiBRIEF16_48x48_U16U8_3_ref(const xi_pTile src, xi_point keypoint, uint8_t* descriptor,const xi_pArray pattern);
_XI_APIREF_ XI_ERR_TYPE xiBRIEF32_48x48_S16U8_3_ref(const xi_pTile src, xi_point keypoint, uint8_t* descriptor,const xi_pArray pattern);
_XI_APIREF_ XI_ERR_TYPE xiBRIEF32_48x48_U16U8_3_ref(const xi_pTile src, xi_point keypoint, uint8_t* descriptor,const xi_pArray pattern);
_XI_APIREF_ XI_ERR_TYPE xiBRIEF64_48x48_S16U8_3_ref(const xi_pTile src, xi_point keypoint, uint8_t* descriptor,const xi_pArray pattern);
_XI_APIREF_ XI_ERR_TYPE xiBRIEF64_48x48_U16U8_3_ref(const xi_pTile src, xi_point keypoint, uint8_t* descriptor,const xi_pArray pattern);

/* Color space conversions */
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_YUV2RGB_422_UYVY_BT709_ref (const xi_pArray uyvy, xi_pArray rgb);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_YUV2RGB_422_YUYV_BT709_ref (const xi_pArray yuyv, xi_pArray rgb);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_YUV2RGBX_422_UYVY_BT709_ref(const xi_pArray uyvy, xi_pArray rgbx);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_YUV2RGBX_422_YUYV_BT709_ref(const xi_pArray yuyv, xi_pArray rgbx);

_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_RGB2Gray_ref(const xi_pArray r, const xi_pArray g, const xi_pArray b, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_S16_RGB2Gray_ref(const xi_pArray r, const xi_pArray g, const xi_pArray b, xi_pArray dst);

_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_RGBX2YUV_420_BT709_ref     (const xi_pArray rgbx, xi_pArray y, xi_pArray u, xi_pArray v);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_RGBX2YUV_420_NV12_BT709_ref(const xi_pArray rgbx, xi_pArray y, xi_pArray uv);

_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_RGB2YUV_420_BT709_ref      (const xi_pArray rgb, xi_pArray y, xi_pArray u, xi_pArray v);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_RGB2YUV_420_NV12_BT709_ref (const xi_pArray rgb, xi_pArray y, xi_pArray uv);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_RGB2YUV_420_NV21_BT709_ref (const xi_pArray rgb, xi_pArray y, xi_pArray uv);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_YUV2RGB_420_BT709_ref      (const xi_pArray y, const xi_pArray u, const xi_pArray v, xi_pArray rgb);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_YUV2RGB_420_NV12_BT709_ref (const xi_pArray y, const xi_pArray uv, xi_pArray rgb);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_YUV2RGB_420_NV21_BT709_ref (const xi_pArray y, const xi_pArray uv, xi_pArray rgb);

_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_YUV2RGBX_420_BT709_ref     (const xi_pArray y, const xi_pArray u, const xi_pArray v, xi_pArray rgbx);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_YUV2RGBX_420_NV12_BT709_ref(const xi_pArray y, const xi_pArray uv, xi_pArray rgbx);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_YUV2RGBX_420_NV21_BT709_ref(const xi_pArray y, const xi_pArray uv, xi_pArray rgbx);

_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A2_YUV2RGB_422_UYVY_BT709_ref (const xi_pArray uyvy, xi_pArray rgb);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A2_YUV2RGB_422_YUYV_BT709_ref (const xi_pArray yuyv, xi_pArray rgb);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A2_YUV2RGBX_422_UYVY_BT709_ref(const xi_pArray uyvy, xi_pArray rgbx);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A2_YUV2RGBX_422_YUYV_BT709_ref(const xi_pArray yuyv, xi_pArray rgbx);

_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A2_RGB2Gray_ref(const xi_pArray r, const xi_pArray g, const xi_pArray b, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_S16A_RGB2Gray_ref(const xi_pArray r, const xi_pArray g, const xi_pArray b, xi_pArray dst);

_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A2_RGBX2YUV_420_BT709_ref     (const xi_pArray rgbx, xi_pArray y, xi_pArray u, xi_pArray v);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A2_RGBX2YUV_420_NV12_BT709_ref(const xi_pArray rgbx, xi_pArray y, xi_pArray uv);

_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A2_RGB2YUV_420_BT709_ref      (const xi_pArray rgb, xi_pArray y, xi_pArray u, xi_pArray v);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A2_RGB2YUV_420_NV12_BT709_ref (const xi_pArray rgb, xi_pArray y, xi_pArray uv);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A2_RGB2YUV_420_NV21_BT709_ref (const xi_pArray rgb, xi_pArray y, xi_pArray uv);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A2_YUV2RGB_420_BT709_ref      (const xi_pArray y, const xi_pArray u, const xi_pArray v, xi_pArray rgb);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A2_YUV2RGB_420_NV12_BT709_ref (const xi_pArray y, const xi_pArray uv, xi_pArray rgb);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A2_YUV2RGB_420_NV21_BT709_ref (const xi_pArray y, const xi_pArray uv, xi_pArray rgb);

_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A2_YUV2RGBX_420_BT709_ref     (const xi_pArray y, const xi_pArray u, const xi_pArray v, xi_pArray rgbx);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A2_YUV2RGBX_420_NV12_BT709_ref(const xi_pArray y, const xi_pArray uv, xi_pArray rgbx);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A2_YUV2RGBX_420_NV21_BT709_ref(const xi_pArray y, const xi_pArray uv, xi_pArray rgbx);


_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_YUYV_NV12_ref(const xi_pArray yuyv, xi_pArray y, xi_pArray uv);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_UYVY_NV12_ref(const xi_pArray uyvy, xi_pArray y, xi_pArray uv);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A2_YUYV_NV12_ref(const xi_pArray yuyv, xi_pArray y, xi_pArray uv);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A2_UYVY_NV12_ref(const xi_pArray uyvy, xi_pArray y, xi_pArray uv);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_YUYV_IYUV_ref(const xi_pArray yuyv, xi_pArray y, xi_pArray u, xi_pArray v);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_UYVY_IYUV_ref(const xi_pArray uyvy, xi_pArray y, xi_pArray u, xi_pArray v);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A2_YUYV_IYUV_ref(const xi_pArray yuyv, xi_pArray y, xi_pArray u, xi_pArray v);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A2_UYVY_IYUV_ref(const xi_pArray uyvy, xi_pArray y, xi_pArray u, xi_pArray v);

_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_RGB2YCrCb_ref (const xi_pArray r, const xi_pArray g, const xi_pArray b, xi_pArray y, xi_pArray cr, xi_pArray cb);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_S16_RGB2YCrCb_ref(const xi_pArray r, const xi_pArray g, const xi_pArray b, xi_pArray y, xi_pArray cr, xi_pArray cb);

_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_YCrCb2RGB_ref (const xi_pArray y, const xi_pArray cr, const xi_pArray cb, xi_pArray r, xi_pArray g, xi_pArray b);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_S16_YCrCb2RGB_ref(const xi_pArray y, const xi_pArray cr, const xi_pArray cb, xi_pArray r, xi_pArray g, xi_pArray b);

_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_RGB2HSV_ref (const xi_pArray r, const xi_pArray g, const xi_pArray b, xi_pArray h, xi_pArray s, xi_pArray v);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_S16_RGB2HSV_ref(const xi_pArray r, const xi_pArray g, const xi_pArray b, xi_pArray h, xi_pArray s, xi_pArray v);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A_RGB2HSV_ref (const xi_pArray r, const xi_pArray g, const xi_pArray b, xi_pArray h, xi_pArray s, xi_pArray v);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_S16A_RGB2HSV_ref(const xi_pArray r, const xi_pArray g, const xi_pArray b, xi_pArray h, xi_pArray s, xi_pArray v);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A2_RGB2YCrCb_ref(const xi_pArray r, const xi_pArray g, const xi_pArray b, xi_pArray y, xi_pArray cr, xi_pArray cb);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_S16A_RGB2YCrCb_ref(const xi_pArray r, const xi_pArray g, const xi_pArray b, xi_pArray y, xi_pArray cr, xi_pArray cb);

_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A_YCrCb2RGB_ref (const xi_pArray y, const xi_pArray cr, const xi_pArray cb, xi_pArray r, xi_pArray g, xi_pArray b);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_S16A_YCrCb2RGB_ref(const xi_pArray y, const xi_pArray cr, const xi_pArray cb, xi_pArray r, xi_pArray g, xi_pArray b);


_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_RGBX2RGB_ref(const xi_pArray rgbx, xi_pArray rgb);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_RGB2RGBX_ref(const xi_pArray rgb, xi_pArray rgbx);

_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A2_RGBX2RGB_ref(const xi_pArray rgbx, xi_pArray rgb);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A2_RGB2RGBX_ref(const xi_pArray rgb, xi_pArray rgbx);

_XI_APIREF_ XI_ERR_TYPE xiUpsample2XNearest_I8_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiUpsample2XNearest_I16_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiUpsample2XNearest_I8A2_ref (const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiUpsample2XNearest_I16Oa_ref (const xi_pArray src, xi_pArray dst);

_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_RGB2YUV_BT709_ref    (const xi_pArray rgb, xi_pArray y, xi_pArray u, xi_pArray v);

_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_YUV2RGB_BT709_ref(const xi_pTile y, const xi_pTile u, const xi_pTile v, xi_pTile rgb);

_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_RGBX2YUV_BT709_ref(const xi_pArray rgbx, xi_pArray y, xi_pArray u, xi_pArray v);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A2_RGB2YUV_BT709_ref (const xi_pArray rgb, xi_pArray y, xi_pArray u, xi_pArray v);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8A2_RGBX2YUV_BT709_ref(const xi_pArray rgbx, xi_pArray y, xi_pArray u, xi_pArray v);


_XI_APIREF_ XI_ERR_TYPE xiCvtColor_U8_YUV2RGB_BT709_ref (const xi_pTile y, const xi_pTile u, const xi_pTile v, xi_pTile rgb);

_XI_APIREF_ XI_ERR_TYPE xiCvtColor_UnpackUV_U8_ref (const xi_pArray uv, xi_pArray u, xi_pArray v);
_XI_APIREF_ XI_ERR_TYPE xiCvtColor_UnpackUV_U8A2_ref (const xi_pArray uv, xi_pArray u, xi_pArray v);

/* NCC */
_XI_APIREF_ XI_ERR_TYPE xiNCC_U8Q16_ref(const xi_pArray src, const xi_pArray templ, xi_pArray dst);

/* ZNCC*/
_XI_APIREF_ XI_ERR_TYPE xiCompareCcoeffNorm_U8Q15_ref (const xi_pTile src, const xi_pTile templ, xi_pArray temptempl, xi_pArray tempsrc, xi_pTile dst);

/* Hamming */
_XI_APIREF_ XI_ERR_TYPE xiCompareHamming_U8_ref (const xi_pTile src, const xi_pTile templ, xi_pTile dst);

/* L1 */
_XI_APIREF_ XI_ERR_TYPE xiCompareL1_U8_ref (const xi_pTile src, const xi_pTile templ, xi_pTile dst);

/* L2 */
_XI_APIREF_ XI_ERR_TYPE xiCompareL2_U8_ref (const xi_pTile src, const xi_pTile templ, xi_pTile dst);

/* Cross correlation */
_XI_APIREF_ XI_ERR_TYPE xiCompareCcorr_U8_ref (const xi_pTile src, const xi_pTile templ, xi_pTile dst);

/* L2 Norm */
_XI_APIREF_ XI_ERR_TYPE xiCompareL2Norm_U8Q16_ref (const xi_pTile src, const xi_pTile templ, xi_pTile dst);

/* Cross correlation Norm */
_XI_APIREF_ XI_ERR_TYPE xiCompareCcorrNorm_U8Q16_ref (const xi_pTile src, const xi_pTile templ, xi_pTile dst);


/* SQDiff */
_XI_APIREF_ XI_ERR_TYPE xiSQDiff_U8Q16_ref(const xi_pTile src, const xi_pTile templ, xi_pTile dst);


/* LK optical flow point tracking */
_XI_APIREF_ XI_ERR_TYPE xiOpticalFlowLK_TrackPoint_U8_ref(xi_pTile prev, xi_pTile next, xi_pTile prev_deriv_x, xi_pTile prev_deriv_y,
                                              xi_size win_size,
                                              XI_Q13_18 fpt_min_eig_threshold,
                                              XI_Q1_30 fpt_criteria_epsilon, int criteria_max_count,
                                              xi_point_fpt prevpt, xi_point_fpt* nextpt, xi_bool* status);

_XI_APIREF_ XI_ERR_TYPE xiOpticalFlowLK_TrackPoint_S16_ref(xi_pTile prev, xi_pTile next, xi_pTile prev_deriv_x, xi_pTile prev_deriv_y,
                                              xi_size win_size,
                                              XI_Q13_18 fpt_min_eig_threshold,
                                              XI_Q1_30 fpt_criteria_epsilon, int criteria_max_count,
                                              xi_point_fpt prevpt, xi_point_fpt* nextpt, xi_bool* status);


/* Block matching optical flow */
_XI_APIREF_ XI_ERR_TYPE xiCalcOpticalFlowBM_U8S16_ref(const xi_pTile prev, const xi_pTile curr, xi_size blockSize, xi_size shiftSize,
                                                   xi_size maxRange, xi_pTile velx, xi_pTile vely);

_XI_APIREF_ XI_ERR_TYPE xiCalcOpticalFlowBM_S16_ref(const xi_pTile prev, const xi_pTile curr, xi_size blockSize, xi_size shiftSize,
                                                 xi_size maxRange, xi_pTile velx, xi_pTile vely);


/* Farneback optical flow */
_XI_APIREF_ XI_ERR_TYPE xiCalcOpticalFlowFarnebackPolyExp_Q8_7F32_ref(const xi_pTile src, xi_pTile dst, xi_pArray tmp, int ksize, double sigma);
_XI_APIREF_ XI_ERR_TYPE xiTakeNextSizesFromOpticalFlow_F32_ref(const xi_pTile _flow, const xi_size framesize, xi_size* nextsiz, xi_point* nextXY);
_XI_APIREF_ XI_ERR_TYPE xiOpticalFlowFarnebackCreateM_F32_ref(const xi_pTile _R0,const xi_pTile _R1,const xi_pTile _flow, xi_pTile matM);
_XI_APIREF_ XI_ERR_TYPE xiCalcOpticalFlowFarneback_F32_ref(const xi_pTile matM, xi_pTile _flow, xi_pArray tmp, int ksize);

_XI_APIREF_ XI_ERR_TYPE xiCalcOpticalFlowFarnebackPolyExp_Q8_7S16_ref(const xi_pTile src, xi_pTile dst, xi_pArray tmp, int ksize, double sigma);
_XI_APIREF_ XI_ERR_TYPE xiTakeNextSizesFromOpticalFlow_S32_ref(const xi_pTile _flow, const xi_size framesize, xi_size* nextsiz, xi_point* nextXY);
_XI_APIREF_ XI_ERR_TYPE xiOpticalFlowFarnebackCreateM_S16S32_ref(const xi_pTile _R0,const xi_pTile _R1,const xi_pTile _flow, xi_pTile matM);
_XI_APIREF_ XI_ERR_TYPE xiCalcOpticalFlowFarneback_S32_ref(const xi_pTile matM, xi_pTile _flow, xi_pArray tmp, int ksize);


/* Harris corner detector */
#ifdef XI_ENABLE_BIT_EXACT_CREF
_XI_APIREF_ XI_ERR_TYPE xiCornerHarris_U8S16_ref(const xi_pTile src, xi_pArray dst, xi_pArray tmp, int blockSize, int ksize, XI_Q15 k_sqrt);
_XI_APIREF_ XI_ERR_TYPE xiCornerHarris_3x3_3x3_U8S16_ref(const xi_pTile src, xi_pArray dst, xi_pArray tmp, XI_Q15 k_sqrt);
_XI_APIREF_ XI_ERR_TYPE xiCornerHarris_3x3_5x5_U8S16_ref(const xi_pTile src, xi_pArray dst, xi_pArray tmp, XI_Q15 k_sqrt);
_XI_APIREF_ XI_ERR_TYPE xiCornerHarris_S16_ref(const xi_pTile src, xi_pArray dst, xi_pArray tmp, int blockSize, int ksize, XI_Q15 k_sqrt);
_XI_APIREF_ XI_ERR_TYPE xiCornerHarris_3x3_3x3_S16_ref(const xi_pTile src, xi_pArray dst, xi_pArray tmp, XI_Q15 k_sqrt);
_XI_APIREF_ XI_ERR_TYPE xiCornerHarris2_U8S16_ref(const xi_pTile src, xi_pArray dst, xi_pArray tmp, int blockSize, int ksize, XI_Q15 k_sqrt);
#else
_XI_APIREF_ XI_ERR_TYPE xiCornerHarris_U8S16_ref(const xi_pTile src, xi_pArray dst, xi_pArray tmp, int blockSize, int ksize, float k, int lsht);
_XI_APIREF_ XI_ERR_TYPE xiCornerHarris_S16_ref  (const xi_pTile src, xi_pArray dst, xi_pArray tmp, int blockSize, int ksize, float k, int lsht);
#endif
_XI_APIREF_ XI_ERR_TYPE xiCornerHarris_U8F32_ref(const xi_pTile src, xi_pTile dst,  xi_pTile tmp, int blockSize, int ksize, float k);

_XI_APIREF_ XI_ERR_TYPE xiCornerHarrisNonMaxima_ref(const xi_pArray src_p, const xi_pArray src_v, int min_dist);


/* Shi-Tomasi corner detector */
_XI_APIREF_ XI_ERR_TYPE xiCornerShiTomasi_U8S16_ref(const xi_pTile src, xi_pArray dst, xi_pArray tmp, int blockSize, int ksize);
_XI_APIREF_ XI_ERR_TYPE xiCornerShiTomasi_S16_ref  (const xi_pTile src, xi_pArray dst, xi_pArray tmp, int blockSize, int ksize);
_XI_APIREF_ XI_ERR_TYPE xiCornerShiTomasi_3x3_3x3_U8S16_ref(const xi_pTile src, xi_pArray dst, xi_pArray tmp);
_XI_APIREF_ XI_ERR_TYPE xiCornerShiTomasi_3x3_3x3_S16_ref(const xi_pTile src, xi_pArray dst, xi_pArray tmp);



/* FAST corner detector */
_XI_APIREF_ XI_ERR_TYPE xiFAST_U8_ref    (const xi_pTile src, xi_pArray score, int threshold);
_XI_APIREF_ XI_ERR_TYPE xiFAST_S16U16_ref(const xi_pTile src, xi_pArray score, int threshold);
_XI_APIREF_ XI_ERR_TYPE xiFAST2_U8_ref    (const xi_pTile src, xi_pArray score, int threshold, xi_pArray tmp);
_XI_APIREF_ XI_ERR_TYPE xiFAST2_S16U16_ref(const xi_pTile src, xi_pArray score, int threshold, xi_pArray tmp);
_XI_APIREF_ XI_ERR_TYPE xiFAST_U8MSOa2_ref    (const xi_pTile src, xi_pArray score, int threshold);
_XI_APIREF_ XI_ERR_TYPE xiFAST_S16U16MSOa_ref(const xi_pTile src, xi_pArray score, int threshold);

/* Census transform */
_XI_APIREF_ XI_ERR_TYPE xiCensusTransform_U8_ref   (const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiCensusTransform_U8Oa2_ref   (const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiCensusTransform_S16U8_ref(const xi_pTile src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiCensusTransform_S16U8Oa_ref(const xi_pTile src, xi_pArray dst);
/* Revised modified census transform */
_XI_APIREF_ XI_ERR_TYPE xiRMCT_U8U16_ref (const xi_pTile src, xi_pTile dst, int r);
_XI_APIREF_ XI_ERR_TYPE xiRMCT_S16U16_ref(const xi_pTile src, xi_pTile dst, int r);
_XI_APIREF_ XI_ERR_TYPE xiRMCT_U8U16Oa_ref (const xi_pTile src, xi_pTile dst, int r);
_XI_APIREF_ XI_ERR_TYPE xiRMCT_S16U16Oa_ref(const xi_pTile src, xi_pTile dst, int r);

/* Modified census transform */
#define xiMCT_U8U16_ref(src, dst) xiRMCT_U8U16_ref(src, dst, 0)
#define xiMCT_U8U16Oa_ref(src, dst) xiRMCT_U8U16Oa_ref(src, dst, 0)
#define xiMCT_S16U16_ref(src, dst) xiRMCT_S16U16_ref(src, dst, 0)
#define xiMCT_S16U16Oa_ref(src, dst) xiRMCT_S16U16Oa_ref(src, dst, 0)


/* Perspective transform */
_XI_APIREF_ XI_ERR_TYPE xiWarpPerspective_U8_Q13_18_ref (const xi_pTile src, xi_pTile dst, const xi_perspective_fpt* perspective);
_XI_APIREF_ XI_ERR_TYPE xiWarpPerspective_S16_Q13_18_ref(const xi_pTile src, xi_pTile dst, const xi_perspective_fpt* perspective);
_XI_APIREF_ XI_ERR_TYPE xiWarpPerspective_U8Oa_Q13_18_ref (const xi_pTile src, xi_pTile dst, const xi_perspective_fpt* perspective);
_XI_APIREF_ XI_ERR_TYPE xiWarpPerspective_S16Oa_Q13_18_ref(const xi_pTile src, xi_pTile dst, const xi_perspective_fpt* perspective);

_XI_APIREF_ XI_ERR_TYPE xiWarpPerspectiveNearest_U8_Q13_18_ref (const xi_pTile src, xi_pTile dst, const xi_perspective_fpt* perspective);
_XI_APIREF_ XI_ERR_TYPE xiWarpPerspectiveNearest_S16_Q13_18_ref(const xi_pTile src, xi_pTile dst, const xi_perspective_fpt* perspective);
_XI_APIREF_ XI_ERR_TYPE xiWarpPerspectiveNearest_I8_Q13_18_ref (const xi_pTile src, xi_pTile dst, const xi_perspective_fpt* perspective);
_XI_APIREF_ XI_ERR_TYPE xiWarpPerspectiveNearest_I16_Q13_18_ref(const xi_pTile src, xi_pTile dst, const xi_perspective_fpt* perspective);

/* Background substractor (mixture of gaussians) */

// element of the model of each pixel
typedef struct
{
    float sortKey;
    float weight;
    float mean;
    float var;
} GaussDistrOCV;

_XI_APIREF_ XI_ERR_TYPE xiBS_MOG_U8_ref(const xi_pArray src, xi_pArray fgmask,
                                     XI_Q0_15 learningRate,
                                     xi_pArray meanM, xi_pArray weightM, xi_pArray varM);
_XI_APIREF_ XI_ERR_TYPE xiBS_MOG_U8A_ref(const xi_pArray src, xi_pArray fgmask,
                                     XI_Q0_15 learningRate,
                                     xi_pArray meanM, xi_pArray weightM, xi_pArray varM);
_XI_APIREF_ XI_ERR_TYPE xiBS_MOG_U8_OCV_ref(const xi_pArray src, xi_pArray fgmask,
                                         XI_Q0_15 learningRate,
                                         GaussDistrOCV * GDistr);


/* k-means clustering */
_XI_APIREF_ XI_ERR_TYPE xiKmeans_GenerateCentersPP_S16_L2_ref(const xi_pArray data, xi_pArray dst, xi_pArray tmp, int trials, int seed);
_XI_APIREF_ XI_ERR_TYPE xiKmeans_S16_L2_ref(const xi_pArray data, xi_pArray labels, xi_pArray centers, uint64_t* _compactness, int maxCount, XI_Q1_30 epsilon);
_XI_APIREF_ XI_ERR_TYPE xiKmeans_S16A_L2_ref(const xi_pArray data, xi_pArray labels, xi_pArray centers, uint64_t* _compactness, int maxCount, XI_Q1_30 epsilon);

/* Connected components labeling */
_XI_APIREF_ XI_ERR_TYPE xiConnectedComponents4_U8U16_ref(const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiConnectedComponents4_I8U16_ref(const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiConnectedComponents4_I8U16A_ref(const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiConnectedComponents8_U8U16_ref(const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiConnectedComponents8_I8U16_ref(const xi_pArray src, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiConnectedComponents8_I8U16A_ref(const xi_pArray src, xi_pArray dst);


/* Logic left shift */
_XI_APIREF_ XI_ERR_TYPE xiShiftLeft_U32_ref(const xi_pTile src, xi_pTile dst, int lsht);
_XI_APIREF_ XI_ERR_TYPE xiShiftLeft_I32_ref(const xi_pTile src, xi_pTile dst, int lsht);
_XI_APIREF_ XI_ERR_TYPE xiShiftLeft_I32A_ref(const xi_pTile src, xi_pTile dst, int lsht);
/* Maximally stable extremal region extraction */
_XI_APIREF_ XI_ERR_TYPE xiMSER_U8_ref(const xi_pTile src, xi_pTile dst, xi_pTile tmp, const xi_mser_params* params);


/* Ellipse fitting */
_XI_APIREF_ XI_ERR_TYPE xiFitEllipse_U8_ref(const xi_pArray ptx, const xi_pArray pty, xi_rotated_rect_f* ellipse, int mode);


/* Reduces number of bins in distribution */
_XI_APIREF_ XI_ERR_TYPE xiShrinkHist_U32_ref(const xi_pArray hist_in, xi_pArray hist_out, int offset, int range);


/* Rectangle */
_XI_APIREF_ XI_ERR_TYPE xiRectangle_U8_ref (xi_pTile src, xi_point32 p0, xi_point32 p1, int color, int thickness);
_XI_APIREF_ XI_ERR_TYPE xiRectangle_S16_ref(xi_pTile src, xi_point32 p0, xi_point32 p1, int color, int thickness);
_XI_APIREF_ XI_ERR_TYPE xiRectangle_I16_ref (xi_pTile src, xi_point32 p0, xi_point32 p1, int color, int thickness);


/* Computes generic geometrical transformation */
_XI_APIREF_ XI_ERR_TYPE xiRemapBilinear_U8_ref(const xi_pTile src, xi_pArray dst, const xi_pArray remap_int, const xi_pArray remap_frac);
_XI_APIREF_ XI_ERR_TYPE xiRemapBilinear_S16_ref(const xi_pTile src, xi_pArray dst, const xi_pArray remap_int, const xi_pArray remap_frac);
_XI_APIREF_ XI_ERR_TYPE xiRemapNearest_U8_ref(const xi_pTile src, xi_pArray dst, const xi_pArray remap_int);
_XI_APIREF_ XI_ERR_TYPE xiRemapNearest_S16_ref(const xi_pTile src, xi_pArray dst, const xi_pArray remap_int);
_XI_APIREF_ XI_ERR_TYPE xiRemapNearest_I8_ref(const xi_pTile src, xi_pArray dst, const xi_pArray remap_int);
_XI_APIREF_ XI_ERR_TYPE xiRemapNearest_I16_ref(const xi_pTile src, xi_pArray dst, const xi_pArray remap_int);


/* Applies the standard Hough transform to an image tile */
_XI_APIREF_ XI_ERR_TYPE xiHoughLinesTransform_U16_ref(const xi_pTile src, xi_pTile dst, xi_size size_image, xi_size size_map);
_XI_APIREF_ XI_ERR_TYPE xiHoughLinesExtract_U16_ref(xi_size size_image, xi_size size_map, const xi_pTile responce_map, uint16_t threshold, xi_line_polar_fpt* lines, int max_lines, int* count);
_XI_APIREF_ XI_ERR_TYPE xiHoughLinesExtractNext_U16_ref(xi_size size_image, xi_point start_loc, xi_size size_map, const xi_pTile responce_map, uint16_t threshold, xi_line_polar_fpt* lines, int max_lines, int* count);


/* Adaptive threshold */
_XI_APIREF_ XI_ERR_TYPE xiAdaptiveThreshold_U8_ref(const xi_pTile src, xi_pArray dst, xi_pArray tmp, int ksize, int maxval XI_DEFAULT(255), XI_Q15_16 c XI_DEFAULT(0));
_XI_APIREF_ XI_ERR_TYPE xiAdaptiveThreshold_S16U8_ref(const xi_pTile src, xi_pArray dst, xi_pArray tmp, int ksize, int maxval XI_DEFAULT(255), int c XI_DEFAULT(0));
_XI_APIREF_ XI_ERR_TYPE xiAdaptiveThreshold_5x5_S16U8_ref(const xi_pTile src, xi_pArray dst, int maxval, int c);
_XI_APIREF_ XI_ERR_TYPE xiAdaptiveThreshold_3x3_S16U8_ref(const xi_pTile src, xi_pArray dst, int maxval, int c);
_XI_APIREF_ XI_ERR_TYPE xiAdaptiveThreshold_3x3_U8_ref(const xi_pTile src, xi_pArray dst, int maxval, XI_Q15_16 c);
_XI_APIREF_ XI_ERR_TYPE xiAdaptiveThreshold_5x5_U8_ref(const xi_pTile src, xi_pArray dst, int maxval, XI_Q15_16 c);

/* Line driving functions */
_XI_APIREF_ XI_ERR_TYPE xiLine_U8_ref (xi_pTile src, xi_point32 p1, xi_point32 p2, int color, int thickness);
_XI_APIREF_ XI_ERR_TYPE xiLine_S16_ref(xi_pTile src, xi_point32 p1, xi_point32 p2, int color, int thickness);
_XI_APIREF_ XI_ERR_TYPE xiLine_I8_ref (xi_pTile src, xi_point32 p1, xi_point32 p2, int color, int thickness);
_XI_APIREF_ XI_ERR_TYPE xiLine_I16_ref(xi_pTile src, xi_point32 p1, xi_point32 p2, int color, int thickness);


/* Circle driving functions */
_XI_APIREF_ XI_ERR_TYPE xiCircle_U8_ref (xi_pTile src, xi_point32 c, int r, int color, int thickness);
_XI_APIREF_ XI_ERR_TYPE xiCircle_S16_ref(xi_pTile src, xi_point32 c, int r, int color, int thickness);


/* Hough circles transform */
_XI_APIREF_ XI_ERR_TYPE xiHoughCirclesTransform_U8U16_ref(const xi_pTile src_sobel, const xi_pTile src_dx, const xi_pTile src_dy, xi_pTile dst, int min_radius, int max_radius);
_XI_APIREF_ XI_ERR_TYPE xiHoughCirclesFindRadius_ref(const xi_point* edge, xi_point center, int* radius, int threshold, int min_radius, int max_radius, int count_edge);
_XI_APIREF_ XI_ERR_TYPE xiHoughCirclesGetEdgePoints_U8S16S16_ref(const xi_pTile src_canny, const xi_pTile src_sobel_dx, const xi_pTile src_sobel_dy, xi_point* edge, xi_point* gradient, int max_edgepoints, int* count);
_XI_APIREF_ XI_ERR_TYPE xiHoughCirclesGetNextEdgePoints_U8S16S16_ref(const xi_pTile src_canny, const xi_pTile src_sobel_dx, const xi_pTile src_sobel_dy, xi_point start_loc, xi_point* edge, xi_point* gradient, int max_edgepoints, int* count);

#ifdef XI_ENABLE_BIT_EXACT_CREF
_XI_APIREF_ XI_ERR_TYPE xiHoughCirclesTransform_U8U16_ref_v1(const xi_pTile src_sobel, const xi_pTile src_dx, const xi_pTile src_dy, xi_pTile dst, int min_radius, int max_radius);
_XI_APIREF_ XI_ERR_TYPE xiHoughCirclesTransform_U8U16_ref_v2(const xi_pTile src_sobel, const xi_pTile src_dx, const xi_pTile src_dy, xi_pTile dst, int min_radius, int max_radius);
#endif

/* Sorting */
_XI_APIREF_ XI_ERR_TYPE xiSort_U8_ref(const xi_pArray keys, const xi_pArray values, xi_pArray keys_out, xi_pArray values_out, xi_bool ascending);
_XI_APIREF_ XI_ERR_TYPE xiSort_U16_ref(xi_pArray keys, xi_pArray values, xi_pArray keys_tmp, xi_pArray values_tmp, xi_bool ascending);
_XI_APIREF_ XI_ERR_TYPE xiSort_S16_ref(xi_pArray keys, xi_pArray values, xi_pArray keys_tmp, xi_pArray values_tmp, xi_bool ascending);
_XI_APIREF_ XI_ERR_TYPE xiSort_U8_2_ref(const xi_pArray keys, const xi_pArray values, xi_pArray keys_out, xi_pArray values_out, int size_of_value, xi_bool ascending);
_XI_APIREF_ XI_ERR_TYPE xiSort_U16_2_ref(xi_pArray keys, xi_pArray values, xi_pArray keys_tmp, xi_pArray values_tmp, int size_of_value, xi_bool ascending);
_XI_APIREF_ XI_ERR_TYPE xiSort_S16_2_ref(xi_pArray keys, xi_pArray values, xi_pArray keys_tmp, xi_pArray values_tmp, int size_of_value, xi_bool ascending);

/* Exctract points */
_XI_APIREF_ XI_ERR_TYPE xiExtractPoints_U16_ref(const xi_pTile src, int threshold, xi_point* points, uint16_t* weight, int max_points, int* count);
_XI_APIREF_ XI_ERR_TYPE xiExtractPoints_S16_ref(const xi_pTile src, int threshold, xi_pArray pArrpoints, xi_pArray pArrweight, int* count);
_XI_APIREF_ XI_ERR_TYPE xiExtractPoints_U8_ref(const xi_pTile src, int threshold, xi_point* points, uint8_t* weight, int max_points, int* count);
_XI_APIREF_ XI_ERR_TYPE xiExtractPoints_S8_ref(const xi_pTile src, int threshold, xi_point* points, uint8_t* weight, int max_points, int* count);
_XI_APIREF_ XI_ERR_TYPE xiExtractNextPoints_U16_ref(const xi_pTile src, xi_point start_loc, int threshold, xi_point* points, uint16_t* weight, int max_points, int* count);
_XI_APIREF_ XI_ERR_TYPE xiExtractNextPoints_S16_ref(const xi_pTile src, xi_point start_loc, int threshold, xi_point* points, int16_t*  weight, int max_points, int* count);


/* Haar cascades */
_XI_APIREF_ XI_ERR_TYPE xiHaarStump_U16U8_ref(const xi_pArray integral_image, const xi_pArray stdDev, int16_t* cascade, xi_pArray detect_map, int xstep, int ystep, xi_size window, int numStages);


/* RANSAC */
_XI_APIREF_ XI_ERR_TYPE xiRANSAC_line_ref(const xi_point * data, unsigned int data_size, unsigned int model_inliers, unsigned int dist_threshold, unsigned int max_iters, unsigned int term_threshold, float* a, float* b, float* c, int seed);
_XI_APIREF_ XI_ERR_TYPE xiRANSAC_affine_ref(const xi_point *data_from, const xi_point * data_to, unsigned int data_size,
                                         unsigned int model_inliers, unsigned int dist_threshold, unsigned int max_iters,
                                         unsigned int term_threshold_mse, xi_affine* model, int seed);
_XI_APIREF_ XI_ERR_TYPE xiRANSAC_parabola_ref(const xi_point * data, unsigned int data_size, unsigned int model_inliers, unsigned int dist_threshold, unsigned int max_iters, unsigned int term_threshold, float* a, float* b, float* c, int seed);


/* HOG Histogram */
_XI_APIREF_ XI_ERR_TYPE xiHOGBinPlane_S16_ref(const xi_pArray src_mag, const xi_pArray src_ang_Q3_12, const xi_pArray dst, int bin, int nbins);
_XI_APIREF_ XI_ERR_TYPE xiHOGGradient_U8S16_ref(const xi_pTile src, xi_pArray dst_dx, xi_pArray dst_dy);
_XI_APIREF_ XI_ERR_TYPE xiHOGGetBlockWeights_U16_Q15_ref(xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiHOGGetBlockHist_ref(const xi_pTile src_mag, const xi_pArray src_ang, const xi_pArray weight_Q15, xi_pArray hist, xi_point pt, xi_size cellSize, int nbins);
_XI_APIREF_ XI_ERR_TYPE xiHOGNormalizeHist_ref(xi_pArray hist, int count);


/* SVM */
_XI_APIREF_ XI_ERR_TYPE xiSVMPredict_S16_ref(const xi_pTile sample, const xi_pTile support_vectors, int64_t* result);

/*Standard LBP*/
_XI_APIREF_ XI_ERR_TYPE xiLBP_3x3_U8_ref (const xi_pTile src, xi_pTile dst);
_XI_APIREF_ XI_ERR_TYPE xiLBP_5x5_U8_ref (const xi_pTile src, xi_pTile dst);

/*Modified LBP*/
_XI_APIREF_ XI_ERR_TYPE xiMLBP_3x3_U8_ref (const xi_pTile src, xi_pTile dst);
_XI_APIREF_ XI_ERR_TYPE xiMLBP_5x5_U8_ref (const xi_pTile src, xi_pTile dst);

/*Uniform LBP*/
_XI_APIREF_ XI_ERR_TYPE xiULBP_3x3_U8_ref (const xi_pTile src, xi_pTile dst);
_XI_APIREF_ XI_ERR_TYPE xiULBP_5x5_U8_ref (const xi_pTile src, xi_pTile dst);

/*2D Matrix Multiply*/
_XI_APIREF_ XI_ERR_TYPE xiMatrixMultiply_U8_ref(const xi_pArray src0, const xi_pArray src1, const xi_pArray src2, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMatrixMultiply_S8_ref(const xi_pArray src0, const xi_pArray src1, const xi_pArray src2, xi_pArray dst);
_XI_APIREF_ XI_ERR_TYPE xiMatrixMultiply_S16Q8_ref(const xi_pArray src0, const xi_pArray src1, const xi_pArray src2, xi_pArray dst);

#endif
