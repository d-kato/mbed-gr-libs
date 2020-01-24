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
* File Name    : r_drp_find_contours.h
* Description  : This source code is the header file of
*              : r_drp_find_contours.c
******************************************************************************/
#ifndef R_DRP_FIND_CONTOURS_H
#define R_DRP_FIND_CONTOURS_H

/*******************************************************************************
Global Typedef definitions
*******************************************************************************/
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
    uint32_t src;              /* Address of input image */
    uint32_t dst_rect;         /* Address of rectangle information */
    uint16_t width;            /* The horizontal size (pixels) of image */
    uint16_t height;           /* The vertical size (pixels) of image */
    uint32_t work;             /* Address of work are */
    uint32_t dst_region;       /* Address of region information */
    uint32_t dst_rect_size;    /* Size of dst_rect area */
    uint32_t dst_region_size;  /* Size of dst_region area */
    uint16_t threshold_width;  /* Threshold of width */
    uint16_t threshold_height; /* Threshold of height */
} r_drp_find_contours_t;

/* Structure of region information */
typedef struct
{
    uint16_t x_coordinate;    /* X-coordinate of pixel */
    uint16_t y_coordinate;    /* Y-coordinate of pixel */
} r_drp_find_contours_output1_t;

/* Structure of rectangle information */
typedef struct
{
    uint16_t x_coordinate; /* left upper X-coordinate of rectangle */
    uint16_t y_coordinate; /* left upper Y-coordinate of rectangle */
    uint16_t width;        /* The horizontal size (pixels) of rectangle */
    uint16_t height;       /* The vertical size (pixels) of rectangle */
    uint32_t count;        /* Count of region information */
    uint32_t addr;         /* Head pointer of region information */
} r_drp_find_contours_output2_t;

/*******************************************************************************
Global Tables
*******************************************************************************/
extern const uint8_t g_drp_lib_find_contours[206688];

#endif /* R_DRP_FIND_CONTOURS_H */

/* end of file */







