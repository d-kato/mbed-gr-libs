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
* @file  RGA_API.h
* @brief   $Module: RGA $ $PublicVersion: 1.20 $ (=RGA_VERSION)
* $Rev: $
* $Date::                           $
*/

#ifndef  RGA_API_H
#define  RGA_API_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "RGA_API_typedef.h"
#include  "frame_buffer_typedef.h"

#ifdef  __cplusplus
extern "C" {
#endif



/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/


/***********************************************************************
* Functions: R_RGA
************************************************************************/
bool_t  R_RGA_IsSupportedPixelFormatForRGAH( pixel_format_t pixel_format );


/***********************************************************************
* Class: graphics_image_t
************************************************************************/
errnum_t  R_GRAPHICS_IMAGE_InitR8G8B8A8(
    graphics_image_t *self, void *ImageDataArray, size_t ImageDataArraySize,
    int_fast32_t width, int_fast32_t height );
errnum_t  R_GRAPHICS_IMAGE_InitSameSizeR8G8B8A8(
    graphics_image_t *self, void *ImageDataArray, size_t ImageDataArraySize,
    graphics_image_t *SameSizeImage );
errnum_t  R_GRAPHICS_IMAGE_InitCopyFrameBufferR8G8B8A8(
    graphics_image_t *self, void *ImageDataArray, size_t ImageDataArraySize,
    graphics_t *Context, int_fast32_t MinX,  int_fast32_t MinY,  int_fast32_t width,  int_fast32_t height );
errnum_t  R_GRAPHICS_IMAGE_InitByShareFrameBuffer( graphics_image_t *self, frame_buffer_t *frame_buffer );
errnum_t  R_GRAPHICS_IMAGE_GetProperties( const graphics_image_t *self, graphics_image_properties_t *out_Properties );
errnum_t  R_GRAPHICS_IMAGE_GetImageFormat( const graphics_image_t *self, pixel_format_t *out_Format );


/***********************************************************************
* Class: graphics_pattern_t
************************************************************************/
errnum_t  R_GRAPHICS_PATTERN_Initialize( graphics_pattern_t *self,
        const graphics_image_t *image,  repetition_t repetition,  graphics_t *Context );


/***********************************************************************
* Class: graphics_t
************************************************************************/
void      R_GRAPHICS_InitConst( graphics_t *self );
errnum_t  R_GRAPHICS_Initialize( graphics_t *self, graphics_config_t *Config );
errnum_t  R_GRAPHICS_Finalize( graphics_t *self, errnum_t e );

errnum_t  R_GRAPHICS_SetFrameBuffer( graphics_t *self, frame_buffer_t *frame_buffer );
errnum_t  R_GRAPHICS_GetFrameBuffer( graphics_t *self, frame_buffer_t **out_frame_buffer );
errnum_t  R_GRAPHICS_Finish( graphics_t *self );
errnum_t  R_GRAPHICS_FinishStart( graphics_t *self, r_ospl_async_t *Async );
errnum_t  R_GRAPHICS_GetAsyncStatus( graphics_t *self, const graphics_async_status_t **out_Status );
errnum_t  R_GRAPHICS_OnInterrupting(void);

errnum_t  R_GRAPHICS_Save( graphics_t *self, graphics_status_t *out_Status );
errnum_t  R_GRAPHICS_Restore( graphics_t *self, graphics_status_t *Status, errnum_t e );

errnum_t  R_GRAPHICS_ResetMatrix( graphics_t *self );
errnum_t  R_GRAPHICS_SetMatrix_2x3( graphics_t *self,  graphics_matrix_float_t sx,  graphics_matrix_float_t ky,
                                    graphics_matrix_float_t kx,  graphics_matrix_float_t sy,  graphics_matrix_float_t tx,  graphics_matrix_float_t ty );
errnum_t  R_GRAPHICS_SetMatrix_3x3( graphics_t *self,  graphics_matrix_float_t *Matrix );
errnum_t  R_GRAPHICS_GetMatrix_3x3( graphics_t *self,  graphics_matrix_float_t *out_Matrix );
errnum_t  R_GRAPHICS_TranslateMatrixI( graphics_t *self, int_fast32_t tx, int_fast32_t ty );
errnum_t  R_GRAPHICS_TranslateMatrix( graphics_t *self, graphics_matrix_float_t tx, graphics_matrix_float_t ty );
errnum_t  R_GRAPHICS_ScaleMatrix( graphics_t *self, graphics_matrix_float_t sx, graphics_matrix_float_t sy );
errnum_t  R_GRAPHICS_RotateMatrixDegree( graphics_t *self, graphics_matrix_float_t degree );
errnum_t  R_GRAPHICS_ShearMatrix( graphics_t *self, graphics_matrix_float_t shx, graphics_matrix_float_t shy );
errnum_t  R_GRAPHICS_TransformMatrix( graphics_t *self,  graphics_matrix_float_t sx,  graphics_matrix_float_t ky,
                                      graphics_matrix_float_t kx,  graphics_matrix_float_t sy,  graphics_matrix_float_t tx,  graphics_matrix_float_t ty );
errnum_t  R_GRAPHICS_MultiplyMatrix( graphics_t *self,  graphics_matrix_float_t *Matrix );
errnum_t  R_GRAPHICS_GetProjectiveMatrix(
    graphics_matrix_float_t SourceTopLeftX,           graphics_matrix_float_t SourceTopLeftY,
    graphics_matrix_float_t SourceTopRightX,          graphics_matrix_float_t SourceTopRightY,
    graphics_matrix_float_t SourceBottomLeftX,        graphics_matrix_float_t SourceBottomLeftY,
    graphics_matrix_float_t SourceBottomRightX,       graphics_matrix_float_t SourceBottomRightY,
    graphics_matrix_float_t DestinationTopLeftX,      graphics_matrix_float_t DestinationTopLeftY,
    graphics_matrix_float_t DestinationTopRightX,     graphics_matrix_float_t DestinationTopRightY,
    graphics_matrix_float_t DestinationBottomLeftX,   graphics_matrix_float_t DestinationBottomLeftY,
    graphics_matrix_float_t DestinationBottomRightX,  graphics_matrix_float_t DestinationBottomRightY,
    graphics_matrix_float_t *out_Matrix );

errnum_t  R_GRAPHICS_SetBackgroundColor( graphics_t *self, r8g8b8a8_t Color );
errnum_t  R_GRAPHICS_GetBackgroundColor( graphics_t *self, r8g8b8a8_t *out_Color );
errnum_t  R_GRAPHICS_GetClearColor( graphics_t *self, r8g8b8a8_t *out_Color );
errnum_t  R_GRAPHICS_Clear( graphics_t *self,
                            int_fast32_t MinX, int_fast32_t MinY, int_fast32_t width, int_fast32_t height );

errnum_t  R_GRAPHICS_DrawImage( graphics_t *self,  const graphics_image_t *image,
                                int_fast32_t MinX,  int_fast32_t MinY );
errnum_t  R_GRAPHICS_DrawImageResized( graphics_t *self,  const graphics_image_t *image,
                                       int_fast32_t MinX,  int_fast32_t MinY,  int_fast32_t width,  int_fast32_t height );
errnum_t  R_GRAPHICS_DrawImageChild( graphics_t *self,  const graphics_image_t *image,
                                     int_fast32_t SourceMinX,        int_fast32_t SourceMinY,
                                     int_fast32_t SourceWidth,       int_fast32_t SourceHeight,
                                     int_fast32_t DestinationMinX,   int_fast32_t DestinationMinY,
                                     int_fast32_t DestinationWidth,  int_fast32_t DestinationHeight );

errnum_t  R_GRAPHICS_FillRect( graphics_t *self,  int_fast32_t MinX,  int_fast32_t MinY,
                               int_fast32_t width,  int_fast32_t height );
errnum_t  R_GRAPHICS_SetFillColor( graphics_t *self, r8g8b8a8_t Color );
errnum_t  R_GRAPHICS_SetFillPattern( graphics_t *self, graphics_pattern_t *Pattern );

errnum_t  R_GRAPHICS_StrokeRect( graphics_t *self,  int_fast32_t MinX,  int_fast32_t MinY,
                                 int_fast32_t width,  int_fast32_t height );
errnum_t  R_GRAPHICS_SetStrokeColor( graphics_t *self, r8g8b8a8_t Color );

errnum_t  R_GRAPHICS_BeginPath( graphics_t *self );
errnum_t  R_GRAPHICS_Rect( graphics_t *self,  int_t MinX,  int_t MinY,
                           int_t width,  int_t height );
errnum_t  R_GRAPHICS_Cilp( graphics_t *self );

errnum_t  R_GRAPHICS_SetGlobalAlpha( graphics_t *self, uint8_t AlphaValue );
errnum_t  R_GRAPHICS_GetGlobalAlpha( graphics_t *self, uint8_t *out_AlphaValue );
errnum_t  R_GRAPHICS_SetGlobalCompositeOperation( graphics_t *self,
        graphics_composite_operation_t CompositeOperation );
errnum_t  R_GRAPHICS_GetGlobalCompositeOperation( graphics_t *self,
        graphics_composite_operation_t *out_CompositeOperation );

errnum_t  R_GRAPHICS_SetQualityFlags( graphics_t *self,
                                      graphics_quality_flags_t Qualities );
errnum_t  R_GRAPHICS_GetQualityFlags( graphics_t *self,
                                      graphics_quality_flags_t *out_Qualities );


/***********************************************************************
* Class: graphics_static_t
************************************************************************/
graphics_static_t  *get_graphics_static(void);
errnum_t  R_GRAPHICS_STATIC_GetVersion( uint32_t *out_Version );
errnum_t  R_GRAPHICS_STATIC_OnInitializeDefault( graphics_t *self, graphics_config_t *in_out_Config,
        void **out_CalleeDefined );
errnum_t  R_GRAPHICS_STATIC_OnFinalizeDefault( graphics_t *self, void *CalleeDefined, errnum_t e );
errnum_t  R_GRAPHICS_STATIC_SetOnInitialize(
    R_GRAPHICS_OnInitialize_FuncType CallbackFunction );
errnum_t  R_GRAPHICS_STATIC_SetOnFinalize(
    R_GRAPHICS_OnFinalize_FuncType CallbackFunction );


/***********************************************************************
* Class: RGA_SHS
*    Synchronize hardware and software rendering
************************************************************************/
errnum_t  R_GRAPHICS_BeginSoftwareRendering( graphics_t *self );
errnum_t  R_GRAPHICS_BeginSoftwareRendering2( graphics_t *self );
errnum_t  R_GRAPHICS_BeginSoftwareRenderingA( graphics_t *self, void *Address );
errnum_t  R_GRAPHICS_EndSoftwareRendering( graphics_t *self );
errnum_t  R_GRAPHICS_EndRenderingInFin( graphics_t *self, errnum_t e );


/***********************************************************************
* Class: animation_timing_function_t
************************************************************************/
errnum_t  R_Get_AnimationTimingFunction( char_t *TimingName,
        animation_timing_function_t **out_Timing );

float32_t  R_ANIMATION_TIMING_FUNCTION_GetValue( animation_timing_function_t *self,
        float32_t ClampTime, float32_t ValueOfPreviousKeyFrame, float32_t ValueOfNextKeyFrame );


/***********************************************************************
* Functions: Self_Test_for_Package
************************************************************************/
errnum_t  R_RGA_CheckStructSize( size_t SizeOf_GraphicsClass, size_t SizeOf_GraphicsStatusClass,
                                 size_t Zero );


#ifdef  __cplusplus
}  /* extern "C" */
#endif

#endif  /* RGA_API_H */
