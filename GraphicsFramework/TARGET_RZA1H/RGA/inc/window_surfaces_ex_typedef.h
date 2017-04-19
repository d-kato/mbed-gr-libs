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
* @file  window_surfaces_ex_typedef.h
* @brief   $Module: CLibCommon $ $PublicVersion: 1.00 $ (=CLIB_VERSION)
* $Rev: 47 $
* $Date:: 2014-01-06 18:18:48 +0900#$
*/

#ifndef  WINDOW_SURFACES_EX_TYPEDEF_H
#define  WINDOW_SURFACES_EX_TYPEDEF_H

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
* @struct  window_surfaces_attribute_t
* @brief  Type of <window_surfaces_t>::attributes.
*/
typedef  struct st_window_surfaces_attribute_t  window_surfaces_attribute_t;
struct st_window_surfaces_attribute_t {

    /** X */
    int_fast32_t  X;

    /** Y */
    int_fast32_t  Y;

    /** Width */
    int_fast32_t  Width;

    /** Height */
    int_fast32_t  Height;

    /** OffsetX */
    int_fast32_t  OffsetX;

    /** OffsetY */
    int_fast32_t  OffsetY;

    /** OffsetByte */
    int_fast32_t  OffsetByte;
};


/* Section: Global */
/**
* @enum   access_t
* @brief  access_t
*    - ACCESS_READ - 1
*    - ACCESS_WRITE - 2
*    - ACCESS_READ_WRITE - 3
*    - ACCESS_PLUS - 4
*/
enum st_access_t {
    ACCESS_READ  = 1,
    ACCESS_WRITE = 2,
    ACCESS_READ_WRITE = ( 1 | 2 ),
    ACCESS_PLUS  = 4,

    ACCESS_ALL_MASK = ACCESS_READ | ACCESS_WRITE | ACCESS_READ_WRITE | ACCESS_PLUS
};
typedef  enum st_access_t  access_t;


/**
* @struct  layer_attributes_t
* @brief  Parameters of <R_WINDOW_SURFACES_AccessLayerAttributes>.
*/
typedef  struct st_layer_attributes_t  layer_attributes_t;
struct st_layer_attributes_t {

    /** access */
    access_t      access;

    /** 0 | F_LAYER_IS_SHOW | ... */
    bit_flags_fast32_t  flags;

    /** id */
    int_fast32_t  id;

    /** priority */
    int_fast32_t  priority;

    /** is_show */
    bool_t        is_show;

    /** is_color_key */
    bool_t        is_color_key;

    /** color_key */
    r8g8b8a8_t    color_key;

    /** layer_color */
    r8g8b8a8_t    layer_color;

    /** x */
    int_fast32_t  x;

    /** y */
    int_fast32_t  y;

    /** width */
    int_fast32_t  width;

    /** height */
    int_fast32_t  height;

    /** offset_x */
    int_fast32_t  offset_x;

    /** offset_y */
    int_fast32_t  offset_y;

    /** CLUT (Color Look Up Table) */
    uint32_t     *CLUT;

    /** CLUT (Color Look Up Table) count */
    int_fast32_t  CLUT_count;
};

enum {
    F_LAYER_ID           = 0x0001,
    F_LAYER_PRIORITY     = 0x0002,
    F_LAYER_IS_SHOW      = 0x0004,
    F_LAYER_IS_COLOR_KEY = 0x0008,
    F_LAYER_COLOR_KEY    = 0x0010,
    F_LAYER_LAYER_COLOR  = 0x0020,
    F_LAYER_X            = 0x0040,
    F_LAYER_Y            = 0x0080,
    F_LAYER_WIDTH        = 0x0100,
    F_LAYER_HEIGHT       = 0x0200,
    F_LAYER_OFFSET_X     = 0x0400,
    F_LAYER_OFFSET_Y     = 0x0800,
    F_LAYER_CLUT         = 0x1000,
    F_LAYER_CLUT_COUNT   = 0x2000
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
/* In "window_surfaces_ex.h" */


#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif
