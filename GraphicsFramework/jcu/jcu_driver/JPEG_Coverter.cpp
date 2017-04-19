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
* @file         jpeg_coverter.cpp
* @version      1.00
* $Rev: 1 $
* $Date:: 2015-08-06 16:33:52 +0900#$
* @brief        Decodes JPEG data and encodes to JPEG data
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  <string.h>
#include  <stdio.h>
#include  "rtos.h"
#include  "r_typedefs.h"
#include  "r_jcu_api.h"
#include  "JPEG_Converter.h"
#include  "converter_wrapper.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
#define  QUANTIZATION_TABLE_SIZE    (64u)
#define  HUFFMAN_TABLE_DC_SIZE      (28u)
#define  HUFFMAN_TABLE_AC_SIZE      (178u)
#define  JPEG_HEADER_LETTER_1       (0xFFu)
#define  JPEG_HEADER_LETTER_2       (0xD8u)
#define  ALPHA_VAL_MAX              (0xFF)
#define  LOC_KIND_COLOR_FORMAT      (3u)

#define  ENC_SIZE_MAX               (1024 * 30)
#define  MASK_8BYTE                 (0xFFFFFFF8)

    /*[QuantizationTable_Y]*/
    /* Quality is IJG75 */
    static const uint8_t  csaDefaultQuantizationTable_Y[QUANTIZATION_TABLE_SIZE] = {
        8, 6, 5, 8, 12, 20, 26, 31,
        6, 6, 7, 10, 13, 29, 30, 28,
        7, 7, 8, 12, 20, 29, 35, 28,
        7, 9, 11, 15, 26, 44, 40, 31,
        9, 11, 19, 28, 34, 55, 52, 39,
        12, 18, 28, 32, 41, 52, 57, 46,
        25, 32, 39, 44, 52, 61, 60, 51,
        36, 46, 48, 49, 56, 50, 52, 50
    };

    /*[QuantizationTable_C]*/
    /* Quality is IJG75 */
    static const uint8_t  csaDefaultQuantizationTable_C[QUANTIZATION_TABLE_SIZE] = {
        9, 9, 12, 24, 50, 50, 50, 50,
        9, 11, 13, 33, 50, 50, 50, 50,
        12, 13, 28, 50, 50, 50, 50, 50,
        24, 33, 50, 50, 50, 50, 50, 50,
        50, 50, 50, 50, 50, 50, 50, 50,
        50, 50, 50, 50, 50, 50, 50, 50,
        50, 50, 50, 50, 50, 50, 50, 50,
        50, 50, 50, 50, 50, 50, 50, 50
    };

    /*[HuffmanTable_Y_DC]*/
    /* Example written in ITU-T T81 specification */
    static const uint8_t  csaDefaultHuffmanTable_Y_DC[HUFFMAN_TABLE_DC_SIZE] = {
        0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B
    };

    /*[HuffmanTable_C_DC]*/
    /* Example written in ITU-T T81 specification */
    static const uint8_t  csaDefaultHuffmanTable_C_DC[HUFFMAN_TABLE_DC_SIZE] = {
        0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B
    };

    /*[HuffmanTable_Y_AC]*/
    /* Example written in ITU-T T81 specification */
    static const uint8_t  csaDefaultHuffmanTable_Y_AC[HUFFMAN_TABLE_AC_SIZE] = {
        0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7D,
        0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
        0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 0xD1, 0xF0,
        0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x25, 0x26, 0x27, 0x28,
        0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
        0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
        0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
        0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
        0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5,
        0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE1, 0xE2,
        0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
        0xF9, 0xFA
    };

    /*[HuffmanTable_C_AC]*/
    /* Example written in ITU-T T81 specification */
    static const uint8_t  csaDefaultHuffmanTable_C_AC[HUFFMAN_TABLE_AC_SIZE] = {
        0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77,
        0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
        0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0,
        0x15, 0x62, 0x72, 0xD1, 0x0A, 0x16, 0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A, 0x26,
        0x27, 0x28, 0x29, 0x2A, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
        0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
        0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
        0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5,
        0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3,
        0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,
        0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
        0xF9, 0xFA
    };

typedef void (JPEG_CallbackFunc_t)(JPEG_Converter::jpeg_conv_error_t err_code);

static uint32_t             driver_ac_count = 0;
static bool                 jcu_error_flag;
static JPEG_CallbackFunc_t* pJPEG_ConverterCallback;
Semaphore                   jpeg_converter_semaphore(1);


/**************************************************************************//**
 * @brief       Callback function from JCU async mode
 * @param[in]   mbed_jcu_err_t          err_code       : JCU result
 * @retval      None
******************************************************************************/
void JPEG_CallbackFunction(mbed_jcu_err_t err_code) {
    if (pJPEG_ConverterCallback != NULL) {
        pJPEG_ConverterCallback((JPEG_Converter::jpeg_conv_error_t)err_code);
    }
    if (err_code != MBED_JCU_E_OK) {
        jcu_error_flag = true;
    }
    jpeg_converter_semaphore.release(); // RELEASE
} /* End of callback function method () */

/**************************************************************************//**
 * @brief       Constructor of the JPEG_Converter class
 * @param[in]   None
 * @retval      None
******************************************************************************/
JPEG_Converter::JPEG_Converter(void) {
    jcu_errorcode_t           jcu_error;
    
    if (driver_ac_count == 0) {
        jcu_error = R_JCU_Initialize(NULL);
        if (jcu_error == JCU_ERROR_OK) {
            driver_ac_count++;
            jcu_error_flag = false;
        }
    } else {
        driver_ac_count++;
    }
} /* End of constructor method () */

/**************************************************************************//**
 * @brief       Destructor of the JPEG_Converter class
 * @param[in]   None
 * @retval      None
******************************************************************************/
JPEG_Converter::~JPEG_Converter(void) {
    if (driver_ac_count > 0) {
        driver_ac_count--;
        if (driver_ac_count == 0) {
            (void)R_JCU_Terminate();
        }
    }
} /* End of destructor method () */

/**************************************************************************//**
 * @brief         JPEG data decode to bitmap
 * @param[in]     void*                 pJpegBuff       : Input JPEG data address
 * @param[in/out] bitmap_buff_info_t*   psOutputBuff    : Output bitmap data address
 * @retval        error code
******************************************************************************/
JPEG_Converter::jpeg_conv_error_t
JPEG_Converter::decode(void* pJpegBuff, bitmap_buff_info_t* psOutputBuff) {
    decode_options_t Options;
    
    return (decode(pJpegBuff, psOutputBuff, &Options));
} /* End of method decode() */

