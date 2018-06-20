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
* @file         r_vdec_check_parameter.c
* @version      1.00
* $Rev: 199 $
* $Date:: 2014-05-23 16:33:52 +0900#$
* @brief        VDEC driver parameter check processing
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_vdec.h"
#include    "r_vdec_user.h"
#include    "r_vdec_check_parameter.h"


#ifdef      R_VDEC_CHECK_PARAMETERS
/******************************************************************************
Macro definitions
******************************************************************************/
/* Valid bit range */
#define VDEC_CPARA_RANGE_0X00000001 (0x00000001u)
#define VDEC_CPARA_RANGE_0X00000003 (0x00000003u)
#define VDEC_CPARA_RANGE_0X00000007 (0x00000007u)
#define VDEC_CPARA_RANGE_0X0000000F (0x0000000Fu)
#define VDEC_CPARA_RANGE_0X0000001F (0x0000001Fu)
#define VDEC_CPARA_RANGE_0X0000003F (0x0000003Fu)
#define VDEC_CPARA_RANGE_0X0000007F (0x0000007Fu)
#define VDEC_CPARA_RANGE_0X000000FF (0x000000FFu)
#define VDEC_CPARA_RANGE_0X000001FF (0x000001FFu)
#define VDEC_CPARA_RANGE_0X000003FF (0x000003FFu)
#define VDEC_CPARA_RANGE_0X000007FF (0x000007FFu)
#define VDEC_CPARA_RANGE_0X00001FFF (0x00001FFFu)

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static vdec_error_t NoiseReductionLPFCheckParam(const vdec_noise_rd_lpf_t * const p_noise_rd_lpf);
static vdec_error_t SyncSlicerCheckParam(const vdec_sync_slicer_t * const p_sync_slicer);
static vdec_error_t HorizontalAFCCheckParam(const vdec_horizontal_afc_t * const p_horizontal_afc);
static vdec_error_t VerticalCountdownCheckParam(const vdec_vcount_down_t * const p_vcount_down);
static vdec_error_t AgcPgaCheckParam(const vdec_agc_t * const p_agc);
static vdec_error_t PeakLimiterControlCheckParam(const vdec_peak_limiter_t * const p_peak_limiter);
static vdec_error_t OverRangeControlCheckParam(const vdec_over_range_t * const p_over_range);
static vdec_error_t YcSeparationControlCheckParam(const vdec_yc_sep_ctrl_t * const p_yc_sep_ctrl);
static vdec_error_t FilterTAPsCoefficientCheckParam(const vdec_chrfil_tap_t * const fil2_2d);
static vdec_error_t ChromaDecodingControlCheckParam(const vdec_chrmdec_ctrl_t * const p_chrmdec_ctrl);
static vdec_error_t BurstLockCheckParam(const vdec_burst_lock_t * const p_burst_lock);
static vdec_error_t AccColorKillerCheckParam(const vdec_acc_t * const p_acc);


/**************************************************************************//**
 * @brief       Checks on input pin control
 * @param[in]   vinsel                  : Input pin control
 * @retval      Error code
 *****************************************************************************/
vdec_error_t VDEC_InitializeCheckParam (const vdec_adc_vinsel_t vinsel)
{
    vdec_error_t vdec_error;

    vdec_error = VDEC_OK;

    if ((vinsel != VDEC_ADC_VINSEL_VIN1) && (vinsel != VDEC_ADC_VINSEL_VIN2)) {
        vdec_error = VDEC_ERR_PARAM;
        goto END;
    }
END:
    return vdec_error;
}   /* End of function VDEC_InitializeCheckParam() */

/**************************************************************************//**
 * @brief       Checks on active image period parameter
 * @param[in]   param                   : Active image period parameter
 * @retval      Error code
 *****************************************************************************/
vdec_error_t VDEC_ActivePeriodCheckParam (const vdec_active_period_t * const param)
{
    vdec_error_t vdec_error;

    vdec_error = VDEC_OK;

    if (param == NULL) {
        vdec_error = VDEC_ERR_PARAM;
        goto END;
    }
    /* Left end of input video signal capturing area */
    if (((uint32_t)param->srcleft & (uint32_t)(~VDEC_CPARA_RANGE_0X000001FF)) != 0u) {
        vdec_error = VDEC_ERR_PARAM;
        goto END;
    }
    /* Top end of input video signal capturing area */
    if (((uint32_t)param->srctop & (uint32_t)(~VDEC_CPARA_RANGE_0X0000003F)) != 0u) {
        vdec_error = VDEC_ERR_PARAM;
        goto END;
    }
    /* Height of input video signal capturing area */
    if (((uint32_t)param->srcheight & (uint32_t)(~VDEC_CPARA_RANGE_0X000003FF)) != 0u) {
        vdec_error = VDEC_ERR_PARAM;
        goto END;
    }
    /* Width of input video signal capturing area */
    if (((uint32_t)param->srcwidth & (uint32_t)(~VDEC_CPARA_RANGE_0X000007FF)) != 0u) {
        vdec_error = VDEC_ERR_PARAM;
        goto END;
    }
END:
    return vdec_error;
}   /* End of function VDEC_ActivePeriodCheckParam() */

/**************************************************************************//**
 * @brief       Checks on sync separation parameter
 * @param[in]   param                   : Sync separation parameter
 * @retval      Error code
 *****************************************************************************/
vdec_error_t VDEC_SyncSeparationCheckParam (const vdec_sync_separation_t * const param)
{
    vdec_error_t vdec_error;

    vdec_error = VDEC_OK;

    if (param == NULL) {
        vdec_error = VDEC_ERR_PARAM;
        goto END;
    }
    /* Noise reduction LPF */
    vdec_error = NoiseReductionLPFCheckParam(param->noise_rd_lpf);
    if (vdec_error != VDEC_OK) {
        goto END;
    }
    /* Sync slicer */
    vdec_error = SyncSlicerCheckParam(param->sync_slicer);
    if (vdec_error != VDEC_OK) {
        goto END;
    }
    /* Horizontal AFC */
    vdec_error = HorizontalAFCCheckParam(param->horizontal_afc);
    if (vdec_error != VDEC_OK) {
        goto END;
    }
    /* Vertical countdown */
    vdec_error = VerticalCountdownCheckParam(param->vcount_down);
    if (vdec_error != VDEC_OK) {
        goto END;
    }
    /* AGC / PGA */
    vdec_error = AgcPgaCheckParam(param->agc);
    if (vdec_error != VDEC_OK) {
        goto END;
    }
    /* Peak limiter control */
    vdec_error = PeakLimiterControlCheckParam(param->peak_limiter);
    if (vdec_error != VDEC_OK) {
        goto END;
    }
END:
    return vdec_error;
}   /* End of function VDEC_SyncSeparationCheckParam() */

/**************************************************************************//**
 * @brief       Checks on Y/C separation parameter
 * @param[in]   param                   : Y/C separation parameter
 * @retval      Error code
 *****************************************************************************/
vdec_error_t VDEC_YcSeparationCheckParam (const vdec_yc_separation_t * const param)
{
    vdec_error_t vdec_error;

    vdec_error = VDEC_OK;

    if (param == NULL) {
        vdec_error = VDEC_ERR_PARAM;
        goto END;
    }
    /* Over-range control */
    vdec_error = OverRangeControlCheckParam(param->over_range);
    if (vdec_error != VDEC_OK) {
        goto END;
    }
    /* Y/C separation control */
    vdec_error = YcSeparationControlCheckParam(param->yc_sep_ctrl);
    if (vdec_error != VDEC_OK) {
        goto END;
    }
    /* Two-dimensional cascade broadband (3.58/4.43/SECAM-DR)/TAKE-OFF filter TAP coefficient */
    vdec_error = FilterTAPsCoefficientCheckParam(param->fil2_2d_wa);
    if (vdec_error != VDEC_OK) {
        goto END;
    }
    /* Two-dimensional cascade broadband (SECAM-DB) filter TAP coefficient */
    vdec_error = FilterTAPsCoefficientCheckParam(param->fil2_2d_wb);
    if (vdec_error != VDEC_OK) {
        goto END;
    }
    /* Two-dimensional cascade narrowband (3.58/4.43/SECAM-DR) filter TAP coefficient */
    vdec_error = FilterTAPsCoefficientCheckParam(param->fil2_2d_na);
    if (vdec_error != VDEC_OK) {
        goto END;
    }
    /* Two-dimensional cascade narrowband (SECAMDB) filter TAP coefficient */
    vdec_error = FilterTAPsCoefficientCheckParam(param->fil2_2d_nb);
    if (vdec_error != VDEC_OK) {
        goto END;
    }
END:
    return vdec_error;
}   /* End of function VDEC_YcSeparationCheckParam() */

/**************************************************************************//**
 * @brief       Checks on chroma decoding parameter
 * @param[in]   param                   : Chroma decoding parameter
 * @retval      Error code
 *****************************************************************************/
