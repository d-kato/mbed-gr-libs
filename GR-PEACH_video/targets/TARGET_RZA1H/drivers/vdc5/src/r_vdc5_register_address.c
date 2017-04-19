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
* @file         r_vdc5_register_address.c
* @version      1.00
* $Rev: 199 $
* $Date:: 2014-05-23 16:33:52 +0900#$
* @brief        VDC5 driver register address table
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
#define     VDC5_CH0_GR1_CLUT_TBL           (*(uint32_t*)0xFCFF6400u)
#define     VDC5_CH0_GR2_CLUT_TBL           (*(uint32_t*)0xFCFF6800u)
#define     VDC5_CH0_GR3_CLUT_TBL           (*(uint32_t*)0xFCFF6C00u)
#define     VDC5_CH0_GR_OIR_CLUT_TBL        (*(uint32_t*)0xFCFF7000u)
#define     VDC5_CH1_GR0_CLUT_TBL           (*(uint32_t*)0xFCFF8000u)
#define     VDC5_CH1_GR1_CLUT_TBL           (*(uint32_t*)0xFCFF8400u)
#define     VDC5_CH1_GR2_CLUT_TBL           (*(uint32_t*)0xFCFF8800u)
#define     VDC5_CH1_GR3_CLUT_TBL           (*(uint32_t*)0xFCFF8C00u)
#define     VDC5_CH1_GR_OIR_CLUT_TBL        (*(uint32_t*)0xFCFF9000u)


/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
/* VDC5 input controller register address list */
const vdc5_regaddr_input_ctrl_t vdc5_regaddr_input_ctrl[VDC5_CHANNEL_NUM] = {
    {   /* Channel 0 */
        &VDC50.INP_UPDATE,
        &VDC50.INP_SEL_CNT,
        &VDC50.INP_EXT_SYNC_CNT,
        &VDC50.INP_VSYNC_PH_ADJ,
        &VDC50.INP_DLY_ADJ,
        &VDC50.IMGCNT_UPDATE,
        &VDC50.IMGCNT_NR_CNT0,
        &VDC50.IMGCNT_NR_CNT1
    },
    {   /* Channel 1 */
        &VDC51.INP_UPDATE,
        &VDC51.INP_SEL_CNT,
        &VDC51.INP_EXT_SYNC_CNT,
        &VDC51.INP_VSYNC_PH_ADJ,
        &VDC51.INP_DLY_ADJ,
        &VDC51.IMGCNT_UPDATE,
        &VDC51.IMGCNT_NR_CNT0,
        &VDC51.IMGCNT_NR_CNT1
    }
};

