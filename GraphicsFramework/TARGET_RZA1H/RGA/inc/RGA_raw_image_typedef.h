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
* @file  RGA_raw_image_typedef.h
* @brief   $Module: RGA_Image $ $PublicVersion: 0.91 $ (=RGA_IMAGE_VERSION)
* $Rev: 38 $
* $Date:: 2014-03-18 16:14:45 +0900#$
*/

#ifndef  RGA_RAW_IMAGE_TYPEDEF_H
#define  RGA_RAW_IMAGE_TYPEDEF_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Typedef definitions
******************************************************************************/

/**
* @struct  graphics_image_t
* @brief  Raw image format of RGA and ImagePackager.
*/
typedef struct st_graphics_image_t  graphics_image_t;
struct st_graphics_image_t {

    /** flags */
    uint32_t    flags;

    /** offset_to_image */
    uint32_t    offset_to_image;

    /** offset_to_alpha */
    uint32_t    offset_to_alpha;

    /** width */
    uint16_t    width;

    /** height */
    uint16_t    height;

    /** type */
    uint8_t     type;

    /** type2 */
    uint8_t     type2;

    /** color */
    uint32_t    color;
};

enum { /* graphics_image_t::flags */
    F_T_IMAGE_INF_RAW_MASK    = 0x01,
    F_T_IMAGE_INF_RAW_ADDRESS = 0x00,  /* T_IMAGE_INF::image and Alpha are pointers */
    F_T_IMAGE_INF_RAW_OFFSET  = 0x01,  /* T_IMAGE_INF::image and Alpha are offsets */

    F_T_IMAGE_INF_PREMULTIPLIED_ALPHA  = 0x04,  /* Premultiplied alpha */

    F_T_IMAGE_INF_LINE_OFFSET = 0x08,  /* Color member variable is line offset */

    F_T_IMAGE_INF_USED_MASK = 0x0000000F
};


/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
/* In "RGA_raw_image.h" */


#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif
