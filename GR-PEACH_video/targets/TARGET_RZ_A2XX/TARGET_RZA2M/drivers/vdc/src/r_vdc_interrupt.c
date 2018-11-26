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
* @file         r_vdc_interrupt.c
* @version      0.01
* $Rev: 18 $
* $Date:: 2014-05-29 11:11:38 +0900#$
* @brief        RZ/A2M VDC driver interrupt related processing
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_vdc.h"
#include    "r_vdc_user.h"
#include    "r_vdc_register.h"


/******************************************************************************
Macro definitions
******************************************************************************/
#define     UNUSED_PARAM(param)             (void)(param)

#define VDC_INT_BIT_S0_VI_VSYNC    ((uint32_t)0x00000001u)
#define VDC_INT_BIT_S0_LO_VSYNC    ((uint32_t)0x00000010u)
#define VDC_INT_BIT_S0_VSYNCERR    ((uint32_t)0x00000100u)
#define VDC_INT_BIT_VLINE          ((uint32_t)0x00001000u)
#define VDC_INT_BIT_S0_VFIELD      ((uint32_t)0x00010000u)
#define VDC_INT_BIT_IV1_VBUFERR    ((uint32_t)0x00100000u)
#define VDC_INT_BIT_IV3_VBUFERR    ((uint32_t)0x01000000u)
#define VDC_INT_BIT_IV5_VBUFERR    ((uint32_t)0x10000000u)
#define VDC_INT_BIT_IV6_VBUFERR    ((uint32_t)0x00000001u)
#define VDC_INT_BIT_S0_WLINE       ((uint32_t)0x00000010u)

#define VDC_GR_UPDATE_P_VEN_BIT    ((uint32_t)0x00000010u)
#define VDC_SCL0_UPDATE_VEN_A_BIT  ((uint32_t)0x00000001u)

/* Valid bit range */
#define VDC_INT_RANGE_0X000007FF   (0x000007FFu)

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static void VDC_Ch0_s0_vi_vsync_ISR(const uint32_t int_sense);
static void VDC_Ch0_s0_lo_vsync_ISR(const uint32_t int_sense);
static void VDC_Ch0_s0_vsyncerr_ISR(const uint32_t int_sense);
static void VDC_Ch0_vline_ISR(const uint32_t int_sense);
static void VDC_Ch0_s0_vfield_ISR(const uint32_t int_sense);
static void VDC_Ch0_iv1_vbuferr_ISR(const uint32_t int_sense);
static void VDC_Ch0_iv3_vbuferr_ISR(const uint32_t int_sense);
static void VDC_Ch0_iv5_vbuferr_ISR(const uint32_t int_sense);
static void VDC_Ch0_iv6_vbuferr_ISR(const uint32_t int_sense);
static void VDC_Ch0_s0_wline_ISR(const uint32_t int_sense);

/*! List of the callback function pointers */
static void (*vdc_int_callback[VDC_INT_TYPE_NUM])(vdc_int_type_t int_type);


/**************************************************************************//**
 * @brief       Interrupt service routine acquisition processing
 *
 *              Description:<br>
 *              This function returns the function pointer to the specified interrupt service routine.
 *              It returns a '0' if the channel specified in ch or the interrupt type specified in type
 *              is found invalid.
 * @param[in]   ch                      : Channel
 *                                        - VDC_CHANNEL_0
 * @param[in]   type                    : VDC interrupt type
 * @retval      VDC Interrupt service routine
 *****************************************************************************/
void (*R_VDC_GetISR (const vdc_channel_t ch, const vdc_int_type_t type))(const uint32_t int_sense)
{
    static void (* const isr_table[VDC_INT_TYPE_NUM])(const uint32_t int_sense) =
    {
        &VDC_Ch0_s0_vi_vsync_ISR,
        &VDC_Ch0_s0_lo_vsync_ISR,
        &VDC_Ch0_s0_vsyncerr_ISR,
        &VDC_Ch0_vline_ISR,
        &VDC_Ch0_s0_vfield_ISR,
        &VDC_Ch0_iv1_vbuferr_ISR,
        &VDC_Ch0_iv3_vbuferr_ISR,
        &VDC_Ch0_iv5_vbuferr_ISR,
        &VDC_Ch0_iv6_vbuferr_ISR,
        &VDC_Ch0_s0_wline_ISR
    };
    void (* isr_function)(const uint32_t int_sense);

    isr_function = isr_table[type];

#ifdef  R_VDC_CHECK_PARAMETERS
    /* Channel and interrupt type */
    if ((ch >= VDC_CHANNEL_NUM) || (type >= VDC_INT_TYPE_NUM))
    {
        isr_function = 0;
    }
#endif  /* R_VDC_CHECK_PARAMETERS */

     return isr_function;
}   /* End of function R_VDC_GetISR() */

/**************************************************************************//**
 * @brief       Disables all VDC interrupts
 * @param[in]   void
 * @retval      None
 *****************************************************************************/
void VDC_Int_Disable (void)
{
    const vdc_regaddr_system_ctrl_t  * system_ctrl;
    int32_t                             int_type;

    system_ctrl = &vdc_regaddr_system_ctrl;

    for (int_type = 0; int_type < VDC_INT_TYPE_NUM; int_type++)
    {
        vdc_int_callback[int_type] = 0;
    }
    /* Interrupt output off */
    *(system_ctrl->syscnt_int4) = 0u;
    *(system_ctrl->syscnt_int5) = 0u;

}   /* End of function VDC_Int_Disable() */

/**************************************************************************//**
 * @brief       Enables/disables the specified VDC interrupt
 * @param[in]   param                   : Interrupt callback setup parameter
 * @retval      None
 *****************************************************************************/
void VDC_Int_SetInterrupt (const vdc_int_t * const param)
{
    volatile uint32_t     * int_clhd_reg;
    volatile uint32_t     * int_onoff_reg;
    volatile uint32_t     * linenum_reg;
    volatile uint16_t     * linenum16_reg;
    volatile uint32_t     * linenum_update_reg;
    uint32_t                mask_bit;
    uint32_t                reg_data;
    static const uint32_t   interrupt_bit_table[VDC_INT_TYPE_NUM] =
    {
        VDC_INT_BIT_S0_VI_VSYNC,
        VDC_INT_BIT_S0_LO_VSYNC,
        VDC_INT_BIT_S0_VSYNCERR,
        VDC_INT_BIT_VLINE,
        VDC_INT_BIT_S0_VFIELD,
        VDC_INT_BIT_IV1_VBUFERR,
        VDC_INT_BIT_IV3_VBUFERR,
        VDC_INT_BIT_IV5_VBUFERR,
        VDC_INT_BIT_IV6_VBUFERR,
        VDC_INT_BIT_S0_WLINE
    };

    if (param->type < VDC_INT_TYPE_IV6_VBUFERR)
    {   /* INT0: VDC_INT_TYPE_S0_VI_VSYNC ~ INT7: VDC_INT_TYPE_IV5_VBUFERR */
        int_clhd_reg  = vdc_regaddr_system_ctrl.syscnt_int1;
        int_onoff_reg = vdc_regaddr_system_ctrl.syscnt_int4;
    }
    else
    {   /* INT8: VDC_INT_TYPE_IV6_VBUFERR ~ INT9: VDC_INT_TYPE_S0_WLINE */
        int_clhd_reg  = vdc_regaddr_system_ctrl.syscnt_int2;
        int_onoff_reg = vdc_regaddr_system_ctrl.syscnt_int5;
    }
    mask_bit = interrupt_bit_table[param->type];

    /* Interrupt callback function pointer */
    vdc_int_callback[param->type] = param->callback;

    if (param->callback != 0)
    {   /* ON */
        if ((*int_onoff_reg & mask_bit) == 0u)
        {   /* OFF to ON */
            *int_onoff_reg |= mask_bit;
            reg_data        = *int_onoff_reg;
            *int_clhd_reg   = reg_data;
        }

        if (param->type == VDC_INT_TYPE_VLINE)
        {   /* Specified line signal for panel output in graphics 3 */
            linenum_reg         = vdc_regaddr_img_synthesizer[VDC_GR_TYPE_GR3].gr_clut;
            linenum_update_reg  = vdc_regaddr_img_synthesizer[VDC_GR_TYPE_GR3].gr_update;
            *linenum_reg        &= (uint32_t)~VDC_INT_RANGE_0X000007FF;
            *linenum_reg        |= (uint32_t)param->line_num;
            *linenum_update_reg |= VDC_GR_UPDATE_P_VEN_BIT;
        }
        else if (param->type == VDC_INT_TYPE_S0_WLINE)
        {   /* Write specification line signal input to scaling-down control block in scaler 0 */
            linenum16_reg       = vdc_regaddr_scaler.scl0_int;
            linenum_update_reg  = vdc_regaddr_scaler.scl0_update;
            *linenum16_reg       = param->line_num;
            *linenum_update_reg |= VDC_SCL0_UPDATE_VEN_A_BIT;
        }
        else
        {   /* Do nothing */
        }
    }
    else
    {   /* OFF */
        *int_onoff_reg &= (uint32_t)~mask_bit;
        reg_data        = *int_onoff_reg;
        *int_clhd_reg   = reg_data;
    }

}   /* End of function VDC_Int_SetInterrupt() */

