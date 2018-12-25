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

/*******************************************************************************
* File Name   : spdif_dma.c
* $Rev: $
* $Date::                           $
* Description : SPDIF driver DMA functions
******************************************************************************/

/*******************************************************************************
Includes <System Includes>, "Project Includes"
*******************************************************************************/
#include "spdif.h"
#include "iodefine.h"
#include "iobitmask.h"
#include "dma_if.h"
#include "cmsis.h"

/*******************************************************************************
Macro definitions
*******************************************************************************/
#define SPDIF_DUMMY_DMA_TRN_SIZE (192u * 2u * 4u)
#define SPDIF_DUMMY_DMA_BUF_SIZE (SPDIF_DUMMY_DMA_TRN_SIZE + sizeof(uint32_t) - 1)

#define SPDIF_STAT_READ_ONLY (0xFFFFC03FuL)

/*******************************************************************************
Typedef definitions
*******************************************************************************/

/*******************************************************************************
Exported global variables (to be accessed by other files)
*******************************************************************************/

/*******************************************************************************
Private global variables and functions
*******************************************************************************/

static void SPDIF_DMA_TxCallback(union sigval param);
static void SPDIF_DMA_RxCallback(union sigval param);

static AIOCB gb_spdif_dma_tx_end_aiocb;
static AIOCB gb_spdif_dma_rx_end_aiocb;

static dma_trans_data_t gb_spdif_txdma_dummy_trparam;
static dma_trans_data_t gb_spdif_rxdma_dummy_trparam;

static uint32_t spdif_tx_dummy_buf[SPDIF_DUMMY_DMA_BUF_SIZE / sizeof(uint32_t)];
static uint32_t spdif_rx_dummy_buf[SPDIF_DUMMY_DMA_BUF_SIZE / sizeof(uint32_t)];

