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
#include <vector>
#include <stdio.h>
#include <assert.h>
#include "xi_tile3d_manager.h"
#include "flk_pool.h"
#include "flk_conv.h"
#include "xi_core_api.h"
#include "xi_cnn_api.h"
#include "xi_api_ref.h"
#include "xi_core.h"

#define min(x,y) ((x) > (y) ? (y) : (x))
#define max(x,y) ((x) > (y) ? (x) : (y))

#if ((XCHAL_VISION_TYPE >= 6)) // && defined(INCLUDE_XI_CNN))
#define S24_MIN  (-(((int32_t) 1) << 23))
#define S24_MAX  ((((int32_t) 1) << 23) - 1)
#define CLAMP(v, min, max)                      ((v) < (min) ? (min) : (v) > (max) ? (max) : (v))
#define ROUND(x, s)                             (((s) == 0) ? (x) : (((x) + ((int64_t)1 << ((s) - 1))) >> (s)))
#define ROUND64B_ASYMM(x, s)                    (((s) == 0) ? (x) : \
                                                 (((x) + ((int64_t) 1 << ((s) - 1))) >> (s)))

#define ROUND_N_CLAMP64B_ASYMM(x, s, min, max)  (((s) == 0) ? (CLAMP(x, min, max)) : \
                                                 (CLAMP(ROUND64B_ASYMM(x, s), min, max)))

#define ROUND_N_CLAMP64B_SYMM(x, s, min, max)   (((s) == 0) ? (CLAMP(x, min, max)) : \
                                                 (CLAMP(ROUND64B_SYMM(x, s), min, max)))

#define ROUND64B_SYMM(x, s)                     (((s) == 0) ? (x) :                                         \
                                                 (((x) > 0) ? (((x) + (((int64_t) 1) << (s - 1))) >> (s)) : \
                                                  -(((-x) + (((int64_t) 1) << (s - 1))) >> (s))))

#define ROUND_N_CLAMP64B        ROUND_N_CLAMP64B_ASYMM
#ifdef ENABLE_CONV_ASYMMETRIC_ROUNDING
#define MORPH_ROUND64B          ROUND64B_ASYMM
#define MORPH_ROUND_N_CLAMP64B  ROUND_N_CLAMP64B_ASYMM
#else
#define MORPH_ROUND64B          ROUND64B_SYMM
#define MORPH_ROUND_N_CLAMP64B  ROUND_N_CLAMP64B_SYMM
#endif
#endif
// Correctly-rounded-to-nearest division by a power-of-two.
// Also known as a rounding arithmetic right shift.
int32_t RoundingDivideByPOT(int32_t numerator, int exponent) {
    /* Extract sign of the dividend*/
    int32_t sign = numerator >= 0 ? 1 : -1;
    int32_t abs_numerator = abs(numerator);
    /* Compute rounding factor if any*/
    int32_t abs_result = \
        (exponent > 0) ? (int32_t)(((int64_t)abs_numerator + (((int64_t)1 << (exponent - 1)))) >> exponent) : abs_numerator;
    /* Return result */
    return sign * abs_result;
}
int32_t SaturatingRoundingDoublingHighMul(int32_t a, int32_t b)
{
    xi_bool overflow = ((a == b) && (a == INT_MIN));
    int64_t a_64 = (int64_t)a; //std::int64_t a_64(a);
    int64_t b_64 = (int64_t)b; //std::int64_t b_64(b);
    int64_t ab_64 = a_64 * b_64; //std::int64_t ab_64 = a_64 * b_64;
    //std::int32_t nudge = ab_64 >= 0 ? (1 << 30) : (1 - (1 << 30));
    int32_t nudge = ab_64 >= 0 ? (1 << 30) : (1 - (1 << 30));
    //std::int32_t ab_x2_high32 = static_cast<std::int32_t>((ab_64 + nudge) / (1ll << 31));
    int32_t ab_x2_high32 = (int32_t)((ab_64 + nudge) / (1ll << 31));
    return overflow ? INT_MAX : ab_x2_high32;
}

