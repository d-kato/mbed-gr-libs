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
* File Name   : spdif.c
* $Rev: $
* $Date::                           $
* Description : SPDIF driver functions
******************************************************************************/

/*******************************************************************************
Includes <System Includes>, "Project Includes"
*******************************************************************************/
#include "spdif.h"
#include "iodefine.h"
#include "iobitmask.h"
#include "cmsis.h"
#include "mbed_critical.h"

/*******************************************************************************
Macro definitions
*******************************************************************************/
/* SPDIF CTRL Register VALUE */
/* b21     RDE      - Receiver DMA Enable                         - 0  : Disable             */
/* b20     TDE      - Transmitter DMA Enable                      - 0  : Disable             */
/* b19     NCSI     - New Channel Status Information              - 0  : Not present         */
/* b18     AOS      - Audio Only Samples                          - 1  : Not user data       */
/* b17     RME      - Receiver Module Enable                      - 0  : Disable             */
/* b16     TME      - Transmitter Module Enable                   - 0  : Disable             */
/* b15     REIE     - Receiver Error Interrupt Enable             - 1  : Enable              */
/* b14     TEIE     - Transmitter Error Interrupt Enable          - 1  : Enable              */
/* b13     UBOI     - User Buffer Overrun Interrupt Enable        - 1  : Enable              */
/* b12     UBUI     - User Buffer Underrun Interrupt Enable       - 1  : Enable              */
/* b11     CREI     - Clock Recovery Error Interrupt Enable       - 1  : Enable              */
/* b10     PAEI     - Parity Error Interrupt Enable               - 1  : Enable              */
/* b9      PREI     - Preamble Error Interrupt Enable             - 1  : Enable              */
/* b8      CSEI     - Channel Status Error Interrupt Enable       - 1  : Enable              */
/* b7      ABOI     - Audio Buffer Overrun Interrupt Enable       - 1  : Enable              */
/* b6      ABUI     - Audio Buffer Underrun Interrupt Enable      - 1  : Enable              */
/* b5      RUII     - Receiver User Information Interrupt Enable  - 0  : Disable             */
/* b4      TUII     - Transmitter User Information Interrupt Enable 0  : Disable             */
/* b3      RCSI     - Receiver Channel Status Interrupt Enable    - 0  : Disable             */
/* b2      RCBI     - Receiver Channel Buffer Interrupt Enable    - 0  : Disable             */
/* b1      TCSI     - Transmitter Channel Status Interrupt Enable - 0  : Disable             */
/* b0      TCBI     - Transmitter Channel Buffer Interrupt Enable - 0  : Disable             */
#define SPDIF_CTRL_BASE_INIT_VALUE  (0x0004FFC0uL)


#define SPDIF_UPDATA_WAIT   (250u)

    /* Status Register
    b31:b30 Reserved - Read only bit       - 00:
    b29:b28 CLAC     - Clock Accuracy      - 01:   Level 1 (50 ppm)
    b27:b24 FS       - Sample Frequency    - 0000: 44.1kHz
    b23:b20 CHNO     - Channel Number      - 0001: Left (0010: Right)
    b19:b16 SRCNO    - Source Number       - 0000:
    b15:b8  CATCD    - Category Code       - 00000000: General 2-channel format
    b7:b6   Reserved - Read only bit       - 00: 
    b5:b1   CTL      - Control             - 0:  2 channel Audio, 
                                             00: Disable preemphasis
                                             0:  Disable copy
                                             0:  LPCM
    b0      Reserved - Read only bit       - 0: */
#define SPDIF_STATUS_LCH                    (0x10100000u)
#define SPDIF_STATUS_RCH                    (0x10200000u)

/*******************************************************************************
Typedef definitions
*******************************************************************************/

/*******************************************************************************
Exported global variables (to be accessed by other files)
*******************************************************************************/
/* ->MISRA 8.8, MISRA 8.10, IPA M2.2.2 : These declare statements are dependent on CMSIS-RTOS */
osSemaphoreDef(spdif_ch0_access);
/* <-MISRA 8.8, MISRA 8.10, IPA M2.2.2 */

#if(1) /* mbed */
static spdif_info_drv_t g_spdif_info_drv;

spdif_info_drv_t * SPDIF_GetDrvInstanc(void) {
    return &g_spdif_info_drv;
}
#else
spdif_info_drv_t g_spdif_info_drv;
#endif

