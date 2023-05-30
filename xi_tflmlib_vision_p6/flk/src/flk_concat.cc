/*
 * Copyright (c) 2018 by Cadence Design Systems, Inc. ALL RIGHTS RESERVED. 
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of 
 * Cadence Design Systems Inc. Any rights to use, modify, and create 
 * derivative works of this file are set forth under the terms of your 
 * license agreement with Cadence Design Systems, Inc.
 */
 
#include <string.h>
#include "cnnrt.h"
#include "flk_concat.h"
#include "xi_tile3d_manager.h"
#include "flk_common.h"

#include <xtensa/tie/xt_ivpn.h>
#include <xtensa/tie/xt_misc.h>
#define IS_MULTICHANNEL_DMA USE_DMA_MULTICHANNEL

//#define DEBUG_LEVEL 2

typedef struct {
    size_t dim;
    size_t pad_low;
    size_t pad_high;
    int    src_stride;
    int    dst_stride;
} dim_t;

static void memset_I8(void* buff, int val, size_t size) {
    size_t its = size / (2 * XCHAL_IVPN_SIMD_WIDTH);
    size_t rem = size % (2 * XCHAL_IVPN_SIMD_WIDTH);
    xb_vec2Nx8 *pDst = (xb_vec2Nx8 *)buff;
    valign vaDst = IVP_ZALIGN();
    xb_vec2Nx8 pattern = IVP_LSR2NX8_I((int8_t *)&val, 0);
    for (size_t i=0; i < its; i++) {
        IVP_SAV2NX8_XP(pattern, vaDst, pDst, 2 * XCHAL_IVPN_SIMD_WIDTH);
    }
    IVP_SAV2NX8_XP(pattern, vaDst, pDst, rem);
    IVP_SAPOS2NX8_FP(vaDst, pDst);
}

static XI_ERR_TYPE copy_pad(uint8_t* buff, uint32_t pad_value, uint8_t elem_size, uint32_t total_size)
{
    switch (elem_size) {
    case 2:
        for (uint32_t i = 0; i < total_size; i++)
            ((uint16_t*)buff)[i] = (uint16_t)pad_value;
        break;
    case 4:
        for (uint32_t i = 0; i < total_size; i++)
            ((uint32_t*)buff)[i] = (uint32_t)pad_value;
        break;
    default:
       // DEBUG_LOG2("error: Invalid elemSize in copy4D()\n");
        return XI_ERR_BADARG;
    }
    return XI_ERR_OK;
}

static XI_ERR_TYPE copy4D(uint8_t *src,
                          uint8_t *dst,
                          const dim_t dims[4],
                          void *tile_buffA,
                          void *tile_buffB,
                          const size_t tile[4],
                          const int inpId,
                          const concat_params_t *params)
{
    XI_ERROR_CHECKS(){} 
    uint8_t *buff = (uint8_t *)tile_buffA;
    uint8_t *buff_next = (uint8_t *)tile_buffB;
    uint8_t *temp;
    size_t frame[4];
    int need_pad = 0;
    //uint32_t tileType;
    for (int i=0; i<4; i++) {
        frame[i] = dims[i].dim + dims[i].pad_low + dims[i].pad_high;
        need_pad |= dims[i].pad_low | dims[i].pad_high;
    }

    if (need_pad) {
        // is there concat + pad usecase?
        // PAD: padding always with zero. PAV_V2: can specify pad value
        uint32_t totalSize = tile[0] * tile[1] * tile[2] * tile[3];
        if (params->padValue == 0 || params->elemSize == 1) {
            memset_I8(buff, params->padValue, totalSize * params->elemSize);
        } else {
            if (XI_ERR_OK != copy_pad(buff, params->padValue, params->elemSize, totalSize))
                return XI_ERR_BADARG;
        }
    }

    for (size_t x3 = 0; x3 < frame[3]; x3 += tile[3]) {
        for (size_t x2 = 0; x2 < frame[2]; x2 += tile[2]) {
            for (size_t x1 = 0; x1 < frame[1]; x1 += tile[1]) {
                for (size_t x0 = 0; x0 < frame[0]; x0 += tile[0]) {
                    /* Intersection with valid data */
                    size_t coord[4] = {x0, x1, x2, x3};
                    /* Valid coordinate */
                    size_t valid[4];
                    /* Upper bound of valid area */
                    size_t bound[4];
                    /* Valid tile size */
                    size_t sizes[4];
                    /* Current tile dimensions */
                    size_t cur_tile[4];
                    /* Current tile pitch in local memory */
                    size_t pitch[4];
                    /* Only invalid data */
                    int invalid = 0;
                    for (int i=0; i<4; i++) {
                        cur_tile[i] = XT_MIN(tile[i], frame[i] - coord[i]);
                        valid[i] = XT_MAX(coord[i], dims[i].pad_low);
                        bound[i] = XT_MIN(coord[i] + cur_tile[i], frame[i] - dims[i].pad_high);
                        sizes[i] = bound[i] - valid[i];
                        invalid |= bound[i] <= valid[i];
                    }
                    pitch[0] = 1;
                    for (int i=1; i<4; i++)
                        pitch[i] = pitch[i-1] * cur_tile[i-1];

                    uint8_t *tr_src = NULL;
                    uint8_t *tr_dst = NULL;
                    if (!invalid) {
                        //uint8_t *tr_src = src;
                        //uint8_t *tr_dst = buff;
                        tr_src = src;
                        tr_dst = buff;
                        for (int i=0; i<4; i++) {
                            tr_src += dims[i].src_stride * (valid[i] - dims[i].pad_low) * params->elemSize;
                            tr_dst += pitch[i] * (valid[i] - coord[i]);
                        }

                        dma_4d_sys2loc_dyn(/* src           */ tr_src,
                                           /* dst           */ tr_dst,
                                           /* row size      */ sizes[0] * params->elemSize,
                                           /* src stride 2d */ dims[1].src_stride * params->elemSize,
                                           /* dst stride 2d */ pitch[1] * params->elemSize,
                                           /* count 2d      */ sizes[1],
                                           /* src stride 3d */ dims[2].src_stride * params->elemSize,
                                           /* dst stride 3d */ pitch[2] * params->elemSize,
                                           /* count 3d      */ sizes[2],
                                           /* src stride 4d */ dims[3].src_stride * params->elemSize,
                                           /* dst stride 4d */ pitch[3] * params->elemSize,
                                           /* count 4d      */ sizes[3]);
                    }
                    XI_CHECK_RESULT(dma_barrier());
                    //uint8_t *tr_dst = dst;
                    tr_dst = dst;
                    for (int i=0; i<4; i++) {
                        tr_dst += dims[i].dst_stride * coord[i] * params->elemSize;
                    }
#if IS_MULTICHANNEL_DMA
                    dma_4d_loc2sys_dyn_ch(/* channel       */ 1,
                                          /* src           */ buff,
                                          /* dst           */ tr_dst,
                                          /* row size      */ cur_tile[0],
                                          /* src stride 2d */ pitch[1],
                                          /* dst stride 2d */ dims[1].dst_stride,
                                          /* count 2d      */ cur_tile[1],
                                          /* src stride 3d */ pitch[2],
                                          /* dst stride 3d */ dims[2].dst_stride,
                                          /* count 3d      */ cur_tile[2],
                                          /* src stride 4d */ pitch[3],
                                          /* dst stride 4d */ dims[3].dst_stride,
                                          /* count 4d      */ cur_tile[3]);

#else
                    dma_4d_loc2sys_dyn(/* src           */ buff,
                                       /* dst           */ tr_dst,
                                       /* row size      */ cur_tile[0] * params->elemSize,
                                       /* src stride 2d */ pitch[1] * params->elemSize,
                                       /* dst stride 2d */ dims[1].dst_stride * params->elemSize,
                                       /* count 2d      */ cur_tile[1],
                                       /* src stride 3d */ pitch[2] * params->elemSize,
                                       /* dst stride 3d */ dims[2].dst_stride * params->elemSize,
                                       /* count 3d      */ cur_tile[2],
                                       /* src stride 4d */ pitch[3] * params->elemSize,
                                       /* dst stride 4d */ dims[3].dst_stride * params->elemSize,
                                       /* count 4d      */ cur_tile[3]);
#endif
		            if (need_pad) {
                        uint32_t totalSize = tile[0] * tile[1] * tile[2] * tile[3];
                        if (params->padValue == 0 || params->elemSize == 1) {
                            memset_I8(buff_next, params->padValue, totalSize * params->elemSize);
                        } else {
                            copy_pad(buff_next, params->padValue,  params->elemSize, totalSize);
                        }
                    }
                    temp = buff; buff = buff_next; buff_next = temp;
                }
            }
        }
    }
#if IS_MULTICHANNEL_DMA
    /* Wait for the last transfer */
    XI_CHECK_RESULT(dma_barrier());
#endif
    return XI_ERROR_STATUS();
}

static void calc_tile_dims(const dim_t dims[4], size_t max_tile_size, size_t *tile, size_t elem_size)
{
    /* Increase dimension by dimension until cannot fit */
    for (int i=0; i<4; i++) {
        /* Full output dimension */
        size_t dim = dims[i].dim + dims[i].pad_low + dims[i].pad_high;
        tile[i] = XT_MIN(dim, max_tile_size);
        max_tile_size /= tile[i] * elem_size;
        max_tile_size = XT_MAX(max_tile_size, 1);
    }
}

XI_ERR_TYPE flk_concat(const uint8_t *raw_params,
                     struct XtensaOperationArgsIn *input,
                     struct XtensaOperationArgsOut *output)
{
#if DEBUG_LEVEL > 1
    print_concat_params(raw_params);
#endif

    const concat_params_t *params = (const concat_params_t *)raw_params;

    XI_ERROR_CHECKS(){
        XI_RUN_TIME_CHECK(params != NULL,
                          "Params cannot be NULL", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->structSize == sizeof(*params),
                          "Params structure size is incorrect", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(input != NULL && input->numArgs >= 1 && input->numArgs <= MAX_FLK_CONCAT_INPUTS,
                          "Invalid input args", XI_ERR_BADARG);
        for (unsigned int i = 0; i < input->numArgs; i++) {
            XI_RUN_TIME_CHECK(input->args[i] != NULL && input->argsSize[i] > 0,
                              "Invalid input arg", XI_ERR_BADARG);
        }
        XI_RUN_TIME_CHECK(output != NULL
                          && output->numArgs == 1
                          && output->args[0] != NULL && output->argsSize[0] > 0,
                          "Invalid output args", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->axis < 4 || input->numArgs == 1, "Invalid axis index", XI_ERR_BADARG);

        XI_RUN_TIME_CHECK(params->numInputs == input->numArgs,
                          "Invalid number of inputs", XI_ERR_BADARG);

        for (int n = 0; n < params->numInputs; n++) {
            for (int i = 0; i < 4; i++) {
                XI_RUN_TIME_CHECK(params->input[n][i] >= 1,
                              "Inconsistent input dimensions", XI_ERR_BADARG);
            }
        }

        /*
        unsigned int total_concat_size = 0;
        for (unsigned int i = 0; i < params->numInputs; i++) {
            XI_RUN_TIME_CHECK(params->input[i] >= 1
                              && params->input[i] <= params->output[params->axis],
                              "Inconsistent input dimensions", XI_ERR_BADARG);
            total_concat_size += params->input[i];
        }
        XI_RUN_TIME_CHECK(total_concat_size == params->output[params->axis],
                          "Inconsistent input/output dimensions", XI_ERR_BADARG);
        */
        XI_RUN_TIME_CHECK(arena_num_banks() > 1,
                          "Two memory banks expected", XI_ERR_BADARG);
    };


    /* Get local memory info */
    uint32_t bank0_size = 0;
    uint32_t bank1_size = 0;

    XI_CHECK_RESULT(arena_bank_free_space(0, &bank0_size));
    XI_CHECK_RESULT(arena_bank_free_space(1, &bank1_size));

    size_t tile_size = (bank0_size < bank1_size) ? bank0_size : bank1_size;

    /* Check if amount of memory is just too small to be usable */
    XI_RUN_TIME_CHECK(tile_size >= 256 + ALIGNMENT,
                      "Out of memory", XI_ERR_BADARG);

    /* Adjust for alignment (during allocation) and making it a multiple of 256
       for performance */
    tile_size -= ALIGNMENT;
    tile_size -= tile_size % 256;

    // Transfer each input tile
    uint8_t* inputA;
    uint8_t* inputB;

    /* Reserve memory */
    XI_CHECK_RESULT(arena_init_two_banks_split(tile_size, ALIGNMENT, tile_size,  ALIGNMENT));

    /* Allocate buffers */
    XI_CHECK_RESULT(arena_alloc((void**)(&inputA),/* bank */ 0, /* size */ tile_size, /* alignment */ ALIGNMENT));
    XI_CHECK_RESULT(arena_alloc((void**)(&inputB),/* bank */ 1, /* size */ tile_size, /* alignment */ ALIGNMENT));

    /* Output size without padding taken into account */
    unsigned int output_size[4] = {0, 0, 0, 0};
    for (unsigned int i = 0; i < params->numInputs; i++) {
        for (int j=0; j<4; j++) {
            if (j == params->axis) {
                output_size[j] += params->input[i][j];
            } else {
                output_size[j] = max(output_size[j], params->input[i][j]);
            }
        }
    }

    /* Base of the output pointer increment along the axis */
    int copy_base = 1;
    for (int i = 0; i < params->axis; i++) {
        copy_base *= params->output_pad_low[i] + params->output_pad_high[i] + output_size[i];
    }
    copy_base = (params->numInputs == 1) ? 1 : copy_base;
    /* Copy each input */
    uint8_t *output_data = (uint8_t *)output->args[0];
    for (unsigned int i = 0; i < params->numInputs; i++) {
        dim_t dims[4];
        size_t tile[4];
        int src_stride = 1;
        int dst_stride = 1;
        int dst_increment = copy_base;
        for (int j=0; j<4; j++) {
            dims[j].dim = params->input[i][j];
            dims[j].src_stride = src_stride;
            dims[j].dst_stride = dst_stride;
            src_stride *= params->input[i][j];
            if (j == params->axis) {
                /* No padding in concatenation axis */
                dims[j].pad_low = 0;
                dims[j].pad_high = 0;
                dst_stride *= output_size[j];
                dst_increment *= dims[j].dim;
            } else {
                /* Calc padding with respect to actual input size */
                dims[j].pad_low = params->output_pad_low[j];
                dims[j].pad_high = params->output_pad_high[j] + output_size[j] - dims[j].dim;
                dst_stride *= dims[j].dim + dims[j].pad_low + dims[j].pad_high;
            }
        }


        calc_tile_dims(dims, tile_size, &tile[0], params->elemSize);

        XI_CHECK_RESULT(copy4D((uint8_t *)input->args[i],
                               output_data,
                               dims,
                               inputA,
                               inputB,
                               tile,
                               i,
                               params));
        output_data += dst_increment * params->elemSize;
    }
    /* Don't reset arena if last dma_barrier is not executed as  DMA transfer is still ongoing */
#if IS_MULTICHANNEL_DMA
    XI_CHECK_RESULT(arena_reset());
#endif

    return XI_ERROR_STATUS();
}
