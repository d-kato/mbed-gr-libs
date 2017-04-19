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
* Copyright (C) 2013 - 2015 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**
* @file  clib_drivers_typedef.h
* @brief   $Module: CLibCommon $ $PublicVersion: 0.90 $ (=CLIB_VERSION)
* $Rev: 30 $
* $Date:: 2014-02-13 21:21:47 +0900#$
* - Description: Common code for drivers and more.
*/

#ifndef  CLIB_DRIVERS_TYPEDEF_H
#define  CLIB_DRIVERS_TYPEDEF_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "Project_Config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Typedef definitions
******************************************************************************/

/**
* @typedef  bool32_t
* @brief  Boolean type in 32 bit variable. This is not C99.
*/
typedef  unsigned int    bool32_t;


/**
* @typedef  bool16_t
* @brief  Boolean type in 16 bit variable. This is not C99.
*/
typedef  unsigned short  bool16_t;


/**
* @typedef  bool8_t
* @brief  Boolean type in 8 bit variable. This is not C99.
*/
typedef  unsigned char   bool8_t;


/**
* @typedef  IntBoxType
* @brief  IntBoxType
*/
typedef struct st_IntBoxType  IntBoxType;
struct st_IntBoxType {
    int  MinX;
    int  MinY;
    int  Width;
    int  Height;
};


/******************************************************************************
Macro definitions
******************************************************************************/

/**
* @def  DUMMY_INITIAL_VALUE
* @brief  Avoids not right warning of not initialized.
*
* @par Description
* - 0xDEDEDEDE means "not initialized".
* - Disable VC++ warning C4701 : local variable may be used without having
*   been initialized
* - See @ref DUMMY_INITIAL_VALUE_8BIT, @ref DUMMY_INITIAL_VALUE_16BIT
*/
#ifndef  R_OSPL_NDEBUG
#define  DUMMY_INITIAL_VALUE  0xDEDEDEDE
#else
enum { DUMMY_INITIAL_VALUE = 0 };
#endif


/**
* @def  DUMMY_INITIAL_VALUE_8BIT
* @brief  <DUMMY_INITIAL_VALUE> for 8 bit integer.
*/
#ifndef  R_OSPL_NDEBUG
enum { DUMMY_INITIAL_VALUE_8BIT  = 0xDE };
#else
enum { DUMMY_INITIAL_VALUE_8BIT  = 0 };
#endif


/**
* @def  DUMMY_INITIAL_VALUE_16BIT
* @brief  <DUMMY_INITIAL_VALUE> for 16 bit integer.
*/
#ifndef  R_OSPL_NDEBUG
enum { DUMMY_INITIAL_VALUE_16BIT = 0xDEDE };
#else
enum { DUMMY_INITIAL_VALUE_16BIT = 0 };
#endif


/**
* @def  U8_255
* @brief  Magic number of generally well known used.
*    unsigned 8bit max value = UINT8_MAX
*/
#define  U8_255   255


/**
* @def  R_CEIL_2U
* @brief  Fast ceil operation. Macro version. See <R_Ceil_N>.
* @par Parameters
*    None
* @return  None.
*/
#define  R_CEIL_2U( value )   (((value)+1)&~1)


/**
* @def  R_CEIL_4U
* @brief  Fast ceil operation. Macro version. See <R_Ceil_N>.
* @par Parameters
*    None
* @return  None.
*/
#define  R_CEIL_4U( value )   (((value)+3)&~3)


/**
* @def  R_CEIL_8U
* @brief  Fast ceil operation. Macro version. See <R_Ceil_N>.
* @par Parameters
*    None
* @return  None.
*/
#define  R_CEIL_8U( value )   (((value)+7)&~7)


/**
* @def  R_CEIL_16U
* @brief  Fast ceil operation. Macro version. See <R_Ceil_N>.
* @par Parameters
*    None
* @return  None.
*/
#define  R_CEIL_16U( value )  (((value)+15)&~15)


/**
* @def  R_CEIL_32U
* @brief  Fast ceil operation. Macro version. See <R_Ceil_N>.
* @par Parameters
*    None
* @return  None.
*/
#define  R_CEIL_32U( value )  (((value)+31)&~31)


/**
* @def  R_CEIL_64U
* @brief  Fast ceil operation. Macro version. See <R_Ceil_N>.
* @par Parameters
*    None
* @return  None.
*/
#define  R_CEIL_64U( value )  (((value)+63)&~63)


/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/

#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif  /* CLIB_DRIVERS_TYPEDEF_H */

