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

#include "shnet.h"

void shnet_server(char *subcmd)
{
  unsigned int port = (unsigned int)process_socket_port;
  char buff[TEST_BUFFER_SIZE];
  ssize_t b_read, b_write;
  int cli_fd;
  int err;
  int fd;

  if (!*subcmd)
    subcmd = "test";

  err = shfs_proc_lock(process_path, subcmd);
  if (err) {
    printf ("Terminating.. '%s' server '%s' is already running.\n", subcmd, process_path);
    return;
  }

  if (0 == strcmp(subcmd, "ping")) {
    shnet_server_ping();
    return;
  }

  printf ("Initializing '%s' server..\n", subcmd);

  fd = shsk();
  if (fd == -1) {
    perror("shsk");
    return;
  }
  
  err = shbindsk(fd, NULL, port);
  if (err) {
    perror("shbindport");
    shclose(fd);
    return;
  }

  printf ("Accepting connections on port %d.\n", port);

  cli_fd = shaccept(fd);
  if (cli_fd == -1) {
    perror("shaccept");
    shclose(fd);
    return;
  }

  printf ("Received new connection on port %d.\n", port);

  memset(buff, 0, sizeof(buff));
  memset(buff, 'a', sizeof(buff) - 1);
  b_write = shwrite(cli_fd, buff, sizeof(buff));
  if (b_write <= 0) {
    shclose(cli_fd);
    shclose(fd);
    perror("shwrite");
return;
  }
  printf ("%d of %d bytes written to port %d on fd %d..\n", b_write, sizeof(buff), port, cli_fd); 

  memset(buff, 0, sizeof(buff));
  b_read = shread(cli_fd, buff, sizeof(buff));
  if (b_read <= 0) {
    perror("shread");
    shclose(cli_fd);
    shclose(fd);
    return;
  }

  printf ("MESSAGE: %-*.*s\n", b_read, b_read, buff);
  printf ("%d of %d bytes read from port %d on fd %d..\n", b_read, sizeof(buff), port, cli_fd); 
  
  err = shclose(fd);
  if (err) {
    perror("shclose");
    shclose(cli_fd);
    shclose(fd);
    return;
  }

    shclose(cli_fd);
    shclose(fd);

  return (0);
}


