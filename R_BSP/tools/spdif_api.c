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

#if defined(TARGET_RZ_A2XX)

#include "spdif_api.h"

static const PinMap PinMap_SPDIF_AUDIO_CLK[] = {
    {P6_4,  0,      4},
    {PH_0,  0,      1},
    {NC,    NC,     0}
};

static const PinMap PinMap_SPDIF_TX[] = {
    {PJ_0,  0,      2},
    {PC_5,  0,      4},
    {NC,    NC,     0}
};

static const PinMap PinMap_SPDIF_RX[] = {
    {PJ_1,  0,      2},
    {PC_4,  0,      4},
    {NC,    NC,     0}
};

int32_t spdif_init(PinName audio_clk, PinName tx, PinName rx) {
    /* determine the ssif to use */
    uint32_t spdif_clk = pinmap_peripheral(audio_clk, PinMap_SPDIF_AUDIO_CLK);
    uint32_t spdif_tx  = pinmap_peripheral(tx, PinMap_SPDIF_TX);
    uint32_t spdif_rx  = pinmap_peripheral(rx, PinMap_SPDIF_RX);
    uint32_t spdif_ch  = pinmap_merge(spdif_tx, spdif_rx);

    if ((int32_t)spdif_clk != NC) {
        pinmap_pinout(audio_clk, PinMap_SPDIF_AUDIO_CLK);
    }

    if ((int32_t)spdif_tx != NC) {
        pinmap_pinout(tx, PinMap_SPDIF_TX);
    }

    if ((int32_t)spdif_rx != NC) {
        pinmap_pinout(rx, PinMap_SPDIF_RX);
    }

    return (int32_t)spdif_ch;
}

#endif /* TARGET_RZ_A2XX */
