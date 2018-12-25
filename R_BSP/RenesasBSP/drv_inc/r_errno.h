/*******************************************************************************
 *
 * $Rev: $
 * $Date::                           $
 *
 * Description : errno for POSIX IEEE Std 1003.1-2001
 *
 * (C) Copyright RENESAS ELECTRONICS Ltd 2012 All Rights Reserved
 *****************************************************************************/

#ifndef SDK_ERRNO
#define SDK_ERRNO

/* Include std headers to override _errno with SDK version */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

#define _REENTRANT 1

/***********************************************************************************
 System Includes
***********************************************************************************/
/* SDK errno extension, SDK returns 0 as success, and -1 when errno set */
#define ESUCCESS    (0)
#define EERROR      (-1)

#if(1) /* mbed */
#define EBADF_RBSP       (-17) /* Bad file descriptor */
#define EINVAL_RBSP      (-28) /* Invalid argument */
#define ENOMEM_RBSP      (-33) /* No memory available */
#define ENOENT_RBSP      (-42) /* No such file or directory */
#define EACCES_RBSP      (-64) /* Access denied */
#define EBUSY_RBSP       (-67) /* Device or resource busy */
#define ECANCELED_RBSP   (-68) /* Operation was cancelled */
#define EIO_RBSP         (-74) /* Input or Output error (possibly recoverable)*/
#define EMFILE_RBSP      (-76) /* Too many open files */
#define ENOSPC_RBSP      (-84) /* No space available on device */
#define ENOTSUP_RBSP     (-87) /* Not supported */
#define EPERM_RBSP       (-90) /* Operation not permitted */
#define EFAULT_RBSP      (-97) /* Section fault, bad address */
#define EOVERFLOW_RBSP  (-117) /* Value too large to store in data type */
#else  /* not mbed */
#define EBADF       (-17) /* Bad file descriptor */
#define EINVAL      (-28) /* Invalid argument */
#define ENOMEM      (-33) /* No memory available */
#define ENOLCK      (-34) /* No lock available */
#define ENOENT      (-42) /* No such file or directory */
#define ETIMEDOUT   (-50) /* Timed out */
#define EACCES      (-64) /* Access denied */
#define EAGAIN      (-65) /* Resource unavailable. Try again */
#define EBUSY       (-67) /* Device or resource busy */
#define ECANCELED   (-68) /* Operation was cancelled */
#define EDOM        (-69) /* Maths argument out of domain */
#define EEXIST      (-70) /* File, directory or mount exists */
#define EFBIG       (-71) /* File too big */
#define EINPROGRESS (-72) /* Operation in progress */
#define EIO         (-74) /* Input or Output error (possibly recoverable)*/
#define EISDIR      (-75) /* Is a directory */
#define EMFILE      (-76) /* Too many open files */
#define ENAMETOOLONG (-77) /* File name too long */
#define ENFILE      (-78) /* Too many files are open in the system */
#define ENOBUFS     (-79) /* No buffers available */
#define ENODEV      (-80) /* Not a valid device */
#define ENOSPC      (-84) /* No space available on device */
#define ENOTDIR     (-85) /* Not a directory */
#define ENOTEMPTY   (-86) /* Directory not empty */
#define ENOTSUP     (-87) /* Not supported */
#define ENOTTY      (-88) /* No TTY available */
#define ENXIO       (-89) /* Not valid IO address or device (unrecoverable)*/
#define EPERM       (-90) /* Operation not permitted */
#define EROFS       (-91) /* Read only file system */
#define ESPIPE      (-92) /* Invalid seek on pipe */
#define EXDEV       (-93) /* Device cross linked */
#define ENOTSOCK    (-94) /* Not a socket */
#define EMSGSIZE    (-95) /* Message too large */
#define EINTR       (-96) /* Function interrupted */
#define EFAULT      (-97) /* Section fault, bad address */
#define EPIPE       (-98) /* Broken pipe */
#define EALREADY    (-99) /* Connection already started */
#define EDESTADDRREQ (-100) /* Destination address requred */
#define EPROTOTYPE  (-101) /* Wrong protocol or codec type */
#define ENOPROTOOPT (-102) /* Protocol or codec not available */
#define EPROTONOSUPPORT (-103) /* Protocol or codec not supported */
#define EOPNOTSUPP  (-104) /* Operation not supported */
#define EAFNOSUPPORT (-105) /* Address family not supported */
#define EADDRINUSE  (-106) /* Address already in use */
#define EADDRNOTAVAIL (-107) /* Address not available */
#define ECONNRESET  (-108) /* Connection reset */
#define EISCONN     (-109) /* Connected */
#define ENOTCONN    (-110) /* Not connected */
#define ECONNREFUSED (-112) /* Connection refused */
#define EPROCLIM    (-113) /* Too many processes */
#define EUSERS      (-114) /* Too many users */
#define EDEADLK     (-115) /* Resource deadlock would occur */
#define EPROTO      (-116) /* Protocol or codec error */
#define EOVERFLOW   (-117) /* Value too large to store in data type */
#define ESRCH       (-118) /* No such task */
#define EILSEQ      (-119) /* Illegal byte sequence format */
#define EBADMSG     (-120) /* Bad message */
#define ENOSYS      (-121) /* Function not supported */
#define ECRC        (-122) /* CRC failure */
#define EEOF        (-123) /* Position is End Of File */
#define ESEEKPASTEND (-124) /* Seek past EOF */
#define EMEDIUMTYPE (-127) /* Wrong medium type */
#define ENOMEDIUM   (-128) /* No medium present */
#define ESEEKRANGE  (-129) /* seek setting error */  /* Source Merge 11-1 */
#endif /* end mbed */

/***********************************************************************************
 Defines
***********************************************************************************/
/* errno is a macro so it is re-entrant (per thread/task value) - this
 * definition works with SH C libs.  The errno returns an l-value so
 * it can be used on the left of an assignment as it is legal for an
 * application to assign to errno.
 *
 * Note: Application code should not call ioif_get_errno directly. All
 * access to errno should be made using the POSIX "errno" macro.
*/
extern int *ioif_get_errno(void);

/* Remove SHC definitions so SDK can be used instead */
#ifdef _errno
    #undef _errno
#endif

#define _errno (*ioif_get_errno())

#ifndef  errno
    #define  errno _errno
#endif

#define iserrno(e)((unsigned int)(e) >= 0xffffff80UL)

#ifdef __cplusplus
}
#endif

#endif
