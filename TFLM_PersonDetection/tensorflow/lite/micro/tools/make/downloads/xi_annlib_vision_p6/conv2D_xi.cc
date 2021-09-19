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
#include <stdio.h>

//#undef IVP_DMULQA2N8XR8

#ifdef IVP_PACKVNX48 /* Only available in Q7*/
#ifdef ENABLE_CONV_ASYMMETRIC_ROUNDING
#define CONV_PACK_AND_ROUNDING(hvecIn1, hvecIn2, vecRightShift, vecOut)      \
		{                                                                            \
	/* Performs ASYMMETRIC ROUNDING Operation */                               \
	vecOut = IVP_PACKVNX48(IVP_CVT48SNX32(hvecIn2, hvecIn1), vecRightShift);   \
}
#else  /* #ifdef ENABLE_CONV_ASYMMETRIC_ROUNDING */
#define CONV_PACK_AND_ROUNDING(hvecIn1, hvecIn2, vecRightShift, vecOut)      \
		{                                                                            \
	/* Performs SYMMETRIC ROUNDING Operation */                                \
	vboolN_2 vbA = IVP_LTN_2X32(hvecIn1, 0);                                   \
	vboolN_2 vbB = IVP_LTN_2X32(hvecIn2, 0);                                   \
	xb_vecN_2x32v hvecAbs1 = IVP_ABSN_2X32(hvecIn1);                           \
	xb_vecN_2x32v hvecAbs2 = IVP_ABSN_2X32(hvecIn2);                           \
	xb_vecNx48 wvec48AB = IVP_CVT48UNX32(hvecAbs2, hvecAbs1);                  \
	vecOut = IVP_PACKVNX48(wvec48AB, vecRightShift);                           \
	vboolN vbAB = IVP_JOINBN_2(vbB, vbA);                                      \
	IVP_SUBNX16T(vecOut, 0, vecOut, vbAB);                                     \
}
#endif
	/* xiConvolvedAVQ3D_QM24_S8_DWH uses 24 bit accumulator for computations. Move the 24 bit accumulated sum      */
	/* to 32 bit vec registers. Accumulated value is multiplied with OutScale value and packed by (31 - Left shift)*/
	/* value. Then, pack the result to 16 bits. zeroPtOutput is added to the result to get quantized output value. */
	/* The result is saturate to minLim and maxLim.                                                                */
#define PACK_SCALE_AND_ROUNDING(daccSum, dvecOut)                                                              \
		{                                                                                                              \
			xb_vecNx16 vecOutL, vecOutH;                                                                                 \
			/* Move the 24 bit accumulated sum to 32 bit vec registers. */                                               \
			xb_vecN_2x32v hvecIn1 = IVP_CVT32S2NX24LL(daccSum);                                                          \
			xb_vecN_2x32v hvecIn2 = IVP_CVT32S2NX24LH(daccSum);                                                          \
			xb_vecN_2x32v hvecIn3 = IVP_CVT32S2NX24HL(daccSum);                                                          \
			xb_vecN_2x32v hvecIn4 = IVP_CVT32S2NX24HH(daccSum);                                                          \
			/* accumulated sum is multiplied with output scale value and Packed by 31 - Left shift */                    \
			hvecIn1 = IVP_PACKVN_2X64W(IVP_MULN_2X32(hvecIn1, hvecScale1), IVP_SUBN_2X32(31, hvecLeftShift1));           \
			hvecIn2 = IVP_PACKVN_2X64W(IVP_MULN_2X32(hvecIn2, hvecScale2), IVP_SUBN_2X32(31, hvecLeftShift2));           \
			hvecIn3 = IVP_PACKVN_2X64W(IVP_MULN_2X32(hvecIn3, hvecScale3), IVP_SUBN_2X32(31, hvecLeftShift3));           \
			hvecIn4 = IVP_PACKVN_2X64W(IVP_MULN_2X32(hvecIn4, hvecScale4), IVP_SUBN_2X32(31, hvecLeftShift4));           \
			/* Perform Pack and Rounding */                                                                              \
			CONV_PACK_AND_ROUNDING(hvecIn1, hvecIn2, vecRightShift1, vecOutL);                                           \
			CONV_PACK_AND_ROUNDING(hvecIn3, hvecIn4, vecRightShift2, vecOutH);                                           \
			/* Add zeroPtOut */                                                                                          \
			xb_vecNx16 vecOutAddZP_L = IVP_ADDSNX16(vecOutL, (xb_vecNx16) zeroPtOut);                                    \
			xb_vecNx16 vecOutAddZP_H = IVP_ADDSNX16(vecOutH, (xb_vecNx16) zeroPtOut);                                    \
			/* Result is saturate to minLim and maxLim */                                                                \
			vecOutL = (IVP_MAXNX16(IVP_MINNX16(vecOutAddZP_L, (xb_vecNx16) maxLim), (xb_vecNx16) minLim));               \
			vecOutH = (IVP_MAXNX16(IVP_MINNX16(vecOutAddZP_H, (xb_vecNx16) maxLim), (xb_vecNx16) minLim));               \
			dvecOut = IVP_SEL2NX8I(IVP_MOV2NX8_FROMNX16(vecOutH), IVP_MOV2NX8_FROMNX16(vecOutL), IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0); \
		}

			/* xiConvolvedAVQ3D_QM32_S8_DWH uses 32 bit accumulator for computations. Accumulated value is multiplied with */
			/* OutScale value and packed by (31 - Left shift) value. Then, pack the result to 16 bits. zeroPtOutput is     */
			/* added to the result to get quantized output value. The result is saturate to minLim and maxLim.             */
#define PACK_SCALE_AND_ROUNDING_QM32(hvecIn1, hvecIn2, hvecIn3, hvecIn4, dvecOut)                              \
		{                                                                                                              \
				/* accumulated sum is multiplied with output scale value and Packed by 31 - Left shift */                    \
				xb_vecNx16 vecOutL, vecOutH;                                                                                 \
				hvecIn1 = IVP_PACKVN_2X64W(IVP_MULN_2X32(hvecIn1, hvecScale1), IVP_SUBN_2X32(31, hvecLeftShift1));           \
				hvecIn2 = IVP_PACKVN_2X64W(IVP_MULN_2X32(hvecIn2, hvecScale2), IVP_SUBN_2X32(31, hvecLeftShift2));           \
				hvecIn3 = IVP_PACKVN_2X64W(IVP_MULN_2X32(hvecIn3, hvecScale3), IVP_SUBN_2X32(31, hvecLeftShift3));           \
				hvecIn4 = IVP_PACKVN_2X64W(IVP_MULN_2X32(hvecIn4, hvecScale4), IVP_SUBN_2X32(31, hvecLeftShift4));           \
				/* Perform Pack and Rounding */                                                                              \
				CONV_PACK_AND_ROUNDING(hvecIn1, hvecIn2, vecRightShift1, vecOutL);                                           \
				CONV_PACK_AND_ROUNDING(hvecIn3, hvecIn4, vecRightShift2, vecOutH);                                           \
				/* Add zeroPtOut */                                                                                          \
				xb_vecNx16 vecOutAddZP_L = IVP_ADDSNX16(vecOutL, (xb_vecNx16) zeroPtOut);                                    \
				xb_vecNx16 vecOutAddZP_H = IVP_ADDSNX16(vecOutH, (xb_vecNx16) zeroPtOut);                                    \
				/* Result is saturate to minLim and maxLim */                                                                \
				vecOutL = (IVP_MAXNX16(IVP_MINNX16(vecOutAddZP_L, (xb_vecNx16) maxLim), (xb_vecNx16) minLim));               \
				vecOutH = (IVP_MAXNX16(IVP_MINNX16(vecOutAddZP_H, (xb_vecNx16) maxLim), (xb_vecNx16) minLim));               \
				dvecOut = IVP_SEL2NX8I(IVP_MOV2NX8_FROMNX16(vecOutH), IVP_MOV2NX8_FROMNX16(vecOutL), IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0); \
			}
#else // #ifdef IVP_PACKVNX48
#ifdef ENABLE_CONV_ASYMMETRIC_ROUNDING
#define CONV_PACK_AND_ROUNDING(hvecIn1, hvecIn2, vecRightShift, vecOut)                   \
		{                                                                                         \
	/* Performs ASYMMETRIC ROUNDING Operation */                                            \
	xb_vecNx16 vecShiftL, vecShiftH;                                                        \
	xb_vecN_2x32v hvecShiftL, hvecShiftH;                                                   \
	IVP_DSELNX16I(vecShiftH, vecShiftL, 0, vecRightShift, IVP_DSELI_INTERLEAVE_1);          \
	hvecShiftL = IVP_MOVN_2X32_FROMNX16(vecShiftL);                                         \
	hvecShiftH = IVP_MOVN_2X32_FROMNX16(vecShiftH);                                         \
	hvecIn1    = IVP_ADDN_2X32(hvecIn1, IVP_SLLN_2X32(1, IVP_SUBN_2X32(hvecShiftL, 1)));    \
	hvecIn2    = IVP_ADDN_2X32(hvecIn2, IVP_SLLN_2X32(1, IVP_SUBN_2X32(hvecShiftH, 1)));    \
	hvecIn1    = IVP_SRAN_2X32(hvecIn1, hvecShiftL);                                        \
	hvecIn2    = IVP_SRAN_2X32(hvecIn2, hvecShiftH);                                        \
	xb_vecNx48 wvec48AB = IVP_CVT48SNX32(hvecIn2, hvecIn1);                                 \
	vecOut = IVP_PACKVRNX48(wvec48AB, 0);                                                   \
}
#else  /* #ifdef ENABLE_CONV_ASYMMETRIC_ROUNDING */
#define CONV_PACK_AND_ROUNDING(hvecIn1, hvecIn2, vecRightShift, vecOut)                   \
		{                                                                                         \
	/* Performs SYMMETRIC ROUNDING Operation */                                             \
	xb_vecNx16 vecShiftL, vecShiftH;                                                        \
	xb_vecN_2x32v hvecShiftL, hvecShiftH, hvecAbs1, hvecAbs2;                               \
	hvecAbs1 = IVP_ABSN_2X32(hvecIn1);                                                      \
	hvecAbs2 = IVP_ABSN_2X32(hvecIn2);                                                      \
	IVP_DSELNX16I(vecShiftH, vecShiftL, 0, vecRightShift, IVP_DSELI_INTERLEAVE_1);          \
	hvecShiftL = IVP_MOVN_2X32_FROMNX16(vecShiftL);                                         \
	hvecShiftH = IVP_MOVN_2X32_FROMNX16(vecShiftH);                                         \
	hvecAbs1   = IVP_ADDN_2X32(hvecAbs1, IVP_SLLN_2X32(1, IVP_SUBN_2X32(hvecShiftL, 1)));   \
	hvecAbs2   = IVP_ADDN_2X32(hvecAbs2, IVP_SLLN_2X32(1, IVP_SUBN_2X32(hvecShiftH, 1)));   \
	hvecAbs1   = IVP_SRAN_2X32(hvecAbs1, hvecShiftL);                                       \
	hvecAbs2   = IVP_SRAN_2X32(hvecAbs2, hvecShiftH);                                       \
	hvecAbs1   = IVP_MULSGNN_2X32(hvecIn1, hvecAbs1);                                       \
	hvecAbs2   = IVP_MULSGNN_2X32(hvecIn2, hvecAbs2);                                       \
	xb_vecNx48 wvec48AB = IVP_CVT48SNX32(hvecAbs2, hvecAbs1);                               \
	vecOut = IVP_PACKVRNX48(wvec48AB, 0);                                                   \
}
#endif

