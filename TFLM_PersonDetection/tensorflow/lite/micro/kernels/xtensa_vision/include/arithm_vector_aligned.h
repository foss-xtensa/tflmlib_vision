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
// This file is used as a template for aligned vector operations.
// Requires following macros to be defined:
// 1) DEPTH_8 or DEPTH_16 or DEPTH_8_16 or DEPTH_16_8 (mutually exclusive)
//    depending on source and destination data depth
// 2) OPERATION(vsrc0, vsrc1, vdst) - should transform source vectors "vsrc0" and "vsrc1" to destination "vdst"

// The function signature is not included into this template. This template assumes that:
// 1) The source tile arguments have names "src0" and "src1"
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
        CHECK_ARRAY(src0, SRC0_CHECK_TYPE);
        CHECK_ARRAY(src1, SRC1_CHECK_TYPE);
        CHECK_ARRAY(dst, DST_CHECK_TYPE);
        XI_CHECK_ARRAY_SIZE_EQ(src0, dst);
        XI_CHECK_ARRAY_SIZE_EQ(src1, dst);

        CHECK_ARRAY_ALIGNMENT(src0, SRC0_DEPTH, SRC0_REAL_DEPTH, IALIGNMENT);
        CHECK_ARRAY_ALIGNMENT(src1, SRC1_DEPTH, SRC1_REAL_DEPTH, IALIGNMENT);
        CHECK_ARRAY_ALIGNMENT(dst, DST_DEPTH, DST_REAL_DEPTH, OALIGNMENT);

#ifdef CHECK_ELEMENT_SIZE_EQ
        XI_CHECK_ARRAY_ELEMENT_SIZE_EQ(src0, dst);
        XI_CHECK_ARRAY_ELEMENT_SIZE_EQ(src1, dst);
#endif

        EXTRA_ERROR_CHECKS();
    }

    const int sstride0 = XI_TILE_GET_PITCH(src0) * SRC0_REAL_DEPTH;
    const int sstride1 = XI_TILE_GET_PITCH(src1) * SRC1_REAL_DEPTH;
    const int dstride  = XI_TILE_GET_PITCH(dst)  * DST_REAL_DEPTH;
    const int width    = XI_TILE_GET_WIDTH(src0) * SRC0_REAL_DEPTH;
    const int height   = XI_TILE_GET_HEIGHT(src0);

    SRC0_TYPE* psrc0 = (SRC0_TYPE*)XI_TILE_GET_DATA_PTR(src0);
    SRC1_TYPE* psrc1 = (SRC1_TYPE*)XI_TILE_GET_DATA_PTR(src1);
    DST_TYPE*  pdst  = (DST_TYPE*) XI_TILE_GET_DATA_PTR(dst);

    SRC0_VEC* __restrict vpsrc0;
    SRC1_VEC* __restrict vpsrc1;
    DST_VEC*  __restrict vpdst;
    int x = 0;

#if UNROLL > 3
    xb_vecNx16U vsrc00, vsrc01, vsrc02,  vsrc03, vsrc10, vsrc11, vsrc12, vsrc13;
    xb_vecNx16 vdst0, vdst1, vdst2, vdst3;
    for (; x <= width - 4 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH; x += 4 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH)
    {
        vpsrc0 = (SRC0_VEC*)OFFSET_PTR_NX8U(psrc0, 0, sstride0, x);
        vpsrc1 = (SRC1_VEC*)OFFSET_PTR_NX8U(psrc1, 0, sstride1, x * SRC1_DEPTH / SRC0_DEPTH);
        vpdst = (DST_VEC*)OFFSET_PTR_NX8U(pdst, 0, dstride, x * DST_DEPTH / SRC0_DEPTH);

        for (int y = 0; y < height; ++y)
        {
            IVP_LVNXDD_IP(SRC0_DEPTH, vsrc00, vpsrc0, SRC0_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            IVP_LVNXDD_IP(SRC0_DEPTH, vsrc01, vpsrc0, SRC0_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            IVP_LVNXDD_IP(SRC0_DEPTH, vsrc02, vpsrc0, SRC0_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            XI_IVP_LVNXDD_XP(SRC0_DEPTH, vsrc03, vpsrc0, sstride0 - 3 * SRC0_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            IVP_LVNXDD_IP(SRC1_DEPTH, vsrc10, vpsrc1, SRC1_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            IVP_LVNXDD_IP(SRC1_DEPTH, vsrc11, vpsrc1, SRC1_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            IVP_LVNXDD_IP(SRC1_DEPTH, vsrc12, vpsrc1, SRC1_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            XI_IVP_LVNXDD_XP(SRC1_DEPTH, vsrc13, vpsrc1, sstride1 - 3 * SRC1_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            OPERATION(vsrc00, vsrc10, vdst0);
            OPERATION(vsrc01, vsrc11, vdst1);
            OPERATION(vsrc02, vsrc12, vdst2);
            OPERATION(vsrc03, vsrc13, vdst3);
            IVP_SVNXDD_IP(DST_DEPTH, vdst0, vpdst, DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            IVP_SVNXDD_IP(DST_DEPTH, vdst1, vpdst, DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            IVP_SVNXDD_IP(DST_DEPTH, vdst2, vpdst, DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            XI_IVP_SVNXDD_XP(DST_DEPTH, vdst3, vpdst, dstride - 3 * DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
        }
    }
#endif

#if UNROLL > 2
#if UNROLL == 3
    xb_vecNx16U vsrc00, vsrc01, vsrc02, vsrc10, vsrc11, vsrc12;
    xb_vecNx16 vdst0, vdst1, vdst2;
    for (; x <= width - 3 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH; x += 3 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH)
#else
    if (x < width - 2 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH)
#endif
    {
        vpsrc0 = (SRC0_VEC*)OFFSET_PTR_NX8U(psrc0, 0, sstride0, x);
        vpsrc1 = (SRC1_VEC*)OFFSET_PTR_NX8U(psrc1, 0, sstride1, x * SRC1_DEPTH / SRC0_DEPTH);
        vpdst = (DST_VEC*)OFFSET_PTR_NX8U(pdst, 0, dstride, x * DST_DEPTH / SRC0_DEPTH);

        for (int y = 0; y < height; ++y)
        {
            IVP_LVNXDD_IP(SRC0_DEPTH, vsrc00, vpsrc0, SRC0_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            IVP_LVNXDD_IP(SRC0_DEPTH, vsrc01, vpsrc0, SRC0_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            XI_IVP_LVNXDD_XP(SRC0_DEPTH, vsrc02, vpsrc0, sstride0 - 2 * SRC0_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            IVP_LVNXDD_IP(SRC1_DEPTH, vsrc10, vpsrc1, SRC1_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            IVP_LVNXDD_IP(SRC1_DEPTH, vsrc11, vpsrc1, SRC1_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            XI_IVP_LVNXDD_XP(SRC1_DEPTH, vsrc12, vpsrc1, sstride1 - 2 * SRC1_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            OPERATION(vsrc00, vsrc10, vdst0);
            OPERATION(vsrc01, vsrc11, vdst1);
            OPERATION(vsrc02, vsrc12, vdst2);
            IVP_SVNXDD_IP(DST_DEPTH, vdst0, vpdst, DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            IVP_SVNXDD_IP(DST_DEPTH, vdst1, vpdst, DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            XI_IVP_SVNXDD_XP(DST_DEPTH, vdst2, vpdst, dstride - 2 * DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
        }
    }
#endif

#if UNROLL > 1
#if UNROLL == 2
    xb_vecNx16U vsrc00, vsrc01, vsrc10, vsrc11;
    xb_vecNx16 vdst0, vdst1;
    for (; x <= width - 2 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH; x += 2 * XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH)
#elif UNROLL > 3
    else if (x < width - XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH)
#else
    if (x < width - XCHAL_IVPN_SIMD_WIDTH * SRC0_DEPTH)
#endif
    {
        vpsrc0 = (SRC0_VEC*)OFFSET_PTR_NX8U(psrc0, 0, sstride0, x);
        vpsrc1 = (SRC1_VEC*)OFFSET_PTR_NX8U(psrc1, 0, sstride1, x * SRC1_DEPTH / SRC0_DEPTH);
        vpdst = (DST_VEC*)OFFSET_PTR_NX8U(pdst, 0, dstride, x * DST_DEPTH / SRC0_DEPTH);

        for (int y = 0; y < height; ++y)
        {
            IVP_LVNXDD_IP(SRC0_DEPTH, vsrc00, vpsrc0, SRC0_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            XI_IVP_LVNXDD_XP(SRC0_DEPTH, vsrc01, vpsrc0, sstride0 - SRC0_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            IVP_LVNXDD_IP(SRC1_DEPTH, vsrc10, vpsrc1, SRC1_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            XI_IVP_LVNXDD_XP(SRC1_DEPTH, vsrc11, vpsrc1, sstride1 - SRC1_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            OPERATION(vsrc00, vsrc10, vdst0);
            OPERATION(vsrc01, vsrc11, vdst1);
            IVP_SVNXDD_IP(DST_DEPTH, vdst0, vpdst, DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
            XI_IVP_SVNXDD_XP(DST_DEPTH, vdst1, vpdst, dstride - DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);
        }
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
        vpsrc0 = (SRC0_VEC*)OFFSET_PTR_NX8U(psrc0, 0, sstride0, x);
        vpsrc1 = (SRC1_VEC*)OFFSET_PTR_NX8U(psrc1, 0, sstride1, x * SRC1_DEPTH / SRC0_DEPTH);
        vpdst = (DST_VEC*)OFFSET_PTR_NX8U(pdst, 0, dstride, x * DST_DEPTH / SRC0_DEPTH);

        for (int y = 0; y < height; ++y)
        {
            XI_IVP_LVNXDD_XP(SRC0_DEPTH, vsrc00, vpsrc0, sstride0);
            XI_IVP_LVNXDD_XP(SRC1_DEPTH, vsrc10, vpsrc1, sstride1);
            OPERATION(vsrc00, vsrc10, vdst0);
            XI_IVP_SVNXDD_XP(DST_DEPTH, vdst0, vpdst, dstride);
        }
    }

    return XI_ERROR_STATUS();

#undef UNROLL
#include "arithm.h"
