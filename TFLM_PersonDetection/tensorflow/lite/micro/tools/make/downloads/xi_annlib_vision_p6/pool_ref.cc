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
#include <stdio.h>
#include <string.h>
#include "xi_tile3d_manager.h"
#include "flk_pool.h"
#include "xi_core_api.h"
#include "xi_cnn_api.h"
#include "xi_api_ref.h"

#define min(x,y) ((x) > (y) ? (y) : (x))
#define max(x,y) ((x) > (y) ? (x) : (y))
/********************************************************************************************/
/* Description  : Reference implementation of averageL2Norm function used in L2PoolQuantize */
/* Inputs       : sumOfSquares, count                                                       */
/* Output       : Square root of (sumOfSquares/count) Code                                  */
/* Assumptions  : Input and Output Data are unsigned 8b                                     */
/* The following routine computes sqrt(sumOfSquares/count) using a combination		        */
/* of Newton Raphson and uint32_t/uint16_t divide operation								    */
/********************************************************************************************/
#define L2POOL_INITIAL_GUESS   (1 << 11)
#define L2POOL_UPSHIFT         16
#define L2POOL_DOWNSHIFT       (L2POOL_UPSHIFT >> 1)
#define L2POOL_NUM_ITERATIONS  (5)

static uint16_t averageL2Norm(uint32_t sumOfSquares, uint16_t count)
{
  uint32_t averageSquared;
  uint32_t outVal;
  // We are assuming uint32_t/uint16_t divide operation is supported
  averageSquared = sumOfSquares / count;
  //printf("value id %d \n",averageSquared);
#if 1 //newton rapson method
  uint32_t scaledAvgSqr;
  uint32_t scaledl2Norm, l2Norm;
  int iter;
  // We are expecting averageSquared values to be 0 < averageSquared < 2^16
  scaledAvgSqr = averageSquared * (1 << L2POOL_UPSHIFT);

  // Use Newton Raphson to find scaledl2Norm = sqrt(scaledAvgSqr)
  // scaledl2Norm[iter+1] = (scaledl2Norm[iter] + (scaledAvgSqr/scaledl2Norm[iter]))/2

  // Final value of scaledl2Norm will be <= 255 * (1 << 8), hence it is reasonable
  // to limit scaledl2Norm to be < (1 << 16).We found that 1 << 11 is a good initial guess
  scaledl2Norm = L2POOL_INITIAL_GUESS;
  for (iter = 0; iter < L2POOL_NUM_ITERATIONS; iter++)
  {
    // scaledAvgSqr is U32, scaledl2Norm is restricted to values < (1 << 16)
    // Hence the division in step below will be a uint32_t/uint16_t divide operation
    scaledl2Norm = scaledl2Norm + (scaledAvgSqr / (uint16_t) scaledl2Norm);
    scaledl2Norm = (scaledl2Norm + 1) >> 1;
    // limit scaledl2Norm to < (1 << 16)
    if (scaledl2Norm >= (1 << L2POOL_UPSHIFT))
    {
      scaledl2Norm = 0x0000ffff;
    }
  }

  // l2Norm = scaledl2Norm/2^8 - with rounding
  l2Norm = (scaledl2Norm + (1 << (L2POOL_DOWNSHIFT - 1))) >> L2POOL_DOWNSHIFT;
  outVal = (uint16_t) l2Norm;
#else //Fast square root method
  int i;
  int est, squareRes, estLeft, estRight, estMiddle;
  int errorMiddle, errorLeft, errorRight;
  squareRes = 0x80;
  for (i = 7; i > 0; i--)
  {
    est       = squareRes * squareRes;
    estLeft   = squareRes | (1 << (i - 1));
    estRight  = (squareRes & ~(1 << i)) | (1 << (i - 1));
    squareRes = averageSquared < est ? estRight : estLeft;
  }
  est         = squareRes * squareRes;
  estLeft     = squareRes - 1;
  estRight    = squareRes + 1;
  estMiddle   = squareRes;
  errorMiddle = abs((estMiddle * estMiddle) - averageSquared);
  errorRight  = abs((estRight * estRight) - averageSquared);
  errorLeft   = abs((estLeft * estLeft) - averageSquared);
  outVal      = errorLeft < errorRight ? estLeft : estRight;
  est         = errorLeft < errorRight ? errorLeft : errorRight;
  outVal      = errorMiddle < est ? estMiddle : outVal;
#endif
  return(outVal);
}

int32_t MultiplyByQuantizedMultiplierSmallerThanOne(int32_t x, int32_t quantized_multiplier, int32_t right_shift)
{
    int32_t input_diff_rescaled = ((((int64_t)(x) * (quantized_multiplier)) + (1 << 30)) >> 31); // SATURATION NOT TAKEN CARE OF IN THIS CODE
    int32_t mask = (1ll << right_shift) - 1;
    int32_t remainder = input_diff_rescaled & mask;
    int32_t threshold = (mask >> 1) + (input_diff_rescaled < 0 ? 1 : 0); // Half in Q(1<<shift)
    return((input_diff_rescaled >> right_shift) + ((remainder > threshold) ? 1 : 0));
}

/*******************************************************************************/
/* Description  : Full Layer Reference implementation for MxN AvgPoolQuantize  */
/*                for Android NN                                               */
/* Inputs       : Input Data Tile, CNN Pooling Parameters, edgeExtendLeft,     */
/*                edgeExtendRight, edgeExtendTop, edgeExtendBottom             */
/* Inout        : Output Data Tile                                             */
/* Output       : XI Error Code                                                */
/* Assumptions  : Input and Output Data are U8                                 */
/*                Kernel size is MxN                                           */
/*                Number of channels in input and output are same              */
/*                Data Orders of inTile and outTile are DWH                    */
/*******************************************************************************/
XI_ERR_TYPE xiAvgPoolQuantizeA3D_MxN_I8_reff(const xi_pTile3D inTile,
    xi_pTile3D outTile,
    const xi_cnn_avgpoolA_params* param,
    const int8_t edgeExtendLeft,
    const int8_t edgeExtendRight,
    const int8_t edgeExtendTop,
    const int8_t edgeExtendBottom)
{
    if (!xiTile3DIsValid_I8_ref(inTile))
    {
        return(XI_ERR_BADARG);
    }
    if (!xiTile3DIsValid_I8_ref(outTile))
    {
        return(XI_ERR_BADARG);
    }
    if (!(XI_TILE3D_GET_DATA_ORDER(inTile) == XI_DWH))
    {
        return(XI_ERR_BADARG);
    }

    if (!(XI_TILE3D_GET_DATA_ORDER(outTile) == XI_DWH))
    {
        return(XI_ERR_BADARG);
    }
    if (!(XI_TILE3D_GET_DIM1(inTile) == XI_TILE3D_GET_DIM1(outTile)))
    {
        return(XI_ERR_CHANNEL_INVALID);
    }
    if (!param)
    {
        return(XI_ERR_NULLARG);
    }
    if (!(XI_CNN_AVGPOOLA_GET_STRIDEX(param) == XI_CNN_AVGPOOLA_GET_STRIDEY(param)))
    {
        return(XI_ERR_BADARG);
    }
    if (!((XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param) > 0) && (XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param) > 0)))
    {
        return(XI_ERR_KSIZE);
    }
    /* Error check for the edges */
    if (!((edgeExtendLeft >= 0) && (edgeExtendRight >= 0) &&
        (edgeExtendTop >= 0) && (edgeExtendBottom >= 0)))
    {
        return(XI_ERR_EDGE);
    }
    if (!((XI_TILE3D_GET_DIM1(inTile) > 0) && (XI_TILE3D_GET_DIM2(inTile) > 0) && (XI_TILE3D_GET_DIM3(inTile) > 0)))
    {
        return(XI_ERR_DATASIZE);
    }
    if (!(XI_CNN_AVGPOOLA_GET_MIN_VAL(param) < XI_CNN_AVGPOOLA_GET_MAX_VAL(param)))
    {
        return(XI_ERR_BADARG);
    }

    int32_t inP1, inP2, inP3;
    int32_t outP1, outP2, outP3;
    int32_t outDataWidth, outDataHeight, numCh;
    int32_t frameWidth, frameHeight;

    /******** Input Tile Parameters ********/
    inP1 = XI_TILE3D_GET_DIM1_PITCH(inTile);
    inP2 = XI_TILE3D_GET_DIM2_PITCH(inTile);
    inP3 = 1;
    frameWidth = XI_TILE3D_GET_DIM2(inTile);
    frameHeight = XI_TILE3D_GET_DIM3(inTile);

    /******** Output Tile Parameters ********/
    outDataWidth = XI_TILE3D_GET_DIM2(outTile);
    outDataHeight = XI_TILE3D_GET_DIM3(outTile);
    numCh = XI_TILE3D_GET_DIM1(outTile);
    outP1 = XI_TILE3D_GET_DIM1_PITCH(outTile);
    outP2 = XI_TILE3D_GET_DIM2_PITCH(outTile);
    outP3 = 1;

    /* Read CNN AVGPOOLA Parameters */
    uint8_t kernelWidthU = XI_CNN_AVGPOOLA_GET_KERNELWIDTH(param);
    uint8_t kernelHeightU = XI_CNN_AVGPOOLA_GET_KERNELHEIGHT(param);
    uint8_t stride = XI_CNN_AVGPOOLA_GET_STRIDE(param);
    int32_t input_zero_point = XI_CNN_AVGPOOLA_GET_ZEROPTINPUT(param);
    int32_t output_zeropoint = XI_CNN_AVGPOOLA_GET_ZEROPTOUTPUT(param);
    int32_t left_shift = XI_CNN_AVGPOOLA_GET_LEFT_SHIFT(param);
    int32_t output_multiplier = XI_CNN_AVGPOOLA_GET_MULTIPLIER_OUT(param);
    int32_t output_shift = XI_CNN_AVGPOOLA_GET_SHIFT_OUT(param);
    int32_t activation_min = XI_CNN_AVGPOOLA_GET_MIN_VAL(param);
    int32_t activation_max = XI_CNN_AVGPOOLA_GET_MAX_VAL(param);

    /* Initialize Data Pointers for Input and Output Tiles */
    int8_t* pInData = (int8_t*)XI_TILE3D_GET_DATA_PTR(inTile);
    int8_t* pOutData = (int8_t*)XI_TILE3D_GET_DATA_PTR(outTile);

    /* Loop Variables */
    int32_t x, y, z, kx, ky, dIdx, isPosFrameEdge, inX, inY;
    int8_t inData;

    for (z = 0; z < numCh; z++)                   // Along output channels
    {
        for (y = 0; y < outDataHeight; y++)         // Along output height
        {
            for (x = 0; x < outDataWidth; x++)        // Along output width
            {
                //Initializing the sumVal and avgVal
                int32_t sumVal = 0;
                int32_t avgVal = 0;

                /* Initializing kernel Size  */
                uint16_t ksizeU = 0;

                //Finding the sum of elements in the receptive field
                for (ky = 0; ky < kernelHeightU; ky++)      // Along kernel height
                {
                    for (kx = 0; kx < kernelWidthU; kx++)     // Along kernel width
                    {
                        inX = x * stride + kx - edgeExtendLeft;
                        inY = y * stride + ky - edgeExtendTop;
                        dIdx = z * inP3 + inY * inP2 + inX * inP1;

                        /* Checks if the input index is outside frame boundary */
                        isPosFrameEdge = ((inY < 0) || (inY > frameHeight - 1) || \
                            (inX < 0) || (inX > frameWidth - 1));

                        if (isPosFrameEdge == 0)
                        {
                            inData = pInData[dIdx];
                            sumVal += ((int32_t)inData - input_zero_point);
                            ksizeU++;
                        }
                    }                         //end of for (kx = 0; kx < kernelWidthU; kx++)
                }                           //end of for (ky = 0; ky < kernelHeightU; ky++)
                if (ksizeU > 0)
                {
                    if (sumVal >= 0)
                    {
                        avgVal = (sumVal + (ksizeU / 2)) / ksizeU;
                    }
                    else
                    {
                        avgVal = (sumVal - (ksizeU / 2)) / ksizeU;
                    }
                }
                const int32_t shifted_avg_val = (avgVal) * (1 << left_shift);
                const int32_t output = MultiplyByQuantizedMultiplierSmallerThanOne(shifted_avg_val, output_multiplier, output_shift) + output_zeropoint;
                avgVal = max(activation_min, min(activation_max, output));

                pOutData[z * outP3 + y * outP2 + x * outP1] = (int8_t)avgVal;
            }       //for (x = 0; x < outDataWidth; x++)
        }         //for (y = 0; y < outDataHeight; y++)
    }           //for (z = 0; z < numCh; z++)

    return(XI_ERR_OK);
}

