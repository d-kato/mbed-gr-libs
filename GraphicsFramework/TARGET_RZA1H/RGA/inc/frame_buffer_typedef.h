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
/**
* @file  frame_buffer_typedef.h
* @brief   $Module: CLibCommon $ $PublicVersion: 1.00 $ (=CLIB_VERSION)
* $Rev: 38 $
* $Date:: 2014-03-18 16:14:45 +0900#$
*/

#ifndef  FRAME_BUFFER_TYPEDEF_H
#define  FRAME_BUFFER_TYPEDEF_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "Project_Config.h"
#include  <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Typedef definitions
******************************************************************************/

/**
* @typedef  byte_per_pixel_t
* @brief  This can have shift value, if byte per pixel is less than 1
*/
typedef int  byte_per_pixel_t;
#ifdef  IS_WINDOW_SURFACES_EX
enum {
    R_BYTE_PER_PIXEL_MASK  = 0xFF,  /*[R_BYTE_PER_PIXEL_MASK]*/
    R_BYTE_PER_PIXEL_SHIFT = 8      /*[R_BYTE_PER_PIXEL_SHIFT]*/
};
#endif


/**
* @typedef  frame_buffer_delegate_t
* @brief  This is user defined variable. Library does not access it.
*/
typedef void  frame_buffer_delegate_t;
enum { /* int_fast32_t */  frame_buffer_t_max_buffer_count = 3 };


/**
* @enum   pixel_format_t
* @brief  pixel_format_t
*
*    - PIXEL_FORMAT_UNKNOWN - 0,              0x00
*    - PIXEL_FORMAT_ARGB8888 - 1,              0x01
*    - PIXEL_FORMAT_RGB565 - 3,              0x03
*    - PIXEL_FORMAT_ARGB4444 - 5,              0x05
*    - PIXEL_FORMAT_A8 - 11,              0x0B
*    - PIXEL_FORMAT_A4 - 14,              0x0E
*    - PIXEL_FORMAT_A1 - 13,              0x0D
*    - PIXEL_FORMAT_RGB888 - 15,              0x0F
*    - PIXEL_FORMAT_R8G8B8A8 - 6 | (1 << 4),   0x16
*    - PIXEL_FORMAT_XRGB8888 - 0 | (1 << 6),   0x40
*    - PIXEL_FORMAT_ARGB1555 - 4 | (1 << 6),   0x10002
*    - PIXEL_FORMAT_YCbCr422 - 2 | (1 << 16),  0x10002
*    - PIXEL_FORMAT_YUV422 - 2 | (1 << 16),  0x10002
*    - PIXEL_FORMAT_YUV422_GRAY_SCALE_IS_0x80 - 2 | (1 << 16),  0x10002
*    - PIXEL_FORMAT_JPEG - 12 | (2 << 8),    0x20C
*    - PIXEL_FORMAT_PNG - 12 | (3 << 8),    0x30C
*    - PIXEL_FORMAT_GIF - 12 | (4 << 8),    0x40C
*    - PIXEL_FORMAT_CLUT1 - 12 | (1 << 12),  0x100C
*    - PIXEL_FORMAT_CLUT4 - 12 | (4 << 12),  0x400C
*    - PIXEL_FORMAT_CLUT8 - 12 | (8 << 12)   0x800C
*/
enum _pixel_format_t {
    PIXEL_FORMAT_UNKNOWN  =  0,
    PIXEL_FORMAT_ARGB8888 =  1,
    PIXEL_FORMAT_RGB565   =  3,
    PIXEL_FORMAT_ARGB4444 =  5,
    PIXEL_FORMAT_A8       = 11,
    PIXEL_FORMAT_A4       = 14,
    PIXEL_FORMAT_A1       = 13,
    PIXEL_FORMAT_RGB888   = 15,
    PIXEL_FORMAT_R8G8B8A8 =  6 | (1 << 4),
    PIXEL_FORMAT_XRGB8888 =  0 | (1 << 6),
    PIXEL_FORMAT_ARGB1555 =  4 | (1 << 6),
    PIXEL_FORMAT_YCbCr422 =  2 | (1 << 16),
    PIXEL_FORMAT_YUV422   =  2 | (1 << 16),
    PIXEL_FORMAT_YUV422_GRAY_SCALE_IS_0x80  = 2 | (1 << 16),
    PIXEL_FORMAT_JPEG     = 12 | (2 << 8),
    PIXEL_FORMAT_PNG      = 12 | (3 << 8),
    PIXEL_FORMAT_GIF      = 12 | (4 << 8),
    PIXEL_FORMAT_CLUT1    = 12 | (1 << 12),
    PIXEL_FORMAT_CLUT4    = 12 | (4 << 12),
    PIXEL_FORMAT_CLUT8    = 12 | (8 << 12)
};
typedef enum _pixel_format_t  pixel_format_t;


