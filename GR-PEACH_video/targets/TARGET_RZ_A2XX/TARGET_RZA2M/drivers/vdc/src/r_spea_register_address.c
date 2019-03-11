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
* @file         r_apea_register_address.c
* @version      0.01
* $Rev: 18 $
* $Date:: 2014-05-29 11:11:38 +0900#$
* @brief        RZ/A2M SPEA driver register address table
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_vdc.h"
#include    "r_vdc_user.h"
#include    "r_vdc_register.h"
#include    "r_spea_register.h"


/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
/*! SPEA Unit update controller register address list */
const spea_regaddr_unit_enable_ctrl_t spea_regaddr_unit_enable_ctrl[SPEA_UNIT_NUMBER_INDEX_MAX] =
{
    {   /* index 0 */
        (volatile uint32_t *)&SPRITE.SPEA0S0EN.LONG,  /* SPEAnSkEN: Sprite Unit k enable register  */
        (volatile uint32_t *)&SPRITE.SPEA0S0DS.LONG,  /* SPEAnSkDS: Sprite Unit k disable register */
    },

    {   /* index 1 */
        (volatile uint32_t *)&SPRITE.SPEA0S1EN.LONG,  /* SPEAnSkEN: Sprite Unit k enable register  */
        (volatile uint32_t *)&SPRITE.SPEA0S1DS.LONG,  /* SPEAnSkDS: Sprite Unit k disable register */
    },
};

/*! SPEA Unit update controller register address list */
const spea_regaddr_unit_update_ctrl_t spea_regaddr_unit_update_ctrl[SPEA_UNIT_NUMBER_INDEX_MAX] =
{
    {   /* index 0 */
        (volatile uint32_t *)&SPRITE.SPEA0S0UP.LONG,  /* SPEAnSkUP: Sprite Unit k update request register */
    },
    {   /* index 1 */
        (volatile uint32_t *)&SPRITE.SPEA0S1UP.LONG,  /* SPEAnSkUP: Sprite Unit k update request register */
    },
};

/*! SPEA Unit controller register address list */
const spea_regaddr_unit_ctrl_t spea_regaddr_unit_ctr[SPEA_UNIT_NUMBER_INDEX_MAX][SPEA_INDEX_MAX] =
{
    { /* unit 0 */
        {   /* index 0 */
            (volatile uint32_t *)&SPRITE.SPEA0S0DA0.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S0LY0.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S0PS0.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 1 */
            (volatile uint32_t *)&SPRITE.SPEA0S0DA1.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S0LY1.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S0PS1.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 2 */
            (volatile uint32_t *)&SPRITE.SPEA0S0DA2.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S0LY2.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S0PS2.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 3 */
            (volatile uint32_t *)&SPRITE.SPEA0S0DA3.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S0LY3.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S0PS3.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 4 */
            (volatile uint32_t *)&SPRITE.SPEA0S0DA4.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S0LY4.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S0PS4.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 5 */
            (volatile uint32_t *)&SPRITE.SPEA0S0DA5.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S0LY5.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S0PS5.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 6 */
            (volatile uint32_t *)&SPRITE.SPEA0S0DA6.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S0LY6.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S0PS6.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 7 */
            (volatile uint32_t *)&SPRITE.SPEA0S0DA7.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S0LY7.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S0PS7.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 8 */
            (volatile uint32_t *)&SPRITE.SPEA0S0DA8.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S0LY8.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S0PS8.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 9 */
            (volatile uint32_t *)&SPRITE.SPEA0S0DA9.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S0LY9.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S0PS9.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 10 */
            (volatile uint32_t *)&SPRITE.SPEA0S0DA10.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S0LY10.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S0PS10.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 11 */
            (volatile uint32_t *)&SPRITE.SPEA0S0DA11.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S0LY11.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S0PS11.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 12 */
            (volatile uint32_t *)&SPRITE.SPEA0S0DA12.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S0LY12.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S0PS12.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 13 */
            (volatile uint32_t *)&SPRITE.SPEA0S0DA13.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S0LY13.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S0PS13.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 14 */
            (volatile uint32_t *)&SPRITE.SPEA0S0DA14.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S0LY14.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S0PS14.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 15 */
            (volatile uint32_t *)&SPRITE.SPEA0S0DA15.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S0LY15.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S0PS15.LONG   /* SPEAnSkPSm: position    */
        },
    },

    {   /* uint 1 */
        {   /* index 0 */
            (volatile uint32_t *)&SPRITE.SPEA0S1DA0.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S1LY0.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S1PS0.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 1 */
            (volatile uint32_t *)&SPRITE.SPEA0S1DA1.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S1LY1.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S1PS1.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 2 */
            (volatile uint32_t *)&SPRITE.SPEA0S1DA2.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S1LY2.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S1PS2.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 3 */
            (volatile uint32_t *)&SPRITE.SPEA0S1DA3.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S1LY3.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S1PS3.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 4 */
            (volatile uint32_t *)&SPRITE.SPEA0S1DA4.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S1LY4.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S1PS4.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 5 */
            (volatile uint32_t *)&SPRITE.SPEA0S1DA5.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S1LY5.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S1PS5.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 6 */
            (volatile uint32_t *)&SPRITE.SPEA0S1DA6.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S1LY6.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S1PS6.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 7 */
            (volatile uint32_t *)&SPRITE.SPEA0S1DA7.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S1LY7.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S1PS7.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 8 */
            (volatile uint32_t *)&SPRITE.SPEA0S1DA8.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S1LY8.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S1PS8.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 9 */
            (volatile uint32_t *)&SPRITE.SPEA0S1DA9.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S1LY9.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S1PS9.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 10 */
            (volatile uint32_t *)&SPRITE.SPEA0S1DA10.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S1LY10.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S1PS10.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 11 */
            (volatile uint32_t *)&SPRITE.SPEA0S1DA11.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S1LY11.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S1PS11.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 12 */
            (volatile uint32_t *)&SPRITE.SPEA0S1DA12.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S1LY12.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S1PS12.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 13 */
            (volatile uint32_t *)&SPRITE.SPEA0S1DA13.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S1LY13.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S1PS13.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 14 */
            (volatile uint32_t *)&SPRITE.SPEA0S1DA14.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S1LY14.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S1PS14.LONG   /* SPEAnSkPSm: position    */
        },
        {   /* index 15 */
            (volatile uint32_t *)&SPRITE.SPEA0S1DA15.LONG,  /* SPEAnSkDAm: dst_address */
            (volatile uint32_t *)&SPRITE.SPEA0S1LY15.LONG,  /* SPEAnSkLYm: width_hight */
            (volatile uint32_t *)&SPRITE.SPEA0S1PS15.LONG   /* SPEAnSkPSm: position    */
        }
    }
};

