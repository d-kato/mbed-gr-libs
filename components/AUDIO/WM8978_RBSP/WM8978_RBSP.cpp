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
#include "WM8978_RBSP.h"
#include "pinmap.h"


WM8978_RBSP::WM8978_RBSP(PinName mosi, PinName miso, PinName sclk, PinName ssel,
    PinName sck, PinName ws, PinName tx, PinName rx, PinName audio_clk,
    uint8_t int_level, int32_t max_write_num, int32_t max_read_num) 
       : mSpi_(mosi, miso, sclk, ssel), mI2s_(sck, ws, tx, rx, audio_clk) {

    // I2S Mode
    ssif_cfg.enabled                = true;
    ssif_cfg.int_level              = int_level;
    if ((int32_t)audio_clk == NC) {
        ssif_cfg.slave_mode         = true;
    } else {
        ssif_cfg.slave_mode         = false;
    }
    ssif_cfg.sample_freq            = 44100u;
    ssif_cfg.clk_select             = SSIF_CFG_CKS_AUDIO_CLK;
    ssif_cfg.multi_ch               = SSIF_CFG_MULTI_CH_1;
    ssif_cfg.data_word              = SSIF_CFG_DATA_WORD_16;
    ssif_cfg.system_word            = SSIF_CFG_SYSTEM_WORD_32;
    ssif_cfg.bclk_pol               = SSIF_CFG_FALLING;
    ssif_cfg.ws_pol                 = SSIF_CFG_WS_LOW;
    ssif_cfg.padding_pol            = SSIF_CFG_PADDING_LOW;
    ssif_cfg.serial_alignment       = SSIF_CFG_DATA_FIRST;
    ssif_cfg.parallel_alignment     = SSIF_CFG_LEFT;
    ssif_cfg.ws_delay               = SSIF_CFG_DELAY;
    ssif_cfg.noise_cancel           = SSIF_CFG_ENABLE_NOISE_CANCEL;
    ssif_cfg.tdm_mode               = SSIF_CFG_DISABLE_TDM;
    ssif_cfg.romdec_direct.mode     = SSIF_CFG_DISABLE_ROMDEC_DIRECT;
    ssif_cfg.romdec_direct.p_cbfunc = NULL;
    mI2s_.init(&ssif_cfg, max_write_num, max_read_num);

    mSpi_.format(16);

    activateDigitalInterface_();

    power(false);                  // Power off
}

// Public Functions
bool WM8978_RBSP::outputVolume(float leftVolumeOut, float rightVolumeOut) {
    // check values are in range
    if ((leftVolumeOut  < 0.0) || (leftVolumeOut  > 1.0)
     || (rightVolumeOut < 0.0) || (rightVolumeOut > 1.0)) {
        return false;
    }
    return true;
}

bool WM8978_RBSP::micVolume(float VolumeIn) {
    // check values are in range
    if ((VolumeIn < 0.0) || (VolumeIn > 1.0)) {
        return false;
    }
    return true;
}

void WM8978_RBSP::power(bool type) {
    
}

bool WM8978_RBSP::format(char length) {
    if (length != 16) {
        return false;
    }
    return true;
}

bool WM8978_RBSP::frequency(int hz) {
    if (hz != 44100) {
        return false;
    }
    return true;
}

// Private Functions
void WM8978_RBSP::activateDigitalInterface_(void) {
    uint32_t        reg_cmd;
    uint32_t        reg_tmp;

    /* ==== Soft Reset. ==== */
    set_register(WM8978_REGADR_SOFT_RESET, WM8978_RESET_INI_VALUE);

    /* ==== Set L/RMIXEN = 1 and DACENL/R = 1 in register R3.  ==== */
    reg_cmd  = WM8978_MANAGE3_INI_VALUE;
    reg_cmd |= (WM8978_MANAGE3_RMIXEN_ON   |
                WM8978_MANAGE3_LMIXEN_ON   |
                WM8978_MANAGE3_DACENL_ON   |
                WM8978_MANAGE3_DACENR_ON);
    set_register(WM8978_REGADR_POW_MANAGE3, reg_cmd);

    /* ==== Set BUFIOEN = 1 and VMIDSEL[1:0] to required value in register R1. ==== */
    reg_cmd  = WM8978_MANAGE1_INI_VALUE;
    reg_cmd |= (WM8978_MANAGE1_BUFIOEN_ON  |
                WM8978_MANAGE1_VMIDSEL_75K |
                WM8978_MANAGE1_MICBEN_ON   |
                WM8978_MANAGE1_PLLEN_ON);
    set_register(WM8978_REGADR_POW_MANAGE1, reg_cmd);

    /* ==== Set BIASEN = 1 in register R1.  ==== */
    reg_cmd |= WM8978_MANAGE1_BIASEN_ON;
    set_register(WM8978_REGADR_POW_MANAGE1, reg_cmd);

    /* ==== Set L/ROUT1EN = 1 in register R2.  ==== */
    reg_cmd  = WM8978_MANAGE2_INI_VALUE;
    reg_cmd |= (WM8978_MANAGE2_LOUT1EN_ON  |
                WM8978_MANAGE2_ROUT1EN_ON  |
                WM8978_MANAGE2_BOOSTENL_ON |
                WM8978_MANAGE2_BOOSTENR_ON);
    set_register(WM8978_REGADR_POW_MANAGE2, reg_cmd);

    /* ==== Set INPPGAENL = 1 in register R2.  ==== */
    reg_cmd |= WM8978_MANAGE2_INPPGAENL_ON;
    set_register(WM8978_REGADR_POW_MANAGE2, reg_cmd);

    /* ==== Set INPPGAENR = 1 in register R2.  ==== */
    reg_cmd |= WM8978_MANAGE2_INPPGAENR_ON;
    reg_tmp = reg_cmd;
    set_register(WM8978_REGADR_POW_MANAGE2, reg_cmd);

    /* ==== Set L2_2INPPGA = 1 and R2_2INPPGA = 1 in register R44.  ==== */
    reg_cmd  = WM8978_INPUTCTL_INI_VALUE;
    reg_cmd |= (WM8978_INPUTCTL_L2_2INPPGA_ON | WM8978_INPUTCTL_R2_2INPPGA_ON);
    set_register(WM8978_REGADR_INPUT_CTL, reg_cmd);

    /* ==== Set LINPPGAGAIN = 0x0018 and MUTEL = 0 in register R45.  ==== */
    reg_cmd  =   WM8978_LINPPGAGAIN_INI_VOLL;
    reg_cmd  &= ~WM8978_LINPPGAGAIN_MUTEL_ON;
    set_register(WM8978_REGADR_LINPPGAGAIN, reg_cmd);

    /* ==== Set RINPPGAGAIN = 0x0018 and MUTEL = 0 in register R46.  ==== */
    reg_cmd  =   WM8978_RINPPGAGAIN_INI_VOLL;
    reg_cmd  &= ~WM8978_RINPPGAGAIN_MUTER_BIT;
    set_register(WM8978_REGADR_RINPPGAGAIN, reg_cmd);

    /* ==== Set ADCENL/ADCENR = 1 in register R2.  ==== */
    reg_cmd = reg_tmp;
    reg_cmd |= (WM8978_MANAGE2_ADCENL_ON | WM8978_MANAGE2_ADCENR_ON);
    set_register(WM8978_REGADR_POW_MANAGE2, reg_cmd);

    /* ==== Set ADCOSR128 = 1 in register R14.  ==== */
    reg_cmd  = WM8978_ADC_CTL_INI_VALUE;
    reg_cmd |= WM8978_ADC_CTL_ADCOSR128_ON;
    set_register(WM8978_REGADR_ADC_CTL, reg_cmd);

    /* ==== Set HPFEN = 0 in register R14.  ==== */
    reg_cmd  = WM8978_ADC_CTL_INI_VALUE;
    reg_cmd |= WM8978_ADC_CTL_ADCOSR128_ON;
    reg_cmd &= ~WM8978_ADC_CTL_HPFEN_BIT;
    set_register(WM8978_REGADR_ADC_CTL, reg_cmd);

    /* ==== Set MCLKDIV = 0 in register R6.  ==== */
    if (ssif_cfg.slave_mode != false) {
        reg_cmd  =   WM8978_CLK_GEN_CTL_INI_VALUE;
        reg_cmd |=   (0x2 << 2);
        reg_cmd |=   0x01;
    } else {
        reg_cmd  =   WM8978_CLK_GEN_CTL_INI_VALUE;
        reg_cmd &=  ~WM8978_CLK_GEN_CTL_MCLKDIV_BIT;
        reg_cmd |=   WM8978_CLK_GEN_CTL_MCLKDIV_DIV1;
    }
    set_register(WM8978_REGADR_CLK_GEN_CTL, reg_cmd);

    /* ==== Set WL = 0 in register R4.  ==== */
    reg_cmd  =   WM8978_AUDIO_IF_INI_VALUE;
    reg_cmd &=  ~WM8978_AUDIO_IF_WL_BIT;
    reg_cmd |=   WM8978_AUDIO_IF_WL_16BIT;
    set_register(WM8978_REGADR_AUDIO_IF_CTL, reg_cmd);

    /* ==== Set DACOSR128 = 1 in register R10.  ==== */
    reg_cmd  =   WM8978_DAC_CTL_INI_VALUE;
    reg_cmd |=   WM8978_DAC_CTL_DACOSR128_ON;
    set_register(WM8978_REGADR_DAC_CTL, reg_cmd);
}

void WM8978_RBSP::set_register(uint8_t reg_addr, uint16_t reg_cmd) {
    uint32_t reg_data;

    reg_data = (((reg_addr << 9) & 0x0000FE00u) | (reg_cmd & 0x000001FFu));
    mSpi_.write(reg_data);
}
