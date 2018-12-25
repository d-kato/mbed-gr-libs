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
* Copyright (C) 2013-2014 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

/**************************************************************************//**
* @file         scux_int.c
* $Rev: $
* $Date::                           $
* @brief        SCUX Driver interrupt functions
******************************************************************************/

/*******************************************************************************
Includes <System Includes>, "Project Includes"
*******************************************************************************/
#include "scux.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

static void R_SCUX_Fdi0InterruptHandler(void);
static void R_SCUX_Fdi1InterruptHandler(void);
static void R_SCUX_Fdi2InterruptHandler(void);
static void R_SCUX_Fdi3InterruptHandler(void);
static void R_SCUX_FdiHandlerProcess(const scux_ch_num_t channel);

static void R_SCUX_Fui0InterruptHandler(void);
static void R_SCUX_Fui1InterruptHandler(void);
static void R_SCUX_Fui2InterruptHandler(void);
static void R_SCUX_Fui3InterruptHandler(void);
static void R_SCUX_FuiHandlerProcess(const scux_ch_num_t channel);

static void R_SCUX_Dvi0InterruptHandler(void);
static void R_SCUX_Dvi1InterruptHandler(void);
static void R_SCUX_Dvi2InterruptHandler(void);
static void R_SCUX_Dvi3InterruptHandler(void);
static void R_SCUX_DviHandlerProcess(const scux_ch_num_t channel);

static void R_SCUX_Ai0InterruptHandler(void);
static void R_SCUX_Ai1InterruptHandler(void);
static void R_SCUX_AiHandlerProcess(const uint32_t src_channel);

static void R_SCUX_ErrHandlerProcess(scux_info_ch_t * const p_scux_info_ch, const int_t ercd);

/**************************************************************************//**
* Function Name: SCUX_InterruptInit
* @brief         Initialize SCUX interrupts.
*
*                Description:<br>
*
* @param[in] channel : SCUX channel number.
* @retval        None.
******************************************************************************/

void SCUX_InterruptInit(const int_t channel)
{
    uint32_t int_type;
    uint32_t ercd;
    scux_info_ch_t * const p_scux_info_ch = SCUX_GetDrvChInfo(channel);

    /* interrupt information table define */
    static const scux_info_int_t scux_int_info[SCUX_CH_NUM][SCUX_INT_MAX] =
    {
        {                                                                 /* ch0 */
            {&R_SCUX_Fdi0InterruptHandler, SCUFDI0_IRQn, SCUX_INT_LEVEL}, /* FDI */
            {&R_SCUX_Fui0InterruptHandler, SCUFUI0_IRQn, SCUX_INT_LEVEL}, /* FUI */
            {&R_SCUX_Dvi0InterruptHandler, SCUDVI0_IRQn, SCUX_INT_LEVEL}, /* DVI */
            {&R_SCUX_Ai0InterruptHandler,  SCUAI0_IRQn,  SCUX_INT_LEVEL}  /* AI */
        },
        {                                                                 /* ch1 */
            {&R_SCUX_Fdi1InterruptHandler, SCUFDI1_IRQn, SCUX_INT_LEVEL}, /* FDI */
            {&R_SCUX_Fui1InterruptHandler, SCUFUI1_IRQn, SCUX_INT_LEVEL}, /* FUI */
            {&R_SCUX_Dvi1InterruptHandler, SCUDVI1_IRQn, SCUX_INT_LEVEL}, /* DVI */
            {&R_SCUX_Ai0InterruptHandler,  SCUAI0_IRQn,  SCUX_INT_LEVEL}  /* AI */
        },
        {                                                                 /* ch2 */
            {&R_SCUX_Fdi2InterruptHandler, SCUFDI2_IRQn, SCUX_INT_LEVEL}, /* FDI */
            {&R_SCUX_Fui2InterruptHandler, SCUFUI2_IRQn, SCUX_INT_LEVEL}, /* FUI */
            {&R_SCUX_Dvi2InterruptHandler, SCUDVI2_IRQn, SCUX_INT_LEVEL}, /* DVI */
            {&R_SCUX_Ai1InterruptHandler,  SCUAI1_IRQn,  SCUX_INT_LEVEL}  /* AI */
        },
        {                                                                 /* ch3 */
            {&R_SCUX_Fdi3InterruptHandler, SCUFDI3_IRQn, SCUX_INT_LEVEL}, /* FDI */
            {&R_SCUX_Fui3InterruptHandler, SCUFUI3_IRQn, SCUX_INT_LEVEL}, /* FUI */
            {&R_SCUX_Dvi3InterruptHandler, SCUDVI3_IRQn, SCUX_INT_LEVEL}, /* DVI */
            {&R_SCUX_Ai1InterruptHandler,  SCUAI1_IRQn,  SCUX_INT_LEVEL}  /* AI */
        }
    };

    if (NULL == p_scux_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        for (int_type = SCUX_INT_FDI; int_type < SCUX_INT_MAX; int_type++)
        {
            ercd = InterruptHandlerRegister(scux_int_info[channel][int_type].int_num,
                                     scux_int_info[channel][int_type].int_handler
                                     );

            if (0U != ercd)
            {
                /* NON_NOTICE_ASSERT: interrupt handler regist error */
            }

            /* set interrupt level (1-N model) */
            GIC_SetConfiguration(scux_int_info[channel][int_type].int_num, 1);

            /* set interrupt priority */
            GIC_SetPriority(scux_int_info[channel][int_type].int_num, p_scux_info_ch->int_level);

            /* get interrupt number */
            p_scux_info_ch->int_num[int_type] = scux_int_info[channel][int_type].int_num;
        }
    }

    return;
}

