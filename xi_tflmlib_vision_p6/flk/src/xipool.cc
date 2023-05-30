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
#include "flk_pool.h"
#include "xi_core_api.h"
#include "cnnrt.h"
#include "utils.h"

uint32_t xiPoolGetMemReqd_Context(uint32_t *pContextSize)
{
	if(pContextSize) {
		*pContextSize = sizeof(pool_params_t);
		return 0;
	}
	else return 1;
}
bool poolSetup(pool_params_t* params, const size_t largeBank, const size_t smallBank, int32_t coreCount)
{
    uint32_t D, W, H;
    uint32_t inpTileD, inpTileH, inpTileW;
    D = (params->output.D / coreCount) ? (params->output.D / coreCount) : params->output.D;
    W = params->output.W;
    H = params->output.H;
    uint8_t SW = params->strideW;
    uint8_t SH = params->strideH;
    uint8_t KW = params->kernelW;
    uint8_t KH = params->kernelH;
    // Calculate input tile size
    inpTileH = (largeBank - 128) / (2 * D * ((W - 1) * SW + KW));
    inpTileW = (largeBank - 128) / (2 * D * KH);
    inpTileD = (largeBank - 128) / (2 * KH * KW * coreCount);

    uint32_t outpTileH = (inpTileH >= KH) ? ((inpTileH - KH) / SH + 1) : 1;
    uint32_t outpTileW = (inpTileW >= KW) ? ((inpTileW - KW) / SW + 1) : 1;
    uint32_t outpTileD = inpTileD;

    H = std::max(1u, std::min(H, outpTileH));
    if (H == 1) {
        W = std::max(1u, std::min(W, outpTileW));
        if (W == 1)
            D = std::max(1u, std::min(D, outpTileD));
    }
    // Check input + output fits in memory
    uint32_t inpTileSize = D * ((W - 1) * SW + KW) * ((H - 1) * SH + KH);
    uint32_t outpTileSize = D * W * H;

    params->memInpTile = 2 * align_up(inpTileSize, ALIGNMENT);
    params->memOutpTile = 2 * align_up(outpTileSize, ALIGNMENT);

    if (largeBank < params->memInpTile || smallBank < params->memOutpTile) {
        params->memInpTile = 2 * align_up(inpTileSize, ALIGNMENT);
        params->memOutpTile = 2 * align_up(outpTileSize, ALIGNMENT);
        // Calculate output tile size to fit into memory
        outpTileH = (smallBank - 128) / (2 * D * W);
        outpTileW = (smallBank - 128) / (2 * D);
        outpTileD = (smallBank - 128) / (2 * coreCount);
        H = std::max(1u, std::min(H, outpTileH));
        if (H == 1) {
            W = std::max(1u, std::min(W, outpTileW));
            if (W == 1)
                D = std::max(1u, std::min(D, outpTileD));
        }
        // Check input + output tiles fit into local memory
        inpTileSize = D * ((W - 1) * SW + KW) * ((H - 1) * SH + KH);
        outpTileSize = D * W * H;
        params->memInpTile = 2 * align_up(inpTileSize, ALIGNMENT);
        params->memOutpTile = 2 * align_up(outpTileSize, ALIGNMENT);
        if (largeBank < params->memInpTile || smallBank < params->memOutpTile) {
            params->tile.D = D;
            params->tile.W = W;
            params->tile.H = H;
            return true;
        }
        else
            return false;
    }
    else
    {
        params->tile.D = D;
        params->tile.W = W;
        params->tile.H = H;
        return true;
    }
    return false;
}

uint32_t xiPoolSetContext(uint8_t* pContext, uint32_t contextSize, 
    const uint32_t inputD, const uint32_t inputW, const uint32_t inputH, const uint32_t inputN,
    const uint32_t outputD, const uint32_t outputW, const uint32_t outputH,
    const uint32_t filterW, const uint32_t filterH,
    const uint32_t strideWidth, const uint32_t strideHeight, 
    const uint32_t pad_width, const uint32_t pad_height,
    const int32_t zeroPtInput, const int32_t zeroPtOutput,
    const uint32_t act_min,const uint32_t act_max, const uint8_t pool_type)
{
  if (!pContext || (contextSize < sizeof(pool_params_t)))
    return 1;
  pool_params_t* pPoolParams = (pool_params_t*)pContext;

  memset(pPoolParams, 0, sizeof(pool_params_t));
  pPoolParams->structSize = sizeof(pool_params_t);
  pPoolParams->input = { inputD, inputW, inputH };
  pPoolParams->output = { outputD, outputW, outputH };
  pPoolParams->batch = inputN;
  pPoolParams->kernelW = filterW;
  pPoolParams->kernelH = filterH;

  pPoolParams->strideW = strideWidth;
  pPoolParams->strideH = strideHeight;
  if (pad_width) {
      pPoolParams->offsetX = 0;
  }
  else {
      pPoolParams->offsetX = filterW / 2;
  }
  if (pad_height) {
      pPoolParams->offsetY = 0;
  }
  else {
      pPoolParams->offsetY = filterH / 2;
  }
  //pPoolParams->offsetX = filterW / 2 - pad_width; // 1;
  //pPoolParams->offsetY = filterH / 2 - pad_height; // 1;
  pPoolParams->type = pool_type;
  pPoolParams->qFlag = 0;
  pPoolParams->multiplierOut = 0;// 0x40000000;
  pPoolParams->shiftOut = 0;
  pPoolParams->left_shift = 20;// 1;
  pPoolParams->zeroPtInput = zeroPtInput;
  pPoolParams->zeroPtOutput = zeroPtOutput;
  pPoolParams->reluMin = act_min;
  pPoolParams->reluMax = act_max;
  pPoolParams->quantTensorSign.dataType = XI_S8;

  local_mem_info_t* mem_info = getMeminfoContext();
  size_t bank0Size = mem_info->bank[0].size;
  size_t bank1Size = mem_info->bank[1].size;
  pPoolParams->largeInd = (bank0Size >= bank1Size) ? 0 : 1;

  uint32_t largeBank = std::max(bank0Size, bank1Size);
  uint32_t smallBank = std::min(bank0Size, bank1Size);
  poolSetup(pPoolParams, largeBank, smallBank, 1);

  return 0;
}


uint32_t xiPool(uint8_t* pContext, uint32_t contextSize,
    int8_t* input, uint32_t inputSize,
    int8_t* output, uint32_t outputSize)
{
    if (!pContext || (contextSize < sizeof(pool_params_t)))
        return 1;
    pool_params_t* poolParams = (pool_params_t*)pContext;

    XtensaOperationArgsIn inputs = {
      1,
      {input, },
      {inputSize, }
    };

    struct XtensaOperationArgsOut outputs = {
      1,
      {output,},
      {outputSize, }
    };

#if FLK_CYCLES
    int start = XT_RSR_CCOUNT();
#endif

    XI_ERR_TYPE status = flk_pool(reinterpret_cast<const uint8_t*>(poolParams), &inputs, &outputs);

#if FLK_CYCLES
    int stop = XT_RSR_CCOUNT();
    printf("Pool (including iDMA) =%d\n", stop - start);
#endif
    if (status != XI_ERR_OK)
    {
        printf("FLK failed with status = %d \n", status);
        return -1;
    }
#if !IS_MULTICHANNEL_DMA
    dma_barrier();

#if KERNEL_INFO
    printf("Add:iW=%d,iH=%d,iD=%d \t kw=%d,kh=%d \t oW=%d,oH=%d,oD=%d \n", addParams->input.W, addParams->input.H, addParams->input.D, addParams->kernelW, addParams->kernelH, addParams->output.W, addParams->output.H, addParams->output.D);

#endif
#endif
    return 0;
}

//uint32_t xiAverageEvalQuantized(uint8_t *pContext, uint32_t contextSize, int8_t *pInput, uint32_t inputSize, int8_t *pOutput, uint32_t outputSize)
//{
//	if ( !pContext || (contextSize < sizeof(pool_params_t)) )
//		return 1;
//
//	pool_params_t *pPoolParams = (pool_params_t *)pContext;
//	uint32_t status = 0;
//
//    struct XtensaOperationArgsIn inputs = {
//		1,
//		{pInput,},
//		{inputSize, }
//	};
//	struct XtensaOperationArgsOut outputs = {
//		1,
//		{pOutput,},
//		{outputSize, }
//	};
//#if FLK_CYCLES
//  int start = XT_RSR_CCOUNT();
//#endif
//#if (REF_FLK_POOL)
//	computeRef(*pPoolParams, &inputs, &outputs);
//#else
//	flk_pool(reinterpret_cast<const uint8_t*>(pPoolParams), &inputs, &outputs);
//#endif
//#if FLK_CYCLES
//    int stop = XT_RSR_CCOUNT();
//    printf("Pooling (including iDMA) =%d\n",stop-start);
//#endif
//
//#if !IS_MULTICHANNEL_DMA
//    dma_barrier();
//#endif
//#if KERNEL_INFO
//	printf("pooling:iW=%d,iH=%d,iD=%d \t kw=%d,kh=%d \t oW=%d,oH=%d,oD=%d \n", pPoolParams->input.W, pPoolParams->input.H, pPoolParams->input.D, pPoolParams->kernelW, pPoolParams->kernelH, pPoolParams->output.W, pPoolParams->output.H, pPoolParams->output.D );
//#endif
//    return status;
//}

