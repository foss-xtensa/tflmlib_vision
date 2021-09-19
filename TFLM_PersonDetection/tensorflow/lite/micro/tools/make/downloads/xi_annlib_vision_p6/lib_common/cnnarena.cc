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
#include "xi_platform.h"

/**************************************
 *
 *  Memory arena management functions
 *
 **************************************/

#if CSTUB_BUILD

#include <malloc.h>

/* CSTUBs build

   Allocating all buffers using malloc from heap.
   This helps to catch memory overrun errors.
*/

typedef struct _mem_buffer {
    void               *ptr;
    /* Padding size at the beginning allocated buffer */
    size_t              pad_head;
    /* Padding size at the end of allocated buffer */
    size_t              pad_tail;
    struct _mem_buffer *next;
    char               *start;
    char               *end;
} mem_buffer_t;

/* Memory fill value */
#define MEM_PADDING_VALUE 0xDB

static int arena_buffers_intersect(mem_buffer_t *p1, mem_buffer_t *p2)
{
    if (p1->start < p2->start) {
        return p1->end > p2->start;
    } else if (p1->start > p2->start) {
        return p2->end > p1->start;
    }
    return 1;
}

#endif // CSTUB_BUILD


/*
 * Memory bank state descriptor.
 */

typedef struct {
    /* Memory bank start address. This is beginning of free space
       in local memory that doesn't change. */
    char *start;
    /* Pointer at first invalid byte after bank free space.
       (end - start) is the memory bank size. */
    char *end;
    /* Adjusted start address. This is the start of free space after
       arena_static_alloc() performed. */
    char *base;
    /* First free byte in the bank. This pointer is adjusted by
       memory allocation functions. */
    char *pos;
    /* Straddling flag: indicates if last allocation is allowed to
       overflow into next memory bank. */
    char  straddling_possible;
    /* Xtensa DRAM index for printing. Corresponds to <N> in
       .dram<N>.data section name. */
    char  dram_index;
#if CSTUB_BUILD
    /* Debug and mem management helper structures in CSTUB build. */
    mem_buffer_t *cstub_buffers;
    /* First allocated buffer during previous arena_init/arena_reset round
       to carry over memory state between rounds. If user requests same
       first alloc() in the bank at the next init/reset round then this
       buffer is returned. */
    mem_buffer_t *cstub_saved;
#endif
} arena_bank_t;

typedef struct {
    /* Number of memory banks */
    int8_t banks_count;
    /* Current allocation mode: two bank contiguous or separate banks. */
    int8_t two_banks_contiguous;
    /* Individual memory bank state */
    arena_bank_t banks[];
} arena_state_t;

/*
 * Current arena global pointer
 */

static arena_state_t *_arena_state _LOCAL_RAM_ = NULL;

INLINE char * alignup(char *ptr, unsigned alignment)
{
    uintptr_t new_addr = (uintptr_t)ptr + alignment - 1;
    return (char *)(new_addr - (new_addr & (alignment - 1)));
}

