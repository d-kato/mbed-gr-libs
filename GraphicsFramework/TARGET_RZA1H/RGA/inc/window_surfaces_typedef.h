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
* Copyright (C) 2012 - 2014 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**
* @file  window_surfaces_typedef.h
* @brief   $Module: CLibCommon $ $PublicVersion: 1.00 $ (=CLIB_VERSION)
* $Rev: 47 $
* $Date:: 2014-01-06 18:18:48 +0900#$
*/

#ifndef  WINDOW_SURFACES_TYPEDEF_H
#define  WINDOW_SURFACES_TYPEDEF_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "Project_Config.h"
#include  "r_vdc5.h"
#include  "frame_buffer_typedef.h"
#include  "RGA_Port_typedef.h"
#ifdef  IS_WINDOW_SURFACES_EX
#include  "window_surfaces_ex_typedef.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Typedef definitions
******************************************************************************/

/**
* @enum   background_format_t
* @brief  background_format_t
*
*    - BACKGROUND_FORMAT_SOLID_COLOR - 0
*    - BACKGROUND_FORMAT_VIDEO_INTERLACE - 1
*/
enum st_background_format_t {
    BACKGROUND_FORMAT_SOLID_COLOR,
    BACKGROUND_FORMAT_VIDEO_INTERLACE
};
typedef enum st_background_format_t  background_format_t;
/* "typedef" must be after "enum". Because GCC_ARM raises an error */


/**
* @struct  window_surfaces_vdc5_layer_t
* @brief  VDC5 layer
*/
typedef struct st_window_surfaces_vdc5_layer_t  window_surfaces_vdc5_layer_t;
struct st_window_surfaces_vdc5_layer_t {

    /*-----------------------------------------------------------*/
    /* Group: Layer */

    /** layer_num */
    int8_t  layer_num;

    /** graphics_layer_ID */
    vdc5_graphics_type_t  graphics_layer_ID;


    /*-----------------------------------------------------------*/
    /* Group: Data Control */

    /** is_data_control */
    bool_t  is_data_control;

    /** data_control_ID */
    vdc5_layer_id_t  data_control_ID;


    /*-----------------------------------------------------------*/
    /* Group: Color Format */

    /** vdc5_format */
    vdc5_gr_format_t  vdc5_format;

    /** layer_color */
    r8g8b8a8_t  layer_color;
};


/**
* @struct  window_surfaces_t
* @brief  window_surfaces_t
*/
typedef struct st_window_surfaces_t  window_surfaces_t;
#define   WINDOW_SURFACES_T_MAX_LAYERS_COUNT   3

struct st_window_surfaces_t {
    /** Index is 0 .. (layer_num_max + background_frame_count) */
    frame_buffer_t  frame_buffers[ WINDOW_SURFACES_T_MAX_LAYERS_COUNT ];

    /** screen_channel */
    vdc5_channel_t  screen_channel;

    /** layer_num_min */
    int8_t  layer_num_min;

    /** layer_num_max */
    int8_t  layer_num_max;

    /** layers */
    window_surfaces_vdc5_layer_t  layers[ WINDOW_SURFACES_T_MAX_LAYERS_COUNT ];

    /** attributes */
#ifdef  IS_WINDOW_SURFACES_EX
    window_surfaces_attribute_t   attributes[ WINDOW_SURFACES_T_MAX_LAYERS_COUNT ];
#endif

    /** vdc5_disp_sel_array */
    vdc5_gr_disp_sel_t   vdc5_disp_sel_array[ VDC5_GR_TYPE_NUM ];

    /** background_format */
    background_format_t  background_format;

    /** background_frame_count */
    int_fast32_t   background_frame_count;


    /*-----------------------------------------------------------*/
    /* Group: VRAM */

    /** start_of_VRAM */
    uint8_t  *start_of_VRAM;

    /** over_of_VRAM */
    uint8_t  *over_of_VRAM;

    /** stack_pointer_of_VRAM */
    uint8_t  *stack_pointer_of_VRAM;


    /*-----------------------------------------------------------*/
    /* Group: State */

    /** is_initialized */
    bool_t  is_initialized;  /* For reduce loading */
};


/**
* @struct  window_surfaces_config_t
* @brief  window_surfaces_config_t
*/
typedef struct st_window_surfaces_config_t  window_surfaces_config_t;
struct st_window_surfaces_config_t {
    /** 0 | F_WINDOW_SURFACES_PIXEL_FORMAT | ... */
    bit_flags_fast32_t  flags;

    /** pixel_format */
    pixel_format_t  pixel_format;

    /** layer_count */
    int_fast32_t  layer_count;

    /** buffer_height */
    int_fast32_t  buffer_height;

    /** background_color */
    r8g8b8a8_t  background_color;

    /** background_format */
    background_format_t  background_format;
};
enum  /* "window_surfaces_config_t::flags" */
{
    F_WINDOW_SURFACES_PIXEL_FORMAT      = 0x08,
    F_WINDOW_SURFACES_LAYER_COUNT       = 0x20,
#define  F_WINDOW_SURFACES_BACKGROUND_COLOR    0x40u
    F_WINDOW_SURFACES_BUFFER_HEIGHT     = 0x80,
    F_WINDOW_SURFACES_BACKGROUND_FORMAT = 0x100
};


/***********************************************************************
* Section: Global
************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/**
* @def  WINDOW_SURFACES_DEFAULT_CHANNEL
* @brief  0 or 1
*/
#define  WINDOW_SURFACES_DEFAULT_CHANNEL  0


/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
/* In "frame_buffer.h" */

#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif
