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
* @file  clib_drivers.c
* @brief   $Module: CLibCommon $ $PublicVersion: 0.90 $ (=CLIB_VERSION)
* $Rev: 30 $
* $Date:: 2014-02-13 21:21:47 +0900#$
* - Description: Common code.
*/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_ospl.h"
#include  "clib_drivers.h"


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

/**
* @brief   Cast with range check
*
* @param   input Input value
* @param   output Output value
* @return  Error Code. 0=No Error.
*/
errnum_t  R_int32_t_to_int8_t( int32_t const  input, int8_t *const  output )
{
    errnum_t  e;

    IF ( (input < INT8_MIN)  ||  (input > INT8_MAX) )  {
        e=E_OTHERS;
        goto fin;
    }
    IF_DQ( output == NULL ) {
        e=E_OTHERS;
        goto fin;
    }

    *output = (int8_t) input;

    e=0;
fin:
    return  e;
}


/**
* @brief   Cast with range check
*
* @param   input Input value
* @param   output Output value
* @return  Error Code. 0=No Error.
*/
errnum_t  R_int32_t_to_int16_t( int32_t input, int16_t *output )
{
    IF ( input < -32768  ||  input > 32767 ) {
        return  E_OTHERS;
    }

    *output = (int16_t) input;
    return  0;
}


/**
* @brief   Cast with range check
*
* @param   input Input value
* @param   output Output value
* @return  Error Code. 0=No Error.
*/
errnum_t  R_int32_t_to_uint8_t( int32_t input, uint8_t *output )
{
    IF ( input < 0  ||  input > 255 ) {
        return  E_OTHERS;
    }

    *output = (uint8_t) input;
    return  0;
}


/**
* @brief   Returns whether specified value has 1 bit only 1.
*
* @param   Value Input value
* @return  Returns whether specified value has 1 bit only 1.
*/
bool_t  Is1bitOnlyInt( uint32_t Value )
{
    if ( (Value & 0x0000FFFF) == 0 ) {
        Value >>= 16;
    }
    if ( (Value & 0x000000FF) == 0 ) {
        Value >>=  8;
    }
    if ( (Value & 0x0000000F) == 0 ) {
        Value >>=  4;
    }
    if ( (Value & 0x00000003) == 0 ) {
        Value >>=  2;
    }
    if ( (Value & 0x00000001) == 0 ) {
        Value >>=  1;
    }
    return  ( Value == 1 );
}


/**
* @brief   Get whether 2 "IntBoxType"s are same area.
*
* @param   in_BoxA A box
* @param   in_BoxB Other box
* @param   out_IsSame Whether 2 "IntBoxType"s are same area.
* @return  Error Code. 0=No Error.
*/
errnum_t  IntBoxType_isSame( const IntBoxType *in_BoxA, const IntBoxType *in_BoxB, bool_t *out_IsSame )
{
    *out_IsSame = (
                      in_BoxA->MinX   == in_BoxB->MinX  &&
                      in_BoxA->MinY   == in_BoxB->MinY  &&
                      in_BoxA->Width  == in_BoxB->Width  &&
                      in_BoxA->Height == in_BoxB->Height );

    return  0;
}


