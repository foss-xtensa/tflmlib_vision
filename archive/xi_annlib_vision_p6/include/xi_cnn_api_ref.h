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
#ifndef __XI_CNN_API_REF_H__
#define __XI_CNN_API_REF_H__

#include "limits.h"
#include "xi_api_ref.h"
#include "xi_core_api.h"
#include "xi_legacy_api.h"
#include "xi_cnn_api.h"

#if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))

#if (XCHAL_HAVE_VISION_HP_VFPU == 1)
#include <xtensa/tie/xt_misc.h>
#endif

#if defined(_MSC_VER) && defined(XI_REF_CREATE_SHARED_LIBRARY)
#  define _XI_EXPORTSREF_  __declspec(dllexport)
#  define _XI_APIREF_      _XI_EXTERN_C_ _XI_EXPORTSREF_
#else
#  define _XI_APIREF_      _XI_API_
#  define _XI_EXPORTSREF_  _XI_EXPORTS_
#endif


/* helper functions used by reference implementation */
_XI_APIREF_ xi_bool xiTile3DhasValidEdges_WHD_ref(const xi_pTile3D inTile, const xi_cnn_conv_params *param, const uint8_t kWidth, const uint8_t kHeight);
_XI_APIREF_ xi_bool xiTile3DhasValidEdges_DWH_ref(const xi_pTile3D inTile, const xi_cnn_conv_params *param, const uint8_t kWidth, const uint8_t kHeight);
_XI_APIREF_ xi_bool xiTile3DhasValidEdgesA_ID16WH_ref(const xi_pTile3D inTile, const xi_cnna_conv_params *param, const uint8_t kW, const uint8_t kH);
_XI_APIREF_ xi_bool xiTile3DisConsistent_MOW_WHD_ref(const xi_pTile3D inTile, const xi_pTile4D coeffTile, const xi_pTile3D outTile, \
                                                     const xi_cnn_conv_params *param);
_XI_APIREF_ xi_bool xiTile3DisConsistent_MOD_WHD_ref(const xi_pTile3D inTile, const xi_pTile4D coeffTile, const xi_pTile3D outTile, \
                                                     const xi_cnn_conv_params *param);
_XI_APIREF_ xi_bool xiTile3DisConsistent_MOD_DWH_ref(const xi_pTile3D inTile, const xi_pTile4D coeffTile, const xi_pTile3D outTile, \
                                                     const xi_cnn_conv_params *param);
_XI_APIREF_ xi_bool xiTile3DisConsistentA_depthwise_MOD_ID16WH_ref(const xi_pTile3D inTile,
                                                    const xi_pTile3D coeffTile,
                                                    const xi_pTile3D outTile,
                                                    const xi_cnna_conv_params *param);

_XI_APIREF_ xi_bool xiTile3DisConsistent_SO_ref(const xi_pTile3D inTile, const xi_pTile4D coeffTile, const xi_pTile3D outTile, \
                                                const xi_cnn_conv_params *param);
_XI_APIREF_ xi_bool xiTile3DhasValidEdges_depthwise_WHD_ref(const xi_pTile3D inTile, const xi_cnn_conv_params *param, const uint8_t kWidth, const uint8_t kHeight);
_XI_APIREF_ xi_bool xiTile3DhasValidEdges_depthwise_DWH_ref(const xi_pTile3D inTile, const xi_cnn_conv_params *param, const uint8_t kWidth, const uint8_t kHeight);
_XI_APIREF_ xi_bool xiTile3DisConsistent_depthwise_MOW_WHD_ref(const xi_pTile3D inTile, const xi_pTile3D coeffTile, \
                                                               const xi_pTile3D outTile, const xi_cnn_conv_params *param);
_XI_APIREF_ xi_bool xiTile3DisConsistent_depthwise_MOD_DWH_ref(const xi_pTile3D inTile, const xi_pTile3D coeffTile, \
                                                               const xi_pTile3D outTile, const xi_cnn_conv_params *param);

_XI_APIREF_ xi_bool xiTile3DhasValidEdgesA_DWH_ref(const xi_pTile3D inTile, const xi_cnna_conv_params *param, \
                                                   const uint8_t kWidth, const uint8_t kHeight);
_XI_APIREF_ xi_bool xiTile3DisConsistentA_MOD_DWH_ref(const xi_pTile3D inTile, const xi_pTile4D coeffTile, \
                                                      const xi_pTile3D outTile, const xi_cnna_conv_params *param);
_XI_APIREF_ xi_bool xiTile3DisConsistentA_SO_ref(const xi_pTile3D inTile, const xi_pTile4D coeffTile, \
                                                 const xi_pTile3D outTile, const xi_cnna_conv_params *param);
_XI_APIREF_ xi_bool xiTile3DisConsistentA_Fixup_DWH_ref(const xi_pTile3D inTile, const xi_pTile4D coeffTile, \
                                                        const xi_pTile fixupTile, const xi_cnna_conv_params *param);

_XI_APIREF_ xi_bool xiTile3DisConsistentA_depthwise_MOD_DWH_ref(const xi_pTile3D inTile, const xi_pTile3D coeffTile, \
                                                                const xi_pTile3D outTile, const xi_cnna_conv_params *param);
_XI_APIREF_ xi_bool xiTile3DhasValidEdgesA_depthwise_DWH_ref(const xi_pTile3D inTile, const xi_cnna_conv_params *param, \
                                                             const uint8_t kWidth, const uint8_t kHeight);
_XI_APIREF_ xi_bool xiTile3DsHaveSameEdge_ref(xi_pTile3D t1, xi_pTile3D t2);
_XI_APIREF_ xi_bool xiTile4DsHaveSameEdge_ref(xi_pTile4D t1, xi_pTile4D t2);


_XI_APIREF_ XI_ERR_TYPE xiConvolvedFixupA3D_MxN_U8S32_ID16WH_ref(const xi_pTile3D inTile,
                                                                 const xi_pTile4D coeffTile,
                                                                 xi_pArray bufArray,
                                                                 xi_pTile fixUpTile,
                                                                 const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_1x1j1_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_1x1j1_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_3x3j1_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_3x3j1_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_5x5j1_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_5x5j1_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_7x7j1_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_7x7j1_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_MxNj1_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_MxNj1_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_1x1j2_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_1x1j2_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_3x3j2_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_3x3j2_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_5x5j2_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_5x5j2_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_7x7j2_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_7x7j2_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_MxNj2_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_MxNj2_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_1x1j4_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_1x1j4_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_3x3j4_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_3x3j4_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_5x5j4_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_5x5j4_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_7x7j4_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_7x7j4_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_MxNj4_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_MxNj4_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_1x1j1d1_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_1x1j1d1_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_3x3j1d1_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_3x3j1d1_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_5x5j1d1_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_5x5j1d1_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_7x7j1d1_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_7x7j1d1_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_MxNj1d1_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_MxNj1d1_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_1x1j2d1_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_1x1j2d1_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_3x3j2d1_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_3x3j2d1_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_5x5j2d1_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_5x5j2d1_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_7x7j2d1_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_7x7j2d1_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_MxNj2d1_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_MxNj2d1_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_1x1j4d1_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_1x1j4d1_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_3x3j4d1_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_3x3j4d1_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_5x5j4d1_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_5x5j4d1_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_7x7j4d1_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_7x7j4d1_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_MxNj4d1_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_MxNj4d1_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_3x3j1d2_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_3x3j1d2_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_5x5j1d2_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_5x5j1d2_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_7x7j1d2_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_7x7j1d2_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_MxNj1d2_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_MxNj1d2_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_3x3j1d4_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_3x3j1d4_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_5x5j1d4_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_5x5j1d4_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_7x7j1d4_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_7x7j1d4_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_MxNj1d4_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_MxNj1d4_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_1x1_S8S8IXCa2_MOD_WHD_DWH_ref(const xi_pTile3D inTile,
                                                                     const xi_pTile4D coeffTile,
                                                                     const xi_pArray biasArray,
                                                                     xi_pTile3D outTile,
                                                                     xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_3x3_S8S8IXCa2_MOD_WHD_DWH_ref(const xi_pTile3D inTile,
                                                                     const xi_pTile4D coeffTile,
                                                                     const xi_pArray biasArray,
                                                                     xi_pTile3D outTile,
                                                                     xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_5x5_S8S8IXCa2_MOD_WHD_DWH_ref(const xi_pTile3D inTile,
                                                                     const xi_pTile4D coeffTile,
                                                                     const xi_pArray biasArray,
                                                                     xi_pTile3D outTile,
                                                                     xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_7x7_S8S8IXCa2_MOD_WHD_DWH_ref(const xi_pTile3D inTile,
                                                                     const xi_pTile4D coeffTile,
                                                                     const xi_pArray biasArray,
                                                                     xi_pTile3D outTile,
                                                                     xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_MxN_S8S8IXCa2_MOD_WHD_DWH_ref(const xi_pTile3D inTile,
                                                                     const xi_pTile4D coeffTile,
                                                                     const xi_pArray biasArray,
                                                                     xi_pTile3D outTile,
                                                                     xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_1x1_S8S8IXCa2_MOD_DWH_ref(const xi_pTile3D inTile,
                                                                 const xi_pTile4D coeffTile,
                                                                 const xi_pArray biasArray,
                                                                 xi_pTile3D outTile,
                                                                 xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_3x3_S8S8IXCa2_MOD_DWH_ref(const xi_pTile3D inTile,
                                                                 const xi_pTile4D coeffTile,
                                                                 const xi_pArray biasArray,
                                                                 xi_pTile3D outTile,
                                                                 xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_5x5_S8S8IXCa2_MOD_DWH_ref(const xi_pTile3D inTile,
                                                                 const xi_pTile4D coeffTile,
                                                                 const xi_pArray biasArray,
                                                                 xi_pTile3D outTile,
                                                                 xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_7x7_S8S8IXCa2_MOD_DWH_ref(const xi_pTile3D inTile,
                                                                 const xi_pTile4D coeffTile,
                                                                 const xi_pArray biasArray,
                                                                 xi_pTile3D outTile,
                                                                 xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_MxN_S8S8IXCa2_MOD_DWH_ref(const xi_pTile3D inTile,
                                                                 const xi_pTile4D coeffTile,
                                                                 const xi_pArray biasArray,
                                                                 xi_pTile3D outTile,
                                                                 xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_MxN_S8S8IX_SO_DWH_ref(const xi_pTile3D inTile,
                                                             const xi_pTile4D coeffTile,
                                                             const xi_pArray biasArray,
                                                             xi_pTile3D outTile,
                                                             xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_MxN_U8S8IX_SO_DWH_ref(const xi_pTile3D inTile,
                                                             const xi_pTile4D coeffTile,
                                                             const xi_pArray biasArray,
                                                             xi_pTile3D outTile,
                                                             xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_1x1_S8S8IXCa2_MOD_WHD_DWH_ref(const xi_pTile3D inTile,
                                                                      const xi_pTile4D coeffTile,
                                                                      const xi_pArray biasArray,
                                                                      xi_pTile3D outTile,
                                                                      const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_3x3_S8S8IXCa2_MOD_WHD_DWH_ref(const xi_pTile3D inTile,
                                                                      const xi_pTile4D coeffTile,
                                                                      const xi_pArray biasArray,
                                                                      xi_pTile3D outTile,
                                                                      const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_5x5_S8S8IXCa2_MOD_WHD_DWH_ref(const xi_pTile3D inTile,
                                                                      const xi_pTile4D coeffTile,
                                                                      const xi_pArray biasArray,
                                                                      xi_pTile3D outTile,
                                                                      const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_7x7_S8S8IXCa2_MOD_WHD_DWH_ref(const xi_pTile3D inTile,
                                                                      const xi_pTile4D coeffTile,
                                                                      const xi_pArray biasArray,
                                                                      xi_pTile3D outTile,
                                                                      const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_MxN_S8S8IXCa2_MOD_WHD_DWH_ref(const xi_pTile3D inTile,
                                                                      const xi_pTile4D coeffTile,
                                                                      const xi_pArray biasArray,
                                                                      xi_pTile3D outTile,
                                                                      const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_1x1_S8S8IXCa2_MOD_DWH_ref(const xi_pTile3D inTile,
                                                                  const xi_pTile4D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_3x3_S8S8IXCa2_MOD_DWH_ref(const xi_pTile3D inTile,
                                                                  const xi_pTile4D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_5x5_S8S8IXCa2_MOD_DWH_ref(const xi_pTile3D inTile,
                                                                  const xi_pTile4D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_7x7_S8S8IXCa2_MOD_DWH_ref(const xi_pTile3D inTile,
                                                                  const xi_pTile4D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_MxN_S8S8IXCa2_MOD_DWH_ref(const xi_pTile3D inTile,
                                                                  const xi_pTile4D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_MxN_S8S8IX_SO_DWH_ref(const xi_pTile3D inTile,
                                                              const xi_pTile4D coeffTile,
                                                              const xi_pArray biasArray,
                                                              xi_pTile3D outTile,
                                                              const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_MxN_U8S8IX_SO_DWH_ref(const xi_pTile3D inTile,
                                                              const xi_pTile4D coeffTile,
                                                              const xi_pArray biasArray,
                                                              xi_pTile3D outTile,
                                                              const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_ref(const xi_pTile3D inTile,
                                         const xi_pTile4D coeffTile,
                                         const xi_pArray biasArray,
                                         xi_pTile3D outTile,
                                         xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_ref(const xi_pTile3D inTile,
                                          const xi_pTile4D coeffTile,
                                          const xi_pArray biasArray,
                                          xi_pTile3D outTile,
                                          const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiFullyConnected3D_ref(const xi_pTile3D inTile,
                                               const xi_pTile4D coeffTile,
                                               const xi_pArray biasArray,
                                               xi_pTile3D outTile,
                                               const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiFullyConnected3D_S_S8S8IX_ref(const xi_pTile3D inTile,
                                                        const xi_pTile4D coeffTile,
                                                        const xi_pArray biasArray,
                                                        xi_pTile3D outTile,
                                                        const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiFullyConnected3D_S_U8S8IX_ref(const xi_pTile3D inTile,
                                                        const xi_pTile4D coeffTile,
                                                        const xi_pArray biasArray,
                                                        xi_pTile3D outTile,
                                                        const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiLRNDepth3D_S_5_U8S8_WHD_ref(const xi_pTile3D inTile,
                                                      const xi_pArray lutArray,
                                                      xi_pTile3D outTile,
                                                      const xi_cnn_lrn_depth_params *param);

_XI_APIREF_ XI_ERR_TYPE xiLRNDepth3D_S_3_U8S8_WHD_ref(const xi_pTile3D inTile,
                                                      const xi_pArray lutArray,
                                                      xi_pTile3D outTile,
                                                      const xi_cnn_lrn_depth_params *param);

_XI_APIREF_ XI_ERR_TYPE xiLRNDepth3D_S_3_U8S8_WHD_float_ref(const xi_pTile3D inTile,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_lrn_depth_params *param,
                                                            float alpha, float beta, float kValue,
                                                            float qIn, float qOut);

_XI_APIREF_ XI_ERR_TYPE xiLRNDepth3D_S_5_U8S8_WHD_float_ref(const xi_pTile3D inTile,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_lrn_depth_params *param,
                                                            float alpha, float beta, float kValue,
                                                            float qIn, float qOut);

_XI_APIREF_ XI_ERR_TYPE xiFillTile3D_ref(xi_pTile3D dstTile,
                                         const int32_t value,
                                         xi_bool fill_edge_extension);

_XI_APIREF_ XI_ERR_TYPE xiFillTile3D_I8_ref(xi_pTile3D dstTile,
                                            const int32_t value,
                                            xi_bool fill_edge_extension);

_XI_APIREF_ XI_ERR_TYPE xiFillTile3D_I16_ref(xi_pTile3D dstTile,
                                             const int32_t value,
                                             xi_bool fill_edge_extension);

_XI_APIREF_ XI_ERR_TYPE xiExtendEdgesConst3D_ref(xi_pTile3D dstTile,
                                                 const int32_t value,
                                                 xi_size3D frame3DSize);

_XI_APIREF_ XI_ERR_TYPE xiExtendEdgesConst3D_I8_ref(xi_pTile3D dstTile,
                                                    const int32_t value,
                                                    xi_size3D frame3DSize);

_XI_APIREF_ XI_ERR_TYPE xiExtendEdgesConst3D_I16_ref(xi_pTile3D dstTile,
                                                     const int32_t value,
                                                     xi_size3D frame3DSize);

_XI_APIREF_ XI_ERR_TYPE xiExtendEdges3D_ref(xi_pTile3D dstTile,
                                            const xi_pArray pArray,
                                            xi_size3D frame3DSize);

_XI_APIREF_ XI_ERR_TYPE xiExtendEdges3D_I8_ref(xi_pTile3D dstTile,
                                               const xi_pArray pArray,
                                               xi_size3D frame3DSize);

_XI_APIREF_ XI_ERR_TYPE xiExtendEdges3D_I16_ref(xi_pTile3D dstTile,
                                                const xi_pArray pArray,
                                                xi_size3D frame3DSize);

_XI_APIREF_ XI_ERR_TYPE xiCopyTile3D_ref(const xi_pTile3D inTile,
                                         xi_pTile3D outTile,
                                         xi_bool copy_edge_extension);

_XI_APIREF_ XI_ERR_TYPE xiTranspose3D_ref(const xi_pTile3D inTile,
                                          xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiTranspose3D_I8_WHD_DWH_ref(const xi_pTile3D inTile,
                                                     xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiTranspose3D_I8_DWH_WHD_ref(const xi_pTile3D inTile,
                                                     xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiUnsignedToSigned3D_U8S8_ref(xi_pTile3D inTile, xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiArgmax_S8_ref(const xi_pTile3D inTile,
                                        xi_pTile3D outTileIdx,
                                        xi_pTile3D outTileVal,
                                        xi_pTile extraValCnt,
                                        xi_pArray sortedIdxArr,
                                        xi_pArray sortedValArr,
                                        const uint16_t numLargestVal);

_XI_APIREF_ XI_ERR_TYPE xiArgmaxA3D_U8U32_ref(const xi_pTile3D inTile,
                                              xi_pTile3D outTile,
                                              const xi_cnn_ArgMinMaxA_params *params);

_XI_APIREF_ XI_ERR_TYPE xiArgmaxA3D_S8U32_ref(const xi_pTile3D inTile,
                                              xi_pTile3D outTile,
                                              const xi_cnn_ArgMinMaxA_params *params);

_XI_APIREF_ XI_ERR_TYPE xiArgmaxA4D_U8U32_ref(const xi_pTile4D inTile,
                                              xi_pTile4D outTile,
                                              const xi_cnn_ArgMinMaxA_params *params);

_XI_APIREF_ XI_ERR_TYPE xiArgmaxA4D_S8U32_ref(const xi_pTile4D inTile,
                                              xi_pTile4D outTile,
                                              const xi_cnn_ArgMinMaxA_params *params);

_XI_APIREF_ XI_ERR_TYPE xiArgminA3D_U8U32_ref(const xi_pTile3D inTile,
                                              xi_pTile3D outTile,
                                              const xi_cnn_ArgMinMaxA_params *params);

_XI_APIREF_ XI_ERR_TYPE xiArgminA3D_S8U32_ref(const xi_pTile3D inTile,
                                              xi_pTile3D outTile,
                                              const xi_cnn_ArgMinMaxA_params *params);

_XI_APIREF_ XI_ERR_TYPE xiArgminA4D_U8U32_ref(const xi_pTile4D inTile,
                                              xi_pTile4D outTile,
                                              const xi_cnn_ArgMinMaxA_params *params);

_XI_APIREF_ XI_ERR_TYPE xiArgminA4D_S8U32_ref(const xi_pTile4D inTile,
                                              xi_pTile4D outTile,
                                              const xi_cnn_ArgMinMaxA_params *params);

_XI_APIREF_ XI_ERR_TYPE xiLeakyRELU_ref(const xi_pTile3D inTile,
                                        xi_pTile3D outTile,
                                        const XI_Q15 slope);

_XI_APIREF_ XI_ERR_TYPE xiLeakyRELU_S8_ref(const xi_pTile3D inTile,
                                           xi_pTile3D outTile,
                                           const XI_Q15 slope);


_XI_APIREF_ XI_ERR_TYPE xiLeakyRELU_S8_float_ref(const xi_pTile3D inTile,
                                                 xi_pTile3D outTile,
                                                 const float slope);


_XI_APIREF_ XI_ERR_TYPE xiLeakyRELU_S16_ref(const xi_pTile3D inTile,
                                            xi_pTile3D outTile,
                                            const XI_Q15 slope);

_XI_APIREF_ XI_ERR_TYPE xiLeakyRELU_S16_float_ref(const xi_pTile3D inTile,
                                                  xi_pTile3D outTile,
                                                  const float slope);

_XI_APIREF_ XI_ERR_TYPE xiPreluA3D_U8_DWH_ref(const xi_pTile3D inTile,
                                              xi_array *slopeArray,
                                              xi_pTile3D outTile,
                                              const xi_cnn_preluA_params *params,
                                              xi_array *scratchBuffArray);

_XI_APIREF_ XI_ERR_TYPE xiPreluA3D_S8_DWH_ref(const xi_pTile3D inTile,
                                              xi_array *slopeArray,
                                              xi_pTile3D outTile,
                                              const xi_cnn_preluA_params *params,
                                              xi_array *scratchBuffArray);

_XI_APIREF_ XI_ERR_TYPE xiPreluA3D_U8U8U8_DWH_ref(const xi_pTile3D inTile,
                                                  const xi_pTile3D slopeTile,
                                                  xi_pTile3D outTile,
                                                  const xi_cnn_preluA_params *param);

_XI_APIREF_ XI_ERR_TYPE xiPreluQuantizeA3D_S8_DWH_ref(const xi_pTile3D inTile,
                                                      const xi_pTile3D slopeTile,
                                                      xi_pTile3D outTile,
                                                      const xi_cnn_preluA_params *param);

_XI_APIREF_ XI_ERR_TYPE xiHardSwishA3D_U8_ref(const xi_pTile3D inTile,
                                              xi_pTile3D outTile,
                                              const xi_cnn_hardSwishA3D_params *params);

_XI_APIREF_ XI_ERR_TYPE xiHardSwishA3D_S8_ref(const xi_pTile3D inTile,
                                              xi_pTile3D outTile,
                                              const xi_cnn_hardSwishA3D_params *params);

_XI_APIREF_ XI_ERR_TYPE xiLUT3D_S8I16_ref(const xi_pTile3D inTile,
                                          const xi_pArray lutArray,
                                          xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiTanh_S8S16_float_ref(const xi_pTile3D inTile,
                                               xi_pTile3D outTile,
                                               const float qIn);

_XI_APIREF_ XI_ERR_TYPE xiSigmoid_S8U16_float_ref(const xi_pTile3D inTile,
                                                  xi_pTile3D outTile,
                                                  const float qIn);

_XI_APIREF_ XI_ERR_TYPE xiLRNDepth3D_S_3_U8S8_DWH_ref(const xi_pTile3D inTile,
                                                      const xi_pArray lutArray,
                                                      xi_pTile3D outTile,
                                                      const xi_cnn_lrn_depth_params *param);

_XI_APIREF_ XI_ERR_TYPE xiLRNDepth3D_S_5_U8S8_DWH_ref(const xi_pTile3D inTile,
                                                      const xi_pArray lutArray,
                                                      xi_pTile3D outTile,
                                                      const xi_cnn_lrn_depth_params *param);

_XI_APIREF_ XI_ERR_TYPE xiLRNDepth3D_S_3_U8S8_DWH_float_ref(const xi_pTile3D inTile,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_lrn_depth_params *param,
                                                            float alpha, float beta, float kValue,
                                                            float qIn, float qOut);

_XI_APIREF_ XI_ERR_TYPE xiLRNDepth3D_S_5_U8S8_DWH_float_ref(const xi_pTile3D inTile,
                                                            xi_pTile3D outTile,
                                                            const xi_cnn_lrn_depth_params *param,
                                                            float alpha, float beta, float kValue,
                                                            float qIn, float qOut);

_XI_APIREF_ XI_ERR_TYPE xiLRNDepth3D_ref(const xi_pTile3D inTile,
                                         const xi_pArray lutArray,
                                         xi_pTile3D outTile,
                                         const xi_cnn_lrn_depth_params *param);

_XI_APIREF_ XI_ERR_TYPE xiLRNSpatial3D_ref(const xi_pTile3D inTile,
                                           const xi_pArray lutArray,
                                           xi_pTile3D outTile,
                                           const xi_cnn_lrn_spatial_params *param);

_XI_APIREF_ XI_ERR_TYPE xiLRNSpatial3D_S_3x3_U8S8_WHD_ref(const xi_pTile3D inTile,
                                                          const xi_pArray lutArray,
                                                          xi_pTile3D outTile,
                                                          const xi_cnn_lrn_spatial_params *param);

_XI_APIREF_ XI_ERR_TYPE xiLRNSpatial3D_S_5x5_U8S8_WHD_ref(const xi_pTile3D inTile,
                                                          const xi_pArray lutArray,
                                                          xi_pTile3D outTile,
                                                          const xi_cnn_lrn_spatial_params *param);

_XI_APIREF_ XI_ERR_TYPE xiLRNSpatial3D_S_3x3_U8S8_WHD_float_ref(const xi_pTile3D inTile,
                                                                xi_pTile3D outTile,
                                                                const xi_cnn_lrn_spatial_params *param,
                                                                float alpha, float beta, float kValue,
                                                                float qIn, float qOut);

_XI_APIREF_ XI_ERR_TYPE xiLRNSpatial3D_S_5x5_U8S8_WHD_float_ref(const xi_pTile3D inTile,
                                                                xi_pTile3D outTile,
                                                                const xi_cnn_lrn_spatial_params *param,
                                                                float alpha, float beta, float kValue,
                                                                float qIn, float qOut);

_XI_APIREF_ XI_ERR_TYPE xiLRNSpatial3D_S_3x3_U8S8_DWH_ref(const xi_pTile3D inTile,
                                                          const xi_pArray lutArray,
                                                          xi_pTile3D outTile,
                                                          const xi_cnn_lrn_spatial_params *param);

_XI_APIREF_ XI_ERR_TYPE xiLRNSpatial3D_S_5x5_U8S8_DWH_ref(const xi_pTile3D inTile,
                                                          const xi_pArray lutArray,
                                                          xi_pTile3D outTile,
                                                          const xi_cnn_lrn_spatial_params *param);

_XI_APIREF_ XI_ERR_TYPE xiLRNSpatial3D_S_3x3_U8S8_DWH_float_ref(const xi_pTile3D inTile,
                                                                xi_pTile3D outTile,
                                                                const xi_cnn_lrn_spatial_params *param,
                                                                float alpha, float beta, float kValue,
                                                                float qIn, float qOut);

_XI_APIREF_ XI_ERR_TYPE xiLRNSpatial3D_S_5x5_U8S8_DWH_float_ref(const xi_pTile3D inTile,
                                                                xi_pTile3D outTile,
                                                                const xi_cnn_lrn_spatial_params *param,
                                                                float alpha, float beta, float kValue,
                                                                float qIn, float qOut);

_XI_APIREF_ XI_ERR_TYPE xiMaxPool3D_MxNj1_S8_WHD_ref(const xi_pTile3D inTile,
                                                     xi_pTile3D outTile,
                                                     const xi_cnn_pooling_params *param);

_XI_APIREF_ XI_ERR_TYPE xiMaxPool3D_MxNj1_U8_WHD_ref(const xi_pTile3D inTile,
                                                     xi_pTile3D outTile,
                                                     const xi_cnn_pooling_params *param);

_XI_APIREF_ XI_ERR_TYPE xiMaxPool3D_MxNj1_S16_WHD_ref(const xi_pTile3D inTile,
                                                      xi_pTile3D outTile,
                                                      const xi_cnn_pooling_params *param);

_XI_APIREF_ XI_ERR_TYPE xiMaxPool3D_MxNj2_S8_WHD_ref(const xi_pTile3D inTile,
                                                     xi_pTile3D outTile,
                                                     const xi_cnn_pooling_params *param);

_XI_APIREF_ XI_ERR_TYPE xiMaxPool3D_MxNj2_U8_WHD_ref(const xi_pTile3D inTile,
                                                     xi_pTile3D outTile,
                                                     const xi_cnn_pooling_params *param);

_XI_APIREF_ XI_ERR_TYPE xiMaxPool3D_MxNj2_S16_WHD_ref(const xi_pTile3D inTile,
                                                      xi_pTile3D outTile,
                                                      const xi_cnn_pooling_params *param);

_XI_APIREF_ XI_ERR_TYPE xiMaxPool3D_MxN_S8_WHD_ref(const xi_pTile3D inTile,
                                                   xi_pTile3D outTile,
                                                   const xi_cnn_pooling_params *param);

_XI_APIREF_ XI_ERR_TYPE xiMaxPool3D_MxN_U8_WHD_ref(const xi_pTile3D inTile,
                                                   xi_pTile3D outTile,
                                                   const xi_cnn_pooling_params *param);

_XI_APIREF_ XI_ERR_TYPE xiMaxPool3D_MxN_S16_WHD_ref(const xi_pTile3D inTile,
                                                    xi_pTile3D outTile,
                                                    const xi_cnn_pooling_params *param);

_XI_APIREF_ XI_ERR_TYPE xiMaxPool3D_MxN_S8_DWH_ref(const xi_pTile3D inTile,
                                                   xi_pTile3D outTile,
                                                   const xi_cnn_pooling_params *param);

_XI_APIREF_ XI_ERR_TYPE xiMaxPool3D_MxN_U8_DWH_ref(const xi_pTile3D inTile,
                                                   xi_pTile3D outTile,
                                                   const xi_cnn_pooling_params *param);

_XI_APIREF_ XI_ERR_TYPE xiMaxPool3D_MxN_S16_DWH_ref(const xi_pTile3D inTile,
                                                    xi_pTile3D outTile,
                                                    const xi_cnn_pooling_params *param);

_XI_APIREF_ XI_ERR_TYPE xiMaxPool3D_ref(const xi_pTile3D inTile,
                                        xi_pTile3D outTile,
                                        const xi_cnn_pooling_params *param);


_XI_APIREF_ XI_ERR_TYPE xiAvgPool3D_MxNj1_S8_WHD_ref(const xi_pTile3D inTile,
                                                     xi_pArray bufArray,
                                                     xi_pTile3D outTile,
                                                     const xi_cnn_pooling_params *param);

_XI_APIREF_ XI_ERR_TYPE xiAvgPool3D_MxNj1_U8_WHD_ref(const xi_pTile3D inTile,
                                                     xi_pArray bufArray,
                                                     xi_pTile3D outTile,
                                                     const xi_cnn_pooling_params *param);

_XI_APIREF_ XI_ERR_TYPE xiAvgPool3D_MxNj2_S8_WHD_ref(const xi_pTile3D inTile,
                                                     xi_pTile3D outTile,
                                                     const xi_cnn_pooling_params *param);

_XI_APIREF_ XI_ERR_TYPE xiAvgPool3D_MxNj2_U8_WHD_ref(const xi_pTile3D inTile,
                                                     xi_pTile3D outTile,
                                                     const xi_cnn_pooling_params *param);

_XI_APIREF_ XI_ERR_TYPE xiAvgPool3D_MxN_S8_DWH_ref(const xi_pTile3D inTile,
                                                   xi_pTile3D outTile,
                                                   const xi_cnn_pooling_params *param);

_XI_APIREF_ XI_ERR_TYPE xiAvgPool3D_MxN_U8_DWH_ref(const xi_pTile3D inTile,
                                                   xi_pTile3D outTile,
                                                   const xi_cnn_pooling_params *param);

_XI_APIREF_ XI_ERR_TYPE xiAvgPool3D_MxNj1_S16_WHD_ref(const xi_pTile3D inTile,
                                                      xi_pArray bufArray,
                                                      xi_pTile3D outTile,
                                                      const xi_cnn_pooling_params *param);

_XI_APIREF_ XI_ERR_TYPE xiAvgPool3D_MxNj2_S16_WHD_ref(const xi_pTile3D inTile,
                                                      xi_pTile3D outTile,
                                                      const xi_cnn_pooling_params *param);

_XI_APIREF_ XI_ERR_TYPE xiAvgPool3D_MxN_S16_DWH_ref(const xi_pTile3D inTile,
                                                    xi_pTile3D outTile,
                                                    const xi_cnn_pooling_params *param);

_XI_APIREF_ XI_ERR_TYPE xiAvgPool3D_ref(const xi_pTile3D inTile,
                                        xi_pArray bufArray,
                                        xi_pTile3D outTile,
                                        const xi_cnn_pooling_params *param);

_XI_APIREF_ XI_ERR_TYPE xiSoftmax_S16U16_ref(const xi_pArray input,
                                             const xi_pArray lutArray,
                                             xi_pArray output,
                                             const xi_cnn_softmax_params *params);

_XI_APIREF_ XI_ERR_TYPE xiSoftmax_S16U16_float_ref(const xi_pArray input,
                                                   xi_pArray output,
                                                   const float qIn);

_XI_APIREF_ XI_ERR_TYPE xiDataConversion3D_ref(const xi_pTile3D inTile,
                                               xi_pTile3D outTile,
                                               const uint16_t scale,
                                               const uint8_t shift);

_XI_APIREF_ XI_ERR_TYPE xiDataConversion3D_S16I8_ref(const xi_pTile3D inTile,
                                                     xi_pTile3D outTile,
                                                     const uint16_t scale,
                                                     const uint8_t shift);

_XI_APIREF_ XI_ERR_TYPE xiDataConversion3D_S8S16_ref(const xi_pTile3D inTile,
                                                     xi_pTile3D outTile,
                                                     const uint16_t scale,
                                                     const uint8_t shift);

_XI_APIREF_ XI_ERR_TYPE xiDataConversion3D_U8S16_ref(const xi_pTile3D inTile,
                                                     xi_pTile3D outTile,
                                                     const uint16_t scale,
                                                     const uint8_t shift);

_XI_APIREF_ XI_ERR_TYPE xiEltwiseAdd3D_ref(const xi_pTile3D inTile1,
                                           const xi_pTile3D inTile2,
                                           xi_pTile3D outTile,
                                           const xi_cnn_eltwise_params *param);

_XI_APIREF_ XI_ERR_TYPE xiEltwiseAdd3D_S8_ref(const xi_pTile3D inTile1,
                                              const xi_pTile3D inTile2,
                                              xi_pTile3D outTile,
                                              const xi_cnn_eltwise_params *param);

_XI_APIREF_ XI_ERR_TYPE xiEltwiseAdd3D_S16_ref(const xi_pTile3D inTile1,
                                               const xi_pTile3D inTile2,
                                               xi_pTile3D outTile,
                                               const xi_cnn_eltwise_params *param);

_XI_APIREF_ XI_ERR_TYPE xiRenorm3D_ref(const xi_pTile3D inTile,
                                       xi_pTile3D outTile,
                                       const uint16_t renormScale,
                                       const uint8_t renormShift);

_XI_APIREF_ XI_ERR_TYPE xiRenorm3D_S8_ref(const xi_pTile3D inTile,
                                          xi_pTile3D outTile,
                                          const uint16_t renormScale,
                                          const uint8_t renormShift);

_XI_APIREF_ XI_ERR_TYPE xiRenorm3D_U8_ref(const xi_pTile3D inTile,
                                          xi_pTile3D outTile,
                                          const uint16_t renormScale,
                                          const uint8_t renormShift);


_XI_APIREF_ XI_ERR_TYPE xiConvolve3D_S_MxN_IXIXIX_reff(const xi_pTile3D inTile,
                                                       const xi_pTile4D coeffTile,
                                                       const xi_pArray biasArray,
                                                       xi_pTile3D outTile,
                                                       xi_cnn_conv_params *param,
                                                       const int32_t numGroups,
                                                       const int8_t edgeExtendLeft,
                                                       const int8_t edgeExtendRight,
                                                       const int8_t edgeExtendTop,
                                                       const int8_t edgeExtendBottom,
                                                       const xi_pArray edgeExtendValArray);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_S_MxN_IXIXIX_reff(const xi_pTile3D inTile,
                                                        const xi_pTile4D coeffTile,
                                                        const xi_pArray biasArray,
                                                        xi_pTile3D outTile,
                                                        const xi_cnn_conv_params *param,
                                                        const int32_t numGroups,
                                                        const int8_t edgeExtendLeft,
                                                        const int8_t edgeExtendRight,
                                                        const int8_t edgeExtendTop,
                                                        const int8_t edgeExtendBottom,
                                                        const xi_pArray edgeExtendValArray);

_XI_APIREF_ XI_ERR_TYPE xiMaxPool3D_MxN_IX_reff(const xi_pTile3D inTile,
                                                xi_pTile3D outTile,
                                                const xi_cnn_pooling_params *param,
                                                const int8_t edgeExtendLeft,
                                                const int8_t edgeExtendRight,
                                                const int8_t edgeExtendTop,
                                                const int8_t edgeExtendBottom,
                                                const int32_t edgeFillValue);

_XI_APIREF_ XI_ERR_TYPE xiMeanA_U8_reff(const xi_pTile3D inTile,
                                        xi_pTile3D outTile,
                                        const xi_cnn_meanA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiStridedSliceA_U8_reff(const xi_pTile3D inTile,
                                                xi_pTile3D outTile,
                                                const xi_cnn_stridedsliceA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiDeconvolveA3D_MxN_U8U8U8_MOD_DWH_reff(const xi_pTile3D inTile, const xi_pTile4D coeffTile, const xi_pArray biasArray,
                                                                xi_pTile3D outTile, xi_cnn_deconvolveA_params *params);


_XI_APIREF_ XI_ERR_TYPE xiDeconvolveAVQ3D_MxN_I8S8I8_MOD_DWH_reff(const xi_pTile3D inTile,
                                                      const xi_pTile4D coeffTile,
                                                      const xi_pArray biasArray,
                                                      const xi_pArray outScaleArray,
                                                      const xi_pArray outShiftArray,
                                                      xi_pTile3D outTile,
                                                      xi_cnn_deconvolveA_params *params);

_XI_APIREF_ XI_ERR_TYPE xiTransposeConvolveA3D_MxN_U8U8U8_MOD_DWH_reff(const xi_pTile3D inTile, const xi_pTile4D coeffTile, const xi_pArray biasArray,
                                                                       xi_pTile3D outTile, xi_cnn_deconvolveA_params *params);

_XI_APIREF_ XI_ERR_TYPE xiAvgPool3D_MxN_IX_reff(const xi_pTile3D inTile,
                                                xi_pTile3D outTile,
                                                const xi_cnn_pooling_params *param,
                                                const int8_t edgeExtendLeft,
                                                const int8_t edgeExtendRight,
                                                const int8_t edgeExtendTop,
                                                const int8_t edgeExtendBottom);

_XI_APIREF_ XI_ERR_TYPE xiLRNDepth3D_U8S8_reff(const xi_pTile3D inTile,
                                               const xi_pArray lutArray,
                                               xi_pTile3D outTile,
                                               const xi_cnn_lrn_depth_params *param,
                                               const int8_t edgeExtendFront,
                                               const int8_t edgeExtendBack);

_XI_APIREF_ XI_ERR_TYPE xiLRNSpatial3D_U8S8_reff(const xi_pTile3D inTile,
                                                 const xi_pArray lutArray,
                                                 xi_pTile3D outTile,
                                                 const xi_cnn_lrn_spatial_params *param,
                                                 const int8_t edgeExtendLeft,
                                                 const int8_t edgeExtendRight,
                                                 const int8_t edgeExtendTop,
                                                 const int8_t edgeExtendBottom);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_MxN_IXIXIX_reff(const xi_pTile3D inTile,
                                                                const xi_pTile3D coeffTile,
                                                                const xi_pArray biasArray,
                                                                xi_pTile3D outTile,
                                                                const xi_cnn_conv_params *param,
                                                                const int8_t edgeExtendLeft,
                                                                const int8_t edgeExtendRight,
                                                                const int8_t edgeExtendTop,
                                                                const int8_t edgeExtendBottom,
                                                                const xi_pArray edgeExtendValArray);

_XI_APIREF_ XI_ERR_TYPE xiMaxout3D_S8_WHD_ref(const xi_pTile3D inTile,
                                              xi_pTile3D outTile,
                                              const uint16_t kSize);

_XI_APIREF_ XI_ERR_TYPE xiMaxout3D_S8_DWH_ref(const xi_pTile3D inTile,
                                              xi_pTile3D outTile,
                                              const uint16_t kSize);

_XI_APIREF_ XI_ERR_TYPE xiMaxout3D_ref(const xi_pTile3D inTile,
                                       xi_pTile3D outTile,
                                       const uint16_t kSize);

_XI_APIREF_ XI_ERR_TYPE xiMeanSubtraction3D_U8S8_ref(const xi_pTile3D inTile,
                                                     xi_pTile3D outTile,
                                                     const uint8_t mean,
                                                     const uint16_t scale,
                                                     const uint8_t shift);


_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_3x3j1_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_3x3j1_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_5x5j1_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_5x5j1_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_7x7j1_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_7x7j1_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_MxNj1_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_MxNj1_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_3x3j2_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_3x3j2_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_5x5j2_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_5x5j2_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_7x7j2_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_7x7j2_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_MxNj2_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_MxNj2_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_3x3j4_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_3x3j4_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_5x5j4_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_5x5j4_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_7x7j4_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_7x7j4_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_MxNj4_S8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_MxNj4_U8S8IX_MOW_WHD_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_3x3_S8S8IXCa2_MOD_DWH_ref(const xi_pTile3D inTile,
                                                                          const xi_pTile3D coeffTile,
                                                                          const xi_pArray biasArray,
                                                                          xi_pTile3D outTile,
                                                                          const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_5x5_S8S8IXCa2_MOD_DWH_ref(const xi_pTile3D inTile,
                                                                          const xi_pTile3D coeffTile,
                                                                          const xi_pArray biasArray,
                                                                          xi_pTile3D outTile,
                                                                          const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_7x7_S8S8IXCa2_MOD_DWH_ref(const xi_pTile3D inTile,
                                                                          const xi_pTile3D coeffTile,
                                                                          const xi_pArray biasArray,
                                                                          xi_pTile3D outTile,
                                                                          const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_S_MxN_S8S8IXCa2_MOD_DWH_ref(const xi_pTile3D inTile,
                                                                          const xi_pTile3D coeffTile,
                                                                          const xi_pArray biasArray,
                                                                          xi_pTile3D outTile,
                                                                          const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolve2D_ref(const xi_pTile3D inTile,
                                                  const xi_pTile3D coeffTile,
                                                  const xi_pArray biasArray,
                                                  xi_pTile3D outTile,
                                                  const xi_cnn_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedBiasUpdateA_S8_ref(const xi_pTile4D coeffTile,
                                                      xi_pArray biasArray,
                                                      const int16_t zeroPtInput,
                                                      const uint8_t zeroPtCoeff);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedBiasUpdateA_U8_ref(const xi_pTile4D coeffTile,
                                                      xi_pArray biasArray,
                                                      const uint8_t zeroPtInput,
                                                      const uint8_t zeroPtCoeff);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedFixupA3D_MxN_U8S32_DWH_ref(const xi_pTile3D inTile,
                                                              const xi_pTile4D coeffTile,
                                                              xi_pTile fixupTile,
                                                              const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedFixupSubtract128A3D_1x1_U8S8S32_DWH_ref(const xi_pTile3D inTile,
                                                                           const xi_pTile4D coeffTile,
                                                                           xi_pTile3D outTile,
                                                                           xi_pTile fixupTile,
                                                                           const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedFixupSubtract128A3D_1x1_U8S8S32Ia2_DWH_ref(const xi_pTile3D inTile,
                                                                              const xi_pTile4D coeffTile,
                                                                              xi_pTile3D outTile,
                                                                              xi_pTile fixupTile,
                                                                              const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedFixupSubtract128A3D_1x1_ref(const xi_pTile3D inTile,
                                                               const xi_pTile4D coeffTile,
                                                               xi_pTile3D outTile,
                                                               xi_pTile fixupTile,
                                                               const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedA3D_S_1x1_S8S8U8Ca2_MOD_DWH_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile fixUpTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedA3D_S_MxN_S8S8U8Ca2_MOD_DWH_ref(const xi_pTile3D inTile,
                                                                   const xi_pTile fixUpTile,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnna_conv_params *param);


_XI_APIREF_ XI_ERR_TYPE xiConvolvedA3D_S_MxN_U8S8U8_SO_DWH_ref(const xi_pTile3D inTile,
                                                               const xi_pTile fixUpTile,
                                                               const xi_pTile4D coeffTile,
                                                               const xi_pArray biasArray,
                                                               xi_pTile3D outTile,
                                                               const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedA3D_ref(const xi_pTile3D inTile,
                                           const xi_pTile fixUpTile,
                                           const xi_pTile4D coeffTile,
                                           const xi_pArray biasArray,
                                           xi_pTile3D outTile,
                                           const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedA3D_3x3_U8S8U8j1d1_MOW_DWH_ref(const xi_pTile3D inTile, xi_pTile fixupTile,
                                                                  const xi_pTile4D coeffTile, const xi_pArray biasArray,
                                                                  xi_pTile3D outTile, const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedA3D_3x3_U8S8U8j2d1_MOW_DWH_DEPTH3_ref(const xi_pTile3D inTile, xi_pTile fixupTile,
                                                                         const xi_pTile4D coeffTile, const xi_pArray biasArray,
                                                                         xi_pTile3D outTile, const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedA3D_7x7_U8S8U8j2d1_MOW_DWH_DEPTH3_ref(const xi_pTile3D inTile, xi_pTile fixupTile,
                                                                         const xi_pTile4D coeffTile, const xi_pArray biasArray,
                                                                         xi_pTile3D outTile, const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedA3D_1x1_U8S8U8j1d1_MOW_DWH_ref(const xi_pTile3D inTile, xi_pTile fixupTile,
                                                                  const xi_pTile4D coeffTile, const xi_pArray biasArray,
                                                                  xi_pTile3D outTile, const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedAVQ3D_1x1_S8j1d1_MOW_DWH_ref(const xi_pTile3D inTile,
                                                                    xi_pTile transposeTile,
                                                                    const xi_pTile4D coeffTile,
                                                                    const xi_pArray biasArray,
                                                                    const xi_pArray outputScaleArray,
                                                                    const xi_pArray outputShiftArray,
                                                                    xi_pTile3D outTile,
                                                                    const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedAVQ3D_3x3_S8j1d1_MOW_DWH_ref(const xi_pTile3D inTile,
                                                                    xi_pTile transposeTile,
                                                                    const xi_pTile4D coeffTile,
                                                                    const xi_pArray biasArray,
                                                                    const xi_pArray outputScaleArray,
                                                                    const xi_pArray outputShiftArray,
                                                                    xi_pTile3D outTile,
                                                                    const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedAVQ3D_3x3_S8j2d1_MOW_DWH_DEPTH3_ref(const xi_pTile3D inTile,
                                                                       xi_pTile transposeTile,
                                                                       const xi_pTile4D coeffTile,
                                                                       const xi_pArray biasArray,
                                                                       const xi_pArray outputScaleArray,
                                                                       const xi_pArray outputShiftArray,
                                                                       xi_pTile3D outTile,
                                                                       const xi_cnna_conv_params *param);


_XI_APIREF_ XI_ERR_TYPE xiConvolvedAVQ3D_7x7_S8j2d1_MOW_DWH_DEPTH3_ref(const xi_pTile3D inTile,
                                                                       xi_pTile transposeTile,
                                                                       const xi_pTile4D coeffTile,
                                                                       const xi_pArray biasArray,
                                                                       const xi_pArray outputScaleArray,
                                                                       const xi_pArray outputShiftArray,
                                                                       xi_pTile3D outTile,
                                                                       const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedFixupA3D_3x3_U8S32_DWH_WHD_ref(const xi_pTile3D inTile, const xi_pTile4D coeffTile,
                                                                  const xi_pTile3D outTile, xi_pTile fixupTile,
                                                                  const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedFixupA3D_3x3_U8S32j2_DWH_WHD_DEPTH3_ref(const xi_pTile3D inTile, const xi_pTile4D coeffTile,
                                                                           const xi_pTile3D outTile, xi_pTile fixupTile,
                                                                           const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedFixupA3D_3x3_U8S32j2_DWH_WHD_DEPTH3_ref(const xi_pTile3D inTile, const xi_pTile4D coeffTile,
                                                                           const xi_pTile3D outTile, xi_pTile fixupTile,
                                                                           const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedFixupA3D_1x1_U8S32_DWH_WHD_ref(const xi_pTile3D inTile, const xi_pTile4D coeffTile,
                                                                  const xi_pTile3D outTile, xi_pTile fixupTile,
                                                                  const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedA3D_3x3_U8S8U8j1d1_MOW_ref(xi_tile3D *src_t, xi_tile *fixupTile, xi_tile4D *coeffTile,
                                                              xi_array *biasArray, xi_tile3D *dst_t, xi_cnna_conv_params *convParams);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedA3D_3x3_U8S8U8j2d1_MOW_DEPTH3_ref(xi_tile3D *src_t, xi_tile *fixupTile, xi_tile4D *coeffTile,
                                                                     xi_array *biasArray, xi_tile3D *dst_t, xi_cnna_conv_params *convParams);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedFixupA3D_7x7_U8S32j2_DWH_WHD_DEPTH3_ref(const xi_pTile3D inTile,
                                                                           const xi_pTile4D coeffTile,
                                                                           const xi_pTile3D outTile,
                                                                           xi_pTile fixupTile,
                                                                           const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedA3D_7x7_U8S8U8j2d1_MOW_DEPTH3_ref(xi_tile3D *src_t, xi_tile *fixupTile, xi_tile4D *coeffTile,
                                                                     xi_array *biasArray, xi_tile3D *dst_t, xi_cnna_conv_params *convParams);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedA3D_1x1_U8S8U8j1d1_MOW_ref(xi_tile3D *src_t, xi_tile *fixupTile, xi_tile4D *coeffTile,
                                                              xi_array *biasArray, xi_tile3D *dst_t, xi_cnna_conv_params *convParams);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedFixupA3D_QM32_MxN_U8S32_ref(const xi_pTile3D inTile,
                                                               const xi_pTile4D coeffTile, const xi_pTile3D outTile,
                                                               xi_pTile fixUpTile, const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedFixupA3D_InvQM24_3x3_U8S32_DWH_ref(const xi_pTile3D inTile, const xi_pTile4D coeffTile,
                                                                      const xi_pTile3D outTile, xi_pTile fixupTile,
                                                                      const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedA3D_QM24_MxN_U8S8U8_ref(const xi_pTile3D inTile,
                                                           const xi_pTile fixUpTile, const xi_pTile4D coeffTile,
                                                           const xi_pArray biasArray, xi_pTile3D outTile,
                                                           const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedA3D_QM32_MxN_U8S8U8_ref(const xi_pTile3D inTile,
                                                           const xi_pTile fixUpTile, const xi_pTile4D coeffTile,
                                                           const xi_pArray biasArray, xi_pTile3D outTile,
                                                           const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedA3D_QM24_U8U8U8_ref(xi_tile3D *src_t, xi_tile *fixupTile, xi_tile4D *coeffTile,
                                                       xi_array *biasArray, xi_tile3D *dst_t, xi_cnna_conv_params *convParams);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedAVQ3D_QM24_S8_DWH_ref(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         const xi_pArray outScaleArray,
                                                         const xi_pArray outShiftArray,
                                                         xi_pTile3D outTile,
                                                         const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedAVQ3D_QM32_S8_DWH_ref(const xi_pTile3D inTile,
                                                         const xi_pTile4D coeffTile,
                                                         const xi_pArray biasArray,
                                                         const xi_pArray outScaleArray,
                                                         const xi_pArray outShiftArray,
                                                         xi_pTile3D outTile,
                                                         const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedAVQ3D_S_MxN_S8_SO_DWH_ref(const xi_pTile3D inTile,
                                                             const xi_pTile4D coeffTile,
                                                             const xi_pArray biasArray,
                                                             const xi_pArray outScaleArray,
                                                             const xi_pArray outShiftArray,
                                                             xi_pTile3D outTile,
                                                             const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedAVQ3D_InvQM24_3x3_S8_DWH_ref(const xi_pTile3D inTile,
                                                                xi_pTile reorderTile,
                                                                const xi_pTile4D coeffTile,
                                                                const xi_pArray biasArray,
                                                                const xi_pArray outScaleArray,
                                                                const xi_pArray outShiftArray,
                                                                xi_pTile3D outTile,
                                                                const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedA3D_InvQM24_3x3_U8S8U8_ref(xi_tile3D *src_t, xi_tile *fixupTile, xi_tile4D *coeffTile,
                                                              xi_array *biasArray, xi_tile3D *dst_t, const xi_cnna_conv_params *convParams);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedA3D_QM32_U8U8U8_ref(xi_tile3D *src_t, xi_tile *fixupTile, xi_tile4D *coeffTile,
                                                       xi_array *biasArray, xi_tile3D *dst_t, xi_cnna_conv_params *convParams);

_XI_APIREF_ XI_ERR_TYPE xiDilatedConvolvedA3D_reff(const xi_pTile3D inTile, const xi_pTile4D coeffTile,
                                                   const xi_pArray biasArray, xi_pTile3D outTile,
                                                   xi_cnna_dilatedConv_params *param,
                                                   int32_t padWidth, int32_t padHeight);

_XI_APIREF_ XI_ERR_TYPE xiDilatedConvolvedAVQ3D_I8S8I8_reff(const xi_pTile3D inTile, const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,const xi_pArray scaleArray,
                                                            const xi_pArray shiftArray,xi_pTile3D outTile,
                                                            xi_cnna_dilatedConv_params *param,
                                                            int32_t padWidth,int32_t padHeight);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseDilatedConvolvedAVQ3D_I8S8I8_reff(const xi_pTile3D inTile,
                                                                     const xi_pTile3D coeffTile,
                                                                     const xi_pArray biasArray,
                                                                     const xi_pArray scaleArray,
                                                                     const xi_pArray shiftArray,
                                                                     xi_pTile3D outTile,
                                                                     xi_cnna_depthwiseDilatedConv_params *param,
                                                                     int32_t padWidth,
                                                                     int32_t padHeight);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseDilatedConvolvedA3D_reff(const xi_pTile3D inTile, const xi_pTile3D coeffTile,
                                                            const xi_pArray biasArray, xi_pTile3D outTile,
                                                            xi_cnna_depthwiseDilatedConv_params *param,
                                                            int32_t padWidth, int32_t padHeight);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseDilatedConvolvedAVQ3D_S8_DWH_ref(const xi_pTile3D inTile,
                                                                    const xi_pTile3D coeffTile,
                                                                    const xi_pArray biasArray,
                                                                    const xi_pArray scaleArray,
                                                                    const xi_pArray shiftArray,
                                                                    xi_pTile3D outTile,
                                                                    const xi_cnna_depthwiseDilatedConv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseMultiplierConvolvedAVQ3D_S8_DWH_ref(const xi_pTile3D inTile,
                                                                       const xi_pTile3D coeffTile,
                                                                       const xi_pArray biasArray,
                                                                       const xi_pArray scaleArray,
                                                                       const xi_pArray shiftArray,
                                                                       xi_pTile3D outTile,
                                                                       const xi_cnna_depthwiseDilatedConv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDilatedConvolvedA3D_ref(const xi_pTile3D inTile,
                                                  const xi_pTile4D coeffTile,
                                                  const xi_pArray biasArray,
                                                  xi_pTile3D outTile,
                                                  xi_cnna_dilatedConv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDilatedConvolvedAVQ3D_S8_DWH_ref(const xi_pTile3D inTile,
                                                           const xi_pTile4D coeffTile,
                                                           const xi_pArray biasArray,
                                                           const xi_pArray scaleArray,
                                                           const xi_pArray shiftArray,
                                                           xi_pTile3D outTile,
                                                           xi_cnna_dilatedConv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDilatedConvolvedAVQ3D_U8S8U8_DWH_ref(const xi_pTile3D inTile,
                                                           const xi_pTile4D coeffTile,
                                                           const xi_pArray biasArray,
                                                           const xi_pArray scaleArray,
                                                           const xi_pArray shiftArray,
                                                           xi_pTile3D outTile,
                                                           xi_cnna_dilatedConv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseDilatedConvolvedA3D_ref(const xi_pTile3D inTile,
                                                           const xi_pTile3D coeffTile,
                                                           const xi_pArray biasArray,
                                                           xi_pTile3D outTile,
                                                           const xi_cnna_depthwiseDilatedConv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiFullyConnectedA3D_S_U8_ref(const xi_pTile3D inTile,
                                                     const xi_pTile4D coeffTile,
                                                     const xi_pArray biasArray,
                                                     xi_pTile3D outTile,
                                                     const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiFullyConnectedA3D_S_S8_ref(const xi_pTile3D inTile,
                                                     const xi_pTile4D coeffTile,
                                                     const xi_pArray biasArray,
                                                     xi_pTile3D outTile,
                                                     const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE  xiFullyConnectedAVQ3D_S_S8_ref(const xi_pTile3D inTile,
                                                            const xi_pTile4D coeffTile,
                                                            const xi_pArray biasArray,
                                                            const xi_pArray outScaleArray,
                                                            const xi_pArray outShiftArray,
                                                            xi_pTile3D outTile,
                                                            const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiAvgPoolA3D_MxN_U8_DWH_ref(const xi_pTile3D inTile,
                                                    xi_pTile3D outTile,
                                                    const xi_cnn_pooling_params *param,
                                                    const xi_size3D frame3DSize);

_XI_APIREF_ XI_ERR_TYPE xiAvgPoolA3D_MxN_S8_DWH_ref(const xi_pTile3D inTile,
                                                    xi_pTile3D outTile,
                                                    const xi_cnn_pooling_params *param,
                                                    const xi_size3D frame3DSize);

_XI_APIREF_ XI_ERR_TYPE xiRELUA_U8_ref(const xi_pTile3D inTile,
                                       xi_pTile3D outTile,
                                       const uint8_t minVal,
                                       const uint8_t maxVal);

_XI_APIREF_ XI_ERR_TYPE xiRELUA_S8_ref(const xi_pTile3D inTile,
                                       xi_pTile3D outTile,
                                       const int8_t minVal,
                                       const int8_t maxVal);

_XI_APIREF_ XI_ERR_TYPE xiSubtractA3D_const128_U8S8_ref(const xi_pTile3D inTile,
                                                        xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiSubtractA3D_const128_I8_ref(const xi_pTile3D inTile,
                                                        xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiSubtractA4D_const128_U8S8_ref(const xi_pTile4D inTile,
                                                        xi_pTile4D outTile);

_XI_APIREF_ XI_ERR_TYPE xiSoftmaxA3D_U8_LUT_ref(const xi_pTile3D inTile, const xi_pArray softmaxArray, xi_pTile3D outTile,
                                                const xi_cnn_softmaxA_params * pparams);

_XI_APIREF_ XI_ERR_TYPE xiSoftmaxA3D_U8_ref(const xi_pTile3D inTile, const xi_pArray softmaxArray, xi_pTile3D outTile,
                                            const xi_cnn_softmaxA_params * pparams);

_XI_APIREF_ XI_ERR_TYPE xiSoftmaxA3D_S8_LUT_ref(const xi_pTile3D inTile, const xi_pArray softmaxArray, xi_pTile3D outTile,
                                                const xi_cnn_softmaxA_params * pparams);

_XI_APIREF_ XI_ERR_TYPE xiSoftmaxA3D_S8_ref(const xi_pTile3D inTile, const xi_pArray softmaxArray, xi_pTile3D outTile,
                                            const xi_cnn_softmaxA_params * pparams);


_XI_APIREF_ XI_ERR_TYPE xiSigmoidA3D_U8_LUT_ref(const xi_pTile3D inTile,
                                                const xi_pArray sigmoidArray,
                                                xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiSigmoidA3D_S8_LUT_ref(const xi_pTile3D inTile,
                                                const xi_pArray sigmoidArray,
                                                xi_pTile3D outTile);


_XI_APIREF_ XI_ERR_TYPE xiSigmoidA3D_U8_ref(const xi_pTile3D inTile,
                                            const xi_pArray sigmoidArray,
                                            xi_pTile3D outTile,
                                            const xi_cnn_sigmoidA_params* pparams);

_XI_APIREF_ XI_ERR_TYPE xiSigmoidA3D_S8_ref(const xi_pTile3D inTile,
                                            const xi_pArray sigmoidArray,
                                            xi_pTile3D outTile,
                                            const xi_cnn_sigmoidA_params* pparams);

_XI_APIREF_ XI_ERR_TYPE xiTanhA3D_U8_LUT_ref(const xi_pTile3D inTile,
                                             const xi_pArray tanhArray,
                                             xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiTanhA3D_S8_LUT_ref(const xi_pTile3D inTile,
                                             const xi_pArray tanhArray,
                                             xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiTanhA3D_U8_ref(const xi_pTile3D inTile,
                                         const xi_pArray tanhArray,
                                         xi_pTile3D outTile,
                                         const xi_cnn_tanhA_params* pparams);

_XI_APIREF_ XI_ERR_TYPE xiTanhA3D_S8_ref(const xi_pTile3D inTile,
                                         const xi_pArray tanhArray,
                                         xi_pTile3D outTile,
                                         const xi_cnn_tanhA_params* pparams);

_XI_APIREF_ XI_ERR_TYPE xiELUA3D_U8_LUT_ref(const xi_pTile3D inTile,
                                            const xi_pArray ELUArray,
                                            xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiELUA3D_U8_ref(const xi_pTile3D inTile,
                                        const xi_pArray ELUArray,
                                        xi_pTile3D outTile,
                                        const xi_cnn_ELUA_params* pparams);

_XI_APIREF_ XI_ERR_TYPE xiELUA3D_S8_LUT_ref(const xi_pTile3D inTile,
                                            const xi_pArray ELUArray,
                                            xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiELUA3D_S8_ref(const xi_pTile3D inTile,
                                        const xi_pArray ELUArray,
                                        xi_pTile3D outTile,
                                        const xi_cnn_ELUA_params* pparams);

_XI_APIREF_ XI_ERR_TYPE xiL2NormA3D_U8_DWH_ref(const xi_pTile3D inTile,
                                               xi_pTile3D outTile,
                                               const xi_cnn_l2NormA_params* pparams);

_XI_APIREF_ XI_ERR_TYPE xiL2NormA3D_S8_DWH_ref(const xi_pTile3D inTile,
                                               xi_pTile3D outTile,
                                               const xi_cnn_l2NormA_params* pparams);

_XI_APIREF_ XI_ERR_TYPE xiAddA3D_U8_ref(const xi_pTile3D inTile1,
                                        const xi_pTile3D inTile2,
                                        xi_pTile3D outTile,
                                        const xi_cnn_addA_params *pparams
                                        );

_XI_APIREF_ XI_ERR_TYPE xiAddA3D_S8_ref(const xi_pTile3D inTile1,
                                        const xi_pTile3D inTile2,
                                        xi_pTile3D outTile,
                                        const xi_cnn_addA_params *pparams
                                        );

_XI_APIREF_ XI_ERR_TYPE xiSubA3D_U8_ref(const xi_pTile3D inTile1,
                                        const xi_pTile3D inTile2,
                                        xi_pTile3D outTile,
                                        const xi_cnn_subA_params *pparams
                                        );

_XI_APIREF_ XI_ERR_TYPE xiSubA3D_S8_ref(const xi_pTile3D inTile1,
                                        const xi_pTile3D inTile2,
                                        xi_pTile3D outTile,
                                        const xi_cnn_subA_params *pparams
                                        );

_XI_APIREF_ XI_ERR_TYPE xiROIAlignQuantizeA3D_S8_DWH_ref(const xi_pTile3D inTile,
                                                         const xi_pArray RoIParam,
                                                         const xi_pArray batchSplitData,
                                                         xi_pTile4D outTile,
                                                         const xi_cnn_roi_align_params *pparams,
                                                         xi_pArray posWeightArr
                                                         );

_XI_APIREF_ XI_ERR_TYPE xiROIAlignQuantizeA3D_U8_DWH_ref(const xi_pTile3D inTile,
                                                         const xi_pArray RoIParam,
                                                         const xi_pArray batchSplitData,
                                                         xi_pTile4D outTile,
                                                         const xi_cnn_roi_align_params *pparams,
                                                         xi_pArray posWeightArr
                                                         );

_XI_APIREF_ XI_ERR_TYPE xiBoxWithNMSLimitA3D_U8_ref(const xi_pTile3D inBBoxTile,
                                                    const xi_pArray inBatchSplitData,
                                                    xi_pArray inScoreArray,
                                                    xi_pArray intermediateScoreArray,
                                                    xi_pArray sortedIdxArray,
                                                    xi_pArray outBatchSplitData,
                                                    xi_pArray outScoreArray,
                                                    xi_pArray outBBoxArray,
                                                    xi_pArray outClassArray,
                                                    const xi_cnn_box_with_NMS_limitA3D_params * pparams);

_XI_APIREF_ XI_ERR_TYPE xiBoxWithNMSLimitA3D_S8_ref(const xi_pTile3D inBBoxTile,
                                                    const xi_pArray inBatchSplitData,
                                                    xi_pArray inScoreArray,
                                                    xi_pArray intermediateScoreArray,
                                                    xi_pArray sortedIdxArray,
                                                    xi_pArray outBatchSplitData,
                                                    xi_pArray outScoreArray,
                                                    xi_pArray outBBoxArray,
                                                    xi_pArray outClassArray,
                                                    const xi_cnn_box_with_NMS_limitA3D_params * pparams);

_XI_APIREF_ XI_ERR_TYPE xiReduceProdA3D_U8_ref(const xi_pTile3D inTile,
                                               xi_pArray intermediateArray,
                                               xi_pTile3D outTile,
                                               xi_cnn_reduceProdA_params *params
                                               );

_XI_APIREF_ XI_ERR_TYPE xiReduceProdA4D_U8_ref(const xi_pTile4D inTile,
                                               xi_pArray intermediateArray,
                                               xi_pTile4D outTile,
                                               xi_cnn_reduceProdA_params *params
                                               );

_XI_APIREF_ XI_ERR_TYPE xiReduceMaxA3D_U8_ref(const xi_pTile3D inTile,
                                              xi_pTile3D outTile,
                                              const xi_cnn_reduceA_params *params);

_XI_APIREF_ XI_ERR_TYPE xiReduceMaxA3D_S8_ref(const xi_pTile3D inTile,
                                              xi_pTile3D outTile,
                                              const xi_cnn_reduceA_params *params);

_XI_APIREF_ XI_ERR_TYPE xiReduceMaxA4D_U8_ref(const xi_pTile4D inTile,
                                              xi_pTile4D outTile,
                                              const xi_cnn_reduceA_params *params);

_XI_APIREF_ XI_ERR_TYPE xiReduceMaxA4D_S8_ref(const xi_pTile4D inTile,
                                              xi_pTile4D outTile,
                                              const xi_cnn_reduceA_params *params);

_XI_APIREF_ XI_ERR_TYPE xiReduceMinA3D_U8_ref(const xi_pTile3D inTile,
                                              xi_pTile3D outTile,
                                              const xi_cnn_reduceA_params *params);

_XI_APIREF_ XI_ERR_TYPE xiReduceMinA3D_S8_ref(const xi_pTile3D inTile,
                                              xi_pTile3D outTile,
                                              const xi_cnn_reduceA_params *params);

_XI_APIREF_ XI_ERR_TYPE xiReduceMinA4D_U8_ref(const xi_pTile4D inTile,
                                              xi_pTile4D outTile,
                                              const xi_cnn_reduceA_params *params);

_XI_APIREF_ XI_ERR_TYPE xiReduceMinA4D_S8_ref(const xi_pTile4D inTile,
                                              xi_pTile4D outTile,
                                              const xi_cnn_reduceA_params *params);

_XI_APIREF_ XI_ERR_TYPE xiReduceAnyA3D_U8_ref(const xi_pTile3D inTile,
                                              xi_pTile3D outTile,
                                              const xi_cnn_reduceA_params *params);

_XI_APIREF_ XI_ERR_TYPE xiReduceAnyA3D_S8_ref(const xi_pTile3D inTile,
                                              xi_pTile3D outTile,
                                              const xi_cnn_reduceA_params *params);

_XI_APIREF_ XI_ERR_TYPE xiReduceAnyA4D_U8_ref(const xi_pTile4D inTile,
                                              xi_pTile4D outTile,
                                              const xi_cnn_reduceA_params *params);

_XI_APIREF_ XI_ERR_TYPE xiReduceAnyA4D_S8_ref(const xi_pTile4D inTile,
                                              xi_pTile4D outTile,
                                              const xi_cnn_reduceA_params *params);

_XI_APIREF_ XI_ERR_TYPE xiAxisAlignedBboxTransformA3D_U8_ref(const xi_pArray RoIParam,
                                                             const xi_pTile3D BBoxDeltaTile,
                                                             const xi_pArray batchSplitData,
                                                             xi_pArray lutArray,
                                                             xi_pTile3D outBoxTile,
                                                             const xi_cnn_axis_aligned_bbox_transformA3D_params * pparams);

_XI_APIREF_ XI_ERR_TYPE xiAxisAlignedBboxTransformA3D_U8_LUT_ref(const xi_pArray RoIParam,
                                                                 const xi_pTile3D BBoxDeltaTile,
                                                                 const xi_pArray batchSplitData,
                                                                 const xi_pArray lutArray,
                                                                 xi_pTile3D outBoxTile,
                                                                 const xi_cnn_axis_aligned_bbox_transformA3D_params * pparams);

_XI_APIREF_ XI_ERR_TYPE xiAxisAlignedBboxTransformA3D_S8_ref(const xi_pArray RoIParam,
                                                             const xi_pTile3D BBoxDeltaTile,
                                                             const xi_pArray batchSplitData,
                                                             xi_pArray lutArray,
                                                             xi_pTile3D outBoxTile,
                                                             const xi_cnn_axis_aligned_bbox_transformA3D_params * pparams);

_XI_APIREF_ XI_ERR_TYPE xiAxisAlignedBboxTransformA3D_S8_LUT_ref(const xi_pArray RoIParam,
                                                                 const xi_pTile3D BBoxDeltaTile,
                                                                 const xi_pArray batchSplitData,
                                                                 const xi_pArray lutArray,
                                                                 xi_pTile3D outBoxTile,
                                                                 const xi_cnn_axis_aligned_bbox_transformA3D_params * pparams);

_XI_APIREF_ XI_ERR_TYPE xiGenerateProposalsA3D_U8_ref(const xi_pTile3D inScoreTile,
                                                      const xi_pTile4D bBoxDeltaTile,
                                                      xi_pArray batchSplitData,
                                                      const xi_pArray anchorArray,
                                                      xi_pArray RoIParam,
                                                      xi_pTile3D intermediateBBoxTile,
                                                      xi_pArray sortedIdxArray,
                                                      xi_pArray outBBoxArray,
                                                      xi_pArray outScoreArray,
                                                      const xi_cnn_generate_proposalsA3D_I8_params* pparams);

_XI_APIREF_ XI_ERR_TYPE xiGenerateProposalsA3D_S8_ref(const xi_pTile3D inScoreTile,
                                                      const xi_pTile4D bBoxDeltaTile,
                                                      xi_pArray batchSplitData,
                                                      const xi_pArray anchorArray,
                                                      xi_pArray RoIParam,
                                                      xi_pTile3D intermediateBBoxTile,
                                                      xi_pArray sortedIdxArray,
                                                      xi_pArray outBBoxArray,
                                                      xi_pArray outScoreArray,
                                                      const xi_cnn_generate_proposalsA3D_I8_params* pparams);

_XI_APIREF_ XI_ERR_TYPE xiEltwiseMaxA3D_U8_ref(const xi_pTile3D inTile1, const xi_pTile3D inTile2,
                                               xi_pTile3D outTile, xi_pTile3D outmaskTile, const xi_cnn_eltwisemaxA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiEltwiseMaxA3D_S8_ref(const xi_pTile3D inTile1, const xi_pTile3D inTile2,
                                               xi_pTile3D outTile, xi_pTile3D outmaskTile, const xi_cnn_eltwisemaxA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiEltwiseMinA3D_U8_ref(const xi_pTile3D inTile1,
                                               const xi_pTile3D inTile2,
                                               xi_pTile3D outTile,
                                               xi_pTile3D outmaskTile,
                                               const xi_cnn_eltwiseminA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiEltwiseMinA3D_S8_ref(const xi_pTile3D inTile1,
                                               const xi_pTile3D inTile2,
                                               xi_pTile3D outTile,
                                               xi_pTile3D outmaskTile,
                                               const xi_cnn_eltwiseminA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiEltwiseGreaterA3D_U8_ref(const xi_pTile3D inTile1,
                                                   const xi_pTile3D inTile2,
                                                   xi_pTile3D outBool,
                                                   const xi_cnn_eltwiseCompareAParams *pparams);

_XI_APIREF_ XI_ERR_TYPE xiEltwiseGreaterOrEqualA3D_U8_ref(const xi_pTile3D inTile1,
                                                          const xi_pTile3D inTile2,
                                                          xi_pTile3D outBool,
                                                          const xi_cnn_eltwiseCompareAParams *pparams);

_XI_APIREF_ XI_ERR_TYPE xiEltwiseLesserA3D_U8_ref(const xi_pTile3D inTile1,
                                                  const xi_pTile3D inTile2,
                                                  xi_pTile3D outBool,
                                                  const xi_cnn_eltwiseCompareAParams *pparams);

_XI_APIREF_ XI_ERR_TYPE xiEltwiseLesserOrEqualA3D_U8_ref(const xi_pTile3D inTile1,
                                                         const xi_pTile3D inTile2,
                                                         xi_pTile3D outBool,
                                                         const xi_cnn_eltwiseCompareAParams *pparams);

_XI_APIREF_ XI_ERR_TYPE xiEltwiseGreaterA3D_S8_ref(const xi_pTile3D inTile1,
                                                   const xi_pTile3D inTile2,
                                                   xi_pTile3D outBool,
                                                   const xi_cnn_eltwiseCompareAParams *pparams);

_XI_APIREF_ XI_ERR_TYPE xiEltwiseGreaterOrEqualA3D_S8_ref(const xi_pTile3D inTile1,
                                                          const xi_pTile3D inTile2,
                                                          xi_pTile3D outBool,
                                                          const xi_cnn_eltwiseCompareAParams *pparams);

_XI_APIREF_ XI_ERR_TYPE xiEltwiseLesserA3D_S8_ref(const xi_pTile3D inTile1,
                                                  const xi_pTile3D inTile2,
                                                  xi_pTile3D outBool,
                                                  const xi_cnn_eltwiseCompareAParams *pparams);

_XI_APIREF_ XI_ERR_TYPE xiEltwiseLesserOrEqualA3D_S8_ref(const xi_pTile3D inTile1,
                                                         const xi_pTile3D inTile2,
                                                         xi_pTile3D outBool,
                                                         const xi_cnn_eltwiseCompareAParams *pparams);

_XI_APIREF_ XI_ERR_TYPE xiEltwiseEqualA3D_U8_ref(const xi_pTile3D inTile1,
                                                 const xi_pTile3D inTile2,
                                                 xi_pTile3D outBool,
                                                 const xi_cnn_eltwiseCompareAParams *pparams);

_XI_APIREF_ XI_ERR_TYPE xiEltwiseEqualA3D_S8_ref(const xi_pTile3D inTile1,
                                                 const xi_pTile3D inTile2,
                                                 xi_pTile3D outBool,
                                                 const xi_cnn_eltwiseCompareAParams *pparams);

_XI_APIREF_ XI_ERR_TYPE xiEltwiseNotEqualA3D_U8_ref(const xi_pTile3D inTile1,
                                                    const xi_pTile3D inTile2,
                                                    xi_pTile3D outBool,
                                                    const xi_cnn_eltwiseCompareAParams *pparams);

_XI_APIREF_ XI_ERR_TYPE xiEltwiseNotEqualA3D_S8_ref(const xi_pTile3D inTile1,
                                                    const xi_pTile3D inTile2,
                                                    xi_pTile3D outBool,
                                                    const xi_cnn_eltwiseCompareAParams *pparams);

/* S32 */
_XI_APIREF_ XI_ERR_TYPE xiEltwiseGreaterA3D_S32_ref(const xi_pTile3D inTile1,
                                                    const xi_pTile3D inTile2,
                                                    xi_pTile3D outBool);

_XI_APIREF_ XI_ERR_TYPE xiEltwiseGreaterOrEqualA3D_S32_ref(const xi_pTile3D inTile1,
                                                           const xi_pTile3D inTile2,
                                                           xi_pTile3D outBool);

_XI_APIREF_ XI_ERR_TYPE xiEltwiseLesserA3D_S32_ref(const xi_pTile3D inTile1,
                                                   const xi_pTile3D inTile2,
                                                   xi_pTile3D outBool);

_XI_APIREF_ XI_ERR_TYPE xiEltwiseLesserOrEqualA3D_S32_ref(const xi_pTile3D inTile1,
                                                          const xi_pTile3D inTile2,
                                                          xi_pTile3D outBool);

_XI_APIREF_ XI_ERR_TYPE xiEltwiseEqualA3D_S32_ref(const xi_pTile3D inTile1,
                                                  const xi_pTile3D inTile2,
                                                  xi_pTile3D outBool);

_XI_APIREF_ XI_ERR_TYPE xiEltwiseNotEqualA3D_S32_ref(const xi_pTile3D inTile1,
                                                     const xi_pTile3D inTile2,
                                                     xi_pTile3D outBool);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastEltwiseMaxA3D_U8_ref(const xi_pTile3D inTile1,
                                                        const xi_pTile3D inTile2,
                                                        xi_pTile3D outTile,
                                                        xi_pTile3D outmaskTile,
                                                        const xi_cnn_eltwisemaxA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastEltwiseMaxA3D_S8_ref(const xi_pTile3D inTile1,
                                                        const xi_pTile3D inTile2,
                                                        xi_pTile3D outTile,
                                                        xi_pTile3D outmaskTile,
                                                        const xi_cnn_eltwisemaxA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastEltwiseMinA3D_U8_ref(const xi_pTile3D inTile1,
                                                        const xi_pTile3D inTile2,
                                                        xi_pTile3D outTile,
                                                        xi_pTile3D outmaskTile,
                                                        const xi_cnn_eltwiseminA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastEltwiseMinA3D_S8_ref(const xi_pTile3D inTile1,
                                                        const xi_pTile3D inTile2,
                                                        xi_pTile3D outTile,
                                                        xi_pTile3D outmaskTile,
                                                        const xi_cnn_eltwiseminA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastLogicalAndA3D_I8_ref(const xi_pTile3D inBoolTile1,
                                                        const xi_pTile3D inBoolTile2,
                                                        xi_pTile3D outBoolTile);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastLogicalOrA3D_I8_ref(const xi_pTile3D inBoolTile1,
                                                       const xi_pTile3D inBoolTile2,
                                                       xi_pTile3D outBoolTile);

_XI_APIREF_ XI_ERR_TYPE xiL2Pool2DA_U8_DWH_ref(const xi_pTile3D inTile, xi_pTile3D outTile,
                                               const xi_cnn_l2pool2D_params *param);

_XI_APIREF_ XI_ERR_TYPE xiL2Pool2DA_S8_DWH_ref(const xi_pTile3D inTile, xi_pTile3D outTile,
                                               const xi_cnn_l2pool2D_params *param);

_XI_APIREF_ XI_ERR_TYPE xiL2Pool2DQuantizeA_U8_DWH_ref(const xi_pTile3D inTile, xi_pTile3D outTile,
                                                       const xi_cnn_l2pool2D_params *param);

_XI_APIREF_ XI_ERR_TYPE xiL2Pool2DQuantizeA_S8_DWH_ref(const xi_pTile3D inTile, xi_pTile3D outTile,
                                                       const xi_cnn_l2pool2D_params *param);

_XI_APIREF_ XI_ERR_TYPE xiMeanA_U8_ref(const xi_pTile3D inTile, xi_pTile3D outTile, xi_pArray accSumArray,
                                       const xi_cnn_meanA_params *param);

_XI_APIREF_ XI_ERR_TYPE xiMeanA_S8_ref(const xi_pTile3D inTile, xi_pTile3D outTile, xi_pArray accSumArray,
                                       const xi_cnn_meanA_params *param);

_XI_APIREF_ XI_ERR_TYPE xiMeanQuantizeA_U8_ref(const xi_pTile3D inTile, xi_pTile3D outTile, xi_pArray accSumArray,
                                               const xi_cnn_meanA_params *param);

_XI_APIREF_ XI_ERR_TYPE xiMeanQuantizeA_S8_ref(const xi_pTile3D inTile, xi_pTile3D outTile, xi_pArray accSumArray,
                                               const xi_cnn_meanA_params *param);

_XI_APIREF_ XI_ERR_TYPE xiLSTMA_U8_ref(const xi_pTile4D inTile, const xi_pTile4D outActiveTile, const xi_pTile4D prevActiveTile,                                    \
                                    const xi_pTile4D weightsTile, const xi_pArray biasArray, const xi_pTile4D prevStateInTile, const xi_pTile4D outputStateTile, \
                                    xi_pTile4D concatTile, xi_pTile4D scratchTile, const xi_cnna_lstmA_params *param);

_XI_APIREF_ XI_ERR_TYPE xiLSTMA_S8_ref(const xi_pTile4D inTile, const xi_pTile4D outActiveTile, const xi_pTile4D prevActiveTile,                                    \
                                    const xi_pTile4D weightsTile, const xi_pArray biasArray, const xi_pTile4D prevStateInTile, const xi_pTile4D outputStateTile, \
                                    xi_pTile4D concatTile, xi_pTile4D scratchTile, const xi_cnna_lstmA_params *param);

_XI_APIREF_ XI_ERR_TYPE xiLSTMA_ref(const xi_pTile4D inTile, const xi_pTile4D outActiveTile, const xi_pTile4D prevActiveTile,                                    \
                                    const xi_pTile4D weightsTile, const xi_pArray biasArray, const xi_pTile4D prevStateInTile, const xi_pTile4D outputStateTile, \
                                    xi_pTile4D concatTile, xi_pTile4D scratchTile, const xi_cnna_lstmA_params *param);


_XI_APIREF_ XI_ERR_TYPE xiMean4DA_U8_ref(const xi_pTile4D inTile, xi_pTile4D outTile, xi_pArray accSumArray,
                                         const xi_cnn_meanA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiMean4DA_S8_ref(const xi_pTile4D inTile, xi_pTile4D outTile, xi_pArray accSumArray,
                                         const xi_cnn_meanA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiMean4DQuantizeA_U8_ref(const xi_pTile4D inTile, xi_pTile4D outTile, xi_pArray accSumArray,
                                                 const xi_cnn_meanA_params *param);

_XI_APIREF_ XI_ERR_TYPE xiMean4DQuantizeA_S8_ref(const xi_pTile4D inTile, xi_pTile4D outTile, xi_pArray accSumArray,
                                                 const xi_cnn_meanA_params *param);

_XI_APIREF_ XI_ERR_TYPE xiStridedSliceA_U8_ref(const xi_pTile3D inTile, xi_pTile3D outTile,
                                               const xi_cnn_stridedsliceA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiStridedSliceA_S8_ref(const xi_pTile3D inTile, xi_pTile3D outTile,
                                               const xi_cnn_stridedsliceA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiMaxPoolQuantizeA_U8_DWH_ref(const xi_pTile3D inTile, xi_pTile3D outTile,
                                                      const xi_cnn_maxpoolA_params *param);

_XI_APIREF_ XI_ERR_TYPE xiMaxPoolQuantizeA_S8_DWH_ref(const xi_pTile3D inTile, xi_pTile3D outTile,
                                                      const xi_cnn_maxpoolA_params *param);

_XI_APIREF_ XI_ERR_TYPE xiAvgPoolQuantizeA_U8_DWH_ref(const xi_pTile3D inTile,
                                                      xi_pTile3D outTile,
                                                      const xi_cnn_avgpoolA_params *param,
                                                      const xi_size3D frame3DSize
                                                      );

_XI_APIREF_ XI_ERR_TYPE xiAvgPoolQuantizeA_S8_DWH_ref(const xi_pTile3D inTile,
                                                      xi_pTile3D outTile,
                                                      const xi_cnn_avgpoolA_params *param,
                                                      const xi_size3D frame3DSize
                                                      );

_XI_APIREF_ XI_ERR_TYPE xiMaxPoolA3D_MxN_U8_DWH_ref(const xi_pTile3D inTile, xi_pTile3D outTile, const xi_cnn_maxpoolA_params *param);

_XI_APIREF_ XI_ERR_TYPE xiMaxPoolA3D_MxN_S8_DWH_ref(const xi_pTile3D inTile, xi_pTile3D outTile, const xi_cnn_maxpoolA_params *param);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastAddA3D_U8_ref(const xi_pTile3D inTile1,
                                                 const xi_pTile3D inTile2,
                                                 xi_pTile3D outTile,
                                                 const xi_cnn_addA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastAddA3D_S8_ref(const xi_pTile3D inTile1,
                                                 const xi_pTile3D inTile2,
                                                 xi_pTile3D outTile,
                                                 const xi_cnn_addA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastSubA3D_U8_ref(const xi_pTile3D inTile1,
                                                 const xi_pTile3D inTile2,
                                                 xi_pTile3D outTile,
                                                 const xi_cnn_subA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastSubA3D_S8_ref(const xi_pTile3D inTile1,
                                                 const xi_pTile3D inTile2,
                                                 xi_pTile3D outTile,
                                                 const xi_cnn_subA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastEltwiseGreaterA3D_U8_ref(const xi_pTile3D inTile1,
                                                            const xi_pTile3D inTile2,
                                                            xi_pTile3D outBool,
                                                            const xi_cnn_eltwiseCompareAParams *pparams);
_XI_APIREF_ XI_ERR_TYPE xiBroadcastEltwiseGreaterOrEqualA3D_U8_ref(const xi_pTile3D inTile1,
                                                                   const xi_pTile3D inTile2,
                                                                   xi_pTile3D outBool,
                                                                   const xi_cnn_eltwiseCompareAParams *pparams);
_XI_APIREF_ XI_ERR_TYPE xiBroadcastEltwiseLesserA3D_U8_ref(const xi_pTile3D inTile1,
                                                           const xi_pTile3D inTile2,
                                                           xi_pTile3D outBool,
                                                           const xi_cnn_eltwiseCompareAParams *pparams);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastEltwiseLesserOrEqualA3D_U8_ref(const xi_pTile3D inTile1,
                                                                  const xi_pTile3D inTile2,
                                                                  xi_pTile3D outBool,
                                                                  const xi_cnn_eltwiseCompareAParams *pparams);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastEltwiseGreaterA3D_S8_ref(const xi_pTile3D inTile1,
                                                            const xi_pTile3D inTile2,
                                                            xi_pTile3D outBool,
                                                            const xi_cnn_eltwiseCompareAParams *pparams);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastEltwiseGreaterOrEqualA3D_S8_ref(const xi_pTile3D inTile1,
                                                                   const xi_pTile3D inTile2,
                                                                   xi_pTile3D outBool,
                                                                   const xi_cnn_eltwiseCompareAParams *pparams);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastEltwiseLesserA3D_S8_ref(const xi_pTile3D inTile1,
                                                           const xi_pTile3D inTile2,
                                                           xi_pTile3D outBool,
                                                           const xi_cnn_eltwiseCompareAParams *pparams);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastEltwiseLesserOrEqualA3D_S8_ref(const xi_pTile3D inTile1,
                                                                  const xi_pTile3D inTile2,
                                                                  xi_pTile3D outBool,
                                                                  const xi_cnn_eltwiseCompareAParams *pparams);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastEltwiseEqualA3D_U8_ref(const xi_pTile3D inTile1,
                                                          const xi_pTile3D inTile2,
                                                          xi_pTile3D outBool,
                                                          const xi_cnn_eltwiseCompareAParams *pparams);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastEltwiseEqualA3D_S8_ref(const xi_pTile3D inTile1,
                                                          const xi_pTile3D inTile2,
                                                          xi_pTile3D outBool,
                                                          const xi_cnn_eltwiseCompareAParams *pparams);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastEltwiseNotEqualA3D_U8_ref(const xi_pTile3D inTile1,
                                                             const xi_pTile3D inTile2,
                                                             xi_pTile3D outBool,
                                                             const xi_cnn_eltwiseCompareAParams *pparams);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastEltwiseNotEqualA3D_S8_ref(const xi_pTile3D inTile1,
                                                             const xi_pTile3D inTile2,
                                                             xi_pTile3D outBool,
                                                             const xi_cnn_eltwiseCompareAParams *pparams);
/* S32 */
_XI_APIREF_ XI_ERR_TYPE xiBroadcastEltwiseGreaterA3D_S32_ref(const xi_pTile3D inTile1,
                                                             const xi_pTile3D inTile2,
                                                             xi_pTile3D outBool);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastEltwiseGreaterOrEqualA3D_S32_ref(const xi_pTile3D inTile1,
                                                                    const xi_pTile3D inTile2,
                                                                    xi_pTile3D outBool);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastEltwiseLesserA3D_S32_ref(const xi_pTile3D inTile1,
                                                            const xi_pTile3D inTile2,
                                                            xi_pTile3D outBool);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastEltwiseLesserOrEqualA3D_S32_ref(const xi_pTile3D inTile1,
                                                                   const xi_pTile3D inTile2,
                                                                   xi_pTile3D outBool);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastEltwiseEqualA3D_S32_ref(const xi_pTile3D inTile1,
                                                           const xi_pTile3D inTile2,
                                                           xi_pTile3D outBool);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastEltwiseNotEqualA3D_S32_ref(const xi_pTile3D inTile1,
                                                              const xi_pTile3D inTile2,
                                                              xi_pTile3D outBool);

_XI_APIREF_ XI_ERR_TYPE xiMulA3D_U8_ref(
  const xi_pTile3D inTile1, const xi_pTile3D inTile2, xi_pTile3D outTile,
  const xi_cnn_mulA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiMulA3D_S8_ref(
  const xi_pTile3D inTile1, const xi_pTile3D inTile2, xi_pTile3D outTile,
  const xi_cnn_mulA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastMulA3D_U8_ref(
  const xi_pTile3D inTile1, const xi_pTile3D inTile2, xi_pTile3D outTile,
  const xi_cnn_mulA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastMulA3D_S8_ref(
  const xi_pTile3D inTile1, const xi_pTile3D inTile2, xi_pTile3D outTile,
  const xi_cnn_mulA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiSpaceToDepthA3D_I8_DWH_ref(const xi_pTile3D inTile, int blockSize, xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiDepthToSpaceA3D_I8_DWH_ref(const xi_pTile3D inTile, int blockSize, xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiHashtableLookupA3D_IX_ref(const xi_pArray lookupArray, const xi_pArray keysArray,
                                                    const xi_pTile3D inTile, xi_pTile3D outTile, xi_pArray outHitsArray);

_XI_APIREF_ XI_ERR_TYPE xiPermuteA3D_I8_ref(const xi_pTile3D inTile, xi_pTile3D outTile, const xi_cnn_permuteA3D_params * pparams);

_XI_APIREF_ XI_ERR_TYPE xiPermuteA4D_I8_ref(const xi_pTile4D inTile, xi_pTile4D outTile, const xi_cnn_permuteA4D_params * pparams);

_XI_APIREF_ XI_ERR_TYPE xiCropA3D_I8_ref(const xi_pTile3D inTile, xi_pTile3D outTile, const uint32_t start0, const uint32_t start1, const uint32_t start2);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolveA2D_S_3x3_U8Ca2_MOD_DWH_ref(const xi_pTile3D inTile,
                                                                       const xi_pTile3D coeffTile,
                                                                       const xi_pArray biasArray,
                                                                       xi_pTile3D outTile,
                                                                       const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolveAVQ2D_S_3x3_S8Ca2_MOD_DWH_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         const xi_pArray outScaleArray,
                                                                         const xi_pArray outShiftArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolveA2D_S_5x5_U8Ca2_MOD_DWH_ref(const xi_pTile3D inTile,
                                                                       const xi_pTile3D coeffTile,
                                                                       const xi_pArray biasArray,
                                                                       xi_pTile3D outTile,
                                                                       const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolveAVQ2D_S_5x5_S8Ca2_MOD_DWH_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         const xi_pArray outScaleArray,
                                                                         const xi_pArray outShiftArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolveA2D_S_7x7_U8Ca2_MOD_DWH_ref(const xi_pTile3D inTile,
                                                                       const xi_pTile3D coeffTile,
                                                                       const xi_pArray biasArray,
                                                                       xi_pTile3D outTile,
                                                                       const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolveAVQ2D_S_7x7_S8Ca2_MOD_DWH_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         const xi_pArray outScaleArray,
                                                                         const xi_pArray outShiftArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolveA2D_S_MxN_U8Ca2_MOD_DWH_ref(const xi_pTile3D inTile,
                                                                       const xi_pTile3D coeffTile,
                                                                       const xi_pArray biasArray,
                                                                       xi_pTile3D outTile,
                                                                       const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolveAVQ2D_S_MxN_S8Ca2_MOD_DWH_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         const xi_pArray outScaleArray,
                                                                         const xi_pArray outShiftArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolveAVQ2D_ref(const xi_pTile3D inTile,
                                                     const xi_pTile3D coeffTile,
                                                     const xi_pArray biasArray,
                                                     const xi_pArray outScaleArray,
                                                     const xi_pArray outShiftArray,
                                                     xi_pTile3D outTile,
                                                     const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolveA2D_ref(const xi_pTile3D inTile,
                                                   const xi_pTile3D coeffTile,
                                                   const xi_pArray biasArray,
                                                   xi_pTile3D outTile,
                                                   const xi_cnna_conv_params *param);


_XI_APIREF_ XI_ERR_TYPE xiConvolveAVQ3D_S_MxN_I8S8I8_reff(const xi_pTile3D inTile,
                                                          const xi_pTile4D coeffTile,
                                                          const xi_pArray biasArray,
                                                          const xi_pArray outScaleArray,
                                                          const xi_pArray outShiftArray,
                                                          xi_pTile3D outTile,
                                                          xi_cnna_conv_params *param,
                                                          const int32_t numGroups,
                                                          const int8_t edgeExtendLeft,
                                                          const int8_t edgeExtendRight,
                                                          const int8_t edgeExtendTop,
                                                          const int8_t edgeExtendBottom,
                                                          const xi_pArray edgeExtendValArray);


_XI_APIREF_ XI_ERR_TYPE xiConvolvedAVQ3D_S_MxN_I8S8I8_reff(const xi_pTile3D inTile,
                                                           const xi_pTile4D coeffTile,
                                                           const xi_pArray biasArray,
                                                           const xi_pArray outScaleArray,
                                                           const xi_pArray outShiftArray,
                                                           xi_pTile3D outTile,
                                                           const xi_cnna_conv_params *param,
                                                           const int32_t numGroups,
                                                           const int8_t edgeExtendLeft,
                                                           const int8_t edgeExtendRight,
                                                           const int8_t edgeExtendTop,
                                                           const int8_t edgeExtendBottom,
                                                           const xi_pArray edgeExtendValArray);

_XI_APIREF_ XI_ERR_TYPE xiAvgPoolA3D_MxN_I8_reff(const xi_pTile3D inTile,
                                                 xi_pTile3D outTile,
                                                 const xi_cnn_pooling_params *param,
                                                 const xi_size3D frame3DSize,
                                                 const int8_t edgeExtendLeft,
                                                 const int8_t edgeExtendRight,
                                                 const int8_t edgeExtendTop,
                                                 const int8_t edgeExtendBottom);

_XI_APIREF_ XI_ERR_TYPE xiL2PoolA2D_MxN_I8_reff(const xi_pTile3D inTile,
                                                xi_pTile3D outTile,
                                                const xi_cnn_l2pool2D_params *param,
                                                const xi_size3D frame3DSize,
                                                const int8_t edgeExtendLeft,
                                                const int8_t edgeExtendRight,
                                                const int8_t edgeExtendTop,
                                                const int8_t edgeExtendBottom);

_XI_APIREF_ XI_ERR_TYPE xiMaxPoolA3D_MxN_I8_reff(const xi_pTile3D inTile,
                                                 xi_pTile3D outTile,
                                                 const xi_cnn_maxpoolA_params *param,
                                                 const int8_t edgeExtendLeft,
                                                 const int8_t edgeExtendRight,
                                                 const int8_t edgeExtendTop,
                                                 const int8_t edgeExtendBottom);

_XI_APIREF_ XI_ERR_TYPE xiAvgPoolQuantizeA3D_MxN_I8_reff(const xi_pTile3D inTile,
                                                         xi_pTile3D outTile,
                                                         const xi_cnn_avgpoolA_params *param,
                                                         const xi_size3D frame3DSize,
                                                         const int8_t edgeExtendLeft,
                                                         const int8_t edgeExtendRight,
                                                         const int8_t edgeExtendTop,
                                                         const int8_t edgeExtendBottom);

_XI_APIREF_ XI_ERR_TYPE xiL2PoolAQuantize2D_MxN_I8_reff(const xi_pTile3D inTile,
                                                        xi_pTile3D outTile,
                                                        const xi_cnn_l2pool2D_params *param,
                                                        const int8_t edgeExtendLeft,
                                                        const int8_t edgeExtendRight,
                                                        const int8_t edgeExtendTop,
                                                        const int8_t edgeExtendBottom);

_XI_APIREF_ XI_ERR_TYPE xiMaxPoolQuantizeA3D_MxN_I8_reff(const xi_pTile3D inTile,
                                                         xi_pTile3D outTile,
                                                         const xi_cnn_maxpoolA_params *param,
                                                         const int8_t edgeExtendLeft,
                                                         const int8_t edgeExtendRight,
                                                         const int8_t edgeExtendTop,
                                                         const int8_t edgeExtendBottom);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolveA2D_S_MxN_U8_reff(const xi_pTile3D inTile,
                                                             const xi_pTile3D coeffTile,
                                                             const xi_pArray biasArray,
                                                             xi_pTile3D outTile,
                                                             const xi_cnna_conv_params *param,
                                                             const int8_t edgeExtendLeft,
                                                             const int8_t edgeExtendRight,
                                                             const int8_t edgeExtendTop,
                                                             const int8_t edgeExtendBottom,
                                                             const xi_pArray edgeExtendValArray);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolveAVQ2D_S_MxN_I8S8I8_reff(const xi_pTile3D inTile,
                                                                   const xi_pTile3D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   const xi_pArray outScaleArray,
                                                                   const xi_pArray outShiftArray,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnna_conv_params *param,
                                                                   const int8_t edgeExtendLeft,
                                                                   const int8_t edgeExtendRight,
                                                                   const int8_t edgeExtendTop,
                                                                   const int8_t edgeExtendBottom,
                                                                   const xi_pArray edgeExtendValArray);

_XI_APIREF_ XI_ERR_TYPE xiConvertIndicesA3D_WHD_DWH_ref(const xi_pTile3D inTileWHD,
                                                        const xi_pTile3D inTileDWH,
                                                        const xi_pArray idxWHDArray,
                                                        xi_pArray idxDWHArray);

_XI_APIREF_ XI_ERR_TYPE xiGatherA3D_I8_ref(const xi_pTile3D inTile,
                                           const xi_pArray indxArray,
                                           xi_pArray outArray);

_XI_APIREF_ XI_ERR_TYPE xiGatherConvertIndicesA3D_ref(const xi_pTile3D inTile,
                                                       const xi_pArray inIndxArray,
                                                       xi_pArray outIndxArray,
                                                       const xi_cnn_GatherA_params *params);

_XI_APIREF_ XI_ERR_TYPE xiEmbeddingLookupA3D_I8_Dim1_ref(const xi_pTile3D inTile,
                                                         const xi_pArray indxArray,
                                                         xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiEmbeddingLookupA3D_I8_Dim2_ref(const xi_pTile3D inTile,
                                                         const xi_pArray indxArray,
                                                         xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiEmbeddingLookupA3D_I8_Dim3_ref(const xi_pTile3D inTile,
                                                         const xi_pArray indxArray,
                                                         xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiDeconvolveA3D_MxN_U8U8U8_MOD_DWH_ref(xi_tile3D *inTile, xi_pTile fixUpTile, xi_array *biasArray,
                                                               xi_tile3D *outTile, xi_cnn_deconvolveA_params *params,
                                                               xi_tile4D *newCoeffTile, xi_array *constTermArray);

_XI_APIREF_ XI_ERR_TYPE xiDeconvolveAVQ3D_MxN_S8_MOD_DWH_ref(xi_tile3D *inTile,
                                                             xi_array *biasArray,
                                                             const xi_pArray outScaleArray,
                                                             const xi_pArray outShiftArray,
                                                             xi_tile3D *outTile,
                                                             xi_cnn_deconvolveA_params *params,
                                                             xi_tile4D *newCoeffTile,
                                                             xi_array *constTermArray);

_XI_APIREF_ XI_ERR_TYPE xiTransposeConv2DA_U8_DWH_ref(const xi_pTile3D inTile,
                                                      const xi_pTile4D coeffTile,
                                                      const xi_pArray biasArray,
                                                      xi_pArray bufArray,
                                                      xi_pTile3D outTile,
                                                      const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiTransposeConv2DAGetDim4D_NDWH_ref(const xi_pTile4D coeffTile,
                                                        xi_pTile4D subCoeffInfo[],
                                                        xi_pTile4D superCoeffInfo[],
                                                        uint16_t *numSubKernels,
                                                        uint16_t *numSuperKernels,
                                                        uint8_t strideX,
                                                        uint8_t strideY,
                                                        uint8_t getNumKernelsFlag);

_XI_APIREF_ XI_ERR_TYPE xiTransposeConv2DAReOrder4D_I8_NDWH_ref(const xi_pTile4D inTile,
                                                            xi_pTile4D subCoeffs[],
                                                            xi_pTile4D superCoeffs[],
                                                            const xi_cnna_conv_params *param,
                                                            const uint8_t transposeCoeffsFlag);

_XI_APIREF_ XI_ERR_TYPE xiBiasExtend_S32_MOD_ref(const xi_pArray inBiasArray,
                                             xi_pArray outBiasArray);

_XI_APIREF_ XI_ERR_TYPE xiReQuantizeA3D_U8_ref(const xi_pTile3D inTile,
                                               xi_pTile3D outTile,
                                               const xi_cnn_reQuantizeA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiReQuantizeA3D_S8_ref(const xi_pTile3D inTile,
                                               xi_pTile3D outTile,
                                               const xi_cnn_reQuantizeA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiReQuantizeA4D_U8_ref(const xi_pTile4D inTile,
                                               xi_pTile4D outTile,
                                               const xi_cnn_reQuantizeA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiReQuantizeA4D_S8_ref(const xi_pTile4D inTile,
                                               xi_pTile4D outTile,
                                               const xi_cnn_reQuantizeA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiCastA3D_ref(const xi_pTile3D inTile,
                                      xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiMinPoolQuantizeA_U8_DWH_ref(const xi_pTile3D inTile,
                                                      xi_pTile3D outTile,
                                                      const xi_cnn_minpoolA_params *param);

_XI_APIREF_ XI_ERR_TYPE xiMinPoolQuantizeA_S8_DWH_ref(const xi_pTile3D inTile,
                                                      xi_pTile3D outTile,
                                                      const xi_cnn_minpoolA_params *param);

_XI_APIREF_ XI_ERR_TYPE xiAbsA3D_U8_ref(const xi_pTile3D inTile,
                                        xi_pTile3D outTile,
                                        const xi_cnn_AbsA_params *params);

_XI_APIREF_ XI_ERR_TYPE xiAbsA3D_S8_ref(const xi_pTile3D inTile,
                                        xi_pTile3D outTile,
                                        const xi_cnn_AbsA_params *params);

_XI_APIREF_ XI_ERR_TYPE xiShuffleA3D_I8_DWH_ref(const xi_pTile3D inTile,
                                                xi_pTile3D outTile,
                                                const xi_cnn_shuffleA_params *shuffParams);

_XI_APIREF_ XI_ERR_TYPE xiShuffleA4D_I8_ref(const xi_pTile4D inTile,
                                                xi_pTile4D outTile,
                                                const xi_cnn_shuffleA_params *shuffParams);

_XI_APIREF_ XI_ERR_TYPE xiTopKV2A3D_U8_DWH_ref(const xi_pTile3D inTile,
                                               xi_pTile3D outTileIdx,
                                               xi_pTile3D outTileVal,
                                               xi_pTile extraValCnt,
                                               xi_pArray sortedIdxArr,
                                               xi_pArray sortedValArr,
                                               const uint16_t numLargestVal);

_XI_APIREF_ XI_ERR_TYPE xiTopKV2A3D_S8_DWH_ref(const xi_pTile3D inTile,
                                               xi_pTile3D outTileIdx,
                                               xi_pTile3D outTileVal,
                                               xi_pTile extraValCnt,
                                               xi_pArray sortedIdxArr,
                                               xi_pArray sortedValArr,
                                               const uint16_t numLargestVal);

_XI_APIREF_ XI_ERR_TYPE xiSelectA3D_U8_ref(const xi_pTile3D inTile1,
                                           const xi_pTile3D inTile2,
                                           const xi_pTile3D boolMask,
                                           xi_pTile3D outTile,
                                           const xi_cnn_selectA_params * pparams);

_XI_APIREF_ XI_ERR_TYPE xiSelectA3D_S8_ref(const xi_pTile3D inTile1,
                                           const xi_pTile3D inTile2,
                                           const xi_pTile3D boolMask,
                                           xi_pTile3D outTile,
                                           const xi_cnn_selectA_params * pparams);

_XI_APIREF_ XI_ERR_TYPE xiLogicalAndA3D_I8_ref(const xi_pTile3D inBoolTile1,
                                               const xi_pTile3D inBoolTile2,
                                               xi_pTile3D outBoolTile);

_XI_APIREF_ XI_ERR_TYPE xiLogicalOrA3D_I8_ref(const xi_pTile3D inBoolTile1,
                                              const xi_pTile3D inBoolTile2,
                                              xi_pTile3D outBoolTile);

_XI_APIREF_ XI_ERR_TYPE xiLogicalNotA3D_I8_ref(const xi_pTile3D inBoolTile,
                                              xi_pTile3D outBoolTile);

_XI_APIREF_ XI_ERR_TYPE xiHeatmapMaxKeypointA3D_U8_ref(const xi_pTile4D heatmapTile,
                                                       const xi_pArray inBBBoxArray,
                                                       xi_pArray outScoreArray,
                                                       xi_pTile3D outKeypointTile,
                                                       const xi_cnn_heatmap_max_keypointA3D_params * pparams);

_XI_APIREF_ XI_ERR_TYPE xiHeatmapMaxKeypointA3D_S8_ref(const xi_pTile4D heatmapTile,
                                                       const xi_pArray inBBBoxArray,
                                                       xi_pArray outScoreArray,
                                                       xi_pTile3D outKeypointTile,
                                                       const xi_cnn_heatmap_max_keypointA3D_params * pparams);

_XI_APIREF_ XI_ERR_TYPE xiResizeNearestNeighborA3D_I8_DWH_ref(const xi_pTile3D inTile,
                                                              xi_pTile3D outTile,
                                                              const xi_cnn_resizeA3D_params * pparams);

_XI_APIREF_ XI_ERR_TYPE xiResizeBilinearA3D_U8_DWH_ref(const xi_pTile3D inTile,
                                                       xi_pTile3D outTile,
                                                       const xi_cnn_resizeA3D_params * pparams);

_XI_APIREF_ XI_ERR_TYPE xiResizeBilinearA3D_S8_DWH_ref(const xi_pTile3D inTile,
                                                       xi_pTile3D outTile,
                                                       const xi_cnn_resizeA3D_params * pparams);

#endif

/*Float half precision (16bit) variants*/
#if (XCHAL_HAVE_VISION_HP_VFPU == 1)

_XI_APIREF_ XI_ERR_TYPE xiBroadcastEltwiseEqualA3D_F16_ref(const xi_pTile3D inTile1,
                                                           const xi_pTile3D inTile2,
                                                           xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastEltwiseNotEqualA3D_F16_ref(const xi_pTile3D inTile1,
                                                              const xi_pTile3D inTile2,
                                                              xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastAddA3D_F16_ref(const xi_pTile3D inTile1,
                                                  const xi_pTile3D inTile2,
                                                  xi_pTile3D outTile,
                                                  const xi_cnn_addA_F16_params * params);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastMulA3D_F16_ref(const xi_pTile3D inTile1,
                                                  const xi_pTile3D inTile2,
                                                  xi_pTile3D outTile,
                                                  const xi_cnn_mulA_F16_params * params);

_XI_APIREF_ XI_ERR_TYPE xiFullyConnectedA3D_F16_ref(const xi_pTile3D inTile,
                                                    const xi_pTile4D coeffTile,
                                                    const xi_pArray biasArray,
                                                    xi_pTile3D outTile,
                                                    const xi_cnna_conv_F16_params * params);

_XI_APIREF_ XI_ERR_TYPE xiSoftMaxA3D_F16_ref(const xi_pTile3D inTile,
                                             xi_pTile3D outTile,
                                             xi_cnn_softmaxA3D_F16_params * params);

_XI_APIREF_ XI_ERR_TYPE xiLogSoftMaxA3D_F16_ref(const xi_pTile3D inTile,
                                                xi_pTile3D outTile,
                                                xi_cnn_softmaxA3D_F16_params * params);

_XI_APIREF_ XI_ERR_TYPE xiPermuteA4D_F16_ref(const xi_pTile4D inTile,
                                             xi_pTile4D outTile,
                                             const xi_cnn_permuteA4D_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiHeatmapMaxKeypointA3D_F16_ref(const xi_pTile4D heatmapTile,
                                                        const xi_pArray inBBBoxArray,
                                                        xi_pArray outScoreArray,
                                                        xi_pTile3D outKeypointTile,
                                                        xi_cnn_heatmap_max_keypointA3D_F16_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiBoxWithNMSLimitA3D_F16_ref(const xi_pTile3D inBBoxTile,
                                                     const xi_pArray inBatchSplitData,
                                                     xi_pArray inScoreArray,
                                                     xi_pArray sortedIdxArray,
                                                     xi_pArray outBatchSplitData,
                                                     xi_pArray outScoreArray,
                                                     xi_pArray outBBoxArray,
                                                     xi_pArray outClassArray,
                                                     const xi_cnn_box_with_NMS_limitA3D_F16_params * pparams);

_XI_APIREF_ XI_ERR_TYPE xiAxisAlignedBboxTransformA3D_F16_ref(const xi_pArray RoIParam,
                                                              const xi_pTile3D BBoxDeltaTile,
                                                              const xi_pArray batchSplitData,
                                                              xi_pTile3D outBoxTile,
                                                              const xi_cnn_axis_aligned_bbox_transformA3D_F16_params * pparams);

_XI_APIREF_ XI_ERR_TYPE xiDetectionPostprocessA3D_F16_ref(const xi_pArray inScoreArray,
                                                          const xi_pArray inBBoxDeltaArray,
                                                          const xi_pArray anchorArray,
                                                          xi_pArray RoIParam,
                                                          xi_pArray maxScoreArray,
                                                          xi_pArray sortedIdxArray,
                                                          xi_pArray outScoreArray,
                                                          xi_pArray outBBoxArray,
                                                          xi_pArray outClassArray,
                                                          int32_t *numDetections,
                                                          const xi_cnn_detection_postprocessA3D_F16_params * pparams);

_XI_APIREF_ XI_ERR_TYPE xiGenerateProposalsA3D_F16_ref(const xi_pTile3D inScoreTile,
                                                       const xi_pTile4D BBoxDeltaTile,
                                                       xi_pArray batchSplitData,
                                                       const xi_pArray anchorArray,
                                                       xi_pArray RoIParam,
                                                       xi_pTile3D intermediateBBoxTile,
                                                       xi_pArray sortedIdxArray,
                                                       xi_pArray outBBoxArray,
                                                       xi_pArray outScoreArray,
                                                       const xi_cnn_generate_proposalsA3D_F16_params* pparams);

_XI_APIREF_ XI_ERR_TYPE xiDeQuantizeA3D_U8F16_ref(const xi_pTile3D inTile,
                                                  xi_pTile3D outTile,
                                                  xi_pArray  lut,
                                                  const xi_cnn_quantDequantA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiDeQuantizeA3D_S8F16_ref(const xi_pTile3D inTile,
                                                  xi_pTile3D outTile,
                                                  xi_pArray  lut,
                                                  const xi_cnn_quantDequantA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiDeQuantizeA4D_U8F16_ref(const xi_pTile4D inTile,
                                                  xi_pTile4D outTile,
                                                  xi_pArray  lut,
                                                  const xi_cnn_quantDequantA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiDeQuantizeA4D_S8F16_ref(const xi_pTile4D inTile,
                                                  xi_pTile4D outTile,
                                                  xi_pArray  lut,
                                                  const xi_cnn_quantDequantA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiQuantizeA3D_F16U8_ref(const xi_pTile3D inTile,
                                                xi_pTile3D outTile,
                                                const xi_cnn_quantDequantA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiQuantizeA3D_F16S8_ref(const xi_pTile3D inTile,
                                                xi_pTile3D outTile,
                                                const xi_cnn_quantDequantA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiQuantizeA4D_F16U8_ref(const xi_pTile4D inTile,
                                                xi_pTile4D outTile,
                                                const xi_cnn_quantDequantA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiQuantizeA4D_F16S8_ref(const xi_pTile4D inTile,
                                                xi_pTile4D outTile,
                                                const xi_cnn_quantDequantA_params *pparams);

_XI_APIREF_ XI_ERR_TYPE xiROIAlignA3D_F16_DWH_ref(const xi_pTile3D inTile,
                                                  const xi_pArray RoIParam,
                                                  const xi_pArray batchSplitData,
                                                  xi_pTile4D outTile,
                                                  const xi_cnn_roi_align_F16_params *pparams,
                                                  xi_pArray posWtsArr);

_XI_APIREF_ XI_ERR_TYPE xiConvolved3D_MXN_F16_MOD_DWH_ref(const xi_pTile3D inTile,
                                                           const xi_pTile4D coeffTile,
                                                           const xi_pArray biasArray,
                                                           xi_pTile3D outTile,
                                                           const xi_cnna_conv_F16_params *params);

_XI_APIREF_ XI_ERR_TYPE xiSqrtA3D_F16_ref(const xi_pTile3D inTile,
                                          xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiRSqrtA3D_F16_ref(const xi_pTile3D inTile,
                                           xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiDivA3D_F16_ref(const xi_pTile3D numeratorTile,
                                         const xi_pTile3D denominatorTile,
                                         xi_pTile3D outTile,
                                         const xi_cnn_divA_F16_params *params);

_XI_APIREF_ XI_ERR_TYPE xiBroadcastDivA3D_F16_ref(const xi_pTile3D numeratorTile,
                                                  const xi_pTile3D denominatorTile,
                                                  xi_pTile3D outTile,
                                                  const xi_cnn_divA_F16_params *params);

#endif // if (XCHAL_HAVE_VISION_HP_VFPU == 1)

/*Q7 function variants*/
_XI_APIREF_ XI_ERR_TYPE xiTranspose3D_I8_WHD_ID16WH_ref(const xi_pTile3D inTile,
                                                     xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiTranspose3D_I8_ID16WH_WHD_ref(const xi_pTile3D inTile,
                                                     xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedA3D_S_MxN_U8S8U8_DWH_QM24_Q7_ref(const xi_pTile3D inTile,
                                                                    const xi_pArray inPtrOffsetArr,
                                                                    const xi_pTile4D coeffTile,
                                                                    const xi_pArray biasArray,
                                                                    xi_pTile fixUpTile,
                                                                    xi_pTile3D outTile,
                                                                    const xi_cnna_conv_params *convParams);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedA3D_S_MxN_U8S8U8_DWH_QM32_Q7_ref(const xi_pTile3D inTile,
                                                                    const xi_pArray inPtrOffsetArr,
                                                                    const xi_pTile4D coeffTile,
                                                                    const xi_pArray biasArray,
                                                                    xi_pTile fixUpTile,
                                                                    xi_pTile3D outTile,
                                                                    const xi_cnna_conv_params *convParams);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolveA2D_S_MxN_U8_MOD_ID16WH_ref(const xi_pTile3D inTile,
                                                                          const xi_pTile3D coeffTile,
                                                                          const xi_pArray biasArray,
                                                                          xi_pTile3D outTile,
                                                                          const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolveA2D_S_3x3_U8_MOD_ID16WH_ref(const xi_pTile3D inTile,
                                                                          const xi_pTile3D coeffTile,
                                                                          const xi_pArray biasArray,
                                                                          xi_pTile3D outTile,
                                                                          const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolveAVQ2D_S_3x3_S8_MOD_ID16WH_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         const xi_pArray outScaleArray,
                                                                         const xi_pArray outShiftArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiDepthwiseConvolveAVQ2D_S_MxN_S8_MOD_ID16WH_ref(const xi_pTile3D inTile,
                                                                         const xi_pTile3D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         const xi_pArray outScaleArray,
                                                                         const xi_pArray outShiftArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedAVQ3D_S_MxN_S8_DWH_QM32_Q7_ref(const xi_pTile3D inTile,
                                                                  const xi_pArray inPtrOffsetArr,
                                                                  const xi_pTile4D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  const xi_pArray outScaleArray,
                                                                  const xi_pArray outShiftArray,
                                                                  xi_pTile fixUpTile,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnna_conv_params *convParams);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedAVQ3D_S_MxN_S8_DWH_QM24_Q7_ref(const xi_pTile3D inTile,
                                                                  const xi_pArray inPtrOffsetArr,
                                                                  const xi_pTile4D coeffTile,
                                                                  const xi_pArray biasArray,
                                                                  const xi_pArray outScaleArray,
                                                                  const xi_pArray outShiftArray,
                                                                  xi_pTile fixUpTile,
                                                                  xi_pTile3D outTile,
                                                                  const xi_cnna_conv_params *convParams);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedA3D_S_MxN_U8S8U8_ID16WH_QM24_Q7_ref(const xi_pTile3D inTile,
                                                                   const xi_pArray inPtrOffsetArr,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile fixUpTile,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnna_conv_params *convParams);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedA3D_S_MxN_U8S8U8_ID16WH_QM32_Q7_ref(const xi_pTile3D inTile,
                                                                   const xi_pArray inPtrOffsetArr,
                                                                   const xi_pTile4D coeffTile,
                                                                   const xi_pArray biasArray,
                                                                   xi_pTile fixUpTile,
                                                                   xi_pTile3D outTile,
                                                                   const xi_cnna_conv_params *convParams);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedAVQ3D_S_MxN_S8_ID16WH_QM24_Q7_ref(const xi_pTile3D inTile,
                                                                     const xi_pArray inPtrOffsetArr,
                                                                     const xi_pTile4D coeffTile,
                                                                     const xi_pArray biasArray,
                                                                     const xi_pArray outScaleArray,
                                                                     const xi_pArray outShiftArray,
                                                                     xi_pTile3D outTile,
                                                                     const xi_cnna_conv_params *convParams);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedAVQ3D_S_MxN_S8_ID16WH_QM32_Q7_ref(const xi_pTile3D inTile,
                                                                     const xi_pArray inPtrOffsetArr,
                                                                     const xi_pTile4D coeffTile,
                                                                     const xi_pArray biasArray,
                                                                     const xi_pArray outScaleArray,
                                                                     const xi_pArray outShiftArray,
                                                                     xi_pTile3D outTile,
                                                                     const xi_cnna_conv_params *convParams);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedAVQ3D_S_MxN_U8S8U8_ID16WH_QM24_Q7_ref(const xi_pTile3D inTile,
                                                                         const xi_pArray inPtrOffsetArr,
                                                                         const xi_pTile4D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         const xi_pArray outScaleArray,
                                                                         const xi_pArray outShiftArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnna_conv_params *convParams);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedAVQ3D_S_MxN_U8S8U8_ID16WH_QM32_Q7_ref(const xi_pTile3D inTile,
                                                                         const xi_pArray inPtrOffsetArr,
                                                                         const xi_pTile4D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         const xi_pArray outScaleArray,
                                                                         const xi_pArray outShiftArray,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnna_conv_params *convParams);

_XI_APIREF_ XI_ERR_TYPE xiReOrder3D_I8_ID16WH_DWH_ref(const xi_pTile3D inTile,
                                                      xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiReOrder3D_I8_DWH_ID16WH_ref(const xi_pTile3D inTile,
                                                      xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiReOrder3D_I16_ID16WH_DWH_ref(const xi_pTile3D inTile,
                                                       xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiReOrder3D_I16_DWH_ID16WH_ref(const xi_pTile3D inTile,
                                                       xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiReOrder3D_I32_ID16WH_DWH_ref(const xi_pTile3D inTile,
                                                       xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiReOrder3D_I32_DWH_ID16WH_ref(const xi_pTile3D inTile,
                                                       xi_pTile3D outTile);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedA3D_S_MxN_U8S8U8Ca2_MOD_ID16WH_DWH_QM24_ref(const xi_pTile3D inTile,
                                                                               const xi_pArray inPtrOffsetArr,
                                                                               const xi_pTile4D coeffTile,
                                                                               const xi_pArray biasArray,
                                                                               const xi_pTile fixUpTile,
                                                                               xi_pTile3D outTile,
                                                                               const xi_cnna_conv_params *convParams);

_XI_APIREF_ XI_ERR_TYPE xiConvolvedA3D_S_MxN_U8S8U8Ca2_MOD_ID16WH_DWH_QM32_ref(const xi_pTile3D inTile,
                                                                               const xi_pArray inPtrOffsetArr,
                                                                               const xi_pTile4D coeffTile,
                                                                               const xi_pArray biasArray,
                                                                               const xi_pTile fixUpTile,
                                                                               xi_pTile3D outTile,
                                                                               const xi_cnna_conv_params *convParams);

_XI_APIREF_ XI_ERR_TYPE xiconvolvedA3D_S_MxN_U8S8U8_ID16WH_DWH_QM32_ref(const xi_pTile3D inTile,
                                                                        const xi_pArray inPtrOffsetArr,
                                                                        const xi_pTile4D coeffTile,
                                                                        const xi_pArray biasArray,
                                                                        const xi_pTile fixUpTile,
                                                                        xi_pTile3D outTile,
                                                                        const xi_cnna_conv_params *convParams);

_XI_APIREF_  XI_ERR_TYPE xiconvolvedA3D_S_MxN_U8S8U8_ID16WH_DWH_QM24_ref(const xi_pTile3D inTile,
                                                                         const xi_pArray inPtrOffsetArr,
                                                                         const xi_pTile4D coeffTile,
                                                                         const xi_pArray biasArray,
                                                                         const xi_pTile fixUpTile,
                                                                         xi_pTile3D outTile,
                                                                         const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiconvolvedFixupA3D_MxN_U8S32_DWH_ID16WH_QM24_ref(const xi_pTile3D inTile,
                                                                          const xi_pTile4D coeffTile,
                                                                          const xi_pTile3D outTile,
                                                                          xi_pTile fixUpTile,
                                                                          const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiconvolvedA3D_S_MxN_U8S8U8_ID16WH_QM32_ref(const xi_pTile3D inTile,
                                                                    const xi_pArray inPtrOffsetArr,
                                                                    const xi_pTile4D coeffTile,
                                                                    const xi_pArray biasArray,
                                                                    const xi_pTile fixUpTile,
                                                                    xi_pTile3D outTile,
                                                                    const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiconvolvedA3D_S_MxN_U8S8U8_ID16WH_QM24_ref(const xi_pTile3D inTile,
                                                                    const xi_pArray inPtrOffsetArr,
                                                                    const xi_pTile4D coeffTile,
                                                                    const xi_pArray biasArray,
                                                                    const xi_pTile fixUpTile,
                                                                    xi_pTile3D outTile,
                                                                    const xi_cnna_conv_params *param);

_XI_APIREF_ XI_ERR_TYPE xiconvolvedFixupA3D_MxN_U8S32_ID16WH_QM24_ref(const xi_pTile3D inTile,
                                                                      const xi_pTile4D coeffTile,
                                                                      const xi_pTile3D outTile,
                                                                      xi_pTile fixUpTile,
                                                                      const xi_cnna_conv_params *param);
_XI_APIREF_ XI_ERR_TYPE xiextendEdges3DA_ID16WH_ref(xi_pTile3D outTile,
                                                    const int value,
                                                    xi_size3D frame3DSize);

_XI_APIREF_ XI_ERR_TYPE xiComputeOffset_Convd3D_MOD_ref(const xi_pTile3D inTile,
                                                        xi_pArray  inPtrOffsetArr,
                                                        const uint8_t dilationX,
                                                        const uint8_t dilationY,
                                                        const uint8_t strideX,
                                                        const uint8_t strideY,
                                                        const uint8_t kWidth,
                                                        const uint8_t kHeight);

_XI_APIREF_ XI_ERR_TYPE xiAvgPoolA3D_MxN_U8_ID16WH_ref(const xi_pTile3D inTile,
                                                   xi_pTile3D outTile,
                                                   const xi_cnn_pooling_params *param,
                                                   const xi_size3D frame3DSize);

_XI_APIREF_ XI_ERR_TYPE xiAvgPoolQuantizeA_U8_ID16WH_ref(const xi_pTile3D inTile,
                                                         xi_pTile3D outTile,
                                                         const xi_cnn_avgpoolA_params *param,
                                                         const xi_size3D frame3DSize
                                                         );

_XI_APIREF_ XI_ERR_TYPE xiComputeGateEnhancedLSTMA_U8_ref(const xi_pArray inputArray,
                                                          const xi_pArray inputToInputWeightsArray,
                                                          const xi_pArray recurrentToInputWeightsArray,
                                                          const xi_pArray inputToForgetWeightsArray,
                                                          const xi_pArray recurrentToForgetWeightsArray,
                                                          const xi_pArray inputToCellWeightsArray,
                                                          const xi_pArray recurrentToCellWeightsArray,
                                                          const xi_pArray inputToOutputWeightsArray,
                                                          const xi_pArray recurrentToOutputWeightsArray,
                                                          const xi_pArray prevOutputArray,
                                                          const xi_pArray inputToInputEffectiveBiasArray,
                                                          const xi_pArray recurrentToInputEffectiveBiasArray,
                                                          const xi_pArray inputToForgetEffectiveBiasArray,
                                                          const xi_pArray recurrentToForgetEffectiveBiasArray,
                                                          const xi_pArray inputToCellEffectiveBiasArray,
                                                          const xi_pArray recurrentToCellEffectiveBiasArray,
                                                          const xi_pArray inputToOutputEffectiveBiasArray,
                                                          const xi_pArray recurrentToOutputEffectiveBiasArray,
                                                          xi_pArray inputGateBuffer,
                                                          xi_pArray forgetGateBuffer,
                                                          xi_pArray cellGateBuffer,
                                                          xi_pArray outputGateBuffer,
                                                          xi_cnn_enhanced_LSTMA_params* params);

_XI_APIREF_ XI_ERR_TYPE xiComputeStateEnhancedLSTMA_U8_ref(const xi_pArray inputLayerNormArray,
                                                           const xi_pArray forgetLayerNormArray,
                                                           const xi_pArray cellLayerNormArray,
                                                           const xi_pArray outputLayerNormArray,
                                                           const xi_pArray prevCellStateArray,
                                                           const xi_pArray inputBiasArray,
                                                           const xi_pArray forgetBiasArray,
                                                           const xi_pArray cellBiasArray,
                                                           const xi_pArray outputBiasArray,
                                                           const xi_pArray cellToInputArray,
                                                           const xi_pArray cellToForgetArray,
                                                           const xi_pArray cellToOutputArray,
                                                           xi_pArray inputGateBuffer,
                                                           xi_pArray forgetGateBuffer,
                                                           xi_pArray cellGateBuffer,
                                                           xi_pArray outputGateBuffer,
                                                           xi_pArray outputBuffer8bit,
                                                           xi_pArray cellStateArray,
                                                           xi_cnn_enhanced_LSTMA_params* params);

_XI_APIREF_ XI_ERR_TYPE xiProjectionEnhancedLSTMA_U8_ref(xi_pArray projectionInputArray,
                                                         xi_pArray projectionEffectiveBiasArray,
                                                         xi_pArray projectionWeightsArray,
                                                         xi_pArray outputArray,
                                                         xi_pArray outputStateArray,
                                                         xi_cnn_enhanced_LSTMA_params* params);

_XI_APIREF_ XI_ERR_TYPE xiPrecomputeBiasEnhancedLSTM_ref(const xi_pArray inputToInputWeightsArray,
                                                         const xi_pArray recurrentToInputWeightsArray,
                                                         const xi_pArray inputToForgetWeightsArray,
                                                         const xi_pArray recurrentToForgetWeightsArray,
                                                         const xi_pArray inputToCellWeightsArray,
                                                         const xi_pArray recurrentToCellWeightsArray,
                                                         const xi_pArray inputToOutputWeightsArray,
                                                         const xi_pArray recurrentToOutputWeightsArray,
                                                         xi_pArray inputToInputEffectiveBiasArray,
                                                         xi_pArray recurrentToInputEffectiveBiasArray,
                                                         xi_pArray inputToForgetEffectiveBiasArray,
                                                         xi_pArray recurrentToForgetEffectiveBiasArray,
                                                         xi_pArray inputToCellEffectiveBiasArray,
                                                         xi_pArray recurrentToCellEffectiveBiasArray,
                                                         xi_pArray inputToOutputEffectiveBiasArray,
                                                         xi_pArray recurrentToOutputEffectiveBiasArray,
                                                         xi_cnn_enhanced_LSTMA_params* params);

_XI_APIREF_ XI_ERR_TYPE xiPrecomputeBiasProjectionEnhancedLSTM_ref(const xi_pArray projectionWeightsArray,
                                                                   const xi_pArray projectionBiasArray,
                                                                   xi_pArray projectionEffectiveBiasArray,
                                                                   xi_cnn_enhanced_LSTMA_params* params);

#endif //if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))
