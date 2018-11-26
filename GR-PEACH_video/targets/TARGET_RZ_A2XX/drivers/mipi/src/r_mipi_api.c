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
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**************************************************************************
* File Name : r_mipi_api.c
* Version : 0.01
* Description : RZ/A2M MIPI driver API function
**************************************************************************/
/***************************************************************************
* History : DD.MM.YYYY Version Description
* : 23.08.2018 0.01 pre version created
**************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    <iodefine.h>

#include    "r_mipi_api.h"

/******************************************************************************
Macro definitions
******************************************************************************/
/* Interrupt table management */
#define MIPI_INTERRUPT_TYPE_NUM  (19u)
#define VIN_INTERRUPT_TYPE_NUM   (7u)
#define INTERRUPT_REGBIT         (0u)
#define INTERRUPT_FLAG           (1u)

/* Interrupt Register Settings */
#define INTEN_MASK_ALL_SET       (0x181FFCDDu)
#define INTEN_ALL_CLEAR          (0x00000000u)
#define VOIE_ALL_CLEAR           (0x00000000u)
#define VNSI_WRITE_MASK          (0x000007FFu)

/* For stride pixel size check */
#define VIN_OUTPUT_FORMAT_NUM    ( 1u)
#define OUTPUT_FORMAT_ENUM       ( 0u)
#define STRIDE_ALIGN_SIZE        ( 1u)
#define PIXEL_ALIGN_32           ( 31u) /* 0x1F is used for align check */
#define PIXEL_ALIGN_64           ( 63u) /* 0x3F is used for align check */
#define PIXEL_ALIGN_128          (127u) /* 0x7F is used for align check */

/******************************************************************************
Typedef definitions
******************************************************************************/
/* Bit definition of MIPI register (INTEN, INTCLOSE, INTSTATE) */
typedef enum
{
    REGBIT_MIPI_INT_LESS_THAN_WC    = 0x10000000,
    REGBIT_MIPI_INT_AFIFO_OF        = 0x08000000,
    REGBIT_MIPI_INT_VD_START        = 0x00100000,
    REGBIT_MIPI_INT_VD_END          = 0x00080000,
    REGBIT_MIPI_INT_SHP_STB         = 0x00040000,
    REGBIT_MIPI_INT_FSFE            = 0x00020000,
    REGBIT_MIPI_INT_LNP_STB         = 0x00010000,
    REGBIT_MIPI_INT_CRC_ERR         = 0x00008000,
    REGBIT_MIPI_INT_HD_WC_ZERO      = 0x00004000,
    REGBIT_MIPI_INT_FRM_SEQ_ERR1    = 0x00002000,
    REGBIT_MIPI_INT_FRM_SEQ_ERR0    = 0x00001000,
    REGBIT_MIPI_INT_ECC_ERR         = 0x00000800,
    REGBIT_MIPI_INT_ECC_CRCT_ERR    = 0x00000400,
    REGBIT_MIPI_INT_ULPS_START      = 0x00000080,
    REGBIT_MIPI_INT_ULPS_END        = 0x00000040,
    REGBIT_MIPI_INT_ERRSOTHS        = 0x00000010,
    REGBIT_MIPI_INT_ERRSOTSYNCHS    = 0x00000008,
    REGBIT_MIPI_INT_ERRESC          = 0x00000004,
    REGBIT_MIPI_INT_ERRCONTROL      = 0x00000001,
} e_bit_mipi_int_type_t;

/* Bit definition of VIN register (VnIE, VnINTS) */
typedef enum
{
    REGBIT_VIN_INT_FIELD2           = 0x80000000,
    REGBIT_VIN_INT_VSYNC_FALL       = 0x00020000,
    REGBIT_VIN_INT_VSYNC_RISE       = 0x00010000,
    REGBIT_VIN_INT_FIELD            = 0x00000010,
    REGBIT_VIN_INT_SCANLINE         = 0x00000004,
    REGBIT_VIN_INT_FRAME            = 0x00000002,
    REGBIT_VIN_INT_FIFO_OF          = 0x00000001 
} e_bit_vin_int_type_t;

/******************************************************************************
Private global variables and functions
******************************************************************************/
/* Static functions */
static uint8_t calc_scale_passband(uint16_t scale_int, uint16_t scale_fra);

/* Mipi Driver Status */
static uint8_t gs_mipi_state;

/* Mipi Callback Function */
static void (* Mipi_Callback  )( e_mipi_interrupt_type_t interrupt_flag);

/* Vin Callback Function */
static void (* Vin_Callback )( e_mipi_interrupt_type_t interrupt_flag);

/* Mipi Error Bit */
static const uint32_t gs_tbl_mipi_interrupt_bit[MIPI_INTERRUPT_TYPE_NUM][2] = {
    { REGBIT_MIPI_INT_LESS_THAN_WC, MIPI_INT_LESS_THAN_WC }, /* INT_LESS_THAN_WC */
    { REGBIT_MIPI_INT_AFIFO_OF    , MIPI_INT_AFIFO_OF     }, /* INT_AFIFO_OF */
    { REGBIT_MIPI_INT_VD_START    , MIPI_INT_VD_START     }, /* INT_VD_START */
    { REGBIT_MIPI_INT_VD_END      , MIPI_INT_VD_END       }, /* INT_VD_END */
    { REGBIT_MIPI_INT_SHP_STB     , MIPI_INT_SHP_STB      }, /* INT_SHP_STB */
    { REGBIT_MIPI_INT_FSFE        , MIPI_INT_FSFE         }, /* INT_FSFE */
    { REGBIT_MIPI_INT_LNP_STB     , MIPI_INT_LNP_STB      }, /* INT_LNP_STB */
    { REGBIT_MIPI_INT_CRC_ERR     , MIPI_INT_CRC_ERR      }, /* INT_CRC_ERR */
    { REGBIT_MIPI_INT_HD_WC_ZERO  , MIPI_INT_HD_WC_ZERO   }, /* INT_HD_WC_ZERO */
    { REGBIT_MIPI_INT_FRM_SEQ_ERR1, MIPI_INT_FRM_SEQ_ERR1 }, /* INT_FRM_SEQ_ERR1 */
    { REGBIT_MIPI_INT_FRM_SEQ_ERR0, MIPI_INT_FRM_SEQ_ERR0 }, /* INT_FRM_SEQ_ERR0 */
    { REGBIT_MIPI_INT_ECC_ERR     , MIPI_INT_ECC_ERR      }, /* INT_ECC_ERR */
    { REGBIT_MIPI_INT_ECC_CRCT_ERR, MIPI_INT_ECC_CRCT_ERR }, /* INT_ECC_CRCT_ERR */
    { REGBIT_MIPI_INT_ULPS_START  , MIPI_INT_ULPS_START   }, /* INT_ULPS_START */
    { REGBIT_MIPI_INT_ULPS_END    , MIPI_INT_ULPS_END     }, /* INT_ULPS_END */
    { REGBIT_MIPI_INT_ERRSOTHS    , MIPI_INT_ERRSOTHS     }, /* INT_ERRSOTHS */
    { REGBIT_MIPI_INT_ERRSOTSYNCHS, MIPI_INT_ERRSOTSYNCHS }, /* INT_ERRSOTSYNCHS */
    { REGBIT_MIPI_INT_ERRESC      , MIPI_INT_ERRESC       }, /* INT_ERRESC */
    { REGBIT_MIPI_INT_ERRCONTROL  , MIPI_INT_ERRCONTROL   }  /* INT_ERRCONTROL */
};

