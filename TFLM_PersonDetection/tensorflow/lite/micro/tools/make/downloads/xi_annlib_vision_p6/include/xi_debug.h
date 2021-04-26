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
#ifndef __XI_DEBUG_H__
#define __XI_DEBUG_H__

#include "xi_core.h"
#include <math.h>
#include <stdio.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/*
 This header provides functions useful for debugging
*/

static inline int xi_min(int a, int b){ return (a<b) ? a : b; }
static inline int xi_max(int a, int b){ return (a>b) ? a : b; }

static inline void xiPrintData_U8(uint8_t *data, int size)
{
    for (int i = 0; i < size; i++)
        printf("%d, ", data[i]);
    printf("\n");
}

static inline void xiPrintData_S16(int16_t *data, int size)
{
    for (int i = 0; i < size; i++)
        printf("%d, ", data[i]);
    printf("\n");
}

// ------------------ N_2 x bool ------------------

#if XCHAL_HAVE_VISION

static inline void print_b16(vboolN_2 v)
{
    uint32_t vec32;

    IVP_SBN_2_I(v, (vboolN_2*)&vec32, 0);

    if ((vec32 & 1) == 1)
    {
        printf("[ t");
    }
    else
    {
        printf("[ f");
    }

    for (int i = 1; i < XCHAL_IVPN_SIMD_WIDTH/2; ++i)
    {
        if ((vec32 & (1u << i)) == (1u << i))
        {
            printf("t");
        }
        else
        {
            printf("f");
        }
    }

    printf(" ]\n");
}

#endif

// ------------------ N x bool ------------------

static inline void print_b32(vboolN v)
{
    uint32_t vec32;

    IVP_SBN_I(v, (vboolN*)&vec32, 0);

    if ((vec32 & 1) == 1)
    {
        printf("[ t");
    }
    else
    {
        printf("[ f");
    }

    for (int i = 1; i < XCHAL_IVPN_SIMD_WIDTH; ++i)
    {
        if ((vec32 & (1u << i)) == (1u << i))
        {
            printf("t");
        }
        else
        {
            printf("f");
        }
    }

    printf(" ]\n");
}

// ------------------ 2N x bool ------------------

static inline void print_b64(vbool2N v)
{
    uint64_t vec64;

    IVP_SB2N_I(v, (vbool2N*)&vec64, 0);

    if ((vec64 & 1) == 1)
    {
        printf("[ t");
    }
    else
    {
        printf("[ f");
    }

    for (int i = 1; i < 2 * XCHAL_IVPN_SIMD_WIDTH; ++i)
    {
        if ((vec64 & ((uint64_t)1 << i)) == ((uint64_t)1 << i))
        {
            printf("t");
        }
        else
        {
            printf("f");
        }
    }

    printf(" ]\n");
}

// ------------------ N x 16 ------------------

static inline void print_nx16s_range(xb_vecNx16 v, int from, int to)
{
    int16_t vec[XCHAL_IVPN_SIMD_WIDTH];
    IVP_SVNX16_I(v, (xb_vecNx16*)vec, 0);

    if (to < from)
    {
        int tmp = from;
        from = to;
        to = tmp;
    }

    from = xi_max(from, 0);
    to = xi_min(to, XCHAL_IVPN_SIMD_WIDTH);

    printf("[%6d", vec[from]);

    for (int i = from + 1; i < to; ++i)
        printf(", %6d", vec[i]);

    printf("]\n");
}

static inline void print_nx16u_range(xb_vecNx16U v, int from, int to)
{
    uint16_t vec[XCHAL_IVPN_SIMD_WIDTH];
    IVP_SVNX16_I(v, (xb_vecNx16*)vec, 0);

    if (to < from)
    {
        int tmp = from;
        from = to;
        to = tmp;
    }

    from = xi_max(from, 0);
    to = xi_min(to, XCHAL_IVPN_SIMD_WIDTH);

    printf("[%5u", vec[from]);

    for (int i = from + 1; i < to; ++i)
        printf(", %5u", vec[i]);

    printf("]\n");
}

