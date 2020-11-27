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
#ifndef __XI_INTRIN_H__
#define __XI_INTRIN_H__

#include <xtensa/tie/xt_ivpn.h>
#include <xtensa/tie/xt_misc.h>

#if (PORT_TO_P1)
#undef IVP_SCATTERNX8UT
#define IVP_SCATTERNX8UT(_a, __b, _c, _d) \
{\
	xb_vecNx16 vecl, vecs, vecr; \
	signed char * _b = (signed char *) __b; \
	vecs = IVP_MOVNX16_FROMNX16U(_a); \
	vecl = IVP_MOVNX16_FROM2NX8(IVP_LSR2NX8_X(_b, IVP_EXTRNX16(_c, 0))); \
	vecr = IVP_MOVNX16T(vecs, vecl, _d); \
	IVP_SSNX8S_X(vecr, _b, IVP_EXTRNX16(_c, 0)); \
\
	vecl = IVP_MOVNX16_FROM2NX8(IVP_LSR2NX8_X(_b, IVP_EXTRNX16(_c, 1))); \
	vecr = IVP_MOVNX16T(vecs, vecl, _d); \
	vecr = IVP_SELNX16((xb_vecNx16)0, vecr, IVP_SEQNX16() + (xb_vecNx16)1); \
	IVP_SSNX8S_X(vecr, _b, IVP_EXTRNX16(_c, 1)); \
\
	vecl = IVP_MOVNX16_FROM2NX8(IVP_LSR2NX8_X(_b, IVP_EXTRNX16(_c, 2))); \
	vecr = IVP_MOVNX16T(vecs, vecl, _d); \
	vecr = IVP_SELNX16((xb_vecNx16)0, vecr, IVP_SEQNX16() + (xb_vecNx16)2); \
	IVP_SSNX8S_X(vecr, _b, IVP_EXTRNX16(_c, 2)); \
\
	vecl = IVP_MOVNX16_FROM2NX8(IVP_LSR2NX8_X(_b, IVP_EXTRNX16(_c, 3))); \
	vecr = IVP_MOVNX16T(vecs, vecl, _d); \
	vecr = IVP_SELNX16((xb_vecNx16)0, vecr, IVP_SEQNX16() + (xb_vecNx16)3); \
	IVP_SSNX8S_X(vecr, _b, IVP_EXTRNX16(_c, 3)); \
\
	vecl = IVP_MOVNX16_FROM2NX8(IVP_LSR2NX8_X(_b, IVP_EXTRNX16(_c, 4))); \
	vecr = IVP_MOVNX16T(vecs, vecl, _d); \
	vecr = IVP_SELNX16((xb_vecNx16)0, vecr, IVP_SEQNX16() + (xb_vecNx16)4); \
	IVP_SSNX8S_X(vecr, _b, IVP_EXTRNX16(_c, 4)); \
\
	vecl = IVP_MOVNX16_FROM2NX8(IVP_LSR2NX8_X(_b, IVP_EXTRNX16(_c, 5))); \
	vecr = IVP_MOVNX16T(vecs, vecl, _d); \
	vecr = IVP_SELNX16((xb_vecNx16)0, vecr, IVP_SEQNX16() + (xb_vecNx16)5); \
	IVP_SSNX8S_X(vecr, _b, IVP_EXTRNX16(_c, 5)); \
\
	vecl = IVP_MOVNX16_FROM2NX8(IVP_LSR2NX8_X(_b, IVP_EXTRNX16(_c, 6))); \
	vecr = IVP_MOVNX16T(vecs, vecl, _d); \
	vecr = IVP_SELNX16((xb_vecNx16)0, vecr, IVP_SEQNX16() + (xb_vecNx16)6); \
	IVP_SSNX8S_X(vecr, _b, IVP_EXTRNX16(_c, 6)); \
\
	vecl = IVP_MOVNX16_FROM2NX8(IVP_LSR2NX8_X(_b, IVP_EXTRNX16(_c, 7))); \
	vecr = IVP_MOVNX16T(vecs, vecl, _d); \
	vecr = IVP_SELNX16((xb_vecNx16)0, vecr, IVP_SEQNX16() + (xb_vecNx16)7); \
	IVP_SSNX8S_X(vecr, _b, IVP_EXTRNX16(_c, 7)); \
}

#undef IVP_SCATTERW
#define IVP_SCATTERW()

//extern xb_gsr IVP_GATHERANX8UT(const unsigned char * b, xb_vecNx16U c, vboolN d);
//proto IVP_GATHERDNX8U { out xb_vecNx16U a, in xb_gsr b }{}{

#undef xb_gsr
#define xb_gsr xb_vecNx16U

#undef IVP_GATHERANX8UT
#define IVP_GATHERANX8UT(__a, _b, _c) \
({\
    signed char * _a = (signed char *) __a; \
	xb_vecNx16 dataAndOffsets = (xb_vecNx16)_b; \
	xb_vecNx16U out; \
	int _i; \
	for (_i = 0; _i < (XCHAL_IVPN_SIMD_WIDTH); _i++) \
	{ \
		int32_t offset = (int32_t) IVP_EXTRNX16(dataAndOffsets,0); \
		xb_int16U data = xb_int16_rtor_xb_int16U(IVP_LSNX8S_X(_a, offset)); \
		data = data & (xb_int16U)0x00FF; \
		/* printf("At _i = %d, data = %d\n", _i, (uint16_t)data); */ \
		out = IVP_SELNX16U(IVP_MOVNX16U_FROM16U(data), out, IVP_SEQNX16() + (xb_vecNx16)1); \
		dataAndOffsets = IVP_SELNX16((xb_vecNx16)0, dataAndOffsets, IVP_SEQNX16() + (xb_vecNx16)1); \
	} \
	out = IVP_MOVNX16UT(out, (xb_vecNx16U)0, _c); \
	out; \
})

#undef IVP_GATHERDNX8U
#define IVP_GATHERDNX8U

#endif

#ifndef XCHAL_HAVE_VISION
#  define XCHAL_HAVE_VISION 1
#endif

////////// CSTUBS workarounds

#if defined(_MSC_VER) && !XCHAL_HAVE_VISION
#   undef IVP_ABSSUBNX16
#   define IVP_ABSSUBNX16(a,b) IVP_MAXNX16(IVP_SUBNX16(b,a),IVP_SUBNX16(a,b))
#endif

#if !defined(__XCC__) && !XCHAL_HAVE_VISION
typedef vselN _xi_intrin_private_xb_vselN;
#   undef IVP_SQZN
#   define IVP_SQZN(a, b, c) do{_xi_intrin_private_xb_vselN _sqzntmp;CSTUB_(_TIE_xt_ivp32_IVP_SQZN)(_sqzntmp,b,c);a =_sqzntmp;}while(0)
#   undef IVP_UNSQZN
#   define IVP_UNSQZN(a, b, c) do{_xi_intrin_private_xb_vselN _sqzntmp;CSTUB_(_TIE_xt_ivp32_IVP_UNSQZN)(_sqzntmp,b,c);a =_sqzntmp;}while(0)
#endif

#if !defined(__XCC__) && XCHAL_HAVE_VISION

#undef IVP_SCATTERNX8U
#undef IVP_SCATTERNX8UT
#define IVP_SCATTERNX8U(val__, ptr__, offs__)                        \
{                                                                       \
    vboolN mask = IVP_LTNX16(0, 1);                                     \
    xb_vecNx16 mask16 = IVP_MOVNX16T(1, 0, mask);                       \
    xb_vecNx16U offs1 = (offs__);                                       \
    xb_vecNx16 val1 = val__;                                            \
    for (int i = 0; i < 32; i++)                                        \
    {                                                                   \
        int v = IVP_MOVAVU16(val1);                                     \
        int o = IVP_MOVAVU16(offs1);                                    \
        int m = IVP_MOVAVU16(mask16);                                   \
        if (m) *((uint8_t*)(ptr__) + o) = v;                            \
        val1 = IVP_SELNX16I(0, val1, IVP_SELI_16B_ROTATE_RIGHT_1);      \
        mask16 = IVP_SELNX16I(0, mask16, IVP_SELI_16B_ROTATE_RIGHT_1);  \
        offs1 = IVP_SELNX16I(0, offs1, IVP_SELI_16B_ROTATE_RIGHT_1);    \
    }                                                                   \
}

