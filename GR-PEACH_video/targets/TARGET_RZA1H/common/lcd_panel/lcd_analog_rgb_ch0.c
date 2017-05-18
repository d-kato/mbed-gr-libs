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
* @file         lcd_analog_rgb_ch0.c
* @version      1.00
* $Rev: 199 $
* $Date:: 2014-05-23 16:33:52 +0900#$
* @brief        LCD panel for vdc5 channel 0 function
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    <string.h>

#include    "r_typedefs.h"

#include    "r_vdc5.h"

#include    "iodefine.h"
#include    "lcd_panel.h"

#if     (LCD_VDC5_CH0_PANEL==1)

/******************************************************************************
Macro definitions
******************************************************************************/
/* Port 11 */
#define LCD_PORT11_5TH (0xE0FFu)
/* Port 10 */
#define LCD_PORT10_5TH (0xFFFFu)

#define FH_1_2_CYCLE   (2u)

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
 * @brief       LCD panel I/O port setup (VDC5 channel 0)
 * @param[in]   void
 * @retval      None
******************************************************************************/
void GRAPHICS_SetLcdPanel_Ch0 (void)
{
    volatile uint32_t   dummy_read;
    uint32_t            reg_data;

    /*  Analog RGB D-sub15 (RGB888)
                        : LCD0_CLK              ... P11_15, 5th alternative function
            HSYNC       : LCD0_TCON0            ... P11_14, 5th alternative function
            VSYNC       : LCD0_TCON1            ... P11_13, 5th alternative function
        LCD0_DATA
            R[7:0]      : LCD0_DATA[23:16]      ... P10_0 ~ P10_7, 5th alternative function
            G[7:0]      : LCD0_DATA[15:8]       ... P10_8 ~ P10_15, 5th alternative function
            B[7:0]      : LCD0_DATA[7:0]        ... P11_0 ~ P11_7, 5th alternative function
    */
    /* Port 11 */
    reg_data        = (uint32_t)GPIO.PMC11 & (uint32_t)~LCD_PORT11_5TH;
    GPIO.PMC11      = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PMC11;
    dummy_read      = reg_data;
    /* PFCAE11, PFCE11, PFC11 ... 5th alternative function
       PIPC11, PMC11
       b15:b13  : P11_15 ~ P11_13
       b7:b0    : P11_7 ~ P11_0 */
    reg_data        = (uint32_t)GPIO.PFCAE11 | (uint32_t)LCD_PORT11_5TH;
    GPIO.PFCAE11    = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PFCE11 & (uint32_t)~LCD_PORT11_5TH;
    GPIO.PFCE11     = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PFC11 & (uint32_t)~LCD_PORT11_5TH;
    GPIO.PFC11      = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PIPC11 | (uint32_t)LCD_PORT11_5TH;
    GPIO.PIPC11     = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PMC11 | (uint32_t)LCD_PORT11_5TH;
    GPIO.PMC11      = (uint16_t)reg_data;

    /* Port 10 */
    reg_data        = (uint32_t)GPIO.PMC10 & (uint32_t)~LCD_PORT10_5TH;
    GPIO.PMC10      = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PMC10;
    dummy_read      = reg_data;
#if(1) /* mbed */
    (void)dummy_read; /* unused */
#endif
    /* PFCAE10, PFCE10, PFC10 ... 5th alternative function
       PIPC10, PMC10
       b15:b0   : P10_15 ~ P10_0 */
    reg_data        = (uint32_t)GPIO.PFCAE10 | (uint32_t)LCD_PORT10_5TH;
    GPIO.PFCAE10    = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PFCE10 & (uint32_t)~LCD_PORT10_5TH;
    GPIO.PFCE10     = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PFC10 & (uint32_t)~LCD_PORT10_5TH;
    GPIO.PFC10      = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PIPC10 | (uint32_t)LCD_PORT10_5TH;
    GPIO.PIPC10     = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PMC10 | (uint32_t)LCD_PORT10_5TH;
    GPIO.PMC10      = (uint16_t)reg_data;
}   /* End of function GRAPHICS_SetLcdPanel_Ch0() */

/**************************************************************************//**
 * @brief       LCD TCON setup parameter acquisition processing (VDC5 channel 0)
 * @param[out]  outctrl     : Address of the area for storing the LCD TCON timing setup data table
 * @retval      None
******************************************************************************/
void GRAPHICS_SetLcdTconSettings_Ch0 (const vdc5_lcd_tcon_timing_t * * const outctrl)
{
    /* Analog RGB D-sub15 (RGB888), SVGA signal 800x600 */
    /* TCON timing setting, VS */
    static const vdc5_lcd_tcon_timing_t lcd_tcon_timing_VS = {
        (uint16_t)(LCD_CH0_S_VSYNC * FH_1_2_CYCLE),   /* Signal pulse start position */
        (uint16_t)(LCD_CH0_W_VSYNC * FH_1_2_CYCLE),   /* Pulse width */
        VDC5_LCD_TCON_POLMD_NORMAL,
        VDC5_LCD_TCON_REFSEL_HSYNC,
        LCD_CH0_POL_VSYNC,                  /* Polarity inversion control of signal */
        VDC5_LCD_TCON_PIN_1,                /* Output pin for LCD driving signal */
        LCD_CH0_OUT_EDGE                    /* Output phase control of signal */
    };
    /* TCON timing setting, HS */
    static const vdc5_lcd_tcon_timing_t lcd_tcon_timing_HS = {
        (uint16_t)LCD_CH0_S_HSYNC,          /* Signal pulse start position */
        (uint16_t)LCD_CH0_W_HSYNC,          /* Pulse width */
        VDC5_LCD_TCON_POLMD_NORMAL,
        VDC5_LCD_TCON_REFSEL_HSYNC,         /* Signal operating reference select */
        LCD_CH0_POL_HSYNC,                  /* Polarity inversion control of signal */
        VDC5_LCD_TCON_PIN_0,                /* Output pin for LCD driving signal */
        LCD_CH0_OUT_EDGE                    /* Output phase control of signal */
    };

    if (outctrl != NULL) {
        outctrl[VDC5_LCD_TCONSIG_STVA_VS]   = &lcd_tcon_timing_VS;  /* STVA/VS: Vsync */
        outctrl[VDC5_LCD_TCONSIG_STVB_VE]   = NULL;                 /* STVB/VE: Not used */
        outctrl[VDC5_LCD_TCONSIG_STH_SP_HS] = &lcd_tcon_timing_HS;  /* STH/SP/HS: Hsync */
        outctrl[VDC5_LCD_TCONSIG_STB_LP_HE] = NULL;                 /* STB/LP/HE: Not used */
        outctrl[VDC5_LCD_TCONSIG_CPV_GCK]   = NULL;                 /* CPV/GCK: Not used */
        outctrl[VDC5_LCD_TCONSIG_POLA]      = NULL;                 /* POLA: Not used */
        outctrl[VDC5_LCD_TCONSIG_POLB]      = NULL;                 /* POLB: Not used */
        outctrl[VDC5_LCD_TCONSIG_DE]        = NULL;                 /* DE: Not used */
    }
}   /* End of function GRAPHICS_SetLcdTconSettings_Ch0() */

#endif  /* LCD_VDC5_CH0_PANEL==LCD_CH0_PANEL_ANALOG_RGB */