static inline void print_nx16_qa_B_range(xb_vecNx16 v, int b, int from, int to)
{
    int16_t vec[XCHAL_IVPN_SIMD_WIDTH];
    IVP_SVNX16_I(v, (xb_vecNx16 *)vec, 0);

    if (to < from)
    {
        int tmp = from;
        from = to;
        to = tmp;
    }

    from = xi_max(from, 0);
    to = xi_min(to, XCHAL_IVPN_SIMD_WIDTH);

    printf("[%12.6f", (float)vec[from] / (1 << b));

    for (int i = from + 1; i < to; ++i)
        printf(", %12.6f", (float)vec[i] / (1 << b));

    printf("]\n");
}

static inline void print_nx16u(xb_vecNx16U v)
{
    print_nx16u_range(v, 0, XCHAL_IVPN_SIMD_WIDTH);
}

static inline void print_nx16s(xb_vecNx16 v)
{
    print_nx16s_range(v, 0, XCHAL_IVPN_SIMD_WIDTH);
}

static inline void print_nx16_qa_B(xb_vecNx16 v, int b)
{
    print_nx16_qa_B_range(v, b, 0, XCHAL_IVPN_SIMD_WIDTH);
}

// ------------------ 2N x 8 ------------------

static inline void print_2nx8u_range(xb_vec2Nx8U v, int from, int to)
{
    uint8_t vec[2 * XCHAL_IVPN_SIMD_WIDTH];
    IVP_SV2NX8_I(v, (xb_vec2Nx8*)vec, 0);

    if (to < from)
    {
        int tmp = from;
        from = to;
        to = tmp;
    }

    from = xi_max(from, 0);
    to = xi_min(to, 2 * XCHAL_IVPN_SIMD_WIDTH);

    printf("[%3u", vec[from]);

    for (int i = from + 1; i < to; ++i)
        printf(", %3u", vec[i]);

    printf("]\n");
}

static inline void print_2nx8s_range(xb_vec2Nx8U v, int from, int to)
{
    int8_t vec[2 * XCHAL_IVPN_SIMD_WIDTH];
    IVP_SV2NX8_I(v, (xb_vec2Nx8*)vec, 0);

    if (to < from)
    {
        int tmp = from;
        from = to;
        to = tmp;
    }

    from = xi_max(from, 0);
    to = xi_min(to, 2 * XCHAL_IVPN_SIMD_WIDTH);

    printf("[%4d", vec[from]);

    for (int i = from + 1; i < to; ++i)
        printf(", %4d", vec[i]);

    printf("]\n");
}

static inline void print_2nx8u(xb_vec2Nx8U v)
{
    print_2nx8u_range(v, 0, 2 * XCHAL_IVPN_SIMD_WIDTH);
}

static inline void print_2nx8s(xb_vec2Nx8 v)
{
    print_2nx8s_range(v, 0, 2 * XCHAL_IVPN_SIMD_WIDTH);
}

// ------------------ N/2 x 32 ------------------

static inline void print_xn16x2_32u(xb_vecNx16 v_h, xb_vecNx16 v_l)
{
    uint16_t vec_h[XCHAL_IVPN_SIMD_WIDTH];
    uint16_t vec_l[XCHAL_IVPN_SIMD_WIDTH];
    IVP_SVNX16_I(v_h, (xb_vecNx16*)vec_h, 0);
    IVP_SVNX16_I(v_l, (xb_vecNx16*)vec_l, 0);
    printf("[%u", (vec_h[0] << 16) + vec_l[0]);
    for (int i = 1; i < XCHAL_IVPN_SIMD_WIDTH; ++i)
        printf(", %u", (vec_h[i] << 16) + vec_l[i]);
    printf("]\n");
}

static inline void print_xn16x2_32s(xb_vecNx16 v_h, xb_vecNx16 v_l)
{
    uint16_t vec_h[XCHAL_IVPN_SIMD_WIDTH];
    uint16_t vec_l[XCHAL_IVPN_SIMD_WIDTH];
    IVP_SVNX16_I(v_h, (xb_vecNx16*)vec_h, 0);
    IVP_SVNX16_I(v_l, (xb_vecNx16*)vec_l, 0);
    printf("[%d", (vec_h[0] << 16) + vec_l[0]);
    for (int i = 1; i < XCHAL_IVPN_SIMD_WIDTH; ++i)
        printf(", %d", (vec_h[i] << 16) + vec_l[i]);
    printf("]\n");
}

// ------------------ 3 x N x 16 ------------------

static inline void print_xn16x3_u48(xb_vecNx16 v_hh, xb_vecNx16 v_h, xb_vecNx16 v_l)
{
    uint16_t vec_hh[XCHAL_IVPN_SIMD_WIDTH];
    uint16_t vec_h[XCHAL_IVPN_SIMD_WIDTH];
    uint16_t vec_l[XCHAL_IVPN_SIMD_WIDTH];
    IVP_SVNX16_I(v_hh, (xb_vecNx16*)vec_hh, 0);
    IVP_SVNX16_I(v_h, (xb_vecNx16*)vec_h, 0);
    IVP_SVNX16_I(v_l, (xb_vecNx16*)vec_l, 0);
    {
        uint64_t val = vec_hh[0];
        val <<= 16;
        val += vec_h[0];
        val <<= 16;
        val += vec_l[0];
        printf("[%16" PRIu64, val);
    }
    for (int i = 1; i < XCHAL_IVPN_SIMD_WIDTH; ++i)
    {
        uint64_t val = vec_hh[i];
        val <<= 16;
        val += vec_h[i];
        val <<= 16;
        val += vec_l[i];
        printf(", %16" PRIu64, val);
    }
    printf("]\n");
}

// ------------------ N x 48 ------------------

static inline void print_nx48_range(xb_vecNx48 v, int from, int to)
{
    int64_t vec[XCHAL_IVPN_SIMD_WIDTH];
    xb_vecNx48_storei(v, (xb_vecNx48*)vec, 0);

    if (to < from)
    {
        int tmp = from;
        from = to;
        to = tmp;
    }

    from = xi_max(from, 0);
    to = xi_min(to, XCHAL_IVPN_SIMD_WIDTH);

    printf("[%16" PRIi64, vec[from]);

    for (int i = from + 1; i < to; ++i)
        printf(", %16" PRIi64, vec[i]);

    printf("]\n");
}

static inline void print_nx48_qa_B_range(xb_vecNx48 a, int b, int from, int to)
{
    int64_t vec[XCHAL_IVPN_SIMD_WIDTH];
    xb_vecNx48_storei(a, (xb_vecNx48*)vec, 0);

    if (to < from)
    {
        int tmp = from;
        from = to;
        to = tmp;
    }

    from = xi_max(from, 0);
    to = xi_min(to, XCHAL_IVPN_SIMD_WIDTH);

    printf("[%16.8f", (double)vec[from] / (1 << b));

    for (int i = from + 1; i < to; ++i)
        printf(", %16.8f", (double)vec[i] / (1 << b));

    printf("]\n");
}

static inline void print_nx48(xb_vecNx48 a)
{
    print_nx48_range(a, 0, XCHAL_IVPN_SIMD_WIDTH);
}

static inline void print_nx48_qa_B(xb_vecNx48 a, int b)
{
    print_nx48_qa_B_range(a, b, 0, XCHAL_IVPN_SIMD_WIDTH);
}

// ------------------ 2N x 24 ------------------

static inline void print_2nx24_range(xb_vec2Nx24 v, int from, int to)
{
    int32_t vec[2*XCHAL_IVPN_SIMD_WIDTH];
    xb_vec2Nx24_storei(v, (xb_vec2Nx24*)vec, 0);

    if (to < from)
    {
        int tmp = from;
        from = to;
        to = tmp;
    }

    from = xi_max(from, 0);
    to = xi_min(to, 2 * XCHAL_IVPN_SIMD_WIDTH);

    printf("[%9d", vec[from]);

    for (int i = from + 1; i < to; ++i)
        printf(", %9d", vec[i]);

    printf("]\n");
}

static inline void print_2nx24_qa_B_range(xb_vec2Nx24 v, int b, int from, int to)
{
    int32_t vec[2*XCHAL_IVPN_SIMD_WIDTH];
    xb_vec2Nx24_storei(v, (xb_vec2Nx24*)vec, 0);

    if (to < from)
    {
        int tmp = from;
        from = to;
        to = tmp;
    }

    from = xi_max(from, 0);
    to = xi_min(to, 2 * XCHAL_IVPN_SIMD_WIDTH);

    printf("[%14.6f", (float)vec[from] / (1 << b));

    for (int i = from + 1; i < to; ++i)
        printf(", %14.6f", (float)vec[i] / (1 << b));

    printf("]\n");
}

