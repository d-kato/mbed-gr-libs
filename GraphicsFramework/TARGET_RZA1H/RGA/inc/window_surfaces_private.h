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
* $FileName: window_surfaces_private.h $
* $Module: CLibCommon $ $PublicVersion: 1.00 $ (=CLIB_VERSION)
* $Rev: 47 $
* $Date:: 2014-01-06 18:18:48 +0900#$
* Description  : Sample API
*******************************************************************************/

#ifndef  WINDOW_SURFACES_PRIVATE_H
#define  WINDOW_SURFACES_PRIVATE_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "RGA_Port_typedef.h"
#include  "r_ospl_typedef.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
#define  RGA_STACK_ADDRESS_ALIGNMENT  64
R_STATIC_ASSERT_GLOBAL( RGA_STACK_ADDRESS_ALIGNMENT >= RGA_SOURCE_IMAGE_STRIDE_ALIGNMENT, "" );
R_STATIC_ASSERT_GLOBAL( RGA_STACK_ADDRESS_ALIGNMENT >= RGA_VDC5_BUFFER_ADDRESS_ALIGNMENT, "" );

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
errnum_t  R_EXRAM_GetBigMemory( void * /*<type**>*/ out_Address, size_t *out_Size );


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* WINDOW_SURFACES_PRIVATE_H */