/* Vin Error Bit */
static const uint32_t gs_tbl_vin_interrupt_bit[VIN_INTERRUPT_TYPE_NUM][2] = {
    { REGBIT_VIN_INT_FIELD2    , VIN_INT_FIELD2     }, /* INT_VIN_FIELD2 */
    { REGBIT_VIN_INT_VSYNC_FALL, VIN_INT_VSYNC_FALL }, /* INT_VIN_VSYNC_FALL */
    { REGBIT_VIN_INT_VSYNC_RISE, VIN_INT_VSYNC_RISE }, /* INT_VIN_VSYNC_RISE */
    { REGBIT_VIN_INT_FIELD     , VIN_INT_FIELD      }, /* INT_VIN_FIELD */
    { REGBIT_VIN_INT_SCANLINE  , VIN_INT_SCANLINE   }, /* INT_VIN_SCANLINC */
    { REGBIT_VIN_INT_FRAME     , VIN_INT_FRAME      }, /* INT_VIN_FRAME */
    { REGBIT_VIN_INT_FIFO_OF   , VIN_INT_FIFO_OF    }  /* INT_VIN_FIFO_OF */
};

static const char_t gs_cnvtbl_inputformat_toinf[5] = {
    0x01,       /* INPUT_YCBCR422_8 */
    0x01,       /* INPUT_YCBCR422_8I */
    0x03,       /* INPUT_YCBCR422_10 */
    0x06,       /* INPUT_RGB888 */
    0x04        /* INPUT_RAW8 */
};

static const char_t gs_cnvtbl_interlace_toim[4] = {
    0x00,       /* INTERLACE_ODD */
    0x02,       /* INTERLACE_EVEN */
    0x01,       /* INTERLACE_BOTH */
    0x01        /* PROGRESSIVE */
};

/* Table for stride pixel size checking */
static const uint32_t gs_tbl_vin_stride_check[VIN_OUTPUT_FORMAT_NUM][2] = {
    { VIN_OUTPUT_RAW8       , PIXEL_ALIGN_64     } 
};

/**********************************************************************
*
* Function Name: R_MIPI_Initialize
* Description :  MIPI driver initialization processing
* Arguments :    none
* Return Value : none
**********************************************************************/
void R_MIPI_Initialize(void (* const init_func)(uint32_t), const uint32_t user_num)
{
    /* call back function */
    if (NULL != init_func)
    {
        init_func (user_num);
    }

    /* Mipi driver internal status initialize */
    gs_mipi_state = MIPI_POWOFF;

    return;
}   /* End of function R_MIPI_Initialize() */

