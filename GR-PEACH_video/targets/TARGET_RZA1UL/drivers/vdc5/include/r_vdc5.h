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
* @file         r_vdc5.h
* @version      0.06
* $Rev: 18 $
* $Date:: 2014-05-29 11:11:38 +0900#$
* @brief        RZ/A1L VDC5 driver API definitions
******************************************************************************/

#ifndef R_VDC5_H
#define R_VDC5_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_vdc5_user.h"


#ifdef  __cplusplus
extern  "C"
{
#endif  /* __cplusplus */


/******************************************************************************
Macro definitions
******************************************************************************/
#define     VDC5_GAM_GAIN_ADJ_NUM       (32u)   /*!< The number of the gamma correction gain coefficient */
#define     VDC5_GAM_START_TH_NUM       (31u)   /*!< The number of the gamma correction start threshold */


/******************************************************************************
Typedef definitions
******************************************************************************/
/*! Error codes of the VDC5 driver */
typedef enum
{
    VDC5_OK = 0,                /*!< Normal termination */
    VDC5_ERR_PARAM_CHANNEL,     /*!< Invalid channel error (parameter error): An illegal channel is specified. */
    VDC5_ERR_PARAM_LAYER_ID,    /*!< Invalid layer ID error (parameter error): An illegal layer ID is specified. */
    VDC5_ERR_PARAM_NULL,        /*!< NULL specification error (parameter error):
                                     NULL is specified for a required parameter. */
    VDC5_ERR_PARAM_BIT_WIDTH,   /*!< Bit width error (parameter error):
                                     A value exceeding the possible bit width is specified. */
    VDC5_ERR_PARAM_UNDEFINED,   /*!< Undefined parameter error (parameter error):
                                     A value that is not defined in the specification is specified. */
    VDC5_ERR_PARAM_EXCEED_RANGE,/*!< Out-of-value-range error (parameter error):
                                     The specified parameter value is beyond the value range defined
                                     in the specification. */
    VDC5_ERR_PARAM_CONDITION,   /*!< Unauthorized condition error (parameter error):
                                     A parameter is specified under conditions that are not authorized
                                     by the specification. */
    VDC5_ERR_IF_CONDITION,      /*!< Interface condition error (interface error):
                                     An API function is called under unauthorized conditions. */
    VDC5_ERR_RESOURCE_CLK,      /*!< Clock resource error (resource error): No panel clock is set up. */
    VDC5_ERR_RESOURCE_VSYNC,    /*!< Vsync signal resource error (resource error): No Vsync signal is set up. */
    VDC5_ERR_RESOURCE_INPUT,    /*!< Input signal resource error (resource error): No video image input is set up. */
    VDC5_ERR_RESOURCE_OUTPUT,   /*!< Output resource error (resource error): No display output is set up. */
    VDC5_ERR_RESOURCE_LAYER,    /*!< Layer resource error (resource error):
                                     The specified layer is under unavailable conditions. */
    VDC5_ERR_NUM                /*!< The number of error codes */
} vdc5_error_t;

/*! VDC5 channels */
typedef enum
{
    VDC5_CHANNEL_0 = 0,         /*!< Channel 0 */
    VDC5_CHANNEL_NUM            /*!< The number of channels */
} vdc5_channel_t;

/*! On/off */
typedef enum
{
    VDC5_OFF    = 0,            /*!< Off */
    VDC5_ON     = 1             /*!< On */
} vdc5_onoff_t;
/*! Edge of a signal */
typedef enum
{
    VDC5_EDGE_RISING    = 0,    /*!< Rising edge */
    VDC5_EDGE_FALLING   = 1     /*!< Falling edge */
} vdc5_edge_t;
/*! Polarity of a signal */
typedef enum
{
    VDC5_SIG_POL_NOT_INVERTED   = 0,    /*!< Not inverted */
    VDC5_SIG_POL_INVERTED       = 1     /*!< Inverted */
} vdc5_sig_pol_t;

/*! Scaling type ID */
typedef enum
{
    VDC5_SC_TYPE_SC0 = 0,               /*!< Scaler 0 */
    VDC5_SC_TYPE_NUM                    /*!< The number of scaler types */
} vdc5_scaling_type_t;
/*! Graphics type ID */
typedef enum
{
    VDC5_GR_TYPE_GR0 = 0,               /*!< Graphics 0 */
    VDC5_GR_TYPE_GR2,                   /*!< Graphics 2 */
    VDC5_GR_TYPE_GR3,                   /*!< Graphics 3 */
    VDC5_GR_TYPE_NUM                    /*!< The number of graphics types */
} vdc5_graphics_type_t;
/*! Layer ID */
typedef enum
{
    VDC5_LAYER_ID_ALL       = -1,                                       /*!< All layers */
    VDC5_LAYER_ID_0_WR      = (VDC5_SC_TYPE_SC0 + 0),                   /*!< Write process for layer 0 */
    VDC5_LAYER_ID_0_RD      = (VDC5_SC_TYPE_NUM + VDC5_GR_TYPE_GR0),    /*!< Read process for layer 0 */
    VDC5_LAYER_ID_2_RD      = (VDC5_SC_TYPE_NUM + VDC5_GR_TYPE_GR2),    /*!< Read process for layer 2 */
    VDC5_LAYER_ID_3_RD      = (VDC5_SC_TYPE_NUM + VDC5_GR_TYPE_GR3),    /*!< Read process for layer 3 */
    VDC5_LAYER_ID_NUM       = (VDC5_SC_TYPE_NUM + VDC5_GR_TYPE_NUM)     /*!< The number of layer IDs */
} vdc5_layer_id_t;
/*! The horizontal/vertical timing of the VDC5 signals */
typedef struct
{
    uint16_t    vs;                 /*!< Vertical signal start position from the reference signal */
    uint16_t    vw;                 /*!< Vertical signal width (height) */
    uint16_t    hs;                 /*!< Horizontal signal start position from the reference signal */
    uint16_t    hw;                 /*!< Horizontal signal width */
} vdc5_period_rect_t;
/*! The relative position within the graphics display area */
typedef struct
{
    uint16_t   vs_rel;              /*!< Vertical start position */
    uint16_t   vw_rel;              /*!< Vertical width (height) */
    uint16_t   hs_rel;              /*!< Horizontal start position */
    uint16_t   hw_rel;              /*!< Horizontal width */
} vdc5_pd_disp_rect_t;

/***********************    For R_VDC5_Initialize       ***********************/
/*! Panel clock select */
typedef enum
{
    VDC5_PANEL_ICKSEL_IMG_DV = 0,   /*!< Divided video image clock (DV_CLK) */
    VDC5_PANEL_ICKSEL_EXT_0,        /*!< Divided external clock (LCD0_EXTCLK) */
    VDC5_PANEL_ICKSEL_PERI,         /*!< Divided peripheral clock 1 */
    VDC5_PANEL_ICKSEL_NUM           /*!< The number of panel clock select settings */
} vdc5_panel_clksel_t;
/*! Clock frequency division ratio */
typedef enum
{
    VDC5_PANEL_CLKDIV_1_1 = 0,      /*!< Division Ratio 1/1 */
    VDC5_PANEL_CLKDIV_1_2,          /*!< Division Ratio 1/2 */
    VDC5_PANEL_CLKDIV_1_3,          /*!< Division Ratio 1/3 */
    VDC5_PANEL_CLKDIV_1_4,          /*!< Division Ratio 1/4 */
    VDC5_PANEL_CLKDIV_1_5,          /*!< Division Ratio 1/5 */
    VDC5_PANEL_CLKDIV_1_6,          /*!< Division Ratio 1/6 */
    VDC5_PANEL_CLKDIV_1_7,          /*!< Division Ratio 1/7 */
    VDC5_PANEL_CLKDIV_1_8,          /*!< Division Ratio 1/8 */
    VDC5_PANEL_CLKDIV_1_9,          /*!< Division Ratio 1/9 */
    VDC5_PANEL_CLKDIV_1_12,         /*!< Division Ratio 1/12 */
    VDC5_PANEL_CLKDIV_1_16,         /*!< Division Ratio 1/16 */
    VDC5_PANEL_CLKDIV_1_24,         /*!< Division Ratio 1/24 */
    VDC5_PANEL_CLKDIV_1_32,         /*!< Division Ratio 1/32 */
    VDC5_PANEL_CLKDIV_NUM           /*!< The number of division ratio settings */
} vdc5_panel_clk_dcdr_t;
/*! The clock input to frequency divider 1 */
typedef enum
{
    VDC5_LVDS_INCLK_SEL_IMG = 0,    /*!< Video image clock (VIDEO_X1) */
    VDC5_LVDS_INCLK_SEL_DV_0,       /*!< Video image clock (DV0_CLK) */
    VDC5_LVDS_INCLK_SEL_DV_1,       /*!< Video image clock (DV1_CLK) */
    VDC5_LVDS_INCLK_SEL_EXT_0,      /*!< External clock (LCD0_EXTCLK) */
    VDC5_LVDS_INCLK_SEL_EXT_1,      /*!< External clock (LCD1_EXTCLK) */
    VDC5_LVDS_INCLK_SEL_PERI,       /*!< Peripheral clock 1 */
    VDC5_LVDS_INCLK_SEL_NUM
} vdc5_lvds_in_clk_sel_t;
/*! The frequency dividing value (NIDIV or NODIV) */
typedef enum
{
    VDC5_LVDS_NDIV_1 = 0,           /*!< Div 1 */
    VDC5_LVDS_NDIV_2,               /*!< Div 2 */
    VDC5_LVDS_NDIV_4,               /*!< Div 4 */
    VDC5_LVDS_NDIV_NUM
} vdc5_lvds_ndiv_t;
/*! The frequency dividing value (NOD) for the output frequency */
typedef enum
{
    VDC5_LVDS_PLL_NOD_1 = 0,        /*!< Div 1 */
    VDC5_LVDS_PLL_NOD_2,            /*!< Div 2 */
    VDC5_LVDS_PLL_NOD_4,            /*!< Div 4 */
    VDC5_LVDS_PLL_NOD_8,            /*!< Div 8 */
    VDC5_LVDS_PLL_NOD_NUM
} vdc5_lvds_pll_nod_t;
/*! LVDS parameter */
typedef struct
{
    vdc5_lvds_in_clk_sel_t  lvds_in_clk_sel;/*!< The clock input to frequency divider 1 */
    vdc5_lvds_ndiv_t        lvds_idiv_set;  /*!< The frequency dividing value (NIDIV) for frequency divider 1 */
    uint16_t                lvdspll_tst;    /*!< Internal parameter setting for LVDS PLL */
    vdc5_lvds_ndiv_t        lvds_odiv_set;  /*!< The frequency dividing value (NODIV) for frequency divider 2 */
    vdc5_channel_t          lvds_vdc_sel;   /*!< A channel in VDC5 whose data is to be output through the LVDS */
    uint16_t                lvdspll_fd;     /*!< The frequency dividing value (NFD) for the feedback frequency */
    uint16_t                lvdspll_rd;     /*!< The frequency dividing value (NRD) for the input frequency */
    vdc5_lvds_pll_nod_t     lvdspll_od;     /*!< The frequency dividing value (NOD) for the output frequency */
} vdc5_lvds_t;
/*! Initialization parameter */
typedef struct
{
    vdc5_panel_clksel_t     panel_icksel;   /*!< Panel clock select */
    vdc5_panel_clk_dcdr_t   panel_dcdr;     /*!< Clock frequency division ratio */
    const vdc5_lvds_t     * lvds;           /*!< LVDS-related parameter */
} vdc5_init_t;

/***********************    For R_VDC5_VideoInput       ***********************/
/*! Input select */
typedef enum
{
    VDC5_INPUT_SEL_EXT      = 1     /*!< Signals supplied via the external input pins */
} vdc5_input_sel_t;
/*! Sync signal delay adjustment parameter */
typedef struct
{
    uint16_t    inp_vs_dly_l;       /*!< Number of lines for delaying Vsync signal and field differentiation signal */
    uint16_t    inp_fld_dly;        /*!< Field differentiation signal delay amount */
    uint16_t    inp_vs_dly;         /*!< Vsync signal delay amount */
    uint16_t    inp_hs_dly;         /*!< Hsync signal delay amount */
} vdc5_sync_delay_t;
/*! External input format select */
typedef enum
{
    VDC5_EXTIN_FORMAT_RGB888 = 0,   /*!< RGB888 */
    VDC5_EXTIN_FORMAT_RGB666,       /*!< RGB666 */
    VDC5_EXTIN_FORMAT_RGB565,       /*!< RGB565 */
    VDC5_EXTIN_FORMAT_BT656,        /*!< BT6556 */
    VDC5_EXTIN_FORMAT_BT601,        /*!< BT6501 */
    VDC5_EXTIN_FORMAT_YCBCR422,     /*!< YCbCr422 */
    VDC5_EXTIN_FORMAT_YCBCR444,     /*!< YCbCr444 */
    VDC5_EXTIN_FORMAT_NUM
} vdc5_extin_format_t;
/*! Reference select for external input BT.656 Hsync signal */
typedef enum
{
    VDC5_EXTIN_REF_H_EAV    = 0,    /*!< EAV */
    VDC5_EXTIN_REF_H_SAV    = 1     /*!< SAV */
} vdc5_extin_ref_hsync_t;
/*! Number of lines for BT.656 external input */
typedef enum
{
    VDC5_EXTIN_LINE_525     = 0,    /*!< 525 lines */
    VDC5_EXTIN_LINE_625     = 1     /*!< 625 lines */
} vdc5_extin_input_line_t;
/*! Y/Cb/Y/Cr data string start timing */
typedef enum
{
    VDC5_EXTIN_H_POS_CBYCRY = 0,    /*!< Cb/Y/Cr/Y (BT656/601), Cb/Cr (YCbCr422) */
    VDC5_EXTIN_H_POS_YCRYCB,        /*!< Y/Cr/Y/Cb (BT656/601), setting prohibited (YCbCr422) */
    VDC5_EXTIN_H_POS_CRYCBY,        /*!< Cr/Y/Cb/Y (BT656/601), setting prohibited (YCbCr422) */
    VDC5_EXTIN_H_POS_YCBYCR,        /*!< Y/Cb/Y/Cr (BT656/601), Cr/Cb (YCbCr422) */
    VDC5_EXTIN_H_POS_NUM
} vdc5_extin_h_pos_t;
/*! External input signal parameter */
typedef struct
{
    vdc5_extin_format_t     inp_format;     /*!< External input format select */
    vdc5_edge_t             inp_pxd_edge;   /*!< Clock edge select for capturing external input video image signals */
    vdc5_edge_t             inp_vs_edge;    /*!< Clock edge select for capturing external input Vsync signals */
    vdc5_edge_t             inp_hs_edge;    /*!< Clock edge select for capturing external input Hsync signals */
    vdc5_onoff_t            inp_endian_on;  /*!< External input bit endian change on/off control */
    vdc5_onoff_t            inp_swap_on;    /*!< External input B/R signal swap on/off control */
    vdc5_sig_pol_t          inp_vs_inv;     /*!< External input Vsync signal DV_VSYNC inversion control */
    vdc5_sig_pol_t          inp_hs_inv;     /*!< External input Hsync signal DV_HSYNC inversion control */
    vdc5_extin_ref_hsync_t  inp_h_edge_sel; /*!< Reference select for external input BT.656 Hsync signal */
    vdc5_extin_input_line_t inp_f525_625;   /*!< Number of lines for BT.656 external input */
    vdc5_extin_h_pos_t      inp_h_pos;      /*!< Y/Cb/Y/Cr data string start timing to Hsync reference */
} vdc5_ext_in_sig_t;
/*! Video input setup parameter */
typedef struct
{
    vdc5_input_sel_t            inp_sel;    /*!< Input select */
    uint16_t                    inp_fh50;   /*!< Vsync signal 1/2fH phase timing */
    uint16_t                    inp_fh25;   /*!< Vsync signal 1/4fH phase timing */
    const vdc5_sync_delay_t   * dly;        /*!< Sync signal delay adjustment parameter */
    const vdc5_ext_in_sig_t   * ext_sig;    /*!< External input signal parameter */
} vdc5_input_t;

/***********************    For R_VDC5_SyncControl      ***********************/
/*! Horizontal/vertical sync signal output and full-screen enable signal select */
typedef enum
{
    VDC5_RES_VS_IN_SEL_SC0  = 0     /*!< Sync signal output and full-screen enable signal from scaler 0 */
} vdc5_res_vs_in_sel_t;
/*! Vsync signal compensation parameter */
typedef struct
{
    uint16_t        res_vmask;              /*!< Repeated Vsync signal masking period */
    uint16_t        res_vlack;              /*!< Missing-Sync compensating pulse output wait time */
} vdc5_vsync_cpmpe_t;
/*! Synchronization control parameter */
typedef struct
{
    vdc5_onoff_t                res_vs_sel;     /*!< Vsync signal output select (free-running Vsync on/off control) */
    vdc5_res_vs_in_sel_t        res_vs_in_sel;  /*!< Horizontal/vertical sync signal output
                                                     and full-screen enable signal select */
    uint16_t                    res_fv;         /*!< Free-running Vsync period setting */
    uint16_t                    res_fh;         /*!< Hsync period setting */
    uint16_t                    res_vsdly;      /*!< Vsync signal delay control */
    vdc5_period_rect_t          res_f;          /*!< Full-screen enable signal */
    const vdc5_vsync_cpmpe_t  * vsync_cpmpe;    /*!< Vsync signal compensation parameter */
} vdc5_sync_ctrl_t;

/***********************    For R_VDC5_DisplayOutput    ***********************/
/*! POLA/POLB signal generation mode select */
typedef enum
{
    VDC5_LCD_TCON_POLMD_NORMAL = 0,         /*!< Normal mode */
    VDC5_LCD_TCON_POLMD_1X1REV,             /*!< 1x1 reverse mode */
    VDC5_LCD_TCON_POLMD_1X2REV,             /*!< 1x2 reverse mode */
    VDC5_LCD_TCON_POLMD_2X2REV,             /*!< 2x2 reverse mode */
    VDC5_LCD_TCON_POLMD_NUM
} vdc5_lcd_tcon_polmode_t;
/*! Signal operating reference select */
typedef enum
{
    VDC5_LCD_TCON_REFSEL_HSYNC      = 0,    /*!< Hsync signal reference */
    VDC5_LCD_TCON_REFSEL_OFFSET_H   = 1     /*!< Offset Hsync signal reference */
} vdc5_lcd_tcon_refsel_t;
/*! LCD TCON output pin select */
typedef enum
{
    VDC5_LCD_TCON_PIN_NON = -1,             /*!< Nothing output */
    VDC5_LCD_TCON_PIN_0,                    /*!< LCD_TCON0 */
    VDC5_LCD_TCON_PIN_1,                    /*!< LCD_TCON1 */
    VDC5_LCD_TCON_PIN_2,                    /*!< LCD_TCON2 */
    VDC5_LCD_TCON_PIN_3,                    /*!< LCD_TCON3 */
    VDC5_LCD_TCON_PIN_4,                    /*!< LCD_TCON4 */
    VDC5_LCD_TCON_PIN_5,                    /*!< LCD_TCON5 */
    VDC5_LCD_TCON_PIN_6,                    /*!< LCD_TCON6 */
    VDC5_LCD_TCON_PIN_NUM
} vdc5_lcd_tcon_pin_t;
/*! LCD TCON timing signal parameter */
typedef struct
{
    uint16_t                tcon_hsvs;      /*!< Signal pulse start position */
    uint16_t                tcon_hwvw;      /*!< Pulse width */
    vdc5_lcd_tcon_polmode_t tcon_md;        /*!< POLA/POLB signal generation mode select */
    vdc5_lcd_tcon_refsel_t  tcon_hs_sel;    /*!< Signal operating reference select */
    vdc5_sig_pol_t          tcon_inv;       /*!< Polarity inversion control of signal */
    vdc5_lcd_tcon_pin_t     tcon_pin;       /*!< LCD TCON output pin select */
    vdc5_edge_t             outcnt_edge;    /*!< Output phase control of signal */
} vdc5_lcd_tcon_timing_t;
/*! Timing signals for driving the LCD panel */
typedef enum
{
    VDC5_LCD_TCONSIG_STVA_VS = 0,           /*!< STVA/VS */
    VDC5_LCD_TCONSIG_STVB_VE,               /*!< STVB/VE */
    VDC5_LCD_TCONSIG_STH_SP_HS,             /*!< STH/SP/HS */
    VDC5_LCD_TCONSIG_STB_LP_HE,             /*!< STB/LP/HE */
    VDC5_LCD_TCONSIG_CPV_GCK,               /*!< CPV/GCK */
    VDC5_LCD_TCONSIG_POLA,                  /*!< POLA */
    VDC5_LCD_TCONSIG_POLB,                  /*!< POLB */
    VDC5_LCD_TCONSIG_DE,                    /*!< DE */
    VDC5_LCD_TCONSIG_NUM
} vdc5_lcd_tcon_sigsel_t;
/*! Output format select */
typedef enum
{
    VDC5_LCD_OUTFORMAT_RGB888 = 0,          /*!< RGB888 */
    VDC5_LCD_OUTFORMAT_RGB666,              /*!< RGB666 */
    VDC5_LCD_OUTFORMAT_RGB565,              /*!< RGB565 */
    VDC5_LCD_OUTFORMAT_SERIAL_RGB,          /*!< Serial RGB */
    VDC5_LCD_OUTFORMAT_NUM
} vdc5_lcd_outformat_t;
/*! Clock frequency control */
typedef enum
{
    VDC5_LCD_PARALLEL_CLKFRQ_1 = 0,         /*!< 100% speed (parallel RGB) */
    VDC5_LCD_SERIAL_CLKFRQ_3,               /*!< Triple speed (serial RGB) */
    VDC5_LCD_SERIAL_CLKFRQ_4,               /*!< Quadruple speed (serial RGB) */
    VDC5_LCD_SERIAL_CLKFRQ_NUM
} vdc5_lcd_clkfreqsel_t;
/*! Scan direction select */
typedef enum
{
    VDC5_LCD_SERIAL_SCAN_FORWARD   = 0,     /*!< Forward scan */
    VDC5_LCD_SERIAL_SCAN_REVERSE   = 1      /*!< Reverse scan */
} vdc5_lcd_scan_t;
/*! Clock phase adjustment for serial RGB output */
typedef enum
{
    VDC5_LCD_SERIAL_CLKPHASE_0 = 0,         /*!< 0[clk] */
    VDC5_LCD_SERIAL_CLKPHASE_1,             /*!< 1[clk] */
    VDC5_LCD_SERIAL_CLKPHASE_2,             /*!< 2[clk] */
    VDC5_LCD_SERIAL_CLKPHASE_3,             /*!< 3[clk] */
    VDC5_LCD_SERIAL_CLKPHASE_NUM
} vdc5_lcd_clkphase_t;
/*! Display output configuration parameter */
typedef struct
{
    uint16_t                        tcon_half;                      /*!< 1/2fH timing */
    uint16_t                        tcon_offset;                    /*!< Offset Hsync signal timing */
    const vdc5_lcd_tcon_timing_t  * outctrl[VDC5_LCD_TCONSIG_NUM];  /*!< LCD TCON timing signal parameter */
    vdc5_edge_t                     outcnt_lcd_edge;                /*!< Output phase control of LCD_DATA23
                                                                         to LCD_DATA0 pin */
    vdc5_onoff_t                    out_endian_on;                  /*!< Bit endian change on/off control */
    vdc5_onoff_t                    out_swap_on;                    /*!< B/R signal swap on/off control */
    vdc5_lcd_outformat_t            out_format;                     /*!< Output format select */
    vdc5_lcd_clkfreqsel_t           out_frq_sel;                    /*!< Clock frequency control */
    vdc5_lcd_scan_t                 out_dir_sel;                    /*!< Scan direction select */
    vdc5_lcd_clkphase_t             out_phase;                      /*!< Clock phase adjustment
                                                                         for serial RGB output */
    uint32_t                        bg_color;                       /*!< Background color in 24-bit RGB color format */
} vdc5_output_t;

/***********************    For R_VDC5_CallbackISR      ***********************/
/*! VDC5 interrupt type */
typedef enum
{
    VDC5_INT_TYPE_S0_VI_VSYNC = 0,      /*!< Vsync signal input to scaler 0 */
    VDC5_INT_TYPE_S0_LO_VSYNC,          /*!< Vsync signal output from scaler 0 */
    VDC5_INT_TYPE_S0_VSYNCERR,          /*!< Missing Vsync signal for scaler 0 */
    VDC5_INT_TYPE_VLINE,                /*!< Specified line signal for panel output in graphics 3 */
    VDC5_INT_TYPE_S0_VFIELD,            /*!< Field end signal for recording function in scaler 0 */
    VDC5_INT_TYPE_IV1_VBUFERR,          /*!< Frame buffer write overflow signal for scaler 0 */
    VDC5_INT_TYPE_IV3_VBUFERR,          /*!< Frame buffer read underflow signal for graphics 0 */
    VDC5_INT_TYPE_IV5_VBUFERR,          /*!< Frame buffer read underflow signal for graphics 2 */
    VDC5_INT_TYPE_IV6_VBUFERR,          /*!< Frame buffer read underflow signal for graphics 3 */
    VDC5_INT_TYPE_S0_WLINE,             /*!< Write specification line signal input to scaling-down control block
                                            in scaler 0 */
    VDC5_INT_TYPE_NUM                   /*!< The number of VDC5 interrupt types */
} vdc5_int_type_t;
/*! Interrupt callback setup parameter */
typedef struct
{
    vdc5_int_type_t     type;                       /*!< VDC5 interrupt type */
    void             (* callback)(vdc5_int_type_t); /*!< Interrupt callback function pointer */
    uint16_t            line_num;                   /*!< Line interrupt set */
} vdc5_int_t;

/***********************    For R_VDC5_WriteDataControl ***********************/
/*! Frame buffer writing mode for image processing */
typedef enum
{
    VDC5_WR_MD_NORMAL = 0,                  /*!< Normal */
    VDC5_WR_MD_MIRROR,                      /*!< Horizontal mirroring */
    VDC5_WR_MD_ROT_90DEG,                   /*!< 90 degree rotation */
    VDC5_WR_MD_ROT_180DEG,                  /*!< 180 degree rotation */
    VDC5_WR_MD_ROT_270DEG,                  /*!< 270 degree rotation */
    VDC5_WR_MD_NUM
} vdc5_wr_md_t;
/*! Scaling-down and rotation parameter */
typedef struct
{
    vdc5_period_rect_t  res;                /*!< Image area to be captured */
    vdc5_onoff_t        res_pfil_sel;       /*!< Prefilter mode select for brightness signals (on/off) */
    uint16_t            res_out_vw;         /*!< Number of valid lines in vertical direction
                                                 output by scaling-down control block */
    uint16_t            res_out_hw;         /*!< Number of valid horizontal pixels
                                                 output by scaling-down control block */
    vdc5_onoff_t        adj_sel;            /*!< Handling for lack of last-input line (on/off) */
    vdc5_wr_md_t        res_ds_wr_md;       /*!< Frame buffer writing mode for image processing */
} vdc5_scalingdown_rot_t;
/*! Frame buffer swap setting */
typedef enum
{
    VDC5_WR_RD_WRSWA_NON = 0,           /*!< Not swapped: 1-2-3-4-5-6-7-8 */
    VDC5_WR_RD_WRSWA_8BIT,              /*!< Swapped in 8-bit units: 2-1-4-3-6-5-8-7 */
    VDC5_WR_RD_WRSWA_16BIT,             /*!< Swapped in 16-bit units: 3-4-1-2-7-8-5-6 */
    VDC5_WR_RD_WRSWA_16_8BIT,           /*!< Swapped in 16-bit units + 8-bit units: 4-3-2-1-8-7-6-5 */
    VDC5_WR_RD_WRSWA_32BIT,             /*!< Swapped in 32-bit units: 5-6-7-8-1-2-3-4 */
    VDC5_WR_RD_WRSWA_32_8BIT,           /*!< Swapped in 32-bit units + 8-bit units: 6-5-8-7-2-1-4-3 */
    VDC5_WR_RD_WRSWA_32_16BIT,          /*!< Swapped in 32-bit units + 16-bit units: 7-8-5-6-3-4-1-2 */
    VDC5_WR_RD_WRSWA_32_16_8BIT,        /*!< Swapped in 32-bit units + 16-bit units + 8-bit units: 8-7-6-5-4-3-2-1 */
    VDC5_WR_RD_WRSWA_NUM
} vdc5_wr_rd_swa_t;
/*! Frame buffer video-signal writing format */
typedef enum
{
    VDC5_RES_MD_YCBCR422 = 0,               /*!< YCbCr422 */
    VDC5_RES_MD_RGB565,                     /*!< RGB565 */
    VDC5_RES_MD_RGB888,                     /*!< RGB888 */
    VDC5_RES_MD_YCBCR444,                   /*!< YCbCr444 */
    VDC5_RES_MD_NUM
} vdc5_res_md_t;
/*! Transfer burst length */
typedef enum
{
    VDC5_BST_MD_32BYTE = 0,                 /*!< 32-byte transfer (4 bursts) */
    VDC5_BST_MD_128BYTE                     /*!< 128-byte transfer (16 bursts) */
} vdc5_bst_md_t;
/*! Field operating mode select */
typedef enum
{
    VDC5_RES_INTER_PROGRESSIVE  = 0,        /*!< Progressive */
    VDC5_RES_INTER_INTERLACE    = 1         /*!< Interlace */
} vdc5_res_inter_t;
/*! Writing rate */
typedef enum
{
    VDC5_RES_FS_RATE_PER1 = 0,              /* 1/1 an input signal */
    VDC5_RES_FS_RATE_PER2,                  /* 1/2 an input signal */
    VDC5_RES_FS_RATE_PER4,                  /* 1/4 an input signal */
    VDC5_RES_FS_RATE_PER8,                  /* 1/8 an input signal */
    VDC5_RES_FS_RATE_NUM
} vdc5_res_fs_rate_t;
/*! Write field select */
typedef enum
{
    VDC5_RES_FLD_SEL_TOP    = 0,            /*!< Top field */
    VDC5_RES_FLD_SEL_BOTTOM = 1             /*!< Bottom field */
} vdc5_res_fld_sel_t;
/*! Data write control parameter */
typedef struct
{
    vdc5_scalingdown_rot_t  scalingdown_rot;/*!< Scaling-down and rotation parameter */
    vdc5_wr_rd_swa_t        res_wrswa;      /*!< Swap setting in frame buffer writing */
    vdc5_res_md_t           res_md;         /*!< Frame buffer video-signal writing format */
    vdc5_bst_md_t           res_bst_md;     /*!< Transfer burst length for frame buffer writing */
    vdc5_res_inter_t        res_inter;      /*!< Field operating mode select */
    vdc5_res_fs_rate_t      res_fs_rate;    /*!< Writing rate */
    vdc5_res_fld_sel_t      res_fld_sel;    /*!< Write field select */
    vdc5_onoff_t            res_dth_on;     /*!< Dither correction on/off */
    void                  * base;           /*!< Frame buffer base address */
    uint32_t                ln_off;         /*!< Frame buffer line offset address [byte] */
    uint32_t                flm_num;        /*!< Number of frames of buffer to be written to (res_flm_num + 1) */
    uint32_t                flm_off;        /*!< Frame buffer frame offset address [byte] */
    void                  * btm_base;       /*!< Frame buffer base address for bottom */
} vdc5_write_t;

/***********************  For R_VDC5_ChangeWriteProcess ***********************/
/*! Data write change parameter */
typedef struct
{
    vdc5_scalingdown_rot_t  scalingdown_rot;/*!< Scaling-down and rotation parameter */
} vdc5_write_chg_t;

/***********************  For R_VDC5_ReadDataControl    ***********************/
/*! Line offset address direction of the frame buffer */
typedef enum
{
    VDC5_GR_LN_OFF_DIR_INC = 0,             /*!< Increments the address by the line offset address */
    VDC5_GR_LN_OFF_DIR_DEC                  /*!< Decrements the address by the line offset address */
} vdc5_gr_ln_off_dir_t;
/*! Frame buffer address setting signal */
typedef enum
{
    VDC5_GR_FLM_SEL_SCALE_DOWN      = 0,        /*!< Links to scaling-down process */
    VDC5_GR_FLM_SEL_FLM_NUM         = 1,        /*!< Selects frame 0 (graphics display) */
    VDC5_GR_FLM_SEL_POINTER_BUFF    = 3         /*!< Links to pointer buffer */
} vdc5_gr_flm_sel_t;
/*! Size of the frame buffer to be read */
typedef struct
{
    uint16_t                in_vw;          /*!< Number of lines in a frame */
    uint16_t                in_hw;          /*!< Width of the horizontal valid period */
} vdc5_width_read_fb_t;
/*! Format of the frame buffer read signal */
typedef enum
{
    VDC5_GR_FORMAT_RGB565 = 0,              /*!< RGB565 */
    VDC5_GR_FORMAT_RGB888,                  /*!< RGB888 */
    VDC5_GR_FORMAT_ARGB1555,                /*!< ARGB1555 */
    VDC5_GR_FORMAT_ARGB4444,                /*!< ARGB4444 */
    VDC5_GR_FORMAT_ARGB8888,                /*!< ARGB8888 */
    VDC5_GR_FORMAT_CLUT8,                   /*!< CLUT8 */
    VDC5_GR_FORMAT_CLUT4,                   /*!< CLUT4 */
    VDC5_GR_FORMAT_CLUT1,                   /*!< CLUT1 */
    VDC5_GR_FORMAT_YCBCR422,                /*!< YCbCr422: This setting is prohibited for the graphics 2 and 3 */
    VDC5_GR_FORMAT_YCBCR444,                /*!< YCbCr444: This setting is prohibited for the graphics 2 and 3 */
    VDC5_GR_FORMAT_RGBA5551,                /*!< RGBA5551 */
    VDC5_GR_FORMAT_RGBA8888,                /*!< RGBA8888 */
    VDC5_GR_FORMAT_NUM                      /*!< The number of signal formats */
} vdc5_gr_format_t;
/*! Swapping of data read from buffer in the YCbCr422 format */
typedef enum
{
    VDC5_GR_YCCSWAP_CBY0CRY1 = 0,
    VDC5_GR_YCCSWAP_Y0CBY1CR,
    VDC5_GR_YCCSWAP_CRY0CBY1,
    VDC5_GR_YCCSWAP_Y0CRY1CB,
    VDC5_GR_YCCSWAP_Y1CRY0CB,
    VDC5_GR_YCCSWAP_CRY1CBY0,
    VDC5_GR_YCCSWAP_Y1CBY0CR,
    VDC5_GR_YCCSWAP_CBY1CRY0,
    VDC5_GR_YCCSWAP_NUM
} vdc5_gr_ycc_swap_t;
/*! Data read control parameter */
typedef struct
{
    vdc5_gr_ln_off_dir_t            gr_ln_off_dir;  /*!< Line offset address direction of the frame buffer */
    vdc5_gr_flm_sel_t               gr_flm_sel;     /*!< Frame buffer address setting signal */
    vdc5_onoff_t                    gr_imr_flm_inv; /*!< Frame buffer number for distortion correction */
    vdc5_bst_md_t                   gr_bst_md;      /*!< Frame buffer burst transfer mode */
    void                          * gr_base;        /*!< Frame buffer base address */
    uint32_t                        gr_ln_off;      /*!< Frame buffer line offset address */
    const vdc5_width_read_fb_t    * width_read_fb;  /*!< Size of the frame buffer to be read */
    vdc5_onoff_t                    adj_sel;        /*!< Folding handling (on/off) */
    vdc5_gr_format_t                gr_format;      /*!< Format of the frame buffer read signal */
    vdc5_gr_ycc_swap_t              gr_ycc_swap;    /*!< Swapping of data read from buffer
                                                         in the YCbCr422 format */
    vdc5_wr_rd_swa_t                gr_rdswa;       /*!< Swap setting in frame buffer reading */
    vdc5_period_rect_t              gr_grc;         /*!< Graphics display area */
} vdc5_read_t;

/******************************* For R_VDC5_ChangeReadProcess       *******************************/
/*! The type of graphics display modes */
typedef enum
{
    VDC5_DISPSEL_IGNORED    = -1,       /*!< Ignored */
    VDC5_DISPSEL_BACK       = 0,        /*!< Background color display */
    VDC5_DISPSEL_LOWER      = 1,        /*!< Lower-layer graphics display */
    VDC5_DISPSEL_CURRENT    = 2,        /*!< Current graphics display */
    VDC5_DISPSEL_BLEND      = 3,        /*!< Blended display of lower-layer graphics and current graphics */
    VDC5_DISPSEL_NUM        = 4         /*!< The number of graphics display modes */
} vdc5_gr_disp_sel_t;
/*! Data read change parameter */
typedef struct
{
    void                          * gr_base;        /*!< Frame buffer base address */
    const vdc5_width_read_fb_t    * width_read_fb;  /*!< Size of the frame buffer to be read */
    const vdc5_period_rect_t      * gr_grc;         /*!< Graphics display area */
    const vdc5_gr_disp_sel_t      * gr_disp_sel;    /*!< Graphics display mode */
} vdc5_read_chg_t;

/******************************* For R_VDC5_StartProcess            *******************************/
/*! Data write/read start parameter */
typedef struct
{
    const vdc5_gr_disp_sel_t      * gr_disp_sel;    /*!< Graphics display mode */
} vdc5_start_t;

/******************************* For R_VDC5_VideoNoiseReduction     *******************************/
/*! TAP select */
typedef enum
{
    VDC5_NR_TAPSEL_1 = 0,           /*!< Adjacent pixel */
    VDC5_NR_TAPSEL_2,               /*!< 2 adjacent pixels */
    VDC5_NR_TAPSEL_3,               /*!< 3 adjacent pixels */
    VDC5_NR_TAPSEL_4,               /*!< 4 adjacent pixels */
    VDC5_NR_TAPSEL_NUM
} vdc5_nr_tap_t;
/*! Noise reduction gain adjustment */
typedef enum
{
    VDC5_NR_GAIN_1_2 = 0,           /*!< 1/2 */
    VDC5_NR_GAIN_1_4,               /*!< 1/4 */
    VDC5_NR_GAIN_1_8,               /*!< 1/8 */
    VDC5_NR_GAIN_1_16,              /*!< 1/16 */
    VDC5_NR_GAIN_NUM
} vdc5_nr_gain_t;
/*! Noise reduction parameter */
typedef struct
{
    vdc5_nr_tap_t   nr1d_tap;       /*!< TAP select */
    uint32_t        nr1d_th;        /*!< Maximum value of coring (absolute value) */
    vdc5_nr_gain_t  nr1d_gain;      /*!< Noise reduction gain adjustment */
} vdc5_nr_param_t;
/*! Noise reduction setup parameter */
typedef struct
{
    vdc5_nr_param_t     y;          /*!< Y/G signal noise reduction parameter */
    vdc5_nr_param_t     cb;         /*!< Cb/B signal noise reduction parameter */
    vdc5_nr_param_t     cr;         /*!< Cr/R signal noise reduction parameter */
} vdc5_noise_reduction_t;

/******************************* For R_VDC5_ImageColorMatrix        *******************************/
/*! Color matrix module */
typedef enum
{
    VDC5_COLORMTX_IMGCNT = 0,           /*!< Input Controller (input video signal) */
    VDC5_COLORMTX_ADJ_0,                /*!< Image quality improver 0 (scaler 0 output) */
    VDC5_COLORMTX_NUM
} vdc5_colormtx_module_t;
/*! Operating mode */
typedef enum
{
    VDC5_COLORMTX_GBR_GBR = 0,          /*!< GBR to GBR */
    VDC5_COLORMTX_GBR_YCBCR,            /*!< GBR to YCbCr */
    VDC5_COLORMTX_YCBCR_GBR,            /*!< YCbCr to GBR */
    VDC5_COLORMTX_YCBCR_YCBCR,          /*!< YCbCr to YCbCr */
    VDC5_COLORMTX_MODE_NUM              /*!< The number of operating modes */
} vdc5_colormtx_mode_t;
/*! Color matrix offset (DC) adjustment */
typedef enum
{
    VDC5_COLORMTX_OFFST_YG = 0,         /*!< YG */
    VDC5_COLORMTX_OFFST_B,              /*!< B */
    VDC5_COLORMTX_OFFST_R,              /*!< R */
    VDC5_COLORMTX_OFFST_NUM             /*!< The number of the color matrix DC offset values */
} vdc5_colormtx_offset_t;
/*! Color matrix signal gain adjustment */
typedef enum
{
    VDC5_COLORMTX_GAIN_GG = 0,          /*!< GG */
    VDC5_COLORMTX_GAIN_GB,              /*!< GB */
    VDC5_COLORMTX_GAIN_GR,              /*!< GR */
    VDC5_COLORMTX_GAIN_BG,              /*!< BG */
    VDC5_COLORMTX_GAIN_BB,              /*!< BB */
    VDC5_COLORMTX_GAIN_BR,              /*!< BR */
    VDC5_COLORMTX_GAIN_RG,              /*!< RG */
    VDC5_COLORMTX_GAIN_RB,              /*!< RB */
    VDC5_COLORMTX_GAIN_RR,              /*!< RR */
    VDC5_COLORMTX_GAIN_NUM              /*!< The number of the color matrix gain values */
} vdc5_colormtx_gain_t;
/*! Color matrix setup parameter */
typedef struct
{
    vdc5_colormtx_module_t  module;                         /*!< Color matrix module */
    vdc5_colormtx_mode_t    mtx_mode;                       /*!< Operating mode */
    uint16_t                offset[VDC5_COLORMTX_OFFST_NUM];/*!< Offset (DC) adjustment of Y/G, B, and R signal */
    uint16_t                gain[VDC5_COLORMTX_GAIN_NUM];   /*!< GG, GB, GR, BG, BB, BR, RG, RB, and RR signal
                                                                 gain adjustment */
} vdc5_color_matrix_t;

/******************************* For R_VDC5_ImageEnhancement        *******************************/
/*! Image quality improver ID */
typedef enum
{
    VDC5_IMG_IMPRV_0 = 0,           /*!< Image quality improver 0 */
    VDC5_IMG_IMPRV_NUM              /*!< The number of image quality improvers */
} vdc5_imgimprv_id_t;

/*! Sharpness band */
typedef enum
{
    VDC5_IMGENH_SHARP_H1 = 0,       /*!< H1: Adjacent pixel used as reference */
    VDC5_IMGENH_SHARP_H2,           /*!< H2: Second adjacent pixel used as reference */
    VDC5_IMGENH_SHARP_H3,           /*!< H3: Third adjacent pixel used as reference */
    VDC5_IMGENH_SHARP_NUM           /*!< The number of horizontal sharpness bands */
} vdc5_img_enh_sh_t;
/*! Sharpness control parameter */
typedef struct
{
    uint8_t     shp_clip_o;         /*!< Sharpness correction value clipping (on the overshoot side) */
    uint8_t     shp_clip_u;         /*!< Sharpness correction value clipping (on the undershoot side) */
    uint8_t     shp_gain_o;         /*!< Sharpness edge amplitude value gain (on the overshoot side) */
    uint8_t     shp_gain_u;         /*!< Sharpness edge amplitude value gain (on the undershoot side) */
    uint8_t     shp_core;           /*!< Active sharpness range */
} vdc5_sharpness_ctrl_t;
/*! Sharpness setup parameter */
typedef struct
{
    vdc5_onoff_t            shp_h2_lpf_sel;                     /*!< LPF selection for folding prevention
                                                                     before H2 edge detection */
    vdc5_sharpness_ctrl_t   hrz_sharp[VDC5_IMGENH_SHARP_NUM];   /*!< Sharpness control parameter (H1, H2, and H3) */
} vdc5_enhance_sharp_t;
/*! LTI band */
typedef enum
{
    VDC5_IMGENH_LTI1 = 0,           /*!< H2: Second adjacent pixel used as reference */
    VDC5_IMGENH_LTI2,               /*!< H4: Fourth adjacent pixel used as reference */
    VDC5_IMGENH_LTI_NUM             /*!< The number of horizontal LTI bands */
} vdc5_img_enh_lti_t;
/*! Median filter reference pixel select */
typedef enum
{
    VDC5_LTI_MDFIL_SEL_ADJ2 = 0,    /*!< Second adjacent pixel selected as reference */
    VDC5_LTI_MDFIL_SEL_ADJ1         /*!< Adjacent pixel selected as reference */
} vdc5_lti_mdfil_sel_t;
/*! LTI control parameter */
typedef struct
{
    uint8_t     lti_inc_zero;       /*!< Median filter LTI correction threshold */
    uint8_t     lti_gain;           /*!< LTI edge amplitude value gain */
    uint8_t     lti_core;           /*!< LTI coring (maximum core value of 255) */
} vdc5_lti_ctrl_t;
/*! Luminance Transient Improvement setup parameter */
typedef struct
{
    vdc5_onoff_t            lti_h2_lpf_sel;             /*!< LPF selection for folding prevention
                                                             before H2 edge detection */
    vdc5_lti_mdfil_sel_t    lti_h4_median_tap_sel;      /*!< Median filter reference pixel select */
    vdc5_lti_ctrl_t         lti[VDC5_IMGENH_LTI_NUM];   /*!< LTI control parameter (H2 and H4) */
} vdc5_enhance_lti_t;

/******************************* For R_VDC5_ImageBlackStretch       *******************************/
/*! Black stretch setup parameter */
typedef struct
{
    uint16_t    bkstr_st;           /*!< Black stretch start point */
    uint16_t    bkstr_d;            /*!< Black stretch depth */
    uint16_t    bkstr_t1;           /*!< Black stretch time constant (T1) */
    uint16_t    bkstr_t2;           /*!< Black stretch time constant (T2) */
} vdc5_black_t;

/******************************* For R_VDC5_AlphaBlending           *******************************/
/*! Alpha signal of the ARGB1555/ARGB5551 format */
typedef struct
{
    uint8_t     gr_a0;                  /*!< Alpha signal when alpha is set to '0' */
    uint8_t     gr_a1;                  /*!< Alpha signal when alpha is set to '1' */
} vdc5_alpha_argb1555_t;
/*! Alpha blending in one-pixel units */
typedef struct
{
    vdc5_onoff_t    gr_acalc_md;        /*!< Premultiplication processing at alpha blending
                                             in one-pixel units (on/off) */
} vdc5_alpha_pixel_t;
/*! Alpha blending setup parameter */
typedef struct
{
    const vdc5_alpha_argb1555_t   * alpha_1bit; /*!< Alpha signal of the ARGB1555/ARGB5551 format */
    const vdc5_alpha_pixel_t      * alpha_pixel;/*!< Premultiplication processing at alpha blending in one-pixel */
} vdc5_alpha_blending_t;

/******************************* For R_VDC5_AlphaBlendingRect       *******************************/
/*! Parameter for alpha blending in a rectangular area */
typedef struct
{
    int16_t         gr_arc_coef;        /*!< Alpha coefficient for alpha blending in a rectangular area
                                             (-255 to 255) */
    uint8_t         gr_arc_rate;        /*!< Frame rate for alpha blending in a rectangular area (gr_arc_rate + 1) */
    uint8_t         gr_arc_def;         /*!< Initial alpha value for alpha blending in a rectangular area */
    vdc5_onoff_t    gr_arc_mul;         /*!< Multiplication processing with current alpha at alpha blending
                                             in a rectangular area (on/off) */
} vdc5_alpha_rect_t;
/*! Selection of lower-layer plane in scaler */
typedef struct
{
    vdc5_onoff_t    gr_vin_scl_und_sel; /*!< Selection of lower-layer plane in scaler
                                             - VDC5_OFF: Selects graphics 0 as lower-layer graphics
                                                         and graphics 1 as current graphics
                                             - VDC5_ON: Selects graphics 1 as lower-layer graphics
                                                        and graphics 0 as current graphics */
} vdc5_scl_und_sel_t;
/*! Setup parameter for alpha blending in a rectangular area */
typedef struct
{
    const vdc5_pd_disp_rect_t * gr_arc;     /*!< Rectangular area subjected to alpha blending */
    const vdc5_alpha_rect_t   * alpha_rect; /*!< Parameter for alpha blending in a rectangular area */
    const vdc5_scl_und_sel_t  * scl_und_sel;/*!< Selection of lower-layer plane in scaler */
} vdc5_alpha_blending_rect_t;

/******************************* For R_VDC5_Chromakey               *******************************/
/*! Chroma-key setup parameter */
typedef struct
{
    uint32_t    ck_color;       /*!< RGB/CLUT signal for RGB/CLUT-index chroma-key processing */
    uint32_t    rep_color;      /*!< Replaced ARGB signal after RGB/CLUT-index chroma-key processing */
    uint8_t     rep_alpha;      /*!< Replaced alpha signal after RGB-index chroma-key processing (in 8 bits) */
} vdc5_chromakey_t;

/******************************* For R_VDC5_CLUT                    *******************************/
/*! CLUT setup parameter */
typedef struct
{
    uint32_t            color_num;  /*!< The number of colors in CLUT */
    const uint32_t    * clut;       /*!< Address of the area storing the CLUT data (in ARGB8888 format) */
} vdc5_clut_t;

/******************************* For R_VDC5_DisplayCalibration      *******************************/
/*! Correction circuit sequence control */
typedef enum
{
    VDC5_CALIBR_ROUTE_BCG = 0,          /*!< Brightness -> contrast -> gamma correction */
    VDC5_CALIBR_ROUTE_GBC               /*!< Gamma correction -> brightness -> contrast */
} vdc5_calibr_route_t;
/*! Brightness (DC) adjustment parameter */
typedef struct
{
    uint16_t    pbrt_g;                 /*!< Brightness (DC) adjustment of G signal */
    uint16_t    pbrt_b;                 /*!< Brightness (DC) adjustment of B signal */
    uint16_t    pbrt_r;                 /*!< Brightness (DC) adjustment of R signal */
} vdc5_calibr_bright_t;
/*! Contrast (gain) adjustment parameter */
typedef struct
{
    uint8_t     cont_g;                 /*!< Contrast (gain) adjustment of G signal */
    uint8_t     cont_b;                 /*!< Contrast (gain) adjustment of B signal */
    uint8_t     cont_r;                 /*!< Contrast (gain) adjustment of R signal */
} vdc5_calibr_contrast_t;
/*! Panel dither operation mode */
typedef enum
{
    VDC5_PDTH_MD_TRU = 0,               /*!< Truncate */
    VDC5_PDTH_MD_RDOF,                  /*!< Round-off */
    VDC5_PDTH_MD_2X2,                   /*!< 2 x 2 pattern dither */
    VDC5_PDTH_MD_RAND,                  /*!< Random pattern dither */
    VDC5_PDTH_MD_NUM
} vdc5_panel_dither_md_t;
/*! Panel dithering parameter */
typedef struct
{
    vdc5_panel_dither_md_t  pdth_sel;   /*!< Panel dither operation mode */
    uint8_t                 pdth_pa;    /*!< Pattern value (A) of 2x2 pattern dither */
    uint8_t                 pdth_pb;    /*!< Pattern value (B) of 2x2 pattern dither */
    uint8_t                 pdth_pc;    /*!< Pattern value (C) of 2x2 pattern dither */
    uint8_t                 pdth_pd;    /*!< Pattern value (D) of 2x2 pattern dither */
} vdc5_calibr_dither_t;
/*! Display calibration parameter */
typedef struct
{
    vdc5_calibr_route_t             route;          /*!< Correction circuit sequence control */
    const vdc5_calibr_bright_t    * bright;         /*!< Brightness (DC) adjustment parameter */
    const vdc5_calibr_contrast_t  * contrast;       /*!< Contrast (gain) adjustment parameter */
    const vdc5_calibr_dither_t    * panel_dither;   /*!< Panel dithering parameter */
} vdc5_disp_calibration_t;

/******************************* For R_VDC5_GammaCorrection         *******************************/
/*! Gamma correction setup parameter */
typedef struct
{
    const uint16_t    * gam_g_gain;     /*!< Gain adjustment of area 0 to 31 of G signal */
    const uint8_t     * gam_g_th;       /*!< Start threshold of area 1 to 31 of G signal */
    const uint16_t    * gam_b_gain;     /*!< Gain adjustment of area 0 to 31 of B signal */
    const uint8_t     * gam_b_th;       /*!< Start threshold of area 1 to 31 of B signal */
    const uint16_t    * gam_r_gain;     /*!< Gain adjustment of area 0 to 31 of R signal */
    const uint8_t     * gam_r_th;       /*!< Start threshold of area 1 to 31 of R signal */
} vdc5_gamma_correction_t;


/******************************************************************************
Exported global variables
******************************************************************************/

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/
vdc5_error_t R_VDC5_Initialize(
    const vdc5_channel_t        ch,
    const vdc5_init_t   * const param,
    void               (* const init_func)(uint32_t),
    const uint32_t              user_num);
vdc5_error_t R_VDC5_Terminate(const vdc5_channel_t ch, void (* const quit_func)(uint32_t), const uint32_t user_num);
vdc5_error_t R_VDC5_VideoInput(const vdc5_channel_t ch, const vdc5_input_t * const param);
vdc5_error_t R_VDC5_SyncControl(const vdc5_channel_t ch, const vdc5_sync_ctrl_t * const param);
vdc5_error_t R_VDC5_DisplayOutput(const vdc5_channel_t ch, const vdc5_output_t * const param);
vdc5_error_t R_VDC5_CallbackISR(const vdc5_channel_t ch, const vdc5_int_t * const param);
vdc5_error_t R_VDC5_WriteDataControl(
    const vdc5_channel_t        ch,
    const vdc5_layer_id_t       layer_id,
    const vdc5_write_t  * const param);
vdc5_error_t R_VDC5_ChangeWriteProcess(
    const vdc5_channel_t            ch,
    const vdc5_layer_id_t           layer_id,
    const vdc5_write_chg_t   * const param);
vdc5_error_t R_VDC5_ReadDataControl(
    const vdc5_channel_t        ch,
    const vdc5_layer_id_t       layer_id,
    const vdc5_read_t   * const param);
vdc5_error_t R_VDC5_ChangeReadProcess(
    const vdc5_channel_t            ch,
    const vdc5_layer_id_t           layer_id,
    const vdc5_read_chg_t   * const param);
vdc5_error_t R_VDC5_StartProcess(
    const vdc5_channel_t        ch,
    const vdc5_layer_id_t       layer_id,
    const vdc5_start_t  * const param);
vdc5_error_t R_VDC5_StopProcess(const vdc5_channel_t ch, const vdc5_layer_id_t layer_id);
vdc5_error_t R_VDC5_ReleaseDataControl(const vdc5_channel_t ch, const vdc5_layer_id_t layer_id);
vdc5_error_t R_VDC5_VideoNoiseReduction(
    const vdc5_channel_t                    ch,
    const vdc5_onoff_t                      nr1d_on,
    const vdc5_noise_reduction_t    * const param);
vdc5_error_t R_VDC5_ImageColorMatrix(const vdc5_channel_t ch, const vdc5_color_matrix_t * const param);
vdc5_error_t R_VDC5_ImageEnhancement(
    const vdc5_channel_t                ch,
    const vdc5_imgimprv_id_t            imgimprv_id,
    const vdc5_onoff_t                  shp_h_on,
    const vdc5_enhance_sharp_t  * const sharp_param,
    const vdc5_onoff_t                  lti_h_on,
    const vdc5_enhance_lti_t    * const lti_param,
    const vdc5_period_rect_t    * const enh_area);
vdc5_error_t R_VDC5_ImageBlackStretch(
    const vdc5_channel_t        ch,
    const vdc5_imgimprv_id_t    imgimprv_id,
    const vdc5_onoff_t          bkstr_on,
    const vdc5_black_t  * const param);
vdc5_error_t R_VDC5_AlphaBlending(
    const vdc5_channel_t                ch,
    const vdc5_layer_id_t               layer_id,
    const vdc5_alpha_blending_t * const param);
vdc5_error_t R_VDC5_AlphaBlendingRect(
    const vdc5_channel_t                        ch,
    const vdc5_layer_id_t                       layer_id,
    const vdc5_onoff_t                          gr_arc_on,
    const vdc5_alpha_blending_rect_t    * const param);
vdc5_error_t R_VDC5_Chromakey(
    const vdc5_channel_t            ch,
    const vdc5_layer_id_t           layer_id,
    const vdc5_onoff_t              gr_ck_on,
    const vdc5_chromakey_t  * const param);
vdc5_error_t R_VDC5_CLUT(const vdc5_channel_t ch, const vdc5_layer_id_t layer_id, const vdc5_clut_t * const param);
vdc5_error_t R_VDC5_DisplayCalibration(const vdc5_channel_t ch, const vdc5_disp_calibration_t * const param);
vdc5_error_t R_VDC5_GammaCorrection(
    const vdc5_channel_t                    ch,
    const vdc5_onoff_t                      gam_on,
    const vdc5_gamma_correction_t   * const param);

void (*R_VDC5_GetISR(const vdc5_channel_t ch, const vdc5_int_type_t type))(const uint32_t int_sense);


#ifdef  __cplusplus
}
#endif  /* __cplusplus */


#endif  /* R_VDC5_H */