/******************************************************************************
End of function SCUX_InterrptInit
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_InterruptUntinit
* @brief         Uninitialize SCUX interrupts.
*
*                Description:<br>
*
* @param[in/out] channel : SCUX channel number.
* @retval        None.
******************************************************************************/

void SCUX_InterruptUninit(const int_t channel)
{
    uint32_t int_type;
    uint32_t ercd;
    scux_info_ch_t * const p_scux_info_ch = SCUX_GetDrvChInfo(channel);

    if (NULL == p_scux_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        for (int_type = SCUX_INT_FDI; int_type < SCUX_INT_MAX; int_type++)
        {
            GIC_DisableIRQ(p_scux_info_ch->int_num[int_type]);
            ercd = InterruptHandlerUnregister(p_scux_info_ch->int_num[int_type]);

            if (0U != ercd)
            {
                /* NON_NOTICE_ASSERT: interrupt handler unregist error */
            }
        }
    }

    return;
}

/******************************************************************************
End of function SCUX_InterruptUnInit
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_Fdi0InterruptHandler
* @brief         FDI interrupt handler on channel 0.
*
*                Description:<br>
*
* @param         None.
* @retval        None.
******************************************************************************/

static void R_SCUX_Fdi0InterruptHandler(void)
{

    R_SCUX_FdiHandlerProcess(SCUX_CH_0);

}

/******************************************************************************
End of function R_SCUX_Fdi0InterruptHandler
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_Fdi1InterruptHandler
* @brief         FDI interrupt handler on channel 1.
*
*                Description:<br>
*
* @param         None.
* @retval        None.
******************************************************************************/

static void R_SCUX_Fdi1InterruptHandler(void)
{

    R_SCUX_FdiHandlerProcess(SCUX_CH_1);

}

/******************************************************************************
End of function R_SCUX_Fdi1InterruptHandler
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_Fdi2InterruptHandler
* @brief         FDI interrupt handler on channel 2.
*
*                Description:<br>
*
* @param         None.
* @retval        None.
******************************************************************************/

static void R_SCUX_Fdi2InterruptHandler(void)
{

    R_SCUX_FdiHandlerProcess(SCUX_CH_2);

}

/******************************************************************************
End of function R_SCUX_Fdi2InterruptHandler
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_Fdi3InterruptHandler
* @brief         FDI interrupt handler on channel 3.
*
*                Description:<br>
*
* @param         None.
* @retval        None.
******************************************************************************/

static void R_SCUX_Fdi3InterruptHandler(void)
{

    R_SCUX_FdiHandlerProcess(SCUX_CH_3);

}

/******************************************************************************
End of function R_SCUX_Fdi3InterruptHandler
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_FdiHandlerProcess
* @brief         Processing FDI interrupt handler.
*
*                Description:<br>
*
* @param[in] channel:SCUX channel number.
* @retval        None.
******************************************************************************/

static void R_SCUX_FdiHandlerProcess(const scux_ch_num_t channel)
{
    scux_info_ch_t * const p_info_ch = SCUX_GetDrvChInfo((int_t)channel);
    int_t ercd;

    if (NULL == p_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        /* check error type */
        if ((p_info_ch->p_scux_reg->p_ffd_reg->DEVCR_FFD0_0 & DEVCR_FFD0_DEVCOL_MASK) != 0)
        {
            /* critical error (over lap) */
            ercd = EIO_RBSP;
        }
        else
        {
            /* be returned error (FFD under flow, over flow) */
            ercd = EOVERFLOW_RBSP;
        }

        p_info_ch->p_scux_reg->p_ffd_reg->DEVCR_FFD0_0 &= ~(DEVCR_FFD0_DEVCUF_SET |
                                                            DEVCR_FFD0_DEVCOF_SET |
                                                            DEVCR_FFD0_DEVCOL_SET |
                                                            DEVCR_FFD0_DEVCIUF_SET);

        R_SCUX_ErrHandlerProcess(p_info_ch, ercd);
    }

}

/******************************************************************************
End of function R_SCUX_FdiHandlerProcess
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_Fui0InterruptHandler
* @brief         FUI interrupt handler on channel 0.
*
*                Description:<br>
*
* @param         None.
* @retval        None.
******************************************************************************/

static void R_SCUX_Fui0InterruptHandler(void)
{

    R_SCUX_FuiHandlerProcess(SCUX_CH_0);

}

/******************************************************************************
End of function R_SCUX_Fui0InterruptHandler
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_Fui1InterruptHandler
* @brief         FUI interrupt handler on channel 1.
*
*                Description:<br>
*
* @param         None.
* @retval        None.
******************************************************************************/

static void R_SCUX_Fui1InterruptHandler(void)
{

    R_SCUX_FuiHandlerProcess(SCUX_CH_1);

}

/******************************************************************************
End of function R_SCUX_Fui1InterruptHandler
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_Fui2InterruptHandler
* @brief         FUI interrupt handler on channel 2.
*
*                Description:<br>
*
* @param         None.
* @retval        None.
******************************************************************************/

static void R_SCUX_Fui2InterruptHandler(void)
{

    R_SCUX_FuiHandlerProcess(SCUX_CH_2);

}

/******************************************************************************
End of function R_SCUX_Fui2InterruptHandler
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_Fui3InterruptHandler
* @brief         FUI interrupt handler on channel 3.
*
*                Description:<br>
*
* @param         None.
* @retval        None.
******************************************************************************/

static void R_SCUX_Fui3InterruptHandler(void)
{

    R_SCUX_FuiHandlerProcess(SCUX_CH_3);

}

/******************************************************************************
End of function R_SCUX_Fui3InterruptHandler
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_FuiHandlerProcess
* @brief         Processing FUI interrupt handler.
*
*                Description:<br>
*
* @param[in] channel:SCUX channel number.
* @retval        None.
******************************************************************************/

static void R_SCUX_FuiHandlerProcess(const scux_ch_num_t channel)
{
    scux_info_ch_t * const p_info_ch = SCUX_GetDrvChInfo((int_t)channel);
    int_t ercd;

    if (NULL == p_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        /* check error type */
        if ((p_info_ch->p_scux_reg->p_ffu_reg->UEVCR_FFU0_0 & UEVCR_FFU0_UEVCOL_MASK) != 0)
        {
            /* critical error (over lap) */
            ercd = EIO_RBSP;
        }
        else
        {
            /* be returned error (FFU under flow, over flow) */
            ercd = EOVERFLOW_RBSP;
        }

        p_info_ch->p_scux_reg->p_ffu_reg->UEVCR_FFU0_0 &= ~(UEVCR_FFU0_UEVCUF_SET |
                                                            UEVCR_FFU0_UEVCOF_SET |
                                                            UEVCR_FFU0_UEVCOL_SET);

        R_SCUX_ErrHandlerProcess(p_info_ch, ercd);

    }

}

/******************************************************************************
End of function R_SCUX_FuiHandlerProcess
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_Dvi0InterruptHandler
* @brief         DVI interrupt handler on channel 0.
*
*                Description:<br>
*
* @param         None.
* @retval        None.
******************************************************************************/

static void R_SCUX_Dvi0InterruptHandler(void)
{

    R_SCUX_DviHandlerProcess(SCUX_CH_0);

}

/******************************************************************************
End of function R_SCUX_Dvi0InterruptHandler
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_Dvi1InterruptHandler
* @brief         DVI interrupt handler on channel 1.
*
*                Description:<br>
*
* @param         None.
* @retval        None.
******************************************************************************/

static void R_SCUX_Dvi1InterruptHandler(void)
{

    R_SCUX_DviHandlerProcess(SCUX_CH_1);

}

/******************************************************************************
End of function R_SCUX_Dvi1InterruptHandler
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_Dvi2InterruptHandler
* @brief         DVI interrupt handler on channel 2.
*
*                Description:<br>
*
* @param         None.
* @retval        None.
******************************************************************************/

static void R_SCUX_Dvi2InterruptHandler(void)
{

    R_SCUX_DviHandlerProcess(SCUX_CH_2);

}

/******************************************************************************
End of function R_SCUX_Dvi2InterruptHandler
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_Dvi3InterruptHandler
* @brief         DVI interrupt handler on channel 3.
*
*                Description:<br>
*
* @param         None.
* @retval        None.
******************************************************************************/

static void R_SCUX_Dvi3InterruptHandler(void)
{

    R_SCUX_DviHandlerProcess(SCUX_CH_3);

}

/******************************************************************************
End of function R_SCUX_Dvi3InterruptHandler
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_DviHandlerProcess
* @brief         Processing DVI interrupt handler.
*
*                Description:<br>
*
* @param[in] channel:SCUX channel number.
* @retval        None.
******************************************************************************/

static void R_SCUX_DviHandlerProcess(const scux_ch_num_t channel)
{
    uint32_t stat_zerocross;
    int_t    ch_zerocross;
    scux_info_ch_t * const p_info_ch = SCUX_GetDrvChInfo((int_t)channel);

    if (NULL == p_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        stat_zerocross = p_info_ch->p_scux_reg->p_dvu_reg->VEVCR_DVU0_0;

        for (ch_zerocross = 0; ch_zerocross <= p_info_ch->src_cfg.use_ch; ch_zerocross++)
        {
            /* check zerocross bit */
            if (0U != (stat_zerocross & (VEVCR_DVU0_VEVCZCM_SET << ch_zerocross)))
            {
                p_info_ch->dvu_mute_stat |= (1U << ch_zerocross);
                if (NULL != p_info_ch->dvu_cfg.dvu_zc_mute.pcallback[ch_zerocross])
                {
                    /* callback function */
                    p_info_ch->dvu_cfg.dvu_zc_mute.pcallback[ch_zerocross]();
                }
            }
            p_info_ch->p_scux_reg->p_dvu_reg->VEVCR_DVU0_0 &= ~stat_zerocross;
        }
    }

}

/******************************************************************************
End of function R_SCUX_DviHandlerProcess
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_Ai0InterruptHandler
* @brief         AI interrupt handler on channel 0.
*
*                Description:<br>
*
* @param         None.
* @retval        None.
******************************************************************************/

static void R_SCUX_Ai0InterruptHandler(void)
{

    R_SCUX_AiHandlerProcess(SRC_MOD_SRC0);

}

/******************************************************************************
End of function R_SCUX_Ai0InterruptHandler
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_Ai1InterruptHandler
* @brief         AI interrupt handler on channel 1.
*
*                Description:<br>
*
* @param         None.
* @retval        None.
******************************************************************************/

static void R_SCUX_Ai1InterruptHandler(void)
{

    R_SCUX_AiHandlerProcess(SRC_MOD_SRC1);

}

/******************************************************************************
End of function R_SCUX_Ai1InterruptHandler
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_AiHandlerProcess
* @brief         Processing AI interrupt handler.
*
*                Description:<br>
*
* @param[in] channel:SCUX channel number.
* @retval        None.
******************************************************************************/

