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
* @file         r_vdec_register.c
* @version      1.00
* $Rev: 199 $
* $Date:: 2014-05-23 16:33:52 +0900#$
* @brief        VDEC driver register setup processing
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
/* shift value */
#define     VEDC_REG_SHIFT_15            (15u)
#define     VEDC_REG_SHIFT_14            (14u)
#define     VEDC_REG_SHIFT_13            (13u)
#define     VEDC_REG_SHIFT_12            (12u)
#define     VEDC_REG_SHIFT_11            (11u)
#define     VEDC_REG_SHIFT_10            (10u)
#define     VEDC_REG_SHIFT_9             (9u)
#define     VEDC_REG_SHIFT_8             (8u)
#define     VEDC_REG_SHIFT_6             (6u)
#define     VEDC_REG_SHIFT_5             (5u)
#define     VEDC_REG_SHIFT_4             (4u)
#define     VEDC_REG_SHIFT_2             (2u)
#define     VEDC_REG_SHIFT_1             (1u)

/* bit set pattern */
#define     VDEC_REG_SET_0X8000         (0x8000u)
#define     VDEC_REG_SET_0X4000         (0x4000u)
#define     VDEC_REG_SET_0X2000         (0x2000u)
#define     VDEC_REG_SET_0X1000         (0x1000u)
#define     VDEC_REG_SET_0X0800         (0x0800u)
#define     VDEC_REG_SET_0X0100         (0x0100u)
#define     VDEC_REG_SET_0X0080         (0x0080u)
#define     VDEC_REG_SET_0X0020         (0x0020u)
#define     VDEC_REG_SET_0X0010         (0x0010u)
#define     VDEC_REG_SET_0X0008         (0x0008u)
#define     VDEC_REG_SET_0X0004         (0x0004u)
#define     VDEC_REG_SET_0X0002         (0x0002u)
#define     VDEC_REG_SET_0X0001         (0x0001u)

/* bit mask pattern */
#define     VDEC_REG_BIT_MASK_0X8000    (0x8000u)
#define     VDEC_REG_BIT_MASK_0X4000    (0x4000u)
#define     VDEC_REG_BIT_MASK_0X2000    (0x2000u)
#define     VDEC_REG_BIT_MASK_0X1000    (0x1000u)
#define     VDEC_REG_BIT_MASK_0X0800    (0x0800u)
#define     VDEC_REG_BIT_MASK_0X0400    (0x0400u)
#define     VDEC_REG_BIT_MASK_0X0200    (0x0200u)
#define     VDEC_REG_BIT_MASK_0X0100    (0x0100u)

/* register mask value */
#define     VDEC_REG_MASK_0X03FF        (0x03FFu) /* mask vdec_reg->syncssr  */
#define     VDEC_REG_MASK_0XFF1F        (0xFF1Fu) /* mask vdec_reg->ycscr7   */
#define     VDEC_REG_MASK_0XF3FF        (0xF3FFu) /* mask vdec_reg->hafccr1  */
#define     VDEC_REG_MASK_0X83FF        (0x83FFu) /* mask vdec_reg->dcpcr1   */
#define     VDEC_REG_MASK_0X01FF        (0x01FFu) /* mask vdec_reg->ycscr11  */
#define     VDEC_REG_MASK_0X1C00        (0x1C00u) /* mask vdec_reg->dcpcr9   */
#define     VDEC_REG_MASK_0X1F00        (0x1F00u) /* mask vdec_reg->pgacr    */
#define     VDEC_REG_MASK_0X3F00        (0x3F00u) /* mask vdec_reg->agccr2   */
#define     VDEC_REG_MASK_0X0007        (0x0007u) /* mask vdec_reg->rgorcr7  */
#define     VDEC_REG_MASK_0XF800        (0xF800u) /* mask vdec_reg->ycscr8   */
#define     VDEC_REG_MASK_0XFC00        (0xFC00u) /* mask vdec_reg->synscr1  */
#define     VDEC_REG_MASK_0X00FF        (0x00FFu) /* mask vdec_reg->synscr1  */
#define     VDEC_REG_MASK_0X0003        (0x0003u) /* mask vdec_reg->cromasr1 */

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static void NoiseReductionLPF(
    const vdec_reg_address_t    * const vdec_reg,
    const vdec_noise_rd_lpf_t   * const p_noise_rd_lpf);
static void SyncSlicer(const vdec_reg_address_t * const vdec_reg, const vdec_sync_slicer_t * const p_sync_slicer);
static void HorizontalAFC(
    const vdec_reg_address_t    * const vdec_reg,
    const vdec_horizontal_afc_t * const p_horizontal_afc);
static void VerticalCountdown(
    const vdec_reg_address_t    * const vdec_reg,
    const vdec_vcount_down_t    * const p_vcount_down);
static void AgcPga(const vdec_reg_address_t * const vdec_reg, const vdec_agc_t * const p_agc);
static void PeakLimiterControl(
    const vdec_reg_address_t    * const vdec_reg,
    const vdec_peak_limiter_t   * const p_peak_limiter);
static void OverRangeControl(const vdec_reg_address_t * const vdec_reg, const vdec_over_range_t * const p_over_range);
static void YcSeparationControl(
    const vdec_reg_address_t    * const vdec_reg,
    const vdec_yc_sep_ctrl_t    * const p_yc_sep_ctrl);
static void FilterTAPsCoefficient(
    volatile uint16_t       * const   * fil_reg_address,
    const vdec_chrfil_tap_t * const     fil2_2d);


/**************************************************************************//**
 * @brief       Sets registers for initialization
 * @param[in]   ch                      : Channel
 * @param[in]   vinsel                  : Input pin control
 * @retval      None
 *****************************************************************************/
void VDEC_Initialize (const vdec_channel_t ch, const vdec_adc_vinsel_t vinsel)
{
    const vdec_reg_address_t  * vdec_reg;
    uint16_t                    reg_data;

    vdec_reg = &vdec_reg_address[ch];

    /* Input pin control */
    if (vinsel == VDEC_ADC_VINSEL_VIN1) {
        reg_data            = (uint16_t)((uint32_t)*(vdec_reg->adccr2) & (~0x0001u));
        *(vdec_reg->adccr2) = reg_data;
    } else {
        reg_data            = (uint16_t)((uint32_t)*(vdec_reg->adccr2) | (0x0001u));
        *(vdec_reg->adccr2) = reg_data;
    }
    return;
}   /* End of function VDEC_Initialize() */

/**************************************************************************//**
 * @brief       Sets registers for active image period
 * @param[in]   ch                      : Channel
 * @param[in]   param                   : Active image period parameter
 * @retval      None
 *****************************************************************************/
