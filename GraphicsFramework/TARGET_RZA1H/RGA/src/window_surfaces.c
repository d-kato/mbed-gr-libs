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
* @file  window_surfaces.c
* @brief   $Module: CLibCommon $ $PublicVersion: 1.00 $ (=CLIB_VERSION)
* $Rev: 51 $
* $Date:: 2014-03-14 18:42:33 +0900#$
*/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include  "r_typedefs.h"
#include  "lcd_panel.h"
#include  "r_ospl.h"
#include  "clib_registers.h"
#include  "clib_drivers.h"
#include  "clib_samples.h"
#include  "window_surfaces.h"
#include  "vsync.h"
#include  "window_surfaces_private.h"
#include  "RGA.h"
#include  "RGA_Port.h"
#if  USE_LCD
#include  "lcd_if.h"
#endif
#include  <stdio.h>  /* in R_WINDOW_SURFACES_DoMessageLoop */


/******************************************************************************
Typedef definitions
******************************************************************************/

/**
* @typedef  gs_frame_width_height_t
* @brief  gs_frame_width_height_t
*/
#ifdef  IS_WINDOW_SURFACES_EX
typedef struct st_gs_frame_width_height_t  gs_frame_width_height_t;
struct st_gs_frame_width_height_t {
    int_fast32_t  Width;
    int_fast32_t  Height;
};
#endif


/**
* @typedef  gs_rectangle_t
* @brief  gs_rectangle_t
*/
#ifdef  IS_WINDOW_SURFACES_EX
typedef struct st_gs_rectangle_t  gs_rectangle_t;
struct st_gs_rectangle_t {
    int_fast32_t  Left;
    int_fast32_t  Top;
    int_fast32_t  Right;   /* include right  bound pixel */
    int_fast32_t  Bottom;  /* include bottom bound pixel */ /* Top < Bottom */
};
#endif


/******************************************************************************
Macro definitions
******************************************************************************/

/* GS_ : global static */
/* VDC5_CHANNEL_0 = LCD_VDC5_CH0_PANEL = ... */
/* VDC5_CHANNEL_1 = LCD_VDC5_CH1_PANEL = ... */
#if WINDOW_SURFACES_DEFAULT_CHANNEL
#define   GS_CHANNEL_n              VDC5_CHANNEL_1
#define   GS_LCD_CHn_SIG_FV         LCD_CH1_SIG_FV
#define   GS_LCD_CHn_SIG_FH         LCD_CH1_SIG_FH
#define   GS_LCD_CHn_DISP_VS        LCD_CH1_DISP_VS
#define   GS_LCD_CHn_DISP_VW        LCD_CH1_DISP_VW
#define   GS_LCD_CHn_DISP_HS        LCD_CH1_DISP_HS
#define   GS_LCD_CHn_DISP_HW        LCD_CH1_DISP_HW
#define   GS_LCD_CHn_TCON_HALF      LCD_CH1_TCON_HALF
#define   GS_LCD_CHn_TCON_OFFSET    LCD_CH1_TCON_OFFSET
#define   GS_LCD_CHn_OUT_EDGE       LCD_CH1_OUT_EDGE
#define   GS_LCD_CHn_OUT_FORMAT     LCD_CH1_OUT_FORMAT
#define   GS_LCD_CHn_PANEL_CLK      LCD_CH1_PANEL_CLK
#define   GS_LCD_CHn_PANEL_CLK_DIV  LCD_CH1_PANEL_CLK_DIV
#define   GS_DISPLAY_CH             1   /* 1 or 0 */
#else
#define   GS_CHANNEL_n              VDC5_CHANNEL_0
#define   GS_LCD_CHn_SIG_FV         LCD_CH0_SIG_FV
#define   GS_LCD_CHn_SIG_FH         LCD_CH0_SIG_FH
#define   GS_LCD_CHn_DISP_VS        LCD_CH0_DISP_VS
#define   GS_LCD_CHn_DISP_VW        LCD_CH0_DISP_VW
#define   GS_LCD_CHn_DISP_HS        LCD_CH0_DISP_HS
#define   GS_LCD_CHn_DISP_HW        LCD_CH0_DISP_HW
#define   GS_LCD_CHn_TCON_HALF      LCD_CH0_TCON_HALF
#define   GS_LCD_CHn_TCON_OFFSET    LCD_CH0_TCON_OFFSET
#define   GS_LCD_CHn_OUT_EDGE       LCD_CH0_OUT_EDGE
#define   GS_LCD_CHn_OUT_FORMAT     LCD_CH0_OUT_FORMAT
#define   GS_LCD_CHn_PANEL_CLK      LCD_CH0_PANEL_CLK
#define   GS_LCD_CHn_PANEL_CLK_DIV  LCD_CH0_PANEL_CLK_DIV
#define   GS_DISPLAY_CH             0   /* 1 or 0 */
#endif

#if  1
#define   GS_BUFFER_WIDTH           800
#define   GS_BUFFER_HEIGHT          480
#else
#define   GS_BUFFER_WIDTH           GS_LCD_CHn_DISP_HW
#define   GS_BUFFER_HEIGHT          GS_LCD_CHn_DISP_VW
#endif

#define   GS_MEASURE_GPU_LOAD       0

#define  GS_DEFAULT_CLEAR_COLOR  R_RGA_DEFAULT_CLEAR_COLOR

#ifdef  R_REE_INIT
#define  GRAPHICS_GetLvdsParam        Graphics_GetLvdsParam
#define  GRAPHICS_SetLcdTconSettings  Graphics_SetLcdTconSettings
#define  GRAPHICS_SetLcdPanel         Graphics_SetLcdPanel
#endif
#ifdef  RZ_A1L
#define  GRAPHICS_GetLvdsParam( ch )                 NULL
#define  GRAPHICS_SetLcdTconSettings( ch, setting )  GRAPHICS_L_SetLcdTconSettings( setting )
#define  GRAPHICS_SetLcdPanel( ch )                  GRAPHICS_L_SetLcdPanel()
#endif

/**
* @def  GS_OFFSET_BYTE_NOT_SHOW
* @brief  GS_OFFSET_BYTE_NOT_SHOW
*/
enum { GS_OFFSET_BYTE_NOT_SHOW = -1 };


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

static errnum_t  R_WINDOW_SURFACES_SwapBuffers_Sub( window_surfaces_t *const  self,
        int_fast32_t const  layer_num );

#ifdef  IS_WINDOW_SURFACES_EX
errnum_t  GS_ToInFrame(
    gs_frame_width_height_t *SourceFrameBuffer,
    gs_frame_width_height_t *DestinationFrameBuffer,
    gs_rectangle_t *SourceRect,
    gs_rectangle_t *DestinationRect );
#endif


/**
* @brief   alloc_VRAM_stack_sub
*
* @param   in_out_StackPointer in_out_StackPointer
* @param   OverOfVRAM OverOfVRAM
* @param   in_out_FrameBuffer in_out_FrameBuffer
* @return  Error code, 0=No error
*
* @par Description
*    - (input) - >stride, ->height, ->buffer_count
*    - (output) - >buffer_address[(all)]
*/
static errnum_t  alloc_VRAM_stack_sub( uint8_t **const  in_out_StackPointer,
                                       const uint8_t *const  OverOfVRAM,
                                       frame_buffer_t *const  in_out_FrameBuffer ); /* QAC-3450 */
static errnum_t  alloc_VRAM_stack_sub( uint8_t **const  in_out_StackPointer,
                                       const uint8_t *const  OverOfVRAM,
                                       frame_buffer_t *const  in_out_FrameBuffer )
{
    errnum_t      e;
    int_fast32_t  size_1;
    int_fast32_t  size_all;
    int_fast32_t  buffer_num;
    uint8_t      *stack_pointer;


    IF_DQ( in_out_StackPointer == NULL ) {
        e=E_OTHERS;
        goto fin;
    }
    IF_DQ( in_out_FrameBuffer == NULL ) {
        e=E_OTHERS;
        goto fin;
    }


    stack_pointer = *in_out_StackPointer;


    /* Set "size_1" */
    size_1 = in_out_FrameBuffer->stride * in_out_FrameBuffer->height;
    size_1 = ( R_Ceil_64s( size_1 ) );


    /* Set "size_all" */
    size_all = in_out_FrameBuffer->buffer_count * size_1;


    /* Check */
    /* ->MISRA 17.4 */ /* ->SEC R1.3.1 (1) */
    IF ( (stack_pointer + size_all) > OverOfVRAM ) {
        in_out_FrameBuffer->buffer_count = 0;
        e=E_FEW_ARRAY;
        goto fin;
    }
    /* <-MISRA 17.4 */ /* <-SEC R1.3.1 (1) */

    IF ( in_out_FrameBuffer->buffer_count >
         (int_fast32_t) R_COUNT_OF( in_out_FrameBuffer->buffer_address ) ) {
        e=E_OTHERS;
        goto fin;
    }


    /* Set "in_out_FrameBuffer->buffer_address" */
    for ( buffer_num = 0;
            buffer_num < in_out_FrameBuffer->buffer_count;
            buffer_num += 1 ) {
        in_out_FrameBuffer->buffer_address[ buffer_num ] = stack_pointer;

        /* ->MISRA 17.4 */ /* ->SEC R1.3.1 (1) */
        stack_pointer += size_1;  /* MISRA 17.4: Bound check is done by "OverOfVRAM" */
        /* <-MISRA 17.4 */ /* <-SEC R1.3.1 (1) */
    }
    for ( /* buffer_num */;
                          buffer_num < (int_fast32_t) R_COUNT_OF( in_out_FrameBuffer->buffer_address );
                          buffer_num += 1 ) {
        in_out_FrameBuffer->buffer_address[ buffer_num ] = NULL;
    }

    *in_out_StackPointer = stack_pointer;

    e=0;
fin:
    return  e;
}


/**
* @brief   free_VRAM_stack_sub
*
* @param   in_out_StackPointer in_out_StackPointer
* @param   frame_buffer frame_buffer
* @param   StartOfVRAM StartOfVRAM
* @return  Error code, 0=No error
*
* @par Description
*    - If frame_buffer - >buffer_count == 0, do nothing.
*/
static errnum_t  free_VRAM_stack_sub( uint8_t **const  in_out_StackPointer,
                                      const frame_buffer_t *const  frame_buffer,
                                      const uint8_t *const  StartOfVRAM ); /* QAC-3450 */
static errnum_t  free_VRAM_stack_sub( uint8_t **const  in_out_StackPointer,
                                      const frame_buffer_t *const  frame_buffer,
                                      const uint8_t *const  StartOfVRAM )
{
    errnum_t      e;
    int_fast32_t  size_1;
    int_fast32_t  size_all;
    int_fast32_t  buffer_num;
    uint8_t      *next_stack_pointer;


    IF_DQ( frame_buffer == NULL ) {
        e=E_OTHERS;
        goto fin;
    }
    IF_DQ( in_out_StackPointer == NULL ) {
        e=E_OTHERS;
        goto fin;
    }


    /* Set "size_1" */
    size_1 = frame_buffer->stride * frame_buffer->height;
    size_1 = ( R_Ceil_64s( size_1 ) );
    R_STATIC_ASSERT( RGA_STACK_ADDRESS_ALIGNMENT == 64, "" );  /* check of ceil_xx */


    /* Set "size_all" */
    size_all = frame_buffer->buffer_count * size_1;


    /* Set "next_stack_pointer" */
    /* ->MISRA 17.4 */ /* ->SEC R1.3.1 (1) */
    next_stack_pointer = *in_out_StackPointer - size_all;
    /* MISRA 17.4: Bound check is done by "StartOfVRAM" */
    /* <-MISRA 17.4 */ /* <-SEC R1.3.1 (1) */


    /* Check */
    IF ( next_stack_pointer < StartOfVRAM ) {
        e=E_OTHERS;
        goto fin;
    }

    IF ( frame_buffer->buffer_count > (int_fast32_t) R_COUNT_OF( frame_buffer->buffer_address ) ) {
        e=E_OTHERS;
        goto fin;
    }

    for ( buffer_num = frame_buffer->buffer_count - 1;  buffer_num >= 0;  buffer_num -= 1 ) {
        /* ->MISRA 17.4 */ /* ->SEC R1.3.1 (1) */
        IF ( frame_buffer->buffer_address[ buffer_num ] !=
             (next_stack_pointer + (size_1 * buffer_num)) ) {
            e=E_ACCESS_DENIED;
            goto fin;
        }
        /* MISRA 17.4: Bound check is done by "size_all" */
        /* <-MISRA 17.4 */ /* <-SEC R1.3.1 (1) */
    }


    /* Set "*in_out_StackPointer" */
    *in_out_StackPointer = next_stack_pointer;

    e=0;
fin:
    return  e;
}


/*-------------------------------------------------------------------------*/
/* <<<< ### (window_surfaces_vdc5_layer_t) Class implement >>>> */
/*-------------------------------------------------------------------------*/

/**
* @brief   Initialize <window_surfaces_vdc5_layer_t>
*
* @param   self window_surfaces_vdc5_layer_t
* @param   layer_num layer_num
* @param   graphics_layer_ID graphics_layer_ID
* @return  Error code, 0=No error
*/
static errnum_t  window_surfaces_vdc5_layer_t__initialize(
    window_surfaces_vdc5_layer_t *const  self,
    int_fast32_t const  layer_num,
    vdc5_graphics_type_t const  graphics_layer_ID ); /* QAC-3450 */

static errnum_t  window_surfaces_vdc5_layer_t__initialize(
    window_surfaces_vdc5_layer_t *const  self,
    int_fast32_t const  layer_num,
    vdc5_graphics_type_t const  graphics_layer_ID )
{
    errnum_t  e;

    IF_DQ( self == NULL ) {
        e=E_OTHERS;
        goto fin;
    }

    e= R_int_t_to_int8_t( layer_num, &self->layer_num );
    IF(e!=0) {
        goto fin;
    }

    self->graphics_layer_ID = graphics_layer_ID;

    self->is_data_control = false;

    self->layer_color = GS_DEFAULT_CLEAR_COLOR;

    switch ( graphics_layer_ID ) {
        case  VDC5_GR_TYPE_GR0:
            self->data_control_ID = VDC5_LAYER_ID_0_RD;
            break;
#ifndef  RZ_A1L
        case  VDC5_GR_TYPE_GR1:
            self->data_control_ID = VDC5_LAYER_ID_1_RD;
            break;
#endif
        case  VDC5_GR_TYPE_GR2:
            self->data_control_ID = VDC5_LAYER_ID_2_RD;
            break;
        case  VDC5_GR_TYPE_GR3:
            self->data_control_ID = VDC5_LAYER_ID_3_RD;
            break;
        default:
            e=E_OTHERS;
            goto fin; /* Bad "graphics_layer_ID" */
    }

    e=0;
fin:
    return  e;
}



/*-------------------------------------------------------------------------*/
/* <<<< ### (window_surfaces_t) Class implement >>>> */
/*-------------------------------------------------------------------------*/

/**
* @brief   Get <window_surfaces_vdc5_layer_t>
*
* @param   self window_surfaces_t
* @param   in_LayerNum in_LayerNum
* @param   out_Layer out_Layer
* @return  Error code, 0=No error
*/
STATIC_INLINE errnum_t  window_surfaces_t__get_layer( window_surfaces_t *const  self,
        int_fast32_t const  in_LayerNum,
        window_surfaces_vdc5_layer_t **const  out_Layer );  /* QAC-3450 */
/* ->MISRA 16.7 : "self" can not "const", because "out_Layer" is not "const" */
/* ->SEC M1.11.1 */
STATIC_INLINE errnum_t  window_surfaces_t__get_layer( window_surfaces_t *const  self,
        int_fast32_t const  in_LayerNum,
        window_surfaces_vdc5_layer_t **const  out_Layer )
/* <-MISRA 16.7 */ /* <-SEC M1.11.1 */
{
    errnum_t       e;
    uint_fast32_t  index;

    IF_DQ( self == NULL ) {
        e=E_OTHERS;
        goto fin;
    }
    IF_DQ( out_Layer == NULL ) {
        e=E_OTHERS;
        goto fin;
    }

    index = in_LayerNum - self->layer_num_min;

    IF ( index >= R_COUNT_OF( self->layers ) ) {
        e = E_OTHERS;
        goto fin;
    }

    *out_Layer = &self->layers[ index ];

    e=0;
fin:
    return  e;
}


/**
* @brief   Get <window_surfaces_attribute_t>
*
* @param   self window_surfaces_t
* @param   in_LayerNum in_LayerNum
* @param   out_Attribute out_Attribute
* @return  Error code, 0=No error
*/
#ifdef  IS_WINDOW_SURFACES_EX
STATIC_INLINE errnum_t  window_surfaces_t__get_attribute( window_surfaces_t *const  self,
        int_fast32_t const  in_LayerNum,
        window_surfaces_attribute_t **const  out_Attribute );  /* QAC-3450 */
/* ->MISRA 16.7 : "self" can not "const", because "out_Attribute" is not "const" */
/* ->SEC M1.11.1 */
STATIC_INLINE errnum_t  window_surfaces_t__get_attribute( window_surfaces_t *const  self,
        int_fast32_t const  in_LayerNum,
        window_surfaces_attribute_t **const  out_Attribute )
/* <-MISRA 16.7 */ /* <-SEC M1.11.1 */
{
    errnum_t       e;
    uint_fast32_t  index;

    IF_DQ( self == NULL ) {
        e=E_OTHERS;
        goto fin;
    }
    IF_DQ( out_Attribute == NULL ) {
        e=E_OTHERS;
        goto fin;
    }

    index = in_LayerNum - self->layer_num_min;

    IF ( index >= R_COUNT_OF( self->layers ) ) {
        e = E_OTHERS;
        goto fin;
    }

    *out_Attribute = &self->attributes[ index ];

    e=0;
fin:
    return  e;
}
#endif


/***********************************************************************
* Class: window_surfaces_t
************************************************************************/

/***********************************************************************
* Implement: R_WINDOW_SURFACES_InitConst
************************************************************************/

static bool_t  gs_window_surfaces_t_is_init = false;

void  R_WINDOW_SURFACES_InitConst( window_surfaces_t *const  self )
{
    int_fast32_t  i;

    IF_DQ( self == NULL ) {
        goto fin;
    }

    for ( i = 0;  i < (ssize_t) R_COUNT_OF( self->frame_buffers );  i += 1 ) {
        self->frame_buffers[i].buffer_address[0] = NULL;
    }
    self->is_initialized = gs_window_surfaces_t_is_init;

fin:
    return;
}


/***********************************************************************
* Implement: R_WINDOW_SURFACES_Initialize
************************************************************************/

static void  vdc5_init_func( uint32_t user_num );

