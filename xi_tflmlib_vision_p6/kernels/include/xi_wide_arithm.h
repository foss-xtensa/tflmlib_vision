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
#ifndef __XI_WIDE_ARITHM_H__
#define __XI_WIDE_ARITHM_H__

#include "xi_intrin.h"

// unsigned sum reduction from xb_vec2Nx24 into uint32_t without saturation
#if XCHAL_HAVE_VISION
#   define XIVP_RADDR2NX24_U32(vec24) ((uint32_t)IVP_RADDN_2X32(IVP_ADDN_2X32(IVP_ADDN_2X32(IVP_CVT32S2NX24LL(vec24), IVP_CVT32S2NX24LH(vec24)), IVP_ADDN_2X32(IVP_CVT32S2NX24HL(vec24), IVP_CVT32S2NX24HH(vec24)))))
#else
#   define XIVP_RADDR2NX24_U32(vec24) (IVP_RADDURNX16(IVP_PACKL2NX24_0(vec24)) + IVP_RADDURNX16(IVP_PACKL2NX24_1(vec24)) + ((IVP_RADDURNX16(IVP_ADDNX16(IVP_PACKVRNR2NX24_0(vec24, 16), IVP_PACKVRNR2NX24_1(vec24, 16)))) << 16))
#endif

// sum reduction from xb_vecNx48 into 32-bit integer without saturation
#if XCHAL_HAVE_VISION
#   define XIVP_RADDRNX32W_I32(vec48) ((int)IVP_RADDN_2X32(IVP_ADDN_2X32(IVP_CVT32UNX48H(vec48), IVP_CVT32UNX48L(vec48))))
#else
#   define XIVP_RADDRNX32W_I32(vec48) (IVP_RADDURNX16(IVP_PACKLNX48(vec48)) + (IVP_RADDRNX16(IVP_PACKMNX48(vec48)) << 16))
#endif

// unsigned sum reduction from xb_vecNx48 truncated to 32-bit into uint64_t
#if XCHAL_HAVE_VISION
#   define XIVP_RADDRNX32W_U64(vec48) ((((uint64_t)(unsigned)IVP_RADDUNX16(IVP_PACKMNX48(vec48))) << 16) + (unsigned)IVP_RADDUNX16(IVP_PACKLNX48(vec48)))
#else
#   define XIVP_RADDRNX32W_U64(vec48) ((((uint64_t)IVP_RADDURNX16(IVP_PACKMNX48(vec48))) << 16) + IVP_RADDURNX16(IVP_PACKLNX48(vec48)))
#endif

// signed sum reduction from xb_vecNx48 truncated to 32-bit into int64_t
#if XCHAL_HAVE_VISION
#   define XIVP_RADDRNX32W_S64(vec48) ((((int64_t)((int)IVP_RADDNX16(IVP_PACKMNX48(vec48)))) << 16) + (int)IVP_RADDUNX16(IVP_PACKLNX48(vec48)))
#else
#   define XIVP_RADDRNX32W_S64(vec48) ((((int64_t)IVP_RADDRNX16(IVP_PACKMNX48(vec48))) << 16) + IVP_RADDURNX16(IVP_PACKLNX48(vec48)))
#endif

// unsigned sum reduction from xb_vecNx48 into uint64_t
#if XCHAL_HAVE_VISION
#   define XIVP_RADDRNX48_U64(vec48) ((((uint64_t)(unsigned)IVP_RADDUNX16(IVP_PACKHNX48(vec48))) << 32) + (((uint64_t)(unsigned)IVP_RADDUNX16(IVP_PACKMNX48(vec48))) << 16) + (unsigned)IVP_RADDUNX16(IVP_PACKLNX48(vec48)))
#else
#   define XIVP_RADDRNX48_U64(vec48) ((((uint64_t)IVP_RADDURNX16(IVP_PACKHNX48(vec48))) << 32) + (((uint64_t)IVP_RADDURNX16(IVP_PACKMNX48(vec48))) << 16) + IVP_RADDURNX16(IVP_PACKLNX48(vec48)))
#endif

// sum reduction from xb_vecNx48 into int64_t
#if XCHAL_HAVE_VISION
#   define XIVP_RADDRNX48_S64(vec48) ((((int64_t)((int)IVP_RADDNX16(IVP_PACKHNX48(vec48)))) << 32) + (((int64_t)((int)IVP_RADDUNX16(IVP_PACKMNX48(vec48)))) << 16) + (int)IVP_RADDUNX16(IVP_PACKLNX48(vec48)))
#else
#   define XIVP_RADDRNX48_S64(vec48) ((((int64_t)IVP_RADDRNX16(IVP_PACKHNX48(vec48))) << 32) + (((int64_t)IVP_RADDURNX16(IVP_PACKMNX48(vec48))) << 16) + IVP_RADDURNX16(IVP_PACKLNX48(vec48)))
#endif

// true-predicated unsigned sum reduction (result is 32-bit on VP5, and 16-bit on IVP-EP - is sum exceeeds 2^16 then result in high half is undefined)
#if XCHAL_HAVE_VISION
#   define XIVP_RADDUNX16T(vec,msk) IVP_MOVNX16_FROM32U(IVP_RADDUNX16T(vec,msk))
#else
#   define XIVP_RADDUNX16T(vec,msk) IVP_MOVNX16_FROM16(IVP_RADDNX16T(vec,msk))
#endif

// true-predicated signed sum reduction; result is truncated to 16-bit; elements 1-31 are set to 0
#if XCHAL_HAVE_VISION
#   define XIVP_RADDSNX16T(vec,msk) IVP_MOVNX16_FROM16(IVP_SELSNX16(IVP_MOVNX16_FROM32(IVP_RADDNX16T(vec,msk)),0))
#else
#   define XIVP_RADDSNX16T(vec,msk) IVP_MOVNX16_FROM16(IVP_RADDNX16T(vec,msk))
#endif

// unsigned sum reduction (result is 32-bit on VP5, and 16-bit on IVP-EP - is sum exceeds 2^16 then result in high half is undefined)
#if XCHAL_HAVE_VISION
#   define XIVP_RADDUNX16(vec) IVP_MOVNX16_FROM32U(IVP_RADDUNX16(vec))
#else
#   define XIVP_RADDUNX16(vec) IVP_MOVNX16_FROM16(IVP_RADDNX16(vec))
#endif


// negation S32
#define XI_NEG_S32(out_h, out_l, in_h, in_l)            \
{                                                       \
    out_h = IVP_NEGNX16(in_h);                          \
    IVP_SUBNX16T(out_h, out_h, 1, IVP_NEQNX16(0, in_l));\
    out_l = IVP_NEGNX16(in_l);                          \
}

// right arithmetic shift S32 (immediate)
#define XI_SRAIN_S32(out_h, out_l, in_h, in_l, rsht)                              \
{                                                                                 \
    out_l = IVP_ADDNX16(IVP_SLLINX16(in_h, 16 - rsht), IVP_SRLINX16(in_l, rsht)); \
    out_h = IVP_SRAINX16(in_h, rsht);                                             \
}

// right logic shift S32 (immediate)
#define XI_SRLIN_I32(out_h, out_l, in_h, in_l, rsht)                              \
{                                                                                 \
    out_l = IVP_ADDNX16(IVP_SLLINX16(in_h, 16 - rsht), IVP_SRLINX16(in_l, rsht)); \
    out_h = IVP_SRLINX16(in_h, rsht);                                             \
}

// left arithmetic shift S32 (immediate)
#define XI_SLLIN_I32(out_h, out_l, in_h, in_l, lsht)                                \
{                                                                                   \
    out_h = IVP_ADDNX16(IVP_SLLINX16(in_h, lsht), IVP_SRLINX16(in_l, (16 - lsht))); \
    out_l = IVP_SLLINX16(in_l, lsht);                                               \
}

// left logic shift S32
#define XI_SLLN_I32(out_h, out_l, in_h, in_l, vlsht)                                                                     \
{                                                                                                                        \
    out_h = IVP_ADDNX16(IVP_SLLNX16(in_h, vlsht), IVP_SRLNX16(in_l, IVP_MOVVSV(IVP_SUBNX16(16, IVP_MOVVVS(vlsht)), 0))); \
    out_l = IVP_SLLNX16(in_l, vlsht);                                                                                    \
}

// right logic shift S32
#define XI_SRLN_I32(out_h, out_l, in_h, in_l, vrsht)                                                                     \
{                                                                                                                        \
    out_l = IVP_ADDNX16(IVP_SLLNX16(in_h, IVP_MOVVSV(IVP_SUBNX16(16, IVP_MOVVVS(vrsht)), 0)), IVP_SRLNX16(in_l, vrsht)); \
    out_h = IVP_SRLNX16(in_h, vrsht);                                                                                    \
}

// left arithmetic shift U48
#define XI_SLLN_I48(out_h, out_m, out_l, in_h, in_m, in_l, vlsht)                                                                                                                                           \
{                                                                                                                                                                                                           \
    out_h = IVP_ADDNX16(IVP_ADDNX16(IVP_SLLNX16(in_h, vlsht), IVP_SLLNX16(in_m, IVP_MOVVSV(IVP_SUBNX16(IVP_MOVVVS(vlsht), 16), 0))), IVP_SLLNX16(in_l, IVP_MOVVSV(IVP_SUBNX16(IVP_MOVVVS(vlsht), 32), 0))); \
    out_m = IVP_ADDNX16(IVP_SLLNX16(in_m, vlsht), IVP_SRLNX16(in_l, IVP_MOVVSV(IVP_SUBNX16(16, IVP_MOVVVS(vlsht)), 0)));                                                                                    \
    out_l = IVP_SLLNX16(in_l, vlsht);                                                                                                                                                                       \
}

// right logic shift S48 (immediate; it can be from 1 to 15)
#define XI_SRLIN_I48(out_h, out_l, in_h, in_m, in_l, rsht)                        \
{                                                                                 \
    out_l = IVP_ADDNX16(IVP_SRLINX16(in_l, rsht), IVP_SLLINX16(in_m, 16 - rsht)); \
    out_h = IVP_ADDNX16(IVP_SRLINX16(in_m, rsht), IVP_SLLINX16(in_h, 16 - rsht)); \
}

// multiply S16 by S16 to produce S32 output, NOTE: can't multiply -32768 by -32768
#define XI_MULNS16_S32(out_h, out_l, in0, in1)                                                                  \
{                                                                                                               \
    out_l = IVP_MULNX16PACKL(in0, in1);                                                                         \
    out_h = IVP_SRAINX16(IVP_SUBNX16(IVP_MULNX16PACKQ(in0, in1), IVP_ANDNX16(IVP_SRAINX16(out_l, 14), 1)), 1);  \
}

// multiply U16 by U15 to produce U32 output
#define XI_MULNL_U16U15(out_h, out_l, in16, in15)                                                                           \
{                                                                                                                           \
    out_l = IVP_MULNX16PACKL(in16, in15);                                                                                   \
    out_h = IVP_SUBNX16(IVP_MULNX16PACKQ(IVP_ANDNX16(in16, (1 << 15) - 1), in15), IVP_ANDNX16(IVP_SRAINX16(out_l, 14), 1)); \
    IVP_ADDNX16T(out_h, out_h, in15, IVP_LTUNX16((1 << 15) - 1, in16));                                                     \
    out_h = IVP_SRLINX16(out_h, 1);                                                                                         \
}

// multiply U16 by U16 to produce U32 output
#define XI_MULNL_U16(out_h, out_l, in0, in1)                                                                            \
{                                                                                                                       \
    vboolN msb0_set = IVP_LTUNX16((1 << 15) - 1, in0);                                                                  \
    xb_vecNx16 in0_msb1 = IVP_MOVNX16T(in0, 0, IVP_LTUNX16((1 << 15) - 1, in1));                                        \
    xb_vecNx16 in0_nomsb = IVP_ANDNX16(in0, (1 << 15) - 1);                                                             \
    xb_vecNx16 in1_nomsb = IVP_ANDNX16(in1, (1 << 15) - 1);                                                             \
    out_l = IVP_MULNX16PACKL(in0, in1);                                                                                 \
    xb_vecNx16 hi2_nomsb = IVP_SUBNX16(IVP_MULNX16PACKQ(in0_nomsb, in1_nomsb), IVP_ANDNX16(IVP_SRAINX16(out_l, 14), 1));\
    IVP_ADDNX16T(hi2_nomsb, hi2_nomsb, in1_nomsb, msb0_set);                                                            \
    out_h = IVP_ADDNX16(IVP_ANDNX16(hi2_nomsb, in0_msb1), IVP_SRLINX16(IVP_XORNX16(hi2_nomsb, in0_msb1), 1));           \
}

// multiply U32 by U15 to produce U32 output
// (in-place is not supported)
#define XI_MULNW_U15(out_h, out_l, in0_h, in0_l, in1)  \
{                                                      \
    XI_MULNL_U16U15(out_h, out_l, in0_l, in1);         \
    IVP_MULANX16PACKL(out_h, in0_h, in1);              \
}

// multiply U32 by U16 to produce U32 output
// (in-place is not supported)
#define XI_MULNW_U16(out_h, out_l, in0_h, in0_l, in1)  \
{                                                      \
    XI_MULNL_U16(out_h, out_l, in0_l, in1);            \
    IVP_MULANX16PACKL(out_h, in0_h, in1);              \
}

// square U16 value to produce U32 value
#define XI_SQRNU16_U32(out_h, out_l, in)                            \
{                                                                   \
    xb_vecNx16 in_nomsb = IVP_ANDNX16(in, (1 << 15) - 1);           \
    vboolN msb_set = IVP_LTUNX16((1 << 15) - 1, in);                \
    XI_MULNS16_S32(out_h, out_l, in_nomsb, in_nomsb);               \
    IVP_ADDNX16T(out_h, out_h, IVP_SUBNX16(in, 1 << 14), msb_set);  \
}

// square S16 value to produce U32 value
#define XI_SQRNS16_U32(out_h, out_l, in)                                                                     \
{                                                                                                            \
    out_l = IVP_MULNX16PACKL(in, in);                                                                        \
    out_h = IVP_SRLINX16(IVP_SUBNX16(IVP_MULNX16PACKQ(in, in), IVP_ANDNX16(IVP_SRAINX16(out_l, 14), 1)), 1); \
}

// 32-way 32-bit unsigned less than or equal to comparison
#define XI_LEUNX32(a_h, a_l, b_h, b_l) IVP_ORBN(IVP_LTUNX16(a_h, b_h), IVP_ANDBN(IVP_EQNX16(a_h, b_h), IVP_LEUNX16(a_l, b_l)))

// 32-way 32-bit unsigned less than comparison
#define XI_LTUNX32(a_h, a_l, b_h, b_l) IVP_ORBN(IVP_LTUNX16(a_h, b_h), IVP_ANDBN(IVP_EQNX16(a_h, b_h), IVP_LTUNX16(a_l, b_l)))

// subtract I32 value from I32 value to produce I32 value
#define XI_SUBN_I32(out_h, out_l, in0_h, in0_l, in1_h, in1_l)   \
{                                                               \
    out_h = IVP_SUBNX16(in0_h, in1_h);                          \
    IVP_SUBNX16T(out_h, out_h, 1, IVP_LTUNX16(in0_l, in1_l));   \
    out_l = IVP_SUBNX16(in0_l, in1_l);                          \
}

// subtract I32 value from I32 value to produce I32 value for predicated TRUE elements
#define XI_SUBN_I32T(out_h, out_l, in0_h, in0_l, in1_h, in1_l, b)               \
    {                                                                           \
        IVP_SUBNX16T(out_h, in0_h, in1_h, b);                                   \
        IVP_SUBNX16T(out_h, out_h, 1, IVP_ANDBN(IVP_LTUNX16(in0_l, in1_l), b)); \
        IVP_SUBNX16T(out_l, in0_l, in1_l, b);                                   \
    }

// subtract U16 value from U32 value to produce U32 value
#define XI_SUBNW_U16(out_h, out_l, in0_h, in0_l, in1_l)         \
{                                                               \
    out_h = in0_h;                                              \
    IVP_SUBNX16T(out_h, out_h, 1, IVP_LTUNX16(in0_l, in1_l));   \
    out_l = IVP_SUBNX16(in0_l, in1_l);                          \
}

// add U16 value to U32 value to produce U32 value
#define XI_ADDNW_U16(out_h, out_l, in0_h, in0_l, in1)               \
{                                                                   \
    xb_vecNx16 __xi__add_l = IVP_ADDNX16(in1, in0_l);               \
    out_h = in0_h;                                                  \
    IVP_ADDNX16T(out_h, out_h, 1, IVP_LTUNX16(__xi__add_l, in0_l)); \
    out_l = __xi__add_l;                                            \
}

// add S16 value to S32 value to produce U32 value
#define XI_ADDNW_S16(out_h, out_l, in0_h, in0_l, in1)               \
{                                                                   \
    out_l = IVP_ADDNX16(in1, in0_l);                                \
    out_h = IVP_ADDNX16(in0_h, IVP_SRAINX16(in1, 15));              \
    IVP_ADDNX16T(out_h, out_h, 1, IVP_LTUNX16(out_l, in1));         \
}

