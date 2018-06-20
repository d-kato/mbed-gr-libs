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

#include "r_bsp_cmn.h"
#include "R_BSP_Ssif.h"
#include "ssif_if.h"
#include "ssif_api.h"

R_BSP_Ssif::R_BSP_Ssif(PinName sck, PinName ws, PinName tx, PinName rx, PinName audio_clk) : ssif_ch(-1) {
    int32_t wk_channel;

    wk_channel = ssif_init(sck, ws, tx, rx, audio_clk);
    if (wk_channel != NC) {
        ssif_ch      = wk_channel;
    }
}

R_BSP_Ssif::~R_BSP_Ssif() {
    // do nothing
}

void R_BSP_Ssif::init(const ssif_channel_cfg_t* const p_ch_cfg, int32_t max_write_num, int32_t max_read_num) {
    if (ssif_ch >= 0) {
        init_channel(R_SSIF_MakeCbTbl_mbed(), ssif_ch, (void *)p_ch_cfg, max_write_num, max_read_num);
    }
}


bool R_BSP_Ssif::ConfigChannel(const ssif_channel_cfg_t* const p_ch_cfg) {
    return ioctl(SSIF_CONFIG_CHANNEL, (void *)p_ch_cfg);
}

bool R_BSP_Ssif::GetStatus(uint32_t* const p_status) {
    return ioctl(SSIF_CONFIG_CHANNEL, (void *)p_status);
}