/* Convolution functions in library follow opposite sign convention for leftShift and rightShift */
int32_t MultiplyByQuantizedMultiplierConv(int32_t x, int32_t quantized_multiplier, int shift)
{
    int left_shift = shift > 0 ? 0 : -shift;
    int right_shift = shift > 0 ? shift : 0;
    return RoundingDivideByPOT(SaturatingRoundingDoublingHighMul(
        x * (1 << left_shift), quantized_multiplier), right_shift);
}
/**************************************************************************************/
/* Description : Full Layer Reference implementation of 3D depthwise dilated          */
/*               convolution  VQ for Android NN                                       */
/* Inputs      : Input Data Tile, Coeff Data Tile, Bias Array,scaleArray, shiftArray  */
/*               CNNA depthwise dilated convolution params structure.                 */
/* Outputs     : XI Error Code                                                        */
/* InOuts      : Output Tile                                                          */
/* Assumptions : InData supported is I8                                               */
/*               CoeffData supported is S8                                            */
/*               OutData supported is I8                                              */
/*               Input and Output are in DWH format.                                  */
/*               Coeff is in NWHD format.                                             */
/**************************************************************************************/
XI_ERR_TYPE xiDepthwiseDilatedConvolvedAVQ3D_I8S8I8_reff(const xi_pTile3D inTile,
    const xi_pTile3D coeffTile,
    const xi_pArray biasArray,
    const xi_pArray scaleArray,
    const xi_pArray shiftArray,
    xi_pTile3D outTile,
    xi_cnna_depthwiseDilatedConv_params* param,
    int32_t padWidth,
    int32_t padHeight)
{
    /* Error Checks */
    if (!xiTile3DIsValid_I8_ref(inTile))
    {
        return(XI_ERR_BADARG);
    }
    if (!xiTile3DIsValid_S8_ref(coeffTile))
    {
        return(XI_ERR_BADARG);
    }
    if (!xiTile3DIsValid_I8_ref(outTile))
    {
        return(XI_ERR_BADARG);
    }
    if (!xiArrayIsValid_S32_ref(biasArray))
    {
        return(XI_ERR_BADARG);
    }
    if (!param)
    {
        return(XI_ERR_NULLARG);
    }
    if (!((XI_TILE3D_GET_DATA_ORDER(inTile) == XI_DWH) && \
        (XI_TILE3D_GET_DATA_ORDER(outTile) == XI_DWH) && \
        (XI_TILE3D_GET_DATA_ORDER(coeffTile) == XI_DWH)))
    {
        return(XI_ERR_BADARG);
    }
    if (!(XI_TILE3D_GET_DIM1(inTile) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DEPTH_MULTIPLIER(param) == XI_TILE3D_GET_DIM1(outTile)))
    {
        return(XI_ERR_DATASIZE);
    }
    if (!(XI_TILE3D_GET_DIM1(outTile) == XI_TILE3D_GET_DIM1(coeffTile)))
    {
        return(XI_ERR_DATASIZE);
    }
    if (!(XI_ARRAY_GET_WIDTH(biasArray) >= XI_TILE3D_GET_DIM1(outTile)))
    {
        return(XI_ERR_DATASIZE);
    }
    if (!(XI_ARRAY_GET_WIDTH(scaleArray) >= XI_TILE3D_GET_DIM1(outTile)))
    {
        return(XI_ERR_DATASIZE);
    }
    if (!(XI_ARRAY_GET_WIDTH(shiftArray) >= XI_TILE3D_GET_DIM1(outTile)))
    {
        return(XI_ERR_DATASIZE);
    }
    if (!(XI_ARRAY_GET_HEIGHT(biasArray) > 0))
    {
        return(XI_ERR_DATASIZE);
    }
    if (!(XI_ARRAY_GET_HEIGHT(scaleArray) > 0))
    {
        return(XI_ERR_DATASIZE);
    }
    if (!(XI_ARRAY_GET_HEIGHT(shiftArray) > 0))
    {
        return(XI_ERR_DATASIZE);
    }
    if (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_RELU(param))
    {
        if (!(XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(param) <= XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(param)))
        {
            return(XI_ERR_BADARG);
        }
        if (xiTile3DCheckType_ref(outTile, XI_U8))
        {
            if (!(XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(param) >= 0))
            {
                return(XI_ERR_BADARG);
            }
            if (!(XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(param) <= UCHAR_MAX))
            {
                return(XI_ERR_BADARG);
            }
        }
        else
        {
            if (!(XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(param) >= SCHAR_MIN))
            {
                return(XI_ERR_BADARG);
            }
            if (!(XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(param) <= SCHAR_MAX))
            {
                return(XI_ERR_BADARG);
            }
        }
    }

    int32_t inDepth = XI_TILE3D_GET_DIM1(inTile);
    int32_t inWidth = XI_TILE3D_GET_DIM2(inTile);
    int32_t inHeight = XI_TILE3D_GET_DIM3(inTile);
    int32_t inPitch1 = XI_TILE3D_GET_DIM1_PITCH(inTile);
    int32_t inPitch2 = XI_TILE3D_GET_DIM2_PITCH(inTile);
    int32_t outWidth = XI_TILE3D_GET_DIM2(outTile);
    int32_t outHeight = XI_TILE3D_GET_DIM3(outTile);
    int32_t outPitch1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
    int32_t outPitch2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
    int32_t filterWidth = XI_TILE3D_GET_DIM2(coeffTile);
    int32_t filterHeight = XI_TILE3D_GET_DIM3(coeffTile);
    int32_t filterPitch1 = XI_TILE3D_GET_DIM1_PITCH(coeffTile);
    int32_t filterPitch2 = XI_TILE3D_GET_DIM2_PITCH(coeffTile);

    uint8_t* inDataU8 = (uint8_t*)XI_TILE3D_GET_DATA_PTR(inTile);
    int8_t* inDataS8 = (int8_t*)XI_TILE3D_GET_DATA_PTR(inTile);
    uint8_t* outDataU8 = (uint8_t*)XI_TILE3D_GET_DATA_PTR(outTile);
    int8_t* outDataS8 = (int8_t*)XI_TILE3D_GET_DATA_PTR(outTile);

    int8_t* filterData = (int8_t*)XI_TILE4D_GET_DATA_PTR(coeffTile);
    int32_t* biasData = (int32_t*)XI_ARRAY_GET_DATA_PTR(biasArray);
    int32_t* scale = (int32_t*)XI_ARRAY_GET_DATA_PTR(scaleArray);
    int8_t* outputShift = (int8_t*)XI_ARRAY_GET_DATA_PTR(shiftArray);

    int32_t strideWidth = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEX(param);
    int32_t strideHeight = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEY(param);
    int32_t dilationX = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param);
    int32_t dilationY = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param);
    int32_t depthMultiplier = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DEPTH_MULTIPLIER(param);
    int32_t inputOffset = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_INPUT_OFFSET(param);
    int32_t outputOffset = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_OUTPUT_OFFSET(param);
    int32_t reluMin = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(param);
    int32_t reluMax = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(param);
    const int8_t enableReLu = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_RELU(param);

    int32_t inputVal;
    int16_t minLim;
    int16_t maxLim;
    if (xiTile3DCheckType_ref(inTile, XI_S8))
    {
        minLim = enableReLu ? reluMin : SCHAR_MIN;
        maxLim = enableReLu ? reluMax : SCHAR_MAX;
    }
    else
    {
        minLim = enableReLu ? reluMin : 0;
        maxLim = enableReLu ? reluMax : UCHAR_MAX;
    }
    for (int32_t outY = 0; outY < outHeight; ++outY)
    {
        for (int32_t outX = 0; outX < outWidth; ++outX)
        {
            for (int32_t m = 0; m < depthMultiplier; m++)
            {
                for (int32_t ic = 0; ic < inDepth; ++ic)
                {
                    int32_t fc = ic + m * inDepth;
                    int32_t oc = m + ic * depthMultiplier;
                    int32_t inXorigin = (outX * strideWidth) - padWidth;
                    int32_t inYorigin = (outY * strideHeight) - padHeight;
                    int32_t acc = 0;
                    for (int32_t filterY = 0; filterY < filterHeight; ++filterY)
                    {
                        for (int32_t filterX = 0; filterX < filterWidth; ++filterX)
                        {
                            int32_t inX = inXorigin + dilationX * filterX;
                            int32_t inY = inYorigin + dilationY * filterY;
                            // If the location is outside the bounds of the input image,
                            // use zero as a default value.
                            if ((inX >= 0) && (inX < inWidth) && (inY >= 0) && (inY < inHeight))
                            {
                                int32_t inputIndex = ic + inX * inPitch1 + inY * inPitch2;
                                int32_t filterIndex = fc + filterX * filterPitch1 + filterY * filterPitch2;

                                if (xiTile3DCheckType_ref(inTile, XI_S8))
                                {
                                    inputVal = inDataS8[inputIndex];
                                }
                                else
                                {
                                    inputVal = inDataU8[inputIndex];
                                }
                                int32_t filterVal = filterData[filterIndex];
                                acc += (filterVal) * (inputVal + inputOffset);
                            }
                        }
                    }
                    acc += biasData[fc];
                    acc = MultiplyByQuantizedMultiplierConv(acc, scale[oc], outputShift[oc]);
                    acc += outputOffset;
                    acc = CLAMP(acc, minLim, maxLim);
                    int32_t outputIndex = oc + outX * outPitch1 + outY * outPitch2;

                    if (xiTile3DCheckType_ref(outTile, XI_S8))
                    {
                        outDataS8[outputIndex] = acc;
                    }
                    else
                    {
                        outDataU8[outputIndex] = acc;
                    }
                }
            }
        }
    }
    return(XI_ERR_OK);
}
void flk_depthwise_conv_ref(const uint8_t* raw_params,
    struct XtensaOperationArgsIn* input,
    struct XtensaOperationArgsOut* output)
{
#if XI_ERROR_LEVEL!=XI_ERROR_LEVEL_NO_ERROR
    XI_ERROR_CHECKS()
#endif
    conv_params_t* params = (conv_params_t*)raw_params;
    xi_cnna_depthwiseDilatedConv_params xiparams;
    int zeroPtInput = -params->zeroPtInput;
    int zeroPtCoeff = -params->zeroPtCoeff;

