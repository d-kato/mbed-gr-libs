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
* Copyright (C) 2017 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

#include "mbed.h"
#include "MAX9867_RBSP.h"
#include "pinmap.h"

MAX9867_RBSP::MAX9867_RBSP(PinName sda, PinName scl, PinName sck, PinName ws, PinName tx, PinName rx,
    uint8_t int_level, int32_t max_write_num, int32_t max_read_num) 
       : mI2c_(sda, scl), mI2s_(sck, ws, tx, rx) {
    mAddr     = 0x30;

    // I2S Mode
    ssif_cfg.enabled                = true;
    ssif_cfg.int_level              = int_level;
    ssif_cfg.slave_mode             = true;
    ssif_cfg.sample_freq            = 44100u;
    ssif_cfg.clk_select             = SSIF_CFG_CKS_AUDIO_X1;
    ssif_cfg.multi_ch               = SSIF_CFG_MULTI_CH_1;
    ssif_cfg.data_word              = SSIF_CFG_DATA_WORD_16;
    ssif_cfg.system_word            = SSIF_CFG_SYSTEM_WORD_24;
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
    power(false);                  // Power off
    frequency(44100);              // Default sample frequency is 44.1kHz
    activateDigitalInterface_();   // The digital part of the chip is active
}

// Public Functions
bool MAX9867_RBSP::outputVolume(float leftVolumeOut, float rightVolumeOut) {
    // check values are in range
    if ((leftVolumeOut  < 0.0) || (leftVolumeOut  > 1.0)
     || (rightVolumeOut < 0.0) || (rightVolumeOut > 1.0)) {
        return false;
    }

    // convert float to encoded char
    char left  = (char)(40 - (40 * leftVolumeOut));
    char right = (char)(40 - (40 * rightVolumeOut));

    cmd[0] = 0x10;  // Playback Volume Registers
    cmd[1] = left;
    cmd[2] = right;
    mI2c_.write(mAddr, cmd, 3);

    return true;
}

bool MAX9867_RBSP::micVolume(float VolumeIn) {
    // check values are in range
    if ((VolumeIn < 0.0) || (VolumeIn > 1.0)) {
        return false;
    }

    // convert float to encoded char
    char vol  = (char)(50 * VolumeIn);

    cmd[0] = 0x12;  // Microphone Input Registers
    if (vol == 0) {
        cmd[1] = 0x00;             // PAREN=00,PGAMR=0x00
    } else if (vol <= 20) {
        cmd[1] = 0x20 + 20 - vol;  // PAREN=01,PGAMR=20-vol
    } else if (vol <= 40) {
        cmd[1] = 0x40 + 40 - vol;  // PAREN=10,PGAMR=40-vol
    } else {
        cmd[1] = 0x60 + 50 - vol;  // PAREN=11,PGAMR=50-vol
    }
    cmd[2] = 0x00;  // Right Microphone Gain PAREN=00,PGAMR=0x00
    mI2c_.write(mAddr, cmd, 3);

    return true;
}

void MAX9867_RBSP::power(bool type) {
    cmd[0] = 0x17; // Power-Management Register
    if (type) {
        cmd[1] = 0x8E;  // SHDN=1,LNLEN=0,LNREN=0,DALEN=1,DAREN=1,ADLEN=1,ADREN=0
    } else {
        cmd[1] = 0x80;  // SHDN=1,LNLEN=0,LNREN=0,DALEN=0,DAREN=0,ADLEN=0,ADREN=0
    }
    mI2c_.write(mAddr, cmd, 2);
}

bool MAX9867_RBSP::format(char length) {
    if (length != 16) {
        return false;
    }
    return true;
}

bool MAX9867_RBSP::frequency(int hz) {
    uint16_t lrclk;

    switch (hz) {
        case 48000:
            lrclk = 0x624E;
            break;
        case 44100:
            lrclk = 0x5A51;
            break;
        case 32000:
            lrclk = 0x4189;
            break;
        case 24000:
            lrclk = 0x3127;
            break;
        case 16000:
            lrclk = 0x20C5;
            break;
        case 8000:
            lrclk = 0x1062;
            break;
        default:
            return false;
    }

    cmd[0] = 0x05;  // Clock Control Registers
    cmd[1] = 0x10;  // PSCLK = 00  FREQ = 0x0
    cmd[2] = (uint8_t)(lrclk >> 8);
    cmd[3] = (uint8_t)(lrclk);
    mI2c_.write(mAddr, cmd, 4);

    return true;
}

// Private Functions
void MAX9867_RBSP::activateDigitalInterface_(void) {
    cmd[0] = 0x04;  // Interrupt Enable
    cmd[1] = 0x00;  // ICLE=0,ISLD=0,IULK=0,SDODLY=0,IJDET=0
    mI2c_.write(mAddr, cmd, 2);

    cmd[0] = 0x08;  // Interface Mode
    cmd[1] = 0x90;  // MAS=1,WCI=0,BCI=0,DLY=1,HIZOFF=0,TDM=0
    cmd[2] = 0x02;  // LVOLFIX=0,DMONO=0,BSEL=010
    mI2c_.write(mAddr, cmd, 3);

    cmd[0] = 0x0B;  // Digital Gain Control
    cmd[1] = 0x00;  // DSTS=0,DVST=0
    mI2c_.write(mAddr, cmd, 2);

    cmd[0] = 0x0E;  // Line Input Registers
    cmd[1] = 0x40;  // LILM=1,LIGL=0x00
    cmd[2] = 0x40;  // LIRM=1,LIGR=0x00
    mI2c_.write(mAddr, cmd, 3);

    cmd[0] = 0x12;  // Microphone Input Registers
    cmd[1] = 0x00;  // Left Microphone Gain PAREN=00,PGAMR=0x00
    cmd[2] = 0x00;  // Right Microphone Gain PAREN=00,PGAMR=0x00
    mI2c_.write(mAddr, cmd, 3);

    cmd[0] = 0x14;  // CONFIGURATION
    cmd[1] = 0x40;  // MXINL=01,MXINR=00,AUXCAP=0,AUXGAIN=0,AUXCAL=0,AUXEN=0
    cmd[2] = 0x00;  // MICCLK=0,DIGMICL=0,DIGMICR=0
    cmd[3] = 0x02;  // DSLEW=0,VSEN=0,ZDEN=0,JDETEN=0,HPMODE=000
    mI2c_.write(mAddr, cmd, 4);
}