#define PACK_SCALE_AND_ROUNDING(daccSum, dvecOut)                                                              \
		{                                                                                                              \
			xb_vecNx16 vecOutL, vecOutH;                                                                                 \
			/* Move the 24 bit accumulated sum to 32 bit vec registers. */                                               \
			xb_vecN_2x32v hvecIn1 = IVP_CVT32S2NX24LL(daccSum);                                                          \
			xb_vecN_2x32v hvecIn2 = IVP_CVT32S2NX24LH(daccSum);                                                          \
			xb_vecN_2x32v hvecIn3 = IVP_CVT32S2NX24HL(daccSum);                                                          \
			xb_vecN_2x32v hvecIn4 = IVP_CVT32S2NX24HH(daccSum);                                                          \
			/* accumulated sum is multiplied with output scale value and Packed by 31 - Left shift */                    \
			hvecIn1 = IVP_SLSN_2X32(hvecIn1, hvecLeftShift1);                                                            \
			hvecIn2 = IVP_SLSN_2X32(hvecIn2, hvecLeftShift2);                                                            \
			hvecIn3 = IVP_SLSN_2X32(hvecIn3, hvecLeftShift3);                                                            \
			hvecIn4 = IVP_SLSN_2X32(hvecIn4, hvecLeftShift4);                                                            \
			xb_vecNx16U vecIn1 = IVP_MOVNX16_FROMN_2X32(hvecIn1);                                                        \
			xb_vecNx16U vecIn2 = IVP_MOVNX16_FROMN_2X32(hvecIn2);                                                        \
			xb_vecNx16U vecIn3 = IVP_MOVNX16_FROMN_2X32(hvecIn3);                                                        \
			xb_vecNx16U vecIn4 = IVP_MOVNX16_FROMN_2X32(hvecIn4);                                                        \
			xb_vecN_2x64w haccScale1 = IVP_MULUSN_2X16X32_0(vecIn1, hvecScale1);                                         \
			xb_vecN_2x64w haccScale2 = IVP_MULUSN_2X16X32_0(vecIn2, hvecScale2);                                         \
			xb_vecN_2x64w haccScale3 = IVP_MULUSN_2X16X32_0(vecIn3, hvecScale3);                                         \
			xb_vecN_2x64w haccScale4 = IVP_MULUSN_2X16X32_0(vecIn4, hvecScale4);                                         \
			IVP_MULAHN_2X16X32_1(haccScale1, vecIn1, hvecScale1);                                                        \
			IVP_MULAHN_2X16X32_1(haccScale2, vecIn2, hvecScale2);                                                        \
			IVP_MULAHN_2X16X32_1(haccScale3, vecIn3, hvecScale3);                                                        \
			IVP_MULAHN_2X16X32_1(haccScale4, vecIn4, hvecScale4);                                                        \
			hvecIn1 = IVP_PACKVRN_2X64W(haccScale1, 31);                                                                 \
			hvecIn2 = IVP_PACKVRN_2X64W(haccScale2, 31);                                                                 \
			hvecIn3 = IVP_PACKVRN_2X64W(haccScale3, 31);                                                                 \
			hvecIn4 = IVP_PACKVRN_2X64W(haccScale4, 31);                                                                 \
			/* Perform Pack and Rounding */                                                                              \
			CONV_PACK_AND_ROUNDING(hvecIn1, hvecIn2, vecRightShift1, vecOutL);                                           \
			CONV_PACK_AND_ROUNDING(hvecIn3, hvecIn4, vecRightShift2, vecOutH);                                           \
			/* Add zeroPtOut */                                                                                          \
			xb_vecNx16 vecOutAddZP_L = IVP_ADDSNX16(vecOutL, (xb_vecNx16) zeroPtOut);                                    \
			xb_vecNx16 vecOutAddZP_H = IVP_ADDSNX16(vecOutH, (xb_vecNx16) zeroPtOut);                                    \
			/* Result is saturate to minLim and maxLim */                                                                \
			vecOutL = (IVP_MAXNX16(IVP_MINNX16(vecOutAddZP_L, (xb_vecNx16) maxLim), (xb_vecNx16) minLim));               \
			vecOutH = (IVP_MAXNX16(IVP_MINNX16(vecOutAddZP_H, (xb_vecNx16) maxLim), (xb_vecNx16) minLim));               \
			dvecOut = IVP_SEL2NX8I(IVP_MOV2NX8_FROMNX16(vecOutH), IVP_MOV2NX8_FROMNX16(vecOutL), IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0); \
		}

#define PACK_SCALE_AND_ROUNDING_QM32(hvecIn1, hvecIn2, hvecIn3, hvecIn4, dvecOut)                                    \
		{                                                                                                                    \
				/* accumulated sum is multiplied with output scale value and Packed by 31 - Left shift */                          \
				hvecIn1 = IVP_SLSN_2X32(hvecIn1, hvecLeftShift1);                                                                  \
				hvecIn2 = IVP_SLSN_2X32(hvecIn2, hvecLeftShift2);                                                                  \
				hvecIn3 = IVP_SLSN_2X32(hvecIn3, hvecLeftShift3);                                                                  \
				hvecIn4 = IVP_SLSN_2X32(hvecIn4, hvecLeftShift4);                                                                  \
				xb_vecNx16 vecOutL, vecOutH;                                                                                       \
				/* Multiply the Accumulated value with Output Scale Value*/                                                        \
				xb_vecNx16U vecIn1 = IVP_MOVNX16_FROMN_2X32(hvecIn1);                                                              \
				xb_vecNx16U vecIn2 = IVP_MOVNX16_FROMN_2X32(hvecIn2);                                                              \
				xb_vecNx16U vecIn3 = IVP_MOVNX16_FROMN_2X32(hvecIn3);                                                              \
				xb_vecNx16U vecIn4 = IVP_MOVNX16_FROMN_2X32(hvecIn4);                                                              \
				xb_vecN_2x64w haccScale1 = IVP_MULUSN_2X16X32_0(vecIn1, hvecScale1);                                               \
				xb_vecN_2x64w haccScale2 = IVP_MULUSN_2X16X32_0(vecIn2, hvecScale2);                                               \
				xb_vecN_2x64w haccScale3 = IVP_MULUSN_2X16X32_0(vecIn3, hvecScale3);                                               \
				xb_vecN_2x64w haccScale4 = IVP_MULUSN_2X16X32_0(vecIn4, hvecScale4);                                               \
				IVP_MULAHN_2X16X32_1(haccScale1, vecIn1, hvecScale1);                                                              \
				IVP_MULAHN_2X16X32_1(haccScale2, vecIn2, hvecScale2);                                                              \
				IVP_MULAHN_2X16X32_1(haccScale3, vecIn3, hvecScale3);                                                              \
				IVP_MULAHN_2X16X32_1(haccScale4, vecIn4, hvecScale4);                                                              \
				/* Pack the result by 31 */                                                                                        \
				hvecIn1 = IVP_PACKVRN_2X64W(haccScale1, 31);                                                                     \
				hvecIn2 = IVP_PACKVRN_2X64W(haccScale2, 31);                                                                     \
				hvecIn3 = IVP_PACKVRN_2X64W(haccScale3, 31);                                                                     \
				hvecIn4 = IVP_PACKVRN_2X64W(haccScale4, 31);                                                                     \
				/* Perform Pack and Rounding */                                                                                    \
				CONV_PACK_AND_ROUNDING(hvecIn1, hvecIn2, vecRightShift1, vecOutL);                                                 \
				CONV_PACK_AND_ROUNDING(hvecIn3, hvecIn4, vecRightShift2, vecOutH);                                                 \
				/* Add zeroPtOut */                                                                                                \
				xb_vecNx16 vecOutAddZP_L = IVP_ADDSNX16(vecOutL, (xb_vecNx16) zeroPtOut);                                          \
				xb_vecNx16 vecOutAddZP_H = IVP_ADDSNX16(vecOutH, (xb_vecNx16) zeroPtOut);                                          \
				/* Result is saturate to minLim and maxLim */                                                                      \
				vecOutL = (IVP_MAXNX16(IVP_MINNX16(vecOutAddZP_L, (xb_vecNx16) maxLim), (xb_vecNx16) minLim));                     \
				vecOutH = (IVP_MAXNX16(IVP_MINNX16(vecOutAddZP_H, (xb_vecNx16) maxLim), (xb_vecNx16) minLim));                     \
				dvecOut = IVP_SEL2NX8I(IVP_MOV2NX8_FROMNX16(vecOutH), IVP_MOV2NX8_FROMNX16(vecOutL), IVP_SELI_8B_EXTRACT_1_OF_2_OFF_0); \
			}
#endif

#define MOV_ACC_VAL(hvecIn1, hvecIn2, hvecIn3, hvecIn4, hvecBias1, hvecBias2, hvecBias3, hvecBias4) \
		{                                                                                                   \
			hvecIn1 = hvecBias1;                                                                              \
			hvecIn2 = hvecBias2;                                                                              \
			hvecIn3 = hvecBias3;                                                                              \
			hvecIn4 = hvecBias4;                                                                              \
		}

#define ACCUMULATE_VAL(hvecSum1, hvecSum2, hvecSum3, hvecSum4, daccSum)    \
		{                                                                          \
	hvecSum1 += IVP_CVT32S2NX24LL(daccSum);                                  \
	hvecSum2 += IVP_CVT32S2NX24LH(daccSum);                                  \
	hvecSum3 += IVP_CVT32S2NX24HL(daccSum);                                  \
	hvecSum4 += IVP_CVT32S2NX24HH(daccSum);                                  \
		}

#define MAX(a, b)  ((b) > (a) ? (b) : (a))
#define min(a, b)  ((a) > (b) ? (b) : (a))
#define MUL_PACK_SCALE_SHIFT_ADD_ZPT_CLAMP_LIMITS(vecOut, vecIn, slope, m1, nm1, neg_output_multiplier,    \
		pos_output_multiplier, neg_output_shift, pos_output_shift,\
		output_zero_point, minVal, maxVal)                        \
		{                                                                                                          \
	xb_vecNx16 vecMul_0;                                                                                     \
	xb_vecN_2x32Uv hvecMul_1L, hvecMul_1H;                                                                   \
	vboolN vbN;                                                                                              \
	vboolN_2 vbN_2_L, vbN_2_H;                                                                               \
	vbN = IVP_LTNX16(vecIn, (xb_vecNx16) 0);                                                                 \
	IVP_EXTRACTBN(vbN_2_H, vbN_2_L, vbN);                                                                    \
	xb_vecNx16  vecTemp;                                                                                     \
	vecTemp = IVP_SLLNX16(slope, (xb_vecNx16)nm1);                                                           \
	vecMul_0  = IVP_MOVNX16T(vecTemp, m1, vbN);                                                              \
	hvecMul_1L = IVP_MOVN_2X32UT(neg_output_multiplier, pos_output_multiplier, vbN_2_L);                     \
	hvecMul_1H = IVP_MOVN_2X32UT(neg_output_multiplier, pos_output_multiplier, vbN_2_H);                     \
	\
	xb_vecNx48 acc = IVP_MULNX16(vecIn, vecMul_0);                                                           \
	\
	xb_vecN_2x32v hvecOutL, hvecOutH, hvecOutLB, hvecOutHB;                                                  \
	hvecOutL = IVP_CVT32SNX48L(acc);                                                                         \
	hvecOutH = IVP_CVT32SNX48H(acc);                                                                         \
	\
	xb_vecN_2x64w haccScaledL, haccScaledH;                                                                  \
	haccScaledL = IVP_MULUUN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecOutL), hvecMul_1L);                        \
	IVP_MULSUAHN_2X16X32_1(haccScaledL, IVP_MOVNX16_FROMN_2X32(hvecOutL), hvecMul_1L);                       \
	haccScaledH = IVP_MULUUN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecOutH), hvecMul_1H);                        \
	IVP_MULSUAHN_2X16X32_1(haccScaledH, IVP_MOVNX16_FROMN_2X32(hvecOutH), hvecMul_1H);                       \
	\
	hvecOutL = IVP_PACKVRN_2X64W(haccScaledL, 31);                                                           \
	hvecOutH = IVP_PACKVRN_2X64W(haccScaledH, 31);                                                           \
	\
	hvecOutLB = hvecOutL;                                                                                    \
	hvecOutHB = hvecOutH;                                                                                    \
	\
	xb_vecNx16 vecOutA, vecOutB, vecOutC;                                                                    \
	/*Pack the result to 16 bits and  saturate to short min and max*/                                        \
	MORPH_IVP_PACKVRNX48(vecOutA, hvecOutH, hvecOutL, pos_output_shift);                                     \
	MORPH_IVP_PACKVRNX48(vecOutB, hvecOutHB, hvecOutLB, neg_output_shift);                                   \
	vecOutC = IVP_MOVNX16T(vecOutB, vecOutA, vbN);                                                           \
	vecOutC = IVP_ADDSNX16(vecOutC, output_zero_point);                                                      \
	vecOut  = IVP_MAXNX16(IVP_MINNX16(vecOutC, (xb_vecNx16) maxVal), (xb_vecNx16) minVal);                   \
}


	/***************************** xiPreluA3D_U8_DWH_ref ******************************************/
	/* Description : VP6 implementation of parameterized RELU which retains                       */
	/*               positive values of input and applies a slope to                              */
	/*               input values which are less than 0.                                          */
	/*               If slope is equal to 0 given function behaves as basic RELU                  */
	/*               If dimension of slope is 1x1x1 the function will behave as leakyRelu         */
	/* Inputs      : Input tile, slope applied to negative values of input                        */
	/* Outputs     : XI Error Code                                                                */
	/* InOuts      : Output Tile                                                                  */
	/* Assumptions : InData, outData, slope is unsigned 8bit                                      */
	/*               min-max values are unsigned 8bit                                             */
	/**********************************************************************************************/
XI_ERR_TYPE xiPreluA3D_U8U8U8_DWH(const xi_pTile3D inTile,
			const xi_pTile3D slopeTile,
			xi_pTile3D outTile,
			const xi_cnn_preluA_params* params)
{
	/* Error Checks */
	XI_ERROR_CHECKS()
    								{
		XI_CHECK_TILE3D_U8(inTile);
		XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
		XI_CHECK_TILE3D_DATA_ORDER(inTile, XI_DWH);
		XI_CHECK_TILE3D_U8(outTile);
		XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
		XI_CHECK_TILE3D_DATA_ORDER(outTile, XI_DWH);
		XI_CHECK_TILE3D_U8(slopeTile);
		XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(slopeTile);
		XI_CHECK_TILE3D_DATA_ORDER(slopeTile, XI_DWH);
		XI_CHECK_POINTER(params);
		XI_CHECK_ERROR(!((XI_TILE3D_GET_DIM1(inTile) != XI_TILE3D_GET_DIM1(slopeTile)) && ((XI_TILE3D_GET_DIM1(inTile) != 1) && (XI_TILE3D_GET_DIM1(slopeTile) != 1))), \
				XI_ERR_DATASIZE, "If the size of a first dimension in both input tiles are not equal, then one of them must be equal to 1");
		XI_CHECK_ERROR(!((XI_TILE3D_GET_DIM2(inTile) != XI_TILE3D_GET_DIM2(slopeTile)) && ((XI_TILE3D_GET_DIM2(inTile) != 1) && (XI_TILE3D_GET_DIM2(slopeTile) != 1))), \
				XI_ERR_DATASIZE, "If the size of a second dimension in both input tiles are not equal, then one of them must be equal to 1");
		XI_CHECK_ERROR(!((XI_TILE3D_GET_DIM3(inTile) != XI_TILE3D_GET_DIM3(slopeTile)) && ((XI_TILE3D_GET_DIM3(inTile) != 1) && (XI_TILE3D_GET_DIM3(slopeTile) != 1))), \
				XI_ERR_DATASIZE, "If the sizes of a third dimension in both input tiles are not equal, then one of them must be equal to 1");
		XI_CHECK_ERROR(!(MAX(XI_TILE3D_GET_DIM1(inTile), XI_TILE3D_GET_DIM1(slopeTile)) != XI_TILE3D_GET_DIM1(outTile)), \
				XI_ERR_DATASIZE, "The size of the first dimension in output tile must be equal to the maximum of sizes of first dimension in both input tiles");
		XI_CHECK_ERROR(!(MAX(XI_TILE3D_GET_DIM2(inTile), XI_TILE3D_GET_DIM2(slopeTile)) != XI_TILE3D_GET_DIM2(outTile)), \
				XI_ERR_DATASIZE, "The size of the second dimension in output tile must be equal to the maximum of sizes of second dimension in both input tiles");
		XI_CHECK_ERROR(!(MAX(XI_TILE3D_GET_DIM3(inTile), XI_TILE3D_GET_DIM3(slopeTile)) != XI_TILE3D_GET_DIM3(outTile)), \
				XI_ERR_DATASIZE, "The size of the third dimension in output tile must be equal to the maximum of sizes of third dimension in both input tiles");
		XI_CHECK_ERROR((!((XI_CNN_PRELUA_PARAMS_GET_IN_ZERO_POINT(params) < 0) || (XI_CNN_PRELUA_PARAMS_GET_IN_ZERO_POINT(params) > 255))), \
				XI_ERR_BADARG, "The input zero point value is invalid");
		XI_CHECK_ERROR((!((XI_CNN_PRELUA_PARAMS_GET_OUT_ZERO_POINT(params) < 0) || (XI_CNN_PRELUA_PARAMS_GET_OUT_ZERO_POINT(params) > 255))), \
				XI_ERR_BADARG, "The output zero point value is invalid");
		XI_CHECK_ERROR((!((XI_CNN_PRELUA_PARAMS_GET_ALPHA_ZERO_POINT(params) < 0) || (XI_CNN_PRELUA_PARAMS_GET_ALPHA_ZERO_POINT(params) > 255))), \
				XI_ERR_BADARG, "The alpha zero point value is invalid");
		XI_CHECK_ERROR(!((XI_CNN_PRELUA_PARAMS_GET_MIN_VAL(params) > XI_CNN_PRELUA_PARAMS_GET_MAX_VAL(params))), \
				XI_ERR_BADARG, "The min val is greater than max val");
		XI_CHECK_ERROR(((XI_CNN_PRELUA_PARAMS_GET_NEGATIVE_OUTPUT_MULTIPLIER(params) >= 0) && (XI_CNN_PRELUA_PARAMS_GET_POSITIVE_OUTPUT_MULTIPLIER(params) >= 0)), \
				XI_ERR_BADARG, "The output multipliers values are invalid");
		XI_CHECK_ERROR(((XI_CNN_PRELUA_PARAMS_GET_POSITIVE_OUTPUT_SHIFT(params) < 32) && (XI_CNN_PRELUA_PARAMS_GET_POSITIVE_OUTPUT_SHIFT(params) > -32) && (XI_CNN_PRELUA_PARAMS_GET_NEGATIVE_OUTPUT_SHIFT(params) <= 31)), \
				XI_ERR_BADARG, "The at least one of the output shift values is invalid");
    								}

	/* Getting parameters from the tile structures */
	const int32_t outDepth = XI_TILE3D_GET_DIM1(outTile);
	const int32_t outWidth = XI_TILE3D_GET_DIM2(outTile);
	const int32_t outHeight = XI_TILE3D_GET_DIM3(outTile);
	int32_t inDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(inTile);
	int32_t inDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(inTile);
	int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
	int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
	int32_t slopeDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(slopeTile);
	int32_t slopeDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(slopeTile);

	uint8_t* pInput = (uint8_t*)XI_TILE3D_GET_DATA_PTR(inTile);
	uint8_t* pOutput = (uint8_t*)XI_TILE3D_GET_DATA_PTR(outTile);
	uint8_t* pSlope = (uint8_t*)XI_TILE3D_GET_DATA_PTR(slopeTile);

	int32_t input_zero_point = XI_CNN_PRELUA_PARAMS_GET_IN_ZERO_POINT(params);
	int32_t alpha_zero_point = XI_CNN_PRELUA_PARAMS_GET_ALPHA_ZERO_POINT(params);
	int32_t output_zero_point = XI_CNN_PRELUA_PARAMS_GET_OUT_ZERO_POINT(params);
	int32_t pos_output_multiplier = XI_CNN_PRELUA_PARAMS_GET_POSITIVE_OUTPUT_MULTIPLIER(params);
	int32_t pos_output_shift = XI_CNN_PRELUA_PARAMS_GET_POSITIVE_OUTPUT_SHIFT(params);
	int32_t neg_output_multiplier = XI_CNN_PRELUA_PARAMS_GET_NEGATIVE_OUTPUT_MULTIPLIER(params);
	int32_t neg_output_shift = XI_CNN_PRELUA_PARAMS_GET_NEGATIVE_OUTPUT_SHIFT(params);
	int32_t minVal = XI_CNN_PRELUA_PARAMS_GET_MIN_VAL(params);
	int32_t maxVal = XI_CNN_PRELUA_PARAMS_GET_MAX_VAL(params);

	int32_t leakyReluCase = (XI_TILE3D_GET_DIM1(slopeTile) == 1) ? 1 : 0;

	int32_t d, w, h;

	int32_t pm1, nm1;
	int32_t ps_, ns_;

	// Merge equations of all three cases of positive multiplier and shift
	if (pos_output_multiplier == 0 && pos_output_shift == 0)
	{
		pm1 = 1;
		ps_ = 0;
	}
	else if (pos_output_shift <= 0)
	{
		pm1 = 1;
		ps_ = (-1) * pos_output_shift;
	}
	else
	{
		pm1 = (1 << pos_output_shift);
		ps_ = 0;
	}

	// Merge equations of all three cases of positive multiplier and shift
	if (neg_output_multiplier == 0 && neg_output_shift == 0)
	{
		nm1 = 0;
		ns_ = 0;
	}
	else if (neg_output_shift <= 0)
	{
		nm1 = 0;
		ns_ = (-1) * neg_output_shift;
	}
	else
	{
		nm1 = neg_output_shift;
		ns_ = 0;
	}

	if (XI_TILE3D_GET_DIM2(inTile) == 1)
	{
		inDataPitch1 = 0;
	}
	if (XI_TILE3D_GET_DIM3(inTile) == 1)
	{
		inDataPitch2 = 0;
	}

	if (XI_TILE3D_GET_DIM2(slopeTile) == 1)
	{
		slopeDataPitch1 = 0;
	}
	if (XI_TILE3D_GET_DIM3(slopeTile) == 1)
	{
		slopeDataPitch2 = 0;
	}

	xb_vecNx8U* __restrict pvecIn = (xb_vecNx8U*)pInput;
	xb_vecNx8U* __restrict pvecOut = (xb_vecNx8U*)pOutput;
	xb_vecNx8U* __restrict pvecSlope = (xb_vecNx8U*)pSlope;

	int32_t numElem;
	xb_vecNx16 vecIn, vecOut, vecSlope;
	valign valSlope, valIn, valOut;

	/*
    The code is conditionally executed based on following conditions:
    1. LeakyReLU
       [1.a] slopeTile = {1, 1, 1} and input, output and slope tile edges are zero enabling contiguous load/store.
       [1.b] slopeTile = {1, dim2, dim3}, input and output may or may not have edges.

    2. ReLU
       [2.a] indim1 > 1, slopeDim1 > 1, dim2 and dim3 of input and slope >=1
       [2.b] indim1 ==1, slopeDim1 > 1, dim2 and dim3 of input and slope >=1
	 */

	if (leakyReluCase)
	{
		/* Leaky ReLU case where the input, ouput and slope tiles do not have edges*/
		if ((outDepth == inDataPitch1) && (outDepth == outDataPitch1) && ((outWidth * outDepth) == inDataPitch2) && (inDataPitch2 == outDataPitch2))
		{
			if ((XI_TILE3D_GET_DIM1(slopeTile) == 1) && (XI_TILE3D_GET_DIM2(slopeTile) == 1) && (XI_TILE3D_GET_DIM3(slopeTile) == 1))
			{
				/*Estimate data count to process */
				int32_t total_count = outDepth * outWidth * outHeight;
				/*Prime the input*/
				valIn = IVP_LANX8U_PP(pvecIn);
				valOut = IVP_ZALIGN();
				/*Load slope value*/
				valSlope = IVP_LANX8U_PP(pvecSlope);
				IVP_LAVNX8U_XP(vecSlope, valSlope, pvecSlope, 1);
				vecSlope = IVP_REPNX16(vecSlope, 0);
				vecSlope = IVP_SUBNX16(vecSlope, alpha_zero_point);

				for (d = 0; d < total_count; d += XCHAL_IVPN_SIMD_WIDTH)
				{
					numElem = total_count - d;
					IVP_LAVNX8U_XP(vecIn, valIn, pvecIn, numElem);
					vecIn = IVP_SUBNX16(vecIn, input_zero_point);

					MUL_PACK_SCALE_SHIFT_ADD_ZPT_CLAMP_LIMITS(vecOut, vecIn, vecSlope, pm1, nm1, neg_output_multiplier, pos_output_multiplier, \
							ns_, ps_, output_zero_point, minVal, maxVal);

					IVP_SAVNX8U_XP(vecOut, valOut, pvecOut, numElem);
				}
				IVP_SAPOSNX8U_FP(valOut, pvecOut);
				return(XI_ERROR_STATUS());
			}
		}
		if ((XI_TILE3D_GET_DIM2(slopeTile) >= 1) && (XI_TILE3D_GET_DIM3(slopeTile) >= 1)) /* generalized leaky ReLU case*/
		{
			for (h = 0; h < outHeight; ++h)     // along the image height(DWH)
			{
				for (w = 0; w < outWidth; ++w) // along the image width(DWH)
				{
					/* Initialize input, output and slope pointers */
					pvecIn = (xb_vecNx8U*)(pInput + h * inDataPitch2 + w * inDataPitch1);
					pvecOut = (xb_vecNx8U*)(pOutput + h * outDataPitch2 + w * outDataPitch1);
					pvecSlope = (xb_vecNx8U*)(pSlope + h * slopeDataPitch2 + w * slopeDataPitch1);
					valIn = IVP_LANX8U_PP(pvecIn);
					valSlope = IVP_LANX8U_PP(pvecSlope);
					/* Load slope values prior to inner loop, as the slope data remains same across dim1 */
					IVP_LAVNX8U_XP(vecSlope, valSlope, pvecSlope, 1);
					vecSlope = IVP_REPNX16(vecSlope, 0);
					vecSlope = IVP_SUBNX16(vecSlope, alpha_zero_point);
					valOut = IVP_ZALIGN();
					for (d = 0; d < (outDepth - XCHAL_IVPN_SIMD_WIDTH); d += XCHAL_IVPN_SIMD_WIDTH)
					{
						IVP_LAVNX8U_XP(vecIn, valIn, pvecIn, outDepth - d);
						vecIn = IVP_SUBNX16(vecIn, input_zero_point);

						MUL_PACK_SCALE_SHIFT_ADD_ZPT_CLAMP_LIMITS(vecOut, vecIn, vecSlope, pm1, nm1, neg_output_multiplier, pos_output_multiplier, \
								ns_, ps_, output_zero_point, minVal, maxVal);

						IVP_SAVNX8U_XP(vecOut, valOut, pvecOut, outDepth - d);
					}
					IVP_SAPOSNX8U_FP(valOut, pvecOut);

					if (d < outDepth) /* Handle corner case*/
					{
						IVP_LAVNX8U_XP(vecIn, valIn, pvecIn, outDepth - d);
						vecIn = IVP_SUBNX16(vecIn, input_zero_point);
						MUL_PACK_SCALE_SHIFT_ADD_ZPT_CLAMP_LIMITS(vecOut, vecIn, vecSlope, pm1, nm1, neg_output_multiplier, pos_output_multiplier, \
								ns_, ps_, output_zero_point, minVal, maxVal);
						IVP_SAVNX8U_XP(vecOut, valOut, pvecOut, outDepth - d);
						IVP_SAPOSNX8U_FP(valOut, pvecOut);
					}
				}
			}
			return(XI_ERROR_STATUS());
		}
	}
	else /* pReLU case (non-leaky ReLU)*/
	{
		if (XI_TILE3D_GET_DIM1(inTile) > 1) /* inDim == slopeDim1 */
		{
			for (h = 0; h < outHeight; ++h)     // along the image height(DWH)
			{
				for (w = 0; w < outWidth; ++w)     // along the image width(DWH)
				{
					/* Initialize input, output and slope pointers */
					pvecIn = (xb_vecNx8U*)(pInput + h * inDataPitch2 + w * inDataPitch1);
					pvecOut = (xb_vecNx8U*)(pOutput + h * outDataPitch2 + w * outDataPitch1);
					pvecSlope = (xb_vecNx8U*)(pSlope + h * slopeDataPitch2 + w * slopeDataPitch1);

					valSlope = IVP_LANX8U_PP(pvecSlope);
					valIn = IVP_LANX8U_PP(pvecIn);
					valOut = IVP_ZALIGN();
					for (d = 0; d < outDepth - XCHAL_IVPN_SIMD_WIDTH; d += XCHAL_IVPN_SIMD_WIDTH)
					{
						IVP_LAVNX8U_XP(vecIn, valIn, pvecIn, outDepth - d);
						IVP_LAVNX8U_XP(vecSlope, valSlope, pvecSlope, outDepth - d);

						vecSlope = IVP_SUBNX16(vecSlope, alpha_zero_point);
						vecIn = IVP_SUBNX16(vecIn, input_zero_point);

						MUL_PACK_SCALE_SHIFT_ADD_ZPT_CLAMP_LIMITS(vecOut, vecIn, vecSlope, pm1, nm1, neg_output_multiplier, pos_output_multiplier, \
								ns_, ps_, output_zero_point, minVal, maxVal);

						IVP_SAVNX8U_XP(vecOut, valOut, pvecOut, outDepth - d);
					}
					IVP_SAPOSNX8U_FP(valOut, pvecOut);
					if (d < outDepth) /* Handle corner case across depth */
					{
						IVP_LAVNX8U_XP(vecIn, valIn, pvecIn, outDepth - d);
						IVP_LAVNX8U_XP(vecSlope, valSlope, pvecSlope, outDepth - d);

						vecSlope = IVP_SUBNX16(vecSlope, alpha_zero_point);
						vecIn = IVP_SUBNX16(vecIn, input_zero_point);

						MUL_PACK_SCALE_SHIFT_ADD_ZPT_CLAMP_LIMITS(vecOut, vecIn, vecSlope, pm1, nm1, neg_output_multiplier, pos_output_multiplier, \
								ns_, ps_, output_zero_point, minVal, maxVal);

						IVP_SAVNX8U_XP(vecOut, valOut, pvecOut, outDepth - d);
						IVP_SAPOSNX8U_FP(valOut, pvecOut);
					}
				}
			}
		}
		else /* (if XI_TILE3D_GET_DIM1(inTile) == 1) */
		{
			int32_t numElemIn = 1;
			for (h = 0; h < outHeight; ++h)     // along the image height(DWH)
			{
				for (w = 0; w < outWidth; ++w)     // along the image width(DWH)
				{
					/* Initialize input pointers */
					pvecIn = (xb_vecNx8U*)(pInput + h * inDataPitch2 + w * inDataPitch1);
					pvecOut = (xb_vecNx8U*)(pOutput + h * outDataPitch2 + w * outDataPitch1);
					pvecSlope = (xb_vecNx8U*)(pSlope + h * slopeDataPitch2 + w * slopeDataPitch1);
					valSlope = IVP_LANX8U_PP(pvecSlope);
					valIn = IVP_LANX8U_PP(pvecIn);

					/* Load input data outside the inner loop as the data remains same across dim1 */
					IVP_LAVNX8U_XP(vecIn, valIn, pvecIn, numElemIn);
					vecIn = IVP_REPNX16(vecIn, 0);
					vecIn = IVP_SUBNX16(vecIn, input_zero_point);
					valOut = IVP_ZALIGN();
					for (d = 0; d < outDepth - XCHAL_IVPN_SIMD_WIDTH; d += XCHAL_IVPN_SIMD_WIDTH)
					{
						IVP_LAVNX8U_XP(vecSlope, valSlope, pvecSlope, outDepth - d);

						vecSlope = IVP_SUBNX16(vecSlope, alpha_zero_point);

						MUL_PACK_SCALE_SHIFT_ADD_ZPT_CLAMP_LIMITS(vecOut, vecIn, vecSlope, pm1, nm1, neg_output_multiplier, pos_output_multiplier, \
								ns_, ps_, output_zero_point, minVal, maxVal);

						IVP_SAVNX8U_XP(vecOut, valOut, pvecOut, outDepth - d);
					}
					IVP_SAPOSNX8U_FP(valOut, pvecOut);
					if (d < outDepth) /* Handle corner cases */
					{
						IVP_LAVNX8U_XP(vecSlope, valSlope, pvecSlope, outDepth - d);

						vecSlope = IVP_SUBNX16(vecSlope, alpha_zero_point);

						MUL_PACK_SCALE_SHIFT_ADD_ZPT_CLAMP_LIMITS(vecOut, vecIn, vecSlope, pm1, nm1, neg_output_multiplier, pos_output_multiplier, \
								ns_, ps_, output_zero_point, minVal, maxVal);

						IVP_SAVNX8U_XP(vecOut, valOut, pvecOut, outDepth - d);
						IVP_SAPOSNX8U_FP(valOut, pvecOut);
					}
				}
			}
		}
	}
	return(XI_ERROR_STATUS());
}

	/*************************** xiConvolvedAVQ3D_QM24_S8_DWH *********************************/
	/* Description : Computes 3D convolution with symmetric quantization support.             */
	/* Inputs      : Input data tile, Coeff data Tile, biasArray, outScaleArray,              */
	/*               outShiftArray, CNNA convolution params structure                         */
	/* InOuts      : outTile                                                                  */
	/* Assumptions : Input data is  S8                                                        */
	/*               Coeff data is  S8                                                        */
	/*               Output data is S8                                                        */
	/*               biasArray is   S32                                                       */
	/*               outScaleArray is   S32                                                   */
	/*               outShiftArray is   S8                                                    */
	/******************************************************************************************/



XI_ERR_TYPE xiConvolvedAVQ3D_QM24_S8_DWH(const xi_pTile3D inTile,
			const xi_pTile4D coeffTile,
			const xi_pArray biasArray,
			const xi_pArray outScaleArray,
			const xi_pArray outShiftArray,
			xi_pTile3D outTile,
			const xi_cnna_conv_params* param)
{
	XI_ERROR_CHECKS()
									{
		XI_CHECK_TILE3D_S8(inTile);
		XI_CHECK_TILE4D_S8(coeffTile);
		XI_CHECK_TILE3D_S8(outTile);
		XI_CHECK_ARRAY_S32(biasArray);
		XI_CHECK_ARRAY_S32(outScaleArray);
		XI_CHECK_ARRAY_S8(outShiftArray);
		XI_CHECK_POINTER(param);
		XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(inTile);
		XI_CHECK_TILE4D_IN_DRAM_BOUNDARY(coeffTile);
		XI_CHECK_TILE3D_IN_DRAM_BOUNDARY(outTile);
		XI_CHECK_TILES3D_ARE_NOT_OVERLAP(inTile, outTile);
		XI_CHECK_TILES3D_ARE_NOT_OVERLAP(coeffTile, outTile);
		XI_CHECK_TILES3D_ARE_NOT_OVERLAP(biasArray, outTile);
		XI_CHECK_TILES3D_ARE_NOT_OVERLAP(outScaleArray, outTile);
		XI_CHECK_TILES3D_ARE_NOT_OVERLAP(outShiftArray, outTile);
		XI_CHECK_TILE3D_DATA_ORDER(inTile, XI_DWH);
		XI_CHECK_TILE4D_DATA_ORDER(coeffTile, XI_NDWH);
		XI_CHECK_TILE3D_DATA_ORDER(outTile, XI_DWH);
		XI_CHECK_DILATION_CNNA(param, 1, 1);
		XI_CHECK_TILE4D_IALIGNMENT_2NX8(coeffTile);
		XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(outTile) == XI_TILE4D_GET_DIM1(coeffTile), XI_ERR_DATASIZE, \
				"\nOutput depth = %d, Coefficient tile dim1 = %d\nOutput depth should be same as coefficient tile dim1", \
				XI_TILE3D_GET_DIM1(outTile), XI_TILE4D_GET_DIM1(coeffTile));
		XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(inTile) == XI_TILE4D_GET_DIM2(coeffTile), XI_ERR_DATASIZE, \
				"\nInput depth = %d, Coefficient tile dim2 = %d\nInput depth should be same as coefficient tile dim2", \
				XI_TILE3D_GET_DIM1(inTile), XI_TILE4D_GET_DIM2(coeffTile));
		XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(biasArray) >= XI_TILE3D_GET_DIM1(outTile), \
				XI_ERR_DATASIZE, "\nWidth of Bias Array = %d, Number of output channels = %d"\
				"\nWidth of Bias Array should be greater than or equal to Number of output channels", \
				XI_ARRAY_GET_WIDTH(biasArray), XI_TILE3D_GET_DIM1(outTile));
		XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(outScaleArray) >= XI_TILE3D_GET_DIM1(outTile), \
				XI_ERR_DATASIZE, "\nWidth of Output Scale Array = %d, Number of output channels = %d"\
				"\nWidth of Output Scale Array should be greater than or equal to Number of output channels", \
				XI_ARRAY_GET_WIDTH(outScaleArray), XI_TILE3D_GET_DIM1(outTile));
		XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(outShiftArray) >= XI_TILE3D_GET_DIM1(outTile), \
				XI_ERR_DATASIZE, "\nWidth of Output Shift Array = %d, Number of output channels = %d"\
				"\nWidth of Output Shift Array should be greater than or equal to Number of output channels", \
				XI_ARRAY_GET_WIDTH(outShiftArray), XI_TILE3D_GET_DIM1(outTile));
		XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_EDGE1(inTile) == XI_TILE4D_GET_DIM3(coeffTile) >> 1, XI_ERR_KSIZE, \
				"\nInput tile edge1 along width = %d, Kernel width = %d\nInput tile edge1 along width should be equal to \
				   kernel width/2", XI_TILE3D_GET_DIM2_EDGE1(inTile), XI_TILE4D_GET_DIM3(coeffTile));
		XI_CHECK_ERROR(XI_TILE3D_GET_DIM3_EDGE1(inTile) == XI_TILE4D_GET_DIM4(coeffTile) >> 1, XI_ERR_KSIZE, \
				"\nInput tile edge1 along height = %d, Kernel height = %d\nInput tile edge1 along height should be equal to\
					kernel height/2", XI_TILE3D_GET_DIM3_EDGE1(inTile), XI_TILE4D_GET_DIM4(coeffTile));
		XI_CHECK_ERROR(XI_TILE3D_GET_DIM2(inTile) == (XI_TILE3D_GET_DIM2(outTile) - 1) * XI_CNNA_CONV_GET_STRIDE(param) + 1, \
				XI_ERR_DATASIZE, "\nInput width = %d, Output width = %d, stride = %hhu\nInput tile width should be equal to\
									 (output tile width - 1) * stride", \
									 XI_TILE3D_GET_DIM2(inTile), XI_TILE3D_GET_DIM2(outTile), XI_CNNA_CONV_GET_STRIDE(param));
		XI_CHECK_ERROR(XI_TILE3D_GET_DIM3(inTile) == (XI_TILE3D_GET_DIM3(outTile) - 1) * XI_CNNA_CONV_GET_STRIDE(param) + 1, \
				XI_ERR_DATASIZE, "\nInput height = %d, Output height = %d, stride = %hhu\nInput tile height should be equal\
									 to (output tile height - 1) * stride", \
									 XI_TILE3D_GET_DIM3(inTile), XI_TILE3D_GET_DIM3(outTile), XI_CNNA_CONV_GET_STRIDE(param));
		XI_CHECK_ERROR(XI_TILE3D_GET_DIM2_PITCH(outTile) == XI_TILE3D_GET_DIM1(outTile) * XI_TILE3D_GET_DIM2(outTile), \
				XI_ERR_DATASIZE, "\nOutput tile dim2 pitch = %d, dim1 size = %d, dim2 size = %d\noutTile should not have \
									edges along dim1 and dim2", \
									XI_TILE3D_GET_DIM2_PITCH(outTile), XI_TILE3D_GET_DIM1(outTile), XI_TILE3D_GET_DIM2(outTile));
		XI_CHECK_ERROR(XI_TILE3D_GET_DIM1(inTile) == XI_TILE3D_GET_DIM1_PITCH(inTile), XI_ERR_DATASIZE, \
				"\nInput tile dim1 size = %d, dim1 pitch = %d\ninTile should not have edges in innermost dimension", \
				XI_TILE3D_GET_DIM1(inTile), XI_TILE3D_GET_DIM1_PITCH(inTile));
		XI_CHECK_ERROR(XI_TILE4D_GET_DIM2_PITCH(coeffTile) == XI_TILE4D_GET_DIM1_PITCH(coeffTile) * XI_TILE4D_GET_DIM2(coeffTile), \
				XI_ERR_DATASIZE, "\nCoefficient tile dim2 pitch = %d, dim1 size = %d, dim2 size = %d\nCoefficient should \
									not have edges along dim1 and dim2", \
									XI_TILE3D_GET_DIM2_PITCH(coeffTile), XI_TILE3D_GET_DIM1(coeffTile), XI_TILE3D_GET_DIM2(coeffTile));
		if (XI_CNNA_CONV_GET_FLAG_RELU(param))
		{
			XI_CHECK_ERROR(XI_CNNA_CONV_GET_RELUMIN(param) <= XI_CNNA_CONV_GET_RELUMAX(param), XI_ERR_BADARG, \
					"\nRelu min = %hi, Relu max = %hi\nRelu max should be greater than or equal to Relu min", \
					XI_CNNA_CONV_GET_RELUMIN(param), XI_CNNA_CONV_GET_RELUMAX(param));
			XI_CHECK_ERROR((XI_CNNA_CONV_GET_RELUMIN(param) >= SCHAR_MIN), XI_ERR_BADARG, \
					"\nRelu min = %hi, value should be greater than or equal to -128", XI_CNNA_CONV_GET_RELUMIN(param));
			XI_CHECK_ERROR((XI_CNNA_CONV_GET_RELUMAX(param) <= SCHAR_MAX), XI_ERR_BADARG, \
					"\nRelu max = %hi, value should be less than or equal to 127", XI_CNNA_CONV_GET_RELUMAX(param));
		}
		XI_CHECK_ERROR(((XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param) >= SCHAR_MIN) && \
				(XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param) <= SCHAR_MAX)), XI_ERR_BADARG, \
				"\nZero point output = %hi, value should be in the range -128 to 127", XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param));
		XI_CHECK_ERROR(((XI_CNNA_CONV_GET_ZEROPT_INPUT(param) >= SCHAR_MIN) && \
				(XI_CNNA_CONV_GET_ZEROPT_INPUT(param) <= SCHAR_MAX)), XI_ERR_BADARG, \
				"\nZero point input = %hi, value should be in the range -128 to 127", XI_CNNA_CONV_GET_ZEROPT_INPUT(param));
		XI_CHECK_ERROR((XI_CNNA_CONV_GET_ZEROPT_COEFF(param) == 0), XI_ERR_BADARG, \
				"\nZero point coefficient = %hi, value should be equal to zero", XI_CNNA_CONV_GET_ZEROPT_COEFF(param));
									}

	/* Getting parameters from the tile structures */
	const int32_t output_depth = XI_TILE3D_GET_DIM1(outTile);
	const int32_t output_width = XI_TILE3D_GET_DIM2(outTile);
	const int32_t output_height = XI_TILE3D_GET_DIM3(outTile);
	const int32_t inChanNum = XI_TILE4D_GET_DIM2(coeffTile);
	const int32_t kernel_width = XI_TILE4D_GET_DIM3(coeffTile);
	//const int32_t kernel_height = XI_TILE4D_GET_DIM4(coeffTile);
	const int32_t outDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
	const int32_t outDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
	const int32_t coeffDataPitch1 = XI_TILE4D_GET_DIM1_PITCH(coeffTile);
	const int32_t coeffDataPitch3 = XI_TILE4D_GET_DIM3_PITCH(coeffTile);
	const int32_t inDataPitch1 = XI_TILE3D_GET_DIM1_PITCH(inTile);
	const int32_t inDataPitch2 = XI_TILE3D_GET_DIM2_PITCH(inTile);

	/* CNNA convolution parameters */
	const int16_t zeroPtOut = XI_CNNA_CONV_GET_ZEROPT_OUTPUT(param);
	const uint8_t enableReLu = XI_CNNA_CONV_GET_FLAG_RELU(param);
	const uint8_t stride = XI_CNNA_CONV_GET_STRIDE(param);
	const int16_t reluMin = XI_CNNA_CONV_GET_RELUMIN(param);
	const int16_t reluMax = XI_CNNA_CONV_GET_RELUMAX(param);

	/* Setting the limits for output data according to ReLu is enabled or not*/
	const int16_t minLim = enableReLu ? reluMin : SCHAR_MIN;
	const int16_t maxLim = enableReLu ? reluMax : SCHAR_MAX;

	/*Get data pointers */
	int8_t* pCoeffData = (int8_t*)XI_TILE3D_GET_DATA_PTR(coeffTile);
	int32_t* pBias = (int32_t*)XI_ARRAY_GET_DATA_PTR(biasArray);
	int32_t* pOutScale = (int32_t*)XI_ARRAY_GET_DATA_PTR(outScaleArray);
	int8_t* pOutShift = (int8_t*)XI_ARRAY_GET_DATA_PTR(outShiftArray);
	int8_t* pOutData = (int8_t*)XI_TILE3D_GET_DATA_PTR(outTile);
	int8_t* pInData = (int8_t*)XI_TILE3D_GET_DATA_PTR(inTile)
										- XI_TILE3D_GET_DIM2_EDGE1(inTile) * XI_TILE3D_GET_DIM1_PITCH(inTile)
										- XI_TILE3D_GET_DIM3_EDGE1(inTile) * XI_TILE3D_GET_DIM2_PITCH(inTile);

	/* Input and output data pointers */
	xb_vecN_2x32v* __restrict phvecBias1;
	xb_vecN_2x32v* __restrict phvecBias2;
	xb_vecN_2x32v* __restrict phvecOutScale1;
	xb_vecN_2x32v* __restrict phvecOutScale2;
	xb_vec2Nx8* __restrict pdvecOutShift;
	xb_vec2Nx8* __restrict pdvecCoeff;
	xb_vec2Nx8* __restrict pdvecData1;
	xb_vec2Nx8* __restrict pdvecData2;
	xb_vec2Nx8* __restrict pdvecData3;
	xb_vec2Nx8* __restrict pdvecData4;

