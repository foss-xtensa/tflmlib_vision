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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <float.h>
#include "cnnrt.h"
#include <xtensa/config/core-isa.h>
#include "xi_platform.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>


//Enable this to get quantizer style dumps
//#define QUANTIZER_COMPATIBLE_DUMP 1

/* Option to dump tensor blob to individual files named after
   data object instead of printing to stdout.
   Boolean. Disabled by default.
*/

int _cnnrt_debug_dump_to_files = 1;

/* Print buffer of unsigned char.

   Args:
       buffer: pointer to the first element.
       stride: stride in elements.
       size: number of elments to print.
*/
static void print_U8(FILE *f, const void *buffer, unsigned stride, unsigned size)
{
    const char *term = (size % 16 == 0) ? "" : "\n";
    const uint8_t *ptr = (const uint8_t *)buffer;
    int eofs_index = 0;

#ifndef QUANTIZER_COMPATIBLE_DUMP
    while (size >= 8) {
        const char *eofs[2] = { "   ", "\n" };
        fprintf (f, "%02x %02x %02x %02x %02x %02x %02x %02x%s",
                 ptr[0*stride], ptr[1*stride], ptr[2*stride], ptr[3*stride],
                 ptr[4*stride], ptr[5*stride], ptr[6*stride], ptr[7*stride],
                 eofs[eofs_index]);
        size -= 8;
        ptr += 8*stride;
        eofs_index = eofs_index ^ 1;
    }

    while (size > 0) {
        fprintf (f, "%02x ", ptr[0]);
        size --;
        ptr += stride;
    }

    fprintf(f, "%s\n", term);
#else // QUANTIZER_COMPATIBLE_DUMP
    while (size > 0) {
        fprintf (f, "%d,", (signed char)(ptr[0]));
        size --;
        ptr += stride;
    }
#endif // QUANTIZER_COMPATIBLE_DUMP
}

/* Print buffer of 16 bit unsigned int.

   Args:
       buffer: pointer to the first element.
       stride: stride in elements.
       size: number of elments to print.
*/
static void print_U16(FILE *f, const void *buffer, unsigned stride, unsigned size)
{
    const char *term = (size % 8 == 0) ? "" : "\n";
    const uint16_t *ptr = (const uint16_t *)buffer;

#ifndef QUANTIZER_COMPATIBLE_DUMP
    while (size >= 8) {
        fprintf (f, "%04x %04x %04x %04x %04x %04x %04x %04x\n",
                 ptr[0*stride], ptr[1*stride], ptr[2*stride], ptr[3*stride],
                 ptr[4*stride], ptr[5*stride], ptr[6*stride], ptr[7*stride]);
        size -= 8;
        ptr += 8*stride;
    }

    while (size > 0) {
        fprintf (f, "%04x ", ptr[0]);
        size --;
        ptr += stride;
    }

    fprintf(f, "%s\n", term);
#else // QUANTIZER_COMPATIBLE_DUMP
    while (size > 0) {
        fprintf (f, "%d,", (signed short)(ptr[0]));
        size --;
        ptr += stride;
    }
#endif // QUANTIZER_COMPATIBLE_DUMP
}

/* Print buffer of 32 bit unsigned int.

   Args:
       buffer: pointer to the first element.
       stride: stride in elements.
       size: number of elments to print.
*/
static void print_U32(FILE *f, const void *buffer, unsigned stride, unsigned size)
{
    const char *term = (size % 8 == 0) ? "" : "\n";
    const uint32_t *ptr = (const uint32_t *)buffer;

#ifndef QUANTIZER_COMPATIBLE_DUMP
    while (size >= 8) {
        fprintf (f, "%08x %08x %08x %08x %08x %08x %08x %08x\n",
                 ptr[0*stride], ptr[1*stride], ptr[2*stride], ptr[3*stride],
                 ptr[4*stride], ptr[5*stride], ptr[6*stride], ptr[7*stride]);
        size -= 8;
        ptr += 8*stride;
    }

    while (size > 0) {
        fprintf (f, "%08x ", ptr[0]);
        size --;
        ptr += stride;
    }

    fprintf(f, "%s\n", term);
#else // QUANTIZER_COMPATIBLE_DUMP
    while (size > 0) {
        fprintf (f, "%d,", (int32_t)(ptr[0]));
        size --;
        ptr += stride;
    }
#endif // QUANTIZER_COMPATIBLE_DUMP
}