/******************************************************************************
* Function Name: SPDIF_InitDMA
* @brief         Allocate and Setup DMA_CH for specified SPDIF channel.
*
*                Description:<br>
*
* @param[in,out] p_info_ch  :channel object.
* @retval        ESUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
int_t SPDIF_InitDMA(spdif_info_ch_t* const p_info_ch)
{
    int_t ercd = ESUCCESS;
    int_t dma_ret;
    int32_t dma_ercd;
    dma_ch_setup_t  dma_ch_setup;

    if (NULL == p_info_ch)
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        /* allocate DMA Channel for write(if necessary) */
        if (ESUCCESS == ercd)
        {
            if (O_RDONLY_RBSP == p_info_ch->openflag)
            {
                p_info_ch->dma_tx_ch = -1;
            }
            else
            {
                dma_ret = R_DMA_Alloc(DMA_ALLOC_CH, &dma_ercd);

                if (EERROR == dma_ret)
                {
                    p_info_ch->dma_tx_ch = -1;
                    ercd = ENOMEM_RBSP;
                }
                else
                {
                    p_info_ch->dma_tx_ch = dma_ret;
                    ercd = ESUCCESS;
                }
            }
        }

        /* allocate DMA Channel for read(if necessary) */
        if (ESUCCESS == ercd)
        {
            if (O_WRONLY_RBSP == p_info_ch->openflag)
            {
                p_info_ch->dma_rx_ch = -1;
            }
            else
            {
                dma_ret = R_DMA_Alloc(DMA_ALLOC_CH, &dma_ercd);

                if (EERROR == dma_ret)
                {
                    p_info_ch->dma_rx_ch = -1;
                    ercd = ENOMEM_RBSP;
                }
                else
                {
                    p_info_ch->dma_rx_ch = dma_ret;
                    ercd = ESUCCESS;
                }
            }
        }

        /* setup DMA channel for write(if necessary) */
        if (ESUCCESS == ercd)
        {
            if (O_RDONLY_RBSP != p_info_ch->openflag)
            {
                AIOCB* const p_tx_aio = &gb_spdif_dma_tx_end_aiocb;
                p_tx_aio->aio_sigevent.sigev_notify = SIGEV_THREAD;
                p_tx_aio->aio_sigevent.sigev_value.sival_ptr = (void*)p_info_ch;
                p_tx_aio->aio_sigevent.sigev_notify_function = &SPDIF_DMA_TxCallback;

                dma_ch_setup.resource = DMA_RS_SPDIFTXI;
                dma_ch_setup.direction = DMA_REQ_DES;
                dma_ch_setup.dst_width = DMA_UNIT_4;
                dma_ch_setup.src_width = DMA_UNIT_4;
                dma_ch_setup.dst_cnt = DMA_ADDR_FIX;
                dma_ch_setup.src_cnt = DMA_ADDR_INCREMENT;
                dma_ch_setup.p_aio = p_tx_aio;
                dma_ch_setup.int_level = p_info_ch->int_level;
                dma_ret = R_DMA_Setup(p_info_ch->dma_tx_ch, &dma_ch_setup, &dma_ercd);
                if (EERROR == dma_ret)
                {
                    ercd = EFAULT_RBSP;
                }
            }
        }

        /* setup DMA channel for read(if necessary) */
        if (ESUCCESS == ercd)
        {
            if (O_WRONLY_RBSP != p_info_ch->openflag)
            {
                AIOCB* const p_rx_aio = &gb_spdif_dma_rx_end_aiocb;
                p_rx_aio->aio_sigevent.sigev_notify = SIGEV_THREAD;
                p_rx_aio->aio_sigevent.sigev_value.sival_ptr = (void*)p_info_ch;
                p_rx_aio->aio_sigevent.sigev_notify_function = &SPDIF_DMA_RxCallback;

                dma_ch_setup.resource = DMA_RS_SPDIFRXI;
                dma_ch_setup.direction = DMA_REQ_SRC;
                dma_ch_setup.dst_width = DMA_UNIT_4;
                dma_ch_setup.src_width = DMA_UNIT_4;
                dma_ch_setup.src_cnt = DMA_ADDR_FIX;
                dma_ch_setup.p_aio = p_rx_aio;
                dma_ch_setup.dst_cnt = DMA_ADDR_INCREMENT;
                dma_ch_setup.int_level = p_info_ch->int_level;
                dma_ret = R_DMA_Setup(p_info_ch->dma_rx_ch, &dma_ch_setup, &dma_ercd);
                if (EERROR == dma_ret)
                {
                    ercd = EFAULT_RBSP;
                }
            }
        }

        /* start DMA dummy transfer for write(if necessary) */
        if (ESUCCESS == ercd)
        {
            if (O_RDONLY_RBSP != p_info_ch->openflag)
            {
                /* setup short dummy transfer */
                gb_spdif_txdma_dummy_trparam.src_addr = (void*)&spdif_tx_dummy_buf[0];
                gb_spdif_txdma_dummy_trparam.dst_addr = (void*)&SPDIF.TDAD.LONG;
                gb_spdif_txdma_dummy_trparam.count = SPDIF_DUMMY_DMA_TRN_SIZE;

                dma_ret = R_DMA_NextData(p_info_ch->dma_tx_ch, &gb_spdif_txdma_dummy_trparam, &dma_ercd);
                if (EERROR == dma_ret)
                {
                    ercd = EFAULT_RBSP;
                }
                else
                {
                    dma_ret = R_DMA_Start(p_info_ch->dma_tx_ch, &gb_spdif_txdma_dummy_trparam, &dma_ercd);
                    if (EERROR == dma_ret)
                    {
                        ercd = EFAULT_RBSP;
                    }
                }
            }
        }

        /* start DMA dummy transfer for read(if necessary) */
        if (ESUCCESS == ercd)
        {
            if (O_WRONLY_RBSP != p_info_ch->openflag)
            {
                /* setup short dummy transfer */
                gb_spdif_rxdma_dummy_trparam.src_addr = (void*)&SPDIF.RDAD.LONG;
                gb_spdif_rxdma_dummy_trparam.dst_addr = (void*)&spdif_rx_dummy_buf[0];
                gb_spdif_rxdma_dummy_trparam.count = SPDIF_DUMMY_DMA_TRN_SIZE;

                dma_ret = R_DMA_NextData(p_info_ch->dma_rx_ch, &gb_spdif_rxdma_dummy_trparam, &dma_ercd);
                if (EERROR == dma_ret)
                {
                    ercd = EFAULT_RBSP;
                }
                else
                {
                    dma_ret = R_DMA_Start(p_info_ch->dma_rx_ch, &gb_spdif_rxdma_dummy_trparam, &dma_ercd);
                    if (EERROR == dma_ret)
                    {
                        ercd = EFAULT_RBSP;
                    }
                }
            }
        }

        /* enable spdif transfer */
        if (ESUCCESS == ercd)
        {
            /* clear status and enable error interrupt */
            /* enable end interrupt */
            if (O_RDWR_RBSP == p_info_ch->openflag)
            {
                /* start write and read DMA at the same time */
                SPDIF.STAT.LONG &= ~(SPDIF_STAT_READ_ONLY | SPDIF_STAT_UBU | SPDIF_STAT_CSE | SPDIF_STAT_ABU |
                                     SPDIF_STAT_UBO | SPDIF_STAT_CE | SPDIF_STAT_PARE | SPDIF_STAT_PREE | SPDIF_STAT_ABO);
                SPDIF.CTRL.LONG |= (SPDIF_CTRL_TME | SPDIF_CTRL_TDE | SPDIF_CTRL_RME | SPDIF_CTRL_RDE);
            }
            else if (O_WRONLY_RBSP == p_info_ch->openflag)
            {
                /* start write DMA only */
                SPDIF.STAT.LONG &= ~(SPDIF_STAT_READ_ONLY | SPDIF_STAT_UBU | SPDIF_STAT_CSE | SPDIF_STAT_ABU);
                SPDIF.CTRL.LONG |= (SPDIF_CTRL_TME | SPDIF_CTRL_TDE);
            }
            else if (O_RDONLY_RBSP == p_info_ch->openflag)
            {
                /* start read DMA only */
                SPDIF.STAT.LONG &= ~(SPDIF_STAT_READ_ONLY | SPDIF_STAT_UBO | SPDIF_STAT_CE | SPDIF_STAT_PARE | SPDIF_STAT_PREE | SPDIF_STAT_ABO);
                SPDIF.CTRL.LONG |= (SPDIF_CTRL_RME | SPDIF_CTRL_RDE);
            }
            else
            {
                ercd = EINVAL_RBSP;
            }
        }

        /* cleanup dma resources when error occured */
        if (ESUCCESS != ercd)
        {
            if (-1 != p_info_ch->dma_tx_ch)
            {
                uint32_t remain;
                dma_ret = R_DMA_Cancel(p_info_ch->dma_tx_ch, &remain, &dma_ercd);
                if (EERROR == dma_ret)
                {
                    /* NON_NOTICE_ASSERT: unexpected dma error */
                }
            }

            if (-1 != p_info_ch->dma_rx_ch)
            {
                uint32_t remain;
                dma_ret = R_DMA_Cancel(p_info_ch->dma_rx_ch, &remain, &dma_ercd);
                if (EERROR == dma_ret)
                {
                    /* NON_NOTICE_ASSERT: unexpected dma error */
                }
            }

            if (-1 != p_info_ch->dma_tx_ch)
            {
                dma_ret = R_DMA_Free(p_info_ch->dma_tx_ch, &dma_ercd);
                if (EERROR == dma_ret)
                {
                    /* NON_NOTICE_ASSERT: unexpected dma error */
                }
                p_info_ch->dma_tx_ch = -1;
            }

            if (-1 != p_info_ch->dma_rx_ch)
            {
                dma_ret = R_DMA_Free(p_info_ch->dma_rx_ch, &dma_ercd);
                if (EERROR == dma_ret)
                {
                    /* NON_NOTICE_ASSERT: unexpected dma error */
                }
                p_info_ch->dma_rx_ch = -1;
            }
        }
    }

    return ercd;
}