/* VDC5 scaler register address list */
const vdc5_regaddr_scaler_t vdc5_regaddr_scaler[VDC5_CHANNEL_NUM][VDC5_SC_TYPE_NUM] = {
    {   /* Channel 0 */
        {   /* SC0 */
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
        },
        {   /* SC1 */
            &VDC50.SC1_SCL0_UPDATE,
            &VDC50.SC1_SCL0_FRC1,
            &VDC50.SC1_SCL0_FRC2,
            &VDC50.SC1_SCL0_FRC3,
            &VDC50.SC1_SCL0_FRC4,
            &VDC50.SC1_SCL0_FRC5,
            &VDC50.SC1_SCL0_FRC6,
            &VDC50.SC1_SCL0_FRC7,
            &VDC50.SC1_SCL0_FRC9,
            &VDC50.SC1_SCL0_MON0,
            &VDC50.SC1_SCL0_INT,
            &VDC50.SC1_SCL0_DS1,
            &VDC50.SC1_SCL0_DS2,
            &VDC50.SC1_SCL0_DS3,
            &VDC50.SC1_SCL0_DS4,
            &VDC50.SC1_SCL0_DS5,
            &VDC50.SC1_SCL0_DS6,
            &VDC50.SC1_SCL0_DS7,
            &VDC50.SC1_SCL0_US1,
            &VDC50.SC1_SCL0_US2,
            &VDC50.SC1_SCL0_US3,
            &VDC50.SC1_SCL0_US4,
            &VDC50.SC1_SCL0_US5,
            &VDC50.SC1_SCL0_US6,
            &VDC50.SC1_SCL0_US7,
            &VDC50.SC1_SCL0_US8,
            &VDC50.SC1_SCL0_OVR1,
            &VDC50.SC1_SCL1_UPDATE,
            &VDC50.SC1_SCL1_WR1,
            &VDC50.SC1_SCL1_WR2,
            &VDC50.SC1_SCL1_WR3,
            &VDC50.SC1_SCL1_WR4,
            &VDC50.SC1_SCL1_WR5,
            &VDC50.SC1_SCL1_WR6,
            &VDC50.SC1_SCL1_WR7,
            &VDC50.SC1_SCL1_WR8,
            &VDC50.SC1_SCL1_WR9,
            &VDC50.SC1_SCL1_WR10,
            &VDC50.SC1_SCL1_WR11,
            &VDC50.SC1_SCL1_MON1,
            &VDC50.SC1_SCL1_PBUF0,
            &VDC50.SC1_SCL1_PBUF1,
            &VDC50.SC1_SCL1_PBUF2,
            &VDC50.SC1_SCL1_PBUF3,
            &VDC50.SC1_SCL1_PBUF_FLD,
            &VDC50.SC1_SCL1_PBUF_CNT
        },
        {   /* OIR */
            &VDC50.OIR_SCL0_UPDATE,
            &VDC50.OIR_SCL0_FRC1,
            &VDC50.OIR_SCL0_FRC2,
            &VDC50.OIR_SCL0_FRC3,
            &VDC50.OIR_SCL0_FRC4,
            &VDC50.OIR_SCL0_FRC5,
            &VDC50.OIR_SCL0_FRC6,
            &VDC50.OIR_SCL0_FRC7,
            NULL,
            NULL,
            NULL,
            &VDC50.OIR_SCL0_DS1,
            &VDC50.OIR_SCL0_DS2,
            &VDC50.OIR_SCL0_DS3,
            NULL,
            NULL,
            NULL,
            &VDC50.OIR_SCL0_DS7,
            &VDC50.OIR_SCL0_US1,
            &VDC50.OIR_SCL0_US2,
            &VDC50.OIR_SCL0_US3,
            NULL,
            NULL,
            NULL,
            NULL,
            &VDC50.OIR_SCL0_US8,
            &VDC50.OIR_SCL0_OVR1,
            &VDC50.OIR_SCL1_UPDATE,
            &VDC50.OIR_SCL1_WR1,
            &VDC50.OIR_SCL1_WR2,
            &VDC50.OIR_SCL1_WR3,
            &VDC50.OIR_SCL1_WR4,
            &VDC50.OIR_SCL1_WR5,
            &VDC50.OIR_SCL1_WR6,
            &VDC50.OIR_SCL1_WR7,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL
        }
    },
    {   /* Channel 1 */
        {   /* SC0 */
            &VDC51.SC0_SCL0_UPDATE,
            &VDC51.SC0_SCL0_FRC1,
            &VDC51.SC0_SCL0_FRC2,
            &VDC51.SC0_SCL0_FRC3,
            &VDC51.SC0_SCL0_FRC4,
            &VDC51.SC0_SCL0_FRC5,
            &VDC51.SC0_SCL0_FRC6,
            &VDC51.SC0_SCL0_FRC7,
            &VDC51.SC0_SCL0_FRC9,
            &VDC51.SC0_SCL0_MON0,
            &VDC51.SC0_SCL0_INT,
            &VDC51.SC0_SCL0_DS1,
            &VDC51.SC0_SCL0_DS2,
            &VDC51.SC0_SCL0_DS3,
            &VDC51.SC0_SCL0_DS4,
            &VDC51.SC0_SCL0_DS5,
            &VDC51.SC0_SCL0_DS6,
            &VDC51.SC0_SCL0_DS7,
            &VDC51.SC0_SCL0_US1,
            &VDC51.SC0_SCL0_US2,
            &VDC51.SC0_SCL0_US3,
            &VDC51.SC0_SCL0_US4,
            &VDC51.SC0_SCL0_US5,
            &VDC51.SC0_SCL0_US6,
            &VDC51.SC0_SCL0_US7,
            &VDC51.SC0_SCL0_US8,
            &VDC51.SC0_SCL0_OVR1,
            &VDC51.SC0_SCL1_UPDATE,
            &VDC51.SC0_SCL1_WR1,
            &VDC51.SC0_SCL1_WR2,
            &VDC51.SC0_SCL1_WR3,
            &VDC51.SC0_SCL1_WR4,
            &VDC51.SC0_SCL1_WR5,
            &VDC51.SC0_SCL1_WR6,
            &VDC51.SC0_SCL1_WR7,
            &VDC51.SC0_SCL1_WR8,
            &VDC51.SC0_SCL1_WR9,
            &VDC51.SC0_SCL1_WR10,
            &VDC51.SC0_SCL1_WR11,
            &VDC51.SC0_SCL1_MON1,
            &VDC51.SC0_SCL1_PBUF0,
            &VDC51.SC0_SCL1_PBUF1,
            &VDC51.SC0_SCL1_PBUF2,
            &VDC51.SC0_SCL1_PBUF3,
            &VDC51.SC0_SCL1_PBUF_FLD,
            &VDC51.SC0_SCL1_PBUF_CNT
        },
        {   /* SC1 */
            &VDC51.SC1_SCL0_UPDATE,
            &VDC51.SC1_SCL0_FRC1,
            &VDC51.SC1_SCL0_FRC2,
            &VDC51.SC1_SCL0_FRC3,
            &VDC51.SC1_SCL0_FRC4,
            &VDC51.SC1_SCL0_FRC5,
            &VDC51.SC1_SCL0_FRC6,
            &VDC51.SC1_SCL0_FRC7,
            &VDC51.SC1_SCL0_FRC9,
            &VDC51.SC1_SCL0_MON0,
            &VDC51.SC1_SCL0_INT,
            &VDC51.SC1_SCL0_DS1,
            &VDC51.SC1_SCL0_DS2,
            &VDC51.SC1_SCL0_DS3,
            &VDC51.SC1_SCL0_DS4,
            &VDC51.SC1_SCL0_DS5,
            &VDC51.SC1_SCL0_DS6,
            &VDC51.SC1_SCL0_DS7,
            &VDC51.SC1_SCL0_US1,
            &VDC51.SC1_SCL0_US2,
            &VDC51.SC1_SCL0_US3,
            &VDC51.SC1_SCL0_US4,
            &VDC51.SC1_SCL0_US5,
            &VDC51.SC1_SCL0_US6,
            &VDC51.SC1_SCL0_US7,
            &VDC51.SC1_SCL0_US8,
            &VDC51.SC1_SCL0_OVR1,
            &VDC51.SC1_SCL1_UPDATE,
            &VDC51.SC1_SCL1_WR1,
            &VDC51.SC1_SCL1_WR2,
            &VDC51.SC1_SCL1_WR3,
            &VDC51.SC1_SCL1_WR4,
            &VDC51.SC1_SCL1_WR5,
            &VDC51.SC1_SCL1_WR6,
            &VDC51.SC1_SCL1_WR7,
            &VDC51.SC1_SCL1_WR8,
            &VDC51.SC1_SCL1_WR9,
            &VDC51.SC1_SCL1_WR10,
            &VDC51.SC1_SCL1_WR11,
            &VDC51.SC1_SCL1_MON1,
            &VDC51.SC1_SCL1_PBUF0,
            &VDC51.SC1_SCL1_PBUF1,
            &VDC51.SC1_SCL1_PBUF2,
            &VDC51.SC1_SCL1_PBUF3,
            &VDC51.SC1_SCL1_PBUF_FLD,
            &VDC51.SC1_SCL1_PBUF_CNT
        },
        {   /* OIR */
            &VDC51.OIR_SCL0_UPDATE,
            &VDC51.OIR_SCL0_FRC1,
            &VDC51.OIR_SCL0_FRC2,
            &VDC51.OIR_SCL0_FRC3,
            &VDC51.OIR_SCL0_FRC4,
            &VDC51.OIR_SCL0_FRC5,
            &VDC51.OIR_SCL0_FRC6,
            &VDC51.OIR_SCL0_FRC7,
            NULL,
            NULL,
            NULL,
            &VDC51.OIR_SCL0_DS1,
            &VDC51.OIR_SCL0_DS2,
            &VDC51.OIR_SCL0_DS3,
            NULL,
            NULL,
            NULL,
            &VDC51.OIR_SCL0_DS7,
            &VDC51.OIR_SCL0_US1,
            &VDC51.OIR_SCL0_US2,
            &VDC51.OIR_SCL0_US3,
            NULL,
            NULL,
            NULL,
            NULL,
            &VDC51.OIR_SCL0_US8,
            &VDC51.OIR_SCL0_OVR1,
            &VDC51.OIR_SCL1_UPDATE,
            &VDC51.OIR_SCL1_WR1,
            &VDC51.OIR_SCL1_WR2,
            &VDC51.OIR_SCL1_WR3,
            &VDC51.OIR_SCL1_WR4,
            &VDC51.OIR_SCL1_WR5,
            &VDC51.OIR_SCL1_WR6,
            &VDC51.OIR_SCL1_WR7,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL
        }
    }
};

/* VDC5 image quality improver register address list */
const vdc5_regaddr_img_qlty_imp_t vdc5_regaddr_img_qlty_imp[VDC5_CHANNEL_NUM][VDC5_IMG_IMPRV_NUM] = {
    {   /* Channel 0 */
        {   /* SC0 */
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
        },
        {   /* SC1 */
            &VDC50.ADJ1_UPDATE,
            &VDC50.ADJ1_BKSTR_SET,
            &VDC50.ADJ1_ENH_TIM1,
            &VDC50.ADJ1_ENH_TIM2,
            &VDC50.ADJ1_ENH_TIM3,
            &VDC50.ADJ1_ENH_SHP1,
            &VDC50.ADJ1_ENH_SHP2,
            &VDC50.ADJ1_ENH_SHP3,
            &VDC50.ADJ1_ENH_SHP4,
            &VDC50.ADJ1_ENH_SHP5,
            &VDC50.ADJ1_ENH_SHP6,
            &VDC50.ADJ1_ENH_LTI1,
            &VDC50.ADJ1_ENH_LTI2
        }
    },
    {   /* Channel 1 */
        {   /* SC0 */
            &VDC51.ADJ0_UPDATE,
            &VDC51.ADJ0_BKSTR_SET,
            &VDC51.ADJ0_ENH_TIM1,
            &VDC51.ADJ0_ENH_TIM2,
            &VDC51.ADJ0_ENH_TIM3,
            &VDC51.ADJ0_ENH_SHP1,
            &VDC51.ADJ0_ENH_SHP2,
            &VDC51.ADJ0_ENH_SHP3,
            &VDC51.ADJ0_ENH_SHP4,
            &VDC51.ADJ0_ENH_SHP5,
            &VDC51.ADJ0_ENH_SHP6,
            &VDC51.ADJ0_ENH_LTI1,
            &VDC51.ADJ0_ENH_LTI2
        },
        {   /* SC1 */
            &VDC51.ADJ1_UPDATE,
            &VDC51.ADJ1_BKSTR_SET,
            &VDC51.ADJ1_ENH_TIM1,
            &VDC51.ADJ1_ENH_TIM2,
            &VDC51.ADJ1_ENH_TIM3,
            &VDC51.ADJ1_ENH_SHP1,
            &VDC51.ADJ1_ENH_SHP2,
            &VDC51.ADJ1_ENH_SHP3,
            &VDC51.ADJ1_ENH_SHP4,
            &VDC51.ADJ1_ENH_SHP5,
            &VDC51.ADJ1_ENH_SHP6,
            &VDC51.ADJ1_ENH_LTI1,
            &VDC51.ADJ1_ENH_LTI2
        }
    }
};

