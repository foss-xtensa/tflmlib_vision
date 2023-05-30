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
#include "flk_eltwiseOp.h"
#include "xi_core_api.h"
#include "utils.h"

uint32_t xiAddGetMemReqd_Context(uint32_t *pContextSize)
{
	if(pContextSize) {
		*pContextSize = sizeof(eltwiseOp_params_t);
		return 0;
	}
	else return 1;
}


typedef float (*kernel_model_f)(const eltwiseOp_params_t& params);

bool
eltwiseOpTileSetup(eltwiseOp_params_t* params, uint32_t bankLarge, uint32_t bankSmall, int32_t coreCount)
{
    // Check if it fits into local memory 
    uint32_t D, W, H;
    D = params->output.D;
    W = params->output.W;
    H = params->output.H;
    // Check if the input fits in local memory
    uint32_t outpTileSize = D * W * H * params->elemSize;
    uint32_t inpATileD = std::min(D, params->inputA.D);
    uint32_t inpATileW = std::min(W, params->inputA.W);
    uint32_t inpATileH = std::min(H, params->inputA.H);
    uint32_t inpBTileD = std::min(D, params->inputB.D);
    uint32_t inpBTileW = std::min(W, params->inputB.W);
    uint32_t inpBTileH = std::min(H, params->inputB.H);
    uint32_t inpATileSize = inpATileD * inpATileW * inpATileH * params->elemSize;
    uint32_t inpBTileSize = inpBTileD * inpBTileW * inpBTileH * params->elemSize;
    uint32_t memInpATileSize = align_up(inpATileSize, ALIGNMENT);
    uint32_t memInpBTileSize = align_up(inpBTileSize, ALIGNMENT);
    uint32_t memInpTileSize = memInpATileSize + memInpBTileSize;
    uint32_t outMaskFlag;
    switch (params->type) {
        //case ELEMENT_WISE_MAX:
    case ELEMENT_WISE_MAXIMUM:
    case ELEMENT_WISE_MINIMUM:
        outMaskFlag = 1;
        break;
    default:
        outMaskFlag = 0;
        break;
    }
    params->memTileSize = align_up(outpTileSize, ALIGNMENT) * (1 + outMaskFlag) + memInpBTileSize;
    if (bankSmall >= params->memTileSize
        && bankLarge >= memInpTileSize
        && params->batch == 1 /*params->batchA == 1 && params->batchB == 1*/) {
        params->outpTile.D = (D / coreCount) ? (D / coreCount) : D;
        params->outpTile.W = W;
        params->outpTile.H = H;
        return true;
    }
    uint32_t tmpH = (bankSmall - 3 * ALIGNMENT) / (params->elemSize * ((params->inputA.D * params->inputA.W) + (params->inputB.D * params->inputB.W) + (1 + outMaskFlag) * (params->output.D * params->output.W)));
    uint32_t tmpW = (bankSmall - 3 * ALIGNMENT) / (params->elemSize * (params->inputA.D + params->inputB.D + (1 + outMaskFlag) * params->output.D));
    uint32_t tmpD = (bankSmall - 3 * ALIGNMENT) / (params->elemSize * (3 + outMaskFlag));
    // Caculate fitting tile size
    H = std::max(1u, std::min(H, tmpH));
    if (H == 1) {
        W = std::max(1u, std::min(W, tmpW));
        if (W == 1)
            D = std::max(1u, std::min(D, tmpD));
    }
    inpATileD = std::min(D, params->inputA.D);
    inpATileW = std::min(W, params->inputA.W);
    inpATileH = std::min(H, params->inputA.H);
    inpBTileD = std::min(D, params->inputB.D);
    inpBTileW = std::min(W, params->inputB.W);
    inpBTileH = std::min(H, params->inputB.H);
    inpATileSize = inpATileD * inpATileW * inpATileH * params->elemSize;
    inpBTileSize = inpBTileD * inpBTileW * inpBTileH * params->elemSize;
    memInpATileSize = align_up(inpATileSize, ALIGNMENT);
    memInpBTileSize = align_up(inpBTileSize, ALIGNMENT);
    outpTileSize = D * W * H * params->elemSize;
    uint32_t memOutpTileSize = align_up(outpTileSize, ALIGNMENT);
    params->memTileSize = memInpATileSize + memInpBTileSize + memOutpTileSize * (1 + outMaskFlag);
    if (bankSmall >= params->memTileSize) {
        params->outpTile.D = (D / coreCount) ? (D / coreCount) : D;
        params->outpTile.W = W;
        params->outpTile.H = H;
        return true;
    }
    return false;
}

uint32_t xiAddSetContext(uint8_t *pContext, uint32_t contextSize, const uint32_t input1D, const uint32_t input1W, const uint32_t input1H, const uint32_t input1N,
        const uint32_t input2D, const uint32_t input2W, const uint32_t input2H, const uint32_t input2N, 
        const uint32_t outputD, const uint32_t outputW, const uint32_t outputH, const uint32_t outputN, 
        const int32_t zeroPtInput1, const int32_t zeroPtInput2, const int32_t zeroPtOutput,
        const uint32_t input1_multiplier, const uint32_t input2_multiplier, const uint32_t output_multiplier,
        const uint32_t input1_shift, const uint32_t input2_shift, const uint32_t output_shift, const uint32_t reluMin, const uint32_t reluMax)
{
  if ( !pContext || (contextSize < sizeof(eltwiseOp_params_t)) )
	return 1;
  eltwiseOp_params_t *addParams = (eltwiseOp_params_t *)pContext;
  memset(addParams, 0, sizeof(eltwiseOp_params_t));
  addParams->structSize = sizeof(eltwiseOp_params_t);
  addParams->inputA = { input1D, input1W, input1H };
  addParams->inputB = { input2D, input2W, input2H };
  addParams->output = { outputD, outputW, outputH };
  addParams->batchA = input1N;
  addParams->batchB = input2N;
  addParams->batch = outputN;
  addParams->type = ELEMENT_WISE_ADD;
  addParams->tensorType.dataType = XI_S8;
  addParams->elemSize = 1;
  addParams->zeroPtInputA = -1 * zeroPtInput1;
  addParams->zeroPtInputB = -1 * zeroPtInput2;
  addParams->zeroPtOutput = zeroPtOutput;
  addParams->multiplierInpA = input1_multiplier;
  addParams->multiplierInpB = input2_multiplier;
  addParams->multiplierOut = output_multiplier;
  addParams->shiftInpA = -1 * input1_shift;
  int64_t q_fixed = (int64_t)(round(input1_multiplier * (1ll << 31)));
  if (q_fixed == (1ll << 31))
  {
      q_fixed /= 2;
      --(addParams->shiftInpA);
  }
  addParams->shiftInpB = -1 * input2_shift;
  q_fixed = (int64_t)(round(input2_multiplier * (1ll << 31)));
  if (q_fixed == (1ll << 31))
  {
      q_fixed /= 2;
      --(addParams->shiftInpB);
  }
  addParams->shiftOut = -1 * output_shift;
  q_fixed = (int64_t)(round(output_multiplier * (1ll << 31)));
  if (q_fixed == (1ll << 31))
  {
      q_fixed /= 2;
      --(addParams->shiftOut);
  }
  addParams->minVal = reluMin;
  addParams->maxVal = reluMax;

  addParams->left_shift = 20; /*TODO : Pass left_shift*/

  int qSkipFlag = 1;
  if (((zeroPtOutput == zeroPtInput1) && (zeroPtOutput == zeroPtInput2)) &&
	  ((output_multiplier == input1_multiplier) && (output_multiplier == input2_multiplier)) &&
      ((output_shift == input1_shift) && (output_shift == input2_shift)))
  {
	  qSkipFlag = 0;
  }
  addParams->qSkipFlag = qSkipFlag;
    
  local_mem_info_t* mem_info = getMeminfoContext();
  size_t bank0Size = mem_info->bank[0].size;
  size_t bank1Size = mem_info->bank[1].size;
  addParams->bankInd = (bank0Size >= bank1Size) ? 0 : 1;
  uint32_t bankLarge = std::max(bank0Size, bank1Size);
  uint32_t bankSmall = std::min(bank0Size, bank1Size);

#if 0
  mem_inf.localMem.banksNumber = arena_num_banks();
  uint32_t free_space = 0;
  arena_bank_free_space(0, &free_space);
  mem_inf.localMem.bankSize[0] = free_space;
  free_space = 0;
  arena_bank_free_space(1, &free_space);
  mem_inf.localMem.bankSize[1] = free_space;
//  if (mem_inf.localMem.banksNumber < 2)
//  mem_inf.localMem.banksMode = 0;
//  else
//  mem_inf.localMem.banksMode = 2; // ((arena_contiguous_banks())? 1 : 2);
#endif
  addParams->outpTile.D = addParams->output.D;
  addParams->outpTile.W = addParams->output.W;
  addParams->outpTile.H = addParams->output.H;
#if !(REF_FLK_CONV2D)
  eltwiseOpTileSetup(addParams, bankLarge, bankSmall, 1);
#endif

  return 0;
}


uint32_t xiAdd(uint8_t *pContext, uint32_t contextSize, 
    int8_t * input1, uint32_t input1Size, 
    int8_t* input2, uint32_t input2Size, 
    int8_t * output, uint32_t outputSize)
{
	if ( !pContext || (contextSize < sizeof(eltwiseOp_params_t)) )
		return 1;
	eltwiseOp_params_t *addParams = (eltwiseOp_params_t *)pContext;

	XtensaOperationArgsIn inputs = {
	  2,
	  {input1, input2,},
	  {input1Size, input2Size,}
	};

	struct XtensaOperationArgsOut outputs = {
	  1,
	  {output,},
	  {outputSize, }
	};

#if FLK_CYCLES
    int start = XT_RSR_CCOUNT();
#endif
#if (REF_FLK_ADD)
    flk_eltwiseOp_ref(reinterpret_cast<const uint8_t*>(addParams), &inputs, &outputs);
#else
    XI_ERR_TYPE status = flk_eltwiseOp(reinterpret_cast<const uint8_t*>(addParams), &inputs, &outputs);
#endif //(REF_FLK_CONV2D)
#if FLK_CYCLES
    int stop = XT_RSR_CCOUNT();
    printf("Add (including iDMA) =%d\n",stop-start);
#endif
    if (status != XI_ERR_OK)
    {
        printf("FLK failed with status = %d \n", status);
        return -1;
    }
#if !IS_MULTICHANNEL_DMA
	dma_barrier();

#if KERNEL_INFO
	printf("Add:iW=%d,iH=%d,iD=%d \t kw=%d,kh=%d \t oW=%d,oH=%d,oD=%d \n", addParams->input.W, addParams->input.H, addParams->input.D, addParams->kernelW, addParams->kernelH, addParams->output.W, addParams->output.H, addParams->output.D );

#endif
#endif
	return 0;
}
