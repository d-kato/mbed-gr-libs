/******************************************************************************
* Copyright(c) 2012 Renesas Electronics Corporation. All rights reserved.
*
* brief  : Sample code of NCG memory operation functions header.
*
* author : Renesas Electronics Corporation
*
* history: 2013.06.25
*          - Changed for RGAH
*          2013.01.08
*          - Created new code.
*
******************************************************************************/

#ifndef NCG_MEMORY_H
#define NCG_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif


/*=============================================================================
 * Definitions
 */


/*=============================================================================
 *  Structures
 */


/*=============================================================================
 *  Function prototyping
 */

/*
 * Host memory management
 */

NCGvoid
NCGSYS_PurgeCPUOperandCache (
    void
);

NCGuint32
NCGSYS_CPUVAddrToSysPAddr (
    NCGuint32   ui32CPUAddr
);

#ifdef __cplusplus
}
#endif

#endif /* NCG_MEMORY_H */
