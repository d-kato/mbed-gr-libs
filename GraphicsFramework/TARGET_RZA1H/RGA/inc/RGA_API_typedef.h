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
/**
* @file  RGA_API_typedef.h
* @brief   $Module: RGA $ $PublicVersion: 1.20 $ (=RGA_VERSION)
* $Rev: $
* $Date::                           $
*/

#ifndef  RGA_API_TYPEDEF_H
#define  RGA_API_TYPEDEF_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "Project_Config.h"
#include  "r_ospl_typedef.h"
#include  "locking_typedef.h"
#include  "clib_drivers_typedef.h"
#include  "frame_buffer_typedef.h"
#include  "RGA_raw_image_typedef.h"

#ifdef  __cplusplus
extern "C" {
#endif


/******************************************************************************
Typedef definitions
******************************************************************************/

/**
* @enum   graphics_quality_flag_t
* @brief  graphics_quality_flag_t
*/
typedef enum {
    /* Set to "ON" */
    GRAPHICS_RENDERING_QUALITY_ANTIALIASED     = 0x0001,
    GRAPHICS_IMAGE_QUALITY_ANTIALIASED         = 0x0002,

    /* Set to "OFF" */
    GRAPHICS_RENDERING_QUALITY_NONANTIALIASED  = GRAPHICS_RENDERING_QUALITY_ANTIALIASED << 16,
    GRAPHICS_IMAGE_QUALITY_NONANTIALIASED      = GRAPHICS_IMAGE_QUALITY_ANTIALIASED << 16
}
        graphics_quality_flag_t;


/**
* @enum   graphics_jpeg_decoder_t
* @brief  graphics_jpeg_decoder_t
*/
typedef enum {
    GRAPHICS_JPEG_DECODER_NONE = 0,
    GRAPHICS_JPEG_DECODER_HARD = 1
} graphics_jpeg_decoder_t;


/**
* @typedef  graphics_quality_flags_t
* @brief  graphics_quality_flags_t
*/
typedef bit_flags_fast32_t /*<graphics_quality_flag_t>*/  graphics_quality_flags_t;


/**
* @struct  graphics_config_t
* @brief  graphics_config_t
*/
typedef struct _GraphicsConfigClass  graphics_config_t;
struct _GraphicsConfigClass {
    bit_flags32_t    flags;
    frame_buffer_t  *frame_buffer;  /* F_GRAPHICS_FRAME_BUFFER */

    /* for RGAH/RGAS hardware renderer */
    void   *work_buffer_address;         /* F_GRAPHICS_WORK_BUFFER_ADDRESS */
    size_t  work_buffer_size;            /* F_GRAPHICS_WORK_BUFFER_SIZE */
    int_fast32_t  max_width_of_frame_buffer;   /* F_GRAPHICS_MAX_WIDTH_OF_FRAME_BUFFER */
    int_fast32_t  max_height_of_frame_buffer;  /* F_GRAPHICS_MAX_HEIGHT_OF_FRAME_BUFFER */

    /* for any */
    graphics_quality_flags_t  quality_flags;         /* F_GRAPHICS_QUALITY_FLAGS */
    r8g8b8a8_t                background_color;      /* F_GRAPHICS_BACK_GROUND_COLOR */
    bool_t                    is_fast_manual_flush;  /* F_GRAPHICS_IS_FAST_MANUAL_FLUSH */

    /* work buffer B */
    void   *work_buffer_b_address;       /* F_GRAPHICS_WORK_BUFFER_B_ADDRESS */
    size_t  work_buffer_b_size;          /* F_GRAPHICS_WORK_BUFFER_B_SIZE */

    /* JPEG */
    graphics_jpeg_decoder_t  jpeg_decoder;  /* F_GRAPHICS_JPEG_DECODER */

    /* PNG */
    size_t  work_size_for_libPNG;  /* F_GRAPHICS_WORK_SIZE_FOR_LIBPNG */

    /* Asynchronous call */
    bit_flags32_t  internal_event_value;  /* F_GRAPHICS_INTERNAL_EVENT_VALUE : 16bit */

    /* Thread safe */
    BSP_CFG_USER_LOCKING_TYPE  *lock_object;  /* F_GRAPHICS_LOCK_OBJECT */

    /* for RGAS */
    int_fast32_t  channel_of_DMAC_1;      /* F_GRAPHICS_CHANNEL_OF_DMAC_1 */
    int_fast32_t  channel_of_DMAC_2;      /* F_GRAPHICS_CHANNEL_OF_DMAC_2 */
    int_fast32_t  event_value_of_DMAC_1;  /* F_GRAPHICS_EVENT_VALUE_OF_DMAC_1 */
    int_fast32_t  event_value_of_DMAC_2;  /* F_GRAPHICS_EVENT_VALUE_OF_DMAC_2 */
};
enum {
    F_GRAPHICS_FRAME_BUFFER               = 0x0001,
    F_GRAPHICS_WORK_BUFFER_ADDRESS        = 0x0002,
    F_GRAPHICS_WORK_BUFFER_SIZE           = 0x0004,
    F_GRAPHICS_MAX_HEIGHT_OF_FRAME_BUFFER = 0x0008,
    F_GRAPHICS_MAX_WIDTH_OF_FRAME_BUFFER  = 0x0080,
    F_GRAPHICS_QUALITY_FLAGS              = 0x0010,
    F_GRAPHICS_BACK_GROUND_COLOR          = 0x0020,
    F_GRAPHICS_IS_FAST_MANUAL_FLUSH       = 0x0040,
    F_GRAPHICS_WORK_BUFFER_B_ADDRESS      = 0x0100,
    F_GRAPHICS_WORK_BUFFER_B_SIZE         = 0x0200,
    F_GRAPHICS_JPEG_DECODER               = 0x1000,
    F_GRAPHICS_WORK_SIZE_FOR_LIBPNG       = 0x8000,
    F_GRAPHICS_INTERNAL_EVENT_VALUE       = 0x2000,
    F_GRAPHICS_LOCK_OBJECT                = 0x4000,
    F_GRAPHICS_CHANNEL_OF_DMAC_1          = 0x0400,
    F_GRAPHICS_CHANNEL_OF_DMAC_2          = 0x0800,
    F_GRAPHICS_EVENT_VALUE_OF_DMAC_1    = 0x040000,
    F_GRAPHICS_EVENT_VALUE_OF_DMAC_2    = 0x080000
};


/**
* @typedef  graphics_matrix_float_t
* @brief  graphics_matrix_float_t
*/
typedef  float  graphics_matrix_float_t;


/**
* @struct  graphics_matrix3x3_t
* @brief  graphics_matrix3x3_t
*/
typedef union {
    graphics_matrix_float_t   xy[3][3];  /* [x][y] */
    graphics_matrix_float_t   array[9];  /* { sx, shy, w0, shx, sy, w1, tx, ty, w2 } */
} graphics_matrix3x3_t;


/**
* @struct  graphics_image_properties_t
* @brief  graphics_image_properties_t
*/
#if  IS_SUPPORT_SAME_TYPEDEF  || ! defined( graphics_image_properties_t )
typedef struct _GraphicsImagePropertiesClass  graphics_image_properties_t;
#define         graphics_image_properties_t   graphics_image_properties_t
#endif
struct _GraphicsImagePropertiesClass {
    int_fast32_t     width;
    int_fast32_t     height;
    uint8_t         *data;    /* NULL, if pixelFormat != PIXEL_FORMAT_R8G8B8A8 */
    void            *pixels;  /* Same as "data" but not NULL */
    pixel_format_t   pixelFormat;
    uint32_t        *CLUT;
    int_fast32_t     CLUT_count;
};


/**
* @enum   repetition_t
* @brief  repetition_t
*/
typedef enum st_repetition_t {
    GRAPHICS_REPEAT = 1
} repetition_t;


/**
* @struct  graphics_pattern_t
* @brief  graphics_pattern_t
*/
typedef struct st_graphics_pattern_t  graphics_pattern_t;
struct st_graphics_pattern_t {
    graphics_image_t *image;
    repetition_t      repetition;
};


/**
* @enum   graphics_composite_operation_t
* @brief  graphics_composite_operation_t
*/
typedef enum st_graphics_composite_operation_t {
    GRAPHICS_SOURCE_OVER = 1,
    GRAPHICS_DESTINATION_OUT = 7,
    GRAPHICS_COPY = 0
} graphics_composite_operation_t;


/**
* @struct  graphics_status_t
* @brief  graphics_status_t
*/
typedef struct st_graphics_status_t  graphics_status_t;
struct st_graphics_status_t {
    uint8_t  __Members[0x30];
};


/**
* @struct  graphics_hard_status_t
* @brief  graphics_hard_status_t
*/
#ifdef  RGAH_VERSION
typedef struct st_graphics_hard_status_t  graphics_hard_status_t;
struct st_graphics_hard_status_t {
    volatile bool_t  IsRenderingCompleted;
};
#endif


/**
* @struct  graphics_async_status_t
* @brief  graphics_async_status_t
*/
typedef struct st_graphics_async_status_t  graphics_async_status_t;
struct st_graphics_async_status_t {
    volatile r_ospl_async_state_t  State;
    r_ospl_async_t *volatile       Async;
    BSP_CFG_USER_LOCKING_TYPE     *CLock;
    r_ospl_thread_id_t             LockedThread;
#ifdef  RGAH_VERSION
    const graphics_hard_status_t  *Hardware;
#endif
};


/**
* @struct  graphics_t
* @brief  graphics_t
*/
#if  IS_SUPPORT_SAME_TYPEDEF  || ! defined( graphics_t )
typedef struct st_graphics_t  graphics_t;
#endif
struct st_graphics_t {
    uint8_t  __Members[0x118];
};


/**
* @brief   R_GRAPHICS_OnInitialize_FuncType
*
* @par Parameters
*    None
* @return  None.
*/
typedef  errnum_t  (*R_GRAPHICS_OnInitialize_FuncType)(
    graphics_t *self, graphics_config_t *in_out_Config, void **out_CalleeDefined );

/**
* @brief   R_GRAPHICS_OnFinalize_FuncType
*
* @par Parameters
*    None
* @return  None.
*/
typedef  errnum_t  (*R_GRAPHICS_OnFinalize_FuncType)(
    graphics_t *self, void *CalleeDefined, errnum_t e );


/**
* @struct  graphics_static_t
* @brief  graphics_static_t
*/
typedef struct st_graphics_static_t  graphics_static_t;
struct st_graphics_static_t {
    R_GRAPHICS_OnInitialize_FuncType  OnInitialize;
    R_GRAPHICS_OnFinalize_FuncType    OnFinalize;
    graphics_t                       *Contexts[1];
};


/**
* @struct  animation_timing_function_t
* @brief  animation_timing_function_t
*/
typedef struct _CSS_animation_timing_function_Class  animation_timing_function_t;


/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/


#ifdef  __cplusplus
}  /* extern "C" */
#endif

#endif  /* RGA_API_TYPEDEF_H */
