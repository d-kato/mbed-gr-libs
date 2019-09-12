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
/**********************************************************************************************************************
* File Name    : r_sdhi_simplified_drv_sc_cfg.h
* Version      : 1.20
* Device(s)    : R7S921053
* Description  : SDHI Configuration.
* Creation Date: 2019-05-29
 *********************************************************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_typedefs.h"
#if(1) /* mbed */
#else
#include "r_gpio_drv_api.h"
#include "r_gpio_drv_sc_cfg.h"
#endif
#include "r_sdif.h"

#ifndef SRC_RENESAS_CONFIG_R_SDHI_SIMPLIFIED_DRV_SC_CFG_H_
#define SRC_RENESAS_CONFIG_R_SDHI_SIMPLIFIED_DRV_SC_CFG_H_

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef enum
{
    SD_CB_UNUSED,               /*!< SD card detection callback function setting is unused */
    SD_CB_USED,                 /*!< SD card detection callback function setting is used */
} e_sd_cb_layout_t;

typedef struct
{
    e_sd_cd_layout_t  cd;       /*!< SD card detection option */
    e_sd_wp_layout_t  wp;       /*!< SD write protection signal detection option */
    e_sd_cb_layout_t  cb;       /*!< SD card detection callback function setting */
    p_intCallbackFunc cb_func;  /*!< SD card detection callback function */
} st_sdhi_config_t;

/**
 * @typedef st_r_drv_sdhi_sc_config_t Smart Configurator Interface
 */
typedef struct
{
    int_t                           channel;
    st_sdhi_config_t                config;
#if(1) /* mbed */
#else
    st_r_drv_gpio_pin_init_table_t  pin;
#endif
} st_r_drv_sdhi_sc_config_t;

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Variable Externs
******************************************************************************/
#if(1) /* mbed */
#else
#if defined(__cplusplus)
extern "C" {
#endif
/* This code is auto-generated. Do not edit manually */
extern int32_t fat_sample_cd_int_cb_function(int32_t sd_port, int32_t cd);
/* End of modification */
#if defined(__cplusplus)
}
#endif
#endif

/**
 * @section SDHI_SC_TABLE Smart Configurator settings table.
 */
#if(1) /* mbed */
static const st_r_drv_sdhi_sc_config_t SDHI_SC_TABLE[] =
{
    /* This code is auto-generated. Do not edit manually */
    { 0, 
        {
            SD_CD_ENABLED, 
            SD_WP_ENABLED, 
            SD_CB_UNUSED, 
            NULL, 
        }
    },
    { 1, 
        {
            SD_CD_ENABLED, 
#if defined(TARGET_RZ_A2M_SBEV)
            SD_WP_DISABLED, 
#else
            SD_WP_ENABLED, 
#endif
            SD_CB_UNUSED, 
            NULL, 
        }
    },
    /* End of modification */
};
#else
static const st_r_drv_sdhi_sc_config_t SDHI_SC_TABLE[] =
{
    /* This code is auto-generated. Do not edit manually */
    { 0, 
        {
            SD_CD_ENABLED, 
            SD_WP_ENABLED, 
            SD_CB_USED, 
            fat_sample_cd_int_cb_function, 
        }, 
        {
            &GPIO_SC_TABLE_sdhi_simplified0[0], 
            sizeof(GPIO_SC_TABLE_sdhi_simplified0)/sizeof(st_r_drv_gpio_sc_config_t), 
        }
    },
    { 1, 
        {
            SD_CD_ENABLED, 
            SD_WP_ENABLED, 
            SD_CB_UNUSED, 
            NULL, 
        }, 
        {
            &GPIO_SC_TABLE_sdhi_simplified1[0], 
            sizeof(GPIO_SC_TABLE_sdhi_simplified1)/sizeof(st_r_drv_gpio_sc_config_t), 
        }
    },
    /* End of modification */
};
#endif


#endif /* SRC_RENESAS_CONFIG_R_SDHI_SIMPLIFIED_DRV_SC_CFG_H_ */

/**********************************************************************************************************************
End  Of File
**********************************************************************************************************************/
