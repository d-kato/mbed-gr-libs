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
 * @file         DisplayBase.h
 * @brief        Display driver wrapper class for GR-PEACH
******************************************************************************/

#ifndef MBED_DISPLAYBASE_H
#define MBED_DISPLAYBASE_H

#include "pinmap.h"

/*! @class DisplayBase
 *  @brief Display driver wrapper class for GR-PEACH
 */
class DisplayBase
{

public:
    /*! @enum video_input_channel_t
        @brief Video input channel select
     */
    typedef enum {
        VIDEO_INPUT_CHANNEL_0 = 0, /*!< Video input channel 0 */
        VIDEO_INPUT_CHANNEL_1      /*!< Video input channel 1 */
    } video_input_channel_t;

    /*! @enum video_adc_vinsel_t
        @brief Input pin control
     */
    typedef enum {
        VIDEO_ADC_VINSEL_VIN1 = 0, /*!< VIN1 input            */
        VIDEO_ADC_VINSEL_VIN2      /*!< VIN2 input            */
    } video_adc_vinsel_t;

    /*! @enum graphics_layer_t
        @brief Graphics layer select
     */
    typedef enum {
        GRAPHICS_LAYER_0 = 0,           /*!< Graphics layer 0         */
        GRAPHICS_LAYER_1,               /*!< Graphics layer 1         */
        GRAPHICS_LAYER_2,               /*!< Graphics layer 2         */
        GRAPHICS_LAYER_3                /*!< Graphics layer 3         */
    } graphics_layer_t;

    /*! @enum graphics_error_t
        @brief Error codes
     */
    typedef enum {
        GRAPHICS_OK = 0,                   /*!< Normal termination       */
        GRAPHICS_VDC5_ERR = -1,            /*!< VDC5 driver error        */
        GRAPHICS_FORMA_ERR = -2,           /*!< Not support format       */
        GRAPHICS_LAYER_ERR = -3,           /*!< Invalid layer ID error   */
        GRAPHICS_CHANNLE_ERR = -4,         /*!< Invalid channel error */
        GRAPHICS_VIDEO_NTSC_SIZE_ERR = -5, /*!< Video write size(vw) error */
        GRAPHICS_VIDEO_PAL_SIZE_ERR = -6,  /*!< Video Write size(vw) error */
        GRAPHICS_PARAM_RANGE_ERR = -7      /*!< Parameter range error */
    } graphics_error_t;

    /*! @enum graphics_format_t
        @brief Graphics layer read format selects
    */
    typedef enum {
        GRAPHICS_FORMAT_YCBCR422 = 0,      /*!< YCbCr422 (2byte / px)  */
        GRAPHICS_FORMAT_RGB565,            /*!< RGB565   (2byte / px)  */
        GRAPHICS_FORMAT_RGB888,            /*!< RGB888   (4byte / px)  */
        GRAPHICS_FORMAT_ARGB8888,          /*!< ARGB8888 (4byte / px)  */
        GRAPHICS_FORMAT_ARGB4444,          /*!< ARGB4444 (2byte / px)  */
        GRAPHICS_FORMAT_CLUT8,             /*!< CLUT8    (1byte / px)  */
        GRAPHICS_FORMAT_CLUT4,             /*!< CLUT4    (0.5byte / px)  */
        GRAPHICS_FORMAT_CLUT1              /*!< CLUT1    (0.125byte / px)  */
    } graphics_format_t;

    /*! @enum video_format_t
        @brief Video writing format selects
     */
    typedef enum {
        VIDEO_FORMAT_YCBCR422 = 0,      /*!< YCbCr422 (2byte / px)    */
        VIDEO_FORMAT_RGB565,            /*!< RGB565   (2byte / px)    */
        VIDEO_FORMAT_RGB888,            /*!< RGB888   (4byte / px)    */
        VIDEO_FORMAT_RAW8               /*!< RAW8     (1byte / px)    */
    } video_format_t;

    /*! @enum wr_rd_swa_t
        @brief Frame buffer swap setting
     */
    typedef enum {
        WR_RD_WRSWA_NON = 0,           /*!< Not swapped: 1-2-3-4-5-6-7-8 */
        WR_RD_WRSWA_8BIT,              /*!< Swapped in 8-bit units: 2-1-4-3-6-5-8-7 */
        WR_RD_WRSWA_16BIT,             /*!< Swapped in 16-bit units: 3-4-1-2-7-8-5-6 */
        WR_RD_WRSWA_16_8BIT,           /*!< Swapped in 16-bit units + 8-bit units: 4-3-2-1-8-7-6-5 */
        WR_RD_WRSWA_32BIT,             /*!< Swapped in 32-bit units: 5-6-7-8-1-2-3-4 */
        WR_RD_WRSWA_32_8BIT,           /*!< Swapped in 32-bit units + 8-bit units: 6-5-8-7-2-1-4-3 */
        WR_RD_WRSWA_32_16BIT,          /*!< Swapped in 32-bit units + 16-bit units: 7-8-5-6-3-4-1-2 */
        WR_RD_WRSWA_32_16_8BIT,        /*!< Swapped in 32-bit units + 16-bit units + 8-bit units: 8-7-6-5-4-3-2-1 */
    } wr_rd_swa_t;

    /*! @enum lcd_tcon_pin_t
        @brief LCD tcon output pin selects
     */
    typedef enum {
        LCD_TCON_PIN_NON = -1,          /*!< Not using output         */
        LCD_TCON_PIN_0,                 /*!< LCD_TCON0                */
        LCD_TCON_PIN_1,                 /*!< LCD_TCON1                */
        LCD_TCON_PIN_2,                 /*!< LCD_TCON2                */
        LCD_TCON_PIN_3,                 /*!< LCD_TCON3                */
        LCD_TCON_PIN_4,                 /*!< LCD_TCON4                */
    } lcd_tcon_pin_t;

    /*! @enum lcd_outformat_t
        @brief LCD output format selects
     */
    typedef enum {
        LCD_OUTFORMAT_RGB888 = 0,       /*!< RGB888 or LVDS           */
        LCD_OUTFORMAT_RGB666,           /*!< RGB666                   */
        LCD_OUTFORMAT_RGB565            /*!< RGB565                   */
    } lcd_outformat_t;

    /*! @enum edge_t
        @brief Edge of a signal
     */
    typedef enum {
        EDGE_RISING    = 0,             /*!< Rising edge              */
        EDGE_FALLING   = 1              /*!< Falling edge             */
    } edge_t;

    /*! @enum lcd_type_t
        @brief LCD type
     */
    typedef enum {
        LCD_TYPE_LVDS = 0,              /*!< LVDS control                */
        LCD_TYPE_PARALLEL_RGB           /*!< RGB parallel signal control */
    } lcd_type_t;

    /*! @enum sig_pol_t
        @brief Polarity of a signal
     */
    typedef enum {
        SIG_POL_NOT_INVERTED = 0,       /*!< Not inverted             */
        SIG_POL_INVERTED                /*!< Inverted                 */
    } sig_pol_t;

    /*! @enum int_type_t
        @brief Interrupt type
     */
    typedef enum {
        INT_TYPE_S0_VI_VSYNC = 0,      /*!< Vsync signal input to scaler 0 */
        INT_TYPE_S0_LO_VSYNC,          /*!< Vsync signal output from scaler 0 */
        INT_TYPE_S0_VSYNCERR,          /*!< Missing Vsync signal for scaler 0 */
        INT_TYPE_VLINE,                /*!< Specified line signal for panel output in graphics 3 */
        INT_TYPE_S0_VFIELD,            /*!< Field end signal for recording function in scaler 0 */
        INT_TYPE_IV1_VBUFERR,          /*!< Frame buffer write overflow signal for scaler 0 */
        INT_TYPE_IV3_VBUFERR,          /*!< Frame buffer read underflow signal for graphics 0 */
        INT_TYPE_IV5_VBUFERR,          /*!< Frame buffer read underflow signal for graphics 2 */
        INT_TYPE_IV6_VBUFERR,          /*!< Frame buffer read underflow signal for graphics 3 */
        INT_TYPE_S0_WLINE,             /*!< Write specification line signal input to scaling-down control block in scaler 0 */
        INT_TYPE_S1_VI_VSYNC,          /*!< Vsync signal input to scaler 1 */
        INT_TYPE_S1_LO_VSYNC,          /*!< Vsync signal output from scaler 1 */
        INT_TYPE_S1_VSYNCERR,          /*!< Missing Vsync signal for scaler 1 */
        INT_TYPE_S1_VFIELD,            /*!< Field end signal for recording function in scaler 1 */
        INT_TYPE_IV2_VBUFERR,          /*!< Frame buffer write overflow signal for scaler 1 */
        INT_TYPE_IV4_VBUFERR,          /*!< Frame buffer read underflow signal for graphics 1 */
        INT_TYPE_S1_WLINE,             /*!< Write specification line signal input to scaling-down control block in scaler 1 */
        INT_TYPE_OIR_VI_VSYNC,         /*!< Vsync signal input to output image generator */
        INT_TYPE_OIR_LO_VSYNC,         /*!< Vsync signal output from output image generator */
        INT_TYPE_OIR_VLINE,            /*!< Specified line signal for panel output in output image generator */
        INT_TYPE_OIR_VFIELD,           /*!< Field end signal for recording function in output image generator */
        INT_TYPE_IV7_VBUFERR,          /*!< Frame buffer write overflow signal for output image generator */
        INT_TYPE_IV8_VBUFERR,          /*!< Frame buffer read underflow signal for graphics (OIR) */
        INT_TYPE_NUM                   /*!< The number of VDC5 interrupt types */
    } int_type_t;

    /*! @enum graphics_video_col_sys_t
        @brief Video color system
     */
    typedef enum {
        COL_SYS_NTSC_358       = 0,        /*!< NTSC-3.58 */
        COL_SYS_NTSC_443       = 1,        /*!< NTSC-4.43 */
        COL_SYS_PAL_443        = 2,        /*!< PAL-4.43 */
        COL_SYS_PAL_M          = 3,        /*!< PAL-M */
        COL_SYS_PAL_N          = 4,        /*!< PAL-N */
        COL_SYS_SECAM          = 5,        /*!< SECAM */
        COL_SYS_NTSC_443_60    = 6,        /*!< NTSC-4.43 (60Hz) */
        COL_SYS_PAL_60         = 7,        /*!< PAL-60 */
    } graphics_video_col_sys_t;

    /*! @enum video_input_sel_t
        @brief External Input select
     */
    typedef enum {
        INPUT_SEL_VDEC     = 0,            /*!< Video decoder output signals */
        INPUT_SEL_EXT      = 1,            /*!< Signals supplied via the external input pins */
        INPUT_SEL_CEU      = 2,            /*!< Signals supplied via the CEU input pins */
        INPUT_SEL_MIPI     = 3             /*!< Signals supplied via the MIPI input pins */
    } video_input_sel_t;

    /*! @enum video_extin_format_t
       @brief External input format select
    */
    typedef enum {
        VIDEO_EXTIN_FORMAT_RGB888 = 0,   /*!< RGB888 Not support */
        VIDEO_EXTIN_FORMAT_RGB666,       /*!< RGB666 */
        VIDEO_EXTIN_FORMAT_RGB565,       /*!< RGB565 */
        VIDEO_EXTIN_FORMAT_BT656,        /*!< BT6556 */
        VIDEO_EXTIN_FORMAT_BT601,        /*!< BT6501 */
        VIDEO_EXTIN_FORMAT_YCBCR422,     /*!< YCbCr422 */
        VIDEO_EXTIN_FORMAT_YCBCR444,     /*!< YCbCr444 Not support */
    } video_extin_format_t;

    /*! @enum onoff_t
        @brief On/off
     */
    typedef enum {
        OFF    = 0,            /*!< Off */
        ON     = 1             /*!< On */
    } onoff_t;

    /*! @enum extin_input_line_t
        @brief Number of lines for BT.656 external input
     */
    typedef enum {
        EXTIN_LINE_525     = 0,    /*!< 525 lines */
        EXTIN_LINE_625     = 1     /*!< 625 lines */
    } extin_input_line_t;

    /*! @enum extin_h_pos_t
        @brief Y/Cb/Y/Cr data string start timing
     */
    typedef enum {
        EXTIN_H_POS_CBYCRY = 0,    /*!< Cb/Y/Cr/Y (BT656/601), Cb/Cr (YCbCr422) */
        EXTIN_H_POS_YCRYCB,        /*!< Y/Cr/Y/Cb (BT656/601), setting prohibited (YCbCr422) */
        EXTIN_H_POS_CRYCBY,        /*!< Cr/Y/Cb/Y (BT656/601), setting prohibited (YCbCr422) */
        EXTIN_H_POS_YCBYCR,        /*!< Y/Cb/Y/Cr (BT656/601), Cr/Cb (YCbCr422) */
        EXTIN_H_POS_NUM
    } extin_h_pos_t;

