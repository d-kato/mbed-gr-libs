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
* @file  vsync_pl.c
* @brief   $Module: CLibCommon $ $PublicVersion: 1.00 $ (=CLIB_VERSION)
* $Rev: 44 $
* $Date:: 2013-12-20 11:20:00 +0900#$
*/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include  "r_ospl.h"
#include  "r_vdc5.h"
#include  "vsync.h"
#include  "vsync_pl.h"

#if ! R_OSPL_IS_PREEMPTION
#include  "dev_drv.h"
#endif

/******************************************************************************
Typedef definitions
******************************************************************************/

typedef void (* gs_rtx_interrupt_t )(void);  /* RTX */
typedef void (* gs_cint_interrupt_t )( uint32_t int_sense );  /* OS less INTC */
typedef void (* gs_vdc_interrupt_t )( vdc5_int_type_t int_type );

typedef struct st_r_v_sync_pl_channel_t  r_v_sync_pl_channel_t;
struct st_r_v_sync_pl_channel_t {
    const r_ospl_caller_t             *InterruptCallbackCaller;
    bool_t                             IsVDC5_Callback;
};

static r_v_sync_pl_channel_t  gs_v_sync_pl_channel[ R_V_SYNC_CHANNEL_COUNT ];


/******************************************************************************
Macro definitions
******************************************************************************/

/**
* @def  GS_VSYNC_INTERRUPT_PRIORITY
* @brief  GS_VSYNC_INTERRUPT_PRIORITY
* @par Parameters
*    None
* @return  None.
*/
enum { GS_VSYNC_INTERRUPT_PRIORITY = 7 };


/**
* @def  GS_INTERRUPT_FUNCTION_TYPE
* @brief  GS_INTERRUPT_FUNCTION_TYPE
* @par Parameters
*    None
* @return  None.
*
*    - 0 : RTX
*    - 1 : INTC
*/
#if R_OSPL_IS_PREEMPTION
#define  GS_INTERRUPT_FUNCTION_TYPE  0
#else
#define  GS_INTERRUPT_FUNCTION_TYPE  1
#endif


/**
* @typedef  gs_interrupt_t
* @brief  Interrupt callback function type
*/
#if  GS_INTERRUPT_FUNCTION_TYPE == 0
#define  gs_interrupt_t  gs_rtx_interrupt_t
#else
#define  gs_interrupt_t  gs_cint_interrupt_t
#endif


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static void  R_V_SYNC_IRQ_HandlerN( int_fast32_t const  ChannelNum );
static void  R_V_SYNC_IRQ_Handler0( vdc5_int_type_t const  int_type );
static void  R_V_SYNC_IRQ_Handler1( vdc5_int_type_t const  int_type );
#if  GS_INTERRUPT_FUNCTION_TYPE != 1
static gs_cint_interrupt_t  gs_cint_vdc5_interrupt_handler[2];
static void  R_V_SYNC_IRQ_HandlerRoot0(void);
static void  R_V_SYNC_IRQ_HandlerRoot1(void);
#endif


/** Table of interrupt line to channel number */
static const r_ospl_interrupt_t  gs_array_of_i_context[ R_V_SYNC_CHANNEL_COUNT ] = {
    { BSP_INT_SRC_GR3_VLINE0,  0 },
    { BSP_INT_SRC_GR3_VLINE1,  1 }
};


/***********************************************************************
* Implement: R_V_SYNC_SetDefaultAsync
************************************************************************/
void  R_V_SYNC_SetDefaultAsync( r_ospl_async_t *const  Async,  r_ospl_async_type_t AsyncType )
{
    R_UNREFERENCED_VARIABLE( AsyncType );

    IF_DQ( Async == NULL ) {
        goto fin;
    }

    if ( IS_BIT_NOT_SET( Async->Flags, R_F_OSPL_A_Thread ) ) {
        Async->A_Thread = NULL;
    }

    if ( IS_BIT_NOT_SET( Async->Flags, R_F_OSPL_A_EventValue ) ) {
        Async->A_EventValue = R_OSPL_A_FLAG;
    }

    if ( IS_BIT_NOT_SET( Async->Flags, R_F_OSPL_I_Thread ) ) {
        Async->I_Thread = NULL;
    }

    if ( IS_BIT_NOT_SET( Async->Flags, R_F_OSPL_I_EventValue ) ) {
        Async->I_EventValue = R_OSPL_I_FLAG;
    }

    if ( IS_BIT_NOT_SET( Async->Flags, R_F_OSPL_InterruptCallback ) ) {
        Async->InterruptCallback = (r_ospl_callback_t)&( R_V_SYNC_OnInterruptDefault );
    }
    /* MISRA 16.9 */

    Async->Flags = R_F_OSPL_A_Thread | R_F_OSPL_A_EventValue |
                   R_F_OSPL_I_Thread | R_F_OSPL_I_EventValue |
                   R_F_OSPL_InterruptCallback | R_F_OSPL_Delegate;

fin:
    return;
}


/***********************************************************************
* Implement: R_V_SYNC_OnInitialize
************************************************************************/
errnum_t  R_V_SYNC_OnInitialize( int_fast32_t const  ChannelNum )
{
#if GS_INTERRUPT_FUNCTION_TYPE == 1
    bsp_int_cb_t    handler;
#endif
    errnum_t       e;
    vdc5_error_t   er;
    bsp_int_err_t  eb;
    bsp_int_src_t  int_id;
    vdc5_int_t     interrupt;
    r_v_sync_pl_channel_t *const     self = &gs_v_sync_pl_channel[ ChannelNum ];
    const r_ospl_interrupt_t *const  i_context = &gs_array_of_i_context[ ChannelNum ];

    /*[gs_IRQHandlers]*/
    static const gs_vdc_interrupt_t  gs_IRQHandlers[ R_V_SYNC_CHANNEL_COUNT ] = {
        &( R_V_SYNC_IRQ_Handler0 ), /* MISRA 16.9 */
        &( R_V_SYNC_IRQ_Handler1 )
    };

#if  GS_INTERRUPT_FUNCTION_TYPE != 1
    /*[gs_IRQHandlerRoots]*/
    static const gs_interrupt_t  gs_IRQHandlerRoots[ R_V_SYNC_CHANNEL_COUNT ] = {
        &( R_V_SYNC_IRQ_HandlerRoot0 ), /* MISRA 16.9 */
        &( R_V_SYNC_IRQ_HandlerRoot1 )
    };
#endif


    IF ( (ChannelNum < 0) || (ChannelNum >= R_V_SYNC_CHANNEL_COUNT) ) {
        e=E_OTHERS;
        goto fin;
    }

    self->InterruptCallbackCaller = NULL;

    int_id  = i_context->IRQ_Num;
#if GS_INTERRUPT_FUNCTION_TYPE == 1
    handler = R_VDC5_GetISR( (vdc5_channel_t) ChannelNum, VDC5_INT_TYPE_VLINE );
    eb= R_BSP_InterruptWrite( int_id, handler );
#else
    gs_cint_vdc5_interrupt_handler[ChannelNum] = R_VDC5_GetISR(
                (vdc5_channel_t) ChannelNum, VDC5_INT_TYPE_VLINE );
    eb= R_BSP_InterruptWrite( int_id, gs_IRQHandlerRoots[ChannelNum] );
#endif
    IF ( eb != BSP_INT_SUCCESS ) {
        e=E_OTHERS;
        goto fin;
    }

    e= R_OSPL_SetInterruptPriority( int_id, GS_VSYNC_INTERRUPT_PRIORITY );
    IF ( e != 0 ) {
        goto fin;
    }

    interrupt.type     = VDC5_INT_TYPE_VLINE;
    interrupt.callback = gs_IRQHandlers[ ChannelNum ];
    interrupt.line_num = (uint16_t) 0;

    er = R_VDC5_CallbackISR( (vdc5_channel_t) ChannelNum, &interrupt );
    IF ( er != VDC5_OK ) {
        e=E_OTHERS;
        goto fin;
    }

    self->IsVDC5_Callback = true;

    e=0;
fin:
    return  e;
}


/***********************************************************************
* Implement: R_V_SYNC_OnFinalize
************************************************************************/
errnum_t  R_V_SYNC_OnFinalize( int_fast32_t const  ChannelNum,  errnum_t e )
{
    r_v_sync_pl_channel_t *const  self = &gs_v_sync_pl_channel[ ChannelNum ];

    if ( (ChannelNum >= 0) && (ChannelNum < R_V_SYNC_CHANNEL_COUNT) ) {
        if ( IS( self->IsVDC5_Callback ) ) {
            vdc5_error_t  er;
            vdc5_int_t    interrupt;

            interrupt.type     = VDC5_INT_TYPE_VLINE;
            interrupt.callback = NULL;
            interrupt.line_num = (uint16_t) 0;

            er = R_VDC5_CallbackISR( (vdc5_channel_t) ChannelNum, &interrupt );
            IF ( (er != VDC5_OK) && (e == 0) ) {
                e=E_OTHERS;
            }

            self->IsVDC5_Callback = false;
        }
    }

    return  e;
}


/***********************************************************************
* Implement: R_V_SYNC_SetInterruptCallbackCaller
************************************************************************/
errnum_t  R_V_SYNC_SetInterruptCallbackCaller( int_fast32_t const  ChannelNum,
        const r_ospl_caller_t *const  Caller )
{
    r_v_sync_pl_channel_t *const  self = &gs_v_sync_pl_channel[ ChannelNum ];

    self->InterruptCallbackCaller = Caller;

    return  0;
}


/***********************************************************************
* Implement: R_V_SYNC_OnEnableInterrupt
************************************************************************/
void  R_V_SYNC_OnEnableInterrupt( int_fast32_t const  ChannelNum,
                                  r_v_sync_interrupt_lines_t const  Enables )
{
    bsp_int_err_t  eb;

    if ( IS_BIT_SET( Enables, R_V_SYNC_INTERRUPT_LINE_V_LINE ) ) {
        bsp_int_src_t const  num_of_IRQ = gs_array_of_i_context[ ChannelNum ].IRQ_Num;

        eb= R_BSP_InterruptControl( num_of_IRQ, BSP_INT_CMD_INTERRUPT_ENABLE, FIT_NO_PTR );
        ASSERT_D( eb == 0,  R_NOOP() );
        R_UNREFERENCED_VARIABLE( eb ); /* for Release configuration */
    }
}


/***********************************************************************
* Implement: R_V_SYNC_OnDisableInterrupt
************************************************************************/
void  R_V_SYNC_OnDisableInterrupt( int_fast32_t const  ChannelNum,
                                   r_v_sync_interrupt_lines_t const  Disables1 )
{
    bsp_int_err_t  eb;

    if ( IS_BIT_SET( Disables1, R_V_SYNC_INTERRUPT_LINE_V_LINE ) ) {
        bsp_int_src_t const  num_of_IRQ = gs_array_of_i_context[ ChannelNum ].IRQ_Num;

        eb= R_BSP_InterruptControl( num_of_IRQ, BSP_INT_CMD_INTERRUPT_DISABLE, FIT_NO_PTR );
        ASSERT_D( eb == 0,  R_NOOP() );
        R_UNREFERENCED_VARIABLE( eb ); /* for Release configuration */
    }
}


/***********************************************************************
* Implement: R_V_SYNC_OnInterruptDefault
************************************************************************/
errnum_t  R_V_SYNC_OnInterruptDefault( const r_ospl_interrupt_t *const  InterruptSource,
                                       const r_ospl_caller_t *const  Caller )
{
    R_UNREFERENCED_VARIABLE( Caller );

    return  R_V_SYNC_OnInterrupting( InterruptSource );
}


/**
* @brief   Interrupt service routine for all channels
*
* @param   ChannelNum ChannelNum
* @return  None
*/
static void  R_V_SYNC_IRQ_HandlerN( int_fast32_t const  ChannelNum )
{
    r_v_sync_pl_channel_t *const     self = &gs_v_sync_pl_channel[ ChannelNum ];
    const r_ospl_interrupt_t *const  i_context = &gs_array_of_i_context[ ChannelNum ];

    /* V-Sync interrupt always be enabled. Because clear interrupt status */

    if ( self->InterruptCallbackCaller != NULL ) {
        R_OSPL_CallInterruptCallback( self->InterruptCallbackCaller, i_context );
    }
}


/**
* @brief   Interrupt service routine for INTC channel 0
*
* @param   int_type ignored
* @return  None
*/
static void  R_V_SYNC_IRQ_Handler0( vdc5_int_type_t const  int_type )
{
    R_UNREFERENCED_VARIABLE( int_type );
    R_V_SYNC_IRQ_HandlerN( 0 );
}


/**
* @brief   Interrupt service routine for INTC channel 1
*
* @param   int_type ignored
* @return  None
*/
static void  R_V_SYNC_IRQ_Handler1( vdc5_int_type_t const  int_type )
{
    R_UNREFERENCED_VARIABLE( int_type );
    R_V_SYNC_IRQ_HandlerN( 1 );
}


/**
* @brief   Interrupt service routine for RTX channel 0
*
* @par Parameters
*    None
* @return  None
*/
#if GS_INTERRUPT_FUNCTION_TYPE != 1
static void  R_V_SYNC_IRQ_HandlerRoot0(void)
{
    gs_cint_vdc5_interrupt_handler[0]( 0 );
    GIC_EndInterrupt( BSP_INT_SRC_GR3_VLINE0 );
}
#endif


/**
* @brief   Interrupt service routine for RTX channel 1
*
* @par Parameters
*    None
* @return  None
*/
#if GS_INTERRUPT_FUNCTION_TYPE != 1
static void  R_V_SYNC_IRQ_HandlerRoot1(void)
{
    gs_cint_vdc5_interrupt_handler[1]( 0 );
    GIC_EndInterrupt( BSP_INT_SRC_GR3_VLINE0 );
}
#endif

