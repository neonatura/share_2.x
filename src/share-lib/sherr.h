
/*
 * @copyright
 *
 *  Copyright 2013 Neo Natura 
 *
 *  This file is part of the Share Library.
 *  (https://github.com/neonatura/share)
 *        
 *  The Share Library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version. 
 *
 *  The Share Library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with The Share Library.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  @endcopyright
 */  


#ifndef __SHERR_H__
#define __SHERR_H__

#include <errno.h>

/**
 * The libshare error codes.
 * @ingroup libshare
 * @defgroup libshare_error Generic libshare error status codes.
 * @{
 */

#define SHERR_NONE 0
#define SHERR_NOENT -2 /* ENOENT */
#define SHERR_SRCH -3 /* ESRCH */
#define SHERR_IO -5 /* EIO */
#define SHERR_2BIG -7 /* E2BIG */
#define SHERR_BADF -9 /* EBADF */
#define SHERR_AGAIN -11 /* EAGAIN */
#define SHERR_NOMEM -12 /* ENOMEM */
#define SHERR_ACCESS -13 /* EACCES */
#define SHERR_EXIST -17 /* EEXIST */
#define SHERR_NOTDIR -20 /* ENOTDIR */
#define SHERR_ISDIR -21 /* EISDIR */
#define SHERR_INVAL -22 /* EINVAL */
#define SHERR_NFILE -23  /* ENFILE (File table overflow) */
#define SHERR_FBIG -27 /* EFBIG */
#define SHERR_NOSPC -28 /* ENOSPC (No space left on device) */
#define SHERR_NAMETOOLONG -36 /* ENAMETOOLONG (File name too long) */
#define SHERR_NOLCK -37 /* No record locks available */
#define SHERR_NOMSG -42 /* ENOMSG */
#define SHERR_XFULL -54 /* EXFULL (Exchange full) */
#define SHERR_OVERFLOW -75 /* Value too large for defined value type */
#define SHERR_NOTUNIQ -76 /* Name not unique on network */
#define SHERR_ILSEQ -84 /* Illegal [byte] sequence. */
#define SHERR_CONNRESET -104 /* Connection reset by peer. */
#define SHERR_NOBUFS -105 /* No buffer space available. */
#define SHERR_TIMEDOUT -110 /* Conenction timed out */
#define SHERR_CONNREFUSED -111 /* Conenction refused */
#define SHERR_ALREADY -114 /* Operation already in progress */ 
#define SHERR_REMOTEIO -121 /* EREMOTEIO */
#define SHERR_TIME -62 /* Timer expired */
#define SHERR_NONET -64  /* Machine is not on the network */
#define SHERR_NOPKG -65  /* Package not installed */
#define SHERR_REMOTE -66  /* Object is remote */
#define SHERR_NOLINK -67  /* Link has been severed */
#define SHERR_PROTO -71 /* Protocol error */
#define SHERR_NOTSOCK -88  /* Socket operation on non-socket */
#define SHERR_OPNOTSUPP -95 /* Operation not supported */
#define SHERR_ADDRINUSE -98  /* Address already in use */
#define SHERR_ADDRNOTAVAIL -99  /* Cannot assign requested address */
#define SHERR_NETDOWN -100 /* Network is down */
#define SHERR_NETUNREACH -101 /* Network is unreachable */
#define SHERR_SHUTDOWN -108 /* Cannot send after transport endpoint shutdown */
#define SHERR_TOOMANYREFS -109 /* Too many references: cannot splice */
#define SHERR_INPROGRESS -115 /* Operation now in progress */
#define SHERR_NOMEDIUM -123 /* No medium found */

/** Operation canceled */
#define SHERR_CANCELED -125

/** Required key not available */
#define SHERR_NOKEY -126

/** Key has expired */
#define SHERR_KEYEXPIRED -127

/** Key has been revoked */
#define SHERR_KEYREVOKED -128

/** Key was rejected by service */
#define SHERR_KEYREJECTED -129

/** Owner died */
#define SHERR_OWNERDEAD -130


/**
 * Converts a libshare error code to a libshare error message.
 * @param _errcode A libshare error code.
 * @returns A string message associated with the libshare error code.
 */
#define sherrstr(_errcode) \
  strerror(-1 * (_errcode))


/**
 * @}
 */

#endif /* ndef __SHERR_H__ */


