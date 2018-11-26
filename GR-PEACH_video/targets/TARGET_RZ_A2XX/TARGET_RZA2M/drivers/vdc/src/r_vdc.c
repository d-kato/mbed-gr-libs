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
* @file         r_vdc.c
* @version      0.01
* $Rev: 18 $
* $Date:: 2014-05-29 11:11:38 +0900#$
* @brief        RZ/A2M VDC driver API function
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_vdc.h"

#include    "r_vdc_user.h"
#include    "r_vdc_register.h"
#include    "r_vdc_shared_param.h"
#include    "r_vdc_check_parameter.h"


/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static vdc_graphics_type_t ConvertRwProcId2GrId(const vdc_layer_id_t layer_id);


/**************************************************************************//**
 * @brief       VDC driver initialization
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Initializes the VDC driver's internal variables.
 *              - Calls the user-defined function specified in init_func.
 *              - Sets up and enables the VDC's panel clock.
 *              - Sets up and enables the LVDS only if LVDS is used.
 *              - Disables all the VDC interrupts.
 * @param[in]   ch                      : Channel
 *                                        - VDC_CHANNEL_0
 * @param[in]   param                   : Initialization parameter
 * @param[in]   init_func               : Pointer to a user-defined function
 * @param[in]   user_num                : User defined number
 * @retval      Error code
 *****************************************************************************/
vdc_error_t R_VDC_Initialize (
    const vdc_channel_t        ch,
    const vdc_init_t   * const param,
    void               (* const init_func)(uint32_t),
    const uint32_t              user_num)
{
    vdc_error_t            ret;
    vdc_resource_state_t   rsrc_state;

    ret = VDC_OK;
#ifdef  R_VDC_CHECK_PARAMETERS
    ret = VDC_InitializeCheckPrm(ch, param);
#endif  /* R_VDC_CHECK_PARAMETERS */
    if (ret == VDC_OK)
    {
        VDC_ShrdPrmInit();
        /* Resource checks */
        rsrc_state = VDC_ShrdPrmGetResource(VDC_RESOURCE_LVDS_CLK);
        if (rsrc_state != VDC_RESOURCE_ST_INVALID)
        {   /* The LVDS PLL clock has been already set. */
            if (param->lvds != NULL)
            {
                ret = VDC_ERR_RESOURCE_LVDS_CLK;
            }
        }
    }
    if (ret == VDC_OK)
    {
        VDC_ShrdPrmSetInitParam(param);

        /* Callback function */
        if (init_func != 0)
        {
            init_func(user_num);
        }

        VDC_Initialize(param);

        /* Disable all VDC interrupts */
        VDC_Int_Disable();

        VDC_ShrdPrmSetResource(VDC_RESOURCE_PANEL_CLK, VDC_RESOURCE_ST_VALID);
        if (param->lvds != NULL)
        {
            VDC_ShrdPrmSetResource(VDC_RESOURCE_LVDS_CLK, VDC_RESOURCE_ST_VALID);
        }
    }
    return ret;
}   /* End of function R_VDC_Initialize() */

/**************************************************************************//**
 * @brief       VDC driver termination
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Disables all the VDC interrupts.
 *              - Disables the VDC panel clock.
 *              - Disables the LVDS if one is used and becomes unnecessary as the result of calling this function.
 *              - Calls the user-defined function specified in quit_func.
 * @param[in]   ch                      : Channel
 *                                        - VDC_CHANNEL_0
 * @param[in]   quit_func               : Pointer to a user-defined function
 * @param[in]   user_num                : User defined number
 * @retval      Error code
 *****************************************************************************/
vdc_error_t R_VDC_Terminate (const vdc_channel_t ch, void (* const quit_func)(uint32_t), const uint32_t user_num)
{
    vdc_error_t    ret;
    vdc_onoff_t    lvds_ref;

    ret = VDC_OK;
#ifdef  R_VDC_CHECK_PARAMETERS
    ret = VDC_TerminateCheckPrm(ch);
#endif  /* R_VDC_CHECK_PARAMETERS */
    if (ret == VDC_OK)
    {
        VDC_ShrdPrmSetTerminate();

        /* Disable all VDC interrupts */
        VDC_Int_Disable();

        VDC_Terminate();

        VDC_ShrdPrmSetResource(VDC_RESOURCE_PANEL_CLK, VDC_RESOURCE_ST_INVALID);

        lvds_ref = VDC_ShrdPrmGetLvdsClkRef();
        if (lvds_ref == VDC_OFF)       /* LVDS PLL clock is not referred. */
        {
            VDC_ShrdPrmSetResource(VDC_RESOURCE_LVDS_CLK, VDC_RESOURCE_ST_INVALID);
        }

        /* Callback function */
        if (quit_func != 0)
        {
            quit_func(user_num);
        }
    }
    return ret;
}   /* End of function R_VDC_Terminate() */

/**************************************************************************//**
 * @brief       Video input setup
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Sets up the phase timing of the input signals.
 *              - Performs delay control on the sync signal for the video inputs.
 *              - Sets up the parameters for the external input video signals.
 * @param[in]   ch                      : Channel
 *                                        - VDC_CHANNEL_0
 * @param[in]   param                   : Video input setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t R_VDC_VideoInput (const vdc_channel_t ch, const vdc_input_t * const param)
{
    vdc_error_t ret;

    ret = VDC_OK;
#ifdef  R_VDC_CHECK_PARAMETERS
    ret = VDC_VideoInputCheckPrm(ch, param);
#endif  /* R_VDC_CHECK_PARAMETERS */
    if (ret == VDC_OK)
    {
        VDC_ShrdPrmSetInputParam(param);

        /* Setting VDC registers */
        VDC_VideoInput(param);

        VDC_ShrdPrmSetResource(VDC_RESOURCE_VIDEO_IN, VDC_RESOURCE_ST_VALID);
    }
    return ret;
}   /* End of function R_VDC_VideoInput() */