void VDEC_ActivePeriod (const vdec_channel_t ch, const vdec_active_period_t * const param)
{
    const vdec_reg_address_t * vdec_reg;

    vdec_reg = &vdec_reg_address[ch];

    /* Left end of input video signal capturing area */
    *(vdec_reg->tgcr1) = param->srcleft;
    /* Top end of input video signal capturing area
       and height of input video signal capturing area */
    *(vdec_reg->tgcr2) = (uint16_t)(((uint32_t)param->srctop << VEDC_REG_SHIFT_10) | (uint32_t)param->srcheight);
    /* Width of input video signal capturing area */
    *(vdec_reg->tgcr3) = param->srcwidth;

    return;
}   /* End of function VDEC_ActivePeriod() */

/**************************************************************************//**
 * @brief       Sets registers for sync separation
 * @param[in]   ch                      : Channel
 * @param[in]   param                   : Sync separation parameter
 * @retval      None
 *****************************************************************************/
void VDEC_SyncSeparation (const vdec_channel_t ch, const vdec_sync_separation_t * const param)
{
    const vdec_reg_address_t  * vdec_reg;

    vdec_reg            = &vdec_reg_address[ch];

    /* Noise reduction LPF */
    NoiseReductionLPF(vdec_reg, param->noise_rd_lpf);
    /* Auto level control sync slicer */
    SyncSlicer(vdec_reg, param->sync_slicer);
    /* Horizontal AFC */
    HorizontalAFC(vdec_reg, param->horizontal_afc);
    /* Vertical count-down */
    VerticalCountdown(vdec_reg, param->vcount_down);
    /* AGC/PGA */
    AgcPga(vdec_reg, param->agc);
    /* Peak limiter control */
    PeakLimiterControl(vdec_reg, param->peak_limiter);

    return;
}   /* End of function VDEC_SyncSeparation() */

/**************************************************************************//**
 * @brief       Sets registers for Y/C separation
 * @param[in]   ch                      : Channel
 * @param[in]   param                   : Y/C separation parameter
 * @retval      None
 *****************************************************************************/
void VDEC_YcSeparation (const vdec_channel_t ch, const vdec_yc_separation_t * const param)
{
    const vdec_reg_address_t  * vdec_reg;

    vdec_reg        = &vdec_reg_address[ch];

    /* Over-range control */
    OverRangeControl(vdec_reg, param->over_range);
    /* Y/C separation control */
    YcSeparationControl(vdec_reg, param->yc_sep_ctrl);

    /* Two-dimensional cascade broadband (3.58/4.43/SECAM-DR)/TAKE-OFF filter TAP coefficient */
    FilterTAPsCoefficient(vdec_filter_reg_address[ch].yctwa_f, param->fil2_2d_wa);
    /* Two-dimensional cascade broadband (SECAM-DB) filter TAP coefficient */
    FilterTAPsCoefficient(vdec_filter_reg_address[ch].yctwb_f, param->fil2_2d_wb);
    /* Two-dimensional cascade narrowband (3.58/4.43/SECAM-DR) filter TAP coefficient */
    FilterTAPsCoefficient(vdec_filter_reg_address[ch].yctna_f, param->fil2_2d_na);
    /* Two-dimensional cascade narrowband (SECAMDB) filter TAP coefficient */
    FilterTAPsCoefficient(vdec_filter_reg_address[ch].yctnb_f, param->fil2_2d_nb);

    return;
}   /* End of function VDEC_YcSeparation() */

/**************************************************************************//**
 * @brief       Sets registers for chroma decoding
 * @param[in]   ch                      : Channel
 * @param[in]   param                   : Chroma decoding parameter
 * @retval      None
 *****************************************************************************/
