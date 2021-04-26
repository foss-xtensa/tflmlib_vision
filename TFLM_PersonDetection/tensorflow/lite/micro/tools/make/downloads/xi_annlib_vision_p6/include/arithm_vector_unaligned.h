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
// This file is used as a template for unaligned vector operations.
// Requires following macros to be defined:
// 1) DEPTH_8 or DEPTH_16 or DEPTH_8_16 or DEPTH_16_8 (mutually exclusive)
//    depending on source and destination data depth
// 2) OPERATION(vsrc0, vsrc1, vdst) - should transform source vectors "vsrc0" and "vsrc1" to destination "vdst"

// The function signature is not included into this template. This template assumes that:
// 1) The source tile arguments have names "src0" and "src1"
// 2) The destination tile argument has name "dst"

#ifndef UNROLL
#   if XCHAL_HAVE_VISION && (XCHAL_VISION_TYPE >= 6) && defined(UNROLL_VP6)
#       define UNROLL UNROLL_VP6
#   elif XCHAL_HAVE_VISION && defined(UNROLL_VP5)
#       define UNROLL UNROLL_VP5
#   elif defined(UNROLL_IVPEP)
#       define UNROLL UNROLL_IVPEP
#   else
#       define UNROLL 4
#   endif
#endif

#include "arithm.h"

    XI_ERROR_CHECKS()
    {
        CHECK_ARRAY(src0, SRC0_CHECK_TYPE);
        CHECK_ARRAY(src1, SRC1_CHECK_TYPE);
        CHECK_ARRAY(dst, DST_CHECK_TYPE);
        XI_CHECK_ARRAY_SIZE_EQ(src0, dst);
        XI_CHECK_ARRAY_SIZE_EQ(src1, dst);

        EXTRA_ERROR_CHECKS();
    }

    const int sstride0 = XI_ARRAY_GET_PITCH(src0) * SRC0_REAL_DEPTH;
    const int sstride1 = XI_ARRAY_GET_PITCH(src1) * SRC1_REAL_DEPTH;
    const int dstride  = XI_ARRAY_GET_PITCH(dst)  * DST_REAL_DEPTH;
    const int width    = XI_ARRAY_GET_WIDTH(src0) * SRC0_REAL_DEPTH;
    const int height   = XI_ARRAY_GET_HEIGHT(src0);

    SRC0_TYPE* pdatasrc0 = (SRC0_TYPE*)XI_ARRAY_GET_DATA_PTR(src0);
    SRC1_TYPE* pdatasrc1 = (SRC1_TYPE*)XI_ARRAY_GET_DATA_PTR(src1);
    DST_TYPE* pdatadst   = (DST_TYPE*) XI_ARRAY_GET_DATA_PTR(dst);

    SRC0_VEC* __restrict vpsrc0;
    SRC1_VEC* __restrict vpsrc1;
    DST_VEC*  __restrict vpdst;

    valign va_src0, va_src1, va_dst = IVP_ZALIGN();
    int x = 0;

#if UNROLL > 3
    xb_vecNx16U vsrc00, vsrc01, vsrc02, vsrc03, vsrc10, vsrc11, vsrc12, vsrc13;
    xb_vecNx16 vdst0, vdst1, vdst2, vdst3;
    for (; x < width - 3 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH; x += 4 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH)
    {
#ifdef PTR_POSTINCR_UNROLL_4
        vpsrc0 = (SRC0_VEC*)OFFSET_PTR_NX8U(pdatasrc0, 0, sstride0, x);
        vpsrc1 = (SRC1_VEC*)OFFSET_PTR_NX8U(pdatasrc1, 0, sstride1, x * SRC1_DEPTH / SRC0_DEPTH);
        vpdst = (DST_VEC*)OFFSET_PTR_NX8U(pdatadst, 0, dstride, x * DST_DEPTH / SRC0_DEPTH);

        int offs0 = sstride0 - XT_MIN(4 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH, width - x);
        int offs1 = sstride1 - XT_MIN(4 * XCHAL_IVPN_SIMD_WIDTH * SRC1_DEPTH, SRC1_DEPTH * (width - x) / SRC0_DEPTH);
        int offd  = dstride  - XT_MIN(4 * XCHAL_IVPN_SIMD_WIDTH * DST_DEPTH,  DST_DEPTH * (width - x) / SRC0_DEPTH);

        for (int y = 0; y < height; ++y)
        {
#ifdef L2A_UNROLL_4
            IVP_L2AUNXDD_IP(SRC0_DEPTH, vsrc00,         va_src0, vpsrc0);
            IVP_L2A2NXDD_IP(SRC0_DEPTH, vsrc02, vsrc01, va_src0, vpsrc0);

            IVP_L2AUNXDD_IP(SRC1_DEPTH, vsrc10,         va_src1, vpsrc1);
            IVP_L2A2NXDD_IP(SRC1_DEPTH, vsrc12, vsrc11, va_src1, vpsrc1);
#elif defined(L2AU_UNROLL_4)
            IVP_L2AUNXDD_IP(SRC0_DEPTH, vsrc00, va_src0, vpsrc0);
            IVP_LANXDD_IP  (SRC0_DEPTH, vsrc01, va_src0, vpsrc0);
            IVP_LANXDD_IP  (SRC0_DEPTH, vsrc02, va_src0, vpsrc0);

            IVP_L2AUNXDD_IP(SRC1_DEPTH, vsrc10, va_src1, vpsrc1);
            IVP_LANXDD_IP  (SRC1_DEPTH, vsrc11, va_src1, vpsrc1);
            IVP_LANXDD_IP  (SRC1_DEPTH, vsrc12, va_src1, vpsrc1);
#elif defined(LA2NX8_UNROLL_4)
            va_src0 = IVP_LANXDD_PP(SRC0_DEPTH, vpsrc0);
            xb_vec2Nx8U* vpsrc_2n = (xb_vec2Nx8U*)vpsrc0;
            xb_vec2Nx8 tmp; IVP_LA2NX8U_IP(tmp, va_src0, vpsrc_2n);
            vpsrc0 = (xb_vecNx8U*)vpsrc_2n;
            IVP_DSELNX16I(vsrc01, vsrc00, 0, IVP_MOVNX16_FROM2NX8(tmp), IVP_DSELI_8B_INTERLEAVE_1);
            IVP_LANXDD_IP(SRC0_DEPTH, vsrc02, va_src0, vpsrc0);

            va_src1 = IVP_LANXDD_PP(SRC1_DEPTH, vpsrc1);
            IVP_LANXDD_IP (SRC1_DEPTH, vsrc10, va_src1, vpsrc1);
            IVP_LANXDD_IP (SRC1_DEPTH, vsrc11, va_src1, vpsrc1);
            IVP_LANXDD_IP (SRC1_DEPTH, vsrc12, va_src1, vpsrc1);
#else
            va_src0 = IVP_LANXDD_PP(SRC0_DEPTH, vpsrc0);
            IVP_LANXDD_IP (SRC0_DEPTH, vsrc00, va_src0, vpsrc0);
            IVP_LANXDD_IP (SRC0_DEPTH, vsrc01, va_src0, vpsrc0);
            IVP_LANXDD_IP (SRC0_DEPTH, vsrc02, va_src0, vpsrc0);

            va_src1 = IVP_LANXDD_PP(SRC1_DEPTH, vpsrc1);
            IVP_LANXDD_IP(SRC1_DEPTH, vsrc10, va_src1, vpsrc1);
            IVP_LANXDD_IP(SRC1_DEPTH, vsrc11, va_src1, vpsrc1);
            IVP_LANXDD_IP(SRC1_DEPTH, vsrc12, va_src1, vpsrc1);
#endif
            IVP_LAVNXDD_XP(SRC0_DEPTH, vsrc03, va_src0, vpsrc0,               width - x - 3 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH);
            IVP_LAVNXDD_XP(SRC1_DEPTH, vsrc13, va_src1, vpsrc1, SRC1_DEPTH * (width - x - 3 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH) / SRC0_DEPTH);

            OPERATION(vsrc00, vsrc10, vdst0);
            OPERATION(vsrc01, vsrc11, vdst1);
            OPERATION(vsrc02, vsrc12, vdst2);
            OPERATION(vsrc03, vsrc13, vdst3);
            IVP_SANXDD_IP (DST_DEPTH, vdst0, va_dst, vpdst);
            IVP_SANXDD_IP (DST_DEPTH, vdst1, va_dst, vpdst);
            IVP_SANXDD_IP (DST_DEPTH, vdst2, va_dst, vpdst);
            IVP_SAVNXDD_XP(DST_DEPTH, vdst3, va_dst, vpdst, DST_DEPTH * (width - x - 3 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH) / SRC0_DEPTH);
            IVP_SAVNXDDPOS_FP(DST_DEPTH, va_dst, vpdst);

            vpsrc0 = (SRC0_VEC*)OFFSET_PTR_NX8U(vpsrc0, 1, offs0, 0);
            vpsrc1 = (SRC1_VEC*)OFFSET_PTR_NX8U(vpsrc1, 1, offs1, 0);
            vpdst = (DST_VEC*)OFFSET_PTR_NX8U(vpdst, 1, offd, 0);
        }
#else
        SRC0_TYPE* psrc0 = (SRC0_TYPE*)OFFSET_PTR_NX8U(pdatasrc0, 0, sstride0, x);
        SRC1_TYPE* psrc1 = (SRC1_TYPE*)OFFSET_PTR_NX8U(pdatasrc1, 0, sstride1, x * SRC1_DEPTH / SRC0_DEPTH);
        DST_TYPE* pdst = (DST_TYPE*)OFFSET_PTR_NX8U(pdatadst, 0, dstride, x * DST_DEPTH / SRC0_DEPTH);

        for (int y = 0; y < height; ++y, psrc0 = (SRC0_TYPE*)OFFSET_PTR_NX8U(psrc0, 1, sstride0, 0),
                                         psrc1 = (SRC1_TYPE*)OFFSET_PTR_NX8U(psrc1, 1, sstride1, 0),
                                         pdst = (DST_TYPE*)OFFSET_PTR_NX8U(pdst, 1, dstride, 0))
        {
            vpsrc0 = (SRC0_VEC*)psrc0;
            vpsrc1 = (SRC1_VEC*)psrc1;
            vpdst  = (DST_VEC*) pdst;
#ifdef L2A_UNROLL_4
            IVP_L2AUNXDD_IP(SRC0_DEPTH, vsrc00,         va_src0, vpsrc0);
            IVP_L2A2NXDD_IP(SRC0_DEPTH, vsrc02, vsrc01, va_src0, vpsrc0);

            IVP_L2AUNXDD_IP(SRC1_DEPTH, vsrc10,         va_src1, vpsrc1);
            IVP_L2A2NXDD_IP(SRC1_DEPTH, vsrc12, vsrc11, va_src1, vpsrc1);
#elif defined(L2AU_UNROLL_4)
            IVP_L2AUNXDD_IP(SRC0_DEPTH, vsrc00, va_src0, vpsrc0);
            IVP_LANXDD_IP  (SRC0_DEPTH, vsrc01, va_src0, vpsrc0);
            IVP_LANXDD_IP  (SRC0_DEPTH, vsrc02, va_src0, vpsrc0);

            IVP_L2AUNXDD_IP(SRC1_DEPTH, vsrc10, va_src1, vpsrc1);
            IVP_LANXDD_IP  (SRC1_DEPTH, vsrc11, va_src1, vpsrc1);
            IVP_LANXDD_IP  (SRC1_DEPTH, vsrc12, va_src1, vpsrc1);
#elif defined(LA2NX8_UNROLL_4)
            va_src0 = IVP_LANXDD_PP(SRC0_DEPTH, vpsrc0);
            xb_vec2Nx8U* vpsrc_2n = (xb_vec2Nx8U*)vpsrc0;
            xb_vec2Nx8 tmp; IVP_LA2NX8U_IP(tmp, va_src0, vpsrc_2n);
            vpsrc0 = (xb_vecNx8U*)vpsrc_2n;
            IVP_DSELNX16I(vsrc01, vsrc00, 0, IVP_MOVNX16_FROM2NX8(tmp), IVP_DSELI_8B_INTERLEAVE_1);
            IVP_LANXDD_IP(SRC0_DEPTH, vsrc02, va_src0, vpsrc0);

            va_src1 = IVP_LANXDD_PP(SRC1_DEPTH, vpsrc1);
            IVP_LANXDD_IP (SRC1_DEPTH, vsrc10, va_src1, vpsrc1);
            IVP_LANXDD_IP (SRC1_DEPTH, vsrc11, va_src1, vpsrc1);
            IVP_LANXDD_IP (SRC1_DEPTH, vsrc12, va_src1, vpsrc1);
#else
            va_src0 = IVP_LANXDD_PP(SRC0_DEPTH, vpsrc0);
            IVP_LANXDD_IP (SRC0_DEPTH, vsrc00, va_src0, vpsrc0);
            IVP_LANXDD_IP (SRC0_DEPTH, vsrc01, va_src0, vpsrc0);
            IVP_LANXDD_IP (SRC0_DEPTH, vsrc02, va_src0, vpsrc0);

            va_src1 = IVP_LANXDD_PP(SRC1_DEPTH, vpsrc1);
            IVP_LANXDD_IP(SRC1_DEPTH, vsrc10, va_src1, vpsrc1);
            IVP_LANXDD_IP(SRC1_DEPTH, vsrc11, va_src1, vpsrc1);
            IVP_LANXDD_IP(SRC1_DEPTH, vsrc12, va_src1, vpsrc1);
#endif
            IVP_LAVNXDD_XP(SRC0_DEPTH, vsrc03, va_src0, vpsrc0,               width - x - 3 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH);
            IVP_LAVNXDD_XP(SRC1_DEPTH, vsrc13, va_src1, vpsrc1, SRC1_DEPTH * (width - x - 3 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH) / SRC0_DEPTH);

            OPERATION(vsrc00, vsrc10, vdst0);
            OPERATION(vsrc01, vsrc11, vdst1);
            OPERATION(vsrc02, vsrc12, vdst2);
            OPERATION(vsrc03, vsrc13, vdst3);
            IVP_SANXDD_IP (DST_DEPTH, vdst0, va_dst, vpdst);
            IVP_SANXDD_IP (DST_DEPTH, vdst1, va_dst, vpdst);
            IVP_SANXDD_IP (DST_DEPTH, vdst2, va_dst, vpdst);
            IVP_SAVNXDD_XP(DST_DEPTH, vdst3, va_dst, vpdst, DST_DEPTH * (width - x - 3 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH) / SRC0_DEPTH);
            IVP_SAVNXDDPOS_FP(DST_DEPTH, va_dst, vpdst);
        }
#endif
    }
#endif

#if UNROLL > 2
#if UNROLL == 3
    xb_vecNx16U vsrc00, vsrc01, vsrc02, vsrc10, vsrc11, vsrc12;
    xb_vecNx16 vdst0, vdst1, vdst2;
    for (; x < width - 2 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH; x += 3 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH)
#else
    if (x < width - 2 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH)
#endif
    {
#ifdef PTR_POSTINCR_UNROLL_3
        vpsrc0 = (SRC0_VEC*)OFFSET_PTR_NX8U(pdatasrc0, 0, sstride0, x);
        vpsrc1 = (SRC1_VEC*)OFFSET_PTR_NX8U(pdatasrc1, 0, sstride1, x * SRC1_DEPTH / SRC0_DEPTH);
        vpdst = (DST_VEC*)OFFSET_PTR_NX8U(pdatadst, 0, dstride, x * DST_DEPTH / SRC0_DEPTH);

        int offs0 = sstride0 - XT_MIN(3 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH, width - x);
        int offs1 = sstride1 - XT_MIN(3 * XCHAL_IVPN_SIMD_WIDTH * SRC1_DEPTH, SRC1_DEPTH * (width - x) / SRC0_DEPTH);
        int offd  = dstride  - XT_MIN(3 * XCHAL_IVPN_SIMD_WIDTH * DST_DEPTH,  DST_DEPTH * (width - x) / SRC0_DEPTH);

        for (int y = 0; y < height; ++y)
        {
#ifdef L2A2_UNROLL_3
            va_src0 = IVP_LANXDD_PP(SRC0_DEPTH, vpsrc0);
            IVP_L2A2NXDD_IP(SRC0_DEPTH, vsrc01, vsrc00, va_src0, vpsrc0);

            va_src1 = IVP_LANXDD_PP(SRC1_DEPTH, vpsrc1);
            IVP_L2A2NXDD_IP(SRC1_DEPTH, vsrc11, vsrc10, va_src1, vpsrc1);
#elif defined(L2AU_UNROLL_3)
            IVP_L2AUNXDD_IP(SRC0_DEPTH, vsrc00, va_src0, vpsrc0);
            IVP_LANXDD_IP  (SRC0_DEPTH, vsrc01, va_src0, vpsrc0);

            IVP_L2AUNXDD_IP(SRC1_DEPTH, vsrc10, va_src1, vpsrc1);
            IVP_LANXDD_IP  (SRC1_DEPTH, vsrc11, va_src1, vpsrc1);
#else
            va_src0 = IVP_LANXDD_PP(SRC0_DEPTH, vpsrc0);
            IVP_LANXDD_IP (SRC0_DEPTH, vsrc00, va_src0, vpsrc0);
            IVP_LANXDD_IP (SRC0_DEPTH, vsrc01, va_src0, vpsrc0);

            va_src1 = IVP_LANXDD_PP(SRC1_DEPTH, vpsrc1);
            IVP_LANXDD_IP (SRC1_DEPTH, vsrc10, va_src1, vpsrc1);
            IVP_LANXDD_IP (SRC1_DEPTH, vsrc11, va_src1, vpsrc1);
#endif
            IVP_LAVNXDD_XP(SRC0_DEPTH, vsrc02, va_src0, vpsrc0, width - x - 2 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH);
            IVP_LAVNXDD_XP(SRC1_DEPTH, vsrc12, va_src1, vpsrc1, SRC1_DEPTH * (width - x - 2 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH) / SRC0_DEPTH);

            OPERATION(vsrc00, vsrc10, vdst0);
            OPERATION(vsrc01, vsrc11, vdst1);
            OPERATION(vsrc02, vsrc12, vdst2);
            IVP_SANXDD_IP (DST_DEPTH, vdst0, va_dst, vpdst);
            IVP_SANXDD_IP (DST_DEPTH, vdst1, va_dst, vpdst);
            IVP_SAVNXDD_XP(DST_DEPTH, vdst2, va_dst, vpdst, DST_DEPTH * (width - x - 2 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH) / SRC0_DEPTH);
            IVP_SAVNXDDPOS_FP(DST_DEPTH, va_dst, vpdst);

            vpsrc0 = (SRC0_VEC*)OFFSET_PTR_NX8U(vpsrc0, 1, offs0, 0);
            vpsrc1 = (SRC1_VEC*)OFFSET_PTR_NX8U(vpsrc1, 1, offs1, 0);
            vpdst = (DST_VEC*)OFFSET_PTR_NX8U(vpdst, 1, offd, 0);
        }
#else
        SRC0_TYPE* psrc0 = (SRC0_TYPE*)OFFSET_PTR_NX8U(pdatasrc0, 0, sstride0, x);
        SRC1_TYPE* psrc1 = (SRC1_TYPE*)OFFSET_PTR_NX8U(pdatasrc1, 0, sstride1, x * SRC1_DEPTH / SRC0_DEPTH);
        DST_TYPE* pdst = (DST_TYPE*)OFFSET_PTR_NX8U(pdatadst, 0, dstride, x * DST_DEPTH / SRC0_DEPTH);

        for (int y = 0; y < height; ++y, psrc0 = (SRC0_TYPE*)OFFSET_PTR_NX8U(psrc0, 1, sstride0, 0),
                                         psrc1 = (SRC1_TYPE*)OFFSET_PTR_NX8U(psrc1, 1, sstride1, 0),
                                         pdst = (DST_TYPE*)OFFSET_PTR_NX8U(pdst, 1, dstride, 0))
        {
            vpsrc0 = (SRC0_VEC*)psrc0;
            vpsrc1 = (SRC1_VEC*)psrc1;
            vpdst  = (DST_VEC*) pdst;
#ifdef L2A2_UNROLL_3
            va_src0 = IVP_LANXDD_PP(SRC0_DEPTH, vpsrc0);
            IVP_L2A2NXDD_IP(SRC0_DEPTH, vsrc01, vsrc00, va_src0, vpsrc0);

            va_src1 = IVP_LANXDD_PP(SRC1_DEPTH, vpsrc1);
            IVP_L2A2NXDD_IP(SRC1_DEPTH, vsrc11, vsrc10, va_src1, vpsrc1);
#elif defined(L2AU_UNROLL_3)
            IVP_L2AUNXDD_IP(SRC0_DEPTH, vsrc00, va_src0, vpsrc0);
            IVP_LANXDD_IP  (SRC0_DEPTH, vsrc01, va_src0, vpsrc0);

            IVP_L2AUNXDD_IP(SRC1_DEPTH, vsrc10, va_src1, vpsrc1);
            IVP_LANXDD_IP  (SRC1_DEPTH, vsrc11, va_src1, vpsrc1);
#else
            va_src0 = IVP_LANXDD_PP(SRC0_DEPTH, vpsrc0);
            IVP_LANXDD_IP (SRC0_DEPTH, vsrc00, va_src0, vpsrc0);
            IVP_LANXDD_IP (SRC0_DEPTH, vsrc01, va_src0, vpsrc0);

            va_src1 = IVP_LANXDD_PP(SRC1_DEPTH, vpsrc1);
            IVP_LANXDD_IP (SRC1_DEPTH, vsrc10, va_src1, vpsrc1);
            IVP_LANXDD_IP (SRC1_DEPTH, vsrc11, va_src1, vpsrc1);
#endif
            IVP_LAVNXDD_XP(SRC0_DEPTH, vsrc02, va_src0, vpsrc0, width - x - 2 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH);
            IVP_LAVNXDD_XP(SRC1_DEPTH, vsrc12, va_src1, vpsrc1, SRC1_DEPTH * (width - x - 2 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH) / SRC0_DEPTH);

            OPERATION(vsrc00, vsrc10, vdst0);
            OPERATION(vsrc01, vsrc11, vdst1);
            OPERATION(vsrc02, vsrc12, vdst2);
            IVP_SANXDD_IP (DST_DEPTH, vdst0, va_dst, vpdst);
            IVP_SANXDD_IP (DST_DEPTH, vdst1, va_dst, vpdst);
            IVP_SAVNXDD_XP(DST_DEPTH, vdst2, va_dst, vpdst, DST_DEPTH * (width - x - 2 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH) / SRC0_DEPTH);
            IVP_SAVNXDDPOS_FP(DST_DEPTH, va_dst, vpdst);
        }
#endif
    }
