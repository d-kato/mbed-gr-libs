/******************************************************************************
 *
 * $Rev: $
 * $Date::                           $
 *
 * Description : ITRON support functions for IOIF Asynchronous I/O header file
 *
 * (C) Copyright RENESAS ELECTRONICS EUROPE Ltd 2012 All Rights Reserved
 *****************************************************************************/

#ifndef AIOIF_H
#define AIOIF_H

#include <ioif_aio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AHF_S
{
    struct aiocb *head;
    struct aiocb *tail;
    uint32_t flags;
    osMutexId_t semid; /* Source Merge 08-81 */
    osMutexAttr_t* p_cmtx;
    int32_t saved_int_mask;
} AHF_S;

#define AHF_CREATESEM   0x1 /* create a semaphore for locking */ /* Source Merge 08-82 */
#define AHF_LOCKSEM     0x2 /* use a semaphore for locking */    /* Source Merge 08-82 */
#define AHF_LOCKINT     0x4 /* use interrupts for locking */

/* AHF (Asynchronous Helper Functions */
int32_t ahf_create (AHF_S * const ahf, const uint32_t f); /* Source Merge 08-83 */
void ahf_destroy (AHF_S const * const ahf);
void ahf_addtail (AHF_S * const ahf, struct aiocb * const aio); /* Source Merge 08-04 */
struct aiocb *ahf_removehead (AHF_S * const ahf);
int32_t ahf_remove (AHF_S *ahf, struct aiocb *aio);
struct aiocb *ahf_peekhead (AHF_S * const ahf);
void ahf_cancelall (AHF_S * const ahf);
void ahf_complete (AHF_S *ahf, struct aiocb * const aio);
int32_t ahf_cancel (AHF_S * const ahf, struct aiocb * const aio);

#ifdef __cplusplus
}
#endif

#endif