#if (XCHAL_IVPN_SIMD_WIDTH == 8)
	xb_vec2Nx8* __restrict pdvecData5;
	xb_vec2Nx8* __restrict pdvecData6;
	xb_vec2Nx8* __restrict pdvecData7;
	xb_vec2Nx8* __restrict pdvecData8;
	xb_vec2Nx8 dvecData5, dvecData6, dvecData7, dvecData8;
	xb_vec2Nx8 dvecCoeff9, dvecCoeff10, dvecCoeff11, dvecCoeff12;
	xb_vec2Nx8 dvecCoeff13, dvecCoeff14, dvecCoeff15, dvecCoeff16;
	xb_vec2Nx24 daccSum5, daccSum6, daccSum7, daccSum8;
	xb_vec2Nx8 dvecOut5, dvecOut6, dvecOut7, dvecOut8;
#endif

	xb_vec2Nx8* __restrict pdvecOut;

	/* Input and output data vectors */
	xb_vec2Nx8 dvecData1, dvecData2, dvecData3, dvecData4;
	xb_vec2Nx8 dvecCoeff1, dvecCoeff2, dvecCoeff3, dvecCoeff4;
	//xb_vec2Nx8 dvecCoeff5, dvecCoeff6, dvecCoeff7, dvecCoeff8;
	xb_vecN_2x32v hvecScale1, hvecScale2, hvecScale3, hvecScale4;
	xb_vec2Nx8 dvecShift;
	xb_vec2Nx24 daccSum1, daccSum2, daccSum3, daccSum4;

	xb_vec2Nx8 dvecOut1, dvecOut2, dvecOut3, dvecOut4;

	valign vaOutData = IVP_ZALIGN();

	/* Variable Declarations */
	int32_t loopCount, outCh, x, y;//, k;

	int32_t inAddrOff, coeffAddrOff;
	int32_t numIter = kernel_width * inChanNum;

	/* Loops Start */
	for (outCh = 0; outCh < output_depth; outCh += 2 * XCHAL_IVPN_SIMD_WIDTH)/* Output Channels */
	{
		/* To handle corner case when number of output channels
		 * is not a multiple of  2 * XCHAL_IVPN_SIMD_WIDTH*/
		int32_t remainingOutCh = XT_MIN(2 * XCHAL_IVPN_SIMD_WIDTH, output_depth - outCh);
		int32_t remBiasLoad = (remainingOutCh > XCHAL_IVPN_SIMD_WIDTH) ? 1 : 0;

		int32_t remCh1 = XT_MIN(((output_depth - outCh) << 2), (2 * XCHAL_IVPN_SIMD_WIDTH));
		int32_t remCh2 = XT_MIN((((output_depth - outCh) - (XCHAL_IVPN_SIMD_WIDTH/2)) << 2), (2 * XCHAL_IVPN_SIMD_WIDTH));
		int32_t remCh3 = XT_MIN((((output_depth - outCh) - XCHAL_IVPN_SIMD_WIDTH) << 2), (2 * XCHAL_IVPN_SIMD_WIDTH));
		int32_t remCh4 = XT_MIN((((output_depth - outCh) - (XCHAL_IVPN_SIMD_WIDTH + (XCHAL_IVPN_SIMD_WIDTH/2))) << 2), (2 * XCHAL_IVPN_SIMD_WIDTH));

		/* Load OutScale values */
		phvecOutScale1 = (xb_vecN_2x32v*)(pOutScale + outCh);
		phvecOutScale2 = (xb_vecN_2x32v*)(pOutScale + outCh + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
		valign vaOutScale = IVP_LAN_2X32_PP(phvecOutScale1);
		IVP_LAVN_2X32_XP(hvecScale1, vaOutScale, phvecOutScale1, remCh1);
		IVP_LAVN_2X32_XP(hvecScale2, vaOutScale, phvecOutScale1, remCh2);

		vaOutScale = IVP_LAN_2X32_PP(phvecOutScale2);
		IVP_LAVN_2X32_XP(hvecScale3, vaOutScale, phvecOutScale2, remCh3);
		IVP_LAVN_2X32_XP(hvecScale4, vaOutScale, phvecOutScale2, remCh4);

		/* Load OutShift values */
		pdvecOutShift = (xb_vec2Nx8*)(pOutShift + outCh);
		valign vaOutShift = IVP_LA2NX8_PP(pdvecOutShift);
		IVP_LAV2NX8_XP(dvecShift, vaOutShift, pdvecOutShift, remainingOutCh);

		/* Calculate left shift and right shift values */
		vbool2N vb2N = IVP_LT2NX8(dvecShift, 0);
		xb_vec2Nx8 dvecRightShift = IVP_MOV2NX8T(0, dvecShift, vb2N);
		xb_vec2Nx8 dvecLeftShift = 0;
		IVP_SUB2NX8T(dvecLeftShift, 0, dvecShift, vb2N);

		xb_vec2Nx8 dvecRightShiftL, dvecRightShiftH;
		IVP_DSEL2NX8I(dvecRightShiftH, dvecRightShiftL, 0, dvecRightShift, IVP_DSELI_8B_INTERLEAVE_1);

		xb_vecNx16 vecRightShift1 = IVP_MOVNX16_FROM2NX8(dvecRightShiftL);
		xb_vecNx16 vecRightShift2 = IVP_MOVNX16_FROM2NX8(dvecRightShiftH);

		xb_vec2Nx8 dvecLeftShiftL, dvecLeftShiftH;
		IVP_DSEL2NX8I(dvecLeftShiftH, dvecLeftShiftL, 0, dvecLeftShift, IVP_DSELI_8B_INTERLEAVE_1);

		xb_vecNx16 vecLeftShiftLL, vecLeftShiftLH, vecLeftShiftHL, vecLeftShiftHH;
		IVP_DSELNX16I(vecLeftShiftLH, vecLeftShiftLL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftL), IVP_DSELI_INTERLEAVE_1);
		IVP_DSELNX16I(vecLeftShiftHH, vecLeftShiftHL, 0, IVP_MOVNX16_FROM2NX8(dvecLeftShiftH), IVP_DSELI_INTERLEAVE_1);

		xb_vecN_2x32v hvecLeftShift1 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLL);
		xb_vecN_2x32v hvecLeftShift2 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftLH);
		xb_vecN_2x32v hvecLeftShift3 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHL);
		xb_vecN_2x32v hvecLeftShift4 = IVP_MOVN_2X32_FROMNX16(vecLeftShiftHH);
#ifdef IVP_DMULQA2N8XR8
		for (y = 0; y < output_height; y += 3) /* output Height */
		{
			/* Variable to handle corner case when height is odd */
			int32_t numY  = XT_MIN(1, output_height - y - 1);
			int32_t numY1 = XT_MIN(1, output_height - y - 2);
#else
		for (y = 0; y < output_height; y += 2) /* output Height */
		{
			/* Variable to handle corner case when height is odd */
			int32_t numY  = XT_MIN(1, output_height - y - 1);
#endif
			for (x = 0; x < output_width; x += 2) /* output Width */
			{

				/* Variable to handle corner case when width is odd */
				int32_t numX = XT_MIN(1, output_width - x - 1);

				/* Output Data pointer */
				int8_t* pOut = pOutData + (x * outDataPitch1 + y * outDataPitch2);

				/* Input Data and Coeff Data Pointers */
				int8_t* pData = pInData + x * stride * inDataPitch1 + y * stride * inDataPitch2;
				int8_t* pCoeff = pCoeffData + outCh;

				/* Initialize accumulators with bias values */
				phvecBias1 = (xb_vecN_2x32v*)(pBias + outCh);
				phvecBias2 = (xb_vecN_2x32v*)(pBias + outCh + XCHAL_IVPN_SIMD_WIDTH * remBiasLoad);
				ACC_INIT_BIAS(phvecBias1, phvecBias2, daccSum1, daccSum2, daccSum3, daccSum4, remCh1, \
						remCh2, remCh3, remCh4);
#ifdef IVP_DMULQA2N8XR8
				daccSum5 = daccSum1;
				daccSum6 = daccSum2;
#endif
				inAddrOff = 0;
				coeffAddrOff = 0;

#if  (XCHAL_IVPN_SIMD_WIDTH == 32) //P6

				//for (k = 0; k < kernel_height; k++) /* Kernel Height  */
				{
					/* Pointers for Input Data Loads */
					pdvecData1 = (xb_vec2Nx8*)(pData + inAddrOff);
					pdvecData2 = (xb_vec2Nx8*)(pData + inAddrOff + stride * inDataPitch1 * numX);
					pdvecData3 = (xb_vec2Nx8*)(pData + inAddrOff + stride * inDataPitch2 * numY);
					pdvecData4 = (xb_vec2Nx8*)(pData + inAddrOff + stride * \
							(inDataPitch1 + inDataPitch2) * numX * numY);

					/* Pointer for Coefficient Load */
					pdvecCoeff = (xb_vec2Nx8*)(pCoeff + coeffAddrOff);

					/* Primes for Aligning Load */
					valign vaData1 = IVP_LA2NX8_PP(pdvecData1);
					valign vaData2 = IVP_LA2NX8_PP(pdvecData2);
					valign vaData3 = IVP_LA2NX8_PP(pdvecData3);
					valign vaData4 = IVP_LA2NX8_PP(pdvecData4);

					for (loopCount = 0; loopCount < (numIter - 3); loopCount += 4) /* Input Channel * Kernel Width */
					{
						/* Aligning variable vector load of pixels */
						IVP_LAV2NX8_XP(dvecData1, vaData1, pdvecData1, 4);
						IVP_LAV2NX8_XP(dvecData2, vaData2, pdvecData2, 4);
						IVP_LAV2NX8_XP(dvecData3, vaData3, pdvecData3, 4);
						IVP_LAV2NX8_XP(dvecData4, vaData4, pdvecData4, 4);

						/* Extracting first 4 bytes of vector into address register */
						/* Scalar integers to be used for QMUL                      */
						int32_t qmulScalar1 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData1)), 0);
						int32_t qmulScalar2 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData2)), 0);
						int32_t qmulScalar3 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData3)), 0);
						int32_t qmulScalar4 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData4)), 0);

						/* Aligned Vector Loads of coefficients */
						IVP_LV2NX8_XP(dvecCoeff1, pdvecCoeff, coeffDataPitch1);
						IVP_LV2NX8_XP(dvecCoeff2, pdvecCoeff, coeffDataPitch1);
						IVP_LV2NX8_XP(dvecCoeff3, pdvecCoeff, coeffDataPitch1);
						IVP_LV2NX8_XP(dvecCoeff4, pdvecCoeff, coeffDataPitch1);

						IVP_MULQA2N8XR8(daccSum1, dvecCoeff4, dvecCoeff3, dvecCoeff2, dvecCoeff1, qmulScalar1);
						IVP_MULQA2N8XR8(daccSum2, dvecCoeff4, dvecCoeff3, dvecCoeff2, dvecCoeff1, qmulScalar2);
						IVP_MULQA2N8XR8(daccSum3, dvecCoeff4, dvecCoeff3, dvecCoeff2, dvecCoeff1, qmulScalar3);
						IVP_MULQA2N8XR8(daccSum4, dvecCoeff4, dvecCoeff3, dvecCoeff2, dvecCoeff1, qmulScalar4);
					} /* End (Input Channel * Kernel Width) */
					/* Corner Case Handling if number of loopCount is not multiple of 4 */
					if (loopCount < numIter)
					{
						int32_t remInCh = (numIter - loopCount);
						/* Aligning variable vector load of pixels */
						IVP_LAV2NX8_XP(dvecData1, vaData1, pdvecData1, remInCh);
						IVP_LAV2NX8_XP(dvecData2, vaData2, pdvecData2, remInCh);
						IVP_LAV2NX8_XP(dvecData3, vaData3, pdvecData3, remInCh);
						IVP_LAV2NX8_XP(dvecData4, vaData4, pdvecData4, remInCh);

						/* Extracting first 4 bytes of vector into address register */
						/* Scalar integers to be used for QMUL                      */
						int32_t qmulScalar1 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData1)), 0);
						int32_t qmulScalar2 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData2)), 0);
						int32_t qmulScalar3 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData3)), 0);
						int32_t qmulScalar4 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData4)), 0);

						/* For conditional coefficient loads */
						int32_t enable2 = XT_SALT(1, remInCh); /* Will be 1 if remInCh > 1 */
						int32_t enable3 = XT_SALT(2, remInCh); /* Will be 1 if remInCh > 2 */

						/* Aligned Vector Loads of coefficients */
						IVP_LV2NX8_XP(dvecCoeff1, pdvecCoeff, coeffDataPitch1 * enable2);
						IVP_LV2NX8_XP(dvecCoeff2, pdvecCoeff, coeffDataPitch1 * enable3);
						IVP_LV2NX8_XP(dvecCoeff3, pdvecCoeff, coeffDataPitch1);

						IVP_MULQA2N8XR8(daccSum1, 0, dvecCoeff3, dvecCoeff2, dvecCoeff1, qmulScalar1);
						IVP_MULQA2N8XR8(daccSum2, 0, dvecCoeff3, dvecCoeff2, dvecCoeff1, qmulScalar2);
						IVP_MULQA2N8XR8(daccSum3, 0, dvecCoeff3, dvecCoeff2, dvecCoeff1, qmulScalar3);
						IVP_MULQA2N8XR8(daccSum4, 0, dvecCoeff3, dvecCoeff2, dvecCoeff1, qmulScalar4);
					} /* End Corner case handling */
					inAddrOff += inDataPitch2;
					coeffAddrOff += coeffDataPitch3;
				} /* End Kernel Height */

				/* Pack, Output Scale, Output Shift and clamping */
				PACK_SCALE_AND_ROUNDING(daccSum1, dvecOut1);

				/* Store the output dvecOut1 along the output depth */
				pdvecOut = (xb_vec2Nx8*)(pOut + outCh);
				IVP_SAV2NX8_XP(dvecOut1, vaOutData, pdvecOut, remainingOutCh);
				IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);

				/* Pack, Output Scale, Output Shift and clamping */
				PACK_SCALE_AND_ROUNDING(daccSum2, dvecOut2);

				/* Store the output dvecOut2 along the output depth */
				pdvecOut = (xb_vec2Nx8*)(pOut + (outCh + outDataPitch1));
				IVP_SAV2NX8_XP(dvecOut2, vaOutData, pdvecOut, remainingOutCh * numX);
				IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);

				/* Pack, Output Scale, Output Shift and clamping */
				PACK_SCALE_AND_ROUNDING(daccSum3, dvecOut3);

				/* Store the output dvecOut3 along the output depth */
				pdvecOut = (xb_vec2Nx8*)(pOut + (outCh + outDataPitch2));
				IVP_SAV2NX8_XP(dvecOut3, vaOutData, pdvecOut, remainingOutCh * numY);
				IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);

				/* Pack, Output Scale, Output Shift and clamping */
				PACK_SCALE_AND_ROUNDING(daccSum4, dvecOut4);

				/* Store the output dvecOut4 along the output depth */
				pdvecOut = (xb_vec2Nx8*)(pOut + (outCh + outDataPitch1 + outDataPitch2));
				IVP_SAV2NX8_XP(dvecOut4, vaOutData, pdvecOut, remainingOutCh * numX * numY);
				IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);

