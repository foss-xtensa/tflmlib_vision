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
#include <algorithm>
#include "xi_tile3d_manager.h"
#include "flk_softmax.h"
#include "xi_core_api.h"
#include "utils.h"

// Max tile size limits to 64k due to gather-scatter
#define MAX_TILE_SIZE  64000

uint32_t xiSoftmaxGetMemReqd_Context(uint32_t *pContextSize)
{
	if(pContextSize) {
		*pContextSize = sizeof(softmax_params_t);
		return 0;
	}
	else return 1;
}

bool softmaxTileSetup(softmax_params_t* params, uint32_t bankSmall, int32_t coreCount)
{
    // Check if it fits into local memory
    uint32_t inpTileD, inpTileW, inpTileH;
    uint32_t tmpH, tmpW, tmpD;
    int config = params->axis;
    uint32_t LUT_Size = 256 * sizeof(int32_t);
    uint32_t scratch_Size = 0;
    inpTileD = params->input.D;
    inpTileW = params->input.W;
    inpTileH = params->input.H;
    params->intermediateArrSize = 0;

    uint32_t bankSize = (params->input.D <= 32) ? std::min((uint32_t)2 * MAX_TILE_SIZE, (uint32_t)bankSmall) : bankSmall;
    if (config == 1)     // XI_CNN_MEAN_DIM1
    {
        if (params->input.D <= 16) {
            params->intermediateArrSize = ALIGNMENT * params->input.D;
        }
        scratch_Size = params->intermediateArrSize;
        tmpH = (bankSize - LUT_Size - 192 - scratch_Size) / (params->elemSize * params->input.D * params->input.W * 2);
        tmpW = (bankSize - LUT_Size - 192 - scratch_Size) / (params->elemSize * params->input.D * 2);
        inpTileH = std::max(1u, std::min(params->input.H, tmpH));
        if (inpTileH == 1)
        {
            inpTileW = std::max(1u, std::min(params->input.W, tmpW));
        }
    }
    else if (config == 2)     // XI_CNN_MEAN_DIM2
    {
        if (params->input.W <= 16) {
            params->intermediateArrSize = ALIGNMENT * params->input.W;
        }
        scratch_Size = params->intermediateArrSize;
        tmpH = (bankSize - LUT_Size - 192 - scratch_Size) / (params->elemSize * params->input.D * params->input.W * 2);
        tmpD = (bankSize - LUT_Size - 192 - scratch_Size) / ((params->elemSize * params->input.W * 2));
        inpTileH = std::max(1u, std::min(params->input.H, tmpH));
        if (inpTileH == 1)
        {
            inpTileD = std::max(1u, std::min(params->input.D, tmpD));
        }
        inpTileD = (inpTileD / coreCount) ? (inpTileD / coreCount) : inpTileD;
    }
    else if (config == 3)     // XI_CNN_MEAN_DIM3
    {
        tmpW = (bankSize - LUT_Size - 192) / (params->elemSize * params->input.H * params->input.D * 2);
        tmpD = (bankSize - LUT_Size - 192) / ((params->elemSize * params->input.H * 2));
        inpTileW = std::max(1u, std::min(params->input.W, tmpW));
        if (inpTileW == 1)
        {
            inpTileD = std::max(1u, std::min(params->input.D, tmpD));
        }
        inpTileD = (inpTileD / coreCount) ? (inpTileD / coreCount) : inpTileD;
    }
    params->outTile.D = inpTileD;
    params->outTile.W = inpTileW;
    params->outTile.H = inpTileH;

    uint32_t tileSize = params->outTile.D * params->outTile.W * params->outTile.H * params->elemSize;

    //uint32_t LUT_Size = 256 * sizeof(int32_t);

    params->doubleBuffer = params->batch != 1 || params->outTile.D != params->input.D || params->outTile.W != params->input.W || params->outTile.H != params->input.H;

    if (params->doubleBuffer)
        params->memTileSize = 2 * align_up(tileSize, ALIGNMENT) + scratch_Size + LUT_Size;
    else
        params->memTileSize = align_up(tileSize, ALIGNMENT) + scratch_Size + LUT_Size;

    if (bankSize < params->memTileSize)
    {
        return(false);
    }
    return(true);
}

uint32_t xiSoftmaxSetContext(uint8_t *pContext, uint32_t contextSize, 
        const uint32_t input1D, const uint32_t input1W, const uint32_t input1H, const uint32_t input1N,
        const int32_t input_beta_multiplier, const int32_t input_beta_left_shift, const int32_t diff_min)
{
  if ( !pContext || (contextSize < sizeof(softmax_params_t)) )
	return 1;
  softmax_params_t *softmaxParams = (softmax_params_t *)pContext;
  memset(softmaxParams, 0, sizeof(softmax_params_t));
  softmaxParams->structSize = sizeof(softmax_params_t);
  softmaxParams->input = { input1D, input1W, input1H };
  softmaxParams->batch = input1N;
  softmaxParams->tensorType.dataType = XI_S8;
  softmaxParams->elemSize = 1;
  softmaxParams->input_beta_multiplier = input_beta_multiplier;
  softmaxParams->input_beta_left_shift =  input_beta_left_shift;
  softmaxParams->diff_min = diff_min;
  softmaxParams->axis = 1;
      
  local_mem_info_t* mem_info = getMeminfoContext();
  size_t bank0Size = mem_info->bank[0].size;
  size_t bank1Size = mem_info->bank[1].size;
  softmaxParams->bankInd = (bank0Size >= bank1Size) ? 0 : 1;
  uint32_t bankSmall = std::min(bank0Size, bank1Size);

  softmaxParams->outTile.D = softmaxParams->input.D;
  softmaxParams->outTile.W = softmaxParams->input.W;
  softmaxParams->outTile.H = softmaxParams->input.H;
#if !(REF_FLK_CONV2D)
  softmaxTileSetup(softmaxParams, bankSmall, 1);
#endif

  return 0;
}


uint32_t xiSoftmax(uint8_t *pContext, uint32_t contextSize, 
    int8_t * input1, uint32_t input1Size, 
    int8_t * output, uint32_t outputSize)
{
	if ( !pContext || (contextSize < sizeof(softmax_params_t)) )
		return 1;
	softmax_params_t *softmaxParams = (softmax_params_t *)pContext;
	
	XtensaOperationArgsIn inputs = {
	  1,
	  {input1,},
	  {input1Size,}
	};

	struct XtensaOperationArgsOut outputs = {
	  1,
	  {output,},
	  {outputSize, }
	};

#if FLK_CYCLES
    int start = XT_RSR_CCOUNT();
#endif
#if (REF_FLK_SOFTMAX)
    flk_softmax_ref(reinterpret_cast<const uint8_t*>(softmaxParams), &inputs, &outputs);
#else
    XI_ERR_TYPE status = flk_softmax(reinterpret_cast<const uint8_t*>(softmaxParams), &inputs, &outputs);
#endif //(REF_FLK_CONV2D)
#if FLK_CYCLES
    int stop = XT_RSR_CCOUNT();
    printf("Softmax (including iDMA) =%d\n",stop-start);
#endif
    if (status != XI_ERR_OK)
    {
        printf("FLK failed with status = %d \n", status);
        return -1;
    }
#if !IS_MULTICHANNEL_DMA
	dma_barrier();

#if KERNEL_INFO
	printf("Add:iW=%d,iH=%d,iD=%d \t kw=%d,kh=%d \t oW=%d,oH=%d,oD=%d \n", softmaxParams->input.W, softmaxParams->input.H, softmaxParams->input.D, softmaxParams->kernelW, softmaxParams->kernelH, softmaxParams->output.W, softmaxParams->output.H, softmaxParams->output.D );

#endif
#endif
	return 0;
}
