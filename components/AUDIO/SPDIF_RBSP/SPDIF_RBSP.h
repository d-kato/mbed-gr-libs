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

#ifndef MBED_SPDIF_RBSP_H
#define MBED_SPDIF_RBSP_H

#include "mbed.h"
#include "AUDIO_RBSP.h"
#include "R_BSP_Spdif.h"

#if (R_BSP_SPDIF_ENABLE == 1)
/** SPDIF_RBSP class, defined on the I2C master bus
*
*/
class SPDIF_RBSP : public AUDIO_RBSP {
public:

    /** Create a SPDIF_RBSP object defined on the I2C port
     * 
     * @param audio_clk  audio clock
     * @param tx  SPDIF serial data output
     * @param rx  SPDIF serial data input
     * @param int_level     Interupt priority (SSIF)
     * @param max_write_num The upper limit of write buffer (SSIF)
     * @param max_read_num  The upper limit of read buffer (SSIF)
     */
    SPDIF_RBSP(PinName audio_clk, PinName tx, PinName rx, bool tx_udata_enable = false,
                uint8_t int_level = 0x80, int32_t max_write_num = 16, int32_t max_read_num = 16);

    virtual ~SPDIF_RBSP() {}

    /** Overloaded power()
     *
     * @param type true=power up, false=power down
     */
    virtual void power(bool type = true);

    /** Set I2S interface bit length and mode
     *
     * @param length Set bit length to 16 bits
     * @return true = success, false = failure
     */
    virtual bool format(char length);

    /** Set sample frequency
     *
     * @param frequency Sample frequency of data in Hz
     * @return true = success, false = failure
     * 
     * supports frequencies: 44.1kHz
     * Default is 44.1kHz
     */
    virtual bool frequency(int hz);

    /** Get a value of SSIF channel number
     *
     * @return SPDIF channel number
     */
    int32_t GetSsifChNo(void) {
        return mSpdif_.GetSsifChNo();
    };

    /** Enqueue asynchronous write request
     *
     * @param p_data Location of the data
     * @param data_size Number of bytes to write
     * @param p_data_conf Asynchronous control block structure
     * @return Number of bytes written on success. negative number on error.
     */
    virtual int write(void * const p_data, uint32_t data_size, const rbsp_data_conf_t * const p_data_conf = NULL);

    int write_s(spdif_t * const p_spdif_data, const rbsp_data_conf_t * const p_data_conf = NULL);

    /** Enqueue asynchronous read request
     *
     * @param p_data Location of the data
     * @param data_size Number of bytes to read
     * @param p_data_conf Asynchronous control block structure
     * @return Number of bytes read on success. negative number on error.
     */
    virtual int read(void * const p_data, uint32_t data_size, const rbsp_data_conf_t * const p_data_conf = NULL);

    int read_s(spdif_t * const p_spdif_data, const rbsp_data_conf_t * const p_data_conf = NULL);

    /** Headphone out volume control
     *
     * @param leftVolumeOut Left headphone-out volume
     * @param rightVolumeOut Right headphone-out volume
     * @return Returns "true" for success, "false" if parameters are out of range
     */
    virtual bool outputVolume(float leftVolumeOut, float rightVolumeOut);

    /** Microphone volume
     *
     * @param VolumeIn Microphone volume
     * @return Returns "true" for success, "false" if parameters are out of range
     */
    virtual bool micVolume(float VolumeIn);

private:
    R_BSP_Spdif mSpdif_;
    spdif_channel_cfg_t  spdif_cfg;

    /******************************************************************************
    Macro definitions
    ******************************************************************************/

};
#endif /* R_BSP_SPDIF_ENABLE */

#endif
