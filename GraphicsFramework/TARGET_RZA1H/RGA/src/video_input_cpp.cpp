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
* @file  video_input_cpp.cpp
* @brief   $Rev: 51 $
* $Date:: 2014-03-14 18:42:33 +0900#$
*/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include  "video_input.hpp"
#include  "video_input.h"
#include  "r_ospl.h"


/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
#define  GS_DEFAULT_INT_VALUE  -1


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

/***********************************************************************
* ClassImplement: VideoInput
************************************************************************/

/***********************************************************************
* Implement: VideoInput
************************************************************************/
VideoInput::VideoInput()
{
    this->_self = NULL;
}


/***********************************************************************
* Implement: ~VideoInput
************************************************************************/
VideoInput::~VideoInput()
{
    this->destroy();
}


/***********************************************************************
* Implement: initialize
************************************************************************/
errnum_t  VideoInput::initialize( VideoInputConfig &in_out_config )
{
    this->_self = new  video_input_t;
    ASSERT_R( this->_self != NULL,  return  E_FEW_MEMORY );

    in_out_config.flags =
        VIDEO_INPUT_CONFIG_T_VIDEO_INPUT_CHANNEL_NUM |
        VIDEO_INPUT_CONFIG_T_DISPLAY_CHANNEL_NUM |
        VIDEO_INPUT_CONFIG_T_DISPLAY_LAYER_NUM |
        VIDEO_INPUT_CONFIG_T_FRAME_BUFFER |
        VIDEO_INPUT_CONFIG_T_CAPTURED_ASYNC;

    if ( in_out_config.video_input_channel_num == GS_DEFAULT_INT_VALUE ) {
        in_out_config.flags &= ~VIDEO_INPUT_CONFIG_T_VIDEO_INPUT_CHANNEL_NUM;
    }
    if ( in_out_config.display_channel_num == GS_DEFAULT_INT_VALUE ) {
        in_out_config.flags &= ~VIDEO_INPUT_CONFIG_T_DISPLAY_CHANNEL_NUM;
    }
    if ( in_out_config.display_layer_num == GS_DEFAULT_INT_VALUE ) {
        in_out_config.flags &= ~VIDEO_INPUT_CONFIG_T_DISPLAY_LAYER_NUM;
    }
    if ( in_out_config.frame_buffer == NULL ) {
        in_out_config.flags &= ~VIDEO_INPUT_CONFIG_T_FRAME_BUFFER;
    }
    if ( in_out_config.captured_async == NULL ) {
        in_out_config.flags &= ~VIDEO_INPUT_CONFIG_T_CAPTURED_ASYNC;
    }

    return  R_VIDEO_INPUT_Initialize( this->_self,  (video_input_config_t *) &in_out_config );
}


/***********************************************************************
* Implement: destroy
************************************************************************/
void  VideoInput::destroy()
{
    if ( this->_self != NULL ) {
        errnum_t  e = R_VIDEO_INPUT_Finalize( this->_self,  0 );
        ASSERT_R( e == 0,  R_NOOP() );
    }
}


/***********************************************************************
* ClassImplement: VideoInputConfig
************************************************************************/

/***********************************************************************
* Implement: VideoInputConfig
************************************************************************/
VideoInputConfig::VideoInputConfig()
{
    this->flags = 0;
    this->video_input_channel_num = GS_DEFAULT_INT_VALUE;
    this->display_channel_num = GS_DEFAULT_INT_VALUE;
    this->display_layer_num = GS_DEFAULT_INT_VALUE;
    this->frame_buffer = NULL;
    this->captured_async = NULL;
}


