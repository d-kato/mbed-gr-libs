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
* @file  locking.c
* @brief   Lock related FIT BSP.
*
* $Module: OSPL $ $PublicVersion: 0.90 $ (=R_OSPL_VERSION)
* $Rev: 35 $
* $Date:: 2014-04-15 21:38:18 +0900#$
*/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_ospl.h"
#if R_OSPL_IS_PREEMPTION
#include  "r_ospl_RTX_private.h"
#endif
#if IS_RZ_A1_BSP_USED
#include  "dma_if.h"  /* R_DMA_Alloc */
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

/** gs_ospl_mutex */
#if R_OSPL_IS_PREEMPTION
static osMutexId_t  gs_ospl_mutex;  /* gs_OSPL_Mutex */
#endif


/** g_bsp_Locks */
BSP_CFG_USER_LOCKING_TYPE  g_bsp_Locks[ BSP_NUM_LOCKS ];


/***********************************************************************
* Implement: R_BSP_HardwareLock
************************************************************************/
bool_t  R_BSP_HardwareLock( mcu_lock_t const  HardwareIndex )
{
#if BSP_CFG_USER_LOCKING_ENABLED
    return  BSP_CFG_USER_LOCKING_HW_LOCK_FUNCTION( HardwareIndex );
#else
    bool_t  is_success;


#if IS_RZ_A1_BSP_USED
    if ( HardwareIndex >= BSP_LOCK_DMAC0  &&  HardwareIndex <= BSP_LOCK_DMAC15 ) {
        int_fast32_t  channel_of_DMAC = HardwareIndex - BSP_LOCK_DMAC0;

        channel_of_DMAC = R_DMA_Alloc( channel_of_DMAC, NULL );
        IF ( channel_of_DMAC == -1 ) {
            is_success = false;
            goto fin;
        }

        is_success = true;
        goto fin;
    }
#endif

    IF_D ( R_CUT_IF_ALWAYS_FALSE( HardwareIndex < 0u  ||)  HardwareIndex >= R_COUNT_OF( g_bsp_Locks ) ) {
        is_success = false;
        goto fin;
    }

    is_success = R_BSP_SoftwareLock( &g_bsp_Locks[ HardwareIndex ] );
    IF ( ! is_success ) {
        goto fin;
    }

    is_success = true;
fin:
    return  is_success;
#endif
}


/***********************************************************************
* Implement: R_OSPL_LockUnlockedChannel
************************************************************************/
#if ! BSP_CFG_USER_LOCKING_ENABLED
errnum_t  R_OSPL_LockUnlockedChannel( int_fast32_t *out_ChannelNum,
                                      mcu_lock_t  HardwareIndexMin,  mcu_lock_t  HardwareIndexMax )
{
    errnum_t    e;
    mcu_lock_t  hardware_index;
#if R_OSPL_IS_PREEMPTION
    bool_t    is_lock = false;
#endif

#if R_OSPL_IS_PREEMPTION
    if ( R_OSPL_THREAD_GetCurrentId() == NULL ) {  /* If interrrupt context */
        e = E_NOT_THREAD;
        goto fin;
    }
    e= R_OSPL_Start_T_Lock();
    if ( e != 0 ) {
        R_OSPL_RaiseUnrecoverable( e );
        goto fin;
    }
    is_lock = true;  /* T-Lock to "self" */
#endif


#if IS_RZ_A1_BSP_USED
    if ( HardwareIndexMin == BSP_LOCK_DMAC0 ) {
        int_fast32_t  channel_of_DMAC;

        channel_of_DMAC = R_DMA_Alloc( DMA_ALLOC_CH, NULL );
        IF ( channel_of_DMAC == -1 ) {
            e = E_FEW_ARRAY;
            goto fin;
        }

        *out_ChannelNum = channel_of_DMAC;
        e = 0;
        goto fin;
    }
#endif


    for ( hardware_index = HardwareIndexMin;  hardware_index <= HardwareIndexMax;
            hardware_index += 1 ) {
        r_ospl_c_lock_t  *lock = &g_bsp_Locks[ hardware_index ];

        if ( ! lock->IsLocked ) {
            lock->IsLocked = true;
            break;
        }
    }
    IF ( hardware_index > HardwareIndexMax ) {
        e=E_FEW_ARRAY;
        goto fin;
    }


    *out_ChannelNum = hardware_index - HardwareIndexMin;


    e=0;
fin:
#if R_OSPL_IS_PREEMPTION
    if ( IS( is_lock ) ) {
        R_OSPL_End_T_Lock();
    }
#endif

    return  e;
}
#endif


/***********************************************************************
* Implement: R_BSP_HardwareUnlock
************************************************************************/
bool_t  R_BSP_HardwareUnlock( mcu_lock_t const  HardwareIndex )
{
#if BSP_CFG_USER_LOCKING_ENABLED
    return  BSP_CFG_USER_LOCKING_HW_UNLOCK_FUNCTION( HardwareIndex );
#else
    bool_t  is_success;


#if IS_RZ_A1_BSP_USED
    if ( HardwareIndex >= BSP_LOCK_DMAC0  &&  HardwareIndex <= BSP_LOCK_DMAC15 ) {
        int_fast32_t  channel_of_DMAC = HardwareIndex - BSP_LOCK_DMAC0;
        int_fast32_t  err2;

        err2 = R_DMA_Free( channel_of_DMAC, NULL );
        IF ( err2 != ESUCCESS ) {
            is_success = false;
            goto fin;
        }

        is_success = true;
        goto fin;
    }
#endif


    IF_D ( R_CUT_IF_ALWAYS_FALSE( HardwareIndex < 0u  ||)  HardwareIndex >= R_COUNT_OF( g_bsp_Locks ) ) {
        is_success = false;
        R_OSPL_RaiseUnrecoverable( E_FEW_ARRAY );
        goto fin;
    }

    is_success = R_BSP_SoftwareUnlock( &g_bsp_Locks[ HardwareIndex ] );
    IF ( ! is_success ) {
        goto fin;
    }

    is_success = true;
fin:
    return  is_success;
#endif
}


/***********************************************************************
* Implement: R_BSP_SoftwareLock
************************************************************************/
bool_t  R_BSP_SoftwareLock( BSP_CFG_USER_LOCKING_TYPE *const  LockObject )
{
#if BSP_CFG_USER_LOCKING_ENABLED
    return  BSP_CFG_USER_LOCKING_SW_LOCK_FUNCTION( LockObject );
#else
    errnum_t  e;
#if R_OSPL_IS_PREEMPTION
    bool_t    is_lock = false;
#endif

#if R_OSPL_IS_PREEMPTION
    if ( R_OSPL_THREAD_GetCurrentId() != NULL ) {  /* If not interrrupt context */
        e= R_OSPL_Start_T_Lock();
        IF(e!=0) {
            goto fin;
        }
        is_lock = true;  /* T-Lock to "self" */
    }
#endif


    e= R_OSPL_C_LOCK_Lock( LockObject );
    if ( e == E_ACCESS_DENIED ) {
        R_OSPL_CLEAR_ERROR();
        goto fin;
    }
    IF(e) {
        goto fin;
    }

    e=0;
fin:
#if R_OSPL_IS_PREEMPTION
    if ( IS( is_lock ) ) {
        R_OSPL_End_T_Lock();
    }
#endif

    return  (bool_t)( e == 0 );
#endif
}


/***********************************************************************
* Implement: R_BSP_SoftwareUnlock
************************************************************************/
bool_t  R_BSP_SoftwareUnlock( BSP_CFG_USER_LOCKING_TYPE *const  LockObject )
{
#if BSP_CFG_USER_LOCKING_ENABLED
    return  BSP_CFG_USER_LOCKING_SW_UNLOCK_FUNCTION( LockObject );
#else
    errnum_t  e;
#if R_OSPL_IS_PREEMPTION
    bool_t    is_lock = false;
#endif

#if R_OSPL_IS_PREEMPTION
    if ( R_OSPL_THREAD_GetCurrentId() != NULL ) {  /* If not interrrupt context */
        e= R_OSPL_Start_T_Lock();
        IF ( e != 0 ) {
            R_OSPL_RaiseUnrecoverable( e );
            goto fin;
        }
        is_lock = true;  /* T-Lock to "self" */
    }
#endif


    e= R_OSPL_C_LOCK_Unlock( LockObject );
    if ( e == E_ACCESS_DENIED ) {
        R_OSPL_CLEAR_ERROR();
        goto fin;
    }
    IF(e) {
        goto fin;
    }

    e=0;
fin:
#if R_OSPL_IS_PREEMPTION
    if ( IS( is_lock ) ) {
        R_OSPL_End_T_Lock();
    }
#endif

    return  (bool_t)( e == 0 );
#endif
}


/**
* @brief   The function callbacked from OSPL internal, when T-Lock started
*
* @par Parameters
*    None
* @return  Error Code. 0=No Error.
*/
#if R_OSPL_IS_PREEMPTION
errnum_t  R_OSPL_Start_T_Lock(void)
{
    errnum_t    e;
    osStatus_t  es;

    osMutexDef( gs_ospl_mutex );

    if ( gs_ospl_mutex == NULL ) {
        gs_ospl_mutex = osMutexNew( osMutex( gs_ospl_mutex ) );
        if ( gs_ospl_mutex == NULL ) {
            e=E_OTHERS;
            goto fin;
        }
    }

    es= osMutexAcquire( gs_ospl_mutex, osWaitForever );
    if ( es == osErrorISR ) {
        es = osOK;
    }
    if ( es != osOK ) {
        e=E_OTHERS;
        goto fin;
    }

    e=0;
fin:
    return  e;
}
#endif


/**
* @brief   The function callbacked from OSPL internal, when T-Lock ended
*
* @par Parameters
*    None
* @return  None
*/
#if R_OSPL_IS_PREEMPTION
void  R_OSPL_End_T_Lock(void)
{
    if ( gs_ospl_mutex != NULL ) {
        osStatus_t rs;

        rs= osMutexRelease( gs_ospl_mutex );
        if ( rs == osErrorISR ) {
            rs = osOK;
        }
        ASSERT_R( rs == osOK,  R_OSPL_RaiseUnrecoverable( E_OTHERS ) );
    }
}
#endif


