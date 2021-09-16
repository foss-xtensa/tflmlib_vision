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
             const uint32_t reluMax);
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
uint32_t xiDepthwiseConvSetContext(uint8_t *pContext, uint32_t contextSize,
          const uint32_t inputD, const uint32_t inputW, const uint32_t inputH,
          const uint32_t outputD, const uint32_t outputW, const uint32_t outputH,
          const uint32_t filterW, const uint32_t filterH,
          const uint32_t stride_width, const uint32_t zeroPtInput,
          const uint32_t zeroPtCoeff, const uint32_t zeroPtOutput,
          const uint32_t quantizedScale, const uint32_t outputShift,
          const uint32_t reluMin, const uint32_t reluMax);
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
uint32_t xiAverageEvalQuantized(uint8_t *pContext, uint32_t contextSize,
      int8_t *pInput, uint32_t inputSize, int8_t *pOutput, uint32_t outputSize);
uint32_t xiPoolGetMemReqd_Context(uint32_t *pContextSize);

uint32_t xiAveragePoolSetContext(uint8_t* pContext, uint32_t contextSize, const uint32_t inputD, const uint32_t inputW, const uint32_t inputH, const uint32_t inputN,
  const uint32_t outputD, const uint32_t outputW, const uint32_t outputH, const uint32_t outputN, const uint32_t filterW, const uint32_t filterH,
  const uint32_t strideWidth, const uint32_t pad_height, const uint32_t pad_width, const uint32_t act_min, const uint32_t act_max);

#endif // _VISION_API_H_INCLUDED_
