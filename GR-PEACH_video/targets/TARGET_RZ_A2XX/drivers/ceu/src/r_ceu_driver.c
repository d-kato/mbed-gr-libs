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
 * Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.
 *******************************************************************************/
/**************************************************************************//**
 * File Name :   r_ceu_driver.c
 * @file         r_ceu_driver.c
 * @version      1.00
 * $Rev: 200 $
 * $Date:: 2017-05-17 15:19:00 +0900#$
 * @brief        CEU driver API functions
 ******************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_typedefs.h"
#include "r_ceu.h"

/******************************************************************************
 Typedef definitions
 ******************************************************************************/

/******************************************************************************
 Macro definitions
 ******************************************************************************/
#define CEU_SW_RESET        (0x00010000u)
#define CAPSR_CPKIL_BIT     (0x00010000u)
#define CSTSR_CPTON_BIT     (0x00000001u)
#define CAPSR_CE_BIT        (0x00000001u)

/* Driver is not supported register vale */
#define CAPCR_FDRP_VALUE    (0u)      /* The frame drop count is 0               */
#define CAPCR_MTCM_VALUE    (0u)      /* Transferred to the bus in 32-byte units */
#define CAPCR_CTNCP_VALUE   (0u)      /* One-frame capture                       */
#define CAIFR_IFS_VALUE     (0u)      /* Progressive */
#define CAIFR_CIM_VALUE     (0u)      /* Capture of frame image (1 VD) or both-field image (2 VD) */
#define CAIFR_FC_VALUE      (0u)      /* Capture starts from the VD input immediately after
                                         the CEU activation regardless of it being a top or bottom field */
#define CRCNTR_RVS_VALUE    (0u)      /* Switches the register plane every 2 VD */
#define CRCNTR_RS_VALUE     (0u)      /* Uses plane A of the register */
#define CRCNTR_RC_VALUE     (0u)      /* Uses the specified register plane in synchronization with VD */
#define CFLCR_VMANT_VALUE   (0u)      /* Mantissa Part of Vertical Scale-Down Factor */
#define CFLCR_VFRAC_VALUE   (0u)      /* Fraction Part of Vertical Scale-Down Factor */
#define CFLCR_HMANT_VALUE   (0u)      /* Mantissa Part of Horizontal Scale-Down Factor */
#define CFLCR_HFRAC_VALUE   (0u)      /* Fraction Part of Horizontal Scale-Down Factor */
#define CMCYR_VCYL_VALUE    (0u)      /* Vertical HD Count for illegal VD is 0      */
#define CMCYR_HCYL_VALUE    (0u)      /* Horizontal Cycle Count for illegal HD is 0 */
#define CLFCR_LPF_VALUE     (0u)      /* Low-pass filter used (only in the horizontal direction) */
#define CAMCR_FLDPOL_VALUE  (0u)      /* When the FLD signal is high-active,
                                         the field is detected as the top field
                                         and when low-active, the field is detected as the bottom field.  */
#define CDOCR_CBE_VALUE     (0u)      /* Normal write */
#define CDOCR_CDS_VALUE     (1u)      /* Outputs data in the YCbCr422 format to the memory without conversion */
#define CFWCR_FWV_VALUE     (0u)      /* The upper limit of a write address */
#define CFWCR_FWE_VALUE     (0u)      /* Firewall is not activated */


/* Driver check value */
#define VOFST_MAX_0FFF      (0x00000FFFu)  /* VOFST MAX check   */
#define HOFST_MAX_1FFF      (0x00001FFFu)  /* HOFST MAX check   */
#define VWDTH_MAX_1920      (1920u)        /* VWDTH MAX check   */
/* HWDTH MAX check */
#define HWDTH_MAX_5120      (5120u)        /* CEU_IMAGE_CAPTURE_MODE & 8bit  */
#define HWDTH_MAX_2560      (2560u)        /* CEU_DATA_SYNC_MODE     & 8bit  */
#define HWDTH_MAX_1280      (1280u)        /* CEU_DATA_SYNC_MODE     & 16bit */
#define CHDW_MAX_1FFF       (0x00001FFFu)  /* CHDW MAX check   */
#define CHDW_MAX_8188       (8188u)        /* CHDW MAX check   */

