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
* @file         lcd_analog_rgb_ch1.c
* @version      1.00
* $Rev: 199 $
* $Date:: 2014-05-23 16:33:52 +0900#$
* @brief        LCD panel for vdc5 channel 1 function
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    <string.h>

#include    "r_typedefs.h"

#include    "r_vdc5.h"

#include    "iodefine.h"
#include    "lcd_panel.h"

#if     (LCD_VDC5_CH1_PANEL==1)

/******************************************************************************
Macro definitions
******************************************************************************/
/* Port 4 */
#define LCD_PORT4_2ND (0x5400u)
/* Port 9 */
#define LCD_PORT9_1ST (0x00FCu)
/* Port 5 */
#define LCD_PORT5_7TH (0x0600u)
#define LCD_PORT5_2ND (0x00FFu)
/* Port 2 */
#define LCD_PORT2_7TH (0xF000u)
#define LCD_PORT2_6TH (0x0F00u)

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
 * @brief       LCD panel I/O port setup (VDC5 channel 1)
 * @param[in]   void
 * @retval      None
******************************************************************************/
void GRAPHICS_SetLcdPanel_Ch1 (void)
{
    volatile uint32_t   dummy_read;
    uint32_t            reg_data;

    /*  Analog RGB D-sub15 (RGB888)
            VSYNC       : LCD1_TCON1            ... P4_14, 2nd alternative function
                        : LCD1_CLK              ... P4_12, 2nd alternative function
            HSYNC       : LCD1_TCON5            ... P4_10, 2nd alternative function
        LCD1_DATA
            R[7:0]      : LCD1_DATA[23:18]      ... P9_7 ~ P9_2, 1st alternative function
                        :          [17:16]      ... P5_10 ~ P5_9, 7th alternative function
            G[7:0]      : LCD1_DATA[15:12]      ... P2_15 ~ P2_12, 7th alternative function
                        :          [11:8]       ... P2_11 ~ P2_8, 6th alternative function
            B[7:0]      : LCD1_DATA[7:0]        ... P5_7 ~ P5_0, 2nd alternative function
    */
    /* Port 4 */
    reg_data        = (uint32_t)GPIO.PMC4 & (uint32_t)~LCD_PORT4_2ND;
    GPIO.PMC4       = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PMC4;
    dummy_read      = reg_data;
    /* PFCAE4, PFCE4, PFC4 ... 2nd alternative function
       PIPC4, PMC4
       b14      : P4_14
       b12      : P4_12
       b10      : P4_10 */
    reg_data        = (uint32_t)GPIO.PFCAE4 & (uint32_t)~LCD_PORT4_2ND;
    GPIO.PFCAE4     = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PFCE4 & (uint32_t)~LCD_PORT4_2ND;
    GPIO.PFCE4      = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PFC4 | (uint32_t)LCD_PORT4_2ND;
    GPIO.PFC4       = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PIPC4 | (uint32_t)LCD_PORT4_2ND;
    GPIO.PIPC4      = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PMC4 | (uint32_t)LCD_PORT4_2ND;
    GPIO.PMC4       = (uint16_t)reg_data;

    /* Port 9 */
    reg_data        = (uint32_t)GPIO.PMC9 & (uint32_t)~LCD_PORT9_1ST;
    GPIO.PMC9       = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PMC9;
    dummy_read      = reg_data;
    /* PFCAE9, PFCE9, PFC9 ... 1st alternative function
       PIPC9, PMC9
       b7:b2    : P9_7 ~ P9_2 */
    reg_data        = (uint32_t)GPIO.PFCAE9 & (uint32_t)~LCD_PORT9_1ST;
    GPIO.PFCAE9     = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PFCE9 & (uint32_t)~LCD_PORT9_1ST;
    GPIO.PFCE9      = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PFC9 & (uint32_t)~LCD_PORT9_1ST;
    GPIO.PFC9       = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PIPC9 | (uint32_t)LCD_PORT9_1ST;
    GPIO.PIPC9      = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PMC9 | (uint32_t)LCD_PORT9_1ST;
    GPIO.PMC9       = (uint16_t)reg_data;

    /* Port 5 */
    reg_data        = (uint32_t)GPIO.PMC5 & (uint32_t)~(LCD_PORT5_7TH|LCD_PORT5_2ND);
    GPIO.PMC5       = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PMC5;
    dummy_read      = reg_data;
    /* PFCAE5, PFCE5, PFC5 ... 7th alternative function
       b10:b9   : P5_10 ~ P5_9 */
    reg_data        = (uint32_t)GPIO.PFCAE5 | (uint32_t)LCD_PORT5_7TH;
    GPIO.PFCAE5     = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PFCE5 | (uint32_t)LCD_PORT5_7TH;
    GPIO.PFCE5      = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PFC5 & (uint32_t)~LCD_PORT5_7TH;
    GPIO.PFC5       = (uint16_t)reg_data;
    /* PFCAE5, PFCE5, PFC5 ... 2nd alternative function
       b7:b0    : P5_7 ~ P5_0 */
    reg_data        = (uint32_t)GPIO.PFCAE5 & (uint32_t)~LCD_PORT5_2ND;
    GPIO.PFCAE5     = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PFCE5 & (uint32_t)~LCD_PORT5_2ND;
    GPIO.PFCE5      = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PFC5 | (uint32_t)LCD_PORT5_2ND;
    GPIO.PFC5       = (uint16_t)reg_data;
    /* PIPC5, PMC5
       b10:b9   : P5_10 ~ P5_9
       b7:b0    : P5_7 ~ P5_0 */
    reg_data        = (uint32_t)GPIO.PIPC5 | (uint32_t)(LCD_PORT5_7TH|LCD_PORT5_2ND);
    GPIO.PIPC5      = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PMC5 | (uint32_t)(LCD_PORT5_7TH|LCD_PORT5_2ND);
    GPIO.PMC5       = (uint16_t)reg_data;

    /* Port 2 */
    reg_data        = (uint32_t)GPIO.PMC2 & (uint32_t)~(LCD_PORT2_7TH|LCD_PORT2_6TH);
    GPIO.PMC2       = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PMC2;
    dummy_read      = reg_data;
#if(1) /* mbed */
    (void)dummy_read; /* unused */
#endif
    /* PFCAE2, PFCE2, PFC2 ... 7th alternative function
       b15:b12  : P2_15 ~ P2_12 */
    reg_data        = (uint32_t)GPIO.PFCAE2 | (uint32_t)LCD_PORT2_7TH;
    GPIO.PFCAE2     = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PFCE2 | (uint32_t)LCD_PORT2_7TH;
    GPIO.PFCE2      = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PFC2 & (uint32_t)~LCD_PORT2_7TH;
    GPIO.PFC2       = (uint16_t)reg_data;
    /* PFCAE2, PFCE2, PFC2 ... 6th alternative function
       b11:b8   : P2_11 ~ P2_8 */
    reg_data        = (uint32_t)GPIO.PFCAE2 | (uint32_t)LCD_PORT2_6TH;
    GPIO.PFCAE2     = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PFCE2 & (uint32_t)~LCD_PORT2_6TH;
    GPIO.PFCE2      = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PFC2 | (uint32_t)LCD_PORT2_6TH;
    GPIO.PFC2       = (uint16_t)reg_data;
    /* PIPC2, PMC2
       b15:b8   : P2_15 ~ P2_8 */
    reg_data        = (uint32_t)GPIO.PIPC2 | (uint32_t)(LCD_PORT2_7TH|LCD_PORT2_6TH);
    GPIO.PIPC2      = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PMC2 | (uint32_t)(LCD_PORT2_7TH|LCD_PORT2_6TH);
    GPIO.PMC2       = (uint16_t)reg_data;
}   /* End of function GRAPHICS_SetLcdPanel_Ch1() */

