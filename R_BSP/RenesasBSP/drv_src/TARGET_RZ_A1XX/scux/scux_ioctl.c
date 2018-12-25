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
* Copyright (C) 2013-2016 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

/**************************************************************************//**
* @file         scux_ioctl.c
* $Rev: $
* $Date::                           $
* @brief        SCUX Driver ioctl functions
******************************************************************************/

/*******************************************************************************
Includes <System Includes>, "Project Includes"
*******************************************************************************/

#include "scux.h"
#include "bsp_util.h"
#include  "mbed_critical.h"

/******************************************************************************
Exported global variables (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global driver management information
******************************************************************************/

/******************************************************************************
 Function prototypes
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_IoctlTransStart
* @brief         SCUX transfer start.
*
*                Description:<br>
*
* @param[in]     channel:SCUX channel number.
* @param[in]     p_scux_addr_param:address parameter.
* @retval        ESUCCESS : Operation successful.
*                EACCES_RBSP : DVU setup isn't carried out when using DVU.
*                EACCES_RBSP : MIX setup isn't carried out when using MIX.
*                EACCES_RBSP : SSIF setup isn't carried out when using SSIF.
*                EACCES_RBSP : SSIF channel is already used.
*                EACCES_RBSP : When use MIX, it is a setup which does not agree in a route setup.
*                EBUSY_RBSP : It has already transmitted.
*                EMFILE_RBSP : Allocate DMA ch for read is failed.
*                EPERM_RBSP : Transfer parameter is unexpected.
*                EFAULT_RBSP : Internal error is occured.
******************************************************************************/

#if(1) /* mbed */
#if defined (__CC_ARM)
#pragma O0
#endif
#endif /* end mbed */
int_t SCUX_IoctlTransStart(const int_t channel)
{
    int32_t          sem_wait_ercd;
    int_t            retval = ESUCCESS;
    uint32_t         ssif_sem_ch = 0;
    int_t            ssif_ch;
    osStatus         sem_ercd;
    scux_ssif_info_t * p_ssif_ch;
    scux_ssif_info_t  * p_ssif_ch_num[SCUX_SSIF_CH_NUM];
    scux_info_drv_t * const p_info_drv = SCUX_GetDrvInstance();
    scux_info_ch_t * const p_info_ch = SCUX_GetDrvChInfo(channel);

    /* for processing for the measure against MISRA R1.1 and IPA R3.2.2 */
    for (ssif_ch = 0; ssif_ch < SCUX_SSIF_CH_NUM; ssif_ch++)
    {
        p_ssif_ch_num[ssif_ch] = SCUX_GetSsifChInfo(ssif_ch);
    }
    if ((NULL == p_info_drv) || (NULL == p_info_ch))
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        /* check using MIX route */
        if (SCUX_ROUTE_MIX == (p_info_ch->route_set & SCUX_GET_ROUTE_MASK))
        {
            /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
            sem_wait_ercd = osSemaphoreAcquire(p_info_drv->shared_info.sem_shared_access, osWaitForever);
            /* <-MISRA 10.6 */
            /* semaphore error check */
            if ((-1) == sem_wait_ercd)
            {
                /* set semaphore error */
                retval = EFAULT_RBSP;
            }
        }

        if ((SCUX_ROUTE_SSIF == (p_info_ch->route_set & SCUX_GET_ROUTE_MASK)) ||
            (SCUX_ROUTE_MIX == (p_info_ch->route_set & SCUX_GET_ROUTE_MASK)))
        {
            /* check using SSIF route */
            /* mode is async mode only when SSIF direvct route */
            p_info_ch->src_cfg.mode_sync = false;
            switch (p_info_ch->route_set)
            {
                case SCUX_ROUTE_SRC0_MEM :
                    /* fall through */
                case SCUX_ROUTE_SRC1_MEM :
                    /* fall through */
                case SCUX_ROUTE_SRC2_MEM :
                    /* fall through */
                case SCUX_ROUTE_SRC3_MEM :
                    /* NOTREACHED on At the time of a normal performance */
                break;

                case SCUX_ROUTE_SRC0_SSIF0 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_0];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_0);
                    }
                break;

                case SCUX_ROUTE_SRC0_SSIF012 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_0];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_0);
                    }

                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_1];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_1);
                    }

                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_2];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_2);
                    }
                break;

                case SCUX_ROUTE_SRC0_SSIF3 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_3];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_3);
                    }
                break;

#if defined(TARGET_RZA1H) /* mbed */
                case SCUX_ROUTE_SRC0_SSIF345 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_3];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_3);
                    }

                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_4];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_4);
                    }

                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_5];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_5);
                    }
                break;

#endif /* mbed */
                case SCUX_ROUTE_SRC1_SSIF0 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_0];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_0);
                    }
                break;

                case SCUX_ROUTE_SRC1_SSIF012 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_0];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_0);
                    }

                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_1];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_1);
                    }

                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_2];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_2);
                    }
                break;

                case SCUX_ROUTE_SRC1_SSIF3 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_3];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_3);
                    }
                break;

#if defined(TARGET_RZA1H) /* mbed */
                case SCUX_ROUTE_SRC1_SSIF345 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_3];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_3);
                    }

                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_4];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_4);
                    }

                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_5];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_5);
                    }
                break;

#endif /* mbed */
                case SCUX_ROUTE_SRC2_SSIF1 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_1];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_1);
                    }
                break;

#if defined(TARGET_RZA1H) /* mbed */
                case SCUX_ROUTE_SRC2_SSIF4 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_4];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_4);
                    }
                break;

#endif /* mbed */
                case SCUX_ROUTE_SRC3_SSIF2 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_2];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_2);
                    }
                break;

#if defined(TARGET_RZA1H) /* mbed */
                case SCUX_ROUTE_SRC3_SSIF5 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_5];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_5);
                    }
                break;

#endif /* mbed */
                case SCUX_ROUTE_SRC0_MIX_SSIF0 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_0];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_0);
                    }
                break;

                case SCUX_ROUTE_SRC0_MIX_SSIF012 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_0];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_0);
                    }

                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_1];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_1);
                    }

                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_2];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_2);
                    }
                break;

                case SCUX_ROUTE_SRC0_MIX_SSIF3 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_3];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_3);
                    }
                break;

#if defined(TARGET_RZA1H) /* mbed */
                case SCUX_ROUTE_SRC0_MIX_SSIF345 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_3];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_3);
                    }

                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_4];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_4);
                    }

                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_5];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_5);
                    }
                break;

#endif /* mbed */
                case SCUX_ROUTE_SRC1_MIX_SSIF0 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_0];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_0);
                    }
                break;

                case SCUX_ROUTE_SRC1_MIX_SSIF012 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_0];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_0);
                    }

                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_1];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_1);
                    }

                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_2];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_2);
                    }
                break;

                case SCUX_ROUTE_SRC1_MIX_SSIF3 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_3];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_3);
                    }
                break;

#if defined(TARGET_RZA1H) /* mbed */
                case SCUX_ROUTE_SRC1_MIX_SSIF345 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_3];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_3);
                    }

                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_4];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_4);
                    }

                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_5];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_5);
                    }
                break;

#endif /* mbed */
                case SCUX_ROUTE_SRC2_MIX_SSIF0 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_0];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_0);
                    }
                break;

                case SCUX_ROUTE_SRC2_MIX_SSIF012 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_0];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_0);
                    }

                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_1];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_1);
                    }

                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_2];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_2);
                    }
                break;

                case SCUX_ROUTE_SRC2_MIX_SSIF3 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_3];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_3);
                    }
                break;

