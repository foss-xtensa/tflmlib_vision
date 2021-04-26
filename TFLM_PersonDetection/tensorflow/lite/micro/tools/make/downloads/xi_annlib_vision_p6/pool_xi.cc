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
#define INCLUDE_XI_CNN
#include "xi_cnn.h"
#include "xi_intrin.h"
#include "xi_cnn_api.h"

#define max(a, b)                      ((a > b) ? a : b)
#define min(a, b)                      ((a > b) ? b : a)

// input vector: xb_vecNx16 vecInput1, vecInput0
// output vector: xb_vec2Nx8U dvecOutput
// int32_t output_zero_point (positive value)
// int32_t output_multiplier
// int32_t output_shift
// int32_t left_shift
// int32_t activation_min
// int32_t activation_max

#define REQUANTIZE_POOLING_OUTPUT(vecInput1, vecInput0, dvecOutput,                                                                    \
                                  output_zero_point, output_multiplier, output_shift, left_shift, activation_min, activation_max)      \
  {                                                                                                                                    \
                                                                                                                                       \
    xb_vecN_2x32v hvecInp1_0 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0(vecInput0, (xb_vecN_2x32v) output_multiplier), (31 - left_shift)); \
    xb_vecN_2x32v hvecInp1_1 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_1(vecInput0, (xb_vecN_2x32v) output_multiplier), (31 - left_shift)); \
    xb_vecN_2x32v hvecOut0   = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16) 1, IVP_ABSN_2X32(hvecInp1_0)), output_shift);         \
    hvecOut0 = IVP_MULSGNN_2X32(hvecInp1_0, hvecOut0);                                                                                 \
    hvecOut0 = hvecOut0 + (xb_vecN_2x32v) output_zero_point;                                                                           \
    xb_vecN_2x32v hvecOut1 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16) 1, IVP_ABSN_2X32(hvecInp1_1)), output_shift);           \
    hvecOut1 = IVP_MULSGNN_2X32(hvecInp1_1, hvecOut1);                                                                                 \
    hvecOut1 = hvecOut1 + (xb_vecN_2x32v) output_zero_point;                                                                           \
    hvecOut0 = IVP_MINN_2X32((xb_vecN_2x32v) activation_max, IVP_MAXN_2X32(hvecOut0, (xb_vecN_2x32v) activation_min));                 \
    hvecOut1 = IVP_MINN_2X32((xb_vecN_2x32v) activation_max, IVP_MAXN_2X32(hvecOut1, (xb_vecN_2x32v) activation_min));                 \
    xb_vecNx16 vecOut0 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecOut1, hvecOut0, IVP_SELI_16B_INTERLEAVE_1_EVEN));                   \
                                                                                                                                       \
    hvecInp1_0 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0(vecInput1, (xb_vecN_2x32v) output_multiplier), (31 - left_shift));               \
    hvecInp1_1 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_1(vecInput1, (xb_vecN_2x32v) output_multiplier), (31 - left_shift));               \
    hvecOut0   = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16) 1, IVP_ABSN_2X32(hvecInp1_0)), output_shift);                       \
    hvecOut0   = IVP_MULSGNN_2X32(hvecInp1_0, hvecOut0);                                                                               \
    hvecOut0   = hvecOut0 + (xb_vecN_2x32v) output_zero_point;                                                                         \
    hvecOut1   = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16) 1, IVP_ABSN_2X32(hvecInp1_1)), output_shift);                       \
    hvecOut1   = IVP_MULSGNN_2X32(hvecInp1_1, hvecOut1);                                                                               \
    hvecOut1   = hvecOut1 + (xb_vecN_2x32v) output_zero_point;                                                                         \
    hvecOut0   = IVP_MINN_2X32((xb_vecN_2x32v) activation_max, IVP_MAXN_2X32(hvecOut0, (xb_vecN_2x32v) activation_min));               \
    hvecOut1   = IVP_MINN_2X32((xb_vecN_2x32v) activation_max, IVP_MAXN_2X32(hvecOut1, (xb_vecN_2x32v) activation_min));               \
    xb_vecNx16 vecOut1 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecOut1, hvecOut0, IVP_SELI_16B_INTERLEAVE_1_EVEN));                   \
                                                                                                                                       \
    dvecOutput = IVP_SEL2NX8I(IVP_MOV2NX8U_FROMNX16(vecOut1), IVP_MOV2NX8U_FROMNX16(vecOut0), IVP_SELI_8B_INTERLEAVE_1_EVEN);          \
  }

#define INPUT_DATA_TYPE  UNSIGNED8BIT
#include "avg_poolingAQuantized.h"
#undef INPUT_DATA_TYPE

#define INPUT_DATA_TYPE  SIGNED8BIT
#include "avg_poolingAQuantized.h"
#undef INPUT_DATA_TYPE

#define INPUT_DATA_TYPE  INTEGER8BIT
#include "extend_edge.h"
#undef INPUT_DATA_TYPE

//#define INPUT_DATA_TYPE  INTEGER16BIT
//#include "extend_edge.h"
//#undef INPUT_DATA_TYPE

#define INPUT_DATA_TYPE  INTEGER8BIT
#include "fill_tile.h"
#undef INPUT_DATA_TYPE

//#define INPUT_DATA_TYPE  INTEGER16BIT
//#include "fill_tile.h"
//#undef INPUT_DATA_TYPE
