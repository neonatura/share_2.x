
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

int listen_tx(int tx_op, shd_t *cli, shkey_t *peer_key)
{

  cli->op_flags[tx_op] |= SHOP_LISTEN;

  return (0);
}

void proc_msg(int type, shkey_t *key, unsigned char *data, size_t data_len)
{
  tx_peer_t peer_tx;
  tx_sig_t sig_tx;
  shfs_hdr_t *fhdr;
  shd_t *cli;
  shpeer_t *peer;
  char ebuf[512];
  int err;

  cli = sharedaemon_client_find(key);
fprintf(stderr, "DEBUG: %x = proc_msg(type:%d, key:%s, data-len:%d)\n", cli, type, shkey_print(key), data_len);

  switch (type) {
    case TX_APP: /* app registration */
      peer = (shpeer_t *)data;
      if (0 != memcmp(key, shpeer_kpub(peer), sizeof(shkey_t))) {
        err = SHERR_ACCESS;
      } else {
        err = sharedaemon_msgclient_init(peer);
      }
      if (err) {
        sprintf(ebuf, "proc_msg: TX_APP: %s [sherr %d, key %s].", 
            str_sherr(err), err, shkey_print(key));
fprintf(stderr, "DEBUG: %s\n", ebuf);
        sherr(err, ebuf); 
      }
      break;
    case TX_PEER: /* peer registration */
      peer = (shpeer_t *)data;
//fprintf(stderr, "DEBUG: proc_msg[TX_PEER]: (peer->type %d): %s\n", peer->type, shpeer_print(peer));

      if (peer->type == SHNET_PEER_LOCAL) {
        /* non-previleged key -- requesting priv peers */
        listen_tx(TX_PEER, cli, shpeer_kpub(peer));
        break;
      } 

      memset(&peer_tx, 0, sizeof(peer_tx));
      err = generate_peer_tx(&peer_tx, peer); 
      if (err) {
        sprintf(ebuf, "proc_msg: TX_PEER: generate_peer_tx: "
            "%s [sherr %d, key %s].", 
            str_sherr(err), err, shkey_print(key));
fprintf(stderr, "DEBUG: proc_msg: error: %s\n", ebuf);
        sherr(err, ebuf); 
        break;
      }

      err = process_peer_tx(key, &peer_tx);
      if (err) {
        sprintf(ebuf, "proc_msg: TX_PEER: %s [sherr %d, key %s].", 
            str_sherr(err), err, shkey_print(key));
fprintf(stderr, "DEBUG: proc_msg: error: %s\n", ebuf);
        sherr(err, ebuf); 
      }
      break;

    case TX_FILE: /* remote file notification */
      peer = (shpeer_t *)data;
      fhdr = (shfs_hdr_t *)(data + sizeof(shpeer_t));
      fprintf(stderr, "DEBUG: proc_msg[TX_FILE]: key %s, peer %s, file "
          " %s %-4.4x:%-4.4x size(%lu) crc(%lx) mtime(%-20.20s)",
          shkey_print(key), shpeer_print(peer), 
          shfs_inode_type(fhdr->type),
          fhdr->pos.jno, fhdr->pos.ino,
          (unsigned long)fhdr->size,
          (unsigned long)fhdr->crc,
          shctime64(fhdr->mtime)+4);
      break;

#if 0
    case TX_SIGNATURE:
      sig = (shsig_t *)data;
      memset(&sig_tx, 0, sizeof(sig_tx));
      err = process_signature_tx(key, &sig_tx);
      if (err) {
        sprintf(ebuf, "proc_msg: TX_SIGNATURE: %s [sherr %d, key %s].", 
            str_sherr(err), err, shkey_print(key));
        sherr(err, ebuf); 
      }
      break;
#endif

    default:
fprintf(stderr, "DEBUG: proc_msg[type %d]: %s\n", type, data);
      break;
  }

}

static void cycle_msg_queue_in(void)
{
  shkey_t msg_key;
  char *data;
  char *ptr;
  size_t data_len;
  uint32_t type;
  size_t len;
  int err;

  /* buffer incoming message */
  memset(&msg_key, 0, sizeof(msg_key));
  err = shmsg_read(_message_queue, &msg_key, _message_queue_buff);
  if (err) {
if (err != SHERR_NOMSG) fprintf(stderr, "DEBUG: cycle_msg_queue_in: err %d\n", err);
    return;
}

  if (shbuf_size(_message_queue_buff) <= sizeof(uint32_t)) {
    /* empty */
    shbuf_clear(_message_queue_buff);
fprintf(stderr, "DEBUG: cycle_msg_queue_in: empty message received.\n");
    return;
  }

  data = shbuf_data(_message_queue_buff);
  data_len = shbuf_size(_message_queue_buff);

  type = *((uint32_t *)data);
fprintf(stderr, "DEBUG: cycle_msg_queue: [type %d] shmsg_read <%d bytes>\n", type, data_len); 
  proc_msg(type, &msg_key, (unsigned char *)data + sizeof(uint32_t), data_len);
  shbuf_clear(_message_queue_buff);

}

static void cycle_msg_queue_out(void)
{
  tx_peer_t *peer;
  shbuf_t *buff;
  shd_t *cli;
  tx_t *tx;
  uint32_t mode;
  int err;

  for (cli = sharedaemon_client_list; cli; cli = cli->next) {
    if (!(cli->flags & SHD_CLIENT_MSG))
      continue;
    if (shbuf_size(cli->buff_out) < sizeof(tx_t))
      continue;

    tx = (tx_t *)shbuf_data(cli->buff_out);
//fprintf(stderr, "DEBUG: cycle_msg_queue_out: tx op %d\n", tx->tx_op);
    switch (tx->tx_op) {

      case TX_PEER:
        if (shbuf_size(cli->buff_out) < sizeof(tx_peer_t)) {
          shbuf_clear(cli->buff_out);
          break;
        }


        peer = (tx_peer_t *)shbuf_data(cli->buff_out);

        mode = TX_PEER;
        buff = shbuf_init();
        shbuf_cat(buff, &mode, sizeof(mode));
        shbuf_cat(buff, &peer->peer, sizeof(shpeer_t));
        err = shmsg_write(_message_queue, buff, &cli->cli.msg.msg_key);
        if (err) fprintf(stderr, "DEBUG: cycle_msg_queue_out[TX_PEER]: error(%d): shmsg_write(%s, <%d bytes>)\n", err, shkey_print(&cli->cli.msg.msg_key), shbuf_size(buff));
        shbuf_free(&buff);

        shbuf_trim(cli->buff_out, sizeof(tx_peer_t));
        break; 

      default:
fprintf(stderr, "DEBUG: cycle_msg_queue_out: unknown tx op %d\n", tx->tx_op);
        shbuf_clear(cli->buff_out);
        break;
    }
  }

}

void cycle_msg_queue(void)
{
  cycle_msg_queue_in();
  cycle_msg_queue_out();
}

int broadcast_filter(shd_t *user, tx_t *tx)
{

#if 0
  if ((user->flags & SHD_CLIENT_REGISTER) &&
      0 == memcmp(&user->app->app_name, &tx->tx_peer, sizeof(shkey_t))) {
    /* supress broadcast to originating user */
fprintf(stderr, "DEBUG: broadcast_filter: cli %x skipped - origin of transaction.\n");
    return (SHERR_INVAL);
  }
#endif

  if (user->flags & SHD_CLIENT_MSG) { /* ipc msg */
    if (user->op_flags[tx->tx_op] & SHOP_LISTEN)
      return (0); /* user is listening to op mode */

fprintf(stderr, "DEBUG: broadcast_filter: (ipc msg !listen) tx->tx_op(%d) tx->tx_peer(%s)\n", tx->tx_op, shkey_print(&tx->tx_peer));
    return (SHERR_INVAL);
  }

  return (0);
}
        
void broadcast_raw(void *raw_data, size_t data_len)
{
  unsigned char *data = (unsigned char *)raw_data;
  tx_t *tx = (tx_t *)data;
  shd_t *user;

  for (user = sharedaemon_client_list; user; user = user->next) {
fprintf(stderr, "DEBUG: broadcast_raw: client %s (#%x)\n", shkey_print(shpeer_kpub(&user->peer.peer)), user);

    if (0 != broadcast_filter(user, tx)) {
fprintf(stderr, "DEBUG: broadcast_raw: skipping user [flags %d, msg %s]\n", user->flags, (user->flags & SHD_CLIENT_MSG) ? "msg" : "sk");
      continue;
    }

    shbuf_cat(user->buff_out, data, data_len);
//fprintf(stderr, "DEBUG: broadcast_raw[tx-op %d]: buff_out <%d bytes> (%d pend) to cli %x [flags %d]\n", tx->tx_op, data_len, shbuf_size(user->buff_out)-data_len, user, user->flags);
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

void cycle_client_request(shd_t *cli)
{
  tx_ledger_t *ledger;
  size_t len;
  tx_t *tx;
  char ebuf[1024];
  int err;

  if (shbuf_size(cli->buff_in) < sizeof(tx_t))
    return;

  err = 0;
  tx = (tx_t *)shbuf_data(cli->buff_in);
fprintf(stderr, "DEBUG: cycle_client_request: cli-app:%x tx_op:%d\n", cli->app, tx->tx_op);
  switch (tx->tx_op) {
    case TX_IDENT:
      if (shbuf_size(cli->buff_in) < sizeof(tx_id_t))
        break; 
      err = process_identity_tx(cli->app, (tx_id_t *)shbuf_data(cli->buff_in));
      shbuf_trim(cli->buff_in, sizeof(tx_id_t));
      break;
    case TX_PEER:
      if (shbuf_size(cli->buff_in) < sizeof(tx_peer_t))
        break; 
      err = process_peer_tx((tx_peer_t *)shbuf_data(cli->buff_in));
      shbuf_trim(cli->buff_in, sizeof(tx_peer_t));
      break;
#if 0
    case TX_FILE:
      if (shbuf_size(cli->buff_in) < sizeof(tx_file_t))
        break; 
      err = process_file_tx((tx_file_t *)shbuf_data(cli->buff_in));
      shbuf_trim(cli->buff_in, sizeof(tx_file_t));
      break;
#endif
    case TX_WARD:
      if (shbuf_size(cli->buff_in) < sizeof(tx_ward_t))
        break; 
      err = process_ward_tx(cli->app, (tx_ward_t *)shbuf_data(cli->buff_in));
      shbuf_trim(cli->buff_in, sizeof(tx_ward_t));
      break;
    case TX_SIGNATURE:
      if (shbuf_size(cli->buff_in) < sizeof(tx_sig_t))
        break; 
      err = process_signature_tx(cli->app, (tx_sig_t *)shbuf_data(cli->buff_in));
      shbuf_trim(cli->buff_in, sizeof(tx_sig_t));
      break;
    case TX_LEDGER:
      if (shbuf_size(cli->buff_in) < sizeof(tx_ledger_t))
        break; 
      ledger = (tx_ledger_t *)shbuf_data(cli->buff_in);
      len = sizeof(tx_ledger_t) + sizeof(tx_t) * ledger->ledger_height;
      if (shbuf_size(cli->buff_in) < len)
        break;
      err = process_ledger_tx(ledger);
      shbuf_trim(cli->buff_in, len);
      break;
    case TX_APP:
      if (shbuf_size(cli->buff_in) < sizeof(tx_app_t))
        break;
      err = process_app_tx((tx_app_t *)shbuf_data(cli->buff_in));
      shbuf_trim(cli->buff_in, sizeof(tx_app_t));
      break;
    case TX_ACCOUNT:
      if (shbuf_size(cli->buff_in) < sizeof(tx_account_t))
        break;
      err = process_account_tx((tx_account_t *)shbuf_data(cli->buff_in));
      shbuf_trim(cli->buff_in, sizeof(tx_account_t));
      break;
#if 0
    case TX_TASK:
      if (shbuf_size(cli->buff_in) < sizeof(tx_task_t))
        break;
      err = process_task_tx((tx_task_t *)shbuf_data(cli->buff_in));
      shbuf_trim(cli->buff_in, sizeof(tx_task_t));
      break;
    case TX_THREAD:
      if (shbuf_size(cli->buff_in) < sizeof(tx_thread_t))
        break;
      err = process_thread_tx((tx_thread_t *)shbuf_data(cli->buff_in));
      shbuf_trim(cli->buff_in, sizeof(tx_thread_t));
      break;
#endif
    case TX_TRUST:
      if (shbuf_size(cli->buff_in) < sizeof(tx_trust_t))
        break;
      err = process_trust_tx(cli->app, (tx_trust_t *)shbuf_data(cli->buff_in));
      shbuf_trim(cli->buff_in, sizeof(tx_trust_t));
      break;
    case TX_EVENT:
      if (shbuf_size(cli->buff_in) < sizeof(tx_event_t))
        break;
      err = process_event_tx((tx_event_t *)shbuf_data(cli->buff_in));
      shbuf_trim(cli->buff_in, sizeof(tx_event_t));
      break;
  }

  if (err) {
    sprintf(ebuf, "proc_msg: TX %d: %s [sherr %d].",
        tx->tx_op, str_sherr(err), err);
    sherr(err, ebuf); 
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

    for (cli = sharedaemon_client_list; cli; cli = cli->next) {
      if (!(cli->flags & SHD_CLIENT_NET))
        continue;

      cycle_client_request(cli);
    }

  }

  cycle_term();

}


