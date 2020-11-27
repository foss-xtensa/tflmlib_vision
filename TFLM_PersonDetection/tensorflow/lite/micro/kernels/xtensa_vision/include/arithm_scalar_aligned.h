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

// This file is used as a template for aligned scalar operations.
// Requires following macros to be defined:
// 1) DEPTH_8 or DEPTH_16 or DEPTH_8_16 or DEPTH_16_8 (mutually exclusive)
//    depending on source and destination data depth
// 2) OPERATION(vsrc, vdst) - should transform source vector "vsrc" to destination "vdst"

// The function signature is not included into this template. This template assumes that:
// 1) The source tile argument has name "src"
// 2) The destination tile argument has name "dst"

#ifndef UNROLL
#   if XCHAL_HAVE_VISION && defined(UNROLL_VP5)
#       define UNROLL UNROLL_VP5
#   elif defined(UNROLL_IVPEP)
#       define UNROLL UNROLL_IVPEP
#   else
#       define UNROLL 2
#   endif
#endif

#include "arithm.h"

    XI_ERROR_CHECKS()
    {
        CHECK_ARRAY(src, SRC_CHECK_TYPE);
        CHECK_ARRAY(dst, DST_CHECK_TYPE);
        XI_CHECK_ARRAY_SIZE_EQ(src, dst);

        CHECK_ARRAY_ALIGNMENT(src, SRC_DEPTH, SRC_REAL_DEPTH, IALIGNMENT);
        CHECK_ARRAY_ALIGNMENT(dst, DST_DEPTH, DST_REAL_DEPTH, OALIGNMENT);

#ifdef CHECK_ELEMENT_SIZE_EQ
        XI_CHECK_ARRAY_ELEMENT_SIZE_EQ(src, dst);
#endif

        EXTRA_ERROR_CHECKS();
    }

    const int sstride = XI_ARRAY_GET_PITCH(src) * SRC_REAL_DEPTH;
    const int dstride = XI_ARRAY_GET_PITCH(dst) * DST_REAL_DEPTH;
    const int width   = XI_ARRAY_GET_WIDTH(src) * SRC_REAL_DEPTH;
    const int height  = XI_ARRAY_GET_HEIGHT(src);

    SRC_TYPE* psrc = (SRC_TYPE*)XI_ARRAY_GET_DATA_PTR(src);
    DST_TYPE* pdst = (DST_TYPE*)XI_ARRAY_GET_DATA_PTR(dst);

    SRC_VEC* restrict vpsrc;
    DST_VEC* restrict vpdst;

    int x = 0;

#if UNROLL > 3
    xb_vecNx16U vsrc0, vsrc1, vsrc2, vsrc3;
    xb_vecNx16 vdst0, vdst1, vdst2, vdst3;
    for (; x <= width - 4 * XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH; x += 4 * XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH)
    {
        vpsrc = (SRC_VEC*)OFFSET_PTR_NX8U(psrc, 0, sstride, x);
        vpdst = (DST_VEC*)OFFSET_PTR_NX8U(pdst, 0, dstride, x * DST_DEPTH / SRC_DEPTH);
        for (int y = 0; y < height; ++y)
        {
            IVP_LVNXDD_IP(SRC_DEPTH, vsrc0, vpsrc, SRC_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            IVP_LVNXDD_IP(SRC_DEPTH, vsrc1, vpsrc, SRC_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            IVP_LVNXDD_IP(SRC_DEPTH, vsrc2, vpsrc, SRC_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            XI_IVP_LVNXDD_XP(SRC_DEPTH, vsrc3, vpsrc, sstride - 3 * SRC_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            OPERATION(vsrc0, vdst0);
            OPERATION(vsrc1, vdst1);
            OPERATION(vsrc2, vdst2);
            OPERATION(vsrc3, vdst3);
            IVP_SVNXDD_IP(DST_DEPTH, vdst0, vpdst, DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            IVP_SVNXDD_IP(DST_DEPTH, vdst1, vpdst, DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            IVP_SVNXDD_IP(DST_DEPTH, vdst2, vpdst, DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            XI_IVP_SVNXDD_XP(DST_DEPTH, vdst3, vpdst, dstride - 3 * DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
        }
    }
#endif

#if UNROLL > 2
#if UNROLL == 3
    xb_vecNx16U vsrc0, vsrc1, vsrc2;
    xb_vecNx16 vdst0, vdst1, vdst2;
    for (; x <= width - 3 * XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH; x += 3 * XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH)
#else
    if (x < width - 2 * XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH)
#endif
    {
        vpsrc = (SRC_VEC*)OFFSET_PTR_NX8U(psrc, 0, sstride, x);
        vpdst = (DST_VEC*)OFFSET_PTR_NX8U(pdst, 0, dstride, x * DST_DEPTH / SRC_DEPTH);
        for (int y = 0; y < height; ++y)
        {
            IVP_LVNXDD_IP(SRC_DEPTH, vsrc0, vpsrc, SRC_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            IVP_LVNXDD_IP(SRC_DEPTH, vsrc1, vpsrc, SRC_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            XI_IVP_LVNXDD_XP(SRC_DEPTH, vsrc2, vpsrc, sstride - 2 * SRC_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            OPERATION(vsrc0, vdst0);
            OPERATION(vsrc1, vdst1);
            OPERATION(vsrc2, vdst2);
            IVP_SVNXDD_IP(DST_DEPTH, vdst0, vpdst, DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            IVP_SVNXDD_IP(DST_DEPTH, vdst1, vpdst, DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            XI_IVP_SVNXDD_XP(DST_DEPTH, vdst2, vpdst, dstride - 2 * DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
        }
    }
#endif

#if UNROLL > 1
#if UNROLL == 2
    xb_vecNx16U vsrc0, vsrc1;
    xb_vecNx16 vdst0, vdst1;
    for (; x <= width - 2 * XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH; x += 2 * XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH)
#elif UNROLL > 3
    else if (x < width - XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH)
#else
    if (x < width - XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH)
#endif
    {
        vpsrc = (SRC_VEC*)OFFSET_PTR_NX8U(psrc, 0, sstride, x);
        vpdst = (DST_VEC*)OFFSET_PTR_NX8U(pdst, 0, dstride, x * DST_DEPTH / SRC_DEPTH);
        for (int y = 0; y < height; ++y)
        {
            IVP_LVNXDD_IP(SRC_DEPTH, vsrc0, vpsrc, SRC_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            XI_IVP_LVNXDD_XP(SRC_DEPTH, vsrc1, vpsrc, sstride - SRC_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            OPERATION(vsrc0, vdst0);
            OPERATION(vsrc1, vdst1);
            IVP_SVNXDD_IP(DST_DEPTH, vdst0, vpdst, DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            XI_IVP_SVNXDD_XP(DST_DEPTH, vdst1, vpdst, dstride - DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
        }
    }
#endif

#if UNROLL == 1
    xb_vecNx16U vsrc0;
    xb_vecNx16 vdst0;
    for (; x <= width - XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH; x += XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH)
#elif UNROLL > 2
    else if (x < width)
#else
    if (x < width)
#endif
    {
        vpsrc = (SRC_VEC*)OFFSET_PTR_NX8U(psrc, 0, sstride, x);
        vpdst = (DST_VEC*)OFFSET_PTR_NX8U(pdst, 0, dstride, x * DST_DEPTH / SRC_DEPTH);
        for (int y = 0; y < height; ++y)
        {
            XI_IVP_LVNXDD_XP(SRC_DEPTH, vsrc0, vpsrc, sstride);
            OPERATION(vsrc0, vdst0);
            XI_IVP_SVNXDD_XP(DST_DEPTH, vdst0, vpdst, dstride);
        }
    }

    return XI_ERROR_STATUS();

#undef UNROLL
#include "arithm.h"
