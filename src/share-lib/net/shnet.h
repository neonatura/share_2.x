
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
 */

#ifndef __SOCKET__SHNET_H__ 
#define __SOCKET__SHNET_H__

/**
 * The libshare Socket Handling provides access to regular socket operations with posix or convienence functions in addition to access to the ESP network protocol.
 * The ESP protocol performs streaming compression and provides more security than TCP.
 * @brief The libshare network socket operations. 
 * @ingroup libshare
 * @defgroup libshare_net Network socket operations.
 * @{
 */


#ifndef USHORT_MAX
#define USHORT_MAX 65536
#endif

/** socket is not closed */
#define SHNET_ALIVE             (1 << 0)
/** include IP hdr on transmission */
#define SHNET_IPHDR             (1 << 1)
/** enable Encoded Stream Protocol */
#define SHNET_ESP               (1 << 2)
/** do not block client calls */
#define SHNET_ASYNC             (1 << 3) 

/*
 * A socket stream with a remote host peer.
 */
typedef struct shnet_t shnet_t;

/*
 * A socket stream with a remote host peer.
 */
struct shnet_t {
	int fd;
	int flags;
	int rcvbuf_len;
	int sndbuf_len;
  shbuf_t *recv_buff;
  shbuf_t *send_buff;
  struct sockaddr_in addr;
};

#include "shnet_accept.h"
#include "shnet_bind.h"
#include "shnet_socket.h"
#include "shnet_connect.h"
#include "shnet_gethost.h"
#include "shnet_fcntl.h"
#include "shnet_write.h"
#include "shnet_select.h"

/**
 * @}
 */

#endif /* ndef __SOCKET__SHNET_H__ */


