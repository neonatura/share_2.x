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

#ifndef __SOCKET__SOCKET_H__
#define __SOCKET__SOCKET_H__

#ifndef USHORT_MAX
#define USHORT_MAX 65536
#endif

/* shsk flags */
#define SHSK_ALIVE             (1 << 0) /* socket is not closed */
#define SHSK_IPHDR             (1 << 1) /* include IP hdr on transmission */
#define SHSK_ESP               (1 << 2) /* enable Encoded Stream Protocol */
#define SHSK_ASYNC             (1 << 3) /* do not block client calls */

typedef struct shsk_s {
	int fd;
	int flags;
	int rcvbuf_len;
	int sndbuf_len;
} shsk_s;

extern shsk_s _sk_table[USHORT_MAX];

int shsk(void);
int shsocket(int domain, int type, int protocol);

#endif /* ndef __SOCKET__SOCKET_H__ */


