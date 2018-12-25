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
* Copyright (C) 2013-2016 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

/**************************************************************************//**
* @file         scux_dev.c
* $Rev: $
* $Date::                           $
* @brief        SCUX device control functions
******************************************************************************/

/*******************************************************************************
Includes <System Includes>, "Project Includes"
*******************************************************************************/

#include "scux.h"
#include  "mbed_critical.h"

/******************************************************************************
Exported global variables (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global driver management information
******************************************************************************/

/* SSIF clock mask register table define */
static const uint32_t gb_cpg_scux_ssif_stbcr_bit[SCUX_SSIF_CH_NUM] =
{
    CPG_STBCR11_BIT_MSTP115,    /* SSIF0 */
    CPG_STBCR11_BIT_MSTP114,    /* SSIF1 */
    CPG_STBCR11_BIT_MSTP113,    /* SSIF2 */
    CPG_STBCR11_BIT_MSTP112,    /* SSIF3 */
#if defined(TARGET_RZA1H) /* mbed */
    CPG_STBCR11_BIT_MSTP111,    /* SSIF4 */
    CPG_STBCR11_BIT_MSTP110     /* SSIF5 */
#endif /* mbed */
};

/* SSIF software reset register table define */
static const uint32_t gb_cpg_scux_ssif_swrst_bit[SCUX_SSIF_CH_NUM] =
{
    CPG_SWRSTCR1_BIT_SRST16,    /* SSIF0 */
    CPG_SWRSTCR1_BIT_SRST15,    /* SSIF1 */
    CPG_SWRSTCR1_BIT_SRST14,    /* SSIF2 */
    CPG_SWRSTCR1_BIT_SRST13,    /* SSIF3 */
#if defined(TARGET_RZA1H) /* mbed */
    CPG_SWRSTCR1_BIT_SRST12,    /* SSIF4 */
    CPG_SWRSTCR1_BIT_SRST11     /* SSIF5 */
#endif /* mbed */
};

/* SSIF noise cancel register table define */
static const uint32_t gb_cpg_scux_ssif_sncr_bit[SCUX_SSIF_CH_NUM] =
{
    SCUX_SSIF_GPIO_SNCR_SHIFT_CH0,  /* SSIF0 */
    SCUX_SSIF_GPIO_SNCR_SHIFT_CH1,  /* SSIF1 */
    SCUX_SSIF_GPIO_SNCR_SHIFT_CH2,  /* SSIF2 */
    SCUX_SSIF_GPIO_SNCR_SHIFT_CH3,  /* SSIF3 */
#if defined(TARGET_RZA1H) /* mbed */
    SCUX_SSIF_GPIO_SNCR_SHIFT_CH4,  /* SSIF4 */
    SCUX_SSIF_GPIO_SNCR_SHIFT_CH5   /* SSIF5 */
#endif /* mbed */
};

/* AIOCB information for DMA */
static AIOCB gb_scux_write_dma_aio[SCUX_CH_NUM];
static AIOCB gb_scux_read_dma_aio[SCUX_CH_NUM];

/******************************************************************************
 Function prototypes
******************************************************************************/

static void  SCUX_SetupDataPosition(scux_info_ch_t * const p_scux_info_ch);
static void  SCUX_SetupSrcClk(scux_info_ch_t * const p_scux_info_ch);
static void  SCUX_SetupFifo(scux_info_ch_t * const p_scux_info_ch);
static void  SCUX_SetupSrcFunction(scux_info_ch_t * const p_scux_info_ch);
static void  SCUX_SetupDvuVolume(scux_info_ch_t * const p_scux_info_ch);
static void  SCUX_SetupMix(scux_info_ch_t * const p_scux_info_ch);
static void  SCUX_DMA_CopyRxCallBack(union sigval const param);
static void  SCUX_DMA_CommonTxNextDummyData(scux_info_ch_t * const p_info_ch);
static void  SCUX_DMA_CopyTxEndFlush(scux_info_ch_t * const p_info_ch);
static void  SCUX_DMA_CopyTxNextRemainData(scux_info_ch_t * const p_info_ch);
static void  SCUX_DMA_CopyTxNextData(scux_info_ch_t * const p_info_ch);
static void  SCUX_DMA_CopyTxCallBack(union sigval const param);
static void  SCUX_DMA_DirectTxEndFlush(scux_info_ch_t * const p_info_ch);
static void  SCUX_DMA_DirectTxNextRemainData(scux_info_ch_t * const p_info_ch);
static void  SCUX_DMA_DirectTxNextData(scux_info_ch_t * const p_info_ch);
static void  SCUX_DMA_DirectTxCallBack(union sigval const param);

/**************************************************************************//**
* Function Name: SCUX_CopyWriteStart
* @brief         Start write request (mem->mem).
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch:SCUX channel information.
* @param[in]     *p_write_aio:Write request information.
* @retval        ESUCCESS : Operation successful.
*                EFAULT_RBSP : Internal error is occured.
******************************************************************************/

int_t SCUX_CopyWriteStart(scux_info_ch_t * const p_scux_info_ch, AIOCB * const p_write_aio)
{
    int_t    retval;
    int_t    dma_retval;
    dma_trans_data_t dma_address_param;
    uint32_t         dma_src_addr;
#if(1) /* mbed */
    int32_t          ret;
#else
    int_t            ret;
#endif

    if ((NULL == p_scux_info_ch) || (NULL == p_write_aio))
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        retval = R_DMA_Setup(p_scux_info_ch->dma_tx_ch, &p_scux_info_ch->dma_tx_setup, NULL);

        if (ESUCCESS != retval)
        {
            retval = EFAULT_RBSP;
        }
        else
        {
            /* set first trans information */
            p_scux_info_ch->p_tx_aio = p_write_aio;
            p_scux_info_ch->p_tx_next_aio = NULL;
            p_scux_info_ch->first_tx_flag = false;

            /* cast uint8_t pointer from void pointer */
            dma_src_addr = (uint32_t)p_write_aio->aio_buf;

            /* set 1st DMA parameter */
            dma_address_param.src_addr = (void *)dma_src_addr;
            dma_address_param.dst_addr = (void *)p_scux_info_ch->p_scux_reg->dmatd_n_cim;
            dma_address_param.count = p_write_aio->aio_nbytes;

            dma_retval = R_DMA_Start(p_scux_info_ch->dma_tx_ch, &dma_address_param, &ret);
            if (ESUCCESS != dma_retval)
            {
                retval = EFAULT_RBSP;
            }
            else
            {
                p_scux_info_ch->dma_tx_current_size = dma_address_param.count;
                /* enable interrupt */
                p_scux_info_ch->p_scux_reg->p_ffd_reg->DEVMR_FFD0_0 = (DEVMR_FFD0_DEVMUF_SET |
                                                                       DEVMR_FFD0_DEVMOF_SET |
                                                                       DEVMR_FFD0_DEVMOL_SET |
                                                                       DEVMR_FFD0_DEVMIUF_SET);
                p_scux_info_ch->p_scux_reg->p_ffd_reg->DEVCR_FFD0_0 = DEVCR_FFD0_INIT_VALUE;
                /* start clock devide on async mode */
                if (false == p_scux_info_ch->src_cfg.mode_sync)
                {
                    if ((SCUX_CLK_SSIF0_WS != p_scux_info_ch->src_cfg.input_clk_async) &&
                        (SCUX_CLK_SSIF1_WS != p_scux_info_ch->src_cfg.input_clk_async) &&
                        (SCUX_CLK_SSIF2_WS != p_scux_info_ch->src_cfg.input_clk_async) &&
#if defined(TARGET_RZA1H) /* mbed */
                        (SCUX_CLK_SSIF3_WS != p_scux_info_ch->src_cfg.input_clk_async) &&
                        (SCUX_CLK_SSIF4_WS != p_scux_info_ch->src_cfg.input_clk_async) &&
                        (SCUX_CLK_SSIF5_WS != p_scux_info_ch->src_cfg.input_clk_async))
#else /* mbed */
                        (SCUX_CLK_SSIF3_WS != p_scux_info_ch->src_cfg.input_clk_async))
#endif /* mbed */
                    {
                        *(p_scux_info_ch->p_scux_reg->fdtsel_n_cim) |= FDTSEL_CIM_DIVEN_SET;
                    }
                }
                /* update channel status */
                switch (p_scux_info_ch->ch_stat)
                {
                    case SCUX_CH_UNINIT :
                        /* fall through */
                    case SCUX_CH_INIT :
                        /* fall through */
                    case SCUX_CH_STOP :
                        retval = EBADF_RBSP;
                    break;

                    case SCUX_CH_TRANS_IDLE :
                        p_scux_info_ch->ch_stat = SCUX_CH_TRANS_WR;
                    break;

                    case SCUX_CH_TRANS_RD :
                        p_scux_info_ch->ch_stat = SCUX_CH_TRANS_RDWR;
                    break;

                    case SCUX_CH_TRANS_WR :
                        /* fall through */
                    case SCUX_CH_TRANS_RDWR :
                        /* fall through */
                    case SCUX_CH_STOP_WAIT :
                        /* fall through */
                    case SCUX_CH_STOP_WAIT_IDLE :
                        /* do nothing, stats isn't updated */
                    break;

                    default :
                        retval = EFAULT_RBSP;
                        /* NOTREACHED on At the time of a normal performance */
                    break;
                }
            }
        }
    }

    return retval;
}

/******************************************************************************
End of function SCUX_CopyWriteStart
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_DirectWriteStart
* @brief         Start write request(SSIF direct route).
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch:SCUX channel information.
* @param[in]     *p_write_aio:Write request information.
* @retval        ESUCCESS : Operation successful.
*                EFAULT_RBSP : Internal error is occured.
******************************************************************************/

int_t SCUX_DirectWriteStart(scux_info_ch_t * const p_scux_info_ch, AIOCB * const p_write_aio)
{
    int_t    retval;
    int_t    dma_retval;
    dma_trans_data_t dma_address_param;
    dma_trans_data_t next_dma_address_param;
    uint32_t         dma_src_addr;
#if(1) /* mbed */
    int32_t          ret;
#else
    int_t            ret;
#endif
    uint32_t         ramp_wait_cnt = 0;

    if ((NULL == p_scux_info_ch) || (NULL == p_write_aio))
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        retval = R_DMA_Setup(p_scux_info_ch->dma_tx_ch, &p_scux_info_ch->dma_tx_setup, NULL);

        if (ESUCCESS != retval)
        {
            retval = EFAULT_RBSP;
        }
        else
        {
            /* set first trans information */
            p_scux_info_ch->p_tx_aio = NULL;
            p_scux_info_ch->p_tx_next_aio = p_write_aio;
            p_scux_info_ch->first_tx_flag = true;

            /* cast uint8_t pointer from void pointer */
            dma_src_addr = (uint32_t)p_write_aio->aio_buf;

            /* set 2nd DMA parameter */
            next_dma_address_param.src_addr = (void *)(dma_src_addr + (p_write_aio->aio_nbytes / SCUX_HALF_SIZE_VALUE));
            next_dma_address_param.dst_addr = (void *)p_scux_info_ch->p_scux_reg->dmatd_n_cim;
            next_dma_address_param.count = (p_write_aio->aio_nbytes / SCUX_HALF_SIZE_VALUE);
            if (0U != (p_write_aio->aio_nbytes % SCUX_HALF_SIZE_VALUE))
            {
                /* last data ,when even value size */
                next_dma_address_param.count++;
            }
            dma_retval = R_DMA_NextData(p_scux_info_ch->dma_tx_ch, &next_dma_address_param, &ret);

            if (ESUCCESS != dma_retval)
            {
                retval = EFAULT_RBSP;
            }
            else
            {
                p_scux_info_ch->dma_tx_next_size = next_dma_address_param.count;
                /* set 1st DMA parameter */
                dma_address_param.src_addr = (void *)dma_src_addr;
                dma_address_param.dst_addr = (void *)p_scux_info_ch->p_scux_reg->dmatd_n_cim;
                dma_address_param.count = (p_write_aio->aio_nbytes / SCUX_HALF_SIZE_VALUE);

                dma_retval = R_DMA_Start(p_scux_info_ch->dma_tx_ch, &dma_address_param, &ret);
                if (ESUCCESS != dma_retval)
                {
                    retval = EFAULT_RBSP;
                }
                else
                {
                    p_scux_info_ch->dma_tx_current_size = dma_address_param.count;
                    /* enable interrupt */
                    p_scux_info_ch->p_scux_reg->p_ffd_reg->DEVMR_FFD0_0 = (DEVMR_FFD0_DEVMUF_SET |
                                                                           DEVMR_FFD0_DEVMOF_SET |
                                                                           DEVMR_FFD0_DEVMOL_SET |
                                                                           DEVMR_FFD0_DEVMIUF_SET);
                    p_scux_info_ch->p_scux_reg->p_ffd_reg->DEVCR_FFD0_0 = DEVCR_FFD0_INIT_VALUE;
                    *(p_scux_info_ch->p_scux_reg->fdtsel_n_cim) |= FDTSEL_CIM_DIVEN_SET;

                    switch (p_scux_info_ch->ch_stat)
                    {
                        case SCUX_CH_UNINIT :
                            /* fall through */
                        case SCUX_CH_INIT :
                            /* fall through */
                        case SCUX_CH_STOP :
                            /* fall through */
                            retval = EBADF_RBSP;
                        break;

                        case SCUX_CH_TRANS_IDLE :
                            p_scux_info_ch->ch_stat = SCUX_CH_TRANS_WR;
                        break;

                        case SCUX_CH_TRANS_RD :
                            /* read function disabled */
                            retval = EFAULT_RBSP;
                        break;

                        case SCUX_CH_TRANS_WR :
                            /* do nothing, stats isn't updated */
                        break;

                        case SCUX_CH_TRANS_RDWR :
                            /* read function disabled */
                            retval = EFAULT_RBSP;
                        break;

                        case SCUX_CH_STOP_WAIT :
                            /* do nothing, stats isn't updated */
                        break;

                        case SCUX_CH_STOP_WAIT_IDLE :
                            /* read function disabled */
                            retval = EFAULT_RBSP;
                        break;

                        default :
                            /* NOTREACHED on At the time of a normal performance */
                            retval = EFAULT_RBSP;
                        break;
                    }

                    /* set ramp setting */
                    if (false == p_scux_info_ch->first_ramp_flag)
                    {
                        while((DVUSR_DVU0_VRSTS_VOL_FIX != (p_scux_info_ch->p_scux_reg->p_dvu_reg->DVUSR_DVU0_0 & DVUSR_DVU0_VRSTS_MASK)) &&
                              (SCUX_RAMP_WAIT_MAX > ramp_wait_cnt))
                        {
                            ramp_wait_cnt++;
                        }
                        if (SCUX_RAMP_WAIT_MAX <= ramp_wait_cnt)
                        {
                            retval = EFAULT_RBSP;
                        }
                        else
                        {
                            p_scux_info_ch->p_scux_reg->p_dvu_reg->DVUER_DVU0_0 &= ~DVUER_DVU0_DVUEN_SET;
                            SCUX_SetRampVolRegister(p_scux_info_ch);
                            p_scux_info_ch->p_scux_reg->p_dvu_reg->DVUER_DVU0_0 |= DVUER_DVU0_DVUEN_SET;
                            p_scux_info_ch->first_ramp_flag = true;
                        }
                    }
                }
            }
        }
    }

    return retval;
}

/******************************************************************************
End of function SCUX_DirectWriteStart
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_CopyReadStart
* @brief         Start read request(mem -> mem).
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch:SCUX channel information.
* @param[in]     *p_read_aio:Read request information.
* @retval        ESUCCESS : Operation successful.
*                EFAULT_RBSP : Internal error is occured.
******************************************************************************/

int_t SCUX_CopyReadStart(scux_info_ch_t * const p_scux_info_ch, AIOCB * const p_read_aio)
{
    int_t    retval;
    int_t    dma_retval;
    dma_trans_data_t dma_address_param;
    uint32_t         dma_dst_addr;

    if ((NULL == p_scux_info_ch) || (NULL == p_read_aio))
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        retval = R_DMA_Setup(p_scux_info_ch->dma_rx_ch, &p_scux_info_ch->dma_rx_setup, NULL);

        if (ESUCCESS != retval)
        {
            retval = EFAULT_RBSP;
        }
        else
        {
            /* set first trans information */
            p_scux_info_ch->p_rx_aio = p_read_aio;
            p_scux_info_ch->p_rx_next_aio = NULL;
            p_scux_info_ch->first_rx_flag = false;

            /* cast uint8_t pointer from void pointer */
            dma_dst_addr = (uint32_t)p_read_aio->aio_buf;

            /* set 1st DMA parameter */
            dma_address_param.src_addr = (void *)p_scux_info_ch->p_scux_reg->dmatu_n_cim;
            dma_address_param.dst_addr = (void *)dma_dst_addr;
            dma_address_param.count = p_read_aio->aio_nbytes;

            dma_retval = R_DMA_Start(p_scux_info_ch->dma_rx_ch, &dma_address_param, NULL);

            if (ESUCCESS != dma_retval)
            {
                retval = EFAULT_RBSP;
            }
            else
            {
                /* enable interrupt */
                p_scux_info_ch->dma_rx_current_size = dma_address_param.count;
                p_scux_info_ch->p_scux_reg->p_ffu_reg->UEVMR_FFU0_0 = (UEVMR_FFU0_UEVMUF_SET |
                                                                       UEVMR_FFU0_UEVMOF_SET |
                                                                       UEVMR_FFU0_UEVMOL_SET);
                p_scux_info_ch->p_scux_reg->p_ffu_reg->UEVCR_FFU0_0 = DEVCR_FFD0_INIT_VALUE;
                /* start clock devide on async mode */
                if (false == p_scux_info_ch->src_cfg.mode_sync)
                {
                    if ((SCUX_CLK_SSIF0_WS != p_scux_info_ch->src_cfg.input_clk_async) &&
                        (SCUX_CLK_SSIF1_WS != p_scux_info_ch->src_cfg.input_clk_async) &&
                        (SCUX_CLK_SSIF2_WS != p_scux_info_ch->src_cfg.input_clk_async) &&
#if defined(TARGET_RZA1H) /* mbed */
                        (SCUX_CLK_SSIF3_WS != p_scux_info_ch->src_cfg.input_clk_async) &&
                        (SCUX_CLK_SSIF4_WS != p_scux_info_ch->src_cfg.input_clk_async) &&
                        (SCUX_CLK_SSIF5_WS != p_scux_info_ch->src_cfg.input_clk_async))
#else /* mbed */
                        (SCUX_CLK_SSIF3_WS != p_scux_info_ch->src_cfg.input_clk_async))
#endif /* mbed */
                    {
                        p_scux_info_ch->futsel_cim_value |= FUTSEL_CIM_DIVEN_SET;
                        *(p_scux_info_ch->p_scux_reg->futsel_n_cim) = p_scux_info_ch->futsel_cim_value;
                    }
                }
                /* update channel status */

                switch (p_scux_info_ch->ch_stat)
                {
                    case SCUX_CH_UNINIT :
                        /* fall through */
                    case SCUX_CH_INIT :
                        /* fall through */
                    case SCUX_CH_STOP :
                        /* fall through */
                        retval = EBADF_RBSP;
                    break;

                    case SCUX_CH_TRANS_IDLE :
                        p_scux_info_ch->ch_stat = SCUX_CH_TRANS_RD;
                    break;

                    case SCUX_CH_TRANS_RD :
                        /* do nothing, stats isn't updated */
                    break;

                    case SCUX_CH_TRANS_WR :
                        p_scux_info_ch->ch_stat = SCUX_CH_TRANS_RDWR;
                    break;

                    case SCUX_CH_TRANS_RDWR :
                        /* fall through */
                    case SCUX_CH_STOP_WAIT :
                        /* do nothing, stats isn't updated */
                    break;

                    case SCUX_CH_STOP_WAIT_IDLE :
                        p_scux_info_ch->ch_stat = SCUX_CH_STOP_WAIT;
                    break;

                    default :
                        /* NOTREACHED on At the time of a normal performance */
                        retval = EFAULT_RBSP;
                    break;
                }
            }
        }
    }

    return retval;
}

/******************************************************************************
End of function SCUX_CopyReadStart
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_FlushWriteStart
* @brief         Start dummy write for FIFO flush.
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch:SCUX channel information.
* @param[in]     *p_write_aio:Write request information.
* @retval        ESUCCESS : Operation successful.
*                EFAULT_RBSP : Internal error is occured.
******************************************************************************/

int_t SCUX_FlushWriteStart(scux_info_ch_t * const p_scux_info_ch)
{
    int_t retval = ESUCCESS;
    dma_trans_data_t dma_address_param;

    if (NULL == p_scux_info_ch)
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        if (0U != p_scux_info_ch->flush_stop_size)
        {
            retval = R_DMA_Setup(p_scux_info_ch->dma_tx_ch, &p_scux_info_ch->dma_tx_setup, NULL);

            if (ESUCCESS != retval)
            {
                retval = EFAULT_RBSP;
            }
            else
            {
                /* trans dummy data size is FIFO size unit */
                if (p_scux_info_ch->flush_stop_size >= p_scux_info_ch->fifo_size)
                {
                    dma_address_param.count = p_scux_info_ch->fifo_size;
                    p_scux_info_ch->flush_stop_size -= p_scux_info_ch->fifo_size;
                }
                else
                {
                    /* last data is fraction of FIFO size */
                    dma_address_param.count = p_scux_info_ch->flush_stop_size;
                    p_scux_info_ch->flush_stop_size = 0;
                }

                dma_address_param.src_addr = (void *)(p_scux_info_ch->p_tx_dummy_data);
                dma_address_param.dst_addr = (void *)p_scux_info_ch->p_scux_reg->dmatd_n_cim;

                retval = R_DMA_Start(p_scux_info_ch->dma_tx_ch, &dma_address_param, NULL);
                if (ESUCCESS != retval)
                {
                    retval = EFAULT_RBSP;
                }
                else
                {
                    p_scux_info_ch->dma_tx_current_size = dma_address_param.count;

                    p_scux_info_ch->tx_dummy_run_flag = true;

                    if ((SCUX_CH_TRANS_RD == p_scux_info_ch->ch_stat) || (SCUX_CH_TRANS_IDLE == p_scux_info_ch->ch_stat))
                    {
                        /* restart write operation when write is stopped */
                        p_scux_info_ch->p_scux_reg->p_ffd_reg->DEVMR_FFD0_0 = (DEVMR_FFD0_DEVMUF_SET |
                                                                               DEVMR_FFD0_DEVMOF_SET |
                                                                               DEVMR_FFD0_DEVMOL_SET |
                                                                               DEVMR_FFD0_DEVMIUF_SET);
                        p_scux_info_ch->p_scux_reg->p_ffd_reg->DEVCR_FFD0_0
                        = DEVCR_FFD0_INIT_VALUE;
                        /* start clock devide on async mode or SSIF direct route setting */
                        if (false == p_scux_info_ch->src_cfg.mode_sync)
                        {
                            if((SCUX_CLK_SSIF0_WS != p_scux_info_ch->src_cfg.input_clk_async) &&
                               (SCUX_CLK_SSIF1_WS != p_scux_info_ch->src_cfg.input_clk_async) &&
                               (SCUX_CLK_SSIF2_WS != p_scux_info_ch->src_cfg.input_clk_async) &&
#if defined(TARGET_RZA1H) /* mbed */
                               (SCUX_CLK_SSIF3_WS != p_scux_info_ch->src_cfg.input_clk_async) &&
                               (SCUX_CLK_SSIF4_WS != p_scux_info_ch->src_cfg.input_clk_async) &&
                               (SCUX_CLK_SSIF5_WS != p_scux_info_ch->src_cfg.input_clk_async)) 
#else /* mbed */
                               (SCUX_CLK_SSIF3_WS != p_scux_info_ch->src_cfg.input_clk_async))
#endif /* mbed */
                            {
                                *(p_scux_info_ch->p_scux_reg->fdtsel_n_cim) |= FDTSEL_CIM_DIVEN_SET;
                            }
                        }
                    }
                }
            }
        }
    }

    return retval;
}

/******************************************************************************
End of function SCUX_FlushWriteStart
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_CopyCancelSpecific
* @brief         Cancel specific request(mem->mem).
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @param[in]     *p_cancel_aio : Cancel request information.
* @retval        ESUCCESS : Operation successful.
*                EBUSY_RBSP  : Cancel requst is on going.
*                EFAULT_RBSP : Internal error is occured.
******************************************************************************/

#if(1) /* mbed */
#if defined (__CC_ARM)
#pragma O0
#endif
#endif /* end mbed */
int_t SCUX_CopyCancelSpecific(scux_info_ch_t * const p_scux_info_ch, AIOCB * const p_cancel_aio)
{
    int_t    retval = ESUCCESS;
    int_t    dummy_retval;

    if (NULL == p_scux_info_ch)
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        core_util_critical_section_enter();

        /* check cancel request */
        if ((p_scux_info_ch->p_tx_aio == p_cancel_aio) || (p_scux_info_ch->p_rx_aio == p_cancel_aio))
        {
            retval = EBUSY_RBSP;
        }
        else
        {
            /* request don't going */
            dummy_retval = ahf_cancel(&p_scux_info_ch->tx_que, p_cancel_aio);
            if (ESUCCESS != dummy_retval)
            {
                /* NON_NOTICE_ASSERT: queue cancel error */
            }
            dummy_retval = ahf_cancel(&p_scux_info_ch->rx_que, p_cancel_aio);
            if (ESUCCESS != dummy_retval)
            {
                /* NON_NOTICE_ASSERT: queue cancel error */
            }
        }

        core_util_critical_section_exit();
    }

    return retval;
}
#if(1) /* mbed */
#if defined (__CC_ARM)
#pragma O3
#endif
#endif /* end mbed */

/******************************************************************************
End of function SCUX_CopyCancelSpecific
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_DirectCancelSpecific
* @brief         Cancel specific request(SSIF direct route).
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @param[in]     *p_cancel_aio : Cancel request information.
* @retval        ESUCCESS : Operation successful.
*                EBUSY_RBSP  : Cancel requst is on going.
*                EFAULT_RBSP : Internal error is occured.
******************************************************************************/

int_t SCUX_DirectCancelSpecific(scux_info_ch_t * const p_scux_info_ch, AIOCB * const p_cancel_aio)
{
    int_t    retval = ESUCCESS;
    int_t    dummy_retval;

    if (NULL == p_scux_info_ch)
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        core_util_critical_section_enter();

        /* check cancel request */
        if ((p_scux_info_ch->p_tx_aio == p_cancel_aio) || (p_scux_info_ch->p_tx_next_aio == p_cancel_aio))
        {
            retval = EBUSY_RBSP;
        }
        else
        {
            /* request don't going */
            dummy_retval = ahf_cancel(&p_scux_info_ch->tx_que, p_cancel_aio);
            if (ESUCCESS != dummy_retval)
            {
                /* NON_NOTICE_ASSERT: queue cancel error */
            }
        }

        core_util_critical_section_exit();
    }

    return retval;
}

/******************************************************************************
End of function SCUX_DirectCancelSpecific
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_CopyCancelAll
* @brief         Cancel all request(mem->mem).
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        ESUCCESS : Operation successful.
*                EFAULT_RBSP : Internal error is occured.
******************************************************************************/

#if(1) /* mbed */
#if defined (__CC_ARM)
#pragma O0
#endif
#endif /* end mbed */
int_t SCUX_CopyCancelAll(scux_info_ch_t * const p_scux_info_ch)
{
    int_t    retval = ESUCCESS;
#if(1) /* mbed */
    int32_t  dma_ercd;
#else
    int_t    dma_ercd;
#endif
    int_t    dma_retval;
    uint32_t tx_remain_size = 0;
    uint32_t rx_remain_size = 0;

    if (NULL == p_scux_info_ch)
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        core_util_critical_section_enter();

        dma_retval = R_DMA_Cancel(p_scux_info_ch->dma_tx_ch, &tx_remain_size, &dma_ercd);
        /* DMA stop check, (when dma_ercd is EBADF_RBSP, DMA stopped already) */
        if ((ESUCCESS != dma_retval) && (EBADF_RBSP != dma_ercd))
        {
            retval = EFAULT_RBSP;
        }
        else
        {
            p_scux_info_ch->tx_fifo_total_size += p_scux_info_ch->dma_tx_current_size;
            p_scux_info_ch->dma_tx_current_size = 0;
        }

        dma_retval = R_DMA_Cancel(p_scux_info_ch->dma_rx_ch, &rx_remain_size, &dma_ercd);
        /* DMA stop check, (when dma_ercd is EBADF_RBSP, DMA stopped already) */
        if ((ESUCCESS != dma_retval) && (EBADF_RBSP != dma_ercd))
        {
            retval = EFAULT_RBSP;
        }
        else
        {
            p_scux_info_ch->rx_fifo_total_size += p_scux_info_ch->dma_rx_current_size;
            p_scux_info_ch->dma_rx_current_size = 0;
        }

        if (ESUCCESS == retval)
        {
            /* return write request */
            if (NULL !=  p_scux_info_ch->p_tx_aio)
            {
                p_scux_info_ch->p_tx_aio->aio_return = ECANCELED_RBSP;
                ahf_complete(&p_scux_info_ch->tx_que, p_scux_info_ch->p_tx_aio);
            }

            /* return read request */
            if (NULL != p_scux_info_ch->p_rx_aio)
            {
                p_scux_info_ch->p_rx_aio->aio_return = ECANCELED_RBSP;
                ahf_complete(&p_scux_info_ch->rx_que, p_scux_info_ch->p_rx_aio);
            }

            ahf_cancelall(&p_scux_info_ch->tx_que);
            ahf_cancelall(&p_scux_info_ch->rx_que);

            p_scux_info_ch->p_tx_aio = NULL;
            p_scux_info_ch->p_rx_aio = NULL;

            SCUX_AdjustAccessFifo(p_scux_info_ch, tx_remain_size, rx_remain_size);
            p_scux_info_ch->cancel_operate_flag = true;

            if (false != p_scux_info_ch->src_cfg.mode_sync)
            {
                SCUX_SyncStopHw(p_scux_info_ch);
                SCUX_InitHw(p_scux_info_ch);
                SCUX_SetupSrc(p_scux_info_ch);
                SCUX_SyncStartHw(p_scux_info_ch);
            }
            else
            {
                SCUX_AsyncStopHw(p_scux_info_ch);
                SCUX_InitHw(p_scux_info_ch);
                SCUX_SetupSrc(p_scux_info_ch);
                SCUX_AsyncStartHw(p_scux_info_ch);
            }

            p_scux_info_ch->ch_stat = SCUX_CH_TRANS_IDLE;
            p_scux_info_ch->p_flush_callback = NULL;

            p_scux_info_ch->tx_fifo_total_size = 0;
            p_scux_info_ch->rx_fifo_total_size = 0;

            p_scux_info_ch->cancel_operate_flag = false;
        }

        core_util_critical_section_exit();
    }

    return retval;
}

/******************************************************************************
End of function SCUX_CopyCancelAll
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_DirectCancelAll
* @brief         Cancel all request(SSIF direct route).
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        ESUCCESS : Operation successful.
*                EFAULT_RBSP : Internal error is occured.
******************************************************************************/

int_t SCUX_DirectCancelAll(scux_info_ch_t * const p_scux_info_ch)
{
    int_t    retval = ESUCCESS;
#if(1) /* mbed */
    int32_t  dma_ercd;
#else
    int_t    dma_ercd;
#endif
    int_t    dma_retval;
    uint32_t tx_remain_size = 0;

    if (NULL == p_scux_info_ch)
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        core_util_critical_section_enter();

        dma_retval = R_DMA_Cancel(p_scux_info_ch->dma_tx_ch, &tx_remain_size, &dma_ercd);
        /* DMA stop check, (when dma_ercd is EBADF_RBSP, DMA stopped already) */
        if ((ESUCCESS != dma_retval) && (EBADF_RBSP != dma_ercd))
        {
            retval = EFAULT_RBSP;
        }
        else
        {
            p_scux_info_ch->tx_fifo_total_size += p_scux_info_ch->dma_tx_current_size;
            p_scux_info_ch->dma_tx_current_size = 0;

            if (NULL != p_scux_info_ch->p_tx_aio)
            {
                p_scux_info_ch->p_tx_aio->aio_return = ECANCELED_RBSP;
                ahf_complete(&p_scux_info_ch->tx_que, p_scux_info_ch->p_tx_aio);
            }
            if (NULL != p_scux_info_ch->p_tx_next_aio)
            {
                p_scux_info_ch->p_tx_next_aio->aio_return = ECANCELED_RBSP;
                ahf_complete(&p_scux_info_ch->tx_que, p_scux_info_ch->p_tx_next_aio);
            }
            ahf_cancelall(&p_scux_info_ch->tx_que);

            p_scux_info_ch->p_tx_aio = NULL;
            p_scux_info_ch->p_tx_next_aio = NULL;

            SCUX_AdjustAccessFifo(p_scux_info_ch, tx_remain_size, 0);

            p_scux_info_ch->cancel_operate_flag = true;
            p_scux_info_ch->restart_ramp_flag = true;

            SCUX_AsyncStopHw(p_scux_info_ch);

            SCUX_InitHw(p_scux_info_ch);
            SCUX_SetupSsif(p_scux_info_ch);
            SCUX_SetupSrc(p_scux_info_ch);
            SCUX_SetupDvu(p_scux_info_ch);
            SCUX_AsyncStartHw(p_scux_info_ch);

            p_scux_info_ch->ch_stat = SCUX_CH_TRANS_IDLE;
            p_scux_info_ch->p_flush_callback = NULL;

            p_scux_info_ch->tx_fifo_total_size = 0;
            p_scux_info_ch->rx_fifo_total_size = 0;
            p_scux_info_ch->dvu_mute_stat = 0;
            p_scux_info_ch->first_ramp_flag = false;
            p_scux_info_ch->cancel_operate_flag = false;
        }

        core_util_critical_section_exit();
    }

    return retval;
}
#if(1) /* mbed */
#if defined (__CC_ARM)
#pragma O3
#endif
#endif /* end mbed */

/******************************************************************************
End of function SCUX_DirectCancelAll
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_AdjustAccessFifo
* @brief         Fifo is Cleared.
*
*                Description:<br>
*
* @param[in]     p_scux_info_ch:channel information.
* @retval        None.
******************************************************************************/

void SCUX_AdjustAccessFifo(scux_info_ch_t * const p_scux_info_ch, const uint32_t tx_remain_size, const uint32_t rx_remain_size)
{
    uint32_t dummy_data_size;
    uint32_t request_data_size;
    uint32_t access_size = SCUX_DMA_UNIT_SIZE16;
    uint32_t dummy_read_data;

    if (NULL == p_scux_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        *(p_scux_info_ch->p_scux_reg->dmacr_cim) &= ~(DMACR_CIM_DMAMDFFD_N_SET << p_scux_info_ch->channel);
        switch (p_scux_info_ch->src_cfg.word_len)
        {
            case SCUX_DATA_LEN_24 :
                access_size = SCUX_DMA_UNIT_SIZE24;
            break;

            case SCUX_DATA_LEN_16 :
                /* fall through */
            case SCUX_DATA_LEN_16_TO_24 :
                access_size = SCUX_DMA_UNIT_SIZE16;
            break;

            default :
                /* ->IPA R3.5.2 Nothing is being processed intentionally. */
                /* <-IPA R3.5.2 */
                /* NOTREACHED on At the time of a normal performance */
            break;
        }

        p_scux_info_ch->p_scux_reg->p_ffd_reg->DEVMR_FFD0_0 &= ~(DEVMR_FFD0_DEVMUF_SET |
                                                                 DEVMR_FFD0_DEVMOF_SET |
                                                                 DEVMR_FFD0_DEVMOL_SET |
                                                                 DEVMR_FFD0_DEVMIUF_SET);
        /* access count is adjusted for tx DMA remain size */
        if (0U != tx_remain_size)
        {
            for (dummy_data_size = 0; dummy_data_size < tx_remain_size; dummy_data_size += access_size )
            {
                *(p_scux_info_ch->p_scux_reg->dmatd_n_cim) = 0;
            }
        }

        /* access count is adjusted for tx total remain size */
        if (0U != (p_scux_info_ch->tx_fifo_total_size & (p_scux_info_ch->fifo_size - 1U)))
        {
            request_data_size = (p_scux_info_ch->fifo_size - (p_scux_info_ch->tx_fifo_total_size & (p_scux_info_ch->fifo_size - 1U)));
            for (dummy_data_size = 0; dummy_data_size < request_data_size; dummy_data_size += access_size )
            {
                *(p_scux_info_ch->p_scux_reg->dmatd_n_cim) = 0;
            }
        }
        p_scux_info_ch->p_scux_reg->p_ffd_reg->DEVCR_FFD0_0 = 0;
        *(p_scux_info_ch->p_scux_reg->dmacr_cim) |= (DMACR_CIM_DMAMDFFD_N_SET << p_scux_info_ch->channel);

        if (SCUX_ROUTE_MEM_TO_MEM == (p_scux_info_ch->route_set & SCUX_GET_ROUTE_MASK))
        {
            *(p_scux_info_ch->p_scux_reg->dmacr_cim) &= ~(DMACR_CIM_DMAMDFFU_N_SET << p_scux_info_ch->channel);
            switch (p_scux_info_ch->src_cfg.word_len)
            {
                case SCUX_DATA_LEN_16 :
                    access_size = SCUX_DMA_UNIT_SIZE16;
                break;

                case SCUX_DATA_LEN_24 :
                    /* fall through */
                case SCUX_DATA_LEN_16_TO_24 :
                    access_size = SCUX_DMA_UNIT_SIZE24;
                break;

                default :
                    /* ->IPA R3.5.2 Nothing is being processed intentionally. */
                    /* <-IPA R3.5.2 */
                    /* NOTREACHED on At the time of a normal performance */
                break;
            }

            p_scux_info_ch->p_scux_reg->p_ffu_reg->UEVMR_FFU0_0 &= ~(UEVMR_FFU0_UEVMUF_SET |
                                                                     UEVMR_FFU0_UEVMOF_SET |
                                                                     UEVMR_FFU0_UEVMOL_SET);
            /* access count is adjusted for rx DMA remain size */
            if (0U != rx_remain_size)
            {
                for (dummy_data_size = 0; dummy_data_size < rx_remain_size; dummy_data_size += access_size )
                {
                    dummy_read_data = *(p_scux_info_ch->p_scux_reg->dmatu_n_cim);
                    UNUSED_ARG(dummy_read_data);
                }
            }

            /* access count is adjusted for rx total remain size */
            if (0U != (p_scux_info_ch->rx_fifo_total_size & (p_scux_info_ch->fifo_size - 1U)))
            {
                request_data_size = (p_scux_info_ch->fifo_size - (p_scux_info_ch->rx_fifo_total_size & (p_scux_info_ch->fifo_size - 1U)));

                for (dummy_data_size = 0; dummy_data_size < request_data_size; dummy_data_size += access_size )
                {
                    dummy_read_data = *(p_scux_info_ch->p_scux_reg->dmatu_n_cim);
                    UNUSED_ARG(dummy_read_data);
                }
            }
            p_scux_info_ch->p_scux_reg->p_ffu_reg->UEVCR_FFU0_0 = 0;
            *(p_scux_info_ch->p_scux_reg->dmacr_cim) |= (DMACR_CIM_DMAMDFFU_N_SET << p_scux_info_ch->channel);
        }
    }

    return;
}

/******************************************************************************
End of function SCUX_AdjustAccessFifo
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_SetupSrc
* @brief         SRC HW setup.
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        None.
******************************************************************************/

void SCUX_SetupSrc(scux_info_ch_t * const p_scux_info_ch)
{
    uint32_t process_delay = 0;
    uint32_t data_size;
    scux_info_drv_t * const p_info_drv = SCUX_GetDrvInstance();

    if ((NULL == p_info_drv) || (NULL == p_scux_info_ch))
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        if (false != p_scux_info_ch->src_cfg.src_enable)
        {
            /* check delay mode and enabled audio channel */
            switch (p_scux_info_ch->src_cfg.delay_mode)
            {
                case SCUX_DELAY_NORMAL :
                    switch (p_scux_info_ch->src_cfg.use_ch)
                    {
                        case SCUX_USE_CH_1 :
                            process_delay = SCUX_PROCESS_DELAY_NORMAL_CH1;
                        break;

                        case SCUX_USE_CH_2 :
                            process_delay = SCUX_PROCESS_DELAY_NORMAL_CH2;
                        break;

                        case SCUX_USE_CH_4 :
                            process_delay = SCUX_PROCESS_DELAY_NORMAL_CH4;
                        break;

                        case SCUX_USE_CH_6 :
                            process_delay = SCUX_PROCESS_DELAY_NORMAL_CH6;
                        break;

                        case SCUX_USE_CH_8 :
                            process_delay = SCUX_PROCESS_DELAY_NORMAL_CH8;
                        break;

                        default :
                            /* ->IPA R3.5.2 Nothing is being processed intentionally. */
                            /* <-IPA R3.5.2 */
                            /* NOTREACHED on At the time of a normal performance */
                        break;
                    }
                break;

                case SCUX_DELAY_LOW_DELAY1 :
                    process_delay = SCUX_PROCESS_DELAY_1_CH1_2;
                break;

                case SCUX_DELAY_LOW_DELAY2 :
                    process_delay = SCUX_PROCESS_DELAY_2_CH1_2;
                break;

                default :
                    /* ->IPA R3.5.2 Nothing is being processed intentionally. */
                    /* <-IPA R3.5.2 */
                    /* NOTREACHED on At the time of a normal performance */
                break;
            }

            /* set dummy data size for flush */
            if (SCUX_DATA_LEN_16 == p_scux_info_ch->src_cfg.word_len)
            {
                data_size = sizeof(uint16_t);
            }
            else
            {
                data_size = sizeof(uint32_t);
            }

            process_delay = (process_delay * ((p_scux_info_ch->output_rate * SCUX_RATE_INT_CONV_VALUE) / p_scux_info_ch->input_rate));
            if (0U != (process_delay % SCUX_RATE_INT_CONV_VALUE))
            {
                /* A redundant value is added that delay size below a decimal point may come out */
                process_delay = ((process_delay / SCUX_RATE_INT_CONV_VALUE) + SCUX_ADJUST_REST_VALUE);
            }
            else
            {
                process_delay = (process_delay / SCUX_RATE_INT_CONV_VALUE);
            }

            p_scux_info_ch->flush_stop_size = (p_scux_info_ch->src_cfg.use_ch * (data_size * (process_delay + SCUX_LOGIC_DELAY_BYPASS_OFF)));
            if (0U != (p_scux_info_ch->flush_stop_size % SCUX_EVEN_VALUE_DIV))
            {
                /* set even value */
                p_scux_info_ch->flush_stop_size++;
            }
        }
        else
        {
            /* set dummy data size for flush */
            if (SCUX_DATA_LEN_16 == p_scux_info_ch->src_cfg.word_len)
            {
                data_size = sizeof(uint16_t);
            }
            else
            {
                data_size = sizeof(uint32_t);
            }

            p_scux_info_ch->flush_stop_size = (p_scux_info_ch->src_cfg.use_ch * (data_size * SCUX_LOGIC_DELAY_BYPASS_ON));
            if (0U != (p_scux_info_ch->flush_stop_size % SCUX_EVEN_VALUE_DIV))
            {
                /* set even value */
                p_scux_info_ch->flush_stop_size++;
            }
        }

        p_scux_info_ch->tx_dummy_run_flag = false;

        /* set FFD & FFU parameter depend on the combination of route and sync mode */
        if (SCUX_ROUTE_MEM_TO_MEM == (p_scux_info_ch->route_set & SCUX_GET_ROUTE_MASK))
        {
            if (false != p_scux_info_ch->src_cfg.mode_sync)
            {
                p_scux_info_ch->p_scux_reg->p_ffd_reg->FFDPR_FFD0_0 = FFDPR_FFD0_PASS_SET_SYNC;
                p_scux_info_ch->p_scux_reg->p_ffu_reg->FFUPR_FFU0_0 = FFUPR_FFU0_PASS_SET_SYNC;
                p_scux_info_ch->p_scux_reg->p_ipc_reg->IPSLR_IPC0_0 = IPSLR_IPC_PASS_SEL_SET_SYNC;
                p_scux_info_ch->p_scux_reg->p_opc_reg->OPSLR_OPC0_0 = OPSLR_OPC_PASS_SEL_SET_SYNC;
            }
            else
            {
                p_scux_info_ch->p_scux_reg->p_ffd_reg->FFDPR_FFD0_0 = FFDPR_FFD0_PASS_SET_ASYNC;
                p_scux_info_ch->p_scux_reg->p_ffu_reg->FFUPR_FFU0_0 = FFUPR_FFU0_PASS_SET_ASYNC;
                p_scux_info_ch->p_scux_reg->p_ipc_reg->IPSLR_IPC0_0 = IPSLR_IPC_PASS_SEL_SET_ASYNC;
                p_scux_info_ch->p_scux_reg->p_opc_reg->OPSLR_OPC0_0 = OPSLR_OPC_PASS_SEL_SET_ASYNC;
            }
        }
        else
        {
            p_scux_info_ch->p_scux_reg->p_ffd_reg->FFDPR_FFD0_0 = FFDPR_FFD0_PASS_SET_ASYNC;
            p_scux_info_ch->p_scux_reg->p_ffu_reg->FFUPR_FFU0_0 = 0;
            p_scux_info_ch->p_scux_reg->p_ipc_reg->IPSLR_IPC0_0 = IPSLR_IPC_PASS_SEL_SET_ASYNC;
            p_scux_info_ch->p_scux_reg->p_opc_reg->OPSLR_OPC0_0 = OPSLR_OPC_PASS_SEL_SET_DIRECT;
        }

        /* set route CIM register for SSIF channel and MIX */
        switch (p_scux_info_ch->route_set)
        {
            case SCUX_ROUTE_SRC0_MEM :
                /* fall through */
            case SCUX_ROUTE_SRC1_MEM :
                /* fall through */
            case SCUX_ROUTE_SRC2_MEM :
                /* fall through */
            case SCUX_ROUTE_SRC3_MEM :
                /* do nothing, when mem to mem route is setting */
            break;

            case SCUX_ROUTE_SRC0_SSIF0 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~SSIRSEL_CIM_SOSEL0_MASK;
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= SSIRSEL_CIM_SOSEL0_SRC0_SET;
            break;

            case SCUX_ROUTE_SRC0_SSIF012 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~(SSIRSEL_CIM_SOSEL0_MASK | SSIRSEL_CIM_SOSEL1_MASK | SSIRSEL_CIM_SOSEL2_MASK);
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= (SSIRSEL_CIM_SOSEL0_SRC0_SET | SSIRSEL_CIM_SOSEL1_SRC0_SET | SSIRSEL_CIM_SOSEL2_SRC0_SET);
            break;

            case SCUX_ROUTE_SRC0_SSIF3 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~SSIRSEL_CIM_SOSEL3_MASK;
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= SSIRSEL_CIM_SOSEL3_SRC0_SET;
            break;

#if defined(TARGET_RZA1H) /* mbed */
            case SCUX_ROUTE_SRC0_SSIF345 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~(SSIRSEL_CIM_SOSEL3_MASK | SSIRSEL_CIM_SOSEL4_MASK | SSIRSEL_CIM_SOSEL5_MASK);
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= (SSIRSEL_CIM_SOSEL3_SRC0_SET | SSIRSEL_CIM_SOSEL4_SRC0_SET | SSIRSEL_CIM_SOSEL5_SRC0_SET);
            break;

#endif /* mbed */
            case SCUX_ROUTE_SRC1_SSIF0 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~SSIRSEL_CIM_SOSEL0_MASK;
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= SSIRSEL_CIM_SOSEL0_SRC1_SET;
            break;

            case SCUX_ROUTE_SRC1_SSIF012 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~(SSIRSEL_CIM_SOSEL0_MASK | SSIRSEL_CIM_SOSEL1_MASK | SSIRSEL_CIM_SOSEL2_MASK);
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= (SSIRSEL_CIM_SOSEL0_SRC1_SET | SSIRSEL_CIM_SOSEL1_SRC1_SET | SSIRSEL_CIM_SOSEL2_SRC1_SET);
            break;

            case SCUX_ROUTE_SRC1_SSIF3 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~SSIRSEL_CIM_SOSEL3_MASK;
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= SSIRSEL_CIM_SOSEL3_SRC1_SET;
            break;

#if defined(TARGET_RZA1H) /* mbed */
            case SCUX_ROUTE_SRC1_SSIF345 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~(SSIRSEL_CIM_SOSEL3_MASK | SSIRSEL_CIM_SOSEL4_MASK | SSIRSEL_CIM_SOSEL5_MASK);
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= (SSIRSEL_CIM_SOSEL3_SRC1_SET | SSIRSEL_CIM_SOSEL4_SRC1_SET | SSIRSEL_CIM_SOSEL5_SRC1_SET);
            break;

#endif /* mbed */
            case SCUX_ROUTE_SRC2_SSIF1 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~SSIRSEL_CIM_SOSEL1_MASK;
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= SSIRSEL_CIM_SOSEL1_SRC2_SET;
            break;

#if defined(TARGET_RZA1H) /* mbed */
            case SCUX_ROUTE_SRC2_SSIF4 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~SSIRSEL_CIM_SOSEL4_MASK;
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= SSIRSEL_CIM_SOSEL4_SRC2_SET;
            break;

#endif /* mbed */
            case SCUX_ROUTE_SRC3_SSIF2 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~SSIRSEL_CIM_SOSEL2_MASK;
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= SSIRSEL_CIM_SOSEL2_SRC3_SET;
            break;

#if defined(TARGET_RZA1H) /* mbed */
            case SCUX_ROUTE_SRC3_SSIF5 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~SSIRSEL_CIM_SOSEL5_MASK;
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= SSIRSEL_CIM_SOSEL5_SRC3_SET;
            break;

#endif /* mbed */
            case SCUX_ROUTE_SRC0_MIX_SSIF0 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~SSIRSEL_CIM_SOSEL0_MASK;
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= SSIRSEL_CIM_SOSEL0_MIX_SET;
            break;

            case SCUX_ROUTE_SRC0_MIX_SSIF012 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~(SSIRSEL_CIM_SOSEL0_MASK | SSIRSEL_CIM_SOSEL1_MASK | SSIRSEL_CIM_SOSEL2_MASK);
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= (SSIRSEL_CIM_SOSEL0_MIX_SET | SSIRSEL_CIM_SOSEL1_MIX_SET | SSIRSEL_CIM_SOSEL2_MIX_SET);
            break;

            case SCUX_ROUTE_SRC0_MIX_SSIF3 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~SSIRSEL_CIM_SOSEL3_MASK;
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= SSIRSEL_CIM_SOSEL3_MIX_SET;
            break;

#if defined(TARGET_RZA1H) /* mbed */
            case SCUX_ROUTE_SRC0_MIX_SSIF345 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~(SSIRSEL_CIM_SOSEL3_MASK | SSIRSEL_CIM_SOSEL4_MASK | SSIRSEL_CIM_SOSEL5_MASK);
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= (SSIRSEL_CIM_SOSEL3_MIX_SET | SSIRSEL_CIM_SOSEL4_MIX_SET | SSIRSEL_CIM_SOSEL5_MIX_SET);
            break;

#endif /* mbed */
            case SCUX_ROUTE_SRC1_MIX_SSIF0 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~SSIRSEL_CIM_SOSEL0_MASK;
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= SSIRSEL_CIM_SOSEL0_MIX_SET;
            break;

            case SCUX_ROUTE_SRC1_MIX_SSIF012 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~(SSIRSEL_CIM_SOSEL0_MASK | SSIRSEL_CIM_SOSEL1_MASK | SSIRSEL_CIM_SOSEL2_MASK);
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= (SSIRSEL_CIM_SOSEL0_MIX_SET | SSIRSEL_CIM_SOSEL1_MIX_SET | SSIRSEL_CIM_SOSEL2_MIX_SET);
            break;

            case SCUX_ROUTE_SRC1_MIX_SSIF3 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~SSIRSEL_CIM_SOSEL3_MASK;
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= SSIRSEL_CIM_SOSEL3_MIX_SET;
            break;

#if defined(TARGET_RZA1H) /* mbed */
            case SCUX_ROUTE_SRC1_MIX_SSIF345 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~(SSIRSEL_CIM_SOSEL3_MASK | SSIRSEL_CIM_SOSEL4_MASK | SSIRSEL_CIM_SOSEL5_MASK);
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= (SSIRSEL_CIM_SOSEL3_MIX_SET | SSIRSEL_CIM_SOSEL4_MIX_SET | SSIRSEL_CIM_SOSEL5_MIX_SET);
            break;

#endif /* mbed */
            case SCUX_ROUTE_SRC2_MIX_SSIF0 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~SSIRSEL_CIM_SOSEL0_MASK;
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= SSIRSEL_CIM_SOSEL0_MIX_SET;
            break;

            case SCUX_ROUTE_SRC2_MIX_SSIF012 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~(SSIRSEL_CIM_SOSEL0_MASK | SSIRSEL_CIM_SOSEL1_MASK | SSIRSEL_CIM_SOSEL2_MASK);
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= (SSIRSEL_CIM_SOSEL0_MIX_SET | SSIRSEL_CIM_SOSEL1_MIX_SET | SSIRSEL_CIM_SOSEL2_MIX_SET);
            break;

            case SCUX_ROUTE_SRC2_MIX_SSIF3 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~SSIRSEL_CIM_SOSEL3_MASK;
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= SSIRSEL_CIM_SOSEL3_MIX_SET;
            break;

#if defined(TARGET_RZA1H) /* mbed */
            case SCUX_ROUTE_SRC2_MIX_SSIF345 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~(SSIRSEL_CIM_SOSEL3_MASK | SSIRSEL_CIM_SOSEL4_MASK | SSIRSEL_CIM_SOSEL5_MASK);
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= (SSIRSEL_CIM_SOSEL3_MIX_SET | SSIRSEL_CIM_SOSEL4_MIX_SET | SSIRSEL_CIM_SOSEL5_MIX_SET);
            break;

#endif /* mbed */
            case SCUX_ROUTE_SRC3_MIX_SSIF0 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~SSIRSEL_CIM_SOSEL0_MASK;
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= SSIRSEL_CIM_SOSEL0_MIX_SET;
            break;

            case SCUX_ROUTE_SRC3_MIX_SSIF012 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~(SSIRSEL_CIM_SOSEL0_MASK | SSIRSEL_CIM_SOSEL1_MASK | SSIRSEL_CIM_SOSEL2_MASK);
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= (SSIRSEL_CIM_SOSEL0_MIX_SET | SSIRSEL_CIM_SOSEL1_MIX_SET | SSIRSEL_CIM_SOSEL2_MIX_SET);
            break;

            case SCUX_ROUTE_SRC3_MIX_SSIF3 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~SSIRSEL_CIM_SOSEL3_MASK;
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= SSIRSEL_CIM_SOSEL3_MIX_SET;
            break;

#if defined(TARGET_RZA1H) /* mbed */
            case SCUX_ROUTE_SRC3_MIX_SSIF345 :
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) &= ~(SSIRSEL_CIM_SOSEL3_MASK | SSIRSEL_CIM_SOSEL4_MASK | SSIRSEL_CIM_SOSEL5_MASK);
                *(p_scux_info_ch->p_scux_reg->ssirsel_cim) |= (SSIRSEL_CIM_SOSEL3_MIX_SET | SSIRSEL_CIM_SOSEL4_MIX_SET | SSIRSEL_CIM_SOSEL5_MIX_SET);
            break;

#endif /* mbed */
            default :
                /* ->IPA R3.5.2 Nothing is being processed intentionally. */
                /* <-IPA R3.5.2 */
                 /* NOTREACHED on At the time of a normal performance */
            break;
        }

        SCUX_SetupDataPosition(p_scux_info_ch);

        /* set route SSICTRL register for each route */
        if (false == p_scux_info_ch->src_cfg.mode_sync)
        {
            switch (p_scux_info_ch->route_set)
            {
                case SCUX_ROUTE_SRC0_MEM :
                    /* fall through */
                case SCUX_ROUTE_SRC1_MEM :
                    /* fall through */
                case SCUX_ROUTE_SRC2_MEM :
                    /* fall through */
                case SCUX_ROUTE_SRC3_MEM :
                    /* do nothing, when mem to mem route is setting */
                break;

                case SCUX_ROUTE_SRC0_SSIF0 :
                    /* fall through */
                case SCUX_ROUTE_SRC0_SSIF012 :
                    p_info_drv->shared_info.ssictrl_cim_value |= SSICTRL_CIM_SSI0TX_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                break;

                case SCUX_ROUTE_SRC0_SSIF3 :
#if defined(TARGET_RZA1H) /* mbed */
                    /* fall through */
                case SCUX_ROUTE_SRC0_SSIF345 :
#endif /* mbed */
                    p_info_drv->shared_info.ssictrl_cim_value |= SSICTRL_CIM_SSI3TX_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                break;

                case SCUX_ROUTE_SRC1_SSIF0 :
                    /* fall through */
                case SCUX_ROUTE_SRC1_SSIF012 :
                    p_info_drv->shared_info.ssictrl_cim_value |= SSICTRL_CIM_SSI0TX_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                break;

                case SCUX_ROUTE_SRC1_SSIF3 :
#if defined(TARGET_RZA1H) /* mbed */
                    /* fall through */
                case SCUX_ROUTE_SRC1_SSIF345 :
#endif /* mbed */
                    p_info_drv->shared_info.ssictrl_cim_value |= SSICTRL_CIM_SSI3TX_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                break;

                case SCUX_ROUTE_SRC2_SSIF1 :
                    /* fall through */
                    p_info_drv->shared_info.ssictrl_cim_value |= SSICTRL_CIM_SSI1TX_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                break;

#if defined(TARGET_RZA1H) /* mbed */
                case SCUX_ROUTE_SRC2_SSIF4 :
                    /* fall through */
                    p_info_drv->shared_info.ssictrl_cim_value |= SSICTRL_CIM_SSI4TX_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                break;

#endif /* mbed */
                case SCUX_ROUTE_SRC3_SSIF2 :
                    /* fall through */
                    p_info_drv->shared_info.ssictrl_cim_value |= SSICTRL_CIM_SSI2TX_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                break;

#if defined(TARGET_RZA1H) /* mbed */
                case SCUX_ROUTE_SRC3_SSIF5 :
                    /* fall through */
                    p_info_drv->shared_info.ssictrl_cim_value |= SSICTRL_CIM_SSI5TX_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                break;

#endif /* mbed */
                case SCUX_ROUTE_SRC0_MIX_SSIF0 :
                    /* fall through */
                case SCUX_ROUTE_SRC0_MIX_SSIF012 :
                    p_info_drv->shared_info.ssictrl_cim_value |= SSICTRL_CIM_SSI0TX_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                break;

                case SCUX_ROUTE_SRC0_MIX_SSIF3 :
#if defined(TARGET_RZA1H) /* mbed */
                    /* fall through */
                case SCUX_ROUTE_SRC0_MIX_SSIF345 :
#endif /* mbed */
                    p_info_drv->shared_info.ssictrl_cim_value |= SSICTRL_CIM_SSI3TX_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                break;

                case SCUX_ROUTE_SRC1_MIX_SSIF0 :
                    /* fall through */
                case SCUX_ROUTE_SRC1_MIX_SSIF012 :
                    p_info_drv->shared_info.ssictrl_cim_value |= SSICTRL_CIM_SSI0TX_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                break;

                case SCUX_ROUTE_SRC1_MIX_SSIF3 :
#if defined(TARGET_RZA1H) /* mbed */
                    /* fall through */
                case SCUX_ROUTE_SRC1_MIX_SSIF345 :
#endif /* mbed */
                    p_info_drv->shared_info.ssictrl_cim_value |= SSICTRL_CIM_SSI3TX_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                break;

                case SCUX_ROUTE_SRC2_MIX_SSIF0 :
                    /* fall through */
                case SCUX_ROUTE_SRC2_MIX_SSIF012 :
                    p_info_drv->shared_info.ssictrl_cim_value |= SSICTRL_CIM_SSI0TX_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                break;

                case SCUX_ROUTE_SRC2_MIX_SSIF3 :
#if defined(TARGET_RZA1H) /* mbed */
                    /* fall through */
                case SCUX_ROUTE_SRC2_MIX_SSIF345 :
#endif /* mbed */
                    p_info_drv->shared_info.ssictrl_cim_value |= SSICTRL_CIM_SSI3TX_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                break;

                case SCUX_ROUTE_SRC3_MIX_SSIF0 :
                    /* fall through */
                case SCUX_ROUTE_SRC3_MIX_SSIF012 :
                    p_info_drv->shared_info.ssictrl_cim_value |= SSICTRL_CIM_SSI0TX_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                break;

                case SCUX_ROUTE_SRC3_MIX_SSIF3 :
#if defined(TARGET_RZA1H) /* mbed */
                    /* fall through */
                case SCUX_ROUTE_SRC3_MIX_SSIF345 :
#endif /* mbed */
                    p_info_drv->shared_info.ssictrl_cim_value |= SSICTRL_CIM_SSI3TX_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                break;

                default :
                    /* ->IPA R3.5.2 Nothing is being processed intentionally. */
                    /* <-IPA R3.5.2 */
                    /* NOTREACHED on At the time of a normal performance */
                break;
            }
        }

        SCUX_SetupSrcClk(p_scux_info_ch);

        /* set pin mode for each route */
        switch (p_scux_info_ch->route_set)
        {
            case SCUX_ROUTE_SRC0_MEM :
                /* fall through */
            case SCUX_ROUTE_SRC1_MEM :
                /* fall through */
            case SCUX_ROUTE_SRC2_MEM :
                /* fall through */
            case SCUX_ROUTE_SRC3_MEM :
#if defined(TARGET_RZA1H) /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET | SSIPMD_CIM_SSI345EN_SET);
#else /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET);
#endif /* mbed */
            break;

            case SCUX_ROUTE_SRC0_SSIF0 :
#if defined(TARGET_RZA1H) /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET | SSIPMD_CIM_SSI345EN_SET);
#else /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET);
#endif /* mbed */
            break;

            case SCUX_ROUTE_SRC0_SSIF012 :
#if defined(TARGET_RZA1H) /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~SSIPMD_CIM_SSI345EN_SET;
#endif /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= SSIPMD_CIM_SSI012EN_SET;
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI1PMD_MASK | SSIPMD_CIM_SSI2PMD_MASK);
                if (false != p_scux_info_ch->p_ssif_info1->ssif_cfg.mode_master)
                {
                    *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= ((SCUX_PIN_MODE_MASTER_SLAVE << SSIPMD_CIM_SSI1PMD_SHIFT) |
                                                                  (SCUX_PIN_MODE_MASTER_SLAVE << SSIPMD_CIM_SSI2PMD_SHIFT));
                    p_scux_info_ch->p_ssif_info2->pin_mode_backup = p_scux_info_ch->p_ssif_info2->pin_mode;
                    p_scux_info_ch->p_ssif_info2->pin_mode = SCUX_PIN_MODE_MASTER_SLAVE;
                    p_scux_info_ch->p_ssif_info3->pin_mode_backup = p_scux_info_ch->p_ssif_info3->pin_mode;
                    p_scux_info_ch->p_ssif_info3->pin_mode = SCUX_PIN_MODE_MASTER_SLAVE;
                }
                else
                {
                    *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= ((SCUX_PIN_MODE_SLAVE_SLAVE << SSIPMD_CIM_SSI1PMD_SHIFT) |
                                                                  (SCUX_PIN_MODE_SLAVE_SLAVE << SSIPMD_CIM_SSI2PMD_SHIFT));
                    p_scux_info_ch->p_ssif_info2->pin_mode_backup = p_scux_info_ch->p_ssif_info2->pin_mode;
                    p_scux_info_ch->p_ssif_info2->pin_mode = SCUX_PIN_MODE_SLAVE_SLAVE;
                    p_scux_info_ch->p_ssif_info3->pin_mode_backup = p_scux_info_ch->p_ssif_info3->pin_mode;
                    p_scux_info_ch->p_ssif_info3->pin_mode = SCUX_PIN_MODE_SLAVE_SLAVE;
                }
            break;

            case SCUX_ROUTE_SRC0_SSIF3 :
