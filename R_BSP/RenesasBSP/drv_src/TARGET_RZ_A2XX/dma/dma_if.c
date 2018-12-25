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
* @file         dma_if.c
* $Rev: $
* $Date::                           $
* @brief        DMA Driver interface functions
******************************************************************************/

/*****************************************************************************
* History : DD.MM.YYYY Version Description
* : 15.01.2013 1.00 First Release
******************************************************************************/

/*******************************************************************************
Includes <System Includes>, "Project Includes"
*******************************************************************************/

#include "dma.h"
#include  "mbed_critical.h"

/******************************************************************************
Private global tables
******************************************************************************/

/**************************************************************************//**
* Function Name: R_DMA_Init
* @brie          Init DMA driver.
*                Check parameter in this function.
*
*                Description:<br>
*
* @param[in]     p_dma_init_param :Point of driver init parameter.
* @param[in,out] p_errno          :Pointer of error code.
*                                  When pointer is NULL, it isn't set error code.
*                                  error code -
*                                  OS error num : Registering handler failed.
*                                  EPERM_RBSP : Pointer of callback function which called in DMA 
*                                          error interrupt handler is NULL.
*                                  EFAULT_RBSP : dma_init_param is NULL.
* @retval        ESUCCESS -
*                  Operation successful.
*                EERROR -
*                  Error occured.
******************************************************************************/

/* ->IPA M1.1.1 If this function is the whole system, it will be called.  */
int_t R_DMA_Init(const dma_drv_init_t * const p_dma_init_param, int32_t * const p_errno)
/* <-IPA M1.1.1 */
{
    int_t         retval = ESUCCESS;
    int_t         result_init;

    DMA_SetErrCode(ESUCCESS, p_errno);

    if (NULL == p_dma_init_param)
    {
        /* set error return value */
        retval = (EERROR);
        DMA_SetErrCode(EFAULT_RBSP, p_errno);
    }

    if (ESUCCESS == retval)
    {
        /* ->MISRA 1.2 It is confirming in advance whether to be NULL or not. */
        if (NULL == p_dma_init_param->p_aio)
        /* <-MISRA 1.2 */
        {
            /* set error return value */
            retval = (EERROR);
            DMA_SetErrCode(EPERM_RBSP, p_errno);
        }
    }

    /* disable all irq */
    core_util_critical_section_enter();

    if (ESUCCESS == retval)
    {
        result_init = DMA_Initialize(p_dma_init_param);
        if (ESUCCESS != result_init)
        {
            /* set error return value */
            retval = (EERROR);
            DMA_SetErrCode(result_init, p_errno);
        }
    }

    core_util_critical_section_exit();

    return retval;
}

/******************************************************************************
End of function R_DMA_Init
******************************************************************************/

/**************************************************************************//**
* Function Name: R_DMA_UnInit
* @brie          UnInit DMA driver.
*                Check parameter in this function.
*
*                Description:<br>
*
* @param[in,out] p_errno   :Pointer of error code
*                           When pointer is NULL, it isn't set error code.
*                           error code -
*                           OS error num : Unegistering handler failed.
*                           EACCES_RBSP : Driver status isn't DMA_DRV_INIT.
*                           EBUSY_RBSP : It has been allocated already in channel.
*                           EFAULT_RBSP : Channel status is besides the status definded in dma_stat_ch_t.
* @retval        ESUCCESS -
*                  Operation successful.
*                EERROR -
*                  Error occured.
******************************************************************************/

