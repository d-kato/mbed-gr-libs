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

/*******************************************************************************
* File Name   : ssif.c
* $Rev: $
* $Date::                           $
* Description : SSIF driver functions
******************************************************************************/

/*******************************************************************************
Includes <System Includes>, "Project Includes"
*******************************************************************************/
#include "ssif.h"
#include "iodefine.h"
#include "ssif_int.h"
#include "cmsis.h"
#include "mbed_critical.h"

/*******************************************************************************
Macro definitions
*******************************************************************************/

/*******************************************************************************
Typedef definitions
*******************************************************************************/

/*******************************************************************************
Exported global variables (to be accessed by other files)
*******************************************************************************/
/* ->MISRA 8.8, MISRA 8.10, IPA M2.2.2 : These declare statements are dependent on CMSIS-RTOS */
osSemaphoreDef(ssif_ch0_access);
osSemaphoreDef(ssif_ch1_access);
osSemaphoreDef(ssif_ch2_access);
osSemaphoreDef(ssif_ch3_access);
#if defined(TARGET_RZA1H) /* mbed */
osSemaphoreDef(ssif_ch4_access);
osSemaphoreDef(ssif_ch5_access);
#endif /* mbed */
/* <-MISRA 8.8, MISRA 8.10, IPA M2.2.2 */

#if(1) /* mbed */
static ssif_info_drv_t g_ssif_info_drv;

ssif_info_drv_t * SSIF_GetDrvInstanc(void) {
    return &g_ssif_info_drv;
}
#else
ssif_info_drv_t g_ssif_info_drv;
#endif

volatile struct st_ssif* const g_ssireg[SSIF_NUM_CHANS] = SSIF_ADDRESS_LIST;

/*******************************************************************************
Private global variables and functions
*******************************************************************************/
static int_t SSIF_InitChannel(ssif_info_ch_t* const p_info_ch);
static void SSIF_UnInitChannel(ssif_info_ch_t* const p_info_ch);
static int_t SSIF_UpdateChannelConfig(ssif_info_ch_t* const p_info_ch,
                                      const ssif_channel_cfg_t* const p_ch_cfg);
static int_t SSIF_SetCtrlParams(const ssif_info_ch_t* const p_info_ch);
static int_t SSIF_CheckChannelCfg(const ssif_channel_cfg_t* const p_ch_cfg);
static int_t SSIF_CheckWordSize(const ssif_info_ch_t* const p_info_ch);
static void SSIF_Reset(const uint32_t ssif_ch);

static const uint32_t gb_cpg_stbcr_bit[SSIF_NUM_CHANS] =
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

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/

#if(1) /* mbed */
/******************************************************************************
* Function Name: SSIF_InitialiseOne
* @brief         Initialize the SSIF driver's internal data
*
*                Description:<br>
*
* @param[in]     channel    :channel number
* @param[in]     p_cfg_data :pointer of several parameters array per channels
* @retval        ESUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
int_t SSIF_InitialiseOne(const int_t channel, const ssif_channel_cfg_t* const p_cfg_data)
{
    int_t           ercd = ESUCCESS;
    ssif_info_ch_t* p_info_ch;

    if (NULL == p_cfg_data)
    {
        ercd = EFAULT_RBSP;
    }
    else if (false == p_cfg_data->enabled)
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        p_info_ch = &g_ssif_info_drv.info_ch[channel];
        p_info_ch->channel = channel;
        p_info_ch->enabled = p_cfg_data->enabled;

        /* copy config data to channel info */
        ercd = SSIF_UpdateChannelConfig(p_info_ch, p_cfg_data);

        if (ESUCCESS == ercd)
        {
            ercd = SSIF_InitChannel(p_info_ch);
        }

        if (ESUCCESS == ercd)
        {
            SSIF_InterruptInit(channel, p_cfg_data->int_level);
        }
    }

    return ercd;
}

/******************************************************************************
* Function Name: SSIF_UnInitialiseOne
* @brief         UnInitialize the SSIF driver's internal data
*
*                Description:<br>
*
* @param[in]     channel    :channel number
* @retval        ESUCCESS   :Success.
******************************************************************************/
int_t SSIF_UnInitialiseOne(const int_t channel)
{
    const int_t     ercd = ESUCCESS;
    ssif_info_ch_t* p_info_ch;

    p_info_ch = &g_ssif_info_drv.info_ch[channel];

    if (false != p_info_ch->enabled)
    {
        SSIF_UnInitChannel(p_info_ch);
    }

    return ercd;
}

#endif /* end mbed */
/******************************************************************************
* Function Name: SSIF_Initialise
* @brief         Initialize the SSIF driver's internal data
*
*                Description:<br>
*
* @param[in]     p_cfg_data :pointer of several parameters array per channels
* @retval        ESUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
int_t SSIF_Initialise(const ssif_channel_cfg_t* const p_cfg_data)
{
    uint32_t        ssif_ch;
    int_t           ercd = ESUCCESS;
    ssif_info_ch_t* p_info_ch;

    if (NULL == p_cfg_data)
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        for (ssif_ch = 0; (ssif_ch < SSIF_NUM_CHANS) && (ESUCCESS == ercd); ssif_ch++)
        {
            p_info_ch = &g_ssif_info_drv.info_ch[ssif_ch];
            p_info_ch->channel = ssif_ch;
            p_info_ch->enabled = p_cfg_data[ssif_ch].enabled;

            if (false != p_info_ch->enabled)
            {
                /* copy config data to channel info */
                ercd = SSIF_UpdateChannelConfig(p_info_ch, &p_cfg_data[ssif_ch]);

                if (ESUCCESS == ercd)
                {
                    ercd = SSIF_InitChannel(p_info_ch);
                }
            }
        }

        if (ESUCCESS == ercd)
        {
            for (ssif_ch = 0; ssif_ch < SSIF_NUM_CHANS; ssif_ch++)
            {
                p_info_ch = &g_ssif_info_drv.info_ch[ssif_ch];

                if (false != p_info_ch->enabled)
                {
                    SSIF_InterruptInit(ssif_ch, p_cfg_data[ssif_ch].int_level);
                }
            }
        }
    }

    return ercd;
}

/******************************************************************************
* Function Name: SSIF_UnInitialise
* @brief         UnInitialize the SSIF driver's internal data
*
*                Description:<br>
*
* @param         none
* @retval        ESUCCESS   :Success.
******************************************************************************/
int_t SSIF_UnInitialise(void)
{
    uint32_t        ssif_ch;
    const int_t     ercd = ESUCCESS;
    ssif_info_ch_t* p_info_ch;

    for (ssif_ch = 0; ssif_ch < SSIF_NUM_CHANS; ssif_ch++)
    {
        p_info_ch = &g_ssif_info_drv.info_ch[ssif_ch];

        if (false != p_info_ch->enabled)
        {
            SSIF_UnInitChannel(p_info_ch);
        }
    }

    return ercd;
}

/******************************************************************************
* Function Name: SSIF_EnableChannel
* @brief         Enable the SSIF channel
*
*                Description:<br>
*
* @param[in,out] p_info_ch  :channel object
* @retval        ESUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
int_t SSIF_EnableChannel(ssif_info_ch_t* const p_info_ch)
{
    int_t ercd = ESUCCESS;
    uint32_t ssif_ch;
    volatile uint8_t dummy_buf;

    if (NULL == p_info_ch)
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        ssif_ch = p_info_ch->channel;

        if (ssif_ch >= SSIF_NUM_CHANS)
        {
            ercd = EFAULT_RBSP;
        }
        else
        {
            /* check channel open flag(duplex) */
            if ((O_RDWR_RBSP == p_info_ch->openflag)
                && (false == p_info_ch->is_full_duplex))
            {
                ercd = EINVAL_RBSP;
            }

#if defined(TARGET_RZA1H) /* mbed */
            /* check channel open flag(romdec direct input) */
            if (ESUCCESS == ercd)
            {
                if ((O_RDONLY_RBSP != p_info_ch->openflag)
                    && (SSIF_CFG_ENABLE_ROMDEC_DIRECT
                        == p_info_ch->romdec_direct.mode))
                {
                    ercd = EINVAL_RBSP;
                }
            }

#endif /* mbed */
            /* enable the SSIF clock */
            if (ESUCCESS == ercd)
            {
                core_util_critical_section_enter();

                /* ->IPA R2.4.2 : This is implicit type conversion that doesn't have bad effect on writing to 8bit register. */
                CPGSTBCR11 &= (uint8_t)~((uint8_t)gb_cpg_stbcr_bit[ssif_ch]);
                /* <-IPA R2.4.2 */
                dummy_buf = CPGSTBCR11;
#if(1) /* mbed */
                (void)dummy_buf;
#endif

                core_util_critical_section_exit();
            }

            /* configure channel hardware */
            if (ESUCCESS == ercd)
            {
                /* software reset */
                SSIF_Reset(ssif_ch);

                /* Set control parameters */
                ercd = SSIF_SetCtrlParams(p_info_ch);
            }

            /* allocate and setup/start DMA transfer */
            if (ESUCCESS == ercd)
            {
                ercd = SSIF_InitDMA(p_info_ch);
            }
        }
    }

    return ercd;
}

/******************************************************************************
* Function Name: SSIF_DisableChannel
* @brief         Disable the SSIF channel
*
*                Description:<br>
*
* @param[in,out] p_info_ch  :channel object
* @retval        ESUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
int_t SSIF_DisableChannel(ssif_info_ch_t* const p_info_ch)
{
    uint32_t dummy_read;
    int_t   ret = ESUCCESS;
    uint32_t ssif_ch;

    if (NULL == p_info_ch)
    {
        ret = EFAULT_RBSP;
    }
    else
    {
        ssif_ch = p_info_ch->channel;

        if (ssif_ch >= SSIF_NUM_CHANS)
        {
            ret = EFAULT_RBSP;
        }
        else
        {
            SSIF_DisableErrorInterrupt(ssif_ch);

            /* TEN and REN are disable */
            g_ssireg[ssif_ch]->SSICR &= ~(SSIF_CR_BIT_TEN | SSIF_CR_BIT_REN);

            /* Reset FIFO */
            g_ssireg[ssif_ch]->SSIFCR |= (SSIF_FCR_BIT_TFRST | SSIF_FCR_BIT_RFRST);
            dummy_read = g_ssireg[ssif_ch]->SSIFCR;
            UNUSED_ARG(dummy_read);
            g_ssireg[ssif_ch]->SSIFCR &= ~(SSIF_FCR_BIT_TFRST | SSIF_FCR_BIT_RFRST);

            /* free DMA resources */
            SSIF_UnInitDMA(p_info_ch);

            /* clear status reg */
            g_ssireg[ssif_ch]->SSISR = 0u; /* ALL CLEAR */

            /* disable ssif clock */
            core_util_critical_section_enter();

            /* ->IPA R2.4.2 : This is implicit type conversion that doesn't have bad effect on writing to 8bit register. */
            CPGSTBCR11 |= (uint8_t)gb_cpg_stbcr_bit[ssif_ch];
            /* <-IPA R2.4.2 */

            core_util_critical_section_exit();

            /* cancel event to ongoing request */
            if (NULL != p_info_ch->p_aio_tx_curr)
            {
#if(1) /* mbed */
                p_info_ch->p_aio_tx_curr->aio_return = -1;
#else
                p_info_ch->p_aio_tx_curr->aio_return = ECANCELED;
#endif
                ahf_complete(&p_info_ch->tx_que, p_info_ch->p_aio_tx_curr);
                p_info_ch->p_aio_tx_curr = NULL;
            }
            if (NULL != p_info_ch->p_aio_tx_next)
            {
#if(1) /* mbed */
                p_info_ch->p_aio_tx_next->aio_return = -1;
#else
                p_info_ch->p_aio_tx_next->aio_return = ECANCELED_RBSP;
#endif
                ahf_complete(&p_info_ch->tx_que, p_info_ch->p_aio_tx_next);
                p_info_ch->p_aio_tx_next = NULL;
            }
            if (NULL != p_info_ch->p_aio_rx_curr)
            {
#if(1) /* mbed */
                p_info_ch->p_aio_rx_curr->aio_return = -1;
#else
                p_info_ch->p_aio_rx_curr->aio_return = ECANCELED;
#endif
                ahf_complete(&p_info_ch->rx_que, p_info_ch->p_aio_rx_curr);
                p_info_ch->p_aio_rx_curr = NULL;
            }
            if (NULL != p_info_ch->p_aio_rx_next)
            {
#if(1) /* mbed */
                p_info_ch->p_aio_rx_next->aio_return = -1;
#else
                p_info_ch->p_aio_rx_next->aio_return = ECANCELED;
#endif
                ahf_complete(&p_info_ch->rx_que, p_info_ch->p_aio_rx_next);
                p_info_ch->p_aio_rx_next = NULL;
            }
        }
    }

    return ret;
}

/******************************************************************************
* Function Name: SSIF_ErrorRecovery
* @brief         Restart the SSIF channel
*
*                Description:<br>
*                When normal mode<br>
*                  Stop and restart DMA transfer.<br>
*                When ROMDEC direct input mode<br>
*                  Stop DMA transfer, and execute callback function.<br>
*                Note: This function execute in interrupt context.
* @param[in,out] p_info_ch  :channel object
* @retval        none
******************************************************************************/
void SSIF_ErrorRecovery(ssif_info_ch_t* const p_info_ch)
{
    uint32_t dummy_read;
    int_t   ercd = ESUCCESS;
    uint32_t ssif_ch;

    if (NULL == p_info_ch)
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        ssif_ch = p_info_ch->channel;

        if (ssif_ch >= SSIF_NUM_CHANS)
        {
            ercd = EFAULT_RBSP;
        }
        else
        {
            /* disable DMA end interrupt */
            g_ssireg[ssif_ch]->SSIFCR &= ~((uint32_t)SSIF_FCR_BIT_TIE | SSIF_FCR_BIT_RIE);

            SSIF_DisableErrorInterrupt(ssif_ch);

            /* TEN and REN are disable */
            g_ssireg[ssif_ch]->SSICR &= ~(SSIF_CR_BIT_TEN | SSIF_CR_BIT_REN);

            /* Reset FIFO */
            g_ssireg[ssif_ch]->SSIFCR |= (SSIF_FCR_BIT_TFRST | SSIF_FCR_BIT_RFRST);
            dummy_read = g_ssireg[ssif_ch]->SSIFCR;
            UNUSED_ARG(dummy_read);
            g_ssireg[ssif_ch]->SSIFCR &= ~(SSIF_FCR_BIT_TFRST | SSIF_FCR_BIT_RFRST);

            /* pause DMA transfer */
            SSIF_CancelDMA(p_info_ch);

            /* clear status reg */
            g_ssireg[ssif_ch]->SSISR = 0u; /* ALL CLEAR */

            /* cancel event to ongoing request */
            if (NULL != p_info_ch->p_aio_tx_curr)
            {
#if(1) /* mbed */
                p_info_ch->p_aio_tx_curr->aio_return = -1;
#else
                p_info_ch->p_aio_tx_curr->aio_return = EIO;
#endif
                ahf_complete(&p_info_ch->tx_que, p_info_ch->p_aio_tx_curr);
                p_info_ch->p_aio_tx_curr = NULL;
            }
            if (NULL != p_info_ch->p_aio_tx_next)
            {
#if(1) /* mbed */
                p_info_ch->p_aio_tx_next->aio_return = -1;
#else
                p_info_ch->p_aio_tx_next->aio_return = EIO;
#endif
                ahf_complete(&p_info_ch->tx_que, p_info_ch->p_aio_tx_next);
                p_info_ch->p_aio_tx_next = NULL;
            }
            if (NULL != p_info_ch->p_aio_rx_curr)
            {
#if(1) /* mbed */
                p_info_ch->p_aio_rx_curr->aio_return = -1;
#else
                p_info_ch->p_aio_rx_curr->aio_return = EIO;
#endif
                ahf_complete(&p_info_ch->rx_que, p_info_ch->p_aio_rx_curr);
                p_info_ch->p_aio_rx_curr = NULL;
            }
            if (NULL != p_info_ch->p_aio_rx_next)
            {
#if(1) /* mbed */
                p_info_ch->p_aio_rx_next->aio_return = -1;
#else
                p_info_ch->p_aio_rx_next->aio_return = EIO;
#endif
                ahf_complete(&p_info_ch->rx_que, p_info_ch->p_aio_rx_next);
                p_info_ch->p_aio_rx_next = NULL;
            }
        }

        /* configure channel hardware */
        if (ESUCCESS == ercd)
        {
            /* software reset */
            SSIF_Reset(ssif_ch);

            /* Set control parameters */
            ercd = SSIF_SetCtrlParams(p_info_ch);
        }

        if (ESUCCESS == ercd)
        {
#if defined(TARGET_RZA1H) /* mbed */
            if (SSIF_CFG_ENABLE_ROMDEC_DIRECT
                != p_info_ch->romdec_direct.mode)
            {
                /* setup/restart DMA transfer */
                ercd = SSIF_RestartDMA(p_info_ch);
            }
            else
            {
                /* execute callback function */
                if (NULL != p_info_ch->romdec_direct.p_cbfunc)
                {
                    (*p_info_ch->romdec_direct.p_cbfunc)();
                }
            }
#else /* mbed */
            /* setup/restart DMA transfer */
            ercd = SSIF_RestartDMA(p_info_ch);
#endif /* mbed */
        }
    }

    if (ESUCCESS != ercd)
    {
        /* NON_NOTICE_ASSERT: cannot restart channel */
    }

    return;
}

/******************************************************************************
* Function Name: SSIF_PostAsyncIo
* @brief         Enqueue asynchronous read/write request
*
*                Description:<br>
*
* @param[in,out] p_info_ch  :channel object
* @param[in,out] p_aio      :aio control block of read/write request
* @retval        none
******************************************************************************/
void SSIF_PostAsyncIo(ssif_info_ch_t* const p_info_ch, AIOCB* const p_aio)
{
    if ((NULL == p_info_ch) || (NULL == p_aio))
    {
        /* NON_NOTICE_ASSERT: illegal pointer */
    }
    else
    {
        if (SSIF_ASYNC_W == p_aio->aio_return)
        {
            ahf_addtail(&p_info_ch->tx_que, p_aio);
        }
        else if (SSIF_ASYNC_R == p_aio->aio_return)
        {
            ahf_addtail(&p_info_ch->rx_que, p_aio);
        }
        else
        {
            /* NON_NOTICE_ASSERT: illegal request type */
        }
    }

    return;
}

/******************************************************************************
* Function Name: SSIF_PostAsyncCancel
* @brief         Cancel read or write request(s)
*
*                Description:<br>
*
* @param[in,out] p_info_ch  :channel object
* @param[in,out] p_aio      :aio control block to cancel or NULL to cancel all.
* @retval        none
******************************************************************************/
void SSIF_PostAsyncCancel(ssif_info_ch_t* const p_info_ch, AIOCB* const p_aio)
{
    int32_t ioif_ret;

    if (NULL == p_info_ch)
    {
        /* NON_NOTICE_ASSERT: illegal pointer */
    }
    else
    {
        if (NULL == p_aio)
        {
            ahf_cancelall(&p_info_ch->tx_que);
            ahf_cancelall(&p_info_ch->rx_que);
        }
        else
        {
            ioif_ret = ahf_cancel(&p_info_ch->tx_que, p_aio);
            if (ESUCCESS != ioif_ret)
            {
                /* NON_NOTICE_ASSERT: unexpected aioif error */
            }

            ioif_ret = ahf_cancel(&p_info_ch->rx_que, p_aio);
            if (ESUCCESS != ioif_ret)
            {
                /* NON_NOTICE_ASSERT: unexpected aioif error */
            }
        }
    }

    return;
}

/******************************************************************************
* Function Name: SSIF_IOCTL_ConfigChannel
* @brief         Save configuration to the SSIF driver.
*
*                Description:<br>
*                Update channel object.
* @param[in,out] p_info_ch  :channel object
* @param[in]     p_ch_cfg   :SSIF channel configuration parameter
* @retval        ESUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
int_t SSIF_IOCTL_ConfigChannel(ssif_info_ch_t* const p_info_ch,
                                const ssif_channel_cfg_t* const p_ch_cfg)
{
    int_t    ercd;

    if ((NULL == p_info_ch) || (NULL == p_ch_cfg))
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        /* stop DMA transfer */
        ercd = SSIF_DisableChannel(p_info_ch);

        if (ESUCCESS == ercd)
        {
            /* copy config data to channel info */
            ercd = SSIF_UpdateChannelConfig(p_info_ch, p_ch_cfg);
        }

        if (ESUCCESS == ercd)
        {
            /* restart DMA transfer */
            ercd = SSIF_EnableChannel(p_info_ch);
        }
    }

    return ercd;
}

/******************************************************************************
* Function Name: SSIF_IOCTL_GetStatus
* @brief         Get a value of SSISR register.
*
*                Description:<br>
*
* @param[in]     p_info_ch  :channel object
* @param[in,out] p_status   :pointer of status value
* @retval        ESUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
int_t SSIF_IOCTL_GetStatus(const ssif_info_ch_t* const p_info_ch, uint32_t* const p_status)
{
    int_t ret = ESUCCESS;

    if ((NULL == p_info_ch) || (NULL == p_status))
    {
        ret = EFAULT_RBSP;
    }
    else
    {
        *p_status = g_ssireg[p_info_ch->channel]->SSISR;
    }

    return ret;
}

/******************************************************************************
* Function Name: SSIF_SWLtoLen
* @brief         Convert SSICR:SWL bits to system word length
*
*                Description:<br>
*
* @param[in]     ssicr_swl  :SSICR register SWL field value(0 to 7)
* @retval        8 to 256   :system word length(bits)
******************************************************************************/
int_t SSIF_SWLtoLen(const ssif_chcfg_system_word_t ssicr_swl)
{
    /* -> IPA M1.10.1 : This is conversion table that can't be macro-coding. */
    static const int_t decode_enum_swl[SSIF_CFG_SYSTEM_WORD_256+1] = {
        8,      /* SSIF_CFG_SYSTEM_WORD_8   */
        16,     /* SSIF_CFG_SYSTEM_WORD_16  */
        24,     /* SSIF_CFG_SYSTEM_WORD_24  */
        32,     /* SSIF_CFG_SYSTEM_WORD_32  */
        48,     /* SSIF_CFG_SYSTEM_WORD_48  */
        64,     /* SSIF_CFG_SYSTEM_WORD_64  */
        128,    /* SSIF_CFG_SYSTEM_WORD_128 */
        256     /* SSIF_CFG_SYSTEM_WORD_256 */
    };
    /* <- IPA M1.10.1 */

    return decode_enum_swl[ssicr_swl];
}

/******************************************************************************
* Function Name: SSIF_DWLtoLen
* @brief         Convert SSICR:DWL bits to data word length
*
*                Description:<br>
*
* @param[in]     ssicr_dwl  :SSICR register DWL field value(0 to 6)
* @retval        8 to 32    :data word length(bits)
******************************************************************************/
int_t SSIF_DWLtoLen(const ssif_chcfg_data_word_t ssicr_dwl)
{
    /* -> IPA M1.10.1 : This is conversion table that can't be macro-coding. */
    static const int_t decode_enum_dwl[SSIF_CFG_DATA_WORD_32+1] = {
        8,   /* SSIF_CFG_DATA_WORD_8  */
        16,  /* SSIF_CFG_DATA_WORD_16 */
        18,  /* SSIF_CFG_DATA_WORD_18 */
        20,  /* SSIF_CFG_DATA_WORD_20 */
        22,  /* SSIF_CFG_DATA_WORD_22 */
        24,  /* SSIF_CFG_DATA_WORD_24 */
        32   /* SSIF_CFG_DATA_WORD_32 */
    };
    /* <- IPA M1.10.1 */

    return decode_enum_dwl[ssicr_dwl];
}

/******************************************************************************
Private functions
******************************************************************************/

/******************************************************************************
* Function Name: SSIF_InitChannel
* @brief         Initialize for the SSIF channel
*
*                Description:<br>
*                Create semaphore and queue for channel.<br>
*                And setup SSIF pin.
* @param[in,out] p_info_ch  :channel object
* @retval        ESUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
static int_t SSIF_InitChannel(ssif_info_ch_t* const p_info_ch)
{
    int32_t os_ret;
    uint32_t ssif_ch;
    int_t ercd = ESUCCESS;
    static const osSemaphoreAttr_t* semdef_access[SSIF_NUM_CHANS] =
    {
        osSemaphore(ssif_ch0_access),
        osSemaphore(ssif_ch1_access),
        osSemaphore(ssif_ch2_access),
        osSemaphore(ssif_ch3_access),
#if defined(TARGET_RZA1H) /* mbed */
        osSemaphore(ssif_ch4_access),
        osSemaphore(ssif_ch5_access)
#endif /* mbed */
    };
    static const bool_t is_duplex_ch[SSIF_NUM_CHANS] =
    {
        true,   /* SSIF0 is full duplex channel */
        true,   /* SSIF1 is full duplex channel */
        false,  /* SSIF2 is half duplex channel */
        true,   /* SSIF3 is full duplex channel */
#if defined(TARGET_RZA1H) /* mbed */
        false,  /* SSIF4 is half duplex channel */
        true    /* SSIF5 is full duplex channel */
#endif /* mbed */
    };

    if (NULL == p_info_ch)
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        ssif_ch = p_info_ch->channel;

        p_info_ch->is_full_duplex = is_duplex_ch[ssif_ch];

        /* Create sem_access semaphore */
        p_info_ch->sem_access = osSemaphoreNew(0xffff, 1, semdef_access[ssif_ch]);

        if (NULL == p_info_ch->sem_access)
        {
            ercd = ENOMEM_RBSP;
        }

        if (ESUCCESS == ercd)
        {
            ercd = ahf_create(&p_info_ch->tx_que, AHF_LOCKINT);
            if (ESUCCESS != ercd)
            {
                ercd = ENOMEM_RBSP;
            }
        }

        if (ESUCCESS == ercd)
        {
            ercd = ahf_create(&p_info_ch->rx_que, AHF_LOCKINT);
            if (ESUCCESS != ercd)
            {
                ercd = ENOMEM_RBSP;
            }
        }

        if (ESUCCESS == ercd)
        {
            /* set channel initialize */
            p_info_ch->openflag = 0;

            p_info_ch->p_aio_tx_curr = NULL;       /* tx request pointer */
            p_info_ch->p_aio_tx_next = NULL;       /* tx request pointer */
            p_info_ch->p_aio_rx_curr = NULL;       /* rx request pointer */
            p_info_ch->p_aio_rx_next = NULL;       /* rx request pointer */
        }

        if (ESUCCESS == ercd)
        {
            ercd = R_SSIF_Userdef_InitPinMux(ssif_ch);
        }

        if (ESUCCESS == ercd)
        {
            p_info_ch->ch_stat = SSIF_CHSTS_INIT;
        }
        else
        {
            if (NULL != p_info_ch->sem_access)
            {
                os_ret = osSemaphoreDelete(p_info_ch->sem_access);
                if (osOK != os_ret)
                {
                    /* NON_NOTICE_ASSERT: unexpected semaphore error */
                }
                p_info_ch->sem_access = NULL;
            }
        }
    }

    return ercd;
}

/******************************************************************************
* Function Name: SSIF_UnInitChannel
* @brief         Uninitialise the SSIF channel.
*
*                Description:<br>
*
* @param[in,out] p_info_ch  :channel object
* @retval        none
******************************************************************************/
static void SSIF_UnInitChannel(ssif_info_ch_t* const p_info_ch)
{
    int32_t os_ret;
    uint32_t ssif_ch;

    if (NULL == p_info_ch)
    {
        /* NON_NOTICE_ASSERT: illegal pointer */
    }
    else
    {
        ssif_ch = p_info_ch->channel;

        if (SSIF_CHSTS_INIT != p_info_ch->ch_stat)
        {
            /* NON_NOTICE_ASSERT: unexpected channel status */
        }

        p_info_ch->ch_stat = SSIF_CHSTS_UNINIT;

        SSIF_DisableErrorInterrupt(ssif_ch);

        core_util_critical_section_enter();

        /* delete the tx queue */
        ahf_cancelall(&p_info_ch->tx_que);
        ahf_destroy(&p_info_ch->tx_que);

        /* delete the rx queue */
        ahf_cancelall(&p_info_ch->rx_que);
        ahf_destroy(&p_info_ch->rx_que);

        /* delete the private semaphore */
        os_ret = osSemaphoreDelete(p_info_ch->sem_access);
        if (osOK != os_ret)
        {
            /* NON_NOTICE_ASSERT: unexpected semaphore error */
        }

        SSIF_InterruptShutdown(ssif_ch);

        core_util_critical_section_exit();
    }

    return;
}

/******************************************************************************
* Function Name: SSIF_UpdateChannelConfig
* @brief         Save configuration to the SSIF driver.
*
*                Description:<br>
*                Update channel object.
* @param[in,out] p_info_ch  :channel object
* @param[in]     p_ch_cfg   :SSIF channel configuration parameter
* @retval        ESUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
static int_t SSIF_UpdateChannelConfig(ssif_info_ch_t* const p_info_ch,
                                      const ssif_channel_cfg_t* const p_ch_cfg)
{
    int_t ercd;

    if ((NULL == p_info_ch) || (NULL == p_ch_cfg))
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        ercd = SSIF_CheckChannelCfg(p_ch_cfg);

        if (ESUCCESS == ercd)
        {
            p_info_ch->slave_mode = p_ch_cfg->slave_mode;

            if (false != p_info_ch->slave_mode)
            {
                /* slave mode */
                p_info_ch->clock_direction = SSIF_CFG_CLOCK_IN;
                p_info_ch->ws_direction = SSIF_CFG_WS_IN;
            }
            else
            {
                /* master mode */
                p_info_ch->clock_direction = SSIF_CFG_CLOCK_OUT;
                p_info_ch->ws_direction = SSIF_CFG_WS_OUT;

                /* when master mode, always disable noise cancel */
                p_info_ch->noise_cancel = SSIF_CFG_DISABLE_NOISE_CANCEL;
            }

            p_info_ch->sample_freq        = p_ch_cfg->sample_freq;

            p_info_ch->clk_select         = p_ch_cfg->clk_select;
            p_info_ch->multi_ch           = p_ch_cfg->multi_ch;
            p_info_ch->data_word          = p_ch_cfg->data_word;
            p_info_ch->system_word        = p_ch_cfg->system_word;
            p_info_ch->bclk_pol           = p_ch_cfg->bclk_pol;
            p_info_ch->ws_pol             = p_ch_cfg->ws_pol;
            p_info_ch->padding_pol        = p_ch_cfg->padding_pol;
            p_info_ch->serial_alignment   = p_ch_cfg->serial_alignment;
            p_info_ch->parallel_alignment = p_ch_cfg->parallel_alignment;
            p_info_ch->ws_delay           = p_ch_cfg->ws_delay;
            p_info_ch->noise_cancel       = p_ch_cfg->noise_cancel;
            p_info_ch->tdm_mode           = p_ch_cfg->tdm_mode;
#if defined(TARGET_RZA1H) /* mbed */
            p_info_ch->romdec_direct.mode     = p_ch_cfg->romdec_direct.mode;
            p_info_ch->romdec_direct.p_cbfunc = p_ch_cfg->romdec_direct.p_cbfunc;
#endif /* mbed */

            if (SSIF_CFG_ENABLE_TDM == p_info_ch->tdm_mode)
            {
                /* check combination of parameters */
                if ((SSIF_CFG_MULTI_CH_1 == p_info_ch->multi_ch)
                    || (SSIF_CFG_WS_HIGH == p_info_ch->ws_pol))
                {
                    ercd = EINVAL_RBSP;
                }
            }
        }

        if (ESUCCESS == ercd)
        {
            ercd = SSIF_CheckWordSize(p_info_ch);
        }

        if (ESUCCESS == ercd)
        {
            if (false == p_info_ch->slave_mode)
            {
                /* Master: call user own clock setting function */
                ercd = R_SSIF_Userdef_SetClockDiv(p_ch_cfg, &p_info_ch->clk_div);
            }
            else
            {
                /* Slave: set dummy value for clear */
                p_info_ch->clk_div = SSIF_CFG_CKDV_BITS_1;
            }
        }
    }

    return ercd;
}

/******************************************************************************
* Function Name: SSIF_SetCtrlParams
* @brief         Set SSIF configuration to hardware.
*
*                Description:<br>
*                Update SSICR register.
* @param[in]     p_info_ch  :channel object
* @retval        ESUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
static int_t SSIF_SetCtrlParams(const ssif_info_ch_t* const p_info_ch)
{
    int_t ret = ESUCCESS;
    uint32_t ssif_ch;
    static const uint32_t gpio_sncr_bit[SSIF_NUM_CHANS] =
    {
        GPIO_SNCR_BIT_SSI0NCE,  /* SSIF0 */
        GPIO_SNCR_BIT_SSI1NCE,  /* SSIF1 */
        GPIO_SNCR_BIT_SSI2NCE,  /* SSIF2 */
        GPIO_SNCR_BIT_SSI3NCE,  /* SSIF3 */
#if defined(TARGET_RZA1H) /* mbed */
        GPIO_SNCR_BIT_SSI4NCE,  /* SSIF4 */
        GPIO_SNCR_BIT_SSI5NCE   /* SSIF5 */
#endif /* mbed */
    };

    if (NULL == p_info_ch)
    {
        ret = EFAULT_RBSP;
    }
    else
    {
        ssif_ch = p_info_ch->channel;

        /* ALL CLEAR */
        g_ssireg[ssif_ch]->SSICR = 0u;
        g_ssireg[ssif_ch]->SSISR = 0u;
        g_ssireg[ssif_ch]->SSIFCCR = 0u;

        g_ssireg[ssif_ch]->SSICR = (uint32_t)(
             ((uint32_t)(p_info_ch->clk_select)         << SSIF_CR_SHIFT_CKS) |
             ((uint32_t)(p_info_ch->multi_ch)           << SSIF_CR_SHIFT_CHNL) |
             ((uint32_t)(p_info_ch->data_word)          << SSIF_CR_SHIFT_DWL) |
             ((uint32_t)(p_info_ch->system_word)        << SSIF_CR_SHIFT_SWL) |
             ((uint32_t)(p_info_ch->bclk_pol)           << SSIF_CR_SHIFT_SCKP) |
             ((uint32_t)(p_info_ch->ws_pol)             << SSIF_CR_SHIFT_SWSP) |
             ((uint32_t)(p_info_ch->padding_pol)        << SSIF_CR_SHIFT_SPDP) |
             ((uint32_t)(p_info_ch->serial_alignment)   << SSIF_CR_SHIFT_SDTA) |
             ((uint32_t)(p_info_ch->parallel_alignment) << SSIF_CR_SHIFT_PDTA) |
             ((uint32_t)(p_info_ch->ws_delay)           << SSIF_CR_SHIFT_DEL) |
             ((uint32_t)(p_info_ch->clock_direction)    << SSIF_CR_SHIFT_SCKD) |
             ((uint32_t)(p_info_ch->ws_direction)       << SSIF_CR_SHIFT_SWSD) |
             ((uint32_t)(p_info_ch->clk_div)            << SSIF_CR_SHIFT_CKDV)
             );

        g_ssireg[ssif_ch]->SSITDMR = ((uint32_t)(p_info_ch->tdm_mode) << SSIF_TDMR_SHIFT_TDM);

        /* change SNCR register: enter exclusive */
        core_util_critical_section_enter();

        if ((SSIF_CFG_ENABLE_NOISE_CANCEL == p_info_ch->noise_cancel)
            && (false != p_info_ch->slave_mode))
        {
            /* ENABLE_NOISE_CANCEL && slave mode */
            GPIO.SNCR |= (uint32_t)gpio_sncr_bit[ssif_ch];
        }
        else
        {
            /* DISABLE_NOISE_CANCEL || master mode */
            GPIO.SNCR &= ~((uint32_t)gpio_sncr_bit[ssif_ch]);
        }

        /* change SNCR register: exit exclusive */
        core_util_critical_section_exit();
    }

    return ret;
}

/******************************************************************************
* Function Name: SSIF_CheckChannelCfg
* @brief         Check channel configuration parameters are valid or not.
*
*                Description:<br>
*
* @param[in]     p_ch_cfg     :channel configuration
* @retval        ESUCCESS     :Success.
* @retval        error code   :Failure.
******************************************************************************/
static int_t SSIF_CheckChannelCfg(const ssif_channel_cfg_t* const p_ch_cfg)
{
    int_t ret = ESUCCESS;

    if (NULL == p_ch_cfg)
    {
        ret = EFAULT_RBSP;
    }
    else
    {
        switch (p_ch_cfg->clk_select)
        {
        case SSIF_CFG_CKS_AUDIO_X1:
            /* fall through */
        case SSIF_CFG_CKS_AUDIO_CLK:
            /* do nothing */
            break;
        default:
            ret = EINVAL_RBSP;
            break;
        }

        if (ESUCCESS == ret)
        {
            switch (p_ch_cfg->multi_ch)
            {
            case SSIF_CFG_MULTI_CH_1:
                /* fall through */
            case SSIF_CFG_MULTI_CH_2:
                /* fall through */
            case SSIF_CFG_MULTI_CH_3:
                /* fall through */
            case SSIF_CFG_MULTI_CH_4:
                /* do nothing */
                break;
            default:
                ret = EINVAL_RBSP;
                break;
            }
        }

        if (ESUCCESS == ret)
        {
            switch (p_ch_cfg->data_word)
            {
            case SSIF_CFG_DATA_WORD_8:
                /* fall through */
            case SSIF_CFG_DATA_WORD_16:
                /* fall through */
            case SSIF_CFG_DATA_WORD_18:
                /* fall through */
            case SSIF_CFG_DATA_WORD_20:
                /* fall through */
            case SSIF_CFG_DATA_WORD_22:
                /* fall through */
            case SSIF_CFG_DATA_WORD_24:
                /* fall through */
            case SSIF_CFG_DATA_WORD_32:
                /* do nothing */
                break;
            default:
                ret = EINVAL_RBSP;
                break;
            }
        }

        if (ESUCCESS == ret)
        {
            switch (p_ch_cfg->system_word)
            {
            case SSIF_CFG_SYSTEM_WORD_8:
                /* fall through */
            case SSIF_CFG_SYSTEM_WORD_16:
                /* fall through */
            case SSIF_CFG_SYSTEM_WORD_24:
                /* fall through */
            case SSIF_CFG_SYSTEM_WORD_32:
                /* fall through */
            case SSIF_CFG_SYSTEM_WORD_48:
                /* fall through */
            case SSIF_CFG_SYSTEM_WORD_64:
                /* fall through */
            case SSIF_CFG_SYSTEM_WORD_128:
                /* fall through */
            case SSIF_CFG_SYSTEM_WORD_256:
                /* do nothing */
                break;
            default:
                ret = EINVAL_RBSP;
                break;
            }
        }

        if (ESUCCESS == ret)
        {
            switch (p_ch_cfg->bclk_pol)
            {
            case SSIF_CFG_FALLING:
                /* fall through */
            case SSIF_CFG_RISING:
                /* do nothing */
                break;
            default:
                ret = EINVAL_RBSP;
                break;
            }
        }

        if (ESUCCESS == ret)
        {
            switch (p_ch_cfg->ws_pol)
            {
            case SSIF_CFG_WS_LOW:
                /* fall through */
            case SSIF_CFG_WS_HIGH:
                /* do nothing */
                break;
            default:
                ret = EINVAL_RBSP;
                break;
            }
        }

        if (ESUCCESS == ret)
        {
            switch (p_ch_cfg->padding_pol)
            {
            case SSIF_CFG_PADDING_LOW:
                /* fall through */
            case SSIF_CFG_PADDING_HIGH:
                /* do nothing */
                break;
            default:
                ret = EINVAL_RBSP;
                break;
            }
        }

        if (ESUCCESS == ret)
        {
            switch (p_ch_cfg->serial_alignment)
            {
            case SSIF_CFG_DATA_FIRST:
                /* fall through */
            case SSIF_CFG_PADDING_FIRST:
                /* do nothing */
                break;
            default:
                ret = EINVAL_RBSP;
                break;
            }
        }

        if (ESUCCESS == ret)
        {
            switch (p_ch_cfg->parallel_alignment)
            {
            case SSIF_CFG_LEFT:
                /* fall through */
            case SSIF_CFG_RIGHT:
                /* do nothing */
                break;
            default:
                ret = EINVAL_RBSP;
                break;
            }
        }

        if (ESUCCESS == ret)
        {
            switch (p_ch_cfg->ws_delay)
            {
            case SSIF_CFG_DELAY:
                /* fall through */
            case SSIF_CFG_NO_DELAY:
                /* do nothing */
                break;
            default:
                ret = EINVAL_RBSP;
                break;
            }
        }

        if (ESUCCESS == ret)
        {
            switch (p_ch_cfg->noise_cancel)
            {
            case SSIF_CFG_DISABLE_NOISE_CANCEL:
                /* fall through */
            case SSIF_CFG_ENABLE_NOISE_CANCEL:
                /* do nothing */
                break;
            default:
                ret = EINVAL_RBSP;
                break;
            }
        }

        if (ESUCCESS == ret)
        {
            switch (p_ch_cfg->tdm_mode)
            {
            case SSIF_CFG_DISABLE_TDM:
                /* fall through */
            case SSIF_CFG_ENABLE_TDM:
                /* do nothing */
                break;
            default:
                ret = EINVAL_RBSP;
                break;
            }
        }
#if defined(TARGET_RZA1H) /* mbed */

        if (ESUCCESS == ret)
        {
            switch (p_ch_cfg->romdec_direct.mode)
            {
            case SSIF_CFG_DISABLE_ROMDEC_DIRECT:
                /* fall through */
            case SSIF_CFG_ENABLE_ROMDEC_DIRECT:
                /* do nothing */
                break;
            default:
                ret = EINVAL_RBSP;
                break;
            }
        }
#endif /* mbed */
    }

    return ret;
}

