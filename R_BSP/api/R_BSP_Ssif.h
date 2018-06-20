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
/**************************************************************************//**
* @file          R_BSP_Ssif.h
* @brief         SSIF API
******************************************************************************/

#ifndef R_BSP_SSIF_H
#define R_BSP_SSIF_H

#include <stdint.h>
#include "R_BSP_SerialFamily.h"
#include "R_BSP_SsifDef.h"
#include "rtos.h"
#include "pinmap.h"

/** A class to communicate a R_BSP_Ssif
 *
 */
class R_BSP_Ssif : public R_BSP_SerialFamily {

public:

    /** Constructor
     *
     * @param sck SSIF serial bit clock
     * @param ws  SSIF word selection
     * @param tx  SSIF serial data output
     * @param rx  SSIF serial data input
     */
    R_BSP_Ssif(PinName sck, PinName ws, PinName tx, PinName rx, PinName audio_clk = NC);

    /** Destructor
     *
     */
    virtual ~R_BSP_Ssif();


    /** Initialization
     *
     * @param p_ch_cfg SSIF channel configuration parameter
     * @param max_write_num The upper limit of write buffer (SSIF)
     * @param max_read_num  The upper limit of read buffer (SSIF)
     * @return true = success, false = failure
     */
    void init(const ssif_channel_cfg_t* const p_ch_cfg, int32_t max_write_num, int32_t max_read_num);

    /** Get a value of SSIF channel number
     *
     * @return SSIF channel number
     */
    int32_t GetSsifChNo(void) {
        return ssif_ch;
    };

    /** Save configuration to the SSIF driver
     *
     * @param p_ch_cfg SSIF channel configuration parameter
     * @return true = success, false = failure
     */
    bool ConfigChannel(const ssif_channel_cfg_t* const p_ch_cfg);

    /** Get a value of SSISR register
     *
     * @param p_status Pointer of status value
     * @return true = success, false = failure
     */
    bool GetStatus(uint32_t* const p_status);

private:
    int32_t ssif_ch;
};
#endif
