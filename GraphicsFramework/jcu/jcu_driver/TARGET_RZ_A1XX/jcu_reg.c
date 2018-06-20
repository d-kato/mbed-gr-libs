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
* http://www.renesas.com/disclaimer
* Copyright (C) 2012 - 2014 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* $FileName: jcu_reg.c $
* $Module: JCU $ $PublicVersion: 1.00 $ (=JCU_VERSION)
* $Rev: 35 $
* $Date:: 2014-02-26 13:18:53 +0900#$
* Description : JCU driver setting register
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    <r_typedefs.h>
#include    <iodefine.h>
#include    <r_jcu_api.h>
#include    <r_jcu_local.h>
#include    <r_jcu_user.h>

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
#define NUMBER_OF_QUANTIZATION_TABLE_DATA   (64)
#define NUMBER_OF_HUFFMAN_TABLE_DATA_DC     (28)
#define NUMBER_OF_HUFFMAN_TABLE_DATA_AC     (178)

#define CBCR_OFFSET_NUM     (2u)

#define SWAP_NORMAL         (0u)
#define SWAP_16BIT_OUTPUT   (1u)
#define SWAP_32BIT_OUTPUT   (2u)

#define KIND_COLOR_FORMAT   (3u)
#define KIND_LINEOFFSET     (2u)
#define KIND_TABLE          (4u)


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
STATIC_INLINE uint8_t  GET_UPPER_BYTE( uint32_t const  value );
STATIC_INLINE uint8_t  GET_LOWER_BYTE( uint32_t const  value );
static void JCU_GetRegisterForGetErrorInfo(jcu_detail_error_t *const errorCode);


/**************************************************************************//**
* Function Name: [GET_UPPER_BYTE]
* @brief         GET_UPPER_BYTE
* @retval        Byte
******************************************************************************/
STATIC_INLINE uint8_t  GET_UPPER_BYTE( uint32_t const  value )
{
    enum { num_8 = 8 }; /* SEC M1.10.1, QAC-3132 */
    return  (uint8_t)( value >> num_8 );
}

/**************************************************************************//**
* Function Name: [GET_LOWER_BYTE]
* @brief         GET_LOWER_BYTE
* @retval        Byte
******************************************************************************/
STATIC_INLINE uint8_t  GET_LOWER_BYTE( uint32_t const  value )
{
    return  (uint8_t)( value );
}

/**************************************************************************//**
* Function Name: [JCU_SetRegisterForInitialize]
* @brief         SetRegisterForInitialize
* @retval        jcu_errorcode_t
******************************************************************************/
jcu_errorcode_t JCU_SetRegisterForInitialize(
    void)
{
    return JCU_ERROR_OK;
}

/**************************************************************************//**
* Function Name: [JCU_SetRegisterForCodec]
* @brief         SetRegisterForCodec
* @param[out]    codec
* @retval        None
******************************************************************************/
void JCU_SetRegisterForCodec(
    const jcu_codec_t codec)
{
    volatile uint8_t DummyRead;

    /* Reset this IP */
    R_SET_REG_BIT_FIELD( CPG.SWRSTCR2, SRST, REG_F_SET );
    DummyRead = R_GET_REG_BIT_FIELD( CPG.SWRSTCR2, SRST );
    R_UNREFERENCED_VARIABLE( DummyRead );

    R_SET_REG_BIT_FIELD( CPG.SWRSTCR2, SRST, REG_F_CLR );
    DummyRead = R_GET_REG_BIT_FIELD( CPG.SWRSTCR2, SRST );
    R_UNREFERENCED_VARIABLE( DummyRead );

    R_SET_REG_BIT_FIELD( JCU.JCCMD, BRST, REG_F_SET);

    /* Enable interrupts in registers (Interrupt lines in INTC are still disabled) */
    /* This registers are set to 0 by reset this IP */
    /* ->QAC 0306 */

    JCU.JINTE1 = JCU_JINTS1_ALL;
    /* <-QAC 0306 */

    /* Clear the all of count mode settings */
    R_SET_REG_BIT_FIELD( JCU.JIFECNT, DINLC, REG_F_CLR );
    R_SET_REG_BIT_FIELD( JCU.JIFECNT, DINRINI, REG_F_CLR );
    /* ->QAC 0306 */
    JCU.JIFESLC = LINES_CLEAR;
    /* <-QAC 0306 */

    R_SET_REG_BIT_FIELD( JCU.JIFECNT, JOUTC, REG_F_CLR );
    R_SET_REG_BIT_FIELD( JCU.JIFECNT, JOUTRINI, REG_F_CLR );
    /* ->QAC 0306 */
    JCU.JIFEDDC = JDATAS_CLEAR;
    /* <-QAC 0306 */

    R_SET_REG_BIT_FIELD( JCU.JIFDCNT, JINC, REG_F_CLR );
    R_SET_REG_BIT_FIELD( JCU.JIFDCNT, JINRINI, REG_F_CLR );
    /* ->QAC 0306 */
    JCU.JIFDSDC = JDATAS_CLEAR;
    /* <-QAC 0306 */

    R_SET_REG_BIT_FIELD( JCU.JIFDCNT, DOUTLC, REG_F_CLR );
    R_SET_REG_BIT_FIELD( JCU.JIFDCNT, DOUTRINI, REG_F_CLR );
    /* ->QAC 0306 */
    JCU.JIFDDLC = LINES_CLEAR;
    /* <-QAC 0306 */

    /* Set the which process is select */
    R_SET_REG_BIT_FIELD( JCU.JCMOD, DSP, codec );
    return;
}

/**************************************************************************//**
* Function Name: [JCU_ClearInterruptFlag]
* @brief         ClearInterruptFlag
* @retval        None
******************************************************************************/
void JCU_ClearInterruptFlag(
    void)
{
    /* Interrupt request clear */
    R_SET_REG_BIT_FIELD( JCU.JCCMD, JEND, REG_F_SET);

    /* Clear the JEDI interrupt source flag */
    /* ->QAC 0306 */
    JCU.JINTS0 = (uint8_t) 0u;  /* Write 0 only */
    /* <-QAC 0306 */

    /* Clear the JDTI interrupt source flag */
    /* ->QAC 0306 */
    JCU.JINTS1 &= ~JCU_JINTS1_ALL;
    /* <-QAC 0306 */
    return;
}

/**************************************************************************//**
* Function Name: [JCU_SetRegisterForStart]
* @brief         SetRegisterForStart
* @retval        None
******************************************************************************/
void JCU_SetRegisterForStart(
    const jcu_int_detail_error_t error_filter)
{
    uint32_t reg_jinte0;
    reg_jinte0 = (uint32_t)JCU.JINTE0 & (uint32_t)(~JCU_INT_ERROR_ALL);
    JCU.JINTE0 = (uint8_t)( reg_jinte0 | (uint32_t)error_filter );
    R_SET_REG_BIT_FIELD( JCU.JCCMD, JSRT, REG_F_SET );
    return;
}

/**************************************************************************//**
* Function Name: [JCU_SetRegisterForSetCountMode]
* @brief         SetRegisterForSetCountMode
* @param[in]     buffer
* @retval        None
******************************************************************************/
void JCU_SetRegisterForSetCountMode(
    const jcu_count_mode_param_t    *const buffer)
{
    jcu_internal_information_t *const  self = R_JCU_GetInternalInformation();

    IF (buffer == NULL) {
        goto fin;
    } /* end if */

    IF_DQ ( self == NULL ) {
        goto fin;
    } /* end if */

    switch(self->Codec) {
        case JCU_STATUS_ENCODE:
            /* ->QAC 0306 */
            R_SET_REG_BIT_FIELD( JCU.JIFECNT, DINRINI, buffer->inputBuffer.isInitAddress );
            if ((buffer->inputBuffer.isEnable != false) && (buffer->inputBuffer.isInitAddress != false)) {
                JCU.JIFESA = (uint32_t)(buffer->inputBuffer.restartAddress);
            } /* end if */
            JCU.JIFESLC = ( LINES_MASK & (buffer->inputBuffer.dataCount) );

            R_SET_REG_BIT_FIELD( JCU.JIFECNT, JOUTRINI ,buffer->outputBuffer.isInitAddress );
            if ((buffer->outputBuffer.isEnable != false) && (buffer->outputBuffer.isInitAddress != false)) {
                JCU.JIFEDA = (uint32_t)(buffer->outputBuffer.restartAddress);
            } /* end if */
            JCU.JIFEDDC = ( JDATAS_MASK & ( buffer->outputBuffer.dataCount ) );
            /* <-QAC 0306 */

            R_SET_REG_BIT_FIELD( JCU.JIFECNT, DINLC ,buffer->inputBuffer.isEnable );
            R_SET_REG_BIT_FIELD( JCU.JIFECNT, JOUTC ,buffer->outputBuffer.isEnable );
            break;

        case JCU_STATUS_DECODE:
            /* ->QAC 0306 */
            R_SET_REG_BIT_FIELD( JCU.JIFDCNT, JINRINI ,buffer->inputBuffer.isInitAddress );
            if ((buffer->inputBuffer.isEnable != false) && (buffer->inputBuffer.isInitAddress != false)) {
                JCU.JIFDSA = (uint32_t)(buffer->inputBuffer.restartAddress);
            } /* end if */
            JCU.JIFDSDC = ( JDATAS_MASK & (buffer->inputBuffer.dataCount ) );

            R_SET_REG_BIT_FIELD( JCU.JIFDCNT, DOUTRINI, buffer->outputBuffer.isInitAddress );
            if ((buffer->outputBuffer.isEnable != false) && (buffer->outputBuffer.isInitAddress != false)) {
                JCU.JIFDDA = (uint32_t)(buffer->outputBuffer.restartAddress);
            } /* end if */
            JCU.JIFDDLC = ( LINES_MASK & ( buffer->outputBuffer.dataCount ) );
            /* <-QAC 0306 */

            R_SET_REG_BIT_FIELD( JCU.JIFDCNT, JINC, buffer->inputBuffer.isEnable );
            R_SET_REG_BIT_FIELD( JCU.JIFDCNT, DOUTLC, buffer->outputBuffer.isEnable );
            break;

        case JCU_CODEC_NOT_SELECTED:
            /* ->QAC 2018 : For MISRA 15.3, SEC R3.5.2 */
        default:
            /* <-QAC 2018 */
            R_NOOP();  /* NOT REACHED */
            break;
    } /* end switch */
fin:
    return;
}

/**************************************************************************//**
* Function Name: [JCU_SetRegisterForContinue]
* @brief         SetRegisterForContinue
* @param[in]     type
* @retval        None
******************************************************************************/
void JCU_SetRegisterForContinue(
    const jcu_continue_type_t  type)
{
    jcu_internal_information_t *const  self = R_JCU_GetInternalInformation();

    IF_DQ ( self == NULL ) {
        goto fin;
    } /* end if */

    /* Restart register setting */
    switch (type) {
        case JCU_INPUT_BUFFER:
            if (self->Codec == JCU_STATUS_DECODE) {
                R_SET_REG_BIT_FIELD( JCU.JIFDCNT, JINRCMD, REG_F_SET );
            } else {
                R_SET_REG_BIT_FIELD( JCU.JIFECNT, DINRCMD, REG_F_SET );
            } /* end if */
            break;

        case JCU_OUTPUT_BUFFER:
            if (self->Codec == JCU_STATUS_DECODE) {
                R_SET_REG_BIT_FIELD( JCU.JIFDCNT, DOUTRCMD, REG_F_SET );
            } else {
                R_SET_REG_BIT_FIELD( JCU.JIFECNT, JOUTRCMD, REG_F_SET );
            } /* end if */
            break;

        case JCU_IMAGE_INFO:
            R_SET_REG_BIT_FIELD( JCU.JCCMD, JRST, REG_F_SET );
            break;

            /* ->QAC 2018 : For MISRA 15.3, SEC R3.5.2 */
        default:
            /* <-QAC 2018 */
            R_NOOP();  /* NOT REACHED */
            break;
    } /* end switch */

fin:
    return;
}