#define IVP_SCATTERNX8UT(val__, ptr__, offs__, mask__)               \
{                                                                       \
    xb_vecNx16 mask16 = IVP_MOVNX16T(1, 0, (mask__));                   \
    xb_vecNx16 val = (val__);                                           \
    xb_vecNx16 off = (offs__);                                          \
    for (int i = 0; i < 32; i++)                                        \
    {                                                                   \
        int v = IVP_MOVAVU16(val);                                      \
        int o = IVP_MOVAVU16(off);                                      \
        int m = IVP_MOVAVU16(mask16);                                   \
        if (m) *(((uint8_t*)ptr__) + o) = v;                            \
        val = IVP_SELNX16I(0, val, IVP_SELI_16B_ROTATE_RIGHT_1);        \
        mask16 = IVP_SELNX16I(0, mask16, IVP_SELI_16B_ROTATE_RIGHT_1);  \
        off = IVP_SELNX16I(0, off, IVP_SELI_16B_ROTATE_RIGHT_1);        \
    }                                                                   \
}

#undef IVP_SCATTERN_2X32
#undef IVP_SCATTERN_2X32T
#define IVP_SCATTERN_2X32(val__, ptr__, offs__)                        \
{                                                                       \
    vboolN_2 mask = IVP_LTN_2X32(0, 1);                                 \
    xb_vecN_2x32v mask32 = IVP_MOVN_2X32T(1, 0, mask);                  \
    xb_vecN_2x32v offs1 = IVP_SRLIN_2X32(offs__, 2);                    \
    xb_vecN_2x32v val1 = val__;                                         \
    for (int i = 0; i < 16; i++)                                        \
    {                                                                   \
        int v = IVP_MOVAV32(val1);                                      \
        int o = IVP_MOVAV32(offs1);                                     \
        int m = IVP_MOVAV32(mask32);                                    \
        if (m) *((ptr__) + o) = v;                                      \
        val1 = IVP_MOVN_2X32_FROMNX16(IVP_SELNX16I(0, IVP_MOVNX16_FROMN_2X32(val1), IVP_SELI_32B_ROTATE_RIGHT_1)); \
        mask32 = IVP_MOVN_2X32_FROMNX16(IVP_SELNX16I(0, IVP_MOVNX16_FROMN_2X32(mask32), IVP_SELI_32B_ROTATE_RIGHT_1)); \
        offs1 = IVP_MOVN_2X32_FROMNX16(IVP_SELNX16I(0, IVP_MOVNX16_FROMN_2X32(offs1), IVP_SELI_32B_ROTATE_RIGHT_1)); \
    }                                                                   \
}

#define IVP_SCATTERN_2X32T(val__, ptr__, offs__, mask__)               \
{                                                                       \
    xb_vecN_2x32v mask32 = IVP_MOVN_2X32T(1, 0, mask__);                \
    xb_vecN_2x32v offs1 = IVP_SRLIN_2X32(offs__, 2);                    \
    xb_vecN_2x32v val1 = val__;                                         \
    for (int i = 0; i < 16; i++)                                        \
    {                                                                   \
        int v = IVP_MOVAV32(val1);                                      \
        int o = IVP_MOVAV32(offs1);                                     \
        int m = IVP_MOVAV32(mask32);                                    \
        if (m) *((ptr__) + o) = v;                                      \
        val1 = IVP_MOVN_2X32_FROMNX16(IVP_SELNX16I(0, IVP_MOVNX16_FROMN_2X32(val1), IVP_SELI_32B_ROTATE_RIGHT_1)); \
        mask32 = IVP_MOVN_2X32_FROMNX16(IVP_SELNX16I(0, IVP_MOVNX16_FROMN_2X32(mask32), IVP_SELI_32B_ROTATE_RIGHT_1)); \
        offs1 = IVP_MOVN_2X32_FROMNX16(IVP_SELNX16I(0, IVP_MOVNX16_FROMN_2X32(offs1), IVP_SELI_32B_ROTATE_RIGHT_1)); \
    }                                                                   \
}

#undef IVP_SCATTER2NX8U_L
#undef IVP_SCATTER2NX8UT_L
#define IVP_SCATTER2NX8U_L(val__, ptr__, offs__)                        \
{                                                                       \
    vbool2N mask = IVP_LT2NX8(0, 1);                                    \
    xb_vec2Nx8 mask8 = IVP_MOV2NX8T(1, 0, mask);                        \
    xb_vecNx16U offs1 = (offs__);                                       \
    xb_vec2Nx8 val1 = val__;                                            \
    for (int i = 0; i < 32; i++)                                        \
    {                                                                   \
        int v = IVP_MOVAVU8(val1);                                      \
        int o = IVP_MOVAVU16(offs1);                                    \
        int m = IVP_MOVAVU8(mask8);                                     \
        if (m) *((uint8_t*)(ptr__) + o) = v;                            \
        val1 = IVP_SEL2NX8I(0, val1, IVP_SELI_8B_ROTATE_RIGHT_1);       \
        mask8 = IVP_SEL2NX8I(0, mask8, IVP_SELI_8B_ROTATE_RIGHT_1);     \
        offs1 = IVP_SELNX16I(0, offs1, IVP_SELI_16B_ROTATE_RIGHT_1);    \
    }                                                                   \
}

#define IVP_SCATTER2NX8UT_L(val__, ptr__, offs__, mask__)               \
{                                                                       \
    vbool2N mask = mask__;                                              \
    xb_vec2Nx8 mask8 = IVP_MOV2NX8T(1, 0, mask);                        \
    xb_vecNx16U offs1 = (offs__);                                       \
    xb_vec2Nx8 val1 = val__;                                            \
    for (int i = 0; i < 32; i++)                                        \
    {                                                                   \
        int v = IVP_MOVAVU8(val1);                                      \
        int o = IVP_MOVAVU16(offs1);                                    \
        int m = IVP_MOVAVU8(mask8);                                     \
        if (m) *((uint8_t*)(ptr__) + o) = v;                            \
        val1 = IVP_SEL2NX8I(0, val1, IVP_SELI_8B_ROTATE_RIGHT_1);       \
        mask8 = IVP_SEL2NX8I(0, mask8, IVP_SELI_8B_ROTATE_RIGHT_1);     \
        offs1 = IVP_SELNX16I(0, offs1, IVP_SELI_16B_ROTATE_RIGHT_1);    \
    }                                                                   \
}

#undef IVP_SCATTER2NX8U_H
#undef IVP_SCATTER2NX8UT_H
#define IVP_SCATTER2NX8U_H(val__, ptr__, offs__)                        \
{                                                                       \
    vbool2N mask = IVP_LT2NX8(0, 1);                                    \
    xb_vec2Nx8 mask8 = IVP_MOV2NX8T(1, 0, mask);                        \
    xb_vecNx16U offs1 = (offs__);                                       \
    xb_vec2Nx8 val1 = val__;                                            \
                                                                        \
    val1 = IVP_SEL2NX8I(0, val1, IVP_SELI_8B_ROTATE_RIGHT_32);          \
    mask8 = IVP_SEL2NX8I(0, mask8, IVP_SELI_8B_ROTATE_RIGHT_32);        \
    for (int i = 0; i < 32; i++)                                        \
    {                                                                   \
        int v = IVP_MOVAVU8(val1);                                      \
        int o = IVP_MOVAVU16(offs1);                                    \
        int m = IVP_MOVAVU8(mask8);                                     \
        if (m) *((uint8_t*)(ptr__) + o) = v;                            \
        val1 = IVP_SEL2NX8I(0, val1, IVP_SELI_8B_ROTATE_RIGHT_1);       \
        mask8 = IVP_SEL2NX8I(0, mask8, IVP_SELI_8B_ROTATE_RIGHT_1);     \
        offs1 = IVP_SELNX16I(0, offs1, IVP_SELI_16B_ROTATE_RIGHT_1);    \
    }                                                                   \
}

#define IVP_SCATTER2NX8UT_H(val__, ptr__, offs__, mask__)               \
{                                                                       \
    vbool2N mask = mask__;                                              \
    xb_vec2Nx8 mask8 = IVP_MOV2NX8T(1, 0, mask);                        \
    xb_vecNx16U offs1 = (offs__);                                       \
    xb_vec2Nx8 val1 = val__;                                            \
                                                                        \
    val1 = IVP_SEL2NX8I(0, val1, IVP_SELI_8B_ROTATE_RIGHT_32);          \
    mask8 = IVP_SEL2NX8I(0, mask8, IVP_SELI_8B_ROTATE_RIGHT_32);        \
    for (int i = 0; i < 32; i++)                                        \
    {                                                                   \
        int v = IVP_MOVAVU8(val1);                                      \
        int o = IVP_MOVAVU16(offs1);                                    \
        int m = IVP_MOVAVU8(mask8);                                     \
        if (m) *((uint8_t*)(ptr__) + o) = v;                            \
        val1 = IVP_SEL2NX8I(0, val1, IVP_SELI_8B_ROTATE_RIGHT_1);       \
        mask8 = IVP_SEL2NX8I(0, mask8, IVP_SELI_8B_ROTATE_RIGHT_1);     \
        offs1 = IVP_SELNX16I(0, offs1, IVP_SELI_16B_ROTATE_RIGHT_1);    \
    }                                                                   \
}

#undef IVP_GATHERNX8UT_V
#define IVP_GATHERNX8UT_V(pdst, offs, mask, dly)    IVP_MOVNX16T(IVP_GATHERNX8U_V((pdst), (offs), (dly)), 0, mask)

#undef IVP_GATHERNX16T_V
#define IVP_GATHERNX16T_V(pdst, offs, mask, dly)    IVP_MOVNX16T(IVP_GATHERNX16_V((pdst), (offs), (dly)), 0, mask)

#undef IVP_GATHERN_2X32T_V
#define IVP_GATHERN_2X32T_V(pdst, offs, mask, dly)  IVP_MOVN_2X32T(IVP_GATHERN_2X32_V((pdst), (offs), (dly)), 0, mask)

#endif

////////// protos extension

// 32-way wide vector (48-bit) element high 16-bits output to narrow (16-bit) output vector register
#ifndef IVP_PACKHNX48
#   define IVP_PACKHNX48(vec) IVP_PACKVRNR2NX24_1(IVP_MOV2NX24_FROMNX48(vec), 8)
#endif

// reinterpret 64 8-bit elements as 16 32-bit elements
#ifndef IVP_MOVN_2X32_FROM2NX8
#   define IVP_MOVN_2X32_FROM2NX8(vec) IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(vec))
#endif

// reinterpret 16 32-bit elements as 64 8-bit elements
#ifndef IVP_MOV2NX8_FROMN_2X32
#   define IVP_MOV2NX8_FROMN_2X32(vec) IVP_MOV2NX8_FROMNX16(IVP_MOVNX16_FROMN_2X32(vec))
#endif

#ifndef IVP_SELN_2X32I
#   define IVP_SELN_2X32I(a, b, i) IVP_MOVN_2X32_FROMNX16(IVP_SELNX16I(IVP_MOVNX16_FROMN_2X32(a), IVP_MOVNX16_FROMN_2X32(b), i))
#endif

// 0 to 63 sequence xb_vec2Nx8U vector
#ifndef IVP_SEQ2NX8U
#   define IVP_SEQ2NX8U() IVP_MOV2NX8U_FROMNX16(IVP_ADDNX16U(256, IVP_MULNX16UPACKL(514, IVP_SEQNX16())))
#endif

// 64-way 8-bit zero
#ifndef IVP_ZERO2NX8U
#   define IVP_ZERO2NX8U() IVP_MOV2NX8U_FROMNX16(IVP_ZERONX16())
#endif

// 16-way 32-bit zero
#ifndef IVP_ZERON_2X32U
#   define IVP_ZERON_2X32U() IVP_MOVN_2X32U_FROMNX16(IVP_ZERONX16())
#endif

// 64-way 24-bit zero
#ifndef IVP_ZERO2NX24
#   define IVP_ZERO2NX24() IVP_MOV2NX24_FROMNX48(IVP_ZERONX48())
#endif

// 32-way 48-bit zero
#ifndef IVP_ZERONX48
#   if XCHAL_HAVE_VISION
#       define IVP_ZERONX48() (IVP_CVT48UNX32L(IVP_ZERON_2X32U()))
#   else
#       define IVP_ZERONX48() (IVP_MOVWVL(IVP_ZERONX16()))
#   endif
#endif

// prohibit usage of scalar-in-vector types in XI library
typedef xb_int16 _xi_intrin_private_xb_int16;
typedef xb_int8U _xi_intrin_private_xb_int8U;
#ifdef __XCC__
#undef vbool1
#undef xb_int8
#undef xb_int8U
#undef xb_int16
//#undef xb_int16U
#undef xb_int24
#undef xb_int32v
#undef xb_int32Uv
#undef xb_int48
#undef vselN
#define vbool1  vbool1_should_not_be_used_by_XI_Library
#define xb_int8  xb_int8_should_not_be_used_by_XI_Library
#define xb_int8U  xb_int8U_should_not_be_used_by_XI_Library
#define xb_int16  xb_int16_should_not_be_used_by_XI_Library
//#define xb_int16U  xb_int16U_should_not_be_used_by_XI_Library
#define xb_int24  xb_int24_should_not_be_used_by_XI_Library
#define xb_int32v  xb_int32v_should_not_be_used_by_XI_Library
#define xb_int32Uv  xb_int32Uv_should_not_be_used_by_XI_Library
#define xb_int48  xb_int48_should_not_be_used_by_XI_Library
#define vselN  vselN_should_not_be_used_by_XI_Library
#endif


