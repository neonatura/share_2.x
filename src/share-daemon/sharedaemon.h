
/*
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
 */  

#ifndef __SHAREDAEMON_H__
#define __SHAREDAEMON_H__
/**
 *  @brief Share Transaction Service 
 *  @note The Share Library is hosted at https://github.com/briburrell/share
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
#include "sharedaemon_scan.h"
#include "sharedaemon_server.h"
#include "sharedaemon_ping.h"
#include "sharedaemon_test.h"

#define SHARE_DAEMON_PORT 1369

/* psuedo-standard ports for shnet operations */
#define SHARE_PING_PORT 32200

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
  shpeer()

#define broadcast_raw(_data, _data_len) \
  fprintf(stderr, "RAW[%-*.*s]\n", (_data_len), (_data_len), (_data))

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

