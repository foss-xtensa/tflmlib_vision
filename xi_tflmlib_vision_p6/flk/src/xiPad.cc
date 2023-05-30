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
#include "flk_concat.h"
#include "xi_core_api.h"
#include "utils.h"

uint32_t xiPadGetMemReqd_Context(uint32_t *pContextSize)
{
  if(pContextSize) {
    *pContextSize = sizeof(concat_params_t);
    return 0;
  }
  else return 1;
}
uint32_t xiPadSetContext(uint8_t* pContext, uint32_t contextSize,
  const uint32_t* inDims, const int32_t* padDims, const int8_t padValue, const uint32_t numInDims)
{
  if ( !pContext || (contextSize < sizeof(concat_params_t)) )
  return 1;

  concat_params_t* concat_params = (concat_params_t*)pContext;
  concat_params->structSize = sizeof(*concat_params);
  concat_params->numInputs = 1;
  concat_params->quantTensorSign.dataType = XI_S8;
  concat_params->elemSize = 1;
  int32_t i, j;

  for (i = numInDims - 1, j = 0; i >= 0; --i, j++) {
    concat_params->output_pad_low[j] = padDims[i * 2];
    concat_params->output_pad_high[j] = padDims[i * 2 + 1];
  }

  for (; j < 4; j++) {
    concat_params->output_pad_low[j] = 0;
    concat_params->output_pad_high[j] = 0;
  }

  concat_params->padValue = padValue;


  concat_params->input[0][0] = inDims[0];
  concat_params->input[0][1] = inDims[1];
  concat_params->input[0][2] = inDims[2];
  concat_params->input[0][3] = inDims[3];

  concat_params->axis = 10;

  return 0;
}

uint32_t xiPad(uint8_t *pContext, uint32_t contextSize, int8_t * input, uint32_t inputSize, int8_t * output, uint32_t outputSize)
{
  if ( !pContext || (contextSize < sizeof(concat_params_t)) )
    return 1;
  concat_params_t* concatParams = (concat_params_t*)pContext;

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

  flk_concat(reinterpret_cast<const uint8_t*>(concatParams), &inputs, &outputs);

#if !IS_MULTICHANNEL_DMA
  dma_barrier();
#endif
  return 0;
}