#define CEU_SHIFT_VALUE_1   (1u)
#define CEU_SHIFT_VALUE_2   (2u)
#define CEU_SHIFT_VALUE_4   (4u)
#define CEU_SHIFT_VALUE_8   (8u)
#define CEU_SHIFT_VALUE_12  (12u)
#define CEU_SHIFT_VALUE_16  (16u)
#define CEU_SHIFT_VALUE_20  (20u)
#define CEU_SHIFT_VALUE_24  (24u)
#define CEU_SHIFT_VALUE_28  (28u)

#define CETCR_CLARE_VALUE   (0x00000000u)
#define CEIER_CLARE_VALUE   (0x00000000u)

/******************************************************************************
 Imported global variables and functions (from other files)
 ******************************************************************************/

/******************************************************************************
 Exported global variables and functions (to be accessed by other files)
 ******************************************************************************/

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
static ceu_error_t ceu_open_check_prm(const ceu_config_t * const config);
static ceu_error_t ceu_execute_check_prm(const void * cayr, const void * cacr, const uint32_t chdw);
static int32_t ceu_align_check(const uint32_t value, uint32_t align);

static void (*ceu_int_callback)(ceu_int_type_t interrupt_flag);

static ceu_jpg_t local_jpg;
static ceu_dtif_t local_dtif;
static uint32_t local_hwdth;

/**************************************************************************//**
 * Function Name: R_CEU_Initialize
 * @brief       CEU initialization processing
 * @param[in]   init_func       : user func
 * @param[in]   user_num        : user param
 * @retval      None
 ******************************************************************************/
void R_CEU_Initialize(void (* const init_func)(uint32_t), const uint32_t user_num)
{
    /* call back function */
    if (NULL != init_func)
    {
        init_func (user_num);
    }

    /* init */
    local_jpg = CEU_IMAGE_CAPTURE_MODE;
    local_dtif = CEU_8BIT_DATA_PINS;
    local_hwdth = 0;

    return;
} /* End of function R_CEU_Initialize() */

/**************************************************************************//**
 * Function Name: R_CEU_Open
 * @brief       CEU config setting
 * @param[in]   config       : CEU configuration
 * @retval      Error codes of the CEU driver
 ******************************************************************************/
ceu_error_t R_CEU_Open(const ceu_config_t * const config)
{
    ceu_error_t error;

    /* param check */
    error = ceu_open_check_prm (config);

    if (CEU_OK == error)
    {
        local_jpg = config->jpg;
        local_dtif = config->dtif;

        /* Capture Interface Control Register (CAMCR)
         * bit16 FLDPOL - Sets the polarity of the field identification signal (FLD) from an external module.
         * bit12 DTIF - Sets the digital image input pins from which data is to be captured.
         * bit9-8 DTARY[1:0] - These bits set the input order of the luminance component and chrominance component.
         * bit5-4 JPG[1:0] - These bits select the fetched data type. : b'01 (Data synchronous fetch mode)
         * bit1 VDPOL - Sets the polarity for detection of the vertical sync signal input from an external module.
         * bit0 HDPOL - Sets the polarity for detection of the horizontal sync signal input from an external module.
         * other bit is reserved    : write value should always be 0.
         */
        CEU.CAMCR.LONG = (uint32_t) (
                (((((CAMCR_FLDPOL_VALUE << CEU_SHIFT_VALUE_16) | (config->dtif << 12u)) | (config->dtary << 8u))
                        | (config->jpg << 4u)) | (config->vdpol << 1u)) | config->hdpol);

        if (CEU_DATA_ENABLE_MODE != config->jpg)
        {
            if (NULL != config->cap)
            {
                local_hwdth = config->cap->hwdth;

                /* Capture Interface Offset Register (CAMOR)
                 * This register is used, during Data synchronous fetch mode.
                 */
                CEU.CAMOR_A.LONG = (uint32_t) ((config->cap->vofst << 16u) | config->cap->hofst);

                /* Capture Interface Width Register (CAPWR)
                 * This register is used, during Data synchronous fetch mode.
                 */
                CEU.CAPWR_A.LONG = (uint32_t) ((config->cap->vwdth << 16u) | config->cap->hwdth);
            }
        }

        {
            uint32_t vfclp = 0;
            uint32_t hfclp = 0;

            if (CEU_IMAGE_CAPTURE_MODE == config->jpg)
            {
                vfclp = config->clp->vfclp;
                hfclp = config->clp->hfclp;
            }
            else if (CEU_DATA_SYNC_MODE == config->jpg)
            {
                if (CEU_8BIT_DATA_PINS == config->dtif)
                {
                    vfclp = config->cap->vwdth;
                    hfclp = config->cap->hwdth / 2;
                }

                if (CEU_16BIT_DATA_PINS == config->dtif)
                {
                    vfclp = config->cap->vwdth;
                    hfclp = config->cap->hwdth;
                }
            }
            else
            {
                vfclp = 0u;
                hfclp = 0u;
            }

            /* Capture Filter Size Clip Register (CFSZR)
             this register used in Data synchronous fetch mode. */
            CEU.CFSZR_A.LONG = ((vfclp << 16) | hfclp);
        }

        /* Capture Data Output Control Register (CDOCR)
         * bit16 CBE - Controls the number of lines of captured data to be written to the memory.
         * bit4  CDS - Sets the image format when outputting the image data captured
         in the YCbCr 422 format to the memory.
         * bit2 COLS - Controls swapping in 32-bit units for data output from the CEU.
         * bit1 COWS - Controls swapping in 16-bit units for data output from the CEU.
         * bit0 COBS - Controls swapping in 8-bit units for data output from the CEU.
         * other bit is reserved    : write value should always be 0.
         */
        CEU.CDOCR_A.LONG = (uint32_t) (
                ((((CDOCR_CBE_VALUE << CEU_SHIFT_VALUE_16) | (CDOCR_CDS_VALUE << CEU_SHIFT_VALUE_4))
                        | (config->cols << CEU_SHIFT_VALUE_2)) | (config->cows << CEU_SHIFT_VALUE_1)) | config->cobs);

        /* Capture Interface Cycle Register (CMCYR)
         * Set 0 in all bits of this register, during Data enable fetch mode.
         */
        CEU.CMCYR.LONG = (uint32_t) ((CMCYR_VCYL_VALUE << 16u) |
        CMCYR_HCYL_VALUE);

        /* capture control register (CAPCR)
         * bit31-24 FDRP[7:0] - the frame drop interval in continuous frame capture. : set H'00 (Single Capture mode)
         * bit21-20 MTCM[1:0] - These bits specify the unit for transferring data to a bus bridge module. : variable.
         * bit16 CTNCP - Continuous capture bit : b'0 (One-frame capture)
         * other bit is reserved    : write value should always be 0.
         */
        CEU.CAPCR.LONG = (uint32_t) (
                ((CAPCR_FDRP_VALUE << CEU_SHIFT_VALUE_24) | (CAPCR_MTCM_VALUE << CEU_SHIFT_VALUE_20))
                        | (CAPCR_CTNCP_VALUE << CEU_SHIFT_VALUE_16));

        /* Capture Interface Input Format Register (CAIFR)
         * this register used in Data synchronous fetch mode.
         */
        CEU.CAIFR.LONG = (uint32_t) (((CAIFR_IFS_VALUE << CEU_SHIFT_VALUE_8) | (CAIFR_CIM_VALUE << CEU_SHIFT_VALUE_4)) |
        CAIFR_FC_VALUE);

        /* CEU Register Control Register (CRCNTR)
         * bit4 RVS - Sets the timing to switch the register plane in both-field capture.
         * bit1 RS - Specifies which register plane is used by the CEU in synchronization with VD.
         * bit0 RC - Specifies switching of the register plane used by the CEU in synchronization with VD.
         * other bit is reserved    : write value should always be 0.
         */
        CEU.CRCNTR.LONG = (uint32_t) (((CRCNTR_RVS_VALUE << CEU_SHIFT_VALUE_4) | (CRCNTR_RS_VALUE << CEU_SHIFT_VALUE_1)) |
        CRCNTR_RC_VALUE);

        /* Capture Filter Control Register (CFLCR)
         * In data fetch mode, set CFLCR to 0.
         */
        CEU.CFLCR_A.LONG = (uint32_t) (
                (((CFLCR_VMANT_VALUE << CEU_SHIFT_VALUE_28) | (CFLCR_VFRAC_VALUE << CEU_SHIFT_VALUE_16))
                        | (CFLCR_HMANT_VALUE << CEU_SHIFT_VALUE_12)) |
                CFLCR_HFRAC_VALUE);

        /* Memory address for Bundle write */
        CEU.CDBYR_A.LONG = 0u;
        CEU.CDBCR_A.LONG = 0u;
        CEU.CDAYR2_A.LONG = 0u;
        CEU.CDACR2_A.LONG = 0u;
        CEU.CDBYR2_A.LONG = 0u;
        CEU.CDBCR2_A.LONG = 0u;

        /* Capture Bundle Destination Size Register (CBDSR)
         * bit22-0 CBVS[22:0] -
         These bits select the number of lines or number of bytes for output to the memory in a bundle write.
         * other bit is reserved    : write value should always be 0.
         */
        CEU.CBDSR_A.LONG = 0u; /* sample program not support bundle write mode */

        /* Capture Low-Pass Filter Control Register (CLFCR)
         * In data fetch mode, clear the LPF bit to B'0.
         * bit0 LPF - Enables or disables operation of the low-pass filter. : set b'1
         */
        CEU.CLFCR_A.LONG = CLFCR_LPF_VALUE;

        /* Firewall Operation Control Register (CFWCR)
         * bit31-5 FWV[26:0] - These bits specify the upper limit of a write address.
         Specify the upper 27 bits of the 32-bit address. : set 0
         * bit0 FWE - FW enable/disable. : set 0 (disable)
         */
        CEU.CFWCR.LONG = (uint32_t) ((CFWCR_FWV_VALUE << 5u) | CFWCR_FWE_VALUE);
    }

    return error;

} /* End of function R_CEU_Open() */