    /* Input tile edges */
    unsigned edge_left = params->kernelW / 2;
    unsigned edge_top = params->kernelH / 2;
    unsigned edge_right = params->kernelW - edge_left - 1;
    unsigned edge_bottom = params->kernelH - edge_top - 1;

    // Max input tile dimensions, assume whole output in one tile
    int inpTileW = (params->output.W - 1) * params->stride + 1;
    int inpTileH = (params->output.H - 1) * params->stride + 1;

    // Tile size in bytes for each data object
    int depthMultiplier = params->output.D / params->input.D;

    xi_tile3D inTile;
    xi_tile3D coeffTile;
    xi_array biasArray;
    xi_array outScaleArray, outShiftArray;
    xi_tile3D outTile;

    memset(&xiparams, 0, sizeof(xiparams));
    memset(&inTile, 0, sizeof(inTile));
    memset(&coeffTile, 0, sizeof(coeffTile));
    memset(&biasArray, 0, sizeof(biasArray));
    memset(&outScaleArray, 0, sizeof(outScaleArray));
    memset(&outShiftArray, 0, sizeof(outShiftArray));
    memset(&outTile, 0, sizeof(outTile));

    uint8_t *inputPtr  = (uint8_t *) input->args[0];
    uint8_t *coeffPtr  = (uint8_t *) input->args[1];
    uint8_t *biasPtr   = (uint8_t *) input->args[2];
    uint8_t* scalePtr = (uint8_t*)input->args[3];
    uint8_t* shiftPtr = (uint8_t*)input->args[4];
    uint8_t *outputPtr = (uint8_t *) output->args[0];
    uint32_t *paddedWidthPtr = (uint32_t *) input->args[5];
    uint32_t *paddedHeightPtr = (uint32_t *) input->args[6];

    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_INPUT_OFFSET(&xiparams, zeroPtInput);
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_COEFF_OFFSET(&xiparams, 0);
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_OUTPUT_OFFSET(&xiparams, params->zeroPtOutput);
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_OUTPUT_MULTIPLIER(&xiparams, 0);
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_OUTPUT_SHIFT(&xiparams, 0);
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_RELU_MIN(&xiparams, params->reluMin);
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_RELU_MAX(&xiparams, params->reluMax);
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_STRIDEX(&xiparams, params->stride);
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_STRIDEY(&xiparams, params->stride);
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_DILATIONX(&xiparams, 1);
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_DILATIONY(&xiparams, 1);
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_FLAG_LEFTEDGE(&xiparams);
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_FLAG_TOPEDGE(&xiparams);
    XI_CNNA_DEPTHWISE_DILATED_CONV_SET_DEPTH_MULTIPLIER(&xiparams, depthMultiplier);