/*******************************************************************************
Private global variables and functions
*******************************************************************************/
static int_t SPDIF_InitChannel(spdif_info_ch_t* const p_info_ch);
static void SPDIF_UnInitChannel(spdif_info_ch_t* const p_info_ch);
static int_t SPDIF_SetCtrlParams(const spdif_info_ch_t* const p_info_ch);
static int_t SPDIF_CheckChannelCfg(const spdif_channel_cfg_t* const p_ch_cfg);
static void SPDIF_Handler(void);
static void SPDIF_UserInfoRegSetWait(void);
static void SPDIF_ChannelStatusRegSet(const spdif_info_ch_t* const p_info_ch);
static void SPDIF_ChannelStatusRegSetWait(void);
static void SPDIF_ChannelStatusRegGet(spdif_info_ch_t* const p_info_ch);

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/

#if(1) /* mbed */
/******************************************************************************
* Function Name: SPDIF_InitialiseOne
* @brief         Initialize the SPDIF driver's internal data
*
*                Description:<br>
*
* @param[in]     channel    :channel number
* @param[in]     p_cfg_data :pointer of several parameters array per channels
* @retval        ESUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
int_t SPDIF_InitialiseOne(const int_t channel, const spdif_channel_cfg_t* const p_cfg_data)
{
    int_t           ercd = ESUCCESS;
    spdif_info_ch_t* p_info_ch;

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
        p_info_ch = &g_spdif_info_drv.info_ch;
        p_info_ch->enabled = p_cfg_data->enabled;

        /* copy config data to channel info */
        ercd = SPDIF_CheckChannelCfg(p_cfg_data);
        if (ESUCCESS == ercd)
        {
            p_info_ch->clk_select         = p_cfg_data->clk_select;
            p_info_ch->audio_bit_tx       = p_cfg_data->audio_bit_tx;
            p_info_ch->audio_bit_rx       = p_cfg_data->audio_bit_rx;
            p_info_ch->int_level          = p_cfg_data->int_level;
            p_info_ch->tx_u_data_enabled  = p_cfg_data->tx_u_data_enabled;
        }

        if (ESUCCESS == ercd)
        {
            ercd = SPDIF_InitChannel(p_info_ch);
        }

        if (ESUCCESS == ercd)
        {
            InterruptHandlerRegister(SPDIFI_IRQn, &SPDIF_Handler);
            GIC_SetPriority(SPDIFI_IRQn, p_cfg_data->int_level);
            GIC_EnableIRQ(SPDIFI_IRQn);
        }
    }

    return ercd;
}

/******************************************************************************
* Function Name: SPDIF_UnInitialiseOne
* @brief         UnInitialize the SPDIF driver's internal data
*
*                Description:<br>
*
* @param[in]     channel    :channel number
* @retval        ESUCCESS   :Success.
******************************************************************************/
int_t SPDIF_UnInitialiseOne(const int_t channel)
{
    const int_t     ercd = ESUCCESS;
    spdif_info_ch_t* p_info_ch;

    p_info_ch = &g_spdif_info_drv.info_ch;

    if (false != p_info_ch->enabled)
    {
        SPDIF_UnInitChannel(p_info_ch);
    }

    return ercd;
}
#else
/******************************************************************************
* Function Name: SPDIF_Initialise
* @brief         Initialize the SPDIF driver's internal data
*
*                Description:<br>
*
* @param[in]     p_cfg_data :pointer of several parameters array per channels
* @retval        ESUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
int_t SPDIF_Initialise(const spdif_channel_cfg_t* const p_cfg_data)
{
    int_t           ercd = ESUCCESS;
    spdif_info_ch_t* p_info_ch;

    if (NULL == p_cfg_data)
    {
        ercd = EFAULT;
    }
    else
    {
        p_info_ch = &g_spdif_info_drv.info_ch;
        p_info_ch->enabled = p_cfg_data.enabled;

        if (false != p_info_ch->enabled)
        {
            /* copy config data to channel info */
            ercd = SPDIF_CheckChannelCfg(p_ch_cfg);
            if (ESUCCESS == ercd)
            {
                p_info_ch->clk_select         = p_cfg_data->clk_select;
                p_info_ch->audio_bit_tx       = p_cfg_data->audio_bit_tx;
                p_info_ch->audio_bit_rx       = p_cfg_data->audio_bit_rx;
                p_info_ch->int_level          = p_cfg_data->int_level;
            }

            if (ESUCCESS == ercd)
            {
                ercd = SPDIF_InitChannel(p_info_ch);
            }
        }

        if (ESUCCESS == ercd)
        {
            p_info_ch = &g_spdif_info_drv.info_ch;

            if (false != p_info_ch->enabled)
            {
                InterruptHandlerRegister(SPDIFI_IRQn, &SPDIF_Handler);
                GIC_SetPriority(SPDIFI_IRQn, p_cfg_data.int_level);
                GIC_EnableIRQ(SPDIFI_IRQn);
            }
        }
    }

    return ercd;
}

