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
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

/**************************************************************************//**
* @file         dma.c
* $Rev: $
* $Date::                           $
* @brief        DMA Driver internal functions
******************************************************************************/

/*****************************************************************************
* History : DD.MM.YYYY Version Description
* : 15.01.2013 1.00 First Release
******************************************************************************/

/*******************************************************************************
Includes <System Includes>, "Project Includes"
*******************************************************************************/
#include "dma.h"
#include "aioif.h"
#include "iodefine.h"
#include "mbed_critical.h"

/******************************************************************************
Private global driver management information
******************************************************************************/

/* driver management infrmation */
static dma_info_drv_t gb_info_drv;

/******************************************************************************
Private function define (interrupt handler)
******************************************************************************/

static void R_DMA_End0InterruptHandler(void);
static void R_DMA_End1InterruptHandler(void);
static void R_DMA_End2InterruptHandler(void);
static void R_DMA_End3InterruptHandler(void);
static void R_DMA_End4InterruptHandler(void);
static void R_DMA_End5InterruptHandler(void);
static void R_DMA_End6InterruptHandler(void);
static void R_DMA_End7InterruptHandler(void);
static void R_DMA_End8InterruptHandler(void);
static void R_DMA_End9InterruptHandler(void);
static void R_DMA_End10InterruptHandler(void);
static void R_DMA_End11InterruptHandler(void);
static void R_DMA_End12InterruptHandler(void);
static void R_DMA_End13InterruptHandler(void);
static void R_DMA_End14InterruptHandler(void);
static void R_DMA_End15InterruptHandler(void);
static void R_DMA_ErrInterruptHandler(void);
static void R_DMA_EndHandlerProcess(const int_t channel);

/******************************************************************************
 Function prototypes
 *****************************************************************************/

static void  DMA_OpenChannel(const int_t channel);

/******************************************************************************
* Function Name: DMA_GetDrvInstance
* Description : Get pointer of gb_info_drv.
* Arguments : *p_dma_info_drv -
*                  Pointer of gb_info_drv is returned.
* Return Value : None
******************************************************************************/

dma_info_drv_t *DMA_GetDrvInstance(void)
{

    return &gb_info_drv;
}

/******************************************************************************
End of function DMA_GetDrv_Instance
******************************************************************************/

/******************************************************************************
* Function Name: DMA_GetDrvChInfo
* Description : Get pointer of gb_info_drv.info_ch[channel].
* Arguments : *p_dma_info_drv -
*                  Pointer of gb_info_drv is returned.
* Return Value : None
******************************************************************************/

dma_info_ch_t *DMA_GetDrvChInfo(const int_t channel)
{

    return &gb_info_drv.info_ch[channel];
}

/******************************************************************************
End of function DMA_GetDrvChInfo
******************************************************************************/

/******************************************************************************
* Function Name: DMA_Initialize
* Description : Initialize DMA driver.
* Arguments : *p_dma_init_param -
*                  Pointer of init parameters.
* Return Value : ESUCCESS -
*                  Operation successful.
*                OS error num -
*                  Registering handler failed.
******************************************************************************/