/* VDC5 color matrix register address list */
const vdc5_regaddr_color_matrix_t vdc5_regaddr_color_matrix[VDC5_CHANNEL_NUM][VDC5_COLORMTX_NUM] = {
    {   /* Channel 0 */
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
        },
        {   /* Image quality improver 1 */
            &VDC50.ADJ1_UPDATE,
            &VDC50.ADJ1_MTX_MODE,
            &VDC50.ADJ1_MTX_YG_ADJ0,
            &VDC50.ADJ1_MTX_YG_ADJ1,
            &VDC50.ADJ1_MTX_CBB_ADJ0,
            &VDC50.ADJ1_MTX_CBB_ADJ1,
            &VDC50.ADJ1_MTX_CRR_ADJ0,
            &VDC50.ADJ1_MTX_CRR_ADJ1
        }
    },
    {   /* Channel 1 */
        {   /* Input Controller */
            &VDC51.IMGCNT_UPDATE,
            &VDC51.IMGCNT_MTX_MODE,
            &VDC51.IMGCNT_MTX_YG_ADJ0,
            &VDC51.IMGCNT_MTX_YG_ADJ1,
            &VDC51.IMGCNT_MTX_CBB_ADJ0,
            &VDC51.IMGCNT_MTX_CBB_ADJ1,
            &VDC51.IMGCNT_MTX_CRR_ADJ0,
            &VDC51.IMGCNT_MTX_CRR_ADJ1
        },
        {   /* Image quality improver 0 */
            &VDC51.ADJ0_UPDATE,
            &VDC51.ADJ0_MTX_MODE,
            &VDC51.ADJ0_MTX_YG_ADJ0,
            &VDC51.ADJ0_MTX_YG_ADJ1,
            &VDC51.ADJ0_MTX_CBB_ADJ0,
            &VDC51.ADJ0_MTX_CBB_ADJ1,
            &VDC51.ADJ0_MTX_CRR_ADJ0,
            &VDC51.ADJ0_MTX_CRR_ADJ1
        },
        {   /* Image quality improver 1 */
            &VDC51.ADJ1_UPDATE,
            &VDC51.ADJ1_MTX_MODE,
            &VDC51.ADJ1_MTX_YG_ADJ0,
            &VDC51.ADJ1_MTX_YG_ADJ1,
            &VDC51.ADJ1_MTX_CBB_ADJ0,
            &VDC51.ADJ1_MTX_CBB_ADJ1,
            &VDC51.ADJ1_MTX_CRR_ADJ0,
            &VDC51.ADJ1_MTX_CRR_ADJ1
        }
    }
};

/* VDC5 image synthesizer register address list */
const vdc5_regaddr_img_synthesizer_t vdc5_regaddr_img_synthesizer[VDC5_CHANNEL_NUM][VDC5_GR_TYPE_NUM] = {
    {   /* Channel 0 */
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
        {   /* GR1 */
            &VDC50.GR1_UPDATE,
            &VDC50.GR1_FLM_RD,
            &VDC50.GR1_FLM1,
            &VDC50.GR1_FLM2,
            &VDC50.GR1_FLM3,
            &VDC50.GR1_FLM4,
            &VDC50.GR1_FLM5,
            &VDC50.GR1_FLM6,
            &VDC50.GR1_AB1,
            &VDC50.GR1_AB2,
            &VDC50.GR1_AB3,
            &VDC50.GR1_AB4,
            &VDC50.GR1_AB5,
            &VDC50.GR1_AB6,
            &VDC50.GR1_AB7,
            &VDC50.GR1_AB8,
            &VDC50.GR1_AB9,
            &VDC50.GR1_AB10,
            &VDC50.GR1_AB11,
            &VDC50.GR1_BASE,
            &VDC50.GR1_CLUT,
            &VDC50.GR1_MON
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
        },
        {   /* VIN */
            &VDC50.GR_VIN_UPDATE,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            &VDC50.GR_VIN_AB1,
            &VDC50.GR_VIN_AB2,
            &VDC50.GR_VIN_AB3,
            &VDC50.GR_VIN_AB4,
            &VDC50.GR_VIN_AB5,
            &VDC50.GR_VIN_AB6,
            &VDC50.GR_VIN_AB7,
            NULL,
            NULL,
            NULL,
            NULL,
            &VDC50.GR_VIN_BASE,
            NULL,
            &VDC50.GR_VIN_MON
        },
        {   /* OIR */
            &VDC50.GR_OIR_UPDATE,
            &VDC50.GR_OIR_FLM_RD,
            &VDC50.GR_OIR_FLM1,
            &VDC50.GR_OIR_FLM2,
            &VDC50.GR_OIR_FLM3,
            &VDC50.GR_OIR_FLM4,
            &VDC50.GR_OIR_FLM5,
            &VDC50.GR_OIR_FLM6,
            &VDC50.GR_OIR_AB1,
            &VDC50.GR_OIR_AB2,
            &VDC50.GR_OIR_AB3,
            NULL,
            NULL,
            NULL,
            &VDC50.GR_OIR_AB7,
            &VDC50.GR_OIR_AB8,
            &VDC50.GR_OIR_AB9,
            &VDC50.GR_OIR_AB10,
            &VDC50.GR_OIR_AB11,
            &VDC50.GR_OIR_BASE,
            &VDC50.GR_OIR_CLUT,
            &VDC50.GR_OIR_MON
        }
    },
    {   /* Channel 1 */
        {   /* GR0 */
            &VDC51.GR0_UPDATE,
            &VDC51.GR0_FLM_RD,
            &VDC51.GR0_FLM1,
            &VDC51.GR0_FLM2,
            &VDC51.GR0_FLM3,
            &VDC51.GR0_FLM4,
            &VDC51.GR0_FLM5,
            &VDC51.GR0_FLM6,
            &VDC51.GR0_AB1,
            &VDC51.GR0_AB2,
            &VDC51.GR0_AB3,
            NULL,
            NULL,
            NULL,
            &VDC51.GR0_AB7,
            &VDC51.GR0_AB8,
            &VDC51.GR0_AB9,
            &VDC51.GR0_AB10,
            &VDC51.GR0_AB11,
            &VDC51.GR0_BASE,
            &VDC51.GR0_CLUT,
            NULL
        },
        {   /* GR1 */
            &VDC51.GR1_UPDATE,
            &VDC51.GR1_FLM_RD,
            &VDC51.GR1_FLM1,
            &VDC51.GR1_FLM2,
            &VDC51.GR1_FLM3,
            &VDC51.GR1_FLM4,
            &VDC51.GR1_FLM5,
            &VDC51.GR1_FLM6,
            &VDC51.GR1_AB1,
            &VDC51.GR1_AB2,
            &VDC51.GR1_AB3,
            &VDC51.GR1_AB4,
            &VDC51.GR1_AB5,
            &VDC51.GR1_AB6,
            &VDC51.GR1_AB7,
            &VDC51.GR1_AB8,
            &VDC51.GR1_AB9,
            &VDC51.GR1_AB10,
            &VDC51.GR1_AB11,
            &VDC51.GR1_BASE,
            &VDC51.GR1_CLUT,
            &VDC51.GR1_MON
        },
        {   /* GR2 */
            &VDC51.GR2_UPDATE,
            &VDC51.GR2_FLM_RD,
            &VDC51.GR2_FLM1,
            &VDC51.GR2_FLM2,
            &VDC51.GR2_FLM3,
            &VDC51.GR2_FLM4,
            &VDC51.GR2_FLM5,
            &VDC51.GR2_FLM6,
            &VDC51.GR2_AB1,
            &VDC51.GR2_AB2,
            &VDC51.GR2_AB3,
            &VDC51.GR2_AB4,
            &VDC51.GR2_AB5,
            &VDC51.GR2_AB6,
            &VDC51.GR2_AB7,
            &VDC51.GR2_AB8,
            &VDC51.GR2_AB9,
            &VDC51.GR2_AB10,
            &VDC51.GR2_AB11,
            &VDC51.GR2_BASE,
            &VDC51.GR2_CLUT,
            &VDC51.GR2_MON
        },
        {   /* GR3 */
            &VDC51.GR3_UPDATE,
            &VDC51.GR3_FLM_RD,
            &VDC51.GR3_FLM1,
            &VDC51.GR3_FLM2,
            &VDC51.GR3_FLM3,
            &VDC51.GR3_FLM4,
            &VDC51.GR3_FLM5,
            &VDC51.GR3_FLM6,
            &VDC51.GR3_AB1,
            &VDC51.GR3_AB2,
            &VDC51.GR3_AB3,
            &VDC51.GR3_AB4,
            &VDC51.GR3_AB5,
            &VDC51.GR3_AB6,
            &VDC51.GR3_AB7,
            &VDC51.GR3_AB8,
            &VDC51.GR3_AB9,
            &VDC51.GR3_AB10,
            &VDC51.GR3_AB11,
            &VDC51.GR3_BASE,
            &VDC51.GR3_CLUT_INT,
            &VDC51.GR3_MON
        },
        {   /* VIN */
            &VDC51.GR_VIN_UPDATE,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            &VDC51.GR_VIN_AB1,
            &VDC51.GR_VIN_AB2,
            &VDC51.GR_VIN_AB3,
            &VDC51.GR_VIN_AB4,
            &VDC51.GR_VIN_AB5,
            &VDC51.GR_VIN_AB6,
            &VDC51.GR_VIN_AB7,
            NULL,
            NULL,
            NULL,
            NULL,
            &VDC51.GR_VIN_BASE,
            NULL,
            &VDC51.GR_VIN_MON
        },
        {   /* OIR */
            &VDC51.GR_OIR_UPDATE,
            &VDC51.GR_OIR_FLM_RD,
            &VDC51.GR_OIR_FLM1,
            &VDC51.GR_OIR_FLM2,
            &VDC51.GR_OIR_FLM3,
            &VDC51.GR_OIR_FLM4,
            &VDC51.GR_OIR_FLM5,
            &VDC51.GR_OIR_FLM6,
            &VDC51.GR_OIR_AB1,
            &VDC51.GR_OIR_AB2,
            &VDC51.GR_OIR_AB3,
            NULL,
            NULL,
            NULL,
            &VDC51.GR_OIR_AB7,
            &VDC51.GR_OIR_AB8,
            &VDC51.GR_OIR_AB9,
            &VDC51.GR_OIR_AB10,
            &VDC51.GR_OIR_AB11,
            &VDC51.GR_OIR_BASE,
            &VDC51.GR_OIR_CLUT,
            &VDC51.GR_OIR_MON
        }
    }
};

