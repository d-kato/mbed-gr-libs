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
* @file  vsync_pl.h
* @brief   $Module: CLibCommon $ $PublicVersion: 1.00 $ (=CLIB_VERSION)
* $Rev: 38 $
* $Date:: 2014-03-18 16:14:45 +0900#$
* - Description: V-Sync porting layer
*/

#ifndef  WINDOW_SURFACES_PL_H
#define  WINDOW_SURFACES_PL_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_ospl_typedef.h"
#include  "vsync_typedef.h"

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


    /**
    * @brief   Set default <r_ospl_async_t>
    *
    * @param   in_out_Async Async parameters
    * @param   AsyncType r_ospl_async_type_t
    * @return  None
    */
    void  R_V_SYNC_SetDefaultAsync( r_ospl_async_t *const  in_out_Async,  r_ospl_async_type_t AsyncType );


    /**
    * @brief   Called from <R_V_SYNC_Initialize>
    *
    * @param   ChannelNum ChannelNum parameters
    * @return  Error Code. 0=No Error.
    */
    errnum_t  R_V_SYNC_OnInitialize( int_fast32_t const  ChannelNum );


    /**
    * @brief   Called from <R_V_SYNC_Finalize>
    *
    * @param   ChannelNum ChannelNum parameters
    * @return  Error Code. 0=No Error.
    */
    errnum_t  R_V_SYNC_OnFinalize( int_fast32_t const  ChannelNum,  errnum_t e );


    /**
    * @brief   Send <r_ospl_caller_t> to porting layer
    *
    * @param   ChannelNum ChannelNum parameters
    * @param   Caller r_ospl_caller_t
    * @return  Error Code. 0=No Error.
    */
    errnum_t  R_V_SYNC_SetInterruptCallbackCaller( int_fast32_t const  ChannelNum,
            const r_ospl_caller_t *const  Caller );


    /**
    * @brief   Enable interrupt lines
    *
    * @param   ChannelNum ChannelNum parameters
    * @param   Enables <r_v_sync_interrupt_lines_t>. 1=Enable, 0=NotChange
    * @return  Error Code. 0=No Error.
    */
    void   R_V_SYNC_OnEnableInterrupt( int_fast32_t const  ChannelNum,
                                       r_v_sync_interrupt_lines_t const  Enables );


    /**
    * @brief   Disable interrupt lines
    *
    * @param   ChannelNum ChannelNum parameters
    * @param   Disables <r_v_sync_interrupt_lines_t>. 1=Disable, 0=NotChange
    * @return  Error Code. 0=No Error.
    */
    void      R_V_SYNC_OnDisableInterrupt( int_fast32_t const  ChannelNum,
                                           r_v_sync_interrupt_lines_t const  Disables );


    /**
    * @brief   Default interrupt callback function
    *
    * @param   InterruptSource r_ospl_interrupt_t
    * @param   Caller r_ospl_caller_t
    * @return  Error Code. 0=No Error.
    */
    errnum_t  R_V_SYNC_OnInterruptDefault( const r_ospl_interrupt_t *const  InterruptSource,
                                           const r_ospl_caller_t *const  Caller );


#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif
