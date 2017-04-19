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
* @file          R_BSP_SerialFamily.h
* @brief         R_BSP_SerialFamily API
******************************************************************************/

#ifndef R_BSP_IOIF_H
#define R_BSP_IOIF_H

#include <stdint.h>
#include <stdarg.h>
#include "R_BSP_Aio.h"
#include "R_BSP_mbed_fns.h"
#include "rtos.h"

/** A class to communicate a R_BSP_SerialFamily
 *
 */
class R_BSP_SerialFamily : public R_BSP_Aio {

public:
    /** Manipulates the underlying device parameters of special files
     *
     * @param request has encoded in it whether the argument is an "in" parameter or "out" parameter,
     * and the size of the argument argp in bytes.
     * @return true = success, false = failure
     */
     bool ioctl(int request, ...);

protected:
    /** Destructor
     *
     */
    virtual ~R_BSP_SerialFamily();

    /** Channel init
     *
     * @param function_list pointer of driver function callback table
     * @param channel       channel number
     * @param config_data   pointer of several parameters for a channel
     * @param max_write_num The upper limit of write buffer
     * @param max_read_num  The upper limit of read buffer
     */
    bool init_channel(RBSP_MBED_FNS * function_list, int channel, void * const config_data, int32_t max_write_num = 16, int32_t max_read_num = 16);

private:
    void * instance;
    void * ch_handle;
    int32_t ch_no;
    RBSP_MBED_FNS * functions;
};
#endif
