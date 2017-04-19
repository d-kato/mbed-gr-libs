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
* http://www.renesas.com/disclaimer*
* Copyright (C) 2015 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

#include <stdio.h>
#include "dma_if.h"


static bool_t dma_init_end = false;
static dma_drv_init_t dma_init_param;
void userdef_dma_error_callback(union sigval signo);
static AIOCB dma_err_aio;

int32_t R_BSP_CMN_Init(void) {
    int32_t retval = ESUCCESS;

    if (dma_init_end == false) {
        dma_init_end = true;
        /***********************************************************************/
        /* Initialise DMA */
        /***********************************************************************/

        dma_init_param.channel[DMA_CH_0] = true;
        dma_init_param.channel[DMA_CH_1] = true;
        dma_init_param.channel[DMA_CH_2] = true;
        dma_init_param.channel[DMA_CH_3] = true;
        dma_init_param.channel[DMA_CH_4] = true;
        dma_init_param.channel[DMA_CH_5] = true;
        dma_init_param.channel[DMA_CH_6] = true;
        dma_init_param.channel[DMA_CH_7] = true;
        dma_init_param.channel[DMA_CH_8] = true;
        dma_init_param.channel[DMA_CH_9] = true;
        dma_init_param.channel[DMA_CH_10] = true;
        dma_init_param.channel[DMA_CH_11] = true;
        dma_init_param.channel[DMA_CH_12] = true;
        dma_init_param.channel[DMA_CH_13] = true;
        dma_init_param.channel[DMA_CH_14] = true;
        dma_init_param.channel[DMA_CH_15] = true;

        dma_err_aio.aio_sigevent.sigev_notify = SIGEV_THREAD;
        dma_err_aio.aio_sigevent.sigev_notify_function = &userdef_dma_error_callback;
        dma_init_param.p_aio = &dma_err_aio;

        retval = R_DMA_Init(&dma_init_param, NULL);
        if (retval != ESUCCESS) {
            dma_init_end = false;
        }
    }

    return retval;
}

/**************************************************************************//**
* Function Name: userdef_dma_error_callback
* @brief         DMA driver : error callback function
*
*                Description:<br>
*                DMA error interrupt callback function
* @param[in]     signo.sival_int : bitmap of error channel(bit0=ch0)
* @retval        none
******************************************************************************/
void userdef_dma_error_callback(union sigval signo) {
    printf("DMAERR MAP:0x%02x\n", signo.sival_int);
}

