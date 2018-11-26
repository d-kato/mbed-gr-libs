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

#include "mbed.h"
#include "SPDIF_RBSP.h"
#include "pinmap.h"

#if (R_BSP_SPDIF_ENABLE == 1)
SPDIF_RBSP::SPDIF_RBSP(PinName audio_clk, PinName tx, PinName rx, bool tx_udata_enable,
    uint8_t int_level, int32_t max_write_num, int32_t max_read_num) 
       : mSpdif_(audio_clk, tx, rx) {

    spdif_cfg.enabled      = true;
    spdif_cfg.int_level    = int_level;
    if ((int32_t)audio_clk == NC) {
        spdif_cfg.clk_select   = SPDIF_CFG_CKS_AUDIO_X1;
    } else {
        spdif_cfg.clk_select   = SPDIF_CFG_CKS_AUDIO_CLK;
    }
    spdif_cfg.audio_bit_tx = SPDIF_CFG_AUDIO_BIT_24;
    spdif_cfg.audio_bit_rx = SPDIF_CFG_AUDIO_BIT_24;
    spdif_cfg.tx_u_data_enabled = tx_udata_enable;
    mSpdif_.init(&spdif_cfg, max_write_num, max_read_num);

    frequency(44100);              // Default sample frequency is 44.1kHz
}

// Public Functions
bool SPDIF_RBSP::outputVolume(float leftVolumeOut, float rightVolumeOut) {
    // do nothing
    return false;
}

bool SPDIF_RBSP::micVolume(float VolumeIn) {
    // do nothing
    return false;
}

void SPDIF_RBSP::power(bool type) {
    // do nothing
}

bool SPDIF_RBSP::format(char length) {
    return mSpdif_.SetTransAudioBit(0, length);
}

bool SPDIF_RBSP::frequency(int hz) {
    uint32_t channel_status_Lch;
    uint32_t channel_status_Rch;

    /* Status Register
    b31:b30 Reserved - Read only bit       - 00:
    b29:b28 CLAC     - Clock Accuracy      - 01:   Level 1 (50 ppm)
    b27:b24 FS       - Sample Frequency    - 0000: 44.1kHz
    b23:b20 CHNO     - Channel Number      - 0001: Left (0010: Right)
    b19:b16 SRCNO    - Source Number       - 0000:
    b15:b8  CATCD    - Category Code       - 00000000: General 2-channel format
    b7:b6   Reserved - Read only bit       - 00: 
    b5:b1   CTL      - Control             - 0:  2 channel Audio, 
                                             00: Disable preemphasis
                                             0:  Disable copy
                                             0:  LPCM
    b0      Reserved - Read only bit       - 0: */

    switch (hz) {
        case 48000:
            channel_status_Lch = 0x12100000uL;
            channel_status_Rch = 0x12200000uL;
            break;
        case 44100:
            channel_status_Lch = 0x10100000uL;
            channel_status_Rch = 0x10200000uL;
            break;
        case 32000:
            channel_status_Lch = 0x13100000uL;
            channel_status_Rch = 0x13200000uL;
            break;
        default:
            return false;
    }

    return mSpdif_.SetChannelStatus(channel_status_Lch, channel_status_Rch);
}

int SPDIF_RBSP::write(void * const p_data, uint32_t data_size, const rbsp_data_conf_t * const p_data_conf) {
    return mSpdif_.write(p_data, data_size, p_data_conf);
}

int SPDIF_RBSP::write_s(spdif_t * const p_spdif_data, const rbsp_data_conf_t * const p_data_conf) {
    return mSpdif_.write((void *)p_spdif_data, 0, p_data_conf);
}

int SPDIF_RBSP::read(void * const p_data, uint32_t data_size, const rbsp_data_conf_t * const p_data_conf) {
    return mSpdif_.read(p_data, data_size, p_data_conf);
}

int SPDIF_RBSP::read_s(spdif_t * const p_spdif_data, const rbsp_data_conf_t * const p_data_conf) {
    return mSpdif_.read((void *)p_spdif_data, 0, p_data_conf);
}

#endif /* R_BSP_SPDIF_ENABLE */