void VDEC_ChromaDecoding (const vdec_channel_t ch, const vdec_chroma_decoding_t * const param)
{
    const vdec_reg_address_t  * vdec_reg;
    vdec_chrmdec_ctrl_t       * p_chrmdec_ctrl;
    vdec_burst_lock_t         * p_burst_lock;
    vdec_acc_t                * p_acc;
    vdec_tint_ry_t            * p_tint_ry;
    uint32_t                    reg_data;

    vdec_reg        = &vdec_reg_address[ch];
    p_chrmdec_ctrl  = param->chrmdec_ctrl;
    p_burst_lock    = param->burst_lock;
    p_acc           = param->acc;
    p_tint_ry       = param->tint_ry;

    /* Color system detection */
    if (p_chrmdec_ctrl != NULL) {
        reg_data    = (uint32_t)*(vdec_reg->btlcr) & (uint32_t)(~VDEC_REG_MASK_0X00FF);
        /* Default color system */
        reg_data   |= (uint32_t)p_chrmdec_ctrl->defaultsys << VEDC_REG_SHIFT_6;
        /* NTSC-M detection control */
        reg_data   |= (p_chrmdec_ctrl->nontsc358_ == VDEC_OFF) ? (uint32_t)VDEC_REG_SET_0X0020 : (uint32_t)0x0000u;
        /* NTSC-4.43 detection control */
        reg_data   |= (p_chrmdec_ctrl->nontsc443_ == VDEC_OFF) ? (uint32_t)VDEC_REG_SET_0X0010 : (uint32_t)0x0000u;
        /* PAL-M detection control */
        reg_data   |= (p_chrmdec_ctrl->nopalm_ == VDEC_OFF) ? (uint32_t)VDEC_REG_SET_0X0008 : (uint32_t)0x0000u;
        /* PAL-N detection control */
        reg_data   |= (p_chrmdec_ctrl->nopaln_ == VDEC_OFF) ? (uint32_t)VDEC_REG_SET_0X0004 : (uint32_t)0x0000u;
        /* PAL-B, G, H, I, D detection control */
        reg_data   |= (p_chrmdec_ctrl->nopal443_ == VDEC_OFF) ? (uint32_t)VDEC_REG_SET_0X0002 : (uint32_t)0x0000u;
        /* SECAM detection control */
        reg_data   |= (p_chrmdec_ctrl->nosecam_ == VDEC_OFF) ? (uint32_t)VDEC_REG_SET_0X0001 : (uint32_t)0x0000u;

        *(vdec_reg->btlcr) = (uint16_t)reg_data;

        /* Luminance signal delay adjustment */
        reg_data    = (uint32_t)p_chrmdec_ctrl->lumadelay << VEDC_REG_SHIFT_4;
        /* LPF for demodulated chroma */
        reg_data   |= (p_chrmdec_ctrl->chromalpf == VDEC_OFF) ? (uint32_t)0x0000u : (uint32_t)VDEC_REG_SET_0X0004;
        /* Averaging processing for pre-demodulated line */
        reg_data   |= (uint32_t)p_chrmdec_ctrl->demodmode;

        *(vdec_reg->ycdcr) = (uint16_t)reg_data;
    }
    /* BCO */
    if (p_burst_lock != NULL) {
        reg_data    = (uint32_t)*(vdec_reg->btlcr) & (uint32_t)(~VDEC_REG_MASK_0XFC00);
        /* Burst lock PLL lock range */
        reg_data   |= (uint32_t)p_burst_lock->lockrange << VEDC_REG_SHIFT_14;
        /* Burst lock PLL loop gain */
        reg_data   |= (uint32_t)p_burst_lock->loopgain << VEDC_REG_SHIFT_12;
        /* Level for burst lock PLL to re-search free-run frequency */
        reg_data   |= (uint32_t)p_burst_lock->locklimit << VEDC_REG_SHIFT_10;

        *(vdec_reg->btlcr) = (uint16_t)reg_data;

        /* burst gate pulse position check */
        reg_data    = (uint32_t)p_burst_lock->bgpcheck << VEDC_REG_SHIFT_15;
        /* burst gate pulse width */
        reg_data   |= (uint32_t)p_burst_lock->bgpwidth << VEDC_REG_SHIFT_8;
        /* burst gate pulse start position */
        reg_data   |= (uint32_t)p_burst_lock->bgpstart;

        *(vdec_reg->btgpcr) = (uint16_t)reg_data;
    }
    /* ACC and color killer */
    if (p_acc != NULL) {
        /* ACC operating mode */
        reg_data    = (p_acc->accmode == VDEC_ACC_MD_AUTO) ? (uint32_t)0x0000u : (uint32_t)VDEC_REG_SET_0X0800;
        /* Maximum ACC Gain */
        reg_data   |= (uint32_t)p_acc->accmaxgain << VEDC_REG_SHIFT_9;
        /* ACC reference color burst amplitude */
        reg_data   |= (uint32_t)p_acc->acclevel;
        /* Color killer offset */
        reg_data   |= (uint32_t)p_acc->killeroffset << VEDC_REG_SHIFT_12;

        *(vdec_reg->acccr1) = (uint16_t)reg_data;

        /* Chroma manual gain (sub) */
        reg_data    = (uint32_t)p_acc->chromasubgain << VEDC_REG_SHIFT_9;
        /* Chroma manual gain (main) */
        reg_data   |= (uint32_t)p_acc->chromamaingain;

        *(vdec_reg->acccr2) = (uint16_t)reg_data;

        /* ACC response speed */
        reg_data    = (uint32_t)p_acc->accresponse << VEDC_REG_SHIFT_14;
        /* ACC gain adjustment accuracy */
        reg_data   |= (uint32_t)p_acc->accprecis << VEDC_REG_SHIFT_8;
        /* Forced color killer mode ON/OFF */
        reg_data   |= (p_acc->killermode == VDEC_OFF) ? (uint32_t)0x0000u : (uint32_t)VDEC_REG_SET_0X0080;
        /* Color killer operation start point */
        reg_data   |= (uint32_t)p_acc->killerlevel << VEDC_REG_SHIFT_1;

        *(vdec_reg->acccr3) = (uint16_t)reg_data;
    }
    /* TINT correction/R-Y axis correction (only valid for NTSC/PAL) */
    if (p_tint_ry != NULL) {
        /* Fine adjustment of R-Y demodulation axis and hue adjustment level */
        reg_data    = (uint32_t)p_tint_ry->tintsub << VEDC_REG_SHIFT_10;
        reg_data   |= (uint32_t)p_tint_ry->tintmain;

        *(vdec_reg->tintcr) = (uint16_t)reg_data;
    }
    return;
}   /* End of function VDEC_ChromaDecoding() */

/**************************************************************************//**
 * @brief       Sets registers for digital clamp
 * @param[in]   ch                      : Channel
 * @param[in]   param                   : Digital clamp parameter
 * @retval      None
 *****************************************************************************/
void VDEC_DigitalClamp (const vdec_channel_t ch, const vdec_degital_clamp_t * const param)
{
    const vdec_reg_address_t  * vdec_reg;
    vdec_pedestal_clamp_t     * p_pedestal_clamp;
    vdec_center_clamp_t       * p_center_clamp;
    vdec_noise_det_t          * p_noise_det;
    uint32_t                    reg_data;

    vdec_reg            = &vdec_reg_address[ch];
    p_pedestal_clamp    = param->pedestal_clamp;
    p_center_clamp      = param->center_clamp;
    p_noise_det         = param->noise_det;

    /* Digital clamp pulse position check */
    reg_data = (uint32_t)*(vdec_reg->dcpcr1);
    if (param->dcpcheck == VDEC_OFF) {
        reg_data &= (uint32_t)(~VDEC_REG_SET_0X0800);
    } else {
        reg_data |= (uint32_t)VDEC_REG_SET_0X0800;
    }
    *(vdec_reg->dcpcr1) = (uint16_t)reg_data;

    /* Digital clamp response speed */
    *(vdec_reg->dcpcr3) = (uint16_t)((uint32_t)param->dcpresponse << VEDC_REG_SHIFT_12);
    /* Digital clamp start line */
    *(vdec_reg->dcpcr4) = (uint16_t)((uint32_t)param->dcpstart << VEDC_REG_SHIFT_10);
    /* Digital clamp end line */
    *(vdec_reg->dcpcr5) = (uint16_t)((uint32_t)param->dcpend << VEDC_REG_SHIFT_10);
    /* Digital clamp pulse width */
    *(vdec_reg->dcpcr6) = (uint16_t)((uint32_t)param->dcpwidth << VEDC_REG_SHIFT_8);

    /* Pedestal clamp */
    if (p_pedestal_clamp != NULL) {
        reg_data    = (uint32_t)*(vdec_reg->dcpcr1) & (uint32_t)(~VDEC_REG_MASK_0X83FF);
        /* Clamp level setting mode (Y signal) */
        reg_data   |= (p_pedestal_clamp->dcpmode_y == VDEC_DCPMODE_MANUAL) ? (uint32_t)0x0000u :
                      (uint32_t)VDEC_REG_SET_0X8000;
        /* Clamp offset level (Y signal) */
        reg_data   |= (uint32_t)p_pedestal_clamp->blanklevel_y;

        *(vdec_reg->dcpcr1) = (uint16_t)reg_data;

        /* Digital clamp pulse horizontal start position (Y signal) */
        *(vdec_reg->dcpcr7) = (uint16_t)((uint32_t)p_pedestal_clamp->dcppos_y << VEDC_REG_SHIFT_8);
    }
    /* Center clamp */
    if (p_center_clamp != NULL) {
        /* Clamp level setting mode (Cb/Cr signal) */
        reg_data    = (p_center_clamp->dcpmode_c == VDEC_DCPMODE_MANUAL) ? (uint32_t)0x0000u :
                      (uint32_t)VDEC_REG_SET_0X8000;
        /* Clamp offset level (Cb signal) */
        reg_data   |= (uint32_t)p_center_clamp->blanklevel_cb << VEDC_REG_SHIFT_6;
        /* Clamp offset level (Cr signal) */
        reg_data   |= (uint32_t)p_center_clamp->blanklevel_cr;

        *(vdec_reg->dcpcr2) = (uint16_t)reg_data;

        /* Digital clamp pulse horizontal start position (Cb/Cr signal) */
        *(vdec_reg->dcpcr8) = (uint16_t)((uint32_t)p_center_clamp->dcppos_c << VEDC_REG_SHIFT_8);
    }
    /* Noise detection */
    if (p_noise_det != NULL) {
        /* Video signal for autocorrelation function */
        reg_data    = (uint32_t)p_noise_det->acfinput << VEDC_REG_SHIFT_12;
        /* Delay time for autocorrelation function calculation */
        reg_data   |= (uint32_t)p_noise_det->acflagtime << VEDC_REG_SHIFT_4;
        /* Smoothing parameter of autocorrelation function data */
        reg_data   |= (uint32_t)p_noise_det->acffilter;

        *(vdec_reg->nsdcr) = (uint16_t)reg_data;
    }
    /* Clamp data hold processing (Y, Cb, Cr) OFF */
    reg_data            = (uint32_t)*(vdec_reg->dcpcr9) & (uint32_t)(~VDEC_REG_MASK_0X1C00);
    *(vdec_reg->dcpcr9) = (uint16_t)reg_data;

    return;
}   /* End of function VDEC_DigitalClamp() */

