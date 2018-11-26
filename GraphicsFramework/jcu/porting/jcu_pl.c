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
/*******************************************************************************
* $FileName: jcu_pl.c $
* $Module: JCU $ $PublicVersion: 1.00 $ (=JCU_VERSION)
* $Rev: 35 $
* $Date:: 2014-02-26 13:18:53 +0900#$
* Description : JCU driver porting layer
******************************************************************************/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_ospl.h"
#include  "iodefine.h"
#include  "r_jcu_api.h"
#include  "r_jcu_pl.h"

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef struct st_jcu_pl_t  jcu_pl_t;

/*[jcu_pl_t]*/
struct st_jcu_pl_t {
    const r_ospl_caller_t     *InterruptCallbackCaller;
    const jcu_async_status_t  *Status;
};

/*[gs_jcu_pl]*/
static jcu_pl_t  gs_jcu_pl
;

/******************************************************************************
Macro definitions
******************************************************************************/
enum { JCU_INT_PRI = 2 };
#define  CPG_JCU_CLOCK_POWER_OFF  0x00000002u

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static void  JCU_IRQ_JEDI_Handler(void);
static void  JCU_IRQ_JDTI_Handler(void);

/*[gs_jedi_interrupt_context] JEDI interrupt context */
/*[gs_jdti_interrupt_context] JDTI interrupt context */
static const r_ospl_interrupt_t  gs_jedi_interrupt_context = { BSP_INT_SRC_JEDI, 0 };
static const r_ospl_interrupt_t  gs_jdti_interrupt_context = { BSP_INT_SRC_JDTI, 0 };


/**************************************************************************//**
* Function Name: [R_JCU_SetDefaultAsync]
* @retval        None
******************************************************************************/
void  R_JCU_SetDefaultAsync( r_ospl_async_t *const  Async,  r_ospl_async_type_t AsyncType )
{
    IF_DQ( Async == NULL ) {
        goto fin;
    }

    if ( IS_BIT_NOT_SET( Async->Flags, R_F_OSPL_A_Thread ) ) {
        Async->A_Thread = NULL;
    }

    if ( IS_BIT_NOT_SET( Async->Flags, R_F_OSPL_A_EventValue ) ) {
        if ( AsyncType == R_OSPL_ASYNC_TYPE_NORMAL ) {
            Async->A_EventValue = R_OSPL_A_FLAG;
        } else {
            Async->A_EventValue = R_OSPL_FINAL_A_FLAG;
        }
    } else {
        ASSERT_D( IS_BIT_SET( Async->Flags, R_F_OSPL_A_Thread ), R_NOOP() );
    }

    if ( IS_BIT_NOT_SET( Async->Flags, R_F_OSPL_I_Thread ) ) {
        Async->I_Thread = NULL;
    }

    if ( IS_BIT_NOT_SET( Async->Flags, R_F_OSPL_I_EventValue ) ) {
        Async->I_EventValue = R_OSPL_I_FLAG;
    } else {
        ASSERT_D( IS_BIT_SET( Async->Flags, R_F_OSPL_I_Thread ), R_NOOP() );
    }

    if ( IS_BIT_NOT_SET( Async->Flags, R_F_OSPL_InterruptCallback ) ) {
        Async->InterruptCallback = &( R_JCU_OnInterruptDefault );    /* MISRA 16.9 */
    }

    Async->Flags = R_F_OSPL_A_Thread | R_F_OSPL_A_EventValue |
                   R_F_OSPL_I_Thread | R_F_OSPL_I_EventValue |
                   R_F_OSPL_InterruptCallback | R_F_OSPL_Delegate;
fin:
    return;
}


