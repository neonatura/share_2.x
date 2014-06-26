
/*
 * @copyright
 *
 *  Copyright 2013 Brian Burrell 
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
 *
 *  @file share.h 
 *  @brief The Share Library
 *
 *  Provides: Optimized file system, IPC, and network operations.
 *  Used By: Client programs.
*/  


#ifndef __SHARE_MACRO_H__
#define __SHARE_MACRO_H__





#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif

#ifndef MIN
#define MIN(a,b) \
  (a < b ? a : b)
#endif

#ifndef MAX
#define MAX(a,b) \
  (a > b ? a : b)
#endif

#ifdef HAVE_MKDIR
#define MKDIR(_path) (mkdir(_path, 0777))
#else
#define MKDIR(_path)
#endif

#ifndef STAT 
#define STAT(_path, _info) (-1) /* no-op */
#endif

#ifndef FCNTL
#define FCNTL(_fd, _mode, _opt) (-1)
#endif

#ifdef HAVE_FOPEN
#define FSTAT(_fl, _st) fstat(fileno(_fl), _st)
#define FSEEK(_fl, _where, _whence) fseek(_fl, _where, _whence)
#else
#define FILE sh_stdio_t
typedef struct sh_stdio_t {
  int fd; /* posix */ 
} sh_stdio_t;

#define FSTAT(_fl, _st) fstat(_fl, _st)
#define FSEEK(_fl, _where, _whence) ftell(_fl, _where, _whence)
#endif

#if defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_GETRUSAGE) && defined(SH_DEBUG)
#include <sys/resource.h>
static struct rusage _share_rusage;
#define PRINT_RUSAGE(_msg) \
  ((memset(&_share_rusage, 0, sizeof(_share_rusage))) ? \
   (0 == getrusage(RUSAGE_SELF, &_share_rusage)) ? \
   fprintf(stderr, "%s [cpu(user:%d.%-6.6ds sys:%d.%-6.6ds maxrss(%uk) ixrss(%uk) idrss(%uk) flt(%uk) swaps(%uk) in-ops(%uk) out-ops(%uk) errno(%d)]\n", \
     ((_msg) ? (_msg) : "SYSTEM"), \
     _share_rusage.ru_utime.tv_sec, _share_rusage.ru_utime.tv_usec, \
     _share_rusage.ru_stime.tv_sec, _share_rusage.ru_stime.tv_usec, \
     _share_rusage.ru_maxrss, _share_rusage.ru_ixrss, _share_rusage.ru_idrss, \
     _share_rusage.ru_majflt, _share_rusage.ru_nswap, \
     _share_rusage.ru_inblock, _share_rusage.ru_oublock, \
     errno) : -1 : -1)
#else
#define PRINT_RUSAGE(_msg) (-1)
#endif

#define PRINT_ERROR(_err, _msg) \
  fprintf(stderr, "Error: %s [code %d: %s].\n", \
									strerror(-(_err)), (_err), _msg)

#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK 0x7f000001
#endif

#if !defined(HAVE_GETTID) && defined(HAVE_PTHREAD_SELF)
#define gettid() \
  ((pid_t)pthread_self())
#endif

#endif /* ndef __SHARE_MACRO_H__ */
