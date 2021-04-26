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
#ifndef _CNNRT_DMA_H_INCLUDED_
#define _CNNRT_DMA_H_INCLUDED_

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
   DMA API
*/

/* Wait for completion of all DMA transfers */
XI_ERR_TYPE dma_barrier();

/* Wait for completion of all DMA transfers of a channel */
XI_ERR_TYPE dma_barrier_ch(int ch);

/* Asynchronous DMA transfers between local and system memory.

   Function suffix:
     1d: contiguous transfer from src to dst address of d1_cnt bytes.
     2d: 2D DMA transfer from src to dst address d2_cnt rows of d1_cnt bytes in each row.
         d2_src_pitch is the stride in bytes between each source row start.
         d2_dst_pitch is the stride in bytes between each destination row start.
     3d: 3D transfer. Performs d3_cnt 2D transfers from src to dst pointers.
         d3_src_pitch is the stride in bytes between each source of 2D transfer.
         d3_dst_pitch is the stride in bytes between each destination of 2D transfer.
     4d: 4D transfer. Performs d4_cnt 3D transfers from src to dst pointers.
         d4_src_pitch is the stride in bytes between each source of 3D transfer.
         d4_dst_pitch is the stride in bytes between each destination of 3D transfer.
     loc2sys: from local to system memory.
     loc2sys: from system to local memory.
     dyn: function does additional preprocessing of each request
          and tries to reduce dimensionality of request when
          strides allow.
     straddles: function does additional preprocessing of each request
                to detect and successfully handle requests that span
                across DRAM boundary.

     DMA transfer may start immediatly during this call.
 */

void dma_1d_loc2sys_ch(int ch,
                       void *src, void *dst, size_t d1_cnt);

void dma_1d_sys2loc_ch(int ch,
                       void *src, void *dst, size_t d1_cnt);

void dma_2d_loc2sys_ch(int ch,
                       void *src, void *dst, size_t d1_cnt,
                       int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt);

void dma_2d_sys2loc_ch(int ch,
                       void *src, void *dst, size_t d1_cnt,
                       int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt);

void dma_3d_loc2sys_ch(int ch,
                       void *src, void *dst, size_t d1_cnt,
                       int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                       int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt);

void dma_3d_sys2loc_ch(int ch,
                       void *src, void *dst, size_t d1_cnt,
                       int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                       int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt);

void dma_4d_loc2sys_ch(int ch,
                       void *src, void *dst, size_t d1_cnt,
                       int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                       int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt,
                       int d4_src_pitch, int d4_dst_pitch, size_t d4_cnt);

void dma_4d_sys2loc_ch(int ch,
                       void *src, void *dst, size_t d1_cnt,
                       int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                       int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt,
                       int d4_src_pitch, int d4_dst_pitch, size_t d4_cnt);

/* DMA requests that dynamically reduce their dimensionality when strides allow */

void dma_2d_loc2sys_dyn_ch(int ch,
                           void *src, void *dst, size_t d1_cnt,
                           int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt);

void dma_2d_sys2loc_dyn_ch(int ch,
                           void *src, void *dst, size_t d1_cnt,
                           int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt);

void dma_3d_loc2sys_dyn_ch(int ch,
                           void *src, void *dst, size_t d1_cnt,
                           int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                           int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt);

void dma_3d_sys2loc_dyn_ch(int ch,
                           void *src, void *dst, size_t d1_cnt,
                           int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                           int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt);

void dma_4d_loc2sys_dyn_ch(int ch,
                        void *src, void *dst, size_t d1_cnt,
                        int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                        int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt,
                        int d4_src_pitch, int d4_dst_pitch, size_t d4_cnt);

void dma_4d_sys2loc_dyn_ch(int ch,
                        void *src, void *dst, size_t d1_cnt,
                        int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                        int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt,
                        int d4_src_pitch, int d4_dst_pitch, size_t d4_cnt);

/* DMA operations that work with local memory buffers
   spanning across DRAM boundary.
*/

void dma_1d_loc2sys_straddles_ch(int ch,
                                 void *src, void *dst, size_t d1_cnt);

void dma_1d_sys2loc_straddles_ch(int ch,
                                 void *src, void *dst, size_t d1_cnt);