/******************************************************************************
* Function Name: SPDIF_UnInitialise
* @brief         UnInitialize the SPDIF driver's internal data
*
*                Description:<br>
*
* @param         none
* @retval        ESUCCESS   :Success.
******************************************************************************/
int_t SPDIF_UnInitialise(void)
{
    const int_t     ercd = ESUCCESS;
    spdif_info_ch_t* p_info_ch;

    p_info_ch = &g_spdif_info_drv.info_ch;

    if (false != p_info_ch->enabled)
    {
        SPDIF_UnInitChannel(p_info_ch);
    }

    return ercd;
}
#endif /* end mbed */

/******************************************************************************
* Function Name: SPDIF_EnableChannel
* @brief         Enable the SPDIF channel
*
*                Description:<br>
*
* @param[in,out] p_info_ch  :channel object
* @retval        ESUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
int_t SPDIF_EnableChannel(spdif_info_ch_t* const p_info_ch)
{
    int_t ercd = ESUCCESS;
    volatile uint8_t dummy_buf_8;
    volatile uint32_t dummy_buf_32;

    if (NULL == p_info_ch)
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        /* enable the SPDIF clock */
        if (ESUCCESS == ercd)
        {
            core_util_critical_section_enter();

            /* ->IPA R2.4.2 : This is implicit type conversion that doesn't have bad effect on writing to 8bit register. */
            CPG.STBCR9.BYTE &= (uint8_t)~0x02;
            /* <-IPA R2.4.2 */
            dummy_buf_8 = CPG.STBCR9.BYTE;
            (void)dummy_buf_8;

            core_util_critical_section_exit();
        }

        /* configure channel hardware */
        if (ESUCCESS == ercd)
        {
            /* Set control parameters */
            ercd = SPDIF_SetCtrlParams(p_info_ch);
        }


        if (O_RDONLY_RBSP != p_info_ch->openflag)
        {
            /* ==== Disable interrupt ==== */
            /* ----  Disable interrupt (User data empty) ----  */
            SPDIF.CTRL.BIT.TUII = 0;

            /* ----  Disable interrupt (Channel Status empty) ----  */
            SPDIF.CTRL.BIT.TCSI = 0;

            /* ==== SPDIF Tx Enable ==== */
            SPDIF.CTRL.BIT.TME = 1;

            /* ==== Channel status information ==== */
            /* Be sure to write data in the order of (1) TLCS and TRCS and (2) TUI. */
            /* Write to the data register */
            SPDIF_ChannelStatusRegSet(p_info_ch);
            SPDIF_ChannelStatusRegSetWait();

            /* ==== User information ==== */
            /* ---- Enable user information ---- */
            if (p_info_ch->tx_u_data_enabled == false)
            {
                SPDIF.CTRL.BIT.AOS = 1;
            }
            else
            {
                SPDIF.CTRL.BIT.AOS = 0;
            }

            /* ==== Enable interrupt ==== */
            /* ----  Enable interrupt (Channel Status empty) ----  */
            SPDIF.CTRL.BIT.TCSI = 1;

            /* ----  Enable interrupt (User data empty) ----  */
            SPDIF.CTRL.BIT.TUII = 1;
        }

        if (O_WRONLY_RBSP != p_info_ch->openflag)
        {
            /* ==== Status Clear(dummy read) ==== */
            dummy_buf_32 = SPDIF.RUI.LONG;
            dummy_buf_32 = SPDIF.RLCS.LONG;
            dummy_buf_32 = SPDIF.RRCS.LONG;
            dummy_buf_32 = SPDIF.RDAD.LONG;
            (void)dummy_buf_32;

            /* ==== Enable interrupt ==== */
            /* ---- Enable interrupt (Channel Status full) ---- */
            SPDIF.CTRL.BIT.RCSI = 1;

            /* ----  Enable interrupt (User data full) ----  */
            SPDIF.CTRL.BIT.RUII = 1;

            SPDIF.CTRL.BIT.RDE = 1;
            SPDIF.CTRL.BIT.RME = 1;
        }

        /* allocate and setup/start DMA transfer */
        if (ESUCCESS == ercd)
        {
            ercd = SPDIF_InitDMA(p_info_ch);
        }
    }

    return ercd;
}

