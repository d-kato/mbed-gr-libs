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
* @file         r_vdec.c
* @version      1.00
* $Rev: 199 $
* $Date:: 2014-05-23 16:33:52 +0900#$
* @brief        VDEC driver API function
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_vdec.h"
#include    "r_vdec_user.h"
#include    "r_vdec_register.h"
#include    "r_vdec_check_parameter.h"


/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

/**************************************************************************//**
 * @brief       VDEC driver initialization
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Calls the user-defined function specified in init_func.
 *              - Sets up the input pins.
 * @param[in]   ch                      : Channel
 * @param[in]   vinsel                  : Input pin control
 *                                        - VDEC_ADC_VINSEL_VIN1
 *                                        - VDEC_ADC_VINSEL_VIN2
 * @param[in]   init_func               : Pointer to a user-defined function
 * @param[in]   user_num                : User defined number
 * @retval      Error code
 *****************************************************************************/
vdec_error_t R_VDEC_Initialize (
    const vdec_channel_t    ch,
    const vdec_adc_vinsel_t vinsel,
    void           (* const init_func)(uint32_t),
    const uint32_t          user_num)
{
    vdec_error_t vdec_error;

    vdec_error = VDEC_OK;
#ifdef      R_VDEC_CHECK_PARAMETERS
    if ((ch != VDEC_CHANNEL_0) && (ch != VDEC_CHANNEL_1)) {
        vdec_error = VDEC_ERR_CHANNEL;      /* Channel error */
    }
    if (vdec_error == VDEC_OK) {
        vdec_error = VDEC_InitializeCheckParam(vinsel);
    }
#endif      /* R_VDEC_CHECK_PARAMETERS */
    if (vdec_error == VDEC_OK) {
        /* User-defined function */
        if (init_func != 0) {
            init_func(user_num);
        }
        VDEC_Initialize(ch, vinsel);
    }
    return vdec_error;
}   /* End of function R_VDEC_Initialize() */

/**************************************************************************//**
 * @brief       VDEC driver termination
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Calls the user-defined function specified in quit_func.
 * @param[in]   ch                      : Channel
 * @param[in]   quit_func               : Pointer to a user-defined function
 * @param[in]   user_num                : User defined number
 * @retval      Error code
 *****************************************************************************/
vdec_error_t R_VDEC_Terminate (const vdec_channel_t ch, void (* const quit_func)(uint32_t), const uint32_t user_num)
{
    vdec_error_t vdec_error;

    vdec_error = VDEC_OK;
#ifdef      R_VDEC_CHECK_PARAMETERS
    if ((ch != VDEC_CHANNEL_0) && (ch != VDEC_CHANNEL_1)) {
        vdec_error = VDEC_ERR_CHANNEL;      /* Channel error */
    }
#endif      /* R_VDEC_CHECK_PARAMETERS */
    if (vdec_error == VDEC_OK) {
        /* User-defined function */
        if (quit_func != 0) {
            quit_func(user_num);
        }
    }
    return vdec_error;
}   /* End of function R_VDEC_Terminate() */

/**************************************************************************//**
 * @brief       Active image period setup
 *
 *              Description:<br>
 *              This function configures the active image period.
 * @param[in]   ch                      : Channel
 * @param[in]   param                   : Active image period parameter
 * @retval      Error code
 *****************************************************************************/
vdec_error_t R_VDEC_ActivePeriod (const vdec_channel_t ch, const vdec_active_period_t * const param)
{
    vdec_error_t vdec_error;

    vdec_error = VDEC_OK;
#ifdef      R_VDEC_CHECK_PARAMETERS
    if ((ch != VDEC_CHANNEL_0) && (ch != VDEC_CHANNEL_1)) {
        vdec_error = VDEC_ERR_CHANNEL;      /* Channel error */
    }
    if (vdec_error == VDEC_OK) {
        vdec_error = VDEC_ActivePeriodCheckParam(param);
    }
#endif      /* R_VDEC_CHECK_PARAMETERS */
    if (vdec_error == VDEC_OK) {
        VDEC_ActivePeriod(ch, param);
    }
    return vdec_error;
}   /* End of function R_VDEC_ActivePeriod() */

