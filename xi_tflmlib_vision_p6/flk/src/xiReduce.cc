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
#include "flk_reduce.h"
#include "xi_core_api.h"
#include "utils.h"

uint32_t xiReduceGetMemReqd_Context(uint32_t *pContextSize)
{
  if(pContextSize) {
    *pContextSize = sizeof(reduce_params_t);
    return 0;
  }
  else return 1;
}
uint32_t xiReduceSetContext(uint8_t* pContext, uint32_t contextSize,
  const uint32_t* inDims,  const uint32_t* outDims,  const uint32_t* reduceDims) 
{
  if ( !pContext || (contextSize < sizeof(reduce_params_t)) )
  return 1;
  reduce_params_t * reduce_params = (reduce_params_t*)pContext;
  memset(reduce_params, 0, sizeof(reduce_params_t));
  reduce_params->structSize = sizeof(reduce_params_t);
  reduce_params->dataType = S8;
  reduce_params->eleSize = 1;

  reduce_params->axis.D = (int)reduceDims[0];
  reduce_params->axis.W = (int)reduceDims[1];
  reduce_params->axis.H = (int)reduceDims[2];
  reduce_params->axis.N = (int)reduceDims[3];

  reduce_params->input.D = inDims[0];
  reduce_params->input.W = inDims[1];
  reduce_params->input.H = inDims[2];
  reduce_params->input.N = inDims[3];

  reduce_params->type = XFL_REDUCE_MAX;

  uint8_t numBanks = arena_num_banks();
  if (numBanks != 2) {
    // Reduce is implemented for 2 Banks
    return 1;
  }
  uint32_t bank0Size = 0, bank1Size = 0;
  arena_bank_free_space(0, &bank0Size);
  arena_bank_free_space(1, &bank1Size);

  reduce_params->bankInd = (bank0Size <= bank1Size) ? 0 : 1;
  uint32_t bankSmall = std::min(bank0Size, bank1Size);
  reduce4DTileSetup(reduce_params, bankSmall);

  return 0;
}
static void calculateOutTileSize(reduce_params_t* params, uint32_t D, uint32_t W, uint32_t H, uint32_t N)
{
  params->outTile.D = params->axis.D ? 1 : D;
  params->outTile.W = params->axis.W ? 1 : W;
  params->outTile.H = params->axis.H ? 1 : H;
  params->outTile.N = params->axis.N ? 1 : N;
  params->output.D = params->axis.D ? 1 : params->input.D;
  params->output.W = params->axis.W ? 1 : params->input.W;
  params->output.H = params->axis.H ? 1 : params->input.H;
  params->output.N = params->axis.N ? 1 : params->input.N;
  params->outTileSize = params->outTile.D * params->outTile.W * params->outTile.H * params->outTile.N * params->eleSize;
  if ((params->type == XFL_REDUCE_MEAN || params->type == XFL_REDUCE_PROD || params->type == XFL_REDUCE_SUM) &&
    params->dataType == FP16) {
    params->intermediateArrSize = params->outTile.D * params->outTile.W * params->outTile.H * params->outTile.N * 4;     // FP32
  }
  else {
    params->intermediateArrSize = 0;
  }
}
static uint32_t calculateTileSize(reduce_params_t* params, uint32_t D, uint32_t W, uint32_t H, uint32_t N)
{
  calculateOutTileSize(params, D, W, H, N);
  params->tile.D = D;
  params->tile.W = W;
  params->tile.H = H;
  params->tile.N = N;

  params->tileSize = D * W * H * N * params->eleSize;
  return align_up(params->tileSize, ALIGNMENT) + align_up(params->outTileSize, ALIGNMENT) + align_up(params->intermediateArrSize, ALIGNMENT);
}
bool reduce4DTileSetup(reduce_params_t* params, const size_t smallBank)
{
  // bankSmall - smaller bank size available among Bank-0 and Bank-1
  uint32_t D, W, H, N;
  uint32_t inpD, inpW, inpH, inpN;
  D = inpD = params->input.D;
  W = inpW = params->input.W;
  H = inpH = params->input.H;
  N = inpN = params->input.N;

  /* extract xiLib config from table */
  uint32_t config = (params->axis.D ? 0x1 : 0) |
    (params->axis.W ? 0x2 : 0) |
    (params->axis.H ? 0x4 : 0) |
    (params->axis.N ? 0x8 : 0);
  params->config = config;

  params->memSize = calculateTileSize(params, D, W, H, N);
  if (smallBank >= params->tileSize) {
    params->combinedTile = 1;
    params->memSize -= align_up(params->intermediateArrSize, ALIGNMENT);
    params->intermediateArrSize = 0;
    return true;
  }
  // now we have tile = IDMA tile
  calculateOutTileSize(params, D, W, H, N);
  uint32_t tmpN = (smallBank - 128) / (2 * (D) * (W) * (H)*params->eleSize);
  uint32_t tmpH = (smallBank - 128) / (2 * (D) * (W)*params->eleSize);
  uint32_t tmpW = (smallBank - 128) / (2 * (D)*params->eleSize);
  uint32_t tmpD = (smallBank - 128) / (2 * params->eleSize);
  // Caculate fitting tile size
  N = std::max(1u, std::min(N, tmpN));
  if (N == 1) {
    H = std::max(1u, std::min(H, tmpH));
    if (H == 1) {
      W = std::max(1u, std::min(W, tmpW));
      if (W == 1)
        D = std::max(1u, std::min(D, tmpD));
    }
  }
  params->memSize = calculateTileSize(params, D, W, H, N);
  int32_t difference = smallBank - params->memSize;
  if (difference > 0) {
    // catchup one reducted axis, it should fit at least (twice - 1)
    if (params->axis.D && (D != inpD) && (D != 1)) {
      D = D * 2 - 1;
      D = std::max(1u, std::min(inpD, D));
    }
    else if (params->axis.W && (W != inpW) && (W != 1)) {
      W = W * 2 - 1;
      W = std::max(1u, std::min(inpW, W));
    }
    else if (params->axis.H && (H != inpH) && (H != 1)) {
      H = H * 2 - 1;
      H = std::max(1u, std::min(inpH, H));
        }
    else if (params->axis.N && (N != inpN) && (N != 1)) {
      N = N * 2 - 1;
      N = std::max(1u, std::min(inpN, N));
    }
    }

  params->memSize = calculateTileSize(params, D, W, H, N);
  if (smallBank >= params->memSize)
    return true;

  return false;
}

uint32_t xiReduce(uint8_t *pContext, uint32_t contextSize, int8_t * input, uint32_t inputSize, int8_t * output, uint32_t outputSize)
{
  if ( !pContext || (contextSize < sizeof(reduce_params_t)) )
    return 1;
  reduce_params_t*reduceParams = (reduce_params_t*)pContext;

  XtensaOperationArgsIn inputs = {
    1,
    {input,},
    {inputSize,}
  };

  struct XtensaOperationArgsOut outputs = {
    1,
    {output,},
    {outputSize, }
  };

  flk_reduce4D(reinterpret_cast<const uint8_t*>(reduceParams), &inputs, &outputs);

#if !IS_MULTICHANNEL_DMA
  dma_barrier();
#endif
  return 0;
}