#if defined(TARGET_RZA1H) /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET | SSIPMD_CIM_SSI345EN_SET);
#else /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET);
#endif /* mbed */
            break;

#if defined(TARGET_RZA1H) /* mbed */
            case SCUX_ROUTE_SRC0_SSIF345 :
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~SSIPMD_CIM_SSI012EN_SET;
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= SSIPMD_CIM_SSI345EN_SET;
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI4PMD_MASK | SSIPMD_CIM_SSI5PMD_MASK);
                if (false != p_scux_info_ch->p_ssif_info1->ssif_cfg.mode_master)
                {
                    *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= ((SCUX_PIN_MODE_MASTER_SLAVE << SSIPMD_CIM_SSI4PMD_SHIFT) |
                                                                  (SCUX_PIN_MODE_MASTER_SLAVE << SSIPMD_CIM_SSI5PMD_SHIFT));
                    p_scux_info_ch->p_ssif_info2->pin_mode_backup = p_scux_info_ch->p_ssif_info2->pin_mode;
                    p_scux_info_ch->p_ssif_info2->pin_mode = SCUX_PIN_MODE_MASTER_SLAVE;
                    p_scux_info_ch->p_ssif_info3->pin_mode_backup = p_scux_info_ch->p_ssif_info3->pin_mode;
                    p_scux_info_ch->p_ssif_info3->pin_mode = SCUX_PIN_MODE_MASTER_SLAVE;                                                                  
                }
                else
                {
                    *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= ((SCUX_PIN_MODE_SLAVE_SLAVE << SSIPMD_CIM_SSI4PMD_SHIFT) |
                                                                  (SCUX_PIN_MODE_SLAVE_SLAVE << SSIPMD_CIM_SSI5PMD_SHIFT));
                    p_scux_info_ch->p_ssif_info2->pin_mode_backup = p_scux_info_ch->p_ssif_info2->pin_mode;
                    p_scux_info_ch->p_ssif_info2->pin_mode = SCUX_PIN_MODE_SLAVE_SLAVE;
                    p_scux_info_ch->p_ssif_info3->pin_mode_backup = p_scux_info_ch->p_ssif_info3->pin_mode;
                    p_scux_info_ch->p_ssif_info3->pin_mode = SCUX_PIN_MODE_SLAVE_SLAVE;                                                                  
                }
            break;

#endif /* mbed */
            case SCUX_ROUTE_SRC1_SSIF0 :
#if defined(TARGET_RZA1H) /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET | SSIPMD_CIM_SSI345EN_SET);
#else /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET);
#endif /* mbed */
            break;

            case SCUX_ROUTE_SRC1_SSIF012 :
#if defined(TARGET_RZA1H) /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~SSIPMD_CIM_SSI345EN_SET;
#endif /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= SSIPMD_CIM_SSI012EN_SET;
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI1PMD_MASK | SSIPMD_CIM_SSI2PMD_MASK);
                if (false != p_scux_info_ch->p_ssif_info1->ssif_cfg.mode_master)
                {
                    *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= ((SCUX_PIN_MODE_MASTER_SLAVE << SSIPMD_CIM_SSI1PMD_SHIFT) |
                                                                  (SCUX_PIN_MODE_MASTER_SLAVE << SSIPMD_CIM_SSI2PMD_SHIFT));
                    p_scux_info_ch->p_ssif_info2->pin_mode_backup = p_scux_info_ch->p_ssif_info2->pin_mode;
                    p_scux_info_ch->p_ssif_info2->pin_mode = SCUX_PIN_MODE_MASTER_SLAVE;
                    p_scux_info_ch->p_ssif_info3->pin_mode_backup = p_scux_info_ch->p_ssif_info3->pin_mode;
                    p_scux_info_ch->p_ssif_info3->pin_mode = SCUX_PIN_MODE_MASTER_SLAVE;
                }
                else
                {
                    *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= ((SCUX_PIN_MODE_SLAVE_SLAVE << SSIPMD_CIM_SSI1PMD_SHIFT) |
                                                                  (SCUX_PIN_MODE_SLAVE_SLAVE << SSIPMD_CIM_SSI2PMD_SHIFT));
                    p_scux_info_ch->p_ssif_info2->pin_mode_backup = p_scux_info_ch->p_ssif_info2->pin_mode;
                    p_scux_info_ch->p_ssif_info2->pin_mode = SCUX_PIN_MODE_SLAVE_SLAVE;
                    p_scux_info_ch->p_ssif_info3->pin_mode_backup = p_scux_info_ch->p_ssif_info3->pin_mode;
                    p_scux_info_ch->p_ssif_info3->pin_mode = SCUX_PIN_MODE_SLAVE_SLAVE;
                }
            break;

            case SCUX_ROUTE_SRC1_SSIF3 :
#if defined(TARGET_RZA1H) /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET | SSIPMD_CIM_SSI345EN_SET);
#else /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET);
#endif /* mbed */
            break;

#if defined(TARGET_RZA1H) /* mbed */
            case SCUX_ROUTE_SRC1_SSIF345 :
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~SSIPMD_CIM_SSI012EN_SET;
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= SSIPMD_CIM_SSI345EN_SET;
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI4PMD_MASK | SSIPMD_CIM_SSI5PMD_MASK);
                if (false != p_scux_info_ch->p_ssif_info1->ssif_cfg.mode_master)
                {
                    *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= ((SCUX_PIN_MODE_MASTER_SLAVE << SSIPMD_CIM_SSI4PMD_SHIFT) |
                                                                  (SCUX_PIN_MODE_MASTER_SLAVE << SSIPMD_CIM_SSI5PMD_SHIFT));
                    p_scux_info_ch->p_ssif_info2->pin_mode_backup = p_scux_info_ch->p_ssif_info2->pin_mode;
                    p_scux_info_ch->p_ssif_info2->pin_mode = SCUX_PIN_MODE_MASTER_SLAVE;
                    p_scux_info_ch->p_ssif_info3->pin_mode_backup = p_scux_info_ch->p_ssif_info3->pin_mode;
                    p_scux_info_ch->p_ssif_info3->pin_mode = SCUX_PIN_MODE_MASTER_SLAVE;                                                                  
                }
                else
                {
                    *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= ((SCUX_PIN_MODE_SLAVE_SLAVE << SSIPMD_CIM_SSI4PMD_SHIFT) |
                                                                  (SCUX_PIN_MODE_SLAVE_SLAVE << SSIPMD_CIM_SSI5PMD_SHIFT));
                    p_scux_info_ch->p_ssif_info2->pin_mode_backup = p_scux_info_ch->p_ssif_info2->pin_mode;
                    p_scux_info_ch->p_ssif_info2->pin_mode = SCUX_PIN_MODE_SLAVE_SLAVE;
                    p_scux_info_ch->p_ssif_info3->pin_mode_backup = p_scux_info_ch->p_ssif_info3->pin_mode;
                    p_scux_info_ch->p_ssif_info3->pin_mode = SCUX_PIN_MODE_SLAVE_SLAVE;                                                                  
                }
            break;

#endif /* mbed */
#if defined(TARGET_RZA1H) /* mbed */
            case SCUX_ROUTE_SRC2_SSIF1 :
                /* fall through */
            case SCUX_ROUTE_SRC2_SSIF4 :
                /* fall through */
            case SCUX_ROUTE_SRC3_SSIF2 :
                /* fall through */
            case SCUX_ROUTE_SRC3_SSIF5 :
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET | SSIPMD_CIM_SSI345EN_SET);
            break;
#else /* mbed */
            case SCUX_ROUTE_SRC2_SSIF1 :
                /* fall through */
            case SCUX_ROUTE_SRC3_SSIF2 :
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET);
            break;
#endif /* mbed */

            case SCUX_ROUTE_SRC0_MIX_SSIF0 :
#if defined(TARGET_RZA1H) /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET | SSIPMD_CIM_SSI345EN_SET);
#else /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET);
#endif /* mbed */
            break;

            case SCUX_ROUTE_SRC0_MIX_SSIF012 :
#if defined(TARGET_RZA1H) /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~SSIPMD_CIM_SSI345EN_SET;
#endif /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= SSIPMD_CIM_SSI012EN_SET;
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI1PMD_MASK | SSIPMD_CIM_SSI2PMD_MASK);
                if (false != p_scux_info_ch->p_ssif_info1->ssif_cfg.mode_master)
                {
                    *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= ((SCUX_PIN_MODE_MASTER_SLAVE << SSIPMD_CIM_SSI1PMD_SHIFT) |
                                                                  (SCUX_PIN_MODE_MASTER_SLAVE << SSIPMD_CIM_SSI2PMD_SHIFT));
                    p_scux_info_ch->p_ssif_info2->pin_mode_backup = p_scux_info_ch->p_ssif_info2->pin_mode;
                    p_scux_info_ch->p_ssif_info2->pin_mode = SCUX_PIN_MODE_MASTER_SLAVE;
                    p_scux_info_ch->p_ssif_info3->pin_mode_backup = p_scux_info_ch->p_ssif_info3->pin_mode;
                    p_scux_info_ch->p_ssif_info3->pin_mode = SCUX_PIN_MODE_MASTER_SLAVE;
                }
                else
                {
                    *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= ((SCUX_PIN_MODE_SLAVE_SLAVE << SSIPMD_CIM_SSI1PMD_SHIFT) |
                                                                  (SCUX_PIN_MODE_SLAVE_SLAVE << SSIPMD_CIM_SSI2PMD_SHIFT));
                    p_scux_info_ch->p_ssif_info2->pin_mode_backup = p_scux_info_ch->p_ssif_info2->pin_mode;
                    p_scux_info_ch->p_ssif_info2->pin_mode = SCUX_PIN_MODE_SLAVE_SLAVE;
                    p_scux_info_ch->p_ssif_info3->pin_mode_backup = p_scux_info_ch->p_ssif_info3->pin_mode;
                    p_scux_info_ch->p_ssif_info3->pin_mode = SCUX_PIN_MODE_SLAVE_SLAVE;
                }
            break;

            case SCUX_ROUTE_SRC0_MIX_SSIF3 :
#if defined(TARGET_RZA1H) /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET | SSIPMD_CIM_SSI345EN_SET);
#else /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET);
#endif /* mbed */
            break;

#if defined(TARGET_RZA1H) /* mbed */
            case SCUX_ROUTE_SRC0_MIX_SSIF345 :
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~SSIPMD_CIM_SSI012EN_SET;
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= SSIPMD_CIM_SSI345EN_SET;
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI4PMD_MASK | SSIPMD_CIM_SSI5PMD_MASK);
                if (false != p_scux_info_ch->p_ssif_info1->ssif_cfg.mode_master)
                {
                    *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= ((SCUX_PIN_MODE_MASTER_SLAVE << SSIPMD_CIM_SSI4PMD_SHIFT) |
                                                                  (SCUX_PIN_MODE_MASTER_SLAVE << SSIPMD_CIM_SSI5PMD_SHIFT));
                    p_scux_info_ch->p_ssif_info2->pin_mode_backup = p_scux_info_ch->p_ssif_info2->pin_mode;
                    p_scux_info_ch->p_ssif_info2->pin_mode = SCUX_PIN_MODE_MASTER_SLAVE;
                    p_scux_info_ch->p_ssif_info3->pin_mode_backup = p_scux_info_ch->p_ssif_info3->pin_mode;
                    p_scux_info_ch->p_ssif_info3->pin_mode = SCUX_PIN_MODE_MASTER_SLAVE;                                                                  
                }
                else
                {
                    *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= ((SCUX_PIN_MODE_SLAVE_SLAVE << SSIPMD_CIM_SSI4PMD_SHIFT) |
                                                                  (SCUX_PIN_MODE_SLAVE_SLAVE << SSIPMD_CIM_SSI5PMD_SHIFT));
                    p_scux_info_ch->p_ssif_info2->pin_mode_backup = p_scux_info_ch->p_ssif_info2->pin_mode;
                    p_scux_info_ch->p_ssif_info2->pin_mode = SCUX_PIN_MODE_SLAVE_SLAVE;
                    p_scux_info_ch->p_ssif_info3->pin_mode_backup = p_scux_info_ch->p_ssif_info3->pin_mode;
                    p_scux_info_ch->p_ssif_info3->pin_mode = SCUX_PIN_MODE_SLAVE_SLAVE;                                                                  
                }
            break;

#endif /* mbed */
            case SCUX_ROUTE_SRC1_MIX_SSIF0 :
#if defined(TARGET_RZA1H) /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET | SSIPMD_CIM_SSI345EN_SET);
#else /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET);
#endif /* mbed */
            break;

            case SCUX_ROUTE_SRC1_MIX_SSIF012 :
#if defined(TARGET_RZA1H) /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~SSIPMD_CIM_SSI345EN_SET;
#endif /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= SSIPMD_CIM_SSI012EN_SET;
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI1PMD_MASK | SSIPMD_CIM_SSI2PMD_MASK);
                if (false != p_scux_info_ch->p_ssif_info1->ssif_cfg.mode_master)
                {
                    *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= ((SCUX_PIN_MODE_MASTER_SLAVE << SSIPMD_CIM_SSI1PMD_SHIFT) |
                                                                  (SCUX_PIN_MODE_MASTER_SLAVE << SSIPMD_CIM_SSI2PMD_SHIFT));
                    p_scux_info_ch->p_ssif_info2->pin_mode_backup = p_scux_info_ch->p_ssif_info2->pin_mode;
                    p_scux_info_ch->p_ssif_info2->pin_mode = SCUX_PIN_MODE_MASTER_SLAVE;
                    p_scux_info_ch->p_ssif_info3->pin_mode_backup = p_scux_info_ch->p_ssif_info3->pin_mode;
                    p_scux_info_ch->p_ssif_info3->pin_mode = SCUX_PIN_MODE_MASTER_SLAVE;
                }
                else
                {
                    *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= ((SCUX_PIN_MODE_SLAVE_SLAVE << SSIPMD_CIM_SSI1PMD_SHIFT) |
                                                                  (SCUX_PIN_MODE_SLAVE_SLAVE << SSIPMD_CIM_SSI2PMD_SHIFT));
                    p_scux_info_ch->p_ssif_info2->pin_mode_backup = p_scux_info_ch->p_ssif_info2->pin_mode;
                    p_scux_info_ch->p_ssif_info2->pin_mode = SCUX_PIN_MODE_SLAVE_SLAVE;
                    p_scux_info_ch->p_ssif_info3->pin_mode_backup = p_scux_info_ch->p_ssif_info3->pin_mode;
                    p_scux_info_ch->p_ssif_info3->pin_mode = SCUX_PIN_MODE_SLAVE_SLAVE;
                }
            break;

            case SCUX_ROUTE_SRC1_MIX_SSIF3 :
#if defined(TARGET_RZA1H) /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET | SSIPMD_CIM_SSI345EN_SET);
#else /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET);
#endif /* mbed */
            break;

#if defined(TARGET_RZA1H) /* mbed */
            case SCUX_ROUTE_SRC1_MIX_SSIF345 :
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~SSIPMD_CIM_SSI012EN_SET;
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= SSIPMD_CIM_SSI345EN_SET;
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI4PMD_MASK | SSIPMD_CIM_SSI5PMD_MASK);
                if (false != p_scux_info_ch->p_ssif_info1->ssif_cfg.mode_master)
                {
                    *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= ((SCUX_PIN_MODE_MASTER_SLAVE << SSIPMD_CIM_SSI4PMD_SHIFT) |
                                                                  (SCUX_PIN_MODE_MASTER_SLAVE << SSIPMD_CIM_SSI5PMD_SHIFT));
                    p_scux_info_ch->p_ssif_info2->pin_mode_backup = p_scux_info_ch->p_ssif_info2->pin_mode;
                    p_scux_info_ch->p_ssif_info2->pin_mode = SCUX_PIN_MODE_MASTER_SLAVE;
                    p_scux_info_ch->p_ssif_info3->pin_mode_backup = p_scux_info_ch->p_ssif_info3->pin_mode;
                    p_scux_info_ch->p_ssif_info3->pin_mode = SCUX_PIN_MODE_MASTER_SLAVE;                                                                  
                }
                else
                {
                    *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= ((SCUX_PIN_MODE_SLAVE_SLAVE << SSIPMD_CIM_SSI4PMD_SHIFT) |
                                                                  (SCUX_PIN_MODE_SLAVE_SLAVE << SSIPMD_CIM_SSI5PMD_SHIFT));
                    p_scux_info_ch->p_ssif_info2->pin_mode_backup = p_scux_info_ch->p_ssif_info2->pin_mode;
                    p_scux_info_ch->p_ssif_info2->pin_mode = SCUX_PIN_MODE_SLAVE_SLAVE;
                    p_scux_info_ch->p_ssif_info3->pin_mode_backup = p_scux_info_ch->p_ssif_info3->pin_mode;
                    p_scux_info_ch->p_ssif_info3->pin_mode = SCUX_PIN_MODE_SLAVE_SLAVE;                                                                  
                }
            break;

#endif /* mbed */
            case SCUX_ROUTE_SRC2_MIX_SSIF0 :
#if defined(TARGET_RZA1H) /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET | SSIPMD_CIM_SSI345EN_SET);
#else /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET);
#endif /* mbed */
            break;

            case SCUX_ROUTE_SRC2_MIX_SSIF012 :
#if defined(TARGET_RZA1H) /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~SSIPMD_CIM_SSI345EN_SET;
#endif /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= SSIPMD_CIM_SSI012EN_SET;
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI1PMD_MASK | SSIPMD_CIM_SSI2PMD_MASK);
                if (false != p_scux_info_ch->p_ssif_info1->ssif_cfg.mode_master)
                {
                    *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= ((SCUX_PIN_MODE_MASTER_SLAVE << SSIPMD_CIM_SSI1PMD_SHIFT) |
                                                                  (SCUX_PIN_MODE_MASTER_SLAVE << SSIPMD_CIM_SSI2PMD_SHIFT));
                    p_scux_info_ch->p_ssif_info2->pin_mode_backup = p_scux_info_ch->p_ssif_info2->pin_mode;
                    p_scux_info_ch->p_ssif_info2->pin_mode = SCUX_PIN_MODE_MASTER_SLAVE;
                    p_scux_info_ch->p_ssif_info3->pin_mode_backup = p_scux_info_ch->p_ssif_info3->pin_mode;
                    p_scux_info_ch->p_ssif_info3->pin_mode = SCUX_PIN_MODE_MASTER_SLAVE;
                }
                else
                {
                    *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= ((SCUX_PIN_MODE_SLAVE_SLAVE << SSIPMD_CIM_SSI1PMD_SHIFT) |
                                                                  (SCUX_PIN_MODE_SLAVE_SLAVE << SSIPMD_CIM_SSI2PMD_SHIFT));
                    p_scux_info_ch->p_ssif_info2->pin_mode_backup = p_scux_info_ch->p_ssif_info2->pin_mode;
                    p_scux_info_ch->p_ssif_info2->pin_mode = SCUX_PIN_MODE_SLAVE_SLAVE;
                    p_scux_info_ch->p_ssif_info3->pin_mode_backup = p_scux_info_ch->p_ssif_info3->pin_mode;
                    p_scux_info_ch->p_ssif_info3->pin_mode = SCUX_PIN_MODE_SLAVE_SLAVE;
                }
            break;

            case SCUX_ROUTE_SRC2_MIX_SSIF3 :
#if defined(TARGET_RZA1H) /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET | SSIPMD_CIM_SSI345EN_SET);
#else /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET);
#endif /* mbed */
            break;

#if defined(TARGET_RZA1H) /* mbed */
            case SCUX_ROUTE_SRC2_MIX_SSIF345 :
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~SSIPMD_CIM_SSI012EN_SET;
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= SSIPMD_CIM_SSI345EN_SET;
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI4PMD_MASK | SSIPMD_CIM_SSI5PMD_MASK);
                if (false != p_scux_info_ch->p_ssif_info1->ssif_cfg.mode_master)
                {
                    *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= ((SCUX_PIN_MODE_MASTER_SLAVE << SSIPMD_CIM_SSI4PMD_SHIFT) |
                                                                  (SCUX_PIN_MODE_MASTER_SLAVE << SSIPMD_CIM_SSI5PMD_SHIFT));
                    p_scux_info_ch->p_ssif_info2->pin_mode_backup = p_scux_info_ch->p_ssif_info2->pin_mode;
                    p_scux_info_ch->p_ssif_info2->pin_mode = SCUX_PIN_MODE_MASTER_SLAVE;
                    p_scux_info_ch->p_ssif_info3->pin_mode_backup = p_scux_info_ch->p_ssif_info3->pin_mode;
                    p_scux_info_ch->p_ssif_info3->pin_mode = SCUX_PIN_MODE_MASTER_SLAVE;                                                                  
                }
                else
                {
                    *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= ((SCUX_PIN_MODE_SLAVE_SLAVE << SSIPMD_CIM_SSI4PMD_SHIFT) |
                                                                  (SCUX_PIN_MODE_SLAVE_SLAVE << SSIPMD_CIM_SSI5PMD_SHIFT));
                    p_scux_info_ch->p_ssif_info2->pin_mode_backup = p_scux_info_ch->p_ssif_info2->pin_mode;
                    p_scux_info_ch->p_ssif_info2->pin_mode = SCUX_PIN_MODE_SLAVE_SLAVE;
                    p_scux_info_ch->p_ssif_info3->pin_mode_backup = p_scux_info_ch->p_ssif_info3->pin_mode;
                    p_scux_info_ch->p_ssif_info3->pin_mode = SCUX_PIN_MODE_SLAVE_SLAVE;                                                                  
                }
            break;

#endif /* mbed */
            case SCUX_ROUTE_SRC3_MIX_SSIF0 :
                /* fall through */
#if defined(TARGET_RZA1H) /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET | SSIPMD_CIM_SSI345EN_SET);
#else /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET);
#endif /* mbed */
            break;

            case SCUX_ROUTE_SRC3_MIX_SSIF012 :
#if defined(TARGET_RZA1H) /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~SSIPMD_CIM_SSI345EN_SET;
#endif /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= SSIPMD_CIM_SSI012EN_SET;
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI1PMD_MASK | SSIPMD_CIM_SSI2PMD_MASK);
                if (false != p_scux_info_ch->p_ssif_info1->ssif_cfg.mode_master)
                {
                    *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= ((SCUX_PIN_MODE_MASTER_SLAVE << SSIPMD_CIM_SSI1PMD_SHIFT) |
                                                                  (SCUX_PIN_MODE_MASTER_SLAVE << SSIPMD_CIM_SSI2PMD_SHIFT));
                    p_scux_info_ch->p_ssif_info2->pin_mode_backup = p_scux_info_ch->p_ssif_info2->pin_mode;
                    p_scux_info_ch->p_ssif_info2->pin_mode = SCUX_PIN_MODE_MASTER_SLAVE;
                    p_scux_info_ch->p_ssif_info3->pin_mode_backup = p_scux_info_ch->p_ssif_info3->pin_mode;
                    p_scux_info_ch->p_ssif_info3->pin_mode = SCUX_PIN_MODE_MASTER_SLAVE;
                }
                else
                {
                    *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= ((SCUX_PIN_MODE_SLAVE_SLAVE << SSIPMD_CIM_SSI1PMD_SHIFT) |
                                                                  (SCUX_PIN_MODE_SLAVE_SLAVE << SSIPMD_CIM_SSI2PMD_SHIFT));
                    p_scux_info_ch->p_ssif_info2->pin_mode_backup = p_scux_info_ch->p_ssif_info2->pin_mode;
                    p_scux_info_ch->p_ssif_info2->pin_mode = SCUX_PIN_MODE_SLAVE_SLAVE;
                    p_scux_info_ch->p_ssif_info3->pin_mode_backup = p_scux_info_ch->p_ssif_info3->pin_mode;
                    p_scux_info_ch->p_ssif_info3->pin_mode = SCUX_PIN_MODE_SLAVE_SLAVE;
                }
            break;

            case SCUX_ROUTE_SRC3_MIX_SSIF3 :
#if defined(TARGET_RZA1H) /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET | SSIPMD_CIM_SSI345EN_SET);
#else /* mbed */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI012EN_SET);
#endif /* mbed */
            break;

#if defined(TARGET_RZA1H) /* mbed */
            case SCUX_ROUTE_SRC3_MIX_SSIF345 :
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~SSIPMD_CIM_SSI012EN_SET;
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= SSIPMD_CIM_SSI345EN_SET;
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI4PMD_MASK | SSIPMD_CIM_SSI5PMD_MASK);
                if (false != p_scux_info_ch->p_ssif_info1->ssif_cfg.mode_master)
                {
                    *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= ((SCUX_PIN_MODE_MASTER_SLAVE << SSIPMD_CIM_SSI4PMD_SHIFT) |
                                                                  (SCUX_PIN_MODE_MASTER_SLAVE << SSIPMD_CIM_SSI5PMD_SHIFT));
                    p_scux_info_ch->p_ssif_info2->pin_mode_backup = p_scux_info_ch->p_ssif_info2->pin_mode;
                    p_scux_info_ch->p_ssif_info2->pin_mode = SCUX_PIN_MODE_MASTER_SLAVE;
                    p_scux_info_ch->p_ssif_info3->pin_mode_backup = p_scux_info_ch->p_ssif_info3->pin_mode;
                    p_scux_info_ch->p_ssif_info3->pin_mode = SCUX_PIN_MODE_MASTER_SLAVE;                                                                  
                }
                else
                {
                    *(p_scux_info_ch->p_scux_reg->ssipmd_cim) |= ((SCUX_PIN_MODE_SLAVE_SLAVE << SSIPMD_CIM_SSI4PMD_SHIFT) |
                                                                  (SCUX_PIN_MODE_SLAVE_SLAVE << SSIPMD_CIM_SSI5PMD_SHIFT));
                    p_scux_info_ch->p_ssif_info2->pin_mode_backup = p_scux_info_ch->p_ssif_info2->pin_mode;
                    p_scux_info_ch->p_ssif_info2->pin_mode = SCUX_PIN_MODE_SLAVE_SLAVE;
                    p_scux_info_ch->p_ssif_info3->pin_mode_backup = p_scux_info_ch->p_ssif_info3->pin_mode;
                    p_scux_info_ch->p_ssif_info3->pin_mode = SCUX_PIN_MODE_SLAVE_SLAVE;                                                                  
                }
            break;

#endif /* mbed */
            default :
                /* ->IPA R3.5.2 Nothing is being processed intentionally. */
                /* <-IPA R3.5.2 */
                /* NOTREACHED on At the time of a normal performance */
            break;
        }

        SCUX_SetupFifo(p_scux_info_ch);
        SCUX_SetupSrcFunction(p_scux_info_ch);
    }

    return;
}

/******************************************************************************
End of function SCUX_SetupSrc
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_SetupDataPosition
* @brief         Audio data position setup.
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        None.
******************************************************************************/

static void SCUX_SetupDataPosition(scux_info_ch_t * const p_scux_info_ch)
{
    int_t audio_ch;
    uint32_t audio_place = 0;
    scux_info_drv_t * const p_info_drv = SCUX_GetDrvInstance();

    if ((NULL == p_info_drv) || (NULL == p_scux_info_ch))
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        /* set data posion for SRC */
        for (audio_ch = 0; audio_ch < p_scux_info_ch->src_cfg.use_ch; audio_ch++)
        {
            audio_place |= (((uint32_t)p_scux_info_ch->src_cfg.select_in_data_ch[audio_ch] & SRCRSEL_CIM_PLACE_N_MASK) << ((uint32_t)audio_ch * SRCRSEL_CIM_PLACE_N_SHIFT));
        }
        *(p_scux_info_ch->p_scux_reg->srcrsel_n_cim) = audio_place;

        /* set data position for MIX */
        if (SCUX_ROUTE_MIX == (p_scux_info_ch->route_set & SCUX_GET_ROUTE_MASK))
        {
            if (0U == p_info_drv->shared_info.mix_run_ch)
            {
                audio_place = 0;
                for (audio_ch = 0; audio_ch < p_scux_info_ch->src_cfg.use_ch; audio_ch++)
                {
                    audio_place |= (((uint32_t)p_info_drv->shared_info.select_out_data_ch[audio_ch] & MIXRSEL_CIM_PLACE_N_MASK) << ((uint32_t)audio_ch * MIXRSEL_CIM_PLACE_N_SHIFT));
                }
                *(p_scux_info_ch->p_scux_reg->mixrsel_cim) = audio_place;
            }
        }
    }

    return;
}

/******************************************************************************
End of function SCUX_SetupDataPosition
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_SetupSrcClk
* @brief         SRC clock setup.
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        None.
******************************************************************************/

static void SCUX_SetupSrcClk(scux_info_ch_t * const p_scux_info_ch)
{
    uint32_t fdtsel_value = 0;

    if (NULL == p_scux_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        /* set input timing clock */
        if (false == p_scux_info_ch->src_cfg.mode_sync)
        {
            /* get value of register setting */
            switch (p_scux_info_ch->src_cfg.input_clk_async)
            {

                case SCUX_CLK_AUDIO_CLK :
                    fdtsel_value = (((p_scux_info_ch->src_cfg.input_div_async << FDTSEL_CIM_SCKDIV_SHIFT) & FDTSEL_CIM_SCKDIV_MASK) |
                                   FDTSEL_CIM_SCKSEL_AUDIO_CLK_SET);
                break;

                case SCUX_CLK_AUDIO_X1 :
                    fdtsel_value = (((p_scux_info_ch->src_cfg.input_div_async << FDTSEL_CIM_SCKDIV_SHIFT) & FDTSEL_CIM_SCKDIV_MASK) |
                                   FDTSEL_CIM_SCKSEL_AUDIO_X1_SET);
                break;

                case SCUX_CLK_MLB_CLK :
                    fdtsel_value = (((p_scux_info_ch->src_cfg.input_div_async << FDTSEL_CIM_SCKDIV_SHIFT) & FDTSEL_CIM_SCKDIV_MASK) |
                                   FDTSEL_CIM_SCKSEL_MLB_CLK_SET);
                break;

                case SCUX_CLK_USB_X1 :
                    fdtsel_value = (((p_scux_info_ch->src_cfg.input_div_async << FDTSEL_CIM_SCKDIV_SHIFT) & FDTSEL_CIM_SCKDIV_MASK) |
                                   FDTSEL_CIM_SCKSEL_USB_X1_SET);
                break;

                case SCUX_CLK_CLKP1_2 :
                    fdtsel_value = (((p_scux_info_ch->src_cfg.input_div_async << FDTSEL_CIM_SCKDIV_SHIFT) & FDTSEL_CIM_SCKDIV_MASK) |
                                   FDTSEL_CIM_SCKSEL_CLKP1_2_SET);
                break;

                case SCUX_CLK_MTU_TIOC3A :
                    fdtsel_value = (((p_scux_info_ch->src_cfg.input_div_async << FDTSEL_CIM_SCKDIV_SHIFT) & FDTSEL_CIM_SCKDIV_MASK) |
                                    FDTSEL_CIM_SCKSEL_MTUSEL2_SET |
                                    FDTSEL_CIM_MTUSEL_SET_TIOC3A);
                break;

                case SCUX_CLK_MTU_TIOC4A :
                    fdtsel_value = (((p_scux_info_ch->src_cfg.input_div_async << FDTSEL_CIM_SCKDIV_SHIFT) & FDTSEL_CIM_SCKDIV_MASK) |
                                    FDTSEL_CIM_SCKSEL_MTUSEL2_SET |
                                    FDTSEL_CIM_MTUSEL_SET_TIOC4A);
                break;

                case SCUX_CLK_SSIF0_WS :
                    fdtsel_value = FDTSEL_CIM_SCKSEL_SSIF0_WS_SET;
                break;

                case SCUX_CLK_SSIF1_WS :
                    fdtsel_value = FDTSEL_CIM_SCKSEL_SSIF1_WS_SET;
                break;

                case SCUX_CLK_SSIF2_WS :
                    fdtsel_value = FDTSEL_CIM_SCKSEL_SSIF2_WS_SET;
                break;

                case SCUX_CLK_SSIF3_WS :
                    fdtsel_value = FDTSEL_CIM_SCKSEL_SSIF3_WS_SET;
                break;

#if defined(TARGET_RZA1H) /* mbed */
                case SCUX_CLK_SSIF4_WS :
                    fdtsel_value = FDTSEL_CIM_SCKSEL_SSIF4_WS_SET;
                break;

                case SCUX_CLK_SSIF5_WS :
                    fdtsel_value = FDTSEL_CIM_SCKSEL_SSIF5_WS_SET;
                break;

#endif /* mbed */
                default :
                    /* ->IPA R3.5.2 Nothing is being processed intentionally. */
                    /* <-IPA R3.5.2 */
                    /* NOTREACHED on At the time of a normal performance */
                break;
            }
            *(p_scux_info_ch->p_scux_reg->fdtsel_n_cim) = fdtsel_value;
        }

        /* set output timing clock on MIX */
        if (SCUX_ROUTE_MIX == (p_scux_info_ch->route_set & SCUX_GET_ROUTE_MASK))
        {
            p_scux_info_ch->futsel_cim_value = (((0 << FUTSEL_CIM_SCKDIV_SHIFT) & FUTSEL_CIM_SCKDIV_MASK) |
                                                (((uint32_t)p_scux_info_ch->p_ssif_info1->ssif_cfg.ssif_ch_num | FUTSEL_CIM_SCKSEL_SSIF0_WS_SET) & FUTSEL_CIM_SCKSEL_MASK));
            *(p_scux_info_ch->p_scux_reg->futsel_n_cim) = p_scux_info_ch->futsel_cim_value;
        }
        else if (SCUX_ROUTE_MEM_TO_MEM == (p_scux_info_ch->route_set & SCUX_GET_ROUTE_MASK))
        {
            /* set output timing clock on other MIX */
            if (false == p_scux_info_ch->src_cfg.mode_sync)
            {
                switch (p_scux_info_ch->src_cfg.output_clk_async)
                {
                    case SCUX_CLK_AUDIO_CLK :
                        p_scux_info_ch->futsel_cim_value = (((p_scux_info_ch->src_cfg.output_div_async << FUTSEL_CIM_SCKDIV_SHIFT) & FUTSEL_CIM_SCKDIV_MASK) |
                                                            FUTSEL_CIM_SCKSEL_AUDIO_CLK_SET);
                    break;

                    case SCUX_CLK_AUDIO_X1 :
                        p_scux_info_ch->futsel_cim_value = (((p_scux_info_ch->src_cfg.output_div_async << FUTSEL_CIM_SCKDIV_SHIFT) & FUTSEL_CIM_SCKDIV_MASK) |
                                                            FUTSEL_CIM_SCKSEL_AUDIO_X1_SET);
                    break;

                    case SCUX_CLK_MLB_CLK :
                        p_scux_info_ch->futsel_cim_value = (((p_scux_info_ch->src_cfg.output_div_async << FUTSEL_CIM_SCKDIV_SHIFT) & FUTSEL_CIM_SCKDIV_MASK) |
                                                            FUTSEL_CIM_SCKSEL_MLB_CLK_SET);
                    break;

                    case SCUX_CLK_USB_X1 :
                        p_scux_info_ch->futsel_cim_value = (((p_scux_info_ch->src_cfg.output_div_async << FUTSEL_CIM_SCKDIV_SHIFT) & FUTSEL_CIM_SCKDIV_MASK) |
                                                            FUTSEL_CIM_SCKSEL_USB_X1_SET);
                    break;

                    case SCUX_CLK_CLKP1_2 :
                        p_scux_info_ch->futsel_cim_value = (((p_scux_info_ch->src_cfg.output_div_async << FUTSEL_CIM_SCKDIV_SHIFT) & FUTSEL_CIM_SCKDIV_MASK) |
                                                            FUTSEL_CIM_SCKSEL_CLKP1_2_SET);
                    break;

                    case SCUX_CLK_MTU_TIOC3A :
                        p_scux_info_ch->futsel_cim_value = (((p_scux_info_ch->src_cfg.output_div_async << FUTSEL_CIM_SCKDIV_SHIFT) & FUTSEL_CIM_SCKDIV_MASK) |
                                                            FUTSEL_CIM_SCKSEL_MTUSEL2_SET |
                                                            FUTSEL_CIM_MTUSEL_SET_TIOC3A);
                    break;

                    case SCUX_CLK_MTU_TIOC4A :
                        p_scux_info_ch->futsel_cim_value = (((p_scux_info_ch->src_cfg.output_div_async << FUTSEL_CIM_SCKDIV_SHIFT) & FUTSEL_CIM_SCKDIV_MASK) |
                                                            FUTSEL_CIM_SCKSEL_MTUSEL2_SET |
                                                            FUTSEL_CIM_MTUSEL_SET_TIOC4A);
                    break;

                    case SCUX_CLK_SSIF0_WS :
                        p_scux_info_ch->futsel_cim_value = FUTSEL_CIM_SCKSEL_SSIF0_WS_SET;
                    break;

                    case SCUX_CLK_SSIF1_WS :
                        p_scux_info_ch->futsel_cim_value = FUTSEL_CIM_SCKSEL_SSIF1_WS_SET;
                    break;

                    case SCUX_CLK_SSIF2_WS :
                        p_scux_info_ch->futsel_cim_value = FUTSEL_CIM_SCKSEL_SSIF2_WS_SET;
                    break;

                    case SCUX_CLK_SSIF3_WS :
                        p_scux_info_ch->futsel_cim_value = FUTSEL_CIM_SCKSEL_SSIF3_WS_SET;
                    break;

#if defined(TARGET_RZA1H) /* mbed */
                    case SCUX_CLK_SSIF4_WS :
                        p_scux_info_ch->futsel_cim_value = FUTSEL_CIM_SCKSEL_SSIF4_WS_SET;
                    break;

                    case SCUX_CLK_SSIF5_WS :
                        p_scux_info_ch->futsel_cim_value = FUTSEL_CIM_SCKSEL_SSIF5_WS_SET;
                    break;

#endif /* mbed */
                    default :
                        /* ->IPA R3.5.2 Nothing is being processed intentionally. */
                        /* <-IPA R3.5.2 */
                        /* NOTREACHED on At the time of a normal performance */
                    break;
                }
                *(p_scux_info_ch->p_scux_reg->futsel_n_cim) = p_scux_info_ch->futsel_cim_value;
            }
        }
        else
        {
            /* do nothing for SSIF route */
        }
    }

    return;
}

/******************************************************************************
End of function SCUX_SetupSrcClk
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_SetupFifo
* @brief         SRC FIFO setup.
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        None.
******************************************************************************/

static void SCUX_SetupFifo(scux_info_ch_t * const p_scux_info_ch)
{
    if (NULL == p_scux_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        /* set FFD register */
        p_scux_info_ch->p_scux_reg->p_ffd_reg->FDAIR_FFD0_0 = p_scux_info_ch->src_cfg.use_ch;
        p_scux_info_ch->p_scux_reg->p_ffd_reg->DRQSR_FFD0_0 = SCUX_FIFO_REQ_SIZE_128_32;

        GIC_EnableIRQ(p_scux_info_ch->int_num[SCUX_INT_FDI]);

        /* set FFU register */
        if (SCUX_ROUTE_MEM_TO_MEM == (p_scux_info_ch->route_set & SCUX_GET_ROUTE_MASK))
        {
            p_scux_info_ch->p_scux_reg->p_ffu_reg->FUAIR_FFU0_0 = p_scux_info_ch->src_cfg.use_ch;
            p_scux_info_ch->p_scux_reg->p_ffu_reg->URQSR_FFU0_0 = SCUX_FIFO_REQ_SIZE_128_32;

            GIC_EnableIRQ(p_scux_info_ch->int_num[SCUX_INT_FUI]);
        }

        p_scux_info_ch->tx_fifo_total_size = 0;
        p_scux_info_ch->rx_fifo_total_size = 0;
    }

    return;
}

/******************************************************************************
End of function SCUX_SetupFifo
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_SetupSrcFunction
* @brief         SRC function setup.
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        None.
******************************************************************************/

