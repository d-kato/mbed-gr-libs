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
* @file  mcu_interrupts.c
* @brief   Interrupt related FIT BSP.
*
* $Module: OSPL $ $PublicVersion: 0.90 $ (=R_OSPL_VERSION)
* $Rev: 35 $
* $Date:: 2014-04-15 21:38:18 +0900#$
*/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_ospl.h"
#include  "mcu_interrupts.h"


/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/


/***********************************************************************
* Implement: R_BSP_InterruptWrite
************************************************************************/
bsp_int_err_t  R_BSP_InterruptWrite( bsp_int_src_t const  in_IRQ_Num,  bsp_int_cb_t const  in_Callback )
{
    uint32_t       er;
    bsp_int_err_t  eb;
    IRQn_Type      irq;  /* QAC 3441 */

    if ( in_Callback == FIT_NO_FUNC ) {
        irq = R_CAST_bsp_int_src_t_to_IRQn_Type( in_IRQ_Num );
        er= InterruptHandlerUnregister( irq );
        IF ( er != 0u ) {
            eb=BSP_INT_ERR_INVALID_ARG;
            goto fin;
        }
    } else {
        irq = R_CAST_bsp_int_src_t_to_IRQn_Type( in_IRQ_Num );  /* QAC 3441 */
        er= InterruptHandlerRegister( irq, in_Callback );
        IF ( er != 0u ) {
            eb=BSP_INT_ERR_INVALID_ARG;
            goto fin;
        }
    }

    eb = BSP_INT_SUCCESS;
fin:
    return  eb;
}


/***********************************************************************
* Implement: R_BSP_InterruptRead
************************************************************************/
bsp_int_err_t  R_BSP_InterruptRead( bsp_int_src_t const  in_IRQ_Num,  bsp_int_cb_t *const  out_Callback )
{
    R_UNREFERENCED_VARIABLE_2( in_IRQ_Num, out_Callback );
    return  BSP_INT_ERR_UNSUPPORTED;
}


/***********************************************************************
* Implement: R_OSPL_OnInterruptForUnregistered
************************************************************************/
void  R_OSPL_OnInterruptForUnregistered( uint32_t const  int_sense )
{
    R_UNREFERENCED_VARIABLE( int_sense );
    R_OSPL_RaiseUnrecoverable( E_OTHERS );
}


/***********************************************************************
* Implement: R_BSP_InterruptControl
************************************************************************/
bsp_int_err_t  R_BSP_InterruptControl( bsp_int_src_t const  in_IRQ_Num,  bsp_int_cmd_t const  in_Command,
                                       void *const  in_NotUsed )
{
    bsp_int_err_t  eb;
    IRQn_Type      irq;  /* QAC 3441 */

    R_UNREFERENCED_VARIABLE( in_NotUsed );
    R_IT_WILL_BE_NOT_CONST( in_NotUsed );

    switch ( in_Command ) {
        case  BSP_INT_CMD_INTERRUPT_ENABLE:

            irq = R_CAST_bsp_int_src_t_to_IRQn_Type( in_IRQ_Num );
            GIC_EnableIRQ( irq );
            break;

        default:
            IF_DS( in_Command != BSP_INT_CMD_INTERRUPT_DISABLE ) {
                eb=BSP_INT_ERR_INVALID_ARG;
                goto fin;
            }

            irq = R_CAST_bsp_int_src_t_to_IRQn_Type( in_IRQ_Num );
            GIC_DisableIRQ( irq );
            break;
    }

    eb = BSP_INT_SUCCESS;
fin:
    return  eb;
}


