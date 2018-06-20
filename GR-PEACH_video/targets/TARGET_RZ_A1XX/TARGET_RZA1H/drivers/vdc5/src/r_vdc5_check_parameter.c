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
* @file         r_vdc5_check_parameter.c
* @version      1.00
* $Rev: 199 $
* $Date:: 2014-05-23 16:33:52 +0900#$
* @brief        VDC5 driver parameter check processing
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_vdc5.h"
#include    "r_vdc5_user.h"
#include    "r_vdc5_shared_param.h"
#include    "r_vdc5_check_parameter.h"


#ifdef      R_VDC5_CHECK_PARAMETERS
/******************************************************************************
Macro definitions
******************************************************************************/
/* Valid bit range */
#define VDC5_CPARA_RANGE_0X00000001 (0x00000001u)
#define VDC5_CPARA_RANGE_0X00000003 (0x00000003u)
#define VDC5_CPARA_RANGE_0X00000007 (0x00000007u)
#define VDC5_CPARA_RANGE_0X0000000F (0x0000000Fu)
#define VDC5_CPARA_RANGE_0X0000001F (0x0000001Fu)
#define VDC5_CPARA_RANGE_0X0000003F (0x0000003Fu)
#define VDC5_CPARA_RANGE_0X0000007F (0x0000007Fu)
#define VDC5_CPARA_RANGE_0X000000FF (0x000000FFu)
#define VDC5_CPARA_RANGE_0X000003FF (0x000003FFu)
#define VDC5_CPARA_RANGE_0X000007FF (0x000007FFu)
#define VDC5_CPARA_RANGE_0X00007FFF (0x00007FFFu)
#define VDC5_CPARA_RANGE_0X0000FFFF (0x0000FFFFu)
#define VDC5_CPARA_RANGE_0X007FFFFF (0x007FFFFFu)
#define VDC5_CPARA_RANGE_0X00FFFFFF (0x00FFFFFFu)
/* Valid range for signal delay amount */
#define VDC5_CPARA_INP_FLD_DLY_MAX (254u)
#define VDC5_CPARA_INP_VS_DLY_MAX  (254u)
#define VDC5_CPARA_INP_HS_DLY_MAX  (254u)
/* Valid range for NFS */
#define VDC5_CPARA_LVDSPLL_FD_LIMIT_56 (56u)
#define VDC5_CPARA_LVDSPLL_FD_LIMIT_24 (24u)
#define VDC5_CPARA_LVDSPLL_FD_LIMIT_28 (28u)
#define VDC5_CPARA_LVDSPLL_FD_LIMIT_31 (31u)
#define VDC5_CPARA_LVDSPLL_FD_LIMIT_37 (37u)
#define VDC5_CPARA_LVDSPLL_FD_LIMIT_39 (39u)
#define VDC5_CPARA_LVDSPLL_FD_LIMIT_46 (46u)
#define VDC5_CPARA_LVDSPLL_FD_LIMIT_47 (47u)
#define VDC5_CPARA_LVDSPLL_FD_LIMIT_55 (55u)
/* Valid range for HS + HW */
#define VDC5_CPARA_HS_HW_MAX       (2015u)
/* Valid range for HS */
#define VDC5_CPARA_HS_MIN          (16u)
/* Valid range for VS + VW */
#define VDC5_CPARA_VS_VW_MAX       (2039u)
/* Valid range for VS */
#define VDC5_CPARA_VS_MIN          (4u)
/* The max number of CLUT data values */
#define VDC5_CPARA_CLUT8_NUM       (256u)
#define VDC5_CPARA_CLUT4_NUM       (16u)
#define VDC5_CPARA_CLUT1_NUM       (2u)
/* Valid range for the alpha coefficient */
#define VDC5_CPARA_GR_ARC_COEF_MIN (-255)
#define VDC5_CPARA_GR_ARC_COEF_MAX (255)
/* Valid range for HS of enhancer area */
#define VDC5_CPARA_ENH_HS_LIMIT    (4u)
/* Valid range for VS of enhancer area */
#define VDC5_CPARA_ENH_VS_LIMIT    (2u)

#define VDC5_BKSTR_T2_PROHIBITED    (31u)

#define VDC5_MULTIPLE_4_MASK        (0x00000003u)
#define VDC5_MULTIPLE_32_MASK       (0x0000001Fu)
#define VDC5_MULTIPLE_128_MASK      (0x0000007Fu)


/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static vdc5_error_t CheckChannel(const vdc5_channel_t ch);
static vdc5_error_t CheckLayerId(const vdc5_layer_id_t layer_id);
static vdc5_error_t CheckGraphicsTypeLayerId(const vdc5_layer_id_t layer_id);
static vdc5_error_t CheckRect(const vdc5_period_rect_t * const rect);
static vdc5_error_t CheckTcon_Vertical(const vdc5_lcd_tcon_timing_t * const lcd_tcon_timing);
static vdc5_error_t CheckTcon_Horizontal(const vdc5_lcd_tcon_timing_t * const lcd_tcon_timing);
static vdc5_error_t CheckTcon_HorizontalPol(const vdc5_lcd_tcon_timing_t * const lcd_tcon_timing);
static vdc5_error_t CheckTcon_DataEnable(const vdc5_lcd_tcon_timing_t * const lcd_tcon_timing);
static vdc5_error_t CheckGammaCrct_GainAdjustment(const uint16_t * gam_gain);


/**************************************************************************//**
 * @brief       Checks on initialization parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Initialization parameter
 * @retval      Error code
 *****************************************************************************/
vdc5_error_t VDC5_InitializeCheckPrm (const vdc5_channel_t ch, const vdc5_init_t * const param)
{
    vdc5_error_t            error;
    const vdc5_lvds_t     * lvds_tmp;
    vdc5_resource_state_t   rsrc_state;
    vdc5_input_sel_t        inp_sel_tmp;
    vdc5_channel_t          lvds_vdc_sel_tmp;

    error = CheckChannel(ch);
    if (error != VDC5_OK) {
        goto END;
    }

    if (param == NULL) {
        error = VDC5_ERR_PARAM_NULL;
        goto END;
    }

    /* Panel clock select */
    if (param->panel_icksel >= VDC5_PANEL_ICKSEL_NUM) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }
    if ((param->panel_icksel != VDC5_PANEL_ICKSEL_LVDS) && (param->panel_icksel != VDC5_PANEL_ICKSEL_LVDS_DIV7)) {
        /* Clock frequency division ratio */
        if (param->panel_dcdr >= VDC5_PANEL_CLKDIV_NUM) {
            error = VDC5_ERR_PARAM_UNDEFINED;
            goto END;
        }
    }
    rsrc_state = VDC5_ShrdPrmGetResource(ch, VDC5_RESOURCE_VIDEO_IN);
    if (rsrc_state != VDC5_RESOURCE_ST_INVALID) {
        inp_sel_tmp = VDC5_ShrdPrmGetInputSelect(ch);
        if (((inp_sel_tmp == VDC5_INPUT_SEL_VDEC) && (param->panel_icksel == VDC5_PANEL_ICKSEL_IMG_DV)) ||
                ((inp_sel_tmp == VDC5_INPUT_SEL_EXT) && (param->panel_icksel == VDC5_PANEL_ICKSEL_IMG))) {
            error = VDC5_ERR_PARAM_CONDITION;
            goto END;
        }
    }

    /* LVDS-related parameter */
    lvds_tmp = param->lvds;
    if (lvds_tmp == NULL) {
        rsrc_state = VDC5_ShrdPrmGetResource(ch, VDC5_RESOURCE_LVDS_CLK);
        if (rsrc_state == VDC5_RESOURCE_ST_INVALID) {
            /* The LVDS PLL clock has not been set yet. */
            if ((param->panel_icksel == VDC5_PANEL_ICKSEL_LVDS) || (param->panel_icksel == VDC5_PANEL_ICKSEL_LVDS_DIV7)) {
                error = VDC5_ERR_PARAM_NULL;
                goto END;
            }
        } else {
            if (param->panel_icksel == VDC5_PANEL_ICKSEL_LVDS_DIV7) {
                lvds_vdc_sel_tmp = VDC5_ShrdPrmGetLvdsCh();
                if (ch != lvds_vdc_sel_tmp) {
                    /* Channel mismatch error */
                    error = VDC5_ERR_PARAM_CONDITION;
                    goto END;
                }
            }
        }
    } else {
        /* The clock input to frequency divider 1 */
        if (lvds_tmp->lvds_in_clk_sel >= VDC5_LVDS_INCLK_SEL_NUM) {
            error = VDC5_ERR_PARAM_UNDEFINED;
            goto END;
        }
        /* The frequency dividing value (NIDIV and NODIV) for frequency divider 1 and 2 */
        if ((lvds_tmp->lvds_idiv_set >= VDC5_LVDS_NDIV_NUM) || (lvds_tmp->lvds_odiv_set >= VDC5_LVDS_NDIV_NUM)) {
            error = VDC5_ERR_PARAM_UNDEFINED;
            goto END;
        }
        /* Internal parameter setting for LVDS PLL */
        if (((uint32_t)lvds_tmp->lvdspll_tst & (uint32_t)~VDC5_CPARA_RANGE_0X0000003F) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
        /* A channel in VDC5 whose data is to be output through the LVDS */
        if (lvds_tmp->lvds_vdc_sel >= VDC5_CHANNEL_NUM) {
            error = VDC5_ERR_PARAM_CHANNEL;
            goto END;
        }
        if (param->panel_icksel != VDC5_PANEL_ICKSEL_LVDS_DIV7) {
            /* When using the clock output from the LVDS PLL as the clock for the LVDS output interface,
               both channel number should match. */
            if (ch != lvds_tmp->lvds_vdc_sel) {
                /* Channel mismatch error */
                error = VDC5_ERR_PARAM_CONDITION;
                goto END;
            }
        }
        /* The frequency dividing value (NFD) for the feedback frequency */
        if (((uint32_t)lvds_tmp->lvdspll_fd & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
        if ((uint32_t)lvds_tmp->lvdspll_fd < (uint32_t)VDC5_CPARA_LVDSPLL_FD_LIMIT_56) {
            if ((uint32_t)lvds_tmp->lvdspll_fd < (uint32_t)VDC5_CPARA_LVDSPLL_FD_LIMIT_24) {
                error = VDC5_ERR_PARAM_EXCEED_RANGE;
                goto END;
            }
            if (((uint32_t)lvds_tmp->lvdspll_fd >= (uint32_t)VDC5_CPARA_LVDSPLL_FD_LIMIT_28) && ((uint32_t)lvds_tmp->lvdspll_fd <= (uint32_t)VDC5_CPARA_LVDSPLL_FD_LIMIT_31)) {
                error = VDC5_ERR_PARAM_EXCEED_RANGE;
                goto END;
            }
            if (((uint32_t)lvds_tmp->lvdspll_fd >= (uint32_t)VDC5_CPARA_LVDSPLL_FD_LIMIT_37) && ((uint32_t)lvds_tmp->lvdspll_fd <= (uint32_t)VDC5_CPARA_LVDSPLL_FD_LIMIT_39)) {
                error = VDC5_ERR_PARAM_EXCEED_RANGE;
                goto END;
            }
            if (((uint32_t)lvds_tmp->lvdspll_fd == (uint32_t)VDC5_CPARA_LVDSPLL_FD_LIMIT_46) ||
                    ((uint32_t)lvds_tmp->lvdspll_fd == (uint32_t)VDC5_CPARA_LVDSPLL_FD_LIMIT_47) ||
                    ((uint32_t)lvds_tmp->lvdspll_fd == (uint32_t)VDC5_CPARA_LVDSPLL_FD_LIMIT_55)) {
                error = VDC5_ERR_PARAM_EXCEED_RANGE;
                goto END;
            }
        }
        /* The frequency dividing value (NRD) for the input frequency */
        if (((uint32_t)lvds_tmp->lvdspll_rd & (uint32_t)~VDC5_CPARA_RANGE_0X0000001F) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
        /* The frequency dividing value (NOD) for the output frequency */
        if (lvds_tmp->lvdspll_od >= VDC5_LVDS_PLL_NOD_NUM) {
            error = VDC5_ERR_PARAM_UNDEFINED;
            goto END;
        }
    }
END:
    return error;
}   /* End of function VDC5_InitializeCheckPrm() */

/**************************************************************************//**
 * @brief       Checks on parameters in R_VDC5_Terminate
 * @param[in]   ch          : Channel
 * @retval      Error code
 *****************************************************************************/
vdc5_error_t VDC5_TerminateCheckPrm (const vdc5_channel_t ch)
{
    vdc5_error_t error;

    error = CheckChannel(ch);
    if (error != VDC5_OK) {
        goto END;
    }
END:
    return error;
}   /* End of function VDC5_TerminateCheckPrm() */

/**************************************************************************//**
 * @brief       Checks on video input setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Video input setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc5_error_t VDC5_VideoInputCheckPrm (const vdc5_channel_t ch, const vdc5_input_t * const param)
{
    vdc5_error_t                error;
    const vdc5_sync_delay_t   * dly_tmp;
    const vdc5_ext_in_sig_t   * ext_sig_tmp;
    vdc5_panel_clksel_t         panel_clksel;
    vdc5_resource_state_t       rsrc_state;

    error = CheckChannel(ch);
    if (error != VDC5_OK) {
        goto END;
    }

    if (param == NULL) {
        error = VDC5_ERR_PARAM_NULL;
        goto END;
    }

    rsrc_state = VDC5_ShrdPrmGetResource(ch, VDC5_RESOURCE_PANEL_CLK);
    if (rsrc_state != VDC5_RESOURCE_ST_INVALID) {
        panel_clksel = VDC5_ShrdPrmGetPanelClkSel(ch);
        if (param->inp_sel == VDC5_INPUT_SEL_VDEC) {
            /* Video decoder output signals */
            if (panel_clksel == VDC5_PANEL_ICKSEL_IMG_DV) {
                /* Video image clock, DV_CLK */
                error = VDC5_ERR_PARAM_CONDITION;
                goto END;
            }
        } else {
            /* Signals supplied via the external input pins */
            if (panel_clksel == VDC5_PANEL_ICKSEL_IMG) {
                /* Video image clock, VIDEO_X1 */
                error = VDC5_ERR_PARAM_CONDITION;
                goto END;
            }
        }
    }

    /* Vsync signal 1/2fH phase timing */
    if (((uint32_t)param->inp_fh50 & (uint32_t)~VDC5_CPARA_RANGE_0X000003FF) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
    /* Vsync signal 1/4fH phase timing */
    if (((uint32_t)param->inp_fh25 & (uint32_t)~VDC5_CPARA_RANGE_0X000003FF) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Sync signal delay adjustment */
    dly_tmp = param->dly;
    if (dly_tmp != NULL) {
        /* Number of lines for delaying Vsync signal and field differentiation signal */
        if (((uint32_t)dly_tmp->inp_vs_dly_l & (uint32_t)~VDC5_CPARA_RANGE_0X00000007) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
        /* Field differentiation signal delay amount */
        if (((uint32_t)dly_tmp->inp_fld_dly & (uint32_t)~VDC5_CPARA_RANGE_0X000000FF) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
        if ((uint32_t)dly_tmp->inp_fld_dly > (uint32_t)VDC5_CPARA_INP_FLD_DLY_MAX) {
            error = VDC5_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
        /* Vsync signal delay amount */
        if (((uint32_t)dly_tmp->inp_vs_dly & (uint32_t)~VDC5_CPARA_RANGE_0X000000FF) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
        if ((uint32_t)dly_tmp->inp_vs_dly > (uint32_t)VDC5_CPARA_INP_VS_DLY_MAX) {
            error = VDC5_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
        /* Hsync signal delay amount */
        if (((uint32_t)dly_tmp->inp_hs_dly & (uint32_t)~VDC5_CPARA_RANGE_0X000000FF) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
        if ((uint32_t)dly_tmp->inp_hs_dly > (uint32_t)VDC5_CPARA_INP_HS_DLY_MAX) {
            error = VDC5_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
    }

    /* External input signal */
    ext_sig_tmp = param->ext_sig;
    if (ext_sig_tmp == NULL) {
        if (param->inp_sel != VDC5_INPUT_SEL_VDEC) {
            error = VDC5_ERR_PARAM_NULL;
            goto END;
        }
    } else {
        /* External input format select */
        if (ext_sig_tmp->inp_format >= VDC5_EXTIN_FORMAT_NUM) {
            error = VDC5_ERR_PARAM_UNDEFINED;
            goto END;
        }
        /* Y/Cb/Y/Cr data string start timing to Hsync reference */
        if (ext_sig_tmp->inp_h_pos >= VDC5_EXTIN_H_POS_NUM) {
            error = VDC5_ERR_PARAM_UNDEFINED;
            goto END;
        }
        if ((ext_sig_tmp->inp_h_pos == VDC5_EXTIN_H_POS_YCRYCB) || (ext_sig_tmp->inp_h_pos == VDC5_EXTIN_H_POS_CRYCBY)) {
            if (ext_sig_tmp->inp_format == VDC5_EXTIN_FORMAT_YCBCR422) {
                error = VDC5_ERR_PARAM_CONDITION;
                goto END;
            }
        }
    }
END:
    return error;
}   /* End of function VDC5_VideoInputCheckPrm() */

/**************************************************************************//**
 * @brief       Checks on sync control parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Sync control parameter
 * @retval      Error code
 *****************************************************************************/
vdc5_error_t VDC5_SyncControlCheckPrm (const vdc5_channel_t ch, const vdc5_sync_ctrl_t * const param)
{
    vdc5_error_t error;

    error = CheckChannel(ch);
    if (error != VDC5_OK) {
        goto END;
    }

    if (param == NULL) {
        error = VDC5_ERR_PARAM_NULL;
        goto END;
    }

    if (param->res_vs_sel != VDC5_OFF) {
        /* Free-running Vsync period setting */
        if (((uint32_t)param->res_fv & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
    }
    /* Hsync period setting */
    if (((uint32_t)param->res_fh & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
    /* Vsync signal delay control */
    if (((uint32_t)param->res_vsdly & (uint32_t)~VDC5_CPARA_RANGE_0X000000FF) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Full-screen enable control */
    error = CheckRect(&param->res_f);
    if (error != VDC5_OK) {
        goto END;
    }
END:
    return error;
}   /* End of function VDC5_SyncControlCheckPrm() */

/**************************************************************************//**
 * @brief       Checks on display output configuration parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Display output configuration parameter
 * @retval      Error code
 *****************************************************************************/
vdc5_error_t VDC5_DisplayOutputCheckPrm (const vdc5_channel_t ch, const vdc5_output_t * const param)
{
    vdc5_error_t                    error;
    uint32_t                        tcon_sig_index;
    static vdc5_error_t    (* const tcon_function_tbl[VDC5_LCD_TCONSIG_NUM])(
        const vdc5_lcd_tcon_timing_t * const lcd_tcon_timing) = {
        &CheckTcon_Vertical,        /* STVA/VS */
        &CheckTcon_Vertical,        /* STVB/VE */
        &CheckTcon_Horizontal,      /* STH/HS */
        &CheckTcon_Horizontal,      /* STB/HE */
        &CheckTcon_Horizontal,      /* CPV/GCK */
        &CheckTcon_HorizontalPol,   /* POLA */
        &CheckTcon_HorizontalPol,   /* POLB */
        &CheckTcon_DataEnable       /* DE */
    };

    error = CheckChannel(ch);
    if (error != VDC5_OK) {
        goto END;
    }

    if (param == NULL) {
        error = VDC5_ERR_PARAM_NULL;
        goto END;
    }

    /* 1/2fH timing */
    if (((uint32_t)param->tcon_half & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
    /* Offset Hsync signal timing */
    if (((uint32_t)param->tcon_offset & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* LCD TCON timing setting */
    for (tcon_sig_index = 0; tcon_sig_index < VDC5_LCD_TCONSIG_NUM; tcon_sig_index++) {
        if (param->outctrl[tcon_sig_index] != NULL) {
            error = tcon_function_tbl[tcon_sig_index](param->outctrl[tcon_sig_index]);
            if (error != VDC5_OK) {
                goto END;
            }
        }
    }

    /* LCD output format select */
    if (param->out_format >= VDC5_LCD_OUTFORMAT_NUM) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }
    if (param->out_format == VDC5_LCD_OUTFORMAT_SERIAL_RGB) {
        /* Clock frequency control */
        if (param->out_frq_sel >= VDC5_LCD_SERIAL_CLKFRQ_NUM) {
            error = VDC5_ERR_PARAM_UNDEFINED;
            goto END;
        }
        if (param->out_frq_sel == VDC5_LCD_PARALLEL_CLKFRQ_1) {
            error = VDC5_ERR_PARAM_CONDITION;
            goto END;
        }
        /* Clock phase adjustment */
        if (param->out_phase >= VDC5_LCD_SERIAL_CLKPHASE_NUM) {
            error = VDC5_ERR_PARAM_UNDEFINED;
            goto END;
        }
        if (param->out_frq_sel == VDC5_LCD_SERIAL_CLKFRQ_3) {
            if (param->out_phase == VDC5_LCD_SERIAL_CLKPHASE_3) {
                error = VDC5_ERR_PARAM_CONDITION;
                goto END;
            }
        }
    }
    /* Background color in 24-bit RGB color format */
    if ((param->bg_color & (uint32_t)~VDC5_CPARA_RANGE_0X00FFFFFF) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
END:
    return error;
}   /* End of function VDC5_DisplayOutputCheckPrm() */

/**************************************************************************//**
 * @brief       Checks on interrupt callback setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Interrupt callback setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc5_error_t VDC5_CallbackISRCheckPrm (const vdc5_channel_t ch, const vdc5_int_t * const param)
{
    vdc5_error_t error;

    error = CheckChannel(ch);
    if (error != VDC5_OK) {
        goto END;
    }

    if (param == NULL) {
        error = VDC5_ERR_PARAM_NULL;
        goto END;
    }

    /* VDC5 interrupt type */
    if (param->type >= VDC5_INT_TYPE_NUM) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }
    /* Interrupt callback function pointer */
    if (param->callback != 0) {
        if ((param->type == VDC5_INT_TYPE_VLINE) ||
                (param->type == VDC5_INT_TYPE_S0_WLINE) ||
                (param->type == VDC5_INT_TYPE_S1_WLINE) ||
                (param->type == VDC5_INT_TYPE_OIR_VLINE)) {
            /* Line interrupt set */
            if (((uint32_t)param->line_num & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
                error = VDC5_ERR_PARAM_BIT_WIDTH;
                goto END;
            }
        }
    }
END:
    return error;
}   /* End of function VDC5_CallbackISRCheckPrm() */

/**************************************************************************//**
 * @brief       Checks on data write control parameter
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   param       : Data write control parameter
 * @retval      Error code
 *****************************************************************************/
vdc5_error_t VDC5_WriteDataControlCheckPrm (
    const vdc5_channel_t        ch,
    const vdc5_layer_id_t       layer_id,
    const vdc5_write_t  * const param)
{
    vdc5_error_t                    error;
    const vdc5_scalingdown_rot_t  * scldw_rt_tmp;

    error = CheckChannel(ch);
    if (error != VDC5_OK) {
        goto END;
    }
    if ((layer_id != VDC5_LAYER_ID_0_WR) && (layer_id != VDC5_LAYER_ID_1_WR) && (layer_id != VDC5_LAYER_ID_OIR_WR)) {
        error = VDC5_ERR_PARAM_LAYER_ID;
        goto END;
    }

    if (param == NULL) {
        error = VDC5_ERR_PARAM_NULL;
        goto END;
    }

    /* Scaling-down and rotation parameter */
    scldw_rt_tmp = &param->scalingdown_rot;
    /* Image area to be captured */
    error = CheckRect(&scldw_rt_tmp->res);
    if (error != VDC5_OK) {
        goto END;
    }
    /* Number of valid lines in vertical direction output by scaling-down control block */
    if (((uint32_t)scldw_rt_tmp->res_out_vw & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
    if (((uint32_t)scldw_rt_tmp->res_out_vw & (uint32_t)VDC5_MULTIPLE_4_MASK) != 0u) {
        error = VDC5_ERR_PARAM_EXCEED_RANGE;
        goto END;
    }
    /* Number of valid horizontal pixels output by scaling-down control block */
    if (((uint32_t)scldw_rt_tmp->res_out_hw & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
    if (((uint32_t)scldw_rt_tmp->res_out_hw & (uint32_t)VDC5_MULTIPLE_4_MASK) != 0u) {
        error = VDC5_ERR_PARAM_EXCEED_RANGE;
        goto END;
    }
    /* Frame buffer writing mode for image processing */
    if (scldw_rt_tmp->res_ds_wr_md >= VDC5_WR_MD_NUM) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }

    /* Swap setting in frame buffer writing */
    if (param->res_wrswa >= VDC5_WR_RD_WRSWA_NUM) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }
    /* Frame buffer video-signal writing format */
    if (param->res_md >= VDC5_RES_MD_NUM) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }
    /* Writing rate */
    if (param->res_fs_rate >= VDC5_RES_FS_RATE_NUM) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }
    /* Frame buffer base address */
    if (param->base == NULL) {
        error = VDC5_ERR_PARAM_NULL;
        goto END;
    }
    /* Frame buffer line offset address [byte] */
    if ((param->ln_off & (uint32_t)~VDC5_CPARA_RANGE_0X00007FFF) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
    if (param->res_bst_md == VDC5_BST_MD_32BYTE) {
        /* 32-byte transfer */
        if ((param->ln_off & (uint32_t)VDC5_MULTIPLE_32_MASK) != 0u) {
            error = VDC5_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
    } else {
        /* 128-byte transfer */
        if ((param->ln_off & (uint32_t)VDC5_MULTIPLE_128_MASK) != 0u) {
            error = VDC5_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
    }
    /* Number of frames of buffer to be written to (res_flm_num + 1) */
    if ((param->flm_num & (uint32_t)~VDC5_CPARA_RANGE_0X000003FF) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
    /* Frame buffer frame offset address [byte] */
    if (param->flm_num != 0u) {
        if ((param->flm_off & (uint32_t)~VDC5_CPARA_RANGE_0X007FFFFF) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
        if (param->res_bst_md == VDC5_BST_MD_32BYTE) {
            /* 32-byte transfer */
            if ((param->flm_off & (uint32_t)VDC5_MULTIPLE_32_MASK) != 0u) {
                error = VDC5_ERR_PARAM_EXCEED_RANGE;
                goto END;
            }
        } else {
            /* 128-byte transfer */
            if ((param->flm_off & (uint32_t)VDC5_MULTIPLE_128_MASK) != 0u) {
                error = VDC5_ERR_PARAM_EXCEED_RANGE;
                goto END;
            }
        }
    }
END:
    return error;
}   /* End of function VDC5_WriteDataControlCheckPrm() */

/**************************************************************************//**
 * @brief       Checks on data write change parameter
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   param       : Data write change parameter
 * @retval      Error code
 *****************************************************************************/
vdc5_error_t VDC5_ChangeWriteProcessCheckPrm (
    const vdc5_channel_t            ch,
    const vdc5_layer_id_t           layer_id,
    const vdc5_write_chg_t  * const param)
{
    vdc5_error_t                    error;
    const vdc5_scalingdown_rot_t  * scldw_rt_tmp;

    error = CheckChannel(ch);
    if (error != VDC5_OK) {
        goto END;
    }
    if ((layer_id != VDC5_LAYER_ID_0_WR) && (layer_id != VDC5_LAYER_ID_1_WR) && (layer_id != VDC5_LAYER_ID_OIR_WR)) {
        error = VDC5_ERR_PARAM_LAYER_ID;
        goto END;
    }

    if (param == NULL) {
        error = VDC5_ERR_PARAM_NULL;
        goto END;
    }

    /* Scaling-down and rotation parameter */
    scldw_rt_tmp = &param->scalingdown_rot;
    /* Image area to be captured */
    error = CheckRect(&scldw_rt_tmp->res);
    if (error != VDC5_OK) {
        goto END;
    }
    /* Number of valid lines in vertical direction output by scaling-down control block */
    if (((uint32_t)scldw_rt_tmp->res_out_vw & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
    if (((uint32_t)scldw_rt_tmp->res_out_vw & (uint32_t)VDC5_MULTIPLE_4_MASK) != 0u) {
        error = VDC5_ERR_PARAM_EXCEED_RANGE;
        goto END;
    }
    /* Number of valid horizontal pixels output by scaling-down control block */
    if (((uint32_t)scldw_rt_tmp->res_out_hw & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
    if (((uint32_t)scldw_rt_tmp->res_out_hw & (uint32_t)VDC5_MULTIPLE_4_MASK) != 0u) {
        error = VDC5_ERR_PARAM_EXCEED_RANGE;
        goto END;
    }
    /* Frame buffer writing mode */
    if (scldw_rt_tmp->res_ds_wr_md >= VDC5_WR_MD_NUM) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }
END:
    return error;
}   /* End of function VDC5_ChangeWriteProcessCheckPrm() */

/**************************************************************************//**
 * @brief       Checks on data read control parameter
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   graphics_id : Graphics type ID
 * @param[in]   param       : Data read control parameter
 * @retval      Error code
 *****************************************************************************/
vdc5_error_t VDC5_ReadDataControlCheckPrm (
    const vdc5_channel_t        ch,
    const vdc5_layer_id_t       layer_id,
    const vdc5_graphics_type_t  graphics_id,
    const vdc5_read_t   * const param)
{
    vdc5_error_t            error;
    void                  * btm_base_tmp;
    vdc5_resource_state_t   rsrc_state;

    error = CheckChannel(ch);
    if (error != VDC5_OK) {
        goto END;
    }
    error = CheckGraphicsTypeLayerId(layer_id);
    if (error != VDC5_OK) {
        goto END;
    }

    if (param == NULL) {
        error = VDC5_ERR_PARAM_NULL;
        goto END;
    }

    /* Frame buffer address setting signal */
    if (param->gr_flm_sel >= VDC5_GR_FLM_SEL_NUM) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }
    btm_base_tmp    = NULL;
    rsrc_state      = VDC5_RESOURCE_ST_INVALID;
    switch (graphics_id) {
        case VDC5_GR_TYPE_GR0:
            btm_base_tmp    = VDC5_ShrdPrmGetFrBuffBtm(ch, VDC5_SC_TYPE_SC0);
            rsrc_state      = VDC5_ShrdPrmGetLayerResource(ch, VDC5_LAYER_ID_0_WR);
            break;
        case VDC5_GR_TYPE_GR1:
            if (param->gr_flm_sel == VDC5_GR_FLM_SEL_DISTORTION) {
                /* IMR-LS2 is implemented in scaler 0 only. */
                error = VDC5_ERR_PARAM_CONDITION;
                goto END;
            }
            btm_base_tmp    = VDC5_ShrdPrmGetFrBuffBtm(ch, VDC5_SC_TYPE_SC1);
            rsrc_state      = VDC5_ShrdPrmGetLayerResource(ch, VDC5_LAYER_ID_1_WR);
            break;
        /* The following two cases are intentionally combined. */
        case VDC5_GR_TYPE_GR2:
        case VDC5_GR_TYPE_GR3:
            if (param->gr_flm_sel != VDC5_GR_FLM_SEL_FLM_NUM) {
                error = VDC5_ERR_PARAM_CONDITION;
                goto END;
            }
            break;
        case VDC5_GR_TYPE_OIR:
            if (param->gr_flm_sel == VDC5_GR_FLM_SEL_POINTER_BUFF) {
                error = VDC5_ERR_PARAM_CONDITION;
                goto END;
            }
            if ((ch != VDC5_CHANNEL_0) && (param->gr_flm_sel == VDC5_GR_FLM_SEL_DISTORTION)) {
                /* IMR-LSD is implemented in channel 0 only. */
                error = VDC5_ERR_PARAM_CONDITION;
                goto END;
            }
            break;
        default:
            /* DO NOTHING */
            break;
    }
    if ((btm_base_tmp != NULL) && (rsrc_state != VDC5_RESOURCE_ST_INVALID)) {
        /* btm_base_tmp is valid only when rsrc_state is valid. */
        if (param->gr_flm_sel == VDC5_GR_FLM_SEL_SCALE_DOWN) {
            error = VDC5_ERR_PARAM_CONDITION;
            goto END;
        }
    }
    /* Frame buffer base address */
    if (param->gr_base == NULL) {
        if (param->gr_flm_sel != VDC5_GR_FLM_SEL_POINTER_BUFF) {
            error = VDC5_ERR_PARAM_NULL;
            goto END;
        }
    }
    /* Frame buffer line offset address */
    if ((param->gr_ln_off & (uint32_t)~VDC5_CPARA_RANGE_0X00007FFF) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
    if (param->gr_bst_md == VDC5_BST_MD_32BYTE) {
        /* 32-byte transfer */
        if ((param->gr_ln_off & (uint32_t)VDC5_MULTIPLE_32_MASK) != 0u) {
            error = VDC5_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
    } else {
        /* 128-byte transfer */
        if ((param->gr_ln_off & (uint32_t)VDC5_MULTIPLE_128_MASK) != 0u) {
            error = VDC5_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
    }
    if (param->width_read_fb != NULL) {
        /* Number of lines in a frame */
        if (((uint32_t)param->width_read_fb->in_vw & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
        if ((uint32_t)param->width_read_fb->in_vw == 0u) {
            error = VDC5_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
        /* Width of the horizontal valid period */
        if (((uint32_t)param->width_read_fb->in_hw & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
        if ((uint32_t)param->width_read_fb->in_hw < (uint32_t)VDC5_CPARA_RANGE_0X00000003) {
            error = VDC5_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
    }
    /* Format of the frame buffer read signal */
    if (param->gr_format >= VDC5_GR_FORMAT_NUM) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }
    if ((param->gr_format == VDC5_GR_FORMAT_YCBCR422) || (param->gr_format == VDC5_GR_FORMAT_YCBCR444)) {
        /* YCbCr422 and YCbCr444 are supported for the graphics 0 and 1. */
        if ((graphics_id != VDC5_GR_TYPE_GR0) && (graphics_id != VDC5_GR_TYPE_GR1)) {
            error = VDC5_ERR_PARAM_CONDITION;
            goto END;
        }
    }
    if (param->gr_format == VDC5_GR_FORMAT_YCBCR422) {
        /* Swapping of data read from buffer in the YCbCr422 format */
        if (param->gr_ycc_swap >= VDC5_GR_YCCSWAP_NUM) {
            error = VDC5_ERR_PARAM_UNDEFINED;
            goto END;
        }
    }
    /* Swap setting in frame buffer reading */
    if (param->gr_rdswa >= VDC5_WR_RD_WRSWA_NUM) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }
    /* Graphics display area */
    error = CheckRect(&param->gr_grc);
    if (error != VDC5_OK) {
        goto END;
    }
END:
    return error;
}   /* End of function VDC5_ReadDataControlCheckPrm() */

/**************************************************************************//**
 * @brief       Checks on data read change parameter
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   param       : Data read change parameter
 * @retval      Error code
 *****************************************************************************/
vdc5_error_t VDC5_ChangeReadProcessCheckPrm (
    const vdc5_channel_t            ch,
    const vdc5_layer_id_t           layer_id,
    const vdc5_read_chg_t   * const param)
{
    vdc5_error_t error;

    error = CheckChannel(ch);
    if (error != VDC5_OK) {
        goto END;
    }
    error = CheckGraphicsTypeLayerId(layer_id);
    if (error != VDC5_OK) {
        goto END;
    }

    if (param == NULL) {
        error = VDC5_ERR_PARAM_NULL;
        goto END;
    }
    /* Width of the image read from frame buffer */
    if (param->width_read_fb != NULL) {
        /* Number of lines in a frame */
        if (((uint32_t)param->width_read_fb->in_vw & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
        if ((uint32_t)param->width_read_fb->in_vw == 0u) {
            error = VDC5_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
        /* Width of the horizontal valid period */
        if (((uint32_t)param->width_read_fb->in_hw & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
        if ((uint32_t)param->width_read_fb->in_hw < (uint32_t)VDC5_CPARA_RANGE_0X00000003) {
            error = VDC5_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
    }
    /* Graphics display area */
    if (param->gr_grc != NULL) {
        error = CheckRect(param->gr_grc);
        if (error != VDC5_OK) {
            goto END;
        }
    }
    /* Graphics display mode */
    if (param->gr_disp_sel != NULL) {
        if ((*(param->gr_disp_sel) < VDC5_DISPSEL_IGNORED) || (*(param->gr_disp_sel) >= VDC5_DISPSEL_NUM)) {
            error = VDC5_ERR_PARAM_UNDEFINED;
            goto END;
        }
    }
END:
    return error;
}   /* End of function VDC5_ChangeReadProcessCheckPrm() */

/**************************************************************************//**
 * @brief       Checks on data write/read start parameter
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   param       : Data write/read start parameter
 * @retval      Error code
 *****************************************************************************/
vdc5_error_t VDC5_StartProcessCheckPrm (
    const vdc5_channel_t        ch,
    const vdc5_layer_id_t       layer_id,
    const vdc5_start_t  * const param)
{
    vdc5_error_t                error;
    uint32_t                    graphics_types;
    const vdc5_gr_disp_sel_t  * gr_disp_sel_tmp;

    error = CheckChannel(ch);
    if (error != VDC5_OK) {
        goto END;
    }
    error = CheckLayerId(layer_id);
    if (error != VDC5_OK) {
        goto END;
    }

    if (param == NULL) {
        error = VDC5_ERR_PARAM_NULL;
        goto END;
    }

    gr_disp_sel_tmp = param->gr_disp_sel;
    if (gr_disp_sel_tmp == NULL) {
        error = VDC5_OK;
        goto END;
    }
    /* Graphics display mode */
    if (layer_id == VDC5_LAYER_ID_ALL) {
        for (graphics_types = 0; graphics_types < VDC5_GR_TYPE_NUM; graphics_types++) {
            if ((gr_disp_sel_tmp[graphics_types] < VDC5_DISPSEL_IGNORED) ||
                    (gr_disp_sel_tmp[graphics_types] >= VDC5_DISPSEL_NUM)) {
                error = VDC5_ERR_PARAM_UNDEFINED;
                goto END;
            }
        }
    } else {
        if ((*gr_disp_sel_tmp < VDC5_DISPSEL_IGNORED) || (*gr_disp_sel_tmp >= VDC5_DISPSEL_NUM)) {
            error = VDC5_ERR_PARAM_UNDEFINED;
            goto END;
        }
    }
END:
    return error;
}   /* End of function VDC5_StartProcessCheckPrm() */

/**************************************************************************//**
 * @brief       Checks on parameters in R_VDC5_StopProcess
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @retval      Error code
 *****************************************************************************/
vdc5_error_t VDC5_StopProcessCheckPrm (const vdc5_channel_t ch, const vdc5_layer_id_t layer_id)
{
    vdc5_error_t error;

    error = CheckChannel(ch);
    if (error != VDC5_OK) {
        goto END;
    }
    error = CheckLayerId(layer_id);
    if (error != VDC5_OK) {
        goto END;
    }
END:
    return error;
}   /* End of function VDC5_StopProcessCheckPrm() */

/**************************************************************************//**
 * @brief       Checks on parameters in R_VDC5_ReleaseDataControl
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @retval      Error code
 *****************************************************************************/
vdc5_error_t VDC5_ReleaseDataControlCheckPrm (const vdc5_channel_t ch, const vdc5_layer_id_t layer_id)
{
    vdc5_error_t error;

    error = CheckChannel(ch);
    if (error != VDC5_OK) {
        goto END;
    }
    error = CheckLayerId(layer_id);
    if (error != VDC5_OK) {
        goto END;
    }
END:
    return error;
}   /* End of function VDC5_ReleaseDataControlCheckPrm() */

/**************************************************************************//**
 * @brief       Checks on noise reduction setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Noise reduction setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc5_error_t VDC5_VideoNoiseReductCheckPrm (const vdc5_channel_t ch, const vdc5_noise_reduction_t * const param)
{
    vdc5_error_t            error;
    const vdc5_nr_param_t * nr_param;

    error = CheckChannel(ch);
    if (error != VDC5_OK) {
        goto END;
    }

    if (param == NULL) {
        error = VDC5_OK;
        goto END;
    }

    /* Y/G signal noise reduction parameter */
    nr_param = &param->y;
    /* TAP select */
    if (nr_param->nr1d_tap >= VDC5_NR_TAPSEL_NUM) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }
    /* Maximum value of coring (absolute value) */
    if ((nr_param->nr1d_th & (uint32_t)~VDC5_CPARA_RANGE_0X0000007F) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
    /* Noise reduction gain adjustment */
    if (nr_param->nr1d_gain >= VDC5_NR_GAIN_NUM) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }

    /* Cb/B signal noise reduction parameter */
    nr_param = &param->cb;
    /* TAP select */
    if (nr_param->nr1d_tap >= VDC5_NR_TAPSEL_NUM) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }
    /* Maximum value of coring (absolute value) */
    if ((nr_param->nr1d_th & (uint32_t)~VDC5_CPARA_RANGE_0X0000007F) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
    /* Noise reduction gain adjustment */
    if (nr_param->nr1d_gain >= VDC5_NR_GAIN_NUM) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }

    /* Cr/R signal noise reduction parameter */
    nr_param = &param->cr;
    /* TAP select */
    if (nr_param->nr1d_tap >= VDC5_NR_TAPSEL_NUM) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }
    /* Maximum value of coring (absolute value) */
    if ((nr_param->nr1d_th & (uint32_t)~VDC5_CPARA_RANGE_0X0000007F) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
    /* Noise reduction gain adjustment */
    if (nr_param->nr1d_gain >= VDC5_NR_GAIN_NUM) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }
END:
    return error;
}   /* End of function VDC5_VideoNoiseReductCheckPrm() */

/**************************************************************************//**
 * @brief       Checks on color matrix setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Color matrix setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc5_error_t VDC5_ImageColorMatrixCheckPrm (const vdc5_channel_t ch, const vdc5_color_matrix_t * const param)
{
    vdc5_error_t    error;
    int32_t         colormtx_index;

    error = CheckChannel(ch);
    if (error != VDC5_OK) {
        goto END;
    }

    if (param == NULL) {
        error = VDC5_ERR_PARAM_NULL;
        goto END;
    }

    /* Color matrix module */
    if (param->module >= VDC5_COLORMTX_NUM) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }
    /* Operating mode */
    if (param->mtx_mode >= VDC5_COLORMTX_MODE_NUM) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }
    if (param->module != VDC5_COLORMTX_IMGCNT) {
        if ((param->mtx_mode == VDC5_COLORMTX_GBR_YCBCR) || (param->mtx_mode == VDC5_COLORMTX_YCBCR_YCBCR)) {
            error = VDC5_ERR_PARAM_CONDITION;
            goto END;
        }
    }
    /* Offset (DC) adjustment of Y/G, B, and R signal */
    for (colormtx_index = 0; colormtx_index < VDC5_COLORMTX_OFFST_NUM; colormtx_index++) {
        if (((uint32_t)param->offset[colormtx_index] & (uint32_t)~VDC5_CPARA_RANGE_0X000000FF) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
    }
    /* GG, GB, GR, BG, BB, BR, RG, RB, and RR signal gain adjustment */
    for (colormtx_index = 0; colormtx_index < VDC5_COLORMTX_GAIN_NUM; colormtx_index++) {
        if (((uint32_t)param->gain[colormtx_index] & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
    }
END:
    return error;
}   /* End of function VDC5_ImageColorMatrixCheckPrm() */

/**************************************************************************//**
 * @brief       Checks on sharpness setup parameter and enhancer-enabled area setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   imgimprv_id : Image quality improver ID
 * @param[in]   sharp_param : Sharpness setup parameter
 * @param[in]   enh_area    : Enhancer-enabled area setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc5_error_t VDC5_ImageEnhancementCheckPrm (
    const vdc5_channel_t                ch,
    const vdc5_imgimprv_id_t            imgimprv_id,
    const vdc5_enhance_sharp_t  * const sharp_param,
    const vdc5_period_rect_t    * const enh_area)
{
    vdc5_error_t error;

    error = CheckChannel(ch);
    if (error != VDC5_OK) {
        goto END;
    }

    /* Image quality improver ID */
    if (imgimprv_id >= VDC5_IMG_IMPRV_NUM) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }

    /* Sharpness parameter */
    if (sharp_param != NULL) {
        /* Sharpness control parameter (H1, H2, and H3) */
        if (((uint32_t)sharp_param->hrz_sharp[VDC5_IMGENH_SHARP_H1].shp_core & (uint32_t)~VDC5_CPARA_RANGE_0X0000007F) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
        if (((uint32_t)sharp_param->hrz_sharp[VDC5_IMGENH_SHARP_H2].shp_core & (uint32_t)~VDC5_CPARA_RANGE_0X0000007F) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
        if (((uint32_t)sharp_param->hrz_sharp[VDC5_IMGENH_SHARP_H3].shp_core & (uint32_t)~VDC5_CPARA_RANGE_0X0000007F) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
    }
    /* Enhancer area */
    if (enh_area != NULL) {
        /* Vertical start position */
        if (((uint32_t)enh_area->vs & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
        if ((uint32_t)enh_area->vs < (uint32_t)VDC5_CPARA_ENH_VS_LIMIT) {
            error = VDC5_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
        /* Height (vertical width) */
        if (((uint32_t)enh_area->vw & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
        /* Horizontal start position */
        if (((uint32_t)enh_area->hs & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
        if ((uint32_t)enh_area->hs < (uint32_t)VDC5_CPARA_ENH_HS_LIMIT) {
            error = VDC5_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
        /* Horizontal width */
        if (((uint32_t)enh_area->hw & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
    }
END:
    return error;
}   /* End of function VDC5_ImageEnhancementCheckPrm() */

/**************************************************************************//**
 * @brief       Checks on black stretch setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   imgimprv_id : Image quality improver ID
 * @param[in]   param       : Black stretch setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc5_error_t VDC5_ImageBlackStretchCheckPrm (
    const vdc5_channel_t        ch,
    const vdc5_imgimprv_id_t    imgimprv_id,
    const vdc5_black_t  * const param)
{
    vdc5_error_t error;

    error = CheckChannel(ch);
    if (error != VDC5_OK) {
        goto END;
    }

    /* Image quality improver ID */
    if (imgimprv_id >= VDC5_IMG_IMPRV_NUM) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }

    if (param != NULL) {
        /* Black stretch start point */
        if (((uint32_t)param->bkstr_st & (uint32_t)~VDC5_CPARA_RANGE_0X0000000F) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
        /* Black stretch depth */
        if (((uint32_t)param->bkstr_d & (uint32_t)~VDC5_CPARA_RANGE_0X0000000F) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
        /* Black stretch time constant (T1) */
        if (((uint32_t)param->bkstr_t1 & (uint32_t)~VDC5_CPARA_RANGE_0X0000001F) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
        /* Black stretch time constant (T2) */
        if (((uint32_t)param->bkstr_t2 & (uint32_t)~VDC5_CPARA_RANGE_0X0000001F) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
        if ((uint32_t)param->bkstr_t2 == VDC5_BKSTR_T2_PROHIBITED) {
            error = VDC5_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }

    }
END:
    return error;
}   /* End of function VDC5_ImageBlackStretchCheckPrm() */

/**************************************************************************//**
 * @brief       Checks on alpha blending setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   param       : Alpha blending setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc5_error_t VDC5_AlphaBlendingCheckPrm (
    const vdc5_channel_t                ch,
    const vdc5_layer_id_t               layer_id,
    const vdc5_alpha_blending_t * const param)
{
    vdc5_error_t error;

    error = CheckChannel(ch);
    if (error != VDC5_OK) {
        goto END;
    }
    if ((layer_id != VDC5_LAYER_ID_1_RD) && (layer_id != VDC5_LAYER_ID_2_RD) && (layer_id != VDC5_LAYER_ID_3_RD)) {
        error = VDC5_ERR_PARAM_LAYER_ID;
        goto END;
    }

    if (param == NULL) {
        error = VDC5_ERR_PARAM_NULL;
        goto END;
    }
END:
    return error;
}   /* End of function VDC5_AlphaBlendingCheckPrm() */

/**************************************************************************//**
 * @brief       Checks on setup parameter for alpha blending in a rectangular area
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   param       : Setup parameter for alpha blending in a rectangular area
 * @retval      Error code
 *****************************************************************************/
vdc5_error_t VDC5_AlphaBlendingRectCheckPrm (
    const vdc5_channel_t                        ch,
    const vdc5_layer_id_t                       layer_id,
    const vdc5_alpha_blending_rect_t    * const param)
{
    vdc5_error_t error;

    error = CheckChannel(ch);
    if (error != VDC5_OK) {
        goto END;
    }
    if ((layer_id != VDC5_LAYER_ID_1_RD) &&
            (layer_id != VDC5_LAYER_ID_2_RD) &&
            (layer_id != VDC5_LAYER_ID_3_RD) &&
            (layer_id != VDC5_LAYER_ID_VIN_RD)) {
        error = VDC5_ERR_PARAM_LAYER_ID;
        goto END;
    }

    if (param != NULL) {
        /* Rectangular area subjected to alpha blending */
        if (param->gr_arc != NULL) {
            /* Vertical start position */
            if (((uint32_t)param->gr_arc->vs_rel & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
                error = VDC5_ERR_PARAM_BIT_WIDTH;
                goto END;
            }
            /* Height (vertical width) */
            if (((uint32_t)param->gr_arc->vw_rel & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
                error = VDC5_ERR_PARAM_BIT_WIDTH;
                goto END;
            }
            /* Horizontal start position */
            if (((uint32_t)param->gr_arc->hs_rel & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
                error = VDC5_ERR_PARAM_BIT_WIDTH;
                goto END;
            }
            /* Horizontal width */
            if (((uint32_t)param->gr_arc->hw_rel & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
                error = VDC5_ERR_PARAM_BIT_WIDTH;
                goto END;
            }
        }
        /* Alpha Blending in a rectangular area */
        if (param->alpha_rect != NULL) {
            /* Frame rate for alpha blending in a rectangular area */
            if ((param->alpha_rect->gr_arc_coef < (int16_t)VDC5_CPARA_GR_ARC_COEF_MIN) || (param->alpha_rect->gr_arc_coef > (int16_t)VDC5_CPARA_GR_ARC_COEF_MAX)) {
                error = VDC5_ERR_PARAM_EXCEED_RANGE;
                goto END;
            }
        }
    }
END:
    return error;
}   /* End of function VDC5_AlphaBlendingRectCheckPrm() */

/**************************************************************************//**
 * @brief       Checks on chroma-key setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   graphics_id : Graphics type ID
 * @param[in]   param       : Chroma-key setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc5_error_t VDC5_ChromakeyCheckPrm (
    const vdc5_channel_t            ch,
    const vdc5_layer_id_t           layer_id,
    const vdc5_graphics_type_t      graphics_id,
    const vdc5_chromakey_t  * const param)
{
    vdc5_error_t        error;
    vdc5_gr_format_t    gr_format_tmp;

    error = CheckChannel(ch);
    if (error != VDC5_OK) {
        goto END;
    }
    error = CheckGraphicsTypeLayerId(layer_id);
    if (error != VDC5_OK) {
        goto END;
    }

    if (param != NULL) {
        gr_format_tmp = VDC5_ShrdPrmGetGraphicsFormat(ch, graphics_id);
        switch (gr_format_tmp) {
            /* The following four cases are intentionally combined,
              since the four types of graphics format are represented as 16 bit unsigned integer. */
            case VDC5_GR_FORMAT_RGB565:
            case VDC5_GR_FORMAT_ARGB1555:
            case VDC5_GR_FORMAT_ARGB4444:
            case VDC5_GR_FORMAT_RGBA5551:
                if (((param->ck_color & (uint32_t)~VDC5_CPARA_RANGE_0X0000FFFF) != 0u) ||
                        ((param->rep_color & (uint32_t)~VDC5_CPARA_RANGE_0X0000FFFF) != 0u)) {
                    error = VDC5_ERR_PARAM_BIT_WIDTH;
                }
                break;
            case VDC5_GR_FORMAT_RGB888:
                if (((param->ck_color & (uint32_t)~VDC5_CPARA_RANGE_0X00FFFFFF) != 0u) ||
                        ((param->rep_color & (uint32_t)~VDC5_CPARA_RANGE_0X00FFFFFF) != 0u)) {
                    error = VDC5_ERR_PARAM_BIT_WIDTH;
                }
                break;
            case VDC5_GR_FORMAT_CLUT8:
                if ((param->ck_color & (uint32_t)~VDC5_CPARA_RANGE_0X000000FF) != 0u) {
                    error = VDC5_ERR_PARAM_BIT_WIDTH;
                }
                break;
            case VDC5_GR_FORMAT_CLUT4:
                if ((param->ck_color & (uint32_t)~VDC5_CPARA_RANGE_0X0000000F) != 0u) {
                    error = VDC5_ERR_PARAM_BIT_WIDTH;
                }
                break;
            case VDC5_GR_FORMAT_CLUT1:
                if ((param->ck_color & (uint32_t)~VDC5_CPARA_RANGE_0X00000001) != 0u) {
                    error = VDC5_ERR_PARAM_BIT_WIDTH;
                }
                break;
            default:
                /* DO NOTHING */
                break;
        }
    }
END:
    return error;
}   /* End of function VDC5_ChromakeyCheckPrm() */

/**************************************************************************//**
 * @brief       Checks on CLUT setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   graphics_id : Graphics type ID
 * @param[in]   param       : CLUT setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc5_error_t VDC5_CLUTCheckPrm (
    const vdc5_channel_t        ch,
    const vdc5_layer_id_t       layer_id,
    const vdc5_graphics_type_t  graphics_id,
    const vdc5_clut_t   * const param)
{
    vdc5_error_t        error;
    vdc5_gr_format_t    gr_format_tmp;

    error = CheckChannel(ch);
    if (error != VDC5_OK) {
        goto END;
    }
    error = CheckGraphicsTypeLayerId(layer_id);
    if (error != VDC5_OK) {
        goto END;
    }

    if (param == NULL) {
        error = VDC5_ERR_PARAM_NULL;
        goto END;
    }

    /* The number of colors in CLUT */
    if (param->color_num == 0u) {
        error = VDC5_ERR_PARAM_EXCEED_RANGE;
        goto END;
    }
    gr_format_tmp = VDC5_ShrdPrmGetGraphicsFormat(ch, graphics_id);
    if (gr_format_tmp == VDC5_GR_FORMAT_CLUT1) {
        /* CLUT1 */
        if (param->color_num > (uint32_t)VDC5_CPARA_CLUT1_NUM) {
            error = VDC5_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
    } else if (gr_format_tmp == VDC5_GR_FORMAT_CLUT4) {
        /* CLUT4 */
        if (param->color_num > (uint32_t)VDC5_CPARA_CLUT4_NUM) {
            error = VDC5_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
    } else {
        /* CLUT8 */
        if (param->color_num > (uint32_t)VDC5_CPARA_CLUT8_NUM) {
            error = VDC5_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
    }

    /* Address of the area storing the CLUT data (in ARGB8888 format) */
    if (param->clut == NULL) {
        error = VDC5_ERR_PARAM_NULL;
        goto END;
    }
END:
    return error;
}   /* End of function VDC5_CLUTCheckPrm() */

/**************************************************************************//**
 * @brief       Checks on display calibration parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Display calibration parameter
 * @retval      Error code
 *****************************************************************************/
vdc5_error_t VDC5_DisplayCalibrationCheckPrm (const vdc5_channel_t ch, const vdc5_disp_calibration_t * const param)
{
    vdc5_error_t                    error;
    const vdc5_calibr_bright_t    * bright_tmp;
    const vdc5_calibr_dither_t    * panel_dither_tmp;

    error = CheckChannel(ch);
    if (error != VDC5_OK) {
        goto END;
    }

    if (param == NULL) {
        error = VDC5_ERR_PARAM_NULL;
        goto END;
    }

    /* Brightness (DC) adjustment */
    bright_tmp = param->bright;
    if (bright_tmp != NULL) {
        /* Brightness (DC) adjustment of G signal */
        if (((uint32_t)bright_tmp->pbrt_g & (uint32_t)~VDC5_CPARA_RANGE_0X000003FF) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
        /* Brightness (DC) adjustment of B signal */
        if (((uint32_t)bright_tmp->pbrt_b & (uint32_t)~VDC5_CPARA_RANGE_0X000003FF) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
        /* Brightness (DC) adjustment of R signal */
        if (((uint32_t)bright_tmp->pbrt_r & (uint32_t)~VDC5_CPARA_RANGE_0X000003FF) != 0u) {
            error = VDC5_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
    }
    /* Panel dithering */
    panel_dither_tmp = param->panel_dither;
    if (panel_dither_tmp != NULL) {
        /* Panel dither operation mode */
        if (panel_dither_tmp->pdth_sel >= VDC5_PDTH_MD_NUM) {
            error = VDC5_ERR_PARAM_UNDEFINED;
            goto END;
        }
        if (panel_dither_tmp->pdth_sel == VDC5_PDTH_MD_2X2) {
            /* Pattern value (A)-(D) of 2x2 pattern dither */
            if (((uint32_t)panel_dither_tmp->pdth_pa & (uint32_t)~VDC5_CPARA_RANGE_0X00000003) != 0u) {
                error = VDC5_ERR_PARAM_BIT_WIDTH;
                goto END;
            }
            if (((uint32_t)panel_dither_tmp->pdth_pb & (uint32_t)~VDC5_CPARA_RANGE_0X00000003) != 0u) {
                error = VDC5_ERR_PARAM_BIT_WIDTH;
                goto END;
            }
            if (((uint32_t)panel_dither_tmp->pdth_pc & (uint32_t)~VDC5_CPARA_RANGE_0X00000003) != 0u) {
                error = VDC5_ERR_PARAM_BIT_WIDTH;
                goto END;
            }
            if (((uint32_t)panel_dither_tmp->pdth_pd & (uint32_t)~VDC5_CPARA_RANGE_0X00000003) != 0u) {
                error = VDC5_ERR_PARAM_BIT_WIDTH;
                goto END;
            }
        }
    }
END:
    return error;
}   /* End of function VDC5_DisplayCalibrationCheckPrm() */

/**************************************************************************//**
 * @brief       Checks on gamma correction setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Gamma correction setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc5_error_t VDC5_GammaCorrectionCheckPrm (const vdc5_channel_t ch, const vdc5_gamma_correction_t * const param)
{
    vdc5_error_t error;

    error = CheckChannel(ch);
    if (error != VDC5_OK) {
        goto END;
    }

    if (param != NULL) {
        /* Gain adjustment of area 0 to 31 of G signal */
        error = CheckGammaCrct_GainAdjustment(param->gam_g_gain);
        if (error != VDC5_OK) {
            goto END;
        }
        /* Gain adjustment of area 0 to 31 of B signal */
        error = CheckGammaCrct_GainAdjustment(param->gam_b_gain);
        if (error != VDC5_OK) {
            goto END;
        }
        /* Gain adjustment of area 0 to 31 of R signal */
        error = CheckGammaCrct_GainAdjustment(param->gam_r_gain);
        if (error != VDC5_OK) {
            goto END;
        }
    }
END:
    return error;
}   /* End of function VDC5_GammaCorrectionCheckPrm() */

/******************************************************************************
Local Functions
******************************************************************************/
/**************************************************************************//**
 * @brief       Checks on VDC5 channel
 * @param[in]   ch          : Channel
 * @retval      Error code
******************************************************************************/
static vdc5_error_t CheckChannel (const vdc5_channel_t ch)
{
    vdc5_error_t error;

    error = VDC5_OK;

    if (ch >= VDC5_CHANNEL_NUM) {
        error = VDC5_ERR_PARAM_CHANNEL;
        goto END;
    }
END:
    return error;
}   /* End of function CheckChannel() */

/**************************************************************************//**
 * @brief       Checks on layer ID
 * @param[in]   layer_id        : Layer ID
 * @retval      Error code
******************************************************************************/
static vdc5_error_t CheckLayerId (const vdc5_layer_id_t layer_id)
{
    vdc5_error_t error;

    error = VDC5_OK;

    if ((layer_id < VDC5_LAYER_ID_ALL) || (layer_id >= VDC5_LAYER_ID_NUM)) {
        error = VDC5_ERR_PARAM_LAYER_ID;
        goto END;
    }
    if (layer_id == VDC5_LAYER_ID_VIN_RD) {
        error = VDC5_ERR_PARAM_LAYER_ID;
        goto END;
    }
END:
    return error;
}   /* End of function CheckLayerId() */

/**************************************************************************//**
 * @brief       Checks on layer ID related to graphics
 * @param[in]   layer_id        : Layer ID
 * @retval      Error code
******************************************************************************/
static vdc5_error_t CheckGraphicsTypeLayerId (const vdc5_layer_id_t layer_id)
{
    vdc5_error_t error;

    error = VDC5_OK;

    if ((layer_id < VDC5_LAYER_ID_0_RD) || (layer_id > VDC5_LAYER_ID_OIR_RD)) {
        error = VDC5_ERR_PARAM_LAYER_ID;
        goto END;
    }
    if (layer_id == VDC5_LAYER_ID_VIN_RD) {
        error = VDC5_ERR_PARAM_LAYER_ID;
        goto END;
    }
END:
    return error;
}   /* End of function CheckGraphicsTypeLayerId() */

/**************************************************************************//**
 * @brief       Checks on horizontal/vertical timing of the VDC5 signals
 * @param[in]   rect            : Horizontal/vertical timing of the VDC5 signals
 * @retval      Error code
******************************************************************************/
static vdc5_error_t CheckRect (const vdc5_period_rect_t * const rect)
{
    vdc5_error_t error;

    error = VDC5_OK;

    /* Vertical start position */
    if (((uint32_t)rect->vs & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
    if ((uint32_t)rect->vs < (uint32_t)VDC5_CPARA_VS_MIN) {
        error = VDC5_ERR_PARAM_EXCEED_RANGE;
        goto END;
    }
    /* Height (vertical width) */
    if (((uint32_t)rect->vw & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
    if (((uint32_t)rect->vs + (uint32_t)rect->vw) > (uint32_t)VDC5_CPARA_VS_VW_MAX) {
        error = VDC5_ERR_PARAM_EXCEED_RANGE;
        goto END;
    }

    /* Horizontal start position */
    if (((uint32_t)rect->hs & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
    if ((uint32_t)rect->hs < (uint32_t)VDC5_CPARA_HS_MIN) {
        error = VDC5_ERR_PARAM_EXCEED_RANGE;
        goto END;
    }
    /* Horizontal width */
    if (((uint32_t)rect->hw & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
    if (((uint32_t)rect->hs + (uint32_t)rect->hw) > (uint32_t)VDC5_CPARA_HS_HW_MAX) {
        error = VDC5_ERR_PARAM_EXCEED_RANGE;
        goto END;
    }
END:
    return error;
}   /* End of function CheckRect() */

/**************************************************************************//**
 * @brief       Checks on LCD TCON timing signal parameter for horizontal panel driver signal
 * @param[in]   lcd_tcon_timing : LCD TCON timing signal parameter
 * @retval      Error code
******************************************************************************/
static vdc5_error_t CheckTcon_Horizontal (const vdc5_lcd_tcon_timing_t * const lcd_tcon_timing)
{
    vdc5_error_t error;

    error = VDC5_OK;

    /* Signal pulse start position */
    if (((uint32_t)lcd_tcon_timing->tcon_hsvs & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
    /* Pulse width */
    if (((uint32_t)lcd_tcon_timing->tcon_hwvw & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
    /* LCD TCON output pin select */
    if ((lcd_tcon_timing->tcon_pin < VDC5_LCD_TCON_PIN_NON) || (lcd_tcon_timing->tcon_pin >= VDC5_LCD_TCON_PIN_NUM)) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }
END:
    return error;
}   /* End of function CheckTcon_Horizontal() */

/**************************************************************************//**
 * @brief       Checks on LCD TCON timing signal parameter for horizontal panel driver signal (POLA/POLB)
 * @param[in]   lcd_tcon_timing : LCD TCON timing signal parameter
 * @retval      Error code
******************************************************************************/
static vdc5_error_t CheckTcon_HorizontalPol (const vdc5_lcd_tcon_timing_t * const lcd_tcon_timing)
{
    vdc5_error_t error;

    error = VDC5_OK;

    /* Signal pulse start position */
    if (((uint32_t)lcd_tcon_timing->tcon_hsvs & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
    /* Pulse width */
    if (((uint32_t)lcd_tcon_timing->tcon_hwvw & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
    /* POLA/POLB signal generation mode select */
    if (lcd_tcon_timing->tcon_md >= VDC5_LCD_TCON_POLMD_NUM) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }
    /* LCD TCON output pin select */
    if ((lcd_tcon_timing->tcon_pin < VDC5_LCD_TCON_PIN_NON) || (lcd_tcon_timing->tcon_pin >= VDC5_LCD_TCON_PIN_NUM)) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }
END:
    return error;
}   /* End of function CheckTcon_HorizontalPol() */

/**************************************************************************//**
 * @brief       Checks on LCD TCON timing signal parameter for vertical panel driver signal
 * @param[in]   lcd_tcon_timing : LCD TCON timing signal parameter
 * @retval      Error code
******************************************************************************/
static vdc5_error_t CheckTcon_Vertical (const vdc5_lcd_tcon_timing_t * const lcd_tcon_timing)
{
    vdc5_error_t error;

    error = VDC5_OK;

    /* Signal pulse start position */
    if (((uint32_t)lcd_tcon_timing->tcon_hsvs & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
    /* Pulse width */
    if (((uint32_t)lcd_tcon_timing->tcon_hwvw & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
        error = VDC5_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
    /* LCD TCON output pin select */
    if ((lcd_tcon_timing->tcon_pin < VDC5_LCD_TCON_PIN_NON) || (lcd_tcon_timing->tcon_pin >= VDC5_LCD_TCON_PIN_NUM)) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }
END:
    return error;
}   /* End of function CheckTcon_Vertical() */

/**************************************************************************//**
 * @brief       Checks on LCD TCON timing signal parameter for DE timing signal
 * @param[in]   lcd_tcon_timing : LCD TCON timing signal parameter
 * @retval      Error code
******************************************************************************/
static vdc5_error_t CheckTcon_DataEnable (const vdc5_lcd_tcon_timing_t * const lcd_tcon_timing)
{
    vdc5_error_t error;

    error = VDC5_OK;

    /* LCD TCON output pin select */
    if ((lcd_tcon_timing->tcon_pin < VDC5_LCD_TCON_PIN_NON) || (lcd_tcon_timing->tcon_pin >= VDC5_LCD_TCON_PIN_NUM)) {
        error = VDC5_ERR_PARAM_UNDEFINED;
        goto END;
    }
END:
    return error;
}   /* End of function CheckTcon_DataEnable() */

/**************************************************************************//**
 * @brief       Checks on gain adjustment of area 0 to 31 of G/B/R signal
 * @param[in]   gam_gain        : Gain adjustment of area 0 to 31 of G/B/R signal
 * @retval      Error code
******************************************************************************/
static vdc5_error_t CheckGammaCrct_GainAdjustment (const uint16_t * gam_gain)
{
    uint32_t        gain_index;
    vdc5_error_t    error;

    error = VDC5_OK;

    if (gam_gain != NULL) {
        for (gain_index = 0; gain_index < VDC5_GAM_GAIN_ADJ_NUM; gain_index++) {
            if (((uint32_t)*gam_gain & (uint32_t)~VDC5_CPARA_RANGE_0X000007FF) != 0u) {
                error = VDC5_ERR_PARAM_BIT_WIDTH;
                goto END;
            }
            gam_gain++;
        }
    }
END:
    return error;
}   /* End of function CheckGammaCrct_GainAdjustment() */

#endif      /* R_VDC5_CHECK_PARAMETERS */