int_t DMA_Initialize(const dma_drv_init_t * const p_dma_init_param)
{
    int_t    retval = ESUCCESS;
    int_t    ch_count;
    uint32_t error_code;
    bool_t   init_check_flag;

    /* ->MISRA 11.3, 11.4, IPA R3.6.2 This cast is needed for register access. */
    /* address table of register set for each channel */
    static volatile struct st_dmac_n *gb_dma_ch_register_addr_table[DMA_CH_NUM] =
        { &(*(struct st_dmac_n *)&DMAC0.N0SA_0S.LONG),
          &(*(struct st_dmac_n *)&DMAC0.N0SA_1S.LONG),
          &(*(struct st_dmac_n *)&DMAC0.N0SA_2S.LONG),
          &(*(struct st_dmac_n *)&DMAC0.N0SA_3S.LONG),
          &(*(struct st_dmac_n *)&DMAC0.N0SA_4S.LONG),
          &(*(struct st_dmac_n *)&DMAC0.N0SA_5S.LONG),
          &(*(struct st_dmac_n *)&DMAC0.N0SA_6S.LONG),
          &(*(struct st_dmac_n *)&DMAC0.N0SA_7S.LONG),
          &(*(struct st_dmac_n *)&DMAC0.N0SA_8S.LONG),
          &(*(struct st_dmac_n *)&DMAC0.N0SA_9S.LONG),
          &(*(struct st_dmac_n *)&DMAC0.N0SA_10S.LONG),
          &(*(struct st_dmac_n *)&DMAC0.N0SA_11S.LONG),
          &(*(struct st_dmac_n *)&DMAC0.N0SA_12S.LONG),
          &(*(struct st_dmac_n *)&DMAC0.N0SA_13S.LONG),
          &(*(struct st_dmac_n *)&DMAC0.N0SA_14S.LONG),
          &(*(struct st_dmac_n *)&DMAC0.N0SA_15S.LONG)
        };
    /* <-MISRA 11.3, 11.4, IPA R3.6.2*/

    /* ->MISRA 11.3, 11.4, IPA R3.6.2 This cast is needed for register access. */
    /* address table of register set for common register */
    static volatile struct st_dmaccommon_n *gb_dma_common_register_addr_table[DMA_CH_NUM] =
        { &(*(struct st_dmaccommon_n *)&DMAC0.DCTRL_0_7S.LONG),
          &(*(struct st_dmaccommon_n *)&DMAC0.DCTRL_0_7S.LONG),
          &(*(struct st_dmaccommon_n *)&DMAC0.DCTRL_0_7S.LONG),
          &(*(struct st_dmaccommon_n *)&DMAC0.DCTRL_0_7S.LONG),
          &(*(struct st_dmaccommon_n *)&DMAC0.DCTRL_0_7S.LONG),
          &(*(struct st_dmaccommon_n *)&DMAC0.DCTRL_0_7S.LONG),
          &(*(struct st_dmaccommon_n *)&DMAC0.DCTRL_0_7S.LONG),
          &(*(struct st_dmaccommon_n *)&DMAC0.DCTRL_0_7S.LONG),
          &(*(struct st_dmaccommon_n *)&DMAC0.DCTRL_8_15S.LONG),
          &(*(struct st_dmaccommon_n *)&DMAC0.DCTRL_8_15S.LONG),
          &(*(struct st_dmaccommon_n *)&DMAC0.DCTRL_8_15S.LONG),
          &(*(struct st_dmaccommon_n *)&DMAC0.DCTRL_8_15S.LONG),
          &(*(struct st_dmaccommon_n *)&DMAC0.DCTRL_8_15S.LONG),
          &(*(struct st_dmaccommon_n *)&DMAC0.DCTRL_8_15S.LONG),
          &(*(struct st_dmaccommon_n *)&DMAC0.DCTRL_8_15S.LONG),
          &(*(struct st_dmaccommon_n *)&DMAC0.DCTRL_8_15S.LONG)
        };
    /* <-MISRA 11.3, 11.4, IPA R3.6.2*/

    /* ->MISRA 11.3, 11.4 This cast is needed for register access. */
    /* address table of register set for DMARS */
    static volatile unsigned long *gb_dmars_register_addr_table[DMA_CH_NUM] =
        { &DMAC0.DMARS0S.LONG,
          &DMAC0.DMARS0S.LONG,
          &DMAC0.DMARS1S.LONG,
          &DMAC0.DMARS1S.LONG,
          &DMAC0.DMARS2S.LONG,
          &DMAC0.DMARS2S.LONG,
          &DMAC0.DMARS3S.LONG,
          &DMAC0.DMARS3S.LONG,
          &DMAC0.DMARS4S.LONG,
          &DMAC0.DMARS4S.LONG,
          &DMAC0.DMARS5S.LONG,
          &DMAC0.DMARS5S.LONG,
          &DMAC0.DMARS6S.LONG,
          &DMAC0.DMARS6S.LONG,
          &DMAC0.DMARS7S.LONG,
          &DMAC0.DMARS7S.LONG
        };
    /* <-MISRA 11.3, 11.4 */

    /* Interrpt handlers table */
    static const IRQHandler gb_dma_int_handler_table[DMA_CH_NUM] =
        { &R_DMA_End0InterruptHandler, /* DMA end interrupt for ch0 - ch15 */
          &R_DMA_End1InterruptHandler,
          &R_DMA_End2InterruptHandler,
          &R_DMA_End3InterruptHandler,
          &R_DMA_End4InterruptHandler,
          &R_DMA_End5InterruptHandler,
          &R_DMA_End6InterruptHandler,
          &R_DMA_End7InterruptHandler,
          &R_DMA_End8InterruptHandler,
          &R_DMA_End9InterruptHandler,
          &R_DMA_End10InterruptHandler,
          &R_DMA_End11InterruptHandler,
          &R_DMA_End12InterruptHandler,
          &R_DMA_End13InterruptHandler,
          &R_DMA_End14InterruptHandler,
          &R_DMA_End15InterruptHandler
        };

    /* Interrupt numbers table */
    static const IRQn_Type gb_dma_int_num_table[DMA_CH_NUM] =
        { DMAINT0_IRQn, /* DMA end interrupt for ch0 - ch15 */
          DMAINT1_IRQn,
          DMAINT2_IRQn,
          DMAINT3_IRQn,
          DMAINT4_IRQn,
          DMAINT5_IRQn,
          DMAINT6_IRQn,
          DMAINT7_IRQn,
          DMAINT8_IRQn,
          DMAINT9_IRQn,
          DMAINT10_IRQn,
          DMAINT11_IRQn,
          DMAINT12_IRQn,
          DMAINT13_IRQn,
          DMAINT14_IRQn,
          DMAINT15_IRQn
        };

    /* element of p_dma_init_param is copied to element of gb_info_drv */
    gb_info_drv.p_err_aio = p_dma_init_param->p_aio;

    /* set DMA error interrupt number */
    gb_info_drv.err_irq_num = DMAERR0_IRQn;

    /* init channel management information */
    for (ch_count = 0; ch_count < DMA_CH_NUM; ch_count++)
    {
        /* set channel number */
        gb_info_drv.info_ch[ch_count].ch = ch_count;

        /* set DMA end interrupt number */
        gb_info_drv.info_ch[ch_count].end_irq_num = gb_dma_int_num_table[ch_count];

        /* init next DMA setting flag */
        gb_info_drv.info_ch[ch_count].next_dma_flag = false;

        if (1U == ((uint32_t)ch_count & CHECK_ODD_EVEN_MASK))
        {
            /* set shift number when channel is odd value */
            gb_info_drv.info_ch[ch_count].shift_dmars = SHIFT_DMARS_ODD_CH;
            /* set mask value when channel is odd value */
            gb_info_drv.info_ch[ch_count].mask_dmars = MASK_DMARS_ODD_CH;
        }
        else
        {
            /* set shift number when channel is even value */
            gb_info_drv.info_ch[ch_count].shift_dmars = SHIFT_DMARS_EVEN_CH;
            /* set mask value when channel is even value */
            gb_info_drv.info_ch[ch_count].mask_dmars = MASK_DMARS_EVEN_CH;
        }

        /* init DMA setup flag */
        gb_info_drv.info_ch[ch_count].setup_flag = false;

        /* ->MISRA 11.4 This cast is needed for register access. */
        /* set DMA register address for each channel */
        gb_info_drv.info_ch[ch_count].p_dma_ch_reg = gb_dma_ch_register_addr_table[ch_count];
        /* set common resgiter for channel 0 - 7 */
        gb_info_drv.info_ch[ch_count].p_dma_common_reg = gb_dma_common_register_addr_table[ch_count];
        /* <-MISRA 11.4 */

        /* set DMARS register for each channel */
        gb_info_drv.info_ch[ch_count].p_dma_dmars_reg = gb_dmars_register_addr_table[ch_count];
    }

    /* init DMA registers */
    for (ch_count = 0; ch_count < DMA_CH_NUM; ch_count++)
    {
        gb_info_drv.info_ch[ch_count].p_dma_ch_reg->N0SA_n =   N0SA_INIT_VALUE;
        gb_info_drv.info_ch[ch_count].p_dma_ch_reg->N1SA_n =   N1SA_INIT_VALUE;
        gb_info_drv.info_ch[ch_count].p_dma_ch_reg->N0DA_n =   N0DA_INIT_VALUE;
        gb_info_drv.info_ch[ch_count].p_dma_ch_reg->N1DA_n =   N1DA_INIT_VALUE;
        gb_info_drv.info_ch[ch_count].p_dma_ch_reg->N0TB_n =   N0TB_INIT_VALUE;
        gb_info_drv.info_ch[ch_count].p_dma_ch_reg->N1TB_n =   N1TB_INIT_VALUE;
        gb_info_drv.info_ch[ch_count].p_dma_ch_reg->CHCTRL_n = CHCTRL_INIT_VALUE;
        gb_info_drv.info_ch[ch_count].p_dma_ch_reg->CHCFG_n =  CHCFG_INIT_VALUE;
        /* set DMA interval = 0 */
        gb_info_drv.info_ch[ch_count].p_dma_ch_reg->CHITVL_n = CHITVL_INIT_VALUE;
        gb_info_drv.info_ch[ch_count].p_dma_ch_reg->CHEXT_n =  CHEXT_INIT_VALUE;
        gb_info_drv.info_ch[ch_count].p_dma_ch_reg->NXLA_n =   NXLA_INIT_VALUE;
        *(gb_info_drv.info_ch[ch_count].p_dma_dmars_reg)   =   DMARS_INIT_VALUE;
    }
    /* init common resgiter for channel 0 - 7 */
    /* set interrupt output : pulse,
       set round robin mode  */
    gb_info_drv.info_ch[DMA_CH_0].p_dma_common_reg->DCTRL_0_7 = DCTRL_INIT_VALUE;
    /* init common resgiter for channel 8 - 15 */
    /* set interrupt output : pulse,
       set round robin mode */
    gb_info_drv.info_ch[HIGH_COMMON_REG_OFFSET].p_dma_common_reg->DCTRL_0_7 = DCTRL_INIT_VALUE;

    if (ESUCCESS == retval)
    {
        /* DMA end interrupt handler register */
        init_check_flag = false;
        ch_count = 0;
        while (false == init_check_flag)
        {
            error_code = InterruptHandlerRegister(gb_info_drv.info_ch[ch_count].end_irq_num,
                                                  gb_dma_int_handler_table[ch_count]
                                                  );
//            GIC_SetPriority(gb_info_drv.info_ch[ch_count].end_irq_num, 0x80);
            /* 0 is no error on InterruptHandlerRegister() */
            if (0U != error_code)
            {
                retval = (int_t)error_code;
                init_check_flag = true;
            }
            if ((DMA_CH_NUM - 1) == ch_count)
            {
                init_check_flag = true;
            }
            ch_count++;
        }

        if (ESUCCESS == retval)
        {
            /* DMA error interrupt handler register */
            error_code = InterruptHandlerRegister(gb_info_drv.err_irq_num,
                                                  &R_DMA_ErrInterruptHandler
                                                 );
#if(1) /* mbed */
            GIC_SetPriority(gb_info_drv.err_irq_num, 0x80);
#endif
            /* 0 is no error on InterruptHandlerRegister() */
            if (0U != error_code)
            {
                retval = (int_t)error_code;
            }
        }
    }

    if (ESUCCESS == retval)
    {
        /* set DMA end interrupt level & priority */
        for (ch_count = 0; ch_count < DMA_CH_NUM; ch_count++)
        {
            /* set interrupt level (set edge trigger, 1-N model) */
            GIC_SetConfiguration(gb_info_drv.info_ch[ch_count].end_irq_num, 3);
        }
        /* set DMA error interrupt level (set edge trgger, 1-N model) */
        GIC_SetConfiguration(gb_info_drv.err_irq_num, 3);
        /* DMA error interrupt enable */
        GIC_EnableIRQ(gb_info_drv.err_irq_num);
    }

    if (ESUCCESS == retval)
    {
        /* set channel status */
        for (ch_count = 0; ch_count < DMA_CH_NUM; ch_count++)
        {
            if ((bool_t)false != p_dma_init_param->channel[ch_count])
            {
                gb_info_drv.info_ch[ch_count].ch_stat = DMA_CH_INIT;
            }
            else
            {
                gb_info_drv.info_ch[ch_count].ch_stat = DMA_CH_UNINIT;
            }
        }
        /* set driver status to DMA_DRV_INIT */
        gb_info_drv.drv_stat = DMA_DRV_INIT;
    }

    return retval;
}

/******************************************************************************
End of function DMA_Initialize
******************************************************************************/

/******************************************************************************
* Function Name: DMA_UnInitialize
* Description : UnInitialize DMA driver.
* Arguments : None.
* Return Value : ESUCCESS -
*                  Operation successful.
*                OS error num -
*                  Unregistering handler failed
******************************************************************************/

int_t DMA_UnInitialize(void)
{
    int_t            retval = ESUCCESS;
    int_t            ch_count;
    uint32_t         error_code;
    bool_t           uninit_check_flag;

    /* init DMA registers */
    for (ch_count = 0; ch_count < DMA_CH_NUM; ch_count++)
    {
        gb_info_drv.info_ch[ch_count].p_dma_ch_reg->CHCTRL_n = 0;
        gb_info_drv.info_ch[ch_count].p_dma_ch_reg->CHCFG_n =  0;
        *(gb_info_drv.info_ch[ch_count].p_dma_dmars_reg) =   0;
        gb_info_drv.info_ch[ch_count].p_dma_ch_reg->N0SA_n =   0;
        gb_info_drv.info_ch[ch_count].p_dma_ch_reg->N1SA_n =   0;
        gb_info_drv.info_ch[ch_count].p_dma_ch_reg->N0DA_n =   0;
        gb_info_drv.info_ch[ch_count].p_dma_ch_reg->N1DA_n =   0;
        gb_info_drv.info_ch[ch_count].p_dma_ch_reg->N0TB_n =   0;
        gb_info_drv.info_ch[ch_count].p_dma_ch_reg->N1TB_n =   0;
        gb_info_drv.info_ch[ch_count].p_dma_ch_reg->CHITVL_n = 0;
        gb_info_drv.info_ch[ch_count].p_dma_ch_reg->CHEXT_n =  0;
        gb_info_drv.info_ch[ch_count].p_dma_ch_reg->NXLA_n =   0;
    }
    /* init common resgiter for channel 0 - 7 */
    gb_info_drv.info_ch[DMA_CH_0].p_dma_common_reg->DCTRL_0_7 = 0;
    /* init common resgiter for channel 8 - 15 */
    gb_info_drv.info_ch[HIGH_COMMON_REG_OFFSET].p_dma_common_reg->DCTRL_0_7 = 0;

    /* uninit DMA interrupt */
    ch_count = 0;
    uninit_check_flag = false;
    while (false == uninit_check_flag)
    {
        /* disable DMA end interrupt */
        GIC_DisableIRQ(gb_info_drv.info_ch[ch_count].end_irq_num);

        /* unregister DMA end interrupt handler */
        error_code = InterruptHandlerUnregister(gb_info_drv.info_ch[ch_count].end_irq_num);
        /* 0 is no error on InterruptHandlerUnRegister() */
        if (0U != error_code)
        {
            retval = (int_t)error_code;
            uninit_check_flag = true;
        }
        if ((DMA_CH_NUM - 1) == ch_count)
        {
            uninit_check_flag = true;
        }
        ch_count++;
    }
    if (ESUCCESS == retval)
    {
        /* disable DMA error interrupt */
        GIC_DisableIRQ(gb_info_drv.err_irq_num);

        /* unregister DMA interrupt error handler */
        error_code = InterruptHandlerUnregister(gb_info_drv.err_irq_num);
        /* 0 is no error on InterruptHandlerUnRegister() */
        if (0U != error_code)
        {
            retval = (int_t)error_code;
        }
    }

    if (ESUCCESS == retval)
    {
        /* set channel status to DMA_CH_UNINIT */
        for (ch_count = 0; ch_count < DMA_CH_NUM; ch_count++)
        {
            gb_info_drv.info_ch[ch_count].ch_stat = DMA_CH_UNINIT;
        }
        /* set driver status to DMA_DRV_UNINIT*/
        gb_info_drv.drv_stat = DMA_DRV_UNINIT;
    }

    return retval;
}