vdec_error_t VDEC_ChromaDecodingCheckParam (const vdec_chroma_decoding_t * const param)
{
    vdec_tint_ry_t    * p_tint_ry;
    vdec_error_t        vdec_error;

    vdec_error = VDEC_OK;

    if (param == NULL) {
        vdec_error = VDEC_ERR_PARAM;
        goto END;
    }
    /* Chroma decoding control */
    vdec_error = ChromaDecodingControlCheckParam(param->chrmdec_ctrl);
    if (vdec_error != VDEC_OK) {
        goto END;
    }
    /* Burst lock */
    vdec_error = BurstLockCheckParam(param->burst_lock);
    if (vdec_error != VDEC_OK) {
        goto END;
    }
    /* ACC/color killer */
    vdec_error = AccColorKillerCheckParam(param->acc);
    if (vdec_error != VDEC_OK) {
        goto END;
    }
    /* TINT/R-Y axis correction (only valid for NTSC/PAL) */
    p_tint_ry = param->tint_ry;
    if (p_tint_ry != NULL) {
        /* Fine adjustment of R-Y demodulation axis and hue adjustment level */
        if ((((uint32_t)p_tint_ry->tintsub & (uint32_t)(~VDEC_CPARA_RANGE_0X0000003F)) != 0u) ||
                (((uint32_t)p_tint_ry->tintmain & (uint32_t)(~VDEC_CPARA_RANGE_0X000003FF)) != 0u)) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
    }
END:
    return vdec_error;
}   /* End of function VDEC_ChromaDecodingCheckParam() */

/**************************************************************************//**
 * @brief       Checks on digital clamp parameter
 * @param[in]   param                   : Digital clamp parameter
 * @retval      Error code
 *****************************************************************************/
