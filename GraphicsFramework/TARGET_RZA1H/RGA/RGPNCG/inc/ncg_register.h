/******************************************************************************
* Copyright(c) 2010-2012 Renesas Electronics Corporation. All rights reserved.
*
* brief  : NCG register read/write header.
*
* author : Renesas Electronics Corporation
*
* history: 2010.10.08
*          - Created the initial code.
*          2012.08.22
*          - Updated coding format.
*
*******************************************************************************/

#ifndef NCG_REGISTER_H
#define NCG_REGISTER_H

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
 *  Register read/write
 */
NCGuint32
NCGSYS_ReadReg (
    NCGuint32   ui32RegBase,
    NCGuint32   ui32Offset
);

NCGvoid
NCGSYS_WriteReg (
    NCGuint32   ui32RegBase,
    NCGuint32   ui32Offset,
    NCGuint32   ui32Value
);

NCGuint32
NCGSYS_ModifyReg (
    NCGuint32   ui32RegBase,
    NCGuint32   ui32Offset,
    NCGuint32   ui32Mask,
    NCGuint32   ui32Value
);


#ifdef __cplusplus
}
#endif

#endif /* NCG_REGISTER_H */
