/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/******************************************************************************
* System Name  : SDHI Driver
* File Name    : sd_dev_low.c
* Version      : 1.20
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RZ/A2M Evaluation Board
* Description  : RZ/A2M SD Driver Sample Program
* Operation    : 
* Limitations  : Ch0 and Ch1 can't be used at the same time, because the timers
*              : used on Ch0 and Ch1 are common.
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 16.03.2018 1.00    First Release
*         : 14.12.2018 1.01    Changed the DMAC soft reset procedure.
*         : 28.12.2018 1.02    Support for OS
*         : 29.05.2019 1.20    Correspond to internal coding rules
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
#define INT_PRV_LEVEL_SDHI      (26u)       /* SDHI interrupt level  */

#define SDHI_PRV_CH_0           (0)
#define SDHI_PRV_CH_1           (1)
#define SDHI_PRV_CH_NUM         (2)

#define SDHI_PRV_PPOC_POC_3_3V  (1u)

#define SDHI_PRV_CLK_TDSEL_3_3V (3u)

#define SDHI_PRV_CARDDET_TIME   (1000uL)
#define SDHI_PRV_POWERON_TIME   (100uL)

#ifdef SD_CFG_HWINT
#define SDHI_PRV_MAX_WAITTIME   (0xFFFFuL)

#else /* #ifdef SD_CFG_HWINT */
#define SDHI_PRV_1MSEC          (1uL)

#endif /* #ifdef SD_CFG_HWINT */

#define SDHI_PRV_TO_SPEED       (512uL)
#define SDHI_PRV_1024BYTE       (1024uL)
#define SDHI_PRV_1000MSEC       (1000uL)

#if(1) // mbed
#define     UNUSED_PARAM(param)             (void)(param)
#endif
/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
/* static uint8_t g_sdhi_priority_backup; */

static int32_t sddev_init_voltage(int32_t sd_port);
#if(1) // mbed
static void sddev_sd_int_handler_0(void);
static void sddev_sd_int_handler_1(void);
osSemaphoreDef(sdint_sem_sync);
osSemaphoreDef(sdint_sem_dma);
#else
static void sddev_sd_int_handler_0(uint32_t int_sense);
static void sddev_sd_int_handler_1(uint32_t int_sense);
#endif

#if(1) // mbed
static st_sdhi_info_dev_ch_t s_sdhi_dev_ch[SDHI_PRV_CH_NUM] =
{
    /*--- ch 0 ---*/
    {
        { false                                                             }, /* stb  */
        { SDHI0_0_IRQn,          INT_PRV_LEVEL_SDHI, sddev_sd_int_handler_0 }, /* intc */
        { false,                 false                                      }, /* gpio */
#ifdef SD_CFG_HWINT
        { 0uL,                   0uL                                        }  /* semaphore */
#endif /* #ifdef SD_CFG_HWINT */
    },

    /*--- ch 1 ---*/
    {
        { false                                                             }, /* stb  */
        { SDHI1_0_IRQn,          INT_PRV_LEVEL_SDHI, sddev_sd_int_handler_1 }, /* intc */
        { false,                 false                                      }, /* gpio */
#ifdef SD_CFG_HWINT
        { 0uL,                   0uL                                        }  /* semaphore */
#endif /* #ifdef SD_CFG_HWINT */
    }
};
#else
static st_sdhi_info_dev_ch_t s_sdhi_dev_ch[SDHI_PRV_CH_NUM] =
{
    /*--- ch 0 ---*/
    {
        { false,                 MODULE_SDMMC0                              }, /* stb  */
        { INTC_ID_SDMMC_SDHI0_0, INT_PRV_LEVEL_SDHI, sddev_sd_int_handler_0 }, /* intc */
        { false,                 false                                      }, /* gpio */
#ifdef SD_CFG_HWINT
        { 0uL,                   0uL                                        }  /* semaphore */
#endif /* #ifdef SD_CFG_HWINT */
    },

    /*--- ch 1 ---*/
    {
        { false,                 MODULE_SDMMC1                              }, /* stb  */
        { INTC_ID_SDMMC_SDHI1_0, INT_PRV_LEVEL_SDHI, sddev_sd_int_handler_1 }, /* intc */
        { false,                 false                                      }, /* gpio */
#ifdef SD_CFG_HWINT
        { 0uL,                   0uL                                        }  /* semaphore */
#endif /* #ifdef SD_CFG_HWINT */
    }
};
#endif

#if(1) // mbed
#else
/*--- PD1 voltage ---*/
static const r_gpio_port_pin_t s_sd_init_ch0_voltage_pin_list[] =
{
    /* Cast to an appropriate type */
    GPIO_PORT_D_PIN_1,
};

static st_r_drv_gpio_pin_list_t s_sd_init_ch0_voltage =
{
    &s_sd_init_ch0_voltage_pin_list[0],                                 /* p_pin_list   */
    (sizeof(s_sd_init_ch0_voltage_pin_list))/sizeof(r_gpio_port_pin_t), /* count        */
    GPIO_SUCCESS                                                        /* err(r/w)     */
};

/* Cast to an appropriate type */
static st_r_drv_gpio_pin_rw_t s_sd_ch0_voltage_3_3 = { GPIO_PORT_D_PIN_1, GPIO_LEVEL_HIGH, GPIO_SUCCESS };

/* SD current */
static const st_r_drv_gpio_sc_config_t s_sd_gpio_current_ch0[] =
{
/*  { pin,               { function(no used),    tint(no used),     current           } } */
    
    /* Cast to an appropriate type */
    { GPIO_PIN_SD0_CLK,  { GPIO_FUNC_SC_DEFAULT, GPIO_TINT_DISABLE, GPIO_CURRENT_12mA } },

    /* Cast to an appropriate type */
    { GPIO_PIN_SD0_CMD,  { GPIO_FUNC_SC_DEFAULT, GPIO_TINT_DISABLE, GPIO_CURRENT_6mA  } },

    /* Cast to an appropriate type */
    { GPIO_PIN_SD0_DAT0, { GPIO_FUNC_SC_DEFAULT, GPIO_TINT_DISABLE, GPIO_CURRENT_6mA  } },

    /* Cast to an appropriate type */
    { GPIO_PIN_SD0_DAT1, { GPIO_FUNC_SC_DEFAULT, GPIO_TINT_DISABLE, GPIO_CURRENT_6mA  } },

    /* Cast to an appropriate type */
    { GPIO_PIN_SD0_DAT2, { GPIO_FUNC_SC_DEFAULT, GPIO_TINT_DISABLE, GPIO_CURRENT_6mA  } },

    /* Cast to an appropriate type */
    { GPIO_PIN_SD0_DAT3, { GPIO_FUNC_SC_DEFAULT, GPIO_TINT_DISABLE, GPIO_CURRENT_6mA  } },

    /* Cast to an appropriate type */
    { GPIO_PIN_SD0_DAT4, { GPIO_FUNC_SC_DEFAULT, GPIO_TINT_DISABLE, GPIO_CURRENT_6mA  } },

    /* Cast to an appropriate type */
    { GPIO_PIN_SD0_DAT5, { GPIO_FUNC_SC_DEFAULT, GPIO_TINT_DISABLE, GPIO_CURRENT_6mA  } },

    /* Cast to an appropriate type */
    { GPIO_PIN_SD0_DAT6, { GPIO_FUNC_SC_DEFAULT, GPIO_TINT_DISABLE, GPIO_CURRENT_6mA  } },

    /* Cast to an appropriate type */
    { GPIO_PIN_SD0_DAT7, { GPIO_FUNC_SC_DEFAULT, GPIO_TINT_DISABLE, GPIO_CURRENT_6mA  } },

    /* Cast to an appropriate type */
    { GPIO_PIN_SD0_RST,  { GPIO_FUNC_SC_DEFAULT, GPIO_TINT_DISABLE, GPIO_CURRENT_6mA  } }
};

static const st_r_drv_gpio_sc_config_t s_sd_gpio_current_ch1[] =
{
/*  { pin,               { function(no used),    tint(no used),     current           } } */

    /* Cast to an appropriate type */
    { GPIO_PIN_SD1_CLK,  { GPIO_FUNC_SC_DEFAULT, GPIO_TINT_DISABLE, GPIO_CURRENT_12mA } },

    /* Cast to an appropriate type */
    { GPIO_PIN_SD1_CMD,  { GPIO_FUNC_SC_DEFAULT, GPIO_TINT_DISABLE, GPIO_CURRENT_6mA  } },

    /* Cast to an appropriate type */
    { GPIO_PIN_SD1_DAT0, { GPIO_FUNC_SC_DEFAULT, GPIO_TINT_DISABLE, GPIO_CURRENT_6mA  } },

    /* Cast to an appropriate type */
    { GPIO_PIN_SD1_DAT1, { GPIO_FUNC_SC_DEFAULT, GPIO_TINT_DISABLE, GPIO_CURRENT_6mA  } },

    /* Cast to an appropriate type */
    { GPIO_PIN_SD1_DAT2, { GPIO_FUNC_SC_DEFAULT, GPIO_TINT_DISABLE, GPIO_CURRENT_6mA  } },

    /* Cast to an appropriate type */
    { GPIO_PIN_SD1_DAT3, { GPIO_FUNC_SC_DEFAULT, GPIO_TINT_DISABLE, GPIO_CURRENT_6mA  } },
};

static st_r_drv_gpio_pin_init_table_t s_sd_gpio_current[] =
{
/*    p_config_table,          count,                                                           err(r/w) */
    { &s_sd_gpio_current_ch0[0], ((sizeof(s_sd_gpio_current_ch0))/sizeof(st_r_drv_gpio_sc_config_t)), GPIO_SUCCESS },
    { &s_sd_gpio_current_ch1[0], ((sizeof(s_sd_gpio_current_ch1))/sizeof(st_r_drv_gpio_sc_config_t)), GPIO_SUCCESS }
};
#endif

/******************************************************************************
* Function Name: sddev_init
* Description  : Initialize H/W to use SDHI
* Arguments    : int32_t sd_port : channel no (0 or 1)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sddev_init(int32_t sd_port)
{
#if(1) // mbed
    int32_t               ret;
    st_sdhi_info_dev_ch_t *p_ch;

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

#ifdef SD_CFG_HWINT
        if (ret == SD_OK) {
            /* ---- Register SDHI interrupt handler ---- */
            (void)IRQ_SetHandler(p_ch->intc.int_id, p_ch->intc.p_func);
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
#endif /* #ifdef SD_CFG_HWINT */
    } else { /* if (p_ch != NULL) */
        ret = SD_ERR;
    }
    return ret;
#else
    int32_t               ret;
    st_sdhi_info_dev_ch_t *p_ch;
    uint32_t              was_masked;
    e_stb_err_t           e_stb_err;
    int_t                 gpio_handle;
    int_t                 gpio_err;
#ifdef SD_CFG_HWINT
    e_r_drv_intc_err_t    e_intc_err;
    bool_t                chk;