static void R_SCUX_AiHandlerProcess(const uint32_t src_channel)
{
    scux_info_ch_t *p_info_ch_check;
    scux_info_ch_t *p_info_ch;

    if (SRC_MOD_SRC0 == src_channel)
    {
        p_info_ch_check = SCUX_GetDrvChInfo(SCUX_CH_0);
        if (NULL == p_info_ch_check)
        {
            /* NON_NOTICE_ASSERT: NULL pointer */
        }
        else
        {
            if (0U != p_info_ch_check->p_scux_reg->p_src_reg->SEVCR0_2SRC0_0)
            {
                p_info_ch = SCUX_GetDrvChInfo(SCUX_CH_0);
                if (NULL == p_info_ch)
                {
                    /* NON_NOTICE_ASSERT: NULL pointer */
                }
                else
                {
                    p_info_ch->p_scux_reg->p_src_reg->SEVCR0_2SRC0_0 &= ~(SEVCR_2SRC0_EVCUF_SET |
                                                                          SEVCR_2SRC0_EVCOF_SET);
                    /* be returned error (FFU under flow, over flow) */
                    R_SCUX_ErrHandlerProcess(p_info_ch, EOVERFLOW_RBSP);
                }
            }

            if (0U != p_info_ch_check->p_scux_reg->p_src_reg->SEVCR1_2SRC0_0)
            {

                p_info_ch = SCUX_GetDrvChInfo(SCUX_CH_1);
                if (NULL == p_info_ch)
                {
                    /* NON_NOTICE_ASSERT: NULL pointer */
                }
                else
                {
                    p_info_ch->p_scux_reg->p_src_reg->SEVCR1_2SRC0_0 &= ~(SEVCR_2SRC0_EVCUF_SET |
                                                                          SEVCR_2SRC0_EVCOF_SET);
                    /* be returned error (FFU under flow, over flow) */
                    R_SCUX_ErrHandlerProcess(p_info_ch, EOVERFLOW_RBSP);
                }
            }
        }
    }
    else
    {
        p_info_ch_check = SCUX_GetDrvChInfo(SCUX_CH_2);
        if (NULL == p_info_ch_check)
        {
            /* NON_NOTICE_ASSERT: NULL pointer */
        }
        else
        {
            if (0U != p_info_ch_check->p_scux_reg->p_src_reg->SEVCR0_2SRC0_0)
            {
                p_info_ch = SCUX_GetDrvChInfo(SCUX_CH_2);
                if (NULL == p_info_ch)
                {
                    /* NON_NOTICE_ASSERT: NULL pointer */
                }
                else
                {
                    p_info_ch->p_scux_reg->p_src_reg->SEVCR0_2SRC0_0 &= ~(SEVCR_2SRC0_EVCUF_SET |
                                                                          SEVCR_2SRC0_EVCOF_SET);
                    /* be returned error (FFU under flow, over flow) */
                    R_SCUX_ErrHandlerProcess(p_info_ch, EOVERFLOW_RBSP);
                }
            }

            if (0U != p_info_ch_check->p_scux_reg->p_src_reg->SEVCR1_2SRC0_0)
            {

                p_info_ch = SCUX_GetDrvChInfo(SCUX_CH_3);
                if (NULL == p_info_ch)
                {
                    /* NON_NOTICE_ASSERT: NULL pointer */
                }
                else
                {
                    p_info_ch->p_scux_reg->p_src_reg->SEVCR1_2SRC0_0 &= ~(SEVCR_2SRC0_EVCUF_SET |
                                                                          SEVCR_2SRC0_EVCOF_SET);
                    /* be returned error (FFU under flow, over flow) */
                    R_SCUX_ErrHandlerProcess(p_info_ch, EOVERFLOW_RBSP);
                }
            }
        }
    }
}

/******************************************************************************
End of function R_SCUX_AiHandlerProcess
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_ErrHandlerProcess
* @brief         Error processing on interrupt handler.
*
*                Description:<br>
*
* @param[in] p_scux_info_ch:SCUX channel information.
* @param[in] ercd:return error code.
* @retval        None.
******************************************************************************/

