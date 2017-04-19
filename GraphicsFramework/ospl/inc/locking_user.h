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
* @file  locking_user.h
* @brief   Lock related FIT BSP. User defined.
*
* $Module: OSPL $ $PublicVersion: 0.90 $ (=R_OSPL_VERSION)
* $Rev: 35 $
* $Date:: 2014-04-15 21:38:18 +0900#$
*/

#ifndef LOCKING_USER_H
#define LOCKING_USER_H


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_typedefs.h"
#include  "locking_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Typedef definitions
******************************************************************************/

/**
* @struct  r_ospl_user_lock_t
* @brief  Example of user defined lock type
*/
typedef struct st_r_ospl_user_lock_t  r_ospl_user_lock_t;
struct st_r_ospl_user_lock_t {
    int  UserLockMember;
};


/******************************************************************************
Macro definitions
******************************************************************************/

/**
* @def  BSP_CFG_USER_LOCKING_TYPE
* @brief  C-lock (FIT BSP portable)
* @par Parameters
*    None
* @return  None.
*
* @par Description
*    The value can be changed.
*    This is enabled, when "BSP_CFG_USER_LOCKING_ENABLED == 1".
*/
#define  BSP_CFG_USER_LOCKING_TYPE  r_ospl_user_lock_t


/**
* @def  BSP_CFG_USER_LOCKING_SW_LOCK_FUNCTION
* @brief  Start C-lock for software module
* @par Parameters
*    None
* @return  None.
*
* @par Description
*    The value can be changed.
*    This is enabled, when "BSP_CFG_USER_LOCKING_ENABLED == 1".
*/
#define  BSP_CFG_USER_LOCKING_SW_LOCK_FUNCTION    R_OSPL_USER_LOCK_SoftwareLock


/**
* @def  BSP_CFG_USER_LOCKING_SW_UNLOCK_FUNCTION
* @brief  End C-lock for software module
* @par Parameters
*    None
* @return  None.
*
* @par Description
*    The value can be changed.
*    This is enabled, when "BSP_CFG_USER_LOCKING_ENABLED == 1".
*/
#define  BSP_CFG_USER_LOCKING_SW_UNLOCK_FUNCTION  R_OSPL_USER_LOCK_SoftwareUnlock


/**
* @def  BSP_CFG_USER_LOCKING_HW_LOCK_FUNCTION
* @brief  Start C-lock for hardware module
* @par Parameters
*    None
* @return  None.
*
* @par Description
*    The value can be changed.
*    This is enabled, when "BSP_CFG_USER_LOCKING_ENABLED == 1".
*/
#define  BSP_CFG_USER_LOCKING_HW_LOCK_FUNCTION    R_OSPL_USER_LOCK_HardwareLock


/**
* @def  BSP_CFG_USER_LOCKING_HW_UNLOCK_FUNCTION
* @brief  End C-lock for hardware module
* @par Parameters
*    None
* @return  None.
*
* @par Description
*    The value can be changed.
*    This is enabled, when "BSP_CFG_USER_LOCKING_ENABLED == 1".
*/
#define  BSP_CFG_USER_LOCKING_HW_UNLOCK_FUNCTION  R_OSPL_USER_LOCK_HardwareUnlock


/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/

/**
* @brief   Example of BSP_CFG_USER_LOCKING_SW_LOCK_FUNCTION
*
* @par Parameters
*    None
* @return  None.
*/
#if BSP_CFG_USER_LOCKING_ENABLED == 1
bool_t  R_OSPL_USER_LOCK_SoftwareLock( r_ospl_user_lock_t *LockObject );
#endif


/**
* @brief   Example of BSP_CFG_USER_LOCKING_SW_UNLOCK_FUNCTION
*
* @par Parameters
*    None
* @return  None.
*/
#if BSP_CFG_USER_LOCKING_ENABLED == 1
bool_t  R_OSPL_USER_LOCK_SoftwareUnlock( r_ospl_user_lock_t *LockObject );
#endif


/**
* @brief   Example of BSP_CFG_USER_LOCKING_HW_LOCK_FUNCTION
*
* @par Parameters
*    None
* @return  None.
*/
#if BSP_CFG_USER_LOCKING_ENABLED == 1
bool_t  R_OSPL_USER_LOCK_HardwareLock( mcu_lock_t  HardwareIndex );
#endif


/**
* @brief   Example of BSP_CFG_USER_LOCKING_HW_UNLOCK_FUNCTION
*
* @par Parameters
*    None
* @return  None.
*/
#if BSP_CFG_USER_LOCKING_ENABLED == 1
bool_t  R_OSPL_USER_LOCK_HardwareUnlock( mcu_lock_t  HardwareIndex );
#endif


/**
* @brief   For Test
*
* @par Parameters
*    None
* @return  None.
*/
#if BSP_CFG_USER_LOCKING_ENABLED == 1
r_ospl_user_lock_t  *R_OSPL_GetHardwareLockObjectForTest( mcu_lock_t  HardwareIndex );
#endif


/***********************************************************************
* End of File:
************************************************************************/
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif /* LOCKING_USER_H */