void dma_2d_loc2sys_straddles_ch(int ch,
                                 void *src, void *dst, size_t d1_cnt,
                                 int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt);

void dma_2d_sys2loc_straddles_ch(int ch,
                                 void *src, void *dst, size_t d1_cnt,
                                 int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt);

void dma_3d_loc2sys_straddles_ch(int ch,
                                 void *src, void *dst, size_t d1_cnt,
                                 int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                                 int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt);

void dma_3d_sys2loc_straddles_ch(int ch,
                                 void *src, void *dst, size_t d1_cnt,
                                 int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                                 int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt);

void dma_4d_loc2sys_straddles_ch(int ch,
                                 void *src, void *dst, size_t d1_cnt,
                                 int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                                 int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt,
                                 int d4_src_pitch, int d4_dst_pitch, size_t d4_cnt);

void dma_4d_sys2loc_straddles_ch(int ch,
                                 void *src, void *dst, size_t d1_cnt,
                                 int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                                 int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt,
                                 int d4_src_pitch, int d4_dst_pitch, size_t d4_cnt);

/* DMA requests that dynamically reduce their dimensionality if strides allow */

void dma_2d_loc2sys_dyn_straddles_ch(int ch,
                                     void *src, void *dst, size_t d1_cnt,
                                     int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt);

void dma_2d_sys2loc_dyn_straddles_ch(int ch,
                                     void *src, void *dst, size_t d1_cnt,
                                     int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt);

void dma_3d_loc2sys_dyn_straddles_ch(int ch,
                                     void *src, void *dst, size_t d1_cnt,
                                     int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                                     int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt);

void dma_3d_sys2loc_dyn_straddles_ch(int ch,
                                     void *src, void *dst, size_t d1_cnt,
                                     int d2_src_pitch, int d2_dst_pitch, size_t d2_cnt,
                                     int d3_src_pitch, int d3_dst_pitch, size_t d3_cnt);

/* DMA functions for loading compressed data */

void compressed_dma_1d_sys2loc_ch(int ch,
                                  void *src, void *dst, const void *header, unsigned tile_idx, size_t dst_size);

void compressed_dma_1d_sys2loc_straddles_ch(int ch,
                                            void *src, void *dst, const void *header, unsigned tile_idx, size_t dst_size);

/* DMA functions for fetching/storing directory indexed data */

void dir_dma_1d_sys2loc_ch(int ch,
                           void *src, void *dst, const void *header, unsigned tile_idx, size_t size);

void dir_dma_1d_sys2loc_straddles_ch(int ch,
                                     void *src, void *dst, const void *header, unsigned tile_idx, size_t size);

void dir_dma_1d_loc2sys_ch(int ch,
                           void *src, void *dst, const void *header, unsigned tile_idx, size_t size);

void dir_dma_1d_loc2sys_straddles_ch(int ch,
                                     void *src, void *dst, const void *header, unsigned tile_idx, size_t size);


/*
 * Old single channel API
 */

#define dma_1d_loc2sys(src, dst, d1_cnt) \
    dma_1d_loc2sys_ch(0, src, dst, d1_cnt)
#define dma_1d_sys2loc(src, dst, d1_cnt) \
    dma_1d_sys2loc_ch(0, src, dst, d1_cnt)
#define dma_2d_loc2sys(src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt) \
    dma_2d_loc2sys_ch(0, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt)
#define dma_2d_sys2loc(src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt) \
    dma_2d_sys2loc_ch(0, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt)
#define dma_3d_loc2sys(src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt) \
    dma_3d_loc2sys_ch(0, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt)
#define dma_3d_sys2loc(src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt) \
    dma_3d_sys2loc_ch(0, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt)
#define dma_4d_loc2sys(src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt, d4_src_pitch, d4_dst_pitch, d4_cnt) \
    dma_4d_loc2sys_ch(0, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt, d4_src_pitch, d4_dst_pitch, d4_cnt)
#define dma_4d_sys2loc(src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt, d4_src_pitch, d4_dst_pitch, d4_cnt) \
    dma_4d_sys2loc_ch(0, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt, d4_src_pitch, d4_dst_pitch, d4_cnt)

#define dma_2d_loc2sys_dyn(src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt) \
    dma_2d_loc2sys_dyn_ch(0, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt)
#define dma_2d_sys2loc_dyn(src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt) \
    dma_2d_sys2loc_dyn_ch(0, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt)
#define dma_3d_loc2sys_dyn(src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt) \
    dma_3d_loc2sys_dyn_ch(0, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt)
#define dma_3d_sys2loc_dyn(src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt) \
    dma_3d_sys2loc_dyn_ch(0, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt)
#define dma_4d_loc2sys_dyn(src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt, d4_src_pitch, d4_dst_pitch, d4_cnt) \
    dma_4d_loc2sys_dyn_ch(0, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt, d4_src_pitch, d4_dst_pitch, d4_cnt)
#define dma_4d_sys2loc_dyn(src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt, d4_src_pitch, d4_dst_pitch, d4_cnt) \
    dma_4d_sys2loc_dyn_ch(0, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt, d4_src_pitch, d4_dst_pitch, d4_cnt)

#define dma_1d_loc2sys_straddles(src, dst, d1_cnt) \
    dma_1d_loc2sys_straddles_ch(0, src, dst, d1_cnt)
#define dma_1d_sys2loc_straddles(src, dst, d1_cnt) \
    dma_1d_sys2loc_straddles_ch(0, src, dst, d1_cnt)
#define dma_2d_loc2sys_straddles(src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt) \
    dma_2d_loc2sys_straddles_ch(0, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt)
#define dma_2d_sys2loc_straddles(src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt) \
    dma_2d_sys2loc_straddles_ch(0, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt)
#define dma_3d_loc2sys_straddles(src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt) \
    dma_3d_loc2sys_straddles_ch(0, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt)
#define dma_3d_sys2loc_straddles(src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt) \
    dma_3d_sys2loc_straddles_ch(0, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt)
#define dma_4d_loc2sys_straddles(src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt, d4_src_pitch, d4_dst_pitch, d4_cnt) \
    dma_4d_loc2sys_straddles_ch(0, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt, d4_src_pitch, d4_dst_pitch, d4_cnt)
#define dma_4d_sys2loc_straddles(src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt, d4_src_pitch, d4_dst_pitch, d4_cnt) \
    dma_4d_sys2loc_straddles_ch(0, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt, d4_src_pitch, d4_dst_pitch, d4_cnt)

#define dma_2d_loc2sys_dyn_straddles(src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt) \
    dma_2d_loc2sys_dyn_straddles_ch(0, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt)
#define dma_2d_sys2loc_dyn_straddles(src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt) \
    dma_2d_sys2loc_dyn_straddles_ch(0, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt)
#define dma_3d_loc2sys_dyn_straddles(src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt) \
    dma_3d_loc2sys_dyn_straddles_ch(0, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt)
#define dma_3d_sys2loc_dyn_straddles(src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt) \
    dma_3d_sys2loc_dyn_straddles_ch(0, src, dst, d1_cnt, d2_src_pitch, d2_dst_pitch, d2_cnt, d3_src_pitch, d3_dst_pitch, d3_cnt)

#define compressed_dma_1d_sys2loc(src, dst, header, tile_idx, dst_size) \
    compressed_dma_1d_sys2loc_ch(0, src, dst, header, tile_idx, dst_size)
#define compressed_dma_1d_sys2loc_straddles(src, dst, header, tile_idx, dst_size) \
    compressed_dma_1d_sys2loc_straddles_ch(0, src, dst, header, tile_idx, dst_size)

#define dir_dma_1d_sys2loc(src, dst, header, tile_idx, size) \
    dir_dma_1d_sys2loc_ch(0, src, dst, header, tile_idx, size)
#define dir_dma_1d_sys2loc_straddles(src, dst, header, tile_idx, size) \
    dir_dma_1d_sys2loc_straddles_ch(0, src, dst, header, tile_idx, size)
#define dir_dma_1d_loc2sys(src, dst, header, tile_idx, size) \
    dir_dma_1d_loc2sys_ch(0, src, dst, header, tile_idx, size)
#define dir_dma_1d_loc2sys_straddles(src, dst, header, tile_idx, size) \
    dir_dma_1d_loc2sys_straddles_ch(0, src, dst, header, tile_idx, size)

#ifdef __cplusplus
}
#endif

#endif /* _CNNRT_DMA_H_INCLUDED_ */

