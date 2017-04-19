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
* @file  r_ospl.c
* @brief   OS Porting Layer. Functions not depended on any environment.
*
* $Module: OSPL $ $PublicVersion: 0.90 $ (=R_OSPL_VERSION)
* $Rev: 35 $
* $Date:: 2014-04-15 21:38:18 +0900#$
*/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_ospl.h"
#include  "iodefine.h"
#include  "iobitmasks/cpg_iobitmask.h"
#include  "r_ospl_private.h"
#include  "r_ospl_os_less_private.h"
#if R_OSPL_IS_PREEMPTION
#include  "r_ospl_RTX_private.h"
#endif
#include  "locking.h"
#ifdef __ICCARM__
#include <intrinsics.h>
#endif
#ifdef  __GNUC__
#if ! IS_MBED_USED
#include  "irq.h"
#endif
#endif
#if R_OSPL_DEBUG_TOOL
#include  <stdio.h>
#endif
#ifndef R_OSPL_NDEBUG
#include  <string.h>
#endif


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

static void  R_OSPL_TABLE_Search_Sub( const r_ospl_table_t *self,
                                      const void *Key, r_ospl_table_searched_t *out_Searched );

#if R_OSPL_ERROR_BREAK  ||  R_OSPL_TLS_ERROR_CODE
static r_ospl_error_t  *R_OSPL_GetCurrentThreadError_Sub( r_ospl_if_not_t TypeOfIfNot );
#endif


/**
* @def  R_OSPL_DEFAULT_DEBUG_THREAD_COUNT
* @brief  For until calling "R_OSPL_SET_DEBUG_WORK"
* @par Parameters
*    None
* @return  None.
*/
#if R_OSPL_ERROR_BREAK  ||  R_OSPL_TLS_ERROR_CODE
#define  R_OSPL_DEFAULT_DEBUG_THREAD_COUNT  2
#endif


/** For until calling "R_OSPL_SET_DEBUG_WORK" */
#if R_OSPL_ERROR_BREAK  ||  R_OSPL_TLS_ERROR_CODE
static r_ospl_table_block_t  gs_thread_index_table_body[ R_OSPL_DEFAULT_DEBUG_THREAD_COUNT ];
#endif


/** For until calling "R_OSPL_SET_DEBUG_WORK" */
#if R_OSPL_ERROR_BREAK  ||  R_OSPL_TLS_ERROR_CODE
static r_ospl_error_t  gs_error;
#endif


/** as <r_ospl_global_error_t> */
#if R_OSPL_ERROR_BREAK  ||  R_OSPL_TLS_ERROR_CODE

/* ->MISRA 8.7 : If not defined R_OSPL_ERROR_BREAK, this is referenced by only 1 function. */ /* ->SEC M2.2.1 */
static r_ospl_global_error_t  gs_global_error =
    /* <-MISRA 8.7 */ /* <-SEC M2.2.1 */
{
    {	                                  /* .ThreadIndexTable */
        gs_thread_index_table_body,          /* .Area */
        0,                                   /* .Count */
        R_OSPL_DEFAULT_DEBUG_THREAD_COUNT,   /* .MaxCount */
        NULL,                                /* .KeyCache */
        0,                                   /* .IndexCache */
        0,                                   /* .FirstFreeIndex */
#if R_OSPL_IS_PREEMPTION
        true                                 /* .Is_T_Lock */
#endif
    },
    &gs_error,  /* .ErrorArray */

#if R_OSPL_ERROR_BREAK
    0,          /* .RaisedGlobalErrorID */  /* Necessary T-Lock */
    0           /* .BreakGlobalErrorID */
#endif
};
#endif


/***********************************************************************
* Implement: R_OSPL_GetVersion
************************************************************************/
int32_t  R_OSPL_GetVersion(void)
{
    return  R_OSPL_VERSION;
}


/***********************************************************************
* Implement: R_OSPL_IsPreemption
************************************************************************/
bool_t  R_OSPL_IsPreemption(void)
{
    return  R_OSPL_IS_PREEMPTION;
}


/***********************************************************************
* Implement: R_OSPL_FLAG32_InitConst
************************************************************************/
void  R_OSPL_FLAG32_InitConst( volatile r_ospl_flag32_t *const  self )
{
    IF_DQ( self == NULL ) {
        goto fin;
    }

    self->Flags = 0;

fin:
    return;
}


/***********************************************************************
* Implement: R_OSPL_FLAG32_Set
************************************************************************/
void  R_OSPL_FLAG32_Set( volatile r_ospl_flag32_t *const  self, bit_flags32_t const  SetFlags )
{
    IF_DQ( self == NULL ) {
        goto fin;
    }

    self->Flags |= SetFlags;

fin:
    return;
}


/***********************************************************************
* Implement: R_OSPL_FLAG32_Clear
************************************************************************/
void  R_OSPL_FLAG32_Clear( volatile r_ospl_flag32_t *const  self, bit_flags32_t const  ClearFlags1 )
{
    IF_DQ( self == NULL ) {
        goto fin;
    }

    self->Flags &= ~ClearFlags1;

fin:
    return;
}


/***********************************************************************
* Implement: R_OSPL_FLAG32_Get
************************************************************************/
bit_flags32_t  R_OSPL_FLAG32_Get( volatile const r_ospl_flag32_t *const  self )
{
    bit_flags32_t  return_value;

    IF_DQ( self == NULL ) {
        return_value = 0;
        goto fin;
    }

    return_value = self->Flags;

fin:
    return  return_value;
}


/***********************************************************************
* Implement: R_OSPL_FLAG32_GetAndClear
************************************************************************/
bit_flags32_t  R_OSPL_FLAG32_GetAndClear( volatile r_ospl_flag32_t *const  self )
{
    bit_flags32_t  return_value;

    IF_DQ( self == NULL ) {
        return_value = 0;
        goto fin;
    }

    return_value = self->Flags;
    self->Flags = 0;

fin:
    return  return_value;
}


/***********************************************************************
* Implement: R_OSPL_ASYNC_CopyExceptAThread
************************************************************************/
void  R_OSPL_ASYNC_CopyExceptAThread( const r_ospl_async_t *const  Source,
                                      r_ospl_async_t *const  Destination )
{
    IF_DQ( Destination == NULL ) {
        goto fin;
    }
    IF_DQ( Source == NULL ) {
        goto fin;
    }

    *Destination = *Source;
    Destination->A_Thread = NULL;

fin:
    return;
}


/***********************************************************************
* Implement: R_OSPL_EnableAllInterrupt
************************************************************************/
void  R_OSPL_EnableAllInterrupt(void)
{
    __enable_irq();
}


/***********************************************************************
* Implement: R_OSPL_DisableAllInterrupt
************************************************************************/
bool_t  R_OSPL_DisableAllInterrupt(void)
{
#ifdef __ICCARM__
    bool_t  was_enabled = ( ( __get_interrupt_state() & 0x80 ) == 0 );
    __disable_irq();
    return  was_enabled;
#else
    return  (bool_t)( __disable_irq() == 0 );
#endif
}


/***********************************************************************
* Implement: R_BSP_InterruptsEnable
************************************************************************/
void  R_BSP_InterruptsEnable(void)
{
    __enable_irq();
}


/***********************************************************************
* Implement: R_BSP_InterruptsDisable
************************************************************************/
void  R_BSP_InterruptsDisable(void)
{
#ifdef __ICCARM__
    __disable_irq();
#else
    int_fast32_t  ret;

    ret = __disable_irq();
    R_UNREFERENCED_VARIABLE( ret );  /* QAC 3200 : This is not error information */
#endif
}


/***********************************************************************
* Implement: R_OSPL_LockChannel
************************************************************************/
errnum_t  R_OSPL_LockChannel( int_fast32_t ChannelNum, int_fast32_t *out_ChannelNum,
                              mcu_lock_t  HardwareIndexMin,  mcu_lock_t  HardwareIndexMax )
{
    errnum_t    e;
    bool_t      is_success;
    mcu_lock_t  hardware_index;

    ASSERT_D( HardwareIndexMin <= HardwareIndexMax,  e=E_OTHERS; goto fin );

    if ( ChannelNum == R_OSPL_UNLOCKED_CHANNEL ) {
        ASSERT_D( out_ChannelNum != NULL,  e=E_OTHERS; goto fin );

#if BSP_CFG_USER_LOCKING_ENABLED
        for ( hardware_index = HardwareIndexMin;  hardware_index <= HardwareIndexMax;
                hardware_index += 1 ) {
            is_success = R_BSP_HardwareLock( hardware_index );
            if ( is_success ) {
                break;
            }
        }
        IF ( hardware_index > HardwareIndexMax ) {
            e=E_FEW_ARRAY;
            goto fin;
        }
#else
        e= R_OSPL_LockUnlockedChannel( out_ChannelNum,
                                       HardwareIndexMin, HardwareIndexMax );
        IF(e) {
            goto fin;
        }
        out_ChannelNum = NULL;
#endif
    } else {
        hardware_index = (mcu_lock_t)( HardwareIndexMin + ChannelNum );

        IF_D ( hardware_index < HardwareIndexMin  ||  hardware_index > HardwareIndexMax ) {
            e=E_FEW_ARRAY;
            goto fin;
        }

        is_success = R_BSP_HardwareLock( hardware_index );
        IF ( ! is_success ) {
            e=E_ACCESS_DENIED;
            goto fin;
        }
    }

    if ( out_ChannelNum != NULL ) {
        *out_ChannelNum = hardware_index - HardwareIndexMin;
    }

    e=0;
fin:
    return  e;
}


/***********************************************************************
* Implement: R_OSPL_UnlockChannel
************************************************************************/
errnum_t  R_OSPL_UnlockChannel( int_fast32_t ChannelNum,  errnum_t  e,
                                mcu_lock_t  HardwareIndexMin,  mcu_lock_t  HardwareIndexMax )
{
    bool_t      is_success;
    mcu_lock_t  hardware_index;

    ASSERT_D( HardwareIndexMin <= HardwareIndexMax,  e= R_OSPL_MergeErrNum( e, E_OTHERS ) );

    hardware_index = (mcu_lock_t)( HardwareIndexMin + ChannelNum );

    if ( hardware_index >= HardwareIndexMin  &&  hardware_index <= HardwareIndexMax ) {
        is_success = R_BSP_HardwareUnlock( hardware_index );
        IF ( ! is_success ) {
            e= R_OSPL_MergeErrNum( e, E_ACCESS_DENIED );
        }
    }

    return  e;
}


/***********************************************************************
* Implement: R_OSPL_C_LOCK_InitConst
************************************************************************/
void  R_OSPL_C_LOCK_InitConst( r_ospl_c_lock_t *const  self )
{
    IF_DQ( self == NULL ) {
        goto fin;
    }

    self->IsLocked = false;

fin:
    return;
}


/***********************************************************************
* Implement: R_OSPL_C_LOCK_Lock
************************************************************************/
errnum_t  R_OSPL_C_LOCK_Lock( r_ospl_c_lock_t *const  self )
{
    errnum_t  e;

    IF_DQ( self == NULL ) {
        e=E_OTHERS;
        goto fin;
    }

    IF ( R_OSPL_THREAD_GetCurrentId() == NULL ) {  /* Interrupt */
        e = E_NOT_THREAD;
        R_OSPL_RaiseUnrecoverable( e );
        goto fin;
    }

    IF ( IS( self->IsLocked ) ) {
        e=E_ACCESS_DENIED;
        goto fin;
    }

    self->IsLocked = true;

    e=0;
fin:
    return  e;
}


/***********************************************************************
* Implement: R_OSPL_C_LOCK_Unlock
************************************************************************/
errnum_t  R_OSPL_C_LOCK_Unlock( r_ospl_c_lock_t *const  self )
{
    errnum_t  e;

    if ( self != NULL ) {

        IF ( R_OSPL_THREAD_GetCurrentId() == NULL ) {  /* Interrupt */
            e = E_NOT_THREAD;
            R_OSPL_RaiseUnrecoverable( e );
            goto fin;
        }

        IF ( ! self->IsLocked ) {
            /* Check not unlock the object that was initialized by other thread */
            e = E_ACCESS_DENIED;
            goto fin;
        }

        self->IsLocked = false;
    }

    e=0;
fin:
    return  e;
}


/***********************************************************************
* Implement: R_OSPL_I_LOCK_LockStub
************************************************************************/
bool_t  R_OSPL_I_LOCK_LockStub( void *const  self_ )
{
    R_IT_WILL_BE_NOT_CONST( self_ );
    R_UNREFERENCED_VARIABLE( self_ );
    return  false;
}


/***********************************************************************
* Implement: R_OSPL_I_LOCK_UnlockStub
************************************************************************/
void  R_OSPL_I_LOCK_UnlockStub( void *const  self_ )
{
    R_IT_WILL_BE_NOT_CONST( self_ );
    R_UNREFERENCED_VARIABLE( self_ );
}


/***********************************************************************
* Implement: R_OSPL_I_LOCK_RequestFinalizeStub
************************************************************************/
void  R_OSPL_I_LOCK_RequestFinalizeStub( void *const  self_ )
{
    R_IT_WILL_BE_NOT_CONST( self_ );
    R_UNREFERENCED_VARIABLE( self_ );
}


/***********************************************************************
* Implement: R_OSPL_MEMORY_Barrier
************************************************************************/
void  R_OSPL_MEMORY_Barrier(void)
{
    /* ->QAC 1006 : asm */
    __asm("DSB");
    /* <-QAC 1006 */
}


/***********************************************************************
* Implement: R_OSPL_InstructionSyncBarrier
************************************************************************/
void  R_OSPL_InstructionSyncBarrier(void)
{
    /* ->QAC 1006 : asm */
    __asm("ISB");
    /* <-QAC 1006 */
}


/***********************************************************************
* Implement: R_OSPL_CALLER_Initialize
************************************************************************/
void  R_OSPL_CALLER_Initialize( r_ospl_caller_t *const  self,  r_ospl_async_t *const  Async,
                                volatile void *const  PointerToState,  int_t const  StateValueOfOnInterrupting,
                                void *const  I_Lock, const r_ospl_i_lock_vtable_t *const  I_LockVTable )
{
    ASSERT_D( Async != NULL,  R_NOOP() );
    ASSERT_D( PointerToState != NULL,  R_NOOP() );
    IF_DQ( self == NULL ) {}
    else {
        self->Async = Async;
        self->PointerToState = (volatile int_fast32_t *) PointerToState;
        self->StateValueOfOnInterrupting = StateValueOfOnInterrupting;
        self->I_Lock = I_Lock;
        self->I_LockVTable = I_LockVTable;
    }

#ifndef R_OSPL_NDEBUG
    /* Set sentinel */
    ASSERT_D( IS_ALL_BITS_NOT_SET( Async->Flags, R_F_OSPL_ASYNC_FLAGS_SENTINEL_MASK ),
              R_NOOP() );
    ASSERT_D( IS_ALL_BITS_NOT_SET( R_F_OSPL_ASYNC_FLAGS_SENTINEL_VALUE,
                                   ~R_F_OSPL_ASYNC_FLAGS_SENTINEL_MASK ),  R_NOOP() );

    Async->Flags |= R_F_OSPL_ASYNC_FLAGS_SENTINEL_VALUE;
#endif
}


/***********************************************************************
* Implement: R_OSPL_FTIMER_InitializeIfNot
************************************************************************/
errnum_t  R_OSPL_FTIMER_InitializeIfNot( r_ospl_ftimer_spec_t *const  out_Specification )
{
    /* ->SEC M1.11.1 : Can not const "is_initialized" because timing */
    bool_t  is_initialized;
    /* <-SEC M1.11.1 */
#if  R_OSPL_FTIMER_IS == R_OSPL_FTIMER_IS_MTU2_1_2
    enum { bits_CST1_CST2 = 0x06 };
#endif

    /* ->QAC 0306 */
#if  R_OSPL_FTIMER_IS == R_OSPL_FTIMER_IS_OSTM0
    struct st_ostm *const  reg_OSTM = &OSTM0;
#elif  R_OSPL_FTIMER_IS == R_OSPL_FTIMER_IS_OSTM1
    struct st_ostm *const  reg_OSTM = &OSTM1;
#endif
    /* <-QAC 0306 */

    /* ->QAC 0306 */
#if  R_OSPL_FTIMER_IS == R_OSPL_FTIMER_IS_OSTM0
    R_OSPL_SET_TO_8_BIT_REGISTER( &CPG.STBCR5, CPG_STBCR5_MSTP51,
                                  CPG_STBCR5_MSTP51_SHIFT, 0 );

#elif  R_OSPL_FTIMER_IS == R_OSPL_FTIMER_IS_OSTM1
    R_OSPL_SET_TO_8_BIT_REGISTER( &CPG.STBCR5, CPG_STBCR5_MSTP50,
                                  CPG_STBCR5_MSTP50_SHIFT, 0 );

#elif  R_OSPL_FTIMER_IS == R_OSPL_FTIMER_IS_MTU2_1_2
    R_OSPL_SET_TO_8_BIT_REGISTER( &CPG.STBCR3, CPG_STBCR3_MSTP33,
                                  CPG_STBCR3_MSTP33_SHIFT, 0 );
#endif

#if  R_OSPL_FTIMER_IS == R_OSPL_FTIMER_IS_MTU2_1_2
    is_initialized = IS_ALL_BITS_NOT_SET( MTU2.TSTR, bits_CST1_CST2 );
#else
    is_initialized = ( (int_fast32_t) reg_OSTM->OSTMnTE == 0 );
#endif
    /* <-QAC 0306 */

    if ( IS( is_initialized ) ) {  /* Integer Promotions */
#if  R_OSPL_FTIMER_IS != R_OSPL_FTIMER_IS_MTU2_1_2
        enum {  free_running_no_interrupt = 2 };
#endif
        bool_t  was_all_enabled; /* = false; */ /* QAC 3197 */

        was_all_enabled = R_OSPL_DisableAllInterrupt();

#if  R_OSPL_FTIMER_IS == R_OSPL_FTIMER_IS_MTU2_1_2
        {
            enum { external_clock_TCLKD = 0x07u };

            /* ->QAC 0306 */

            /* Channel 1 */
            MTU2.TCR_1  = (uint8_t) external_clock_TCLKD;  /* overflow of the timer2 */
            MTU2.TMDR_1 = 0;
            MTU2.TIOR_1 = 0;
            MTU2.TIER_1 = 0;
            MTU2.TSR_1  = 0;
            MTU2.TCNT_1 = 0;

            /* Channel 2 */
            MTU2.TCR_2  = 0;  /* 33MHz */
            MTU2.TMDR_2 = 0;
            MTU2.TIOR_2 = 0;
            MTU2.TIER_2 = 0;
            MTU2.TSR_2  = 0;
            MTU2.TCNT_2 = 0;

            /* Timer start */
            /* MTU2.TSTR |= bits_CST1_CST2; */
            {
                uint8_t const  value = MTU2.TSTR;
                MTU2.TSTR = (uint8_t)( (uint_fast32_t) value | bits_CST1_CST2 );
            } /* QAC 2100 */

            /* <-QAC 0306 */
        }
#else
        if ( (int_fast32_t) reg_OSTM->OSTMnTE == 0 ) {  /* Integer Promotions */
            reg_OSTM->OSTMnCTL = free_running_no_interrupt;
            reg_OSTM->OSTMnTS = 1;
        }
#endif

        if ( IS( was_all_enabled ) ) {
            R_OSPL_EnableAllInterrupt();
        }
    }

    R_OSPL_FTIMER_GetSpecification( out_Specification );

    return  0;
}


