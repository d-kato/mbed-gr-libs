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
* @file         r_spea_register.h
* @version      0.01
* $Rev: 18 $
* $Date:: 2014-05-29 11:11:38 +0900#$
* @brief        RZ/A2M SPEA driver register setup definitions
******************************************************************************/

#ifndef R_SPEA_REGISTER_H
#define R_SPEA_REGISTER_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_spea.h"

/******************************************************************************
Macro definitions
******************************************************************************/
#define SPEA_UNIT_NUMBER_INDEX_MAX  (2u)
#define SPEA_INDEX_MAX              (16u)

/******************************************************************************
Typedef definitions
******************************************************************************/
/*! SPEA Unit enable controller register address list */
typedef struct
{
    volatile uint32_t * enable;
    volatile uint32_t * disable;
} spea_regaddr_unit_enable_ctrl_t;

/*! SPEA Unit update controller register address list */
typedef struct
{
    volatile uint32_t * update;
} spea_regaddr_unit_update_ctrl_t;

/*! SPEA Unit controller register address list */
typedef struct
{
    volatile uint32_t * dst_address;
    volatile uint32_t * width_hight;
    volatile uint32_t * position;
} spea_regaddr_unit_ctrl_t;

/*! VDC image synthesizer register address list for SPEA */
typedef struct
{
    volatile uint32_t * gr_update;
    volatile uint32_t * gr_flm2;
} spea_regaddr_img_synthesizer_t;

/*! RLE register address list for SPEA */
typedef struct
{
    volatile uint32_t * enable;
    volatile uint32_t * start_address;
    volatile uint32_t * physical_address;
    volatile uint32_t * color;
    volatile uint32_t * update;
    volatile uint32_t * rlen_rdth;
} rle_regaddr_t;

/*! VDC image synthesizer register address list for RLE */
typedef struct
{
    volatile uint32_t * gr_update;
    volatile uint32_t * gr_flm1;
    volatile uint32_t * gr_flm2;
    volatile uint32_t * sc0_scl1_update;
    volatile uint32_t * sc0_scl1_wr2;
} rle_regaddr_img_synthesizer_t;

/******************************************************************************
Variable Externs
******************************************************************************/
extern const spea_regaddr_unit_enable_ctrl_t spea_regaddr_unit_enable_ctrl[SPEA_UNIT_NUMBER_INDEX_MAX];
extern const spea_regaddr_unit_update_ctrl_t spea_regaddr_unit_update_ctrl[SPEA_UNIT_NUMBER_INDEX_MAX];
extern const spea_regaddr_unit_ctrl_t spea_regaddr_unit_ctr[SPEA_UNIT_NUMBER_INDEX_MAX][SPEA_INDEX_MAX];
extern const spea_regaddr_img_synthesizer_t  spea_regaddr_img_synthesizer[SPEA_UNIT_NUMBER_INDEX_MAX];
extern const rle_regaddr_t rle_regaddr;
extern const rle_regaddr_img_synthesizer_t rle_regaddr_img_synthesizer;

/******************************************************************************
Functions Prototypes
******************************************************************************/
/*--- SPEA function --------------------------------------*/

void SPEA_SetWindow(
    const spea_layer_id_t      layer_id,
    const spea_window_id_t     window_id,
    const spea_onoff_t         sken,
	const spea_sklym_t       * sklym,
    const spea_skpsm_t       * skpsm,
	const void               * buffer);

void SPEA_WindowUpdate(
    const spea_layer_id_t      layer_id);

void SPEA_WindowOffset(
    const spea_layer_id_t      layer_id,
    const uint16_t             offset_x,
    const uint16_t             offset_y);

void SPEA_set_offset(
    const spea_layer_id_t   layer_id,
    const spea_skpsm_t    * offset );

void SPEA_get_offset(
    const spea_layer_id_t   layer_id,
          spea_skpsm_t    * offset );

/*--- RLE function --------------------------------------*/

void RLE_SetWindow(
    const rle_onoff_t    rbussel,
	const rle_cfg_t    * rle_cfg,
	const void         * buffer);

void RLE_WindowUpdate(void);

#endif  /* R_SPEA_REGISTER_H */