/**************************************************************************//**
 * Function Name: R_CEU_Execute
 * @brief       CEU Execute
 * @param[in]   cayr          : Capture buffer (Y)
 * @param[in]   cacr          : Capture buffer (CbCr)
 * @param[in]   chdw          : stride
 * @param[in]   auto_capture  : ceu_onoff_t
 * @retval      Error codes of the CEU driver
 ******************************************************************************/
ceu_error_t R_CEU_Execute(const void * cayr, const void * cacr, const uint32_t chdw, ceu_onoff_t auto_capture)
{
    ceu_error_t error;

    /* param check */
    error = ceu_execute_check_prm (cayr, cacr, chdw);
    if (CEU_OK == error)
    {
        CEU.CDAYR_A.LONG = R_CEU_CPUVAddrToSysPAddr((uint32_t) cayr);
        CEU.CDACR_A.LONG = R_CEU_CPUVAddrToSysPAddr((uint32_t) cacr);

        /* Capture Destination Width Register (CDWDR)
         * this register used in Data synchronous fetch mode.
         */
        CEU.CDWDR_A.LONG = chdw;

        if( CEU_ON == auto_capture )
        {
            CEU.CAPCR.LONG |= 0x00010000u;
        }
        else
        {
            CEU.CAPCR.LONG &= ~0x00010000u;
        }

        /* start capture */
        /* Capture Start Register (CAPSR)
         * bit0 CE - Capture Enable : set b'1 (Starts capturing)
         * other bit is not change.
         */
        CEU.CAPSR.LONG |= CAPSR_CE_BIT;
    }

    return error;

} /* End of function R_CEU_Execute() */

#if(1) /* mbed */
/**************************************************************************//**
 * Function Name: R_CEU_Execute_Setting
 * @brief       CEU Execute
 * @param[in]   cayr          : Capture buffer (Y)
 * @param[in]   cacr          : Capture buffer (CbCr)
 * @param[in]   chdw          : stride
 * @param[in]   auto_capture  : ceu_onoff_t
 * @retval      Error codes of the CEU driver
 ******************************************************************************/
ceu_error_t R_CEU_Execute_Setting(const void * cayr, const void * cacr, const uint32_t chdw, ceu_onoff_t auto_capture)
{
    ceu_error_t error;

    /* param check */
    error = ceu_execute_check_prm (cayr, cacr, chdw);
    if (CEU_OK == error)
    {
        CEU.CDAYR_A.LONG = R_CEU_CPUVAddrToSysPAddr((uint32_t) cayr);
        CEU.CDACR_A.LONG = R_CEU_CPUVAddrToSysPAddr((uint32_t) cacr);

        /* Capture Destination Width Register (CDWDR)
         * this register used in Data synchronous fetch mode.
         */
        CEU.CDWDR_A.LONG = chdw;

        if( CEU_ON == auto_capture )
        {
            CEU.CAPCR.LONG |= 0x00010000u;
        }
        else
        {
            CEU.CAPCR.LONG &= ~0x00010000u;
        }
    }

    return error;

} /* End of function R_CEU_Execute() */