/*! VDC image synthesizer register address list for SPEA */
const spea_regaddr_img_synthesizer_t  spea_regaddr_img_synthesizer[SPEA_UNIT_NUMBER_INDEX_MAX] =
{
    {
        (volatile uint32_t *)&VDC6.GR2_UPDATE.LONG,
        (volatile uint32_t *)&VDC6.GR2_FLM2.LONG,
    },
    {
        (volatile uint32_t *)&VDC6.GR3_UPDATE.LONG,
        (volatile uint32_t *)&VDC6.GR3_FLM2.LONG,
    }
};

/*! RLE register address list for SPEA */
const rle_regaddr_t rle_regaddr =
{
    (volatile uint32_t *)&SPRITE.SPEA0RLSL.LONG,  /* SPEAnRLSL : enable           */
    (volatile uint32_t *)&SPRITE.SPEA0STA0.LONG,  /* SPEAnSTAi : start_address    */
    (volatile uint32_t *)&SPRITE.SPEA0PHA0.LONG,  /* SPEAnPHAi : physical_address */
    (volatile uint32_t *)&SPRITE.SPEA0RCM0.LONG,  /* SPEAnRCMi : color            */
    (volatile uint32_t *)&SPRITE.SPEA0RUP.LONG,   /* SPEAnRUP  : update           */
    (volatile uint32_t *)&SPRITE.SPEA0RCFG.LONG,  /* SPEAnRCFG : rlen_rdth        */
};

/*! VDC image synthesizer register address list for RLE */
const rle_regaddr_img_synthesizer_t rle_regaddr_img_synthesizer =
{
    (volatile uint32_t *)&VDC6.GR0_UPDATE.LONG,
    (volatile uint32_t *)&VDC6.GR0_FLM1.LONG,
    (volatile uint32_t *)&VDC6.GR0_FLM2.LONG,
    (volatile uint32_t *)&VDC6.SC0_SCL1_UPDATE.LONG,
	(volatile uint32_t *)&VDC6.SC0_SCL1_WR2.LONG,
};