/******************************************************************************
End of function DMA_UnInitialize
******************************************************************************/

/******************************************************************************
* Function Name: DMA_OpenChannel
* Description : DMA channel open.
*               Set DMA channel status to DMA_CH_OPEN.
* Arguments : channel -
*                  Open channel number.
* Return Value : None.
******************************************************************************/

static void DMA_OpenChannel(const int_t channel)
{
    /* set channel status to DMA_CH_OPEN */
    gb_info_drv.info_ch[channel].ch_stat = DMA_CH_OPEN;

    return;
}

/******************************************************************************
End of function DMA_OpenChannel
******************************************************************************/

/******************************************************************************
* Function Name: DMA_GetFreeChannel
* Description : Find free DMA channel and Get DMA channel.
* Arguments : None
* Return Value :   channel -
*                     Open channel number.
*                  error code -
*                     EMFILE_RBSP : When looking for a free channel, but a free
*                              channel didn't exist.
******************************************************************************/

int_t DMA_GetFreeChannel(void)
{
    int_t           retval = EFAULT_RBSP;
    dma_info_ch_t   *dma_info_ch;
    int_t           ch_alloc;
    bool_t          ch_stat_check_flag;

    /* looking for free channel */
    ch_stat_check_flag = false;
    ch_alloc = 0;
    while (false == ch_stat_check_flag)
    {
        dma_info_ch = DMA_GetDrvChInfo(ch_alloc);

        if (false == ch_stat_check_flag)
        {
            if (DMA_CH_INIT == dma_info_ch->ch_stat)
            {
                DMA_OpenChannel(ch_alloc);
                retval = ch_alloc;
                ch_stat_check_flag = true;
            }
            if (false ==  ch_stat_check_flag)
            {
                ch_alloc++;
                /* not detected free channel */
                if (DMA_CH_NUM == ch_alloc)
                {
                    /* set error return value */
                    retval = EMFILE_RBSP;
                    ch_stat_check_flag = true;
                }
            }
        }
    }

    return retval;
}

/******************************************************************************
End of function DMA_GetFreeChannel
******************************************************************************/

/******************************************************************************
* Function Name: DMA_GetFixedChannel
* Description : Get specified DMA channel number.
* Arguments : channel -
*                  Open channel number.
* Return Value :   channel -
*                     Open channel number.
*                  error code -
*                     EBUSY : It has been allocated already in channel.
*                     ENOTSUP : Channel status is DMA_CH_UNINIT.
*                     EFAULT: Channel status is besides the status definded in
*                             dma_stat_ch_t.
******************************************************************************/

int_t DMA_GetFixedChannel(const int_t channel)
{
    int_t           retval = ESUCCESS;
    dma_info_ch_t   *dma_info_ch;

    /* allocate the specified number */
    dma_info_ch = DMA_GetDrvChInfo(channel);

    if (ESUCCESS == retval)
    {
        if (DMA_CH_INIT == dma_info_ch->ch_stat)
        {
            DMA_OpenChannel(channel);
            /* return alloc channel number */
            retval = channel;
        }
        else
        {
#if(1) /* mbed */
            retval = EERROR;
#else
            /* set error return value */
            switch (dma_info_ch->ch_stat)
            {
                case DMA_CH_UNINIT:
                    retval = ENOTSUP;
                break;
                /* These 2 cases are intentionally combined. */
                case DMA_CH_OPEN:
                case DMA_CH_TRANSFER:
                    retval = EBUSY;
                break;

                default:
                    retval = EFAULT;
                break;

            }
#endif
        }
    }

    return retval;
}

/******************************************************************************
End of function DMA_GetFixedChannel
******************************************************************************/

/******************************************************************************
* Function Name: DMA_CloseChannel
* Description : DMA channel close.
*               Set DMA channel status to DMA_CH_INIT.
* Arguments : channel -
*                  Close channel number.
* Return Value : None.
******************************************************************************/

void DMA_CloseChannel(const int_t channel)
{
    /* clear DMARS register */
    *(gb_info_drv.info_ch[channel].p_dma_dmars_reg) &= gb_info_drv.info_ch[channel].mask_dmars;

    /* set channel status to DMA_CH_INIT */
    gb_info_drv.info_ch[channel].ch_stat = DMA_CH_INIT;

    return;
}

/******************************************************************************
End of function DMA_CloseChannel
******************************************************************************/

/******************************************************************************
* Function Name: DMA_Setparam
* Description : Set DMA transfer parameter to Register.
* Arguments : channel -
*                  Set up channel number.
*             *p_ch_setup -
*                  Set up parameters.
*             *p_ch_cfg -
*                  DMA channel config table parameters.
*             *reqd
*                  set vaule for REQD bit on CHCFG
* Return Value : None.
******************************************************************************/

void DMA_SetParam(const int_t channel, const dma_ch_setup_t *const p_ch_setup,
                  const dma_ch_cfg_t * const p_ch_cfg, const uint32_t reqd)
{
    uint32_t  chcfg_sel;
    uint32_t  value_dmars;

#if(1) /* mbed */
    GIC_SetPriority(gb_info_drv.info_ch[channel].end_irq_num, p_ch_setup->int_level);
#endif

    /* set DMA transfer parameter to DMA channel infomation */
    gb_info_drv.info_ch[channel].resource =  p_ch_setup->resource;
    gb_info_drv.info_ch[channel].direction = p_ch_setup->direction;
    gb_info_drv.info_ch[channel].src_width = p_ch_setup->src_width;
    gb_info_drv.info_ch[channel].src_cnt =   p_ch_setup->src_cnt;
    gb_info_drv.info_ch[channel].dst_width = p_ch_setup->dst_width;
    gb_info_drv.info_ch[channel].dst_cnt =   p_ch_setup->dst_cnt;
    gb_info_drv.info_ch[channel].p_end_aio = p_ch_setup->p_aio;

    /* set DMARS value and protect non change bit */
    value_dmars = *(gb_info_drv.info_ch[channel].p_dma_dmars_reg);
    value_dmars = ((value_dmars & gb_info_drv.info_ch[channel].mask_dmars) |
                   (uint32_t)(p_ch_cfg->dmars << gb_info_drv.info_ch[channel].shift_dmars));
    /* set DMARS register value */
    *(gb_info_drv.info_ch[channel].p_dma_dmars_reg) = value_dmars;

    /* set CHCFG regsiter */
    if (channel < HIGH_COMMON_REG_OFFSET)
    {
        chcfg_sel = (uint32_t)channel;
    }
    else
    {
        chcfg_sel = (uint32_t)(channel - HIGH_COMMON_REG_OFFSET);
    }
    gb_info_drv.info_ch[channel].p_dma_ch_reg->CHCFG_n
        = ((uint32_t)CHCFG_FIXED_VALUE |
           /* ->MISRA 21.1 ,IPA R2.4.1 The value of every parameter won't be minus.
              and the value after a shift will be less than 0x80000000 certainly.
           */
           (((uint32_t)p_ch_setup->dst_cnt << CHCFG_SHIFT_DAD) & CHCFG_MASK_DAD) |
           (((uint32_t)p_ch_setup->src_cnt << CHCFG_SHIFT_SAD) & CHCFG_MASK_SAD) |
           (((uint32_t)p_ch_setup->dst_width << CHCFG_SHIFT_DDS) & CHCFG_MASK_DDS) |
           (((uint32_t)p_ch_setup->src_width << CHCFG_SHIFT_SDS) & CHCFG_MASK_SDS) |
           /* <-MISRA 21.1, IPA R2.4.1 */
           p_ch_cfg->tm |
           p_ch_cfg->lvl |
           reqd |
           chcfg_sel);

    /* set setup flag */
    gb_info_drv.info_ch[channel].setup_flag = true;

    return;
}

/******************************************************************************
End of function DMA_SetParam
******************************************************************************/

/******************************************************************************
* Function Name: DMA_BusParam
* Description : Set bus parameter for DMA.
* Arguments : channel -
*                  Set address channel number.
*             *p_dma_data -
*                  DMA transfer address parameter set.
* Return Value : None.
******************************************************************************/

void DMA_BusParam(const int_t channel, const dma_trans_data_t * const p_dma_data)
{
    uint32_t src_bus_addr = (uint32_t)p_dma_data->src_addr;
    uint32_t dst_bus_addr = (uint32_t)p_dma_data->dst_addr;
    uint32_t chext_value = (CHEXT_SET_DPR_NON_SECURE | CHEXT_SET_SPR_NON_SECURE);

    /* set bus parameter for SRC */
    if (DMA_EXTERNAL_BUS_END >= src_bus_addr)
    {
        chext_value |= CHEXT_SET_SCA_NORMAL;
    }
    else
    {
        chext_value |= CHEXT_SET_SCA_STRONG;
    }

    /* set bus parameter for DST */
    if (DMA_EXTERNAL_BUS_END >= dst_bus_addr)
    {
        chext_value |= CHEXT_SET_DCA_NORMAL;
    }
    else
    {
        chext_value |= CHEXT_SET_DCA_STRONG;
    }

    gb_info_drv.info_ch[channel].p_dma_ch_reg->CHEXT_n = chext_value;

    return;
}

/******************************************************************************
End of function DMA_BusParam
******************************************************************************/

/******************************************************************************
* Function Name: DMA_SetData
* Description : Set DMA transfer address to Register.
* Arguments : channel -
*                  Set address channel number.
*             *p_dma_data -
*                  DMA transfer address parameter set.
*             next_register_set -
*                  Number of next register set.
* Return Value : None.
******************************************************************************/

void DMA_SetData(const int_t channel, const dma_trans_data_t * const p_dma_data,
                          const uint32_t next_register_set)
{
    if (0U == next_register_set)
    {
        /* set DMA transfer address parameters to next register set0 */
        gb_info_drv.info_ch[channel].src_addr0 = p_dma_data->src_addr;
        gb_info_drv.info_ch[channel].dst_addr0 = p_dma_data->dst_addr;
        gb_info_drv.info_ch[channel].count0 =    p_dma_data->count;

        /* ->MISRA 11.3 This cast is needed for setting address to register. */
        /* set DAM transfer addres to register */
        gb_info_drv.info_ch[channel].p_dma_ch_reg->N0SA_n = (uint32_t)p_dma_data->src_addr;
        gb_info_drv.info_ch[channel].p_dma_ch_reg->N0DA_n = (uint32_t)p_dma_data->dst_addr;
        /* <-MISRA 11.3 */
        gb_info_drv.info_ch[channel].p_dma_ch_reg->N0TB_n = p_dma_data->count;
    }
    else
    {
        /* set DMA transfer address parameters to next regiter set1 */
        gb_info_drv.info_ch[channel].src_addr1 = p_dma_data->src_addr;
        gb_info_drv.info_ch[channel].dst_addr1 = p_dma_data->dst_addr;
        gb_info_drv.info_ch[channel].count1 =    p_dma_data->count;

        /* ->MISRA 11.3 This cast is needed for setting address to register. */
        /* set DAM transfer addres to register */
        gb_info_drv.info_ch[channel].p_dma_ch_reg->N1SA_n = (uint32_t)p_dma_data->src_addr;
        gb_info_drv.info_ch[channel].p_dma_ch_reg->N1DA_n = (uint32_t)p_dma_data->dst_addr;
        /* <-MISRA 11.3 */
        gb_info_drv.info_ch[channel].p_dma_ch_reg->N1TB_n = p_dma_data->count;
    }

    return;
}

/******************************************************************************
End of function DMA_SetData
******************************************************************************/

/******************************************************************************
* Function Name: DMA_SetNextData
* Description : Set continuous DMA transfer setting.
* Arguments : channel -
*                  Set continuous DMA transfer channel number.
*             *p_dma_data -
*                  DMA transfer address parameter set.
* Return Value : None.
******************************************************************************/

void DMA_SetNextData(const int_t channel, const dma_trans_data_t * const p_dma_data)
{
    uint32_t  next_register_set;

    /* check number of next register set for next DMA transfer */
    /* The reverse number in current number is set in next regsiter set of next DMA. */
    if (0U == (gb_info_drv.info_ch[channel].p_dma_ch_reg->CHSTAT_n & CHSTAT_MASK_SR))
    {
        next_register_set = 1U;
    }
    else
    {
        next_register_set = 0U;
    }

    /* set DMA transfer address for next DMA */
    DMA_SetData(channel, p_dma_data, next_register_set);

    /* start setting for next DMA */
    gb_info_drv.info_ch[channel].p_dma_ch_reg->CHCFG_n |= (uint32_t)(CHCFG_SET_REN | CHCFG_SET_RSW);

    /* set flag wich indicates that next DMA transfer set already */
    gb_info_drv.info_ch[channel].next_dma_flag = true;

    /* auto restart continous DMA */
    if ((0U == (gb_info_drv.info_ch[channel].p_dma_ch_reg->CHSTAT_n & CHSTAT_MASK_EN)) &&
        (false == gb_info_drv.info_ch[channel].setup_flag))
    {
        /* auto restart DMA */
        DMA_SetData(channel, p_dma_data, 0);
        DMA_Start(channel, true);
    }

    return;
}

/******************************************************************************
End of function DMA_Nextdata
******************************************************************************/

/******************************************************************************
* Function Name: DMA_Start
* Description : Start DMA transfer.
* Arguments : channel -
*                  DMA transfer start channel number.
*           :restart_flag -
*                  Flag of DMA continous transfer auto restart.
* Return Value : None.
******************************************************************************/

void DMA_Start(const int_t channel, const bool_t restart_flag)
{
    if (false != restart_flag)
    {
        /* clear continous DMA setting */
        gb_info_drv.info_ch[channel].p_dma_ch_reg->CHCFG_n &=
        ~(uint32_t)(CHCFG_SET_RSW | CHCFG_SET_RSEL | CHCFG_SET_REN);
        gb_info_drv.info_ch[channel].next_dma_flag = false;
    }

    /* clear setup flag */
    gb_info_drv.info_ch[channel].setup_flag = false;

    /* reset DMA */
    gb_info_drv.info_ch[channel].p_dma_ch_reg->CHCTRL_n = CHCTRL_SET_SWRST;

    /* clear mask of DMA transfer end */
    gb_info_drv.info_ch[channel].p_dma_ch_reg->CHCFG_n &= ~((uint32_t)CHCFG_SET_DEM);

    GIC_EnableIRQ(gb_info_drv.info_ch[channel].end_irq_num);

    /* start DMA transfer */
    gb_info_drv.info_ch[channel].p_dma_ch_reg->CHCTRL_n = CHCTRL_SET_SETEN;

    /* set channel status to DMA_CH_TRANSFER */
    gb_info_drv.info_ch[channel].ch_stat = DMA_CH_TRANSFER;

    return;
}

/******************************************************************************
End of function DMA_Start
******************************************************************************/

/******************************************************************************
* Function Name: DMA_Stop
* Description : Stop DMA transfer.
* Arguments : channel -
*                  DMA transfer start channel number.
*             *p_remain -
*                  Remain data size of DMA transfer.
* Return Value : None.
******************************************************************************/

void DMA_Stop(const int_t channel, uint32_t * const p_remain)
{
    uint32_t  stop_wait_cnt;

    /* disable DMA end interrupt */
    GIC_DisableIRQ(gb_info_drv.info_ch[channel].end_irq_num);

    /* stop DMA transfer */
    gb_info_drv.info_ch[channel].p_dma_ch_reg->CHCTRL_n = CHCTRL_SET_CLREN;

    /* wait DMA stop */
    stop_wait_cnt = 0;
    while ((0 != (gb_info_drv.info_ch[channel].p_dma_ch_reg->CHSTAT_n & CHSTAT_MASK_TACT)) &&
           (DMA_STOP_WAIT_MAX_CNT > stop_wait_cnt))
    {
        stop_wait_cnt++;
    }

    if (DMA_STOP_WAIT_MAX_CNT <= stop_wait_cnt)
    {
        /* NON_NOTICE_ASSERT: wait count is abnormal value (usually, a count is set to 0 or 1) */
    }

    /* get remain data size */
    *p_remain = gb_info_drv.info_ch[channel].p_dma_ch_reg->CRTB_n;

    /* set mask of DMA transfer end */
    gb_info_drv.info_ch[channel].p_dma_ch_reg->CHCFG_n |= (uint32_t)CHCFG_SET_DEM;

    /* clear setting of continuous DMA */
    gb_info_drv.info_ch[channel].p_dma_ch_reg->CHCFG_n &= ~(uint32_t)(CHCFG_SET_RSW | CHCFG_SET_RSEL);

    /* clear TC, END bit */
    gb_info_drv.info_ch[channel].p_dma_ch_reg->CHCTRL_n = (CHCTRL_SET_CLRTC | CHCTRL_SET_CLREND);

    /* clear flag wich indicates that next DMA transfer set already */
    gb_info_drv.info_ch[channel].next_dma_flag = false;

    /* interrupt clear, if interrupt occured already */
    GIC_ClearPendingIRQ(gb_info_drv.info_ch[channel].end_irq_num);

    /* set channel status to DMA_CH_OPEN */
    gb_info_drv.info_ch[channel].ch_stat = DMA_CH_OPEN;

    return;
}

/******************************************************************************
End of function DMA_Stop
******************************************************************************/

/******************************************************************************
* Function Name: DMA_SetErrCode
* Description : Set error code to error code pointer.
*               If error code pointer is NULL, nothing is done.
* Arguments : error_code -
*                  Error code.
*             *p_errno -
*                  Pointer of set error code.
* Return Value : None.
******************************************************************************/

void DMA_SetErrCode(const int_t error_code, int32_t * const p_errno)
{
    if (NULL != p_errno)
    {
        *p_errno = error_code;
    }

    return;
}

/******************************************************************************
End of function DMA_SetErrCode
******************************************************************************/

/******************************************************************************
* Function Name: R_DMA_ErrInterruptHandler
* Description : DMA error interrupt handler.
*               Notify error information to the module function which called DMA driver.
* Arguments : None.
* Return Value : None.
*                Note: store error code (EIO_RBSP) to AIOCB member.
******************************************************************************/

static void R_DMA_ErrInterruptHandler(void)
{
    uint32_t dstat_er_0_7;
    uint32_t dstat_er_8_15;

    if (NULL != gb_info_drv.p_err_aio)
    {
        /* get error channel number */
        dstat_er_0_7  = gb_info_drv.info_ch[DMA_CH_0].p_dma_common_reg->DSTAT_ER_0_7;
        dstat_er_8_15 = gb_info_drv.info_ch[HIGH_COMMON_REG_OFFSET].p_dma_common_reg->DSTAT_ER_0_7;

        /* set error infrmation */
        gb_info_drv.p_err_aio->aio_sigevent.sigev_value.sival_int = (int_t)(dstat_er_0_7 | (dstat_er_8_15 << HIGH_COMMON_REG_OFFSET));

        /* set error code (EIO_RBSP) */
        gb_info_drv.p_err_aio->aio_return = EIO_RBSP;

        /* call back to the module function which called DMA driver */
        ahf_complete(NULL, gb_info_drv.p_err_aio);
    }
    else
    {
        ;
        /* NON_NOTICE_ASSERT:<callback pointer is NULL> */
    }

}

/******************************************************************************
End of function R_DMA_ErrInteruuptHandler
******************************************************************************/

/******************************************************************************
* Function Name: R_DMA_End0InterruptHandler
* Description : DMA end interrupt handler for channel 0.
*               It's processed to DMA complete and notify DMA transfer finished
*               to the module function which called DMA driver.
* Arguments : None.
* Return Value : None.
*                Note: store error code (EIO_RBSP) to AIOCB.
*                  ESUCCESS -
*                     DMA transfer completed.
*                  EIO_RBSP -
*                     DMA transfer don't stopped.
******************************************************************************/

static void R_DMA_End0InterruptHandler(void)
{

    R_DMA_EndHandlerProcess(DMA_CH_0);

}

/******************************************************************************
End of function R_DMA_End0InterruptHandler
******************************************************************************/

/******************************************************************************
* Function Name: R_DMA_End1InterruptHandler
* Description : DMA end interrupt handler for channel 1.
*               It's processed to DMA complete and notify DMA transfer finished
*               to the module function which called DMA driver.
* Arguments : None.
* Return Value : None.
*                Note: store error code (EIO_RBSP) to AIOCB.
*                  ESUCCESS -
*                     DMA transfer completed.
*                  EIO_RBSP -
*                     DMA transfer don't stopped.
******************************************************************************/

static void R_DMA_End1InterruptHandler(void)
{

    R_DMA_EndHandlerProcess(DMA_CH_1);

}

/******************************************************************************
End of function R_DMA_End1InterruptHandler
******************************************************************************/

/******************************************************************************
* Function Name: R_DMA_End2InterruptHandler
* Description : DMA end interrupt handler for channel 2.
*               It's processed to DMA complete and notify DMA transfer finished
*               to the module function which called DMA driver.
* Arguments : None.
* Return Value : None.
*                Note: store error code (EIO_RBSP) to AIOCB.
*                  ESUCCESS -
*                     DMA transfer completed.
*                  EIO_RBSP -
*                     DMA transfer don't stopped.
******************************************************************************/

static void R_DMA_End2InterruptHandler(void)
{

    R_DMA_EndHandlerProcess(DMA_CH_2);

}

/******************************************************************************
End of function R_DMA_End2InterruptHandler
******************************************************************************/

/******************************************************************************
* Function Name: R_DMA_End3InterruptHandler
* Description : DMA end interrupt handler for channel 3.
*               It's processed to DMA complete and notify DMA transfer finished
*               to the module function which called DMA driver.
* Arguments : None.
* Return Value : None.
*                Note: store error code (EIO_RBSP) to AIOCB.
*                  ESUCCESS -
*                     DMA transfer completed.
*                  EIO_RBSP -
*                     DMA transfer don't stopped.
******************************************************************************/

static void R_DMA_End3InterruptHandler(void)
{

    R_DMA_EndHandlerProcess(DMA_CH_3);

}

/******************************************************************************
End of function R_DMA_End3InterruptHandler
******************************************************************************/

/******************************************************************************
* Function Name: R_DMA_End4InterruptHandler
* Description : DMA end interrupt handler for channel 4.
*               It's processed to DMA complete and notify DMA transfer finished
*               to the module function which called DMA driver.
* Arguments : None.
* Return Value : None.
*                Note: store error code (EIO_RBSP) to AIOCB.
*                  ESUCCESS -
*                     DMA transfer completed.
*                  EIO_RBSP -
*                     DMA transfer don't stopped.
******************************************************************************/

static void R_DMA_End4InterruptHandler(void)
{

    R_DMA_EndHandlerProcess(DMA_CH_4);

}

/******************************************************************************
End of function R_DMA_End4InterruptHandler
******************************************************************************/

/******************************************************************************
* Function Name: R_DMA_End5InterruptHandler
* Description : DMA end interrupt handler for channel 5.
*               It's processed to DMA complete and notify DMA transfer finished
*               to the module function which called DMA driver.
* Arguments : None.
* Return Value : None.
*                Note: store error code (EIO_RBSP) to AIOCB.
*                  ESUCCESS -
*                     DMA transfer completed.
*                  EIO_RBSP -
*                     DMA transfer don't stopped.
******************************************************************************/

static void R_DMA_End5InterruptHandler(void)
{

    R_DMA_EndHandlerProcess(DMA_CH_5);

}

/******************************************************************************
End of function R_DMA_End5InterruptHandler
******************************************************************************/

/******************************************************************************
* Function Name: R_DMA_End6InterruptHandler
* Description : DMA end interrupt handler for channel 6.
*               It's processed to DMA complete and notify DMA transfer finished
*               to the module function which called DMA driver.
* Arguments : None.
* Return Value : None.
*                Note: store error code (EIO_RBSP) to AIOCB.
*                  ESUCCESS -
*                     DMA transfer completed.
*                  EIO_RBSP -
*                     DMA transfer don't stopped.
******************************************************************************/

static void R_DMA_End6InterruptHandler(void)
{

    R_DMA_EndHandlerProcess(DMA_CH_6);

}

/******************************************************************************
End of function R_DMA_End6InterruptHandler
******************************************************************************/

/******************************************************************************
* Function Name: R_DMA_End7InterruptHandler
* Description : DMA end interrupt handler for channel 7.
*               It's processed to DMA complete and notify DMA transfer finished
*               to the module function which called DMA driver.
* Arguments : None.
* Return Value : None.
*                Note: store error code (EIO_RBSP) to AIOCB.
*                  ESUCCESS -
*                     DMA transfer completed.
*                  EIO_RBSP -
*                     DMA transfer don't stopped.
******************************************************************************/

static void R_DMA_End7InterruptHandler(void)
{

    R_DMA_EndHandlerProcess(DMA_CH_7);

}

/******************************************************************************
End of function R_DMA_End7InterruptHandler
******************************************************************************/

/******************************************************************************
* Function Name: R_DMA_End8InterruptHandler
* Description : DMA end interrupt handler for channel 8.
*               It's processed to DMA complete and notify DMA transfer finished
*               to the module function which called DMA driver.
* Arguments : None.
* Return Value : None.
*                Note: store error code (EIO_RBSP) to AIOCB.
*                  ESUCCESS -
*                     DMA transfer completed.
*                  EIO_RBSP -
*                     DMA transfer don't stopped.
******************************************************************************/

static void R_DMA_End8InterruptHandler(void)
{

    R_DMA_EndHandlerProcess(DMA_CH_8);

}

/******************************************************************************
End of function R_DMA_End8InterruptHandler
******************************************************************************/

/******************************************************************************
* Function Name: R_DMA_End9InterruptHandler
* Description : DMA end interrupt handler for channel 9.
*               It's processed to DMA complete and notify DMA transfer finished
*               to the module function which called DMA driver.
* Arguments : None.
* Return Value : None.
*                Note: store error code (EIO_RBSP) to AIOCB.
*                  ESUCCESS -
*                     DMA transfer completed.
*                  EIO_RBSP -
*                     DMA transfer don't stopped.
******************************************************************************/

static void R_DMA_End9InterruptHandler(void)
{

    R_DMA_EndHandlerProcess(DMA_CH_9);

}

/******************************************************************************
End of function R_DMA_End9InterruptHandler
******************************************************************************/

/******************************************************************************
* Function Name: R_DMA_End10InterruptHandler
* Description : DMA end interrupt handler for channel 10.
*               It's processed to DMA complete and notify DMA transfer finished
*               to the module function which called DMA driver.
* Arguments : None.
* Return Value : None.
*                Note: store error code (EIO_RBSP) to AIOCB.
*                  ESUCCESS -
*                     DMA transfer completed.
*                  EIO_RBSP -
*                     DMA transfer don't stopped.
******************************************************************************/