/* VDC5 CLUT register address list */
uint32_t * const vdc5_regaddr_clut[VDC5_CHANNEL_NUM][VDC5_GR_TYPE_NUM] = {
    {   /* Channel 0 */
        &VDC5_CH0_GR0_CLUT_TBL,
        &VDC5_CH0_GR1_CLUT_TBL,
        &VDC5_CH0_GR2_CLUT_TBL,
        &VDC5_CH0_GR3_CLUT_TBL,
        NULL,
        &VDC5_CH0_GR_OIR_CLUT_TBL
    },
    {   /* Channel 1 */
        &VDC5_CH1_GR0_CLUT_TBL,
        &VDC5_CH1_GR1_CLUT_TBL,
        &VDC5_CH1_GR2_CLUT_TBL,
        &VDC5_CH1_GR3_CLUT_TBL,
        NULL,
        &VDC5_CH1_GR_OIR_CLUT_TBL
    }
};

/* VDC5 output controller register address list */
const vdc5_regaddr_output_ctrl_t vdc5_regaddr_output_ctrl[VDC5_CHANNEL_NUM] = {
    {   /* Channel 0 */
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
    },
    {   /* Channel 1 */
        &VDC51.TCON_UPDATE,
        &VDC51.TCON_TIM,
        &VDC51.TCON_TIM_STVA1,
        &VDC51.TCON_TIM_STVA2,
        &VDC51.TCON_TIM_STVB1,
        &VDC51.TCON_TIM_STVB2,
        &VDC51.TCON_TIM_STH1,
        &VDC51.TCON_TIM_STH2,
        &VDC51.TCON_TIM_STB1,
        &VDC51.TCON_TIM_STB2,
        &VDC51.TCON_TIM_CPV1,
        &VDC51.TCON_TIM_CPV2,
        &VDC51.TCON_TIM_POLA1,
        &VDC51.TCON_TIM_POLA2,
        &VDC51.TCON_TIM_POLB1,
        &VDC51.TCON_TIM_POLB2,
        &VDC51.TCON_TIM_DE,
        &VDC51.OUT_UPDATE,
        &VDC51.OUT_SET,
        &VDC51.OUT_BRIGHT1,
        &VDC51.OUT_BRIGHT2,
        &VDC51.OUT_CONTRAST,
        &VDC51.OUT_PDTHA,
        &VDC51.OUT_CLK_PHASE
    }
};

