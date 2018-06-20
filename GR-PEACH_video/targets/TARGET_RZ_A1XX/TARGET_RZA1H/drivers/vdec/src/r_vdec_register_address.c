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
* @file         r_vdec_register_address.c
* @version      1.00
* $Rev: 199 $
* $Date:: 2014-05-23 16:33:52 +0900#$
* @brief        VDEC driver register address table
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_vdec.h"
#include    "r_vdec_user.h"
#include    "r_vdec_register.h"


/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
/* VDEC register address list */
const vdec_reg_address_t vdec_reg_address[VDEC_CHANNEL_NUM] = {
    {   /* Channel 0 */
        &DVDEC0.ADCCR1,
        &DVDEC0.TGCR1,
        &DVDEC0.TGCR2,
        &DVDEC0.TGCR3,
        &DVDEC0.SYNSCR1,
        &DVDEC0.SYNSCR2,
        &DVDEC0.SYNSCR3,
        &DVDEC0.SYNSCR4,
        &DVDEC0.SYNSCR5,
        &DVDEC0.HAFCCR1,
        &DVDEC0.HAFCCR2,
        &DVDEC0.HAFCCR3,
        &DVDEC0.VCDWCR1,
        &DVDEC0.DCPCR1,
        &DVDEC0.DCPCR2,
        &DVDEC0.DCPCR3,
        &DVDEC0.DCPCR4,
        &DVDEC0.DCPCR5,
        &DVDEC0.DCPCR6,
        &DVDEC0.DCPCR7,
        &DVDEC0.DCPCR8,
        &DVDEC0.NSDCR,
        &DVDEC0.BTLCR,
        &DVDEC0.BTGPCR,
        &DVDEC0.ACCCR1,
        &DVDEC0.ACCCR2,
        &DVDEC0.ACCCR3,
        &DVDEC0.TINTCR,
        &DVDEC0.YCDCR,
        &DVDEC0.AGCCR1,
        &DVDEC0.AGCCR2,
        &DVDEC0.PKLIMITCR,
        &DVDEC0.RGORCR1,
        &DVDEC0.RGORCR2,
        &DVDEC0.RGORCR3,
        &DVDEC0.RGORCR4,
        &DVDEC0.RGORCR5,
        &DVDEC0.RGORCR6,
        &DVDEC0.RGORCR7,
        &DVDEC0.AFCPFCR,
        &DVDEC0.RUPDCR,
        &DVDEC0.VSYNCSR,
        &DVDEC0.HSYNCSR,
        &DVDEC0.DCPSR1,
        &DVDEC0.DCPSR2,
        &DVDEC0.NSDSR,
        &DVDEC0.CROMASR1,
        &DVDEC0.CROMASR2,
        &DVDEC0.SYNCSSR,
        &DVDEC0.AGCCSR1,
        &DVDEC0.AGCCSR2,
        &DVDEC0.YCSCR3,
        &DVDEC0.YCSCR4,
        &DVDEC0.YCSCR5,
        &DVDEC0.YCSCR6,
        &DVDEC0.YCSCR7,
        &DVDEC0.YCSCR8,
        &DVDEC0.YCSCR9,
        &DVDEC0.YCSCR11,
        &DVDEC0.YCSCR12,
        &DVDEC0.DCPCR9,
        &DVDEC0.YGAINCR,
        &DVDEC0.CBGAINCR,
        &DVDEC0.CRGAINCR,
        &DVDEC0.PGA_UPDATE,
        &DVDEC0.PGACR,
        &DVDEC0.ADCCR2
    },
    {   /* Channel 1 */
        &DVDEC1.ADCCR1,
        &DVDEC1.TGCR1,
        &DVDEC1.TGCR2,
        &DVDEC1.TGCR3,
        &DVDEC1.SYNSCR1,
        &DVDEC1.SYNSCR2,
        &DVDEC1.SYNSCR3,
        &DVDEC1.SYNSCR4,
        &DVDEC1.SYNSCR5,
        &DVDEC1.HAFCCR1,
        &DVDEC1.HAFCCR2,
        &DVDEC1.HAFCCR3,
        &DVDEC1.VCDWCR1,
        &DVDEC1.DCPCR1,
        &DVDEC1.DCPCR2,
        &DVDEC1.DCPCR3,
        &DVDEC1.DCPCR4,
        &DVDEC1.DCPCR5,
        &DVDEC1.DCPCR6,
        &DVDEC1.DCPCR7,
        &DVDEC1.DCPCR8,
        &DVDEC1.NSDCR,
        &DVDEC1.BTLCR,
        &DVDEC1.BTGPCR,
        &DVDEC1.ACCCR1,
        &DVDEC1.ACCCR2,
        &DVDEC1.ACCCR3,
        &DVDEC1.TINTCR,
        &DVDEC1.YCDCR,
        &DVDEC1.AGCCR1,
        &DVDEC1.AGCCR2,
        &DVDEC1.PKLIMITCR,
        &DVDEC1.RGORCR1,
        &DVDEC1.RGORCR2,
        &DVDEC1.RGORCR3,
        &DVDEC1.RGORCR4,
        &DVDEC1.RGORCR5,
        &DVDEC1.RGORCR6,
        &DVDEC1.RGORCR7,
        &DVDEC1.AFCPFCR,
        &DVDEC1.RUPDCR,
        &DVDEC1.VSYNCSR,
        &DVDEC1.HSYNCSR,
        &DVDEC1.DCPSR1,
        &DVDEC1.DCPSR2,
        &DVDEC1.NSDSR,
        &DVDEC1.CROMASR1,
        &DVDEC1.CROMASR2,
        &DVDEC1.SYNCSSR,
        &DVDEC1.AGCCSR1,
        &DVDEC1.AGCCSR2,
        &DVDEC1.YCSCR3,
        &DVDEC1.YCSCR4,
        &DVDEC1.YCSCR5,
        &DVDEC1.YCSCR6,
        &DVDEC1.YCSCR7,
        &DVDEC1.YCSCR8,
        &DVDEC1.YCSCR9,
        &DVDEC1.YCSCR11,
        &DVDEC1.YCSCR12,
        &DVDEC1.DCPCR9,
        &DVDEC1.YGAINCR,
        &DVDEC1.CBGAINCR,
        &DVDEC1.CRGAINCR,
        &DVDEC1.PGA_UPDATE,
        &DVDEC1.PGACR,
        &DVDEC1.ADCCR2
    }
};

/* VDEC register address list (for 2D filter tap coefficient) */
const vdec_filter_reg_address_t vdec_filter_reg_address[VDEC_CHANNEL_NUM] = {
    {   /* Channel 0 */
        {
            &DVDEC0.YCTWA_F0,
            &DVDEC0.YCTWA_F1,
            &DVDEC0.YCTWA_F2,
            &DVDEC0.YCTWA_F3,
            &DVDEC0.YCTWA_F4,
            &DVDEC0.YCTWA_F5,
            &DVDEC0.YCTWA_F6,
            &DVDEC0.YCTWA_F7,
            &DVDEC0.YCTWA_F8
        },
        {
            &DVDEC0.YCTWB_F0,
            &DVDEC0.YCTWB_F1,
            &DVDEC0.YCTWB_F2,
            &DVDEC0.YCTWB_F3,
            &DVDEC0.YCTWB_F4,
            &DVDEC0.YCTWB_F5,
            &DVDEC0.YCTWB_F6,
            &DVDEC0.YCTWB_F7,
            &DVDEC0.YCTWB_F8
        },
        {
            &DVDEC0.YCTNA_F0,
            &DVDEC0.YCTNA_F1,
            &DVDEC0.YCTNA_F2,
            &DVDEC0.YCTNA_F3,
            &DVDEC0.YCTNA_F4,
            &DVDEC0.YCTNA_F5,
            &DVDEC0.YCTNA_F6,
            &DVDEC0.YCTNA_F7,
            &DVDEC0.YCTNA_F8
        },
        {
            &DVDEC0.YCTNB_F0,
            &DVDEC0.YCTNB_F1,
            &DVDEC0.YCTNB_F2,
            &DVDEC0.YCTNB_F3,
            &DVDEC0.YCTNB_F4,
            &DVDEC0.YCTNB_F5,
            &DVDEC0.YCTNB_F6,
            &DVDEC0.YCTNB_F7,
            &DVDEC0.YCTNB_F8
        }
    },
    {   /* Channel 1 */
        {
            &DVDEC1.YCTWA_F0,
            &DVDEC1.YCTWA_F1,
            &DVDEC1.YCTWA_F2,
            &DVDEC1.YCTWA_F3,
            &DVDEC1.YCTWA_F4,
            &DVDEC1.YCTWA_F5,
            &DVDEC1.YCTWA_F6,
            &DVDEC1.YCTWA_F7,
            &DVDEC1.YCTWA_F8
        },
        {
            &DVDEC1.YCTWB_F0,
            &DVDEC1.YCTWB_F1,
            &DVDEC1.YCTWB_F2,
            &DVDEC1.YCTWB_F3,
            &DVDEC1.YCTWB_F4,
            &DVDEC1.YCTWB_F5,
            &DVDEC1.YCTWB_F6,
            &DVDEC1.YCTWB_F7,
            &DVDEC1.YCTWB_F8
        },
        {
            &DVDEC1.YCTNA_F0,
            &DVDEC1.YCTNA_F1,
            &DVDEC1.YCTNA_F2,
            &DVDEC1.YCTNA_F3,
            &DVDEC1.YCTNA_F4,
            &DVDEC1.YCTNA_F5,
            &DVDEC1.YCTNA_F6,
            &DVDEC1.YCTNA_F7,
            &DVDEC1.YCTNA_F8
        },
        {
            &DVDEC1.YCTNB_F0,
            &DVDEC1.YCTNB_F1,
            &DVDEC1.YCTNB_F2,
            &DVDEC1.YCTNB_F3,
            &DVDEC1.YCTNB_F4,
            &DVDEC1.YCTNB_F5,
            &DVDEC1.YCTNB_F6,
            &DVDEC1.YCTNB_F7,
            &DVDEC1.YCTNB_F8
        }
    }
};

