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
/* entry point api used by tensorflow kernels */
#ifndef _VISION_API_H_INCLUDED_
#define _VISION_API_H_INCLUDED_

int32_t InitXtensaContext(void);
// conv apis
uint32_t xiConvGetMemReqd_Context(uint32_t *pContextSize);
uint32_t xiConvSetContext(uint8_t *pContext, uint32_t contextSize,
             const uint32_t inputD, const uint32_t inputW, const uint32_t inputH,
             const uint32_t outputD, const uint32_t outputW,
             const uint32_t outputH, const uint32_t filterW,
             const uint32_t filterH, const uint32_t stride_width,
             const uint32_t zeroPtInput, const uint32_t zeroPtCoeff,
             const uint32_t zeroPtOutput, const uint32_t quantizedScale,
             const uint32_t outputShift, const uint32_t reluMin,
             const uint32_t reluMax, uint8_t* pFilter,
             const uint32_t paddingWidth, const uint32_t paddingHeight);
uint32_t xiConvGetMemReqd_Coeff(
             uint8_t *pContext, uint32_t contextSize, uint32_t *pCoeffSize);
uint32_t xiConvDoCoeffReorder(uint8_t *pContext, uint32_t contextSize,
             uint8_t *reordCoeffnBias, uint32_t reordCoeffnBiasSize,
             uint8_t *pFilter, int32_t *pBias);
uint32_t xiConv(uint8_t *pContext, uint32_t contextSize, int8_t * input,
             uint32_t inputSize, int8_t * output, uint32_t outputSize,
             int8_t *reordCoeffnBias, uint32_t reordCoeffnBiasSize,
             int32_t *outScale, int8_t *outShift, uint32_t outShiftSize);

// depthwise conv apis
uint32_t xiDepthwiseConvGetMemReqd_Context(uint32_t *pContextSize);
uint32_t xiDepthwiseConvSetContext(uint8_t* pContext, uint32_t contextSize,
  const uint32_t inputD, const uint32_t inputW, const uint32_t inputH,
  const uint32_t outputD, const uint32_t outputW, const uint32_t outputH,
  const uint32_t filterW, const uint32_t filterH,
  const uint32_t stride_width, const uint32_t zeroPtInput,
  const uint32_t zeroPtCoeff, const uint32_t zeroPtOutput,
  const uint32_t quantizedScale, const uint32_t outputShift,
  const uint32_t reluMin, const uint32_t reluMax,
  const uint32_t paddingWidth, const uint32_t paddingHeight);

uint32_t xiDepthwiseConvGetMemReqd_Coeff(uint8_t *pContext,
          uint32_t contextSize, uint32_t *pCoeffSize);
uint32_t xiDepthwiseConvDoCoeffReorder(uint8_t *pContext, uint32_t contextSize,
          uint8_t *reordCoeffnBias, uint32_t reordCoeffnBiasSize,
          uint8_t *pFilter, int32_t *pBias);
uint32_t xiDepthwiseConv(uint8_t *pContext, uint32_t contextSize,
          int8_t * input, uint32_t inputSize, int8_t * output,
          uint32_t outputSize, int8_t *reordCoeffnBias,
          uint32_t reordCoeffnBiasSize, int32_t *outScale, int8_t *outShift, uint32_t num_channels,
          uint32_t paddingWidth, uint32_t paddingHeight);

// pooling apis
uint32_t xiPoolGetMemReqd_Context(uint32_t *pContextSize);
uint32_t xiPoolSetContext(uint8_t* pContext, uint32_t contextSize,
    const uint32_t inputD, const uint32_t inputW, const uint32_t inputH, const uint32_t inputN,
    const uint32_t outputD, const uint32_t outputW, const uint32_t outputH,
    const uint32_t filterW, const uint32_t filterH,
    const uint32_t strideWidth, const uint32_t strideHeight,
    const uint32_t pad_width, const uint32_t pad_height,
    const int32_t zeroPtInput, const int32_t zeroPtOutput,
    const uint32_t act_min, const uint32_t act_max, const uint8_t pool_type);
uint32_t xiPool(uint8_t* pContext, uint32_t contextSize,
    int8_t* input, uint32_t inputSize,
    int8_t* output, uint32_t outputSize);

//FullyConnected
uint32_t xiFullyConnectedGetMemReqd_Context(uint32_t* pContextSize);
uint32_t xiFullyConnectedGetMemReqd_Coeff(
  uint8_t* pContext, uint32_t contextSize, uint32_t* pCoeffSize);
uint32_t xiFullyConnectedDoCoeffReorder(uint8_t* pContext, uint32_t contextSize,
  uint8_t* reordCoeffnBias, uint32_t reordCoeffnBiasSize,
  uint8_t* pFilter, int32_t* pBias);
uint32_t xiFullyConnected(uint8_t* pContext, uint32_t contextSize, int8_t* input,
  uint32_t inputSize, int8_t* output, uint32_t outputSize,
  int8_t* reordCoeffnBias, uint32_t reordCoeffnBiasSize,
  int32_t* outScale, int8_t* outShift,
  uint32_t outShiftSize);

uint32_t xiFullyConnectedSetContext(uint8_t* pContext, uint32_t contextSize,
  const uint32_t *inDims,
  const uint32_t *outDims,
  const uint32_t *filterDims,
  const uint32_t stride_width,
  const uint32_t zeroPtInput, const uint32_t zeroPtCoeff,
  const uint32_t zeroPtOutput, const uint32_t quantizedScale,
  const uint32_t outputShift, const uint32_t reluMin,
  const uint32_t reluMax, uint8_t* pFilter);

//Reduce
uint32_t xiReduceGetMemReqd_Context(uint32_t* pContextSize);

uint32_t xiReduceSetContext(uint8_t* pContext, uint32_t contextSize,
  const uint32_t* inDims,
  const uint32_t* outDims,
  const uint32_t* reduceDims);

uint32_t xiReduce(uint8_t* pContext, uint32_t contextSize, int8_t* input,
  uint32_t inputSize, int8_t* output, uint32_t outputSize);

//Pad
uint32_t xiPadGetMemReqd_Context(uint32_t* pContextSize);

uint32_t xiPadSetContext(uint8_t* pContext, uint32_t contextSize,
  const uint32_t* inDims,
  const int32_t* padDims,
  const int8_t padValue,
  const uint32_t numInDims);

uint32_t xiPad(uint8_t* pContext, uint32_t contextSize, int8_t* input,
  uint32_t inputSize, int8_t* output, uint32_t outputSize);

// add apis
uint32_t xiAddGetMemReqd_Context(uint32_t* pContextSize);
uint32_t xiAddSetContext(uint8_t* pContext, uint32_t contextSize, 
    const uint32_t input1D, const uint32_t input1W, const uint32_t input1H, const uint32_t input1N,
    const uint32_t input2D, const uint32_t input2W, const uint32_t input2H, const uint32_t input2N,
    const uint32_t outputD, const uint32_t outputW, const uint32_t outputH, const uint32_t outputN,
    const int32_t zeroPtInput1, const int32_t zeroPtInput2, const int32_t zeroPtOutput,
    const uint32_t input1_multiplier, const uint32_t input2_multiplier, const uint32_t output_multiplier,
    const uint32_t input1_shift, const uint32_t input2_shift, const uint32_t output_shift, 
    const uint32_t reluMin, const uint32_t reluMax);
uint32_t xiAdd(uint8_t* pContext, uint32_t contextSize,
    int8_t* input1, uint32_t input1Size,
    int8_t* input2, uint32_t input2Size,
    int8_t* output, uint32_t outputSize);

// softmax apis
uint32_t xiSoftmaxGetMemReqd_Context(uint32_t* pContextSize);
uint32_t xiSoftmaxSetContext(uint8_t* pContext, uint32_t contextSize,
    const uint32_t input1D, const uint32_t input1W, const uint32_t input1H, const uint32_t input1N,
    const int32_t input_beta_multiplier, const int32_t input_beta_left_shift, const int32_t diff_min);
uint32_t xiSoftmax(uint8_t* pContext, uint32_t contextSize,
    int8_t* input1, uint32_t input1Size,
    int8_t* output, uint32_t outputSize);

//Reshape
uint32_t xiReshapeGetMemReqd_Context(uint32_t* pContextSize);

uint32_t xiReshapeSetContext(uint8_t* pContext, uint32_t contextSize,
  const uint32_t* inDims,
  const uint32_t numInDims);

uint32_t xiReshape(uint8_t* pContext, uint32_t contextSize, int8_t* input,
  uint32_t inputSize, int8_t* output, uint32_t outputSize);

#endif // _VISION_API_H_INCLUDED_