/**************************************************************************//**
 * @brief       LCD TCON setup parameter acquisition processing (VDC5 channel 1)
 * @param[out]  outctrl     : Address of the area for storing the LCD TCON timing setup data table
 * @retval      None
******************************************************************************/
void GRAPHICS_SetLcdTconSettings_Ch1 (const vdc5_lcd_tcon_timing_t * * const outctrl)
{
    /* Analog RGB D-sub15 (RGB888), SVGA signal 800x600 */
    /* TCON timing setting, VS */
    static const vdc5_lcd_tcon_timing_t lcd_tcon_timing_VS = {
        (uint16_t)(LCD_CH1_S_VSYNC * FH_1_2_CYCLE),   /* Signal pulse start position */
        (uint16_t)(LCD_CH1_W_VSYNC * FH_1_2_CYCLE),   /* Pulse width */
        VDC5_LCD_TCON_POLMD_NORMAL,
        VDC5_LCD_TCON_REFSEL_HSYNC,
        LCD_CH1_POL_VSYNC,                  /* Polarity inversion control of signal */
        VDC5_LCD_TCON_PIN_1,                /* Output pin for LCD driving signal */
        LCD_CH1_OUT_EDGE                    /* Output phase control of signal */
    };
    /* TCON timing setting, HS */
    static const vdc5_lcd_tcon_timing_t lcd_tcon_timing_HS = {
        (uint16_t)LCD_CH1_S_HSYNC,          /* Signal pulse start position */
        (uint16_t)LCD_CH1_W_HSYNC,          /* Pulse width */
        VDC5_LCD_TCON_POLMD_NORMAL,
        VDC5_LCD_TCON_REFSEL_HSYNC,         /* Signal operating reference select */
        LCD_CH1_POL_HSYNC,                  /* Polarity inversion control of signal */
        VDC5_LCD_TCON_PIN_5,                /* Output pin for LCD driving signal */
        LCD_CH1_OUT_EDGE                    /* Output phase control of signal */
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
}   /* End of function GRAPHICS_SetLcdTconSettings_Ch1() */

#endif  /* LCD_VDC5_CH1_PANEL==LCD_CH1_PANEL_ANALOG_RGB */