static void SCUX_SetupSrcFunction(scux_info_ch_t * const p_scux_info_ch)
{
    float32_t ifs_calc_value;
    uint32_t ifs_value;
    uint32_t sadir_value;
    uint32_t srccr_value = 0;
    uint32_t bfssr_value;

    if (NULL == p_scux_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        if (false == p_scux_info_ch->src_cfg.src_enable)
        {
            /* set bypass mode */
            if ((SCUX_CH_0 == p_scux_info_ch->channel) || (SCUX_CH_2 == p_scux_info_ch->channel))
            {
                p_scux_info_ch->p_scux_reg->p_src_reg->SRCBR0_2SRC0_0 |= SRCBR_2SRC0_BYPASS_SET;
            }
            else
            {
                p_scux_info_ch->p_scux_reg->p_src_reg->SRCBR1_2SRC0_0 |= SRCBR_2SRC0_BYPASS_SET;
            }
        }
        else
        {
            /* calculate sampling rate convert rate parameter */
            ifs_calc_value = ((float32_t)p_scux_info_ch->input_rate / (float32_t)p_scux_info_ch->output_rate);
            /* ->IPA R2.4.1 float value is intentionall casted to uint32_t value, because it is resigter value. */
            ifs_value = (uint32_t)(ifs_calc_value * (float32_t)SCUX_RATE_CONVERT_CALC_VALUE);
            /* <-IPA R2.4.1 */
            if (SCUX_DATA_LEN_16 == p_scux_info_ch->src_cfg.word_len)
            {
                sadir_value = (((uint32_t)p_scux_info_ch->src_cfg.use_ch & SADIR_2SRC0_CHNUM_MASK) | SADIR_2SRC0_OTBL_SET_16BIT);
            }
            else
            {
                sadir_value = (((uint32_t)p_scux_info_ch->src_cfg.use_ch & SADIR_2SRC0_CHNUM_MASK) | SADIR_2SRC0_OTBL_SET_24BIT);
            }

            /* setting of sync mode bit */
            if (false != p_scux_info_ch->src_cfg.mode_sync)
            {
                /* set sync mode */
                srccr_value |= SRCCR_2SRC0_SRCMD_SET;
            }
            else
            {
                /* set async mode (bit clear) */
                srccr_value &= ~SRCCR_2SRC0_SRCMD_SET;
            }

            /* setting of buffer for delay mode */
            if (SCUX_DELAY_NORMAL == p_scux_info_ch->src_cfg.delay_mode)
            {
                /* delay normal setting (bit clear) */
                srccr_value &= ~SRCCR_2SRC0_BUFMD_SET;
            }
            else
            {
                /* delay setting (bit set) */
                srccr_value |= SRCCR_2SRC0_BUFMD_SET;
            }

            /* setting enable bit for wait mode */
            if ((0U == p_scux_info_ch->src_cfg.wait_sample) || (false != p_scux_info_ch->cancel_operate_flag))
            {
                /* clear wait mode enable bit when wait value is 0 or cancel operatin on going */
                srccr_value &= ~SRCCR_2SRC0_WATMD_SET;
            }
            else
            {
                /* set wait mode enable bit when wait value is other than 0 */
                srccr_value |= SRCCR_2SRC0_WATMD_SET;
            }

            /* setting buffer value for delay mode */
            if (SCUX_DELAY_NORMAL == p_scux_info_ch->src_cfg.delay_mode)
            {
                /* set 0 to BFSSR register on delay normal */
                bfssr_value = 0;
            }
            else if (SCUX_DELAY_LOW_DELAY1 == p_scux_info_ch->src_cfg.delay_mode)
            {
                /* set delay mode 1 setting to BFSSR register on delay mode1 */
                bfssr_value = ((BFSSR_2SRC0_BUFDATA_SET_DELAY_MODE1 & BFSSR_2SRC0_BUFDATA_MASK) |
                               (BFSSR_2SRC0_BUFIN_SET_DELAY_MODE & BFSSR_2SRC0_BUFIN_MASK));
            }
            else
            {
                /* set delay mode 2 setting to BFSSR register on delay mode2 */
                bfssr_value = ((BFSSR_2SRC0_BUFDATA_SET_DELAY_MODE2 & BFSSR_2SRC0_BUFDATA_MASK) |
                               (BFSSR_2SRC0_BUFIN_SET_DELAY_MODE & BFSSR_2SRC0_BUFIN_MASK));
            }

            /* set register */
            if ((SCUX_CH_0 == p_scux_info_ch->channel) || (SCUX_CH_2 == p_scux_info_ch->channel))
            {
                p_scux_info_ch->p_scux_reg->p_src_reg->SRCBR0_2SRC0_0 &= ~SRCBR_2SRC0_BYPASS_SET;
                p_scux_info_ch->p_scux_reg->p_src_reg->SADIR0_2SRC0_0 = sadir_value;
                p_scux_info_ch->p_scux_reg->p_src_reg->IFSCR0_2SRC0_0 |= IFSCR_2SRC0_INTIFSEN_SET;
                p_scux_info_ch->p_scux_reg->p_src_reg->IFSVR0_2SRC0_0 = ifs_value;
                p_scux_info_ch->p_scux_reg->p_src_reg->MNFSR0_2SRC0_0 = ((ifs_value * (uint32_t)p_scux_info_ch->src_cfg.min_rate_percentage)
                                                                         / SCUX_CALC_MINFS_VALUE);
                p_scux_info_ch->p_scux_reg->p_src_reg->SRCCR0_2SRC0_0 = (SRCCR_2SRC0_BASE_VALUE | srccr_value);
                p_scux_info_ch->p_scux_reg->p_src_reg->BFSSR0_2SRC0_0 = bfssr_value;
                if (false != p_scux_info_ch->cancel_operate_flag)
                {
                    /* set 0 to register compulsorily when cancel operation on going */
                    p_scux_info_ch->p_scux_reg->p_src_reg->WATSR0_2SRC0_0 = 0U;
                }
                else
                {
                    /* set wait value to register when cancel operation on going */
                    p_scux_info_ch->p_scux_reg->p_src_reg->WATSR0_2SRC0_0 = (p_scux_info_ch->src_cfg.wait_sample & WATSR_2SRC0_WTIME_MASK);
                }
            }
            else
            {
                p_scux_info_ch->p_scux_reg->p_src_reg->SRCBR1_2SRC0_0 &= ~SRCBR_2SRC0_BYPASS_SET;
                p_scux_info_ch->p_scux_reg->p_src_reg->SADIR1_2SRC0_0 = sadir_value;
                p_scux_info_ch->p_scux_reg->p_src_reg->IFSCR1_2SRC0_0 |= IFSCR_2SRC0_INTIFSEN_SET;
                p_scux_info_ch->p_scux_reg->p_src_reg->IFSVR1_2SRC0_0 = ifs_value;
                p_scux_info_ch->p_scux_reg->p_src_reg->MNFSR1_2SRC0_0 = ((ifs_value * (uint32_t)p_scux_info_ch->src_cfg.min_rate_percentage)
                                                                         / SCUX_CALC_MINFS_VALUE);
                p_scux_info_ch->p_scux_reg->p_src_reg->SRCCR1_2SRC0_0 = (SRCCR_2SRC0_BASE_VALUE | srccr_value);
                p_scux_info_ch->p_scux_reg->p_src_reg->BFSSR1_2SRC0_0 = bfssr_value;
                if (false != p_scux_info_ch->cancel_operate_flag)
                {
                    /* set 0 to register compulsorily when cancel operation on going */
                    p_scux_info_ch->p_scux_reg->p_src_reg->WATSR1_2SRC0_0 = 0U;
                }
                else
                {
                    /* set wait value to register when cancel operation on going */
                    p_scux_info_ch->p_scux_reg->p_src_reg->WATSR1_2SRC0_0 = (p_scux_info_ch->src_cfg.wait_sample & WATSR_2SRC0_WTIME_MASK);
                }
            }
        }
    }

    return;
}

/******************************************************************************
End of function SCUX_SetupSrcFunction
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_SetupDvu
* @brief         DVU module setup.
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        None.
******************************************************************************/

void SCUX_SetupDvu(scux_info_ch_t * const p_scux_info_ch)
{
    if (NULL == p_scux_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        if (SCUX_DATA_LEN_16 == p_scux_info_ch->src_cfg.word_len)
        {
            p_scux_info_ch->p_scux_reg->p_dvu_reg->VADIR_DVU0_0 = (((uint32_t)p_scux_info_ch->src_cfg.use_ch & VADIR_DVU0_CHNUM_MASK) |
                                                                   VADIR_DVU0_OTBL_SET_16BIT);
        }
        else
        {
            p_scux_info_ch->p_scux_reg->p_dvu_reg->VADIR_DVU0_0 = (((uint32_t)p_scux_info_ch->src_cfg.use_ch & VADIR_DVU0_CHNUM_MASK) |
                                                                   VADIR_DVU0_OTBL_SET_24BIT);
        }

        if (false == p_scux_info_ch->dvu_cfg.dvu_enable)
        {
            p_scux_info_ch->p_scux_reg->p_dvu_reg->DVUBR_DVU0_0 |= DVUBR_DVU0_BYPASS_SET;
        }
        else
        {
            p_scux_info_ch->p_scux_reg->p_dvu_reg->DVUBR_DVU0_0 &= ~DVUBR_DVU0_BYPASS_SET;
        }
    }

    return;
}

/******************************************************************************
End of function SCUX_SetupDvu
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_SetupDvuVolume
* @brief         DVU volume setup.
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        None.
******************************************************************************/

static void SCUX_SetupDvuVolume(scux_info_ch_t * const p_scux_info_ch)
{
    int_t audio_ch;
    uint32_t vrctr_value = 0;

    if (NULL == p_scux_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        /* init DVU module */
        p_scux_info_ch->p_scux_reg->p_dvu_reg->DVUIR_DVU0_0 &= ~DVUIR_DVU0_INIT_SET;
        if (false != p_scux_info_ch->dvu_cfg.dvu_enable)
        {
            /* set digital volume */
            SCUX_SetDigiVolRegister(p_scux_info_ch);

            /* dummy setting to volume ramp */
            for (audio_ch = 0; audio_ch < p_scux_info_ch->src_cfg.use_ch; audio_ch++)
            {
                vrctr_value |= (VRCTR_DVU0_VREN_SET << audio_ch);
            }

            p_scux_info_ch->p_scux_reg->p_dvu_reg->DVUCR_DVU0_0 |= DVUCR_DVU0_VRMD_SET;
            p_scux_info_ch->p_scux_reg->p_dvu_reg->VRCTR_DVU0_0 = vrctr_value;

            p_scux_info_ch->p_scux_reg->p_dvu_reg->VRPDR_DVU0_0 = 0;
            p_scux_info_ch->p_scux_reg->p_dvu_reg->VRWTR_DVU0_0 = 0;
            p_scux_info_ch->p_scux_reg->p_dvu_reg->VRDBR_DVU0_0 = 0;

            /* set zerocross mute */
            SCUX_SetZerocrossMuteRegister(p_scux_info_ch);

            p_scux_info_ch->p_scux_reg->p_dvu_reg->DVUER_DVU0_0 |= DVUER_DVU0_DVUEN_SET;
            GIC_EnableIRQ(p_scux_info_ch->int_num[SCUX_INT_DVI]);
        }
    }

    return;
}

/******************************************************************************
End of function SCUX_SetupDvu
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_SetupMix
* @brief         MIX module setup.
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        None.
******************************************************************************/

static void SCUX_SetupMix(scux_info_ch_t * const p_scux_info_ch)
{
    int_t scux_ch;
    scux_info_drv_t * const p_info_drv = SCUX_GetDrvInstance();

    if ((NULL == p_info_drv) || (NULL == p_scux_info_ch))
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        /* initialized MIX resgister when the channel using MIX to the begining and not on going cancel */
        if ((0U == p_info_drv->shared_info.mix_run_ch) && (false == p_scux_info_ch->cancel_operate_flag))
        {
            *(p_scux_info_ch->p_scux_reg->madir_mix0_0) = ((uint32_t)p_scux_info_ch->src_cfg.use_ch & MADIR_MIX0_CHNUM_MASK);
            *(p_scux_info_ch->p_scux_reg->mixir_mix0_0) &= ~MIXIR_MIX0_INIT_SET;
        }

        for (scux_ch = SCUX_CH_0; scux_ch < SCUX_CH_NUM; scux_ch++)
        {
            /* register set on the channel itself which sets up or channel which has already MIX operated */
            if ((scux_ch == p_scux_info_ch->channel) ||
                (0U != (p_info_drv->shared_info.mix_run_ch & (1U << scux_ch))))
            {
                SCUX_SetMixVolRegister(scux_ch);
            }
        }

        /* initialized MIX resgister when the channel using MIX to the begining and not on going cancel */
        if ((0U == p_info_drv->shared_info.mix_run_ch) && (false == p_scux_info_ch->cancel_operate_flag))
        {
            *(p_scux_info_ch->p_scux_reg->mdber_mix0_0) &= ~MDBER_MIX0_MIXDBEN_SET;

            if (false != p_info_drv->shared_info.mixmode_ramp)
            {
                /* set ramp mode */
                *(p_scux_info_ch->p_scux_reg->mixmr_mix0_0) |= MIXMR_MIX0_MIXMODE_SET;
                *(p_scux_info_ch->p_scux_reg->mvpdr_mix0_0) = (uint32_t)((((uint32_t)p_info_drv->shared_info.up_period << MVPDR_MIX0_MXPDUP_SHIFT) & MVPDR_MIX0_MXPDUP_MASK) |
                                                              (((uint32_t)p_info_drv->shared_info.down_period << MVPDR_MIX0_MXPDDW_SHIFT) & MVPDR_MIX0_MXPDDW_MASK));
            }
            else
            {
                /* set step mode */
                *(p_scux_info_ch->p_scux_reg->mixmr_mix0_0) &= ~MIXMR_MIX0_MIXMODE_SET;
            }

            *(p_scux_info_ch->p_scux_reg->mdber_mix0_0) |= MDBER_MIX0_MIXDBEN_SET;
        }
        /* set bit SCUX channel which useing MIX */
        p_info_drv->shared_info.mix_run_ch |= (1U << p_scux_info_ch->channel);
    }

    return;
}

/******************************************************************************
End of function SCUX_SetupMix
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_SetupSsif
* @brief         SSIF module setup.
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        None.
******************************************************************************/

void SCUX_SetupSsif(const scux_info_ch_t * const p_scux_info_ch)
{
    scux_ssif_info_t *p_set_ssif;
    volatile uint8_t dummy_buf;
    uint32_t ssif_arrange_num;
    uint32_t ssicr_value = 0;
    scux_info_drv_t * const p_info_drv = SCUX_GetDrvInstance();

    if ((NULL == p_info_drv) || (NULL == p_scux_info_ch))
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        for (ssif_arrange_num = 0; ssif_arrange_num < SCUX_SSIF_NUM_CH_ARRANGEMENT; ssif_arrange_num++)
        {
            switch (ssif_arrange_num)
            {
                case SCUX_SSIF_CH_ARRANGEMENT1:
                    p_set_ssif = p_scux_info_ch->p_ssif_info1;
                break;

                case SCUX_SSIF_CH_ARRANGEMENT2:
                    p_set_ssif = p_scux_info_ch->p_ssif_info2;
                break;

                case SCUX_SSIF_CH_ARRANGEMENT3:
                    p_set_ssif = p_scux_info_ch->p_ssif_info3;
                break;

                default :
                    /* ->IPA R3.5.2 Nothing is being processed intentionally. */
                    /* <-IPA R3.5.2 */
                    /* NOTREACHED on At the time of a normal performance */
                break;
            }

            /* check wherher SSIF pointer 1, 2, 3 is NULL */
            if (NULL != p_set_ssif)
            {
                /* check SSIF is used on other MIX SCUX channel */
                if (0U == ((uint32_t)p_set_ssif->scux_channel & ~(1U << p_scux_info_ch->channel)))
                {
                    if (SCUX_ROUTE_MIX == (p_scux_info_ch->route_set & SCUX_GET_ROUTE_MASK))
                    {
                        p_info_drv->shared_info.mix_ssif_ch |= (1U << p_set_ssif->ssif_cfg.ssif_ch_num);
                    }

                    core_util_critical_section_enter();

                    /* input clock */
                    CPGSTBCR11 &= (uint8_t)~((uint8_t)gb_cpg_scux_ssif_stbcr_bit[p_set_ssif->ssif_cfg.ssif_ch_num]);
                    dummy_buf = CPGSTBCR11;

                    /* SSIF rest */
                    CPGSWRSTCR1 |= (uint8_t)gb_cpg_scux_ssif_swrst_bit[p_set_ssif->ssif_cfg.ssif_ch_num];
                    dummy_buf = CPGSWRSTCR1;

                    CPGSWRSTCR1 &= (uint8_t)~((uint8_t)gb_cpg_scux_ssif_swrst_bit[p_set_ssif->ssif_cfg.ssif_ch_num]);
                    dummy_buf = CPGSWRSTCR1;
#if(1) /* mbed */
                    (void)dummy_buf;
#endif

                    core_util_critical_section_exit();

                    SCUX_SetupSsifGpio(p_set_ssif->ssif_cfg.ssif_ch_num);

                    /* if mode is master mode, WS continue mode is enabled */
                    if (false != p_set_ssif->ssif_cfg.mode_master)
                    {
                        p_set_ssif->p_scux_ssif_reg->SSITDMR |= SCUX_SSITDMR_CONT_SET;
                    }
                    else
                    {
                        p_set_ssif->p_scux_ssif_reg->SSITDMR &= ~SCUX_SSITDMR_CONT_SET;
                    }

                    /* TDM mode is set according to use_tdm */
                    if (false != p_set_ssif->ssif_cfg.use_tdm)
                    {
                        p_set_ssif->p_scux_ssif_reg->SSITDMR |= SCUX_SSITDMR_TDM_SET;
                    }
                    else
                    {
                        p_set_ssif->p_scux_ssif_reg->SSITDMR &= ~SCUX_SSITDMR_TDM_SET;
                    }

                    /* over sample clock is set according to select_audio_clk */
                    /* if select_audio_clk is 0, set AUDIO_X1 */
                    /* if select_audio_clk is 0, set AUDIO_CLK */
                    if (false != p_set_ssif->ssif_cfg.select_audio_clk)
                    {
                        ssicr_value |= SCUX_SSICR_CKS_SET;
                    }
                    else
                    {
                        ssicr_value &= ~SCUX_SSICR_CKS_SET;
                    }

                    if (NULL == p_scux_info_ch->p_ssif_info2)
                    {
                        /* in case single channel, SSIF channel is set as use_ch kinds */
                        switch (p_scux_info_ch->src_cfg.use_ch)
                        {
                            case SCUX_USE_CH_1:
                                /* NOTREACHED on At the time of a normal performance */
                            break;

                            case SCUX_USE_CH_2:
                                /* if TDM mode is used on audio 2ch , error occured in SCUX_CheckSrcParam */
                                ssicr_value |= SCUX_SSICR_CHNL_SET_1CH;
                            break;

                            case SCUX_USE_CH_4:
                                ssicr_value |= SCUX_SSICR_CHNL_SET_2CH;
                            break;

                            case SCUX_USE_CH_6:
                                ssicr_value |= SCUX_SSICR_CHNL_SET_3CH;
                            break;

                            case SCUX_USE_CH_8:
                                ssicr_value |= SCUX_SSICR_CHNL_SET_4CH;
                            break;

                            default :
                                /* ->IPA R3.5.2 Nothing is being processed intentionally. */
                                /* <-IPA R3.5.2 */
                                /* NOTREACHED on At the time of a normal performance */
                            break;
                        }
                    }
                    else
                    {
                        /* in case multi channel, SSIF channel is set 1ch compulsorily */
                        ssicr_value |= SCUX_SSICR_CHNL_SET_1CH;
                    }

                    /* set data word */
                    if (SCUX_DATA_LEN_16 == p_scux_info_ch->src_cfg.word_len)
                    {
                        ssicr_value |= SCUX_SSICR_DWL_16BIT_SET;
                    }
                    else
                    {
                        ssicr_value |= SCUX_SSICR_DWL_24BIT_SET;
                    }

                    ssicr_value |= ((uint32_t)p_set_ssif->ssif_cfg.system_word << SCUX_SSICR_SWL_SHIFT);


                    if (SCUX_PIN_MODE_INDEPEND == p_set_ssif->pin_mode)
                    {
                        /* in case pin synchronization is slave, SSIF mode is set mode following mode_master */
                        if (false != p_set_ssif->ssif_cfg.mode_master)
                        {
                            ssicr_value |= (SCUX_SSICR_SCKD_SET | SCUX_SSICR_SWSD_SET);
                        }
                        else
                        {
                            ssicr_value &= ~(SCUX_SSICR_SCKD_SET | SCUX_SSICR_SWSD_SET);
                        }
                    }
                    else
                    {
                        /* in case pin synchronization is slave, SSIF mode is set slave mode compulsorily */
                        ssicr_value &= ~(SCUX_SSICR_SCKD_SET | SCUX_SSICR_SWSD_SET);
                    }

                    /* set serial bit clock polarity */
                    if (false != p_set_ssif->ssif_cfg.sck_polarity_rise)
                    {
                        ssicr_value |= SCUX_SSICR_SCKP_SET;
                    }
                    else
                    {
                        ssicr_value &= ~SCUX_SSICR_SCKP_SET;
                    }

                    /* set WS signal polarity */
                    if (false != p_set_ssif->ssif_cfg.ws_polarity_high)
                    {
                        ssicr_value |= SCUX_SSICR_SWSP_SET;
                    }
                    else
                    {
                        ssicr_value &= ~SCUX_SSICR_SWSP_SET;
                    }

                    /* set serial padding polarity */
                    if (false != p_set_ssif->ssif_cfg.padding_high)
                    {
                        ssicr_value |= SCUX_SSICR_SPDP_SET;
                    }
                    else
                    {
                        ssicr_value &= ~SCUX_SSICR_SPDP_SET;
                    }

                    /* set serial data align polarity */
                    if (false != p_set_ssif->ssif_cfg.serial_data_align)
                    {
                        ssicr_value &= ~SCUX_SSICR_SDTA_SET;
                    }
                    else
                    {
                        ssicr_value |= SCUX_SSICR_SDTA_SET;
                    }

                    /* set WS delay setting */
                    if (false == p_set_ssif->ssif_cfg.ws_delay)
                    {
                        ssicr_value |= SCUX_SSICR_DEL_SET;
                    }
                    else
                    {
                        ssicr_value &= ~SCUX_SSICR_DEL_SET;
                    }

                    /* set SSIF devide rate */
                    ssicr_value |= ((uint32_t)p_set_ssif->clk_div << SCUX_SSICR_CKDV_SHIFT);

                    p_set_ssif->p_scux_ssif_reg->SSICR = ssicr_value;

                    /* set noise cancel setting */
                    if ((false == p_set_ssif->ssif_cfg.mode_master) && (false != p_set_ssif->ssif_cfg.use_noise_cancel))
                    {
                        GPIO.SNCR |= (1U << gb_cpg_scux_ssif_sncr_bit[p_set_ssif->ssif_cfg.ssif_ch_num]);
                    }
                    else
                    {
                        GPIO.SNCR &= ~(1U << gb_cpg_scux_ssif_sncr_bit[p_set_ssif->ssif_cfg.ssif_ch_num]);
                    }
                }
            }
        }
    }

    return;
}

/******************************************************************************
End of function SCUX_SetupSsif
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_SetupDma
* @brief         DMA setup.
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        None.
******************************************************************************/

int_t SCUX_SetupDma(scux_info_ch_t * const p_scux_info_ch)
{
    int_t    retval = ESUCCESS;
    AIOCB    *dma_aio;
    uint32_t dmacr_cim_value = 0;

    if (NULL == p_scux_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        dma_aio = &gb_scux_write_dma_aio[p_scux_info_ch->channel];
        /* dummy data write start */
        dma_aio->aio_sigevent.sigev_notify = SIGEV_THREAD;
        dma_aio->aio_sigevent.sigev_value.sival_int = p_scux_info_ch->channel;
        if (SCUX_ROUTE_MEM_TO_MEM != (p_scux_info_ch->route_set & SCUX_GET_ROUTE_MASK))
        {
            dma_aio->aio_sigevent.sigev_notify_function = &SCUX_DMA_DirectTxCallBack;
        }
        else
        {
            dma_aio->aio_sigevent.sigev_notify_function = &SCUX_DMA_CopyTxCallBack;
        }
        p_scux_info_ch->dma_tx_setup.resource = p_scux_info_ch->dma_resource_tx;
        p_scux_info_ch->dma_tx_setup.direction = DMA_REQ_DES;

        switch (p_scux_info_ch->src_cfg.word_len)
        {
            case SCUX_DATA_LEN_16 :
                /* fall through */
            case SCUX_DATA_LEN_16_TO_24 :
                p_scux_info_ch->dma_tx_setup.dst_width = DMA_UNIT_2;
                p_scux_info_ch->dma_tx_setup.src_width = DMA_UNIT_2;
            break;

            case SCUX_DATA_LEN_24 :
                p_scux_info_ch->dma_tx_setup.dst_width = DMA_UNIT_4;
                p_scux_info_ch->dma_tx_setup.src_width = DMA_UNIT_4;
            break;

            default :
                /* NOTREACHED on At the time of a normal performance */
            break;
        }

        p_scux_info_ch->dma_tx_setup.dst_cnt = DMA_ADDR_FIX;
        p_scux_info_ch->dma_tx_setup.src_cnt = DMA_ADDR_INCREMENT;
        p_scux_info_ch->dma_tx_setup.p_aio = dma_aio;
#if(1) /* mbed */
        p_scux_info_ch->dma_tx_setup.int_level = p_scux_info_ch->int_level;
#endif

        /* get read DMA channel */
        if (SCUX_ROUTE_MEM_TO_MEM == (p_scux_info_ch->route_set & SCUX_GET_ROUTE_MASK))
        {
            p_scux_info_ch->dma_rx_ch = R_DMA_Alloc(DMA_ALLOC_CH, NULL);
            if (EERROR == p_scux_info_ch->dma_rx_ch)
            {
                retval = EMFILE_RBSP;
            }
            else
            {
                dma_aio = &gb_scux_read_dma_aio[p_scux_info_ch->channel];
                dma_aio->aio_sigevent.sigev_notify = SIGEV_THREAD;
                dma_aio->aio_sigevent.sigev_value.sival_int = p_scux_info_ch->channel;
                dma_aio->aio_sigevent.sigev_notify_function = &SCUX_DMA_CopyRxCallBack;
                p_scux_info_ch->dma_rx_setup.resource = p_scux_info_ch->dma_resource_rx;
                p_scux_info_ch->dma_rx_setup.direction = DMA_REQ_SRC;

                switch (p_scux_info_ch->src_cfg.word_len)
                {
                    case SCUX_DATA_LEN_16 :
                        p_scux_info_ch->dma_rx_setup.dst_width = DMA_UNIT_2;
                        p_scux_info_ch->dma_rx_setup.src_width = DMA_UNIT_2;
                    break;

                    case SCUX_DATA_LEN_24 :
                        /* fall through */
                    case SCUX_DATA_LEN_16_TO_24 :
                        p_scux_info_ch->dma_rx_setup.dst_width = DMA_UNIT_4;
                        p_scux_info_ch->dma_rx_setup.src_width = DMA_UNIT_4;
                    break;

                    default :
                        /* NOTREACHED on At the time of a normal performance */
                    break;
                }

                p_scux_info_ch->dma_rx_setup.dst_cnt = DMA_ADDR_INCREMENT;
                p_scux_info_ch->dma_rx_setup.src_cnt = DMA_ADDR_FIX;
                p_scux_info_ch->dma_rx_setup.p_aio = dma_aio;
#if(1) /* mbed */
                p_scux_info_ch->dma_rx_setup.int_level = p_scux_info_ch->int_level;
#endif
            }
        }

        if (ESUCCESS == retval)
        {
            dmacr_cim_value = *(p_scux_info_ch->p_scux_reg->dmacr_cim);
            /* set write DMA param */
            dmacr_cim_value |= (DMACR_CIM_DMAMDFFD_N_SET << p_scux_info_ch->channel);

            /* set read DMA param */
            if (SCUX_ROUTE_MEM_TO_MEM == (p_scux_info_ch->route_set & SCUX_GET_ROUTE_MASK))
            {
                dmacr_cim_value |= (DMACR_CIM_DMAMDFFU_N_SET << p_scux_info_ch->channel);
            }
        }

        if (ESUCCESS == retval)
        {
            *(p_scux_info_ch->p_scux_reg->dmacr_cim) = dmacr_cim_value;
        }
    }

    return retval;
}

/******************************************************************************
End of function SCUX_SetupDma
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_InitHw
* @brief         Initialize HW .
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        None.
******************************************************************************/