/******************************************************************************
* Function Name: SPDIF_UnInitDMA
* @brief         Free DMA_CH for specified SPDIF channel.
*
*                Description:<br>
*
* @param[in,out] p_info_ch  :channel object.
* @retval        none
******************************************************************************/
void SPDIF_UnInitDMA(spdif_info_ch_t* const p_info_ch)
{
    int_t dma_ret;
    int32_t dma_ercd;

    if (NULL == p_info_ch)
    {
        /* NON_NOTICE_ASSERT: illegal pointer */
    }
    else
    {
        if (-1 != p_info_ch->dma_tx_ch)
        {
            uint32_t remain;
            dma_ret = R_DMA_Cancel(p_info_ch->dma_tx_ch, &remain, &dma_ercd);
            if (EERROR == dma_ret)
            {
                /* NON_NOTICE_ASSERT: unexpected dma error */
            }
        }

        if (-1 != p_info_ch->dma_rx_ch)
        {
            uint32_t remain;
            dma_ret = R_DMA_Cancel(p_info_ch->dma_rx_ch, &remain, &dma_ercd);
            if (EERROR == dma_ret)
            {
                /* NON_NOTICE_ASSERT: unexpected dma error */
            }
        }

        if (-1 != p_info_ch->dma_tx_ch)
        {
            dma_ret = R_DMA_Free(p_info_ch->dma_tx_ch, &dma_ercd);
            if (EERROR == dma_ret)
            {
                /* NON_NOTICE_ASSERT: unexpected dma error */
            }
            p_info_ch->dma_tx_ch = -1;
        }

        if (-1 != p_info_ch->dma_rx_ch)
        {
            dma_ret = R_DMA_Free(p_info_ch->dma_rx_ch, &dma_ercd);
            if (EERROR == dma_ret)
            {
                /* NON_NOTICE_ASSERT: unexpected dma error */
            }
            p_info_ch->dma_rx_ch = -1;
        }
    }

    return;
}

