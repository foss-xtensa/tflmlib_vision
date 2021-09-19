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
#include "flk_conv.h"
#include "utils.h"

#define max(a, b)                         ((a > b) ? a : b)
#define min(a, b)                         ((a > b) ? b : a)

typedef XI_ERR_TYPE (*xiDepthwiseConvolvedA3D_f)(const xi_pTile3D inTile,
                                                 const xi_pTile3D coeffTile,
                                                 const xi_pArray biasArray,
                                                 xi_pTile3D outTile,
                                                 const xi_cnna_conv_params *param);

typedef XI_ERR_TYPE (*xiDepthwiseConvolvedAVQ3D_f)(const xi_pTile3D inTile,
                                                   const xi_pTile3D coeffTile,
                                                   const xi_pArray biasArray,
                                                   const xi_pArray outScaleArray,
                                                   const xi_pArray outShiftArray,
                                                   xi_pTile3D outTile,
                                                   const xi_cnna_conv_params *param);

/* Setup output tile based on coordinates and params. */
static inline void
setup_outp_tile(int D, int X, int Y, xi_pTile3D outp, const conv_params_t *params)
{
    /* Set output tile coordinates */
    XI_TILE3D_SET_DIM1_COORD(outp, D);
    XI_TILE3D_SET_DIM2_COORD(outp, X);
    XI_TILE3D_SET_DIM3_COORD(outp, Y);

    /* Compute output tile dimensions from intersection of output tile size with output data */
    XI_TILE3D_SET_DIM1(outp, min(D + params->tile.D, params->output.D) - D);
    XI_TILE3D_SET_DIM2(outp, min(X + params->tile.W, params->output.W) - X);
    XI_TILE3D_SET_DIM3(outp, min(Y + params->tile.H, params->output.H) - Y);

    /* Update output tile pitch according to tile size */
    XI_TILE3D_SET_DIM1_PITCH(outp, XI_TILE3D_GET_DIM1(outp));
    XI_TILE3D_SET_DIM2_PITCH(outp, XI_TILE3D_GET_DIM2(outp) * XI_TILE3D_GET_DIM1(outp));
}

/* Setup input tile coordinates/dimensions based on output tile coordinates/dimensions and params. */
static inline void
setup_inp_tile(const xi_pTile3D outp, xi_pTile3D inp, const conv_params_t *params)
{
    int depthMultiplier = params->output.D / params->input.D;
    int D = XI_TILE3D_GET_DIM1_COORD(outp);
    int X = XI_TILE3D_GET_DIM2_COORD(outp);
    int Y = XI_TILE3D_GET_DIM3_COORD(outp);

    /* Compute input tile coordinates */
    XI_TILE3D_SET_DIM1_COORD(inp, D / depthMultiplier);
    XI_TILE3D_SET_DIM2_COORD(inp, X * params->stride + params->offsetX);
    XI_TILE3D_SET_DIM3_COORD(inp, Y * params->stride + params->offsetY);

    /* Compute input tile dimensions */
    XI_TILE3D_SET_DIM1(inp, XI_TILE3D_GET_DIM1(outp) / depthMultiplier);
    XI_TILE3D_SET_DIM2(inp, (XI_TILE3D_GET_DIM2(outp) - 1) * params->stride + 1);
    XI_TILE3D_SET_DIM3(inp, (XI_TILE3D_GET_DIM3(outp) - 1) * params->stride + 1);

    /* Update input tile pitch according to tile size */
    XI_TILE3D_SET_DIM1_PITCH(inp, XI_TILE3D_GET_DIM1(inp));
    XI_TILE3D_SET_DIM2_PITCH(inp, (XI_TILE3D_GET_DIM2(inp) + XI_TILE3D_GET_DIM2_EDGE1(inp) + XI_TILE3D_GET_DIM2_EDGE2(inp)) * XI_TILE3D_GET_DIM1_PITCH(inp));

    /* Update data pointer, edge in depth is always 0 */
    XI_TILE3D_SET_DATA_PTR(inp, (uint8_t*)XI_TILE3D_GET_BUFF_PTR(inp)
                           + XI_TILE3D_GET_DIM1_PITCH(inp) * XI_TILE3D_GET_DIM2_EDGE1(inp)
                           + XI_TILE3D_GET_DIM2_PITCH(inp) * XI_TILE3D_GET_DIM3_EDGE1(inp));
}


/* Setup coefficients tile coordinates/dimensions based on output tile coordinates/dimensions and params.
   Returns tile size in bytes. */
static inline unsigned
setup_coeff_tile(const xi_pTile3D outp, xi_pTile3D coeff, const conv_params_t *params)
{
    unsigned int D = XI_TILE3D_GET_DIM1(outp);

    XI_TILE3D_SET_DIM1_PITCH(coeff, align_up(D, ALIGNMENT));
    XI_TILE3D_SET_DIM2_PITCH(coeff, XI_TILE3D_GET_DIM1_PITCH(coeff) * params->kernelW);
    XI_TILE3D_SET_DIM1(coeff, D);
    XI_TILE3D_SET_DIM1_COORD(coeff, XI_TILE3D_GET_DIM1_COORD(outp));

    return XI_TILE3D_GET_DIM2_PITCH(coeff) * params->kernelH;
}


/* For input tile at (X, Y) of (W, H) with edges find intersection with input data and amount of padding needed.
   Schedules DMA transfer to bring valid data into tile.
   Sets tile status to non-zero if zero padding is needed. */
static inline void
transfer_input_tile(uint8_t *inputPtr, xi_pTile3D inp, const conv_params_t *params)
{
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

    /* Indicate that tile needs edge extension function called */
    XI_TILE3D_SET_STATUS_FLAGS(inp, (pad_left | pad_top | pad_right | pad_bottom) != 0);

    uint8_t *tilePtr = (uint8_t*)XI_TILE3D_GET_BUFF_PTR(inp);

    dma_3d_sys2loc_dyn_straddles(/* src */ inputPtr + (validY * params->input.W + validX) * params->input.D + XI_TILE3D_GET_DIM1_COORD(inp),
                                 /* dst */ tilePtr + pad_top * XI_TILE3D_GET_DIM2_PITCH(inp) + pad_left * XI_TILE3D_GET_DIM1_PITCH(inp),
                                 /* row size */ XI_TILE3D_GET_DIM1(inp),
                                 /* src stride 2d */ params->input.D,
                                 /* dst stride 2d */ XI_TILE3D_GET_DIM1_PITCH(inp),
                                 /* count 2d */ validW,
                                 /* src stride 3d */ params->input.D * params->input.W,
                                 /* dst stride 3d */ XI_TILE3D_GET_DIM2_PITCH(inp),
                                 /* count 3d */ validH);
}

/* Schedule output tile DMA transfer */
static inline void
transfer_output_tile(uint8_t *outputPtr, xi_pTile3D outp, const conv_params_t *params)
{
    int D = XI_TILE3D_GET_DIM1_COORD(outp);
    int X = XI_TILE3D_GET_DIM2_COORD(outp);
    int Y = XI_TILE3D_GET_DIM3_COORD(outp);

    uint8_t *tilePtr = (uint8_t*)XI_TILE3D_GET_BUFF_PTR(outp);

    dma_3d_loc2sys_dyn_straddles(/* src */ tilePtr,
                                 /* dst */ outputPtr + (Y * params->output.W + X) * params->output.D + D,
                                 /* row size */ XI_TILE3D_GET_DIM1(outp),
                                 /* src stride 2d */ XI_TILE3D_GET_DIM1_PITCH(outp),
                                 /* dst stride 2d */ params->output.D,
                                 /* count 2d */ XI_TILE3D_GET_DIM2(outp),
                                 /* src stride 3d */ XI_TILE3D_GET_DIM2_PITCH(outp),
                                 /* dst stride 3d */ params->output.D * params->output.W,
                                 /* count 3d */ XI_TILE3D_GET_DIM3(outp));
}

XI_ERR_TYPE flk_depthwise_conv(const uint8_t *raw_params,
                               struct XtensaOperationArgsIn *input,
                               struct XtensaOperationArgsOut *output)
{
    const conv_params_t *params = (const conv_params_t *)raw_params;
    int depthMultiplier = params->output.D / params->input.D;

    XI_ERROR_CHECKS(){
        XI_RUN_TIME_CHECK(params != NULL,
                          "Params can be NULL", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->structSize == sizeof(*params),
                          "Params structure size is incorrect", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(input != NULL
                      && (input->numArgs == 3 || input->numArgs == 5)
                          && input->args[0] != NULL && input->argsSize[0] > 0
                          && input->args[1] != NULL && input->argsSize[1] > 0
                          && input->args[2] != NULL && input->argsSize[2] > 0,
                          "Invalid input args", XI_ERR_BADARG);
	    if (input->numArgs == 5)
	    {
	      XI_RUN_TIME_CHECK(input->args[3] != NULL && input->argsSize[3] > 0
	                        && input->args[4] != NULL && input->argsSize[4] > 0,
	                        "Invalid input args", XI_ERR_BADARG);
	    }
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
        XI_RUN_TIME_CHECK((params->input.D * depthMultiplier) == params->output.D,
                          "Input and output depth must be the same", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->kernelW >= 1 && params->kernelH >= 1,
                          "Inconsistent kernel dimensions", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->kernelW * params->kernelH * params->output.D <= input->argsSize[1],
                          "Inconsistent coefficients array size", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->output.D * sizeof(int32_t) <= input->argsSize[2],
                          "Inconsistent bias array size", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(params->offsetX >= 0
                          && (params->offsetX + (params->output.W - 1) * params->stride) < params->input.W
                          && params->offsetY >= 0
                          && (params->offsetY + (params->output.H - 1) * params->stride) < params->input.H,
                          "Input dimensions are not consistent with output dimensions", XI_ERR_BADARG);
    };

    conv_mem_info_t mem_info;

    FillMemInfoDepthwise(params, &mem_info);

    uint8_t* buffBias = NULL;
    uint8_t* buffCoeffA = NULL;
    uint8_t* buffCoeffB = NULL;
    uint8_t* buffInputA = NULL;
    uint8_t* buffInputB = NULL;
    uint8_t* buffOutputA = NULL;
    uint8_t* buffOutputB = NULL;
	uint8_t* buffOutScale = NULL;
	uint8_t* buffOutShift = NULL;

    struct {
        xi_tile3D tiles3D[6];
        xi_array  arr1DBias;
        xi_cnna_conv_params xiparams;
        xi_cnna_depthwiseDilatedConv_params xiparams_dm;
    } structs;

    xi_pTile3D tile3DInpA, tile3DInpB, temp3D;
    xi_pTile3D tile3DOutpA, tile3DOutpB;
    xi_pTile3D tile3DCoeffA, tile3DCoeffB;

  xi_array arr1DOutScale, arr1DOutShift;

    xi_size3D frame_size_tile3DInp;

    xiDepthwiseConvolvedA3D_f kernel = NULL;
  xiDepthwiseConvolvedAVQ3D_f kernelVQ = NULL;

    /* Double buffer if more than one tile */
    int doubleBuffCoeff = mem_info.numTilesD > 1;
    int doubleBuffIO = params->batch != 1 || mem_info.numTilesW != 1 || mem_info.numTilesH != 1 || doubleBuffCoeff;

    /* Reserve memory */
    // XI_CHECK_RESULT(arena_init_two_banks_contiguous(/* size */ mem_info.totalSize, /* max_alignment */ ALIGNMENT));
    switch (params->banks_info.banksMode)
    {
    case 0: //  single bank
        XI_CHECK_RESULT(arena_init_one_bank(/* size */  mem_info.totalSize, /* max_alignment */ ALIGNMENT));
        break;
    case 1: //  contiguous banks
        XI_CHECK_RESULT(arena_init_two_banks_contiguous(/* size */ mem_info.totalSize, /* max_alignment */ ALIGNMENT));
        break;
    case 2: //  split banks
        XI_CHECK_RESULT(arena_init_two_banks_split(/* size0 */ params->banks_info.bank0total, /* max_alignment */ ALIGNMENT, /* size1 */ params->banks_info.bank1total, /* max_alignment */ ALIGNMENT));
        break;
    }
#if DEBUG_LEVEL_CONV > 1
    unsigned int dram0Total = 0, dram1Total = 0;
    XI_CHECK_RESULT(arena_bank_free_space_debug(0, &dram0Total));
    if(params->banks_info.banksMode != 0)
    {
      XI_CHECK_RESULT(arena_bank_free_space_debug(1, &dram1Total));
    }
#endif

    /* Allocate buffers */
    arena_alloc((void**)(&buffBias),/* bank */ (params->banks_info.bankAssignments & (1 << 0)? 1 : 0), /* size */ mem_info.biasTileSize, /* alignment */ ALIGNMENT);
    if (buffBias == NULL) XI_CHECK_RESULT(arena_alloc((void**)(&buffBias),/* bank */ (params->banks_info.bankAssignments & (1 << 0)? 0 : 1), /* size */ mem_info.biasTileSize, /* alignment */ ALIGNMENT));

    arena_alloc((void**)(&buffInputA),/* bank */ (params->banks_info.bankAssignments & (1 << 1)? 1 : 0), /* size */ mem_info.inpTileSize, /* alignment */ ALIGNMENT);
    if (buffInputA == NULL) XI_CHECK_RESULT(arena_alloc((void**)(&buffInputA),/* bank */ (params->banks_info.bankAssignments & (1 << 1)? 0 : 1), /* size */ mem_info.inpTileSize, /* alignment */ ALIGNMENT));
    if (doubleBuffIO)
        XI_CHECK_RESULT(arena_alloc((void**)(&buffInputB),/* bank */ (params->banks_info.bankAssignments & (1 << 2)? 1 : 0), /* size */ mem_info.inpTileSize, /* alignment */ ALIGNMENT));

    arena_alloc((void**)(&buffOutputA),/* bank */ (params->banks_info.bankAssignments & (1 << 3)? 1 : 0), /* size */mem_info.outpTileSize, /* alignment */ ALIGNMENT);
    if (buffOutputA == NULL) XI_CHECK_RESULT(arena_alloc((void**)(&buffOutputA),/* bank */ (params->banks_info.bankAssignments & (1 << 3)? 0 : 1), /* size */mem_info.outpTileSize, /* alignment */ ALIGNMENT));
    if (doubleBuffIO)
        XI_CHECK_RESULT(arena_alloc((void**)(&buffOutputB),/* bank */ (params->banks_info.bankAssignments & (1 << 4)? 1 : 0), /* size */ mem_info.outpTileSize, /* alignment */ ALIGNMENT));

    arena_alloc((void**)(&buffCoeffA),/* bank */ (params->banks_info.bankAssignments & (1 << 5)? 1 : 0), /* size */ mem_info.coeffTileSize, /* alignment */ ALIGNMENT);
    if (buffCoeffA == NULL) XI_CHECK_RESULT(arena_alloc((void**)(&buffCoeffA),/* bank */ (params->banks_info.bankAssignments & (1 << 5)? 0 : 1), /* size */ mem_info.coeffTileSize, /* alignment */ ALIGNMENT));
    if (doubleBuffCoeff)
        XI_CHECK_RESULT(arena_alloc((void**)(&buffCoeffB),/* bank */ (params->banks_info.bankAssignments & (1 << 6)? 1 : 0), /* size */ mem_info.coeffTileSize, /* alignment */ ALIGNMENT));
	if (mem_info.outScaleSize)
	{
		arena_alloc((void * *) (&buffOutScale), /* bank */ (params->banks_info.bankAssignments & (1 << 7) ? 1 : 0), /* size */ mem_info.outScaleSize, /* alignment */ ALIGNMENT);
		if (buffOutScale == NULL) XI_CHECK_RESULT(arena_alloc((void * *) (&buffOutScale), /* bank */ (params->banks_info.bankAssignments & (1 << 7) ? 1 : 0), /* size */ mem_info.outScaleSize, /* alignment */ ALIGNMENT));
	}
	if (mem_info.outShiftSize)
	{
		arena_alloc((void * *) (&buffOutShift), /* bank */ (params->banks_info.bankAssignments & (1 << 8) ? 1 : 0), /* size */ mem_info.outShiftSize, /* alignment */ ALIGNMENT);
		if (buffOutShift == NULL) XI_CHECK_RESULT(arena_alloc((void * *) (&buffOutShift), /* bank */ (params->banks_info.bankAssignments & (1 << 8) ? 1 : 0), /* size */ mem_info.outShiftSize, /* alignment */ ALIGNMENT));
	}

#if DEBUG_LEVEL_CONV > 1
    void print_conv_params(const char * name, const void *raw_params);
    void print_conv_mem_info(const void *mem_info);

    print_conv_params("DEPTHWISE_CONV_2D", raw_params);
    print_conv_mem_info(&mem_info);
    printf("#   doubleBuff: I: %d, O: %d, C: %d\n", doubleBuffIO, doubleBuffIO, doubleBuffCoeff);
    unsigned int dram0Free = 0, dram1Free = 0;
    XI_CHECK_RESULT(arena_bank_free_space_debug(0, &dram0Free));
    printf("#   dram0Free: %d\n", dram0Free);
    printf("#   dram0Total: %d\n", dram0Total);
    if(params->banks_info.banksMode != 0)
    {
      XI_CHECK_RESULT(arena_bank_free_space_debug(1, &dram1Free));
      printf("#   dram1Free: %d\n", dram1Free);
      printf("#   dram1Total: %d\n", dram1Total);
    }
    else
    {
      printf("#   dram1Free: -NA-\n");
      printf("#   dram1Total: -NA-\n");
    }
#endif

    uint8_t *inputPtr = (uint8_t*)input->args[0];
    uint8_t *coeffPtr = (uint8_t *)input->args[1];
    uint8_t *biasPtr = (uint8_t *)input->args[2];
    uint8_t *outputPtr = (uint8_t *)output->args[0];

  int32_t *outScalePtr;
  int8_t *outShiftPtr;

  if (input->numArgs == 5)
  {
    outScalePtr = (int32_t *) input->args[3];
    outShiftPtr = (int8_t *) input->args[4];
    dma_1d_sys2loc_straddles(/* src */ outScalePtr, /* dst */ buffOutScale, /* row size */ mem_info.outScaleSize);
    dma_1d_sys2loc_straddles(/* src */ outShiftPtr, /* dst */ buffOutShift, /* row size */ mem_info.outShiftSize);
  }

    dma_1d_sys2loc_straddles(/* src */ biasPtr, /* dst */ buffBias, /* row size */ mem_info.biasTileSize);

    /* Input tile edges */
    unsigned edge_left = params->kernelW / 2;
    unsigned edge_top = params->kernelH / 2;
    unsigned edge_right = params->kernelW - edge_left - 1;
    unsigned edge_bottom = params->kernelH - edge_top - 1;

    xv_memset((int16_t*)&structs, 0, sizeof(structs));

    tile3DInpA = &structs.tiles3D[0];
    tile3DInpB = &structs.tiles3D[1];
    tile3DOutpA = &structs.tiles3D[2];
    tile3DOutpB = &structs.tiles3D[3];
    tile3DCoeffA = &structs.tiles3D[4];
    tile3DCoeffB = &structs.tiles3D[5];

    /* Setup coefficients tiles */
    XI_TILE3D_SET_BUFF_SIZE(tile3DCoeffA, mem_info.coeffTileSize);
    XI_TILE3D_SET_BUFF_PTR(tile3DCoeffA, buffCoeffA);
    XI_TILE3D_SET_DATA_PTR(tile3DCoeffA, buffCoeffA);
    XI_TILE3D_SET_DATA_ORDER(tile3DCoeffA, XI_DWH);
    XI_TILE3D_SET_DIM2(tile3DCoeffA, params->kernelW);
    XI_TILE3D_SET_DIM3(tile3DCoeffA, params->kernelH);
    XI_TILE3D_SET_TYPE(tile3DCoeffA, XI_TILE3D_U8);

    XI_TILE3D_SET_BUFF_SIZE(tile3DCoeffB, mem_info.coeffTileSize);
    XI_TILE3D_SET_BUFF_PTR(tile3DCoeffB, buffCoeffB);
    XI_TILE3D_SET_DATA_PTR(tile3DCoeffB, buffCoeffB);
    XI_TILE3D_SET_DATA_ORDER(tile3DCoeffB, XI_DWH);
    XI_TILE3D_SET_DIM2(tile3DCoeffB, params->kernelW);
    XI_TILE3D_SET_DIM3(tile3DCoeffB, params->kernelH);
    XI_TILE3D_SET_TYPE(tile3DCoeffB, XI_TILE3D_U8);
    if (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkVQ_Depthwise)
    {
      XI_TILE3D_SET_TYPE(tile3DCoeffA, XI_TILE3D_S8);
      XI_TILE3D_SET_TYPE(tile3DCoeffB, XI_TILE3D_S8);
	}

    /* Setup output tiles */
    XI_TILE3D_SET_BUFF_SIZE(tile3DOutpA, mem_info.outpTileSize);
    XI_TILE3D_SET_BUFF_PTR(tile3DOutpA, buffOutputA);
    XI_TILE3D_SET_DATA_PTR(tile3DOutpA, buffOutputA);
    XI_TILE3D_SET_DATA_ORDER(tile3DOutpA, XI_DWH);
    XI_TILE3D_SET_TYPE(tile3DOutpA, XI_TILE3D_U8);

    XI_TILE3D_SET_BUFF_SIZE(tile3DOutpB, mem_info.outpTileSize);
    XI_TILE3D_SET_BUFF_PTR(tile3DOutpB, buffOutputB);
    XI_TILE3D_SET_DATA_PTR(tile3DOutpB, buffOutputB);
    XI_TILE3D_SET_DATA_ORDER(tile3DOutpB, XI_DWH);
    XI_TILE3D_SET_TYPE(tile3DOutpB, XI_TILE3D_U8);

    if (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkVQ_Depthwise)
    {
      XI_TILE3D_SET_TYPE(tile3DOutpA, XI_TILE3D_S8);
      XI_TILE3D_SET_TYPE(tile3DOutpB, XI_TILE3D_S8);
	}

    XI_TILE3D_SET_BUFF_SIZE(tile3DInpA, mem_info.inpTileSize);
    XI_TILE3D_SET_BUFF_PTR(tile3DInpA, buffInputA);
    XI_TILE3D_SET_DATA_ORDER(tile3DInpA, XI_DWH);
    XI_TILE3D_SET_TYPE(tile3DInpA, XI_TILE3D_U8);
    XI_TILE3D_SET_DIM1(tile3DInpA, params->input.D);
    XI_TILE3D_SET_DIM1_PITCH(tile3DInpA, params->input.D);
    XI_TILE3D_SET_DIM2_EDGE1(tile3DInpA, edge_left);
    XI_TILE3D_SET_DIM2_EDGE2(tile3DInpA, edge_right);
    XI_TILE3D_SET_DIM3_EDGE1(tile3DInpA, edge_top);
    XI_TILE3D_SET_DIM3_EDGE2(tile3DInpA, edge_bottom);

    XI_TILE3D_SET_BUFF_SIZE(tile3DInpB, mem_info.inpTileSize);
    XI_TILE3D_SET_BUFF_PTR(tile3DInpB, buffInputB);
    XI_TILE3D_SET_DATA_ORDER(tile3DInpB, XI_DWH);
    XI_TILE3D_SET_TYPE(tile3DInpB, XI_TILE3D_U8);
    XI_TILE3D_SET_DIM1(tile3DInpB, params->input.D);
    XI_TILE3D_SET_DIM1_PITCH(tile3DInpB, params->input.D);
    XI_TILE3D_SET_DIM2_EDGE1(tile3DInpB, edge_left);
    XI_TILE3D_SET_DIM2_EDGE2(tile3DInpB, edge_right);
    XI_TILE3D_SET_DIM3_EDGE1(tile3DInpB, edge_top);
    XI_TILE3D_SET_DIM3_EDGE2(tile3DInpB, edge_bottom);

    if (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkVQ_Depthwise)
    {
      XI_TILE3D_SET_TYPE(tile3DInpA, XI_TILE3D_S8);
      XI_TILE3D_SET_TYPE(tile3DInpB, XI_TILE3D_S8);
	}


    frame_size_tile3DInp.dim1Size = params->input.D;
    frame_size_tile3DInp.dim2Size = params->input.W;
    frame_size_tile3DInp.dim3Size = params->input.H;

	XI_ARRAY_SET_BUFF_SIZE(&arr1DOutScale, mem_info.outScaleSize);
	XI_ARRAY_SET_BUFF_PTR(&arr1DOutScale, buffOutScale);
	XI_ARRAY_SET_HEIGHT(&arr1DOutScale, 1);
	XI_ARRAY_SET_TYPE(&arr1DOutScale, XI_ARRAY_S32);

	XI_ARRAY_SET_BUFF_SIZE(&arr1DOutShift, mem_info.outShiftSize);
	XI_ARRAY_SET_BUFF_PTR(&arr1DOutShift, buffOutShift);
	XI_ARRAY_SET_HEIGHT(&arr1DOutShift, 1);
	XI_ARRAY_SET_TYPE(&arr1DOutShift, XI_ARRAY_S8);

    XI_ARRAY_SET_BUFF_SIZE(&structs.arr1DBias, mem_info.biasTileSize);
    XI_ARRAY_SET_BUFF_PTR(&structs.arr1DBias, buffBias);
    XI_ARRAY_SET_HEIGHT(&structs.arr1DBias, 1);
    XI_ARRAY_SET_TYPE(&structs.arr1DBias, XI_ARRAY_S32);

	if (depthMultiplier == 1)
    {
        XI_CNNA_CONV_SET_STRIDE(&structs.xiparams, params->stride);
        XI_CNNA_CONV_SET_DILATION(&structs.xiparams, 1);
        XI_CNNA_CONV_SET_ZEROPT_INPUT(&structs.xiparams, params->zeroPtInput);
        XI_CNNA_CONV_SET_ZEROPT_COEFF(&structs.xiparams, params->zeroPtCoeff);
        XI_CNNA_CONV_SET_ZEROPT_OUTPUT(&structs.xiparams, params->zeroPtOutput);
        XI_CNNA_CONV_SET_QUANT_SCALE(&structs.xiparams, params->quantizedScale);
        XI_CNNA_CONV_SET_OUTPUT_SHIFT(&structs.xiparams, params->outputShift);
        XI_CNNA_CONV_SET_FLAGS(&structs.xiparams, 0);
        XI_CNNA_CONV_SET_RELUMIN(&structs.xiparams, params->reluMin);
        XI_CNNA_CONV_SET_RELUMAX(&structs.xiparams, params->reluMax);
        if (CONV_FLAG_GET_RELU(params->flags))
            XI_CNNA_CONV_SET_FLAG_RELU(&structs.xiparams);
        if (params->kernelW % 2 == 0)
            XI_CNNA_CONV_SET_FLAG_LEFTEDGE(&structs.xiparams);
        if (params->kernelH % 2 == 0)
            XI_CNNA_CONV_SET_FLAG_TOPEDGE(&structs.xiparams);

        if (params->kernelW == 3 && params->kernelH == 3)
        {
            kernel = XI_KERNEL_NAME(xiDepthwiseConvolveA2D_S_3x3_U8Ca2_MOD_DWH);
            kernelVQ = XI_KERNEL_NAME(xiDepthwiseConvolveAVQ2D_S_3x3_S8Ca2_MOD_DWH);
#if DEBUG_LEVEL_CONV > 1
			if (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkDepthwise)
			{
				printf("#   Conv type: xiDepthwiseConvolveA2D_S_3x3_U8Ca2_MOD_DWH\n");
			}
			else
			{
				printf("#   Conv type: xiDepthwiseConvolveAVQ2D_S_3x3_S8Ca2_MOD_DWH\n");
			}
#endif
        }
        else {
        	assert(0); // un-supported for now
        }
    }
    else // (depthMultiplier > 1)
    {
        XI_CNNA_DEPTHWISE_DILATED_CONV_SET_STRIDEX(&structs.xiparams_dm, params->stride);
        XI_CNNA_DEPTHWISE_DILATED_CONV_SET_STRIDEY(&structs.xiparams_dm, params->stride);
        XI_CNNA_DEPTHWISE_DILATED_CONV_SET_DILATIONX(&structs.xiparams_dm, 1);
        XI_CNNA_DEPTHWISE_DILATED_CONV_SET_DILATIONY(&structs.xiparams_dm, 1);
        XI_CNNA_DEPTHWISE_DILATED_CONV_SET_DEPTH_MULTIPLIER(&structs.xiparams_dm, depthMultiplier);
        XI_CNNA_DEPTHWISE_DILATED_CONV_SET_INPUT_OFFSET(&structs.xiparams_dm, -params->zeroPtInput);
        XI_CNNA_DEPTHWISE_DILATED_CONV_SET_COEFF_OFFSET(&structs.xiparams_dm, -params->zeroPtCoeff);
        XI_CNNA_DEPTHWISE_DILATED_CONV_SET_OUTPUT_OFFSET(&structs.xiparams_dm, params->zeroPtOutput);
        XI_CNNA_DEPTHWISE_DILATED_CONV_SET_OUTPUT_MULTIPLIER(&structs.xiparams_dm, params->quantizedScale);
        XI_CNNA_DEPTHWISE_DILATED_CONV_SET_OUTPUT_SHIFT(&structs.xiparams_dm, params->outputShift);
        XI_CNNA_DEPTHWISE_DILATED_CONV_SET_RELU_MIN(&structs.xiparams_dm, params->reluMin);
        XI_CNNA_DEPTHWISE_DILATED_CONV_SET_RELU_MAX(&structs.xiparams_dm, params->reluMax);
        XI_CNNA_DEPTHWISE_DILATED_CONV_SET_FLAGS(&structs.xiparams_dm, 0);
        if (CONV_FLAG_GET_RELU(params->flags))
            XI_CNNA_DEPTHWISE_DILATED_CONV_SET_FLAG_RELU(&structs.xiparams_dm);
        if (params->kernelW % 2 == 0)
            XI_CNNA_DEPTHWISE_DILATED_CONV_SET_FLAG_LEFTEDGE(&structs.xiparams_dm);
        if (params->kernelH % 2 == 0)
            XI_CNNA_DEPTHWISE_DILATED_CONV_SET_FLAG_TOPEDGE(&structs.xiparams_dm);
    }

    /* Split D dimension equally on each core */
    /* process the segment of D assigned to this core */
    int32_t Dstart, Dend, Dcount;
    Dcount = (mem_info.numTilesD + getTotalCores() - 1) / getTotalCores();
    Dstart = Dcount * getMyCore();
    Dend   = min((int32_t)mem_info.numTilesD, (Dstart + Dcount));
    if (Dstart >= Dend)
        return XI_ERROR_STATUS();

    /* Prepare first output tile */
    setup_outp_tile(Dstart, 0, 0, tile3DOutpA, params);

    /* Load first coefficients tile */
    setup_coeff_tile(tile3DOutpA, tile3DCoeffA, params);
    dma_1d_sys2loc_straddles(/* src */ coeffPtr + Dstart * mem_info.coeffTileSize, /* dst */ buffCoeffA, /* row size */ mem_info.coeffTileSize);

    /* Load first input tile */
    setup_inp_tile(tile3DOutpA, tile3DInpA, params);
    transfer_input_tile(inputPtr, tile3DInpA, params);

    /* Wait for first tiles to arrive */
    XI_CHECK_RESULT(dma_barrier());

    unsigned int batchSize = params->batch;

    /* Prefer to reload inputs to reduce overall bandwidth */
    for (int D=Dstart; D < Dend; D++) {
        /* Request next coefficients tile */
        int lastCoeff = D == Dend - 1;
        if (!lastCoeff) {
            int nD;
            /* Find next tile coordinates, incrementing loop variables with wraparound and carry over */
            inc_iter_to_temp(&nD, D, mem_info.numTilesD, 1);

            setup_outp_tile(nD * params->tile.D, 0, 0, tile3DOutpB, params);
            unsigned tileSize = setup_coeff_tile(tile3DOutpB, tile3DCoeffB, params);

            dma_1d_sys2loc_straddles(/* src */ coeffPtr + nD * mem_info.coeffTileSize,
                                     /* dst */ XI_TILE3D_GET_BUFF_PTR(tile3DCoeffB),
                                     /* row size */ tileSize);
        }

        for (unsigned int H=0; H < mem_info.numTilesH; H++) {
            int lastH = H == mem_info.numTilesH - 1;
            for (unsigned int W=0; W < mem_info.numTilesW; W++) {
                int lastW = lastH && W == mem_info.numTilesW - 1;
                for (unsigned int B=0; B < batchSize; B++) {
                    int lastSpatial = lastW && B == batchSize - 1;
                    /* Request next input transfer if it's not the last iteration */
                    if (doubleBuffIO && (!lastSpatial || !lastCoeff)) {
                        int nD, nW, nH, nB;
                        /* Find next tile coordinates, incrementing loop variables with wraparound and carry over */
                        inc_iter_to_temp(&nD, D, mem_info.numTilesD,
                                         inc_iter_to_temp(&nH, H, mem_info.numTilesH,
                                                          inc_iter_to_temp(&nW, W, mem_info.numTilesW,
                                                                           inc_iter_to_temp(&nB, B, batchSize, 1))));

                        setup_outp_tile(nD * params->tile.D, nW * params->tile.W, nH * params->tile.H, tile3DOutpB, params);
                        setup_inp_tile(tile3DOutpB, tile3DInpB, params);

                        transfer_input_tile(&inputPtr[nB * params->input.D * params->input.W * params->input.H],
                                            tile3DInpB,
                                            params);
                    }

                    /* Extend edge on the current tile */
                    if (XI_TILE3D_GET_STATUS_FLAGS(tile3DInpA)) {
                        INST_EDGE_EXTENSION_BEGIN();
                        XI_TILE3D_SET_STATUS_FLAGS(tile3DInpA, 0);
                        XI_CHECK_RESULT(_proto_xiExtendEdgesConstWH3D_DWH_IX(tile3DInpA, params->zeroPtInput, frame_size_tile3DInp));
                        INST_EDGE_EXTENSION_END();
                    }

                    XI_ARRAY_SET_DATA_PTR(&structs.arr1DBias, (int32_t*)XI_ARRAY_GET_BUFF_PTR(&structs.arr1DBias) + D * params->tile.D);
                    XI_ARRAY_SET_WIDTH(&structs.arr1DBias, XI_TILE3D_GET_DIM1(tile3DOutpA));
                    XI_ARRAY_SET_CAPACITY(&structs.arr1DBias, XI_TILE3D_GET_DIM1(tile3DOutpA));

					XI_ARRAY_SET_DATA_PTR(&arr1DOutScale, (int32_t *) XI_ARRAY_GET_BUFF_PTR(&arr1DOutScale) + D * params->tile.D);
					XI_ARRAY_SET_WIDTH(&arr1DOutScale, XI_TILE3D_GET_DIM1(tile3DOutpA));
					XI_ARRAY_SET_CAPACITY(&arr1DOutScale, XI_TILE3D_GET_DIM1(tile3DOutpA));

					XI_ARRAY_SET_DATA_PTR(&arr1DOutShift, (int32_t *) XI_ARRAY_GET_BUFF_PTR(&arr1DOutShift) + D * params->tile.D);
					XI_ARRAY_SET_WIDTH(&arr1DOutShift, XI_TILE3D_GET_DIM1(tile3DOutpA));
					XI_ARRAY_SET_CAPACITY(&arr1DOutShift, XI_TILE3D_GET_DIM1(tile3DOutpA));
                    /* Call XI kernel */
#if KERNEL_CYCLES
					int start = XT_RSR_CCOUNT();
#endif
					if (depthMultiplier == 1)
                    {
						if (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkVQ_Depthwise)
						{
						  INST_KERNEL_BEGIN();
						  XI_CHECK_RESULT(kernelVQ(tile3DInpA, tile3DCoeffA, &structs.arr1DBias, &arr1DOutScale, &arr1DOutShift, tile3DOutpA, &structs.xiparams));
						  INST_KERNEL_END();
						}
						else
						{
						  INST_KERNEL_BEGIN();
						  XI_CHECK_RESULT(kernel(tile3DInpA, tile3DCoeffA, &structs.arr1DBias, tile3DOutpA, &structs.xiparams));
						  INST_KERNEL_END();
						}
					}
					else // (depthMultiplier > 1)
					{
						if (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkVQ_Depthwise)
						{
						  INST_KERNEL_BEGIN();
#ifdef IVP_NOT_AVAILABLE
						  XI_CHECK_RESULT(xiDepthwiseMultiplierConvolvedAVQ3D_S8_DWH_ref(tile3DInpA, tile3DCoeffA, &structs.arr1DBias, &arr1DOutScale, &arr1DOutShift, tile3DOutpA, &structs.xiparams_dm));
#else
						  XI_CHECK_RESULT(xiDepthwiseMultiplierConvolvedAVQ3D_S8_DWH(tile3DInpA, tile3DCoeffA, &structs.arr1DBias, &arr1DOutScale, &arr1DOutShift, tile3DOutpA, &structs.xiparams_dm));
#endif
						  INST_KERNEL_END();
						}
						else                                             // (depthMultiplier > 1)
						{
						  INST_KERNEL_BEGIN();
						  XI_CHECK_RESULT(xiDepthwiseMultiplierConvolvedA3D_U8_DWH(tile3DInpA, tile3DCoeffA,
						                                                           &structs.arr1DBias, tile3DOutpA, &structs.xiparams_dm));
						  INST_KERNEL_END();
						}
					}
#if KERNEL_CYCLES
					int stop = XT_RSR_CCOUNT();
					printf("DepthwiseConv2D=%d\n",stop-start);
#endif
					/* Wait for overlapping DMA transfers */
                    XI_CHECK_RESULT(dma_barrier());

                    /* Async transfer output tile */
                    transfer_output_tile(&outputPtr[B * params->output.D * params->output.W * params->output.H],
                                         tile3DOutpA,
                                         params);

                    /* Swap double buffered pointers */
                    temp3D = tile3DOutpA; tile3DOutpA = tile3DOutpB; tile3DOutpB = temp3D;
                    temp3D = tile3DInpA; tile3DInpA = tile3DInpB; tile3DInpB = temp3D;
                }
            }
        }
        temp3D = tile3DCoeffA; tile3DCoeffA = tile3DCoeffB; tile3DCoeffB = temp3D;
    }
#if IS_MULTICHANNEL_DMA
    /* Wait for the last transfer */
    XI_CHECK_RESULT(dma_barrier());
    /* Don't reset arena if last dma_barrier is not executed as  DMA transfer is still ongoing */
    XI_CHECK_RESULT(arena_reset());
#endif
    return XI_ERROR_STATUS();
}
