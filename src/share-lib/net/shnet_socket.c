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

shnet_t _sk_table[USHORT_MAX];

int shnet_sk(void)
{
	return (shnet_socket(AF_INET, SOCK_STREAM, 0));
}

int shnet_socket(int domain, int type, int protocol)
{
	int err;
	int sk;
	int val;
	int flags;
	int val_len;
	unsigned short usk;

	if (domain != PF_INET)
		return -EAFNOSUPPORT;
 
	if (type != SOCK_STREAM)
		return -EPROTONOSUPPORT;

	if (protocol != 0 && protocol != IPPROTO_TCP)
		return -EPROTONOSUPPORT;

	flags = 0;

	//sk = socket(AF_INET, SOCK_RAW, 0); 
sk = -1; /* DEBUG: */
	if (sk == -1) {
		sk = socket(AF_INET, SOCK_STREAM, 0);
	} else {
		flags |= SHNET_ESP;
	}
	if (sk == -1)
		return (-1);
	flags |= SHNET_ALIVE;

#if 0
	err = fcntl(sk, F_SETFL, O_NONBLOCK);
	if (err) {
		close(sk);
		return (-1);
	}
#endif

#ifdef SO_HDRINCL
	val = 0;
	err = setsockopt(sk, SOL_SOCKET, SO_HDRINCL, &val, sizeof(val));
	if (!err)
		flags |= SHNET_IPHDR;
#endif

	val = (flags & SHNET_IPHDR) ? 50652 : 50630;
	setsockopt(sk, SOL_SOCKET, SO_SNDBUF, &val, sizeof(val));
	val = (flags & SHNET_IPHDR) ? 87380 : 87356;
	setsockopt(sk, SOL_SOCKET, SO_RCVBUF, &val, sizeof(val));

	usk = (unsigned short)sk;
	_sk_table[usk].fd = sk;
	_sk_table[usk].flags = flags;

#ifdef LINUX
	/* safe compatible minimum */
	_sk_table[usk].rcvbuf_len = 16360;
	_sk_table[usk].sndbuf_len = 16360;
#endif

	val_len = sizeof(int);
	err = getsockopt(sk, SOL_SOCKET, SO_RCVBUF, &val, &val_len);
	if (!err)
		_sk_table[usk].rcvbuf_len = val;

	val_len = sizeof(int);
	err = getsockopt(sk, SOL_SOCKET, SO_SNDBUF, &val, &val_len);
	if (!err)
		_sk_table[usk].sndbuf_len = val;

	return (sk);
} 


struct sockaddr *shnet_host(int sockfd)
{
  unsigned int usk;

  if (sockfd < 1)
    return (NULL);

  usk  = (unsigned int)sockfd;
  if (!(_sk_table[usk].flags & SHNET_ALIVE))
    return (NULL);

  return ((struct sockaddr *)&_sk_table[usk].addr);
}

