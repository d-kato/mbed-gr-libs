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
* Copyright (C) 2014 - 2015 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**
* @file  video_input.c
* @brief   $Rev: 51 $
* $Date:: 2014-03-14 18:42:33 +0900#$
*/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include  "r_typedefs.h"
#include  "r_ospl.h"
#include  "video_input.h"
#include  "video_decoder.h"
#include  "window_surfaces_typedef.h"  /* WINDOW_SURFACES_DEFAULT_CHANNEL */
#ifndef R_OSPL_NDEBUG
#include  <stdio.h>
#endif


/******************************************************************************
Typedef definitions
******************************************************************************/

/**
* @typedef  video_input_vdc5_layer_t
* @brief  video_input_vdc5_layer_t
*/
typedef struct st_video_input_vdc5_layer_t  video_input_vdc5_layer_t;
struct st_video_input_vdc5_layer_t {
    bool_t                is_data_control;
    vdc5_layer_id_t       data_control_ID;
};


/**
* @typedef  vdc5_int_cb_t
* @brief  vdc5_int_cb_t
*/
typedef  void (* vdc5_int_cb_t )( const uint32_t );


/******************************************************************************
Macro definitions
******************************************************************************/

#define VSYNC_1_2_FH_TIMING  (858u) /* Vsync signal 1/2fH phase timing */
#define VSYNC_1_4_FH_TIMING  (429u) /* Vsync signal 1/4fH phase timing */

#define     IMGCAP_SIZE_NTSC_HS         (122u * 2u)
#define     IMGCAP_SIZE_NTSC_HW         (720u * 2u)
#define     IMGCAP_SIZE_NTSC_VS         (16u)
#define     IMGCAP_SIZE_NTSC_VW         (480u/2u)

#define     IMGCAP_SIZE_PAL_HS          (132u * 2u)
#define     IMGCAP_SIZE_PAL_HW          (720u * 2u)
#define     IMGCAP_SIZE_PAL_VS          (19u)
#define     IMGCAP_SIZE_PAL_VW          (560u/2u)

#define     VIDEO_FORMAT                (VDC5_RES_MD_YCBCR422)


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

static void  R_VIDEO_INPUT_OnVideoInVSync( vdc5_channel_t  video_input_channel,  vdc5_int_type_t int_type );
static void  R_VIDEO_INPUT_OnVideoInVSync_Ch0( vdc5_int_type_t int_type );
static void  R_VIDEO_INPUT_OnVideoInVSync_Ch1( vdc5_int_type_t int_type );
#if INTERRUPT_FUNCTION_TYPE == INTERRUPT_FUNCTION_TYPE_OS_RTX
static void  InterruptCallbackVector_S0_VI_VSYNC0(void);
static void  InterruptCallbackVector_S0_VI_VSYNC1(void);
static void  InterruptCallbackVector_S1_VI_VSYNC0(void);
static void  InterruptCallbackVector_S1_VI_VSYNC1(void);
vdc5_int_cb_t  gs_VDC5_ISR_S0_VI_VSYNC0;
vdc5_int_cb_t  gs_VDC5_ISR_S0_VI_VSYNC1;
vdc5_int_cb_t  gs_VDC5_ISR_S1_VI_VSYNC0;
vdc5_int_cb_t  gs_VDC5_ISR_S1_VI_VSYNC1;
#endif

static video_input_t  *gs_InterruptToSelf[ 2 /* video_input_channel */ ][ 2 /* data_control_ID */ ];

static errnum_t  R_VIDEO_INPUT_STATIC_AttachSelfToInterrupt(
    vdc5_channel_t  in_video_input_channel,  vdc5_layer_id_t  in_data_control_ID,
    video_input_t  *in_self );
static errnum_t  R_VIDEO_INPUT_STATIC_DetachSelfFromInterrupt(
    vdc5_channel_t  in_video_input_channel,  vdc5_layer_id_t  in_data_control_ID,
    video_input_t  *in_self );
static errnum_t  R_VIDEO_INPUT_STATIC_GetSelfPointer(
    vdc5_channel_t  in_video_input_channel,  vdc5_layer_id_t  in_data_control_ID,
    video_input_t ***  out_self_pp );
