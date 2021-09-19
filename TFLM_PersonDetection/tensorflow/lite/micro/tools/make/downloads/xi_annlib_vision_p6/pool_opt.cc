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
#include <string.h>
#include <assert.h>
#include "cnnrt.h"
#include "xi_tile3d_manager.h"
#include "flk_pool.h"
#include "utils.h"

#define max(a, b)                         ((a > b) ? a : b)
#define min(a, b)                         ((a > b) ? b : a)

#if DEBUG_LEVEL > 1
void print_pool_params(const uint8_t *raw_params){
    const pool_params_t *params = (const pool_params_t *)raw_params;
    if (params == NULL) {
        DEBUG_LOG2("# error: params is NULL\n");
        return;
    }

    printf("# ===== Pool Params =====\n"
               "#   StructSize: %d \n",
               params->structSize);
    print_dims3d("  Input  ", params->input);
    print_dims3d("  Output  ", params->output);
    print_dims3d("  Tile  ", params->tile);
    printf("#   Batch: %d\n"
               "#   Kernel (W, H): (%d, %d)\n"
               "#   Stride: %d\n"
               "#   Offset (X, Y): (%d, %d)\n"
               "#   Zero points (Input, Output): (%d, %d)\n"
               "#   POOL_TYPE: %s\n"
               "#   Re-quantization Flag: %d\n"
               "#   Multiplier Out : %d\n"
               "#   Shift Out : %d\n"
               "#   Left Shift : %d\n"
               "#   Activation: %d\n"
               "#   Relu (Min,Max): (%d, %d)\n"
               "#   Bank Index: %d \n"
               "#   Mem. Req. (Bank0, Bank1): (%d, %d) \n"
               , params->batch
               , params->kernelW, params->kernelH
               , params->stride
               , params->offsetX, params->offsetY
               , params->zeroPtInput, params->zeroPtOutput
               , ((params->type == MAX_POOLING ) ? "MAX_POOL" : ((params->type == AVG_POOLING) ? "AVG_POOL" : "L2_POOL"))
               , params->qFlag
               , params->multiplierOut, params->shiftOut, params->left_shift
               , params->activation
               , params->reluMin, params->reluMax
               , params->largeInd
               , params->memInpTile, params->memOutpTile);

}
#endif

// Setup output tile based on coordinates and params.
static inline void
setup_outp_tile(int D, int X, int Y, xi_pTile3D outp, const pool_params_t *params)
{
    // Set output tile coordinates
    XI_TILE3D_SET_DIM1_COORD(outp, D);
    XI_TILE3D_SET_DIM2_COORD(outp, X);
    XI_TILE3D_SET_DIM3_COORD(outp, Y);

    // Compute output tile dimensions from intersection of output tile size with output data
    XI_TILE3D_SET_DIM1(outp, min(D + params->tile.D, params->output.D) - D);
    XI_TILE3D_SET_DIM2(outp, min(X + params->tile.W, params->output.W) - X);
    XI_TILE3D_SET_DIM3(outp, min(Y + params->tile.H, params->output.H) - Y);

    // Update output tile pitch according to tile size
    XI_TILE3D_SET_DIM1_PITCH(outp, XI_TILE3D_GET_DIM1(outp));
    XI_TILE3D_SET_DIM2_PITCH(outp, XI_TILE3D_GET_DIM2(outp) * XI_TILE3D_GET_DIM1(outp));
}