/**************************************************************************//**
 * Function Name: R_CEU_Start
 * @brief       CEU Execute
 * @param[in]   None
 * @retval      Error codes of the CEU driver
 ******************************************************************************/
ceu_error_t R_CEU_Start(void)
{
    ceu_error_t error = CEU_OK;

    /* start capture */
    /* Capture Start Register (CAPSR)
     * bit0 CE - Capture Enable : set b'1 (Starts capturing)
     * other bit is not change.
     */
    CEU.CAPSR.LONG |= CAPSR_CE_BIT;

    return error;
} /* End of function R_CEU_Start() */
#endif

/**************************************************************************//**
 * Function Name: R_CEU_Stop
 * @brief       CEU Execute
 * @param[in]   None
 * @retval      Error codes of the CEU driver
 ******************************************************************************/
ceu_error_t R_CEU_Stop(void)
{
    ceu_error_t error = CEU_OK;

    CEU.CAPSR.LONG &= ~0x00000001;

    return error;
} /* End of function R_CEU_Stop() */

/**************************************************************************//**
 * Function Name: R_CEU_Terminate
 * @brief       CEU Close
 * @param[in]   quit_func  : user func
 * @param[in]   user_num   : user param
 * @retval      Error codes of the CEU driver
 ******************************************************************************/
ceu_error_t R_CEU_Terminate(void (* const quit_func)(uint32_t), const uint32_t user_num)
{
    ceu_error_t error = CEU_OK;

    /* software reset
     * bit16 CPKIL : set 1 (Software reset of capturing)
     * bit0 CE     : set 0 (Stops capturing)
     */
    CEU.CAPSR.LONG = CEU_SW_RESET;

    /* wait software reset complete
     * Complete : CSTSR.CPTON = b'0 & CAPSR.CPKIL=b'0
     * [CSTSR]
     * bit0 CPTON - Indicates that the CEU is operating.
     * [CAPSR]
     * bit16 CPKIL - Write 1 to this bit to perform a software reset of capturing.
     */
    while (0u != (CEU.CSTSR.LONG & CSTSR_CPTON_BIT))
    {
        /* wait state :"Halted" */
    }

    while (0u != (CEU.CAPSR.LONG & CAPSR_CPKIL_BIT))
    {
        /* wait state :"Normal" */
    }

    /* call back function */
    if (NULL != quit_func)
    {
        quit_func (user_num);
    }

    return error;
} /* End of function R_CEU_Terminate() */

/**************************************************************************//**
 * Function Name: R_CEU_InterruptEnable
 * @brief       CEU Interrupt Enable
 * @param[in]   int_type       : Capture Event Interrupt Enable Register (CEIER)
 * @param[in]   callback       : Callback func
 * @retval      Error codes of the CEU driver
 ******************************************************************************/
void R_CEU_InterruptEnable(const ceu_int_type_t int_type, void (* const callback)(ceu_int_type_t))
{
    /* Callback function pointer */
    ceu_int_callback = callback;

    /* Capture Event Interrupt Enable Register (CEIER)  */
    CEU.CEIER.LONG = int_type;

} /* End of function R_CEU_InterruptEnable() */

/**************************************************************************//**
 * Function Name: R_CEU_InterruptDisable
 * @brief       CEU Interrupt Disable
 * @retval      None
 ******************************************************************************/
void R_CEU_InterruptDisable(void)
{
    /* Callback function pointer clear */
    ceu_int_callback = 0;

    /* Capture Event Interrupt Enable Register (CEIER) claer */
    CEU.CEIER.LONG = CEIER_CLARE_VALUE;

} /* End of function R_CEU_InterruptDisable() */

/**************************************************************************//**
 * Function Name: R_CEU_Isr
 * @brief       CEU interrupt handler
 * @param[in]   int_sense       :
 * @retval      None
 ******************************************************************************/