static errnum_t  R_VIDEO_INPUT_STATIC_GetSelfFromInterrupt(
    vdc5_channel_t  in_video_input_channel,  vdc5_int_type_t  in_interrupt,
    video_input_t  **out_self );


/**
* @brief   Initializes by constant data.
*
* @param   self video_input_t.
* @return  None.
*/
void  R_VIDEO_INPUT_InitConst( video_input_t *const  self )
{
    self->is_data_control = false;
    self->is_vsync_interrupt_registered[0] = false;
    self->is_vsync_interrupt_registered[1] = false;
    self->is_vsync_interrupt_registered[2] = false;
}


/***********************************************************************
* Implement: R_VIDEO_INPUT_Initialize
************************************************************************/
errnum_t  R_VIDEO_INPUT_Initialize( video_input_t *const  self,
                                    video_input_config_t *in_out_Config )
{
    errnum_t         e;
    vdc5_error_t     e_vdc5;
#ifdef  IS_MBED_USED
    vdec_error_t     e_vdec;
#endif
    vdc5_write_t     writing_config;
    frame_buffer_t  *frame;
    uintptr_t        frame_physical_address[ R_COUNT_OF( frame->buffer_address ) ];


    /* Initialize by constant */
    ASSERT_R( ! self->is_data_control,  e=E_STATE; goto fin );
    ASSERT_R( ! self->is_vsync_interrupt_registered[0],  e=E_STATE; goto fin );
    ASSERT_R( ! self->is_vsync_interrupt_registered[1],  e=E_STATE; goto fin );
    ASSERT_R( ! self->is_vsync_interrupt_registered[2],  e=E_STATE; goto fin );
    self->captured_count = 0;


    /* Check necessary flags */
    {
        enum { necessary_flags =
                   VIDEO_INPUT_CONFIG_T_FRAME_BUFFER
             };

        ASSERT_R( IS_ALL_BITS_SET( in_out_Config->flags, necessary_flags ),
                  e=E_OTHERS; goto fin );
    }


    /* Set "frame" */
    {
        int_fast32_t  i;

        frame = in_out_Config->frame_buffer;
        ASSERT_R( frame->buffer_count == 2,  e=E_OTHERS; goto fin );
        self->frame_buffer = *frame;

        for ( i = 0;  i < frame->buffer_count;  i += 1 ) {
            e= R_OSPL_ToPhysicalAddress( frame->buffer_address[ i ],
                                         &frame_physical_address[ i ] );
            IF(e!=0) {
                goto fin;
            }
        }
    }


    /* Set default "in_out_Config->video_input_channel_num" */
    if ( IS_BIT_NOT_SET( in_out_Config->flags, VIDEO_INPUT_CONFIG_T_VIDEO_INPUT_CHANNEL_NUM ) ) {
        in_out_Config->video_input_channel_num = WINDOW_SURFACES_DEFAULT_CHANNEL;
    }


    /* Set from "in_out_Config->video_input_channel_num" */
    switch ( in_out_Config->video_input_channel_num ) {
        case  0:
            self->video_input_channel = VDC5_CHANNEL_0;
            self->video_input_select = VDEC_ADC_VINSEL_VIN1;
            break;
        case  1:
            self->video_input_channel = VDC5_CHANNEL_1;
            self->video_input_select = VDEC_ADC_VINSEL_VIN1;
            break;
        case  2:
            self->video_input_channel = VDC5_CHANNEL_0;
            self->video_input_select = VDEC_ADC_VINSEL_VIN2;
            break;
        case  3:
            self->video_input_channel = VDC5_CHANNEL_1;
            self->video_input_select = VDEC_ADC_VINSEL_VIN2;
            break;
        default:
            ASSERT_R( false,  e=E_OTHERS; goto fin );
    }


    /* Set default "in_out_Config->display_channel_num" */
    if ( IS_BIT_NOT_SET( in_out_Config->flags, VIDEO_INPUT_CONFIG_T_DISPLAY_CHANNEL_NUM ) ) {
        if ( IS_BIT_NOT_SET( in_out_Config->flags, VIDEO_INPUT_CONFIG_T_DISPLAY_LAYER_NUM ) ) {
            in_out_Config->display_channel_num = VIDEO_INPUT_NOT_DISPLAY;
        } else {
            in_out_Config->display_channel_num = WINDOW_SURFACES_DEFAULT_CHANNEL;
        }
    }


    /* Set default "in_out_Config->display_layer_num" */
    if ( IS_BIT_NOT_SET( in_out_Config->flags, VIDEO_INPUT_CONFIG_T_DISPLAY_LAYER_NUM ) ) {
        if ( in_out_Config->display_channel_num == VIDEO_INPUT_NOT_DISPLAY ) {
            in_out_Config->display_layer_num = VIDEO_INPUT_NOT_DISPLAY;
        } else {
            in_out_Config->display_layer_num = -1;
        }
    }
    ASSERT_D( in_out_Config->display_layer_num == VIDEO_INPUT_NOT_DISPLAY  ||
              in_out_Config->display_layer_num <= -1,  e=E_OTHERS; goto fin );
    ASSERT_D(
        ( in_out_Config->display_channel_num == VIDEO_INPUT_NOT_DISPLAY  &&
          in_out_Config->display_layer_num   == VIDEO_INPUT_NOT_DISPLAY ) ||
        ( in_out_Config->display_channel_num != VIDEO_INPUT_NOT_DISPLAY  &&
          in_out_Config->display_layer_num   != VIDEO_INPUT_NOT_DISPLAY ),
        e=E_OTHERS; goto fin );


    /* Set "self->data_control_ID" : No care of "display_layer_num" */
    if ( in_out_Config->display_channel_num == 0 ) {
        if ( self->video_input_channel == 0 ) {
            self->data_control_ID = VDC5_LAYER_ID_0_WR;
        } else {
            self->data_control_ID = VDC5_LAYER_ID_1_WR;
        }
    } else if ( in_out_Config->display_channel_num == 1 ) {
        if ( self->video_input_channel == 0 ) {
            self->data_control_ID = VDC5_LAYER_ID_1_WR;
        } else {
            self->data_control_ID = VDC5_LAYER_ID_0_WR;
        }
    } else {
        ASSERT_D( in_out_Config->display_channel_num == VIDEO_INPUT_NOT_DISPLAY,
                  e=E_OTHERS; goto fin );

        self->data_control_ID = VDC5_LAYER_ID_0_WR;
    }


    /* Set "self->captured_async" */
    self->captured_async = NULL;
    if ( IS_BIT_SET( in_out_Config->flags, VIDEO_INPUT_CONFIG_T_CAPTURED_ASYNC ) ) {
        self->captured_async = in_out_Config->captured_async;
    }


    /* From GRPDRV_Init() */
    {
        vdc5_input_t      input;

        /* Input parameter */
        input.inp_sel   = VDC5_INPUT_SEL_VDEC;           /* Input select */
        input.inp_fh50  = (uint16_t)VSYNC_1_2_FH_TIMING; /* Vsync signal 1/2fH phase timing */
        input.inp_fh25  = (uint16_t)VSYNC_1_4_FH_TIMING; /* Vsync signal 1/4fH phase timing */
        input.dly       = NULL;                          /* Sync signal delay adjustment */
        input.ext_sig   = NULL;                          /* External input signal */
        e_vdc5 = R_VDC5_VideoInput( self->video_input_channel, &input );
        IF ( e_vdc5 != VDC5_OK ) {
            e=E_OTHERS;
            goto fin;
        }
    }


    /* ... */
    {
        vdec_channel_t           vdec_channel;  /* video decoder channel */
#ifdef  IS_MBED_USED
        graphics_col_sys_t       color_system = GRPH_COL_SYS_NTSC_358;
#else
        graphics_col_sys_t       color_system;
#endif
        vdc5_scalingdown_rot_t  *writing_scale = &writing_config.scalingdown_rot;


        /* From SetVideoDecoder() */

        if ( self->video_input_channel == VDC5_CHANNEL_0 ) {
            vdec_channel = VDEC_CHANNEL_0;
        } else {
            ASSERT_R( self->video_input_channel == VDC5_CHANNEL_1,  e=E_OTHERS; goto fin );
            vdec_channel = VDEC_CHANNEL_1;
        }
#ifdef  IS_MBED_USED
        e_vdec = GRAPHICS_VideoDecoderInit( self->video_input_select, vdec_channel, color_system );
        IF ( e_vdec != VDEC_OK ) {
            e=E_OTHERS;
            goto fin;
        }
#else
        color_system = GRAPHICS_VideoDecoderInit( self->video_input_select, vdec_channel );
        if ( color_system == GRPH_COL_SYS_UNKNOWN ) {
            color_system = GRPH_COL_SYS_NTSC_358;
        }
#endif


        /* From GRPDRV_VideoCreateNonDispSurface() */

        /* Image area to be captured */
#if  VIDEO_FORMAT_TYPE == VIDEO_FORMAT_TYPE_NTSC
        ASSERT_R( (color_system == GRPH_COL_SYS_NTSC_358) ||
                  (color_system == GRPH_COL_SYS_NTSC_443),
                  e=E_OTHERS; goto fin );

        writing_scale->res.vs = (uint16_t)( IMGCAP_SIZE_NTSC_VS - 1 );
        writing_scale->res.vw = (uint16_t)IMGCAP_SIZE_NTSC_VW;
        writing_scale->res.hs = (uint16_t)IMGCAP_SIZE_NTSC_HS;
        writing_scale->res.hw = (uint16_t)IMGCAP_SIZE_NTSC_HW;
        /* v = vertical, h = horizontal, s = start, w = width */
#elif  VIDEO_FORMAT_TYPE == VIDEO_FORMAT_TYPE_PAL
        ASSERT_R( (color_system != GRPH_COL_SYS_NTSC_358) &&
                  (color_system != GRPH_COL_SYS_NTSC_443),
                  e=E_OTHERS; goto fin );

        writing_scale->res.vs = (uint16_t)( IMGCAP_SIZE_PAL_VS - 1 );
        writing_scale->res.vw = (uint16_t)IMGCAP_SIZE_PAL_VW;
        writing_scale->res.hs = (uint16_t)IMGCAP_SIZE_PAL_HS;
        writing_scale->res.hw = (uint16_t)IMGCAP_SIZE_PAL_HW;
#else
#error
#endif


        /* Write data parameter */
        writing_scale->res_pfil_sel = VDC5_ON;
        /* Prefilter mode select for brightness signals (on/off) */

        writing_scale->res_out_vw   = frame->height / 2;
        /* Number of valid lines in vertical direction */
        /* output by scaling-down control block */

        writing_scale->res_out_hw   = frame->width;
        /* Number of valid horizontal pixels */
        /* output by scaling-down control block */

        writing_scale->adj_sel      = VDC5_ON;
        /* Measures to decrease the influence */
        /* by lack of last-input line (on/off) */

        writing_scale->res_ds_wr_md = VDC5_WR_MD_NORMAL;
        /* Frame buffer writing mode */
    }


    /* From GRPDRV_VideoCreateNonDispSurface() */
    {
        writing_config.res_wrswa     = VDC5_WR_RD_WRSWA_16BIT;
        /* Frame buffer swap setting */

        writing_config.res_md        = VIDEO_FORMAT;
        /* Frame buffer video-signal writing format */

        writing_config.res_bst_md    = VDC5_BST_MD_32BYTE;
        /* Transfer burst length for frame buffer */

        writing_config.res_inter     = VDC5_RES_INTER_INTERLACE;
        /* Field operating mode select */

        writing_config.res_fs_rate   = VDC5_RES_FS_RATE_PER1;
        /* Writing rate */

        writing_config.res_fld_sel   = VDC5_RES_FLD_SEL_TOP;
        /* Write field select */

        writing_config.res_dth_on    = VDC5_ON;
        /* Dither correction on/off */

        writing_config.base          = (void *) frame_physical_address[0];
        /* Frame buffer base address */

        writing_config.ln_off        = frame->stride;
        /* Frame buffer line offset address [byte] */

        writing_config.flm_num       = (uint32_t)( frame->buffer_count - 1u );
        /* Number of frames of buffer (res_flm_num + 1) */

        writing_config.flm_off       = frame_physical_address[1] - frame_physical_address[0];

        writing_config.btm_base      = NULL;
        /* Frame buffer base address for bottom */


        e_vdc5 = R_VDC5_WriteDataControl( self->video_input_channel,
                                          self->data_control_ID,  &writing_config );
        IF ( e_vdc5 != VDC5_OK ) {
            e=E_OTHERS;
            goto fin;
        }

        self->is_data_control = true;
    }


    {
        vdc5_start_t  start;

        e_vdc5 = R_VDC5_StartProcess( self->video_input_channel, self->data_control_ID, &start );
        IF ( e_vdc5 != VDC5_OK ) {
            e = E_OTHERS;
            goto fin;
        }

        /* This module does not call "R_VDC5_StartProcess" function for reading (showing). */
    }


    {
        vdc5_int_t     *config = &self->interrupt_for_VDC5;
        bsp_int_err_t   ret_b;
#if INTERRUPT_FUNCTION_TYPE == INTERRUPT_FUNCTION_TYPE_OS_RTX
        vdc5_int_cb_t  *p_VDC5_ISR;
#endif

        e= R_VIDEO_INPUT_STATIC_AttachSelfToInterrupt(
               self->video_input_channel, self->data_control_ID, self );
        IF(e) {
            goto fin;
        }

        self->is_vsync_interrupt_registered[2] = true;


        if ( self->data_control_ID == VDC5_LAYER_ID_0_WR ) {
            config->type = VDC5_INT_TYPE_S0_VI_VSYNC;
        } else {
            config->type = VDC5_INT_TYPE_S1_VI_VSYNC;
        }
        if ( self->video_input_channel == 0 ) {
            config->callback = R_VIDEO_INPUT_OnVideoInVSync_Ch0;

            if ( self->data_control_ID == VDC5_LAYER_ID_0_WR ) {
                self->interrupt_ID = BSP_INT_SRC_S0_VI_VSYNC0;

#if INTERRUPT_FUNCTION_TYPE == INTERRUPT_FUNCTION_TYPE_OS_RTX
                p_VDC5_ISR = &gs_VDC5_ISR_S0_VI_VSYNC0;
                self->interrupt_vector = InterruptCallbackVector_S0_VI_VSYNC0;
#endif
            } else {
                self->interrupt_ID = BSP_INT_SRC_S1_VI_VSYNC0;

#if INTERRUPT_FUNCTION_TYPE == INTERRUPT_FUNCTION_TYPE_OS_RTX
                p_VDC5_ISR = &gs_VDC5_ISR_S1_VI_VSYNC0;
                self->interrupt_vector = InterruptCallbackVector_S1_VI_VSYNC0;
#endif
            }
        } else {
            config->callback = R_VIDEO_INPUT_OnVideoInVSync_Ch1;

#if INTERRUPT_FUNCTION_TYPE == INTERRUPT_FUNCTION_TYPE_OS_RTX
            if ( self->data_control_ID == VDC5_LAYER_ID_0_WR ) {
                self->interrupt_ID = BSP_INT_SRC_S0_VI_VSYNC1;

#if INTERRUPT_FUNCTION_TYPE == INTERRUPT_FUNCTION_TYPE_OS_RTX
                p_VDC5_ISR = &gs_VDC5_ISR_S0_VI_VSYNC1;
                self->interrupt_vector = InterruptCallbackVector_S0_VI_VSYNC1;
#endif
            } else {
                self->interrupt_ID = BSP_INT_SRC_S1_VI_VSYNC1;

#if INTERRUPT_FUNCTION_TYPE == INTERRUPT_FUNCTION_TYPE_OS_RTX
                p_VDC5_ISR = &gs_VDC5_ISR_S1_VI_VSYNC1;
                self->interrupt_vector = InterruptCallbackVector_S1_VI_VSYNC1;
#endif
            }
#endif
        }
        config->line_num = 0;

        e_vdc5 = R_VDC5_CallbackISR( self->video_input_channel, config );
        IF ( e_vdc5 != VDC5_OK ) {
            e=E_OTHERS;
            goto fin;
        }

        self->is_vsync_interrupt_registered[1] = true;


#if INTERRUPT_FUNCTION_TYPE == INTERRUPT_FUNCTION_TYPE_OS_LESS
        {
            vdc5_int_cb_t  a_VDC5_ISR;

            a_VDC5_ISR = R_VDC5_GetISR( self->video_input_channel, config->type );
            ret_b = R_BSP_InterruptWrite( self->interrupt_ID, a_VDC5_ISR );
            IF ( ret_b != BSP_INT_SUCCESS ) {
                e=E_OTHERS;
                goto fin;
            }
        }
#else
        *p_VDC5_ISR = R_VDC5_GetISR( self->video_input_channel, config->type );
        ret_b = R_BSP_InterruptWrite( self->interrupt_ID, self->interrupt_vector );
        IF ( ret_b != BSP_INT_SUCCESS ) {
            e=E_OTHERS;
            goto fin;
        }
#endif

        self->is_vsync_interrupt_registered[0] = true;
    }


#ifndef R_OSPL_NDEBUG
    printf( "VideoInput %dx%dx%dx%d stride=%d \n address[0]=0x%08X address[1]=0x%08X\n",
            frame->buffer_count, frame->width, frame->height, frame->byte_per_pixel,
            frame->stride,
            (uintptr_t) frame->buffer_address[0],
            (uintptr_t) frame->buffer_address[1] );
    /* Cast of "uintptr_t" is for avoiding "format" warning of GNU_ARM */
#endif


    e=0;
fin:
    if ( e != 0 ) {
        e= R_VIDEO_INPUT_Finalize( self, e );
    }
    return  e;
}


/***********************************************************************
* Implement: R_VIDEO_INPUT_Finalize
************************************************************************/
errnum_t  R_VIDEO_INPUT_Finalize( video_input_t *const  self, errnum_t e )
{
    errnum_t       ee;
    vdc5_error_t   e_vdc5;
    bsp_int_err_t  ret_b;

    if ( self->is_vsync_interrupt_registered[0] ) {
        ret_b = R_BSP_InterruptWrite( self->interrupt_ID, FIT_NO_FUNC );
        IF ( ret_b != BSP_INT_SUCCESS ) {
            e= R_OSPL_MergeErrNum( e, E_OTHERS );
        }

        self->is_vsync_interrupt_registered[0] = false;
    }


    if ( self->is_vsync_interrupt_registered[1] ) {
        vdc5_int_t  *config = &self->interrupt_for_VDC5;

        config->callback = NULL;
        config->line_num = 0;

        e_vdc5 = R_VDC5_CallbackISR( self->video_input_channel, config );
        IF ( e_vdc5 != VDC5_OK ) {
            e= R_OSPL_MergeErrNum( e, E_OTHERS );
        }

        config->type = VDC5_INT_TYPE_NUM;  /* Not Used interrupt type */

        self->is_vsync_interrupt_registered[1] = false;
    }


    if ( self->is_vsync_interrupt_registered[2] ) {
        ee= R_VIDEO_INPUT_STATIC_DetachSelfFromInterrupt(
                self->video_input_channel, self->data_control_ID, self );
        e= R_OSPL_MergeErrNum( e, ee );

        self->is_vsync_interrupt_registered[2] = false;
    }


    if ( self->is_data_control ) {
        e_vdc5 = R_VDC5_StopProcess( self->video_input_channel,
                                     self->data_control_ID );
        IF ( e_vdc5 != VDC5_OK ) {
            e= R_OSPL_MergeErrNum( e, E_OTHERS );
        }

        e_vdc5 = R_VDC5_ReleaseDataControl( self->video_input_channel,
                                            self->data_control_ID );
        IF ( e_vdc5 != VDC5_OK ) {
            e= R_OSPL_MergeErrNum( e, E_OTHERS );
        }

        self->is_data_control = false;
    }

    return  e;
}


