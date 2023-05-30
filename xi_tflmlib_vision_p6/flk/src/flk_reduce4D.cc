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
#include "xi_tile3d_manager.h"
#include "flk_reduce.h"
#include "flk_common.h"

#define REDUCE_MAX_ONLY

typedef enum {
 FALSE,//=0,
 TRUE,//=1,
} BOOL_T;

#if DEBUG_LEVEL > 1
void print_reduce4D_params(const uint8_t *raw_params){
    const reduce_params_t *params = (const reduce_params_t *)raw_params;
    if (params == NULL) {
        DEBUG_LOG2("# error: params is NULL\n");
        return;
    }
    DEBUG_LOG2("# ===== REDUCE Params =====\n"
               "#   StructSize: %d \n",
               params->structSize);
    print_dims4d("  Input  ", params->input);
    print_dims4d("  Tile  ", params->tile);
    print_dims4d("  Output  ", params->outTile);
    DEBUG_LOG2("#   axes            :    %d %d %d %d\n"
               "#   translationAxis :    %d \n"
               "#   config          :    %d \n"
               "#   combinedTile    :    %d \n"
               "#   type            :    %d \n"
               "#   Bank Index      :    %d \n"
               "#   Mem. Tile Size  : %d\n"
               , params->axis.D, params->axis.W, params->axis.H, params->axis.N
               , params->translationAxis
               , params->config
               , params->combinedTile
               , params->type
               , params->bankInd
               , params->memSize);
}

#endif

static inline void setup_inp_tile(xi_pTile4D pTile, dim4d_t *NxtTileLoc, const reduce_params_t *params, int ReduceFlag) {
    uint32_t chkNxtDim = 0;
    // chk for dim D
    if ( (params->config & 1) == ReduceFlag) {
        NxtTileLoc->D = NxtTileLoc->D + params->tile.D;
        if (NxtTileLoc->D >= params->input.D) {
            NxtTileLoc->D = 0;
            chkNxtDim = 1;
        }
    } else chkNxtDim =1 ;

    //chk for dim W
    ReduceFlag <<= 1;
    if (chkNxtDim == 1) {
        chkNxtDim = 0;
        if ( (params->config & 2) == ReduceFlag) {
            NxtTileLoc->W = NxtTileLoc->W + params->tile.W;
            if (NxtTileLoc->W >= params->input.W) {
                NxtTileLoc->W = 0;
                chkNxtDim = 1;
            }
        } else chkNxtDim = 1;
    }

    //chk for dim H
    ReduceFlag <<= 1;
    if (chkNxtDim == 1) {
        chkNxtDim = 0;
        if ( (params->config & 4) == ReduceFlag) {
            NxtTileLoc->H = NxtTileLoc->H + params->tile.H;
            if (NxtTileLoc->H >= params->input.H) {
                NxtTileLoc->H = 0;
                chkNxtDim = 1;
            }
        } else chkNxtDim = 1;
    }

    //chk for dim N (batch)
    ReduceFlag <<= 1;
    if (chkNxtDim == 1) {
        chkNxtDim = 0;
        if ( (params->config & 8) == ReduceFlag) {
            NxtTileLoc->N = NxtTileLoc->N + params->tile.N;
            if (NxtTileLoc->N >= params->input.H) {
                NxtTileLoc->N = 0;
                chkNxtDim = 1;
            }
        } else chkNxtDim = 1;
    }
    if (NxtTileLoc->D + params->tile.D < params->input.D) XI_TILE4D_SET_DIM1(pTile, params->tile.D );
    else XI_TILE4D_SET_DIM1(pTile, params->input.D -NxtTileLoc->D);
    if (NxtTileLoc->W + params->tile.W < params->input.W) XI_TILE4D_SET_DIM2(pTile, params->tile.W );
    else XI_TILE4D_SET_DIM2(pTile, params->input.W - NxtTileLoc->W);
    if (NxtTileLoc->H + params->tile.H < params->input.H) XI_TILE4D_SET_DIM3(pTile, params->tile.H );
    else XI_TILE4D_SET_DIM3(pTile, params->input.H - NxtTileLoc->H);
    if (NxtTileLoc->N + params->tile.N < params->input.N) XI_TILE4D_SET_DIM4(pTile, params->tile.N );
    else XI_TILE4D_SET_DIM4(pTile, params->input.N - NxtTileLoc->N);

     
}
static inline void transfer_inp_tile(uint8_t *pInp, xi_pTile4D pTile, const reduce_params_t *params, const dim4d_t *TileLoc)
{
    uint8_t *psrc, *pdst;
    uint32_t row_size;
    uint32_t count2d, count3d, count4d;

    psrc = pInp
           + (TileLoc->D
           + TileLoc->W * params->input.D
           + TileLoc->H * params->input.D * params->input.W
           + TileLoc->N * params->input.D * params->input.W * params->input.H)*params->eleSize;

    pdst = (uint8_t *) XI_TILE3D_GET_BUFF_PTR(pTile);

    row_size = params->tile.D * params->eleSize;
    if (row_size > ((params->input.D - TileLoc->D)*params->eleSize)) row_size = (params->input.D - TileLoc->D)*params->eleSize;
    count2d = params->tile.W;
    if (count2d > (params->input.W - TileLoc->W)) count2d = params->input.W - TileLoc->W;
    count3d = params->tile.H;
    if (count3d > (params->input.H - TileLoc->H)) count3d = params->input.H - TileLoc->H;
    count4d = params->tile.N;
    if (count4d > (params->input.N - TileLoc->N)) count4d = params->input.N - TileLoc->N;

    dma_4d_sys2loc_dyn(/* src */ psrc,
                       /* dst */ pdst,
                       row_size ,
                       /* src stride 2d */ params->input.D * params->eleSize,
                       /* dst stride 2d */ XI_TILE4D_GET_DIM1_PITCH(pTile) * params->eleSize,
                       count2d ,
                       /* src stride 3d */ params->input.D*params->input.W * params->eleSize,
                       /* dst stride 3d */ XI_TILE4D_GET_DIM2_PITCH(pTile) * params->eleSize,
                       count3d ,
                       /* src stride 4d */ params->input.D * params->input.W * params->input.H * params->eleSize,
                       /* dst strides 4d */ XI_TILE4D_GET_DIM3_PITCH(pTile) * params->eleSize,
                       count4d );
}

