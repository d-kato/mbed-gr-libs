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
* @file         r_vdec_register.h
* @version      1.00
* $Rev: 199 $
* $Date:: 2014-05-23 16:33:52 +0900#$
* @brief        VDEC driver register setup definitions
******************************************************************************/

#ifndef R_VDEC_REGISTER_H
#define R_VDEC_REGISTER_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_vdec.h"
#include    "r_vdec_user.h"


/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/
/*! VDEC register address list */
typedef struct {
    volatile uint16_t * adccr1;
    volatile uint16_t * tgcr1;
    volatile uint16_t * tgcr2;
    volatile uint16_t * tgcr3;
    volatile uint16_t * synscr1;
    volatile uint16_t * synscr2;
    volatile uint16_t * synscr3;
    volatile uint16_t * synscr4;
    volatile uint16_t * synscr5;
    volatile uint16_t * hafccr1;
    volatile uint16_t * hafccr2;
    volatile uint16_t * hafccr3;
    volatile uint16_t * vcdwcr1;
    volatile uint16_t * dcpcr1;
    volatile uint16_t * dcpcr2;
    volatile uint16_t * dcpcr3;
    volatile uint16_t * dcpcr4;
    volatile uint16_t * dcpcr5;
    volatile uint16_t * dcpcr6;
    volatile uint16_t * dcpcr7;
    volatile uint16_t * dcpcr8;
    volatile uint16_t * nsdcr;
    volatile uint16_t * btlcr;
    volatile uint16_t * btgpcr;
    volatile uint16_t * acccr1;
    volatile uint16_t * acccr2;
    volatile uint16_t * acccr3;
    volatile uint16_t * tintcr;
    volatile uint16_t * ycdcr;
    volatile uint16_t * agccr1;
    volatile uint16_t * agccr2;
    volatile uint16_t * pklimitcr;
    volatile uint16_t * rgorcr1;
    volatile uint16_t * rgorcr2;
    volatile uint16_t * rgorcr3;
    volatile uint16_t * rgorcr4;
    volatile uint16_t * rgorcr5;
    volatile uint16_t * rgorcr6;
    volatile uint16_t * rgorcr7;
    volatile uint16_t * afcpfcr;
    volatile uint16_t * rupdcr;
    volatile uint16_t * vsyncsr;
    volatile uint16_t * hsyncsr;
    volatile uint16_t * dcpsr1;
    volatile uint16_t * dcpsr2;
    volatile uint16_t * nsdsr;
    volatile uint16_t * cromasr1;
    volatile uint16_t * cromasr2;
    volatile uint16_t * syncssr;
    volatile uint16_t * agccsr1;
    volatile uint16_t * agccsr2;
    volatile uint16_t * ycscr3;
    volatile uint16_t * ycscr4;
    volatile uint16_t * ycscr5;
    volatile uint16_t * ycscr6;
    volatile uint16_t * ycscr7;
    volatile uint16_t * ycscr8;
    volatile uint16_t * ycscr9;
    volatile uint16_t * ycscr11;
    volatile uint16_t * ycscr12;
    volatile uint16_t * dcpcr9;
    volatile uint16_t * ygaincr;
    volatile uint16_t * cbgaincr;
    volatile uint16_t * crgaincr;
    volatile uint16_t * pga_update;
    volatile uint16_t * pgacr;
    volatile uint16_t * adccr2;
} vdec_reg_address_t;

/*! VDEC register address list (for 2D filter tap coefficient) */
typedef struct {
    volatile uint16_t * yctwa_f[VDEC_CHRFIL_TAPCOEF_NUM];
    volatile uint16_t * yctwb_f[VDEC_CHRFIL_TAPCOEF_NUM];
    volatile uint16_t * yctna_f[VDEC_CHRFIL_TAPCOEF_NUM];
    volatile uint16_t * yctnb_f[VDEC_CHRFIL_TAPCOEF_NUM];
} vdec_filter_reg_address_t;


/******************************************************************************
Variable Externs
******************************************************************************/
extern const vdec_reg_address_t         vdec_reg_address[VDEC_CHANNEL_NUM];
extern const vdec_filter_reg_address_t  vdec_filter_reg_address[VDEC_CHANNEL_NUM];


/******************************************************************************
Functions Prototypes
******************************************************************************/
void VDEC_Initialize(const vdec_channel_t ch, const vdec_adc_vinsel_t vinsel);
void VDEC_ActivePeriod(const vdec_channel_t ch, const vdec_active_period_t * const param);
void VDEC_SyncSeparation(const vdec_channel_t ch, const vdec_sync_separation_t * const param);
void VDEC_YcSeparation(const vdec_channel_t ch, const vdec_yc_separation_t * const param);
void VDEC_ChromaDecoding(const vdec_channel_t ch, const vdec_chroma_decoding_t * const param);
void VDEC_DigitalClamp(const vdec_channel_t ch, const vdec_degital_clamp_t * const param);
void VDEC_Output(const vdec_channel_t ch, const vdec_output_t * const param);
void VDEC_Query(
    const vdec_channel_t            ch,
    vdec_q_sync_sep_t       * const q_sync_sep,
    vdec_q_agc_t            * const q_agc,
    vdec_q_chroma_dec_t     * const q_chroma_dec,
    vdec_q_digital_clamp_t  * const q_digital_clamp);


#endif  /* R_VDEC_REGISTER_H */