/* VDC5 gamma correction register address list */
const vdc5_regaddr_gamma_t vdc5_regaddr_gamma[VDC5_CHANNEL_NUM] = {
    {   /* Channel 0 */
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
    },
    {   /* Channel 1 */
        &VDC51.GAM_SW,
        &VDC51.GAM_G_UPDATE,
        {
            &VDC51.GAM_G_LUT1,
            &VDC51.GAM_G_LUT2,
            &VDC51.GAM_G_LUT3,
            &VDC51.GAM_G_LUT4,
            &VDC51.GAM_G_LUT5,
            &VDC51.GAM_G_LUT6,
            &VDC51.GAM_G_LUT7,
            &VDC51.GAM_G_LUT8,
            &VDC51.GAM_G_LUT9,
            &VDC51.GAM_G_LUT10,
            &VDC51.GAM_G_LUT11,
            &VDC51.GAM_G_LUT12,
            &VDC51.GAM_G_LUT13,
            &VDC51.GAM_G_LUT14,
            &VDC51.GAM_G_LUT15,
            &VDC51.GAM_G_LUT16
        },
        {
            &VDC51.GAM_G_AREA1,
            &VDC51.GAM_G_AREA2,
            &VDC51.GAM_G_AREA3,
            &VDC51.GAM_G_AREA4,
            &VDC51.GAM_G_AREA5,
            &VDC51.GAM_G_AREA6,
            &VDC51.GAM_G_AREA7,
            &VDC51.GAM_G_AREA8
        },
        &VDC51.GAM_B_UPDATE,
        {
            &VDC51.GAM_B_LUT1,
            &VDC51.GAM_B_LUT2,
            &VDC51.GAM_B_LUT3,
            &VDC51.GAM_B_LUT4,
            &VDC51.GAM_B_LUT5,
            &VDC51.GAM_B_LUT6,
            &VDC51.GAM_B_LUT7,
            &VDC51.GAM_B_LUT8,
            &VDC51.GAM_B_LUT9,
            &VDC51.GAM_B_LUT10,
            &VDC51.GAM_B_LUT11,
            &VDC51.GAM_B_LUT12,
            &VDC51.GAM_B_LUT13,
            &VDC51.GAM_B_LUT14,
            &VDC51.GAM_B_LUT15,
            &VDC51.GAM_B_LUT16
        },
        {
            &VDC51.GAM_B_AREA1,
            &VDC51.GAM_B_AREA2,
            &VDC51.GAM_B_AREA3,
            &VDC51.GAM_B_AREA4,
            &VDC51.GAM_B_AREA5,
            &VDC51.GAM_B_AREA6,
            &VDC51.GAM_B_AREA7,
            &VDC51.GAM_B_AREA8
        },
        &VDC51.GAM_R_UPDATE,
        {
            &VDC51.GAM_R_LUT1,
            &VDC51.GAM_R_LUT2,
            &VDC51.GAM_R_LUT3,
            &VDC51.GAM_R_LUT4,
            &VDC51.GAM_R_LUT5,
            &VDC51.GAM_R_LUT6,
            &VDC51.GAM_R_LUT7,
            &VDC51.GAM_R_LUT8,
            &VDC51.GAM_R_LUT9,
            &VDC51.GAM_R_LUT10,
            &VDC51.GAM_R_LUT11,
            &VDC51.GAM_R_LUT12,
            &VDC51.GAM_R_LUT13,
            &VDC51.GAM_R_LUT14,
            &VDC51.GAM_R_LUT15,
            &VDC51.GAM_R_LUT16
        },
        {
            &VDC51.GAM_R_AREA1,
            &VDC51.GAM_R_AREA2,
            &VDC51.GAM_R_AREA3,
            &VDC51.GAM_R_AREA4,
            &VDC51.GAM_R_AREA5,
            &VDC51.GAM_R_AREA6,
            &VDC51.GAM_R_AREA7,
            &VDC51.GAM_R_AREA8
        }
    }
};

/* VDC5 system controller register address list */
const vdc5_regaddr_system_ctrl_t vdc5_regaddr_system_ctrl[VDC5_CHANNEL_NUM] = {
    {   /* Channel 0 */
        &VDC50.SYSCNT_INT1,
        &VDC50.SYSCNT_INT2,
        &VDC50.SYSCNT_INT3,
        &VDC50.SYSCNT_INT4,
        &VDC50.SYSCNT_INT5,
        &VDC50.SYSCNT_INT6,
        &VDC50.SYSCNT_PANEL_CLK,
        &VDC50.SYSCNT_CLUT
    },
    {   /* Channel 1 */
        &VDC51.SYSCNT_INT1,
        &VDC51.SYSCNT_INT2,
        &VDC51.SYSCNT_INT3,
        &VDC51.SYSCNT_INT4,
        &VDC51.SYSCNT_INT5,
        &VDC51.SYSCNT_INT6,
        &VDC51.SYSCNT_PANEL_CLK,
        &VDC51.SYSCNT_CLUT
    }
};

/* LVDS register address list */
const vdc5_regaddr_lvds_t vdc5_regaddr_lvds = {
    &LVDS.LVDS_UPDATE,
    &LVDS.LVDSFCL,
    &LVDS.LCLKSELR,
    &LVDS.LPLLSETR,
    &LVDS.LPHYACC
};