    /*! @struct rect_t
        @brief The relative position within the graphics display area
     */
    typedef struct {
        unsigned short vs;          /*!< Vertical start pos       */
        unsigned short vw;          /*!< Vertical width (height)  */
        unsigned short hs;          /*!< Horizontal start pos     */
        unsigned short hw;          /*!< Horizontal width         */
    } rect_t;

    /*! @struct clut_t
        @brief CLUT setup parameter
     */
    typedef struct {
        uint32_t            color_num;  /*!< The number of colors in CLUT */
        const uint32_t    * clut;       /*!< Address of the area storing the CLUT data (in ARGB8888 format) */
    } clut_t;

    /*! @struct lcd_config_t
        @brief LCD configuration
     */
    typedef struct {
        lcd_type_t      lcd_type;               /*!< LVDS or Pararel RGB                      */
        double          intputClock;            /*!< P1  clk [MHz] ex. 66.67f                 */
        double          outputClock;            /*!< LCD clk [MHz] ex. 33.33f                 */

        lcd_outformat_t lcd_outformat;          /*!< Output format select */
        edge_t          lcd_edge;               /*!< Output phase control of LCD_DATA23 to LCD_DATA0 pin */

        unsigned short  h_toatal_period;        /*!< Free-running Hsync period                */
        unsigned short  v_toatal_period;        /*!< Free-running Vsync period                */
        unsigned short  h_disp_widht;           /*!< LCD display area size, horizontal width  */
        unsigned short  v_disp_widht;           /*!< LCD display area size, vertical width    */
        unsigned short  h_back_porch;           /*!< LCD display horizontal back porch period */
        unsigned short  v_back_porch;           /*!< LCD display vertical back porch period   */

        lcd_tcon_pin_t  h_sync_port;            /*!< TCONn or Not use(-1)                     */
        sig_pol_t       h_sync_port_polarity;   /*!< Polarity inversion control of signal     */
        unsigned short  h_sync_width;           /*!< Hsync width                              */

        lcd_tcon_pin_t  v_sync_port;            /*!< TCONn or Not use(-1)                     */
        sig_pol_t       v_sync_port_polarity;   /*!< Polarity inversion control of signal     */
        unsigned short  v_sync_width;           /*!< Vsync width  */

        lcd_tcon_pin_t  de_port;                /*!< TCONn or Not use(-1)                     */
        sig_pol_t       de_port_polarity;       /*!< Polarity inversion control of signal     */
    } lcd_config_t;

    /*! @struct video_ext_in_config_t
        @brief Digital Video Input configuration
     */
    typedef struct {
        video_extin_format_t     inp_format;    /*!< External Input Format Select                                */
        edge_t                   inp_pxd_edge;  /*!< Clock Edge Select for Capturing External Input Video Image  */
        edge_t                   inp_vs_edge;   /*!< Clock Edge Select for Capturing External Input Vsync Signal */
        edge_t                   inp_hs_edge;   /*!< Clock Edge Select for Capturing External Input Hsync Signal */
        onoff_t                  inp_endian_on; /*!< External Input B/R Signal Swap On/Off Control               */
        onoff_t                  inp_swap_on;   /*!< External Input Bit Endian Change On/Off Control             */
        sig_pol_t                inp_vs_inv;    /*!< External Input Vsync Signal DV_VSYNC Inversion Control      */
        sig_pol_t                inp_hs_inv;    /*!< External Input Hsync Signal DV_HSYNC Inversion Control      */
        extin_input_line_t       inp_f525_625;  /*!< Number of lines for BT.656 external input */
        extin_h_pos_t            inp_h_pos;     /*!< Y/Cb/Y/Cr data string start timing to Hsync reference */
        unsigned short           cap_vs_pos;    /*!< Capture start position from Vsync */
        unsigned short           cap_hs_pos;    /*!< Capture start position form Hsync */
        unsigned short           cap_width;     /*!< Capture width  */
        unsigned short           cap_height;    /*!< Capture height should be a multiple of 4.*/
    } video_ext_in_config_t;

    /* mipi phy timing struct */
    typedef struct {
        uint16_t mipi_ths_prepare;  /*!< Setting of the duration of the LP-00 state (immediately before entry to the HS-0 state) */
        uint16_t mipi_ths_settle;   /*!< Setting of the period in which a transition to the HS state is ignored after the TTHS_PREPARE period begins */
        uint16_t mipi_tclk_prepare; /*!< Setting of the duration of the LP-00 state (immediately before entry to the HS-0) */
        uint16_t mipi_tclk_settle;  /*!< Setting of the period in which a transition to the HS state is ignored after the TCLK_PREPARE period begins */
        uint16_t mipi_tclk_miss;    /*!< Setting of the period in which the absence of the clock is detected, and the HS-RX is disabled */
        uint16_t mipi_t_init_slave; /*!< Minimum duration of the INIT state */
    } video_mipi_phy_timing_t;

    /* mipi parameter struct */
    typedef struct {
        uint8_t  mipi_lanenum;                 /*!< Mipi Lane Num */
        uint8_t  mipi_vc;                      /*!< Mipi Virtual Channel */
        uint8_t  mipi_interlace;               /*!< Interlace or Progressive */
        uint8_t  mipi_laneswap;                /*!< Mipi Lane Swap Setting */
        uint16_t mipi_frametop;                /*!< (for Interlace)Top Field Packet ID */
        uint16_t mipi_outputrate;              /*!< Mipi Data Send Speed(Mbit per sec) */
        video_mipi_phy_timing_t mipi_phy_timing;  /*!< Mipi D-PHY timing settings */
    } video_mipi_param_t;

    /*! Vin parameter Struct */
    typedef struct {
        uint16_t vin_preclip_starty;    /*!< Pre Area Clip Start Line */
        uint16_t vin_preclip_endy;      /*!< Pre Area Clip End Line */
        uint16_t vin_preclip_startx;    /*!< Pre Area Clip Start Column */
        uint16_t vin_preclip_endx;      /*!< Pre Area Clip End Column */
    } video_vin_preclip_t;

    typedef struct {
        uint8_t  vin_scaleon;           /*!< Scaling On or OFF */
        uint8_t  vin_interpolation;     /*!< Scaling Interpolation */
        uint16_t vin_scale_h;           /*!< Horizontal multiple */
        uint16_t vin_scale_v;           /*!< vertical multiple */
    } video_vin_scale_t;

    typedef struct {
        uint16_t vin_afterclip_size_x;  /*!< After Area Clip horizontal size */
        uint16_t vin_afterclip_size_y;  /*!< After Area Clip vertical size */
    } video_vin_afterclip_t;

    /*! YCbCr422 input data alignment */
    typedef enum
    {
        VIN_Y_UPPER = 0,  /*!< Upper bit is Y, lower bit is CbCr */
        VIN_CB_UPPER,     /*!< Upper bit is CbCr, lower bit is Y */
    } video_vin_input_align_t;

    /*! Output data byte swap mode */
    typedef enum
    {
        VIN_SWAP_OFF = 0,   /*!< Not swap */
        VIN_SWAP_ON,        /*!< Swap */
    } video_vin_output_swap_t;

    typedef struct {
        video_vin_preclip_t   vin_preclip;     /*!< Pre Area Clip Parameter */
        video_vin_scale_t     vin_scale;       /*!< Scale Parameter */
        video_vin_afterclip_t vin_afterclip;   /*!< After Area Clip Parameter */
        uint8_t         vin_yuv_clip;       /*!< YUV Range Clip Parameter */
        uint8_t         vin_lut;            /*!< LUT Conversion On or OFF */
        uint8_t         vin_inputformat;    /*!< Input Image Format */
        uint8_t         vin_outputformat;   /*!< Output Image Format */
        uint8_t         vin_outputendian;   /*!< Output Data Endian*/
        uint8_t         vin_dither;         /*!< (for RGB565 or ARGB1555)Output Data Dithering On or Off */
        uint8_t         vin_interlace;      /*!< (for Interlace input)Capture Method */
        uint8_t         vin_alpha_val8;     /*!< (for ARGB8888)Alpha Value */
        uint8_t         vin_alpha_val1;     /*!< (for ARGB1555)Alpha Value */
        uint16_t        vin_stride;         /*!< Stride (byte) */
        uint32_t        vin_ycoffset;       /*!< (for YC separate output)Address Offset Value */
        video_vin_input_align_t  vin_input_align;  /*!< YCbCr422 input data alignment */
        video_vin_output_swap_t  vin_output_swap;  /*!< Output data byte swap mode */
    } video_vin_setup_t;

    /** Constructor method of display base object
     */
    DisplayBase( void );

    /** Graphics initialization processing<br>
     *  If not using display, set NULL in parameter.
     *  @param[in]    lcd_config          : LCD configuration
     *  @retval       Error code
     */
    graphics_error_t Graphics_init( const lcd_config_t * lcd_config );

