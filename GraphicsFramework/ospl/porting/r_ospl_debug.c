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
* @file  r_ospl_debug.c
* @brief   Debug tools provided by OSPL.
*
* $Module: OSPL $ $PublicVersion: 0.90 $ (=R_OSPL_VERSION)
* $Rev: 35 $
* $Date:: 2014-04-15 21:38:18 +0900#$
*/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_ospl.h"
#include  "r_ospl_debug.h"
#if R_OSPL_DEBUG_TOOL
#include  <stdio.h>
#endif

/******************************************************************************
Typedef definitions
******************************************************************************/

/**
* @struct  r_ospl_debug_watch4_t
* @brief  Watch context
*
* @par Description
*    Member variables should not be accessed.
*/
#if R_OSPL_DEBUG_TOOL
typedef struct st_r_ospl_debug_watch4_t  r_ospl_debug_watch4_t;
struct st_r_ospl_debug_watch4_t {
    volatile const uint32_t  *Address;
    uint32_t  BreakValue;
    bool_t    IsPrintf;
};
static r_ospl_debug_watch4_t  g_r_ospl_debug_watch4[ 10 ];
#endif


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


/***********************************************************************
* Implement: R_D_Add
************************************************************************/
#if R_OSPL_DEBUG_TOOL
void  R_D_Add( int_fast32_t IndexNum, volatile const void *in_Address, uint32_t BreakValue, bool_t IsPrintf )
{
    r_ospl_debug_watch4_t  *self = &g_r_ospl_debug_watch4[ IndexNum ];
    bool_t                  was_all_enabled; /* = false; */ /* QAC 3197 */
    int_t                   r;

    was_all_enabled = R_OSPL_DisableAllInterrupt();

    if ( IS( IsPrintf ) ) {
        r= printf( "Debug_add( %d, 0x%08X )\n", IndexNum, (uintptr_t) in_Address );
        R_UNREFERENCED_VARIABLE( r ); /* QAC 3200 : This is not error information */
    }
    self->Address = (uint32_t *) in_Address;
    self->BreakValue = BreakValue;
    self->IsPrintf = IsPrintf;

    if ( IS( was_all_enabled ) ) {
        R_OSPL_EnableAllInterrupt();
    }
}
#endif  /* R_OSPL_DEBUG_TOOL */


/***********************************************************************
* Implement: R_D_Watch
************************************************************************/
#if R_OSPL_DEBUG_TOOL
void  R_D_Watch( int_fast32_t in_IndexNum )
{
    r_ospl_debug_watch4_t  *self = &g_r_ospl_debug_watch4[ in_IndexNum ];
    bool_t                  was_all_enabled; /* = false; */ /* QAC 3197 */
    int_t                   r;

    was_all_enabled = R_OSPL_DisableAllInterrupt();

    if ( self->Address != NULL ) {
        if ( IS( self->IsPrintf ) ) {
            r= printf( "Debug_watch( %d ): 0x%08X\n", in_IndexNum, (uintptr_t) *self->Address );
            R_UNREFERENCED_VARIABLE( r ); /* QAC 3200 : This is not error information */
            /* Cast of "uintptr_t" is for avoiding "format" warning of GNU_ARM */
        }
        if ( *self->Address == self->BreakValue ) {
            R_DEBUG_BREAK();
        }
    }

    if ( IS( was_all_enabled ) ) {
        R_OSPL_EnableAllInterrupt();
    }
}
#endif  /* R_OSPL_DEBUG_TOOL */


/***********************************************************************
* Implement: R_D_AddToIntLog
************************************************************************/
#if R_OSPL_DEBUG_TOOL
#define  IntLogBreakID  -1   /* -1=not break */

volatile uint_fast32_t  g_DebugVar[ g_DebugVarCount ];
volatile int_fast32_t   g_IntLog[ g_IntLogCount ];
volatile int_fast32_t   g_IntLogLength;

void  R_D_AddToIntLog( int_fast32_t in_Value )  /* Add to g_IntLog, g_IntLogLength */
{
    g_IntLog[ (uint_fast32_t) g_IntLogLength % ( sizeof(g_IntLog) / sizeof(*g_IntLog) ) ] = in_Value;
    if ( g_IntLogLength == IntLogBreakID ) {
        R_DEBUG_BREAK();
    }
    g_IntLogLength += 1;
}
#endif  /* R_OSPL_DEBUG_TOOL */


/***********************************************************************
* Implement: R_D_Counter
************************************************************************/
#if R_OSPL_DEBUG_TOOL
bool_t  R_D_Counter( int_fast32_t *in_out_Counter, int_fast32_t TargetCount, char_t *in_Label )
{
    int_fast32_t  counter;
    bool_t        is_stop;
    int_t         r;

    counter = *in_out_Counter;
    counter += 1;
    is_stop = ( counter == TargetCount );
    if ( in_Label != NULL ) {
        r= printf( "%s %d:\n", in_Label, counter );
        R_UNREFERENCED_VARIABLE( r );  /* QAC 3200 : This is not error information */
    } else {
        if ( counter == 1 ) {
            printf( "R_D_Counter address: 0x%08X\n", (uintptr_t) in_out_Counter );
        }
    }
    *in_out_Counter = counter;

    return  is_stop;
}
#endif  /* R_OSPL_DEBUG_TOOL */