/**************************************************************************//**
 * @brief       Synchronization control setup
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Selects the vertical sync signal.
 *              - Sets up the period of the sync signal.
 *              - Sets up the delay of the vertical sync signal.
 *              - Sets up the full-screen enable signal.
 *              - Sets up the compensation for the vertical sync signal.
 * @param[in]   ch                      : Channel
 *                                        - VDC_CHANNEL_0
 * @param[in]   param                   : Synchronization control parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t R_VDC_SyncControl (const vdc_channel_t ch, const vdc_sync_ctrl_t * const param)
{
    vdc_error_t            ret;
    vdc_resource_state_t   rsrc_state;

    ret = VDC_OK;
#ifdef  R_VDC_CHECK_PARAMETERS
    ret = VDC_SyncControlCheckPrm(ch, param);
#endif  /* R_VDC_CHECK_PARAMETERS */
    if (ret == VDC_OK)
    {
        /* Resource checks */
        rsrc_state = VDC_ShrdPrmGetResource(VDC_RESOURCE_PANEL_CLK);
        if (rsrc_state != VDC_RESOURCE_ST_VALID)
        {
            ret = VDC_ERR_RESOURCE_CLK;
        }
    }
    if (ret == VDC_OK)
    {
        if (param->res_vs_sel == VDC_OFF)
        {   /* External input Vsync signal is selected. */
            rsrc_state = VDC_ShrdPrmGetResource(VDC_RESOURCE_VIDEO_IN);
            if (rsrc_state != VDC_RESOURCE_ST_VALID)
            {
                ret = VDC_ERR_RESOURCE_INPUT;
            }
        }
    }
    if (ret == VDC_OK)
    {
        /* Setting VDC registers */
        VDC_SyncControl(param);

        VDC_ShrdPrmSetResource(VDC_RESOURCE_VSYNC, VDC_RESOURCE_ST_VALID);
    }
    return ret;
}   /* End of function R_VDC_SyncControl() */

/**************************************************************************//**
 * @brief       Display output setup
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Sets up the timing signals for driving the LCD panel.
 *              - Sets up the phase, data sequence, and format of the LCD panel output data.
 *              - Sets up the background color.
 * @param[in]   ch                      : Channel
 *                                        - VDC_CHANNEL_0
 * @param[in]   param                   : Display output configuration parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t R_VDC_DisplayOutput (const vdc_channel_t ch, const vdc_output_t * const param)
{
    vdc_error_t            ret;
    vdc_resource_state_t   rsrc_state;

    ret = VDC_OK;
#ifdef  R_VDC_CHECK_PARAMETERS
    ret = VDC_DisplayOutputCheckPrm(ch, param);
#endif  /* R_VDC_CHECK_PARAMETERS */
    if (ret == VDC_OK)
    {
        /* Resource checks */
        rsrc_state = VDC_ShrdPrmGetResource(VDC_RESOURCE_PANEL_CLK);
        if (rsrc_state != VDC_RESOURCE_ST_VALID)
        {
            ret = VDC_ERR_RESOURCE_CLK;
        }
        else
        {
            rsrc_state = VDC_ShrdPrmGetResource(VDC_RESOURCE_VSYNC);
            if (rsrc_state != VDC_RESOURCE_ST_VALID)
            {
                ret = VDC_ERR_RESOURCE_VSYNC;
            }
        }
    }
    if (ret == VDC_OK)
    {
        VDC_ShrdPrmSetOutputParam(param);

        /* Setting VDC registers */
        VDC_DisplayOutput(param);

        VDC_ShrdPrmSetResource(VDC_RESOURCE_LCD_PANEL, VDC_RESOURCE_ST_VALID);
    }
    return ret;
}   /* End of function R_VDC_DisplayOutput() */

/**************************************************************************//**
 * @brief       Interrupt callback setup
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Enables the interrupt when the pointer to the corresponding interrupt callback function is specified.
 *              - Registers the specified interrupt callback function.
 *              - Disables the interrupt when the pointer to the corresponding interrupt callback function is not
 *                specified.
 * @param[in]   ch                      : Channel
 *                                        - VDC_CHANNEL_0
 * @param[in]   param                   : Interrupt callback setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t R_VDC_CallbackISR (const vdc_channel_t ch, const vdc_int_t * const param)
{
    vdc_error_t            ret;
    vdc_resource_state_t   rsrc_state;

    ret = VDC_OK;
#ifdef  R_VDC_CHECK_PARAMETERS
    ret = VDC_CallbackISRCheckPrm(ch, param);
#endif  /* R_VDC_CHECK_PARAMETERS */
    if (ret == VDC_OK)
    {
        /* Resource checks */
        rsrc_state = VDC_ShrdPrmGetResource(VDC_RESOURCE_PANEL_CLK);
        if (rsrc_state != VDC_RESOURCE_ST_VALID)
        {
            ret = VDC_ERR_RESOURCE_CLK;
        }
        else
        {
            rsrc_state = VDC_ShrdPrmGetResource(VDC_RESOURCE_VSYNC);
            if (rsrc_state != VDC_RESOURCE_ST_VALID)
            {
                ret = VDC_ERR_RESOURCE_VSYNC;
            }
        }
    }
    if (ret == VDC_OK)
    {
        /* Setting VDC registers */
        VDC_Int_SetInterrupt(param);
    }
    return ret;
}   /* End of function R_VDC_CallbackISR() */