    /** Graphics Video initialization processing<br>
     *  If setting INPUT_SEL_VDEC in video_input_sel parameter, set NULL in video_ext_in_config parameter.
     *  @param[in]    video_input_sel     : Input select
     *  @param[in]    video_ext_in_config : Video configuration
     *  @retval       error code
     */
    graphics_error_t Graphics_Video_init( video_input_sel_t video_input_sel, video_ext_in_config_t * video_ext_in_config );

    /** Graphics Video initialization processing<br>
     *  @param[in]    video_input_sel     : Input select
     *  @param[in]    video_mipi_config   : MIPI configuration
     *  @param[in]    video_vin_setup     : MIPI configuration
     *  @retval       error code
     */
    graphics_error_t Graphics_Video_init( video_input_sel_t video_input_sel, video_mipi_param_t * video_mipi_config, video_vin_setup_t * video_vin_setup );

    /** LCD output port initialization processing
     *  @param[in]    pin                 : Pin assign for LCD output
     *  @param[in]    pin_count           : Total number of pin assign
     *  @retval       Error code
     */
    graphics_error_t Graphics_Lcd_Port_Init( PinName *pin, unsigned int pin_count );

    /** LVDS output port initialization processing
     *  @param[in]    pin                 : Pin assign for LVDS output
     *  @param[in]    pin_count           : Total number of pin assign
     *  @retval       Error code
     */
    graphics_error_t Graphics_Lvds_Port_Init( PinName *pin, unsigned int pin_count );

    /** Digital video input port initialization processing
     *  @param[in]    pin                 : Pin assign for digital video input port
     *  @param[in]    pin_count           : Total number of pin assign
     *  @retval       Error code
     */
    graphics_error_t Graphics_Dvinput_Port_Init( PinName *pin, unsigned int pin_count );

    /** CEU input port initialization processing
     *  @param[in]    pin                 : Pin assign for CEU input port
     *  @param[in]    pin_count           : Total number of pin assign
     *  @retval       Error code
     */
    graphics_error_t Graphics_Ceu_Port_Init( PinName *pin, unsigned int pin_count );

    /**  Interrupt callback setup
     *  This function performs the following processing:
     *  - Enables the interrupt when the pointer to the corresponding interrupt callback function is specified.<br>
     *  - Registers the specified interrupt callback function.<br>
     *  - Disables the interrupt when the pointer to the corresponding interrupt callback function is not specified.<br>
     *  @param[in]    Graphics_Irq_Handler_Set : VDC5 interrupt type
     *  @param[in]    num                      : Interrupt line number
     *  @param[in]    callback                 : Interrupt callback function pointer
     *  @retval       Error code
     */
    graphics_error_t Graphics_Irq_Handler_Set( int_type_t Graphics_Irq_Handler_Set, unsigned short num, void (* callback)(int_type_t)  );

    /** Start the graphics surface read process
     *  @param[in]    layer_id            : Graphics layer ID <br />
     *      - GRAPHICS_LAYER_0 : Layer 0
     *      - GRAPHICS_LAYER_1 : Layer 1
     *      - GRAPHICS_LAYER_2 : Layer 2
     *      - GRAPHICS_LAYER_3 : Layer 3
     *  @retval       Error code
     */
    graphics_error_t Graphics_Start( graphics_layer_t layer_id );

    /** Stop the graphics surface read process
     *  @param[in]    layer_id            : Graphics layer ID <br />
     *      - GRAPHICS_LAYER_0 : Layer 0
     *      - GRAPHICS_LAYER_1 : Layer 1
     *      - GRAPHICS_LAYER_2 : Layer 2
     *      - GRAPHICS_LAYER_3 : Layer 3
     *  @retval       Error code
     */
    graphics_error_t Graphics_Stop( graphics_layer_t layer_id );

    /** Start the video surface write process
     *  @param[in]    video_input_channel : Video input channel <br />
     *      - VIDEO_INPUT_CHANNEL_0 : Video channel 0
     *      - VIDEO_INPUT_CHANNEL_1 : Video channel 1
     *  @retval       Error code
     */
    graphics_error_t Video_Start ( video_input_channel_t video_input_channel );

    /** Stop the video surface write process
     *  @param[in]    video_input_channel : Video input channel <br />
     *      - VIDEO_INPUT_CHANNEL_0 : Video channel 0
     *      - VIDEO_INPUT_CHANNEL_1 : Video channel 1
     *  @retval       Error code
     */
    graphics_error_t Video_Stop ( video_input_channel_t video_input_channel );

