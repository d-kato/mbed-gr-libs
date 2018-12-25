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
* Copyright (C) 2009,2012 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

/******************************************************************************
 *
 *  V. 2.03.00
 * $Rev: $
 * $Date::                           $
 *
 * Description : POSIX types
 *
 *****************************************************************************/

#ifndef POSIXTYPES_H_INCLUDED
#define POSIXTYPES_H_INCLUDED

/***********************************************************************************
 System Includes
***********************************************************************************/


/***********************************************************************************
 User Includes
***********************************************************************************/


/***********************************************************************************
 Defines
***********************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/* From <unistd.h> */
#ifndef SEEK_SET
#define SEEK_SET        0           /* from the biginning of file */
#define SEEK_CUR        1           /* from the current position in file */
#define SEEK_END        2           /* from the end of file */
#endif

/* <unistd.h> POSIX Standard: 2.10 file descriptors for stdin, stdout,
  stderr not included with SHC. Therefore defined below. */
#ifndef STDIN_FILENO
    #define STDIN_FILENO (0)
#endif

#ifndef STDOUT_FILENO
    #define STDOUT_FILENO (1)
#endif

#ifndef STDERR_FILENO
    #define STDERR_FILENO (2)
#endif


/* These values must be as defined under open in the compiler manual
 * otherwise the buffered file functions fopen() etc. will not work.
 */
#if(1) /* mbed */
#define O_ACCMODE_RBSP  00000007
#define O_RDONLY_RBSP   00000001
#define O_WRONLY_RBSP   00000002
#define O_RDWR_RBSP     00000004
#else  /* not mbed */
#define O_ACCMODE       00000007
#define O_RDONLY        00000001
#define O_WRONLY        00000002
#define O_RDWR          00000004
#define O_CREAT         00000010
#define O_EXCL          00000020
#define O_TRUNC         00000040
#define O_APPEND        00002000
#define O_NONBLOCK      00004000
#define O_SYNC          00010000
#endif /* end mbed */

/***********************************************************************************
 Constant Macros
***********************************************************************************/


/***********************************************************************************
 Function Macros
***********************************************************************************/


/***********************************************************************************
 Typedefs and Structures
***********************************************************************************/

/* The POSIX standard states that:-
    mode_t                              shall be an integer type.
    nlink_t, uid_t, gid_t, and id_t     shall be integer types.
    blkcnt_t and off_t                  shall be signed integer types.
    fsblkcnt_t, fsfilcnt_t, and ino_t   shall be defined as unsigned integer types.
    size_t                              shall be an unsigned integer type.
    blksize_t, pid_t, and ssize_t       shall be signed integer types.
    time_t and clock_t                  shall be integer or real-floating types.
*/

#if(1) /* mbed */
#ifndef off_t
typedef long off_t;
#endif
#ifndef ssize_t
typedef int ssize_t;
#endif
#else  /* not mbed */
typedef int fsid_t;
typedef long time_t;    /* Time in seconds. */
typedef long long off_t;
typedef unsigned int mode_t;
typedef unsigned int ino_t;
typedef int nlink_t;
typedef int uid_t;
typedef int gid_t;
typedef int dev_t;
typedef int ssize_t;
#endif /* end mbed */

#if !defined(__cplusplus) && defined(__STRICT_ANSI__)
#if defined (__GNUC__)
typedef unsigned int size_t;
#else /*__GNUC__*/
typedef unsigned long size_t;
#endif/*__GNUC__*/
#endif

#ifdef __cplusplus
}
#endif

#endif
