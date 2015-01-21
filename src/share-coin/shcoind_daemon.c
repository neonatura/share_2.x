
#include "shcoind.h"
#include <signal.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#include <errno.h>

user_t *client_list;

void daemon_close_clients(void)
{
  user_t *user;

  for (user = client_list; user; user = user->next) {
    if (user->fd == -1)
      continue;
    close(user->fd);
    user->fd = -1;
  }

}

user_t *register_client(int fd)
{
  user_t *user;
  int err;

  err = shnet_fcntl(fd, F_SETFL, O_NONBLOCK);
  if (err) {
    shnet_close(fd);
    return (NULL);
  }

  user = stratum_user_init(fd);
  user->next = client_list;
  client_list = user;

  return (user);
}

int register_client_task(user_t *user, char *json_text)
{
  shjson_t *tree;
  int err;

  if (!*json_text) {
fprintf(stderr, "DEBUG: empty JSON message.\n");
    return (0);
}

  tree = shjson_init(json_text);
  if (tree == NULL) {
fprintf(stderr, "DEBUG: unknown JSON:\n%s\n", json_text);
    return (SHERR_INVAL);
  }

//fprintf(stderr, "DEBUG: stratum_request_message: %s\n", json_text);
  err = stratum_request_message(user, tree);
  shjson_free(&tree);

  return (err);
}

void shcoind_poll_msg_queue(void)
{
  tx_app_msg_t *app;
  tx_id_msg_t *dest_id;
  tx_id_msg_t *id;
  tx_session_msg_t *sess;
  tx_bond_msg_t *bond;
  struct in_addr in_addr;
  char host_buf[MAXHOSTNAMELEN+1];
  double amount;
  int tx_op;
  int err;

  shbuf_clear(server_msg_buff);
  err = shmsg_read(server_msgq, NULL, server_msg_buff);
  if (err)
    return;

  if (shbuf_size(server_msg_buff) < sizeof(uint32_t)) return;
  tx_op = *(uint32_t *)shbuf_data(server_msg_buff);

  switch (tx_op) {
    case TX_APP:
      shbuf_trim(server_msg_buff, sizeof(uint32_t));
      if (shbuf_size(server_msg_buff) < sizeof(tx_app_msg_t)) return;
      app = (tx_app_msg_t *)shbuf_data(server_msg_buff);
      if (0 != strcasecmp(app->app_peer.label, "shcoind"))
        break;
    fprintf(stderr, "DEBUG: found 'shcoind' peer: %s\n", shpeer_print(&app->app_peer));
      if (app->app_peer.type == SHNET_PEER_IPV4) {
        memcpy(&in_addr, &app->app_peer.addr.sin_addr, sizeof(struct in_addr));
        strcpy(host_buf, inet_ntoa(in_addr));
        start_node_peer(host_buf, ntohs(app->app_peer.addr.sin_port));
      }
      break;

#if 0
    case TX_IDENT:
      shbuf_trim(server_msg_buff, sizeof(uint32_t));
      if (shbuf_size(server_msg_buff) < sizeof(tx_id_msg_t)) return;
      id = (tx_id_msg_t *)shbuf_data(server_msg_buff);
    fprintf(stderr, "DEBUG: found id: %s\n", shpeer_print(&id->id_peer));
      if (0 != strcasecmp(id->id_peer.label, "shcoind"))
        break;

      if (id->id_peer.type == SHNET_PEER_IPV4) {
        memcpy(&in_addr, &id->id_peer.addr.sin_addr, sizeof(struct in_addr));
        strcpy(host_buf, inet_ntoa(in_addr));
        start_node_peer(host_buf, ntohs(id->id_peer.addr.sin_port));
      }

      /* store identity key mapping to account name */
      break;
#endif

#if 0
    case TX_SESSION:
      shbuf_trim(server_msg_buff, sizeof(uint32_t));
      if (shbuf_size(server_msg_buff) < sizeof(tx_id_msg_t)) return;
      sess = (tx_session_msg_t *)shbuf_data(server_msg_buff);

      id = (tx_ident_msg_t *)pstore_load(TX_IDENT, &sess->sess_id);
      if (!id)
        break;

      /* store session expiration & key for account. */
/* .. */

      if (*id->id_label) {
        /* send server wallet info */
        send_wallet_tx(&sess->sess_id,
            getaddressbyaccount(id->id_label),
            getaccountbalance(id->id_label));
      }
      break;
      
    case TX_BOND:
      shbuf_trim(server_msg_buff, sizeof(uint32_t));
      if (shbuf_size(server_msg_buff) < sizeof(tx_bond_msg_t)) return;
      bond = (tx_bond_msg_t *)shbuf_data(server_msg_buff);

      switch (bond->bond_state) {
        case TX_BOND_TRANSMIT:
//p_bond = ... if (!= PENDING) break
          /* currency xfer request */ 
          sess = (tx_session_msg_t *)pstore_load(TX_SESSION, &bond->bond_sess); 
          if (!sess || sess->sess_expire < shtime64()) {
            send_bond_tx(bond, TX_BONDERR_SESS);
            break;
          }

          id = (tx_ident_msg_t *)pstore_load(TX_IDENT, &sess->sess_id);
          dest_id = (tx_ident_msg_t *)pstore_load(TX_IDENT, &bond->bond_id);
          if (!id || !dest_id)
            break;

          amount = (double)bond->bond_credit / (double)COIN;
          err = wallet_account_transfer(id->id_label, dest_id->id_label, bond->bond_label, amount);
          if (!err) {
            send_bond_tx(bond, TX_BOND_CONFIRM);
            /* send updated server wallet info */
            send_wallet_tx(&sess->sess_id,
                getaddressbyaccount(id->id_label),
                getaccountbalance(id->id_label));
          } else {
            if (err == -5) {
              send_bond_tx(bond, TX_BONDERR_ADDR);
            } else if (err == -3 || err == -6) {
              send_bond_tx(bond, TX_BONDERR_DEBIT);
            } else if (err == -13) {
              send_bond_tx(bond, TX_BOND_CONFIRM); /* retry */
            } else {
              send_bond_tx(bond, TX_BONDERR_NET);
            }
          }
          break;
      }
      break;
#endif

    default:
      fprintf(stderr, "DEBUG: server sent msg type %d\n", tx_op);
      break;
  }

}

void daemon_server(void)
{
  user_t *peer;
  user_t *peer_last;
  user_t *peer_next;
  fd_set read_set;
  fd_set write_set;
shbuf_t *buff;
  char *data;
  size_t len;
  double work_t;
  double cur_t;
  int fd_max;
  int cli_fd;
  int fd;
  int err;


  work_t = shtime();
  while (server_fd != -1) {
    double start_t, diff_t;
    struct timeval to;

    start_t = shtime();

    peer_last = NULL;
    for (peer = client_list; peer; peer = peer_next) {
      peer_next = peer->next;

      if (peer->fd != -1 || (peer->flags & USER_SYSTEM)) {
        peer_last = peer;
        continue;
      }

      if (!peer_last) {
        client_list = peer_next;
      } else {
        peer_last->next = peer_next;
      }
      free(peer);
    }

    cli_fd = shnet_accept_nb(server_fd);
    if (cli_fd < 0 && cli_fd != SHERR_AGAIN) {
      perror("shnet_accept");
    } else if (cli_fd > 0) {
      struct sockaddr_in *addr = shnet_host(cli_fd);
      printf ("Received new connection on port %d (%s).\n", STRATUM_DAEMON_PORT, inet_ntoa(addr->sin_addr));
      register_client(cli_fd);
    }

    for (peer = client_list; peer; peer = peer->next) {
      if (peer->fd == -1)
        continue;

      buff = shnet_read_buf(peer->fd);
      if (!buff) {
        perror("shnet_write");
        shnet_close(peer->fd);
        peer->fd = -1;
        continue;
      }


      len = shbuf_idx(buff, '\n');
      if (len == -1)
        continue;
      data = shbuf_data(buff);
      data[len] = '\0';
      register_client_task(peer, data);
      shbuf_trim(buff, len + 1);
    }

    for (peer = client_list; peer; peer = peer->next) {
      if (peer->fd == -1)
        continue;

      /* flush writes */
      len = shnet_write_flush(peer->fd);
      if (len == -1) {
        perror("shnet_write");
        shnet_close(peer->fd);
        peer->fd = -1;
        continue;
      }
    }

    /* once per x1 seconds */
    cur_t = shtime();
    if (cur_t - 1.0 > work_t) {
      stratum_task_gen();
      work_t = cur_t;
    }

    diff_t = (shtime() - start_t);
    diff_t = MAX(0, 20 - (diff_t * 1000));
    memset(&to, 0, sizeof(to));
    to.tv_usec = (1000 * diff_t);
    if (to.tv_usec > 1000) {
      select(1, NULL, NULL, NULL, &to);
    }

    shcoind_poll_msg_queue();
  }

  fprintf (stderr, "Shutting down daemon.\n");

  if (server_fd != -1) {
    shnet_close(server_fd);
    server_fd = -1;
  }


  /* terminate cgb server */
  server_shutdown();

  /* close block fs */
  block_close();

}


