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
* Copyright (C) 2012 - 2015 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**
* @file  clib_registers.h
* @brief   $Module: CLibCommon $ $PublicVersion: 0.90 $ (=CLIB_VERSION)
* $Rev: 30 $
* $Date:: 2014-02-13 21:21:47 +0900#$
* - Description: Common code for drivers and more.
*/

#ifndef  CLIB_REGISTERS_H
#define  CLIB_REGISTERS_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_typedefs.h"
#include  "r_ospl.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


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

/******************************************************************************
Inline Functions
******************************************************************************/


/**
* @brief   CPG unit of RZ/A1H.
*
* @par Parameters
*    None
* @return  Pointer to CPG structure.
*/
INLINE struct st_cpg  *R_Get_CPG_Base(void) {
    /* ->QAC 0306 */
#if IODEFINE_H_VERSION >= 100
    return  &CPG;
#else
    return  (struct st_cpg *) &CPG;
#endif
    /* <-QAC 0306 */
}


/***********************************************************************
* Group: Register_Access
************************************************************************/

/**
* @brief   Set a value to register bit field.
*
* @param   in_out_Register Address of register or variable
* @param   RegisterName Name of register
* @param   BitName Name of bit
* @param   Value Writing value
* @return  None.
*
* @par Description
*    Bit width is got from "RegisterName".
*/
/* ->MISRA 19.12 */ /* ->MISRA 19.13 */ /* ->SEC M5.1.2 (1) */
#define  R_DRV_SET_REGISTER_BIT_FIELD( \
		in_out_Register, RegisterName, BitName, Value ) \
	R_DRV_SET_REGISTER_BIT_FIELD_WITH_REG_WIDTH( \
		in_out_Register, RegisterName, BitName, Value, \
		DRV__BIT_WIDTH__##RegisterName )
/* <-MISRA 19.12 */ /* <-MISRA 19.13 */ /* <-SEC M5.1.2 (1) */


/**
* @brief   Set a value to register bit field with width parameter.
*
* @param   in_out_Register Address of register or variable
* @param   RegisterName Name of register
* @param   BitName Name of bit
* @param   Value Writing value
* @param   BitWidth BitWidth
* @return  None.
*/
/* ->MISRA 19.12 */ /* ->MISRA 19.13 */ /* ->SEC M5.1.2 (1) */
/* ->MISRA 19.7 : Expand "DRV__BIT_WIDTH__##RegisterName" macro */
/* ->SEC M5.1.3 */
#define  R_DRV_SET_REGISTER_BIT_FIELD_WITH_REG_WIDTH( \
		in_out_Register, RegisterName, BitName, Value, BitWidth ) \
	R_DRV_SET_REGISTER_BIT_FIELD_SUB0( \
		in_out_Register, RegisterName##__##BitName, Value, BitWidth )
/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */
/* <-MISRA 19.12 */ /* <-MISRA 19.13 */ /* <-SEC M5.1.2 (1) */

/* Sub macro */
/* ->MISRA 19.12 */ /* ->MISRA 19.13 */ /* ->SEC M5.1.2 (1) */
/* ->MISRA 19.7 : Expand "RegisterName##__##BitName" macro */
/* ->SEC M5.1.3 */
#define  R_DRV_SET_REGISTER_BIT_FIELD_SUB0( \
		in_out_Register, RegisterBitName, Value, BitWidth ) \
	R_DRV_SET_REGISTER_BIT_FIELD_SUB( \
		in_out_Register, RegisterBitName, Value, BitWidth )
/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */
/* <-MISRA 19.12 */ /* <-MISRA 19.13 */ /* <-SEC M5.1.2 (1) */

/* Sub macro */
/* ->MISRA 19.12 */ /* ->MISRA 19.13 */ /* ->SEC M5.1.2 (1) */
#define  R_DRV_SET_REGISTER_BIT_FIELD_SUB( \
		in_out_Register, RegisterBitName, Value, BitWidth ) \
	R_OSPL_SET_TO_##BitWidth##_BIT_REGISTER( \
		(volatile uint##BitWidth##_t*)(in_out_Register), \
		DRV__MASK##BitWidth##__##RegisterBitName, \
		DRV__SHIFT__##RegisterBitName, \
		(uint##BitWidth##_t)(Value) )
/* <-MISRA 19.12 */ /* <-MISRA 19.13 */ /* <-SEC M5.1.2 (1) */


/**
* @brief   Get a value from register bit field.
*
* @param   RegisterValue Value of register or variable
* @param   RegisterName Name of register
* @param   BitName Name of bit
* @return  Value of shifted bit field.
*
* @par Description
*    Bit width is got from "RegisterName".
*/
/* ->MISRA 19.12 */ /* ->MISRA 19.13 */ /* ->SEC M5.1.2 (1) */
#define  R_DRV_GET_REGISTER_BIT_FIELD( \
		RegisterValue, RegisterName, BitName ) \
	R_DRV_GET_REGISTER_BIT_FIELD_WITH_REG_WIDTH( \
		RegisterValue, RegisterName, BitName, \
		DRV__BIT_WIDTH__##RegisterName )
/* <-MISRA 19.12 */ /* <-MISRA 19.13 */ /* <-SEC M5.1.2 (1) */


/**
* @brief   Get a value from register bit field with width parameter.
*
* @param   RegisterValue Value of register or variable
* @param   RegisterName Name of register
* @param   BitName Name of bit
* @param   BitWidth BitWidth
* @return  Value of shifted bit field.
*/
/* ->MISRA 19.12 */ /* ->MISRA 19.13 */ /* ->SEC M5.1.2 (1) */
/* ->MISRA 19.7 : Expand "DRV__BIT_WIDTH__##RegisterName" macro */
/* ->SEC M5.1.3 */
#define  R_DRV_GET_REGISTER_BIT_FIELD_WITH_REG_WIDTH( \
		RegisterValue, RegisterName, BitName, BitWidth ) \
	R_DRV_GET_REGISTER_BIT_FIELD_SUB0( \
		RegisterValue, RegisterName##__##BitName, BitWidth )
/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */
/* <-MISRA 19.12 */ /* <-MISRA 19.13 */ /* <-SEC M5.1.2 (1) */

/* Sub macro */
/* ->MISRA 19.12 */ /* ->MISRA 19.13 */ /* ->SEC M5.1.2 (1) */
/* ->MISRA 19.7 : Expand "RegisterName##__##BitName" macro */
/* ->SEC M5.1.3 */
#define  R_DRV_GET_REGISTER_BIT_FIELD_SUB0( \
		RegisterValue, RegisterBitName, BitWidth ) \
	R_DRV_GET_REGISTER_BIT_FIELD_SUB( \
		RegisterValue, RegisterBitName, BitWidth )
/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */
/* <-MISRA 19.12 */ /* <-MISRA 19.13 */ /* <-SEC M5.1.2 (1) */

/* Sub macro */
/* ->MISRA 19.12 */ /* ->MISRA 19.13 */ /* ->SEC M5.1.2 (1) */
#define  R_DRV_GET_REGISTER_BIT_FIELD_SUB( \
		RegisterValue, RegisterBitName, BitWidth ) \
	R_OSPL_GET_FROM_##BitWidth##_BIT_REGISTER( \
		(volatile const uint##BitWidth##_t*) &(RegisterValue), \
		DRV__MASK##BitWidth##__##RegisterBitName, \
		DRV__SHIFT__##RegisterBitName )
/* <-MISRA 19.12 */ /* <-MISRA 19.13 */ /* <-SEC M5.1.2 (1) */


/**
* @brief   Returns whether specified value is overflowed from the bit field.
*
* @param   RegisterName Name of register
* @param   BitName Name of bit
* @param   Value Checking value
* @return  Whether specified value is overflowed.
*/
/* ->MISRA 19.12 */ /* ->MISRA 19.13 */ /* ->SEC M5.1.2 (1) */
#define  R_DRV_IS_OVERFLOW_BIT_FIELD( \
		RegisterName, BitName, Value ) \
	R_DRV_IS_OVERFLOW_BIT_FIELD_WITH_REG_WIDTH( \
		RegisterName, BitName, Value, DRV__BIT_WIDTH__##RegisterName )
/* <-MISRA 19.12 */ /* <-MISRA 19.13 */ /* <-SEC M5.1.2 (1) */


/**
* @brief   Returns whether specified value is overflowed from the bit field.
*
* @param   RegisterName Name of register
* @param   BitName Name of bit
* @param   Value Checking value
* @param   BitWidth BitWidth
* @return  Whether specified value is overflowed.
*/
/* ->MISRA 19.12 */ /* ->MISRA 19.13 */ /* ->SEC M5.1.2 (1) */
/* ->MISRA 19.7 : Expand "DRV__BIT_WIDTH__##RegisterName" macro */
/* ->SEC M5.1.3 */
#define  R_DRV_IS_OVERFLOW_BIT_FIELD_WITH_REG_WIDTH( \
		RegisterName, BitName, Value, BitWidth ) \
	R_DRV_IS_OVERFLOW_BIT_FIELD_SUB0( \
		RegisterName##__##BitName, Value, BitWidth )
/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */
/* <-MISRA 19.12 */ /* <-MISRA 19.13 */ /* <-SEC M5.1.2 (1) */

/* Sub macro */
/* ->MISRA 19.12 */ /* ->MISRA 19.13 */ /* ->SEC M5.1.2 (1) */
/* ->MISRA 19.7 : Expand "RegisterName##__##BitName" macro */
/* ->SEC M5.1.3 */
#define  R_DRV_IS_OVERFLOW_BIT_FIELD_SUB0( \
		RegisterBitName, Value, BitWidth ) \
	R_DRV_IS_OVERFLOW_BIT_FIELD_SUB( \
		RegisterBitName, Value, BitWidth )
/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */
/* <-MISRA 19.12 */ /* <-MISRA 19.13 */ /* <-SEC M5.1.2 (1) */

/* Sub macro */
/* ->MISRA 19.12 */ /* ->MISRA 19.13 */ /* ->SEC M5.1.2 (1) */
#define  R_DRV_IS_OVERFLOW_BIT_FIELD_SUB( \
		RegisterBitName, Value, BitWidth ) \
	R_DRV_IsOverflowBitField##BitWidth##_Sub( \
		DRV__MASK##BitWidth##__##RegisterBitName, \
		DRV__SHIFT__##RegisterBitName, \
		Value )
/* <-MISRA 19.12 */ /* <-MISRA 19.13 */ /* <-SEC M5.1.2 (1) */


/* Sub function */
/* : R_DRV_IsOverflowBitField32_Sub */
/* : R_DRV_IsOverflowBitField16_Sub */
/* : R_DRV_IsOverflowBitField8_Sub */
INLINE bool_t  R_DRV_IsOverflowBitField32_Sub( uint32_t const  Mask,
        int_fast32_t const  Shift,  uint32_t const  Value )
{
    return  ( ( (uint32_t)(Value) &
                ~( (uint32_t)(Mask) >> (Shift) ) )
              != 0u );
}

INLINE bool_t  R_DRV_IsOverflowBitField16_Sub( uint16_t const  Mask,
        int_fast32_t const  Shift,  uint16_t const  Value )
{
    return  ( ( (uint_fast16_t)(Value) &
                ~( (uint_fast16_t)(Mask) >> (Shift) ) )
              != 0u );
}

INLINE bool_t  R_DRV_IsOverflowBitField8_Sub( uint8_t const  Mask,
        int_fast32_t const  Shift, uint8_t const  Value )
{
    return  ( ( (uint_fast8_t)(Value) &
                ~( (uint_fast8_t)(Mask) >> (Shift) ) )
              != 0u );
}


/**
* @def  CPG
* @brief  CPG
*/
/* 0xFCFE0438 */

#define  DRV__BIT_WIDTH__STBCR9  8

enum { /*uint8_t */       DRV__MASK8__STBCR9__MSTP91 = 0x02 };  /* VDC5-0, LVDS */
enum { /*uint8_t */       DRV__MASK8__STBCR9__MSTP90 = 0x01 };  /* VDC5-1 */

enum { /* int_fast32_t */  DRV__SHIFT__STBCR9__MSTP91 =  1 };
enum { /* int_fast32_t */  DRV__SHIFT__STBCR9__MSTP90 =  0 };

#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif  /* CLIB_REGISTERS_H */



