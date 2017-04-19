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
* Copyright (C) 2012 - 2014 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**
* @file  video_input.hpp
* @brief   $Module: CLibCommon $ $PublicVersion: 1.00 $ (=CLIB_VERSION)
* $Rev: 47 $
* $Date:: 2014-01-06 18:18:48 +0900#$
* - Description: Video Input for mbed style C++ API
*/

#ifndef  VIDEO_INPUT_HPP
#define  VIDEO_INPUT_HPP

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_ospl_typedef.h"
#include  "video_input_typedef.h"


/******************************************************************************
Typedef definitions
******************************************************************************/

class  VideoInputConfig;


/***********************************************************************
* Class: VideoInput
*    mbed style C++ version of <video_input_t>
************************************************************************/
class  VideoInput {
public:
	/**
* @brief   VideoInput
*
* @par Parameters
*    None
* @return  None.
*/
	VideoInput();
	~VideoInput();

	/**
* @brief   initialize
*
* @par Parameters
*    None
* @return  None.
*/
	errnum_t  initialize( VideoInputConfig& in_out_Config );

	/**
* @brief   destroy
*
* @par Parameters
*    None
* @return  None.
*/
	void  destroy();

private:
	video_input_t*  _self;
};


/***********************************************************************
* Class: VideoInputConfig
*    mbed style C++ version of <video_input_config_t>
************************************************************************/
class  VideoInputConfig : public video_input_config_t {
public:
	/**
* @brief   VideoInputConfig
*
* @par Parameters
*    None
* @return  None.
*/
	VideoInputConfig();
};


/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/

#endif