// add U16 value to U32 value and select high half
#define XI_ADDNW_U16_HIGH(out_h, in0_h, in0_l, in1)                 \
{                                                                   \
    xb_vecNx16 __xi__add_l = IVP_ADDNX16(in1, in0_l);               \
    out_h = in0_h;                                                  \
    IVP_ADDNX16T(out_h, out_h, 1, IVP_LTUNX16(__xi__add_l, in0_l)); \
}

// add I32 value to I32 value to produce I32 value
#define XI_ADDN_U32(out_h, out_l, in0_h, in0_l, in1_h, in1_l)       \
{                                                                   \
    xb_vecNx16 __xi__add_l = IVP_ADDNX16(in1_l, in0_l);             \
    out_h = IVP_ADDNX16(in0_h, in1_h);                              \
    IVP_ADDNX16T(out_h, out_h, 1, IVP_LTUNX16(__xi__add_l, in0_l)); \
    out_l = __xi__add_l;                                            \
}

// add U32 value to U48 value to produce U48 value
#define XI_ADDNW_U32(out_hh, out_h, out_l, in0_hh, in0_h, in0_l, in1_h, in1_l)      \
{                                                                                   \
        xb_vecNx16 __xi__add_l = IVP_ADDNX16(in1_l, in0_l);                         \
        xb_vecNx16 __xi__add_h = IVP_ADDNX16(in1_h, in0_h);                         \
        IVP_ADDNX16T(__xi__add_h, __xi__add_h, 1, IVP_LTUNX16(__xi__add_l, in0_l)); \
        out_hh = in0_hh;                                                            \
        IVP_ADDNX16T(out_hh, out_hh, 1, IVP_LTUNX16(__xi__add_h, in0_h));           \
        out_h = __xi__add_h;                                                        \
        out_l = __xi__add_l;                                                        \
}

// reduce add U32 values to produce U32 value
#define XI_RADDN_U32(out, in0_h, in0_l)                                                                     \
{                                                                                                           \
    xb_vecNx16 __xi__in0 = IVP_ANDNX16(in0_l, 0x7ff);                                                       \
    xb_vecNx16 __xi__in1 = IVP_ANDNX16(IVP_ORNX16(IVP_SRLINX16(in0_l, 11), IVP_SLLINX16(in0_h, 5)), 0x7ff); \
    xb_vecNx16 __xi__in2 = IVP_SRLINX16(in0_h,  6);                                                         \
    out = (uint32_t)IVP_MOVAVU16(XIVP_RADDUNX16(__xi__in0))                                                 \
                 + (IVP_MOVAVU16(XIVP_RADDUNX16(__xi__in1)) << 11)                                          \
                 + (IVP_MOVAVU16(XIVP_RADDUNX16(__xi__in2)) << 22);                                         \
}

// reduce add U32 TRUE predicated values to produce U32 value
#define XI_RADDNT_U32(out, in0_h, in0_l, b)                                                                 \
{                                                                                                           \
    xb_vecNx16 __xi__in0 = IVP_ANDNX16(in0_l, 0x7ff);                                                       \
    xb_vecNx16 __xi__in1 = IVP_ANDNX16(IVP_ORNX16(IVP_SRLINX16(in0_l, 11), IVP_SLLINX16(in0_h, 5)), 0x7ff); \
    xb_vecNx16 __xi__in2 = IVP_SRLINX16(in0_h,  6);                                                         \
    out = (uint32_t)IVP_MOVAVU16(XIVP_RADDUNX16T(__xi__in0, b))                                             \
                 + (IVP_MOVAVU16(XIVP_RADDUNX16T(__xi__in1, b)) << 11)                                      \
                 + (IVP_MOVAVU16(XIVP_RADDUNX16T(__xi__in2, b)) << 22);                                     \
}

// reduce add U48 values to produce U48 (U64) value
#define XI_RADDN_U48(out, in0_hh, in0_h, in0_l)                                                       \
{                                                                                                     \
    uint64_t __xi__l_l  = IVP_MOVAVU16(XIVP_RADDUNX16(IVP_ANDNX16 (in0_l, 0xff)));                    \
    uint64_t __xi__l_h  = IVP_MOVAVU16(XIVP_RADDUNX16(IVP_SRLINX16(in0_l, 8)));                       \
    uint64_t __xi__h_l  = IVP_MOVAVU16(XIVP_RADDUNX16(IVP_ANDNX16 (in0_h, 0xff)));                    \
    uint64_t __xi__h_h  = IVP_MOVAVU16(XIVP_RADDUNX16(IVP_SRLINX16(in0_h, 8)));                       \
    uint64_t __xi__hh_l = IVP_MOVAVU16(XIVP_RADDUNX16(IVP_ANDNX16 (in0_hh, 0xff))));                  \
    uint64_t __xi__hh_h = IVP_MOVAVU16(XIVP_RADDUNX16(IVP_SRLINX16(in0_hh, 8)));                      \
    out = (((uint64_t)((__xi__hh_h << 8) + __xi__hh_l)) << 32)                                        \
        + (((uint64_t)((__xi__h_h << 8) + __xi__h_l)) << 16)                                          \
        + (__xi__l_h << 8) + __xi__l_l;                                                               \
}

// table lookup transform for (2*XCHAL_IVPN_SIMD_WIDTH) elements lookup
#define XI_LUT2NX16(vsrc, vdst, lut0, lut1, rshift) \
    vdst = IVP_SELNX16(lut1, lut0, IVP_MOVVSELNX16(vsrc, rshift))

// table lookup transform for (4*XCHAL_IVPN_SIMD_WIDTH) elements lookup
#define XI_LUT4NX16(vsrc, vdst, lut0, lut1, lut2, lut3, rshift)                         \
{                                                                                       \
    vsaN __xi__sel = IVP_MOVVSELNX16(vsrc, rshift);                                     \
    vboolN __xi__vb2 = IVP_LTUNX16U(2 * (XCHAL_IVPN_SIMD_WIDTH << rshift) - 1, vsrc);   \
    vdst = IVP_SELNX16(lut1, lut0, __xi__sel);                                          \
    xb_vecNx16 __xi__val2 = IVP_SELNX16(lut3, lut2, __xi__sel);                         \
    vdst = IVP_MOVNX16T(__xi__val2, vdst, __xi__vb2);                                   \
}

// table lookup transform for (8*XCHAL_IVPN_SIMD_WIDTH) elements lookup
#define XI_LUT8NX16(vsrc, vdst, lut0, lut1, lut2, lut3, lut4, lut5, lut6, lut7, rshift) \
{                                                                                       \
    vsaN __xi__sel = IVP_MOVVSELNX16(vsrc, rshift);                                     \
    vboolN __xi__vb2 = IVP_LTUNX16U(2 * (XCHAL_IVPN_SIMD_WIDTH << rshift) - 1, vsrc);   \
    vboolN __xi__vb3 = IVP_LTUNX16U(4 * (XCHAL_IVPN_SIMD_WIDTH << rshift) - 1, vsrc);   \
    vboolN __xi__vb4 = IVP_LTUNX16U(6 * (XCHAL_IVPN_SIMD_WIDTH << rshift) - 1, vsrc);   \
    vdst = IVP_SELNX16(lut1, lut0, __xi__sel);                                          \
    xb_vecNx16 __xi__val2 = IVP_SELNX16(lut3, lut2, __xi__sel);                         \
    xb_vecNx16 __xi__val3 = IVP_SELNX16(lut5, lut4, __xi__sel);                         \
    xb_vecNx16 __xi__val4 = IVP_SELNX16(lut7, lut6, __xi__sel);                         \
    vdst = IVP_MOVNX16T(__xi__val2, vdst, __xi__vb2);                                   \
    vdst = IVP_MOVNX16T(__xi__val3, vdst, __xi__vb3);                                   \
    vdst = IVP_MOVNX16T(__xi__val4, vdst, __xi__vb4);                                   \
}