/**
* @struct  frame_buffer_t
* @brief  frame_buffer_t
*/
typedef struct st_frame_buffer_t  frame_buffer_t;
struct st_frame_buffer_t {

    /** Cached address, [2] is Reserved */
    uint8_t          *buffer_address[ frame_buffer_t_max_buffer_count ];

    /** max is 2 */
    int_fast32_t      buffer_count;

    /** Index of "buffer_address" */
    int_fast32_t      show_buffer_index;

    /** Index of "buffer_address" */
    int_fast32_t      draw_buffer_index;

    /** width */
    int_fast32_t      width;

    /** check Stride and PixelFormat */
    byte_per_pixel_t  byte_per_pixel;

    /** byte to next bottom line */
    int_fast32_t      stride;

    /** height */
    int_fast32_t      height;

    /** pixel_format */
    pixel_format_t    pixel_format;

    /** delegate */
    frame_buffer_delegate_t  *delegate;
};


/**
* @struct  r8g8b8a8_t
* @brief  One of color type
*/
#define  r8g8b8a8_t  r8g8b8a8_t
typedef union st_r8g8b8a8_t  r8g8b8a8_t;
union st_r8g8b8a8_t {
    struct {
#if  BIT_FIELD_ENDIAN == BIT_FIELD_LITTLE_ENDIAN

        /** u */
        uint8_t  Red;

        /** u */
        uint8_t  Green;

        /** u */
        uint8_t  Blue;

        /** u */
        uint8_t  Alpha;
#else
        uint8_t  Alpha;
        uint8_t  Blue;
        uint8_t  Green;
        uint8_t  Red;
#endif
    } u;  /* 1st member "u" can be used by initializer */

    /** Value */
    uint32_t  Value;  /* This is depend on CPU endian. Use only copy or compare */
};


/**
* @struct  argb8888_t
* @brief  One of color type
*/
#define        argb8888_t  argb8888_t
typedef union _ARGB8888Type  argb8888_t;
union _ARGB8888Type {
    uint32_t  Value;  /* CPU Endian. Not frame buffer endian */
    struct {
#if BYTE_ENDIAN == BYTE_LITTLE_ENDIAN

        /** Blue is Value & 0x000000FF */
        uint8_t  Blue;

        /** u */
        uint8_t  Green;

        /** u */
        uint8_t  Red;

        /** u */
        uint8_t  Alpha;
#else
        uint8_t  Alpha;
        uint8_t  Red;
        uint8_t  Green;
        uint8_t  Blue;   /* Blue is Value & 0x000000FF */
#endif
    } u;
};


/**
* @struct  xrgb8888_t
* @brief  One of color type
*/
#define        xrgb8888_t  xrgb8888_t
typedef union _XRGB8888Type  xrgb8888_t;
union _XRGB8888Type {
    uint32_t  Value;  /* CPU Endian. Not frame buffer endian */
    struct {
#if BYTE_ENDIAN == BYTE_LITTLE_ENDIAN

        /** Blue is Value & 0x000000FF */
        uint8_t  Blue;

        /** u */
        uint8_t  Green;

        /** u */
        uint8_t  Red;

        /** u */
        uint8_t  X;      /* zero */
#else
        uint8_t  X;      /* zero */
        uint8_t  Red;
        uint8_t  Green;
        uint8_t  Blue;   /* Blue is Value & 0x000000FF */
#endif
    } u;
};


/**
* @struct  rgb565_t
* @brief  One of color type
*/
#define        rgb565_t  rgb565_t
typedef union _RGB565Type  rgb565_t;
union _RGB565Type {
    uint16_t  Value;  /* CPU Endian. Not frame buffer endian */
    struct {
#if  BIT_FIELD_ENDIAN == BIT_FIELD_LITTLE_ENDIAN

        /** Blue is at LSB */
        unsigned  Blue  : 5;

        /** u */
        unsigned  Green : 6;

        /** u */
        unsigned  Red   : 5;
#else
        unsigned  Red   : 5;
        unsigned  Green : 6;
        unsigned  Blue  : 5;  /* Blue is at LSB */
#endif
    } u;
};


