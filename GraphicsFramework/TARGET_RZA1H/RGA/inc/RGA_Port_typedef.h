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
* Copyright (C) 2013 - 2014 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**
* @file  RGA_Port_typedef.h
* @brief   $Module: RGA $ $PublicVersion: 1.20 $ (=RGA_VERSION)
* $Rev: $
* $Date::                           $
*/

#ifndef  RGA_PORT_TYPEDEF_H
#define  RGA_PORT_TYPEDEF_H


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_ospl_typedef.h"
#include  "clib_drivers.h"  /* ceil_8 */
#include  "RGA_Port_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Typedef definitions
******************************************************************************/

/**
* @struct  uint16_not_aligned_t
* @brief  uint16_not_aligned_t
*/
#pragma pack (push,1)
/* uint16_not_aligned_t* variable raises warning, when it was passed to uint16_t* argument. */
typedef struct _uint16_not_aligned_t {
    uint8_t  byte[2];
} uint16_not_aligned_t;
#pragma pack (pop)


/**
* @struct  uint32_not_aligned_t
* @brief  uint32_not_aligned_t
*/
#pragma pack (push,1)
/* uint32_not_aligned_t* variable raises warning, when it was passed to uint32_t* argument. */
typedef struct _uint32_not_aligned_t {
    uint8_t  byte[4];
} uint32_not_aligned_t;
#pragma pack (pop)


/**
* @struct  BITMAPFILEHEADER
* @brief  BITMAPFILEHEADER
*/
#pragma pack (push,1)
typedef struct tagBITMAPFILEHEADER {
    uint16_t  bfType;
    uint32_t  bfSize;
    uint16_t  bfReserved1;
    uint16_t  bfReserved2;
    uint32_t  bfOffBits;
} BITMAPFILEHEADER;
#pragma pack (pop)


/**
* @struct  BITMAPINFOHEADER
* @brief  BITMAPINFOHEADER
*/
#pragma pack (push,1)
typedef struct tagBITMAPINFOHEADER {
    uint32_t  biSize;
    int32_t   biWidth;
    int32_t   biHeight;
    uint16_t  biPlanes;
    uint16_t  biBitCount;
    uint32_t  biCompression;
    uint32_t  biSizeImage;
    int32_t   biXPelsPerMeter;
    int32_t   biYPelsPerMeter;
    uint32_t  biClrUsed;
    uint32_t  biClrImportant;
} BITMAPINFOHEADER;
#pragma pack (pop)


/**
* @def  BI_BITFIELDS
* @brief  BI_BITFIELDS
*/
enum { BI_BITFIELDS = 3 };


/**
* @struct  vram_ex_stack_t
* @brief  vram_ex_stack_t
*/
typedef struct _vram_ex_stack_t  vram_ex_stack_t;
struct _vram_ex_stack_t {
    uint8_t  *Start;
    uint8_t  *Over;
    uint8_t  *StackPointer;
};


/******************************************************************************
Macro definitions
******************************************************************************/

#ifdef  RGAH_VERSION
#define  RGA_FRAME_BUFFER_ADDRESS_ALIGNMENT   32  /* byte */
#define  RGA_SOURCE_IMAGE_STRIDE_ALIGNMENT    32  /* byte */
#define  RGA_DESTINATION_STRIDE_ALIGNMENT     32  /* byte */
#define  RGA_JPEG_ADDRESS_ALIGNMENT            8  /* byte */
#define  RGA_JPEG_MAX_WIDTH_ALIGNMENT         16  /* pixel. Max of width_of_MCU */
#define  RGA_JPEG_MAX_HEIGHT_ALIGNMENT        16  /* pixel. Max of height_of_MCU */
#define  RGA_VDC5_BUFFER_ADDRESS_ALIGNMENT    64  /* byte */
#define  RGA_WORK_BUFFER_MAX_DISPLAY_LIST    184  /* Constant value */
#define  RGA_WORK_BUFFER_STRIDE               64  /* Constant value. This is not width of frame buffer */
#define  RGA_WORK_BUFFER_ADDRESS_ALIGNMENT    64  /* byte */
#define  RGA_WORK_BUFFER_HEIGHT_ALIGNMENT      8  /* pixel */
#define  RGA_WORK_BUFFER_B_ADDRESS_ALIGNMENT  32  /* byte */
/* RGA_WORK_BUFFER_B_ADDRESS_ALIGNMENT : RGA_RGAH_initBmp_ForSourceImage( colorBuffer ) */
#endif
#ifdef  RGAS_VERSION
#define  RGA_FRAME_BUFFER_ADDRESS_ALIGNMENT    4  /* byte */
#define  RGA_SOURCE_IMAGE_STRIDE_ALIGNMENT     4  /* byte */
#define  RGA_DESTINATION_STRIDE_ALIGNMENT      4  /* byte */
#define  RGA_JPEG_ADDRESS_ALIGNMENT            4  /* byte */
#define  RGA_JPEG_MAX_WIDTH_ALIGNMENT         16  /* pixel. Max of width_of_MCU */
#define  RGA_JPEG_MAX_HEIGHT_ALIGNMENT        16  /* pixel. Max of height_of_MCU */
#define  RGA_WORK_BUFFER_ADDRESS_ALIGNMENT     4  /* byte */
#define  RGA_WORK_BUFFER_B_ADDRESS_ALIGNMENT   4  /* byte */
#endif
#define  RGA_VDC5_BUFFER_ADDRESS_ALIGNMENT    64  /* byte */

#define  RGA_MAX_WORK_SIZE_FOR_LIB_PNG_DEFAULT   0x40000 /* byte */ /* "work_size_for_libPNG" */

#define  RGA_WORK_BUFFER_DMAC_LM_WIDTH_BYTE    4  /* byte */
#define  RGA_WORK_BUFFER_DMAC_LM_HEIGHT_BYTE  32  /* byte */

/**
* @def  R_RGA_DEFAULT_CLEAR_COLOR
* @brief  R_RGA_DEFAULT_CLEAR_COLOR
* @par Parameters
*    None
* @return  None.
*/
#define  R_RGA_DEFAULT_CLEAR_COLOR  R_RGA_Get_R8G8B8A8( 255, 255, 255, 255 )


/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
/* in RGA_Port_typedef.h */


#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif  /* RGA_PORT_TYPEDEF_H */