/******************************************************************************
* Function Name: SPDIF_DisableChannel
* @brief         Disable the SPDIF channel
*
*                Description:<br>
*
* @param[in,out] p_info_ch  :channel object
* @retval        ESUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
int_t SPDIF_DisableChannel(spdif_info_ch_t* const p_info_ch)
{
    int_t   ret = ESUCCESS;
    volatile uint8_t dummy_buf_8;

    if (NULL == p_info_ch)
    {
        ret = EFAULT_RBSP;
    }
    else
    {
        /* ---- Reset the register ---- */
        /* STAT - Status Register  */
        /* CTRL - Control Register */
        SPDIF.STAT.LONG = 0x00000000;
        SPDIF.CTRL.LONG = 0x00000000;

        /* free DMA resources */
        SPDIF_UnInitDMA(p_info_ch);

        /* disable spdif clock */
        core_util_critical_section_enter();

        /* ->IPA R2.4.2 : This is implicit type conversion that doesn't have bad effect on writing to 8bit register. */
        CPG.STBCR9.BYTE |= (uint8_t)0x02;
        /* <-IPA R2.4.2 */
        dummy_buf_8 = CPG.STBCR9.BYTE;
        (void)dummy_buf_8;
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
            p_info_ch->p_aio_tx_next->aio_return = ECANCELED;
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

    return ret;
}

/******************************************************************************
* Function Name: SPDIF_ErrorRecovery
* @brief         Restart the SPDIF channel
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
void SPDIF_ErrorRecovery_tx(spdif_info_ch_t* const p_info_ch)
{
    int_t   ercd = ESUCCESS;

    if (NULL == p_info_ch)
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        /* disable DMA end interrupt */
        SPDIF.CTRL.LONG &= ~(SPDIF_CTRL_TME | SPDIF_CTRL_TDE);

        /* pause DMA transfer */
        SPDIF_CancelDMA(p_info_ch, O_WRONLY_RBSP);

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

        /* setup/restart DMA transfer */
        ercd = SPDIF_RestartDMA(p_info_ch, O_WRONLY_RBSP);
    }

    if (ESUCCESS != ercd)
    {
        /* NON_NOTICE_ASSERT: cannot restart channel */
    }

    return;
}

void SPDIF_ErrorRecovery_rx(spdif_info_ch_t* const p_info_ch)
{
    int_t   ercd = ESUCCESS;

    if (NULL == p_info_ch)
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        /* disable DMA end interrupt */
        SPDIF.CTRL.LONG &= ~(SPDIF_CTRL_RME | SPDIF_CTRL_RDE);

        /* pause DMA transfer */
        SPDIF_CancelDMA(p_info_ch, O_RDONLY_RBSP);

        /* cancel event to ongoing request */
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
        /* setup/restart DMA transfer */
        ercd = SPDIF_RestartDMA(p_info_ch, O_RDONLY_RBSP);
    }

    if (ESUCCESS != ercd)
    {
        /* NON_NOTICE_ASSERT: cannot restart channel */
    }

    return;
}

/******************************************************************************
* Function Name: SPDIF_PostAsyncIo
* @brief         Enqueue asynchronous read/write request
*
*                Description:<br>
*
* @param[in,out] p_info_ch  :channel object
* @param[in,out] p_aio      :aio control block of read/write request
* @retval        none
******************************************************************************/
void SPDIF_PostAsyncIo(spdif_info_ch_t* const p_info_ch, AIOCB* const p_aio)
{
    if ((NULL == p_info_ch) || (NULL == p_aio))
    {
        /* NON_NOTICE_ASSERT: illegal pointer */
    }
    else
    {
        if ((SPDIF_ASYNC_W == p_aio->aio_return)
         || (SPDIF_ASYNC_STRUCT_W == p_aio->aio_return))
        {
            ahf_addtail(&p_info_ch->tx_que, p_aio);
        }
        else if ((SPDIF_ASYNC_R == p_aio->aio_return)
              || (SPDIF_ASYNC_STRUCT_R == p_aio->aio_return))
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
* Function Name: SPDIF_PostAsyncCancel
* @brief         Cancel read or write request(s)
*
*                Description:<br>
*
* @param[in,out] p_info_ch  :channel object
* @param[in,out] p_aio      :aio control block to cancel or NULL to cancel all.
* @retval        none
******************************************************************************/
void SPDIF_PostAsyncCancel(spdif_info_ch_t* const p_info_ch, AIOCB* const p_aio)
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
* Function Name: SPDIF_IOCTL_ConfigChannel
* @brief         Save configuration to the SPDIF driver.
*
*                Description:<br>
*                Update channel object.
* @param[in,out] p_info_ch  :channel object
* @param[in]     p_ch_cfg   :SPDIF channel configuration parameter
* @retval        ESUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
int_t SPDIF_IOCTL_ConfigChannel(spdif_info_ch_t* const p_info_ch,
                                const spdif_channel_cfg_t* const p_ch_cfg)
{
    int_t    ercd;

    if ((NULL == p_info_ch) || (NULL == p_ch_cfg))
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        /* stop DMA transfer */
        ercd = SPDIF_DisableChannel(p_info_ch);

        if (ESUCCESS == ercd)
        {
            /* copy config data to channel info */
            ercd = SPDIF_CheckChannelCfg(p_ch_cfg);
            if (ESUCCESS == ercd)
            {
                p_info_ch->clk_select         = p_ch_cfg->clk_select;
                p_info_ch->audio_bit_tx       = p_ch_cfg->audio_bit_tx;
                p_info_ch->audio_bit_rx       = p_ch_cfg->audio_bit_rx;
            }
        }

        if (ESUCCESS == ercd)
        {
            /* restart DMA transfer */
            ercd = SPDIF_EnableChannel(p_info_ch);
        }
    }

    return ercd;
}

/******************************************************************************
* Function Name: SPDIF_IOCTL_SetChannelStatus
* @brief         Set a value of Channel Status.
*
*                Description:<br>
*
* @param[in]     p_info_ch  :channel object
* @param[in,out] p_status   :pointer of status value
* @retval        ESUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
int_t SPDIF_IOCTL_SetChannelStatus(spdif_info_ch_t* const p_info_ch,
                                   spdif_channel_status_t* const p_channel_status)
{
    int_t ret = ESUCCESS;

    if ((NULL == p_info_ch) || (NULL == p_channel_status))
    {
        ret = EFAULT_RBSP;
    }
    else
    {
        p_info_ch->ch_sts[0] = p_channel_status->Lch;
        p_info_ch->ch_sts[1] = p_channel_status->Rch;
        ahf_addtail(&p_info_ch->tx_que, &p_info_ch->s_aio_set_ch_sts);
    }

    return ret;
}

/******************************************************************************
* Function Name: SPDIF_IOCTL_SetChannelStatus
* @brief         Get a value of Channel Status.
*
*                Description:<br>
*
* @param[in]     p_info_ch  :channel object
* @param[in,out] p_status   :pointer of status value
* @retval        ESUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
int_t SPDIF_IOCTL_GetChannelStatus(spdif_info_ch_t* const p_info_ch,
                                   spdif_channel_status_t* const p_channel_status)
{
    int_t ret = ESUCCESS;

    if ((NULL == p_info_ch) || (NULL == p_channel_status))
    {
        ret = EFAULT_RBSP;
    }
    else
    {
        p_channel_status->Lch = p_info_ch->rlcs;
        p_channel_status->Rch = p_info_ch->rrcs;
    }

    return ret;
}

/******************************************************************************
* Function Name: SPDIF_IOCTL_SetTransAudioBit
* @brief         Set a value of Channel Status.
*
*                Description:<br>
*
* @param[in]     p_info_ch  :channel object
* @param[in,out] p_status   :pointer of status value
* @retval        ESUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
int_t SPDIF_IOCTL_SetTransAudioBit(spdif_info_ch_t* const p_info_ch,
                                   spdif_chcfg_audio_bit_t sudio_bit)
{
    int_t ret = ESUCCESS;

    if (NULL == p_info_ch)
    {
        ret = EFAULT_RBSP;
    }
    else
    {
        switch (sudio_bit)
        {
        case SPDIF_CFG_AUDIO_BIT_16:
            /* fall through */
        case SPDIF_CFG_AUDIO_BIT_20:
            /* fall through */
        case SPDIF_CFG_AUDIO_BIT_24:
            /* do nothing */
            break;
        default:
            ret = EINVAL_RBSP;
            break;
        }

        if (ESUCCESS == ret)
        {
            p_info_ch->audio_bit_tx = sudio_bit;
            SPDIF.CTRL.BIT.TASS = p_info_ch->audio_bit_tx;
        }
    }

    return ret;
}

/******************************************************************************
* Function Name: SPDIF_IOCTL_SetRecvAudioBit
* @brief         Set a value of Channel Status.
*
*                Description:<br>
*
* @param[in]     p_info_ch  :channel object
* @param[in,out] p_status   :pointer of status value
* @retval        ESUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
int_t SPDIF_IOCTL_SetRecvAudioBit(spdif_info_ch_t* const p_info_ch,
                                   spdif_chcfg_audio_bit_t sudio_bit)
{
    int_t ret = ESUCCESS;

    if (NULL == p_info_ch)
    {
        ret = EFAULT_RBSP;
    }
    else
    {
        switch (sudio_bit)
        {
        case SPDIF_CFG_AUDIO_BIT_16:
            /* fall through */
        case SPDIF_CFG_AUDIO_BIT_20:
            /* fall through */
        case SPDIF_CFG_AUDIO_BIT_24:
            /* do nothing */
            break;
        default:
            ret = EINVAL_RBSP;
            break;
        }

        if (ESUCCESS == ret)
        {
            p_info_ch->audio_bit_rx = sudio_bit;
            SPDIF.CTRL.BIT.RASS = p_info_ch->audio_bit_rx;
        }
    }

    return ret;
}


