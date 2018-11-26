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
* @file         r_vdc_shared_param.c
* @version      0.01
* $Rev: 18 $
* $Date:: 2014-05-29 11:11:38 +0900#$
* @brief        RZ/A2M VDC driver shared parameter processing
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    <string.h>

#include    "r_vdc_shared_param.h"
#include    "r_vdc.h"
#include    "r_vdc_user.h"


/******************************************************************************
Macro definitions
******************************************************************************/
/*! Two to the power of eleven */
#define     TWO_POWER_ELEVEN                (2048)

#define     VDC_SPARA_DIV_2                (2)
#define     VDC_SPARA_DIV_256              (256)

/* Color Conversion Value */
#define     VDC_SPARA_COLOR_CONV_RANGE     (255)
#define     VDC_SPARA_COLOR_CONV_OFFSET    (128)

/* Color mask data */
#define     VDC_SPARA_COLOR_8BIT_MASK      (0x000000FFu)

/* shift value */
#define     VDC_SPARA_SHIFT_16             (16u)
#define     VDC_SPARA_SHIFT_8              (8u)

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef struct
{
    vdc_panel_clksel_t     panel_icksel;   /*!< Panel clock select */
    vdc_onoff_t            lvds_ref;       /*!< Reference to the LVDS PLL clock */
    vdc_color_space_t      color_sp_in;    /*!< Color space of the input video image signal */
    uint32_t                bg_color_rgb;   /*!< Background color in 24-bit RGB color format */
    uint32_t                bg_color_crycb; /*!< Background color in 24-bit CrYCb format */
} vdc_shared_param_common_t;

typedef struct
{
    vdc_wr_md_t            res_ds_wr_md;   /*!< Frame buffer writing mode for image processing */
    vdc_res_inter_t        res_inter;      /*!< Field operating mode select */
    vdc_color_space_t      color_sp_fb_wr; /*!< Color space of the frame buffer writing format */
    void                  * btm_base;       /*!< Frame buffer base address for bottom */
    vdc_width_read_fb_t    width_wr_fb;    /*!< Size of the image output by scaling-down control block */
} vdc_shared_param_scaling_t;

typedef struct
{
    vdc_gr_ln_off_dir_t    gr_ln_off_dir;      /*!< Line offset address direction of the frame buffer */
    vdc_gr_flm_sel_t       gr_flm_sel;         /*!< Frame buffer address setting signal */
    vdc_gr_format_t        gr_format;          /*!< Format of the frame buffer read signal */
    vdc_color_space_t      color_sp_fb_rd;     /*!< Color space of the frame buffer reading format */
    vdc_onoff_t            adj_sel;            /*!< Folding handling (on/off) */
    vdc_period_rect_t      gr_grc;             /*!< Graphics display area */
    vdc_width_read_fb_t    width_read_fb;      /*!< Size of the frame buffer to be read */
} vdc_shared_param_graphics_t;

typedef struct
{
    vdc_resource_state_t   rsrc_panel_clock;
    vdc_resource_state_t   rsrc_video_input;
    vdc_resource_state_t   rsrc_vsync_signal;
    vdc_resource_state_t   rsrc_lcd_panel;
} vdc_shared_param_resource_t;

/*! Read/write process state */
typedef enum
{
    VDC_RW_PROC_STATE_DISABLE   = 0,
    VDC_RW_PROC_STATE_ENABLE    = 1
} vdc_rw_proc_state_t;


/******************************************************************************
Private global variables and functions
******************************************************************************/
static vdc_resource_state_t * GetResourceStatePointer(const vdc_resource_type_t rsrc_type);
static uint32_t SumProduct(
    const int32_t   red,
    const int32_t   green,
    const int32_t   blue,
    int32_t         coeff_r,
    int32_t         coeff_g,
    int32_t         coeff_b,
    const int32_t   offset);

static vdc_shared_param_common_t   param_common;
static vdc_shared_param_scaling_t  param_scaling;
static vdc_shared_param_graphics_t param_graphics[VDC_GR_TYPE_NUM];
/*! A channel in VDC whose data is to be output through the LVDS */
static vdc_channel_t               lvds_vdc_sel_ch;

static vdc_resource_state_t        layer_resource[VDC_LAYER_ID_NUM];
static vdc_rw_proc_state_t         rw_proc_state[VDC_LAYER_ID_NUM];

static vdc_onoff_t                 video_input_flag = VDC_OFF;


/**************************************************************************//**
 * @brief       Initializes variables of the VDC driver
 * @param[in]   void
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmInit (void)
{
    int32_t                         layer_id_num;
    vdc_shared_param_common_t    * shared_param_common;
    vdc_shared_param_scaling_t   * shared_param_scaling;
    vdc_shared_param_graphics_t  * shared_param_graphics;
    vdc_graphics_type_t            graphics_id;

    for (layer_id_num = 0; layer_id_num < VDC_LAYER_ID_NUM; layer_id_num++)
    {
        layer_resource[layer_id_num] = VDC_RESOURCE_ST_INVALID;
        rw_proc_state[layer_id_num]  = VDC_RW_PROC_STATE_DISABLE;
    }

    shared_param_common = &param_common;

    shared_param_common->panel_icksel   = VDC_PANEL_ICKSEL_PERI;   /* Panel clock select */
    shared_param_common->lvds_ref       = VDC_OFF;                 /* Reference to the LVDS PLL clock */
    shared_param_common->bg_color_rgb   = (uint32_t)0u;             /* Background color in 24-bit RGB color format */
    shared_param_common->bg_color_crycb = (uint32_t)0u;             /* Background color in 24-bit CrYCb format */

    if (video_input_flag == VDC_OFF)
    {
        shared_param_common->color_sp_in    = VDC_COLOR_SPACE_GBR; /* Color space of the input video image signal */
    }

    shared_param_scaling = &param_scaling;

    /* Frame buffer writing mode for image processing */
    shared_param_scaling->res_ds_wr_md      = VDC_WR_MD_NORMAL;
    shared_param_scaling->res_inter         = VDC_RES_INTER_PROGRESSIVE;   /* Field operating mode select */
    /* Color space of the frame buffer writing format */
    shared_param_scaling->color_sp_fb_wr    = VDC_COLOR_SPACE_GBR;
    /* Frame buffer base address for bottom */
    shared_param_scaling->btm_base          = NULL;
    /* Size of the image output by scaling-down control block */
    shared_param_scaling->width_wr_fb.in_vw = (uint16_t)0;
    shared_param_scaling->width_wr_fb.in_hw = (uint16_t)0;

    for (graphics_id = VDC_GR_TYPE_GR0; graphics_id < VDC_GR_TYPE_NUM; graphics_id++)
    {
        shared_param_graphics = &param_graphics[graphics_id];

        /* Line offset address direction of the frame buffer */
        shared_param_graphics->gr_ln_off_dir = VDC_GR_LN_OFF_DIR_INC;
        /* Frame buffer address setting signal */
        shared_param_graphics->gr_flm_sel = VDC_GR_FLM_SEL_FLM_NUM;
        /* Format of the frame buffer read signal */
        shared_param_graphics->gr_format = VDC_GR_FORMAT_RGB565;
        /* Color space of the frame buffer reading format */
        shared_param_graphics->color_sp_fb_rd = VDC_COLOR_SPACE_GBR;
        /* Folding handling (on/off) */
        shared_param_graphics->adj_sel = VDC_OFF;
        /* Graphics display area */
        shared_param_graphics->gr_grc.vs = (uint16_t)0;
        shared_param_graphics->gr_grc.vw = (uint16_t)0;
        shared_param_graphics->gr_grc.hs = (uint16_t)0;
        shared_param_graphics->gr_grc.hw = (uint16_t)0;
        /* Size of the frame buffer to be read */
        shared_param_graphics->width_read_fb.in_vw = (uint16_t)0;
        shared_param_graphics->width_read_fb.in_hw = (uint16_t)0;
    }
    /* A channel in VDC whose data is to be output through the LVDS */
    lvds_vdc_sel_ch = VDC_CHANNEL_NUM;     /* Initialized with invalid channel */
}   /* End of function VDC_ShrdPrmInit() */

/**************************************************************************//**
 * @brief       Sets the initialization parameter
 * @param[in]   param                   : Initialization parameter
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmSetInitParam (const vdc_init_t * const param)
{
    vdc_shared_param_common_t    * shared_param_common;

    shared_param_common = &param_common;

    /* Panel clock select */
    shared_param_common->panel_icksel = param->panel_icksel;
    /* Reference to the LVDS PLL clock */
    if ((param->panel_icksel == VDC_PANEL_ICKSEL_LVDS) || (param->panel_icksel == VDC_PANEL_ICKSEL_LVDS_DIV7))
    {
        shared_param_common->lvds_ref = VDC_ON;
    }
    if (param->lvds != NULL)
    {   /* A channel in VDC whose data is to be output through the LVDS */
        lvds_vdc_sel_ch = param->lvds->lvds_vdc_sel;
    }
}   /* End of function VDC_ShrdPrmSetInitParam() */

/**************************************************************************//**
 * @brief       Sets the termination parameter
 * @param[in]   None
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmSetTerminate (void)
{
    vdc_shared_param_common_t    * shared_param_common;

    shared_param_common = &param_common;

    /* Reference to the LVDS PLL clock */
    shared_param_common->lvds_ref = VDC_OFF;
}   /* End of function VDC_ShrdPrmSetTerminate() */

/**************************************************************************//**
 * @brief       Sets the video input setup parameter
 * @param[in]   param                   : Video input setup parameter
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmSetInputParam (const vdc_input_t * const param)
{
    vdc_shared_param_common_t    * shared_param_common;

    video_input_flag = VDC_ON;

    shared_param_common = &param_common;

    /* Color space */
    if ((param->ext_sig->inp_format == VDC_EXTIN_FORMAT_RGB888) ||
        (param->ext_sig->inp_format == VDC_EXTIN_FORMAT_RGB666) ||
        (param->ext_sig->inp_format == VDC_EXTIN_FORMAT_RGB565))
    {
        shared_param_common->color_sp_in = VDC_COLOR_SPACE_GBR;
    }
    else
    {
        shared_param_common->color_sp_in = VDC_COLOR_SPACE_YCBCR;
    }
}   /* End of function VDC_ShrdPrmSetInputParam() */

/**************************************************************************//**
 * @brief       Sets the display output configuration parameter
 * @param[in]   param                   : Display output configuration parameter
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmSetOutputParam (const vdc_output_t * const param)
{
    vdc_shared_param_common_t    * shared_param_common;
    int32_t                         red;
    int32_t                         green;
    int32_t                         blue;
    uint32_t                        y_val;
    uint32_t                        cb_val;
    uint32_t                        cr_val;

    shared_param_common = &param_common;

    /* Background color in 24-bit RGB color format */
    shared_param_common->bg_color_rgb = param->bg_color;

    red     = (int32_t)((param->bg_color >> VDC_SPARA_SHIFT_16) & (uint32_t)VDC_SPARA_COLOR_8BIT_MASK);
    green   = (int32_t)((param->bg_color >> VDC_SPARA_SHIFT_8)  & (uint32_t)VDC_SPARA_COLOR_8BIT_MASK);
    blue    = (int32_t)( param->bg_color                         & (uint32_t)VDC_SPARA_COLOR_8BIT_MASK);
    /* Y */
    y_val = SumProduct(
        red,
        green,
        blue,
        (int32_t)VDC_COLORCONV_Y_R,
        (int32_t)VDC_COLORCONV_Y_G,
        (int32_t)VDC_COLORCONV_Y_B,
        0);
    /* Cb */
    cb_val = SumProduct(
        red,
        green,
        blue,
        (int32_t)VDC_COLORCONV_CB_R,
        (int32_t)VDC_COLORCONV_CB_G,
        (int32_t)VDC_COLORCONV_CB_B,
        (int32_t)VDC_SPARA_COLOR_CONV_OFFSET);
    /* Cr */
    cr_val = SumProduct(
        red,
        green,
        blue,
        (int32_t)VDC_COLORCONV_CR_R,
        (int32_t)VDC_COLORCONV_CR_G,
        (int32_t)VDC_COLORCONV_CR_B,
        (int32_t)VDC_SPARA_COLOR_CONV_OFFSET);
    /* Background color in 24-bit CrYCb format */
    shared_param_common->bg_color_crycb = (uint32_t)((cr_val << VDC_SPARA_SHIFT_16) | (y_val << VDC_SPARA_SHIFT_8) | cb_val);
}   /* End of function VDC_ShrdPrmSetOutputParam() */