/**************************************************************************//**
 * @brief       Sets registers for output adjustment
 * @param[in]   ch                      : Channel
 * @param[in]   param                   : Output adjustment parameter
 * @retval      None
 *****************************************************************************/
void VDEC_Output (const vdec_channel_t ch, const vdec_output_t * const param)
{
    const vdec_reg_address_t * vdec_reg;

    vdec_reg = &vdec_reg_address[ch];

    /* Y, Cb and Cr signal gain coefficient */
    *(vdec_reg->ygaincr)    = param->y_gain2;
    *(vdec_reg->cbgaincr)   = param->cb_gain2;
    *(vdec_reg->crgaincr)   = param->cr_gain2;

    return;
}   /* End of function VDEC_Output() */

/**************************************************************************//**
 * @brief       Query VDEC parameters
 * @param[in]   ch                      : Channel
 * @param[out]  q_sync_sep              : Sync separation parameters
 * @param[out]  q_agc                   : Agc parameters
 * @param[out]  q_chroma_dec            : Chroma decoding parameters
 * @param[out]  q_digital_clamp         : Digital clamp parameters
 * @retval      None
 *****************************************************************************/
void VDEC_Query (
    const vdec_channel_t            ch,
    vdec_q_sync_sep_t       * const q_sync_sep,
    vdec_q_agc_t            * const q_agc,
    vdec_q_chroma_dec_t     * const q_chroma_dec,
    vdec_q_digital_clamp_t  * const q_digital_clamp)
{
    const vdec_reg_address_t  * vdec_reg;
    uint32_t                    reg_value;

    vdec_reg = &vdec_reg_address[ch];

    /* Sync separation */
    if (q_sync_sep != NULL) {
        reg_value = (uint32_t)*(vdec_reg->vsyncsr);
        /* Horizontal AFC lock detection result */
        q_sync_sep->fhlock = ((reg_value & (uint32_t)VDEC_REG_BIT_MASK_0X4000) == 0u) ? VDEC_UNLOCK : VDEC_LOCK;
        /* Detection result of low S/N signal by sync separation */
        q_sync_sep->isnoisy = ((reg_value & (uint32_t)VDEC_REG_BIT_MASK_0X2000) == 0u) ? VDEC_NO : VDEC_YES;
        /* Speed detection result */
        q_sync_sep->fhmode = ((reg_value & (uint32_t)VDEC_REG_BIT_MASK_0X1000) == 0u) ? VDEC_FHMD_NORMAL :
                             VDEC_FHMD_MULTIPLIED;
        /* No-signal detection result */
        q_sync_sep->nosignal_ = ((reg_value & (uint32_t)VDEC_REG_BIT_MASK_0X0800) == 0u) ? VDEC_YES : VDEC_NO;
        /* Vertical countdown lock detection result */
        q_sync_sep->fvlock = ((reg_value & (uint32_t)VDEC_REG_BIT_MASK_0X0400) == 0u) ? VDEC_UNLOCK : VDEC_LOCK;
        /* Vertical countdown oscillation mode */
        q_sync_sep->fvmode = ((reg_value & (uint32_t)VDEC_REG_BIT_MASK_0X0200) == 0u) ? VDEC_FVMD_50HZ :
                             VDEC_FVMD_60HZ;
        /* Interlace detection result */
        q_sync_sep->interlaced = ((reg_value & (uint32_t)VDEC_REG_BIT_MASK_0X0100) == 0u) ? VDEC_NO : VDEC_YES;
        /* Vertical cycle measurement result */
        q_sync_sep->fvcount = (uint16_t)(reg_value & (uint32_t)VDEC_REG_MASK_0X00FF);
        /* Horizontal AFC oscillation cycle */
        q_sync_sep->fhcount  = ((reg_value & (uint32_t)VDEC_REG_BIT_MASK_0X8000) == 0u) ? 0x0000u : (uint32_t)0x0001u;
        q_sync_sep->fhcount |= (uint32_t)*(vdec_reg->hsyncsr) << VEDC_REG_SHIFT_1;

        reg_value = (uint32_t)*(vdec_reg->syncssr);
        /* Sync amplitude detection result during VBI period */
        q_sync_sep->isreduced = ((reg_value & (uint32_t)VDEC_REG_BIT_MASK_0X1000) == 0u) ? VDEC_NO : VDEC_YES;
        /* Sync pulse amplitude detection result */
        q_sync_sep->syncdepth = (uint16_t)(reg_value & (uint32_t)VDEC_REG_MASK_0X03FF);
    }
    /* Agc */
    if (q_agc != NULL) {
        reg_value = (uint32_t)*(vdec_reg->agccsr1);
        /* Number of pixels which have larger luminance value than peak luminance limited by peak limiter */
        q_agc->highsamples = (uint16_t)(reg_value >> VEDC_REG_SHIFT_8);
        /* Number of overflowing pixels */
        q_agc->peaksamples = (uint16_t)(reg_value & (uint32_t)VDEC_REG_MASK_0X00FF);

        reg_value = (uint32_t)*(vdec_reg->agccsr2);
        /* AGC convergence detection result */
        if ((reg_value & (uint32_t)VDEC_REG_BIT_MASK_0X0100) == 0u) {
            q_agc->agcconverge = (uint16_t)0x0000u;
        } else {
            q_agc->agcconverge = (uint16_t)0x0001u;
        }
        /* Current AGC gain value */
        q_agc->agcgain = (uint16_t)(reg_value & (uint32_t)VDEC_REG_MASK_0X00FF);
    }
    /* Chroma decoding */
    if (q_chroma_dec != NULL) {
        reg_value = (uint32_t)*(vdec_reg->cromasr1);
        /* Color system detection result */
        q_chroma_dec->colorsys = (vdec_color_sys_t)(reg_value >> VEDC_REG_SHIFT_14);
        /* Color sub-carrier frequency detection result */
        q_chroma_dec->fscmode = ((reg_value & (uint32_t)VDEC_REG_BIT_MASK_0X2000) == 0u) ? VDEC_FSCMD_3_58 :
                                VDEC_FSCMD_4_43;
        /* Burst lock PLL lock state detection result */
        q_chroma_dec->fsclock = ((reg_value & (uint32_t)VDEC_REG_BIT_MASK_0X1000) == 0u) ? VDEC_UNLOCK : VDEC_LOCK;
        /* Color burst detection result */
        q_chroma_dec->noburst_ = ((reg_value & (uint32_t)VDEC_REG_BIT_MASK_0X0800) == 0u) ? VDEC_YES : VDEC_NO;
        /* Current ACC gain value (sub) */
        q_chroma_dec->accsubgain = (vdec_chrm_subgain_t)((reg_value >> VEDC_REG_SHIFT_9) &
                                   (uint32_t)VDEC_REG_MASK_0X0003);
        /* Current ACC gain value (main) */
        q_chroma_dec->accmaingain = (uint16_t)(reg_value & (uint32_t)VDEC_REG_MASK_0X01FF);

        reg_value = (uint32_t)*(vdec_reg->cromasr2);
        /* SECAM detection result */
        q_chroma_dec->issecam = ((reg_value & (uint32_t)VDEC_REG_BIT_MASK_0X1000) == 0u) ? VDEC_NO : VDEC_YES;
        /* PAL detection result */
        q_chroma_dec->ispal = ((reg_value & (uint32_t)VDEC_REG_BIT_MASK_0X0800) == 0u) ? VDEC_NO : VDEC_YES;
        /* NTSC detection result */
        q_chroma_dec->isntsc = ((reg_value & (uint32_t)VDEC_REG_BIT_MASK_0X0400) == 0u) ? VDEC_NO : VDEC_YES;
        /* Low S/N signal detection result by burst lock PLL */
        q_chroma_dec->locklevel = (uint16_t)(reg_value & (uint32_t)VDEC_REG_MASK_0X00FF);
    }
    /* Digital clamp */
    if (q_digital_clamp != NULL) {
        reg_value = (uint32_t)*(vdec_reg->dcpsr1);
        /* Digital clamp subtraction value (Cb signal) */
        q_digital_clamp->clamplevel_cb = (uint16_t)(reg_value >> VEDC_REG_SHIFT_10);
        /* Digital clamp subtraction value (Y signal) */
        q_digital_clamp->clamplevel_y = (uint16_t)(reg_value & (uint32_t)VDEC_REG_MASK_0X03FF);

        /* Digital clamp subtraction value (Cr signal) */
        q_digital_clamp->clamplevel_cr = (uint16_t)((uint32_t)*(vdec_reg->dcpsr2) >> VEDC_REG_SHIFT_10);

        /* Noise autocorrelation strength at digital clamp pulse position */
        q_digital_clamp->acfstrength = *(vdec_reg->nsdsr);
    }
    return;
}   /* End of function VDEC_Query() */

