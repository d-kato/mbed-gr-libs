/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**************************************************************************
* File Name : r_mipi_user.h
* Version : 1.11
* Description : RZ/A2M MIPI and VIN driver user-defined header
**************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_typedefs.h"
#include    "iodefine.h"

#ifndef R_MIPI_USER_H
#define R_MIPI_USER_H

#ifdef  __cplusplus
extern  "C"
{
#endif  /* __cplusplus */

/******************************************************************************
Macro definitions
******************************************************************************/
/* Version Number of API */
#define MIPI_RZA2_VERSION_MAJOR (1)
#define MIPI_RZA2_VERSION_MINOR (11)

/* Interrupt priority */
#define MIPI_INTERRUPT_PRIORITY  (28u)
#define VIN_INTERRUPT_PRIORITY   (28u)

/* For wait processing */
#define MIPI_1US_WAIT        (528u)    /* CPU Clock = 528MHz, 528 clock is needed to wait 1us */

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/

/**************************************************************************//**
 * @fn            R_MIPI_CPUVAddrToSysPAddr
 * @brief       Physical address translation
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Get the System Physical address of the given virtual memory address.
 * @param[in]   vaddr               : virtual address
 * @retval      physical address
 *****************************************************************************/
uint32_t R_MIPI_CPUVAddrToSysPAddr(uint32_t vaddr);

/**************************************************************************//**
 * @fn          R_MIPI_OnInitialize
 * @brief       MIPI and VIN User definition part of initialize
 * @param[in]   user_num          :user specify argument
 * @retval      none
 *****************************************************************************/
void R_MIPI_OnInitialize (const uint32_t user_num);

/**************************************************************************//**
 * @fn          R_MIPI_OnFinalize
 * @brief       MIPI and VIN  User definition part of finalize
 * @param[in]   user_num          :user specify argument
 * @retval      none
******************************************************************************/
void R_MIPI_OnFinalize( const uint32_t user_num );

#ifdef  __cplusplus
}
#endif  /* __cplusplus */

#endif  /* R_MIPI_USER_H */

