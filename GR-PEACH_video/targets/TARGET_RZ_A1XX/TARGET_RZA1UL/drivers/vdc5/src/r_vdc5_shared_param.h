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
* @file         r_vdc5_shared_param.h
* @version      0.06
* $Rev: 18 $
* $Date:: 2014-05-29 11:11:38 +0900#$
* @brief        RZ/A1L VDC5 driver shared parameter definitions
******************************************************************************/

#ifndef R_VDC5_SHARED_PARAM_H
#define R_VDC5_SHARED_PARAM_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_vdc5.h"
#include    "r_vdc5_user.h"


/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/
/*! Color space */
typedef enum
{
    VDC5_COLOR_SPACE_GBR    = 0,        /*!< GBR */
    VDC5_COLOR_SPACE_YCBCR  = 1         /*!< YCbCr */
} vdc5_color_space_t;

/*! Resource state */
typedef enum
{
    VDC5_RESOURCE_ST_INVALID    = 0,    /*!< Invalid */
    VDC5_RESOURCE_ST_VALID      = 1     /*!< Valid */
} vdc5_resource_state_t;

/*! Resource type */
typedef enum
{
    VDC5_RESOURCE_PANEL_CLK = 0,        /*!< Panel clock */
    VDC5_RESOURCE_VIDEO_IN,             /*!< Input video */
    VDC5_RESOURCE_VSYNC,                /*!< Vsync signal */
    VDC5_RESOURCE_LCD_PANEL,            /*!< LCD panel (output video) */
    VDC5_RESOURCE_NUM
} vdc5_resource_type_t;


/******************************************************************************
Functions Prototypes
******************************************************************************/
void VDC5_ShrdPrmInit(void);

void VDC5_ShrdPrmSetInputParam(const vdc5_input_t * const param);
void VDC5_ShrdPrmSetOutputParam(const vdc5_output_t * const param);
void VDC5_ShrdPrmSetWriteParam(const vdc5_write_t * const param);
void VDC5_ShrdPrmSetChgWriteParam(const vdc5_write_chg_t * const param);
void VDC5_ShrdPrmSetReadParam(const vdc5_graphics_type_t graphics_id, const vdc5_read_t * const param);
void VDC5_ShrdPrmSetChgReadParam(const vdc5_graphics_type_t graphics_id, const vdc5_read_chg_t * const param);

vdc5_color_space_t VDC5_ShrdPrmGetColorSpace(void);
uint32_t VDC5_ShrdPrmGetBgColor(const vdc5_color_space_t color_space);
vdc5_wr_md_t VDC5_ShrdPrmGetWritingMode(void);
vdc5_res_inter_t VDC5_ShrdPrmGetInterlace(void);
vdc5_color_space_t VDC5_ShrdPrmGetColorSpaceFbWr(void);
void * VDC5_ShrdPrmGetFrBuffBtm(void);

vdc5_gr_ln_off_dir_t VDC5_ShrdPrmGetLineOfsAddrDir(const vdc5_graphics_type_t graphics_id);
vdc5_gr_flm_sel_t VDC5_ShrdPrmGetSelFbAddrSig(const vdc5_graphics_type_t graphics_id);
vdc5_gr_format_t VDC5_ShrdPrmGetGraphicsFormat(const vdc5_graphics_type_t graphics_id);
vdc5_color_space_t VDC5_ShrdPrmGetColorSpaceFbRd(const vdc5_graphics_type_t graphics_id);
vdc5_onoff_t VDC5_ShrdPrmGetMeasureFolding(const vdc5_graphics_type_t graphics_id);
vdc5_period_rect_t * VDC5_ShrdPrmGetDisplayArea(const vdc5_graphics_type_t graphics_id);
vdc5_width_read_fb_t * VDC5_ShrdPrmGetFrBuffWidth_Rd(const vdc5_graphics_type_t  graphics_id);

void VDC5_ShrdPrmSetResource(const vdc5_resource_type_t rsrc_type, const vdc5_resource_state_t rsrc_state);
void VDC5_ShrdPrmSetLayerResource(const vdc5_layer_id_t layer_id, const vdc5_resource_state_t rsrc_state);
vdc5_resource_state_t VDC5_ShrdPrmGetResource(const vdc5_resource_type_t rsrc_type);
vdc5_resource_state_t VDC5_ShrdPrmGetLayerResource(const vdc5_layer_id_t layer_id);

void VDC5_ShrdPrmSetRwProcEnable(const vdc5_layer_id_t layer_id);
void VDC5_ShrdPrmSetRwProcDisable(const vdc5_layer_id_t layer_id);
vdc5_resource_state_t VDC5_ShrdPrmGetRwProcReady(const vdc5_layer_id_t layer_id);
vdc5_resource_state_t VDC5_ShrdPrmGetRwProcEnabled(const vdc5_layer_id_t layer_id);


#endif  /* R_VDC5_SHARED_PARAM_H */

