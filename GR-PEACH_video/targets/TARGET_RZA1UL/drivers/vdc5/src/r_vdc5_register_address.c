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
* Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**************************************************************************//**
* @file         r_vdc5_register_address.c
* @version      0.06
* $Rev: 18 $
* $Date:: 2014-05-29 11:11:38 +0900#$
* @brief        RZ/A1L VDC5 driver register address table
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_vdc5.h"
#include    "r_vdc5_user.h"
#include    "r_vdc5_register.h"


/******************************************************************************
Macro definitions
******************************************************************************/
#define     VDC5_CH0_GR0_CLUT_TBL           (*(uint32_t*)0xFCFF6000u)
#define     VDC5_CH0_GR2_CLUT_TBL           (*(uint32_t*)0xFCFF6800u)
#define     VDC5_CH0_GR3_CLUT_TBL           (*(uint32_t*)0xFCFF6C00u)


/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
/* VDC5 input controller register address list */
const vdc5_regaddr_input_ctrl_t vdc5_regaddr_input_ctrl =
{
    &VDC50.INP_UPDATE,
    &VDC50.INP_SEL_CNT,
    &VDC50.INP_EXT_SYNC_CNT,
    &VDC50.INP_VSYNC_PH_ADJ,
    &VDC50.INP_DLY_ADJ,
    &VDC50.IMGCNT_UPDATE,
    &VDC50.IMGCNT_NR_CNT0,
    &VDC50.IMGCNT_NR_CNT1
};

/* VDC5 scaler register address list */
const vdc5_regaddr_scaler_t vdc5_regaddr_scaler =
{
    &VDC50.SC0_SCL0_UPDATE,
    &VDC50.SC0_SCL0_FRC1,
    &VDC50.SC0_SCL0_FRC2,
    &VDC50.SC0_SCL0_FRC3,
    &VDC50.SC0_SCL0_FRC4,
    &VDC50.SC0_SCL0_FRC5,
    &VDC50.SC0_SCL0_FRC6,
    &VDC50.SC0_SCL0_FRC7,
    &VDC50.SC0_SCL0_FRC9,
    &VDC50.SC0_SCL0_MON0,
    &VDC50.SC0_SCL0_INT,
    &VDC50.SC0_SCL0_DS1,
    &VDC50.SC0_SCL0_DS2,
    &VDC50.SC0_SCL0_DS3,
    &VDC50.SC0_SCL0_DS4,
    &VDC50.SC0_SCL0_DS5,
    &VDC50.SC0_SCL0_DS6,
    &VDC50.SC0_SCL0_DS7,
    &VDC50.SC0_SCL0_US1,
    &VDC50.SC0_SCL0_US2,
    &VDC50.SC0_SCL0_US3,
    &VDC50.SC0_SCL0_US4,
    &VDC50.SC0_SCL0_US5,
    &VDC50.SC0_SCL0_US6,
    &VDC50.SC0_SCL0_US7,
    &VDC50.SC0_SCL0_US8,
    &VDC50.SC0_SCL0_OVR1,
    &VDC50.SC0_SCL1_UPDATE,
    &VDC50.SC0_SCL1_WR1,
    &VDC50.SC0_SCL1_WR2,
    &VDC50.SC0_SCL1_WR3,
    &VDC50.SC0_SCL1_WR4,
    &VDC50.SC0_SCL1_WR5,
    &VDC50.SC0_SCL1_WR6,
    &VDC50.SC0_SCL1_WR7,
    &VDC50.SC0_SCL1_WR8,
    &VDC50.SC0_SCL1_WR9,
    &VDC50.SC0_SCL1_WR10,
    &VDC50.SC0_SCL1_WR11,
    &VDC50.SC0_SCL1_MON1,
    &VDC50.SC0_SCL1_PBUF0,
    &VDC50.SC0_SCL1_PBUF1,
    &VDC50.SC0_SCL1_PBUF2,
    &VDC50.SC0_SCL1_PBUF3,
    &VDC50.SC0_SCL1_PBUF_FLD,
    &VDC50.SC0_SCL1_PBUF_CNT
};

/* VDC5 image quality improver register address list */
const vdc5_regaddr_img_qlty_imp_t vdc5_regaddr_img_qlty_imp =
{
    &VDC50.ADJ0_UPDATE,
    &VDC50.ADJ0_BKSTR_SET,
    &VDC50.ADJ0_ENH_TIM1,
    &VDC50.ADJ0_ENH_TIM2,
    &VDC50.ADJ0_ENH_TIM3,
    &VDC50.ADJ0_ENH_SHP1,
    &VDC50.ADJ0_ENH_SHP2,
    &VDC50.ADJ0_ENH_SHP3,
    &VDC50.ADJ0_ENH_SHP4,
    &VDC50.ADJ0_ENH_SHP5,
    &VDC50.ADJ0_ENH_SHP6,
    &VDC50.ADJ0_ENH_LTI1,
    &VDC50.ADJ0_ENH_LTI2
};

