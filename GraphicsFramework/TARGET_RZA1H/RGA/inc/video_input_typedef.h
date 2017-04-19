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
* @file  video_input_typedef.h
* @brief   $Rev: 47 $
* $Date:: 2014-01-06 18:18:48 +0900#$
*/

#ifndef  VIDEO_INPUT_TYPEDEF_H
#define  VIDEO_INPUT_TYPEDEF_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_vdc5.h"
#include  "r_vdec.h"
#include  "frame_buffer_typedef.h"
#include  "r_ospl_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Macro definitions
******************************************************************************/

/***********************************************************************
* Macros: INTERRUPT_FUNCTION_TYPE
*    INTERRUPT_FUNCTION_TYPE
*
*    : INTERRUPT_FUNCTION_TYPE_OS_RTX  - RTX
*    : INTERRUPT_FUNCTION_TYPE_OS_LESS - OS less
************************************************************************/
#if R_OSPL_IS_PREEMPTION
#define  INTERRUPT_FUNCTION_TYPE  INTERRUPT_FUNCTION_TYPE_OS_RTX
#else
#define  INTERRUPT_FUNCTION_TYPE  INTERRUPT_FUNCTION_TYPE_OS_LESS
#endif
#define  INTERRUPT_FUNCTION_TYPE_OS_LESS  0
#define  INTERRUPT_FUNCTION_TYPE_OS_RTX   1


/******************************************************************************
Typedef definitions
******************************************************************************/

/**
* @enum   video_input_channel_enum_t
* @brief  Special integer value
*
*    - VIDEO_INPUT_NOT_DISPLAY - 0xFF
*/
enum st_video_input_channel_enum_t {
    VIDEO_INPUT_NOT_DISPLAY = 0xFF
};
typedef enum st_video_input_channel_enum_t  video_input_channel_enum_t;
/* "typedef" must be after "enum". Because GCC_ARM raises an error */


/**
* @struct  video_input_t
* @brief  video_input_t
*
* @par Description
*    Member variables can not be accessed by user.
*/
typedef struct st_video_input_t  video_input_t;
struct st_video_input_t {

    /*-----------------------------------------------------------*/
    /* Group: Cannel */

    /** video_input_channel */
    vdc5_channel_t      video_input_channel;

    /** video_input_select */
    vdec_adc_vinsel_t   video_input_select;

    /** is_data_control */
    bool_t              is_data_control;

    /** data_control_ID */
    vdc5_layer_id_t     data_control_ID;


    /*-----------------------------------------------------------*/
    /* Group: Interrupt */

    /** is_vsync_interrupt_registered */
    bool_t              is_vsync_interrupt_registered[3];

    /** interrupt_for_VDC5 */
    vdc5_int_t          interrupt_for_VDC5;

    /** interrupt_ID */
    bsp_int_src_t       interrupt_ID;

    /** interrupt_vector */
#if INTERRUPT_FUNCTION_TYPE == INTERRUPT_FUNCTION_TYPE_OS_RTX
    bsp_int_cb_t        interrupt_vector;
#endif


    /*-----------------------------------------------------------*/
    /* Group: Buffer */

    /** frame_buffer */
    frame_buffer_t      frame_buffer;

    /** captured_buffer_index */
    int_fast32_t        captured_buffer_index;

    /** captured_count */
    int_fast32_t        captured_count;

    /** captured_async */
    r_ospl_async_t     *captured_async;
};


/**
* @struct  video_input_config_t
* @brief  Parameters of <R_VIDEO_INPUT_Initialize>
*/
typedef struct st_video_input_config_t  video_input_config_t;
struct st_video_input_config_t {

    /** 0 | VIDEO_INPUT_CONFIG_T_layer_ID | ... */
    bit_flags_fast32_t  flags;


    /** 0=ch0.VIN1, 1=ch1.VIN1, 2=ch0.VIN2, 3=ch1.VIN2 */
    int_fast32_t        video_input_channel_num;

    /** 0, 1 or VIDEO_INPUT_NOT_DISPLAY */
    int_fast32_t        display_channel_num;

    /** -1 or VIDEO_INPUT_NOT_DISPLAY */
    int_fast32_t        display_layer_num;

    /** frame_buffer */
    frame_buffer_t     *frame_buffer;

    /** captured_async */
    r_ospl_async_t     *captured_async;
};
/* video_input_config_t::flags */
#define  VIDEO_INPUT_CONFIG_T_VIDEO_INPUT_CHANNEL_NUM  0x0001
#define  VIDEO_INPUT_CONFIG_T_DISPLAY_CHANNEL_NUM      0x0002
#define  VIDEO_INPUT_CONFIG_T_DISPLAY_LAYER_NUM        0x0004
#define  VIDEO_INPUT_CONFIG_T_FRAME_BUFFER             0x0008
#define  VIDEO_INPUT_CONFIG_T_CAPTURED_ASYNC           0x0010


/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
/* In "video_input.h" */

#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif
