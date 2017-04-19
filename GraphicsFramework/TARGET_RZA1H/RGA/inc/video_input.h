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
* @file  video_input.h
* @brief   $Module: CLibCommon $ $PublicVersion: 1.00 $ (=CLIB_VERSION)
* $Rev: 47 $
* $Date:: 2014-01-06 18:18:48 +0900#$
* - Description: V-Sync porting layer
*/

#ifndef  VIDEO_INPUT_H
#define  VIDEO_INPUT_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "video_input_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Typedef definitions
******************************************************************************/
/* In "video_input_typedef.h" */

/******************************************************************************
Macro definitions
******************************************************************************/
/* In "video_input_typedef.h" */

/******************************************************************************
Variable Externs
******************************************************************************/
/* In "video_input_typedef.h" */

/******************************************************************************
Functions Prototypes
******************************************************************************/


/**
* @brief   R_VIDEO_INPUT_InitConst
*
* @param   self video_input_t
* @return  None
*/
void  R_VIDEO_INPUT_InitConst( video_input_t *const  self );


/**
* @brief   R_VIDEO_INPUT_Initialize
*
* @param   self video_input_t
* @param   in_out_Config Configuration
* @return  Error Code. 0=No Error.
*
* @par Description
*    "R_WINDOW_SURFACES_SwapBuffers" must be called after calling this
*    function for showing video layer.
*    Set "window_surfaces_config_t::background_mode" = "BACKGROUND_MODE_VIDEO_INTERLACE"
*/
errnum_t  R_VIDEO_INPUT_Initialize( video_input_t *const  self,
                                    video_input_config_t *in_out_Config );


/**
* @brief   R_VIDEO_INPUT_Finalize
*
* @param   self video_input_t
* @param   e Errors that have occurred. No error = 0
* @return  Error code or e, 0 = successful and input e=0
*/
errnum_t  R_VIDEO_INPUT_Finalize( video_input_t *const  self, errnum_t e );


#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif
