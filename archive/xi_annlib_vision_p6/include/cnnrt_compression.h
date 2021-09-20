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
#ifndef _CNNRT_COMPRESSION_H_INCLUDED_
#define _CNNRT_COMPRESSION_H_INCLUDED_

#include <stdlib.h>
#include <stdint.h>
#include "cnnrt_compression_support.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*
 *  Coefficient decompression library functions.
 */

void compressed_transfer_info(void *src, void *dst, const void *header, unsigned tile_idx, size_t dst_size,
                              void **compressed_src, void **compressed_dst, size_t *compressed_data_size);

void decompress_coeff(void *buff, size_t size, unsigned tile_idx, const void *header, size_t data_size);

/*
 *  Dictionary initializatation functions.
 */

XI_ERR_TYPE dir_init_plain(dir_entry_t *pDir, size_t buffer_size, int element_size,
                           /* Dimensions of a MEMTILE in elements */
                           int mtile_D, int mtile_W, int mtile_H, int mtile_M,
                           /* Number of MEMTILEs in eadh dimension */
                           int num_D, int num_W, int num_H, int num_M);

XI_ERR_TYPE dir_init_compressed(dir_entry_t *pDir, size_t buffer_size, int element_size,
                                /* Dimensions of a MEMTILE in elements */
                                int mtile_D, int mtile_W, int mtile_H, int mtile_M,
                                /* Number of MEMTILEs in eadh dimension */
                                int num_D, int num_W, int num_H, int num_M);

static inline uintptr_t dir_addr(const dir_entry_t *pDir, void *buffer, int index)
{
    return (uintptr_t)((uint8_t*)buffer + pDir[index].offset);
}

static inline int32_t dir_size(const dir_entry_t *pDir, int index)
{
    return pDir[index].size;
}

static inline uintptr_t dir_size_slot_ptr(dir_entry_t *pDir, int index)
{
    return (uintptr_t)&pDir[index].size;
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* _CNNRT_COMPRESSION_H_INCLUDED_ */