errnum_t  R_WINDOW_SURFACES_Initialize( window_surfaces_t *const  self,
                                        window_surfaces_config_t  *in_out_config )
{
    errnum_t                  e;
    errnum_t                  ee;
    vdc5_error_t              error_vdc;
    window_surfaces_config_t  default_config;
    int_fast32_t              layer_num;
    window_surfaces_vdc5_layer_t  *main_layer[ R_COUNT_OF( self->frame_buffers ) ];
    window_surfaces_vdc5_layer_t  *back_layer;

    enum { num_2 = 2,  num_4 = 4 };


    IF_DQ( self == NULL ) {
        e=E_OTHERS;
        goto fin;
    }

    self->screen_channel = GS_CHANNEL_n;


    /* Set default configuration */
    if ( in_out_config == NULL ) {
        in_out_config = &default_config;
        in_out_config->flags = 0;
    }

    if ( IS_BIT_NOT_SET( in_out_config->flags, F_WINDOW_SURFACES_PIXEL_FORMAT ) ) {
        in_out_config->pixel_format = PIXEL_FORMAT_ARGB8888;
        in_out_config->flags |= F_WINDOW_SURFACES_PIXEL_FORMAT;
    }


    if ( IS_BIT_NOT_SET( in_out_config->flags, F_WINDOW_SURFACES_LAYER_COUNT ) ) {
        in_out_config->layer_count = 1;
        in_out_config->flags |= F_WINDOW_SURFACES_LAYER_COUNT;
    }
    ASSERT_D( in_out_config->layer_count >= 1  &&  in_out_config->layer_count <= 2,
              e=E_OTHERS; goto fin );


    if ( IS_BIT_NOT_SET( in_out_config->flags, F_WINDOW_SURFACES_BUFFER_HEIGHT ) ) {
        in_out_config->buffer_height = R_ToSigned( GS_BUFFER_HEIGHT );
        in_out_config->flags |= F_WINDOW_SURFACES_BUFFER_HEIGHT;
    }


    if ( IS_BIT_NOT_SET( in_out_config->flags, F_WINDOW_SURFACES_BACKGROUND_COLOR ) ) {
        in_out_config->background_color = GS_DEFAULT_CLEAR_COLOR;
        in_out_config->flags |= F_WINDOW_SURFACES_BACKGROUND_COLOR;
    }


    if ( IS_BIT_NOT_SET( in_out_config->flags, F_WINDOW_SURFACES_BACKGROUND_FORMAT ) ) {
        in_out_config->background_format = BACKGROUND_FORMAT_SOLID_COLOR;
        in_out_config->flags |= F_WINDOW_SURFACES_BACKGROUND_FORMAT;
    }


    /* Set layers */
#ifndef  RZ_A1L  /* RZ/A1H */
    self->layer_num_min = -1;
    self->layer_num_max =  in_out_config->layer_count - 1;
    ASSERT_D( (ssize_t) R_COUNT_OF( self->layers ) >= self->layer_num_max - self->layer_num_min + 1,
              e=E_OTHERS; goto fin );
    e= window_surfaces_vdc5_layer_t__initialize( &self->layers[0], -1, VDC5_GR_TYPE_GR0 );
    IF(e!=0) {
        goto fin;
    }
    e= window_surfaces_vdc5_layer_t__initialize( &self->layers[1],  0, VDC5_GR_TYPE_GR1 );
    IF(e!=0) {
        goto fin;
    }
    e= window_surfaces_vdc5_layer_t__initialize( &self->layers[2],  1, VDC5_GR_TYPE_GR2 );
    IF(e!=0) {
        goto fin;
    }
#else
    if ( in_out_config->pixel_format != PIXEL_FORMAT_YUV422 ) {
        self->layer_num_min = -1;
        self->layer_num_max =  in_out_config->layer_count - 1;
        ASSERT_D( R_COUNT_OF( self->layers ) >= self->layer_num_max - self->layer_num_min + 1,
                  e=E_OTHERS; goto fin );
        e= window_surfaces_vdc5_layer_t__initialize( &self->layers[0], -1, VDC5_GR_TYPE_GR0 );
        IF(e!=0) {
            goto fin;
        }
        e= window_surfaces_vdc5_layer_t__initialize( &self->layers[1],  0, VDC5_GR_TYPE_GR2 );
        IF(e!=0) {
            goto fin;
        }
        e= window_surfaces_vdc5_layer_t__initialize( &self->layers[2],  1, VDC5_GR_TYPE_GR3 );
        IF(e!=0) {
            goto fin;
        }
    } else { /* PIXEL_FORMAT_YUV422 */
        self->layer_num_min = 0;
        self->layer_num_max = in_out_config->layer_count - 1;
        ASSERT_D( R_COUNT_OF( self->layers ) >= self->layer_num_max - self->layer_num_min + 1,
                  e=E_OTHERS; goto fin );
        e= window_surfaces_vdc5_layer_t__initialize( &self->layers[0],  0, VDC5_GR_TYPE_GR0 );
        IF(e!=0) {
            goto fin;
        }
        e= window_surfaces_vdc5_layer_t__initialize( &self->layers[1],  1, VDC5_GR_TYPE_GR2 );
        IF(e!=0) {
            goto fin;
        }
    }
#endif


    /* Set "main_layer", "back_layer" */
    for ( layer_num = 0;  layer_num <= self->layer_num_max;  layer_num += 1 ) {
        e= window_surfaces_t__get_layer( self,  layer_num,  &main_layer[ layer_num ] );
        IF(e!=0) {
            goto fin;
        }
        IF_DQ( main_layer[ layer_num ] == NULL ) {
            e=E_OTHERS;
            goto fin;
        }
    }

    self->background_format = in_out_config->background_format;
    if ( self->layer_num_min >= 0 ) {
        back_layer = NULL;
        self->background_frame_count = 0;
    } else {
        back_layer = NULL;
        /* NULL is for avoiding warning C417W of mbed cloud compiler */
        e= window_surfaces_t__get_layer( self, -1, &back_layer );
        IF(e!=0) {
            goto fin;
        }
        IF_DQ( back_layer == NULL ) {
            e=E_OTHERS;
            goto fin;
        }

        if ( in_out_config->background_format != BACKGROUND_FORMAT_SOLID_COLOR ) {
            self->background_frame_count = 1;
        } else {
            self->background_frame_count = 0;
        }
    }


    {
        uint8_t   *memory_address;
        size_t     memory_size;


        /* Set "physical_address", ... */
        e= R_Sample_GetBigMemory( &memory_address,  &memory_size );
        IF ( e != 0 ) {
            goto fin;
        }
        /* 64byte alignment */
        ASSERT_R( R_Mod_64u( (uintptr_t) memory_address ) == 0,  e=E_OTHERS; goto fin );


        /* Set "self->start_of_VRAM", ... */
        self->start_of_VRAM = memory_address;
        /* ->MISRA 17.4 */ /* ->SEC R1.3.1 (1) */
        self->over_of_VRAM  = self->start_of_VRAM + memory_size;  /* MISRA 17.4: This is a bound */
        /* <-MISRA 17.4 */ /* <-SEC R1.3.1 (1) */
        self->stack_pointer_of_VRAM = self->start_of_VRAM;
    }


    for ( layer_num = -self->background_frame_count;  layer_num <= self->layer_num_max;  layer_num += 1 ) {
        frame_buffer_t  *frame;
        window_surfaces_vdc5_layer_t  *layer = main_layer[ layer_num ];
        pixel_format_t  pixel_format;

        e= R_WINDOW_SURFACES_GetLayerFrameBuffer( self, layer_num, &frame );
        IF(e) {
            goto fin;
        }

        if ( layer_num <= -1  &&  in_out_config->background_format != BACKGROUND_FORMAT_SOLID_COLOR ) {
            pixel_format = PIXEL_FORMAT_YUV422;
        } else if ( layer_num >= 1  &&  in_out_config->pixel_format == PIXEL_FORMAT_YUV422 ) {
            pixel_format = PIXEL_FORMAT_ARGB4444;
        } else {
            pixel_format = in_out_config->pixel_format;
        }


        /* Set "self->frame_buffers" */
        frame->buffer_count      = num_2;
        frame->show_buffer_index = 0;
        frame->draw_buffer_index = 0;
        frame->width             = R_ToSigned( GS_BUFFER_WIDTH );
        frame->height            = in_out_config->buffer_height;
        frame->pixel_format      = pixel_format;
        frame->delegate          = NULL;

        switch ( frame->pixel_format ) {
            case PIXEL_FORMAT_ARGB8888:
                frame->buffer_count      = 1;
                frame->draw_buffer_index = 0;
                frame->byte_per_pixel    = num_4;
                layer->vdc5_format = VDC5_GR_FORMAT_ARGB8888;
                break;

            case PIXEL_FORMAT_XRGB8888:
                frame->buffer_count      = 1;
                frame->draw_buffer_index = 0;
                frame->byte_per_pixel    = num_4;
                layer->vdc5_format = VDC5_GR_FORMAT_RGB888;
                break;

            case PIXEL_FORMAT_RGB565:
                frame->buffer_count      = num_2;
                frame->draw_buffer_index = 1;
                frame->byte_per_pixel    = num_2;
                layer->vdc5_format = VDC5_GR_FORMAT_RGB565;
                break;

            case PIXEL_FORMAT_ARGB1555:
                frame->buffer_count      = num_2;
                frame->draw_buffer_index = 1;
                frame->byte_per_pixel    = num_2;
                layer->vdc5_format = VDC5_GR_FORMAT_ARGB1555;
                break;

            case PIXEL_FORMAT_ARGB4444:
                frame->buffer_count      = num_2;
                frame->draw_buffer_index = 1;
                frame->byte_per_pixel    = num_2;
                layer->vdc5_format = VDC5_GR_FORMAT_ARGB4444;
                break;

            case PIXEL_FORMAT_YUV422:
                frame->buffer_count      = num_2;
                frame->draw_buffer_index = 1;
                frame->byte_per_pixel    = num_2;
                layer->vdc5_format = VDC5_GR_FORMAT_YCBCR422;
                break;

            case PIXEL_FORMAT_CLUT8:
                frame->buffer_count      = 2;
                frame->draw_buffer_index = 1;
                frame->byte_per_pixel    = R_RGA_BitPerPixelType_To_BytePerPixelType( 8 );
                layer->vdc5_format = VDC5_GR_FORMAT_CLUT8;
                break;

            case PIXEL_FORMAT_CLUT4:
                frame->buffer_count      = 2;
                frame->draw_buffer_index = 1;
                frame->byte_per_pixel    = R_RGA_BitPerPixelType_To_BytePerPixelType( 4 );
                layer->vdc5_format = VDC5_GR_FORMAT_CLUT4;
                break;

            case PIXEL_FORMAT_CLUT1:
                frame->buffer_count      = 2;
                frame->draw_buffer_index = 1;
                frame->byte_per_pixel    = R_RGA_BitPerPixelType_To_BytePerPixelType( 1 );
                layer->vdc5_format = VDC5_GR_FORMAT_CLUT1;
                break;

            default:
                layer->vdc5_format = VDC5_GR_FORMAT_NUM;  /* dummy data */
                break;
        }
        if( R_BYTE_PER_PIXEL_IsInteger( frame->byte_per_pixel ) ) {
            frame->stride = frame->width * frame->byte_per_pixel;
        } else {
            frame->stride = R_Ceil_8s( frame->width *
                                       R_RGA_BytePerPixelType_To_BitPerPixelType( frame->byte_per_pixel ) ) / 8;
            frame->stride = R_Ceil_32s( frame->stride );
        }
        e= alloc_VRAM_stack_sub( &self->stack_pointer_of_VRAM,
                                 self->over_of_VRAM, frame );
        IF(e!=0) {
            goto fin;
        }

#ifndef R_OSPL_NDEBUG
        printf( "Screen %dx%dx%dx%d vdc5_format=%d stride=%d \n address[0]=0x%08X address[1]=0x%08X\n",
                frame->buffer_count, frame->width, frame->height, frame->byte_per_pixel,
                layer->vdc5_format, frame->stride,
                (uintptr_t) frame->buffer_address[0], (uintptr_t) frame->buffer_address[1] );
        /* Cast of "uintptr_t" is for avoiding "format" warning of GNU_ARM */
#endif
    }


    if ( ! self->is_initialized ) {

        /* Call "R_VDC5_Initialize" */
        {
            vdc5_init_t  init;

            init.panel_icksel = GS_LCD_CHn_PANEL_CLK;
            init.panel_dcdr   = GS_LCD_CHn_PANEL_CLK_DIV;
            init.lvds = GRAPHICS_GetLvdsParam( self->screen_channel );

            error_vdc = R_VDC5_Initialize( self->screen_channel, &init,
                                           &vdc5_init_func, (uint32_t) self->screen_channel );
            IF ( error_vdc != VDC5_OK ) {
                e = E_OTHERS;
                goto fin;
            }
        }


        /* Call "R_VDC5_SyncControl" */
        {
            vdc5_sync_ctrl_t  sync_ctrl;

            sync_ctrl.res_vs_sel    = VDC5_ON;                      /* Free-running Vsync ON/OFF */
            sync_ctrl.res_vs_in_sel = VDC5_RES_VS_IN_SEL_SC0;       /* SC_RES_VS_IN_SEL */
            sync_ctrl.res_fv        = (uint16_t)GS_LCD_CHn_SIG_FV;     /* Free-running Vsync period setting */
            sync_ctrl.res_fh        = (uint16_t)GS_LCD_CHn_SIG_FH;     /* Hsync period setting */
            sync_ctrl.res_vsdly     = (uint16_t)0u;                 /* Vsync signal delay control */
            /* Full-screen enable control */
            sync_ctrl.res_f.vs      = (uint16_t)GS_LCD_CHn_DISP_VS;
            sync_ctrl.res_f.vw      = (uint16_t)GS_LCD_CHn_DISP_VW;
            sync_ctrl.res_f.hs      = (uint16_t)GS_LCD_CHn_DISP_HS;
            sync_ctrl.res_f.hw      = (uint16_t)GS_LCD_CHn_DISP_HW;
            sync_ctrl.vsync_cpmpe   = NULL;                         /* Vsync signal compensation */

            error_vdc = R_VDC5_SyncControl( self->screen_channel, &sync_ctrl );
            IF ( error_vdc != VDC5_OK ) {
                e = E_OTHERS;
                goto fin;
            }
        }


        /* Call "R_VDC5_DisplayOutput" */
        {
            vdc5_output_t  output;
            argb8888_t     background_color_;  /* _ is for MISRA 5.6 */

            /* ->QAC 3198 */
            background_color_.Value = DUMMY_INITIAL_VALUE;  /* for avoid ARMCC warning C4017W */
            /* <-QAC 3198 */
            background_color_.u.Alpha = 0;
            background_color_.u.Red   = in_out_config->background_color.u.Red;
            background_color_.u.Green = in_out_config->background_color.u.Green;
            background_color_.u.Blue  = in_out_config->background_color.u.Blue;


            output.tcon_half        = (uint16_t)GS_LCD_CHn_TCON_HALF;  /* TCON reference timing, 1/2fH timing */
            output.tcon_offset      = (uint16_t)GS_LCD_CHn_TCON_OFFSET;/* TCON reference timing, offset Hsync signal timing */
            /* LCD TCON timing setting */
            GRAPHICS_SetLcdTconSettings( self->screen_channel, output.outctrl );
            output.outcnt_lcd_edge  = GS_LCD_CHn_OUT_EDGE;          /* Output phase control of LCD_DATA23 to LCD_DATA0 pin */
            output.out_endian_on    = VDC5_OFF;                     /* Bit endian change ON/OFF control */
            output.out_swap_on      = VDC5_OFF;                     /* B/R signal swap ON/OFF control */
            output.out_format       = GS_LCD_CHn_OUT_FORMAT;        /* LCD output format select */
            output.out_frq_sel      = VDC5_LCD_PARALLEL_CLKFRQ_1;   /* Clock frequency control */
            output.out_dir_sel      = VDC5_LCD_SERIAL_SCAN_FORWARD; /* Scan direction select */
            output.out_phase        = VDC5_LCD_SERIAL_CLKPHASE_0;   /* Clock phase adjustment */
            output.bg_color         = background_color_.Value;

            error_vdc = R_VDC5_DisplayOutput( self->screen_channel, &output );
            IF ( error_vdc != VDC5_OK ) {
                e = E_OTHERS;
                goto fin;
            }

            if ( back_layer != NULL ) {
                back_layer->layer_color = in_out_config->background_color;
            }
        }

        self->is_initialized = true;
    }


    /* Call "R_VDC5_ReadDataControl" */
    {
        vdc5_read_t           read;
        vdc5_width_read_fb_t  read_area;


        for ( layer_num = 0;  layer_num <= self->layer_num_max;  layer_num += 1 ) {
            int_fast32_t     bit_per_pixel;
            frame_buffer_t  *frame;
            window_surfaces_vdc5_layer_t  *layer = main_layer[ layer_num ];

            e= R_WINDOW_SURFACES_GetLayerFrameBuffer( self, layer_num, &frame );
            IF(e) {
                goto fin;
            }


            /* Read data parameter */
            read.gr_ln_off_dir  = VDC5_GR_LN_OFF_DIR_INC;   /* Line offset address direction of the frame buffer */
            read.gr_flm_sel = VDC5_GR_FLM_SEL_FLM_NUM;  /* Selects a frame buffer address setting signal */
            read.gr_imr_flm_inv = VDC5_OFF;                 /* Sets the frame buffer number for distortion correction */
            read.gr_bst_md      = VDC5_BST_MD_32BYTE;       /* Frame buffer burst transfer mode */
            /* ->QAC 0306 */
            read.gr_base        = (void *) frame->buffer_address[ frame->show_buffer_index ]; /* Frame buffer base address */
            /* <-QAC 0306 */
            read.gr_ln_off      = (uint32_t) frame->stride; /* Frame buffer line offset address */
            read.width_read_fb = NULL;             /* width of the image read from frame buffer */
            read.adj_sel = VDC5_OFF;                /* Measures to decrease the influence
		                                                   by folding pixels/lines (ON/OFF) */
            read.gr_format      = layer->vdc5_format;       /* Graphics format of the frame buffer read signal */
            read.gr_ycc_swap = VDC5_GR_YCCSWAP_Y1CRY0CB;
            /* Controls swapping of data read from buffer in the YCbCr422 format */

            bit_per_pixel = R_RGA_BytePerPixelType_To_BitPerPixelType( frame->byte_per_pixel );
            if ( (bit_per_pixel == 32) || (frame->pixel_format == PIXEL_FORMAT_YCbCr422) ) {
                read.gr_rdswa = VDC5_WR_RD_WRSWA_32BIT;
            } else if ( bit_per_pixel == 16 ) {
                read.gr_rdswa = VDC5_WR_RD_WRSWA_32_16BIT;
            } else if ( bit_per_pixel <= 8 ) {
                read.gr_rdswa = VDC5_WR_RD_WRSWA_32_16_8BIT;
            }
            /* Display area */
            read.gr_grc.hs      = (uint16_t) GS_LCD_CHn_DISP_HS;
            read.gr_grc.hw      = (uint16_t) GS_LCD_CHn_DISP_HW;
            read.gr_grc.vs      = (uint16_t) GS_LCD_CHn_DISP_VS;
            if ( GS_BUFFER_HEIGHT < GS_LCD_CHn_DISP_VW ) {
                read.gr_grc.vw      = (uint16_t) GS_BUFFER_HEIGHT;
            } else {
                read.gr_grc.vw      = (uint16_t) GS_LCD_CHn_DISP_VW;
            }

            error_vdc = R_VDC5_ReadDataControl( self->screen_channel, layer->data_control_ID, &read );
            IF ( error_vdc != VDC5_OK ) {
                e = E_OTHERS;
                goto fin;
            }
            layer->is_data_control = true;
        }

        if ( back_layer != NULL ) {
            if ( self->background_format == BACKGROUND_FORMAT_SOLID_COLOR ) {

                /* Set display area for back ground */
                /* "read.gr_base", ... are ignored */

                error_vdc = R_VDC5_ReadDataControl( self->screen_channel, back_layer->data_control_ID, &read );
                IF ( error_vdc != VDC5_OK ) {
                    e = E_OTHERS;
                    goto fin;
                }
            } else {
                frame_buffer_t                *frame;
                window_surfaces_vdc5_layer_t  *layer = back_layer;

                e= R_WINDOW_SURFACES_GetLayerFrameBuffer( self, -1, &frame );
                IF(e) {
                    goto fin;
                }


                /* Read data parameter */
                read.gr_ln_off_dir  = VDC5_GR_LN_OFF_DIR_INC;   /* Line offset address direction of the frame buffer */
                read.gr_flm_sel     = VDC5_GR_FLM_SEL_FLM_NUM;  /* Selects a frame buffer address setting signal */
                read.gr_imr_flm_inv = VDC5_OFF;                 /* Sets the frame buffer number for distortion correction */
                read.gr_bst_md      = VDC5_BST_MD_32BYTE;       /* Frame buffer burst transfer mode */
                /* ->QAC 0306 */
                read.gr_base        = (void *) frame->buffer_address[ frame->show_buffer_index ]; /* Frame buffer base address */
                /* <-QAC 0306 */
                read.gr_ln_off      = (uint32_t) frame->stride; /* Frame buffer line offset address */
                read_area.in_hw     = frame->width;
                read_area.in_vw     = frame->height / 2;
                read.width_read_fb  = &read_area;
                read.adj_sel        = VDC5_ON;
                read.gr_format      = layer->vdc5_format;       /* Graphics format of the frame buffer read signal */
                read.gr_ycc_swap    = VDC5_GR_YCCSWAP_CBY0CRY1;
                /* Controls swapping of data read from buffer in the YCbCr422 format */
                read.gr_rdswa = VDC5_WR_RD_WRSWA_16BIT;

                /* Display area */
                read.gr_grc.hs      = (uint16_t) GS_LCD_CHn_DISP_HS;
                read.gr_grc.hw      = (uint16_t) GS_LCD_CHn_DISP_HW;
                read.gr_grc.vs      = (uint16_t) GS_LCD_CHn_DISP_VS;
                if ( GS_BUFFER_HEIGHT < GS_LCD_CHn_DISP_VW ) {
                    read.gr_grc.vw = (uint16_t) GS_BUFFER_HEIGHT;
                } else {
                    read.gr_grc.vw = (uint16_t) GS_LCD_CHn_DISP_VW;
                }

                error_vdc = R_VDC5_ReadDataControl( self->screen_channel, layer->data_control_ID, &read );
                IF ( error_vdc != VDC5_OK ) {
                    e = E_OTHERS;
                    goto fin;
                }
            }
            back_layer->is_data_control = true;
        }
    }


    /* Call "R_VDC5_AlphaBlending" for ARGB1555 */
    for ( layer_num = 0;  layer_num <= self->layer_num_max;  layer_num += 1 ) {
        frame_buffer_t  *frame;

        e= R_WINDOW_SURFACES_GetLayerFrameBuffer( self, layer_num, &frame );
        IF(e) {
            goto fin;
        }

        if ( frame->pixel_format == PIXEL_FORMAT_ARGB1555 ) {
            static vdc5_alpha_argb1555_t  alpha_for_argb1555 = { 0x00, U8_255 };
            static vdc5_alpha_blending_t  blend = { &alpha_for_argb1555, NULL };

            error_vdc = R_VDC5_AlphaBlending( self->screen_channel,
                                              main_layer[ layer_num ]->data_control_ID,
                                              &blend );
            IF( error_vdc != VDC5_OK ) {
                e=E_OTHERS;
                goto fin;
            }
        }
    }


    /* Call "R_V_SYNC_Initialize" */
    e= R_V_SYNC_Initialize( (int_fast32_t) self->screen_channel );
    IF ( e != 0 ) {
        goto fin;
    }


    /* Wait for avoiding LCD filled white (LCD-KIT-B01) */
    e= R_OSPL_Delay( 80 );
    IF(e) {
        goto fin;
    }


    /* Set graphics display mode */
    {
        vdc5_start_t  start;
        int_fast32_t  i;

        for ( i = 0;  i < (int_fast32_t) R_COUNT_OF( self->vdc5_disp_sel_array );  i += 1 ) {
            self->vdc5_disp_sel_array[ i ] = VDC5_DISPSEL_LOWER;  /* Reset */
        }
        self->vdc5_disp_sel_array[ VDC5_GR_TYPE_GR0 ] = VDC5_DISPSEL_BACK;

        start.gr_disp_sel = self->vdc5_disp_sel_array;
        error_vdc = R_VDC5_StartProcess( self->screen_channel, VDC5_LAYER_ID_ALL, &start );
        IF ( error_vdc != VDC5_OK ) {
            e = E_OTHERS;
            goto fin;
        }
    }

#if  USE_LCD
    R_OSPL_Delay( 2*17 );  /* Synchronize to LCD */
    LCD_SetBacklight( 100 );
#endif


    /* Set "self->attributes" */
    for ( layer_num = self->layer_num_min;  layer_num <=  self->layer_num_max;  layer_num += 1 ) {
        window_surfaces_attribute_t  *attribute = NULL;
        /* NULL is for avoiding warning C417W of mbed cloud compiler */

        e= window_surfaces_t__get_attribute( self, layer_num, &attribute );
        IF(e) {
            goto fin;
        }

        attribute->X = 0;
        attribute->Y = 0;
        attribute->Width  = R_ToSigned( GS_BUFFER_WIDTH );
        attribute->Height = in_out_config->buffer_height;
        attribute->OffsetX = 0;
        attribute->OffsetY = 0;
        attribute->OffsetByte = 0;
    }


    e=0;
fin:
    if ( e != 0 ) {
        if ( self != NULL ) {
            ee= R_WINDOW_SURFACES_Finalize( self, e );
            R_UNREFERENCED_VARIABLE( ee );
        }
    }
    return  e;
}


