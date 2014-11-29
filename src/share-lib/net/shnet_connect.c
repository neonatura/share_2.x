
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

#include "share.h"

int shnet_conn(int sk, char *host, unsigned short port, int async)
{
  struct sockaddr_in addr;
	struct hostent *peer;
	int err;

  peer = shnet_peer(host);
  if (!peer) {
    return (-1);
}

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  memcpy(&addr.sin_addr.s_addr,
 peer->h_addr,
 peer->h_length);

  err = shnet_connect(sk, (struct sockaddr *)&addr, sizeof(addr));
	if (err)
    return (err);

	if (async)
		shnet_fcntl(sk, F_SETFL, O_NONBLOCK);

	return (0);
}

int shnet_connect(int sk, struct sockaddr *skaddr, socklen_t skaddr_len) 
{
	int err;
	unsigned short usk = (unsigned short)sk;

  err = connect(sk, skaddr, skaddr_len);
  if (err)
    return (err);

  if (_sk_table[usk].dst_addr.addr.sin_family == AF_INET) {
    struct sockaddr_in *sin = (struct sockaddr_in *)skaddr;
    _sk_table[usk].dst_addr.addr.sin_port = sin->sin_port;
    memcpy(&_sk_table[usk].dst_addr.addr.sin_addr[0], &sin->sin_addr, sizeof(uint32_t));
  }

/*
	if (!(_sk_table[usk].flags & SHSK_ESP)) {
		err = connect(sk, skaddr, skaddr_len);
	} else {
		err = -EPROTONOSUPPORT;
	}
*/

	return (0);
}

int shnet_conn_peer(shpeer_t *peer, int async)
{
  struct sockaddr_in net_addr;
  int err;
  int fd;

  if (peer->type == SHNET_PEER_IPV4 ||
      peer->type == SHNET_PEER_LOCAL) {
    memset(&net_addr, 0, sizeof(net_addr));
    net_addr.sin_family = peer->addr.sin_family;
    net_addr.sin_port = peer->addr.sin_port;
    memcpy(&net_addr.sin_addr, &peer->addr.sin_addr[0], sizeof(struct in_addr));

    fd = shnet_sk();
    if (fd == -1)
      return (-errno);
    err = shnet_connect(fd, (struct sockaddr *)&net_addr, sizeof(net_addr)); 
    if (err) {
      shnet_close(fd);
      return (err);
    }
    if (async)
      shnet_fcntl(fd, F_SETFL, O_NONBLOCK);
    return (fd);
  }

  return (SHERR_INVAL);
}


