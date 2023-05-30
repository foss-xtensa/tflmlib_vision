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

#include "xi_tile3d_manager.h"
#include "flk_eltwiseOp.h"
#include "utils.h"

/* Setup output tile based on coordinates and params. */
static inline void
setup_outp_tile(int D, int X, int Y, xi_pTile3D outp, const eltwiseOp_params_t* params)
{
    /* Set output tile coordinates */
    XI_TILE3D_SET_DIM1_COORD(outp, D);
    XI_TILE3D_SET_DIM2_COORD(outp, X);
    XI_TILE3D_SET_DIM3_COORD(outp, Y);

    /* Compute output tile dimensions from intersection of output tile size with output data */
    XI_TILE3D_SET_DIM1(outp, min(D + params->outpTile.D, params->output.D) - D);
    XI_TILE3D_SET_DIM2(outp, min(X + params->outpTile.W, params->output.W) - X);
    XI_TILE3D_SET_DIM3(outp, min(Y + params->outpTile.H, params->output.H) - Y);

    /* Update output tile pitch according to tile size */
    XI_TILE3D_SET_DIM1_PITCH(outp, XI_TILE3D_GET_DIM1(outp));
    XI_TILE3D_SET_DIM2_PITCH(outp, XI_TILE3D_GET_DIM2(outp) * XI_TILE3D_GET_DIM1(outp));
}

/* Setup input tile coordinates/dimensions based on output tile coordinates/dimensions and params. */
static inline void
setup_inp_tile(const xi_pTile3D outp, xi_pTile3D inp, int i, const eltwiseOp_params_t* params)
{
    uint32_t inputD, inputW, inputH;
    if (i == 1) {
        inputD = params->inputB.D;
        inputW = params->inputB.W;
        inputH = params->inputB.H;
    }
    else {
        inputD = params->inputA.D;
        inputW = params->inputA.W;
        inputH = params->inputA.H;
    }

    /* Compute input tile dimensions */
    XI_TILE3D_SET_DIM1(inp, min(XI_TILE3D_GET_DIM1(outp), inputD));
    XI_TILE3D_SET_DIM2(inp, min(XI_TILE3D_GET_DIM2(outp), inputW));
    XI_TILE3D_SET_DIM3(inp, min(XI_TILE3D_GET_DIM3(outp), inputH));

    /* Compute input tile coordinates */
    XI_TILE3D_SET_DIM1_COORD(inp, (inputD == 1) ? 0 : XI_TILE3D_GET_DIM1_COORD(outp));
    XI_TILE3D_SET_DIM2_COORD(inp, (inputW == 1) ? 0 : XI_TILE3D_GET_DIM2_COORD(outp));
    XI_TILE3D_SET_DIM3_COORD(inp, (inputH == 1) ? 0 : XI_TILE3D_GET_DIM3_COORD(outp));

    /* Update input tile pitch according to tile size */
    XI_TILE3D_SET_DIM1_PITCH(inp, XI_TILE3D_GET_DIM1(inp));
    XI_TILE3D_SET_DIM2_PITCH(inp, XI_TILE3D_GET_DIM2(inp) * XI_TILE3D_GET_DIM1_PITCH(inp));
}

/* For input tile at (X, Y) of (W, H) with edges find intersection with input data and amount of padding needed.
   Schedules DMA transfer to bring valid data into tile.
   Sets tile status to non-zero if zero padding is needed. */
static inline void
transfer_inp_tile(uint8_t* inputPtr, xi_pTile3D inp, int i, const eltwiseOp_params_t* params)
{
    uint32_t inputD = (i == 0) ? params->inputA.D : params->inputB.D;
    uint32_t inputW = (i == 0) ? params->inputA.W : params->inputB.W;
    uint32_t inputH = (i == 0) ? params->inputA.H : params->inputB.H;

    int boundX = XI_TILE3D_GET_DIM2_COORD(inp);
    int boundY = XI_TILE3D_GET_DIM3_COORD(inp);
    int boundW = XI_TILE3D_GET_DIM2_COORD(inp) + XI_TILE3D_GET_DIM2(inp);
    int boundH = XI_TILE3D_GET_DIM3_COORD(inp) + XI_TILE3D_GET_DIM3(inp);

    int validX = max(0, boundX);
    int validY = max(0, boundY);
    int validW = min(boundW, inputW) - validX;
    int validH = min(boundH, inputH) - validY;

    uint8_t* tilePtr = (uint8_t*)XI_TILE3D_GET_BUFF_PTR(inp);
    dma_3d_sys2loc_dyn(/* src */ inputPtr +
        params->elemSize * ((validY * inputW * inputD) + (validX * inputD) + XI_TILE3D_GET_DIM1_COORD(inp)),
        /* dst */ tilePtr,
        /* row size */      params->elemSize * XI_TILE3D_GET_DIM1_PITCH(inp),
        /* src stride 2d */ params->elemSize * inputD,
        /* dst stride 2d */ params->elemSize * XI_TILE3D_GET_DIM1_PITCH(inp),
        /* count 2d */ validW,
        /* src stride 3d */ params->elemSize * inputD * inputW,
        /* dst stride 3d */ params->elemSize * XI_TILE3D_GET_DIM2_PITCH(inp),
        /* count 3d */ validH);
}

/* Schedule output tile DMA transfer */
static inline void
transfer_outp_tile(uint8_t* outputPtr, xi_pTile3D outp, const eltwiseOp_params_t* params)
{

    int D = XI_TILE3D_GET_DIM1_COORD(outp);
    int X = XI_TILE3D_GET_DIM2_COORD(outp);
    int Y = XI_TILE3D_GET_DIM3_COORD(outp);

    uint8_t* tilePtr = (uint8_t*)XI_TILE3D_GET_BUFF_PTR(outp);
#if IS_MULTICHANNEL_DMA
    dma_3d_loc2sys_dyn_ch(/*channel*/ 1,
        /* src */ tilePtr,
        /* dst */ outputPtr + params->elemSize * (Y * params->output.D * params->output.W + X * params->output.D + D),
        /* row size */      params->elemSize * XI_TILE3D_GET_DIM1(outp),
        /* src stride 2d */ params->elemSize * XI_TILE3D_GET_DIM1_PITCH(outp),
        /* dst stride 2d */ params->elemSize * params->output.D,
        /* count 2d */ XI_TILE3D_GET_DIM2(outp),
        /* src stride 3d */ params->elemSize * XI_TILE3D_GET_DIM2_PITCH(outp),
        /* dst stride 3d */ params->elemSize * params->output.D * params->output.W,
        /* count 3d */ XI_TILE3D_GET_DIM3(outp));

#else
    dma_3d_loc2sys_dyn(/* src */ tilePtr,
        /* dst */ outputPtr + params->elemSize * (Y * params->output.D * params->output.W + X * params->output.D + D),
        /* row size */      params->elemSize * XI_TILE3D_GET_DIM1(outp),
        /* src stride 2d */ params->elemSize * XI_TILE3D_GET_DIM1_PITCH(outp),
        /* dst stride 2d */ params->elemSize * params->output.D,
        /* count 2d */ XI_TILE3D_GET_DIM2(outp),
        /* src stride 3d */ params->elemSize * XI_TILE3D_GET_DIM2_PITCH(outp),
        /* dst stride 3d */ params->elemSize * params->output.D * params->output.W,
        /* count 3d */ XI_TILE3D_GET_DIM3(outp));
#endif
}

XI_ERR_TYPE xiElementwiseAddA_I8(const xi_pTile3D inTile1,
    const xi_pTile3D inTile2,
    xi_pTile3D outTile,
    xi_pTile3D outmaskTile,
    const uint8_t* func_params) {
    (void)outmaskTile;
    return xiBroadcastAddA3D_S8(inTile1, inTile2, outTile, (const xi_cnn_addA_params*)func_params);
}

XI_ERR_TYPE flk_eltwiseOp(const uint8_t* raw_params,
    struct XtensaOperationArgsIn* input,
    struct XtensaOperationArgsOut* output)
{
#if DEBUG_LEVEL > 1
    print_eltwiseOp_params(raw_params);
#endif
    const eltwiseOp_params_t* params = (const eltwiseOp_params_t*)raw_params;

    XI_ERROR_CHECKS() {
        XI_RUN_TIME_CHECK(params != NULL,
            "Params cannot be NULL", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->structSize == sizeof(*params),
            "Params structure size is incorrect", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(input != NULL
            && input->numArgs == 2
            && input->args[0] != NULL
            && input->args[1] != NULL
            && input->argsSize[0] > 0
            && input->argsSize[1] > 0,
            "Invalid input args", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(output != NULL
            && output->numArgs >= 1
            && output->args[0] != NULL && output->argsSize[0] > 0,
            "Invalid output args", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->batchA >= 1
            && params->batchB >= 1
            && params->batch >= 1,
            "Invalid batch size", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->output.D >= 1
            && params->output.W >= 1
            && params->output.H >= 1
            && params->output.D * params->output.W * params->output.H * params->batch <= output->argsSize[0],
            "Inconsistent output dimensions", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->inputA.D >= 1
            && params->inputA.W >= 1
            && params->inputA.H >= 1
            && params->inputA.D * params->inputA.W * params->inputA.H * params->batchA <= input->argsSize[0],
            "Inconsistent input dimensions", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->inputB.D >= 1
            && params->inputB.W >= 1
            && params->inputB.H >= 1
            && params->inputB.D * params->inputB.W * params->inputB.H * params->batchB <= input->argsSize[1],
            "Inconsistent input dimensions", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->outpTile.D >= 1 && params->output.D >= params->outpTile.D
            && params->outpTile.W >= 1 && params->output.W >= params->outpTile.W
            && params->outpTile.H >= 1 && params->output.H >= params->outpTile.H,
            "Inconsistent output tile dimensions", XI_ERR_BADARG);
#ifndef _MSC_VER
        XI_RUN_TIME_CHECK(params->type == ELEMENT_WISE_ADD
            || params->type == ELEMENT_WISE_MUL
#if (XCHAL_HAVE_VISION_HP_VFPU == 1)
            || params->type == ELEMENT_WISE_ADD_FP16
            || params->type == ELEMENT_WISE_MUL_FP16
            || params->type == ELEMENT_WISE_DIV_FP16
            || params->type == ELEMENT_WISE_POW_FP16
#endif
            || params->type == ELEMENT_WISE_MAXIMUM
            || params->type == ELEMENT_WISE_MINIMUM
            || params->type == ELEMENT_WISE_SUB,
            "Invalid type of operation", XI_ERR_BADARG);
#endif
        XI_RUN_TIME_CHECK(params->bankInd == 0 || params->bankInd == 1,
            "Invalid bank size", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->activation == ACTIVATION_RELU_NONE
            || params->activation == ACTIVATION_RELU_0
            || params->activation == ACTIVATION_RELU_1
            || params->activation == ACTIVATION_RELU_6,
            "Invalid activation type", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->memTileSize >= 1,
            "Invalid Memory Tile Size", XI_ERR_BADARG);
    };

    XI_ERR_TYPE(*element_wise_func)(const xi_pTile3D inTile1,
        const xi_pTile3D inTile2,
        xi_pTile3D outTile,
        xi_pTile3D outmaskTile,
        const uint8_t * param);

    //if (params->type == ELEMENT_WISE_MUL) {
    //    element_wise_func = xiElementwiseMulA;
    //}
    //else if (params->type == ELEMENT_WISE_ADD) {
        element_wise_func = xiElementwiseAddA_I8;
    //}/*
    //else if (params->type == ELEMENT_WISE_MAX) {
    //    element_wise_func = xiElementwiseMaxA_U8;
    //}*/
    /*else if (params->type == ELEMENT_WISE_MAXIMUM) {
        element_wise_func = xiElementwiseMaximumA_I8;
    }
    else if (params->type == ELEMENT_WISE_MINIMUM) {
        element_wise_func = xiElementwiseMinimumA_I8;
    }
    else if (params->type == ELEMENT_WISE_SUB) {
        element_wise_func = xiElementwiseSubA_I8;
    }*/
//#if (XCHAL_HAVE_VISION_HP_VFPU == 1)
//    else if (params->type == ELEMENT_WISE_DIV_FP16) {
//        element_wise_func = xiElementwiseDivA_FP16;
//    }
//    else if (params->type == ELEMENT_WISE_MUL_FP16) {
//        element_wise_func = xiElementwiseMulA_FP16;
//    }
//    else if (params->type == ELEMENT_WISE_ADD_FP16) {
//        element_wise_func = xiElementwiseAddA_FP16;
//    }
//    else if (params->type == ELEMENT_WISE_POW_FP16) {
//        element_wise_func = xiElementwisePowA_FP16;
//    }
//#endif

    uint8_t* inputA_data = (uint8_t*)input->args[0];
    uint8_t* inputB_data = (uint8_t*)input->args[1];
    uint8_t* output_data = (uint8_t*)output->args[0];
    //uint8_t *outMask_data = (uint8_t *)output->args[1];

    uint8_t* inputA01 = NULL;
    uint8_t* inputA02 = NULL;
    uint8_t* inputB01 = NULL;
    uint8_t* inputB02 = NULL;
    uint8_t* output03 = NULL;
    uint8_t* output04 = NULL;
    uint8_t* outMask01 = NULL;
    uint8_t* outMask02 = NULL;

    xi_tile3D tiles[8];
    xi_pTile3D tile_inputA0, tile_inputA1, tile_inputB0, tile_inputB1, temp3D, tile_output0, tile_output1;
    xi_pTile3D tile_outMask0, tile_outMask1;
    // Number of tiles in each dimension 
    int numTilesD = (params->output.D + params->outpTile.D - 1) / params->outpTile.D;
    int numTilesW = (params->output.W + params->outpTile.W - 1) / params->outpTile.W;
    int numTilesH = (params->output.H + params->outpTile.H - 1) / params->outpTile.H;
    int doubleBuffer = numTilesD > 1 || numTilesW > 1 || numTilesH > 1 || params->batch > 1;

    // Max input tile dimensions 
    uint32_t inpATileD = (params->outpTile.D < params->inputA.D) ? params->outpTile.D : params->inputA.D;
    uint32_t inpATileW = (params->outpTile.W < params->inputA.W) ? params->outpTile.W : params->inputA.W;
    uint32_t inpATileH = (params->outpTile.H < params->inputA.H) ? params->outpTile.H : params->inputA.H;
    uint32_t inpATileSize = inpATileD * inpATileW * inpATileH * params->elemSize;

    uint32_t inpBTileD = (params->outpTile.D < params->inputB.D) ? params->outpTile.D : params->inputB.D;
    uint32_t inpBTileW = (params->outpTile.W < params->inputB.W) ? params->outpTile.W : params->inputB.W;
    uint32_t inpBTileH = (params->outpTile.H < params->inputB.H) ? params->outpTile.H : params->inputB.H;
    uint32_t inpBTileSize = inpBTileD * inpBTileW * inpBTileH * params->elemSize;

    uint32_t memInpTileSize, memOutpTileSize, memTotTileSize;
    uint32_t outpTileSize = params->outpTile.D * params->outpTile.W * params->outpTile.H * params->elemSize;
    int32_t  tileType;

    switch (params->tensorType.dataType) {
    case XI_S8:
        tileType = XI_TILE3D_S8;
        break;
    case XI_U8:
        tileType = XI_TILE3D_U8;
        break;
    case XI_S32:
        tileType = XI_TILE3D_S32;
        break;
#if (XCHAL_HAVE_VISION_HP_VFPU == 1)
    default:
        tileType = XI_TILE3D_F16;
        break;
#endif
    }
    if (doubleBuffer) {
        memTotTileSize = align_up(inpATileSize, ALIGNMENT) + align_up(inpBTileSize, ALIGNMENT) + align_up(outpTileSize, ALIGNMENT);
        memInpTileSize = memTotTileSize;
        memOutpTileSize = memTotTileSize;
    }
    else {
        memInpTileSize = align_up(inpATileSize, ALIGNMENT) + align_up(inpBTileSize, ALIGNMENT);
        memOutpTileSize = align_up(outpTileSize, ALIGNMENT);
    }

    // Reserve memory 
    XI_CHECK_RESULT(arena_init_two_banks_split(/*size0:*/ memInpTileSize, /* max_alignment0 */ ALIGNMENT, /* size1:*/ memOutpTileSize, /* max_alignment1 */ ALIGNMENT));
    // Allocate buffers 
    XI_CHECK_RESULT(arena_alloc((void**)(&inputA01),/* bank */ params->bankInd, /* size */ inpATileSize, /* alignment */ ALIGNMENT));
    XI_CHECK_RESULT(arena_alloc((void**)(&output03),/* bank */ (1 - params->bankInd), /* size */ outpTileSize, /* alignment */ ALIGNMENT));
    XI_CHECK_RESULT(arena_alloc((void**)(&inputB01),/* bank */ params->bankInd, /* size */ inpBTileSize, /* alignment */ ALIGNMENT));
    if ((params->type == ELEMENT_WISE_MAXIMUM) || (params->type == ELEMENT_WISE_MINIMUM)) {
        XI_CHECK_RESULT(arena_alloc((void**)(&outMask01),/* bank */ params->bankInd, /* size */ outpTileSize, /* alignment */ ALIGNMENT));
    }
    if (doubleBuffer) {
        XI_CHECK_RESULT(arena_alloc((void**)(&inputA02),/* bank */ (1 - params->bankInd), /* size*/ inpATileSize, /* alignment */ ALIGNMENT));
        XI_CHECK_RESULT(arena_alloc((void**)(&inputB02),/* bank */ (1 - params->bankInd), /* size*/ inpBTileSize, /* alignment */ ALIGNMENT));
        XI_CHECK_RESULT(arena_alloc((void**)(&output04),/* bank */ params->bankInd, /* size */ outpTileSize, /* alignment */ ALIGNMENT));
        if ((params->type == ELEMENT_WISE_MAXIMUM) || (params->type == ELEMENT_WISE_MINIMUM)) {
            XI_CHECK_RESULT(arena_alloc((void**)(&outMask02),/* bank */ 1 - params->bankInd, /* size */ outpTileSize, /* alignment */ ALIGNMENT));
        }
    }
    memset(&tiles, 0, sizeof(tiles));
    tile_inputA0 = (xi_pTile3D)&tiles[0];
    tile_inputA1 = (xi_pTile3D)&tiles[1];
    tile_inputB0 = (xi_pTile3D)&tiles[2];
    tile_inputB1 = (xi_pTile3D)&tiles[3];
    tile_output0 = (xi_pTile3D)&tiles[4];
    tile_output1 = (xi_pTile3D)&tiles[5];
    tile_outMask0 = (xi_pTile3D)&tiles[6];
    tile_outMask1 = (xi_pTile3D)&tiles[7];
    // Input Parameters
    XI_TILE3D_SET_BUFF_SIZE(tile_inputA0, inpATileSize);
    XI_TILE3D_SET_BUFF_PTR(tile_inputA0, inputA01);
    XI_TILE3D_SET_DATA_PTR(tile_inputA0, &(inputA01[0]));
    XI_TILE3D_SET_DATA_ORDER(tile_inputA0, XI_DWH);
    XI_TILE3D_SET_TYPE(tile_inputA0, tileType);
    XI_TILE3D_SET_DIM1(tile_inputA0, inpATileD);
    XI_TILE3D_SET_DIM2(tile_inputA0, inpATileW);
    XI_TILE3D_SET_DIM3(tile_inputA0, inpATileH);
    XI_TILE3D_SET_DIM1_PITCH(tile_inputA0, inpATileD);
    XI_TILE3D_SET_DIM2_PITCH(tile_inputA0, inpATileD * inpATileW);

    XI_TILE3D_SET_BUFF_SIZE(tile_inputA1, inpATileSize);
    XI_TILE3D_SET_BUFF_PTR(tile_inputA1, inputA02);
    XI_TILE3D_SET_DATA_PTR(tile_inputA1, &(inputA02[0]));
    XI_TILE3D_SET_DATA_ORDER(tile_inputA1, XI_DWH);
    XI_TILE3D_SET_TYPE(tile_inputA1, tileType);
    XI_TILE3D_SET_DIM1(tile_inputA1, inpATileD);
    XI_TILE3D_SET_DIM2(tile_inputA1, inpATileW);
    XI_TILE3D_SET_DIM3(tile_inputA1, inpATileH);
    XI_TILE3D_SET_DIM1_PITCH(tile_inputA1, inpATileD);
    XI_TILE3D_SET_DIM2_PITCH(tile_inputA1, inpATileD * inpATileW);

    XI_TILE3D_SET_BUFF_SIZE(tile_inputB0, inpBTileSize);
    XI_TILE3D_SET_BUFF_PTR(tile_inputB0, inputB01);
    XI_TILE3D_SET_DATA_PTR(tile_inputB0, &(inputB01[0]));
    XI_TILE3D_SET_DATA_ORDER(tile_inputB0, XI_DWH);
    XI_TILE3D_SET_TYPE(tile_inputB0, tileType);
    XI_TILE3D_SET_DIM1(tile_inputB0, inpBTileD);
    XI_TILE3D_SET_DIM2(tile_inputB0, inpBTileW);
    XI_TILE3D_SET_DIM3(tile_inputB0, inpBTileH);
    XI_TILE3D_SET_DIM1_PITCH(tile_inputB0, inpBTileD);
    XI_TILE3D_SET_DIM2_PITCH(tile_inputB0, inpBTileD * inpBTileW);

    XI_TILE3D_SET_BUFF_SIZE(tile_inputB1, inpBTileSize);
    XI_TILE3D_SET_BUFF_PTR(tile_inputB1, inputB02);
    XI_TILE3D_SET_DATA_PTR(tile_inputB1, &(inputB02[0]));
    XI_TILE3D_SET_DATA_ORDER(tile_inputB1, XI_DWH);
    XI_TILE3D_SET_TYPE(tile_inputB1, tileType);
    XI_TILE3D_SET_DIM1(tile_inputB1, inpBTileD);
    XI_TILE3D_SET_DIM2(tile_inputB1, inpBTileW);
    XI_TILE3D_SET_DIM3(tile_inputB1, inpBTileH);
    XI_TILE3D_SET_DIM1_PITCH(tile_inputB1, inpBTileD);
    XI_TILE3D_SET_DIM2_PITCH(tile_inputB1, inpBTileD * inpBTileW);

    // Output Parameters
    XI_TILE3D_SET_BUFF_SIZE(tile_output0, outpTileSize);
    XI_TILE3D_SET_BUFF_PTR(tile_output0, output03);
    XI_TILE3D_SET_DATA_PTR(tile_output0, &(output03[0]));
    XI_TILE3D_SET_DATA_ORDER(tile_output0, XI_DWH);
    XI_TILE3D_SET_TYPE(tile_output0, tileType);
    XI_TILE3D_SET_DIM1_PITCH(tile_output0, params->outpTile.D);
    XI_TILE3D_SET_DIM2_PITCH(tile_output0, params->outpTile.D * params->outpTile.W);

    XI_TILE3D_SET_BUFF_SIZE(tile_output1, outpTileSize);
    XI_TILE3D_SET_BUFF_PTR(tile_output1, output04);
    XI_TILE3D_SET_DATA_PTR(tile_output1, &(output04[0]));
    XI_TILE3D_SET_DATA_ORDER(tile_output1, XI_DWH);
    XI_TILE3D_SET_TYPE(tile_output1, tileType);
    XI_TILE3D_SET_DIM1_PITCH(tile_output1, params->outpTile.D);
    XI_TILE3D_SET_DIM2_PITCH(tile_output1, params->outpTile.D * params->outpTile.W);

    XI_TILE3D_SET_BUFF_SIZE(tile_outMask0, outpTileSize);
    XI_TILE3D_SET_BUFF_PTR(tile_outMask0, outMask01);
    XI_TILE3D_SET_DATA_PTR(tile_outMask0, &(outMask01[0]));
    XI_TILE3D_SET_DATA_ORDER(tile_outMask0, XI_DWH);
    XI_TILE3D_SET_TYPE(tile_outMask0, tileType);
    XI_TILE3D_SET_DIM1_PITCH(tile_outMask0, params->outpTile.D);
    XI_TILE3D_SET_DIM2_PITCH(tile_outMask0, params->outpTile.D * params->outpTile.W);

    XI_TILE3D_SET_BUFF_SIZE(tile_outMask1, outpTileSize);
    XI_TILE3D_SET_BUFF_PTR(tile_outMask1, outMask02);
    XI_TILE3D_SET_DATA_PTR(tile_outMask1, &(outMask02[0]));
    XI_TILE3D_SET_DATA_ORDER(tile_outMask1, XI_DWH);
    XI_TILE3D_SET_TYPE(tile_outMask1, tileType);
    XI_TILE3D_SET_DIM1_PITCH(tile_outMask1, params->outpTile.D);
    XI_TILE3D_SET_DIM2_PITCH(tile_outMask1, params->outpTile.D * params->outpTile.W);

    union {
        xi_cnn_mulA_params mul_params;
        xi_cnn_addA_params add_params;
        xi_cnn_subA_params sub_params;
        xi_cnn_eltwisemaxA_params  max_params;
        xi_cnn_eltwisemaxA_params  min_params;
    } temp_params;
    uint8_t* func_params_ptr = (uint8_t*)&temp_params;
    //uint8_t *func_params_ptr;
    //xi_cnn_addA_params func_params;
    // mul layer parameters
    if (params->type == ELEMENT_WISE_ADD) {
        XI_CNN_ADDA_SET_ZERO_POINT_IN1(&(temp_params.add_params), params->zeroPtInputA);
        XI_CNN_ADDA_SET_ZERO_POINT_IN2(&(temp_params.add_params), params->zeroPtInputB);
        XI_CNN_ADDA_SET_ZERO_POINT_OUT(&(temp_params.add_params), params->zeroPtOutput);
        XI_CNN_ADDA_SET_MULTIPLIER_IN1(&(temp_params.add_params), params->multiplierInpA);
        XI_CNN_ADDA_SET_MULTIPLIER_IN2(&(temp_params.add_params), params->multiplierInpB);
        XI_CNN_ADDA_SET_MULTIPLIER_OUT(&(temp_params.add_params), params->multiplierOut);
        XI_CNN_ADDA_SET_SHIFT_IN1(&(temp_params.add_params), params->shiftInpA);
        XI_CNN_ADDA_SET_SHIFT_IN2(&(temp_params.add_params), params->shiftInpB);
        XI_CNN_ADDA_SET_SHIFT_OUT(&(temp_params.add_params), params->shiftOut);
        XI_CNN_ADDA_SET_MIN_VAL(&(temp_params.add_params), params->minVal);
        XI_CNN_ADDA_SET_MAX_VAL(&(temp_params.add_params), params->maxVal);
        XI_CNN_ADDA_SET_LEFT_SHIFT(&(temp_params.add_params), params->left_shift);
        XI_CNN_ADDA_SET_QUANTIZATION_FLAG(&(temp_params.add_params), params->qSkipFlag);
    }
//    else if (params->type == ELEMENT_WISE_SUB) {
//        XI_CNN_SUBA_SET_ZERO_POINT_IN1(&(temp_params.sub_params), params->zeroPtInputA);
//        XI_CNN_SUBA_SET_ZERO_POINT_IN2(&(temp_params.sub_params), params->zeroPtInputB);
//        XI_CNN_SUBA_SET_ZERO_POINT_OUT(&(temp_params.sub_params), params->zeroPtOutput);
//        XI_CNN_SUBA_SET_MULTIPLIER_IN1(&(temp_params.sub_params), params->multiplierInpA);
//        XI_CNN_SUBA_SET_MULTIPLIER_IN2(&(temp_params.sub_params), params->multiplierInpB);
//        XI_CNN_SUBA_SET_MULTIPLIER_OUT(&(temp_params.sub_params), params->multiplierOut);
//        XI_CNN_SUBA_SET_SHIFT_IN1(&(temp_params.sub_params), params->shiftInpA);
//        XI_CNN_SUBA_SET_SHIFT_IN2(&(temp_params.sub_params), params->shiftInpB);
//        XI_CNN_SUBA_SET_SHIFT_OUT(&(temp_params.sub_params), params->shiftOut);
//        XI_CNN_SUBA_SET_MIN_VAL(&(temp_params.sub_params), params->minVal);
//        XI_CNN_SUBA_SET_MAX_VAL(&(temp_params.sub_params), params->maxVal);
//        XI_CNN_SUBA_SET_LEFT_SHIFT(&(temp_params.sub_params), params->left_shift);
//        XI_CNN_SUBA_SET_QUANTIZATION_FLAG(&(temp_params.add_params), params->qSkipFlag);
//    }
//    else if ((params->type == ELEMENT_WISE_MAXIMUM)) {
//        XI_CNN_ADDA_SET_ZERO_POINT_IN1(&(temp_params.max_params), params->zeroPtInputA);
//        XI_CNN_ADDA_SET_ZERO_POINT_IN2(&(temp_params.max_params), params->zeroPtInputB);
//        XI_CNN_ADDA_SET_ZERO_POINT_OUT(&(temp_params.max_params), params->zeroPtOutput);
//        XI_CNN_ADDA_SET_MULTIPLIER_IN1(&(temp_params.max_params), params->multiplierInpA);
//        XI_CNN_ADDA_SET_MULTIPLIER_IN2(&(temp_params.max_params), params->multiplierInpB);
//        XI_CNN_ADDA_SET_MULTIPLIER_OUT(&(temp_params.max_params), params->multiplierOut);
//        XI_CNN_ADDA_SET_SHIFT_IN1(&(temp_params.max_params), params->shiftInpA);
//        XI_CNN_ADDA_SET_SHIFT_IN2(&(temp_params.max_params), params->shiftInpB);
//        XI_CNN_ADDA_SET_SHIFT_OUT(&(temp_params.max_params), params->shiftOut);
//        XI_CNN_ADDA_SET_LEFT_SHIFT(&(temp_params.max_params), params->left_shift);
//    }
//    else if (params->type == ELEMENT_WISE_MINIMUM) {
//        XI_CNN_ADDA_SET_ZERO_POINT_IN1(&(temp_params.min_params), params->zeroPtInputA);
//        XI_CNN_ADDA_SET_ZERO_POINT_IN2(&(temp_params.min_params), params->zeroPtInputB);
//        //depot/dev/CNN/AndroidNN/v1/vision_ann/operations/topkv2.c	# edit
//        XI_CNN_ADDA_SET_ZERO_POINT_OUT(&(temp_params.min_params), params->zeroPtOutput);
//        XI_CNN_ADDA_SET_MULTIPLIER_IN1(&(temp_params.min_params), params->multiplierInpA);
//        XI_CNN_ADDA_SET_MULTIPLIER_IN2(&(temp_params.min_params), params->multiplierInpB);
//        XI_CNN_ADDA_SET_MULTIPLIER_OUT(&(temp_params.min_params), params->multiplierOut);
//        XI_CNN_ADDA_SET_SHIFT_IN1(&(temp_params.min_params), params->shiftInpA);
//        XI_CNN_ADDA_SET_SHIFT_IN2(&(temp_params.min_params), params->shiftInpB);
//        XI_CNN_ADDA_SET_SHIFT_OUT(&(temp_params.min_params), params->shiftOut);
//        XI_CNN_ADDA_SET_LEFT_SHIFT(&(temp_params.min_params), params->left_shift);
//    }
//    else if (params->type == ELEMENT_WISE_MUL) {
//        XI_CNN_MULA_SET_ZERO_POINT_IN1(&(temp_params.mul_params), params->zeroPtInputA);
//        XI_CNN_MULA_SET_ZERO_POINT_IN2(&(temp_params.mul_params), params->zeroPtInputB);
//        XI_CNN_MULA_SET_ZERO_POINT_OUT(&(temp_params.mul_params), params->zeroPtOutput);
//        XI_CNN_MULA_SET_MULTIPLIER_OUT(&(temp_params.mul_params), params->multiplierOut);
//        XI_CNN_MULA_SET_SHIFT_OUT(&(temp_params.mul_params), params->shiftOut);
//        XI_CNN_MULA_SET_MIN_VAL(&(temp_params.mul_params), params->minVal);
//        XI_CNN_MULA_SET_MAX_VAL(&(temp_params.mul_params), params->maxVal);
//    }
//#if (XCHAL_HAVE_VISION_HP_VFPU == 1)
//    else if (params->type == ELEMENT_WISE_ADD_FP16) {
//        func_params_ptr = (uint8_t*)&add_F16_params;
//        const eltwiseOp_fp16_params_t* params_FP16 = (eltwiseOp_fp16_params_t*)params;
//        XI_TILE3D_SET_TYPE(tile_inputA0, tileType);
//        XI_TILE3D_SET_TYPE(tile_inputA1, tileType);
//        XI_TILE3D_SET_TYPE(tile_inputB0, tileType);
//        XI_TILE3D_SET_TYPE(tile_inputB1, tileType);
//        XI_TILE3D_SET_TYPE(tile_output0, tileType);
//        XI_TILE3D_SET_TYPE(tile_output1, tileType);
//        XI_CNN_ADDA_SET_RELU_MIN(&(add_F16_params), params_FP16->minVal);
//        XI_CNN_ADDA_SET_RELU_MAX(&(add_F16_params), params_FP16->maxVal);
//        XI_CNN_ADDA_RESET_FLAG_RELU(&(add_F16_params));
//        if (params_FP16->activation != ACTIVATION_RELU_NONE)
//            XI_CNN_ADDA_SET_FLAG_RELU(&(add_F16_params));
//    }
//    else if (params->type == ELEMENT_WISE_DIV_FP16) {
//        func_params_ptr = (uint8_t*)&div_F16_params;
//        const eltwiseOp_params_t* params_FP16 = (eltwiseOp_params_t*)params;
//        XI_TILE3D_SET_TYPE(tile_inputA0, tileType);
//        XI_TILE3D_SET_TYPE(tile_inputA1, tileType);
//        XI_TILE3D_SET_TYPE(tile_inputB0, tileType);
//        XI_TILE3D_SET_TYPE(tile_inputB1, tileType);
//        XI_TILE3D_SET_TYPE(tile_output0, tileType);
//        XI_TILE3D_SET_TYPE(tile_output1, tileType);
//        XI_CNN_DIVA_SET_ACTIVATION_MIN(&(div_F16_params), (xb_f16)params_FP16->minVal_f32);
//        XI_CNN_DIVA_SET_ACTIVATION_MAX(&(div_F16_params), (xb_f16)params_FP16->maxVal_f32);
//        //    if (params_FP16->activation != ACTIVATION_RELU_NONE)
//          //      XI_CNN_ADDA_SET_FLAG_RELU(&(div_F16_params));
//    }
//    else if (params->type == ELEMENT_WISE_MUL_FP16) {
//        func_params_ptr = (uint8_t*)&mul_F16_params;
//        const eltwiseOp_fp16_params_t* params_FP16 = (eltwiseOp_fp16_params_t*)params;
//        XI_TILE3D_SET_TYPE(tile_inputA0, tileType);
//        XI_TILE3D_SET_TYPE(tile_inputA1, tileType);
//        XI_TILE3D_SET_TYPE(tile_inputB0, tileType);
//        XI_TILE3D_SET_TYPE(tile_inputB1, tileType);
//        XI_TILE3D_SET_TYPE(tile_output0, tileType);
//        XI_TILE3D_SET_TYPE(tile_output1, tileType);
//        XI_CNN_MULA_SET_RELU_MIN(&(mul_F16_params), params_FP16->minVal);
//        XI_CNN_MULA_SET_RELU_MAX(&(mul_F16_params), params_FP16->maxVal);
//        XI_CNN_ADDA_RESET_FLAG_RELU(&(mul_F16_params));
//        if (params_FP16->activation != ACTIVATION_RELU_NONE)
//            XI_CNN_ADDA_SET_FLAG_RELU(&(mul_F16_params));
//    }
//    else if (params->type == ELEMENT_WISE_POW_FP16) {
//        XI_TILE3D_SET_TYPE(tile_inputA0, tileType);
//        XI_TILE3D_SET_TYPE(tile_inputA1, tileType);
//        XI_TILE3D_SET_TYPE(tile_inputB0, tileType);
//        XI_TILE3D_SET_TYPE(tile_inputB1, tileType);
//        XI_TILE3D_SET_TYPE(tile_output0, tileType);
//        XI_TILE3D_SET_TYPE(tile_output1, tileType);
//    }
//#endif

    // Load first input tile 
    // Setup the output file and transfer
#if 0
    setup_outp_tile(0, 0, 0, tile_output0, params);
    if ((params->type == ELEMENT_WISE_MAXIMUM) || (params->type == ELEMENT_WISE_MINIMUM))
        setup_outp_tile(0, 0, 0, tile_outMask0, params);
    setup_inp_tile(tile_output0, tile_inputA0, 0, params);
    setup_inp_tile(tile_output0, tile_inputB0, 1, params);
    XI_TILE3D_SET_STATUS_FLAGS(tile_inputA0, 0);
    XI_TILE3D_SET_STATUS_FLAGS(tile_inputB0, 0);
    XI_TILE3D_SET_STATUS_FLAGS(tile_inputA1, -1);
    XI_TILE3D_SET_STATUS_FLAGS(tile_inputB1, -1);
    transfer_inp_tile(&(inputA_data[0]), tile_inputA0, 0, params);
    transfer_inp_tile(&(inputB_data[0]), tile_inputB0, 1, params);
    /* Wait for first tiles to arrive */
    XI_CHECK_RESULT(dma_barrier());
#endif 
    /* Split D dimension equally on each core */
    /* process the segment of D assigned to this core */
    int32_t Dstart, Dend, Dcount;
    Dcount = (numTilesD + getTotalCores() - 1) / getTotalCores();
    Dstart = Dcount * getMyCore();
    Dend = min(numTilesD, (Dstart + Dcount));
    if (Dstart >= Dend)
        return XI_ERROR_STATUS();

    /* Prepare first output tile */
    setup_outp_tile(Dstart * params->outpTile.D, 0, 0, tile_output0, params);
    if ((params->type == ELEMENT_WISE_MAXIMUM) || (params->type == ELEMENT_WISE_MINIMUM))
        setup_outp_tile(Dstart * params->outpTile.D, 0, 0, tile_outMask0, params);
    setup_inp_tile(tile_output0, tile_inputA0, 0, params);
    setup_inp_tile(tile_output0, tile_inputB0, 1, params);
    XI_TILE3D_SET_STATUS_FLAGS(tile_inputA0, 0);
    XI_TILE3D_SET_STATUS_FLAGS(tile_inputB0, 0);
    XI_TILE3D_SET_STATUS_FLAGS(tile_inputA1, -1);
    XI_TILE3D_SET_STATUS_FLAGS(tile_inputB1, -1);
    transfer_inp_tile(&(inputA_data[0]), tile_inputA0, 0, params);
    transfer_inp_tile(&(inputB_data[0]), tile_inputB0, 1, params);
    /* Wait for first tiles to arrive */
    XI_CHECK_RESULT(dma_barrier());

    int32_t batchSize = (params->batchA >= params->batchB) ? params->batchA : params->batchB;
    uint32_t lastD, lastW, lastH, lastB;
    int32_t nextBA, nextBB;
    for (int32_t B = 0; B < batchSize; B++) {
        lastB = (B == batchSize - 1);
        for (int32_t H = 0; H < numTilesH; H++) {
            lastH = lastB && (H == (numTilesH - 1));
            for (int32_t W = 0; W < numTilesW; W++) {
                lastW = lastH && (W == (numTilesW - 1));
                for (int32_t D = Dstart; D < Dend; D++) {
                    lastD = lastW && (D == (Dend - 1));
                    if (!lastD) {
                        int32_t nextB, nextD, nextW, nextH;
                        int32_t prevAD = XI_TILE3D_GET_DIM1_COORD(tile_inputA1);
                        int32_t prevAW = XI_TILE3D_GET_DIM2_COORD(tile_inputA1);
                        int32_t prevAH = XI_TILE3D_GET_DIM3_COORD(tile_inputA1);
                        int32_t prevAB = XI_TILE3D_GET_STATUS_FLAGS(tile_inputA1);
                        int32_t prevBD = XI_TILE3D_GET_DIM1_COORD(tile_inputB1);
                        int32_t prevBW = XI_TILE3D_GET_DIM2_COORD(tile_inputB1);
                        int32_t prevBH = XI_TILE3D_GET_DIM3_COORD(tile_inputB1);
                        int32_t prevBB = XI_TILE3D_GET_STATUS_FLAGS(tile_inputB1);

                        inc_iter_to_temp(&nextB, B, batchSize, inc_iter_to_temp(&nextH, H, numTilesH, inc_iter_to_temp(&nextW, W, numTilesW, inc_iter_to_temp(&nextD, D, Dend, 1))));
                        nextD = (nextD) ? nextD : Dstart;
                        setup_outp_tile(nextD * params->outpTile.D, nextW * params->outpTile.W, nextH * params->outpTile.H, tile_output1, params);
                        if ((params->type == ELEMENT_WISE_MAXIMUM) || (params->type == ELEMENT_WISE_MINIMUM))
                            setup_outp_tile(nextD * params->outpTile.D, nextW * params->outpTile.W, nextH * params->outpTile.H, tile_outMask1, params);
                        setup_inp_tile(tile_output1, tile_inputA1, 0, params);
                        setup_inp_tile(tile_output1, tile_inputB1, 1, params);
                        prevAB = XI_TILE3D_GET_STATUS_FLAGS(tile_inputA1);
                        prevBB = XI_TILE3D_GET_STATUS_FLAGS(tile_inputB1);
                        nextBA = (params->batchA == 1) ? 0 : nextB;
                        nextBB = (params->batchB == 1) ? 0 : nextB;
                        // Check for co-ordinates of tile
                        if (!(prevAD == XI_TILE3D_GET_DIM1_COORD(tile_inputA1)
                            && prevAW == XI_TILE3D_GET_DIM2_COORD(tile_inputA1)
                            && prevAH == XI_TILE3D_GET_DIM3_COORD(tile_inputA1)
                            && nextBA == prevAB)) {
                            transfer_inp_tile(&inputA_data[params->elemSize * nextBA * (params->inputA.D * params->inputA.W * params->inputA.H)], tile_inputA1, 0, params);
                        }
                        if (!(prevBD == XI_TILE3D_GET_DIM1_COORD(tile_inputB1)
                            && prevBW == XI_TILE3D_GET_DIM2_COORD(tile_inputB1)
                            && prevBH == XI_TILE3D_GET_DIM3_COORD(tile_inputB1)
                            && nextBB == prevBB)) {
                            transfer_inp_tile(&inputB_data[params->elemSize * nextBB * (params->inputB.D * params->inputB.W * params->inputB.H)], tile_inputB1, 1, params);
                        }
                        XI_TILE3D_SET_STATUS_FLAGS(tile_inputA1, nextBA);
                        XI_TILE3D_SET_STATUS_FLAGS(tile_inputB1, nextBB);
                    }
                    INST_KERNEL_BEGIN();
                    XI_CHECK_RESULT((*element_wise_func)(tile_inputA0, tile_inputB0, tile_output0, tile_outMask0, func_params_ptr));
                    INST_KERNEL_END();
                    // Transfer output tile
                    transfer_outp_tile(&output_data[params->elemSize * B * (params->output.D * params->output.W * params->output.H)], tile_output0, params);
                    /*
                    if (params->type == ELEMENT_WISE_MAX)   // no MAXIMUM or MINIMUM
                        transfer_outp_tile(&outMask_data[params->elemSize * B * (params->output.D * params->output.W * params->output.H)], tile_outMask0, params);
                    */
                    // wait for overlapping DMA transfer               
                    XI_CHECK_RESULT(dma_barrier());
                    temp3D = tile_output0; tile_output0 = tile_output1; tile_output1 = temp3D;
                    temp3D = tile_inputA0; tile_inputA0 = tile_inputA1; tile_inputA1 = temp3D;
                    temp3D = tile_inputB0; tile_inputB0 = tile_inputB1; tile_inputB1 = temp3D;
                    temp3D = tile_outMask0; tile_outMask0 = tile_outMask1; tile_outMask1 = temp3D;
                }
            }
        }
    }
#if IS_MULTICHANNEL_DMA
    /* waita for the last transfer */
    XI_CHECK_RESULT(dma_barrier());
    /* Don't reset arena if last dma_barrier is not executed as  DMA transfer is still ongoing */
    XI_CHECK_RESULT(arena_reset());
#endif
    return XI_ERROR_STATUS();
}

