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


int shnet_accept(int sockfd)
{
  struct sockaddr peer_addr;
  unsigned int usk;
  socklen_t peer_len;
  ssize_t sk;
  int err;

  peer_len = (socklen_t)sizeof(peer_addr);
  memset(&peer_addr, 0, sizeof(peer_addr));
  sk = accept(sockfd, &peer_addr, &peer_len);
  if (sk == -1)
    return (-1);

/*
  if (addr) {
    memcpy(peer, &peer_addr, peer_len);
  }
  if (addrlen_p) {
    *addrlen_p = peer_len;
  }
*/

  usk = (unsigned int)sockfd;
  memcpy(&_sk_table[usk].addr, &peer_addr, peer_len);

  return (sk);
}

