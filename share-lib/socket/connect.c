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

#include "../share.h"

int shconn(int sk, char *host, unsigned short port, int async)
{
	struct hostent *peer = shpeer(host);
	int err = shsocket(AF_INET, SOCK_STREAM, 0);
	if (err)
		return (-errno);
	if (!async)
		shfcntl(sk, F_SETFL, O_NONBLOCK);
	return (0);
}

int shconnect(int sk, struct sockaddr *skaddr, socklen_t skaddr_len) 
{
	int err;
	unsigned short usk = (unsigned short)sk;

	if (!(_sk_table[usk].flags & SHSK_ESP)) {
		err = connect(sk, skaddr, skaddr_len);
	} else {
		err = -EPROTONOSUPPORT;
	}

	return (err);
}


