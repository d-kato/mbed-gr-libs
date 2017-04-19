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
* Copyright (C) 2013 - 2015 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**
* @file  RGA_Callback_private.h
* @brief   $Module: RGA $ $PublicVersion: 1.20 $ (=RGA_VERSION)
* $Rev: $
* $Date::                           $
*/

#include  "r_typedefs.h"
#include  "RGA_Port_typedef.h"

#ifdef  RZ_A1L  /* Reduce working buffer size for LibPNG */
#define  WORK_SIZE_FOR_LIB_PNG   0x10000
#endif

enum {
    MAX_WIDTH_OF_FRAME_BUFFER  = 1280,
    MAX_HEIGHT_OF_FRAME_BUFFER = 800,

    WORK_BUFFER_SIZE = R_RGA_CalcWorkBufferSize2(
                           MAX_WIDTH_OF_FRAME_BUFFER, MAX_HEIGHT_OF_FRAME_BUFFER ),

#ifdef  WORK_SIZE_FOR_LIB_PNG
    WORK_BUFFER_B_SIZE = R_RGA_CalcWorkBufferB_Size(
                             MAX_WIDTH_OF_FRAME_BUFFER, MAX_HEIGHT_OF_FRAME_BUFFER, 4 ) +
                         WORK_SIZE_FOR_LIB_PNG
#else
    WORK_BUFFER_B_SIZE = R_RGA_CalcWorkBufferB_Size(
                             MAX_WIDTH_OF_FRAME_BUFFER, MAX_HEIGHT_OF_FRAME_BUFFER, 4 ) +
                         RGA_MAX_WORK_SIZE_FOR_LIB_PNG_DEFAULT
#endif
};

void  R_MEMORY_SECTION_GetMemory_RGA_WorkBuffer(
    uint8_t **out_WorkBufferAddress,  size_t *out_WorkBufferSize,
    uint8_t **out_WorkBufferB_Address,  size_t *out_WorkBufferB_Size );