////////// compatibility between IVPEP - VP5
#if XCHAL_HAVE_VISION

typedef xb_vecNx16 vsaN;

#   define IVP_MOVWVL(a) IVP_CVT48UNX32L(a)
#   define IVP_MOVV2WHH(a) IVP_MOVNX16_FROMN_2X32(IVP_CVT32S2NX24HH(IVP_MOV2NX24_FROMNX48(a)))
#   define IVP_MOVV2WHL(a) IVP_MOVNX16_FROMN_2X32(IVP_CVT32S2NX24HL(IVP_MOV2NX24_FROMNX48(a)))
#   define IVP_MOVV2WLH(a) IVP_MOVNX16_FROMN_2X32(IVP_CVT32S2NX24LH(IVP_MOV2NX24_FROMNX48(a)))
#   define IVP_MOVV2WLL(a) IVP_MOVNX16_FROMN_2X32(IVP_CVT32S2NX24LL(IVP_MOV2NX24_FROMNX48(a)))
#   define IVP_MOVSVWH(a) IVP_MOVNX16_FROMN_2X32(IVP_CVT32SNX48H(a))
#   define IVP_MOVSVWL(a) IVP_MOVNX16_FROMN_2X32(IVP_CVT32SNX48L(a))
#   define IVP_MOVVWHH(a) IVP_MOVNX16_FROM2NX8(IVP_CVT64SNX48HH(a))
#   define IVP_MOVVWHL(a) IVP_MOVNX16_FROM2NX8(IVP_CVT64SNX48HL(a))
#   define IVP_MOVVWLH(a) IVP_MOVNX16_FROM2NX8(IVP_CVT64SNX48LH(a))
#   define IVP_MOVVWLL(a) IVP_MOVNX16_FROM2NX8(IVP_CVT64SNX48LL(a))
#   define IVP_MOVV2WL(a) IVP_CVT16U2NX24L(IVP_MOV2NX24_FROMNX48(a))
#   define IVP_MOVV2WH(a) IVP_CVT16U2NX24H(IVP_MOV2NX24_FROMNX48(a))
#   define IVP_MOVVWL(a) IVP_MOVNX16_FROMN_2X32(IVP_CVT32UNX48L(a))
#   define IVP_MOVVWH(a) IVP_MOVNX16_FROMN_2X32(IVP_CVT32UNX48H(a))
#   define IVP_MOVSV2WL(a) IVP_CVT16S2NX24L(IVP_MOV2NX24_FROMNX48(a))
#   define IVP_MOVSV2WH(a) IVP_CVT16S2NX24H(IVP_MOV2NX24_FROMNX48(a))
#   define IVP_MOV2W2VL(a,b) IVP_MOVNX48_FROM2NX24(IVP_CVT24UNX32L(IVP_MOVN_2X32_FROMNX16(a),IVP_MOVN_2X32_FROMNX16(b)))
#   define IVP_MOVSWV(a,b) IVP_CVT48SNX32(IVP_MOVN_2X32_FROMNX16(a),IVP_MOVN_2X32_FROMNX16(b))
#   define IVP_MOVS2WV(a,b) IVP_MOVNX48_FROM2NX24(IVP_CVT24S2NX16(a,b))
#   define IVP_MOVWV(a,b) IVP_CVT48UNX32(IVP_MOVN_2X32_FROMNX16(a),IVP_MOVN_2X32_FROMNX16(b))

#   define IVP_MOVVVS(a) (a)
#   define IVP_MOVVSA32(a) IVP_MOVVA16(a)
#   define IVP_MOVVSV(vr,sa) (vr) // sa is always zero in XI, if not zero -> use IVP_MOVVSELNX16
#   define IVP_MOVVSELNX16(vr,sa) IVP_SRLINX16(vr, sa)
#   define IVP_MOVVSVADDNX16(a,b,c,d) { a = c; c = IVP_ADDNX16(c,b); } // d is always zero in XI
#   define IVP_MOVPVSV(a,b,c,d) { xb_vec2Nx8 t = IVP_SRLI2NX8(c,d); a = IVP_UNPKS2NX8_1(t); b = IVP_UNPKS2NX8_0(t); }

#undef IVP_LSNX8U_XP
#undef IVP_LSNX8U_IP
#undef IVP_LSNX8U_X
#undef IVP_LSNX8U_I
#   define IVP_LSNX8U_XP(a,b,c) do { _xi_intrin_private_xb_int8U tmp; IVP_LS2NX8U_XP(tmp,b,c); a = IVP_MOVNX16_FROM8U(tmp); } while (0)
#   define IVP_LSNX8U_IP(a,b,c) do { _xi_intrin_private_xb_int8U tmp; IVP_LS2NX8U_IP(tmp,b,c); a = IVP_MOVNX16_FROM8U(tmp); } while (0)
#   define IVP_LSNX8U_X(b,c) IVP_MOVNX16_FROM8U(IVP_LS2NX8U_X(b,c))
#   define IVP_LSNX8U_I(b,c) IVP_MOVNX16_FROM8U(IVP_LS2NX8U_I(b,c))

#   define IVP_PACKLNX48_L(a) IVP_CVT32UNX48L(a)
#   define IVP_PACKLNX48_H(a) IVP_CVT32UNX48H(a)

#   define IVP_SA2NX8UPOS_FP IVP_SAPOS2NX8U_FP
#   define IVP_SAN_2X32POS_FP IVP_SAPOSN_2X32_FP
#   define IVP_SANX16POS_FP IVP_SAPOSNX16_FP
#   define IVP_SANX16UPOS_FP IVP_SAPOSNX16U_FP
#   define IVP_SANX8UPOS_FP IVP_SAPOSNX8U_FP
#   define IVP_SAV2NX8POS_FP IVP_SAPOS2NX8_FP
#   define IVP_SAV2NX8UPOS_FP IVP_SAPOS2NX8U_FP
#   define IVP_SAVN_2X32POS_FP IVP_SAPOSN_2X32_FP
#   define IVP_SAVNX16POS_FP IVP_SAPOSNX16_FP
#   define IVP_SAVNX16UPOS_FP IVP_SAPOSNX16U_FP
#   define IVP_SAVNX8UPOS_FP IVP_SAPOSNX8U_FP
#   define IVP_LAVNX8U_PP IVP_LANX8U_PP
#   define IVP_LAVNX16_PP IVP_LANX16_PP

#   define IVP_RADDURNX16(b) ((int)IVP_RADDUNX16(b))
#   define IVP_RADDRNX16(b)  ((int)IVP_RADDNX16(b))
#   define IVP_ADDSNX16F(a,b,c,d) IVP_ADDSNX16T(a,b,c, IVP_NOTBN(d))
#   define IVP_ADDNX16F(a,b,c,d) IVP_ADDNX16T(a,b,c,IVP_NOTBN(d))
#   define IVP_SUBNX16F(a,b,c,d) IVP_SUBNX16T(a,b,c,IVP_NOTBN(d))
#   define IVP_NEGNX16F(a,b,c) IVP_NEGNX16T(a,b,IVP_NOTBN(c))
#   define IVP_NEGSNX16F(a,b,c) IVP_NEGSNX16T(a,b,IVP_NOTBN(c))
#   define IVP_RMINNX16F(b,c) IVP_RMINNX16T(b,IVP_NOTBN(c))
#   define IVP_MINUNX16F(a,b,c,d) IVP_MINUNX16T(a,b,c,IVP_NOTBN(d))
#   define IVP_SVNX8UF_XP(a,b,c,d) IVP_SVNX8UT_XP(a,b,c,IVP_NOTBN(d))
#   define IVP_SVNX8UF_I(a,b,c,d) IVP_SVNX8UT_I(a,b,c,IVP_NOTBN(d))
#   define IVP_SVNX16F_XP(a,b,c,d) IVP_SVNX16T_XP(a,b,c,IVP_NOTBN(d))
#   define IVP_SVNX16F_I(a,b,c,d) IVP_SVNX16T_I(a,b,c,IVP_NOTBN(d))

#endif

#if XCHAL_HAVE_VISION
#   define IVP__LSNX16_XP(a,b,c) do { _xi_intrin_private_xb_int16 tmp; IVP_LSNX16_XP(tmp, b, c); a = IVP_MOVNX16_FROM16(tmp); } while(0)
#else
#   define IVP__LSNX16_XP IVP_LSNX16_XP
#endif

#if XCHAL_HAVE_VISION
#   define IVP__LSNX16_IP(a,b,c) do { _xi_intrin_private_xb_int16 tmp; IVP_LSNX16_IP(tmp, b, c); a = IVP_MOVNX16_FROM16(tmp); } while(0)
#else
#   define IVP__LSNX16_IP IVP_LSNX16_IP
#endif