/**************************************************************************//**
 * @brief       Sync separation processing
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Configures the noise reduction LPF.
 *              - Configures the auto level control sync slicer.
 *              - Configures the horizontal AFC.
 *              - Configures the vertical count-down.
 *              - Configures the AGC.
 *              - Configures the peak limiter control.
 * @param[in]   ch                      : Channel
 * @param[in]   param                   : Sync separation parameter
 * @retval      Error code
 *****************************************************************************/
vdec_error_t R_VDEC_SyncSeparation (const vdec_channel_t ch, const vdec_sync_separation_t * const param)
{
    vdec_error_t vdec_error;

    vdec_error = VDEC_OK;
#ifdef      R_VDEC_CHECK_PARAMETERS
    if ((ch != VDEC_CHANNEL_0) && (ch != VDEC_CHANNEL_1)) {
        vdec_error = VDEC_ERR_CHANNEL;      /* Channel error */
    }
    if (vdec_error == VDEC_OK) {
        vdec_error = VDEC_SyncSeparationCheckParam(param);
    }
#endif      /* R_VDEC_CHECK_PARAMETERS */
    if (vdec_error == VDEC_OK) {
        VDEC_SyncSeparation(ch, param);
    }
    return vdec_error;
}   /* End of function R_VDEC_SyncSeparation() */

/**************************************************************************//**
 * @brief       Y/C separation processing
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Configures the over-range control.
 *              - Configures the Y/C separation control.
 *              - Configures the chroma filter TAP coefficients for Y/C separation.
 * @param[in]   ch                      : Channel
 * @param[in]   param                   : Y/C separation parameter
 * @retval      Error code
 *****************************************************************************/
vdec_error_t R_VDEC_YcSeparation (const vdec_channel_t ch, const vdec_yc_separation_t * const param)
{
    vdec_error_t vdec_error;

    vdec_error = VDEC_OK;
#ifdef      R_VDEC_CHECK_PARAMETERS
    if ((ch != VDEC_CHANNEL_0) && (ch != VDEC_CHANNEL_1)) {
        vdec_error = VDEC_ERR_CHANNEL;      /* Channel error */
    }
    if (vdec_error == VDEC_OK) {
        vdec_error = VDEC_YcSeparationCheckParam(param);
    }
#endif      /* R_VDEC_CHECK_PARAMETERS */
    if (vdec_error == VDEC_OK) {
        VDEC_YcSeparation(ch, param);
    }
    return vdec_error;
}   /* End of function R_VDEC_YcSeparation() */

/**************************************************************************//**
 * @brief       Chroma decoding processing
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Configures the color system detection.
 *              - Configures the BCO.
 *              - Configures the ACC/color killer.
 *              - Configures the TINT correction/R-Y axis correction.
 * @param[in]   ch                      : Channel
 * @param[in]   param                   : Chroma decoding parameter
 * @retval      Error code
 *****************************************************************************/
vdec_error_t R_VDEC_ChromaDecoding (const vdec_channel_t ch, const vdec_chroma_decoding_t * const param)
{
    vdec_error_t vdec_error;

    vdec_error = VDEC_OK;
#ifdef      R_VDEC_CHECK_PARAMETERS
    if ((ch != VDEC_CHANNEL_0) && (ch != VDEC_CHANNEL_1)) {
        vdec_error = VDEC_ERR_CHANNEL;      /* Channel error */
    }
    if (vdec_error == VDEC_OK) {
        vdec_error = VDEC_ChromaDecodingCheckParam(param);
    }
#endif      /* R_VDEC_CHECK_PARAMETERS */
    if (vdec_error == VDEC_OK) {
        VDEC_ChromaDecoding(ch, param);
    }
    return vdec_error;
}   /* End of function R_VDEC_ChromaDecoding() */