/**********************************************************************
*
* Function Name: R_MIPI_Open
* Description :  This function performs the following processing
*                - Parameter check.
*                - MIPI sw reset.
*                - Initialization of PHY register.
* Arguments :    mipi_data : MIPI setting parameter
* Return Value : Error code
**********************************************************************/
e_mipi_error_t R_MIPI_Open(const st_mipi_param_t * const mipi_data)
{
    e_mipi_error_t merr = MIPI_OK;
    uint32_t cnt;

    /* Check MIPI State */
    if (gs_mipi_state != MIPI_POWOFF)
    {
        merr = MIPI_STATUS_ERR;
    }

    /* Check Parameter */
    if (merr == MIPI_OK)
    {
        /* This implicit casting is valid because either values are pointer */
        if (mipi_data == NULL)
        {
            /* NULL CHECK */
            merr = MIPI_PARAM_ERR;
        }
        else if ((mipi_data->mipi_lanenum != 1) && (mipi_data->mipi_lanenum != 2))
        {
            /* Lane Num == 1 or 2 ? */
            merr = MIPI_PARAM_ERR;
        }
        else if (mipi_data->mipi_vc > 3)
        {
            /* Virtual Channel == 0 to 3 ? */
            merr = MIPI_PARAM_ERR;
        }
        else if ((mipi_data->mipi_interlace != MIPI_PROGRESSIVE) && (mipi_data->mipi_interlace != MIPI_INTERLACE))
        {
            /* Interlace or Progressive ? */
            merr = MIPI_PARAM_ERR;
        }
        else if (mipi_data->mipi_laneswap > 1)
        {
            /* Mipi Lane Swap Setting ? */
            merr = MIPI_PARAM_ERR;
        }
        else if (mipi_data->mipi_outputrate > (mipi_data->mipi_lanenum * 1000))
        {
            /* Mipi Data Send Speed ? */
            merr = MIPI_PARAM_ERR;
        }
        else
        {
            /* Do Nothing */
        }
    }

    if (merr == MIPI_OK)
    {
        /* MIPI SW-Reset */
        CSI2LINK.TREF.BIT.TREF = 1;
        /* This implicit casting is valid because unsigned long is acceptable the value */
        CSI2LINK.SRST.BIT.SRST = 1;
        for (cnt = (MIPI_1US_WAIT * 5); cnt > 0; cnt--)
        {
            /* Do nothing 5us wait */
        }
        /* This implicit casting is valid because unsigned long is acceptable the value */
        CSI2LINK.SRST.BIT.SRST = 0;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        CSI2LINK.PHYTIM3.BIT.THS_PREPARE  = mipi_data->mipi_phy_timing.mipi_ths_prepare;    /* MIPI D-PHY Tths_prepare parameter */
        CSI2LINK.PHYTIM3.BIT.THS_SETTLE   = mipi_data->mipi_phy_timing.mipi_ths_settle;     /* MIPI D-PHY Tths_settle parameter */
        CSI2LINK.PHYTIM2.BIT.TCLK_PREPARE = mipi_data->mipi_phy_timing.mipi_tclk_prepare;   /* MIPI D-PHY Tclk_prepare parameter */
        CSI2LINK.PHYTIM2.BIT.TCLK_SETTLE  = mipi_data->mipi_phy_timing.mipi_tclk_settle;    /* MIPI D-PHY Tclk_settle parameter */
        CSI2LINK.PHYTIM2.BIT.TCLK_MISS    = mipi_data->mipi_phy_timing.mipi_tclk_miss;      /* MIPI D-PHY Tclk_miss parameter */
        CSI2LINK.PHYTIM1.BIT.T_INIT_SLAVE = mipi_data->mipi_phy_timing.mipi_t_init_slave;   /* MIPI D-PHY Tint parameter */
        CSI2LINK.FLD.BIT.FLD_NUM     = mipi_data->mipi_frametop;
        /* This implicit casting is valid because unsigned long is acceptable the value */
        CSI2LINK.FLD.BIT.FLD_DET_SEL = 0; /* Top frame */

        if (mipi_data->mipi_interlace == MIPI_INTERLACE)
        {
            /* This implicit casting is valid because unsigned long is acceptable the value */
            CSI2LINK.FLD.BIT.FLD_EN1 = 1; /* Top frame detectable */
        }
        else
        {
            /* This implicit casting is valid because unsigned long is acceptable the value */
            CSI2LINK.FLD.BIT.FLD_EN1 = 0; /* Top frame undetectable */
        }

        /* This implicit casting is valid because unsigned long is acceptable the value */
        CSI2LINK.CHKSUM.BIT.CRC_EN            = 1;      /* CRC Checkable */
        CSI2LINK.CHKSUM.BIT.ECC_EN            = 1;      /* ECC error correctable */
        CSI2LINK.VCDT.BIT.VCDT_EN             = 1;      /* Channel Setting */
        CSI2LINK.VCDT.BIT.SEL_VC              = mipi_data->mipi_vc;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        CSI2LINK.VCDT.BIT.SEL_DT_ON           = 0;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        CSI2LINK.VCDT.BIT.SEL_DT              = 0x2A;   /* RAW8 */
        CSI2LINK.FRDT.BIT.DT_FS               = 0;      /* Frame type setting */
        CSI2LINK.FRDT.BIT.DT_FE               = 1;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        CSI2LINK.LINKCNT.BIT.REG_MONI_PACT_EN = 1;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        CSI2LINK.LINKCNT.BIT.MONITOR_EN       = 1;
        /* This implicit casting is valid because unsigned long is acceptable the value */
        CSI2LINK.PHYCNT.BIT.SHUTDOWNZ         = 1;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        CSI2LINK.PHYCNT.BIT.RSTZ              = 1;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        CSI2LINK.PHYCNT.BIT.ENABLECLK         = 1;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        CSI2LINK.PHYCNT.BIT.ENABLE_0          = 1;

        if (mipi_data->mipi_lanenum == 2)
        {
            /* This implicit casting is valid because unsigned long is acceptable the value */
            CSI2LINK.PHYCNT.BIT.ENABLE_1      = 1;
        }
        else
        {
            /* This implicit casting is valid because unsigned long is acceptable the value */
            CSI2LINK.PHYCNT.BIT.ENABLE_1      = 0;
        }

        for (cnt = (MIPI_1US_WAIT * 25); cnt > 0; cnt--)
        {
            /* Do nothing (25us wait) */
        }

        /* MIPI State Update */
        gs_mipi_state = MIPI_STOP;
    }

    return merr;
}   /* End of function R_MIPI_Open() */

/**********************************************************************
*
* Function Name: R_MIPI_Close
* Description :  This function performs the following processing
*                - Check MIPI state.
*                - MIPI interruput all disable.
*                - VIN capture stop.
*                - MIPI sw-reset.
*                - Calls the user-defined function specified in finalize_func.
* Arguments :    finalize_func : Pointer to a user-defined function
*                user_num  : User defined number
* Return Value : Error code
**********************************************************************/
e_mipi_error_t R_MIPI_Close(void (* const finalize_func)(uint32_t), const uint32_t user_num)
{
    e_mipi_error_t merr = MIPI_OK;

    /* Check MIPI State */
    if( ( gs_mipi_state != MIPI_STOP ) && ( gs_mipi_state != MIPI_CAPTURE ) )
    {
        merr = MIPI_STATUS_ERR;
    }

    if( merr == MIPI_OK )
    {
        /* Interrupt Disable and Mask */
        CSI2LINK.INTEN.LONG    = INTEN_ALL_CLEAR;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        CSI2LINK.INTCLOSE.LONG = INTEN_MASK_ALL_SET;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0IE.LONG          = VOIE_ALL_CLEAR;

        /* Capture Stop */
        VIN.V0MC.BIT.ME = 0;    /* VIN Stop */
        VIN.V0FC.BIT.SC = 0;    /* Capture mode off */
        VIN.V0FC.BIT.CC = 0;

        /* MIPI Reset */
        /* This implicit casting is valid because unsigned long is acceptable the value */
        CSI2LINK.PHYCNT.BIT.SHUTDOWNZ = 0;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        CSI2LINK.PHYCNT.BIT.RSTZ      = 0;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        CSI2LINK.SRST.BIT.SRST        = 1;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        CSI2LINK.SRST.BIT.SRST        = 0;

        /* MIPI State Update */
        gs_mipi_state = MIPI_POWOFF;

        /* Callback function */
        if (finalize_func != 0)
        {
            finalize_func(user_num);
        }
    }

    return merr;
}   /* End of function R_MIPI_Close() */

