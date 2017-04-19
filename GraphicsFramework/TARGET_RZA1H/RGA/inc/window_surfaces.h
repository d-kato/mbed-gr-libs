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
/******************************************************************************
* $FileName: window_surfaces.h $
* $Module: CLibCommon $ $PublicVersion: 1.00 $ (=CLIB_VERSION)
* $Rev: 47 $
* $Date:: 2014-01-06 18:18:48 +0900#$
* Description: Sample API
*******************************************************************************/

#ifndef  WINDOW_SURFACES_H
#define  WINDOW_SURFACES_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "clib_drivers_typedef.h"
#include  "window_surfaces_typedef.h"
#include  "r_multi_compiler_typedef.h"
#include  "window_surfaces_inline.h"
#ifdef  IS_WINDOW_SURFACES_EX
#include  "window_surfaces_ex.h"
#include  "RGA_API_typedef.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Typedef definitions
******************************************************************************/
/* In "frame_buffer_typedef.h" */

/******************************************************************************
Macro definitions
******************************************************************************/
/* In "frame_buffer_typedef.h" */

/******************************************************************************
Variable Externs
******************************************************************************/
/* In "frame_buffer_typedef.h" */

/******************************************************************************
Functions Prototypes
******************************************************************************/

/**
* @brief   R_WINDOW_SURFACES_InitConst
*
* @param   self window_surfaces_t
* @return  None
*/
void      R_WINDOW_SURFACES_InitConst( window_surfaces_t *const  self );


/**
* @brief   R_WINDOW_SURFACES_Initialize
*
* @param   self window_surfaces_t
* @param   in_out_config window_surfaces_config_t
* @return  Error code, 0=No error
*/
errnum_t  R_WINDOW_SURFACES_Initialize( window_surfaces_t *const  self,
                                        window_surfaces_config_t *in_out_config );


/**
* @brief   R_WINDOW_SURFACES_Finalize
*
* @param   self window_surfaces_t
* @param   e Errors that have occurred. No error = 0
* @return  Error code or e, 0 = successful and input e=0
*/
errnum_t  R_WINDOW_SURFACES_Finalize( window_surfaces_t *const  self, errnum_t e );


/**
* @brief   Get <frame_buffer_t>* of specified layer number
*
* @param   self window_surfaces_t
* @param   layer_num layer_num
* @param   out_frame_buffer out_frame_buffer
* @return  Error code, 0=No error
*/
errnum_t  R_WINDOW_SURFACES_GetLayerFrameBuffer( const window_surfaces_t *const  self,
        int_fast32_t const  layer_num,  frame_buffer_t **const  out_frame_buffer );


/**
* @brief   R_WINDOW_SURFACES_SwapBuffers
*
* @param   self window_surfaces_t
* @param   layer_num layer_num
* @param   context Graphics context or NULL
* @return  Error code, 0=No error
*/
#ifdef  IS_WINDOW_SURFACES_EX
errnum_t  R_WINDOW_SURFACES_SwapBuffers( window_surfaces_t *const  self,
        int_fast32_t const  layer_num,  graphics_t *const  context );
#else
errnum_t  R_WINDOW_SURFACES_SwapBuffers( window_surfaces_t *const  self,
        int_fast32_t const  layer_num,  const void *const  null_context );
#endif


/**
* @brief   Asynchronous function of <R_WINDOW_SURFACES_SwapBuffers>
*
* @param   self window_surfaces_t
* @param   layer_num layer_num
* @param   async r_ospl_async_t
* @return  Error code, 0=No error
*/
errnum_t  R_WINDOW_SURFACES_SwapBuffersStart( window_surfaces_t *const  self,
        int_fast32_t const  layer_num,  r_ospl_async_t *async );


/**
* @brief   Wait for V-Sync
*
* @param   swap_interval If 1=60fps Then 2=30fps, 3=20fps
* @param   is_1_v_sync_at_minimum false = It is possible to return soon
* @return  Error Code. 0=No Error.
*/
errnum_t  R_WINDOW_SURFACES_WaitForVSync( window_surfaces_t *const  self,
        int_fast32_t const  swap_interval,  bool_t const  is_1_v_sync_at_minimum );


/**
* @brief   Allocate offscreen from stack
*
* @param   self window_surfaces_t
* @param   in_out_frame_buffer frame_buffer_t
* @return  Error code, 0=No error
*
* @par Description
*    - (input) - >stride, ->height, ->buffer_count
*    - (output) - >buffer_address[(all)]
*/
errnum_t  R_WINDOW_SURFACES_AllocOffscreenStack( window_surfaces_t *const  self,
        frame_buffer_t *const  in_out_frame_buffer );


/**
* @brief   Free offscreen to stack
*
* @param   self window_surfaces_t
* @param   frame_buffer frame_buffer_t
* @return  Error code, 0=No error
*
* @par Description
*    - If frame_buffer - >buffer_count == 0, do nothing.
*/
errnum_t  R_WINDOW_SURFACES_FreeOffscreenStack( window_surfaces_t *const  self,
        const frame_buffer_t *const  frame_buffer );


#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif
