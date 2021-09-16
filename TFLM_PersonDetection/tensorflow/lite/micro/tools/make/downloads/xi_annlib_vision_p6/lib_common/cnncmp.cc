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
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <float.h>
#include <xtensa/tie/xt_ivpn.h>
#include <math.h>

#include "cnnrt.h"


#define ABSDIFF(suffix, type)                           \
    static uint32_t absdiff_##suffix(void *a, void *b)  \
    {                                                   \
        type aa = *(type*)a;                            \
        type bb = *(type*)b;                            \
        return (aa<bb) ? (bb-aa) : (aa-bb);             \
    }

#if (XCHAL_HAVE_VISION_HP_VFPU==1)
//   static xb_f16 absdiff_F16(void *a, void *b)
   static int absdiff_F16(void *a, void *b)
   {
        int16_t short_a = *(short *)a;
        int16_t short_b = *(short *)b;
        if ((((short_a >> 10) & 0x1f) == 0x1f) && ((short_a & 0x2ff) !=0)) return (0);
        if ((((short_b >> 10) & 0x1f) == 0x1f) && ((short_b & 0x2ff) !=0)) return (0);
        if ((abs(short_a-short_b)) > 0) return(1);
        else return(0);
#if 0
        return ((xb_f16)0.0f);
        xb_f16 aa = *(xb_f16 *)a;
        xb_f16 bb = *(xb_f16 *)b;
        return (aa<bb) ? (bb-aa) : (aa-bb);
#endif
   }
#endif // (XCHAL_HAVE_VISION_HP_VFPU==1)


ABSDIFF(U8,  uint8_t)
ABSDIFF(S8,  int8_t)
ABSDIFF(U16, uint16_t)
ABSDIFF(S16, int16_t)
ABSDIFF(U32, uint32_t)
ABSDIFF(S32, int32_t)

static uint32_t absdiff_F32(void *a, void *b)
{
        float aa = *(float*)a;
        float bb = *(float*)b;
        float fdiff = (aa<bb) ? (bb-aa) : (aa-bb);
        return fdiff != 0.0f;
}

#define UPDATE_RANGE(min, max, value) \
    {                                                   \
        (min) = ((min) <= (value)) ? (min) : (value);   \
        (max) = ((max) >= (value)) ? (max) : (value);   \
    }