    /** Graphics surface read process setting
     *  @param[in]    layer_id            : Graphics layer ID <br />
     *      - GRAPHICS_LAYER_0 : Layer 0
     *      - GRAPHICS_LAYER_1 : Layer 1
     *      - GRAPHICS_LAYER_2 : Layer 2
     *      - GRAPHICS_LAYER_3 : Layer 3
     *  @param[in]    framebuff           : Base address of the frame buffer(Not set NULL)
     *  @param[in]    fb_stride           : Line offset address of the frame buffer[byte]  <br />
     *      Frame buffer stride should be set to a multiple of 32 or 128
     *      in accordance with the frame buffer burst transfer mode.
     *  @param[in]    gr_format           : Format of the frame buffer read signal <br />
     *      - GRAPHICS_FORMAT_YCBCR422 : YCBCR422 (2byte/px)
     *      - GRAPHICS_FORMAT_RGB565   : RGB565 (2byte/px)
     *      - GRAPHICS_FORMAT_RGB888   : RGB888 (4byte/px)
     *      - GRAPHICS_FORMAT_ARGB8888 : ARGB8888 (4byte/px)
     *      - GRAPHICS_FORMAT_CLUT8    : CLUT8 (1byte/px)
     *      - GRAPHICS_FORMAT_CLUT4    : CLUT4 (0.5byte/px)
     *      - GRAPHICS_FORMAT_CLUT1    : CLUT1 (0,12byte/px)
     *  @param[in]    wr_rd_swa : frame buffer swap setting <br />
     *      -    WR_RD_WRSWA_NON        : Not swapped: 1-2-3-4-5-6-7-8
     *      -    WR_RD_WRSWA_8BIT       : Swapped in 8-bit units: 2-1-4-3-6-5-8-7
     *      -    WR_RD_WRSWA_16BIT      : Swapped in 16-bit units: 3-4-1-2-7-8-5-6
     *      -    WR_RD_WRSWA_16_8BIT    : Swapped in 16-bit units + 8-bit units: 4-3-2-1-8-7-6-5
     *      -    WR_RD_WRSWA_32BIT      : Swapped in 32-bit units: 5-6-7-8-1-2-3-4
     *      -    WR_RD_WRSWA_32_8BIT    : Swapped in 32-bit units + 8-bit units: 6-5-8-7-2-1-4-3
     *      -    WR_RD_WRSWA_32_16BIT   : Swapped in 32-bit units + 16-bit units: 7-8-5-6-3-4-1-2
     *      -    WR_RD_WRSWA_32_16_8BIT : Swapped in 32-bit units + 16-bit units + 8-bit units: 8-7-6-5-4-3-2-1
     *  @param[in]    gr_rect             : Graphics display area
     *  @param[in]    gr_clut             : CLUT setup parameter
     *  @retval       Error code
     */
    graphics_error_t Graphics_Read_Setting (
        graphics_layer_t    layer_id,
        void              * framebuff,
        unsigned int        fb_stride,
        graphics_format_t   gr_format,
        wr_rd_swa_t         wr_rd_swa,
        rect_t            * gr_rect,
        clut_t            * gr_clut = 0 );

    /** Graphics surface read buffer change process
     *  @param[in]    layer_id            : Graphics layer ID <br />
     *      - GRAPHICS_LAYER_0 : Layer 0
     *      - GRAPHICS_LAYER_1 : Layer 1
     *      - GRAPHICS_LAYER_2 : Layer 2
     *      - GRAPHICS_LAYER_3 : Layer 3
     *  @param[in]    framebuff           : Base address of the frame buffer(Not set NULL)
     *  @retval       Error code
     */
    graphics_error_t Graphics_Read_Change (
        graphics_layer_t    layer_id,
        void             *  framebuff);

