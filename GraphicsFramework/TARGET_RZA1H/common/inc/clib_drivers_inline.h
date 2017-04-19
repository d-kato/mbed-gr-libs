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
* @file  clib_drivers_inline.h
* @brief   $Module: CLibCommon $ $PublicVersion: 0.90 $ (=CLIB_VERSION)
* $Rev: 30 $
* $Date:: 2014-02-13 21:21:47 +0900#$
* - Description: Common code for drivers and more.
*/

#ifndef  CLIB_DRIVERS_INLINE_H
#define  CLIB_DRIVERS_INLINE_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Typedef definitions
******************************************************************************/
/* in clib_drivers_typedef.h */

/******************************************************************************
Macro definitions
******************************************************************************/
/* in clib_drivers_typedef.h */

/******************************************************************************
Variable Externs
******************************************************************************/
/* in clib_drivers_typedef.h */

/******************************************************************************
Functions Prototypes
******************************************************************************/


/**
* @brief   Cast with range check
*
* @param   input Input value
* @param   output Output value
* @return  Error Code. 0=No Error.
*/
INLINE errnum_t  R_int_t_to_int8_t( int_t input, int8_t *output )
{
    return  R_int32_t_to_int8_t( input, output );
}


/**
* @brief   Fast mod operation
*
* @param   value Left operand
* @return  value % N
*
* @par Description
* - This is shared description of
*   R_Mod_2s, R_Mod_4s, R_Mod_8s, R_Mod_16s, R_Mod_32s, R_Mod_64s,
*   R_Mod_2u, R_Mod_4u, R_Mod_8u, R_Mod_16u, R_Mod_32u, R_Mod_64u.
* - Porting layer of fast % operator.
* - MISRA & IPA SEC confirmed version.
* - Minus value is expected 2's complement.
* - Not same C99 % operator.
*   - Sample: R_Mod_4s( - 7 ) == +1.  Sample: -7 % 4 == -3
*/


/**
* @brief   Fast mod operation. See <R_Mod_N>.
*
* @par Parameters
*    None
* @return  None.
*/
INLINE int_fast32_t  R_Mod_2s( int_fast32_t const  value )
{
    enum { mask = 1 };
    return  ((int_fast32_t)((uint_fast32_t)(value) & mask));
}


/**
* @brief   Fast mod operation. See <R_Mod_N>.
*
* @par Parameters
*    None
* @return  None.
*/
INLINE int_fast32_t  R_Mod_4s( int_fast32_t const  value )
{
    enum { mask = 3 };
    return  ((int_fast32_t)((uint_fast32_t)(value) & mask));
}


/**
* @brief   Fast mod operation. See <R_Mod_N>.
*
* @par Parameters
*    None
* @return  None.
*/
INLINE int_fast32_t  R_Mod_8s( int_fast32_t const  value )
{
    enum { mask = 7 };
    return  ((int_fast32_t)((uint_fast32_t)(value) & mask));
}


/**
* @brief   Fast mod operation. See <R_Mod_N>.
*
* @par Parameters
*    None
* @return  None.
*/
INLINE int_fast32_t  R_Mod_16s( int_fast32_t const  value )
{
    enum { mask = 15 };
    return  ((int_fast32_t)((uint_fast32_t)(value) & mask));
}


/**
* @brief   Fast mod operation. See <R_Mod_N>.
*
* @par Parameters
*    None
* @return  None.
*/
INLINE int_fast32_t  R_Mod_32s( int_fast32_t const  value )
{
    enum { mask = 31 };
    return  ((int_fast32_t)((uint_fast32_t)(value) & mask));
}


/**
* @brief   Fast mod operation. See <R_Mod_N>.
*
* @par Parameters
*    None
* @return  None.
*/
INLINE int_fast32_t  R_Mod_64s( int_fast32_t const  value )
{
    enum { mask = 63 };
    return  ((int_fast32_t)((uint_fast32_t)(value) & mask));
}


/**
* @brief   Fast mod operation. See <R_Mod_N>.
*
* @par Parameters
*    None
* @return  None.
*/
INLINE uint_fast32_t  R_Mod_2u( uint_fast32_t const  value )
{
    return  ((value) & 1u);
}


/**
* @brief   Fast mod operation. See <R_Mod_N>.
*
* @par Parameters
*    None
* @return  None.
*/
INLINE uint_fast32_t  R_Mod_4u( uint_fast32_t const  value )
{
    return  ((value) & 3u);
}


/**
* @brief   Fast mod operation. See <R_Mod_N>.
*
* @par Parameters
*    None
* @return  None.
*/
INLINE uint_fast32_t  R_Mod_8u( uint_fast32_t const  value )
{
    return  ((value) & 7u);
}


/**
* @brief   Fast mod operation. See <R_Mod_N>.
*
* @par Parameters
*    None
* @return  None.
*/
INLINE uint_fast32_t  R_Mod_16u( uint_fast32_t const  value )
{
    return  ((value) & 15u);
}


/**
* @brief   Fast mod operation. See <R_Mod_N>.
*
* @par Parameters
*    None
* @return  None.
*/
INLINE uint_fast32_t  R_Mod_32u( uint_fast32_t const  value )
{
    return  ((value) & 31u);
}


