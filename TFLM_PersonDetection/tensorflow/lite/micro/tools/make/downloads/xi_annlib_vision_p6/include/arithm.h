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
#ifndef __ARITHM_H__
#define __ARITHM_H__

#define DEPTH_ABBREV_1 8U
#define DEPTH_ABBREV_2 16U
#define XDEPTH_ABBREV(depth) DEPTH_ABBREV_##depth
#define DEPTH_ABBREV(depth) XDEPTH_ABBREV(depth)

#define XCONCAT3(a,b,c) a##b##c
#define CONCAT3(a,b,c) XCONCAT3(a,b,c)

#define XCHECK_ARRAY(array, type) XI_CHECK_ARRAY##type(array)
#define CHECK_ARRAY(array, type) XCHECK_ARRAY(array, type)

#define ARRAY_IS_WIDTH_ALIGNED(t, a, s)  (((s * XI_ARRAY_GET_WIDTH(t)) & (a * XCHAL_IVPN_SIMD_WIDTH - 1)) == 0)
#define ARRAY_IS_STRIDE_ALIGNED(t, a, s) (((s * XI_ARRAY_GET_PITCH(t)) & (a * XCHAL_IVPN_SIMD_WIDTH - 1)) == 0)
#define ARRAY_IS_PTR_ALIGNED(t, a)       ((XI_PTR_TO_ADDR(XI_ARRAY_GET_DATA_PTR(t)) & (a * XCHAL_IVPN_SIMD_WIDTH - 1)) == 0)
#define ARRAY_IS_ALIGNED(t, a, s)\
    (ARRAY_IS_PTR_ALIGNED(t, a) && ARRAY_IS_WIDTH_ALIGNED(t, a, s) && ARRAY_IS_STRIDE_ALIGNED(t, a, s))

