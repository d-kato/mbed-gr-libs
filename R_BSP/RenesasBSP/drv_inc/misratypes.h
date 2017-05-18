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
* http://www.renesas.com/disclaimer*
* Copyright (C) 2009-2012 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

/******************************************************************************
 *
 *  V. 2.03.00
 * $Rev: $
 * $Date::                           $
 *
 * Description : Types for MISRA compliant C code.
 *
 *****************************************************************************/

#ifndef _MISRATYPES_H_INCLUDED_
#define _MISRATYPES_H_INCLUDED_


/***********************************************************************************
 System Includes
***********************************************************************************/
#include <cmsis_os.h>

/***********************************************************************************
 User Includes
***********************************************************************************/
#include "r_typedefs.h"

/***********************************************************************************
 Defines
***********************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif


/***********************************************************************************
 Constant Macros
***********************************************************************************/


/***********************************************************************************
 Function Macros
***********************************************************************************/

/* Use this macro for an unused function argument */
#define UNUSED_ARG(var)  (void)unused_arg(&(var))

/* Use this macro to check is a value of any type is an error code */
#define ISERROR(val) iserror((int32_t)(val))

/* Use this macro when assigning a function for a task */
#define TO_FP_FUNCTION(val) to_fp_function(val)

/***********************************************************************************
 Typedefs
***********************************************************************************/

typedef void (*funcptr_t)(void);

/***********************************************************************************
 Enumerated Types
***********************************************************************************/

/***********************************************************************************
 Function Prototypes
***********************************************************************************/

/* These functions are inlined by the SHC compiler in an optimise=1 build */
static const void* unused_arg(const void* const var);
static void *error_to_ptr(int32_t er);
static bool_t iserror(int32_t ser);
static funcptr_t to_fp_function(void (*val)(int32_t));

/***********************************************************************************
 Inline Functions
***********************************************************************************/

/**********************************************************************************
Function Name:  unused_arg
Description:    Used to prevent unused argument warnings from QA-C.
                This function will be inlined by the SHC compiler and generate no code.
                Use this function via the UNUSED_ARG macro.

Parameters:     Pointer to unused argument

Return value:   Passed in argument.
***********************************************************************************/
#if   defined (__CC_ARM)
#pragma inline unused_arg
#elif defined (__ICCARM__)
#pragma inline =forced
#endif
static inline const void* unused_arg(const void* const var)
{
    return var;
}

/**********************************************************************************
Function Name:  iserror
Description:    Check if a value is an error code.  This is used instead of the HIOS
                iserrno macro as that generates many warnings in QA-C.
                This function will be inlined by the SHC compiler.
                Use this function via the ISERROR macro.

Parameters:     Value to check for error code

Return value:   Non-zero if the value is an error code
***********************************************************************************/

#if   defined (__CC_ARM)
#pragma inline iserror
#elif defined (__ICCARM__)
#pragma inline =forced
#endif
static inline bool_t iserror(const int32_t ser)
{
    uint32_t er = (uint32_t) ser;
    return er >= 0xffffff80u;
}

/**********************************************************************************
Function Name:  error_to_ptr
Description:    Convert an error code to a pointer so we can return an error from
                IOIF driver functions such as open and initialise without getting
                warnings from QA-C.
                This function will be inlined by the SHC compiler.

Parameters:     Error code

Return value:   Error code converted to a pointer
***********************************************************************************/
#if   defined (__CC_ARM)
#pragma inline error_to_ptr
#elif defined (__ICCARM__)
#pragma inline =forced
#endif
static inline void *error_to_ptr(const int32_t er)
{
    return (void*) er;
}

/**********************************************************************************
Function Name:  to_fp_function
Description:    Converts a void (*fn)(VP_INT) function pointer to a FP function pointer
                without getting warnings from QA-C.
                This function will be inlined by the SHC compiler.

Parameters:     Function pointer as void (*fn)(VP_INT)

Return value:   Functino pointer as a FP
***********************************************************************************/
#if   defined (__CC_ARM)
#pragma inline to_fp_function
#elif defined (__ICCARM__)
#pragma inline =forced
#endif
static inline funcptr_t to_fp_function(void (*val)(int32_t))
{
    /* QA-C will generate an error for this line as we are casting
     * between different function pointer types.
     */
    return (funcptr_t)val;
}

#ifdef __cplusplus
}
#endif

#endif /* _MISRATYPES_H_INCLUDED_ */