static void R_SCUX_ErrHandlerProcess(scux_info_ch_t * const p_scux_info_ch, const int_t ercd)
{
    int_t retval;
#if(1) /* mbed */
    int32_t dma_retval;
#else
    int_t dma_retval;
#endif
    scux_stat_ch_t old_stat;
    uint32_t tx_remain_size = 0;
    uint32_t rx_remain_size = 0;

    if (NULL == p_scux_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        retval = R_DMA_Cancel(p_scux_info_ch->dma_tx_ch, &tx_remain_size, &dma_retval);
        /* DMA stop check, (when dma_retval is EBADF_RBSP, DMA stopped already) */
        if ((ESUCCESS != retval) && (EBADF_RBSP != dma_retval))
        {
            /* NON_NOTICE_ASSERT: NULL pointer */
        }
        else
        {
            p_scux_info_ch->tx_fifo_total_size += p_scux_info_ch->dma_tx_current_size;
            p_scux_info_ch->dma_tx_current_size = 0;
        }

        if (SCUX_ROUTE_MEM_TO_MEM == (p_scux_info_ch->route_set & SCUX_GET_ROUTE_MASK))
        {
            retval = R_DMA_Cancel(p_scux_info_ch->dma_rx_ch, &rx_remain_size, &dma_retval);

            /* DMA stop check, (when dma_retval is EBADF_RBSP, DMA stopped already) */
            if ((ESUCCESS != retval) && (EBADF_RBSP != dma_retval))
            {
                /* NON_NOTICE_ASSERT: NULL pointer */
            }
            else
            {
                p_scux_info_ch->rx_fifo_total_size += p_scux_info_ch->dma_rx_current_size;
                p_scux_info_ch->dma_rx_current_size = 0;
            }

            retval = R_DMA_Free(p_scux_info_ch->dma_rx_ch, NULL);

            if (ESUCCESS != retval)
            {
                /* NON_NOTICE_ASSERT: NULL pointer */
            }
        }

        SCUX_AdjustAccessFifo(p_scux_info_ch, tx_remain_size, rx_remain_size);
        /* stop HW */
        if (false != p_scux_info_ch->src_cfg.mode_sync)
        {
            SCUX_SyncStopHw(p_scux_info_ch);
        }
        else
        {
            SCUX_AsyncStopHw(p_scux_info_ch);
        }

        if (false == p_scux_info_ch->first_tx_flag)
        {
            /* return callback on normal operation */
            if (NULL != p_scux_info_ch->p_tx_aio)
            {
                p_scux_info_ch->p_tx_aio->aio_return = ercd;
                ahf_complete(&p_scux_info_ch->tx_que, p_scux_info_ch->p_tx_aio);
            }
            if (NULL != p_scux_info_ch->p_tx_next_aio)
            {
                if (NULL == p_scux_info_ch->p_tx_aio)
                {
                    p_scux_info_ch->p_tx_next_aio->aio_return = ercd;
                }
                else
                {
                    p_scux_info_ch->p_tx_next_aio->aio_return = ECANCELED_RBSP;
                }
                ahf_complete(&p_scux_info_ch->tx_que, p_scux_info_ch->p_tx_next_aio);
            }
            ahf_cancelall(&p_scux_info_ch->tx_que);
            p_scux_info_ch->p_tx_aio = NULL;
            p_scux_info_ch->p_tx_next_aio = NULL;
        }
        else
        {
            /* return callback on first operation */
            if (NULL != p_scux_info_ch->p_tx_next_aio)
            {
                p_scux_info_ch->p_tx_next_aio->aio_return = ercd;
                ahf_complete(&p_scux_info_ch->tx_que, p_scux_info_ch->p_tx_next_aio);
            }
            else
            {
                p_scux_info_ch->p_tx_aio->aio_return = ercd;
                ahf_complete(&p_scux_info_ch->tx_que, p_scux_info_ch->p_tx_aio);
            }
            ahf_cancelall(&p_scux_info_ch->tx_que);
            p_scux_info_ch->p_tx_aio = NULL;
            p_scux_info_ch->p_tx_next_aio = NULL;
        }

        if (SCUX_ROUTE_MEM_TO_MEM == (p_scux_info_ch->route_set & SCUX_GET_ROUTE_MASK))
        {
            /* if request is nothing, aio is NULL */
            if (NULL != p_scux_info_ch->p_rx_aio)
            {
                p_scux_info_ch->p_rx_aio->aio_return = ercd;
                ahf_complete(&p_scux_info_ch->rx_que, p_scux_info_ch->p_rx_aio);
            }
            ahf_cancelall(&p_scux_info_ch->rx_que);
            p_scux_info_ch->p_rx_aio = NULL;
        }
        old_stat = p_scux_info_ch->ch_stat;
        p_scux_info_ch->ch_stat = SCUX_CH_STOP;
        if ((SCUX_CH_STOP_WAIT == old_stat) || (SCUX_CH_STOP_WAIT_IDLE == old_stat))
        {
            if (NULL != p_scux_info_ch->p_flush_callback)
            {
                p_scux_info_ch->p_flush_callback(ercd);
            }
        }

        /* set error status */
        p_scux_info_ch->err_stat_backup = ercd;
    }
}

/******************************************************************************
End of function R_SCUX_ErrHandlerProcess
******************************************************************************/