/**
* @brief   Video input V-Sync interrupt handler.
*
* @param   channel_num channel_num.
* @param   int_type vdc5_int_type_t.
* @return  None.
*/
static void  R_VIDEO_INPUT_OnVideoInVSync( vdc5_channel_t  video_input_channel,  vdc5_int_type_t int_type )
{
    errnum_t        e;
    video_input_t  *self = NULL;
    /* NULL is for avoiding warning C4017W of mbed cloud compiler */

    e= R_VIDEO_INPUT_STATIC_GetSelfFromInterrupt( video_input_channel,  int_type,  &self );
    IF(e) {
        R_NOOP();
    }
    else if (
        int_type == VDC5_INT_TYPE_S0_VI_VSYNC  ||
        int_type == VDC5_INT_TYPE_S1_VI_VSYNC  ||
        int_type == VDC5_INT_TYPE_S0_WLINE  ||
        int_type == VDC5_INT_TYPE_S1_WLINE ) {
        if ( IS_BIT_SET( VDC51.SC0_SCL1_WR7, 0x00000001u ) ) {
            self->captured_buffer_index = 0;
        } else {
            self->captured_buffer_index = 1;
        }

        self->captured_count += 1;

        if ( self->captured_async != NULL ) {
            R_OSPL_EVENT_Set(
                self->captured_async->A_Thread,
                self->captured_async->A_EventValue );
        }
    }

    R_DEBUG_BREAK_IF_ERROR();
}


