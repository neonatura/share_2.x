
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
 *
 */

#ifndef __NET__SHNET_H__ 
#define __NET__SHNET_H__

#ifndef SHARELIB
#include <sys/socket.h>
#include <netinet/in.h>
#endif

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

#define IPPROTO_SHNET 145

/** socket is not closed */
#define SHNET_ALIVE             (1 << 0)
/** do not block client calls */
#define SHNET_ASYNC             (1 << 2) 
/** socket is bound to listen on port. */
#define SHNET_LISTEN            (1 << 3)
/** user-level emulation of network protocol */
#define SHNET_EMULATE           (1 << 4)


#define SHNET_DEFAULT_DEVICE "eth0"

/*
 * A socket stream with a remote host peer.
 */
typedef struct shnet_t shnet_t;

/*
 * A socket stream with a remote host peer.
 */
struct shnet_t 
{
	int fd;
	int flags;
  int protocol; /* IPPROTO_UDP | IPPROTO_TCP | IPPROTO_SHNET */

	int rcvbuf_len;
	int sndbuf_len;
  shbuf_t *recv_buff;
  shbuf_t *send_buff; /* not used */

  shpeer_t src_addr;
  shpeer_t dst_addr;
};

#define TX_NONE    0
#define TX_IDENT   1
#define TX_PEER    2 
#define TX_FILE    3
#define TX_WALLET  4
/**
 *  * A ward can be placed on another transaction to prohibit from being used.
 *   * @note Applying the identical ward causes the initial ward to be removed.
 *    */
#define TX_WARD 5
#define TX_SIGNATURE 6
#define TX_LEDGER 7
#define TX_APP 8

#define TX_ACCOUNT 9
#define TX_TASK 10
#define TX_THREAD 11
#define TX_TRUST 12

/**
 * An event specifies when a transaction will become valid.
 * @see shicald
 */
#define TX_EVENT 13

#define TX_SESSION 14

#define TX_LICENSE 15

/** The maximum number of transaction operations supported. */
#define MAX_TX 15



/** accept */ int shnet_accept(int sockfd);
/** bind */ int shnet_bindsk(int sockfd, char *hostname, unsigned int port);
/** bind */ int shnet_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
/** close */ int shnet_close(int sk);
/** connect */ int shnet_conn(int sk, char *host, unsigned short port, int async);
/** fcntl */ int shnet_fcntl(int fd, int cmd, long arg);
/** gethost */ struct hostent *shnet_gethostbyname(char *name);
/** gethost */ struct hostent *shnet_peer(char *name);
/** read */ ssize_t shnet_read(int fd, const void *buf, size_t count);
/** socket */ extern shnet_t _sk_table[USHORT_MAX];

/** 
 * Creates a standard IPv4 TCP socket.
 */
int shnet_sk(void);

/** 
 * Create a network socket.
 * @param domain Either AF_INET or AF_INET6.
 * @param type Only SOCK_STREAM is supported.
 * @param protocol Either IPPROTO_TCP or IPPROTO_SHNET. 
 */
int shnet_socket(int domain, int type, int protocol);

/** socket */ struct sockaddr *shnet_host(int sockfd);
/** write */ ssize_t shnet_write(int fd, const void *buf, size_t count);

/**
 * Waits on the specified read/write socket streams and marks which are available for an IO operation.
 * @see shnet_select()
 */
int shnet_verify(fd_set *readfds, fd_set *writefds, long *millis);

/**
 * Performs a POSIX select() against a set of @ref shnet_t socket streams.
 */
int shnet_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);


/**
 * Initiate a socket connection to a libshare peer entity.
 */
int shnet_conn_peer(shpeer_t *peer, int async);

/**
 * Obtain the internal socket buffer used for a network connection.
 */
shbuf_t *shnet_read_buf(int fd);

/**
 * @}
 */

#endif /* ndef __NET__SHNET_H__ */


