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

#ifndef MBED_WM8978_RBSP_H
#define MBED_WM8978_RBSP_H

#include "mbed.h"
#include "AUDIO_RBSP.h"
#include "R_BSP_Ssif.h"

/** WM8978_RBSP class, defined on the I2C master bus
*
*/
class WM8978_RBSP : public AUDIO_RBSP {
public:

    /** Create a WM8978_RBSP object defined on the I2C port
     * 
     * @param mosi SPI Master Out, Slave In pin
     * @param miso SPI Master In, Slave Out pin
     * @param sclk SPI Clock pin
     * @param ssel SPI chip select pin
     * @param sck SSIF serial bit clock
     * @param ws  SSIF word selection
     * @param tx  SSIF serial data output
     * @param rx  SSIF serial data input
     * @param audio_clk  audio clock
     * @param int_level     Interupt priority (SSIF)
     * @param max_write_num The upper limit of write buffer (SSIF)
     * @param max_read_num  The upper limit of read buffer (SSIF)
     */
    WM8978_RBSP(PinName mosi, PinName miso, PinName sclk, PinName ssel,
                PinName sck, PinName ws, PinName tx, PinName rx, PinName audio_clk,
                uint8_t int_level = 0x80, int32_t max_write_num = 16, int32_t max_read_num = 16);

    virtual ~WM8978_RBSP() {}

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

private:
    SPI mSpi_; 
    R_BSP_Ssif mI2s_;
    ssif_channel_cfg_t  ssif_cfg;
    /** Digital interface activation
     *
     */
    void activateDigitalInterface_(void);
    void set_register(uint8_t reg_addr, uint16_t reg_cmd);

    /******************************************************************************
    Macro definitions
    ******************************************************************************/
    #define WM8978_CALLBACK (0) /* 1:Callback at WM8978_RBSP, 0: Callback at SSIF */

    /* ==== Register Address ==== */
    #define WM8978_REGADR_SOFT_RESET        (0x00u)
    #define WM8978_REGADR_POW_MANAGE1       (0x01u)
    #define WM8978_REGADR_POW_MANAGE2       (0x02u)
    #define WM8978_REGADR_POW_MANAGE3       (0x03u)
    #define WM8978_REGADR_AUDIO_IF_CTL      (0x04u)
    #define WM8978_REGADR_CLK_GEN_CTL       (0x06u)
    #define WM8978_REGADR_DAC_CTL           (0x0Au)
    #define WM8978_REGADR_ADC_CTL           (0x0Eu)
    #define WM8978_REGADR_INPUT_CTL         (0x2Cu)
    #define WM8978_REGADR_LINPPGAGAIN       (0x2Du)
    #define WM8978_REGADR_RINPPGAGAIN       (0x2Eu)
    #define WM8978_REGADR_LMIXER_CTL        (0x32u)
    #define WM8978_REGADR_RMIXER_CTL        (0x33u)
    #define WM8978_REGADR_LOUT1_VOL_CTL     (0x34u)
    #define WM8978_REGADR_ROUT1_VOL_CTL     (0x35u)

    /* ==== Register Setting Value ==== */
    /* RESET R0 */
    #define WM8978_RESET_INI_VALUE          (0x0000u)
    /* WM8978_REGADR_POW_MANAGE1 R1 */
    #define WM8978_MANAGE1_INI_VALUE        (0x0000u)
    #define WM8978_MANAGE1_VMIDSEL_75K      (0x0001u)
    #define WM8978_MANAGE1_BUFIOEN_ON       (0x0004u)
    #define WM8978_MANAGE1_BIASEN_ON        (0x0008u)
    #define WM8978_MANAGE1_MICBEN_ON        (0x0010u)
    #define WM8978_MANAGE1_PLLEN_ON         (0x0020u)
    /* WM8978_REGADR_POW_MANAGE2 R2 */
    #define WM8978_MANAGE2_INI_VALUE        (0x0000u)
    #define WM8978_MANAGE2_ADCENL_ON        (0x0001u)
    #define WM8978_MANAGE2_ADCENR_ON        (0x0002u)
    #define WM8978_MANAGE2_INPPGAENL_ON     (0x0004u)
    #define WM8978_MANAGE2_INPPGAENR_ON     (0x0008u)
    #define WM8978_MANAGE2_BOOSTENL_ON      (0x0010u)
    #define WM8978_MANAGE2_BOOSTENR_ON      (0x0020u)
    #define WM8978_MANAGE2_LOUT1EN_ON       (0x0080u)
    #define WM8978_MANAGE2_ROUT1EN_ON       (0x0100u)
    /* WM8978_REGADR_POW_MANAGE3 R3 */
    #define WM8978_MANAGE3_INI_VALUE        (0x0000u)
    #define WM8978_MANAGE3_DACENL_ON        (0x0001u)
    #define WM8978_MANAGE3_DACENR_ON        (0x0002u)
    #define WM8978_MANAGE3_LMIXEN_ON        (0x0004u)
    #define WM8978_MANAGE3_RMIXEN_ON        (0x0008u)
    /* WM8978_REGADR_AUDIO_IF_CTL R4 */
    #define WM8978_AUDIO_IF_INI_VALUE       (0x0050u)
    #define WM8978_AUDIO_IF_WL_BIT          (0x0060u)
    #define WM8978_AUDIO_IF_WL_16BIT        (0x0000u)
    /* WM8978_REGADR_CLK_GEN_CTL  R6 */
    #define WM8978_CLK_GEN_CTL_INI_VALUE    (0x0040u)
    #define WM8978_CLK_GEN_CTL_MCLKDIV_BIT  (0x00e0u)
    #define WM8978_CLK_GEN_CTL_MCLKDIV_DIV1 (0x0000u)
    /* WM8978_REGADR_ADC_CTL R14 */
    #define WM8978_ADC_CTL_INI_VALUE        (0x0100u)
    #define WM8978_ADC_CTL_HPFEN_BIT        (0x0100u)
    #define WM8978_ADC_CTL_ADCOSR128_ON     (0x0008u)
    /* WM8978_REGADR_DAC_CTL R10 */
    #define WM8978_DAC_CTL_INI_VALUE        (0x0000u)
    #define WM8978_DAC_CTL_DACOSR128_ON     (0x0008u)
    /* WM8978_REGADR_INPUT_CTL R44 */
    #define WM8978_INPUTCTL_INI_VALUE       (0x0033u)
    #define WM8978_INPUTCTL_L2_2INPPGA_ON   (0x0004u)
    #define WM8978_INPUTCTL_R2_2INPPGA_ON   (0x0040u)
    /* WM8978_REGADR_LINPPGAGAIN R45 */
    #define WM8978_LINPPGAGAIN_INI_VOLL     (0x0018u)
    #define WM8978_LINPPGAGAIN_MUTEL_ON     (0x0040u)
    /* WM8978_REGADR_RINPPGAGAIN R46 */
    #define WM8978_RINPPGAGAIN_INI_VOLL     (0x0018u)
    #define WM8978_RINPPGAGAIN_MUTER_BIT    (0x0040u)
    /* WM8978_REGADR_LMIX_CTL R50 */
    #define WM8978_LMIX_CTL_INI_VALUE       (0x0001u)
    #define WM8978_LMIX_CTL_DACL2LMIX_BIT   (0x0001u)
    #define WM8978_LMIX_CTL_BYPL2LMIX_BIT   (0x0002u)
    /* WM8978_REGADR_RMIX_CTL R51 */
    #define WM8978_RMIX_CTL_INI_VALUE       (0x0001u)
    #define WM8978_RMIX_CTL_DACR2RMIX_BIT   (0x0001u)
    #define WM8978_RMIX_CTL_BYPR2RMIX_BIT   (0x0002u)
    /* WM8978_REGADR_LOUT1_VOL_CTL R52 */
    #define WM8978_LOUT1_HPVU_BIT           (0x0100u)
    /* WM8978_REGADR_ROUT1_VOL_CTL R53 */
    #define WM8978_ROUT1_HPVU_BIT           (0x0100u)

};

#endif