/**************************************************************************//**
 * @brief       Data write control processing
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Sets up the input image area to be captured.
 *              - Makes input image scale-down/rotation control settings.
 *              - Makes frame buffer write control settings.
 * @param[in]   ch                      : Channel
 *                                        - VDC_CHANNEL_0
 * @param[in]   layer_id                : Layer ID
 *                                        - VDC_LAYER_ID_0_WR
 * @param[in]   param                   : Data write control parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t R_VDC_WriteDataControl (
    const vdc_channel_t        ch,
    const vdc_layer_id_t       layer_id,
    const vdc_write_t  * const param)
{
    vdc_error_t            ret;
    vdc_resource_state_t   rsrc_state;

    ret = VDC_OK;
#ifdef  R_VDC_CHECK_PARAMETERS
    ret = VDC_WriteDataControlCheckPrm(ch, layer_id, param);
#endif  /* R_VDC_CHECK_PARAMETERS */
    if (ret == VDC_OK)
    {
        /* Resource checks */
        rsrc_state = VDC_ShrdPrmGetLayerResource(layer_id);
        if (rsrc_state != VDC_RESOURCE_ST_INVALID)
        {   /* The specified layer is already used. */
            ret = VDC_ERR_RESOURCE_LAYER;
        }
        else
        {
            rsrc_state = VDC_ShrdPrmGetResource(VDC_RESOURCE_VIDEO_IN);
            if (rsrc_state != VDC_RESOURCE_ST_VALID)
            {
                ret = VDC_ERR_RESOURCE_INPUT;
            }
        }
    }
    if (ret == VDC_OK)
    {
        VDC_ShrdPrmSetWriteParam(param);

        /* Setting VDC registers */
        VDC_WriteDataControl(param);

        VDC_ShrdPrmSetLayerResource(layer_id, VDC_RESOURCE_ST_VALID);
    }
    return ret;
}   /* End of function R_VDC_WriteDataControl() */

/**************************************************************************//**
 * @brief       Data write change processing
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Changes the input image area to be captured.
 *              - Makes changes with respect to scaling-down/rotation control of the input image.
 * @param[in]   ch                      : Channel
 *                                        - VDC_CHANNEL_0
 * @param[in]   layer_id                : Layer ID
 *                                        - VDC_LAYER_ID_0_WR
 * @param[in]   param                   : Data write change parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t R_VDC_ChangeWriteProcess (
    const vdc_channel_t            ch,
    const vdc_layer_id_t           layer_id,
    const vdc_write_chg_t   * const param)
{
    vdc_error_t            ret;
    vdc_resource_state_t   rsrc_state;

    ret = VDC_OK;
#ifdef  R_VDC_CHECK_PARAMETERS
    ret = VDC_ChangeWriteProcessCheckPrm(ch, layer_id, param);
#endif  /* R_VDC_CHECK_PARAMETERS */
    if (ret == VDC_OK)
    {
        /* Resource checks */
        rsrc_state = VDC_ShrdPrmGetRwProcEnabled(layer_id);
        if (rsrc_state == VDC_RESOURCE_ST_INVALID)
        {   /* The specified layer is invalid. */
            ret = VDC_ERR_RESOURCE_LAYER;
        }
    }
    if (ret == VDC_OK)
    {
        VDC_ShrdPrmSetChgWriteParam(param);

        /* Setting VDC registers */
        VDC_ChangeWriteProcess(param);
    }
    return ret;
}   /* End of function R_VDC_ChangeWriteProcess() */

/**************************************************************************//**
 * @brief       Data read control processing
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Sets up the display area for graphics images.
 *              - Makes image scale-up control settings (only layer 0).
 *              - Makes frame buffer read control settings.
 * @param[in]   ch                      : Channel
 *                                        - VDC_CHANNEL_0
 * @param[in]   layer_id                : Layer ID
 *                                        - VDC_LAYER_ID_0_RD
 *                                        - VDC_LAYER_ID_2_RD
 *                                        - VDC_LAYER_ID_3_RD
 * @param[in]   param                   : Data read control parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t R_VDC_ReadDataControl (
    const vdc_channel_t        ch,
    const vdc_layer_id_t       layer_id,
    const vdc_read_t   * const param)
{
    vdc_error_t            ret;
    vdc_graphics_type_t    graphics_id;
    vdc_resource_state_t   rsrc_state;

    graphics_id = ConvertRwProcId2GrId(layer_id);

    ret = VDC_OK;
#ifdef  R_VDC_CHECK_PARAMETERS
    ret = VDC_ReadDataControlCheckPrm(ch, layer_id, graphics_id, param);
#endif  /* R_VDC_CHECK_PARAMETERS */
    if (ret == VDC_OK)
    {
        /* Resource checks */
        rsrc_state = VDC_ShrdPrmGetLayerResource(layer_id);
        if (rsrc_state != VDC_RESOURCE_ST_INVALID)
        {   /* The specified layer is already used. */
            ret = VDC_ERR_RESOURCE_LAYER;
        }
    }
    if (ret == VDC_OK)
    {
        VDC_ShrdPrmSetReadParam(graphics_id, param);

        /* Setting VDC registers */
        VDC_ReadDataControl(graphics_id, param);

        VDC_ShrdPrmSetLayerResource(layer_id, VDC_RESOURCE_ST_VALID);
    }
    return ret;
}   /* End of function R_VDC_ReadDataControl() */

/**************************************************************************//**
 * @brief       Data read change processing
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Changes the frame buffer base address.
 *              - Changes the frame buffer read size (image scale-up control, only layer 0).
 *              - Changes the display area for graphics images.
 *              - Changes the graphics display mode.
 * @param[in]   ch                      : Channel
 *                                        - VDC_CHANNEL_0
 * @param[in]   layer_id                : Layer ID
 *                                        - VDC_LAYER_ID_0_RD
 *                                        - VDC_LAYER_ID_2_RD
 *                                        - VDC_LAYER_ID_3_RD
 * @param[in]   param                   : Data read change parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t R_VDC_ChangeReadProcess (
    const vdc_channel_t            ch,
    const vdc_layer_id_t           layer_id,
    const vdc_read_chg_t   * const param)
{
    vdc_error_t            ret;
    vdc_graphics_type_t    graphics_id;
    vdc_resource_state_t   rsrc_state;

    graphics_id = ConvertRwProcId2GrId(layer_id);

    ret = VDC_OK;
#ifdef  R_VDC_CHECK_PARAMETERS
    ret = VDC_ChangeReadProcessCheckPrm(ch, layer_id, param);
#endif  /* R_VDC_CHECK_PARAMETERS */
    if (ret == VDC_OK)
    {
        /* Resource checks */
        rsrc_state = VDC_ShrdPrmGetRwProcEnabled(layer_id);
        if (rsrc_state == VDC_RESOURCE_ST_INVALID)
        {   /* The specified layer is invalid. */
            ret = VDC_ERR_RESOURCE_LAYER;
        }
    }
    if (ret == VDC_OK)
    {
        VDC_ShrdPrmSetChgReadParam(graphics_id, param);

        /* Setting VDC registers */
        VDC_ChangeReadProcess(graphics_id, param);
    }
    return ret;
}   /* End of function R_VDC_ChangeReadProcess() */

/**************************************************************************//**
 * @brief       Data write/read start processing
 *
 *              Description:<br>
 *              This function performs layer start processing. If the layer ID specified in layer_id
 *              is VDC_LAYER_ID_ALL, the function starts all the layers that are in the stopped state
 *              and also enabled. If the layer ID is not VDC_LAYER_ID_ALL, the function starts only
 *              the specified layer.<br>
 *              When performing start processing for write, the function starts a write to the frame buffer.
 *              When performing start processing for read, the function starts a read from the frame buffer
 *              and sets the graphics display mode to the specified values for each layer.
 * @param[in]   ch                      : Channel
 *                                        - VDC_CHANNEL_0
 * @param[in]   layer_id                : Layer ID
 *                                        - VDC_LAYER_ID_ALL
 *                                        - VDC_LAYER_ID_0_WR
 *                                        - VDC_LAYER_ID_0_RD
 *                                        - VDC_LAYER_ID_2_RD
 *                                        - VDC_LAYER_ID_3_RD
 * @param[in]   param                   : Data write/read start parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t R_VDC_StartProcess (
    const vdc_channel_t        ch,
    const vdc_layer_id_t       layer_id,
    const vdc_start_t  * const param)
{
    vdc_error_t            ret;
    vdc_resource_state_t   rsrc_state;

    ret = VDC_OK;
#ifdef  R_VDC_CHECK_PARAMETERS
    ret = VDC_StartProcessCheckPrm(ch, layer_id, param);
#endif  /* R_VDC_CHECK_PARAMETERS */
    if (ret == VDC_OK)
    {
        /* Resource checks */
        if (layer_id != VDC_LAYER_ID_ALL)
        {
            rsrc_state = VDC_ShrdPrmGetRwProcReady(layer_id);
            if (rsrc_state == VDC_RESOURCE_ST_INVALID)
            {   /* The specified layer is invalid. */
                ret = VDC_ERR_RESOURCE_LAYER;
            }
        }
    }
    if (ret == VDC_OK)
    {
        /* Setting VDC registers */
        VDC_StartProcess(layer_id, param);
    }
    return ret;
}   /* End of function R_VDC_StartProcess() */

/**************************************************************************//**
 * @brief       Data write/read stop processing
 *
 *              Description:<br>
 *              This function performs layer stop processing. If the layer ID specified in layer_id is
 *              VDC_LAYER_ID_ALL, the function stops all the layers that are enabled and running.
 *              If the layer ID is not VDC_LAYER_ID_ALL, the function stops only the specified layer.<br>
 *              When performing stop processing for write, the function stops the write to the frame buffer.
 *              When performing stop processing for read, the function stops the read from the frame buffer
 *              and resets the graphics display mode to the initial values for each of the layers.
 * @param[in]   ch                      : Channel
 *                                        - VDC_CHANNEL_0
 * @param[in]   layer_id                : Layer ID
 *                                        - VDC_LAYER_ID_ALL
 *                                        - VDC_LAYER_ID_0_WR
 *                                        - VDC_LAYER_ID_0_RD
 *                                        - VDC_LAYER_ID_2_RD
 *                                        - VDC_LAYER_ID_3_RD
 * @retval      Error code
 *****************************************************************************/
