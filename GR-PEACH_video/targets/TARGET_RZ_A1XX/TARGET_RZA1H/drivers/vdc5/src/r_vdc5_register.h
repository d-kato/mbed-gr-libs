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
* Copyright (C) 2012 - 2015 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**************************************************************************//**
* @file         r_vdc5_register.h
* @version      1.00
* $Rev: 199 $
* $Date:: 2014-05-23 16:33:52 +0900#$
* @brief        VDC5 driver register setup definitions
******************************************************************************/

#ifndef R_VDC5_REGISTER_H
#define R_VDC5_REGISTER_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_vdc5.h"
#include    "r_vdc5_user.h"


/******************************************************************************
Macro definitions
******************************************************************************/
#define     VDC5_GAM_LUT_REG_NUM    (16u)   /*!< The number of table setting register in gamma correction block */
#define     VDC5_GAM_AREA_REG_NUM   (8u)    /*!< The number of area setting register in gamma correction block */


/******************************************************************************
Typedef definitions
******************************************************************************/
/*! VDC5 input controller register address list */
typedef struct {
    volatile uint32_t * inp_update;
    volatile uint32_t * inp_sel_cnt;
    volatile uint32_t * inp_ext_sync_cnt;
    volatile uint32_t * inp_vsync_ph_adj;
    volatile uint32_t * inp_dly_adj;
    volatile uint32_t * imgcnt_update;
    volatile uint32_t * imgcnt_nr_cnt0;
    volatile uint32_t * imgcnt_nr_cnt1;
} vdc5_regaddr_input_ctrl_t;

/*! VDC5 scaler register address list */
typedef struct {
    volatile uint32_t * scl0_update;
    volatile uint32_t * scl0_frc1;
    volatile uint32_t * scl0_frc2;
    volatile uint32_t * scl0_frc3;
    volatile uint32_t * scl0_frc4;
    volatile uint32_t * scl0_frc5;
    volatile uint32_t * scl0_frc6;
    volatile uint32_t * scl0_frc7;
    volatile uint32_t * scl0_frc9;
    volatile uint16_t * scl0_mon0;
    volatile uint16_t * scl0_int;
    volatile uint32_t * scl0_ds1;
    volatile uint32_t * scl0_ds2;
    volatile uint32_t * scl0_ds3;
    volatile uint32_t * scl0_ds4;
    volatile uint32_t * scl0_ds5;
    volatile uint32_t * scl0_ds6;
    volatile uint32_t * scl0_ds7;
    volatile uint32_t * scl0_us1;
    volatile uint32_t * scl0_us2;
    volatile uint32_t * scl0_us3;
    volatile uint32_t * scl0_us4;
    volatile uint32_t * scl0_us5;
    volatile uint32_t * scl0_us6;
    volatile uint32_t * scl0_us7;
    volatile uint32_t * scl0_us8;
    volatile uint32_t * scl0_ovr1;
    volatile uint32_t * scl1_update;
    volatile uint32_t * scl1_wr1;
    volatile uint32_t * scl1_wr2;
    volatile uint32_t * scl1_wr3;
    volatile uint32_t * scl1_wr4;
    volatile uint32_t * scl1_wr5;
    volatile uint32_t * scl1_wr6;
    volatile uint32_t * scl1_wr7;
    volatile uint32_t * scl1_wr8;
    volatile uint32_t * scl1_wr9;
    volatile uint32_t * scl1_wr10;
    volatile uint32_t * scl1_wr11;
    volatile uint32_t * scl1_mon1;
    volatile uint32_t * scl1_pbuf0;
    volatile uint32_t * scl1_pbuf1;
    volatile uint32_t * scl1_pbuf2;
    volatile uint32_t * scl1_pbuf3;
    volatile uint32_t * scl1_pbuf_fld;
    volatile uint32_t * scl1_pbuf_cnt;
} vdc5_regaddr_scaler_t;

/*! VDC5 image quality improver register address list */
typedef struct {
    volatile uint32_t * adj_update;
    volatile uint32_t * adj_bkstr_set;
    volatile uint32_t * adj_enh_tim1;
    volatile uint32_t * adj_enh_tim2;
    volatile uint32_t * adj_enh_tim3;
    volatile uint32_t * adj_enh_shp1;
    volatile uint32_t * adj_enh_shp2;
    volatile uint32_t * adj_enh_shp3;
    volatile uint32_t * adj_enh_shp4;
    volatile uint32_t * adj_enh_shp5;
    volatile uint32_t * adj_enh_shp6;
    volatile uint32_t * adj_enh_lti1;
    volatile uint32_t * adj_enh_lti2;
} vdc5_regaddr_img_qlty_imp_t;

/*! VDC5 color matrix register address list */
typedef struct {
    volatile uint32_t * mtx_update;
    volatile uint32_t * mtx_mode;
    volatile uint32_t * mtx_yg_adj0;
    volatile uint32_t * mtx_yg_adj1;
    volatile uint32_t * mtx_cbb_adj0;
    volatile uint32_t * mtx_cbb_adj1;
    volatile uint32_t * mtx_crr_adj0;
    volatile uint32_t * mtx_crr_adj1;
} vdc5_regaddr_color_matrix_t;

/*! VDC5 image synthesizer register address list */
typedef struct {
    volatile uint32_t * gr_update;
    volatile uint32_t * gr_flm_rd;
    volatile uint32_t * gr_flm1;
    volatile uint32_t * gr_flm2;
    volatile uint32_t * gr_flm3;
    volatile uint32_t * gr_flm4;
    volatile uint32_t * gr_flm5;
    volatile uint32_t * gr_flm6;
    volatile uint32_t * gr_ab1;
    volatile uint32_t * gr_ab2;
    volatile uint32_t * gr_ab3;
    volatile uint32_t * gr_ab4;
    volatile uint32_t * gr_ab5;
    volatile uint32_t * gr_ab6;
    volatile uint32_t * gr_ab7;
    volatile uint32_t * gr_ab8;
    volatile uint32_t * gr_ab9;
    volatile uint32_t * gr_ab10;
    volatile uint32_t * gr_ab11;
    volatile uint32_t * gr_base;
    volatile uint32_t * gr_clut;
    volatile uint32_t * gr_mon;
} vdc5_regaddr_img_synthesizer_t;

/*! VDC5 gamma correction register address list */
typedef struct {
    volatile uint32_t * gam_sw;
    volatile uint32_t * gam_g_update;
    volatile uint32_t * gam_g_lut[VDC5_GAM_LUT_REG_NUM];
    volatile uint32_t * gam_g_area[VDC5_GAM_AREA_REG_NUM];
    volatile uint32_t * gam_b_update;
    volatile uint32_t * gam_b_lut[VDC5_GAM_LUT_REG_NUM];
    volatile uint32_t * gam_b_area[VDC5_GAM_AREA_REG_NUM];
    volatile uint32_t * gam_r_update;
    volatile uint32_t * gam_r_lut[VDC5_GAM_LUT_REG_NUM];
    volatile uint32_t * gam_r_area[VDC5_GAM_AREA_REG_NUM];
} vdc5_regaddr_gamma_t;

/*! VDC5 output controller register address list */
typedef struct {
    volatile uint32_t * tcon_update;
    volatile uint32_t * tcon_tim;
    volatile uint32_t * tcon_tim_stva1;
    volatile uint32_t * tcon_tim_stva2;
    volatile uint32_t * tcon_tim_stvb1;
    volatile uint32_t * tcon_tim_stvb2;
    volatile uint32_t * tcon_tim_sth1;
    volatile uint32_t * tcon_tim_sth2;
    volatile uint32_t * tcon_tim_stb1;
    volatile uint32_t * tcon_tim_stb2;
    volatile uint32_t * tcon_tim_cpv1;
    volatile uint32_t * tcon_tim_cpv2;
    volatile uint32_t * tcon_tim_pola1;
    volatile uint32_t * tcon_tim_pola2;
    volatile uint32_t * tcon_tim_polb1;
    volatile uint32_t * tcon_tim_polb2;
    volatile uint32_t * tcon_tim_de;
    volatile uint32_t * out_update;
    volatile uint32_t * out_set;
    volatile uint32_t * out_bright1;
    volatile uint32_t * out_bright2;
    volatile uint32_t * out_contrast;
    volatile uint32_t * out_pdtha;
    volatile uint32_t * out_clk_phase;
} vdc5_regaddr_output_ctrl_t;

/*! VDC5 system controller register address list */
typedef struct {
    volatile uint32_t * syscnt_int1;
    volatile uint32_t * syscnt_int2;
    volatile uint32_t * syscnt_int3;
    volatile uint32_t * syscnt_int4;
    volatile uint32_t * syscnt_int5;
    volatile uint32_t * syscnt_int6;
    volatile uint16_t * syscnt_panel_clk;
    volatile uint16_t * syscnt_clut;
} vdc5_regaddr_system_ctrl_t;

/*! LVDS register address list */
typedef struct {
    volatile uint32_t * lvds_update;
    volatile uint32_t * lvdsfcl;
    volatile uint32_t * lclkselr;
    volatile uint32_t * lpllsetr;
    volatile uint32_t * lphyacc;
} vdc5_regaddr_lvds_t;


/******************************************************************************
Variable Externs
******************************************************************************/
extern const vdc5_regaddr_input_ctrl_t      vdc5_regaddr_input_ctrl[VDC5_CHANNEL_NUM];
extern const vdc5_regaddr_scaler_t          vdc5_regaddr_scaler[VDC5_CHANNEL_NUM][VDC5_SC_TYPE_NUM];
extern const vdc5_regaddr_img_qlty_imp_t    vdc5_regaddr_img_qlty_imp[VDC5_CHANNEL_NUM][VDC5_IMG_IMPRV_NUM];
extern const vdc5_regaddr_color_matrix_t    vdc5_regaddr_color_matrix[VDC5_CHANNEL_NUM][VDC5_COLORMTX_NUM];
extern const vdc5_regaddr_img_synthesizer_t vdc5_regaddr_img_synthesizer[VDC5_CHANNEL_NUM][VDC5_GR_TYPE_NUM];
extern uint32_t                     * const vdc5_regaddr_clut[VDC5_CHANNEL_NUM][VDC5_GR_TYPE_NUM];
extern const vdc5_regaddr_output_ctrl_t     vdc5_regaddr_output_ctrl[VDC5_CHANNEL_NUM];
extern const vdc5_regaddr_gamma_t           vdc5_regaddr_gamma[VDC5_CHANNEL_NUM];
extern const vdc5_regaddr_system_ctrl_t     vdc5_regaddr_system_ctrl[VDC5_CHANNEL_NUM];
extern const vdc5_regaddr_lvds_t            vdc5_regaddr_lvds;


/******************************************************************************
Functions Prototypes
******************************************************************************/
void VDC5_Initialize(const vdc5_channel_t ch, const vdc5_init_t * const param);
void VDC5_Terminate(const vdc5_channel_t ch);
void VDC5_VideoInput(const vdc5_channel_t ch, const vdc5_input_t * const param);
void VDC5_SyncControl(const vdc5_channel_t ch, const vdc5_sync_ctrl_t * const param);
void VDC5_DisplayOutput(const vdc5_channel_t ch, const vdc5_output_t * const param);
void VDC5_WriteDataControl(
    const vdc5_channel_t        ch,
    const vdc5_scaling_type_t   scaling_id,
    const vdc5_write_t  * const param);
void VDC5_ChangeWriteProcess(
    const vdc5_channel_t            ch,
    const vdc5_scaling_type_t       scaling_id,
    const vdc5_write_chg_t  * const param);
void VDC5_ReadDataControl(
    const vdc5_channel_t        ch,
    const vdc5_graphics_type_t  graphics_id,
    const vdc5_read_t   * const param);
void VDC5_ChangeReadProcess(
    const vdc5_channel_t            ch,
    const vdc5_graphics_type_t      graphics_id,
    const vdc5_read_chg_t   * const param);
void VDC5_StartProcess(const vdc5_channel_t ch, const vdc5_layer_id_t layer_id, const vdc5_start_t * const param);
void VDC5_StopProcess(const vdc5_channel_t ch, const vdc5_layer_id_t layer_id);
void VDC5_ReleaseDataControl(const vdc5_channel_t ch, const vdc5_layer_id_t layer_id);
void VDC5_VideoNoiseReduction(
    const vdc5_channel_t                    ch,
    const vdc5_onoff_t                      nr1d_on,
    const vdc5_noise_reduction_t    * const param);
void VDC5_ImageColorMatrix(const vdc5_channel_t ch, const vdc5_color_matrix_t * const param);
void VDC5_ImageEnhancement(
    const vdc5_channel_t                ch,
    const vdc5_imgimprv_id_t            imgimprv_id,
    const vdc5_onoff_t                  shp_h_on,
    const vdc5_enhance_sharp_t  * const sharp_param,
    const vdc5_onoff_t                  lti_h_on,
    const vdc5_enhance_lti_t    * const lti_param,
    const vdc5_period_rect_t    * const enh_area);
void VDC5_ImageBlackStretch(
    const vdc5_channel_t        ch,
    const vdc5_imgimprv_id_t    imgimprv_id,
    const vdc5_onoff_t          bkstr_on,
    const vdc5_black_t  * const param);
void VDC5_AlphaBlending(
    const vdc5_channel_t                ch,
    const vdc5_graphics_type_t          graphics_id,
    const vdc5_alpha_blending_t * const param);
void VDC5_AlphaBlendingRect(
    const vdc5_channel_t                        ch,
    const vdc5_graphics_type_t                  graphics_id,
    const vdc5_onoff_t                          gr_arc_on,
    const vdc5_alpha_blending_rect_t    * const param);
void VDC5_Chromakey(
    const vdc5_channel_t            ch,
    const vdc5_graphics_type_t      graphics_id,
    const vdc5_onoff_t              gr_ck_on,
    const vdc5_chromakey_t  * const param);
void VDC5_CLUT(const vdc5_channel_t ch, const vdc5_graphics_type_t graphics_id, const vdc5_clut_t * const param);
void VDC5_DisplayCalibration(const vdc5_channel_t ch, const vdc5_disp_calibration_t * const param);
void VDC5_GammaCorrection(
    const vdc5_channel_t                    ch,
    const vdc5_onoff_t                      gam_on,
    const vdc5_gamma_correction_t   * const param);

void VDC5_Int_Disable(const vdc5_channel_t ch);
void VDC5_Int_SetInterrupt(const vdc5_channel_t ch, const vdc5_int_t * const param);


#endif  /* R_VDC5_REGISTER_H */

