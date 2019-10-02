/******************************************************************************
 *
 *  V. 2.03.00
 * $Rev: $
 * $Date::                           $
 *
 * Description : ioif_aio header file
 *
 * Copyright (C) 2009,2012 Renesas Electronics Corporation. All rights reserved.
 *****************************************************************************/

#ifndef IOIF_AIO_H
#define IOIF_AIO_H

/*************************************************************************
 System Includes
*************************************************************************/

#include <cmsis_os.h>
#include <posix_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************
 User Includes
*************************************************************************/

/*************************************************************************
 Defines
*************************************************************************/

/* sigev_notify values */

#define SIGEV_NONE      (1) /**< No notification */
#define SIGEV_THREAD    (2) /**< A notification function is called in the driver's context */
#define SIGEV_EVENT     (3) /**< An event flag is set */
#define SIGEV_CALLBACK  (4) /**< An inter-process callback is made */

/*************************************************************************
 Constant Macros
*************************************************************************/

/*************************************************************************
 Function Macros
*************************************************************************/

/*************************************************************************
 Typedefs
*************************************************************************/

#if(1) /* mbed */
#else  /* not mbed */
typedef void *      pthread_attr_t;     /**< unsupported */
#endif /* end mbed */

union sigval
{
    int             sival_int;          /**< Integer interpretation */
    void *          sival_ptr;          /**< Pointer interpretation */
};

struct sigevent
{
    int             sigev_notify;       /**< SIGEV_NONE, SIGEV_THREAD, SIGEV_EVENT */
    int             sigev_signo;        /**< signal or callback fp */
    union sigval    sigev_value;        /**< user defined value to pass */
    void            (*sigev_notify_function) (union sigval);
                                        /**< notification function */
};

struct aiocb
{
    /* Implementation specific stuff */
    struct aiocb *  pNext;              /**< next aio request */
    struct aiocb *  pPrev;              /**< previous aio request */
    ssize_t         aio_return;         /**< return value of operation */
    int             aio_complete;       /**< non-zero when op is completed */

    /* User parameters */
    int             aio_fildes;         /**< FD for asynchronous op */
    off_t           aio_offset;         /**< Absolute offset to seek to */
    volatile void*  aio_buf;            /**< Buffer to read from/write to */
    size_t          aio_nbytes;         /**< Number of bytes to read/write */
    struct sigevent aio_sigevent;       /**< Signal to deliver */

    /* Private implementation specific function pointer used by IOIF. Application should not set. */
    ssize_t (*aio_iofn)(void *,void *,size_t,int);  /* synchronous I/O function */
};
typedef struct aiocb AIOCB;

#if(1) /* mbed */
#else  /* not mbed */
struct timespec
{
    time_t   tv_sec;            /* Seconds.  */
    long int tv_nsec;           /* Nanoseconds.  */
};
#endif /* end mbed */

#ifdef __cplusplus
}
#endif

#endif