/* ->IPA M1.1.1 If this function is the whole system, it will be called. */
int_t R_DMA_UnInit(int32_t * const p_errno)
/* <-IPA M1.1.1 */
{
    int_t          retval = ESUCCESS;
    int_t          result_uninit;
    dma_info_drv_t *dma_info_drv;
    dma_info_ch_t  *dma_info_ch;
    int_t          ch_count;
    bool_t         ch_stat_check_flag;

    DMA_SetErrCode(ESUCCESS, p_errno);

    dma_info_drv = DMA_GetDrvInstance();

    /* disable all irq */
    core_util_critical_section_enter();

    /* check driver status */
    if (ESUCCESS == retval)
    {
        if (DMA_DRV_INIT != dma_info_drv->drv_stat)
        {
            /* set error return value */
            retval = EERROR;
            DMA_SetErrCode(EACCES_RBSP, p_errno);
        }
        else
        {
            ch_stat_check_flag = false;
            ch_count = 0;
            while (false == ch_stat_check_flag)
            {
                /* check channel status */
                dma_info_ch = DMA_GetDrvChInfo(ch_count);
                if ((DMA_CH_UNINIT != dma_info_ch->ch_stat) &&
                    (DMA_CH_INIT != dma_info_ch->ch_stat))
                {
                    /*  set error return value */
                    retval = EERROR;
                    /* check channel status is busy */
                    switch (dma_info_ch->ch_stat)
                    {
                        /* These 2 cases are intentionally combined. */
                        case DMA_CH_OPEN:
                        case DMA_CH_TRANSFER:
                            DMA_SetErrCode(EBUSY_RBSP, p_errno);
                        break;

                        default:
                            DMA_SetErrCode(EFAULT_RBSP, p_errno);
                        break;
                    }
                }

                if ((DMA_CH_NUM - 1) == ch_count)
                {
                    /* channel status check end */
                    ch_stat_check_flag = true;
                }
                ch_count++;
            }
        }
        /* uninitialize DMA */
        if (ESUCCESS == retval)
        {
            result_uninit = DMA_UnInitialize();
            if (ESUCCESS != result_uninit)
            {
                /* set error return value */
                retval = EERROR;
                DMA_SetErrCode(result_uninit, p_errno);
            }
        }
    }

    core_util_critical_section_exit();

    return retval;
}

/******************************************************************************
End of function R_DMA_UnInit
******************************************************************************/

/**************************************************************************//**
* Function Name: R_DMA_Alloc
* @brie          Open DMA channel.
*                Check parameter in this function mainly.
*
*                Description:<br>
*
* @param[in]     channel   :Open channel number.
*                           If channel is (-1), it looking for free chanel and allocate.
* @param[in,out] p_errno   :Pointer of error code
*                           When pointer is NULL, it isn't set error code.
*                           error code -
*                           EINVAL_RBSP : Value of the ch is outside the range of DMA_ALLOC_CH(-1) <= ch < DMA_CH_NUM.
*                           EACCES_RBSP : Driver status isn't DMA_DRV_INIT.
*                           EBUSY_RBSP : It has been allocated already in channel.
*                           EMFILE_RBSP : When looking for a free channel, but a free channel didn't exist.
*                           ENOTSUP_RBSP : Channel status is DMA_CH_UNINIT.
*                           EFAULT_RBSP: Channel status is besides the status definded in dma_stat_ch_t.
* @retval        ESUCCESS -
*                  Operation successful.
*                EERROR -
*                  Error occured.
******************************************************************************/

/* ->IPA M1.1.1 If this function is the whole system, it will be called. */
int_t R_DMA_Alloc(const int_t channel, int32_t * const p_errno)
/* <-IPA M1.1.1 */
{
    int_t           retval = ESUCCESS;
    int_t           ercd = ESUCCESS;
    int_t           get_ch_num;
    dma_info_drv_t  *dma_info_drv;

    DMA_SetErrCode(ESUCCESS, p_errno);

    /* check driver status */
    dma_info_drv = DMA_GetDrvInstance();

    /* disable all irq */
    core_util_critical_section_enter();

    if (ESUCCESS == ercd)
    {
        if (DMA_DRV_INIT != dma_info_drv->drv_stat)
        {
            /* set error return value */
            ercd =  EACCES_RBSP;
        }
        else
        {
            /* check channel of argment */
            if ((DMA_ALLOC_CH <= channel) && (channel < DMA_CH_NUM))
            {
                if (DMA_ALLOC_CH == channel)
                {
                    get_ch_num = DMA_GetFreeChannel();
                }
                else
                {
                    get_ch_num = DMA_GetFixedChannel(channel);
                }

                /* check return number or error number */
                if ((DMA_ALLOC_CH < get_ch_num) && (get_ch_num < DMA_CH_NUM))
                {
                    /* set channel number to return value */
                    retval = get_ch_num;
                }
                else
                {
                    /* set error code to error value */
                    ercd = get_ch_num;
                }
            }
            else
            {
                /* set error return value */
                ercd =  EINVAL_RBSP;
            }
        }
    }

    /* occured error check */
    if (ESUCCESS != ercd)
    {
        retval = EERROR;
        DMA_SetErrCode(ercd, p_errno);
    }

    core_util_critical_section_exit();

    return retval;
}

/******************************************************************************
End of function R_DMA_Alloc
******************************************************************************/

/**************************************************************************//**
* Function Name: R_DMA_Free
* @brie          Close DMA channel.
*                Check parameter in this function mainly.
*
*                Description:<br>
*
* @param[in]     channel   :Close channel number.
* @param[in,out] p_errno   :Pointer of error code
*                           When pointer is NULL, it isn't set error code.
*                           error code -
*                           EBADF_RBSP : Channel status is DMA_CH_INIT.
*                           EINVAL_RBSP : Value of the ch is outside the range of (-1) < ch < (DMA_CH_NUM + 1).
*                           EACCES_RBSP : Driver status isn't DMA_DRV_INIT.
*                           EBUSY_RBSP : It has been start DMA transfer in channel.
*                           ENOTSUP_RBSP : Channel status is DMA_CH_UNINIT.
*                           EFAULT_RBSP: Channel status is besides the status definded in dma_stat_ch_t.
* @retval        ESUCCESS -
*                  Operation successful.
*                EERROR -
*                  Error occured.
******************************************************************************/

/* ->IPA M1.1.1 If this function is the whole system, it will be called. */
int_t R_DMA_Free(const int_t channel, int32_t *const p_errno)
/* <-IPA M1.1.1 */
{
    int_t           retval = ESUCCESS;
    dma_info_drv_t  *dma_info_drv;
    dma_info_ch_t   *dma_info_ch;
    int_t           error_code;

    DMA_SetErrCode(ESUCCESS, p_errno);

    /* disable all irq */
    core_util_critical_section_enter();

    /* check channel of argument */
    if ((0 <= channel) && (channel < DMA_CH_NUM))
    {
         /* check driver status */
        dma_info_drv = DMA_GetDrvInstance();

        if (ESUCCESS == retval)
        {
            /* check driver status */
            if (DMA_DRV_INIT == dma_info_drv->drv_stat)
            {
                dma_info_ch = DMA_GetDrvChInfo(channel);

                if (ESUCCESS == retval)
                {
                    if (DMA_CH_OPEN == dma_info_ch->ch_stat)
                    {
                        DMA_CloseChannel(channel);
                    }
                    else
                    {
                        /* set error return value */
                        retval = EERROR;
                        switch (dma_info_ch->ch_stat)
                        {
                            case DMA_CH_UNINIT:
                                error_code = ENOTSUP_RBSP;
                            break;

                            case DMA_CH_INIT:
                                error_code = EBADF_RBSP;
                            break;

                            case DMA_CH_TRANSFER:
                                error_code = EBUSY_RBSP;
                            break;

                            default:
                                error_code = EFAULT_RBSP;
                            break;
                        }
                        DMA_SetErrCode(error_code, p_errno);
                    }
                }
            }
            else
            {
                /* set error return value */
                retval = EERROR;
                DMA_SetErrCode(EACCES_RBSP, p_errno);

            }
        }
    }
    else
    {
        /* set error return value */
        retval = EERROR;
        DMA_SetErrCode(EINVAL_RBSP, p_errno);
    }

    core_util_critical_section_exit();

    return retval;
}

