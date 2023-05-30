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
#ifndef __XI_ADDRESS_TRANSLATION_H__
#define __XI_ADDRESS_TRANSLATION_H__


#define OFFSET_PTR_N_2X32U(ptr, nrows, stride, in_row_offset) ((xb_vecN_2x32Uv*)((uint32_t*)(ptr)+(in_row_offset)+(nrows)*(stride)))
#define OFFSET_PTR_N_2X32( ptr, nrows, stride, in_row_offset) ((xb_vecN_2x32v*) ((int32_t*) (ptr)+(in_row_offset)+(nrows)*(stride)))
#define OFFSET_PTR_NX16U(  ptr, nrows, stride, in_row_offset) ((xb_vecNx16U*)   ((uint16_t*)(ptr)+(in_row_offset)+(nrows)*(stride)))
#define OFFSET_PTR_NX16(   ptr, nrows, stride, in_row_offset) ((xb_vecNx16*)    ((int16_t*) (ptr)+(in_row_offset)+(nrows)*(stride)))
#define OFFSET_PTR_N_2X16( ptr, nrows, stride, in_row_offset) ((xb_vecN_2x16*)  ((int16_t*) (ptr)+(in_row_offset)+(nrows)*(stride)))
#define OFFSET_PTR_NX8U(   ptr, nrows, stride, in_row_offset) ((xb_vecNx8U*)    ((uint8_t*) (ptr)+(in_row_offset)+(nrows)*(stride)))
#define OFFSET_PTR_NX8(    ptr, nrows, stride, in_row_offset) ((xb_vecNx8*)     ((int8_t*)  (ptr)+(in_row_offset)+(nrows)*(stride)))
#define OFFSET_PTR_2NX8U(  ptr, nrows, stride, in_row_offset) ((xb_vec2Nx8U*)   ((uint8_t*) (ptr)+(in_row_offset)+(nrows)*(stride)))
#define OFFSET_PTR_2NX8(   ptr, nrows, stride, in_row_offset) ((xb_vec2Nx8*)    ((int8_t*)  (ptr)+(in_row_offset)+(nrows)*(stride)))