/**
* @brief   R_VIDEO_INPUT_OnVideoInVSync_Ch0
*
* @par Parameters
*    None
* @return  None.
*/
static void  R_VIDEO_INPUT_OnVideoInVSync_Ch0( vdc5_int_type_t int_type )
{
    R_VIDEO_INPUT_OnVideoInVSync( VDC5_CHANNEL_0, int_type );
}


/**
* @brief   R_VIDEO_INPUT_OnVideoInVSync_Ch1
*
* @par Parameters
*    None
* @return  None.
*/
static void  R_VIDEO_INPUT_OnVideoInVSync_Ch1( vdc5_int_type_t int_type )
{
    R_VIDEO_INPUT_OnVideoInVSync( VDC5_CHANNEL_1, int_type );
}


/**
* @brief   InterruptCallbackVector_S0_VI_VSYNC0
*
* @par Parameters
*    None
* @return  None.
*/
#if INTERRUPT_FUNCTION_TYPE == INTERRUPT_FUNCTION_TYPE_OS_RTX
static void  InterruptCallbackVector_S0_VI_VSYNC0(void)
{
    gs_VDC5_ISR_S0_VI_VSYNC0(0);
    GIC_EndInterrupt( BSP_INT_SRC_S0_VI_VSYNC0 );
}
#endif


/**
* @brief   InterruptCallbackVector_S0_VI_VSYNC1
*
* @par Parameters
*    None
* @return  None.
*/
#if INTERRUPT_FUNCTION_TYPE == INTERRUPT_FUNCTION_TYPE_OS_RTX
static void  InterruptCallbackVector_S0_VI_VSYNC1(void)
{
    gs_VDC5_ISR_S0_VI_VSYNC1(0);
    GIC_EndInterrupt( BSP_INT_SRC_S0_VI_VSYNC1 );
}
#endif


/**
* @brief   InterruptCallbackVector_S1_VI_VSYNC0
*
* @par Parameters
*    None
* @return  None.
*/
#if INTERRUPT_FUNCTION_TYPE == INTERRUPT_FUNCTION_TYPE_OS_RTX
static void  InterruptCallbackVector_S1_VI_VSYNC0(void)
{
    gs_VDC5_ISR_S1_VI_VSYNC0(0);
    GIC_EndInterrupt( BSP_INT_SRC_S1_VI_VSYNC0 );
}
#endif


/**
* @brief   InterruptCallbackVector_S1_VI_VSYNC1
*
* @par Parameters
*    None
* @return  None.
*/
#if INTERRUPT_FUNCTION_TYPE == INTERRUPT_FUNCTION_TYPE_OS_RTX
static void  InterruptCallbackVector_S1_VI_VSYNC1(void)
{
    gs_VDC5_ISR_S1_VI_VSYNC1(0);
    GIC_EndInterrupt( BSP_INT_SRC_S1_VI_VSYNC1 );
}
#endif