    /** Video surface write process setting
     *  @param[in]    video_input_channel : Video input channel <br />
     *                If using digital input, this parameter is not referenced. <br />
     *      - VIDEO_INPUT_CHANNEL_0 : Video channel 0
     *      - VIDEO_INPUT_CHANNEL_1 : Video channel 1
     *  @param[in]    col_sys             : Analog video signal color system <br />
     *                If using digital input, this parameter is not referenced. <br />
     *      - COL_SYS_NTSC_358    : NTSC-3.58
     *      - COL_SYS_NTSC_443    : NTSC-4.43
     *      - COL_SYS_PAL_443     : PAL-4.43
     *      - COL_SYS_PAL_M       : PAL-M
     *      - COL_SYS_PAL_N       : PAL-N
     *      - COL_SYS_SECAM       : SECAM
     *      - COL_SYS_NTSC_443_60 : NTSC-4.43 (60Hz)
     *      - COL_SYS_PAL_60      : PAL-60
     *  @param[in]    framebuff           : Base address of the frame buffer(Not set NULL)
     *  @param[in]    fb_stride           : Line offset address of the frame buffer[byte]  <br />
     *      Frame buffer stride should be set to a multiple of 32 or 128
     *      in accordance with the frame buffer burst transfer mode.
     *  @param[in]    video_format        : Frame buffer video-signal writing format <br />
     *      - VIDEO_FORMAT_YCBCR422 : YCBCR422 (2byte/px)
     *      - VIDEO_FORMAT_RGB565   : RGB565 (2byte/px)
     *      - VIDEO_FORMAT_RGB888   : RGB888 (4byte/px)
     *  @param[in]    wr_rd_swa : frame buffer swap setting  <br />
     *      -    WR_RD_WRSWA_NON        : Not swapped: 1-2-3-4-5-6-7-8
     *      -    WR_RD_WRSWA_8BIT       : Swapped in 8-bit units: 2-1-4-3-6-5-8-7
     *      -    WR_RD_WRSWA_16BIT      : Swapped in 16-bit units: 3-4-1-2-7-8-5-6
     *      -    WR_RD_WRSWA_16_8BIT    : Swapped in 16-bit units + 8-bit units: 4-3-2-1-8-7-6-5
     *      -    WR_RD_WRSWA_32BIT      : Swapped in 32-bit units: 5-6-7-8-1-2-3-4
     *      -    WR_RD_WRSWA_32_8BIT    : Swapped in 32-bit units + 8-bit units: 6-5-8-7-2-1-4-3
     *      -    WR_RD_WRSWA_32_16BIT   : Swapped in 32-bit units + 16-bit units: 7-8-5-6-3-4-1-2
     *      -    WR_RD_WRSWA_32_16_8BIT : Swapped in 32-bit units + 16-bit units + 8-bit units: 8-7-6-5-4-3-2-1
     *  @param[in]    video_write_buff_vw : Output height[px] <br />
     *      - NTSC format : Max height is 480[px]
     *      - PAL format  : Max height is 520[px]
     *  @param[in]    video_write_buff_hw : Output width[px] <br />
     *      - Max width : 800[px]
     *  @param[in]    video_adc_vinsel : Input pin control <br />
     *      - VIDEO_ADC_VINSEL_VIN1 : VIN1 input
     *      - VIDEO_ADC_VINSEL_VIN2 : VIN2 input
     *  @retval       Error code
     */
    graphics_error_t Video_Write_Setting (
        video_input_channel_t       video_input_channel,
        graphics_video_col_sys_t    col_sys,
        void                      * framebuff,
        unsigned int                fb_stride,
        video_format_t              video_format,
        wr_rd_swa_t                 wr_rd_swa,
        unsigned short              video_write_buff_vw,
        unsigned short              video_write_buff_hw,
        video_adc_vinsel_t          video_adc_vinsel = VIDEO_ADC_VINSEL_VIN1 );

    /** Video surface write buffer change process
     *  @param[in]    video_input_channel : Video input channel <br />
     *      - VIDEO_INPUT_CHANNEL_0 : Video channel 0
     *      - VIDEO_INPUT_CHANNEL_1 : Video channel 1
     *  @param[in]    framebuff           : Base address of the frame buffer(Not set NULL)
     *  @param[in]    fb_stride           : Line offset address of the frame buffer <br />
     *      Frame buffer stride should be set to a multiple of 32 or 128
     *      in accordance with the frame buffer burst transfer mode.
     *  @retval       Error code
     */
    graphics_error_t Video_Write_Change (
        video_input_channel_t    video_input_channel,
        void                   * framebuff,
        uint32_t                 fb_stride );

protected:
    lcd_config_t          _lcd_config;
    video_input_sel_t     _video_input_sel;
    video_ext_in_config_t _video_ext_in_config;
#if defined(TARGET_RZ_A2XX)
    video_mipi_param_t    _video_mipi_config;
    video_vin_setup_t     _video_vin_setup;
#endif
};


#endif /* MBED_DISPLAYBASE_H */
