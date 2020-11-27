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
#ifndef __XI_CNN_H__
#define __XI_CNN_H__
#if defined(__XTENSA__) || defined(__GNUC__)
#else
#define ALIGN(x)  _declspec(align(x))
#define ALIGN16  _declspec(align(16))
#define ALIGN32  _declspec(align(32))
#define ALIGN64  _declspec(align(64))
#define __restrict
#endif

#include "xi_cnn_api.h"
#include "xi_tile3d_manager.h"
#include "xi_core.h"
#include "limits.h"

#if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))

/****************************************************************************/
/* MACROS :                                                                 */
/****************************************************************************/
/****************************************************************************/

/* Macro for aligning to any value */
#ifndef ALIGN
#define ALIGN(val, align)  XI_ALIGN_VAL(val, align)
#endif
/****************************************************************************/
/* Macro for initializing 48bit the accumulator with bias values            */
/****************************************************************************/
#define ACC48_INIT_BIAS(phvecBias1, accSum0, accSum1, \
                        accSum2, accSum3, remCh1, remCh2)  {      \
    valign vaBias;                                                \
    xb_vecN_2x32v hvecBias1, hvecBias2;                           \
    vaBias = IVP_LAN_2X32_PP(phvecBias1);                         \
    IVP_LAVN_2X32_XP(hvecBias1, vaBias, phvecBias1, remCh1);      \
    IVP_LAVN_2X32_XP(hvecBias2, vaBias, phvecBias1, remCh2);      \
    accSum0 = IVP_CVT48SNX32(hvecBias2, hvecBias1);               \
    accSum1 = IVP_CVT48SNX32(hvecBias2, hvecBias1);               \
    accSum2 = IVP_CVT48SNX32(hvecBias2, hvecBias1);               \
    accSum3 = IVP_CVT48SNX32(hvecBias2, hvecBias1);               \
}

#define IVP_PACKVRNX48_MORPH_ASYMML(vecOut, hvecIn, shift) \
  vecOut = IVP_PACKVRNX48(IVP_CVT48SNX32L(hvecIn), shift);

#define IVP_PACKVRNX48_MORPH_ASYMM(vecOut, hvecIn_H, hvecIn_L, shift) \
  vecOut = IVP_PACKVRNX48(IVP_CVT48SNX32(hvecIn_H, hvecIn_L), shift);

#define IVP_PACKVRNX48_MORPH_SYMML(vecOut, hvecIn, shift)             { \
    vboolN_2 vbA = IVP_LTN_2X32(hvecIn, 0);                             \
    hvecIn = IVP_ABSN_2X32(hvecIn);                                     \
    vecOut = IVP_PACKVRNX48(IVP_CVT48SNX32L(hvecIn), shift);            \
    vboolN vbAA = IVP_JOINBN_2(vbA, vbA);                               \
    IVP_SUBNX16T(vecOut, 0, vecOut, vbAA);                              \
}

#define IVP_PACKVRNX48_MORPH_SYMM(vecOut, hvecIn_H, hvecIn_L, shift)  {   \
    vboolN_2 vbA = IVP_LTN_2X32(hvecIn_H, 0);                             \
    hvecIn_H = IVP_ABSN_2X32(hvecIn_H);                                   \
    vboolN_2 vbB = IVP_LTN_2X32(hvecIn_L, 0);                             \
    hvecIn_L = IVP_ABSN_2X32(hvecIn_L);                                   \
    vecOut   = IVP_PACKVRNX48(IVP_CVT48SNX32(hvecIn_H, hvecIn_L), shift); \
    vboolN vbAB = IVP_JOINBN_2(vbA, vbB);                                 \
    IVP_SUBNX16T(vecOut, 0, vecOut, vbAB);                                \
}

#ifdef IVP_PACKVNX48
#define IVP_PACKVRNX48_MORPH_SYMM_VQ(vecOut, hvecIn_H, hvecIn_L, vecshift)  {   \
    vboolN_2 vbA = IVP_LTN_2X32(hvecIn_H, 0);                                   \
    hvecIn_H = IVP_ABSN_2X32(hvecIn_H);                                         \
    vboolN_2 vbB = IVP_LTN_2X32(hvecIn_L, 0);                                   \
    hvecIn_L = IVP_ABSN_2X32(hvecIn_L);                                         \
    vecOut   = IVP_PACKVNX48(IVP_CVT48SNX32(hvecIn_H, hvecIn_L), vecshift);     \
    vboolN vbAB = IVP_JOINBN_2(vbA, vbB);                                       \
    IVP_SUBNX16T(vecOut, 0, vecOut, vbAB);                                      \
}

#define IVP_PACKVRNX48_MORPH_ASYMM_VQ(vecOut, hvecIn_H, hvecIn_L, vecshift) \
  vecOut = IVP_PACKVNX48(IVP_CVT48SNX32(hvecIn_H, hvecIn_L), vecshift);

#else
#define IVP_PACKVRNX48_MORPH_SYMM_VQ(vecOut, hvecIn_H, hvecIn_L, vecshift)  {         \
    xb_vecN_2x32v hvecInAbs_H = IVP_ABSN_2X32(hvecIn_H);                              \
    xb_vecN_2x32v hvecInAbs_L = IVP_ABSN_2X32(hvecIn_L);                              \
    xb_vecN_2x32v hvecShiftL = IVP_MOVN_2X32_FROMNX16(IVP_SELNX16I((xb_vecNx16)0,     \
                                        vecshift, IVP_SELI_16B_INTERLEAVE_1_LO));     \
    xb_vecN_2x32v hvecShiftH = IVP_MOVN_2X32_FROMNX16(IVP_SELNX16I((xb_vecNx16)0,     \
                                        vecshift, IVP_SELI_16B_INTERLEAVE_1_HI));     \
    hvecInAbs_L = IVP_ADDN_2X32(hvecInAbs_L,IVP_SLAN_2X32((xb_vecN_2x32v)1,           \
                                        IVP_SUBN_2X32(hvecShiftL,(xb_vecN_2x32v)1))); \
    hvecInAbs_H = IVP_ADDN_2X32(hvecInAbs_H,IVP_SLAN_2X32((xb_vecN_2x32v)1,           \
                                        IVP_SUBN_2X32(hvecShiftH,(xb_vecN_2x32v)1))); \
    hvecInAbs_L = IVP_SRSN_2X32(hvecInAbs_L,hvecShiftL);                              \
    hvecInAbs_H = IVP_SRSN_2X32(hvecInAbs_H, hvecShiftH);                             \
    hvecIn_L = IVP_MULSGNN_2X32(hvecIn_L ,hvecInAbs_L);                               \
    hvecIn_H = IVP_MULSGNN_2X32(hvecIn_H, hvecInAbs_H);                               \
    vecOut   = IVP_PACKVRNX48(IVP_CVT48SNX32(hvecIn_H, hvecIn_L), 0);                 \
  }
#define IVP_PACKVRNX48_MORPH_ASYMM_VQ(vecOut, hvecIn_H, hvecIn_L, vecshift)       \
{                                                                                 \
  xb_vecN_2x32v hvecShiftL,hvecShiftH;                                            \
  xb_vecNx16  vecShiftH, vecShiftL; \
  IVP_DSELNX16I(vecShiftH, vecShiftL, 0, vecshift, IVP_DSELI_INTERLEAVE_1);       \
  hvecShiftL = IVP_MOVN_2X32_FROMNX16(vecShiftL);                                 \
  hvecShiftH = IVP_MOVN_2X32_FROMNX16(vecShiftH);                                 \
  hvecIn_L = IVP_ADDN_2X32(hvecIn_L, IVP_SLAN_2X32((xb_vecN_2x32v)1,              \
             IVP_SUBN_2X32(hvecShiftL, (xb_vecN_2x32v)1)));                       \
  hvecIn_H = IVP_ADDN_2X32(hvecIn_H, IVP_SLAN_2X32((xb_vecN_2x32v)1,              \
             IVP_SUBN_2X32(hvecShiftH, (xb_vecN_2x32v)1)));                       \
  hvecIn_L = IVP_SRSN_2X32(hvecIn_L, hvecShiftL);                                 \
  hvecIn_H = IVP_SRSN_2X32(hvecIn_H, hvecShiftH);                                 \
  vecOut = IVP_PACKVRNX48(IVP_CVT48SNX32(hvecIn_H, hvecIn_L), 0);                 \
}
#endif
#ifdef ENABLE_CONV_ASYMMETRIC_ROUNDING
#define MORPH_IVP_PACKVRNX48L  IVP_PACKVRNX48_MORPH_ASYMML
#define MORPH_IVP_PACKVRNX48   IVP_PACKVRNX48_MORPH_ASYMM

#define MORPH_IVP_PACKVRNX48_VQ IVP_PACKVRNX48_MORPH_ASYMM_VQ
#else
#define MORPH_IVP_PACKVRNX48L  IVP_PACKVRNX48_MORPH_SYMML
#define MORPH_IVP_PACKVRNX48   IVP_PACKVRNX48_MORPH_SYMM

#define MORPH_IVP_PACKVRNX48_VQ IVP_PACKVRNX48_MORPH_SYMM_VQ
#endif

#define MORPH_IVP_MUL16X32_QSCALE(dProd1, hvecOutH1W1A, quantScale)  {                   \
    dProd1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecOutH1W1A),                  \
                                  quantScale);                                           \
    IVP_MULAHN_2X16X32_1(dProd1, IVP_MOVNX16_FROMN_2X32(hvecOutH1W1A), quantScale);      \
}                                                                                        \

#define MORPH_IVP_MUL32X32_QSCALE(dProd1, hvecOutH1W1A, quantScale)  {       \
    dProd1 = IVP_MULN_2X32(hvecOutH1W1A, quantScale);                        \
}                                                                            \

#if defined(IVP_MULN_2X32)
#define MORPH_IVP_MUL32_QUANTSCALE MORPH_IVP_MUL32X32_QSCALE
#else
#define MORPH_IVP_MUL32_QUANTSCALE MORPH_IVP_MUL16X32_QSCALE
#endif

/****************************************************************************/
/* Macro for adding zero point with accumulated o/p, Packing the accumulator*/
/* output after convolution, scaling it,                                    */
/* shifting and clamping the final output between min and max limits        */
/****************************************************************************/
#define ADD48_ZPT_PACK_SCALE_SHIFT_CLAMP_LIMITS(vec16H1W1AB, accSum1,                                     \
                                                zeroPtOutputU, leftShift,rightShift, quantScale, maxLimU, minLimU)  { \
    /* The accumulated sum which is now in 48 bits wvec registers is moved to 32 bit  */                  \
    /*  vec registers to handle the addition of bias and fixup values to it. */                           \
    /* H -height W-width */                                                                               \
    /* Moving accumlated sum H1-W1 to normal vec registers */                                             \
    xb_vecN_2x32v hvecOutH1W1A, hvecOutH1W1B;                                                             \
    xb_vecN_2x64w dProd1, dProd2;                                                                         \
    hvecOutH1W1A = IVP_CVT32SNX48L(accSum1);                                                              \
    hvecOutH1W1B = IVP_CVT32SNX48H(accSum1);                                                              \
    /* Accumulated value is multiplied with (inputScale*coeffScale) /outputScale;*/                       \
    /* zeroPtOutputU is added to the result to get quantized output value. */                             \
    /* (inputScale*coeffScale) /outputScale = fractional part * 2^(outputShift) */                        \
    /* quantized scale = fractional part in q31 format. */                                                \
    /* H1- W1 */                                                                                          \
    /* Handling the 32x32 bit multiplication of accumulated sum with quantScale */                        \
    MORPH_IVP_MUL32_QUANTSCALE(dProd1, hvecOutH1W1A, quantScale);                                         \
    MORPH_IVP_MUL32_QUANTSCALE(dProd2, hvecOutH1W1B, quantScale);                                         \
    hvecOutH1W1A = IVP_PACKVRN_2X64W(dProd1, 31 - leftShift);                                             \
    hvecOutH1W1B = IVP_PACKVRN_2X64W(dProd2, 31 - leftShift);                                             \
    /*Pack the result to 16 bits and  saturate to short min and max*/                                     \
    MORPH_IVP_PACKVRNX48(vec16H1W1AB, hvecOutH1W1B, hvecOutH1W1A, rightShift);                            \
    vec16H1W1AB = IVP_ADDSNX16(vec16H1W1AB, zeroPtOutputU);                                               \
    vec16H1W1AB = IVP_MAXNX16(IVP_MINNX16(vec16H1W1AB, (xb_vecNx16) maxLimU),                             \
                              (xb_vecNx16) minLimU);                                                      \
}
#ifdef IVP_PACKVN_2X64W
#define ADD48_ZPT_PACK_SCALE_SHIFT_CLAMP_LIMITS_VQ(vec16H1W1AB, accSum1,                                     \
                                                  zeroPtOutputU, vecleftShiftL,vecleftShiftH,                \
                                                  vecRightShift, vecScaleL,vecScaleH, maxLimU, minLimU)  {   \
    /* The accumulated sum which is now in 48 bits wvec registers is moved to 32 bit  */                     \
    /*  vec registers to handle the addition of bias and fixup values to it. */                              \
    /* H -height W-width */                                                                                  \
    /* Moving accumlated sum H1-W1 to normal vec registers */                                                \
    xb_vecN_2x32v hvecOutH1W1A, hvecOutH1W1B;                                                                \
    xb_vecN_2x64w dProd1, dProd2;                                                                            \
    hvecOutH1W1A = IVP_CVT32SNX48L(accSum1);                                                                 \
    hvecOutH1W1B = IVP_CVT32SNX48H(accSum1);                                                                 \
    /* Accumulated value is multiplied with (inputScale*coeffScale) /outputScale;*/                          \
    /* zeroPtOutputU is added to the result to get quantized output value. */                                \
    /* (inputScale*coeffScale) /outputScale = fractional part * 2^(outputShift) */                           \
    /* quantized scale = fractional part in q31 format. */                                                   \
    /* H1- W1 */                                                                                             \
    /* Handling the 32x32 bit multiplication of accumulated sum with quantScale */                           \
    MORPH_IVP_MUL32_QUANTSCALE(dProd1, hvecOutH1W1A, vecScaleL);                                             \
    MORPH_IVP_MUL32_QUANTSCALE(dProd2, hvecOutH1W1B, vecScaleH);                                             \
    hvecOutH1W1A = IVP_PACKVN_2X64W(dProd1,IVP_SUBN_2X32(31 , vecleftShiftL));                               \
    hvecOutH1W1B = IVP_PACKVN_2X64W(dProd2,IVP_SUBN_2X32(31 , vecleftShiftH));                               \
    /*Pack the result to 16 bits and  saturate to short min and max*/                                        \
    MORPH_IVP_PACKVRNX48_VQ(vec16H1W1AB, hvecOutH1W1B, hvecOutH1W1A, vecRightShift);                         \
    vec16H1W1AB = IVP_ADDSNX16(vec16H1W1AB, zeroPtOutputU);                                                  \
    vec16H1W1AB = IVP_MAXNX16(IVP_MINNX16(vec16H1W1AB, (xb_vecNx16) maxLimU),                                \
                              (xb_vecNx16) minLimU);                                                         \
}
#else

#define ADD48_ZPT_PACK_SCALE_SHIFT_CLAMP_LIMITS_VQ(vec16H1W1AB, accSum1,                                              \
                                                  zeroPtOutputU, vecleftShiftL,vecleftShiftH,                         \
                                                  vecRightShift, vecScaleL,vecScaleH, maxLimU, minLimU)  {            \
    /* The accumulated sum which is now in 48 bits wvec registers is moved to 32 bit  */                              \
    /*  vec registers to handle the addition of bias and fixup values to it. */                                       \
    /* H -height W-width */                                                                                           \
    /* Moving accumlated sum H1-W1 to normal vec registers */                                                         \
    xb_vecN_2x32v hvecOutH1W1A, hvecOutH1W1B;                                                                         \
    xb_vecN_2x64w dProd1, dProd2;                                                                                     \
    hvecOutH1W1A = IVP_CVT32SNX48L(accSum1);                                                                          \
    hvecOutH1W1B = IVP_CVT32SNX48H(accSum1);                                                                          \
    /* Accumulated value is multiplied with (inputScale*coeffScale) /outputScale;*/                                   \
    /* zeroPtOutputU is added to the result to get quantized output value. */                                         \
    /* (inputScale*coeffScale) /outputScale = fractional part * 2^(outputShift) */                                    \
    /* quantized scale = fractional part in q31 format. */                                                            \
    /* H1- W1 */                                                                                                      \
    /* Handling the 32x32 bit multiplication of accumulated sum with quantScale */                                    \
    MORPH_IVP_MUL32_QUANTSCALE(dProd1, hvecOutH1W1A, vecScaleL);                                                      \
    MORPH_IVP_MUL32_QUANTSCALE(dProd2, hvecOutH1W1B, vecScaleH);                                                      \
    xb_vecN_2x32v vecleftShiftLDif = IVP_SUBN_2X32(31 , vecleftShiftL);                                               \
    xb_vecN_2x32v vecleftShiftHDif = IVP_SUBN_2X32(31, vecleftShiftH);                                                \
    /* Calculating the rounding factor and adding */                                                                  \
    xb_vecN_2x32v hvecRoundValL = IVP_SLAN_2X32((xb_vecN_2x32v)1,IVP_SUBN_2X32(vecleftShiftLDif,(xb_vecN_2x32v) 1));  \
    xb_vecN_2x32v hvecRoundValH = IVP_SLAN_2X32((xb_vecN_2x32v)1,IVP_SUBN_2X32(vecleftShiftHDif,(xb_vecN_2x32v) 1));  \
    IVP_MULAN_2X16X32_0(dProd1,1,hvecRoundValL);                                                                      \
    IVP_MULAN_2X16X32_0(dProd2,1,hvecRoundValH);                                                                      \
    hvecOutH1W1A = IVP_PACKVRNRN_2X64W(dProd1, 31);                                                                   \
    hvecOutH1W1B = IVP_PACKVRNRN_2X64W(dProd2, 31);                                                                   \
    xb_vecN_2x32v hvecProd1L = IVP_PACKLN_2X64W(dProd1);                                                              \
    xb_vecN_2x32v hvecProd2L = IVP_PACKLN_2X64W(dProd2);                                                              \
    /*Obtaining values which gets lost while packing*/                                                                \
    hvecProd1L = IVP_ANDN_2X32(IVP_SRSN_2X32(hvecProd1L,vecleftShiftLDif),                                            \
                               IVP_SUBN_2X32(IVP_SLAN_2X32((xb_vecN_2x32v)1,vecleftShiftL),(xb_vecN_2x32v)1));        \
    hvecProd2L = IVP_ANDN_2X32(IVP_SRSN_2X32(hvecProd2L, vecleftShiftHDif),                                           \
                               IVP_SUBN_2X32(IVP_SLAN_2X32((xb_vecN_2x32v)1,vecleftShiftH),(xb_vecN_2x32v)1));        \
    hvecOutH1W1A = IVP_SLSN_2X32(hvecOutH1W1A, vecleftShiftL);                                                        \
    hvecOutH1W1B = IVP_SLSN_2X32(hvecOutH1W1B, vecleftShiftH);                                                        \
    /* Checking whether the values are saturated or not */                                                            \
    /* If saturated, the masked values are not added */                                                               \
    vboolN_2 bmask1 = IVP_ANDBN_2(IVP_NEQN_2X32(hvecOutH1W1A,                                                         \
                                 (xb_vecN_2x32v)INT_MAX), IVP_NEQN_2X32(hvecOutH1W1A, (xb_vecN_2x32v)INT_MIN));       \
    vboolN_2 bmask2 = IVP_ANDBN_2(IVP_NEQN_2X32(hvecOutH1W1B,                                                         \
                                 (xb_vecN_2x32v)INT_MAX), IVP_NEQN_2X32(hvecOutH1W1B, (xb_vecN_2x32v)INT_MIN));       \
    IVP_ADDN_2X32T(hvecOutH1W1A,hvecOutH1W1A,hvecProd1L,bmask1);                                                      \
    IVP_ADDN_2X32T(hvecOutH1W1B,hvecOutH1W1B,hvecProd2L,bmask2);                                                      \
    /*Pack the result to 16 bits and  saturate to short min and max*/                                                 \
    MORPH_IVP_PACKVRNX48_VQ(vec16H1W1AB, hvecOutH1W1B, hvecOutH1W1A, vecRightShift);                                  \
    vec16H1W1AB = IVP_ADDSNX16(vec16H1W1AB, zeroPtOutputU);                                                           \
    vec16H1W1AB = IVP_MAXNX16(IVP_MINNX16(vec16H1W1AB, (xb_vecNx16) maxLimU),                                         \
                              (xb_vecNx16) minLimU);                                                                  \
}
#endif

/****************************************************************************/
/* Macro for adding fix up sum across reOrdered input                       */
/****************************************************************************/

#define FIXUP_INV_QM24(fixUpSum, constScale, hvecOutA, hvecOutB, hvecOutC, hvecOutD)                                    \
{                                                                                                                       \
  xb_vecN_2x32Uv hvecA, hvecB, hvecC, hvecD;                                                                            \
  xb_vecN_2x64w dProd1, dProd2, dProd3, dProd4;                                                                         \
  hvecA = IVP_CVT32S2NX24LL(fixUpSum);                                                                                  \
  hvecB = IVP_CVT32S2NX24LH(fixUpSum);                                                                                  \
  hvecC = IVP_CVT32S2NX24HL(fixUpSum);                                                                                  \
  hvecD = IVP_CVT32S2NX24HH(fixUpSum);                                                                                  \
  dProd1 = IVP_MULSUN_2X16X32_0(vConstScale, hvecA);                                                                    \
  dProd2 = IVP_MULSUN_2X16X32_0(vConstScale, hvecB);                                                                    \
  dProd3 = IVP_MULSUN_2X16X32_0(vConstScale, hvecC);                                                                    \
  dProd4 = IVP_MULSUN_2X16X32_0(vConstScale, hvecD);                                                                    \
  hvecOutA = IVP_PACKVRN_2X64W(dProd1, 0);                                                                              \
  hvecOutB = IVP_PACKVRN_2X64W(dProd2, 0);                                                                              \
  hvecOutC = IVP_PACKVRN_2X64W(dProd3, 0);                                                                              \
  hvecOutD = IVP_PACKVRN_2X64W(dProd4, 0);                                                                              \
}

/****************************************************************************/
/* Macro for adding zero point with accumulated o/p, Packing the accumulator*/
/* output after convolution, scaling it,                                    */
/* shifting and clamping the final output between min and max limits        */
/****************************************************************************/
#define ADD_FIXUP_INV_QM24_ZPT_PACK_SCALE_SHIFT_CLAMP_LIMITS(dvecOut, daccSum, hvecBias, hvecFixUp1, hvecFixUp2,                       \
                                                 vecFixUp3, hvecFixUp4, zPtOut, leftShift,rightShift, quantScale, maxLimU, minLimU)    \
{                                                                                                                                      \
    /*The accumulated sum which is now in 24 bits wvec registers is moved to 32 bit                                                    \
        vec registers to handle the addition of bias and fixup values to it.*/                                                         \
    /*Moving accumlated sum to normal vec registers*/                                                                                  \
    xb_vecN_2x32v hvecOutA, hvecOutB, hvecOutC, hvecOutD;                                                                              \
    xb_vecN_2x64w dProd1, dProd2, dProd3, dProd4;                                                                                      \
    xb_vecNx16 vec16AB;                                                                                                                \
    xb_vecNx16 vec16CD;                                                                                                                \
    hvecOutA = IVP_CVT32S2NX24LL(daccSum);                                                                                             \
    hvecOutB = IVP_CVT32S2NX24LH(daccSum);                                                                                             \
    hvecOutC = IVP_CVT32S2NX24HL(daccSum);                                                                                             \
    hvecOutD = IVP_CVT32S2NX24HH(daccSum);                                                                                             \
    /*Adding bias to accumulated sum */                                                                                                \
    hvecOutA = IVP_ADDN_2X32(hvecOutA, hvecBias);                                                                                      \
    hvecOutB = IVP_ADDN_2X32(hvecOutB, hvecBias);                                                                                      \
    hvecOutC = IVP_ADDN_2X32(hvecOutC, hvecBias);                                                                                      \
    hvecOutD = IVP_ADDN_2X32(hvecOutD, hvecBias);                                                                                      \
    /*Adding fixup to accumulated sum */                                                                                               \
    hvecOutA = IVP_ADDN_2X32(hvecOutA, hvecFixUp1);                                                                                    \
    hvecOutB = IVP_ADDN_2X32(hvecOutB, hvecFixUp2);                                                                                    \
    hvecOutC = IVP_ADDN_2X32(hvecOutC, hvecFixUp3);                                                                                    \
    hvecOutD = IVP_ADDN_2X32(hvecOutD, hvecFixUp4);                                                                                    \
    /* Accumulated value is multiplied with inputScale*coeffScale) /outputScale and                                                    \
      * zeroPtOutputU is added to the result to get quantized output value.                                                            \
      * (inputScale*coeffScale) /outputScale = fractional part * 2^(outputShift)                                                       \
      * quantized scale = fractional part in q31 format. */                                                                            \
    /*Handling the 32x32 bit multiplication of accumulated sum with quantScale*/                                                       \
    dProd1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecOutA), quantScale);                                                       \
    IVP_MULAHN_2X16X32_1(dProd1, IVP_MOVNX16_FROMN_2X32(hvecOutA), quantScale);                                                        \
    dProd2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecOutB), quantScale);                                                       \
    IVP_MULAHN_2X16X32_1(dProd2, IVP_MOVNX16_FROMN_2X32(hvecOutB), quantScale);                                                        \
    dProd3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecOutC), quantScale);                                                       \
    IVP_MULAHN_2X16X32_1(dProd3, IVP_MOVNX16_FROMN_2X32(hvecOutC), quantScale);                                                        \
    dProd4 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecOutD), quantScale);                                                       \
    IVP_MULAHN_2X16X32_1(dProd4, IVP_MOVNX16_FROMN_2X32(hvecOutD), quantScale);                                                        \
    hvecOutA = IVP_PACKVRN_2X64W(dProd1, 31 - leftShift);                                                                                          \
    hvecOutB = IVP_PACKVRN_2X64W(dProd2, 31 - leftShift);                                                                                          \
    hvecOutC = IVP_PACKVRN_2X64W(dProd3, 31 - leftShift);                                                                                          \
    hvecOutD = IVP_PACKVRN_2X64W(dProd4, 31 - leftShift);                                                                                          \
    /*Pack the result to 16 bits and  saturate to short min and max*/                                                                  \
    MORPH_IVP_PACKVRNX48(vec16AB, hvecOutB, hvecOutA, rightShift);                                                                       \
    MORPH_IVP_PACKVRNX48(vec16CD, hvecOutD, hvecOutC, rightShift);                                                                       \
    vec16AB = IVP_ADDSNX16(vec16AB, zPtOut);                                                                                           \
    vec16CD = IVP_ADDSNX16(vec16CD, zPtOut);                                                                                           \
    vec16AB = IVP_MAXNX16(IVP_MINNX16(vec16AB, (xb_vecNx16) maxLimU), (xb_vecNx16) minLimU);                                           \
    vec16CD = IVP_MAXNX16(IVP_MINNX16(vec16CD, (xb_vecNx16) maxLimU), (xb_vecNx16) minLimU);                                           \
    dvecOut = IVP_SEL2NX8I(IVP_MOV2NX8_FROMNX16(vec16CD), IVP_MOV2NX8_FROMNX16(vec16AB), IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0);            \
}

/****************************************************************************/
/* Macro for adding bias, fixup and zero point , Packing the accumulator    */
/* output after convolution, scaling it,                                    */
/* shifting and clamping the final output between min and max limits        */
/****************************************************************************/

#define ADD_FIXUP_ZPT_PACK_SCALE_SHIFT_CLAMP_LIMITS(dvec8H1W1ABCD, daccSum1, hvecBias1, hvecBias2,                                              \
                                                    hvecBias3, hvecBias4, dvecFixUp1, zeroPtOutputU, leftShift,rightShift, quantScale, maxLimU, minLimU)  { \
    /*The accumulated sum which is now in 24 bits wvec registers is moved to 32 bit \
       vec registers to handle the addition of bias and fixup values to it.*/ \
    /*H -height W-width*/                                                     \
    /*Moving accumlated sum H1-W1 to normal vec registers*/                   \
    xb_vecN_2x32v hvecOutH1W1A, hvecOutH1W1B, hvecOutH1W1C, hvecOutH1W1D;     \
    xb_vecN_2x64w dProd1, dProd2, dProd3, dProd4;                             \
    xb_vecNx16 vec16H1W1AB;                                                   \
    xb_vecNx16 vec16H1W1CD;                                                   \
    hvecOutH1W1A = IVP_CVT32S2NX24LL(daccSum1);                               \
    hvecOutH1W1B = IVP_CVT32S2NX24LH(daccSum1);                               \
    hvecOutH1W1C = IVP_CVT32S2NX24HL(daccSum1);                               \
    hvecOutH1W1D = IVP_CVT32S2NX24HH(daccSum1);                               \
    /*Adding bias to accumulated sum H1-W1*/                                  \
    hvecOutH1W1A = IVP_ADDN_2X32(hvecOutH1W1A, hvecBias1);                    \
    hvecOutH1W1B = IVP_ADDN_2X32(hvecOutH1W1B, hvecBias2);                    \
    hvecOutH1W1C = IVP_ADDN_2X32(hvecOutH1W1C, hvecBias3);                    \
    hvecOutH1W1D = IVP_ADDN_2X32(hvecOutH1W1D, hvecBias4);                    \
    /*Adding fixup to accumulated sum H1-W1*/                                 \
    hvecOutH1W1A = IVP_ADDN_2X32(hvecOutH1W1A, dvecFixUp1);                   \
    hvecOutH1W1B = IVP_ADDN_2X32(hvecOutH1W1B, dvecFixUp1);                   \
    hvecOutH1W1C = IVP_ADDN_2X32(hvecOutH1W1C, dvecFixUp1);                   \
    hvecOutH1W1D = IVP_ADDN_2X32(hvecOutH1W1D, dvecFixUp1);                   \
    /* Accumulated value is multiplied with inputScale*coeffScale) /outputScale and \
     * zeroPtOutputU is added to the result to get quantized output value. \
     * (inputScale*coeffScale) /outputScale = fractional part * 2^(outputShift) \
     * quantized scale = fractional part in q31 format. */                                             \
    /*H1- W1*/                                                                                         \
    /*Handling the 32x32 bit multiplication of accumulated sum with quantScale*/                       \
    dProd1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecOutH1W1A),                                \
                                  quantScale);                                                         \
    IVP_MULAHN_2X16X32_1(dProd1, IVP_MOVNX16_FROMN_2X32(hvecOutH1W1A), quantScale);                    \
    dProd2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecOutH1W1B),                                \
                                  quantScale);                                                         \
    IVP_MULAHN_2X16X32_1(dProd2, IVP_MOVNX16_FROMN_2X32(hvecOutH1W1B), quantScale);                    \
    dProd3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecOutH1W1C),                                \
                                  quantScale);                                                         \
    IVP_MULAHN_2X16X32_1(dProd3, IVP_MOVNX16_FROMN_2X32(hvecOutH1W1C), quantScale);                    \
    dProd4 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecOutH1W1D),                                \
                                  quantScale);                                                         \
    IVP_MULAHN_2X16X32_1(dProd4, IVP_MOVNX16_FROMN_2X32(hvecOutH1W1D), quantScale);                    \
    hvecOutH1W1A = IVP_PACKVRN_2X64W(dProd1, 31 - leftShift);                                          \
    hvecOutH1W1B = IVP_PACKVRN_2X64W(dProd2, 31 - leftShift);                                          \
    hvecOutH1W1C = IVP_PACKVRN_2X64W(dProd3, 31 - leftShift);                                          \
    hvecOutH1W1D = IVP_PACKVRN_2X64W(dProd4, 31 - leftShift);                                          \
    /*Pack the result to 16 bits and  saturate to short min and max*/                                  \
    MORPH_IVP_PACKVRNX48(vec16H1W1AB, hvecOutH1W1B, hvecOutH1W1A, rightShift);                           \
    MORPH_IVP_PACKVRNX48(vec16H1W1CD, hvecOutH1W1D, hvecOutH1W1C, rightShift);                           \
    vec16H1W1AB = IVP_ADDSNX16(vec16H1W1AB, zeroPtOutputU);                                            \
    vec16H1W1CD = IVP_ADDSNX16(vec16H1W1CD, zeroPtOutputU);                                            \
    vec16H1W1AB = IVP_MAXNX16(IVP_MINNX16(vec16H1W1AB, (xb_vecNx16) maxLimU),                          \
                              (xb_vecNx16) minLimU);                                                   \
    vec16H1W1CD = IVP_MAXNX16(IVP_MINNX16(vec16H1W1CD, (xb_vecNx16) maxLimU),                          \
                              (xb_vecNx16) minLimU);                                                   \
    dvec8H1W1ABCD = IVP_SEL2NX8I(IVP_MOV2NX8_FROMNX16(vec16H1W1CD),                                    \
                                 IVP_MOV2NX8_FROMNX16(vec16H1W1AB), IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0); \
}

#define ADD_FIXUP_CONST_ZPT_PACK_SCALE_SHIFT_CLAMP_LIMITS(dvec8H1W1ABCD, daccSum1,dvecConstTerm1,     \
dvecConstTerm2, dvecConstTerm3, dvecConstTerm4, dvecFixUp1, zeroPtOutputU, leftShift, rightShift,     \
quantScale,maxLimU, minLimU)  {                                                                       \
    /*The accumulated sum which is now in 24 bits wvec registers is moved to 32 bit \
       vec registers to handle the addition of bias and fixup values to it.*/                         \
    /*H -height W-width*/                                                                             \
    /*Moving accumlated sum H1-W1 to normal vec registers*/                                           \
    xb_vecN_2x32v hvecOutH1W1A, hvecOutH1W1B, hvecOutH1W1C, hvecOutH1W1D;                             \
    xb_vecN_2x64w dProd1, dProd2, dProd3, dProd4;                                                     \
    xb_vecNx16 vec16H1W1AB;                                                                           \
    xb_vecNx16 vec16H1W1CD;                                                                           \
    hvecOutH1W1A = IVP_CVT32S2NX24LL(daccSum1);                                                       \
    hvecOutH1W1B = IVP_CVT32S2NX24LH(daccSum1);                                                       \
    hvecOutH1W1C = IVP_CVT32S2NX24HL(daccSum1);                                                       \
    hvecOutH1W1D = IVP_CVT32S2NX24HH(daccSum1);                                                       \
    /*Adding bias to accumulated sum H1-W1*/                                                          \
    hvecOutH1W1A = IVP_ADDN_2X32(hvecOutH1W1A, dvecConstTerm1);                                       \
    hvecOutH1W1B = IVP_ADDN_2X32(hvecOutH1W1B, dvecConstTerm2);                                       \
    hvecOutH1W1C = IVP_ADDN_2X32(hvecOutH1W1C, dvecConstTerm3);                                       \
    hvecOutH1W1D = IVP_ADDN_2X32(hvecOutH1W1D, dvecConstTerm4);                                       \
    /*Adding fixup to accumulated sum H1-W1*/                                                         \
    hvecOutH1W1A = IVP_ADDN_2X32(hvecOutH1W1A, dvecFixUp1);                                           \
    hvecOutH1W1B = IVP_ADDN_2X32(hvecOutH1W1B, dvecFixUp1);                                           \
    hvecOutH1W1C = IVP_ADDN_2X32(hvecOutH1W1C, dvecFixUp1);                                           \
    hvecOutH1W1D = IVP_ADDN_2X32(hvecOutH1W1D, dvecFixUp1);                                           \
    /* Accumulated value is multiplied with inputScale*coeffScale) /outputScale and \
     * zeroPtOutputU is added to the result to get quantized output value. \
     * (inputScale*coeffScale) /outputScale = fractional part * 2^(outputShift) \
     * quantized scale = fractional part in q31 format. */                                             \
    /*H1- W1*/                                                                                         \
    /*Handling the 32x32 bit multiplication of accumulated sum with quantScale*/                       \
    dProd1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecOutH1W1A),                                \
                                  quantScale);                                                         \
    IVP_MULAHN_2X16X32_1(dProd1, IVP_MOVNX16_FROMN_2X32(hvecOutH1W1A), quantScale);                    \
    dProd2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecOutH1W1B),                                \
                                  quantScale);                                                         \
    IVP_MULAHN_2X16X32_1(dProd2, IVP_MOVNX16_FROMN_2X32(hvecOutH1W1B), quantScale);                    \
    dProd3 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecOutH1W1C),                                \
                                  quantScale);                                                         \
    IVP_MULAHN_2X16X32_1(dProd3, IVP_MOVNX16_FROMN_2X32(hvecOutH1W1C), quantScale);                    \
    dProd4 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecOutH1W1D),                                \
                                  quantScale);                                                         \
    IVP_MULAHN_2X16X32_1(dProd4, IVP_MOVNX16_FROMN_2X32(hvecOutH1W1D), quantScale);                    \
    hvecOutH1W1A = IVP_PACKVRN_2X64W(dProd1, 31 - leftShift);                                          \
    hvecOutH1W1B = IVP_PACKVRN_2X64W(dProd2, 31 - leftShift);                                          \
    hvecOutH1W1C = IVP_PACKVRN_2X64W(dProd3, 31 - leftShift);                                          \
    hvecOutH1W1D = IVP_PACKVRN_2X64W(dProd4, 31 - leftShift);                                          \
    /*Pack the result to 16 bits and  saturate to short min and max*/                                  \
    MORPH_IVP_PACKVRNX48(vec16H1W1AB, hvecOutH1W1B, hvecOutH1W1A, rightShift);                         \
    MORPH_IVP_PACKVRNX48(vec16H1W1CD, hvecOutH1W1D, hvecOutH1W1C, rightShift);                         \
    vec16H1W1AB = IVP_ADDSNX16(vec16H1W1AB, zeroPtOutputU);                                            \
    vec16H1W1CD = IVP_ADDSNX16(vec16H1W1CD, zeroPtOutputU);                                            \
    vec16H1W1AB = IVP_MAXNX16(IVP_MINNX16(vec16H1W1AB, (xb_vecNx16) maxLimU),                          \
                            (xb_vecNx16) minLimU);                                                     \
    vec16H1W1CD = IVP_MAXNX16(IVP_MINNX16(vec16H1W1CD, (xb_vecNx16) maxLimU),                          \
                            (xb_vecNx16) minLimU);                                                     \
    dvec8H1W1ABCD = IVP_SEL2NX8I(IVP_MOV2NX8_FROMNX16(vec16H1W1CD),                                    \
                                 IVP_MOV2NX8_FROMNX16(vec16H1W1AB), IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0); \
}

/****************************************************************************/
/* Macro for Packing the accumulator output after convolution, scaling it,  */
/* shifting and clamping the final output between min and max limits        */
/****************************************************************************/

#define PACK_SCALE_SHIFT_CLAMP_LIMITS(dvecOut1, dvecOut2, daccSum, packSA, outSc, outSh, min, max, flag)               { \
    xb_vecNx16 m_outEven = IVP_PACKVR2NX24_0(daccSum, packSA);                                                           \
    xb_vecNx16 m_outOdd  = IVP_PACKVR2NX24_1(daccSum, packSA);                                                           \
    xb_vecNx48 m_wvec    = IVP_MULUSNX16((xb_vecNx16U) outSc, m_outEven);                                                \
    m_outEven = IVP_PACKVRNX48(m_wvec, outSh);                                                                           \
    m_outEven = IVP_MAXNX16(IVP_MINNX16(m_outEven, (xb_vecNx16) max), (xb_vecNx16) min);                                 \
    m_wvec    = IVP_MULUSNX16((xb_vecNx16U) outSc, m_outOdd);                                                            \
    m_outOdd  = IVP_PACKVRNX48(m_wvec, outSh);                                                                           \
    m_outOdd  = IVP_MAXNX16(IVP_MINNX16(m_outOdd, (xb_vecNx16) max), (xb_vecNx16) min);                                  \
    xb_vec2Nx8 m_dvec = IVP_SEL2NX8I(IVP_MOV2NX8_FROMNX16(m_outOdd),                                                     \
                                     IVP_MOV2NX8_FROMNX16(m_outEven),                                                    \
                                     IVP_SELI_8B_INTERLEAVE_1_EVEN);                                                     \
    dvecOut1 = IVP_SEL2NX8I(IVP_MOV2NX8_FROMNX16(m_outOdd),                                                              \
                            IVP_MOV2NX8_FROMNX16(m_outEven),                                                             \
                            IVP_SELI_16B_INTERLEAVE_1_LO);                                                               \
    dvecOut1 = IVP_MOV2NX8T(dvecOut1, m_dvec, IVP_EQ2NX8(flag, 1));                                                      \
    dvecOut2 = IVP_SEL2NX8I(IVP_MOV2NX8_FROMNX16(m_outOdd),                                                              \
                            IVP_MOV2NX8_FROMNX16(m_outEven),                                                             \
                            IVP_SELI_16B_INTERLEAVE_1_HI);                                                               \
}

#define SCALE_SHIFT_ADDZP_CLAMP_LIMITS_FC(vecOutData, hvecOut, quantScale, leftShift,rightShift, zeroPtOutputU, minLimU, maxLimU)  { \
    xb_vecN_2x64w haccScaled = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecOut), quantScale);                        \
    IVP_MULAHN_2X16X32_1(haccScaled, IVP_MOVNX16_FROMN_2X32(hvecOut), quantScale);                                       \
    xb_vecN_2x32v hvecShift31 = IVP_PACKVRN_2X64W(haccScaled, 31 - leftShift);                                           \
    xb_vecNx16 vecOutShifted;                                                                                            \
    MORPH_IVP_PACKVRNX48L(vecOutShifted, hvecShift31, rightShift);                                                       \
    xb_vecNx16 vecOutAddZP = IVP_ADDSNX16(vecOutShifted, (xb_vecNx16) zeroPtOutputU);                                    \
    vecOutData = IVP_MAXNX16(IVP_MINNX16(vecOutAddZP, (xb_vecNx16) maxLimU), (xb_vecNx16) minLimU);                      \
}

#define BIAS_FIXUP_SCALE_SHIFT_ADDZP_CLAMP_LIMITS_SO(vecOutData, hvecOut, hvecBias, hvecFixUp,                           \
                                                 quantScale, leftShift,rightShift, zeroPtOutputU, minLimU, maxLimU)    { \
    hvecOut = IVP_ADDN_2X32(hvecOut, hvecBias);                                                                          \
    hvecOut = IVP_ADDN_2X32(hvecOut, hvecFixUp);                                                                         \
    xb_vecN_2x64w haccScaled = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecOut), quantScale);                        \
    IVP_MULAHN_2X16X32_1(haccScaled, IVP_MOVNX16_FROMN_2X32(hvecOut), quantScale);                                       \
    xb_vecN_2x32v hvecShift31 = IVP_PACKVRN_2X64W(haccScaled, 31 - leftShift);                                           \
    xb_vecNx16 vecOutShifted;                                                                                            \
    MORPH_IVP_PACKVRNX48L(vecOutShifted, hvecShift31, rightShift);                                                       \
    xb_vecNx16 vecOutAddZP = IVP_ADDSNX16(vecOutShifted, (xb_vecNx16) zeroPtOutputU);                                    \
    vecOutData = IVP_MAXNX16(IVP_MINNX16(vecOutAddZP, (xb_vecNx16) maxLimU), (xb_vecNx16) minLimU);                      \
}

#define BIAS_CONSTTERM_FIXUP_SCALE_SHIFT_ADDZP_CLAMP_LIMITS_SO(vecOutData, hvecOut, hvecBias, hvecConstTerm,                     \
                                                               hvecFixUp, quantScale, leftShift, rightShift, zeroPtOutputU, minLimU, maxLimU) \
  {                                                                                                                              \
    hvecOut = IVP_ADDN_2X32(hvecOut, hvecBias);                                                                                  \
    hvecOut = IVP_ADDN_2X32(hvecOut, hvecConstTerm);                                                                             \
    hvecOut = IVP_ADDN_2X32(hvecOut, hvecFixUp);                                                                                 \
    xb_vecN_2x64w haccScaled = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecOut), quantScale);                                \
    IVP_MULAHN_2X16X32_1(haccScaled, IVP_MOVNX16_FROMN_2X32(hvecOut), quantScale);                                               \
    xb_vecN_2x32v hvecShift31 = IVP_PACKVRN_2X64W(haccScaled, 31 - leftShift);                                                   \
    vboolN_2 vbA              = IVP_LTN_2X32(hvecShift31, 0);                                                                    \
    hvecShift31 = IVP_ABSN_2X32(hvecShift31);                                                                                    \
    xb_vecNx16 vecOutShifted = IVP_PACKVRNX48(IVP_CVT48SNX32L(hvecShift31), rightShift);                                         \
    vboolN vbAA              = IVP_JOINBN_2(vbA, vbA);                                                                           \
    vecOutShifted = IVP_MOVNX16T(IVP_SUBNX16(0,vecOutShifted), vecOutShifted, vbAA);                                             \
    xb_vecNx16 vecOutAddZP = IVP_ADDSNX16(vecOutShifted, (xb_vecNx16) zeroPtOutputU);                                            \
    vecOutData = IVP_MAXNX16(IVP_MINNX16(vecOutAddZP, (xb_vecNx16) maxLimU), (xb_vecNx16) minLimU);                              \
  }

#define ROIALIGN_SCALE_SHIFT_ADDZP_CLAMP_LIMITS(hacc0, hacc1, dvecOut0, leftShift, mulScale, outShift, outZeroPt, min, max)   \
  xb_vecN_2x64w dProd2, dProd1;                                                                                               \
  xb_vecNx16 vec16AB;                                                                                                         \
  xb_vecN_2x32v hvec0 = IVP_PACKVRN_2X64W(hacc0, 31);                                                                         \
  xb_vecN_2x32v hvec1 = IVP_PACKVRN_2X64W(hacc1, 31);                                                                         \
  hvec0 = IVP_SLAN_2X32(hvec0, leftShift);                                                                                    \
  hvec1 = IVP_SLAN_2X32(hvec1, leftShift);                                                                                    \
  dProd1 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvec0), mulScale);                                                     \
  IVP_MULAHN_2X16X32_1(dProd1, IVP_MOVNX16_FROMN_2X32(hvec0), mulScale);                                                      \
  dProd2 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvec1), mulScale);                                                     \
  IVP_MULAHN_2X16X32_1(dProd2, IVP_MOVNX16_FROMN_2X32(hvec1), mulScale);                                                      \
  hvec0 = IVP_PACKVRN_2X64W(dProd1, 31);                                                                                      \
  hvec1 = IVP_PACKVRN_2X64W(dProd2, 31);                                                                                      \
  vec16AB = IVP_PACKVRNX48(IVP_CVT48SNX32(hvec1, hvec0), outShift);                                                           \
  vec16AB = IVP_ADDSNX16(vec16AB, outZeroPt);                                                                                 \
  xb_vecNx16 vecOut = IVP_MAXNX16(IVP_MINNX16(vec16AB, (xb_vecNx16) max), (xb_vecNx16) min);                                  \
  dvecOut = IVP_SEL2NX8I(IVP_MOV2NX8_FROMNX16(vecOut), IVP_MOV2NX8_FROMNX16(vecOut), IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0);       \

#define XI_CHECK_ROIALIGN_POOLING_PARAMS(param)                                                                                                         \
XI_CHECK_ERROR(((XI_CNN_ROI_ALIGN_GET_ONE_BY_POOLED_WIDTH_SCALE(param) <= UINT_MAX) && (XI_CNN_ROI_ALIGN_GET_ONE_BY_POOLED_HEIGHT_SCALE(param) <= UINT_MAX)), \
  XI_ERR_NORM, "oneByPooledWidth & oneByPooledHeight should be less than U32_MAX");                                                                     \
XI_CHECK_ERROR(((XI_CNN_ROI_POOLING_GET_STRIDE_SHIFTX(param) == 8) && (XI_CNN_ROI_POOLING_GET_STRIDE_SHIFTY(param) == 8)),              \
  XI_ERR_NORM, "strideShiftX & strideShiftY should be equal to 8");                                                         \
XI_CHECK_ERROR(((XI_CNN_ROI_ALIGN_GET_ONE_BY_POOLED_WIDTH_SHIFT(param) == 31) && (XI_CNN_ROI_ALIGN_GET_ONE_BY_POOLED_HEIGHT_SHIFT(param) == 31)),         \
  XI_ERR_NORM, "shiftPool should be equal to 31 (scalar shift value)");                                                                                \

#define XI_CHECK_TILE3D_EDGE(tile, edge)                                                                   \
  if (XI_TILE3D_GET_DATA_ORDER(tile) == XI_WHD)                                                            \
  {                                                                                                        \
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM1_EDGE1(tile) >= edge && XI_TILE3D_GET_DIM1_EDGE2(tile) >= edge &&     \
                   XI_TILE3D_GET_DIM2_EDGE1(tile) >= edge && XI_TILE3D_GET_DIM2_EDGE2(tile) >= edge,       \
                   XI_ERR_EDGE, "The (" #tile ") tile must have at least " #edge "-pixel edge extension"); \
  }                                                                                                        \
  else if (XI_TILE3D_GET_DATA_ORDER(tile) == XI_DWH)                                                       \
  {                                                                                                        \
    XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_EDGE1(tile) >= edge && XI_TILE3D_GET_DIM2_EDGE2(tile) >= edge &&     \
                   XI_TILE3D_GET_DIM3_EDGE1(tile) >= edge && XI_TILE3D_GET_DIM3_EDGE2(tile) >= edge,       \
                   XI_ERR_EDGE, "The (" #tile ") tile must have at least " #edge "-pixel edge extension"); \
  }                                                                                                        \
  else if (XI_TILE3D_GET_DATA_ORDER(tile) == XI_ID16WH)                                                    \
  {                                                                                                        \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(tile) >> 4) >= edge && (XI_TILE3D_GET_DIM1_EDGE2(tile) >> 4) >= edge &&   \
                   XI_TILE3D_GET_DIM3_EDGE1(tile) >= edge && XI_TILE3D_GET_DIM3_EDGE2(tile) >= edge,                   \
                   XI_ERR_EDGE, "The (" #tile ") tile must have at least " #edge "-pixel edge extension");             \
  }                                                                                                                    \