void SCUX_InitHw(scux_info_ch_t * const p_scux_info_ch)
{
    scux_ssif_info_t *p_set_ssif;
    uint32_t ssif_arrange_num;
    scux_info_drv_t * const p_info_drv = SCUX_GetDrvInstance();

    if ((NULL == p_info_drv) || (NULL == p_scux_info_ch))
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        /* init FFD register */
        p_scux_info_ch->p_scux_reg->p_ffd_reg->FFDIR_FFD0_0 = FFDIR_FFD0_INIT_VALUE;
        p_scux_info_ch->p_scux_reg->p_ffd_reg->FDAIR_FFD0_0 = FDAIR_FFD0_INIT_VALUE;
        p_scux_info_ch->p_scux_reg->p_ffd_reg->DRQSR_FFD0_0 = DRQSR_FFD0_INIT_VALUE;
        p_scux_info_ch->p_scux_reg->p_ffd_reg->FFDPR_FFD0_0 = FFDPR_FFD0_INIT_VALUE;
        p_scux_info_ch->p_scux_reg->p_ffd_reg->FFDBR_FFD0_0 = FFDBR_FFD0_INIT_VALUE;
        p_scux_info_ch->p_scux_reg->p_ffd_reg->DEVMR_FFD0_0 = DEVMR_FFD0_INIT_VALUE;
        p_scux_info_ch->p_scux_reg->p_ffd_reg->DEVCR_FFD0_0 = DEVCR_FFD0_INIT_VALUE;
        p_scux_info_ch->p_scux_reg->p_ipc_reg->IPCIR_IPC0_0 = IPCIR_IPC0_INIT_VALUE;
        p_scux_info_ch->p_scux_reg->p_ipc_reg->IPSLR_IPC0_0 = IPSLR_IPC0_INIT_VALUE;
        *(p_scux_info_ch->p_scux_reg->fdtsel_n_cim) = FDTSEL_CIM_INIT_VALUE;
        *(p_scux_info_ch->p_scux_reg->srcrsel_n_cim) = SRCRSEL_CIM_INIT_VALUE;

        /* check usc_ch ,and init SRC register */
        if ((SCUX_CH_0 == p_scux_info_ch->channel) || (SCUX_CH_2 == p_scux_info_ch->channel))
        {
            p_scux_info_ch->p_scux_reg->p_src_reg->SRCIR0_2SRC0_0 = SRCIR_2SRC0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_src_reg->SADIR0_2SRC0_0 = SADIR_2SRC0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_src_reg->SRCBR0_2SRC0_0 = SRCBR_2SRC0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_src_reg->IFSCR0_2SRC0_0 = IFSCR_2SRC0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_src_reg->IFSVR0_2SRC0_0 = IFSVR_2SRC0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_src_reg->SRCCR0_2SRC0_0 = (SRCCR_2SRC0_INIT_VALUE | SRCCR_2SRC0_BASE_VALUE);
            p_scux_info_ch->p_scux_reg->p_src_reg->MNFSR0_2SRC0_0 = MNFSR_2SRC0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_src_reg->BFSSR0_2SRC0_0 = BFSSR_2SRC0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_src_reg->WATSR0_2SRC0_0 = WATSR_2SRC0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_src_reg->SEVMR0_2SRC0_0 = SEVMR_2SRC0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_src_reg->SEVCR0_2SRC0_0 = SEVCR_2SRC0_INIT_VALUE;
        }
        else
        {
            p_scux_info_ch->p_scux_reg->p_src_reg->SRCIR1_2SRC0_0 = SRCIR_2SRC0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_src_reg->SADIR1_2SRC0_0 = SADIR_2SRC0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_src_reg->SRCBR1_2SRC0_0 = SRCBR_2SRC0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_src_reg->IFSCR1_2SRC0_0 = IFSCR_2SRC0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_src_reg->IFSVR1_2SRC0_0 = IFSVR_2SRC0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_src_reg->SRCCR1_2SRC0_0 = (SRCCR_2SRC0_INIT_VALUE | SRCCR_2SRC0_BASE_VALUE);
            p_scux_info_ch->p_scux_reg->p_src_reg->MNFSR1_2SRC0_0 = MNFSR_2SRC0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_src_reg->BFSSR1_2SRC0_0 = BFSSR_2SRC0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_src_reg->WATSR1_2SRC0_0 = WATSR_2SRC0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_src_reg->SEVMR1_2SRC0_0 = SEVMR_2SRC0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_src_reg->SEVCR1_2SRC0_0 = SEVCR_2SRC0_INIT_VALUE;
        }

        /* init FFU register */
        if (SCUX_ROUTE_MEM_TO_MEM == (p_scux_info_ch->route_set & SCUX_GET_ROUTE_MASK))
        {
            p_scux_info_ch->p_scux_reg->p_ffu_reg->FFUIR_FFU0_0 = FFUIR_FFU0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_ffu_reg->FUAIR_FFU0_0 = FUAIR_FFU0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_ffu_reg->URQSR_FFU0_0 = URQSR_FFU0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_ffu_reg->FFUPR_FFU0_0 = FFUPR_FFU0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_ffu_reg->UEVMR_FFU0_0 = UEVMR_FFU0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_ffu_reg->UEVCR_FFU0_0 = UEVCR_FFU0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_opc_reg->OPCIR_OPC0_0 = OPCIR_OPC0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_opc_reg->OPSLR_OPC0_0 = OPSLR_OPC0_INIT_VALUE;
            *(p_scux_info_ch->p_scux_reg->futsel_n_cim) = FUTSEL_CIM_INIT_VALUE;
            p_scux_info_ch->futsel_cim_value = FUTSEL_CIM_INIT_VALUE;
        }
        else
        {
            p_scux_info_ch->p_scux_reg->p_dvu_reg->DVUIR_DVU0_0 = DVUIR_DVU0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_dvu_reg->VADIR_DVU0_0 = VADIR_DVU0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_dvu_reg->DVUBR_DVU0_0 = DVUBR_DVU0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_dvu_reg->DVUCR_DVU0_0 = DVUCR_DVU0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_dvu_reg->ZCMCR_DVU0_0 = ZCMCR_DVU0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_dvu_reg->VRCTR_DVU0_0 = VRCTR_DVU0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_dvu_reg->VRPDR_DVU0_0 = VRPDR_DVU0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_dvu_reg->VRDBR_DVU0_0 = VRDBR_DVU0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_dvu_reg->VRWTR_DVU0_0 = VRWTR_DVU0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_dvu_reg->VOL0R_DVU0_0 = VOL_N_R_DVU0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_dvu_reg->VOL1R_DVU0_0 = VOL_N_R_DVU0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_dvu_reg->VOL2R_DVU0_0 = VOL_N_R_DVU0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_dvu_reg->VOL3R_DVU0_0 = VOL_N_R_DVU0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_dvu_reg->VOL4R_DVU0_0 = VOL_N_R_DVU0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_dvu_reg->VOL5R_DVU0_0 = VOL_N_R_DVU0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_dvu_reg->VOL6R_DVU0_0 = VOL_N_R_DVU0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_dvu_reg->VOL7R_DVU0_0 = VOL_N_R_DVU0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_dvu_reg->DVUER_DVU0_0 = DVUER_DVU0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_dvu_reg->VEVMR_DVU0_0 = VEVMR_DVU0_INIT_VALUE;
            p_scux_info_ch->p_scux_reg->p_dvu_reg->VEVCR_DVU0_0 = VEVCR_DVU0_INIT_VALUE;

            for (ssif_arrange_num = 0; ssif_arrange_num < SCUX_SSIF_NUM_CH_ARRANGEMENT; ssif_arrange_num++)
            {
                switch (ssif_arrange_num)
                {
                    case SCUX_SSIF_CH_ARRANGEMENT1:
                        p_set_ssif = p_scux_info_ch->p_ssif_info1;
                    break;

                    case SCUX_SSIF_CH_ARRANGEMENT2:
                        p_set_ssif = p_scux_info_ch->p_ssif_info2;
                    break;

                    case SCUX_SSIF_CH_ARRANGEMENT3:
                        p_set_ssif = p_scux_info_ch->p_ssif_info3;
                    break;

                    default :
                        /* ->IPA R3.5.2 Nothing is being processed intentionally. */
                        /* <-IPA R3.5.2 */
                        /* NOTREACHED on At the time of a normal performance */
                    break;
                }

                if (NULL != p_set_ssif)
                {
                    /* check SSIF is used on other MIX SCUX channel */
                    if (0U == ((uint32_t)p_set_ssif->scux_channel & ~(1U << p_scux_info_ch->channel)))
                    {
                        /* init SSIF register */
                        p_set_ssif->p_scux_ssif_reg->SSICR = SCUX_SSICR_INIT_VALUE;
                        p_set_ssif->p_scux_ssif_reg->SSIFCR = SCUX_SSIFCR_INIT_VALUE;
                        p_set_ssif->p_scux_ssif_reg->SSIFTDR = SCUX_SSIFTDR_INIT_VALUE;
                        p_set_ssif->p_scux_ssif_reg->SSITDMR = SCUX_SSITDMR_INIT_VALUE;
                        p_set_ssif->p_scux_ssif_reg->SSIFCCR = SCUX_SSIFCCR_INIT_VALUE;
                        p_set_ssif->p_scux_ssif_reg->SSIFCMR = SCUX_SSIFCMR_INIT_VALUE;
                    }
                }
            }
        }

        if (SCUX_ROUTE_MIX == (p_scux_info_ch->route_set & SCUX_GET_ROUTE_MASK))
        {
            /* initialized MIX resgister when the channel using MIX to the begining and not on going cancel */
            if ((0U == p_info_drv->shared_info.mix_run_ch) && (false == p_scux_info_ch->cancel_operate_flag))
            {
                *(p_scux_info_ch->p_scux_reg->mdb_n_r_mix0_0) = MDB_N_R_MIX0_0_INIT_VALUE;
                *(p_scux_info_ch->p_scux_reg->futsel_n_cim) = FUTSEL_CIM_INIT_VALUE;
                p_scux_info_ch->futsel_cim_value = FUTSEL_CIM_INIT_VALUE;
                *(p_scux_info_ch->p_scux_reg->srcrsel_n_cim) = SRCRSEL_CIM_INIT_VALUE;
                *(p_scux_info_ch->p_scux_reg->mixir_mix0_0) = MIXIR_MIX0_0_INIT_VALUE;
                *(p_scux_info_ch->p_scux_reg->madir_mix0_0) = MADIR_MIX0_0_INIT_VALUE;
                *(p_scux_info_ch->p_scux_reg->mixbr_mix0_0) = MIXBR_MIX0_0_INIT_VALUE;
                *(p_scux_info_ch->p_scux_reg->mixmr_mix0_0) = MIXMR_MIX0_0_INIT_VALUE;
                *(p_scux_info_ch->p_scux_reg->mvpdr_mix0_0) = MVPDR_MIX0_0_INIT_VALUE;
                *(p_scux_info_ch->p_scux_reg->mdber_mix0_0) = MDBER_MIX0_0_INIT_VALUE;
                *(p_scux_info_ch->p_scux_reg->mixrsel_cim) = MIXRSEL_CIM_INIT_VALUE;
            }
        }
    }

    return;
}

/******************************************************************************
End of function SCUX_InitHw
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_SyncStartHw
* @brief         Start Hw on sync mode.
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        None.
******************************************************************************/

void SCUX_SyncStartHw(const scux_info_ch_t * const p_scux_info_ch)
{
    if (NULL == p_scux_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        p_scux_info_ch->p_scux_reg->p_ffd_reg->FFDIR_FFD0_0 &= ~FFDIR_FFD0_INIT_SET;
        p_scux_info_ch->p_scux_reg->p_ffd_reg->FFDBR_FFD0_0 |= FFDBR_FFD0_BOOT_SET;
        p_scux_info_ch->p_scux_reg->p_ffu_reg->FFUIR_FFU0_0 &= ~FFUIR_FFU0_INIT_SET;

        /* check usc_ch ,and init SRC register */
        if ((SCUX_CH_0 == p_scux_info_ch->channel) || (SCUX_CH_2 == p_scux_info_ch->channel))
        {
            p_scux_info_ch->p_scux_reg->p_src_reg->SRCIR0_2SRC0_0 &= ~SRCIR_2SRC0_INIT_SET;
            p_scux_info_ch->p_scux_reg->p_src_reg->SEVMR0_2SRC0_0 |= (SEVMR_2SRC0_EVMUF_SET | SEVMR_2SRC0_EVMOF_SET);
        }
        else
        {
            p_scux_info_ch->p_scux_reg->p_src_reg->SRCIR1_2SRC0_0 &= ~SRCIR_2SRC0_INIT_SET;
            p_scux_info_ch->p_scux_reg->p_src_reg->SEVMR1_2SRC0_0 |= (SEVMR_2SRC0_EVMUF_SET | SEVMR_2SRC0_EVMOF_SET);
        }
        p_scux_info_ch->p_scux_reg->p_ipc_reg->IPCIR_IPC0_0 &= ~IPCIR_IPC0_INIT_SET;
        p_scux_info_ch->p_scux_reg->p_opc_reg->OPCIR_OPC0_0 &= ~OPCIR_OPC0_INIT_SET;

        p_scux_info_ch->p_scux_reg->p_src_reg->SRCIRR_2SRC0_0 &= ~SRCIRR_2SRC0_INIT_SET;

        GIC_EnableIRQ(p_scux_info_ch->int_num[SCUX_INT_AI]);
    }

    return;
}

/******************************************************************************
End of function SCUX_SyncStartHw
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_AsyncStartHw
* @brief         Start Hw on async mode.
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        None.
******************************************************************************/

void SCUX_AsyncStartHw(scux_info_ch_t * const p_scux_info_ch)
{
    scux_info_drv_t * const p_info_drv = SCUX_GetDrvInstance();

    if ((NULL == p_info_drv) || (NULL == p_scux_info_ch))
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        p_scux_info_ch->p_scux_reg->p_ffd_reg->FFDIR_FFD0_0 &= ~FFDIR_FFD0_INIT_SET;
        p_scux_info_ch->p_scux_reg->p_ffd_reg->FFDBR_FFD0_0 |= FFDBR_FFD0_BOOT_SET;

        if (SCUX_ROUTE_MEM_TO_MEM == (p_scux_info_ch->route_set & SCUX_GET_ROUTE_MASK))
        {
            p_scux_info_ch->p_scux_reg->p_ffu_reg->FFUIR_FFU0_0 &= ~FFUIR_FFU0_INIT_SET;
        }

        /* check usc_ch ,and init SRC register */
        if ((SCUX_CH_0 == p_scux_info_ch->channel) || (SCUX_CH_2 == p_scux_info_ch->channel))
        {
            p_scux_info_ch->p_scux_reg->p_src_reg->SRCIR0_2SRC0_0 &= ~SRCIR_2SRC0_INIT_SET;
            p_scux_info_ch->p_scux_reg->p_src_reg->SEVMR0_2SRC0_0 |= (SEVMR_2SRC0_EVMUF_SET | SEVMR_2SRC0_EVMOF_SET);
        }
        else
        {
            p_scux_info_ch->p_scux_reg->p_src_reg->SRCIR1_2SRC0_0 &= ~SRCIR_2SRC0_INIT_SET;
            p_scux_info_ch->p_scux_reg->p_src_reg->SEVMR1_2SRC0_0 |= (SEVMR_2SRC0_EVMUF_SET | SEVMR_2SRC0_EVMOF_SET);
        }

        p_scux_info_ch->p_scux_reg->p_src_reg->SRCIRR_2SRC0_0 &= ~SRCIRR_2SRC0_INIT_SET;

        GIC_EnableIRQ(p_scux_info_ch->int_num[SCUX_INT_AI]);

        if (SCUX_ROUTE_MEM_TO_MEM != (p_scux_info_ch->route_set & SCUX_GET_ROUTE_MASK))
        {
            SCUX_SetupDvuVolume(p_scux_info_ch);

            if (SCUX_ROUTE_MIX == (p_scux_info_ch->route_set & SCUX_GET_ROUTE_MASK))
            {
                SCUX_SetupMix(p_scux_info_ch);
            }
        }

        p_scux_info_ch->p_scux_reg->p_ipc_reg->IPCIR_IPC0_0 &= ~IPCIR_IPC0_INIT_SET;
        p_scux_info_ch->p_scux_reg->p_opc_reg->OPCIR_OPC0_0 &= ~OPCIR_OPC0_INIT_SET;

        if (SCUX_ROUTE_MEM_TO_MEM != (p_scux_info_ch->route_set & SCUX_GET_ROUTE_MASK))
        {
            /* check used SSIF is only 1 ch */
            if (NULL == p_scux_info_ch->p_ssif_info2)
            {
                p_scux_info_ch->p_ssif_info1->p_scux_ssif_reg->SSICR |= SCUX_SSICR_TEN_SET;
            }
            else
            {
                if (SCUX_SSIF_CH_0 == p_scux_info_ch->p_ssif_info1->ssif_cfg.ssif_ch_num)
                {
                    p_info_drv->shared_info.ssictrl_cim_value  |= SSICTRL_CIM_SSI012TEN_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                }
#if defined(TARGET_RZA1H) /* mbed */
                else
                {
                    p_info_drv->shared_info.ssictrl_cim_value |= SSICTRL_CIM_SSI345TEN_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                }
#endif /* mbed */
            }
        }
    }

    return;
}

/******************************************************************************
End of function SCUX_AsyncStartHw
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_SyncStopHw
* @brief         Stop Hw on sync mode.
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        None.
******************************************************************************/

void SCUX_SyncStopHw(const scux_info_ch_t * const p_scux_info_ch)
{
    if (NULL == p_scux_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        p_scux_info_ch->p_scux_reg->p_ffu_reg->UEVMR_FFU0_0 &= ~(UEVMR_FFU0_UEVMUF_SET |
                                                                 UEVMR_FFU0_UEVMOF_SET |
                                                                 UEVMR_FFU0_UEVMOL_SET);
        GIC_DisableIRQ(p_scux_info_ch->int_num[SCUX_INT_FUI]);

        p_scux_info_ch->p_scux_reg->p_ffu_reg->FFUIR_FFU0_0 |= FFUIR_FFU0_INIT_SET;
        p_scux_info_ch->p_scux_reg->p_opc_reg->OPCIR_OPC0_0 |= OPCIR_OPC0_INIT_SET;

        p_scux_info_ch->p_scux_reg->p_ffd_reg->DEVMR_FFD0_0 &= ~(DEVMR_FFD0_DEVMUF_SET |
                                                                 DEVMR_FFD0_DEVMOF_SET |
                                                                 DEVMR_FFD0_DEVMOL_SET |
                                                                 DEVMR_FFD0_DEVMIUF_SET);
        GIC_DisableIRQ(p_scux_info_ch->int_num[SCUX_INT_FDI]);

        p_scux_info_ch->p_scux_reg->p_ffd_reg->FFDIR_FFD0_0 |= FFDIR_FFD0_INIT_SET;
        p_scux_info_ch->p_scux_reg->p_ipc_reg->IPCIR_IPC0_0 |= IPCIR_IPC0_INIT_SET;

        p_scux_info_ch->p_scux_reg->p_ffd_reg->FFDBR_FFD0_0 &= ~FFDBR_FFD0_BOOT_SET;

        /* check usc_ch ,and init SRC register */
        if ((SCUX_CH_0 == p_scux_info_ch->channel) || (SCUX_CH_2 == p_scux_info_ch->channel))
        {
            p_scux_info_ch->p_scux_reg->p_src_reg->SRCIR0_2SRC0_0 |= SRCIR_2SRC0_INIT_SET;
            p_scux_info_ch->p_scux_reg->p_src_reg->SEVMR0_2SRC0_0 &= ~(SEVMR_2SRC0_EVMUF_SET |
                                                                       SEVMR_2SRC0_EVMOF_SET);
        }
        else
        {
            p_scux_info_ch->p_scux_reg->p_src_reg->SRCIR1_2SRC0_0 |= SRCIR_2SRC0_INIT_SET;
            p_scux_info_ch->p_scux_reg->p_src_reg->SEVMR1_2SRC0_0 &= ~(SEVMR_2SRC0_EVMUF_SET |
                                                                       SEVMR_2SRC0_EVMOF_SET);
        }

        p_scux_info_ch->p_scux_reg->p_src_reg->SRCIRR_2SRC0_0 |= SRCIRR_2SRC0_INIT_SET;

        GIC_DisableIRQ(p_scux_info_ch->int_num[SCUX_INT_AI]);
    }

    return;
}

/******************************************************************************
End of function SCUX_SyncStopHw
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_AsyncStopHw
* @brief         Stop Hw on async mode.
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        None.
******************************************************************************/

