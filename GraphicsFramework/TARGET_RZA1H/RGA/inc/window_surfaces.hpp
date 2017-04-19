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
* @file  window_surfaces.hpp
* @brief   $Module: CLibCommon $ $PublicVersion: 1.20 $ (=CLIB_VERSION)
* $Rev: 47 $
* $Date:: 2014-01-06 18:18:48 +0900#$
* - Description: Window Surfaces for mbed style C++ API
*/

#ifndef  WINDOW_SURFACES_HPP
#define  WINDOW_SURFACES_HPP

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_ospl_typedef.h"
#include  "window_surfaces_typedef.h"
#include  "RGA.h"


/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

class  WindowSurfacesConfigClass;
#ifdef  IS_WINDOW_SURFACES_EX
class  GraphicsClass;
class  LayerAttributes;
#endif


/***********************************************************************
* Class: WindowSurfacesClass
*    mbed style C++ version of <window_surfaces_t>
************************************************************************/
class  WindowSurfacesClass {
public:
	/**
* @brief   WindowSurfacesClass
*
* @par Parameters
*    None
* @return  None.
*/
	WindowSurfacesClass();
	~WindowSurfacesClass();

	/**
* @brief   initialize
*
* @par Parameters
*    None
* @return  None.
*/
	errnum_t  initialize( WindowSurfacesConfigClass& in_out_config );

	/**
* @brief   destroy
*
* @par Parameters
*    None
* @return  None.
*/
	void  destroy();

	/**
* @brief   get_layer_frame_buffer
*
* @par Parameters
*    None
* @return  None.
*/
	errnum_t  get_layer_frame_buffer( int_fast32_t const  layer_num,  frame_buffer_t** const  out_frame_buffer );

	/**
* @brief   swap_buffers
*
* @par Parameters
*    None
* @return  None.
*/
	#ifdef  IS_WINDOW_SURFACES_EX
	errnum_t  swap_buffers( int_fast32_t const  layer_num,  Canvas2D_ContextClass&  context );
	#else
	errnum_t  swap_buffers( int_fast32_t const  layer_num,  const void* const  null_context );
	#endif

	/**
* @brief   swap_buffers_start
*
* @par Parameters
*    None
* @return  None.
*/
	errnum_t  swap_buffers_start( int_fast32_t const  layer_num,  r_ospl_async_t* async );

	/**
* @brief   wait_for_v_sync
*
* @par Parameters
*    None
* @return  None.
*/
	errnum_t  wait_for_v_sync( int_fast32_t const  swap_interval,  bool_t const  is_1_v_sync_at_minimum );

	/**
* @brief   alloc_offscreen_stack
*
* @par Parameters
*    None
* @return  None.
*/
	errnum_t  alloc_offscreen_stack( frame_buffer_t* const  in_out_frame_buffer );

	/**
* @brief   free_offscreen_stack
*
* @par Parameters
*    None
* @return  None.
*/
	errnum_t  free_offscreen_stack( const frame_buffer_t* const  frame_buffer );

#ifdef  IS_WINDOW_SURFACES_EX
	/**
* @brief   do_message_loop
*
* @par Parameters
*    None
* @return  None.
*/
	errnum_t  do_message_loop();

	/**
* @brief   access_layer_attributes
*
* @par Parameters
*    None
* @return  None.
*/
	errnum_t  access_layer_attributes( LayerAttributes& in_out_Attributes );
#endif

private:
	window_surfaces_t*  _self;
};


/***********************************************************************
* Class: WindowSurfacesConfigClass
*    mbed style C++ version of <window_surfaces_config_t>
************************************************************************/
class  WindowSurfacesConfigClass : public window_surfaces_config_t {
public:
	/**
* @brief   WindowSurfacesConfigClass
*
* @par Parameters
*    None
* @return  None.
*/
	WindowSurfacesConfigClass();
};


/***********************************************************************
* Class: LayerAttributes
*    mbed style C++ version of <layer_attributes_t>
************************************************************************/
#ifdef  IS_WINDOW_SURFACES_EX
class  LayerAttributes : public layer_attributes_t {
public:
	/**
* @brief   LayerAttributes
*
* @par Parameters
*    None
* @return  None.
*/
	LayerAttributes();
};
#endif


/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/

#endif  /* WINDOW_SURFACES_HPP */
