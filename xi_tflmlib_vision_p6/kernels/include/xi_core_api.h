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
#ifndef __XI_CORE_API_H__
#define __XI_CORE_API_H__

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

#include "xi_config_api.h"
#include "xi_tile_manager.h"
#include "xi_tile3d_manager.h"

/* library information */
_XI_API_ char XI_BUILD_CONFIGURATION[];
_XI_API_ char XI_BUILD_TOOLS_VERSION[];
_XI_API_ char XI_BUILD_LIBRARY_VERSION[];
_XI_API_ char XI_BUILD_CORE_ID[];
_XI_API_ char XI_BUILD_ERROR_LEVEL[];
_XI_API_ char XI_BUILD_FEATURES_STR[];

/* Math constants */

#define XI_PI   3.14159265358979323846
#define XI_PI_F 3.14159265358979323846f

/* compiler-specific macro */

#if defined __GNUC__
#  define XI_INLINE_OR_DIE static inline __attribute__ ((always_inline))
#  define XI_NOINLINE __attribute__ ((noinline))
#  define XI_ALIGNED(alignment) __attribute__((aligned(alignment)))
#else
#  define XI_INLINE_OR_DIE static
#  define XI_NOINLINE
#  define XI_ALIGNED(alignment)
#endif

/* IVP library data types */

typedef int32_t XI_ERR_TYPE;
typedef uint8_t xi_bool;

typedef int16_t XI_Q0_15;
typedef int16_t XI_Q5_10;
typedef int16_t XI_Q6_9;
typedef int16_t XI_Q7_8;
typedef int16_t XI_Q8_7;
typedef int16_t XI_Q12_3;
typedef int16_t XI_Q13_2;

typedef int32_t XI_Q0_31;
typedef int32_t XI_Q1_30;
typedef int32_t XI_Q12_19;
typedef int32_t XI_Q13_18;
typedef int32_t XI_Q15_16;
typedef int32_t XI_Q16_15;
typedef int32_t XI_Q22_9;
typedef int32_t XI_Q28_3;

typedef XI_Q0_15 XI_Q15;
typedef uint16_t XI_Q0_16;


typedef struct {
    int16_t x;
    int16_t y;
} xi_point;

typedef struct {
    int32_t x;
    int32_t y;
} xi_point32;

typedef struct {
    XI_Q16_15 x;
    XI_Q16_15 y;
} xi_point_fpt;

typedef struct {
    float x;
    float y;
} xi_point_f;

typedef struct {
    uint16_t width;
    uint16_t height;
} xi_size;

typedef struct {
    float a11;
    float a12;
    float a21;
    float a22;
    float xt;
    float yt;
} xi_affine;

typedef struct {
    XI_Q13_18 a11;
    XI_Q13_18 a12;
    XI_Q13_18 a21;
    XI_Q13_18 a22;
    XI_Q13_18 xt;
    XI_Q13_18 yt;
} xi_affine_fpt;

typedef struct {
    float a11;
    float a12;
    float a13;
    float a21;
    float a22;
    float a23;
    float a31;
    float a32;
    float a33;
} xi_perspective;

typedef struct {
    XI_Q13_18 a11;
    XI_Q13_18 a12;
    XI_Q13_18 a13;
    XI_Q13_18 a21;
    XI_Q13_18 a22;
    XI_Q13_18 a23;
    XI_Q13_18 a31;
    XI_Q13_18 a32;
    XI_Q13_18 a33;
} xi_perspective_fpt;

typedef struct {
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
} xi_rect;

typedef struct {
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
    XI_Q5_10 angle;
} xi_rotated_rect;

typedef struct {
    float x;
    float y;
    float width;
    float height;
    float angle;
} xi_rotated_rect_f;

typedef struct
{
    int32_t M00;
    int64_t M10;
    int64_t M01;
    int64_t M11;
    int64_t M20;
    int64_t M02;
} xi_moments;

typedef struct XI_ALIGNED(XI_KEYPOINT_ALIGNMENT)
{
    int32_t x;
    int32_t y;
    float   strength;
    float   scale;
    float   orientation;
    int32_t tracking_status;
    float   error;
} xi_keypoint;

typedef struct
{
    XI_Q13_18 rho;
    XI_Q13_18 theta;
} xi_line_polar_fpt;

typedef struct
{
    uint32_t  size;   // number of pyramid levels
    float     scale;
    xi_tile **levels; // array of pyramid levels
} xi_pyramid, *xi_pPyramid;
#define XI_HAS_PYRAMID 1


/* Error codes */

#define XI_ERR_OK               0   // no error
#define XI_ERR_IALIGNMENT       1   // input alignment requirements are not satisfied
#define XI_ERR_OALIGNMENT       2   // output alignment requirements are not satisfied
#define XI_ERR_MALIGNMENT       3   // same modulo alignment requirement is not satisfied
#define XI_ERR_BADARG           4   // arguments are somehow invalid
#define XI_ERR_MEMLOCAL         5   // tile is not placed in local memory
#define XI_ERR_INPLACE          6   // inplace operation is not supported
#define XI_ERR_EDGE             7   // edge extension size is too small
#define XI_ERR_DATASIZE         8   // input/output tile size is too small or too big or otherwise inconsistent
#define XI_ERR_TMPSIZE          9   // temporary tile size is too small or otherwise inconsistent
#define XI_ERR_KSIZE            10  // filer kernel size is not supported
#define XI_ERR_NORM             11  // invalid normalization divisor or shift value
#define XI_ERR_COORD            12  // invalid coordinates
#define XI_ERR_BADTRANSFORM     13  // the transform is singular or otherwise invalid
#define XI_ERR_NULLARG          14  // one of required arguments is null
#define XI_ERR_THRESH_INVALID   15  // threshold value is somehow invalid
#define XI_ERR_SCALE            16  // provided scale factor is not supported
#define XI_ERR_OVERFLOW         17  // tile size can lead to sum overflow
#define XI_ERR_NOTIMPLEMENTED   18  // the requested functionality is absent in current version
#define XI_ERR_CHANNEL_INVALID  19  // invalid channel number
#define XI_ERR_DATATYPE         20  // argument has invalid data type
#define XI_ERR_NO_VARIANT       21  // No suitable variant found for the function

#define XI_ERR_LAST             21

/* non-fatal errors */

#define XI_ERR_POOR_DECOMPOSITION  1024 // computed transform decomposition can produce visual artifacts
#define XI_ERR_OUTOFTILE           1025 // arguments or results are out of tile
#define XI_ERR_OBJECTLOST          1026 // tracked object is lost
#define XI_ERR_RANSAC_NOTFOUND     1027 // there is no found appropriate model for RANSAC
#define XI_ERR_REPLAY              1028 // function has to be called again for completion


/* helper macro */

#ifdef XCHAL_IVPN_SIMD_WIDTH
#  define XI_SIMD_WIDTH XCHAL_IVPN_SIMD_WIDTH
#else
#  define XI_SIMD_WIDTH 32
#endif

#define XI_SIZE_AREA(sz) ((size_t)sz.width * sz.height)
#define XI_ALIGN_VAL(val, pow2) (((val) + ((pow2) - 1)) & ~((pow2) - 1))
#define XI_ALIGN_VALN(val) XI_ALIGN_VAL(val, XI_SIMD_WIDTH)


/* temporary space requirement for xiSort */
#if XCHAL_HAVE_GRIVPEP_HISTOGRAM  ||  XCHAL_HAVE_VISION_HISTOGRAM
#   define XI_SORT_TMP_SIZE 0                                        // use vector registers only
#elif XCHAL_HAVE_VISION
#   define XI_SORT_TMP_SIZE (XI_SIMD_WIDTH * 256 + XI_SIMD_WIDTH)    // SIMD_WIDTH histograms by 256 bins + 32 for pointer alignment inside optimized function
#else
#   define XI_SORT_TMP_SIZE (2 * 256 + XI_SIMD_WIDTH)                // 3 histograms by 256 bins + 32 for pointer alignment inside optimized function
#endif


/* error code to text conversion */
_XI_API_ const char* xiErrStr(XI_ERR_TYPE code);


/* setting tile to constant value */
_XI_API_ XI_ERR_TYPE xiFillTile_I8 (xi_pArray dst, int value, xi_bool fill_edge_extension XI_DEFAULT(true));
_XI_API_ XI_ERR_TYPE xiFillTile_I16(xi_pArray dst, int value, xi_bool fill_edge_extension XI_DEFAULT(true));
#define xiFillArray_I8(dst, value)  xiFillTile_I8 (dst, value, 0)
#define xiFillArray_I16(dst, value) xiFillTile_I16(dst, value, 0)


/* copying one tile to another  */
_XI_API_ XI_ERR_TYPE xiCopyTile    (const xi_pArray src, xi_pArray dst, xi_bool copy_edge_extension XI_DEFAULT(true));
#define xiCopyArray(src, dst) xiCopyTile(src, dst, 0)


/* exchange tile data  */
_XI_API_ XI_ERR_TYPE xiSwapTiles(xi_pArray t1 /*inout*/, xi_pArray t2 /*inout*/, xi_bool with_edge_extension XI_DEFAULT(false));
#define xiSwapArrays(t1, t2) xiSwapTiles(t1, t2, 0)


/* finding min and max value and their locations */
_XI_API_ XI_ERR_TYPE xiMinMaxLoc_U8  (const xi_pArray src, unsigned* min_value, unsigned* max_value, xi_point* min_loc, xi_point* max_loc);
_XI_API_ XI_ERR_TYPE xiMinMaxLoc_U8A2(const xi_pArray src, unsigned* min_value, unsigned* max_value, xi_point* min_loc, xi_point* max_loc);
_XI_API_ XI_ERR_TYPE xiMinMaxLoc_S16 (const xi_pArray src, int*      min_value, int*      max_value, xi_point* min_loc, xi_point* max_loc);
_XI_API_ XI_ERR_TYPE xiMinMaxLoc_S16A(const xi_pArray src, int*      min_value, int*      max_value, xi_point* min_loc, xi_point* max_loc);


/* finding min value and its location */
_XI_API_ XI_ERR_TYPE xiMinLoc_U8  (const xi_pArray src, unsigned* min_value, xi_point* min_loc);
_XI_API_ XI_ERR_TYPE xiMinLoc_U8A2(const xi_pArray src, unsigned* min_value, xi_point* min_loc);
_XI_API_ XI_ERR_TYPE xiMinLoc_S16 (const xi_pArray src, int*      min_value, xi_point* min_loc);
_XI_API_ XI_ERR_TYPE xiMinLoc_S16A(const xi_pArray src, int*      min_value, xi_point* min_loc);


/* finding max value and its location */
_XI_API_ XI_ERR_TYPE xiMaxLoc_U8  (const xi_pArray src, unsigned* max_value, xi_point* max_loc);
_XI_API_ XI_ERR_TYPE xiMaxLoc_U8A2(const xi_pArray src, unsigned* max_value, xi_point* max_loc);
_XI_API_ XI_ERR_TYPE xiMaxLoc_S16 (const xi_pArray src, int*      max_value, xi_point* max_loc);
_XI_API_ XI_ERR_TYPE xiMaxLoc_S16A(const xi_pArray src, int*      max_value, xi_point* max_loc);


/* calculating Min and Max values */
_XI_API_ XI_ERR_TYPE xiRMinMax_U8  (const xi_pArray src, unsigned* minval, unsigned* maxval);
_XI_API_ XI_ERR_TYPE xiRMinMax_U8A2(const xi_pArray src, unsigned* minval, unsigned* maxval);
_XI_API_ XI_ERR_TYPE xiRMinMax_S16 (const xi_pArray src, int*      minval, int*      maxval);
_XI_API_ XI_ERR_TYPE xiRMinMax_S16A(const xi_pArray src, int*      minval, int*      maxval);

_XI_API_ XI_ERR_TYPE xiRMinMax2_S16 (const xi_pArray src, int* minval0, int* minval1, int* maxval0, int* maxval1);
_XI_API_ XI_ERR_TYPE xiRMinMax2_S16A(const xi_pArray src, int* minval0, int* minval1, int* maxval0, int* maxval1);


/* gather locations of the specific value */
_XI_API_ XI_ERR_TYPE xiGatherLocationsEQ_I8     (const xi_pTile src, xi_pArray locations, int* count, int value);
_XI_API_ XI_ERR_TYPE xiGatherLocationsEQ_I16    (const xi_pTile src, xi_pArray locations, int* count, int value);
_XI_API_ XI_ERR_TYPE xiGatherNextLocationsEQ_I8 (const xi_pTile src, xi_point32 start_loc, xi_pArray locations, int* count, int value);
_XI_API_ XI_ERR_TYPE xiGatherNextLocationsEQ_I16(const xi_pTile src, xi_point32 start_loc, xi_pArray locations, int* count, int value);


/* mean and standard deviation */
_XI_API_ XI_ERR_TYPE xiMeanStdDev_U8 (const xi_pArray src, XI_Q8_7*   mean, float* stddev);
_XI_API_ XI_ERR_TYPE xiMeanStdDev_S16(const xi_pArray src, XI_Q15_16* mean, float* stddev);

_XI_API_ XI_ERR_TYPE xiSumSquares_U8 (const xi_pArray src, uint32_t* sum, uint32_t* sqsum);
_XI_API_ XI_ERR_TYPE xiSumSquares_S16(const xi_pArray src, int64_t*  sum, uint64_t* sqsum);


/* log */
_XI_API_ XI_ERR_TYPE xiLog_S16_Q3_12_init(xi_pArray lut);
_XI_API_ XI_ERR_TYPE xiLog_S16_Q3_12_2 (const xi_pArray src, xi_pArray dst, const xi_pArray lut);
_XI_API_ XI_ERR_TYPE xiLog_S16A_Q3_12_2(const xi_pArray src, xi_pArray dst, const xi_pArray lut);

_XI_API_ XI_ERR_TYPE xiLog_S16_Q3_12 (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiLog_S16A_Q3_12(const xi_pArray src, xi_pArray dst);


/* look-up */
_XI_API_ XI_ERR_TYPE xiLUT64_U8   (const xi_pArray src, xi_pArray dst, const uint8_t* lut64);
_XI_API_ XI_ERR_TYPE xiLUT64_U8A2 (const xi_pArray src, xi_pArray dst, const uint8_t* lut64);
_XI_API_ XI_ERR_TYPE xiLUT128_U8  (const xi_pArray src, xi_pArray dst, const uint8_t* lut128);
_XI_API_ XI_ERR_TYPE xiLUT128_U8A2(const xi_pArray src, xi_pArray dst, const uint8_t* lut128);
_XI_API_ XI_ERR_TYPE xiLUT256_U8  (const xi_pArray src, xi_pArray dst, const uint8_t* lut256);
_XI_API_ XI_ERR_TYPE xiLUT256_U8A2(const xi_pArray src, xi_pArray dst, const uint8_t* lut256);

_XI_API_ XI_ERR_TYPE xiLUT_S16    (const xi_pArray src, xi_pArray dst, const int16_t* lut, int lutsz, int lutoffs);
_XI_API_ XI_ERR_TYPE xiLUT_S16Oa  (const xi_pArray src, xi_pArray dst, const int16_t* lut, int lutsz, int lutoffs);


/* clip to specific range */
_XI_API_ XI_ERR_TYPE xiClip_U8  (const xi_pArray src, xi_pArray dst, int low, int high);
_XI_API_ XI_ERR_TYPE xiClip_U8A2(const xi_pArray src, xi_pArray dst, int low, int high);
_XI_API_ XI_ERR_TYPE xiClip_S16 (const xi_pArray src, xi_pArray dst, int low, int high);
_XI_API_ XI_ERR_TYPE xiClip_S16A(const xi_pArray src, xi_pArray dst, int low, int high);


/* generate mask for specific range */
_XI_API_ XI_ERR_TYPE xiInRange2_U8     (const xi_pArray src, xi_pArray dst, int low, int high, int trueval, int falseval);
_XI_API_ XI_ERR_TYPE xiInRange2_U8A2   (const xi_pArray src, xi_pArray dst, int low, int high, int trueval, int falseval);
_XI_API_ XI_ERR_TYPE xiInRange2_S16U8  (const xi_pArray src, xi_pArray dst, int low, int high, int trueval, int falseval);
_XI_API_ XI_ERR_TYPE xiInRange2_S16U8A2(const xi_pArray src, xi_pArray dst, int low, int high, int trueval, int falseval);
_XI_API_ XI_ERR_TYPE xiInRange2_S16U16 (const xi_pArray src, xi_pArray dst, int low, int high, int trueval, int falseval);
_XI_API_ XI_ERR_TYPE xiInRange2_S16U16A(const xi_pArray src, xi_pArray dst, int low, int high, int trueval, int falseval);

#define xiInRange_U8(src, dst, low, high)      xiInRange2_U8(src, dst, low, high, 255, 0)
#define xiInRange_U8A2(src, dst, low, high)    xiInRange2_U8A2(src, dst, low, high, 255, 0)
#define xiInRange_S16U8(src, dst, low, high)   xiInRange2_S16U8(src, dst, low, high, 255, 0)
#define xiInRange_S16U8A2(src, dst, low, high) xiInRange2_S16U8A2(src, dst, low, high, 255, 0)
#define xiInRange_S16U16A(src, dst, low, high) xiInRange2_S16U16A(src, dst, low, high, 65535, 0)
#define xiInRange_S16U16(src, dst, low, high)  xiInRange2_S16U16(src, dst, low, high, 65535, 0)


/* check range */
_XI_API_ XI_ERR_TYPE xiCheckRange_U8  (const xi_pArray src, xi_bool* result, xi_point* pos, int low, int high);
_XI_API_ XI_ERR_TYPE xiCheckRange_U8A2(const xi_pArray src, xi_bool* result, xi_point* pos, int low, int high);
_XI_API_ XI_ERR_TYPE xiCheckRange_S16 (const xi_pArray src, xi_bool* result, xi_point* pos, int low, int high);
_XI_API_ XI_ERR_TYPE xiCheckRange_S16A(const xi_pArray src, xi_bool* result, xi_point* pos, int low, int high);


/* data depth conversion */
_XI_API_ XI_ERR_TYPE xiConvertBitDepth_U8S16   (const xi_pArray src, xi_pArray dst, unsigned r);
_XI_API_ XI_ERR_TYPE xiConvertBitDepth_U8S16A  (const xi_pArray src, xi_pArray dst, unsigned r);
_XI_API_ XI_ERR_TYPE xiConvertBitDepth_S16U8   (const xi_pArray src, xi_pArray dst, unsigned r);
_XI_API_ XI_ERR_TYPE xiConvertBitDepth_S16U8A  (const xi_pArray src, xi_pArray dst, unsigned r);
_XI_API_ XI_ERR_TYPE xiConvertBitDepth_S_S16U8 (const xi_pArray src, xi_pArray dst, unsigned r);
_XI_API_ XI_ERR_TYPE xiConvertBitDepth_S_S16U8A(const xi_pArray src, xi_pArray dst, unsigned r);


/* sum elements */
_XI_API_ XI_ERR_TYPE xiSum_U8  (const xi_pArray src, unsigned* sum);
_XI_API_ XI_ERR_TYPE xiSum_U8A2(const xi_pArray src, unsigned* sum);
_XI_API_ XI_ERR_TYPE xiSum_S16 (const xi_pArray src, int*      sum);
_XI_API_ XI_ERR_TYPE xiSum_S16A(const xi_pArray src, int*      sum);


/* count elements */
_XI_API_ XI_ERR_TYPE xiCountNonZero_I8  (const xi_pArray src, int* count);
_XI_API_ XI_ERR_TYPE xiCountNonZero_I8A2(const xi_pArray src, int* count);
_XI_API_ XI_ERR_TYPE xiCountNonZero_I16 (const xi_pArray src, int* count);
_XI_API_ XI_ERR_TYPE xiCountNonZero_I16A(const xi_pArray src, int* count);

_XI_API_ XI_ERR_TYPE xiCountEqual_I8  (const xi_pArray src, int value, int *count);
_XI_API_ XI_ERR_TYPE xiCountEqual_I8A2(const xi_pArray src, int value, int *count);
_XI_API_ XI_ERR_TYPE xiCountEqual_I16 (const xi_pArray src, int value, int *count);
_XI_API_ XI_ERR_TYPE xiCountEqual_I16A(const xi_pArray src, int value, int *count);

_XI_API_ XI_ERR_TYPE xiCountGreater_U8  (const xi_pArray src, unsigned threshold, int *count);
_XI_API_ XI_ERR_TYPE xiCountGreater_U8A2(const xi_pArray src, unsigned threshold, int *count);
_XI_API_ XI_ERR_TYPE xiCountGreater_U16 (const xi_pArray src, unsigned threshold, int *count);
_XI_API_ XI_ERR_TYPE xiCountGreater_U16A(const xi_pArray src, unsigned threshold, int *count);
_XI_API_ XI_ERR_TYPE xiCountGreater_S16 (const xi_pArray src, int      threshold, int *count);
_XI_API_ XI_ERR_TYPE xiCountGreater_S16A(const xi_pArray src, int      threshold, int *count);


/* L1 Norm */
_XI_API_ XI_ERR_TYPE xiNormL1_U8  (const xi_pArray src1, const xi_pArray src2, unsigned* norm);
_XI_API_ XI_ERR_TYPE xiNormL1_U8A2(const xi_pArray src1, const xi_pArray src2, unsigned* norm);
_XI_API_ XI_ERR_TYPE xiNormL1_S16 (const xi_pArray src1, const xi_pArray src2, unsigned* norm);
_XI_API_ XI_ERR_TYPE xiNormL1_S16A(const xi_pArray src1, const xi_pArray src2, unsigned* norm);


/* L2 norm */
_XI_API_ XI_ERR_TYPE xiNormL2_U8  (const xi_pArray src1, const xi_pArray src2, unsigned* norm);
_XI_API_ XI_ERR_TYPE xiNormL2_U8A2(const xi_pArray src1, const xi_pArray src2, unsigned* norm);
_XI_API_ XI_ERR_TYPE xiNormL2_S16 (const xi_pArray src1, const xi_pArray src2, unsigned* norm);
_XI_API_ XI_ERR_TYPE xiNormL2_S16A(const xi_pArray src1, const xi_pArray src2, unsigned* norm);

_XI_API_ XI_ERR_TYPE xiNormL2S_U8  (const xi_pArray src1, const xi_pArray src2, unsigned* norm);
_XI_API_ XI_ERR_TYPE xiNormL2S_U8A2(const xi_pArray src1, const xi_pArray src2, unsigned* norm);


/* infinite norm */
_XI_API_ XI_ERR_TYPE xiNormInf_U8  (const xi_pArray src1, const xi_pArray src2, unsigned* norm);
_XI_API_ XI_ERR_TYPE xiNormInf_U8A2(const xi_pArray src1, const xi_pArray src2, unsigned* norm);
_XI_API_ XI_ERR_TYPE xiNormInf_S16 (const xi_pArray src1, const xi_pArray src2, unsigned* norm);
_XI_API_ XI_ERR_TYPE xiNormInf_S16A(const xi_pArray src1, const xi_pArray src2, unsigned* norm);


/* Hamming norm */
_XI_API_ XI_ERR_TYPE xiNormHamming_U8  (const xi_pArray src1, const xi_pArray src2, unsigned* norm);
_XI_API_ XI_ERR_TYPE xiNormHamming_U8A2(const xi_pArray src1, const xi_pArray src2, unsigned* norm);


/* magnitude */
_XI_API_ XI_ERR_TYPE xiMagnitude_U8U16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMagnitude_U8U16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMagnitude_S_S16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMagnitude_S_S16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);


/* square root */
_XI_API_ XI_ERR_TYPE xiSquareRoot_U32U16 (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiSquareRoot_U32U16A(const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiSquareRoot_U16A(const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiSquareRoot_U16 (const xi_pArray src, xi_pArray dst);


/* merge channels */
_XI_API_ XI_ERR_TYPE xiMerge2_I8  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMerge2_I8A2(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMerge2_I16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMerge2_I16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiMerge3_I8  (const xi_pArray src0, const xi_pArray src1, const xi_pArray src2, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMerge3_I8A2(const xi_pArray src0, const xi_pArray src1, const xi_pArray src2, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMerge3_I16 (const xi_pArray src0, const xi_pArray src1, const xi_pArray src2, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMerge3_I16A(const xi_pArray src0, const xi_pArray src1, const xi_pArray src2, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiMerge4_I8  (const xi_pArray src0, const xi_pArray src1, const xi_pArray src2, const xi_pArray src3, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMerge4_I8A2(const xi_pArray src0, const xi_pArray src1, const xi_pArray src2, const xi_pArray src3, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMerge4_I16 (const xi_pArray src0, const xi_pArray src1, const xi_pArray src2, const xi_pArray src3, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMerge4_I16A(const xi_pArray src0, const xi_pArray src1, const xi_pArray src2, const xi_pArray src3, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiMerge_U8_YUYV  (const xi_pArray src_y, const xi_pArray src_u, const xi_pArray src_v, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMerge_U8A2_YUYV(const xi_pArray src_y, const xi_pArray src_u, const xi_pArray src_v, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMerge_U8_UYVY  (const xi_pArray src_y, const xi_pArray src_u, const xi_pArray src_v, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMerge_U8A2_UYVY(const xi_pArray src_y, const xi_pArray src_u, const xi_pArray src_v, xi_pArray dst);


/* split channels */
_XI_API_ XI_ERR_TYPE xiSplit2_I8  (const xi_pArray src, xi_pArray dst0, xi_pArray dst1);
_XI_API_ XI_ERR_TYPE xiSplit2_I8A2(const xi_pArray src, xi_pArray dst0, xi_pArray dst1);
_XI_API_ XI_ERR_TYPE xiSplit2_I16 (const xi_pArray src, xi_pArray dst0, xi_pArray dst1);
_XI_API_ XI_ERR_TYPE xiSplit2_I16A(const xi_pArray src, xi_pArray dst0, xi_pArray dst1);

_XI_API_ XI_ERR_TYPE xiSplit3_I8  (const xi_pArray src, xi_pArray dst0, xi_pArray dst1, xi_pArray dst2);
_XI_API_ XI_ERR_TYPE xiSplit3_I8A2(const xi_pArray src, xi_pArray dst0, xi_pArray dst1, xi_pArray dst2);
_XI_API_ XI_ERR_TYPE xiSplit3_I16 (const xi_pArray src, xi_pArray dst0, xi_pArray dst1, xi_pArray dst2);
_XI_API_ XI_ERR_TYPE xiSplit3_I16A(const xi_pArray src, xi_pArray dst0, xi_pArray dst1, xi_pArray dst2);

_XI_API_ XI_ERR_TYPE xiSplit4_I8  (const xi_pArray src, xi_pArray dst0, xi_pArray dst1, xi_pArray dst2, xi_pArray dst3);
_XI_API_ XI_ERR_TYPE xiSplit4_I8A2(const xi_pArray src, xi_pArray dst0, xi_pArray dst1, xi_pArray dst2, xi_pArray dst3);
_XI_API_ XI_ERR_TYPE xiSplit4_I16 (const xi_pArray src, xi_pArray dst0, xi_pArray dst1, xi_pArray dst2, xi_pArray dst3);
_XI_API_ XI_ERR_TYPE xiSplit4_I16A(const xi_pArray src, xi_pArray dst0, xi_pArray dst1, xi_pArray dst2, xi_pArray dst3);


/* extract one channel */
_XI_API_ XI_ERR_TYPE xiExtractChannel2_I8  (const xi_pArray src, xi_pArray dst, int channel);
_XI_API_ XI_ERR_TYPE xiExtractChannel2_I8A2(const xi_pArray src, xi_pArray dst, int channel);
_XI_API_ XI_ERR_TYPE xiExtractChannel3_I8  (const xi_pArray src, xi_pArray dst, int channel);
_XI_API_ XI_ERR_TYPE xiExtractChannel3_I8A2(const xi_pArray src, xi_pArray dst, int channel);
_XI_API_ XI_ERR_TYPE xiExtractChannel4_I8  (const xi_pArray src, xi_pArray dst, int channel);
_XI_API_ XI_ERR_TYPE xiExtractChannel4_I8A2(const xi_pArray src, xi_pArray dst, int channel);


/* extend edges */
_XI_API_ XI_ERR_TYPE xiExtendEdgesConst_I8 (xi_pTile tile /*inout*/, int frame_width, int frame_height, int value);
_XI_API_ XI_ERR_TYPE xiExtendEdgesConst_I16(xi_pTile tile /*inout*/, int frame_width, int frame_height, int value);

_XI_API_ XI_ERR_TYPE xiExtendEdgesReplicate_I8 (xi_pTile tile /*inout*/, int frame_width, int frame_height);
_XI_API_ XI_ERR_TYPE xiExtendEdgesReplicate_I16(xi_pTile tile /*inout*/, int frame_width, int frame_height);

_XI_API_ XI_ERR_TYPE xiExtendEdgesReflect101_I8 (xi_pTile tile /*inout*/, int frame_width, int frame_height);
_XI_API_ XI_ERR_TYPE xiExtendEdgesReflect101_I16(xi_pTile tile /*inout*/, int frame_width, int frame_height);


/* phase */
_XI_API_ XI_ERR_TYPE xiPhase_init(xi_pArray atan_lut, int input_type, int output_type);

_XI_API_ XI_ERR_TYPE xiPhase_U8S16_Q3_12_2 (const xi_pArray x, const xi_pArray y, xi_pArray angle, const xi_pArray atan_lut);
_XI_API_ XI_ERR_TYPE xiPhase_U8S16A_Q3_12_2(const xi_pArray x, const xi_pArray y, xi_pArray angle, const xi_pArray atan_lut);
_XI_API_ XI_ERR_TYPE xiPhase_S16_Q3_12_2   (const xi_pArray x, const xi_pArray y, xi_pArray angle, const xi_pArray atan_lut);
_XI_API_ XI_ERR_TYPE xiPhase_S16A_Q3_12_2  (const xi_pArray x, const xi_pArray y, xi_pArray angle, const xi_pArray atan_lut);

_XI_API_ XI_ERR_TYPE xiPhase_S16U8_2 (const xi_pArray x, const xi_pArray y, xi_pArray angle, const xi_pArray atan_lut);
_XI_API_ XI_ERR_TYPE xiPhase_S16U8A_2(const xi_pArray x, const xi_pArray y, xi_pArray angle, const xi_pArray atan_lut);


_XI_API_ XI_ERR_TYPE xiPhase_U8S16_Q3_12 (const xi_pArray x, const xi_pArray y, xi_pArray angle);
_XI_API_ XI_ERR_TYPE xiPhase_U8S16A_Q3_12(const xi_pArray x, const xi_pArray y, xi_pArray angle);
_XI_API_ XI_ERR_TYPE xiPhase_S16_Q3_12   (const xi_pArray x, const xi_pArray y, xi_pArray angle);
_XI_API_ XI_ERR_TYPE xiPhase_S16A_Q3_12  (const xi_pArray x, const xi_pArray y, xi_pArray angle);

_XI_API_ XI_ERR_TYPE xiPhase_S16U8 (const xi_pArray x, const xi_pArray y, xi_pArray angle);
_XI_API_ XI_ERR_TYPE xiPhase_S16U8A(const xi_pArray x, const xi_pArray y, xi_pArray angle);


/*  B = max(A, a) */
_XI_API_ XI_ERR_TYPE xiMaxScalar_U8  (const xi_pArray src, xi_pArray dst, unsigned a);
_XI_API_ XI_ERR_TYPE xiMaxScalar_U8A2(const xi_pArray src, xi_pArray dst, unsigned a);
_XI_API_ XI_ERR_TYPE xiMaxScalar_S16 (const xi_pArray src, xi_pArray dst, int      a);
_XI_API_ XI_ERR_TYPE xiMaxScalar_S16A(const xi_pArray src, xi_pArray dst, int      a);


/*  B = min(A, a) */
_XI_API_ XI_ERR_TYPE xiMinScalar_U8  (const xi_pArray src, xi_pArray dst, unsigned a);
_XI_API_ XI_ERR_TYPE xiMinScalar_U8A2(const xi_pArray src, xi_pArray dst, unsigned a);
_XI_API_ XI_ERR_TYPE xiMinScalar_S16 (const xi_pArray src, xi_pArray dst, int      a);
_XI_API_ XI_ERR_TYPE xiMinScalar_S16A(const xi_pArray src, xi_pArray dst, int      a);


/* B = a * A + b */
_XI_API_ XI_ERR_TYPE xiConvertScale_U8  (const xi_pArray src, xi_pArray dst, int a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScale_U8A (const xi_pArray src, xi_pArray dst, int a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScale_S16 (const xi_pArray src, xi_pArray dst, int a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScale_S16A(const xi_pArray src, xi_pArray dst, int a, int b);

_XI_API_ XI_ERR_TYPE xiConvertScale_U8_Q15  (const xi_pArray src, xi_pArray dst, XI_Q15 a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScale_U8A_Q15 (const xi_pArray src, xi_pArray dst, XI_Q15 a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScale_S16_Q15 (const xi_pArray src, xi_pArray dst, XI_Q15 a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScale_S16A_Q15(const xi_pArray src, xi_pArray dst, XI_Q15 a, int b);

_XI_API_ XI_ERR_TYPE xiConvertScale_U8_Q5_10  (const xi_pArray src, xi_pArray dst, XI_Q5_10 a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScale_U8A_Q5_10 (const xi_pArray src, xi_pArray dst, XI_Q5_10 a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScale_S16_Q5_10 (const xi_pArray src, xi_pArray dst, XI_Q5_10 a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScale_S16A_Q5_10(const xi_pArray src, xi_pArray dst, XI_Q5_10 a, int b);

_XI_API_ XI_ERR_TYPE xiConvertScale_S_U8  (const xi_pArray src, xi_pArray dst, int a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScale_S_U8A2(const xi_pArray src, xi_pArray dst, int a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScale_S_S16 (const xi_pArray src, xi_pArray dst, int a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScale_S_S16A(const xi_pArray src, xi_pArray dst, int a, int b);

_XI_API_ XI_ERR_TYPE xiConvertScale_S_U8_Q15  (const xi_pArray src, xi_pArray dst, XI_Q15 a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScale_S_U8A_Q15 (const xi_pArray src, xi_pArray dst, XI_Q15 a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScale_S_S16_Q15 (const xi_pArray src, xi_pArray dst, XI_Q15 a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScale_S_S16A_Q15(const xi_pArray src, xi_pArray dst, XI_Q15 a, int b);

_XI_API_ XI_ERR_TYPE xiConvertScale_S_U8_Q5_10  (const xi_pArray src, xi_pArray dst, XI_Q5_10 a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScale_S_U8A2_Q5_10(const xi_pArray src, xi_pArray dst, XI_Q5_10 a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScale_S_S16_Q5_10 (const xi_pArray src, xi_pArray dst, XI_Q5_10 a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScale_S_S16A_Q5_10(const xi_pArray src, xi_pArray dst, XI_Q5_10 a, int b);


/* B = |a * A + b| */
_XI_API_ XI_ERR_TYPE xiConvertScaleAbs_U8  (const xi_pArray src, xi_pArray dst, int a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScaleAbs_U8A (const xi_pArray src, xi_pArray dst, int a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScaleAbs_S16 (const xi_pArray src, xi_pArray dst, int a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScaleAbs_S16A(const xi_pArray src, xi_pArray dst, int a, int b);

_XI_API_ XI_ERR_TYPE xiConvertScaleAbs_U8_Q15  (const xi_pArray src, xi_pArray dst, XI_Q15 a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScaleAbs_U8A_Q15 (const xi_pArray src, xi_pArray dst, XI_Q15 a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScaleAbs_S16_Q15 (const xi_pArray src, xi_pArray dst, XI_Q15 a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScaleAbs_S16A_Q15(const xi_pArray src, xi_pArray dst, XI_Q15 a, int b);

_XI_API_ XI_ERR_TYPE xiConvertScaleAbs_U8_Q5_10  (const xi_pArray src, xi_pArray dst, XI_Q5_10 a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScaleAbs_U8A_Q5_10 (const xi_pArray src, xi_pArray dst, XI_Q5_10 a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScaleAbs_S16_Q5_10 (const xi_pArray src, xi_pArray dst, XI_Q5_10 a, int b);
_XI_API_ XI_ERR_TYPE xiConvertScaleAbs_S16A_Q5_10(const xi_pArray src, xi_pArray dst, XI_Q5_10 a, int b);


/* bitwise scalar and, or, xor */
_XI_API_ XI_ERR_TYPE xiBitwiseAndScalar_I8  (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiBitwiseAndScalar_I8A2(const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiBitwiseAndScalar_I16 (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiBitwiseAndScalar_I16A(const xi_pArray src, xi_pArray dst, int a);

_XI_API_ XI_ERR_TYPE xiBitwiseOrScalar_I8  (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiBitwiseOrScalar_I8A2(const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiBitwiseOrScalar_I16 (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiBitwiseOrScalar_I16A(const xi_pArray src, xi_pArray dst, int a);

_XI_API_ XI_ERR_TYPE xiBitwiseXorScalar_I8  (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiBitwiseXorScalar_I8A2(const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiBitwiseXorScalar_I16 (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiBitwiseXorScalar_I16A(const xi_pArray src, xi_pArray dst, int a);


/* B = a * A */
_XI_API_ XI_ERR_TYPE xiMultiplyScalar_I8  (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiMultiplyScalar_I8A2(const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiMultiplyScalar_I16 (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiMultiplyScalar_I16A(const xi_pArray src, xi_pArray dst, int a);

_XI_API_ XI_ERR_TYPE xiMultiplyScalar_U8_Q15  (const xi_pArray src, xi_pArray dst, XI_Q15 a);
_XI_API_ XI_ERR_TYPE xiMultiplyScalar_U8A_Q15 (const xi_pArray src, xi_pArray dst, XI_Q15 a);
_XI_API_ XI_ERR_TYPE xiMultiplyScalar_S16_Q15 (const xi_pArray src, xi_pArray dst, XI_Q15 a);
_XI_API_ XI_ERR_TYPE xiMultiplyScalar_S16A_Q15(const xi_pArray src, xi_pArray dst, XI_Q15 a);

_XI_API_ XI_ERR_TYPE xiMultiplyScalar_U8_Q5_10  (const xi_pArray src, xi_pArray dst, XI_Q5_10 a);
_XI_API_ XI_ERR_TYPE xiMultiplyScalar_U8A_Q5_10 (const xi_pArray src, xi_pArray dst, XI_Q5_10 a);
_XI_API_ XI_ERR_TYPE xiMultiplyScalar_S16_Q5_10 (const xi_pArray src, xi_pArray dst, XI_Q5_10 a);
_XI_API_ XI_ERR_TYPE xiMultiplyScalar_S16A_Q5_10(const xi_pArray src, xi_pArray dst, XI_Q5_10 a);

_XI_API_ XI_ERR_TYPE xiMultiplyScalar_S_U8  (const xi_pArray src, xi_pArray dst, int a, int fractional_bits XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiMultiplyScalar_S_U8A2(const xi_pArray src, xi_pArray dst, int a, int fractional_bits XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiMultiplyScalar_S_S16 (const xi_pArray src, xi_pArray dst, int a, int fractional_bits XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiMultiplyScalar_S_S16A(const xi_pArray src, xi_pArray dst, int a, int fractional_bits XI_DEFAULT(0));


/* B = A + a */
_XI_API_ XI_ERR_TYPE xiAddScalar_I8    (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiAddScalar_I8A2  (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiAddScalar_I16   (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiAddScalar_I16A  (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiAddScalar_U8S16 (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiAddScalar_U8S16A(const xi_pArray src, xi_pArray dst, int a);

_XI_API_ XI_ERR_TYPE xiAddScalar_S_U8  (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiAddScalar_S_U8A (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiAddScalar_S_S16 (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiAddScalar_S_S16A(const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiAddScalar_S_U8S16(const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiAddScalar_S_U8S16A(const xi_pArray src, xi_pArray dst, int a);


/* B = a - A */
_XI_API_ XI_ERR_TYPE xiSubtractScalar_I8    (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiSubtractScalar_I8A2  (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiSubtractScalar_I16   (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiSubtractScalar_I16A  (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiSubtractScalar_U8S16 (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiSubtractScalar_U8S16A(const xi_pArray src, xi_pArray dst, int a);

_XI_API_ XI_ERR_TYPE xiSubtractScalar_S_U8    (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiSubtractScalar_S_U8A   (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiSubtractScalar_S_S16   (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiSubtractScalar_S_S16A  (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiSubtractScalar_S_U8S16 (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiSubtractScalar_S_U8S16A(const xi_pArray src, xi_pArray dst, int a);

/* B = A - a */
_XI_API_ XI_ERR_TYPE xiSubtractScalar2_S_U8(const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiSubtractScalar2_S_S16(const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiSubtractScalar2_S_U8S16(const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiSubtractScalar2_S_U8A2(const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiSubtractScalar2_S_S16A(const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiSubtractScalar2_S_U8S16A(const xi_pArray src, xi_pArray dst, int a);

/* B = |a - A| */
_XI_API_ XI_ERR_TYPE xiAbsdiffScalar_U8  (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiAbsdiffScalar_U8A2(const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiAbsdiffScalar_S16 (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiAbsdiffScalar_S16A(const xi_pArray src, xi_pArray dst, int a);

_XI_API_ XI_ERR_TYPE xiAbsdiffScalar_S_U8  (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiAbsdiffScalar_S_U8A (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiAbsdiffScalar_S_S16 (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiAbsdiffScalar_S_S16A(const xi_pArray src, xi_pArray dst, int a);


/* B = a / A */
_XI_API_ XI_ERR_TYPE xiDivideScalar_U8  (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiDivideScalar_U8A (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiDivideScalar_S16 (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiDivideScalar_S16A(const xi_pArray src, xi_pArray dst, int a);

_XI_API_ XI_ERR_TYPE xiDivideScalar_U8_Q    (const xi_pArray src, xi_pArray dst, unsigned a);
_XI_API_ XI_ERR_TYPE xiDivideScalar_U8A_Q   (const xi_pArray src, xi_pArray dst, unsigned a);
_XI_API_ XI_ERR_TYPE xiDivideScalar_U8U16_Q (const xi_pArray src, xi_pArray dst, unsigned a);
_XI_API_ XI_ERR_TYPE xiDivideScalar_U8U16A_Q(const xi_pArray src, xi_pArray dst, unsigned a);
_XI_API_ XI_ERR_TYPE xiDivideScalar_U16_Q   (const xi_pArray src, xi_pArray dst, unsigned a);
_XI_API_ XI_ERR_TYPE xiDivideScalar_U16A_Q  (const xi_pArray src, xi_pArray dst, unsigned a);


/* B = A == a */
_XI_API_ XI_ERR_TYPE xiCompareScalarEQ_I8U8  (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiCompareScalarEQ_I8U8A2(const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiCompareScalarEQ_I16U8 (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiCompareScalarEQ_I16U8A(const xi_pArray src, xi_pArray dst, int a);


/* B = A != a */
_XI_API_ XI_ERR_TYPE xiCompareScalarNE_I8U8  (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiCompareScalarNE_I8U8A2(const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiCompareScalarNE_I16U8 (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiCompareScalarNE_I16U8A(const xi_pArray src, xi_pArray dst, int a);


/* B = A > a */
_XI_API_ XI_ERR_TYPE xiCompareScalarGT_U8    (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiCompareScalarGT_U8A2  (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiCompareScalarGT_S16U8 (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiCompareScalarGT_S16U8A(const xi_pArray src, xi_pArray dst, int a);


/* B = A < a */
_XI_API_ XI_ERR_TYPE xiCompareScalarLT_U8    (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiCompareScalarLT_U8A2  (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiCompareScalarLT_S16U8 (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiCompareScalarLT_S16U8A(const xi_pArray src, xi_pArray dst, int a);


/* B = A >= a */
_XI_API_ XI_ERR_TYPE xiCompareScalarGE_U8    (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiCompareScalarGE_U8A2  (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiCompareScalarGE_S16U8 (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiCompareScalarGE_S16U8A(const xi_pArray src, xi_pArray dst, int a);


/* B = A <= a */
_XI_API_ XI_ERR_TYPE xiCompareScalarLE_U8    (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiCompareScalarLE_U8A2  (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiCompareScalarLE_S16U8 (const xi_pArray src, xi_pArray dst, int a);
_XI_API_ XI_ERR_TYPE xiCompareScalarLE_S16U8A(const xi_pArray src, xi_pArray dst, int a);


/* C = max(A, B) */
_XI_API_ XI_ERR_TYPE xiMax_U8  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMax_U8A2(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMax_S16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMax_S16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);


/* C = min(A, B) */
_XI_API_ XI_ERR_TYPE xiMin_U8  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMin_U8A2(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMin_S16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMin_S16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);


/* C = a * A + b * B + c */
_XI_API_ XI_ERR_TYPE xiAddWeighted_U8  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int a, int b, int c);
_XI_API_ XI_ERR_TYPE xiAddWeighted_U8A (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int a, int b, int c);
_XI_API_ XI_ERR_TYPE xiAddWeighted_U8A2(const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int a, int b, int c);
_XI_API_ XI_ERR_TYPE xiAddWeighted_S16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int a, int b, int c);
_XI_API_ XI_ERR_TYPE xiAddWeighted_S16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int a, int b, int c);

_XI_API_ XI_ERR_TYPE xiAddWeighted_U8_Q15  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q15 a, XI_Q15 b, int c);
_XI_API_ XI_ERR_TYPE xiAddWeighted_U8A_Q15 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q15 a, XI_Q15 b, int c);
_XI_API_ XI_ERR_TYPE xiAddWeighted_S16_Q15 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q15 a, XI_Q15 b, int c);
_XI_API_ XI_ERR_TYPE xiAddWeighted_S16A_Q15(const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q15 a, XI_Q15 b, int c);

_XI_API_ XI_ERR_TYPE xiAddWeighted_U8_Q5_10  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q5_10 a, XI_Q5_10 b, int c);
_XI_API_ XI_ERR_TYPE xiAddWeighted_U8A_Q5_10 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q5_10 a, XI_Q5_10 b, int c);
_XI_API_ XI_ERR_TYPE xiAddWeighted_S16_Q5_10 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q5_10 a, XI_Q5_10 b, int c);
_XI_API_ XI_ERR_TYPE xiAddWeighted_S16A_Q5_10(const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q5_10 a, XI_Q5_10 b, int c);

_XI_API_ XI_ERR_TYPE xiAddWeighted_S_U8  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int a, int b, int c, int fractional_bits XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiAddWeighted_S_U8A (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int a, int b, int c, int fractional_bits XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiAddWeighted_S_S16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int a, int b, int c, int fractional_bits XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiAddWeighted_S_S16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int a, int b, int c, int fractional_bits XI_DEFAULT(0));


/* C = A + B */
_XI_API_ XI_ERR_TYPE xiAdd_I8  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiAdd_I8A2(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiAdd_I16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiAdd_I16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiAdd_U8S16    (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiAdd_U8S16A   (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiAdd_U8S16S16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiAdd_U8S16S16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiAdd_U8S16S32 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiAdd_U8S16S32A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiAdd_S16S32   (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiAdd_S16S32A  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiAdd_S_U8  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiAdd_S_U8A2(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiAdd_S_S16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiAdd_S_S16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiAdd_S_U8S16S16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiAdd_S_U8S16S16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiAdd_S_S16U8(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiAdd_S_S16U8A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);


/* C = A - B */
_XI_API_ XI_ERR_TYPE xiSubtract_I8  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiSubtract_I8A2(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiSubtract_I16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiSubtract_I16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiSubtract_U8S16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiSubtract_U8S16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiSubtract_U8S16S16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiSubtract_U8S16S16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiSubtract_S16U8S16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiSubtract_S16U8S16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiSubtract_U8S16S32 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiSubtract_U8S16S32A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiSubtract_S16U8S32 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiSubtract_S16U8S32A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiSubtract_S16S32   (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiSubtract_S16S32A  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiSubtract_S_U8  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiSubtract_S_U8A2(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiSubtract_S_S16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiSubtract_S_S16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiSubtract_S_S16U8S16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiSubtract_S_S16U8S16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiSubtract_S_U8S16S16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiSubtract_S_U8S16S16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);


/* C = |A - B| */
_XI_API_ XI_ERR_TYPE xiAbsdiff_U8  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiAbsdiff_U8A2(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiAbsdiff_S16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiAbsdiff_S16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiAbsdiff_S_S16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiAbsdiff_S_S16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);


/* C = A * B */
_XI_API_ XI_ERR_TYPE xiMultiply_I8    (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMultiply_I8A2  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMultiply_U8U16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMultiply_U8U16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMultiply_I16   (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMultiply_I16A  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiMultiply_S_U8  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int fractional_bits XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiMultiply_S_U8A2(const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int fractional_bits XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiMultiply_S_S16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int fractional_bits XI_DEFAULT(0));
_XI_API_ XI_ERR_TYPE xiMultiply_S_S16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int fractional_bits XI_DEFAULT(0));


/* C = trunc((A * B) >> s) */
_XI_API_ XI_ERR_TYPE xiMultiplyTrunc_R_U8       (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyTrunc_R_U8A2     (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyTrunc_R_U8S16    (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyTrunc_R_U8S16A   (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyTrunc_R_U8S16S16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyTrunc_R_U8S16S16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyTrunc_R_S16      (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyTrunc_R_S16A     (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int shift);

_XI_API_ XI_ERR_TYPE xiMultiplyTrunc_SR_U8       (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyTrunc_SR_U8A2     (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyTrunc_SR_U8S16    (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyTrunc_SR_U8S16A   (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyTrunc_SR_U8S16S16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyTrunc_SR_U8S16S16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyTrunc_SR_S16      (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyTrunc_SR_S16A     (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, int shift);


/* dst = trunc((src * scalar) >> shift) */
_XI_API_ XI_ERR_TYPE xiMultiplyScalarTrunc_R_U8        (const xi_pArray src, xi_pArray dst, int scalar, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarTrunc_R_U8A2      (const xi_pArray src, xi_pArray dst, int scalar, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarTrunc_R_S16       (const xi_pArray src, xi_pArray dst, int scalar, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarTrunc_R_S16A      (const xi_pArray src, xi_pArray dst, int scalar, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarTrunc_R_U8S16     (const xi_pArray src, xi_pArray dst, int scalar, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarTrunc_R_U8S16A    (const xi_pArray src, xi_pArray dst, int scalar, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarTrunc_R_U8S16S16  (const xi_pArray src, xi_pArray dst, int scalar, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarTrunc_R_U8S16S16A (const xi_pArray src, xi_pArray dst, int scalar, int shift);

_XI_API_ XI_ERR_TYPE xiMultiplyScalarTrunc_SR_U8       (const xi_pArray src, xi_pArray dst, int scalar, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarTrunc_SR_U8A2     (const xi_pArray src, xi_pArray dst, int scalar, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarTrunc_SR_S16      (const xi_pArray src, xi_pArray dst, int scalar, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarTrunc_SR_S16A     (const xi_pArray src, xi_pArray dst, int scalar, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarTrunc_SR_U8S16    (const xi_pArray src, xi_pArray dst, int scalar, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarTrunc_SR_U8S16A   (const xi_pArray src, xi_pArray dst, int scalar, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarTrunc_SR_U8S16S16 (const xi_pArray src, xi_pArray dst, int scalar, int shift);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarTrunc_SR_U8S16S16A(const xi_pArray src, xi_pArray dst, int scalar, int shift);


/* C = round((A * B) / d) */
_XI_API_ XI_ERR_TYPE xiMultiplyRound_D_U8       (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, unsigned divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_D_U8A2     (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, unsigned divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_D_U8S16    (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, unsigned divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_D_U8S16A   (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, unsigned divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_D_U8S16S16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, unsigned divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_D_U8S16S16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst, unsigned divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_D_S16      (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, unsigned divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_D_S16A     (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, unsigned divisor);

_XI_API_ XI_ERR_TYPE xiMultiplyRound_SD_U8       (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, unsigned divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_SD_U8A2     (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, unsigned divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_SD_U8S16    (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, unsigned divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_SD_U8S16A   (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, unsigned divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_SD_U8S16S16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, unsigned divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_SD_U8S16S16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst, unsigned divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_SD_S16      (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, unsigned divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_SD_S16A     (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, unsigned divisor);


/* dst = round((src * scalar) / divisor) */
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_D_U8        (const xi_pArray src, xi_pArray dst, int scalar, unsigned int divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_D_U8A2      (const xi_pArray src, xi_pArray dst, int scalar, unsigned int divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_D_U8S16     (const xi_pArray src, xi_pArray dst, int scalar, unsigned int divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_D_U8S16A    (const xi_pArray src, xi_pArray dst, int scalar, unsigned int divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_D_U8S16S16  (const xi_pArray src, xi_pArray dst, int scalar, unsigned int divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_D_U8S16S16A (const xi_pArray src, xi_pArray dst, int scalar, unsigned int divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_D_S16       (const xi_pArray src, xi_pArray dst, int scalar, unsigned int divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_D_S16A      (const xi_pArray src, xi_pArray dst, int scalar, unsigned int divisor);

_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_SD_U8       (const xi_pArray src, xi_pArray dst, int scalar, unsigned int divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_SD_U8A2     (const xi_pArray src, xi_pArray dst, int scalar, unsigned int divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_SD_U8S16    (const xi_pArray src, xi_pArray dst, int scalar, unsigned int divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_SD_U8S16A   (const xi_pArray src, xi_pArray dst, int scalar, unsigned int divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_SD_U8S16S16 (const xi_pArray src, xi_pArray dst, int scalar, unsigned int divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_SD_U8S16S16A(const xi_pArray src, xi_pArray dst, int scalar, unsigned int divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_SD_S16      (const xi_pArray src, xi_pArray dst, int scalar, unsigned int divisor);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_SD_S16A     (const xi_pArray src, xi_pArray dst, int scalar, unsigned int divisor);

/* C = round((A * B) * s) */
_XI_API_ XI_ERR_TYPE xiMultiplyRound_Q_U8       (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_Q_U8A      (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_Q_U8S16    (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_Q_U8S16A   (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_Q_U8S16S16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_Q_U8S16S16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_Q_S16      (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_Q_S16A     (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q0_16 scale);

_XI_API_ XI_ERR_TYPE xiMultiplyRound_Q_S_U8       (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_Q_S_U8A      (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_Q_S_U8S16    (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_Q_S_U8S16A   (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_Q_S_U8S16S16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_Q_S_U8S16S16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_Q_S_S16      (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyRound_Q_S_S16A     (const xi_pArray src0, const xi_pArray src1, xi_pArray dst, XI_Q0_16 scale);


/* dst = round((src * scalar) * scale) */
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_Q_U8         (const xi_pArray src, xi_pArray dst, int scalar, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_Q_U8A        (const xi_pArray src, xi_pArray dst, int scalar, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_Q_U8S16      (const xi_pArray src, xi_pArray dst, int scalar, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_Q_U8S16A     (const xi_pArray src, xi_pArray dst, int scalar, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_Q_U8S16S16   (const xi_pArray src, xi_pArray dst, int scalar, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_Q_U8S16S16A  (const xi_pArray src, xi_pArray dst, int scalar, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_Q_S16        (const xi_pArray src, xi_pArray dst, int scalar, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_Q_S16A       (const xi_pArray src, xi_pArray dst, int scalar, XI_Q0_16 scale);

_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_Q_S_U8       (const xi_pArray src, xi_pArray dst, int scalar, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_Q_S_U8A      (const xi_pArray src, xi_pArray dst, int scalar, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_Q_S_U8S16    (const xi_pArray src, xi_pArray dst, int scalar, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_Q_S_U8S16A   (const xi_pArray src, xi_pArray dst, int scalar, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_Q_S_U8S16S16 (const xi_pArray src, xi_pArray dst, int scalar, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_Q_S_U8S16S16A(const xi_pArray src, xi_pArray dst, int scalar, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_Q_S_S16      (const xi_pArray src, xi_pArray dst, int scalar, XI_Q0_16 scale);
_XI_API_ XI_ERR_TYPE xiMultiplyScalarRound_Q_S_S16A     (const xi_pArray src, xi_pArray dst, int scalar, XI_Q0_16 scale);

/* C = A / B */
_XI_API_ XI_ERR_TYPE xiDivide_U8  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDivide_U8A (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDivide_S16 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDivide_S16A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiDivide_U8_Q    (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDivide_U8A_Q   (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDivide_U8U16_Q (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDivide_U8U16A_Q(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDivide_U16_Q   (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiDivide_U16A_Q  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);


/* C = A == B */
_XI_API_ XI_ERR_TYPE xiCompareEQ_I8U8  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiCompareEQ_I8U8A2(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiCompareEQ_I16U8 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiCompareEQ_I16U8A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);


/* C = A != B */
_XI_API_ XI_ERR_TYPE xiCompareNE_I8U8  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiCompareNE_I8U8A2(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiCompareNE_I16U8 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiCompareNE_I16U8A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);


/* C = A >= B */
_XI_API_ XI_ERR_TYPE xiCompareGE_U8    (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiCompareGE_U8A2  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiCompareGE_S16U8 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiCompareGE_S16U8A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);


/* C = A <= B */
#define xiCompareLE_U8(src0, src1, dst)     xiCompareGE_U8((src1), (src0), (dst))
#define xiCompareLE_U8A2(src0, src1, dst)   xiCompareGE_U8A2((src1), (src0), (dst))
#define xiCompareLE_S16U8(src0, src1, dst)  xiCompareGE_S16U8((src1), (src0), (dst))
#define xiCompareLE_S16U8A(src0, src1, dst) xiCompareGE_S16U8A((src1), (src0), (dst))


/* C = A > B */
_XI_API_ XI_ERR_TYPE xiCompareGT_U8    (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiCompareGT_U8A2  (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiCompareGT_S16U8 (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiCompareGT_S16U8A(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);


/* C = A < B */
#define xiCompareLT_U8(src0, src1, dst)     xiCompareGT_U8((src1), (src0), (dst))
#define xiCompareLT_S16U8(src0, src1, dst)  xiCompareGT_S16U8((src1), (src0), (dst))
#define xiCompareLT_U8A2(src0, src1, dst)   xiCompareGT_U8A2((src1), (src0), (dst))
#define xiCompareLT_S16U8A(src0, src1, dst) xiCompareGT_S16U8A((src1), (src0), (dst))


/* C = A & B */
_XI_API_ XI_ERR_TYPE xiBitwiseAnd   (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiBitwiseAnd_A2(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);


/* C = A | B */
_XI_API_ XI_ERR_TYPE xiBitwiseOr   (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiBitwiseOr_A2(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);


/* C = A ^ B */
_XI_API_ XI_ERR_TYPE xiBitwiseXor   (const xi_pArray src0, const xi_pArray src1, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiBitwiseXor_A2(const xi_pArray src0, const xi_pArray src1, xi_pArray dst);


/* B = ~A */
_XI_API_ XI_ERR_TYPE xiBitwiseNot   (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiBitwiseNot_A2(const xi_pArray src, xi_pArray dst);


/* B = -A */
#define xiNeg_S16(src, dst)    xiSubtractScalar_I16 ((src), (dst), 0)
#define xiNeg_S16A(src, dst)   xiSubtractScalar_I16A((src), (dst), 0)

#define xiNeg_S_S16(src, dst)  xiSubtractScalar_S_S16 ((src), (dst), 0)
#define xiNeg_S_S16A(src, dst) xiSubtractScalar_S_S16A((src), (dst), 0)

_XI_API_ XI_ERR_TYPE xiNeg_U8S16 (const xi_pArray src, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiNeg_U8S16A(const xi_pArray src, xi_pArray dst);


/* B = |A| */
#define xiAbs_S16(src, dst)    xiAbsdiffScalar_S16 ((src), (dst), 0)
#define xiAbs_S16A(src, dst)   xiAbsdiffScalar_S16A((src), (dst), 0)

#define xiAbs_S_S16(src, dst)  xiAbsdiffScalar_S_S16 ((src), (dst), 0)
#define xiAbs_S_S16A(src, dst) xiAbsdiffScalar_S_S16A((src), (dst), 0)


/* Polar coordinates into Cartesian; x = magnitude * cos(angle), y = magnitude * sin(angle) */
_XI_API_ XI_ERR_TYPE xiPolarToCart_S16 (const xi_pArray magnitude, const xi_pArray angle, xi_pArray x, xi_pArray y);
_XI_API_ XI_ERR_TYPE xiPolarToCart_S16A(const xi_pArray magnitude, const xi_pArray angle, xi_pArray x, xi_pArray y);

_XI_API_ XI_ERR_TYPE xiPolarToCart_S_S16U8 (const xi_pArray magnitude, const xi_pArray angle, xi_pArray x, xi_pArray y);
_XI_API_ XI_ERR_TYPE xiPolarToCart_S_S16U8A(const xi_pArray magnitude, const xi_pArray angle, xi_pArray x, xi_pArray y);


/* Cartesian coordinates into polar; angle = atan(y/x), magnitude = sqrt(x^2 + y^2) */
_XI_API_ XI_ERR_TYPE xiCartToPolar_U8S16_2 (const xi_pArray x, const xi_pArray y, xi_pArray magnitude, xi_pArray angle, const xi_pArray atan_lut);
_XI_API_ XI_ERR_TYPE xiCartToPolar_U8S16A_2(const xi_pArray x, const xi_pArray y, xi_pArray magnitude, xi_pArray angle, const xi_pArray atan_lut);

_XI_API_ XI_ERR_TYPE xiCartToPolar_U8S16 (const xi_pArray x, const xi_pArray y, xi_pArray magnitude, xi_pArray angle);
_XI_API_ XI_ERR_TYPE xiCartToPolar_U8S16A(const xi_pArray x, const xi_pArray y, xi_pArray magnitude, xi_pArray angle);


/* K-means clusterization */
_XI_API_ XI_ERR_TYPE xiKmeans_S16_L2 (const xi_pArray data, xi_pArray labels, xi_pArray centers /*inout*/, uint64_t* compactness, int maxCount, XI_Q1_30 epsilon);
_XI_API_ XI_ERR_TYPE xiKmeans_S16A_L2(const xi_pArray data, xi_pArray labels, xi_pArray centers /*inout*/, uint64_t* compactness, int maxCount, XI_Q1_30 epsilon);


/* Shift Left functions */
_XI_API_ XI_ERR_TYPE xiShiftLeft_I32 (const xi_pArray src, xi_pArray dst, int lsht);
_XI_API_ XI_ERR_TYPE xiShiftLeft_I32A(const xi_pArray src, xi_pArray dst, int lsht);


/* Sort */
_XI_API_ XI_ERR_TYPE xiSort_U8 (const xi_pArray keys, const xi_pArray values, xi_pArray keys_out, xi_pArray values_out, xi_bool ascending XI_DEFAULT(true));
_XI_API_ XI_ERR_TYPE xiSort_U16(xi_pArray keys /*inout*/, xi_pArray values /*inout*/, xi_pArray keys_tmp, xi_pArray values_tmp, xi_bool ascending XI_DEFAULT(true));
_XI_API_ XI_ERR_TYPE xiSort_S16(xi_pArray keys /*inout*/, xi_pArray values /*inout*/, xi_pArray keys_tmp, xi_pArray values_tmp, xi_bool ascending XI_DEFAULT(true));
_XI_API_ XI_ERR_TYPE xiSort_U8_2 (const xi_pArray keys, const xi_pArray values, xi_pArray keys_out, xi_pArray values_out, xi_pArray tmp, xi_bool ascending XI_DEFAULT(true));
_XI_API_ XI_ERR_TYPE xiSort_U16_2(xi_pArray keys /*inout*/, xi_pArray values /*inout*/, xi_pArray keys_tmp, xi_pArray values_tmp, xi_pArray tmp, xi_bool ascending XI_DEFAULT(true));
_XI_API_ XI_ERR_TYPE xiSort_S16_2(xi_pArray keys /*inout*/, xi_pArray values /*inout*/, xi_pArray keys_tmp, xi_pArray values_tmp, xi_pArray tmp, xi_bool ascending XI_DEFAULT(true));


/*2D Matrix Multiply*/
_XI_API_ XI_ERR_TYPE xiMatrixMultiply_U8(const xi_pArray src0, const xi_pArray src1, const xi_pArray src2, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMatrixMultiply_S8(const xi_pArray src0, const xi_pArray src1, const xi_pArray src2, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMatrixMultiply_S16Q8(const xi_pArray src0, const xi_pArray src1, const xi_pArray src2, xi_pArray dst);

_XI_API_ XI_ERR_TYPE xiMatrixMultiply_U8Sa(const xi_pArray src0, const xi_pArray src1, const xi_pArray src2, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMatrixMultiply_S8Sa(const xi_pArray src0, const xi_pArray src1, const xi_pArray src2, xi_pArray dst);
_XI_API_ XI_ERR_TYPE xiMatrixMultiply_S16Q8Sa(const xi_pArray src0, const xi_pArray src1, const xi_pArray src2, xi_pArray dst);

#endif
