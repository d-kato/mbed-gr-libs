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
* @file         r_vdc_shared_param.h
* @version      0.01
* $Rev: 18 $
* $Date:: 2014-05-29 11:11:38 +0900#$
* @brief        RZ/A2M VDC driver shared parameter definitions
******************************************************************************/

#ifndef R_VDC_SHARED_PARAM_H
#define R_VDC_SHARED_PARAM_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_vdc.h"
#include "r_vdc_user.h"


/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/
/*! Color space */
typedef enum
{
    VDC_COLOR_SPACE_GBR    = 0,        /*!< GBR */
    VDC_COLOR_SPACE_YCBCR  = 1         /*!< YCbCr */
} vdc_color_space_t;

/*! Resource state */
typedef enum
{
    VDC_RESOURCE_ST_INVALID    = 0,    /*!< Invalid */
    VDC_RESOURCE_ST_VALID      = 1     /*!< Valid */
} vdc_resource_state_t;

/*! Resource type */
typedef enum
{
    VDC_RESOURCE_PANEL_CLK = 0,        /*!< Panel clock */
    VDC_RESOURCE_VIDEO_IN,             /*!< Input video */
    VDC_RESOURCE_VSYNC,                /*!< Vsync signal */
    VDC_RESOURCE_LCD_PANEL,            /*!< LCD panel (output video) */
    VDC_RESOURCE_LVDS_CLK,             /*!< LVDS PLL clock */
    VDC_RESOURCE_NUM
} vdc_resource_type_t;


/******************************************************************************
Functions Prototypes
******************************************************************************/
void VDC_ShrdPrmInit(void);

void VDC_ShrdPrmSetInitParam(const vdc_init_t * const param);
void VDC_ShrdPrmSetTerminate(void);
void VDC_ShrdPrmSetInputParam(const vdc_input_t * const param);
void VDC_ShrdPrmSetOutputParam(const vdc_output_t * const param);
void VDC_ShrdPrmSetWriteParam(const vdc_write_t * const param);
void VDC_ShrdPrmSetChgWriteParam(const vdc_write_chg_t * const param);
void VDC_ShrdPrmSetReadParam(const vdc_graphics_type_t graphics_id, const vdc_read_t * const param);
void VDC_ShrdPrmSetChgReadParam(const vdc_graphics_type_t graphics_id, const vdc_read_chg_t * const param);

vdc_onoff_t VDC_ShrdPrmGetLvdsClkRef(void);
vdc_color_space_t VDC_ShrdPrmGetColorSpace(void);
uint32_t VDC_ShrdPrmGetBgColor(const vdc_color_space_t color_space);
vdc_wr_md_t VDC_ShrdPrmGetWritingMode(void);
vdc_res_inter_t VDC_ShrdPrmGetInterlace(void);
vdc_color_space_t VDC_ShrdPrmGetColorSpaceFbWr(void);
void * VDC_ShrdPrmGetFrBuffBtm(void);

vdc_gr_ln_off_dir_t VDC_ShrdPrmGetLineOfsAddrDir(const vdc_graphics_type_t graphics_id);
vdc_gr_flm_sel_t VDC_ShrdPrmGetSelFbAddrSig(const vdc_graphics_type_t graphics_id);
vdc_gr_format_t VDC_ShrdPrmGetGraphicsFormat(const vdc_graphics_type_t graphics_id);
vdc_color_space_t VDC_ShrdPrmGetColorSpaceFbRd(const vdc_graphics_type_t graphics_id);
vdc_onoff_t VDC_ShrdPrmGetMeasureFolding(const vdc_graphics_type_t graphics_id);
vdc_period_rect_t * VDC_ShrdPrmGetDisplayArea(const vdc_graphics_type_t graphics_id);
vdc_width_read_fb_t * VDC_ShrdPrmGetFrBuffWidth_Rd(const vdc_graphics_type_t  graphics_id);

void VDC_ShrdPrmSetResource(const vdc_resource_type_t rsrc_type, const vdc_resource_state_t rsrc_state);
void VDC_ShrdPrmSetLayerResource(const vdc_layer_id_t layer_id, const vdc_resource_state_t rsrc_state);
vdc_resource_state_t VDC_ShrdPrmGetResource(const vdc_resource_type_t rsrc_type);
vdc_resource_state_t VDC_ShrdPrmGetLayerResource(const vdc_layer_id_t layer_id);

void VDC_ShrdPrmSetRwProcEnable(const vdc_layer_id_t layer_id);
void VDC_ShrdPrmSetRwProcDisable(const vdc_layer_id_t layer_id);
vdc_resource_state_t VDC_ShrdPrmGetRwProcReady(const vdc_layer_id_t layer_id);
vdc_resource_state_t VDC_ShrdPrmGetRwProcEnabled(const vdc_layer_id_t layer_id);
vdc_channel_t VDC_ShrdPrmGetLvdsCh(void);


#endif  /* R_VDC_SHARED_PARAM_H */

