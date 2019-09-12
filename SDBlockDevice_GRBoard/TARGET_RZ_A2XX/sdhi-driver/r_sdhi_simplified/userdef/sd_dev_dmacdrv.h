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
/*******************************************************************************
* File Name    : sd_dev_dmacdrv.h
* Version      : 1.20
* Description  : RZ/A2M SD Sample Program - DMAC Sample Program
******************************************************************************/
/*****************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 16.03.2018 1.00     First Release
*         : 14.12.2018 1.01     Changed the DMAC soft reset procedure.
*         : 29.05.2019 1.20     Correspond to internal coding rules
******************************************************************************/
#if(1) // mbed
#include "cmsis_os.h"
#include "irq_ctrl.h"
#else
#include "r_stb_lld_rza2m.h"
#include "r_intc_lld_rza2m.h"
#endif
#include "r_sd_cfg.h"

#ifndef SD_DEV_DMACDRV_H
#define SD_DEV_DMACDRV_H

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
        void                    (* p_func)(void);
#else
        e_r_drv_intc_intid_t    int_id;
        e_r_drv_intc_priority_t int_priority;
        void                    (* p_func)(uint32_t int_sense);
#endif
    } intc;
    struct
    {
        bool_t                  gpio_init;
        bool_t                  gpio_vol_init;
    } gpio;
#ifdef SD_CFG_HWINT
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
#endif /* #ifdef SD_CFG_HWINT */
} st_sdhi_info_dev_ch_t;

/******************************************************************************
Exported global variables
******************************************************************************/

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/
/* Function Name: sd_DMAC_PeriReqInit */
/**************************************************************************//**
 * @fn            int32_t sd_DMAC_PeriReqInit(int32_t sd_port, uint32_t buff, int32_t dir)
 * @brief         Initialize DMAC.
 * @warning       .
 * @param [in]    int32_t  sd_port : channel no (0 or 1).
 * @param [in]    uint32_t buff    : buffer addrees to transfer datas.
 * @param [in]    int32_t  dir     : direction to transfer.
 * @retval        success : SD_OK.
 * @retval        fail    : SD_ERR.
 *****************************************************************************/
int32_t sd_DMAC_PeriReqInit(int32_t sd_port, uint32_t buff, int32_t dir);

/* Function Name: sd_DMAC_Open */
/**************************************************************************//**
 * @fn            int32_t sd_DMAC_Open(int32_t sd_port, int32_t dir)
 * @brief         Start DMAC.
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1).
 * @param [in]    int32_t dir     : direction to transfer.
 * @retval        success : SD_OK.
 * @retval        fail    : SD_ERR.
 *****************************************************************************/
int32_t sd_DMAC_Open(int32_t sd_port, int32_t dir);

/* Function Name: sd_DMAC_Close */
/**************************************************************************//**
 * @fn            int32_t sd_DMAC_Close(int32_t sd_port)
 * @brief         Stop DMAC.
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1).
 * @retval        success : SD_OK.
 * @retval        fail    : SD_ERR.
 *****************************************************************************/
int32_t sd_DMAC_Close(int32_t sd_port);

/* Function Name: sd_DMAC_Reset */
/**************************************************************************//**
 * @fn            int32_t sd_DMAC_Reset(int32_t sd_port)
 * @brief         Soft resets of the SDHI module built-in DMAC.
 * @warning       .
 * @param [in]    int32_t  sd_port : channel no (0 or 1).
 * @retval        success : SD_OK.
 * @retval        fail    : SD_ERR.
 *****************************************************************************/
int32_t sd_DMAC_Reset(int32_t sd_port);

/* Function Name: sd_DMAC_Released */
/**************************************************************************//**
 * @fn            int32_t sd_DMAC_Released(int32_t sd_port)
 * @brief         Soft resets released of the SDHI module built-in DMAC.
 * @warning       .
 * @param [in]    int32_t  sd_port : channel no (0 or 1).
 * @retval        success : SD_OK.
 * @retval        fail    : SD_ERR.
 *****************************************************************************/
int32_t sd_DMAC_Released(int32_t sd_port);

/* Function Name: sddev_get_dev_ch_instance */
/**************************************************************************//**
 * @fn            st_sdhi_info_dev_ch_t *sddev_get_dev_ch_instance(int32_t sd_port)
 * @brief         Get SDHI driver target CPU interface information object.
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1).
 * @retval        success : pointer to SDHI driver target CPU interface information.
 * @retval        fail    : NULL.
 *****************************************************************************/
st_sdhi_info_dev_ch_t *sddev_get_dev_ch_instance(int32_t sd_port);

#endif  /* SD_DEV_DMACDRV_H */

/* End of File */
