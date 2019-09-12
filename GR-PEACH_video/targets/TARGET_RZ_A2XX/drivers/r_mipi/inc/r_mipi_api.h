/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**************************************************************************
* File Name : r_mipi_api.h
* Version : 1.11
* Description : RZ/A2M MIPI driver API definitions
**************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_typedefs.h"
#include "r_mipi_user.h"

#ifndef R_MIPI_H
#define R_MIPI_H

#ifdef  __cplusplus
extern  "C"
{
#endif  /* __cplusplus */

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/
/*! Mipi Driver Status ( for "Mipi_State" ) */
typedef enum
{
    MIPI_POWOFF,    /*!< Reset Status(Clock Driven) */
    MIPI_STOP,      /*!< Capture Stopped */
    MIPI_CAPTURE,   /*!< Capturing */
} e_mipi_state_t;

/*! Mipi Capture Mode */
typedef enum
{
    MIPI_SINGLE_MODE = 0,   /*!< Single Capture Mode */
    MIPI_CONTINUOUS_MODE,   /*!< Continuous Capture Mode */
} e_mipi_capture_mode_t;

/*! Interlace or Progressive (for "Mipi_Param_t.Mipi_Interlace" ) */
typedef enum
{
    MIPI_PROGRESSIVE = 0,   /*!< Capture Progressive Data */
    MIPI_INTERLACE,         /*!< Capture Interlace Data */
} e_mipi_inter_t;

/*! YUV Range Clip Parameter (Not supported) */
typedef enum
{
    VIN_CLIP_C_16_240 = 0, /*!< Not supported parameter (Clip C, Between 16 and 240) */
    VIN_CLIP_YC_16_240,    /*!< Not supported parameter (Clip YC, Between 16 and 240) */
    VIN_CLIP_C_128_128,    /*!< Not supported parameter (Clip C, Between 16 and 240 ( if Out of Range, Set to 128 )) */
    VIN_CLIP_NONE,         /*!< Not supported parameter (No Clip) */
} e_vin_yuv_clip_t;

/*! LUT Conversion On or OFF (Not supported) */
typedef enum
{
    VIN_LUT_OFF = 0,    /*!< Not supported parameter (No LUT Conversion) */
    VIN_LUT_ON,         /*!< Not supported parameter (LUT Conversion) */
} e_vin_lut_t;

/*! Input Image Format */
typedef enum
{
    VIN_INPUT_YCBCR422_8 = 0,   /*!< YUV(=YCbCr422 8bit) */
    VIN_INPUT_YCBCR422_8I,      /*!< UYVY */
    VIN_INPUT_YCBCR422_10,      /*!< Not supported parameter (YCbCr422 10bit) */
    VIN_INPUT_RGB888,           /*!< Not supported parameter (RGB888) */
    VIN_INPUT_RAW8,             /*!< RAW8 */
} e_vin_inputformat_t;

/*! Output Image Format */
typedef enum
{
    VIN_OUTPUT_YCBCR422_8  = (0x00),    /*!< YUV(=YCbCr422 8bit) */
    VIN_OUTPUT_YCBCR422_10 = (0x01),    /*!< Not supported parameter (YUV(=YCbCr422 10bit) */
    VIN_OUTPUT_Y8_CbCr8    = (0x02),    /*!< YC separate output, Y=8bit, C=8bit */
    VIN_OUTPUT_Y8          = (0x03),    /*!< 8bit Gray scale */
    VIN_OUTPUT_Y10_CbCr10  = (0x04),    /*!< Not supported parameter (YC separate output, Y=10bit, C=10bit) */
    VIN_OUTPUT_Y10         = (0x05),    /*!< Not supported parameter (10bit Gray scale) */
    VIN_OUTPUT_Y10_CbCr8   = (0x06),    /*!< Not supported parameter (YC separate output, Y=10bit, C=8bit) */
    VIN_OUTPUT_ARGB8888    = (0x10),    /*!< Not supported parameter (ARGB8888) */
    VIN_OUTPUT_XRGB8888    = (0x11),    /*!< Not supported parameter (XRGB8888) */
    VIN_OUTPUT_ARGB1555    = (0x12),    /*!< Not supported parameter (ARGB1555) */
    VIN_OUTPUT_RGB565      = (0x13),    /*!< Not supported parameter (RGB565) */
    VIN_OUTPUT_RAW8        = (0x20),    /*!< RAW8 */
} e_vin_outputformat_t;

/*! Output Image Endian */
typedef enum
{
    VIN_OUTPUT_EN_LITTLE = 0,   /*!< Little Endian */
    VIN_OUTPUT_EN_BIG,          /*!< Big Endian */
} e_vin_outputendian_t;

/*! Output Data Dithering On or Off (Not supported) */
typedef enum
{
    VIN_DITHER_CUMULATIVE = 0,  /*!< Not supported parameter (Cumulative Mode) */
    VIN_DITHER_ORDERED,         /*!< Not supported parameter (Ordered Mode) */
} e_vin_dither_t;

/*! Capture Method */
typedef enum
{
    VIN_INTERLACE_ODD = 0,  /*!< Capture Odd field of Interlace data */
    VIN_INTERLACE_EVEN,     /*!< Capture Even field of Interlace data */
    VIN_INTERLACE_BOTH,     /*!< Capture Both field of Interlace data */
    VIN_PROGRESSIVE,        /*!< Capture Progressive data */
} e_vin_interlace_t;

/*! Scaling On or OFF (Not supported) */
typedef enum
{
    VIN_SCALE_OFF = 0,  /*!< Not supported parameter (Scaling is Valid) */
    VIN_SCALE_ON,       /*!< Not supported parameter (Scaling is Invalid) */
} e_vin_scaleon_t;

/*! Scaling Interpolation (Not supported) */
typedef enum
{
    VIN_BILINEAR = 0,   /*!< Not supported parameter (Bilinear interpolation) */
    VIN_NEAREST,        /*!< Not supported parameter (Nearest interpolation) */
    VIN_MULTITAPS,      /*!< Not supported parameter (Multi taps interpolation) */
} e_vin_interpolation_t;

/*! YCbCr422 input data alignment */
typedef enum
{
    VIN_Y_UPPER = 0,  /*!< Upper bit is Y, lower bit is CbCr */
    VIN_CB_UPPER,     /*!< Upper bit is CbCr, lower bit is Y */
} e_vin_input_align_t;

/*! Output data byte swap mode */
typedef enum
{
    VIN_SWAP_OFF = 0,   /*!< Not swap */
    VIN_SWAP_ON,        /*!< Swap */
} e_vin_output_swap_t;

/*! MIPI Interrupt Factor */
typedef enum
{
    MIPI_INT_LESS_THAN_WC    = 0x00000001,  /*!< Error interrupt that is generated when the length of payload data
                                                 of a long packet is less than the WC value. */
    MIPI_INT_AFIFO_OF        = 0x00000002,  /*!< Error Interrupt that is generated by an overflow of
                                                 the asynchronous FIFO, which stores the HS data sent from the PHY.*/
    MIPI_INT_VD_START        = 0x00000004,  /*!< Frame start interrupt.*/
    MIPI_INT_VD_END          = 0x00000008,  /*!< Frame end interrupt).*/
    MIPI_INT_SHP_STB         = 0x00000010,  /*!< Short packet reception interrupt. */
    MIPI_INT_FSFE            = 0x00000020,  /*!< Frame packet reception interrupt. */
    MIPI_INT_LNP_STB         = 0x00000040,  /*!< Long packet reception interrupt.*/
    MIPI_INT_CRC_ERR         = 0x00000080,  /*!< CRC error interrupt.*/
    MIPI_INT_HD_WC_ZERO      = 0x00000100,  /*!< WC (word count) zero interrupt.*/
    MIPI_INT_FRM_SEQ_ERR1    = 0x00000200,  /*!< Frame sequence error 1 interrupt. */
    MIPI_INT_FRM_SEQ_ERR0    = 0x00000400,  /*!< Frame sequence error 0 interrupt. */
    MIPI_INT_ECC_ERR         = 0x00000800,  /*!< ECC error interrupt. */
    MIPI_INT_ECC_CRCT_ERR    = 0x00001000,  /*!< ECC 1-bit correction interrupt. */
    MIPI_INT_ULPS_START      = 0x00002000,  /*!< Ultra-low power data transfer start interrupt. */
    MIPI_INT_ULPS_END        = 0x00004000,  /*!< Ultra-low power data transfer end interrupt. */
    MIPI_INT_ERRSOTHS        = 0x00008000,  /*!< Synchronized SOT (start of transfer) error interrupt
                                                 during HS reception. */
    MIPI_INT_ERRSOTSYNCHS    = 0x00010000,  /*!< Non-synchronizable SOT (start of transfer) error
                                                 interrupt during HS reception. */
    MIPI_INT_ERRESC          = 0x00020000,  /*!< Escape mode entry error interrupt. */
    MIPI_INT_ERRCONTROL      = 0x00040000,  /*!< PHY control error interrupt. */
    VIN_INT_FIELD2           = 0x00100000,  /*!< Field interrupts. */
    VIN_INT_VSYNC_FALL       = 0x00200000,  /*!< VSYNC Falling Edge Detect Interrupt. */
    VIN_INT_VSYNC_RISE       = 0x00400000,  /*!< VSYNC Rising Edge Detect Interrupt. */
    VIN_INT_FIELD            = 0x00800000,  /*!< Field switching Interrupt.*/
    VIN_INT_SCANLINE         = 0x01000000,  /*!< Scan line interrupt. */
    VIN_INT_FRAME            = 0x02000000,  /*!< End of frame interrupt. */
    VIN_INT_FIFO_OF          = 0x04000000   /*!< FIFO overflow interrupt. */
} e_mipi_interrupt_type_t;

/*! MIPI Error Factor */
typedef enum
{
    MIPI_OK             = 0x0000,   /*!< Normal termination. */
    MIPI_STATUS_ERR     = 0x0001,   /*!< An API function is called under unauthorized conditions. */
    MIPI_PARAM_ERR      = 0x0002,   /*!< A parameter is specified under conditions that
                                         are not authorized by the specification. */
} e_mipi_error_t;

/***********************    For R_VDC_VideoInput       ***********************/
/*! mipi phy timing struct */
typedef struct
{
    uint16_t mipi_ths_prepare;  /*!< Setting of the duration of the LP-00 state
                                     (immediately before entry to the HS-0 state) */
    uint16_t mipi_ths_settle;   /*!< Setting of the period in which a transition to the HS state is
                                     ignored after the TTHS_PREPARE period begins */
    uint16_t mipi_tclk_prepare; /*!< Setting of the duration of the LP-00 state
                                     (immediately before entry to the HS-0) */
    uint16_t mipi_tclk_settle;  /*!< Setting of the period in which a transition to the HS state is
                                     ignored after the TCLK_PREPARE period begins */
    uint16_t mipi_tclk_miss;    /*!< Setting of the period in which the absence of the clock is detected,
                                     and the HS-RX is disabled */
    uint16_t mipi_t_init_slave; /*!< Minimum duration of the INIT state */
} st_mipi_phy_timing_t;

/*! mipi parameter struct */
typedef struct
{
    uint8_t  mipi_lanenum;                 /*!< Mipi Lane Num */
    uint8_t  mipi_vc;                      /*!< Mipi Virtual Channel */
    uint8_t  mipi_interlace;               /*!< Interlace or Progressive */
    uint8_t  mipi_laneswap;                /*!< Mipi Lane Swap Setting */
    uint16_t mipi_frametop;                /*!< (for Interlace)Top Field Packet ID */
    uint16_t mipi_outputrate;              /*!< Mipi Data Send Speed(Mbit per sec) */
    st_mipi_phy_timing_t mipi_phy_timing;  /*!< Mipi D-PHY timing settings */
} st_mipi_param_t;

/*! mipi interrupt callback setup parameter */
typedef struct
{
    e_mipi_interrupt_type_t type;                                      /*!< MIPI interrupt type */
    void (* p_mipiCallback) (e_mipi_interrupt_type_t interrupt_flag);  /*!< Interrupt callback function pointer */
    void (* p_vinCallback)  (e_mipi_interrupt_type_t interrupt_flag);  /*!< Interrupt callback function pointer */
    uint32_t line_num;                                                 /*!< Line interrupt set */
} st_mipi_int_t;

/*! Vin parameter Struct */
typedef struct
{
    uint16_t vin_preclip_starty;    /*!< Pre Area Clip Start Line */
    uint16_t vin_preclip_endy;      /*!< Pre Area Clip End Line */
    uint16_t vin_preclip_startx;    /*!< Pre Area Clip Start Column */
    uint16_t vin_preclip_endx;      /*!< Pre Area Clip End Column */
} st_vin_preclip_t;

typedef struct
{
    uint8_t  vin_scaleon;           /*!< Not supported parameter (Scaling On or OFF) */
    uint8_t  vin_interpolation;     /*!< Not supported parameter (Scaling Interpolation) */
    uint16_t vin_scale_h;           /*!< Not supported parameter (Horizontal multiple) */
    uint16_t vin_scale_v;           /*!< Not supported parameter (vertical multiple) */
} st_vin_scale_t;

typedef struct
{
    uint16_t vin_afterclip_size_x;  /*!< Not supported parameter (After Area Clip horizontal size) */
    uint16_t vin_afterclip_size_y;  /*!< Not supported parameter (After Area Clip vertical size) */
} st_vin_afterclip_t;

typedef struct
{
    st_vin_preclip_t   vin_preclip;        /*!< Pre Area Clip Parameter */
    st_vin_scale_t     vin_scale;          /*!< Not supported parameter */
    st_vin_afterclip_t vin_afterclip;      /*!< Not supported parameter */
    uint8_t            vin_yuv_clip;       /*!< Not supported parameter */
    uint8_t            vin_lut;            /*!< Not supported parameter */
    uint8_t            vin_inputformat;    /*!< Input Image Format */
    uint8_t            vin_outputformat;   /*!< Output Image Format */
    uint8_t            vin_outputendian;   /*!< Output Data Endian*/
    uint8_t            vin_dither;         /*!< Not supported parameter */
    uint8_t            vin_interlace;      /*!< (for Interlace input)Capture Method */
    uint8_t            vin_alpha_val8;     /*!< Not supported parameter */
    uint8_t            vin_alpha_val1;     /*!< Not supported parameter */
    uint16_t           vin_stride;         /*!< Stride (byte) */
    uint32_t           vin_ycoffset;       /*!< (for YC separate output)Address Offset Value */
    e_vin_input_align_t  vin_input_align;  /*!< YCbCr422 input data alignment */
    e_vin_output_swap_t  vin_output_swap;  /*!< Output data byte swap mode */
} st_vin_setup_t;

/* Vin Information Struct */
typedef struct
{
    uint16_t vin_nowcaptureline; /*!< Line Num Captured Now */
    uint8_t vin_nowcapturefield; /*!< Field Captured Now */
    uint8_t vin_nowcapturebase;  /*!< Base Captured Now */
} st_vin_info_type_t;

/******************************************************************************
Exported global variables
******************************************************************************/

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/

/**************************************************************************//**
 * @fn          R_MIPI_Initialize
 * @brief       MIPI driver initialize
 *
 *              Description:<br>
 *              MIPI driver initialization processing
 * @param[in]   init_func               : Pointer to a user-defined function
 * @param[in]   user_num                : User defined number
 * @retval      None
 *****************************************************************************/
void R_MIPI_Initialize(void (* const init_func)(uint32_t), const uint32_t user_num);

/**************************************************************************//**
 * @fn          R_MIPI_Open
 * @brief       MIPI driver open
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Parameter check.
 *              - MIPI sw reset.
 *              - Initialization of PHY register.
 * @param[in]   mipi_data               : MIPI setting parameter
 * @retval      Error code
 *****************************************************************************/
e_mipi_error_t R_MIPI_Open(const st_mipi_param_t * const mipi_data);

/**************************************************************************//**
 * @fn          R_MIPI_Close
 * @brief       MIPI driver close
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Check MIPI state.
 *              - MIPI interruput all disable.
 *              - VIN capture stop.
 *              - MIPI sw-reset.
 *              - Calls the user-defined function specified in finalize_func.
 * @param[in]   finalize_func               : Pointer to a user-defined function
 * @param[in]   user_num                : User defined number
 * @retval      Error code
 *****************************************************************************/
e_mipi_error_t R_MIPI_Close(void (* const finalize_func)(uint32_t), const uint32_t user_num);

/**************************************************************************//**
 * @fn          R_MIPI_Setup
 * @brief       MIPI driver VIN setup
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Parameter check.
 *              - Initialization of VIN register.
 *              - VIN scaling setting.
 *              - VIN clipping setting.
 * @param[in]   vin_setup               : VIN setting parameter
 * @retval      Error code
 *****************************************************************************/
e_mipi_error_t R_MIPI_Setup(const st_vin_setup_t * const vin_setup );

/**************************************************************************//**
 * @fn          R_MIPI_SetBufferAdr
 * @brief       MIPI driver VIN set buffer
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Parameter check.
 *              - VIN setting buffer address.
 * @param[in]   buffer_no               : Select buffer base (MB1, MB2, MB3)
 * @param[in]   bufferBase              : Buffer base address
 * @retval      Error code
 *****************************************************************************/
e_mipi_error_t R_MIPI_SetBufferAdr(const uint8_t buffer_no, const uint8_t * const bufferBase);

/**************************************************************************//**
 * @fn          R_MIPI_InterruptEnable
 * @brief       MIPI driver interruput enable (MIPI and VIN).
 *
 *              Description:<br>
 *              Set the interrupt enable register of MIPI and VIN
 * @param[in]   param                   : Interrupt settings
 * @retval      None
 *****************************************************************************/
void R_MIPI_InterruptEnable(const st_mipi_int_t * const param );

/**************************************************************************//**
 * @fn          R_MIPI_InterruptDisable
 * @brief       MIPI driver interruput disable (MIPI and VIN).
 *
 *              Description:<br>
 *              Clear the interrupt enable register of MIPI and VIN
 * @param[in]   None
 * @retval      None
 *****************************************************************************/
void R_MIPI_InterruptDisable(void);

/**************************************************************************//**
 * @fn          R_MIPI_GetInfo
 * @brief       MIPI driver getting registers status.
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Getting registers status (Line Count/Field Status/Frame Buffer Status).
 * @param[out]  infoType       : registers status
 * @retval      Error code
 *****************************************************************************/
e_mipi_error_t R_MIPI_GetInfo(st_vin_info_type_t * infoType);

/**************************************************************************//**
 * @fn          R_MIPI_CaptureStart
 * @brief       MIPI driver capture start.
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Setting capture mode( Single or Continuous ).
 *              - Capture start.
 * @param[in]  captureMode      : Single or Continuous
 * @retval      Error code
 *****************************************************************************/
e_mipi_error_t R_MIPI_CaptureStart(const e_mipi_capture_mode_t captureMode);

/**************************************************************************//**
 * @fn          R_MIPI_CaptureStop
 * @brief       MIPI driver capture stop.
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Capture stop.
 * @param[in]   None
 * @retval      Error code
 *****************************************************************************/
e_mipi_error_t R_MIPI_CaptureStop(void);

/**************************************************************************//**
 * @fn          R_MIPI_InterruptHandler
 * @brief       MIPI driver interrupt handler for MIPI.
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Check MIPI interrupt type.
 *              - Call the callback funtion.
 * @param[in]   int_sense   : sense
 * @retval      None
 *****************************************************************************/
void R_MIPI_InterruptHandler( uint32_t int_sense );

/**************************************************************************//**
 * @fn          R_VIN_InterruptHandler
 * @brief       MIPI driver interrupt handler for VIN.
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Call the callback funtion.
 * @param[in]   int_sense   : sense
 * @retval      None
 *****************************************************************************/
void R_VIN_InterruptHandler( uint32_t int_sense );

#ifdef  __cplusplus
}
#endif  /* __cplusplus */


#endif  /* R_MIPI_H */

