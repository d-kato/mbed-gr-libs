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

/******************************************************************************
* File Name    : spdif.h
* $Rev: $
* $Date::                           $
* Description  : SPDIF driver functions header
******************************************************************************/

#ifndef SPDIF_H
#define SPDIF_H

/*******************************************************************************
Includes <System Includes>, "Project Includes"
*******************************************************************************/
#include "aioif.h"
#include "iodefine.h"
#include "iobitmask.h"
#include "spdif_if.h"
#include "bsp_drv_cmn.h"

/******************************************************************************
Macro definitions
******************************************************************************/

#define SPDIF_CHNUM_0    (0u)

#define SPDIF_CHSTR_0 "\\0"

#define SPDIF_MAX_PATH_LEN           (32u)


/******************************************************************************
Private global variables and functions
******************************************************************************/

/*************************************************************************
 Enumerated Types
*************************************************************************/
typedef enum {
    SPDIF_DRVSTS_UNINIT = 0,
    SPDIF_DRVSTS_INIT
} spdif_drv_stat_t;

typedef enum
{
    SPDIF_CHSTS_UNINIT = 0,
    SPDIF_CHSTS_INIT,
    SPDIF_CHSTS_OPEN
} spdif_ch_stat_t;

typedef enum
{
    SPDIF_ASYNC_W = 0,
    SPDIF_ASYNC_R,
    SPDIF_ASYNC_STRUCT_W,
    SPDIF_ASYNC_STRUCT_R,
    SPDIF_ASYNC_SET_CH_STS
} spdif_rw_mode_t;

/*************************************************************************
 Structures
*************************************************************************/
typedef struct spdif_info_ch
{
    uint32_t    channel;
    bool_t      enabled;
    uint32_t    sample_freq;
    spdif_ch_stat_t ch_stat;
    osSemaphoreId_t sem_access;
    AHF_S       tx_que;
    AHF_S       rx_que;
    int_t       dma_rx_ch;
    int_t       dma_tx_ch;
    int_t       openflag;
    AIOCB*      p_aio_tx_curr;
    AIOCB*      p_aio_tx_next;
    AIOCB*      p_aio_rx_curr;
    AIOCB*      p_aio_rx_next;
    AIOCB       s_aio_set_ch_sts;
    uint32_t    ch_sts[2];
    bool_t      ch_sts_set_req;
    spdif_chcfg_cks_t               clk_select;          /**< CTRL-CKS : Audio clock select         */
    spdif_chcfg_audio_bit_t         audio_bit_tx;        /**< CTRL-TASS : Audio bit length          */
    spdif_chcfg_audio_bit_t         audio_bit_rx;        /**< CTRL-RASS : Audio bit length          */
    bool_t      tx_u_data_enabled;
    uint32_t    tlcs;
    uint32_t    trcs;
    uint32_t    rlcs;
    uint32_t    rrcs;
    uint32_t    tu_idx;
    uint32_t    ru_idx;
    uint8_t     int_level;
} spdif_info_ch_t;


typedef struct spdif_info_drv
{
    spdif_drv_stat_t drv_stat;
    spdif_info_ch_t  info_ch;
} spdif_info_drv_t;

/******************************************************************************
 Function Prototypes
 *****************************************************************************/
#if(1) /* mbed */
int_t SPDIF_InitialiseOne(const int_t channel, const spdif_channel_cfg_t* const p_cfg_data);
int_t SPDIF_UnInitialiseOne(const int_t channel);
#endif /* end mbed */
int_t SPDIF_Initialise(const spdif_channel_cfg_t* const p_cfg_data);
int_t SPDIF_UnInitialise(void);
int_t SPDIF_EnableChannel(spdif_info_ch_t* const p_info_ch);
int_t SPDIF_DisableChannel(spdif_info_ch_t* const p_info_ch);
void  SPDIF_ErrorRecovery(spdif_info_ch_t* const p_info_ch);

void SPDIF_PostAsyncIo(spdif_info_ch_t* const p_info_ch, AIOCB* const p_aio);
void SPDIF_PostAsyncCancel(spdif_info_ch_t* const p_info_ch, AIOCB* const p_aio);

int_t SPDIF_IOCTL_ConfigChannel(spdif_info_ch_t* const p_info_ch,
                const spdif_channel_cfg_t* const p_ch_cfg);
int_t SPDIF_IOCTL_SetChannelStatus(spdif_info_ch_t* const p_info_ch,
                                   spdif_channel_status_t* const p_channel_status);
int_t SPDIF_IOCTL_GetChannelStatus(spdif_info_ch_t* const p_info_ch,
                                   spdif_channel_status_t* const p_channel_status);
int_t SPDIF_IOCTL_SetTransAudioBit(spdif_info_ch_t* const p_info_ch,
                                   spdif_chcfg_audio_bit_t sudio_bit);
int_t SPDIF_IOCTL_SetRecvAudioBit(spdif_info_ch_t* const p_info_ch,
                                   spdif_chcfg_audio_bit_t sudio_bit);

int_t SPDIF_InitDMA(spdif_info_ch_t* const p_info_ch);
void SPDIF_UnInitDMA(spdif_info_ch_t* const p_info_ch);
void SPDIF_CancelDMA(const spdif_info_ch_t* const p_info_ch, int_t openflag);
int_t SPDIF_RestartDMA(spdif_info_ch_t* const p_info_ch, int_t openflag);

void SPDIF_UserInfoRegSet(spdif_info_ch_t* const p_info_ch);;
void SPDIF_UserInfoRegGet(spdif_info_ch_t* const p_info_ch);


#if(1) /* mbed */
extern spdif_info_drv_t * SPDIF_GetDrvInstanc(void);
#else
extern spdif_info_drv_t g_spdif_info_drv;
#endif /* end mbed */

#endif /* SPDIF_H */