static inline void InitTileLoc (xi_pTile4D pTile, dim4d_t *TileLoc, const reduce_params_t *params) {
    setup_inp_tile(pTile,TileLoc, params, 0);
}


static inline void setup_outp_tile(xi_pTile4D pTile, dim4d_t *OutTileLoc, const dim4d_t *tileOut, const reduce_params_t *params) {
    OutTileLoc->D = OutTileLoc->D + tileOut->D;
    if (OutTileLoc->D >= params->output.D) {
        OutTileLoc->D = 0;
        OutTileLoc->W = OutTileLoc->W + tileOut->W;
        if (OutTileLoc->W >= params->output.W) {
            OutTileLoc->W = 0;
            OutTileLoc->H = OutTileLoc->H + tileOut->H;
            if (OutTileLoc->H >= params->output.H) {
                OutTileLoc->H = 0;
                OutTileLoc->N = OutTileLoc->N + tileOut->N;
                if (OutTileLoc->N >= params->output.N) OutTileLoc->N=0;
            }
        }
    }
     if (OutTileLoc->D + tileOut->D < params->output.D) XI_TILE4D_SET_DIM1(pTile, tileOut->D );
     else XI_TILE4D_SET_DIM1(pTile, params->output.D -OutTileLoc->D);
     if (OutTileLoc->W + tileOut->W < params->output.W) XI_TILE4D_SET_DIM2(pTile, tileOut->W );
     else XI_TILE4D_SET_DIM2(pTile, params->output.W - OutTileLoc->W);
     if (OutTileLoc->H + tileOut->H < params->output.H) XI_TILE4D_SET_DIM3(pTile, tileOut->H );
     else XI_TILE4D_SET_DIM3(pTile, params->output.H - OutTileLoc->H);
     if (OutTileLoc->N + tileOut->N < params->output.N) XI_TILE4D_SET_DIM4(pTile, tileOut->N );
     else XI_TILE4D_SET_DIM4(pTile, params->output.N - OutTileLoc->N);
                     
}

/* Schedule output tile DMA transfer */