// Setup input tile coordinates/dimensions based on output tile coordinates/dimensions and params
static inline void
setup_inp_tile(const xi_pTile3D outp, xi_pTile3D inp, const pool_params_t *params)
{
    int X = XI_TILE3D_GET_DIM2_COORD(outp);
    int Y = XI_TILE3D_GET_DIM3_COORD(outp);

    // Compute input tile coordinates
    XI_TILE3D_SET_DIM1_COORD(inp, XI_TILE3D_GET_DIM1_COORD(outp));
    XI_TILE3D_SET_DIM2_COORD(inp, X * params->stride + params->offsetX);
    XI_TILE3D_SET_DIM3_COORD(inp, Y * params->stride + params->offsetY);

    // Compute input tile dimensions
    XI_TILE3D_SET_DIM1(inp,  XI_TILE3D_GET_DIM1(outp));
    XI_TILE3D_SET_DIM2(inp, (XI_TILE3D_GET_DIM2(outp) - 1) * params->stride + 1);
    XI_TILE3D_SET_DIM3(inp, (XI_TILE3D_GET_DIM3(outp) - 1) * params->stride + 1);

    // Update input tile pitch according to tile size
    XI_TILE3D_SET_DIM2_PITCH(inp, (XI_TILE3D_GET_DIM2(inp) + XI_TILE3D_GET_DIM2_EDGE1(inp) + XI_TILE3D_GET_DIM2_EDGE2(inp)) * XI_TILE3D_GET_DIM1_PITCH(inp));

    // Update data pointer, edge in depth is always 0
    XI_TILE3D_SET_DATA_PTR(inp, (uint8_t*)XI_TILE3D_GET_BUFF_PTR(inp)
                           + XI_TILE3D_GET_DIM1_PITCH(inp) * XI_TILE3D_GET_DIM2_EDGE1(inp)
                           + XI_TILE3D_GET_DIM2_PITCH(inp) * XI_TILE3D_GET_DIM3_EDGE1(inp));
}

/* For input tile at (X, Y) of (W, H) with edges find intersection with input data and amount of padding needed.
   Schedules DMA transfer to bring valid data into tile.
   Sets tile status to non-zero if zero padding is needed. */
static inline void
transfer_inp_tile(uint8_t *inputPtr, xi_pTile3D inp, const pool_params_t *params)
{
    int D = XI_TILE3D_GET_DIM1_COORD(inp);
    int boundX = XI_TILE3D_GET_DIM2_COORD(inp) - XI_TILE3D_GET_DIM2_EDGE1(inp);
    int boundY = XI_TILE3D_GET_DIM3_COORD(inp) - XI_TILE3D_GET_DIM3_EDGE1(inp);
    int boundW = XI_TILE3D_GET_DIM2_COORD(inp) + XI_TILE3D_GET_DIM2(inp) + XI_TILE3D_GET_DIM2_EDGE2(inp);
    int boundH = XI_TILE3D_GET_DIM3_COORD(inp) + XI_TILE3D_GET_DIM3(inp) + XI_TILE3D_GET_DIM3_EDGE2(inp);

    int validX = max(0, boundX);
    int validY = max(0, boundY);
    int validW = min(boundW, (int)params->input.W) - validX;
    int validH = min(boundH, (int)params->input.H) - validY;

    int pad_left = validX - boundX;
    int pad_top = validY - boundY;
    int pad_right = boundW - validX - validW;
    int pad_bottom = boundH - validY - validH;

    // Indicate that tile needs edge extension function called
    XI_TILE3D_SET_STATUS_FLAGS(inp, (pad_left | pad_top | pad_right | pad_bottom) != 0);

    /* Don't issue DMA request if tile has no overlap with valid data */
    if (validW <= 0 || validH <= 0)
        return;

    uint8_t *tilePtr = (uint8_t*)XI_TILE3D_GET_BUFF_PTR(inp);

    dma_3d_sys2loc_dyn(/* src */ inputPtr + (validY * params->input.W*params->input.D) + (validX* params->input.D) + D,
                       /* dst */ tilePtr + pad_top * XI_TILE3D_GET_DIM2_PITCH(inp) + pad_left * XI_TILE3D_GET_DIM1_PITCH(inp),
                       /* row size */  XI_TILE3D_GET_DIM1_PITCH(inp),
                       /* src stride 2d */ params->input.D,
                       /* dst stride 2d */ XI_TILE3D_GET_DIM1_PITCH(inp),
                       /* count 2d */ validW,
                       /* src stride 3d */ params->input.D*params->input.W,
                       /* dst stride 3d */ XI_TILE3D_GET_DIM2_PITCH(inp),
                       /* count 3d */ validH);
}

