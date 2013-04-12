
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

#include "share.h"

int shnet_conn(int sk, char *host, unsigned short port, int async)
{
  struct sockaddr_in addr;
	struct hostent *peer;
	int err;

  peer = shnet_peer(host);
  if (!peer)
    return (-1);

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

  memcpy(&_sk_table[usk].addr, skaddr, sizeof(struct sockaddr_in)); /* v4 */

/*
	if (!(_sk_table[usk].flags & SHSK_ESP)) {
		err = connect(sk, skaddr, skaddr_len);
	} else {
		err = -EPROTONOSUPPORT;
	}
*/

	return (err);
}