/******************************************************************************
* Function Name: SPDIF_RestartDMA
* @brief         Setup DMA_CH for specified SPDIF channel(without allocate)
*
*                Description:<br>
*
* @param[in,out] p_info_ch  :channel object.
* @retval        ESUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
int_t SPDIF_RestartDMA(spdif_info_ch_t* const p_info_ch, int_t openflag)
{
    int_t ercd = ESUCCESS;
    int_t dma_ret;
    int32_t dma_ercd;
    dma_ch_setup_t  dma_ch_setup;

    if (NULL == p_info_ch)
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        /* setup DMA channel for write(if necessary) */
        if (ESUCCESS == ercd)
        {
            if (O_RDONLY_RBSP != openflag)
            {
                AIOCB* const p_tx_aio = &gb_spdif_dma_tx_end_aiocb;
                p_tx_aio->aio_sigevent.sigev_notify = SIGEV_THREAD;
                p_tx_aio->aio_sigevent.sigev_value.sival_ptr = (void*)p_info_ch;
                p_tx_aio->aio_sigevent.sigev_notify_function = &SPDIF_DMA_TxCallback;

                dma_ch_setup.resource = DMA_RS_SPDIFTXI;
                dma_ch_setup.direction = DMA_REQ_DES;
                dma_ch_setup.dst_width = DMA_UNIT_4;
                dma_ch_setup.src_width = DMA_UNIT_4;
                dma_ch_setup.dst_cnt = DMA_ADDR_FIX;
                dma_ch_setup.src_cnt = DMA_ADDR_INCREMENT;
                dma_ch_setup.p_aio = p_tx_aio;
                dma_ch_setup.int_level = p_info_ch->int_level;
                dma_ret = R_DMA_Setup(p_info_ch->dma_tx_ch, &dma_ch_setup, &dma_ercd);
                if (EERROR == dma_ret)
                {
                    ercd = EFAULT_RBSP;
                }
            }
        }

        /* setup DMA channel for read(if necessary) */
        if (ESUCCESS == ercd)
        {
            if (O_WRONLY_RBSP != openflag)
            {
                AIOCB* const p_rx_aio = &gb_spdif_dma_rx_end_aiocb;
                p_rx_aio->aio_sigevent.sigev_notify = SIGEV_THREAD;
                p_rx_aio->aio_sigevent.sigev_value.sival_ptr = (void*)p_info_ch;
                p_rx_aio->aio_sigevent.sigev_notify_function = &SPDIF_DMA_RxCallback;

                dma_ch_setup.resource = DMA_RS_SPDIFRXI;
                dma_ch_setup.direction = DMA_REQ_SRC;
                dma_ch_setup.dst_width = DMA_UNIT_4;
                dma_ch_setup.src_width = DMA_UNIT_4;
                dma_ch_setup.dst_cnt = DMA_ADDR_INCREMENT;
                dma_ch_setup.src_cnt = DMA_ADDR_FIX;
                dma_ch_setup.p_aio = p_rx_aio;
                dma_ch_setup.int_level = p_info_ch->int_level;
                dma_ret = R_DMA_Setup(p_info_ch->dma_rx_ch, &dma_ch_setup, &dma_ercd);
                if (EERROR == dma_ret)
                {
                    ercd = EFAULT_RBSP;
                }
            }
        }

        /* start DMA dummy transfer for write(if necessary) */
        if (ESUCCESS == ercd)
        {
            if (O_RDONLY_RBSP != openflag)
            {
                /* setup short dummy transfer */
                gb_spdif_txdma_dummy_trparam.src_addr = (void*)&spdif_tx_dummy_buf[0];
                gb_spdif_txdma_dummy_trparam.dst_addr = (void*)&SPDIF.TDAD.LONG;
                gb_spdif_txdma_dummy_trparam.count = SPDIF_DUMMY_DMA_TRN_SIZE;

                dma_ret = R_DMA_NextData(p_info_ch->dma_tx_ch, &gb_spdif_txdma_dummy_trparam, &dma_ercd);
                if (EERROR == dma_ret)
                {
                    ercd = EFAULT_RBSP;
                }
                else
                {
                    dma_ret = R_DMA_Start(p_info_ch->dma_tx_ch, &gb_spdif_txdma_dummy_trparam, &dma_ercd);
                    if (EERROR == dma_ret)
                    {
                        ercd = EFAULT_RBSP;
                    }
                }
            }
        }

        /* start DMA dummy transfer for read(if necessary) */
        if (ESUCCESS == ercd)
        {
            if (O_WRONLY_RBSP != openflag)
            {
                /* setup short dummy transfer */
                gb_spdif_rxdma_dummy_trparam.src_addr = (void*)&SPDIF.RDAD.LONG;
                gb_spdif_rxdma_dummy_trparam.dst_addr = (void*)&spdif_rx_dummy_buf[0];
                gb_spdif_rxdma_dummy_trparam.count = SPDIF_DUMMY_DMA_TRN_SIZE;

                dma_ret = R_DMA_NextData(p_info_ch->dma_rx_ch, &gb_spdif_rxdma_dummy_trparam, &dma_ercd);
                if (EERROR == dma_ret)
                {
                    ercd = EFAULT_RBSP;
                }
                else
                {
                    dma_ret = R_DMA_Start(p_info_ch->dma_rx_ch, &gb_spdif_rxdma_dummy_trparam, &dma_ercd);
                    if (EERROR == dma_ret)
                    {
                        ercd = EFAULT_RBSP;
                    }
                }
            }
        }

        /* enable spdif transfer */
        if (ESUCCESS == ercd)
        {
            /* clear status and enable error interrupt */
            /* enable end interrupt */
            if (O_RDWR_RBSP == openflag)
            {
                /* start write and read DMA at the same time */
                SPDIF.STAT.LONG &= ~(SPDIF_STAT_UBU | SPDIF_STAT_CSE | SPDIF_STAT_ABU);
                SPDIF.STAT.LONG &= ~(SPDIF_STAT_UBO | SPDIF_STAT_CE | SPDIF_STAT_PARE | SPDIF_STAT_PREE | SPDIF_STAT_ABO);
                SPDIF.CTRL.LONG |= (SPDIF_CTRL_TME | SPDIF_CTRL_TDE | SPDIF_CTRL_RME | SPDIF_CTRL_RDE);
            }
            else if (O_WRONLY_RBSP == openflag)
            {
                /* start write DMA only */
                SPDIF.STAT.LONG &= ~(SPDIF_STAT_UBU | SPDIF_STAT_CSE | SPDIF_STAT_ABU);
                SPDIF.CTRL.LONG |= (SPDIF_CTRL_TME | SPDIF_CTRL_TDE);
            }
            else if (O_RDONLY_RBSP == openflag)
            {
                /* start read DMA only */
                SPDIF.STAT.LONG &= ~(SPDIF_STAT_UBO | SPDIF_STAT_CE | SPDIF_STAT_PARE | SPDIF_STAT_PREE | SPDIF_STAT_ABO);
                SPDIF.CTRL.LONG |= (SPDIF_CTRL_RME | SPDIF_CTRL_RDE);
            }
            else
            {
                ercd = EINVAL_RBSP;
            }
        }
    }

    return ercd;
}

/******************************************************************************
* Function Name: SPDIF_CancelDMA
* @brief         Pause DMA transfer for specified SPDIF channel.
*
*                Description:<br>
*
* @param[in,out] p_info_ch  :channel object.
* @retval        none
******************************************************************************/
void SPDIF_CancelDMA(const spdif_info_ch_t* const p_info_ch, int_t openflag)
{
    int_t dma_ret;
    int32_t dma_ercd;

    if (NULL == p_info_ch)
    {
        /* NON_NOTICE_ASSERT: illegal pointer */
    }
    else
    {
        if (O_RDONLY_RBSP != openflag) {
            if (-1 != p_info_ch->dma_tx_ch)
            {
                uint32_t remain;
                dma_ret = R_DMA_Cancel(p_info_ch->dma_tx_ch, &remain, &dma_ercd);
                if (EERROR == dma_ret)
                {
                    /* NON_NOTICE_ASSERT: unexpected dma error */
                }
            }
        }

        if (O_WRONLY_RBSP != openflag) {
            if (-1 != p_info_ch->dma_rx_ch)
            {
                uint32_t remain;
                dma_ret = R_DMA_Cancel(p_info_ch->dma_rx_ch, &remain, &dma_ercd);
                if (EERROR == dma_ret)
                {
                    /* NON_NOTICE_ASSERT: unexpected dma error */
                }
            }
        }
    }

    return;
}

/******************************************************************************
Private functions
******************************************************************************/

/******************************************************************************
* Function Name: SPDIF_DMA_TxCallback
* @brief         DMA callback function
*
*                Description:<br>
*
* @param[in]     param      :callback param
* @retval        none
******************************************************************************/
static void SPDIF_DMA_TxCallback(const union sigval param)
{
    spdif_info_ch_t* const p_info_ch = param.sival_ptr;
    dma_trans_data_t dma_data_next;
#if(1) /* mbed */
    int32_t ercd = ESUCCESS;
#else
    int_t ercd = ESUCCESS;
#endif
    int_t ret;
    AIOCB* p_aio_work;

    if (NULL == p_info_ch)
    {
        /* NON_NOTICE_ASSERT: illegal pointer */
    }
    else
    {
        if ((p_info_ch->p_aio_tx_next != NULL)
         && (p_info_ch->ch_sts_set_req != false)) {
            p_info_ch->ch_sts_set_req = false;
            if ((p_info_ch->tlcs != p_info_ch->ch_sts[0])
             || (p_info_ch->trcs != p_info_ch->ch_sts[1])) {
	            p_info_ch->tlcs = p_info_ch->ch_sts[0];
	            p_info_ch->trcs = p_info_ch->ch_sts[1];
                SPDIF.CTRL.BIT.NCSI = 1;
            }
        }
        if ((p_info_ch->p_aio_tx_next != NULL)
         && (p_info_ch->p_aio_tx_next->aio_return == SPDIF_ASYNC_STRUCT_W)) {
            spdif_t * p_wk_spdif = (spdif_t*)p_info_ch->p_aio_tx_next->aio_buf;

            if ((p_info_ch->tlcs != p_wk_spdif->s_buff[0])
             || (p_info_ch->trcs != p_wk_spdif->s_buff[1])) {
                p_info_ch->tlcs = p_wk_spdif->s_buff[0];
                p_info_ch->trcs = p_wk_spdif->s_buff[1];
                SPDIF.CTRL.BIT.NCSI = 1;
            }
        }
        if (NULL == p_info_ch->p_aio_tx_curr)
        {
            /* now complete dummy transfer, It isn't neccessary to signal. */
        }
        else
        {
            /* now complete user request transfer, Signal to application */

            /* return aio complete */
            p_info_ch->p_aio_tx_curr->aio_return = (ssize_t)p_info_ch->p_aio_tx_curr->aio_nbytes;
            ahf_complete(&p_info_ch->tx_que, p_info_ch->p_aio_tx_curr);
        }

        /* copy next to curr(even if it's NULL) */
        p_info_ch->p_aio_tx_curr = p_info_ch->p_aio_tx_next;

        /* get next request(It's maybe NULL) */
        while (1) {
            p_aio_work = ahf_removehead(&p_info_ch->tx_que);
            if ((p_aio_work == NULL)
             || (p_aio_work->aio_return != SPDIF_ASYNC_SET_CH_STS)) {
                break;
            } else {
                p_info_ch->ch_sts_set_req = true;
                ahf_complete(&p_info_ch->tx_que, p_aio_work);
            }
        }
        p_info_ch->p_aio_tx_next = p_aio_work;

        p_info_ch->tu_idx = 0;
        SPDIF_UserInfoRegSet(p_info_ch);

        if (NULL != p_info_ch->p_aio_tx_next)
        {
            /* add user request */
            if (p_info_ch->p_aio_tx_next->aio_return == SPDIF_ASYNC_STRUCT_W) {
                dma_data_next.src_addr = (void*)(((spdif_t*)p_info_ch->p_aio_tx_next->aio_buf)->a_buff);
                p_info_ch->p_aio_tx_next->aio_nbytes = SPDIF_AUDIO_BUFFSIZE * 4;
            } else {
                dma_data_next.src_addr = (void*)p_info_ch->p_aio_tx_next->aio_buf;
            }
            dma_data_next.count = (uint32_t)p_info_ch->p_aio_tx_next->aio_nbytes;
            dma_data_next.dst_addr = (void*)&SPDIF.TDAD.LONG;
            ret = R_DMA_NextData(p_info_ch->dma_tx_ch, &dma_data_next, &ercd);
            if (EERROR == ret)
            {
                /* NON_NOTICE_ASSERT: unexpected DMA error */
            }
        }
        else
        {
            /* add dummy request */
            ret = R_DMA_NextData(p_info_ch->dma_tx_ch, &gb_spdif_txdma_dummy_trparam, &ercd);
            if (EERROR == ret)
            {
                /* NON_NOTICE_ASSERT: unexpected DMA error */
            }
        }
    }

    return;
}

/******************************************************************************
* Function Name: SPDIF_DMA_RxCallback
* @brief         DMA callback function
*
*                Description:<br>
*
* @param[in]     param      :callback param
* @retval        none
******************************************************************************/
static void SPDIF_DMA_RxCallback(const union sigval param)
{
    spdif_info_ch_t* const p_info_ch = param.sival_ptr;
    dma_trans_data_t dma_data_next;
#if(1) /* mbed */
    int32_t ercd = ESUCCESS;
#else
    int_t ercd = ESUCCESS;
#endif
    int_t ret;

    if (NULL == p_info_ch)
    {
        /* NON_NOTICE_ASSERT: illegal pointer */
    }
    else
    {
        SPDIF_UserInfoRegGet(p_info_ch);
        if (NULL == p_info_ch->p_aio_rx_curr)
        {
            /* now complete dummy transfer, It isn't neccessary to signal. */
        }
        else
        {
            /* now complete user request transfer, Signal to application */
            if (p_info_ch->p_aio_rx_curr->aio_return == SPDIF_ASYNC_STRUCT_R) {
                spdif_t * p_wk_spdif = (spdif_t*)p_info_ch->p_aio_rx_curr->aio_buf;

                p_wk_spdif->s_buff[0] = p_info_ch->rlcs;
                p_wk_spdif->s_buff[1] = p_info_ch->rrcs;
            }

            /* return aio complete */
            p_info_ch->p_aio_rx_curr->aio_return = (ssize_t)p_info_ch->p_aio_rx_curr->aio_nbytes;
            ahf_complete(&p_info_ch->rx_que, p_info_ch->p_aio_rx_curr);
        }

        /* copy next to curr(even if it's NULL) */
        p_info_ch->p_aio_rx_curr = p_info_ch->p_aio_rx_next;
        p_info_ch->ru_idx = 0;

        /* get next request(It's maybe NULL) */
        p_info_ch->p_aio_rx_next = ahf_removehead(&p_info_ch->rx_que);

        if (NULL != p_info_ch->p_aio_rx_next)
        {
            /* add user request */
            if (p_info_ch->p_aio_rx_next->aio_return == SPDIF_ASYNC_STRUCT_R) {
                dma_data_next.dst_addr = (void*)(((spdif_t*)p_info_ch->p_aio_rx_next->aio_buf)->a_buff);
                p_info_ch->p_aio_rx_next->aio_nbytes = SPDIF_AUDIO_BUFFSIZE * 4;
            } else {
                dma_data_next.dst_addr = (void*)p_info_ch->p_aio_rx_next->aio_buf;
            }
            dma_data_next.count = (uint32_t)p_info_ch->p_aio_rx_next->aio_nbytes;
            dma_data_next.src_addr = (void*)&SPDIF.RDAD.LONG;

            ret = R_DMA_NextData(p_info_ch->dma_rx_ch, &dma_data_next, &ercd);
            if (EERROR == ret)
            {
                /* NON_NOTICE_ASSERT: unexpected DMA error */
            }
        }
        else
        {
            /* add dummy request */
            ret = R_DMA_NextData(p_info_ch->dma_rx_ch, &gb_spdif_rxdma_dummy_trparam, &ercd);
            if (EERROR == ret)
            {
                /* NON_NOTICE_ASSERT: unexpected DMA error */
            }
        }
    }

    return;
}
