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
* @file  vsync.h
* @brief   $Module: CLibCommon $ $PublicVersion: 1.00 $ (=CLIB_VERSION)
* $Rev: 38 $
* $Date:: 2014-03-18 16:14:45 +0900#$
*/

#ifndef  VSYNC_H
#define  VSYNC_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_typedefs.h"
#include  "vsync_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Typedef definitions
******************************************************************************/
/* In "vsync_typedef.h" */

/******************************************************************************
Macro definitions
******************************************************************************/
/* In "vsync_typedef.h" */

/******************************************************************************
Variable Externs
******************************************************************************/
/* In "vsync_typedef.h" */

/******************************************************************************
Functions Prototypes
******************************************************************************/

/**
* @brief   Initialize V-Sync object of specified channel number
*
* @param   ChannelNum ChannelNum
* @return  Error Code. 0=No Error.
*/
errnum_t  R_V_SYNC_Initialize( int_fast32_t const  ChannelNum );


/**
* @brief   R_V_SYNC_Finalize
*
* @param   ChannelNum ChannelNum
* @param   e Errors that have occurred. No error = 0
* @return  Error code or e, 0 = successful and input e=0
*/
errnum_t  R_V_SYNC_Finalize( int_fast32_t const  ChannelNum,  errnum_t e );


/**
* @brief   Wait for V-Sync
*
* @param   ChannelNum ChannelNum
* @param   SwapInterval If 1=60fps Then 2=30fps, 3=20fps
* @param   Is1VSyncAtMinimum false = It is possible to return soon
* @return  Error Code. 0=No Error.
*/
errnum_t  R_V_SYNC_Wait( int_fast32_t const  ChannelNum,
                         int_fast32_t const  SwapInterval,  bool_t const  Is1VSyncAtMinimum );


/**
* @brief   Asynchronous function of <R_V_SYNC_Wait>
*
* @param   ChannelNum ChannelNum
* @param   SwapInterval If 1=60fps Then 2=30fps, 3=20fps
* @param   Is1VSyncAtMinimum false = It is possible to set event soon
* @param   Async r_ospl_async_t
* @return  Error Code. 0=No Error.
*/
errnum_t  R_V_SYNC_WaitStart( int_fast32_t const  ChannelNum,
                              int_fast32_t const  SwapInterval,  bool_t const  Is1VSyncAtMinimum,
                              r_ospl_async_t *const  Async );


/**
* @brief   Receive interrupt information.
*
* @param   InterruptSource r_ospl_interrupt_t
* @return  Error Code. 0=No Error.
*/
errnum_t  R_V_SYNC_OnInterrupting( const r_ospl_interrupt_t *const  InterruptSource );


/**
* @brief   Get <r_v_sync_async_status_t>
*
* @param   ChannelNum ChannelNum
* @param   out_Status AsyncStatus
* @return  Error Code. 0=No Error.
*/
errnum_t  R_V_SYNC_GetAsyncStatus( int_fast32_t const  ChannelNum,
                                   const r_v_sync_async_status_t **const  out_Status );


/* Following functions can not be called from Application. */
void      R_V_SYNC_EnableInterrupt( int_fast32_t const  ChannelNum );
bool_t    R_V_SYNC_DisableInterrupt( int_fast32_t const  ChannelNum );
/* It is not necessary: R_V_SYNC_FinalizeAsync(), R_V_SYNC_OnInterrupted() */


#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif
