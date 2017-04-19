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
* $FileName: jcu_pl.h $
* $Module: JCU $ $PublicVersion: 1.00 $ (=JCU_VERSION)
* $Rev: 35 $
* $Date:: 2014-02-26 13:18:53 +0900#$
* Description  : Sample Data
******************************************************************************/

#ifndef JCU_PL_H
#define JCU_PL_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_jcu_api.h"
#include  "r_ospl.h"

#ifdef __cplusplus
extern "C"
{
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
void      R_JCU_SetDefaultAsync( r_ospl_async_t *const  Async,  r_ospl_async_type_t AsyncType );
errnum_t  R_JCU_OnInitialize(void);
errnum_t  R_JCU_OnFinalize( errnum_t e );
errnum_t  R_JCU_SetInterruptCallbackCaller( const r_ospl_caller_t *const  Caller );
void      R_JCU_OnEnableInterrupt( jcu_interrupt_lines_t const  Enables );
void      R_JCU_OnDisableInterrupt( jcu_interrupt_lines_t const  Disables1 );
errnum_t  R_JCU_OnInterruptDefault( const r_ospl_interrupt_t *const  InterruptSource,
                                    const r_ospl_caller_t *const  Caller );

/* For integrating driver */
bool_t    R_JCU_I_LOCK_Replace( void *const  I_Lock,  const r_ospl_i_lock_vtable_t *const  I_LockVTable );
bool_t    R_JCU_DisableInterrupt(void);
void      R_JCU_EnableInterrupt(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _JCU_PL_H */

