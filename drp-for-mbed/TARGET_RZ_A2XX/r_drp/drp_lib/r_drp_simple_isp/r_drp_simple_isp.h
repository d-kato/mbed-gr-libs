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
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
 * File Name    : r_drp_simple_isp.h
 * Description  : This source code is input image conversion, filter processing simply.
 ******************************************************************************/
#ifndef R_DRP_SIMPLE_ISP_H
#define R_DRP_SIMPLE_ISP_H

/******************************************************************************
Global Typedef definitions
*****************************************************************************/
#ifdef BDL
typedef unsigned long long uint64_t;
typedef unsigned long uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef signed long long int64_t;
typedef signed long int32_t;
typedef signed short int16_t;
typedef signed char int8_t;
#else /* BDL */
#include <stdint.h>
#endif /* BDL */

/* Structure of interface parameters between CPU and DRP library. */
typedef struct
{
    uint32_t src;
    uint32_t dst;

    uint32_t accumulate;
    uint32_t table;

    uint16_t width;
    uint16_t height;
    uint16_t area1_offset_x;
    uint16_t area1_offset_y;

    uint16_t area1_width;
    uint16_t area1_height;
    uint16_t area2_offset_x;
    uint16_t area2_offset_y;

    uint16_t area2_width;
    uint16_t area2_height;
    uint16_t area3_offset_x;
    uint16_t area3_offset_y;

    uint16_t area3_width;
    uint16_t area3_height;
    uint16_t gain_r;
    uint16_t gain_g;

    uint16_t gain_b;
    uint16_t blend;
    int8_t   bias_r;
    int8_t   bias_g;
    int8_t   bias_b;
    uint8_t  gamma;

    uint8_t  component;
    uint8_t  strength;
    uint8_t  coring;
}
r_drp_simple_isp_t;

/*******************************************************************************
Global Tables
*******************************************************************************/
extern const uint8_t g_drp_lib_simple_isp_bayer2grayscale_3[201824];
extern const uint8_t g_drp_lib_simple_isp_bayer2grayscale_6[369088];
extern const uint8_t g_drp_lib_simple_isp_bayer2yuv_3[235328];
extern const uint8_t g_drp_lib_simple_isp_bayer2yuv_6[427424];

#endif /* R_DRP_SIMPLE_ISP_H */

/* end of file */
