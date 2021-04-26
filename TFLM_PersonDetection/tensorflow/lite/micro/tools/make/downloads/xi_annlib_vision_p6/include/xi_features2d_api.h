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
#ifndef __XI_FEATURES2D_API_H__
#define __XI_FEATURES2D_API_H__

#include "xi_core_api.h"


/* BRIEF descriptor */
_XI_API_ XI_ERR_TYPE xiBRIEF_48x48_init(xi_pArray pattern);

_XI_API_ XI_ERR_TYPE xiBRIEF16_48x48_S16U8_3(const xi_pTile src, xi_point keypoint, uint8_t* descriptor, const xi_pArray pattern);
_XI_API_ XI_ERR_TYPE xiBRIEF16_48x48_U16U8_3(const xi_pTile src, xi_point keypoint, uint8_t* descriptor, const xi_pArray pattern);

_XI_API_ XI_ERR_TYPE xiBRIEF32_48x48_S16U8_3(const xi_pTile src, xi_point keypoint, uint8_t* descriptor, const xi_pArray pattern);
_XI_API_ XI_ERR_TYPE xiBRIEF32_48x48_U16U8_3(const xi_pTile src, xi_point keypoint, uint8_t* descriptor, const xi_pArray pattern);

_XI_API_ XI_ERR_TYPE xiBRIEF64_48x48_S16U8_3(const xi_pTile src, xi_point keypoint, uint8_t* descriptor, const xi_pArray pattern);
_XI_API_ XI_ERR_TYPE xiBRIEF64_48x48_U16U8_3(const xi_pTile src, xi_point keypoint, uint8_t* descriptor, const xi_pArray pattern);


_XI_API_ XI_ERR_TYPE xiBRIEF16_48x48_S16U8_2(const xi_pTile src, xi_point keypoint, uint8_t* descriptor);
_XI_API_ XI_ERR_TYPE xiBRIEF16_48x48_U16U8_2(const xi_pTile src, xi_point keypoint, uint8_t* descriptor);

_XI_API_ XI_ERR_TYPE xiBRIEF32_48x48_S16U8_2(const xi_pTile src, xi_point keypoint, uint8_t* descriptor);
_XI_API_ XI_ERR_TYPE xiBRIEF32_48x48_U16U8_2(const xi_pTile src, xi_point keypoint, uint8_t* descriptor);

_XI_API_ XI_ERR_TYPE xiBRIEF64_48x48_S16U8_2(const xi_pTile src, xi_point keypoint, uint8_t* descriptor);
_XI_API_ XI_ERR_TYPE xiBRIEF64_48x48_U16U8_2(const xi_pTile src, xi_point keypoint, uint8_t* descriptor);


/* Harris corner detector */
_XI_API_ XI_ERR_TYPE xiCornerHarris_3x3_3x3_U8S16(const xi_pTile src, xi_pArray dst, xi_pArray tmp, XI_Q15 k_sqrt XI_DEFAULT(6554));
_XI_API_ XI_ERR_TYPE xiCornerHarris_3x3_3x3_S16  (const xi_pTile src, xi_pArray dst, xi_pArray tmp, XI_Q15 k_sqrt XI_DEFAULT(6554));
_XI_API_ XI_ERR_TYPE xiCornerHarris_3x3_5x5_U8S16(const xi_pTile src, xi_pArray dst, xi_pArray tmp, XI_Q15 k_sqrt XI_DEFAULT(6554));

_XI_API_ XI_ERR_TYPE xiCornerHarris_U8S16(const xi_pTile src, xi_pArray dst, xi_pArray tmp, int block_size, int ksize, XI_Q15 k_sqrt XI_DEFAULT(6554));
_XI_API_ XI_ERR_TYPE xiCornerHarris_S16  (const xi_pTile src, xi_pArray dst, xi_pArray tmp, int block_size, int ksize, XI_Q15 k_sqrt XI_DEFAULT(6554));

_XI_API_ XI_ERR_TYPE xiGetScaleCornerHarris2_U8S16(int block_size, int ksize, double *scale);
_XI_API_ XI_ERR_TYPE xiCornerHarris2_U8S16(const xi_pTile src, xi_pArray dst, xi_pArray tmp, int block_size, int ksize, XI_Q15 k_sqrt XI_DEFAULT(6554));

_XI_API_ XI_ERR_TYPE xiCornerHarrisNonMaxima(xi_pArray src_p /*inout*/, xi_pArray src_v /*inout*/, int min_dist);


/* Shi-Tomasi corner detector */
_XI_API_ XI_ERR_TYPE xiCornerShiTomasi_3x3_3x3_U8S16(const xi_pTile src, xi_pArray dst, xi_pArray tmp);
_XI_API_ XI_ERR_TYPE xiCornerShiTomasi_3x3_3x3_S16  (const xi_pTile src, xi_pArray dst, xi_pArray tmp);