/**************************************************************************//**
 * @brief       VDC S0_VI_VSYNC interrupt service routine
 * @param[in]   int_sense
 * @retval      None
 *****************************************************************************/
static void VDC_Ch0_s0_vi_vsync_ISR (const uint32_t int_sense)
{
    uint32_t            IntState;
    volatile uint32_t * int_clhd_reg;
    volatile uint32_t * int_onoff_reg;

    UNUSED_PARAM(int_sense);

    int_clhd_reg  = vdc_regaddr_system_ctrl.syscnt_int1;
    int_onoff_reg = vdc_regaddr_system_ctrl.syscnt_int4;
    IntState      = *int_onoff_reg;

    if (((IntState & VDC_INT_BIT_S0_VI_VSYNC) != 0u) && ((*int_clhd_reg & VDC_INT_BIT_S0_VI_VSYNC) != 0u))
    {
        /* Clear */
        *int_clhd_reg = IntState & ~VDC_INT_BIT_S0_VI_VSYNC;

        if (vdc_int_callback[VDC_INT_TYPE_S0_VI_VSYNC] != 0)
        {
            vdc_int_callback[VDC_INT_TYPE_S0_VI_VSYNC](VDC_INT_TYPE_S0_VI_VSYNC);
        }
        /* Set */
        *int_clhd_reg = IntState;
    }
}   /* End of function VDC_Ch0_s0_vi_vsync_ISR() */

/**************************************************************************//**
 * @brief       VDC S0_LO_VSYNC interrupt service routine
 * @param[in]   int_sense
 * @retval      None
 *****************************************************************************/
static void VDC_Ch0_s0_lo_vsync_ISR (const uint32_t int_sense)
{
    uint32_t            IntState;
    volatile uint32_t * int_clhd_reg;
    volatile uint32_t * int_onoff_reg;

    UNUSED_PARAM(int_sense);

    int_clhd_reg  = vdc_regaddr_system_ctrl.syscnt_int1;
    int_onoff_reg = vdc_regaddr_system_ctrl.syscnt_int4;
    IntState      = *int_onoff_reg;

    if (((IntState & VDC_INT_BIT_S0_LO_VSYNC) != 0u) && ((*int_clhd_reg & VDC_INT_BIT_S0_LO_VSYNC) != 0u))
    {
        /* Clear */
        *int_clhd_reg = IntState & ~VDC_INT_BIT_S0_LO_VSYNC;

        if (vdc_int_callback[VDC_INT_TYPE_S0_LO_VSYNC] != 0)
        {
            vdc_int_callback[VDC_INT_TYPE_S0_LO_VSYNC](VDC_INT_TYPE_S0_LO_VSYNC);
        }
        /* Set */
        *int_clhd_reg = IntState;
    }
}   /* End of function VDC_Ch0_s0_lo_vsync_ISR() */

/**************************************************************************//**
 * @brief       VDC S0_VSYNCERR interrupt service routine
 * @param[in]   int_sense
 * @retval      None
 *****************************************************************************/
static void VDC_Ch0_s0_vsyncerr_ISR (const uint32_t int_sense)
{
    uint32_t            IntState;
    volatile uint32_t * int_clhd_reg;
    volatile uint32_t * int_onoff_reg;

    UNUSED_PARAM(int_sense);

    int_clhd_reg  = vdc_regaddr_system_ctrl.syscnt_int1;
    int_onoff_reg = vdc_regaddr_system_ctrl.syscnt_int4;
    IntState      = *int_onoff_reg;

    if (((IntState & VDC_INT_BIT_S0_VSYNCERR) != 0u) && ((*int_clhd_reg & VDC_INT_BIT_S0_VSYNCERR) != 0u))
    {
        /* Clear */
        *int_clhd_reg = IntState & ~VDC_INT_BIT_S0_VSYNCERR;

        if (vdc_int_callback[VDC_INT_TYPE_S0_VSYNCERR] != 0)
        {
            vdc_int_callback[VDC_INT_TYPE_S0_VSYNCERR](VDC_INT_TYPE_S0_VSYNCERR);
        }
        /* Set */
        *int_clhd_reg = IntState;
    }
}   /* End of function VDC_Ch0_s0_vsyncerr_ISR() */

/**************************************************************************//**
 * @brief       VDC VLINE interrupt service routine
 * @param[in]   int_sense
 * @retval      None
 *****************************************************************************/
static void VDC_Ch0_vline_ISR (const uint32_t int_sense)
{
    uint32_t            IntState;
    volatile uint32_t * int_clhd_reg;
    volatile uint32_t * int_onoff_reg;

    UNUSED_PARAM(int_sense);

    int_clhd_reg  = vdc_regaddr_system_ctrl.syscnt_int1;
    int_onoff_reg = vdc_regaddr_system_ctrl.syscnt_int4;
    IntState      = *int_onoff_reg;

    if (((IntState & VDC_INT_BIT_VLINE) != 0u) && ((*int_clhd_reg & VDC_INT_BIT_VLINE) != 0u))
    {
        /* Clear */
        *int_clhd_reg = IntState & ~VDC_INT_BIT_VLINE;

        if (vdc_int_callback[VDC_INT_TYPE_VLINE] != 0)
        {
            vdc_int_callback[VDC_INT_TYPE_VLINE](VDC_INT_TYPE_VLINE);
        }
        /* Set */
        *int_clhd_reg = IntState;
    }
}   /* End of function VDC_Ch0_vline_ISR() */

/**************************************************************************//**
 * @brief       VDC S0_VFIELD interrupt service routine
 * @param[in]   int_sense
 * @retval      None
 *****************************************************************************/
static void VDC_Ch0_s0_vfield_ISR (const uint32_t int_sense)
{
    uint32_t            IntState;
    volatile uint32_t * int_clhd_reg;
    volatile uint32_t * int_onoff_reg;

    UNUSED_PARAM(int_sense);

    int_clhd_reg  = vdc_regaddr_system_ctrl.syscnt_int1;
    int_onoff_reg = vdc_regaddr_system_ctrl.syscnt_int4;
    IntState      = *int_onoff_reg;

    if (((IntState & VDC_INT_BIT_S0_VFIELD) != 0u) && ((*int_clhd_reg & VDC_INT_BIT_S0_VFIELD) != 0u))
    {
        /* Clear */
        *int_clhd_reg = IntState & ~VDC_INT_BIT_S0_VFIELD;

        if (vdc_int_callback[VDC_INT_TYPE_S0_VFIELD] != 0)
        {
            vdc_int_callback[VDC_INT_TYPE_S0_VFIELD](VDC_INT_TYPE_S0_VFIELD);
        }
        /* Set */
        *int_clhd_reg = IntState;
    }
}   /* End of function VDC_Ch0_s0_vfield_ISR() */

/**************************************************************************//**
 * @brief       VDC IV1_VBUFERR interrupt service routine
 * @param[in]   int_sense
 * @retval      None
 *****************************************************************************/
static void VDC_Ch0_iv1_vbuferr_ISR (const uint32_t int_sense)
{
    uint32_t            IntState;
    volatile uint32_t * int_clhd_reg;
    volatile uint32_t * int_onoff_reg;

    UNUSED_PARAM(int_sense);

    int_clhd_reg  = vdc_regaddr_system_ctrl.syscnt_int1;
    int_onoff_reg = vdc_regaddr_system_ctrl.syscnt_int4;
    IntState      = *int_onoff_reg;

    if (((IntState & VDC_INT_BIT_IV1_VBUFERR) != 0u) && ((*int_clhd_reg & VDC_INT_BIT_IV1_VBUFERR) != 0u))
    {
        /* Clear */
        *int_clhd_reg = IntState & ~VDC_INT_BIT_IV1_VBUFERR;

        if (vdc_int_callback[VDC_INT_TYPE_IV1_VBUFERR] != 0)
        {
            vdc_int_callback[VDC_INT_TYPE_IV1_VBUFERR](VDC_INT_TYPE_IV1_VBUFERR);
        }
        /* Set */
        *int_clhd_reg = IntState;
    }
}   /* End of function VDC_Ch0_iv1_vbuferr_ISR() */

/**************************************************************************//**
 * @brief       VDC IV3_VBUFERR interrupt service routine
 * @param[in]   int_sense
 * @retval      None
 *****************************************************************************/
static void VDC_Ch0_iv3_vbuferr_ISR (const uint32_t int_sense)
{
    uint32_t            IntState;
    volatile uint32_t * int_clhd_reg;
    volatile uint32_t * int_onoff_reg;

    UNUSED_PARAM(int_sense);

    int_clhd_reg  = vdc_regaddr_system_ctrl.syscnt_int1;
    int_onoff_reg = vdc_regaddr_system_ctrl.syscnt_int4;
    IntState      = *int_onoff_reg;

    if (((IntState & VDC_INT_BIT_IV3_VBUFERR) != 0u) && ((*int_clhd_reg & VDC_INT_BIT_IV3_VBUFERR) != 0u))
    {
        /* Clear */
        *int_clhd_reg = IntState & ~VDC_INT_BIT_IV3_VBUFERR;

        if (vdc_int_callback[VDC_INT_TYPE_IV3_VBUFERR] != 0)
        {
            vdc_int_callback[VDC_INT_TYPE_IV3_VBUFERR](VDC_INT_TYPE_IV3_VBUFERR);
        }
        /* Set */
        *int_clhd_reg = IntState;
    }
}   /* End of function VDC_Ch0_iv3_vbuferr_ISR() */

