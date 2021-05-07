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
#include <string.h>

/* Accumulator for layer cycles, contauns sum of cycles for all layers. */
uint64_t _LOCAL_RAM_ _cnnrt_perf_totalCycles;
/* Accumulator for kernel cycles over all layers, contauns sum of cycles
   spent in kernel functions without DMA and tiling harness. */
uint64_t _LOCAL_RAM_ _cnnrt_perf_totalKernelCycles;
/* Accumulator for cycles spent in edge extension function in all layers. */
uint64_t _LOCAL_RAM_ _cnnrt_perf_totalEdgeExtensionCycles;
/* Accumulator for cycles spent in kernel in current layer. */
uint64_t _LOCAL_RAM_ _cnnrt_perf_currentKernelCycles;
/* Accumulator for cycles spent in edge extension function in current layer. */
uint64_t _LOCAL_RAM_ _cnnrt_perf_currentEdgeExtensionCycles;

/* Accumulator for full network cycles spent in WAIT for DMA/XNNE completion */
uint64_t _LOCAL_RAM_ _cnnrt_perf_totalidleWaitCycles;
/* Number of cycles spent in WAIT for DMA/XNNE completion */
uint64_t _LOCAL_RAM_ _cnnrt_perf_idleWaitCycles;
/* Temp placeholder for cycle counter. */
uint32_t _LOCAL_RAM_ _cnnrt_perf_temp;

/* Current layer index for printing */
uint16_t _LOCAL_RAM_ _cnnrt_perf_layerIndex;
/* Max queue size observed. */
uint8_t _LOCAL_RAM_ _cnnrt_perf_idmaMaxQueueSize;
/* Was queue overflow detected or not */
uint8_t _LOCAL_RAM_ _cnnrt_perf_idmaQueueOverflow;


/* Print execution summary in the case when per-layer performance info is not tracked. */
void _cnnrt_perf_print_exec_info_summary(uint32_t _compute_cycles, uint64_t frequency,  uint32_t batch, uint64_t _t_macs, uint32_t num_cores, uint32_t macs_per_cycle)
{
    char temp_buff[42];
    if (batch > 1) {
        sprintf(temp_buff, " (FPS of a frame from batch of %d)", batch);
    } else {
        temp_buff[0] = '\0';
    }
    printf("COMPLETED EXECUTION %llu Total Cycles, %llu Cycles in XI Kernels, %llu Cycles in Edge Extension, "
           "%llu Cycles in DSP Idle Wait, %lu DMA Max Queue Size%s, %8.2f MACs per cycle, %0.2f%% MAC Utilization, %llu MACs, %0.2f FPS @%0.2fMHz%s\n\n",
           (unsigned long long)_compute_cycles,
           _cnnrt_perf_totalKernelCycles,
           _cnnrt_perf_totalEdgeExtensionCycles,
           (unsigned long long)_cnnrt_perf_idleWaitCycles,
           (unsigned long)_cnnrt_perf_idmaMaxQueueSize,
           _cnnrt_perf_idmaQueueOverflow ? " (3D STALLS)" : "",
           (double)_t_macs/(_compute_cycles ? _compute_cycles : 1),
           (double)((double)(_t_macs)/((macs_per_cycle ? macs_per_cycle : PEAK_MACS)*num_cores))*100/(_compute_cycles ? _compute_cycles : 1),
           (unsigned long long)_t_macs,
           ((double)(frequency * batch * 1000 * 1000)/(_compute_cycles ? _compute_cycles : 1)),
           (double)frequency,
           temp_buff);
}


/* Print performance info header */

void _cnnrt_perf_print_layer_info_header()
{
    printf ("                         XI     Edge       DSP     MACs                      DMA\n");
    printf ("           Total     Kernel      Ext Idle WAIT      per    MAC             Queue\n");
    printf ("    #     Cycles     Cycles   Cycles    Cycles    Cycle      %%        MACs  Size Layer Name\n");
    printf ("----- ---------- ---------- -------- --------- -------- ------ ----------- ----- -----------------------------\n");
    _cnnrt_perf_layerIndex = 1;
}

/* Print per-layer performance statistics. */
void _cnnrt_perf_print_layer_info(const char *_t_name, uint64_t _t_cycles_layer, uint64_t _t_macs, uint32_t num_cores, uint32_t macs_per_cycle)
{
    printf ("%5d %10llu %10llu %8llu %9llu %8.2f %6.2f %11llu %5lu %s%s\n",
            (unsigned int)_cnnrt_perf_layerIndex++,
            (unsigned long long)_t_cycles_layer,
            (unsigned long long)_cnnrt_perf_currentKernelCycles,
            (unsigned long long)_cnnrt_perf_currentEdgeExtensionCycles,
            (unsigned long long)_cnnrt_perf_idleWaitCycles,
            (double)_t_macs/(_t_cycles_layer ? _t_cycles_layer : 1),
            (double)((double)(_t_macs)/((macs_per_cycle ? macs_per_cycle : PEAK_MACS)*num_cores))*100/(_t_cycles_layer ? _t_cycles_layer : 1),
            (unsigned long long)_t_macs,
            (unsigned long)_cnnrt_perf_idmaMaxQueueSize,
            _t_name,
            _cnnrt_perf_idmaQueueOverflow ? " (3D DMA STALLS)" : "");
}

/* Print execution summary in the case when per-layer performance info is tracked. */
void _cnnrt_perf_print_exec_info(uint64_t frequency,  uint32_t batch, uint64_t _t_macs, uint32_t num_cores, uint32_t macs_per_cycle)
{
    char temp_buff[42];
    if (batch > 1) {
        sprintf(temp_buff, " (FPS of a frame from batch of %d)", batch);
    } else {
        temp_buff[0] = '\0';
    }

    printf ("----- ---------- ---------- -------- --------- -------- ------ ----------- ----- -----------------------------\n");
    printf ("TOTAL %10llu %10llu %8llu %9llu %8.2f %6.2f %11llu\nPERFORMANCE %0.2f FPS @%0.2fMHz%s\n\n",
            (unsigned long long)_cnnrt_perf_totalCycles,
            (unsigned long long)_cnnrt_perf_totalKernelCycles,
            (unsigned long long)_cnnrt_perf_totalEdgeExtensionCycles,
            (unsigned long long)_cnnrt_perf_totalidleWaitCycles,
            (double)_t_macs/(_cnnrt_perf_totalCycles ? _cnnrt_perf_totalCycles : 1),
            (double)((double)(_t_macs)/((macs_per_cycle ? macs_per_cycle : PEAK_MACS)*num_cores))*100/(_cnnrt_perf_totalCycles ? _cnnrt_perf_totalCycles : 1),
            (unsigned long long)_t_macs,
            ((double)(frequency * batch * 1000 * 1000)/(_cnnrt_perf_totalCycles ? _cnnrt_perf_totalCycles : 1)),
            (double)frequency,
            temp_buff);
}