/**************************************************************************//**
* Function Name: [R_JCU_OnInitialize]
* @retval        Error code, 0=No error
******************************************************************************/
errnum_t  R_JCU_OnInitialize(void)
{
    errnum_t         e;
    bsp_int_err_t    eb;
    bsp_int_src_t const  num_of_JEDI_IRQ = gs_jedi_interrupt_context.IRQ_Num;
    bsp_int_src_t const  num_of_JDTI_IRQ = gs_jdti_interrupt_context.IRQ_Num;
    jcu_pl_t *const  self = &gs_jcu_pl;

    self->InterruptCallbackCaller = NULL;
    self->Status = NULL;


    /* Register "JEDI" */
    eb= R_BSP_InterruptWrite( num_of_JEDI_IRQ, &( JCU_IRQ_JEDI_Handler ) );  /* MISRA 16.9 */
    IF ( eb != 0 ) {
        e=E_OTHERS;
        goto fin;
    }

    e= R_OSPL_SetInterruptPriority( num_of_JEDI_IRQ, JCU_INT_PRI );
    IF ( e != 0 ) {
        goto fin;
    }


    /* Register "JDTI" */
    eb= R_BSP_InterruptWrite( num_of_JDTI_IRQ, &( JCU_IRQ_JDTI_Handler ) );  /* MISRA 16.9 */
    IF ( eb != 0 ) {
        e=E_OTHERS;
        goto fin;
    }

    e= R_OSPL_SetInterruptPriority( num_of_JDTI_IRQ, JCU_INT_PRI );
    IF ( e != 0 ) {
        goto fin;
    }


    /* start to suuply the clock for JCU */
    { /* ->QAC 0306 */
#if defined(TARGET_RZ_A1XX)
        uint32_t cpg_reg;
        cpg_reg = (uint32_t)(CPG.STBCR6) & ~CPG_JCU_CLOCK_POWER_OFF;
        CPG.STBCR6 =(uint8_t)cpg_reg;
#elif defined(TARGET_RZ_A2XX)
        CPG.STBCR5.BIT.MSTP51 = 0;
#endif
    } /* <-QAC 0306 */

    e=0;
fin:
    return  e;
}


/**************************************************************************//**
* Function Name: [R_JCU_OnFinalize]
* @retval        Error code, 0=No error and e=0
******************************************************************************/
/* ->QAC 3227 : "e" is usually changed in finalize function. */
errnum_t  R_JCU_OnFinalize( errnum_t e )
/* <-QAC 3227 */
{
    /* stop to suuply the clock for JCU */
    { /* ->QAC 0306 */
#if defined(TARGET_RZ_A1XX)
        uint32_t cpg_reg;
        cpg_reg = (uint32_t)(CPG.STBCR6) | CPG_JCU_CLOCK_POWER_OFF;
        CPG.STBCR6 = (uint8_t)cpg_reg;
#elif defined(TARGET_RZ_A2XX)
        CPG.STBCR5.BIT.MSTP51 = 1;
#endif
    } /* <-QAC 0306 */

    return  e;
}


/******************************************************************************
* Function Name: [R_JCU_SetInterruptCallbackCaller]
* @retval        Error code, 0=No error
******************************************************************************/
errnum_t  R_JCU_SetInterruptCallbackCaller( const r_ospl_caller_t *const  Caller )
{
    jcu_pl_t *const  self = &gs_jcu_pl;

    self->InterruptCallbackCaller = Caller;

    return  0;
}


/******************************************************************************
* Function Name: [R_JCU_OnEnableInterrupt]
* @retval        None
******************************************************************************/
void  R_JCU_OnEnableInterrupt( jcu_interrupt_lines_t const  Enables )
{
    bsp_int_err_t  eb;

    if ( IS_BIT_SET( Enables, JCU_INTERRUPT_LINE_JEDI ) ) {
        bsp_int_src_t const  num_of_IRQ = gs_jedi_interrupt_context.IRQ_Num;

        eb= R_BSP_InterruptControl( num_of_IRQ, BSP_INT_CMD_INTERRUPT_ENABLE, FIT_NO_PTR );
        ASSERT_D( eb == 0,  R_NOOP() );
        R_UNREFERENCED_VARIABLE( eb ); /* for Release configuration */
    }

    if ( IS_BIT_SET( Enables, JCU_INTERRUPT_LINE_JDTI ) ) {
        bsp_int_src_t const  num_of_IRQ = gs_jdti_interrupt_context.IRQ_Num;

        eb= R_BSP_InterruptControl( num_of_IRQ, BSP_INT_CMD_INTERRUPT_ENABLE, FIT_NO_PTR );
        ASSERT_D( eb == 0,  R_NOOP() );
        R_UNREFERENCED_VARIABLE( eb ); /* for Release configuration */
    }
}


