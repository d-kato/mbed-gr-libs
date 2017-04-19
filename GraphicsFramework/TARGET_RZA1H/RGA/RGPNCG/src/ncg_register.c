/******************************************************************************
* Copyright (c) 2010-2012 Renesas Electronics Corporation. All rights reserved.
*
* brief  : NCG register read/write Functions
*
* author : Renesas Electronics Corporation.
*
* history: 2013.06.25
*          - Changed for RGAH
*          2010.10.08
*          - Created the initial code.
*          2012.08.22
*          - Updated coding format.
*          2014.12.22
*          - Applied to OSPL and RGA.
*
*******************************************************************************/

/*=============================================================================
 * Includes
 */

#include "Project_Config.h"
#include "ncg_defs.h"
#include "ncg_register.h"

#ifdef  RGAH_VERSION

/*=============================================================================
 * Internal definitions
 */


/*=============================================================================
 *  Prototyping of internal functions
 */


/*=============================================================================
 *  Private global variables and functions
 */


/*=============================================================================
 *  Global Function
 */

/*----------------------------------------------------------------------------
 NAME       : NCGSYS_ReadReg
 FUNCTION   : Read a hardware register.
 PARAMETERS : ui32RegBase : [IN ] Base address.
              ui32Offset  : [IN ] Address offset.
 RETURN     : Register value.
------------------------------------------------------------------------------*/
NCGuint32
NCGSYS_ReadReg (
    NCGuint32       ui32RegBase,
    NCGuint32       ui32Offset )
{
    NCGuint32 ui32RegVal;

    ui32RegVal = *(((volatile NCGuint32 *)ui32RegBase) + (ui32Offset>>2));

    return ui32RegVal;
}

/*----------------------------------------------------------------------------
 NAME       : NCGSYS_WriteReg
 FUNCTION   : Write a hardware register.
 PARAMETERS : ui32RegBase : [IN ] Base address.
              ui32Offset  : [IN ] Address offset.
              ui32Value   : [IN ] Register value.
 RETURN     : None.
------------------------------------------------------------------------------*/
NCGvoid
NCGSYS_WriteReg (
    NCGuint32       ui32RegBase,
    NCGuint32       ui32Offset,
    NCGuint32       ui32Value )
{
    *(((volatile NCGuint32 *)ui32RegBase) + (ui32Offset>>2)) = ui32Value;
}

#endif
/* -- end of file -- */