/**
* @brief   R_VIDEO_INPUT_STATIC_AttachSelfToInterrupt
*
* @param   in_video_input_channel vdc5_channel_t.
* @param   in_data_control_ID vdc5_layer_id_t.
* @param   in_self video_input_t*
* @return  Error Code. 0=No Error.
*/
static errnum_t  R_VIDEO_INPUT_STATIC_AttachSelfToInterrupt(
    vdc5_channel_t  in_video_input_channel,  vdc5_layer_id_t  in_data_control_ID,
    video_input_t  *in_self )
{
    errnum_t         e;
    video_input_t  **self_pp = NULL;
    /* NULL is for avoiding warning C4017W of mbed cloud compiler */

    e= R_VIDEO_INPUT_STATIC_GetSelfPointer(
           in_video_input_channel,  in_data_control_ID,
           &self_pp );
    IF(e) {
        goto fin;
    }

    ASSERT_R( *self_pp == NULL,  e=E_OTHERS; goto fin );

    *self_pp = in_self;

    e=0;
fin:
    return  e;
}


/**
* @brief   R_VIDEO_INPUT_STATIC_DetachSelfFromInterrupt
*
* @param   in_video_input_channel vdc5_channel_t.
* @param   in_data_control_ID vdc5_layer_id_t.
* @param   in_self video_input_t*
* @return  Error Code. 0=No Error.
*/
static errnum_t  R_VIDEO_INPUT_STATIC_DetachSelfFromInterrupt(
    vdc5_channel_t  in_video_input_channel,  vdc5_layer_id_t  in_data_control_ID,
    video_input_t  *in_self )
{
    errnum_t         e;
    video_input_t  **self_pp = NULL;
    /* NULL is for avoiding warning C4017W of mbed cloud compiler */

    e= R_VIDEO_INPUT_STATIC_GetSelfPointer(
           in_video_input_channel,  in_data_control_ID,
           &self_pp );
    IF(e) {
        goto fin;
    }

    ASSERT_R( *self_pp == in_self,  e=E_OTHERS; goto fin );

    *self_pp = NULL;

    e=0;
fin:
    return  e;
}


