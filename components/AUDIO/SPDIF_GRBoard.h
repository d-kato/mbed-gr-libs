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

#ifndef MBED_SPDIF_GRBOARD_H
#define MBED_SPDIF_GRBOARD_H

#if defined(TARGET_RZ_A2M_EVB) || defined(TARGET_RZ_A2M_EVB_HF)

#include "mbed.h"
#include "SPDIF_RBSP.h"

/** SPDIF_GRBoard class
*
*/
class SPDIF_GRBoard : public SPDIF_RBSP {
public:

    /** Create a audio codec class
     * 
     * @param int_level     Interupt priority (SPDIF)
     * @param max_write_num The upper limit of write buffer (SPDIF)
     * @param max_read_num  The upper limit of read buffer (SPDIF)
     */
    SPDIF_GRBoard(uint8_t int_level = 0x80, int32_t max_write_num = 16, int32_t max_read_num = 16) :
      SPDIF_RBSP(P6_4, PC_5, NC, false, int_level, max_write_num, max_read_num) {
    }
};

#endif

#endif
