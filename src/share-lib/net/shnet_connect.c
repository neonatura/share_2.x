
/*
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
*/

#include "share.h"
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

int shconnect(int sk, struct sockaddr *skaddr, socklen_t skaddr_len) 
{
	unsigned short usk = (unsigned short)sk;
  socklen_t len;
	int err;

  err = connect(sk, skaddr, skaddr_len);
  if (err == -1 && errno != EINPROGRESS)
    return (-errno);

  
  len = sizeof(_sk_table[usk].addr_src);
  getsockname(sk, &_sk_table[usk].addr_src, &len);
  memcpy(&_sk_table[usk].addr_dst,
      skaddr, MIN(sizeof(struct sockaddr), skaddr_len));
#if 0
{
struct sockaddr_in pin;
struct sockaddr_in *in;
socklen_t len;
len = sizeof(pin);
getpeername(sk, &pin, &len);
fprintf(stderr, "DEBUG: shnet_connect: peer(%s)\n", inet_ntoa(pin.sin_addr));
in = (struct sockaddr_in *)&_sk_table[usk].addr_src;
fprintf(stderr, "DEBUG: shnet_connect: src(%s)\n", inet_ntoa(in->sin_addr));
in = (struct sockaddr_in *)&_sk_table[usk].addr_dst;
fprintf(stderr, "DEBUG: shnet_connect: dst(%s)\n", inet_ntoa(in->sin_addr));
}
#endif

  if (err == -1)
    return (SHERR_INPROGRESS);

/*
	if (!(_sk_table[usk].flags & SHSK_ESP)) {
		err = connect(sk, skaddr, skaddr_len);
	} else {
		err = -EPROTONOSUPPORT;
	}
*/

	return (0);
}

int shconnect_peer(shpeer_t *peer, int flags)
{
  struct sockaddr_in net_addr;
  int err;
  int fd;

  if (!peer)
    return (SHERR_INVAL);

  if (peer->type != SHNET_PEER_IPV4 &&
      peer->type != SHNET_PEER_LOCAL)
    return (SHERR_OPNOTSUPP);

  fd = shnet_sk();
  if (fd == -1)
    return (-errno);

  if (flags & SHNET_CONNECT) {
    shnet_fcntl(fd, F_SETFL, O_NONBLOCK);
  }

  memset(&net_addr, 0, sizeof(net_addr));
  net_addr.sin_family = AF_INET;//peer->addr.sin_family;
  net_addr.sin_port = peer->addr.sin_port;
  memcpy(&net_addr.sin_addr, &peer->addr.sin_addr[0], sizeof(struct in_addr));
  err = shconnect(fd, (struct sockaddr *)&net_addr, sizeof(net_addr)); 
  if (err && err != SHERR_INPROGRESS) {
    shnet_close(fd);
    return (err);
  }

  if ((flags & SHNET_ASYNC) && !(flags & SHNET_CONNECT)) {
    shnet_fcntl(fd, F_SETFL, O_NONBLOCK);
  }

  return (fd);
}

_TEST(shconnect_peer)
{
#ifdef linux
  shpeer_t *peer;
  fd_set fds;
  char buf[256];
  long ms;
  int err;
  int sk;

  /* connect to portmapper */
  peer = shpeer_init(NULL, "127.0.0.1:111");
  sk = shconnect_peer(peer, SHNET_CONNECT | SHNET_ASYNC);
  _TRUE(sk > 0);
  shpeer_free(&peer);

  ms = 100;
  FD_ZERO(&fds);
  FD_SET(sk, &fds);
  err = shnet_verify(NULL, &fds, &ms);
  _TRUE(err == 1);

  err = read(sk, buf, 0);
  _TRUE(err == 0);

  _TRUE(0 == close(sk));
#endif
}

int shconnect_host(char *host, unsigned short port, int flags)
{
  struct sockaddr_in addr;
  struct hostent *peer;
  int err;
  int sk;

  peer = shresolve(host);
  if (!peer) {
    return (-1);
  }

  sk = shnet_sk();
  if (sk == -1)
    return (-1);

  if (flags & SHNET_CONNECT)
    shnet_fcntl(sk, F_SETFL, O_NONBLOCK);

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  memcpy(&addr.sin_addr.s_addr, peer->h_addr, peer->h_length);
  err = shconnect(sk, (struct sockaddr *)&addr, sizeof(addr));
  if (err)
    return (-1);

  if (flags & SHNET_ASYNC)
    shnet_fcntl(sk, F_SETFL, O_NONBLOCK);

  return (sk);
}