/**
* @brief   Fast mod operation. See <R_Mod_N>.
*
* @par Parameters
*    None
* @return  None.
*/
INLINE uint_fast32_t  R_Mod_64u( uint_fast32_t const  value )
{
    return  ((value) & 63u);
}


/**
* @brief   Fast ceil operation
*
* @param   value Left operand
* @return  Ceil( value / N )
*
* @par Description
* - This is shared description of
*   R_Ceil_2s, R_Ceil_4s, R_Ceil_8s, R_Ceil_16s, R_Ceil_32s, R_Ceil_64s,
*   R_Ceil_2u, R_Ceil_4u, R_Ceil_8u, R_Ceil_16u, R_Ceil_32u, R_Ceil_64u.
* - Porting layer of fast ceil operation.
* - Function version is confirmed with MISRA & IPA SEC.
*/


/**
* @brief   Fast ceil operation. See <R_Ceil_N>.
*
* @par Parameters
*    None
* @return  None.
*/
INLINE int_fast32_t  R_Ceil_2s( int_fast32_t const  value )
{
    static const uint_fast32_t  mask = 1;
    return  ((int_fast32_t)((uint_fast32_t)((value)+mask)&~mask));
}


/**
* @brief   Fast ceil operation. See <R_Ceil_N>.
*
* @par Parameters
*    None
* @return  None.
*/
INLINE int_fast32_t  R_Ceil_4s( int_fast32_t const  value )
{
    static const uint_fast32_t  mask = 3;
    return  ((int_fast32_t)((uint_fast32_t)((value)+mask)&~mask));
}


/**
* @brief   Fast ceil operation. See <R_Ceil_N>.
*
* @par Parameters
*    None
* @return  None.
*/
INLINE int_fast32_t  R_Ceil_8s( int_fast32_t const  value )
{
    static const uint_fast32_t  mask = 7;
    return  ((int_fast32_t)((uint_fast32_t)((value)+mask)&~mask));
}


/**
* @brief   Fast ceil operation. See <R_Ceil_N>.
*
* @par Parameters
*    None
* @return  None.
*/
INLINE int_fast32_t  R_Ceil_16s( int_fast32_t const  value )
{
    static const uint_fast32_t  mask = 15;
    return  ((int_fast32_t)((uint_fast32_t)((value)+mask)&~mask));
}


/**
* @brief   Fast ceil operation. See <R_Ceil_N>.
*
* @par Parameters
*    None
* @return  None.
*/
INLINE int_fast32_t  R_Ceil_32s( int_fast32_t const  value )
{
    static const uint_fast32_t  mask = 31;
    return  ((int_fast32_t)((uint_fast32_t)((value)+mask)&~mask));
}


/**
* @brief   Fast ceil operation. See <R_Ceil_N>.
*
* @par Parameters
*    None
* @return  None.
*/
INLINE int_fast32_t  R_Ceil_64s( int_fast32_t const  value )
{
    static const uint_fast32_t  mask = 63;
    return  ((int_fast32_t)((uint_fast32_t)((value)+mask)&~mask));
}


/**
* @brief   Fast ceil operation. See <R_Ceil_N>.
*
* @par Parameters
*    None
* @return  None.
*/
INLINE uint_fast32_t  R_Ceil_2u( uint_fast32_t const  value )
{
    static const uint_fast32_t  mask = 1;
    return  (((value)+mask)&~mask);
}


/**
* @brief   Fast ceil operation. See <R_Ceil_N>.
*
* @par Parameters
*    None
* @return  None.
*/
INLINE uint_fast32_t  R_Ceil_4u( uint_fast32_t const  value )
{
    static const uint_fast32_t  mask = 3;
    return  (((value)+mask)&~mask);
}


/**
* @brief   Fast ceil operation. See <R_Ceil_N>.
*
* @par Parameters
*    None
* @return  None.
*/
INLINE uint_fast32_t  R_Ceil_8u( uint_fast32_t const  value )
{
    static const uint_fast32_t  mask = 7;
    return  (((value)+mask)&~mask);
}


/**
* @brief   Fast ceil operation. See <R_Ceil_N>.
*
* @par Parameters
*    None
* @return  None.
*/
INLINE uint_fast32_t  R_Ceil_16u( uint_fast32_t const  value )
{
    static const uint_fast32_t  mask = 15;
    return  (((value)+mask)&~mask);
}


/**
* @brief   Fast ceil operation. See <R_Ceil_N>.
*
* @par Parameters
*    None
* @return  None.
*/
INLINE uint_fast32_t  R_Ceil_32u( uint_fast32_t const  value )
{
    static const uint_fast32_t  mask = 31;
    return  (((value)+mask)&~mask);
}


/**
* @brief   Fast ceil operation. See <R_Ceil_N>.
*
* @par Parameters
*    None
* @return  None.
*/
INLINE uint_fast32_t  R_Ceil_64u( uint_fast32_t const  value )
{
    static const uint_fast32_t  mask = 63;
    return  (((value)+mask)&~mask);
}


#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif  /* CLIB_DRIVERS_INLINE_H */



