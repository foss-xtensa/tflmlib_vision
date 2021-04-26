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
#ifndef XI_LIBRARY_VERSION

#define XI_LIBRARY_VERSION_MAJOR    7
#define XI_LIBRARY_VERSION_MINOR    13
#define XI_LIBRARY_VERSION_PATCH    0

#define XI_MAKE_LIBRARY_VERSION(major, minor, patch) ((major)*100000 + (minor)*1000 + (patch)*10)
#define XI_AUX_STR_EXP(__A) #__A
#define XI_AUX_STR(__A) XI_AUX_STR_EXP(__A)

#define XI_LIBRARY_VERSION     (XI_MAKE_LIBRARY_VERSION(XI_LIBRARY_VERSION_MAJOR, XI_LIBRARY_VERSION_MINOR, XI_LIBRARY_VERSION_PATCH))
#define XI_LIBRARY_VERSION_STR XI_AUX_STR(XI_LIBRARY_VERSION_MAJOR) "." XI_AUX_STR(XI_LIBRARY_VERSION_MINOR) "." XI_AUX_STR(XI_LIBRARY_VERSION_PATCH)

#endif
