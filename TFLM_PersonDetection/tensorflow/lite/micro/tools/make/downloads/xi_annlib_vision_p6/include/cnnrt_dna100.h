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
#ifndef __CNNRT_DNA100_H__
#define __CNNRT_DNA100_H__

#include <assert.h>
#include <stdint.h>

#if XCHAL_HAVE_XNNE == 1

#include <xtensa/tie/xt_ivpn.h>
#include <xtensa/tie/xt_xcxnne.h>
#ifdef __XTENSA__
#  include <xtensa/xnne.h>
#endif

#include "cnnrt_compression.h"

#define PACKED __attribute__ ((__packed__))

#define PTILE_REQ_ARR_SIZE    4
#define NUM_LUT_TABLE_ENTRIES 16
#define XNNE_MSG_FETCH_SIZE   64
#define XNNE_CMDPULL_MSG_MAX  63

typedef enum {
    ePCH_DMAin              = 0,
    ePCH_TF                 = 1,
    ePCH_VPU                = 3,
    ePCH_DMAout             = 4
} eChannel;

typedef enum {
    eCMD_DMAin_Layer_Config        = 1,
    eCMD_DMAin_TensorActivation_Go = 2,
    eCMD_DMAin_PTILEActivation_Go  = 3,
    eCMD_DMAin_FCActivation_Go     = 4,
    eCMD_DMAin_OBUF_Go             = 5,
    eCMD_DMAin_Coefficient_Go      = 6,
    eCMD_DMAin_FW_Go               = 7,
    eCMD_DMAin_LAST                = eCMD_DMAin_FW_Go,
    eCMD_TF_Layer_Config           = 1,
    eCMD_TF_Set_LUT                = 2,
    eCMD_TF_SwitchLayer            = 3,
    eCMD_TF_Go                     = 4,
    eCMD_TF_LAST                   = eCMD_TF_Go,
    eCMD_DMAout_SysM_Go            = 1,
    eCMD_DMAout_IBUF_Go            = 2,
    eCMD_DMAout_TensorSysM_Go      = 3,
    eCMD_DMAout_LAST               = eCMD_DMAout_TensorSysM_Go,
    eCMD_VPU_Pool_Layer_Config     = 1,  /* VPU_Pool, _Group1, and _Group2 share the same CMD */
    eCMD_VPU_Group1_Layer_Config   = 1,
    eCMD_VPU_Group2_Layer_Config   = 1,
    eCMD_VPU_Group3_Layer_Config   = 1,
    eCMD_VPU_Go                    = 2,
    eCMD_VPU_LAST                  = eCMD_VPU_Go
} eCMD;

typedef enum {
    eSHARED = 0,
    eIDMA   = 1,
    eCDMA   = 2,
    eTF     = 3,
    eVPU    = 4,
    eODMA   = 5,
    eUNIT_FIRST = eIDMA,
    eUNIT_LAST  = eODMA
} eXNNEUnit;

typedef enum {
    eSMO_PTILE_COMPRESSED   = 0,
    eSMO_PTILE_UNCOMPRESSED = 1,
    eSMO_TENSOR             = 2,
    eSMO_1x1_FC_LINEAR      = 3,
    eSMO_LAST               = eSMO_1x1_FC_LINEAR,
    eSMO_RESERVED1          = 4,
    eSMO_RESERVED2          = 5,
    eSMO_RESERVED3          = 6,
    eSMO_RESERVED4          = 7
} eSysMemOrder;

typedef enum {
    eIBO_SUBTENSOR_PTILE    = 0,
    eIBO_FC_ORDER           = 1,
    eIBO_LAST               = eIBO_FC_ORDER,
    eIBO_RESERVED1          = 2,
    eIBO_RESERVED2          = 3
} eIBUFOrder;

typedef enum {
    eBI_BATCH1              = 0,
    eBI_BATCH2              = 1,
    eBI_BATCH4              = 2,
    eBI_BATCH8              = 3,
    eBI_BATCH16             = 4,
    eBI_LAST                = eBI_BATCH16,
    eBI_RESERVED5           = 5,
    eBI_RESERVED6           = 6,
    eBI_RESERVED7           = 7
} eLog2NBI;

typedef enum {
    eLC_3DCONV              = 0,
    eLC_DWCONV              = 1,
    eLC_FC_BATCH1           = 2,
    eLC_FC_BATCH8           = 3,
    eLC_FC_BATCH16          = 4,
    eLC_LAST                = eLC_FC_BATCH16,
    eLC_RESERVED1           = 5,
    eLC_RESERVED2           = 6,
    eLC_RESERVED3           = 7
} eLayerClass;

typedef enum {
    eDT_S8                  = 0,
    eDT_U8                  = 1,
    eDT_S16                 = 2,
    eDT_U16                 = 3,
    eDT_F16                 = 4,
    eDT_LAST                = eDT_F16,
    eDT_BF16                = 5
} eDataType;

typedef enum {
    eVPU_NONE               = 0,
    eVPU_POOLING            = 1,
    eVPU_ELTWISEADD         = 2,
    eVPU_LEAKY_RELU         = 3,
    eVPU_RENORM             = 4,
    eVPU_CROP               = 5,
    eVPU_DECONV_INTLV       = 6,
    eVPU_ACTIVATION         = 7,
    eVPU_RENORM_VQ          = 8,
    eVPU_PRELU              = 9,
    eVPU_RELUX              = 10,
    eVPU_ELTWISEMUL         = 11,
    eVPU_DEINTERLEAVE       = 12,
    eVPU_ERROR_INJECT       = 13,
    eVPU_BINNING            = 14,
    eVPU_LAST               = eVPU_BINNING
} eVPUType;

