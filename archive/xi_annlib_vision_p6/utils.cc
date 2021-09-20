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
#include "cnnrt.h"
#include "utils.h"

static uint32_t       initDone   _LOCAL_RAM_ = 0;
static uint32_t       totalCores _LOCAL_RAM_;
static uint32_t       myCore     _LOCAL_RAM_;
static local_mem_info_t mem_info _LOCAL_RAM_;
static void           *scratchPad _LOCAL_RAM_ = NULL;

int32_t InitXtensaContext(void)
{
    if(!initDone) {
        totalCores = 1;
        myCore = 0;
        // init the mem manager
        XI_ERR_TYPE status = cnnrt_get_local_mem_info(STACK_SIZE, &mem_info);
        status |= cnnrt_init(32, &mem_info);
        if (!status)
            initDone = 1;
        else
        	return 1;
    }
    return 0;
}

local_mem_info_t *getMeminfoContext(void)
{
    return &mem_info;
}

int32_t getTotalCores(void)
{
	return totalCores;
}

int32_t getMyCore(void)
{
	return myCore;
}

void *getScratchpadBuffer(void)
{
	return scratchPad;
}

void xv_memset(int16_t * pMem, int16_t val, size_t size)
{
	xb_vecNx16 * __restrict pvecMem = (xb_vecNx16 *)pMem;
	valign vaS = IVP_ZALIGN();
	int32_t totalBytes = size;
	for (int32_t col = 0; col < totalBytes; col += (XCHAL_IVPN_SIMD_WIDTH << 1))
	{
		IVP_SAVNX16_XP(val, vaS, pvecMem, totalBytes - col);
	}
	IVP_SAPOSNX16_FP(vaS, pvecMem);
	return;
}

xi_bool xiTile3DIsValid_S8_ref(const xi_pTile3D p)
{
  // memory is allocated
  if (p == 0)
  {
    return(0);
  }

  const int max_mes_len = 50;
  char fname[max_mes_len];
  if (XI_TYPE_IS_TILE3D(XI_TILE3D_GET_TYPE(p)))
  {
    sprintf(fname, "xiTile3DIsValid_S8_ref");
  }
  else
  {
    return(0);
  }

  if (XI_TILE3D_GET_BUFF_PTR(p) == 0)
  {
    printf("%s: Buffer pointer is zero\n", fname);
    return(0);
  }

  if(!( (XI_TYPE_ELEMENT_TYPE(XI_TILE3D_GET_TYPE(p)) == XI_S8 ) &&
        (XI_TYPE_IS_TILE3D(XI_TILE3D_GET_TYPE(p)))) )
  {
    printf("%s: Datatype is not valid\n", fname);
    return(0);
  }

  // stride has to be not smaller than width
  if (XI_TILE3D_GET_DIM1_PITCH(p) < XI_TILE3D_GET_DIM1(p) + XI_TILE3D_GET_DIM1_EDGE1(p) + XI_TILE3D_GET_DIM1_EDGE2(p))
  {
    printf("%s: Dim1Pitch is smaller than width\n", fname);
    return(0);
  }

  // edge extension should start inside the buffer
  if ((int8_t *) XI_TILE3D_GET_DATA_PTR(p) - XI_TILE3D_GET_DIM1_EDGE1(p) - XI_TILE3D_GET_DIM1_PITCH(p) * XI_TILE3D_GET_DIM2_EDGE1(p) \
      < (int8_t *) XI_TILE3D_GET_BUFF_PTR(p))
  {
    printf("%s: The pointer on the top left corner of data is out of the buffer\n", fname);
    return(0);
  }

  // consistency with buf size
  if ((XI_TILE3D_GET_BUFF_SIZE(p) < (uint32_t)((XI_TILE3D_GET_DIM1(p) + XI_TILE3D_GET_DIM1_EDGE1(p) + XI_TILE3D_GET_DIM1_EDGE2(p))    \
                                     * (XI_TILE3D_GET_DIM2(p) + XI_TILE3D_GET_DIM2_EDGE1(p) + XI_TILE3D_GET_DIM2_EDGE2(p))  \
                                     * (XI_TILE3D_GET_DIM3(p) + XI_TILE3D_GET_DIM3_EDGE1(p) + XI_TILE3D_GET_DIM3_EDGE2(p))) \
       * XI_ARRAY_GET_ELEMENT_SIZE(p)))
  {
    printf("%s: The buffer size is not consistent with dimensions in the tile\n", fname);
    return(0);
  }

  return(1);
}

xi_bool xiTile3DIsValid_U8_ref(const xi_pTile3D p)
{
    // memory is allocated
    if (p == 0)
    {
        return(0);
    }

    const int max_mes_len = 50;
    char fname[max_mes_len];
    if (XI_TYPE_IS_TILE3D(XI_TILE3D_GET_TYPE(p)))
    {
        sprintf(fname, "xiTile3DIsValid_U8_ref");
    }
    else
    {
        return(0);
    }
    if (XI_TILE3D_GET_BUFF_PTR(p) == 0)
    {
        printf("%s: Buffer pointer is zero\n", fname);
        return(0);
    }

    if (!((XI_TYPE_ELEMENT_TYPE(XI_TILE3D_GET_TYPE(p)) == XI_U8) &&
        (XI_TYPE_IS_TILE3D(XI_TILE3D_GET_TYPE(p)))))
    {
        printf("%s: Datatype is not valid\n", fname);
        return(0);
    }

    // stride has to be not smaller than width
    if (XI_TILE3D_GET_DIM1_PITCH(p) < XI_TILE3D_GET_DIM1(p) + XI_TILE3D_GET_DIM1_EDGE1(p) + XI_TILE3D_GET_DIM1_EDGE2(p))
    {
        printf("%s: Dim1Pitch is smaller than width\n", fname);
        return(0);
    }

    // edge extension should start inside the buffer
    if ((uint8_t*)XI_TILE3D_GET_DATA_PTR(p) - XI_TILE3D_GET_DIM1_EDGE1(p) - XI_TILE3D_GET_DIM1_PITCH(p) * XI_TILE3D_GET_DIM2_EDGE1(p) \
        < (uint8_t*)XI_TILE3D_GET_BUFF_PTR(p))
    {
        printf("%s: The pointer on the top left corner of data is out of the buffer\n", fname);
        return(0);
    }

    // consistency with buf size
    if ((XI_TILE3D_GET_BUFF_SIZE(p) < (uint32_t)((XI_TILE3D_GET_DIM1(p) + XI_TILE3D_GET_DIM1_EDGE1(p) + XI_TILE3D_GET_DIM1_EDGE2(p))    \
        * (XI_TILE3D_GET_DIM2(p) + XI_TILE3D_GET_DIM2_EDGE1(p) + XI_TILE3D_GET_DIM2_EDGE2(p))  \
        * (XI_TILE3D_GET_DIM3(p) + XI_TILE3D_GET_DIM3_EDGE1(p) + XI_TILE3D_GET_DIM3_EDGE2(p))) \
        * XI_ARRAY_GET_ELEMENT_SIZE(p)))
    {
        printf("%s: The buffer size is not consistent with dimensions in the tile\n", fname);
        return(0);
    }

    return(1);
}

xi_bool xiTile3DCheckType_ref(const xi_pTile3D p, uint16_t type)
{
  uint16_t tileType = XI_TILE3D_GET_TYPE(p);

  if((XI_TYPE_ELEMENT_TYPE(tileType) == type ) && (XI_TYPE_IS_TILE3D(tileType)))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
xi_bool xiTile3DIsValid_I8_ref(const xi_pTile3D p)
{
  if(p == NULL)
  {
    return(0);
  }

  if (xiTile3DCheckType_ref(p, XI_U8))
  {
    return(xiTile3DIsValid_U8_ref(p));
  }
  if (xiTile3DCheckType_ref(p, XI_S8))
  {
    return(xiTile3DIsValid_S8_ref(p));
  }
  return(0);
}

xi_bool xiTile4DIsValid_U8_ref(const xi_pTile4D p)
{
  // memory is allocated
  if (p == 0)
  {
    return(0);
  }

  const int max_mes_len = 50;
  char fname[max_mes_len];
  if (XI_TYPE_IS_TILE4D(XI_TILE4D_GET_TYPE(p)))
  {
    sprintf(fname, "xiTile4DIsValid_U8_ref");
  }
  else
  {
    return(0);
  }

  if (XI_TILE4D_GET_BUFF_PTR(p) == 0)
  {
    printf("%s: Buffer pointer is zero\n", fname);
    return(0);
  }

  if (!((XI_TYPE_ELEMENT_TYPE(XI_TILE4D_GET_TYPE(p)) == XI_U8) &&
    (XI_TYPE_IS_TILE4D(XI_TILE4D_GET_TYPE(p)))))
  {
    printf("%s: Datatype is not valid\n", fname);
    return(0);
  }

  // stride has to be not smaller than width
  if (XI_TILE4D_GET_DIM1_PITCH(p) < XI_TILE4D_GET_DIM1(p) + XI_TILE4D_GET_DIM1_EDGE1(p) + XI_TILE4D_GET_DIM1_EDGE2(p))
  {
    printf("%s: Dim1Pitch is smaller than (Dim1Size + Dim1Edge1 + Dim1Edge2)\n", fname);
    return(0);
  }

  // edge extension should start inside the buffer
  if ((uint8_t *)XI_TILE4D_GET_DATA_PTR(p) - XI_TILE4D_GET_DIM1_EDGE1(p) - \
       XI_TILE4D_GET_DIM1_PITCH(p) * XI_TILE4D_GET_DIM2_EDGE1(p) -         \
       XI_TILE4D_GET_DIM2_PITCH(p) * XI_TILE4D_GET_DIM3_EDGE1(p)           \
    < (uint8_t *)XI_TILE4D_GET_BUFF_PTR(p))
  {
    printf("%s: The pointer on the top left corner of data is out of the buffer\n", fname);
    return(0);
  }

  // consistency with buf size
  if ((XI_TILE4D_GET_BUFF_SIZE(p) < (uint32_t)((XI_TILE4D_GET_DIM1(p) + XI_TILE4D_GET_DIM1_EDGE1(p) + XI_TILE4D_GET_DIM1_EDGE2(p))    \
    * (XI_TILE4D_GET_DIM2(p) + XI_TILE4D_GET_DIM2_EDGE1(p) + XI_TILE4D_GET_DIM2_EDGE2(p))  \
    * (XI_TILE4D_GET_DIM3(p) + XI_TILE4D_GET_DIM3_EDGE1(p) + XI_TILE4D_GET_DIM3_EDGE2(p)) * XI_TILE4D_GET_DIM4(p)) \
    * XI_ARRAY_GET_ELEMENT_SIZE(p)))
  {
    printf("%s: The buffer size is not consistent with dimensions in the tile\n", fname);
    return(0);
  }

  return(1);
}

xi_bool xiTile4DIsValid_S8_ref(const xi_pTile4D p)
{
  // memory is allocated
  if (p == 0)
  {
    return(0);
  }

  const int max_mes_len = 50;
  char fname[max_mes_len];
  if (XI_TYPE_IS_TILE4D(XI_TILE4D_GET_TYPE(p)))
  {
    sprintf(fname, "xiTile4DIsValid_S8_ref");
  }
  else
  {
    return(0);
  }

  if (XI_TILE4D_GET_BUFF_PTR(p) == 0)
  {
    printf("%s: Buffer pointer is zero\n", fname);
    return(0);
  }

  if(!( (XI_TYPE_ELEMENT_TYPE(XI_TILE4D_GET_TYPE(p)) == XI_S8 ) &&
          (XI_TYPE_IS_TILE4D(XI_TILE4D_GET_TYPE(p)))) )
  {
    printf("%s: Datatype is not valid\n", fname);
    return(0);
  }

  // stride has to be not smaller than width
  if (XI_TILE4D_GET_DIM1_PITCH(p) < XI_TILE4D_GET_DIM1(p) + XI_TILE4D_GET_DIM1_EDGE1(p) + XI_TILE4D_GET_DIM1_EDGE2(p))
  {
    printf("%s: Dim1Pitch is smaller than width\n", fname);
    return(0);
  }

  // edge extension should start inside the buffer
  if ((int8_t *) XI_TILE4D_GET_DATA_PTR(p) - XI_TILE4D_GET_DIM1_EDGE1(p) - \
       XI_TILE4D_GET_DIM1_PITCH(p) * XI_TILE4D_GET_DIM2_EDGE1(p) -         \
       XI_TILE4D_GET_DIM2_PITCH(p) * XI_TILE4D_GET_DIM3_EDGE1(p)           \
      < (int8_t *) XI_TILE4D_GET_BUFF_PTR(p))
  {
    printf("%s: The pointer on the top left corner of data is out of the buffer\n", fname);
    return(0);
  }

  // consistency with buf size
  if ((XI_TILE4D_GET_BUFF_SIZE(p) < (uint32_t)((XI_TILE4D_GET_DIM1(p) + XI_TILE4D_GET_DIM1_EDGE1(p) + XI_TILE4D_GET_DIM1_EDGE2(p))                  \
                                     * (XI_TILE4D_GET_DIM2(p) + XI_TILE4D_GET_DIM2_EDGE1(p) + XI_TILE4D_GET_DIM2_EDGE2(p))                          \
                                     * (XI_TILE4D_GET_DIM3(p) + XI_TILE4D_GET_DIM3_EDGE1(p) + XI_TILE4D_GET_DIM3_EDGE2(p)) * XI_TILE4D_GET_DIM4(p)) \
       * XI_ARRAY_GET_ELEMENT_SIZE(p)))
  {
    printf("%s: The buffer size is not consistent with dimensions in the tile\n", fname);
    return(0);
  }

  return(1);
}

static xi_bool xiDataContainerIsValid_S32_ref(const xi_pArray p, const int edge_w, const int edge_h)
{
    // memory is allocated
    if (p == 0) return 0;

    const int max_mes_len = 50;
    char fname[max_mes_len];
    if (XI_ARRAY_IS_TILE(p))
        sprintf(fname, "xiTileIsValid_S32_ref");
    else
        sprintf(fname, "xiArrayIsValid_S32_ref");
    char edge_ext_mes[max_mes_len];
    if (XI_ARRAY_IS_TILE(p))
        sprintf(edge_ext_mes, " with edges ");
    else
        sprintf(edge_ext_mes, " ");

    if (XI_ARRAY_GET_BUFF_PTR(p) == 0)
    {
        printf("%s: Buffer pointer is zero\n", fname);
        return 0;
    }

    // stride has to be not smaller than width
    if (XI_ARRAY_GET_PITCH(p) < XI_ARRAY_GET_WIDTH(p) + edge_w * 2)
    {
        printf("%s: Pitch is smaller than width\n", fname);
        return 0;
    }

    // edge extension should start inside the buffer
    if ((int32_t*)XI_ARRAY_GET_DATA_PTR(p) - edge_w - XI_ARRAY_GET_PITCH(p) * edge_h < (int32_t*)XI_ARRAY_GET_BUFF_PTR(p))
    {
        printf("%s: The pointer on the top left corner of data%sis out of the buffer\n", fname, edge_ext_mes);
        return 0;
    }

    // edge extension should fit into the buffer
    if(XI_TILE_GET_BUFF_SIZE(p) != (uint16_t)-1)
    if ((int32_t*)XI_ARRAY_GET_DATA_PTR(p) + XI_ARRAY_GET_PITCH(p) * (XI_ARRAY_GET_HEIGHT(p) + edge_h - 1) + XI_ARRAY_GET_WIDTH(p) + edge_w
        > (int32_t*)((uint8_t*)XI_ARRAY_GET_BUFF_PTR(p) + XI_ARRAY_GET_BUFF_SIZE(p)))
    {
        printf("%s: The pointer on the bottom right corner of data%sis out of the buffer\n", fname, edge_ext_mes);
        return 0;
    }

    // minimal alignment requirement
    if ((size_t)(XI_ARRAY_GET_DATA_PTR(p)) % 4 != 0)
    {
        printf("%s: The data pointer has to be aligned by 4\n", fname);
        return 0;
    }

    return 1;
}

xi_bool xiArrayIsValid_S32_ref(const xi_pArray p)
{
  return(xiDataContainerIsValid_S32_ref(p, 0, 0));
}

static xi_bool xiDataContainerIsValid_S8_ref(const xi_pArray p, const int edge_w, const int edge_h)
{
  // memory is allocated
  if (p == 0)
  {
    return(0);
  }

  const int max_mes_len = 50;
  char fname[max_mes_len];
  if (XI_ARRAY_IS_TILE(p))
  {
    sprintf(fname, "xiTileIsValid_S8_ref");
  }
  else
  {
    sprintf(fname, "xiArrayIsValid_S8_ref");
  }
  char edge_ext_mes[max_mes_len];
  if (XI_ARRAY_IS_TILE(p))
  {
    sprintf(edge_ext_mes, " with edges ");
  }
  else
  {
    sprintf(edge_ext_mes, " ");
  }

  if (XI_ARRAY_GET_BUFF_PTR(p) == 0)
  {
    printf("%s: Buffer pointer is zero\n", fname);
    return(0);
  }
  if (!(XI_TYPE_ELEMENT_TYPE(XI_ARRAY_GET_TYPE(p)) == XI_S8))
  {
    printf("%s: Datatype is not valid\n", fname);
    return(0);
  }

  // stride has to be not smaller than width
  if (XI_ARRAY_GET_PITCH(p) < XI_ARRAY_GET_WIDTH(p) + edge_w * 2)
  {
    printf("%s: Pitch is smaller than width\n", fname);
    return(0);
  }

  // edge extension should start inside the buffer
  if ((uint8_t *)XI_ARRAY_GET_DATA_PTR(p) - edge_w - XI_ARRAY_GET_PITCH(p) * edge_h < (uint8_t *)XI_ARRAY_GET_BUFF_PTR(p))
  {
    printf("%s: The pointer on the top left corner of data%sis out of the buffer\n", fname, edge_ext_mes);
    return(0);
  }

  // edge extension should fit into the buffer
  if (XI_ARRAY_GET_BUFF_SIZE(p) != (uint16_t)-1)
  {
    if ((uint8_t *)XI_ARRAY_GET_DATA_PTR(p) + XI_ARRAY_GET_PITCH(p) * (XI_ARRAY_GET_HEIGHT(p) + edge_h - 1) + XI_ARRAY_GET_WIDTH(p) + edge_w
  >(uint8_t *) XI_ARRAY_GET_BUFF_PTR(p) + XI_ARRAY_GET_BUFF_SIZE(p))
    {
      printf("%s: The pointer on the bottom right corner of data%sis out of the buffer\n", fname, edge_ext_mes);
      return(0);
    }
  }

  return(1);
}

xi_bool xiArrayIsValid_S8_ref(const xi_pArray p)
{
  return(xiDataContainerIsValid_S8_ref(p, 0, 0));
}

int dumpOutputToFile(std::string filename, int dim0, int dim1, int dim2, int dim3, uint8_t *pData)
{
  int i;
  FILE *op;
  int outputSize = dim0 * dim1 * dim2 * dim3;
  if (!(op=fopen(filename.c_str(), "w")))
    exit(-1);
  fprintf(op, "dims:%d, %d, %d, %d \n", dim0, dim1, dim2, dim3);
  for (i = 0; i < outputSize; i++)
    {
      fprintf(op,"[%d]=%d,", i, pData[i]);
      if ((i%10)==0) fprintf(op, "\n");
    }
    fclose(op);
    return 0;
}
