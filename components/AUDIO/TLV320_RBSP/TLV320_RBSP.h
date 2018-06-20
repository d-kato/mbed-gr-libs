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

#ifndef MBED_TLV320_RBSP_H
#define MBED_TLV320_RBSP_H

#include "mbed.h"
#include "AUDIO_RBSP.h"
#include "R_BSP_Ssif.h"

/** TLV320_RBSP class, defined on the I2C master bus
*
*/
class TLV320_RBSP : public AUDIO_RBSP {
public:

    /** Create a TLV320_RBSP object defined on the I2C port
     * 
     * @param cs  Control port input latch/address select (codec pin)
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
    TLV320_RBSP(PinName cs, PinName sda, PinName scl, PinName sck, PinName ws, PinName tx, PinName rx, uint8_t int_level = 0x80, int32_t max_write_num = 16, int32_t max_read_num = 16);

    virtual ~TLV320_RBSP() {}

    /** Overloaded power() function default = 0x80, record requires 0x02
     *
     * @param device Call individual devices to power up/down
     * Device power      0x00 = On 0x80 = Off
     * Clock             0x00 = On 0x40 = Off
     * Oscillator        0x00 = On 0x20 = Off
     * Outputs           0x00 = On 0x10 = Off
     * DAC               0x00 = On 0x08 = Off
     * ADC               0x00 = On 0x04 = Off
     * Microphone input  0x00 = On 0x02 = Off
     * Line input        0x00 = On 0x01 = Off
     */
    void power(int device);

    /** Overloaded power()
     *
     * @param type true=power up, false=power down
     */
    virtual void power(bool type = true) {
        if (type) {
            power(0x00);
        } else {
            power(0x80);
        }
    }

    /** Set I2S interface bit length and mode
     *
     * @param length Set bit length to 16, 20, 24 or 32 bits
     * @return true = success, false = failure
     */
    virtual bool format(char length);

    /** Set sample frequency
     *
     * @param frequency Sample frequency of data in Hz
     * @return true = success, false = failure
     * 
     * The TLV320 supports the following frequencies: 8kHz, 8.021kHz, 32kHz, 44.1kHz, 48kHz, 88.2kHz, 96kHz
     * Default is 44.1kHz
     */
    virtual bool frequency(int hz);

    /** Reset TLV320
     *
     */
    void reset(void);

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

    /** Line in volume control i.e. record volume
     *
     * @param leftVolumeIn Left line-in volume 
     * @param rightVolumeIn Right line-in volume
     * @return Returns "true" for success, "false" if parameters are out of range
     * Parameters accept a value, where 0.0 < parameter < 1.0 and where 0.0 maps to -34.5dB 
     * and 1.0 maps to +12dB (0.74 = 0 dB default).
     */
    bool inputVolume(float leftVolumeIn, float rightVolumeIn);

    /** Headphone out volume control
     *
     * @param leftVolumeOut Left headphone-out volume
     * @param rightVolumeOut Right headphone-out volume
     * @return Returns "true" for success, "false" if parameters are out of range
     * Parameters accept a value, where 0.0 < parameter < 1.0 and where 0.0 maps to -73dB (mute) 
     * and 1.0 maps to +6dB (0.5 = default)
     */
    virtual bool outputVolume(float leftVolumeOut, float rightVolumeOut);

    /** Analog audio path control (Bypass) function default = false
     *
     * @param bypassVar Route analogue audio direct from line in to headphone out
     */
    void bypass(bool bypassVar);

    /** Analog audio path control (Input select for ADC) function default = false
     *
     * @param micVar Input select for ADC. true : Microphone , false : Line
     */
    void mic(bool micVar);

    /** Microphone volume
     *
     * @param mute Microphone mute. true : mute , false : normal
     * @param boost Microphone boost. true : 20dB , false : 0dB
     */
    void micVolume(bool mute, bool boost);

    /** Microphone volume
     *
     * @param VolumeIn Microphone volume
     * @return Returns "true" for success, "false" if parameters are out of range
     */
    virtual bool micVolume(float VolumeIn) {
        if (VolumeIn > 0) {
            if (VolumeIn >= 0.5) {
                micVolume(false , true);
            } else {
                micVolume(false, false);
            }
        } else {
            micVolume(true, false);
        }
        return true;
    }

    /** Digital audio path control
     *
     * @param softMute Mute output
     */
    void mute(bool softMute);

protected:
    char cmd[2];    // the address and command for TLV320 internal registers
    int mAddr;      // register write address
private:
    DigitalOut audio_cs_;
    I2C mI2c_;      // MUST use the I2C port
    R_BSP_Ssif mI2s_;
    ssif_channel_cfg_t  ssif_cfg;
    char audio_path_control;
    /** Digital interface activation
     *
     */
    void activateDigitalInterface_(void);

    // TLV320AIC23B register addresses as defined in the TLV320AIC23B datasheet
    #define LEFT_LINE_INPUT_CHANNEL_VOLUME_CONTROL  (0x00 << 1)
    #define RIGHT_LINE_INPUT_CHANNEL_VOLUME_CONTROL (0x01 << 1)
    #define LEFT_CHANNEL_HEADPHONE_VOLUME_CONTROL   (0x02 << 1)
    #define RIGHT_CHANNEL_HEADPHONE_VOLUME_CONTROL  (0x03 << 1)
    #define ANALOG_AUDIO_PATH_CONTROL               (0x04 << 1)
    #define DIGITAL_AUDIO_PATH_CONTROL              (0x05 << 1)
    #define POWER_DOWN_CONTROL                      (0x06 << 1)
    #define DIGITAL_AUDIO_INTERFACE_FORMAT          (0x07 << 1)
    #define SAMPLE_RATE_CONTROL                     (0x08 << 1)
    #define DIGITAL_INTERFACE_ACTIVATION            (0x09 << 1)
    #define RESET_REGISTER                          (0x0F << 1)
};

#endif