typedef enum {
    ePOOL_NONE              = 0,
    ePOOL_MAX               = 1,
    ePOOL_AVG               = 2
} ePoolType;

typedef enum {
    eELTWISE_HIGH_PREC      = 0,
    eELTWISE_LOW_PREC       = 1
} eEltwisePrecType;

typedef enum {
    eMODE_SIGMOID           = 0,
    eMODE_TANH              = 1
} eModeType;

typedef struct PACKED {
    uint32_t SBLKID  :4;
    uint32_t LAYERID :4;
    uint32_t MSGID   :8;

    eChannel PHYSCH  :3;
    eCMD     CMD     :5;
    uint8_t  dummy;
} MessageHdr;



typedef struct PACKED {
    MessageHdr   hdr;
    eSysMemOrder SYSORDER          :3;
    uint32_t     dummy1            :1;
    uint32_t     dummy2            :4;
    eLog2NBI     LOG2NBI           :3;
    uint32_t     dummy3            :1;
    uint32_t     STW_M1            :2;
    uint32_t     STH_M1            :2;
    uint16_t     dummy4;
    uint32_t     BINSTRIDE;

    uint32_t     CB;
    uint32_t     PadValue;
    uint32_t     dummy6;
    uint16_t     PTD;
    uint16_t     PTW;
    uint32_t     TF_R_IBUF_IDMA_MW :4;
    uint32_t     Reserved1         :4;
    uint32_t     TF_R_CBUF_CDMA_MW :4;
    uint32_t     Reserved2         :4;
    uint32_t     ODMA_W_SYS_IDMA_MW:4;
    uint32_t     Reserved3         :4;
    uint32_t     IDMA_W_IBUF_TF_BC :4;
    uint32_t     Reserved4         :4;
    uint32_t     CDMA_W_CBUF_TF_BC :4;
    uint32_t     Reserved5         :4;
}  DMAin_Layer_Config;



typedef struct PACKED {
    MessageHdr hdr;
    uint32_t SPA;
    uint16_t SUB_C;
    uint16_t dummy0;
    uint32_t DPA;
    uint32_t REP_TGT            :4;
    uint32_t dummy1             :28;
    uint16_t SUB_IW;
    uint16_t SUB_IH;
    uint32_t SYS_COL_BYTES;
    uint32_t SYS_ROW_BYTES;
    uint32_t ODMA_W_SYS_IDMA_EC :4;
    uint32_t XC_W_SYS_IDMA_EC   :4;
    uint32_t TF_R_IBUF_IDMA_EC  :4;
    uint32_t IDMA_W_IBUF_TF_EC  :4;
} DMAin_TensorActivation_Go;



typedef struct PACKED {
    uint32_t SPA;
    uint32_t DPA;
    uint16_t LENGTH;
    uint32_t ZeroFillHeight  :1;
    uint32_t ZeroFillWidth   :1;
    uint16_t Dummy           :14;
} PtileReq;



typedef struct PACKED {
    MessageHdr hdr;
    uint8_t    NREQS;
    uint32_t   LASTCHREQ             :1;
    uint32_t   CBIT                  :1;
    uint32_t   dummy1                :6;
    uint32_t   REP_TGT               :4;
    uint32_t   dummy2                :12;
    uint16_t   NCPB;
    uint32_t   ODMA_W_SYS_IDMA_EC    :4;
    uint32_t   XC_W_SYS_IDMA_EC      :4;
    uint32_t   TF_R_IBUF_IDMA_EC     :4;
    uint32_t   IDMA_W_IBUF_TF_EC     :4;
    PtileReq   REQS[PTILE_REQ_ARR_SIZE];
} DMAin_PTILEActivation_Go;



typedef struct PACKED {
    MessageHdr hdr;
    uint8_t    NREQS;
    uint32_t   LASTCHREQ             :1;
    uint32_t   CBIT                  :1;
    uint32_t   dummy1                :6;
    uint32_t   REP_TGT               :4;
    uint32_t   dummy2                :12;
    uint16_t   NCPB;
    uint32_t   ODMA_W_SYS_IDMA_EC    :4;
    uint32_t   XC_W_SYS_IDMA_EC      :4;
    uint32_t   TF_R_IBUF_IDMA_EC     :4;
    uint32_t   IDMA_W_IBUF_TF_EC     :4;
    PtileReq   REQS[PTILE_REQ_ARR_SIZE];
} DMAin_FCActivation_Go;



typedef struct PACKED {
    MessageHdr hdr;
    uint8_t    NREQS;
    uint32_t   LASTCHREQ           :1;
    uint32_t   CBIT                :1;
    uint32_t   dummy1              :6;
    uint32_t   ODMA_W_SYS_IDMA_EC  :4;
    uint32_t   XC_W_SYS_IDMA_EC    :4;
    uint32_t   VPU_R_OBUF_IDMA_EC  :4;
    uint32_t   ODMA_R_OBUF_IDMA_EC :4;
    uint32_t   IDMA_W_OBUF_VPU_EC  :4;
    uint32_t   CMPRS               :1;
    uint32_t   dummy2              :11;
    uint16_t   NCPB;
    PtileReq REQS[PTILE_REQ_ARR_SIZE];
} DMAin_OBUF_Go;



typedef struct PACKED {
    MessageHdr hdr;
    uint8_t    NREQS;
    uint32_t   LASTCHREQ         :1;
    uint32_t   CBIT              :1;
    uint32_t   dummy1            :6;
    uint32_t   REP_TGT           :4;
    uint32_t   dummy2            :12;
    uint32_t   TF_R_CBUF_CDMA_EC :4;
    uint32_t   CDMA_W_CBUF_TF_EC :4;
    uint32_t   dummy3            :24;
    PtileReq REQS[PTILE_REQ_ARR_SIZE];
} DMAin_Coefficient_Go;



typedef struct PACKED {
    MessageHdr hdr;
    uint8_t    NREQS;
    uint32_t   LASTCHREQ          :1;
    uint32_t   CBIT               :1;
    uint32_t   dummy1             :6;
    uint32_t   dummy2             :4;
    uint32_t   dummy3             :12;
    uint32_t   VPU_R_OBUF_IDMA_EC :4;
    uint32_t   IDMA_W_OBUF_VPU_EC :4;
    uint32_t   dummy4             :24;
    PtileReq REQS[PTILE_REQ_ARR_SIZE];
} DMAin_FW_Go;



typedef struct PACKED {
    MessageHdr  hdr;
    eLayerClass LAYERCL           :3;
    uint32_t    dummy0            :1;
    uint32_t    NumBatches_M1     :4;
    eDataType   IDT               :3;
    uint32_t    SS_TF_M1          :1;
    eDataType   CDT               :3;
    uint32_t    SM_TF_M1          :1;
    eDataType   ODT               :3;
    uint32_t    RoundMode         :1;
    uint32_t    EnAQAct           :1;
    uint32_t    EnAQFilter        :1;
    uint32_t    EnFilterLUT       :1;
    uint32_t    FilterComp        :1;
    uint8_t     ZD                :8;
    uint8_t     ZF                :8;
    uint32_t    RW_M1             :4;
    uint32_t    RH_M1             :4;
    uint32_t    STW_M1            :2;
    uint32_t    STH_M1            :2;
    uint32_t    SS_MBLK_M1        :1;
    uint32_t    SM_MBLK_M1        :1;
    uint32_t    SS_QBLK_M1        :1;
    uint32_t    SM_QBLK_M1        :1;
    uint32_t    DW_M1             :4;
    uint32_t    DH_M1             :4;
    uint32_t    dummy11;
    uint32_t    EnMaxPooling      :1;
    uint32_t    EnEltwiseAdd      :1;
    uint32_t    dummy12           :14;
    uint16_t    OBStepW;
    uint16_t    OBStepH;
    uint16_t    OBStepD;
    uint32_t    M;
    uint16_t    dummy3;
    uint32_t    NumFilters        :2;
    uint32_t    dummy4            :22;
    uint32_t    ApplyRelu         :1;
    uint32_t    ApplyMpyScale     :1;
    uint32_t    ApplyStickySat    :1;
#if XCHAL_XNNE_VERSION > 1
    uint32_t    dummy5            :1;
    uint32_t    MBLKEn            :8;
#else
    uint32_t    dummy5            :5;
    uint32_t    MBLKEn            :4;
#endif
    uint32_t    rowEn             :4;
    uint32_t    colEn             :4;
    uint32_t    dummy6            :4;
    uint32_t    BINSTRIDE;
} TF_Layer_Config;



typedef struct PACKED {
    uint32_t LUT   : 9;
    uint32_t dummy : 7;
} TF_LUT_Entry;

typedef struct PACKED {
    MessageHdr   hdr;
    uint32_t LUT0    : 9;
    uint32_t dummy0  : 7;
    uint32_t LUT1    : 9;
    uint32_t dummy1  : 7;
    uint32_t LUT2    : 9;
    uint32_t dummy2  : 7;
    uint32_t LUT3    : 9;
    uint32_t dummy3  : 7;
    uint32_t LUT4    : 9;
    uint32_t dummy4  : 7;
    uint32_t LUT5    : 9;
    uint32_t dummy5  : 7;
    uint32_t LUT6    : 9;
    uint32_t dummy6  : 7;
    uint32_t LUT7    : 9;
    uint32_t dummy7  : 7;
    uint32_t LUT8    : 9;
    uint32_t dummy8  : 7;
    uint32_t LUT9    : 9;
    uint32_t dummy9  : 7;
    uint32_t LUT10   : 9;
    uint32_t dummy10 : 7;
    uint32_t LUT11   : 9;
    uint32_t dummy11 : 7;
    uint32_t LUT12   : 9;
    uint32_t dummy12 : 7;
    uint32_t LUT13   : 9;
    uint32_t dummy13 : 7;
    uint32_t LUT14   : 9;
    uint32_t dummy14 : 7;
    uint32_t LUT15   : 9;
    uint32_t dummy15 : 7;
} TF_Set_LUT;


typedef struct PACKED {
    MessageHdr hdr;
} TF_SwitchLayer;



typedef struct PACKED {
    MessageHdr hdr;
    uint32_t   IBaseAddr;
    uint32_t   CBaseAddr;
    uint32_t   OBaseAddr;
    uint32_t   IBaseOffsetAlt;
    uint32_t   UncmprSzFilterSet;
#if XCHAL_XNNE_VERSION > 1
    uint16_t   IBStepH_B;
    uint16_t   IBStepH_T;
    uint16_t   IBStepW_L;
    uint16_t   IBStepW_R;
#else
    uint32_t   IBStepH;
    uint32_t   IBStepW;
#endif
    uint16_t   SubC;
    uint32_t   RecircFormat      :1;
    uint32_t   InputReLU         :1;
    uint32_t   StartRow          :2;
    uint32_t   MultipleGoSequence :1;
    uint16_t   dummy0            :11;
    uint32_t   EnPADW_L          :1;
    uint32_t   EnPADW_R          :1;
    uint32_t   EnPADH_T          :1;
    uint32_t   EnPADH_B          :1;
    uint32_t   FullC             :1;
    uint32_t   FiltGrpStart      :1;
    uint32_t   AccSpill          :1;
    uint32_t   AccFill           :1;
    uint32_t   IDMA_W_IBUF_TF_EC   :4;
    uint32_t   ODMA_W_IBUF_TF_EC   :4;
    uint32_t   CDMA_W_CBUF_TF_EC   :4;
    uint32_t   VPU_R_OBUF_TF_EC    :4;
    uint32_t   ODMA_R_OBUF_TF_EC   :4;
    uint32_t   TF_R_IBUF_ODMA_EC   :4;
    uint32_t   TF_R_IBUF_IDMA_EC   :4;
    uint32_t   TF_R_CBUF_CDMA_EC   :4;
    uint32_t   QBLK_W_OBUF_ODMA_EC :4;
    uint32_t   QBLK_W_OBUF_VPU_EC  :4;
#if XCHAL_XNNE_VERSION > 1
    uint16_t   IBStepH_B2;
#else
    uint16_t   dummy2;
#endif
    uint32_t   SUB_IW;
    uint32_t   SUB_IH;
    uint32_t   PADW_L              :5;
    uint32_t   dummy3              :3;
    uint32_t   PADW_R              :5;
    uint32_t   dummy4              :3;
    uint32_t   PADH_T              :5;
    uint32_t   dummy5              :3;
    uint32_t   PADH_B              :5;
    uint32_t   dummy7              :3;
    uint32_t   PadValue;
    uint16_t   OutputPadValue;
    uint16_t   dummy8;
} TF_Go;



typedef struct PACKED {
    MessageHdr hdr;
    uint8_t    NREQS;
    uint32_t   CMPRS                :1;
    uint32_t   WFence               :1;
    uint32_t   dummy1               :6;
    uint32_t   QBLK_W_OBUF_ODMA_EC : 4;
    uint32_t   VPU_W_OBUF_ODMA_EC : 4;
    uint32_t   ODMA_R_OBUF_IDMA_EC :4;
    uint32_t   ODMA_R_OBUF_TF_EC   :4;
    uint32_t   ODMA_R_OBUF_VPU_EC  :4;
    uint32_t   ODMA_W_SYS_IDMA_EC  :4;
    uint32_t   ODMA_W_SYS_XC       :4;
    uint32_t   dummy2              :4;
    uint32_t   ODMA_W_SYS_IDMA_BC  :4;
    uint32_t   Reserved            :4;
    uint32_t   QBLK_W_OBUF_ODMA_EN :8;
    PtileReq   REQS[PTILE_REQ_ARR_SIZE];
} DMAout_SysM_Go;



typedef struct PACKED {
    MessageHdr hdr;
    uint8_t    NREQS;
    uint8_t    dummy1;
    uint16_t   C;
    uint16_t   PTD;
    uint32_t   QBLK_W_OBUF_ODMA_EC  :4;
    uint32_t   VPU_W_OBUF_ODMA_EC   :4;
    uint32_t   TF_R_IBUF_ODMA_EC    :4;
    uint32_t   ODMA_R_OBUF_IDMA_EC  :4;
    uint32_t   ODMA_R_OBUF_TF_EC    :4;
    uint32_t   ODMA_R_OBUF_VPU_EC   :4;
    uint32_t   ODMA_W_IBUF_TF_EC    :4;
    uint32_t   dummy2               :4;
    uint32_t   QBLK_W_OBUF_ODMA_EN  :8;
    uint32_t   dummy3               :8;
    PtileReq   REQS[PTILE_REQ_ARR_SIZE];
} DMAout_IBUF_Go;


typedef struct PACKED {
    MessageHdr hdr;
    uint8_t    NREQS;
    uint32_t   dummy0               :1;
    uint32_t   WFence               :1;
    uint32_t   dummy1               :6;
    uint32_t   SUB_C                :16;
    uint32_t   SUB_IW               :16;
    uint32_t   SUB_IH               :16;
    uint32_t   SYS_COL_BYTES        :32;
    uint32_t   SYS_ROW_BYTES        :32;
    uint32_t   QBLK_W_OBUF_ODMA_EC  :4;
    uint32_t   VPU_W_OBUF_ODMA_EC   :4;
    uint32_t   ODMA_R_OBUF_IDMA_EC  :4;
    uint32_t   ODMA_R_OBUF_TF_EC    :4;
    uint32_t   ODMA_R_OBUF_VPU_EC   :4;
    uint32_t   ODMA_W_SYS_IDMA_EC   :4;
    uint32_t   ODMA_W_SYS_XC        :4;
    uint32_t   dummy2               :4;
    uint32_t   ODMA_W_SYS_IDMA_BC   :4;
    uint32_t   Reserved             :4;
    uint32_t   QBLK_W_OBUF_ODMA_EN  :8;
    uint32_t   PTD                  :8;
    uint32_t   dummy4               :8;
    PtileReq   REQS[PTILE_REQ_ARR_SIZE - 1];
} DMAout_TensorSysM_Go;

// Unified struct for PTILE DMAs from system memory to access REQS fields.
typedef struct PACKED {
    MessageHdr hdr;
    uint64_t   dummy;
    PtileReq   REQS[PTILE_REQ_ARR_SIZE];
} DMAin_PTILE_Go;


typedef struct PACKED {
    uint32_t SBLKID :4;
    uint32_t RIDX   :2;
    uint32_t dummy  :2;
    uint8_t  MSGID;
    uint16_t LENGTH;
} DMAout_Response;


typedef struct PACKED {
    uint32_t : 4;
} FOURBITS;

/* Pooling config message */
typedef struct PACKED {
    MessageHdr hdr;
    eVPUType   FNTYPE      :8;
    ePoolType  POOLTYPE    :4;
    eDataType  IDT         :3;
    eDataType  ODT         :3;
    uint32_t   dummy1      :6;
    uint32_t   RW_M1       :4;
    uint32_t   RH_M1       :4;
    uint32_t   STW_M1      :8;
    uint32_t   STH_M1      :8;
    uint16_t   PTD;
    uint8_t    dummy2;
    uint8_t    OUT_SHIFT;
    uint16_t   OUT_SCALE;
    uint32_t   dummy3;
    uint32_t   dummy4;
} VPU_Pool_Layer_Config;

/* Eltwise, Relu, and Renorm config message */
typedef struct PACKED {
    MessageHdr hdr;
    eVPUType   FNTYPE      :8;
    eEltwisePrecType    ELTWISE_PREC    :4;
    eDataType  IDT         :3;
    eDataType  ODT         :3;
    uint32_t   Relu        :1;
    uint32_t   dummy1      :13;
    uint32_t   STW_M1      :8;
    uint32_t   STH_M1      :8;
    uint16_t   PTD;
    uint8_t    ACC_SHIFT;
    uint8_t    OUT_SHIFT;
    uint16_t   OUT_SCALE;
    uint16_t   IN_SCALE_1;
    uint16_t   IN_SCALE_2;
    uint16_t   MIN_VAL;
    uint16_t   MAX_VAL;
} VPU_Group1_Layer_Config;

/* Sigmoid, TanH, Crop, Deconvolve_interleave config message */
typedef struct PACKED {
    MessageHdr hdr;
    eVPUType   FNTYPE      :8;
    eModeType  MODE        :4;
    eDataType  IDT         :3;
    eDataType  ODT         :3;
    uint32_t   dummy2      :14;
    uint32_t   STW_M1      :8;
    uint32_t   STH_M1      :8;
    uint16_t   PTD;
    uint16_t   IN_SCALE;
    uint8_t    IN_SHIFT;
    uint8_t    SHIFT;
    uint16_t   OUT_SCALE;
    uint8_t    OUT_SHIFT;
    uint8_t    dummy3;
    uint32_t   dummy4;
} VPU_Group2_Layer_Config;

/* Binning and FC deinterleave */
typedef struct PACKED {
    MessageHdr hdr;
    eVPUType   FNTYPE      :8;
    uint32_t   dummy1      :4;
    eDataType  IDT         :3;
    eDataType  ODT         :3;
    uint32_t   Relu        :1;
    uint32_t   dummy2      :13;
    uint32_t   STW_M1      :8;
    uint32_t   STH_M1      :8;
    uint16_t   PTD;
    uint32_t   dummy3;
    uint32_t   dummy4;
    uint32_t   PAD_VALUE   :8;
    uint32_t   dummy5      :24;
} VPU_Group3_Layer_Config;

typedef struct PACKED {
    MessageHdr hdr;
    eVPUType   FNTYPE      :8;
    uint32_t   PADDING     :2;
    uint32_t   PADW_L      :2;
    uint32_t   PADW_R      :2;
    uint32_t   PADH_T      :2;
    uint32_t   PADH_B      :2;
    uint32_t   POOL_TYPE_SELECT :2;
    uint32_t   dummy1      :12;
    uint32_t   SrcAddr0;
    uint32_t   SrcAddr1;
    uint32_t   TgtAddr;
    uint32_t   IW;
    uint32_t   IH;
    uint32_t   OW;
    uint32_t   OH;
    uint32_t   M;
    uint16_t   IPTW_OFFSET;
    uint16_t   IPTH_OFFSET;
    uint16_t   OPTW_OFFSET;
    uint16_t   OPTH_OFFSET;
    uint16_t   X_OFFSET;
    uint16_t   Y_OFFSET;
    uint16_t   X_END;
    uint16_t   Y_END;
    uint32_t   IDMA_W_OBUF_VPU_EC :4;
    uint32_t   QBLK_W_OBUF_VPU_EC :4;
    uint32_t   ODMA_R_OBUF_VPU_EC :4;
    uint32_t   VPU_R_OBUF_TF_EC   :4;
    uint32_t   VPU_R_OBUF_IDMA_EC :4;
    uint32_t   VPU_W_OBUF_ODMA_EC :4;
    uint32_t   QBLK_W_OBUF_VPU_EN :8;
} VPU_Go;


typedef struct PACKED _xnne_cmd_hdr {
    uint32_t              msg_num     :8;
    uint32_t              dummy1      :24;
    struct _xnne_cmd_hdr  *next;
    uint32_t              dummy2[13];
    uint32_t              dummy3      :31;
    uint32_t              interrupt   :1;
} xnne_cmd_hdr;


typedef union {
    MessageHdr                  mHdr;
    DMAin_Layer_Config          mDMAin_Layer_Config;
    DMAin_TensorActivation_Go   mDMAin_TensorActivation_Go;
    DMAin_PTILEActivation_Go    mDMAin_PTILEActivation_Go;
    DMAin_FCActivation_Go       mDMAin_FCActivation_Go;
    DMAin_OBUF_Go               mDMAin_OBUF_Go;
    DMAin_Coefficient_Go        mDMAin_Coefficient_Go;
    DMAin_FW_Go                 mDMAin_FW_Go;
    DMAin_PTILE_Go              mDMAin_PTILE_Go;
    TF_Layer_Config             mTF_Layer_Config;
    TF_Set_LUT                  mTF_Set_LUT;
    TF_SwitchLayer              mTF_SwitchLayer;
    TF_Go                       mTF_Go;
    VPU_Pool_Layer_Config       mVPU_Pool_Layer_Config;
    VPU_Group1_Layer_Config     mVPU_Group1_Layer_Config;
    VPU_Group2_Layer_Config     mVPU_Group2_Layer_Config;
    VPU_Group3_Layer_Config     mVPU_Group3_Layer_Config;
    VPU_Go                      mVPU_Go;
    DMAout_SysM_Go              mDMAout_SysM_Go;
    DMAout_IBUF_Go              mDMAout_IBUF_Go;
    DMAout_TensorSysM_Go        mDMAout_TensorSysM_Go;
    xnne_cmd_hdr                mCmdHdr;
#ifdef __XTENSA__
    xb_vec2Nx8U                 mvec;
#else
    uint64_t                    mvec;
#endif
} DNA100_Msg __attribute__((aligned(sizeof(xb_vec2Nx8U))));


typedef union {
    DMAout_Response  mDMAout;
    uint32_t         mval;
} DNA100_Resp;


typedef union {
    struct PACKED {
        uint16_t num_sblks;
        uint16_t cmp_mtile_size;
        uint32_t addr_fixup_idx;
        uint32_t cmp_idx[PTILE_REQ_ARR_SIZE];
        uint32_t cmp_start_idx;
        uint32_t cmp_dir_size;
    } meta;
#ifdef __XTENSA__
    xb_vecNx8U   mvec;
#else
    uint64_t     mvec;
#endif
} DNA100_Meta __attribute__((aligned(sizeof(xb_vecNx8U))));


typedef union {
    struct PACKED {
        uint32_t arid     :16;
        uint32_t dummy    :15;
        uint32_t enable   :1;
    } mCtrl;
    uint32_t mInt;
} xnne_cmd_ctrl;

typedef struct {
    unsigned int queue_size;
    unsigned int queue_free_slots;
    unsigned int msg_pushed;
    unsigned int message_id;
} xnne_unit_state_t;

typedef struct {
    uint32_t *table[256];
    uint32_t count;
} xnne_cmpr_state_t;

typedef struct {
    DNA100_Msg  *sys_ptr;
    DNA100_Msg  *loc_ptr[2];
    DNA100_Msg  *buf_ptr[2];
    DNA100_Meta *meta_ptr;
    uint32_t    idx;
} xnne_msg_fetch_t;

typedef struct {
    xnne_unit_state_t unit[XCHAL_XNNE_NUM_SBLKS][eUNIT_LAST + 1];
    xnne_cmpr_state_t resp[XCHAL_XNNE_NUM_SBLKS];
    xnne_msg_fetch_t  fetch;
    int               num_sblks;
    int               num_sblks_to_sync;
} xnne_state_t;


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __XTENSA__

#define XNNE_SBLK_BROADCAST                   15

#define XNNE_SBLK_REG_APERTURE                0x10000
#define XNNE_UNIT_REG_APERTURE                0x01000

// Use odd numbers for broadcast messages and even numbers for non-broadcast messages.
// FIXME: These seem to add extra few cycles of overhead per message (comparing to simple increment).
//        May be a good idea to create a special TIE instruction for them.
#define XNNE_MSGID_NEXT_EVEN(id)              ((id + 2) & ~1)
#define XNNE_MSGID_NEXT_ODD(id)               ((id + 1) | 1)

#define XNNE_READ_GOMSGCNT(sblk, unit)        (XNNE_READ(sblk, unit, 0x00) & 0xFFFF)
#define XNNE_READ_GOMSGLAST(sblk, unit)       (XNNE_READ(sblk, unit, 0x04) & 0xFFFF)
#define XNNE_READ_GOMSGCOMPLETE(sblk, unit)   (XNNE_READ(sblk, unit, 0x08) & 0xFFFF)
#define XNNE_READ_MQCAPACITY(sblk, unit)      (XNNE_READ(sblk, unit, 0x80) & 0xFFFF)
#define XNNE_READ_ERRORSTATUS(sblk)           (XNNE_READ(sblk, eSHARED, 0x20) & 0xBF)
#define XNNE_READ_CMDPULLCTRL(sblk)           (XNNE_READ(sblk, eSHARED, 0x34))

