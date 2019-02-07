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
* http://www.renesas.com/disclaimer *
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.    
*******************************************************************************/
/*******************************************************************************
* File Name    : sd_dev_dmacdrv.h
* Version      : 1.00
* Description  : RZ/A2M SD Sample Program - DMAC Sample Program
******************************************************************************/
/*****************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 16.03.2018 1.00     First Release
*         : 14.12.2018 1.01     Changed the DMAC soft reset procedure.
******************************************************************************/
#ifndef _SD_DEV_DMACDRV_H_
#define _SD_DEV_DMACDRV_H_

#if(1) // mbed
#include "cmsis.h"
#include "cmsis_os.h"
#include "irq_ctrl.h"
#else
#include "r_stb_lld_rza2m.h"
#include "r_intc_lld_rza2m.h"
#endif
#include "r_sd_cfg.h"
/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef struct
{
    struct
    {
        bool_t                  stb_pon_init;
#if(1) // mbed
#else
        e_stb_module_t          stb_ch;
#endif
    } stb;
    struct
    {
#if(1) // mbed
        IRQn_ID_t               int_id;
        uint32_t                int_priority;
        void                    (* func)(void);
#else
        e_r_drv_intc_intid_t    int_id;
        e_r_drv_intc_priority_t int_priority;
        void                    (* func)(uint32_t int_sense);
#endif
    } intc;
    struct
    {
        bool_t                  gpio_init;
    } gpio;
#ifdef SDCFG_HWINT
    struct
    {
#if(1) // mbed
        osSemaphoreId_t         sem_sync;
        osSemaphoreId_t         sem_dma;
#else
        uint32_t                sem_sync;
        uint32_t                sem_dma;
#endif
    } semaphore;
#endif /* #ifdef SDCFG_HWINT */
} sdhi_info_dev_ch_t;

/******************************************************************************
Exported global variables
******************************************************************************/

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/
int32_t sd_DMAC_PeriReqInit(int32_t sd_port, uint32_t buff, int32_t dir);
int32_t sd_DMAC_Open(int32_t sd_port, int32_t dir);
int32_t sd_DMAC_Close(int32_t sd_port);
int32_t sd_DMAC_Reset(int32_t sd_port);
int32_t sd_DMAC_Released(int32_t sd_port);

#endif  /* _SD_DEV_DMACDRV_H_ */

/* End of File */