#endif

#if UNROLL > 1
#if UNROLL == 2
    xb_vecNx16U vsrc00, vsrc01, vsrc10, vsrc11;
    xb_vecNx16 vdst0, vdst1;
    for (; x < width - XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH; x += 2 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH)
#elif UNROLL > 3
    else if (x < width - XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH)
#else
    if (x < width - XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH)
#endif
    {
#ifdef PTR_POSTINCR_UNROLL_2
        vpsrc0 = (SRC0_VEC*)OFFSET_PTR_NX8U(pdatasrc0, 0, sstride0, x);
        vpsrc1 = (SRC1_VEC*)OFFSET_PTR_NX8U(pdatasrc1, 0, sstride1, x * SRC1_DEPTH / SRC0_DEPTH);
        vpdst = (DST_VEC*)OFFSET_PTR_NX8U(pdatadst, 0, dstride, x * DST_DEPTH / SRC0_DEPTH);

        int offs0 = sstride0 - XT_MIN(2 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH, width - x);
        int offs1 = sstride1 - XT_MIN(2 * XCHAL_IVPN_SIMD_WIDTH * SRC1_DEPTH, SRC1_DEPTH * (width - x) / SRC0_DEPTH);
        int offd  = dstride  - XT_MIN(2 * XCHAL_IVPN_SIMD_WIDTH * DST_DEPTH,  DST_DEPTH * (width - x) / SRC0_DEPTH);

        for (int y = 0; y < height; ++y)
        {
#ifdef L2AU_UNROLL_2
            IVP_L2AUNXDD_IP(SRC0_DEPTH, vsrc00, va_src0, vpsrc0);
            IVP_L2AUNXDD_IP(SRC1_DEPTH, vsrc10, va_src1, vpsrc1);
#else
            va_src0 = IVP_LANXDD_PP(SRC0_DEPTH, vpsrc0);
            IVP_LANXDD_IP (SRC0_DEPTH, vsrc00, va_src0, vpsrc0);

            va_src1 = IVP_LANXDD_PP(SRC1_DEPTH, vpsrc1);
            IVP_LANXDD_IP (SRC1_DEPTH, vsrc10, va_src1, vpsrc1);
#endif
            IVP_LAVNXDD_XP(SRC0_DEPTH, vsrc01, va_src0, vpsrc0, width - x - XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH);
            IVP_LAVNXDD_XP(SRC1_DEPTH, vsrc11, va_src1, vpsrc1, SRC1_DEPTH * (width - x - XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH) / SRC0_DEPTH);

            OPERATION(vsrc00, vsrc10, vdst0);
            OPERATION(vsrc01, vsrc11, vdst1);
            IVP_SANXDD_IP (DST_DEPTH, vdst0, va_dst, vpdst);
            IVP_SAVNXDD_XP(DST_DEPTH, vdst1, va_dst, vpdst, DST_DEPTH * (width - x - XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH) / SRC0_DEPTH);
            IVP_SAVNXDDPOS_FP(DST_DEPTH, va_dst, vpdst);

            vpsrc0 = (SRC0_VEC*)OFFSET_PTR_NX8U(vpsrc0, 1, offs0, 0);
            vpsrc1 = (SRC1_VEC*)OFFSET_PTR_NX8U(vpsrc1, 1, offs1, 0);
            vpdst = (DST_VEC*)OFFSET_PTR_NX8U(vpdst, 1, offd, 0);
        }
#else
        SRC0_TYPE* psrc0 = (SRC0_TYPE*)OFFSET_PTR_NX8U(pdatasrc0, 0, sstride0, x);
        SRC1_TYPE* psrc1 = (SRC1_TYPE*)OFFSET_PTR_NX8U(pdatasrc1, 0, sstride1, x * SRC1_DEPTH / SRC0_DEPTH);
        DST_TYPE* pdst = (DST_TYPE*)OFFSET_PTR_NX8U(pdatadst, 0, dstride, x * DST_DEPTH / SRC0_DEPTH);

        for (int y = 0; y < height; ++y, psrc0 = (SRC0_TYPE*)OFFSET_PTR_NX8U(psrc0, 1, sstride0, 0),
                                         psrc1 = (SRC1_TYPE*)OFFSET_PTR_NX8U(psrc1, 1, sstride1, 0),
                                         pdst = (DST_TYPE*)OFFSET_PTR_NX8U(pdst, 1, dstride, 0))
        {
            vpsrc0 = (SRC0_VEC*)psrc0;
            vpsrc1 = (SRC1_VEC*)psrc1;
            vpdst  = (DST_VEC*) pdst;
#ifdef L2AU_UNROLL_2
            IVP_L2AUNXDD_IP(SRC0_DEPTH, vsrc00, va_src0, vpsrc0);
            IVP_L2AUNXDD_IP(SRC1_DEPTH, vsrc10, va_src1, vpsrc1);
#else
            va_src0 = IVP_LANXDD_PP(SRC0_DEPTH, vpsrc0);
            IVP_LANXDD_IP (SRC0_DEPTH, vsrc00, va_src0, vpsrc0);

            va_src1 = IVP_LANXDD_PP(SRC1_DEPTH, vpsrc1);
            IVP_LANXDD_IP (SRC1_DEPTH, vsrc10, va_src1, vpsrc1);
#endif
            IVP_LAVNXDD_XP(SRC0_DEPTH, vsrc01, va_src0, vpsrc0, width - x - XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH);
            IVP_LAVNXDD_XP(SRC1_DEPTH, vsrc11, va_src1, vpsrc1, SRC1_DEPTH * (width - x - XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH) / SRC0_DEPTH);

            OPERATION(vsrc00, vsrc10, vdst0);
            OPERATION(vsrc01, vsrc11, vdst1);
            IVP_SANXDD_IP (DST_DEPTH, vdst0, va_dst, vpdst);
            IVP_SAVNXDD_XP(DST_DEPTH, vdst1, va_dst, vpdst, DST_DEPTH * (width - x - XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH) / SRC0_DEPTH);
            IVP_SAVNXDDPOS_FP(DST_DEPTH, va_dst, vpdst);
        }
#endif
    }
