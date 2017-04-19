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
* $FileName: RGA_Port.h $
* $Module: RGA $ $PublicVersion: 1.03 $ (=RGA_VERSION)
* $Rev: $
* $Date::                           $
* Description:
******************************************************************************/

#ifndef  RGA_PORT_H
#define  RGA_PORT_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "RGA_API_typedef.h"
#include  "RGA_Port_typedef.h"
#include  "clib_drivers_typedef.h"  /* "R_CEIL_8U" in "R_RGA_CalcWorkBufferSize" */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Typedef definitions
******************************************************************************/
/* in RGA_Port_typedef.h */

/******************************************************************************
Macro definitions
******************************************************************************/
/* in RGA_Port_typedef.h */

/******************************************************************************
Variable Externs
******************************************************************************/
/* in RGA_Port_typedef.h */

/******************************************************************************
Functions Prototypes
******************************************************************************/
errnum_t  drawCLUTImage( uint8_t *SrcAddress, const graphics_image_t *image, frame_buffer_t *frame );


/***********************************************************************
* Class: vram_ex_stack_t
************************************************************************/
errnum_t  R_VRAM_EX_STACK_Initialize( vram_ex_stack_t *self, void *NullConfig );
errnum_t  R_VRAM_EX_STACK_Alloc( vram_ex_stack_t *self, frame_buffer_t *in_out_FrameBuffer );
errnum_t  R_VRAM_EX_STACK_Free( vram_ex_stack_t *self, frame_buffer_t *frame_buffer );


/* Section: Global */
/**
* @brief   Calculate size of hardware work buffer
*
* @param   max_height_of_frame_buffer int
* @return  size of hardware work buffer
*/
#ifdef  RGAH_VERSION

#define  R_RGA_CalcWorkBufferSize( max_height_of_frame_buffer ) \
	( (size_t)( RGA_WORK_BUFFER_MAX_DISPLAY_LIST + \
		RGA_WORK_BUFFER_STRIDE * R_CEIL_8U( max_height_of_frame_buffer ) * 4 * 2 ) )
R_STATIC_ASSERT_GLOBAL( RGA_WORK_BUFFER_HEIGHT_ALIGNMENT == 8, "" );  /* check of R_CEIL_8U */

#endif


/**
* @brief   Calculate size of hardware work buffer
*
* @param   max_width_of_frame_buffer int
* @param   max_height_of_frame_buffer int
* @return  size of hardware work buffer
*/
#ifdef  RGAH_VERSION

#define  R_RGA_CalcWorkBufferSize2( max_width_of_frame_buffer, max_height_of_frame_buffer ) \
	R_RGA_CalcWorkBufferSize( max_height_of_frame_buffer )
#else

#define  R_RGA_CalcWorkBufferSize2( max_width_of_frame_buffer, max_height_of_frame_buffer ) \
	( (size_t)( (max_width_of_frame_buffer) * RGA_WORK_BUFFER_DMAC_LM_WIDTH_BYTE + \
		(max_height_of_frame_buffer) * RGA_WORK_BUFFER_DMAC_LM_HEIGHT_BYTE ) )
#endif


/**
* @brief   Calculate size of hardware work buffer B
*
* @param   MaxWidthOfJPEG int
* @param   MaxHeightOfJPEG int
* @param   MaxBytePerPixelOfFrameBuffer int
* @return  size of hardware work buffer B
*/
#define  R_RGA_CalcWorkBufferB_Size( MaxWidthOfJPEG, MaxHeightOfJPEG, MaxBytePerPixelOfFrameBuffer ) \
	( R_CEIL_16U( MaxWidthOfJPEG ) * R_CEIL_16U( MaxHeightOfJPEG ) * (MaxBytePerPixelOfFrameBuffer) )
R_STATIC_ASSERT_GLOBAL( RGA_JPEG_MAX_WIDTH_ALIGNMENT  == 16, "" );  /* check of R_CEIL_16U */
R_STATIC_ASSERT_GLOBAL( RGA_JPEG_MAX_HEIGHT_ALIGNMENT == 16, "" );  /* check of R_CEIL_16U */


#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

/* Inline Functions */
#include  "RGA_Port_inline.h"

#endif  /* RGA_PORT_H */