/**************************************************************************//**
 * @brief       Sets registers for noise reduction LPF
 * @param[in]   vdec_reg                : VDEC registers
 * @param[in]   p_noise_rd_lpf          : Noise reduction LPF parameter
 * @retval      None
 *****************************************************************************/
static void NoiseReductionLPF (
    const vdec_reg_address_t    * const vdec_reg,
    const vdec_noise_rd_lpf_t   * const p_noise_rd_lpf)
{
    uint32_t reg_data;

    /* Noise reduction LPF */
    if (p_noise_rd_lpf != NULL) {
        reg_data    = (uint32_t)*(vdec_reg->synscr1) & (uint32_t)(~VDEC_REG_MASK_0XFC00);
        /* LPF cutoff frequency before vertical sync separation */
        reg_data   |= (uint32_t)p_noise_rd_lpf->lpfvsync << VEDC_REG_SHIFT_13;
        /* LPF cutoff frequency before horizontal sync separation */
        reg_data   |= (uint32_t)p_noise_rd_lpf->lpfhsync << VEDC_REG_SHIFT_10;

        *(vdec_reg->synscr1) = (uint16_t)reg_data;
    }
    return;
}   /* End of function NoiseReductionLPF() */

/**************************************************************************//**
 * @brief       Sets registers for auto level control sync slicer
 * @param[in]   vdec_reg                : VDEC registers
 * @param[in]   p_sync_slicer           : Auto level control sync slicer parameter
 * @retval      None
 *****************************************************************************/
static void SyncSlicer (const vdec_reg_address_t * const vdec_reg, const vdec_sync_slicer_t * const p_sync_slicer)
{
    uint32_t reg_data;

    /* Auto level control sync slicer */
    if (p_sync_slicer != NULL) {
        reg_data    = (uint32_t)*(vdec_reg->synscr1) & (uint32_t)(~VDEC_REG_MASK_0X00FF);
        /* Reference level operation speed control for composite sync separation (for Hsync signal) */
        reg_data   |= (uint32_t)p_sync_slicer->velocityshift_h << VEDC_REG_SHIFT_4;
        /* Auto-slice level setting for composite sync separation circuit (for Hsync signal) */
        reg_data   |= (uint32_t)p_sync_slicer->slicermode_h << VEDC_REG_SHIFT_2;
        /* Auto-slice level setting for composite sync separation circuit (for Vsync signal) */
        reg_data   |= (uint32_t)p_sync_slicer->slicermode_v;

        *(vdec_reg->synscr1) = (uint16_t)reg_data;

        /* Max ratio of horizontal cycle to horizontal sync signal pulse width
           and min ratio of horizontal cycle to horizontal sync signal pulse width (for Hsync signal) */
        reg_data    = (uint32_t)p_sync_slicer->syncmaxduty_h << VEDC_REG_SHIFT_6;
        reg_data   |= (uint32_t)p_sync_slicer->syncminduty_h;

        *(vdec_reg->synscr2) = (uint16_t)reg_data;

        /* Clipping level and slice level for composite sync signal separation (for Hsync signal) */
        reg_data    = (uint32_t)p_sync_slicer->ssclipsel << VEDC_REG_SHIFT_10;
        reg_data   |= (uint32_t)p_sync_slicer->csyncslice_h;

        *(vdec_reg->synscr3) = (uint16_t)reg_data;

        /* Max ratio of horizontal cycle to horizontal sync signal pulse width
           and min ratio of horizontal cycle to horizontal sync signal pulse width (for Vsync signal) */
        reg_data    = (uint32_t)p_sync_slicer->syncmaxduty_v << VEDC_REG_SHIFT_6;
        reg_data   |= (uint32_t)p_sync_slicer->syncminduty_v;

        *(vdec_reg->synscr4) = (uint16_t)reg_data;

        /* Delays the separated vertical sync signal for 1/4 horizontal cycle */
        reg_data    = (p_sync_slicer->vsyncdelay == VDEC_OFF) ? (uint32_t)0x0000u : (uint32_t)VDEC_REG_SET_0X8000;
        /* Threshold for vertical sync separation */
        reg_data   |= (uint32_t)p_sync_slicer->vsyncslice << VEDC_REG_SHIFT_10;
        /* Slice level for composite sync signal separation (for Vsync signal) */
        reg_data   |= (uint32_t)p_sync_slicer->csyncslice_v;

        *(vdec_reg->synscr5) = (uint16_t)reg_data;
    }
    return;
}   /* End of function SyncSlicer() */

/**************************************************************************//**
 * @brief       Sets registers for horizontal AFC
 * @param[in]   vdec_reg                : VDEC registers
 * @param[in]   p_horizontal_afc        : Horizontal AFC parameter
 * @retval      None
 *****************************************************************************/
static void HorizontalAFC (
    const vdec_reg_address_t    * const vdec_reg,
    const vdec_horizontal_afc_t * const p_horizontal_afc)
{
    uint32_t reg_data;

    /* Horizontal AFC */
    if (p_horizontal_afc != NULL) {
        reg_data    = (uint32_t)*(vdec_reg->hafccr1) & (uint32_t)(~VDEC_REG_MASK_0XF3FF);
        /* Horizontal AFC loop gain */
        reg_data   |= (uint32_t)p_horizontal_afc->hafcgain << VEDC_REG_SHIFT_12;
        /* Horizontal AFC center oscillation frequency */
        reg_data   |= (uint32_t)p_horizontal_afc->hafctyp;

        *(vdec_reg->hafccr1) = (uint16_t)reg_data;

        /* Start line of horizontal AFC normal operation
           and Horizontal AFC forced double-speed oscillation (DOX2HOSC = 0, auto control) */
        reg_data    = (uint32_t)p_horizontal_afc->hafcstart << VEDC_REG_SHIFT_12;
        /* Disable of horizontal AFC double speed detection */
        reg_data   |= (p_horizontal_afc->nox2hosc == VDEC_OFF) ? (uint32_t)0x0000u : (uint32_t)VDEC_REG_SET_0X0800;
        /* Maximum oscillation frequency of horizontal AFC */
        reg_data   |= (uint32_t)p_horizontal_afc->hafcmax;

        *(vdec_reg->hafccr2) = (uint16_t)reg_data;

        /* End line of horizontal AFC normal operation */
        reg_data    = (uint32_t)p_horizontal_afc->hafcend << VEDC_REG_SHIFT_12;
        /* Horizontal AFC VBI period operating mode */
        reg_data   |= (uint32_t)p_horizontal_afc->hafcmode << VEDC_REG_SHIFT_10;
        /* Min oscillation frequency of horizontal AFC */
        reg_data   |= (uint32_t)p_horizontal_afc->hafcmin;

        *(vdec_reg->hafccr3) = (uint16_t)reg_data;

        /* Forcible or LOWGAIN control */
        reg_data    = (p_horizontal_afc->phdet_fix == VDEC_OFF) ? (uint32_t)0x0000u : (uint32_t)VDEC_REG_SET_0X0010;
        /* Phase comparator feedback adjust for low sync signal lock stability */
        reg_data   |= (uint32_t)p_horizontal_afc->phdet_div;

        *(vdec_reg->afcpfcr) = (uint16_t)reg_data;
    }
    return;
}   /* End of function HorizontalAFC() */

/**************************************************************************//**
 * @brief       Sets registers for vertical count-down
 * @param[in]   vdec_reg                : VDEC registers
 * @param[in]   p_vcount_down           : Vertical count-down parameter
 * @retval      None
 *****************************************************************************/
static void VerticalCountdown (
    const vdec_reg_address_t    * const vdec_reg,
    const vdec_vcount_down_t    * const p_vcount_down)
{
    uint32_t reg_data;

    /* Vertical count-down */
    if (p_vcount_down != NULL) {
        /* Vertical countdown 50-Hz oscillation mode
           and Vertical countdown free-run oscillation mode (VCDFREERUN = OFF) */
        reg_data    = (p_vcount_down->novcd50_ == VDEC_OFF) ? (uint32_t)VDEC_REG_SET_0X4000 : (uint32_t)0x0000u;
        /* Vertical countdown 60-Hz (59.94-Hz) oscillation mode */
        reg_data   |= (p_vcount_down->novcd60_ == VDEC_OFF) ? (uint32_t)VDEC_REG_SET_0X2000 : (uint32_t)0x0000u;
        /* Vertical countdown center oscillation frequency */
        reg_data   |= (uint32_t)p_vcount_down->vcddefault << VEDC_REG_SHIFT_11;
        /* Vertical countdown sync area */
        reg_data   |= (uint32_t)p_vcount_down->vcdwindow << VEDC_REG_SHIFT_5;
        /* Vertical countdown minimum oscillation frequency */
        reg_data   |= (uint32_t)p_vcount_down->vcdoffset;

        *(vdec_reg->vcdwcr1) = (uint16_t)reg_data;
    }
    return;
}   /* End of function VerticalCountdown() */

/**************************************************************************//**
 * @brief       Sets registers for AGC/PGA
 * @param[in]   vdec_reg                : VDEC registers
 * @param[in]   p_agc                   : AGC/PGA parameter
 * @retval      None
 *****************************************************************************/
