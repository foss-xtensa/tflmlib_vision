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
#ifndef __XI_VIDEO_API_H__
#define __XI_VIDEO_API_H__

#include "xi_core_api.h"


/* Accumulate functions */
_XI_API_ XI_ERR_TYPE xiAccumulate_U8U16 (const xi_pArray src, xi_pArray dst /*inout*/, const xi_pArray msk);
_XI_API_ XI_ERR_TYPE xiAccumulate_U8U16A(const xi_pArray src, xi_pArray dst /*inout*/, const xi_pArray msk);
_XI_API_ XI_ERR_TYPE xiAccumulate_I16   (const xi_pArray src, xi_pArray dst /*inout*/, const xi_pArray msk);
_XI_API_ XI_ERR_TYPE xiAccumulate_I16A  (const xi_pArray src, xi_pArray dst /*inout*/, const xi_pArray msk);


/* Accumulate Weighted functions */
_XI_API_ XI_ERR_TYPE xiAccumulateWeighted_U8  (const xi_pArray src, xi_pArray dst /*inout*/, XI_Q15 alpha, const xi_pArray msk);
_XI_API_ XI_ERR_TYPE xiAccumulateWeighted_U8A2(const xi_pArray src, xi_pArray dst /*inout*/, XI_Q15 alpha, const xi_pArray msk);
_XI_API_ XI_ERR_TYPE xiAccumulateWeighted_S16 (const xi_pArray src, xi_pArray dst /*inout*/, XI_Q15 alpha, const xi_pArray msk);
_XI_API_ XI_ERR_TYPE xiAccumulateWeighted_S16A(const xi_pArray src, xi_pArray dst /*inout*/, XI_Q15 alpha, const xi_pArray msk);


/* AccumulateWeightedImage functions */
_XI_API_ XI_ERR_TYPE xiAccumulateWeightedImage_U8  (const xi_pArray src, xi_pArray dst /*inout*/, XI_Q15 alpha);
_XI_API_ XI_ERR_TYPE xiAccumulateWeightedImage_U8A2(const xi_pArray src, xi_pArray dst /*inout*/, XI_Q15 alpha);
_XI_API_ XI_ERR_TYPE xiAccumulateWeightedImage_S16 (const xi_pArray src, xi_pArray dst /*inout*/, XI_Q15 alpha);
_XI_API_ XI_ERR_TYPE xiAccumulateWeightedImage_S16A(const xi_pArray src, xi_pArray dst /*inout*/, XI_Q15 alpha);


/* AccumulateSquare functions */
_XI_API_ XI_ERR_TYPE xiAccumulateSquare_U8U16 (const xi_pArray src, xi_pArray dst /*inout*/, const xi_pArray msk);
_XI_API_ XI_ERR_TYPE xiAccumulateSquare_U8U16A(const xi_pArray src, xi_pArray dst /*inout*/, const xi_pArray msk);
_XI_API_ XI_ERR_TYPE xiAccumulateSquare_I16   (const xi_pArray src, xi_pArray dst /*inout*/, const xi_pArray msk);
_XI_API_ XI_ERR_TYPE xiAccumulateSquare_I16A  (const xi_pArray src, xi_pArray dst /*inout*/, const xi_pArray msk);


/* AccumulateSquareImage functions */
_XI_API_ XI_ERR_TYPE xiAccumulateSquareImage_S_U8S16  (const xi_pArray src, xi_pArray dst /*inout*/, int shift);
_XI_API_ XI_ERR_TYPE xiAccumulateSquareImage_S_U8S16A (const xi_pArray src, xi_pArray dst /*inout*/, int shift);
_XI_API_ XI_ERR_TYPE xiAccumulateSquareImage_S_S16S32 (const xi_pArray src, xi_pArray dst /*inout*/, int shift);
_XI_API_ XI_ERR_TYPE xiAccumulateSquareImage_S_S16S32A(const xi_pArray src, xi_pArray dst /*inout*/, int shift);


/* AccumulateProduct functions */
_XI_API_ XI_ERR_TYPE xiAccumulateProduct_U8S16 (const xi_pArray src1, const xi_pArray src2, xi_pArray dst /*inout*/, const xi_pArray msk);
_XI_API_ XI_ERR_TYPE xiAccumulateProduct_U8S16A(const xi_pArray src1, const xi_pArray src2, xi_pArray dst /*inout*/, const xi_pArray msk);
_XI_API_ XI_ERR_TYPE xiAccumulateProduct_I16   (const xi_pArray src1, const xi_pArray src2, xi_pArray dst /*inout*/, const xi_pArray msk);
_XI_API_ XI_ERR_TYPE xiAccumulateProduct_I16A  (const xi_pArray src1, const xi_pArray src2, xi_pArray dst /*inout*/, const xi_pArray msk);


/* AccumulateImage functions */
_XI_API_ XI_ERR_TYPE xiAccumulateImage_S_U8S16  (const xi_pArray src, xi_pArray dst /*inout*/);
_XI_API_ XI_ERR_TYPE xiAccumulateImage_S_U8S16A (const xi_pArray src, xi_pArray dst /*inout*/);
_XI_API_ XI_ERR_TYPE xiAccumulateImage_S_S16S32 (const xi_pArray src, xi_pArray dst /*inout*/);
_XI_API_ XI_ERR_TYPE xiAccumulateImage_S_S16S32A(const xi_pArray src, xi_pArray dst /*inout*/);
_XI_API_ XI_ERR_TYPE xiAccumulateScalar_S_S16   (xi_pArray dst /*inout*/, int val);
_XI_API_ XI_ERR_TYPE xiAccumulateScalar_S_S16A  (xi_pArray dst /*inout*/, int val);


