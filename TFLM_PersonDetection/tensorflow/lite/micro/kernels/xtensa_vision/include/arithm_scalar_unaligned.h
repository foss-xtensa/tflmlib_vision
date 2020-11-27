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
// This file is used as a template for unaligned scalar operations.
// Requires following macros to be defined:
// 1) DEPTH_8 or DEPTH_16 or DEPTH_8_16 or DEPTH_16_8 (mutually exclusive)
//    depending on source and destination data depth
// 2) OPERATION(vsrc, vdst) - should transform source vector "vsrc" to destination "vdst"

// The function signature is not included into this template. This template assumes that:
// 1) The source tile argument has name "src"
// 2) The destination tile argument has name "dst"

#include "arithm.h"

    XI_ERROR_CHECKS()
    {
        CHECK_ARRAY(src, SRC_CHECK_TYPE);
        CHECK_ARRAY(dst, DST_CHECK_TYPE);
        XI_CHECK_ARRAY_SIZE_EQ(src, dst);

        EXTRA_ERROR_CHECKS();
    }

    const int sstride = XI_ARRAY_GET_PITCH(src) * SRC_REAL_DEPTH;
    const int dstride = XI_ARRAY_GET_PITCH(dst) * DST_REAL_DEPTH;
    const int width   = XI_ARRAY_GET_WIDTH(src) * SRC_REAL_DEPTH;
    const int height  = XI_ARRAY_GET_HEIGHT(src);

    SRC_TYPE* pdatasrc = (SRC_TYPE*)XI_ARRAY_GET_DATA_PTR(src);
    DST_TYPE* pdatadst = (DST_TYPE*)XI_ARRAY_GET_DATA_PTR(dst);

    SRC_VEC* restrict vpsrc;
    DST_VEC* restrict vpdst;

    xb_vecNx16U vsrc0, vsrc1, vsrc2, vsrc3;
    xb_vecNx16 vdst0, vdst1, vdst2, vdst3;

    valign va_src, va_dst = IVP_ZALIGN();

    int x = 0;
    for (; x < width - 3 * XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH; x += 4 * XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH)
    {
#ifdef PTR_POSTINCR_UNROLL_4
        vpsrc = (SRC_VEC*)OFFSET_PTR_NX8U(pdatasrc, 0, sstride, x);
        vpdst = (DST_VEC*)OFFSET_PTR_NX8U(pdatadst, 0, dstride, x * DST_DEPTH / SRC_DEPTH);

        int offs  = sstride - XT_MIN(4 * XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH, width - x);
        int offd  = dstride - XT_MIN(4 * XCHAL_IVPN_SIMD_WIDTH * DST_DEPTH,  DST_DEPTH * (width - x) / SRC_DEPTH);

        for (int y = 0; y < height; ++y)
        {
#ifdef L2AU_UNROLL_4
            IVP_L2AUNXDD_IP(SRC_DEPTH, vsrc0, va_src, vpsrc);
            IVP_LANXDD_IP (SRC_DEPTH, vsrc1, va_src, vpsrc);
            IVP_LANXDD_IP (SRC_DEPTH, vsrc2, va_src, vpsrc);
#elif defined(L2A_UNROLL_4)
            IVP_L2AUNXDD_IP(SRC_DEPTH, vsrc0, va_src, vpsrc);
            IVP_L2A2NXDD_IP(SRC_DEPTH, vsrc2, vsrc1, va_src, vpsrc);
#else
            va_src = IVP_LANXDD_PP(SRC_DEPTH, vpsrc);
            IVP_LANXDD_IP (SRC_DEPTH, vsrc0, va_src, vpsrc);
            IVP_LANXDD_IP (SRC_DEPTH, vsrc1, va_src, vpsrc);
            IVP_LANXDD_IP (SRC_DEPTH, vsrc2, va_src, vpsrc);
#endif
            IVP_LAVNXDD_XP(SRC_DEPTH, vsrc3, va_src, vpsrc, width - x - 3 * XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH);
            OPERATION(vsrc0, vdst0);
            OPERATION(vsrc1, vdst1);
            OPERATION(vsrc2, vdst2);
            OPERATION(vsrc3, vdst3);
            IVP_SANXDD_IP (DST_DEPTH, vdst0, va_dst, vpdst);
            IVP_SANXDD_IP (DST_DEPTH, vdst1, va_dst, vpdst);
            IVP_SANXDD_IP (DST_DEPTH, vdst2, va_dst, vpdst);
            IVP_SAVNXDD_XP(DST_DEPTH, vdst3, va_dst, vpdst, DST_DEPTH * (width - x - 3 * XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH) / SRC_DEPTH);
            IVP_SAVNXDDPOS_FP(DST_DEPTH, va_dst, vpdst);

            vpsrc = (SRC_VEC*)OFFSET_PTR_NX8U(vpsrc, 1, offs, 0);
            vpdst = (DST_VEC*)OFFSET_PTR_NX8U(vpdst, 1, offd, 0);
        }
#else
        SRC_TYPE* psrc = (SRC_TYPE*)OFFSET_PTR_NX8U(pdatasrc, 0, sstride, x);
        DST_TYPE* pdst = (DST_TYPE*)OFFSET_PTR_NX8U(pdatadst, 0, dstride, x * DST_DEPTH / SRC_DEPTH);
        for (int y = 0; y < height; ++y, psrc = (SRC_TYPE*)OFFSET_PTR_NX8U(psrc, 1, sstride, 0), pdst = (DST_TYPE*)OFFSET_PTR_NX8U(pdst, 1, dstride, 0))
        {
            vpsrc = (SRC_VEC*)psrc;
            vpdst = (DST_VEC*)pdst;
#ifdef L2AU_UNROLL_4
            IVP_L2AUNXDD_IP(SRC_DEPTH, vsrc0, va_src, vpsrc);
            IVP_LANXDD_IP (SRC_DEPTH, vsrc1, va_src, vpsrc);
            IVP_LANXDD_IP (SRC_DEPTH, vsrc2, va_src, vpsrc);
#elif defined(L2A_UNROLL_4)
            IVP_L2AUNXDD_IP(SRC_DEPTH, vsrc0, va_src, vpsrc);
            IVP_L2A2NXDD_IP(SRC_DEPTH, vsrc2, vsrc1, va_src, vpsrc);
#else
            va_src = IVP_LANXDD_PP(SRC_DEPTH, vpsrc);
            IVP_LANXDD_IP (SRC_DEPTH, vsrc0, va_src, vpsrc);
            IVP_LANXDD_IP (SRC_DEPTH, vsrc1, va_src, vpsrc);
            IVP_LANXDD_IP (SRC_DEPTH, vsrc2, va_src, vpsrc);
#endif
            IVP_LAVNXDD_XP(SRC_DEPTH, vsrc3, va_src, vpsrc, width - x - 3 * XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH);
            OPERATION(vsrc0, vdst0);
            OPERATION(vsrc1, vdst1);
            OPERATION(vsrc2, vdst2);
            OPERATION(vsrc3, vdst3);
            IVP_SANXDD_IP (DST_DEPTH, vdst0, va_dst, vpdst);
            IVP_SANXDD_IP (DST_DEPTH, vdst1, va_dst, vpdst);
            IVP_SANXDD_IP (DST_DEPTH, vdst2, va_dst, vpdst);
            IVP_SAVNXDD_XP(DST_DEPTH, vdst3, va_dst, vpdst, DST_DEPTH * (width - x - 3 * XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH) / SRC_DEPTH);
            IVP_SAVNXDDPOS_FP(DST_DEPTH, va_dst, vpdst);
        }
#endif
    }

    if (x < width - 2 * XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH)
    {
#ifdef PTR_POSTINCR_UNROLL_3
        vpsrc = (SRC_VEC*)OFFSET_PTR_NX8U(pdatasrc, 0, sstride, x);
        vpdst = (DST_VEC*)OFFSET_PTR_NX8U(pdatadst, 0, dstride, x * DST_DEPTH / SRC_DEPTH);

        int offs  = sstride - XT_MIN(3 * XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH, width - x);
        int offd  = dstride - XT_MIN(3 * XCHAL_IVPN_SIMD_WIDTH * DST_DEPTH,  DST_DEPTH * (width - x) / SRC_DEPTH);

        for (int y = 0; y < height; ++y)
        {
#ifdef L2AU_UNROLL_3
            IVP_L2AUNXDD_IP(SRC_DEPTH, vsrc0, va_src, vpsrc);
#else
            va_src = IVP_LANXDD_PP(SRC_DEPTH, vpsrc);
            IVP_LANXDD_IP (SRC_DEPTH, vsrc0, va_src, vpsrc);
#endif
            IVP_LANXDD_IP (SRC_DEPTH, vsrc1, va_src, vpsrc);
            IVP_LAVNXDD_XP(SRC_DEPTH, vsrc2, va_src, vpsrc, width - x - 2 * XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH);
            OPERATION(vsrc0, vdst0);
            OPERATION(vsrc1, vdst1);
            OPERATION(vsrc2, vdst2);
            IVP_SANXDD_IP (DST_DEPTH, vdst0, va_dst, vpdst);
            IVP_SANXDD_IP (DST_DEPTH, vdst1, va_dst, vpdst);
            IVP_SAVNXDD_XP(DST_DEPTH, vdst2, va_dst, vpdst, DST_DEPTH * (width - x - 2 * XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH) / SRC_DEPTH);
            IVP_SAVNXDDPOS_FP(DST_DEPTH, va_dst, vpdst);

            vpsrc = (SRC_VEC*)OFFSET_PTR_NX8U(vpsrc, 1, offs, 0);
            vpdst = (DST_VEC*)OFFSET_PTR_NX8U(vpdst, 1, offd, 0);
        }
#else
        SRC_TYPE* psrc = (SRC_TYPE*)OFFSET_PTR_NX8U(pdatasrc, 0, sstride, x);
        DST_TYPE* pdst = (DST_TYPE*)OFFSET_PTR_NX8U(pdatadst, 0, dstride, x * DST_DEPTH / SRC_DEPTH);
        for (int y = 0; y < height; ++y, psrc = (SRC_TYPE*)OFFSET_PTR_NX8U(psrc, 1, sstride, 0), pdst = (DST_TYPE*)OFFSET_PTR_NX8U(pdst, 1, dstride, 0))
        {
            vpsrc = (SRC_VEC*)psrc;
            vpdst = (DST_VEC*)pdst;
#ifdef L2AU_UNROLL_3
            IVP_L2AUNXDD_IP(SRC_DEPTH, vsrc0, va_src, vpsrc);
#else
            va_src = IVP_LANXDD_PP(SRC_DEPTH, vpsrc);
            IVP_LANXDD_IP (SRC_DEPTH, vsrc0, va_src, vpsrc);
#endif
            IVP_LANXDD_IP (SRC_DEPTH, vsrc1, va_src, vpsrc);
            IVP_LAVNXDD_XP(SRC_DEPTH, vsrc2, va_src, vpsrc, width - x - 2 * XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH);
            OPERATION(vsrc0, vdst0);
            OPERATION(vsrc1, vdst1);
            OPERATION(vsrc2, vdst2);
            IVP_SANXDD_IP (DST_DEPTH, vdst0, va_dst, vpdst);
            IVP_SANXDD_IP (DST_DEPTH, vdst1, va_dst, vpdst);
            IVP_SAVNXDD_XP(DST_DEPTH, vdst2, va_dst, vpdst, DST_DEPTH * (width - x - 2 * XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH) / SRC_DEPTH);
            IVP_SAVNXDDPOS_FP(DST_DEPTH, va_dst, vpdst);
        }
#endif
    }
    else if (x < width - XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH)
    {
#ifdef PTR_POSTINCR_UNROLL_2
        vpsrc = (SRC_VEC*)OFFSET_PTR_NX8U(pdatasrc, 0, sstride, x);
        vpdst = (DST_VEC*)OFFSET_PTR_NX8U(pdatadst, 0, dstride, x * DST_DEPTH / SRC_DEPTH);

        int offs  = sstride - XT_MIN(2 * XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH, width - x);
        int offd  = dstride - XT_MIN(2 * XCHAL_IVPN_SIMD_WIDTH * DST_DEPTH,  DST_DEPTH * (width - x) / SRC_DEPTH);

        for (int y = 0; y < height; ++y)
        {
            va_src = IVP_LANXDD_PP(SRC_DEPTH, vpsrc);
            IVP_LANXDD_IP (SRC_DEPTH, vsrc0, va_src, vpsrc);
            IVP_LAVNXDD_XP(SRC_DEPTH, vsrc1, va_src, vpsrc, width - x - XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH);
            OPERATION(vsrc0, vdst0);
            OPERATION(vsrc1, vdst1);
            IVP_SANXDD_IP (DST_DEPTH, vdst0, va_dst, vpdst);
            IVP_SAVNXDD_XP(DST_DEPTH, vdst1, va_dst, vpdst, DST_DEPTH * (width - x - XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH) / SRC_DEPTH);
            IVP_SAVNXDDPOS_FP(DST_DEPTH, va_dst, vpdst);

            vpsrc = (SRC_VEC*)OFFSET_PTR_NX8U(vpsrc, 1, offs, 0);
            vpdst = (DST_VEC*)OFFSET_PTR_NX8U(vpdst, 1, offd, 0);
        }
#else
        SRC_TYPE* psrc = (SRC_TYPE*)OFFSET_PTR_NX8U(pdatasrc, 0, sstride, x);
        DST_TYPE* pdst = (DST_TYPE*)OFFSET_PTR_NX8U(pdatadst, 0, dstride, x * DST_DEPTH / SRC_DEPTH);
        for (int y = 0; y < height; ++y, psrc = (SRC_TYPE*)OFFSET_PTR_NX8U(psrc, 1, sstride, 0), pdst = (DST_TYPE*)OFFSET_PTR_NX8U(pdst, 1, dstride, 0))
        {
            vpsrc = (SRC_VEC*)psrc;
            vpdst = (DST_VEC*)pdst;
            va_src = IVP_LANXDD_PP(SRC_DEPTH, vpsrc);
            IVP_LANXDD_IP (SRC_DEPTH, vsrc0, va_src, vpsrc);
            IVP_LAVNXDD_XP(SRC_DEPTH, vsrc1, va_src, vpsrc, width - x - XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH);
            OPERATION(vsrc0, vdst0);
            OPERATION(vsrc1, vdst1);
            IVP_SANXDD_IP (DST_DEPTH, vdst0, va_dst, vpdst);
            IVP_SAVNXDD_XP(DST_DEPTH, vdst1, va_dst, vpdst, DST_DEPTH * (width - x - XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH) / SRC_DEPTH);
            IVP_SAVNXDDPOS_FP(DST_DEPTH, va_dst, vpdst);
        }
#endif
    }
    else if (x < width)
    {
#ifdef PTR_POSTINCR_UNROLL_1
        vpsrc = (SRC_VEC*)OFFSET_PTR_NX8U(pdatasrc, 0, sstride, x);
        vpdst = (DST_VEC*)OFFSET_PTR_NX8U(pdatadst, 0, dstride, x * DST_DEPTH / SRC_DEPTH);

        int offs  = sstride - XT_MIN(XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH, width - x);
        int offd  = dstride - XT_MIN(XCHAL_IVPN_SIMD_WIDTH * DST_DEPTH,  DST_DEPTH * (width - x) / SRC_DEPTH);

        for (int y = 0; y < height; ++y)
        {
            va_src = IVP_LANXDD_PP(SRC_DEPTH, vpsrc);
            IVP_LAVNXDD_XP(SRC_DEPTH, vsrc0, va_src, vpsrc, width - x);
            OPERATION(vsrc0, vdst0);
            IVP_SAVNXDD_XP(DST_DEPTH, vdst0, va_dst, vpdst, DST_DEPTH * (width - x) / SRC_DEPTH);
            IVP_SAVNXDDPOS_FP(DST_DEPTH, va_dst, vpdst);

            vpsrc = (SRC_VEC*)OFFSET_PTR_NX8U(vpsrc, 1, offs, 0);
            vpdst = (DST_VEC*)OFFSET_PTR_NX8U(vpdst, 1, offd, 0);
        }
#else
        SRC_TYPE* psrc = (SRC_TYPE*)OFFSET_PTR_NX8U(pdatasrc, 0, sstride, x);
        DST_TYPE* pdst = (DST_TYPE*)OFFSET_PTR_NX8U(pdatadst, 0, dstride, x * DST_DEPTH / SRC_DEPTH);
        for (int y = 0; y < height; ++y, psrc = (SRC_TYPE*)OFFSET_PTR_NX8U(psrc, 1, sstride, 0), pdst = (DST_TYPE*)OFFSET_PTR_NX8U(pdst, 1, dstride, 0))
        {
            vpsrc = (SRC_VEC*)psrc;
            vpdst = (DST_VEC*)pdst;
            va_src = IVP_LANXDD_PP(SRC_DEPTH, vpsrc);
            IVP_LAVNXDD_XP(SRC_DEPTH, vsrc0, va_src, vpsrc, width - x);
            OPERATION(vsrc0, vdst0);
            IVP_SAVNXDD_XP(DST_DEPTH, vdst0, va_dst, vpdst, DST_DEPTH * (width - x) / SRC_DEPTH);
            IVP_SAVNXDDPOS_FP(DST_DEPTH, va_dst, vpdst);
        }
#endif
    }

    return XI_ERROR_STATUS();

#include "arithm.h"
