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
* File Name   : ssif_int.c
* $Rev: $
* $Date::                           $
* Description : SSIF driver interrupt functions
******************************************************************************/

/*******************************************************************************
Includes <System Includes>, "Project Includes"
*******************************************************************************/
#include "ssif.h"
#include "ssif_int.h"
#include "cmsis.h"

/*******************************************************************************
Typedef definitions
*******************************************************************************/


/*******************************************************************************
Macro definitions
*******************************************************************************/
#define SSIF_INT_PER_CH     (4u)


/*******************************************************************************
Private global variables and functions
*******************************************************************************/

/******************************************************************************
 Function prototypes
 *****************************************************************************/

static void SSIF_ERI0_Handler(void);
static void SSIF_ERI1_Handler(void);
static void SSIF_ERI2_Handler(void);
static void SSIF_ERI3_Handler(void);
#if defined(TARGET_RZA1H) /* mbed */
static void SSIF_ERI4_Handler(void);
static void SSIF_ERI5_Handler(void);
#endif /* mbed */

static void SSIF_ERI_Handler(const uint32_t ssif_ch);

/******************************************************************************
* Function Name: SSIF_InterruptInit
* @brief         Initialize interrupt setting for SSIF channel.
*
*                Description:<br>
*                Enable interrupt and register interrupt handler.
* @param[in]     ssif_ch    :SSIF channel
* @param[in]     int_level  :GIC priority level of interrupt
* @retval        none
******************************************************************************/
void SSIF_InterruptInit(const uint32_t ssif_ch, const uint8_t int_level)
{
    uint32_t ret;

    switch (ssif_ch)
    {
        case SSIF_CHNUM_0:
            ret = InterruptHandlerRegister(SSII0_IRQn, &SSIF_ERI0_Handler);
            if (0u != ret)
            {
                /* NON_NOTICE_ASSERT: Illegal IRQ number */
            }
            GIC_SetPriority(SSII0_IRQn, int_level);
            GIC_SetPriority(SSIRXI0_IRQn, int_level);
            GIC_SetPriority(SSITXI0_IRQn, int_level);
            GIC_EnableIRQ(SSII0_IRQn);
            GIC_EnableIRQ(SSIRXI0_IRQn);
            GIC_EnableIRQ(SSITXI0_IRQn);
            break;
        case SSIF_CHNUM_1:
            ret = InterruptHandlerRegister(SSII1_IRQn, &SSIF_ERI1_Handler);
            if (0u != ret)
            {
                /* NON_NOTICE_ASSERT: Illegal IRQ number */
            }
            GIC_SetPriority(SSII1_IRQn, int_level);
            GIC_SetPriority(SSIRXI1_IRQn, int_level);
            GIC_SetPriority(SSITXI1_IRQn, int_level);
            GIC_EnableIRQ(SSII1_IRQn);
            GIC_EnableIRQ(SSIRXI1_IRQn);
            GIC_EnableIRQ(SSITXI1_IRQn);
            break;
        case SSIF_CHNUM_2:
            ret = InterruptHandlerRegister(SSII2_IRQn, &SSIF_ERI2_Handler);
            if (0u != ret)
            {
                /* NON_NOTICE_ASSERT: Illegal IRQ number */
            }
            GIC_SetPriority(SSII2_IRQn, int_level);
            GIC_SetPriority(SSIRTI2_IRQn, int_level);
            GIC_EnableIRQ(SSII2_IRQn);
            GIC_EnableIRQ(SSIRTI2_IRQn);
            break;
        case SSIF_CHNUM_3:
            ret = InterruptHandlerRegister(SSII3_IRQn, &SSIF_ERI3_Handler);
            if (0u != ret)
            {
                /* NON_NOTICE_ASSERT: Illegal IRQ number */
            }
            GIC_SetPriority(SSII3_IRQn, int_level);
            GIC_SetPriority(SSIRXI3_IRQn, int_level);
            GIC_SetPriority(SSITXI3_IRQn, int_level);
            GIC_EnableIRQ(SSII3_IRQn);
            GIC_EnableIRQ(SSIRXI3_IRQn);
            GIC_EnableIRQ(SSITXI3_IRQn);
            break;
#if defined(TARGET_RZA1H) /* mbed */
        case SSIF_CHNUM_4:
            ret = InterruptHandlerRegister(SSII4_IRQn, &SSIF_ERI4_Handler);
            if (0u != ret)
            {
                /* NON_NOTICE_ASSERT: Illegal IRQ number */
            }
            GIC_SetPriority(SSII4_IRQn, int_level);
            GIC_SetPriority(SSIRTI4_IRQn, int_level);
            GIC_EnableIRQ(SSII4_IRQn);
            GIC_EnableIRQ(SSIRTI4_IRQn);
            break;
        case SSIF_CHNUM_5:
            ret = InterruptHandlerRegister(SSII5_IRQn, &SSIF_ERI5_Handler);
            if (0u != ret)
            {
                /* NON_NOTICE_ASSERT: Illegal IRQ number */
            }
            GIC_SetPriority(SSII5_IRQn, int_level);
            GIC_SetPriority(SSIRXI5_IRQn, int_level);
            GIC_SetPriority(SSITXI5_IRQn, int_level);
            GIC_EnableIRQ(SSII5_IRQn);
            GIC_EnableIRQ(SSIRXI5_IRQn);
            GIC_EnableIRQ(SSITXI5_IRQn);
            break;
#endif /* mbed */
        /* ->IPA R3.5.2 : There is nothing to do when unusual conditons. */
        default:
            /* NON_NOTICE_ASSERT: Illegal channel number */
            break;
        /* <-IPA R3.5.2 */
    }

    return;
}

/******************************************************************************
* Function Name: SSIF_InterruptShutdown
* @brief         Uninitialize interrupt setting for SSIF channel.
*
*                Description:<br>
*                Disable interrupt and unregister interrupt handler
* @param[in]     ssif_ch    :SSIF channel
* @retval        none
******************************************************************************/
void SSIF_InterruptShutdown(const uint32_t ssif_ch)
{
    uint32_t ret;

    switch (ssif_ch)
    {
        case SSIF_CHNUM_0:
            GIC_DisableIRQ(SSII0_IRQn);
            GIC_DisableIRQ(SSIRXI0_IRQn);
            GIC_DisableIRQ(SSITXI0_IRQn);
            ret = InterruptHandlerUnregister(SSII0_IRQn);
            if (0u != ret)
            {
                /* NON_NOTICE_ASSERT: Illegal IRQ number */
            }
            break;
        case SSIF_CHNUM_1:
            GIC_DisableIRQ(SSII1_IRQn);
            GIC_DisableIRQ(SSIRXI1_IRQn);
            GIC_DisableIRQ(SSITXI1_IRQn);
            ret = InterruptHandlerUnregister(SSII1_IRQn);
            if (0u != ret)
            {
                /* NON_NOTICE_ASSERT: Illegal IRQ number */
            }
            break;
        case SSIF_CHNUM_2:
            GIC_DisableIRQ(SSII2_IRQn);
            GIC_DisableIRQ(SSIRTI2_IRQn);
            ret = InterruptHandlerUnregister(SSII2_IRQn);
            if (0u != ret)
            {
                /* NON_NOTICE_ASSERT: Illegal IRQ number */
            }
            break;
        case SSIF_CHNUM_3:
            GIC_DisableIRQ(SSII3_IRQn);
            GIC_DisableIRQ(SSIRXI3_IRQn);
            GIC_DisableIRQ(SSITXI3_IRQn);
            ret = InterruptHandlerUnregister(SSII3_IRQn);
            if (0u != ret)
            {
                /* NON_NOTICE_ASSERT: Illegal IRQ number */
            }
            break;
#if defined(TARGET_RZA1H) /* mbed */
        case SSIF_CHNUM_4:
            GIC_DisableIRQ(SSII4_IRQn);
            GIC_DisableIRQ(SSIRTI4_IRQn);
            ret = InterruptHandlerUnregister(SSII4_IRQn);
            if (0u != ret)
            {
                /* NON_NOTICE_ASSERT: Illegal IRQ number */
            }
            break;
        case SSIF_CHNUM_5:
            GIC_DisableIRQ(SSII5_IRQn);
            GIC_DisableIRQ(SSIRXI5_IRQn);
            GIC_DisableIRQ(SSITXI5_IRQn);
            ret = InterruptHandlerUnregister(SSII5_IRQn);
            if (0u != ret)
            {
                /* NON_NOTICE_ASSERT: Illegal IRQ number */
            }
            break;
#endif /* mbed */
        /* ->IPA R3.5.2 : There is nothing to do when unusual conditons. */
        default:
            /* NON_NOTICE_ASSERT: Illegal channel number */
            break;
        /* <-IPA R3.5.2 */
    }

    return;
}

/******************************************************************************
* Function Name: SSIF_EnableErrorInterrupt
* @brief         Enable the SSIF channel error interrupt
*
*                Description:<br>
*
* @param[in]     ssif_ch    :SSIF channel
* @retval        none
******************************************************************************/
void SSIF_EnableErrorInterrupt(const uint32_t ssif_ch)
{
    /* clear error status */
    g_ssireg[ssif_ch]->SSISR = 0u;

    /* enable error interrupt */
    g_ssireg[ssif_ch]->SSICR |= SSIF_CR_INT_ERR_MASK;

    return;
}

/******************************************************************************
* Function Name: SSIF_DisableErrorInterrupt
* @brief         Disable the SSIF channel error interrupt
*
*                Description:<br>
*
* @param[in]     ssif_ch    :SSIF channel
* @retval        none
******************************************************************************/
void SSIF_DisableErrorInterrupt(const uint32_t ssif_ch)
{
    /* disable error interrupt */
    g_ssireg[ssif_ch]->SSICR &= ~(SSIF_CR_INT_ERR_MASK);

    return;
}

/******************************************************************************
* Function Name: SSIF_ERI<n>_Handler
* @brief         SSIF error interrupt handler per channel
*
*                Description:<br>
*
* @param         none
* @retval        none
******************************************************************************/

static void SSIF_ERI0_Handler(void)
{
    SSIF_ERI_Handler(SSIF_CHNUM_0);
    return;
}

static void SSIF_ERI1_Handler(void)
{
    SSIF_ERI_Handler(SSIF_CHNUM_1);
    return;
}

static void SSIF_ERI2_Handler(void)
{
    SSIF_ERI_Handler(SSIF_CHNUM_2);
    return;
}

static void SSIF_ERI3_Handler(void)
{
    SSIF_ERI_Handler(SSIF_CHNUM_3);
    return;
}

#if defined(TARGET_RZA1H) /* mbed */
static void SSIF_ERI4_Handler(void)
{
    SSIF_ERI_Handler(SSIF_CHNUM_4);
    return;
}

static void SSIF_ERI5_Handler(void)
{
    SSIF_ERI_Handler(SSIF_CHNUM_5);
    return;
}

#endif /* mbed */
/******************************************************************************
* Function Name: SSIF_ERI_Handler
* @brief         SSIF error interrupt handler common function
*
*                Description:<br>
*
* @param[in]     ssif_ch    :SSIF channel
* @retval        none
******************************************************************************/

static void SSIF_ERI_Handler(const uint32_t ssif_ch)
{
#if(1) /* mbed */
    ssif_info_drv_t* const p_info_drv = SSIF_GetDrvInstanc();
    ssif_info_ch_t* const p_info_ch = &p_info_drv->info_ch[ssif_ch];
#else
    ssif_info_ch_t* const p_info_ch = &g_ssif_info_drv.info_ch[ssif_ch];
#endif /* end mbed */

    if (0u != (g_ssireg[ssif_ch]->SSISR & SSIF_SR_INT_ERR_MASK))
    {
        /* Restart or Callback */
        SSIF_ErrorRecovery(p_info_ch);
    }

    return;
}