#if XCHAL_XNNE_VERSION > 1
#define XNNE_READ_FW_VERSION(sblk)            (XNNE_READ(sblk, eVPU, 0x7C))
#endif

#define XNNE_WRITE_DISABLE(sblk, flag)        (XNNE_WRITE(sblk, eSHARED, 0x08, (flag) & 0x1))
#define XNNE_WRITE_CMDPULLADDR(sblk, addr)    (XNNE_WRITE(sblk, eSHARED, 0x30, addr))
#define XNNE_WRITE_CMDPULLCTRL(sblk, ctrl)    (XNNE_WRITE(sblk, eSHARED, 0x34, ctrl))

INLINE eXNNEUnit xnne_unit(unsigned int physch, unsigned int cmd)
{
    return physch + 1 + (physch == ePCH_TF || cmd == eCMD_DMAin_Coefficient_Go);
}

INLINE eXNNEUnit xnne_unit_from_msg(const DNA100_Msg *msg)
{
    return xnne_unit(msg->mHdr.PHYSCH, msg->mHdr.CMD);
}

INLINE uint32_t XNNE_READ(int sblk, eXNNEUnit unit, unsigned int addr) {
    return XNNE_RD(sblk * XNNE_SBLK_REG_APERTURE + unit * XNNE_UNIT_REG_APERTURE + addr);
}

INLINE void XNNE_WRITE(int sblk, eXNNEUnit unit, unsigned int addr, uint32_t value) {
    XNNE_WR(sblk * XNNE_SBLK_REG_APERTURE + unit * XNNE_UNIT_REG_APERTURE + addr, value);
}

INLINE void xnne_init_responses(xnne_state_t *state, uint32_t **resps, int sblk, uint8_t msgid, uint8_t nreqs) {
    // Compute hash using the following assumptions:
    //  * MSGID is 8 bit unsigned, RIDX is 2 bit unsigned
    //  * Message broadcasting is not allowed for ODMA => MSGID is always even => MSGID LSB is always 0
    //  * ODMA message queue is << 128 => MSGID MSB can be ignored safely
    // Taking into account everything above, 8 bit hash can be used to store unique MSGID and RIDX info.
    assert(sblk < XCHAL_XNNE_NUM_SBLKS && "Unexpected SBLKID");
    assert(msgid % 2 == 0 && "Unexpected MSGID");
    assert(nreqs <= PTILE_REQ_ARR_SIZE && "Unexpected number of requests");
    xnne_cmpr_state_t *st = &state->resp[sblk];
    const uint8_t hash_base = (msgid << 1) & 0xFC;
    for (int i = 0; i < PTILE_REQ_ARR_SIZE; i++) {
        st->table[hash_base + i] = resps[i];
    }
    st->count += nreqs;
}

INLINE void xnne_read_responses(xnne_state_t *state) {
    while (!XNNE_RQ_EMPTY()) {
        DNA100_Resp resp;
        resp.mval = XNNE_RQ_POP();
        xnne_cmpr_state_t *st = &state->resp[resp.mDMAout.SBLKID];
        const uint8_t hash = (resp.mDMAout.MSGID << 1) | resp.mDMAout.RIDX;
        *st->table[hash] = resp.mDMAout.LENGTH;
        st->count--;
    }
}

INLINE XI_ERR_TYPE xnne_wait_responses(xnne_state_t *state) {
    XI_ERROR_CHECKS();
#if CNNRT_PERF_LEVEL != CNNRT_PERF_LEVEL_NONE
    uint32_t _t_cycles = _time_stamp();
#endif
    for (int i = 0; i < XCHAL_XNNE_NUM_SBLKS; i++) {
        while (state->resp[i].count > 0) {
            XI_RUN_TIME_CHECK(!XNNE_READ_ERRORSTATUS(i), "XNNE is in error state", XI_ERR_BADARG);
            xnne_read_responses(state);
        }
    }
#if CNNRT_PERF_LEVEL != CNNRT_PERF_LEVEL_NONE
    _cnnrt_perf_idleWaitCycles += _time_stamp() - _t_cycles;
#endif
    return XI_ERROR_STATUS();
}

INLINE unsigned int xnne_messages_to_process(xnne_state_t *state, int sblk, eXNNEUnit unit) {
    return (state->unit[sblk][unit].msg_pushed - XNNE_READ_GOMSGCNT(sblk, unit)) & 0xFFFF;
}

INLINE int xnne_unit_queue_is_full (xnne_state_t *state, int sblk, eXNNEUnit unit) {
    if (state->unit[sblk][unit].queue_free_slots == 0u) {
        HINT_NEVER;
        state->unit[sblk][unit].queue_free_slots =
            state->unit[sblk][unit].queue_size - xnne_messages_to_process(state, sblk, unit);
    }
    return state->unit[sblk][unit].queue_free_slots == 0u;
}

INLINE int xnne_unit_is_busy (xnne_state_t *state, int sblk, eXNNEUnit unit) {
    if (unit == eODMA) {
        HINT_NEVER;
        return (state->unit[sblk][unit].message_id & 0xFF) != XNNE_READ_GOMSGCOMPLETE(sblk, unit);
    } else {
        return (state->unit[sblk][unit].message_id & 0xFF) != XNNE_READ_GOMSGLAST(sblk, unit);
    }
}

INLINE XI_ERR_TYPE xnne_wait_idle(xnne_state_t *state, eXNNEUnit unit) {
    XI_ERROR_CHECKS();
#if CNNRT_PERF_LEVEL != CNNRT_PERF_LEVEL_NONE
    uint32_t _t_cycles = _time_stamp();
#endif
    for (int i = 0; i < state->num_sblks_to_sync; i++) {
        while (xnne_unit_is_busy(state, i, unit)) {
            XI_RUN_TIME_CHECK(!XNNE_READ_ERRORSTATUS(i), "XNNE is in error state", XI_ERR_BADARG);
            xnne_read_responses(state);
        }
        XI_RUN_TIME_CHECK(!XNNE_READ_ERRORSTATUS(i), "XNNE is in error state", XI_ERR_BADARG);
    }
#if CNNRT_PERF_LEVEL != CNNRT_PERF_LEVEL_NONE
    _cnnrt_perf_idleWaitCycles += _time_stamp() - _t_cycles;
#endif
    return XI_ERROR_STATUS();
}

INLINE XI_ERR_TYPE xnne_wait_idle_all(xnne_state_t *state) {
    XI_ERROR_CHECKS();
    for (eXNNEUnit unit = eUNIT_FIRST; unit <= eUNIT_LAST; unit++) {
        xnne_wait_idle(state, unit);
    }
    return XI_ERROR_STATUS();
}

NO_INLINE XI_ERR_TYPE xnne_push(xnne_state_t *state, DNA100_Msg *msg);

NO_INLINE XI_ERR_TYPE xnne_enqueue(xnne_state_t *state, DNA100_Msg *msg);

NO_INLINE XI_ERR_TYPE xnne_enqueue_cmprs(xnne_state_t *state, DNA100_Msg *msg, uint32_t *resps);

#if !defined(XCHAL_XNNE_VERSION) || XCHAL_XNNE_VERSION == 1

NO_INLINE XI_ERR_TYPE xnne_run(xnne_state_t *state, uintptr_t *fixup_table, dir_entry_t *cmp_table,
                               DNA100_Msg *msgs, int msg_num);

#endif  /* !defined(XCHAL_XNNE_VERSION) || if XNNE_VERSION == 1 */

#if XCHAL_XNNE_VERSION > 1

NO_INLINE XI_ERR_TYPE xnne_cmd_pull(xnne_state_t *state, uintptr_t *fixup_table, dir_entry_t *cmp_table,
                                    DNA100_Msg *msgs, DNA100_Msg *buff, int blk_num);

#endif  /* if XCHAL_XNNE_VERSION > 1 */


#else /* CSTUBs */

INLINE XI_ERR_TYPE xnne_push(xnne_state_t *state, DNA100_Msg *msg) {
    return XI_ERR_OK;
}

INLINE XI_ERR_TYPE xnne_enqueue(xnne_state_t *state, DNA100_Msg *msg) {
    return XI_ERR_OK;
}

INLINE XI_ERR_TYPE xnne_enqueue_cmprs(xnne_state_t *state, DNA100_Msg *msg, uint32_t *resps) {
    return XI_ERR_OK;
}

INLINE XI_ERR_TYPE xnne_wait_idle(xnne_state_t *state, eXNNEUnit unit) {
    return XI_ERR_OK;
}

INLINE XI_ERR_TYPE xnne_wait_idle_all(xnne_state_t *state) {
    return XI_ERR_OK;
}

INLINE XI_ERR_TYPE xnne_wait_responses(xnne_state_t *state) {
    return XI_ERR_OK;
}

INLINE void xnne_read_responses(xnne_state_t *state) {}

INLINE XI_ERR_TYPE xnne_run(xnne_state_t *state, uintptr_t *fixup_table, dir_entry_t *cmp_table,
                            DNA100_Msg *msgs, int msg_num) {
    return XI_ERR_OK;
}

INLINE XI_ERR_TYPE xnne_cmd_pull(xnne_state_t *state, uintptr_t *fixup_table, dir_entry_t *cmp_table,
                                 DNA100_Msg *msgs, DNA100_Msg *buff, int blk_num) {
    return XI_ERR_OK;
}

#endif /* CSTUBs end */

void xnne_state_init(xnne_state_t *state);

INLINE void xnne_set_num_sblks(xnne_state_t *state, int sblks) {
    state->num_sblks = sblks;
}

XI_ERR_TYPE xnne_wait_all(xnne_state_t *state);


#if XCHAL_XNNE_VERSION > 1

#  define  XFW_FW_VERSION    ((_cnnrt_firmware_version & 0xFFFF) | ((XCHAL_HAVE_MEM_ECC_PARITY) ? 0x80000000 : 0))

extern uint32_t _cnnrt_firmware_version;

#endif
extern uint8_t  _cnnrt_firmware_text[];
extern uint32_t _cnnrt_firmware_text_size;
extern uint8_t  _cnnrt_firmware_data[];
extern uint32_t _cnnrt_firmware_data_size;

extern xnne_state_t *_cnnrt_xnne_state;

#ifdef __cplusplus
}
#endif

#endif /* XCHAL_HAVE_XNNE == 1 */

#endif // __CNNRT_DNA100_H__
