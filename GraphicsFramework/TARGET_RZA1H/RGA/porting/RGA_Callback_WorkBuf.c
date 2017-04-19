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
* @file  RGA_Callback_WorkBuf.c
* @brief   $Module: RGA $ $PublicVersion: 1.20 $ (=RGA_VERSION)
* $Rev: $
* $Date::                           $
*/

#include  <string.h>
#include  "RGA.h"
#include  "RGA_Callback_private.h"


/** gs_WorkBufferMemory */

/* <MapSection attribute="Uncachable"/> */
/* .WORK_SECTION1 is for only R_REE_INIT. This is ignored in other environments. */
#if defined(__ICCARM__)
R_OSPL_ALIGNMENT( 16,
                  R_OSPL_SECTION_FOR_ZERO_INIT( ".mirrorram",
                          static uint8_t  gs_WorkBufferMemory[ WORK_BUFFER_SIZE ] ));
#else
R_OSPL_ALIGNMENT( RGA_WORK_BUFFER_ADDRESS_ALIGNMENT,
                  R_OSPL_SECTION_FOR_ZERO_INIT( "NC_BSS",
                          static uint8_t  gs_WorkBufferMemory[ WORK_BUFFER_SIZE ] ));
#endif


/** gs_WorkBufferB_Memory */

R_OSPL_ALIGNMENT( RGA_WORK_BUFFER_B_ADDRESS_ALIGNMENT,
                  static uint8_t  gs_WorkBufferB_Memory[ WORK_BUFFER_B_SIZE ] );


/**
* @brief   Get Memory of RGA WorkBuffer
*
* @param   out_WorkBufferAddress uint8_t*
* @param   out_WorkBufferSize size_t
* @param   out_WorkBufferB_Address uint8_t*
* @param   out_WorkBufferB_Size size_t
* @return  None
*/
void  R_MEMORY_SECTION_GetMemory_RGA_WorkBuffer(
    uint8_t **out_WorkBufferAddress,  size_t *out_WorkBufferSize,
    uint8_t **out_WorkBufferB_Address,  size_t *out_WorkBufferB_Size )
{
    *out_WorkBufferAddress = gs_WorkBufferMemory;
    *out_WorkBufferSize = sizeof( gs_WorkBufferMemory );
    *out_WorkBufferB_Address = gs_WorkBufferB_Memory;
    *out_WorkBufferB_Size = sizeof( gs_WorkBufferB_Memory );
}


