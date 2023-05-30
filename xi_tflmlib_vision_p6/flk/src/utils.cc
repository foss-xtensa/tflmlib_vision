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
#include "cnnrt.h"
#include "utils.h"

static uint32_t       initDone   _LOCAL_RAM_ = 0;
static uint32_t       totalCores _LOCAL_RAM_;
static uint32_t       myCore     _LOCAL_RAM_;
static local_mem_info_t mem_info _LOCAL_RAM_;
static void           *scratchPad _LOCAL_RAM_ = NULL;

int32_t InitXtensaContext(void)
{
    if(!initDone) {
        totalCores = 1;
        myCore = 0;
        // init the mem manager
        XI_ERR_TYPE status = cnnrt_get_local_mem_info(STACK_SIZE, &mem_info);
        status |= cnnrt_init(32, &mem_info);
        if (!status)
            initDone = 1;
        else
        	return 1;
    }
    return 0;
}

local_mem_info_t *getMeminfoContext(void)
{
    return &mem_info;
}

int32_t getTotalCores(void)
{
	return totalCores;
}

int32_t getMyCore(void)
{
	return myCore;
}

void *getScratchpadBuffer(void)
{
	return scratchPad;
}

void xv_memset(int16_t * pMem, int16_t val, size_t size)
{
	xb_vecNx16 * __restrict pvecMem = (xb_vecNx16 *)pMem;
	valign vaS = IVP_ZALIGN();
	int32_t totalBytes = size;
	for (int32_t col = 0; col < totalBytes; col += (XCHAL_IVPN_SIMD_WIDTH << 1))
	{
		IVP_SAVNX16_XP(val, vaS, pvecMem, totalBytes - col);
	}
	IVP_SAPOSNX16_FP(vaS, pvecMem);
	return;
}

#if 0
int dumpOutputToFile(std::string filename, int dim0, int dim1, int dim2, int dim3, uint8_t *pData)
{
  int i;
  FILE *op;
  int outputSize = dim0 * dim1 * dim2 * dim3;
  if (!(op=fopen(filename.c_str(), "w")))
    exit(-1);
  fprintf(op, "dims:%d, %d, %d, %d \n", dim0, dim1, dim2, dim3);
  for (i = 0; i < outputSize; i++)
    {
      fprintf(op,"[%d]=%d,", i, pData[i]);
      if ((i%10)==0) fprintf(op, "\n");
    }
    fclose(op);
    return 0;
}
#else
int dumpOutputToFile(std::string filename, int dim0, int dim1, int dim2, int dim3, uint8_t* pData)
{
  FILE* op;
  int outputSize = dim0 * dim1 * dim2 * dim3;
  if (!(op = fopen(filename.c_str(), "wb")))
    exit(-1);
  fwrite(pData, 1, outputSize, op);
  fclose(op);
  return 0;
}

#endif
void print_layer_info_header()
{
    printf ("          \t        XI\t    Edge\t    MACs\t      \t           \t  DMA\n");
    printf ("     Total\t    Kernel\t     Ext\t     per\t   MAC\t           \t Wait\n");
    printf ("    Cycles\t    Cycles\t  Cycles\t   Cycle\t    %%\t       MACs\tCycle\t     Layer Name\n");
    printf ("----------\t----------\t--------\t--------\t------\t-----------\t-----\t--------------\n");
}

void PrintLog(const char *_t_name, int num_ops, void *ptr) {
   (void) num_ops;
   (void) ptr;
   (void)_t_name;
#if CNNRT_PERF_LEVEL == CNNRT_PERF_LEVEL_ALL
   unsigned long long *pLog;
   volatile unsigned long long LayerCycles, KernelCyc, EdgeCyc,  macs;
   volatile unsigned long long  DmaQueue;
   volatile unsigned long long waitCyc;
   double macs_per_cycle, mac_percent;
   volatile unsigned long long LayerStart;
   unsigned long long MaxLayerCyc=0;

   pLog = (unsigned long long *) ptr;
   //
   //Print Log
       //print_layer_info_header();
       unsigned long long *pCoreLog = pLog + myCore * num_ops * LOG_END;
       int valid_op=1;
       for (int op=0; op < num_ops; op++) {
           LayerStart = *(pCoreLog + LAYER_START);
           if (LayerStart !=0) {
              LayerCycles = *(pCoreLog + TOT_CYC);
              MaxLayerCyc = (MaxLayerCyc > LayerCycles) ? MaxLayerCyc : LayerCycles;
              KernelCyc = *(pCoreLog + KERNEL_CYC);
              EdgeCyc = *(pCoreLog + EDGE_CYC);
              macs = *(pCoreLog + TOT_MAC);
              macs_per_cycle = *((double *)(pCoreLog + MAC_PER_CYC));
              waitCyc = *(pCoreLog + IDLE_CYC);
              mac_percent = *((double *)(pCoreLog + MAC_PERCENT));
              DmaQueue = *(pCoreLog + DMA_QUEUE);


              printf ("%10llu\t%10llu\t%8llu\t%8.2f\t%6.2f\t%11llu\t%8llu\t%s\n",
                  LayerCycles,
                  KernelCyc,
                  EdgeCyc,
                  macs_per_cycle,
                  mac_percent,
                  macs,
                  waitCyc,
                  _t_name);
             valid_op++;
            }

            pCoreLog += LOG_END;
       }
       //printf("\n\n\n");
   pLog = (unsigned long long *) ptr;
  // printf("Network Cycles = %llu\n", MaxLayerCyc);
  // printf("Performance = %f FPS @1000.00MHz\n", ((double)(1000000000))/(MaxLayerCyc));
#endif


}