/**********************************************************************
*
* Function Name: R_MIPI_Setup
* Description :  This function performs the following processing
*                - Parameter check.
*                - Initialization of VIN register.
*                - VIN scaling setting.
*                - VIN clipping setting.
* Arguments :    vin_setup : VIN setting parameter
* Return Value : Error code
**********************************************************************/
e_mipi_error_t R_MIPI_Setup(const st_vin_setup_t * const vin_setup )
{
    e_mipi_error_t merr = MIPI_OK;
    uint8_t scale_enable;

    /* Check MIPI State */
    if( gs_mipi_state != MIPI_STOP )
    {
        merr = MIPI_STATUS_ERR;
    }

    /* Check Parameter */
    if( merr == MIPI_OK )
    {
        /* This implicit casting is valid because either values are pointer */
        if (vin_setup == NULL)
        {
            /* NULL CHECK */
            merr = MIPI_PARAM_ERR;
        }
        else if (vin_setup->vin_yuv_clip > VIN_CLIP_NONE)
        {
            /* RANGE CHECK */
            merr = MIPI_PARAM_ERR;
        }
        else if (vin_setup->vin_lut > VIN_LUT_ON)
        {
            /* RANGE CHECK */
            merr = MIPI_PARAM_ERR;
        }
        else if (vin_setup->vin_inputformat != VIN_INPUT_RAW8)
        {
            /* RANGE CHECK */
            merr = MIPI_PARAM_ERR;
        }
        else if (vin_setup->vin_outputformat != VIN_OUTPUT_RAW8)
        {
            /* RANGE CHECK */
            merr = MIPI_PARAM_ERR;
        }
        else if (vin_setup->vin_outputendian > VIN_OUTPUT_EN_BIG)
        {
            /* RANGE CHECK */
            merr = MIPI_PARAM_ERR;
        }
        else if (vin_setup->vin_dither > VIN_DITHER_ORDERED)
        {
            /* RANGE CHECK */
            merr = MIPI_PARAM_ERR;
        }
        else if (vin_setup->vin_interlace > VIN_PROGRESSIVE)
        {
            /* RANGE CHECK */
            merr = MIPI_PARAM_ERR;
        }
        else if (vin_setup->vin_alpha_val1 > 1)
        {
            /* RANGE CHECK */
            merr = MIPI_PARAM_ERR;
        }
        else if ((vin_setup->vin_ycoffset & 0x7F) != 0)
        {
            /* CHECK MULTIPLE of 128 */
            merr = MIPI_PARAM_ERR;
        }
        else
        {
            /* Check stride size */
            if ((vin_setup->vin_scale.vin_scaleon == VIN_SCALE_ON) &&
                (vin_setup->vin_stride < vin_setup->vin_afterclip.vin_afterclip_size_x))
            {
                /* Minimal than post-clip horizontal size in case of scale on */
                merr = MIPI_PARAM_ERR;
            }
            else if (vin_setup->vin_stride < (vin_setup->vin_preclip.vin_preclip_endx - vin_setup->vin_preclip.vin_preclip_startx))
            {
                /* Minimal than pre-clip horizontal size in case of scale off */
                merr = MIPI_PARAM_ERR;
            }
            else
            {
                uint32_t count;
                for (count = 0; count < VIN_OUTPUT_FORMAT_NUM; count++)
                {
                    if ((vin_setup->vin_outputformat == gs_tbl_vin_stride_check[count][OUTPUT_FORMAT_ENUM]) &&
                        ((vin_setup->vin_stride & gs_tbl_vin_stride_check[count][STRIDE_ALIGN_SIZE]) != 0))
                    {
                        /* Stride size is not aligned with target output format specification */
                        merr = MIPI_PARAM_ERR;
                    }
                }
            }
        }
    }

    if (merr == MIPI_OK)
    {
        /* This driver does not support scaling function */
        if (vin_setup->vin_scale.vin_scaleon == VIN_SCALE_ON)
        {
            merr = MIPI_PARAM_ERR;
        }
    }

    if (merr == MIPI_OK)
    {
        /* Check PreClip Parameter */
        uint8_t min_y = 1;
        if (vin_setup->vin_scale.vin_scaleon == VIN_SCALE_ON)
        {
            min_y = 3;
        }

        if ((vin_setup->vin_preclip.vin_preclip_starty > vin_setup->vin_preclip.vin_preclip_endy) ||
            ((vin_setup->vin_preclip.vin_preclip_endy - vin_setup->vin_preclip.vin_preclip_starty) < min_y))
        {
            /* RANGE CHECK */
            merr = MIPI_PARAM_ERR;
        }
        else if ((vin_setup->vin_preclip.vin_preclip_startx > vin_setup->vin_preclip.vin_preclip_endx) ||
                 ((vin_setup->vin_preclip.vin_preclip_endx - vin_setup->vin_preclip.vin_preclip_startx) < 6) ||
                 ((vin_setup->vin_preclip.vin_preclip_startx % 2) != 0) ||
                 (((vin_setup->vin_preclip.vin_preclip_endx - vin_setup->vin_preclip.vin_preclip_startx) % 2) == 0))
        {
            /* RANGE CHECK */
            merr = MIPI_PARAM_ERR;
        }
        else
        {
            /* Do Nothing */
        }
    }

    if (merr == MIPI_OK)
    {
        /* Check Scale Parameter */
        if (vin_setup->vin_scale.vin_scaleon == VIN_SCALE_ON)
        {
            if ((vin_setup->vin_scale.vin_scale_h < 0x0800) || (vin_setup->vin_scale.vin_scale_v < 0x0556))
            {
                /* RANGE CHECK */
                merr = MIPI_PARAM_ERR;
            }
            else if ((vin_setup->vin_scale.vin_interpolation == VIN_NEAREST)
                       && ((vin_setup->vin_scale.vin_scale_h < 0x1000)
                       ||  (vin_setup->vin_scale.vin_scale_h > 0x4000)
                       ||  (vin_setup->vin_scale.vin_scale_v < 0x1000)
                       ||  (vin_setup->vin_scale.vin_scale_v > 0x4000)))
            {
                /* RANGE CHECK */
                merr = MIPI_PARAM_ERR;
            }
            else
            {
                /* Do Nothing */
            }
        }
    }

    if (merr == MIPI_OK)
    {
        /* Check AfterClip Parameter */
        if (vin_setup->vin_scale.vin_scaleon == VIN_SCALE_ON)
        {
            float32_t pre_clip_size_x;
            float32_t pre_clip_size_y;
            uint16_t  scale_size_x;
            uint16_t  scale_size_y;

            /* Get pre clip size */
            pre_clip_size_x = (float32_t)(vin_setup->vin_preclip.vin_preclip_endx - vin_setup->vin_preclip.vin_preclip_startx);
            /* Get scale size */
            scale_size_x    = (uint16_t)(pre_clip_size_x * (0x1000 / (vin_setup->vin_scale.vin_scale_h)));

            /* Get scaling factor of vartical */
            pre_clip_size_y = (float32_t)(vin_setup->vin_preclip.vin_preclip_endy - vin_setup->vin_preclip.vin_preclip_starty);
            /* Get scale size */
            scale_size_y    = (uint16_t)(pre_clip_size_y * (0x1000 / (vin_setup->vin_scale.vin_scale_v)));

            if ((scale_size_x < vin_setup->vin_afterclip.vin_afterclip_size_x) ||
                (scale_size_y < vin_setup->vin_afterclip.vin_afterclip_size_y) ||
                (vin_setup->vin_afterclip.vin_afterclip_size_x < 4)    ||
                (vin_setup->vin_afterclip.vin_afterclip_size_x > 2048) ||
                (vin_setup->vin_afterclip.vin_afterclip_size_y < 4)    ||
                (vin_setup->vin_afterclip.vin_afterclip_size_y > 2048))
            {
                /* RANGE CHECK */
                merr = MIPI_PARAM_ERR;
            }
        }
    }

    if( merr == MIPI_OK )
    {
        int32_t in_rgb;
        int32_t out_rgb;

        /* VIN Initial Setting */
        VIN.V0MC.BIT.CLP   = vin_setup->vin_yuv_clip;          /* YUV Clip */
        VIN.V0MC.BIT.SCLE  = vin_setup->vin_scale.vin_scaleon; /* Scaling On or Off */
        VIN.V0MC.BIT.LUTE  = vin_setup->vin_lut;               /* LUT conversion */

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0MC.BIT.YCAL  = (vin_setup->vin_inputformat == VIN_INPUT_YCBCR422_8I) ? 1 : 0;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0MC.BIT.INF   = gs_cnvtbl_inputformat_toinf[vin_setup->vin_inputformat];

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0MC.BIT.DC    = vin_setup->vin_dither;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0MC.BIT.EN    = vin_setup->vin_outputendian;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0MC.BIT.IM    = gs_cnvtbl_interlace_toim[vin_setup->vin_interlace];

        if (vin_setup->vin_inputformat == VIN_INPUT_RGB888)
        {
            in_rgb = 1; /* RGB*/
        }
        else if (vin_setup->vin_inputformat == VIN_INPUT_RAW8)
        {
            in_rgb = 2;          /* Not RGB or YUV */
        }
        else
        {
            in_rgb = 0; /* YUV */
        }

        out_rgb                 = (vin_setup->vin_outputformat) >> 4;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0MC.BIT.BPS        = ((in_rgb + out_rgb) == 1 ) ? 0 : 1;   /* 0 : YUV <=> RGB Convert */

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0MC.BIT.ME         = 0;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0CSI_IFMD.BIT.DES0 = 1;                                  /* Eexpand bit to '0' (8 or 10bit -> 12bit) */

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0DMR2.BIT.DES      = 1;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0DMR2.BIT.FTEV     = 1;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0DMR2.BIT.VLV      = 1;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0DMR2.BIT.FTEH     = 0;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0DMR2.BIT.HLV      = 0;

        if (vin_setup->vin_outputformat == VIN_OUTPUT_RAW8)
        {
            /* This implicit casting is valid because unsigned long is acceptable the value */
            VIN.V0IS.LONG           = (vin_setup->vin_stride / 2);
        }
        else
        {
            /* This implicit casting is valid because unsigned long is acceptable the value */
            VIN.V0IS.LONG           = vin_setup->vin_stride;
        }

        /* Pre Clip Setting */
        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0SLPrC.LONG        = vin_setup->vin_preclip.vin_preclip_starty;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0ELPrC.LONG        = vin_setup->vin_preclip.vin_preclip_endy;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0SPPrC.LONG        = vin_setup->vin_preclip.vin_preclip_startx;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0EPPrC.LONG        = vin_setup->vin_preclip.vin_preclip_endx;

        /* Scaling Setting */
        VIN.V0UDS_CTRL.BIT.AMD  =1;
        if (vin_setup->vin_scale.vin_interpolation == VIN_NEAREST)
        {
            /* This implicit casting is valid because unsigned long is acceptable the value */
            VIN.V0UDS_CTRL.BIT.BC     = 0;

            /* This implicit casting is valid because unsigned long is acceptable the value */
            VIN.V0UDS_CTRL.BIT.NE_RCR = 1;

            /* This implicit casting is valid because unsigned long is acceptable the value */
            VIN.V0UDS_CTRL.BIT.NE_BCB = 1;

            /* This implicit casting is valid because unsigned long is acceptable the value */
            VIN.V0UDS_CTRL.BIT.NE_GY  = 1;
        }
        else if (vin_setup->vin_scale.vin_interpolation == VIN_BILINEAR)
        {
            /* This implicit casting is valid because unsigned long is acceptable the value */
            VIN.V0UDS_CTRL.BIT.BC     = 0;

            /* This implicit casting is valid because unsigned long is acceptable the value */
            VIN.V0UDS_CTRL.BIT.NE_RCR = 0;

            /* This implicit casting is valid because unsigned long is acceptable the value */
            VIN.V0UDS_CTRL.BIT.NE_BCB = 0;

            /* This implicit casting is valid because unsigned long is acceptable the value */
            VIN.V0UDS_CTRL.BIT.NE_GY  = 0;
        }
        else
        {
            /* This implicit casting is valid because unsigned long is acceptable the value */
            VIN.V0UDS_CTRL.BIT.BC     = 1;

            /* This implicit casting is valid because unsigned long is acceptable the value */
            VIN.V0UDS_CTRL.BIT.NE_RCR = 0;

            /* This implicit casting is valid because unsigned long is acceptable the value */
            VIN.V0UDS_CTRL.BIT.NE_BCB = 0;

            /* This implicit casting is valid because unsigned long is acceptable the value */
            VIN.V0UDS_CTRL.BIT.NE_GY  = 0;
        }

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0UDS_SCALE.LONG = (vin_setup->vin_scale.vin_scale_h << 16) | (vin_setup->vin_scale.vin_scale_v);
        {
            uint16_t scale_integral;
            uint16_t scale_fractional;
            uint8_t  passband_width;

            scale_integral   = (vin_setup->vin_scale.vin_scale_h & 0xF000) >> 12;  /* Get HMANT part value */
            scale_fractional = (vin_setup->vin_scale.vin_scale_h & 0x0FFF);        /* Get HFRAC part value */
            /* Set BWIDTH_H */
            if (scale_integral == 0) /* enlarge */
            {
                /* This implicit casting is valid because unsigned long is acceptable the value */
                VIN.V0UDS_PASS_BWIDTH.BIT.BWIDTH_H = 64;
            }
            else
            {
                passband_width = calc_scale_passband(scale_integral, scale_fractional);

                /* This implicit casting is valid because unsigned long is acceptable the value */
                VIN.V0UDS_PASS_BWIDTH.BIT.BWIDTH_H = passband_width;
            }

            scale_integral   = (vin_setup->vin_scale.vin_scale_v & 0xF000) >> 12;  /* Get VMANT part value */
            scale_fractional = (vin_setup->vin_scale.vin_scale_v & 0x0FFF);        /* Get VFRAC part value */
            /* Set BWIDTH_V */
            if (scale_integral == 0) /* enlarge */
            {
                /* This implicit casting is valid because unsigned long is acceptable the value */
                VIN.V0UDS_PASS_BWIDTH.BIT.BWIDTH_V = 64;
            }
            else
            {
                passband_width = calc_scale_passband(scale_integral, scale_fractional);

                /* This implicit casting is valid because unsigned long is acceptable the value */
                VIN.V0UDS_PASS_BWIDTH.BIT.BWIDTH_V = passband_width;
            }
        }

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0UDS_CLIP_SIZE.BIT.CL_HSIZE = vin_setup->vin_afterclip.vin_afterclip_size_x;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0UDS_CLIP_SIZE.BIT.CL_VSIZE = vin_setup->vin_afterclip.vin_afterclip_size_y;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0DMR.BIT.A8BIT              = vin_setup->vin_alpha_val8;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0DMR.BIT.EVA                = 0;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0DMR.BIT.YMODE   = 0;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0DMR.BIT.EXRGB   = 0;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0DMR.BIT.BPSM    = 0;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0DMR.BIT.ABIT    = 0;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0DMR.BIT.DTMD    = 0;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0UVAOF.BIT.UVAOF = 0;
    }

    return merr;
}   /* End of function R_MIPI_Setup() */

/**********************************************************************
*
* Function Name: R_MIPI_SetBufferAdr
* Description :  This function performs the following processing
*                - Parameter check.
*                - VIN setting buffer address.
* Arguments :    buffer_no  : Select buffer base (MB1, MB2, MB3)
*                bufferBase : Buffer base address
* Return Value : Error code
**********************************************************************/
e_mipi_error_t R_MIPI_SetBufferAdr(const uint8_t buffer_no, const uint8_t * const bufferBase)
{
    e_mipi_error_t merr  = MIPI_OK;

    /* This casting is valid because uint32_t and unsigned long is same byte length */
    uint32_t vin_intf  = VIN.V0IE.LONG;

    /* This implicit casting is valid because uint32_t and unsigned long is same 1byte length */
    uint32_t mipi_intf = CSI2LINK.INTCLOSE.LONG;

    /* This implicit casting is valid because unsigned long is acceptable the value */
    VIN.V0IE.LONG          = 0;

    /* This implicit casting is valid because unsigned long is acceptable the value */
    CSI2LINK.INTCLOSE.LONG = INTEN_MASK_ALL_SET;

    /* Check MIPI State */
    if( ( gs_mipi_state != MIPI_STOP ) && ( gs_mipi_state != MIPI_CAPTURE ) )
    {
        merr = MIPI_STATUS_ERR;
    }

    /* Check Parameter */
    if( merr == MIPI_OK )
    {
        if( buffer_no > 2 )
        {
            merr = MIPI_PARAM_ERR;
        }
        /* This implicit casting(right side statement) is valid because either values are pointer */
        else if(bufferBase == NULL)
        {
            merr = MIPI_PARAM_ERR;
        }
        /* This casting is valid because this processing just checking address as value */
        else if( ( ((uint32_t)bufferBase) & 0x7FUL ) != 0 )
        {
            merr = MIPI_PARAM_ERR;
        }
    }

    if( merr == MIPI_OK )
    {
        if( buffer_no == 0 )
        {
            /* This casting is valid because this function required to hand the pointer as value */
            VIN.V0MB1.LONG = R_MIPI_CPUVAddrToSysPAddr((uint32_t)bufferBase);
        }
        else if( buffer_no == 1 )
        {
            /* This casting is valid because this function required to hand the pointer as value */
            VIN.V0MB2.LONG = R_MIPI_CPUVAddrToSysPAddr((uint32_t)bufferBase);
        }
        else
        {
            /* This casting is valid because this function required to hand the pointer as value */
            VIN.V0MB3.LONG = R_MIPI_CPUVAddrToSysPAddr((uint32_t)bufferBase);
        }
    }

    /* This implicit casting is valid because unsigned long is acceptable the value */
    VIN.V0IE.LONG          = vin_intf;

    /* This implicit casting is valid because unsigned long is acceptable the value */
    CSI2LINK.INTCLOSE.LONG = mipi_intf;

    return merr;
}   /* End of function R_MIPI_SetBufferAdr() */

