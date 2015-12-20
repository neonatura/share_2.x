
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


struct hostent *shresolve(char *hostname)
{
	struct hostent *host;

  if (!hostname || !*hostname)
    return (NULL);

	host = gethostbyname(hostname);
	if (!host)
		return (NULL);

	return (host);
}

struct sockaddr *shaddr(int sockfd)
{
  static struct sockaddr ret_addr;
  socklen_t addr_len;
  int err;

  addr_len = sizeof(ret_addr);
  memset(&ret_addr, 0, addr_len);
  err = getsockname(sockfd, &ret_addr, &addr_len);
  if (err)
    return (NULL);

  return (&ret_addr);
}

const char *shaddr_print(struct sockaddr *addr)
{
  static char ret_text[1024];
  struct sockaddr_in6 *in6;
  struct sockaddr_in *in;
  sa_family_t in_fam;
  char *ptr;

  if (!addr)
    return (NULL); /* error */

  in_fam = *((sa_family_t *)addr);
  memset(ret_text, 0, sizeof(ret_text));
  switch (in_fam) {
    case AF_INET:
      in = (struct sockaddr_in *)addr;
      sprintf(ret_text, "%s:%d", inet_ntoa(in->sin_addr), ntohs(in->sin_port));
      break;
    case AF_INET6:
      in6 = (struct sockaddr_in6 *)addr;
      ptr = (char *)&in6->sin6_addr;
      sprintf(ret_text,
          "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x %d",
          (int)ptr[0], (int)ptr[1], (int)ptr[2], (int)ptr[3],
          (int)ptr[4], (int)ptr[5], (int)ptr[6], (int)ptr[7],
          (int)ptr[8], (int)ptr[9], (int)ptr[10], (int)ptr[11],
          (int)ptr[12], (int)ptr[13], (int)ptr[14], (int)ptr[15],
          (int)ntohs(in6->sin6_port));
  }

  return (ret_text);
}