/**
* @brief   vdc5_init_func
*
* @param   user_num channel
* @return  None
*/
static void  vdc5_init_func( uint32_t const  user_num )
{
    vdc5_channel_t        channel;
    struct st_cpg *const  reg_CPG = R_Get_CPG_Base();

    IF_DQ( reg_CPG == NULL ) {
        goto fin;
    }

    channel = (vdc5_channel_t)user_num;
    if (channel == VDC5_CHANNEL_0) {
        /* Standby control register 9 (STBCR9)
        	b1      ------0-;  MSTP91 : 0 : Video display controller channel 0 & LVDS enable */
        R_DRV_SET_REGISTER_BIT_FIELD( &reg_CPG->STBCR9, STBCR9, MSTP91, false );
    } else {
        /* Standby control register 9 (STBCR9)
        	b1      ------0-;  MSTP91 : 0 : Video display controller channel 0 & LVDS enable
        	b0      -------0;  MSTP90 : 0 : Video display controller channel 1 enable */
        R_DRV_SET_REGISTER_BIT_FIELD( &reg_CPG->STBCR9, STBCR9, MSTP91, false );
        R_DRV_SET_REGISTER_BIT_FIELD( &reg_CPG->STBCR9, STBCR9, MSTP90, false );
    }
    GRAPHICS_SetLcdPanel(channel);

fin:
    return;
}


/***********************************************************************
* Implement: R_WINDOW_SURFACES_Finalize
************************************************************************/

static void  vdc5_quit_func( uint32_t const  user_num );

errnum_t  R_WINDOW_SURFACES_Finalize( window_surfaces_t *const  self,  errnum_t e )
{
    vdc5_error_t  error_vdc;

    IF_DQ( self == NULL ) {
        e=E_OTHERS;
        goto fin;
    }

    e= R_V_SYNC_Finalize( (int_fast32_t) self->screen_channel, e );

    if ( ! self->is_initialized ) {
        error_vdc = R_VDC5_Terminate( self->screen_channel, &vdc5_quit_func,
                                      (uint32_t) self->screen_channel );
        IF ( (error_vdc != VDC5_OK) && (e == 0) ) {
            e = E_OTHERS;
        }
    } else {
        int_fast32_t  i;

        for ( i = -self->background_frame_count;  i <= self->layer_num_max;  i += 1 ) {
            frame_buffer_t  *frame;
            errnum_t         ee;

            ee= R_WINDOW_SURFACES_GetLayerFrameBuffer( self, i, &frame );
            e= R_OSPL_MergeErrNum( e, ee );
            frame->buffer_address[0] = NULL;
        }

        for ( i = self->layer_num_min;  i <=  self->layer_num_max;  i += 1 ) {
            window_surfaces_vdc5_layer_t  *layer = NULL;
            /* NULL is for avoiding warning C417W of mbed cloud compiler */
            errnum_t  ee = window_surfaces_t__get_layer( self, i, &layer );

            IF_DQ ( layer == NULL ) {
                e=E_OTHERS;    /* Same check as "ee" */
                goto fin;
            }

            if ( (ee == 0) && (layer->is_data_control) ) {
                error_vdc = R_VDC5_StopProcess( self->screen_channel,
                                                layer->data_control_ID );
                IF ( (error_vdc != VDC5_OK) && (e == 0) ) {
                    e = E_OTHERS;
                }
                error_vdc = R_VDC5_ReleaseDataControl( self->screen_channel,
                                                       layer->data_control_ID );
                layer->is_data_control = false;
                IF ( (error_vdc != VDC5_OK) && (e == 0) ) {
                    e = E_OTHERS;
                }
            }
        }

#define  R_WINDOW_SURFACES_TERMINATE_VDC5  1  /* 0 or 1 */
#if      R_WINDOW_SURFACES_TERMINATE_VDC5
        self->is_initialized = false;

        error_vdc = R_VDC5_Terminate( self->screen_channel, &vdc5_quit_func,
                                      (uint32_t) self->screen_channel );
        IF ( (error_vdc != VDC5_OK) && (e == 0) ) {
            e = E_OTHERS;
        }
#endif
    }

    gs_window_surfaces_t_is_init = self->is_initialized;

fin:
    return  e;
}


/**
* @brief   vdc5_quit_func
*
* @param   user_num channel
* @return  None
*/
static void  vdc5_quit_func( uint32_t const  user_num )
{
    vdc5_channel_t const  channel = (vdc5_channel_t) user_num;
    struct st_cpg *const  reg_CPG = R_Get_CPG_Base();

    IF_DQ( reg_CPG == NULL ) {
        goto fin;
    }

    if (channel == VDC5_CHANNEL_0) {
        /* If LVDS should remain enabled, the following code should be removed. */
        R_DRV_SET_REGISTER_BIT_FIELD( &reg_CPG->STBCR9, STBCR9, MSTP91, true );
    } else {
        R_DRV_SET_REGISTER_BIT_FIELD( &reg_CPG->STBCR9, STBCR9, MSTP90, true );
    }

fin:
    return;
}


/***********************************************************************
* Implement: R_WINDOW_SURFACES_GetLayerFrameBuffer
************************************************************************/
errnum_t  R_WINDOW_SURFACES_GetLayerFrameBuffer( const window_surfaces_t *const  self,
        int_fast32_t const  layer_num,  frame_buffer_t **const  out_frame_buffer )
{
    errnum_t       e;
    uint_fast32_t  index;

    IF_DQ( out_frame_buffer == NULL ) {
        e=E_OTHERS;
        goto fin;
    }
    IF_DQ( self == NULL ) {
        e=E_OTHERS;
        goto fin;
    }

    index = layer_num + self->background_frame_count;
    IF ( index >= R_COUNT_OF( self->frame_buffers ) ) {
        e = E_OTHERS;
        goto fin;
    }

    *out_frame_buffer = (frame_buffer_t *) &self->frame_buffers[ index ];
    /* Cast is for const */

    e=0;
fin:
    return  e;
}