#else //P1 code
				//for (k = 0; k < kernel_height; k++) /* Kernel Height  */
				{
					/* Pointers for Input Data Loads */
					pdvecData1 = (xb_vec2Nx8*)(pData + inAddrOff); //w0,h0
					pdvecData2 = (xb_vec2Nx8*)(pData + inAddrOff + stride * inDataPitch1 * numX);//w1,h0

					pdvecData3 = (xb_vec2Nx8*)(pData + inAddrOff + stride * inDataPitch2 * numY);//w0,h1
					pdvecData4 = (xb_vec2Nx8*)(pData + inAddrOff + stride * \
							(inDataPitch1 + inDataPitch2) * numX * numY); //w1,h1

#ifdef IVP_DMULQA2N8XR8
					pdvecData5 = (xb_vec2Nx8*)(pData + inAddrOff + stride * (2*inDataPitch2) * numY1);//w0,h2
					pdvecData6 = (xb_vec2Nx8*)(pData + inAddrOff + stride * \
							(inDataPitch1 + (2*inDataPitch2)) * numX * numY1); //w1,h2
#endif
					/* Pointer for Coefficient Load */
					pdvecCoeff = (xb_vec2Nx8*)(pCoeff + coeffAddrOff);

					if(numIter==8)
					{
						valign vaData1 = IVP_LA2NX8_PP(pdvecData1); //h=0
						valign vaData2 = IVP_LA2NX8_PP(pdvecData3); //h=1
#ifdef IVP_DMULQA2N8XR8
						valign vaData3 = IVP_LA2NX8_PP(pdvecData5); //h=1
#endif
						for (loopCount = 0; loopCount < (numIter); loopCount += 8) /* Input Channel * Kernel Width */
						{
							//h=0
							/* Aligning variable vector load of pixels */

							IVP_LA2NX8_XP(dvecData1, vaData1, pdvecData1, 2*XCHAL_IVPN_SIMD_WIDTH); //load 2 widths //w0,w1
							//h=1
							IVP_LA2NX8_XP(dvecData3, vaData2, pdvecData3, 2*XCHAL_IVPN_SIMD_WIDTH); //load 2 widths
#ifdef IVP_DMULQA2N8XR8
							//h=2
							IVP_LA2NX8_XP(dvecData5, vaData3, pdvecData5, 2*XCHAL_IVPN_SIMD_WIDTH); //load 2 widths
#endif
							/* Aligned Vector Loads of coefficients */
							IVP_LV2NX8_XP(dvecCoeff1, pdvecCoeff, coeffDataPitch1);
							IVP_LV2NX8_XP(dvecCoeff2, pdvecCoeff, coeffDataPitch1);
							IVP_LV2NX8_XP(dvecCoeff3, pdvecCoeff, coeffDataPitch1);
							IVP_LV2NX8_XP(dvecCoeff4, pdvecCoeff, coeffDataPitch1);

#ifdef IVP_DMULQA2N8XR8

							xb_int64pr qmulScalar1 = IVP_DEXTRPRN_2X32((IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData1))),
									(IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData1))), 2, 0) ;

							xb_int64pr qmulScalar2 = IVP_DEXTRPRN_2X32((IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData3))),
									(IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData3))), 2, 0) ;

							xb_int64pr qmulScalar3 = IVP_DEXTRPRN_2X32((IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData5))),
									(IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData5))), 2, 0) ;
						    //h=0
							IVP_DMULQA2N8XR8(daccSum2, daccSum1, dvecCoeff4, dvecCoeff3, dvecCoeff2, dvecCoeff1, qmulScalar1);
							//h=1
							IVP_DMULQA2N8XR8(daccSum4, daccSum3, dvecCoeff4, dvecCoeff3, dvecCoeff2, dvecCoeff1, qmulScalar2);
							//h=2
							IVP_DMULQA2N8XR8(daccSum6, daccSum5, dvecCoeff4, dvecCoeff3, dvecCoeff2, dvecCoeff1, qmulScalar3);
#else

							int32_t qmulScalar1 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData1)), 0);
							int32_t qmulScalar2 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData1)), 2);

							int32_t qmulScalar3 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData3)), 0);
							int32_t qmulScalar4 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData3)), 2);
							//h=0
							IVP_MULQA2N8XR8(daccSum1, dvecCoeff4, dvecCoeff3, dvecCoeff2, dvecCoeff1, qmulScalar1);
							IVP_MULQA2N8XR8(daccSum2, dvecCoeff4, dvecCoeff3, dvecCoeff2, dvecCoeff1, qmulScalar2);
							//h=1
							IVP_MULQA2N8XR8(daccSum3, dvecCoeff4, dvecCoeff3, dvecCoeff2, dvecCoeff1, qmulScalar3);
							IVP_MULQA2N8XR8(daccSum4, dvecCoeff4, dvecCoeff3, dvecCoeff2, dvecCoeff1, qmulScalar4);
#endif


							/* Aligned Vector Loads of coefficients */
							IVP_LV2NX8_XP(dvecCoeff5, pdvecCoeff, coeffDataPitch1);
							IVP_LV2NX8_XP(dvecCoeff6, pdvecCoeff, coeffDataPitch1);
							IVP_LV2NX8_XP(dvecCoeff7, pdvecCoeff, coeffDataPitch1);
							IVP_LV2NX8_XP(dvecCoeff8, pdvecCoeff, coeffDataPitch1);

#ifdef IVP_DMULQA2N8XR8
							xb_int64pr qmulScalar4 = IVP_DEXTRPRN_2X32((IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData1))),
									(IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData1))), 3, 1) ;

							xb_int64pr qmulScalar5 = IVP_DEXTRPRN_2X32((IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData3))),
									(IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData3))), 3, 1) ;

							xb_int64pr qmulScalar6 = IVP_DEXTRPRN_2X32((IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData5))),
									(IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData5))), 3, 1) ;

							//h=0
							IVP_DMULQA2N8XR8(daccSum2, daccSum1, dvecCoeff8, dvecCoeff7, dvecCoeff6, dvecCoeff5, qmulScalar4);
							//h=1
							IVP_DMULQA2N8XR8(daccSum4, daccSum3, dvecCoeff8, dvecCoeff7, dvecCoeff6, dvecCoeff5, qmulScalar5);
							//h=2
							IVP_DMULQA2N8XR8(daccSum6, daccSum5, dvecCoeff8, dvecCoeff7, dvecCoeff6, dvecCoeff5, qmulScalar6);
#else
							int32_t qmulScalar7 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData1)), 1);
							int32_t qmulScalar8 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData1)), 3);

							int32_t qmulScalar9 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData3)), 1);
							int32_t qmulScalar10 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData3)), 3);
							//h=0
							IVP_MULQA2N8XR8(daccSum1, dvecCoeff8, dvecCoeff7, dvecCoeff6, dvecCoeff5, qmulScalar7);
							IVP_MULQA2N8XR8(daccSum2, dvecCoeff8, dvecCoeff7, dvecCoeff6, dvecCoeff5, qmulScalar8);
							//h=1
							IVP_MULQA2N8XR8(daccSum3, dvecCoeff8, dvecCoeff7, dvecCoeff6, dvecCoeff5, qmulScalar9);
							IVP_MULQA2N8XR8(daccSum4, dvecCoeff8, dvecCoeff7, dvecCoeff6, dvecCoeff5, qmulScalar10);

#endif

						} /* End (Input Channel * Kernel Width) */
					}
					else // number of input channels multiples of 16
					{
						for (loopCount = 0; loopCount < (numIter); loopCount += 2*XCHAL_IVPN_SIMD_WIDTH) /* Input Channel * Kernel Width */
						{
							/* Aligning variable vector load of pixels */

							//1st 4 widths of height0
							dvecData1 = *pdvecData1++;
							dvecData2 = *pdvecData2++;
#ifdef IVP_DMULQA2N8XR8
							xb_int64pr qmulScalar1 = IVP_DEXTRPRN_2X32((IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData2))),
									(IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData1))), 0, 0) ;

							xb_int64pr qmulScalar3 = IVP_DEXTRPRN_2X32((IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData2))),
									(IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData1))), 1, 1) ;

							xb_int64pr qmulScalar5 = IVP_DEXTRPRN_2X32((IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData2))),
									(IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData1))), 2, 2) ;

							xb_int64pr qmulScalar7 = IVP_DEXTRPRN_2X32((IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData2))),
									(IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData1))), 3, 3) ;
#else

							int32_t qmulScalar1 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData1)), 0);
							int32_t qmulScalar2 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData2)), 0);

							int32_t qmulScalar3 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData1)), 1);
							int32_t qmulScalar4 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData2)), 1);


							int32_t qmulScalar5 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData1)),  2);
							int32_t qmulScalar6 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData2)), 2);

							int32_t qmulScalar7 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData1)),  3);
							int32_t qmulScalar8 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData2)), 3);


#endif
							/* Aligned Vector Loads of coefficients */
							IVP_LV2NX8_XP(dvecCoeff1, pdvecCoeff, coeffDataPitch1);
							IVP_LV2NX8_XP(dvecCoeff2, pdvecCoeff, coeffDataPitch1);
							IVP_LV2NX8_XP(dvecCoeff3, pdvecCoeff, coeffDataPitch1);
							IVP_LV2NX8_XP(dvecCoeff4, pdvecCoeff, coeffDataPitch1);

#ifdef IVP_DMULQA2N8XR8
							IVP_DMULQA2N8XR8(daccSum2, daccSum1, dvecCoeff4, dvecCoeff3, dvecCoeff2, dvecCoeff1, qmulScalar1);
