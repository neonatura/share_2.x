
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
#include "sharetool.h"

void share_scan(int argc, char **argv)
{
  int idx;
  int i;

  for (i = 0; i < 3; i++) {
    shnet_scan_net();
  }
  
}

void shnet_scan_net(void)
{
  unsigned short port;
  int fd;
  int err;
  
  fprintf(stdout, "Scanning..\n");
  for (port = 2; port <= 1024; port++) {
    fd = shnet_sk();
    if (fd == -1) {
      perror("shsk");
      continue;
    }
    err = shnet_conn(fd, "127.0.0.1", port, FALSE);
    shnet_close(fd);
    if (err) {
      //perror("shnet_conn");
      continue;
    }

    printf ("* Listening port %d found on 'localhost'.\n", (int)port);
  }

}