/**************************************************************************//**
* Function Name: [JCU_SetRegisterForSetDecodePrm]
* @brief         SetRegisterForSetDecod
* @param[in]     decode
* @param[in]     buffer
* @param[in]     interruptKind
* @retval        None
******************************************************************************/
void JCU_SetRegisterForSetDecodePrm(
    const jcu_decode_param_t   *const decode,
    const jcu_buffer_param_t   *const buffer)
{
    const uint32_t byteSize[KIND_COLOR_FORMAT] =
        /* ->SEC M1.10.1 Table data. */
    {
        2u, /* JCU_OUTPUT_YCbCr422 */
        4u, /* JCU_OUTPUT_ARGB8888 */
        2u  /* JCU_OUTPUT_RGB565   */
    };
    /* <-SEC M1.10.1 */

    if ((decode == NULL) || (buffer == NULL)) {
        goto fin;
    } /* end if */

    /* Settings for the cull */
    R_SET_REG_BIT_FIELD( JCU.JIFDCNT, VINTER, decode->verticalSubSampling );
    R_SET_REG_BIT_FIELD( JCU.JIFDCNT, HINTER, decode->horizontalSubSampling );
    R_SET_REG_BIT_FIELD( JCU.JIFDCNT, OPF, decode->decodeFormat );
    R_SET_REG_BIT_FIELD( JCU.JIFDCNT, JINSWAP, buffer->source.swapSetting );
    R_SET_REG_BIT_FIELD( JCU.JIFDCNT, DOUTSWAP, buffer->destination.swapSetting );

    /* Settings for input jpeg file information */
    /* ->QAC 0306 */
    JCU.JIFDSA = (uint32_t)(buffer->source.address);
    /* <-QAC 0306 */
    /* Settings for output image data information */
    /* ->QAC 0306 */
    JCU.JIFDDA = (uint32_t)(buffer->destination.address);
    /* <-QAC 0306 */
    R_SET_REG_BIT_FIELD( JCU.JIFDDOFST, DDMW, ((uint32_t)buffer->lineOffset * (uint32_t)(byteSize[decode->decodeFormat])) );
    R_SET_REG_BIT_FIELD( JCU.JIFDADT, ALPHA, (uint32_t)(decode->alpha) );


    /* Setting for Cb/Cr offset */
    R_SET_REG_BIT_FIELD( JCU.JIFESHSZ, DOUTYCHG, decode->outputCbCrOffset );
fin:
    return;

}

/**************************************************************************//**
* Function Name: [JCU_SetRegisterForSetQtTable]
* @brief         SetRegisterForSetQtTable
* @param[in]     tableNo
* @param[in]     table
* @retval        None
******************************************************************************/
void JCU_SetRegisterForSetQtTable(
    const jcu_table_no_t   tableNo,
    const uint8_t *const   table)
{
    volatile uint8_t  *tableAtRegister;
    int_fast32_t       counter;

    /* Select the destination of the Quantization table */
    switch (tableNo) {
            /* ->QAC 0306 */
        case JCU_TABLE_NO_0:
            tableAtRegister = (volatile uint8_t *)&JCU.JCQTBL0;
            break;
        case JCU_TABLE_NO_1:
            tableAtRegister = (volatile uint8_t *)&JCU.JCQTBL1;
            break;
        case JCU_TABLE_NO_2:
            tableAtRegister = (volatile uint8_t *)&JCU.JCQTBL2;
            break;
        case JCU_TABLE_NO_3:
            tableAtRegister = (volatile uint8_t *)&JCU.JCQTBL3;
            break;
            /* ->QAC 2018 : For MISRA 15.3, SEC R3.5.2 */
        default:
            /* <-QAC 2018 */
            /* NOT REACHED */
            return;
            /* <-QAC 0306 */
    } /* end switch */

    IF ((tableAtRegister == NULL) || (table == NULL)) {
        goto fin;
    } /* end if */

    /* Update the quantization table data */
    for (counter = 0; counter < NUMBER_OF_QUANTIZATION_TABLE_DATA; counter++) {
        tableAtRegister[ counter ] = table[ counter ];
    } /* end for */
fin:
    return;
}

/**************************************************************************//**
* Function Name: [JCU_SetRegisterForSetHuffTbl]
* @brief         SetRegisterForSetHuffTbl
* @param[in]     tableNo
* @param[in]     type
* @param[in]     table
* @retval        None
******************************************************************************/
void JCU_SetRegisterForSetHuffTbl(
    const jcu_table_no_t   tableNo,
    const jcu_huff_t       type,
    const uint8_t         *table)
{
    volatile uint8_t  *tableAtRegister;
    int_fast32_t       copyCount;
    int_fast32_t       counter;

    /* Select the destination Huffman table and the size of copy data */
    if (tableNo == JCU_TABLE_NO_0) {
        /* ->QAC 0306 */
        if (type == JCU_HUFFMAN_DC) {
            tableAtRegister  = (volatile uint8_t *)&JCU.JCHTBD0;
            copyCount        = NUMBER_OF_HUFFMAN_TABLE_DATA_DC;
        } else {
            tableAtRegister  = (volatile uint8_t *)&JCU.JCHTBA0;
            copyCount        = NUMBER_OF_HUFFMAN_TABLE_DATA_AC;
        } /* end if */
        /* <-QAC 0306 */
    } else {
        /* ->QAC 0306 */
        if (type == JCU_HUFFMAN_DC) {
            tableAtRegister  = (volatile uint8_t *)&JCU.JCHTBD1;
            copyCount        = NUMBER_OF_HUFFMAN_TABLE_DATA_DC;
        } else {
            tableAtRegister  = (volatile uint8_t *)&JCU.JCHTBA1;
            copyCount        = NUMBER_OF_HUFFMAN_TABLE_DATA_AC;
        } /* end if */
        /* <-QAC 0306 */
    }  /* end if */

    IF(table == NULL) {
        goto fin;
    } /* end if */

    /* Update the Huffman table */
    for (counter = 0; counter < copyCount; counter++) {
        tableAtRegister[ counter ] = table[ counter ];
    } /* end for */
fin:
    return;
}