#define XI_CHECK_TILE3D_DATA_ORDER(tile, type) \
  XI_CHECK_ERROR(XI_TILE3D_GET_DATA_ORDER(tile) == type, XI_ERR_BADARG, "The Data Order of (" #tile ") is not supported by this function")

#define XI_CHECK_TILE4D_DATA_ORDER(tile, type) \
  XI_CHECK_ERROR(XI_TILE4D_GET_DATA_ORDER(tile) == type, XI_ERR_BADARG, "The Data Order of (" #tile ") is not supported by this function")

#define XI_CHECK_KERNEL_SIZE(coeffT, size)                                                       \
  if (XI_TILE4D_GET_DATA_ORDER(coeffT) == XI_WHDN)                                               \
  {                                                                                              \
    XI_CHECK_ERROR((XI_TILE4D_GET_DIM1(coeffT) == size) && (XI_TILE4D_GET_DIM2(coeffT) == size), \
                   XI_ERR_KSIZE, "The Coefficient Kernel Size is not supported");                \
  }                                                                                              \
  else if (XI_TILE4D_GET_DATA_ORDER(coeffT) == XI_NDWH)                                          \
  {                                                                                              \
    XI_CHECK_ERROR((XI_TILE4D_GET_DIM3(coeffT) == size) && (XI_TILE4D_GET_DIM4(coeffT) == size), \
                   XI_ERR_KSIZE, "The Coefficient Kernel Size is not supported");                \
  }

#define XI_CHECK_CONV_OUTPUT_TYPE_BASED_ON_RELU(outTile, param)                                       \
  if (XI_CNN_CONV_GET_FLAG_RELU(param))                                                               \
  {                                                                                                   \
    XI_CHECK_ERROR((XI_TILE3D_CHECK_TYPE(outTile, XI_U8)) || (XI_TILE3D_CHECK_TYPE(outTile, XI_S8))   \
                   || (XI_TILE3D_CHECK_TYPE(outTile, XI_S16)),                                        \
                   XI_ERR_DATATYPE, "The argument (" #outTile ") has wrong type");                    \
  }                                                                                                   \
  else                                                                                                \
  {                                                                                                   \
    XI_CHECK_ERROR((XI_TILE3D_CHECK_TYPE(outTile, XI_S8)) || (XI_TILE3D_CHECK_TYPE(outTile, XI_S16)), \
                   XI_ERR_DATATYPE, "The argument (" #outTile ") has wrong type");                    \
  }                                                                                                   \

#define XI_CHECK_STRIDE(param, stride) \
  XI_CHECK_ERROR(XI_CNN_CONV_GET_STRIDE(param) == stride, XI_ERR_BADARG, "The stride amount provided is not supported.");

#define XI_CHECK_STRIDE_CNNA(param, strideX, strideY) \
  XI_CHECK_ERROR((XI_CNNA_CONV_GET_STRIDEX(param) == strideX && XI_CNNA_CONV_GET_STRIDEY(param) == strideY), \
                  XI_ERR_BADARG, "The stride amount provided is not supported.");

#define XI_CHECK_DILATION(param, dilation) \
  XI_CHECK_ERROR(XI_CNN_CONV_GET_DILATION(param) == dilation, XI_ERR_BADARG, "The dilation value provided is not supported.");

#define XI_CHECK_DILATION_CNNA(param, dilationX, dilationY) \
  XI_CHECK_ERROR((XI_CNNA_CONV_GET_DILATIONX(param) == dilationX && XI_CNNA_CONV_GET_DILATIONY(param) == dilationY),\
                  XI_ERR_BADARG, "The dilation value provided is not supported.");

#define XI_CHECK_POOLING_STRIDE(param, stride) \
  XI_CHECK_ERROR(XI_CNN_POOLING_GET_STRIDE(param) == stride, XI_ERR_BADARG, "The stride amount provided is not supported.");


#define XI_CHECK_CONSISTENCY_A_FIXUP_MOD_DWH(inTile, coeffTile, fixupTile, param)                                                  \
  {                                                                                                                                \
    uint16_t dilationX      = XI_CNNA_CONV_GET_DILATIONX(param);                                                                   \
    uint16_t dilationY      = XI_CNNA_CONV_GET_DILATIONY(param);                                                                   \
    int32_t dilatedKWidth   = dilationX * (XI_TILE4D_GET_DIM3(coeffTile) - 1) + 1;                                                 \
    int32_t dilatedKHeight  = dilationY * (XI_TILE4D_GET_DIM4(coeffTile) - 1) + 1;                                                 \
    if (XI_TILE4D_GET_DATA_ORDER(coeffTile) == XI_NDWH)                                                                            \
    {                                                                                                                              \
      dilatedKWidth  = dilationX * (XI_TILE4D_GET_DIM3(coeffTile) - 1) + 1;                                                        \
      dilatedKHeight = dilationY * (XI_TILE4D_GET_DIM4(coeffTile) - 1) + 1;                                                        \
    }                                                                                                                              \
    else                                                                                                                           \
    {                                                                                                                              \
      dilatedKWidth  = dilationX * (XI_TILE4D_GET_DIM2(coeffTile) - 1) + 1;                                                        \
      dilatedKHeight = dilationY * (XI_TILE4D_GET_DIM3(coeffTile) - 1) + 1;                                                        \
    }                                                                                                                              \
    if (!(dilatedKWidth % 2 == 0))                                                                                                 \
    {                                                                                                                              \
      XI_CHECK_ERROR((XI_TILE_GET_WIDTH(fixupTile) <= (((XI_TILE3D_GET_DIM2(inTile) +                                              \
                                                         (dilatedKWidth >> 1) + (dilatedKWidth >> 1) - dilatedKWidth) >>           \
                                                        (XI_CNNA_CONV_GET_STRIDEX(param) >> 1)) + 1)), XI_ERR_DATASIZE,            \
                     "Fixup tile width is invalid.");                                                                              \
    }                                                                                                                              \
    else                                                                                                                           \
    {                                                                                                                              \
      XI_CHECK_ERROR((XI_TILE_GET_WIDTH(fixupTile) <= (((XI_TILE3D_GET_DIM2(inTile) +                                              \
                                                         (dilatedKWidth >> 1) + ((dilatedKWidth >> 1) - 1) - dilatedKWidth) >>     \
                                                        (XI_CNNA_CONV_GET_STRIDEX(param) >> 1)) + 1)), XI_ERR_DATASIZE,             \
                     "Fixup tile width is invalid.");                                                                              \
    }                                                                                                                              \
    if (!(dilatedKHeight % 2 == 0))                                                                                                \
    {                                                                                                                              \
      XI_CHECK_ERROR((XI_TILE_GET_HEIGHT(fixupTile) <= (((XI_TILE3D_GET_DIM3(inTile) +                                             \
                                                          (dilatedKHeight >> 1) + (dilatedKHeight >> 1) - dilatedKHeight) >>       \
                                                         (XI_CNNA_CONV_GET_STRIDEY(param) >> 1)) + 1)), XI_ERR_DATASIZE,            \
                     "Fixup tile height is invalid.");                                                                             \
    }                                                                                                                              \
    else                                                                                                                           \
    {                                                                                                                              \
      XI_CHECK_ERROR((XI_TILE_GET_HEIGHT(fixupTile) <= (((XI_TILE3D_GET_DIM3(inTile) +                                             \
                                                          (dilatedKHeight >> 1) + ((dilatedKHeight >> 1) - 1) - dilatedKHeight) >> \
                                                         (XI_CNNA_CONV_GET_STRIDEY(param) >> 1)) + 1)), XI_ERR_DATASIZE,            \
                     "Fixup tile height is is invalid.");                                                                          \
    }                                                                                                                              \
  }
#define XI_CHECK_CONSISTENCY_MOD_DWH(inT, coeffT, biasArr, outT, param)                                                                          \
  uint16_t dilatedKW_MOD = (uint16_t) (XI_CNN_CONV_GET_DILATION(param) * (XI_TILE4D_GET_DIM3(coeffT) - 1) + 1);                                  \
  uint16_t dilatedKH_MOD = (uint16_t) (XI_CNN_CONV_GET_DILATION(param) * (XI_TILE4D_GET_DIM4(coeffT) - 1) + 1);                                  \
  XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(inT) == XI_TILE4D_GET_DIM2(coeffT), XI_ERR_DATASIZE,                                                         \
                 "Number of Input Channels not equal to the number of channels in the Kernel.");                                                 \
  XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(outT) == XI_TILE4D_GET_DIM1(coeffT), XI_ERR_DATASIZE,                                                        \
                 "Number of Output Channels not equal to the number of Kernels.");                                                               \
  if (dilatedKW_MOD % 2 != 0)                                                                                                                    \
  {                                                                                                                                              \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM2(inT) + (dilatedKW_MOD >> 1)                                                \
                                                   + (dilatedKW_MOD >> 1) - dilatedKW_MOD) >> (XI_CNN_CONV_GET_STRIDE(param) >> 1)) + 1)),       \
                   XI_ERR_DATASIZE, "Output Width is invalid.");                                                                                 \
  }                                                                                                                                              \
  else                                                                                                                                           \
  {                                                                                                                                              \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM2(inT) + (dilatedKW_MOD >> 1)                                                \
                                                   + ((dilatedKW_MOD >> 1) - 1) - dilatedKW_MOD) >> (XI_CNN_CONV_GET_STRIDE(param) >> 1)) + 1)), \
                   XI_ERR_DATASIZE, "Output Width is invalid.");                                                                                 \
  }                                                                                                                                              \
  if (dilatedKH_MOD % 2 != 0)                                                                                                                    \
  {                                                                                                                                              \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3(outT) <= (((XI_TILE3D_GET_DIM3(inT) + (dilatedKH_MOD >> 1)                                                \
                                                   + (dilatedKH_MOD >> 1) - dilatedKH_MOD) >> (XI_CNN_CONV_GET_STRIDE(param) >> 1)) + 1)),       \
                   XI_ERR_DATASIZE, "Output Height is invalid.");                                                                                \
  }                                                                                                                                              \
  else                                                                                                                                           \
  {                                                                                                                                              \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3(outT) <= (((XI_TILE3D_GET_DIM3(inT) + (dilatedKH_MOD >> 1)                                                \
                                                   + ((dilatedKH_MOD >> 1) - 1) - dilatedKH_MOD) >> (XI_CNN_CONV_GET_STRIDE(param) >> 1)) + 1)), \
                   XI_ERR_DATASIZE, "Output Height is invalid.");                                                                                \
  }                                                                                                                                              \
  XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(biasArr) >= XI_TILE4D_GET_DIM1(coeffT), XI_ERR_DATASIZE,                                                     \
                 "Width of Bias Array is less than number of Kernels.");                                                                         \
  XI_CHECK_ERROR(XI_ARRAY_GET_HEIGHT(biasArray) > 0, XI_ERR_DATASIZE,                                                                            \
                 "Height of Bias Array should be greater than zero.");

#define XI_CHECK_CONSISTENCY_MOD_WHD_DWH(inT, coeffT, biasArr, outT, param)                                                                      \
  uint16_t dilatedKW_MOD = (uint16_t) (XI_CNN_CONV_GET_DILATION(param) * (XI_TILE4D_GET_DIM3(coeffT) - 1) + 1);                                  \
  uint16_t dilatedKH_MOD = (uint16_t) (XI_CNN_CONV_GET_DILATION(param) * (XI_TILE4D_GET_DIM4(coeffT) - 1) + 1);                                  \
  XI_CHECK_ERROR(XI_TILE3D_GET_DIM3(inT) == XI_TILE4D_GET_DIM2(coeffT), XI_ERR_DATASIZE,                                                         \
                 "Number of Input Channels not equal to the number of channels in the Kernel.");                                                 \
  XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(outT) == XI_TILE4D_GET_DIM1(coeffT), XI_ERR_DATASIZE,                                                        \
                 "Number of Output Channels not equal to the number of Kernels.");                                                               \
  if (dilatedKW_MOD % 2 != 0)                                                                                                                    \
  {                                                                                                                                              \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM1(inT) + (dilatedKW_MOD >> 1)                                                \
                                                   + (dilatedKW_MOD >> 1) - dilatedKW_MOD) >> (XI_CNN_CONV_GET_STRIDE(param) >> 1)) + 1)),       \
                   XI_ERR_DATASIZE, "Output Width is invalid.");                                                                                 \
  }                                                                                                                                              \
  else                                                                                                                                           \
  {                                                                                                                                              \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM1(inT) + (dilatedKW_MOD >> 1)                                                \
                                                   + ((dilatedKW_MOD >> 1) - 1) - dilatedKW_MOD) >> (XI_CNN_CONV_GET_STRIDE(param) >> 1)) + 1)), \
                   XI_ERR_DATASIZE, "Output Width is invalid.");                                                                                 \
  }                                                                                                                                              \
  if (dilatedKH_MOD % 2 != 0)                                                                                                                    \
  {                                                                                                                                              \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3(outT) <= (((XI_TILE3D_GET_DIM2(inT) + (dilatedKH_MOD >> 1)                                                \
                                                   + (dilatedKH_MOD >> 1) - dilatedKH_MOD) >> (XI_CNN_CONV_GET_STRIDE(param) >> 1)) + 1)),       \
                   XI_ERR_DATASIZE, "Output Height is invalid.");                                                                                \
  }                                                                                                                                              \
  else                                                                                                                                           \
  {                                                                                                                                              \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3(outT) <= (((XI_TILE3D_GET_DIM2(inT) + (dilatedKH_MOD >> 1)                                                \
                                                   + ((dilatedKH_MOD >> 1) - 1) - dilatedKH_MOD) >> (XI_CNN_CONV_GET_STRIDE(param) >> 1)) + 1)), \
                   XI_ERR_DATASIZE, "Output Height is invalid.");                                                                                \
  }                                                                                                                                              \
  XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(biasArr) >= XI_TILE4D_GET_DIM1(coeffT), XI_ERR_DATASIZE,                                                     \
                 "Width of Bias Array is less than number of Kernels.");                                                                         \
  XI_CHECK_ERROR(XI_ARRAY_GET_HEIGHT(biasArray) > 0, XI_ERR_DATASIZE,                                                                            \
                 "Height of Bias Array should be greater than zero.");

#define XI_CHECK_CONSISTENCY_MOW_WHD(inT, coeffT, biasArr, outT, param)                                                                          \
  XI_CHECK_ERROR(XI_TILE3D_GET_DIM3(inT) == XI_TILE4D_GET_DIM3(coeffT), XI_ERR_DATASIZE,                                                         \
                 "Number of Input Channels not equal to the number of channels in the Kernel.");                                                 \
  XI_CHECK_ERROR(XI_TILE3D_GET_DIM3(outT) == XI_TILE4D_GET_DIM4(coeffT), XI_ERR_DATASIZE,                                                        \
                 "Number of Output Channels not equal to the number of Kernels.");                                                               \
  uint16_t dilatedKW_MOW = (uint16_t) (XI_CNN_CONV_GET_DILATION(param) * (XI_TILE4D_GET_DIM1(coeffTile) - 1) + 1);                               \
  uint16_t dilatedKH_MOW = (uint16_t) (XI_CNN_CONV_GET_DILATION(param) * (XI_TILE4D_GET_DIM2(coeffTile) - 1) + 1);                               \
  if (dilatedKW_MOW % 2 != 0)                                                                                                                    \
  {                                                                                                                                              \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM1(outT) <= (((XI_TILE3D_GET_DIM1(inT) + (dilatedKW_MOW >> 1)                                                \
                                                   + (dilatedKW_MOW >> 1) - dilatedKW_MOW) >> (XI_CNN_CONV_GET_STRIDE(param) >> 1)) + 1)),       \
                   XI_ERR_DATASIZE, "Output Width is invalid.");                                                                                 \
  }                                                                                                                                              \
  else                                                                                                                                           \
  {                                                                                                                                              \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM1(outT) <= (((XI_TILE3D_GET_DIM1(inT) + (dilatedKW_MOW >> 1)                                                \
                                                   + ((dilatedKW_MOW >> 1) - 1) - dilatedKW_MOW) >> (XI_CNN_CONV_GET_STRIDE(param) >> 1)) + 1)), \
                   XI_ERR_DATASIZE, "Output Width is invalid.");                                                                                 \
  }                                                                                                                                              \
  if (dilatedKH_MOW % 2 != 0)                                                                                                                    \
  {                                                                                                                                              \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM2(inT) + (dilatedKH_MOW >> 1)                                                \
                                                   + (dilatedKH_MOW >> 1) - dilatedKH_MOW) >> (XI_CNN_CONV_GET_STRIDE(param) >> 1)) + 1)),       \
                   XI_ERR_DATASIZE, "Output Height is invalid.");                                                                                \
  }                                                                                                                                              \
  else                                                                                                                                           \
  {                                                                                                                                              \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM2(inT) + (dilatedKH_MOW >> 1)                                                \
                                                   + ((dilatedKH_MOW >> 1) - 1) - dilatedKH_MOW) >> (XI_CNN_CONV_GET_STRIDE(param) >> 1)) + 1)), \
                   XI_ERR_DATASIZE, "Output Height is invalid.");                                                                                \
  }                                                                                                                                              \
  XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(biasArr) >= XI_TILE4D_GET_DIM4(coeffT), XI_ERR_DATASIZE,                                                     \
                 "Width of Bias Array is less than number of Kernels.");                                                                         \
  XI_CHECK_ERROR(XI_ARRAY_GET_HEIGHT(biasArray) > 0, XI_ERR_DATASIZE,                                                                            \
                 "Height of Bias Array should be greater than zero.");

#define XI_CHECK_CONSISTENCY_SO_DWH(inT, coeffT, biasArr, outT, param)                                                                         \
  XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(inT) == XI_TILE4D_GET_DIM1(coeffT), XI_ERR_DATASIZE,                                                       \
                 "Number of Input Channels not equal to the number of channels in the Kernel.");                                               \
  XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(outT) == XI_TILE4D_GET_DIM4(coeffT), XI_ERR_DATASIZE,                                                      \
                 "Number of Output Channels not equal to the number of Kernels.");                                                             \
  uint16_t dilatedKW_SO = (uint16_t) (XI_CNN_CONV_GET_DILATION(param) * (XI_TILE4D_GET_DIM2(coeffTile) - 1) + 1);                              \
  uint16_t dilatedKH_SO = (uint16_t) (XI_CNN_CONV_GET_DILATION(param) * (XI_TILE4D_GET_DIM3(coeffTile) - 1) + 1);                              \
  if (dilatedKW_SO % 2 != 0)                                                                                                                   \
  {                                                                                                                                            \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM2(inT) + (dilatedKW_SO >> 1)                                               \
                                                   + (dilatedKW_SO >> 1) - dilatedKW_SO) >> (XI_CNN_CONV_GET_STRIDE(param) >> 1)) + 1)),       \
                   XI_ERR_DATASIZE, "Output Width is invalid.");                                                                               \
  }                                                                                                                                            \
  else                                                                                                                                         \
  {                                                                                                                                            \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM2(inT) + (dilatedKW_SO >> 1)                                               \
                                                   + ((dilatedKW_SO >> 1) - 1) - dilatedKW_SO) >> (XI_CNN_CONV_GET_STRIDE(param) >> 1)) + 1)), \
                   XI_ERR_DATASIZE, "Output Width is invalid.");                                                                               \
  }                                                                                                                                            \
  if (dilatedKH_SO % 2 != 0)                                                                                                                   \
  {                                                                                                                                            \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3(outT) <= (((XI_TILE3D_GET_DIM3(inT) + (dilatedKH_SO >> 1)                                               \
                                                   + (dilatedKH_SO >> 1) - dilatedKH_SO) >> (XI_CNN_CONV_GET_STRIDE(param) >> 1)) + 1)),       \
                   XI_ERR_DATASIZE, "Output Height is invalid.");                                                                              \
  }                                                                                                                                            \
  else                                                                                                                                         \
  {                                                                                                                                            \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3(outT) <= (((XI_TILE3D_GET_DIM3(inT) + (dilatedKH_SO >> 1)                                               \
                                                   + ((dilatedKH_SO >> 1) - 1) - dilatedKH_SO) >> (XI_CNN_CONV_GET_STRIDE(param) >> 1)) + 1)), \
                   XI_ERR_DATASIZE, "Output Height is invalid.");                                                                              \
  }                                                                                                                                            \
  XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(biasArr) >= XI_TILE4D_GET_DIM4(coeffT), XI_ERR_DATASIZE,                                                   \
                 "Width of Bias Array is less than number of Kernels.");                                                                       \
  XI_CHECK_ERROR(XI_ARRAY_GET_HEIGHT(biasArray) > 0, XI_ERR_DATASIZE,                                                                          \
                 "Height of Bias Array should be greater than zero.");

#define XI_CHECK_COEFFTILE_CONTIGUOUS(coeffT, param)                                 \
  XI_CHECK_ERROR((XI_TILE4D_GET_DIM1_PITCH(coeffT) == XI_TILE4D_GET_DIM1(coeffT)) && \
                 (XI_TILE4D_GET_DIM2_PITCH(coeffT) == XI_TILE4D_GET_DIM1(coeffT) *   \
                  XI_TILE4D_GET_DIM2(coeffT)), XI_ERR_BADARG,                        \
                 "CoeffTile is not contiguous.");

#define XI_CHECK_TILE3D_EQUAL(tile1, tile2)                                               \
  XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(tile1) == XI_TILE3D_GET_DIM1(tile2) &&                \
                 XI_TILE3D_GET_DIM2(tile1) == XI_TILE3D_GET_DIM2(tile2) &&                \
                 XI_TILE3D_GET_DIM3(tile1) == XI_TILE3D_GET_DIM3(tile2), XI_ERR_DATASIZE, \
                 "Tiles sizes are not equal.");

#define XI_CHECK_TILE3D_EDGES_EQUAL(tile1, tile2)                                                   \
  XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(tile1) == XI_TILE3D_GET_DIM1_EDGE1(tile2) &&             \
                  XI_TILE3D_GET_DIM1_EDGE2(tile1) == XI_TILE3D_GET_DIM1_EDGE2(tile2) &&             \
                  XI_TILE3D_GET_DIM2_EDGE1(tile1) == XI_TILE3D_GET_DIM2_EDGE1(tile2) &&             \
                  XI_TILE3D_GET_DIM2_EDGE2(tile1) == XI_TILE3D_GET_DIM2_EDGE2(tile2) &&             \
                  XI_TILE3D_GET_DIM3_EDGE1(tile1) == XI_TILE3D_GET_DIM3_EDGE1(tile2) &&             \
                  XI_TILE3D_GET_DIM3_EDGE2(tile1) == XI_TILE3D_GET_DIM3_EDGE2(tile2)), XI_ERR_EDGE, \
                 "Tile edge dimensions are not equal.");

#define XI_CHECK_TILE3D_ELEMENT_SIZE_EQ(inT, outT)                                    \
  XI_CHECK_ERROR(XI_TILE3D_GET_ELEMENT_SIZE(inT) == XI_TILE3D_GET_ELEMENT_SIZE(outT), \
                 XI_ERR_DATATYPE, "Input tile element element size must be equal to output tile element size")

#define XI_CHECK_CONSISTENCY_POOL_WHD(inT, outT, param)                                                                                                                                    \
  XI_CHECK_ERROR(XI_TILE3D_GET_DIM3(inT) == XI_TILE3D_GET_DIM3(outT),                                                                                                                      \
                 XI_ERR_CHANNEL_INVALID, "Number of input and output channels don't match");                                                                                               \
  if (XI_CNN_POOLING_GET_KERNELWIDTH(param) % 2 != 0)                                                                                                                                      \
  {                                                                                                                                                                                        \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inT) >= (XI_CNN_POOLING_GET_KERNELWIDTH(param) / 2) &&                                                                                        \
                    XI_TILE3D_GET_DIM1_EDGE2(inT) >= (XI_CNN_POOLING_GET_KERNELWIDTH(param) / 2)),                                                                                         \
                   XI_ERR_EDGE, "Invalid edge for odd kernel size");                                                                                                                       \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM1(outT) <= (((XI_TILE3D_GET_DIM1(inT) + (XI_CNN_POOLING_GET_KERNELWIDTH(param) >> 1)                                                                  \
                                                   + (XI_CNN_POOLING_GET_KERNELWIDTH(param) >> 1) - XI_CNN_POOLING_GET_KERNELWIDTH(param)) / (XI_CNN_POOLING_GET_STRIDEX(param))) + 1)),   \
                   XI_ERR_BADARG, "Output Width is invalid.");                                                                                                                             \
  }                                                                                                                                                                                        \
  else                                                                                                                                                                                     \
  {                                                                                                                                                                                        \
    if (XI_CNN_POOLING_GET_LEFTEDGE_FLAG(param))                                                                                                                                           \
    {                                                                                                                                                                                      \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inT) >= (XI_CNN_POOLING_GET_KERNELWIDTH(param) / 2) &&                                                                                      \
                      XI_TILE3D_GET_DIM1_EDGE2(inT) >= ((XI_CNN_POOLING_GET_KERNELWIDTH(param) / 2) - 1)),                                                                                 \
                     XI_ERR_EDGE, "Invalid edge for even kernel size with left edge flag set");                                                                                            \
    }                                                                                                                                                                                      \
    else                                                                                                                                                                                   \
    {                                                                                                                                                                                      \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inT) >= ((XI_CNN_POOLING_GET_KERNELWIDTH(param) / 2) - 1) &&                                                                                \
                      XI_TILE3D_GET_DIM1_EDGE2(inT) >= (XI_CNN_POOLING_GET_KERNELWIDTH(param) / 2)),                                                                                       \
                     XI_ERR_EDGE, "Invalid edge for even kernel size with left edge flag reset");                                                                                          \
    }                                                                                                                                                                                      \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM1(outT) <= (((XI_TILE3D_GET_DIM1(inT) + ((XI_CNN_POOLING_GET_KERNELWIDTH(param) >> 1) - 1)                                                            \
                                                   + (XI_CNN_POOLING_GET_KERNELWIDTH(param) >> 1) - XI_CNN_POOLING_GET_KERNELWIDTH(param)) / (XI_CNN_POOLING_GET_STRIDEX(param))) + 1)),   \
                   XI_ERR_BADARG, "Output Width is invalid.");                                                                                                                             \
  }                                                                                                                                                                                        \
  if (XI_CNN_POOLING_GET_KERNELHEIGHT(param) % 2 != 0)                                                                                                                                     \
  {                                                                                                                                                                                        \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inT) >= (XI_CNN_POOLING_GET_KERNELHEIGHT(param) / 2) &&                                                                                       \
                    XI_TILE3D_GET_DIM2_EDGE2(inT) >= (XI_CNN_POOLING_GET_KERNELHEIGHT(param) / 2)),                                                                                        \
                   XI_ERR_EDGE, "Invalid edge for odd kernel size");                                                                                                                       \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM2(inT) + (XI_CNN_POOLING_GET_KERNELHEIGHT(param) >> 1)                                                                 \
                                                   + (XI_CNN_POOLING_GET_KERNELHEIGHT(param) >> 1) - XI_CNN_POOLING_GET_KERNELHEIGHT(param)) / (XI_CNN_POOLING_GET_STRIDEY(param))) + 1)), \
                   XI_ERR_BADARG, "Output Height is invalid.");                                                                                                                            \
  }                                                                                                                                                                                        \
  else                                                                                                                                                                                     \
  {                                                                                                                                                                                        \
    if (XI_CNN_POOLING_GET_TOPEDGE_FLAG(param))                                                                                                                                            \
    {                                                                                                                                                                                      \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inT) >= (XI_CNN_POOLING_GET_KERNELHEIGHT(param) / 2) &&                                                                                     \
                      XI_TILE3D_GET_DIM2_EDGE2(inT) >= ((XI_CNN_POOLING_GET_KERNELHEIGHT(param) / 2) - 1)),                                                                                \
                     XI_ERR_EDGE, "Invalid edge for even kernel size with top edge flag set");                                                                                             \
    }                                                                                                                                                                                      \
    else                                                                                                                                                                                   \
    {                                                                                                                                                                                      \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inT) >= ((XI_CNN_POOLING_GET_KERNELHEIGHT(param) / 2) - 1) &&                                                                               \
                      XI_TILE3D_GET_DIM2_EDGE2(inT) >= (XI_CNN_POOLING_GET_KERNELHEIGHT(param) / 2)),                                                                                      \
                     XI_ERR_EDGE, "Invalid edge for even kernel size with top edge flag reset");                                                                                           \
    }                                                                                                                                                                                      \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM2(inT) + ((XI_CNN_POOLING_GET_KERNELHEIGHT(param) >> 1) - 1)                                                           \
                                                   + (XI_CNN_POOLING_GET_KERNELHEIGHT(param) >> 1) - XI_CNN_POOLING_GET_KERNELHEIGHT(param)) / (XI_CNN_POOLING_GET_STRIDEY(param))) + 1)), \
                   XI_ERR_BADARG, "Output Height is invalid.");                                                                                                                            \
  }

#define XI_CHECK_CONSISTENCY_POOL_DWH(inT, outT, param)                                                                                                                                    \
  XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(inT) == XI_TILE3D_GET_DIM1(outT),                                                                                                                      \
                 XI_ERR_CHANNEL_INVALID, "Number of input and output channels don't match");                                                                                               \
  if ((XI_CNN_POOLING_GET_KERNELWIDTH(param) % 2 != 0))                                                                                                                                    \
  {                                                                                                                                                                                        \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inT) >= (XI_CNN_POOLING_GET_KERNELWIDTH(param) / 2) &&                                                                                        \
                    XI_TILE3D_GET_DIM2_EDGE2(inT) >= (XI_CNN_POOLING_GET_KERNELWIDTH(param) / 2)),                                                                                         \
                   XI_ERR_EDGE, "Invalid edge for odd kernel size");                                                                                                                       \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM2(inT) + (XI_CNN_POOLING_GET_KERNELWIDTH(param) >> 1)                                                                  \
                                                   + (XI_CNN_POOLING_GET_KERNELWIDTH(param) >> 1) - XI_CNN_POOLING_GET_KERNELWIDTH(param)) / (XI_CNN_POOLING_GET_STRIDEX(param))) + 1)),   \
                   XI_ERR_BADARG, "Output Width is invalid.");                                                                                                                             \
  }                                                                                                                                                                                        \
  else                                                                                                                                                                                     \
  {                                                                                                                                                                                        \
    if (XI_CNN_POOLING_GET_LEFTEDGE_FLAG(param))                                                                                                                                           \
    {                                                                                                                                                                                      \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inT) >= (XI_CNN_POOLING_GET_KERNELWIDTH(param) / 2) &&                                                                                      \
                      XI_TILE3D_GET_DIM2_EDGE2(inT) >= ((XI_CNN_POOLING_GET_KERNELWIDTH(param) / 2) - 1)),                                                                                 \
                     XI_ERR_EDGE, "Invalid edge for even kernel size with left edge flag set");                                                                                            \
    }                                                                                                                                                                                      \
    else                                                                                                                                                                                   \
    {                                                                                                                                                                                      \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inT) >= ((XI_CNN_POOLING_GET_KERNELWIDTH(param) / 2) - 1) &&                                                                                \
                      XI_TILE3D_GET_DIM2_EDGE2(inT) >= (XI_CNN_POOLING_GET_KERNELWIDTH(param) / 2)),                                                                                       \
                     XI_ERR_EDGE, "Invalid edge for even kernel size with left edge flag reset");                                                                                          \
    }                                                                                                                                                                                      \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM2(inT) + ((XI_CNN_POOLING_GET_KERNELWIDTH(param) >> 1) - 1)                                                            \
                                                   + (XI_CNN_POOLING_GET_KERNELWIDTH(param) >> 1) - XI_CNN_POOLING_GET_KERNELWIDTH(param)) / (XI_CNN_POOLING_GET_STRIDEX(param))) + 1)),   \
                   XI_ERR_BADARG, "Output Width is invalid.");                                                                                                                             \
  }                                                                                                                                                                                        \
  if ((XI_CNN_POOLING_GET_KERNELHEIGHT(param) % 2 != 0))                                                                                                                                   \
  {                                                                                                                                                                                        \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3_EDGE1(inT) >= (XI_CNN_POOLING_GET_KERNELHEIGHT(param) / 2) &&                                                                                       \
                    XI_TILE3D_GET_DIM3_EDGE2(inT) >= (XI_CNN_POOLING_GET_KERNELHEIGHT(param) / 2)),                                                                                        \
                   XI_ERR_EDGE, "Invalid edge for odd kernel size");                                                                                                                       \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3(outT) <= (((XI_TILE3D_GET_DIM3(inT) + (XI_CNN_POOLING_GET_KERNELHEIGHT(param) >> 1)                                                                 \
                                                   + (XI_CNN_POOLING_GET_KERNELHEIGHT(param) >> 1) - XI_CNN_POOLING_GET_KERNELHEIGHT(param)) / (XI_CNN_POOLING_GET_STRIDEY(param))) + 1)), \
                   XI_ERR_BADARG, "Output Height is invalid.");                                                                                                                            \
  }                                                                                                                                                                                        \
  else                                                                                                                                                                                     \
  {                                                                                                                                                                                        \
    if (XI_CNN_POOLING_GET_TOPEDGE_FLAG(param))                                                                                                                                            \
    {                                                                                                                                                                                      \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM3_EDGE1(inT) >= (XI_CNN_POOLING_GET_KERNELHEIGHT(param) / 2) &&                                                                                     \
                      XI_TILE3D_GET_DIM3_EDGE2(inT) >= ((XI_CNN_POOLING_GET_KERNELHEIGHT(param) / 2) - 1)),                                                                                \
                     XI_ERR_EDGE, "Invalid edge for even kernel size with top edge flag set");                                                                                             \
    }                                                                                                                                                                                      \
    else                                                                                                                                                                                   \
    {                                                                                                                                                                                      \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM3_EDGE1(inT) >= ((XI_CNN_POOLING_GET_KERNELHEIGHT(param) / 2) - 1) &&                                                                               \
                      XI_TILE3D_GET_DIM3_EDGE2(inT) >= (XI_CNN_POOLING_GET_KERNELHEIGHT(param) / 2)),                                                                                      \
                     XI_ERR_EDGE, "Invalid edge for even kernel size with top edge flag reset");                                                                                           \
    }                                                                                                                                                                                      \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3(outT) <= (((XI_TILE3D_GET_DIM3(inT) + ((XI_CNN_POOLING_GET_KERNELHEIGHT(param) >> 1) - 1)                                                           \
                                                   + (XI_CNN_POOLING_GET_KERNELHEIGHT(param) >> 1) - XI_CNN_POOLING_GET_KERNELHEIGHT(param)) / (XI_CNN_POOLING_GET_STRIDEY(param))) + 1)), \
                   XI_ERR_BADARG, "Output Height is invalid.");                                                                                                                            \
  }


#define XI_CHECK_EDGES_MOW_WHD(inTile, coeffTile, param)                                                       \
  uint16_t dilatedKW = (uint16_t) (XI_CNN_CONV_GET_DILATION(param) * (XI_TILE4D_GET_DIM1(coeffTile) - 1) + 1); \
  uint16_t dilatedKH = (uint16_t) (XI_CNN_CONV_GET_DILATION(param) * (XI_TILE4D_GET_DIM2(coeffTile) - 1) + 1); \
  if (dilatedKW % 2 != 0)                                                                                      \
  {                                                                                                            \
    if (dilatedKH % 2 != 0)                                                                                    \
    {                                                                                                          \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= dilatedKW / 2)                                       \
                     && (XI_TILE3D_GET_DIM1_EDGE2(inTile) >= dilatedKW / 2)                                    \
                     && (XI_TILE3D_GET_DIM2_EDGE1(inTile) >= dilatedKH / 2)                                    \
                     && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= dilatedKH / 2),                                   \
                     XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                       \
    }                                                                                                          \
    else                                                                                                       \
    {                                                                                                          \
      if (XI_CNN_CONV_GET_FLAG_TOPEDGE(param))                                                                 \
      {                                                                                                        \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= dilatedKW / 2)                                     \
                       && (XI_TILE3D_GET_DIM1_EDGE2(inTile) >= dilatedKW / 2)                                  \
                       && (XI_TILE3D_GET_DIM2_EDGE1(inTile) >= dilatedKH / 2)                                  \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((dilatedKH / 2) - 1)),                         \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                     \
      }                                                                                                        \
      else                                                                                                     \
      {                                                                                                        \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= dilatedKW / 2)                                     \
                       && (XI_TILE3D_GET_DIM1_EDGE2(inTile) >= dilatedKW / 2)                                  \
                       && (XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((dilatedKH / 2) - 1))                          \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= dilatedKH / 2),                                 \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                     \
      }                                                                                                        \
    }                                                                                                          \
  }                                                                                                            \
  else                                                                                                         \
  {                                                                                                            \
    if (dilatedKH % 2 != 0)                                                                                    \
    {                                                                                                          \
      if (XI_CNN_CONV_GET_FLAG_LEFTEDGE(param))                                                                \
      {                                                                                                        \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= dilatedKW / 2)                                     \
                       && (XI_TILE3D_GET_DIM1_EDGE2(inTile) >= ((dilatedKW / 2) - 1))                          \
                       && (XI_TILE3D_GET_DIM2_EDGE1(inTile) >= dilatedKH / 2)                                  \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= dilatedKH / 2),                                 \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                     \
      }                                                                                                        \
      else                                                                                                     \
      {                                                                                                        \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= ((dilatedKW / 2) - 1))                             \
                       && (XI_TILE3D_GET_DIM1_EDGE2(inTile) >= dilatedKW / 2)                                  \
                       && (XI_TILE3D_GET_DIM2_EDGE1(inTile) >= dilatedKH / 2)                                  \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= dilatedKH / 2),                                 \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                     \
      }                                                                                                        \
    }                                                                                                          \
    else                                                                                                       \
    {                                                                                                          \
      if (XI_CNN_CONV_GET_FLAG_TOPEDGE(param))                                                                 \
      {                                                                                                        \
        if (XI_CNN_CONV_GET_FLAG_LEFTEDGE(param))                                                              \
        {                                                                                                      \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= (dilatedKW / 2) &&                               \
                          XI_TILE3D_GET_DIM1_EDGE2(inTile) >= ((dilatedKW / 2) - 1) &&                         \
                          XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (dilatedKH / 2) &&                               \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((dilatedKH / 2) - 1)),                          \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                   \
        }                                                                                                      \
        else                                                                                                   \
        {                                                                                                      \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= ((dilatedKW / 2) - 1) &&                         \
                          XI_TILE3D_GET_DIM1_EDGE2(inTile) >= (dilatedKW / 2) &&                               \
                          XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (dilatedKH / 2) &&                               \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((dilatedKH / 2) - 1)),                          \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                   \
        }                                                                                                      \
      }                                                                                                        \
      else                                                                                                     \
      {                                                                                                        \
        if (XI_CNN_CONV_GET_FLAG_LEFTEDGE(param))                                                              \
        {                                                                                                      \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= (dilatedKW / 2) &&                               \
                          XI_TILE3D_GET_DIM1_EDGE2(inTile) >= (dilatedKW / 2 - 1) &&                           \
                          XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((dilatedKH / 2) - 1) &&                         \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (dilatedKH / 2)),                                \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                   \
        }                                                                                                      \
        else                                                                                                   \
        {                                                                                                      \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= ((dilatedKW / 2) - 1) &&                         \
                          XI_TILE3D_GET_DIM1_EDGE2(inTile) >= (dilatedKW / 2) &&                               \
                          XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((dilatedKH / 2) - 1) &&                         \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (dilatedKH / 2)),                                \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                   \
        }                                                                                                      \
      }                                                                                                        \
    }                                                                                                          \
  }


#define XI_CHECK_EDGES_MOD_WHD(inTile, coeffTile, param)                                                       \
  uint16_t dilatedKW = (uint16_t) (XI_CNN_CONV_GET_DILATION(param) * (XI_TILE4D_GET_DIM3(coeffTile) - 1) + 1); \
  uint16_t dilatedKH = (uint16_t) (XI_CNN_CONV_GET_DILATION(param) * (XI_TILE4D_GET_DIM4(coeffTile) - 1) + 1); \
  if (dilatedKW % 2 != 0)                                                                                      \
  {                                                                                                            \
    if (dilatedKH % 2 != 0)                                                                                    \
    {                                                                                                          \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= dilatedKW / 2)                                       \
                     && (XI_TILE3D_GET_DIM1_EDGE2(inTile) >= dilatedKW / 2)                                    \
                     && (XI_TILE3D_GET_DIM2_EDGE1(inTile) >= dilatedKH / 2)                                    \
                     && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= dilatedKH / 2),                                   \
                     XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                       \
    }                                                                                                          \
    else                                                                                                       \
    {                                                                                                          \
      if (XI_CNN_CONV_GET_FLAG_TOPEDGE(param))                                                                 \
      {                                                                                                        \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= dilatedKW / 2)                                     \
                       && (XI_TILE3D_GET_DIM1_EDGE2(inTile) >= dilatedKW / 2)                                  \
                       && (XI_TILE3D_GET_DIM2_EDGE1(inTile) >= dilatedKH / 2)                                  \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((dilatedKH / 2) - 1)),                         \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                     \
      }                                                                                                        \
      else                                                                                                     \
      {                                                                                                        \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= dilatedKW / 2)                                     \
                       && (XI_TILE3D_GET_DIM1_EDGE2(inTile) >= dilatedKW / 2)                                  \
                       && (XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((dilatedKH / 2) - 1))                          \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= dilatedKH / 2),                                 \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                     \
      }                                                                                                        \
    }                                                                                                          \
  }                                                                                                            \
  else                                                                                                         \
  {                                                                                                            \
    if (dilatedKH % 2 != 0)                                                                                    \
    {                                                                                                          \
      if (XI_CNN_CONV_GET_FLAG_LEFTEDGE(param))                                                                \
      {                                                                                                        \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= dilatedKW / 2)                                     \
                       && (XI_TILE3D_GET_DIM1_EDGE2(inTile) >= (dilatedKW / 2) - 1)                            \
                       && (XI_TILE3D_GET_DIM2_EDGE1(inTile) >= dilatedKH / 2)                                  \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= dilatedKH / 2),                                 \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                     \
      }                                                                                                        \
      else                                                                                                     \
      {                                                                                                        \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= ((dilatedKW / 2) - 1))                             \
                       && (XI_TILE3D_GET_DIM1_EDGE2(inTile) >= dilatedKW / 2)                                  \
                       && (XI_TILE3D_GET_DIM2_EDGE1(inTile) >= dilatedKH / 2)                                  \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= dilatedKH / 2),                                 \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                     \
      }                                                                                                        \
    }                                                                                                          \
    else                                                                                                       \
    {                                                                                                          \
      if (XI_CNN_CONV_GET_FLAG_TOPEDGE(param))                                                                 \
      {                                                                                                        \
        if (XI_CNN_CONV_GET_FLAG_LEFTEDGE(param))                                                              \
        {                                                                                                      \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= (dilatedKW / 2) &&                               \
                          XI_TILE3D_GET_DIM1_EDGE2(inTile) >= ((dilatedKW / 2) - 1) &&                         \
                          XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (dilatedKH / 2) &&                               \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((dilatedKH / 2) - 1)),                          \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                   \
        }                                                                                                      \
        else                                                                                                   \
        {                                                                                                      \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= ((dilatedKW / 2) - 1) &&                         \
                          XI_TILE3D_GET_DIM1_EDGE2(inTile) >= (dilatedKW / 2) &&                               \
                          XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (dilatedKH / 2) &&                               \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((dilatedKH / 2) - 1)),                          \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                   \
        }                                                                                                      \
      }                                                                                                        \
      else                                                                                                     \
      {                                                                                                        \
        if (XI_CNN_CONV_GET_FLAG_LEFTEDGE(param))                                                              \
        {                                                                                                      \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= (dilatedKW / 2) &&                               \
                          XI_TILE3D_GET_DIM1_EDGE2(inTile) >= ((dilatedKW / 2) - 1) &&                         \
                          XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((dilatedKH / 2) - 1) &&                         \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (dilatedKH / 2)),                                \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                   \
        }                                                                                                      \
        else                                                                                                   \
        {                                                                                                      \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= ((dilatedKW / 2) - 1) &&                         \
                          XI_TILE3D_GET_DIM1_EDGE2(inTile) >= (dilatedKW / 2) &&                               \
                          XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((dilatedKH / 2) - 1) &&                         \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (dilatedKH / 2)),                                \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                   \
        }                                                                                                      \
      }                                                                                                        \
    }                                                                                                          \
  }


#define XI_CHECK_EDGES_MOD_DWH(inTile, coeffTile, param)                                                       \
  uint16_t dilatedKW = (uint16_t) (XI_CNN_CONV_GET_DILATION(param) * (XI_TILE4D_GET_DIM3(coeffTile) - 1) + 1); \
  uint16_t dilatedKH = (uint16_t) (XI_CNN_CONV_GET_DILATION(param) * (XI_TILE4D_GET_DIM4(coeffTile) - 1) + 1); \
  if (dilatedKW % 2 != 0)                                                                                      \
  {                                                                                                            \
    if (dilatedKH % 2 != 0)                                                                                    \
    {                                                                                                          \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= dilatedKW / 2)                                       \
                     && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= dilatedKW / 2)                                    \
                     && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= dilatedKH / 2)                                    \
                     && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= dilatedKH / 2),                                   \
                     XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                       \
    }                                                                                                          \
    else                                                                                                       \
    {                                                                                                          \
      if (XI_CNN_CONV_GET_FLAG_TOPEDGE(param))                                                                 \
      {                                                                                                        \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= dilatedKW / 2)                                     \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= dilatedKW / 2)                                  \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= dilatedKH / 2)                                  \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((dilatedKH / 2) - 1)),                         \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                     \
      }                                                                                                        \
      else                                                                                                     \
      {                                                                                                        \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= dilatedKW / 2)                                     \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= dilatedKW / 2)                                  \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((dilatedKH / 2) - 1))                          \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= dilatedKH / 2),                                 \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                     \
      }                                                                                                        \
    }                                                                                                          \
  }                                                                                                            \
  else                                                                                                         \
  {                                                                                                            \
    if (dilatedKH % 2 != 0)                                                                                    \
    {                                                                                                          \
      if (XI_CNN_CONV_GET_FLAG_LEFTEDGE(param))                                                                \
      {                                                                                                        \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= dilatedKW / 2)                                     \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (dilatedKW / 2) - 1)                            \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= dilatedKH / 2)                                  \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= dilatedKH / 2),                                 \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                     \
      }                                                                                                        \
      else                                                                                                     \
      {                                                                                                        \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((dilatedKW / 2) - 1))                             \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= dilatedKW / 2)                                  \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= dilatedKH / 2)                                  \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= dilatedKH / 2),                                 \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                     \
      }                                                                                                        \
    }                                                                                                          \
    else                                                                                                       \
    {                                                                                                          \
      if (XI_CNN_CONV_GET_FLAG_TOPEDGE(param))                                                                 \
      {                                                                                                        \
        if (XI_CNN_CONV_GET_FLAG_LEFTEDGE(param))                                                              \
        {                                                                                                      \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (dilatedKW / 2) &&                               \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((dilatedKW / 2) - 1) &&                         \
                          XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (dilatedKH / 2) &&                               \
                          XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((dilatedKH / 2) - 1)),                          \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                   \
        }                                                                                                      \
        else                                                                                                   \
        {                                                                                                      \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((dilatedKW / 2) - 1) &&                         \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (dilatedKW / 2) &&                               \
                          XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (dilatedKH / 2) &&                               \
                          XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((dilatedKH / 2) - 1)),                          \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                   \
        }                                                                                                      \
      }                                                                                                        \
      else                                                                                                     \
      {                                                                                                        \
        if (XI_CNN_CONV_GET_FLAG_LEFTEDGE(param))                                                              \
        {                                                                                                      \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (dilatedKW / 2) &&                               \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((dilatedKW / 2) - 1) &&                         \
                          XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((dilatedKH / 2) - 1) &&                         \
                          XI_TILE3D_GET_DIM3_EDGE2(inTile) >= (dilatedKH / 2)),                                \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                   \
        }                                                                                                      \
        else                                                                                                   \
        {                                                                                                      \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((dilatedKW / 2) - 1) &&                         \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (dilatedKW / 2) &&                               \
                          XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((dilatedKH / 2) - 1) &&                         \
                          XI_TILE3D_GET_DIM3_EDGE2(inTile) >= (dilatedKH / 2)),                                \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                   \
        }                                                                                                      \
      }                                                                                                        \
    }                                                                                                          \
  }


#define XI_CHECK_EDGES_F16_MOD_DWH(inTile, coeffTile, param)                                                       \
  uint16_t dilatedKW = (uint16_t) (XI_CNNA_CONV_F16_GET_DILATION(param) * (XI_TILE4D_GET_DIM3(coeffTile) - 1) + 1); \
  uint16_t dilatedKH = (uint16_t) (XI_CNNA_CONV_F16_GET_DILATION(param) * (XI_TILE4D_GET_DIM4(coeffTile) - 1) + 1); \
  if (dilatedKW % 2 != 0)                                                                                      \
  {                                                                                                            \
    if (dilatedKH % 2 != 0)                                                                                    \
    {                                                                                                          \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= dilatedKW / 2)                                       \
                     && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= dilatedKW / 2)                                    \
                     && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= dilatedKH / 2)                                    \
                     && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= dilatedKH / 2),                                   \
                     XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                       \
    }                                                                                                          \
    else                                                                                                       \
    {                                                                                                          \
      if (XI_CNNA_CONV_F16_GET_FLAG_TOPEDGE(param))                                                                 \
      {                                                                                                        \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= dilatedKW / 2)                                     \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= dilatedKW / 2)                                  \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= dilatedKH / 2)                                  \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((dilatedKH / 2) - 1)),                         \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                     \
      }                                                                                                        \
      else                                                                                                     \
      {                                                                                                        \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= dilatedKW / 2)                                     \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= dilatedKW / 2)                                  \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((dilatedKH / 2) - 1))                          \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= dilatedKH / 2),                                 \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                     \
      }                                                                                                        \
    }                                                                                                          \
  }                                                                                                            \
  else                                                                                                         \
  {                                                                                                            \
    if (dilatedKH % 2 != 0)                                                                                    \
    {                                                                                                          \
      if (XI_CNNA_CONV_F16_GET_FLAG_LEFTEDGE(param))                                                                \
      {                                                                                                        \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= dilatedKW / 2)                                     \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (dilatedKW / 2) - 1)                            \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= dilatedKH / 2)                                  \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= dilatedKH / 2),                                 \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                     \
      }                                                                                                        \
      else                                                                                                     \
      {                                                                                                        \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((dilatedKW / 2) - 1))                             \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= dilatedKW / 2)                                  \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= dilatedKH / 2)                                  \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= dilatedKH / 2),                                 \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                     \
      }                                                                                                        \
    }                                                                                                          \
    else                                                                                                       \
    {                                                                                                          \
      if (XI_CNNA_CONV_F16_GET_FLAG_TOPEDGE(param))                                                                 \
      {                                                                                                        \
        if (XI_CNNA_CONV_F16_GET_FLAG_LEFTEDGE(param))                                                              \
        {                                                                                                      \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (dilatedKW / 2) &&                               \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((dilatedKW / 2) - 1) &&                         \
                          XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (dilatedKH / 2) &&                               \
                          XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((dilatedKH / 2) - 1)),                          \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                   \
        }                                                                                                      \
        else                                                                                                   \
        {                                                                                                      \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((dilatedKW / 2) - 1) &&                         \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (dilatedKW / 2) &&                               \
                          XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (dilatedKH / 2) &&                               \
                          XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((dilatedKH / 2) - 1)),                          \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                   \
        }                                                                                                      \
      }                                                                                                        \
      else                                                                                                     \
      {                                                                                                        \
        if (XI_CNNA_CONV_F16_GET_FLAG_LEFTEDGE(param))                                                              \
        {                                                                                                      \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (dilatedKW / 2) &&                               \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((dilatedKW / 2) - 1) &&                         \
                          XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((dilatedKH / 2) - 1) &&                         \
                          XI_TILE3D_GET_DIM3_EDGE2(inTile) >= (dilatedKH / 2)),                                \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                   \
        }                                                                                                      \
        else                                                                                                   \
        {                                                                                                      \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((dilatedKW / 2) - 1) &&                         \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (dilatedKW / 2) &&                               \
                          XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((dilatedKH / 2) - 1) &&                         \
                          XI_TILE3D_GET_DIM3_EDGE2(inTile) >= (dilatedKH / 2)),                                \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                   \
        }                                                                                                      \
      }                                                                                                        \
    }                                                                                                          \
  }

#define XI_CHECK_EDGES_SO(inTile, coeffTile, param)                                                            \
  uint16_t dilatedKW = (uint16_t) (XI_CNN_CONV_GET_DILATION(param) * (XI_TILE4D_GET_DIM2(coeffTile) - 1) + 1); \
  uint16_t dilatedKH = (uint16_t) (XI_CNN_CONV_GET_DILATION(param) * (XI_TILE4D_GET_DIM3(coeffTile) - 1) + 1); \
  if (dilatedKW % 2 != 0)                                                                                      \
  {                                                                                                            \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (dilatedKW / 2)) &&                                    \
                   (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (dilatedKW / 2)),                                      \
                   XI_ERR_EDGE, "Invalid edge for odd kernel size");                                           \
  }                                                                                                            \
  else                                                                                                         \
  {                                                                                                            \
    if (XI_CNN_CONV_GET_FLAG_LEFTEDGE(param))                                                                  \
    {                                                                                                          \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (dilatedKW / 2)) &&                                  \
                     (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (dilatedKW / 2 - 1)),                                \
                     XI_ERR_EDGE, "Invalid edge for even kernel size with left edge flag set");                \
    }                                                                                                          \
    else                                                                                                       \
    {                                                                                                          \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (dilatedKW / 2 - 1)) &&                              \
                     (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (dilatedKW / 2)),                                    \
                     XI_ERR_EDGE, "Invalid edge for even kernel size with left edge flag reset");              \
    }                                                                                                          \
  }                                                                                                            \
  if (dilatedKH % 2 != 0)                                                                                      \
  {                                                                                                            \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3_EDGE1(inTile) >= dilatedKH / 2) &&                                      \
                   (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= dilatedKH / 2),                                        \
                   XI_ERR_EDGE, "Invalid edge for odd kernel size");                                           \
  }                                                                                                            \
  else                                                                                                         \
  {                                                                                                            \
    if (XI_CNN_CONV_GET_FLAG_TOPEDGE(param))                                                                   \
    {                                                                                                          \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (dilatedKH / 2)) &&                                  \
                     (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= (dilatedKH / 2 - 1)),                                \
                     XI_ERR_EDGE, "Invalid edge for even kernel size with top edge flag set");                 \
    }                                                                                                          \
    else                                                                                                       \
    {                                                                                                          \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (dilatedKH / 2 - 1)) &&                              \
                     (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= (dilatedKH / 2)),                                    \
                     XI_ERR_EDGE, "Invalid edge for even kernel size with top edge flag reset");               \
    }                                                                                                          \
  }                                                                                                            \

#define ACC_INIT_BIAS(phvecBias1, phvecBias2, daccSum0, daccSum1, daccSum2, daccSum3,    \
                      remCh1, remCh2, remCh3, remCh4)  { \
    valign vaBias;                                                                       \
    xb_vecN_2x32v hvecBias1, hvecBias2;                                                  \
    vaBias = IVP_LAN_2X32_PP(phvecBias1);                                                \
    IVP_LAVN_2X32_XP(hvecBias1, vaBias, phvecBias1, remCh1);                             \
    IVP_LAVN_2X32_XP(hvecBias2, vaBias, phvecBias1, remCh2);                             \
    daccSum0 = IVP_CVT24UNX32L(hvecBias2, hvecBias1);                                    \
    daccSum1 = IVP_CVT24UNX32L(hvecBias2, hvecBias1);                                    \
    daccSum2 = IVP_CVT24UNX32L(hvecBias2, hvecBias1);                                    \
    daccSum3 = IVP_CVT24UNX32L(hvecBias2, hvecBias1);                                    \
    vaBias   = IVP_LAN_2X32_PP(phvecBias2);                                              \
    IVP_LAVN_2X32_XP(hvecBias1, vaBias, phvecBias2, remCh3);                             \
    IVP_LAVN_2X32_XP(hvecBias2, vaBias, phvecBias2, remCh4);                             \
    IVP_CVT24UNX32H(daccSum0, hvecBias2, hvecBias1);                                     \
    IVP_CVT24UNX32H(daccSum1, hvecBias2, hvecBias1);                                     \
    IVP_CVT24UNX32H(daccSum2, hvecBias2, hvecBias1);                                     \
    IVP_CVT24UNX32H(daccSum3, hvecBias2, hvecBias1);                                     \
}

#define ACC_INIT_BIAS_DECONV(phvecBias, numBias, daccSum0, daccSum1, daccSum2, daccSum3){ \
  xb_vecN_2x32v hvecBias1, hvecBias2;                                                     \
  valign vaBias = IVP_LAN_2X32_PP(phvecBias);                                             \
  IVP_LAVN_2X32_XP(hvecBias1, vaBias, phvecBias, 4*numBias);                              \
  IVP_LAVN_2X32_XP(hvecBias2, vaBias, phvecBias, 4*numBias - 2*XCHAL_IVPN_SIMD_WIDTH);    \
  daccSum0 = IVP_CVT24UNX32L(hvecBias2, hvecBias1);                                       \
  daccSum1 = IVP_CVT24UNX32L(hvecBias2, hvecBias1);                                       \
  daccSum2 = IVP_CVT24UNX32L(hvecBias2, hvecBias1);                                       \
  daccSum3 = IVP_CVT24UNX32L(hvecBias2, hvecBias1);                                       \
  IVP_LAVN_2X32_XP(hvecBias1, vaBias, phvecBias, 4*numBias - 4*XCHAL_IVPN_SIMD_WIDTH);    \
  IVP_LAVN_2X32_XP(hvecBias2, vaBias, phvecBias, 4*numBias - 6*XCHAL_IVPN_SIMD_WIDTH);    \
  IVP_CVT24UNX32H(daccSum0, hvecBias2, hvecBias1);                                        \
  IVP_CVT24UNX32H(daccSum1, hvecBias2, hvecBias1);                                        \
  IVP_CVT24UNX32H(daccSum2, hvecBias2, hvecBias1);                                        \
  IVP_CVT24UNX32H(daccSum3, hvecBias2, hvecBias1);                                        \
}

#define XI_CHECK_CONSISTENCY_DEPTHWISE_MOD_DWH(inT, coeffT, biasArr, outT, param)                                                  \
  int32_t KW_MOD = XI_TILE3D_GET_DIM2(coeffT);                                                                                    \
  int32_t KH_MOD = XI_TILE3D_GET_DIM3(coeffT);                                                                                    \
  XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(inT) == XI_TILE3D_GET_DIM1(coeffT), XI_ERR_DATASIZE,                                           \
                 "Number of Input Channels not equal to the number of channels in the Kernel.");                                   \
  XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(outT) == XI_TILE3D_GET_DIM1(coeffT), XI_ERR_DATASIZE,                                          \
                 "Number of Output Channels not equal to the number of channels in the Kernel.");                                  \
  if (KW_MOD % 2 != 0)                                                                                                             \
  {                                                                                                                                \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM2(inT) + (KW_MOD >> 1)                                         \
                                                   + (KW_MOD >> 1) - KW_MOD) >> (XI_CNN_CONV_GET_STRIDE(param) >> 1)) + 1)),       \
                   XI_ERR_DATASIZE, "Output Width is invalid.");                                                                   \
  }                                                                                                                                \
  else                                                                                                                             \
  {                                                                                                                                \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM2(inT) + (KW_MOD >> 1)                                         \
                                                   + ((KW_MOD >> 1) - 1) - KW_MOD) >> (XI_CNN_CONV_GET_STRIDE(param) >> 1)) + 1)), \
                   XI_ERR_DATASIZE, "Output Width is invalid.");                                                                   \
  }                                                                                                                                \
  if (KH_MOD % 2 != 0)                                                                                                             \
  {                                                                                                                                \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3(outT) <= (((XI_TILE3D_GET_DIM3(inT) + (KH_MOD >> 1)                                         \
                                                   + (KH_MOD >> 1) - KH_MOD) >> (XI_CNN_CONV_GET_STRIDE(param) >> 1)) + 1)),       \
                   XI_ERR_DATASIZE, "Output Height is invalid.");                                                                  \
  }                                                                                                                                \
  else                                                                                                                             \
  {                                                                                                                                \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3(outT) <= (((XI_TILE3D_GET_DIM3(inT) + (KH_MOD >> 1)                                         \
                                                   + ((KH_MOD >> 1) - 1) - KH_MOD) >> (XI_CNN_CONV_GET_STRIDE(param) >> 1)) + 1)), \
                   XI_ERR_DATASIZE, "Output Height is invalid.");                                                                  \
  }                                                                                                                                \
  XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(biasArr) >= XI_TILE3D_GET_DIM1(coeffT), XI_ERR_DATASIZE,                                       \
                 "Width of Bias Array is less than number of channels in the Kernel.");                                            \
  XI_CHECK_ERROR(XI_ARRAY_GET_HEIGHT(biasArray) > 0, XI_ERR_DATASIZE,                                                              \
                 "Height of Bias Array should be greater than zero.");

#define XI_CHECK_CONSISTENCY_F16_MOD_DWH(inT, coeffT, biasArr, outT, param)                                                  \
  int32_t KW_MOD = XI_TILE3D_GET_DIM2(coeffT);                                                                                    \
  int32_t KH_MOD = XI_TILE3D_GET_DIM3(coeffT);                                                                                    \
  XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(inT) == XI_TILE3D_GET_DIM2(coeffT), XI_ERR_DATASIZE,                                           \
                 "Number of Input Channels not equal to the number of channels in the Kernel.");                                   \
  XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(outT) == XI_TILE3D_GET_DIM1(coeffT), XI_ERR_DATASIZE,                                          \
                 "Number of Output Channels not equal to the number of channels in the Kernel.");                                  \
  if (KW_MOD % 2 != 0)                                                                                                             \
  {                                                                                                                                \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM2(inT) + (KW_MOD >> 1)                                         \
                                                   + (KW_MOD >> 1) - KW_MOD) >> (XI_CNNA_CONV_F16_GET_STRIDE(param) >> 1)) + 1)),       \
                   XI_ERR_DATASIZE, "Output Width is invalid.");                                                                   \
  }                                                                                                                                \
  else                                                                                                                             \
  {                                                                                                                                \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM2(inT) + (KW_MOD >> 1)                                         \
                                                   + ((KW_MOD >> 1) - 1) - KW_MOD) >> (XI_CNNA_CONV_F16_GET_STRIDE(param) >> 1)) + 1)), \
                   XI_ERR_DATASIZE, "Output Width is invalid.");                                                                   \
  }                                                                                                                                \
  if (KH_MOD % 2 != 0)                                                                                                             \
  {                                                                                                                                \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3(outT) <= (((XI_TILE3D_GET_DIM3(inT) + (KH_MOD >> 1)                                         \
                                                   + (KH_MOD >> 1) - KH_MOD) >> (XI_CNNA_CONV_F16_GET_STRIDE(param) >> 1)) + 1)),       \
                   XI_ERR_DATASIZE, "Output Height is invalid.");                                                                  \
  }                                                                                                                                \
  else                                                                                                                             \
  {                                                                                                                                \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3(outT) <= (((XI_TILE3D_GET_DIM3(inT) + (KH_MOD >> 1)                                         \
                                                   + ((KH_MOD >> 1) - 1) - KH_MOD) >> (XI_CNNA_CONV_F16_GET_STRIDE(param) >> 1)) + 1)), \
                   XI_ERR_DATASIZE, "Output Height is invalid.");                                                                  \
  }                                                                                                                                \
  XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(biasArr) >= XI_TILE3D_GET_DIM1(coeffT), XI_ERR_DATASIZE,                                       \
                 "Width of Bias Array is less than number of channels in the Kernel.");                                            \
  XI_CHECK_ERROR(XI_ARRAY_GET_HEIGHT(biasArray) > 0, XI_ERR_DATASIZE,                                                              \
                 "Height of Bias Array should be greater than zero.");

#define XI_CHECK_CONSISTENCY_DEPTHWISE_MOW_WHD(inT, coeffT, biasArr, outT, param)                                                \
  XI_CHECK_ERROR(XI_TILE3D_GET_DIM3(inT) == XI_TILE3D_GET_DIM3(coeffT), XI_ERR_DATASIZE,                                         \
                 "Number of Input Channels not equal to the number of channels in the Kernel.");                                 \
  XI_CHECK_ERROR(XI_TILE3D_GET_DIM3(outT) == XI_TILE3D_GET_DIM3(coeffT), XI_ERR_DATASIZE,                                        \
                 "Number of Output Channels not equal to the number of channels in the Kernel.");                                \
  int32_t kW_MOW = XI_TILE3D_GET_DIM1(coeffTile);                                                                               \
  int32_t kH_MOW = XI_TILE3D_GET_DIM2(coeffTile);                                                                               \
  if (kW_MOW % 2 != 0)                                                                                                           \
  {                                                                                                                              \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM1(outT) <= (((XI_TILE3D_GET_DIM1(inT) + (kW_MOW >> 1) +                                     \
                                                   (kW_MOW >> 1) - kW_MOW) >> (XI_CNN_CONV_GET_STRIDE(param) >> 1)) + 1)),       \
                   XI_ERR_DATASIZE, "Output Width is invalid.");                                                                 \
  }                                                                                                                              \
  else                                                                                                                           \
  {                                                                                                                              \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM1(outT) <= (((XI_TILE3D_GET_DIM1(inT) + (kW_MOW >> 1) +                                     \
                                                   ((kW_MOW >> 1) - 1) - kW_MOW) >> (XI_CNN_CONV_GET_STRIDE(param) >> 1)) + 1)), \
                   XI_ERR_DATASIZE, "Output Width is invalid.");                                                                 \
  }                                                                                                                              \
  if (kH_MOW % 2 != 0)                                                                                                           \
  {                                                                                                                              \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM2(inT) + (kH_MOW >> 1) +                                     \
                                                   (kH_MOW >> 1) - kH_MOW) >> (XI_CNN_CONV_GET_STRIDE(param) >> 1)) + 1)),       \
                   XI_ERR_DATASIZE, "Output Height is invalid.");                                                                \
  }                                                                                                                              \
  else                                                                                                                           \
  {                                                                                                                              \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM2(inT) + (kH_MOW >> 1) +                                     \
                                                   ((kH_MOW >> 1) - 1) - kH_MOW) >> (XI_CNN_CONV_GET_STRIDE(param) >> 1)) + 1)), \
                   XI_ERR_DATASIZE, "Output Height is invalid.");                                                                \
  }                                                                                                                              \
  XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(biasArr) >= XI_TILE3D_GET_DIM3(coeffT), XI_ERR_DATASIZE,                                     \
                 "Width of Bias Array is less than number of channels in the Kernel.");                                          \
  XI_CHECK_ERROR(XI_ARRAY_GET_HEIGHT(biasArray) > 0, XI_ERR_DATASIZE,                                                            \
                 "Height of Bias Array should be greater than zero.");

#define XI_CHECK_KERNEL_SIZE_DEPTHWISE(coeffT, size)                                             \
  if (XI_TILE3D_GET_DATA_ORDER(coeffT) == XI_WHD)                                                \
  {                                                                                              \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM1(coeffT) == size) && (XI_TILE3D_GET_DIM2(coeffT) == size), \
                   XI_ERR_KSIZE, "The Coefficient Kernel Size is not supported");                \
  }                                                                                              \
  else if (XI_TILE3D_GET_DATA_ORDER(coeffT) == XI_DWH)                                           \
  {                                                                                              \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(coeffT) == size) && (XI_TILE3D_GET_DIM3(coeffT) == size), \
                   XI_ERR_KSIZE, "The Coefficient Kernel Size is not supported");                \
  }

#define XI_CHECK_EDGES_DEPTHWISE_MOW_WHD(inTile, coeffTile, param)                           \
  int32_t kW = XI_TILE3D_GET_DIM1(coeffTile);                                               \
  int32_t kH = XI_TILE3D_GET_DIM2(coeffTile);                                               \
  if (kW % 2 != 0)                                                                           \
  {                                                                                          \
    if (kH % 2 != 0)                                                                         \
    {                                                                                        \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= kW / 2)                            \
                     && (XI_TILE3D_GET_DIM1_EDGE2(inTile) >= kW / 2)                         \
                     && (XI_TILE3D_GET_DIM2_EDGE1(inTile) >= kH / 2)                         \
                     && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= kH / 2),                        \
                     XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");     \
    }                                                                                        \
    else                                                                                     \
    {                                                                                        \
      if (XI_CNN_CONV_GET_FLAG_TOPEDGE(param))                                               \
      {                                                                                      \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= kW / 2)                          \
                       && (XI_TILE3D_GET_DIM1_EDGE2(inTile) >= kW / 2)                       \
                       && (XI_TILE3D_GET_DIM2_EDGE1(inTile) >= kH / 2)                       \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((kH / 2) - 1)),              \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");   \
      }                                                                                      \
      else                                                                                   \
      {                                                                                      \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= kW / 2)                          \
                       && (XI_TILE3D_GET_DIM1_EDGE2(inTile) >= kW / 2)                       \
                       && (XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((kH / 2) - 1))               \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= kH / 2),                      \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");   \
      }                                                                                      \
    }                                                                                        \
  }                                                                                          \
  else                                                                                       \
  {                                                                                          \
    if (kH % 2 != 0)                                                                         \
    {                                                                                        \
      if (XI_CNN_CONV_GET_FLAG_LEFTEDGE(param))                                              \
      {                                                                                      \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= kW / 2)                          \
                       && (XI_TILE3D_GET_DIM1_EDGE2(inTile) >= ((kW / 2) - 1))               \
                       && (XI_TILE3D_GET_DIM2_EDGE1(inTile) >= kH / 2)                       \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= kH / 2),                      \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");   \
      }                                                                                      \
      else                                                                                   \
      {                                                                                      \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= ((kW / 2) - 1))                  \
                       && (XI_TILE3D_GET_DIM1_EDGE2(inTile) >= kW / 2)                       \
                       && (XI_TILE3D_GET_DIM2_EDGE1(inTile) >= kH / 2)                       \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= kH / 2),                      \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");   \
      }                                                                                      \
    }                                                                                        \
    else                                                                                     \
    {                                                                                        \
      if (XI_CNN_CONV_GET_FLAG_TOPEDGE(param))                                               \
      {                                                                                      \
        if (XI_CNN_CONV_GET_FLAG_LEFTEDGE(param))                                            \
        {                                                                                    \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= (kW / 2) &&                    \
                          XI_TILE3D_GET_DIM1_EDGE2(inTile) >= ((kW / 2) - 1) &&              \
                          XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (kH / 2) &&                    \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((kH / 2) - 1)),               \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data"); \
        }                                                                                    \
        else                                                                                 \
        {                                                                                    \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= ((kW / 2) - 1) &&              \
                          XI_TILE3D_GET_DIM1_EDGE2(inTile) >= (kW / 2) &&                    \
                          XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (kH / 2) &&                    \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((kH / 2) - 1)),               \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data"); \
        }                                                                                    \
      }                                                                                      \
      else                                                                                   \
      {                                                                                      \
        if (XI_CNN_CONV_GET_FLAG_LEFTEDGE(param))                                            \
        {                                                                                    \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= (kW / 2) &&                    \
                          XI_TILE3D_GET_DIM1_EDGE2(inTile) >= (kW / 2 - 1) &&                \
                          XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((kH / 2) - 1) &&              \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (kH / 2)),                     \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data"); \
        }                                                                                    \
        else                                                                                 \
        {                                                                                    \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM1_EDGE1(inTile) >= ((kW / 2) - 1) &&              \
                          XI_TILE3D_GET_DIM1_EDGE2(inTile) >= (kW / 2) &&                    \
                          XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((kH / 2) - 1) &&              \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (kH / 2)),                     \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data"); \
        }                                                                                    \
      }                                                                                      \
    }                                                                                        \
  }

#define XI_CHECK_EDGES_DEPTHWISE_MOD_DWH(inTile, coeffTile, param)                           \
  int32_t kW = XI_TILE3D_GET_DIM2(coeffTile);                                               \
  int32_t kH = XI_TILE3D_GET_DIM3(coeffTile);                                               \
  if (kW % 2 != 0)                                                                           \
  {                                                                                          \
    if (kH % 2 != 0)                                                                         \
    {                                                                                        \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= kW / 2)                            \
                     && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= kW / 2)                         \
                     && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= kH / 2)                         \
                     && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= kH / 2),                        \
                     XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");     \
    }                                                                                        \
    else                                                                                     \
    {                                                                                        \
      if (XI_CNN_CONV_GET_FLAG_TOPEDGE(param))                                               \
      {                                                                                      \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= kW / 2)                          \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= kW / 2)                       \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= kH / 2)                       \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((kH / 2) - 1)),              \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");   \
      }                                                                                      \
      else                                                                                   \
      {                                                                                      \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= kW / 2)                          \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= kW / 2)                       \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((kH / 2) - 1))               \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= kH / 2),                      \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");   \
      }                                                                                      \
    }                                                                                        \
  }                                                                                          \
  else                                                                                       \
  {                                                                                          \
    if (kH % 2 != 0)                                                                         \
    {                                                                                        \
      if (XI_CNN_CONV_GET_FLAG_LEFTEDGE(param))                                              \
      {                                                                                      \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= kW / 2)                          \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (kW / 2) - 1)                 \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= kH / 2)                       \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= kH / 2),                      \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");   \
      }                                                                                      \
      else                                                                                   \
      {                                                                                      \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((kW / 2) - 1))                  \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= kW / 2)                       \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= kH / 2)                       \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= kH / 2),                      \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");   \
      }                                                                                      \
    }                                                                                        \
    else                                                                                     \
    {                                                                                        \
      if (XI_CNN_CONV_GET_FLAG_TOPEDGE(param))                                               \
      {                                                                                      \
        if (XI_CNN_CONV_GET_FLAG_LEFTEDGE(param))                                            \
        {                                                                                    \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (kW / 2) &&                    \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((kW / 2) - 1) &&              \
                          XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (kH / 2) &&                    \
                          XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((kH / 2) - 1)),               \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data"); \
        }                                                                                    \
        else                                                                                 \
        {                                                                                    \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((kW / 2) - 1) &&              \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (kW / 2) &&                    \
                          XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (kH / 2) &&                    \
                          XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((kH / 2) - 1)),               \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data"); \
        }                                                                                    \
      }                                                                                      \
      else                                                                                   \
      {                                                                                      \
        if (XI_CNN_CONV_GET_FLAG_LEFTEDGE(param))                                            \
        {                                                                                    \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (kW / 2) &&                    \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((kW / 2) - 1) &&              \
                          XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((kH / 2) - 1) &&              \
                          XI_TILE3D_GET_DIM3_EDGE2(inTile) >= (kH / 2)),                     \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data"); \
        }                                                                                    \
        else                                                                                 \
        {                                                                                    \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((kW / 2) - 1) &&              \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (kW / 2) &&                    \
                          XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((kH / 2) - 1) &&              \
                          XI_TILE3D_GET_DIM3_EDGE2(inTile) >= (kH / 2)),                     \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data"); \
        }                                                                                    \
      }                                                                                      \
    }                                                                                        \
  }

#define XI_CHECK_EDGESA_SO(inTile, coeffTile, param)                                                            \
  uint16_t dilatedKW = (uint16_t) (XI_CNNA_CONV_GET_DILATIONX(param) * (XI_TILE4D_GET_DIM2(coeffTile) - 1) + 1); \
  uint16_t dilatedKH = (uint16_t) (XI_CNNA_CONV_GET_DILATIONY(param) * (XI_TILE4D_GET_DIM3(coeffTile) - 1) + 1); \
  if (dilatedKW % 2 != 0)                                                                                       \
  {                                                                                                             \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (dilatedKW / 2)) &&                                     \
                   (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (dilatedKW / 2)),                                       \
                   XI_ERR_EDGE, "Invalid edge for odd kernel size");                                            \
  }                                                                                                             \
  else                                                                                                          \
  {                                                                                                             \
    if (XI_CNNA_CONV_GET_FLAG_LEFTEDGE(param))                                                                  \
    {                                                                                                           \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (dilatedKW / 2)) &&                                   \
                     (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (dilatedKW / 2 - 1)),                                 \
                     XI_ERR_EDGE, "Invalid edge for even kernel size with left edge flag set");                 \
    }                                                                                                           \
    else                                                                                                        \
    {                                                                                                           \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (dilatedKW / 2 - 1)) &&                               \
                     (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (dilatedKW / 2)),                                     \
                     XI_ERR_EDGE, "Invalid edge for even kernel size with left edge flag reset");               \
    }                                                                                                           \
  }                                                                                                             \
  if (dilatedKH % 2 != 0)                                                                                       \
  {                                                                                                             \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3_EDGE1(inTile) >= dilatedKH / 2) &&                                       \
                   (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= dilatedKH / 2),                                         \
                   XI_ERR_EDGE, "Invalid edge for odd kernel size");                                            \
  }                                                                                                             \
  else                                                                                                          \
  {                                                                                                             \
    if (XI_CNNA_CONV_GET_FLAG_TOPEDGE(param))                                                                   \
    {                                                                                                           \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (dilatedKH / 2)) &&                                   \
                     (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= (dilatedKH / 2 - 1)),                                 \
                     XI_ERR_EDGE, "Invalid edge for even kernel size with top edge flag set");                  \
    }                                                                                                           \
    else                                                                                                        \
    {                                                                                                           \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (dilatedKH / 2 - 1)) &&                               \
                     (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= (dilatedKH / 2)),                                     \
                     XI_ERR_EDGE, "Invalid edge for even kernel size with top edge flag reset");                \
    }                                                                                                           \
  }                                                                                                             \

#define XI_CHECK_CONSISTENCYA_SO_DWH(inT, coeffT, biasArr, outT, param)                                                                         \
  XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(inT) == XI_TILE4D_GET_DIM1(coeffT), XI_ERR_DATASIZE,                                                        \
                 "Number of Input Channels not equal to the number of channels in the Kernel.");                                                \
  XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(outT) == XI_TILE4D_GET_DIM4(coeffT), XI_ERR_DATASIZE,                                                       \
                 "Number of Output Channels not equal to the number of Kernels.");                                                              \
  uint16_t dilatedKW_SO = (uint16_t) (XI_CNNA_CONV_GET_DILATIONX(param) * (XI_TILE4D_GET_DIM2(coeffTile) - 1) + 1);                              \
  uint16_t dilatedKH_SO = (uint16_t) (XI_CNNA_CONV_GET_DILATIONY(param) * (XI_TILE4D_GET_DIM3(coeffTile) - 1) + 1);                              \
  if (dilatedKW_SO % 2 != 0)                                                                                                                    \
  {                                                                                                                                             \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM2(inT) + (dilatedKW_SO >> 1)                                                \
                                                   + (dilatedKW_SO >> 1) - dilatedKW_SO) >> (XI_CNNA_CONV_GET_STRIDEX(param) >> 1)) + 1)),       \
                   XI_ERR_DATASIZE, "Output Width is invalid.");                                                                                \
  }                                                                                                                                             \
  else                                                                                                                                          \
  {                                                                                                                                             \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM2(inT) + (dilatedKW_SO >> 1)                                                \
                                                   + ((dilatedKW_SO >> 1) - 1) - dilatedKW_SO) >> (XI_CNNA_CONV_GET_STRIDEX(param) >> 1)) + 1)), \
                   XI_ERR_DATASIZE, "Output Width is invalid.");                                                                                \
  }                                                                                                                                             \
  if (dilatedKH_SO % 2 != 0)                                                                                                                    \
  {                                                                                                                                             \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3(outT) <= (((XI_TILE3D_GET_DIM3(inT) + (dilatedKH_SO >> 1)                                                \
                                                   + (dilatedKH_SO >> 1) - dilatedKH_SO) >> (XI_CNNA_CONV_GET_STRIDEY(param) >> 1)) + 1)),       \
                   XI_ERR_DATASIZE, "Output Height is invalid.");                                                                               \
  }                                                                                                                                             \
  else                                                                                                                                          \
  {                                                                                                                                             \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3(outT) <= (((XI_TILE3D_GET_DIM3(inT) + (dilatedKH_SO >> 1)                                                \
                                                   + ((dilatedKH_SO >> 1) - 1) - dilatedKH_SO) >> (XI_CNNA_CONV_GET_STRIDEY(param) >> 1)) + 1)), \
                   XI_ERR_DATASIZE, "Output Height is invalid.");                                                                               \
  }                                                                                                                                             \
  XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(biasArr) >= XI_TILE4D_GET_DIM4(coeffT), XI_ERR_DATASIZE,                                                    \
                 "Width of Bias Array is less than number of channels in the Kernel.");                                                         \
  XI_CHECK_ERROR(XI_ARRAY_GET_HEIGHT(biasArray) > 0, XI_ERR_DATASIZE,                                                                           \
                 "Height of Bias Array should be greater than zero.");

#define XI_CHECK_EDGESA_FIXUP(inTile, coeffTile, param)                                      \
  uint16_t dilationX = XI_CNNA_CONV_GET_DILATIONX(param);                                    \
  uint16_t dilationY = XI_CNNA_CONV_GET_DILATIONY(param);                                    \
  int32_t dilatedKW;                                                                         \
  int32_t dilatedKH;                                                                         \
  if (XI_TILE4D_GET_DATA_ORDER(coeffTile) == XI_NDWH)                                        \
  {                                                                                          \
    dilatedKW = dilationX * (XI_TILE4D_GET_DIM3(coeffTile) - 1) + 1;                         \
    dilatedKH = dilationY * (XI_TILE4D_GET_DIM4(coeffTile) - 1) + 1;                         \
  }                                                                                          \
  else                                                                                       \
  {                                                                                          \
    dilatedKW = dilationX * (XI_TILE4D_GET_DIM2(coeffTile) - 1) + 1;                         \
    dilatedKH = dilationY * (XI_TILE4D_GET_DIM3(coeffTile) - 1) + 1;                         \
  }                                                                                          \
  if (dilatedKW % 2 != 0)                                                                    \
  {                                                                                          \
    if (dilatedKH % 2 != 0)                                                                  \
    {                                                                                        \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= dilatedKW / 2)                     \
                     && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= dilatedKW / 2)                  \
                     && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= dilatedKH / 2)                  \
                     && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= dilatedKH / 2),                 \
                     XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");     \
    }                                                                                        \
    else                                                                                     \
    {                                                                                        \
      if (XI_CNNA_CONV_GET_FLAG_TOPEDGE(param))                                              \
      {                                                                                      \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= dilatedKW / 2)                   \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= dilatedKW / 2)                \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= dilatedKH / 2)                \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((dilatedKH / 2) - 1)),       \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");   \
      }                                                                                      \
      else                                                                                   \
      {                                                                                      \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= dilatedKW / 2)                   \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= dilatedKW / 2)                \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((dilatedKH / 2) - 1))        \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= dilatedKH / 2),               \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");   \
      }                                                                                      \
    }                                                                                        \
  }                                                                                          \
  else                                                                                       \
  {                                                                                          \
    if (dilatedKH % 2 != 0)                                                                  \
    {                                                                                        \
      if (XI_CNNA_CONV_GET_FLAG_LEFTEDGE(param))                                             \
      {                                                                                      \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= dilatedKW / 2)                   \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (dilatedKW / 2) - 1)          \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= dilatedKH / 2)                \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= dilatedKH / 2),               \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");   \
      }                                                                                      \
      else                                                                                   \
      {                                                                                      \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((dilatedKW / 2) - 1))           \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= dilatedKW / 2)                \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= dilatedKH / 2)                \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= dilatedKH / 2),               \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");   \
      }                                                                                      \
    }                                                                                        \
    else                                                                                     \
    {                                                                                        \
      if (XI_CNNA_CONV_GET_FLAG_TOPEDGE(param))                                              \
      {                                                                                      \
        if (XI_CNNA_CONV_GET_FLAG_LEFTEDGE(param))                                           \
        {                                                                                    \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (dilatedKW / 2) &&             \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((dilatedKW / 2) - 1) &&       \
                          XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (dilatedKH / 2) &&             \
                          XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((dilatedKH / 2) - 1)),        \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data"); \
        }                                                                                    \
        else                                                                                 \
        {                                                                                    \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((dilatedKW / 2) - 1) &&       \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (dilatedKW / 2) &&             \
                          XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (dilatedKH / 2) &&             \
                          XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((dilatedKH / 2) - 1)),        \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data"); \
        }                                                                                    \
      }                                                                                      \
      else                                                                                   \
      {                                                                                      \
        if (XI_CNNA_CONV_GET_FLAG_LEFTEDGE(param))                                           \
        {                                                                                    \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (dilatedKW / 2) &&             \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((dilatedKW / 2) - 1) &&       \
                          XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((dilatedKH / 2) - 1) &&       \
                          XI_TILE3D_GET_DIM3_EDGE2(inTile) >= (dilatedKH / 2)),              \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data"); \
        }                                                                                    \
        else                                                                                 \
        {                                                                                    \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((dilatedKW / 2) - 1) &&       \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (dilatedKW / 2) &&             \
                          XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((dilatedKH / 2) - 1) &&       \
                          XI_TILE3D_GET_DIM3_EDGE2(inTile) >= (dilatedKH / 2)),              \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data"); \
        }                                                                                    \
      }                                                                                      \
    }                                                                                        \
  }

#define XI_CHECK_EDGESA_MOD_DWH(inTile, coeffTile, param)                                                       \
  uint16_t dilatedKW = (uint16_t) (XI_CNNA_CONV_GET_DILATION(param) * (XI_TILE4D_GET_DIM3(coeffTile) - 1) + 1); \
  uint16_t dilatedKH = (uint16_t) (XI_CNNA_CONV_GET_DILATION(param) * (XI_TILE4D_GET_DIM4(coeffTile) - 1) + 1); \
  if (dilatedKW % 2 != 0)                                                                                       \
  {                                                                                                             \
    if (dilatedKH % 2 != 0)                                                                                     \
    {                                                                                                           \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= dilatedKW / 2)                                        \
                     && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= dilatedKW / 2)                                     \
                     && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= dilatedKH / 2)                                     \
                     && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= dilatedKH / 2),                                    \
                     XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                        \
    }                                                                                                           \
    else                                                                                                        \
    {                                                                                                           \
      if (XI_CNNA_CONV_GET_FLAG_TOPEDGE(param))                                                                 \
      {                                                                                                         \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= dilatedKW / 2)                                      \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= dilatedKW / 2)                                   \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= dilatedKH / 2)                                   \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((dilatedKH / 2) - 1)),                          \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                      \
      }                                                                                                         \
      else                                                                                                      \
      {                                                                                                         \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= dilatedKW / 2)                                      \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= dilatedKW / 2)                                   \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((dilatedKH / 2) - 1))                           \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= dilatedKH / 2),                                  \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                      \
      }                                                                                                         \
    }                                                                                                           \
  }                                                                                                             \
  else                                                                                                          \
  {                                                                                                             \
    if (dilatedKH % 2 != 0)                                                                                     \
    {                                                                                                           \
      if (XI_CNNA_CONV_GET_FLAG_LEFTEDGE(param))                                                                \
      {                                                                                                         \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= dilatedKW / 2)                                      \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (dilatedKW / 2) - 1)                             \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= dilatedKH / 2)                                   \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= dilatedKH / 2),                                  \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                      \
      }                                                                                                         \
      else                                                                                                      \
      {                                                                                                         \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((dilatedKW / 2) - 1))                              \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= dilatedKW / 2)                                   \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= dilatedKH / 2)                                   \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= dilatedKH / 2),                                  \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                      \
      }                                                                                                         \
    }                                                                                                           \
    else                                                                                                        \
    {                                                                                                           \
      if (XI_CNNA_CONV_GET_FLAG_TOPEDGE(param))                                                                 \
      {                                                                                                         \
        if (XI_CNNA_CONV_GET_FLAG_LEFTEDGE(param))                                                              \
        {                                                                                                       \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (dilatedKW / 2) &&                                \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((dilatedKW / 2) - 1) &&                          \
                          XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (dilatedKH / 2) &&                                \
                          XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((dilatedKH / 2) - 1)),                           \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                    \
        }                                                                                                       \
        else                                                                                                    \
        {                                                                                                       \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((dilatedKW / 2) - 1) &&                          \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (dilatedKW / 2) &&                                \
                          XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (dilatedKH / 2) &&                                \
                          XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((dilatedKH / 2) - 1)),                           \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                    \
        }                                                                                                       \
      }                                                                                                         \
      else                                                                                                      \
      {                                                                                                         \
        if (XI_CNNA_CONV_GET_FLAG_LEFTEDGE(param))                                                              \
        {                                                                                                       \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (dilatedKW / 2) &&                                \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((dilatedKW / 2) - 1) &&                          \
                          XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((dilatedKH / 2) - 1) &&                          \
                          XI_TILE3D_GET_DIM3_EDGE2(inTile) >= (dilatedKH / 2)),                                 \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                    \
        }                                                                                                       \
        else                                                                                                    \
        {                                                                                                       \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((dilatedKW / 2) - 1) &&                          \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (dilatedKW / 2) &&                                \
                          XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((dilatedKH / 2) - 1) &&                          \
                          XI_TILE3D_GET_DIM3_EDGE2(inTile) >= (dilatedKH / 2)),                                 \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                    \
        }                                                                                                       \
      }                                                                                                         \
    }                                                                                                           \
  }

#define XI_CHECK_CONSISTENCYA_MOD_DWH(inT, coeffT, biasArr, outT, param)                                                                          \
  uint16_t dilatedKW_MOD = (uint16_t) (XI_CNNA_CONV_GET_DILATIONX(param) * (XI_TILE4D_GET_DIM3(coeffT) - 1) + 1);                                  \
  uint16_t dilatedKH_MOD = (uint16_t) (XI_CNNA_CONV_GET_DILATIONY(param) * (XI_TILE4D_GET_DIM4(coeffT) - 1) + 1);                                  \
  XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(inT) == XI_TILE4D_GET_DIM2(coeffT), XI_ERR_DATASIZE,                                                          \
                 "Number of Input Channels not equal to the number of channels in the Kernel.");                                                  \
  XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(outT) == XI_TILE4D_GET_DIM1(coeffT), XI_ERR_DATASIZE,                                                         \
                 "Number of Output Channels not equal to the number of Kernels.");                                                                \
  if (dilatedKW_MOD % 2 != 0)                                                                                                                     \
  {                                                                                                                                               \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM2(inT) + (dilatedKW_MOD >> 1)                                                 \
                                                   + (dilatedKW_MOD >> 1) - dilatedKW_MOD) >> (XI_CNNA_CONV_GET_STRIDEX(param) >> 1)) + 1)),       \
                   XI_ERR_DATASIZE, "Output Width is invalid.");                                                                                  \
  }                                                                                                                                               \
  else                                                                                                                                            \
  {                                                                                                                                               \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM2(inT) + (dilatedKW_MOD >> 1)                                                 \
                                                   + ((dilatedKW_MOD >> 1) - 1) - dilatedKW_MOD) >> (XI_CNNA_CONV_GET_STRIDEX(param) >> 1)) + 1)), \
                   XI_ERR_DATASIZE, "Output Width is invalid.");                                                                                  \
  }                                                                                                                                               \
  if (dilatedKH_MOD % 2 != 0)                                                                                                                     \
  {                                                                                                                                               \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3(outT) <= (((XI_TILE3D_GET_DIM3(inT) + (dilatedKH_MOD >> 1)                                                 \
                                                   + (dilatedKH_MOD >> 1) - dilatedKH_MOD) >> (XI_CNNA_CONV_GET_STRIDEY(param) >> 1)) + 1)),       \
                   XI_ERR_DATASIZE, "Output Height is invalid.");                                                                                 \
  }                                                                                                                                               \
  else                                                                                                                                            \
  {                                                                                                                                               \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3(outT) <= (((XI_TILE3D_GET_DIM3(inT) + (dilatedKH_MOD >> 1)                                                 \
                                                   + ((dilatedKH_MOD >> 1) - 1) - dilatedKH_MOD) >> (XI_CNNA_CONV_GET_STRIDEY(param) >> 1)) + 1)), \
                   XI_ERR_DATASIZE, "Output Height is invalid.");                                                                                 \
  }                                                                                                                                               \
  XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(biasArr) >= XI_TILE4D_GET_DIM1(coeffT), XI_ERR_DATASIZE,                                                      \
                 "Width of Bias Array is less than number of the Kernel.");                                                                       \
  XI_CHECK_ERROR(XI_ARRAY_GET_HEIGHT(biasArray) > 0, XI_ERR_DATASIZE,                                                                             \
                 "Height of Bias Array should be greater than zero.");

#define XI_CHECK_EDGESA_DEPTHWISE_MOD_ID16WH(inTile, coeffT, param)                                         \
  int32_t kWidthMOD, kHeightMOD;                                                                           \
  uint16_t dilationX = XI_CNNA_CONV_GET_DILATIONX(param);                                                   \
  uint16_t dilationY = XI_CNNA_CONV_GET_DILATIONY(param);                                                   \
  kWidthMOD  = dilationX * (XI_TILE3D_GET_DIM2(coeffT) - 1) + 1;                                           \
  kHeightMOD = dilationY * (XI_TILE3D_GET_DIM3(coeffT) - 1) + 1;                                           \
  if (kWidthMOD % 2 != 0)                                                                                  \
  {                                                                                                        \
    if (kHeightMOD % 2 != 0)                                                                               \
    {                                                                                                      \
      XI_CHECK_ERROR(((XI_TILE3D_GET_DIM1_EDGE1(inTile) >> 4) >= kWidthMOD / 2)                            \
                     && ((XI_TILE3D_GET_DIM1_EDGE2(inTile) >> 4) >= kWidthMOD / 2)                         \
                     && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= kHeightMOD / 2)                               \
                     && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= kHeightMOD / 2),                              \
                     XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                   \
    }                                                                                                      \
    else                                                                                                   \
    {                                                                                                      \
      if (XI_CNNA_CONV_GET_FLAG_TOPEDGE(param))                                                             \
      {                                                                                                    \
        XI_CHECK_ERROR(((XI_TILE3D_GET_DIM1_EDGE1(inTile) >> 4) >= kWidthMOD / 2)                          \
                       && ((XI_TILE3D_GET_DIM1_EDGE2(inTile) >> 4) >= kWidthMOD / 2)                       \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= kHeightMOD / 2)                             \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((kHeightMOD / 2) - 1)),                    \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                 \
      }                                                                                                    \
      else                                                                                                 \
      {                                                                                                    \
        XI_CHECK_ERROR(((XI_TILE3D_GET_DIM1_EDGE1(inTile) >> 4) >= kWidthMOD / 2)                          \
                       && ((XI_TILE3D_GET_DIM1_EDGE2(inTile) >> 4) >= kWidthMOD / 2)                       \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((kHeightMOD / 2) - 1))                     \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= kHeightMOD / 2),                            \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                 \
      }                                                                                                    \
    }                                                                                                      \
  }                                                                                                        \
  else                                                                                                     \
  {                                                                                                        \
    if (kHeightMOD % 2 != 0)                                                                               \
    {                                                                                                      \
      if (XI_CNNA_CONV_GET_FLAG_LEFTEDGE(param))                                                            \
      {                                                                                                    \
        XI_CHECK_ERROR(((XI_TILE3D_GET_DIM1_EDGE1(inTile) >> 4) >= kWidthMOD / 2)                          \
                       && ((XI_TILE3D_GET_DIM1_EDGE2(inTile) >> 4) >= (kWidthMOD / 2) - 1)                 \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= kHeightMOD / 2)                             \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= kHeightMOD / 2),                            \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                 \
      }                                                                                                    \
      else                                                                                                 \
      {                                                                                                    \
        XI_CHECK_ERROR(((XI_TILE3D_GET_DIM1_EDGE1(inTile) >> 4) >= ((kWidthMOD / 2) - 1))                  \
                       && ((XI_TILE3D_GET_DIM1_EDGE2(inTile) >> 4) >= kWidthMOD / 2)                       \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= kHeightMOD / 2)                             \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= kHeightMOD / 2),                            \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");                 \
      }                                                                                                    \
    }                                                                                                      \
    else                                                                                                   \
    {                                                                                                      \
      if (XI_CNNA_CONV_GET_FLAG_TOPEDGE(param))                                                             \
      {                                                                                                    \
        if (XI_CNNA_CONV_GET_FLAG_LEFTEDGE(param))                                                          \
        {                                                                                                  \
          XI_CHECK_ERROR((((XI_TILE3D_GET_DIM1_EDGE1(inTile) >> 4) >= (kWidthMOD / 2)) &&                  \
                          ((XI_TILE3D_GET_DIM1_EDGE2(inTile) >> 4) >= ((kWidthMOD / 2) - 1)) &&            \
                          (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (kHeightMOD / 2)) &&                        \
                          (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((kHeightMOD / 2) - 1))),                   \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");               \
        }                                                                                                  \
        else                                                                                               \
        {                                                                                                  \
          XI_CHECK_ERROR((((XI_TILE3D_GET_DIM1_EDGE1(inTile) >> 4) >= ((kWidthMOD / 2) - 1)) &&            \
                          ((XI_TILE3D_GET_DIM1_EDGE2(inTile) >> 4) >= (kWidthMOD / 2)) &&                  \
                          (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (kHeightMOD / 2)) &&                        \
                          (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((kHeightMOD / 2) - 1))),                   \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");               \
        }                                                                                                  \
      }                                                                                                    \
      else                                                                                                 \
      {                                                                                                    \
        if (XI_CNNA_CONV_GET_FLAG_LEFTEDGE(param))                                                          \
        {                                                                                                  \
          XI_CHECK_ERROR((((XI_TILE3D_GET_DIM1_EDGE1(inTile) >> 4) >= (kWidthMOD / 2)) &&                  \
                          ((XI_TILE3D_GET_DIM1_EDGE2(inTile) >> 4)>= ((kWidthMOD / 2) - 1)) &&             \
                          (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((kHeightMOD / 2) - 1)) &&                  \
                          (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= (kHeightMOD / 2))),                         \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");               \
        }                                                                                                  \
        else                                                                                               \
        {                                                                                                  \
          XI_CHECK_ERROR((((XI_TILE3D_GET_DIM1_EDGE1(inTile) >> 4) >= ((kWidthMOD / 2) - 1)) &&            \
                          ((XI_TILE3D_GET_DIM1_EDGE2(inTile) >> 4) >= (kWidthMOD / 2))&&                   \
                          (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((kHeightMOD / 2) - 1)) &&                  \
                          (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= (kHeightMOD / 2))),                         \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");               \
        }                                                                                                  \
      }                                                                                                    \
    }                                                                                                      \
  }

#define XI_CHECK_EDGESA_DEPTHWISE_MOD_DWH(inTile, coeffTile, param)                          \
  int32_t kW = XI_TILE3D_GET_DIM2(coeffTile);                                               \
  int32_t kH = XI_TILE3D_GET_DIM3(coeffTile);                                               \
  if (kW % 2 != 0)                                                                           \
  {                                                                                          \
    if (kH % 2 != 0)                                                                         \
    {                                                                                        \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= kW / 2)                            \
                     && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= kW / 2)                         \
                     && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= kH / 2)                         \
                     && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= kH / 2),                        \
                     XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");     \
    }                                                                                        \
    else                                                                                     \
    {                                                                                        \
      if (XI_CNNA_CONV_GET_FLAG_TOPEDGE(param))                                              \
      {                                                                                      \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= kW / 2)                          \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= kW / 2)                       \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= kH / 2)                       \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((kH / 2) - 1)),              \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");   \
      }                                                                                      \
      else                                                                                   \
      {                                                                                      \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= kW / 2)                          \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= kW / 2)                       \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((kH / 2) - 1))               \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= kH / 2),                      \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");   \
      }                                                                                      \
    }                                                                                        \
  }                                                                                          \
  else                                                                                       \
  {                                                                                          \
    if (kH % 2 != 0)                                                                         \
    {                                                                                        \
      if (XI_CNNA_CONV_GET_FLAG_LEFTEDGE(param))                                             \
      {                                                                                      \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= kW / 2)                          \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (kW / 2) - 1)                 \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= kH / 2)                       \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= kH / 2),                      \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");   \
      }                                                                                      \
      else                                                                                   \
      {                                                                                      \
        XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((kW / 2) - 1))                  \
                       && (XI_TILE3D_GET_DIM2_EDGE2(inTile) >= kW / 2)                       \
                       && (XI_TILE3D_GET_DIM3_EDGE1(inTile) >= kH / 2)                       \
                       && (XI_TILE3D_GET_DIM3_EDGE2(inTile) >= kH / 2),                      \
                       XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data");   \
      }                                                                                      \
    }                                                                                        \
    else                                                                                     \
    {                                                                                        \
      if (XI_CNNA_CONV_GET_FLAG_TOPEDGE(param))                                              \
      {                                                                                      \
        if (XI_CNNA_CONV_GET_FLAG_LEFTEDGE(param))                                           \
        {                                                                                    \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (kW / 2) &&                    \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((kW / 2) - 1) &&              \
                          XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (kH / 2) &&                    \
                          XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((kH / 2) - 1)),               \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data"); \
        }                                                                                    \
        else                                                                                 \
        {                                                                                    \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((kW / 2) - 1) &&              \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (kW / 2) &&                    \
                          XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (kH / 2) &&                    \
                          XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((kH / 2) - 1)),               \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data"); \
        }                                                                                    \
      }                                                                                      \
      else                                                                                   \
      {                                                                                      \
        if (XI_CNNA_CONV_GET_FLAG_LEFTEDGE(param))                                           \
        {                                                                                    \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (kW / 2) &&                    \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((kW / 2) - 1) &&              \
                          XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((kH / 2) - 1) &&              \
                          XI_TILE3D_GET_DIM3_EDGE2(inTile) >= (kH / 2)),                     \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data"); \
        }                                                                                    \
        else                                                                                 \
        {                                                                                    \
          XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((kW / 2) - 1) &&              \
                          XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (kW / 2) &&                    \
                          XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((kH / 2) - 1) &&              \
                          XI_TILE3D_GET_DIM3_EDGE2(inTile) >= (kH / 2)),                     \
                         XI_ERR_EDGE, "The input Tile doesn't have the required Edge Data"); \
        }                                                                                    \
      }                                                                                      \
    }                                                                                        \
  }

#define XI_CHECK_CONSISTENCYA_DEPTHWISE_MOD_DWH(inT, coeffT, biasArr, outT, param)                                                  \
  int32_t KW_MOD = XI_TILE3D_GET_DIM2(coeffT);                                                                                     \
  int32_t KH_MOD = XI_TILE3D_GET_DIM3(coeffT);                                                                                     \
  XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(inT) == XI_TILE3D_GET_DIM1(coeffT), XI_ERR_DATASIZE,                                            \
                 "Number of Input Channels not equal to the number of channels in the Kernel.");                                    \
  XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(outT) == XI_TILE3D_GET_DIM1(coeffT), XI_ERR_DATASIZE,                                           \
                 "Number of Output Channels not equal to the number of channels in the Kernel.");                                   \
  if (KW_MOD % 2 != 0)                                                                                                              \
  {                                                                                                                                 \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM2(inT) + (KW_MOD >> 1)                                          \
                                                   + (KW_MOD >> 1) - KW_MOD) >> (XI_CNNA_CONV_GET_STRIDEX(param) >> 1)) + 1)),       \
                   XI_ERR_DATASIZE, "Output Width is invalid.");                                                                    \
  }                                                                                                                                 \
  else                                                                                                                              \
  {                                                                                                                                 \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM2(inT) + (KW_MOD >> 1)                                          \
                                                   + ((KW_MOD >> 1) - 1) - KW_MOD) >> (XI_CNNA_CONV_GET_STRIDEX(param) >> 1)) + 1)), \
                   XI_ERR_DATASIZE, "Output Width is invalid.");                                                                    \
  }                                                                                                                                 \
  if (KH_MOD % 2 != 0)                                                                                                              \
  {                                                                                                                                 \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3(outT) <= (((XI_TILE3D_GET_DIM3(inT) + (KH_MOD >> 1)                                          \
                                                   + (KH_MOD >> 1) - KH_MOD) >> (XI_CNNA_CONV_GET_STRIDEY(param) >> 1)) + 1)),       \
                   XI_ERR_DATASIZE, "Output Height is invalid.");                                                                   \
  }                                                                                                                                 \
  else                                                                                                                              \
  {                                                                                                                                 \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3(outT) <= (((XI_TILE3D_GET_DIM3(inT) + (KH_MOD >> 1)                                          \
                                                   + ((KH_MOD >> 1) - 1) - KH_MOD) >> (XI_CNNA_CONV_GET_STRIDEY(param) >> 1)) + 1)), \
                   XI_ERR_DATASIZE, "Output Height is invalid.");                                                                   \
  }                                                                                                                                 \
  XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(biasArr) >= XI_TILE3D_GET_DIM1(coeffT), XI_ERR_DATASIZE,                                        \
                 "Width of Bias Array is less than number of channels in the Kernel.");                                             \
  XI_CHECK_ERROR(XI_ARRAY_GET_HEIGHT(biasArray) > 0, XI_ERR_DATASIZE,                                                               \
                 "Height of Bias Array should be greater than zero.");

#define XI_CHECK_CONSISTENCYA_DEPTHWISE_MOD_ID16WH(inTile, coeffT, outTile, param)                                       \
{                                                                                                                       \
  uint16_t dilationX = XI_CNNA_CONV_GET_DILATIONX(param);                                                                \
  uint16_t dilationY = XI_CNNA_CONV_GET_DILATIONY(param);                                                                \
  int32_t dilatedkWidth  = dilationX * (XI_TILE3D_GET_DIM2(coeffT) - 1) + 1;                                            \
  int32_t dilatedkHeight = dilationY * (XI_TILE3D_GET_DIM3(coeffT) - 1) + 1;                                            \
    XI_CHECK_ERROR(((XI_TILE3D_GET_DIM2(inTile) << 4) == (XI_TILE3D_GET_DIM2(outTile) << 4)),                           \
                    XI_ERR_DATASIZE, "Number of input and output channel should be equal.");                            \
  if (dilatedkWidth % 2 != 0)                                                                                           \
  {                                                                                                                     \
    XI_CHECK_ERROR(((XI_TILE3D_GET_DIM1(outTile) >> 4) <= ((((XI_TILE3D_GET_DIM1(inTile) >> 4) +                        \
               (dilatedkWidth >> 1) + (dilatedkWidth >> 1) -  dilatedkWidth) / (XI_CNNA_CONV_GET_STRIDEX(param))) + 1)), \
                XI_ERR_DATASIZE, "Input and Output tile widths are inconsistent.");                                     \
  }                                                                                                                     \
  else                                                                                                                  \
  {                                                                                                                     \
    XI_CHECK_ERROR(((XI_TILE3D_GET_DIM1(outTile) >> 4) <= ((((XI_TILE3D_GET_DIM1(inTile) >> 4) +                        \
                      (dilatedkWidth >> 1)+ ((dilatedkWidth >> 1) - 1) -                                                \
                       dilatedkWidth) / (XI_CNNA_CONV_GET_STRIDEX(param))) + 1)),                                        \
                       XI_ERR_DATASIZE, "Input and Output tile widths are inconsistent.");                              \
  }                                                                                                                     \
  if (dilatedkHeight % 2 != 0)                                                                                          \
  {                                                                                                                     \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3(outTile) <= (((XI_TILE3D_GET_DIM3(inTile) + (dilatedkHeight >> 1) +              \
                          (dilatedkHeight >> 1) - dilatedkHeight) / (XI_CNNA_CONV_GET_STRIDEY(param))) + 1)),            \
                           XI_ERR_DATASIZE, "Input and Output tile heights are inconsistent.");                         \
  }                                                                                                                     \
  else                                                                                                                  \
  {                                                                                                                     \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3(outTile) <= (((XI_TILE3D_GET_DIM3(inTile) + (dilatedkHeight >> 1) +              \
                          ((dilatedkHeight >> 1) - 1)  - dilatedkHeight) / (XI_CNNA_CONV_GET_STRIDEY(param))) + 1)),     \
                           XI_ERR_DATASIZE, "Input and Output tile heights are inconsistent.");                         \
  }                                                                                                                     \
}

#define XI_CHECK_DIM_CONSISTENCY_PERMUTEA3D(inTile, outTile, pparams)                                                                                                                                           \
  if ((XI_CNN_PERMUTEA3D_GET_ORDER1(pparams) == 0) && (XI_CNN_PERMUTEA3D_GET_ORDER2(pparams) == 1) && (XI_CNN_PERMUTEA3D_GET_ORDER3(pparams) == 2))                                                             \
  {                                                                                                                                                                                                             \
    XI_CHECK_ERROR(((XI_TILE3D_GET_DIM1(outTile) == XI_TILE3D_GET_DIM1(inTile)) && (XI_TILE3D_GET_DIM2(outTile) == XI_TILE3D_GET_DIM2(inTile)) && (XI_TILE3D_GET_DIM3(outTile) == XI_TILE3D_GET_DIM3(inTile))), \
                   XI_ERR_DATASIZE, "Input and Output tile dimensions are inconsistent");                                                                                                                       \
  }                                                                                                                                                                                                             \
  if ((XI_CNN_PERMUTEA3D_GET_ORDER1(pparams) == 0) && (XI_CNN_PERMUTEA3D_GET_ORDER2(pparams) == 2) && (XI_CNN_PERMUTEA3D_GET_ORDER3(pparams) == 1))                                                             \
  {                                                                                                                                                                                                             \
    XI_CHECK_ERROR(((XI_TILE3D_GET_DIM1(outTile) == XI_TILE3D_GET_DIM1(inTile)) && (XI_TILE3D_GET_DIM2(outTile) == XI_TILE3D_GET_DIM3(inTile)) && (XI_TILE3D_GET_DIM3(outTile) == XI_TILE3D_GET_DIM2(inTile))), \
                   XI_ERR_DATASIZE, "Input and Output tile dimensions are inconsistent");                                                                                                                       \
  }                                                                                                                                                                                                             \
  if ((XI_CNN_PERMUTEA3D_GET_ORDER1(pparams) == 1) && (XI_CNN_PERMUTEA3D_GET_ORDER2(pparams) == 0) && (XI_CNN_PERMUTEA3D_GET_ORDER3(pparams) == 2))                                                             \
  {                                                                                                                                                                                                             \
    XI_CHECK_ERROR(((XI_TILE3D_GET_DIM1(outTile) == XI_TILE3D_GET_DIM2(inTile)) && (XI_TILE3D_GET_DIM2(outTile) == XI_TILE3D_GET_DIM1(inTile)) && (XI_TILE3D_GET_DIM3(outTile) == XI_TILE3D_GET_DIM3(inTile))), \
                   XI_ERR_DATASIZE, "Input and Output tile dimensions are inconsistent");                                                                                                                       \
  }                                                                                                                                                                                                             \
  if ((XI_CNN_PERMUTEA3D_GET_ORDER1(pparams) == 1) && (XI_CNN_PERMUTEA3D_GET_ORDER2(pparams) == 2) && (XI_CNN_PERMUTEA3D_GET_ORDER3(pparams) == 0))                                                             \
  {                                                                                                                                                                                                             \
    XI_CHECK_ERROR(((XI_TILE3D_GET_DIM1(outTile) == XI_TILE3D_GET_DIM2(inTile)) && (XI_TILE3D_GET_DIM2(outTile) == XI_TILE3D_GET_DIM3(inTile)) && (XI_TILE3D_GET_DIM3(outTile) == XI_TILE3D_GET_DIM1(inTile))), \
                   XI_ERR_DATASIZE, "Input and Output tile dimensions are inconsistent");                                                                                                                       \
  }                                                                                                                                                                                                             \
  if ((XI_CNN_PERMUTEA3D_GET_ORDER1(pparams) == 2) && (XI_CNN_PERMUTEA3D_GET_ORDER2(pparams) == 0) && (XI_CNN_PERMUTEA3D_GET_ORDER3(pparams) == 1))                                                             \
  {                                                                                                                                                                                                             \
    XI_CHECK_ERROR(((XI_TILE3D_GET_DIM1(outTile) == XI_TILE3D_GET_DIM3(inTile)) && (XI_TILE3D_GET_DIM2(outTile) == XI_TILE3D_GET_DIM1(inTile)) && (XI_TILE3D_GET_DIM3(outTile) == XI_TILE3D_GET_DIM2(inTile))), \
                   XI_ERR_DATASIZE, "Input and Output tile dimensions are inconsistent");                                                                                                                       \
  }                                                                                                                                                                                                             \
  if ((XI_CNN_PERMUTEA3D_GET_ORDER1(pparams) == 2) && (XI_CNN_PERMUTEA3D_GET_ORDER2(pparams) == 1) && (XI_CNN_PERMUTEA3D_GET_ORDER3(pparams) == 0))                                                             \
  {                                                                                                                                                                                                             \
    XI_CHECK_ERROR(((XI_TILE3D_GET_DIM1(outTile) == XI_TILE3D_GET_DIM3(inTile)) && (XI_TILE3D_GET_DIM2(outTile) == XI_TILE3D_GET_DIM2(inTile)) && (XI_TILE3D_GET_DIM3(outTile) == XI_TILE3D_GET_DIM1(inTile))), \
                   XI_ERR_DATASIZE, "Input and Output tile dimensions are inconsistent");                                                                                                                       \
  }

#define XI_CHECK_INPUT_TILESIZE_PERMUTEA3D_GATHER(inTile, pparams)                                                                                                                                                    \
  if (!(XI_CNN_PERMUTEA3D_GET_ORDER1(pparams) == 0))                                                                                                                                                                  \
  {                                                                                                                                                                                                                   \
    XI_CHECK_ERROR(((((XI_TILE3D_GET_DIM3(inTile) - 1) * XI_TILE3D_GET_DIM2_PITCH(inTile)) + ((XI_TILE3D_GET_DIM2(inTile) - 1) * XI_TILE3D_GET_DIM1_PITCH(inTile)) + (XI_TILE3D_GET_DIM1(inTile) - 1)) <= USHRT_MAX), \
                   XI_ERR_NOTIMPLEMENTED, "Offset for the last element in the active area of input tile is exceeding unsigned 16 bit maximum value, Input Tile size is inconsistent for gather approach");            \
  }

#define XI_CHECK_DIM_CONSISTENCY_PERMUTEA4D(inTile, outTile, pparams)                                                                                \
  if (XI_CNN_PERMUTEA4D_GET_ORDER1(pparams) == 0)                                                                                                    \
  {                                                                                                                                                  \
    XI_CHECK_ERROR(XI_TILE4D_GET_DIM1(outTile) == XI_TILE4D_GET_DIM1(inTile), XI_ERR_DATASIZE, "Input and Output tile dimensions are inconsistent"); \
  }                                                                                                                                                  \
  if (XI_CNN_PERMUTEA4D_GET_ORDER1(pparams) == 1)                                                                                                    \
  {                                                                                                                                                  \
    XI_CHECK_ERROR(XI_TILE4D_GET_DIM1(outTile) == XI_TILE4D_GET_DIM2(inTile), XI_ERR_DATASIZE, "Input and Output tile dimensions are inconsistent"); \
  }                                                                                                                                                  \
  if (XI_CNN_PERMUTEA4D_GET_ORDER1(pparams) == 2)                                                                                                    \
  {                                                                                                                                                  \
    XI_CHECK_ERROR(XI_TILE4D_GET_DIM1(outTile) == XI_TILE4D_GET_DIM3(inTile), XI_ERR_DATASIZE, "Input and Output tile dimensions are inconsistent"); \
  }                                                                                                                                                  \
  if (XI_CNN_PERMUTEA4D_GET_ORDER1(pparams) == 3)                                                                                                    \
  {                                                                                                                                                  \
    XI_CHECK_ERROR(XI_TILE4D_GET_DIM1(outTile) == XI_TILE4D_GET_DIM4(inTile), XI_ERR_DATASIZE, "Input and Output tile dimensions are inconsistent"); \
  }                                                                                                                                                  \
  if (XI_CNN_PERMUTEA4D_GET_ORDER2(pparams) == 0)                                                                                                    \
  {                                                                                                                                                  \
    XI_CHECK_ERROR(XI_TILE4D_GET_DIM2(outTile) == XI_TILE4D_GET_DIM1(inTile), XI_ERR_DATASIZE, "Input and Output tile dimensions are inconsistent"); \
  }                                                                                                                                                  \
  if (XI_CNN_PERMUTEA4D_GET_ORDER2(pparams) == 1)                                                                                                    \
  {                                                                                                                                                  \
    XI_CHECK_ERROR(XI_TILE4D_GET_DIM2(outTile) == XI_TILE4D_GET_DIM2(inTile), XI_ERR_DATASIZE, "Input and Output tile dimensions are inconsistent"); \
  }                                                                                                                                                  \
  if (XI_CNN_PERMUTEA4D_GET_ORDER2(pparams) == 2)                                                                                                    \
  {                                                                                                                                                  \
    XI_CHECK_ERROR(XI_TILE4D_GET_DIM2(outTile) == XI_TILE4D_GET_DIM3(inTile), XI_ERR_DATASIZE, "Input and Output tile dimensions are inconsistent"); \
  }                                                                                                                                                  \
  if (XI_CNN_PERMUTEA4D_GET_ORDER2(pparams) == 3)                                                                                                    \
  {                                                                                                                                                  \
    XI_CHECK_ERROR(XI_TILE4D_GET_DIM2(outTile) == XI_TILE4D_GET_DIM4(inTile), XI_ERR_DATASIZE, "Input and Output tile dimensions are inconsistent"); \
  }                                                                                                                                                  \
  if (XI_CNN_PERMUTEA4D_GET_ORDER3(pparams) == 0)                                                                                                    \
  {                                                                                                                                                  \
    XI_CHECK_ERROR(XI_TILE4D_GET_DIM3(outTile) == XI_TILE4D_GET_DIM1(inTile), XI_ERR_DATASIZE, "Input and Output tile dimensions are inconsistent"); \
  }                                                                                                                                                  \
  if (XI_CNN_PERMUTEA4D_GET_ORDER3(pparams) == 1)                                                                                                    \
  {                                                                                                                                                  \
    XI_CHECK_ERROR(XI_TILE4D_GET_DIM3(outTile) == XI_TILE4D_GET_DIM2(inTile), XI_ERR_DATASIZE, "Input and Output tile dimensions are inconsistent"); \
  }                                                                                                                                                  \
  if (XI_CNN_PERMUTEA4D_GET_ORDER3(pparams) == 2)                                                                                                    \
  {                                                                                                                                                  \
    XI_CHECK_ERROR(XI_TILE4D_GET_DIM3(outTile) == XI_TILE4D_GET_DIM3(inTile), XI_ERR_DATASIZE, "Input and Output tile dimensions are inconsistent"); \
  }                                                                                                                                                  \
  if (XI_CNN_PERMUTEA4D_GET_ORDER3(pparams) == 3)                                                                                                    \
  {                                                                                                                                                  \
    XI_CHECK_ERROR(XI_TILE4D_GET_DIM3(outTile) == XI_TILE4D_GET_DIM4(inTile), XI_ERR_DATASIZE, "Input and Output tile dimensions are inconsistent"); \
  }                                                                                                                                                  \
  if (XI_CNN_PERMUTEA4D_GET_ORDER4(pparams) == 0)                                                                                                    \
  {                                                                                                                                                  \
    XI_CHECK_ERROR(XI_TILE4D_GET_DIM4(outTile) == XI_TILE4D_GET_DIM1(inTile), XI_ERR_DATASIZE, "Input and Output tile dimensions are inconsistent"); \
  }                                                                                                                                                  \
  if (XI_CNN_PERMUTEA4D_GET_ORDER4(pparams) == 1)                                                                                                    \
  {                                                                                                                                                  \
    XI_CHECK_ERROR(XI_TILE4D_GET_DIM4(outTile) == XI_TILE4D_GET_DIM2(inTile), XI_ERR_DATASIZE, "Input and Output tile dimensions are inconsistent"); \
  }                                                                                                                                                  \
  if (XI_CNN_PERMUTEA4D_GET_ORDER4(pparams) == 2)                                                                                                    \
  {                                                                                                                                                  \
    XI_CHECK_ERROR(XI_TILE4D_GET_DIM4(outTile) == XI_TILE4D_GET_DIM3(inTile), XI_ERR_DATASIZE, "Input and Output tile dimensions are inconsistent"); \
  }                                                                                                                                                  \
  if (XI_CNN_PERMUTEA4D_GET_ORDER4(pparams) == 3)                                                                                                    \
  {                                                                                                                                                  \
    XI_CHECK_ERROR(XI_TILE4D_GET_DIM4(outTile) == XI_TILE4D_GET_DIM4(inTile), XI_ERR_DATASIZE, "Input and Output tile dimensions are inconsistent"); \
  }                                                                                                                                                  \

#define XI_CHECK_INPUT_TILESIZE_PERMUTEA4D_GATHER(inTile, pparams)                           \
  if (!(XI_CNN_PERMUTEA4D_GET_ORDER1(pparams) == 0))                                         \
  {                                                                                          \
    XI_CHECK_ERROR(((((XI_TILE4D_GET_DIM4(inTile) - 1) * XI_TILE4D_GET_DIM3_PITCH(inTile)) + \
                     ((XI_TILE4D_GET_DIM3(inTile) - 1) * XI_TILE4D_GET_DIM2_PITCH(inTile)) + \
                     ((XI_TILE4D_GET_DIM2(inTile) - 1) * XI_TILE4D_GET_DIM1_PITCH(inTile)) + \
                     (XI_TILE4D_GET_DIM1(inTile) - 1)) <= USHRT_MAX),                        \
                   XI_ERR_NOTIMPLEMENTED, "Tile size > 64K not supported");                  \
  }

#define XI_CHECK_CONSISTENCY_MINPOOLA3D_DWH(inT, outT, param)                                                                                        \
{                                                                                                                                                    \
  XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(inT) == XI_TILE3D_GET_DIM1(outT),                                                                                \
                 XI_ERR_CHANNEL_INVALID, "Number of input and output channels don't match");                                                         \
  if ((XI_CNN_MINPOOLA_GET_KERNELWIDTH(param) % 2 != 0))                                                                                             \
  {                                                                                                                                                  \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inT) >= (XI_CNN_MINPOOLA_GET_KERNELWIDTH(param) / 2) &&                                                 \
                    XI_TILE3D_GET_DIM2_EDGE2(inT) >= (XI_CNN_MINPOOLA_GET_KERNELWIDTH(param) / 2)),                                                  \
                   XI_ERR_EDGE, "Invalid edge for odd kernel size");                                                                                 \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM2(inT) + (XI_CNN_MINPOOLA_GET_KERNELWIDTH(param) >> 1) +                         \
                                                   (XI_CNN_MINPOOLA_GET_KERNELWIDTH(param) >> 1) - XI_CNN_MINPOOLA_GET_KERNELWIDTH(param)) /         \
                                                   (XI_CNN_MINPOOLA_GET_STRIDEX(param))) + 1)),                                                      \
                   XI_ERR_DATASIZE, "Output Width is invalid.");                                                                                     \
  }                                                                                                                                                  \
  else                                                                                                                                               \
  {                                                                                                                                                  \
    if (XI_CNN_MINPOOLA_GET_LEFTEDGE_FLAG(param))                                                                                                    \
    {                                                                                                                                                \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inT) >= (XI_CNN_MINPOOLA_GET_KERNELWIDTH(param) / 2) &&                                               \
                      XI_TILE3D_GET_DIM2_EDGE2(inT) >= ((XI_CNN_MINPOOLA_GET_KERNELWIDTH(param) / 2) - 1)),                                          \
                     XI_ERR_EDGE, "Invalid edge for even kernel size with left edge flag set");                                                      \
    }                                                                                                                                                \
    else                                                                                                                                             \
    {                                                                                                                                                \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM2_EDGE1(inT) >= ((XI_CNN_MINPOOLA_GET_KERNELWIDTH(param) / 2) - 1) &&                                         \
                      XI_TILE3D_GET_DIM2_EDGE2(inT) >= (XI_CNN_MINPOOLA_GET_KERNELWIDTH(param) / 2)),                                                \
                     XI_ERR_EDGE, "Invalid edge for even kernel size with left edge flag reset");                                                    \
    }                                                                                                                                                \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM2(outT) <= (((XI_TILE3D_GET_DIM2(inT) + ((XI_CNN_MINPOOLA_GET_KERNELWIDTH(param) >> 1) - 1) +                   \
                                                  (XI_CNN_MINPOOLA_GET_KERNELWIDTH(param) >> 1) - XI_CNN_MINPOOLA_GET_KERNELWIDTH(param)) /          \
                                                   (XI_CNN_MINPOOLA_GET_STRIDEX(param))) + 1)),                                                      \
                   XI_ERR_DATASIZE, "Output Width is invalid.");                                                                                     \
  }                                                                                                                                                  \
  if ((XI_CNN_MINPOOLA_GET_KERNELHEIGHT(param) % 2 != 0))                                                                                            \
  {                                                                                                                                                  \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3_EDGE1(inT) >= (XI_CNN_MINPOOLA_GET_KERNELHEIGHT(param) / 2) &&                                                \
                    XI_TILE3D_GET_DIM3_EDGE2(inT) >= (XI_CNN_MINPOOLA_GET_KERNELHEIGHT(param) / 2)),                                                 \
                   XI_ERR_EDGE, "Invalid edge for odd kernel size");                                                                                 \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3(outT) <= (((XI_TILE3D_GET_DIM3(inT) + (XI_CNN_MINPOOLA_GET_KERNELHEIGHT(param) >> 1) +                        \
                                                  (XI_CNN_MINPOOLA_GET_KERNELHEIGHT(param) >> 1) - XI_CNN_MINPOOLA_GET_KERNELHEIGHT(param)) /        \
                                                  (XI_CNN_MINPOOLA_GET_STRIDEY(param))) + 1)),                                                       \
                   XI_ERR_DATASIZE, "Output Height is invalid.");                                                                                    \
  }                                                                                                                                                  \
  else                                                                                                                                               \
  {                                                                                                                                                  \
    if (XI_CNN_MINPOOLA_GET_TOPEDGE_FLAG(param))                                                                                                     \
    {                                                                                                                                                \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM3_EDGE1(inT) >= (XI_CNN_MINPOOLA_GET_KERNELHEIGHT(param) / 2) &&                                              \
                      XI_TILE3D_GET_DIM3_EDGE2(inT) >= ((XI_CNN_MINPOOLA_GET_KERNELHEIGHT(param) / 2) - 1)),                                         \
                     XI_ERR_EDGE, "Invalid edge for even kernel size with top edge flag set");                                                       \
    }                                                                                                                                                \
    else                                                                                                                                             \
    {                                                                                                                                                \
      XI_CHECK_ERROR((XI_TILE3D_GET_DIM3_EDGE1(inT) >= ((XI_CNN_MINPOOLA_GET_KERNELHEIGHT(param) / 2) - 1) &&                                        \
                      XI_TILE3D_GET_DIM3_EDGE2(inT) >= (XI_CNN_MINPOOLA_GET_KERNELHEIGHT(param) / 2)),                                               \
                     XI_ERR_EDGE, "Invalid edge for even kernel size with top edge flag reset");                                                     \
    }                                                                                                                                                \
    XI_CHECK_ERROR((XI_TILE3D_GET_DIM3(outT) <= (((XI_TILE3D_GET_DIM3(inT) + ((XI_CNN_MINPOOLA_GET_KERNELHEIGHT(param) >> 1) - 1) +                  \
                                                    (XI_CNN_MINPOOLA_GET_KERNELHEIGHT(param) >> 1) - XI_CNN_MINPOOLA_GET_KERNELHEIGHT(param)) /      \
                                                   (XI_CNN_MINPOOLA_GET_STRIDEY(param))) + 1)),                                                      \
                   XI_ERR_DATASIZE, "Output Height is invalid.");                                                                                    \
  }                                                                                                                                                  \
}

/**********************  SCALE_SHIFT_CLAMP_ADDZP   ************************/
/* Requantization :                                                       */
/*  output = (ScaleIn *  shifted_min_val)/ScaleOut + ZeroPointOut         */
/*         =  Multiplier * shifted_min_val + ZeroPointOut                 */
/*  where shifted_min_val  = (minVal - zeroPointIn) << left_shift         */
/*        Multiplier is stored as  output_multplier x 2^(-output_shift)   */
/*        output_multiplier- Q31 format                                   */
/*                                                                        */
/*  minVal_shifted = minVal - input_zero_point                            */
/*  B = (minVal_shifted << l_shift * out_multplier ) >> 31                */
/*    = (A * output_multiplier) >> (31- left_shift)                       */
/*  Shift B by out_shift and round towards +ve or -ve infinity            */
/*  C = abs(B) >> out_shift                                               */
/*  D = multiply C with sign bit of B                                     */
/*  result = D + output_zero_point                                        */
/**************************************************************************/

#define SCALE_SHIFT_CLAMP_ADDZP(vecMinVal_shifted, l_shift, out_multiplier, out_shift, out_zero_point, min, max, vecOut0)    \
{                                                                                                                            \
  xb_vecN_2x32v hvecOut0, hvecOut1;                                                                                          \
  xb_vecN_2x32v hvecInp1_0, hvecInp1_1;                                                                                      \
  hvecInp1_0 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0(vecMinVal_shifted, (xb_vecN_2x32v)out_multiplier), (31 - l_shift));      \
  hvecInp1_1 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_1(vecMinVal_shifted, (xb_vecN_2x32v)out_multiplier), (31 - l_shift));      \
  hvecOut0 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, IVP_ABSN_2X32(hvecInp1_0)), out_shift);                     \
  hvecOut1 = IVP_PACKVRN_2X64W(IVP_MULN_2X16X32_0((xb_vecNx16)1, IVP_ABSN_2X32(hvecInp1_1)), out_shift);                     \
  hvecOut0 = IVP_MULSGNN_2X32(hvecInp1_0, hvecOut0);                                                                         \
  hvecOut1 = IVP_MULSGNN_2X32(hvecInp1_1, hvecOut1);                                                                         \
  hvecOut0 = hvecOut0 + (xb_vecN_2x32v)out_zero_point;                                                                       \
  hvecOut1 = hvecOut1 + (xb_vecN_2x32v)out_zero_point;                                                                       \
  hvecOut0 = IVP_MINN_2X32((xb_vecN_2x32v)max, IVP_MAXN_2X32(hvecOut0, (xb_vecN_2x32v)min));                                 \
  hvecOut1 = IVP_MINN_2X32((xb_vecN_2x32v)max, IVP_MAXN_2X32(hvecOut1, (xb_vecN_2x32v)min));                                 \
  vecOut0 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecOut1, hvecOut0, IVP_SELI_16B_INTERLEAVE_1_EVEN));                      \
}

/********************** ABS_SCALE_SHIFT_CLAMP_ADDZP ***********************/
/* Requantization :                                                       */
/*  output = (ScaleIn *  absVal_shifted)/ScaleOut + ZeroPointOut          */
/*         =  Multiplier * absVal_shifted + ZeroPointOut                  */
/*  where absVal_shifted  = ABS(val - zeroPointIn) << left_shift         */
/*        Multiplier is stored as  output_multplier x 2^(-output_shift)   */
/*        output_multiplier- Q31 format                                   */
/*                                                                        */
/*  absVal_shifted = ABS(val - input_zero_point)                          */
/*  B = (absVal_shifted << l_shift * out_multplier ) >> 31                */
/*    = (A * output_multiplier) >> (31- left_shift)                       */
/*  Shift B by out_shift and round towards +ve (B will be always positive)*/
/*  C = B >> out_shift                                                    */
/*  result = C + output_zero_point                                        */
/**************************************************************************/

#define ABS_SCALE_SHIFT_CLAMP_ADDZP(vecVal_shifted, l_shift, out_multiplier, out_shift, out_zero_point, min, max, vecOut0)   \
{                                                                                                                            \
  xb_vecN_2x32v hvecOut0, hvecOut1;                                                                                          \
  xb_vecN_2x32v hvecInp1_0, hvecInp1_1;                                                                                      \
  xb_vecNx16 vecAbs = IVP_ABSNX16(vecVal_shifted);                                                                           \
  hvecInp1_0 = IVP_PACKVRN_2X64W(IVP_MULUUN_2X16X32_0(vecAbs, (xb_vecN_2x32Uv)out_multiplier), (31 - l_shift));               \
  hvecInp1_1 = IVP_PACKVRN_2X64W(IVP_MULUUN_2X16X32_1(vecAbs, (xb_vecN_2x32Uv)out_multiplier), (31 - l_shift));               \
  hvecOut0 = IVP_PACKVRN_2X64W(IVP_MULUUN_2X16X32_0((xb_vecNx16U)1, hvecInp1_0), out_shift);                                  \
  hvecOut1 = IVP_PACKVRN_2X64W(IVP_MULUUN_2X16X32_0((xb_vecNx16U)1, hvecInp1_1), out_shift);                                  \
  hvecOut0 = IVP_ADDN_2X32(hvecOut0, (xb_vecN_2x32v)out_zero_point);                                                         \
  hvecOut1 = IVP_ADDN_2X32(hvecOut1, (xb_vecN_2x32v)out_zero_point);                                                         \
  hvecOut0 = IVP_MINN_2X32((xb_vecN_2x32v)max, IVP_MAXN_2X32(hvecOut0, (xb_vecN_2x32v)min));                                 \
  hvecOut1 = IVP_MINN_2X32((xb_vecN_2x32v)max, IVP_MAXN_2X32(hvecOut1, (xb_vecN_2x32v)min));                                 \
  vecOut0 = IVP_MOVNX16_FROMN_2X32(IVP_SELN_2X32I(hvecOut1, hvecOut0, IVP_SELI_16B_INTERLEAVE_1_EVEN));                      \
}

#define XI_CHECK_TILES3D_CHECK_EDGES_DEQUANT(inTile, outTile)                                                                    \
{                                                                                                                                \
  if(XI_TILE3D_GET_DATA_PTR(inTile) == XI_TILE3D_GET_DATA_PTR(outTile))                                                          \
  {                                                                                                                              \
    XI_CHECK_ERROR(((XI_TILE3D_GET_DIM1_EDGE1(inTile) + XI_TILE3D_GET_DIM1_EDGE2(inTile)) <=                                     \
      (2*(XI_TILE3D_GET_DIM1_PITCH(outTile) - XI_TILE3D_GET_DIM1(outTile)))), XI_ERR_BADARG,                                     \
      "Output and Input tile edges constraints have not been met along dimension 1");                                            \
    XI_CHECK_ERROR(((XI_TILE3D_GET_DIM1_PITCH(inTile)*(XI_TILE3D_GET_DIM2_EDGE1(inTile) + XI_TILE3D_GET_DIM2_EDGE2(inTile))) <=  \
      (2*(XI_TILE3D_GET_DIM2_PITCH(outTile) - (XI_TILE3D_GET_DIM1_PITCH(outTile) * XI_TILE3D_GET_DIM2(outTile) )))),             \
         XI_ERR_BADARG, "Output and Input tile edges constraints have not been met  along dimension 2");                         \
    XI_CHECK_ERROR(((XI_TILE3D_GET_DIM3_EDGE1(inTile) + XI_TILE3D_GET_DIM3_EDGE2(inTile)) <=                                     \
      (2*(XI_TILE3D_GET_DIM3_EDGE1(outTile) + XI_TILE3D_GET_DIM3_EDGE2(outTile)))), XI_ERR_BADARG,                               \
      "Output and Input tile edges constraints have not been met  along dimension 3");                                           \
    XI_CHECK_ERROR(((size_t)(XI_TILE_GET_BUFF_PTR(outTile)) <= ((size_t)(XI_TILE_GET_BUFF_PTR(inTile)))), XI_ERR_BADARG,         \
       "Input tile buffer pointer should be greater than or equal to output tile buffer pointer");                               \
  }                                                                                                                              \
}

#define XI_CHECK_TILES4D_CHECK_EDGES_DEQUANT(inTile, outTile)                                                                       \
{                                                                                                                                   \
  if(XI_TILE4D_GET_DATA_PTR(inTile) == XI_TILE4D_GET_DATA_PTR(outTile))                                                             \
  {                                                                                                                                 \
    XI_CHECK_ERROR(((XI_TILE4D_GET_DIM1_EDGE1(inTile) + XI_TILE4D_GET_DIM1_EDGE2(inTile)) <=                                        \
      (2 * (XI_TILE4D_GET_DIM1_PITCH(outTile) - XI_TILE4D_GET_DIM1(outTile)))), XI_ERR_BADARG,                                      \
      "Output and Input tile edges constraints have not been met along dimension 1");                                               \
    XI_CHECK_ERROR(((XI_TILE4D_GET_DIM1_PITCH(inTile)*(XI_TILE4D_GET_DIM2_EDGE1(inTile) + XI_TILE4D_GET_DIM2_EDGE2(inTile))) <=     \
      (2*(XI_TILE4D_GET_DIM2_PITCH(outTile) - (XI_TILE3D_GET_DIM1_PITCH(outTile) * XI_TILE4D_GET_DIM2(outTile) )))),                \
         XI_ERR_BADARG, "Output and Input tile edges constraints have not been met  along dimension 2");                            \
    XI_CHECK_ERROR(((XI_TILE4D_GET_DIM2_PITCH(inTile)*(XI_TILE4D_GET_DIM3_EDGE1(inTile) + XI_TILE4D_GET_DIM3_EDGE2(inTile))) <=     \
    (2*(XI_TILE4D_GET_DIM3_PITCH(outTile)-(XI_TILE3D_GET_DIM2_PITCH(outTile)*XI_TILE4D_GET_DIM3(outTile))))),                       \
      XI_ERR_BADARG, "Output and Input tile edges constraints have not been met  along dimension 3");                               \
    XI_CHECK_ERROR(((size_t)(XI_TILE4D_GET_BUFF_PTR(outTile)) <= ((size_t)(XI_TILE4D_GET_BUFF_PTR(inTile)))), XI_ERR_BADARG,        \
       "Input tile buffer pointer should be greater than or equal to output tile buffer pointer");                                  \
  }                                                                                                                                 \
}
#if XI_ERROR_LEVEL != XI_ERROR_LEVEL_NO_ERROR
#define  XI_CHECK_NUMGROUP_CONSISTENCY_SHUFFLEA4D(outTile, params)                                                             \
{                                                                                                                              \
  int32_t dimArray[4] = { XI_TILE4D_GET_DIM1(outTile), XI_TILE4D_GET_DIM2(outTile), XI_TILE4D_GET_DIM3(outTile),               \
                          XI_TILE4D_GET_DIM4(outTile) };                                                                       \
  uint32_t groupsIn = XI_CNN_SHUFFLE_GET_INTERLEAVEGROUPS(params);                                                             \
  XI_CHECK_ERROR((dimArray[XI_CNN_SHUFFLEA_GET_CONFIG(shuffParams) - 1] % groupsIn == 0 ), XI_ERR_DATASIZE,                    \
                   "Length of dimension specified by the config parameter should be a multiple of input groups");              \
}
#else
#define XI_CHECK_NUMGROUP_CONSISTENCY_SHUFFLEA4D(outTile, params)
#endif


#define XI_CHECK_TILES3D_CHECK_EDGES_QUANT(inTile, outTile)                                                                          \
{                                                                                                                                    \
  if(XI_TILE3D_GET_DATA_PTR(inTile) == XI_TILE3D_GET_DATA_PTR(outTile))                                                              \
  {                                                                                                                                  \
    XI_CHECK_ERROR(((XI_TILE3D_GET_DIM1_EDGE1(outTile) + XI_TILE3D_GET_DIM1_EDGE2(outTile)) <=                                       \
      (2 * (XI_TILE3D_GET_DIM1_PITCH(inTile) - XI_TILE3D_GET_DIM1(inTile)))), XI_ERR_BADARG,                                         \
      "Output and Input tile edges constraints have not been met along dimension 1");                                                \
    XI_CHECK_ERROR(((XI_TILE3D_GET_DIM1_PITCH(outTile)*(XI_TILE3D_GET_DIM2_EDGE1(outTile) + XI_TILE3D_GET_DIM2_EDGE2(outTile))) <=   \
      (2*(XI_TILE3D_GET_DIM2_PITCH(inTile) - (XI_TILE3D_GET_DIM1_PITCH(inTile)* XI_TILE3D_GET_DIM2(inTile))))),                      \
       XI_ERR_BADARG, "Output and Input tile edges constraints have not been met  along dimension 2");                               \
    XI_CHECK_ERROR(((XI_TILE3D_GET_DIM3_EDGE1(outTile) + XI_TILE3D_GET_DIM3_EDGE2(outTile)) <=                                       \
      (2*(XI_TILE3D_GET_DIM3_EDGE1(inTile) + XI_TILE3D_GET_DIM3_EDGE2(inTile)))), XI_ERR_BADARG,                                     \
      "Output and Input tile edges constraints have not been met  along dimension 3");                                               \
    XI_CHECK_ERROR(((size_t)(XI_TILE_GET_BUFF_PTR(inTile)) <= ((size_t)(XI_TILE_GET_BUFF_PTR(outTile)))), XI_ERR_BADARG,             \
       "Output tile buffer pointer should be greater than or equal to input tile buffer pointer");                                   \
  }                                                                                                                                  \
}
#define XI_CHECK_TILES4D_CHECK_EDGES_QUANT(inTile, outTile)                                                                            \
{                                                                                                                                      \
  if(XI_TILE4D_GET_DATA_PTR(inTile) == XI_TILE4D_GET_DATA_PTR(outTile))                                                                \
  {                                                                                                                                    \
    XI_CHECK_ERROR(((XI_TILE4D_GET_DIM1_EDGE1(outTile) + XI_TILE4D_GET_DIM1_EDGE2(outTile)) <=                                         \
      (2 * (XI_TILE4D_GET_DIM1_PITCH(inTile) - XI_TILE4D_GET_DIM1(inTile)))), XI_ERR_BADARG,                                           \
      "Output and Input tile edges constraints have not been met along dimension 1");                                                  \
    XI_CHECK_ERROR(((XI_TILE4D_GET_DIM1_PITCH(outTile)*(XI_TILE4D_GET_DIM2_EDGE1(outTile) + XI_TILE4D_GET_DIM2_EDGE2(outTile))) <=     \
      (2*(XI_TILE4D_GET_DIM2_PITCH(inTile) - (XI_TILE3D_GET_DIM1_PITCH(inTile) * XI_TILE4D_GET_DIM2(inTile) )))),                      \
         XI_ERR_BADARG, "Output and Input tile edges constraints have not been met  along dimension 2");                               \
    XI_CHECK_ERROR(((XI_TILE4D_GET_DIM2_PITCH(outTile)*(XI_TILE4D_GET_DIM3_EDGE1(outTile) + XI_TILE4D_GET_DIM3_EDGE2(outTile))) <=     \
    (2*(XI_TILE4D_GET_DIM3_PITCH(inTile)-(XI_TILE3D_GET_DIM2_PITCH(inTile)*XI_TILE4D_GET_DIM3(inTile))))),                             \
      XI_ERR_BADARG, "Output and Input tile edges constraints have not been met  along dimension 3");                                  \
    XI_CHECK_ERROR(((size_t)(XI_TILE4D_GET_BUFF_PTR(inTile)) <= ((size_t)(XI_TILE4D_GET_BUFF_PTR(outTile)))), XI_ERR_BADARG,           \
       "Output tile buffer pointer should be greater than or equal to input tile buffer pointer");                                     \
  }                                                                                                                                    \
}

#define SIGMOID(vecInp, vecRes)                                                                                                   \
  vboolN vb2, vb1;                                                                                                                \
  xb_vecNx16 veconeFourthQ4_11 = (xb_vecNx16) QUARTER_QIN;                                                                        \
  xb_vecNx16 vecMask           = (xb_vecNx16) veconeFourthQ4_11 - (xb_vecNx16) 1;                                                 \
  xb_vecNx16 vecConst          = (xb_vecNx16) 28918;                                                                              \
  xb_vecNx16 vecConst_1_3      = (xb_vecNx16) 10923;                                                                              \
  xb_vecNx16 vecAbsVal, vecNegValAbs;                                                                                             \
  vecAbsVal    = IVP_ABSNX16(vecInp);                                                                                             \
  vecNegValAbs = IVP_NEGNX16(vecAbsVal);                                                                                          \
  xb_vecNx16 veNegIn_mod_1_4_m_1_4_Q4_11 = IVP_SUBNX16(IVP_ANDNX16(vecNegValAbs, vecMask), veconeFourthQ4_11);                    \
  xb_vecNx16 veNegIn_mod_1_4_m_1_4_Q_15  = veNegIn_mod_1_4_m_1_4_Q4_11 << QIN_FORMAT_INT;                                         \
  xb_vecNx16 vecRemainderQ4_11           = IVP_SUBNX16(veNegIn_mod_1_4_m_1_4_Q4_11, vecNegValAbs);                                \
  xb_vecNx16 vecX1                       = veNegIn_mod_1_4_m_1_4_Q_15 + (xb_vecNx16) ONEEIGHTH_Q15;                               \
  xb_vecNx16 vecX2                       = IVP_PACKVRNX48(IVP_MULNX16(vecX1, vecX1), 15);                                         \
  xb_vecNx16 vecX3                       = IVP_PACKVRNX48(IVP_MULNX16(vecX1, vecX2), 15);                                         \
  xb_vecNx16 vecX4                       = IVP_PACKVRNX48(IVP_MULNX16(vecX2, vecX2), 15);                                         \
  xb_vecNx16 vec_x4_over_4               = IVP_PACKVRNX48(IVP_MULNX16(vecX4, (xb_vecNx16) QUARTER_Q15), 15);                      \
  xb_vecNx16 vecTemp                     = IVP_ADDNX16(vec_x4_over_4, vecX3);                                                     \
  vecTemp = IVP_PACKVRNX48(IVP_MULNX16(vecTemp, vecConst_1_3), 15);                                                               \
  vecTemp = IVP_ADDNX16(vecTemp, vecX2);                                                                                          \
  vecTemp = IVP_PACKVRNX48(IVP_MULNX16(vecTemp, (xb_vecNx16) HALF_Q15), 15);                                                      \
  vecTemp = IVP_ADDNX16(vecTemp, vecX1);                                                                                          \
  vecTemp = IVP_PACKVRNX48(IVP_MULNX16(vecTemp, vecConst), 15);                                                                   \
  vecRes  = IVP_ADDNX16(vecTemp, vecConst);                                                                                       \
  vb2     = IVP_NEQNX16(IVP_ANDNX16(vecRemainderQ4_11, (xb_vecNx16) (1 << (QIN_FORMAT_FRAC - 2))), (xb_vecNx16) 0);               \
  xb_vecNx16 vecRes1 = IVP_PACKVRNX48(IVP_MULNX16(vecRes, (xb_vecNx16) 25520), 15);                                               \
  vecRes  = IVP_MOVNX16T(vecRes1, vecRes, vb2);                                                                                   \
  vb2     = IVP_NEQNX16(IVP_ANDNX16(vecRemainderQ4_11, (xb_vecNx16) (1 << (QIN_FORMAT_FRAC - 1))), (xb_vecNx16) 0);               \
  vecRes1 = IVP_PACKVRNX48(IVP_MULNX16(vecRes, (xb_vecNx16) 19875), 15);                                                          \
  vecRes  = IVP_MOVNX16T(vecRes1, vecRes, vb2);                                                                                   \
  vb2     = IVP_NEQNX16(IVP_ANDNX16(vecRemainderQ4_11, (xb_vecNx16) (1 << (QIN_FORMAT_FRAC))), (xb_vecNx16) 0);                   \
  vecRes1 = IVP_PACKVRNX48(IVP_MULNX16(vecRes, (xb_vecNx16) 12055), 15);                                                          \
  vecRes  = IVP_MOVNX16T(vecRes1, vecRes, vb2);                                                                                   \
  vb2     = IVP_NEQNX16(IVP_ANDNX16(vecRemainderQ4_11, (xb_vecNx16) (1 << (QIN_FORMAT_FRAC + 1))), (xb_vecNx16) 0);               \
  vecRes1 = IVP_PACKVRNX48(IVP_MULNX16(vecRes, (xb_vecNx16) 4435), 15);                                                           \
  vecRes  = IVP_MOVNX16T(vecRes1, vecRes, vb2);                                                                                   \
  vb2     = IVP_NEQNX16(IVP_ANDNX16(vecRemainderQ4_11, (xb_vecNx16) (1 << (QIN_FORMAT_FRAC + 2))), (xb_vecNx16) 0);               \
  vecRes1 = IVP_PACKVRNX48(IVP_MULNX16(vecRes, (xb_vecNx16) 600), 15);                                                            \
  vecRes  = IVP_MOVNX16T(vecRes1, vecRes, vb2);                                                                                   \
  vb2     = IVP_NEQNX16(IVP_ANDNX16(vecRemainderQ4_11, (xb_vecNx16) (1 << (QIN_FORMAT_FRAC + 3))), (xb_vecNx16) 0);               \
  vecRes1 = IVP_PACKVRNX48(IVP_MULNX16(vecRes, (xb_vecNx16) 11), 15);                                                             \
  vecRes  = IVP_MOVNX16T(vecRes1, vecRes, vb2);                                                                                   \
  vb2     = IVP_EQNX16(veNegIn_mod_1_4_m_1_4_Q4_11, (xb_vecNx16) 0);                                                              \
  vecRes  = IVP_MOVNX16T((xb_vecNx16) ONE_Q15, vecRes, vb2);                                                                      \
  const int16_t constant_48_over_17     = 23130;                                                                                  \
  const int16_t constant_neg_32_over_17 = -15420;                                                                                 \
  xb_vecNx16 vecHalfDen                 = IVP_PACKVRNRNX48(IVP_MULPNX16((xb_vecNx16) ONE_Q15, 1, vecRes + (xb_vecNx16) 1, 1), 1); \
  vecX1 = IVP_PACKVRNX48(IVP_MULNX16(vecHalfDen, (xb_vecNx16) constant_neg_32_over_17), 15);                                      \
  vecX1 = vecX1 + (xb_vecNx16) constant_48_over_17;                                                                               \
  xb_vecNx16 vecHalfDenTimesX   = IVP_PACKVRNX48(IVP_MULNX16(vecHalfDen, vecX1), 15);                                             \
  xb_vecNx16 vec1mHalfDenTimesX = IVP_SUBNX16((1 << (15 - 2)), vecHalfDenTimesX);                                                 \
  xb_vecNx16 vecDeltaX          = IVP_PACKVRNX48(IVP_MULNX16(vec1mHalfDenTimesX, vecX1), 15);                                     \
  vecX1              = vecX1 + (vecDeltaX << 2);                                                                                  \
  vecHalfDenTimesX   = IVP_PACKVRNX48(IVP_MULNX16(vecHalfDen, vecX1), 15);                                                        \
  vec1mHalfDenTimesX = IVP_SUBNX16((1 << (15 - 2)), vecHalfDenTimesX);                                                            \
  vecDeltaX          = IVP_PACKVRNX48(IVP_MULNX16(vec1mHalfDenTimesX, vecX1), 15);                                                \
  vecX1              = vecX1 + (vecDeltaX << 2);                                                                                  \
  vecHalfDenTimesX   = IVP_PACKVRNX48(IVP_MULNX16(vecHalfDen, vecX1), 15);                                                        \
  vec1mHalfDenTimesX = IVP_SUBNX16((1 << (15 - 2)), vecHalfDenTimesX);                                                            \
  vecDeltaX          = IVP_PACKVRNX48(IVP_MULNX16(vec1mHalfDenTimesX, vecX1), 15);                                                \
  vecX1              = vecX1 + (vecDeltaX << 2);                                                                                  \
  vecRes             = vecX1 << 1;                                                                                                \
  vb1                = IVP_EQNX16(vecInp, 0);                                                                                     \
  vecRes             = IVP_MOVNX16T(HALF_Q15, vecRes, vb1);                                                                       \
  vb1                = IVP_LTNX16(vecInp, 0);                                                                                     \
  vecRes             = IVP_MOVNX16T(IVP_SUBNX16(ONE_Q15, vecRes), vecRes, vb1);


#define XI_CHECK_ZEROPOINT_AVGPOOLAQUANT(inT, param)                                    \
{                                                                                       \
  if(XI_TYPE_ELEMENT_TYPE(XI_TILE3D_GET_TYPE(inT)) == XI_S8)                            \
  {                                                                                     \
    XI_CHECK_ERROR((XI_CNN_AVGPOOLA_GET_ZEROPTINPUT(param) >= -128 &&                   \
      XI_CNN_AVGPOOLA_GET_ZEROPTINPUT(param) <= 127), XI_ERR_BADARG,                    \
      "Input ZeroPoint is not in between -128 and 127");                                \
    XI_CHECK_ERROR((XI_CNN_AVGPOOLA_GET_ZEROPTOUTPUT(param) >= -128 &&                  \
      XI_CNN_AVGPOOLA_GET_ZEROPTOUTPUT(param) <= 127), XI_ERR_BADARG,                   \
      "Output ZeroPoint is not in between -128 and 127");                               \
  }                                                                                     \
  else                                                                                  \
  {                                                                                     \
    XI_CHECK_ERROR((XI_CNN_AVGPOOLA_GET_ZEROPTINPUT(param) >= 0 &&                      \
      XI_CNN_AVGPOOLA_GET_ZEROPTINPUT(param) <= 255), XI_ERR_BADARG,                    \
      "Input ZeroPoint is not in between 0 and 255");                                   \
    XI_CHECK_ERROR((XI_CNN_AVGPOOLA_GET_ZEROPTOUTPUT(param) >= 0 &&                     \
      XI_CNN_AVGPOOLA_GET_ZEROPTOUTPUT(param) <= 255), XI_ERR_BADARG,                   \
      "Output ZeroPoint is not in between 0 and 255");                                  \
  }                                                                                     \
}

#define XI_CHECK_MINMAX_AVGPOOLA(inT, param)                                             \
{                                                                                        \
  if(XI_TYPE_ELEMENT_TYPE(XI_TILE3D_GET_TYPE(inT)) == XI_S8)                             \
  {                                                                                      \
    XI_CHECK_ERROR((XI_CNN_POOLING_GET_MIN_VAL(param) >= -128 &&                         \
      XI_CNN_POOLING_GET_MAX_VAL(param) <= 127), XI_ERR_BADARG,                          \
      "Min and Max activation should be between between -128 and 127");                  \
  }                                                                                      \
  else                                                                                   \
  {                                                                                      \
    XI_CHECK_ERROR((XI_CNN_POOLING_GET_MIN_VAL(param) >= 0 &&                            \
      XI_CNN_POOLING_GET_MAX_VAL(param) <= 255), XI_ERR_BADARG,                          \
      "Min and Max activation should be between between 0 and 255");                     \
  }                                                                                      \
  XI_CHECK_ERROR((XI_CNN_POOLING_GET_MIN_VAL(param) < XI_CNN_POOLING_GET_MAX_VAL(param)),\
    XI_ERR_BADARG, "Maximum value is not less than minimum value.");                     \
}

#define XI_CHECK_MINMAX_AVGPOOLAQUANT(inT, param)                                           \
{                                                                                           \
  if(XI_TYPE_ELEMENT_TYPE(XI_TILE3D_GET_TYPE(inT)) == XI_S8)                                \
  {                                                                                         \
    XI_CHECK_ERROR((XI_CNN_AVGPOOLA_GET_MIN_VAL(param) >= -128 &&                           \
      XI_CNN_AVGPOOLA_GET_MAX_VAL(param) <= 127), XI_ERR_BADARG,                            \
      "Min and Max activation should be between between -128 and 127");                     \
  }                                                                                         \
  else                                                                                      \
  {                                                                                         \
    XI_CHECK_ERROR((XI_CNN_AVGPOOLA_GET_MIN_VAL(param) >= 0 &&                              \
      XI_CNN_AVGPOOLA_GET_MAX_VAL(param) <= 255), XI_ERR_BADARG,                            \
      "Min and Max activation should be between between 0 and 255");                        \
  }                                                                                         \
  XI_CHECK_ERROR((XI_CNN_AVGPOOLA_GET_MIN_VAL(param) <= XI_CNN_AVGPOOLA_GET_MAX_VAL(param)),\
    XI_ERR_BADARG, "Maximum value should not be less than minimum value.");                 \
}

#define XI_CHECK_ZEROPOINT_L2POOL2D(inT, param)                                         \
{                                                                                       \
  if(XI_TYPE_ELEMENT_TYPE(XI_TILE3D_GET_TYPE(inT)) == XI_S8)                            \
  {                                                                                     \
    XI_CHECK_ERROR((XI_CNN_L2POOL2D_GET_ZEROPTINPUT(param) >= -128 &&                   \
      XI_CNN_L2POOL2D_GET_ZEROPTINPUT(param) <= 127), XI_ERR_BADARG,                    \
      "Input ZeroPoint is not in between -128 and 127");                                \
    XI_CHECK_ERROR((XI_CNN_L2POOL2D_GET_ZEROPTOUTPUT(param) >= -128 &&                  \
      XI_CNN_L2POOL2D_GET_ZEROPTOUTPUT(param) <= 127), XI_ERR_BADARG,                   \
      "Output ZeroPoint is not in between -128 and 127");                               \
  }                                                                                     \
  else                                                                                  \
  {                                                                                     \
    XI_CHECK_ERROR((XI_CNN_L2POOL2D_GET_ZEROPTINPUT(param) >= 0 &&                      \
      XI_CNN_L2POOL2D_GET_ZEROPTINPUT(param) <= 255), XI_ERR_BADARG,                    \
      "Input ZeroPoint is not in between 0 and 255");                                   \
    XI_CHECK_ERROR((XI_CNN_L2POOL2D_GET_ZEROPTOUTPUT(param) >= 0 &&                     \
      XI_CNN_L2POOL2D_GET_ZEROPTOUTPUT(param) <= 255), XI_ERR_BADARG,                   \
      "Output ZeroPoint is not in between 0 and 255");                                  \
  }                                                                                     \
}

#define XI_CHECK_MINMAX_PARAM_L2POOL2D(inT, param)                                          \
{                                                                                           \
  if(XI_TYPE_ELEMENT_TYPE(XI_TILE3D_GET_TYPE(inT)) == XI_S8)                                \
  {                                                                                         \
    XI_CHECK_ERROR((XI_CNN_L2POOL2D_GET_MIN_VAL(param) >= -128 &&                           \
      XI_CNN_L2POOL2D_GET_MAX_VAL(param) <= 127), XI_ERR_BADARG,                            \
      "Min and Max activation should be between between -128 and 127");                     \
  }                                                                                         \
  else                                                                                      \
  {                                                                                         \
    XI_CHECK_ERROR((XI_CNN_L2POOL2D_GET_MIN_VAL(param) >= 0 &&                              \
      XI_CNN_L2POOL2D_GET_MAX_VAL(param) <= 255), XI_ERR_BADARG,                            \
      "Min and Max activation should be between between 0 and 255");                        \
  }                                                                                         \
  XI_CHECK_ERROR((XI_CNN_L2POOL2D_GET_MIN_VAL(param) <= XI_CNN_L2POOL2D_GET_MAX_VAL(param)),\
    XI_ERR_BADARG, "Maximum value should not be less than minimum value.");                 \
}
#endif //if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))
#endif //__XI_CNN_H__