/* AccumulateScalar functions */
_XI_API_ XI_ERR_TYPE xiAccumulateWeightedScalar_U8(xi_pArray dst, int val, XI_Q15 alpha);
_XI_API_ XI_ERR_TYPE xiAccumulateWeightedScalar_U8A2(xi_pArray dst, int val, XI_Q15 alpha);


/* MeanShift tracking */
_XI_API_ XI_ERR_TYPE xiMeanShift2_U8 (const xi_pTile src, xi_rect* window /*inout*/, int max_steps, int eps2, int* curr_step /*inout*/, xi_point* prev_offset /*inout*/, int frame_width, int frame_height);
_XI_API_ XI_ERR_TYPE xiMeanShift2_S16(const xi_pTile src, xi_rect* window /*inout*/, int max_steps, int eps2, int* curr_step /*inout*/, xi_point* prev_offset /*inout*/, int frame_width, int frame_height);

#define xiMeanShift_U8(src, window, max_steps, eps2) xiMeanShift2_U8(src, window, max_steps, eps2, 0, 0, 0, 0)
#define xiMeanShift_S16(src, window, max_steps, eps2) xiMeanShift2_S16(src, window, max_steps, eps2, 0, 0, 0, 0)


/* CamShift tracking */
_XI_API_ XI_ERR_TYPE xiCamShift2_U8 (const xi_pTile src, xi_rect* start_wnd /*inout*/, xi_rotated_rect* r_wnd, int max_steps, int eps2, int* curr_step, xi_point* prev_offset, int frame_width, int frame_height);
_XI_API_ XI_ERR_TYPE xiCamShift2_S16(const xi_pTile src, xi_rect* start_wnd /*inout*/, xi_rotated_rect* r_wnd, int max_steps, int eps2, int* curr_step, xi_point* prev_offset, int frame_width, int frame_height);

#define xiCamShift_U8(src, start_wnd, r_wnd, max_steps, eps2) xiCamShift2_U8(src, start_wnd, r_wnd, max_steps, eps2, 0, 0, 0, 0)
#define xiCamShift_S16(src, start_wnd, r_wnd, max_steps, eps2) xiCamShift2_S16(src, start_wnd, r_wnd, max_steps, eps2, 0, 0, 0, 0)


/* Background subtractor functions */
_XI_API_ XI_ERR_TYPE xiBS_MOG_U8(const xi_pArray src, xi_pArray fgmask, XI_Q0_15 learningRate,
                                 xi_pArray meanM /*inout*/, xi_pArray weightM /*inout*/, xi_pArray varM /*inout*/);

_XI_API_ XI_ERR_TYPE xiBS_MOG_U8A(const xi_pArray src, xi_pArray fgmask, XI_Q0_15 learningRate,
                                  xi_pArray meanM /*inout*/, xi_pArray weightM /*inout*/, xi_pArray varM /*inout*/);


/* Lucas Kanade optical flow point tracking function */
_XI_API_ XI_ERR_TYPE xiOpticalFlowLK_TrackPoint_U8(const xi_pTile prev, const xi_pTile next, xi_pArray tmp,
                                                   xi_size win_size, XI_Q13_18 fpt_min_eig_threshold,
                                                   XI_Q1_30 fpt_criteria_epsilon, int criteria_max_count,
                                                   xi_point_fpt prevpt, xi_point_fpt* nextpt, xi_bool* status);

_XI_API_ XI_ERR_TYPE xiOpticalFlowLK_TrackPoint_S16(const xi_pTile prev, const xi_pTile next, xi_pArray tmp,
                                                    xi_size win_size, XI_Q13_18 fpt_min_eig_threshold,
                                                    XI_Q1_30 fpt_criteria_epsilon, int criteria_max_count,
                                                    xi_point_fpt prevpt, xi_point_fpt* nextpt, xi_bool* status);


/* size of velocity for OpticalFlowBM */
#define XI_OPTFLOWBM_VEL_SIZE(frame_sz, block_sz, shift_sz) \
        {((frame_sz).width - (block_sz).width + (shift_sz).width) / (shift_sz).width, ((frame_sz).height - (block_sz).height + (shift_sz).height) / (shift_sz).height}

/* Block matching optical flow */
_XI_API_ XI_ERR_TYPE xiCalcOpticalFlowBM_U8S16(const xi_pArray prev, const xi_pTile curr, xi_size blockSize, xi_size shiftSize,
                                               xi_size maxRange, xi_pArray velx, xi_pArray vely);

_XI_API_ XI_ERR_TYPE xiCalcOpticalFlowBM_S16(const xi_pArray prev, const xi_pTile curr, xi_size blockSize, xi_size shiftSize,
                                             xi_size maxRange, xi_pArray velx, xi_pArray vely);


/* Farneback optical flow */
_XI_API_ XI_ERR_TYPE xiGetFarnebacksKernels(XI_Q0_31* kernels, int ksize, double sigma);
_XI_API_ XI_ERR_TYPE xiCalcOpticalFlowFarnebackPolyExp_Q8_7S16(const xi_pTile src, xi_pTile dst, xi_pArray tmp, XI_Q0_31* kernels, int ksize);
_XI_API_ XI_ERR_TYPE xiTakeNextSizesFromOpticalFlow_S32(const xi_pTile _flow, const xi_size framesize, xi_size* nextsiz, xi_point* nextXY);
_XI_API_ XI_ERR_TYPE xiOpticalFlowFarnebackCreateM_S16S32(const xi_pTile _R0,const xi_pTile _R1, const xi_pTile _flow, xi_pTile matM);
_XI_API_ XI_ERR_TYPE xiCalcOpticalFlowFarneback_S32(const xi_pTile matM, xi_pTile _flow, int ksize);

#endif
