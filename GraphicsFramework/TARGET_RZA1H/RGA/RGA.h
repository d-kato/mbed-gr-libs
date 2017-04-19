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
* Copyright (C) 2013 - 2014 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/******************************************************************************
* $FileName: RGA.h $
* $Module: RGA $ $PublicVersion: 1.20 $ (=RGA_VERSION)
* $Rev: $
* $Date::                           $
* Description:
******************************************************************************/


/* This is MAIN header file */

#ifndef  RGA_H
#define  RGA_H

#include  "Project_Config.h"


#ifdef _MSC_VER
	#include  "PC/RGA_Sub_PC.h"
	#include  "sub/bitblt_alpha.h"
	#include  "PC/RGA_PC.h"
#endif
#ifdef _SH
	#include  "SH7269/RGA_Sub_SH7269.h"
	#include  "sub/bitblt_alpha.h"
	#include  "SH7269/RGA_SH.h"
#endif
#ifdef R_ARM
	#include  "r_ospl.h"
	#include  "r_typedefs.h"
	#include  "RGA_Port.h"
#endif

#include  "RGA_API.h"
#ifdef  __cplusplus
	#include  "RGA_Cpp.h"
#endif


/* Sample window surfaces library */
#ifdef _MSC_VER
	#include  <PC/RGA_SampleLib_RZ_A1H.h>
#endif
#ifdef _SH
	#include  <SH7269/RGA_SampleLib_RZ_A1H.h>
#endif
#ifdef R_ARM
	#include  "RGA_raw_image.h"
	#include  "window_surfaces.h"
	#include  "RGA_Port.h"
#endif


#endif  /* RGA_H */


