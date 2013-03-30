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

int shbindsk(int sockfd, char *hostname, unsigned int port)
{
  struct sockaddr_in addr;
  struct hostent *host;
  unsigned short sport = (unsigned short)port;

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(sport); 

  host = NULL;
  if (hostname != NULL) 
    host = shpeer(host);
  if (!host) {
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
  } else {
    memcpy(&addr.sin_addr.s_addr, host->h_addr, host->h_length);
  }

  return (shbind(sockfd, &addr, sizeof(addr)));
}

int shbind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
  int err;

  err = bind(sockfd, addr, addrlen);
  if (err)
    return (err);

  err = listen(sockfd, SOMAXCONN);
  if (err)
    return (err);

  return (0);
}