/***********************************************************************
* Implement: R_WINDOW_SURFACES_SwapBuffers
************************************************************************/
errnum_t  R_WINDOW_SURFACES_SwapBuffers( window_surfaces_t *const  self,
        int_fast32_t const  layer_num,  graphics_t *const  context )
{
    errnum_t  e;
#if GS_MEASURE_GPU_LOAD
    enum {                interval = 60 };
    uint32_t              idle_start_time;
    uint32_t              idle_end_time;
    static  uint32_t      skip_count;
    static  uint32_t      previous_idle_end_time;
    r_ospl_ftimer_spec_t  ts;
#endif


    e= R_GRAPHICS_Finish( context );
    IF(e!=0) {
        goto fin;
    }


#if GS_MEASURE_GPU_LOAD
    e= R_OSPL_FTIMER_InitializeIfNot( &ts );
    IF(e) {
        goto fin;
    }
    idle_start_time = R_OSPL_FTIMER_Get();
#endif


    e= R_WINDOW_SURFACES_SwapBuffers_Sub( self, layer_num );
    IF(e!=0) {
        goto fin;
    }


    e= R_V_SYNC_Wait( self->screen_channel, 1, true );
    IF(e!=0) {
        goto fin;
    }


#if GS_MEASURE_GPU_LOAD
    idle_end_time = R_OSPL_FTIMER_Get();
    if ( previous_idle_end_time != 0 ) {  /* Skip at first */
        skip_count += 1;
        if ( skip_count >= interval ) {
            printf( "GPU %d%% in %dmsec\n",
                    ( idle_start_time - previous_idle_end_time ) * 100 /
                    ( idle_end_time   - previous_idle_end_time ),
                    R_OSPL_FTIMER_CountToTime( &ts,
                                               idle_end_time - previous_idle_end_time ) );
            skip_count = 0;

            /* __heapstats( (__heapprt) fprintf, stdout ); */

            /* R_DEBUG_BREAK(); */ /* This is for measuring CPU load */
        }
    }
    previous_idle_end_time = idle_end_time;
#endif


    e=0;
fin:
    return  e;
}


/***********************************************************************
* Implement: R_WINDOW_SURFACES_SwapBuffersStart
************************************************************************/
errnum_t  R_WINDOW_SURFACES_SwapBuffersStart( window_surfaces_t *const  self,
        int_fast32_t const  layer_num,  r_ospl_async_t *async )
{
    errnum_t  e;


    e= R_WINDOW_SURFACES_SwapBuffers_Sub( self, layer_num );
    IF(e!=0) {
        goto fin;
    }


    e= R_V_SYNC_WaitStart( self->screen_channel, 1, true, async );
    IF(e!=0) {
        goto fin;
    }

    e=0;
fin:
    return  e;
}


/**
* @brief   Sub routine of <R_WINDOW_SURFACES_SwapBuffers>
*
* @param   self window_surfaces_t
* @param   layer_num layer_num
* @return  Error code, 0=No error
*/
static errnum_t  R_WINDOW_SURFACES_SwapBuffers_Sub( window_surfaces_t *const  self,
        int_fast32_t const  layer_num )
{
    errnum_t         e;
    vdc5_error_t     error_vdc;
    frame_buffer_t  *frame;
    window_surfaces_vdc5_layer_t  *layer = NULL;
    /* NULL is for avoiding warning C417W of mbed cloud compiler */


    IF_DQ( self == NULL ) {
        e=E_OTHERS;
        goto fin;
    }

    e= window_surfaces_t__get_layer( self, layer_num, &layer );
    IF(e!=0) {
        goto fin;
    }
    IF_DQ( layer == NULL ) {
        e=E_OTHERS;
        goto fin;
    }


    /* Swap buffer index */
    e= R_WINDOW_SURFACES_GetLayerFrameBuffer( self, layer_num, &frame );
    IF(e) {
        goto fin;
    }
    frame->show_buffer_index = frame->draw_buffer_index;
    frame->draw_buffer_index += 1;
    if ( frame->draw_buffer_index >= (int_t) frame->buffer_count ) {
        frame->draw_buffer_index = 0;
    }


    /* Show the frame buffer */
    {
        vdc5_read_chg_t  config;
        uintptr_t        physical_address;

        window_surfaces_attribute_t   *attribute = NULL;
        /* NULL is for avoiding warning C417W of mbed cloud compiler */

        e= R_OSPL_ToPhysicalAddress( frame->buffer_address[ frame->show_buffer_index ],
                                     &physical_address );
        IF(e!=0) {
            goto fin;
        }

        e= window_surfaces_t__get_attribute( self, layer_num, &attribute );
        IF(e) {
            goto fin;
        }

        if ( attribute->OffsetByte != GS_OFFSET_BYTE_NOT_SHOW ) {
            physical_address += attribute->OffsetByte;

            config.gr_base        = (void *) physical_address;
            config.width_read_fb  = NULL;
            config.gr_grc         = NULL;
            config.gr_disp_sel    = NULL;
            error_vdc = R_VDC5_ChangeReadProcess( self->screen_channel, layer->data_control_ID, &config );
            IF ( error_vdc != VDC5_OK ) {
                e=E_OTHERS;
                goto  fin;
            }
        }
    }


    /* Show the layer */
    {
        vdc5_gr_disp_sel_t  new_value;

        if ( layer_num == 1 ) {
            new_value = VDC5_DISPSEL_BLEND;
        } else if ( layer_num == 0 ) {
            frame_buffer_t  *frame;

            e= R_WINDOW_SURFACES_GetLayerFrameBuffer( self, layer_num, &frame );
            IF(e) {
                goto fin;
            }

            if ( frame->pixel_format == PIXEL_FORMAT_YUV422 ) {
                new_value = VDC5_DISPSEL_CURRENT;
            } else {
                new_value = VDC5_DISPSEL_BLEND;
            }
        } else {
            ASSERT_D( layer_num == -1,  e=E_OTHERS; goto fin );

            if ( self->background_format == BACKGROUND_FORMAT_SOLID_COLOR ) {
                new_value = VDC5_DISPSEL_CURRENT;
            } else {
                new_value = VDC5_DISPSEL_LOWER;  /* "*_LOWER" is "*_CURRENT" for resize */
            }
        }

        if ( self->vdc5_disp_sel_array[ layer->graphics_layer_ID ] != new_value ) {
            vdc5_start_t  start;

            self->vdc5_disp_sel_array[ layer->graphics_layer_ID ] = new_value;

            start.gr_disp_sel = self->vdc5_disp_sel_array;
            error_vdc = R_VDC5_StartProcess( self->screen_channel, VDC5_LAYER_ID_ALL, &start );
            IF ( error_vdc != VDC5_OK ) {
                e = E_OTHERS;
                goto fin;
            }
        }
    }

    e=0;
fin:
    return  e;
}


/***********************************************************************
* Implement: R_WINDOW_SURFACES_WaitForVSync
************************************************************************/
errnum_t  R_WINDOW_SURFACES_WaitForVSync( window_surfaces_t *const  self,
        int_fast32_t const  swap_interval,  bool_t const  is_1_v_sync_at_minimum )
{
    return  R_V_SYNC_Wait( self->screen_channel, swap_interval, is_1_v_sync_at_minimum );
}


/**
* @brief   R_WINDOW_SURFACES_AllocOffscreenStack
*
* @par Parameters
*    None
* @return  None.
*/
errnum_t  R_WINDOW_SURFACES_AllocOffscreenStack( window_surfaces_t *const  self,
        frame_buffer_t *const  in_out_frame_buffer )
{
    errnum_t  e;

    IF_DQ( self == NULL ) {
        e=E_OTHERS;
        goto fin;
    }

    e= alloc_VRAM_stack_sub( &self->stack_pointer_of_VRAM,  self->over_of_VRAM,
                             in_out_frame_buffer );
    IF(e!=0) {
        goto fin;
    }

    e=0;
fin:
    return  e;
}


/**
* @brief   R_WINDOW_SURFACES_FreeOffscreenStack
*
* @par Parameters
*    None
* @return  None.
*/
errnum_t  R_WINDOW_SURFACES_FreeOffscreenStack( window_surfaces_t *const  self,
        const frame_buffer_t *const  frame_buffer )
{
    errnum_t  e;

    IF_DQ( self == NULL ) {
        e=E_OTHERS;
        goto fin;
    }

    e= free_VRAM_stack_sub( &self->stack_pointer_of_VRAM, frame_buffer,
                            self->start_of_VRAM );
    IF(e!=0) {
        goto fin;
    }

    e=0;
fin:
    return  e;
}


/***********************************************************************
* Implement: R_WINDOW_SURFACES_DoMessageLoop
************************************************************************/
errnum_t  R_WINDOW_SURFACES_DoMessageLoop( window_surfaces_t *self )
{
    R_UNREFERENCED_VARIABLE( self );

    R_OSPL_Delay( 2000 );

    return  0;
}


