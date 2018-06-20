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
* @file         r_vdc5_check_parameter.h
* @version      0.06
* $Rev: 18 $
* $Date:: 2014-05-29 11:11:38 +0900#$
* @brief        RZ/A1L VDC5 driver parameter check definitions
******************************************************************************/

#ifndef R_VDC5_CHECK_PARAMETER_H
#define R_VDC5_CHECK_PARAMETER_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_vdc5.h"
#include    "r_vdc5_user.h"


#ifdef      R_VDC5_CHECK_PARAMETERS
/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
vdc5_error_t VDC5_InitializeCheckPrm(const vdc5_channel_t ch, const vdc5_init_t * const param);
vdc5_error_t VDC5_TerminateCheckPrm(const vdc5_channel_t ch);
vdc5_error_t VDC5_VideoInputCheckPrm(const vdc5_channel_t ch, const vdc5_input_t * const param);
vdc5_error_t VDC5_SyncControlCheckPrm(const vdc5_channel_t ch, const vdc5_sync_ctrl_t * const param);
vdc5_error_t VDC5_DisplayOutputCheckPrm(const vdc5_channel_t ch, const vdc5_output_t * const param);
vdc5_error_t VDC5_CallbackISRCheckPrm(const vdc5_channel_t ch, const vdc5_int_t * const param);
vdc5_error_t VDC5_WriteDataControlCheckPrm(
    const vdc5_channel_t        ch,
    const vdc5_layer_id_t       layer_id,
    const vdc5_write_t  * const param);
vdc5_error_t VDC5_ChangeWriteProcessCheckPrm(
    const vdc5_channel_t            ch,
    const vdc5_layer_id_t           layer_id,
    const vdc5_write_chg_t  * const param);
vdc5_error_t VDC5_ReadDataControlCheckPrm(
    const vdc5_channel_t        ch,
    const vdc5_layer_id_t       layer_id,
    const vdc5_graphics_type_t  graphics_id,
    const vdc5_read_t   * const param);
vdc5_error_t VDC5_ChangeReadProcessCheckPrm(
    const vdc5_channel_t            ch,
    const vdc5_layer_id_t           layer_id,
    const vdc5_read_chg_t   * const param);
vdc5_error_t VDC5_StartProcessCheckPrm(
    const vdc5_channel_t        ch,
    const vdc5_layer_id_t       layer_id,
    const vdc5_start_t  * const param);
vdc5_error_t VDC5_StopProcessCheckPrm(const vdc5_channel_t ch, const vdc5_layer_id_t layer_id);
vdc5_error_t VDC5_ReleaseDataControlCheckPrm(const vdc5_channel_t ch, const vdc5_layer_id_t layer_id);
vdc5_error_t VDC5_VideoNoiseReductCheckPrm(const vdc5_channel_t ch, const vdc5_noise_reduction_t * const param);
vdc5_error_t VDC5_ImageColorMatrixCheckPrm(const vdc5_channel_t ch, const vdc5_color_matrix_t * const param);
vdc5_error_t VDC5_ImageEnhancementCheckPrm(
    const vdc5_channel_t                ch,
    const vdc5_imgimprv_id_t            imgimprv_id,
    const vdc5_enhance_sharp_t  * const sharp_param,
    const vdc5_period_rect_t    * const enh_area);
vdc5_error_t VDC5_ImageBlackStretchCheckPrm(
    const vdc5_channel_t        ch,
    const vdc5_imgimprv_id_t    imgimprv_id,
    const vdc5_black_t  * const param);
vdc5_error_t VDC5_AlphaBlendingCheckPrm(
    const vdc5_channel_t                ch,
    const vdc5_layer_id_t               layer_id,
    const vdc5_alpha_blending_t * const param);
vdc5_error_t VDC5_AlphaBlendingRectCheckPrm(
    const vdc5_channel_t                        ch,
    const vdc5_layer_id_t                       layer_id,
    const vdc5_alpha_blending_rect_t    * const param);
vdc5_error_t VDC5_ChromakeyCheckPrm(
    const vdc5_channel_t            ch,
    const vdc5_layer_id_t           layer_id,
    const vdc5_graphics_type_t      graphics_id,
    const vdc5_chromakey_t  * const param);
vdc5_error_t VDC5_CLUTCheckPrm(
    const vdc5_channel_t        ch,
    const vdc5_layer_id_t       layer_id,
    const vdc5_graphics_type_t  graphics_id,
    const vdc5_clut_t   * const param);
vdc5_error_t VDC5_DisplayCalibrationCheckPrm(const vdc5_channel_t ch, const vdc5_disp_calibration_t * const param);
vdc5_error_t VDC5_GammaCorrectionCheckPrm(const vdc5_channel_t ch, const vdc5_gamma_correction_t * const param);


#endif      /* R_VDC5_CHECK_PARAMETERS */

#endif  /* R_VDC5_CHECK_PARAMETER_H */