static void AgcPga (const vdec_reg_address_t * const vdec_reg, const vdec_agc_t * const p_agc)
{
    uint32_t reg_data;

    /* AGC/PGA */
    if (p_agc != NULL) {
        /* A/D converter AGC ON/OFF control & PGA switch */
        if (p_agc->agcmode == VDEC_OFF) {
            reg_data            = (uint32_t)*(vdec_reg->pgacr) | (uint32_t)VDEC_REG_SET_0X2000;
            *(vdec_reg->pgacr)  = (uint16_t)reg_data;
            *(vdec_reg->adccr1) = (uint16_t)0u;
        } else {
            *(vdec_reg->adccr1) = (uint16_t)VDEC_REG_SET_0X0100;
            reg_data            = (uint32_t)*(vdec_reg->pgacr) & (uint32_t)(~VDEC_REG_SET_0X2000);
            *(vdec_reg->pgacr)  = (uint16_t)reg_data;
        }
        /* PGA gain */
        reg_data    = (uint32_t)*(vdec_reg->pgacr) & (uint32_t)(~VDEC_REG_MASK_0X1F00);
        reg_data   |= (uint32_t)p_agc->pga_gain << VEDC_REG_SHIFT_8;

        *(vdec_reg->pgacr) = (uint16_t)reg_data;

        /* PGA register update register */
        *(vdec_reg->pga_update) = (uint16_t)1u;

        /* Manual control of sync signal amplitude detection during VBI period */
        reg_data    = (p_agc->doreduce == VDEC_OFF) ? (uint32_t)0x0000u : (uint32_t)VDEC_REG_SET_0X2000;
        /* Control of sync signal amplitude detection during VBI period */
        reg_data   |= (p_agc->noreduce_ == VDEC_OFF) ? (uint32_t)VDEC_REG_SET_0X1000 : (uint32_t)0x0000u;
        /* AGC response speed */
        reg_data   |= (uint32_t)p_agc->agcresponse << VEDC_REG_SHIFT_9;
        /* Sync signal reference amplitude */
        reg_data   |= (uint32_t)p_agc->agclevel;

        *(vdec_reg->agccr1) = (uint16_t)reg_data;

        /* AGC gain adjustment accuracy */
        reg_data    = (uint32_t)*(vdec_reg->agccr2) & (uint32_t)(~VDEC_REG_MASK_0X3F00);
        reg_data   |= (uint32_t)p_agc->agcprecis << VEDC_REG_SHIFT_8;

        *(vdec_reg->agccr2) = (uint16_t)reg_data;
    }
    return;
}   /* End of function AgcPga() */

/**************************************************************************//**
 * @brief       Sets registers for peak limiter control
 * @param[in]   vdec_reg                : VDEC registers
 * @param[in]   p_peak_limiter          : Peak limiter control parameter
 * @retval      None
 *****************************************************************************/
static void PeakLimiterControl (
    const vdec_reg_address_t    * const vdec_reg,
    const vdec_peak_limiter_t   * const p_peak_limiter)
{
    uint32_t reg_data;

    /* Peak limiter control */
    if (p_peak_limiter != NULL) {
        /* Peak luminance value limited by peak limiter */
        reg_data    = (uint32_t)p_peak_limiter->peaklevel << VEDC_REG_SHIFT_14;
        /* Response speed with peak limiter gain decreased */
        reg_data   |= (uint32_t)p_peak_limiter->peakattack << VEDC_REG_SHIFT_12;
        /* Response speed with peak limiter gain increased */
        reg_data   |= (uint32_t)p_peak_limiter->peakrelease << VEDC_REG_SHIFT_10;
        /* Maximum compression rate of peak limiter */
        reg_data   |= (uint32_t)p_peak_limiter->peakratio << VEDC_REG_SHIFT_8;
        /* Allowable number of overflowing pixels */
        reg_data   |= (uint32_t)p_peak_limiter->maxpeaksamples;

        *(vdec_reg->pklimitcr) = (uint16_t)reg_data;
    }
    return;
}   /* End of function PeakLimiterControl() */

/**************************************************************************//**
 * @brief       Sets registers for over-range control
 * @param[in]   vdec_reg                : VDEC registers
 * @param[in]   p_over_range            : Over-range control parameter
 * @retval      None
 *****************************************************************************/
static void OverRangeControl (const vdec_reg_address_t * const vdec_reg, const vdec_over_range_t * const p_over_range)
{
    uint32_t reg_data;

    /* Over-range control */
    if (p_over_range != NULL) {
        /* A/D over-threshold level (between levels 0 and 1) */
        *(vdec_reg->rgorcr1) = p_over_range->radj_o_level0;
        /* A/D under-threshold level (between levels 2 and 3) */
        *(vdec_reg->rgorcr2) = p_over_range->radj_u_level0;
        /* A/D over-threshold level (between levels 1 and 2) */
        *(vdec_reg->rgorcr3) = p_over_range->radj_o_level1;
        /* A/D under-threshold level (between levels 1 and 2) */
        *(vdec_reg->rgorcr4) = p_over_range->radj_u_level1;
        /* A/D over-threshold level (between levels 2 and 3) */
        *(vdec_reg->rgorcr5) = p_over_range->radj_o_level2;
        /* A/D under-threshold level (between levels 0 and 1) */
        *(vdec_reg->rgorcr6) = p_over_range->radj_u_level2;

        reg_data    = (uint32_t)*(vdec_reg->rgorcr7) & (uint32_t)(~VDEC_REG_MASK_0X0007);
        /* Over-range detection enable */
        reg_data   |= (p_over_range->ucmp_sw == VDEC_OFF) ? (uint32_t)0x0000u : (uint32_t)VDEC_REG_SET_0X0004;
        /* Under-range detection enable */
        reg_data   |= (p_over_range->dcmp_sw == VDEC_OFF) ? (uint32_t)0x0000u : (uint32_t)VDEC_REG_SET_0X0002;
        /* Horizontal enlargement of over/under-range level */
        reg_data   |= (p_over_range->hwide_sw == VDEC_OFF) ? (uint32_t)0x0000u : (uint32_t)0x0001u;

        *(vdec_reg->rgorcr7) = (uint16_t)reg_data;
    }
    return;
}   /* End of function OverRangeControl() */

/**************************************************************************//**
 * @brief       Sets registers for Y/C separation control
 * @param[in]   vdec_reg                : VDEC registers
 * @param[in]   p_yc_sep_ctrl           : Y/C separation control parameter
 * @retval      None
 *****************************************************************************/