/***********************************************************************
* Implement: R_WINDOW_SURFACES_AccessLayerAttributes
************************************************************************/
errnum_t  R_WINDOW_SURFACES_AccessLayerAttributes( window_surfaces_t *self,
        layer_attributes_t *in_out_Attributes )
{
    errnum_t         e;
    vdc5_error_t     error_vdc;
    window_surfaces_vdc5_layer_t  *layer = NULL;

    if ( IS_BIT_SET( in_out_Attributes->flags, F_LAYER_ID ) ) {
        e= window_surfaces_t__get_layer( self, in_out_Attributes->id, &layer );
        IF(e) {
            goto fin;
        }
    }

    ASSERT_R( IS_ALL_BITS_NOT_SET( in_out_Attributes->access, ~ACCESS_ALL_MASK ),  e=E_OTHERS; goto fin );


    if ( in_out_Attributes->flags & F_LAYER_LAYER_COLOR ) {
        vdc5_output_t  output;
        vdc5_read_t    read;
        r8g8b8a8_t     previous_layer_color = { {0,0,0,0} };
        /* Avoid Warning: C4017W: previous_layer_color may be used before being set */
        enum { necessary_flags = F_LAYER_ID };


        ASSERT_R( IS_ALL_BITS_SET( in_out_Attributes->flags, necessary_flags ),
                  e=E_OTHERS; goto fin );
        ASSERT_R( IS_ALL_BITS_NOT_SET( in_out_Attributes->access, ~(ACCESS_READ | ACCESS_WRITE) ),
                  e=E_OTHERS; goto fin );


        if ( IS_BIT_SET( in_out_Attributes->access, ACCESS_READ ) ) {
            previous_layer_color = layer->layer_color;
        }


        if ( IS_BIT_SET( in_out_Attributes->access, ACCESS_WRITE ) ) {

            if ( IS_BIT_SET( in_out_Attributes->flags, F_LAYER_LAYER_COLOR ) ) {

                /* Set "output.bg_color" to "R_VDC5_DisplayOutput" */
                output.tcon_half        = (uint16_t)GS_LCD_CHn_TCON_HALF;  /* TCON reference timing, 1/2fH timing */
                output.tcon_offset      = (uint16_t)GS_LCD_CHn_TCON_OFFSET;/* TCON reference timing, offset Hsync signal timing */
                GRAPHICS_SetLcdTconSettings( self->screen_channel, output.outctrl );
                output.outcnt_lcd_edge  = GS_LCD_CHn_OUT_EDGE;          /* Output phase control of LCD_DATA23 to LCD_DATA0 pin */
                output.out_endian_on    = VDC5_OFF;                     /* Bit endian change ON/OFF control */
                output.out_swap_on      = VDC5_OFF;                     /* B/R signal swap ON/OFF control */
                output.out_format       = GS_LCD_CHn_OUT_FORMAT;        /* LCD output format select */
                output.out_frq_sel      = VDC5_LCD_PARALLEL_CLKFRQ_1;   /* Clock frequency control */
                output.out_dir_sel      = VDC5_LCD_SERIAL_SCAN_FORWARD; /* Scan direction select */
                output.out_phase        = VDC5_LCD_SERIAL_CLKPHASE_0;   /* Clock phase adjustment */
                output.bg_color         =
                    ( in_out_Attributes->layer_color.u.Red << 16 ) |
                    ( in_out_Attributes->layer_color.u.Green << 8 ) |
                    ( in_out_Attributes->layer_color.u.Blue << 0 );  /* 24-bit RGB color format */

                error_vdc = R_VDC5_DisplayOutput( self->screen_channel, &output );
                IF ( error_vdc != VDC5_OK ) {
                    e = E_OTHERS;
                    goto fin;
                }


                /* Update ReadDataControl */

                /* Release */
                if ( layer->is_data_control ) {
                    error_vdc = R_VDC5_StopProcess( self->screen_channel, layer->data_control_ID );
                    IF ( error_vdc != VDC5_OK ) {
                        R_NOOP();
                    }
                    error_vdc = R_VDC5_ReleaseDataControl( self->screen_channel, layer->data_control_ID );
                    IF ( error_vdc != VDC5_OK ) {
                        e=E_OTHERS;
                        goto fin;
                    }
                    layer->is_data_control = false;
                }

                /* Read data parameter : These values are dummy */
                read.gr_ln_off_dir  = VDC5_GR_LN_OFF_DIR_INC;   /* Line offset address direction of the frame buffer */
                read.gr_flm_sel     = VDC5_GR_FLM_SEL_FLM_NUM;  /* Selects a frame buffer address setting signal */
                read.gr_imr_flm_inv = VDC5_OFF;                 /* Sets the frame buffer number for distortion correction */
                read.gr_bst_md      = VDC5_BST_MD_32BYTE;       /* Frame buffer burst transfer mode */
                read.gr_base        = (void *) &read;           /* Dummy, Frame buffer base address */
                read.gr_ln_off      = 3200;                     /* Dummy, Frame buffer line offset address */
                read.width_read_fb  = NULL;                     /* width of the image read from frame buffer */
                read.adj_sel        = VDC5_OFF;                 /* Measures to decrease the influence
				                                                   by folding pixels/lines (ON/OFF) */
                read.gr_format      = VDC5_GR_FORMAT_RGB888;    /* Graphics format of the frame buffer read signal */
                read.gr_ycc_swap    = VDC5_GR_YCCSWAP_Y1CRY0CB; /* Controls swapping of data read from buffer
				                                                   in the YCbCr422 format */
                read.gr_rdswa       = VDC5_WR_RD_WRSWA_32BIT;   /* for 32 bit format */

                /* Set bakcground color area */
                read.gr_grc.hs      = GS_LCD_CHn_DISP_HS;
                read.gr_grc.hw      = (uint16_t) GS_LCD_CHn_DISP_HW;
                read.gr_grc.vs      = GS_LCD_CHn_DISP_VS;
                if ( GS_BUFFER_HEIGHT < GS_LCD_CHn_DISP_VW ) {
                    read.gr_grc.vw      = (uint16_t) GS_BUFFER_HEIGHT;
                } else {
                    read.gr_grc.vw      = GS_LCD_CHn_DISP_VW;
                }

                /* Start to read again */
                error_vdc = R_VDC5_ReadDataControl( self->screen_channel, layer->data_control_ID, &read );
                IF ( error_vdc != VDC5_OK ) {
                    e = E_OTHERS;
                    goto fin;
                }
                layer->is_data_control = true;
            }


            /* Set graphics display mode */
            {
                vdc5_start_t  start;

                start.gr_disp_sel = self->vdc5_disp_sel_array;
                error_vdc = R_VDC5_StartProcess( self->screen_channel, VDC5_LAYER_ID_ALL, &start );
                IF ( error_vdc != VDC5_OK ) {
                    e = E_OTHERS;
                    goto fin;
                }
            }
        }

        if ( IS_BIT_SET( in_out_Attributes->access, ACCESS_READ ) ) {
            in_out_Attributes->layer_color = previous_layer_color;
        }
    }


    /* Set window position */
    {
        enum { flags_of_window_position =
                   F_LAYER_X | F_LAYER_Y | F_LAYER_WIDTH | F_LAYER_HEIGHT |
                   F_LAYER_OFFSET_X | F_LAYER_OFFSET_Y
             };

        if ( IS_ANY_BITS_SET( in_out_Attributes->flags, flags_of_window_position )  &&
                IS_BIT_SET( in_out_Attributes->access, ACCESS_WRITE ) ) {
            vdc5_read_chg_t          change;
            vdc5_width_read_fb_t     change_source_rectangle;
            vdc5_period_rect_t       change_destination_rectangle;
            gs_frame_width_height_t  source_frame;
            gs_frame_width_height_t  destination_frame;
            gs_rectangle_t           source_rectangle;
            gs_rectangle_t           destination_rectangle;
            frame_buffer_t          *frame;
            bool_t                   is_interlace;
            bool_t                   is_solid_color;
            bool_t                   is_show = true;
            window_surfaces_attribute_t  *attribute = NULL;
            /* NULL is for avoiding warning C417W of mbed cloud compiler */


            ASSERT_R( IS_ALL_BITS_SET( in_out_Attributes->flags, F_LAYER_ID ), e=E_OTHERS; goto fin );

            e= window_surfaces_t__get_attribute( self, in_out_Attributes->id, &attribute );
            IF(e) {
                goto fin;
            }

            is_interlace = ( self->background_format == BACKGROUND_FORMAT_VIDEO_INTERLACE  &&
                             in_out_Attributes->id == -1 );

            is_solid_color = ( self->background_format == BACKGROUND_FORMAT_SOLID_COLOR  &&
                               in_out_Attributes->id == -1 );

            change.gr_base = NULL;
            change.width_read_fb = NULL;
            change.gr_grc = NULL;
            change.gr_disp_sel = NULL;

            if ( IS_BIT_SET( in_out_Attributes->flags, F_LAYER_X ) ) {
                attribute->X = in_out_Attributes->x;
                change.gr_grc = &change_destination_rectangle;
            }
            if ( IS_BIT_SET( in_out_Attributes->flags, F_LAYER_Y ) ) {
                attribute->Y = in_out_Attributes->y;
                change.gr_grc = &change_destination_rectangle;
            }
            if ( IS_BIT_SET( in_out_Attributes->flags, F_LAYER_WIDTH ) ) {
                attribute->Width = in_out_Attributes->width;
                change.gr_grc = &change_destination_rectangle;
            }
            if ( IS_BIT_SET( in_out_Attributes->flags, F_LAYER_HEIGHT ) ) {
                attribute->Height = in_out_Attributes->height;
                change.gr_grc = &change_destination_rectangle;
            }
            if ( IS_BIT_SET( in_out_Attributes->flags, F_LAYER_OFFSET_X ) ) {
                attribute->OffsetX = in_out_Attributes->offset_x;
                change.gr_grc = &change_destination_rectangle;
            }
            if ( IS_BIT_SET( in_out_Attributes->flags, F_LAYER_OFFSET_Y ) ) {
                attribute->OffsetY = in_out_Attributes->offset_y;
                change.gr_grc = &change_destination_rectangle;
            }


            /* Call "GS_ToInFrame" */
            if ( is_solid_color ) {
                e= R_WINDOW_SURFACES_GetLayerFrameBuffer( self, 0, &frame );
                IF(e) {
                    goto fin;
                }
            } else {
                e= R_WINDOW_SURFACES_GetLayerFrameBuffer( self, in_out_Attributes->id, &frame );
                IF(e) {
                    goto fin;
                }
            }
            source_frame.Width  = frame->width;
            source_frame.Height = frame->height;
            destination_frame.Width  = frame->width;
            destination_frame.Height = frame->height;
            source_rectangle.Left   = attribute->OffsetX;
            source_rectangle.Top    = attribute->OffsetY;
            source_rectangle.Right  = attribute->OffsetX + attribute->Width;
            source_rectangle.Bottom = attribute->OffsetY + attribute->Height;
            destination_rectangle.Left   = attribute->X;
            destination_rectangle.Top    = attribute->Y;
            destination_rectangle.Right  = attribute->X + attribute->Width;
            destination_rectangle.Bottom = attribute->Y + attribute->Height;

            e= GS_ToInFrame( &source_frame, &destination_frame,
                             &source_rectangle, &destination_rectangle );
            IF(e) {
                goto fin;
            }
            if ( source_rectangle.Left > source_rectangle.Right ) {
                is_show = false;
            }


            /* Call "R_VDC5_ChangeReadProcess" */
            if ( is_show ) {
                byte_t       *source_address;
                int_fast32_t  offset_byte;

                ASSERT_R( ! is_solid_color,  e=E_OTHERS; goto fin );

                if ( is_interlace ) {
                    source_address = frame->buffer_address[0];
                    offset_byte = ( source_rectangle.Top / 2 ) * frame->stride;
                } else {
                    source_address = frame->buffer_address[ frame->show_buffer_index ];
                    offset_byte = source_rectangle.Top * frame->stride;
                }
                offset_byte += source_rectangle.Left * frame->byte_per_pixel;
                ASSERT_R( R_BYTE_PER_PIXEL_IsInteger( frame->byte_per_pixel ),
                          e=E_OTHERS; goto fin );
                attribute->OffsetByte = offset_byte;

                change.gr_base = source_address + attribute->OffsetByte;
            }
            if ( change.gr_grc != NULL ) {
                change_destination_rectangle.hs = destination_rectangle.Left + GS_LCD_CHn_DISP_HS;
                change_destination_rectangle.vs = destination_rectangle.Top  + GS_LCD_CHn_DISP_VS;
                change_destination_rectangle.hw = destination_rectangle.Right - destination_rectangle.Left + 1;
                change_destination_rectangle.vw = destination_rectangle.Bottom - destination_rectangle.Top + 1;

                change.width_read_fb = &change_source_rectangle;
                change_source_rectangle.in_hw = change_destination_rectangle.hw;
                change_source_rectangle.in_vw = change_destination_rectangle.vw;

                if ( is_interlace ) {
                    change_source_rectangle.in_vw /= 2;
                }
            }

            if ( change_source_rectangle.in_hw < 3  ||  change_source_rectangle.in_vw == 0 ) {
                is_show = false;
            }

            if ( ! is_show ) {
                change_destination_rectangle.hs = GS_LCD_CHn_DISP_HS - 2;  /* out of visible */
                change_destination_rectangle.vs = GS_LCD_CHn_DISP_VS;
                change_destination_rectangle.hw = 3;
                change_destination_rectangle.vw = 1;
                change_source_rectangle.in_hw = 3;
                change_source_rectangle.in_vw = 1;
                attribute->OffsetByte = GS_OFFSET_BYTE_NOT_SHOW;
            }

            error_vdc = R_VDC5_ChangeReadProcess(
                            self->screen_channel, layer->data_control_ID, &change );
            IF ( error_vdc != VDC5_OK ) {
                e = E_OTHERS;
                goto fin;
            }
        }
        if ( IS_ANY_BITS_SET( in_out_Attributes->flags, flags_of_window_position )  &&
                IS_BIT_SET( in_out_Attributes->access, ACCESS_READ ) ) {
            window_surfaces_attribute_t  *attribute = NULL;
            /* NULL is for avoiding warning C417W of mbed cloud compiler */

            ASSERT_R( IS_ALL_BITS_SET( in_out_Attributes->flags, F_LAYER_ID ), e=E_OTHERS; goto fin );

            e= window_surfaces_t__get_attribute( self, in_out_Attributes->id, &attribute );
            IF(e) {
                goto fin;
            }

            if ( IS_BIT_SET( in_out_Attributes->flags, F_LAYER_X ) ) {
                in_out_Attributes->x = attribute->X;
            }
            if ( IS_BIT_SET( in_out_Attributes->flags, F_LAYER_Y ) ) {
                in_out_Attributes->y = attribute->Y;
            }
            if ( IS_BIT_SET( in_out_Attributes->flags, F_LAYER_WIDTH ) ) {
                in_out_Attributes->width = attribute->Width;
            }
            if ( IS_BIT_SET( in_out_Attributes->flags, F_LAYER_HEIGHT ) ) {
                in_out_Attributes->height = attribute->Height;
            }
            if ( IS_BIT_SET( in_out_Attributes->flags, F_LAYER_OFFSET_X ) ) {
                in_out_Attributes->offset_x = attribute->OffsetX;
            }
            if ( IS_BIT_SET( in_out_Attributes->flags, F_LAYER_OFFSET_Y ) ) {
                in_out_Attributes->offset_y = attribute->OffsetY;
            }
        }
    }


    /* Call "R_VDC5_CLUT" */
    {
        enum { flags_of_CLUT = F_LAYER_CLUT | F_LAYER_CLUT_COUNT };

        if ( IS_ANY_BITS_SET( in_out_Attributes->flags, flags_of_CLUT ) ) {
            vdc5_clut_t  clut_data;
            enum { necessary_flags = flags_of_CLUT | F_LAYER_ID };

            ASSERT_R( IS_ALL_BITS_SET( in_out_Attributes->flags, necessary_flags ),
                      e=E_OTHERS; goto fin );
            ASSERT_R( IS_ANY_BITS_NOT_SET(
                          in_out_Attributes->access,  ~ (bit_flags_fast32_t) ACCESS_WRITE ),
                      e=E_OTHERS; goto fin );

            clut_data.color_num = in_out_Attributes->CLUT_count;
            clut_data.clut = (uint32_t *)( in_out_Attributes->CLUT );
            error_vdc = R_VDC5_CLUT( self->screen_channel, layer->data_control_ID, &clut_data );
            IF( error_vdc != VDC5_OK ) {
                e=E_OTHERS;
                goto fin;
            }
        }
    }

    IF( IS_ANY_BITS_SET( in_out_Attributes->flags,
                         ~( F_LAYER_ID | F_LAYER_LAYER_COLOR | F_LAYER_CLUT_COUNT | F_LAYER_CLUT |
                            F_LAYER_X | F_LAYER_Y | F_LAYER_WIDTH | F_LAYER_HEIGHT |
                            F_LAYER_OFFSET_X | F_LAYER_OFFSET_Y ) ) ) {
        e=E_LIMITATION;
        goto fin;
    }

    e=0;
fin:
    return  e;
}