/******************************************************************************
* Function Name: [R_JCU_OnDisableInterrupt]
* @retval        None
******************************************************************************/
void  R_JCU_OnDisableInterrupt( jcu_interrupt_lines_t const  Disables1 )
{
    bsp_int_err_t  eb;

    if ( IS_BIT_SET( Disables1, JCU_INTERRUPT_LINE_JEDI ) ) {
        bsp_int_src_t const  num_of_IRQ = gs_jedi_interrupt_context.IRQ_Num;

        eb= R_BSP_InterruptControl( num_of_IRQ, BSP_INT_CMD_INTERRUPT_DISABLE, FIT_NO_PTR );
        ASSERT_D( eb == 0,  R_NOOP() );
        R_UNREFERENCED_VARIABLE( eb ); /* for Release configuration */
    }

    if ( IS_BIT_SET( Disables1, JCU_INTERRUPT_LINE_JDTI ) ) {
        bsp_int_src_t const  num_of_IRQ = gs_jdti_interrupt_context.IRQ_Num;

        eb= R_BSP_InterruptControl( num_of_IRQ, BSP_INT_CMD_INTERRUPT_DISABLE, FIT_NO_PTR );
        ASSERT_D( eb == 0,  R_NOOP() );
        R_UNREFERENCED_VARIABLE( eb ); /* for Release configuration */
    }
}


/******************************************************************************
* Function Name: [R_JCU_OnInterruptDefault]
* @retval        Error code, 0=No error
******************************************************************************/
errnum_t  R_JCU_OnInterruptDefault( const r_ospl_interrupt_t *const  InterruptSource,
                                    const r_ospl_caller_t *const  Caller )
{
    errnum_t  e;
    errnum_t  ee;
    r_ospl_async_t  *async;

    e = 0;
    R_AVOID_UNSAFE_ALWAYS_WARNING( e );

    IF_DQ( Caller == NULL ) {
        goto fin;
    }

    ee= R_JCU_OnInterrupting( InterruptSource );
    IF ( (ee != 0) && (e == 0) ) {
        e = ee;
    }

    async = Caller->Async;

    if ( async->I_Thread == NULL ) {
        ee= R_JCU_OnInterrupted();
        IF ( (ee != 0) && (e == 0) ) {
            e = ee;
        }
    } else {
        R_OSPL_EVENT_Set( async->I_Thread, async->I_EventValue );
    }

fin:
    return  e;
}


/**************************************************************************//**
* Function Name: [JCU_IRQ_JEDI_Handler]
* @brief         JEDI (JCU Encode Decode Interrupt) interrupt handler
* @retval        None
******************************************************************************/
static void  JCU_IRQ_JEDI_Handler(void)
{
    jcu_pl_t *const                  self = &gs_jcu_pl;
    const r_ospl_interrupt_t *const  i_context = &gs_jedi_interrupt_context;

    R_OSPL_CallInterruptCallback( self->InterruptCallbackCaller, i_context );
}


/**************************************************************************//**
* Function Name: [JCU_IRQ_JDTI_Handler]
* @brief         JDTI (JCU Data Transfer Interrupt) interrupt handler
* @retval        None
******************************************************************************/
static void  JCU_IRQ_JDTI_Handler(void)
{
    jcu_pl_t *const                  self = &gs_jcu_pl;
    const r_ospl_interrupt_t *const  i_context = &gs_jdti_interrupt_context;

    R_OSPL_CallInterruptCallback( self->InterruptCallbackCaller, i_context );
}


