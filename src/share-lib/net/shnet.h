
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
struct shnet_t 
{
	int fd;
	int flags;
	int rcvbuf_len;
	int sndbuf_len;
  shbuf_t *recv_buff;
  shbuf_t *send_buff;
  struct sockaddr_in addr;
};

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
/** socket */ int shnet_sk(void);
/** socket */ int shnet_socket(int domain, int type, int protocol);
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
 * @}
 */

#endif /* ndef __NET__SHNET_H__ */