/***********************************************************************
* Implement: R_OSPL_FTIMER_GetSpecification
************************************************************************/
void  R_OSPL_FTIMER_GetSpecification( r_ospl_ftimer_spec_t *const  out_Specification )
{
    if ( out_Specification != NULL ) {
        enum { msec_numerator   = 1 };      /* SEC M1.10.1, QAC-3132 */
        enum { msec_denominator = 33333 };  /* SEC M1.10.1, QAC-3132 */
        static const uint32_t  max_count = 0xFFFFFFFFu;  /* SEC M1.10.1, QAC-3132 */
        static const uint32_t  extension_of_count = (1000 * msec_denominator) / msec_numerator;
        /* SEC M1.10.1, QAC-3132 */

        out_Specification->msec_Numerator   = msec_numerator;
        out_Specification->msec_Denominator = msec_denominator;
        out_Specification->MaxCount = max_count;
        out_Specification->ExtensionOfCount = extension_of_count;
    }
}


/***********************************************************************
* Implement: R_OSPL_FTIMER_Get
************************************************************************/
uint32_t  R_OSPL_FTIMER_Get(void)
{
#if  R_OSPL_FTIMER_IS == R_OSPL_FTIMER_IS_MTU2_1_2
    enum { num_16bit = 16 }; /* SEC M1.10.1, QAC-3132 */
    /* ->QAC 0306 */

    uint32_t  now_high = (uint32_t)(MTU2.TCNT_1);
    uint32_t  not_low  = (uint32_t)(MTU2.TCNT_2);

    while( now_high != (uint32_t)(MTU2.TCNT_1) ) {
        /* If higher byte was changed while reading lower byte, re-read. */
        now_high = (uint32_t)(MTU2.TCNT_1);
        not_low  = (uint32_t)(MTU2.TCNT_2);
    }

    return  ( now_high << num_16bit ) | not_low;

    /* <-QAC 0306 */
#else

    /* ->QAC 0306 */
#if  R_OSPL_FTIMER_IS == R_OSPL_FTIMER_IS_OSTM0
    struct st_ostm *const  reg_OSTM = &OSTM0;
#elif  R_OSPL_FTIMER_IS == R_OSPL_FTIMER_IS_OSTM1
    struct st_ostm *const  reg_OSTM = &OSTM1;
#endif
    /* <-QAC 0306 */

    ASSERT_R( reg_OSTM->OSTMnCMP == 0x0000000u,  R_NOOP() );
    /* If "OSTMnCMP != 0", OSTMnCNT is set from 0 to OSTMnCMP-1 */

    return  reg_OSTM->OSTMnCNT;

#endif
}


/***********************************************************************
* Implement: R_OSPL_FTIMER_IsPast
************************************************************************/
errnum_t  R_OSPL_FTIMER_IsPast( const r_ospl_ftimer_spec_t *const  ts,
                                uint32_t const  Now,  uint32_t const  TargetTime,  bool_t *const  out_IsPast )
{
    uint32_t const         target_minus_now = TargetTime - Now;
    static const uint32_t  minus_flag = 0x80000000u;
    errnum_t       e;
    bool_t const   is_past = IS_BIT_SET( target_minus_now, minus_flag );

    IF_DQ( ts == NULL ) {
        e=E_OTHERS;
        goto fin;
    }
    IF_DQ( out_IsPast == NULL ) {
        e=E_OTHERS;
        goto fin;
    }

    if ( IS( is_past ) ) {
        uint32_t const  now_minus_target = Now - TargetTime;

        IF ( now_minus_target > ts->ExtensionOfCount ) {
            e=E_TIME_OUT;
            goto fin;
        }
    }

    *out_IsPast = is_past;

    e=0;
fin:
    return  e;
}


/***********************************************************************
* Implement: R_OSPL_TABLE_InitConst
************************************************************************/
void  R_OSPL_TABLE_InitConst( r_ospl_table_t *const  self,
                              void *const  Area,  size_t const  AreaByteSize,  bool_t const  Is_T_Lock )
{
#if  ! defined( R_OSPL_NDEBUG ) ||  R_OSPL_IS_PREEMPTION
    errnum_t  e;
#endif
#if R_OSPL_IS_PREEMPTION
    bool_t    is_lock = false;
#endif

    IF_DQ( self == NULL ) {
#ifndef R_OSPL_NDEBUG
        e=E_OTHERS;
#endif
        goto fin;
    }

#if R_OSPL_IS_PREEMPTION
    if ( IS( Is_T_Lock ) ) {
        if ( R_OSPL_THREAD_GetCurrentId() != NULL ) {  /* If not interrrupt context */
            e= R_OSPL_Start_T_Lock();
            if(e!=0) {
                goto fin;
            }
            is_lock = true;  /* T-Lock to "self" */
        }
    }
#endif

    self->Area       = Area;
    self->Count      = 0;
    self->MaxCount   = (int_fast32_t)( AreaByteSize / sizeof( r_ospl_table_block_t ) );
    self->KeyCache   = NULL;
    self->FirstFreeIndex = 0;
#if R_OSPL_IS_PREEMPTION
    self->Is_T_Lock  = Is_T_Lock;
#else
    R_UNREFERENCED_VARIABLE( Is_T_Lock );
#endif

    /* Set "self->Area[].NextFreeIndex" */
    {
        r_ospl_table_block_t *const  block_array = (r_ospl_table_block_t *) Area;
        int_fast32_t                 index;
        int_fast32_t const           max_index = self->MaxCount - 1;

        IF_DQ( block_array == NULL ) {
#ifndef R_OSPL_NDEBUG
            e=E_OTHERS;
#endif
            goto fin;
        }

        for ( index = 0;  index < max_index;  index += 1 ) {
            block_array[ index ].NextFreeIndex = (int16_t)( index + 1 );
        }
        block_array[ max_index ].NextFreeIndex = (int16_t) R_OSPL_TABLE_BLOCK_NO_NEXT;
    }

#ifndef R_OSPL_NDEBUG
    e=0;
#endif
fin:
    R_NOOP();/* for following all #if is false */

#if R_OSPL_IS_PREEMPTION
    if ( IS( is_lock ) ) {
        R_OSPL_End_T_Lock();
    }
#endif
#ifndef R_OSPL_NDEBUG
    if ( e != 0 ) {
        R_DebugBreak( NULL, e );
    }
#endif
}


/***********************************************************************
* Implement: R_OSPL_TABLE_GetIndex
************************************************************************/
errnum_t  R_OSPL_TABLE_GetIndex( r_ospl_table_t *const  self,  const void *const  Key,
                                 int_fast32_t *const  out_Index,  r_ospl_if_not_t const  TypeOfIfNot )
{
    errnum_t  e;
#if R_OSPL_IS_PREEMPTION
    bool_t    is_lock = false;
#endif

    IF_DQ( self == NULL ) {
        e=E_OTHERS;
        goto fin;
    }
    IF_DQ( out_Index == NULL ) {
        e=E_OTHERS;
        goto fin;
    }

#if R_OSPL_IS_PREEMPTION
    if ( IS( self->Is_T_Lock ) ) {
        if ( R_OSPL_THREAD_GetCurrentId() != NULL ) {  /* If not interrrupt context */
            e= R_OSPL_Start_T_Lock();
            if(e!=0) {
                goto fin;
            }
            is_lock = true;  /* T-Lock to "self" */
        }
    }
#endif


    if ( (Key == self->KeyCache)  &&  (self->Count >= 1) ) {
        *out_Index = self->IndexCache;
    } else {
        r_ospl_table_searched_t      searched;
        r_ospl_table_block_t *const  block_array = self->Area;
        int_fast32_t                 found_index;

        R_OSPL_TABLE_Search_Sub( self, Key, &searched );

        if ( ! searched.IsFound ) {
            int_fast32_t  so_index;  /* sorted_key_index */
            int_fast32_t  first_index;  /* FirstFreeIndex */

            if ( TypeOfIfNot == R_OSPL_ALLOCATE_IF_NOT ) {
                if ( self->Count == self->MaxCount ) {
                    e = E_FEW_ARRAY;
                    goto fin;
                }

                /* Insert and Set "block_array[ searched.SortedKeyIndex ].Key" */
                for ( so_index = self->Count - 1;
                        so_index >= searched.SortedKeyIndex;
                        so_index -= 1 ) {
                    block_array[ so_index + 1 ].Key   = block_array[ so_index ].Key;
                    block_array[ so_index + 1 ].Index = block_array[ so_index ].Index;
                }
                block_array[ searched.SortedKeyIndex ].Key = Key;
                self->Count += 1;

                /* Set "block_array[ searched.SortedKeyIndex ].Index" */
                first_index = self->FirstFreeIndex;
                block_array[ searched.SortedKeyIndex ].Index = (int16_t) first_index;
                self->FirstFreeIndex = block_array[ first_index ].NextFreeIndex;
                block_array[ first_index ].NextFreeIndex = R_OSPL_TABLE_BLOCK_USED;

            } else if ( TypeOfIfNot == R_OSPL_ERROR_IF_NOT ) {
                e = E_NOT_FOUND_SYMBOL;
                goto fin;
            } else { /* TypeOfIfNot == R_OSPL_DO_NOTHING_IF_NOT */
                e = 0;
                goto fin;
            }
            /* Don't use "IF", Because this function called from "IF". */
        }

        self->KeyCache = Key;
        found_index = block_array[ searched.SortedKeyIndex ].Index;
        self->IndexCache = found_index;
        *out_Index = found_index;
    }

    e=0;
fin:
#if R_OSPL_IS_PREEMPTION
    if ( IS( is_lock ) ) {
        R_OSPL_End_T_Lock();
    }
#endif
    return  e;
}


/***********************************************************************
* Implement: R_OSPL_TABLE_Free
************************************************************************/
void  R_OSPL_TABLE_Free( r_ospl_table_t *const  self,  const void *const  Key )
{
#if  ! defined( R_OSPL_NDEBUG ) ||  R_OSPL_IS_PREEMPTION
    errnum_t  e;
#endif
#if R_OSPL_IS_PREEMPTION
    bool_t    is_lock = false;
#endif
    r_ospl_table_searched_t  searched;

    IF_DQ( self == NULL ) {
#ifndef R_OSPL_NDEBUG
        e=0;
#endif
        goto fin;
    }

#if R_OSPL_IS_PREEMPTION
    if ( R_OSPL_THREAD_GetCurrentId() != NULL ) {  /* If not interrrupt context */
        e= R_OSPL_Start_T_Lock();
        if(e!=0) {
            goto fin;
        }
        is_lock = true;  /* T-Lock to "gs_global_error" */
    }
#endif

    R_OSPL_TABLE_Search_Sub( self, Key, &searched );

    if ( IS( searched.IsFound ) ) {
        r_ospl_table_block_t *const  block_array = self->Area;
        int_fast32_t                 so_index;  /* sorted_key_index */
        int_fast32_t                 first_index;  /* FirstFreeIndex */
        int_fast32_t const           count_ = self->Count;  /* _ is for MISRA 5.6 */

        /* Set "FirstFreeIndex", ".NextFreeIndex" */
        first_index = self->FirstFreeIndex;
        self->FirstFreeIndex = block_array[ searched.SortedKeyIndex ].Index;
        block_array[ self->FirstFreeIndex ].NextFreeIndex = (int16_t) first_index;

        /* Remove one "r_ospl_table_block_t" */
        /* Set "self->KeyCache" */
        if ( searched.SortedKeyIndex < count_ ) {
            for ( so_index = searched.SortedKeyIndex;  so_index < count_ ;  so_index += 1 ) {
                block_array[ so_index ].Key   = block_array[ so_index + 1 ].Key;
                block_array[ so_index ].Index = block_array[ so_index + 1 ].Index;
            }
            self->KeyCache   = block_array[ searched.SortedKeyIndex ].Key;
            self->IndexCache = block_array[ searched.SortedKeyIndex ].Index;
        } else if ( searched.SortedKeyIndex > 0 ) { /* searched.SortedKeyIndex >= 1 */
            self->KeyCache   = block_array[ searched.SortedKeyIndex - 1 ].Key;
            self->IndexCache = block_array[ searched.SortedKeyIndex - 1 ].Index;
        } else {
            self->KeyCache = NULL;
        }
        self->Count -= 1;
    }

#if  ! defined( R_OSPL_NDEBUG )
    e=0;
#endif
fin:
    R_NOOP();/* for following all #if is false */

#if R_OSPL_IS_PREEMPTION
    if ( IS( is_lock ) ) {
        R_OSPL_End_T_Lock();
    }
#endif
#ifndef R_OSPL_NDEBUG
    if ( e != 0 ) {
        R_DebugBreak( NULL, e );
    }
#endif
}


/***********************************************************************
* Implement: R_OSPL_TABLE_Search_Sub
************************************************************************/
static void  R_OSPL_TABLE_Search_Sub( const r_ospl_table_t *const  self,
                                      const void *const  Key,  r_ospl_table_searched_t *const  out_Searched )
{
    int_fast32_t     left;
    int_fast32_t     right;
    int_fast32_t     middle;
    /* ->QAC 0306 : Sort by pointer value */
    uintptr_t const  target_key = (uintptr_t) Key;
    /* <-QAC 0306 */
    uintptr_t        middle_key;
    enum {           num_2 = 2 };  /* SEC M1.10.1, QAC-3132 */
    r_ospl_table_block_t  *array;

    IF_DQ( self == NULL ) {
        goto fin;
    }
    IF_DQ( out_Searched == NULL ) {
        goto fin;
    }

    array = self->Area;


    if ( self->Count == 0 ) {
        out_Searched->SortedKeyIndex = 0;
        out_Searched->IsFound = false;
    } else {
        left = 0;
        right = self->Count - 1;

        while ( (right - left) >= num_2 ) {
            middle = (int_fast32_t)( (uint_fast32_t)( right + left ) / num_2 );
            /* ->QAC 0306 : Sort by pointer value */
            middle_key = (uintptr_t) array[ middle ].Key;
            /* <-QAC 0306 */

            if ( target_key == middle_key ) {
                out_Searched->SortedKeyIndex = middle;
                out_Searched->IsFound = true;
                goto fin;
            } else if ( target_key <  middle_key ) {
                right = (int_fast32_t)( middle - 1 );
            } else {
                left = (int_fast32_t)( middle + 1 );
            }
        }

        /* ->QAC 0306 : Sort by pointer value */
        if ( target_key == (uintptr_t) array[ left ].Key ) {
            out_Searched->SortedKeyIndex = left;
            out_Searched->IsFound = true;
        } else if ( target_key == (uintptr_t) array[ right ].Key ) {
            out_Searched->SortedKeyIndex = right;
            out_Searched->IsFound = true;
        } else if ( target_key < (uintptr_t) array[ left ].Key ) {
            out_Searched->SortedKeyIndex = left;
            out_Searched->IsFound = false;
        } else if ( target_key > (uintptr_t) array[ right ].Key ) {
            out_Searched->SortedKeyIndex = right + 1;
            out_Searched->IsFound = false;
        } else {
            out_Searched->SortedKeyIndex = right;
            out_Searched->IsFound = false;
        }
        /* <-QAC 0306 */
    }

fin:
    return;
}


/***********************************************************************
* Implement: R_OSPL_TABLE_Print
************************************************************************/
#if R_OSPL_DEBUG_TOOL
void  R_OSPL_TABLE_Print( r_ospl_table_t *const  self )
{
    int_fast32_t  so_index;  /* sorted_key_index */
    int_fast32_t  index;
    r_ospl_table_block_t *const  array = self->Area;

    printf( "R_OSPL_TABLE_Print: r_ospl_table_t 0x%08X\n", (uintptr_t) self );
    printf( "    .Count = %d\n", self->Count );
    for ( so_index = 0;  so_index < self->Count;  so_index += 1 ) {
        printf( "    .Area[%d].Key, Index = 0x%08X, %d\n",
                so_index,  (uintptr_t) array[ so_index ].Key,  array[ so_index ].Index );
    }
    printf( "    .FirstFreeIndex = %d\n", self->FirstFreeIndex );
    for ( index = 0;  index < self->MaxCount;  index += 1 ) {
        printf( "    .Area[%d].NextFreeIndex = %d\n", index,  array[ index ].NextFreeIndex );
    }
    printf( "        %d = R_OSPL_TABLE_BLOCK_NO_NEXT\n", R_OSPL_TABLE_BLOCK_NO_NEXT );
    printf( "        %d = R_OSPL_TABLE_BLOCK_USED\n", R_OSPL_TABLE_BLOCK_USED );
}
#endif


/***********************************************************************
* Implement: R_OSPL_CallInterruptCallback
************************************************************************/
void  R_OSPL_CallInterruptCallback( const r_ospl_caller_t *const  self,
                                    const r_ospl_interrupt_t *const  InterruptSource )
{
    errnum_t  e;
    r_ospl_async_t  *async;
#if ! R_OSPL_IS_PREEMPTION
    r_ospl_master_t *const  gs_master = R_OSPL_GetPrivateContext();
    r_ospl_thread_def_t  *current_thread = NULL;  /* NULL is for avoid QAC 3353 */
#endif
#if R_OSPL_ERROR_BREAK
    static int_fast32_t  gs_nested_interrupt_level = 0;
#endif

#if R_OSPL_ERROR_BREAK
    gs_nested_interrupt_level += 1;
#endif

#if ! R_OSPL_IS_PREEMPTION
    IF_DQ( gs_master == NULL ) {
        e=E_OTHERS;
        goto fin;
    }

#if R_OSPL_CPU_LOAD
    e= gs_master->IdleCallback( R_OSPL_INTERRUPT_START );
    IF(e) {
        goto fin;
    }
#endif

    current_thread = gs_master->CurrentThread;
#endif

    IF_DQ( self == NULL ) {
        e=E_OTHERS;
        goto fin;
    }
    IF_DQ( self->Async == NULL ) {
        e=E_OTHERS;
        goto fin;
    }

    async = self->Async;

    ASSERT_D( ( async->Flags & R_F_OSPL_ASYNC_FLAGS_SENTINEL_MASK ) ==
              R_F_OSPL_ASYNC_FLAGS_SENTINEL_VALUE,  e=E_OTHERS; goto fin );
    /* If failed, memory area of "Async" variable was overwritten by other variable. */
    /* Reason of failed may be not disabled interrupt */
    /* or not called "*_I_LOCK_Unlock()" disabling interrupt by "*_I_LOCK_Disable()". */

    IF_DQ( self->PointerToState == NULL ) {
        e=E_OTHERS;
        goto fin;
    }
    IF_DQ( async->InterruptCallback == NULL ) {
        e=E_OTHERS;
        goto fin;
    }

#if ! R_OSPL_IS_PREEMPTION
    gs_master->CurrentThread = NULL;  /* Interrupt */
#endif

    *self->PointerToState = self->StateValueOfOnInterrupting;



    e= async->InterruptCallback( InterruptSource, self );  /* Main of callback */



    if ( e != 0 ) {
        if ( async->ReturnValue == 0 ) {
            async->ReturnValue = e;
        }
        R_OSPL_CLEAR_ERROR();
    }

    e=0;
fin:
#if ! R_OSPL_IS_PREEMPTION
    IF_DQ ( gs_master == NULL ) {}
    else {
        gs_master->CurrentThread = current_thread;

#if R_OSPL_CPU_LOAD
        {
            errnum_t  ee;

            ee= gs_master->IdleCallback( R_OSPL_INTERRUPT_END );
            e= R_OSPL_MergeErrNum( e, ee );
        }
#endif
    }
#endif


#if R_OSPL_ERROR_BREAK
    gs_nested_interrupt_level -= 1;

    if ( gs_nested_interrupt_level == 0 ) {
        if ( gs_master != NULL ) {
            current_thread = gs_master->CurrentThread;
            gs_master->CurrentThread = NULL;
        }


        R_DEBUG_BREAK_IF_ERROR();


        if ( gs_master != NULL ) {
            gs_master->CurrentThread = current_thread;
        }
    }
#endif
    R_UNREFERENCED_VARIABLE( e );
}


/***********************************************************************
* Implement: R_OSPL_SetErrNum
************************************************************************/
#if  R_OSPL_TLS_ERROR_CODE
void  R_OSPL_SetErrNum( errnum_t const  e )
{
    r_ospl_error_t  *err;

    if ( e != 0 ) {

        err = R_OSPL_GetCurrentThreadError_Sub( R_OSPL_ALLOCATE_IF_NOT );

        if ( err != NULL ) {
            if ( err->ErrNum == 0 ) {
                err->ErrNum = e;
            }
        }
    }
}
#endif


/***********************************************************************
* Implement: R_OSPL_GetErrNum
************************************************************************/
#if  R_OSPL_TLS_ERROR_CODE
errnum_t  R_OSPL_GetErrNum(void)
{
    errnum_t         e;
    r_ospl_error_t  *err;

    err = R_OSPL_GetCurrentThreadError_Sub( R_OSPL_ALLOCATE_IF_NOT );

    if ( err != NULL ) {
        e = err->ErrNum;
    } else {
        e = E_NO_DEBUG_TLS;
    }

    return  e;
}
#endif


/***********************************************************************
* Implement: R_OSPL_CLEAR_ERROR
************************************************************************/
#if R_OSPL_ERROR_BREAK  ||  R_OSPL_TLS_ERROR_CODE
void  R_OSPL_CLEAR_ERROR(void)
{
    r_ospl_error_t  *err;

    err = R_OSPL_GetCurrentThreadError_Sub( R_OSPL_DO_NOTHING_IF_NOT );

    if ( err != NULL ) {
#if R_OSPL_ERROR_BREAK
        err->IsError = false;
#endif

#if R_OSPL_TLS_ERROR_CODE
        err->ErrNum = 0;
#endif
    }
}
#endif


/**
* @brief   Function part of error break
*
* @par Parameters
*    None
* @return  None.
*/
#if R_OSPL_ERROR_BREAK
static bool_t  R_OSPL_OnRaisingError_Sub( const char_t *const  FilePath,
        int_fast32_t const  LineNum );  /* QAC-3450 */
static bool_t  R_OSPL_OnRaisingError_Sub( const char_t *const  FilePath,
        int_fast32_t const  LineNum )
{
#if  ! defined( R_OSPL_NDEBUG ) ||  R_OSPL_IS_PREEMPTION
    errnum_t         e;
#endif
    bool_t           is_break = false;
#if R_OSPL_IS_PREEMPTION
    bool_t           is_lock = false;
#endif
    r_ospl_error_t  *err;


    err = R_OSPL_GetCurrentThreadError_Sub( R_OSPL_ALLOCATE_IF_NOT );


    if ( err != NULL ) {
        if ( IS( err->IsError ) ) {
            is_break = false;
        } else {
#if R_OSPL_IS_PREEMPTION
            if ( R_OSPL_THREAD_GetCurrentId() != NULL ) {  /* If not interrrupt context */
                e= R_OSPL_Start_T_Lock();
                if(e!=0) {
                    goto fin;
                }
                is_lock = true;  /* T-Lock to "gs_global_error" */
            }
#endif

            gs_global_error.RaisedGlobalErrorID ++;
            is_break = ( gs_global_error.RaisedGlobalErrorID ==
                         gs_global_error.BreakGlobalErrorID );

            err->IsError = true;
            err->ErrorID = gs_global_error.RaisedGlobalErrorID;
            err->FilePath = FilePath;
            err->LineNum = LineNum;
        }
    }

#ifndef R_OSPL_NDEBUG
    e=0;
#endif
#if R_OSPL_IS_PREEMPTION
fin:
    if ( IS( is_lock ) ) {
        R_OSPL_End_T_Lock();
    }
#endif
#ifndef R_OSPL_NDEBUG
    if ( e != 0 ) {
        R_DebugBreak( NULL, e );
    }
#endif
    return  is_break;
}
#endif


/***********************************************************************
* Implement: R_OSPL_OnRaisingErrorForMISRA
************************************************************************/
#if R_OSPL_ERROR_BREAK
bool_t  R_OSPL_OnRaisingErrorForMISRA( bool_t const  Condition,  const char_t *const  File,
                                       int_t const  Line )
{
    if ( IS( Condition ) ) {
        if ( R_OSPL_OnRaisingError_Sub( File, Line ) != 0 ) {
            R_DebugBreak( File, Line );
        }
    }
    return  Condition;
}
#endif


/**
* @brief   Function part of <R_DEBUG_BREAK_IF_ERROR>
*
* @par Parameters
*    None
* @return  None.
*/
#if R_OSPL_ERROR_BREAK
void  R_OSPL_DebugBreakIfError( const char_t *const  File,  int_t const  Line )
{
    r_ospl_error_t  *err;

    err = R_OSPL_GetCurrentThreadError_Sub( R_OSPL_DO_NOTHING_IF_NOT );

    if ( err != NULL  &&  err->IsError ) {
        /* See the value of "err->ErrorID" in watch window. */
        /* Call R_OSPL_SET_BREAK_ERROR_ID( N ); at main function. */
        printf( "<ERROR error_ID=\"0x%X\" file=\"%s(%d)\"/>\n",
                err->ErrorID, err->FilePath, err->LineNum );
        R_DebugBreak( File, Line );
    }

    R_OSPL_CLEAR_ERROR();
}
#endif


/***********************************************************************
* Implement: R_OSPL_GET_ERROR_ID
************************************************************************/
#if R_OSPL_ERROR_BREAK
int_fast32_t  R_OSPL_GET_ERROR_ID(void)
{
    return  gs_global_error.RaisedGlobalErrorID;
}
#endif


/***********************************************************************
* Implement: R_OSPL_SET_BREAK_ERROR_ID
************************************************************************/
#if R_OSPL_ERROR_BREAK
void  R_OSPL_SET_BREAK_ERROR_ID( int_fast32_t const  ErrorID )
{
#ifndef R_OSPL_NDEBUG
    if ( gs_global_error.BreakGlobalErrorID != ErrorID ) {
        printf( ">R_OSPL_SET_BREAK_ERROR_ID( %d );\n", ErrorID );
    }
#endif

    gs_global_error.BreakGlobalErrorID = ErrorID;
}
#endif


/***********************************************************************
* Implement: R_OSPL_GetCurrentThreadError
************************************************************************/
#if R_OSPL_ERROR_BREAK  ||  R_OSPL_TLS_ERROR_CODE
r_ospl_error_t  *R_OSPL_GetCurrentThreadError(void)
{
    return  R_OSPL_GetCurrentThreadError_Sub( R_OSPL_ALLOCATE_IF_NOT );
}
#endif


/**
* @brief   GetCurrentThreadError
*
* @param   TypeOfIfNot <r_ospl_if_not_t>
* @return  Error information of current thread
*/
#if R_OSPL_ERROR_BREAK  ||  R_OSPL_TLS_ERROR_CODE
static r_ospl_error_t  *R_OSPL_GetCurrentThreadError_Sub( r_ospl_if_not_t const  TypeOfIfNot )
{
    errnum_t            e;
    int_fast32_t        index;
    r_ospl_error_t     *err = NULL;
    r_ospl_thread_id_t  th;  /* QAC 3441 */

    index = R_OSPL_NO_INDEX;

    th = R_OSPL_THREAD_GetCurrentId();
    e= R_OSPL_TABLE_GetIndex( &gs_global_error.ThreadIndexTable, th, &index, TypeOfIfNot );
    if ( e == E_FEW_ARRAY ) {
#ifndef R_OSPL_NDEBUG
        if ( gs_global_error.ThreadIndexTable.Area == gs_thread_index_table_body ) {
            e = E_NO_DEBUG_TLS;
        }
#endif
        goto fin;
    }
    if ( e != 0 ) {
        goto fin;
    }

    if ( index != R_OSPL_NO_INDEX ) {
        IF_DQ( gs_global_error.ErrorArray == NULL ) {
#ifndef R_OSPL_NDEBUG
            e=E_OTHERS;
#endif
            goto fin;
        }
        IF_DQ( index < -1 ) {
#ifndef R_OSPL_NDEBUG
            e=E_OTHERS;
#endif
            goto fin;
        }

        err = &gs_global_error.ErrorArray[ index ];
    }

#ifndef R_OSPL_NDEBUG
    e=0;
#endif
fin:
#ifndef R_OSPL_NDEBUG
    if ( e != 0 ) {
        R_DebugBreak( NULL, e );
        /* If e == 2 (E_FEW_ARRAY), Set big work area by "R_OSPL_SET_DEBUG_WORK". */
        /* If e == 29 (E_NO_DEBUG_TLS), Call "R_OSPL_SET_DEBUG_WORK". */
    }
#endif

    return  err;
}
#endif


/***********************************************************************
* Implement: R_OSPL_SET_DEBUG_WORK
************************************************************************/
#if R_OSPL_ERROR_BREAK
void  R_OSPL_SET_DEBUG_WORK( void *const  WorkArea,  uint32_t const  WorkAreaSize )
{
    errnum_t               e;
    int_fast32_t           count = WorkAreaSize / R_OSPL_DEBUG_WORK_1_SIZE;
    uint8_t               *work = WorkArea;
    r_ospl_table_block_t  *thread_index_array;
    r_ospl_error_t        *error_array;
    int_fast32_t           index;
#if R_OSPL_IS_PREEMPTION
    bool_t                 is_lock = false;
#endif

#if R_OSPL_IS_PREEMPTION
    if ( R_OSPL_THREAD_GetCurrentId() != NULL ) {  /* If not interrrupt context */
        e= R_OSPL_Start_T_Lock();
        if(e!=0) {
            goto fin;
        }
        is_lock = true;  /* T-Lock to "gs_global_error" */
    }
#endif


    if ( gs_global_error.ThreadIndexTable.Area != gs_thread_index_table_body ) {
        e = E_ACCESS_DENIED;
        goto fin;
    }

    thread_index_array = (r_ospl_table_block_t *) &work[0];
    error_array = (r_ospl_error_t *) &thread_index_array[ count ];
    ASSERT_D( (uint8_t *) &error_array[ count ] <= &work[ WorkAreaSize ],  e=E_FEW_ARRAY; goto fin );

    R_OSPL_TABLE_InitConst( &gs_global_error.ThreadIndexTable,
                            thread_index_array,  count * sizeof( thread_index_array[0] ), true );

    e= R_OSPL_TABLE_GetIndex( &gs_global_error.ThreadIndexTable,
                              gs_thread_index_table_body[0].Key, &index, R_OSPL_ALLOCATE_IF_NOT );
    if(e!=0) {
        goto fin;
    }

    gs_global_error.ErrorArray = error_array;
    gs_global_error.ErrorArray[ index ] = gs_error;

#ifndef R_OSPL_NDEBUG
    memset( gs_thread_index_table_body, 0xFE, sizeof(gs_thread_index_table_body) );
    memset( &gs_error, 0xFE, sizeof(gs_error) );
#endif

    e=0;
fin:
#if R_OSPL_IS_PREEMPTION
    if ( IS( is_lock ) ) {
        R_OSPL_End_T_Lock();
    }
#endif
#ifndef R_OSPL_NDEBUG
    if ( e != 0 ) {
        R_DebugBreak( NULL, e );
    }
#endif
}
#endif


/***********************************************************************
* Implement: R_OSPL_MODIFY_THREAD_LOCKED_COUNT
************************************************************************/
#if R_OSPL_ERROR_BREAK  &&  R_OSPL_IS_PREEMPTION
void  R_OSPL_MODIFY_THREAD_LOCKED_COUNT( int_fast32_t Plus )
{
    r_ospl_error_t  *err;

    err = R_OSPL_GetCurrentThreadError_Sub( R_OSPL_ALLOCATE_IF_NOT );

    if ( err != NULL ) {
        err->ThreadLockedCount += Plus;
    }
}
#endif


/***********************************************************************
* Implement: R_OSPL_GET_THREAD_LOCKED_COUNT
************************************************************************/
#if R_OSPL_ERROR_BREAK  &&  R_OSPL_IS_PREEMPTION
int_fast32_t  R_OSPL_GET_THREAD_LOCKED_COUNT(void)
{
    int_fast32_t  count;
    r_ospl_error_t  *err;

    err = R_OSPL_GetCurrentThreadError_Sub( R_OSPL_ALLOCATE_IF_NOT );

    if ( err != NULL ) {
        count = err->ThreadLockedCount;
    } else {
        count = 0;
    }

    return  count;
}
#endif