int cnnCmpTiles(const char *net, const char *param, void *tileA, void *tileB)
{
    int N=1, M=1, H=1, W=1;
    int pN=0, pM=0, pH;
    int sz;
    char *pa, *pb;
    int first_coord[4] = {0,0,0,0};
    unsigned errors = 0;
    unsigned max_diff = 0;
    int32_t min_a = INT_MAX;
    int32_t max_a = INT_MIN;
    int32_t min_b = INT_MAX;
    int32_t max_b = INT_MIN;
    uint32_t umin_a = UINT_MAX;
    uint32_t umax_a = 0;
    uint32_t umin_b = UINT_MAX;
    uint32_t umax_b = 0;
#if (XCHAL_HAVE_VISION_HP_VFPU==1)
    //xb_f16 xb_f16_min_a;
    //xb_f16 xb_f16_max_a;
    //xb_f16 xb_f16_min_b;
    //xb_f16 xb_f16_max_b;
#endif
    float fmin_a = -FLT_MAX;
    float fmax_a = FLT_MAX;
    float fmin_b = -FLT_MAX;
    float fmax_b = FLT_MAX;
    int type;

    if (param && net) {
        printf("Comparing parameter %s of %s: ", param, net);
    }

    type = XI_ARRAY_GET_TYPE((xi_pArray)tileA);

    assert(type == XI_ARRAY_GET_TYPE((xi_pArray)tileB));

#if XCHAL_HAVE_XNNE == 1
    if (XI_TYPE_IS_TILE3D(type) && XI_TILE3D_GET_DATA_ORDER((xi_pTile3D)tileA) == XI_MTILE) {
        xi_tile3D tileA_DWH;
        xi_tile3D tileB_DWH;
        allocEdgelessDWHTile3D(&tileA_DWH, (xi_pTile3D)tileA);
        allocEdgelessDWHTile3D(&tileB_DWH, (xi_pTile3D)tileB);
        if (copy_fromMTILES_toTile3D_ref((xi_pTile3D)tileA, &tileA_DWH) == XI_ERR_OK &&
            copy_fromMTILES_toTile3D_ref((xi_pTile3D)tileB, &tileB_DWH) == XI_ERR_OK) {
            errors = cnnCmpTiles(NULL, NULL, &tileA_DWH, &tileB_DWH);
        } else {
            printf("FAILED \n - Cannot convert MEMTILE format into DWH\n");
            errors = 1;
        }
        freeEdgelessDWHTile3D(&tileA_DWH);
        freeEdgelessDWHTile3D(&tileB_DWH);
        return errors;
    } else if (XI_TYPE_IS_TILE3D(type) && XI_TILE3D_GET_DATA_ORDER((xi_pTile3D)tileA) == XI_CMTILE) {
        xi_tile3D tileA_DWH;
        xi_tile3D tileB_DWH;
        allocEdgelessDWHTile3D(&tileA_DWH, (xi_pTile3D)tileA);
        allocEdgelessDWHTile3D(&tileB_DWH, (xi_pTile3D)tileB);
        if (copy_fromCMTILES_toTile3D_ref((xi_pTile3D)tileA, &tileA_DWH) == XI_ERR_OK &&
            copy_fromCMTILES_toTile3D_ref((xi_pTile3D)tileB, &tileB_DWH) == XI_ERR_OK) {
            errors = cnnCmpTiles(NULL, NULL, &tileA_DWH, &tileB_DWH);
        } else {
            printf("FAILED \n - Cannot convert CMEMTILE format into DWH\n");
            errors = 1;
        }
        freeEdgelessDWHTile3D(&tileA_DWH);
        freeEdgelessDWHTile3D(&tileB_DWH);
        return errors;
    }
#endif

    if (XI_TYPE_IS_TILE4D(type)) {
        W = XI_TILE4D_GET_DIM1((xi_pTile4D)tileA);
        H = XI_TILE4D_GET_DIM2((xi_pTile4D)tileA);
        M = XI_TILE4D_GET_DIM3((xi_pTile4D)tileA);
        N = XI_TILE4D_GET_DIM4((xi_pTile4D)tileA);
        pH = XI_TILE4D_GET_DIM1_PITCH((xi_pTile4D)tileA);
        pM = XI_TILE4D_GET_DIM2_PITCH((xi_pTile4D)tileA);
        pN = XI_TILE4D_GET_DIM3_PITCH((xi_pTile4D)tileA);
    } else if (XI_TYPE_IS_TILE3D(type)) {
        W = XI_TILE3D_GET_DIM1((xi_pTile3D)tileA);
        H = XI_TILE3D_GET_DIM2((xi_pTile3D)tileA);
        M = XI_TILE3D_GET_DIM3((xi_pTile3D)tileA);
        pH = XI_TILE3D_GET_DIM1_PITCH((xi_pTile3D)tileA);
        pM = XI_TILE3D_GET_DIM2_PITCH((xi_pTile3D)tileA);
    } else if (XI_TYPE_IS_TILE(type)) {
        W = XI_TILE_GET_WIDTH((xi_pTile)tileA);
        H = XI_TILE_GET_HEIGHT((xi_pTile)tileA);
        pH = XI_TILE_GET_PITCH((xi_pTile)tileA);
    } else {
        /* Assume it's XI array */
        W = XI_ARRAY_GET_WIDTH((xi_pArray)tileA);
        H = XI_ARRAY_GET_HEIGHT((xi_pArray)tileA);
        pH = XI_ARRAY_GET_PITCH((xi_pArray)tileA);
    }

    pa = (char*)XI_ARRAY_GET_DATA_PTR((xi_pArray)tileA);
    pb = (char*)XI_ARRAY_GET_DATA_PTR((xi_pArray)tileB);

    sz = XI_ARRAY_GET_ELEMENT_SIZE((xi_pArray)tileA);

    for (int i=0; i<N; i++)
        for (int j=0; j<M; j++)
            for (int k=0; k<H; k++)
                for (int l=0; l<W; l++) {
                    unsigned diff = 0;
                    void *pae = (void*)(pa + (i*pN + j*pM + k*pH + l)*sz);
                    void *pbe = (void*)(pb + (i*pN + j*pM + k*pH + l)*sz);
                    switch (XI_TYPE_ELEMENT_TYPE(type)) {
                    case XI_U8:
                        diff = absdiff_U8(pae, pbe);
                        UPDATE_RANGE(min_a, max_a, *(uint8_t*)pae);
                        UPDATE_RANGE(min_b, max_b, *(uint8_t*)pbe);
                        break;
                    case XI_S8:
                        diff = absdiff_S8(pae, pbe);
                        UPDATE_RANGE(min_a, max_a, *(int8_t*)pae);
                        UPDATE_RANGE(min_b, max_b, *(int8_t*)pbe);
                        break;
                    case XI_U16:
                        diff = absdiff_U16(pae, pbe);
                        UPDATE_RANGE(min_a, max_a, *(uint16_t*)pae);
                        UPDATE_RANGE(min_b, max_b, *(uint16_t*)pbe);
                        break;
                    case XI_S16:
                        diff = absdiff_S16(pae, pbe);
                        UPDATE_RANGE(min_a, max_a, *(int16_t*)pae);
                        UPDATE_RANGE(min_b, max_b, *(int16_t*)pbe);
                        break;
                    case XI_U32:
                        diff = absdiff_U32(pae, pbe);
                        UPDATE_RANGE(umin_a, umax_a, *(uint32_t*)pae);
                        UPDATE_RANGE(umin_b, umax_b, *(uint32_t*)pbe);
                        break;
                    case XI_S32:
                        diff = absdiff_S32(pae, pbe);
                        UPDATE_RANGE(min_a, max_a, *(int32_t*)pae);
                        UPDATE_RANGE(min_b, max_b, *(int32_t*)pbe);
                        break;
#if (XCHAL_HAVE_VISION_HP_VFPU==1)
                    case XI_F16:
                        diff = absdiff_F16(pae, pbe);
                        if (diff) errors++;
//                        UPDATE_RANGE(xb_f16_min_a, xb_f16_max_a, *(xb_f16*)pae);
//                        UPDATE_RANGE(xb_f16_min_b, xb_f16_max_b, *(xb_f16*)pbe);
                        break;
#endif // (XCHAL_HAVE_VISION_HP_VFPU==1)
                    case XI_F32:
                        diff = absdiff_F32(pae, pbe);
                        UPDATE_RANGE(fmin_a, fmin_b, *(float*)pae);
                        UPDATE_RANGE(fmin_b, fmin_b, *(float*)pbe);
                        break;
                    default:
                        assert(0 && "unsupported type");
                    }
                    if (diff != 0) {
                        errors++;
                        if (max_diff == 0) {
                            first_coord[0] = l;
                            first_coord[1] = k;
                            first_coord[2] = j;
                            first_coord[3] = i;
                        }
                        if (max_diff < diff)
                            max_diff = diff;
                    }
                }

    if (max_diff > 0) {
        printf("FAILED\n - First error is at {%d,%d,%d,%d}. Max difference: %u. Total errors: %d.\n",
               first_coord[0], first_coord[1], first_coord[2], first_coord[3],
               max_diff, errors);
        if (XI_TYPE_ELEMENT_TYPE(type) == XI_F32) {
            printf(" - Data range {%f, %f} vs {%f, %f}.\n", fmin_a, fmax_a, fmin_b, fmax_b);
        } else if (XI_TYPE_ELEMENT_TYPE(type) == XI_U32) {
            printf(" - Data range {%u, %u} vs {%u, %u}.\n", umin_a, umax_a, umin_b, umax_b);
        } else {
            printf(" - Data range {%d, %d} vs {%d, %d}.\n", min_a, max_a, min_b, max_b);
        }
    } else {
        if (XI_TYPE_ELEMENT_TYPE(type) == XI_F32) {
            printf("OK. Data range {%f, %f}.\n", fmin_a, fmax_a);
        } else if (XI_TYPE_ELEMENT_TYPE(type) == XI_U32) {
            printf("OK. Data range {%u, %u}.\n", umin_a, umax_a);
        } else {
            printf("OK. Data range {%d, %d}.\n", min_a, max_a);
        }
    }
    return errors;
}