/* Print buffer of FLOAT

   Args:
       buffer: pointer to the first element.
       stride: stride in elements.
       size: number of elments to print.
*/
static void print_FLOAT(FILE *f, const void *buffer, unsigned stride, unsigned size)
{
    const char *term = (size % 8 == 0) ? "" : "\n";
    const float *ptr = (const float *)buffer;

#ifndef QUANTIZER_COMPATIBLE_DUMP
    while (size >= 8) {
        fprintf (f, "%f %f %f %f %f %f %f %f\n",
                 ptr[0*stride], ptr[1*stride], ptr[2*stride], ptr[3*stride],
                 ptr[4*stride], ptr[5*stride], ptr[6*stride], ptr[7*stride]);
        size -= 8;
        ptr += 8*stride;
    }

    while (size > 0) {
        fprintf (f, "%f ", ptr[0]);
        size --;
        ptr += stride;
    }

    fprintf(f, "%s\n", term);
#else // QUANTIZER_COMPATIBLE_DUMP
    while (size > 0) {
        fprintf (f, "%d,", (int)(ptr[0]));
        size --;
        ptr += stride;
    }
#endif // QUANTIZER_COMPATIBLE_DUMP
}

void dump_buffer(const char *name, const void *buffer, unsigned size)
{
    /* Make sure all transfers complete */
    _cnnrt_wait_all();

    printf("==== BUFFER: %s\n", (name) ? name : "");

    if (buffer != NULL) {
        print_U8(stdout, buffer, 1, size);
    } else {
        printf("Pinned buffer.\n");
    }
}

#if !defined(GOLDEN_DIAG) && defined(ENABLE_XNNE_PERF_COUNTER) && (ENABLE_XNNE_PERF_COUNTER)

#if defined(PERF_SET_1) && (PERF_SET_1)
#define XNNE_PERF_EVENT_0 XNNE_EVENT_DmaOut_GoMsgTransferCycleCount	 	///* Number of cycles for Go message transfer after last dependent event occurs
#define XNNE_PERF_EVENT_1 XNNE_EVENT_DmaIn_IOGoMsgTransferCycleCount	///* Number of cycles for Activation & OBUF Go message transfer after last dependent event occurs
#define XNNE_PERF_EVENT_2 XNNE_EVENT_DmaIn_CGoMsgTransferCycleCount		///* Number of cycles for Coefficient Go message transfer after last dependent event occurs
#define XNNE_PERF_EVENT_3 XNNE_EVENT_Vpu_GoMsgCalcCycleCount          ///* Number of cycles for Vpu calculation after last dependent event occurs
#endif

#if defined(PERF_SET_2) && (PERF_SET_2)
#define XNNE_PERF_EVENT_0 XNNE_EVENT_DmaOut_GoMsgCycleCount	  ///* Go message Latency, Number of cycles required for a Go message processing
#define XNNE_PERF_EVENT_1 XNNE_EVENT_DmaIn_IOGoMsgCycleCount	///* Number of cycles required to process Activation & OBUF Go message
#define XNNE_PERF_EVENT_2 XNNE_EVENT_DmaIn_CGoMsgCycleCount	  ///* Number of cycles required to process Coefficient Go message
#define XNNE_PERF_EVENT_3 XNNE_EVENT_Vpu_GoMsgWaitCount       ///* Number of cycles Vpu waited for dependent events before it starts calculation
#endif

#if defined(PERF_SET_3) && (PERF_SET_3)
#define XNNE_PERF_EVENT_0 XNNE_EVENT_Tf_GoMsgCycleCount	///* Count of number of cycles taken to process TF_Go messages
#define XNNE_PERF_EVENT_1 XNNE_EVENT_Tf_GoMsgWaitCount	///* number of cycles taken waiting for input events before starting TF_Go messages
#define XNNE_PERF_EVENT_2 XNNE_EVENT_Cycles             ///* XNNE Cycles
#define XNNE_PERF_EVENT_3 XNNE_EVENT_Mblk_MulCount      ///* Count of number of multiplications performed by up to 8 MBLK’s
#endif

/* Performance Counters */
#define PERF_LAYERS 1000
uint32_t layer_cntr = 0;
uint32_t *perf_cntr0 = 0;
uint32_t *perf_cntr1 = 0;
uint32_t *perf_cntr2 = 0;
uint32_t *perf_cntr3 = 0;

