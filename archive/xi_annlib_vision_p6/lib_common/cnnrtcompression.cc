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
#include "cnnrt.h"

#if XCHAL_HAVE_VISION == 1 && XCHAL_VISION_TYPE >= 6

#  if XCHAL_HAVE_NX == 1

/* Q6 version */

static void unsqz(uint8_t *pOut, const uint8_t *pIn, const uint8_t *pMask, int unsqzed_size)
{
#if CSTUB_BUILD
    /* Checking alignment and abort execution if it's incorrect */
    if (((uintptr_t)pOut & 63) != 0 || ((uintptr_t)pMask & 7) != 0)
        printf("Alignment violation in decompression unsqz: pOut %x, pMask %x", (uintptr_t)pOut,  (uintptr_t)pMask);
#endif
    const xb_vec2Nx8U * __restrict pInVec = (const xb_vec2Nx8U *)pIn;
    const vbool2N * __restrict pMaskVb = (const vbool2N *)pMask;
    xb_vec2Nx8U * __restrict pOutVec = (xb_vec2Nx8U *)pOut;
    valign vaIn;
    xb_vec2Nx8U vSel, vecIn, vecOut;
    vbool2N vbMask;
    int i, count;

    vaIn = IVP_LA2NX8U_PP(pInVec);
    for (i=0; i<unsqzed_size; i+=64) {
        vbMask = *pMaskVb++;
        IVP_UNSQZ2N(vSel, count, vbMask);  // select pattern for non zero coeffs
        IVP_LA2NX8U_XP(vecIn, vaIn, pInVec, count);
        vSel = IVP_MOV2NX8UT(vSel, (xb_vec2Nx8U)64, vbMask);  // pattern to insert zeros in coeff stream
        vecOut = IVP_SEL2NX8U((xb_vec2Nx8U)0, vecIn, vSel); // decompress coeffs
        *pOutVec++ = vecOut;
    }
}

#  else

/* P6 version */

static void unsqz(uint8_t *pOut, const uint8_t *pIn, const uint8_t *pMask, int unsqzed_size)
{
#if CSTUB_BUILD
    /* Checking alignment and abort execution if it's incorrect */
    if (((uintptr_t)pOut & 63) != 0 || ((uintptr_t)pMask & 3) != 0)
        printf("Alignment violation in decompression unsqz: pOut %x, pMask %x", (uintptr_t)pOut,  (uintptr_t)pMask);
#endif
    const xb_vecNx8U * __restrict pInVec = (const xb_vecNx8U *)pIn;
    const vboolN * __restrict pMaskVb = (const vboolN *)pMask;
    xb_vecNx8U * __restrict pOutVec = (xb_vecNx8U *)pOut;
    valign vaIn;
    xb_vecNx16 vSel, vecIn, vecOut;
    vboolN vbMask;
    int i, count;

    vaIn = IVP_LANX8U_PP(pInVec);
    for (i=0; i<unsqzed_size; i+=32) {
        vbMask = *pMaskVb++;
        IVP_UNSQZN(vSel, count, vbMask);  // select pattern for non zero coeffs
        IVP_LANX8U_XP(vecIn, vaIn, pInVec, count >> 1);
        vSel = IVP_MOVNX16T(vSel, (xb_vecNx16)32, vbMask);  // pattern to insert zeros in coeff stream
        vecOut = IVP_SELNX16((xb_vecNx16)0, vecIn, vSel); // decompress coeffs
        *pOutVec++ = vecOut;
    }
}

#  endif

#elif XCHAL_HAVE_VISIONC == 1

/* C5 version */

static void unsqz(uint8_t *pOut, const uint8_t *pIn, const uint8_t *pCount, int unsqzed_size)
{
#if CSTUB_BUILD
    /* Checking alignment and abort execution if it's incorrect */
    if (((uintptr_t)pOut & 63) != 0)
        printf("Alignment violation in decompression unsqz: pOut %x", (uintptr_t)pOut);
#endif
    const xb_vec2Nx8 * __restrict pInVec = (const xb_vec2Nx8 *)pIn;
    const xb_vec2Nx8 * __restrict pCountVec = (const xb_vec2Nx8 *)pCount;
    xb_vec4Nx8 * __restrict pOutVec = (xb_vec4Nx8 *)pOut;
    valign vaIn, vaCount;
    xb_vec2Nx8 vecH, vecL, vecLen;
    xb_vec4Nx8 vecOut;
    int i, bytes;

    vaIn = IVP_LA2NX8_PP(pInVec);
    vaCount = IVP_LA2NX8_PP(pCountVec);
    for (i=0; i<unsqzed_size; i += 128) {
        IVP_LAV2NX8_XP(vecLen, vaCount, pCountVec, 1);
        bytes = IVP_EXTRN_2X32(vecLen, 0);
        IVP_LAA2NX8_X4L(vecL, vaIn, pInVec, bytes, 0);
        IVP_LAA2NX8_XP4H(vecH, vaIn, pInVec, bytes, 0);
        vecOut = IVP_DCMPRS4NX8(IVP_MOV4NX8_FROM2NX8(vecH, vecL), bytes, 0);
        *pOutVec++ = vecOut;
    }
}

#endif /* C5 */

/*
 * Coefficient fetching and decompression library functions.
 */

void compressed_transfer_info(void *src, void *dst, const void *header, unsigned tile_idx, size_t dst_size,
                              void **compressed_src, void **compressed_dst, size_t *compressed_data_size)
{
    uint32_t *header_ptr = (uint32_t *)header;
    uint32_t compressed_data_offset = header_ptr[tile_idx];
    *compressed_data_size = header_ptr[tile_idx + 1] - compressed_data_offset;
    *compressed_src = (uint8_t *)src + compressed_data_offset;
    *compressed_dst = (uint8_t *)dst + dst_size - *compressed_data_size;
}

#if XCHAL_HAVE_VISION == 1 && XCHAL_VISION_TYPE >= 6
#  if XCHAL_HAVE_NX == 1
#    define DATA_SIZE_ROUNDUP 64
#  else
#    define DATA_SIZE_ROUNDUP 32
#  endif
#  define DATA_SIZE_DIVISOR 8
#elif XCHAL_HAVE_VISIONC == 1
#  define DATA_SIZE_ROUNDUP 128
#  define DATA_SIZE_DIVISOR 128
#endif /* C5 or P6/Q6*/

void decompress_coeff(void *buff, size_t buff_size, unsigned tile_idx, const void *header, size_t data_size)
{
    size_t compressed_data_size;
    void *compressed_src, *compressed_dst;
    uint8_t *bit_mask;

    data_size = cnnrt_align_up(data_size, DATA_SIZE_ROUNDUP);
    compressed_transfer_info(NULL, buff, header, tile_idx, buff_size,
                             &compressed_src, &compressed_dst, &compressed_data_size);
    bit_mask = (uint8_t *)compressed_dst + compressed_data_size - data_size / DATA_SIZE_DIVISOR;
    unsqz((uint8_t *)buff, (const uint8_t *)compressed_dst, bit_mask, data_size);
}


XI_ERR_TYPE dir_init_plain(dir_entry_t *pDir, size_t buffer_size, int element_size,
                           int mtile_D, int mtile_W, int mtile_H, int mtile_M,
                           int num_D, int num_W, int num_H, int num_M)
{
    XI_ERROR_CHECKS() {
        XI_RUN_TIME_CHECK(sizeof(dir_entry_t) * num_D * num_W * num_H * num_M <= buffer_size, "Buffer is too small", XI_ERR_BADARG);
    }
    uint32_t size = mtile_D * mtile_W * mtile_H * mtile_M * element_size;
    // Starting from 0 offset because this header is never saved into blob
    int32_t offset = 0;
    for (int i = 0; i < num_D * num_W * num_H * num_M; i++) {
        pDir[i].offset = offset;
        pDir[i].size = size;
        offset += size;
    }
    return XI_ERROR_STATUS();
}

XI_ERR_TYPE dir_init_compressed(dir_entry_t *pDir, size_t buffer_size, int element_size,
                                int mtile_D, int mtile_W, int mtile_H, int mtile_M,
                                int num_D, int num_W, int num_H, int num_M)
{
    size_t dir_size = sizeof(dir_entry_t) * num_D * num_W * num_H * num_M;
    XI_ERROR_CHECKS() {
        XI_RUN_TIME_CHECK(dir_size <= buffer_size, "Buffer is too small", XI_ERR_BADARG);
    }
    uint32_t size = mtile_D * mtile_W * mtile_H * mtile_M * element_size;
    // account for compression overhead
    size = cnnrt_align_up(((size + 127) / 128) * 129, 128);
    // Reserve space for this blob
    int32_t offset = cnnrt_align_up(dir_size, 128);
    for (int i = 0; i < num_D * num_W * num_H * num_M; i++) {
        pDir[i].offset = offset;
        pDir[i].size = size;
        offset += size;
    }

    return XI_ERROR_STATUS();
}

