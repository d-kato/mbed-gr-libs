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
* @file  r_ospl_config.h
* @brief   Configuration of OSPL.
*
* $Module: OSPL $ $PublicVersion: 0.90 $ (=R_OSPL_VERSION)
* $Rev: 35 $
* $Date:: 2014-04-15 21:38:18 +0900#$
*/



/* This file is included from "Project_Config.h" */


#ifndef R_OSPL_CONFIG_H
#define R_OSPL_CONFIG_H


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "platform_config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/**
* @def  R_OSPL_VERSION
* @brief  Version number of OSPL
* @par Parameters
*    None
* @return  None.
*
* @par Description
*    The value can not be changed.
*    101 = version 1.01.
*    Hundreds place is version number of OSPL specification.
*    Tens place and one's place are minor version number in specified OS and board.
*/
#define  R_OSPL_VERSION  90


/**
* @def  R_OSPL_VERSION_STRING
* @brief  String of version number of OSPL.
* @par Parameters
*    None
* @return  None.
*
* @par Description
*    The value can not be changed.
*/
#define  R_OSPL_VERSION_STRING  "0.90"


/**
* @def  R_OSPL_IS_PREEMPTION
* @brief  Whether preemptive RTOS or not.
* @par Parameters
*    None
* @return  None.
*
* @par Description
*    The value can not be changed.
*    The value is 1 or 0.
*    This value is 0, if the environment was OS less.
*    It is necessary to configure to pseudo multithreading, if this value was 0.
*/
#define  R_OSPL_IS_PREEMPTION   BSP_CFG_RTOS_USED


/**
* @def  R_OSPL_NDEBUG
* @brief  Debug configuration or Release configuration.
* @par Parameters
*    None
* @return  None.
*
* @par Description
*    The value can be changed.
*    Define (=Release) or not define (=Debug).
*    This is same as "NDEBUG" of standard library.
*    The system can run with the debug configuration OSPL and the release
*    configuration application.
*    If the library (compiled binary) called the debug configuration OSPL,
*    compile the OSPL source with debug configuration.
*/
#ifndef  R_OSPL_NDEBUG
#define  R_OSPL_NDEBUG
#endif


/**
* @def  R_OSPL_ERROR_BREAK
* @brief  Whether it is supported to break, when error was raised.
* @par Parameters
*    None
* @return  None.
*
* @par Description
*    The value can be changed.
*    The value is 1 or 0.
*/
#ifndef  R_OSPL_ERROR_BREAK
#ifdef  R_OSPL_NDEBUG
#define  R_OSPL_ERROR_BREAK  0
#else
#define  R_OSPL_ERROR_BREAK  1
#endif
#endif


/**
* @def  R_OSPL_TLS_ERROR_CODE
* @brief  Whether it is supported that error code is stored in thread local storage
* @par Parameters
*    None
* @return  None.
*
* @par Description
*    The value can be changed.
*    The value is 1 or 0.
*    This value must be 1 by some application or some library with OSPL.
*/
#ifndef  R_OSPL_TLS_ERROR_CODE
#define  R_OSPL_TLS_ERROR_CODE  1
#endif


/**
* @def  R_OSPL_LIBRARY_MAKING
* @brief  Whether current project makes library.
* @par Parameters
*    None
* @return  None.
*
* @par Description
*    The value can be changed.
*    The value is 1 or 0.
*/
#ifndef  R_OSPL_LIBRARY_MAKING
#define  R_OSPL_LIBRARY_MAKING  0
#endif


/**
* @def  R_BOOL_IS_SIGNED
* @brief  Whether compiler defines that bool_t type is signed.
* @par Parameters
*    None
* @return  None.
*
* @par Description
*    The value can be changed.
*    The value is 1 or 0.
*/
#define  R_BOOL_IS_SIGNED  1


/**
* @def  R_OSPL_BIT_FIELD_ACCESS_MACRO
* @brief  Whether bit field access function is provided as macro or function.
* @par Parameters
*    None
* @return  None.
*
* @par Description
*    The value can be changed.
*    The value is 1 (=macro) or 0 (=function).
*/
#define  R_OSPL_BIT_FIELD_ACCESS_MACRO  1


/***********************************************************************
* Macros: R_OSPL_FTIMER_IS
*    Which channel of timer to use.
*
*    : R_OSPL_FTIMER_IS_OSTM0    - OSTM0
*    : R_OSPL_FTIMER_IS_OSTM1    - OSTM1
*    : R_OSPL_FTIMER_IS_MTU2_1_2 - MTU2 ch1 and ch2
*
* Description:
*    The value can be changed.
************************************************************************/
#define  R_OSPL_FTIMER_IS        R_OSPL_FTIMER_IS_MTU2_1_2
#define  R_OSPL_FTIMER_IS_OSTM0     0
#define  R_OSPL_FTIMER_IS_OSTM1     1
#define  R_OSPL_FTIMER_IS_MTU2_1_2  2

#if R_OSPL_FTIMER_IS == R_OSPL_FTIMER_IS_OSTM0
#error  OSTM0 is used by RTX
#endif


/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/

/***********************************************************************
* End of File:
************************************************************************/
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif /* R_OSPL_H */