void print_perf_counters()
{
#if defined(XTSC_DIAG)
  if (perf_cntr0 != NULL && perf_cntr1 != NULL && perf_cntr2 != NULL && perf_cntr3 != NULL)
  {
    int i = 0;
    uint32_t cnt0 = perf_cntr0[0];
    uint32_t cnt1 = perf_cntr1[0];
    uint32_t cnt2 = perf_cntr2[0];
    uint32_t cnt3 = perf_cntr3[0];

    while(i++ < layer_cntr) {
#if defined(PERF_SET_1) && (PERF_SET_1)
      //printf("XNNE Perf Counter Layer: %d , DmaOut: %d , DmaIn: %d , DmaInC: %d , VPU: %d\n",perf_cntr0[i],perf_cntr1[i],perf_cntr2[i],perf_cntr3[i]);
      printf("Layer: %u , DmaOut: %u , DmaIn: %u , DmaInC: %u , VPU: %u\n",i,cnt0,cnt1,cnt2,cnt3);
#endif
#if defined(PERF_SET_2) && (PERF_SET_2)
      printf("Layer:%u , DmaOut(+wait): %u , DmaInIO(+wait): %u , DmaInC(+wait): %u , VPU(wait): %u\n",i,cnt0,cnt1,cnt2,cnt3);
#endif
#if defined(PERF_SET_3) && (PERF_SET_3)
      printf("Layer: %u , TF: %u , TF(wait): %u , XNNE: %u , MACs: %u \n",i,cnt0,cnt1,cnt2,cnt3);
#endif
        cnt0 = perf_cntr0[i] - perf_cntr0[i-1];
        cnt1 = perf_cntr1[i] - perf_cntr1[i-1];
        cnt2 = perf_cntr2[i] - perf_cntr2[i-1];
        cnt3 = perf_cntr3[i] - perf_cntr3[i-1];
    }
  }
#endif
}

void release_perf_counters()
{
  if (perf_cntr0 != NULL)
      free(perf_cntr0);
  if (perf_cntr1 != NULL)
      free(perf_cntr1);
  if (perf_cntr2 != NULL)
      free(perf_cntr2);
  if (perf_cntr3 != NULL)
      free(perf_cntr3);
}

void setup_perf_counters()
{
    /* Initialize the Performance event counters */
    xthal_xnne_init_counter(0, 0, XNNE_PERF_EVENT_0);
    xthal_xnne_init_counter(0, 1, XNNE_PERF_EVENT_1);
    xthal_xnne_init_counter(0, 2, XNNE_PERF_EVENT_2);
    xthal_xnne_init_counter(0, 3, XNNE_PERF_EVENT_3);

    /* Start the Performance event counters */
    xthal_xnne_start_counters(0, XNNE_PERF_COUNTER0|XNNE_PERF_COUNTER1|XNNE_PERF_COUNTER2|XNNE_PERF_COUNTER3);

    /* Allocate memory for counters*/
    perf_cntr0 = (uint32_t*)memalign(2*XCHAL_IVPN_SIMD_WIDTH, PERF_LAYERS*sizeof(uint32_t));
    perf_cntr1 = (uint32_t*)memalign(2*XCHAL_IVPN_SIMD_WIDTH, PERF_LAYERS*sizeof(uint32_t));
    perf_cntr2 = (uint32_t*)memalign(2*XCHAL_IVPN_SIMD_WIDTH, PERF_LAYERS*sizeof(uint32_t));
    perf_cntr3 = (uint32_t*)memalign(2*XCHAL_IVPN_SIMD_WIDTH, PERF_LAYERS*sizeof(uint32_t));
}

void read_perf_counters()
{
    /* Snapshot the Performance event counters */
    xthal_xnne_counter_snapshot(0);

    /* Read the Performance event counters */
    xthal_xnne_read_counter(0, 0, &perf_cntr0[layer_cntr]);
    xthal_xnne_read_counter(0, 1, &perf_cntr1[layer_cntr]);
    xthal_xnne_read_counter(0, 2, &perf_cntr2[layer_cntr]);
    xthal_xnne_read_counter(0, 3, &perf_cntr3[layer_cntr]);

    layer_cntr++;
}
#endif// ENABLE_XNNE_PERF_COUNTER

#if defined(GOLDEN_DIAG) || defined (XTSC_DIAG) || defined(RTL_DIAG) || defined(POWER_DIAG)

#define MASK8  (0x000000FF)
#define MASK16 (0x0000FFFF)
#define MASK24 (0x00FFFFFF)
#define MASKN(N,W) (((N) == 3) ? ((W) & MASK24) : (((N) == 2) ? ((W) & MASK16) : ((W) & MASK8)))

// Output directory to dump.
extern char *output_dir;

static void dump_buffer_int(const char *name,
                            const char *suffix,
                            int element_size,
                            void (*printer)(FILE *f, const void *buffer, unsigned stride, unsigned size),
                            const void *buffer,
                            unsigned dim0, unsigned stride0,
                            unsigned dim1, unsigned stride1,
                            unsigned dim2, unsigned stride2,
                            unsigned dim3, unsigned stride3)
{
    int size = element_size * ((stride3!=0) ? (stride3*dim3) : ((stride2!=0) ? (stride2*dim2) : ((stride1!=0) ? (stride1*dim1) : (stride0*dim0))));

    int is_mt_inl = (strcmp(name, "minitest_inl") == 0);                  //Check for call from dump_tiles(input)
    int is_mt_outl = (strcmp(name, "minitest_outl") == 0);                  //Check for call from dump_tiles(input)
    int is_mt_in  = (strcmp(name, "minitest_in") == 0);                   //Check for call from dump_tiles(input)
    int is_mt_out = (strcmp(name, "minitest_out") == 0);                  //Check for call from dump_tiles(output)
    int is_mt_in_out = (is_mt_inl || is_mt_outl || is_mt_in || is_mt_out); //Check for call from dump_tiles(output|input)

    if(is_mt_in_out) {
      size /= element_size; //dump_tiles passes the actual byte size in dim0 field (not the dimension). Adjust for element_size.
    }

    uint8_t* ptr = (const uint8_t *)buffer;

    //printf("%s: size:%d, name:%s suffix:%s\n",__func__,size,name,suffix);

    /* Make sure all transfers complete */
    _cnnrt_wait_all();

    // Assembly format array (XTSC)
#if defined(GOLDEN_DIAG)
    char *out_name[MAXFILENAMELEN];
    FILE *output_file;
    int k;
    int32_t* ptr_32 = (const int32_t*)buffer;
    int size_32 = size/(sizeof(int32_t));
    int size_32_mod = size % sizeof(int32_t);

    static int dump_cnt = 0;

    struct stat st = {0};
    if (stat(output_dir, &st) == -1) {
       mkdir(output_dir, 0700);
    }

    if(is_mt_in_out) { //dump_tiles needs the .S with correct names.
      snprintf(out_name, MAXFILENAMELEN, "%s/golden_%s.S", output_dir, suffix);
      output_file = fopen(out_name, "wb");
      if (output_file == NULL) {
         printf("Output Dump File path dir %s is invalid. Please create it.\n",out_name);
         exit(0);
      }
      fprintf(output_file, "    .section    .data.cnnsysact%s\n",suffix);
      fprintf(output_file, "    .global %s\n    .type %s, @object\n    .align 128\n%s:\n    .byte ",suffix,suffix,suffix);

      if(is_mt_out) { // Keep output.S zeroed to avoid false-positive checks
         for (k=0; k<size-1; k++)
            fprintf(output_file, "0x0,");
         fprintf(output_file, "0x0");
      } else {
         for (k=0; k<size-1; k++)
            fprintf(output_file, "0x%02hhx,", (unsigned char)ptr[k]);
         fprintf(output_file, "0x%02hhx", (unsigned char)ptr[k]);
      }
      fprintf(output_file, "\n    .size %s, .-%s\n",suffix,suffix);
      fclose(output_file);

      // Binary format output (RTL)
      snprintf(out_name, MAXFILENAMELEN, "%s/golden_%d.dat", output_dir, dump_cnt++);
      output_file = fopen(out_name, "wb");
      if (output_file == NULL) {
        printf("Output Dump File path dir %s is invalid. Please create it.\n", out_name);
        exit(0);
      }

      fprintf(output_file, "@%x\n", (unsigned int)ptr_32);
      for (k=0; k<size_32; k++)
          fprintf(output_file, "%x\n", ptr_32[k]);
      if (size_32_mod)
          fprintf(output_file, "%x\n", MASKN(size_32_mod, ptr_32[k]));
      fclose(output_file);
    }

#elif defined(XTSC_DIAG)

#if !defined(RTL_DIAG_DEBUG)
    if(is_mt_in_out) { //Output.dat is generated from layer sequence. Avoid doing it from dump_tiles().
#endif

      /* Simcall to load image to memory */
      if (is_mt_inl) {
          XT_MEMW();
          if(xt_iss_simcall(4, &ptr[0], size, dim0, dim1, 0)) {
             printf ("Image List Complete. Exiting\n");
             exit(0);
          }
          XT_MEMW();
      }

#if defined(ENABLE_XNNE_PERF_COUNTER) && (ENABLE_XNNE_PERF_COUNTER)
      if (is_mt_in || is_mt_inl) {
        // Initialize Perf Counters
        setup_perf_counters();
      } else {
        // Read Perf Counters
        read_perf_counters();
      }
#endif //ENABLE_XNNE_PERF_COUNTER

      //Simcall to xtsc simulation to dump buffer
      //Dump DMAout SysM buffer to file
      XT_MEMW();
      xt_iss_simcall(((is_mt_inl || is_mt_outl) ? 3 : 0), &ptr[0], size, 0, 0, 0);
      XT_MEMW();

#if !defined(RTL_DIAG_DEBUG)
    }
#endif

#elif defined(RTL_DIAG) || defined(POWER_DIAG)

#if !defined(RTL_DIAG_DEBUG)
    if(is_mt_in_out) { //Output.dat is generated from layer sequence. Avoid doing it from dump_tiles().
#endif

      /* Simcall to load image to memory */
      if (is_mt_inl) {
          XT_MEMW();
          set_memload_address_region(&ptr[0], size);
          XT_MEMW();
      }

#if defined(ENABLE_XNNE_PERF_COUNTER) && (ENABLE_XNNE_PERF_COUNTER)
      if (is_mt_in || is_mt_inl) {
        // Initialize Perf Counters
        setup_perf_counters();
      } else {
        // Read Perf Counters
        read_perf_counters();
      }
#endif //ENABLE_XNNE_PERF_COUNTER

      //Simcall to socgo testbench to dump buffer
      //Dump DMAout SysM buffer to file
      XT_MEMW();
      set_memdump_address_region(&ptr[0], size);
      XT_MEMW();

#if !defined(RTL_DIAG_DEBUG)
    }
#endif

#endif //if defined(GOLDEN_DIAG)

}
#else
static void dump_buffer_int(const char *name,
                            const char *suffix,
                            int element_size,
                            void (*printer)(FILE *f, const void *buffer, unsigned stride, unsigned size),
                            const void *buffer,
                            unsigned dim0, unsigned stride0,
                            unsigned dim1, unsigned stride1,
                            unsigned dim2, unsigned stride2,
                            unsigned dim3, unsigned stride3)
{

    const uint8_t *ptr = (const uint8_t *)buffer;
    FILE *f;
    if (!name)
        name = "none";
    if (!suffix)
        suffix = "none";
    if (_cnnrt_debug_dump_to_files) {
        printf("saving tensor to file: %s\n", name);
        f = fopen(name, "w");
        if (!f) {
            printf("Cannot create file %s for dumping tensor data\n", name);
            return;
        }
        fprintf(f, "==== DIMENSIONS(innermost:...:outermost): %d:%d:%d:%d\n", dim0, dim1, dim2, dim3);
    } else {
        f = stdout;
        fprintf(f, "==== BUFFER: '%s:%s' DIMENSIONS(innermost:...:outermost): %d:%d:%d:%d\n",
                name, suffix, dim0, dim1, dim2, dim3);
    }

    /* Make sure all transfers complete */
    _cnnrt_wait_all();

    if (ptr != NULL) {
#ifdef __XCC__
#if XCHAL_DCACHE_SIZE
        size_t buffer_size = ((dim3-1) * stride3 + (dim2-1) * stride2 + (dim1-1) * stride1 + (dim0-1) * stride0 + 1) * element_size;
        xthal_dcache_region_writeback_inv((void*)buffer, buffer_size); /* use writeback + invalidate to accomodate full-layer reference */
#endif
#endif

        for (unsigned i3=0; i3<dim3; i3++) {
            for (unsigned i2=0; i2<dim2; i2++) {
                for (unsigned i1=0; i1<dim1; i1++) {
#ifndef QUANTIZER_COMPATIBLE_DUMP
                    fprintf(f, "[%d][%d][%d]:\n", i1, i2, i3);
#endif // QUANTIZER_COMPATIBLE_DUMP
                    printer(f, &ptr[element_size*(i3*stride3 + i2*stride2 + i1*stride1)], stride0, dim0);
                }
            }
        }
#ifdef __XCC__
#if XCHAL_DCACHE_SIZE
        xthal_dcache_region_writeback_inv((void*)buffer, buffer_size);
#endif
#endif
    } else {
        fprintf(f, "Pinned buffer.\n");
    }
#ifdef QUANTIZER_COMPATIBLE_DUMP
    fprintf(f, "\n");
#endif // QUANTIZER_COMPATIBLE_DUMP
    if (_cnnrt_debug_dump_to_files) {
        fprintf(f, "==== END OF BUFFER\n");
        fclose(f);
    } else {
        fprintf(f, "==== END OF BUFFER: '%s:%s'\n", name, suffix);
    }
}
#endif//GOLDEN/XTSC/RTL/POWER_DIAG

