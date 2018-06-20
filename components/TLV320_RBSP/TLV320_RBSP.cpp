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

#include "mbed.h"
#include "TLV320_RBSP.h"
#include "pinmap.h"

#define MCLK_12MHZ
#undef  MCLK_11_2896MHZ

TLV320_RBSP::TLV320_RBSP(PinName cs, PinName sda, PinName scl, PinName sck, PinName ws, PinName tx, PinName rx,
    uint8_t int_level, int32_t max_write_num, int32_t max_read_num) 
       : audio_cs_(cs), mI2c_(sda, scl), mI2s_(sck, ws, tx, rx) {
    audio_cs_ = 0;
    mAddr     = 0x34;
    audio_path_control = 0;

    // I2S Mode
    ssif_cfg.enabled                = true;
    ssif_cfg.int_level              = int_level;
    ssif_cfg.slave_mode             = true;
    ssif_cfg.sample_freq            = 44100u;
    ssif_cfg.clk_select             = SSIF_CFG_CKS_AUDIO_X1;
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

    mI2c_.frequency(150000);
    reset();                       // TLV resets
    power(0x80);                   // Power off
    format(16);                    // 16Bit I2S protocol format
    frequency(44100);              // Default sample frequency is 44.1kHz
    bypass(false);                 // Do not bypass device
    mic(false);                    // Input select for ADC is line
    mute(false);                   // Not muted
    activateDigitalInterface_();   // The digital part of the chip is active
}

// Public Functions
bool TLV320_RBSP::inputVolume(float leftVolumeIn, float rightVolumeIn) {
    // check values are in range
    if ((leftVolumeIn  < 0.0) || (leftVolumeIn  > 1.0)
     || (rightVolumeIn < 0.0) || (rightVolumeIn > 1.0)) {
        return false;
    }

    // convert float to encoded char
    char left  = (char)(31 * leftVolumeIn);
    char right = (char)(31 * rightVolumeIn);

    // Left Channel
    cmd[0] = LEFT_LINE_INPUT_CHANNEL_VOLUME_CONTROL;
    cmd[1] = left | (0 << 7);    // set volume, mute off
    mI2c_.write(mAddr, cmd, 2);

    // Right Channel
    cmd[0] = RIGHT_LINE_INPUT_CHANNEL_VOLUME_CONTROL;
    cmd[1] = right | (0 << 7);   // set volume, mute off
    mI2c_.write(mAddr, cmd, 2);

    return true;
}

bool TLV320_RBSP::outputVolume(float leftVolumeOut, float rightVolumeOut) {
    // check values are in range
    if ((leftVolumeOut  < 0.0) || (leftVolumeOut  > 1.0)
     || (rightVolumeOut < 0.0) || (rightVolumeOut > 1.0)) {
        return false;
    }

    // convert float to encoded char
    char left  = (char)((79 * leftVolumeOut) + 0x30);
    char right = (char)((79 * rightVolumeOut) + 0x30);

    // Left Channel
    cmd[0] = LEFT_CHANNEL_HEADPHONE_VOLUME_CONTROL;
    cmd[1] = left | (1 << 7);    // set volume, Zero-cross detect
    mI2c_.write(mAddr, cmd, 2);

    // Right Channel
    cmd[0] = RIGHT_CHANNEL_HEADPHONE_VOLUME_CONTROL;
    cmd[1] = right | (1 << 7);   // set volume, Zero-cross detect
    mI2c_.write(mAddr, cmd, 2);

    return true;
}

void TLV320_RBSP::bypass(bool bypassVar) {
    cmd[0] = ANALOG_AUDIO_PATH_CONTROL;
    if (bypassVar != false) {
        audio_path_control |= (1 << 3);       // bypass enabled
        audio_path_control &= ~(1 << 4);      // DAC disable
    } else {
        audio_path_control &= ~(1 << 3);      // bypass disable
        audio_path_control |= (1 << 4);       // DAC enabled
    }
    cmd[1] = audio_path_control;
    mI2c_.write(mAddr, cmd, 2);
}

void TLV320_RBSP::mic(bool micVar) {
    cmd[0] = ANALOG_AUDIO_PATH_CONTROL;
    if (micVar != false) {
        audio_path_control |= (1 << 2);        // INSEL Microphone
    } else {
        audio_path_control &= ~(1 << 2);       // INSEL Line
    }
    cmd[1] = audio_path_control;
    mI2c_.write(mAddr, cmd, 2);
}

void TLV320_RBSP::micVolume(bool mute, bool boost) {
    cmd[0] = ANALOG_AUDIO_PATH_CONTROL;
    if (mute != false) {
        audio_path_control |= (1 << 1);        // MICM Muted
    } else {
        audio_path_control &= ~(1 << 1);       // MICM Normal
    }
    if (boost != false) {
        audio_path_control |= (1 << 0);        // MICB 20dB
    } else {
        audio_path_control &= ~(1 << 0);       // MICB 0dB
    }
    cmd[1] = audio_path_control;
    mI2c_.write(mAddr, cmd, 2);
}

void TLV320_RBSP::mute(bool softMute) {
    cmd[0] = DIGITAL_AUDIO_PATH_CONTROL;
    if (softMute != false) {
        cmd[1] = 0x08;           // set instruction to mute
    } else {
        cmd[1] = 0x00;           // set instruction to NOT mute
    }
    mI2c_.write(mAddr, cmd, 2);
}

void TLV320_RBSP::power(int device) {
    cmd[0] = POWER_DOWN_CONTROL;
    cmd[1] = (char)device;       // set user defined commands
    mI2c_.write(mAddr, cmd, 2);
}

bool TLV320_RBSP::format(char length) {
    cmd[0] = DIGITAL_AUDIO_INTERFACE_FORMAT;
    cmd[1] = (1 << 6);           // Master
    switch (length) {            // input data into instruction byte
        case 16:
            ssif_cfg.data_word = SSIF_CFG_DATA_WORD_16;
            cmd[1] |= 0x02;
            break;
        case 20:
            ssif_cfg.data_word = SSIF_CFG_DATA_WORD_20;
            cmd[1] |= 0x06;
            break;
        case 24:
            ssif_cfg.data_word = SSIF_CFG_DATA_WORD_24;
            cmd[1] |= 0x0A;
            break;
        case 32:
            ssif_cfg.data_word = SSIF_CFG_DATA_WORD_32;
            cmd[1] |= 0x0E;
            break;
        default:
            return false;
    }
    mI2s_.ConfigChannel(&ssif_cfg);
    mI2c_.write(mAddr, cmd, 2);

    return true;
}

bool TLV320_RBSP::frequency(int hz) {
    char control_setting;

#if defined(MCLK_12MHZ)
    switch (hz) {
        case 96000:
            control_setting = 0x1D;
            break;
        case 88200:
            control_setting = 0x3F;
            break;
        case 48000:
            control_setting = 0x01;
            break;
        case 44100:
            control_setting = 0x23;
            break;
        case 32000:
            control_setting = 0x19;
            break;
        case 8021:
            control_setting = 0x2F;
            break;
        case 8000:
            control_setting = 0x0D;
            break;
        default:
            return false;
    }
#elif defined(MCLK_11_2896MHZ)
    switch (hz) {
        case 88200:
            control_setting = 0x3C;
            break;
        case 44100:
            control_setting = 0x20;
            break;
        case 8021:
            control_setting = 0x2C;
            break;
        default:
            return false;
    }
#else
    #error MCLK error
#endif
    cmd[0] = SAMPLE_RATE_CONTROL;
    cmd[1] = control_setting;
    mI2c_.write(mAddr, cmd, 2);

    return true;
}

void TLV320_RBSP::reset(void) {
    cmd[0] = RESET_REGISTER;
    cmd[1] = 0x00;               // Write 000000000 to this register triggers reset
    mI2c_.write(mAddr, cmd, 2);
}

// Private Functions
void TLV320_RBSP::activateDigitalInterface_(void) {
    cmd[0] = DIGITAL_INTERFACE_ACTIVATION;
    cmd[1] = 0x01;               // Activate
    mI2c_.write(mAddr, cmd, 2);
}
