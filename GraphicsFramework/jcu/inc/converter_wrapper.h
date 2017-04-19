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
* @file         converter_wrapper.h
* @version      1.00
* $Rev: 1 $
* $Date:: 2015-08-06 16:33:52 +0900#$
* @brief        Graphics driver wrapper function definitions in C
******************************************************************************/

#ifndef CONVERTER_WRAPPER_H
#define CONVERTER_WRAPPER_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_ospl_typedef.h"
#include "cmsis_os.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*! @enum mbed_jcu_err_t
    @brief Error codes
 */
typedef	enum {
        MBED_JCU_E_OK              =  0,	/*!< Normal termination      */
        MBED_JCU_E_JCU_ERR         = -1,	/*!< JCU driver error        */
        MBED_JCU_E_FORMA_ERR       = -2,	/*!< Not support format      */
        MBED_JCU_E_PARAM_ERR       = -3,	/*!< Parameter error         */
        MBED_JCU_E_PARAM_RANGE_ERR = -7,	/*!< Parameter range error   */
 } mbed_jcu_err_t;


/** Callback function format
 */
typedef void (mbed_CallbackFunc_t)(mbed_jcu_err_t err_code);


/**************************************************************************//**
 * @brief       Set callback function address for decode
 * @param[in]   pSetCallbackAdr  Callback function address
 * @param[in]   p_async          ospl async data address
 * @param[in]   width            Decode data width
 * @param[in]   height           Decode data height
 * @retval      error code
******************************************************************************/
errnum_t R_wrpper_set_decode_callback(mbed_CallbackFunc_t* pSetCallbackAdr, size_t width, size_t height);

/**************************************************************************//**
 * @brief       Set callback function address for encode
 * @param[in]   InterruptSource  Interrput source data struct address
 * @param[in]   Caller           ospl caller data address
 * @retval      error code
******************************************************************************/
errnum_t R_wrpper_LocalDecodeCallback(const r_ospl_interrupt_t *InterruptSource, const r_ospl_caller_t *Caller);

/**************************************************************************//**
 * @brief       Set callback function address for encode
 * @param[in]   pSetCallbackAdr  Callback function address
 * @param[in]   p_async          ospl async data address
 * @param[in]   pSize            Encode size input address
 * @param[in]   count_max        Encode count max num
 * @retval      error code
******************************************************************************/
errnum_t R_wrpper_set_encode_callback(mbed_CallbackFunc_t* pSetCallbackAdr, size_t* pSize, int32_t count_max);

/**************************************************************************//**
 * @brief       Set callback function address for encode
 * @param[in]   InterruptSource  Interrput source data struct address
 * @param[in]   Caller           ospl caller data address
 * @retval      error code
******************************************************************************/
errnum_t R_wrpper_LocalEncodeCallback(const r_ospl_interrupt_t *InterruptSource, const r_ospl_caller_t *Caller);

#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif	/* CONVERTER_WRAPPER_H */