void dump_buffer_U8(const char *name, const char *suffix, const void *buffer, unsigned dim0, unsigned stride0,
                    unsigned dim1, unsigned stride1, unsigned dim2, unsigned stride2,
                    unsigned dim3, unsigned stride3)
{
    dump_buffer_int(name, suffix, sizeof(uint8_t), print_U8, buffer, dim0, stride0, dim1, stride1, dim2, stride2, dim3, stride3);
}

void dump_buffer_U16(const char *name, const char *suffix, const void *buffer, unsigned dim0, unsigned stride0,
                     unsigned dim1, unsigned stride1, unsigned dim2, unsigned stride2,
                     unsigned dim3, unsigned stride3)
{
    dump_buffer_int(name, suffix, sizeof(uint16_t), print_U16, buffer, dim0, stride0, dim1, stride1, dim2, stride2, dim3, stride3);
}

void dump_buffer_U32(const char *name, const char *suffix, const void *buffer, unsigned dim0, unsigned stride0,
                     unsigned dim1, unsigned stride1, unsigned dim2, unsigned stride2,
                     unsigned dim3, unsigned stride3)
{
    dump_buffer_int(name, suffix, sizeof(uint32_t), print_U32, buffer, dim0, stride0, dim1, stride1, dim2, stride2, dim3, stride3);
}

void dump_buffer_FLOAT(const char *name, const char *suffix, const void *buffer, unsigned dim0, unsigned stride0,
                     unsigned dim1, unsigned stride1, unsigned dim2, unsigned stride2,
                     unsigned dim3, unsigned stride3)
{
    dump_buffer_int(name, suffix, sizeof(float), print_FLOAT, buffer, dim0, stride0, dim1, stride1, dim2, stride2, dim3, stride3);
}

void print_stats_U8(const char *name, const void *buffer, unsigned dim0, unsigned stride0,
                    unsigned dim1, unsigned stride1, unsigned dim2, unsigned stride2,
                    unsigned dim3, unsigned stride3)
{
    const uint8_t *ptr = (const uint8_t *)buffer;
    unsigned int total, zeros;
    unsigned int val;
    unsigned int min_val = 0;
    unsigned int max_val = 0;

    /* Make sure all transfers complete */
    _cnnrt_wait_all();

    total = dim0 * dim1 * dim2 * dim3;
    zeros = 0;
    if (ptr != NULL) {
#ifdef __XCC__
#if XCHAL_DCACHE_SIZE
        size_t buffer_size = ((dim3-1) * stride3 + (dim2-1) * stride2 + (dim1-1) * stride1 + (dim0-1) * stride0 + 1) * 1;
        xthal_dcache_region_writeback_inv((void*)buffer, buffer_size); /* use writeback + invalidate to accomodate full-layer reference */
#endif
#endif
        for (unsigned i3=0; i3<dim3; i3++) {
            for (unsigned i2=0; i2<dim2; i2++) {
                for (unsigned i1=0; i1<dim1; i1++) {
                    for (unsigned i0=0; i0<dim0; i0++) {
                        val = ptr[i3*stride3 + i2*stride2 + i1*stride1 + i0*stride0];
                        if (val == 0) {
                            zeros++;
                        }
                        if (val < min_val) {
                            min_val = val;
                        }
                        if (val > max_val) {
                            max_val = val;
                        }
                    }
                }
            }
        }
        printf("[STATS] %s, dims: %dx%dx%dx%d, total: %d, min_val: %d, max_val: %d, zeros: %d, zero ratio: %.2f\n",
                    name, dim0, dim1, dim2, dim3, total, min_val, max_val, zeros, (float)zeros/total);
#ifdef __XCC__
#if XCHAL_DCACHE_SIZE
        xthal_dcache_region_writeback_inv((void*)buffer, buffer_size);
#endif
#endif
    } else {
        printf("[STATS] %s, pinned buffer\n", name);
    }
}

void print_stats_U16(const char *name, const void *buffer, unsigned dim0, unsigned stride0,
                     unsigned dim1, unsigned stride1, unsigned dim2, unsigned stride2,
                     unsigned dim3, unsigned stride3)
{
    const uint16_t *ptr = (const uint16_t *)buffer;
    unsigned int total, zeros;
    unsigned int val;
    unsigned int min_val = 0;
    unsigned int max_val = 0;

    /* Make sure all transfers complete */
    _cnnrt_wait_all();

    total = dim0 * dim1 * dim2 * dim3;
    zeros = 0;
    if (ptr != NULL) {
#ifdef __XCC__
#if XCHAL_DCACHE_SIZE
        size_t buffer_size = ((dim3-1) * stride3 + (dim2-1) * stride2 + (dim1-1) * stride1 + (dim0-1) * stride0 + 1) * 2;
        xthal_dcache_region_writeback_inv((void*)buffer, buffer_size); /* use writeback + invalidate to accomodate full-layer reference */
#endif
#endif
        for (unsigned i3=0; i3<dim3; i3++) {
            for (unsigned i2=0; i2<dim2; i2++) {
                for (unsigned i1=0; i1<dim1; i1++) {
                    for (unsigned i0=0; i0<dim0; i0++) {
                        val = ptr[i3*stride3 + i2*stride2 + i1*stride1 + i0*stride0];
                        if (val == 0) {
                            zeros++;
                        }
                        if (val < min_val) {
                            min_val = val;
                        }
                        if (val > max_val) {
                            max_val = val;
                        }
                    }
                }
            }
        }
        printf("[STATS] %s, dims: %dx%dx%dx%d, total: %d, min_val: %d, max_val: %d, zeros: %d, zero ratio: %.2f\n",
                    name, dim0, dim1, dim2, dim3, total, min_val, max_val, zeros, (float)zeros/total);
#ifdef __XCC__
#if XCHAL_DCACHE_SIZE
        xthal_dcache_region_writeback_inv((void*)buffer, buffer_size);
#endif
#endif
    } else {
        printf("[STATS] %s, pinned buffer\n", name);
    }
}

