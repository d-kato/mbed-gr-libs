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
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

#include "R_BSP_Spdif.h"
#if (R_BSP_SPDIF_ENABLE == 1)
#include "r_bsp_cmn.h"
#include "spdif_if.h"
#include "spdif_api.h"

R_BSP_Spdif::R_BSP_Spdif(PinName audio_clk, PinName tx, PinName rx) : spdif_ch(-1) {
    int32_t wk_channel;

    wk_channel = spdif_init(audio_clk, tx, rx);
    if (wk_channel != NC) {
        spdif_ch = wk_channel;
    }
}

R_BSP_Spdif::~R_BSP_Spdif() {
    // do nothing
}

void R_BSP_Spdif::init(const spdif_channel_cfg_t* const p_ch_cfg, int32_t max_write_num, int32_t max_read_num) {
    if (spdif_ch >= 0) {
        init_channel(R_SPDIF_MakeCbTbl_mbed(), spdif_ch, (void *)p_ch_cfg, max_write_num, max_read_num);
    }
}

bool R_BSP_Spdif::ConfigChannel(const spdif_channel_cfg_t* const p_ch_cfg) {
    return ioctl(SPDIF_CONFIG_CHANNEL, (void *)p_ch_cfg);
}

bool R_BSP_Spdif::SetChannelStatus(uint32_t status_Lch, uint32_t status_Rch) {
    spdif_channel_status_t channel_status;

    channel_status.Lch = status_Lch;
    channel_status.Rch = status_Rch;

    return ioctl(SPDIF_SET_CHANNEL_STATUS, (void *)&channel_status);
}

bool R_BSP_Spdif::GetChannelStatus(uint32_t * p_status_Lch, uint32_t * p_status_Rch) {
    spdif_channel_status_t channel_status;

    if (ioctl(SPDIF_GET_CHANNEL_STATUS, (void *)&channel_status) != false) {
        if (p_status_Lch != NULL) {
            *p_status_Lch = channel_status.Lch;
        }
        if (p_status_Rch != NULL) {
            *p_status_Rch = channel_status.Rch;
        }
    }

    return true;
}

bool R_BSP_Spdif::SetTransAudioBit(int direction, char length) {
    spdif_chcfg_audio_bit_t audio_bit;
    int control_type;

    if (direction == 0) {
        control_type = SPDIF_SET_TRANS_AUDIO_BIT;
    } else {
        control_type = SPDIF_SET_RECV_AUDIO_BIT;
    }

    switch (length) {
        case 16:
            audio_bit = SPDIF_CFG_AUDIO_BIT_16;
            break;
        case 20:
            audio_bit = SPDIF_CFG_AUDIO_BIT_20;
            break;
        case 24:
            audio_bit = SPDIF_CFG_AUDIO_BIT_24;
            break;
        default:
            return false;
    }

    return ioctl(control_type, (void *)&audio_bit);
}

#endif /* R_BSP_SPDIF_ENABLE */