/******************************************************************************
Private functions
******************************************************************************/

/******************************************************************************
* Function Name: SPDIF_InitChannel
* @brief         Initialize for the SPDIF channel
*
*                Description:<br>
*                Create semaphore and queue for channel.<br>
*                And setup SPDIF pin.
* @param[in,out] p_info_ch  :channel object
* @retval        ESUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
static int_t SPDIF_InitChannel(spdif_info_ch_t* const p_info_ch)
{
    int32_t os_ret;
    int_t ercd = ESUCCESS;

    if (NULL == p_info_ch)
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        /* Create sem_access semaphore */
        p_info_ch->sem_access = osSemaphoreNew(0xffff, 1, osSemaphore(spdif_ch0_access));

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

            p_info_ch->tlcs = SPDIF_STATUS_LCH;
            p_info_ch->trcs = SPDIF_STATUS_RCH;
            p_info_ch->rlcs = 0;
            p_info_ch->rrcs = 0;

            p_info_ch->s_aio_set_ch_sts.aio_fildes = 0;
            p_info_ch->s_aio_set_ch_sts.aio_buf    = &p_info_ch->ch_sts[0];
            p_info_ch->s_aio_set_ch_sts.aio_nbytes = 0;
            p_info_ch->s_aio_set_ch_sts.aio_offset = 0;
            p_info_ch->s_aio_set_ch_sts.aio_return = SPDIF_ASYNC_SET_CH_STS;
            p_info_ch->s_aio_set_ch_sts.aio_sigevent.sigev_notify = SIGEV_NONE;
            p_info_ch->s_aio_set_ch_sts.aio_sigevent.sigev_value.sival_ptr = (void*)NULL;
            p_info_ch->s_aio_set_ch_sts.aio_sigevent.sigev_notify_function = NULL;
            p_info_ch->ch_sts_set_req = false;
            p_info_ch->ch_sts[0] = SPDIF_STATUS_LCH;
            p_info_ch->ch_sts[1] = SPDIF_STATUS_RCH;
        }

        if (ESUCCESS == ercd)
        {
            ercd = R_SPDIF_Userdef_InitPinMux();
        }

        if (ESUCCESS == ercd)
        {
            p_info_ch->ch_stat = SPDIF_CHSTS_INIT;
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
* Function Name: SPDIF_UnInitChannel
* @brief         Uninitialise the SPDIF channel.
*
*                Description:<br>
*
* @param[in,out] p_info_ch  :channel object
* @retval        none
******************************************************************************/
static void SPDIF_UnInitChannel(spdif_info_ch_t* const p_info_ch)
{
    int32_t os_ret;

    if (NULL == p_info_ch)
    {
        /* NON_NOTICE_ASSERT: illegal pointer */
    }
    else
    {
        p_info_ch->ch_stat = SPDIF_CHSTS_UNINIT;

        SPDIF.CTRL.LONG = 0;

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

        GIC_DisableIRQ(SPDIFI_IRQn);
        InterruptHandlerUnregister(SPDIFI_IRQn);

        core_util_critical_section_exit();
    }

    return;
}

/******************************************************************************
* Function Name: SPDIF_SetCtrlParams
* @brief         Set SPDIF configuration to hardware.
*
*                Description:<br>
*                Update SSICR register.
* @param[in]     p_info_ch  :channel object
* @retval        ESUCCESS   :Success.
* @retval        error code :Failure.
******************************************************************************/
static int_t SPDIF_SetCtrlParams(const spdif_info_ch_t* const p_info_ch)
{
    int_t ret = ESUCCESS;

    if (NULL == p_info_ch)
    {
        ret = EFAULT_RBSP;
    }
    else
    {
        /* ---- Reset the register ---- */
        /* STAT - Status Register  */
        /* CTRL - Control Register */
        SPDIF.STAT.LONG = 0x00000000;
        SPDIF.CTRL.LONG = 0x00000000;

        /* ---- CTRL Register Setting ---- */
        SPDIF.CTRL.LONG = (SPDIF_CTRL_BASE_INIT_VALUE
                        | (p_info_ch->clk_select << SPDIF_CTRL_CKS_SHIFT)
                        | (p_info_ch->audio_bit_rx << SPDIF_CTRL_RASS_SHIFT)
                        | (p_info_ch->audio_bit_tx << SPDIF_CTRL_TASS_SHIFT)
#if(0) //test debug
                        | (SPDIF_CTRL_PB)
#endif
                        );
    }

    return ret;
}

/******************************************************************************
* Function Name: SPDIF_CheckChannelCfg
* @brief         Check channel configuration parameters are valid or not.
*
*                Description:<br>
*
* @param[in]     p_ch_cfg     :channel configuration
* @retval        ESUCCESS     :Success.
* @retval        error code   :Failure.
******************************************************************************/
static int_t SPDIF_CheckChannelCfg(const spdif_channel_cfg_t* const p_ch_cfg)
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
        case SPDIF_CFG_CKS_AUDIO_X1:
            /* fall through */
        case SPDIF_CFG_CKS_AUDIO_CLK:
            /* do nothing */
            break;
        default:
            ret = EINVAL_RBSP;
            break;
        }

        if (ESUCCESS == ret)
        {
            switch (p_ch_cfg->audio_bit_tx)
            {
            case SPDIF_CFG_AUDIO_BIT_16:
                /* fall through */
            case SPDIF_CFG_AUDIO_BIT_20:
                /* fall through */
            case SPDIF_CFG_AUDIO_BIT_24:
                /* do nothing */
                break;
            default:
                ret = EINVAL_RBSP;
                break;
            }
        }

        if (ESUCCESS == ret)
        {
            switch (p_ch_cfg->audio_bit_rx)
            {
            case SPDIF_CFG_AUDIO_BIT_16:
                /* fall through */
            case SPDIF_CFG_AUDIO_BIT_20:
                /* fall through */
            case SPDIF_CFG_AUDIO_BIT_24:
                /* do nothing */
                break;
            default:
                ret = EINVAL_RBSP;
                break;
            }
        }
    }

    return ret;
}