void print_stats_U32(const char *name, const void *buffer, unsigned dim0, unsigned stride0,
                     unsigned dim1, unsigned stride1, unsigned dim2, unsigned stride2,
                     unsigned dim3, unsigned stride3)
{
    const uint32_t *ptr = (const uint32_t *)buffer;
    unsigned int total, zeros;
    unsigned int val;
    unsigned int min_val = 0;
    unsigned int max_val = 0;

    /* Make sure all transfers complete */
    _cnnrt_wait_all();

    total = dim0 * dim1 * dim2 * dim3;
    zeros = 0;
    if (ptr != NULL) {
#ifdef __XCC__
#if XCHAL_DCACHE_SIZE
        size_t buffer_size = ((dim3-1) * stride3 + (dim2-1) * stride2 + (dim1-1) * stride1 + (dim0-1) * stride0 + 1) * 4;
        xthal_dcache_region_writeback_inv((void*)buffer, buffer_size); /* use writeback + invalidate to accomodate full-layer reference */
#endif
#endif
        for (unsigned i3=0; i3<dim3; i3++) {
            for (unsigned i2=0; i2<dim2; i2++) {
                for (unsigned i1=0; i1<dim1; i1++) {
                    for (unsigned i0=0; i0<dim0; i0++) {
                        val = ptr[i3*stride3 + i2*stride2 + i1*stride1 + i0*stride0];
                        if (val == 0) {
                            zeros++;
                        }
                        if (val < min_val) {
                            min_val = val;
                        }
                        if (val > max_val) {
                            max_val = val;
                        }
                    }
                }
            }
        }
        printf("[STATS] %s, dims: %dx%dx%dx%d, total: %d, min_val: %d, max_val: %d, zeros: %d, zero ratio: %.2f\n",
                    name, dim0, dim1, dim2, dim3, total, min_val, max_val, zeros, (float)zeros/total);
#ifdef __XCC__
#if XCHAL_DCACHE_SIZE
        xthal_dcache_region_writeback_inv((void*)buffer, buffer_size);
#endif
#endif
    } else {
        printf("[STATS] %s, pinned buffer\n", name);
    }
}

void print_stats_FLOAT(const char *name, const void *buffer, unsigned dim0, unsigned stride0,
                     unsigned dim1, unsigned stride1, unsigned dim2, unsigned stride2,
                     unsigned dim3, unsigned stride3)
{
    const float *ptr = (const float *)buffer;
    unsigned int total, zeros;
    float val;
    float min_val;
    float max_val;

    /* Make sure all transfers complete */
    _cnnrt_wait_all();

    total = dim0 * dim1 * dim2 * dim3;
    zeros = 0;
    if (ptr != NULL) {
#ifdef __XCC__
#if XCHAL_DCACHE_SIZE
        size_t buffer_size = ((dim3-1) * stride3 + (dim2-1) * stride2 + (dim1-1) * stride1 + (dim0-1) * stride0 + 1) * 4;
        xthal_dcache_region_writeback_inv((void*)buffer, buffer_size); /* use writeback + invalidate to accomodate full-layer reference */
#endif
#endif
        min_val = ptr[0];
        max_val = ptr[0];
        for (unsigned i3=0; i3<dim3; i3++) {
            for (unsigned i2=0; i2<dim2; i2++) {
                for (unsigned i1=0; i1<dim1; i1++) {
                    for (unsigned i0=0; i0<dim0; i0++) {
                        val = ptr[i3*stride3 + i2*stride2 + i1*stride1 + i0*stride0];
                        if (val == 0) {
                            zeros++;
                        }
                        if (val < min_val) {
                            min_val = val;
                        }
                        if (val > max_val) {
                            max_val = val;
                        }
                    }
                }
            }
        }
        printf("[STATS] %s, dims: %dx%dx%dx%d, total: %d, min_val: %f, max_val: %f, zeros: %d, zero ratio: %.2f\n",
                    name, dim0, dim1, dim2, dim3, total, min_val, max_val, zeros, (float)zeros/total);
#ifdef __XCC__
#if XCHAL_DCACHE_SIZE
        xthal_dcache_region_writeback_inv((void*)buffer, buffer_size);
#endif
#endif
    } else {
        printf("[STATS] %s, pinned buffer\n", name);
    }
}