XI_ERR_TYPE xiAvgPoolA3D_MxN_reff(const xi_pTile3D inTile,
    xi_pTile3D outTile,
    const uint8_t* param,
    const xi_size3D frame3DSize,
    const int8_t edgeExtendLeft,
    const int8_t edgeExtendRight,
    const int8_t edgeExtendTop,
    const int8_t edgeExtendBottom)
{
    /*
    return xiAvgPoolA3D_MxN_U8_reff(inTile,
                                    outTile,
                                    (xi_cnn_pooling_params *)param,
                                    frame3DSize,
                                    edgeExtendLeft,
                                    edgeExtendRight,
                                    edgeExtendTop,
                                    edgeExtendBottom);
    */
    (void)frame3DSize;

    return xiAvgPoolQuantizeA3D_MxN_I8_reff(inTile,
        outTile,
        (xi_cnn_avgpoolA_params*)param,
        //frame3DSize,
        edgeExtendLeft,
        edgeExtendRight,
        edgeExtendTop,
        edgeExtendBottom);
}

// full layer ref. functions to compute ref. output
void computeRef(const pool_params_t& params, struct XtensaOperationArgsIn *input, struct XtensaOperationArgsOut *output)
{
    xi_tile3D inTile;
    xi_tile3D outTile;
    xi_size3D frame3DSize;
    XI_ERR_TYPE(*pPoolFuncRef)(const xi_pTile3D inTile,
        xi_pTile3D outTile,
        const uint8_t * param,
        const xi_size3D frame3DSize,
        const int8_t edgeExtendLeft,
        const int8_t edgeExtendRight,
        const int8_t edgeExtendTop,
        const int8_t edgeExtendBottom);

#if 0
    if (params.type == AVG_POOLING) {
        pPoolFuncRef = xiAvgPoolA3D_MxN_reff;
        //pPoolFuncRef = xiAvgPoolQuantizeA3D_MxN_U8_reff;
    }
    else if (params.type == L2_POOLING) {
        pPoolFuncRef = xiL2PoolA2D_MxN_reff;
    }
    else {
        pPoolFuncRef = xiMaxPoolA3D_MxN_reff;
    }
#else
    pPoolFuncRef = xiAvgPoolA3D_MxN_reff;
#endif

    memset(&inTile, 0, sizeof(inTile));
    memset(&outTile, 0, sizeof(outTile));
    // Input tile edges
    uint32_t edge1W = 0, edge2W = 0, edge1H = 0, edge2H = 0;
    union {
        xi_cnn_pooling_params  pooling_params;
        xi_cnn_l2pool2D_params l2pool2D_params;
        xi_cnn_avgpoolA_params avgPoolQuant_params;
        xi_cnn_maxpoolA_params maxPoolQuant_params;
    } temp_params;
    uint8_t* pool_params = (uint8_t*)&temp_params;
    if (params.type == L2_POOLING) {
        XI_CNN_L2POOL2D_SET_KERNELHEIGHT(&(temp_params.l2pool2D_params), params.kernelH);
        XI_CNN_L2POOL2D_SET_KERNELWIDTH(&(temp_params.l2pool2D_params), params.kernelW);
        XI_CNN_L2POOL2D_SET_STRIDE(&(temp_params.l2pool2D_params), params.stride);
        XI_CNN_L2POOL2D_SET_MIN_VAL(&(temp_params.l2pool2D_params), params.reluMin);
        XI_CNN_L2POOL2D_SET_MAX_VAL(&(temp_params.l2pool2D_params), params.reluMax);
        XI_CNN_L2POOL2D_SET_ZEROPTINPUT(&(temp_params.l2pool2D_params), params.zeroPtInput);
        XI_CNN_L2POOL2D_SET_ZEROPTOUTPUT(&(temp_params.l2pool2D_params), params.zeroPtOutput);
        XI_CNN_L2POOL2D_SET_FRAME_WIDTH(&temp_params.l2pool2D_params, params.input.W);
        XI_CNN_L2POOL2D_SET_FRAME_HEIGHT(&temp_params.l2pool2D_params, params.input.H);
        XI_CNN_L2POOL2D_SET_LEFTEDGE_FLAG(&(temp_params.l2pool2D_params));
        XI_CNN_L2POOL2D_SET_TOPEDGE_FLAG(&(temp_params.l2pool2D_params));
        XI_CNN_L2POOL2D_SET_MULTIPLIER_OUT(&(temp_params.l2pool2D_params), params.multiplierOut);
        XI_CNN_L2POOL2D_SET_SHIFT_OUT(&(temp_params.l2pool2D_params), params.shiftOut);
        XI_CNN_L2POOL2D_SET_LEFT_SHIFT(&(temp_params.l2pool2D_params), params.left_shift);
    }
    else if (params.type == MAX_POOLING) {
        XI_CNN_MAXPOOLA_SET_KERNELHEIGHT(&(temp_params.maxPoolQuant_params), params.kernelH);
        XI_CNN_MAXPOOLA_SET_KERNELWIDTH(&(temp_params.maxPoolQuant_params), params.kernelW);
        XI_CNN_MAXPOOLA_SET_STRIDE(&(temp_params.maxPoolQuant_params), params.stride);
        XI_CNN_MAXPOOLA_SET_MIN_VAL(&(temp_params.maxPoolQuant_params), params.reluMin);
        XI_CNN_MAXPOOLA_SET_MAX_VAL(&(temp_params.maxPoolQuant_params), params.reluMax);
        if (edge1W != 0)
            XI_CNN_MAXPOOLA_SET_LEFTEDGE_FLAG(&(temp_params.maxPoolQuant_params));
        else
            XI_CNN_MAXPOOLA_RESET_LEFTEDGE_FLAG(&(temp_params.maxPoolQuant_params));
        if (edge1H != 0)
            XI_CNN_MAXPOOLA_SET_TOPEDGE_FLAG(&(temp_params.maxPoolQuant_params));
        else
            XI_CNN_MAXPOOLA_RESET_TOPEDGE_FLAG(&(temp_params.maxPoolQuant_params));
        XI_CNN_MAXPOOLA_SET_ZEROPTINPUT(&(temp_params.maxPoolQuant_params), params.zeroPtInput);
        XI_CNN_MAXPOOLA_SET_ZEROPTOUTPUT(&(temp_params.maxPoolQuant_params), params.zeroPtOutput);
        XI_CNN_MAXPOOLA_SET_MULTIPLIER_OUT(&(temp_params.maxPoolQuant_params), params.multiplierOut);
        XI_CNN_MAXPOOLA_SET_SHIFT_OUT(&(temp_params.maxPoolQuant_params), params.shiftOut);
        XI_CNN_MAXPOOLA_SET_LEFT_SHIFT(&(temp_params.maxPoolQuant_params), params.left_shift);
    }
    else if (params.type == AVG_POOLING) {
        XI_CNN_AVGPOOLA_SET_KERNELHEIGHT(&(temp_params.avgPoolQuant_params), params.kernelH);
        XI_CNN_AVGPOOLA_SET_KERNELWIDTH(&(temp_params.avgPoolQuant_params), params.kernelW);
        XI_CNN_AVGPOOLA_SET_STRIDE(&(temp_params.avgPoolQuant_params), params.stride);
        XI_CNN_AVGPOOLA_SET_MIN_VAL(&(temp_params.avgPoolQuant_params), params.reluMin);
        XI_CNN_AVGPOOLA_SET_MAX_VAL(&(temp_params.avgPoolQuant_params), params.reluMax);
        if (edge1W != 0)
            XI_CNN_AVGPOOLA_SET_LEFTEDGE_FLAG(&(temp_params.avgPoolQuant_params));
        else
            XI_CNN_AVGPOOLA_RESET_LEFTEDGE_FLAG(&(temp_params.avgPoolQuant_params));
        if (edge1H != 0)
            XI_CNN_AVGPOOLA_SET_TOPEDGE_FLAG(&(temp_params.avgPoolQuant_params));
        else
            XI_CNN_AVGPOOLA_RESET_TOPEDGE_FLAG(&(temp_params.maxPoolQuant_params));
        XI_CNN_AVGPOOLA_SET_ZEROPTINPUT(&(temp_params.avgPoolQuant_params), params.zeroPtInput);
        XI_CNN_AVGPOOLA_SET_ZEROPTOUTPUT(&(temp_params.avgPoolQuant_params), params.zeroPtOutput);
        XI_CNN_AVGPOOLA_SET_MULTIPLIER_OUT(&(temp_params.avgPoolQuant_params), params.multiplierOut);
        XI_CNN_AVGPOOLA_SET_SHIFT_OUT(&(temp_params.avgPoolQuant_params), params.shiftOut);
        XI_CNN_AVGPOOLA_SET_LEFT_SHIFT(&(temp_params.avgPoolQuant_params), params.left_shift);
    }
    else { //default
        XI_CNN_POOLING_SET_KERNELHEIGHT(&(temp_params.pooling_params), params.kernelH);
        XI_CNN_POOLING_SET_KERNELWIDTH(&(temp_params.pooling_params), params.kernelW);
        XI_CNN_POOLING_SET_STRIDE(&(temp_params.pooling_params), params.stride);
        XI_CNN_POOLING_SET_MIN_VAL(&(temp_params.pooling_params), params.reluMin);
        XI_CNN_POOLING_SET_MAX_VAL(&(temp_params.pooling_params), params.reluMax);
        if (edge1W != 0)
            XI_CNN_POOLING_SET_LEFTEDGE_FLAG(&(temp_params.pooling_params));
        else
            XI_CNN_POOLING_RESET_LEFTEDGE_FLAG(&(temp_params.pooling_params));
        if (edge1H != 0)
            XI_CNN_POOLING_SET_TOPEDGE_FLAG(&(temp_params.pooling_params));
        else
            XI_CNN_POOLING_RESET_TOPEDGE_FLAG(&(temp_params.pooling_params));
    }
    // only for DWH format support
    frame3DSize.dim1Size = params.input.D;
    frame3DSize.dim2Size = params.input.W;
    frame3DSize.dim3Size = params.input.H;

    XI_TILE3D_SET_BUFF_SIZE(&inTile, params.input.D * (params.input.W + edge1W + edge2W) * (params.input.H + edge1H + edge2H));
    XI_TILE3D_SET_DATA_ORDER(&inTile, XI_DWH);
    XI_TILE3D_SET_TYPE(&inTile, XI_TILE3D_U8);
    XI_TILE3D_SET_DIM1_PITCH(&inTile, (params.input.D));
    XI_TILE3D_SET_DIM2_PITCH(&inTile, params.input.D * (params.input.W + edge1W + edge2W));
    XI_TILE3D_SET_DIM1(&inTile, params.input.D);
    XI_TILE3D_SET_DIM2(&inTile, params.input.W);
    XI_TILE3D_SET_DIM3(&inTile, params.input.H);
    XI_TILE3D_SET_DIM2_EDGE1(&inTile, edge1W);
    XI_TILE3D_SET_DIM2_EDGE2(&inTile, edge2W);
    XI_TILE3D_SET_DIM3_EDGE1(&inTile, edge1H);
    XI_TILE3D_SET_DIM3_EDGE2(&inTile, edge2H);

    XI_TILE3D_SET_BUFF_SIZE(&outTile, params.output.D * params.output.W * params.output.H);
    XI_TILE3D_SET_DATA_ORDER(&outTile, XI_DWH);
    XI_TILE3D_SET_TYPE(&outTile, XI_TILE3D_U8);
    XI_TILE3D_SET_DIM1_PITCH(&outTile, params.output.D);
    XI_TILE3D_SET_DIM2_PITCH(&outTile, params.output.D * params.output.W);
    XI_TILE3D_SET_DIM1(&outTile, params.output.D);
    XI_TILE3D_SET_DIM2(&outTile, params.output.W);
    XI_TILE3D_SET_DIM3(&outTile, params.output.H);
    for (unsigned int i = 0; i < params.batch; i++) {
#if 0
    	extern uint8_t* mInput;
        extern uint8_t* mOutput;
#else
    	int8_t* mInput = (int8_t *)input->args[0] ; // todo batch; "(uint8_t *)input->args[0] + b * tile_size"
        int8_t* mOutput = (int8_t *)output->args[0];
#endif
        XI_TILE3D_SET_BUFF_PTR(&inTile, &mInput[i * XI_TILE3D_GET_BUFF_SIZE(&inTile)]);
        XI_TILE3D_SET_DATA_PTR(&inTile, &mInput[i * XI_TILE3D_GET_BUFF_SIZE(&inTile)]);
        XI_TILE3D_SET_BUFF_PTR(&outTile, &mOutput[i * XI_TILE3D_GET_BUFF_SIZE(&outTile)]);
        XI_TILE3D_SET_DATA_PTR(&outTile, &mOutput[i * XI_TILE3D_GET_BUFF_SIZE(&outTile)]);
        pPoolFuncRef(&inTile,
            &outTile,
            pool_params,
            frame3DSize,
            params.kernelW / 2 - params.offsetX,
            params.kernelW / 2 - params.offsetX,
            params.kernelH / 2 - params.offsetY,
            params.kernelH / 2 - params.offsetY);
    }

}