static void R_DMA_End10InterruptHandler(void)
{

    R_DMA_EndHandlerProcess(DMA_CH_10);

}

/******************************************************************************
End of function R_DMA_End10InterruptHandler
******************************************************************************/

/******************************************************************************
* Function Name: R_DMA_End11InterruptHandler
* Description : DMA end interrupt handler for channel 11.
*               It's processed to DMA complete and notify DMA transfer finished
*               to the module function which called DMA driver.
* Arguments : None.
* Return Value : None.
*                Note: store error code (EIO_RBSP) to AIOCB.
*                  ESUCCESS -
*                     DMA transfer completed.
*                  EIO_RBSP -
*                     DMA transfer don't stopped.
******************************************************************************/

static void R_DMA_End11InterruptHandler(void)
{

    R_DMA_EndHandlerProcess(DMA_CH_11);

}

/******************************************************************************
End of function R_DMA_End11InterruptHandler
******************************************************************************/

/******************************************************************************
* Function Name: R_DMA_End12InterruptHandler
* Description : DMA end interrupt handler for channel 12.
*               It's processed to DMA complete and notify DMA transfer finished
*               to the module function which called DMA driver.
* Arguments : None.
* Return Value : None.
*                Note: store error code (EIO_RBSP) to AIOCB.
*                  ESUCCESS -
*                     DMA transfer completed.
*                  EIO_RBSP -
*                     DMA transfer don't stopped.
******************************************************************************/

static void R_DMA_End12InterruptHandler(void)
{

    R_DMA_EndHandlerProcess(DMA_CH_12);

}

/******************************************************************************
End of function R_DMA_End12InterruptHandler
******************************************************************************/

/******************************************************************************
* Function Name: R_DMA_End13InterruptHandler
* Description : DMA end interrupt handler for channel 13.
*               It's processed to DMA complete and notify DMA transfer finished
*               to the module function which called DMA driver.
* Arguments : None.
* Return Value : None.
*                Note: store error code (EIO_RBSP) to AIOCB.
*                  ESUCCESS -
*                     DMA transfer completed.
*                  EIO_RBSP -
*                     DMA transfer don't stopped.
******************************************************************************/

static void R_DMA_End13InterruptHandler(void)
{

    R_DMA_EndHandlerProcess(DMA_CH_13);

}

/******************************************************************************
End of function R_DMA_End13InterruptHandler
******************************************************************************/

/******************************************************************************
* Function Name: R_DMA_End14InterruptHandler
* Description : DMA end interrupt handler for channel 14.
*               It's processed to DMA complete and notify DMA transfer finished
*               to the module function which called DMA driver.
* Arguments : None.
* Return Value : None.
*                Note: store error code (EIO_RBSP) to AIOCB.
*                  ESUCCESS -
*                     DMA transfer completed.
*                  EIO_RBSP -
*                     DMA transfer don't stopped.
******************************************************************************/

static void R_DMA_End14InterruptHandler(void)
{

    R_DMA_EndHandlerProcess(DMA_CH_14);

}

/******************************************************************************
End of function R_DMA_End14InterruptHandler
******************************************************************************/

/******************************************************************************
* Function Name: R_DMA_End15InterruptHandler
* Description : DMA end interrupt handler for channel 15.
*               It's processed to DMA complete and notify DMA transfer finished
*               to the module function which called DMA driver.
* Arguments : None.
* Return Value : None.
*                Note: store error code (EIO_RBSP) to AIOCB.
*                  ESUCCESS -
*                     DMA transfer completed.
*                  EIO_RBSP -
*                     DMA transfer don't stopped.
******************************************************************************/

static void R_DMA_End15InterruptHandler(void)
{

    R_DMA_EndHandlerProcess(DMA_CH_15);

}

/******************************************************************************
End of function R_DMA_End15InterruptHandler
******************************************************************************/

/******************************************************************************
* Function Name: R_DMA_EndHandlerProcess
* Description : DMA end interrupt handler process carry out.
*               It's processed to DMA complete and notify DMA transfer finished
*               to the module function which called DMA driver.
* Arguments : channel -
*                  Open channel number.
* Return Value : None.
*                Note: store error code (EIO_RBSP) to AIOCB.
*                  ESUCCESS -
*                     DMA transfer completed.
*                  EIO_RBSP -
*                     DMA transfer don't stopped.
******************************************************************************/

__INLINE static void R_DMA_EndHandlerProcess(const int_t channel)
{
    bool_t   store_next_dma_flag;

    if (NULL != gb_info_drv.info_ch[channel].p_end_aio)
    {
        /* disable all irq */
        core_util_critical_section_enter();

        /* store next_dma_flag */
        store_next_dma_flag = gb_info_drv.info_ch[channel].next_dma_flag;

        /* clear flag wich indicates that next DMA transfer set already */
        gb_info_drv.info_ch[channel].next_dma_flag = false;

        if (false == store_next_dma_flag)
        {
            /* DMA transfer complete */
            /* mask DMA end interrupt */
            GIC_DisableIRQ(gb_info_drv.info_ch[channel].end_irq_num);

            /* set channel status to DMA_CH_OPEN */
            gb_info_drv.info_ch[channel].ch_stat = DMA_CH_OPEN;

            /* set mask of DMA transfer end */
            gb_info_drv.info_ch[channel].p_dma_ch_reg->CHCFG_n |= (uint32_t)CHCFG_SET_DEM;

            /* clear setting of continuous DMA */
            gb_info_drv.info_ch[channel].p_dma_ch_reg->CHCFG_n &= ~(uint32_t)(CHCFG_SET_RSW | CHCFG_SET_RSEL);

            /* check EN bit is clear */
            if (0U == (gb_info_drv.info_ch[channel].p_dma_ch_reg->CHSTAT_n & CHSTAT_MASK_EN))
            {
                /* set error code (ESUCCESS) */
                gb_info_drv.info_ch[channel].p_end_aio->aio_return = ESUCCESS;
            }
            else
            {
                /* set error code (EIO_RBSP) */
                gb_info_drv.info_ch[channel].p_end_aio->aio_return = EIO_RBSP;
            }
        }
        else
        {
            /* set next DMA already */
            /* set error code (ESUCCESS) */
            gb_info_drv.info_ch[channel].p_end_aio->aio_return = ESUCCESS;
        }

        /* clear TC, END bit */
        gb_info_drv.info_ch[channel].p_dma_ch_reg->CHCTRL_n = (CHCTRL_SET_CLRTC | CHCTRL_SET_CLREND);

        core_util_critical_section_exit();

        /* call back to the module function which called DMA driver */
        ahf_complete(NULL, gb_info_drv.info_ch[channel].p_end_aio);
    }
    else
    {
        ;
        /* NON_NOTICE_ASSERT:<callback pointer is NULL> */
    }

    return;
}

/******************************************************************************
End of function R_DMA_EndHandlerProcess
******************************************************************************/