/**************************************************************************//**
 * @brief       Sets the data write control parameter
 * @param[in]   param                   : Data write control parameter
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmSetWriteParam (const vdc_write_t * const param)
{
    vdc_shared_param_scaling_t   * shared_param_scaling;

    shared_param_scaling = &param_scaling;

    /* Frame buffer writing mode for image processing */
    shared_param_scaling->res_ds_wr_md = param->scalingdown_rot.res_ds_wr_md;
    /* Field operating mode select */
    shared_param_scaling->res_inter = param->res_inter;

    /* Color space of the frame buffer */
    if ((param->res_md == VDC_RES_MD_YCBCR422) || (param->res_md == VDC_RES_MD_YCBCR444))
    {
        shared_param_scaling->color_sp_fb_wr = VDC_COLOR_SPACE_YCBCR;
    }
    else
    {
        shared_param_scaling->color_sp_fb_wr = VDC_COLOR_SPACE_GBR;
    }

    /* Frame buffer base address for bottom */
    shared_param_scaling->btm_base = param->btm_base;
    /* Size of the image output by scaling-down control block */
    shared_param_scaling->width_wr_fb.in_vw = param->scalingdown_rot.res_out_vw;
    shared_param_scaling->width_wr_fb.in_hw = param->scalingdown_rot.res_out_hw;
}   /* End of function VDC_ShrdPrmSetWriteParam() */

/**************************************************************************//**
 * @brief       Sets the data write change parameter
 * @param[in]   param                   : Data write change parameter
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmSetChgWriteParam (const vdc_write_chg_t * const param)
{
    vdc_shared_param_scaling_t   * shared_param_scaling;

    shared_param_scaling = &param_scaling;

    /* Frame buffer writing mode for image processing */
    shared_param_scaling->res_ds_wr_md = param->scalingdown_rot.res_ds_wr_md;
    /* Size of the image output by scaling-down control block */
    shared_param_scaling->width_wr_fb.in_vw = param->scalingdown_rot.res_out_vw;
    shared_param_scaling->width_wr_fb.in_hw = param->scalingdown_rot.res_out_hw;
}   /* End of function VDC_ShrdPrmSetChgWriteParam() */

/**************************************************************************//**
 * @brief       Sets the data read control parameter
 * @param[in]   graphics_id             : Graphics type ID
 * @param[in]   param                   : Data read control parameter
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmSetReadParam (const vdc_graphics_type_t graphics_id, const vdc_read_t * const param)
{
    vdc_shared_param_graphics_t  * shared_param_graphics;

    shared_param_graphics = &param_graphics[graphics_id];

    /* Line offset address direction of the frame buffer */
    shared_param_graphics->gr_ln_off_dir = param->gr_ln_off_dir;

    /* Frame buffer address setting signal */
    shared_param_graphics->gr_flm_sel = param->gr_flm_sel;

    /* Format of the frame buffer read signal */
    shared_param_graphics->gr_format = param->gr_format;

    /* Color space of the frame buffer */
    if ((param->gr_format == VDC_GR_FORMAT_YCBCR422) || (param->gr_format == VDC_GR_FORMAT_YCBCR444))
    {
        shared_param_graphics->color_sp_fb_rd = VDC_COLOR_SPACE_YCBCR;
    }
    else
    {
        shared_param_graphics->color_sp_fb_rd = VDC_COLOR_SPACE_GBR;
    }

    /* Folding handling (on/off) */
    shared_param_graphics->adj_sel = param->adj_sel;

    /* Graphics display area */
    shared_param_graphics->gr_grc = param->gr_grc;
    /* Size of the frame buffer to be read */
    if (param->width_read_fb != NULL)
    {
        shared_param_graphics->width_read_fb.in_vw = param->width_read_fb->in_vw;
        shared_param_graphics->width_read_fb.in_hw = param->width_read_fb->in_hw;
    }
    else
    {   /* If not specified, ... */
        if (param->gr_flm_sel == VDC_GR_FLM_SEL_FLM_NUM)
        {   /* The size of the frame buffer to be read is assumed that it is the same as
               the width of the graphics display area. */
            shared_param_graphics->width_read_fb.in_vw = param->gr_grc.vw;
            shared_param_graphics->width_read_fb.in_hw = param->gr_grc.hw;
        }
        else
        {   /* The size of the image output by scaling-down control block is used for the size of the frame buffer
               to be read when a frame buffer address setting signal links to the video image signals. */
            shared_param_graphics->width_read_fb = param_scaling.width_wr_fb;
        }
    }
}   /* End of function VDC_ShrdPrmSetReadParam() */

/**************************************************************************//**
 * @brief       Sets the data read change parameter
 * @param[in]   graphics_id             : Graphics type ID
 * @param[in]   param                   : Data read change parameter
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmSetChgReadParam (const vdc_graphics_type_t graphics_id, const vdc_read_chg_t * const param)
{
    vdc_shared_param_graphics_t  * shared_param_graphics;

    shared_param_graphics = &param_graphics[graphics_id];

    /* Graphics display area */
    if (param->gr_grc != NULL)
    {
        shared_param_graphics->gr_grc = *(param->gr_grc);
    }

    /* Size of the frame buffer to be read */
    if (param->width_read_fb != NULL)
    {
        shared_param_graphics->width_read_fb.in_vw = param->width_read_fb->in_vw;
        shared_param_graphics->width_read_fb.in_hw = param->width_read_fb->in_hw;
    }
}   /* End of function VDC_ShrdPrmSetChgReadParam() */

/**************************************************************************//**
 * @brief       Gets the reference to the LVDS PLL clock
 * @param[in]   void
 * @retval      Reference to the LVDS PLL clock
 *****************************************************************************/
vdc_onoff_t VDC_ShrdPrmGetLvdsClkRef (void)
{
    vdc_onoff_t onoff;

    if (param_common.lvds_ref == VDC_OFF)
    {
        onoff = VDC_OFF;
    }
    else
    {
        onoff = VDC_ON;
    }
    return onoff;
}   /* End of function VDC_ShrdPrmGetLvdsClkRef() */

/**************************************************************************//**
 * @brief       Gets the color space of the input video image signal
 * @param[in]   void
 * @retval      Color space of the input video image signal
 *****************************************************************************/
vdc_color_space_t VDC_ShrdPrmGetColorSpace (void)
{
    return param_common.color_sp_in;
}   /* End of function VDC_ShrdPrmGetColorSpace() */

/**************************************************************************//**
 * @brief       Get the background color
 * @param[in]   color_space             : Color space
 * @retval      Background color in 24-bit RGB color format or CrYCb format
 *****************************************************************************/
uint32_t VDC_ShrdPrmGetBgColor (const vdc_color_space_t color_space)
{
    return (color_space == VDC_COLOR_SPACE_GBR) ? param_common.bg_color_rgb : param_common.bg_color_crycb;
}   /* End of function VDC_ShrdPrmGetBgColor() */

/**************************************************************************//**
 * @brief       Get the frame buffer writing mode for image processing
 * @param[in]   void
 * @retval      Frame buffer writing mode for image processing
 *****************************************************************************/
vdc_wr_md_t VDC_ShrdPrmGetWritingMode (void)
{
    return param_scaling.res_ds_wr_md;
}   /* End of function VDC_ShrdPrmGetWritingMode() */

/**************************************************************************//**
 * @brief
 * @param[in]   void
 * @retval      Field operating mode select
 *****************************************************************************/
vdc_res_inter_t VDC_ShrdPrmGetInterlace (void)
{
    return param_scaling.res_inter;
}   /* End of function VDC_ShrdPrmGetInterlace() */

/**************************************************************************//**
 * @brief       Gets the color space of the frame buffer writing format
 * @param[in]   void
 * @retval      Color space of the frame buffer writing format
 *****************************************************************************/
vdc_color_space_t VDC_ShrdPrmGetColorSpaceFbWr (void)
{
    return param_scaling.color_sp_fb_wr;
}   /* End of function VDC_ShrdPrmGetColorSpaceFbWr() */

/**************************************************************************//**
 * @brief       Gets the frame buffer base address for bottom
 * @param[in]   void
 * @retval      Frame buffer base address for bottom
 *****************************************************************************/
void * VDC_ShrdPrmGetFrBuffBtm (void)
{
    return param_scaling.btm_base;
}   /* End of function VDC_ShrdPrmGetFrBuffBtm() */

/**************************************************************************//**
 * @brief       Gets the line offset address direction of the frame buffer
 * @param[in]   graphics_id             : Graphics type ID
 * @retval      Line offset address direction of the frame buffer
 *****************************************************************************/
vdc_gr_ln_off_dir_t VDC_ShrdPrmGetLineOfsAddrDir (const vdc_graphics_type_t graphics_id)
{
    return param_graphics[graphics_id].gr_ln_off_dir;
}   /* End of function VDC_ShrdPrmGetLineOfsAddrDir() */

/**************************************************************************//**
 * @brief       Gets the frame buffer address setting signal
 * @param[in]   graphics_id             : Graphics type ID
 * @retval      Frame buffer address setting signal
 *****************************************************************************/
vdc_gr_flm_sel_t VDC_ShrdPrmGetSelFbAddrSig (const vdc_graphics_type_t graphics_id)
{
    return param_graphics[graphics_id].gr_flm_sel;
}   /* End of function VDC_ShrdPrmGetSelFbAddrSig() */

/**************************************************************************//**
 * @brief       Gets the format of the frame buffer read signal
 * @param[in]   graphics_id             : Graphics type ID
 * @retval      Format of the frame buffer read signal
 *****************************************************************************/
vdc_gr_format_t VDC_ShrdPrmGetGraphicsFormat (const vdc_graphics_type_t graphics_id)
{
    return param_graphics[graphics_id].gr_format;
}   /* End of function VDC_ShrdPrmGetGraphicsFormat() */

/**************************************************************************//**
 * @brief       Gets the color space of the frame buffer reading format
 * @param[in]   graphics_id             : Graphics type ID
 * @retval      Color space of the frame buffer reading format
 *****************************************************************************/
vdc_color_space_t VDC_ShrdPrmGetColorSpaceFbRd (const vdc_graphics_type_t graphics_id)
{
    return param_graphics[graphics_id].color_sp_fb_rd;
}   /* End of function VDC_ShrdPrmGetColorSpaceFbRd() */

/**************************************************************************//**
 * @brief       Gets the folding handling
 * @param[in]   graphics_id             : Graphics type ID
 * @retval      Folding handling (on/off)
 *****************************************************************************/
vdc_onoff_t VDC_ShrdPrmGetMeasureFolding (const vdc_graphics_type_t graphics_id)
{
    return param_graphics[graphics_id].adj_sel;
}   /* End of function VDC_ShrdPrmGetMeasureFolding() */

/**************************************************************************//**
 * @brief       Gets the graphics display area
 * @param[in]   graphics_id             : Graphics type ID
 * @retval      Graphics display area
 *****************************************************************************/
vdc_period_rect_t * VDC_ShrdPrmGetDisplayArea (const vdc_graphics_type_t graphics_id)
{
    return &param_graphics[graphics_id].gr_grc;
}   /* End of function VDC_ShrdPrmGetDisplayArea() */

/**************************************************************************//**
 * @brief       Gets the size of the frame buffer to be read
 * @param[in]   graphics_id             : Graphics type ID
 * @retval      Size of the frame buffer to be read
 *****************************************************************************/
vdc_width_read_fb_t * VDC_ShrdPrmGetFrBuffWidth_Rd (const vdc_graphics_type_t graphics_id)
{
    return &param_graphics[graphics_id].width_read_fb;
}   /* End of function VDC_ShrdPrmGetFrBuffWidth_Rd() */

/**************************************************************************//**
 * @brief       Gets the VDC channel whose data is to be output through the LVDS
 * @param[in]   void
 * @retval      A channel in VDC whose data is to be output through the LVDS
 *****************************************************************************/
vdc_channel_t VDC_ShrdPrmGetLvdsCh (void)
{
    /* This value is valid only when LVDS PLL clock resource (VDC_RESOURCE_LVDS_CLK) is valid. */
    return lvds_vdc_sel_ch;
}   /* End of function VDC_ShrdPrmGetLvdsCh() */

/**************************************************************************//**
 * @brief       Updates the resource state
 * @param[in]   rsrc_type               : Resource type
 * @param[in]   rsrc_state              : Resource state
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmSetResource (const vdc_resource_type_t rsrc_type, const vdc_resource_state_t rsrc_state)
{
    vdc_resource_state_t * resource_state;

    resource_state = GetResourceStatePointer(rsrc_type);
    if (resource_state != NULL)
    {
        *resource_state = rsrc_state;
    }
}   /* End of function VDC_ShrdPrmSetResource() */

/**************************************************************************//**
 * @brief       Updates the layer resource state
 * @param[in]   layer_id                : Layer ID
 * @param[in]   rsrc_state              : Resource state
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmSetLayerResource (const vdc_layer_id_t layer_id, const vdc_resource_state_t rsrc_state)
{
    uint32_t layer_id_num;

    if (layer_id != VDC_LAYER_ID_ALL)
    {
        layer_resource[layer_id] = rsrc_state;
    }
    else
    {
        for (layer_id_num = 0; layer_id_num < (uint32_t)VDC_LAYER_ID_NUM; layer_id_num++)
        {
            if (rw_proc_state[layer_id_num] == VDC_RW_PROC_STATE_DISABLE)
            {
                layer_resource[layer_id_num] = rsrc_state;
            }
        }
    }
}   /* End of function VDC_ShrdPrmSetLayerResource() */

/**************************************************************************//**
 * @brief       Gets the resource state
 * @param[in]   rsrc_type               : Resource type
 * @retval      Resource state
 *****************************************************************************/
vdc_resource_state_t VDC_ShrdPrmGetResource (const vdc_resource_type_t rsrc_type)
{
    vdc_resource_state_t * resource_state;
    vdc_resource_state_t   rsrc_state;

    rsrc_state = VDC_RESOURCE_ST_INVALID;

    resource_state = GetResourceStatePointer(rsrc_type);
    if (resource_state != NULL)
    {
        rsrc_state = *resource_state;
    }
    return rsrc_state;
}   /* End of function VDC_ShrdPrmGetResource() */

/**************************************************************************//**
 * @brief       Gets the layer resource state
 * @param[in]   layer_id                : Layer ID
 * @retval      Layer resource state
 *****************************************************************************/
vdc_resource_state_t VDC_ShrdPrmGetLayerResource (const vdc_layer_id_t layer_id)
{
    return layer_resource[layer_id];
}   /* End of function VDC_ShrdPrmGetLayerResource() */

/**************************************************************************//**
 * @brief       Makes the data write/read processing enabled
 * @param[in]   layer_id                : Layer ID
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmSetRwProcEnable (const vdc_layer_id_t layer_id)
{
    if (layer_id != VDC_LAYER_ID_ALL)
    {
        if (layer_resource[layer_id] != VDC_RESOURCE_ST_INVALID)
        {
            rw_proc_state[layer_id] = VDC_RW_PROC_STATE_ENABLE;
        }
    }
}   /* End of function VDC_ShrdPrmSetRwProcEnable() */

/**************************************************************************//**
 * @brief       Makes the data write/read processing disabled
 * @param[in]   layer_id                : Layer ID
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmSetRwProcDisable (const vdc_layer_id_t layer_id)
{
    if (layer_id != VDC_LAYER_ID_ALL)
    {
        if (layer_resource[layer_id] != VDC_RESOURCE_ST_INVALID)
        {
            rw_proc_state[layer_id] = VDC_RW_PROC_STATE_DISABLE;
        }
    }
}   /* End of function VDC_ShrdPrmSetRwProcDisable() */

/**************************************************************************//**
 * @brief       Gets the state whether the specified layer is ready or not
 * @param[in]   layer_id                : Layer ID
 * @retval      Resource state
 *              - VDC_RESOURCE_ST_VALID: The layer resource state of the specified layer is valid and
 *                the data write/read processing in the layer is disabled.
 *              - VDC_RESOURCE_ST_INVALID: The specified layer is not ready.
 *****************************************************************************/
vdc_resource_state_t VDC_ShrdPrmGetRwProcReady (const vdc_layer_id_t layer_id)
{
    vdc_resource_state_t state;

    if ((layer_resource[layer_id] != VDC_RESOURCE_ST_INVALID) &&
        (rw_proc_state[layer_id] == VDC_RW_PROC_STATE_DISABLE))
    {
        state = VDC_RESOURCE_ST_VALID;
    }
    else
    {
        state = VDC_RESOURCE_ST_INVALID;
    }
    return state;
}   /* End of function VDC_ShrdPrmGetRwProcReady() */

/**************************************************************************//**
 * @brief       Gets the state whether the specified layer is already run or not
 * @param[in]   layer_id                : Layer ID
 * @retval      Resource state
 *              - VDC_RESOURCE_ST_VALID: The layer resource state of the specified layer is valid and
 *                the data write/read processing in the layer is enabled.
 *              - VDC_RESOURCE_ST_INVALID: The specified layer is not enabled.
 *****************************************************************************/
vdc_resource_state_t VDC_ShrdPrmGetRwProcEnabled (const vdc_layer_id_t layer_id)
{
    vdc_resource_state_t state;

    if ((layer_resource[layer_id] != VDC_RESOURCE_ST_INVALID) &&
        (rw_proc_state[layer_id] != VDC_RW_PROC_STATE_DISABLE))
    {
        state = VDC_RESOURCE_ST_VALID;
    }
    else
    {
        state = VDC_RESOURCE_ST_INVALID;
    }
    return state;
}   /* End of function VDC_ShrdPrmGetRwProcEnabled() */

/******************************************************************************
Local Functions
******************************************************************************/
/**************************************************************************//**
 * @brief       Gets the pointer to the resource state
 * @param[in]   rsrc_type               : Resource type
 * @retval      Pointer to the resource state
 *****************************************************************************/
static vdc_resource_state_t * GetResourceStatePointer (const vdc_resource_type_t rsrc_type)
{
    vdc_resource_state_t             * resource_state;
    static vdc_shared_param_resource_t param_resource =
    {
        VDC_RESOURCE_ST_INVALID, VDC_RESOURCE_ST_INVALID, VDC_RESOURCE_ST_INVALID, VDC_RESOURCE_ST_INVALID
    };
    static vdc_resource_state_t        lvds_resource = VDC_RESOURCE_ST_INVALID;

    switch (rsrc_type)
    {
        case VDC_RESOURCE_PANEL_CLK:
            resource_state = &param_resource.rsrc_panel_clock;
        break;
        case VDC_RESOURCE_VIDEO_IN:
            resource_state = &param_resource.rsrc_video_input;
        break;
        case VDC_RESOURCE_VSYNC:
            resource_state = &param_resource.rsrc_vsync_signal;
        break;
        case VDC_RESOURCE_LCD_PANEL:
            resource_state = &param_resource.rsrc_lcd_panel;
        break;
        case VDC_RESOURCE_LVDS_CLK:
            resource_state = &lvds_resource;
        break;

        default:
            resource_state = NULL;
        break;
    }
    return resource_state;
}   /* End of function GetResourceStatePointer() */

/**************************************************************************//**
 * @brief       Product-sum operation
 * @param[in]   red             : 8 bits for red
 * @param[in]   green           : 8 bits for green
 * @param[in]   blue            : 8 bits for blue
 * @param[in]   coeff_r         : Coefficient value for Cr/R signal gain adjustment
 * @param[in]   coeff_g         : Coefficient value for Y/G signal gain adjustment
 * @param[in]   coeff_b         : Coefficient value for Cb/B signal gain adjustment
 * @param[in]   offset          : Coefficient value for offset adjustment
 * @retval      Answer
 *****************************************************************************/
static uint32_t SumProduct (
    const int32_t   red,
    const int32_t   green,
    const int32_t   blue,
    int32_t         coeff_r,
    int32_t         coeff_g,
    int32_t         coeff_b,
    const int32_t   offset)
{
    int32_t color_val;

    /* Coefficient values are represented in 11-bit two's complement integer. */
    if (coeff_r >= (TWO_POWER_ELEVEN / VDC_SPARA_DIV_2))
    {
        coeff_r -= (int32_t)TWO_POWER_ELEVEN;
    }
    if (coeff_g >= (TWO_POWER_ELEVEN / VDC_SPARA_DIV_2))
    {
        coeff_g -= (int32_t)TWO_POWER_ELEVEN;
    }
    if (coeff_b >= (TWO_POWER_ELEVEN / VDC_SPARA_DIV_2))
    {
        coeff_b -= (int32_t)TWO_POWER_ELEVEN;
    }

    color_val  = (red * coeff_r) + (green * coeff_g) + (blue * coeff_b);
    color_val /= (int32_t)VDC_SPARA_DIV_256;
    color_val += offset;
    if (color_val < 0)
    {
        color_val = 0;
    }
    else if (color_val > (int32_t)VDC_SPARA_COLOR_CONV_RANGE)
    {
        color_val = (int32_t)VDC_SPARA_COLOR_CONV_RANGE;
    }
    else
    {
    }
    return (uint32_t)color_val;
}   /* End of function SumProduct() */