static inline void print_2nx24(xb_vec2Nx24 a)
{
    print_2nx24_range(a, 0, 2 * XCHAL_IVPN_SIMD_WIDTH);
}

static inline void print_2nx24_qa_B(xb_vec2Nx24 a, int b)
{
    print_2nx24_qa_B_range(a, b, 0, 2 * XCHAL_IVPN_SIMD_WIDTH);
}

// ------------------ N/2 x 32 ------------------

static inline void print_n_2x32s_range(xb_vecN_2x32v v, int from, int to)
{
    int32_t vec[XCHAL_IVPN_SIMD_WIDTH/2];
    xb_vecN_2x32v_storei(v, (xb_vecN_2x32v*)vec, 0);

    if (to < from)
    {
        int tmp = from;
        from = to;
        to = tmp;
    }

    from = xi_max(from, 0);
    to = xi_min(to, XCHAL_IVPN_SIMD_WIDTH/2);

    printf("[%12d", (int)vec[from]);

    for (int i = from + 1; i < to; ++i)
        printf(", %12d", (int)vec[i]);

    printf("]\n");
}

static inline void print_n_2x32u_range(xb_vecN_2x32v v, int from, int to)
{
    uint32_t vec[XCHAL_IVPN_SIMD_WIDTH/2];
    xb_vecN_2x32v_storei(v, (xb_vecN_2x32v*)vec, 0);

    if (to < from)
    {
        int tmp = from;
        from = to;
        to = tmp;
    }

    from = xi_max(from, 0);
    to = xi_min(to, XCHAL_IVPN_SIMD_WIDTH/2);

    printf("[%11u", (unsigned)vec[from]);

    for (int i = from + 1; i < to; ++i)
        printf(", %11u", (unsigned)vec[i]);

    printf("]\n");
}

static inline void print_n_2x32s(xb_vecN_2x32v a)
{
    print_n_2x32s_range(a, 0, XCHAL_IVPN_SIMD_WIDTH/2);
}

static inline void print_n_2x32u(xb_vecN_2x32Uv a)
{
    print_n_2x32u_range(a, 0, XCHAL_IVPN_SIMD_WIDTH/2);
}

// ------------------ N/2 x 64 ------------------

#if XCHAL_HAVE_VISION

static inline void print_n_2x64u_range(xb_vecN_2x64w a, int from, int to)
{
    uint64_t vec[XCHAL_IVPN_SIMD_WIDTH/2];
    xb_vecN_2x64w_storei(a, (xb_vecN_2x64w*)vec, 0);

    if (to < from)
    {
        int tmp = from;
        from = to;
        to = tmp;
    }

    from = xi_max(from, 0);
    to = xi_min(to, XCHAL_IVPN_SIMD_WIDTH/2);

    printf("[%20" PRIu64, (uint64_t)vec[from]);

    for (int i = from + 1; i < to; ++i)
        printf(", %20" PRIu64, (uint64_t)vec[i]);

    printf("]\n");
}

static inline void print_n_2x64s_range(xb_vecN_2x64w a, int from, int to)
{
    int64_t vec[XCHAL_IVPN_SIMD_WIDTH/2];
    xb_vecN_2x64w_storei(a, (xb_vecN_2x64w*)vec, 0);

    if (to < from)
    {
        int tmp = from;
        from = to;
        to = tmp;
    }

    from = xi_max(from, 0);
    to = xi_min(to, XCHAL_IVPN_SIMD_WIDTH/2);

    printf("[%21" PRIi64, (int64_t)vec[from]);

    for (int i = from + 1; i < to; ++i)
        printf(", %21" PRIi64, (int64_t)vec[i]);

    printf("]\n");
}

static inline void print_n_2x64s(xb_vecN_2x64w a)
{
    print_n_2x64s_range(a, 0, XCHAL_IVPN_SIMD_WIDTH/2);
}

static inline void print_n_2x64u(xb_vecN_2x64w a)
{
    print_n_2x64u_range(a, 0, XCHAL_IVPN_SIMD_WIDTH/2);
}

#endif

#endif
