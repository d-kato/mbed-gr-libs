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
* $FileName: jcu_para.c $
* $Module: JCU $ $PublicVersion: 1.00 $ (=JCU_VERSION)
* $Rev: 38 $
* $Date:: 2014-03-18 16:14:45 +0900#$
* Description : JCU driver checking parameter
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    <string.h>
#include    "r_typedefs.h"
#include    "r_ospl.h"
#include    "r_jcu_api.h"
#include    "r_jcu_local.h"
#include    "r_jcu_user.h"
#include    "iodefine.h"

#ifdef      JCU_PARAMETER_CHECK

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
#define MASK_LOW_3BIT   (0x7u)
#define MOD_8           (0x8u)
#define MOD_16          (0x10u)

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

/**************************************************************************//**
 * Function Name : [JCU_ParaCheckSelectCodec]
 * @brief       SelectCodec api's parameter checking
 * @param       [in] codec  codec type
 * @retval      jcu_errorcode_t
 *****************************************************************************/
jcu_errorcode_t JCU_ParaCheckSelectCodec(
    const jcu_codec_t   codec)
{
    jcu_errorcode_t  returnValue;

    IF_DS ((codec != JCU_ENCODE)
           && (codec != JCU_DECODE)) {
        returnValue = JCU_ERROR_PARAM;
    }
    else {
        returnValue = JCU_ERROR_OK;
    } /* end if */

    return (returnValue);
}

/**************************************************************************//**
 * Function Name : [JCU_ParaCheckStart]
 * @brief       Start api's parameter checking
 * @param       [in] codec  codec type
 * @retval      jcu_errorcode_t
 *****************************************************************************/
jcu_errorcode_t JCU_ParaCheckStart(
    void)
{
    jcu_errorcode_t   returnValue;

    /* If jcu already start, it cannot update this bit (this is limitation of hardware) */
    IF ( (uint32_t)R_GET_REG_BIT_FIELD( JCU.JCCMD, JSRT ) == REG_F_SET ) {
        returnValue = JCU_ERROR_STATUS;
    }
    else {
        returnValue = JCU_ERROR_OK;
    } /* end if */

    return (returnValue);
}

/**************************************************************************//**
 * Function Name : [JCU_ParaCheckSetCountMode]
 * @brief       parameter check for the SetCountMode
 * @param       [in] buffer the parameter for the Count Mode
 * @retval      jcu_errorcode_t
 *****************************************************************************/
jcu_errorcode_t JCU_ParaCheckSetCountMode(
    const jcu_count_mode_param_t  *const buffer)
{
    jcu_errorcode_t  returnValue = JCU_ERROR_OK;

    IF (buffer == NULL) {
        returnValue = JCU_ERROR_PARAM;
        goto fin;
    } /* end if */

    IF ((buffer->inputBuffer.isEnable != false) && (buffer->outputBuffer.isEnable != false)) {
        returnValue = JCU_ERROR_PARAM;
        goto fin;
    }

    if (buffer->inputBuffer.isEnable != false) {
        /* When initAddress is true, restartAddress has to set the address */
        if (buffer->inputBuffer.isInitAddress != false) {
            IF (buffer->inputBuffer.restartAddress == NULL) {
                returnValue = JCU_ERROR_PARAM;
                goto fin;
            } /* end if */
        } /* end if */

        /* Datasize(JDATAS/LINES bit) have to 8byte alignment */
        IF ((buffer->inputBuffer.dataCount & MASK_LOW_3BIT) != 0u) {
            returnValue = JCU_ERROR_PARAM;
            goto fin;
        } /* end if */

        /* Datasize have to bigger than 0 */
        IF (buffer->inputBuffer.dataCount == 0u) {
            returnValue = JCU_ERROR_PARAM;
            goto fin;
        } /* end if */

    } /* end if */

    if (buffer->outputBuffer.isEnable != false) {
        /* When initAddress is true, restartAddress has to set the address */
        if (buffer->outputBuffer.isInitAddress != false) {
            IF (buffer->outputBuffer.restartAddress == NULL) {
                returnValue = JCU_ERROR_PARAM;
                goto fin;
            } /* end if */
        } /* end if */

        /* Datasize(JDATAS/LINES bit) have to 8byte alignment */
        IF ((buffer->outputBuffer.dataCount & MASK_LOW_3BIT) != 0u) {
            returnValue = JCU_ERROR_PARAM;
            goto fin;
        } /* end if */

        /* Datasize have to bigger than 0 */
        IF (buffer->outputBuffer.dataCount == 0u) {
            returnValue = JCU_ERROR_PARAM;
            goto fin;
        } /* end if */

    } /* end if */
fin:
    return (returnValue);
}

/**************************************************************************//**
 * Function Name : [JCU_ParaCheckSetDecodeParam]
 * @brief       SetDecodeParam api's parameter checking
 * @param       [in] buffer         input and output buffer settings
 * @param       [in] interruptKind  tye type of interrupt that use in this system
 * @retval      jcu_errorcode_t
 *****************************************************************************/
jcu_errorcode_t JCU_ParaCheckSetDecodeParam(
    const jcu_decode_param_t   *const decode,
    const jcu_buffer_param_t   *const buffer)
{
    jcu_errorcode_t  returnValue = JCU_ERROR_OK;

    IF ((decode == NULL) || (buffer == NULL)) {
        returnValue = JCU_ERROR_PARAM;
        goto fin;
    } /* end if */

    /* NULL check for the address data */
    IF ((buffer->source.address == NULL)
        || (buffer->destination.address == NULL)) {
        returnValue = JCU_ERROR_PARAM;
        goto fin;
    }  /* end if */

    /* Check for the address alignment */
    /* ->MISRA 11.3 */ /* ->SEC R2.7.1 */
    IF ((((uint32_t) (buffer->source.address) % MOD_8) != 0u) ||
        (((uint32_t) (buffer->destination.address) % MOD_8) != 0u))
    /* <-MISRA 11.3 */ { /* <-SEC R2.7.1 */
        returnValue = JCU_ERROR_PARAM;
        goto fin;
    }  /* end if */

    /*Check for Cb/Cr offset */
    if(decode->decodeFormat == JCU_OUTPUT_YCbCr422) {
        IF (((decode->outputCbCrOffset) != JCU_CBCR_OFFSET_0) && ((decode->outputCbCrOffset) != JCU_CBCR_OFFSET_128)) {
            returnValue = JCU_ERROR_PARAM;
        }  /* end if */
    } else {
        IF ((decode->outputCbCrOffset) != JCU_CBCR_OFFSET_0) {
            returnValue = JCU_ERROR_PARAM;
        }  /* end if */
    } /* end if */
fin:
    return (returnValue);
}

/**************************************************************************//**
 * Function Name : [JCU_ParaCheckGetImageInfo]
 * @brief       GetImageInfo api's parameter checking
 * @param       [in] buffer         buffer address that set the image information
 * @retval      jcu_errorcode_t
 *****************************************************************************/
jcu_errorcode_t JCU_ParaCheckGetImageInfo(
    const jcu_image_info_t   *const buffer)
{
    jcu_errorcode_t  returnValue = JCU_ERROR_OK;

    IF (buffer == NULL) {
        returnValue = JCU_ERROR_PARAM;
    } /* end if */

    return (returnValue);
}

/**************************************************************************//**
 * Function Name : [JCU_ParaCheckSetQuantizationTbl]
 * @brief       SetQuantizationTable api's parameter checking
 * @param       [in] tableNo        the table number that set the parameter
 * @retval      jcu_errorcode_t
 *****************************************************************************/
jcu_errorcode_t JCU_ParaCheckSetQuantizationTbl(
    const jcu_table_no_t   tableNo)
{
    jcu_errorcode_t  returnValue = JCU_ERROR_OK;

    /* ->MISRA 13.7 : For fail safe. */
    IF_DS (tableNo > JCU_TABLE_NO_3)
    /* <-MISRA 13.7 */
    /* ->MISRA 14.1 ->QAC 3201 : For fail safe. */
    {
        returnValue = JCU_ERROR_PARAM;
    } /* end if */
    /* <-MISRA 14.1 <-QAC 3201 */

    return (returnValue);
}

/**************************************************************************//**
 * Function Name : [JCU_ParaCheckSetHuffmanTable]
 * @brief       SetHuffmanTable api's parameter checking
 * @param       [in] tableNo        the table number that set the parameter
 * @param       [in] type        the type which table is set(AC or DC)
 * @retval      jcu_errorcode_t
 *****************************************************************************/
jcu_errorcode_t JCU_ParaCheckSetHuffmanTable(
    const jcu_table_no_t   tableNo,
    const jcu_huff_t       type)
{
    jcu_errorcode_t  returnValue = JCU_ERROR_OK;

    /* ->MISRA 13.7 : For fail safe. */
    IF (tableNo > JCU_TABLE_NO_1)
    /* <-MISRA 13.7 */
    {
        returnValue = JCU_ERROR_PARAM;
        goto fin;
    } /* end if */

    /* ->MISRA 13.7 : For fail safe. */
    IF_DS ((type != JCU_HUFFMAN_AC)
           && (type != JCU_HUFFMAN_DC))
    /* <-MISRA 13.7 */
    /* ->MISRA 14.1 ->QAC 3201 : For fail safe. */
    {
        returnValue = JCU_ERROR_PARAM;
    } /* end if */
    /* <-MISRA 14.1 <-QAC 3201 */
fin:
    return (returnValue);
}

/**************************************************************************//**
 * Function Name : [JCU_ParaCheckEncodeParam]
 * @brief        api's parameter checking
 * @param       [in]
 * @retval      jcu_errorcode_t
 *****************************************************************************/
jcu_errorcode_t JCU_ParaCheckEncodeParam(
    const jcu_encode_param_t   *const encode,
    const jcu_buffer_param_t   *const buffer)
{
    jcu_errorcode_t  returnValue = JCU_ERROR_OK;

    IF ((encode == NULL) || (buffer == NULL)) {
        returnValue = JCU_ERROR_PARAM;
        goto fin;
    } /* end if */

    /* NULL check for the address data */
    IF ((buffer->source.address == NULL)
        || (buffer->destination.address == NULL)) {
        returnValue = JCU_ERROR_PARAM;
        goto fin;
    }  /* end if */


    if (encode->encodeFormat == JCU_JPEG_YCbCr422) {
        /* Check the width of the image data */
        IF ((encode->width % MOD_16) != 0u) {
            returnValue = JCU_ERROR_PARAM;
            goto fin;
        } /* end if */

        /* Check the height of the image data */
        IF ((encode->height % MOD_8) != 0u) {
            returnValue = JCU_ERROR_PARAM;
            goto fin;
        } /* end if */
    } else {
        returnValue = JCU_ERROR_PARAM;
        ASSERT_D( false, R_NOOP() );
        goto fin;
    } /* end if */

    /*Check for Cb/Cr offset */
    IF (((encode->inputCbCrOffset) != JCU_CBCR_OFFSET_0) && ((encode->inputCbCrOffset) != JCU_CBCR_OFFSET_128)) {
        returnValue = JCU_ERROR_PARAM;
    }  /* end if */

fin:
    return (returnValue);
}

#endif
