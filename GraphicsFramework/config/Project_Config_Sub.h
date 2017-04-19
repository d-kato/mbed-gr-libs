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
/******************************************************************************
* $FileName: Project_Config.h $
* $Rev: 35 $
* $Date:: 2014-04-15 21:38:18 +0900#$
* Description  : This file is for drivers and applications
******************************************************************************/

#ifndef PROJECT_CONFIG_SUB_H
#define PROJECT_CONFIG_SUB_H


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

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
* @def  IS_RZ_A1_BSP_USED
* @brief  0 or 1
* @par Parameters
*    None
* @return  None.
*/
#define  IS_RZ_A1_BSP_USED  1


/**
* @def  IS_CMSIS_USED
* @brief  0 or 1
* @par Parameters
*    None
* @return  None.
*/
#define  IS_CMSIS_USED  1


/**
* @def  IS_RTX_USED
* @brief  0 or 1. Not CMSIS API.
* @par Parameters
*    None
* @return  None.
*/
#define  IS_RTX_USED  1


/**
* @def  IS_MBED_USED
* @brief  0 or 1
* @par Parameters
*    None
* @return  None.
*/
#define  IS_MBED_USED  1


/**
* @def  IS_VDC_SAMPLE_VRAM_USED
* @brief  0 or 1
* @par Parameters
*    None
* @return  None.
*/
#define  IS_VDC_SAMPLE_VRAM_USED  0


/**
* @def  R_ARM
* @brief  ARM core = 1, otherwise = not define
* @par Parameters
*    None
* @return  None.
*/
#define  R_ARM  1


/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/

#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif /* PROJECT_CONFIG_SUB_H */

