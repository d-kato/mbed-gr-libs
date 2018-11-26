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
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**************************************************************************//**
* @file         spdif_ver.c
* $Rev: $
* $Date::                           $
* @brief        SPDIF Driver get verion function
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "cmsis_os.h"
#include "spdif_if.h"

/******************************************************************************
Macro definitions
******************************************************************************/

/* Ex. V0.01 -> MAJOR=0, MINOR=01 */
#define SPDIF_DRV_VER_MAJOR  (0u)
#define SPDIF_DRV_VER_MINOR  (1u)

#define SPDIF_DRV_VER_MASK   (0xFFu)
#define SPDIF_DRV_VER_SHIFT  (8u)

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SPDIF_GetVersion
* @brief         Get SPDIF driver version.
*
*                Description:<br>
*
* @param         none
* @retval        driver version -
*                    upper 8bit=MAJOR
*                    lower 8bit=MINOR
******************************************************************************/
uint16_t R_SPDIF_GetVersion(void)
{
    const uint16_t version =
        ((SPDIF_DRV_VER_MAJOR & SPDIF_DRV_VER_MASK) << SPDIF_DRV_VER_SHIFT)
        | (SPDIF_DRV_VER_MINOR & SPDIF_DRV_VER_MASK);

    return version;
}
/******************************************************************************
End of function R_SPDIF_GetVersion
******************************************************************************/