/**
* @struct  argb1555_t
* @brief  One of color type
*/
#define        argb1555_t  argb1555_t
typedef union _ARGB1555Type  argb1555_t;
union _ARGB1555Type {
    uint16_t  Value;  /* CPU Endian. Not frame buffer endian */
    struct {
#if  BIT_FIELD_ENDIAN == BIT_FIELD_LITTLE_ENDIAN

        /** Blue is at LSB */
        unsigned  Blue  : 5;

        /** u */
        unsigned  Green : 5;

        /** u */
        unsigned  Red   : 5;

        /** u */
        unsigned  Alpha : 1;
#else
        unsigned  Alpha : 1;
        unsigned  Red   : 5;
        unsigned  Green : 5;
        unsigned  Blue  : 5;  /* Blue is at LSB */
#endif
    } u;
};


/**
* @struct  argb4444_t
* @brief  One of color type
*/
#define        argb4444_t  argb4444_t
typedef union _ARGB4444Type  argb4444_t;
union _ARGB4444Type {
    uint16_t  Value;  /* CPU Endian. Not frame buffer endian */
    struct {
#if  BIT_FIELD_ENDIAN == BIT_FIELD_LITTLE_ENDIAN

        /** Blue is at LSB */
        unsigned  Blue  : 4;

        /** u */
        unsigned  Green : 4;

        /** u */
        unsigned  Red   : 4;

        /** u */
        unsigned  Alpha : 4;
#else
        unsigned  Alpha : 4;
        unsigned  Red   : 4;
        unsigned  Green : 4;
        unsigned  Blue  : 4;  /* Blue is at LSB */
#endif
    } u;
};


/**
* @struct  rgba8888_t
* @brief  One of color type
*/
#define        rgba8888_t  rgba8888_t
typedef union _RGBA8888Type  rgba8888_t;
union _RGBA8888Type {
    uint32_t  Value;  /* CPU Endian. Not frame buffer endian */
    struct {
#if  BIT_FIELD_ENDIAN == BIT_FIELD_LITTLE_ENDIAN

        /** Alpha is at LSB */
        unsigned  Alpha : 8;

        /** u */
        unsigned  Blue  : 8;

        /** u */
        unsigned  Green : 8;

        /** u */
        unsigned  Red   : 8;
#else
        unsigned  Red   : 8;
        unsigned  Green : 8;
        unsigned  Blue  : 8;
        unsigned  Alpha : 8;  /* Alpha is at LSB */
#endif
    } u;
};


/**
* @struct  y_cb_cr_422_t
* @brief  Same as <yuv422_t>
*/
/**
* @struct  yuv422_t
* @brief  One of color type
*/
#define          y_cb_cr_422_t  y_cb_cr_422_t
typedef union st_y_cb_cr_422_t  y_cb_cr_422_t;
#define          yuv422_t       yuv422_t
typedef union st_y_cb_cr_422_t  yuv422_t;
union st_y_cb_cr_422_t {
    struct {
        /** U */
        uint8_t  Cb_01;

        /** u */
        uint8_t  Y_0;

        /** V */
        uint8_t  Cr_01;

        /** u */
        uint8_t  Y_1;

    } u;  /* 1st member "u" can be used by initializer */

    struct {
        /** uu */
        uint8_t  U_01;

        /** uu */
        uint8_t  Y_0;

        /** uu */
        uint8_t  V_01;

        /** uu */
        uint8_t  Y_1;
    } uu;

    uint32_t  Value;
};


/******************************************************************************
Macro definitions
******************************************************************************/

/* Section: Global */
/**
* @enum   E_CATEGORY_PIXEL_FORMAT
* @brief  E_CATEGORY_PIXEL_FORMAT
*
*    - E_NOT_SUPPORTED_PIXEL_FORMAT - E_CATEGORY_PIXEL_FORMAT | 0x01
*/
#ifndef  E_CATEGORY_PIXEL_FORMAT
#define  E_CATEGORY_PIXEL_FORMAT  E_CATEGORY_PIXEL_FORMAT
enum {   E_CATEGORY_PIXEL_FORMAT = 0x00009400 };
#endif
enum { E_NOT_SUPPORTED_PIXEL_FORMAT  = E_CATEGORY_PIXEL_FORMAT | 0x01 }; /* 0x9401 = 37889 */


/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
/* In "frame_buffer.h" */

#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif
