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

#ifndef MBED_AUDIO_GRBOARD_H
#define MBED_AUDIO_GRBOARD_H

#if defined(TARGET_RZ_A1H)

#include "mbed.h"
#include "TLV320_RBSP.h"

/** AUDIO_GRBoard class
*
*/
class AUDIO_GRBoard : public TLV320_RBSP {
public:

    /** Create a audio codec class
     * 
     * @param int_level     Interupt priority (SSIF)
     * @param max_write_num The upper limit of write buffer (SSIF)
     * @param max_read_num  The upper limit of read buffer (SSIF)
     */
    AUDIO_GRBoard(uint8_t int_level = 0x80, int32_t max_write_num = 16, int32_t max_read_num = 16) :
      TLV320_RBSP(P10_13, I2C_SDA, I2C_SCL, P4_4, P4_5, P4_7, P4_6, int_level, max_write_num, max_read_num) {
        TLV320_RBSP::mic(true);
        TLV320_RBSP::micVolume(true, false);
    }
};

#elif defined(TARGET_GR_LYCHEE)

#include "mbed.h"
#include "MAX9867_RBSP.h"

/** AUDIO_GRBoard class
*
*/
class AUDIO_GRBoard : public MAX9867_RBSP {
public:

    /** Create a audio codec class
     * 
     * @param int_level     Interupt priority (SSIF)
     * @param max_write_num The upper limit of write buffer (SSIF)
     * @param max_read_num  The upper limit of read buffer (SSIF)
     */
    AUDIO_GRBoard(uint8_t int_level = 0x80, int32_t max_write_num = 16, int32_t max_read_num = 16) :
      MAX9867_RBSP(P1_7, P1_6, P2_7, P2_9, P2_8, P2_6, int_level, max_write_num, max_read_num) {
    }
};

#elif defined(TARGET_GR_MANGO)

#include "mbed.h"
#include "MAX9867_RBSP.h"

/** AUDIO_GRBoard class
*
*/
class AUDIO_GRBoard : public MAX9867_RBSP {
public:

    /** Create a audio codec class
     * 
     * @param int_level     Interupt priority (SSIF)
     * @param max_write_num The upper limit of write buffer (SSIF)
     * @param max_read_num  The upper limit of read buffer (SSIF)
     */
    AUDIO_GRBoard(uint8_t int_level = 0x80, int32_t max_write_num = 16, int32_t max_read_num = 16) :
      MAX9867_RBSP(I2C_SDA, I2C_SCL, PJ_4, PJ_3, PJ_2, PJ_0, int_level, max_write_num, max_read_num) {
    }
};

#elif defined(TARGET_RZ_A2M_EVB) || defined(TARGET_RZ_A2M_EVB_HF)

#include "mbed.h"
#include "WM8978_RBSP.h"

/** AUDIO_GRBoard class
*
*/
class AUDIO_GRBoard : public WM8978_RBSP {
public:

    /** Create a audio codec class
     * 
     * @param int_level     Interupt priority (SSIF)
     * @param max_write_num The upper limit of write buffer (SSIF)
     * @param max_read_num  The upper limit of read buffer (SSIF)
     */
    AUDIO_GRBoard(uint8_t int_level = 0x80, int32_t max_write_num = 16, int32_t max_read_num = 16) :
      WM8978_RBSP(P8_6, NC, P8_7, P8_4, P9_6, P9_5, P9_4, P9_3, P6_4, int_level, max_write_num, max_read_num) {
    }
};

#else

#include "mbed.h"
#include "AUDIO_RBSP.h"

class AUDIO_GRBoard : public AUDIO_RBSP {
public:
    AUDIO_GRBoard(uint8_t int_level = 0x80, int32_t max_write_num = 16, int32_t max_read_num = 16){}

    virtual void power(bool type = true) {}
    virtual bool format(char length) { return false; }
    virtual bool frequency(int hz) { return false; }
    virtual int write(void * const p_data, uint32_t data_size, const rbsp_data_conf_t * const p_data_conf = NULL) { return -1; }
    virtual int read(void * const p_data, uint32_t data_size, const rbsp_data_conf_t * const p_data_conf = NULL) { return -1; }
    virtual bool outputVolume(float leftVolumeOut, float rightVolumeOut) { return false; }
    virtual bool micVolume(float VolumeIn) { return false; }
};

#endif

#endif