/**************************************************************************//**
 * @brief       JPEG data decode to bitmap
 * @param[in]     void*                 pJpegBuff       : Input JPEG data address
 * @param[in/out] bitmap_buff_info_t*   psOutputBuff    : Output bitmap data address
 * @param[in]     decode_options_t*     pOptions        : Decode option(Optional)
 * @retval      error code
******************************************************************************/
JPEG_Converter::jpeg_conv_error_t
JPEG_Converter::decode(void* pJpegBuff, bitmap_buff_info_t* psOutputBuff, decode_options_t* pOptions) {
    jpeg_conv_error_t           e;
    jcu_errorcode_t             jcu_error;
    jcu_decode_param_t          decode;
    jcu_buffer_param_t          buffer;
    uint8_t*            pBuff = (uint8_t *)pJpegBuff;
    const jcu_async_status_t*   status;
    jcu_image_info_t            image_info;
    bool                        mutex_release = false;
    
    // Check JCU initialized
    if (driver_ac_count > 0) {
        size_t calc_height;
        size_t calc_width;
        
        calc_height = psOutputBuff->height * (2 ^ pOptions->vertical_sub_sampling);
        calc_width  = psOutputBuff->width * (2 ^ pOptions->horizontal_sub_sampling);
        
        // Check input address
        if ((pJpegBuff == NULL) || (psOutputBuff == NULL) || (pOptions == NULL)) {
            e = JPEG_CONV_PARAM_ERR;  // Input address error
            goto  fin;
        }
        // Check JPEG header
        if (((uint32_t)(pBuff[0]) != JPEG_HEADER_LETTER_1) ||
            ((uint32_t)(pBuff[1]) != JPEG_HEADER_LETTER_2)) {
            e = JPEG_CONV_FORMA_ERR;  // JPEG data is not in ROM
            goto  fin;
        }
        // JCU Error reset
        if (jcu_error_flag == true) {
            (void)R_JCU_Terminate();
            (void)R_JCU_Initialize(NULL);
            jcu_error_flag = false;
        }
        // Get mutex
        if (pOptions->p_DecodeCallBackFunc == NULL) {
            jpeg_converter_semaphore.wait(0xFFFFFFFFuL); // WAIT
        } else {
            if (!jpeg_converter_semaphore.wait(0)) {
                e = JPEG_CONV_BUSY;  // Busy
                goto  fin;
            }
        }
        // Select decode
        jcu_error = R_JCU_SelectCodec( JCU_DECODE );
        if (jcu_error != JCU_ERROR_OK) {
            e = JPEG_CONV_JCU_ERR;
            mutex_release = true;
            goto fin;
        }
        
        buffer.source.swapSetting       = JCU_SWAP_LONG_WORD_AND_WORD_AND_BYTE;
        buffer.source.address           = (uint32_t *)pBuff;
        buffer.lineOffset               = (int16_t)psOutputBuff->width;
        buffer.destination.address      = (uint32_t *)psOutputBuff->buffer_address;
        decode.decodeFormat             = (jcu_decode_format_t)psOutputBuff->format;
        buffer.destination.swapSetting  = (jcu_swap_t)pOptions->output_swapsetting;
        decode.outputCbCrOffset         = (jcu_cbcr_offset_t)pOptions->output_cb_cr_offset;
        decode.alpha                    = pOptions->alpha;
        decode.horizontalSubSampling    = (jcu_sub_sampling_t)pOptions->horizontal_sub_sampling;
        decode.verticalSubSampling      = (jcu_sub_sampling_t)pOptions->vertical_sub_sampling;

        jcu_error = R_JCU_SetDecodeParam(&decode, &buffer);
        if (jcu_error != JCU_ERROR_OK) {
            e = JPEG_CONV_FORMA_ERR;
            mutex_release = true;
            jcu_error_flag = false;
            goto fin;
        }

        if (pOptions->check_jpeg_format != false) {
            jcu_error = R_JCU_SetPauseForImageInfo(true);
            if (jcu_error != JCU_ERROR_OK) {
                e = JPEG_CONV_JCU_ERR;
                mutex_release = true;
                jcu_error_flag = false;
                goto fin;
            }
        }
        if (pOptions->p_DecodeCallBackFunc == NULL) {
            jcu_error = R_JCU_Start();
            mutex_release = true;
            if (jcu_error != JCU_ERROR_OK) {
                e = JPEG_CONV_JCU_ERR;
                jcu_error_flag = false;
                goto fin;
            }
            if (pOptions->check_jpeg_format != false) {
                R_JCU_GetAsyncStatus( &status );
                if (status -> IsPaused == false) {
                    e = JPEG_CONV_JCU_ERR;
                    jcu_error_flag = false;
                    goto fin;
                }
                if ((status->SubStatusFlags & JCU_SUB_INFOMATION_READY) == 0) {
                    e = JPEG_CONV_JCU_ERR;
                    jcu_error_flag = false;
                    goto fin;
                }
                jcu_error = R_JCU_GetImageInfo( &image_info );
                if (jcu_error != JCU_ERROR_OK) {
                    e = JPEG_CONV_JCU_ERR;
                    jcu_error_flag = false;
                    goto fin;
                }
                if ((image_info.width == 0u) || (image_info.height == 0u) || 
                    (image_info.width > calc_width) || 
                    (image_info.height > calc_height)) {
                    e = JPEG_CONV_FORMA_ERR;
                    jcu_error_flag = false;
                    goto fin;
                }
                if ((image_info.encodedFormat != JCU_JPEG_YCbCr444) &&
                    (image_info.encodedFormat != JCU_JPEG_YCbCr422) &&
                    (image_info.encodedFormat != JCU_JPEG_YCbCr420) &&
                    (image_info.encodedFormat != JCU_JPEG_YCbCr411)) {
                    e = JPEG_CONV_FORMA_ERR;
                    jcu_error_flag = false;
                    goto fin;
                }
                jcu_error = R_JCU_Continue(JCU_IMAGE_INFO);
                if (jcu_error != JCU_ERROR_OK) {
                    e = JPEG_CONV_JCU_ERR;
                    jcu_error_flag = false;
                    goto fin;
                }
            }
        } else {
            pJPEG_ConverterCallback = pOptions->p_DecodeCallBackFunc;
            jcu_error = R_wrpper_set_decode_callback((mbed_CallbackFunc_t*)JPEG_CallbackFunction, (size_t)calc_width, calc_height);
            if (jcu_error != JCU_ERROR_OK) {
                e = JPEG_CONV_JCU_ERR;
                mutex_release = true;
                goto fin;
            }
        }
        e = JPEG_CONV_OK;
    }
    else
    {
        e = JPEG_CONV_JCU_ERR;
    }
fin:
    if (mutex_release == true) {
        jpeg_converter_semaphore.release(); // RELEASE
    }

    return  e;
} /* End of method decode() */