/**********************************************************************
*
* Function Name: R_MIPI_InterruptEnable
* Description :  Set the interrupt enable register of MIPI and VIN
* Arguments :    param : Interrupt settings
* Return Value : none
**********************************************************************/
void R_MIPI_InterruptEnable(const st_mipi_int_t * const param)
{
    uint32_t int_type      = param->type;
    uint32_t mipi_int_type = 0;
    uint32_t vin_int_type  = 0;
    uint32_t count;

    /* At first, mask all interrupt */
    VIN.V0IE.LONG          = VOIE_ALL_CLEAR;      /* Set all VIN interrupt disable at once */
    CSI2LINK.INTEN.LONG    = INTEN_ALL_CLEAR;     /* Set all MIPI interrupt disable at once */
    CSI2LINK.INTCLOSE.LONG = INTEN_MASK_ALL_SET;  /* Mask all MIPI interrupt at once  */

    /* Callback function pointer */
    Mipi_Callback = param->p_mipiCallback;
    Vin_Callback  = param->p_vinCallback;

    /* check interrupt type of MIPI */
    for (count = 0; count < MIPI_INTERRUPT_TYPE_NUM; count++)
    {
        if((int_type & gs_tbl_mipi_interrupt_bit[count][INTERRUPT_FLAG]) == gs_tbl_mipi_interrupt_bit[count][INTERRUPT_FLAG])
        {
            mipi_int_type |= gs_tbl_mipi_interrupt_bit[count][INTERRUPT_REGBIT];
        }
    }

    /* check interrupt type of VIN */
    for (count = 0; count < VIN_INTERRUPT_TYPE_NUM; count++)
    {
        if((int_type & gs_tbl_vin_interrupt_bit[count][INTERRUPT_FLAG]) == gs_tbl_vin_interrupt_bit[count][INTERRUPT_FLAG])
        {
            vin_int_type |= gs_tbl_vin_interrupt_bit[count][INTERRUPT_REGBIT];
            if (gs_tbl_vin_interrupt_bit[count][1] == VIN_INT_SCANLINE)
            {
                /* This implicit casting is valid because unsigned long is acceptable the value */
                VIN.V0SI.BIT.SI = (param->line_num & VNSI_WRITE_MASK);
            }
        }
    }

    /* Interrupt Enable */
    CSI2LINK.INTSTATE.LONG |=  mipi_int_type;  /* Clear all MIPI interrupt state */
    VIN.V0INTS.LONG        |=  vin_int_type;   /* Clear all VIN  interrupt state */

    /* This implicit casting is valid because unsigned long is acceptable the value */
    CSI2LINK.INTEN.LONG    |=  mipi_int_type;                         /* Set MIPI interrupt enable */
    CSI2LINK.INTCLOSE.LONG  =  (INTEN_MASK_ALL_SET - mipi_int_type);  /* Mask unused interrupt */
    VIN.V0IE.LONG          |=  vin_int_type;                          /* Set VIN interrupt enable */
}   /* End of function R_MIPI_InterruptEnable() */

/**********************************************************************
*
* Function Name: R_MIPI_InterruptDisable
* Description :  Clear the interrupt enable register of MIPI and VIN
* Arguments :    none
* Return Value : none
**********************************************************************/
void R_MIPI_InterruptDisable(void)
{
    /* Callback function pointer clear */
    Mipi_Callback = 0;
    Vin_Callback  = 0;

    /* Interrupt Enable Register claer */
    VIN.V0IE.LONG          = VOIE_ALL_CLEAR;      /* Set all VIN interrupt disable */
    CSI2LINK.INTEN.LONG    = INTEN_ALL_CLEAR;     /* Set all MIPI interrupt disable */
    CSI2LINK.INTCLOSE.LONG = INTEN_MASK_ALL_SET;  /* Mask all MIPI interrupt */
}   /* End of function R_MIPI_InterruptDisable() */

/**********************************************************************
*
* Function Name: R_MIPI_GetInfo
* Description :  This function performs the following processing
*                - Getting registers status (Line Count/Field Status/Frame Buffer Status).
* Arguments :    infoType : registers status
* Return Value : Error code
**********************************************************************/
e_mipi_error_t R_MIPI_GetInfo(st_vin_info_type_t * infoType)
{
    e_mipi_error_t merr = MIPI_OK;

    /* Check MIPI State */
    if( ( gs_mipi_state != MIPI_STOP ) && ( gs_mipi_state != MIPI_CAPTURE ) ){
        merr = MIPI_STATUS_ERR;
    }

    /* Set Status */
    infoType->vin_nowcaptureline  = VIN.V0LC.BIT.LC; /* Line Count */
    infoType->vin_nowcapturefield = VIN.V0MS.BIT.FS; /* Field Status */
    infoType->vin_nowcapturebase  = VIN.V0MS.BIT.FBS;/* Frame Buffer Status */

    return merr;
}   /* End of function R_MIPI_GetInfo() */

/**********************************************************************
*
* Function Name: R_MIPI_CaptureStart
* Description :  This function performs the following processing
*                - Setting capture mode( Single or Continuous ).
*                - Capture start.
* Arguments :    captureMode : Single or Continuous
* Return Value : Error code
**********************************************************************/
e_mipi_error_t R_MIPI_CaptureStart(const e_mipi_capture_mode_t captureMode)
{
    e_mipi_error_t merr = MIPI_OK;
    uint32_t intstate;

    /* This casting is valid because uint32_t and unsigned long is same byte length */
    uint32_t vin_intf = VIN.V0IE.LONG;

    /* This implicit casting is valid because uint32_t and unsigned long is same 1byte length */
    uint32_t mipi_intf = CSI2LINK.INTCLOSE.LONG;

    /* This implicit casting is valid because unsigned long is acceptable the value */
    VIN.V0IE.LONG = 0;

    /* This implicit casting is valid because unsigned long is acceptable the value */
    CSI2LINK.INTCLOSE.LONG = INTEN_MASK_ALL_SET;

    /* Check MIPI State */
    if( ( gs_mipi_state != MIPI_STOP ) && ( gs_mipi_state != MIPI_CAPTURE ) )
    {
        merr = MIPI_STATUS_ERR;
    }

    if( merr == MIPI_OK )
    {
        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0FC.BIT.CC = 0;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0FC.BIT.SC = 0;

        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0MC.BIT.ME = 1; /* Capture Enable */

        if (captureMode == MIPI_SINGLE_MODE)
        {
            /* This implicit casting is valid because unsigned long is acceptable the value */
            VIN.V0FC.BIT.SC = 1; /* Single Capture Mode */
        }
        else
        {
            /* This implicit casting is valid because unsigned long is acceptable the value */
            VIN.V0FC.BIT.CC = 1;
        }

        /* This implicit casting is valid because uint32_t and unsigned long is same 1byte length */
        intstate               = CSI2LINK.INTSTATE.LONG;

        /* This implicit casting is valid because uint32_t and unsigned long is same 1byte length */
        CSI2LINK.INTSTATE.LONG = intstate;

        /* This implicit casting is valid because uint32_t and unsigned long is same 1byte length */
        intstate               = VIN.V0INTS.LONG;

        /* This implicit casting is valid because uint32_t and unsigned long is same 1byte length */
        VIN.V0INTS.LONG        = intstate;
        gs_mipi_state             = MIPI_CAPTURE;
    }

    /* This implicit casting is valid because unsigned long is acceptable the value */
    VIN.V0IE.LONG          = vin_intf;

    /* This implicit casting is valid because uint32_t and unsigned long is same 1byte length */
    CSI2LINK.INTCLOSE.LONG = mipi_intf;

    return merr;
}   /* End of function R_MIPI_CaptureStart() */

/**********************************************************************
*
* Function Name: R_MIPI_CaptureStop
* Description :  This function performs the following processing
*                - Capture stop.
* Arguments :    none
* Return Value : Error code
**********************************************************************/
e_mipi_error_t R_MIPI_CaptureStop(void)
{
    e_mipi_error_t merr      = MIPI_OK;

    /* This casting is valid because uint32_t and unsigned long is same byte length */
    uint32_t     vin_intf  = VIN.V0IE.LONG;

    /* This implicit casting is valid because uint32_t and unsigned long is same 1byte length */
    uint32_t     mipi_intf = CSI2LINK.INTCLOSE.LONG;

    /* This implicit casting is valid because unsigned long is acceptable the value */
    VIN.V0IE.LONG          = 0;

    /* This implicit casting is valid because unsigned long is acceptable the value */
    CSI2LINK.INTCLOSE.LONG = INTEN_MASK_ALL_SET;

    /* Check MIPI State */
    if( gs_mipi_state != MIPI_CAPTURE )
    {
        merr = MIPI_STATUS_ERR;
    }

    if( merr == MIPI_OK )
    {
        /* This implicit casting is valid because unsigned long is acceptable the value */
        VIN.V0MC.BIT.ME = 0;         /* Capture Disable */
        gs_mipi_state      = MIPI_STOP;
    }

    /* This implicit casting is valid because unsigned long is acceptable the value */
    VIN.V0IE.LONG          = vin_intf;

    /* This implicit casting is valid because uint32_t and unsigned long is same 1byte length */
    CSI2LINK.INTCLOSE.LONG = mipi_intf;

    return merr;
}   /* End of function R_MIPI_CaptureStop() */

/**********************************************************************
*
* Function Name: R_MIPI_InterruptHandler
* Description :  This function performs the following processing
*                - Check MIPI interrupt type.
*                - Call the callback funtion.
* Arguments :    int_sense   : sense
* Return Value : none
**********************************************************************/
void R_MIPI_InterruptHandler( uint32_t int_sense )
{
    volatile uint32_t intdata;
    uint32_t          mipi_cnt;
    uint32_t          mipi_int_type = 0;

    /* This implicit casting is valid because uint32_t and unsigned long is same 1byte length */
    intdata                = CSI2LINK.INTSTATE.LONG;

    /* This implicit casting is valid because uint32_t and unsigned long is same 1byte length */
    CSI2LINK.INTSTATE.LONG = intdata;    /* Clear interrupt status */

    /* check interrupt type of MIPI */
    for (mipi_cnt = 0; mipi_cnt < MIPI_INTERRUPT_TYPE_NUM; mipi_cnt++)
    {
        if((intdata & gs_tbl_mipi_interrupt_bit[mipi_cnt][INTERRUPT_REGBIT]) == gs_tbl_mipi_interrupt_bit[mipi_cnt][INTERRUPT_REGBIT])
        {
            mipi_int_type |= gs_tbl_mipi_interrupt_bit[mipi_cnt][INTERRUPT_FLAG];
        }
    }

    /* If callback function is set, call it */
    if (Mipi_Callback != NULL)
    {
        /* This casting is valid because the value never out of range of e_mipi_interrupt_type_t */
        Mipi_Callback((e_mipi_interrupt_type_t)mipi_int_type);
    }
}   /* End of function R_MIPI_InterruptHandler() */

/**********************************************************************
*
* Function Name: R_VIN_InterruptHandler
* Description :  This function performs the following processing
*                - Check VIN interrupt type.
*                - Call the callback funtion.
* Arguments :    int_sense   : sense
* Return Value : none
**********************************************************************/
void R_VIN_InterruptHandler( uint32_t int_sense )
{
    volatile uint32_t intdata;
    uint32_t          vin_cnt;
    uint32_t          vin_int_type = 0;

    /* This casting is valid because uint32_t and unsigned long is same byte length */
    intdata         = VIN.V0INTS.LONG;

    /* This implicit casting is valid because unsigned long is acceptable the value */
    VIN.V0INTS.LONG = intdata;   /* Clear interrupt status */

    /* check interrupt type of VIN */
    for (vin_cnt = 0; vin_cnt < VIN_INTERRUPT_TYPE_NUM; vin_cnt++)
    {
        if((intdata & gs_tbl_vin_interrupt_bit[vin_cnt][INTERRUPT_REGBIT]) == gs_tbl_vin_interrupt_bit[vin_cnt][INTERRUPT_REGBIT])
        {
            vin_int_type |= gs_tbl_vin_interrupt_bit[vin_cnt][INTERRUPT_FLAG];
        }
    }

    /* If callback function is set, call it */
    if (Vin_Callback != NULL)
    {
        /* This casting is valid because the value never out of range of e_mipi_interrupt_type_t */
        Vin_Callback((e_mipi_interrupt_type_t)vin_int_type);
    }
}   /* End of function R_VIN_InterruptHandler() */

/**********************************************************************
*
* Function Name: calc_scale_passband
* Description :  This function calculate video passband width
* Arguments :    scale_int   : integral part of scaling factor
*                scale_fra   : fractional part of scaling factor
* Return Value : pass band width
**********************************************************************/
static uint8_t calc_scale_passband(uint16_t scale_int, uint16_t scale_fra)
{
    uint8_t mulm;
    uint8_t width;
    
    mulm = 1;
    if (scale_int >= 8)
    {
        mulm = 4;
    }
    else if (scale_int >= 4)
    {
        mulm = 2;
    }
    /****** Calculate passband width ******/
    width = (uint8_t) ((64 * (4096 * mulm)) / ((4096 * scale_int) + scale_fra));
    
    return width;
}   /* End of function calc_scale_passband() */
