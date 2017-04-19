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
* Copyright (C) 2014 - 2015 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**
* @file  window_surfaces_cpp.cpp
* @brief   $Module: CLibCommon $ $PublicVersion: 1.20 $ (=CLIB_VERSION)
* $Rev: 47 $
* $Date:: 2014-01-06 18:18:48 +0900#$
* - Description: Window Surfaces for mbed style C++ API
*/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include  "window_surfaces.hpp"
#include  "window_surfaces.h"


/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
#define  GS_DEFAULT_INT_VALUE  0x7FFFFFFF
#define  GS_DEFAULT_HEIGHT  -1
#define  GS_DEFAULT_CLEAR_COLOR  0x00000100


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/


/***********************************************************************
* ClassImplement: WindowSurfacesClass
************************************************************************/

/***********************************************************************
* Implement: WindowSurfacesClass
************************************************************************/
WindowSurfacesClass::WindowSurfacesClass()
{
    this->_self = NULL;
}


/***********************************************************************
* Implement: ~WindowSurfacesClass
************************************************************************/
WindowSurfacesClass::~WindowSurfacesClass()
{
    this->destroy();
}


/***********************************************************************
* Implement: initialize
************************************************************************/
errnum_t  WindowSurfacesClass::initialize( WindowSurfacesConfigClass &in_out_config )
{
    this->_self = new  window_surfaces_t;
    ASSERT_R( this->_self != NULL,  return  E_FEW_MEMORY );

    in_out_config.flags =
        F_WINDOW_SURFACES_PIXEL_FORMAT |
        F_WINDOW_SURFACES_LAYER_COUNT |
        F_WINDOW_SURFACES_BACKGROUND_COLOR |
        F_WINDOW_SURFACES_BUFFER_HEIGHT;

    if ( in_out_config.buffer_height == GS_DEFAULT_HEIGHT ) {
        in_out_config.flags &= ~F_WINDOW_SURFACES_BUFFER_HEIGHT;
    }
    if ( in_out_config.background_color.Value == GS_DEFAULT_CLEAR_COLOR ) {
        in_out_config.flags &= ~F_WINDOW_SURFACES_BACKGROUND_COLOR;
    }

    R_WINDOW_SURFACES_InitConst( this->_self );

    return  R_WINDOW_SURFACES_Initialize( this->_self,
                                          (window_surfaces_config_t *) &in_out_config );
}


/***********************************************************************
* Implement: destroy
************************************************************************/
void  WindowSurfacesClass::destroy()
{
    if ( this->_self != NULL ) {
        errnum_t  e = R_WINDOW_SURFACES_Finalize( this->_self,  0 );
        ASSERT_R( e == 0,  R_NOOP() );
    }
}


/***********************************************************************
* Implement: get_layer_frame_buffer
************************************************************************/
errnum_t  WindowSurfacesClass::get_layer_frame_buffer( int_fast32_t const  layer_num,  frame_buffer_t **const  out_frame_buffer )
{
    return  R_WINDOW_SURFACES_GetLayerFrameBuffer( this->_self,  layer_num,  out_frame_buffer );
}


/***********************************************************************
* Implement: swap_buffers
************************************************************************/
#ifdef  IS_WINDOW_SURFACES_EX
errnum_t  WindowSurfacesClass::swap_buffers( int_fast32_t const  layer_num,  Canvas2D_ContextClass  &context )
{
    return  R_WINDOW_SURFACES_SwapBuffers( this->_self,  layer_num,  context.c_LanguageContext );
}
#else
errnum_t  WindowSurfacesClass::swap_buffers( int_fast32_t const  layer_num,  const void *const  null_context )
{
    return  R_WINDOW_SURFACES_SwapBuffers( this->_self,  layer_num,  null_context );
}
#endif


/***********************************************************************
* Implement: swap_buffers_start
************************************************************************/
errnum_t  WindowSurfacesClass::swap_buffers_start( int_fast32_t const  layer_num,  r_ospl_async_t *async )
{
    return  R_WINDOW_SURFACES_SwapBuffersStart( this->_self,  layer_num,  async );
}


/***********************************************************************
* Implement: wait_for_v_sync
************************************************************************/
errnum_t  WindowSurfacesClass::wait_for_v_sync( int_fast32_t const  swap_interval,  bool_t const  is_1_v_sync_at_minimum )
{
    return  R_WINDOW_SURFACES_WaitForVSync( this->_self,  swap_interval,  is_1_v_sync_at_minimum );
}


/***********************************************************************
* Implement: alloc_offscreen_stack
************************************************************************/
errnum_t  WindowSurfacesClass::alloc_offscreen_stack( frame_buffer_t *const  in_out_frame_buffer )
{
    return  R_WINDOW_SURFACES_AllocOffscreenStack( this->_self,  in_out_frame_buffer );
}


/***********************************************************************
* Implement: free_offscreen_stack
************************************************************************/
errnum_t  WindowSurfacesClass::free_offscreen_stack( const frame_buffer_t *const  frame_buffer )
{
    return  R_WINDOW_SURFACES_FreeOffscreenStack( this->_self,  frame_buffer );
}


/***********************************************************************
* Implement: do_message_loop
************************************************************************/
#ifdef  IS_WINDOW_SURFACES_EX
errnum_t  WindowSurfacesClass::do_message_loop()
{
    return  R_WINDOW_SURFACES_DoMessageLoop( this->_self );
}
#endif


/***********************************************************************
* Implement: access_layer_attributes
************************************************************************/
#ifdef  IS_WINDOW_SURFACES_EX
errnum_t  WindowSurfacesClass::access_layer_attributes( LayerAttributes &in_out_Attributes )
{
    in_out_Attributes.flags =
        F_LAYER_ID |
        F_LAYER_PRIORITY |
        F_LAYER_X |
        F_LAYER_Y |
        F_LAYER_WIDTH |
        F_LAYER_HEIGHT |
        F_LAYER_OFFSET_X |
        F_LAYER_OFFSET_Y |
        F_LAYER_CLUT |
        F_LAYER_CLUT_COUNT;

    if ( in_out_Attributes.id == GS_DEFAULT_INT_VALUE ) {
        in_out_Attributes.flags &= ~F_LAYER_ID;
    }
    if ( in_out_Attributes.priority == GS_DEFAULT_INT_VALUE ) {
        in_out_Attributes.flags &= ~F_LAYER_PRIORITY;
    }
    if ( in_out_Attributes.x == GS_DEFAULT_INT_VALUE ) {
        in_out_Attributes.flags &= ~F_LAYER_X;
    }
    if ( in_out_Attributes.y == GS_DEFAULT_INT_VALUE ) {
        in_out_Attributes.flags &= ~F_LAYER_Y;
    }
    if ( in_out_Attributes.width == GS_DEFAULT_INT_VALUE ) {
        in_out_Attributes.flags &= ~F_LAYER_WIDTH;
    }
    if ( in_out_Attributes.height == GS_DEFAULT_INT_VALUE ) {
        in_out_Attributes.flags &= ~F_LAYER_HEIGHT;
    }
    if ( in_out_Attributes.offset_x == GS_DEFAULT_INT_VALUE ) {
        in_out_Attributes.flags &= ~F_LAYER_OFFSET_X;
    }
    if ( in_out_Attributes.offset_y == GS_DEFAULT_INT_VALUE ) {
        in_out_Attributes.flags &= ~F_LAYER_OFFSET_Y;
    }
    if ( in_out_Attributes.CLUT == NULL ) {
        in_out_Attributes.flags &= ~F_LAYER_CLUT;
    }
    if ( in_out_Attributes.CLUT_count == 0 ) {
        in_out_Attributes.flags &= ~F_LAYER_CLUT_COUNT;
    }

    return  R_WINDOW_SURFACES_AccessLayerAttributes( this->_self,
            (layer_attributes_t *) &in_out_Attributes );
}
#endif


/***********************************************************************
* ClassImplement: WindowSurfacesConfigClass
************************************************************************/

/***********************************************************************
* Implement: WindowSurfacesConfigClass
************************************************************************/
WindowSurfacesConfigClass::WindowSurfacesConfigClass()
{
    this->flags = 0;
    this->pixel_format = PIXEL_FORMAT_ARGB8888;
    this->layer_count = 1;
    this->buffer_height = GS_DEFAULT_HEIGHT;
    this->background_color.Value = GS_DEFAULT_CLEAR_COLOR;
}


/***********************************************************************
* ClassImplement: LayerAttributes
************************************************************************/

/***********************************************************************
* Implement: LayerAttributes
************************************************************************/
#ifdef  IS_WINDOW_SURFACES_EX
LayerAttributes::LayerAttributes()
{
    this->access = ACCESS_READ;
    this->flags = 0;
    this->id       = GS_DEFAULT_INT_VALUE;
    this->priority = GS_DEFAULT_INT_VALUE;
    this->is_show = false;
    this->is_color_key = false;
    this->color_key.Value = GS_DEFAULT_CLEAR_COLOR;
    this->layer_color.Value = 0;
    this->x        = GS_DEFAULT_INT_VALUE;
    this->y        = GS_DEFAULT_INT_VALUE;
    this->width    = GS_DEFAULT_INT_VALUE;
    this->height   = GS_DEFAULT_INT_VALUE;
    this->offset_x = GS_DEFAULT_INT_VALUE;
    this->offset_y = GS_DEFAULT_INT_VALUE;
    this->CLUT     = NULL;
    this->CLUT_count = 0;
}
#endif


