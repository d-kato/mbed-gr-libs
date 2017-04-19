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
* $FileName: RGA_Port_inline.h $
* $Module: RGA $ $PublicVersion: 1.03 $ (=RGA_VERSION)
* $Rev: $
* $Date::                           $
* Description:
******************************************************************************/


/* This file is included from "RGA_Port.h" */


#ifndef  RGA_PORT_INLINE_H
#define  RGA_PORT_INLINE_H
#ifndef  NOT_DEFINE_INLINE_FUNCTION

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "clib_drivers.h"
#include  "frame_buffer.h"

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


/**
* @brief   SwapEndian to 2Byte
*
* @param   in_out_2byteVariable Address to 2 byte
* @return  None
*/
INLINE void  SwapEndian2Byte( void *in_out_2byteVariable )
{
    byte_t   buffer[2];
    byte_t  *data = (byte_t *) in_out_2byteVariable;

    buffer[0] = data[0];
    buffer[1] = data[1];

    data[0] = buffer[1];
    data[1] = buffer[0];
}


/**
* @brief   SwapEndian to 4Byte
*
* @param   in_out_2byteVariable Address to 4 byte
* @return  None
*/
INLINE void  SwapEndian4Byte( void *in_out_4byteVariable )
{
    byte_t   buffer[4];
    byte_t  *data = (byte_t *) in_out_4byteVariable;

    buffer[0] = data[0];
    buffer[1] = data[1];
    buffer[2] = data[2];
    buffer[3] = data[3];

    data[0] = buffer[3];
    data[1] = buffer[2];
    data[2] = buffer[1];
    data[3] = buffer[0];
}


/**
* @brief   Sets the value to the address not aligned 4byte
*
* @param   out_Variable <uint32_not_aligned_t>
* @param   Value Set value
* @return  None
*/
INLINE void  uint32_not_aligned_t_set( uint32_not_aligned_t *out_Variable,  uint32_t Value )
{
#if BYTE_ENDIAN == BYTE_LITTLE_ENDIAN
    out_Variable->byte[0] = (byte_t)( ( Value & 0x000000FF ) );
    out_Variable->byte[1] = (byte_t)( ( Value & 0x0000FF00 ) >>  8 );
    out_Variable->byte[2] = (byte_t)( ( Value & 0x00FF0000 ) >> 16 );
    out_Variable->byte[3] = (byte_t)( ( Value & 0xFF000000 ) >> 24 );
#else
    out_Variable->byte[0] = (byte_t)( ( Value & 0xFF000000 ) >> 24 );
    out_Variable->byte[1] = (byte_t)( ( Value & 0x00FF0000 ) >> 16 );
    out_Variable->byte[2] = (byte_t)( ( Value & 0x0000FF00 ) >>  8 );
    out_Variable->byte[3] = (byte_t)( ( Value & 0x000000FF ) );
#endif
}


/***********************************************************************
* Class: byte_per_pixel_t
************************************************************************/


/**
* @brief   R_RGA_BitPerPixelType_To_BytePerPixelType
*
* @param   BitPerPixel Input
* @return  byte_per_pixel_t
*/
INLINE byte_per_pixel_t  R_RGA_BitPerPixelType_To_BytePerPixelType( int_fast32_t BitPerPixel )
{
    if ( R_Mod_8s( BitPerPixel ) == 0 )  {
        return  (byte_per_pixel_t)( (uint_fast32_t) BitPerPixel / 8 );
    } else                                 {
        return  BitPerPixel << R_BYTE_PER_PIXEL_SHIFT;
    }
}


/**
* @brief   R_RGA_BytePerPixelType_To_BitPerPixelType
*
* @param   BytePerPixel byte_per_pixel_t
* @return  BitPerPixel
*/
INLINE int_fast32_t  R_RGA_BytePerPixelType_To_BitPerPixelType( byte_per_pixel_t  BytePerPixel )
{
    return  ( ( BytePerPixel & R_BYTE_PER_PIXEL_MASK ) * 8 ) |
            ( BytePerPixel >> R_BYTE_PER_PIXEL_SHIFT );
}


/**
* @brief   R_BYTE_PER_PIXEL_IsInteger
*
* @param   BytePerPixel byte_per_pixel_t
* @return  Whether "BytePerPixel" is integer type.
*/
INLINE bool_t  R_BYTE_PER_PIXEL_IsInteger( byte_per_pixel_t BytePerPixel )
{
    return  ( BytePerPixel & R_BYTE_PER_PIXEL_MASK ) != 0;
}


/* Section: Global */
/**
* @brief   argb8888_t_to_rgb565_t
*
* @par Parameters
*    None
* @return  None.
*/
INLINE  rgb565_t  argb8888_t_to_rgb565_t( argb8888_t Color )
{
    rgb565_t  ret = {0};
    /* Warning: C4017W: ret may be used before being set */
    /* This warning is not correct */

    ret.Value = (uint16_t)(
                    ( ( Color.Value & 0x00F80000 ) >> 8 ) |
                    ( ( Color.Value & 0x0000FC00 ) >> 5 ) |
                    ( ( Color.Value & 0x000000F8 ) >> 3 ) );

    return  ret;
}


/**
* @brief   argb8888_t_to_argb1555_t
*
* @par Parameters
*    None
* @return  None.
*/
INLINE  argb1555_t  argb8888_t_to_argb1555_t( argb8888_t Color )
{
    argb1555_t  ret = {0};
    /* Warning: C4017W: ret may be used before being set */
    /* This warning is not correct */

    ret.Value = (uint16_t)(
                    ( ( Color.Value & 0x80000000 ) >> 16 ) |  /* A */
                    ( ( Color.Value & 0x00F80000 ) >>  9 ) |  /* R */
                    ( ( Color.Value & 0x0000F800 ) >>  6 ) |  /* G */
                    ( ( Color.Value & 0x000000F8 ) >>  3 ) ); /* B */

    return  ret;
}


/**
* @brief   argb8888_t_to_argb4444_t
*
* @par Parameters
*    None
* @return  None.
*/
INLINE  argb4444_t  argb8888_t_to_argb4444_t( argb8888_t Color )
{
    argb4444_t  ret = {0};
    /* Warning: C4017W: ret may be used before being set */
    /* This warning is not correct */

    ret.Value = (uint16_t)(
                    ( ( Color.Value & 0xF0000000 ) >> 16 ) |
                    ( ( Color.Value & 0x00F00000 ) >> 12 ) |
                    ( ( Color.Value & 0x0000F000 ) >>  8 ) |
                    ( ( Color.Value & 0x000000F0 ) >>  4 ) );

    return  ret;
}


/**
* @brief   argb8888_t_to_r8g8b8a8_t
*
* @par Parameters
*    None
* @return  None.
*/
INLINE  r8g8b8a8_t  argb8888_t_to_r8g8b8a8_t( argb8888_t Color )
{
    r8g8b8a8_t  ret;
    ret.Value = 0;  /* Avoid Warning: C4017W: ret may be used before being set */

    ret.u.Red   = Color.u.Red;
    ret.u.Green = Color.u.Green;
    ret.u.Blue  = Color.u.Blue;
    ret.u.Alpha = Color.u.Alpha;

    return  ret;
}


/**
* @brief   rgb565_t_to_argb8888_t
*
* @par Parameters
*    None
* @return  None.
*/
INLINE  argb8888_t  rgb565_t_to_argb8888_t( rgb565_t Color )
{
    argb8888_t  ret;

    ret.Value = 0xFF000000 | /* A */
                ( ( Color.Value & 0xF800 ) << 8 ) | ( ( Color.Value & 0xE000 ) << 3 ) |  /* R */
                ( ( Color.Value & 0x07E0 ) << 5 ) | ( ( Color.Value & 0x0600 ) >> 1 ) |  /* G */
                ( ( Color.Value & 0x001F ) << 3 ) | ( ( Color.Value & 0x001C ) >> 2 );   /* B */

    return  ret;
}


/**
* @brief   argb1555_t_to_argb8888_t
*
* @par Parameters
*    None
* @return  None.
*/
INLINE  argb8888_t  argb1555_t_to_argb8888_t( argb1555_t Color )
{
    argb8888_t  ret;

    ret.Value =
        ( ( Color.Value & 0x7C00 ) << 9 ) | ( ( Color.Value & 0x7000 ) << 4 ) |  /* R */
        ( ( Color.Value & 0x03E0 ) << 6 ) | ( ( Color.Value & 0x0380 ) << 1 ) |  /* G */
        ( ( Color.Value & 0x001F ) << 3 ) | ( ( Color.Value & 0x001C ) >> 2 );   /* B */

    if ( Color.u.Alpha ) {
        ret.Value |= 0xFF000000;    /* A */
    }

    return  ret;
}


