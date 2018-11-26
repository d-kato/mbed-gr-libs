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
 *
 * Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.
 *******************************************************************************/
/**************************************************************************//**
 * File Name :   r_ceu.h
 * @file         r_ceu.h
 * @version      1.00
 * $Rev: 182 $
 * $Date:: 2017-05-11 07:35:51 +0900#$
 * @brief        CEU driver header
 ******************************************************************************/

#ifndef R_CEU_H
#define R_CEU_H

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_typedefs.h"
#include "iodefine.h"
#include "r_ceu_user.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 Typedef definitions
 ******************************************************************************/
/*! @enum ceu_onoff_t
 *  @brief On/off
 */
typedef enum
{
    CEU_OFF = 0, /*!< Off */
    CEU_ON, /*!< On  */
} ceu_onoff_t;

/*! @enum ceu_jpg_t
 *  @brief Select the fetched data type
 */
typedef enum
{
    CEU_IMAGE_CAPTURE_MODE = 0, /*!< Image capture mode          */
    CEU_DATA_SYNC_MODE, /*!< Data synchronous fetch mode */
    CEU_DATA_ENABLE_MODE /*!< Data enable fetch mode      */
} ceu_jpg_t;

/*! @enum ceu_dtif_t
 *  @brief The digital image input pins from which data is to be captured
 */
typedef enum
{
    CEU_8BIT_DATA_PINS = 0, /*!<  8-bit digital image input pins */
    CEU_16BIT_DATA_PINS /*!< 16-bit digital image input pins */
} ceu_dtif_t;

/*! @enum ceu_sig_pol_t
 *  @brief The polarity for detection of the vertical sync signal and the horizontal sync signal.
 */
typedef enum
{
    CEU_HIGH_ACTIVE = 0, /*!< High active */
    CEU_LOW_ACTIVE /*!< Low active  */
} ceu_sig_pol_t;

/*! @enum ceu_dtary_t
 *  @brief The input order of the luminance component and chrominance component
 */
typedef enum
{
    CEU_CB0_Y0_CR0_Y1 = 0, /*!< Image input data is fetched in the order of Cb0, Y0, Cr0, and Y1 */
    CEU_CR0_Y0_CB0_Y1, /*!< Image input data is fetched in the order of Cr0, Y0, Cb0, and Y1 */
    CEU_Y0_CB0_Y1_CR0, /*!< Image input data is fetched in the order of Y0, Cb0, Y1, and Cr0 */
    CEU_Y0_CR0_Y1_CB0 /*!< Image input data is fetched in the order of Y0, Cr0, Y1, and Cb0 */
} ceu_dtary_t;

/*! @enum ceu_int_type_t
 *  @brief Capture Event Interrupt Enable flag
 */
typedef enum
{
    CEU_INT_CPEIE = (0x00000001u), /*!< One Frame capture end interrupt */
    CEU_INT_CFEIE = (0x00000002u), /*!< One Filed capture end interrupt */
    CEU_INT_IGRWIE = (0x00000010u), /*!< illegal register write interrupt */
    CEU_INT_HDIE = (0x00000100u), /*!< HD interrupt */
    CEU_INT_VDIE = (0x00000200u), /*!< VD interrupt */
    CEU_INT_CPBE1IE = (0x00001000u), /*!< bundle write complete interrupt (TOP field) */
    CEU_INT_CPBE2IE = (0x00002000u), /*!< bundle write complete interrupt (TOP field2) */
    CEU_INT_CPBE3IE = (0x00004000u), /*!< bundle write complete interrupt (Bottom field) */
    CEU_INT_CPBE4IE = (0x00008000u), /*!< bundle write complete interrupt (Bottom field2) */
    CEU_INT_CDTOFIE = (0x00010000u), /*!< CRAM data overflow interrupt */
    CEU_INT_IGHSIE = (0x00020000u), /*!< illegal H-Sync interrupt */
    CEU_INT_IGVSIE = (0x00040000u), /*!< illegal V-Sync interrupt */
    CEU_INT_VBPIE = (0x00100000u), /*!< insufficient vertical sync front porch interrupt */
    CEU_INT_FWFIE = (0x00800000u), /*!< FW FAILED interrupt */
    CEU_INT_NHDIE = (0x01000000u), /*!< Non-HD Interrupt */
    CEU_INT_NVDIE = (0x02000000u) /*!< Non-VD Interrupt */
} ceu_int_type_t;

/*! @enum ceu_error_t
 *  @brief Error codes of the CEU driver
 */
typedef enum
{
    CEU_OK = 0, /*!< Normal termination        */
    CEU_ERR_PARAM, /*!< Parameter error           */
    CEU_ERR_NUM /*!< The number of error codes */
} ceu_error_t;

/*! @struct ceu_cap_rect_t
 *  @brief The horizontal/vertical timing of the Capture signals
 */
typedef struct
{
    uint32_t vofst; /*!< The capture start location in terms of the HD count from a vertical sync signal (1-HD units) */
    uint32_t vwdth; /*!< The vertical capture period (4-HD units). */
    uint32_t hofst; /*!< The capture start location in terms of the number of clock cycles from a horizontal sync signal (1-cycle units). */
    uint32_t hwdth; /*!< The horizontal capture period
     - Image capture mode( 8-bit)           8 cycle units
     - Data synchronous fetch mode( 8-bit)  4 cycle units
     - Image capture mode(16-bit)           4 cycle units
     - Data synchronous fetch mode(16-bit)  2 cycle units */
} ceu_cap_rect_t;

/*! @struct ceu_clp_t
 *  @brief Capture Filter Size (Set this param if Image capture mode.)
 */
typedef struct
{
    uint32_t vfclp; /*!< Vertical clipping value   (4-pixel unit) */
    uint32_t hfclp; /*!< Horizontal clipping value (4-pixel unit) */
} ceu_clp_t;

/*! @struct ceu_config_t
 *  @brief CEU config
 */
typedef struct
{
    ceu_jpg_t jpg; /*!< Select the fetched data type */
    ceu_dtif_t dtif; /*!< Sets the digital image input pins from which data is to be captured  */
    ceu_sig_pol_t vdpol; /*!< Sets the polarity for detection of the vertical sync signal          */
    ceu_sig_pol_t hdpol; /*!< Sets the polarity for detection of the horizontal sync signal        */
    ceu_dtary_t dtary; /*!< Sets the input order of the luminance component and chrominance component */
    ceu_cap_rect_t * cap; /*!< The horizontal/vertical timing of the Capture signals */
    ceu_clp_t * clp; /*!< Capture Filter Size (Set this param if Image capture mode.)*/
    ceu_onoff_t cols; /*!< Controls swapping in 32-bit units for data output */
    ceu_onoff_t cows; /*!< Controls swapping in 16-bit units for data output */
    ceu_onoff_t cobs; /*!< Controls swapping in  8-bit units for data output */
} ceu_config_t;

/******************************************************************************
 Macro definitions
 ******************************************************************************/

/******************************************************************************
 Variable Externs
 ******************************************************************************/

/******************************************************************************
 Functions Prototypes
 ******************************************************************************/
void R_CEU_Initialize(void (* const init_func)(uint32_t), const uint32_t user_num);
ceu_error_t R_CEU_Open(const ceu_config_t * const config);
ceu_error_t R_CEU_Execute(const void * cayr, const void * cacr, const uint32_t chdw, ceu_onoff_t auto_capture);
ceu_error_t R_CEU_Terminate(void (* const quit_func)(uint32_t), const uint32_t user_num);
void R_CEU_InterruptEnable(const ceu_int_type_t int_type, void (* const callback)(ceu_int_type_t));
void R_CEU_InterruptDisable(void);
void R_CEU_Isr(const uint32_t int_sense);
ceu_error_t R_CEU_Stop(void);
#if(1) /* mbed */
ceu_error_t R_CEU_Execute_Setting(const void * cayr, const void * cacr, const uint32_t chdw, ceu_onoff_t auto_capture);
ceu_error_t R_CEU_Start(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* R_CEU_H */
