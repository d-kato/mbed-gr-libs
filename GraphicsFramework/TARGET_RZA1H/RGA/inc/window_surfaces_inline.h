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
* @file  window_surfaces_inline.h
* @brief   $Module: CLibCommon $ $PublicVersion: 1.00 $ (=CLIB_VERSION)
* $Rev: 47 $
* $Date:: 2014-01-06 18:18:48 +0900#$
*/


/* This file is included from "window_surfaces.h" */


#ifndef  WINDOW_SURFACES_INLINE_H
#define  WINDOW_SURFACES_INLINE_H
#ifndef  NOT_DEFINE_INLINE_FUNCTION

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  <stdint.h>
#include  "clib_drivers_typedef.h"
#include  "frame_buffer_typedef.h"
#include  "r_multi_compiler_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/


/**
* @brief   Get color
*
* @param   Red Red.
* @param   Green Green.
* @param   Blue Blue.
* @param   Alpha Alpha.
* @return  Color code as "r8g8b8a8_t"
*/
INLINE r8g8b8a8_t  R_RGA_Get_R8G8B8A8( int_fast32_t const  Red,
                                       int_fast32_t const  Green,
                                       int_fast32_t const  Blue,
                                       int_fast32_t const  Alpha )
{
    r8g8b8a8_t  color_;  /* _ is for MISRA 5.6 */

    /* ->QAC 3198 */
    color_.Value = DUMMY_INITIAL_VALUE;  /* for avoid ARMCC warning C4017W */
    /* <-QAC 3198 */
    color_.u.Red   = (uint8_t) Red;
    color_.u.Green = (uint8_t) Green;
    color_.u.Blue  = (uint8_t) Blue;
    color_.u.Alpha = (uint8_t) Alpha;

    return  color_;
}


#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif  /* NOT_DEFINE_INLINE_FUNCTION */
#endif
