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



int shnet_verify(fd_set *readfds, fd_set *writefds, long *millis)
{
  struct timeval tv;
  int err;

  memset(&tv, 0, sizeof(tv));
  tv.tv_sec = (*millis/1000); /* ms -> second */
  tv.tv_usec = (*millis%1000) * 1000; /* ms -> microsecond */
  err = select(nfds,readfds,writefds,NULL,&tv);
  *millis = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);

  return (err);
}

int shnet_select(int nfds, fd_set *readfds, fd_set *writefds,
    fd_set *exceptfds, struct timeval *timeout)
{
  return (select(nfds,readfds,writefds,exceptfds,timeout));
}
