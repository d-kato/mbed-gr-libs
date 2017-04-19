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
/**
* @file  RGA_Config.h
* @brief   $Module: RGA $ $PublicVersion: 1.20 $ (=RGA_VERSION)
* $Rev: $
* $Date::                           $
*/


/* This file is included from "Project_Config.h" */


#ifndef  RGA_CONFIG_H
#define  RGA_CONFIG_H


/**
* @def  RGA_VERSION
* @brief  RGA_VERSION
* @par Parameters
*    None
* @return  None.
*/
#define  RGA_VERSION  120


/**
* @def  RGA_VERSION_STRING
* @brief  RGA_VERSION_STRING
* @par Parameters
*    None
* @return  None.
*/
#define  RGA_VERSION_STRING  "1.20"


/**
* @def  GSCE_VERSION
* @brief  GSCE_VERSION
* @par Parameters
*    None
* @return  None.
*/
#define  GSCE_VERSION  402


/**
* @def  RGAH_VERSION
* @brief  RGAH_VERSION
* @par Parameters
*    None
* @return  None.
*/
/**
* @def  RGAS_VERSION
* @brief  RGAS_VERSION
* @par Parameters
*    None
* @return  None.
*/

#ifdef R_ARM
#define  IS_RGAH_ARM_VERSION
/* #define  IS_RGAS_ARM_VERSION */
#endif

#ifdef _SH
#define  IS_RGAH_SH_VERSION
/* #define  IS_RGAS_SH_VERSION */
#endif

#ifdef _MSC_VER
/* #define  IS_RGAH_PC_VERSION */
#define  IS_RGAS_PC_VERSION
#endif

#if defined( IS_RGAH_ARM_VERSION ) || defined( IS_RGAH_SH_VERSION ) || defined( IS_RGAH_PC_VERSION )
#define  RGAH_VERSION  103
#endif

#if defined( IS_RGAS_ARM_VERSION ) || defined( IS_RGAS_SH_VERSION ) || defined( IS_RGAS_PC_VERSION )
#define  RGAS_VERSION  203
#endif

#undef  IS_RGAS_ARM_VERSION
#undef  IS_RGAH_ARM_VERSION
#undef  IS_RGAS_SH_VERSION
#undef  IS_RGAH_SH_VERSION
#undef  IS_RGAS_PC_VERSION
#undef  IS_RGAH_PC_VERSION
/* Use "R_ARM" or "RGAH_VERSION" */


/**
* @def  IS_WINDOW_SURFACES_EX
* @brief  define or not define
* @par Parameters
*    None
* @return  None.
*/
#define  IS_WINDOW_SURFACES_EX


/***********************************************************************
* Macros: RGA_LIMITATION_*
*    0 or 1
************************************************************************/
#ifdef  RGAH_VERSION
#define  RGA_LIMITATION_01_MATRIXED_IMAGE_STRIDE  1
#define  RGA_LIMITATION_02_XRGB8888_BLEND         0
#define  RGA_LIMITATION_03_INIT_BMP               1
#define  RGA_LIMITATION_04_AYNCHRONOOUS_JCU       1
#define  RGA_LIMITATION_06_COLOR_LITTLE_ERROR     1
#define  RGA_LIMITATION_07_DRAWIMAGE_VERTEX_ERR   1
#define  RGA_LIMITATION_08_OPTIMIZATION_ERR       0
#define  RGA_LIMITATION_09_RGAH_BITBLT_CLIP       1
#define  RGA_LIMITATION_11_SOURCE_FLUSH_B         0
#else
#define  RGA_LIMITATION_01_MATRIXED_IMAGE_STRIDE  0
#define  RGA_LIMITATION_02_XRGB8888_BLEND         0
#define  RGA_LIMITATION_03_INIT_BMP               0
#define  RGA_LIMITATION_04_AYNCHRONOOUS_JCU       1
#define  RGA_LIMITATION_06_COLOR_LITTLE_ERROR     0
#define  RGA_LIMITATION_07_DRAWIMAGE_VERTEX_ERR   1
#define  RGA_LIMITATION_08_OPTIMIZATION_ERR       0
#define  RGA_LIMITATION_09_RGAH_BITBLT_CLIP       0
#define  RGA_LIMITATION_11_SOURCE_FLUSH_B         0
#endif


#endif  /* __RGA_CONFIG_H */


