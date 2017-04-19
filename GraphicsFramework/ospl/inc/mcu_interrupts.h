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
* Copyright (C) 2012 - 2015 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**
* @file  mcu_interrupts.h
* @brief   Interrupt related FIT BSP. Functions.
*
* $Module: OSPL $ $PublicVersion: 0.90 $ (=R_OSPL_VERSION)
* $Rev: 35 $
* $Date:: 2014-04-15 21:38:18 +0900#$
*/

#ifndef MCU_INTERRUPTS_H
#define MCU_INTERRUPTS_H


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "Project_Config.h"
#include "mcu_interrupts_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Typedef definitions
******************************************************************************/
/* In "mcu_interrupts_typedef.h" */

/******************************************************************************
Macro definitions
******************************************************************************/
/* In "mcu_interrupts_typedef.h" */

/******************************************************************************
Variable Externs
******************************************************************************/
/* In "mcu_interrupts_typedef.h" */

/******************************************************************************
Functions Prototypes
******************************************************************************/

/**
* @brief   Registers a interrupt handler
*
* @param   IRQ_Num Interrupt request number
* @param   Callback The function as interrupt handler
* @return  Error code.  If there is no error, the return value is BSP_INT_SUCCESS.
*/
bsp_int_err_t  R_BSP_InterruptWrite( bsp_int_src_t const  IRQ_Num,  bsp_int_cb_t const  Callback );


/**
* @brief   Returns registered interrupt handler
*
* @param   IRQ_Num Interrupt request number
* @param   out_Callback Output: the function as interrupt handler
* @return  Error code.  If there is no error, the return value is BSP_INT_SUCCESS.
*/
bsp_int_err_t  R_BSP_InterruptRead( bsp_int_src_t const  IRQ_Num,  bsp_int_cb_t *const  out_Callback );


/**
* @brief   Controls related to the interrupt
*
* @param   IRQ_Num Interrupt request number
* @param   Command Control command. See <bsp_int_cmd_t>
* @param   NotUsed Depend on "Command" argument
* @return  Error code.  If there is no error, the return value is BSP_INT_SUCCESS.
*/
bsp_int_err_t  R_BSP_InterruptControl( bsp_int_src_t const  IRQ_Num,  bsp_int_cmd_t const  Command,
                                       void *const  NotUsed );


/***********************************************************************
* End of File:
************************************************************************/
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif /* MCU_INTERRUPTS_H */

