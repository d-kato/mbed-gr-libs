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
* Copyright (C) 2015 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**************************************************************************//**
* @file         converter_wrapper.c
* @version      1.00
* $Rev: 1 $
* $Date:: 2015-08-06 16:33:52 +0900#$
* @brief        Graphics driver wrapper function definitions in C
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "converter_wrapper.h"
#include  "r_ospl.h"
#include  "r_jcu_api.h"
#include  "r_jcu_pl.h"

static mbed_CallbackFunc_t*     SetCallback;
static size_t*                  pEncodeSize;
static int32_t                  EncodeCount;
static int32_t                  EncodeCountMax;
static size_t                   DecodeWidth;
static size_t                   DecodeHeight;
static r_ospl_async_t           Async;

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
/**************************************************************************//**
 * @brief       Set callback function address for decode
 * @param[in]   pSetCallbackAdr  Callback function address
 * @param[in]   width            Decode data width
 * @param[in]   height           Decode data height
 * @retval      error code
******************************************************************************/
errnum_t    R_wrpper_set_decode_callback(mbed_CallbackFunc_t* pSetCallbackAdr, size_t width, size_t height)
{
    errnum_t    e;

    Async.Flags             = R_F_OSPL_InterruptCallback;
    Async.A_Thread          = R_OSPL_THREAD_GetCurrentId();
    Async.InterruptCallback = &R_wrpper_LocalDecodeCallback;
    SetCallback             = pSetCallbackAdr;
    DecodeWidth             = width;
    DecodeHeight            = height;

    e = R_JCU_StartAsync(&Async);

    return e;
}

/**************************************************************************//**
 * @brief       Set callback function address for encode
 * @param[in]   InterruptSource  Interrput source data struct address
 * @param[in]   Caller           ospl caller data address
 * @retval      error code
******************************************************************************/
errnum_t R_wrpper_LocalDecodeCallback(const r_ospl_interrupt_t *InterruptSource, const r_ospl_caller_t *Caller)
{
    errnum_t                  e = 0;
    const jcu_async_status_t* status;
    jcu_image_info_t          image_info;
    jcu_errorcode_t           jcu_error;

    e = R_JCU_OnInterruptDefault(InterruptSource, Caller);
    if (e != 0) {
        SetCallback(MBED_JCU_E_JCU_ERR);
        goto fin;
    }
    R_JCU_GetAsyncStatus( &status );
    if (status -> IsPaused == true) {
        if ((status->SubStatusFlags & JCU_SUB_INFOMATION_READY) == 0) {
            e = E_OTHERS;
            SetCallback(MBED_JCU_E_FORMA_ERR);
            goto fin;
        }
        R_JCU_GetImageInfo( &image_info );
        if ((image_info.width == 0u) || (image_info.height == 0u) || 
            (image_info.width > DecodeWidth) || 
            (image_info.height > DecodeHeight)) {
            e = E_OTHERS;
            SetCallback(MBED_JCU_E_FORMA_ERR);
            goto fin;
        }
        if ((image_info.encodedFormat != JCU_JPEG_YCbCr444) &&
            (image_info.encodedFormat != JCU_JPEG_YCbCr422) &&
            (image_info.encodedFormat != JCU_JPEG_YCbCr420) &&
            (image_info.encodedFormat != JCU_JPEG_YCbCr411)) {
            e = E_OTHERS;
            SetCallback(MBED_JCU_E_FORMA_ERR);
            goto fin;
        }
        jcu_error = R_JCU_ContinueAsync(JCU_IMAGE_INFO, &Async);
        if (jcu_error != JCU_ERROR_OK) {
            e = E_OTHERS;
            SetCallback(MBED_JCU_E_JCU_ERR);
            goto fin;
        }
    } else {
        SetCallback(MBED_JCU_E_OK);
    }

fin:
    return  e;

}

/**************************************************************************//**
 * @brief       Set callback function address for encode
 * @param[in]   pSetCallbackAdr  Callback function address
 * @param[in]   pSize            Encode size input address
 * @param[in]   count_max        Encode count max num
 * @retval      error code
******************************************************************************/
errnum_t R_wrpper_set_encode_callback( mbed_CallbackFunc_t* pSetCallbackAdr, size_t* pSize, int32_t count_max)
{
    errnum_t e;

    Async.Flags             = R_F_OSPL_InterruptCallback;
    Async.A_Thread          = R_OSPL_THREAD_GetCurrentId();
    Async.InterruptCallback = &R_wrpper_LocalEncodeCallback;
    SetCallback             = pSetCallbackAdr;
    pEncodeSize             = pSize;
    *pEncodeSize            = 0;
    EncodeCount             = 1;
    EncodeCountMax          = count_max;

    e = R_JCU_StartAsync(&Async);

    return e;
}

/**************************************************************************//**
 * @brief       Set callback function address for encode
 * @param[in]   InterruptSource  Interrput source data struct address
 * @param[in]   Caller           ospl caller data address
 * @retval      error code
******************************************************************************/
errnum_t    R_wrpper_LocalEncodeCallback(const r_ospl_interrupt_t *InterruptSource, const r_ospl_caller_t *Caller)
{
    errnum_t                  e = 0;
    const jcu_async_status_t* status;
    jcu_errorcode_t           jcu_error;

    e = R_JCU_OnInterruptDefault(InterruptSource, Caller);
    if (e != 0) {
        SetCallback(MBED_JCU_E_JCU_ERR);
        goto fin;
    }
    R_JCU_GetAsyncStatus(&status);
    if (status -> IsPaused == true) {
        if ((status->SubStatusFlags & JCU_SUB_ENCODE_OUTPUT_PAUSE) == 0) {
            e = E_OTHERS;
            goto fin;
        }
        if (EncodeCount > EncodeCountMax) {
            e = E_OTHERS;
            SetCallback(MBED_JCU_E_JCU_ERR);
            goto fin;
        }
        EncodeCount++;
        jcu_error = R_JCU_ContinueAsync(JCU_OUTPUT_BUFFER, &Async);
        if (jcu_error != JCU_ERROR_OK) {
            e = E_OTHERS;
            SetCallback(MBED_JCU_E_JCU_ERR);
            goto fin;
        }
    } else {
        (void)R_JCU_GetEncodedSize(pEncodeSize);
        SetCallback(MBED_JCU_E_OK);
    }

fin:
    return  e;

}
