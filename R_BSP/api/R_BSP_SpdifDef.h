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
/**************************************************************************//**
* @file          R_BSP_SpdifDef.h
* @brief         SPDIF defines
******************************************************************************/

#ifndef R_BSP_SPDIF_DEF_H
#define R_BSP_SPDIF_DEF_H

/******************************************************************************
Includes <System Includes> , "Project Includes"
******************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/******************************************************************************
 Defines
 *****************************************************************************/

/******************************************************************************
 Constant Macros
 *****************************************************************************/
#define SPDIF_NUM_CHANS       (1u)    /**< Number of SPDIF channels */

/* ==== SPDIF Buffer ==== */
#define SPDIF_NUMOF_FRM   (192)
#define SPDIF_NUMOF_CH    (2)
#define SPDIF_AUDIO_BUFFSIZE (SPDIF_NUMOF_FRM * SPDIF_NUMOF_CH)
#define SPDIF_USER_BUFFSIZE  ((SPDIF_NUMOF_FRM * SPDIF_NUMOF_CH) / 32)

/******************************************************************************
 Function Macros
 *****************************************************************************/

/******************************************************************************
 Enumerated Types
 *****************************************************************************/
/** CTRL:CKS(Clock source for oversampling) */
typedef enum
{
    SPDIF_CFG_CKS_AUDIO_X1  = 0,  /**< select AUDIO_X1   */
    SPDIF_CFG_CKS_AUDIO_CLK = 1   /**< select AUIDIO_CLK */
} spdif_chcfg_cks_t; 

/** CTRL:RASS,TASS(Audio bit length) */
typedef enum
{
    SPDIF_CFG_AUDIO_BIT_16 = 0,   /**< Audio bit length 16 */
    SPDIF_CFG_AUDIO_BIT_20 = 1,   /**< Audio bit length 20 */
    SPDIF_CFG_AUDIO_BIT_24 = 2    /**< Audio bit length 24 */
} spdif_chcfg_audio_bit_t;

/******************************************************************************
 Structures
 *****************************************************************************/

/** This structure contains the configuration settings */
typedef struct
{
    bool                            enabled;             /**< The enable flag for the channel       */
    uint8_t                         int_level;           /**< Interrupt priority for the channel    */
    spdif_chcfg_cks_t               clk_select;          /**< CTRL-CKS : Audio clock select         */
    spdif_chcfg_audio_bit_t         audio_bit_tx;        /**< CTRL-TASS : Audio bit length          */
    spdif_chcfg_audio_bit_t         audio_bit_rx;        /**< CTRL-RASS : Audio bit length          */
    bool                            tx_u_data_enabled;   /**< CTRL-RASS : Audio bit length          */
} spdif_channel_cfg_t;

typedef struct
{
    uint32_t                        Lch;                 /**< Channel Status bit Lch    */
    uint32_t                        Rch;                 /**< Channel Status bit Rch    */
} spdif_channel_status_t;

typedef struct
{
    uint32_t a_buff[SPDIF_AUDIO_BUFFSIZE];               /**< Audio Buffer */
    uint32_t u_buff[SPDIF_USER_BUFFSIZE];                /**< User Buffer */
    uint32_t s_buff[SPDIF_NUMOF_CH];                     /**< Channel Status Buffer */
} spdif_t;

/******************************************************************************
 IOCTLS
 *****************************************************************************/

#define SPDIF_CONFIG_CHANNEL        (0)
#define SPDIF_SET_CHANNEL_STATUS    (1)
#define SPDIF_GET_CHANNEL_STATUS    (2)
#define SPDIF_SET_TRANS_AUDIO_BIT   (3)
#define SPDIF_SET_RECV_AUDIO_BIT    (4)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* R_BSP_SPDIF_DEF_H */