/**************************************************************************//**
 * @brief       Bitmap data encode to JPEG
 * @param[in]   bitmap_buff_info_t*     psInputBuff     : Input bitmap data address
 * @param[out]  void*                   pJpegBuff       : Output JPEG data address
 * @param[out]  size_t*                 pEncodeSize     : Encode size address
 * @retval      error code
******************************************************************************/
JPEG_Converter::jpeg_conv_error_t
JPEG_Converter::encode(bitmap_buff_info_t* psInputBuff, void* pJpegBuff, size_t* pEncodeSize ) {
    encode_options_t Options;
    
    return (encode(psInputBuff, pJpegBuff, pEncodeSize, &Options));
} /* End of method encode() */


/**************************************************************************//**
 * @brief       Bitmap data encode to JPEG
 * @param[in]   bitmap_buff_info_t*     psInputBuff     : Input bitmap data address
 * @param[out]  void*                   pJpegBuff       : Output JPEG data address
 * @param[out]  size_t*                 pEncodeSize     : Encode size address
 * @param[in]   encode_options_t*       pOptions        : Encode option(Optional)
 * @retval      error code
******************************************************************************/
JPEG_Converter::jpeg_conv_error_t
JPEG_Converter::encode(bitmap_buff_info_t* psInputBuff, void* pJpegBuff, size_t* pEncodeSize, encode_options_t* pOptions ) {

    jpeg_conv_error_t   e;
    jcu_errorcode_t     jcu_error;
    jcu_buffer_param_t  buffer;
    uint8_t*            TableAddress;
    jcu_encode_param_t  encode;
    jcu_count_mode_param_t      count_para;
    int32_t     encode_count;
    int32_t    size_max_count = 1;
    size_t      BufferSize = pOptions->encode_buff_size;
    const jcu_async_status_t*  status;
    bool                        mutex_release = false;

    // Check JCU initialized
    if (driver_ac_count > 0) {
        // Check input address
        if ((pJpegBuff == NULL) || (psInputBuff == NULL) || (pEncodeSize == NULL)) {
            e = JPEG_CONV_PARAM_ERR;  // Input address error
            goto  fin;
        }
        // JCU Error reset
        if (jcu_error_flag == true) {
            (void)R_JCU_Terminate();
            (void)R_JCU_Initialize(NULL);
            jcu_error_flag = false;
        }
        // Get mutex
        if ( pOptions->p_EncodeCallBackFunc == NULL ) {
            jpeg_converter_semaphore.wait(0xFFFFFFFFuL); // WAIT
        } else {
            if (!jpeg_converter_semaphore.wait(0)) {
                e = JPEG_CONV_BUSY;  // Busy
                goto  fin;
            }
        }
        // Select encode
        jcu_error = R_JCU_SelectCodec(JCU_ENCODE);
        if (jcu_error != JCU_ERROR_OK) {
            e = JPEG_CONV_JCU_ERR;
            mutex_release = true;
            goto fin;
        }
        /* Set tables */
        if ( pOptions->quantization_table_Y != NULL ) {
            TableAddress = (uint8_t*)pOptions->quantization_table_Y;
        } else {
            TableAddress = (uint8_t*)csaDefaultQuantizationTable_Y;
        }
        jcu_error = R_JCU_SetQuantizationTable( JCU_TABLE_NO_0, (uint8_t*)TableAddress );
        if ( jcu_error != JCU_ERROR_OK ) {
            e = JPEG_CONV_PARAM_RANGE_ERR;
            mutex_release = true;
            jcu_error_flag = true;
            goto fin;
        }
        if ( pOptions->quantization_table_C != NULL ) {
            TableAddress = (uint8_t*)pOptions->quantization_table_C;
        } else {
            TableAddress = (uint8_t*)csaDefaultQuantizationTable_C;
        }
        jcu_error = R_JCU_SetQuantizationTable( JCU_TABLE_NO_1, (uint8_t*)TableAddress );
        if ( jcu_error != JCU_ERROR_OK ) {
            e = JPEG_CONV_PARAM_RANGE_ERR;
            mutex_release = true;
            jcu_error_flag = true;
            goto fin;
        }
        if ( pOptions->huffman_table_Y_DC != NULL ) {
            TableAddress = (uint8_t*)pOptions->huffman_table_Y_DC;
        } else {
            TableAddress = (uint8_t*)csaDefaultHuffmanTable_Y_DC;
        }
        jcu_error = R_JCU_SetHuffmanTable( JCU_TABLE_NO_0, JCU_HUFFMAN_DC, (uint8_t*)TableAddress );
        if ( jcu_error != JCU_ERROR_OK ) {
            e = JPEG_CONV_PARAM_RANGE_ERR;
            mutex_release = true;
            jcu_error_flag = true;
            goto fin;
        }
        if ( pOptions->huffman_table_C_DC != NULL ) {
            TableAddress = (uint8_t*)pOptions->huffman_table_C_DC;
        } else {
            TableAddress = (uint8_t*)csaDefaultHuffmanTable_C_DC;
        }
        jcu_error = R_JCU_SetHuffmanTable( JCU_TABLE_NO_1, JCU_HUFFMAN_DC, (uint8_t*)TableAddress );
        if ( jcu_error != JCU_ERROR_OK ) {
            e = JPEG_CONV_PARAM_RANGE_ERR;
            mutex_release = true;
            jcu_error_flag = true;
            goto fin;
        }
        if ( pOptions->huffman_table_Y_AC != NULL ) {
            TableAddress = (uint8_t*)pOptions->huffman_table_Y_AC;
        } else {
            TableAddress = (uint8_t*)csaDefaultHuffmanTable_Y_AC;
        }
        jcu_error = R_JCU_SetHuffmanTable( JCU_TABLE_NO_0, JCU_HUFFMAN_AC, (uint8_t*)TableAddress );
        if ( jcu_error != JCU_ERROR_OK ) {
            e = JPEG_CONV_PARAM_RANGE_ERR;
            mutex_release = true;
            jcu_error_flag = true;
            goto fin;
        }
        if ( pOptions->huffman_table_C_AC != NULL ) {
            TableAddress = (uint8_t*)pOptions->huffman_table_C_AC;
        } else {
            TableAddress = (uint8_t*)csaDefaultHuffmanTable_C_AC;
        }
        jcu_error = R_JCU_SetHuffmanTable( JCU_TABLE_NO_1, JCU_HUFFMAN_AC, (uint8_t*)TableAddress );
        if ( jcu_error != JCU_ERROR_OK ) {
            e = JPEG_CONV_PARAM_RANGE_ERR;
            mutex_release = true;
            jcu_error_flag = true;
            goto fin;
        }

        // JPEG encode
        buffer.source.swapSetting       = (jcu_swap_t)pOptions->input_swapsetting;
        buffer.source.address           = (uint32_t *)psInputBuff->buffer_address;
        buffer.destination.swapSetting  = JCU_SWAP_LONG_WORD_AND_WORD_AND_BYTE;
        buffer.destination.address      = (uint32_t *)pJpegBuff;
        buffer.lineOffset               = psInputBuff->width;
        encode.encodeFormat = (jcu_jpeg_format_t)JCU_JPEG_YCbCr422;
        encode.QuantizationTable[ JCU_ELEMENT_Y  ] = JCU_TABLE_NO_0;
        encode.QuantizationTable[ JCU_ELEMENT_Cb ] = JCU_TABLE_NO_1;
        encode.QuantizationTable[ JCU_ELEMENT_Cr ] = JCU_TABLE_NO_1;
        encode.HuffmanTable[ JCU_ELEMENT_Y  ] = JCU_TABLE_NO_0;
        encode.HuffmanTable[ JCU_ELEMENT_Cb ] = JCU_TABLE_NO_1;
        encode.HuffmanTable[ JCU_ELEMENT_Cr ] = JCU_TABLE_NO_1;
        encode.DRI_value = pOptions->DRI_value;
        if ( pOptions->width != 0 ) {
            encode.width  = pOptions->width;
        } else {
            encode.width  = psInputBuff->width;
        }
        if ( pOptions->height != 0 ) {
            encode.height = pOptions->height;
        } else {
            encode.height = psInputBuff->height;
        }
        encode.inputCbCrOffset = (jcu_cbcr_offset_t)pOptions->input_cb_cr_offset;
        jcu_error = R_JCU_SetEncodeParam( &encode, &buffer );
        if ( jcu_error != JCU_ERROR_OK ) {
            e = JPEG_CONV_PARAM_RANGE_ERR;
            mutex_release = true;
            jcu_error_flag = true;
            goto fin;
        }
        if (pOptions->encode_buff_size > 0) {

            while(BufferSize > ENC_SIZE_MAX) {
                size_max_count *= 2;
                BufferSize /= 2;
            }
            BufferSize = BufferSize & MASK_8BYTE;
            
            count_para.inputBuffer.isEnable       = false;
            count_para.inputBuffer.isInitAddress  = false;
            count_para.inputBuffer.restartAddress = NULL;
            count_para.inputBuffer.dataCount      = 0;
            count_para.outputBuffer.isEnable       = true;
            count_para.outputBuffer.isInitAddress  = false;
            count_para.outputBuffer.restartAddress = NULL;
            count_para.outputBuffer.dataCount      = BufferSize;
            
            R_JCU_SetCountMode(&count_para);
        } else {
            size_max_count = 0;
        }
        // Check async
        if ( pOptions->p_EncodeCallBackFunc == NULL ) {
            jcu_error = R_JCU_Start();
            mutex_release = true;
            if ( jcu_error != JCU_ERROR_OK ) {
                e = JPEG_CONV_JCU_ERR;
                jcu_error_flag = true;
                goto fin;
            }
            if (pOptions->encode_buff_size > 0) {
                // Check Pause flag
                R_JCU_GetAsyncStatus( &status );
                for ( encode_count = 1; (encode_count < size_max_count) && (status->IsPaused != false); encode_count++) {
                    if ((status->SubStatusFlags & JCU_SUB_ENCODE_OUTPUT_PAUSE) == 0) {
                        e = JPEG_CONV_JCU_ERR;
                        jcu_error_flag = true;
                        goto fin;
                    }
                    jcu_error = R_JCU_Continue( JCU_OUTPUT_BUFFER );
                    if (jcu_error != JCU_ERROR_OK) {
                        e = JPEG_CONV_JCU_ERR;
                        jcu_error_flag = true;
                        goto fin;
                    }
                    R_JCU_GetAsyncStatus( &status );
                }
                if (status->IsPaused != false) {
                    e = JPEG_CONV_PARAM_RANGE_ERR;
                    jcu_error_flag = true;
                    goto fin;
                }
            }
            (void)R_JCU_GetEncodedSize(pEncodeSize);
        } else {
            pJPEG_ConverterCallback = pOptions->p_EncodeCallBackFunc;
            jcu_error = R_wrpper_set_encode_callback((mbed_CallbackFunc_t*)JPEG_CallbackFunction, pEncodeSize, size_max_count);
            if ( jcu_error != JCU_ERROR_OK ) {
                e = JPEG_CONV_JCU_ERR;
                goto fin;
            }
        }
        e = JPEG_CONV_OK;
    } else {
        e = JPEG_CONV_PARAM_RANGE_ERR;
    }
fin:
    if (mutex_release == true) {
        jpeg_converter_semaphore.release(); // RELEASE
    }

    return  e;
} /* End of method encode() */