XI_ERR_TYPE arena_init(const local_mem_info_t *mem_info)
{
    XI_ERROR_CHECKS() {
        XI_RUN_TIME_CHECK(mem_info,
                          "No memory information provided", XI_ERR_NULLARG);
        XI_RUN_TIME_CHECK(mem_info->banks_count == 1 || mem_info->banks_count == 2,
                          "Invalid number of memory banks, only 1 and 2 are supported", XI_ERR_BADARG);
    }

    arena_deinit();

    /* Order memory banks by ascending start address */
    char *addr0 = NULL;
    char *addr1 = NULL;
    size_t size0 = 0;
    size_t size1 = 0;
    int index0 = 0;
    int index1 = -1;

    if (mem_info->banks_count == 2) {
        if (mem_info->bank[0].start < mem_info->bank[1].start) {
            index0 = 0;
            index1 = 1;
        } else {
            index0 = 1;
            index1 = 0;
        }
    }

    addr0 = (char *) mem_info->bank[index0].start;
    size0 = mem_info->bank[index0].size;
    if (index1 >= 0) {
        addr1 = (char *) mem_info->bank[index1].start;
        size1 = mem_info->bank[index1].size;
    }

    /* Reserve memory for control structures in the lowest bank */
    size_t ctrl_size = sizeof(arena_state_t) + sizeof(arena_bank_t) * mem_info->banks_count;
    /* Allocate control structure in the lowest bank */
    arena_state_t *ctrl = (arena_state_t *) alignup(addr0, sizeof(int));
    size_t ctrl_alloc = ((char*)ctrl - addr0) + ctrl_size;

    XI_RUN_TIME_CHECK(size0 > ctrl_alloc,
                      "Not enough memory to allocate control data structures", XI_ERR_BADARG);

    size0 -= ctrl_alloc;
    addr0 += ctrl_alloc;

#if CSTUB_BUILD
    ctrl = (arena_state_t *) malloc(ctrl_size);
    XI_RUN_TIME_CHECK(ctrl != NULL, "Out of memory", XI_ERR_BADARG);
#endif // CSTUB_BUILD

    memset(ctrl, 0, ctrl_size);

    ctrl->banks_count = mem_info->banks_count;
    ctrl->banks[0].start = addr0;
    ctrl->banks[0].end = addr0 + size0;
    ctrl->banks[0].base = addr0;
    ctrl->banks[0].pos = addr0;
    ctrl->banks[0].dram_index = index0;

    if (index1 >= 0) {
        ctrl->banks[1].start = addr1;
        ctrl->banks[1].end = addr1 + size1;
        ctrl->banks[1].base = addr1;
        ctrl->banks[1].pos = addr1;
        ctrl->banks[1].dram_index = index1;
    }

    _arena_state = ctrl;

    return XI_ERROR_STATUS();
}


void arena_deinit()
{
    if (_arena_state != NULL) {
        arena_reset();
#if CSTUB_BUILD
        /* Free saved memory states */
        for (int i=0; i < _arena_state->banks_count; i++) {
            if (_arena_state->banks[i].cstub_saved) {
#ifndef _MSC_VER
                free(_arena_state->banks[i].cstub_saved->ptr);
#else
                _aligned_free(_arena_state->banks[i].cstub_saved->ptr);
#endif
                
                free(_arena_state->banks[i].cstub_saved);
            }
        }
        free(_arena_state);
#endif
        _arena_state = NULL;
    }
}

