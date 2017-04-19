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
* @file  window_surfaces_ex.h
* @brief   $Module: CLibCommon $ $PublicVersion: 1.00 $ (=CLIB_VERSION)
* $Rev: 47 $
* $Date:: 2014-01-06 18:18:48 +0900#$
*/
#ifndef  WINDOW_SURFACES_EX_H
#define  WINDOW_SURFACES_EX_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "window_surfaces_ex_typedef.h"

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


/***********************************************************************
* Class: window_surfaces_t
*    Extra methods of <window_surfaces_t>
************************************************************************/

/**
* @brief   Emulate message loop function.
*
* @param   self window_surfaces_t
* @return  Error code, 0=No error
*/
errnum_t  R_WINDOW_SURFACES_DoMessageLoop( window_surfaces_t *self );


/**
* @brief   Set or get by <layer_attributes_t>
*
* @param   self window_surfaces_t
* @param   in_out_Attributes <layer_attributes_t>
* @return  Error code, 0=No error
*/
errnum_t  R_WINDOW_SURFACES_AccessLayerAttributes( window_surfaces_t *self,
        layer_attributes_t *in_out_Attributes );


/***********************************************************************
* Class: vram_ex_stack_t
*    VRAM stack in external RAM
************************************************************************/

/**
* @brief   R_VRAM_EX_STACK_Initialize
*
* @param   self vram_ex_stack_t
* @param   NullConfig NULL
* @return  Error Code. 0=No Error.
*/
errnum_t  R_VRAM_EX_STACK_Initialize( vram_ex_stack_t *self, void *NullConfig );


/**
* @brief   Allocate VRAM from external RAM
*
* @param   self vram_ex_stack_t
* @param   in_out_FrameBuffer frame_buffer_t
* @return  Error code, 0=No error
*
* @par Description
*    - (input) - >stride, ->height, ->buffer_count
*    - (output) - >buffer_address[(all)]
*/
errnum_t  R_VRAM_EX_STACK_Alloc( vram_ex_stack_t *self, frame_buffer_t *in_out_FrameBuffer );


/**
* @brief   Free VRAM to external RAM
*
* @param   self vram_ex_stack_t
* @param   frame_buffer frame_buffer_t
* @return  Error code, 0=No error
*
* @par Description
*    - If frame_buffer - >buffer_count == 0, do nothing.
*/
errnum_t  R_VRAM_EX_STACK_Free( vram_ex_stack_t *self, frame_buffer_t *frame_buffer );


#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif
