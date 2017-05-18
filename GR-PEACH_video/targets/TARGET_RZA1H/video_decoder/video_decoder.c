/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
* Copyright (C) 2012 - 2015 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**************************************************************************//**
* @file         video_decoder.c
* @version      1.00
* $Rev: 199 $
* $Date:: 2014-05-23 16:33:52 +0900#$
* @brief        Video decoder function
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    <stdlib.h>
#include    <string.h>

#include    "r_typedefs.h"
#include    "iodefine.h"

#include    "r_vdec.h"
#include    "r_vdec_user.h"

#include    "video_decoder.h"


/******************************************************************************
Macro definitions
******************************************************************************/
#define     STBCR7_BIT7                 (0x80u)
#define     STBCR7_BIT6                 (0x40u)

/*! Video decoder recommended value */
#define SRCLEFT_NTSC_3_58       ((uint16_t)256u)
#define SRCLEFT_NTSC_4_43       ((uint16_t)256u)
#define SRCLEFT_PAL_4_43        ((uint16_t)256u)
#define SRCLEFT_PAL_M           ((uint16_t)256u)
#define SRCLEFT_PAL_N           ((uint16_t)256u)
#define SRCLEFT_SECAM           ((uint16_t)256u)
#define SRCLEFT_NTSC_443_60     ((uint16_t)256u)
#define SRCLEFT_PAL_60          ((uint16_t)256u)

#define SRCTOP_NTSC_3_58        ((uint16_t)16u)
#define SRCTOP_NTSC_4_43        ((uint16_t)19u)
#define SRCTOP_PAL_4_43         ((uint16_t)19u)
#define SRCTOP_PAL_M            ((uint16_t)16u)
#define SRCTOP_PAL_N            ((uint16_t)19u)
#define SRCTOP_SECAM            ((uint16_t)19u)
#define SRCTOP_NTSC_443_60      ((uint16_t)16u)
#define SRCTOP_PAL_60           ((uint16_t)16u)

#define SRCHEIGHT_NTSC_3_58     ((uint16_t)241u)
#define SRCHEIGHT_NTSC_4_43     ((uint16_t)288u)
#define SRCHEIGHT_PAL_4_43      ((uint16_t)288u)
#define SRCHEIGHT_PAL_M         ((uint16_t)241u)
#define SRCHEIGHT_PAL_N         ((uint16_t)288u)
#define SRCHEIGHT_SECAM         ((uint16_t)288u)
#define SRCHEIGHT_NTSC_443_60   ((uint16_t)241u)
#define SRCHEIGHT_PAL_60        ((uint16_t)241u)

#define SRCWIDTH_NTSC_3_58      ((uint16_t)1428u)
#define SRCWIDTH_NTSC_4_43      ((uint16_t)1412u)
#define SRCWIDTH_PAL_4_43       ((uint16_t)1412u)
#define SRCWIDTH_PAL_M          ((uint16_t)1428u)
#define SRCWIDTH_PAL_N          ((uint16_t)1412u)
#define SRCWIDTH_SECAM          ((uint16_t)1412u)
#define SRCWIDTH_NTSC_443_60    ((uint16_t)1428u)
#define SRCWIDTH_PAL_60         ((uint16_t)1428u)

#define HAFCTYP_NTSC_3_58       ((uint16_t)692u)
#define HAFCTYP_NTSC_4_43       ((uint16_t)704u)
#define HAFCTYP_PAL_4_43        ((uint16_t)704u)
#define HAFCTYP_PAL_M           ((uint16_t)692u)
#define HAFCTYP_PAL_N           ((uint16_t)704u)
#define HAFCTYP_SECAM           ((uint16_t)704u)
#define HAFCTYP_NTSC_443_60     ((uint16_t)692u)
#define HAFCTYP_PAL_60          ((uint16_t)692u)

#define HAFCMAX_NTSC_3_58       ((uint16_t)792u)
#define HAFCMAX_NTSC_4_43       ((uint16_t)785u)
#define HAFCMAX_PAL_4_43        ((uint16_t)785u)
#define HAFCMAX_PAL_M           ((uint16_t)792u)
#define HAFCMAX_PAL_N           ((uint16_t)785u)
#define HAFCMAX_SECAM           ((uint16_t)785u)
#define HAFCMAX_NTSC_443_60     ((uint16_t)792u)
#define HAFCMAX_PAL_60          ((uint16_t)792u)

#define HAFCMIN_NTSC_3_58       ((uint16_t)592u)
#define HAFCMIN_NTSC_4_43       ((uint16_t)630u)
#define HAFCMIN_PAL_4_43        ((uint16_t)630u)
#define HAFCMIN_PAL_M           ((uint16_t)592u)
#define HAFCMIN_PAL_N           ((uint16_t)630u)
#define HAFCMIN_SECAM           ((uint16_t)630u)
#define HAFCMIN_NTSC_443_60     ((uint16_t)592u)
#define HAFCMIN_PAL_60          ((uint16_t)592u)

#define VCDWINDOW_NTSC_3_58     ((uint16_t)30u)
#define VCDWINDOW_NTSC_4_43     ((uint16_t)30u)
#define VCDWINDOW_PAL_4_43      ((uint16_t)30u)
#define VCDWINDOW_PAL_M         ((uint16_t)30u)
#define VCDWINDOW_PAL_N         ((uint16_t)30u)
#define VCDWINDOW_SECAM         ((uint16_t)30u)
#define VCDWINDOW_NTSC_443_60   ((uint16_t)30u)
#define VCDWINDOW_PAL_60        ((uint16_t)30u)

#define VCDOFFSET_NTSC_3_58     ((uint16_t)15u)
#define VCDOFFSET_NTSC_4_43     ((uint16_t)15u)
#define VCDOFFSET_PAL_4_43      ((uint16_t)15u)
#define VCDOFFSET_PAL_M         ((uint16_t)15u)
#define VCDOFFSET_PAL_N         ((uint16_t)15u)
#define VCDOFFSET_SECAM         ((uint16_t)15u)
#define VCDOFFSET_NTSC_443_60   ((uint16_t)15u)
#define VCDOFFSET_PAL_60        ((uint16_t)15u)

#define ACCLEVEL_NTSC_3_58      ((uint16_t)220u)
#define ACCLEVEL_NTSC_4_43      ((uint16_t)220u)
#define ACCLEVEL_PAL_4_43       ((uint16_t)220u)
#define ACCLEVEL_PAL_M          ((uint16_t)230u)
#define ACCLEVEL_PAL_N          ((uint16_t)230u)
#define ACCLEVEL_SECAM          ((uint16_t)220u)
#define ACCLEVEL_NTSC_443_60    ((uint16_t)220u)
#define ACCLEVEL_PAL_60         ((uint16_t)230u)

#define AGCLEVEL_NTSC_3_58      ((uint16_t)230u)
#define AGCLEVEL_NTSC_4_43      ((uint16_t)230u)
#define AGCLEVEL_PAL_4_43       ((uint16_t)242u)
#define AGCLEVEL_PAL_M          ((uint16_t)242u)
#define AGCLEVEL_PAL_N          ((uint16_t)242u)
#define AGCLEVEL_SECAM          ((uint16_t)242u)
#define AGCLEVEL_NTSC_443_60    ((uint16_t)230u)
#define AGCLEVEL_PAL_60         ((uint16_t)242u)

#define K15_NTSC_3_58           ((uint16_t)2u)
#define K15_NTSC_4_43           ((uint16_t)2u)
#define K15_PAL_4_43            ((uint16_t)2u)
#define K15_PAL_M               ((uint16_t)2u)
#define K15_PAL_N               ((uint16_t)2u)
#define K15_SECAM               ((uint16_t)2u)
#define K15_NTSC_443_60         ((uint16_t)2u)
#define K15_PAL_60              ((uint16_t)2u)

#define K13_NTSC_3_58           ((uint16_t)8u)
#define K13_NTSC_4_43           ((uint16_t)8u)
#define K13_PAL_4_43            ((uint16_t)8u)
#define K13_PAL_M               ((uint16_t)8u)
#define K13_PAL_N               ((uint16_t)8u)
#define K13_SECAM               ((uint16_t)8u)
#define K13_NTSC_443_60         ((uint16_t)8u)
#define K13_PAL_60              ((uint16_t)8u)

#define K11_NTSC_3_58           ((uint16_t)4u)
#define K11_NTSC_4_43           ((uint16_t)4u)
#define K11_PAL_4_43            ((uint16_t)3u)
#define K11_PAL_M               ((uint16_t)3u)
#define K11_PAL_N               ((uint16_t)3u)
#define K11_SECAM               ((uint16_t)4u)
#define K11_NTSC_443_60         ((uint16_t)4u)
#define K11_PAL_60              ((uint16_t)3u)

#define K16_NTSC_3_58           ((uint16_t)3u)
#define K16_NTSC_4_43           ((uint16_t)3u)
#define K16_PAL_4_43            ((uint16_t)4u)
#define K16_PAL_M               ((uint16_t)4u)
#define K16_PAL_N               ((uint16_t)4u)
#define K16_SECAM               ((uint16_t)3u)
#define K16_NTSC_443_60         ((uint16_t)3u)
#define K16_PAL_60              ((uint16_t)4u)

#define K14_NTSC_3_58           ((uint16_t)16u)
#define K14_NTSC_4_43           ((uint16_t)16u)
#define K14_PAL_4_43            ((uint16_t)63u)
#define K14_PAL_M               ((uint16_t)63u)
#define K14_PAL_N               ((uint16_t)63u)
#define K14_SECAM               ((uint16_t)16u)
#define K14_NTSC_443_60         ((uint16_t)16u)
#define K14_PAL_60              ((uint16_t)63u)

#define K12_NTSC_3_58           ((uint16_t)8u)
#define K12_NTSC_4_43           ((uint16_t)8u)
#define K12_PAL_4_43            ((uint16_t)2u)
#define K12_PAL_M               ((uint16_t)2u)
#define K12_PAL_N               ((uint16_t)2u)
#define K12_SECAM               ((uint16_t)1u)
#define K12_NTSC_443_60         ((uint16_t)8u)
#define K12_PAL_60              ((uint16_t)2u)

#define K22A_NTSC_3_58          ((uint16_t)32u)
#define K22A_NTSC_4_43          ((uint16_t)32u)
#define K22A_PAL_4_43           ((uint16_t)32u)
#define K22A_PAL_M              ((uint16_t)32u)
#define K22A_PAL_N              ((uint16_t)32u)
#define K22A_SECAM              ((uint16_t)32u)
#define K22A_NTSC_443_60        ((uint16_t)32u)
#define K22A_PAL_60             ((uint16_t)32u)

#define K21A_NTSC_3_58          ((uint16_t)6u)
#define K21A_NTSC_4_43          ((uint16_t)6u)
#define K21A_PAL_4_43           ((uint16_t)10u)
#define K21A_PAL_M              ((uint16_t)10u)
#define K21A_PAL_N              ((uint16_t)10u)
#define K21A_SECAM              ((uint16_t)10u)
#define K21A_NTSC_443_60        ((uint16_t)6u)
#define K21A_PAL_60             ((uint16_t)10u)

#define K22B_NTSC_3_58          ((uint16_t)8u)
#define K22B_NTSC_4_43          ((uint16_t)8u)
#define K22B_PAL_4_43           ((uint16_t)15u)
#define K22B_PAL_M              ((uint16_t)15u)
#define K22B_PAL_N              ((uint16_t)15u)
#define K22B_SECAM              ((uint16_t)15u)
#define K22B_NTSC_443_60        ((uint16_t)8u)
#define K22B_PAL_60             ((uint16_t)15u)

#define K21B_NTSC_3_58          ((uint16_t)6u)
#define K21B_NTSC_4_43          ((uint16_t)6u)
#define K21B_PAL_4_43           ((uint16_t)10u)
#define K21B_PAL_M              ((uint16_t)10u)
#define K21B_PAL_N              ((uint16_t)10u)
#define K21B_SECAM              ((uint16_t)6u)
#define K21B_NTSC_443_60        ((uint16_t)6u)
#define K21B_PAL_60             ((uint16_t)10u)

#define K23B_NTSC_3_58          ((uint16_t)6u)
#define K23B_NTSC_4_43          ((uint16_t)6u)
#define K23B_PAL_4_43           ((uint16_t)3u)
#define K23B_PAL_M              ((uint16_t)3u)
#define K23B_PAL_N              ((uint16_t)3u)
#define K23B_SECAM              ((uint16_t)3u)
#define K23B_NTSC_443_60        ((uint16_t)6u)
#define K23B_PAL_60             ((uint16_t)3u)

#define K23A_NTSC_3_58          ((uint16_t)3u)
#define K23A_NTSC_4_43          ((uint16_t)3u)
#define K23A_PAL_4_43           ((uint16_t)3u)
#define K23A_PAL_M              ((uint16_t)3u)
#define K23A_PAL_N              ((uint16_t)3u)
#define K23A_SECAM              ((uint16_t)3u)
#define K23A_NTSC_443_60        ((uint16_t)3u)
#define K23A_PAL_60             ((uint16_t)3u)

#define K24_NTSC_3_58           ((uint16_t)5u)
#define K24_NTSC_4_43           ((uint16_t)5u)
#define K24_PAL_4_43            ((uint16_t)8u)
#define K24_PAL_M               ((uint16_t)8u)
#define K24_PAL_N               ((uint16_t)8u)
#define K24_SECAM               ((uint16_t)8u)
#define K24_NTSC_443_60         ((uint16_t)5u)
#define K24_PAL_60              ((uint16_t)8u)

/* SyncSep */
#define SYNCMAXDUTY_H_VALUE  ((uint16_t)15u)
#define SYNCMINDUTY_H_VALUE  ((uint16_t)10u)
#define CSYNCSLICE_H_VALUE   ((uint16_t)146u)
#define SYNCMAXDUTY_V_VALUE  ((uint16_t)15u)
#define SYNCMINDUTY_V_VALUE  ((uint16_t)9u)
#define VSYNCSLICE_VALUE     ((uint16_t)10u)
#define CSYNCSLICE_V_VALUE   ((uint16_t)146u)
#define HAFCGAIN_VALUE       ((uint16_t)12u)
#define HAFCSTART_VALUE      ((uint16_t)0u)
#define HAFCEND_VALUE        ((uint16_t)8u)
#define AGCRESPONSE_VALUE    ((uint16_t)4u)
#define AGCPRECIS_VALUE      ((uint16_t)10u)
#define PGA_GAIN_VALUE       ((uint16_t)0u)
#define PEAKATTACK_VALUE     ((uint16_t)2u)
#define PEAKRELEASE_VALUE    ((uint16_t)3u)
#define MAXPEAKSAMPLES_VALUE ((uint16_t)20u)

/* YcSep */
#define RADJ_O_LEVEL0_VALUE  ((uint16_t)928u)
#define RADJ_U_LEVEL0_VALUE  ((uint16_t)32u)
#define RADJ_O_LEVEL1_VALUE  ((uint16_t)960u)
#define RADJ_U_LEVEL1_VALUE  ((uint16_t)48u)
#define RADJ_O_LEVEL2_VALUE  ((uint16_t)992u)
#define RADJ_U_LEVEL2_VALUE  ((uint16_t)64u)
#define V_Y_LEVEL_VALUE      ((uint16_t)0u)

/* ChromaDec */
#define LUMADELAY_VALUE      ((uint16_t)0u)
#define LOOPGAIN_VALUE       ((uint16_t)3u)
#define LOCKLIMIT_VALUE      ((uint16_t)1u)
#define BGPCHECK_VALUE       ((uint16_t)0u)
#define BGPWIDTH_VALUE       ((uint16_t)54u)
#define BGPSTART_VALUE       ((uint16_t)110u)
#define CHROMAMAINGAIN_VALUE ((uint16_t)210u)
#define ACCRESPONSE_VALUE    ((uint16_t)1u)
#define ACCPRECIS_VALUE      ((uint16_t)8u)
#define KILLERLEVEL_VALUE    ((uint16_t)4u)
#define KILLEROFFSET_VALUE   ((uint16_t)5u)

/* DigitalClamp */
#define BLANKLEVEL_Y_VALUE   ((uint16_t)984u)
#define DCPPOS_Y_VALUE       ((uint16_t)162u)
#define BLANKLEVEL_CB_VALUE  ((uint16_t)0u)
#define BLANKLEVEL_CR_VALUE  ((uint16_t)0u)
#define DCPPOS_C_VALUE       ((uint16_t)54u)
#define ACFLAGTIME_VALUE     ((uint16_t)0u)
#define ACFFILTER_VALUE      ((uint16_t)3u)
#define DCPRESPONSE_VALUE    ((uint16_t)0u)
#define DCPSTART_VALUE       ((uint16_t)16u)
#define DCPEND_VALUE         ((uint16_t)16u)
#define DCPWIDTH_VALUE       ((uint16_t)27u)

/* Output */
#define Y_GAIN2_VALUE        ((uint16_t)816u)
#define CB_GAIN2_VALUE       ((uint16_t)663u)
#define CR_GAIN2_VALUE       ((uint16_t)663u)


/******************************************************************************
Typedef definitions
******************************************************************************/
/*! Video decoder recommended value */
typedef struct {
    /* Capturing position setting */
    uint16_t            srcleft;
    uint16_t            srctop;
    uint16_t            srcheight;
    uint16_t            srcwidth;
    /* Horizontal AFC setting */
    uint16_t            hafctyp;
    uint16_t            hafcmax;
    uint16_t            hafcmin;
    /* Vertical countdown setting */
    vdec_onoff_t        novcd50_;
    vdec_onoff_t        novcd60_;
    vdec_vcddefault_t   vcddefault;
    uint16_t            vcdwindow;
    uint16_t            vcdoffset;
    /* BCO setting */
    vdec_color_sys_t    defaultsys;
    vdec_onoff_t        nontsc358_;
    vdec_onoff_t        nontsc443_;
    vdec_onoff_t        nopalm_;
    vdec_onoff_t        nopaln_;
    vdec_onoff_t        nopal443_;
    vdec_onoff_t        nosecam_;
    /* ACC level setting */
    uint16_t            acclevel;
    /* AGC level setting */
    uint16_t            agclevel;
    /* Y/C separation setting */
    uint16_t            k15;
    uint16_t            k13;
    uint16_t            k11;
    uint16_t            k16;
    uint16_t            k14;
    uint16_t            k12;
    uint16_t            k22A;
    uint16_t            k21A;
    uint16_t            k22B;
    uint16_t            k21B;
    uint16_t            k23B;
    uint16_t            k23A;
    uint16_t            k24;
    vdec_onoff_t        det2_on;
    vdec_filmix_ratio_t hsel_mix_y;
    vdec_filmix_ratio_t vsel_mix_y;
    vdec_filmix_ratio_t hvsel_mix_y;
    vdec_filmix_ratio_t det2_mix_c;
    vdec_filmix_ratio_t det2_mix_y;
    vdec_2dfil_mdsel_t  fil2_mode_2d;
    vdec_2d_fil_sel_t   fil2_narrow_2d;
} video_recmd_val_t;


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static void Input(const graphics_col_sys_t color_system, const vdec_channel_t channel);
static void SyncSep(const graphics_col_sys_t color_system, const vdec_channel_t channel);
static void YcSep(const graphics_col_sys_t color_system, const vdec_channel_t channel);
static void ChromaDec(const graphics_col_sys_t color_system, const vdec_channel_t channel);
static void DigitalClamp(const vdec_channel_t channel);
static void Output(const vdec_channel_t channel);

static void init_func(const uint32_t user_num);

