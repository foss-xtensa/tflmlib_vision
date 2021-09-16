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
#ifndef _CNNRT_HELPERS_H_INCLUDED_
#define _CNNRT_HELPERS_H_INCLUDED_

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
  Internal helper functions.
*/

// required for windows
#undef min
#undef max
static inline int min(int a, int b) { return a < b ? a : b; }
static inline int max(int a, int b) { return a > b ? a : b; }

static inline void swap_buffers(uint8_t** a, uint8_t** b)
{
    uint8_t *t = *a;
    *a = *b;
    *b = t;
}

static inline void rotate_buffers(uint8_t** a, uint8_t** b, uint8_t** c)
{
    uint8_t *t = *c;
    *c = *b;
    *b = *a;
    *a = t;
}

/* Modulo increment into a new variable.
     (*temp) = ((*var) + carry) % bound;
     carry is either 0 or 1.

   Returns overflow flag:
      1 if overflowed
      0 if didn't overflow
*/
static inline int inc_iter_to_temp(int *temp, int var, int bound, int carry)
{
    int new_val = var + carry;
    carry = new_val == bound;
    *temp = carry ? 0 : new_val;
    return carry;
}

/* increment into a new variable with offset.

   Returns overflow flag:
      1 if overflowed
      0 if didn't overflow
*/
static inline int inc_iter_to_temp_with_offset(int *temp, int var, int start, int bound, int carry)
{
    int new_val = var + carry;
    carry = new_val == bound;
    *temp = carry ? start : new_val;
    return carry;
}

/*
   Align m to multiple of n's
*/
static inline unsigned int cnnrt_align_up(unsigned int m, unsigned int n)
{
    return ((m + n - 1) / n) * n;
}

/* Modulo increment by value into a new variable.
     (*temp) = ((*var) + increment) % bound;
     increment can be 0 or positive value.

   Returns an overflow value of next level increment
*/
static inline uint32_t inc_variable_iter(uint32_t *temp, uint32_t var, uint32_t bound, uint32_t increment)
{
    uint32_t new_val = var + increment;
    *temp = new_val % bound;
    uint32_t currLevel = var/bound;
    uint32_t newLevel = new_val/bound;
    return newLevel - currLevel;    // next level increment value
}

#ifdef __cplusplus
}
#endif

#endif /* _CNNRT_HELPERS_H_INCLUDED_ */