#endif /* #ifdef SD_CFG_HWINT */
    int32_t               sc_port;

    ret  = SD_OK;
    p_ch = sddev_get_dev_ch_instance(sd_port);

    /* Cast to an appropriate type */
    if (NULL != p_ch)
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
        if (STB_SUCCESS == e_stb_err)
        {
            if (false == p_ch->stb.stb_pon_init)
            {
                /* case a */
                p_ch->stb.stb_pon_init = true;
            }
            else
            {
                /* case b */
                /* No timer monitering. */
                e_stb_err = STB_AGAIN;
                while (STB_AGAIN == e_stb_err)
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
        if (STB_SUCCESS != e_stb_err)
        {
            ret = SD_ERR;
        }

        /* --- SDHI, clear module standby end ---*/

        /* --- SDx_CD, SDx_WP(x = 0(ch0) or 1(ch1)) start --- */
        if (SD_OK == ret)
        {
            if (false == p_ch->gpio.gpio_init)
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
                    if (SD_OK == ret)
                    {
                        gpio_err = direct_control(gpio_handle,

                                                /* Cast to an appropriate type */
                                                (uint32_t)CTL_GPIO_INIT_BY_TABLE,

                                                /* Cast to an appropriate type */
                                                (st_r_drv_sdhi_sc_config_t *)&SDHI_SC_TABLE[sc_port].pin);
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
            } /* if (false == p_ch->gpio.gpio_init) */
        }

        /* --- SDx_CD, SDx_WP(x = 0(ch0) or 1(ch1)) end --- */

        if (SD_OK == ret)
        {
            ret = sddev_init_voltage(sd_port);  /* Initialize voltage 3.3V */
        }

        if (0uL == was_masked)
        {
            __enable_irq();
        }

#ifdef SD_CFG_HWINT
        if (SD_OK == ret)
        {
            /* ---- Register SDHI interrupt handler start ---- */
            e_intc_err = R_INTC_RegistIntFunc(p_ch->intc.int_id, p_ch->intc.p_func);
            if (INTC_SUCCESS != e_intc_err)
            {
                /* Invalid id */
                ret = SD_ERR;
            }
            else
            {
                /* ---- Set priority of SDHI interrupt handler to INT_PRV_LEVEL_SDHI ---- */
                e_intc_err = R_INTC_SetPriority(p_ch->intc.int_id, p_ch->intc.int_priority);
                if (INTC_SUCCESS != e_intc_err)
                {
                    /* Invalid id or invalid priority */
                    ret = SD_ERR;
                }
                else
                {
                    /* ---- Validate SDHI interrupt ---- */
                    e_intc_err = R_INTC_Enable(p_ch->intc.int_id);
                    if (INTC_SUCCESS != e_intc_err)
                    {
                        /* Invalid id */
                        ret = SD_ERR;
                    }
                }
            }

            /* ---- Register SDHI interrupt handler end ---- */

            if (SD_ERR == ret)
            {
                /* --- Invalidate SDHI interrupt --- */
                e_intc_err = R_INTC_Disable(p_ch->intc.int_id);
                if (INTC_SUCCESS != e_intc_err)
                {
                    /* DO NOTHING */
                    ;
                }

                /* --- Unregister SDHI interrupt handler --- *//* Cast to an appropriate type */
                e_intc_err = R_INTC_RegistIntFunc(p_ch->intc.int_id, NULL);
                if (INTC_SUCCESS != e_intc_err)
                {
                    /* DO NOTHING */
                    ;
                }
            }
        }

        if (SD_OK == ret)
        {
            /* --- Create semaphore start --- */
            if (0uL == p_ch->semaphore.sem_sync)
            {
                chk = R_OS_SemaphoreCreate(&p_ch->semaphore.sem_sync, 1uL);
                if (true == chk)
                {
                    chk = R_OS_SemaphoreWait(&p_ch->semaphore.sem_sync, 0uL);
                }
                if (false == chk)
                {
                    ret = SD_ERR;
                }
            }
            if (0uL == p_ch->semaphore.sem_dma)
            {
                chk = R_OS_SemaphoreCreate(&p_ch->semaphore.sem_dma, 1uL);
                if (true == chk)
                {
                    chk = R_OS_SemaphoreWait(&p_ch->semaphore.sem_dma, 0uL);
                }
                if (false == chk)
                {
                    ret = SD_ERR;
                }
            }

            /* --- Create semaphore end --- */

            if (SD_ERR == ret)
            {
                /* --- Delete semaphore --- */
                R_OS_SemaphoreDelete(&p_ch->semaphore.sem_sync);      /* p_ch->semaphore.sem_sync = 0 */
                R_OS_SemaphoreDelete(&p_ch->semaphore.sem_dma);       /* p_ch->semaphore.sem_dma  = 0 */
            }
        }
#endif /* #ifdef SD_CFG_HWINT */

        if (SD_OK == ret)
        {
            /* ---- wait card detect ---- */
            R_OS_TaskSleep(SDHI_PRV_CARDDET_TIME);
        }
    }
    else /* if (NULL != p_ch) */
    {
        ret = SD_ERR;
    }
    return ret;
#endif
}
/*******************************************************************************
 End of function sddev_init
 ******************************************************************************/

#if(1) // mbed
#else
/******************************************************************************
* Function Name: sddev_init_voltage
* Description  : SDHI initialize voltage(sd_port = 0,1)
* Arguments    : int32_t sd_port : channel no (0 or 1)
* Return Value : success : SD_OK
******************************************************************************/
static int32_t sddev_init_voltage(int32_t sd_port)
{
    int32_t ret;
    int_t   gpio_handle;
    int_t   gpio_err;
    st_sdhi_info_dev_ch_t *p_ch;

    ret = SD_OK;
    p_ch = sddev_get_dev_ch_instance(sd_port);

    /* Cast to an appropriate type */
    if (NULL != p_ch)
    {
        /* --- Open GPIO --- */
        gpio_handle = direct_open("gpio", 0);
        if (gpio_handle < 0)
        {
            ret = SD_ERR;
        }
        else /* if (gpio_handle < 0) */
        {
            if(SDHI_PRV_CH_0 == sd_port)
            {
                if (false == p_ch->gpio.gpio_vol_init)
                {
                    /*--- Control GPIO (PD1 = High(3.3V)) ---*//* Cast to an appropriate type */
                    gpio_err = direct_control(gpio_handle, (uint32_t)CTL_GPIO_INIT_BY_PIN_LIST,
                                                &s_sd_init_ch0_voltage);
                    if (gpio_err < 0)
                    {
                        ret = SD_ERR;
                    }
                    else
                    {
                        p_ch->gpio.gpio_vol_init = true;
                    }
                }
                else
                {
                    /*--- Control GPIO (PD1 = High(3.3V)) ---*//* Cast to an appropriate type */
                    gpio_err = direct_control(gpio_handle, (uint32_t)CTL_GPIO_PIN_WRITE, &s_sd_ch0_voltage_3_3);
                    if (gpio_err < 0)
                    {
                        ret = SD_ERR;
                    }
                }
                if (SD_OK == ret)
                {
                    /* GPIO.PPOC.BIT.POC2 = 1(3.3V) */
                    RZA_IO_RegWrite_32((volatile uint32_t*)&GPIO.PPOC.LONG,
                                        SDHI_PRV_PPOC_POC_3_3V,
                                        GPIO_PPOC_POC2_SHIFT,
                                        GPIO_PPOC_POC2);

                    /* GPIO.PSDMMC0.BIT.SD0_CLK_TDSEL = 3(3.3V) */
                    RZA_IO_RegWrite_32((volatile uint32_t*)&GPIO.PSDMMC0.LONG,
                                        SDHI_PRV_CLK_TDSEL_3_3V,
                                        GPIO_PSDMMC0_SD0_CLK_TDSEL_SHIFT,
                                        GPIO_PSDMMC0_SD0_CLK_TDSEL);

                    /*--- Control GPIO ---*/
                    gpio_err = direct_control(gpio_handle, (uint32_t)CTL_GPIO_INIT_BY_TABLE,
                                                &s_sd_gpio_current[sd_port]);
                    if (gpio_err < 0)
                    {
                        ret = SD_ERR;
                    }
                }
            }
            else /* if(SDHI_PRV_CH_0 == sd_port) */
            {
                /* GPIO.PPOC.BIT.POC3 = 1(3.3V) */
                RZA_IO_RegWrite_32((volatile uint32_t*)&GPIO.PPOC.LONG,
                                    SDHI_PRV_PPOC_POC_3_3V,
                                    GPIO_PPOC_POC3_SHIFT,
                                    GPIO_PPOC_POC3);

                /* GPIO.PSDMMC2.BIT.SD1_CLK_TDSEL = 3(3.3V) */
                RZA_IO_RegWrite_32((volatile uint32_t*)&GPIO.PSDMMC2.LONG,
                                    SDHI_PRV_CLK_TDSEL_3_3V,
                                    GPIO_PSDMMC2_SD1_CLK_TDSEL_SHIFT,
                                    GPIO_PSDMMC2_SD1_CLK_TDSEL);

                /*--- Control GPIO ---*/
                gpio_err = direct_control(gpio_handle, (uint32_t)CTL_GPIO_INIT_BY_TABLE, &s_sd_gpio_current[sd_port]);
                if ( gpio_err < 0 )
                {
                    ret = SD_ERR;
                }
            } /* if(SDHI_PRV_CH_0 == sd_port) */

            /* --- Close GPIO --- */
            (void)direct_close(gpio_handle);

        } /* if (gpio_handle < 0) */
    }
    else /* if (NULL != p_ch) */
    {
        ret = SD_ERR;
    }

    return ret;
}
/*******************************************************************************
 End of function sddev_init_voltage
 ******************************************************************************/
#endif

/******************************************************************************
* Function Name: sddev_power_on
* Description  : Power-on H/W to use SDHI
* Arguments    : int32_t sd_port : channel no (0 or 1)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sddev_power_on(int32_t sd_port)
{
    /* Cast to an appropriate type */
    UNUSED_PARAM(sd_port);

    /* ---Power On SD ---- */

    /* ---- Wait for  SD Wake up ---- */
#if(1) // mbed
    osDelay(SDHI_PRV_POWERON_TIME);
#else
    R_OS_TaskSleep(SDHI_PRV_POWERON_TIME);
#endif

    return SD_OK;
}
/*******************************************************************************
 End of function sddev_power_on
 ******************************************************************************/

/******************************************************************************
* Function Name: sddev_power_off
* Description  : Power-off H/W to use SDHI
* Arguments    : int32_t sd_port : channel no (0 or 1)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sddev_power_off(int32_t sd_port)
{
    /* Cast to an appropriate type */
    UNUSED_PARAM(sd_port);

    return SD_OK;
}
/*******************************************************************************
 End of function sddev_power_off
 ******************************************************************************/

/******************************************************************************
* Function Name: sddev_read_data
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
    uint64_t *p_reg;
    uint64_t *p_l;
    uint8_t  *p_c;
    volatile uint64_t tmp;

    /* Cast to an appropriate type */
    UNUSED_PARAM(sd_port);

    /* Cast to an appropriate type */
    p_reg = (uint64_t *)(reg_addr);

    cnt = (num / 8);

    /* Cast to an appropriate type */
    if (0uL != ((uint32_t)buff & 0x7uL))
    {
        /* Cast to an appropriate type */
        p_c = (uint8_t *)buff;
        for (i = cnt; i > 0 ; i--)
        {
            tmp = *p_reg;

            /* Cast to an appropriate type */
            *p_c++ = (uint8_t)(tmp);

            /* Cast to an appropriate type */
            *p_c++ = (uint8_t)(tmp >> 8);

            /* Cast to an appropriate type */
            *p_c++ = (uint8_t)(tmp >> 16);

            /* Cast to an appropriate type */
            *p_c++ = (uint8_t)(tmp >> 24);

            /* Cast to an appropriate type */
            *p_c++ = (uint8_t)(tmp >> 32);

            /* Cast to an appropriate type */
            *p_c++ = (uint8_t)(tmp >> 40);

            /* Cast to an appropriate type */
            *p_c++ = (uint8_t)(tmp >> 48);

            /* Cast to an appropriate type */
            *p_c++ = (uint8_t)(tmp >> 56);
        }

        cnt = (num % 8);
        if (0 != cnt)
        {
            tmp = *p_reg;
            for (i = cnt; i > 0 ; i--)
            {
                /* Cast to an appropriate type */
                *p_c++ = (uint8_t)(tmp);
                tmp >>= 8;
            }
        }
    }
    else
    {
        /* Cast to an appropriate type */
        p_l = (uint64_t *)buff;
        for (i = cnt; i > 0 ; i--)
        {
            *p_l++ = *p_reg;
        }

        cnt = (num % 8);
        if (0 != cnt)
        {
            /* Cast to an appropriate type */
            p_c = (uint8_t *)p_l;
            tmp = *p_reg;
            for (i = cnt; i > 0 ; i--)
            {
                /* Cast to an appropriate type */
                *p_c++ = (uint8_t)(tmp);
                tmp >>= 8;
            }
        }
    }

    return SD_OK;
}
/*******************************************************************************
 End of function sddev_read_data
 ******************************************************************************/

/******************************************************************************
* Function Name: sddev_write_data
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

    /* Cast to an appropriate type */
    UNUSED_PARAM(sd_port);

    /* Cast to an appropriate type */
    uint64_t *p_reg = (uint64_t *)(reg_addr);

    /* Cast to an appropriate type */
    uint64_t *p_buff = (uint64_t *)buff;
    uint64_t tmp;

    /* dont care non 8byte alignment data */
    num += 7;
    num /= 8;

    /* Cast to an appropriate type */
    if (((uint32_t)buff & 0x7uL) != 0uL)
    {
        for (i = num; i > 0 ; i--)
        {
            /* Cast to an appropriate type */
            tmp  = (uint64_t)(*buff++);

            /* Cast to an appropriate type */
            tmp |= ((uint64_t)(*buff++) << 8);

            /* Cast to an appropriate type */
            tmp |= ((uint64_t)(*buff++) << 16);

            /* Cast to an appropriate type */
            tmp |= ((uint64_t)(*buff++) << 24);

            /* Cast to an appropriate type */
            tmp |= ((uint64_t)(*buff++) << 32);

            /* Cast to an appropriate type */
            tmp |= ((uint64_t)(*buff++) << 40);

            /* Cast to an appropriate type */
            tmp |= ((uint64_t)(*buff++) << 48);

            /* Cast to an appropriate type */
            tmp |= ((uint64_t)(*buff++) << 56);
            *p_reg = tmp;
        }
    }
    else
    {
        for (i = num; i > 0 ; i--)
        {
            *p_reg = *p_buff++;
        }
    }

    return SD_OK;
}
/*******************************************************************************
 End of function sddev_write_data
 ******************************************************************************/

/******************************************************************************
* Function Name: sddev_get_clockdiv
* Description  : Get clock div value.
* Arguments    : int32_t sd_port : channel no (0 or 1)
*              : int32_t clock   : request clock frequency
*              :   SD_CLK_50MHZ
*              :   SD_CLK_25MHZ
*              :   SD_CLK_20MHZ
*              :   SD_CLK_10MHZ
*              :   SD_CLK_5MHZ
*              :   SD_CLK_1MHZ
*              :   SD_CLK_400KHZ
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

    /* Cast to an appropriate type */
    UNUSED_PARAM(sd_port);

    switch (clock)
    {
        case SD_CLK_50MHZ:
            div = SD_DIV_4;        /* 132MHz/4 = 33MHz      */
            break;
        case SD_CLK_25MHZ:
            div = SD_DIV_8;        /* 132MHz/8 = 16.5MHz    */
            break;
        case SD_CLK_20MHZ:
            div = SD_DIV_8;        /* 132MHz/8 = 16.5MHz    */
            break;
        case SD_CLK_10MHZ:
            div = SD_DIV_16;       /* 132MHz/16 = 8.3MHz    */
            break;
        case SD_CLK_5MHZ:
            div = SD_DIV_32;       /* 132MHz/32 = 4.1MHz    */
            break;
        case SD_CLK_1MHZ:
            div = SD_DIV_256;      /* 132MHz/256 = 515.6kHz */
            break;
        case SD_CLK_400KHZ:
            div = SD_DIV_512;      /* 132MHz/512 = 257.8kHz */
            break;
        default:
            div = SD_DIV_512;      /* 132MHz/512 = 257.8kHz */
            break;
    }

    return div;
}
/*******************************************************************************
 End of function sddev_get_clockdiv
 ******************************************************************************/

/******************************************************************************
* Function Name: sddev_set_port
* Description  : setting ports to use SDHI
* Arguments    : int32_t sd_port : channel no (0 or 1)
*              : int32_t mode    : SD_PORT_PARALLEL : 4bit mode
*                                : SD_PORT_SERIAL   : 1bit mode
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sddev_set_port(int32_t sd_port, int32_t mode)
{
    /* Cast to an appropriate type */
    UNUSED_PARAM(sd_port);

    /* Cast to an appropriate type */
    UNUSED_PARAM(mode);

    return SD_OK;
}
/*******************************************************************************
 End of function sddev_set_port
 ******************************************************************************/

/******************************************************************************
* Function Name: sddev_int_wait
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
#ifdef SD_CFG_HWINT
    st_sdhi_info_dev_ch_t *p_ch;

    ret  = SD_ERR;
    p_ch = sddev_get_dev_ch_instance(sd_port);

    if (p_ch != NULL) {
        if (osSemaphoreAcquire(p_ch->semaphore.sem_sync, time) == osOK) {
            ret = SD_OK;
        }
    }
#else /* #ifdef SD_CFG_HWINT */
    uint32_t waittime = (uint32_t)time;

    /* interrupt generated? */
    ret = sd_check_int(sd_port);
    while ((ret == SD_ERR) && (waittime > 0uL)) {
        osDelay(SDHI_PRV_1MSEC);
        waittime--;

        /* interrupt generated? */
        ret = sd_check_int(sd_port);
    }

#endif /* #ifdef SD_CFG_HWINT */

    return ret;
#else
    int32_t  ret;
    uint32_t waittime;

#ifdef SD_CFG_HWINT
    bool_t   chk;
    st_sdhi_info_dev_ch_t *p_ch;

    ret  = SD_ERR;
    p_ch = sddev_get_dev_ch_instance(sd_port);

    /* Cast to an appropriate type */
    if (NULL != p_ch)
    {
        /* Cast to an appropriate type */
        waittime = (uint32_t)time;
        chk = false;

        while ((false == chk) && (waittime > SDHI_PRV_MAX_WAITTIME))
        {
            chk = R_OS_SemaphoreWait(&p_ch->semaphore.sem_sync, SDHI_PRV_MAX_WAITTIME);
            waittime -= SDHI_PRV_MAX_WAITTIME;
        }
        if (false == chk)
        {
            chk = R_OS_SemaphoreWait(&p_ch->semaphore.sem_sync, waittime);
        }
        if (true == chk)
        {
            ret = SD_OK;
        }
    }

#else /* #ifdef SD_CFG_HWINT */
    waittime = (uint32_t)time;

    /* interrupt generated? */
    ret = sd_check_int(sd_port);
    while ((SD_ERR == ret) && (waittime > 0uL))
    {
        R_OS_TaskSleep(SDHI_PRV_1MSEC);
        waittime--;

        /* interrupt generated? */
        ret = sd_check_int(sd_port);
    }

#endif /* #ifdef SD_CFG_HWINT */

    return ret;
#endif
}
/*******************************************************************************
 End of function sddev_int_wait
 ******************************************************************************/

/******************************************************************************
* Function Name: sddev_init_dma
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
#ifdef    SD_CFG_TRNS_DMA
    int32_t ret = SD_ERR;

    ret = sd_DMAC_PeriReqInit(sd_port, buff, dir);
    if (SD_OK == ret)
    {
        ret = sd_DMAC_Open(sd_port, dir);
    }
    return ret;

#else

    /* Cast to an appropriate type */
    UNUSED_PARAM(sd_port); 

    /* Cast to an appropriate type */
    UNUSED_PARAM(buff); 

    /* Cast to an appropriate type */
    UNUSED_PARAM(dir); 

    return SD_OK;

#endif
}
/*******************************************************************************
 End of function sddev_init_dma
 ******************************************************************************/

/******************************************************************************
* Function Name: sddev_wait_dma_end
* Description  : Wait to complete DMAC transfer
* Arguments    : int32_t sd_port : channel no (0 or 1)
*              : int32_t cnt     : counts to transfer(unit:byte)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sddev_wait_dma_end(int32_t sd_port, int32_t cnt)
{
#ifdef SD_CFG_TRNS_DMA
#if(1) // mbed
    int32_t             ret;
    uint32_t            waittime;

#ifdef SD_CFG_HWINT
    st_sdhi_info_dev_ch_t *p_ch;

    ret  = SD_ERR;
    p_ch = sddev_get_dev_ch_instance(sd_port);
    
    if (p_ch != NULL)
    {
        /* Caluculate timeout time (as 512kByte/sec)    */
        waittime = ((((uint32_t)cnt / SDHI_PRV_TO_SPEED) / SDHI_PRV_1024BYTE) * SDHI_PRV_1000MSEC);
        if (waittime < SDHI_PRV_1000MSEC)
        {
            waittime = SDHI_PRV_1000MSEC;
        }

        if (osSemaphoreAcquire(p_ch->semaphore.sem_dma, waittime) == osOK) {
            ret = SD_OK;
        }
    }

#else /* #ifdef SD_CFG_HWINT */
    /* Caluculate timeout time (as 512kByte/sec)    */
    waittime = ((((uint32_t)cnt / SDHI_PRV_TO_SPEED) / SDHI_PRV_1024BYTE) * SDHI_PRV_1000MSEC);
    if (waittime < SDHI_PRV_1000MSEC)
    {
        waittime = SDHI_PRV_1000MSEC;
    }

    /* interrupt generated? */
    ret = sd_check_int_dm(sd_port);
    while ((ret == SD_ERR) && (waittime > 0uL))
    {
        osDelay(SDHI_PRV_1MSEC);
        waittime--;

        /* interrupt generated? */
        ret = sd_check_int_dm(sd_port);
    }

#endif /* #ifdef SD_CFG_HWINT */

    return ret;

#else
    int32_t  ret;
    uint32_t waittime;

#ifdef SD_CFG_HWINT
    bool_t   chk;
    st_sdhi_info_dev_ch_t *p_ch;

    ret  = SD_ERR;
    p_ch = sddev_get_dev_ch_instance(sd_port);

    /* Cast to an appropriate type */
    if (NULL != p_ch)
    {
        /* Caluculate timeout time (as 512kByte/sec)    */
        waittime = ((((uint32_t)cnt / SDHI_PRV_TO_SPEED) / SDHI_PRV_1024BYTE) * SDHI_PRV_1000MSEC);
        if (waittime < SDHI_PRV_1000MSEC)
        {
            waittime = SDHI_PRV_1000MSEC;
        }
        chk = false;

        while ((false == chk) && (waittime > SDHI_PRV_MAX_WAITTIME))
        {
            chk = R_OS_SemaphoreWait(&p_ch->semaphore.sem_dma, SDHI_PRV_MAX_WAITTIME);
            waittime -= SDHI_PRV_MAX_WAITTIME;
        }
        if (false == chk)
        {
            chk = R_OS_SemaphoreWait(&p_ch->semaphore.sem_dma, waittime);
        }
        if (true == chk)
        {
            ret = SD_OK;
        }
    }

#else /* #ifdef SD_CFG_HWINT */
    /* Caluculate timeout time (as 512kByte/sec)    */
    waittime = ((((uint32_t)cnt / SDHI_PRV_TO_SPEED) / SDHI_PRV_1024BYTE) * SDHI_PRV_1000MSEC);
    if (waittime < SDHI_PRV_1000MSEC)
    {
        waittime = SDHI_PRV_1000MSEC;
    }

    /* interrupt generated? */
    ret = sd_check_int_dm(sd_port);
    while ((SD_ERR == ret) && (waittime > 0uL))
    {
        R_OS_TaskSleep(SDHI_PRV_1MSEC);
        waittime--;

        /* interrupt generated? */
        ret = sd_check_int_dm(sd_port);
    }

#endif /* #ifdef SD_CFG_HWINT */

    return ret;

#endif
#else /* #ifdef SD_CFG_TRNS_DMA */

    /* Cast to an appropriate type */
    UNUSED_PARAM(sd_port); 

    /* Cast to an appropriate type */
    UNUSED_PARAM(cnt); 

    return SD_OK;

#endif /* #ifdef SD_CFG_TRNS_DMA */
}
/*******************************************************************************
 End of function sddev_wait_dma_end
 ******************************************************************************/

/******************************************************************************
* Function Name: sddev_disable_dma
* Description  : Disable DMAC transfer
* Arguments    : int32_t sd_port : channel no (0 or 1)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sddev_disable_dma(int32_t sd_port)
{
#ifdef SD_CFG_TRNS_DMA
    int32_t ret;

    ret = sd_DMAC_Close(sd_port);

    return ret;

#else

    /* Cast to an appropriate type */
    UNUSED_PARAM(sd_port); 

    return SD_OK;

