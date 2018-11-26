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
* Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**************************************************************************//**
* @file         r_spea.h
* @version      0.01
* $Rev: 18 $
* $Date:: 2014-05-29 11:11:38 +0900#$
* @brief        RZ/A2M SPEA driver API definitions
******************************************************************************/

#ifndef R_SPEA_H
#define R_SPEA_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_vdc.h"


#ifdef  __cplusplus
extern  "C"
{
#endif  /* __cplusplus */

/******************************************************************************
Macro definitions
******************************************************************************/
#define VIRTUAL_FRAME_BASE_ADD      (0x30000000u)
#define VIRTUAL_FRAME_STRAID        (8192u)

/*! Error codes of the VDC driver */
typedef enum
{
    SPEA_OK = 0,                /*!< Normal termination */
    SPEA_ERR_PARAM_LAYER_ID,    /*!< Invalid layer ID error (parameter error): An illegal layer ID is specified. */
    SPEA_ERR_PARAM_NULL,        /*!< NULL specification error (parameter error):
                                     NULL is specified for a required parameter. */
    SPEA_ERR_PARAM,             /*!< Detecting the specification violation. */
    SPEA_ERR_NUM                /*!< The number of error codes */
} spea_error_t;

/*! Layer ID */
typedef enum
{
    SPEA_LAYER_ID_0   = 0,   /*!< SPE layer 0 */
    SPEA_LAYER_ID_1   = 1,   /*!< SPE layer 1 */
    SPEA_LAYER_ID_NUM = 2    /*!< The number of layer IDs */
} spea_layer_id_t;

/*! On/off */
typedef enum
{
    SPEA_OFF    = 0,            /*!< Off */
    SPEA_ON     = 1             /*!< On */
} spea_onoff_t;

/*! On/off */
typedef enum
{
    RLE_OFF    = 0,            /*!< Off */
    RLE_ON     = 1             /*!< On */
} rle_onoff_t;

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************* For R_SPEA_SetWindow       *******************************/
/*! Window position control parameter for SPEA */
/*! Error codes of the VDC driver */
typedef enum
{
    WINDOW_00 = 0,
    WINDOW_01,
    WINDOW_02,
    WINDOW_03,
    WINDOW_04,
    WINDOW_05,
    WINDOW_06,
    WINDOW_07,
    WINDOW_08,
    WINDOW_09,
    WINDOW_10,
    WINDOW_11,
    WINDOW_12,
    WINDOW_13,
    WINDOW_14,
    WINDOW_15,
    WINDOW_NUM
} spea_window_id_t;

typedef struct
{
    int32_t x;                     /*!< Line offset address direction of the frame buffer */
    int32_t y;                     /*!< Line offset address direction of the frame buffer */
} spea_skpsm_t;

/*! Window size control parameter for SPEA */
typedef struct
{
	int32_t width;                 /*!< Line offset address direction of the frame buffer */
	int32_t height;                /*!< Line offset address direction of the frame buffer */
} spea_sklym_t;

/*! Window control parameter for SPEA */
typedef struct
{
	spea_window_id_t  window_id;   /*! Window ID for SPEA */
	spea_onoff_t      sken;        /*! Window on/off for SPEA */
	spea_skpsm_t      pos;         /*! Window position control parameter for SPEA */
    spea_sklym_t      size;        /*! Window size control parameter for SPEA */
    const void      * buffer;      /*! Window buffer control parameter for SPEA */
} spea_window_conf_t;

/******************************* For R_RLE_SetWindow       *******************************/
/*! RLE Window control parameter */
typedef struct
{
	int32_t rlen;                 /*!< Burst size of the RLE unit next data pre-fetching */
	int32_t rdth;                 /*!< Pre-fetch timing for FIFO */
} rle_cfg_t;

/*! targa structure */
typedef struct
{
	uint8_t   id_length;            /*!< Length of the image ID field  */
	uint8_t   color_map_type;       /*!< Whether a color map is included */
	uint8_t   image_type;           /*!< Compression and color types */
	uint8_t   first_entry_index[2]; /*!< index of first color map entry that is included in the file */
	uint8_t   color_map_length[2];  /*!< number of entries of the color map that are included in the file */
	uint8_t   color_map_entry_size; /*!< number of bits per pixel */
	uint8_t   x_origin[2];          /*!< absolute coordinate of lower-left corner for displays
	                                    where origin is at the lower left  */
	uint8_t   y_origin[2];          /*!< as for X-origin */
	uint8_t   image_width[2];       /*!< width in pixels */
	uint8_t   image_height[2];      /*!< height in pixels */
	uint8_t   pixel_depth;          /*!< bits per pixel */
	uint8_t   image_descriptor;     /*!< bits 3-0 give the alpha channel depth, bits 5-4 give direction */

} targa_struct_t;

/******************************************************************************
Exported global variables
******************************************************************************/

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/
/*--- SPEA function --------------------------------------*/

spea_error_t R_SPEA_SetWindow(
    const vdc_layer_id_t       layer_id,
    const spea_window_id_t     window_id,
    const spea_onoff_t         sken,
	const spea_sklym_t       * sklym,
    const spea_skpsm_t       * skpsm,
	const void               * buffer);
spea_error_t R_SPEA_WindowOffset(
    const vdc_layer_id_t       layer_id,
    const uint16_t             offset_x,
    const uint16_t             offset_y);
spea_error_t R_SPEA_WindowUpdate(
    const vdc_layer_id_t       layer_id);

/*--- RLE function --------------------------------------*/

spea_error_t R_RLE_SetWindow(
	const vdc_error_t    layer_id,
    const rle_onoff_t    rbussel,
	const rle_cfg_t    * rle_cfg,
	const void         * buffer);
spea_error_t R_RLE_WindowUpdate(
    const vdc_layer_id_t   layer_id);

#ifdef  __cplusplus
}
#endif  /* __cplusplus */


#endif  /* R_SPE_H */

