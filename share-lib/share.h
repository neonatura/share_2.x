/*
 * @copyright
 *
 *  Copyright 2013 Brian Burrell 
 *
 *  This file is part of the Share Library.
 *  (https://github.com/briburrell/share)
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
 *  Requires: libsvn
 *  Provides: Optimized file system, IPC, and network operations.
 *  Used By: Client programs.
*/  

#ifndef __SHARE_H__
#define __SHARE_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#include "share_version.h"

#include "key/shkey.h"

#include "svn/shsvn.h"
#include "svn/shsvn_hash.h"

#include "fs/shfs.h"
#include "fs/shfs_meta.h"
#include "fs/shfs_read.h"
#include "fs/shfs_write.h"
#include "fs/shfs_adler.h"
#include "fs/shfs_time64.h"

#include "socket/sockbuff.h"
#include "socket/socket.h"
#include "socket/connect.h"
#include "socket/gethost.h"


#ifndef MIN
#define MIN(a,b) \
  (a < b ? a : b)
#endif

#ifndef MAX
#define MAX(a,b) \
  (a > b ? a : b)
#endif


#endif /* ndef __SHARE_H__ */


