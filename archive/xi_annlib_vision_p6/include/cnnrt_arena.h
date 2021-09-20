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
#ifndef _CNNRT_ARENA_H_INCLUDED_
#define _CNNRT_ARENA_H_INCLUDED_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
   Local memory arena API.

   Typical use pattern of the arena API is:

   // Initalize local memory manager.
   arena_init(mem_info);
   ....
   // Reserve memory
   arena_static_alloc(...);
   ....
   // For computational stages:
       // Initialize buffer management mode
       arena_init_two_banks_split(...)
       ....
       // Reserve buffer
       arena_alloc(...)
       ....
       // Reset buffer management mode
       arena_reset()
*/

/* Arena initialization.

   This function takes control over memory described by mem_info
   and provides access to it through arena_ API.

   WARNING: arena_init() is called from cnnrt_init() and
   shouldn't be used along with cnnrt_init().
*/
XI_ERR_TYPE arena_init(const local_mem_info_t *mem_info);

/* Arena de-initialization */
void arena_deinit();

/* Static memory allocator.

   Allows to reserve memory in the lowest bank after arena_init().

   This memory is not available to arena_init_*() functions and stays
   reseved until next arena_init() is called.

   On success returns XI_ERR_OK and pointer to allocated block in *dest.
*/
XI_ERR_TYPE arena_static_alloc(void ** dest, size_t size, size_t alignment);

/* Arena allocation mode initialization. */

XI_ERR_TYPE arena_init_two_banks_split(size_t bank0_space_allocated,
                                       size_t bank0_max_alignment,
                                       size_t bank1_space_allocated,
                                       size_t bank1_max_alignment);

XI_ERR_TYPE arena_init_two_banks_contiguous(size_t space_allocated,
                                            size_t max_alignment);

XI_ERR_TYPE arena_init_one_bank(size_t bank0_space_allocated,
                                size_t bank0_max_alignment);

XI_ERR_TYPE arena_alloc(void **dest, int bank, size_t size, size_t alignment);
XI_ERR_TYPE arena_reset();

/*
  Query current arena allocator configuration.
*/

/* Returns number of memory regions in arena. */
uint32_t arena_num_banks();

/* Returns non-zero if all memory regions located next to each other
   in address space without gaps in between. Returns zero if
   it's not the case. */
int arena_contiguous_banks();

/* On success returns XI_ERR_OK and fills out *free_space with number of
   bytes available for allocation in memory region. bank_num is a
   zero-based index of memory region. */
XI_ERR_TYPE arena_bank_free_space(uint32_t bank_num, uint32_t *free_space);
XI_ERR_TYPE arena_bank_free_space_debug(uint32_t bank_num, uint32_t *free_space);

void * arena_pinned_buffer(uint32_t bank_num, uint32_t alignment);

#ifdef __cplusplus
}
#endif

#endif /* _CNNRT_ARENA_H_INCLUDED_ */

