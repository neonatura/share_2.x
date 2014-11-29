
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
  char buff[TEST_BUFFER_SIZE];
  ssize_t b_read, b_write;
  int cli_fd;
  int err;

  if (!*subcmd)
    subcmd = "run";

#if 0
  err = shfs_proc_lock(process_path, subcmd);
  if (err) {
    printf ("Terminating.. '%s' server '%s' is already running.\n", subcmd, process_path);
    return;
  }
#endif

/*
  if (0 == strcmp(subcmd, "ping")) {
    shnet_server_ping();
    return;
  }
*/

//  printf ("Initializing '%s' server..\n", subcmd);


  run_state = STATE_CYCLE;

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
  

  return;
}

static int _message_queue;
static shbuf_t *_message_queue_buff;
void cycle_init(void)
{
  _message_queue_buff = shbuf_init();
  _message_queue = shmsgget(NULL);
}

void proc_msg(int type, shkey_t *key, unsigned char *data, size_t data_len)
{
  shpeer_t *peer;
  char ebuf[512];
  int err;

  switch (type) {
    case TX_APP: /* app registration */
      peer = (shpeer_t *)(data + sizeof(uint32_t));
      err = sharedaemon_msgclient_init(key, peer);
fprintf(stderr, "DEBUG: proc_msg[TX_APP]: %d = sharedaemon_msgclient_init(key %s, peer %s)\n", err, shkey_print(key), shpeer_print(peer));
      if (err) {
        sprintf(ebuf, "proc_msg: TX_APP: %s [sherr %d, key %s].", 
            str_sherr(err), err, shkey_print(key));
        sherr(err, ebuf); 
      }
      break;
    case TX_PEER: /* peer registration */
      peer = (shpeer_t *)(data + sizeof(uint32_t));
fprintf(stderr, "DEBUG: proc_msg[TX_PEER]: key %s, peer %s\n", shkey_print(key), shpeer_print(peer));
/* DEBUG: todo: add listener for receiving peer info on <key> app via msgq */
      break;
    default:
      fprintf(stderr, "DEBUG: proc_msg[type %d]: %s\n", type, data);
      break;
  }

}

void cycle_msg_queue(void)
{
  shkey_t msg_key;
  char *data;
  char *ptr;
  size_t data_len;
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
  data_len = shbuf_size(_message_queue_buff);
fprintf(stderr, "DEBUG: cycle_msg_queue: shmsg_read <%d bytes>\n", data_len); 

  type = *((uint32_t *)data);
  proc_msg(type, &msg_key, (unsigned char *)data + sizeof(uint32_t), data_len);
  shbuf_clear(_message_queue_buff);

}

void broadcast_raw(unsigned char *data, size_t data_len)
{
  shd_t *user;

  for (user = sharedaemon_client_list; user; user = user->next) {
    shbuf_cat(user->buff_out, data, data_len);
  }

}

void cycle_term(void)
{

}


void cycle_socket(fd_set *read_fd, fd_set *write_fd)
{
  shd_t *cli;
  shbuf_t *rbuf;
  ssize_t len;
  int cli_fd;

  if (FD_ISSET(listen_sk, read_fd)) {
    cli_fd = shnet_accept(listen_sk);
    if (cli_fd != -1) {
      sharedaemon_netclient_init(cli_fd,
          (struct sockaddr_in *)shnet_host(cli_fd));
    }
  }

  /* incoming socket data */
  for (cli = sharedaemon_client_list; cli; cli = cli->next) {
    if (!(cli->flags & SHD_CLIENT_NET) ||
        cli->cli.net.fd == 0)
      continue;
    if (FD_ISSET(cli->cli.net.fd, read_fd)) {
      rbuf = shnet_read_buf(cli->cli.net.fd);
      if (rbuf) {
        shbuf_append(rbuf, cli->buff_in);
        shbuf_clear(rbuf);
      }
    }
  }
  /* outgoing socket data */
  for (cli = sharedaemon_client_list; cli; cli = cli->next) {
    if (!(cli->flags & SHD_CLIENT_NET) ||
        cli->cli.net.fd == 0)
      continue;
    if (FD_ISSET(cli->cli.net.fd, write_fd)) {
      len = shnet_write(cli->cli.net.fd, shbuf_data(cli->buff_out), shbuf_size(cli->buff_out)); 
      if (len > 0) {
        shbuf_trim(cli->buff_out, len);
      } else {
        close(cli->cli.net.fd);
        cli->cli.net.fd = 0; /* mark for removal */
      }
    }
    shnet_write_flush(cli->cli.net.fd);
  } 

}

void cycle_main(int run_state)
{
  fd_set read_fd;
  fd_set write_fd;
  shd_t *cli;
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

    for (cli = sharedaemon_client_list; cli; cli = cli->next) {
      if (!(cli->flags & SHD_CLIENT_NET) ||
          cli->cli.net.fd == 0)
        continue;
      FD_SET(cli->cli.net.fd, &read_fd);
      if (shbuf_size(cli->buff_out) != 0)
        FD_SET(cli->cli.net.fd, &write_fd);
    }

    err = shnet_verify(&read_fd, &write_fd, &ms);
    if (err >= 1) {  
      cycle_socket(&read_fd, &write_fd);
    }

  }

  cycle_term();

}


