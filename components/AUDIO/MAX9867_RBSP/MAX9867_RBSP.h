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

#ifndef MBED_MAX9867_RBSP_H
#define MBED_MAX9867_RBSP_H

#include "mbed.h"
#include "AUDIO_RBSP.h"
#include "R_BSP_Ssif.h"

/** MAX9867_RBSP class, defined on the I2C master bus
*
*/
class MAX9867_RBSP : public AUDIO_RBSP {
public:

    /** Create a MAX9867_RBSP object defined on the I2C port
     * 
     * @param sda I2C data line pin
     * @param scl I2C clock line pin
     * @param sck SSIF serial bit clock
     * @param ws  SSIF word selection
     * @param tx  SSIF serial data output
     * @param rx  SSIF serial data input
     * @param int_level     Interupt priority (SSIF)
     * @param max_write_num The upper limit of write buffer (SSIF)
     * @param max_read_num  The upper limit of read buffer (SSIF)
     */
    MAX9867_RBSP(PinName sda, PinName scl, PinName sck, PinName ws, PinName tx, PinName rx, uint8_t int_level = 0x80, int32_t max_write_num = 16, int32_t max_read_num = 16);

    virtual ~MAX9867_RBSP() {}

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
     * The TLV320 supports the following frequencies: 8kHz, 8.021kHz, 32kHz, 44.1kHz, 48kHz
     * Default is 44.1kHz
     */
    virtual bool frequency(int hz);

    /** Get a value of SSIF channel number
     *
     * @return SSIF channel number
     */
    int32_t GetSsifChNo(void) {
        return mI2s_.GetSsifChNo();
    };

    /** Enqueue asynchronous write request
     *
     * @param p_data Location of the data
     * @param data_size Number of bytes to write
     * @param p_data_conf Asynchronous control block structure
     * @return Number of bytes written on success. negative number on error.
     */
    virtual int write(void * const p_data, uint32_t data_size, const rbsp_data_conf_t * const p_data_conf = NULL) {
        return mI2s_.write(p_data, data_size, p_data_conf);
    };

    /** Enqueue asynchronous read request
     *
     * @param p_data Location of the data
     * @param data_size Number of bytes to read
     * @param p_data_conf Asynchronous control block structure
     * @return Number of bytes read on success. negative number on error.
     */
    virtual int read(void * const p_data, uint32_t data_size, const rbsp_data_conf_t * const p_data_conf = NULL) {
        return mI2s_.read(p_data, data_size, p_data_conf);
    };

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

protected:
    char cmd[4];    // the address and command for MAX9867 internal registers
    int mAddr;      // register write address
private:
    I2C mI2c_;      // MUST use the I2C port
    R_BSP_Ssif mI2s_;
    ssif_channel_cfg_t  ssif_cfg;
    /** Digital interface activation
     *
     */
    void activateDigitalInterface_(void);

};

#endif
