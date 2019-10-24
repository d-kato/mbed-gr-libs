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
* Copyright (C) 2017 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**************************************************************************//**
* @file         ceu_userdef.c
* @version      0.06
* $Rev: 18 $
* $Date:: 2014-05-29 11:11:38 +0900#$
* @brief        RZ/A1L CEU driver user function
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "iodefine.h"
#include "iobitmask.h"
#include "cmsis.h"
#include "pinmap.h"
#include "r_ceu.h"
#include "r_ceu_user.h"
#include "r_mmu_lld.h"

/******************************************************************************
Macro definitions
******************************************************************************/
#define STP56_BIT               (0x40u)
#define STBRQ10_BIT             (0x01u)
#define STBAK10_BIT             (0x01u)

#define CEU_INTERRUPT_PRIORITY  (5u)

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/


/**************************************************************************//**
 * @brief       Physical address translation
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Get the System Physical address of the given virtual memory address.
 * @param[in]   vaddr               : virtual address
 * @retval      physical address
 *****************************************************************************/
uint32_t R_CEU_CPUVAddrToSysPAddr(uint32_t vaddr)
{
    e_mmu_err_t ret;
    uint32_t  paddress = 0;

    ret = R_MMU_VAtoPA(vaddr, &paddress);
    if( MMU_SUCCESS != ret)
    {
        paddress = 0;
    }
    return paddress;
}   /* End of function R_CEU_CPUVAddrToSysPAddr() */

/**************************************************************************//**
 * Function Name: R_CEU_OnInitialize
 * @brief       CEU User difinistion part Init
 * @param[in]   user_num          :
 * @retval      none
 *****************************************************************************/
void R_CEU_OnInitialize(const uint32_t user_num)
{
    uint32_t reg_data;
    volatile uint8_t dummy_read;

    /* Standby control register 5 (STBCR5)
        b6      -0------;  MSTP56 : 0 : CEU enable */
    reg_data    = (uint32_t)CPG.STBCR5.BYTE & (uint32_t)~STP56_BIT;
    CPG.STBCR5.BYTE  = (uint8_t)reg_data;
    /* In order to reflect the change, a dummy read should be done. */
    dummy_read = CPG.STBCR5.BYTE;
    (void)dummy_read;

    /* Standby Request Register 1 (STBREQ1)
        b0      -------0;  STBRQ10 : The standby request to CEU is invalid. */
    reg_data    = (uint32_t)CPG.STBREQ1.BYTE & (uint32_t)~STBRQ10_BIT;
    CPG.STBREQ1.BYTE = (uint8_t)reg_data;
    /* Standby Acknowledge Register 1 (STBACK1)
        b0      -------*;  STBAK10 : Standby acknowledgement from CEU. */
    while (((uint32_t)CPG.STBACK1.BYTE & (uint32_t)STBAK10_BIT) != 0u) {
        /* Wait for the STBAK10 to be cleared to 0. */
    }

    InterruptHandlerRegister(CEUI_IRQn, R_CEU_Isr);
    GIC_SetPriority(CEUI_IRQn, (uint8_t)CEU_INTERRUPT_PRIORITY);
    GIC_SetConfiguration(CEUI_IRQn, 1);
    GIC_EnableIRQ(CEUI_IRQn);
} /* End of function R_CEU_OnInitialize() */

/**************************************************************************//**
 * Function Name: R_CEU_OnFinalize
 * @brief       CEU User difinistion part final
 * @param[in]   user_num          :
 * @retval      none
 *****************************************************************************/
void R_CEU_OnFinalize(const uint32_t user_num)
{
    uint32_t reg_data;
    volatile uint8_t dummy_read;

    /* Standby Request Register 1 (STBREQ1)
        b0      -------1;  STBRQ10 : The standby request to CEU is valid. */
    reg_data    = (uint32_t)CPG.STBREQ1.BYTE | (uint32_t)STBRQ10_BIT;
    CPG.STBREQ1.BYTE = (uint8_t)reg_data;
    /* Standby Acknowledge Register 1 (STBACK1)
        b0      -------*;  STBAK10 : Standby acknowledgement from CEU. */
    while (((uint32_t)CPG.STBACK1.BYTE & (uint32_t)STBAK10_BIT) == 0u) {
        /* Wait for the STBAK10 to be set to 1. */
    }

    /* Standby control register 5 (STBCR5)
        b6      -1------;  MSTP56 : 1 : CEU disable */
    reg_data    = (uint32_t)CPG.STBCR5.BYTE | (uint32_t)STP56_BIT;
    CPG.STBCR5.BYTE  = (uint8_t)reg_data;

    /* In order to reflect the change, a dummy read should be done. */
    dummy_read = CPG.STBCR5.BYTE;
    (void)dummy_read;

} /* End of function R_CEU_OnFinalize() */

/* End of File */