static void YcSeparationControl (
    const vdec_reg_address_t    * const vdec_reg,
    const vdec_yc_sep_ctrl_t    * const p_yc_sep_ctrl)
{
    uint32_t reg_data;

    /* Y/C separation control */
    if (p_yc_sep_ctrl != NULL) {
        /* Two-dimensional Y/C separation filter select coefficient (K15, K13, and K11) */
        reg_data    = (uint32_t)p_yc_sep_ctrl->k15 << VEDC_REG_SHIFT_12;
        reg_data   |= (uint32_t)p_yc_sep_ctrl->k13 << VEDC_REG_SHIFT_6;
        reg_data   |= (uint32_t)p_yc_sep_ctrl->k11;

        *(vdec_reg->ycscr3) = (uint16_t)reg_data;

        /* Two-dimensional Y/C separation filter select coefficient (K16, K14, and K12) */
        reg_data    = (uint32_t)p_yc_sep_ctrl->k16 << VEDC_REG_SHIFT_12;
        reg_data   |= (uint32_t)p_yc_sep_ctrl->k14 << VEDC_REG_SHIFT_6;
        reg_data   |= (uint32_t)p_yc_sep_ctrl->k12;

        *(vdec_reg->ycscr4) = (uint16_t)reg_data;

        /* Two-dimensional Y/C separation filter select coefficient (K22A and K21A) */
        reg_data    = (uint32_t)p_yc_sep_ctrl->k22a << VEDC_REG_SHIFT_8;
        reg_data   |= (uint32_t)p_yc_sep_ctrl->k21a;

        *(vdec_reg->ycscr5) = (uint16_t)reg_data;

        /* Two-dimensional Y/C separation filter select coefficient (K22B and K21B) */
        reg_data    = (uint32_t)p_yc_sep_ctrl->k22b << VEDC_REG_SHIFT_8;
        reg_data   |= (uint32_t)p_yc_sep_ctrl->k21b;

        *(vdec_reg->ycscr6) = (uint16_t)reg_data;

        /* Two-dimensional Y/C separation filter select coefficient (K23B, K23A, and K24) */
        reg_data    = (uint32_t)*(vdec_reg->ycscr7) & (uint32_t)(~VDEC_REG_MASK_0XFF1F);
        reg_data   |= (uint32_t)p_yc_sep_ctrl->k23b << VEDC_REG_SHIFT_12;
        reg_data   |= (uint32_t)p_yc_sep_ctrl->k23a << VEDC_REG_SHIFT_8;
        reg_data   |= (uint32_t)p_yc_sep_ctrl->k24;

        *(vdec_reg->ycscr7) = (uint16_t)reg_data;

        reg_data    = (uint32_t)*(vdec_reg->ycscr8) & (uint32_t)(~VDEC_REG_MASK_0XF800);
        /* Latter-stage horizontal BPF select */
        reg_data   |= (p_yc_sep_ctrl->hbpf_narrow == VDEC_LSTG_BPFSEL_BYPASS) ? (uint32_t)0x0000u :
                      (uint32_t)VDEC_REG_SET_0X8000;
        /* Latter-stage horizontal/vertical BPF select */
        reg_data   |= (p_yc_sep_ctrl->hvbpf_narrow == VDEC_LSTG_BPFSEL_BYPASS) ? (uint32_t)0x0000u :
                      (uint32_t)VDEC_REG_SET_0X4000;
        /* Former-stage horizontal BPF select */
        reg_data   |= (p_yc_sep_ctrl->hbpf1_9tap_on == VDEC_FSTG_BPFSEL_17TAP) ? (uint32_t)0x0000u :
                      (uint32_t)VDEC_REG_SET_0X2000;
        /* Former-stage horizontal/vertical BPF select */
        reg_data   |= (p_yc_sep_ctrl->hvbpf1_9tap_on == VDEC_FSTG_BPFSEL_17TAP) ? (uint32_t)0x0000u :
                      (uint32_t)VDEC_REG_SET_0X1000;
        /* Horizontal filter and horizontal/vertical filter bandwidth switch signal */
        reg_data   |= (p_yc_sep_ctrl->hfil_tap_sel == VDEC_HFIL_TAP_SEL_17TAP) ? (uint32_t)0x0000u :
                      (uint32_t)VDEC_REG_SET_0X0800;

        *(vdec_reg->ycscr8) = (uint16_t)reg_data;

        /* Two-dimensional filter mixing select */
        reg_data    = (p_yc_sep_ctrl->det2_on == VDEC_OFF) ? (uint32_t)0x0000u : (uint32_t)VDEC_REG_SET_0X8000;
        /* Mixing ratio of signal after passing horizontal filter
          to signal after passing former-stage horizontal filter */
        reg_data   |= (uint32_t)p_yc_sep_ctrl->hsel_mix_y << VEDC_REG_SHIFT_8;
        /* Mixing ratio of signal after passing vertical filter
           to signal after passing former-stage horizontal/vertical filter */
        reg_data   |= (uint32_t)p_yc_sep_ctrl->vsel_mix_y << VEDC_REG_SHIFT_4;
        /* Mixing ratio of signal after passing horizontal/vertical filter
           to signal after passing former-stage horizontal/vertical filter */
        reg_data   |= (uint32_t)p_yc_sep_ctrl->hvsel_mix_y;

        *(vdec_reg->ycscr9) = (uint16_t)reg_data;

        /* Vertical luminance detection level for correlation detection filter */
        reg_data    = (uint32_t)*(vdec_reg->ycscr11) & (uint32_t)(~VDEC_REG_MASK_0X01FF);
        reg_data   |= (uint32_t)p_yc_sep_ctrl->v_y_level;

        *(vdec_reg->ycscr11) = (uint16_t)reg_data;

        if (p_yc_sep_ctrl->det2_on == VDEC_OFF) {
            reg_data    = ((uint32_t)VDEC_FILMIX_RATIO_0 << VEDC_REG_SHIFT_12) |
                          ((uint32_t)VDEC_FILMIX_RATIO_0 << VEDC_REG_SHIFT_8);
        } else {
            /* Mixing ratio of C signal after passing horizontal/vertical adaptive filter
               to signal after passing correlation detection filter */
            reg_data    = (uint32_t)p_yc_sep_ctrl->det2_mix_c << VEDC_REG_SHIFT_12;
            /* Mixing ratio of C signal for Y generation after passing
               horizontal/vertical adaptive filter to signal after passing correlation */
            reg_data   |= (uint32_t)p_yc_sep_ctrl->det2_mix_y << VEDC_REG_SHIFT_8;
        }
        /* Two-dimensional cascade/TAKE-OFF filter mode select */
        reg_data   |= (uint32_t)p_yc_sep_ctrl->fil2_mode_2d << VEDC_REG_SHIFT_2;
        /* Two-dimensional cascade filter select */
        reg_data   |= (p_yc_sep_ctrl->fil2_narrow_2d == VDEC_2D_FIL_SEL_BYPASS) ? (uint32_t)0x0000u :
                      (uint32_t)0x0001u;

        *(vdec_reg->ycscr12) = (uint16_t)reg_data;
    }
    return;
}   /* End of function YcSeparationControl() */

/**************************************************************************//**
 * @brief       Sets registers for chroma filter TAP coefficient
 * @param[in]   fil_reg_address         : 2D filter TAP coefficient registers
 * @param[in]   fil2_2d                 : Chroma filter TAP coefficient for Y/C separation
 * @retval      None
 *****************************************************************************/
static void FilterTAPsCoefficient (
    volatile uint16_t       * const   * fil_reg_address,
    const vdec_chrfil_tap_t * const     fil2_2d)
{
    int32_t             tap_coef;
    volatile uint16_t * fil_reg;
    const uint16_t    * taps;

    if (fil2_2d != NULL) {
        taps = fil2_2d->fil2_2d_f;
        for (tap_coef = 0; tap_coef < VDEC_CHRFIL_TAPCOEF_NUM; tap_coef++) {
            fil_reg = *fil_reg_address;
            fil_reg_address++;
            *fil_reg = *taps;
            taps++;
        }
    }
    return;
}   /* End of function FilterTAPsCoefficient() */

