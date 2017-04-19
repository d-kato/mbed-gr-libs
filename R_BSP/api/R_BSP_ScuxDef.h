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
* Copyright (C) 2015 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**************************************************************************//**
* @file          R_BSP_ScuxDef.h
* @brief         SCUX defines
******************************************************************************/

#ifndef R_BSP_SCUXDEF_H
#define R_BSP_SCUXDEF_H

/******************************************************************************
Includes <System Includes> , "Project Includes"
******************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*************************************************************************
 Enumerated Types
*************************************************************************/

/** SCUX channel number */
typedef enum
{
    SCUX_CH_0 = 0,   /**< Specifies SCUX0. */
    SCUX_CH_1 = 1,   /**< Specifies SCUX1. */
    SCUX_CH_2 = 2,   /**< Specifies SCUX2. */
    SCUX_CH_3 = 3,   /**< Specifies SCUX3. */
    SCUX_CH_NUM = 4  /**< Number of SCUX channels. */
} scux_ch_num_t;

/** used audio channel number setting */
typedef enum
{
    SCUX_USE_CH_1 = 1,   /**< used audio channel number is 1 */
    SCUX_USE_CH_2 = 2,   /**< used audio channel number is 2 */
    SCUX_USE_CH_4 = 4,   /**< used audio channel number is 4 */
    SCUX_USE_CH_6 = 6,   /**< used audio channel number is 6 */
    SCUX_USE_CH_8 = 8    /**< used audio channel number is 8 */
} scux_use_channel_t;

/** SCUX data word length */
typedef enum
{
    SCUX_DATA_LEN_MIN      =(-1), /**< For data word length identification [unsettable] */
    SCUX_DATA_LEN_24       = 0,   /**< Runs with a data word length of 24 bits. */
    SCUX_DATA_LEN_16       = 1,   /**< Runs with a data word length of 16 bits. */
    SCUX_DATA_LEN_16_TO_24 = 2,   /**< Outputs 16-bit data in 24-bit mode. */
    SCUX_DATA_LEN_MAX      = 3    /**< For data word length identification [unsettable] */
} scux_data_word_len_t;

/******************************************************************************
Macro definitions
******************************************************************************/

/** Status of the request */
#define SCUX_STAT_STOP    0  /**< Acceptance of requests is stopped */
#define SCUX_STAT_IDLE    1  /**< Processing of all requests is completed and waiting for a request. */
#define SCUX_STAT_TRANS   2  /**< Transfer in progress */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* R_BSP_SCUXDEF_H */