vdc_error_t R_VDC_StopProcess (const vdc_channel_t ch, const vdc_layer_id_t layer_id)
{
    vdc_error_t            ret;
    vdc_resource_state_t   rsrc_state;

    ret = VDC_OK;
#ifdef  R_VDC_CHECK_PARAMETERS
    ret = VDC_StopProcessCheckPrm(ch, layer_id);
#endif  /* R_VDC_CHECK_PARAMETERS */
    if (ret == VDC_OK)
    {
        /* Resource checks */
        if (layer_id != VDC_LAYER_ID_ALL)
        {
            rsrc_state = VDC_ShrdPrmGetRwProcEnabled(layer_id);
            if (rsrc_state == VDC_RESOURCE_ST_INVALID)
            {   /* The specified layer is invalid. */
                ret = VDC_ERR_RESOURCE_LAYER;
            }
        }
    }
    if (ret == VDC_OK)
    {
        /* Setting VDC registers */
        VDC_StopProcess(layer_id);
    }
    return ret;
}   /* End of function R_VDC_StopSurface() */

/**************************************************************************//**
 * @brief       Data write/read control release processing
 *
 *              Description:<br>
 *              If the layer ID specified in layer_id is VDC_LAYER_ID_ALL, the function disables all the layers
 *              that are not running and also enabled. If the layer ID is not VDC_LAYER_ID_ALL, the function
 *              disables only the specified layers.
 * @param[in]   ch                      : Channel
 *                                        - VDC_CHANNEL_0
 * @param[in]   layer_id                : Layer ID
 *                                        - VDC_LAYER_ID_ALL
 *                                        - VDC_LAYER_ID_0_WR
 *                                        - VDC_LAYER_ID_0_RD
 *                                        - VDC_LAYER_ID_2_RD
 *                                        - VDC_LAYER_ID_3_RD
 * @retval      Error code
 *****************************************************************************/
vdc_error_t R_VDC_ReleaseDataControl (const vdc_channel_t ch, const vdc_layer_id_t layer_id)
{
    vdc_error_t            ret;
    vdc_resource_state_t   rsrc_state;

    ret = VDC_OK;
#ifdef  R_VDC_CHECK_PARAMETERS
    ret = VDC_ReleaseDataControlCheckPrm(ch, layer_id);
#endif  /* R_VDC_CHECK_PARAMETERS */
    if (ret == VDC_OK)
    {
        /* Resource checks */
        if (layer_id != VDC_LAYER_ID_ALL)
        {
            rsrc_state = VDC_ShrdPrmGetRwProcReady(layer_id);
            if (rsrc_state == VDC_RESOURCE_ST_INVALID)
            {   /* The specified layer is invalid. */
                ret = VDC_ERR_RESOURCE_LAYER;
            }
        }
    }
    if (ret == VDC_OK)
    {
        VDC_ShrdPrmSetLayerResource(layer_id, VDC_RESOURCE_ST_INVALID);
    }
    return ret;
}   /* End of function R_VDC_ReleaseDataControl() */

/**************************************************************************//**
 * @brief       Noise reduction setup
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Turns on and off noise reduction processing.
 *              - Sets up the noise reduction parameters for the Y/G, Cb/B, and Cr/R signals.
 * @param[in]   ch                      : Channel
 *                                        - VDC_CHANNEL_0
 * @param[in]   nr1d_on                 : Noise reduction ON/OFF setting
 * @param[in]   param                   : Noise reduction setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t R_VDC_VideoNoiseReduction (
    const vdc_channel_t                    ch,
    const vdc_onoff_t                      nr1d_on,
    const vdc_noise_reduction_t    * const param)
{
    vdc_error_t            ret;
    vdc_resource_state_t   rsrc_state;

    ret = VDC_OK;
#ifdef  R_VDC_CHECK_PARAMETERS
    ret = VDC_VideoNoiseReductCheckPrm(ch, param);
#endif  /* R_VDC_CHECK_PARAMETERS */
    if (ret == VDC_OK)
    {
        /* Resource checks */
        rsrc_state = VDC_ShrdPrmGetResource(VDC_RESOURCE_VIDEO_IN);
        if (rsrc_state != VDC_RESOURCE_ST_VALID)
        {
            ret = VDC_ERR_RESOURCE_INPUT;
        }
    }
    if (ret == VDC_OK)
    {
        /* Setting VDC registers */
        VDC_VideoNoiseReduction(nr1d_on, param);
    }
    return ret;
}   /* End of function R_VDC_VideoNoiseReduction() */

/**************************************************************************//**
 * @brief       Color matrix setup
 *
 *              Description:<br>
 *              This function sets up the specified color matrix.
 * @param[in]   ch                      : Channel
 *                                        - VDC_CHANNEL_0
 * @param[in]   param                   : Color matrix setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t R_VDC_ImageColorMatrix (const vdc_channel_t ch, const vdc_color_matrix_t * const param)
{
    vdc_error_t            ret;
    vdc_resource_state_t   rsrc_state;

    ret = VDC_OK;
#ifdef  R_VDC_CHECK_PARAMETERS
    ret = VDC_ImageColorMatrixCheckPrm(ch, param);
#endif  /* R_VDC_CHECK_PARAMETERS */
    if (ret == VDC_OK)
    {
        /* Resource checks */
        if (param->module == VDC_COLORMTX_IMGCNT)
        {
            rsrc_state = VDC_ShrdPrmGetLayerResource(VDC_LAYER_ID_0_WR);
        }
        else    /* VDC_COLORMTX_ADJ_0 */
        {
            rsrc_state = VDC_ShrdPrmGetLayerResource(VDC_LAYER_ID_0_RD);
        }

        if (rsrc_state == VDC_RESOURCE_ST_INVALID)
        {
            ret = VDC_ERR_RESOURCE_LAYER;
        }
    }
    if (ret == VDC_OK)
    {
        /* Setting VDC registers */
        VDC_ImageColorMatrix(param);
    }
    return ret;
}   /* End of function R_VDC_ImageColorMatrix() */

/**************************************************************************//**
 * @brief       Image enhancement processing
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Turns on and off sharpness processing.
 *              - Sets up the sharpness parameter.
 *              - Turns on and off LTI processing.
 *              - Sets up the LTI parameter.
 *              - Sets up the enhancer-enabled area to be subjected to sharpness and LTI processing.
 * @param[in]   ch                      : Channel
 *                                        - VDC_CHANNEL_0
 * @param[in]   imgimprv_id             : Image quality improver ID
 *                                        - VDC_IMG_IMPRV_0
 * @param[in]   shp_h_on                : Sharpness ON/OFF setting
 * @param[in]   sharp_param             : Sharpness setup parameter
 * @param[in]   lti_h_on                : LTI ON/OFF setting
 * @param[in]   lti_param               : LTI setup parameter
 * @param[in]   enh_area                : Enhancer-enabled area setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t R_VDC_ImageEnhancement (
    const vdc_channel_t                ch,
    const vdc_imgimprv_id_t            imgimprv_id,
    const vdc_onoff_t                  shp_h_on,
    const vdc_enhance_sharp_t  * const sharp_param,
    const vdc_onoff_t                  lti_h_on,
    const vdc_enhance_lti_t    * const lti_param,
    const vdc_period_rect_t    * const enh_area)
{
    vdc_error_t            ret;
    vdc_resource_state_t   rsrc_state;
    vdc_color_space_t      color_space;

    ret = VDC_OK;
#ifdef  R_VDC_CHECK_PARAMETERS
    ret = VDC_ImageEnhancementCheckPrm(ch, imgimprv_id, sharp_param, enh_area);
#endif  /* R_VDC_CHECK_PARAMETERS */
    if (ret == VDC_OK)
    {
        /* Resource checks */
        rsrc_state = VDC_ShrdPrmGetLayerResource(VDC_LAYER_ID_0_RD);
        if (rsrc_state == VDC_RESOURCE_ST_INVALID)
        {   /* The layer is invalid. */
            ret = VDC_ERR_RESOURCE_LAYER;
        }
        else
        {
            /* Condition checks */
            color_space = VDC_ShrdPrmGetColorSpaceFbRd(VDC_GR_TYPE_GR0);
            if (color_space == VDC_COLOR_SPACE_GBR)
            {   /* The image quality improver does not act on RGB signals. */
                ret = VDC_ERR_IF_CONDITION;
            }
        }
    }
    if (ret == VDC_OK)
    {
        /* Setting VDC registers */
        VDC_ImageEnhancement(shp_h_on, sharp_param, lti_h_on, lti_param, enh_area);
    }
    return ret;
}   /* End of function R_VDC_ImageEnhancement() */

/**************************************************************************//**
 * @brief       Black stretch setup
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Turns on and off black stretch processing.
 *              - Sets up the black stretch parameters.
 * @param[in]   ch                      : Channel
 *                                        - VDC_CHANNEL_0
 * @param[in]   imgimprv_id             : Image quality improver ID
 *                                        - VDC_IMG_IMPRV_0
 * @param[in]   bkstr_on                : Black stretch ON/OFF setting
 * @param[in]   param                   : Black stretch setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t R_VDC_ImageBlackStretch (
    const vdc_channel_t        ch,
    const vdc_imgimprv_id_t    imgimprv_id,
    const vdc_onoff_t          bkstr_on,
    const vdc_black_t  * const param)
{
    vdc_error_t            ret;
    vdc_resource_state_t   rsrc_state;
    vdc_color_space_t      color_space;

    ret = VDC_OK;
#ifdef  R_VDC_CHECK_PARAMETERS
    ret = VDC_ImageBlackStretchCheckPrm(ch, imgimprv_id, param);
#endif  /* R_VDC_CHECK_PARAMETERS */
    if (ret == VDC_OK)
    {
        /* Resource checks */
        rsrc_state = VDC_ShrdPrmGetLayerResource(VDC_LAYER_ID_0_RD);
        if (rsrc_state == VDC_RESOURCE_ST_INVALID)
        {   /* The layer is invalid. */
            ret = VDC_ERR_RESOURCE_LAYER;
        }
        else
        {
            /* Condition checks */
            color_space = VDC_ShrdPrmGetColorSpaceFbRd(VDC_GR_TYPE_GR0);
            if (color_space == VDC_COLOR_SPACE_GBR)
            {   /* The image quality improver does not act on RGB signals. */
                ret = VDC_ERR_IF_CONDITION;
            }
        }
    }
    if (ret == VDC_OK)
    {
        /* Setting VDC registers */
        VDC_ImageBlackStretch(bkstr_on, param);
    }
    return ret;
}   /* End of function R_VDC_ImageBlackStretch() */

/**************************************************************************//**
 * @brief       Alpha blending setup
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Sets up the alpha value of the ARGB1555/RGBA5551 formats.
 *              - Make settings for premultiplication processing at alpha blending in one-pixel.
 * @param[in]   ch                      : Channel
 *                                        - VDC_CHANNEL_0
 * @param[in]   layer_id                : Layer ID
 *                                        - VDC_LAYER_ID_2_RD
 *                                        - VDC_LAYER_ID_3_RD
 * @param[in]   param                   : Alpha blending setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t R_VDC_AlphaBlending (
    const vdc_channel_t                ch,
    const vdc_layer_id_t               layer_id,
    const vdc_alpha_blending_t * const param)
{
    vdc_error_t            ret;
    vdc_graphics_type_t    graphics_id;
    vdc_resource_state_t   rsrc_state;

    graphics_id = ConvertRwProcId2GrId(layer_id);

    ret = VDC_OK;
#ifdef  R_VDC_CHECK_PARAMETERS
    ret = VDC_AlphaBlendingCheckPrm(ch, layer_id, param);
#endif  /* R_VDC_CHECK_PARAMETERS */
    if (ret == VDC_OK)
    {
        /* Resource checks */
        rsrc_state = VDC_ShrdPrmGetLayerResource(layer_id);
        if (rsrc_state == VDC_RESOURCE_ST_INVALID)
        {   /* The specified layer is invalid. */
            ret = VDC_ERR_RESOURCE_LAYER;
        }
    }
    if (ret == VDC_OK)
    {
        /* Setting VDC registers */
        VDC_AlphaBlending(graphics_id, param);
    }
    return ret;
}   /* End of function R_VDC_AlphaBlending() */

/**************************************************************************//**
 * @brief       Rectangle alpha blending setup
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Turns on and off alpha blending in a rectangular area.
 *              - Sets up the rectangular area subjected to alpha blending.
 *              - Sets up the alpha value for alpha blending in a rectangular area.
 *              - Makes fade-in/-out settings to be applied to rectangle alpha blending.
 * @param[in]   ch                      : Channel
 *                                        - VDC_CHANNEL_0
 * @param[in]   layer_id                : Layer ID
 *                                        - VDC_LAYER_ID_2_RD
 *                                        - VDC_LAYER_ID_3_RD
 * @param[in]   gr_arc_on               : ON/OFF setting for alpha blending in a rectangular area
 * @param[in]   param                   : Setup parameter for alpha blending in a rectangular area
 * @retval      Error code
 *****************************************************************************/
vdc_error_t R_VDC_AlphaBlendingRect (
    const vdc_channel_t                        ch,
    const vdc_layer_id_t                       layer_id,
    const vdc_onoff_t                          gr_arc_on,
    const vdc_alpha_blending_rect_t    * const param)
{
    vdc_error_t            ret;
    vdc_graphics_type_t    graphics_id;
    vdc_resource_state_t   rsrc_state;

    graphics_id = ConvertRwProcId2GrId(layer_id);

    ret = VDC_OK;
#ifdef  R_VDC_CHECK_PARAMETERS
    ret = VDC_AlphaBlendingRectCheckPrm(ch, layer_id, param);
#endif  /* R_VDC_CHECK_PARAMETERS */
    if (ret == VDC_OK)
    {
        /* Resource checks */
        rsrc_state = VDC_ShrdPrmGetLayerResource(layer_id);
        if (rsrc_state == VDC_RESOURCE_ST_INVALID)
        {   /* The specified layer is invalid. */
            ret = VDC_ERR_RESOURCE_LAYER;
        }
    }
    if (ret == VDC_OK)
    {
        /* Setting VDC registers */
        VDC_AlphaBlendingRect(graphics_id, gr_arc_on, param);
    }
    return ret;
}   /* End of function R_VDC_AlphaBlendingRect() */

/**************************************************************************//**
 * @brief       Chroma-key setup
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Turns on and off the chroma-key processing.
 *              - Sets up the color signals to be subject to chroma-key processing and
 *                the color signals after replacement.
 * @param[in]   ch                      : Channel
 *                                        - VDC_CHANNEL_0
 * @param[in]   layer_id                : Layer ID
 *                                        - VDC_LAYER_ID_0_RD
 *                                        - VDC_LAYER_ID_2_RD
 *                                        - VDC_LAYER_ID_3_RD
 * @param[in]   gr_ck_on                : Chroma-key ON/OFF setting
 * @param[in]   param                   : Chroma-key setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t R_VDC_Chromakey (
    const vdc_channel_t            ch,
    const vdc_layer_id_t           layer_id,
    const vdc_onoff_t              gr_ck_on,
    const vdc_chromakey_t  * const param)
{
    vdc_error_t            ret;
    vdc_graphics_type_t    graphics_id;
    vdc_color_space_t      color_space;
    vdc_resource_state_t   rsrc_state;

    graphics_id = ConvertRwProcId2GrId(layer_id);

    ret = VDC_OK;
#ifdef  R_VDC_CHECK_PARAMETERS
    ret = VDC_ChromakeyCheckPrm(ch, layer_id, graphics_id, param);
#endif  /* R_VDC_CHECK_PARAMETERS */
    if (ret == VDC_OK)
    {
        /* Resource checks */
        rsrc_state = VDC_ShrdPrmGetLayerResource(layer_id);
        if (rsrc_state == VDC_RESOURCE_ST_INVALID)
        {   /* The specified layer is invalid. */
            ret = VDC_ERR_RESOURCE_LAYER;
        }
        else
        {
            /* Condition checks */
            color_space = VDC_ShrdPrmGetColorSpaceFbRd(graphics_id);
            if (color_space == VDC_COLOR_SPACE_YCBCR)
            {   /* Chroma-key processing cannot be used
                   when the format of the signal read from the frame buffer is YCbCr422 or YCbCr444. */
                ret = VDC_ERR_IF_CONDITION;
            }
        }
    }
    if (ret == VDC_OK)
    {
        /* Setting VDC registers */
        VDC_Chromakey(graphics_id, gr_ck_on, param);
    }
    return ret;
}   /* End of function R_VDC_Chromakey() */

/**************************************************************************//**
 * @brief       CLUT setup
 *
 *              Description:<br>
 *              This function sets up CLUT for the specified layer.
 * @param[in]   ch                      : Channel
 *                                        - VDC_CHANNEL_0
 * @param[in]   layer_id                : Layer ID
 *                                        - VDC_LAYER_ID_0_RD
 *                                        - VDC_LAYER_ID_2_RD
 *                                        - VDC_LAYER_ID_3_RD
 * @param[in]   param                   : CLUT setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t R_VDC_CLUT (const vdc_channel_t ch, const vdc_layer_id_t layer_id, const vdc_clut_t * const param)
{
    vdc_error_t            ret;
    vdc_graphics_type_t    graphics_id;
    vdc_resource_state_t   rsrc_state;

    graphics_id = ConvertRwProcId2GrId(layer_id);

    ret = VDC_OK;
#ifdef  R_VDC_CHECK_PARAMETERS
    ret = VDC_CLUTCheckPrm(ch, layer_id, graphics_id, param);
#endif  /* R_VDC_CHECK_PARAMETERS */
    if (ret == VDC_OK)
    {
        /* Resource checks */
        rsrc_state = VDC_ShrdPrmGetLayerResource(layer_id);
        if (rsrc_state == VDC_RESOURCE_ST_INVALID)
        {   /* The specified layer is invalid. */
            ret = VDC_ERR_RESOURCE_LAYER;
        }
    }
    if (ret == VDC_OK)
    {
        /* Setting VDC registers */
        VDC_CLUT(graphics_id, param);
    }
    return ret;
}   /* End of function R_VDC_SetCLUT() */

/**************************************************************************//**
 * @brief       Display calibration processing
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Sets up panel brightness adjustment.
 *              - Sets up contrast adjustment.
 *              - Sets up panel dithering.
 *              - Makes control settings for the correction circuit sequence.
 * @param[in]   ch                      : Channel
 *                                        - VDC_CHANNEL_0
 * @param[in]   param                   : Display calibration parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t R_VDC_DisplayCalibration (const vdc_channel_t ch, const vdc_disp_calibration_t * const param)
{
    vdc_error_t            ret;
    vdc_resource_state_t   rsrc_state;

    ret = VDC_OK;
#ifdef  R_VDC_CHECK_PARAMETERS
    ret = VDC_DisplayCalibrationCheckPrm(ch, param);
#endif  /* R_VDC_CHECK_PARAMETERS */
    if (ret == VDC_OK)
    {
        /* Resource checks */
        rsrc_state = VDC_ShrdPrmGetResource(VDC_RESOURCE_LCD_PANEL);
        if (rsrc_state != VDC_RESOURCE_ST_VALID)
        {
            ret = VDC_ERR_RESOURCE_OUTPUT;
        }
    }
    if (ret == VDC_OK)
    {
        /* Setting VDC registers */
        VDC_DisplayCalibration(param);
    }
    return ret;
}   /* End of function R_VDC_DisplayCalibration() */

/**************************************************************************//**
 * @brief       Gamma correction setup
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Turns on and off gamma correction processing.
 *              - Sets up the gamma correction gain adjustment values for the G/B/R signals.
 *              - Sets up the gamma correction start threshold values for the G/B/R signals.
 * @param[in]   ch                      : Channel
 *                                        - VDC_CHANNEL_0
 * @param[in]   gam_on                  : Gamma correction ON/OFF setting
 * @param[in]   param                   : Gamma correction setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t R_VDC_GammaCorrection (
    const vdc_channel_t                    ch,
    const vdc_onoff_t                      gam_on,
    const vdc_gamma_correction_t   * const param)
{
    vdc_error_t            ret;
    vdc_resource_state_t   rsrc_state;

    ret = VDC_OK;
#ifdef  R_VDC_CHECK_PARAMETERS
    ret = VDC_GammaCorrectionCheckPrm(ch, param);
#endif  /* R_VDC_CHECK_PARAMETERS */
    if (ret == VDC_OK)
    {
        /* Resource checks */
        rsrc_state = VDC_ShrdPrmGetResource(VDC_RESOURCE_LCD_PANEL);
        if (rsrc_state != VDC_RESOURCE_ST_VALID)
        {
            ret = VDC_ERR_RESOURCE_OUTPUT;
        }
    }
    if (ret == VDC_OK)
    {
        /* Setting VDC registers */
        VDC_GammaCorrection(gam_on, param);
    }
    return ret;
}   /* End of function R_VDC_GammaCorrection() */

/******************************************************************************
Local Functions
******************************************************************************/
/**************************************************************************//**
 * @brief       Convert read/write process ID into graphics type ID
 *
 * @param[in]   layer_id              : Layer ID
 * @retval      Graphics type ID
 *****************************************************************************/
static vdc_graphics_type_t ConvertRwProcId2GrId (const vdc_layer_id_t layer_id)
{
    vdc_graphics_type_t graphics_id;

    switch (layer_id)
    {
        case VDC_LAYER_ID_0_RD:                /* Layer 0, read process */
            graphics_id = VDC_GR_TYPE_GR0;     /* Graphics 0 */
        break;
        case VDC_LAYER_ID_2_RD:                /* Layer 2, read process */
            graphics_id = VDC_GR_TYPE_GR2;     /* Graphics 2 */
        break;
        case VDC_LAYER_ID_3_RD:                /* Layer 3, read process */
            graphics_id = VDC_GR_TYPE_GR3;     /* Graphics 3 */
        break;
        default:                                /* Error */
            graphics_id = VDC_GR_TYPE_NUM;
        break;
    }
    return graphics_id;
}   /* End of function ConvertRwProcId2GrId() */

