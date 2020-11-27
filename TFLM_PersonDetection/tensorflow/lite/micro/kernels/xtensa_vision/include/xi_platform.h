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
#ifndef _XI_PLATFORM_INCLUDED_
#define _XI_PLATFORM_INCLUDED_

#define PORT_TO_P1 0 // set to 1 for compiling code for P1, else 0

#if (!PORT_TO_P1)
#ifndef XCHAL_IVPN_SIMD_WIDTH
#define XCHAL_IVPN_SIMD_WIDTH 32
#endif
#else
#undef XCHAL_IVPN_SIMD_WIDTH
#define XCHAL_IVPN_SIMD_WIDTH 8
#endif

#undef XCHAL_VISION_TYPE
#define XCHAL_VISION_TYPE 6
#undef XCHAL_HAVE_VISION
#define XCHAL_HAVE_VISION 1

#endif
