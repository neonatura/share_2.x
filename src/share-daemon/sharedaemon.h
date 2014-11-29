
/*
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
 */  

#ifndef __SHAREDAEMON_H__
#define __SHAREDAEMON_H__
/**
 *  @brief Share Transaction Service 
 *  @note The Share Library is hosted at https://github.com/neonatura/share/
 *  @defgroup sharedaemon
 *  @{
 */

#undef __STRICT_ANSI__ 

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

#include "bits/bits.h"

#include "sharedaemon_file.h"
#include "sharedaemon_client.h"
#include "sharedaemon_server.h"
#include "sharedaemon_app.h"

#define SHARE_DAEMON_PORT 32080
/**
 * The maximum number of additional gateway ports allowed.
 * @note This effictively limits the share daemon's maximum number of network clients to 16384 connections.
 */
#define MAX_SHARE_GATEWAY_PORTS 16

/** server run modes */
#define PROC_SERVE "serve"

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE !FALSE
#endif

#ifndef MIN
#define MIN(a,b) \
  (a < b ? a : b)
#endif

#ifndef MAX
#define MAX(a,b) \
  (a > b ? a : b)
#endif

#define sharedaemon_peer() \
  server_peer

extern shpeer_t *server_peer;
extern sh_account_t *server_account;
extern sh_ledger_t *server_ledger;

/*
extern char process_path[PATH_MAX + 1];
extern char process_file_path[PATH_MAX + 1];
extern char process_socket_host[PATH_MAX + 1];
extern unsigned int process_socket_port;
*/

/**
 *  @}
 */

#endif /* __SHAREDAEMON_H__ */

