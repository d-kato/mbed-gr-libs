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
* Copyright (C) 2013 - 2014 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**
* @file  vsync.c
* @brief   $Module: CLibCommon $ $PublicVersion: 1.00 $ (=CLIB_VERSION)
* $Rev: $
* $Date::                           $
*/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "vsync.h"
#include  "vsync_pl.h"
#include  "r_ospl.h"


/******************************************************************************
Typedef definitions
******************************************************************************/

/**
* @struct  r_v_sync_i_lock_t
* @brief  I-Lock
*/
typedef struct st_r_v_sync_i_lock_t  r_v_sync_i_lock_t;
struct st_r_v_sync_i_lock_t {
    /** is_lock */
    bool_t  is_lock;

    /** channel_num */
    int_fast32_t  channel_num;
};


/**
* @struct  r_v_sync_channel_t
* @brief  V-Sync context
*/
typedef struct st_r_v_sync_channel_t  r_v_sync_channel_t;
struct st_r_v_sync_channel_t {
    /*-----------------------------------------------------------*/
    /* Group: Count */

    /** VSync_FrameCount */
    volatile int_fast32_t  VSync_FrameCount;

    /** VSync_PreviousFrameCount */
    volatile int_fast32_t  VSync_PreviousFrameCount;

    /** VSync_TargetCount */
    volatile int_fast32_t  VSync_TargetCount;


    /*-----------------------------------------------------------*/
    /* Group: Interrupt */

    /** AsyncStatus */
    r_v_sync_async_status_t  AsyncStatus;

    /** InterruptCallbackCaller */
    r_ospl_caller_t    InterruptCallbackCaller;

    /** InterruptFlag */
    r_ospl_flag32_t    InterruptFlag;

    /** IsEnabledInterrupt */
    bool_t             IsEnabledInterrupt;

    /** InterruptEnables */
    r_ospl_flag32_t    InterruptEnables;

    /** I_Lock */
    r_v_sync_i_lock_t  I_Lock;
};


/* Section: Global */
/** V-Sync context */
static r_v_sync_channel_t  gs_v_sync_channel[ R_V_SYNC_CHANNEL_COUNT ];


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

static void   R_V_SYNC_I_LOCK_Reset( r_v_sync_i_lock_t *const  self );
static bool_t R_V_SYNC_I_LOCK_Lock( r_v_sync_i_lock_t *const  self );
static void   R_V_SYNC_I_LOCK_Unlock( r_v_sync_i_lock_t *const  self );


/***********************************************************************
* Implement: R_V_SYNC_Initialize
************************************************************************/
errnum_t  R_V_SYNC_Initialize( int_fast32_t const  ChannelNum )
{
    errnum_t  e;
    r_v_sync_channel_t *const  self = &gs_v_sync_channel[ ChannelNum ];

    IF ( (ChannelNum < 0) || (ChannelNum >= R_V_SYNC_CHANNEL_COUNT) ) {
        e=E_OTHERS;
        goto fin;
    }

    self->I_Lock.channel_num = ChannelNum;
    R_V_SYNC_I_LOCK_Reset( &self->I_Lock );

    ASSERT_R( self->AsyncStatus.State == R_OSPL_UNINITIALIZED,  e=E_STATE; goto fin );
    R_OSPL_FLAG32_InitConst( &self->AsyncStatus.InterruptFlags );
    R_OSPL_FLAG32_InitConst( &self->AsyncStatus.CancelFlags );

    e= R_V_SYNC_OnInitialize( ChannelNum );
    IF(e!=0) {
        goto fin;
    }

    self->IsEnabledInterrupt = true;
    R_OSPL_FLAG32_InitConst( &self->InterruptEnables );
    self->AsyncStatus.State = R_OSPL_RUNNABLE;

    R_OSPL_FLAG32_Set( &self->InterruptEnables, R_V_SYNC_INTERRUPT_LINE_V_LINE );
    /* V-Sync interrupt always be enabled. Because clear interrupt status */

    e=0;
fin:
    if ( e != 0 ) {
        e= R_V_SYNC_Finalize( ChannelNum, e );
        R_UNREFERENCED_VARIABLE( e );
    }
    return  e;
}


/***********************************************************************
* Implement: R_V_SYNC_Finalize
************************************************************************/
errnum_t  R_V_SYNC_Finalize( int_fast32_t const  ChannelNum,  errnum_t e )
{
    bool_t                     was_enabled; /* = false; */ /* QAC 3197 */
    r_v_sync_channel_t *const  self = &gs_v_sync_channel[ ChannelNum ];

    was_enabled = R_V_SYNC_I_LOCK_Lock( &self->I_Lock );

    if ( ! ( (ChannelNum < 0) || (ChannelNum >= R_V_SYNC_CHANNEL_COUNT) ) ) {
        if ( self->AsyncStatus.State != R_OSPL_UNINITIALIZED ) {
            R_OSPL_FLAG32_Clear( &self->InterruptEnables, R_V_SYNC_INTERRUPT_LINE_V_LINE );

            R_OSPL_FLAG32_Set( &self->AsyncStatus.CancelFlags,
                               R_OSPL_CANNEL_REQUEST | R_OSPL_CANNELING | R_OSPL_CANNELED |
                               R_OSPL_FINALIZE_REQUEST | R_OSPL_FINALIZING );

            e= R_V_SYNC_OnFinalize( ChannelNum, e );

            R_OSPL_FLAG32_Set( &self->AsyncStatus.CancelFlags, R_OSPL_FINALIZED );
            R_OSPL_FLAG32_Clear( &self->AsyncStatus.CancelFlags, R_OSPL_FLAG32_ALL_BITS );
            self->AsyncStatus.State = R_OSPL_UNINITIALIZED;
        }
    }

    if ( IS( was_enabled ) ) {
        R_V_SYNC_I_LOCK_Unlock( &self->I_Lock );
    }
    R_V_SYNC_I_LOCK_Reset( &self->I_Lock );

    return  e;
}


/***********************************************************************
* Implement: R_V_SYNC_Wait
************************************************************************/
errnum_t  R_V_SYNC_Wait( int_fast32_t const  ChannelNum,
                         int_fast32_t const  SwapInterval,  bool_t const  Is1VSyncAtMinimum )
{
    errnum_t        e;
    r_ospl_async_t  async;
    bit_flags32_t   got_flags;

    async.Flags = R_F_OSPL_A_Thread;
    async.A_Thread = R_OSPL_THREAD_GetCurrentId();

    e= R_V_SYNC_WaitStart( ChannelNum, SwapInterval, Is1VSyncAtMinimum, &async );
    IF(e!=0) {
        goto fin;
    }

    e= R_OSPL_EVENT_Wait( async.A_EventValue, &got_flags, R_OSPL_INFINITE );
    IF(e!=0) {
        goto fin;
    }

    e= async.ReturnValue;
    IF(e!=0) {
        goto fin;
    }

    e=0;
fin:
    return  e;
}


/***********************************************************************
* Implement: R_V_SYNC_WaitStart
************************************************************************/
errnum_t  R_V_SYNC_WaitStart( int_fast32_t const  ChannelNum,
                              int_fast32_t const  SwapInterval,  bool_t const  Is1VSyncAtMinimum,
                              r_ospl_async_t *const  Async )
{
    errnum_t                   e;
    bool_t                     was_enabled = false;
    int_fast32_t               target_count;
    int_fast32_t               operand1;
    int_fast32_t               operand2;
    r_v_sync_channel_t *const  self = &gs_v_sync_channel[ ChannelNum ];

    IF ( (ChannelNum < 0) || (ChannelNum >= R_V_SYNC_CHANNEL_COUNT) ) {
        e=E_OTHERS;
        goto fin;
    }
    IF ( Async == NULL ) {
        e=E_OTHERS;
        goto fin;
    }

    R_V_SYNC_SetDefaultAsync( Async, R_OSPL_ASYNC_TYPE_NORMAL );
    Async->ReturnValue = 0;
    ASSERT_R( Async->I_Thread == NULL,  e=E_OTHERS; goto fin );

    was_enabled = R_V_SYNC_I_LOCK_Lock( &self->I_Lock );

    ASSERT_R( self->AsyncStatus.State == R_OSPL_RUNNABLE, e=E_STATE; goto fin );


    /* Set "self->VSync_TargetCount" */
    target_count = self->VSync_PreviousFrameCount + SwapInterval;
    if ( (Is1VSyncAtMinimum) && (target_count < (self->VSync_FrameCount + 1)) ) {
        target_count = self->VSync_FrameCount + 1;
    }
    self->VSync_TargetCount = target_count;


    /* If already target count */
    operand1 = self->VSync_TargetCount;  /* SEC R3.6.2 */
    operand2 = self->VSync_FrameCount;   /* SEC R3.6.2 */
    if ( (operand1 - operand2) <= 0 ) {
        operand1 = self->VSync_FrameCount;  /* SEC R3.6.2 */
        self->VSync_PreviousFrameCount = operand1;

        /* Set application event */
        R_OSPL_EVENT_Set( Async->A_Thread, Async->A_EventValue );
    } else {
        /* Change state to waiting */
        self->AsyncStatus.State = R_OSPL_RUNNING;

        /* Clear application event */
        R_OSPL_EVENT_Clear( Async->A_Thread, Async->A_EventValue );

        /* Attach "Async" to interrupt */
        /* ->MISRA 11.4 : Not too big "enum" is same bit count as "int" */ /* ->SEC R2.7.1 */
        R_OSPL_CALLER_Initialize( &self->InterruptCallbackCaller,
                                  Async, (volatile int_t *) &self->AsyncStatus.State, R_OSPL_INTERRUPTING, NULL, NULL );
        /* <-MISRA 11.4 */ /* <-SEC R2.7.1 */
        e= R_V_SYNC_SetInterruptCallbackCaller( ChannelNum,
                                                &self->InterruptCallbackCaller );
        IF(e!=0) {
            goto fin;
        }
    }

    e=0;
fin:
    if ( IS( was_enabled ) ) {
        R_V_SYNC_I_LOCK_Unlock( &self->I_Lock );
    }
    return  e;
}


/***********************************************************************
* Implement: R_V_SYNC_OnInterrupting
************************************************************************/
errnum_t  R_V_SYNC_OnInterrupting( const r_ospl_interrupt_t *const  InterruptSource )
{
    errnum_t      e;
    bool_t        was_enabled  = false;
    int_fast32_t  operand1;
    int_fast32_t  operand2;
    int_fast32_t  channel_num_;
    r_v_sync_channel_t  *self;
    r_ospl_async_t      *notify_async = NULL;

    IF_DQ( InterruptSource == NULL ) {
        e=E_OTHERS;
        goto fin;
    }

    channel_num_ = InterruptSource->ChannelNum;
    self = &gs_v_sync_channel[ channel_num_ ];

    if ( self->AsyncStatus.State != R_OSPL_INTERRUPTING ) {
        e=0;
        goto fin;
    }

    self->AsyncStatus.State = R_OSPL_INTERRUPTED;


    /* Operate like R_V_SYNC_OnInterrupted() */
    was_enabled = R_V_SYNC_I_LOCK_Lock( &self->I_Lock );

    self->VSync_FrameCount += 1;


    operand1 = self->VSync_TargetCount;  /* SEC R3.6.2 */
    operand2 = self->VSync_FrameCount;   /* SEC R3.6.2 */
    if ( (operand1 - operand2) <= 0 ) {
        operand1 = self->VSync_FrameCount;  /* SEC R3.6.2 */
        self->VSync_PreviousFrameCount = operand1;

        e= R_V_SYNC_SetInterruptCallbackCaller( channel_num_, NULL );

        self->AsyncStatus.State = R_OSPL_RUNNABLE;
        notify_async = self->InterruptCallbackCaller.Async;
    } else {
        self->AsyncStatus.State = R_OSPL_RUNNING;
    }


    e=0;
fin:
    if ( IS( was_enabled ) ) {
        /* ->QAC 3353 : "self" is always assigned, if "was_enabled" is true */
        IF_DQ( self == NULL ) {}  /* QAC 3353 raises SEC R3.2.2 */
        else {
            R_V_SYNC_I_LOCK_Unlock( &self->I_Lock );
        }
        /* <-QAC 3353 */
        /* This is for disabling interrupt by R_V_SYNC_I_LOCK_Disable() */
    }

    if ( notify_async != NULL ) {
        R_OSPL_EVENT_Set( notify_async->A_Thread, notify_async->A_EventValue );
    }
    return  e;
}