vdec_error_t VDEC_DigitalClampCheckParam (const vdec_degital_clamp_t * const param)
{
    vdec_pedestal_clamp_t     * p_pedestal_clamp;
    vdec_center_clamp_t       * p_center_clamp;
    vdec_noise_det_t          * p_noise_det;
    vdec_error_t                vdec_error;

    vdec_error = VDEC_OK;

    if (param == NULL) {
        vdec_error = VDEC_ERR_PARAM;
        goto END;
    }
    /* Digital clamp response speed */
    if (((uint32_t)param->dcpresponse & (uint32_t)(~VDEC_CPARA_RANGE_0X00000007)) != 0u) {
        vdec_error = VDEC_ERR_PARAM;
        goto END;
    }
    /* Digital clamp start line (in 1-line units) */
    if (((uint32_t)param->dcpstart & (uint32_t)(~VDEC_CPARA_RANGE_0X0000003F)) != 0u) {
        vdec_error = VDEC_ERR_PARAM;
        goto END;
    }
    /* Digital clamp end line (in 1-line units) */
    if (((uint32_t)param->dcpend & (uint32_t)(~VDEC_CPARA_RANGE_0X0000003F)) != 0u) {
        vdec_error = VDEC_ERR_PARAM;
        goto END;
    }
    /* Digital clamp pulse width */
    if (((uint32_t)param->dcpwidth & (uint32_t)(~VDEC_CPARA_RANGE_0X0000007F)) != 0u) {
        vdec_error = VDEC_ERR_PARAM;
        goto END;
    }

    /* Pedestal clamp */
    p_pedestal_clamp = param->pedestal_clamp;
    if (p_pedestal_clamp != NULL) {
        /* Clamp offset level (Y signal) */
        if (((uint32_t)p_pedestal_clamp->blanklevel_y & (uint32_t)(~VDEC_CPARA_RANGE_0X000003FF)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Digital clamp pulse horizontal start position (Y signal) */
        if (((uint32_t)p_pedestal_clamp->dcppos_y & (uint32_t)(~VDEC_CPARA_RANGE_0X000000FF)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
    }
    /* Center clamp */
    p_center_clamp = param->center_clamp;
    if (p_center_clamp != NULL) {
        /* Clamp offset level (Cb signal) */
        if (((uint32_t)p_center_clamp->blanklevel_cb & (uint32_t)(~VDEC_CPARA_RANGE_0X0000003F)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Clamp offset level (Cr signal) */
        if (((uint32_t)p_center_clamp->blanklevel_cr & (uint32_t)(~VDEC_CPARA_RANGE_0X0000003F)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Digital clamp pulse horizontal start position (Cb/Cr signal) */
        if (((uint32_t)p_center_clamp->dcppos_c & (uint32_t)(~VDEC_CPARA_RANGE_0X000000FF)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
    }
    /* Noise detection */
    p_noise_det = param->noise_det;
    if (p_noise_det != NULL) {
        /* Video signal for autocorrelation function */
        if (p_noise_det->acfinput >= VDEC_ACFINPUT_NUM) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Delay time for autocorrelation function calculation */
        if (((uint32_t)p_noise_det->acflagtime & (uint32_t)(~VDEC_CPARA_RANGE_0X0000001F)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Smoothing parameter of autocorrelation function data */
        if (((uint32_t)p_noise_det->acffilter & (uint32_t)(~VDEC_CPARA_RANGE_0X00000003)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
    }
END:
    return vdec_error;
}   /* End of function VDEC_DigitalClampCheckParam() */

/**************************************************************************//**
 * @brief       Checks on output adjustment parameter
 * @param[in]   param                   : Output adjustment parameter
 * @retval      Error code
 *****************************************************************************/
vdec_error_t VDEC_OutputCheckParam (const vdec_output_t * const param)
{
    vdec_error_t vdec_error;

    vdec_error = VDEC_OK;

    if (param == NULL) {
        vdec_error = VDEC_ERR_PARAM;
        goto END;
    }
    /* Y signal gain coefficient */
    if (((uint32_t)param->y_gain2 & (uint32_t)(~VDEC_CPARA_RANGE_0X000003FF)) != 0u) {
        vdec_error = VDEC_ERR_PARAM;
        goto END;
    }
    /* Cb signal gain coefficient */
    if (((uint32_t)param->cb_gain2 & (uint32_t)(~VDEC_CPARA_RANGE_0X000003FF)) != 0u) {
        vdec_error = VDEC_ERR_PARAM;
        goto END;
    }
    /* Cr signal gain coefficient */
    if (((uint32_t)param->cr_gain2 & (uint32_t)(~VDEC_CPARA_RANGE_0X000003FF)) != 0u) {
        vdec_error = VDEC_ERR_PARAM;
        goto END;
    }
END:
    return vdec_error;
}   /* End of function VDEC_OutputCheckParam() */

/**************************************************************************//**
 * @brief       Checks on noise reduction LPF parameter
 * @param[in]   p_noise_rd_lpf          : Noise reduction LPF parameter
 * @retval      Error code
 *****************************************************************************/
static vdec_error_t NoiseReductionLPFCheckParam (const vdec_noise_rd_lpf_t * const p_noise_rd_lpf)
{
    vdec_error_t vdec_error;

    vdec_error = VDEC_OK;

    /* Noise reduction LPF */
    if (p_noise_rd_lpf != NULL) {
        /* LPF cutoff frequency before vertical sync separation */
        if (p_noise_rd_lpf->lpfvsync >= VDEC_LPF_VSYNC_NUM) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* LPF cutoff frequency before horizontal sync separation */
        if (p_noise_rd_lpf->lpfhsync >= VDEC_LPF_HSYNC_NUM) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
    }
END:
    return vdec_error;
}   /* End of function NoiseReductionLPFCheckParam() */

/**************************************************************************//**
 * @brief       Checks on auto level control sync slicer parameter
 * @param[in]   p_sync_slicer           : Auto level control sync slicer parameter
 * @retval      Error code
 *****************************************************************************/
static vdec_error_t SyncSlicerCheckParam (const vdec_sync_slicer_t * const p_sync_slicer)
{
    vdec_error_t vdec_error;

    vdec_error = VDEC_OK;

    /* Sync slicer */
    if (p_sync_slicer != NULL) {
        /* Reference level operation speed control for composite sync separation (for Hsync signal) */
        if (p_sync_slicer->velocityshift_h >= VDEC_VELOCITY_SHIFT_NUM) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Auto-slice level setting for composite sync separator circuit (for Hsync signal) */
        if (p_sync_slicer->slicermode_h >= VDEC_SLICE_MODE_NUM) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Auto-slice level setting for composite sync separation circuit (for Vsync signal) */
        if (p_sync_slicer->slicermode_v >= VDEC_SLICE_MODE_NUM) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Max ratio of horizontal cycle to horizontal sync signal pulse width
           and min ratio of horizontal cycle to horizontal sync signal pulse width (for Hsync signal) */
        if ((((uint32_t)p_sync_slicer->syncmaxduty_h & (uint32_t)(~VDEC_CPARA_RANGE_0X0000003F)) != 0u) ||
                (((uint32_t)p_sync_slicer->syncminduty_h & (uint32_t)(~VDEC_CPARA_RANGE_0X0000003F)) != 0u)) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Clipping level */
        if (p_sync_slicer->ssclipsel >= VDEC_CLIP_LV_NUM) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Slice level for composite sync signal separation (for Hsync signal) */
        if (((uint32_t)p_sync_slicer->csyncslice_h & (uint32_t)(~VDEC_CPARA_RANGE_0X000003FF)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Max ratio of horizontal cycle to horizontal sync signal pulse width
           and min ratio of horizontal cycle to horizontal sync signal pulse width (for Vsync signal) */
        if ((((uint32_t)p_sync_slicer->syncmaxduty_v & (uint32_t)(~VDEC_CPARA_RANGE_0X0000003F)) != 0u) ||
                (((uint32_t)p_sync_slicer->syncminduty_v & (uint32_t)(~VDEC_CPARA_RANGE_0X0000003F)) != 0u)) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Threshold for vertical sync separation */
        if (((uint32_t)p_sync_slicer->vsyncslice & (uint32_t)(~VDEC_CPARA_RANGE_0X0000001F)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Slice level for composite sync signal separation (for Vsync signal) */
        if (((uint32_t)p_sync_slicer->csyncslice_v & (uint32_t)(~VDEC_CPARA_RANGE_0X000003FF)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
    }
END:
    return vdec_error;
}   /* End of function SyncSlicerCheckParam() */

/**************************************************************************//**
 * @brief       Checks on horizontal AFC parameter
 * @param[in]   p_horizontal_afc        : Horizontal AFC parameter
 * @retval      Error code
 *****************************************************************************/
static vdec_error_t HorizontalAFCCheckParam (const vdec_horizontal_afc_t * const p_horizontal_afc)
{
    vdec_error_t vdec_error;

    vdec_error = VDEC_OK;

    /* Horizontal AFC */
    if (p_horizontal_afc != NULL) {
        /* Horizontal AFC loop gain */
        if (((uint32_t)p_horizontal_afc->hafcgain & (uint32_t)(~VDEC_CPARA_RANGE_0X0000000F)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Horizontal AFC center oscillation frequency */
        if (((uint32_t)p_horizontal_afc->hafcgain & (uint32_t)(~VDEC_CPARA_RANGE_0X000003FF)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Start line of horizontal AFC normal operation */
        if (((uint32_t)p_horizontal_afc->hafcstart & (uint32_t)(~VDEC_CPARA_RANGE_0X0000000F)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Maximum oscillation frequency of horizontal AFC */
        if (((uint32_t)p_horizontal_afc->hafcmax & (uint32_t)(~VDEC_CPARA_RANGE_0X000003FF)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* End line of horizontal AFC normal operation */
        if (((uint32_t)p_horizontal_afc->hafcend & (uint32_t)(~VDEC_CPARA_RANGE_0X0000000F)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Horizontal AFC VBI period operating mode */
        if (p_horizontal_afc->hafcmode >= VDEC_HAFCMD_NUM) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Min oscillation frequency of horizontal AFC */
        if (((uint32_t)p_horizontal_afc->hafcmin & (uint32_t)(~VDEC_CPARA_RANGE_0X000003FF)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Phase comparator feedback adjust for low sync signal lock stability */
        if (p_horizontal_afc->phdet_div >= VDEC_PHDET_DIV_NUM) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
    }
END:
    return vdec_error;
}   /* End of function HorizontalAFCCheckParam() */

/**************************************************************************//**
 * @brief       Checks on vertical count-down parameter
 * @param[in]   p_vcount_down           : Vertical count-down parameter
 * @retval      Error code
 *****************************************************************************/
static vdec_error_t VerticalCountdownCheckParam (const vdec_vcount_down_t * const p_vcount_down)
{
    vdec_error_t vdec_error;

    vdec_error = VDEC_OK;

    /* Vertical countdown */
    if (p_vcount_down != NULL) {
        /* Vertical countdown center oscillation frequency */
        if (p_vcount_down->vcddefault >= VDEC_VCD_FRQ_NUM) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Vertical countdown sync area */
        if (((uint32_t)p_vcount_down->vcdwindow & (uint32_t)(~VDEC_CPARA_RANGE_0X0000003F)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Vertical countdown minimum oscillation frequency */
        if (((uint32_t)p_vcount_down->vcdoffset & (uint32_t)(~VDEC_CPARA_RANGE_0X0000001F)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
    }
END:
    return vdec_error;
}   /* End of function VerticalCountdownCheckParam() */

/**************************************************************************//**
 * @brief       Checks on AGC/PGA parameter
 * @param[in]   p_agc                   : AGC/PGA parameter
 * @retval      Error code
 *****************************************************************************/
static vdec_error_t AgcPgaCheckParam (const vdec_agc_t * const p_agc)
{
    vdec_error_t vdec_error;

    vdec_error = VDEC_OK;

    /* AGC / PGA */
    if (p_agc != NULL) {
        /* AGC response speed */
        if (((uint32_t)p_agc->agcresponse & (uint32_t)(~VDEC_CPARA_RANGE_0X00000007)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Sync signal reference amplitude */
        if (((uint32_t)p_agc->agclevel & (uint32_t)(~VDEC_CPARA_RANGE_0X000001FF)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* AGC gain adjustment accuracy */
        if (((uint32_t)p_agc->agcprecis & (uint32_t)(~VDEC_CPARA_RANGE_0X0000003F)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* PGA gain */
        if (((uint32_t)p_agc->pga_gain & (uint32_t)(~VDEC_CPARA_RANGE_0X0000003F)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
    }
END:
    return vdec_error;
}   /* End of function AgcPgaCheckParam() */

/**************************************************************************//**
 * @brief       Checks on peak limiter control parameter
 * @param[in]   p_peak_limiter          : Peak limiter control parameter
 * @retval      Error code
 *****************************************************************************/
static vdec_error_t PeakLimiterControlCheckParam (const vdec_peak_limiter_t * const p_peak_limiter)
{
    vdec_error_t vdec_error;

    vdec_error = VDEC_OK;

    /* Peak limiter control */
    if (p_peak_limiter != NULL) {
        /* Peak luminance value to operate peak limiter */
        if (p_peak_limiter->peaklevel >= VDEC_PEAKLV_NUM) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }

        /* Response speed with peak limiter gain decreased */
        if (((uint32_t)p_peak_limiter->peakattack & (uint32_t)(~VDEC_CPARA_RANGE_0X00000003)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Response speed with peak limiter gain increased */
        if (((uint32_t)p_peak_limiter->peakrelease & (uint32_t)(~VDEC_CPARA_RANGE_0X00000003)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Maximum compression rate of peak limiter */
        if (p_peak_limiter->peakratio >= VDEC_PEAKRATIO_NUM) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Allowable number of overflowing pixels */
        if (((uint32_t)p_peak_limiter->maxpeaksamples & (uint32_t)(~VDEC_CPARA_RANGE_0X000000FF)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
    }
END:
    return vdec_error;
}   /* End of function PeakLimiterControlCheckParam() */

/**************************************************************************//**
 * @brief       Checks on over-range control parameter
 * @param[in]   p_over_range            : Over-range control parameter
 * @retval      Error code
 *****************************************************************************/
static vdec_error_t OverRangeControlCheckParam (const vdec_over_range_t * const p_over_range)
{
    vdec_error_t vdec_error;

    vdec_error = VDEC_OK;

    /* Over-range control */
    if (p_over_range != NULL) {
        /* A/D over-threshold level (between levels 0 and 1) */
        if (((uint32_t)p_over_range->radj_o_level0 & (uint32_t)(~VDEC_CPARA_RANGE_0X000003FF)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* A/D under-threshold level (between levels 2 and 3) */
        if (((uint32_t)p_over_range->radj_u_level0 & (uint32_t)(~VDEC_CPARA_RANGE_0X000003FF)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* A/D over-threshold level (between levels 1 and 2) */
        if (((uint32_t)p_over_range->radj_o_level1 & (uint32_t)(~VDEC_CPARA_RANGE_0X000003FF)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* A/D under-threshold level (between levels 1 and 2) */
        if (((uint32_t)p_over_range->radj_u_level1 & (uint32_t)(~VDEC_CPARA_RANGE_0X000003FF)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* A/D over-threshold level (between levels 2 and 3) */
        if (((uint32_t)p_over_range->radj_o_level2 & (uint32_t)(~VDEC_CPARA_RANGE_0X000003FF)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* A/D under-threshold level (between levels 0 and 1) */
        if (((uint32_t)p_over_range->radj_u_level2 & (uint32_t)(~VDEC_CPARA_RANGE_0X000003FF)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
    }
END:
    return vdec_error;
}   /* End of function OverRangeControlCheckParam() */

/**************************************************************************//**
 * @brief       Checks on Y/C separation control parameter
 * @param[in]   p_yc_sep_ctrl           : Y/C separation control parameter
 * @retval      Error code
 *****************************************************************************/
static vdec_error_t YcSeparationControlCheckParam (const vdec_yc_sep_ctrl_t * const p_yc_sep_ctrl)
{
    vdec_error_t vdec_error;

    vdec_error = VDEC_OK;

    /* Y/C separation control */
    if (p_yc_sep_ctrl != NULL) {
        /* Two-dimensional Y/C separation filter select coefficient (K15, K13, and K11) */
        if ((((uint32_t)p_yc_sep_ctrl->k15 & (uint32_t)(~VDEC_CPARA_RANGE_0X0000000F)) != 0u) ||
                (((uint32_t)p_yc_sep_ctrl->k13 & (uint32_t)(~VDEC_CPARA_RANGE_0X0000003F)) != 0u) ||
                (((uint32_t)p_yc_sep_ctrl->k11 & (uint32_t)(~VDEC_CPARA_RANGE_0X0000003F)) != 0u)) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Two-dimensional Y/C separation filter select coefficient (K16, K14, and K12) */
        if ((((uint32_t)p_yc_sep_ctrl->k16 & (uint32_t)(~VDEC_CPARA_RANGE_0X0000000F)) != 0u) ||
                (((uint32_t)p_yc_sep_ctrl->k14 & (uint32_t)(~VDEC_CPARA_RANGE_0X0000003F)) != 0u) ||
                (((uint32_t)p_yc_sep_ctrl->k12 & (uint32_t)(~VDEC_CPARA_RANGE_0X0000003F)) != 0u)) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Two-dimensional Y/C separation filter select coefficient (K22A and K21A) */
        if ((((uint32_t)p_yc_sep_ctrl->k22a & (uint32_t)(~VDEC_CPARA_RANGE_0X000000FF)) != 0u) ||
                (((uint32_t)p_yc_sep_ctrl->k21a & (uint32_t)(~VDEC_CPARA_RANGE_0X0000003F)) != 0u)) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Two-dimensional Y/C separation filter select coefficient (K22B and K21B) */
        if ((((uint32_t)p_yc_sep_ctrl->k22b & (uint32_t)(~VDEC_CPARA_RANGE_0X000000FF)) != 0u) ||
                (((uint32_t)p_yc_sep_ctrl->k21b & (uint32_t)(~VDEC_CPARA_RANGE_0X0000003F)) != 0u)) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Two-dimensional Y/C separation filter select coefficient (K23B, K23A, and K24) */
        if ((((uint32_t)p_yc_sep_ctrl->k23b & (uint32_t)(~VDEC_CPARA_RANGE_0X0000000F)) != 0u) ||
                (((uint32_t)p_yc_sep_ctrl->k23a & (uint32_t)(~VDEC_CPARA_RANGE_0X0000000F)) != 0u) ||
                (((uint32_t)p_yc_sep_ctrl->k24 & (uint32_t)(~VDEC_CPARA_RANGE_0X0000001F)) != 0u)) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Mixing ratio of signal after passing horizontal filter
          to signal after passing former-stage horizontal filter */
        if (p_yc_sep_ctrl->hsel_mix_y >= VDEC_FILMIX_RATIO_NUM) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Mixing ratio of signal after passing vertical filter
           to signal after passing former-stage horizontal/vertical filter */
        if (p_yc_sep_ctrl->vsel_mix_y >= VDEC_FILMIX_RATIO_NUM) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Mixing ratio of signal after passing horizontal/vertical filter
           to signal after passing former-stage horizontal/vertical filter */
        if (p_yc_sep_ctrl->hvsel_mix_y >= VDEC_FILMIX_RATIO_NUM) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Vertical luminance detection level for correlation detection filter */
        if (((uint32_t)p_yc_sep_ctrl->v_y_level & (uint32_t)(~VDEC_CPARA_RANGE_0X000001FF)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }

        /* Mixing ratio of c signal after passing horizontal/vertical adaptive filter
           to signal after passing correlation detection filter */
        if (p_yc_sep_ctrl->det2_mix_c >= VDEC_FILMIX_RATIO_NUM) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Mixing ratio of C signal for Y generation after passing
           horizontal/vertical adaptive filter to signal after passing correlation */
        if (p_yc_sep_ctrl->det2_mix_y >= VDEC_FILMIX_RATIO_NUM) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Two-dimensional cascade/TAKE-OFF filter mode select */
        if (p_yc_sep_ctrl->fil2_mode_2d >= VDEC_2DFIL_MDSEL_NUM) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
    }
END:
    return vdec_error;
}   /* End of function YcSeparationControlCheckParam() */

/**************************************************************************//**
 * @brief       Checks on chroma filter TAP coefficient for Y/C separation
 * @param[in]   fil2_2d                 : Chroma filter TAP coefficient for Y/C separation
 * @retval      Error code
 *****************************************************************************/
static vdec_error_t FilterTAPsCoefficientCheckParam (const vdec_chrfil_tap_t * const fil2_2d)
{
    int32_t             tap_coef;
    const uint16_t    * taps;
    vdec_error_t        vdec_error;

    vdec_error = VDEC_OK;

    if (fil2_2d != NULL) {
        taps = fil2_2d->fil2_2d_f;
        for (tap_coef = 0; tap_coef < VDEC_CHRFIL_TAPCOEF_NUM; tap_coef++) {
            if (((uint32_t)*taps & (uint32_t)(~VDEC_CPARA_RANGE_0X00001FFF)) != 0u) {
                vdec_error = VDEC_ERR_PARAM;
                goto END;
            }
            taps++;
        }
    }
END:
    return vdec_error;
}   /* End of function FilterTAPsCoefficientCheckParam() */

/**************************************************************************//**
 * @brief       Checks on color system detection parameter
 * @param[in]   p_chrmdec_ctrl          : Color system detection parameter
 * @retval      Error code
 *****************************************************************************/
static vdec_error_t ChromaDecodingControlCheckParam (const vdec_chrmdec_ctrl_t * const p_chrmdec_ctrl)
{
    vdec_error_t vdec_error;

    vdec_error = VDEC_OK;

    /* Chroma decoding control */
    if (p_chrmdec_ctrl != NULL) {
        /* Default color system */
        if (p_chrmdec_ctrl->defaultsys >= VDEC_COL_SYS_NUM) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Luminance signal delay adjustment */
        if (((uint32_t)p_chrmdec_ctrl->lumadelay & (uint32_t)(~VDEC_CPARA_RANGE_0X0000001F)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Averaging processing for pre-demodulated line */
        if ((p_chrmdec_ctrl->demodmode != VDEC_DEMOD_MD_NO) && (p_chrmdec_ctrl->demodmode != VDEC_DEMOD_MD_PAL)) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
    }
END:
    return vdec_error;
}   /* End of function ChromaDecodingControlCheckParam() */

/**************************************************************************//**
 * @brief       Checks on BCO parameter
 * @param[in]   p_burst_lock            : BCO parameter
 * @retval      Error code
 *****************************************************************************/
static vdec_error_t BurstLockCheckParam (const vdec_burst_lock_t * const p_burst_lock)
{
    vdec_error_t vdec_error;

    vdec_error = VDEC_OK;

    /* Burst lock */
    if (p_burst_lock != NULL) {
        /* Burst lock PLL lock range */
        if (p_burst_lock->lockrange >= VDEC_LOCK_RANGE_NUM) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Burst lock PLL loop gain */
        if (((uint32_t)p_burst_lock->loopgain & (uint32_t)(~VDEC_CPARA_RANGE_0X00000003)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Level for burst lock PLL to re-search free-run frequency */
        if (((uint32_t)p_burst_lock->locklimit & (uint32_t)(~VDEC_CPARA_RANGE_0X00000003)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* burst gate pulse position check */
        if (((uint32_t)p_burst_lock->bgpcheck & (uint32_t)(~VDEC_CPARA_RANGE_0X00000001)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* burst gate pulse width */
        if (((uint32_t)p_burst_lock->bgpwidth & (uint32_t)(~VDEC_CPARA_RANGE_0X0000007F)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* burst gate pulse start position */
        if (((uint32_t)p_burst_lock->bgpstart & (uint32_t)(~VDEC_CPARA_RANGE_0X000000FF)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
    }
END:
    return vdec_error;
}   /* End of function BurstLockCheckParam() */

/**************************************************************************//**
 * @brief       Checks on ACC and color killer parameter
 * @param[in]   p_acc                   : ACC and color killer parameter
 * @retval      Error code
 *****************************************************************************/
static vdec_error_t AccColorKillerCheckParam (const vdec_acc_t * const p_acc)
{
    vdec_error_t vdec_error;

    vdec_error = VDEC_OK;

    /* ACC/color killer */
    if (p_acc != NULL) {
        /* Maximum ACC Gain */
        if (p_acc->accmaxgain >= VDEC_ACC_MAX_GAIN_NUM) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* ACC reference color burst amplitude */
        if (((uint32_t)p_acc->acclevel & (uint32_t)(~VDEC_CPARA_RANGE_0X000001FF)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Chroma manual gain (sub) */
        if (p_acc->chromasubgain >= VDEC_CHRM_SB_GAIN_NUM) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Chroma manual gain (main) */
        if (((uint32_t)p_acc->chromamaingain & (uint32_t)(~VDEC_CPARA_RANGE_0X000001FF)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* ACC response speed */
        if (((uint32_t)p_acc->accresponse & (uint32_t)(~VDEC_CPARA_RANGE_0X00000003)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* ACC gain adjustment accuracy */
        if (((uint32_t)p_acc->accprecis & (uint32_t)(~VDEC_CPARA_RANGE_0X0000003F)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* Color killer operation start point */
        if (((uint32_t)p_acc->killerlevel & (uint32_t)(~VDEC_CPARA_RANGE_0X0000003F)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
        /* The offset level to turn off the color killer */
        if (((uint32_t)p_acc->killeroffset & (uint32_t)(~VDEC_CPARA_RANGE_0X0000000F)) != 0u) {
            vdec_error = VDEC_ERR_PARAM;
            goto END;
        }
    }
END:
    return vdec_error;
}   /* End of function AccColorKillerCheckParam() */

#endif      /* R_VDEC_CHECK_PARAMETERS */