/******************************************************************************
End of function R_DMA_Free
******************************************************************************/

/**************************************************************************//**
* Function Name: R_DMA_Setup
* @brie          Setup DMA transfer parameter.
*                Check parameter in this function mainly.
*
*                Description:<br>
*
* @param[in]     channel   :Setup channel number.
* @param[in]     p_ch_setup:Set up parameters.
* @param[in,out] p_errno   :Pointer of error code
*                           When pointer is NULL, it isn't set error code.
*                           error code -
*                           EBADF_RBSP : Channel status is DMA_CH_INIT.
*                           EINVAL_RBSP : Value of the ch is outside the range of (-1) < ch < (DMA_CH_NUM + 1).
*                           EBUSY_RBSP : It has been start DMA transfer in channel.
*                           ENOTSUP_RBSP : Channel status is DMA_CH_UNINIT.
*                           EPERM_RBSP : The value in p_ch_setup isn't in the right range.
*                           EFAULT_RBSP: Channel status is besides the status definded in dma_stat_ch_t. p_ch_setup is NULL.
* @retval        ESUCCESS -
*                  Operation successful.
*                EERROR -
*                  Error occured.
******************************************************************************/

/* ->IPA M1.1.1 If this function is the whole system, it will be called. */
int_t R_DMA_Setup(const int_t channel, const dma_ch_setup_t * const p_ch_setup,
                  int32_t * const p_errno)
/* <-IPA M1.1.1 */
{
    int_t            retval = ESUCCESS;
    dma_info_ch_t    *dma_info_ch;
    int_t            error_code;
    uint32_t         cfg_table_count;
    dma_ch_cfg_t     ch_cfg_set_table;
    uint32_t         set_reqd;
    bool_t           check_table_flag;

    /* Resouce Configure Set Table */
    static const dma_ch_cfg_t ch_cfg_table[] =
    {
        {DMA_RS_SSITXI0,    CHCFG_SET_AM_BUS_CYCLE, CHCFG_SET_LVL_EDGE,  CHCFG_SET_REQD_DST  },
        {DMA_RS_SSIRXI0,    CHCFG_SET_AM_BUS_CYCLE, CHCFG_SET_LVL_EDGE,  CHCFG_SET_REQD_SRC  },
        {DMA_RS_SSITXI1,    CHCFG_SET_AM_BUS_CYCLE, CHCFG_SET_LVL_EDGE,  CHCFG_SET_REQD_DST  },
        {DMA_RS_SSIRXI1,    CHCFG_SET_AM_BUS_CYCLE, CHCFG_SET_LVL_EDGE,  CHCFG_SET_REQD_SRC  },
        {DMA_RS_SSIRTI2,    CHCFG_SET_AM_BUS_CYCLE, CHCFG_SET_LVL_EDGE,  CHCFG_REQD_UNDEFINED},
        {DMA_RS_SSITXI3,    CHCFG_SET_AM_BUS_CYCLE, CHCFG_SET_LVL_EDGE,  CHCFG_SET_REQD_DST  },
        {DMA_RS_SSIRXI3,    CHCFG_SET_AM_BUS_CYCLE, CHCFG_SET_LVL_EDGE,  CHCFG_SET_REQD_SRC  },

        {DMA_RS_SPDIFRXI,   CHCFG_SET_AM_BUS_CYCLE, CHCFG_SET_LVL_LEVEL, CHCFG_SET_REQD_SRC  },
        {DMA_RS_SPDIFTXI,   CHCFG_SET_AM_BUS_CYCLE, CHCFG_SET_LVL_LEVEL, CHCFG_SET_REQD_DST  },
    };

    DMA_SetErrCode(ESUCCESS, p_errno);
    /* dummy init set_reqd */
    set_reqd = CHCFG_REQD_UNDEFINED;
    ch_cfg_set_table = ch_cfg_table[0];

    /* check channel of argument */
    if ((0 <= channel) && (channel < DMA_CH_NUM))
    {
        if (NULL != p_ch_setup)
        {
            /* check setup parameter */
            /* check AIOCB pointer */
            if (NULL == p_ch_setup->p_aio)
            {
                /* set error return value */
                retval = EERROR;
                DMA_SetErrCode(EPERM_RBSP, p_errno);
            }

            if (ESUCCESS == retval)
            {
                /* check DMA transfer unit size for destination */
                if (((int_t)p_ch_setup->dst_width <= DMA_UNIT_MIN) ||
                    ((int_t)p_ch_setup->dst_width >= DMA_UNIT_MAX))
                {
                    /* set error return value */
                    retval = EERROR;
                    DMA_SetErrCode(EPERM_RBSP, p_errno);
                }
            }

            if (ESUCCESS == retval)
            {
                /* check DMA transfer unit size for source */
                if (((int_t)p_ch_setup->src_width <= DMA_UNIT_MIN) ||
                    ((int_t)p_ch_setup->src_width >= DMA_UNIT_MAX))
                {
                    /* set error return value */
                    retval = EERROR;
                    DMA_SetErrCode(EPERM_RBSP, p_errno);
                }
            }

            if (ESUCCESS == retval)
            {
                /* check DMA address count direction for destination */
                if (((int_t)p_ch_setup->dst_cnt <= DMA_ADDR_MIN) ||
                    ((int_t)p_ch_setup->dst_cnt >= DMA_ADDR_MAX))
                {
                    /* set error return value */
                    retval = EERROR;
                    DMA_SetErrCode(EPERM_RBSP, p_errno);
                }
            }

            if (ESUCCESS == retval)
            {
                /* check DMA address count direction for source */
                if (((int_t)(p_ch_setup->src_cnt) <= DMA_ADDR_MIN) ||
                    ((int_t)p_ch_setup->src_cnt >= DMA_ADDR_MAX))
                {
                    /* set error return value */
                    retval = EERROR;
                    DMA_SetErrCode(EPERM_RBSP, p_errno);
                }
            }

            if (ESUCCESS == retval)
            {
                /* check DMA transfer direction */
                if (((int_t)p_ch_setup->direction <= DMA_REQ_MIN) ||
                    ((int_t)p_ch_setup->direction >= DMA_REQ_MAX))
                {
                    /* set error return value */
                    retval = EERROR;
                    DMA_SetErrCode(EPERM_RBSP, p_errno);
                }
            }

            if (ESUCCESS == retval)
            {
                /* check DMA transfer resouce */
                check_table_flag = false;
                cfg_table_count = 0;
                while (false == check_table_flag)
                {
                    if (p_ch_setup->resource == ch_cfg_table[cfg_table_count].dmars)
                    {
                        /* check reqd is undefined */
                        if (CHCFG_REQD_UNDEFINED == ch_cfg_table[cfg_table_count].reqd)
                        {
                            /* set reqd value on fixed value */
                            if (DMA_REQ_SRC == p_ch_setup->direction)
                            {
                                set_reqd = CHCFG_SET_REQD_SRC;
                            }
                            else
                            {
                                set_reqd = CHCFG_SET_REQD_DST;
                            }
                        }
                        else
                        {
                            /* set reqd value in channel config table */
                            set_reqd = ch_cfg_table[cfg_table_count].reqd;
                        }
                        /* set channel config table address for DMA_SetParam() */
                        ch_cfg_set_table = ch_cfg_table[cfg_table_count];
                        check_table_flag = true;
                    }
                    if (false == check_table_flag)
                    {
                        /* resource value did not exist in channel config table */
                        if ((uint32_t)((sizeof(ch_cfg_table)/sizeof(dma_ch_cfg_t)) - 1U) == cfg_table_count)
                        {
                            /* set error return value */
                            retval = EERROR;
                            DMA_SetErrCode(EPERM_RBSP, p_errno);
                            check_table_flag = true;
                        }
                        cfg_table_count++;
                    }
                }
            }

            /* disable all irq */
            core_util_critical_section_enter();

            if (ESUCCESS == retval)
            {
                dma_info_ch = DMA_GetDrvChInfo(channel);

                if (ESUCCESS == retval)
                {
                    if (DMA_CH_OPEN == dma_info_ch->ch_stat)
                    {
                        /* set up parameter */
                        DMA_SetParam(channel, p_ch_setup, &ch_cfg_set_table, set_reqd);
                    }
                    else
                    {
                        /* set error return value */
                        retval = EERROR;
                        switch (dma_info_ch->ch_stat)
                        {
                            case DMA_CH_UNINIT:
                                error_code = ENOTSUP_RBSP;
                            break;

                            case DMA_CH_INIT:
                                error_code = EBADF_RBSP;
                            break;

                            case DMA_CH_TRANSFER:
                                error_code = EBUSY_RBSP;
                            break;

                            default:
                                error_code = EFAULT_RBSP;
                            break;
                        }
                        DMA_SetErrCode(error_code, p_errno);
                    }
                }
            }

            core_util_critical_section_exit();
        }
        else
        {
            /* set error return value */
            retval = EERROR;
            DMA_SetErrCode(EFAULT_RBSP, p_errno);
        }
    }
    else
    {
        /* set error return value */
        retval = EERROR;
        DMA_SetErrCode(EINVAL_RBSP, p_errno);
    }

    return retval;
}

