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
* @file  r_ospl_RTX.c
* @brief   OS Porting Layer API for RTX
*
* $Module: OSPL $ $PublicVersion: 0.90 $ (=R_OSPL_VERSION)
* $Rev: 35 $
* $Date:: 2014-04-15 21:38:18 +0900#$
*/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_ospl.h"
#include  "r_ospl_os_less_private.h"
#include  "r_ospl_private.h"
#include  "core_ca.h"
#if R_OSPL_IS_PREEMPTION
#include  "cmsis_os.h"
#include  "r_ospl_RTX_private.h"
#endif


/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/**
* @def  OS_ERROR_SIGNAL
* @brief  CMSIS-RTOS defined immediate value
* @par Parameters
*    None
* @return  None.
*/
#define  OS_ERROR_SIGNAL  0x80000000


/**
* @def  R_OSPL_EVENT_WATCH
* @brief  Debug tool
* @par Parameters
*    None
* @return  None.
*/
#define  R_OSPL_EVENT_WATCH  0  /* [R_OSPL_EVENT_WATCH] 0 or 1 */


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
* Implement: R_OSPL_Initialize
************************************************************************/
errnum_t  R_OSPL_Initialize( const void *const  in_NullConfig )
{
    R_UNREFERENCED_VARIABLE( in_NullConfig );
    return  0;
}


/***********************************************************************
* Implement: R_OSPL_THREAD_GetCurrentId
************************************************************************/
r_ospl_thread_id_t  R_OSPL_THREAD_GetCurrentId(void)
{
    return  osThreadGetId();
}


/***********************************************************************
* Implement: R_OSPL_EVENT_Set
************************************************************************/
void  R_OSPL_EVENT_Set( r_ospl_thread_id_t const  ThreadId,  bit_flags32_t const  SetFlags )
{
    int32_t  ret;

    if ( ThreadId != NULL ) {
#if  R_OSPL_EVENT_WATCH
        R_D_AddToIntLog( 0x70100000 + SetFlags );
        R_D_AddToIntLog( (uintptr_t) R_OSPL_THREAD_GetCurrentId() );
        R_D_AddToIntLog( (uintptr_t) ThreadId );
#endif

        ret = osSignalSet( (osThreadId_t) ThreadId, (int32_t) SetFlags );
        ASSERT_D( (ret & OS_ERROR_SIGNAL) == 0,  R_NOOP() );
        R_UNREFERENCED_VARIABLE( ret );  /* for Release configuration */
    }
}


/***********************************************************************
* Implement: R_OSPL_EVENT_Clear
************************************************************************/
void  R_OSPL_EVENT_Clear( r_ospl_thread_id_t const  ThreadId,  bit_flags32_t const  ClearFlags1 )
{
    int32_t  ret;

    if ( ThreadId != NULL ) {
        IF_D( ( ClearFlags1 & ~0xFFFF ) != 0 ) {
            R_NOOP();
        }

#if  R_OSPL_EVENT_WATCH
        R_D_AddToIntLog( 0x70C00000 + ClearFlags1 );
        R_D_AddToIntLog( (uintptr_t) R_OSPL_THREAD_GetCurrentId() );
        R_D_AddToIntLog( (uintptr_t) ThreadId );
#endif

        ret = osSignalClear( (osThreadId_t) ThreadId, (int32_t) ClearFlags1 );
        /* "& 0xFFFF" is for avoiding error in osSignalClear */
        ASSERT_D( (ret & OS_ERROR_SIGNAL) == 0,  R_NOOP() );
        R_UNREFERENCED_VARIABLE( ret );  /* for Release configuration */
    }
}


/***********************************************************************
* Implement: R_OSPL_EVENT_Get
************************************************************************/
#if ( ! defined( osCMSIS )  ||  osCMSIS <= 0x10001 ) &&  R_OSPL_VERSION < 85
bit_flags32_t  R_OSPL_EVENT_Get( r_ospl_thread_id_t const  ThreadId )
{
    int32_t  ret;

    if ( ThreadId == NULL ) {
        ret = 0;
    } else {
        ret = osSignalGet( (osThreadId_t) ThreadId );
        ASSERT_D( (ret & OS_ERROR_SIGNAL) == 0,  R_NOOP() );
    }

    return  (bit_flags32_t) ret;
}
#endif


/***********************************************************************
* Implement: R_OSPL_EVENT_Wait
************************************************************************/
errnum_t  R_OSPL_EVENT_Wait( bit_flags32_t const  WaigingFlags,  bit_flags32_t *const  out_GotFlags,
                             uint32_t const  Timeout_msec )
{
    errnum_t  e;
    osEvent   event;

    R_STATIC_ASSERT( R_OSPL_INFINITE == osWaitForever, "" );

#if  R_OSPL_EVENT_WATCH
    R_D_AddToIntLog( 0x70BE0000 + WaigingFlags );
    R_D_AddToIntLog( (uintptr_t) R_OSPL_THREAD_GetCurrentId() );

    {
        static int tc;
        if ( R_D_Counter( &tc, 0, NULL ) ) {
            R_DEBUG_BREAK();
        }
    }
#endif


    event = osSignalWait( (int32_t) WaigingFlags, Timeout_msec );


#if  R_OSPL_EVENT_WATCH
    R_D_AddToIntLog( 0x70AF0000 );
    R_D_AddToIntLog( (uintptr_t) R_OSPL_THREAD_GetCurrentId() );
#endif

    if ( (event.status == osOK) || (event.status == osEventTimeout) ) {
        if ( out_GotFlags != NULL ) {
            *out_GotFlags = R_OSPL_TIMEOUT;
        }
        IF ( event.status == osEventTimeout ) {
            e=E_TIME_OUT;
            goto fin;
        }
    } else {
        ASSERT_R( event.status == osEventSignal,  e=E_OTHERS;  goto fin );

        if ( out_GotFlags != NULL ) {
            *out_GotFlags = (bit_flags32_t) event.value.signals;
        }
    }

    e=0;
fin:
    return  e;
}


/***********************************************************************
* Implement: R_OSPL_SetInterruptPriority
************************************************************************/
errnum_t  R_OSPL_SetInterruptPriority( bsp_int_src_t const  IRQ_Num, int_fast32_t const  Priority )
{
    GIC_SetPriority( IRQ_Num, (uint32_t) Priority );
    return  0;
}


/***********************************************************************
* Implement: R_OSPL_MEMORY_Flush
************************************************************************/
void  R_OSPL_MEMORY_Flush( r_ospl_flush_t const  FlushType )
{
    if ( FlushType == R_OSPL_FLUSH_WRITEBACK_INVALIDATE ) {
#if 0
        printf( "L1Flush\n" );
#endif

#if IS_RTX_USED
        L1C_CleanInvalidateDCacheAll();
#else
#error
#endif
    } else if ( FlushType == R_OSPL_FLUSH_WRITEBACK_INVALIDATE_2ND ) {
#if 0
        printf( "PL310Flush\n" );
#endif

        L2C_CleanInvAllByWay();
    } else {
        ASSERT_D( false,  R_NOOP() );
    }
}


/**
* @brief   R_OSPL_Is1bitOnly_Fast32_Sub
*
* @par Parameters
*    None
* @return  None.
*/
#ifndef  R_OSPL_NDEBUG
static bool_t  R_OSPL_Is1bitOnly_Fast32_Sub( uint_fast32_t Value )
{
    if ( (Value & 0x0000FFFFu) == 0 ) {
        Value >>= 16;
    }
    if ( (Value & 0x000000FFu) == 0 ) {
        Value >>=  8;
    }
    if ( (Value & 0x0000000Fu) == 0 ) {
        Value >>=  4;
    }
    if ( (Value & 0x00000003u) == 0 ) {
        Value >>=  2;
    }
    if ( (Value & 0x00000001u) == 0 ) {
        Value >>=  1;
    }
    return  ( Value == 1 );
}
#endif


/***********************************************************************
* Implement: R_OSPL_MEMORY_RangeFlush
************************************************************************/
errnum_t  R_OSPL_MEMORY_RangeFlush( r_ospl_flush_t const  FlushType,
                                    const void *const  StartAddress,  size_t const  Length )
{
    errnum_t   e;
    size_t     cache_line_size;
    size_t     cache_line_mask;
    uintptr_t  start;
    uintptr_t  over;

    ASSERT_R( FlushType == R_OSPL_FLUSH_INVALIDATE,  e=E_BAD_COMMAND_ID; goto fin );

    cache_line_size = R_OSPL_MEMORY_GetCacheLineSize();
    cache_line_mask = cache_line_size - 1u;
    ASSERT_D( R_OSPL_Is1bitOnly_Fast32_Sub( cache_line_size ), e=E_OTHERS; goto fin );

    /* ->MISRA 11.3 */ /* ->SEC R2.7.1 */
    ASSERT_R( ( (uintptr_t) StartAddress & cache_line_mask ) == 0u, e=E_OTHERS; goto fin );
    ASSERT_R( ( Length & cache_line_mask ) == 0u, e=E_OTHERS; goto fin );

    start = (uintptr_t) StartAddress;
    over  = ((uintptr_t) StartAddress + Length) - 1u;
    /* <-MISRA 11.3 */ /* <-SEC R2.7.1 */

    R_OSPL_MEMORY_RangeFlush_Sub( start, over, cache_line_size );

    e=0;
fin:
    return  e;
}


/***********************************************************************
* Implement: R_OSPL_MEMORY_GetSpecification
************************************************************************/
void  R_OSPL_MEMORY_GetSpecification( r_ospl_memory_spec_t *const  out_MemorySpec )
{
    IF_DQ( out_MemorySpec == NULL ) {
        goto fin;
    }

    out_MemorySpec->CacheLineSize = R_OSPL_MEMORY_GetCacheLineSize();

fin:
    return;
}


/***********************************************************************
* Implement: R_OSPL_Delay
************************************************************************/
errnum_t  R_OSPL_Delay( uint32_t const  DelayTime_msec )
{
    errnum_t   e;
    osStatus_t rs;
    bool_t const    is_overflow = ( DelayTime_msec > R_OSPL_MAX_TIME_OUT );
    uint32_t const  delay_parameter = DelayTime_msec + 1u;

    ASSERT_D( ! is_overflow, R_NOOP() );
    /* RTX 5.16: If delay_parameter = 100000, "osDelay" waits 65534 */

    rs= osDelay( delay_parameter );
    IF ( rs == osErrorISR ) {
        e=E_NOT_THREAD;
        R_OSPL_RaiseUnrecoverable( e );
        goto fin;
    }
    IF ( IS( is_overflow ) ) {
        e=E_TIME_OUT;
        goto fin;
    }

    IF (
        (rs != osOK) &&           /* for delay_parameter == 0 */
        (rs != osEventTimeout) ) { /* for delay_parameter != 0 */
        e=E_OTHERS;
        goto fin;
    }

    e=0;
fin:
    return  e;
}


/***********************************************************************
* Implement: R_OSPL_QUEUE_Create
************************************************************************/
errnum_t  R_OSPL_QUEUE_Create( r_ospl_queue_t **out_self, r_ospl_queue_def_t *QueueDefine )
{
    errnum_t  e;
    r_ospl_queue_t  *self = QueueDefine;

    self->MailQId = osMailCreate( (osMailQDef_t *) QueueDefine->MailQDef, NULL );
    IF ( self->MailQId == NULL ) {
        e=E_OTHERS;
        goto fin;
    }
    self->PublicStatus.UsedCount = 0;

    *out_self = self;

    e=0;
fin:
    return  e;
}


/***********************************************************************
* Implement: R_OSPL_QUEUE_GetStatus
************************************************************************/
errnum_t  R_OSPL_QUEUE_GetStatus( r_ospl_queue_t *self,  const r_ospl_queue_status_t **out_Status )
{
    *out_Status = &self->PublicStatus;
    return  0;
}


/***********************************************************************
* Implement: R_OSPL_QUEUE_Allocate
************************************************************************/
errnum_t  R_OSPL_QUEUE_Allocate( r_ospl_queue_t *self,  void *out_Address,  uint32_t Timeout_msec )
{
    errnum_t  e;
    void     *address;

    address = osMailAlloc( self->MailQId, Timeout_msec );
    *(void **) out_Address = address;
    IF ( address == NULL  &&  Timeout_msec > 0 ) {
        if ( R_OSPL_THREAD_GetCurrentId() == NULL ) {
            e=E_NOT_THREAD;
        } else {
            e=E_TIME_OUT;
        }
        goto fin;
    }

    R_OSPL_DisableAllInterrupt();
    self->PublicStatus.UsedCount += 1;

    e=0;
    R_OSPL_EnableAllInterrupt();
fin:
    return  e;
}


/***********************************************************************
* Implement: R_OSPL_QUEUE_Put
************************************************************************/
errnum_t  R_OSPL_QUEUE_Put( r_ospl_queue_t *self,  void *Address )
{
    errnum_t    e;
    osStatus_t  status;

    status = osMailPut( self->MailQId, Address );
    IF ( status != osOK ) {
        e=E_OTHERS;
        goto fin;
    }

    e=0;
fin:
    return  e;
}


/***********************************************************************
* Implement: R_OSPL_QUEUE_Get
************************************************************************/
errnum_t  R_OSPL_QUEUE_Get( r_ospl_queue_t *self,  void *out_Address,  uint32_t Timeout_msec )
{
    errnum_t  e;
    osEvent   event;

    event = osMailGet( self->MailQId, Timeout_msec );

    if ( event.status != osOK ) {
        IF ( event.status != osEventMail ) {
            if ( event.status == osEventTimeout ) {
                e = E_TIME_OUT;
            } else if ( event.status == osErrorParameter ) {
                if ( R_OSPL_THREAD_GetCurrentId() == NULL ) {
                    e = E_NOT_THREAD;
                } else {
                    e = E_OTHERS;
                }
            } else {
                e = E_OTHERS;
            }
            goto fin;
        }
    }

    *(void **) out_Address = event.value.p;

    e=0;
fin:
    return  e;
}


/***********************************************************************
* Implement: R_OSPL_QUEUE_Free
************************************************************************/
errnum_t  R_OSPL_QUEUE_Free( r_ospl_queue_t *self,  void *Address )
{
    errnum_t    e;
    osStatus_t  status;

    status = osMailFree( self->MailQId, Address );
    IF ( status != osOK ) {
        e=E_OTHERS;
        goto fin;
    }

    R_OSPL_DisableAllInterrupt();
    self->PublicStatus.UsedCount -= 1;

    e=0;
    R_OSPL_EnableAllInterrupt();
fin:
    return  e;
}


