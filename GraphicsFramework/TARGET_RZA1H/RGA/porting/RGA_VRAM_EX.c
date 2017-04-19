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
* @file  RGA_VRAM_EX.c
* @brief   External VRAM Area
*
* $Module: CLibCommon $ $PublicVersion: 1.00 $ (=CLIB_VERSION)
* $Rev: 51 $
* $Date:: 2014-03-14 18:42:33 +0900#$
*/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "Project_Config.h"
#include  "r_typedefs.h"
#include  "window_surfaces_private.h"
#include  "r_ospl.h"


/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

/** gs_VRAM_Ex */
/* <MapSection attribute="SDRAM_Cached"/> */
R_OSPL_ALIGNMENT( RGA_STACK_ADDRESS_ALIGNMENT,
                  R_OSPL_SECTION_FOR_ZERO_INIT( "VRAM_EX",
                          static uint8_t gs_VRAM_Ex[ 0x00500000 ] ));


/**
* @brief   R_EXRAM_GetBigMemory
*
* @par Parameters
*    None
* @return  None.
*/
errnum_t  R_EXRAM_GetBigMemory( void * /*<type**>*/ out_Address,  size_t *out_Size )
{
    *(void **) out_Address = gs_VRAM_Ex;
    *out_Size = sizeof( gs_VRAM_Ex );
    return  0;
}

