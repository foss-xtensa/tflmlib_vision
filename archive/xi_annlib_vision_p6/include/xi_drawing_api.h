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
#ifndef __XI_DRAWING_API_H__
#define __XI_DRAWING_API_H__

#include "xi_core_api.h"

/* Circle drawing functions */
_XI_API_ XI_ERR_TYPE xiCircle_I8  (xi_pTile src /*inout*/, xi_point32 c, int r, int color, int thickness);
_XI_API_ XI_ERR_TYPE xiCircle_I8Sa(xi_pTile src /*inout*/, xi_point32 c, int r, int color, int thickness);

_XI_API_ XI_ERR_TYPE xiCircle_I16  (xi_pTile src /*inout*/, xi_point32 c, int r, int color, int thickness);
_XI_API_ XI_ERR_TYPE xiCircle_I16Sa(xi_pTile src /*inout*/, xi_point32 c, int r, int color, int thickness);


/* Line drawing functions */
_XI_API_ XI_ERR_TYPE xiLine_I8 (xi_pTile src /*inout*/, xi_point32 p1, xi_point32 p2, int color, int thickness);
_XI_API_ XI_ERR_TYPE xiLine_I16(xi_pTile src /*inout*/, xi_point32 p1, xi_point32 p2, int color, int thickness);


/* Rectangle */
_XI_API_ XI_ERR_TYPE xiRectangle_I8 (xi_pTile src /*inout*/, xi_point32 p0, xi_point32 p1, int color, int thickness);
_XI_API_ XI_ERR_TYPE xiRectangle_I16(xi_pTile src /*inout*/, xi_point32 p0, xi_point32 p1, int color, int thickness);


#endif