INLINE XI_ERR_TYPE arena_check_space_allocated(int bank, size_t space_allocated, size_t max_alignment)
{
	//printf("arena_check_space_allocated(): bank=%d, space=%d, alignment=%d vs base=0x%x (aligned 0x%x), end=0x%x\n", bank, space_allocated, max_alignment,
	//		(int) _arena_state->banks[bank].base, (int)alignup(_arena_state->banks[bank].base, max_alignment), (int)_arena_state->banks[bank].end);
    XI_ERROR_CHECKS() {
        XI_RUN_TIME_CHECK(_arena_state != NULL, "Not initialized", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(alignup(_arena_state->banks[bank].base, max_alignment) + space_allocated <= _arena_state->banks[bank].end,
                          "Not enough space to allocate", XI_ERR_BADARG);
    }
    return XI_ERROR_STATUS();
}

INLINE XI_ERR_TYPE arena_adjust_bank_start(int bank, size_t max_alignment)
{
    XI_ERROR_CHECKS() {
        XI_RUN_TIME_CHECK(_arena_state != NULL, "Not initialized", XI_ERR_BADARG);
    }
    _arena_state->banks[bank].pos = alignup(_arena_state->banks[bank].pos, max_alignment);
    return XI_ERROR_STATUS();
}

XI_ERR_TYPE arena_init_two_banks_split(size_t bank0_space_allocated, size_t bank0_max_alignment, size_t bank1_space_allocated, size_t bank1_max_alignment)
{
    XI_ERROR_CHECKS() {
        XI_RUN_TIME_CHECK(_arena_state != NULL, "Not initialized", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(_arena_state->banks_count == 2,
                          "Two local memory banks requested but not supported by hardware", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(bank0_max_alignment > 0 && (bank0_max_alignment & (bank0_max_alignment - 1)) == 0,
                          "Bank 0 alignment must be power of two", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(bank1_max_alignment > 0 && (bank1_max_alignment & (bank1_max_alignment - 1)) == 0,
                          "Bank 1 alignment must be power of two", XI_ERR_BADARG);
    }
    XI_CHECK_RESULT(arena_reset());
    XI_CHECK_RESULT(arena_check_space_allocated(0, bank0_space_allocated, bank0_max_alignment));
    XI_CHECK_RESULT(arena_check_space_allocated(1, bank1_space_allocated, bank1_max_alignment));
    XI_CHECK_RESULT(arena_adjust_bank_start(0, bank0_max_alignment));
    XI_CHECK_RESULT(arena_adjust_bank_start(1, bank1_max_alignment));
    _arena_state->banks[0].straddling_possible = 0;
    _arena_state->banks[1].straddling_possible = 0;
    return XI_ERROR_STATUS();
}

XI_ERR_TYPE arena_init_two_banks_contiguous(size_t space_allocated, size_t max_alignment)
{
    XI_ERROR_CHECKS() {
        XI_RUN_TIME_CHECK(_arena_state != NULL, "Not initialized", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(_arena_state->banks_count == 2,
                          "Two local memory banks requested but not supported by hardware", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(max_alignment > 0 && (max_alignment & (max_alignment - 1)) == 0,
                          "Alignment must be power of two", XI_ERR_BADARG);
    }

    XI_CHECK_RESULT(arena_reset());

    /* check that we can stradde allocations from bank0 to bank1 */
    XI_RUN_TIME_CHECK(_arena_state->banks[0].end == _arena_state->banks[1].start,
                      "Two local memory banks are not contiguous in memory", XI_ERR_BADARG);
    /* check that the whole allocated size fits in both banks */
    XI_RUN_TIME_CHECK(alignup(_arena_state->banks[0].start, max_alignment) + space_allocated <= _arena_state->banks[1].end,
                      "Not enough space to allocate", XI_ERR_BADARG);
    /* do the adjustment of bank0 start */
    XI_CHECK_RESULT(arena_adjust_bank_start(0, max_alignment));

    _arena_state->two_banks_contiguous = 1;

    _arena_state->banks[0].straddling_possible = 1;
    _arena_state->banks[1].straddling_possible = 0;

    return XI_ERROR_STATUS();
}

XI_ERR_TYPE arena_init_one_bank(size_t bank0_space_allocated, size_t bank0_max_alignment)
{
    XI_ERROR_CHECKS() {
        XI_RUN_TIME_CHECK(_arena_state != NULL, "Not initialized", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(bank0_max_alignment > 0 && (bank0_max_alignment & (bank0_max_alignment - 1)) == 0,
                          "Alignment must be power of two", XI_ERR_BADARG);
    }
    XI_CHECK_RESULT(arena_reset());
    XI_CHECK_RESULT(arena_check_space_allocated(0, bank0_space_allocated, bank0_max_alignment));
    XI_CHECK_RESULT(arena_adjust_bank_start(0, bank0_max_alignment));
    _arena_state->banks[0].straddling_possible = 0;
    return XI_ERROR_STATUS();
}


XI_ERR_TYPE arena_alloc(void **dest, int bank, size_t size, size_t alignment)
{
    XI_ERROR_CHECKS() {
        XI_RUN_TIME_CHECK(_arena_state != NULL, "Not initialized", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(bank < _arena_state->banks_count, "Invalid bank index", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(size > 0, "Invalid allocation size", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(((alignment > 0) && !(alignment & (alignment - 1))),
                          "Invalid allocation alignment", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(dest != NULL,
                          "Pointer to write memory allocation result is NULL.", XI_ERR_NULLARG)
    }
    if (bank < 0) {
        /* Find first bank that can fit requested buffer */
        for (int i = 0; i < _arena_state->banks_count; i++) {
            if (_arena_state->banks[i].pos < _arena_state->banks[i].end) {
                char *start = alignup(_arena_state->banks[i].pos, alignment);
                if (start + size <= _arena_state->banks[i].end
                    || (_arena_state->two_banks_contiguous
                        && (start + size) > _arena_state->banks[i].end
                        && _arena_state->banks[i].straddling_possible
                        && _arena_state->banks[i + 1].pos == _arena_state->banks[i + 1].start))
                {
                    /* Found free space in a bank */
                    bank = i;
                    break;
                }
            }
        }
        XI_RUN_TIME_CHECK(bank >= 0,
                          "Cannot find free space for requested memory allocation", XI_ERR_BADARG);
#if XI_ERROR_LEVEL == XI_ERROR_LEVEL_NO_ERROR
        /* Always check for memory allocation errors */
        if (bank < 0) {
            return XI_ERR_BADARG;
        }
#endif
    }
    char *start = alignup(_arena_state->banks[bank].pos, alignment);
    char *pos = start;
    if (_arena_state->two_banks_contiguous
        && (pos + size) > _arena_state->banks[bank].end
        && _arena_state->banks[bank].straddling_possible)
    {
        _arena_state->banks[bank].straddling_possible = 0;
        XI_RUN_TIME_CHECK(_arena_state->banks[bank + 1].pos == _arena_state->banks[bank].end,
                          "Allocation requested cannot overflow into next bank because of gap "
                          "between memories or incorrect allocation order", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK((pos + size) <= _arena_state->banks[bank + 1].end,
                          "Allocation requested goes past the end of the bank", XI_ERR_BADARG);
#if XI_ERROR_LEVEL == XI_ERROR_LEVEL_NO_ERROR
        /* Always check for memory allocation errors */
        if (_arena_state->banks[bank + 1].pos != _arena_state->banks[bank].end
            || pos + size > _arena_state->banks[bank + 1].end) {
            return XI_ERR_BADARG;
        }
#endif
        _arena_state->banks[bank].pos = _arena_state->banks[bank].end;
        _arena_state->banks[bank + 1].pos = pos + size;
    } else {
        XI_RUN_TIME_CHECK((pos + size) <= _arena_state->banks[bank].end,
                          "Allocation requested goes past the end of the bank", XI_ERR_BADARG);
#if XI_ERROR_LEVEL == XI_ERROR_LEVEL_NO_ERROR
        /* Always check for memory allocation errors */
        if (pos + size > _arena_state->banks[bank].end) {
            return XI_ERR_BADARG;
        }
#endif
        _arena_state->banks[bank].pos = pos + size;
    }
#if CSTUB_BUILD
    {
        mem_buffer_t *entry = _arena_state->banks[bank].cstub_saved;
        /* If this is the first allocation in the bank and its parameters are the same as
           on previos alooc/reset round then return existing buffer saved from previos
           round. This way memory state is preserved across alloc/reset rounds. */
#ifdef SAVE_FIRST_ALLOC
        if (entry
            && entry->start == pos
            && entry->end == pos + size
            && _arena_state->banks[bank].cstub_buffers == NULL)
        {
            pos = (char *)entry->ptr + entry->pad_head;
            _arena_state->banks[bank].cstub_buffers = entry;
            _arena_state->banks[bank].cstub_saved = NULL;
        }
        else
#endif
        {
            entry = (mem_buffer_t *)malloc(sizeof(*entry));
            if (entry) {
                mem_buffer_t* tmp;

                /* Adding XCHAL_DATA_WIDTH bytes around to make sure valid load/stores
                   from those areas by simulated memory operations do not trigger errors
                   in valgrind and electric fence. */
                size_t ls_alignment = alignment > XCHAL_DATA_WIDTH ? alignment : XCHAL_DATA_WIDTH;
                size_t ls_size, ls_offset;
                if (alignment < XCHAL_DATA_WIDTH) {
                    /* Align size up to a multiple of XCHAL_DATA_WIDTH and make sure there is always padding
                       to the next multiple of XCHAL_DATA_WIDTH.

                       For example below _ is padding and * is data. Assuming required
                       alignment is 2 and XCHAL_DATA_WIDTH is 8 the buffer is aligned to 2
                       but padded to higher alignement of 8.
                       1. |______**|********|*****___|________|
                       2. If end of buffer is on alignemnt boundary then extra padding is added:
                       |______**|********|________|
                       3. Alignemnt greater or equal to 8:
                       |********|********|**______|________|
                    */
                    ls_offset = XCHAL_DATA_WIDTH - alignment;

                } else {
                    ls_offset = 0;
                }
                ls_size = (size_t)alignup((char*)(ls_offset + size + XCHAL_DATA_WIDTH), XCHAL_DATA_WIDTH);
                /* Allocate extra 64Kb to cover Gather/Scatter accesses that may happen in CSTUBs even
                   with byte-disables. */
                ls_size += 65536;
#ifndef _MSC_VER
                entry->ptr = memalign(ls_alignment, ls_size);
#else
                entry->ptr = _aligned_malloc(ls_size, ls_alignment);
#endif
                entry->next = _arena_state->banks[bank].cstub_buffers;
                entry->pad_head = ls_offset;
                entry->pad_tail = ls_size - size - ls_offset;
                entry->start = pos;
                entry->end   = pos + size;
                tmp = _arena_state->banks[bank].cstub_buffers;
                while (tmp) {
                    if (arena_buffers_intersect(tmp, entry))
                        {
#ifndef _MSC_VER
                            free(entry->ptr);
#else
                            _aligned_free(entry->ptr);
#endif
                            free(entry);
                            XI_RUN_TIME_CHECK(0, "Allocated buffers intersect", XI_ERR_BADARG);
                            return XI_ERR_BADARG;
                        }
                    tmp = tmp->next;
                }
                _arena_state->banks[bank].cstub_buffers = entry;
                pos = (char *)entry->ptr + ls_offset;
                /* Initialize memory around allocated buffer */
                if (entry->pad_head > 0)
                    memset(entry->ptr, MEM_PADDING_VALUE, entry->pad_head);
                if (entry->pad_tail > 0)
                    memset(pos + size, MEM_PADDING_VALUE, entry->pad_tail);
            } else {
                pos = NULL;
            }
        }
    }
    XI_RUN_TIME_CHECK(pos != NULL, "Dynamic memory allocation failed", XI_ERR_BADARG);
#endif // CSTUB_BUILD
    *dest = (void*)pos;
    return XI_ERROR_STATUS();
}

XI_ERR_TYPE arena_static_alloc(void ** dest, size_t size, size_t alignment)
{
    XI_ERROR_CHECKS() {
        XI_RUN_TIME_CHECK(_arena_state != NULL, "Not initialized", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(alignment > 0 && (alignment & (alignment - 1)) == 0,
                          "Alignment must be power of two", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(size > 0, "Invalid allocation size", XI_ERR_BADARG);
        XI_RUN_TIME_CHECK(dest != NULL,
                          "Pointer to write memory allocation result is NULL.", XI_ERR_NULLARG)
    }
    char *start = alignup(_arena_state->banks[0].base, alignment);
    char *end = start + size;

#if CNNRT_PERF_LEVEL != CNNRT_PERF_LEVEL_NONE
    printf("Permanently reserving 0x%x bytes in DRAM%d, new start address: 0x%x\n",
           end - _arena_state->banks[0].base, (int)_arena_state->banks[0].dram_index, (uintptr_t)end);
#endif

    XI_RUN_TIME_CHECK(end <= _arena_state->banks[0].end,
                      "Not enough memory for static allocation", XI_ERR_BADARG);
    /* Adjust base in the lowest bank, this is permanent allocation. */
    _arena_state->banks[0].base = end;
#if CSTUB_BUILD
#ifndef _MSC_VER
    start = (char*)memalign(alignment, size);
#else
    start = (char*)_aligned_malloc(size, alignment);
#endif
#endif // CSTUB_BUILD
    *dest = (void*)start;

    return XI_ERROR_STATUS();
}

XI_ERR_TYPE arena_reset()
{
    XI_ERROR_CHECKS() {
        XI_RUN_TIME_CHECK(_arena_state != NULL, "Not initialized", XI_ERR_BADARG);
    }
    if (_arena_state != NULL) {
#if CSTUB_BUILD
        for (int i=0; i<_arena_state->banks_count; i++) {
            while (_arena_state->banks[i].cstub_buffers) {
                mem_buffer_t *entry = _arena_state->banks[i].cstub_buffers;
                _arena_state->banks[i].cstub_buffers = entry->next;
                /* Check memory around allocated buffer */
                for (size_t i=0; i<entry->pad_head; i++) {
                    XI_RUN_TIME_CHECK(((uint8_t*)entry->ptr)[i] == MEM_PADDING_VALUE,
                                      "Memory overrun detected", XI_ERR_BADARG);
                }
                for (size_t i=0; i<entry->pad_tail; i++) {
                    uint8_t* ptr = (uint8_t*)entry->ptr + entry->pad_head + (entry->end - entry->start);
                    XI_RUN_TIME_CHECK(ptr[i] == MEM_PADDING_VALUE,
                                      "Memory overrun detected", XI_ERR_BADARG);
                }
#ifdef SAVE_FIRST_ALLOC
                if (_arena_state->banks[i].cstub_buffers == NULL) {
                    /* Save first alloc() entry for the next round */
                    mem_buffer_t *tmp = _arena_state->banks[i].cstub_saved;
                    _arena_state->banks[i].cstub_saved = entry;
                    entry = tmp;
                }
#endif
                if (entry) {
#ifndef _MSC_VER
                    free(entry->ptr);
#else
                  _aligned_free(entry->ptr);
#endif
                    free(entry);
                }
            }
        }
#endif // CSTUB_BUILD
        _arena_state->two_banks_contiguous = 0;
        for (int i=0; i < _arena_state->banks_count; i++) {
            _arena_state->banks[i].pos = _arena_state->banks[i].base;
        }
    }
    return XI_ERROR_STATUS();
}

uint32_t arena_num_banks()
{
    if (_arena_state == NULL)
        return 0;
    return _arena_state->banks_count;
}

int arena_contiguous_banks()
{
    int is_contiguous = 1;
    if (_arena_state == NULL)
        return 0;
    for (int i = 1; i < _arena_state->banks_count; ++i) {
        if ((uint32_t)_arena_state->banks[i-1].end != (uint32_t)_arena_state->banks[i].start) {
            is_contiguous = 0;
            break;
        }
    }
    return is_contiguous;
}

XI_ERR_TYPE arena_bank_free_space(uint32_t bank_num, uint32_t *free_space)
{
    XI_RUN_TIME_CHECK(_arena_state != NULL, "Not initialized", XI_ERR_BADARG);
    XI_RUN_TIME_CHECK(free_space != NULL, "free_space argument cannot be NULL", XI_ERR_NULLARG);
    XI_RUN_TIME_CHECK(bank_num < _arena_state->banks_count, "Invalid bank num", XI_ERR_BADARG);
    *free_space = (uint32_t)_arena_state->banks[bank_num].end -
        (uint32_t)_arena_state->banks[bank_num].base;
    return XI_ERR_OK;
}

XI_ERR_TYPE arena_bank_free_space_debug(uint32_t bank_num, uint32_t *free_space)
{
    XI_RUN_TIME_CHECK(_arena_state != NULL, "Not initialized", XI_ERR_BADARG);
    XI_RUN_TIME_CHECK(free_space != NULL, "free_space argument cannot be NULL", XI_ERR_NULLARG);
    XI_RUN_TIME_CHECK(bank_num < _arena_state->banks_count, "Invalid bank num", XI_ERR_BADARG);
    *free_space = (uint32_t)_arena_state->banks[bank_num].end -
        (uint32_t)_arena_state->banks[bank_num].pos;
    return XI_ERR_OK;
}

void * arena_pinned_buffer(uint32_t bank_num, uint32_t alignment)
{
    if (_arena_state == NULL || bank_num >= (uint32_t)_arena_state->banks_count) {
        return NULL;
    }
#if CSTUB_BUILD
    /* See if arena was reset or not */
    if (_arena_state->banks[bank_num].cstub_buffers) {
        /* Return first allocated buffer */
        return alignup((char *)_arena_state->banks[bank_num].cstub_buffers->ptr, alignment);
    } else if (_arena_state->banks[bank_num].cstub_saved) {
        /* Lookup in saved after reset */
        return alignup((char *)_arena_state->banks[bank_num].cstub_saved->ptr, alignment);
    } else {
        /* No buffers pinned */
        return NULL;
    }
#else
    return alignup(_arena_state->banks[bank_num].base, alignment);
#endif
}