void R_CEU_Isr(const uint32_t int_sense)
{
    uint32_t interrupt_flag;

    /* get interrupt flag */
    interrupt_flag = CEU.CETCR.LONG;

    /* clear interrupt flag
     * Capture Event Flag Clear Register (CETCR)
     * To clear the bit corresponding to the interrupt source
     to be cleared to 0 and retain that state, write 1 to that bit.
     */
    CEU.CETCR.LONG = CETCR_CLARE_VALUE; /* clear all */

    if (NULL != ceu_int_callback)
    {
        /* callback func */
        ceu_int_callback ((ceu_int_type_t) interrupt_flag);
    }

} /* End of function R_CEU_Isr() */

/**************************************************************************//**
 * Function Name: ceu_open_check_prm
 * @brief       CEU Open check param
 * @param[in]   config       : CEU configuration
 * @retval      Error codes of the CEU driver
 ******************************************************************************/
static ceu_error_t ceu_open_check_prm(const ceu_config_t * const config)
{
    ceu_error_t error = CEU_OK;

    if (NULL != config)
    {
        if ((CEU_IMAGE_CAPTURE_MODE == config->jpg) || (CEU_DATA_SYNC_MODE == config->jpg))
        {
            if (NULL == config->cap)
            {
                error = CEU_ERR_PARAM;
            }
        }
    }

    if (CEU_OK == error)
    {
        if (NULL != config)
        {
            if ((CEU_IMAGE_CAPTURE_MODE == config->jpg) || (CEU_DATA_SYNC_MODE == config->jpg))
            {
                /* vofst max */
                if (((uint32_t) config->cap->vofst & (uint32_t) ~VOFST_MAX_0FFF) != 0u)
                {
                    error = CEU_ERR_PARAM;
                }

                /* hofst max */
                if (((uint32_t) config->cap->hofst & (uint32_t) ~HOFST_MAX_1FFF) != 0u)
                {
                    error = CEU_ERR_PARAM;
                }
            }
        }
    }

    if (CEU_OK == error)
    {
        if (NULL != config)
        {
            if ((CEU_IMAGE_CAPTURE_MODE == config->jpg) || (CEU_DATA_SYNC_MODE == config->jpg))
            {
                /* vwdth max */
                if (config->cap->vwdth > VWDTH_MAX_1920)
                {
                    error = CEU_ERR_PARAM;
                }

                /* vwdth align 4 */
                if (ceu_align_check (config->cap->vwdth, 4u) != 0)
                {
                    error = CEU_ERR_PARAM;
                }
            }
        }
    }

    if (CEU_OK == error)
    {
        if (NULL != config)
        {
            /* CEU_IMAGE_CAPTURE_MODE & 8bit */
            if ((CEU_IMAGE_CAPTURE_MODE == config->jpg) && (CEU_8BIT_DATA_PINS == config->dtif))
            {
                /* hwdth max */
                if (config->cap->hwdth > HWDTH_MAX_5120)
                {
                    error = CEU_ERR_PARAM;
                }

                /* hwdth align 8 */
                if (ceu_align_check (config->cap->hwdth, 8u) != 0)
                {
                    error = CEU_ERR_PARAM;
                }
            }

            /* CEU_DATA_SYNC_MODE & 8bit */
            if ((CEU_DATA_SYNC_MODE == config->jpg) && (CEU_8BIT_DATA_PINS == config->dtif))
            {
                /* hwdth max */
                if (config->cap->hwdth > HWDTH_MAX_2560)
                {
                    error = CEU_ERR_PARAM;
                }

                /* hwdth align 4 */
                if (ceu_align_check (config->cap->hwdth, 4u) != 0)
                {
                    error = CEU_ERR_PARAM;
                }
            }
        }

        if (NULL != config)
        {
            /* RZ/A1H, M */
            /* CEU_IMAGE_CAPTURE_MODE & 16bit */
            if ((CEU_IMAGE_CAPTURE_MODE == config->jpg) && (CEU_16BIT_DATA_PINS == config->dtif))
            {
                /* hwdth max */
                if (config->cap->hwdth > HWDTH_MAX_2560)
                {
                    error = CEU_ERR_PARAM;
                }

                /* hwdth align 4 */
                if (ceu_align_check (config->cap->hwdth, 4u) != 0)
                {
                    error = CEU_ERR_PARAM;
                }
            }
        }

        if (NULL != config)
        {
            /* CEU_DATA_SYNC_MODE & 16bit */
            if ((CEU_DATA_SYNC_MODE == config->jpg) && (CEU_16BIT_DATA_PINS == config->dtif))
            {
                /* hwdth max */
                if (config->cap->hwdth > HWDTH_MAX_1280)
                {
                    error = CEU_ERR_PARAM;
                }

                /* hwdth align 2 */
                if (ceu_align_check (config->cap->hwdth, 2u) != 0)
                {
                    error = CEU_ERR_PARAM;
                }
            }
        }
    }

    if (CEU_OK == error)
    {
        if (NULL != config)
        {
            if (CEU_IMAGE_CAPTURE_MODE == config->jpg)
            {
                if (NULL == config->clp)
                {
                    error = CEU_ERR_PARAM;
                }
            }
        }
    }

    if (CEU_OK == error)
    {
        if (NULL != config)
        {
            if (CEU_IMAGE_CAPTURE_MODE == config->jpg)
            {
                /* vfclp align 4 */
                if (ceu_align_check (config->clp->vfclp, 4u) != 0)
                {
                    error = CEU_ERR_PARAM;
                }

                /* hfclp align 4 */
                if (ceu_align_check (config->clp->hfclp, 4u) != 0)
                {
                    error = CEU_ERR_PARAM;
                }
            }
        }
    }
    return error;
} /* End of function ceu_open_check_prm() */