#define CHECK_ARRAY_ALIGNMENT(array, a, s, ERR)\
    XI_CHECK_ERROR(ARRAY_IS_ALIGNED(array, a, s), XI_ERR_##ERR, "The argument (" #array ") is not fully aligned")


#define XI_IVP_LVNXDD_XP(depth, vec, ptr, offset) CONCAT3(XI_IVP_LVNX, DEPTH_ABBREV(depth), _XP)(vec, ptr, offset)
#define IVP_LVNXDD_IP(depth, vec, ptr, offset) CONCAT3(IVP_LVNX, DEPTH_ABBREV(depth), _IP)(vec, ptr, offset)
#define IVP_LANXDD_PP(depth, ptr) CONCAT3(IVP_LANX, DEPTH_ABBREV(depth), _PP)(ptr)
#define IVP_LANXDD_IP(depth, vec, va, ptr) CONCAT3(IVP_LANX, DEPTH_ABBREV(depth), _IP)(vec, va, ptr)
#define IVP_L2AUNXDD_IP(depth, vec, va, ptr) CONCAT3(IVP_L2AUNX, DEPTH_ABBREV(depth), _IP)(vec, va, ptr)
#define IVP_L2A2NXDD_IP(depth, vech, vecl, va, ptr) CONCAT3(IVP_L2A2NX, DEPTH_ABBREV(depth), _IP)(vech, vecl, va, ptr)
#define IVP_LAVNXDD_XP(depth, vec, va, ptr, count) CONCAT3(IVP_LAVNX, DEPTH_ABBREV(depth), _XP)(vec, va, ptr, count)

#define XI_IVP_SVNXDD_XP(depth, vec, ptr, offset) CONCAT3(XI_IVP_SVNX, DEPTH_ABBREV(depth), _XP)(vec, ptr, offset)
#define IVP_SVNXDD_IP(depth, vec, ptr, offset) CONCAT3(IVP_SVNX, DEPTH_ABBREV(depth), _IP)(vec, ptr, offset)
#define IVP_SANXDD_IP(depth, vec, va, ptr) CONCAT3(IVP_SANX, DEPTH_ABBREV(depth), _IP)(vec, va, ptr)
#define IVP_SAVNXDD_XP(depth, vec, va, ptr, count) CONCAT3(IVP_SAVNX, DEPTH_ABBREV(depth), _XP)(vec, va, ptr, count)
#define IVP_SAVNXDDPOS_FP(depth, va, ptr) CONCAT3(IVP_SAVNX, DEPTH_ABBREV(depth), POS_FP)(va, ptr)


#if XCHAL_HAVE_VISION && XCHAL_VISION_TYPE >= 6
#define LOOPBODY_SCALAR_UNROLL4(vecp___s, vecp___d)                                                                                                                                                    \
    {                                                                                                                                                   \
        SRC_TYPE* psrc = (SRC_TYPE*)OFFSET_PTR_NX8U(pdatasrc, 0, sstride, x);                                                                           \
        DST_TYPE* pdst = (DST_TYPE*)OFFSET_PTR_NX8U(pdatadst, 0, dstride, x * DST_DEPTH / SRC_DEPTH);                                                   \
        for (int y = 0; y < height; ++y, psrc = (SRC_TYPE*)OFFSET_PTR_NX8U(psrc, 1, sstride, 0), pdst = (DST_TYPE*)OFFSET_PTR_NX8U(pdst, 1, dstride, 0))\
        {                                                                                                                                               \
            vecp___s = (SRC_VEC*)psrc;                                                                                                                  \
            vecp___d = (DST_VEC*)pdst;                                                                                                                  \
            xb_vecNx16U vsrc0, vsrc1, vsrc2, vsrc3;                                                                                                     \
            xb_vecNx16 vdst0, vdst1, vdst2, vdst3;                                                                                                      \
            va_src = IVP_LANXDD_PP(SRC_DEPTH, vecp___s);                                                                                                \
            IVP_LANXDD_IP (SRC_DEPTH, vsrc0, va_src, vecp___s);                                                                                         \
            IVP_LANXDD_IP (SRC_DEPTH, vsrc1, va_src, vecp___s);                                                                                         \
            IVP_LANXDD_IP (SRC_DEPTH, vsrc2, va_src, vecp___s);                                                                                         \
            IVP_LAVNXDD_XP(SRC_DEPTH, vsrc3, va_src, vecp___s, width - x - 3 * XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH);                                      \
            OPERATION(vsrc0, vdst0);                                                                                                                    \
            OPERATION(vsrc1, vdst1);                                                                                                                    \
            OPERATION(vsrc2, vdst2);                                                                                                                    \
            OPERATION(vsrc3, vdst3);                                                                                                                    \
            IVP_SANXDD_IP (DST_DEPTH, vdst0, va_dst, vecp___d);                                                                                         \
            IVP_SANXDD_IP (DST_DEPTH, vdst1, va_dst, vecp___d);                                                                                         \
            IVP_SANXDD_IP (DST_DEPTH, vdst2, va_dst, vecp___d);                                                                                         \
            IVP_SAVNXDD_XP(DST_DEPTH, vdst3, va_dst, vecp___d, DST_DEPTH * (width - x - 3 * XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH) / SRC_DEPTH);            \
            IVP_SAVNXDDPOS_FP(DST_DEPTH, va_dst, vecp___d);                                                                                             \
        }                                                                                                                                               \
    }

#define LOOPBODY_SCALAR_UNROLL3(vecp___s, vecp___d)                                                                                                             \
    {                                                                                                                                                   \
        SRC_TYPE* psrc = (SRC_TYPE*)OFFSET_PTR_NX8U(pdatasrc, 0, sstride, x);                                                                           \
        DST_TYPE* pdst = (DST_TYPE*)OFFSET_PTR_NX8U(pdatadst, 0, dstride, x * DST_DEPTH / SRC_DEPTH);                                                   \
        for (int y = 0; y < height; ++y, psrc = (SRC_TYPE*)OFFSET_PTR_NX8U(psrc, 1, sstride, 0), pdst = (DST_TYPE*)OFFSET_PTR_NX8U(pdst, 1, dstride, 0))\
        {                                                                                                                                               \
            vecp___s = (SRC_VEC*)psrc;                                                                                                                  \
            vecp___d = (DST_VEC*)pdst;                                                                                                                  \
            xb_vecNx16U vsrc0, vsrc1, vsrc2;                                                                                                            \
            xb_vecNx16 vdst0, vdst1, vdst2;                                                                                                             \
            va_src = IVP_LANXDD_PP(SRC_DEPTH, vecp___s);                                                                                                \
            IVP_LANXDD_IP (SRC_DEPTH, vsrc0, va_src, vecp___s);                                                                                         \
            IVP_LANXDD_IP (SRC_DEPTH, vsrc1, va_src, vecp___s);                                                                                         \
            IVP_LAVNXDD_XP(SRC_DEPTH, vsrc2, va_src, vecp___s, width - x - 2 * XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH);                                      \
            OPERATION(vsrc0, vdst0);                                                                                                                    \
            OPERATION(vsrc1, vdst1);                                                                                                                    \
            OPERATION(vsrc2, vdst2);                                                                                                                    \
            IVP_SANXDD_IP (DST_DEPTH, vdst0, va_dst, vecp___d);                                                                                         \
            IVP_SANXDD_IP (DST_DEPTH, vdst1, va_dst, vecp___d);                                                                                         \
            IVP_SAVNXDD_XP(DST_DEPTH, vdst2, va_dst, vecp___d, DST_DEPTH * (width - x - 2 * XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH) / SRC_DEPTH);            \
            IVP_SAVNXDDPOS_FP(DST_DEPTH, va_dst, vecp___d);                                                                                             \
        }                                                                                                                                               \
    }

#define LOOPBODY_SCALAR_UNROLL2(vecp___s, vecp___d)                                                                                                             \
    {                                                                                                                                                   \
        SRC_TYPE* psrc = (SRC_TYPE*)OFFSET_PTR_NX8U(pdatasrc, 0, sstride, x);                                                                           \
        DST_TYPE* pdst = (DST_TYPE*)OFFSET_PTR_NX8U(pdatadst, 0, dstride, x * DST_DEPTH / SRC_DEPTH);                                                   \
        for (int y = 0; y < height; ++y, psrc = (SRC_TYPE*)OFFSET_PTR_NX8U(psrc, 1, sstride, 0), pdst = (DST_TYPE*)OFFSET_PTR_NX8U(pdst, 1, dstride, 0))\
        {                                                                                                                                               \
            vecp___s = (SRC_VEC*)psrc;                                                                                                                  \
            vecp___d = (DST_VEC*)pdst;                                                                                                                  \
            xb_vecNx16U vsrc0, vsrc1;                                                                                                                   \
            xb_vecNx16 vdst0, vdst1;                                                                                                                    \
            va_src = IVP_LANXDD_PP(SRC_DEPTH, vecp___s);                                                                                                \
            IVP_LANXDD_IP (SRC_DEPTH, vsrc0, va_src, vecp___s);                                                                                         \
            IVP_LAVNXDD_XP(SRC_DEPTH, vsrc1, va_src, vecp___s, width - x - XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH);                                          \
            OPERATION(vsrc0, vdst0);                                                                                                                    \
            OPERATION(vsrc1, vdst1);                                                                                                                    \
            IVP_SANXDD_IP (DST_DEPTH, vdst0, va_dst, vecp___d);                                                                                         \
            IVP_SAVNXDD_XP(DST_DEPTH, vdst1, va_dst, vecp___d, DST_DEPTH * (width - x - XCHAL_IVPN_SIMD_WIDTH * SRC_DEPTH) / SRC_DEPTH);                \
            IVP_SAVNXDDPOS_FP(DST_DEPTH, va_dst, vecp___d);                                                                                             \
        }                                                                                                                                               \
    }

#define LOOPBODY_SCALAR_UNROLL1(vecp___s, vecp___d)                                                                                                             \
    {                                                                                                                                                   \
        SRC_TYPE* psrc = (SRC_TYPE*)OFFSET_PTR_NX8U(pdatasrc, 0, sstride, x);                                                                           \
        DST_TYPE* pdst = (DST_TYPE*)OFFSET_PTR_NX8U(pdatadst, 0, dstride, x * DST_DEPTH / SRC_DEPTH);                                                   \
        for (int y = 0; y < height; ++y, psrc = (SRC_TYPE*)OFFSET_PTR_NX8U(psrc, 1, sstride, 0), pdst = (DST_TYPE*)OFFSET_PTR_NX8U(pdst, 1, dstride, 0))\
        {                                                                                                                                               \
            vecp___s = (SRC_VEC*)psrc;                                                                                                                  \
            vecp___d = (DST_VEC*)pdst;                                                                                                                  \
            xb_vecNx16U vsrc0;                                                                                                                          \
            xb_vecNx16 vdst0;                                                                                                                           \
            va_src = IVP_LANXDD_PP(SRC_DEPTH, vecp___s);                                                                                                \
            IVP_LAVNXDD_XP(SRC_DEPTH, vsrc0, va_src, vecp___s, width - x);                                                                              \
            OPERATION(vsrc0, vdst0);                                                                                                                    \
            IVP_SAVNXDD_XP(DST_DEPTH, vdst0, va_dst, vecp___d, DST_DEPTH * (width - x) / SRC_DEPTH);                                                    \
            IVP_SAVNXDDPOS_FP(DST_DEPTH, va_dst, vecp___d);                                                                                             \
        }                                                                                                                                               \
    }

#define LOOPBODY_SCALAR_UNROLL4A(vecp___s, vecp___d)                                                                \
    {                                                                                                       \
        xb_vecNx16U vsrc0, vsrc1, vsrc2, vsrc3;                                                             \
        xb_vecNx16 vdst0, vdst1, vdst2, vdst3;                                                              \
        vecp___s = (SRC_VEC*)OFFSET_PTR_NX8U(pdatasrc, 0, sstride, x);                                      \
        vecp___d = (DST_VEC*)OFFSET_PTR_NX8U(pdatadst, 0, dstride, x * DST_DEPTH / SRC_DEPTH);              \
        for (int y = 0; y < height; ++y)                                                                    \
        {                                                                                                   \
            IVP_LVNXDD_IP(SRC_DEPTH, vsrc0, vecp___s, SRC_DEPTH * XCHAL_IVPN_SIMD_WIDTH);                   \
            IVP_LVNXDD_IP(SRC_DEPTH, vsrc1, vecp___s, SRC_DEPTH * XCHAL_IVPN_SIMD_WIDTH);                   \
            IVP_LVNXDD_IP(SRC_DEPTH, vsrc2, vecp___s, SRC_DEPTH * XCHAL_IVPN_SIMD_WIDTH);                   \
            XI_IVP_LVNXDD_XP(SRC_DEPTH, vsrc3, vecp___s, sstride - 3 * SRC_DEPTH * XCHAL_IVPN_SIMD_WIDTH);  \
            OPERATION(vsrc0, vdst0);                                                                        \
            OPERATION(vsrc1, vdst1);                                                                        \
            OPERATION(vsrc2, vdst2);                                                                        \
            OPERATION(vsrc3, vdst3);                                                                        \
            IVP_SVNXDD_IP(DST_DEPTH, vdst0, vecp___d, DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);                   \
            IVP_SVNXDD_IP(DST_DEPTH, vdst1, vecp___d, DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);                   \
            IVP_SVNXDD_IP(DST_DEPTH, vdst2, vecp___d, DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);                   \
            XI_IVP_SVNXDD_XP(DST_DEPTH, vdst3, vecp___d, dstride - 3 * DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);  \
        }                                                                                                   \
    }

#define LOOPBODY_SCALAR_UNROLL3A(vecp___s, vecp___d)                                                                \
    {                                                                                                       \
        xb_vecNx16U vsrc0, vsrc1, vsrc2;                                                                    \
        xb_vecNx16 vdst0, vdst1, vdst2;                                                                     \
        vecp___s = (SRC_VEC*)OFFSET_PTR_NX8U(pdatasrc, 0, sstride, x);                                      \
        vecp___d = (DST_VEC*)OFFSET_PTR_NX8U(pdatadst, 0, dstride, x * DST_DEPTH / SRC_DEPTH);              \
        for (int y = 0; y < height; ++y)                                                                    \
        {                                                                                                   \
            IVP_LVNXDD_IP(SRC_DEPTH, vsrc0, vecp___s, SRC_DEPTH * XCHAL_IVPN_SIMD_WIDTH);                   \
            IVP_LVNXDD_IP(SRC_DEPTH, vsrc1, vecp___s, SRC_DEPTH * XCHAL_IVPN_SIMD_WIDTH);                   \
            XI_IVP_LVNXDD_XP(SRC_DEPTH, vsrc2, vecp___s, sstride - 2 * SRC_DEPTH * XCHAL_IVPN_SIMD_WIDTH);  \
            OPERATION(vsrc0, vdst0);                                                                        \
            OPERATION(vsrc1, vdst1);                                                                        \
            OPERATION(vsrc2, vdst2);                                                                        \
            IVP_SVNXDD_IP(DST_DEPTH, vdst0, vecp___d, DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);                   \
            IVP_SVNXDD_IP(DST_DEPTH, vdst1, vecp___d, DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);                   \
            XI_IVP_SVNXDD_XP(DST_DEPTH, vdst2, vecp___d, dstride - 2 * DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);  \
        }                                                                                                   \
    }

#define LOOPBODY_SCALAR_UNROLL2A(vecp___s, vecp___d)                                                            \
    {                                                                                                   \
        xb_vecNx16U vsrc0, vsrc1;                                                                       \
        xb_vecNx16 vdst0, vdst1;                                                                        \
        vecp___s = (SRC_VEC*)OFFSET_PTR_NX8U(pdatasrc, 0, sstride, x);                                  \
        vecp___d = (DST_VEC*)OFFSET_PTR_NX8U(pdatadst, 0, dstride, x * DST_DEPTH / SRC_DEPTH);          \
        for (int y = 0; y < height; ++y)                                                                \
        {                                                                                               \
            IVP_LVNXDD_IP(SRC_DEPTH, vsrc0, vecp___s, SRC_DEPTH * XCHAL_IVPN_SIMD_WIDTH);               \
            XI_IVP_LVNXDD_XP(SRC_DEPTH, vsrc1, vecp___s, sstride - SRC_DEPTH * XCHAL_IVPN_SIMD_WIDTH);  \
            OPERATION(vsrc0, vdst0);                                                                    \
            OPERATION(vsrc1, vdst1);                                                                    \
            IVP_SVNXDD_IP(DST_DEPTH, vdst0, vecp___d, DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);               \
            XI_IVP_SVNXDD_XP(DST_DEPTH, vdst1, vecp___d, dstride - DST_DEPTH * XCHAL_IVPN_SIMD_WIDTH);  \
        }                                                                                               \
    }

#define LOOPBODY_SCALAR_UNROLL1A(vecp___s, vecp___d)                                                    \
    {                                                                                           \
        xb_vecNx16U vsrc0;                                                                      \
        xb_vecNx16 vdst0;                                                                       \
        vecp___s = (SRC_VEC*)OFFSET_PTR_NX8U(pdatasrc, 0, sstride, x);                          \
        vecp___d = (DST_VEC*)OFFSET_PTR_NX8U(pdatadst, 0, dstride, x * DST_DEPTH / SRC_DEPTH);  \
        for (int y = 0; y < height; ++y)                                                        \
        {                                                                                       \
            XI_IVP_LVNXDD_XP(SRC_DEPTH, vsrc0, vecp___s, sstride);                              \
            OPERATION(vsrc0, vdst0);                                                            \
            XI_IVP_SVNXDD_XP(DST_DEPTH, vdst0, vecp___d, dstride);                              \
        }                                                                                       \
    }
#endif //XCHAL_HAVE_VISION && XCHAL_VISION_TYPE >= 6

#endif // __ARITHM_H__

// every odd include sets macro, every even include unsets macro
// file is normally included twice - before and after the block that uses these macro
#ifdef SRC0_TYPE
#    undef OPERATION
#    undef OPERATIONN
#    undef SRC_DEPTH
#    undef SRC_REAL_DEPTH
#    undef SRC_VEC
#    undef SRC_TYPE
#    undef SRC_CHECK_TYPE
#    undef SRC0_DEPTH
#    undef SRC0_REAL_DEPTH
#    undef SRC0_VEC
#    undef SRC0_TYPE
#    undef SRC0_CHECK_TYPE
#    undef SRC1_DEPTH
#    undef SRC1_REAL_DEPTH
#    undef SRC1_VEC
#    undef SRC1_TYPE
#    undef SRC1_CHECK_TYPE
#    undef DST_DEPTH
#    undef DST_REAL_DEPTH
#    undef DST_VEC
#    undef DST_TYPE
#    undef DST_CHECK_TYPE
#    undef DEPTH_2x8
#    undef DEPTH_8
#    undef DEPTH_16
#    undef DEPTH_8_16
#    undef DEPTH_16_8
#    undef DEPTH_8_16_16
#    undef DEPTH_16_8_16
#    undef DEPTH_2x8_TYPELESS
#    undef EXTRA_ERROR_CHECKS
#    undef CHECK_ELEMENT_SIZE_EQ
#    undef UNROLL
#    undef UNROLL_VP6
#    undef UNROLL_VP5
#    undef UNROLL_IVPEP
#    undef PTR_POSTINCR_UNROLL_4
#    undef PTR_POSTINCR_UNROLL_3
#    undef PTR_POSTINCR_UNROLL_2
#    undef PTR_POSTINCR_UNROLL_1
#    undef L2A_UNROLL_4
#    undef L2AU_UNROLL_4
#    undef LA2NX8_UNROLL_4
#    undef L2AU_UNROLL_3
#    undef L2A2_UNROLL_3
#    undef L2AU_UNROLL_2
#else
#    ifdef DEPTH_2x8_TYPELESS
#        define DEPTH_2x8
#        define CHECK_ELEMENT_SIZE_EQ
#        define SRC_REAL_DEPTH XI_ARRAY_GET_ELEMENT_SIZE(src)
#        define SRC0_REAL_DEPTH XI_ARRAY_GET_ELEMENT_SIZE(src0)
#        define SRC1_REAL_DEPTH XI_ARRAY_GET_ELEMENT_SIZE(src1)
#        define DST_REAL_DEPTH XI_ARRAY_GET_ELEMENT_SIZE(dst)
#        define SRC_CHECK_TYPE
#        define SRC0_CHECK_TYPE
#        define SRC1_CHECK_TYPE
#        define DST_CHECK_TYPE
#    endif

#    if defined (DEPTH_2x8)
#        define SRC0_DEPTH 2
#        define SRC1_DEPTH 2
#        define DST_DEPTH 2
#    ifndef SRC0_REAL_DEPTH
#        define SRC0_REAL_DEPTH 1
#    endif
#    ifndef SRC1_REAL_DEPTH
#        define SRC1_REAL_DEPTH 1
#    endif
#    ifndef DST_REAL_DEPTH
#        define DST_REAL_DEPTH 1
#    endif
#    elif defined (DEPTH_8)
#        define SRC0_DEPTH 1
#        define SRC1_DEPTH 1
#        define DST_DEPTH 1
#    elif defined(DEPTH_16)
#        define SRC0_DEPTH 2
#        define SRC1_DEPTH 2
#        define DST_DEPTH 2
#    elif defined(DEPTH_8_16)
#        define SRC0_DEPTH 1
#        define SRC1_DEPTH 1
#        define DST_DEPTH 2
#    elif defined(DEPTH_16_8)
#        define SRC0_DEPTH 2
#        define SRC1_DEPTH 2
#        define DST_DEPTH 1
#    elif defined(DEPTH_8_16_16)
#        define SRC0_DEPTH 1
#        define SRC1_DEPTH 2
#        define DST_DEPTH 2
#    elif defined(DEPTH_16_8_16)
#        define SRC0_DEPTH 2
#        define SRC1_DEPTH 1
#        define DST_DEPTH 2
#    endif

#    ifndef SRC0_REAL_DEPTH
#        define SRC0_REAL_DEPTH SRC0_DEPTH
#    endif
#    ifndef SRC1_REAL_DEPTH
#        define SRC1_REAL_DEPTH SRC1_DEPTH
#    endif
#    ifndef DST_REAL_DEPTH
#        define DST_REAL_DEPTH DST_DEPTH
#    endif

#ifndef EXTRA_ERROR_CHECKS
#    define EXTRA_ERROR_CHECKS()
#endif

#    if (SRC0_DEPTH == 2)
#        define SRC0_VEC xb_vecNx16U
#        define SRC0_TYPE uint16_t
#    elif (SRC0_DEPTH == 1)
#        define SRC0_VEC xb_vecNx8U
#        define SRC0_TYPE uint8_t
#    else
#        define SRC0_TYPE void
#    endif

#    if (SRC1_DEPTH == 2)
#        define SRC1_VEC xb_vecNx16U
#        define SRC1_TYPE uint16_t
#    elif (SRC1_DEPTH == 1)
#        define SRC1_VEC xb_vecNx8U
#        define SRC1_TYPE uint8_t
#    else
#        define SRC1_TYPE void
#    endif

#    if (DST_DEPTH == 2)
#        define DST_VEC xb_vecNx16U
#        define DST_TYPE uint16_t
#    elif (DST_DEPTH == 1)
#        define DST_VEC xb_vecNx8U
#        define DST_TYPE uint8_t
#    else
#        define DST_TYPE void
#    endif

#    ifndef SRC0_CHECK_TYPE
#        if (SRC0_REAL_DEPTH == 2)
#            define SRC0_CHECK_TYPE _I16
#        elif (SRC0_REAL_DEPTH == 1)
#            define SRC0_CHECK_TYPE _I8
#        endif
#    endif

#    ifndef SRC1_CHECK_TYPE
#        if (SRC1_REAL_DEPTH == 2)
#            define SRC1_CHECK_TYPE _I16
#        elif (SRC1_REAL_DEPTH == 1)
#            define SRC1_CHECK_TYPE _I8
#        endif
#    endif

#    ifndef DST_CHECK_TYPE
#        if (DST_REAL_DEPTH == 2)
#            define DST_CHECK_TYPE _I16
#        elif (DST_REAL_DEPTH == 1)
#            define DST_CHECK_TYPE _I8
#        endif
#    endif

#    ifndef SRC_CHECK_TYPE
#        define SRC_CHECK_TYPE SRC0_CHECK_TYPE
#    endif

#    ifndef SRC_REAL_DEPTH
#        define SRC_REAL_DEPTH SRC0_REAL_DEPTH
#    endif

#    define SRC_DEPTH SRC0_DEPTH
#    define SRC_VEC SRC0_VEC
#    define SRC_TYPE SRC0_TYPE

#endif