/* Section: Global */
/**
* @brief   Put "SourceRect" and "DestinationRect" into the frame.
*
* @param   SourceFrameBuffer gs_frame_width_height_t
* @param   DestinationFrameBuffer gs_frame_width_height_t
* @param   SourceRect gs_rectangle_t
* @param   DestinationRect gs_rectangle_t
* @return  Error Code. 0=No Error.
*
* @par Description
*    - If out of frame then "SourceRect - >Left > SourceRect->Right".
*/
errnum_t  GS_ToInFrame(
    gs_frame_width_height_t *SourceFrameBuffer,
    gs_frame_width_height_t *DestinationFrameBuffer,
    gs_rectangle_t *SourceRect,
    gs_rectangle_t *DestinationRect )
{
    errnum_t  e;
    int  src_top,  src_bottom,  dst_top,  dst_bottom;  /* top down coord */
    int  diff;


    /* fast culling by out of frame buffer horizontal */
    if (      SourceRect->Left >= SourceFrameBuffer->Width ||
              DestinationRect->Left >= DestinationFrameBuffer->Width ||
              SourceRect->Right < 0 ||
              DestinationRect->Right < 0 ) {
        SourceRect->Right = SourceRect->Left - 1;  /* out of frame */
        e = 0;
        goto fin;
    }


    /* set "src_top", "src_bottom", "dst_top", "dst_bottom" */
    src_top    = SourceRect->Top;
    src_bottom = SourceRect->Bottom;
    dst_top    = DestinationRect->Top;
    dst_bottom = DestinationRect->Bottom;


    /* fast culling by out of frame buffer vertical */
    if ( src_top >= SourceFrameBuffer->Height ||
            dst_top >= DestinationFrameBuffer->Height ||
            src_bottom < 0 ||
            dst_bottom < 0 ) {
        SourceRect->Right = SourceRect->Left - 1;  /* out of frame */
        e = 0;
        goto fin;
    }


    /* set top >= 0 */
    if ( src_top < 0 ) {
        dst_top += - src_top;
        src_top = 0;
    }

    if ( dst_top < 0 ) {
        src_top += - dst_top;
        dst_top = 0;
    }


    /* set bottom < frame height */
    if ( src_bottom >= SourceFrameBuffer->Height ) {
        diff = src_bottom - SourceFrameBuffer->Height + 1;
        src_bottom -= diff;
        dst_bottom -= diff;
    }

    if ( dst_bottom >= DestinationFrameBuffer->Height ) {
        diff = dst_bottom - DestinationFrameBuffer->Height + 1;
        src_bottom -= diff;
        dst_bottom -= diff;
    }


    /* culling by out of frame buffer vertical for modified top and bottom */
    if ( src_top < 0 ||
            dst_top < 0 ||
            src_bottom >= SourceFrameBuffer->Height ||
            dst_bottom >= DestinationFrameBuffer->Height ) {
        SourceRect->Right = SourceRect->Left - 1;  /* out of frame */
        e = 0;
        goto fin;
    }


    /* return from "src_top", "src_bottom", "dst_top", "dst_bottom" */
    SourceRect->Top    = src_top;
    SourceRect->Bottom = src_bottom;
    DestinationRect->Top    = dst_top;
    DestinationRect->Bottom = dst_bottom;


    /* set left >= 0 */
    if ( SourceRect->Left < 0 ) {
        DestinationRect->Left += - SourceRect->Left;
        SourceRect->Left = 0;
    }

    if ( DestinationRect->Left < 0 ) {
        SourceRect->Left += - DestinationRect->Left;
        DestinationRect->Left = 0;
    }


    /* set right < frame width */
    if ( SourceRect->Right >= SourceFrameBuffer->Width ) {
        diff = SourceRect->Right - SourceFrameBuffer->Width + 1;
        SourceRect->Right      -= diff;
        DestinationRect->Right -= diff;
    }

    if ( DestinationRect->Right >= DestinationFrameBuffer->Width ) {
        diff = DestinationRect->Right - DestinationFrameBuffer->Width + 1;
        SourceRect->Right      -= diff;
        DestinationRect->Right -= diff;
    }


    /* culling by out of frame buffer horizontal for modified top and bottom */
    if (      SourceRect->Left >= SourceFrameBuffer->Width ||
              DestinationRect->Left >= DestinationFrameBuffer->Width ||
              SourceRect->Right < 0 ||
              DestinationRect->Right < 0 ) {
        SourceRect->Right = SourceRect->Left - 1;  /* out of frame */
        e = 0;
        goto fin;
    }

    e=0;
fin:
    return  e;
}


/***********************************************************************
* Class: vram_ex_stack_t
************************************************************************/

/***********************************************************************
* Implement: R_VRAM_EX_STACK_Initialize
************************************************************************/
errnum_t  R_VRAM_EX_STACK_Initialize( vram_ex_stack_t *self, void *NullConfig )
{
    errnum_t  e;
    uint8_t  *address;
    size_t    size;

    R_UNREFERENCED_VARIABLE( NullConfig );

    e= R_EXRAM_GetBigMemory( &address, &size );
    IF(e) {
        goto fin;
    }

    self->Start = address;
    self->Over  = address + size;
    self->StackPointer = self->Start;

    e=0;
fin:
    return  e;
}


/***********************************************************************
* Implement: R_VRAM_EX_STACK_Alloc
************************************************************************/
errnum_t  R_VRAM_EX_STACK_Alloc( vram_ex_stack_t *self, frame_buffer_t *in_out_FrameBuffer )
{
    return  alloc_VRAM_stack_sub( &self->StackPointer, self->Over, in_out_FrameBuffer );
}


/***********************************************************************
* Implement: R_VRAM_EX_STACK_Free
************************************************************************/
errnum_t  R_VRAM_EX_STACK_Free( vram_ex_stack_t *self, frame_buffer_t *frame_buffer )
{
    return  free_VRAM_stack_sub( &self->StackPointer, frame_buffer, self->Start );
}