static inline void transfer_out_tile(uint8_t *pOut, xi_pTile4D pTile, const reduce_params_t *params, const dim4d_t *OutTileLoc) 
{
    uint32_t row_size;
    uint8_t *psrc, *pdst;
    uint32_t count2d, count3d, count4d;

    pdst = pOut
           + (OutTileLoc->D
           + OutTileLoc->W * params->output.D
           + OutTileLoc->H * params->output.D * params->output.W
           + OutTileLoc->N * params->output.D * params->output.W * params->output.H) * params->eleSize;
    psrc = (uint8_t *) XI_TILE3D_GET_BUFF_PTR(pTile);

    row_size = XI_TILE4D_GET_DIM1(pTile) * params->eleSize;
    if (row_size > ((params->output.D - OutTileLoc->D)*params->eleSize)) row_size = (params->output.D - OutTileLoc->D)*params->eleSize;
    count2d = XI_TILE4D_GET_DIM2(pTile);
    if (count2d > (params->output.W - OutTileLoc->W)) count2d = params->output.W - OutTileLoc->W;
    count3d = XI_TILE4D_GET_DIM3(pTile);
    if (count3d > (params->output.H - OutTileLoc->H)) count3d = params->output.H - OutTileLoc->H;
    count4d = XI_TILE4D_GET_DIM4(pTile);
    if (count4d > (params->output.N - OutTileLoc->N)) count4d = params->output.N - OutTileLoc->N;

     dma_4d_loc2sys_dyn(/* src */ psrc,
                       /* dst */ pdst,
                       /* row size */ row_size,
                       /* src stride 2d */ XI_TILE4D_GET_DIM1_PITCH(pTile) * params->eleSize,
                       /* dst stride 2d */ params->output.D * params->eleSize,
                       /* count 2d */ count2d,
                       /* src stride 3d */ XI_TILE4D_GET_DIM2_PITCH(pTile) * params->eleSize,
                       /* dst stride 3d */ params->output.D * params->output.W * params->eleSize,
                       /* count 3d */ count3d,
                       /* src stride 4d */ XI_TILE4D_GET_DIM3_PITCH(pTile) * params->eleSize,
                       /* dst stride 4d */ params->output.D * params->output.W * params->output.H * params->eleSize,
                       /* count 4d */ count4d);
}

typedef XI_ERR_TYPE (*xiReduceA4D_f)(const xi_pTile4D inputTile,
                                    xi_pTile4D outTile,
                                    const xi_cnn_reduceA_params *params);
typedef XI_ERR_TYPE (*xiReduceA4DIntermediateF32_f)(const xi_pTile4D inputTile,
                                    xi_pArray intermediateArray,
                                    xi_pTile4D outTile,
                                    const xi_cnn_reduceA_params *params);

XI_ERR_TYPE flk_reduce4D(const uint8_t *raw_params,
                     struct XtensaOperationArgsIn *input, 
                     struct XtensaOperationArgsOut *output)
{
#if DEBUG_LEVEL > 1
    print_reduce4D_params(raw_params);
#endif
    const reduce_params_t *params = (const reduce_params_t *)raw_params;
    XI_ERROR_CHECKS(){
        XI_RUN_TIME_CHECK(params != NULL,
                          "Params cannot be NULL", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->structSize == sizeof(*params),
                          "Params structure size is incorrect", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(input != NULL
                          && input->numArgs >= 1
                          && input->args[0] != NULL && input->argsSize[0] > 0,
                          "Invalid input args", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(output != NULL
                          && output->numArgs == 1
                          && output->args[0] != NULL && output->argsSize[0] > 0,
                          "Invalid output args", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->input.D >= 1
                          && params->input.W >= 1
                          && params->input.H >= 1
                          && params->input.N >= 1
                          && params->input.D * params->input.W * params->input.H * params->input.N <= input->argsSize[0],
                          "Inconsistent input dimensions", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->output.D >= 1
                          && params->output.W >= 1
                          && params->output.H >= 1
                          && params->output.N >= 1
                          && params->output.D * params->output.W * params->output.H * params->output.N <= output->argsSize[0],
                          "Inconsistent output dimensions", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->tile.D >= 1 && params->input.D >= params->tile.D
                          && params->tile.W >= 1 && params->input.W >= params->tile.W
                          && params->tile.H >= 1 && params->input.H >= params->tile.H
                          && params->tile.N >= 1 && params->input.N >= params->tile.N,
                          "Inconsistent tile dimensions", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->outTile.D >= 1
                          && params->outTile.W >= 1
                          && params->outTile.H >= 1
                          && params->outTile.N >= 1,
                          "Inconsistent output dimensions", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->bankInd == 0 || params->bankInd == 1,
                          "Invalid bank size", XI_ERR_BADARG);
    };
    
    uint8_t *input_data = (uint8_t *)input->args[0];
    uint8_t *output_data = (uint8_t *)output->args[0];
    uint8_t* input01 = NULL;
    uint8_t* input02 = NULL;
    uint8_t* output03 = NULL;
    uint8_t* output04 = NULL;
    uint8_t* intermediateData = NULL;
    xi_tile4D tiles[4];
    xi_pTile4D tile_input0,tile_input1 ;
    xi_pTile4D tile_output0,tile_output1;
    xi_pTile4D ptemp4D;
#ifndef REDUCE_MAX_ONLY
    xi_array intermediateArr;
#endif
    (void)intermediateData;
    /* Number of tiles in each dimension */
    uint32_t numTilesD = (params->input.D + params->tile.D - 1) / params->tile.D;
    uint32_t numTilesW = (params->input.W + params->tile.W - 1) / params->tile.W;
    uint32_t numTilesH = (params->input.H + params->tile.H - 1) / params->tile.H;
    uint32_t numTilesN = (params->input.N + params->tile.N - 1) / params->tile.N;

    /* Max input tile dimensions */
    int32_t inpTileD = params->tile.D;
    int32_t inpTileW = params->tile.W;
    int32_t inpTileH = params->tile.H;
    int32_t inpTileN = params->tile.N;


    xiReduceA4D_f   kernel = NULL;
#ifndef REDUCE_MAX_ONLY
#if (XCHAL_HAVE_VISION_HP_VFPU == 1)
    xiReduceA4DIntermediateF32_f kernelIntmdtF32 = XI_KERNEL_NAME(xiReduceMeanA4D_F16);
#endif
#endif
    /* Reserve memory */
    XI_CHECK_RESULT(arena_init_two_banks_split(/*size0:*/ params->memSize, /* max_alignment0 */ ALIGNMENT, /*size1:*/ params->memSize, /* max_alignment1 */ ALIGNMENT));
    /* Allocate buffers */
#ifndef REDUCE_MAX_ONLY
    if ((params->type == XFL_REDUCE_MEAN || params->type == XFL_REDUCE_PROD || params->type == XFL_REDUCE_SUM)
        && params->dataType == FP16)
    {
        XI_CHECK_RESULT(arena_alloc((void * *) (&input01), /* bank */ params->bankInd, /* size */ params->tileSize, /* alignment */ ALIGNMENT));
        XI_CHECK_RESULT(arena_alloc((void * *) (&output03), /* bank */ (params->bankInd), /* size */ params->outTileSize, /* alignment */ ALIGNMENT));
        if (!params->combinedTile)
        {
            // Input ping-pong buffer required
            XI_CHECK_RESULT(arena_alloc((void * *) (&input02), /* bank */ (1 - params->bankInd), /* size*/ params->tileSize, /* alignment */ ALIGNMENT));
        }
        if (params->intermediateArrSize > 0)
        {
            // Intermediate array required
            XI_CHECK_RESULT(arena_alloc((void * *) (&intermediateData), /* bank */ (1 - params->bankInd), /* size */ params->intermediateArrSize, /* alignment */ ALIGNMENT));
        }
    }
    else
#endif
    {
        XI_CHECK_RESULT(arena_alloc((void**)(&input01),/* bank */ params->bankInd, /* size */ params->tileSize, /* alignment */ ALIGNMENT));
        XI_CHECK_RESULT(arena_alloc((void**)(&output03),/* bank */ (params->bankInd), /* size */ params->outTileSize, /* alignment */ ALIGNMENT));
        if (!params->combinedTile){
            XI_CHECK_RESULT(arena_alloc((void**)(&input02),/* bank */ (1-params->bankInd), /* size*/ params->tileSize, /* alignment */ ALIGNMENT));
            XI_CHECK_RESULT(arena_alloc((void**)(&output04),/* bank */ (1 - params->bankInd), /* size */ params->outTileSize, /* alignment */ ALIGNMENT));
        }
    }
    memset(&tiles, 0, sizeof(tiles));
    tile_input0 = (xi_pTile4D)&tiles[0];
    tile_input1 = (xi_pTile4D)&tiles[1];
    tile_output0 = (xi_pTile4D)&tiles[2];
    tile_output1 = (xi_pTile4D)&tiles[3];

    int tileType;
    if (params->dataType == FP16) tileType = XI_TILE4D_F16; 
    else if (params->dataType == S8) tileType = XI_TILE4D_S8; 
    else tileType = XI_TILE4D_U8; 

    // Input Parameters
    XI_TILE4D_SET_BUFF_SIZE(tile_input0, params->tileSize);
    XI_TILE4D_SET_BUFF_PTR(tile_input0, input01);
    XI_TILE4D_SET_DATA_PTR(tile_input0, &(input01[0]));
    XI_TILE4D_SET_DATA_ORDER(tile_input0, XI_DWH);
    XI_TILE4D_SET_TYPE(tile_input0, tileType);
    XI_TILE4D_SET_DIM1(tile_input0, inpTileD );
    XI_TILE4D_SET_DIM2(tile_input0, inpTileW );
    XI_TILE4D_SET_DIM3(tile_input0, inpTileH );
    XI_TILE4D_SET_DIM4(tile_input0, inpTileN );
    XI_TILE4D_SET_DIM1_PITCH(tile_input0, inpTileD);
    XI_TILE4D_SET_DIM2_PITCH(tile_input0, inpTileD * inpTileW);
    XI_TILE4D_SET_DIM3_PITCH(tile_input0, inpTileD * inpTileW * inpTileH);

    XI_TILE4D_SET_BUFF_SIZE(tile_input1, params->tileSize);
    XI_TILE4D_SET_BUFF_PTR(tile_input1,  input02);
    XI_TILE4D_SET_DATA_PTR(tile_input1, &(input02[0]));
    XI_TILE4D_SET_DATA_ORDER(tile_input1, XI_DWH);
    XI_TILE4D_SET_TYPE(tile_input1, tileType);
    XI_TILE4D_SET_DIM1(tile_input1, inpTileD );
    XI_TILE4D_SET_DIM2(tile_input1, inpTileW );
    XI_TILE4D_SET_DIM3(tile_input1, inpTileH );
    XI_TILE4D_SET_DIM4(tile_input1, inpTileN );
    XI_TILE4D_SET_DIM1_PITCH(tile_input1, inpTileD);
    XI_TILE4D_SET_DIM2_PITCH(tile_input1, inpTileD * inpTileW);
    XI_TILE4D_SET_DIM3_PITCH(tile_input1, inpTileD * inpTileW * inpTileH);

    // Output Parameters
    XI_TILE4D_SET_BUFF_SIZE(tile_output0, params->outTileSize);
    XI_TILE4D_SET_BUFF_PTR(tile_output0, output03);
    XI_TILE4D_SET_DATA_PTR(tile_output0, &(output03[0]));
    XI_TILE4D_SET_DATA_ORDER(tile_output0, XI_DWH);
    XI_TILE4D_SET_TYPE(tile_output0, tileType);
    XI_TILE4D_SET_DIM1(tile_output0, params->outTile.D );
    XI_TILE4D_SET_DIM2(tile_output0, params->outTile.W );
    XI_TILE4D_SET_DIM3(tile_output0, params->outTile.H );
    XI_TILE4D_SET_DIM4(tile_output0, params->outTile.N );

    XI_TILE4D_SET_BUFF_SIZE(tile_output1, params->outTileSize);
    XI_TILE4D_SET_BUFF_PTR(tile_output1, output04);
    XI_TILE4D_SET_DATA_PTR(tile_output1, &(output04[0]));
    XI_TILE4D_SET_DATA_ORDER(tile_output1, XI_DWH);
    XI_TILE4D_SET_TYPE(tile_output1, tileType);
    XI_TILE4D_SET_DIM1(tile_output1, params->outTile.D );
    XI_TILE4D_SET_DIM2(tile_output1, params->outTile.W );
    XI_TILE4D_SET_DIM3(tile_output1, params->outTile.H );
    XI_TILE4D_SET_DIM4(tile_output1, params->outTile.N );

#ifndef REDUCE_MAX_ONLY
    if ((params->type == XFL_REDUCE_MEAN || params->type == XFL_REDUCE_PROD || params->type == XFL_REDUCE_SUM) &&
        params->dataType == FP16)
    {
        // Intermediate array parameters
        XI_ARRAY_SET_TYPE(&intermediateArr, XI_ARRAY_F32);
        XI_ARRAY_SET_BUFF_PTR(&intermediateArr, intermediateData);
        XI_ARRAY_SET_DATA_PTR(&intermediateArr, intermediateData);
        XI_ARRAY_SET_BUFF_SIZE(&intermediateArr, params->intermediateArrSize);
        XI_ARRAY_SET_WIDTH(&intermediateArr, params->intermediateArrSize / 4);
        XI_ARRAY_SET_PITCH(&intermediateArr, params->intermediateArrSize / 4);
        XI_ARRAY_SET_HEIGHT(&intermediateArr, 1);
    }
#endif
    int totRedCount = 1;
    if (params->axis.D) {
        XI_TILE4D_SET_DIM1(tile_output0, 1);
        XI_TILE4D_SET_DIM1(tile_output1, 1);
        totRedCount *= params->input.D;
    }
    if (params->axis.W) {
        XI_TILE4D_SET_DIM2(tile_output0, 1);
        XI_TILE4D_SET_DIM2(tile_output1, 1);
        totRedCount *= params->input.W;
    }
    if (params->axis.H) {
        XI_TILE4D_SET_DIM3(tile_output0, 1);
        XI_TILE4D_SET_DIM3(tile_output1, 1);
        totRedCount *= params->input.H;
    }
    if (params->axis.N) {
        XI_TILE4D_SET_DIM4(tile_output0, 1);
        XI_TILE4D_SET_DIM4(tile_output1, 1);
        totRedCount *= params->input.N;
    }
    XI_TILE4D_SET_DIM1_PITCH(tile_output0, XI_TILE4D_GET_DIM1(tile_output0));
    XI_TILE4D_SET_DIM1_PITCH(tile_output1, XI_TILE4D_GET_DIM1(tile_output1));
    XI_TILE4D_SET_DIM2_PITCH(tile_output0, XI_TILE4D_GET_DIM1(tile_output0) * XI_TILE4D_GET_DIM2(tile_output0));
    XI_TILE4D_SET_DIM2_PITCH(tile_output1, XI_TILE4D_GET_DIM1(tile_output1) * XI_TILE4D_GET_DIM2(tile_output1));
    XI_TILE4D_SET_DIM3_PITCH(tile_output0, XI_TILE4D_GET_DIM1(tile_output0) * XI_TILE4D_GET_DIM2(tile_output0) * XI_TILE4D_GET_DIM3(tile_output0));
    XI_TILE4D_SET_DIM3_PITCH(tile_output1, XI_TILE4D_GET_DIM1(tile_output1) * XI_TILE4D_GET_DIM2(tile_output1) * XI_TILE4D_GET_DIM3(tile_output1));

    xi_cnn_reduceA_params xi_params;
    xi_params.config = params->config;
    xi_params.tileFlag = 1;
    switch (params->type) {
#ifndef REDUCE_MAX_ONLY

        case XFL_REDUCE_MIN:    if (params->dataType==S8) kernel = XI_KERNEL_NAME(xiReduceMinA4D_S8); 
                                else if (params->dataType==U8) kernel = XI_KERNEL_NAME(xiReduceMinA4D_U8); 
#if (XCHAL_HAVE_VISION_HP_VFPU == 1)
				else kernel = XI_KERNEL_NAME(xiReduceMinA4D_F16_ref); 
#endif                             
			     	break;
#endif
        case XFL_REDUCE_MAX:    
#if 0
          if (params->dataType==S8) kernel = XI_KERNEL_NAME(xiReduceMaxA4D_S8); 
                                else if (params->dataType==U8) kernel = XI_KERNEL_NAME(xiReduceMaxA4D_U8); 
#if (XCHAL_HAVE_VISION_HP_VFPU == 1)
     				else  kernel = XI_KERNEL_NAME(xiReduceMaxA4D_F16); 
#endif   
#else
          kernel = XI_KERNEL_NAME(xiReduceMaxA4D_S8);
#endif
			     	break;
#ifndef REDUCE_MAX_ONLY
        case XFL_REDUCE_MEAN:
#if (XCHAL_HAVE_VISION_HP_VFPU == 1)
            if (params->dataType == FP16)
            {
                kernelIntmdtF32 = XI_KERNEL_NAME(xiReduceMeanA4D_F16);
            }
#endif
            break;
        case XFL_REDUCE_PROD:
#if (XCHAL_HAVE_VISION_HP_VFPU == 1)
            if (params->dataType == FP16)
            {
                kernelIntmdtF32 = XI_KERNEL_NAME(xiReduceProdA4D_F16);
            }
#endif
            break;
        case XFL_REDUCE_ANY:    kernel = XI_KERNEL_NAME(xiReduceAnyA4D_I8); break;
        case XFL_REDUCE_ALL:    kernel = XI_KERNEL_NAME(xiReduceAllA4D_I8); break;
        case XFL_REDUCE_SUM:
#if (XCHAL_HAVE_VISION_HP_VFPU == 1)
          if (params->dataType == FP16)
          {
            kernel = XI_KERNEL_NAME(xiReduceSumA4D_F16);
          }
#endif
          break;
#endif
    }
#ifndef REDUCE_MAX_ONLY
    XI_ERR_TYPE (*requant_func)(const xi_pTile4D inTile, 
                xi_pTile4D outTile, 
                const xi_cnn_reQuantizeA_params *pparams);
  xi_cnn_reQuantizeA_params quant_params;
    int32_t doRequant = params->doRequant;
  
  if(doRequant)
  {
    XI_CNN_REQUANTIZEA_SET_ZERO_POINT_IN(&quant_params, params->zero_pointInp);
    XI_CNN_REQUANTIZEA_SET_ZERO_POINT_OUT(&quant_params, params->zero_pointOut);
    XI_CNN_REQUANTIZEA_SET_MULTIPLIER_OUT(&quant_params, params->multiplierOut);
    XI_CNN_REQUANTIZEA_SET_SHIFT_OUT(&quant_params, params->shiftOut);
    XI_CNN_REQUANTIZEA_SET_LEFT_SHIFT(&quant_params, params->left_shift);
    if(params->dataType == S8){
      XI_CNN_REQUANTIZEA_SET_MIN_VAL(&quant_params, -128); //pramas->Min
      XI_CNN_REQUANTIZEA_SET_MAX_VAL(&quant_params,  127); //params->Max
    }
    else{
      XI_CNN_REQUANTIZEA_SET_MIN_VAL(&quant_params, 	0); //pramas->Min
      XI_CNN_REQUANTIZEA_SET_MAX_VAL(&quant_params, 255); //params->Max
    }
    XI_CNN_REQUANTIZEA_SET_SCALE_FLAG(&quant_params, params->ScaleFlag);
                       
    if(params->dataType == S8){
      requant_func = xiReQuantizeA4D_S8;
    }
    else{
      requant_func = xiReQuantizeA4D_U8;
    }	
  }
    if (params->type == XFL_REDUCE_MEAN && params->dataType == FP16)
    {
        XI_CNN_REDUCE_SET_REDUCED_ELEMENTS_COUNT(&xi_params, totRedCount);
    }
#endif
    uint32_t NonReducedTiles = 1;
    uint32_t ReducedTiles = 1;
    if (!(params->config & 1)) NonReducedTiles *= numTilesD; 
    else ReducedTiles *= numTilesD; 
    if (!(params->config & 2)) NonReducedTiles *= numTilesW; 
    else ReducedTiles *= numTilesW; 
    if (!(params->config & 4)) NonReducedTiles *= numTilesH; 
    else ReducedTiles *= numTilesH; 
    if (!(params->config & 8)) NonReducedTiles *= numTilesN; 
    else ReducedTiles *= numTilesN; 


    dim4d_t TileLoc, NxtTileLoc;
    dim4d_t OutTileLoc;
    dim4d_t tileOut;
    tileOut.D = XI_TILE4D_GET_DIM1(tile_output0);
    tileOut.W = XI_TILE4D_GET_DIM2(tile_output0);
    tileOut.H = XI_TILE4D_GET_DIM3(tile_output0);
    tileOut.N = XI_TILE4D_GET_DIM4(tile_output0);
    TileLoc.D = TileLoc.W = TileLoc.H = TileLoc.N = 0;
    NxtTileLoc.D = NxtTileLoc.W = NxtTileLoc.H = NxtTileLoc.N = 0;
    OutTileLoc.D = OutTileLoc.W = OutTileLoc.H = OutTileLoc.N = 0;
    OP_STATE_T TileState;

    for (uint32_t tile = 0; tile<NonReducedTiles ; tile++) {
        xi_params.tileFlag = 1;
#ifndef REDUCE_MAX_ONLY
        if ((params->type == XFL_REDUCE_MEAN || params->type == XFL_REDUCE_PROD) &&
            params->dataType == FP16)
        {
            if (ReducedTiles == 1)       // Only one tile to be reduced
            {
                xi_params.tileFlag = XI_CNN_REDUCE_FIRST_LAST_TILE;
            }
        }
#endif
        transfer_inp_tile(&input_data[0], tile_input0, params, &NxtTileLoc);    // transfer first tile
        for (uint32_t rtile = 0; rtile < ReducedTiles; rtile++) {    // loop for reduction tiles
            TileState = SET_BUF;
            while (TileState != TILE_DONE) {
                switch (TileState) {
                    case SET_BUF:
                        ptemp4D = tile_input0;
                        tile_input0 = tile_input1;
                        tile_input1 = ptemp4D;
                        TileState = SET_IN_DMA;
                    case SET_IN_DMA:
                        XI_CHECK_RESULT(dma_barrier());
                        if (rtile < ReducedTiles-1) {
                            setup_inp_tile(tile_input0, &NxtTileLoc, params, 1);
                            transfer_inp_tile(&input_data[0], tile_input0, params, &NxtTileLoc);
                        }
                        TileState = REDUCE;
                    case REDUCE:
#ifndef REDUCE_MAX_ONLY
#if (XCHAL_HAVE_VISION_HP_VFPU == 1)
                        if ((params->type == XFL_REDUCE_MEAN || params->type == XFL_REDUCE_PROD) &&
                            params->dataType == FP16)
                        {
                            INST_KERNEL_BEGIN();
                            XI_CHECK_RESULT((*kernelIntmdtF32)(tile_input1, &intermediateArr, tile_output0, &xi_params));
                            INST_KERNEL_END();
                        }
                        else if ((params->type == XFL_REDUCE_SUM) && params->dataType == FP16)
                        {
                          INST_KERNEL_BEGIN();
                          XI_CHECK_RESULT((*kernel)(tile_input1, tile_output0, &xi_params));
                          INST_KERNEL_END();
                        }
                        else
#endif
#endif
                        {
                            INST_KERNEL_BEGIN();
                            XI_CHECK_RESULT((*kernel)(tile_input1, tile_output0, &xi_params));
#ifndef REDUCE_MAX_ONLY
    						if(doRequant)
    						{
    							XI_CHECK_RESULT((*requant_func)(tile_output0, tile_output0, &quant_params));
    						}
#endif
                            INST_KERNEL_END();
                        }
#ifndef REDUCE_MAX_ONLY
                        if ((params->type == XFL_REDUCE_MEAN || params->type == XFL_REDUCE_PROD) &&
                            params->dataType == FP16)
                        {
                            if ((rtile + 1) == ReducedTiles - 1)               // Last tile
                            {
                                xi_params.tileFlag = XI_CNN_REDUCE_LAST_TILE;
                            }
                            else
                            {
                                xi_params.tileFlag = 0;
                            }
                        }
                        else
#endif
                        {
                            xi_params.tileFlag = 0;
                        }
                        TileState = TILE_DONE;
                    default:
                        break;
                }
            }
        }
        transfer_out_tile(&output_data[0], tile_output0, params, &OutTileLoc);
        setup_outp_tile(tile_output0, &OutTileLoc, &tileOut,  params);
        InitTileLoc (tile_input0, &TileLoc, params);
        NxtTileLoc.D = TileLoc.D;
        NxtTileLoc.W = TileLoc.W;
        NxtTileLoc.H = TileLoc.H;
        NxtTileLoc.N = TileLoc.N;
    }

    XI_CHECK_RESULT(dma_barrier());
    return XI_ERROR_STATUS();
}