/**
* @brief   R_VIDEO_INPUT_STATIC_GetSelfFromInterrupt
*
* @param   in_interrupt vdc5_int_type_t.
* @param   in_self video_input_t*
* @return  Error Code. 0=No Error.
*/
static errnum_t  R_VIDEO_INPUT_STATIC_GetSelfFromInterrupt(
    vdc5_channel_t  in_video_input_channel,  vdc5_int_type_t  in_interrupt,
    video_input_t  **out_self )
{
    errnum_t         e;
    vdc5_layer_id_t  data_control_ID = VDC5_LAYER_ID_0_WR;
    /* VDC5_LAYER_ID_0_WR is for avoiding warning C4017W of mbed cloud compiler */
    video_input_t  **self_pp = NULL;
    /* NULL is for avoiding warning C4017W of mbed cloud compiler */


    switch ( in_interrupt ) {
        case  VDC5_INT_TYPE_S0_VI_VSYNC:
            data_control_ID = VDC5_LAYER_ID_0_WR;
            break;

        case  VDC5_INT_TYPE_S1_VI_VSYNC:
            data_control_ID = VDC5_LAYER_ID_1_WR;
            break;

        default:
            ASSERT_R( false,  e=E_OTHERS;  goto fin );
    }


    e= R_VIDEO_INPUT_STATIC_GetSelfPointer(
           in_video_input_channel,  data_control_ID,  &self_pp );
    IF(e) {
        goto fin;
    }

    *out_self = *self_pp;

    e=0;
fin:
    return  e;
}


/**
* @brief   R_VIDEO_INPUT_STATIC_GetSelfPointer
*
* @param   in_video_input_channel vdc5_channel_t.
* @param   in_data_control_ID vdc5_layer_id_t.
* @param   out_self_pp video_input_t*
* @return  Error Code. 0=No Error.
*/
static errnum_t  R_VIDEO_INPUT_STATIC_GetSelfPointer(
    vdc5_channel_t  in_video_input_channel,  vdc5_layer_id_t  in_data_control_ID,
    video_input_t ***  out_self_pp )
{
    errnum_t      e;
    int_fast32_t  video_input_index;
    int_fast32_t  data_control_index;


    if ( in_video_input_channel == VDC5_CHANNEL_0 ) {
        video_input_index = 0;
    } else {
        ASSERT_D( in_video_input_channel == VDC5_CHANNEL_1,  e=E_OTHERS; goto fin );

        video_input_index = 1;
    }

    if ( in_data_control_ID == VDC5_LAYER_ID_0_WR ) {
        data_control_index = 0;
    } else {
        ASSERT_D( in_data_control_ID == VDC5_LAYER_ID_1_WR,  e=E_OTHERS; goto fin );

        data_control_index = 1;
    }

    *out_self_pp = &gs_InterruptToSelf[ video_input_index ][ data_control_index ];

    e=0;
#ifndef  R_OSPL_NDEBUG
fin:
#endif
    return  e;
}


