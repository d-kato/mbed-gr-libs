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
/*******************************************************************************
* File Name : r_mmu_lld.c
* $Rev: 175 $
* $Date:: 2017-12-22 19:13:06 +0900#$
* Description :
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "cmsis_compiler.h"
#include "r_typedefs.h"
#include "r_mmu_lld.h"

/******************************************************************************
Private global variables and functions
******************************************************************************/

/******************************************************************************
* Function Name: R_MMU_VAtoPA
* Description  : Convert virtual address to physical address
* Arguments    : uint32_t   vaddress  ; I : virtual address to be convert
*              : uint32_t * paddress  ; O : physical address
* Return Value : MMU_SUCCESS         : successful
*                MMU_ERR_TRANSLATION : overflow of virtual or physical area
******************************************************************************/
e_mmu_err_t R_MMU_VAtoPA( uint32_t vaddress, uint32_t * paddress )
{
#if(1) /* mbed */
    /* Since the virtual address is not used in the Mbed, the process is simplified. */
    *paddress = vaddress;
    return MMU_SUCCESS;
#else
    uint32_t *ttb = (uint32_t *)(__get_TTBR0() & 0xFFFFC000);
    uint32_t *ttb_l2;
    e_mmu_err_t err = MMU_ERR_TRANSLATION;

    if (paddress != NULL) {
        ttb += (vaddress >> 20);
        if ((*ttb & (0x40000 | 0x3)) == 2) {
            *paddress = (*ttb & 0xfff00000) | (vaddress & 0xfffff);
            err = MMU_SUCCESS;
        } else if ((*ttb & 0x3) == 1) {
            ttb_l2 = (uint32_t *)(*ttb & 0xFFFFFC00);
            if ((*ttb_l2 & 0x3) == 0) {
                /* do nothing */
            } else if ((*ttb_l2 & 0x3) == 1) {
                /* 64k page entry */
                ttb_l2 += ((vaddress & 0x000ff000) >> 12);
                *paddress = (*ttb_l2 & 0xffff0000) | (vaddress & 0xffff);
                err = MMU_SUCCESS;
            } else {
                /* 4k page entry */
                ttb_l2 += ((vaddress & 0x000ff000) >> 12);
                *paddress = (*ttb_l2 & 0xfffff000) | (vaddress & 0xfff);
                err = MMU_SUCCESS;
            }
        } else {
            /* do nothing */
        }
    }

    return err;
#endif
}

/* End of File */
