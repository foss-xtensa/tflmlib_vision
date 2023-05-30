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
#include "flk_softmax.h"
#include "utils.h"

/* Setup output tile based on coordinates and params. */
static inline void
setup_outp_tile(int D, int X, int Y, xi_pTile3D outp, const softmax_params_t* params)
{
    /* Set output tile coordinates */
    XI_TILE3D_SET_DIM1_COORD(outp, D);
    XI_TILE3D_SET_DIM2_COORD(outp, X);
    XI_TILE3D_SET_DIM3_COORD(outp, Y);

    /* Compute output tile dimensions from intersection of output tile size with output data */
    XI_TILE3D_SET_DIM1(outp, min(D + params->outTile.D, params->input.D) - D);
    XI_TILE3D_SET_DIM2(outp, min(X + params->outTile.W, params->input.W) - X);
    XI_TILE3D_SET_DIM3(outp, min(Y + params->outTile.H, params->input.H) - Y);

    /* Update output tile pitch according to tile size */
    XI_TILE3D_SET_DIM1_PITCH(outp, XI_TILE3D_GET_DIM1(outp));
    XI_TILE3D_SET_DIM2_PITCH(outp, XI_TILE3D_GET_DIM2(outp) * XI_TILE3D_GET_DIM1(outp));
}

/* Setup input tile coordinates/dimensions based on output tile coordinates/dimensions and params. */
static inline void
setup_inp_tile(const xi_pTile3D outp, xi_pTile3D inp, const softmax_params_t* params)
{
    uint32_t inputD, inputW, inputH;

    inputD = params->input.D;
    inputW = params->input.W;
    inputH = params->input.H;


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
transfer_inp_tile(uint8_t* inputPtr, xi_pTile3D inp, const softmax_params_t* params)
{
    uint32_t inputD = params->input.D;
    uint32_t inputW = params->input.W;
    uint32_t inputH = params->input.H;

    int boundX = XI_TILE3D_GET_DIM2_COORD(inp);
    int boundY = XI_TILE3D_GET_DIM3_COORD(inp);
    int boundW = XI_TILE3D_GET_DIM2_COORD(inp) + XI_TILE3D_GET_DIM2(inp);
    int boundH = XI_TILE3D_GET_DIM3_COORD(inp) + XI_TILE3D_GET_DIM3(inp);

    int validX = max(0, boundX);
    int validY = max(0, boundY);
    int validW = min(boundW, inputW) - validX;
    int validH = min(boundH, inputH) - validY;

    uint8_t* tilePtr = (uint8_t*)XI_TILE3D_GET_BUFF_PTR(inp);
    dma_3d_sys2loc_dyn(/* src */ inputPtr + (params->elemSize * ((validY * inputW * inputD) + (validX * inputD) + XI_TILE3D_GET_DIM1_COORD(inp))),
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
transfer_outp_tile(uint8_t* outputPtr, xi_pTile3D outp, const softmax_params_t* params)
{

    int D = XI_TILE3D_GET_DIM1_COORD(outp);
    int X = XI_TILE3D_GET_DIM2_COORD(outp);
    int Y = XI_TILE3D_GET_DIM3_COORD(outp);

    uint8_t* tilePtr = (uint8_t*)XI_TILE3D_GET_BUFF_PTR(outp);
#if IS_MULTICHANNEL_DMA 
    dma_3d_loc2sys_dyn_ch(/*channel*/ 1,
        /* src */ tilePtr,
        /* dst */ outputPtr + params->elemSize * (Y * params->input.D * params->input.W + X * params->input.D + D),
        /* row size */      params->elemSize * XI_TILE3D_GET_DIM1(outp),
        /* src stride 2d */ params->elemSize * XI_TILE3D_GET_DIM1_PITCH(outp),
        /* dst stride 2d */ params->elemSize * params->input.D,
        /* count 2d */ XI_TILE3D_GET_DIM2(outp),
        /* src stride 3d */ params->elemSize * XI_TILE3D_GET_DIM2_PITCH(outp),
        /* dst stride 3d */ params->elemSize * params->input.D * params->input.W,
        /* count 3d */ XI_TILE3D_GET_DIM3(outp));

#else
    dma_3d_loc2sys_dyn(/* src */ tilePtr,
        /* dst */ outputPtr + params->elemSize * (Y * params->input.D * params->input.W + X * params->input.D + D),
        /* row size */      params->elemSize * XI_TILE3D_GET_DIM1(outp),
        /* src stride 2d */ params->elemSize * XI_TILE3D_GET_DIM1_PITCH(outp),
        /* dst stride 2d */ params->elemSize * params->input.D,
        /* count 2d */ XI_TILE3D_GET_DIM2(outp),
        /* src stride 3d */ params->elemSize * XI_TILE3D_GET_DIM2_PITCH(outp),
        /* dst stride 3d */ params->elemSize * params->input.D * params->input.W,
        /* count 3d */ XI_TILE3D_GET_DIM3(outp));
#endif
}

XI_ERR_TYPE xiSoftmaxQuant8(const xi_pTile3D inTile,
    const xi_pArray softmaxArray,
    xi_pTile3D outTile,
    uint8_t* func_params,
    xi_pArray scratchArray)
{
    XI_ERR_TYPE status = XI_ERR_OK;
    (void)scratchArray;
    //Based on the tile type the ASYMM or ASYMM_SIGNED variant of Softmax gets called
    status = xiSoftmaxA3D_S8_LUT(inTile, softmaxArray, outTile, (const xi_cnn_softmaxA_params*)func_params, scratchArray);
    return status;

}

XI_ERR_TYPE flk_softmax(const uint8_t* raw_params,
    struct XtensaOperationArgsIn* input,
    struct XtensaOperationArgsOut* output)
{
#if DEBUG_LEVEL > 1
    print_softmax_params(raw_params);
#endif
    const softmax_params_t* params = (const softmax_params_t*)raw_params;

    XI_ERROR_CHECKS() {
        XI_RUN_TIME_CHECK(params != NULL,
            "Params cannot be NULL", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->structSize == sizeof(*params),
            "Params structure size is incorrect", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(input != NULL
            && input->numArgs == 1
            && input->args[0] != NULL && input->argsSize[0] > 0,
            "Invalid input args", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(output != NULL
            && output->numArgs == 1
            && output->args[0] != NULL && output->argsSize[0] > 0,
            "Invalid output args", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->batch >= 1,
            "Invalid batch size", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->input.D >= 1
            && params->input.W >= 1
            && params->input.H >= 1
            && params->input.D * params->input.W * params->input.H * params->batch <= input->argsSize[0],
            "Inconsistent input dimensions", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->bankInd == 0 || params->bankInd == 1,
            "Invalid bank size", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->memTileSize >= 1,
            "Invalid bank size", XI_ERR_BADARG);
    };

    XI_ERR_TYPE(*softmax_func)(const xi_pTile3D inTile,
        const xi_pArray softmaxArray,
        xi_pTile3D outTile,
        uint8_t * param,
        xi_pArray scratchArray
        );

    softmax_func = xiSoftmaxQuant8;

    int numTilesD = (params->input.D + params->outTile.D - 1) / params->outTile.D;
    int numTilesW = (params->input.W + params->outTile.W - 1) / params->outTile.W;
    int numTilesH = (params->input.H + params->outTile.H - 1) / params->outTile.H;

    uint8_t* input_data = (uint8_t*)input->args[0];
    uint8_t* output_data = (uint8_t*)output->args[0];

    uint8_t* input01 = NULL;
    uint8_t* input02 = NULL;
    uint8_t* output03 = NULL;
    uint8_t* output04 = NULL;
    uint8_t* LUT = NULL;
    uint8_t* scratchData = NULL;

    xi_tile3D tiles[8];
    xi_pTile3D tile_input0, tile_input1, temp3D;
    xi_pTile3D tile_output0, tile_output1;

    xi_array memArray;
    xi_pArray softmaxArray;
    xi_array scratchArr;

    xi_cnn_softmaxA_params       softmax_params;
    uint8_t* func_params_ptr = (uint8_t*)&softmax_params;
    int32_t tile_type = (params->tensorType.dataType == XI_S8) ? XI_TILE3D_S8 : (params->tensorType.dataType == XI_U8) ? XI_TILE3D_U8 : XI_TILE3D_F16;
#if (XCHAL_HAVE_VISION_HP_VFPU == 1)
    xi_cnn_softmaxA3D_F16_params softmaxF16_params;
#endif

    uint32_t tileSize = params->outTile.D * params->outTile.W * params->outTile.H * params->elemSize;
    /* Reserve memory */
    XI_CHECK_RESULT(arena_init_two_banks_split(/*size0:*/  params->memTileSize, /* max_alignment0 */ ALIGNMENT, /* size1:*/  params->memTileSize, /* max_alignment1 */ ALIGNMENT));
    /* Allocate buffers */
    XI_CHECK_RESULT(arena_alloc((void**)(&input01),/* bank */ params->bankInd, /* size */ tileSize, /* alignment */ ALIGNMENT));
    XI_CHECK_RESULT(arena_alloc((void**)(&output03),/* bank */1 - params->bankInd, /* size */ tileSize, /* alignment */ ALIGNMENT));
    if (params->doubleBuffer) {
        XI_CHECK_RESULT(arena_alloc((void**)(&input02),/* bank */ params->bankInd, /* size */ tileSize, /* alignment */ ALIGNMENT));
        XI_CHECK_RESULT(arena_alloc((void**)(&output04),/* bank */ 1 - params->bankInd, /* size */ tileSize, /* alignment */ ALIGNMENT));
    }
    XI_CHECK_RESULT(arena_alloc((void**)(&LUT),/* bank */ params->bankInd, /* size */ 256 * sizeof(int32_t), /* alignment */ ALIGNMENT));

    if (params->intermediateArrSize) {
        XI_CHECK_RESULT(arena_alloc((void**)(&scratchData),/* bank */ 1 - params->bankInd, /* size */ params->intermediateArrSize, /* alignment */ ALIGNMENT));
    }

    memset(&tiles, 0, sizeof(tiles));
    memset(&memArray, 0, sizeof(memArray));
    tile_input0 = (xi_pTile3D)&tiles[0];
    tile_input1 = (xi_pTile3D)&tiles[1];
    tile_output0 = (xi_pTile3D)&tiles[2];
    tile_output1 = (xi_pTile3D)&tiles[3];
    softmaxArray = (xi_pArray)&memArray;

    // Input Parameters
    XI_TILE3D_SET_BUFF_SIZE(tile_input0, tileSize);
    XI_TILE3D_SET_BUFF_PTR(tile_input0, input01);
    XI_TILE3D_SET_DATA_PTR(tile_input0, &(input01[0]));
    XI_TILE3D_SET_DATA_ORDER(tile_input0, XI_DWH);
    XI_TILE3D_SET_TYPE(tile_input0, tile_type);
    XI_TILE3D_SET_DIM1(tile_input0, params->outTile.D);
    XI_TILE3D_SET_DIM2(tile_input0, params->outTile.W);
    XI_TILE3D_SET_DIM3(tile_input0, params->outTile.H);
    XI_TILE3D_SET_DIM1_PITCH(tile_input0, params->outTile.D);
    XI_TILE3D_SET_DIM2_PITCH(tile_input0, params->outTile.D * params->outTile.W);

    XI_ARRAY_SET_BUFF_PTR(&scratchArr, scratchData);
    if (params->intermediateArrSize) {
        // Intermediate array parameters
        XI_ARRAY_SET_TYPE(&scratchArr, params->tensorType.dataType);
        XI_ARRAY_SET_DATA_PTR(&scratchArr, scratchData);
        XI_ARRAY_SET_BUFF_SIZE(&scratchArr, params->intermediateArrSize);
        XI_ARRAY_SET_WIDTH(&scratchArr, params->intermediateArrSize);
        XI_ARRAY_SET_PITCH(&scratchArr, params->intermediateArrSize);
        XI_ARRAY_SET_HEIGHT(&scratchArr, 1);
    }

    XI_TILE3D_SET_BUFF_SIZE(tile_input1, tileSize);
    XI_TILE3D_SET_BUFF_PTR(tile_input1, input02);
    XI_TILE3D_SET_DATA_PTR(tile_input1, &(input02[0]));
    XI_TILE3D_SET_DATA_ORDER(tile_input1, XI_DWH);
    XI_TILE3D_SET_TYPE(tile_input1, tile_type);
    XI_TILE3D_SET_DIM1(tile_input1, params->outTile.D);
    XI_TILE3D_SET_DIM2(tile_input1, params->outTile.W);
    XI_TILE3D_SET_DIM3(tile_input1, params->outTile.H);
    XI_TILE3D_SET_DIM1_PITCH(tile_input1, params->outTile.D);
    XI_TILE3D_SET_DIM2_PITCH(tile_input1, params->outTile.D * params->outTile.W);

    // Output Parameters
    XI_TILE3D_SET_BUFF_SIZE(tile_output0, tileSize);
    XI_TILE3D_SET_BUFF_PTR(tile_output0, output03);
    XI_TILE3D_SET_DATA_PTR(tile_output0, &(output03[0]));
    XI_TILE3D_SET_DATA_ORDER(tile_output0, XI_DWH);
    XI_TILE3D_SET_TYPE(tile_output0, tile_type);
    XI_TILE3D_SET_DIM1(tile_output0, params->outTile.D);
    XI_TILE3D_SET_DIM2(tile_output0, params->outTile.W);
    XI_TILE3D_SET_DIM3(tile_output0, params->outTile.H);
    XI_TILE3D_SET_DIM1_PITCH(tile_output0, params->outTile.D);
    XI_TILE3D_SET_DIM2_PITCH(tile_output0, params->outTile.D * params->outTile.W);

    XI_TILE3D_SET_BUFF_SIZE(tile_output1, tileSize);
    XI_TILE3D_SET_BUFF_PTR(tile_output1, output04);
    XI_TILE3D_SET_DATA_PTR(tile_output1, &(output04[0]));
    XI_TILE3D_SET_DATA_ORDER(tile_output1, XI_DWH);
    XI_TILE3D_SET_TYPE(tile_output1, tile_type);
    XI_TILE3D_SET_DIM1(tile_output1, params->outTile.D);
    XI_TILE3D_SET_DIM2(tile_output1, params->outTile.W);
    XI_TILE3D_SET_DIM3(tile_output1, params->outTile.H);
    XI_TILE3D_SET_DIM1_PITCH(tile_output1, params->outTile.D);
    XI_TILE3D_SET_DIM2_PITCH(tile_output1, params->outTile.D * params->outTile.W);

    // Softmax LUT parameters
    XI_ARRAY_SET_BUFF_SIZE(softmaxArray, 256 * sizeof(int32_t));
    XI_ARRAY_SET_BUFF_PTR(softmaxArray, LUT);
    XI_ARRAY_SET_DATA_PTR(softmaxArray, &(LUT[0]));
    XI_ARRAY_SET_WIDTH(softmaxArray, 256);
    XI_ARRAY_SET_PITCH(softmaxArray, 256);
    XI_ARRAY_SET_HEIGHT(softmaxArray, 1);
    XI_ARRAY_SET_STATUS_FLAGS(softmaxArray, 1);
    XI_ARRAY_SET_TYPE(softmaxArray, XI_ARRAY_S32);
    XI_ARRAY_SET_CAPACITY(softmaxArray, 256);

    if (params->elemSize == 1) {
        func_params_ptr = (uint8_t*)&softmax_params;
        XI_CNN_SOFTMAXA_PARAMS_SET_DIFF_MIN(&(softmax_params), params->diff_min);
        XI_CNN_SOFTMAXA_PARAMS_SET_MULTIPLIER(&(softmax_params), params->input_beta_multiplier);
        XI_CNN_SOFTMAXA_PARAMS_SET_LEFT_SHIFT(&(softmax_params), params->input_beta_left_shift);
        XI_CNN_SOFTMAXA_PARAMS_SET_AXIS(&(softmax_params), params->axis);
    }
#if (XCHAL_HAVE_VISION_HP_VFPU == 1)
    else if (params->elemSize == 2)
    {
        func_params_ptr = (uint8_t*)&softmaxF16_params;
        XI_TILE3D_SET_TYPE(tile_input0, tile_type);
        XI_TILE3D_SET_TYPE(tile_input1, tile_type);
        XI_TILE3D_SET_TYPE(tile_output0, tile_type);
        XI_TILE3D_SET_TYPE(tile_output1, tile_type);
        XI_CNN_SOFTMAXAF16_PARAMS_SET_AXIS(&(softmaxF16_params), params->axis);
        XI_CNN_SOFTMAXAF16_PARAMS_SET_BETA(&(softmaxF16_params), (xb_f16)params->beta);
    }
#endif
    /* Generate LUT Array */
    XI_CHECK_RESULT(xiSoftmaxA_generateLUT(softmaxArray, (xi_cnn_softmaxA_params*)func_params_ptr));
    int batchSize = params->batch;

    /* Split D equally for each core */
    /* processing the section of D dimension in current core */
    int32_t Dstart, Dend, Dcount;
    Dcount = (numTilesD + getTotalCores() - 1) / getTotalCores();
    Dstart = Dcount * getMyCore();
    Dend = min(numTilesD, (Dstart + Dcount));
    if (Dstart >= Dend)
        return XI_ERROR_STATUS();

    setup_outp_tile(Dstart * params->outTile.D, 0, 0, tile_output0, params);
    setup_inp_tile(tile_output0, tile_input0, params);
    XI_TILE3D_SET_STATUS_FLAGS(tile_input0, 0);
    XI_TILE3D_SET_STATUS_FLAGS(tile_input1, -1);
    transfer_inp_tile(&(input_data[0]), tile_input0, params);
    /* Wait for first tiles to arrive */
    XI_CHECK_RESULT(dma_barrier());
    batchSize = params->batch;
    uint32_t lastD, lastW, lastH, lastB;
    int32_t nextBA;

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
                        int32_t prevAD = XI_TILE3D_GET_DIM1_COORD(tile_input1);
                        int32_t prevAW = XI_TILE3D_GET_DIM2_COORD(tile_input1);
                        int32_t prevAH = XI_TILE3D_GET_DIM3_COORD(tile_input1);
                        int32_t prevAB = XI_TILE3D_GET_STATUS_FLAGS(tile_input1);
                        inc_iter_to_temp(&nextB, B, batchSize, inc_iter_to_temp(&nextH, H, numTilesH, inc_iter_to_temp(&nextW, W, numTilesW, inc_iter_to_temp(&nextD, D, Dend, 1))));
                        nextD = (nextD) ? nextD : Dstart;
                        setup_outp_tile(nextD * params->outTile.D, nextW * params->outTile.W, nextH * params->outTile.H, tile_output1, params);
                        setup_inp_tile(tile_output1, tile_input1, params);
                        prevAB = XI_TILE3D_GET_STATUS_FLAGS(tile_input1);
                        nextBA = (params->batch == 1) ? 0 : nextB;
                        // Check for co-ordinates of tile
                        if (!(prevAD == XI_TILE3D_GET_DIM1_COORD(tile_input1)
                            && prevAW == XI_TILE3D_GET_DIM2_COORD(tile_input1)
                            && prevAH == XI_TILE3D_GET_DIM3_COORD(tile_input1)
                            && nextBA == prevAB)) {
                            transfer_inp_tile(&input_data[nextBA * params->elemSize * (params->input.D * params->input.W * params->input.H)], tile_input1, params);
                        }
                        XI_TILE3D_SET_STATUS_FLAGS(tile_input1, nextBA);
                    }
                    INST_KERNEL_BEGIN();
                    XI_CHECK_RESULT((*softmax_func)(tile_input0, softmaxArray, tile_output0, (func_params_ptr), &scratchArr));
                    INST_KERNEL_END();
                    // Transfer output tile
                    transfer_outp_tile(&output_data[B * params->elemSize * (params->input.D * params->input.W * params->input.H)], tile_output0, params);
                    // wait for overlapping DMA transfer               
                    XI_CHECK_RESULT(dma_barrier());
                    temp3D = tile_output0; tile_output0 = tile_output1; tile_output1 = temp3D;
                    temp3D = tile_input0; tile_input0 = tile_input1; tile_input1 = temp3D;
                    //temp3D = tile_outputA; tile_outputA = tile_outputB; tile_outputB = temp3D;
                    //temp3D = tile_inputA; tile_inputA = tile_inputB; tile_inputB = temp3D; 
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
    return(XI_ERROR_STATUS());
}