/******************************************************************************
End of function R_DMA_SetParam
******************************************************************************/

/**************************************************************************//**
* Function Name: R_DMA_Start
* @brie          Start DMA transfer.
*                Check parameter in this function mainly.
*
*                Description:<br>
*
* @param[in]     channel   :DMA start channel number.
* @param[in]     p_dma_data:DMA address parameters.
* @param[in,out] p_errno   :Pointer of error code
*                           When pointer is NULL, it isn't set error code.
*                           error code -
*                           EBADF_RBSP : Channel status is DMA_CH_INIT.
*                           EINVAL_RBSP : Value of the ch is outside the range of (-1) < ch < (DMA_CH_NUM + 1).
*                           EBUSY_RBSP : It has been start DMA transfer in channel.
*                           ENOTSUP_RBSP : Channel status is DMA_CH_UNINIT.
*                           EPERM_RBSP : The value in p_ch_setup isn't in the right range.
*                           EFAULT_RBSP: Channel status is besides the status definded in dma_stat_ch_t. p_dma_data is NULL.
* @retval        ESUCCESS -
*                  Operation successful.
*                EERROR -
*                  Error occured.
******************************************************************************/

/* ->IPA M1.1.1 If this function is the whole system, it will be called. */
int_t R_DMA_Start(const int_t channel, const dma_trans_data_t * const p_dma_data,
                  int32_t * const p_errno)