/**
* @brief   argb4444_t_to_argb8888_t
*
* @par Parameters
*    None
* @return  None.
*/
INLINE  argb8888_t  argb4444_t_to_argb8888_t( argb4444_t Color )
{
    argb8888_t  ret;

    ret.Value =
        ( ( Color.Value & 0xF000 ) << 16 ) | ( ( Color.Value & 0xF000 ) << 12 ) |  /* A */
        ( ( Color.Value & 0x0F00 ) << 12 ) | ( ( Color.Value & 0x0F00 ) <<  8 ) |  /* R */
        ( ( Color.Value & 0x00F0 ) <<  8 ) | ( ( Color.Value & 0x00F0 ) <<  4 ) |  /* G */
        ( ( Color.Value & 0x000F ) <<  4 ) | ( ( Color.Value & 0x000F )       );   /* B */

    return  ret;
}


/**
* @brief   rgba8888_t_to_rgb565_t
*
* @par Parameters
*    None
* @return  None.
*/
INLINE rgb565_t  rgba8888_t_to_rgb565_t( rgba8888_t Color )
{
    rgb565_t  ret;

    ret.Value = (uint16_t) (
                    ( ( Color.Value & 0xF8000000 ) >> 16 ) |  /* R */
                    ( ( Color.Value & 0x00FC0000 ) >> 13 ) |  /* G */
                    ( ( Color.Value & 0x0000F800 ) >> 11 ) ); /* B */

    return  ret;
}


/**
* @brief   rgba8888_t_to_argb1555_t
*
* @par Parameters
*    None
* @return  None.
*/
INLINE argb1555_t  rgba8888_t_to_argb1555_t( rgba8888_t Color )
{
    argb1555_t  ret;

    ret.Value = (uint16_t) (
                    ( ( Color.Value & 0xF8000000 ) >> 17 ) |  /* R */
                    ( ( Color.Value & 0x00F80000 ) >> 14 ) |  /* G */
                    ( ( Color.Value & 0x0000F800 ) >> 11 ) |  /* B */
                    ( ( Color.Value & 0x00000080 ) <<  8 ) ); /* A */

    return  ret;
}


/**
* @brief   rgba8888_t_to_argb4444_t
*
* @par Parameters
*    None
* @return  None.
*/
INLINE argb4444_t  rgba8888_t_to_argb4444_t( rgba8888_t Color )
{
    argb4444_t  ret;

    ret.Value = (uint16_t) (
                    ( ( Color.Value & 0xF0000000 ) >> 20 ) |  /* R */
                    ( ( Color.Value & 0x00F00000 ) >> 16 ) |  /* G */
                    ( ( Color.Value & 0x0000F000 ) >> 12 ) |  /* B */
                    ( ( Color.Value & 0x000000F0 ) <<  8 ) ); /* A */

    return  ret;
}


/**
* @brief   r8g8b8a8_t_to_argb8888_t
*
* @par Parameters
*    None
* @return  None.
*/
INLINE  argb8888_t  r8g8b8a8_t_to_argb8888_t( r8g8b8a8_t Color )
{
    argb8888_t  ret;

    ret.Value =
        ( Color.u.Alpha << 24 ) |
        ( Color.u.Red   << 16 ) |
        ( Color.u.Green <<  8 ) |
        ( Color.u.Blue );

    return  ret;
}


/**
* @brief   rgba8888_t_to_argb8888_t
*
* @par Parameters
*    None
* @return  None.
*/
INLINE argb8888_t  rgba8888_t_to_argb8888_t( rgba8888_t Color )
{
    argb8888_t  ret;

    ret.Value = (
                    ( ( Color.Value & 0xFF000000 ) >>  8 ) |  /* R */
                    ( ( Color.Value & 0x00FF0000 ) >>  8 ) |  /* G */
                    ( ( Color.Value & 0x0000FF00 ) >>  8 ) |  /* B */
                    ( ( Color.Value & 0x000000FF ) << 24 ) ); /* A */

    return  ret;
}


#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif  /* NOT_DEFINE_INLINE_FUNCTION */
#endif  /* RGA_PORT_INLINE_H */
