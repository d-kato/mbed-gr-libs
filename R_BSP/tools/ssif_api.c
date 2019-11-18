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
* Copyright (C) 2015 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

#include "ssif_api.h"

#if defined(TARGET_RZ_A1XX)
#if defined(TARGET_RZA1H) /* mbed */
static const PinMap PinMap_SSIF_SCK[] = {
//   pin      ch     func
    {P2_8   , 0    , 4},
    {P4_4   , 0    , 5},
    {P3_4   , 1    , 3},
    {P7_1   , 1    , 6},
    {P10_12 , 1    , 2},
    {P7_5   , 2    , 6},
    {P9_5   , 2    , 3},
    {P4_12  , 3    , 6},
    {P7_8   , 3    , 2},
    {P7_12  , 4    , 2},
    {P8_12  , 4    , 8},
    {P11_4  , 4    , 3},
    {P2_4   , 5    , 4},
    {P4_8   , 5    , 5},
    {P8_8   , 5    , 8},
    {NC     , NC   , 0}
};

static const PinMap PinMap_SSIF_WS[] = {
//   pin      ch     func
    {P2_9   , 0    , 4},
    {P4_5   , 0    , 5},
    {P3_5   , 1    , 3},
    {P7_2   , 1    , 6},
    {P10_13 , 1    , 2},
    {P7_6   , 2    , 6},
    {P9_6   , 2    , 3},
    {P4_13  , 3    , 6},
    {P7_9   , 3    , 2},
    {P7_13  , 4    , 2},
    {P8_13  , 4    , 8},
    {P11_5  , 4    , 3},
    {P2_5   , 5    , 4},
    {P4_9   , 5    , 5},
    {P8_9   , 5    , 8},
    {NC     , NC   , 0}
};

static const PinMap PinMap_SSIF_TxD[] = {
//   pin      ch     func
    {P2_11  , 0    , 4},
    {P4_7   , 0    , 5},
    {P7_4   , 1    , 6},
    {P10_15 , 1    , 2},
    {P7_7   , 2    , 6},  /* SSIDATA2 */
    {P9_7   , 2    , 3},  /* SSIDATA2 */
    {P4_15  , 3    , 6},
    {P7_11  , 3    , 2},
    {P6_1   , 4    , 6},  /* SSIDATA4 */
    {P7_14  , 4    , 2},  /* SSIDATA4 */
    {P8_14  , 4    , 8},  /* SSIDATA4 */
    {P11_6  , 4    , 3},  /* SSIDATA4 */
    {P2_7   , 5    , 4},
    {P4_11  , 5    , 5},
    {P8_10  , 5    , 8},
    {NC     , NC   , 0}
};

static const PinMap PinMap_SSIF_RxD[] = {
//   pin      ch     func
    {P2_10  , 0    , 4},
    {P4_6   , 0    , 5},
    {P3_7   , 1    , 3},
    {P7_3   , 1    , 6},
    {P10_14 , 1    , 2},
    {P7_7   , 2    , 6},  /* SSIDATA2 */
    {P9_7   , 2    , 3},  /* SSIDATA2 */
    {P4_14  , 3    , 6},
    {P7_10  , 3    , 2},
    {P6_1   , 4    , 6},  /* SSIDATA4 */
    {P7_14  , 4    , 2},  /* SSIDATA4 */
    {P8_14  , 4    , 8},  /* SSIDATA4 */
    {P11_6  , 4    , 3},  /* SSIDATA4 */
    {P2_6   , 5    , 4},
    {P4_10  , 5    , 5},
    {NC     , NC   , 0}
};

static const PinMap PinMap_SPDIF_AUDIO_CLK[] = {
    {NC,    NC,     0}
};

#else
static const PinMap PinMap_SSIF_SCK[] = {
//   pin      ch     func
    {P2_7   , 3    , 2},
    {NC     , NC   , 0}
};

static const PinMap PinMap_SSIF_WS[] = {
//   pin      ch     func
    {P2_9   , 3    , 2},
    {NC     , NC   , 0}
};

static const PinMap PinMap_SSIF_TxD[] = {
//   pin      ch     func
    {P2_8   , 3    , 2},
    {NC     , NC   , 0}
};

static const PinMap PinMap_SSIF_RxD[] = {
//   pin      ch     func
    {P2_6   , 3    , 2},
    {NC     , NC   , 0}
};

static const PinMap PinMap_SPDIF_AUDIO_CLK[] = {
    {NC,    NC,     0}
};

#endif

static void ssif_power_enable(uint32_t ssif_ch) {
    volatile uint8_t dummy;

    switch (ssif_ch) {
        case 0:
            CPGSTBCR11 &= ~(0x20);
            break;
        case 1:
            CPGSTBCR11 &= ~(0x10);
            break;
        case 2:
            CPGSTBCR11 &= ~(0x08);
            break;
        case 3:
            CPGSTBCR11 &= ~(0x04);
            break;
#if defined(TARGET_RZA1H) /* mbed */
        case 4:
            CPGSTBCR11 &= ~(0x02);
            break;
        case 5:
            CPGSTBCR11 &= ~(0x01);
            break;
#endif
    }
    dummy = CPGSTBCR11;
    (void)dummy;
}

#elif defined(TARGET_RZ_A2XX)

static const PinMap PinMap_SSIF_SCK[] = {
//   pin      ch     func
    {P9_6   , 0    , 5},
    {PF_4   , 0    , 5},
    {PE_3   , 0    , 3},
    {PB_2   , 1    , 4},
    {P4_0   , 1    , 4},
    {PK_0   , 2    , 5},
    {P9_2   , 2    , 5},
    {PB_5   , 2    , 4},
    {P8_7   , 3    , 5},
    {PJ_4   , 3    , 5},
    {NC     , NC   , 0}
};

static const PinMap PinMap_SSIF_WS[] = {
//   pin      ch     func
    {P9_5   , 0    , 5},
    {PE_4   , 0    , 3},
    {PF_5   , 0    , 5},
    {PB_1   , 1    , 4},
    {P4_3   , 1    , 4},
    {P9_1   , 2    , 5},
    {PB_4   , 2    , 4},
    {PJ_5   , 2    , 5},
    {P8_6   , 3    , 5},
    {PJ_3   , 3    , 5},
    {NC     , NC   , 0}
};

static const PinMap PinMap_SSIF_TxD[] = {
//   pin      ch     func
    {P9_4   , 0    , 5},
    {PE_5   , 0    , 3},
    {PF_6   , 0    , 5},
    {PB_0   , 1    , 4},
    {P4_2   , 1    , 4},
    {PK_1   , 2    , 5},
    {P9_0   , 2    , 5},
    {PB_3   , 2    , 4},
    {P8_5   , 3    , 5},
    {PJ_2   , 3    , 5},
    {NC     , NC   , 0}
};

static const PinMap PinMap_SSIF_RxD[] = {
//   pin      ch     func
    {P9_3   , 0    , 5},
    {PE_6   , 0    , 3},
    {PH_2   , 0    , 5},
    {PA_7   , 1    , 4},
    {P4_1   , 1    , 4},
    {PK_1   , 2    , 5},
    {P9_0   , 2    , 5},
    {PB_3   , 2    , 4},
    {P8_4   , 3    , 5},
    {PJ_0   , 3    , 5},
    {NC     , NC   , 0}
};

static void ssif_power_enable(uint32_t ssif_ch) {
    volatile uint8_t dummy;

    switch (ssif_ch) {
        case 0:
            CPG.STBCR7.BYTE &= ~(0x08);
            break;
        case 1:
            CPG.STBCR7.BYTE &= ~(0x04);
            break;
        case 2:
            CPG.STBCR7.BYTE &= ~(0x02);
            break;
        case 3:
            CPG.STBCR7.BYTE &= ~(0x01);
            break;
    }
    dummy = CPG.STBCR7.BYTE;
    (void)dummy;
}

static const PinMap PinMap_SPDIF_AUDIO_CLK[] = {
    {P6_4,  0,      4},
    {PH_0,  0,      1},
    {NC,    NC,     0}
};

#endif

int32_t ssif_init(PinName sck, PinName ws, PinName tx, PinName rx, PinName audio_clk) {
    /* determine the ssif to use */
    uint32_t ssif_sck = pinmap_peripheral(sck, PinMap_SSIF_SCK);
    uint32_t ssif_ws  = pinmap_peripheral(ws,  PinMap_SSIF_WS);
    uint32_t ssif_tx  = pinmap_peripheral(tx,  PinMap_SSIF_TxD);
    uint32_t ssif_rx  = pinmap_peripheral(rx,  PinMap_SSIF_RxD);
    uint32_t ssif_ch  = pinmap_merge(ssif_tx, ssif_rx);

    if ((ssif_ch == ssif_sck) && (ssif_ch == ssif_ws)) {
        ssif_power_enable(ssif_ch);
        pinmap_pinout(sck, PinMap_SSIF_SCK);
        pinmap_pinout(ws,  PinMap_SSIF_WS);
        pinmap_pinout(tx,  PinMap_SSIF_TxD);
        pinmap_pinout(rx,  PinMap_SSIF_RxD);
        if ((int32_t)audio_clk != NC) {
            pinmap_pinout(audio_clk, PinMap_SPDIF_AUDIO_CLK);
        }
    } else {
        ssif_ch = (uint32_t)NC;
    }

    return (int32_t)ssif_ch;
}

