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
#include "shnet.h"

static int is_running;

typedef struct shnet_ping_s {
  uint64_t stamp;
  uint64_t index;
} shnet_ping_s;

static void spawn_ping_server(void)
{
  static int init;
  char sysbuf[4096];
  int err;

  if (init) {
    printf ("Skipping respawn of server..\n");
    return;
  }
  init = TRUE;
  
  err = fork();
  if (err != 0)
    return;

  sprintf(sysbuf, "%s server ping", process_path);
  err = system(sysbuf);
  exit (err);
}

void shnet_ping(char *subcmd)
{
  unsigned short port = (unsigned short)process_socket_port;
  shnet_ping_s ctrl;
  char data[TEST_BUFFER_SIZE];
  int fd;
  int err;
  int is_conn;
  ssize_t r_len;
  ssize_t w_len;
  uint64_t ping_idx;

  if (!*subcmd)
    subcmd = "127.0.0.1";
  if (!port)
    port = SHARE_PING_PORT;
  
  fprintf(stdout, "Pinging..\n");

  memset(&ctrl, 0, sizeof(ctrl));

  fd = 0;
  ping_idx = 0;
  is_conn = FALSE;
  is_running = TRUE;
  while (is_running) {
    ping_idx++;

    if (!is_conn) {
      fd = shsk();
      err = shconn(fd, subcmd, port, FALSE);
      if (!err) {
        is_conn = TRUE;
        printf ("Connected to port %u on host '%s'.\n", port, subcmd);
      } else if (0 == strcmp(subcmd, "127.0.0.1") || 0 == strcmp(subcmd, "localhost")) {
        spawn_ping_server();
      } 
    }

    if (is_conn) {
      ctrl.stamp = shtime64();
      ctrl.index = ping_idx;
      memset(data, 'a', TEST_BUFFER_SIZE);
      memcpy(data, &ctrl, sizeof(ctrl));

      w_len = shwrite(fd, data, TEST_BUFFER_SIZE);
      if (w_len < 1) {
        shclose(fd);
        is_conn = FALSE;
        continue;
      }
      printf ("Wrote %d byte ping.\n", w_len);

      r_len = shread(fd, data, TEST_BUFFER_SIZE);
      if (r_len < 1) {
        shclose(fd);
        is_conn = FALSE;
        continue;
      }
      printf ("Read %d byte ping.\n", r_len);
      
    }

    sleep(1);
  }

  shclose(fd);

}


void shnet_server_ping(void)
{
  unsigned int port = (unsigned int)process_socket_port;
  char buff[TEST_BUFFER_SIZE];
  struct shnet_ping_s ctrl;
  ssize_t b_read, b_write;
  int is_running;
  int cli_fd;
  int err;
  int fd;

  if (!port) {
    port = SHARE_PING_PORT; 
  }


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

  printf ("[PING] Accepting connections on port %d.\n", port);

  cli_fd = shaccept(fd);
  if (cli_fd == -1) {
    perror("shaccept");
    shclose(fd);
    return;
  }

  printf ("[PING] Received new connection on port %d.\n", port);

  is_running = TRUE;
  while (is_running) {
    memset(buff, 0, sizeof(buff));
    b_read = shread(cli_fd, buff, sizeof(buff));
    if (b_read <= 0) {
      perror("shread");
      break;
    }
    printf ("%d of %d bytes read from port %d on fd %d..\n", b_read, sizeof(buff), port, cli_fd); 

    memcpy(&ctrl, buff, sizeof(ctrl));
    ctrl.stamp = shtime64();
    memcpy(buff, &ctrl, sizeof(ctrl));

    b_write = shwrite(cli_fd, buff, sizeof(buff));
    if (b_write <= 0) {
      perror("shwrite");
      break;
    }
    printf ("%d of %d bytes written to port %d on fd %d..\n", b_write, sizeof(buff), port, cli_fd); 

  }

  shclose(cli_fd);
  shclose(fd);

  return (0);
}