/**************************************************************************//**
 * @brief       VDC IV5_VBUFERR interrupt service routine
 * @param[in]   int_sense
 * @retval      None
 *****************************************************************************/
static void VDC_Ch0_iv5_vbuferr_ISR (const uint32_t int_sense)
{
    uint32_t            IntState;
    volatile uint32_t * int_clhd_reg;
    volatile uint32_t * int_onoff_reg;

    UNUSED_PARAM(int_sense);

    int_clhd_reg  = vdc_regaddr_system_ctrl.syscnt_int1;
    int_onoff_reg = vdc_regaddr_system_ctrl.syscnt_int4;
    IntState      = *int_onoff_reg;

    if (((IntState & VDC_INT_BIT_IV5_VBUFERR) != 0u) && ((*int_clhd_reg & VDC_INT_BIT_IV5_VBUFERR) != 0u))
    {
        /* Clear */
        *int_clhd_reg = IntState & ~VDC_INT_BIT_IV5_VBUFERR;

        if (vdc_int_callback[VDC_INT_TYPE_IV5_VBUFERR] != 0)
        {
            vdc_int_callback[VDC_INT_TYPE_IV5_VBUFERR](VDC_INT_TYPE_IV5_VBUFERR);
        }
        /* Set */
        *int_clhd_reg = IntState;
    }
}   /* End of function VDC_Ch0_iv5_vbuferr_ISR() */

/**************************************************************************//**
 * @brief       VDC IV6_VBUFERR interrupt service routine
 * @param[in]   int_sense
 * @retval      None
 *****************************************************************************/
static void VDC_Ch0_iv6_vbuferr_ISR (const uint32_t int_sense)
{
    uint32_t            IntState;
    volatile uint32_t * int_clhd_reg;
    volatile uint32_t * int_onoff_reg;

    UNUSED_PARAM(int_sense);

    int_clhd_reg  = vdc_regaddr_system_ctrl.syscnt_int2;
    int_onoff_reg = vdc_regaddr_system_ctrl.syscnt_int5;
    IntState      = *int_onoff_reg;

    if (((IntState & VDC_INT_BIT_IV6_VBUFERR) != 0u) && ((*int_clhd_reg & VDC_INT_BIT_IV6_VBUFERR) != 0u))
    {
        /* Clear */
        *int_clhd_reg = IntState & ~VDC_INT_BIT_IV6_VBUFERR;

        if (vdc_int_callback[VDC_INT_TYPE_IV6_VBUFERR] != 0)
        {
            vdc_int_callback[VDC_INT_TYPE_IV6_VBUFERR](VDC_INT_TYPE_IV6_VBUFERR);
        }
        /* Set */
        *int_clhd_reg = IntState;
    }
}   /* End of function VDC_Ch0_iv6_vbuferr_ISR() */

/**************************************************************************//**
 * @brief       VDC S0_WLINE interrupt service routine
 * @param[in]   int_sense
 * @retval      None
 *****************************************************************************/
static void VDC_Ch0_s0_wline_ISR (const uint32_t int_sense)
{
    uint32_t            IntState;
    volatile uint32_t * int_clhd_reg;
    volatile uint32_t * int_onoff_reg;

    UNUSED_PARAM(int_sense);

    int_clhd_reg  = vdc_regaddr_system_ctrl.syscnt_int2;
    int_onoff_reg = vdc_regaddr_system_ctrl.syscnt_int5;
    IntState      = *int_onoff_reg;

    if (((IntState & VDC_INT_BIT_S0_WLINE) != 0u) && ((*int_clhd_reg & VDC_INT_BIT_S0_WLINE) != 0u))
    {
        /* Clear */
        *int_clhd_reg = IntState & ~VDC_INT_BIT_S0_WLINE;

        if (vdc_int_callback[VDC_INT_TYPE_S0_WLINE] != 0)
        {
            vdc_int_callback[VDC_INT_TYPE_S0_WLINE](VDC_INT_TYPE_S0_WLINE);
        }
        /* Set */
        *int_clhd_reg = IntState;
    }
}   /* End of function VDC_Ch0_s0_wline_ISR() */

