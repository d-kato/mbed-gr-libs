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
* @file  locking_typedef.h
* @brief   Lock related FIT BSP. Data types.
*
* $Module: OSPL $ $PublicVersion: 0.90 $ (=R_OSPL_VERSION)
* $Rev: 38 $
* $Date:: 2014-05-15 16:39:09 +0900#$
*/

#ifndef LOCKING_TYPEDEF_H
#define LOCKING_TYPEDEF_H


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
* @struct  r_ospl_c_lock_t
* @brief  C-lock
*/
typedef struct st_r_ospl_c_lock_t  r_ospl_c_lock_t;
struct st_r_ospl_c_lock_t {

    /** IsLocked */
    volatile bool_t  IsLocked;
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
*/
#if ! BSP_CFG_USER_LOCKING_ENABLED
#define  BSP_CFG_USER_LOCKING_TYPE  r_ospl_c_lock_t
#endif


/**
* @enum   mcu_lock_t
* @brief  Hardware channel number for locking
*
*    : BSP_LOCK_ADC
*    @par :
*/
typedef enum {
    BSP_LOCK_ADC,
    BSP_LOCK_BSC,
    BSP_LOCK_CEU,
    BSP_LOCK_CPG,
    BSP_LOCK_DISC0,
    BSP_LOCK_DISC1,
    BSP_LOCK_DMAC0,
    BSP_LOCK_DMAC1,
    BSP_LOCK_DMAC2,
    BSP_LOCK_DMAC3,
    BSP_LOCK_DMAC4,
    BSP_LOCK_DMAC5,
    BSP_LOCK_DMAC6,
    BSP_LOCK_DMAC7,
    BSP_LOCK_DMAC8,
    BSP_LOCK_DMAC9,
    BSP_LOCK_DMAC10,
    BSP_LOCK_DMAC11,
    BSP_LOCK_DMAC12,
    BSP_LOCK_DMAC13,
    BSP_LOCK_DMAC14,
    BSP_LOCK_DMAC15,
    BSP_LOCK_DVDEC0,
    BSP_LOCK_DVDEC1,
    BSP_LOCK_ETHER,
    BSP_LOCK_FLCTL,
    BSP_LOCK_GPIO,
    BSP_LOCK_IEB,
    BSP_LOCK_INB,
    BSP_LOCK_INTC,
    BSP_LOCK_IRDA,
    BSP_LOCK_JCU,
    BSP_LOCK_I2C,
    BSP_LOCK_LIN0,
    BSP_LOCK_LIN1,
    BSP_LOCK_LVDS,
    BSP_LOCK_MLB,
    BSP_LOCK_MMC,
    BSP_LOCK_MTU2,
    BSP_LOCK_OSTM0,
    BSP_LOCK_OSTM1,
    BSP_LOCK_PFV0,
    BSP_LOCK_PFV1,
    BSP_LOCK_PWM,
    BSP_LOCK_RIIC0,
    BSP_LOCK_RIIC1,
    BSP_LOCK_RIIC2,
    BSP_LOCK_RIIC3,
    BSP_LOCK_ROMDEC,
    BSP_LOCK_RSCAN0,
    BSP_LOCK_RSPI0,
    BSP_LOCK_RSPI1,
    BSP_LOCK_RSPI2,
    BSP_LOCK_RSPI3,
    BSP_LOCK_RSPI4,
    BSP_LOCK_RTC,
    BSP_LOCK_SCIF0,
    BSP_LOCK_SCIF1,
    BSP_LOCK_SCIF2,
    BSP_LOCK_SCIF3,
    BSP_LOCK_SCIF4,
    BSP_LOCK_SCIF5,
    BSP_LOCK_SCIF6,
    BSP_LOCK_SCIF7,
    BSP_LOCK_SCIM0,
    BSP_LOCK_SCIM1,
    BSP_LOCK_SCUX,
    BSP_LOCK_SDG0,
    BSP_LOCK_SDG1,
    BSP_LOCK_SDG2,
    BSP_LOCK_SDG3,
    BSP_LOCK_SPDIF,
    BSP_LOCK_SPIBSC0,
    BSP_LOCK_SPIBSC1,
    BSP_LOCK_SSIF0,
    BSP_LOCK_SSIF1,
    BSP_LOCK_SSIF2,
    BSP_LOCK_SSIF3,
    BSP_LOCK_SSIF4,
    BSP_LOCK_SSIF5,
    BSP_LOCK_USB200,
    BSP_LOCK_USB201,
    BSP_LOCK_VDC50,
    BSP_LOCK_VDC51,
    BSP_LOCK_WDT,
    BSP_NUM_LOCKS /* This entry is not a valid lock. It is used for sizing
		g_bsp_Locks[] array below. Do not touch! */
} mcu_lock_t;


/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
/* In "mcu_interrupts.h" */


/***********************************************************************
* End of File:
************************************************************************/
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif /* LOCKING_TYPEDEF_H */