/***********************************************************************
* Implement: R_V_SYNC_GetAsyncStatus
************************************************************************/
errnum_t  R_V_SYNC_GetAsyncStatus( int_fast32_t const  ChannelNum,
                                   const r_v_sync_async_status_t **const  out_Status )
{
    errnum_t  e;
    r_v_sync_channel_t *const  self = &gs_v_sync_channel[ ChannelNum ];

    IF_DQ( out_Status == NULL ) {
        e=E_OTHERS;
        goto fin;
    }
    IF ( (ChannelNum < 0) || (ChannelNum >= R_V_SYNC_CHANNEL_COUNT) ) {
        e=E_OTHERS;
        goto fin;
    }

    *out_Status = &self->AsyncStatus;

    e=0;
fin:
    return  e;
}


/**
* @brief   Enable interrupt API
*
* @param   ChannelNum ChannelNum
* @return  None
*/
void  R_V_SYNC_EnableInterrupt( int_fast32_t const  ChannelNum )
{
    errnum_t  e;
    r_v_sync_channel_t *const  self = &gs_v_sync_channel[ ChannelNum ];

    IF ( (ChannelNum < 0) || (ChannelNum >= R_V_SYNC_CHANNEL_COUNT) ) {
        e=E_OTHERS;
        goto fin;
    }

    R_V_SYNC_OnEnableInterrupt( ChannelNum, self->InterruptEnables.Flags );
    self->IsEnabledInterrupt = true;

    e=0;
fin:
    R_UNREFERENCED_VARIABLE( e );
}


/**
* @brief   Disable interrupt API
*
* @param   ChannelNum ChannelNum
* @return  None
*/
bool_t  R_V_SYNC_DisableInterrupt( int_fast32_t const  ChannelNum )
{
    errnum_t  e;
    bool_t    was_interrupted = false;
    r_v_sync_channel_t *const  self = &gs_v_sync_channel[ ChannelNum ];

    IF ( (ChannelNum < 0) || (ChannelNum >= R_V_SYNC_CHANNEL_COUNT) ) {
        e=E_OTHERS;
        goto fin;
    }

    was_interrupted = self->IsEnabledInterrupt;
    self->IsEnabledInterrupt = false;

    R_V_SYNC_OnDisableInterrupt( ChannelNum, self->InterruptEnables.Flags );

    e=0;
fin:
    R_UNREFERENCED_VARIABLE( e );
    return  was_interrupted;
}


/**
* @brief   Reset I-Lock (Interrupt Lock)
*
* @param   self r_v_sync_i_lock_t
* @return  None
*/
static void  R_V_SYNC_I_LOCK_Reset( r_v_sync_i_lock_t *const  self )
{
    IF_DQ( self == NULL ) {
        goto fin;
    }

    self->is_lock = false;
    R_V_SYNC_OnDisableInterrupt( self->channel_num, R_V_SYNC_INTERRUPT_LINE_ALL );

fin:
    return;
}


/**
* @brief   Lock
*
* @param   self r_v_sync_i_lock_t
* @return  Was interrupt enabled
*/
static bool_t  R_V_SYNC_I_LOCK_Lock( r_v_sync_i_lock_t *const  self )
{
    bool_t  is_locked;
    bool_t  was_all_enabled; /* = false; */ /* QAC 3197 */
    bool_t  b;

    IF_DQ( self == NULL ) {
        is_locked = true;
        goto fin;
    }

    was_all_enabled = R_OSPL_DisableAllInterrupt();

    is_locked = self->is_lock;
    if ( ! is_locked ) {
        b= R_V_SYNC_DisableInterrupt( self->channel_num );
        R_UNREFERENCED_VARIABLE( b );  /* QAC 3200 : This is not error information */
        self->is_lock = true;
    }

    if ( IS( was_all_enabled ) ) {
        R_OSPL_EnableAllInterrupt();
    }

fin:
    return  ! is_locked;
}


/**
* @brief   Unlock
*
* @param   self r_v_sync_i_lock_t
* @return  None
*/
static void  R_V_SYNC_I_LOCK_Unlock( r_v_sync_i_lock_t *const  self )
{
    bool_t  was_all_enabled; /* = false; */ /* QAC 3197 */

    IF_DQ( self == NULL ) {
        goto fin;
    }

    was_all_enabled = R_OSPL_DisableAllInterrupt();

    R_V_SYNC_EnableInterrupt( self->channel_num );
    self->is_lock = false;

    if ( IS( was_all_enabled ) ) {
        R_OSPL_EnableAllInterrupt();
    }
fin:
    return;
}


