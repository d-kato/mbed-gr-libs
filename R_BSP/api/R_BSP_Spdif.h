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
/**************************************************************************//**
* @file          R_BSP_Spdif.h
* @brief         SPDIF API
******************************************************************************/


#ifndef R_BSP_SPDIF_H
#define R_BSP_SPDIF_H

#if defined(TARGET_RZ_A2XX)
#define R_BSP_SPDIF_ENABLE    1

#include <stdint.h>
#include "R_BSP_SerialFamily.h"
#include "R_BSP_SpdifDef.h"
#include "rtos.h"
#include "pinmap.h"


/** A class to communicate a R_BSP_Spdif
 *
 */
class R_BSP_Spdif : public R_BSP_SerialFamily {

public:

    /** Constructor
     *
     * @param audio_clk  audio clock
     * @param tx  SPDIF serial data output
     * @param rx  SPDIF serial data input
     */
    R_BSP_Spdif(PinName audio_clk, PinName tx, PinName rx);

    /** Destructor
     *
     */
    virtual ~R_BSP_Spdif();

    /** Initialization
     *
     * @param p_ch_cfg SPDIF channel configuration parameter
     * @param max_write_num The upper limit of write buffer (SPDIF)
     * @param max_read_num  The upper limit of read buffer (SPDIF)
     * @return true = success, false = failure
     */
    void init(const spdif_channel_cfg_t* const p_ch_cfg, int32_t max_write_num, int32_t max_read_num);

    /** Get a value of SPDIF channel number
     *
     * @return SPDIF channel number
     */
    int32_t GetSsifChNo(void) {
        return spdif_ch;
    };

    /** Save configuration to the SPDIF driver
     *
     * @param p_ch_cfg SPDIF channel configuration parameter
     * @return true = success, false = failure
     */
    bool ConfigChannel(const spdif_channel_cfg_t* const p_ch_cfg);

    bool SetChannelStatus(uint32_t status_Lch, uint32_t status_Rch);
    bool GetChannelStatus(uint32_t * p_status_Lch, uint32_t * p_status_Rch);
    bool SetTransAudioBit(int direction, char length);

private:
    int32_t spdif_ch;
};
#endif /* TARGET_RZ_A2XX */

#endif

