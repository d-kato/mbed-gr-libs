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
* @file         r_vdc_check_parameter.h
* @version      0.01
* $Rev: 18 $
* $Date:: 2014-05-29 11:11:38 +0900#$
* @brief        RZ/A2M VDC driver parameter check definitions
******************************************************************************/

#ifndef R_VDC_CHECK_PARAMETER_H
#define R_VDC_CHECK_PARAMETER_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_vdc.h"
#include "r_vdc_user.h"


#ifdef      R_VDC_CHECK_PARAMETERS
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
vdc_error_t VDC_InitializeCheckPrm(const vdc_channel_t ch, const vdc_init_t * const param);
vdc_error_t VDC_TerminateCheckPrm(const vdc_channel_t ch);
vdc_error_t VDC_VideoInputCheckPrm(const vdc_channel_t ch, const vdc_input_t * const param);
vdc_error_t VDC_SyncControlCheckPrm(const vdc_channel_t ch, const vdc_sync_ctrl_t * const param);
vdc_error_t VDC_DisplayOutputCheckPrm(const vdc_channel_t ch, const vdc_output_t * const param);
vdc_error_t VDC_CallbackISRCheckPrm(const vdc_channel_t ch, const vdc_int_t * const param);
vdc_error_t VDC_WriteDataControlCheckPrm(
    const vdc_channel_t        ch,
    const vdc_layer_id_t       layer_id,
    const vdc_write_t  * const param);
vdc_error_t VDC_ChangeWriteProcessCheckPrm(
    const vdc_channel_t            ch,
    const vdc_layer_id_t           layer_id,
    const vdc_write_chg_t  * const param);
vdc_error_t VDC_ReadDataControlCheckPrm(
    const vdc_channel_t        ch,
    const vdc_layer_id_t       layer_id,
    const vdc_graphics_type_t  graphics_id,
    const vdc_read_t   * const param);
vdc_error_t VDC_ChangeReadProcessCheckPrm(
    const vdc_channel_t            ch,
    const vdc_layer_id_t           layer_id,
    const vdc_read_chg_t   * const param);
vdc_error_t VDC_StartProcessCheckPrm(
    const vdc_channel_t        ch,
    const vdc_layer_id_t       layer_id,
    const vdc_start_t  * const param);
vdc_error_t VDC_StopProcessCheckPrm(const vdc_channel_t ch, const vdc_layer_id_t layer_id);
vdc_error_t VDC_ReleaseDataControlCheckPrm(const vdc_channel_t ch, const vdc_layer_id_t layer_id);
vdc_error_t VDC_VideoNoiseReductCheckPrm(const vdc_channel_t ch, const vdc_noise_reduction_t * const param);
vdc_error_t VDC_ImageColorMatrixCheckPrm(const vdc_channel_t ch, const vdc_color_matrix_t * const param);
vdc_error_t VDC_ImageEnhancementCheckPrm(
    const vdc_channel_t                ch,
    const vdc_imgimprv_id_t            imgimprv_id,
    const vdc_enhance_sharp_t  * const sharp_param,
    const vdc_period_rect_t    * const enh_area);
vdc_error_t VDC_ImageBlackStretchCheckPrm(
    const vdc_channel_t        ch,
    const vdc_imgimprv_id_t    imgimprv_id,
    const vdc_black_t  * const param);
vdc_error_t VDC_AlphaBlendingCheckPrm(
    const vdc_channel_t                ch,
    const vdc_layer_id_t               layer_id,
    const vdc_alpha_blending_t * const param);
vdc_error_t VDC_AlphaBlendingRectCheckPrm(
    const vdc_channel_t                        ch,
    const vdc_layer_id_t                       layer_id,
    const vdc_alpha_blending_rect_t    * const param);
vdc_error_t VDC_ChromakeyCheckPrm(
    const vdc_channel_t            ch,
    const vdc_layer_id_t           layer_id,
    const vdc_graphics_type_t      graphics_id,
    const vdc_chromakey_t  * const param);
vdc_error_t VDC_CLUTCheckPrm(
    const vdc_channel_t        ch,
    const vdc_layer_id_t       layer_id,
    const vdc_graphics_type_t  graphics_id,
    const vdc_clut_t   * const param);
vdc_error_t VDC_DisplayCalibrationCheckPrm(const vdc_channel_t ch, const vdc_disp_calibration_t * const param);
vdc_error_t VDC_GammaCorrectionCheckPrm(const vdc_channel_t ch, const vdc_gamma_correction_t * const param);


#endif      /* R_VDC_CHECK_PARAMETERS */

#endif  /* R_VDC_CHECK_PARAMETER_H */