#ifndef XI_ADDRESS_TRANSLATION
#   define XI_IVP_LVNX8U_X(a, b, c, d) IVP_LVNX8U_X(a, (b) * (c) + (d))
#   define XI_IVP_LVNX8U_XP(a, b, c) IVP_LVNX8U_XP(a, b, c)
#   define XI_IVP_LVNX8S_X(a, b, c, d) IVP_LVNX8S_X(a, (b) * (c) + (d))
#   define XI_IVP_LVNX8S_XP(a, b, c) IVP_LVNX8S_XP(a, b, c)
#   define XI_IVP_LV2NX8_XP(a, b, c) IVP_LV2NX8_XP(a, b, c)
#   define XI_IVP_LV2NX8U_X(a, b, c, d) IVP_LV2NX8U_X(a, (b) * (c) + (d))
#   define XI_IVP_LV2NX8U_XP(a, b, c) IVP_LV2NX8U_XP(a, b, c)
#   define XI_IVP_L2U2NX8_XP(a, b, c) IVP_L2U2NX8_XP(a, b, c)
#   define XI_IVP_LVNX16_X(a, b, c, d) IVP_LVNX16_X(a, (b) * (c) + (d))
#   define XI_IVP_LVNX16_XP(a, b, c) IVP_LVNX16_XP(a, b, c)
#   define XI_IVP_LVNX16U_XP(a, b, c) IVP_LVNX16U_XP(a, b, c)
#   define XI_IVP_LVN_2X32_XP(a, b, c) IVP_LVN_2X32_XP(a, b, c)
#   define XI_IVP_LVN_2X32U_XP(a, b, c) IVP_LVN_2X32U_XP(a, b, c)
#   define XI_IVP_LSNX8U_XP(a, b, c) IVP_LSNX8U_XP(a, b, c)
#   define XI_IVP_LSNX16_XP(a, b, c) IVP__LSNX16_XP(a, b, c)
#   define XI_IVP_LSR2NX8U_X(a, b) IVP_LSR2NX8U_X(a, b)
#   define XI_IVP_LSNX8U_X(a, b) IVP_LSNX8U_X(a, b)
#   define XI_IVP_LSRNX16_X(a, b) IVP_LSRNX16_X(a, b)
#   define XI_IVP_LSRNX16_XP(v, a, b) IVP_LSRNX16_XP(v, a, b)
#   define XI_IVP_LSNX16_X(a, b) IVP_LSNX16_X(a, b)
#   define XI_IVP_LSRN_2X32U_XP(a, b, c) IVP_LSRN_2X32U_XP(a, b, c)
#   define XI_IVP_SVNX8U_XP(a, b, c) IVP_SVNX8U_XP(a, b, c)
#   define XI_IVP_SVNX8UT_XP(a, b, c, d) IVP_SVNX8UT_XP(a, b, c, d)
#   define XI_IVP_SVNX8UF_XP(a, b, c, d) IVP_SVNX8UF_XP(a, b, c, d)
#   define XI_IVP_SV2NX8_XP(a, b, c) IVP_SV2NX8_XP(a, b, c)
#   define XI_IVP_SV2NX8U_XP(a, b, c) IVP_SV2NX8U_XP(a, b, c)
#   define XI_IVP_SV2NX8UT_XP(a, b, c, d) IVP_SV2NX8UT_XP(a, b, c, d)
#   define XI_IVP_SVNX16_XP(a, b, c) IVP_SVNX16_XP(a, b, c)
#   define XI_IVP_SVNX16T_XP(a, b, c, d) IVP_SVNX16T_XP(a, b, c, d)
#   define XI_IVP_SVNX16F_XP(a, b, c, d) IVP_SVNX16F_XP(a, b, c, d)
#   define XI_IVP_SVNX16U_XP(a, b, c) IVP_SVNX16U_XP(a, b, c)
#   define XI_IVP_SVNX16UT_XP(a, b, c, d) IVP_SVNX16UT_XP(a, b, c, d)
#   define XI_IVP_SVNX16UF_XP(a, b, c, d) IVP_SVNX16UF_XP(a, b, c, d)
#   define XI_IVP_SVN_2X32_XP(a, b, c) IVP_SVN_2X32_XP(a, b, c)
#   define XI_IVP_SVN_2X32U_XP(a, b, c) IVP_SVN_2X32U_XP(a, b, c)
#   define XI_IVP_GATHERNX8U(a, b) IVP_GATHERNX8U((const unsigned char *)(a), b)
#   define XI_IVP_GATHERNX8U_V(a, b, c) IVP_GATHERNX8U_V((const unsigned char *)(a), b, c)
#   define XI_IVP_GATHERNX8UT_V(a, b, c, d) IVP_GATHERNX8UT_V((const unsigned char *)(a), b, c, d)
#   define XI_IVP_GATHERANX8U(a, b) IVP_GATHERANX8U((const unsigned char *)(a), b)
#   define XI_IVP_GATHERANX8U_V(a, b, c) IVP_GATHERANX8U_V((const unsigned char *)(a), b, c)
#   define XI_IVP_GATHERANX8UT_V(a, b, c, d) IVP_GATHERANX8UT_V((const unsigned char *)(a), b, c, d)
#   define XI_IVP_GATHERNX16(a, b) IVP_GATHERNX16((const short int *)(a), b)
#   define XI_IVP_GATHERNX16_V(a, b, c) IVP_GATHERNX16_V((const short int *)(a), b, c)
#   define XI_IVP_GATHERNX16T_V(a, b, c, d) IVP_GATHERNX16T_V((const short int *)(a), b, c, d)
#   define XI_IVP_GATHERANX16(a, b) IVP_GATHERANX16((const short int *)(a), b)
#   define XI_IVP_GATHERANX16T(a, b, c) IVP_GATHERANX16T((const short int *)(a), b, c)
#   define XI_IVP_GATHERANX16_V(a, b, c) IVP_GATHERANX16_V((const short int *)(a), b, c)
#   define XI_IVP_GATHERANX16T_V(a, b, c, d) IVP_GATHERANX16T_V((const short int *)(a), b, c, d)
#   define XI_IVP_SCATTER2NX8U_L(a, b, c) IVP_SCATTER2NX8U_L(a, (unsigned char *)(b), c)
#   define XI_IVP_SCATTER2NX8UT_L(a, b, c, d) IVP_SCATTER2NX8UT_L(a, (unsigned char *)(b), c, d)
#   define XI_IVP_SCATTER2NX8U_H(a, b, c) IVP_SCATTER2NX8U_H(a, (unsigned char *)(b), c)
#   define XI_IVP_SCATTER2NX8UT_H(a, b, c, d) IVP_SCATTER2NX8UT_H(a, (unsigned char *)(b), c, d)
#   define XI_IVP_SCATTERNX8U(a, b, c) IVP_SCATTERNX8U(a, (unsigned char *)(b), c)
#   define XI_IVP_SCATTERNX8UT(a, b, c, d) IVP_SCATTERNX8UT(a, (unsigned char *)(b), c, d)
#   define XI_IVP_SCATTERNX16(a, b, c) IVP_SCATTERNX16(a, (const short int *)(b), c)
#   define XI_IVP_SCATTERNX16T(a, b, c, d) IVP_SCATTERNX16T(a, (const short int *)(b), c, d)
#else
#   define XI_IVP_LVNX8U_X(a, b, c, d) IVP_LVNX8U_X(OFFSET_PTR_NX8U(a, b, c, d), 0)
#   define XI_IVP_LVNX8U_XP(a, b, c) a = IVP_LVNX8U_I(b, 0); b = OFFSET_PTR_NX8U(b, 1, c, 0)
#   define XI_IVP_LVNX8S_X(a, b, c, d) IVP_LVNX8S_X(OFFSET_PTR_NX8U(a, b, c, d), 0)
#   define XI_IVP_LVNX8S_XP(a, b, c) a = IVP_LVNX8S_I(b, 0); b = (xb_vecNx8*)OFFSET_PTR_NX8U(b, 1, c, 0)
#   define XI_IVP_LV2NX8_XP(a, b, c) a = IVP_LV2NX8_I(b, 0); b = OFFSET_PTR_2NX8(b, 1, c, 0)
#   define XI_IVP_LV2NX8U_X(a, b, c, d) IVP_LV2NX8U_X(OFFSET_PTR_2NX8U(a, b, c, d), 0)
#   define XI_IVP_LV2NX8U_XP(a, b, c) a = IVP_LV2NX8U_I(b, 0); b = OFFSET_PTR_2NX8U(b, 1, c, 0)
#   define XI_IVP_L2U2NX8_XP(a, b, c) IVP_L2U2NX8_XP(a, b, 0); b = OFFSET_PTR_2NX8U(b, 1, c, 0)
#   define XI_IVP_LVNX16_X(a, b, c, d) IVP_LVNX16_X((xb_vecNx16*)OFFSET_PTR_NX8U(a, b, c, d), 0)
#   define XI_IVP_LVNX16_XP(a, b, c) a = IVP_LVNX16_I(b, 0); b = (xb_vecNx16*)OFFSET_PTR_NX8U(b, 1, c, 0)
#   define XI_IVP_LVNX16U_XP(a, b, c) a = IVP_LVNX16U_I(b, 0); b = (xb_vecNx16U*)OFFSET_PTR_NX8U(b, 1, c, 0)
#   define XI_IVP_LVN_2X32_XP(a, b, c) a = IVP_LVN_2X32_I(b, 0); b = (xb_vecN_2x32v*)OFFSET_PTR_NX8U(b, 1, c, 0)
#   define XI_IVP_LSNX8U_XP(a, b, c) a = IVP_LSNX8U_I(b, 0); b = (uint8_t*)OFFSET_PTR_NX8U(b, 1, c, 0)
#   define XI_IVP_LSNX16_XP(a, b, c) a = IVP_LSNX16_I(b, 0); b = (int16_t*)OFFSET_PTR_NX8U(b, 1, c, 0)
#   define XI_IVP_LSR2NX8U_X(a, b) IVP_LSR2NX8U_I((const unsigned char*)OFFSET_PTR_2NX8U(a, 1, b, 0), 0)
#   define XI_IVP_LSNX8U_X(a, b) IVP_LSNX8U_I((const unsigned char*)OFFSET_PTR_NX8U(a, 1, b, 0), 0)
#   define XI_IVP_LSRNX16_X(a, b) IVP_LSRNX16_I((const short int*)OFFSET_PTR_NX8U(a, 1, b, 0), 0)
#   define XI_IVP_LSRNX16_XP(a, b, c) a = IVP_LSRNX16_I(b, 0); b = (int16_t*)OFFSET_PTR_NX8U(b, 1, c, 0)
#   define XI_IVP_LSNX16_X(a, b) IVP_LSNX16_I((short int*)OFFSET_PTR_NX8U(a, 1, b, 0), 0)
#   define XI_IVP_LSRN_2X32U_XP(a, b, c) a = IVP_LSRN_2X32U_I(b, 0); b = (unsigned*)OFFSET_PTR_2NX8U(b, 1, c, 0)
#   define XI_IVP_SVNX8U_XP(a, b, c) IVP_SVNX8U_I(a, b, 0); b = OFFSET_PTR_NX8U(b, 1, c, 0)
#   define XI_IVP_SVNX8UT_XP(a, b, c, d) IVP_SVNX8UT_I(a, b, 0, d); b = OFFSET_PTR_NX8U(b, 1, c, 0)
#   define XI_IVP_SVNX8UF_XP(a, b, c, d) IVP_SVNX8UF_I(a, b, 0, d); b = OFFSET_PTR_NX8U(b, 1, c, 0)
#   define XI_IVP_SV2NX8_XP(a, b, c) IVP_SV2NX8_I(a, b, 0); b = OFFSET_PTR_2NX8(b, 1, c, 0)
#   define XI_IVP_SV2NX8U_XP(a, b, c) IVP_SV2NX8U_I(a, b, 0); b = OFFSET_PTR_2NX8U(b, 1, c, 0)
#   define XI_IVP_SV2NX8UT_XP(a, b, c, d) IVP_SV2NX8UT_I(a, b, 0, d); b = OFFSET_PTR_2NX8U(b, 1, c, 0)
#   define XI_IVP_SVNX16_XP(a, b, c) IVP_SVNX16_I(a, b, 0); b = (xb_vecNx16*)OFFSET_PTR_NX8U(b, 1, c, 0)
#   define XI_IVP_SVNX16T_XP(a, b, c, d) IVP_SVNX16T_I(a, b, 0, d); b = (xb_vecNx16*)OFFSET_PTR_NX8(b, 1, c, 0)
#   define XI_IVP_SVNX16F_XP(a, b, c, d) IVP_SVNX16F_I(a, b, 0, d); b = (xb_vecNx16*)OFFSET_PTR_NX8(b, 1, c, 0)
#   define XI_IVP_SVNX16U_XP(a, b, c) IVP_SVNX16U_I(a, b, 0); b = (xb_vecNx16U*)OFFSET_PTR_NX8U(b, 1, c, 0)
#   define XI_IVP_SVNX16UT_XP(a, b, c, d) IVP_SVNX16UT_I(a, b, 0, d); b = (xb_vecNx16U*)OFFSET_PTR_NX8U(b, 1, c, 0)
#   define XI_IVP_SVNX16UF_XP(a, b, c, d) IVP_SVNX16UF_I(a, b, 0, d); b = (xb_vecNx16U*)OFFSET_PTR_NX8U(b, 1, c, 0)
#   define XI_IVP_SVN_2X32_XP(a, b, c) IVP_SVN_2X32_I(a, b, 0); b = (xb_vecN_2x32v*)OFFSET_PTR_NX8U(b, 1, c, 0)
#   define XI_IVP_SVN_2X32U_XP(a, b, c) IVP_SVN_2X32U_I(a, b, 0); b = (xb_vecN_2x32Uv*)OFFSET_PTR_NX8U(b, 1, c, 0)
#   define XI_IVP_GATHERNX8U(a, b) IVP_GATHERNX8U((const unsigned char *)(a), b)
#   define XI_IVP_GATHERNX8U_V(a, b, c) IVP_GATHERNX8U_V((const unsigned char *)(a), b, c)
#   define XI_IVP_GATHERNX8UT_V(a, b, c, d) IVP_GATHERNX8UT_V((const unsigned char *)(a), b, c, d)
#   define XI_IVP_GATHERANX8U(a, b) IVP_GATHERANX8U((const unsigned char *)(a), b)
#   define XI_IVP_GATHERANX8U_V(a, b, c) IVP_GATHERANX8U_V((const unsigned char *)(a), b, c)
#   define XI_IVP_GATHERANX8UT_V(a, b, c, d) IVP_GATHERANX8UT_V((const unsigned char *)(a), b, c, d)
#   define XI_IVP_GATHERNX16(a, b) IVP_GATHERNX16((const short int *)(a), b)
#   define XI_IVP_GATHERNX16_V(a, b, c) IVP_GATHERNX16_V((const short int *)(a), b, c)
#   define XI_IVP_GATHERNX16T_V(a, b, c, d) IVP_GATHERNX16T_V((const short int *)(a), b, c, d)
#   define XI_IVP_GATHERANX16(a, b) IVP_GATHERANX16((const short int *)(a), b)
#   define XI_IVP_GATHERANX16T(a, b, c) IVP_GATHERANX16T((const short int *)(a), b, c)
#   define XI_IVP_GATHERANX16_V(a, b, c) IVP_GATHERANX16_V((const short int *)(a), b, c)
#   define XI_IVP_GATHERANX16T_V(a, b, c, d) IVP_GATHERANX16T_V((const short int *)(a), b, c, d)
#   define XI_IVP_SCATTER2NX8U_L(a, b, c) IVP_SCATTER2NX8U_L(a, (unsigned char *)(b), c)
#   define XI_IVP_SCATTER2NX8UT_L(a, b, c, d) IVP_SCATTER2NX8UT_L(a, (unsigned char *)(b), c, d)
#   define XI_IVP_SCATTER2NX8U_H(a, b, c) IVP_SCATTER2NX8U_H(a, (unsigned char *)(b), c)
#   define XI_IVP_SCATTER2NX8UT_H(a, b, c, d) IVP_SCATTER2NX8UT_H(a, (unsigned char *)(b), c, d)
#   define XI_IVP_SCATTERNX8U(a, b, c) IVP_SCATTERNX8U(a, (unsigned char *)(b), c)
#   define XI_IVP_SCATTERNX8UT(a, b, c, d) IVP_SCATTERNX8UT(a, (unsigned char *)(b), c, d)
#   define XI_IVP_SCATTERNX16(a, b, c) IVP_SCATTERNX16(a, (const short int *)(b), c)
#   define XI_IVP_SCATTERNX16T(a, b, c, d) IVP_SCATTERNX16T(a, (const short int *)(b), c, d)
#endif

#endif