// Target independent PRNG.
static int32_t debug_random() {
  static uint64_t next = 1;
  next = next * 6364136223846793005 + 1442695040888963407;
  return (int32_t)(next % (1 << 31));
}

uint8_t random_U8()
{
    return debug_random() % 256;
}

int8_t random_S8()
{
    return (debug_random() % 256) - 128;
}

uint16_t random_U16()
{
    return debug_random() % 65536;
}

int16_t random_S16()
{
    return (debug_random() % 65536) - 32768;
}

uint32_t random_U32()
{
    return (uint32_t)debug_random() + (uint32_t)debug_random() + (uint32_t)debug_random();
}

int32_t random_S32()
{
    return (int64_t)random_U32() - 32768;
}

float random_F32()
{
    return (debug_random() / (float)(1 << 31)) * 1e+37f;
}

void randomize_array(void *dst, size_t size)
{
    uint8_t* ptr = (uint8_t*) dst;
    int i;
    for (i = 0; i < size; i++) {
        ptr[i] = random_U8();
    }
}

/*
 * debug_jam - Jam inputs into outputs to simulate custom layers.
 *
 * NOTE: Assuming float custom layers for now
 */

void debug_jam(const xi_array *in, int copy_first, xi_array *out)
{
    float *f_in;
    float *f_out;
    int size_in;
    int size_out;
    int max_size;

    f_in = (float*)XI_ARRAY_GET_BUFF_PTR(in);
    f_out = (float*)XI_ARRAY_GET_BUFF_PTR(out);
    size_in = XI_ARRAY_GET_BUFF_SIZE(in);
    size_out = XI_ARRAY_GET_BUFF_SIZE(out);
    size_in /= sizeof(float);
    size_out /= sizeof(float);

    if (copy_first) {
        for (int i = 0; i<size_out; i++) {
            f_out[i] = f_in[i % size_in];
        }
        if (size_in <= size_out)
            return;
        f_in += size_out;
        size_in -= size_out;
    }
    max_size = max(size_in, size_out);
    for (int i = 0; i<max_size; i++) {
        f_out[i % size_out] += f_in[i % size_in];
    }
}

const char* xiErrStr(XI_ERR_TYPE code)
{
    switch(code)
    {
    case XI_ERR_OK:             return "No error";
    case XI_ERR_IALIGNMENT:     return "Input alignment requirements are not satisfied";
    case XI_ERR_OALIGNMENT:     return "Output alignment requirements are not satisfied";
    case XI_ERR_MALIGNMENT:     return "Same modulo alignment requirement is not satisfied";
    case XI_ERR_BADARG:         return "Function arguments are somehow invalid";
    case XI_ERR_MEMLOCAL:       return "Tile is not placed in local memory";
    case XI_ERR_INPLACE:        return "Inplace operation is not supported";
    case XI_ERR_EDGE:           return "Edge extension size is too small";
    case XI_ERR_DATASIZE:       return "Input/output tile size is too small or too big or otherwise inconsistent";
    case XI_ERR_TMPSIZE:        return "Temporary tile size is too small or otherwise inconsistent";
    case XI_ERR_KSIZE:          return "Filer kernel size is not supported";
    case XI_ERR_NORM:           return "Invalid normalization divisor or shift value";
    case XI_ERR_COORD:          return "Tile coordinates are invalid";
    case XI_ERR_BADTRANSFORM:   return "Transform is singular or otherwise invalid";
    case XI_ERR_NULLARG:        return "One of required arguments is NULL";
    case XI_ERR_THRESH_INVALID: return "Threshold value is somehow invalid";
    case XI_ERR_SCALE:          return "Provided scale factor is not supported";
    case XI_ERR_OVERFLOW:       return "Tile size can lead to sum overflow";
    case XI_ERR_NOTIMPLEMENTED: return "The requested functionality is absent in current version of XI Library";
    case XI_ERR_CHANNEL_INVALID:return "Channel number is somehow invalid";
    case XI_ERR_DATATYPE:       return "Argument has invalid data type";
    case XI_ERR_NO_VARIANT:     return "No suitable variant of the function is available";

    case XI_ERR_POOR_DECOMPOSITION: return "Computed transform decomposition can produce visual artifacts";
    case XI_ERR_OUTOFTILE:      return "The arguments or results are out of tile";
    case XI_ERR_OBJECTLOST:     return "Tracked object is lost";
    case XI_ERR_RANSAC_NOTFOUND:return "Unable to find an appropriate model for RANSAC";
    case XI_ERR_REPLAY:         return "Repeated function call is required for completion";

    };
    return "Unknown error";
}

const char *cnnrt_err_str(XI_ERR_TYPE error_code)
{
    switch (error_code) {
    case XI_ERR_SBLK_COMPARE:
        return "SBLK comparison error";
    default:
        return xiErrStr(error_code);
    }
}
