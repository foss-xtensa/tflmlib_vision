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
#include <stdlib.h>
#include <stdint.h>
#include "cnnrt.h"
#include <string.h>

#include <xtensa/config/core-isa.h>

/******************************************************
 *
 *  Local memory configuration discovery from Linker
 *  Support Package (LSP) and Core Configuration.
 *
 ******************************************************/

#if !CSTUB_BUILD

/* Symbols provided by LSP */

/* Start of free space in DRAM0 (end of statically allocated data) */
extern char _memmap_seg_dram0_0_end;
/* Max DRAM0 address */
extern char _memmap_seg_dram0_0_max;

#  if XCHAL_NUM_DATARAM>1
extern char _memmap_seg_dram1_0_end;
extern char _memmap_seg_dram1_0_max;
#  endif

// extern void __stack; TODO : need to find stack start pointer in xt-clang case
#define TEMP_STACK_FIX 1 // Temporary fix : adjust available memory to account for stack with workaround way

#endif // CSTUB_BUILD


XI_ERR_TYPE cnnrt_get_local_mem_info(const size_t stack_size, local_mem_info_t *mem_info)
{
    XI_ERROR_CHECKS() {
        //XI_ERR_NULLARG
#if !CSTUB_BUILD
        /* Check if LSP symbols are consistent with core headers */
        XI_RUN_TIME_CHECK((char*)XCHAL_DATARAM0_PADDR <= (char*)&_memmap_seg_dram0_0_end
                          && (char*)XCHAL_DATARAM0_PADDR + XCHAL_DATARAM0_SIZE >= (char*)&_memmap_seg_dram0_0_max,
                          "Location of DRAM0 and LSP symbols do not match. Check if LSP is correct.", XI_ERR_BADARG);
#  if XCHAL_NUM_DATARAM>1
        XI_RUN_TIME_CHECK((char*)XCHAL_DATARAM1_PADDR <= (char*)&_memmap_seg_dram1_0_end
                          && (char*)XCHAL_DATARAM1_PADDR + XCHAL_DATARAM1_SIZE >= (char*)&_memmap_seg_dram1_0_max,
                          "Location of DRAM1 and LSP symbols do not match. Check if LSP is correct.", XI_ERR_BADARG);
#  endif
#endif
    }

    memset(mem_info, 0, sizeof(*mem_info));

    /* Getting info about free space in each local memory from LSP symbols.
       If it's CSTUBs build then assume that all memory is available. */
#if XCHAL_NUM_DATARAM>0 && XCHAL_DATARAM0_HAVE_IDMA
#  if !CSTUB_BUILD
    mem_info->bank[mem_info->banks_count].start = &_memmap_seg_dram0_0_end;
    mem_info->bank[mem_info->banks_count].size = &_memmap_seg_dram0_0_max - &_memmap_seg_dram0_0_end;
#  else
    mem_info->bank[mem_info->banks_count].start = (void *)XCHAL_DATARAM0_PADDR;
    mem_info->bank[mem_info->banks_count].size = XCHAL_DATARAM0_SIZE;
#  endif
    mem_info->banks_count++;
#endif
#if XCHAL_NUM_DATARAM>1 && XCHAL_DATARAM1_HAVE_IDMA
#  if !CSTUB_BUILD
    mem_info->bank[mem_info->banks_count].start = &_memmap_seg_dram1_0_end;
    mem_info->bank[mem_info->banks_count].size = &_memmap_seg_dram1_0_max - &_memmap_seg_dram1_0_end;
#  else
    mem_info->bank[mem_info->banks_count].start = (void *)XCHAL_DATARAM1_PADDR;
    mem_info->bank[mem_info->banks_count].size = XCHAL_DATARAM1_SIZE;
#  endif
    mem_info->banks_count++;
#endif

    /* Adjust memory bank size to exclude stack if stack is not explicitly reserved in LSP. */
#if !CSTUB_BUILD
    // TODO : make '__stack' work below. Otherwise local stack case won't work
#if (TEMP_STACK_FIX)
    char temp;
    char *stack_end = &temp; // pointer should be on stack
#else
    char *stack_end = (char *)&__stack - stack_size;
#endif
    for (int bank = 0; bank < mem_info->banks_count; bank++) {
        if ((char*)mem_info->bank[bank].start <= stack_end
            && (char*)mem_info->bank[bank].start + mem_info->bank[bank].size > stack_end) {
#if (TEMP_STACK_FIX)
        	mem_info->bank[bank].size -= stack_size;
#else
            mem_info->bank[bank].size = stack_end - (char*)mem_info->bank[bank].start;
#endif
            break;
        }
    }
#endif

    return XI_ERROR_STATUS();
}

void cnnrt_print_local_mem_info(const size_t stack_size, local_mem_info_t *mem_info)
{
#if CNNRT_PERF_LEVEL != CNNRT_PERF_LEVEL_NONE
    printf("Local Memory Start : Size\n----------------------------\n");
    for (int i = 0; i < mem_info->banks_count; i++) {
        printf("DRAM%d: 0x%x : 0x%x\n", i, (uintptr_t)mem_info->bank[i].start,
               mem_info->bank[i].size);
    }
    printf("----------------------------\n");
#if 0 // TODO
#if !CSTUB_BUILD
    char *stack_end = (char *)&__stack - stack_size;
    printf("Stack: 0x%x - 0x%x\n", (uintptr_t)stack_end, (uintptr_t)&__stack);
    printf("Stack Size: %d\n", stack_size);
#endif // !CSTUB_BUILD
#endif
#endif

}
