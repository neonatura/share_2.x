
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
#include "sharedaemon.h"

#define STATE_NONE 0
#define STATE_CYCLE 1

int run_state;
int listen_sk;

#define TEST_BUFFER_SIZE 8
void share_server(char *process_path, char *subcmd)
{
  unsigned int port = (unsigned int)SHARE_DAEMON_PORT;//process_socket_port;
  char buff[TEST_BUFFER_SIZE];
  ssize_t b_read, b_write;
  int cli_fd;
  int err;
  int fd;

  if (!*subcmd)
    subcmd = "run";

  err = shfs_proc_lock(process_path, subcmd);
  if (err) {
    printf ("Terminating.. '%s' server '%s' is already running.\n", subcmd, process_path);
    return;
  }

/*
  if (0 == strcmp(subcmd, "ping")) {
    shnet_server_ping();
    return;
  }
*/

  printf ("Initializing '%s' server..\n", subcmd);

  fd = shnet_sk();
  if (fd == -1) {
    perror("shsk");
    return;
  }
  
  err = shnet_bindsk(fd, NULL, port);
  if (err) {
    perror("shbindport");
    shnet_close(fd);
    return;
  }

  listen_sk = fd;

  if (0 == strcmp(subcmd, "run")) {
    run_state = STATE_CYCLE;
  }

  cycle_main(run_state);
#if 0
  printf ("Accepting connections on port %d.\n", port);

  cli_fd = shnet_accept(fd);
  if (cli_fd == -1) {
    perror("shnet_accept");
    shnet_close(fd);
    return;
  }

  printf ("Received new connection on port %d.\n", port);

  memset(buff, 0, sizeof(buff));
  memset(buff, 'a', sizeof(buff) - 1);
  b_write = shnet_write(cli_fd, buff, sizeof(buff));
  if (b_write <= 0) {
    shnet_close(cli_fd);
    shnet_close(fd);
    perror("shnet_write");
return;
  }
  printf ("%d of %d bytes written to port %d on fd %d..\n", b_write, sizeof(buff), port, cli_fd); 

  memset(buff, 0, sizeof(buff));
  b_read = shnet_read(cli_fd, buff, sizeof(buff));
  if (b_read <= 0) {
    perror("shread");
    shnet_close(cli_fd);
    shnet_close(fd);
    return;
  }

  printf ("MESSAGE: %-*.*s\n", b_read, b_read, buff);
  printf ("%d of %d bytes read from port %d on fd %d..\n", b_read, sizeof(buff), port, cli_fd); 
#endif
  
  err = shnet_close(fd);
  if (err) {
    perror("shnet_close");
    shnet_close(cli_fd);
    shnet_close(fd);
    return;
  }

    shnet_close(cli_fd);
    shnet_close(fd);

  return;
}

static int _message_queue;
static shbuf_t *_message_queue_buff;
void cycle_init(void)
{
  _message_queue_buff = shbuf_init();
  _message_queue = shmsgget(NULL);
}

void cycle_term(void)
{
}

void proc_msg(int type, shkey_t *key, unsigned char *data)
{
fprintf(stderr, "DEBUG: proc_msg[type %d]: %s\n", type, data);
}

void cycle_msg_queue(void)
{
  shkey_t msg_key;
  char *data;
  char *ptr;
  uint32_t type;
  size_t len;
  int err;

  /* buffer incoming message */
  err = shmsg_read(_message_queue, &msg_key, _message_queue_buff);
  if (err)
    return;

  if (shbuf_size(_message_queue_buff) <= sizeof(uint32_t)) {
    /* empty */
    shbuf_clear(_message_queue_buff);
    return;
  }

  data = shbuf_data(_message_queue_buff);
  type = *((uint32_t *)data);
  proc_msg(type, &msg_key, (unsigned char *)data + sizeof(uint32_t));
  shbuf_clear(_message_queue_buff);

}

void proc_socket_init(int cli_fd)
{
fprintf(stderr, "DEBUG: proc_socket_init: %d\n", cli_fd);
}

void cycle_socket(fd_set *read_fd, fd_set *write_fd)
{
  int cli_fd;

  if (FD_ISSET(listen_sk, read_fd)) {
    cli_fd = shnet_accept(listen_sk);
    if (cli_fd != -1) {
      proc_socket_init(cli_fd);
    }
  }

}

void cycle_main(int run_state)
{
  fd_set read_fd;
  fd_set write_fd;
  long ms;
  int err;

  cycle_init();

  while (run_state != STATE_NONE) {
    /* check message queue */

    cycle_msg_queue();

    ms = 20;
    FD_ZERO(&read_fd);
    FD_SET(listen_sk, &read_fd);
    FD_ZERO(&write_fd);
    err = shnet_verify(&read_fd, &write_fd, &ms);
    if (err >= 1) {  
      cycle_socket(&read_fd, &write_fd);
    }

  }

  cycle_term();

}