/**************************************************************************//**
 * Function Name: ceu_execute_check_prm
 * @brief       CEU Execute check param
 * @param[in]   cayr       : Capture buffer (Y)
 * @param[in]   cacr       : Capture buffer (CbCr)
 * @param[in]   chdw       : stride
 * @retval      Error codes of the CEU driver
 ******************************************************************************/
static ceu_error_t ceu_execute_check_prm(const void * cayr, const void * cacr, const uint32_t chdw)
{
    ceu_error_t error = CEU_OK;

    if (CEU_IMAGE_CAPTURE_MODE == local_jpg)
    {
        if ((NULL == cayr) || (NULL == cacr))
        {
            error = CEU_ERR_PARAM;
        }

        /* cayr align 4 */
        if (ceu_align_check ((uint32_t) cayr, 4u) != 0)
        {
            error = CEU_ERR_PARAM;
        }

        /* cacr align 4 */
        if (ceu_align_check ((uint32_t) cacr, 4u) != 0)
        {
            error = CEU_ERR_PARAM;
        }

        /* chdw max */
        if (chdw > CHDW_MAX_8188)
        {
            error = CEU_ERR_PARAM;
        }

        /* chdw align 4 */
        if (ceu_align_check ((uint32_t) chdw, 4u) != 0)
        {
            error = CEU_ERR_PARAM;
        }
    }

    if (CEU_OK == error)
    {
        if (CEU_DATA_SYNC_MODE == local_jpg)
        {
            if (NULL == cayr)
            {
                error = CEU_ERR_PARAM;
            }

            /* cayr align 4 */
            if (ceu_align_check ((uint32_t) cayr, 4u) != 0)
            {
                error = CEU_ERR_PARAM;
            }

            if (CEU_16BIT_DATA_PINS == local_dtif)
            {
                if ((local_hwdth * 2) != chdw)
                {
                    error = CEU_ERR_PARAM;
                }
            }

            if (CEU_8BIT_DATA_PINS == local_dtif)
            {
                if (local_hwdth != chdw)
                {
                    error = CEU_ERR_PARAM;
                }
            }
        }
    }

    if (CEU_OK == error)
    {
        if (CEU_DATA_ENABLE_MODE == local_jpg)
        {
            if (NULL == cayr)
            {
                error = CEU_ERR_PARAM;
            }

            /* cayr align 4 */
            if (ceu_align_check ((uint32_t) cayr, 32u) != 0)
            {
                error = CEU_ERR_PARAM;
            }
        }
    }

    return error;
} /* End of function ceu_execute_check_prm() */

/**************************************************************************//**
 * Function Name: ceu_align_check
 * @brief       align check
 * @param[in]   value       : check value
 * @param[in]   align       : align value
 * @retval      OK(0) NG(-1)
 ******************************************************************************/
static int32_t ceu_align_check(const uint32_t value, uint32_t align)
{
    int32_t error;

    align -= 1u;
    if ((value & align) != 0)
    {
        error = -1;
    }
    else
    {
        error = 0;
    }

    return error;
} /* End of function ceu_align_check() */