static const video_recmd_val_t RecommendedValue[GRPH_COL_SYS_NUM] = {
    {   /* NTSC-3.58 */
        /* Capturing position setting */
        SRCLEFT_NTSC_3_58, SRCTOP_NTSC_3_58, SRCHEIGHT_NTSC_3_58, SRCWIDTH_NTSC_3_58,
        /* Horizontal AFC setting */
        HAFCTYP_NTSC_3_58, HAFCMAX_NTSC_3_58, HAFCMIN_NTSC_3_58,
        /* Vertical countdown setting */
        VDEC_OFF, VDEC_ON, VDEC_VCD_FRQ_59_94HZ, VCDWINDOW_NTSC_3_58, VCDOFFSET_NTSC_3_58,
        /* BCO setting */
        VDEC_COL_SYS_NTSC, VDEC_ON, VDEC_OFF, VDEC_OFF, VDEC_OFF, VDEC_OFF, VDEC_OFF,
        /* ACC level setting */
        ACCLEVEL_NTSC_3_58,
        /* AGC level setting */
        AGCLEVEL_NTSC_3_58,
        /* Y/C separation setting */
        K15_NTSC_3_58, K13_NTSC_3_58, K11_NTSC_3_58, K16_NTSC_3_58, K14_NTSC_3_58, K12_NTSC_3_58,
        K22A_NTSC_3_58, K21A_NTSC_3_58, K22B_NTSC_3_58, K21B_NTSC_3_58, K23B_NTSC_3_58, K23A_NTSC_3_58, K24_NTSC_3_58,
        VDEC_ON, VDEC_FILMIX_RATIO_75, VDEC_FILMIX_RATIO_75, VDEC_FILMIX_RATIO_0,
        VDEC_FILMIX_RATIO_0, VDEC_FILMIX_RATIO_25, VDEC_2DFIL_MDSEL_CASCADE, VDEC_2D_FIL_SEL_17TAP
    },
    {   /* NTSC-4.43 */
        /* Capturing position setting */
        SRCLEFT_NTSC_4_43, SRCTOP_NTSC_4_43, SRCHEIGHT_NTSC_4_43, SRCWIDTH_NTSC_4_43,
        /* Horizontal AFC setting */
        HAFCTYP_NTSC_4_43, HAFCMAX_NTSC_4_43, HAFCMIN_NTSC_4_43,
        /* Vertical countdown setting */
        VDEC_ON, VDEC_OFF, VDEC_VCD_FRQ_50HZ, VCDWINDOW_NTSC_4_43, VCDOFFSET_NTSC_4_43,
        /* BCO setting */
        VDEC_COL_SYS_NTSC, VDEC_OFF, VDEC_ON, VDEC_OFF, VDEC_OFF, VDEC_OFF, VDEC_OFF,
        /* ACC level setting */
        ACCLEVEL_NTSC_4_43,
        /* AGC level setting */
        AGCLEVEL_NTSC_4_43,
        /* Y/C separation setting */
        K15_NTSC_4_43, K13_NTSC_4_43, K11_NTSC_4_43, K16_NTSC_4_43, K14_NTSC_4_43, K12_NTSC_4_43,
        K22A_NTSC_4_43, K21A_NTSC_4_43, K22B_NTSC_4_43, K21B_NTSC_4_43, K23B_NTSC_4_43, K23A_NTSC_4_43, K24_NTSC_4_43,
        VDEC_ON, VDEC_FILMIX_RATIO_75, VDEC_FILMIX_RATIO_75, VDEC_FILMIX_RATIO_0,
        VDEC_FILMIX_RATIO_0, VDEC_FILMIX_RATIO_25, VDEC_2DFIL_MDSEL_CASCADE, VDEC_2D_FIL_SEL_17TAP
    },
    {   /* PAL-4.43 */
        /* Capturing position setting */
        SRCLEFT_PAL_4_43, SRCTOP_PAL_4_43, SRCHEIGHT_PAL_4_43, SRCWIDTH_PAL_4_43,
        /* Horizontal AFC setting */
        HAFCTYP_PAL_4_43, HAFCMAX_PAL_4_43, HAFCMIN_PAL_4_43,
        /* Vertical countdown setting */
        VDEC_ON, VDEC_OFF, VDEC_VCD_FRQ_50HZ, VCDWINDOW_PAL_4_43, VCDOFFSET_PAL_4_43,
        /* BCO setting */
        VDEC_COL_SYS_PAL, VDEC_OFF, VDEC_OFF, VDEC_OFF, VDEC_OFF, VDEC_ON, VDEC_OFF,
        /* ACC level setting */
        ACCLEVEL_PAL_4_43,
        /* AGC level setting */
        AGCLEVEL_PAL_4_43,
        /* Y/C separation setting */
        K15_PAL_4_43, K13_PAL_4_43, K11_PAL_4_43, K16_PAL_4_43, K14_PAL_4_43, K12_PAL_4_43,
        K22A_PAL_4_43, K21A_PAL_4_43, K22B_PAL_4_43, K21B_PAL_4_43, K23B_PAL_4_43, K23A_PAL_4_43, K24_PAL_4_43,
        VDEC_OFF, VDEC_FILMIX_RATIO_0, VDEC_FILMIX_RATIO_0, VDEC_FILMIX_RATIO_0,
        VDEC_FILMIX_RATIO_0, VDEC_FILMIX_RATIO_0, VDEC_2DFIL_MDSEL_BYPASS, VDEC_2D_FIL_SEL_17TAP
    },
    {   /* PAL-M */
        /* Capturing position setting */
        SRCLEFT_PAL_M, SRCTOP_PAL_M, SRCHEIGHT_PAL_M, SRCWIDTH_PAL_M,
        /* Horizontal AFC setting */
        HAFCTYP_PAL_M, HAFCMAX_PAL_M, HAFCMIN_PAL_M,
        /* Vertical countdown setting */
        VDEC_OFF, VDEC_ON, VDEC_VCD_FRQ_59_94HZ, VCDWINDOW_PAL_M, VCDOFFSET_PAL_M,
        /* BCO setting */
        VDEC_COL_SYS_PAL, VDEC_OFF, VDEC_OFF, VDEC_ON, VDEC_OFF, VDEC_OFF, VDEC_OFF,
        /* ACC level setting */
        ACCLEVEL_PAL_M,
        /* AGC level setting */
        AGCLEVEL_PAL_M,
        /* Y/C separation setting */
        K15_PAL_M, K13_PAL_M, K11_PAL_M, K16_PAL_M, K14_PAL_M, K12_PAL_M,
        K22A_PAL_M, K21A_PAL_M, K22B_PAL_M, K21B_PAL_M, K23B_PAL_M, K23A_PAL_M, K24_PAL_M,
        VDEC_OFF, VDEC_FILMIX_RATIO_0, VDEC_FILMIX_RATIO_0, VDEC_FILMIX_RATIO_0,
        VDEC_FILMIX_RATIO_0, VDEC_FILMIX_RATIO_0, VDEC_2DFIL_MDSEL_BYPASS, VDEC_2D_FIL_SEL_17TAP
    },
    {   /* PAL-N */
        /* Capturing position setting */
        SRCLEFT_PAL_N, SRCTOP_PAL_N, SRCHEIGHT_PAL_N, SRCWIDTH_PAL_N,
        /* Horizontal AFC setting */
        HAFCTYP_PAL_N, HAFCMAX_PAL_N, HAFCMIN_PAL_N,
        /* Vertical countdown setting */
        VDEC_ON, VDEC_OFF, VDEC_VCD_FRQ_50HZ, VCDWINDOW_PAL_N, VCDOFFSET_PAL_N,
        /* BCO setting */
        VDEC_COL_SYS_PAL, VDEC_OFF, VDEC_OFF, VDEC_OFF, VDEC_ON, VDEC_OFF, VDEC_OFF,
        /* ACC level setting */
        ACCLEVEL_PAL_N,
        /* AGC level setting */
        AGCLEVEL_PAL_N,
        /* Y/C separation setting */
        K15_PAL_N, K13_PAL_N, K11_PAL_N, K16_PAL_N, K14_PAL_N, K12_PAL_N,
        K22A_PAL_N, K21A_PAL_N, K22B_PAL_N, K21B_PAL_N, K23B_PAL_N, K23A_PAL_N, K24_PAL_N,
        VDEC_OFF, VDEC_FILMIX_RATIO_0, VDEC_FILMIX_RATIO_0, VDEC_FILMIX_RATIO_0,
        VDEC_FILMIX_RATIO_0, VDEC_FILMIX_RATIO_0, VDEC_2DFIL_MDSEL_BYPASS, VDEC_2D_FIL_SEL_17TAP
    },
    {   /* SECAM */
        /* Capturing position setting */
        SRCLEFT_SECAM, SRCTOP_SECAM, SRCHEIGHT_SECAM, SRCWIDTH_SECAM,
        /* Horizontal AFC setting */
        HAFCTYP_SECAM, HAFCMAX_SECAM, HAFCMIN_SECAM,
        /* Vertical countdown setting */
        VDEC_ON, VDEC_OFF, VDEC_VCD_FRQ_50HZ, VCDWINDOW_SECAM, VCDOFFSET_SECAM,
        /* BCO setting */
        VDEC_COL_SYS_SECAM, VDEC_OFF, VDEC_OFF, VDEC_OFF, VDEC_OFF, VDEC_OFF, VDEC_ON,
        /* ACC level setting */
        ACCLEVEL_SECAM,
        /* AGC level setting */
        AGCLEVEL_SECAM,
        /* Y/C separation setting */
        K15_SECAM, K13_SECAM, K11_SECAM, K16_SECAM, K14_SECAM, K12_SECAM,
        K22A_SECAM, K21A_SECAM, K22B_SECAM, K21B_SECAM, K23B_SECAM, K23A_SECAM, K24_SECAM,
        VDEC_ON, VDEC_FILMIX_RATIO_75, VDEC_FILMIX_RATIO_75, VDEC_FILMIX_RATIO_0,
        VDEC_FILMIX_RATIO_0, VDEC_FILMIX_RATIO_0, VDEC_2DFIL_MDSEL_CASCADE, VDEC_2D_FIL_SEL_17TAP
    },
    {   /* NTSC-4.43 (60Hz) */
        /* Capturing position setting */
        SRCLEFT_NTSC_443_60, SRCTOP_NTSC_443_60, SRCHEIGHT_NTSC_443_60, SRCWIDTH_NTSC_443_60,
        /* Horizontal AFC setting */
        HAFCTYP_NTSC_443_60, HAFCMAX_NTSC_443_60, HAFCMIN_NTSC_443_60,
        /* Vertical countdown setting */
        VDEC_OFF, VDEC_ON, VDEC_VCD_FRQ_59_94HZ, VCDWINDOW_NTSC_443_60, VCDOFFSET_NTSC_443_60,
        /* BCO setting */
        VDEC_COL_SYS_NTSC, VDEC_OFF, VDEC_ON, VDEC_OFF, VDEC_OFF, VDEC_OFF, VDEC_OFF,
        /* ACC level setting */
        ACCLEVEL_NTSC_443_60,
        /* AGC level setting */
        AGCLEVEL_NTSC_443_60,
        /* Y/C separation setting */
        K15_NTSC_443_60, K13_NTSC_443_60, K11_NTSC_443_60, K16_NTSC_443_60, K14_NTSC_443_60, K12_NTSC_443_60,
        K22A_NTSC_443_60, K21A_NTSC_443_60, K22B_NTSC_443_60, K21B_NTSC_443_60, K23B_NTSC_443_60, K23A_NTSC_443_60,
        K24_NTSC_443_60,
        VDEC_ON, VDEC_FILMIX_RATIO_75, VDEC_FILMIX_RATIO_75, VDEC_FILMIX_RATIO_0,
        VDEC_FILMIX_RATIO_0, VDEC_FILMIX_RATIO_25, VDEC_2DFIL_MDSEL_CASCADE, VDEC_2D_FIL_SEL_17TAP
    },
    {   /* PAL-60 */
        /* Capturing position setting */
        SRCLEFT_PAL_60, SRCTOP_PAL_60, SRCHEIGHT_PAL_60, SRCWIDTH_PAL_60,
        /* Horizontal AFC setting */
        HAFCTYP_PAL_60, HAFCMAX_PAL_60, HAFCMIN_PAL_60,
        /* Vertical countdown setting */
        VDEC_OFF, VDEC_ON, VDEC_VCD_FRQ_59_94HZ, VCDWINDOW_PAL_60, VCDOFFSET_PAL_60,
        /* BCO setting */
        VDEC_COL_SYS_PAL, VDEC_OFF, VDEC_OFF, VDEC_OFF, VDEC_OFF, VDEC_ON, VDEC_OFF,
        /* ACC level setting */
        ACCLEVEL_PAL_60,
        /* AGC level setting */
        AGCLEVEL_PAL_60,
        /* Y/C separation setting */
        K15_PAL_60, K13_PAL_60, K11_PAL_60, K16_PAL_60, K14_PAL_60, K12_PAL_60,
        K22A_PAL_60, K21A_PAL_60, K22B_PAL_60, K21B_PAL_60, K23B_PAL_60, K23A_PAL_60, K24_PAL_60,
        VDEC_OFF, VDEC_FILMIX_RATIO_0, VDEC_FILMIX_RATIO_0, VDEC_FILMIX_RATIO_0,
        VDEC_FILMIX_RATIO_0, VDEC_FILMIX_RATIO_0, VDEC_2DFIL_MDSEL_BYPASS, VDEC_2D_FIL_SEL_17TAP
    }
};
/* Two-Dimensional Cascade Broadband and Narrowband (3.58/4.43) Filter TAP Coefficient */
static const vdec_chrfil_tap_t wana_ntsc = {
    {
        (uint16_t)24u, (uint16_t)44u, (uint16_t)20u, (uint16_t)(0x1000u | 52u), (uint16_t)(0x1000u | 128u),
        (uint16_t)(0x1000u | 128u), (uint16_t)(0x1000u | 12u), (uint16_t)132u, (uint16_t)200u
    }
};
/* Two-Dimensional Cascade Broadband (SECAM-DR) Filter TAP Coefficient */
static const vdec_chrfil_tap_t wa_secam = {
    {
        (uint16_t)(0x1000u | 20u), (uint16_t)24u, (uint16_t)64u, (uint16_t)40u, (uint16_t)(0x1000u | 76u),
        (uint16_t)(0x1000u | 164u), (uint16_t)(0x1000u | 84u), (uint16_t)108u, (uint16_t)216u
    }
};
/* Two-Dimensional Cascade Broadband (SECAM-DB) Filter TAP Coefficient */
static const vdec_chrfil_tap_t wb_secam = {
    {
        (uint16_t)(0x1000u | 12u), (uint16_t)40u, (uint16_t)60u, (uint16_t)12u, (uint16_t)(0x1000u | 104u),
        (uint16_t)(0x1000u | 156u), (uint16_t)(0x1000u | 64u), (uint16_t)120u, (uint16_t)208u
    }
};
/* Two-Dimensional Cascade Narrowband (SECAM-DR) Filter TAP Coefficient */
static const vdec_chrfil_tap_t na_secam = {
    {
        (uint16_t)(0x1000u | 1008u), (uint16_t)1976u, (uint16_t)(0x1000u | 2024u), (uint16_t)444u, (uint16_t)1868u,
        (uint16_t)(0x1000u | 2864u), (uint16_t)1352u, (uint16_t)1376u, (uint16_t)(0x1000u | 2240u)
    }
};
/* Two-Dimensional Cascade Narrowband (SECAM-DB) Filter TAP Coefficient */
static const vdec_chrfil_tap_t nb_secam = {
    {
        (uint16_t)(0x1000u | 1080u), (uint16_t)2800u, (uint16_t)(0x1000u | 3308u), (uint16_t)1628u, (uint16_t)1444u,
        (uint16_t)(0x1000u | 3308u), (uint16_t)2140u, (uint16_t)376u, (uint16_t)(0x1000u | 1384u)
    }
};


/**************************************************************************//**
 * @brief       Video decoder initialization
 * @param       [in]adc_vinsel      : Input pin control
 * @param       [in]channel         : Channel
 * @retval      Detected color system
 *****************************************************************************/
vdec_error_t GRAPHICS_VideoDecoderInit (const vdec_adc_vinsel_t adc_vinsel, const vdec_channel_t channel, graphics_col_sys_t col_sys)
{
    vdec_error_t        error = VDEC_OK;

    /* Video Decoder Driver Initialization */
    error = R_VDEC_Initialize(channel, adc_vinsel, &init_func, (uint32_t)channel);
    if (error == VDEC_OK) {
        if (col_sys != GRPH_COL_SYS_UNKNOWN) {
            /* Input */
            Input(col_sys, channel);
            /* Sync separation */
            SyncSep(col_sys, channel);
            /* Y/C separation */
            YcSep(col_sys, channel);
            /* Chroma decoding */
            ChromaDec(col_sys, channel);
            /* Digital clamp */
            DigitalClamp(channel);
            /* Output */
            Output(channel);
        } else {
            error = VDEC_ERR_PARAM;
        }
    }
    return error;
}   /* End of function GRAPHICS_VideoDecoderInit() */


/**************************************************************************//**
 * @brief       User-defined function
 * @param       [in]user_num
 * @retval      None
 *****************************************************************************/
static void init_func (const uint32_t user_num)
{
    uint32_t            reg_data;
    volatile uint8_t    dummy_read;

    reg_data = (uint32_t)CPG.STBCR7;
    if ((vdec_channel_t)user_num == VDEC_CHANNEL_0) {
        /* Standby control register 7 (STBCR7)
            b7      0-------;  MSTP77 : 0 : Video decoder channel 0 enable */
        reg_data &= (uint32_t)~STBCR7_BIT7;
    } else {
        /* Standby control register 7 (STBCR7)
            b6      -0------;  MSTP76 : 0 : Video decoder channel 1 enable */
        reg_data &= (uint32_t)~STBCR7_BIT6;
    }
    CPG.STBCR7 = (uint8_t)reg_data;
    /* In order to reflect the change, a dummy read should be done. */
    dummy_read = CPG.STBCR7;
#if(1) /* mbed */
    (void)dummy_read; /* unused */
#endif
}   /* End of function init_func() */

/**************************************************************************//**
 * @brief       Video decoder input setup
 * @param       [in]color_system    : Color system
 * @param       [in]channel         : Channel
 * @retval      None
 *****************************************************************************/
static void Input (const graphics_col_sys_t color_system, const vdec_channel_t channel)
{
    vdec_error_t                error;
    vdec_active_period_t        param;
    const video_recmd_val_t   * RecommendValTbl;

    RecommendValTbl = &RecommendedValue[color_system];

    param.srcleft   = RecommendValTbl->srcleft;     /* Left end of input video signal capturing area */
    param.srctop    = RecommendValTbl->srctop;      /* Top end of input video signal capturing area */
    param.srcheight = RecommendValTbl->srcheight;   /* Height of input video signal capturing area */
    param.srcwidth  = RecommendValTbl->srcwidth;    /* Width of input video signal capturing area */
    /* Video decoder driver input */
    error = R_VDEC_ActivePeriod(channel, &param);
    if (error != VDEC_OK) {
    }
}   /* End of function Input() */

/**************************************************************************//**
 * @brief       Video decoder sync separation setup
 * @param       [in]color_system    : Color system
 * @param       [in]channel         : Channel
 * @retval      None
 *****************************************************************************/
static void SyncSep (const graphics_col_sys_t color_system, const vdec_channel_t channel)
{
    vdec_error_t                error;
    vdec_sync_separation_t      param;
    vdec_noise_rd_lpf_t         noise_rd_lpf_tmp;
    vdec_sync_slicer_t          sync_slicer_tmp;
    vdec_horizontal_afc_t       horizontal_afc_tmp;
    vdec_vcount_down_t          vcount_down_tmp;
    vdec_agc_t                  agc_tmp;
    vdec_peak_limiter_t         peak_limiter_tmp;
    const video_recmd_val_t   * RecommendValTbl;

    RecommendValTbl = &RecommendedValue[color_system];

    /* Noise reduction LPF */
    noise_rd_lpf_tmp.lpfvsync   = VDEC_LPF_VSYNC_0_54;      /* LPF cutoff frequency before vertical sync separation */
    noise_rd_lpf_tmp.lpfhsync   = VDEC_LPF_HSYNC_0_94;      /* LPF cutoff frequency before horizontal sync separation */
    /* Sync slicer */
    sync_slicer_tmp.velocityshift_h = VDEC_VELOCITY_SHIFT_4;    /* Reference level operation speed control for
                                                                   composite sync separation (for Hsync signal) */
    sync_slicer_tmp.slicermode_h    = VDEC_SLICE_MODE_AUTO_50;  /* Auto-slice level setting for composite sync
                                                                   separator circuit (for Hsync signal) */
    sync_slicer_tmp.slicermode_v    = VDEC_SLICE_MODE_AUTO_50;  /* Auto-slice level setting for composite sync
                                                                   separation circuit (for Vsync signal) */
    sync_slicer_tmp.syncmaxduty_h   = SYNCMAXDUTY_H_VALUE;      /* Max ratio of horizontal cycle to horizontal sync
                                                                   signal pulse width (for Hsync signal) */
    sync_slicer_tmp.syncminduty_h   = SYNCMINDUTY_H_VALUE;      /* Min ratio of horizontal cycle to horizontal sync
                                                                   signal pulse width (for Hsync signal) */
    sync_slicer_tmp.ssclipsel       = VDEC_CLIP_LV_1023;        /* Clipping level */
    sync_slicer_tmp.csyncslice_h    = CSYNCSLICE_H_VALUE;       /* Slice level for composite sync
                                                                   signal separation (for Hsync signal) */
    sync_slicer_tmp.syncmaxduty_v   = SYNCMAXDUTY_V_VALUE;      /* Max ratio of horizontal cycle to horizontal sync
                                                                   signal pulse width (for Vsync signal) */
    sync_slicer_tmp.syncminduty_v   = SYNCMINDUTY_V_VALUE;      /* Min ratio of horizontal cycle to horizontal sync
                                                                   signal pulse width (for Vsync signal) */
    sync_slicer_tmp.vsyncdelay      = VDEC_OFF;                 /* Delays the separated vertical sync signal for
                                                                   1/4 horizontal cycle */
    sync_slicer_tmp.vsyncslice      = VSYNCSLICE_VALUE;         /* Threshold for vertical sync separation */
    sync_slicer_tmp.csyncslice_v    = CSYNCSLICE_V_VALUE;       /* Slice level for composite sync
                                                                   signal separation (for Vsync signal) */
    /* Horizontal AFC */
    horizontal_afc_tmp.hafcgain     = HAFCGAIN_VALUE;           /* Horizontal AFC loop gain */
    horizontal_afc_tmp.hafctyp      = RecommendValTbl->hafctyp; /* Horizontal AFC center oscillation frequency */
    horizontal_afc_tmp.hafcstart    = HAFCSTART_VALUE;          /* Start line of horizontal AFC normal operation */
    horizontal_afc_tmp.nox2hosc     = VDEC_ON;                  /* Disable of horizontal AFC double speed detection */
    horizontal_afc_tmp.hafcmax      = RecommendValTbl->hafcmax; /* Maximum oscillation frequency of horizontal AFC */
    horizontal_afc_tmp.hafcend      = HAFCEND_VALUE;            /* End line of horizontal AFC normal operation */
    horizontal_afc_tmp.hafcmode     = VDEC_HAFCMD_AUTO_PHST;    /* Horizontal AFC VBI period operating mode */
    horizontal_afc_tmp.hafcmin      = RecommendValTbl->hafcmin; /* Min oscillation frequency of horizontal AFC */
    horizontal_afc_tmp.phdet_fix    = VDEC_OFF;                 /* Forcible or LOWGAIN control */
    horizontal_afc_tmp.phdet_div    = VDEC_PHDET_DIV_1_32;      /* Phase comparator feedback adjust for
                                                                   low sync signal lock stability */
    /* Vertical countdown */
    vcount_down_tmp.novcd50_    = RecommendValTbl->novcd50_;    /* Vertical countdown 50-Hz oscillation mode */
    vcount_down_tmp.novcd60_    = RecommendValTbl->novcd60_;    /* Vertical countdown 60-Hz (59.94-Hz)
                                                                   oscillation mode */
    vcount_down_tmp.vcddefault  = RecommendValTbl->vcddefault;  /* Vertical countdown center oscillation frequency */
    vcount_down_tmp.vcdwindow   = RecommendValTbl->vcdwindow;   /* Vertical countdown sync area */
    vcount_down_tmp.vcdoffset   = RecommendValTbl->vcdoffset;   /* Vertical countdown minimum oscillation frequency */
    /* AGC / PGA */
    agc_tmp.agcmode     = VDEC_ON;                  /* A/D converter AGC ON/OFF control & PGA gain select */
    agc_tmp.doreduce    = VDEC_OFF;                 /* Manual control of sync signal amplitude detection
                                                       during VBI period */
    agc_tmp.noreduce_   = VDEC_ON;                  /* Control of sync signal amplitude detection during VBI period */
    agc_tmp.agcresponse = AGCRESPONSE_VALUE;        /* AGC response speed */
    agc_tmp.agclevel    = RecommendValTbl->agclevel;/* Sync signal reference amplitude */
    agc_tmp.agcprecis   = AGCPRECIS_VALUE;          /* AGC gain adjustment accuracy */
    agc_tmp.pga_gain    = PGA_GAIN_VALUE;           /* PGA gain */
    /* Peak limiter control */
    peak_limiter_tmp.peaklevel      = VDEC_PEAKLV_992;      /* Peak luminance value to operate peak limiter */
    peak_limiter_tmp.peakattack     = PEAKATTACK_VALUE;     /* Response speed with peak limiter gain decreased */
    peak_limiter_tmp.peakrelease    = PEAKRELEASE_VALUE;    /* Response speed with peak limiter gain increased */
    peak_limiter_tmp.peakratio      = VDEC_PEAKRATIO_50;    /* Maximum compression rate of peak limiter */
    peak_limiter_tmp.maxpeaksamples = MAXPEAKSAMPLES_VALUE; /* Allowable number of overflowing pixels */

    param.noise_rd_lpf      = &noise_rd_lpf_tmp;    /* Noise reduction LPF */
    param.sync_slicer       = &sync_slicer_tmp;     /* Sync slicer */
    param.horizontal_afc    = &horizontal_afc_tmp;  /* Horizontal AFC */
    param.vcount_down       = &vcount_down_tmp;     /* Vertical countdown */
    param.agc               = &agc_tmp;             /* AGC / PGA */
    param.peak_limiter      = &peak_limiter_tmp;    /* Peak limiter control */
    /* Video decoder driver sync separation */
    error = R_VDEC_SyncSeparation(channel, &param);
    if (error != VDEC_OK) {
    }
}   /* End of function SyncSep() */

/**************************************************************************//**
 * @brief       Video decoder Y/C separation setup
 * @param       [in]color_system    : Color system
 * @param       [in]channel         : Channel
 * @retval      None
 *****************************************************************************/
static void YcSep (const graphics_col_sys_t color_system, const vdec_channel_t channel)
{
    vdec_error_t                error;
    vdec_yc_separation_t        param;
    vdec_over_range_t           over_range_tmp;
    vdec_yc_sep_ctrl_t          yc_sep_ctrl_tmp;
    const video_recmd_val_t   * RecommendValTbl;

    RecommendValTbl = &RecommendedValue[color_system];

    /* Over-range control */
    over_range_tmp.radj_o_level0    = RADJ_O_LEVEL0_VALUE;   /* A/D over-threshold level (between levels 0 and 1) */
    over_range_tmp.radj_u_level0    = RADJ_U_LEVEL0_VALUE;   /* A/D under-threshold level (between levels 2 and 3) */
    over_range_tmp.radj_o_level1    = RADJ_O_LEVEL1_VALUE;   /* A/D over-threshold level (between levels 1 and 2) */
    over_range_tmp.radj_u_level1    = RADJ_U_LEVEL1_VALUE;   /* A/D under-threshold level (between levels 1 and 2) */
    over_range_tmp.radj_o_level2    = RADJ_O_LEVEL2_VALUE;   /* A/D over-threshold level (between levels 2 and 3) */
    over_range_tmp.radj_u_level2    = RADJ_U_LEVEL2_VALUE;   /* A/D under-threshold level (between levels 0 and 1) */
    over_range_tmp.ucmp_sw          = VDEC_ON;          /* Over-range detection */
    over_range_tmp.dcmp_sw          = VDEC_ON;          /* Under-range detection */
    over_range_tmp.hwide_sw         = VDEC_ON;          /* Horizontal enlargement of over/under-range level */
    /* Y/C separation control */
    /* Two-dimensional Y/C separation filter select coefficient */
    yc_sep_ctrl_tmp.k15             = RecommendValTbl->k15;         /* K15 */
    yc_sep_ctrl_tmp.k13             = RecommendValTbl->k13;         /* K13 */
    yc_sep_ctrl_tmp.k11             = RecommendValTbl->k11;         /* K11 */
    yc_sep_ctrl_tmp.k16             = RecommendValTbl->k16;         /* K16 */
    yc_sep_ctrl_tmp.k14             = RecommendValTbl->k14;         /* K14 */
    yc_sep_ctrl_tmp.k12             = RecommendValTbl->k12;         /* K12 */
    yc_sep_ctrl_tmp.k22a            = RecommendValTbl->k22A;        /* K22A */
    yc_sep_ctrl_tmp.k21a            = RecommendValTbl->k21A;        /* K21A */
    yc_sep_ctrl_tmp.k22b            = RecommendValTbl->k22B;        /* K22B */
    yc_sep_ctrl_tmp.k21b            = RecommendValTbl->k21B;        /* K21B */
    yc_sep_ctrl_tmp.k23b            = RecommendValTbl->k23B;        /* K23B */
    yc_sep_ctrl_tmp.k23a            = RecommendValTbl->k23A;        /* K23A */
    yc_sep_ctrl_tmp.k24             = RecommendValTbl->k24;         /* K24 */
    yc_sep_ctrl_tmp.hbpf_narrow     = VDEC_LSTG_BPFSEL_BYPASS;      /* Latter-stage horizontal BPF select */
    yc_sep_ctrl_tmp.hvbpf_narrow    = VDEC_LSTG_BPFSEL_BYPASS;      /* Latter-stage horizontal/vertical BPF select */
    yc_sep_ctrl_tmp.hbpf1_9tap_on   = VDEC_FSTG_BPFSEL_17TAP;       /* Former-stage horizontal BPF select */
    yc_sep_ctrl_tmp.hvbpf1_9tap_on  = VDEC_FSTG_BPFSEL_17TAP;       /* Former-stage horizontal/vertical BPF select */
    yc_sep_ctrl_tmp.hfil_tap_sel    = VDEC_HFIL_TAP_SEL_17TAP;      /* Horizontal filter and horizontal/vertical filter
                                                                       bandwidth switch signal */
    yc_sep_ctrl_tmp.det2_on         = RecommendValTbl->det2_on;     /* Two-dimensional filter mixing select */
    yc_sep_ctrl_tmp.hsel_mix_y      = RecommendValTbl->hsel_mix_y;  /* Mixing ratio of signal after passing
                                                                       horizontal filter to signal after passing
                                                                       former-stage horizontal filter */
    yc_sep_ctrl_tmp.vsel_mix_y      = RecommendValTbl->vsel_mix_y;  /* Mixing ratio of signal after passing
                                                                       vertical filter to signal after passing
                                                                       former-stage horizontal/vertical filter */
    yc_sep_ctrl_tmp.hvsel_mix_y     = RecommendValTbl->hvsel_mix_y; /* Mixing ratio of signal after passing
                                                                       horizontal/vertical filter to signal
                                                                       after passing former-stage
                                                                       horizontal/vertical filter */
    yc_sep_ctrl_tmp.v_y_level       = V_Y_LEVEL_VALUE;              /* Vertical luminance detection level
                                                                       for correlation detection filter */
    yc_sep_ctrl_tmp.det2_mix_c      = RecommendValTbl->det2_mix_c;  /* Mixing ratio of c signal after passing
                                                                       horizontal/vertical adaptive filter
                                                                       to signal after passing correlation detection
                                                                       filter */
    yc_sep_ctrl_tmp.det2_mix_y      = RecommendValTbl->det2_mix_y;  /* Mixing ratio of c signal for y generation after
                                                                       passing horizontal/vertical adaptive filter to
                                                                       signal after passing correlation */
    yc_sep_ctrl_tmp.fil2_mode_2d    = RecommendValTbl->fil2_mode_2d;/* Two-dimensional cascade/TAKE-OFF
                                                                       filter mode select */
    yc_sep_ctrl_tmp.fil2_narrow_2d  = RecommendValTbl->fil2_narrow_2d;  /* Two-dimensional cascade filter select */

    param.over_range    = &over_range_tmp;  /* Over-range control */
    param.yc_sep_ctrl   = &yc_sep_ctrl_tmp; /* Y/C separation control */
    param.fil2_2d_wa    = NULL;             /* Two-dimensional cascade broadband
                                               (3.58/4.43/SECAM-DR)/TAKE-OFF filter TAP coefficient */
    param.fil2_2d_wb    = NULL;             /* Two-dimensional cascade broadband (SECAM-DB) filter TAP coefficient */
    param.fil2_2d_na    = NULL;             /* Two-dimensional cascade narrowband
                                               (3.58/4.43/SECAM-DR) filter TAP coefficient */
    param.fil2_2d_nb    = NULL;             /* Two-dimensional cascade narrowband (SECAMDB) filter TAP coefficient */
    if ((color_system == GRPH_COL_SYS_NTSC_358) ||
            (color_system == GRPH_COL_SYS_NTSC_443) ||
            (color_system == GRPH_COL_SYS_NTSC_443_60)) {
        param.fil2_2d_wa = &wana_ntsc;
        param.fil2_2d_na = &wana_ntsc;
    } else if (color_system == GRPH_COL_SYS_SECAM) {
        param.fil2_2d_wa = &wa_secam;
        param.fil2_2d_wb = &wb_secam;
        param.fil2_2d_na = &na_secam;
        param.fil2_2d_nb = &nb_secam;
    } else {
        /* Do nothing */
    }
    /* Video decoder driver y/c separation */
    error = R_VDEC_YcSeparation(channel, &param);
    if (error != VDEC_OK) {
    }
}   /* End of function YcSep() */

/**************************************************************************//**
 * @brief       Video decoder chroma decoding setup
 * @param       [in]color_system    : Color system
 * @param       [in]channel         : Channel
 * @retval      None
 *****************************************************************************/
static void ChromaDec (const graphics_col_sys_t color_system, const vdec_channel_t channel)
{
    vdec_error_t                error;
    vdec_chroma_decoding_t      param;
    vdec_chrmdec_ctrl_t         chrmdec_ctrl_tmp;
    vdec_burst_lock_t           burst_lock_tmp;
    vdec_acc_t                  acc_tmp;
    const video_recmd_val_t   * RecommendValTbl;

    RecommendValTbl = &RecommendedValue[color_system];

    /* Chroma decoding control */
    chrmdec_ctrl_tmp.defaultsys = RecommendValTbl->defaultsys;  /* Default color system */
    chrmdec_ctrl_tmp.nontsc358_ = RecommendValTbl->nontsc358_;  /* NTSC-M detection control */
    chrmdec_ctrl_tmp.nontsc443_ = RecommendValTbl->nontsc443_;  /* NTSC-4.43 detection control */
    chrmdec_ctrl_tmp.nopalm_    = RecommendValTbl->nopalm_;     /* PAL-M detection control */
    chrmdec_ctrl_tmp.nopaln_    = RecommendValTbl->nopaln_;     /* PAL-N detection control */
    chrmdec_ctrl_tmp.nopal443_  = RecommendValTbl->nopal443_;   /* PAL-B, G, H, I, D detection control */
    chrmdec_ctrl_tmp.nosecam_   = RecommendValTbl->nosecam_;    /* SECAM detection control */
    chrmdec_ctrl_tmp.lumadelay  = LUMADELAY_VALUE;              /* Luminance signal delay adjustment */
    chrmdec_ctrl_tmp.chromalpf  = VDEC_OFF;                     /* LPF for demodulated chroma */
    chrmdec_ctrl_tmp.demodmode  = VDEC_DEMOD_MD_PAL;            /* Averaging processing for pre-demodulated line */
    /* Burst lock */
    burst_lock_tmp.lockrange    = VDEC_LOCK_RANGE_800HZ;/* Burst lock PLL lock range */
    burst_lock_tmp.loopgain     = LOOPGAIN_VALUE;       /* Burst lock PLL loop gain */
    burst_lock_tmp.locklimit    = LOCKLIMIT_VALUE;      /* Level for burst lock PLL to re-search free-run frequency */
    burst_lock_tmp.bgpcheck     = BGPCHECK_VALUE;       /* burst gate pulse position check */
    burst_lock_tmp.bgpwidth     = BGPWIDTH_VALUE;       /* burst gate pulse width */
    burst_lock_tmp.bgpstart     = BGPSTART_VALUE;       /* burst gate pulse start position */
    /* ACC / color killer */
    acc_tmp.accmode         = VDEC_ACC_MD_AUTO;         /* ACC operating mode */
    acc_tmp.accmaxgain      = VDEC_ACC_MAX_GAIN_6;      /* Maximum ACC Gain */
    acc_tmp.acclevel        = RecommendValTbl->acclevel;/* ACC reference color burst amplitude */
    acc_tmp.chromasubgain   = VDEC_CHRM_SB_GAIN_1;      /* Chroma manual gain (sub) */
    acc_tmp.chromamaingain  = CHROMAMAINGAIN_VALUE;     /* Chroma manual gain (main) */
    acc_tmp.accresponse     = ACCRESPONSE_VALUE;        /* ACC response speed */
    acc_tmp.accprecis       = ACCPRECIS_VALUE;          /* ACC gain adjustment accuracy */
    acc_tmp.killermode      = VDEC_OFF;                 /* Forced color killer mode ON/OFF */
    acc_tmp.killerlevel     = KILLERLEVEL_VALUE;        /* Color killer operation start point */
    acc_tmp.killeroffset    = KILLEROFFSET_VALUE;       /* The offset level to turn off the color killer */

    param.chrmdec_ctrl  = &chrmdec_ctrl_tmp;    /* Chroma decoding control */
    param.burst_lock    = &burst_lock_tmp;      /* Burst lock */
    param.acc           = &acc_tmp;             /* ACC/color killer */
    param.tint_ry       = NULL;                 /* TINT/R-Y axis correction (only valid for NTSC/PAL) */
    /* Video decoder driver chroma decoding */
    error = R_VDEC_ChromaDecoding(channel, &param);
    if (error != VDEC_OK) {
    }
}   /* End of function ChromaDec() */

/**************************************************************************//**
 * @brief       Video decoder digital clamp setup
 * @param       [in]channel         : Channel
 * @retval      None
 *****************************************************************************/
static void DigitalClamp (const vdec_channel_t channel)
{
    vdec_error_t            error;
    vdec_degital_clamp_t    param;
    vdec_pedestal_clamp_t   pedestal_clamp_tmp;
    vdec_center_clamp_t     center_clamp_tmp;
    vdec_noise_det_t        noise_det_tmp;

    /* Pedestal clamp */
    pedestal_clamp_tmp.dcpmode_y    = VDEC_DCPMODE_AUTO;    /* Clamp level setting mode */
    pedestal_clamp_tmp.blanklevel_y = BLANKLEVEL_Y_VALUE;   /* Clamp offset level (Y signal) */
    pedestal_clamp_tmp.dcppos_y     = DCPPOS_Y_VALUE;   /* Digital clamp pulse horizontal start position (Y signal) */
    /* Center clamp */
    center_clamp_tmp.dcpmode_c      = VDEC_DCPMODE_MANUAL;  /* Clamp level setting mode */
    center_clamp_tmp.blanklevel_cb  = BLANKLEVEL_CB_VALUE;  /* Clamp offset level (Cb signal) */
    center_clamp_tmp.blanklevel_cr  = BLANKLEVEL_CR_VALUE;  /* Clamp offset level (Cr signal) */
    center_clamp_tmp.dcppos_c       = DCPPOS_C_VALUE;       /* Digital clamp pulse
                                                               horizontal start position (Cb/Cr signal) */
    /* Noise detection */
    noise_det_tmp.acfinput      = VDEC_ACFINPUT_Y;  /* Video signal for autocorrelation function */
    noise_det_tmp.acflagtime    = ACFLAGTIME_VALUE; /* Delay time for autocorrelation function calculation */
    noise_det_tmp.acffilter     = ACFFILTER_VALUE;  /* Smoothing parameter of autocorrelation function data */

    param.dcpcheck          = VDEC_OFF;             /* Digital clamp pulse position check */
    param.dcpresponse       = DCPRESPONSE_VALUE;    /* Digital clamp response speed */
    param.dcpstart          = DCPSTART_VALUE;       /* Digital clamp start line (in 1-line units) */
    param.dcpend            = DCPEND_VALUE;         /* Digital clamp end line (in 1-line units) */
    param.dcpwidth          = DCPWIDTH_VALUE;       /* Digital clamp pulse width */
    param.pedestal_clamp    = &pedestal_clamp_tmp;  /* Pedestal clamp */
    param.center_clamp      = &center_clamp_tmp;    /* Center clamp */
    param.noise_det         = &noise_det_tmp;       /* Noise detection */
    /* Video decoder driver digital clamp */
    error = R_VDEC_DigitalClamp(channel, &param);
    if (error != VDEC_OK) {
    }
}   /* End of function DigitalClamp() */

/**************************************************************************//**
 * @brief       Video decoder output adjustment setup
 * @param       [in]channel         : Channel
 * @retval      None
 *****************************************************************************/
static void Output (const vdec_channel_t channel)
{
    vdec_error_t        error;
    vdec_output_t       param;

    param.y_gain2   = Y_GAIN2_VALUE;        /* Y signal gain coefficient */
    param.cb_gain2  = CB_GAIN2_VALUE;       /* Cb signal gain coefficient */
    param.cr_gain2  = CR_GAIN2_VALUE;       /* Cr signal gain coefficient */
    /* Video decoder driver output */
    error = R_VDEC_Output(channel, &param);
    if (error != VDEC_OK) {
    }
}   /* End of function Output() */