#define XI_LUT8NX16_2(vsrc0, vsrc1, vdst0, vdst1, lut0, lut1, lut2, lut3, lut4, lut5, lut6, lut7, rshift) \
    {                                                                                                     \
        vsaN __xi__sel_0 = IVP_MOVVSV(vsrc0, 0);                                                          \
        vboolN __xi__vb2_0 = IVP_LTUNX16U(2 * (XCHAL_IVPN_SIMD_WIDTH << rshift) - 1, vsrc0);              \
        vboolN __xi__vb3_0 = IVP_LTUNX16U(4 * (XCHAL_IVPN_SIMD_WIDTH << rshift) - 1, vsrc0);              \
        vboolN __xi__vb4_0 = IVP_LTUNX16U(6 * (XCHAL_IVPN_SIMD_WIDTH << rshift) - 1, vsrc0);              \
        vsaN __xi__sel_1 = IVP_MOVVSV(vsrc1, 0);                                                          \
        vboolN __xi__vb2_1 = IVP_LTUNX16U(2 * (XCHAL_IVPN_SIMD_WIDTH << rshift) - 1, vsrc1);              \
        vboolN __xi__vb3_1 = IVP_LTUNX16U(4 * (XCHAL_IVPN_SIMD_WIDTH << rshift) - 1, vsrc1);              \
        vboolN __xi__vb4_1 = IVP_LTUNX16U(6 * (XCHAL_IVPN_SIMD_WIDTH << rshift) - 1, vsrc1);              \
                                                                                                          \
        IVP__DSELNX16_2X16(vdst1, vdst0, lut1, lut0, __xi__sel_1, __xi__sel_0);                           \
                                                                                                          \
        xb_vecNx16 __xi__val2_1, __xi__val2_0;                                                            \
        IVP__DSELNX16_2X16(__xi__val2_1, __xi__val2_0, lut3, lut2, __xi__sel_1, __xi__sel_0);             \
                                                                                                          \
        xb_vecNx16 __xi__val3_1, __xi__val3_0;                                                            \
        IVP__DSELNX16_2X16(__xi__val3_1, __xi__val3_0, lut5, lut4, __xi__sel_1, __xi__sel_0);             \
                                                                                                          \
        xb_vecNx16 __xi__val4_1, __xi__val4_0;                                                            \
        IVP__DSELNX16_2X16(__xi__val4_1, __xi__val4_0, lut7, lut6, __xi__sel_1, __xi__sel_0);             \
                                                                                                          \
        vdst0 = IVP_MOVNX16T(__xi__val2_0, vdst0, __xi__vb2_0);                                           \
        vdst0 = IVP_MOVNX16T(__xi__val3_0, vdst0, __xi__vb3_0);                                           \
        vdst0 = IVP_MOVNX16T(__xi__val4_0, vdst0, __xi__vb4_0);                                           \
        vdst1 = IVP_MOVNX16T(__xi__val2_1, vdst1, __xi__vb2_1);                                           \
        vdst1 = IVP_MOVNX16T(__xi__val3_1, vdst1, __xi__vb3_1);                                           \
        vdst1 = IVP_MOVNX16T(__xi__val4_1, vdst1, __xi__vb4_1);                                           \
    }

// table lookup transform for (2*XCHAL_IVPN_SIMD_WIDTH) elements lookup (for 2N vectors)
#if XCHAL_HAVE_VISION
#define XI_LUT2NX8U(vsrc, vdst, lut, rshift)               \
{                                                          \
    vdst = IVP_SHFL2NX8(lut, IVP_SRLI2NX8(vsrc, rshift));  \
}
#else
#define XI_LUT2NX8U(vsrc, vdst, lut, rshift) \
{                                            \
    vsaN sel_a, sel_b;                       \
    IVP_MOVPVSV(sel_a, sel_b, vsrc, rshift); \
    vdst = IVP_SHFL2NX8(lut, sel_a, sel_b);  \
}
#endif

// table lookup transform for (4*XCHAL_IVPN_SIMD_WIDTH) elements lookup (for 2N vectors)
#if XCHAL_HAVE_VISION
#define XI_LUT4NX8U(vsrc, vdst, lut0, lut1, rshift)             \
{                                                               \
    vdst = IVP_SEL2NX8(lut1, lut0, IVP_SRLI2NX8(vsrc, rshift)); \
}
#else
#define XI_LUT4NX8U(vsrc, vdst, lut0, lut1, rshift)     \
{                                                       \
    vsaN sel_a, sel_b;                                  \
    IVP_MOVPVSV(sel_a, sel_b, vsrc, rshift);            \
    vdst = IVP__SEL2NX8_2X16(lut1, lut0, sel_a, sel_b); \
}
#endif

// table lookup transform for (8*XCHAL_IVPN_SIMD_WIDTH) elements lookup (for 2N vectors)
#if XCHAL_HAVE_VISION
#define XI_LUT8NX8U(vsrc, vdst, lut0, lut1, lut2, lut3)                                     \
{                                                                                           \
    vdst = IVP_SEL2NX8(lut1, lut0, vsrc);                                                   \
    IVP_SEL2NX8T(vdst, lut3, lut2, vsrc, IVP_LTU2NX8(4 * XCHAL_IVPN_SIMD_WIDTH - 1, vsrc)); \
}
#else
#define XI_LUT8NX8U(vsrc, vdst, lut0, lut1, lut2, lut3)                     \
{                                                                           \
    vsaN sel_a, sel_b;                                                      \
    IVP_MOVPVSV(sel_a, sel_b, vsrc, 0);                                     \
    vbool2N __xi__vb = IVP_LTU2NX8(4 * XCHAL_IVPN_SIMD_WIDTH - 1, vsrc);    \
    vdst = IVP__SEL2NX8_2X16(lut1, lut0, sel_a, sel_b);                     \
    xb_vec2Nx8U __xi__val = IVP__SEL2NX8_2X16(lut3, lut2, sel_a, sel_b);    \
    vdst = IVP_MOV2NX8T(__xi__val, vdst, __xi__vb);                         \
}
#endif

// prefix sum of N 16-bit integers
#define XI_SCANN_I16(scan_out, scan_in, zero)                                                       \
{                                                                                                   \
    xb_vecNx16 scan_v1 = IVP_ADDNX16(scan_in, IVP_SELNX16I(scan_in, zero, IVP_SELI_ROTATE_LEFT_1)); \
    xb_vecNx16 scan_v2 = IVP_ADDNX16(scan_v1, IVP_SELNX16I(scan_v1, zero, IVP_SELI_ROTATE_LEFT_2)); \
    xb_vecNx16 scan_v3 = IVP_ADDNX16(scan_v2, IVP_SELNX16I(scan_v2, zero, IVP_SELI_ROTATE_LEFT_4)); \
    xb_vecNx16 scan_v4 = IVP_ADDNX16(scan_v3, IVP_SELNX16I(scan_v3, zero, IVP_SELI_ROTATE_LEFT_8)); \
    scan_out           = IVP_ADDNX16(scan_v4, IVP_SELNX16I(scan_v4, zero, IVP_SELI_ROTATE_LEFT_16));\
}

// prefix sum of 2*N 16-bit integers
#define XI_SCAN2N_I16(scan_out0, scan_out1, scan_in0, scan_in1, zero)                                       \
{                                                                                                           \
    xb_vecNx16 scan_vl1 = IVP_ADDNX16(scan_in0, IVP_SELNX16I(scan_in0, zero,     IVP_SELI_ROTATE_LEFT_1));  \
    xb_vecNx16 scan_vr1 = IVP_ADDNX16(scan_in1, IVP_SELNX16I(scan_in1, scan_in0, IVP_SELI_ROTATE_LEFT_1));  \
    xb_vecNx16 scan_vl2 = IVP_ADDNX16(scan_vl1, IVP_SELNX16I(scan_vl1, zero,     IVP_SELI_ROTATE_LEFT_2));  \
    xb_vecNx16 scan_vr2 = IVP_ADDNX16(scan_vr1, IVP_SELNX16I(scan_vr1, scan_vl1, IVP_SELI_ROTATE_LEFT_2));  \
    xb_vecNx16 scan_vl3 = IVP_ADDNX16(scan_vl2, IVP_SELNX16I(scan_vl2, zero,     IVP_SELI_ROTATE_LEFT_4));  \
    xb_vecNx16 scan_vr3 = IVP_ADDNX16(scan_vr2, IVP_SELNX16I(scan_vr2, scan_vl2, IVP_SELI_ROTATE_LEFT_4));  \
    xb_vecNx16 scan_vl4 = IVP_ADDNX16(scan_vl3, IVP_SELNX16I(scan_vl3, zero,     IVP_SELI_ROTATE_LEFT_8));  \
    xb_vecNx16 scan_vr4 = IVP_ADDNX16(scan_vr3, IVP_SELNX16I(scan_vr3, scan_vl3, IVP_SELI_ROTATE_LEFT_8));  \
    scan_out0           = IVP_ADDNX16(scan_vl4, IVP_SELNX16I(scan_vl4, zero,     IVP_SELI_ROTATE_LEFT_16)); \
    xb_vecNx16 scan_vr5 = IVP_ADDNX16(scan_vr4, IVP_SELNX16I(scan_vr4, scan_vl4, IVP_SELI_ROTATE_LEFT_16)); \
    scan_out1           = IVP_ADDNX16(scan_vr5, scan_out0);                                                 \
}