void SCUX_AsyncStopHw(scux_info_ch_t * const p_scux_info_ch)
{
    uint32_t  scux_check_ch;
    scux_info_drv_t * const p_info_drv = SCUX_GetDrvInstance();
    uint32_t  ssipmd_mask_ssif2 = 0;
    uint32_t  ssipmd_mask_ssif3 = 0;
    uint32_t  ssipmd_shift_ssif2 = 0;
    uint32_t  ssipmd_shift_ssif3 = 0;
    uint32_t  ssipmd_reg;
    scux_info_ch_t *p_pair_scux_ch;

    if ((NULL == p_info_drv) || (NULL == p_scux_info_ch))
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        if (SCUX_ROUTE_MEM_TO_MEM == (p_scux_info_ch->route_set & SCUX_GET_ROUTE_MASK))
        {
            p_scux_info_ch->p_scux_reg->p_ffu_reg->UEVMR_FFU0_0 &= ~(UEVMR_FFU0_UEVMUF_SET |
                                                                     UEVMR_FFU0_UEVMOF_SET |
                                                                     UEVMR_FFU0_UEVMOL_SET);
            GIC_DisableIRQ(p_scux_info_ch->int_num[SCUX_INT_FUI]);
            p_scux_info_ch->futsel_cim_value &= ~FUTSEL_CIM_DIVEN_SET;
            *(p_scux_info_ch->p_scux_reg->futsel_n_cim) = p_scux_info_ch->futsel_cim_value;

            p_scux_info_ch->p_scux_reg->p_ffu_reg->FFUIR_FFU0_0 |= FFUIR_FFU0_INIT_SET;
        }

        p_scux_info_ch->p_scux_reg->p_opc_reg->OPCIR_OPC0_0 |= OPCIR_OPC0_INIT_SET;

        if (SCUX_ROUTE_MIX == (p_scux_info_ch->route_set & SCUX_GET_ROUTE_MASK))
        {
            p_info_drv->shared_info.mix_run_ch &= ~(1U << p_scux_info_ch->channel);
        }

        switch (p_scux_info_ch->route_set)
        {
            case SCUX_ROUTE_SRC0_MEM :
                /* fall through */
            case SCUX_ROUTE_SRC1_MEM :
                /* fall through */
            case SCUX_ROUTE_SRC2_MEM :
                /* fall through */
            case SCUX_ROUTE_SRC3_MEM :
                /* do nothing, when mem to mem route is setting */
            break;

            case SCUX_ROUTE_SRC0_SSIF0 :
                p_info_drv->shared_info.ssictrl_cim_value &= ~SSICTRL_CIM_SSI0TX_SET;
                *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                p_scux_info_ch->p_ssif_info1->p_scux_ssif_reg->SSICR &= ~SCUX_SSICR_TEN_SET;
            break;

            case SCUX_ROUTE_SRC0_SSIF012 :
                p_info_drv->shared_info.ssictrl_cim_value  &= ~(SSICTRL_CIM_SSI0TX_SET | SSICTRL_CIM_SSI012TEN_SET);
                *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
            break;

            case SCUX_ROUTE_SRC0_SSIF3 :
                p_info_drv->shared_info.ssictrl_cim_value &= ~SSICTRL_CIM_SSI3TX_SET;
                *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                p_scux_info_ch->p_ssif_info1->p_scux_ssif_reg->SSICR &= ~SCUX_SSICR_TEN_SET;
            break;

#if defined(TARGET_RZA1H) /* mbed */
            case SCUX_ROUTE_SRC0_SSIF345 :
                p_info_drv->shared_info.ssictrl_cim_value  &= ~(SSICTRL_CIM_SSI3TX_SET | SSICTRL_CIM_SSI345TEN_SET);
                *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
            break;

#endif /* mbed */
            case SCUX_ROUTE_SRC1_SSIF0 :
                p_info_drv->shared_info.ssictrl_cim_value &= ~SSICTRL_CIM_SSI0TX_SET;
                *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                p_scux_info_ch->p_ssif_info1->p_scux_ssif_reg->SSICR &= ~SCUX_SSICR_TEN_SET;
            break;

            case SCUX_ROUTE_SRC1_SSIF012 :
                p_info_drv->shared_info.ssictrl_cim_value  &= ~(SSICTRL_CIM_SSI0TX_SET | SSICTRL_CIM_SSI012TEN_SET);
                *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
            break;

            case SCUX_ROUTE_SRC1_SSIF3 :
                p_info_drv->shared_info.ssictrl_cim_value &= ~SSICTRL_CIM_SSI3TX_SET;
                *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                p_scux_info_ch->p_ssif_info1->p_scux_ssif_reg->SSICR &= ~SCUX_SSICR_TEN_SET;
            break;

#if defined(TARGET_RZA1H) /* mbed */
            case SCUX_ROUTE_SRC1_SSIF345 : 
                p_info_drv->shared_info.ssictrl_cim_value  &= ~(SSICTRL_CIM_SSI3TX_SET | SSICTRL_CIM_SSI345TEN_SET);
                *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
            break;

#endif /* mbed */
            case SCUX_ROUTE_SRC2_SSIF1 :
                p_info_drv->shared_info.ssictrl_cim_value &= ~SSICTRL_CIM_SSI1TX_SET;
                *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                p_scux_info_ch->p_ssif_info1->p_scux_ssif_reg->SSICR &= ~SCUX_SSICR_TEN_SET;
            break;

#if defined(TARGET_RZA1H) /* mbed */
            case SCUX_ROUTE_SRC2_SSIF4 :
                p_info_drv->shared_info.ssictrl_cim_value &= ~SSICTRL_CIM_SSI4TX_SET;
                *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                p_scux_info_ch->p_ssif_info1->p_scux_ssif_reg->SSICR &= ~SCUX_SSICR_TEN_SET;
            break;

#endif /* mbed */
            case SCUX_ROUTE_SRC3_SSIF2 :
                p_info_drv->shared_info.ssictrl_cim_value &= ~SSICTRL_CIM_SSI2TX_SET;
                *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                p_scux_info_ch->p_ssif_info1->p_scux_ssif_reg->SSICR &= ~SCUX_SSICR_TEN_SET;
            break;

#if defined(TARGET_RZA1H) /* mbed */
            case SCUX_ROUTE_SRC3_SSIF5 :
                p_info_drv->shared_info.ssictrl_cim_value &= ~SSICTRL_CIM_SSI5TX_SET;
                *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                p_scux_info_ch->p_ssif_info1->p_scux_ssif_reg->SSICR &= ~SCUX_SSICR_TEN_SET;
            break;

#endif /* mbed */
            case SCUX_ROUTE_SRC0_MIX_SSIF0 :
                if (0U == p_info_drv->shared_info.mix_run_ch)
                {
                    p_info_drv->shared_info.ssictrl_cim_value &= ~SSICTRL_CIM_SSI0TX_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                    p_scux_info_ch->p_ssif_info1->p_scux_ssif_reg->SSICR &= ~SCUX_SSICR_TEN_SET;
                    /* initialized MIX parameter not on going cancel */
                    if (false == p_scux_info_ch->cancel_operate_flag)
                    {
                        p_info_drv->shared_info.mix_ssif_ch = 0;
                    }
                }
            break;

            case SCUX_ROUTE_SRC0_MIX_SSIF012 :
                if (0U == p_info_drv->shared_info.mix_run_ch)
                {
                    p_info_drv->shared_info.ssictrl_cim_value  &= ~(SSICTRL_CIM_SSI0TX_SET | SSICTRL_CIM_SSI012TEN_SET);
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                    /* initialized MIX parameter not on going cancel */
                    if (false == p_scux_info_ch->cancel_operate_flag)
                    {
                        p_info_drv->shared_info.mix_ssif_ch = 0;
                    }
                }
            break;

            case SCUX_ROUTE_SRC0_MIX_SSIF3 :
                if (0U == p_info_drv->shared_info.mix_run_ch)
                {
                    p_info_drv->shared_info.ssictrl_cim_value &= ~SSICTRL_CIM_SSI3TX_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                    p_scux_info_ch->p_ssif_info1->p_scux_ssif_reg->SSICR &= ~SCUX_SSICR_TEN_SET;
                    /* initialized MIX parameter not on going cancel */
                    if (false == p_scux_info_ch->cancel_operate_flag)
                    {
                        p_info_drv->shared_info.mix_ssif_ch = 0;
                    }
                }
            break;

#if defined(TARGET_RZA1H) /* mbed */
            case SCUX_ROUTE_SRC0_MIX_SSIF345 :
                if (0U == p_info_drv->shared_info.mix_run_ch)
                {
                    p_info_drv->shared_info.ssictrl_cim_value  &= ~(SSICTRL_CIM_SSI3TX_SET | SSICTRL_CIM_SSI345TEN_SET);
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                    /* initialized MIX parameter not on going cancel */ 
                    if (false == p_scux_info_ch->cancel_operate_flag)
                    {
                        p_info_drv->shared_info.mix_ssif_ch = 0;
                    }
                }
            break;

#endif /* mbed */
            case SCUX_ROUTE_SRC1_MIX_SSIF0 :
                if (0U == p_info_drv->shared_info.mix_run_ch)
                {
                    p_info_drv->shared_info.ssictrl_cim_value &= ~SSICTRL_CIM_SSI0TX_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                    p_scux_info_ch->p_ssif_info1->p_scux_ssif_reg->SSICR &= ~SCUX_SSICR_TEN_SET;
                    /* initialized MIX parameter not on going cancel */
                    if (false == p_scux_info_ch->cancel_operate_flag)
                    {
                        p_info_drv->shared_info.mix_ssif_ch = 0;
                    }
                }
            break;

            case SCUX_ROUTE_SRC1_MIX_SSIF012 :
                if (0U == p_info_drv->shared_info.mix_run_ch)
                {
                    p_info_drv->shared_info.ssictrl_cim_value  &= ~(SSICTRL_CIM_SSI0TX_SET | SSICTRL_CIM_SSI012TEN_SET);
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                    /* initialized MIX parameter not on going cancel */
                    if (false == p_scux_info_ch->cancel_operate_flag)
                    {
                        p_info_drv->shared_info.mix_ssif_ch = 0;
                    }
                }
            break;

            case SCUX_ROUTE_SRC1_MIX_SSIF3 :
                if (0U == p_info_drv->shared_info.mix_run_ch)
                {
                    p_info_drv->shared_info.ssictrl_cim_value &= ~SSICTRL_CIM_SSI3TX_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                    p_scux_info_ch->p_ssif_info1->p_scux_ssif_reg->SSICR &= ~SCUX_SSICR_TEN_SET;
                    /* initialized MIX parameter not on going cancel */
                    if (false == p_scux_info_ch->cancel_operate_flag)
                    {
                        p_info_drv->shared_info.mix_ssif_ch = 0;
                    }
                }
            break;

#if defined(TARGET_RZA1H) /* mbed */
            case SCUX_ROUTE_SRC1_MIX_SSIF345 :
                if (0U == p_info_drv->shared_info.mix_run_ch)
                {
                    p_info_drv->shared_info.ssictrl_cim_value  &= ~(SSICTRL_CIM_SSI3TX_SET | SSICTRL_CIM_SSI345TEN_SET);
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                    /* initialized MIX parameter not on going cancel */
                    if (false == p_scux_info_ch->cancel_operate_flag)
                    {
                        p_info_drv->shared_info.mix_ssif_ch = 0;
                    }
                }
            break;

#endif /* mbed */
            case SCUX_ROUTE_SRC2_MIX_SSIF0 :
                if (0U == p_info_drv->shared_info.mix_run_ch)
                {
                    p_info_drv->shared_info.ssictrl_cim_value &= ~SSICTRL_CIM_SSI0TX_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                    p_scux_info_ch->p_ssif_info1->p_scux_ssif_reg->SSICR &= ~SCUX_SSICR_TEN_SET;
                    /* initialized MIX parameter not on going cancel */
                    if (false == p_scux_info_ch->cancel_operate_flag)
                    {
                        p_info_drv->shared_info.mix_ssif_ch = 0;
                    }
                }
            break;

            case SCUX_ROUTE_SRC2_MIX_SSIF012 :
                if (0U == p_info_drv->shared_info.mix_run_ch)
                {
                    p_info_drv->shared_info.ssictrl_cim_value  &= ~(SSICTRL_CIM_SSI0TX_SET | SSICTRL_CIM_SSI012TEN_SET);
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                    /* initialized MIX parameter not on going cancel */
                    if (false == p_scux_info_ch->cancel_operate_flag)
                    {
                        p_info_drv->shared_info.mix_ssif_ch = 0;
                    }
                }
            break;

            case SCUX_ROUTE_SRC2_MIX_SSIF3 :
                if (0U == p_info_drv->shared_info.mix_run_ch)
                {
                    p_info_drv->shared_info.ssictrl_cim_value &= ~SSICTRL_CIM_SSI3TX_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                    p_scux_info_ch->p_ssif_info1->p_scux_ssif_reg->SSICR &= ~SCUX_SSICR_TEN_SET;
                    /* initialized MIX parameter not on going cancel */
                    if (false == p_scux_info_ch->cancel_operate_flag)
                    {
                        p_info_drv->shared_info.mix_ssif_ch = 0;
                    }
                }
            break;

#if defined(TARGET_RZA1H) /* mbed */
            case SCUX_ROUTE_SRC2_MIX_SSIF345 :
                if (0U == p_info_drv->shared_info.mix_run_ch)
                {
                    p_info_drv->shared_info.ssictrl_cim_value  &= ~(SSICTRL_CIM_SSI3TX_SET | SSICTRL_CIM_SSI345TEN_SET);
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                    /* initialized MIX parameter not on going cancel */ 
                    if (false == p_scux_info_ch->cancel_operate_flag)
                    {
                        p_info_drv->shared_info.mix_ssif_ch = 0;
                    }
                }
            break;

#endif /* mbed */
            case SCUX_ROUTE_SRC3_MIX_SSIF0 :
                if (0U == p_info_drv->shared_info.mix_run_ch)
                {
                    p_info_drv->shared_info.ssictrl_cim_value &= ~SSICTRL_CIM_SSI0TX_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                    p_scux_info_ch->p_ssif_info1->p_scux_ssif_reg->SSICR &= ~SCUX_SSICR_TEN_SET;
                    /* initialized MIX parameter not on going cancel */
                    if (false == p_scux_info_ch->cancel_operate_flag)
                    {
                        p_info_drv->shared_info.mix_ssif_ch = 0;
                    }
                }
            break;

            case SCUX_ROUTE_SRC3_MIX_SSIF012 :
                if (0U == p_info_drv->shared_info.mix_run_ch)
                {
                    p_info_drv->shared_info.ssictrl_cim_value  &= ~(SSICTRL_CIM_SSI0TX_SET | SSICTRL_CIM_SSI012TEN_SET);
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                    /* initialized MIX parameter not on going cancel */
                    if (false == p_scux_info_ch->cancel_operate_flag)
                    {
                        p_info_drv->shared_info.mix_ssif_ch = 0;
                    }
                }
            break;

            case SCUX_ROUTE_SRC3_MIX_SSIF3 :
                if (0U == p_info_drv->shared_info.mix_run_ch)
                {
                    p_info_drv->shared_info.ssictrl_cim_value &= ~SSICTRL_CIM_SSI3TX_SET;
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                    p_scux_info_ch->p_ssif_info1->p_scux_ssif_reg->SSICR &= ~SCUX_SSICR_TEN_SET;
                    /* initialized MIX parameter not on going cancel */
                    if (false == p_scux_info_ch->cancel_operate_flag)
                    {
                        p_info_drv->shared_info.mix_ssif_ch = 0;
                    }
                }
            break;

#if defined(TARGET_RZA1H) /* mbed */
            case SCUX_ROUTE_SRC3_MIX_SSIF345 :
                if (0U == p_info_drv->shared_info.mix_run_ch)
                {
                    p_info_drv->shared_info.ssictrl_cim_value  &= ~(SSICTRL_CIM_SSI3TX_SET | SSICTRL_CIM_SSI345TEN_SET);
                    *(p_scux_info_ch->p_scux_reg->ssictrl_cim) = p_info_drv->shared_info.ssictrl_cim_value;
                    /* initialized MIX parameter not on going cancel */ 
                    if (false == p_scux_info_ch->cancel_operate_flag)
                    {
                        p_info_drv->shared_info.mix_ssif_ch = 0;
                    }
                }
            break;

#endif /* mbed */
            default :
                /* ->IPA R3.5.2 Nothing is being processed intentionally. */
                /* <-IPA R3.5.2 */
                /* do nothing, when mem to mem route is setting */
            break;
        }

        p_scux_info_ch->p_scux_reg->p_ffd_reg->DEVMR_FFD0_0 &= ~(DEVMR_FFD0_DEVMUF_SET |
                                                                 DEVMR_FFD0_DEVMOF_SET |
                                                                 DEVMR_FFD0_DEVMOL_SET |
                                                                 DEVMR_FFD0_DEVMIUF_SET);
        GIC_DisableIRQ(p_scux_info_ch->int_num[SCUX_INT_FDI]);

        *(p_scux_info_ch->p_scux_reg->fdtsel_n_cim) &= ~FDTSEL_CIM_DIVEN_SET;

        p_scux_info_ch->p_scux_reg->p_ffd_reg->FFDIR_FFD0_0 |= FFDIR_FFD0_INIT_SET;
        p_scux_info_ch->p_scux_reg->p_ipc_reg->IPCIR_IPC0_0 |= IPCIR_IPC0_INIT_SET;

        p_scux_info_ch->p_scux_reg->p_ffd_reg->FFDBR_FFD0_0 &= ~FFDBR_FFD0_BOOT_SET;

        /* check usc_ch ,and init SRC register */
        if ((SCUX_CH_0 == p_scux_info_ch->channel) || (SCUX_CH_2 == p_scux_info_ch->channel))
        {
            p_scux_info_ch->p_scux_reg->p_src_reg->SEVMR0_2SRC0_0 &= ~(SEVMR_2SRC0_EVMUF_SET |
                                                                       SEVMR_2SRC0_EVMOF_SET);
            p_scux_info_ch->p_scux_reg->p_src_reg->SRCIR0_2SRC0_0 |= SRCIR_2SRC0_INIT_SET;
        }
        else
        {
            p_scux_info_ch->p_scux_reg->p_src_reg->SEVMR1_2SRC0_0 &= ~(SEVMR_2SRC0_EVMUF_SET |
                                                                       SEVMR_2SRC0_EVMOF_SET);
            p_scux_info_ch->p_scux_reg->p_src_reg->SRCIR1_2SRC0_0 |= SRCIR_2SRC0_INIT_SET;
        }

        /* if pair channel is stopped, common SRC unit is initialized */
        if (SCUX_CH_0 == p_scux_info_ch->channel)
        {
            p_pair_scux_ch = SCUX_GetDrvChInfo(SCUX_CH_1);
        }
        else if (SCUX_CH_1 == p_scux_info_ch->channel)
        {
            p_pair_scux_ch = SCUX_GetDrvChInfo(SCUX_CH_0);
        }
        else if (SCUX_CH_2 == p_scux_info_ch->channel)
        {
            p_pair_scux_ch = SCUX_GetDrvChInfo(SCUX_CH_3);
        }
        else
        {
            p_pair_scux_ch = SCUX_GetDrvChInfo(SCUX_CH_2);
        }

        if (NULL != p_pair_scux_ch)
        {
            if ((SCUX_CH_UNINIT == p_pair_scux_ch->ch_stat) ||
                (SCUX_CH_INIT == p_pair_scux_ch->ch_stat)   ||
                (SCUX_CH_STOP == p_pair_scux_ch->ch_stat))
            {
                p_scux_info_ch->p_scux_reg->p_src_reg->SRCIRR_2SRC0_0 |= SRCIRR_2SRC0_INIT_SET;
                GIC_DisableIRQ(p_scux_info_ch->int_num[SCUX_INT_AI]);
            }
        }
        else
        {
            /* NON_NOTICE_ASSERT: NULL pointer */
        }

        if (SCUX_ROUTE_MEM_TO_MEM != (p_scux_info_ch->route_set & SCUX_GET_ROUTE_MASK))
        {
            p_scux_info_ch->p_scux_reg->p_dvu_reg->DVUIR_DVU0_0 |= DVUIR_DVU0_INIT_SET;

            p_scux_info_ch->p_scux_reg->p_dvu_reg->VEVMR_DVU0_0 = 0;
            GIC_DisableIRQ(p_scux_info_ch->int_num[SCUX_INT_DVI]);

            scux_check_ch = (uint32_t)p_scux_info_ch->p_ssif_info1->scux_channel;
            scux_check_ch &= ~(1U << p_scux_info_ch->channel);
            p_scux_info_ch->p_ssif_info1->scux_channel = (int_t)scux_check_ch;
            if (0 == p_scux_info_ch->p_ssif_info1->scux_channel)
            {
                core_util_critical_section_enter();

                /* clock mask on all used SSIF channel shutdown */
                CPGSTBCR11 |= ((uint8_t)gb_cpg_scux_ssif_stbcr_bit[p_scux_info_ch->p_ssif_info1->ssif_cfg.ssif_ch_num]);

                core_util_critical_section_exit();
            }

            if (NULL != p_scux_info_ch->p_ssif_info2)
            {
                /* clear multiple SSIF setting */
                scux_check_ch = (uint32_t)p_scux_info_ch->p_ssif_info2->scux_channel;
                scux_check_ch &= ~(1U << p_scux_info_ch->channel);
                p_scux_info_ch->p_ssif_info2->scux_channel = (int_t)scux_check_ch;
                p_scux_info_ch->p_ssif_info2->pin_mode = p_scux_info_ch->p_ssif_info2->pin_mode_backup;

                switch (p_scux_info_ch->p_ssif_info2->ssif_cfg.ssif_ch_num)
                {
                    case SCUX_SSIF_CH_1 :
                        ssipmd_mask_ssif2 = SSIPMD_CIM_SSI1PMD_MASK;
                        ssipmd_shift_ssif2 = SSIPMD_CIM_SSI1PMD_SHIFT;
                    break;

                    case SCUX_SSIF_CH_2 :
                        ssipmd_mask_ssif2 = SSIPMD_CIM_SSI2PMD_MASK;
                        ssipmd_shift_ssif2 = SSIPMD_CIM_SSI2PMD_SHIFT;
                    break;

                    case SCUX_SSIF_CH_3 :
                        ssipmd_mask_ssif2 = SSIPMD_CIM_SSI3PMD_MASK;
                        ssipmd_shift_ssif2 = SSIPMD_CIM_SSI3PMD_SHIFT;
                    break;

#if defined(TARGET_RZA1H) /* mbed */
                    case SCUX_SSIF_CH_4 :
                        ssipmd_mask_ssif2 = SSIPMD_CIM_SSI4PMD_MASK;
                        ssipmd_shift_ssif2 = SSIPMD_CIM_SSI4PMD_SHIFT;
                    break;
                                
                    case SCUX_SSIF_CH_5 :
                        ssipmd_mask_ssif2 = SSIPMD_CIM_SSI5PMD_MASK;
                        ssipmd_shift_ssif2 = SSIPMD_CIM_SSI5PMD_SHIFT;
                    break;

#endif /* mbed */
                    default :
                        /* ->IPA R3.5.2 Nothing is being processed intentionally. */
                        /* <-IPA R3.5.2 */
                        /* NOTREACHED on At the time of a normal performance */
                    break;
                }

                scux_check_ch = (uint32_t)p_scux_info_ch->p_ssif_info3->scux_channel;
                scux_check_ch &= ~(1U << p_scux_info_ch->channel);
                p_scux_info_ch->p_ssif_info3->scux_channel = (int_t)scux_check_ch;
                p_scux_info_ch->p_ssif_info3->pin_mode = p_scux_info_ch->p_ssif_info3->pin_mode_backup;

                switch (p_scux_info_ch->p_ssif_info3->ssif_cfg.ssif_ch_num)
                {
                    case SCUX_SSIF_CH_1 :
                        ssipmd_mask_ssif3 = SSIPMD_CIM_SSI1PMD_MASK;
                        ssipmd_shift_ssif3 = SSIPMD_CIM_SSI1PMD_SHIFT;
                    break;

                    case SCUX_SSIF_CH_2 :
                        ssipmd_mask_ssif3 = SSIPMD_CIM_SSI2PMD_MASK;
                        ssipmd_shift_ssif3 = SSIPMD_CIM_SSI2PMD_SHIFT;
                    break;

                    case SCUX_SSIF_CH_3 :
                        ssipmd_mask_ssif3 = SSIPMD_CIM_SSI3PMD_MASK;
                        ssipmd_shift_ssif3 = SSIPMD_CIM_SSI3PMD_SHIFT;
                    break;

#if defined(TARGET_RZA1H) /* mbed */
                    case SCUX_SSIF_CH_4 :
                        ssipmd_mask_ssif3 = SSIPMD_CIM_SSI4PMD_MASK;
                        ssipmd_shift_ssif3 = SSIPMD_CIM_SSI4PMD_SHIFT;
                    break;

                    case SCUX_SSIF_CH_5 :
                        ssipmd_mask_ssif3 = SSIPMD_CIM_SSI5PMD_MASK;
                        ssipmd_shift_ssif3 = SSIPMD_CIM_SSI5PMD_SHIFT;
                    break;

#endif /* mbed */
                    default :
                        /* ->IPA R3.5.2 Nothing is being processed intentionally. */
                        /* <-IPA R3.5.2 */
                        /* NOTREACHED on At the time of a normal performance */
                    break;
                }
                ssipmd_reg = *(p_scux_info_ch->p_scux_reg->ssipmd_cim);
                ssipmd_reg &= ~(ssipmd_mask_ssif2 | ssipmd_mask_ssif3);

                /* ->IPA R2.4.1 Even if pinmode and ssipmd_shift are max value, omission dose not occur. */
                *(p_scux_info_ch->p_scux_reg->ssipmd_cim) = (ssipmd_reg |
                                                        ((uint32_t)p_scux_info_ch->p_ssif_info2->pin_mode << ssipmd_shift_ssif2) |
                                                        ((uint32_t)p_scux_info_ch->p_ssif_info3->pin_mode << ssipmd_shift_ssif3));
                /* <-IPA R2.4.1 */

                if (0 == p_scux_info_ch->p_ssif_info2->scux_channel)
                {
                    core_util_critical_section_enter();

                    /* clock mask */
                    CPGSTBCR11 |= ((uint8_t)gb_cpg_scux_ssif_stbcr_bit[p_scux_info_ch->p_ssif_info2->ssif_cfg.ssif_ch_num]);

                    core_util_critical_section_exit();
                }

                if (0 == p_scux_info_ch->p_ssif_info3->scux_channel)
                {
                    core_util_critical_section_enter();

                    CPGSTBCR11 |= ((uint8_t)gb_cpg_scux_ssif_stbcr_bit[p_scux_info_ch->p_ssif_info3->ssif_cfg.ssif_ch_num]);

                    core_util_critical_section_exit();
                }

                if (false == p_scux_info_ch->cancel_operate_flag)
                {
                    /* set NULL to SSIF information when cancel not on going */
                    p_scux_info_ch->p_ssif_info2 = NULL;
                    p_scux_info_ch->p_ssif_info3 = NULL;
                }
            }
            if (false == p_scux_info_ch->cancel_operate_flag)
            {
                /* set NULL to SSIF information when cancel not on going */
                p_scux_info_ch->p_ssif_info1 = NULL;
            }

            if (SCUX_ROUTE_MIX == (p_scux_info_ch->route_set & SCUX_GET_ROUTE_MASK))
            {
                if (0U == p_info_drv->shared_info.mix_run_ch)
                {
                    *(p_scux_info_ch->p_scux_reg->mixmr_mix0_0) |= MIXIR_MIX0_INIT_SET;
                    *(p_scux_info_ch->p_scux_reg->mdber_mix0_0) &= ~MDBER_MIX0_MIXDBEN_SET;
                }
            }
        }
    }

    return;
}

/******************************************************************************
End of function SCUX_AsyncStopHw
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_SetDigiVolRegister
* @brief         Set digital volume register.
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        none.
******************************************************************************/

void SCUX_SetDigiVolRegister(const scux_info_ch_t * const p_scux_info_ch)
{
    int_t audio_ch;

    if (NULL == p_scux_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        /* check and set digital volume */
        if (false != p_scux_info_ch->dvu_cfg.dvu_digi_vol.digi_vol_enable)
        {
            p_scux_info_ch->p_scux_reg->p_dvu_reg->DVUCR_DVU0_0 |= DVUCR_DVU0_VVMD_SET;

            for (audio_ch = 0; audio_ch < p_scux_info_ch->src_cfg.use_ch; audio_ch++)
            {
                switch (audio_ch)
                {
                    case SCUX_AUDIO_CH_0 :
                        p_scux_info_ch->p_scux_reg->p_dvu_reg->VOL0R_DVU0_0 = p_scux_info_ch->dvu_cfg.dvu_digi_vol.digi_vol[audio_ch];
                    break;

                    case SCUX_AUDIO_CH_1 :
                        p_scux_info_ch->p_scux_reg->p_dvu_reg->VOL1R_DVU0_0 = p_scux_info_ch->dvu_cfg.dvu_digi_vol.digi_vol[audio_ch];
                    break;

                    case SCUX_AUDIO_CH_2 :
                        p_scux_info_ch->p_scux_reg->p_dvu_reg->VOL2R_DVU0_0 = p_scux_info_ch->dvu_cfg.dvu_digi_vol.digi_vol[audio_ch];
                    break;

                    case SCUX_AUDIO_CH_3 :
                        p_scux_info_ch->p_scux_reg->p_dvu_reg->VOL3R_DVU0_0 = p_scux_info_ch->dvu_cfg.dvu_digi_vol.digi_vol[audio_ch];
                    break;

                    case SCUX_AUDIO_CH_4 :
                        p_scux_info_ch->p_scux_reg->p_dvu_reg->VOL4R_DVU0_0 = p_scux_info_ch->dvu_cfg.dvu_digi_vol.digi_vol[audio_ch];
                    break;

                    case SCUX_AUDIO_CH_5 :
                        p_scux_info_ch->p_scux_reg->p_dvu_reg->VOL5R_DVU0_0 = p_scux_info_ch->dvu_cfg.dvu_digi_vol.digi_vol[audio_ch];
                    break;

                    case SCUX_AUDIO_CH_6 :
                        p_scux_info_ch->p_scux_reg->p_dvu_reg->VOL6R_DVU0_0 = p_scux_info_ch->dvu_cfg.dvu_digi_vol.digi_vol[audio_ch];
                    break;

                    case SCUX_AUDIO_CH_7 :
                        p_scux_info_ch->p_scux_reg->p_dvu_reg->VOL7R_DVU0_0 = p_scux_info_ch->dvu_cfg.dvu_digi_vol.digi_vol[audio_ch];
                    break;

                    default :
                        /* ->IPA R3.5.2 Nothing is being processed intentionally. */
                        /* <-IPA R3.5.2 */
                        /* NOTREACHED on At the time of a normal performance */
                    break;
                }
            }
        }
        else
        {
            p_scux_info_ch->p_scux_reg->p_dvu_reg->DVUCR_DVU0_0 &= ~DVUCR_DVU0_VVMD_SET;
        }
    }

    return;
}

/******************************************************************************
End of function SCUX_SetDigiVolRegister
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_SetRampVolRegister
* @brief         Set ramp volume register.
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        none.
******************************************************************************/

void SCUX_SetRampVolRegister(scux_info_ch_t * const p_scux_info_ch)
{
    int_t audio_ch;
    uint32_t vrctr_value = 0;

    if (NULL == p_scux_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        for (audio_ch = 0; audio_ch < p_scux_info_ch->src_cfg.use_ch; audio_ch++)
        {
            if (false != p_scux_info_ch->dvu_cfg.dvu_ramp_vol.ramp_vol_enable[audio_ch])
            {
                vrctr_value |= (VRCTR_DVU0_VREN_SET << audio_ch);
            }
            else
            {
                vrctr_value &= ~(VRCTR_DVU0_VREN_SET << audio_ch);
            }
        }

        p_scux_info_ch->p_scux_reg->p_dvu_reg->VRCTR_DVU0_0 = vrctr_value;

        if (false == p_scux_info_ch->restart_ramp_flag)
        {
            /* set ramp paramteter to register when timming isn't restart after cancel */
            /* ->IPA R2.4.1 Even if each parameters are max value, omission dose not occur. */
            p_scux_info_ch->p_scux_reg->p_dvu_reg->VRPDR_DVU0_0 = (uint32_t)((((uint32_t)p_scux_info_ch->dvu_cfg.dvu_ramp_vol.up_period << VRPDR_DVU0_VRPDUP_SHIFT) & VRPDR_DVU0_VRPDUP_MASK) |
                                                                  (((uint32_t)p_scux_info_ch->dvu_cfg.dvu_ramp_vol.down_period << VRPDR_DVU0_VRPDDW_SHIFT) & VRPDR_DVU0_VRPDDW_MASK));
            /* <-IPA R2.4.1 */
            p_scux_info_ch->p_scux_reg->p_dvu_reg->VRWTR_DVU0_0 = (p_scux_info_ch->dvu_cfg.dvu_ramp_vol.ramp_wait_time & VRWTR_DVU0_VRWT_MASK);
        }
        else
        {
            /* set fatest ramp parameter to register when timming is restart after cancel */
            /* ->IPA R2.4.1 Even if each parameters are max value, omission dose not occur. */
            /* when restart ,volume is changed immedeatly */
            p_scux_info_ch->p_scux_reg->p_dvu_reg->VRPDR_DVU0_0 = (uint32_t)((((uint32_t)SCUX_DVU_TIME_128DB_1STEP << VRPDR_DVU0_VRPDUP_SHIFT) & VRPDR_DVU0_VRPDUP_MASK) |
                                                                  (((uint32_t)SCUX_DVU_TIME_128DB_1STEP << VRPDR_DVU0_VRPDDW_SHIFT) & VRPDR_DVU0_VRPDDW_MASK));
            /* <-IPA R2.4.1 */
            p_scux_info_ch->p_scux_reg->p_dvu_reg->VRWTR_DVU0_0 = 0U;
            p_scux_info_ch->restart_ramp_flag = false;
        }
        p_scux_info_ch->p_scux_reg->p_dvu_reg->VRDBR_DVU0_0 = p_scux_info_ch->dvu_cfg.dvu_ramp_vol.ramp_vol;
    }

    return;
}