_XI_API_ XI_ERR_TYPE xiCornerShiTomasi_U8S16(const xi_pTile src, xi_pArray dst, xi_pArray tmp, int block_size, int ksize);
_XI_API_ XI_ERR_TYPE xiCornerShiTomasi_S16  (const xi_pTile src, xi_pArray dst, xi_pArray tmp, int block_size, int ksize);


/* FAST corner detector */
_XI_API_ XI_ERR_TYPE xiFAST_U8        (const xi_pTile src, xi_pArray score, int threshold);
_XI_API_ XI_ERR_TYPE xiFAST_U8MSOa2   (const xi_pTile src, xi_pArray score, int threshold);
_XI_API_ XI_ERR_TYPE xiFAST_S16U16    (const xi_pTile src, xi_pArray score, int threshold);
_XI_API_ XI_ERR_TYPE xiFAST_S16U16MSOa(const xi_pTile src, xi_pArray score, int threshold);

_XI_API_ XI_ERR_TYPE xiFAST2_U8        (const xi_pTile src, xi_pArray score, int threshold, xi_pArray tmp);
_XI_API_ XI_ERR_TYPE xiFAST2_S16U16    (const xi_pTile src, xi_pArray score, int threshold, xi_pArray tmp);


/* extract keypoints */
_XI_API_ XI_ERR_TYPE xiExtractKeypoints_U8 (const xi_pTile responce_map, xi_pArray keypoints, int* count);
_XI_API_ XI_ERR_TYPE xiExtractKeypoints_U8A(const xi_pTile responce_map, xi_pArray keypoints, int* count);
_XI_API_ XI_ERR_TYPE xiExtractNextKeypoints_U8 (const xi_pTile responce_map, xi_point32 start_loc, xi_pArray keypoints, int* count);
_XI_API_ XI_ERR_TYPE xiExtractNextKeypoints_U8A(const xi_pTile responce_map, xi_point32 start_loc, xi_pArray keypoints, int* count);


/* For each line of query tile searches nearest neighbour(s) among lines of train */
_XI_API_ XI_ERR_TYPE xiKnnSearch1_L1_U8     (const xi_pArray query, const xi_pArray train, xi_pArray matches, xi_pArray distance);
_XI_API_ XI_ERR_TYPE xiKnnSearch1_L2S_U8    (const xi_pArray query, const xi_pArray train, xi_pArray matches, xi_pArray distance);
_XI_API_ XI_ERR_TYPE xiKnnSearch1_Hamming_U8(const xi_pArray query, const xi_pArray train, xi_pArray matches, xi_pArray distance);

_XI_API_ XI_ERR_TYPE xiKnnSearch1_L1_S16    (const xi_pArray query, const xi_pArray train, xi_pArray matches, xi_pArray distance);
_XI_API_ XI_ERR_TYPE xiKnnSearch1_L2S_S16   (const xi_pArray query, const xi_pArray train, xi_pArray matches, xi_pArray distance);

_XI_API_ XI_ERR_TYPE xiKnnSearch2_L1_U8     (const xi_pArray query, const xi_pArray train, xi_pArray matches, xi_pArray distance);
_XI_API_ XI_ERR_TYPE xiKnnSearch2_L2S_U8    (const xi_pArray query, const xi_pArray train, xi_pArray matches, xi_pArray distance);
_XI_API_ XI_ERR_TYPE xiKnnSearch2_Hamming_U8(const xi_pArray query, const xi_pArray train, xi_pArray matches, xi_pArray distance);

_XI_API_ XI_ERR_TYPE xiKnnSearch2_L1_S16    (const xi_pArray query, const xi_pArray train, xi_pArray matches, xi_pArray distance);
_XI_API_ XI_ERR_TYPE xiKnnSearch2_L2S_S16   (const xi_pArray query, const xi_pArray train, xi_pArray matches, xi_pArray distance);


/* extract points */
_XI_API_ XI_ERR_TYPE xiExtractPoints_U8(const xi_pTile responce_map, int threshold, xi_pArray points, xi_pArray weight, int* count);
_XI_API_ XI_ERR_TYPE xiExtractPoints_S8(const xi_pTile responce_map, int threshold, xi_pArray points, xi_pArray weight, int* count);

_XI_API_ XI_ERR_TYPE xiExtractPoints_U16(const xi_pTile responce_map, int threshold, xi_pArray points, xi_pArray weight, int* count);
_XI_API_ XI_ERR_TYPE xiExtractPoints_S16(const xi_pTile responce_map, int threshold, xi_pArray points, xi_pArray weight, int* count);
_XI_API_ XI_ERR_TYPE xiExtractNextPoints_U16(const xi_pTile src, xi_point start_point, int threshold, xi_pArray points, xi_pArray weight, int* count);
_XI_API_ XI_ERR_TYPE xiExtractNextPoints_S16(const xi_pTile src, xi_point start_loc,   int threshold, xi_pArray points, xi_pArray weight, int* count);


#endif
