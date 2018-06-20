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
* @file         r_vdec_check_parameter.h
* @version      1.00
* $Rev: 199 $
* $Date:: 2014-05-23 16:33:52 +0900#$
* @brief        VDEC driver parameter check definitions
******************************************************************************/

#ifndef R_VDEC_CHECK_PARAMETER_H
#define R_VDEC_CHECK_PARAMETER_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_vdec.h"
#include    "r_vdec_user.h"


#ifdef      R_VDEC_CHECK_PARAMETERS
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
vdec_error_t VDEC_InitializeCheckParam(const vdec_adc_vinsel_t vinsel);
vdec_error_t VDEC_ActivePeriodCheckParam(const vdec_active_period_t * const param);
vdec_error_t VDEC_SyncSeparationCheckParam(const vdec_sync_separation_t * const param);
vdec_error_t VDEC_YcSeparationCheckParam(const vdec_yc_separation_t * const param);
vdec_error_t VDEC_ChromaDecodingCheckParam(const vdec_chroma_decoding_t * const param);
vdec_error_t VDEC_DigitalClampCheckParam(const vdec_degital_clamp_t * const param);
vdec_error_t VDEC_OutputCheckParam(const vdec_output_t * const param);
#endif      /* R_VDEC_CHECK_PARAMETERS */

#endif  /* R_VDEC_CHECK_PARAMETER_H */
