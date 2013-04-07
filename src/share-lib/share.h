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
 *  @author Brian Burrell
 *  @date 2013
 *
 *  Provides: Optimized file system, IPC, and network operations.
 *  Used By: Client programs.
 */  


#ifndef __SHARE_H__
#define __SHARE_H__

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif

#include <fcntl.h>
#include <math.h>
#include <sys/types.h>

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#define STAT stat
#endif

#include <sys/socket.h>
#include <netinet/in.h>


#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#define FCNTL(_fd, _mode, _opt) fcntl(_fd, _mode, _opt)
#endif

#include "share_macro.h"

/**
 *
 * This documentation covers the public API provided by the Share library. The documentation is for developers of 3rd-party applications intending to use this API. 
 *
 * @mainpage ShareLib Documentation
 * @brief The Share Library
 * @note The Share Library is hosted at https://github.com/briburrell/share
 * @defgroup libshare 
 * @{
 */

// See the libshare_meta.3 API man page for meta definition hash maps.


/**
 * A specification of byte size.
 * @manonly
 * See the libshare_socket.3 API man page for ESP protocol network operations.
 * @endmanonly
 * @seealso shmeta_value_t.sz
 */
typedef uint64_t shsize_t;

/**
 * The local machine.
 */
#define SHSK_PEER_LOCAL 0

/**
 * A remote IPv4 network destination.
 */
#define SHSK_PEER_IPV4 1

/**
 * A remote IPv6 network destination.
 */
#define SHSK_PEER_IPV6 2

/**
 * A IPv4 network destination on the sharenet VPN.
 */
#define SHSK_PEER_VPN_IPV4 3

/**
 * A IPv6 network destination on the sharenet VPN.
 */
#define SHSK_PEER_VPN_IPV6 4

/**
 * The local or remote machine associated with the sharefs partition.
 * @manonly
 * See the libshare_socket.3 API man page for ESP protocol network operations.
 * @endmanonly
 * @note Addresses are stored in network byte order.
 */
typedef struct shpeer_t {
  /**
   * A SHSK_PEER_XX type
   */
  int type;

  /**
   * A IP 4/6 network address
   */
  union {
    uint32_t ip;
    uint64_t ip6;
  } addr;
} shpeer_t;

/**
 * An email address where bug reports can be submitted.
 */
char *get_libshare_email(void);

/**
 * The current libshare library version.
 */
char *get_libshare_version(void);

/**
 * The libshare library package name.
 */
char *get_libshare_title(void);

#include "shtime.h"
#include "shcrc.h"
#include "shkey.h"
#include "shbuf.h"
#include "meta/shmeta.h"
#include "shpref.h"
#include "fs/shfs.h"
#include "socket/shsk.h"
#include "test/shtest.h"


/**
 * @}
 */

#endif /* ndef __SHARE_H__ */