// prefix sum of 3*N 16-bit integers
#define XI_SCAN3N_I16(scan_out0, scan_out1, scan_out2, scan_in0, scan_in1, scan_in2, zero)                  \
{                                                                                                           \
    xb_vecNx16 scan_vl1 = IVP_ADDNX16(scan_in0, IVP_SELNX16I(scan_in0, zero,     IVP_SELI_ROTATE_LEFT_1));  \
    xb_vecNx16 scan_vr1 = IVP_ADDNX16(scan_in1, IVP_SELNX16I(scan_in1, scan_in0, IVP_SELI_ROTATE_LEFT_1));  \
    xb_vecNx16 scan_vm1 = IVP_ADDNX16(scan_in2, IVP_SELNX16I(scan_in2, scan_in1, IVP_SELI_ROTATE_LEFT_1));  \
    xb_vecNx16 scan_vl2 = IVP_ADDNX16(scan_vl1, IVP_SELNX16I(scan_vl1, zero,     IVP_SELI_ROTATE_LEFT_2));  \
    xb_vecNx16 scan_vr2 = IVP_ADDNX16(scan_vr1, IVP_SELNX16I(scan_vr1, scan_vl1, IVP_SELI_ROTATE_LEFT_2));  \
    xb_vecNx16 scan_vm2 = IVP_ADDNX16(scan_vm1, IVP_SELNX16I(scan_vm1, scan_vr1, IVP_SELI_ROTATE_LEFT_2));  \
    xb_vecNx16 scan_vl3 = IVP_ADDNX16(scan_vl2, IVP_SELNX16I(scan_vl2, zero,     IVP_SELI_ROTATE_LEFT_4));  \
    xb_vecNx16 scan_vr3 = IVP_ADDNX16(scan_vr2, IVP_SELNX16I(scan_vr2, scan_vl2, IVP_SELI_ROTATE_LEFT_4));  \
    xb_vecNx16 scan_vm3 = IVP_ADDNX16(scan_vm2, IVP_SELNX16I(scan_vm2, scan_vr2, IVP_SELI_ROTATE_LEFT_4));  \
    xb_vecNx16 scan_vl4 = IVP_ADDNX16(scan_vl3, IVP_SELNX16I(scan_vl3, zero,     IVP_SELI_ROTATE_LEFT_8));  \
    xb_vecNx16 scan_vr4 = IVP_ADDNX16(scan_vr3, IVP_SELNX16I(scan_vr3, scan_vl3, IVP_SELI_ROTATE_LEFT_8));  \
    xb_vecNx16 scan_vm4 = IVP_ADDNX16(scan_vm3, IVP_SELNX16I(scan_vm3, scan_vr3, IVP_SELI_ROTATE_LEFT_8));  \
    scan_out0           = IVP_ADDNX16(scan_vl4, IVP_SELNX16I(scan_vl4, zero,     IVP_SELI_ROTATE_LEFT_16)); \
    xb_vecNx16 scan_vr5 = IVP_ADDNX16(scan_vr4, IVP_SELNX16I(scan_vr4, scan_vl4, IVP_SELI_ROTATE_LEFT_16)); \
    xb_vecNx16 scan_vm5 = IVP_ADDNX16(scan_vm4, IVP_SELNX16I(scan_vm4, scan_vr4, IVP_SELI_ROTATE_LEFT_16)); \
    scan_out1 = IVP_ADDNX16(scan_vr5, scan_out0);                                                           \
    scan_out2 = IVP_ADDNX16(scan_vm5, scan_out1);                                                           \
}

// prefix sum of N 32-bit integers
#define XI_SCANN_I32(scan_out_hi, scan_out_lo, scan_in_hi, scan_in_lo, zero)                                                \
{                                                                                                                           \
    xb_vecNx16 scan_v1_hi, scan_v1_lo, scan_v2_hi, scan_v2_lo, scan_v3_hi, scan_v3_lo, scan_v4_hi, scan_v4_lo;              \
    XI_ADDN_U32(scan_v1_hi,  scan_v1_lo,  scan_in_hi, scan_in_lo, IVP_SELNX16I(scan_in_hi, zero, IVP_SELI_ROTATE_LEFT_1),   \
                                                                  IVP_SELNX16I(scan_in_lo, zero, IVP_SELI_ROTATE_LEFT_1));  \
    XI_ADDN_U32(scan_v2_hi,  scan_v2_lo,  scan_v1_hi, scan_v1_lo, IVP_SELNX16I(scan_v1_hi, zero, IVP_SELI_ROTATE_LEFT_2),   \
                                                                  IVP_SELNX16I(scan_v1_lo, zero, IVP_SELI_ROTATE_LEFT_2));  \
    XI_ADDN_U32(scan_v3_hi,  scan_v3_lo,  scan_v2_hi, scan_v2_lo, IVP_SELNX16I(scan_v2_hi, zero, IVP_SELI_ROTATE_LEFT_4),   \
                                                                  IVP_SELNX16I(scan_v2_lo, zero, IVP_SELI_ROTATE_LEFT_4));  \
    XI_ADDN_U32(scan_v4_hi,  scan_v4_lo,  scan_v3_hi, scan_v3_lo, IVP_SELNX16I(scan_v3_hi, zero, IVP_SELI_ROTATE_LEFT_8),   \
                                                                  IVP_SELNX16I(scan_v3_lo, zero, IVP_SELI_ROTATE_LEFT_8));  \
    XI_ADDN_U32(scan_out_hi, scan_out_lo, scan_v4_hi, scan_v4_lo, IVP_SELNX16I(scan_v4_hi, zero, IVP_SELI_ROTATE_LEFT_16),  \
                                                                  IVP_SELNX16I(scan_v4_lo, zero, IVP_SELI_ROTATE_LEFT_16)); \
}

// 32-bit prefix sum of N signed 16-bit integers
#define XI_SCANNW_S16(scan_out_hi, scan_out_lo, scan_in, zero)                                                              \
{                                                                                                                           \
    xb_vecNx16 scan_v2_hi, scan_v2_lo, scan_v3_hi, scan_v3_lo, scan_v4_hi, scan_v4_lo;                                      \
    xb_vecNx16 scan_v1_hi = IVP_SRAINX16(scan_in, 15); /* sign-extend */                                                    \
    xb_vecNx16 scan_v1_lo = scan_in;                                                                                        \
    XI_ADDNW_S16(scan_v1_hi, scan_v1_lo,  scan_v1_hi, scan_v1_lo, IVP_SELNX16I(scan_in, zero, IVP_SELI_ROTATE_LEFT_1));     \
    XI_ADDN_U32(scan_v2_hi,  scan_v2_lo,  scan_v1_hi, scan_v1_lo, IVP_SELNX16I(scan_v1_hi, zero, IVP_SELI_ROTATE_LEFT_2),   \
                                                                  IVP_SELNX16I(scan_v1_lo, zero, IVP_SELI_ROTATE_LEFT_2));  \
    XI_ADDN_U32(scan_v3_hi,  scan_v3_lo,  scan_v2_hi, scan_v2_lo, IVP_SELNX16I(scan_v2_hi, zero, IVP_SELI_ROTATE_LEFT_4),   \
                                                                  IVP_SELNX16I(scan_v2_lo, zero, IVP_SELI_ROTATE_LEFT_4));  \
    XI_ADDN_U32(scan_v4_hi,  scan_v4_lo,  scan_v3_hi, scan_v3_lo, IVP_SELNX16I(scan_v3_hi, zero, IVP_SELI_ROTATE_LEFT_8),   \
                                                                  IVP_SELNX16I(scan_v3_lo, zero, IVP_SELI_ROTATE_LEFT_8));  \
    XI_ADDN_U32(scan_out_hi, scan_out_lo, scan_v4_hi, scan_v4_lo, IVP_SELNX16I(scan_v4_hi, zero, IVP_SELI_ROTATE_LEFT_16),  \
                                                                  IVP_SELNX16I(scan_v4_lo, zero, IVP_SELI_ROTATE_LEFT_16)); \
}

// 32-bit prefix sum of 2*N signed 16-bit integers
#define XI_SCAN2NW_S16(scan_out0_hi, scan_out0_lo, scan_out1_hi, scan_out1_lo, scan_in0, scan_in1, zero)                            \
{                                                                                                                                   \
    xb_vecNx16 scan_v2_hi, scan_v2_lo, scan_v3_hi, scan_v3_lo, scan_v4_hi, scan_v4_lo;                                              \
    xb_vecNx16 scan_w2_hi, scan_w2_lo, scan_w3_hi, scan_w3_lo, scan_w4_hi, scan_w4_lo;                                              \
    xb_vecNx16 scan_w1_hi = IVP_SRAINX16(scan_in1, 15); /* sign-extend */                                                           \
    xb_vecNx16 scan_v1_hi = IVP_SRAINX16(scan_in0, 15); /* sign-extend */                                                           \
    xb_vecNx16 scan_w1_lo = scan_in1;                                                                                               \
    xb_vecNx16 scan_v1_lo = scan_in0;                                                                                               \
    XI_ADDNW_S16(scan_w1_hi, scan_w1_lo,  scan_w1_hi, scan_w1_lo,   IVP_SELNX16I(scan_in1, scan_in0, IVP_SELI_ROTATE_LEFT_1));      \
    XI_ADDNW_S16(scan_v1_hi, scan_v1_lo,  scan_v1_hi, scan_v1_lo,   IVP_SELNX16I(scan_in0, zero,     IVP_SELI_ROTATE_LEFT_1));      \
    XI_ADDN_U32(scan_w2_hi,  scan_w2_lo,  scan_w1_hi, scan_w1_lo,   IVP_SELNX16I(scan_w1_hi, scan_v1_hi, IVP_SELI_ROTATE_LEFT_2),   \
                                                                    IVP_SELNX16I(scan_w1_lo, scan_v1_lo, IVP_SELI_ROTATE_LEFT_2));  \
    XI_ADDN_U32(scan_v2_hi,  scan_v2_lo,  scan_v1_hi, scan_v1_lo,   IVP_SELNX16I(scan_v1_hi, zero,       IVP_SELI_ROTATE_LEFT_2),   \
                                                                    IVP_SELNX16I(scan_v1_lo, zero,       IVP_SELI_ROTATE_LEFT_2));  \
    XI_ADDN_U32(scan_w3_hi,  scan_w3_lo,  scan_w2_hi, scan_w2_lo,   IVP_SELNX16I(scan_w2_hi, scan_v2_hi, IVP_SELI_ROTATE_LEFT_4),   \
                                                                    IVP_SELNX16I(scan_w2_lo, scan_v2_lo, IVP_SELI_ROTATE_LEFT_4));  \
    XI_ADDN_U32(scan_v3_hi,  scan_v3_lo,  scan_v2_hi, scan_v2_lo,   IVP_SELNX16I(scan_v2_hi, zero,       IVP_SELI_ROTATE_LEFT_4),   \
                                                                    IVP_SELNX16I(scan_v2_lo, zero,       IVP_SELI_ROTATE_LEFT_4));  \
    XI_ADDN_U32(scan_w4_hi,  scan_w4_lo,  scan_w3_hi, scan_w3_lo,   IVP_SELNX16I(scan_w3_hi, scan_v3_hi, IVP_SELI_ROTATE_LEFT_8),   \
                                                                    IVP_SELNX16I(scan_w3_lo, scan_v3_lo, IVP_SELI_ROTATE_LEFT_8));  \
    XI_ADDN_U32(scan_v4_hi,  scan_v4_lo,  scan_v3_hi, scan_v3_lo,   IVP_SELNX16I(scan_v3_hi, zero,       IVP_SELI_ROTATE_LEFT_8),   \
                                                                    IVP_SELNX16I(scan_v3_lo, zero,       IVP_SELI_ROTATE_LEFT_8));  \
    XI_ADDN_U32(scan_out1_hi, scan_out1_lo, scan_w4_hi, scan_w4_lo, IVP_SELNX16I(scan_w4_hi, scan_v4_hi, IVP_SELI_ROTATE_LEFT_16),  \
                                                                    IVP_SELNX16I(scan_w4_lo, scan_v4_lo, IVP_SELI_ROTATE_LEFT_16)); \
    XI_ADDN_U32(scan_out0_hi, scan_out0_lo, scan_v4_hi, scan_v4_lo, IVP_SELNX16I(scan_v4_hi, zero,       IVP_SELI_ROTATE_LEFT_16),  \
                                                                    IVP_SELNX16I(scan_v4_lo, zero,       IVP_SELI_ROTATE_LEFT_16)); \
    XI_ADDN_U32(scan_out1_hi, scan_out1_lo, scan_out1_hi, scan_out1_lo, scan_out0_hi, scan_out0_lo);                                \
}

#ifdef __XCC__
// fractional division with 8-bit result
#define XI_QUONX16Q_8U(quo, dvdnd, dvsr)                    \
{                                                           \
    xb_vecNx16 tmp_quo, tmp_pr, tmp_quo2, tmp_pr2;          \
    IVP_DIVNX16Q_4STEP0(tmp_quo, tmp_pr, dvdnd, dvsr, 0);   \
    IVP_DIVNX16Q_4STEP0(tmp_quo2, tmp_pr2, dvdnd, dvsr, 1); \
    IVP_DIVNX16U_4STEPN(tmp_quo, tmp_pr, dvsr, 0);          \
    IVP_DIVNX16U_4STEPN(tmp_quo2, tmp_pr2, dvsr, 1);        \
    quo = IVP_ORNX16(tmp_quo2, tmp_quo);                    \
}
#else
// fractional division with 8-bit result
#define XI_QUONX16Q_8U(quo, dvdnd, dvsr)                        \
{                                                               \
    xb_vecNx16U tmp_quo, tmp_pr, tmp_quo2, tmp_pr2;             \
    xb_vecNx16 tmp_quo_s, tmp_pr_s, tmp_quo2_s, tmp_pr2_s;      \
    IVP_DIVNX16Q_4STEP0(tmp_quo_s, tmp_pr_s, dvdnd, dvsr, 0);   \
    IVP_DIVNX16Q_4STEP0(tmp_quo2_s, tmp_pr2_s, dvdnd, dvsr, 1); \
    tmp_quo  = tmp_quo_s;  tmp_pr  = tmp_pr_s;                  \
    tmp_quo2 = tmp_quo2_s; tmp_pr2 = tmp_pr2_s;                 \
    IVP_DIVNX16U_4STEPN(tmp_quo, tmp_pr, dvsr, 0);              \
    IVP_DIVNX16U_4STEPN(tmp_quo2, tmp_pr2, dvsr, 1);            \
    quo = IVP_ORNX16(tmp_quo2, tmp_quo);                        \
}
#endif

#ifdef __XCC__
// fractional division with 12-bit result
#define XI_QUONX16Q_12U(quo, dvdnd, dvsr)                    \
{                                                            \
    xb_vecNx16U tmp_quo, tmp_pr, tmp_quo2, tmp_pr2;          \
    IVP_DIVNX16Q_4STEP0(tmp_quo, tmp_pr, dvdnd, dvsr, 0);    \
    IVP_DIVNX16Q_4STEP0(tmp_quo2, tmp_pr2, dvdnd, dvsr, 1);  \
    IVP_DIVNX16U_4STEP(tmp_quo, tmp_pr, dvsr, 0);            \
    IVP_DIVNX16U_4STEP(tmp_quo2, tmp_pr2, dvsr, 1);          \
    IVP_DIVNX16U_4STEPN(tmp_quo, tmp_pr, dvsr, 0);           \
    IVP_DIVNX16U_4STEPN(tmp_quo2, tmp_pr2, dvsr, 1);         \
    quo = IVP_ORNX16(tmp_quo2, tmp_quo);                     \
}
#else
#define XI_QUONX16Q_12U(quo, dvdnd, dvsr)                       \
{                                                               \
    xb_vecNx16U tmp_quo, tmp_pr, tmp_quo2, tmp_pr2;             \
    xb_vecNx16 tmp_quo_s, tmp_pr_s, tmp_quo2_s, tmp_pr2_s;      \
    IVP_DIVNX16Q_4STEP0(tmp_quo_s, tmp_pr_s, dvdnd, dvsr, 0);   \
    IVP_DIVNX16Q_4STEP0(tmp_quo2_s, tmp_pr2_s, dvdnd, dvsr, 1); \
    tmp_quo  = tmp_quo_s;  tmp_pr  = tmp_pr_s;                  \
    tmp_quo2 = tmp_quo2_s; tmp_pr2 = tmp_pr2_s;                 \
    IVP_DIVNX16U_4STEP(tmp_quo, tmp_pr, dvsr, 0);               \
    IVP_DIVNX16U_4STEP(tmp_quo2, tmp_pr2, dvsr, 1);             \
    IVP_DIVNX16U_4STEPN(tmp_quo, tmp_pr, dvsr, 0);              \
    IVP_DIVNX16U_4STEPN(tmp_quo2, tmp_pr2, dvsr, 1);            \
    quo = IVP_ORNX16(tmp_quo2, tmp_quo);                        \
}
#endif

#if defined (XCHAL_HAVE_VISION) && XCHAL_HAVE_VISION
// 8 high bits of unsigned X32/X16 division
#define XI_DIVNX32X16U_8HI(quo, rem, dvdnd_hi, dvdn_lo, dvsr)               \
{                                                                           \
    xb_vecNx16U rem0 = dvdnd_hi;                                            \
    xb_vecNx16U quo0 = IVP_SELNX16I(dvdn_lo, 0, IVP_SELI_ROTATE_LEFT_1);    \
    xb_vecNx16U rem1 = IVP_SELNX16I(0, dvdnd_hi, IVP_SELI_ROTATE_RIGHT_1);  \
    xb_vecNx16U quo1 = dvdn_lo;                                             \
    IVP_DIVNX16U_4STEP(quo0, rem0, dvsr, 0);                                \
    IVP_DIVNX16U_4STEP(quo1, rem1, dvsr, 1);                                \
    IVP_DIVNX16U_4STEPN(quo0, rem0, dvsr, 0);                               \
    IVP_DIVNX16U_4STEPN(quo1, rem1, dvsr, 1);                               \
    quo = IVP_ORNX16(quo0, quo1);                                           \
    rem = IVP_ORNX16(rem0, rem1);                                           \
}
// unsigned X32/X16 division with 16-bit result
#define XI_DIVNX32X16U(quo, rem, dvdnd_hi, dvdn_lo, dvsr)                   \
{                                                                           \
    xb_vecNx16U rem0 = dvdnd_hi;                                            \
    xb_vecNx16U quo0 = IVP_SELNX16I(dvdn_lo, 0, IVP_SELI_ROTATE_LEFT_1);    \
    xb_vecNx16U rem1 = IVP_SELNX16I(0, dvdnd_hi, IVP_SELI_ROTATE_RIGHT_1);  \
    xb_vecNx16U quo1 = dvdn_lo;                                             \
    IVP_DIVNX16U_4STEP(quo0, rem0, dvsr, 0);                                \
    IVP_DIVNX16U_4STEP(quo1, rem1, dvsr, 1);                                \
    IVP_DIVNX16U_4STEP(quo0, rem0, dvsr, 0);                                \
    IVP_DIVNX16U_4STEP(quo1, rem1, dvsr, 1);                                \
    IVP_DIVNX16U_4STEP(quo0, rem0, dvsr, 0);                                \
    IVP_DIVNX16U_4STEP(quo1, rem1, dvsr, 1);                                \
    IVP_DIVNX16U_4STEPN(quo0, rem0, dvsr, 0);                               \
    IVP_DIVNX16U_4STEPN(quo1, rem1, dvsr, 1);                               \
    quo = IVP_ORNX16(quo0, quo1);                                           \
    /*rem = IVP_ORNX16(rem0, rem1);*/                                       \
    /* reminder returned by IVP_DIVNX16U_4STEPN is 0 if quo = 65535 */      \
    rem = dvdn_lo; IVP_MULSNX16PACKL(rem, quo, dvsr);                       \
}
#else
// 8 high bits of unsigned X32/X16 division
#define XI_DIVNX32X16U_8HI(quo, rem, dvdnd_hi, dvdn_lo, dvsr)               \
{                                                                           \
    xb_vecNx16U rem0 = dvdnd_hi;                                            \
    xb_vecNx16U quo0 = dvdn_lo;                                             \
    xb_vecNx16U rem1 = dvdnd_hi;                                            \
    xb_vecNx16U quo1 = dvdn_lo;                                             \
    IVP_DIVNX16U_4STEP(quo0, rem0, dvsr, 0);                                \
    IVP_DIVNX16U_4STEP(quo1, rem1, dvsr, 1);                                \
    IVP_DIVNX16U_4STEPN(quo0, rem0, dvsr, 0);                               \
    IVP_DIVNX16U_4STEPN(quo1, rem1, dvsr, 1);                               \
    quo = IVP_ORNX16(quo0, quo1);                                           \
    rem = IVP_ORNX16(rem0, rem1);                                           \
}
// unsigned division with 16-bit result
#define XI_DIVNX32X16U(quo, rem, dvdnd_hi, dvdn_lo, dvsr)                   \
{                                                                           \
    xb_vecNx16U rem0 = dvdnd_hi;                                            \
    xb_vecNx16U quo0 = dvdn_lo;                                             \
    xb_vecNx16U rem1 = dvdnd_hi;                                            \
    xb_vecNx16U quo1 = dvdn_lo;                                             \
    IVP_DIVNX16U_4STEP(quo0, rem0, dvsr, 0);                                \
    IVP_DIVNX16U_4STEP(quo1, rem1, dvsr, 1);                                \
    IVP_DIVNX16U_4STEP(quo0, rem0, dvsr, 0);                                \
    IVP_DIVNX16U_4STEP(quo1, rem1, dvsr, 1);                                \
    IVP_DIVNX16U_4STEP(quo0, rem0, dvsr, 0);                                \
    IVP_DIVNX16U_4STEP(quo1, rem1, dvsr, 1);                                \
    IVP_DIVNX16U_4STEPN(quo0, rem0, dvsr, 0);                               \
    IVP_DIVNX16U_4STEPN(quo1, rem1, dvsr, 1);                               \
    quo = IVP_ORNX16(quo0, quo1);                                           \
    rem = IVP_ORNX16(rem0, rem1);                                           \
}
#endif

// sum reduction from xb_vecN_2x32 into uint32_t
#ifndef IVP_RADD_vecN_2x32U_to_u32
#   define IVP_RADD_vecN_2x32U_to_u32(sum, vec)                                           \
    {                                                                                     \
        xb_vecNx16 vh, vl;                                                                \
        IVP_DSELNX16I(vh, vl, IVP_MOVNX16_FROMN_2X32U(vec), 0, IVP_DSELI_DEINTERLEAVE_1); \
        sum = IVP_RADDURNX16(vl) + (IVP_RADDURNX16(vh) << 16);                            \
    }
#endif

#define XI_ROOTSTEP_U16(x, root, place)           \
    {                                             \
        xb_vecNx16 rp = IVP_ADDNX16(root, place); \
        root = IVP_SRLINX16(root, 1);             \
        vboolN b = IVP_LEUNX16(rp, x);            \
        IVP_SUBNX16T(x, x, rp, b);                \
        IVP_ADDNX16T(root, root, place, b);       \
    }

#define XI_ROOT_U32(root, input_h, input_l)                                         \
    {                                                                               \
        root = IVP_MOVNX16T(1 << 14, 0, IVP_LEUNX16(1 << 14, input_h));             \
        xb_vecNx16 tmp = IVP_SUBNX16(input_h, root);                                \
                                                                                    \
        XI_ROOTSTEP_U16(tmp, root, 1 << 12);                                        \
        XI_ROOTSTEP_U16(tmp, root, 1 << 10);                                        \
        XI_ROOTSTEP_U16(tmp, root, 1 << 8);                                         \
        XI_ROOTSTEP_U16(tmp, root, 1 << 6);                                         \
        XI_ROOTSTEP_U16(tmp, root, 1 << 4);                                         \
        XI_ROOTSTEP_U16(tmp, root, 1 << 2);                                         \
                                                                                    \
        IVP_ADDNX16T(root, root, 2, IVP_LTUNX16(root, tmp));                        \
        root = IVP_SRLINX16(root, 1);                                               \
                                                                                    \
        IVP_MULSNX16PACKL(input_h, root, root);                                     \
        xb_vecNx16 rp_h = IVP_SRLINX16(root, 1);                                    \
                                                                                    \
        vboolN b = XI_LEUNX32(root, 1 << 14, input_h, input_l);                     \
        XI_SUBN_I32T(input_h, input_l, input_h, input_l, root, 1 << 14, b);         \
                                                                                    \
        root = IVP_SLLINX16(root, 8);                                               \
        IVP_ADDNX16T(root, root, 1 << 7, b);                                        \
        xb_vecNx16 rp_l;                                                            \
                                                                                    \
        rp_l = IVP_SLLINX16(IVP_ADDNX16(root, 1 << 5), 7);                          \
        rp_h = IVP_SRLINX16(root, 9);                                               \
        b = XI_LEUNX32(rp_h, rp_l, input_h, input_l);                               \
        XI_SUBN_I32T(input_h, input_l, input_h, input_l, rp_h, rp_l, b);            \
        IVP_ADDNX16T(root, root, 1 << 6, b);                                        \
                                                                                    \
        rp_l = IVP_SLLINX16(IVP_ADDNX16(root, 1 << 4), 6);                          \
        rp_h = IVP_SRLINX16(root, 10);                                              \
        b = XI_LEUNX32(rp_h, rp_l, input_h, input_l);                               \
        XI_SUBN_I32T(input_h, input_l, input_h, input_l, rp_h, rp_l, b);            \
        IVP_ADDNX16T(root, root, 1 << 5, b);                                        \
                                                                                    \
        rp_l = IVP_SLLINX16(IVP_ADDNX16(root, 1 << 3), 5);                          \
        rp_h = IVP_SRLINX16(root, 11);                                              \
        b = XI_LEUNX32(rp_h, rp_l, input_h, input_l);                               \
        XI_SUBN_I32T(input_h, input_l, input_h, input_l, rp_h, rp_l, b);            \
        IVP_ADDNX16T(root, root, 1 << 4, b);                                        \
                                                                                    \
        rp_l = IVP_SLLINX16(IVP_ADDNX16(root, 1 << 2), 4);                          \
        rp_h = IVP_SRLINX16(root, 12);                                              \
        b = XI_LEUNX32(rp_h, rp_l, input_h, input_l);                               \
        XI_SUBN_I32T(input_h, input_l, input_h, input_l, rp_h, rp_l, b);            \
        IVP_ADDNX16T(root, root, 1 << 3, b);                                        \
                                                                                    \
        rp_l = IVP_SLLINX16(IVP_ADDNX16(root, 1 << 1), 3);                          \
        rp_h = IVP_SRLINX16(root, 13);                                              \
        b = XI_LEUNX32(rp_h, rp_l, input_h, input_l);                               \
        XI_SUBN_I32T(input_h, input_l, input_h, input_l, rp_h, rp_l, b);            \
        IVP_ADDNX16T(root, root, 1 << 2, b);                                        \
                                                                                    \
        rp_l = IVP_SLLINX16(IVP_ADDNX16(root, 1 << 0), 2);                          \
        rp_h = IVP_SRLINX16(root, 14);                                              \
        b = XI_LEUNX32(rp_h, rp_l, input_h, input_l);                               \
        XI_SUBN_I32T(input_h, input_l, input_h, input_l, rp_h, rp_l, b);            \
        IVP_ADDNX16T(root, root, 1 << 1, b);                                        \
                                                                                    \
        rp_l = IVP_ADDNX16(IVP_SLLINX16(root, 1), 1);                               \
        rp_h = IVP_SRLINX16(root, 15);                                              \
        IVP_ADDNX16T(root, root, 1 << 0, XI_LEUNX32(rp_h, rp_l, input_h, input_l)); \
    }

#define XI_MUL_U32_U32_U64(n64, n48, n32, n16, n1h, n1l, n2h, n2l) \
    {                                                              \
                                                                   \
        xb_vecNx48 x = IVP_MULUUNX16(n1l, n2l);                    \
        xb_vecNx16 t = IVP_PACKMNX48(x);                           \
        n16 = IVP_PACKLNX48(x);                                    \
                                                                   \
        xb_vecNx48 y = IVP_MULUUNX16(n1h, n2h);                    \
        xb_vecNx16 yl = IVP_PACKLNX48(y);                          \
        xb_vecNx16 yh = IVP_PACKMNX48(y);                          \
                                                                   \
        xb_vecNx48 z = IVP_MULUUNX16(n1h, n2l);                    \
        IVP_MULUUANX16(z, n2h, n1l);                               \
        IVP_MULUUANX16(z, t, 1);                                   \
        IVP_MULUUANX16(z, yl, 1 << 15);                            \
        IVP_MULUUANX16(z, yl, 1 << 15);                            \
                                                                   \
        n32 = IVP_PACKLNX48(z);                                    \
        n48 = IVP_PACKMNX48(z);                                    \
        n64 = IVP_ADDNX16(yh, IVP_PACKHNX48(z));                   \
    }

#define XI_MUL_U32_U32_U64HIGH(n64, n48, n1h, n1l, n2h, n2l)   \
    {                                                          \
        xb_vecNx16 t = IVP_PACKMNX48(IVP_MULUUNX16(n1l, n2l)); \
        xb_vecNx48 z = IVP_MULUUNX16(n1h, n2l);                \
        IVP_MULUUANX16(z, n2h, n1l);                           \
        IVP_MULUUANX16(z, t, 1);                               \
        xb_vecNx16 t2 = IVP_PACKMNX48(z);                      \
        xb_vecNx16 t3 = IVP_PACKHNX48(z);                      \
        xb_vecNx48 y = IVP_MULUUNX16(n1h, n2h);                \
        IVP_MULUUANX16(y, t2, 1);                              \
        n48 = IVP_PACKLNX48(y);                                \
        n64 = IVP_ADDNX16(t3, IVP_PACKMNX48(y));               \
    }

#define XI_NORMALIZE_U32(inout_h, inout_l, ind)                                                                   \
    {                                                                                                             \
        vsaN a = IVP_NSAUNX16(inout_h);                                                                           \
        ind = IVP_MOVVVS(a);                                                                                      \
        inout_h = IVP_ORNX16(IVP_SLLNX16(inout_h, a), IVP_SRLNX16(inout_l, IVP_MOVVSV(IVP_SUBNX16(16, ind), 0))); \
        inout_l = IVP_SLLNX16(inout_l, a);                                                                        \
        vsaN b = IVP_NSAUNX16(inout_h);                                                                           \
        ind = IVP_ADDNX16(ind, IVP_MOVVVS(b));                                                                    \
        inout_h = IVP_SLLNX16(inout_h, b);                                                                        \
    }

#define XI_NSANX16_S48(out_vsht, in_h, in_m, in_l)                             \
{                                                                              \
    vboolN vbh = IVP_EQNX16(in_h, 0);                                          \
    vboolN vbm = IVP_EQNX16(in_m, 0);                                          \
    out_vsht   = IVP_NSANX16(in_h);                                            \
    IVP_ADDNX16T(out_vsht, out_vsht, IVP_NSAUNX16(in_m), vbh);                 \
    IVP_ADDNX16T(out_vsht, out_vsht, IVP_NSAUNX16(in_l), IVP_ANDBN(vbh, vbm)); \
}

#if XCHAL_HAVE_VISION
#  define GET_VAL_FROM_VEC_1(vec) IVP_EXTRNX16(vec, 1)
#  define GET_VAL_FROM_VEC_2(vec) IVP_EXTRNX16(vec, 2)
#  define GET_VAL_FROM_VEC_3(vec) IVP_EXTRNX16(vec, 3)
#  define GET_VAL_FROM_VEC_4(vec) IVP_EXTRNX16(vec, 4)
#  define GET_VAL_FROM_VEC_5(vec) IVP_EXTRNX16(vec, 5)
#  define GET_VAL_FROM_VEC_6(vec) IVP_EXTRNX16(vec, 6)
#  define GET_VAL_FROM_VEC_7(vec) IVP_EXTRNX16(vec, 7)
#  define GET_VAL_FROM_VEC_8(vec) IVP_EXTRNX16(vec, 8)
#  define GET_VAL_FROM_VEC_9(vec) IVP_EXTRNX16(vec, 9)
#else
#  define GET_VAL_FROM_VEC_1(vec) IVP_MOVAV16(IVP_SHFLNX16I(vec, IVP_SHFLI_REP_1X1))
#  define GET_VAL_FROM_VEC_2(vec) IVP_MOVAV16(IVP_SHFLNX16I(vec, IVP_SHFLI_REP_2X1))
#  define GET_VAL_FROM_VEC_3(vec) IVP_MOVAV16(IVP_SHFLNX16I(vec, IVP_SHFLI_REP_3X1))
#  define GET_VAL_FROM_VEC_4(vec) IVP_MOVAV16(IVP_SHFLNX16I(vec, IVP_SHFLI_REP_4X1))
#  define GET_VAL_FROM_VEC_5(vec) IVP_MOVAV16(IVP_SHFLNX16I(vec, IVP_SHFLI_REP_5X1))
#  define GET_VAL_FROM_VEC_6(vec) IVP_MOVAV16(IVP_SHFLNX16I(vec, IVP_SHFLI_REP_6X1))
#  define GET_VAL_FROM_VEC_7(vec) IVP_MOVAV16(IVP_SHFLNX16I(vec, IVP_SHFLI_REP_7X1))
#  define GET_VAL_FROM_VEC_8(vec) IVP_MOVAV16(IVP_SHFLNX16I(vec, IVP_SHFLI_REP_8X1))
#  define GET_VAL_FROM_VEC_9(vec) IVP_MOVAV16(IVP_SHFLNX16I(vec, IVP_SHFLI_REP_9X1))
#endif

#define GET_VAL_FROM_VEC_0(vec) IVP_MOVAV16(vec)
#define GET_VAL_FROM_VEC_I(vec, iindex) IVP_MOVAV16(IVP_SHFLNX16(vec, IVP_MOVVSV(IVP_ADDNX16(IVP_SEQNX16(), iindex), 0)))

#endif // __XI_WIDE_ARITHM_H__