// Schedule output tile DMA transfer
static inline void
transfer_outp_tile(uint8_t *outputPtr, xi_pTile3D outp, const pool_params_t *params)
{
    int D = XI_TILE3D_GET_DIM1_COORD(outp);
    int X = XI_TILE3D_GET_DIM2_COORD(outp);
    int Y = XI_TILE3D_GET_DIM3_COORD(outp);

    uint8_t *tilePtr = (uint8_t*)XI_TILE3D_GET_BUFF_PTR(outp);

    dma_3d_loc2sys_dyn(/* src */ tilePtr,
                       /* dst */ outputPtr + Y * params->output.D * params->output.W + X * params->output.D + D,
                       /* row size */ XI_TILE3D_GET_DIM1(outp),
                       /* src stride 2d */ XI_TILE3D_GET_DIM1_PITCH(outp),
                       /* dst stride 2d */ params->output.D,
                       /* count 2d */ XI_TILE3D_GET_DIM2(outp),
                       /* src stride 3d */ XI_TILE3D_GET_DIM2_PITCH(outp),
                       /* dst stride 3d */ params->output.D * params->output.W,
                       /* count 3d */ XI_TILE3D_GET_DIM3(outp));
}

XI_ERR_TYPE xiAvgPoolQuantizeA3D_DWH(const xi_pTile3D inTile, xi_pTile3D outTile,
                                    const uint8_t *param, xi_size3D frame3DSize )
{
    XI_ERR_TYPE status = 0;
    //Based on the input tile type we call the appropriate average pooling operation.
    if(XI_TILE3D_GET_TYPE(inTile) == XI_TILE3D_U8){
	 status = xiAvgPoolQuantizeA_U8_DWH(inTile, outTile, (xi_cnn_avgpoolA_params *)param, frame3DSize);
    }
    else if(XI_TILE3D_GET_TYPE(inTile) == XI_TILE3D_S8){
	 status = xiAvgPoolQuantizeA_S8_DWH(inTile, outTile, (xi_cnn_avgpoolA_params *)param, frame3DSize);
    }
    return status;
}

#if 1
XI_ERR_TYPE flk_pool(const uint8_t *raw_params,
                     struct XtensaOperationArgsIn *input,
                     struct XtensaOperationArgsOut *output)
{
	const pool_params_t *params = (const pool_params_t *)raw_params;
#if DEBUG_LEVEL > 1
    print_pool_params(raw_params);
#endif
    XI_ERROR_CHECKS(){
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
        XI_RUN_TIME_CHECK(params->stride >= 1,
                          "Invalid stride", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->output.D >= 1
                          && params->output.W >= 1
                          && params->output.H >= 1
                          && params->output.D * params->output.W * params->output.H * params->batch <= output->argsSize[0],
                          "Inconsistent output dimensions", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->input.D >= 1
                          && params->input.W >= 1
                          && params->input.H >= 1
                          && params->input.D * params->input.W * params->input.H * params->batch <= input->argsSize[0],
                          "Inconsistent input dimensions", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->tile.D >= 1 && params->output.D >= params->tile.D
                          && params->tile.W >= 1 && params->output.W >= params->tile.W
                          && params->tile.H >= 1 && params->output.H >= params->tile.H,
                          "Inconsistent output tile dimensions", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->kernelW >= 1 && params->kernelH >= 1,
                          "Inconsistent kernel dimensions", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->largeInd == 0 || params->largeInd == 1,
                          "Invalid bank index ", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->type == MAX_POOLING
        		          || params->type == AVG_POOLING
        		          || params->type == L2_POOLING,
                          "Invalid operation type, must be MAX/AVG pooling ", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->memInpTile > 0 && params->memOutpTile > 0,
                          "Invalid memory tile size ", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->activation == ACTIVATION_RELU_NONE
                          || params->activation == ACTIVATION_RELU_0
                          || params->activation == ACTIVATION_RELU_1
                          || params->activation == ACTIVATION_RELU_6,
                          "Invalid activation, must be RELU_NONE/0/1/6 ", XI_ERR_BADARG);
    };
    XI_ERR_TYPE (*pool_func)(const xi_pTile3D inTile,
    						 xi_pTile3D outTile,
                             const uint8_t *param,
                             const xi_size3D frame3DSize);

    if(params->type != AVG_POOLING) return XI_ERROR_STATUS();
    pool_func = NULL;
    if (params->qFlag)
    	assert(0);
    else
    	pool_func = xiAvgPoolQuantizeA3D_DWH;

    uint8_t *input_data_dwh = (uint8_t *)input->args[0];
    uint8_t *output_data_dwh = (uint8_t *)output->args[0];
    uint8_t* input01 = NULL;
    uint8_t* input02 = NULL;
    uint8_t* output03 = NULL;
    uint8_t* output04 = NULL;
    xi_tile3D tiles[4];
    xi_pTile3D tile_input0, tile_input1, temp3D;
    xi_pTile3D tile_output0, tile_output1;
    xi_size3D frame_size_tile_input0;

    // Number of tiles in each dimension
    int numTilesD = (params->output.D + params->tile.D - 1) / params->tile.D;
    int numTilesW = (params->output.W + params->tile.W - 1) / params->tile.W;
    int numTilesH = (params->output.H + params->tile.H - 1) / params->tile.H;
    // Max input tile dimensions
    int inpTileD = params->tile.D;
    int inpTileW = (params->tile.W - 1) * params->stride + 1;
    int inpTileH = (params->tile.H - 1) * params->stride + 1;
    // Double buffer if more than one tile
    uint32_t memSize0, memSize1;
    int doubleBuffer = params->batch != 1 || numTilesD != 1 || numTilesW != 1 || numTilesH != 1;
    if (params->largeInd == 0){
        memSize0 = params->memInpTile;
        memSize1 = params->memOutpTile;
    }
    else{
        memSize1 = params->memInpTile;
        memSize0 = params->memOutpTile;
    }
    // Check input + output fits in memory
    uint32_t inpTileSize = params->tile.D*((params->tile.W - 1) * params->stride + params->kernelW)*((params->tile.H - 1) * params->stride + params->kernelH);
    uint32_t outpTileSize = params->tile.D * params->tile.W * params->tile.H;
    // Reserve memory
    XI_CHECK_RESULT(arena_init_two_banks_split(/*size0:*/  memSize0, /* max_alignment0 */ ALIGNMENT, /* size1:*/  memSize1, /* max_alignment1 */ ALIGNMENT));

    // Allocate buffers
   arena_alloc((void**)(&input01),/* bank */ params->largeInd,/* size */ inpTileSize, /* alignment */ ALIGNMENT);
   if(input01 == NULL) XI_CHECK_RESULT(arena_alloc((void**)(&input01),/* bank */ (1-params->largeInd),/* size */ inpTileSize, /* alignment */ ALIGNMENT));

   arena_alloc((void**)(&output03),/* bank */ (1 - params->largeInd), /* size */ outpTileSize, /* alignment */ ALIGNMENT);
   if(output03 == NULL) XI_CHECK_RESULT(arena_alloc((void**)(&output03),/* bank */ params->largeInd, /* size */ outpTileSize, /* alignment */ ALIGNMENT));

   if (doubleBuffer){
       XI_CHECK_RESULT(arena_alloc((void**)(&input02),/* bank */ params->largeInd, /* size*/ inpTileSize, /* alignment */ ALIGNMENT));
       XI_CHECK_RESULT(arena_alloc((void**)(&output04),/* bank */ (1 - params->largeInd), /* size */ outpTileSize, /* alignment */ ALIGNMENT));
   }

    // Input tile type is decided based on the value of quantTensorSign field
    int32_t tile_type = (params->quantTensorSign.dataType==XI_S8) ? XI_TILE3D_S8 : XI_TILE3D_U8;
    // Input tile edges
    uint32_t edge1W = params->kernelW / 2;
    uint32_t edge1H = params->kernelH / 2;
    uint32_t edge2W = params->kernelW - edge1W - 1;
    uint32_t edge2H = params->kernelH - edge1H - 1;
    xv_memset((int16_t*)&tiles, 0, sizeof(tiles));
    tile_input0   = (xi_pTile3D)&tiles[0];
    tile_input1   = (xi_pTile3D)&tiles[1];
    tile_output0  = (xi_pTile3D)&tiles[2];
    tile_output1  = (xi_pTile3D)&tiles[3];
    // Input Parameters
    XI_TILE3D_SET_BUFF_SIZE(tile_input0, inpTileSize);
    XI_TILE3D_SET_BUFF_PTR(tile_input0, input01);
    XI_TILE3D_SET_DATA_PTR(tile_input0, &(input01[inpTileD*(inpTileW+edge1W+edge2W)*edge1H+ inpTileD*edge1W]));
    XI_TILE3D_SET_DATA_ORDER(tile_input0, XI_DWH);
    XI_TILE3D_SET_TYPE(tile_input0, tile_type);
    XI_TILE3D_SET_DIM1(tile_input0, inpTileD );
    XI_TILE3D_SET_DIM2(tile_input0, inpTileW );
    XI_TILE3D_SET_DIM3(tile_input0, inpTileH );
    XI_TILE3D_SET_DIM1_PITCH(tile_input0, inpTileD);
    XI_TILE3D_SET_DIM2_PITCH(tile_input0, inpTileD*(inpTileW+edge1W+edge2W));
    XI_TILE3D_SET_DIM2_EDGE1(tile_input0, edge1W);
    XI_TILE3D_SET_DIM2_EDGE2(tile_input0, edge2W);
    XI_TILE3D_SET_DIM3_EDGE1(tile_input0, edge1H);
    XI_TILE3D_SET_DIM3_EDGE2(tile_input0, edge2H);

    XI_TILE3D_SET_BUFF_SIZE(tile_input1, inpTileSize);
    XI_TILE3D_SET_BUFF_PTR(tile_input1,  input02);
    XI_TILE3D_SET_DATA_PTR(tile_input1, &(input02[inpTileD*(inpTileW+edge1W+edge2W)*edge1H+ inpTileD*edge1W]));
    XI_TILE3D_SET_DATA_ORDER(tile_input1, XI_DWH);
    XI_TILE3D_SET_TYPE(tile_input1, tile_type);
    XI_TILE3D_SET_DIM1(tile_input1, inpTileD );
    XI_TILE3D_SET_DIM2(tile_input1, inpTileW );
    XI_TILE3D_SET_DIM3(tile_input1, inpTileH );
    XI_TILE3D_SET_DIM1_PITCH(tile_input1, inpTileD);
    XI_TILE3D_SET_DIM2_PITCH(tile_input1, inpTileD*(inpTileW+edge1W+edge2W));
    XI_TILE3D_SET_DIM2_EDGE1(tile_input1, edge1W);
    XI_TILE3D_SET_DIM2_EDGE2(tile_input1, edge2W);
    XI_TILE3D_SET_DIM3_EDGE1(tile_input1, edge1H);
    XI_TILE3D_SET_DIM3_EDGE2(tile_input1, edge2H);

    // Output Parameters
    XI_TILE3D_SET_BUFF_SIZE( tile_output0, outpTileSize);
    XI_TILE3D_SET_BUFF_PTR( tile_output0, output03);
    XI_TILE3D_SET_DATA_PTR(tile_output0, &(output03[0]));
    XI_TILE3D_SET_DATA_ORDER( tile_output0, XI_DWH);
    XI_TILE3D_SET_TYPE( tile_output0, tile_type);
    XI_TILE3D_SET_DIM1_PITCH(tile_output0, params->tile.D);
    XI_TILE3D_SET_DIM2_PITCH(tile_output0, params->tile.D*params->tile.W);

    XI_TILE3D_SET_BUFF_SIZE( tile_output1, outpTileSize);
    XI_TILE3D_SET_BUFF_PTR( tile_output1, output04);
    XI_TILE3D_SET_DATA_PTR(tile_output1, &(output04[0]));
    XI_TILE3D_SET_DATA_ORDER( tile_output1, XI_DWH);
    XI_TILE3D_SET_TYPE( tile_output1, tile_type);
    XI_TILE3D_SET_DIM1_PITCH(tile_output1, params->tile.D);
    XI_TILE3D_SET_DIM2_PITCH(tile_output1, params->tile.D*params->tile.W);

    /* Split D dimension equally for each core, selecti section of */
    int32_t Dstart, Dend, Dcount;
    Dcount = (numTilesD + getTotalCores() - 1) / getTotalCores();
    Dstart = Dcount * getMyCore();
    Dend   = min(numTilesD, (Dstart + Dcount));
    if (Dstart >= Dend) {
        return XI_ERROR_STATUS();
    }
    // Load first input tile
    setup_outp_tile(Dstart, 0, 0, tile_output0, params);
    setup_inp_tile(tile_output0, tile_input0, params);
    transfer_inp_tile ( &(input_data_dwh[0]), tile_input0, params);

    frame_size_tile_input0.dim1Size = params->input.D;
    frame_size_tile_input0.dim2Size = params->input.W;
    frame_size_tile_input0.dim3Size = params->input.H;

    union {
        xi_cnn_l2pool2D_params l2pool2D_params;
        xi_cnn_pooling_params  pooling_params;
        xi_cnn_maxpoolA_params maxPoolQuant_params;
        xi_cnn_avgpoolA_params avgPoolQuant_params;
    } temp_params;
    uint8_t *pool_params = (uint8_t *)&temp_params;

  	XI_CNN_AVGPOOLA_SET_KERNELHEIGHT(&(temp_params.avgPoolQuant_params), params->kernelH);
  	XI_CNN_AVGPOOLA_SET_KERNELWIDTH(&(temp_params.avgPoolQuant_params), params->kernelW);
  	XI_CNN_AVGPOOLA_SET_STRIDE(&(temp_params.avgPoolQuant_params), params->stride);
  	XI_CNN_AVGPOOLA_SET_MIN_VAL(&(temp_params.avgPoolQuant_params), params->reluMin);
  	XI_CNN_AVGPOOLA_SET_MAX_VAL(&(temp_params.avgPoolQuant_params), params->reluMax);
  	if (edge1W != 0 )
  		XI_CNN_AVGPOOLA_SET_LEFTEDGE_FLAG(&(temp_params.avgPoolQuant_params));
  	else
  		XI_CNN_AVGPOOLA_RESET_LEFTEDGE_FLAG(&(temp_params.avgPoolQuant_params));
  	if (edge1H != 0 )
  		XI_CNN_AVGPOOLA_SET_TOPEDGE_FLAG(&(temp_params.avgPoolQuant_params));
  	else
  		XI_CNN_AVGPOOLA_RESET_TOPEDGE_FLAG(&(temp_params.maxPoolQuant_params));
      XI_CNN_AVGPOOLA_SET_ZEROPTINPUT(&(temp_params.avgPoolQuant_params), params->zeroPtInput);
  	XI_CNN_AVGPOOLA_SET_ZEROPTOUTPUT(&(temp_params.avgPoolQuant_params), params->zeroPtOutput);
      XI_CNN_AVGPOOLA_SET_MULTIPLIER_OUT(&(temp_params.avgPoolQuant_params), params->multiplierOut);
      XI_CNN_AVGPOOLA_SET_SHIFT_OUT(&(temp_params.avgPoolQuant_params), params->shiftOut);
      XI_CNN_AVGPOOLA_SET_LEFT_SHIFT(&(temp_params.avgPoolQuant_params), params->left_shift);

    // Wait for first tiles to arrive
    XI_CHECK_RESULT(dma_barrier());
    int batchSize = params->batch;
    uint32_t lastD,lastW,lastH,lastB;
    for (int B = 0; B < batchSize; B++){
        lastB = ( B == batchSize - 1);
        for (int H = 0; H < numTilesH; H++){
            lastH = (lastB)&&((H) == (numTilesH - 1));
            for (int W = 0; W < numTilesW; W++){
                lastW = (lastH)&&((W) == (numTilesW - 1));
                for (int D = Dstart; D < Dend; D++){
                    lastD = (lastW && (D == (Dend - 1)));
                    if(!(lastD)) {
                        int nextB, nextD, nextW, nextH;
                        inc_iter_to_temp(&(nextB), B, batchSize ,inc_iter_to_temp(&(nextH), H, numTilesH, inc_iter_to_temp(&(nextW), W, numTilesW, inc_iter_to_temp(&(nextD), D, numTilesD, 1))));
                        setup_outp_tile (nextD * params->tile.D, nextW * params->tile.W, nextH * params->tile.H, tile_output1, params);
                        setup_inp_tile (tile_output1, tile_input1, params );
                        transfer_inp_tile (&input_data_dwh[nextB*(params->input.D*params->input.W*params->input.H)], tile_input1, params );
                    }
                    // Extend edge on the current tile
                    if (XI_TILE3D_GET_STATUS_FLAGS(tile_input0)) {
                        XI_TILE3D_SET_STATUS_FLAGS(tile_input0, 0);
                        INST_EDGE_EXTENSION_BEGIN();
                        // TO DO : update with fast 2d edge extension with asymmetric edge extension support
						int32_t zeroPt;
                        if(tile_type == XI_TILE3D_S8){
                        	zeroPt = (params->type == L2_POOLING) ? params->zeroPtInput :
                                         (params->type == AVG_POOLING) ? params->zeroPtInput : -128;
						}
						else{
					          	zeroPt = (params->type == L2_POOLING) ? params->zeroPtInput :
			                                         (params->type == AVG_POOLING) ? params->zeroPtInput : 0;
						}
                        XI_CHECK_RESULT(xiExtendEdgesConst3D_I8(tile_input0, zeroPt, frame_size_tile_input0));
                        INST_EDGE_EXTENSION_END();
                    }
                    INST_KERNEL_BEGIN();
#if KERNEL_CYCLES
 			        int start = XT_RSR_CCOUNT();
#endif

                    XI_CHECK_RESULT((*pool_func)(tile_input0, tile_output0, pool_params, frame_size_tile_input0 ));
#if KERNEL_CYCLES
 			        int stop = XT_RSR_CCOUNT();
			        printf("AveragePool2D=%d\n",stop-start);
#endif
			        INST_KERNEL_END();
                    // wait for overlapping DMA transfer
                    XI_CHECK_RESULT(dma_barrier());
                    // Transfer output tile
                    transfer_outp_tile(&output_data_dwh[B*(params->output.D*params->output.W*params->output.H)], tile_output0, params);
                    temp3D = tile_output0; tile_output0 = tile_output1; tile_output1 = temp3D;
                    temp3D = tile_input0; tile_input0 = tile_input1; tile_input1 = temp3D;
                }
            }
        }
    }

#if IS_MULTICHANNEL_DMA
    /* wait for the last transfer */
    XI_CHECK_RESULT(dma_barrier());
    /* Don't reset arena if last dma_barrier is not executed as  DMA transfer is still ongoing */
    XI_CHECK_RESULT(arena_reset());
#endif
    return XI_ERROR_STATUS();
}
#endif
