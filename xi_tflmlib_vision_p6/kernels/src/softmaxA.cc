/*
 * Copyright (c) 2017 by Cadence Design Systems, Inc.  ALL RIGHTS RESERVED.
 * These coded instructions, statements, and computer programs are the
 * copyrighted works and confidential proprietary information of
 * Cadence Design Systems Inc.  They may be adapted and modified by bona fide
 * purchasers for internal use, but neither the original nor any adapted
 * or modified version may be disclosed or distributed to third parties
 * in any manner, medium, or form, in whole or in part, without the prior
 * written consent of Cadence Design Systems Inc.  This software and its
 * derivatives are to be executed solely on products incorporating a Cadence
 * Design Systems processor.
 */

#include "xi_cnn.h"
#include "xi_intrin.h"
#include <xtensa/tie/xt_ivpn.h>
#include <xtensa/tie/xt_misc.h>
#include "xi_cnn_api.h"
#if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))

#define OUTPUT_BITS      8
#define ONE_Q31          ((1U << 31) - 1)
#define HALF_Q31         ((1U << 31) >> 1)
#define QUARTER_Q31      ((1U << 31) >> 2)
#define ONEEIGHTH_Q31    ((1U << 31) >> 3)

#define QIN_FORMAT_INT   5 // has to be >0
#define QIN_FORMAT_FRAC  (31 - QIN_FORMAT_INT)
#define QUARTER_QIN      ((1 << QIN_FORMAT_FRAC) >> 2)

//#define INPUT_DATA_TYPE  UNSIGNED8BIT
//#include "softmaxA.h"
//#undef INPUT_DATA_TYPE

#define INPUT_DATA_TYPE  SIGNED8BIT
#include "softmaxA.h"
#undef INPUT_DATA_TYPE

//Generates Lookup table on VP6 processor for AndroidNN Softmax implementation on VP6
XI_ERR_TYPE xiSoftmaxA_generateLUT(xi_pArray softmaxArray,
                                   const xi_cnn_softmaxA_params * pparams)
{
 /* Error Checks */
  XI_ERROR_CHECKS()
  {
    XI_CHECK_ARRAY_S32(softmaxArray);
    XI_CHECK_ERROR(XI_ARRAY_GET_WIDTH(softmaxArray) == 256, XI_ERR_BADARG, \
                   "\nsoftmaxArray LUT witdh = %d, size should be equal to 256", XI_ARRAY_GET_WIDTH(softmaxArray));
    XI_CHECK_POINTER(pparams);
  }

  int32_t *LUTOut               = (int32_t *) XI_ARRAY_GET_BUFF_PTR(softmaxArray);
  int32_t input_beta_multiplier = XI_CNN_SOFTMAXA_PARAMS_GET_MULTIPLIER(pparams);//pparams->input_beta_multiplier;
  int32_t input_beta_left_shift = XI_CNN_SOFTMAXA_PARAMS_GET_LEFT_SHIFT(pparams);//pparams->input_beta_left_shift;
  int32_t diff_min              = XI_CNN_SOFTMAXA_PARAMS_GET_DIFF_MIN(pparams);//pparams->diff_min;

  int i;
  xb_vecNx16 vecInpDiff;
  xb_vecN_2x64w wvec0;
  xb_vecN_2x32v hvec0, hvecTemp, hvecDiff;
  xb_vecN_2x32v hvecOneQuarterQ5_26, hvecMask, hvecConst, hvecConst1Over3, hvecX1, hvecX2, hvecX3, hvecX4, hvecX4_Over_4, hvecRes, hvecRes1;
  xb_vecN_2x32v hvecNeginput_mod_quarter_minus_one_quarterQ_31, hvecNeginput_mod_quarter_minus_one_quarterQ5_26, hvecRemainderQ5_26;
  vboolN_2 vb;
  valign vaStore = IVP_ZALIGN();
  vecInpDiff = IVP_MOVNX16_FROMN_2X32(IVP_NEGN_2X32(IVP_SEQN_2X32()));

  xb_vecN_2x32v hvecinput_beta_multiplier = (xb_vecN_2x32v) (input_beta_multiplier);
  xb_vecN_2x32v vecDiffMin                = (xb_vecN_2x32v) - diff_min;
  hvecOneQuarterQ5_26 = (xb_vecN_2x32v) QUARTER_QIN;
  hvecMask            = (xb_vecN_2x32v) hvecOneQuarterQ5_26 - (xb_vecN_2x32v) 1;
  hvecConst           = (xb_vecN_2x32v) 1895147668;
  hvecConst1Over3     = (xb_vecN_2x32v) 715827883;


  xb_vecN_2x32v *phvecOut = (xb_vecN_2x32v *) LUTOut;
  for (i = 0; i < 256; i += XCHAL_IVPN_SIMD_WIDTH / 2)
  {
    vecInpDiff = IVP_MOVNX16_FROMN_2X32(IVP_NEGN_2X32(IVP_SEQN_2X32())) - (xb_vecNx16) i;
    wvec0      = IVP_MULN_2X16X32_0(vecInpDiff, (xb_vecN_2x32v) (1 << input_beta_left_shift));
    hvec0      = IVP_PACKLN_2X64W(wvec0);
    wvec0      = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecinput_beta_multiplier), hvec0);
    IVP_MULAHN_2X16X32_1(wvec0, IVP_MOVNX16_FROMN_2X32(hvecinput_beta_multiplier), hvec0);
    hvecDiff = IVP_PACKVRN_2X64W(wvec0, 31);
    //start of negative values
    hvecNeginput_mod_quarter_minus_one_quarterQ5_26 = IVP_SUBN_2X32(IVP_ANDN_2X32(hvecDiff, hvecMask), hvecOneQuarterQ5_26);
    hvecNeginput_mod_quarter_minus_one_quarterQ_31  = hvecNeginput_mod_quarter_minus_one_quarterQ5_26 << QIN_FORMAT_INT;
    hvecRemainderQ5_26                              = hvecNeginput_mod_quarter_minus_one_quarterQ5_26 - hvecDiff;
    // Start of exp_on_interval_between_negative_one_quarter_and_0_excl

    hvecX1 = hvecNeginput_mod_quarter_minus_one_quarterQ_31 + (xb_vecN_2x32v) ONEEIGHTH_Q31;
    wvec0  = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecX1), hvecX1);
    IVP_MULAHN_2X16X32_1(wvec0, IVP_MOVNX16_FROMN_2X32(hvecX1), hvecX1);
    hvecX2 = IVP_PACKVRN_2X64W(wvec0, 31);
    //x3
    wvec0 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecX1), hvecX2);
    IVP_MULAHN_2X16X32_1(wvec0, IVP_MOVNX16_FROMN_2X32(hvecX1), hvecX2);
    hvecX3 = IVP_PACKVRN_2X64W(wvec0, 31);
    //x4
    wvec0 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecX2), hvecX2);
    IVP_MULAHN_2X16X32_1(wvec0, IVP_MOVNX16_FROMN_2X32(hvecX2), hvecX2);
    hvecX4 = IVP_PACKVRN_2X64W(wvec0, 31);

    wvec0 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecX4), (xb_vecN_2x32v) QUARTER_Q31);
    IVP_MULAHN_2X16X32_1(wvec0, IVP_MOVNX16_FROMN_2X32(hvecX4), (xb_vecN_2x32v) QUARTER_Q31);
    hvecX4_Over_4 = IVP_PACKVRN_2X64W(wvec0, 31);

    hvecTemp = hvecX4_Over_4 + hvecX3;
    wvec0    = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecConst1Over3), hvecTemp);
    IVP_MULAHN_2X16X32_1(wvec0, IVP_MOVNX16_FROMN_2X32(hvecConst1Over3), hvecTemp);
    hvecTemp = IVP_PACKVRN_2X64W(wvec0, 31);
    hvecTemp = hvecTemp + hvecX2;
    wvec0    = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecTemp), (xb_vecN_2x32v) HALF_Q31);
    IVP_MULAHN_2X16X32_1(wvec0, IVP_MOVNX16_FROMN_2X32(hvecTemp), (xb_vecN_2x32v) HALF_Q31);
    hvecTemp = IVP_PACKVRN_2X64W(wvec0, 31);
    hvecTemp = hvecTemp + hvecX1;
    wvec0    = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecConst), hvecTemp);
    IVP_MULAHN_2X16X32_1(wvec0, IVP_MOVNX16_FROMN_2X32(hvecConst), hvecTemp);
    hvecTemp = IVP_PACKVRN_2X64W(wvec0, 31);
    hvecRes  = hvecTemp + hvecConst;

    vb    = IVP_NEQN_2X32(IVP_ANDN_2X32(hvecRemainderQ5_26, (xb_vecN_2x32v) (1 << (QIN_FORMAT_FRAC - 2))), (xb_vecN_2x32v) 0);
    wvec0 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecRes), (xb_vecN_2x32v) 1672461947);
    IVP_MULAHN_2X16X32_1(wvec0, IVP_MOVNX16_FROMN_2X32(hvecRes), (xb_vecN_2x32v) 1672461947);
    hvecRes1 = IVP_PACKVRN_2X64W(wvec0, 31);
    hvecRes  = IVP_MOVN_2X32T(hvecRes1, hvecRes, vb);

    vb    = IVP_NEQN_2X32(IVP_ANDN_2X32(hvecRemainderQ5_26, (xb_vecN_2x32v) (1 << (QIN_FORMAT_FRAC - 1))), (xb_vecN_2x32v) 0);
    wvec0 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecRes), (xb_vecN_2x32v) 1302514674);
    IVP_MULAHN_2X16X32_1(wvec0, IVP_MOVNX16_FROMN_2X32(hvecRes), (xb_vecN_2x32v) 1302514674);
    hvecRes1 = IVP_PACKVRN_2X64W(wvec0, 31);
    hvecRes  = IVP_MOVN_2X32T(hvecRes1, hvecRes, vb);


    vb    = IVP_NEQN_2X32(IVP_ANDN_2X32(hvecRemainderQ5_26, (xb_vecN_2x32v) (1 << (QIN_FORMAT_FRAC))), (xb_vecN_2x32v) 0);
    wvec0 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecRes), (xb_vecN_2x32v) 790015084);
    IVP_MULAHN_2X16X32_1(wvec0, IVP_MOVNX16_FROMN_2X32(hvecRes), (xb_vecN_2x32v) 790015084);
    hvecRes1 = IVP_PACKVRN_2X64W(wvec0, 31);
    hvecRes  = IVP_MOVN_2X32T(hvecRes1, hvecRes, vb);


    vb    = IVP_NEQN_2X32(IVP_ANDN_2X32(hvecRemainderQ5_26, (xb_vecN_2x32v) (1 << (QIN_FORMAT_FRAC + 1))), (xb_vecN_2x32v) 0);
    wvec0 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecRes), (xb_vecN_2x32v) 290630308);
    IVP_MULAHN_2X16X32_1(wvec0, IVP_MOVNX16_FROMN_2X32(hvecRes), (xb_vecN_2x32v) 290630308);
    hvecRes1 = IVP_PACKVRN_2X64W(wvec0, 31);
    hvecRes  = IVP_MOVN_2X32T(hvecRes1, hvecRes, vb);

    vb    = IVP_NEQN_2X32(IVP_ANDN_2X32(hvecRemainderQ5_26, (xb_vecN_2x32v) (1 << (QIN_FORMAT_FRAC + 2))), (xb_vecN_2x32v) 0);
    wvec0 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecRes), (xb_vecN_2x32v) 39332535);
    IVP_MULAHN_2X16X32_1(wvec0, IVP_MOVNX16_FROMN_2X32(hvecRes), (xb_vecN_2x32v) 39332535);
    hvecRes1 = IVP_PACKVRN_2X64W(wvec0, 31);
    hvecRes  = IVP_MOVN_2X32T(hvecRes1, hvecRes, vb);

    vb    = IVP_NEQN_2X32(IVP_ANDN_2X32(hvecRemainderQ5_26, (xb_vecN_2x32v) (1 << (QIN_FORMAT_FRAC + 3))), (xb_vecN_2x32v) 0);
    wvec0 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecRes), (xb_vecN_2x32v) 720401);
    IVP_MULAHN_2X16X32_1(wvec0, IVP_MOVNX16_FROMN_2X32(hvecRes), (xb_vecN_2x32v) 720401);
    hvecRes1 = IVP_PACKVRN_2X64W(wvec0, 31);
    hvecRes  = IVP_MOVN_2X32T(hvecRes1, hvecRes, vb);

    vb    = IVP_NEQN_2X32(IVP_ANDN_2X32(hvecRemainderQ5_26, (xb_vecN_2x32v) (1 << (QIN_FORMAT_FRAC + 4))), (xb_vecN_2x32v) 0);
    wvec0 = IVP_MULUSN_2X16X32_0(IVP_MOVNX16_FROMN_2X32(hvecRes), (xb_vecN_2x32v) 242);
    IVP_MULAHN_2X16X32_1(wvec0, IVP_MOVNX16_FROMN_2X32(hvecRes), (xb_vecN_2x32v) 242);
    hvecRes1 = IVP_PACKVRN_2X64W(wvec0, 31);
    hvecRes  = IVP_MOVN_2X32T(hvecRes1, hvecRes, vb);


    hvecRes = IVP_MOVN_2X32T(hvecRes, IVP_ZERON_2X32(), IVP_GEN_2X32(vecDiffMin, (xb_vecN_2x32v) i + IVP_SEQN_2X32()));
    hvecRes = IVP_MOVN_2X32T((xb_vecN_2x32v) ONE_Q31, hvecRes, IVP_EQN_2X32(IVP_SEQN_2X32() + (xb_vecN_2x32v) i, IVP_ZERON_2X32()));
    IVP_SAN_2X32_IP(hvecRes, vaStore, phvecOut);
  }
  IVP_SAPOSN_2X32_FP(vaStore, phvecOut);

  xb_vecN_2x32v *hvecLUTBase = (xb_vecN_2x32v *) LUTOut;
  xb_vecN_2x32v *phvecOut1   = (xb_vecN_2x32v *) LUTOut;
  valign va1                 = IVP_LAN_2X32_PP(hvecLUTBase);
  vaStore = IVP_ZALIGN();
  xb_vecN_2x32v hvecLUT0, hvecLUT1, hvecLUT2, hvecLUT3, hvecLUT4, hvecLUT5, hvecLUT6, hvecLUT7;
  xb_vecN_2x32v hvecLUT8, hvecLUT9, hvecLUT10, hvecLUT11, hvecLUT12, hvecLUT13, hvecLUT14, hvecLUT15;
  IVP_LAN_2X32_IP(hvecLUT0, va1, hvecLUTBase);
  IVP_LAN_2X32_IP(hvecLUT1, va1, hvecLUTBase);
  IVP_LAN_2X32_IP(hvecLUT2, va1, hvecLUTBase);
  IVP_LAN_2X32_IP(hvecLUT3, va1, hvecLUTBase);
  IVP_LAN_2X32_IP(hvecLUT4, va1, hvecLUTBase);
  IVP_LAN_2X32_IP(hvecLUT5, va1, hvecLUTBase);
  IVP_LAN_2X32_IP(hvecLUT6, va1, hvecLUTBase);
  IVP_LAN_2X32_IP(hvecLUT7, va1, hvecLUTBase);
  IVP_LAN_2X32_IP(hvecLUT8, va1, hvecLUTBase);
  IVP_LAN_2X32_IP(hvecLUT9, va1, hvecLUTBase);
  IVP_LAN_2X32_IP(hvecLUT10, va1, hvecLUTBase);
  IVP_LAN_2X32_IP(hvecLUT11, va1, hvecLUTBase);
  IVP_LAN_2X32_IP(hvecLUT12, va1, hvecLUTBase);
  IVP_LAN_2X32_IP(hvecLUT13, va1, hvecLUTBase);
  IVP_LAN_2X32_IP(hvecLUT14, va1, hvecLUTBase);
  IVP_LAN_2X32_IP(hvecLUT15, va1, hvecLUTBase);


  IVP_DSELN_2X32I(hvecLUT1, hvecLUT0, hvecLUT1, hvecLUT0, IVP_DSELI_DEINTERLEAVE_1);
  IVP_DSELN_2X32I(hvecLUT3, hvecLUT2, hvecLUT3, hvecLUT2, IVP_DSELI_DEINTERLEAVE_1);
  IVP_DSELN_2X32I(hvecLUT5, hvecLUT4, hvecLUT5, hvecLUT4, IVP_DSELI_DEINTERLEAVE_1);
  IVP_DSELN_2X32I(hvecLUT7, hvecLUT6, hvecLUT7, hvecLUT6, IVP_DSELI_DEINTERLEAVE_1);

  //vec0 , vec4 contians LL ; vec2,vec6 contians LH
  IVP_DSELN_2X32I(hvecLUT2, hvecLUT0, hvecLUT2, hvecLUT0, IVP_DSELI_8B_DEINTERLEAVE_1);
  IVP_DSELN_2X32I(hvecLUT6, hvecLUT4, hvecLUT6, hvecLUT4, IVP_DSELI_8B_DEINTERLEAVE_1);
  //vec1 , vec5 contians HL ; vec3,vec7 contians HH
  IVP_DSELN_2X32I(hvecLUT3, hvecLUT1, hvecLUT3, hvecLUT1, IVP_DSELI_8B_DEINTERLEAVE_1);
  IVP_DSELN_2X32I(hvecLUT7, hvecLUT5, hvecLUT7, hvecLUT5, IVP_DSELI_8B_DEINTERLEAVE_1);



  IVP_DSELN_2X32I(hvecLUT9, hvecLUT8, hvecLUT9, hvecLUT8, IVP_DSELI_DEINTERLEAVE_1);
  IVP_DSELN_2X32I(hvecLUT11, hvecLUT10, hvecLUT11, hvecLUT10, IVP_DSELI_DEINTERLEAVE_1);
  IVP_DSELN_2X32I(hvecLUT13, hvecLUT12, hvecLUT13, hvecLUT12, IVP_DSELI_DEINTERLEAVE_1);
  IVP_DSELN_2X32I(hvecLUT15, hvecLUT14, hvecLUT15, hvecLUT14, IVP_DSELI_DEINTERLEAVE_1);

  //vec0 , vec4 contians LL ; vec2,vec6 contians LH
  IVP_DSELN_2X32I(hvecLUT10, hvecLUT8, hvecLUT10, hvecLUT8, IVP_DSELI_8B_DEINTERLEAVE_1);
  IVP_DSELN_2X32I(hvecLUT14, hvecLUT12, hvecLUT14, hvecLUT12, IVP_DSELI_8B_DEINTERLEAVE_1);
  //vec1 , vec5 contians HL ; vec3,vec7 contians HH
  IVP_DSELN_2X32I(hvecLUT11, hvecLUT9, hvecLUT11, hvecLUT9, IVP_DSELI_8B_DEINTERLEAVE_1);
  IVP_DSELN_2X32I(hvecLUT15, hvecLUT13, hvecLUT15, hvecLUT13, IVP_DSELI_8B_DEINTERLEAVE_1);

  IVP_SAN_2X32_IP(hvecLUT0, vaStore, phvecOut1);
  IVP_SAN_2X32_IP(hvecLUT4, vaStore, phvecOut1);
  IVP_SAN_2X32_IP(hvecLUT8, vaStore, phvecOut1);
  IVP_SAN_2X32_IP(hvecLUT12, vaStore, phvecOut1);
  IVP_SAN_2X32_IP(hvecLUT2, vaStore, phvecOut1);
  IVP_SAN_2X32_IP(hvecLUT6, vaStore, phvecOut1);
  IVP_SAN_2X32_IP(hvecLUT10, vaStore, phvecOut1);
  IVP_SAN_2X32_IP(hvecLUT14, vaStore, phvecOut1);
  IVP_SAN_2X32_IP(hvecLUT1, vaStore, phvecOut1);
  IVP_SAN_2X32_IP(hvecLUT5, vaStore, phvecOut1);
  IVP_SAN_2X32_IP(hvecLUT9, vaStore, phvecOut1);
  IVP_SAN_2X32_IP(hvecLUT13, vaStore, phvecOut1);
  IVP_SAN_2X32_IP(hvecLUT3, vaStore, phvecOut1);
  IVP_SAN_2X32_IP(hvecLUT7, vaStore, phvecOut1);
  IVP_SAN_2X32_IP(hvecLUT11, vaStore, phvecOut1);
  IVP_SAN_2X32_IP(hvecLUT15, vaStore, phvecOut1);
  IVP_SAPOSN_2X32_FP(vaStore, phvecOut1);



  return XI_ERROR_STATUS();
}

#endif //if ((XCHAL_VISION_TYPE >= 6) && defined(INCLUDE_XI_CNN))