/******************************************************************************
* Function Name: SSIF_CheckWordSize
* @brief         Check system word size whether that is valid or not.
*
*                Description:<br>
*                if system_words couldn't involve specified number of<br>
*                data_words then error.
* @param[in]     p_info_ch    :channel object
* @retval        ESUCCESS     :Success.
* @retval        error code   :Failure.
******************************************************************************/
static int_t SSIF_CheckWordSize(const ssif_info_ch_t* const p_info_ch)
{
    uint32_t ssicr_chnl;
    uint32_t dw_per_sw;
    uint32_t datawd_len;
    uint32_t syswd_len;
    int_t ret = ESUCCESS;

    if (NULL == p_info_ch)
    {
        ret = EFAULT_RBSP;
    }
    else
    {
        ssicr_chnl = p_info_ch->multi_ch;
        /* ->MISRA 13.7 : This is verbose error check by way of precaution */
        if (SSIF_CFG_MULTI_CH_4 < ssicr_chnl)
        /* <-MISRA 13.7 */
        {
            ret = EINVAL_RBSP;
        }
        else
        {
            /* data_words number per system_words */
            if (SSIF_CFG_ENABLE_TDM == p_info_ch->tdm_mode)
            {
                /* When TDM Mode data_word number per system_words fixed to 1 */
                dw_per_sw = 1u;
            }
            else
            {
                /* When not TDM data_word number per system_words depends CHNL */
                dw_per_sw = ssicr_chnl + 1u;
            }

            /* size of data_words */
            datawd_len = (uint32_t)SSIF_DWLtoLen(p_info_ch->data_word);

            if (0u == datawd_len)
            {
                ret = EINVAL_RBSP;
            }
            else
            {
                /* size of system_words */
                syswd_len = (uint32_t)SSIF_SWLtoLen(p_info_ch->system_word);

                if (syswd_len < (datawd_len * dw_per_sw))
                {
                    ret = EINVAL_RBSP;
                }
            }
        }
    }

    return ret;
}

/******************************************************************************
* Function Name: SSIF_Reset
* @brief         SSIF software reset
*
*                Description:<br>
*
* @param[in]     ssif_ch       :SSIF channel
* @retval        none
******************************************************************************/
static void SSIF_Reset(const uint32_t ssif_ch)
{
    uint8_t dummy_read_u8;
    static const uint32_t cpg_swrst_bit[SSIF_NUM_CHANS] =
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

    /* change register: enter exclusive */
    core_util_critical_section_enter();

    /* SW Reset ON */
    /* ->IPA R2.4.2 : This is implicit type conversion that doesn't have bad effect on accessing to 8bit register. */
    CPGSWRSTCR1 |= (uint8_t)cpg_swrst_bit[ssif_ch];
    dummy_read_u8 = CPGSWRSTCR1;
    /* <-IPA R2.4.2 */
    UNUSED_ARG(dummy_read_u8);

    /* SW Reset OFF */
    /* ->IPA R2.4.2 : This is implicit type conversion that doesn't have bad effect on accessing to 8bit register. */
    CPGSWRSTCR1 &= (uint8_t)~((uint8_t)cpg_swrst_bit[ssif_ch]);
    dummy_read_u8 = CPGSWRSTCR1;
    /* <-IPA R2.4.2 */
    UNUSED_ARG(dummy_read_u8);

    /* change register: exit exclusive */
    core_util_critical_section_exit();

    return;
}