#endif

#if UNROLL == 1
    xb_vecNx16U vsrc00, vsrc10;
    xb_vecNx16 vdst0;
    for (; x < width; x += XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH)
#elif UNROLL > 2
    else if (x < width)
#else
    if (x < width)
#endif
    {
#ifdef PTR_POSTINCR_UNROLL_1
        vpsrc0 = (SRC0_VEC*)OFFSET_PTR_NX8U(pdatasrc0, 0, sstride0, x);
        vpsrc1 = (SRC1_VEC*)OFFSET_PTR_NX8U(pdatasrc1, 0, sstride1, x * SRC1_DEPTH / SRC0_DEPTH);
        vpdst = (DST_VEC*)OFFSET_PTR_NX8U(pdatadst, 0, dstride, x * DST_DEPTH / SRC0_DEPTH);

        int offs0 = sstride0 - XT_MIN(XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH, width - x);
        int offs1 = sstride1 - XT_MIN(XCHAL_IVPN_SIMD_WIDTH * SRC1_DEPTH, SRC1_DEPTH * (width - x) / SRC0_DEPTH);
        int offd  = dstride  - XT_MIN(XCHAL_IVPN_SIMD_WIDTH * DST_DEPTH,  DST_DEPTH * (width - x) / SRC0_DEPTH);

        for (int y = 0; y < height; ++y)
        {
            va_src0 = IVP_LANXDD_PP(SRC0_DEPTH, vpsrc0);
            va_src1 = IVP_LANXDD_PP(SRC1_DEPTH, vpsrc1);
            IVP_LAVNXDD_XP(SRC0_DEPTH, vsrc00, va_src0, vpsrc0, width - x);
            IVP_LAVNXDD_XP(SRC1_DEPTH, vsrc10, va_src1, vpsrc1, SRC1_DEPTH * (width - x) / SRC0_DEPTH);
            OPERATION(vsrc00, vsrc10, vdst0);
            IVP_SAVNXDD_XP(DST_DEPTH, vdst0, va_dst, vpdst, DST_DEPTH * (width - x) / SRC0_DEPTH);
            IVP_SAVNXDDPOS_FP(DST_DEPTH, va_dst, vpdst);

            vpsrc0 = (SRC0_VEC*)OFFSET_PTR_NX8U(vpsrc0, 1, offs0, 0);
            vpsrc1 = (SRC1_VEC*)OFFSET_PTR_NX8U(vpsrc1, 1, offs1, 0);
            vpdst = (DST_VEC*)OFFSET_PTR_NX8U(vpdst, 1, offd, 0);
        }
#else
        SRC0_TYPE* psrc0 = (SRC0_TYPE*)OFFSET_PTR_NX8U(pdatasrc0, 0, sstride0, x);
        SRC1_TYPE* psrc1 = (SRC1_TYPE*)OFFSET_PTR_NX8U(pdatasrc1, 0, sstride1, x * SRC1_DEPTH / SRC0_DEPTH);
        DST_TYPE* pdst = (DST_TYPE*)OFFSET_PTR_NX8U(pdatadst, 0, dstride, x * DST_DEPTH / SRC0_DEPTH);

        for (int y = 0; y < height; ++y, psrc0 = (SRC0_TYPE*)OFFSET_PTR_NX8U(psrc0, 1, sstride0, 0),
                                         psrc1 = (SRC1_TYPE*)OFFSET_PTR_NX8U(psrc1, 1, sstride1, 0),
                                         pdst = (DST_TYPE*)OFFSET_PTR_NX8U(pdst, 1, dstride, 0))
        {
            vpsrc0 = (SRC0_VEC*)psrc0;
            vpsrc1 = (SRC1_VEC*)psrc1;
            vpdst  = (DST_VEC*) pdst;
            va_src0 = IVP_LANXDD_PP(SRC0_DEPTH, vpsrc0);
            va_src1 = IVP_LANXDD_PP(SRC1_DEPTH, vpsrc1);
            IVP_LAVNXDD_XP(SRC0_DEPTH, vsrc00, va_src0, vpsrc0, width - x);
            IVP_LAVNXDD_XP(SRC1_DEPTH, vsrc10, va_src1, vpsrc1, SRC1_DEPTH * (width - x) / SRC0_DEPTH);
            OPERATION(vsrc00, vsrc10, vdst0);
            IVP_SAVNXDD_XP(DST_DEPTH, vdst0, va_dst, vpdst, DST_DEPTH * (width - x) / SRC0_DEPTH);
            IVP_SAVNXDDPOS_FP(DST_DEPTH, va_dst, vpdst);
        }
#endif
    }

    return XI_ERROR_STATUS();

#include "arithm.h"
