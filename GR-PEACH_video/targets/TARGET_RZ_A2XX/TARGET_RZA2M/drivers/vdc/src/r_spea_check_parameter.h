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
* @file         r_spea_check_parameter.h
* @version      0.01
* $Rev: 18 $
* $Date:: 2014-05-29 11:11:38 +0900#$
* @brief        RZ/A2M SPEA driver parameter check definitions
******************************************************************************/

#ifndef R_SPEA_CHECK_PARAMETER_H
#define R_SPEA_CHECK_PARAMETER_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_spea.h"
#include "r_vdc.h"

#ifdef      R_SPEA_CHECK_PARAMETERS
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
/*--- SPEA function --------------------------------------*/

spea_error_t SPEA_SetWindowCheckPrm (
	const vdc_layer_id_t       layer_id,
	const spea_window_id_t     window_id,
	const spea_onoff_t         sken,
	const spea_sklym_t       * sklym,
	const spea_skpsm_t       * skpsm,
	const void               * buffer);

spea_error_t SPEA_WindowOffsetCheckPrm (
    const vdc_layer_id_t       layer_id,
    const uint16_t             offset_x,
    const uint16_t             offset_y);

spea_error_t SPEA_CreateSurfaceIDCheckPrm (
    const vdc_layer_id_t      layer_id);

/*--- RLE function --------------------------------------*/

spea_error_t RLE_CreateSurfaceIDCheckPrm (
    const vdc_layer_id_t      layer_id);

spea_error_t RLE_SetWindowCheckPrm(
	    const vdc_layer_id_t  layer_id,
		const rle_cfg_t     * rle_cfg,
		const void          * buffer);

#endif      /* R_SPEA_CHECK_PARAMETERS */

#endif  /* R_SPEA_CHECK_PARAMETER_H */
