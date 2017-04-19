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
* @file          JPEG_Converter.h
* @brief         JCU API
******************************************************************************/

#ifndef JPEG_CONVERTER_H
#define JPEG_CONVERTER_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/** A class to communicate a JCU
 *
 */
class JPEG_Converter {

public:
    /*! @enum jpeg_conv_error_t
        @brief Error codes
     */
    typedef enum {
        JPEG_CONV_OK              =  0, /*!< Normal termination      */
        JPEG_CONV_JCU_ERR         = -1, /*!< JCU driver error        */
        JPEG_CONV_FORMA_ERR       = -2, /*!< Not support format      */
        JPEG_CONV_PARAM_ERR       = -3, /*!< Parameter error         */
        JPEG_CONV_BUSY            = -4, /*!< JCU Busy                */
        JPEG_CONV_PARAM_RANGE_ERR = -7, /*!< Parameter range error   */
    } jpeg_conv_error_t;

    /*! @enum wr_rd_swa_t
        @brief Write/Read image pixcel frame buffer swap setting
     */
    typedef enum {
        WR_RD_WRSWA_NON        = 0,     /*!< Not swapped: 1-2-3-4-5-6-7-8 */
        WR_RD_WRSWA_8BIT       = 1,     /*!< Swapped in 8-bit units: 2-1-4-3-6-5-8-7 */
        WR_RD_WRSWA_16BIT      = 2,     /*!< Swapped in 16-bit units: 3-4-1-2-7-8-5-6 */
        WR_RD_WRSWA_16_8BIT    = 3,     /*!< Swapped in 16-bit units + 8-bit units: 4-3-2-1-8-7-6-5 */
        WR_RD_WRSWA_32BIT      = 4,     /*!< Swapped in 32-bit units: 5-6-7-8-1-2-3-4 */
        WR_RD_WRSWA_32_8BIT    = 5,     /*!< Swapped in 32-bit units + 8-bit units: 6-5-8-7-2-1-4-3 */
        WR_RD_WRSWA_32_16BIT   = 6,     /*!< Swapped in 32-bit units + 16-bit units: 7-8-5-6-3-4-1-2 */
        WR_RD_WRSWA_32_16_8BIT = 7,     /*!< Swapped in 32-bit units + 16-bit units + 8-bit units: 8-7-6-5-4-3-2-1 */
    } wr_rd_swa_t;

    /*! @enum wr_rd_format_t
        @brief Write/Read image pixcel format selects
    */
    typedef enum {
        WR_RD_YCbCr422 = 0x00,          /*!< YCbCr422 (2byte / px)  */
        WR_RD_ARGB8888 = 0x01,          /*!< ARGB8888 (4byte / px)  */
        WR_RD_RGB565   = 0x02,          /*!< RGB565   (2byte / px)  */
    } wr_rd_format_t;

    /*! @enum sub_sampling_t
        @brief Thinning output image selects
    */
    typedef enum {
        SUB_SAMPLING_1_1 = 0x00,        /*!< Thinning output image to 1/1 */
        SUB_SAMPLING_1_2 = 0x01,        /*!< Thinning output image to 1/2 */
        SUB_SAMPLING_1_4 = 0x02,        /*!< Thinning output image to 1/4 */
        SUB_SAMPLING_1_8 = 0x03         /*!< Thinning output image to 1/8 */
    } sub_sampling_t;

    /*! @enum cbcr_offset_t
        @brief Cb/Cr range selects for decode
    */
    typedef enum {
        CBCR_OFFSET_0   = 0x00,         /*!< Cb/Cr range -128 to 127 (all format used) */
        CBCR_OFFSET_128 = 0x01,         /*!< Cb/Cr range    0 to 255 (Default and YCbCr format only used)      */
    } cbcr_offset_t;

    /*! @struct bitmap_buff_info_t
        @brief Bitmap data setting struct
     */
    typedef struct {
        int32_t         width;              /*!< width           width of bitmap data   */
        int32_t         height;             /*!< height          height of bitmap data  */
        wr_rd_format_t  format;             /*!< format          format of bitmap data  */
        void*           buffer_address;     /*!< buffer_address  address of bitmap data */
    } bitmap_buff_info_t;

    /** Constructor method of JPEG converter(encode/decode)
     */
    JPEG_Converter();

    /** Destructor method of JPEG converter(encode/decode)
     */
    virtual ~JPEG_Converter();

    /*! @struct DecodeOptions_t
        @brief Decode option setting
     */
    struct decode_options_t {
        sub_sampling_t  vertical_sub_sampling;                      /*!< Vertical sampling setting */
        sub_sampling_t  horizontal_sub_sampling;                    /*!< Horizontal sampling setting */
        cbcr_offset_t   output_cb_cr_offset;                        /*!< Output data Cb/Cr range setting */
        wr_rd_swa_t     output_swapsetting;                         /*!< Output data swap setteing */
        int32_t         alpha;                                      /*!< alpha setting for ARGB8888 */
        bool            check_jpeg_format;                          /*!< Flag check JPEG data format : Setting NULL does not check JPEG format when decoding */
        void (* p_DecodeCallBackFunc)(jpeg_conv_error_t err_code);  /*!< Callback function address */

        decode_options_t() {
            vertical_sub_sampling   = SUB_SAMPLING_1_1;
            horizontal_sub_sampling = SUB_SAMPLING_1_1;
            output_cb_cr_offset     = CBCR_OFFSET_128;
            output_swapsetting      = WR_RD_WRSWA_8BIT;
            alpha                   = 0;
            check_jpeg_format       = false;
            p_DecodeCallBackFunc    = NULL;
        }
    };

    /*! @struct encode_options_t
        @brief Encode option setting
     */
    struct encode_options_t {
        int32_t         DRI_value;              /*!< DRI setting */
        int32_t         width;                  /*!< Encode width */
        int32_t         height;                 /*!< Encode height */
        cbcr_offset_t   input_cb_cr_offset;     /*!< Input data Cb/Cr range setting */
        wr_rd_swa_t     input_swapsetting;      /*!< Input data swap setteing */
        size_t          encode_buff_size;       /*!< Encode buffer size : Setting 0 does not the buffer size confirmation of when encoding  */
        void*           quantization_table_Y;   /*!< Quantization table address(Y) */
        void*           quantization_table_C;   /*!< Quantization table address(C) */
        void*           huffman_table_Y_DC;     /*!< Huffman Table address(Y DC) */
        void*           huffman_table_C_DC;     /*!< Huffman Table address(C DC) */
        void*           huffman_table_Y_AC;     /*!< Huffman Table address(Y AC) */
        void*           huffman_table_C_AC;     /*!< Huffman Table address(C AC) */
        void (* p_EncodeCallBackFunc)(jpeg_conv_error_t err_code);  /*!< Callback function address */

        encode_options_t() {
            DRI_value           = 0;
            width               = 0;
            height              = 0;
            input_cb_cr_offset  = CBCR_OFFSET_128;
            input_swapsetting   = WR_RD_WRSWA_8BIT;
            encode_buff_size    = 0;
            quantization_table_Y = NULL;
            quantization_table_C = NULL;
            huffman_table_Y_DC = NULL;
            huffman_table_C_DC = NULL;
            huffman_table_Y_AC = NULL;
            huffman_table_C_AC = NULL;
            p_EncodeCallBackFunc = NULL;
        }
    };

    /** Decode JPEG to rinear data
     *
     * @param[in]     void*                 pJpegBuff       : Input JPEG data address
     * @param[in/out] bitmap_buff_info_t*   psOutputBuff    : Output bitmap data address
     * @param[in]     decode_options_t*     pOptions        : Decode option(Optional)
     * @return JPEG_CONV_OK              = success
     *         JPEG_CONV_JCU_ERR         = failure (JCU error)
     *         JPEG_CONV_FORMA_ERR       = failure (data format error)
     *         JPEG_CONV_PARAM_ERR       = failure (input parameter error)
     *         JPEG_CONV_PARAM_RANGE_ERR = failure (input parameter range error)
     */
    JPEG_Converter::jpeg_conv_error_t decode(void* pJpegBuff, bitmap_buff_info_t* psOutputBuff );
    JPEG_Converter::jpeg_conv_error_t decode(void* pJpegBuff, bitmap_buff_info_t* psOutputBuff, decode_options_t* pOptions );

    /** Encode rinear data to JPEG
     *
     * @param[in]   bitmap_buff_info_t*     psInputBuff     : Input bitmap data address
     * @param[out]  void*                   pJpegBuff       : Output JPEG data address
     * @param[out]  size_t*                 pEncodeSize     : Encode size address
     * @param[in]   encode_options_t*       pOptions[IN]    : Encode option(Optional)
     * @return JPEG_CONV_OK              = success
     *         JPEG_CONV_JCU_ERR         = failure (JCU error)
     *         JPEG_CONV_FORMA_ERR       = failure (data format error)
     *         JPEG_CONV_PARAM_ERR       = failure (input parameter error)
     *         JPEG_CONV_PARAM_RANGE_ERR = failure (input parameter range error)
     */
    JPEG_Converter::jpeg_conv_error_t encode(bitmap_buff_info_t* psInputBuff, void* pJpegBuff, size_t* pEncodeSize );
    JPEG_Converter::jpeg_conv_error_t encode(bitmap_buff_info_t* psInputBuff, void* pJpegBuff, size_t* pEncodeSize, encode_options_t* pOptions );

private:

};

#endif  /* JPEG_CONVERTER_H */