/**************************************************************************//**
* Function Name: [JCU_SetRegisterForSetEncodePrm]
* @brief         SetRegisterForSetEncode
* @param[in]     encode
* @param[in]     buffer
* @retval        None
******************************************************************************/
void JCU_SetRegisterForSetEncodePrm(
    const jcu_encode_param_t   *const encode,
    const jcu_buffer_param_t   *const buffer)
{
    jcu_jpeg_format_t encodeFormatTemp;
    const uint32_t byteSize[KIND_LINEOFFSET] =
        /* ->SEC M1.10.1 Table data. */
    {
        0u, /* Reserved          */
        2u  /* JCU_JPEG_YCbCr422 */
    };
    /* <-SEC M1.10.1 */

    IF ((encode == NULL) || (buffer == NULL)) {
        goto fin;
    } /* end if */

    /* Settings for the input image format */
    R_SET_REG_BIT_FIELD( JCU.JCMOD, REDU, encode->encodeFormat );

    /* Settings for the quantization table */
    R_SET_REG_BIT_FIELD( JCU.JCQTN, QT1, encode->QuantizationTable[JCU_ELEMENT_Y]  );
    R_SET_REG_BIT_FIELD( JCU.JCQTN, QT2, encode->QuantizationTable[JCU_ELEMENT_Cb]  );
    R_SET_REG_BIT_FIELD( JCU.JCQTN, QT3, encode->QuantizationTable[JCU_ELEMENT_Cr]  );

    /* Settings for the Huffman table */
    R_SET_REG_BIT_FIELD( JCU.JCHTN, HTA1, (uint32_t)(encode->HuffmanTable[JCU_ELEMENT_Y])  );
    R_SET_REG_BIT_FIELD( JCU.JCHTN, HTD1, (uint32_t)(encode->HuffmanTable[JCU_ELEMENT_Y])  );
    R_SET_REG_BIT_FIELD( JCU.JCHTN, HTA2, (uint32_t)(encode->HuffmanTable[JCU_ELEMENT_Cb]) );
    R_SET_REG_BIT_FIELD( JCU.JCHTN, HTD2, (uint32_t)(encode->HuffmanTable[JCU_ELEMENT_Cb]) );
    R_SET_REG_BIT_FIELD( JCU.JCHTN, HTA3, (uint32_t)(encode->HuffmanTable[JCU_ELEMENT_Cr]) );
    R_SET_REG_BIT_FIELD( JCU.JCHTN, HTD3, (uint32_t)(encode->HuffmanTable[JCU_ELEMENT_Cr]) );

    /* ->QAC 0306 */
    JCU.JCDRIU = GET_UPPER_BYTE(encode->DRI_value);
    JCU.JCDRID = GET_LOWER_BYTE(encode->DRI_value);
    /* Settings for the image size */
    /* width setting */
    JCU.JCHSZU = GET_UPPER_BYTE(encode->width);
    JCU.JCHSZD = GET_LOWER_BYTE(encode->width);
    /* height setting */
    JCU.JCVSZU = GET_UPPER_BYTE(encode->height);
    JCU.JCVSZD = GET_LOWER_BYTE(encode->height);
    /* <-QAC 0306 */


    /* Settings for input jpeg file information */
    R_SET_REG_BIT_FIELD( JCU.JIFECNT, DINSWAP, buffer->source.swapSetting );

    /* ->QAC 0306 */
    JCU.JIFESA              = (uint32_t)(buffer->source.address);
    /* <-QAC 0306 */

    /* Settings for output image data information */
    R_SET_REG_BIT_FIELD( JCU.JIFECNT, JOUTSWAP, buffer->destination.swapSetting );

    /* ->QAC 0306 */
    JCU.JIFEDA = (uint32_t)(buffer->destination.address);
    /* <-QAC 0306 */

    encodeFormatTemp = encode->encodeFormat;
    if((encodeFormatTemp != JCU_JPEG_YCbCr444) && (encodeFormatTemp != JCU_JPEG_YCbCr422)) {
        encodeFormatTemp = JCU_JPEG_YCbCr444;
    } /* end if */

    R_SET_REG_BIT_FIELD( JCU.JIFESOFST, ESMW, ((uint32_t)buffer->lineOffset * (uint32_t)(byteSize[encodeFormatTemp])) );

    /* Setting for Cb/Cr offset */
    R_SET_REG_BIT_FIELD( JCU.JIFESVSZ, DINYCHG, encode->inputCbCrOffset );
fin:
    return;

}

/**************************************************************************//**
* Function Name: [JCU_SetRegisterForSetPause]
* @brief
* @param[out]    is_stop
* @retval        None
******************************************************************************/
void JCU_SetRegisterForSetPause( const bool_t is_pause, const jcu_int_detail_errors_t filter )
{
    /* ->QAC 0306 */
    if ( IS( is_pause ) ) {
        JCU.JINTE0 = filter | INT3_MASK;
    } else {
        JCU.JINTE0 = filter;
    }
    /* <-QAC 0306 */
}

/**************************************************************************//**
* Function Name: [JCU_GetRegisterForGetImageInfo]
* @brief         Get "jcu_image_info_t" type value
* @param[out]    buffer  :jcu_image_info_t
* @retval        None
******************************************************************************/
void JCU_GetRegisterForGetImageInfo(
    jcu_image_info_t *const buffer)
{
    if (buffer == NULL) {
        goto fin;
    } /* end if */

    /* Set the register value to the variables */
    /* ->QAC 0306 */
    buffer->width           = ((uint32_t)JCU.JCHSZU << SHIFT_8BITS);
    buffer->width           |= (uint32_t)JCU.JCHSZD;
    buffer->height          = ((uint32_t)JCU.JCVSZU << SHIFT_8BITS);
    buffer->height          |= (uint32_t)JCU.JCVSZD;
    buffer->encodedFormat   = (jcu_jpeg_format_t)R_GET_REG_BIT_FIELD( JCU.JCMOD, REDU );
    /* <-QAC 0306 */

fin:
    return;
}

/**************************************************************************//**
* Function Name: [JCU_GetRegisterForGetErrorInfo]
* @brief         Get "jcu_detail_error_t" type value
* @param[out]    errorCode
* @retval        None
******************************************************************************/
static void JCU_GetRegisterForGetErrorInfo(
    jcu_detail_error_t *const errorCode)
{
    if (errorCode != NULL) {
        /* ->QAC 0306 */
        uint8_t const  code = R_GET_REG_BIT_FIELD( JCU.JCDERR, ERR );
        /* <-QAC 0306 */

        if ( (uint_fast8_t) code == 0u ) {
            *errorCode = JCU_JCDERR_OK;
        } else {
            *errorCode = (jcu_detail_error_t)( (int_t) code + E_CATEGORY_JCU_JCDERR );
        }
    } /* end if */
    return;
}

/**************************************************************************//**
* Function Name: [JCU_GetEncodedSize]
* @brief         GetEncodedSize
* @param[out]    out_Size
* @retval        None
******************************************************************************/
void JCU_GetEncodedSize(
    size_t *const out_Size)
{
    if (out_Size != NULL) {
        /* ->QAC 0306 */
        *out_Size = (((uint32_t)JCU.JCDTCU) << SHIFT_16BITS);
        *out_Size |= (((uint32_t)JCU.JCDTCM) << SHIFT_8BITS);
        *out_Size |= ((uint32_t)JCU.JCDTCD);
        /* <-QAC 0306 */
    } /* end if */
    return;
}

/**************************************************************************//**
* Function Name: [R_JCU_OnInterrupting]
* @brief         Operations on interrupting
* @retval        Error code, 0=No error
******************************************************************************/
errnum_t  R_JCU_OnInterrupting( const r_ospl_interrupt_t *const  InterruptSource )
{
    errnum_t  e;
    uint32_t  flags0;
    uint32_t  flags1;
    jcu_internal_information_t *const  self = R_JCU_GetInternalInformation();

    R_UNREFERENCED_VARIABLE( InterruptSource );
    IF_DQ( self == NULL ) {
        e=E_OTHERS;
        goto fin;
    }

    ASSERT_R( self->AsyncStatus.Status == JCU_STATUS_INTERRUPTING,  e=E_STATE; goto fin );

    /* Get interrupt status */
    /* ->QAC 0306 */
    flags0 = JCU.JINTS0;
    flags1 = JCU.JINTS1;
    R_OSPL_FLAG32_Set( &self->AsyncStatus.InterruptFlags, flags0 );
    R_OSPL_FLAG32_Set( &self->AsyncStatus.InterruptFlags, flags1 << JCU_SHIFT_JINTS1 );

    /* Clear interrupt status "JINTS0" */
    if ( flags0 != 0u ) {
        R_SET_REG_BIT_FIELD( JCU.JCCMD, JEND, 1u );
        JCU.JINTS0 = (uint8_t) 0u;  /* Write 0 only */
    }

    /* Clear interrupt status "JINTS1" */
    JCU.JINTS1 = ~flags1 & JCU_JINTS1_ALL;
    /* <-QAC 0306 */

    /* Set state */
    self->AsyncStatus.Status = JCU_STATUS_INTERRUPTED;

    e=0;
fin:
    return  e;
}

/**************************************************************************//**
* Function Name: [R_JCU_OnInterrupted]
* @brief         Bottom half operations on interrupting
* @retval        Error code, 0=No error
******************************************************************************/
errnum_t  R_JCU_OnInterrupted(void)
{
    bit_flags32_t const  sub_status_bits =
        ( INS3_MASK ) |
        ( JINF_MASK   << JCU_SHIFT_JINTS1 ) |
        ( DOUTLF_MASK << JCU_SHIFT_JINTS1 ) |
        ( DINLF_MASK  << JCU_SHIFT_JINTS1 ) |
        ( JOUTF_MASK  << JCU_SHIFT_JINTS1 );

    errnum_t         e;
    uint32_t         cancel_flag;
    uint32_t         interrupt_flags;
    bool_t           was_enabled = false;
    bool_t           is_end;
    bool_t           or;
    jcu_errorcode_t  ej;
    r_ospl_async_t  *notify_async_1 = NULL;
    r_ospl_async_t  *notify_async_2 = NULL;
    jcu_i_lock_t *const                i_lock = R_JCU_GetILockObject();
    jcu_internal_information_t *const  self = R_JCU_GetInternalInformation();

    IF_DQ( self == NULL ) {
        e=E_OTHERS;
        goto fin;
    }

    was_enabled = self->I_LockVTable->Lock( self->I_Lock );

    ASSERT_R( self->AsyncStatus.Status != JCU_STATUS_UNDEF,  e=E_STATE; goto fin );

    interrupt_flags = R_OSPL_FLAG32_GetAndClear( &self->AsyncStatus.InterruptFlags );


    if ( IS_ANY_BITS_SET( interrupt_flags, sub_status_bits ) ) {
        if ( IS_BIT_NOT_SET( interrupt_flags, INS6_MASK ) ) {
            self->AsyncStatus.IsPaused = true;
            self->AsyncStatus.SubStatusFlags = interrupt_flags & sub_status_bits;
        }
    }


    if ( IS_ANY_BITS_SET( interrupt_flags, INS6_MASK ) ) {
        is_end = true;
    } else {
        is_end = false;
    }


    or = ( (is_end) || (self->AsyncStatus.IsPaused) );
    if (!or) {
        or = IS_BIT_SET( interrupt_flags, INS5_MASK );
    }
    if (IS( or )) {
        /* Set "AsyncStatus" */
        R_OSPL_FLAG32_Clear( &self->AsyncStatus.InterruptEnables, JCU_INTERRUPT_LINE_ALL );
        self->AsyncStatus.Status = JCU_STATUS_READY;
        notify_async_1 = self->InterruptCallbackCaller.Async;
    } else {
        self->AsyncStatus.Status = JCU_STATUS_RUN;
    }


    IF ( IS_BIT_SET( interrupt_flags, INS5_MASK ) ) {
        jcu_detail_error_t  ed;

        JCU_GetRegisterForGetErrorInfo( &ed );
        e = ed;
        goto fin;
    }


    if ( notify_async_1 != NULL ) {
        cancel_flag = R_OSPL_FLAG32_Get( &self->AsyncStatus.CancelFlags );
        if ( IS_BIT_SET( cancel_flag, R_OSPL_FINALIZE_REQUEST ) ) {
            notify_async_2 = self->AsyncForFinalize;
            ej= R_JCU_TerminateStep2();
            IF ( ej != JCU_ERROR_OK ) {
                e=E_OTHERS;
                goto fin;
            }
        }
    }

    e=0;
fin:
    if ( IS( was_enabled ) ) {
        /* ->QAC 3353 : "self" is always assigned, if "was_enabled" is true */
        IF_DQ( self == NULL ) {}  /* QAC 3353 raises SEC R3.2.2 */
        else {
            self->I_LockVTable->Unlock( self->I_Lock );

            if ( IS( self->Is_I_LockMaster ) ) {
                IF_DQ( i_lock == NULL ) {}
                else {
                    if ( IS( i_lock->IsRequestedFinalize ) ) {
                        R_JCU_I_LOCK_Finalize();
                    }
                }
            }
        }
        /* <-QAC 3353 */
    }

    if ( notify_async_1 != NULL ) {
        R_OSPL_EVENT_Set( notify_async_1->A_Thread, notify_async_1->A_EventValue );
    }
    if ( notify_async_2 != NULL ) {
        R_OSPL_EVENT_Set( notify_async_2->A_Thread, notify_async_2->A_EventValue );
    }

    return  e;
}