/**************************************************************************//**
 * @brief       Digital clamp processing
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Configures the digital clamp control.
 *              - Configures the center clamp.
 *              - Configures the pedestal clamp.
 *              - Configures the noise detection control.
 * @param[in]   ch                      : Channel
 * @param[in]   param                   : Digital clamp parameter
 * @retval      Error code
 *****************************************************************************/
vdec_error_t R_VDEC_DigitalClamp (const vdec_channel_t ch, const vdec_degital_clamp_t * const param)
{
    vdec_error_t vdec_error;

    vdec_error = VDEC_OK;
#ifdef      R_VDEC_CHECK_PARAMETERS
    if ((ch != VDEC_CHANNEL_0) && (ch != VDEC_CHANNEL_1)) {
        vdec_error = VDEC_ERR_CHANNEL;      /* Channel error */
    }
    if (vdec_error == VDEC_OK) {
        vdec_error = VDEC_DigitalClampCheckParam(param);
    }
#endif      /* R_VDEC_CHECK_PARAMETERS */
    if (vdec_error == VDEC_OK) {
        VDEC_DigitalClamp(ch, param);
    }
    return vdec_error;
}   /* End of function R_VDEC_DigitalClamp() */

/**************************************************************************//**
 * @brief       Output adjustment processing
 *
 *              Description:<br>
 *              This function makes settings for output adjustment.
 * @param[in]   ch                      : Channel
 * @param[in]   param                   : Output adjustment parameter
 * @retval      Error code
 *****************************************************************************/
vdec_error_t R_VDEC_Output (const vdec_channel_t ch, const vdec_output_t * const param)
{
    vdec_error_t vdec_error;

    vdec_error = VDEC_OK;
#ifdef      R_VDEC_CHECK_PARAMETERS
    if ((ch != VDEC_CHANNEL_0) && (ch != VDEC_CHANNEL_1)) {
        vdec_error = VDEC_ERR_CHANNEL;      /* Channel error */
    }
    if (vdec_error == VDEC_OK) {
        vdec_error = VDEC_OutputCheckParam(param);
    }
#endif      /* R_VDEC_CHECK_PARAMETERS */
    if (vdec_error == VDEC_OK) {
        VDEC_Output(ch, param);
    }
    return vdec_error;
}   /* End of function R_VDEC_Output() */

/**************************************************************************//**
 * @brief       VDEC information acquisition processing
 *
 *              Description:<br>
 *              This gets the parameters of the VDEC modules. The parameters that can be obtained are listed below.
 *              - Sync separation parameters
 *              - AGC parameters
 *              - Chroma decoding parameters
 *              - Digital clamp parameters
 * @param[in]   ch                      : Channel
 * @param[out]  q_sync_sep              : Pointer to the place where the sync separation parameters are stored.
 * @param[out]  q_agc                   : Pointer to the place where the AGC parameters are stored.
 * @param[out]  q_chroma_dec            : Pointer to the place where the chroma decoding parameters are stored.
 * @param[out]  q_digital_clamp         : Pointer to the place where the digital clamp parameters are stored.
 * @retval      Error code
 *****************************************************************************/
vdec_error_t R_VDEC_Query (
    const vdec_channel_t            ch,
    vdec_q_sync_sep_t       * const q_sync_sep,
    vdec_q_agc_t            * const q_agc,
    vdec_q_chroma_dec_t     * const q_chroma_dec,
    vdec_q_digital_clamp_t  * const q_digital_clamp)
{
    vdec_error_t vdec_error;

    vdec_error = VDEC_OK;
#ifdef      R_VDEC_CHECK_PARAMETERS
    if ((ch != VDEC_CHANNEL_0) && (ch != VDEC_CHANNEL_1)) {
        vdec_error = VDEC_ERR_CHANNEL;      /* Channel error */
    }
#endif      /* R_VDEC_CHECK_PARAMETERS */
    if (vdec_error == VDEC_OK) {
        VDEC_Query(ch, q_sync_sep, q_agc, q_chroma_dec, q_digital_clamp);
    }
    return vdec_error;
}   /* End of function R_VDEC_Query() */