#endif
}
/*******************************************************************************
 End of function sddev_disable_dma
 ******************************************************************************/

/******************************************************************************
* Function Name: sddev_reset_dma
* Description  : Reset of the SDHI module built-in DMAC.
* Arguments    : int32_t sd_port : channel no (0 or 1)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sddev_reset_dma(int32_t sd_port)
{
    int32_t ret;

    ret = sd_DMAC_Reset(sd_port);
    if (SD_OK == ret)
    {
        ret = sd_DMAC_Released(sd_port);
    }
    return ret;
}
/*******************************************************************************
 End of function sddev_reset_dma
 ******************************************************************************/

/******************************************************************************
* Function Name: sddev_finalize_dma
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
/*******************************************************************************
 End of function sddev_finalize_dma
 ******************************************************************************/

/******************************************************************************
* Function Name: sddev_loc_cpu
* Description  : lock cpu to disable interrupt
* Arguments    : int32_t sd_port : channel no (0 or 1)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sddev_loc_cpu(int32_t sd_port)
{
    /* Cast to an appropriate type */
    UNUSED_PARAM(sd_port);

    /* R_INTC_GetMaskLevel(&g_sdhi_priority_backup); */
    /* R_INTC_SetMaskLevel(0); */

    return SD_OK;
}
/*******************************************************************************
 End of function sddev_loc_cpu
 ******************************************************************************/

/******************************************************************************
* Function Name: sddev_unl_cpu
* Description  : unlock cpu to enable interrupt
* Arguments    : int32_t sd_port : channel no (0 or 1)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sddev_unl_cpu(int32_t sd_port)
{
    /* Cast to an appropriate type */
    UNUSED_PARAM(sd_port);

    /* R_INTC_SetMaskLevel(g_sdhi_priority_backup); */

    return SD_OK;
}
/*******************************************************************************
 End of function sddev_unl_cpu
 ******************************************************************************/

/******************************************************************************
* Function Name: sddev_finalize
* Description  : finalize SDHI
* Arguments    : int32_t sd_port : channel no (0 or 1)
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sddev_finalize(int32_t sd_port)
{
#if(1) // mbed
    int32_t               ret;
    st_sdhi_info_dev_ch_t *p_ch;
    volatile uint8_t      reg_read_8;

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

#ifdef SD_CFG_HWINT
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
    e_stb_err_t           e_stb_err;
    int32_t               ret;
    uint32_t              was_masked;
    st_sdhi_info_dev_ch_t *p_ch;
#ifdef SD_CFG_HWINT
    e_r_drv_intc_err_t  e_intc_err;
#endif /* #ifdef SD_CFG_HWINT */

    ret  = SD_OK;
    p_ch = sddev_get_dev_ch_instance(sd_port);

    /* Cast to an appropriate type */
    if (NULL != p_ch)
    {
        was_masked = __disable_irq();

        /* No timer monitering. */
        e_stb_err = STB_AGAIN;
        while (STB_AGAIN == e_stb_err)
        {
            /* [Transition to Module Standby Function]
               1. Set the corresponding bit in the STBREQ register to 1 to
                  generate a request to stop the module.
               2. Confirm that the module is ready to be stopped by the
                  corresponding bit in the STBACK register being set to 1.  */
            e_stb_err = R_STB_RequestModuleStop(p_ch->stb.stb_ch);
        }
        if (STB_SUCCESS == e_stb_err)
        {
            /* 3. Set the MSTP bit of the corresponding module to 1. */
            e_stb_err = R_STB_StopModule(p_ch->stb.stb_ch);
        }
        else
        {
            ret = SD_ERR;
        }

        if (0uL == was_masked)
        {
            /* enable all irq */
            __enable_irq();
        }
        
#ifdef SD_CFG_HWINT
        /* --- Invalidate SDHI interrupt --- */
        e_intc_err = R_INTC_Disable(p_ch->intc.int_id);
        if (INTC_SUCCESS != e_intc_err)
        {
            /* invalid id */
            ret = SD_ERR;
        }

        /* --- Unregister SDHI interrupt handler --- *//* Cast to an appropriate type */
        e_intc_err = R_INTC_RegistIntFunc(p_ch->intc.int_id, NULL);
        if (INTC_SUCCESS != e_intc_err)
        {
            /* invalid id */
            ret = SD_ERR;
        }

        /* --- Delete semaphore --- */
        R_OS_SemaphoreDelete(&p_ch->semaphore.sem_sync);      /* p_ch->semaphore.sem_sync = 0 */
        R_OS_SemaphoreDelete(&p_ch->semaphore.sem_dma);       /* p_ch->semaphore.sem_dma  = 0 */
#endif
    }
    else /* if (NULL != p_ch) */
    {
        ret = SD_ERR;
    }
    return ret;
#endif
}
/*******************************************************************************
 End of function sddev_finalize
 ******************************************************************************/

/******************************************************************************
* Function Name: sddev_sd_int_handler_0
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
#if(1) // mbed
#else
    /* Cast to an appropriate type */
    UNUSED_PARAM(int_sense);
#endif

    sd_int_handler(0);
    sd_int_dm_handler(0);
}
/*******************************************************************************
 End of function sddev_sd_int_handler_0
 ******************************************************************************/

/******************************************************************************
* Function Name: sddev_sd_int_handler_1
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
#if(1) // mbed
#else
    /* Cast to an appropriate type */
    UNUSED_PARAM(int_sense);
#endif

    sd_int_handler(1);
    sd_int_dm_handler(1);
}
/*******************************************************************************
 End of function sddev_sd_int_handler_1
 ******************************************************************************/

/******************************************************************************
* Function Name: sddev_cd_layout
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
    if (SD_OK == ret)
    {
        if (SD_CD_ENABLED != SDHI_SC_TABLE[sc_port].config.cd)
        {
            ret = SD_ERR;
        }
    }
    return ret;
}
/*******************************************************************************
 End of function sddev_cd_layout
 ******************************************************************************/

/******************************************************************************
* Function Name: sddev_wp_layout
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
    if (SD_OK == ret)
    {
        if (SD_WP_ENABLED != SDHI_SC_TABLE[sc_port].config.wp)
        {
            ret = SD_ERR;
        }
    }
    return ret;
}
/*******************************************************************************
 End of function sddev_wp_layout
 ******************************************************************************/

/******************************************************************************
* Function Name: sddev_get_sc_table_config_ch
* Description  : Get the channel number of the table generated by the smart
*              : configurator.
* Arguments    : int32_t sd_port    : channel no (0 or 1)
*              : int32_t *p_sc_port : pointer to sc channel no
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t sddev_get_sc_table_config_ch(int32_t sd_port, int32_t *p_sc_port)
{
    int32_t ret;
    int32_t ch_num;
    int32_t cnt;

    ret = SD_ERR;

    /* Cast to an appropriate type */
    if (NULL != p_sc_port)
    {
        ch_num = (sizeof(SDHI_SC_TABLE))/sizeof(st_r_drv_sdhi_sc_config_t);

        for (cnt = 0; cnt < ch_num; cnt++)
        {
            if (SDHI_SC_TABLE[cnt].channel == sd_port)
            {
                ret = SD_OK;
                *p_sc_port = cnt;
                break;
            }
        }
    }
    return ret;
}
/*******************************************************************************
 End of function sddev_get_sc_table_config_ch
 ******************************************************************************/

/******************************************************************************
* Function Name: sddev_get_dev_ch_instance
* Description  : Get SDHI driver target CPU interface information object
* Arguments    : int32_t sd_port : channel no (0 or 1)
* Return Value : success : pointer to SDHI driver target CPU interface information
*              : fail    : NULL
******************************************************************************/
st_sdhi_info_dev_ch_t *sddev_get_dev_ch_instance(int32_t sd_port)
{
    st_sdhi_info_dev_ch_t *p_info;

    if ((sd_port >= SDHI_PRV_CH_0) && (sd_port < SDHI_PRV_CH_NUM))
    {
        p_info = &s_sdhi_dev_ch[sd_port];
    }
    else
    {
        /* Cast to an appropriate type */
        p_info = NULL;

        /* NON_NOTICE_ASSERT: channel range over */
    }
    return p_info;
}
/*******************************************************************************
 End of function sddev_get_dev_ch_instance
 ******************************************************************************/

/******************************************************************************
* Function Name: SD_status_callback_function
* Description  : Callback interrupt function for SDHI protocol status control
* Arguments    : int32_t sd_port : channel no (0 or 1)
*              : int32_t cd      : no used
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t SD_status_callback_function(int32_t sd_port, int32_t cd)
{
    /* Cast to an appropriate type */
    UNUSED_PARAM(cd);

#ifdef SD_CFG_HWINT
    int32_t               ret;
    st_sdhi_info_dev_ch_t *p_ch;

    ret  = SD_ERR;
    p_ch = sddev_get_dev_ch_instance(sd_port);

    /* Cast to an appropriate type */
    if (NULL != p_ch)
    {
#if(1) // mbed
        (void)osSemaphoreRelease(p_ch->semaphore.sem_sync);
#else
        R_OS_SemaphoreRelease(&p_ch->semaphore.sem_sync);
#endif
        ret = SD_OK;
    }
    return ret;

#else /* #ifdef SD_CFG_HWINT */

    /* Cast to an appropriate type */
    UNUSED_PARAM(sd_port);

    return SD_OK;

#endif /* #ifdef SD_CFG_HWINT */
}
/*******************************************************************************
 End of function SD_status_callback_function
 ******************************************************************************/

/******************************************************************************
* Function Name: SD_dma_end_callback_function
* Description  : Callback interrupt function for complete DMA transfer control
* Arguments    : int32_t sd_port : channel no (0 or 1)
*              : int32_t cd      : no used
* Return Value : success : SD_OK
*              : fail    : SD_ERR
******************************************************************************/
int32_t SD_dma_end_callback_function(int32_t sd_port, int32_t cd)
{
    /* Cast to an appropriate type */
    UNUSED_PARAM(cd);

#ifdef SD_CFG_HWINT
    int32_t               ret;
    st_sdhi_info_dev_ch_t *p_ch;

    ret  = SD_ERR;
    p_ch = sddev_get_dev_ch_instance(sd_port);

    /* Cast to an appropriate type */
    if (NULL != p_ch)
    {
#if(1) // mbed
        (void)osSemaphoreRelease(p_ch->semaphore.sem_dma);
#else
        R_OS_SemaphoreRelease(&p_ch->semaphore.sem_dma);
#endif
        ret = SD_OK;
    }
    return ret;

#else /* #ifdef SD_CFG_HWINT */

    /* Cast to an appropriate type */
    UNUSED_PARAM(sd_port);

    return SD_OK;

#endif /* #ifdef SD_CFG_HWINT */
}
/*******************************************************************************
 End of function SD_dma_end_callback_function
 ******************************************************************************/

/******************************************************************************
 * Function Name: SD_confirm_semaphore
 * Description  : Confirm semaphore status
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 * Return Value : none
 *****************************************************************************/
void SD_confirm_semaphore(int32_t sd_port)
{
#ifdef SD_CFG_HWINT
#if(1) // mbed
    st_sdhi_info_dev_ch_t  *p_ch;

    p_ch = sddev_get_dev_ch_instance(sd_port);

    if (p_ch != NULL) {
        while (osSemaphoreAcquire(p_ch->semaphore.sem_sync, 0) == osOK);
    }
#else
    bool_t                chk;
    st_sdhi_info_dev_ch_t *p_ch;

    chk  = true;
    p_ch = sddev_get_dev_ch_instance(sd_port);

    /* Cast to an appropriate type */
    if (NULL != p_ch)
    {
        while (true == chk)
        {
            chk = R_OS_SemaphoreWait(&p_ch->semaphore.sem_sync, 0uL);
        }
    }
#endif

#else /* #ifdef SD_CFG_HWINT */

    /* Cast to an appropriate type */
    UNUSED_PARAM(sd_port);

#endif /* #ifdef SD_CFG_HWINT */
}
/*******************************************************************************
 End of function SD_confirm_semaphore
 ******************************************************************************/

/* End of File */