#else
					        IVP_MULQA2N8XR8(daccSum1, dvecCoeff4, dvecCoeff3, dvecCoeff2, dvecCoeff1, qmulScalar1);
							IVP_MULQA2N8XR8(daccSum2, dvecCoeff4, dvecCoeff3, dvecCoeff2, dvecCoeff1, qmulScalar2);
#endif

							/* Aligned Vector Loads of coefficients */
							IVP_LV2NX8_XP(dvecCoeff5, pdvecCoeff, coeffDataPitch1);
							IVP_LV2NX8_XP(dvecCoeff6, pdvecCoeff, coeffDataPitch1);
							IVP_LV2NX8_XP(dvecCoeff7, pdvecCoeff, coeffDataPitch1);
							IVP_LV2NX8_XP(dvecCoeff8, pdvecCoeff, coeffDataPitch1);
#ifdef IVP_DMULQA2N8XR8
							IVP_DMULQA2N8XR8(daccSum2, daccSum1, dvecCoeff8, dvecCoeff7, dvecCoeff6, dvecCoeff5, qmulScalar3);
#else
					        IVP_MULQA2N8XR8(daccSum1, dvecCoeff8, dvecCoeff7, dvecCoeff6, dvecCoeff5, qmulScalar3);
							IVP_MULQA2N8XR8(daccSum2, dvecCoeff8, dvecCoeff7, dvecCoeff6, dvecCoeff5, qmulScalar4);
#endif
							/* Aligned Vector Loads of coefficients */
							/* Aligned Vector Loads of coefficients */
							IVP_LV2NX8_XP(dvecCoeff9, pdvecCoeff, coeffDataPitch1);
							IVP_LV2NX8_XP(dvecCoeff10, pdvecCoeff, coeffDataPitch1);
							IVP_LV2NX8_XP(dvecCoeff11, pdvecCoeff, coeffDataPitch1);
							IVP_LV2NX8_XP(dvecCoeff12, pdvecCoeff, coeffDataPitch1);

#ifdef IVP_DMULQA2N8XR8
							IVP_DMULQA2N8XR8(daccSum2, daccSum1, dvecCoeff12, dvecCoeff11, dvecCoeff10, dvecCoeff9, qmulScalar5);
#else
						    IVP_MULQA2N8XR8(daccSum1, dvecCoeff12, dvecCoeff11, dvecCoeff10, dvecCoeff9, qmulScalar5);
							IVP_MULQA2N8XR8(daccSum2, dvecCoeff12, dvecCoeff11, dvecCoeff10, dvecCoeff9, qmulScalar6);
#endif
							/* Aligned Vector Loads of coefficients */
							IVP_LV2NX8_XP(dvecCoeff13, pdvecCoeff, coeffDataPitch1);
							IVP_LV2NX8_XP(dvecCoeff14, pdvecCoeff, coeffDataPitch1);
							IVP_LV2NX8_XP(dvecCoeff15, pdvecCoeff, coeffDataPitch1);
							IVP_LV2NX8_XP(dvecCoeff16, pdvecCoeff, coeffDataPitch1);

#ifdef IVP_DMULQA2N8XR8
							IVP_DMULQA2N8XR8(daccSum2, daccSum1, dvecCoeff16, dvecCoeff15, dvecCoeff14, dvecCoeff13, qmulScalar7);
#else
						    IVP_MULQA2N8XR8(daccSum1, dvecCoeff16, dvecCoeff15, dvecCoeff14, dvecCoeff13, qmulScalar7);
							IVP_MULQA2N8XR8(daccSum2, dvecCoeff16, dvecCoeff15, dvecCoeff14, dvecCoeff13, qmulScalar8);
#endif


							//2nd 4 widths of height0
							dvecData3 = *pdvecData3++;
							dvecData4 = *pdvecData4++;
#ifdef IVP_DMULQA2N8XR8
							xb_int64pr qmulScalar9 = IVP_DEXTRPRN_2X32((IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData4))),
									(IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData3))), 0, 0) ;


							xb_int64pr qmulScalar11 = IVP_DEXTRPRN_2X32((IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData4))),
									(IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData3))), 1, 1) ;

							xb_int64pr qmulScalar13 = IVP_DEXTRPRN_2X32((IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData4))),
									(IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData3))), 2, 2) ;


							xb_int64pr qmulScalar15 = IVP_DEXTRPRN_2X32((IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData4))),
									(IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData3))), 3, 3) ;

							/* Aligned Vector Loads of coefficients */
							IVP_DMULQA2N8XR8(daccSum4, daccSum3, dvecCoeff4, dvecCoeff3, dvecCoeff2,    dvecCoeff1, qmulScalar9);
							IVP_DMULQA2N8XR8(daccSum4, daccSum3, dvecCoeff8, dvecCoeff7, dvecCoeff6,    dvecCoeff5, qmulScalar11);
							IVP_DMULQA2N8XR8(daccSum4, daccSum3, dvecCoeff12, dvecCoeff11, dvecCoeff10, dvecCoeff9, qmulScalar13);
							IVP_DMULQA2N8XR8(daccSum4, daccSum3, dvecCoeff16, dvecCoeff15, dvecCoeff14, dvecCoeff13, qmulScalar15);

#else

						   int32_t qmulScalar9 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData3)), 0);
							int32_t qmulScalar10 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData4)), 0);

							int32_t qmulScalar11 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData3)), 1);
							int32_t qmulScalar12 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData4)), 1);


							int32_t qmulScalar13 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData3)),  2);
							int32_t qmulScalar14 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData4)), 2);

							int32_t qmulScalar15 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData3)),  3);
							int32_t qmulScalar16 = IVP_EXTRN_2X32(IVP_MOVN_2X32_FROMNX16 \
								(IVP_MOVNX16_FROM2NX8(dvecData4)), 3);


						    IVP_MULQA2N8XR8(daccSum3, dvecCoeff4, dvecCoeff3, dvecCoeff2,    dvecCoeff1, qmulScalar9);
							IVP_MULQA2N8XR8(daccSum4, dvecCoeff4, dvecCoeff3, dvecCoeff2,    dvecCoeff1, qmulScalar10);
							IVP_MULQA2N8XR8(daccSum3, dvecCoeff8, dvecCoeff7, dvecCoeff6,    dvecCoeff5, qmulScalar11);
							IVP_MULQA2N8XR8(daccSum4, dvecCoeff8, dvecCoeff7, dvecCoeff6,    dvecCoeff5, qmulScalar12);
							IVP_MULQA2N8XR8(daccSum3, dvecCoeff12, dvecCoeff11, dvecCoeff10, dvecCoeff9, qmulScalar13);
							IVP_MULQA2N8XR8(daccSum4, dvecCoeff12, dvecCoeff11, dvecCoeff10, dvecCoeff9, qmulScalar14);
							IVP_MULQA2N8XR8(daccSum3, dvecCoeff16, dvecCoeff15, dvecCoeff14, dvecCoeff13, qmulScalar15);
							IVP_MULQA2N8XR8(daccSum4, dvecCoeff16, dvecCoeff15, dvecCoeff14, dvecCoeff13, qmulScalar16);

#endif

#ifdef IVP_DMULQA2N8XR8
							//3rd 4 widths of height0
							dvecData5 = *pdvecData5++;
							dvecData6 = *pdvecData6++;
							xb_int64pr qmulScalar2 = IVP_DEXTRPRN_2X32((IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData6))),
									(IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData5))), 0, 0) ;


							xb_int64pr qmulScalar4 = IVP_DEXTRPRN_2X32((IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData6))),
									(IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData5))), 1, 1) ;

							xb_int64pr qmulScalar6 = IVP_DEXTRPRN_2X32((IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData6))),
									(IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData5))), 2, 2) ;


							xb_int64pr qmulScalar8 = IVP_DEXTRPRN_2X32((IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData6))),
									(IVP_MOVN_2X32_FROMNX16(IVP_MOVNX16_FROM2NX8(dvecData5))), 3, 3) ;

							/* Aligned Vector Loads of coefficients */
							IVP_DMULQA2N8XR8(daccSum6, daccSum5, dvecCoeff4, dvecCoeff3, dvecCoeff2,    dvecCoeff1, qmulScalar2);
							IVP_DMULQA2N8XR8(daccSum6, daccSum5, dvecCoeff8, dvecCoeff7, dvecCoeff6,    dvecCoeff5, qmulScalar4);
							IVP_DMULQA2N8XR8(daccSum6, daccSum5, dvecCoeff12, dvecCoeff11, dvecCoeff10, dvecCoeff9, qmulScalar6);
							IVP_DMULQA2N8XR8(daccSum6, daccSum5, dvecCoeff16, dvecCoeff15, dvecCoeff14, dvecCoeff13, qmulScalar8);
#endif

						} /* End (Input Channel * Kernel Width) */
					}
					inAddrOff += inDataPitch2;
					coeffAddrOff += coeffDataPitch3;
				} /* End Kernel Height */


				/* w = 0 , h = 0 */
				/* Pack, Output Scale, Output Shift and clamping */
				PACK_SCALE_AND_ROUNDING(daccSum1, dvecOut1);
				/* Store the output dvecOut1 along the output depth */
				pdvecOut = (xb_vec2Nx8*)(pOut + outCh);
				IVP_SAV2NX8_XP(dvecOut1, vaOutData, pdvecOut, remainingOutCh);
				IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
				/* w = 1 , h = 0 */
				/* Pack, Output Scale, Output Shift and clamping */
				PACK_SCALE_AND_ROUNDING(daccSum2, dvecOut2);
				/* Store the output dvecOut2 along the output depth */
				pdvecOut = (xb_vec2Nx8*)(pOut + (outCh + outDataPitch1));
				IVP_SAV2NX8_XP(dvecOut2, vaOutData, pdvecOut, remainingOutCh * numX);
				IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);

				/* w = 0 , h = 1 */
				/* Pack, Output Scale, Output Shift and clamping */
				PACK_SCALE_AND_ROUNDING(daccSum3, dvecOut3);
				/* Store the output dvecOut3 along the output depth */
				pdvecOut = (xb_vec2Nx8*)(pOut + (outCh + outDataPitch2));
				IVP_SAV2NX8_XP(dvecOut3, vaOutData, pdvecOut, remainingOutCh * numY);
				IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
				/* w = 1 , h = 1 */
				/* Pack, Output Scale, Output Shift and clamping */
				PACK_SCALE_AND_ROUNDING(daccSum4, dvecOut4);
				/* Store the output dvecOut4 along the output depth */
				pdvecOut = (xb_vec2Nx8*)(pOut + (outCh + outDataPitch1 + outDataPitch2));
				IVP_SAV2NX8_XP(dvecOut4, vaOutData, pdvecOut, remainingOutCh * numX * numY);
				IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);

#ifdef IVP_DMULQA2N8XR8
				/* w = 0 , h = 2 */
				/* Pack, Output Scale, Output Shift and clamping */
				PACK_SCALE_AND_ROUNDING(daccSum5, dvecOut5);
				/* Store the output dvecOut3 along the output depth */
				pdvecOut = (xb_vec2Nx8*)(pOut + (outCh + (2*outDataPitch2) ));
				IVP_SAV2NX8_XP(dvecOut5, vaOutData, pdvecOut, remainingOutCh * numY1);
				IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
				/* w = 1 , h = 2 */
				/* Pack, Output Scale, Output Shift and clamping */
				PACK_SCALE_AND_ROUNDING(daccSum6, dvecOut6);
				/* Store the output dvecOut4 along the output depth */
				pdvecOut = (xb_vec2Nx8*)(pOut + (outCh + outDataPitch1 + (2*outDataPitch2)));
				IVP_SAV2NX8_XP(dvecOut6, vaOutData, pdvecOut, remainingOutCh * numX * numY1);
				IVP_SAPOS2NX8_FP(vaOutData, pdvecOut);
#endif

#endif
			} /* End output width */
		} /* End output height */
	} /* End Output Channels */
	return(XI_ERROR_STATUS());
}
