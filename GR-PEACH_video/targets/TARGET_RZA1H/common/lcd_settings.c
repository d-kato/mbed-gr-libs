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
* @file         lcd_settings.c
* @version      1.00
* $Rev: 199 $
* $Date:: 2014-05-23 16:33:52 +0900#$
* @brief        VDC5 driver LCD panel settings sample
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    <string.h>

#include    "r_typedefs.h"

#include    "r_vdc5.h"

#include    "iodefine.h"
#include    "lcd_panel.h"


/******************************************************************************
Macro definitions
******************************************************************************/
#define LVDSPLL_TST (16u)     /* LVDSPLL_TST (LVDS PLL internal parameters) */

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

/**************************************************************************//**
 * @brief       LCD panel I/O port setup
 * @param[in]   channel
 * @retval      None
******************************************************************************/
void GRAPHICS_SetLcdPanel (const vdc5_channel_t channel)
{
    if (channel == VDC5_CHANNEL_0)    {
        GRAPHICS_SetLcdPanel_Ch0();
    } else {
        GRAPHICS_SetLcdPanel_Ch1();
    }
}   /* End of function GRAPHICS_SetLcdPanel() */

/**************************************************************************//**
 * @brief       LCD TCON setup parameter acquisition processing
 * @param[in]   channel
 * @param[out]  outctrl         : Address of the area for storing the LCD TCON timing setup data table
 * @retval      None
******************************************************************************/
void GRAPHICS_SetLcdTconSettings (const vdc5_channel_t channel, const vdc5_lcd_tcon_timing_t * * const outctrl)
{
    /* LCD TCON timing setting */
    if (channel == VDC5_CHANNEL_0) {
        GRAPHICS_SetLcdTconSettings_Ch0(outctrl);
    } else {
        GRAPHICS_SetLcdTconSettings_Ch1(outctrl);
    }
}   /* End of function GRAPHICS_SetLcdTconSettings() */

/**************************************************************************//**
 * @brief       LVDS-related parameter acquisition processing
 * @param[in]   channel
 * @retval      Address of the area containing the LVDS-related parameters
******************************************************************************/
vdc5_lvds_t * GRAPHICS_GetLvdsParam (const vdc5_channel_t channel)
{
    static vdc5_lvds_t  lvds_param;
    vdc5_lvds_t       * p_lvds;

    p_lvds = (vdc5_lvds_t *)&lvds_param;

    if (channel == VDC5_CHANNEL_0) {
        p_lvds = NULL;
    } else {
        p_lvds = NULL;
    }
    return p_lvds;
}   /* End of function GRAPHICS_GetLvdsParam() */

