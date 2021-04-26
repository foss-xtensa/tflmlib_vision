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
#ifndef _CNNRT_COMPRESSION_SUPPORT_H_INCLUDED_
#define _CNNRT_COMPRESSION_SUPPORT_H_INCLUDED_

#include <stdlib.h>
#include <stdint.h>
#include "cnnrt_helpers.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct {
    int32_t   offset;
    int32_t   size;
} dir_entry_t;

/* Coefficients compression functions.

Params:
   dst - destination buffer. Pass NULL to compute only compressed
         size without writing data out.
   src - data to compress.
   size - size of source data in bytes.
   block_size - compression block size (i.e. tile size).

Returns:
   Size of compressed data.
*/

size_t compress_p6(void *dst, const void *src, size_t size, size_t block_size);
size_t compress_q6(void *dst, const void *src, size_t size, size_t block_size);
size_t compress_c5(void *dst, const void *src, size_t size, size_t block_size);

/* Coefficients reordering function for hardware. */
size_t reorder_coeff_xnne_hw(void *pDst,
                             const int8_t *pCoeffData,
                             const int32_t *pBiasData,
                             const uint16_t *pScaleData,
                             const uint8_t *pShiftData,
                             int kernelW,
                             int kernelH,
                             int kernelD,
                             int N,
                             int tileD,
                             int tileA,
                             int tileN,
                             int SM,
                             int NMBLK,
                             int STW,
                             int STH,
                             int PAD_L,
                             int PAD_T,
                             int compress_coeff_to_4bit,
                             int always_include_headers,
                             const int32_t *pEncodingTable);

/* Coefficients reordering function for hardware, uses compression. */
size_t reorder_coeff_xnne_hw_comp(void *pDst,
                             const int8_t *pCoeffData,
                             const int32_t *pBiasData,
                             const uint16_t *pScaleData,
                             const uint8_t *pShiftData,
                             int kernelW,
                             int kernelH,
                             int kernelD,
                             int N,
                             int tileD,
                             int tileA,
                             int tileN,
                             int SM,
                             int NMBLK,
                             int STW,
                             int STH,
                             int PAD_L,
                             int PAD_T,
                             int compress_coeff_to_4bit,
                             int always_include_headers,
                             const int32_t *pEncodingTable);

/* slopes expansion function for PReLU on VPU for XNNE. */
int slopes_expand_xnne(int16_t *result,
                       int32_t res_size,
                       const int16_t *values,
                       int32_t values_size);

/* scales expansion function for RenormVQ on VPU for XNNE. */
int scales_expand_xnne(uint16_t *result,
                       int32_t res_size,
                       const uint16_t *values,
                       int32_t values_size);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _CNNRT_COMPRESSION_SUPPORT_H_INCLUDED_ */
