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
* @file  locking.h
* @brief   Lock related FIT BSP. Functions.
*
* $Module: OSPL $ $PublicVersion: 0.90 $ (=R_OSPL_VERSION)
* $Rev: 35 $
* $Date:: 2014-04-15 21:38:18 +0900#$
*/

#ifndef LOCKING_H
#define LOCKING_H


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "Project_Config.h"
#if BSP_CFG_USER_LOCKING_ENABLED
#include "locking_user.h"
#endif
#include "r_typedefs.h"
#include "locking_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Typedef definitions
******************************************************************************/
/* In "locking_typedef.h" */

/******************************************************************************
Macro definitions
******************************************************************************/
/* In "locking_typedef.h" */

/******************************************************************************
Variable Externs
******************************************************************************/
/* In "locking_typedef.h" */

/******************************************************************************
Functions Prototypes
******************************************************************************/


/**
* @brief   Attempts to reserve a hardware peripheral lock.
*
* @param   HardwareIndex Index of lock to acquire from the hardware lock array.
* @return  true = Successful, false = Failure
*/
bool_t  R_BSP_HardwareLock( mcu_lock_t const  HardwareIndex );


/**
* @brief   Releases a hardware peripheral lock.
*
* @param   HardwareIndex Index of lock to release from the hardware lock array.
* @return  true = Successful, false = Failure
*/
bool_t  R_BSP_HardwareUnlock( mcu_lock_t const  HardwareIndex );


/**
* @brief   Attempts to reserve a lock.
*
* @param   LockObject Pointer to lock structure with lock to try and acquire.
* @return  true = Successful, false = Failure
*/
bool_t  R_BSP_SoftwareLock( BSP_CFG_USER_LOCKING_TYPE *const  LockObject );


/**
* @brief   Releases a lock.
*
* @param   LockObject Pointer to lock structure with lock to release.
* @return  true = Successful, false = Failure
*/
bool_t  R_BSP_SoftwareUnlock( BSP_CFG_USER_LOCKING_TYPE *const  LockObject );


/***********************************************************************
* End of File:
************************************************************************/
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif /* LOCKING_H */

