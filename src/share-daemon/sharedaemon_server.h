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

#ifndef __SHAREDAEMON_SERVER_H__
#define __SHAREDAEMON_SERVER_H__

typedef struct sock_t
{
  int fd;
  shbuf_t *out_buff;
  struct sock_t *next;
} sock_t;


extern int listen_sk;
extern sock_t *sock_client_list;


void sharedaemon_server(char *subcmd);

void cycle_main(int run_state);


#endif /* __SHAREDAEMON_SERVER_H__ */