/* <-IPA M1.1.1 */
{
    int_t          retval = ESUCCESS;
    dma_info_ch_t  *dma_info_ch;
    int_t          error_code;

    DMA_SetErrCode(ESUCCESS, p_errno);

    /* disable all irq */
    core_util_critical_section_enter();

    /* check channel of argument */
    if ((0 <= channel) && (channel < DMA_CH_NUM))
    {
        if (NULL != p_dma_data)
        {
            /* check address parameter */
            /* check DMA transfer count destination address is 0 */
            if (0U == p_dma_data->count)
            {
                /* set error return value */
                retval = EERROR;
                DMA_SetErrCode(EPERM_RBSP, p_errno);
            }

            if (ESUCCESS == retval)
            {
                dma_info_ch = DMA_GetDrvChInfo(channel);

                if (ESUCCESS == retval)
                {
                    if (DMA_CH_OPEN == dma_info_ch->ch_stat)
                    {
                        /* set bus paramter for DMA */
                        DMA_BusParam(channel, p_dma_data);
                        /* set up address parameter */
                        /* Next register set is 0 */
                        DMA_SetData(channel, p_dma_data, 0);
                        /* DMA transfer start */
                        DMA_Start(channel, false);
                    }
                    else
                    {
                        /* set error return value */
                        retval = EERROR;
                        switch (dma_info_ch->ch_stat)
                        {
                            case DMA_CH_UNINIT:
                                error_code = ENOTSUP_RBSP;
                             break;

                             case DMA_CH_INIT:
                                error_code = EBADF_RBSP;
                             break;

                            case DMA_CH_TRANSFER:
                                error_code = EBUSY_RBSP;
                            break;

                            default:
                                error_code = EFAULT_RBSP;
                             break;
                        }
                        DMA_SetErrCode(error_code, p_errno);
                    }
                }
            }
        }
        else
        {
            /* set error return value */
            retval = EERROR;
            DMA_SetErrCode(EFAULT_RBSP, p_errno);
        }
    }
    else
    {
        /* set error return value */
        retval = EERROR;
        DMA_SetErrCode(EINVAL_RBSP, p_errno);
    }

    core_util_critical_section_exit();

    return retval;
}

/******************************************************************************
End of function R_DMA_Start
******************************************************************************/

/**************************************************************************//**
* Function Name: R_DMA_NextData
* @brie          Set continous DMA mode.
*                Check parameter in this function mainly.
*
*                Description:<br>
*
* @param[in]     channel   :Continuous DMA channel number.
* @param[in,out] p_dma_data:DMA address parameters.
* @param[in,out] p_errno   :Pointer of error code
*                           When pointer is NULL, it isn't set error code.
*                           error code -
*                           EBADF_RBSP : Channel status is DMA_CH_INIT.
*                           EINVAL_RBSP : Value of the ch is outside the range of (-1) < ch < (DMA_CH_NUM + 1).
*                           EBUSY_RBSP : It has been set continous DMA transfer.
*                           ENOTSUP_RBSP : Channel status is DMA_CH_UNINIT.
*                           EPERM_RBSP : The value in p_ch_setup isn't in the right range.
*                           EFAULT_RBSP: Channel status is besides the status definded in dma_stat_ch_t. p_dma_data is NULL.
* @retval        ESUCCESS -
*                  Operation successful.
*                EERROR -
*                  Error occured.
******************************************************************************/

/* ->IPA M1.1.1 If this function is the whole system, it will be called. */
int_t R_DMA_NextData(const int_t channel, const dma_trans_data_t * const p_dma_data,
                     int32_t * const p_errno)
/* <-IPA M1.1.1 */
{
    int_t          retval = ESUCCESS;
    dma_info_ch_t  *dma_info_ch;
    int_t          error_code;

    DMA_SetErrCode(ESUCCESS, p_errno);

    /* disable all irq */
    core_util_critical_section_enter();

    /* check channel of argument */
    if ((0 <= channel) && (channel < DMA_CH_NUM))
    {
        if (NULL != p_dma_data)
        {
            /* check address parameter */
            /* check DMA transfer count destination address is 0 */
            if (0U == p_dma_data->count)
            {
                /* set error return value */
                retval = EERROR;
                DMA_SetErrCode(EPERM_RBSP, p_errno);
            }

            if (ESUCCESS == retval)
            {
                dma_info_ch = DMA_GetDrvChInfo(channel);

                if (ESUCCESS == retval)
                {
                    if ((DMA_CH_OPEN == dma_info_ch->ch_stat) ||
                        (DMA_CH_TRANSFER == dma_info_ch->ch_stat))
                    {
                        if (false == dma_info_ch->next_dma_flag)
                        {
                            /* set up address parameter for continous DMA*/
                            DMA_SetNextData(channel, p_dma_data);
                        }
                        else
                        {
                            /* set error return value */
                            retval = EERROR;
                            DMA_SetErrCode(EBUSY_RBSP, p_errno);
                        }
                    }
                    else
                    {
                        /* set error return value */
                        retval = EERROR;
                        switch (dma_info_ch->ch_stat)
                        {
                            case DMA_CH_UNINIT:
                                error_code = ENOTSUP_RBSP;
                            break;

                            case DMA_CH_INIT:
                                error_code = EBADF_RBSP;
                            break;

                            default:
                                error_code = EFAULT_RBSP;
                            break;
                        }
                        DMA_SetErrCode(error_code, p_errno);
                    }
                }
            }
        }
        else
        {
            /* set error return value */
            retval = EERROR;
            DMA_SetErrCode(EFAULT_RBSP, p_errno);
        }
    }
    else
    {
        /* set error return value */
        retval = EERROR;
        DMA_SetErrCode(EINVAL_RBSP, p_errno);
    }

    core_util_critical_section_exit();

    return retval;
}

