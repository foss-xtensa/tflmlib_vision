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
#ifndef XI_CNN_LIBRARY_VERSION

#include "xi_version.h"
#include "xi_config_api.h"

#if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))

#define XI_CNN_LIBRARY_VERSION_MAJOR                1
#define XI_CNN_LIBRARY_VERSION_MINOR                1
#define XI_CNN_LIBRARY_VERSION_PATCH                6
#define XI_CNN_LIBRARY_VERSION_INTERNAL_TRACKING    1

#define XI_CNN_LIBRARY_VERSION     (XI_MAKE_LIBRARY_VERSION(XI_CNN_LIBRARY_VERSION_MAJOR, XI_CNN_LIBRARY_VERSION_MINOR, XI_CNN_LIBRARY_VERSION_PATCH, XI_CNN_LIBRARY_VERSION_INTERNAL_TRACKING))
#define XI_CNN_LIBRARY_VERSION_STR "A." XI_AUX_STR(XI_CNN_LIBRARY_VERSION_MAJOR) "." XI_AUX_STR(XI_CNN_LIBRARY_VERSION_MINOR) "." XI_AUX_STR(XI_CNN_LIBRARY_VERSION_PATCH) "." XI_AUX_STR(XI_CNN_LIBRARY_VERSION_INTERNAL_TRACKING)

#endif /*  */

#endif //if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))
