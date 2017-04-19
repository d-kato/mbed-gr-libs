/******************************************************************************
* Copyright(c) 2010-2012 Renesas Electronics Corporation. All rights reserved.
*
* brief  : NCG time and delay control functions header.
*
* author : Renesas Electronics Corporation
*
* history: 2010.10.08
*          - Created the initial code.
*          2012.08.22
*          - Updated coding format.
*
*******************************************************************************/

#ifndef NCG_TIME_H
#define NCG_TIME_H

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
 *  Delay control
 */
NCGint32
NCGSYS_Sleep (
    NCGuint32   ui32MilliSeconds
);

/*
 *  Time acquisition
 */

NCGint32
NCGSYS_StartTickCount (
    void
);

NCGuint32
NCGSYS_GetTickCount (
    void
);


#ifdef __cplusplus
}
#endif

#endif /* __NCG_TIME_H__ */