/* VDC5 color matrix register address list */
const vdc5_regaddr_color_matrix_t vdc5_regaddr_color_matrix[VDC5_COLORMTX_NUM] =
{
    {   /* Input Controller */
        &VDC50.IMGCNT_UPDATE,
        &VDC50.IMGCNT_MTX_MODE,
        &VDC50.IMGCNT_MTX_YG_ADJ0,
        &VDC50.IMGCNT_MTX_YG_ADJ1,
        &VDC50.IMGCNT_MTX_CBB_ADJ0,
        &VDC50.IMGCNT_MTX_CBB_ADJ1,
        &VDC50.IMGCNT_MTX_CRR_ADJ0,
        &VDC50.IMGCNT_MTX_CRR_ADJ1
    },
    {   /* Image quality improver 0 */
        &VDC50.ADJ0_UPDATE,
        &VDC50.ADJ0_MTX_MODE,
        &VDC50.ADJ0_MTX_YG_ADJ0,
        &VDC50.ADJ0_MTX_YG_ADJ1,
        &VDC50.ADJ0_MTX_CBB_ADJ0,
        &VDC50.ADJ0_MTX_CBB_ADJ1,
        &VDC50.ADJ0_MTX_CRR_ADJ0,
        &VDC50.ADJ0_MTX_CRR_ADJ1
    }
};

/* VDC5 image synthesizer register address list */
const vdc5_regaddr_img_synthesizer_t vdc5_regaddr_img_synthesizer[VDC5_GR_TYPE_NUM] =
{
    {   /* GR0 */
        &VDC50.GR0_UPDATE,
        &VDC50.GR0_FLM_RD,
        &VDC50.GR0_FLM1,
        &VDC50.GR0_FLM2,
        &VDC50.GR0_FLM3,
        &VDC50.GR0_FLM4,
        &VDC50.GR0_FLM5,
        &VDC50.GR0_FLM6,
        &VDC50.GR0_AB1,
        &VDC50.GR0_AB2,
        &VDC50.GR0_AB3,
        NULL,
        NULL,
        NULL,
        &VDC50.GR0_AB7,
        &VDC50.GR0_AB8,
        &VDC50.GR0_AB9,
        &VDC50.GR0_AB10,
        &VDC50.GR0_AB11,
        &VDC50.GR0_BASE,
        &VDC50.GR0_CLUT,
        NULL
    },
    {   /* GR2 */
        &VDC50.GR2_UPDATE,
        &VDC50.GR2_FLM_RD,
        &VDC50.GR2_FLM1,
        &VDC50.GR2_FLM2,
        &VDC50.GR2_FLM3,
        &VDC50.GR2_FLM4,
        &VDC50.GR2_FLM5,
        &VDC50.GR2_FLM6,
        &VDC50.GR2_AB1,
        &VDC50.GR2_AB2,
        &VDC50.GR2_AB3,
        &VDC50.GR2_AB4,
        &VDC50.GR2_AB5,
        &VDC50.GR2_AB6,
        &VDC50.GR2_AB7,
        &VDC50.GR2_AB8,
        &VDC50.GR2_AB9,
        &VDC50.GR2_AB10,
        &VDC50.GR2_AB11,
        &VDC50.GR2_BASE,
        &VDC50.GR2_CLUT,
        &VDC50.GR2_MON
    },
    {   /* GR3 */
        &VDC50.GR3_UPDATE,
        &VDC50.GR3_FLM_RD,
        &VDC50.GR3_FLM1,
        &VDC50.GR3_FLM2,
        &VDC50.GR3_FLM3,
        &VDC50.GR3_FLM4,
        &VDC50.GR3_FLM5,
        &VDC50.GR3_FLM6,
        &VDC50.GR3_AB1,
        &VDC50.GR3_AB2,
        &VDC50.GR3_AB3,
        &VDC50.GR3_AB4,
        &VDC50.GR3_AB5,
        &VDC50.GR3_AB6,
        &VDC50.GR3_AB7,
        &VDC50.GR3_AB8,
        &VDC50.GR3_AB9,
        &VDC50.GR3_AB10,
        &VDC50.GR3_AB11,
        &VDC50.GR3_BASE,
        &VDC50.GR3_CLUT_INT,
        &VDC50.GR3_MON
    }
};

/* VDC5 CLUT register address list */
uint32_t * const vdc5_regaddr_clut[VDC5_GR_TYPE_NUM] =
{
    &VDC5_CH0_GR0_CLUT_TBL,
    &VDC5_CH0_GR2_CLUT_TBL,
    &VDC5_CH0_GR3_CLUT_TBL
};

/* VDC5 output controller register address list */
const vdc5_regaddr_output_ctrl_t vdc5_regaddr_output_ctrl =
{
    &VDC50.TCON_UPDATE,
    &VDC50.TCON_TIM,
    &VDC50.TCON_TIM_STVA1,
    &VDC50.TCON_TIM_STVA2,
    &VDC50.TCON_TIM_STVB1,
    &VDC50.TCON_TIM_STVB2,
    &VDC50.TCON_TIM_STH1,
    &VDC50.TCON_TIM_STH2,
    &VDC50.TCON_TIM_STB1,
    &VDC50.TCON_TIM_STB2,
    &VDC50.TCON_TIM_CPV1,
    &VDC50.TCON_TIM_CPV2,
    &VDC50.TCON_TIM_POLA1,
    &VDC50.TCON_TIM_POLA2,
    &VDC50.TCON_TIM_POLB1,
    &VDC50.TCON_TIM_POLB2,
    &VDC50.TCON_TIM_DE,
    &VDC50.OUT_UPDATE,
    &VDC50.OUT_SET,
    &VDC50.OUT_BRIGHT1,
    &VDC50.OUT_BRIGHT2,
    &VDC50.OUT_CONTRAST,
    &VDC50.OUT_PDTHA,
    &VDC50.OUT_CLK_PHASE
};

/* VDC5 gamma correction register address list */
const vdc5_regaddr_gamma_t vdc5_regaddr_gamma =
{
    &VDC50.GAM_SW,
    &VDC50.GAM_G_UPDATE,
    {
        &VDC50.GAM_G_LUT1,
        &VDC50.GAM_G_LUT2,
        &VDC50.GAM_G_LUT3,
        &VDC50.GAM_G_LUT4,
        &VDC50.GAM_G_LUT5,
        &VDC50.GAM_G_LUT6,
        &VDC50.GAM_G_LUT7,
        &VDC50.GAM_G_LUT8,
        &VDC50.GAM_G_LUT9,
        &VDC50.GAM_G_LUT10,
        &VDC50.GAM_G_LUT11,
        &VDC50.GAM_G_LUT12,
        &VDC50.GAM_G_LUT13,
        &VDC50.GAM_G_LUT14,
        &VDC50.GAM_G_LUT15,
        &VDC50.GAM_G_LUT16
    },
    {
        &VDC50.GAM_G_AREA1,
        &VDC50.GAM_G_AREA2,
        &VDC50.GAM_G_AREA3,
        &VDC50.GAM_G_AREA4,
        &VDC50.GAM_G_AREA5,
        &VDC50.GAM_G_AREA6,
        &VDC50.GAM_G_AREA7,
        &VDC50.GAM_G_AREA8
    },
    &VDC50.GAM_B_UPDATE,
    {
        &VDC50.GAM_B_LUT1,
        &VDC50.GAM_B_LUT2,
        &VDC50.GAM_B_LUT3,
        &VDC50.GAM_B_LUT4,
        &VDC50.GAM_B_LUT5,
        &VDC50.GAM_B_LUT6,
        &VDC50.GAM_B_LUT7,
        &VDC50.GAM_B_LUT8,
        &VDC50.GAM_B_LUT9,
        &VDC50.GAM_B_LUT10,
        &VDC50.GAM_B_LUT11,
        &VDC50.GAM_B_LUT12,
        &VDC50.GAM_B_LUT13,
        &VDC50.GAM_B_LUT14,
        &VDC50.GAM_B_LUT15,
        &VDC50.GAM_B_LUT16
    },
    {
        &VDC50.GAM_B_AREA1,
        &VDC50.GAM_B_AREA2,
        &VDC50.GAM_B_AREA3,
        &VDC50.GAM_B_AREA4,
        &VDC50.GAM_B_AREA5,
        &VDC50.GAM_B_AREA6,
        &VDC50.GAM_B_AREA7,
        &VDC50.GAM_B_AREA8
    },
    &VDC50.GAM_R_UPDATE,
    {
        &VDC50.GAM_R_LUT1,
        &VDC50.GAM_R_LUT2,
        &VDC50.GAM_R_LUT3,
        &VDC50.GAM_R_LUT4,
        &VDC50.GAM_R_LUT5,
        &VDC50.GAM_R_LUT6,
        &VDC50.GAM_R_LUT7,
        &VDC50.GAM_R_LUT8,
        &VDC50.GAM_R_LUT9,
        &VDC50.GAM_R_LUT10,
        &VDC50.GAM_R_LUT11,
        &VDC50.GAM_R_LUT12,
        &VDC50.GAM_R_LUT13,
        &VDC50.GAM_R_LUT14,
        &VDC50.GAM_R_LUT15,
        &VDC50.GAM_R_LUT16
    },
    {
        &VDC50.GAM_R_AREA1,
        &VDC50.GAM_R_AREA2,
        &VDC50.GAM_R_AREA3,
        &VDC50.GAM_R_AREA4,
        &VDC50.GAM_R_AREA5,
        &VDC50.GAM_R_AREA6,
        &VDC50.GAM_R_AREA7,
        &VDC50.GAM_R_AREA8
    }
};

/* VDC5 system controller register address list */
const vdc5_regaddr_system_ctrl_t vdc5_regaddr_system_ctrl =
{
    &VDC50.SYSCNT_INT1,
    &VDC50.SYSCNT_INT2,
    &VDC50.SYSCNT_INT4,
    &VDC50.SYSCNT_INT5,
    &VDC50.SYSCNT_PANEL_CLK,
    &VDC50.SYSCNT_CLUT
};