/******************************************************************************
* Function Name: SPDIF_Handler
* @brief         SPDIF error interrupt handler per channel
*
*                Description:<br>
*
* @param         none
* @retval        none
******************************************************************************/
static void SPDIF_Handler(void)
{
#if(1) /* mbed */
    spdif_info_drv_t* const p_info_drv = SPDIF_GetDrvInstanc();
    spdif_info_ch_t* const p_info_ch = &p_info_drv->info_ch;
#else
    spdif_info_ch_t* const p_info_ch = &g_spdif_info_drv.info_ch;
#endif /* end mbed */
    volatile uint32_t dummy_buf_32;
    uint32_t status;
    bool     err_flg;

    err_flg = false;

    /* ==== Read the status ==== */
    status = SPDIF.STAT.LONG;

    /* ==== Transmitter Error interrupt ==== */
    if ((status & (SPDIF_STAT_UBU | SPDIF_STAT_CSE | SPDIF_STAT_ABU)) != 0u)
    {
        err_flg = true;
        SPDIF_ErrorRecovery_tx(p_info_ch);
    }

    /* ==== Receiver Error interrupt ==== */
    if ((status & ( SPDIF_STAT_UBO  |
                    SPDIF_STAT_CE   |
                    SPDIF_STAT_PARE |
                    SPDIF_STAT_PREE |
                    SPDIF_STAT_ABO) ) != 0u)
    {
        err_flg = true;
        SPDIF_ErrorRecovery_rx(p_info_ch);
    }

    if (false == err_flg)
    {
        /* ==== Transmitter Channel Status interrupt (CSTX) ==== */
        if ((status & SPDIF_STAT_CSTX) != 0u)
        {
            SPDIF.CTRL.BIT.NCSI = 0;
            SPDIF_ChannelStatusRegSet(p_info_ch);
            SPDIF_ChannelStatusRegSetWait();
        }

        /* ==== Transmitter User Information interrupt (TUIR) ==== */
        if ((status & SPDIF_STAT_TUIR) != 0u)
        {
            /* ---- Write the user data ---- */
            SPDIF_UserInfoRegSet(p_info_ch);
            SPDIF_UserInfoRegSetWait();
        }

        /* ==== Receiver User Information Interrupt (RUIR) ==== */
        if ((status & SPDIF_STAT_RUIR) != 0u)
        {
            /* ---- Read the user data ---- */
            (void)SPDIF_UserInfoRegGet(p_info_ch);
        }

        /* ==== Receiver Channel Status Interrupt (CSRX) ==== */
        if ((status & SPDIF_STAT_CSRX) != 0u)
        {
            /* ---- Read the status data ---- */
            SPDIF_ChannelStatusRegGet(p_info_ch);
        }
    }

    /* Wait to clear request */
    dummy_buf_32 = SPDIF.STAT.LONG;
    (void)dummy_buf_32;


    return;
}

/*******************************************************************************
* Function Name: SPDIF_UserInfoRegSet
* Description  : This function set User Infomation
* Arguments    : none
* Return Value : DEVDRV_SUCCESS -
*                    User information registration success
*                DEVDRV_ERROR -
*                    User information registration error
*                DEVDRV_QUEUE_EMPTY -
*                    Since queue is empty, default user information is registered. 
*******************************************************************************/
void SPDIF_UserInfoRegSet(spdif_info_ch_t* const p_info_ch)
{
    if ((p_info_ch->p_aio_tx_next != NULL)
     && (p_info_ch->p_aio_tx_next->aio_return == SPDIF_ASYNC_STRUCT_W))
    {
        spdif_t * p_wk_spdif = (spdif_t*)p_info_ch->p_aio_tx_next->aio_buf;
        SPDIF.TUI.LONG = p_wk_spdif->u_buff[p_info_ch->tu_idx];
        p_info_ch->tu_idx++;
        if (p_info_ch->tu_idx >= SPDIF_USER_BUFFSIZE)
        {
            p_info_ch->tu_idx = 0;
        }
    }
    else
    {
        SPDIF.TUI.LONG = 0x00000000;
    }
}

/*******************************************************************************
* Function Name: SPDIF_UserInfoRegSetWait
* Description  : This function wait until Transmission module user info register
*                becomes to the full.
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void SPDIF_UserInfoRegSetWait(void)
{
    uint32_t counter;

    /* ---- Wait ---- */
    for (counter = 0; counter < SPDIF_UPDATA_WAIT; counter++)
    {
        if (SPDIF.STAT.BIT.TUIR == 0)
        {
            /* Wait for the TUIR bit is cleared */
            /* maximum of one frame is completed*/
            break;
        }
    }
}

/*******************************************************************************
* Function Name: SPDIF_UserInfoRegGet
* Description  : This function get User Infomation
* Arguments    : none
* Return Value : DEVDRV_SUCCESS -
*                    User Infomation registration success
*                DEVDRV_ERROR -
*                    Operation error
*                DEVDRV_QUEUE_EMPTY -
*                    queue is empty, Operation error. 
*******************************************************************************/
void SPDIF_UserInfoRegGet(spdif_info_ch_t* const p_info_ch)
{
    volatile uint32_t dummy_buf_32;

    if ((p_info_ch->p_aio_rx_next != NULL)
     && (p_info_ch->p_aio_rx_next->aio_return == SPDIF_ASYNC_STRUCT_R))
    {
        spdif_t * p_wk_spdif = (spdif_t*)p_info_ch->p_aio_rx_next->aio_buf;
        p_wk_spdif->u_buff[p_info_ch->ru_idx] = SPDIF.RUI.LONG;
        p_info_ch->ru_idx++;
        if (p_info_ch->ru_idx >= SPDIF_USER_BUFFSIZE)
        {
            p_info_ch->ru_idx = 0;
        }
    }
    else
    {
        dummy_buf_32 = SPDIF.RUI.LONG;
        (void)dummy_buf_32;
    }
}

/*******************************************************************************
* Function Name: SPDIF_ChannelStatusRegSet
* Description  : This function set Channel Status information
* Arguments    : none
* Return Value : DEVDRV_SUCCESS -
*                    Channel Status registration success
*                DEVDRV_QUEUE_EMPTY -
*                    Since queue is empty, default Channel Status information is registered. 
*******************************************************************************/
static void SPDIF_ChannelStatusRegSet(const spdif_info_ch_t* const p_info_ch)
{
    SPDIF.TLCS.LONG = p_info_ch->tlcs;
    SPDIF.TRCS.LONG = p_info_ch->trcs;
}

/*******************************************************************************
* Function Name: SPDIF_ChannelStatusRegSetWait
* Description  : This function wait until transmission module channel status register
*                becomes to the full.
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void SPDIF_ChannelStatusRegSetWait(void)
{
    uint32_t counter;

    /* ---- Wait ---- */
    for (counter = 0; counter < SPDIF_UPDATA_WAIT; counter++)
    {
        if (SPDIF.STAT.BIT.CSTX == 0)
        {
            /* Wait for register full */
            break;
        }
    }
}

/*******************************************************************************
* Function Name: SPDIF_ChannelStatusRegGet
* Description  : This function get Channel Status
* Arguments    : none
* Return Value : DEVDRV_SUCCESS -
*                    Channel Status Infomation registration success
*                DEVDRV_ERROR -
*                    Operation error
*                DEVDRV_QUEUE_EMPTY -
*                    queue is empty, Operation error. 
*******************************************************************************/
static void SPDIF_ChannelStatusRegGet(spdif_info_ch_t* const p_info_ch)
{
    /* ==== Get Channel Status  ==== */
    p_info_ch->rlcs = SPDIF.RLCS.LONG;
    p_info_ch->rrcs = SPDIF.RRCS.LONG;
}

