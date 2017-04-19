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
* @file  r_static_an_tag.h
* @brief   Reviewed tag for warnings of static code analysis.
*
* $Module: OSPL $ $PublicVersion: 0.90 $ (=R_OSPL_VERSION)
* $Rev: 35 $
* $Date:: 2014-04-15 21:38:18 +0900#$
*/

#ifndef R_STATIC_AN_TAG_H
#define R_STATIC_AN_TAG_H


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "Project_Config.h"
#include  "r_typedefs.h"
#include  "r_multi_compiler.h"

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

/***********************************************************************
* Macros: IS
*    Changes the code accepted with MISRA 13.2 to readable
*
* Arguments:
*    bool_value - The expression that evaluated result becomes boolean type
*
* Return Value:
*    Evaluate result of "bool_value != 0".
*
* Description:
*   Avoid "not 0" as double negation.
*   "IS" macro corresponds to cast to boolean type specified in the language.
*
*   This is for QAC warning : MISRA 13.2 Advice : Tests of a value against
*   zero should be made explicit, unless the operand is effectively Boolean.
*
*   Write this macro after being warned by static code analyzer.
*
* Example:
*    > if ( IS( bool_value ) ) {...}
*    > if ( IS( bool_value ) && IS( bool_value2 ) && ! bool_value3 ) {...}
*    > if ( IS( unsigned_bitfield ) ) {...}
*    > bool_t is_flag = (bool_t) unsigned_bitfield;  ("IS" is not used)
************************************************************************/
/* ->MISRA 19.7 : For return _Bool type */ /* ->SEC M5.1.3 */
#if ! R_BOOL_IS_SIGNED
#define  IS( bool_value )  ( (bool_t)( bool_value ) != 0u )
#else
#define  IS( bool_value )  ( (bool_t)( bool_value ) != 0 )
#endif
/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */

#if 0
/* Comment out because if checking type function(this) was called, raises SEC R3.6.2(QAC 3441) */
#define  IS( bool_value )  ( IS_Sub( bool_value ) != 0 )
INLINE bool_t  IS_Sub( bool_t const  value )   /* Check type only */
{
    return  value;
}
#endif


/***********************************************************************
* Macros: IF_DQ
*    Error check on DEBUG and QAC version for faster
************************************************************************/
/* ->MISRA 19.4 : Compliant with C language syntax. */ /* ->SEC M1.8.2 */
#if defined(__QAC_ARM_H__)
#define  IF_DQ  if
#else
#define  IF_DQ  IF_D
#endif
/* <-MISRA 19.4 */ /* <-SEC M1.8.2 */


/***********************************************************************
* Macros: IF_DS
*    Error check on DEBUG and QAC version for always false
*
* Description:
*    QAC assist tool founds IF_DS keyword.
*    Sentence using this expects IPA SEC O4.1, O1.1.
************************************************************************/
#define  IF_DS  IF_DQ


/***********************************************************************
* Macros: IF_S
*    Error check on QAC version for always false
*
* Description:
*    QAC assist tool founds IF_S keyword.
*    Compiler always does not check it.
************************************************************************/
/* ->MISRA 19.4 : Compliant with C language syntax. */ /* ->SEC M1.8.2 */
#if defined(__QAC_ARM_H__)
#define  IF_S               if
#else
#define  IF_S( Condition )  if ( false )
#endif
/* <-MISRA 19.4 */ /* <-SEC M1.8.2 */


/**
* @def  R_CUT_IF_ALWAYS
* @brief  Whether a expression always true/false was cut or not.
* @par Parameters
*    None
* @return  None.
*
* @par Description
*    The value is 0 or 1.
*    This macro is for @ref R_CUT_IF_ALWAYS_TRUE and @ref R_CUT_IF_ALWAYS_FALSE.
*/
#ifndef  R_CUT_IF_ALWAYS
#if defined(__QAC_ARM_H__)
#define  R_CUT_IF_ALWAYS  0
#else
#define  R_CUT_IF_ALWAYS  1
#endif
#endif


/**
* @def  R_CUT_IF_ALWAYS_TRUE
* @brief  Assertion that the expression is always true
* @par Parameters
*    None
* @return  None.
*/
#if R_CUT_IF_ALWAYS
#define  R_CUT_IF_ALWAYS_TRUE( expression_and )
#else
/* ->MISRA 19.10 : Can not in ( ). e.g. expression_and = " a >= 0  &&" */
#define  R_CUT_IF_ALWAYS_TRUE( expression_and )  expression_and
/* <-MISRA 19.10 */
#endif


/**
* @def  R_CUT_IF_ALWAYS_FALSE
* @brief  Assertion that the expression is always false
* @par Parameters
*    None
* @return  None.
*/
#if R_CUT_IF_ALWAYS
#define  R_CUT_IF_ALWAYS_FALSE( expression_or )
#else
/* ->MISRA 19.10 : Can not in ( ). e.g. expression_and = " a >= 0  &&" */
#define  R_CUT_IF_ALWAYS_FALSE( expression_or )  expression_or
/* <-MISRA 19.10 */
#endif


/**
* @def  R_UNREFERENCED_VARIABLE
* @brief  Assertion that specified variable is not referenced
* @par Parameters
*    None
* @return  None.
*
* @par Description
*    This avoids warning of "set but never used".
*/
/* ->MISRA 19.7 : Argument must be any type */ /* ->SEC M5.1.3 */
#define  R_UNREFERENCED_VARIABLE( a1 )  R_UNREFERENCED_VARIABLE_Sub( &(a1) )
/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */
/* ->QAC 3206 : Not used argument */
INLINE void  R_UNREFERENCED_VARIABLE_Sub( volatile const void *a1 ) { }
/* <-QAC 3206 */


/**
* @def  R_UNREFERENCED_VARIABLE_2
* @brief  Assertion that specified variable is not referenced
* @par Parameters
*    None
* @return  None.
*
* @par Description
*    This avoids warning of "set but never used".
*/
/* ->MISRA 19.7 : Argument must be any type */ /* ->SEC M5.1.3 */
#define  R_UNREFERENCED_VARIABLE_2( a1,a2 )  R_UNREFERENCED_VARIABLE_2_Sub( &(a1), &(a2) )
/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */
/* ->QAC 3206 : Not used argument */
INLINE void  R_UNREFERENCED_VARIABLE_2_Sub( volatile const void *a1, volatile const void *a2 ) { }
/* <-QAC 3206 */


/**
* @def  R_UNREFERENCED_VARIABLE_3
* @brief  Assertion that specified variable is not referenced
* @par Parameters
*    None
* @return  None.
*
* @par Description
*    This avoids warning of "set but never used".
*/
/* ->MISRA 19.7 : Argument must be any type */ /* ->SEC M5.1.3 */
#define  R_UNREFERENCED_VARIABLE_3( a1,a2,a3 )  R_UNREFERENCED_VARIABLE_3_Sub( &(a1), &(a2), &(a3) )
/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */
/* ->QAC 3206 : Not used argument */
INLINE void  R_UNREFERENCED_VARIABLE_3_Sub( volatile const void *a1, volatile const void *a2,
        volatile const void *a3 ) { }
/* <-QAC 3206 */


/**
* @def  R_UNREFERENCED_VARIABLE_4
* @brief  Assertion that specified variable is not referenced
* @par Parameters
*    None
* @return  None.
*
* @par Description
*    This avoids warning of "set but never used".
*/
/* ->MISRA 19.7 : Argument must be any type */ /* ->SEC M5.1.3 */
#define  R_UNREFERENCED_VARIABLE_4( a1,a2,a3,a4 )  R_UNREFERENCED_VARIABLE_4_Sub( &(a1), &(a2), &(a3), &(a4) )
/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */
/* ->QAC 3206 : Not used argument */
INLINE void  R_UNREFERENCED_VARIABLE_4_Sub( volatile const void *a1, volatile const void *a2,
        volatile const void *a3, volatile const void *a4 ) { }
/* <-QAC 3206 */


/**
* @def  R_IT_WILL_BE_NOT_CONST
* @brief  variable WILL_BE_NOT_CONST
* @par Parameters
*    None
* @return  None.
*
* @par Description
*    This avoids MISRA 16.7 advisory and SEC M1.11.1 for not modified API argument
*    will be not const future.
*/
/* ->MISRA 16.7 */ /* ->SEC M1.11.1 : Tell to QAC/compiler that "*Pointer" was changed */
INLINE void  R_IT_WILL_BE_NOT_CONST( void *Pointer );  /* MISRA 8.1 Advice */
INLINE void  R_IT_WILL_BE_NOT_CONST( void *Pointer )
{
    R_UNREFERENCED_VARIABLE( Pointer );
}
/* <-MISRA 16.7 */ /* <-SEC M1.11.1 */


/**
* @def  R_AVOID_UNSAFE_ALWAYS_WARNING
* @brief  Specifies the code cannot be cut.
* @par Parameters
*    None
* @return  None.
*
* @par Description
*    This avoid MISRA 13.7 require always true/false condition.
*    This tells to QAC/compiler that "Variable" was changed.
*/
/* ->MISRA 19.7 : Argument must be any type */ /* ->SEC M5.1.3 */
#define  R_AVOID_UNSAFE_ALWAYS_WARNING( Variable ) \
	R_AVOID_UNSAFE_ALWAYS_WARNING_SUB( &(Variable) )
/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */
/* ->MISRA 16.7 : Tell to QAC/compiler that "Variable" was changed */ /* ->SEC M1.11.1 */
INLINE void  R_AVOID_UNSAFE_ALWAYS_WARNING_SUB( void *Pointer );  /* MISRA 8.1 Advice */
INLINE void  R_AVOID_UNSAFE_ALWAYS_WARNING_SUB( void *Pointer )
/* <-MISRA 16.7 */ /* <-SEC M1.11.1 */
{
    R_UNREFERENCED_VARIABLE( Pointer );
}


/**
* @def  TO_UNSIGNED
* @brief  Cast to unsigned type constant value
* @par Parameters
*    None
* @return  None.
*/
/* ->MISRA 19.7 : It is not able to replace to function because this expands macro's parameter */
/* ->SEC M5.1.3 */
#define  TO_UNSIGNED(x)  TO_UNSIGNED_AGAIN(x)  /* Expand "x" */
/* <-MISRA 19.7 */
/* <-SEC M5.1.3 */

/* ->MISRA 19.13 */ /* ->SEC M5.1.2 (1) */
#define  TO_UNSIGNED_AGAIN(x)  (x##u)
/* <-MISRA 19.13 */ /* <-SEC M5.1.2 (1) */


/**
* @brief   Cast to unsigned type constant value
*
* @par Parameters
*    None
* @return  None.
*
* @par Description
*    This can not avoid QAC warning. This macro is for count reviewed code automatically.
*/
INLINE uint32_t  R_ToUnsigned( int32_t const  ConstantInteger )
{
    return  (uint32_t) ConstantInteger;
}


/**
* @brief   Cast to signed type constant value
*
* @par Parameters
*    None
* @return  None.
*
* @par Description
*    This can not avoid QAC warning. This macro is for count reviewed code automatically.
*/
INLINE int32_t  R_ToSigned( uint32_t const  ConstantInteger )
{
    return  (int32_t) ConstantInteger;
}


/***********************************************************************
* End of File:
************************************************************************/
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif /* R_STATIC_AN_TAG_H */

