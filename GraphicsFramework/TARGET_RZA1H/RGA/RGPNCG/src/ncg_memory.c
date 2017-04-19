/******************************************************************************
* Copyright(c) 2010-2012 Renesas Electronics Corporation. All rights reserved.
*
* brief  : NCG memory operation functions.
*
* author : Renesas Electronics Corporation
*
* history: 2013.06.25
*          - Changed for RGAH
*          2010.10.08
*          - Created the initial code.
*          2012.08.23
*          - Created the initial code.
*          2014.12.22
*          - Applied to OSPL and RGA.
*
*******************************************************************************/

/*=============================================================================
 * Includes
 */

#include "Project_Config.h"
#include "ncg_defs.h"
#include "ncg_memory.h"

#ifdef  RGAH_VERSION

/*=============================================================================
 * Internal definitions
 */


/*----------------------------------------------------------------------------
 NAME       : NCGSYS_CPUVAddrToSysPAddr
 FUNCTION   : Get the system physical address of the given host memory block.
 PARAMETERS : ui32CPUAddr  : [IN ] The address of the host memory area.
 RETURN     : Phy.
------------------------------------------------------------------------------*/
NCGuint32
NCGSYS_CPUVAddrToSysPAddr (
    NCGuint32   ui32CPUAddr )
{
    NCGuint32   ui32Ret_addr;

    ui32Ret_addr = ui32CPUAddr;

    return ui32Ret_addr;
}


#endif
/* -- end of file -- */