    if (CONV_FLAG_GET_RELU(params->flags))
        XI_CNNA_CONV_SET_FLAG_RELU(&xiparams);
    if (params->kernelW % 2 == 0)
        XI_CNNA_CONV_SET_FLAG_LEFTEDGE(&xiparams);
    if (params->kernelH % 2 == 0)
        XI_CNNA_CONV_SET_FLAG_TOPEDGE(&xiparams);

    XI_TILE3D_SET_BUFF_PTR(&coeffTile, coeffPtr);
    XI_TILE3D_SET_BUFF_SIZE(&coeffTile, params->output.D * params->kernelW * params->kernelH);
    XI_TILE3D_SET_DATA_PTR(&coeffTile, coeffPtr);
    XI_TILE3D_SET_DATA_ORDER(&coeffTile, XI_DWH);
    XI_TILE3D_SET_TYPE(&coeffTile, XI_TILE3D_S8);
    XI_TILE3D_SET_DIM1_PITCH(&coeffTile, params->output.D);
    XI_TILE3D_SET_DIM2_PITCH(&coeffTile, params->output.D * params->kernelW);
    XI_TILE3D_SET_DIM1(&coeffTile, params->output.D);
    XI_TILE3D_SET_DIM2(&coeffTile, params->kernelW);
    XI_TILE3D_SET_DIM3(&coeffTile, params->kernelH);

    XI_ARRAY_SET_BUFF_PTR(&biasArray, biasPtr);
    XI_ARRAY_SET_BUFF_SIZE(&biasArray, params->output.D * sizeof(int32_t));
    XI_ARRAY_SET_DATA_PTR(&biasArray, biasPtr);
    XI_ARRAY_SET_WIDTH(&biasArray, params->output.D);
    XI_ARRAY_SET_HEIGHT(&biasArray, 1);
    XI_ARRAY_SET_TYPE(&biasArray, XI_ARRAY_S32);
    XI_ARRAY_SET_CAPACITY(&biasArray, params->output.D);

    XI_TILE3D_SET_BUFF_SIZE(&inTile, params->input.D * params->input.W * params->input.H);
    XI_TILE3D_SET_DATA_ORDER(&inTile, XI_DWH);
    XI_TILE3D_SET_TYPE(&inTile, XI_TILE3D_S8);
    XI_TILE3D_SET_DIM1_PITCH(&inTile, params->input.D);
    XI_TILE3D_SET_DIM2_PITCH(&inTile, params->input.D * params->input.W);
    XI_TILE3D_SET_DIM1(&inTile, params->input.D);
    XI_TILE3D_SET_DIM2(&inTile, params->input.W);
    XI_TILE3D_SET_DIM3(&inTile, params->input.H);

    XI_TILE3D_SET_BUFF_SIZE(&outTile, params->output.D * params->output.W * params->output.H);
    XI_TILE3D_SET_DATA_ORDER(&outTile, XI_DWH);
    XI_TILE3D_SET_TYPE(&outTile, XI_TILE3D_S8);
    XI_TILE3D_SET_DIM1_PITCH(&outTile, params->output.D);
    XI_TILE3D_SET_DIM2_PITCH(&outTile, params->output.D * params->output.W);
    XI_TILE3D_SET_DIM1(&outTile, params->output.D);
    XI_TILE3D_SET_DIM2(&outTile, params->output.W);
    XI_TILE3D_SET_DIM3(&outTile, params->output.H);

    XI_ARRAY_SET_BUFF_PTR(&outScaleArray, scalePtr);
    XI_ARRAY_SET_BUFF_SIZE(&outScaleArray, params->output.D * sizeof(int32_t));
    XI_ARRAY_SET_DATA_PTR(&outScaleArray, scalePtr);
    XI_ARRAY_SET_WIDTH(&outScaleArray, params->output.D);
    XI_ARRAY_SET_HEIGHT(&outScaleArray, 1);
    XI_ARRAY_SET_TYPE(&outScaleArray, XI_ARRAY_S32);
    XI_ARRAY_SET_CAPACITY(&outScaleArray, params->output.D);
    
    XI_ARRAY_SET_BUFF_PTR(&outShiftArray, shiftPtr);
    XI_ARRAY_SET_BUFF_SIZE(&outShiftArray, params->output.D);
    XI_ARRAY_SET_DATA_PTR(&outShiftArray, shiftPtr);
    XI_ARRAY_SET_WIDTH(&outShiftArray, params->output.D);
    XI_ARRAY_SET_HEIGHT(&outShiftArray, 1);
    XI_ARRAY_SET_TYPE(&outShiftArray, XI_ARRAY_S8);
    XI_ARRAY_SET_CAPACITY(&outShiftArray, params->output.D);

    for (unsigned int i=0; i<params->batch; i++) {
        XI_TILE3D_SET_BUFF_PTR(&inTile, &inputPtr[i * XI_TILE3D_GET_BUFF_SIZE(&inTile)]);
        XI_TILE3D_SET_DATA_PTR(&inTile, &inputPtr[i * XI_TILE3D_GET_BUFF_SIZE(&inTile)]);
        XI_TILE3D_SET_BUFF_PTR(&outTile, &outputPtr[i * XI_TILE3D_GET_BUFF_SIZE(&outTile)]);
        XI_TILE3D_SET_DATA_PTR(&outTile, &outputPtr[i * XI_TILE3D_GET_BUFF_SIZE(&outTile)]);
        
        if (CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkVQ_Depthwise
        	|| CONV_FLAG_GET_KERNEL_KIND(params->flags) == kkNone ) {
            xiDepthwiseDilatedConvolvedAVQ3D_I8S8I8_reff(&inTile,
                                                                 &coeffTile,
                                                                 &biasArray,
                                                                 &outScaleArray,
                                                                 &outShiftArray,
                                                                 &outTile,
                                                                 &xiparams,
                                                                 paddedWidthPtr[0],
                                                                 paddedHeightPtr[0]);
        }
        else {
            printf("kernel type NOT supported in depthwise conv...\n");
            exit(0);
        }
    
    
    }
	return ;
}

/*******************************************************************************/
/* Description : Reference implementation of 3D depthwise multiplier dilated   */
/*               convolution with symmetric quantization support.              */
/* Inputs      : Input Data Tile, Coeff Data Tile, Bias Array,scaleArray       */
/*               shiftArray,CNNA depthwise dilated convolution params structure*/
/* Outputs     : XI Error Code                                                 */
/* InOuts      : Output Tile                                                   */
/* Assumptions : InData supported is S8                                        */
/*               CoeffData supported is S8                                     */
/*               OutData supported is S8                                       */
/*               Input and Output are in DWH format.                           */
/*               Coeff is in DWH format.                                       */
/*******************************************************************************/
XI_ERR_TYPE xiDepthwiseMultiplierConvolvedAVQ3D_S8_DWH_ref(const xi_pTile3D inTile,
                                                           const xi_pTile3D coeffTile,
                                                           const xi_pArray biasArray,
                                                           const xi_pArray scaleArray,
                                                           const xi_pArray shiftArray,
                                                           xi_pTile3D outTile,
                                                           const xi_cnna_depthwiseDilatedConv_params *param)
{
  /* Error Checks */
  if (!xiTile3DIsValid_S8_ref(inTile))
  {
    return(XI_ERR_BADARG);
  }
  if (!xiTile3DIsValid_S8_ref(coeffTile))
  {
    return(XI_ERR_BADARG);
  }
  if (!xiTile3DIsValid_S8_ref(outTile))
  {
    return(XI_ERR_BADARG);
  }
  if (!xiArrayIsValid_S32_ref(biasArray))
  {
    return(XI_ERR_BADARG);
  }
  if (!xiArrayIsValid_S32_ref(scaleArray))
  {
    return(XI_ERR_BADARG);
  }
  if (!xiArrayIsValid_S8_ref(shiftArray))
  {
    return(XI_ERR_BADARG);
  }
  if (!param)
  {
    return(XI_ERR_NULLARG);
  }
  if (!((XI_TILE3D_GET_DATA_ORDER(inTile) == XI_DWH) && \
        (XI_TILE3D_GET_DATA_ORDER(outTile) == XI_DWH) && \
        (XI_TILE3D_GET_DATA_ORDER(coeffTile) == XI_DWH)))
  {
    return(XI_ERR_BADARG);
  }
  if (!(XI_TILE3D_GET_DIM1(inTile) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DEPTH_MULTIPLIER(param) == XI_TILE3D_GET_DIM1(outTile)))
  {
    return(XI_ERR_DATASIZE);
  }
  if (!(XI_TILE3D_GET_DIM1(outTile) == XI_TILE3D_GET_DIM1(coeffTile)))
  {
    return(XI_ERR_DATASIZE);
  }
  if (!(XI_ARRAY_GET_WIDTH(biasArray) >= XI_TILE3D_GET_DIM1(outTile)))
  {
    return(XI_ERR_DATASIZE);
  }
  if (!(XI_ARRAY_GET_HEIGHT(biasArray) > 0))
  {
    return(XI_ERR_DATASIZE);
  }

  if (!(XI_ARRAY_GET_WIDTH(scaleArray) >= XI_TILE3D_GET_DIM1(outTile)))
  {
    return(XI_ERR_DATASIZE);
  }
  if (!(XI_ARRAY_GET_HEIGHT(scaleArray) > 0))
  {
    return(XI_ERR_DATASIZE);
  }

  if (!(XI_ARRAY_GET_WIDTH(shiftArray) >= XI_TILE3D_GET_DIM1(outTile)))
  {
    return(XI_ERR_DATASIZE);
  }
  if (!(XI_ARRAY_GET_HEIGHT(shiftArray) > 0))
  {
    return(XI_ERR_DATASIZE);
  }

  //Array overlap check
  if (!(XI_TILE3D_GET_DATA_PTR(inTile) != XI_TILE3D_GET_DATA_PTR(outTile)))
  {
    return(XI_ERR_INPLACE);
  }
  if (!(XI_TILE4D_GET_DATA_PTR(coeffTile) != XI_TILE3D_GET_DATA_PTR(outTile)))
  {
    return(XI_ERR_INPLACE);
  }
  if (!((XI_TILE4D_GET_DIM2(coeffTile) <= 8) && (XI_TILE4D_GET_DIM3(coeffTile) <= 8)))
  {
    return(XI_ERR_KSIZE);
  }
  if (!((XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) <= 36) && (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) <= 36)))
  {
    return(XI_ERR_BADARG);
  }
  if (!((XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEX(param) <= 4) && (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEY(param) <= 4)))
  {
    return(XI_ERR_BADARG);
  }
  //Check for coefficient dimension and input edges.
  if (!(((XI_TILE4D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) % 2 == 0))
  {
    // Odd filter dimensions
    // edgeLeft >= dilatedKernelWidth / 2
    if (!(XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (((XI_TILE4D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2)))
    {
      return(XI_ERR_EDGE);
    }
    if (!(XI_TILE3D_GET_DIM2_EDGE2(inTile) >= (((XI_TILE4D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2)))
    {
      return(XI_ERR_EDGE);
    }
    // inWidth >= (outWidth - 1) * strideWidth + 1
    if (!(XI_TILE3D_GET_DIM2(inTile) >= (XI_TILE3D_GET_DIM2(outTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEX(param) + 1))
    {
      return(XI_ERR_BADARG);
    }
  }
  else
  {
    // Even filter dimensions
    if (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_LEFTEDGE(param))
    {
      // edgeLeft >= dilatedKernelWidth / 2
      if (!(XI_TILE3D_GET_DIM2_EDGE1(inTile) >= (((XI_TILE4D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2)))
      {
        return(XI_ERR_EDGE);
      }
      if (!(XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((((XI_TILE4D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2) - 1)))
      {
        return(XI_ERR_EDGE);
      }
    }
    else
    {
      // edgeLeft >= dilatedKernelWidth / 2 - 1
      if (!(XI_TILE3D_GET_DIM2_EDGE1(inTile) >= ((((XI_TILE4D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2) - 1)))
      {
        return(XI_ERR_EDGE);
      }
      if (!(XI_TILE3D_GET_DIM2_EDGE2(inTile) >= ((((XI_TILE4D_GET_DIM2(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param) + 1) / 2))))
      {
        return(XI_ERR_EDGE);
      }
    }
    // inWidth >= (outWidth - 1) * strideWidth + 1
    if (!(XI_TILE3D_GET_DIM2(inTile) >= (XI_TILE3D_GET_DIM2(outTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEX(param) + 1))
    {
      return(XI_ERR_BADARG);
    }
  }
  if (!(((XI_TILE4D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) % 2 == 0))
  {
    // Odd filter dimensions
    // edgeRight >= dilatedKernelHeight / 2
    if (!(XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (((XI_TILE4D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2)))
    {
      return(XI_ERR_EDGE);
    }
    if (!(XI_TILE3D_GET_DIM3_EDGE2(inTile) >= (((XI_TILE4D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2)))
    {
      return(XI_ERR_EDGE);
    }
    // inHeight >= (outHeight - 1) * strideHeight + 1
    if (!(XI_TILE3D_GET_DIM3(inTile) >= (XI_TILE3D_GET_DIM3(outTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEY(param) + 1))
    {
      return(XI_ERR_BADARG);
    }
  }
  else
  {
    // Even filter dimensions
    if (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_TOPEDGE(param))
    {
      // edgeTop >= dilatedKernelHeight / 2
      if (!(XI_TILE3D_GET_DIM3_EDGE1(inTile) >= (((XI_TILE4D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2)))
      {
        return(XI_ERR_EDGE);
      }
      if (!(XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((((XI_TILE4D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2) - 1)))
      {
        return(XI_ERR_EDGE);
      }
    }
    else
    {
      // edgeTop >= dilatedKernelHeight / 2 - 1
      if (!(XI_TILE3D_GET_DIM3_EDGE1(inTile) >= ((((XI_TILE4D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2) - 1)))
      {
        return(XI_ERR_EDGE);
      }
      if (!(XI_TILE3D_GET_DIM3_EDGE2(inTile) >= ((((XI_TILE4D_GET_DIM3(coeffTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param) + 1) / 2))))
      {
        return(XI_ERR_EDGE);
      }
    }
    // inHeight >= (outHeight - 1) * strideHeight + 1
    if (!(XI_TILE3D_GET_DIM3(inTile) >= (XI_TILE3D_GET_DIM3(outTile) - 1) * XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEY(param) + 1))
    {
      return(XI_ERR_BADARG);
    }
  }
  if (XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_RELU(param))
  {
    if (((XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(param) < XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(param))))
    {
      return(XI_ERR_BADARG);
    }
    if (!(XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(param) >= SCHAR_MIN))
    {
      return(XI_ERR_BADARG);
    }
    if (!(XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(param) <= SCHAR_MAX))
    {
      return(XI_ERR_BADARG);
    }
  }
  /* Get Tile Parameters */
  int32_t inDepth      = XI_TILE3D_GET_DIM1(inTile);
  int32_t inPitch1     = XI_TILE3D_GET_DIM1_PITCH(inTile);
  int32_t inPitch2     = XI_TILE3D_GET_DIM2_PITCH(inTile);
  int32_t outWidth     = XI_TILE3D_GET_DIM2(outTile);
  int32_t outHeight    = XI_TILE3D_GET_DIM3(outTile);
  int32_t outPitch1    = XI_TILE3D_GET_DIM1_PITCH(outTile);
  int32_t outPitch2    = XI_TILE3D_GET_DIM2_PITCH(outTile);
  int32_t filterWidth  = XI_TILE3D_GET_DIM2(coeffTile);
  int32_t filterHeight = XI_TILE3D_GET_DIM3(coeffTile);
  int32_t filterPitch1 = XI_TILE3D_GET_DIM1_PITCH(coeffTile);
  int32_t filterPitch2 = XI_TILE3D_GET_DIM2_PITCH(coeffTile);
  /* Input and Output data pointers */
  int8_t *pInput  = (int8_t *)XI_TILE3D_GET_DATA_PTR(inTile);
  int8_t *pOutput = (int8_t *)XI_TILE3D_GET_DATA_PTR(outTile);
  int8_t *pFilter = (int8_t *)XI_TILE4D_GET_DATA_PTR(coeffTile);
  int32_t *pBias  = (int32_t *)XI_ARRAY_GET_DATA_PTR(biasArray);
  int32_t *pScale = (int32_t *)XI_ARRAY_GET_DATA_PTR(scaleArray);
  int8_t *pShift  = (int8_t *)XI_ARRAY_GET_DATA_PTR(shiftArray);
  /* Read Depthwise Dilated Conv parameters*/
  int32_t strideWidth         = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEX(param);
  int32_t strideHeight        = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_STRIDEY(param);
  int32_t dilationX           = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONX(param);
  int32_t dilationY           = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DILATIONY(param);
  int32_t depthMultiplier     = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_DEPTH_MULTIPLIER(param);
  int32_t inputOffset         = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_INPUT_OFFSET(param);
  int32_t outputOffset        = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_OUTPUT_OFFSET(param);
  int32_t reluMin             = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MIN(param);
  int32_t reluMax             = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_RELU_MAX(param);
  uint8_t leftEdgeFlag        = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_LEFTEDGE(param);
  uint8_t topEdgeFlag         = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_TOPEDGE(param);
  const int8_t enableReLu     = XI_CNNA_DEPTHWISE_DILATED_CONV_GET_FLAG_RELU(param);
  const int8_t minLim         = enableReLu ? reluMin : SCHAR_MIN;
  const int8_t maxLim         = enableReLu ? reluMax : SCHAR_MAX;

  int32_t dilatedFilterWidth  = (filterWidth - 1) * dilationX + 1;
  int32_t dilatedFilterHeight = (filterHeight - 1) * dilationY + 1;

  int32_t leftEdge, topEdge;
  if ((dilatedFilterWidth % 2) != 0)
  {
    leftEdge = dilatedFilterWidth / 2;
  }
  else
  {
    leftEdge = leftEdgeFlag ? (dilatedFilterWidth / 2) : ((dilatedFilterWidth / 2) - 1);
  }

  if ((dilatedFilterHeight % 2) != 0)
  {
    topEdge = dilatedFilterHeight / 2;
  }
  else
  {
    topEdge = topEdgeFlag ? (dilatedFilterHeight / 2) : ((dilatedFilterHeight / 2) - 1);
  }
  pInput = &pInput[-(topEdge * inPitch2 + leftEdge * inPitch1)];

  for (int32_t outY = 0; outY < outHeight; ++outY)
  {
    for (int32_t outX = 0; outX < outWidth; ++outX)
    {
      for (int32_t ic = 0; ic < inDepth; ++ic)
      {
        for (int32_t m = 0; m < depthMultiplier; m++)
        {
          int32_t fc = m + ic * depthMultiplier;
          int32_t oc = m + ic * depthMultiplier;
          int32_t inXorigin = (outX * strideWidth);
          int32_t inYorigin = (outY * strideHeight);
          int32_t acc = pBias[fc];
          for (int32_t filterY = 0; filterY < filterHeight; ++filterY)
          {
            for (int32_t filterX = 0; filterX < filterWidth; ++filterX)
            {
              int32_t inX = inXorigin + dilationX * filterX;
              int32_t inY = inYorigin + dilationY * filterY;
              int32_t inputIndex = ic + inX * inPitch1 + inY * inPitch2;
              int32_t filterIndex = fc + filterX * filterPitch1 + filterY * filterPitch2;
              int32_t inputVal = pInput[inputIndex];
              int32_t filterVal = pFilter[filterIndex];
              acc += (filterVal)* (inputVal + inputOffset);
            }
          }
          /* calculate left and right shift values */
          int32_t leftShift = pShift[oc] < 0 ? -pShift[oc] : 0;
          int32_t rightShift = pShift[oc] < 0 ? 0 : pShift[oc];

          int64_t acc64 = ROUND((((int64_t)acc) * pScale[oc]), 31 - leftShift);
          acc = (int32_t)(MORPH_ROUND_N_CLAMP64B(acc64, rightShift, SHRT_MIN, SHRT_MAX));
          acc += outputOffset;
          int8_t result = CLAMP(acc, minLim, maxLim);
          int32_t outputIndex = oc + outX * outPitch1 + outY * outPitch2;
          pOutput[outputIndex] = result;
        }
      }
    }
  }
  return(XI_ERR_OK);
}