/******************************************************************************
End of function SCUX_SetRampVolRegister
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_SetZerocrossMuteRegister
* @brief         Set zerocross mute register.
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        None.
******************************************************************************/

void SCUX_SetZerocrossMuteRegister(const scux_info_ch_t * const p_scux_info_ch)
{
    int_t    audio_ch;
    uint32_t zcmcr_value = 0;
    uint32_t vevmr_value = 0;

    if (NULL == p_scux_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        for (audio_ch = 0; audio_ch < p_scux_info_ch->src_cfg.use_ch; audio_ch++)
        {
            if (false != p_scux_info_ch->dvu_cfg.dvu_zc_mute.zc_mute_enable[audio_ch])
            {
                zcmcr_value |= (ZCMCR_DVU0_ZCEN_SET << audio_ch);
                vevmr_value |= (VEVMR_DVU0_VEVMZCM_SET << audio_ch);
            }
            else
            {
                zcmcr_value &= ~(ZCMCR_DVU0_ZCEN_SET << audio_ch);
                vevmr_value &= ~(VEVMR_DVU0_VEVMZCM_SET << audio_ch);
            }
        }
        p_scux_info_ch->p_scux_reg->p_dvu_reg->ZCMCR_DVU0_0 = zcmcr_value;
        p_scux_info_ch->p_scux_reg->p_dvu_reg->VEVMR_DVU0_0 = vevmr_value;

        if (0U != zcmcr_value)
        {
            p_scux_info_ch->p_scux_reg->p_dvu_reg->DVUCR_DVU0_0 |= DVUCR_DVU0_ZCMD_SET;
        }
        else
        {
            p_scux_info_ch->p_scux_reg->p_dvu_reg->DVUCR_DVU0_0 &= ~DVUCR_DVU0_ZCMD_SET;
        }

    }

    return;
}

/******************************************************************************
End of function SCUX_SetZerocrossMuteRegister
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_SetMixVolRegister
* @brief         Set MIX volume register.
*
*                Description:<br>
*
* @param[in]     channel : SCUX channel number.
* @retval        None.
******************************************************************************/

void SCUX_SetMixVolRegister(const int_t channel)
{
    scux_info_drv_t * const p_info_drv = SCUX_GetDrvInstance();
    scux_info_ch_t * const p_scux_info_ch = SCUX_GetDrvChInfo(channel);

    if ((NULL == p_info_drv) || (NULL == p_scux_info_ch))
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        *(p_scux_info_ch->p_scux_reg->mdber_mix0_0) &= ~MDBER_MIX0_MIXDBEN_SET;
        *(p_scux_info_ch->p_scux_reg->mdb_n_r_mix0_0) = p_info_drv->shared_info.mix_vol[channel];
        *(p_scux_info_ch->p_scux_reg->mdber_mix0_0) |= MDBER_MIX0_MIXDBEN_SET;
    }

    return;
}

/******************************************************************************
End of function SCUX_SetMixVolRegister
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_DMA_RxCallBack
* @brief         Read request callback (mem to mem).
*
*                Description:<br>
*
* @param[in]     param : SCUX channel number.
* @retval        None.
******************************************************************************/

static void SCUX_DMA_CopyRxCallBack(union sigval const param)
{
    dma_trans_data_t dma_address_param;
    int_t retval;
    scux_info_ch_t * const p_info_ch = SCUX_GetDrvChInfo(param.sival_int);

    if (NULL == p_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        if (NULL != p_info_ch->p_rx_aio)
        {
            p_info_ch->p_rx_aio->aio_return = (ssize_t)p_info_ch->p_rx_aio->aio_nbytes;
            ahf_complete(&p_info_ch->rx_que, p_info_ch->p_rx_aio);
            p_info_ch->first_rx_flag = false;
        }

        p_info_ch->rx_fifo_total_size += p_info_ch->dma_rx_current_size;
        p_info_ch->dma_rx_current_size = 0;
        p_info_ch->p_rx_aio = ahf_removehead(&p_info_ch->rx_que);

        if (NULL != p_info_ch->p_rx_aio)
        {
            /* set nect data read */
            dma_address_param.src_addr = (void *)p_info_ch->p_scux_reg->dmatu_n_cim;
            dma_address_param.dst_addr = (void *)(p_info_ch->p_rx_aio->aio_buf);
            dma_address_param.count = p_info_ch->p_rx_aio->aio_nbytes;

            retval = R_DMA_Start(p_info_ch->dma_rx_ch, &dma_address_param, NULL);
            if (ESUCCESS != retval)
            {
                /* NON_NOTICE_ASSERT: NULL pointer */
            }
            else
            {
                p_info_ch->dma_rx_current_size = dma_address_param.count;
            }
        }
        else
        {
            switch (p_info_ch->ch_stat)
            {
                case SCUX_CH_UNINIT :
                    /* fall through */
                case SCUX_CH_INIT :
                    /* fall through */
                case SCUX_CH_STOP :
                    /* fall through */
                case SCUX_CH_TRANS_IDLE :
                    /* NON_NOTICE_ASSERT : NOTREACHED on At the time of a normal performance */
                break;

                case SCUX_CH_TRANS_RD :
                    p_info_ch->ch_stat = SCUX_CH_TRANS_IDLE;
                break;

                case SCUX_CH_TRANS_WR :
                    /* NON_NOTICE_ASSERT : NOTREACHED on At the time of a normal performance */
                break;

                case SCUX_CH_TRANS_RDWR :
                    p_info_ch->ch_stat = SCUX_CH_TRANS_WR;
                break;

                case SCUX_CH_STOP_WAIT :
                    p_info_ch->ch_stat = SCUX_CH_STOP_WAIT_IDLE;
                break;

                case SCUX_CH_STOP_WAIT_IDLE :
                    /* NON_NOTICE_ASSERT : NOTREACHED on At the time of a normal performance */
                break;

                default :
                    /* NON_NOTICE_ASSERT : NOTREACHED on At the time of a normal performance */
                break;
            }
        }
    }
}

/******************************************************************************
End of function SCUX_DMA_CopyRxCallBack
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_DMA_CommonTxNextDummyData
* @brief         Set next dummy data for flush (mem to mem , SSIF direct route).
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        None.
******************************************************************************/

static void SCUX_DMA_CommonTxNextDummyData(scux_info_ch_t * const p_info_ch)
{
    int_t retval;

    if (NULL == p_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        p_info_ch->tx_fifo_total_size += p_info_ch->dma_tx_current_size;
        p_info_ch->dma_tx_current_size = 0;

        retval = SCUX_FlushWriteStart(p_info_ch);
        if (ESUCCESS != retval)
        {
            /* NON_NOTICE_ASSERT: DMA operation failed */
        }
    }
}

/******************************************************************************
End of function SCUX_DMA_CommonTxNextDummyData
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_DMA_CopyTxEndFlush
* @brief         End of flush operation (mem to mem route).
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        None.
******************************************************************************/

static void SCUX_DMA_CopyTxEndFlush(scux_info_ch_t * const p_info_ch)
{
    int_t retval;
#if(1) /* mbed */
    int32_t  dma_ercd;
#else
    int_t dma_ercd;
#endif
    uint32_t tx_remain_size = 0;
    uint32_t rx_remain_size = 0;

    if (NULL == p_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        /* finish send dummy data process, and SCUX stop process */
        retval = R_DMA_Cancel(p_info_ch->dma_tx_ch, &tx_remain_size, &dma_ercd);
        /* It isn't an error even if error code is EBADF_RBSP, because it is already stopped. */
        if ((ESUCCESS != retval) && (EBADF_RBSP != dma_ercd))
        {
            /* NON_NOTICE_ASSERT: DMA operation failed */
        }

        p_info_ch->tx_fifo_total_size += p_info_ch->dma_tx_current_size;
        p_info_ch->dma_tx_current_size = 0;

        retval = R_DMA_Cancel(p_info_ch->dma_rx_ch, &rx_remain_size, &dma_ercd);
        /* It isn't an error even if error code is EBADF_RBSP, because it is already stopped. */
        if ((ESUCCESS != retval) && (EBADF_RBSP != dma_ercd))
        {
            /* NON_NOTICE_ASSERT: DMA operation failed */
        }
        else
        {
            retval = R_DMA_Free(p_info_ch->dma_rx_ch, NULL);
            if (ESUCCESS != retval)
            {
                /* NON_NOTICE_ASSERT: DMA operation failed */
            }
        }
        p_info_ch->rx_fifo_total_size += p_info_ch->dma_rx_current_size;
        p_info_ch->dma_rx_current_size = 0;

        if (NULL != p_info_ch->p_rx_aio)
        {
            /* in case NULL, dummy data read */
            p_info_ch->p_rx_aio->aio_return = (ssize_t)(p_info_ch->p_rx_aio->aio_nbytes - rx_remain_size);
            ahf_complete(&p_info_ch->rx_que, p_info_ch->p_rx_aio);
        }
        ahf_cancelall(&p_info_ch->rx_que);
        p_info_ch->p_rx_aio = NULL;

        SCUX_AdjustAccessFifo(p_info_ch, tx_remain_size, rx_remain_size);

        if (false != p_info_ch->src_cfg.mode_sync)
        {
            SCUX_SyncStopHw(p_info_ch);
        }
        else
        {
            SCUX_AsyncStopHw(p_info_ch);
        }

        p_info_ch->ch_stat = SCUX_CH_STOP;
        if (NULL != p_info_ch->p_flush_callback)
        {
            p_info_ch->p_flush_callback(ESUCCESS);
        }
    }
}

/******************************************************************************
End of function SCUX_DMA_CopyTxEndFlush
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_DMA_CopyTxNextRemainData
* @brief         Set next remain data for flush (mem to mem route).
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        None.
******************************************************************************/

static void SCUX_DMA_CopyTxNextRemainData(scux_info_ch_t * const p_info_ch)
{
    dma_trans_data_t dma_address_param;
    int_t retval;

    if (NULL == p_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        /* send remain tx data prcess for flush */
        if (NULL != p_info_ch->p_tx_aio)
        {
            p_info_ch->p_tx_aio->aio_return = (ssize_t)p_info_ch->p_tx_aio->aio_nbytes;
            ahf_complete(&p_info_ch->tx_que, p_info_ch->p_tx_aio);
        }

        p_info_ch->tx_fifo_total_size += p_info_ch->dma_tx_current_size;
        p_info_ch->dma_tx_current_size = 0;
        p_info_ch->p_tx_aio = ahf_removehead(&p_info_ch->tx_que);

        if (NULL == p_info_ch->p_tx_aio)
        {
            retval = SCUX_FlushWriteStart(p_info_ch);
            if (ESUCCESS != retval)
            {
                /* NON_NOTICE_ASSERT: DMA operation failed */
            }
        }
        else
        {
            dma_address_param.src_addr = (void *)(p_info_ch->p_tx_aio->aio_buf);
            dma_address_param.dst_addr = (void *)p_info_ch->p_scux_reg->dmatd_n_cim;
            dma_address_param.count = p_info_ch->p_tx_aio->aio_nbytes;

            retval = R_DMA_Start(p_info_ch->dma_tx_ch, &dma_address_param, NULL);
            if (ESUCCESS != retval)
            {
                /* NON_NOTICE_ASSERT: DMA operation failed */
            }
            else
            {
                p_info_ch->dma_tx_current_size = dma_address_param.count;
            }
        }
    }
}

/******************************************************************************
End of function SCUX_DMA_CopyTxNextRemainData
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_DMA_CopyTxNextData
* @brief         Set next data for normal operation (mem to mem route).
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        None.
******************************************************************************/

static void SCUX_DMA_CopyTxNextData(scux_info_ch_t * const p_info_ch)
{
    dma_trans_data_t dma_address_param;
    int_t retval;

    if (NULL == p_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        /* normal send process */
        if (NULL != p_info_ch->p_tx_aio)
        {
            p_info_ch->p_tx_aio->aio_return = (ssize_t)p_info_ch->p_tx_aio->aio_nbytes;
            ahf_complete(&p_info_ch->tx_que, p_info_ch->p_tx_aio);
        }

        p_info_ch->tx_fifo_total_size += p_info_ch->dma_tx_current_size;
        p_info_ch->dma_tx_current_size = 0;
        p_info_ch->p_tx_aio = ahf_removehead(&p_info_ch->tx_que);

        if (NULL != p_info_ch->p_tx_aio)
        {
            dma_address_param.src_addr = (void *)(p_info_ch->p_tx_aio->aio_buf);
            dma_address_param.dst_addr = (void *)p_info_ch->p_scux_reg->dmatd_n_cim;
            dma_address_param.count = p_info_ch->p_tx_aio->aio_nbytes;

            retval = R_DMA_Start(p_info_ch->dma_tx_ch, &dma_address_param, NULL);
            if (ESUCCESS != retval)
            {
                /* NON_NOTICE_ASSERT: NULL pointer */
            }
            else
            {
                p_info_ch->dma_tx_current_size = dma_address_param.count;
            }
        }
        else
        {
            switch (p_info_ch->ch_stat)
            {
                case SCUX_CH_UNINIT :
                    /* fall through */
                case SCUX_CH_INIT :
                    /* fall through */
                case SCUX_CH_STOP :
                    /* fall through */
                case SCUX_CH_TRANS_IDLE :
                    /* fall through */
                case SCUX_CH_TRANS_RD :
                    /* NON_NOTICE_ASSERT : NOTREACHED on At the time of a normal performance */
                break;

                case SCUX_CH_TRANS_WR :
                    p_info_ch->ch_stat = SCUX_CH_TRANS_IDLE;
                break;

                case SCUX_CH_TRANS_RDWR :
                    p_info_ch->ch_stat = SCUX_CH_TRANS_RD;
                break;

                case SCUX_CH_STOP_WAIT :
                    /* fall through */
                case SCUX_CH_STOP_WAIT_IDLE :
                    /* NON_NOTICE_ASSERT : NOTREACHED on At the time of a normal performance */
                break;

                default :
                    /* NOTREACHED on At the time of a normal performance */
                break;
            }
        }
    }
}

/******************************************************************************
End of function SCUX_DMA_CopyTxNextData
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_DMA_CopyTxCallBack
* @brief         Write request callback (mem to mem route).
*
*                Description:<br>
*
* @param[in]     param : SCUX channel number.
* @retval        None.
******************************************************************************/

static void SCUX_DMA_CopyTxCallBack(union sigval const param)
{
    scux_info_ch_t * const p_info_ch = SCUX_GetDrvChInfo(param.sival_int);

    if (NULL == p_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        if ((SCUX_CH_STOP_WAIT == p_info_ch->ch_stat) || (SCUX_CH_STOP_WAIT_IDLE == p_info_ch->ch_stat))
        {
            /* data flush process */
            if (false != p_info_ch->tx_dummy_run_flag)
            {
                if (0U == p_info_ch->flush_stop_size)
                {
                    SCUX_DMA_CopyTxEndFlush(p_info_ch);
                }
                else
                {
                    SCUX_DMA_CommonTxNextDummyData(p_info_ch);
                }
            }
            else
            {
                SCUX_DMA_CopyTxNextRemainData(p_info_ch);
            }
        }
        else
        {
            SCUX_DMA_CopyTxNextData(p_info_ch);
        }
    }
}

/******************************************************************************
End of function SCUX_DMA_CopyTxCallBack
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_DMA_DirectTxEndFlush
* @brief         End of flush operation (SSIF Direct route).
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        None.
******************************************************************************/

static void SCUX_DMA_DirectTxEndFlush(scux_info_ch_t * const p_info_ch)
{
    int_t retval;
#if(1) /* mbed */
    int32_t  dma_ercd;
#else
    int_t dma_ercd;
#endif
    uint32_t tx_remain_size = 0;

    if (NULL == p_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        /* finish send dummy data process, and SCUX stop process */
        retval = R_DMA_Cancel(p_info_ch->dma_tx_ch, &tx_remain_size, &dma_ercd);
        /* It isn't an error even if error code is EBADF_RBSP, because it is already stopped. */
        if ((ESUCCESS != retval) && (EBADF_RBSP != dma_ercd))
        {
            /* NON_NOTICE_ASSERT: NULL pointer */
        }
        p_info_ch->tx_fifo_total_size += p_info_ch->dma_tx_current_size;
        p_info_ch->dma_tx_current_size = 0;

        SCUX_AdjustAccessFifo(p_info_ch, tx_remain_size, 0);

        SCUX_AsyncStopHw(p_info_ch);

        p_info_ch->ch_stat = SCUX_CH_STOP;
        if (NULL != p_info_ch->p_flush_callback)
        {
            p_info_ch->p_flush_callback(ESUCCESS);
        }
    }
}

/******************************************************************************
End of function SCUX_DMA_DirectTxEndFlush
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_DMA_DirectTxNextRemainData
* @brief         Set next remain data for flush (SSIF Direct route).
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        None.
******************************************************************************/

static void SCUX_DMA_DirectTxNextRemainData(scux_info_ch_t * const p_info_ch)
{
    dma_trans_data_t dma_next_address_param;
    int_t retval;

    if (NULL == p_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        if (NULL != p_info_ch->p_tx_aio)
        {
            p_info_ch->p_tx_aio->aio_return = (ssize_t)p_info_ch->p_tx_aio->aio_nbytes;
            ahf_complete(&p_info_ch->tx_que, p_info_ch->p_tx_aio);
            p_info_ch->first_tx_flag = false;
        }


        /* send remain tx data prcess for flush */
        p_info_ch->p_tx_aio = p_info_ch->p_tx_next_aio;
        p_info_ch->p_tx_next_aio = ahf_removehead(&p_info_ch->tx_que);
        p_info_ch->tx_fifo_total_size += p_info_ch->dma_tx_current_size;
        p_info_ch->dma_tx_current_size = 0;

        if ((NULL != p_info_ch->p_tx_next_aio) && (NULL != p_info_ch->p_tx_aio))
        {
           /* 1.
            * p_info_ch->p_tx_next_aio isn't NULL, and p_info_ch->p_tx_aio isn't NULL.
            * Since, Next DMA is effective and p_info_ch->p_tx_aio is set to Next DMA
            * The vaule of p_info_ch->dma_tx_current_size is updated to the value of
            * p_info_ch->dma_tx_next_size.
            * The vaule of p_info_ch->dma_tx_next_size is updated to the value of DMA size
            * on Next DMA.
            */
            dma_next_address_param.src_addr = (void *)(p_info_ch->p_tx_next_aio->aio_buf);
            dma_next_address_param.dst_addr = (void *)p_info_ch->p_scux_reg->dmatd_n_cim;
            dma_next_address_param.count = p_info_ch->p_tx_next_aio->aio_nbytes;

            retval = R_DMA_NextData(p_info_ch->dma_tx_ch, &dma_next_address_param, NULL);
            if (ESUCCESS != retval)
            {
                /* NON_NOTICE_ASSERT: NULL pointer */
            }
            else
            {
                p_info_ch->dma_tx_current_size = p_info_ch->dma_tx_next_size;
                p_info_ch->dma_tx_next_size = dma_next_address_param.count;
            }
        }
        else if ((NULL != p_info_ch->p_tx_next_aio) && (NULL == p_info_ch->p_tx_aio))
        {
           /* 2.
            * p_info_ch->p_tx_next_aio isn't NULL, and p_info_ch->p_tx_aio is NULL.
            * Since, Next DMA is Stopped and DMA is restarted to p_info_ch->p_tx_aio
            * The vaule of p_info_ch->dma_tx_current_size and p_info_ch->dma_tx_next_size
            * are updated to restart DMA value.
            */
            retval = SCUX_DirectWriteStart(p_info_ch, p_info_ch->p_tx_next_aio);
            if (ESUCCESS != retval)
            {
                /* NON_NOTICE_ASSERT: NULL pointer */
            }
        }
        else if ((NULL == p_info_ch->p_tx_next_aio) && (NULL == p_info_ch->p_tx_aio))
        {
            /* 3.
             * p_info_ch->p_tx_next_aio is NULL, and p_info_ch->p_tx_aio is NULL.
             * Since, all request finished, and flush data write start.
             * The vaule of p_info_ch->dma_tx_current_size is updated to flush DMA value.
             */

            retval = SCUX_FlushWriteStart(p_info_ch);
            if (ESUCCESS != retval)
            {
                /* NON_NOTICE_ASSERT: DMA operation failed */
            }
        }
        else
        {
           /* 4.
            * p_info_ch->p_tx_next_aio is NULL, and p_info_ch->p_tx_aio isn't NULL.
            * Since, Nothing new aio request, and processing of 2. or 3. is performed
            * in the next loop.
            * The vaule of p_info_ch->dma_tx_current_size is updated to the value of
            * p_info_ch->dma_tx_next_size.
            */

            p_info_ch->dma_tx_current_size = p_info_ch->dma_tx_next_size;
            p_info_ch->dma_tx_next_size = 0;
        }
    }
}

/******************************************************************************
End of function SCUX_DMA_DirectTxNextRemainData
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_DMA_DirectTxNextData
* @brief         Set next data normal operation (SSIF Direct route).
*
*                Description:<br>
*
* @param[in]     *p_scux_info_ch : SCUX channel information.
* @retval        None.
******************************************************************************/

static void SCUX_DMA_DirectTxNextData(scux_info_ch_t * const p_info_ch)
{
    dma_trans_data_t dma_next_address_param;
    int_t retval;

    if (NULL == p_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        if (NULL != p_info_ch->p_tx_aio)
        {
            p_info_ch->p_tx_aio->aio_return = (ssize_t)p_info_ch->p_tx_aio->aio_nbytes;
            ahf_complete(&p_info_ch->tx_que, p_info_ch->p_tx_aio);
            p_info_ch->first_tx_flag = false;
        }

        /* normal send process */
        p_info_ch->p_tx_aio = p_info_ch->p_tx_next_aio;
        p_info_ch->tx_fifo_total_size += p_info_ch->dma_tx_current_size;
        p_info_ch->p_tx_next_aio = ahf_removehead(&p_info_ch->tx_que);

        if ((NULL != p_info_ch->p_tx_next_aio) && (NULL != p_info_ch->p_tx_aio))
        {
            /* 1.
             * p_info_ch->p_tx_next_aio isn't NULL, and p_info_ch->p_tx_aio isn't NULL.
.            * Since, Next DMA is effective and p_info_ch->p_tx_aio is set to Next DMA
             * The vaule of p_info_ch->dma_tx_current_size is updated to the value of
             * p_info_ch->dma_tx_next_size.
             * The vaule of p_info_ch->dma_tx_next_size is updated to the value of DMA size
             * on Next DMA.
             */
            dma_next_address_param.src_addr = (void *)(p_info_ch->p_tx_next_aio->aio_buf);
            dma_next_address_param.dst_addr = (void *)p_info_ch->p_scux_reg->dmatd_n_cim;
            dma_next_address_param.count = p_info_ch->p_tx_next_aio->aio_nbytes;

            retval = R_DMA_NextData(p_info_ch->dma_tx_ch, &dma_next_address_param, NULL);
            if (ESUCCESS != retval)
            {
                /* NON_NOTICE_ASSERT: NULL pointer */
            }
            else
            {
                p_info_ch->dma_tx_current_size = p_info_ch->dma_tx_next_size;
                p_info_ch->dma_tx_next_size = dma_next_address_param.count;
            }
        }
        else if ((NULL != p_info_ch->p_tx_next_aio) && (NULL == p_info_ch->p_tx_aio))
        {
            /* 2.
             * p_info_ch->p_tx_next_aio isn't NULL, and p_info_ch->p_tx_aio is NULL.
             * Since, Next DMA is Stopped and DMA is restarted to p_info_ch->p_tx_aio
             * The vaule of p_info_ch->dma_tx_current_size and p_info_ch->dma_tx_next_size
             * are updated to restart DMA value.
             */

            retval = SCUX_DirectWriteStart(p_info_ch, p_info_ch->p_tx_next_aio);
            if (ESUCCESS != retval)
            {
                /* NON_NOTICE_ASSERT: NULL pointer */
            }
        }
        else if((NULL == p_info_ch->p_tx_next_aio) && (NULL == p_info_ch->p_tx_aio))
        {
            /* 3.
             * p_info_ch->p_tx_next_aio is NULL, and p_info_ch->p_tx_aio is NULL.
             * Since, all request finished, and status is made to change IDLE.
             * The vaule of p_info_ch->dma_tx_current_size is updated to 0.
             */
            p_info_ch->ch_stat = SCUX_CH_TRANS_IDLE;
            p_info_ch->dma_tx_current_size = 0;

        }
        else
        {
            /* 4.
             * p_info_ch->p_tx_next_aio is NULL, and p_info_ch->p_tx_aio isn't NULL.
             * Since, Nothing new aio request, and processing of 2. or 3. is performed
             * in the next loop.
             * The vaule of p_info_ch->dma_tx_current_size is updated to the value of
             * p_info_ch->dma_tx_next_size.
             */

            p_info_ch->dma_tx_current_size = p_info_ch->dma_tx_next_size;
            p_info_ch->dma_tx_next_size = 0;
        }
    }
}

/******************************************************************************
End of function SCUX_DMA_DirectTxNextData
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_DMA_DirectTxCallBack
* @brief         Write request callback (SSIF Direct route).
*
*                Description:<br>
*
* @param[in]     param : SCUX channel number.
* @retval        None.
******************************************************************************/

static void SCUX_DMA_DirectTxCallBack(union sigval const param)
{
    scux_info_ch_t * const p_info_ch = SCUX_GetDrvChInfo(param.sival_int);

    if (NULL == p_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        if (SCUX_CH_STOP_WAIT == p_info_ch->ch_stat)
        {
            /* data flush process */
            if (false != p_info_ch->tx_dummy_run_flag)
            {
                if (0U == p_info_ch->flush_stop_size)
                {
                    SCUX_DMA_DirectTxEndFlush(p_info_ch);
                }
                else
                {
                    SCUX_DMA_CommonTxNextDummyData(p_info_ch);
                }
            }
            else
            {
                SCUX_DMA_DirectTxNextRemainData(p_info_ch);
            }
        }
        else
        {
            SCUX_DMA_DirectTxNextData(p_info_ch);
        }
    }
}

/******************************************************************************
End of function SCUX_DMA_DirextTxCallBack
******************************************************************************/