/******************************************************************************
End of function R_DMA_NextData
******************************************************************************/

/**************************************************************************//**
* Function Name: R_DMA_Cancel
* @brie          Cancel DMA transfer.
*                Check parameter in this function mainly.
*
*                Description:<br>
*
* @param[in]     channel   :Cancel DMA channel number.
* @param[in]    p_remain   :Remain data size of DMA transfer when it stopping.
* @param[in,out] p_errno   :Pointer of error code
*                           When pointer is NULL, it isn't set error code.
*                           error code -
*                           EBADF_RBSP : Channel status is DMA_CH_INIT or DMA_CH_OPEN. (DMA stopped)
*                           EINVAL_RBSP : Value of the ch is outside the range of (-1) < ch < (DMA_CH_NUM + 1).
*                           ENOTSUP_RBSP : Channel status is DMA_CH_UNINIT.
*                           EFAULT_RBSP: Channel status is besides the status definded in dma_stat_ch_t. p_remain is NULL.
* @retval        ESUCCESS -
*                  Operation successful.
*                EERROR -
*                  Error occured.
******************************************************************************/

/* ->IPA M1.1.1 If this function is the whole system, it will be called. */
int_t R_DMA_Cancel(const int_t channel, uint32_t * const p_remain, int32_t * const p_errno)
/* <-IPA M1.1.1 */
{
    int_t          retval = ESUCCESS;
    dma_info_ch_t  *dma_info_ch;
    int_t          error_code;

    DMA_SetErrCode(ESUCCESS, p_errno);

    /* disable all irq */
    core_util_critical_section_enter();

    /* check channel of argument */
    if ((0 <= channel) && (channel < DMA_CH_NUM))
    {
        /* check whether p_remain is NULL */
        if (NULL != p_remain)
        {
            dma_info_ch = DMA_GetDrvChInfo(channel);

            if (ESUCCESS == retval)
            {
                if (DMA_CH_TRANSFER == dma_info_ch->ch_stat)
                {
                    /* set up address parameter for continous DMA*/
                    DMA_Stop(channel, p_remain);
                }
                else
                {
                    /* set error return value */
                    retval = EERROR;
                    switch (dma_info_ch->ch_stat)
                    {
                        case DMA_CH_UNINIT:
                            error_code = ENOTSUP_RBSP;
                        break;

                        case DMA_CH_INIT:
                            error_code = EBADF_RBSP;
                        break;

                        case DMA_CH_OPEN:
                            error_code = EBADF_RBSP;
                        break;

                        default:
                            error_code = EFAULT_RBSP;
                        break;
                    }
                    DMA_SetErrCode(error_code, p_errno);
                }
            }
        }
        else
        {
            /* set error return value */
            retval = EERROR;
            DMA_SetErrCode(EFAULT_RBSP, p_errno);
        }
    }
    else
    {
        /* set error return value */
        retval = EERROR;
        DMA_SetErrCode(EINVAL_RBSP, p_errno);
    }

    core_util_critical_section_exit();

    return retval;
}

/******************************************************************************
End of function R_DMA_Cancel
******************************************************************************/