#if XCHAL_HAVE_VISION
#   define IVP__DSELNX16_2X16(a,b,c,d,e,f) { \
        xb_vecNx16 _v0, _v1 ; \
        _v0 = d ; \
        _v1 = c ; \
        a = IVP_SELNX16(_v1,_v0,e); \
        b = IVP_SELNX16(_v1,_v0,f); \
    }
#else
#   define IVP__DSELNX16_2X16 IVP_DSELNX16
#endif

#if XCHAL_HAVE_VISION
#   define IVP__SEL2NX8_2X16(b,c,d,e) IVP_SEL2NX8(b,c, IVP_SEL2NX8I(IVP_MOV2NX8_FROMNX16(d),IVP_MOV2NX8_FROMNX16(e),IVP_SELI_8B_INTERLEAVE_1_EVEN))
#else
#   define IVP__SEL2NX8_2X16 IVP_SEL2NX8
#endif

////////// compatibility for RF-2014.0 IVP-EP cores

#ifndef IVP_SVN_2X32_IP
#define IVP_SVN_2X32_IP(a,b,c)                              \
    do {                                                    \
        xb_vecNx16 *bb = (xb_vecNx16 *)b;                   \
        IVP_SVNX16_IP(IVP_MOVNX16_FROMN_2X32(a), bb, c);    \
        b = (xb_vecN_2x32v *)bb;                            \
    }while(0)
#endif

#ifndef IVP_SVN_2X32_XP
#define IVP_SVN_2X32_XP(a,b,c)                              \
    do {                                                    \
        xb_vecNx16 *bb = (xb_vecNx16 *)b;                   \
        IVP_SVNX16_XP(IVP_MOVNX16_FROMN_2X32(a), bb, c);    \
        b = (xb_vecN_2x32v *)bb;                            \
    }while(0)
#endif

#ifndef IVP_LVN_2X32_IP
#define IVP_LVN_2X32_IP(a,b,c)                              \
    do {                                                    \
        xb_vecNx16 *bb = (xb_vecNx16 *)b;                   \
        xb_vecNx16 aa; IVP_LVNX16_IP(aa, bb, c);            \
        a = IVP_MOVN_2X32_FROMNX16(aa);                     \
        b = (xb_vecN_2x32v *)bb;                            \
    }while(0)
#endif

#ifndef IVP_LVN_2X32_XP
#define IVP_LVN_2X32_XP(a,b,c)                              \
    do {                                                    \
        xb_vecNx16 *bb = (xb_vecNx16 *)b;                   \
        xb_vecNx16 aa; IVP_LVNX16_XP(aa, bb, c);            \
        a = IVP_MOVN_2X32_FROMNX16(aa);                     \
        b = (xb_vecN_2x32v *)bb;                            \
    }while(0)
#endif

////////// select/shuffle indexes
#if XCHAL_HAVE_VISION
#define XI_DSEL_16B_ROTATE_LEFT(n)  IVP_AVGU2NX8(IVP_SEQ2NX8(), IVP_MOV2NX8_FROMNX16((0x4000 - 2*(((n)<<8)+(n)))))
#define XI_DSEL_16B_ROTATE_RIGHT(n) IVP_AVGU2NX8(IVP_SEQ2NX8(), IVP_MOV2NX8_FROMNX16((0x3F00 + 2*(((n)<<8)+(n)))))

#define XI_DSEL_16B_ROTATE_RIGHT_2_1 IVP_AVGU2NX8(IVP_SEQ2NX8(), IVP_MOV2NX8_FROMNX16(2 * (1 + ( (1+1) << 8 ) )))
#define XI_DSEL_16B_ROTATE_RIGHT_4_3 IVP_AVGU2NX8(IVP_SEQ2NX8(), IVP_MOV2NX8_FROMNX16(2 * (3 + ( (3+1) << 8 ) )))
#define XI_DSEL_32B_ROTATE_RIGHT_2_1 IVP_AVGU2NX8(IVP_SEQ2NX8(), IVP_MOV2NX8_FROMNX16(4 * (1 + ( (1+1) << 8 ) )))
#define XI_DSEL_32B_ROTATE_RIGHT_4_3 IVP_AVGU2NX8(IVP_SEQ2NX8(), IVP_MOV2NX8_FROMNX16(4 * (3 + ( (3+1) << 8 ) )))
#endif

////////// address translation wrappers
#include "xi_pointer_translation.h"

#endif
