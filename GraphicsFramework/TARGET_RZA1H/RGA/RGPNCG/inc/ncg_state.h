/******************************************************************************
* Copyright(c) 2010-2012 Renesas Electronics Corporation. All rights reserved.
*
* brief  : NCG state variable control functions header.
*
* author : Renesas Electronics Corporation
*
* history: 2010.10.08
*          - Created the initial code.
*          2012.08.22
*          - Updated coding format.
*
*******************************************************************************/

#ifndef NCG_STATE_H
#define NCG_STATE_H

#ifdef __cplusplus
extern "C" {
#endif


/*=============================================================================
 * Definitions
 */

/*
 *  Mode Flags
 */

#define NCGSYS_STATE_CALL_MASK       (0x00000003U)
#define NCGSYS_STATE_CALL_NORMAL     (0x00000000U)
#define NCGSYS_STATE_CALL_INTERRUPT  (0x00000001U)
#define NCGSYS_STATE_CALL_CALLBACK   (0x00000002U)

#define NCGSYS_STATE_WAIT_MASK       (0x00000004U)
#define NCGSYS_STATE_WAIT_OR         (0x00000004U)
#define NCGSYS_STATE_WAIT_AND        (0x00000000U)

#define NCGSYS_STATE_SET_MASK        (0x00000018U)
#define NCGSYS_STATE_SET_AND         (0x00000000U)
#define NCGSYS_STATE_SET_OR          (0x00000008U)
#define NCGSYS_STATE_SET_SET         (0x00000010U)

#define NCGSYS_STATE_OR             NCGSYS_STATE_SET_OR
#define NCGSYS_STATE_SET            NCGSYS_STATE_SET_SET
#define NCGSYS_STATE_CLEAR          NCGSYS_STATE_SET_AND


/*=============================================================================
 *  Structures
 */


/*=============================================================================
 *  Function prototyping
 */

/*
 *  State control
 */

NCGint32
NCGSYS_CreateState (
    NCGvoid     **ppObj,
    NCGuint32   ui32StateID
);

NCGint32
NCGSYS_DestroyState (
    NCGvoid     *pObj
);

NCGint32
NCGSYS_SetState (
    NCGvoid     *pObj,
    NCGuint32   ui32State,
    NCGuint32   ui32Flags
);

NCGuint32
NCGSYS_GetState (
    NCGvoid     *pObj,
    NCGuint32   ui32Flags
);

NCGint32
NCGSYS_WaitState (
    NCGvoid     *pObj,
    NCGuint32   ui32State,
    NCGuint32   ui32Flags,
    NCGuint32   ui32Timeout
);

NCGvoid
NCGSYS_SetStateEventValue (
    NCGvoid    *pObj,
    NCGuint32   ui32EventValue
);

NCGvoid *
NCGSYS_GetLastCreatedState(void);


#ifdef __cplusplus
}
#endif

#endif /* NCG_STATE_H */