#if defined(TARGET_RZA1H) /* mbed */
                case SCUX_ROUTE_SRC2_MIX_SSIF345 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_3];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_3);
                    }

                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_4];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_4);
                    }

                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_5];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_5);
                    }
                break;

#endif /* mbed */
                case SCUX_ROUTE_SRC3_MIX_SSIF0 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_0];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_0);
                    }
                break;

                case SCUX_ROUTE_SRC3_MIX_SSIF012 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_0];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_0);
                    }

                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_1];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_1);
                    }

                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_2];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_2);
                    }
                break;

                case SCUX_ROUTE_SRC3_MIX_SSIF3 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_3];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_3);
                    }
                break;

#if defined(TARGET_RZA1H) /* mbed */
                case SCUX_ROUTE_SRC3_MIX_SSIF345 :
                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_3];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_3);
                    }

                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_4];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_4);
                    }

                    p_ssif_ch = p_ssif_ch_num[SCUX_SSIF_CH_5];
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        ssif_sem_ch |= (1U << SCUX_SSIF_CH_5);
                    }
                break;

#endif /* mbed */
                default :
                    /* ->IPA R3.5.2 Nothing is being processed intentionally. */
                    /* <-IPA R3.5.2 */
                    /* NOTREACHED on At the time of a normal performance */
                break;
            }
        }

        if (ESUCCESS == retval)
        {
            retval = SCUX_CheckParam(p_info_ch);

            if (ESUCCESS == retval)
            {
                core_util_critical_section_enter();
                SCUX_InitHw(p_info_ch);
                if (SCUX_ROUTE_MEM_TO_MEM != (p_info_ch->route_set & SCUX_GET_ROUTE_MASK))
                {
                    SCUX_SetupSsif(p_info_ch);
                }

                SCUX_SetupSrc(p_info_ch);
                if (SCUX_ROUTE_MEM_TO_MEM != (p_info_ch->route_set & SCUX_GET_ROUTE_MASK))
                {
                    SCUX_SetupDvu(p_info_ch);
                }

                retval = SCUX_SetupDma(p_info_ch);

                if (ESUCCESS == retval)
                {
                    if (false != p_info_ch->src_cfg.mode_sync)
                    {
                        SCUX_SyncStartHw(p_info_ch);
                    }
                    else
                    {
                        SCUX_AsyncStartHw(p_info_ch);
                    }
                    p_info_ch->ch_stat = SCUX_CH_TRANS_IDLE;
                    p_info_ch->p_flush_callback = NULL;

                    p_info_ch->dma_tx_current_size = 0;
                    p_info_ch->dma_rx_current_size = 0;
                    p_info_ch->tx_fifo_total_size = 0;
                    p_info_ch->rx_fifo_total_size = 0;

                    p_info_ch->p_tx_aio = NULL;
                    p_info_ch->p_rx_aio = NULL;
                    p_info_ch->p_tx_next_aio = NULL;
                    p_info_ch->p_rx_next_aio = NULL;
                    p_info_ch->dvu_mute_stat = 0;
                    p_info_ch->first_ramp_flag = false;
                    p_info_ch->cancel_operate_flag = false;
                    p_info_ch->restart_ramp_flag = false;
                    p_info_ch->err_stat_backup = ESUCCESS;
                }

                core_util_critical_section_exit();
            }
        }

        for (ssif_ch = 0; ssif_ch < SCUX_SSIF_CH_NUM; ssif_ch++)
        {
            if (0U != (ssif_sem_ch & (1U << ssif_ch)))
            {
                p_ssif_ch = p_ssif_ch_num[ssif_ch];
                sem_ercd = osSemaphoreRelease(p_ssif_ch->sem_ch_scux_ssif_access);
                /* semaphore error check */
                if (osOK != sem_ercd)
                {
                    /* set semaphore error */
                    retval = EFAULT_RBSP;
                    p_info_ch->ch_stat = SCUX_CH_STOP;
                }
            }
        }

        /* check using MIX route */
        if (SCUX_ROUTE_MIX == (p_info_ch->route_set & SCUX_GET_ROUTE_MASK))
        {
            sem_ercd = osSemaphoreRelease(p_info_drv->shared_info.sem_shared_access);
            /* semaphore error check */
            if (osOK != sem_ercd)
            {
                /* set semaphore error */
                retval = EFAULT_RBSP;
                p_info_ch->ch_stat = SCUX_CH_STOP;
            }
        }
    }

    return retval;
}
#if(1) /* mbed */
#if defined (__CC_ARM)
#pragma O3
#endif
#endif /* end mbed */

/******************************************************************************
End of function SCUX_IoctlTransStart
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_IoctlFlushStop
* @brief         Set flush stop paramter.
*
*                Description:<br>
*
* @param[in]     channel:SCUX channel number.
* @param[in]     (*callback)(void):callback function pointer.
* @retval        ESUCCESS : Operation successful.
*                EFAULT_RBSP : Internal error is occured.
******************************************************************************/

int_t SCUX_IoctlFlushStop(const int_t channel, void (* const callback)(int_t))
{
    int_t retval = ESUCCESS;
    scux_info_ch_t * const p_info_ch = SCUX_GetDrvChInfo(channel);

    if (NULL == p_info_ch)
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        if (SCUX_ROUTE_MEM_TO_MEM == (p_info_ch->route_set & SCUX_GET_ROUTE_MASK))
        {
            /* mem to mem route */
            if ((SCUX_CH_TRANS_IDLE == p_info_ch->ch_stat) || (SCUX_CH_TRANS_RD == p_info_ch->ch_stat))
            {
                /* Flush process is performed only when dummy buffer transmission has not been started yet */
                if (false == p_info_ch->tx_dummy_run_flag)
                {
                    /* start flush write */
                    retval = SCUX_FlushWriteStart(p_info_ch);
                }

                if (ESUCCESS == retval)
                {
                    /* memory to memory route */
                    if (SCUX_CH_TRANS_RD == p_info_ch->ch_stat)
                    {
                        p_info_ch->ch_stat = SCUX_CH_STOP_WAIT;
                    }
                    else
                    {
                        p_info_ch->ch_stat = SCUX_CH_STOP_WAIT_IDLE;
                    }
                    p_info_ch->p_flush_callback = callback;
                }
            }
            else
            {
                /* on going write process */
                /* status is SCUX_CH_TRANS_WR, SCUX_CH_TRANS_RDWR, SCUX_CH_STOP_WAIT, SCUX_CH_STOP_WAIT_IDLE */
                if (SCUX_CH_TRANS_RDWR == p_info_ch->ch_stat)
                {
                    p_info_ch->ch_stat = SCUX_CH_STOP_WAIT;
                }
                if (SCUX_CH_TRANS_WR == p_info_ch->ch_stat)
                {
                    p_info_ch->ch_stat = SCUX_CH_STOP_WAIT_IDLE;
                }

                p_info_ch->p_flush_callback = callback;
            }
        }
        else
        {
            /* SSIF direct route */
            if (SCUX_CH_TRANS_IDLE == p_info_ch->ch_stat)
            {
                /* Flush process is performed only when dummy buffer transmission has not been started yet */
                if (false == p_info_ch->tx_dummy_run_flag)
                {
                    /* start flush write */
                    retval = SCUX_FlushWriteStart(p_info_ch);
                }

                if (ESUCCESS == retval)
                {
                    /* memory to memory route */
                    p_info_ch->ch_stat = SCUX_CH_STOP_WAIT;
                    p_info_ch->p_flush_callback = callback;
                }
            }
            else
            {
                /* on going write process */
                p_info_ch->ch_stat = SCUX_CH_STOP_WAIT;
                p_info_ch->p_flush_callback = callback;

            }
        }
    }

    core_util_critical_section_exit();

    return retval;
}

/******************************************************************************
End of function SCUX_IoctlFlushStop
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_IoctlClearStop
* @brief         Set flush stop paramter.
*
*                Description:<br>
*
* @param[in]     channel:SCUX channel number.
* @retval        ESUCCESS : Operation successful.
*                EFAULT_RBSP : Internal error is occured.
******************************************************************************/

int_t SCUX_IoctlClearStop(const int_t channel)
{
    int_t retval = ESUCCESS;
#if(1) /* mbed */
    int32_t dma_ercd;
#else
    int_t dma_ercd;
#endif
    int_t dma_retval;
    scux_stat_ch_t old_stat;
    uint32_t rx_remain_size = 0;
    uint32_t tx_remain_size = 0;

    scux_info_ch_t * const p_info_ch = SCUX_GetDrvChInfo(channel);
    if (NULL == p_info_ch)
    {
        retval = EFAULT_RBSP;
        core_util_critical_section_exit();
    }
    else
    {
        if (SCUX_ROUTE_MEM_TO_MEM == (p_info_ch->route_set & SCUX_GET_ROUTE_MASK))
        {
            /* memory to memory route */
            dma_retval = R_DMA_Cancel(p_info_ch->dma_tx_ch, &tx_remain_size, &dma_ercd);
            /* DMA stop check, (when dma_ercd is EBADF_RBSP, DMA stopped already) */
            if ((ESUCCESS != dma_retval) && (EBADF_RBSP != dma_ercd))
            {
                retval = EFAULT_RBSP;
            }
            else
            {
                p_info_ch->tx_fifo_total_size += p_info_ch->dma_tx_current_size;
                p_info_ch->dma_tx_current_size = 0;
            }

            dma_retval = R_DMA_Cancel(p_info_ch->dma_rx_ch, &rx_remain_size, &dma_ercd);
            /* DMA stop check, (when dma_ercd is EBADF_RBSP, DMA stopped already) */
            if ((ESUCCESS != dma_retval) && (EBADF_RBSP != dma_ercd))
            {
                retval = EFAULT_RBSP;
            }
            else
            {
                p_info_ch->rx_fifo_total_size += p_info_ch->dma_rx_current_size;
                p_info_ch->dma_rx_current_size = 0;

                dma_retval = R_DMA_Free(p_info_ch->dma_rx_ch, NULL);
                if (ESUCCESS != dma_retval)
                {
                    retval = EFAULT_RBSP;
                }
            }

            if (ESUCCESS == retval)
            {
                /* return write request */
                if (NULL !=  p_info_ch->p_tx_aio)
                {
                    p_info_ch->p_tx_aio->aio_return = (ssize_t)(p_info_ch->p_tx_aio->aio_nbytes - tx_remain_size);
                    ahf_complete(&p_info_ch->tx_que, p_info_ch->p_tx_aio);
                }

                /* return read request */
                if (NULL != p_info_ch->p_rx_aio)
                {
                    p_info_ch->p_rx_aio->aio_return = (ssize_t)(p_info_ch->p_rx_aio->aio_nbytes - rx_remain_size);
                    ahf_complete(&p_info_ch->rx_que, p_info_ch->p_rx_aio);
                }

                ahf_cancelall(&p_info_ch->tx_que);
                ahf_cancelall(&p_info_ch->rx_que);

                p_info_ch->p_tx_aio = NULL;
                p_info_ch->p_rx_aio = NULL;
            }
        }
        else
        {
            /* SSIF direct route */
            dma_retval = R_DMA_Cancel(p_info_ch->dma_tx_ch, &tx_remain_size, &dma_ercd);
            /* DMA stop check, (when dma_ercd is EBADF_RBSP, DMA stopped already) */
            if ((ESUCCESS != dma_retval) && (EBADF_RBSP != dma_ercd))
            {
                retval = EFAULT_RBSP;
            }
            else
            {
                p_info_ch->tx_fifo_total_size += p_info_ch->dma_tx_current_size;
                p_info_ch->dma_tx_current_size = 0;
            }

            if (ESUCCESS == retval)
            {
                /* return the request after 2nd */
                if (false == p_info_ch->first_tx_flag)
                {
                    /* return current aio request */
                    if (NULL !=  p_info_ch->p_tx_aio)
                    {
                        p_info_ch->p_tx_aio->aio_return = (ssize_t)(p_info_ch->p_tx_aio->aio_nbytes - tx_remain_size);
                        ahf_complete(&p_info_ch->tx_que, p_info_ch->p_tx_aio);
                    }
                    /* return next aio request */
                    if (NULL != p_info_ch->p_tx_next_aio)
                    {
                        p_info_ch->p_tx_next_aio->aio_return = ECANCELED_RBSP;
                        ahf_complete(&p_info_ch->tx_que, p_info_ch->p_tx_next_aio);
                    }
                }
                else
                {
                    /* return the 1st request  */
                    if (NULL == p_info_ch->p_tx_aio)
                    {
                        /* return the first half part of a 1st request */
                        if (NULL != p_info_ch->p_tx_next_aio)
                        {
                            p_info_ch->p_tx_next_aio->aio_return
                            = (ssize_t)((p_info_ch->p_tx_next_aio->aio_nbytes / SCUX_HALF_SIZE_VALUE) - tx_remain_size);
                            ahf_complete(&p_info_ch->tx_que, p_info_ch->p_tx_next_aio);
                        }
                    }
                    else
                    {
                        /* return the second half part of a 1st request */
                        p_info_ch->p_tx_aio->aio_return
                        = (ssize_t)(p_info_ch->p_tx_aio->aio_nbytes - tx_remain_size);
                        ahf_complete(&p_info_ch->tx_que, p_info_ch->p_tx_aio);

                        /* in case of the next request is acquired */
                        if (NULL != p_info_ch->p_tx_next_aio)
                        {
                            p_info_ch->p_tx_next_aio->aio_return = ECANCELED_RBSP;
                            ahf_complete(&p_info_ch->tx_que, p_info_ch->p_tx_next_aio);
                        }
                    }
                }
                ahf_cancelall(&p_info_ch->tx_que);

                p_info_ch->p_tx_aio = NULL;
                p_info_ch->p_tx_next_aio = NULL;
            }
        }

        if (ESUCCESS == retval)
        {
            SCUX_AdjustAccessFifo(p_info_ch, tx_remain_size, rx_remain_size);
            if (false != p_info_ch->src_cfg.mode_sync)
            {
                SCUX_SyncStopHw(p_info_ch);
            }
            else
            {
                SCUX_AsyncStopHw(p_info_ch);
            }

            old_stat = p_info_ch->ch_stat;
            p_info_ch->ch_stat = SCUX_CH_STOP;

            core_util_critical_section_exit();

            if ((SCUX_CH_STOP_WAIT == old_stat) || (SCUX_CH_STOP_WAIT_IDLE == old_stat))
            {
                p_info_ch->p_flush_callback(ECANCELED_RBSP);
                p_info_ch->p_flush_callback = NULL;
            }
        }
        else
        {
            core_util_critical_section_exit();
        }
    }

    return retval;
}

/******************************************************************************
End of function SCUX_IoctlClearStop
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_IoctlSetRoute
* @brief         Set SCUX data transfer route.
*
*                Description:<br>
*
* @param[in]     channel:SCUX channel number.
* @param[in]     route:route parameter.
* @retval        None.
******************************************************************************/

void SCUX_IoctlSetRoute(const int_t channel, const scux_route_t route)
{
    scux_info_ch_t * const p_info_ch = SCUX_GetDrvChInfo(channel);

    if (NULL == p_info_ch)
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        p_info_ch->route_set = route;
    }

    return;
}

/******************************************************************************
End of function SCUX_IoctlSetRoute
******************************************************************************/


/**************************************************************************//**
* Function Name: SCUX_IoctlSetPinClk
* @brief         Set clock source for AUDIO_CLK pin.
*
*                Description:<br>
*
* @param[in]     channel:SCUX channel number.
* @param[in]     *p_pin_clk_param:clock source parameter.
* @retval        ESUCCESS:Operation successful.
*                EPERM_RBSP:Parameter is unexpected value.
*                EFAULT_RBSP:Internal error is occured.
******************************************************************************/

int_t SCUX_IoctlSetPinClk(const int_t channel, const scux_ssif_pin_clk_t * const p_pin_clk_param)
{
    int_t retval = ESUCCESS;
    int32_t  sem_wait_ercd;
    osStatus sem_ercd;
    scux_ssif_info_t * p_ssif_ch;
    scux_info_drv_t * const p_info_drv = SCUX_GetDrvInstance();
    scux_info_ch_t * const p_info_ch = SCUX_GetDrvChInfo(channel);

    if ((NULL == p_info_drv) || (NULL == p_info_ch) || (NULL == p_pin_clk_param))
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        /* check SSIF channel number */
        if (SCUX_SSIF_CH_NUM > p_pin_clk_param->ssif_ch_num)
        {
            p_ssif_ch = SCUX_GetSsifChInfo((int_t)p_pin_clk_param->ssif_ch_num);
            if (NULL == p_ssif_ch)
            {
                retval = EFAULT_RBSP;
            }
            else
            {
                /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                /* <-MISRA 10.6 */
                /* semaphore error check */
                if ((-1) == sem_wait_ercd)
                {
                    /* set semaphore error */
                    retval = EFAULT_RBSP;
                }
                else
                {
                    /* check clock source parameter */
                    if ((SCUX_CLK_AUDIO_CLK == p_pin_clk_param->pin_clk) || (SCUX_CLK_MLB_CLK == p_pin_clk_param->pin_clk))
                    {
                        p_ssif_ch->pin_clk = p_pin_clk_param->pin_clk;
                    }
                    else
                    {
                        retval = EPERM_RBSP;
                    }
                }
                sem_ercd = osSemaphoreRelease(p_ssif_ch->sem_ch_scux_ssif_access);
                /* semaphore error check */
                if (osOK != sem_ercd)
                {
                    /* set semaphore error */
                    retval = EFAULT_RBSP;
                }

                if (ESUCCESS == retval)
                {
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_info_drv->shared_info.sem_shared_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        if (SCUX_CLK_MLB_CLK == p_pin_clk_param->pin_clk)
                        {
                            /* set MLB_CLK */
                            *(p_info_ch->p_scux_reg->ssipmd_cim) |= (SSIPMD_CIM_SSI0CKS_SET << p_pin_clk_param->ssif_ch_num);
                        }
                        else
                        {
                            /* set AUDIO_CLK */
                            *(p_info_ch->p_scux_reg->ssipmd_cim) &= ~(SSIPMD_CIM_SSI0CKS_SET << p_pin_clk_param->ssif_ch_num);
                        }
                    }

                    sem_ercd = osSemaphoreRelease(p_info_drv->shared_info.sem_shared_access);
                    /* semaphore error check */
                    if (osOK != sem_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                }
            }
        }
        else
        {
            retval = EPERM_RBSP;
        }
    }

    return retval;
}

/******************************************************************************
End of function SCUX_IoctlSetPinClk
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_IoctlSetPinMode
* @brief         Set synchronous setting for each SSIP pin.
*
*                Description:<br>
*
* @param[in]     channel:SCUX channel number.
* @param[in]     *p_pin_mode_param:synchronous setting parameter.
* @retval        ESUCCESS:Operation successful.
*                EPERM_RBSP:Parameter is unexpected value.
*                EFAUT:Internal error is occured.
******************************************************************************/

int_t SCUX_IoctlSetPinMode(const int_t channel, const scux_ssif_pin_mode_t * const p_pin_mode_param)
{
    int_t retval = ESUCCESS;
    uint32_t ssipmd_reg;
    uint32_t ssipmd_mask = 0;
    uint32_t ssipmd_shift = 0;
    osStatus sem_ercd;
    int32_t  sem_wait_ercd;
    scux_ssif_info_t * p_ssif_ch;
    scux_info_drv_t * const p_info_drv = SCUX_GetDrvInstance();
    scux_info_ch_t * const p_info_ch = SCUX_GetDrvChInfo(channel);

    if ((NULL == p_info_drv) || (NULL == p_info_ch) || (NULL == p_pin_mode_param))
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        /* check SSIF channel number (disable setting for SSIF0) */
        if (SCUX_SSIF_CH_NUM > p_pin_mode_param->ssif_ch_num)
        {
            p_ssif_ch = SCUX_GetSsifChInfo((int_t)p_pin_mode_param->ssif_ch_num);
            if (NULL == p_ssif_ch)
            {
                retval = EFAULT_RBSP;
            }
            else
            {
                /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                /* <-MISRA 10.6 */
                /* semaphore error check */
                if ((-1) == sem_wait_ercd)
                {
                    /* set semaphore error */
                    retval = EFAULT_RBSP;
                }
                else
                {
                    /* check clock source parameter */
                    if ((SCUX_PIN_MODE_MIN < p_pin_mode_param->pin_mode) || (SCUX_PIN_MODE_MAX > p_pin_mode_param->pin_mode))
                    {
                        p_ssif_ch->pin_mode = p_pin_mode_param->pin_mode;
                    }
                    else
                    {
                        retval = EPERM_RBSP;
                    }
                }
                sem_ercd = osSemaphoreRelease(p_ssif_ch->sem_ch_scux_ssif_access);
                /* semaphore error check */
                if (osOK != sem_ercd)
                {
                    /* set semaphore error */
                    retval = EFAULT_RBSP;
                }

                if (ESUCCESS == retval)
                {
                    /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                    sem_wait_ercd = osSemaphoreAcquire(p_info_drv->shared_info.sem_shared_access, osWaitForever);
                    /* <-MISRA 10.6 */
                    /* semaphore error check */
                    if ((-1) == sem_wait_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                    else
                    {
                        /* pin_mode setup to SSIF0 does not exist */
                        if (SCUX_SSIF_CH_0 != p_pin_mode_param->ssif_ch_num)
                        {
                            switch (p_pin_mode_param->ssif_ch_num)
                            {
                                case SCUX_SSIF_CH_1 :
                                    ssipmd_mask = SSIPMD_CIM_SSI1PMD_MASK;
                                    ssipmd_shift = SSIPMD_CIM_SSI1PMD_SHIFT;
                                break;

                                case SCUX_SSIF_CH_2 :
                                    ssipmd_mask = SSIPMD_CIM_SSI2PMD_MASK;
                                    ssipmd_shift = SSIPMD_CIM_SSI2PMD_SHIFT;
                                break;

                                case SCUX_SSIF_CH_3 :
                                    ssipmd_mask = SSIPMD_CIM_SSI3PMD_MASK;
                                    ssipmd_shift = SSIPMD_CIM_SSI3PMD_SHIFT;
                                break;

#if defined(TARGET_RZA1H) /* mbed */
                                case SCUX_SSIF_CH_4 :
                                    ssipmd_mask = SSIPMD_CIM_SSI4PMD_MASK;
                                    ssipmd_shift = SSIPMD_CIM_SSI4PMD_SHIFT;
                                break;

                                case SCUX_SSIF_CH_5 :
                                    ssipmd_mask = SSIPMD_CIM_SSI5PMD_MASK;
                                    ssipmd_shift = SSIPMD_CIM_SSI5PMD_SHIFT;
                                break;

#endif /* mbed */
                                default :
                                    /* SCUX channel is 6 oe more */
                                    retval = EPERM_RBSP;
                                break;
                            }

                            if (ESUCCESS == retval)
                            {
                                ssipmd_reg = *(p_info_ch->p_scux_reg->ssipmd_cim);
                                ssipmd_reg &= ~ssipmd_mask;

                                /* ->IPA R2.4.1 Even if pinmode and ssipmd_shift are max value, omission dose not occur. */
                                *(p_info_ch->p_scux_reg->ssipmd_cim) = (ssipmd_reg | ((uint32_t)p_pin_mode_param->pin_mode << ssipmd_shift));
                                /* <-IPA R2.4.1 */
                            }
                        }
                    }

                    sem_ercd = osSemaphoreRelease(p_info_drv->shared_info.sem_shared_access);
                    /* semaphore error check */
                    if (osOK != sem_ercd)
                    {
                        /* set semaphore error */
                        retval = EFAULT_RBSP;
                    }
                }
            }
        }
        else
        {
            retval = EPERM_RBSP;
        }
    }

    return retval;
}

/******************************************************************************
End of function SCUX_IoctlSetPinMode
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_IoctlSetSrcCfg
* @brief         Set SRC paramter.
*
*                Description:<br>
*
* @param[in]     channel:SCUX channel number.
* @param[in]     *p_src_param:SRC parameter.
* @retval        None.
******************************************************************************/

void SCUX_IoctlSetSrcCfg(const int_t channel, const scux_src_cfg_t * const p_src_param)
{
    scux_audio_channel_t audio_ch;
    scux_info_ch_t * const p_info_ch = SCUX_GetDrvChInfo(channel);

    if ((NULL == p_info_ch) || (NULL == p_src_param))
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        p_info_ch->src_cfg.src_enable = p_src_param->src_enable;
        p_info_ch->src_cfg.use_ch = p_src_param->use_ch;
        p_info_ch->src_cfg.word_len = p_src_param->word_len;
        p_info_ch->src_cfg.mode_sync = p_src_param->mode_sync;
        p_info_ch->src_cfg.input_rate_sync = p_src_param->input_rate_sync;
        p_info_ch->src_cfg.input_clk_async = p_src_param->input_clk_async;
        p_info_ch->src_cfg.input_div_async = p_src_param->input_div_async;
        p_info_ch->src_cfg.output_rate_sync = p_src_param->output_rate_sync;
        p_info_ch->src_cfg.output_clk_async = p_src_param->output_clk_async;
        p_info_ch->src_cfg.output_div_async = p_src_param->output_div_async;
        p_info_ch->src_cfg.delay_mode = p_src_param->delay_mode;
        p_info_ch->src_cfg.wait_sample = p_src_param->wait_sample;
        p_info_ch->src_cfg.min_rate_percentage = p_src_param->min_rate_percentage;
        p_info_ch->src_cfg.output_ws = p_src_param->output_ws;
        p_info_ch->src_cfg.input_ws = p_src_param->input_ws;
        p_info_ch->src_cfg.freq_tioc3a = p_src_param->freq_tioc3a;
        p_info_ch->src_cfg.freq_tioc4a = p_src_param->freq_tioc4a;
        for (audio_ch = SCUX_AUDIO_CH_0; audio_ch < SCUX_AUDIO_CH_MAX; audio_ch++)
        {
            p_info_ch->src_cfg.select_in_data_ch[audio_ch] = p_src_param->select_in_data_ch[audio_ch];
        }
    }

    return;
}

/******************************************************************************
End of function SCUX_IoctlSetSrcCfg
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_IoctlSetDvuCfg
* @brief         Set DVU parameter.
*
*                Description:<br>
*
* @param[in]     channel:SCUX channel number.
* @param[in]     *p_dvu_param:DVU parameter.
* @retval        None.
******************************************************************************/

void SCUX_IoctlSetDvuCfg(const int_t channel, const scux_dvu_cfg_t * const p_dvu_param)
{
    scux_audio_channel_t audio_ch;
    scux_info_ch_t * const p_info_ch = SCUX_GetDrvChInfo(channel);

    if ((NULL == p_info_ch) || (NULL == p_dvu_param))
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        p_info_ch->dvu_cfg.dvu_enable = p_dvu_param->dvu_enable;

        /* digital volume setting */
        p_info_ch->dvu_cfg.dvu_digi_vol.digi_vol_enable = p_dvu_param->dvu_digi_vol.digi_vol_enable;
        for (audio_ch = SCUX_AUDIO_CH_0; audio_ch < SCUX_AUDIO_CH_MAX; audio_ch++)
        {
            p_info_ch->dvu_cfg.dvu_digi_vol.digi_vol[audio_ch] = p_dvu_param->dvu_digi_vol.digi_vol[audio_ch];
        }

        /* ramp volume setting */
        for (audio_ch = SCUX_AUDIO_CH_0; audio_ch < SCUX_AUDIO_CH_MAX; audio_ch++)
        {
            p_info_ch->dvu_cfg.dvu_ramp_vol.ramp_vol_enable[audio_ch] = p_dvu_param->dvu_ramp_vol.ramp_vol_enable[audio_ch];
        }
        p_info_ch->dvu_cfg.dvu_ramp_vol.up_period = p_dvu_param->dvu_ramp_vol.up_period;
        p_info_ch->dvu_cfg.dvu_ramp_vol.down_period = p_dvu_param->dvu_ramp_vol.down_period;
        p_info_ch->dvu_cfg.dvu_ramp_vol.ramp_vol = p_dvu_param->dvu_ramp_vol.ramp_vol;
        p_info_ch->dvu_cfg.dvu_ramp_vol.ramp_wait_time = p_dvu_param->dvu_ramp_vol.ramp_wait_time;

        /* zerocross mute setting */
        for (audio_ch = SCUX_AUDIO_CH_0; audio_ch < SCUX_AUDIO_CH_MAX; audio_ch++)
        {
            p_info_ch->dvu_cfg.dvu_zc_mute.zc_mute_enable[audio_ch] = p_dvu_param->dvu_zc_mute.zc_mute_enable[audio_ch];
            p_info_ch->dvu_cfg.dvu_zc_mute.pcallback[audio_ch] = p_dvu_param->dvu_zc_mute.pcallback[audio_ch];
        }

        p_info_ch->dvu_setup = true;
    }

    return;
}

/******************************************************************************
End of function SCUX_IoctlSetDvuCfg
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_IoctlSetDvuDigiVol
* @brief         Set Digital volume parameter.
*
*                Description:<br>
*
* @param[in]     channel:SCUX channel number.
* @param[in]     *p_dvu_param:Digital volume parameter.
* @retval        ESUCCESS:Operation successful.
*                EPERM_RBSP:Parameter is unexpected value.
*                EFAULT_RBSP : Internal error is occured.
******************************************************************************/

int_t SCUX_IoctlSetDvuDigiVol(const int_t channel, const scux_dvu_digi_vol_t * const p_digi_vol_param)
{
    int_t retval = ESUCCESS;
    scux_audio_channel_t audio_ch;
    scux_info_ch_t * const p_info_ch = SCUX_GetDrvChInfo(channel);

    if ((NULL == p_info_ch) || (NULL == p_digi_vol_param))
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        /* digital volume setting */
        p_info_ch->dvu_cfg.dvu_digi_vol.digi_vol_enable = p_digi_vol_param->digi_vol_enable;
        for (audio_ch = SCUX_AUDIO_CH_0; audio_ch < SCUX_AUDIO_CH_MAX; audio_ch++)
        {
            p_info_ch->dvu_cfg.dvu_digi_vol.digi_vol[audio_ch] = p_digi_vol_param->digi_vol[audio_ch];
        }

        /* check digital volume */
#if(1) /* mbed */
        for (audio_ch = SCUX_AUDIO_CH_0; ((ESUCCESS == retval) && ((int)audio_ch < (int)p_info_ch->src_cfg.use_ch)); audio_ch++)
#else
        for (audio_ch = SCUX_AUDIO_CH_0; ((ESUCCESS == retval) && (audio_ch < p_info_ch->src_cfg.use_ch)); audio_ch++)
#endif
        {
            if (SCUX_MAX_DIGITAL_VOLUME < p_info_ch->dvu_cfg.dvu_digi_vol.digi_vol[audio_ch])
            {
                retval = EPERM_RBSP;
            }
        }

        if (ESUCCESS == retval)
        {
            core_util_critical_section_enter();

            if (SCUX_CH_STOP != p_info_ch->ch_stat)
            {
                p_info_ch->p_scux_reg->p_dvu_reg->DVUER_DVU0_0 &= ~DVUER_DVU0_DVUEN_SET;

                /* set parameter for register */
                SCUX_SetDigiVolRegister(p_info_ch);

                p_info_ch->p_scux_reg->p_dvu_reg->DVUER_DVU0_0 |= DVUER_DVU0_DVUEN_SET;
            }

            core_util_critical_section_exit();
        }
    }

    return retval;
}

/******************************************************************************
End of function SCUX_IoctlSetDvuDigiVol
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_IoctlSetDvuRampVol
* @brief         Set Digital volume parameter.
*
*                Description:<br>
*
* @param[in]     channel:SCUX channel number.
* @param[in]     *p_dvu_param:Digital volume parameter.
* @retval        ESUCCESS:Operation successful.
*                EPERM_RBSP:Parameter is unexpected value.
*                EFAULT_RBSP : Internal error is occured.
******************************************************************************/

int_t SCUX_IoctlSetDvuRampVol(const int_t channel, const scux_dvu_ramp_vol_t * const p_ramp_vol_param)
{
    int_t retval = ESUCCESS;
    scux_audio_channel_t audio_ch;
    scux_info_ch_t * const p_info_ch = SCUX_GetDrvChInfo(channel);

    if ((NULL == p_info_ch) || (NULL == p_ramp_vol_param))
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        /* ramp volume setting */
        for (audio_ch = SCUX_AUDIO_CH_0; audio_ch < SCUX_AUDIO_CH_MAX; audio_ch++)
        {
            p_info_ch->dvu_cfg.dvu_ramp_vol.ramp_vol_enable[audio_ch] = p_ramp_vol_param->ramp_vol_enable[audio_ch];
        }
        p_info_ch->dvu_cfg.dvu_ramp_vol.up_period = p_ramp_vol_param->up_period;
        p_info_ch->dvu_cfg.dvu_ramp_vol.down_period = p_ramp_vol_param->down_period;
        p_info_ch->dvu_cfg.dvu_ramp_vol.ramp_vol = p_ramp_vol_param->ramp_vol;
        p_info_ch->dvu_cfg.dvu_ramp_vol.ramp_wait_time = p_ramp_vol_param->ramp_wait_time;

        /* check ramp volume */
        /* check ramp down period */
        if ((p_info_ch->dvu_cfg.dvu_ramp_vol.down_period <= SCUX_DVU_TIME_MIN) ||
            (p_info_ch->dvu_cfg.dvu_ramp_vol.down_period >= SCUX_DVU_TIME_MAX))
        {
               retval = EPERM_RBSP;
        }

        if (ESUCCESS == retval)
        {
            /* check ramp up period */
            if ((p_info_ch->dvu_cfg.dvu_ramp_vol.up_period <= SCUX_DVU_TIME_MIN) ||
                (p_info_ch->dvu_cfg.dvu_ramp_vol.up_period >= SCUX_DVU_TIME_MAX))
            {
                retval = EPERM_RBSP;
            }
        }

        if (ESUCCESS == retval)
        {
            /* check ramp volume */
            if (SCUX_MAX_RAMP_VOLUME < p_info_ch->dvu_cfg.dvu_ramp_vol.ramp_vol)
            {
                retval = EPERM_RBSP;
            }
        }

        if (ESUCCESS != retval)
        {
            /* check wait time */
            if (SCUX_MAX_WAIT_TIME < p_info_ch->dvu_cfg.dvu_ramp_vol.ramp_wait_time)
            {
                retval = EPERM_RBSP;
            }
        }

        if (ESUCCESS == retval)
        {
            core_util_critical_section_enter();

            if ((SCUX_CH_STOP != p_info_ch->ch_stat) && (false != p_info_ch->first_ramp_flag))
            {
                p_info_ch->p_scux_reg->p_dvu_reg->DVUER_DVU0_0 &= ~DVUER_DVU0_DVUEN_SET;

                /* set parameter for register */
                SCUX_SetRampVolRegister(p_info_ch);

                p_info_ch->p_scux_reg->p_dvu_reg->DVUER_DVU0_0 |= DVUER_DVU0_DVUEN_SET;
            }

            /* an updating setting is used when a seting is updated before the first request is going */
            p_info_ch->restart_ramp_flag = false;

            core_util_critical_section_exit();
        }
    }

    return retval;
}

/******************************************************************************
End of function SCUX_IoctlSetDvuRampVol
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_IoctlSetDvuZerocrossMute
* @brief         Set zerocross mute parameter.
*
*                Description:<br>
*
* @param[in]     channel:SCUX channel number.
* @param[in]     *p_zc_mute_param:zerocross mute parameter.
* @retval        ESUCCESS:Operation successful.
*                EPERM_RBSP:Parameter is unexpected value.
*                EPERM_RBSP:Rewrite callback pointer while waiting zerocross.
******************************************************************************/

int_t SCUX_IoctlSetDvuZerocrossMute(const int_t channel, const scux_zc_mute_t * const p_zc_mute_param)
{
    int_t retval = ESUCCESS;
    scux_audio_channel_t audio_ch;
    scux_info_ch_t * const p_info_ch = SCUX_GetDrvChInfo(channel);

    if ((NULL == p_info_ch) || (NULL == p_zc_mute_param))
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        /* set zerocross mute paramter */
        for (audio_ch = SCUX_AUDIO_CH_0; audio_ch < SCUX_AUDIO_CH_MAX; audio_ch++)
        {
            p_info_ch->dvu_cfg.dvu_zc_mute.zc_mute_enable[audio_ch]= p_zc_mute_param->zc_mute_enable[audio_ch];
            p_info_ch->dvu_cfg.dvu_zc_mute.pcallback[audio_ch] = p_zc_mute_param->pcallback[audio_ch];
        }

        core_util_critical_section_enter();

        if (SCUX_CH_STOP != p_info_ch->ch_stat)
        {
            p_info_ch->p_scux_reg->p_dvu_reg->DVUER_DVU0_0 &= ~DVUER_DVU0_DVUEN_SET;

            /* set parameter for register */
            SCUX_SetZerocrossMuteRegister(p_info_ch);

            p_info_ch->p_scux_reg->p_dvu_reg->DVUER_DVU0_0 |= DVUER_DVU0_DVUEN_SET;
        }

        core_util_critical_section_exit();
    }

    return retval;
}

/******************************************************************************
End of function SCUX_IoctlSetDvuZerocrossMute
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_IoctlSetStopMute
* @brief         Stop mute.
*
*                Description:<br>
*
* @param[in]     channel:SCUX channel number.
* @param[in]     audio_channel:Audio channel number.
* @retval        ESUCCESS:Operation successful.
*                EPERM_RBSP:Parameter is unexpected value.
*                EFAULT_RBSP : Internal error is occured.
******************************************************************************/

int_t SCUX_IoctlSetStopMute(const int_t channel, const uint32_t audio_channel)
{
    int_t retval = ESUCCESS;
    scux_info_ch_t * const p_info_ch = SCUX_GetDrvChInfo(channel);

    if (NULL == p_info_ch)
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        if (audio_channel >= (uint32_t)p_info_ch->src_cfg.use_ch)
        {
            retval = EPERM_RBSP;
        }

        if (ESUCCESS == retval)
        {
            core_util_critical_section_enter();

            p_info_ch->dvu_mute_stat &= ~(1U << audio_channel);
            p_info_ch->dvu_cfg.dvu_zc_mute.zc_mute_enable[audio_channel] = false;
            p_info_ch->p_scux_reg->p_dvu_reg->DVUER_DVU0_0 &= ~DVUER_DVU0_DVUEN_SET;
            p_info_ch->p_scux_reg->p_dvu_reg->ZCMCR_DVU0_0 &= ~(ZCMCR_DVU0_ZCEN_SET << audio_channel);
            p_info_ch->p_scux_reg->p_dvu_reg->DVUER_DVU0_0 |= DVUER_DVU0_DVUEN_SET;

            core_util_critical_section_exit();
        }
    }

    return retval;
}

/******************************************************************************
End of function SCUX_IoctlSetStopMute
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_IoctlSetMixCfg
* @brief         Set MIX parameter.
*
*                Description:<br>
*
* @param[in]     *p_mix_param:MIX parameter.
* @retval        ESUCCESS : Operation successful.
*                EPERM_RBSP : Transfer parameter is unexpected.
*                EFAULT_RBSP : Internal error is occured.
******************************************************************************/

int_t SCUX_IoctlSetMixCfg(const scux_mix_cfg_t * const p_mix_param)
{
    int_t retval = ESUCCESS;
    int_t scux_ch;
    int_t audio_ch;
    scux_info_drv_t * const p_info_drv = SCUX_GetDrvInstance();

    if ((NULL == p_info_drv) || (NULL == p_mix_param))
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        core_util_critical_section_enter();

        p_info_drv->shared_info.mixmode_ramp = p_mix_param->mixmode_ramp;
        p_info_drv->shared_info.up_period = p_mix_param->up_period;
        p_info_drv->shared_info.down_period = p_mix_param->down_period;
        for (scux_ch = SCUX_CH_0; scux_ch < SCUX_CH_NUM; scux_ch++)
        {
            p_info_drv->shared_info.mix_vol[scux_ch] = p_mix_param->mix_vol[scux_ch];
        }
        for (audio_ch = SCUX_AUDIO_CH_0; audio_ch < SCUX_AUDIO_CH_MAX; audio_ch++)
        {
            p_info_drv->shared_info.select_out_data_ch[audio_ch] = p_mix_param->select_out_data_ch[audio_ch];
        }

        for (scux_ch = SCUX_CH_0; ((ESUCCESS == retval) && (scux_ch < SCUX_CH_NUM)); scux_ch++)
        {
            /* check going channel number */
            if (0U != (p_info_drv->shared_info.mix_run_ch & (1U << scux_ch)))
            {
                /* check ramp volume */
                if (SCUX_MAX_RAMP_VOLUME < p_info_drv->shared_info.mix_vol[scux_ch])
                {
                    retval = EPERM_RBSP;
                }
            }
        }

        if (ESUCCESS == retval)
        {
            for (scux_ch = SCUX_CH_0; scux_ch < SCUX_CH_NUM; scux_ch++)
            {
                /* check going channel number */
                if (0U != (p_info_drv->shared_info.mix_run_ch & (1U << scux_ch)))
                {
                    /* set parameter for register */
                    SCUX_SetMixVolRegister(scux_ch);
                }
            }

            p_info_drv->shared_info.mix_setup = true;
        }
        else
        {
            p_info_drv->shared_info.mix_setup = false;
        }

        core_util_critical_section_exit();
    }

    return retval;
}

/******************************************************************************
End of function SCUX_IoctlSetMixCfg
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_IoctlSetMixVol
* @brief         Set MIX volume.
*
*                Description:<br>
*
* @param[in]     channel:SCUX channel number.
* @param[in]     mix_vol:MIX volume.
* @retval        ESUCCESS : Operation successful.
*                EPERM_RBSP : Transfer parameter is unexpected.
*                EFAULT_RBSP: Internel error si occured.
******************************************************************************/

int_t SCUX_IoctlSetMixVol(const int_t channel, const uint32_t mix_vol)
{
    int_t retval = ESUCCESS;
    int32_t  sem_wait_ercd;
    osStatus sem_ercd;
    scux_info_drv_t * const p_info_drv = SCUX_GetDrvInstance();

    if (NULL == p_info_drv)
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
        if (0 == R_ExceptionalMode())
        {
            sem_wait_ercd = osSemaphoreAcquire(p_info_drv->shared_info.sem_shared_access, osWaitForever);
            /* <-MISRA 10.6 */
            /* semaphore error check */
            if ((-1) == sem_wait_ercd)
            {
                /* set semaphore error */
                retval = EFAULT_RBSP;
            }
        }

        if (ESUCCESS == retval)
        {
            p_info_drv->shared_info.mix_vol[channel] = mix_vol;

            core_util_critical_section_enter();

            /* check going channel number */
            if (0U != (p_info_drv->shared_info.mix_run_ch & (1U << channel)))
            {
                /* check ramp volume */
                if (SCUX_MAX_RAMP_VOLUME < p_info_drv->shared_info.mix_vol[channel])
                {
                    retval = EPERM_RBSP;
                }
                else
                {
                    /* set parameter for register */
                    SCUX_SetMixVolRegister(channel);
                }
            }

            core_util_critical_section_exit();
        }

        if (0 == R_ExceptionalMode())
        {
            sem_ercd = osSemaphoreRelease(p_info_drv->shared_info.sem_shared_access);
            /* semaphore error check */
            if (osOK != sem_ercd)
            {
                /* set semaphore error */
                retval = EFAULT_RBSP;
            }
        }
    }

    return retval;
}

/******************************************************************************
End of function SCUX_IoctlSetMixVol
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_IoctlSetSsifCfg
* @brief         Set SSIF parameter.
*
*                Description:<br>
*
* @param[in]     channel:SCUX channel number.
* @param[in]     p_ssif_param:SSIF parameter.
* @retval        ESUCCESS : Operation successful.
*                EBADF_RBSP : SSIF channel has already used on other SCUX channel.
*                EPERM_RBSP : Transfer parameter is unexpected.
*                EFAULT_RBSP: Internel error is occured.
******************************************************************************/

int_t SCUX_IoctlSetSsifCfg(const scux_ssif_cfg_t * const p_ssif_param)
{
    int_t retval = ESUCCESS;
    int32_t  sem_wait_ercd;
    osStatus sem_ercd;
    scux_ssif_info_t * p_ssif_ch;

    if (NULL == p_ssif_param)
    {
        retval = EFAULT_RBSP;
    }
    else
    {
        if (SCUX_SSIF_CH_NUM > p_ssif_param->ssif_ch_num)
        {
            p_ssif_ch = SCUX_GetSsifChInfo((int_t)p_ssif_param->ssif_ch_num);
            if (NULL == p_ssif_ch)
            {
                retval = EFAULT_RBSP;
            }
            else
            {
                /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                sem_wait_ercd = osSemaphoreAcquire(p_ssif_ch->sem_ch_scux_ssif_access, osWaitForever);
                /* <-MISRA 10.6 */
                /* semaphore error check */
                if ((-1) == sem_wait_ercd)
                {
                    /* set semaphore error */
                    retval = EFAULT_RBSP;
                }
                else
                {
                    if (0 != p_ssif_ch->scux_channel)
                    {
                        retval = EBADF_RBSP;
                    }
                    else
                    {
                        p_ssif_ch->ssif_cfg.ssif_ch_num = p_ssif_param->ssif_ch_num;
                        p_ssif_ch->ssif_cfg.mode_master = p_ssif_param->mode_master;
                        p_ssif_ch->ssif_cfg.select_audio_clk = p_ssif_param->select_audio_clk;
                        p_ssif_ch->ssif_cfg.system_word = p_ssif_param->system_word;
                        p_ssif_ch->ssif_cfg.sck_polarity_rise = p_ssif_param->sck_polarity_rise;
                        p_ssif_ch->ssif_cfg.ws_polarity_high = p_ssif_param->ws_polarity_high;
                        p_ssif_ch->ssif_cfg.padding_high = p_ssif_param->padding_high;
                        p_ssif_ch->ssif_cfg.serial_data_align = p_ssif_param->serial_data_align;
                        p_ssif_ch->ssif_cfg.ws_delay = p_ssif_param->ws_delay;
                        p_ssif_ch->ssif_cfg.use_noise_cancel = p_ssif_param->use_noise_cancel;
                        p_ssif_ch->ssif_cfg.use_tdm = p_ssif_param->use_tdm;
                        p_ssif_ch->ssif_setup = true;
                    }
                }

                sem_ercd = osSemaphoreRelease(p_ssif_ch->sem_ch_scux_ssif_access);
                /* semaphore error check */
                if (osOK != sem_ercd)
                {
                    /* set semaphore error */
                    retval = EFAULT_RBSP;
                }
            }
        }
        else
        {
            retval = EPERM_RBSP;
        }
    }

    return retval;
}

/******************************************************************************
End of function SCUX_IoctlSetMixVol
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_IoctlGetWriteStat
* @brief         Get write request status.
*
*                Description:<br>
*
* @param[in]     channel:SCUX channel number.
* @param[in]     *p_write_stat:Write request status.
* @retval        None.
******************************************************************************/

void SCUX_IoctlGetWriteStat(const int_t channel, uint32_t * const p_write_stat)
{
    scux_info_ch_t * const p_info_ch = SCUX_GetDrvChInfo(channel);

    if ((NULL == p_info_ch) || (NULL == p_write_stat))
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        core_util_critical_section_enter();

        if ((SCUX_CH_STOP == p_info_ch->ch_stat) ||
            (SCUX_CH_STOP_WAIT == p_info_ch->ch_stat) ||
            (SCUX_CH_STOP_WAIT_IDLE == p_info_ch->ch_stat))
        {
            *p_write_stat = SCUX_STAT_STOP;
        }
        else if ((SCUX_CH_TRANS_IDLE == p_info_ch->ch_stat) || (SCUX_CH_TRANS_RD == p_info_ch->ch_stat))
        {
            *p_write_stat = SCUX_STAT_IDLE;
        }
        else
        {
            *p_write_stat = SCUX_STAT_TRANS;
        }

        core_util_critical_section_exit();
    }

    return;
}

/******************************************************************************
End of function SCUX_IoctlGetWriteStat
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_IoctlGetReadStat
* @brief         Get read request status.
*
*                Description:<br>
*
* @param[in]     channel:SCUX channel number.
* @param[in]     *p_read_stat:Read request status.
* @retval        None.
******************************************************************************/

void SCUX_IoctlGetReadStat(const int_t channel, uint32_t * const p_read_stat)
{
    scux_info_ch_t * const p_info_ch = SCUX_GetDrvChInfo(channel);

    core_util_critical_section_enter();

    if ((NULL == p_info_ch) || (NULL == p_read_stat))
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        if (SCUX_CH_STOP == p_info_ch->ch_stat)
        {
            *p_read_stat = SCUX_STAT_STOP;
        }
        else if ((SCUX_CH_TRANS_IDLE == p_info_ch->ch_stat) ||
                 (SCUX_CH_TRANS_WR == p_info_ch->ch_stat) ||
                 (SCUX_CH_STOP_WAIT_IDLE == p_info_ch->ch_stat))
        {
            *p_read_stat = SCUX_STAT_IDLE;
        }
        else
        {
            *p_read_stat = SCUX_STAT_TRANS;
        }

        core_util_critical_section_exit();
    }

    return;
}

/******************************************************************************
End of function SCUX_IoctlGetReadStat
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_IoctlGetDvuStat
* @brief         Get DVU status.
*
*                Description:<br>
*
* @param[in]     channel:SCUX channel number.
* @param[in]     *p_dvu_stat:DVU status.
* @retval        None.
******************************************************************************/

void SCUX_IoctlGetDvuStat(const int_t channel, uint32_t * const p_dvu_stat)
{
    scux_info_ch_t * const p_info_ch = SCUX_GetDrvChInfo(channel);

    if ((NULL == p_info_ch) || (NULL == p_dvu_stat))
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        *p_dvu_stat = (p_info_ch->p_scux_reg->p_dvu_reg->DVUSR_DVU0_0 & DVUSR_DVU0_VRSTS_MASK);
    }

    return;
}

/******************************************************************************
End of function SCUX_IoctlGetDvuStat
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_IoctlGetMuteStat
* @brief         Get MUTE status.
*
*                Description:<br>
*
* @param[in]     channel:SCUX channel number.
* @param[in]     *p_mute_stat:MUTE status.
* @retval        None.
******************************************************************************/

void SCUX_IoctlGetMuteStat(const int_t channel, uint32_t * const p_mute_stat)
{
    scux_info_ch_t * const p_info_ch = SCUX_GetDrvChInfo(channel);

    if ((NULL == p_info_ch) || (NULL == p_mute_stat))
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        core_util_critical_section_enter();

        *p_mute_stat = p_info_ch->dvu_mute_stat;

        core_util_critical_section_exit();
    }

    return;
}

/******************************************************************************
End of function SCUX_IoctlGetMuteStat
******************************************************************************/

/**************************************************************************//**
* Function Name: SCUX_IoctlGetMixStat
* @brief         Get MIX status.
*
*                Description:<br>
*
* @param[in]     channel:SCUX channel number.
* @param[in]     *p_mix_stat:MIX status.
* @retval        None.
******************************************************************************/

void SCUX_IoctlGetMixStat(const int_t channel, uint32_t * const p_mix_stat)
{
    scux_info_ch_t * const p_info_ch = SCUX_GetDrvChInfo(channel);

    if ((NULL == p_info_ch) || (NULL == p_mix_stat))
    {
        /* NON_NOTICE_ASSERT: NULL pointer */
    }
    else
    {
        *p_mix_stat = (*(p_info_ch->p_scux_reg->mixsr_mix0_0) & MIXSR_MIX0_MIXSTS_MASK);
    }

    return;
}

/******************************************************************************
End of function SCUX_IoctlGetMixStat
******************************************************************************/

