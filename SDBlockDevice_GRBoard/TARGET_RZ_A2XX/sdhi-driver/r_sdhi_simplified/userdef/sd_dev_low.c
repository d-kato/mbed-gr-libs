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
* http://www.renesas.com/disclaimer *
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.    
*******************************************************************************/
/******************************************************************************
* System Name  : SDHI Driver
* File Name    : sd_dev_low.c
* Version      : 1.00
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RSK board
* Description  : RZ/A2M SD Driver Sample Program
* Operation    : 
* Limitations  : Ch0 and Ch1 can't be used at the same time, because the timers
*              : used on Ch0 and Ch1 are common.
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 16.03.2018 1.00    First Release
*         : 14.12.2018 1.01    Changed the DMAC soft reset procedure.
*         : 28.12.2018 1.02    Support for OS
******************************************************************************/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_typedefs.h"
#include "iodefine.h"
#include "iobitmask.h"
#if(1) // mbed
#include "cmsis.h"
#include "cmsis_os.h"
#include "irq_ctrl.h"
#include "mbed_wait_api.h"
#include "platform/mbed_critical.h"
#else
#include "rza_io_regrw.h"
#include "r_intc_lld_rza2m.h"
#include "r_stb_lld_rza2m.h"
#include "r_gpio_lld_rza2m.h"
#include "r_devlink_wrapper.h"
#include "r_os_abstraction_api.h"
#include "r_compiler_abstraction_api.h"
#endif
#include "r_sdif.h"
#include "r_sd_cfg.h"
#include "sd_dev_dmacdrv.h"
#include "r_sdhi_simplified_drv_sc_cfg.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
#define INT_LEVEL_SDHI      (26u)       /* SDHI interrupt level  */

#define SDHI_CH_0           (0)
#define SDHI_CH_1           (1)
#define SDHI_CH_NUM         (2)

#define SDHI_CARDDET_TIME   (1000uL)
#define SDHI_POWERON_TIME   (100uL)

#ifdef SDCFG_HWINT
#define SDHI_MAX_WAITTIME   (0xFFFFuL)

#else /* #ifdef SDCFG_HWINT */
#define SDHI_1MSEC          (1uL)

#endif /* #ifdef SDCFG_HWINT */

#define SDHI_TO_SPEED       (512uL)
#define SDHI_1024BYTE       (1024uL)
#define SDHI_1000MSEC       (1000uL)

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
sdhi_info_dev_ch_t *sddev_get_dev_ch_instance(int32_t sd_port);

/******************************************************************************
Private global variables and functions
******************************************************************************/
#if 0
static uint8_t g_sdhi_priority_backup;
#endif

#if(1) // mbed
static void sddev_sd_int_handler_0(void);
static void sddev_sd_int_handler_1(void);
osSemaphoreDef(sdint_sem_sync);
osSemaphoreDef(sdint_sem_dma);
#else
static void sddev_sd_int_handler_0(uint32_t int_sense);
static void sddev_sd_int_handler_1(uint32_t int_sense);
#endif

static int32_t sddev_get_sc_table_config_ch(int32_t sd_port, int32_t *p_sc_port);

#if(1) // mbed
static sdhi_info_dev_ch_t g_sdhi_dev_ch[SDHI_CH_NUM] =
{
    /*--- ch 0 ---*/
    { { false                                                         }, /* stb  */
      { SDHI0_0_IRQn,          INT_LEVEL_SDHI, sddev_sd_int_handler_0 }, /* intc */
      { false                                                         }, /* gpio */
#ifdef SDCFG_HWINT
      { 0uL,                   0uL                                    }  /* semaphore */
#endif /* #ifdef SDCFG_HWINT */
    },
    /*--- ch 1 ---*/
    { { false                                                         }, /* stb  */
      { SDHI1_0_IRQn,          INT_LEVEL_SDHI, sddev_sd_int_handler_1 }, /* intc */
      { false                                                         }, /* gpio */
#ifdef SDCFG_HWINT
      { 0uL,                   0uL                                    }  /* semaphore */
#endif /* #ifdef SDCFG_HWINT */
    }
};
#else
static sdhi_info_dev_ch_t g_sdhi_dev_ch[SDHI_CH_NUM] =
{
    /*--- ch 0 ---*/
    { { false,                 MODULE_SDMMC0                          }, /* stb  */
      { INTC_ID_SDMMC_SDHI0_0, INT_LEVEL_SDHI, sddev_sd_int_handler_0 }, /* intc */
      { false                                                         }, /* gpio */
#ifdef SDCFG_HWINT
      { 0uL,                   0uL                                    }  /* semaphore */
#endif /* #ifdef SDCFG_HWINT */
    },
    /*--- ch 1 ---*/
    { { false,                 MODULE_SDMMC1                          }, /* stb  */
      { INTC_ID_SDMMC_SDHI1_0, INT_LEVEL_SDHI, sddev_sd_int_handler_1 }, /* intc */
      { false                                                         }, /* gpio */
#ifdef SDCFG_HWINT
      { 0uL,                   0uL                                    }  /* semaphore */
#endif /* #ifdef SDCFG_HWINT */
    }
};
#endif

/******************************************************************************
* Function Name: int32_t sddev_init(int32_t sd_port)
* Description  : Initialize H/W to use SDHI
* Arguments    : int32_t sd_port : channel no (0 or 1)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sddev_init(int32_t sd_port)
{
#if(1) // mbed
    int32_t             ret;
    sdhi_info_dev_ch_t  *p_ch;

    ret  = SD_OK;
    p_ch = sddev_get_dev_ch_instance(sd_port);

    if (p_ch != NULL) {
        core_util_critical_section_enter();

        /* --- SDHI, clear module standby start ---*/
        /* [Canceling Module Standby Function] */
        /* a. Release from the module standby state after the activation of the
              module by a power-on reset while the MSTP bit is set to 1
           1. Clear the MSTP bit to 0.
           2. After that, dummy-read the same register. */

        /* b. Release from the module standby state after a transition to standby
              following activation of the module
           1. Clear the MSTP bit to 0, then dummy-read the same register.   */
        volatile uint8_t reg_read_8;

        if (sd_port == 0) {
            CPG.STBCR10.BYTE &= ~0x0C;
            reg_read_8 = CPG.STBCR10.BYTE; /* dummy read */
        } else  {
            CPG.STBCR10.BYTE &= ~0x03u;
            reg_read_8 = CPG.STBCR10.BYTE; /* dummy read */
        }
        (void)reg_read_8;

        if (p_ch->stb.stb_pon_init == false) {
            /* case a */
            p_ch->stb.stb_pon_init = true;
        } else {
            /* case b */
            if (sd_port == 0) {
                while (1) {
                    CPG.STBREQ1.BYTE &= ~0x04;
                    reg_read_8 = CPG.STBREQ1.BYTE; /* dummy read */
                    reg_read_8 = CPG.STBACK1.BYTE;
                    if ((reg_read_8 & 0x04) == 0) {
                        break;
                    }
                }
            } else {
                while (1) {
                    CPG.STBREQ1.BYTE &= ~0x02;
                    reg_read_8 = CPG.STBREQ1.BYTE; /* dummy read */
                    reg_read_8 = CPG.STBACK1.BYTE;
                    if ((reg_read_8 & 0x02) == 0) {
                        break;
                    }
                }
            }
        }
        /* --- SDHI, clear module standby end ---*/

        core_util_critical_section_exit();

#ifdef SDCFG_HWINT
        if (ret == SD_OK) {
            /* ---- Register SDHI interrupt handler ---- */
            (void)IRQ_SetHandler(p_ch->intc.int_id, p_ch->intc.func);
            /* ---- Set priority of SDHI interrupt handler to INT_LEVEL_SDHI ---- */
            GIC_SetConfiguration(p_ch->intc.int_id, 1);
            (void)IRQ_SetPriority(p_ch->intc.int_id, p_ch->intc.int_priority);
            /* ---- Validate SDHI interrupt ---- */
            (void)IRQ_Enable(p_ch->intc.int_id);

            if (p_ch->semaphore.sem_sync == NULL) {
                p_ch->semaphore.sem_sync = osSemaphoreNew(0xffff, 0, osSemaphore(sdint_sem_sync));
            }
            if (p_ch->semaphore.sem_sync == NULL) {
                ret = SD_ERR;
            }
            if (p_ch->semaphore.sem_dma == NULL) {
                p_ch->semaphore.sem_dma = osSemaphoreNew(0xffff, 0, osSemaphore(sdint_sem_dma));
            }
            if (p_ch->semaphore.sem_dma == NULL) {
                ret = SD_ERR;
            }
            if (ret == SD_ERR) {
                if (p_ch->semaphore.sem_sync != NULL) {
                    osSemaphoreDelete(p_ch->semaphore.sem_sync);
                    p_ch->semaphore.sem_sync = NULL;
                }
                if (p_ch->semaphore.sem_dma != NULL) {
                    osSemaphoreDelete(p_ch->semaphore.sem_dma);
                    p_ch->semaphore.sem_dma = NULL;
                }
            }
        }
#endif /* #ifdef SDCFG_HWINT */
    } else { /* if (p_ch != NULL) */
        ret = SD_ERR;
    }
    return ret;
#else
    int32_t             ret;
    sdhi_info_dev_ch_t  *p_ch;
    uint32_t            was_masked;
    e_stb_err_t         e_stb_err;
    int_t               gpio_handle;
    int_t               gpio_err;
#ifdef SDCFG_HWINT
    e_r_drv_intc_err_t  e_intc_err;
    bool_t              chk;
#endif /* #ifdef SDCFG_HWINT */
    int32_t             sc_port;

    ret  = SD_OK;
    p_ch = sddev_get_dev_ch_instance(sd_port);

    if (p_ch != NULL)
    {
        was_masked = __disable_irq();

        /* --- SDHI, clear module standby start ---*/
        /* [Canceling Module Standby Function] */
        /* a. Release from the module standby state after the activation of the
              module by a power-on reset while the MSTP bit is set to 1
           1. Clear the MSTP bit to 0.
           2. After that, dummy-read the same register. */

        /* b. Release from the module standby state after a transition to standby
              following activation of the module
           1. Clear the MSTP bit to 0, then dummy-read the same register.   */
        e_stb_err = R_STB_StartModule(p_ch->stb.stb_ch);
        if (e_stb_err == STB_SUCCESS)
        {
            if (p_ch->stb.stb_pon_init == false)
            {
                /* case a */
                p_ch->stb.stb_pon_init = true;
            }
            else
            {
                /* case b */
                /* No timer monitering. */
                e_stb_err = STB_AGAIN;
                while (e_stb_err == STB_AGAIN)
                {
                    /* 2. Clear the corresponding bit in the STBREQ register to 0
                          to cancel the request to stop the module.
                       3. Confirm that the corresponding bit in the STBACK register
                          has been cleared to 0, indicating the completion of release
                          from standby. */
                    e_stb_err = R_STB_RequestModuleStart(p_ch->stb.stb_ch);
                }
            }
        }
        if (e_stb_err != STB_SUCCESS)
        {
            ret = SD_ERR;
        }
        /* --- SDHI, clear module standby end ---*/

        /* --- SDx_CD, SDx_WP(x = 0(ch0) or 1(ch1)) start --- */
        if (ret == SD_OK)
        {
            if (p_ch->gpio.gpio_init == false)
            {
                /* --- Open GPIO --- */
                gpio_handle = direct_open("gpio", 0);
                if (gpio_handle < 0)
                {
                    ret = SD_ERR;
                }
                else
                {
                    /* --- Control GPIO --- */
                    ret = sddev_get_sc_table_config_ch(sd_port, &sc_port);
                    if (ret == SD_OK)
                    {
                        gpio_err = direct_control(gpio_handle
                                                 ,(uint32_t)CTL_GPIO_INIT_BY_TABLE
                                                 ,(st_r_drv_sdhi_sc_config_t *)&SDHI_SC_TABLE[sc_port].pin);
                        if (gpio_err < 0)
                        {
                            ret = SD_ERR;
                        }
                        else
                        {
                            p_ch->gpio.gpio_init = true;
                        }
                    }
                    /* --- Close GPIO --- */
                    (void)direct_close(gpio_handle);
                }
            } /* if (p_ch->gpio.gpio_init == false) */
        }
        /* --- SDx_CD, SDx_WP(x = 0(ch0) or 1(ch1)) end --- */

        if (was_masked == 0uL)
        {
            __enable_irq();
        }

#ifdef SDCFG_HWINT
        if (ret == SD_OK)
        {
            /* ---- Register SDHI interrupt handler start ---- */
            e_intc_err = R_INTC_RegistIntFunc(p_ch->intc.int_id, p_ch->intc.func);
            if (e_intc_err != INTC_SUCCESS)
            {
                /* Invalid id */
                ret = SD_ERR;
            }
            else
            {
                /* ---- Set priority of SDHI interrupt handler to INT_LEVEL_SDHI ---- */
                e_intc_err = R_INTC_SetPriority(p_ch->intc.int_id, p_ch->intc.int_priority);
                if (e_intc_err != INTC_SUCCESS)
                {
                    /* Invalid id or invalid priority */
                    ret = SD_ERR;
                }
                else
                {
                    /* ---- Validate SDHI interrupt ---- */
                    e_intc_err = R_INTC_Enable(p_ch->intc.int_id);
                    if (e_intc_err != INTC_SUCCESS)
                    {
                        /* Invalid id */
                        ret = SD_ERR;
                    }
                }
            }
            /* ---- Register SDHI interrupt handler end ---- */

            if (ret == SD_ERR)
            {
                /* --- Invalidate SDHI interrupt --- */
                e_intc_err = R_INTC_Disable(p_ch->intc.int_id);
                if (e_intc_err != INTC_SUCCESS)
                {
                    /* do nothing */
                }
                /* --- Unregister SDHI interrupt handler --- */
                e_intc_err = R_INTC_RegistIntFunc(p_ch->intc.int_id, NULL);
                if (e_intc_err != INTC_SUCCESS)
                {
                    /* do nothing */
                }
            }
        }

        if (ret == SD_OK)
        {
            /* --- Create semaphore start --- */
            if (p_ch->semaphore.sem_sync == 0uL)
            {
                chk = R_OS_SemaphoreCreate(&p_ch->semaphore.sem_sync, 1uL);
                if (chk == true)
                {
                    chk = R_OS_SemaphoreWait(&p_ch->semaphore.sem_sync, 0uL);
                }
                if (chk == false)
                {
                    ret = SD_ERR;
                }
            }
            if (p_ch->semaphore.sem_dma == 0uL)
            {
                chk = R_OS_SemaphoreCreate(&p_ch->semaphore.sem_dma, 1uL);
                if (chk == true)
                {
                    chk = R_OS_SemaphoreWait(&p_ch->semaphore.sem_dma, 0uL);
                }
                if (chk == false)
                {
                    ret = SD_ERR;
                }
            }
            /* --- Create semaphore end --- */

            if (ret == SD_ERR)
            {
                /* --- Delete semaphore --- */
                R_OS_SemaphoreDelete(&p_ch->semaphore.sem_sync);      /* p_ch->semaphore.sem_sync = 0 */
                R_OS_SemaphoreDelete(&p_ch->semaphore.sem_dma);       /* p_ch->semaphore.sem_dma  = 0 */
            }
        }
#endif /* #ifdef SDCFG_HWINT */

        if (ret == SD_OK)
        {
            /* ---- wait card detect ---- */
            R_OS_TaskSleep(SDHI_CARDDET_TIME);
        }
    }
    else /* if (p_ch != NULL) */
    {
        ret = SD_ERR;
    }
    return ret;
#endif
}

/******************************************************************************
* Function Name: int32_t sddev_power_on(int32_t sd_port);
* Description  : Power-on H/W to use SDHI
* Arguments    : int32_t sd_port : channel no (0 or 1)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sddev_power_on(int32_t sd_port)
{
    /* ---Power On SD ---- */

    /* ---- Wait for  SD Wake up ---- */
#if(1) // mbed
    wait_ms(SDHI_POWERON_TIME);
#else
    R_OS_TaskSleep(SDHI_POWERON_TIME);
#endif

    return SD_OK;
}

/******************************************************************************
* Function Name: int32_t sddev_power_off(int32_t sd_port);
* Description  : Power-off H/W to use SDHI
* Arguments    : int32_t sd_port : channel no (0 or 1)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sddev_power_off(int32_t sd_port)
{
    return SD_OK;
}

/******************************************************************************
* Function Name: int32_t sddev_read_data(int32_t sd_port, uint8_t *buff, uint32_t reg_addr, int32_t num);
* Description  : read from SDHI buffer FIFO
* Arguments    : int32_t sd_port   : channel no (0 or 1)
*              : uint8_t *buff     : buffer addrees to store reading datas
*              : uint32_t reg_addr : SDIP FIFO address
*              : int32_t num       : counts to read(unit:byte)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sddev_read_data(int32_t sd_port, uint8_t *buff, uint32_t reg_addr, int32_t num)
{
    int32_t  i;
    int32_t  cnt;
    uint64_t *reg;
    uint64_t *ptr_l;
    uint8_t  *ptr_c;
    volatile uint64_t tmp;

    reg = (uint64_t *)(reg_addr);

    cnt = (num / 8);
    if (((uint32_t)buff & 0x7uL) != 0uL)
    {
        ptr_c = (uint8_t *)buff;
        for (i = cnt; i > 0 ; i--)
        {
            tmp = *reg;
            *ptr_c++ = (uint8_t)(tmp);
            *ptr_c++ = (uint8_t)(tmp >> 8);
            *ptr_c++ = (uint8_t)(tmp >> 16);
            *ptr_c++ = (uint8_t)(tmp >> 24);
            *ptr_c++ = (uint8_t)(tmp >> 32);
            *ptr_c++ = (uint8_t)(tmp >> 40);
            *ptr_c++ = (uint8_t)(tmp >> 48);
            *ptr_c++ = (uint8_t)(tmp >> 56);
        }

        cnt = (num % 8);
        if ( cnt != 0 )
        {
            tmp = *reg;
            for (i = cnt; i > 0 ; i--)
            {
                *ptr_c++ = (uint8_t)(tmp);
                tmp >>= 8;
            }
        }
    }
    else
    {
        ptr_l = (uint64_t *)buff;
        for (i = cnt; i > 0 ; i--)
        {
            *ptr_l++ = *reg;
        }

        cnt = (num % 8);
        if ( cnt != 0 )
        {
            ptr_c = (uint8_t *)ptr_l;
            tmp = *reg;
            for (i = cnt; i > 0 ; i--)
            {
                *ptr_c++ = (uint8_t)(tmp);
                tmp >>= 8;
            }
        }
    }

    return SD_OK;
}

/******************************************************************************
* Function Name: int32_t sddev_write_data(int32_t sd_port, uint8_t *buff, uint32_t reg_addr, int32_t num);
* Description  : write to SDHI buffer FIFO
* Arguments    : int32_t sd_port   : channel no (0 or 1)
*              : uint8_t *buff     : buffer addrees to store writting datas
*              : uint32_t reg_addr : SDIP FIFO address
*              : int32_t num       : counts to write(unit:byte)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sddev_write_data(int32_t sd_port, uint8_t *buff, uint32_t reg_addr, int32_t num)
{
    int32_t  i;
    uint64_t *reg = (uint64_t *)(reg_addr);
    uint64_t *ptr = (uint64_t *)buff;
    uint64_t tmp;

    /* dont care non 8byte alignment data */
    num += 7;
    num /= 8;
    if (((uint32_t)buff & 0x7uL) != 0uL)
    {
        for (i = num; i > 0 ; i--)
        {
            tmp  = (uint64_t)(*buff++);
            tmp |= (uint64_t)(*buff++) << 8;
            tmp |= (uint64_t)(*buff++) << 16;
            tmp |= (uint64_t)(*buff++) << 24;
            tmp |= (uint64_t)(*buff++) << 32;
            tmp |= (uint64_t)(*buff++) << 40;
            tmp |= (uint64_t)(*buff++) << 48;
            tmp |= (uint64_t)(*buff++) << 56;
            *reg = tmp;
        }
    }
    else
    {
        for (i = num; i > 0 ; i--)
        {
            *reg = *ptr++;
        }
    }

    return SD_OK;
}

/******************************************************************************
* Function Name: uint32_t sddev_get_clockdiv(int32_t sd_port, int32_t clock);
* Description  : Get clock div value.
* Arguments    : int32_t sd_port : channel no (0 or 1)
*              : int32_t clock   : request clock frequency
*              :   SD_CLK_50MHz
*              :   SD_CLK_25MHz
*              :   SD_CLK_20MHz
*              :   SD_CLK_10MHz
*              :   SD_CLK_5MHz
*              :   SD_CLK_1MHz
*              :   SD_CLK_400kHz
* Return Value : clock div value
*              :   SD_DIV_4   : 1/4   clock
*              :   SD_DIV_8   : 1/8   clock
*              :   SD_DIV_16  : 1/16  clock
*              :   SD_DIV_32  : 1/32  clock
*              :   SD_DIV_256 : 1/256 clock
*              :   SD_DIV_512 : 1/512 clock
******************************************************************************/
uint32_t sddev_get_clockdiv(int32_t sd_port, int32_t clock)
{
    uint32_t div;

    switch (clock)
    {
    case SD_CLK_50MHz:
        div = SD_DIV_4;        /* 132MHz/4 = 33MHz      */
        break;
    case SD_CLK_25MHz:
        div = SD_DIV_8;        /* 132MHz/8 = 16.5MHz    */
        break;
    case SD_CLK_20MHz:
        div = SD_DIV_8;        /* 132MHz/8 = 16.5MHz    */
        break;
    case SD_CLK_10MHz:
        div = SD_DIV_16;       /* 132MHz/16 = 8.3MHz    */
        break;
    case SD_CLK_5MHz:
        div = SD_DIV_32;       /* 132MHz/32 = 4.1MHz    */
        break;
    case SD_CLK_1MHz:
        div = SD_DIV_256;      /* 132MHz/256 = 515.6kHz */
        break;
    case SD_CLK_400kHz:
        div = SD_DIV_512;      /* 132MHz/512 = 257.8kHz */
        break;
    default:
        div = SD_DIV_512;      /* 132MHz/512 = 257.8kHz */
        break;
    }

    return div;
}

/******************************************************************************
* Function Name: int32_t sddev_set_port(int32_t sd_port, int32_t mode);
* Description  : setting ports to use SDHI
* Arguments    : int32_t sd_port : channel no (0 or 1)
*              : int32_t mode    : SD_PORT_PARALLEL : 4bit mode
*                                : SD_PORT_SERIAL   : 1bit mode
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sddev_set_port(int32_t sd_port, int32_t mode)
{
    /* do nothing */

    return SD_OK;
}

/******************************************************************************
* Function Name: int32_t sddev_int_wait(int32_t sd_port, int32_t time);
* Description  : Waitting for SDHI Interrupt
* Arguments    : int32_t sd_port : channel no (0 or 1)
*              : int32_t time    : time out value to wait interrupt
* Return Value : get interrupt : SD_OK
*              : time out      : SD_ERR
******************************************************************************/
int32_t sddev_int_wait(int32_t sd_port, int32_t time)
{
#if(1) // mbed
    int32_t             ret;
#ifdef SDCFG_HWINT
    sdhi_info_dev_ch_t  *p_ch;

    ret  = SD_ERR;
    p_ch = sddev_get_dev_ch_instance(sd_port);

    if (p_ch != NULL) {
        if (osSemaphoreAcquire(p_ch->semaphore.sem_sync, time) == osOK) {
            ret = SD_OK;
        }
    }
#else /* #ifdef SDCFG_HWINT */
    uint32_t waittime = (uint32_t)time;

    /* interrupt generated? */
    ret = sd_check_int(sd_port);
    while ((ret == SD_ERR) && (waittime > 0uL)) {
        wait_ms(SDHI_1MSEC);
        waittime--;

        /* interrupt generated? */
        ret = sd_check_int(sd_port);
    }

#endif /* #ifdef SDCFG_HWINT */

    return ret;
#else
    int32_t             ret;
    uint32_t            waittime;

#ifdef SDCFG_HWINT
    bool_t              chk;
    sdhi_info_dev_ch_t  *p_ch;

    ret  = SD_ERR;
    p_ch = sddev_get_dev_ch_instance(sd_port);

    if (p_ch != NULL)
    {
        waittime = (uint32_t)time;
        chk = false;

        while ((chk == false) && (waittime > SDHI_MAX_WAITTIME))
        {
            chk = R_OS_SemaphoreWait(&p_ch->semaphore.sem_sync, SDHI_MAX_WAITTIME);
            waittime -= SDHI_MAX_WAITTIME;
        }
        if (chk == false)
        {
            chk = R_OS_SemaphoreWait(&p_ch->semaphore.sem_sync, waittime);
        }
        if (chk == true)
        {
            ret = SD_OK;
        }
    }

#else /* #ifdef SDCFG_HWINT */
    waittime = (uint32_t)time;

    /* interrupt generated? */
    ret = sd_check_int(sd_port);
    while ((ret == SD_ERR) && (waittime > 0uL))
    {
        R_OS_TaskSleep(SDHI_1MSEC);
        waittime--;

        /* interrupt generated? */
        ret = sd_check_int(sd_port);
    }

#endif /* #ifdef SDCFG_HWINT */

    return ret;
#endif
}

/******************************************************************************
* Function Name: int32_t sddev_init_dma(int32_t sd_port, uint32_t buff, int32_t dir);
* Description  : Initialize DMAC to transfer data from SDHI FIFO
* Arguments    : int32_t sd_port : channel no (0 or 1)
*              : uint32_t buff   : buffer addrees to transfer datas
*              : int32_t dir     : direction to transfer
*              :                 :   0 : FIFO -> buffer
*              :                 :   1 : buffer -> FIFO
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sddev_init_dma(int32_t sd_port, uint32_t buff, int32_t dir)
{
#ifdef    SDCFG_TRNS_DMA
    int32_t ret = SD_ERR;

    ret = sd_DMAC_PeriReqInit(sd_port, buff, dir);
    if (ret == SD_OK)
    {
        ret = sd_DMAC_Open(sd_port, dir);
    }
    return ret;

#else
    return SD_OK;

#endif
}

/******************************************************************************
* Function Name: int32_t sddev_wait_dma_end(int32_t sd_port, int32_t cnt);
* Description  : Wait to complete DMAC transfer
* Arguments    : int32_t sd_port : channel no (0 or 1)
*              : int32_t cnt     : counts to transfer(unit:byte)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sddev_wait_dma_end(int32_t sd_port, int32_t cnt)
{
#ifdef    SDCFG_TRNS_DMA
#if(1) // mbed
    int32_t             ret;
    uint32_t            waittime;

#ifdef SDCFG_HWINT
    sdhi_info_dev_ch_t  *p_ch;

    ret  = SD_ERR;
    p_ch = sddev_get_dev_ch_instance(sd_port);
    
    if (p_ch != NULL)
    {
        /* Caluculate timeout time (as 512kByte/sec)    */
        waittime = ((((uint32_t)cnt / SDHI_TO_SPEED) / SDHI_1024BYTE) * SDHI_1000MSEC);
        if (waittime < SDHI_1000MSEC)
        {
            waittime = SDHI_1000MSEC;
        }

        if (osSemaphoreAcquire(p_ch->semaphore.sem_dma, waittime) == osOK) {
            ret = SD_OK;
        }
    }

#else /* #ifdef SDCFG_HWINT */
    /* Caluculate timeout time (as 512kByte/sec)    */
    waittime = ((((uint32_t)cnt / SDHI_TO_SPEED) / SDHI_1024BYTE) * SDHI_1000MSEC);
    if (waittime < SDHI_1000MSEC)
    {
        waittime = SDHI_1000MSEC;
    }

    /* interrupt generated? */
    ret = sd_check_int_dm(sd_port);
    while ((ret == SD_ERR) && (waittime > 0uL))
    {
        wait_ms(SDHI_1MSEC);
        waittime--;

        /* interrupt generated? */
        ret = sd_check_int_dm(sd_port);
    }

#endif /* #ifdef SDCFG_HWINT */

    return ret;

#else
    int32_t             ret;
    uint32_t            waittime;

#ifdef SDCFG_HWINT
    bool_t              chk;
    sdhi_info_dev_ch_t  *p_ch;

    ret  = SD_ERR;
    p_ch = sddev_get_dev_ch_instance(sd_port);
    
    if (p_ch != NULL)
    {
        /* Caluculate timeout time (as 512kByte/sec)    */
        waittime = ((((uint32_t)cnt / SDHI_TO_SPEED) / SDHI_1024BYTE) * SDHI_1000MSEC);
        if (waittime < SDHI_1000MSEC)
        {
            waittime = SDHI_1000MSEC;
        }
        chk = false;

        while ((chk == false) && (waittime > SDHI_MAX_WAITTIME))
        {
            chk = R_OS_SemaphoreWait(&p_ch->semaphore.sem_dma, SDHI_MAX_WAITTIME);
            waittime -= SDHI_MAX_WAITTIME;
        }
        if (chk == false)
        {
            chk = R_OS_SemaphoreWait(&p_ch->semaphore.sem_dma, waittime);
        }
        if (chk == true)
        {
            ret = SD_OK;
        }
    }

#else /* #ifdef SDCFG_HWINT */
    /* Caluculate timeout time (as 512kByte/sec)    */
    waittime = ((((uint32_t)cnt / SDHI_TO_SPEED) / SDHI_1024BYTE) * SDHI_1000MSEC);
    if (waittime < SDHI_1000MSEC)
    {
        waittime = SDHI_1000MSEC;
    }

    /* interrupt generated? */
    ret = sd_check_int_dm(sd_port);
    while ((ret == SD_ERR) && (waittime > 0uL))
    {
        R_OS_TaskSleep(SDHI_1MSEC);
        waittime--;

        /* interrupt generated? */
        ret = sd_check_int_dm(sd_port);
    }

#endif /* #ifdef SDCFG_HWINT */

    return ret;

#endif
#else
    return SD_OK;

#endif
}

/******************************************************************************
* Function Name: int32_t sddev_disable_dma(int32_t sd_port);
* Description  : Disable DMAC transfer
* Arguments    : int32_t sd_port : channel no (0 or 1)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sddev_disable_dma(int32_t sd_port)
{
#ifdef SDCFG_TRNS_DMA
    int32_t ret;

    ret = sd_DMAC_Close(sd_port);

    return ret;

#else
    return SD_OK;

#endif
}

/******************************************************************************
* Function Name: int32_t sddev_reset_dma(int32_t sd_port);
* Description  : Reset of the SDHI module built-in DMAC.
* Arguments    : int32_t sd_port : channel no (0 or 1)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sddev_reset_dma(int32_t sd_port)
{
    int32_t ret;

    ret = sd_DMAC_Reset(sd_port);
    if (ret == SD_OK)
    {
        ret = sd_DMAC_Released(sd_port);
    }
    return ret;
}

/******************************************************************************
* Function Name: int32_t sddev_finalize_dma(int32_t sd_port);
* Description  : Finalize of the SDHI module built-in DMAC.
* Arguments    : int32_t sd_port : channel no (0 or 1)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sddev_finalize_dma(int32_t sd_port)
{
    int32_t ret;

    ret = sd_DMAC_Reset(sd_port);

    return ret;
}

/******************************************************************************
* Function Name: int32_t sddev_loc_cpu(int32_t sd_port);
* Description  : lock cpu to disable interrupt
* Arguments    : int32_t sd_port : channel no (0 or 1)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sddev_loc_cpu(int32_t sd_port)
{
#if 0
    R_INTC_GetMaskLevel(&g_sdhi_priority_backup);
    R_INTC_SetMaskLevel(0);
#endif

    return SD_OK;
}

/******************************************************************************
* Function Name: int32_t sddev_unl_cpu(int32_t sd_port);
* Description  : unlock cpu to enable interrupt
* Arguments    : int32_t sd_port : channel no (0 or 1)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sddev_unl_cpu(int32_t sd_port)
{
#if 0
    R_INTC_SetMaskLevel(g_sdhi_priority_backup);
#endif
    return SD_OK;
}

/******************************************************************************
* Function Name: int32_t sddev_finalize(int32_t sd_port);
* Description  : finalize SDHI
* Arguments    : int32_t sd_port : channel no (0 or 1)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sddev_finalize(int32_t sd_port)
{
#if(1) // mbed
    int32_t             ret;
    sdhi_info_dev_ch_t  *p_ch;
    volatile uint8_t    reg_read_8;

    ret  = SD_OK;
    p_ch = sddev_get_dev_ch_instance(sd_port);

    if (p_ch != NULL) {
#if(1)
        core_util_critical_section_enter();
        if (sd_port == 0) {
            while (1) {
                CPG.STBREQ1.BYTE |= 0x04;
                reg_read_8 = CPG.STBREQ1.BYTE; /* dummy read */
                reg_read_8 = CPG.STBACK1.BYTE;
                if ((reg_read_8 & 0x04) != 0) {
                    break;
                }
            }
            CPG.STBCR10.BYTE |= 0x0C;
            reg_read_8 = CPG.STBCR10.BYTE; /* dummy read */
        } else {
            while (1) {
                CPG.STBREQ1.BYTE |= 0x02;
                reg_read_8 = CPG.STBREQ1.BYTE; /* dummy read */
                reg_read_8 = CPG.STBACK1.BYTE;
                if ((reg_read_8 & 0x02) != 0) {
                    break;
                }
            }
            CPG.STBCR10.BYTE |= 0x03u;
            reg_read_8 = CPG.STBCR10.BYTE; /* dummy read */
        }
        core_util_critical_section_exit();
#endif

#ifdef SDCFG_HWINT
        /* --- Invalidate SDHI interrupt --- */
        (void)IRQ_Disable(p_ch->intc.int_id);

        /* --- Unregister SDHI interrupt handler --- */
        (void)IRQ_SetHandler(p_ch->intc.int_id, NULL);

        /* --- Delete semaphore --- */
        if (p_ch->semaphore.sem_sync != NULL) {
            osSemaphoreDelete(p_ch->semaphore.sem_sync);
            p_ch->semaphore.sem_sync = NULL;
        }
        if (p_ch->semaphore.sem_dma != NULL) {
            osSemaphoreDelete(p_ch->semaphore.sem_dma);
            p_ch->semaphore.sem_dma = NULL;
        }
#endif
    }
    else /* if (p_ch != NULL) */
    {
        ret = SD_ERR;
    }
    return ret;
#else
    e_stb_err_t         e_stb_err;
    int32_t             ret;
    uint32_t            was_masked;
    sdhi_info_dev_ch_t  *p_ch;
#ifdef SDCFG_HWINT
    e_r_drv_intc_err_t  e_intc_err;
#endif /* #ifdef SDCFG_HWINT */

    ret  = SD_OK;
    p_ch = sddev_get_dev_ch_instance(sd_port);

    if (p_ch != NULL)
    {
        was_masked = __disable_irq();

        /* No timer monitering. */
        e_stb_err = STB_AGAIN;
        while (e_stb_err == STB_AGAIN)
        {
            /* [Transition to Module Standby Function]
               1. Set the corresponding bit in the STBREQ register to 1 to
                  generate a request to stop the module.
               2. Confirm that the module is ready to be stopped by the
                  corresponding bit in the STBACK register being set to 1.  */
            e_stb_err = R_STB_RequestModuleStop(p_ch->stb.stb_ch);
        }
        if (e_stb_err == STB_SUCCESS)
        {
            /* 3. Set the MSTP bit of the corresponding module to 1. */
            e_stb_err = R_STB_StopModule(p_ch->stb.stb_ch);
        }
        else
        {
            ret = SD_ERR;
        }

        if (was_masked == 0uL)
        {
            /* enable all irq */
            __enable_irq();
        }
        
#ifdef SDCFG_HWINT
        /* --- Invalidate SDHI interrupt --- */
        e_intc_err = R_INTC_Disable(p_ch->intc.int_id);
        if (e_intc_err != INTC_SUCCESS)
        {
            /* invalid id */
            ret = SD_ERR;
        }
        /* --- Unregister SDHI interrupt handler --- */
        e_intc_err = R_INTC_RegistIntFunc(p_ch->intc.int_id, NULL);
        if (e_intc_err != INTC_SUCCESS)
        {
            /* invalid id */
            ret = SD_ERR;
        }

        /* --- Delete semaphore --- */
        R_OS_SemaphoreDelete(&p_ch->semaphore.sem_sync);      /* p_ch->semaphore.sem_sync = 0 */
        R_OS_SemaphoreDelete(&p_ch->semaphore.sem_dma);       /* p_ch->semaphore.sem_dma  = 0 */
#endif
    }
    else /* if (p_ch != NULL) */
    {
        ret = SD_ERR;
    }
    return ret;
#endif
}

/******************************************************************************
* Function Name: static void sddev_sd_int_handler_0(uint32_t int_sense);
* Description  : Setting Interrupt function for SDHI(INTC_ID_SDMMC_SDHI0_0)
* Arguments    : uint32_t int_sense : Interrupt mode
* Return Value : none
******************************************************************************/
#if(1) // mbed
static void sddev_sd_int_handler_0(void)
#else
static void sddev_sd_int_handler_0(uint32_t int_sense)
#endif
{
    sd_int_handler(0);
    sd_int_dm_handler(0);
}

/******************************************************************************
* Function Name: static void sddev_sd_int_handler_1(uint32_t int_sense);
* Description  : Setting Interrupt function for SDHI(INTC_ID_SDMMC_SDHI1_0)
* Arguments    : uint32_t int_sense : Interrupt mode
* Return Value : none
******************************************************************************/
#if(1) // mbed
static void sddev_sd_int_handler_1(void)
#else
static void sddev_sd_int_handler_1(uint32_t int_sense)
#endif
{
    sd_int_handler(1);
    sd_int_dm_handler(1);
}

/******************************************************************************
* Function Name: int32_t sddev_cd_layout(int32_t sd_port)
* Description  : CD Terminal Support Confirmation
* Arguments    : int32_t sd_port : channel no (0 or 1)
* Return Value : Support     : SD_OK
*              : Not Support : SD_ERR
******************************************************************************/
int32_t sddev_cd_layout(int32_t sd_port)
{
    int32_t ret;
    int32_t sc_port;
    
    ret = sddev_get_sc_table_config_ch(sd_port, &sc_port);
    if (ret == SD_OK)
    {
        if (SDHI_SC_TABLE[sc_port].config.cd != SD_CD_ENABLED)
        {
            ret = SD_ERR;
        }
    }
    return ret;
}

/******************************************************************************
* Function Name: int32_t sddev_wp_layout(int32_t sd_port)
* Description  : WP Terminal Support Confirmation
* Arguments    : int32_t sd_port : channel no (0 or 1)
* Return Value : Support     : SD_OK
*              : Not Support : SD_ERR
******************************************************************************/
int32_t sddev_wp_layout(int32_t sd_port)
{
    int32_t ret;
    int32_t sc_port;

    ret = sddev_get_sc_table_config_ch(sd_port, &sc_port);
    if (ret == SD_OK)
    {
        if (SDHI_SC_TABLE[sc_port].config.wp != SD_WP_ENABLED)
        {
            ret = SD_ERR;
        }
    }
    return ret;
}

/******************************************************************************
* Function Name: static int32_t sddev_get_sc_table_config_ch(int32_t sd_port, int32_t *p_sc_port)
* Description  : Get the channel number of the table generated by the smart
*              : configurator.
* Arguments    : int32_t sd_port    : channel no (0 or 1)
*              : int32_t *p_sc_port : pointer to sc channel no
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
static int32_t sddev_get_sc_table_config_ch(int32_t sd_port, int32_t *p_sc_port)
{
    int32_t ret;
    int32_t ch_num;
    int32_t cnt;

    ret = SD_ERR;
    if (p_sc_port != NULL)
    {
        ch_num = sizeof(SDHI_SC_TABLE)/sizeof(st_r_drv_sdhi_sc_config_t);

        for (cnt = 0; cnt < ch_num; cnt++)
        {
            if (sd_port == SDHI_SC_TABLE[cnt].channel)
            {
                ret = SD_OK;
                *p_sc_port = cnt;
                break;
            }
        }
    }
    return ret;
}

/******************************************************************************
* Function Name: sdhi_info_dev_ch_t *sddev_get_dev_ch_instance(int32_t sd_port)
* Description  : Get SDHI driver target CPU interface information object
* Arguments    : int32_t sd_port : channel no (0 or 1)
* Return Value : success : pointer to SDHI driver target CPU interface information
*              : fail    : NULL
******************************************************************************/
sdhi_info_dev_ch_t *sddev_get_dev_ch_instance(int32_t sd_port)
{
    sdhi_info_dev_ch_t *p_info;

    if ((sd_port >= SDHI_CH_0)
     && (sd_port <  SDHI_CH_NUM))
    {
        p_info = &g_sdhi_dev_ch[sd_port];
    }
    else
    {
        p_info = NULL;
        /* NON_NOTICE_ASSERT: channel range over */
    }
    return p_info;
}

/******************************************************************************
* Function Name: int32_t SD_status_callback_function(int32_t sd_port, int32_t cd)
* Description  : Callback interrupt function for SDHI protocol status control
* Arguments    : int32_t sd_port : channel no (0 or 1)
*              : int32_t cd      : no used
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t SD_status_callback_function(int32_t sd_port, int32_t cd)
{
#ifdef SDCFG_HWINT
    int32_t             ret;
    sdhi_info_dev_ch_t  *p_ch;

    ret  = SD_ERR;
    p_ch = sddev_get_dev_ch_instance(sd_port);
    if (p_ch != NULL)
    {
#if(1) // mbed
        (void)osSemaphoreRelease(p_ch->semaphore.sem_sync);
#else
        R_OS_SemaphoreRelease(&p_ch->semaphore.sem_sync);
#endif
        ret = SD_OK;
    }
    return ret;

#else /* #ifdef SDCFG_HWINT */
    return SD_OK;

#endif /* #ifdef SDCFG_HWINT */
}

/******************************************************************************
* Function Name: int32_t SD_dma_end_callback_function(int32_t sd_port, int32_t cd)
* Description  : Callback interrupt function for complete DMA transfer control
* Arguments    : int32_t sd_port : channel no (0 or 1)
*              : int32_t cd      : no used
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t SD_dma_end_callback_function(int32_t sd_port, int32_t cd)
{
#ifdef SDCFG_HWINT
    int32_t             ret;
    sdhi_info_dev_ch_t  *p_ch;

    ret  = SD_ERR;
    p_ch = sddev_get_dev_ch_instance(sd_port);
    if (p_ch != NULL)
    {
#if(1) // mbed
        (void)osSemaphoreRelease(p_ch->semaphore.sem_dma);
#else
        R_OS_SemaphoreRelease(&p_ch->semaphore.sem_dma);
#endif
        ret = SD_OK;
    }
    return ret;

#else /* #ifdef SDCFG_HWINT */
    return SD_OK;

#endif /* #ifdef SDCFG_HWINT */
}

/******************************************************************************
* Function Name: void SD_confirm_semaphore(int32_t sd_port)
* Description  : Confirm semaphore status
* Arguments    : int32_t sd_port : channel no (0 or 1)
* Return Value : none
******************************************************************************/
void SD_confirm_semaphore(int32_t sd_port)
{
#ifdef SDCFG_HWINT
#if(1) // mbed
    sdhi_info_dev_ch_t  *p_ch;

    p_ch = sddev_get_dev_ch_instance(sd_port);

    if (p_ch != NULL) {
        while (osSemaphoreAcquire(p_ch->semaphore.sem_sync, 0) == osOK);
    }
#else
    bool_t              chk;
    sdhi_info_dev_ch_t  *p_ch;

    chk  = true;
    p_ch = sddev_get_dev_ch_instance(sd_port);

    if (p_ch != NULL)
    {
        while (chk == true)
        {
            chk = R_OS_SemaphoreWait(&p_ch->semaphore.sem_sync, 0uL);
        }
    }
#endif

#else /* #ifdef SDCFG_HWINT */
    /* do nothing */

#endif /* #ifdef SDCFG_HWINT */
}

/* End of File */
