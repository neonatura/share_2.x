
/*
 *  Copyright 2013 Neo Natura
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
int http_listen_sk;

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

  libusb_init(NULL);

#ifdef USE_MAGTEK_DEVICE
  /* https://www.magtek.com/shop/mini.aspx */
  sharedaemon_device_add(USB_MAGTEK_DEVICE);
#endif

#ifdef USE_ZTEX_DEVICE
  /* http://www.ztex.de/usb-fpga-1/usb-fpga-1.15y.e.html */
  sharedaemon_device_add(USB_ZTEX_DEVICE);
#endif

#ifdef USE_LEITCH_DEVICE
  /* http://videoengineer.net/documents/manuals/Leitch/ */
  sharedaemon_device_add(SERIAL_LEITCH_DEVICE);
#endif


}


void proc_msg(int type, shkey_t *key, unsigned char *data, size_t data_len)
{
  tx_account_msg_t m_acc;
  tx_id_msg_t m_id;
  tx_session_msg_t m_sess;
  tx_account_t *acc;
  tx_id_t *id;
  tx_session_t sess;
  tx_file_t *file;
  shfs_hdr_t *fhdr;
  shd_t *cli;
  shpeer_t *peer;
  shkey_t *seed_key;
  char ebuf[512];
  int tx_op;
  int err;

fprintf(stderr, "proc_msg: type(%d) key(%s) <%d bytes>\n", type, shkey_print(key), data_len);

  if (type == TX_APP) { /* app registration */
    if (data_len < sizeof(shpeer_t))
      return;

    peer = (shpeer_t *)data;
    if (0 != memcmp(key, shpeer_kpub(peer), sizeof(shkey_t))) {
      err = SHERR_ACCESS;
    } else {
      err = sharedaemon_msgclient_init(peer);
    }
    goto done;
  }

  cli = sharedaemon_client_find(key);
  if (!cli) {
fprintf(stderr, "DEBUG: proc_msg: sharedaemon_client_find ret'd null.\n");
    err = SHERR_ACCESS;
    goto done;
  }

  err = 0;
  switch (type) {
    case TX_ACCOUNT:
      if (data_len < sizeof(m_acc))
        break;

      memcpy(&m_acc, data, sizeof(m_acc));
      if (m_acc.pam_flag & SHPAM_CREATE) {
        acc = alloc_account(&m_acc.pam_seed);
        if (!acc) {
          sprintf(ebuf, "proc_msg[TX_ACCOUNT]: invalid account uid %llu.", m_acc.pam_seed.seed_uid);
          shwarn(ebuf);
        } else {
          free(acc);
        }
      }
      break;

    case TX_IDENT:
      if (data_len < sizeof(m_id))
        break;

      memcpy(&m_id, data, sizeof(m_id));

      /* In order to establish an identity an account seed must be known. */
      acc = (tx_account_t *)pstore_load(TX_ACCOUNT, shcrcstr(m_id.id_uid));
      if (!acc) {
        sprintf(ebuf, "proc_msg[TX_IDENT]: invalid user id %llu.\n", m_id.id_uid);
        shwarn(ebuf);
        break;
      }

      id = alloc_ident(acc->pam_seed.seed_uid, &cli->peer);
      pstore_free(acc);
      if (!id) {
        sprintf(ebuf, "proc_msg[TX_IDENT]: error generating identity (peer '%s').", shpeer_print(&cli->peer)); 
        sherr(err, ebuf);
        break;
      }
/* tx_send() */
      pstore_free(id);
      break;

    case TX_SESSION:
      if (data_len < sizeof(tx_session_msg_t))
        break;

      memset(&sess, 0, sizeof(sess));
      memcpy(&m_sess, data, sizeof(m_sess));
      err = inittx_session(&sess, m_sess.sess_uid,
          &m_sess.sess_id, m_sess.sess_stamp);
      if (err) {
        sprintf(ebuf, "proc_msg: generating session (id '%s', stamp '%llu')", shkey_print(&sess.sess_id), (unsigned long long)sess.sess_stamp);
        sherr(err, ebuf);
        break;
      }
      break;

    case TX_FILE: /* remote file notification */
      peer = (shpeer_t *)data;
      file = alloc_file_path(peer, (char *)(data + sizeof(shpeer_t))); 
      if (!file)
        break;

fprintf(stderr, "DEBUG: PROC_MSG[TX_FILE]: key %s, peer %s, file "
    " %s %-4.4x:%-4.4x size(%lu) crc(%lx)",
    shkey_print(&file->ino_name), shpeer_print(&file->ino_peer), 
    (unsigned long)file->ino_size,
    (unsigned long)file->ino_crc);

      tx_send(NULL, file);
      pstore_free(file);
      break;

    case TX_WALLET:
      break;

    case TX_BOND:
      break;

    case TX_SUBSCRIBE:
      if (data_len < sizeof(uint32_t))
        break;

      tx_op = *((uint32_t *)data); 
      peer = (shpeer_t *)(data + sizeof(uint32_t));
      if (tx_op > 0 && tx_op < MAX_TX)
        cli->op_flags[tx_op] |= SHOP_LISTEN;

#if 0
      /* track associated application */
      if (*peer->label) {
        err = inittx_app(&cli->app, peer);
        if (err)
          break;

        /* broadcast app tx */
        err = tx_send(NULL, &cli->app);
      }
#endif
      break;

    case TX_METRIC:
      if (data_len < sizeof(tx_metric_msg_t))
        break;

      break;

    default:
      /* mark as processed even if not handled */
      err = 0;
      break;
  }

done:
  if (err) {
    char ebuf[256];
    sprintf(ebuf, "proc_msg: err(%s [%d]) type(%d) key(%s) data-len(%d)\n", sherrstr(err), err, type, shkey_print(key), data_len);
    sherr(err, ebuf);
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
if (err != SHERR_NOMSG && err != SHERR_AGAIN) fprintf(stderr, "DEBUG: cycle_msg_queue_in: err %d\n", err);
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
  proc_msg(type, &msg_key, (unsigned char *)data + sizeof(uint32_t), data_len);
  shbuf_clear(_message_queue_buff);

}

static void cycle_msg_queue_out(void)
{
  tx_ledger_t *ledger;
  tx_account_t *acc;
  tx_id_t *id;
  tx_app_t *app;
  tx_license_t *lic;
  tx_event_t *event;
  tx_bond_t *bond;
  tx_session_t *session;
  tx_metric_t *met;
  tx_app_msg_t m_app;
  tx_account_msg_t m_acc;
  tx_id_msg_t m_id;
  tx_license_msg_t m_lic;
  tx_event_msg_t m_event;
  tx_bond_msg_t m_bond;
  tx_session_msg_t m_session;
  tx_metric_msg_t m_met;
  shbuf_t *buff;
  shd_t *cli;
  tx_t *tx;
  uint32_t mode;
  int err;

  for (cli = sharedaemon_client_list; cli; cli = cli->next) {
    if (!(cli->flags & SHD_CLIENT_MSG))
      continue;

    if (shbuf_size(cli->buff_out) < sizeof(tx_t)) {
      /* a full message should exist */
      shbuf_clear(cli->buff_out);
      continue;
    }

    tx = (tx_t *)shbuf_data(cli->buff_out);
    switch (tx->tx_op) {
      case TX_APP:
        if (shbuf_size(cli->buff_out) < sizeof(tx_app_t)) {
          shbuf_clear(cli->buff_out);
          break;
        }

        app = (tx_app_t *)shbuf_data(cli->buff_out);
        memset(&m_app, 0, sizeof(m_app));
        memcpy(&m_app.app_peer, &app->app_peer, sizeof(shpeer_t));
//        memcpy(&m_app.app_context, &app->app_context, sizeof(shkey_t));
        m_app.app_stamp = app->app_stamp;
        m_app.app_trust = app->app_trust;

        mode = TX_APP;
        buff = shbuf_init();
        shbuf_cat(buff, &mode, sizeof(mode));
        shbuf_cat(buff, &m_app, sizeof(m_app));
        err = shmsg_write(_message_queue, buff, &cli->cli.msg.msg_key);
        shbuf_free(&buff);

        shbuf_trim(cli->buff_out, sizeof(tx_app_t));
        break; 

      case TX_ACCOUNT:
        if (shbuf_size(cli->buff_out) < sizeof(tx_account_t)) {
          shbuf_clear(cli->buff_out);
          break;
        }

        acc = (tx_account_t *)shbuf_data(cli->buff_out);
        memset(&m_acc, 0, sizeof(m_acc));
        memcpy(&m_acc.pam_seed, &acc->pam_seed, sizeof(shseed_t));

        mode = TX_ACCOUNT;
        buff = shbuf_init();
        shbuf_cat(buff, &mode, sizeof(mode));
        shbuf_cat(buff, &m_acc, sizeof(m_acc));
        err = shmsg_write(_message_queue, buff, &cli->cli.msg.msg_key);
        shbuf_free(&buff);

        shbuf_trim(cli->buff_out, sizeof(tx_account_t));
        break;

      case TX_IDENT:
        if (shbuf_size(cli->buff_out) < sizeof(tx_id_t)) {
          shbuf_clear(cli->buff_out);
          break;
        }

        id = (tx_id_t *)shbuf_data(cli->buff_out);
        memset(&m_id, 0, sizeof(m_id));
        memcpy(&m_id.id_peer, &id->id_peer, sizeof(shpeer_t));
        memcpy(&m_id.id_key, &id->id_key, sizeof(shkey_t));
        m_id.id_stamp = id->id_stamp;
        m_id.id_uid = id->id_uid;

        mode = TX_IDENT;
        buff = shbuf_init();
        shbuf_cat(buff, &mode, sizeof(mode));
        shbuf_cat(buff, &m_id, sizeof(m_id));
        err = shmsg_write(_message_queue, buff, &cli->cli.msg.msg_key);
        shbuf_free(&buff);

        shbuf_trim(cli->buff_out, sizeof(tx_id_t));
        break;

      case TX_SESSION:
        if (shbuf_size(cli->buff_out) < sizeof(tx_session_t)) {
          shbuf_clear(cli->buff_out);
          break;
        }

        session = (tx_session_t *)shbuf_data(cli->buff_out);
        memset(&m_session, 0, sizeof(m_session));
        memcpy(&m_session.sess_id, &session->sess_id, sizeof(shkey_t));
        memcpy(&m_session.sess_stamp, &session->sess_stamp, sizeof(shtime_t));

        mode = TX_SESSION;
        buff = shbuf_init();
        shbuf_cat(buff, &mode, sizeof(mode));
        shbuf_cat(buff, &m_session, sizeof(m_session));
        err = shmsg_write(_message_queue, buff, &cli->cli.msg.msg_key);
        shbuf_free(&buff);

        shbuf_trim(cli->buff_out, sizeof(tx_session_t));
        break;

      case TX_LICENSE:
        if (shbuf_size(cli->buff_out) < sizeof(tx_license_t)) {
          shbuf_clear(cli->buff_out);
          break;
        }

        lic = (tx_license_t *)shbuf_data(cli->buff_out);
        memset(&m_lic, 0, sizeof(m_lic));
        memcpy(&m_lic.lic_peer, &lic->lic_cert.cert_sub.ent_peer, sizeof(shpeer_t));
        memcpy(&m_lic.lic_sig, &lic->lic_cert.cert_sub.ent_sig, sizeof(shsig_t));
        memcpy(&m_lic.lic_name, &lic->lic_cert.cert_sub.ent_name, sizeof(shkey_t));
        m_lic.lic_expire = lic->lic_cert.cert_sub.ent_sig.sig_expire;

        mode = TX_LICENSE;
        buff = shbuf_init();
        shbuf_cat(buff, &mode, sizeof(mode));
        shbuf_cat(buff, &m_lic, sizeof(m_lic));
        err = shmsg_write(_message_queue, buff, &cli->cli.msg.msg_key);
        shbuf_free(&buff);

        shbuf_trim(cli->buff_out, sizeof(tx_license_t));
        break;

      case TX_EVENT:
        if (shbuf_size(cli->buff_out) < sizeof(tx_event_t)) {
          shbuf_clear(cli->buff_out);
          break;
        }

        event = (tx_event_t *)shbuf_data(cli->buff_out);
        memset(&m_event, 0, sizeof(m_event));
        memcpy(&m_event.event_peer, &event->event_peer, sizeof(shpeer_t));
        memcpy(&m_event.event_sig, &event->event_sig, sizeof(shsig_t));
        memcpy(&m_event.event_stamp, &event->event_stamp, sizeof(shtime_t));

        mode = TX_EVENT;
        buff = shbuf_init();
        shbuf_cat(buff, &mode, sizeof(mode));
        shbuf_cat(buff, &m_event, sizeof(m_event));
        err = shmsg_write(_message_queue, buff, &cli->cli.msg.msg_key);
        shbuf_free(&buff);

        shbuf_trim(cli->buff_out, sizeof(tx_event_t));
        break;

      case TX_WALLET:
        break;

      case TX_BOND:
/* ~ shcoined uses sig [key held only by daemon] to verify xfer */
        if (shbuf_size(cli->buff_out) < sizeof(tx_bond_t)) {
          shbuf_clear(cli->buff_out);
          break;
        }

        bond = (tx_bond_t *)shbuf_data(cli->buff_out);
        memset(&m_bond, 0, sizeof(m_bond));
        strncpy(m_bond.bond_sink, bond->bond_sink, MAX_SHARE_HASH_LENGTH);
        strncpy(m_bond.bond_label, bond->bond_label, MAX_SHARE_HASH_LENGTH);
        memcpy(&m_bond.bond_sig, &bond->bond_sig, sizeof(shsig_t));
        m_bond.bond_credit = bond->bond_credit;

        mode = TX_BOND;
        buff = shbuf_init();
        shbuf_cat(buff, &mode, sizeof(mode));
        shbuf_cat(buff, &m_bond, sizeof(m_bond));
        err = shmsg_write(_message_queue, buff, &cli->cli.msg.msg_key);
        shbuf_free(&buff);

        shbuf_trim(cli->buff_out, sizeof(tx_bond_t));
        break;

      case TX_LEDGER:
        if (shbuf_size(cli->buff_out) < sizeof(tx_ledger_t)) {
          shbuf_clear(cli->buff_out);
          break;
        }

        ledger = (tx_ledger_t *)shbuf_data(cli->buff_out);
        if (shbuf_size(cli->buff_out) < sizeof(tx_ledger_t) + ledger->ledger_height * sizeof(tx_t)) {
          shbuf_clear(cli->buff_out);
          break;
        }

        /* ensure ledger is closed */
/* DEBUG:        if (ledger->ledger_stamp != 0) { */
          mode = TX_LEDGER;
          buff = shbuf_init();
          shbuf_cat(buff, &ledger->ledger_txkey, sizeof(shkey_t));
          shbuf_cat(buff, (char *)ledger + sizeof(tx_ledger_t), ledger->ledger_height * sizeof(tx_t));
          err = shmsg_write(_message_queue, buff, &cli->cli.msg.msg_key);
          shbuf_free(&buff);
/* DEBUG:        } */

        shbuf_trim(cli->buff_out, sizeof(tx_ledger_t));
        shbuf_trim(cli->buff_out, ledger->ledger_height * sizeof(tx_t)); 
        break; 

      case TX_METRIC:
        if (shbuf_size(cli->buff_out) < sizeof(tx_metric_t)) {
          shbuf_clear(cli->buff_out);
          break;
        }

        met = (tx_metric_t *)shbuf_data(cli->buff_out);

        memset(&m_met, 0, sizeof(tx_metric_msg_t));
        m_met.met_type = met->met_type;
        m_met.met_flags = met->met_flags;
        strncpy(m_met.met_name, met->met_name, sizeof(m_met.met_name)-1);
        m_met.met_expire = met->met_expire;
        m_met.met_acc = met->met_acc;

        mode = TX_METRIC; 
        buff = shbuf_init();
        shbuf_cat(buff, &mode, sizeof(mode));
        shbuf_cat(buff, &m_met, sizeof(m_met));
        err = shmsg_write(_message_queue, buff, &cli->cli.msg.msg_key);
        free(buff);
 
        shbuf_trim(cli->buff_out, sizeof(tx_metric_t));
        break;

      default:
//        shwarn("cycle_msg_queue_out: uknown tx op %d\n", tx->tx_op);
        shbuf_clear(cli->buff_out);
        break;
    }
  }

}

#define SHAREDAEMON_DEVICE_POLL_TIME 5000
void cycle_usb_device(void)
{
  shdev_t *p_dev, *n_dev;
  shdev_t *dev;
  shdev_t *dev_next;
  tx_metric_t *met;
  int err;

  p_dev = NULL;
  for (dev = sharedaemon_device_list; dev; dev = n_dev) {
    n_dev = dev->next;
    if (dev->err_state == SHERR_SHUTDOWN &&
        !(dev->def->flags & SHDEV_START)) {
      /* remove from list */
      if (p_dev)
        p_dev->next = n_dev;
      else
        sharedaemon_device_list = n_dev;
      sharedaemon_device_free(dev);
      continue;
    }
    p_dev = dev;
  }

  for (dev = sharedaemon_device_list; dev; dev = dev->next) {
    switch (dev->err_state) {
      case SHERR_SHUTDOWN:
        if ((dev->def->flags & SHDEV_START)) {
          /* re-initialize */
        }
        break;
      case 0:
        /* read from device */
        err = sharedaemon_device_poll(dev, SHAREDAEMON_DEVICE_POLL_TIME);
        if (!err) {
          /* pending data to process */
          if (dev->def->flags & SHDEV_CARD) {
            err = local_metric_generate(SHMETRIC_CARD, 
                &dev->data.card, sizeof(shcard_t), &met);
            if (!err) {
              tx_send(NULL, (tx_t *)met);
              pstore_free(met);
            }
          }
          if (dev->def->flags & SHDEV_CLOCK) {
            refclock_receive(&dev->data.clock);
          }
        }
        break;
      default:
        if (dev->err_state && dev->err_state != SHERR_SHUTDOWN) {
          /* terminate device */
          sharedaemon_device_shutdown(dev);
        }
        break;
    }
  }

}

void cycle_msg_queue(void)
{
  cycle_msg_queue_in();
  cycle_msg_queue_out();
}


/**
 * Returns whether to filter a particular request from being broadcasted.
 */
static int broadcast_filter(shd_t *user, tx_t *tx, tx_net_t *net)
{

  if (user->flags & SHD_CLIENT_MSG) { /* ipc msg */
    /* filter is user is listening to op mode */
    if (!(user->op_flags[tx->tx_op] & SHOP_LISTEN))
      return (TRUE);
  } else if (user->flags & SHD_CLIENT_NET) { /* network connection */
    if (0 != memcmp(&net->tx_sink, ashkey_blank(), sizeof(shkey_t))) {
      /* supress broadcast to originating user */
      if (0 != memcmp(shpeer_kpriv(&user->peer), &net->tx_sink, sizeof(shkey_t)))
        return (TRUE);
    }
  }

  return (FALSE);
}
        
void broadcast_raw(void *raw_data, size_t data_len)
{
  unsigned char *data = (unsigned char *)raw_data;
  tx_net_t *net = (tx_net_t *)data;
  tx_t *tx = (tx_t *)(data + sizeof(tx_t));
  shd_t *user;
  int hop;

  if (data_len < (sizeof(tx_net_t) + sizeof(tx_t)))
    return;

  hop = (0 == memcmp(&net->tx_sink, ashkey_blank(), sizeof(shkey_t)));
  if (!hop) {
    for (user = sharedaemon_client_list; user; user = user->next) {
      if (!(user->flags & SHD_CLIENT_NET))
        continue;
      if (0 == memcmp(shpeer_kpriv(&user->peer),
            &net->tx_sink, sizeof(shkey_t))) {
        hop = TRUE;
        break;
      }
    }
  }

  for (user = sharedaemon_client_list; user; user = user->next) {
    if (user->flags & SHD_CLIENT_MSG) { /* ipc msg */
      /* filter is user is listening to op mode */
      if (!(user->op_flags[tx->tx_op] & SHOP_LISTEN))
        continue;
    } else if (user->flags & SHD_CLIENT_NET) { /* network connection */
      if (hop &&
          0 != memcmp(&net->tx_sink, 
            ashkey_blank(), sizeof(shkey_t)) &&
          0 != memcmp(shpeer_kpriv(&user->peer), 
            &net->tx_sink, sizeof(shkey_t)))
        continue;
    }

fprintf(stderr, "DEBUG: broadcast_raw: shbuf_cat(user{%s}->buff_out data_len(%d)\n", shpeer_print(&user->peer), data_len); 
    shbuf_cat(user->buff_out, data, data_len);
  }

}

void cycle_term(void)
{
  shdev_t *dev_next;
  shdev_t *dev;

  for (dev = sharedaemon_device_list; dev; dev = dev->next) {
    sharedaemon_device_shutdown(dev);
  }
  for (dev = sharedaemon_device_list; dev; dev = dev_next) {
    dev_next = dev->next;
    sharedaemon_device_free(dev);
  }
  sharedaemon_device_list = NULL;

  libusb_exit(NULL);

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
      shnet_fcntl(cli_fd, F_SETFL, O_NONBLOCK);
      if (sharedaemon_client_count(shaddr(cli_fd)) > MAX_CLIENT_CONNECTIONS) {
fprintf(stderr, "DEBUG: cycle_socket: warning: closing fd %d due to > %d connections\n", cli_fd, MAX_CLIENT_CONNECTIONS);
        close(cli_fd);
      } else {
        sharedaemon_netclient_init(cli_fd, shaddr(cli_fd));
fprintf(stderr, "DEBUG: cycle_socket: warning: opening fd %d for network connection.\n", cli_fd);
      }
    }
  }
  if (http_listen_sk && FD_ISSET(http_listen_sk, read_fd)) {
    cli_fd = shnet_accept(http_listen_sk);
    if (cli_fd != -1) {
      shnet_fcntl(cli_fd, F_SETFL, O_NONBLOCK);
      if (sharedaemon_client_count(shaddr(cli_fd)) > MAX_CLIENT_CONNECTIONS) {
fprintf(stderr, "DEBUG: cycle_socket: warning: closing fd %d due to > %d http connections\n", cli_fd, MAX_CLIENT_CONNECTIONS);
        close(cli_fd);
      } else {
        sharedaemon_httpclient_add(cli_fd);
fprintf(stderr, "DEBUG: cycle_socket: warning: opening fd %d for http connection.\n", cli_fd);
      }
    }
  }

  /* incoming socket data */
  for (cli = sharedaemon_client_list; cli; cli = cli->next) {
    if (cli->cli.net.fd == 0)
      continue;
    if (!(cli->flags & SHD_CLIENT_NET) &&
        !(cli->flags & SHD_CLIENT_HTTP))
      continue;

    /* copy socket buffer segment to incoming client buffer. */
    rbuf = shnet_read_buf(cli->cli.net.fd);
    if (rbuf && shbuf_size(rbuf)) {
fprintf(stderr, "DEBUG: fd %d has <%d bytes> incoming data.\n", cli->cli.net.fd, shbuf_size(rbuf));
      shbuf_append(rbuf, cli->buff_in);
      shbuf_clear(rbuf);
      cli->buff_stamp = shtime();
    }
  }

  /* outgoing socket data */
  for (cli = sharedaemon_client_list; cli; cli = cli->next) {
    if (cli->cli.net.fd == 0)
      continue;
    if (!(cli->flags & SHD_CLIENT_NET) &&
        !(cli->flags & SHD_CLIENT_HTTP))
      continue;
    if (FD_ISSET(cli->cli.net.fd, write_fd)) {
size_t orig_len = shbuf_size(cli->buff_out);
      len = shnet_write(cli->cli.net.fd, shbuf_data(cli->buff_out), shbuf_size(cli->buff_out)); 
fprintf(stderr, "DEBUG: %d = shnet_write(<%d bytes>)\n", len, orig_len);
      if (len > 0) {
        shbuf_trim(cli->buff_out, len);
        cli->buff_stamp = shtime();
      } else {
size_t orig_len = shbuf_size(cli->buff_out);
fprintf(stderr, "DEBUG: %s = shnet_write(<%d bytes))\n", strerror(errno), orig_len);
        close(cli->cli.net.fd);
        cli->cli.net.fd = 0; /* mark for removal */
      }
    }
//    shnet_write_flush(cli->cli.net.fd);
  } 

}

void cycle_socket_verify(void)
{
  shd_t *cli;
  shbuf_t *rbuf;
  char buf[1024];
  ssize_t len;
  int cli_fd;
  int err;

  for (cli = sharedaemon_client_list; cli; cli = cli->next) {
    if (cli->cli.net.fd == 0)
      continue;

    if (cli->flags & SHD_CLIENT_SHUTDOWN) { /* socket connection verification */
      struct timeval to;
      fd_set w_set, x_set;
      int fd = cli->cli.net.fd;

      FD_ZERO(&w_set);
      FD_ZERO(&x_set);
      memset(&to, 0, sizeof(to));
      err = select(fd+1, NULL, &w_set, &x_set, &to);
      if (err < 0 || FD_ISSET(fd, &w_set) || FD_ISSET(fd, &x_set)) {
        cli->cli.net.fd = 0;
        shnet_close(fd);
      }
      continue;
    }

    if (!(cli->flags & SHD_CLIENT_NET) &&
        !(cli->flags & SHD_CLIENT_HTTP))
      continue;

    /* connection idle time */
    if (cli->buff_stamp == SHTIME_UNDEFINED) {
      /* no data has been processed on socket. */
      if (shtime_diff(shtime(), cli->birth) > MAX_CLIENT_CONNECTION_IDLE_TIME) {
        /* no warning */
        sprintf(buf, "cycle_socket_verify: closing transient network connection [idle %-2.2fs] (buff-stamp %llu): %s", shtime_diff(shtime(), cli->birth), cli->buff_stamp, shpeer_print(&cli->peer)); 
        shwarn(buf);

        close(cli->cli.net.fd);
        cli->cli.net.fd = 0; /* mark for removal */
        continue;
      }
    } else {
      /* connection idle time */
      if (shtime_diff(shtime(), cli->buff_stamp) > MAX_CLIENT_IDLE_TIME) {
        sprintf(buf, "cycle_socket_verify: closing network connection [idle %-2.2fs] (buff-stamp %llu): %s", shtime_diff(shtime(), cli->buff_stamp), cli->buff_stamp, shpeer_print(&cli->peer)); 
        shwarn(buf);

        close(cli->cli.net.fd);
        cli->cli.net.fd = 0; /* mark for removal */
        continue;
      }
    }
  }

}

int cycle_client_request(shd_t *cli)
{
  tx_ledger_t *ledger;
  size_t size;
  tx_t *tx;
  tx_net_t net;
  uint32_t crc;
  unsigned char *raw_data;
  char ebuf[1024];
  int err;
  int tx_op;

  if (shbuf_size(cli->buff_in) < sizeof(tx_net_t))
    return;

  raw_data = (unsigned char *)shbuf_data(cli->buff_in);
  memcpy(&net, raw_data, sizeof(tx_net_t));

  if ((unsigned int)net.tx_magic != (unsigned int)SHMEM32_MAGIC) {
    shbuf_clear(cli->buff_in);
    sherr(SHERR_ILSEQ, "cyclie_client_request [sequence]");
    return (SHERR_ILSEQ);
  } 

  size = ntohl(net.tx_size);
  if (size < sizeof(tx_t)) {
    shbuf_clear(cli->buff_in);
    sherr(SHERR_ILSEQ, "cyclie_client_request [truncated]");
    return (SHERR_ILSEQ);
  }

  if (shbuf_size(cli->buff_in) < sizeof(tx_net_t) + size)
    return (0); /* data is pending sir */

  raw_data += sizeof(tx_net_t);
  tx = (tx_t *)raw_data;

  crc = (uint32_t)shcrc(tx, sizeof(tx_t));
  if (crc != net.tx_crc) {
    shbuf_clear(cli->buff_in);
    sherr(SHERR_ILSEQ, "cyclie_client_request [checksum]");
    return (SHERR_ILSEQ);
  }

  tx_op = (int)tx->tx_op;
  if (tx_op < 0 || tx_op >= MAX_TX) {
    if (tx_op >= MAX_VERSION_TX) {
      err = SHERR_ILSEQ;
    } else {
      /* unsupported operation */
      err = SHERR_OPNOTSUPP;
    }
    sherr(err, "cyclie_client_request [invalid operation]");
    goto done;
  }

  if (tx_op != TX_INIT && (cli->flags & SHD_CLIENT_AUTH)) {
    /* connection not initialized. */
    err = SHERR_AGAIN;
    goto done;
  }

  err = tx_confirm(&cli->peer, tx);
  if (err) {
    sprintf(ebuf, "cycle_client_request: TX %d confirm: %s [sherr %d].",
        tx->tx_op, sherrstr(err), err);
    shwarn(ebuf); 
    goto done;
  }

  err = tx_recv(&cli->peer, tx);
  if (err) {
    sprintf(ebuf, "cycle_client_request: TX %d: %s [sherr %d].",
        tx->tx_op, sherrstr(err), err);
    sherr(err, ebuf); 
    goto done;
  }

done:
  shbuf_trim(cli->buff_in, sizeof(tx_net_t) + size);
  return (err);
}

void client_http_response(shd_t *cli)
{
  char text[1024];
  char buf[1024];
  char *tmpl;
  time_t now;

  tmpl = cli->cli.net.tmpl;
  while (*tmpl == '/')
    tmpl++;
  if (!*tmpl) {
    strcpy(tmpl, "default");
  }

  if (0 == strcmp(tmpl, "default")) {
strcpy(text, "<html><body>hi</body></html>\r\n");

now = time(NULL);

strcpy(buf, "HTTP/1.1 200 OK\r\n");
shbuf_catstr(cli->buff_out, buf); 
strftime(buf, sizeof(buf) - 1, "Date: %a, %d %b %Y %H:%M:%S GMT\r\n", gmtime(&now));
shbuf_catstr(cli->buff_out, buf); 
sprintf(buf, "Server: %s/%s\r\n", get_libshare_title(), get_libshare_version());
shbuf_catstr(cli->buff_out, buf); 
//shbuf_catstr(cli->buff_out, "Accept-Ranges: bytes\r\n");
sprintf(buf, "Content-Length: %u\r\n", (unsigned int)strlen(text));
shbuf_catstr(cli->buff_out, buf); 
//shbuf_catstr(cli->buff_out, "Connection: close\r\n");
shbuf_catstr(cli->buff_out, "Content-Type: text/html; charset=UTF-8\r\n");
shbuf_catstr(cli->buff_out, "\r\n");

    shbuf_catstr(cli->buff_out, text);
  } else if (0 == strcmp(tmpl, "favicon.ico")) {
    oauth_response_favicon(cli->buff_out);
  } else if (0 == strcmp(tmpl, "token")) {
    char *grant_type = shmap_get_str(cli->cli.net.fields, ashkey_str("grant_type"));
    char *code = shmap_get_str(cli->cli.net.fields, ashkey_str("code"));
    char *redirect_uri = shmap_get_str(cli->cli.net.fields, ashkey_str("redirect_uri"));
    char *client_id = shmap_get_str(cli->cli.net.fields, ashkey_str("client_id"));
    char *client_secret = shmap_get_str(cli->cli.net.fields, ashkey_str("client_secret"));

    if (grant_type && 0 == strcmp(grant_type, "authorization_code")) {
#if 0
      /* responds with {"access_token":".."} or {"error":"invalid argument"} */
      oauth_grant_auth_code(cli->buff_out, 
          code, redirect_uri, client_id, client_secret); 
#endif
    } else {
      /* {"error":"invalid argument"} */
    } 
  } else if (0 == strcmp(tmpl, "auth")) {
    char *response_type = shmap_get_str(cli->cli.net.fields, ashkey_str("response_type"));
    char *client_id = shmap_get_str(cli->cli.net.fields, ashkey_str("client_id"));

    if (response_type) {
      if (0 == strcmp(response_type, "token")) {
        char *redirect_uri = shmap_get_str(cli->cli.net.fields, ashkey_str("redirect_uri"));
        char *scope = shmap_get_str(cli->cli.net.fields, ashkey_str("scope"));
        /* requesting a session token */
        /* redirects to <redirect_uri>?token=.. */
        oauth_response_token(cli, cli->buff_out, client_id, redirect_uri, scope);
      } else if (0 == strcmp(response_type, "password")) {
        /* response to user/pass login template */
        char *username = shmap_get_str(cli->cli.net.fields, ashkey_str("username"));
        char *password = shmap_get_str(cli->cli.net.fields, ashkey_str("password"));
        char *enable_2fa = shmap_get_str(cli->cli.net.fields, ashkey_str("enable_2fa"));
        int err = oauth_response_password(cli, client_id, username, password, 
            enable_2fa ? (0 == strcmp(enable_2fa, "on")) : FALSE);
      } else if (0 == strcmp(response_type, "2fa")) {
        /* response to 2fa login template */
        char *token = shmap_get_str(cli->cli.net.fields, ashkey_str("code"));
        char *code = shmap_get_str(cli->cli.net.fields, ashkey_str("2fa"));
        char *enable_2fa = shmap_get_str(cli->cli.net.fields, ashkey_str("enable_2fa"));
        int err = oauth_response_2fa(cli, token, client_id, code,
            enable_2fa ? (0 == strcmp(enable_2fa, "on")) : FALSE);
      } else if (0 == strcmp(response_type, "access")) {
        char *code = shmap_get_str(cli->cli.net.fields, ashkey_str("code"));
        /* response to "Accept" in app access template */
        int err = oauth_response_access(cli, client_id, code);
      } else {
//        oauth_response_redir_account_template(cli->buff_out);
      }
    }

  } else {
    /* 404 Not Found */
    oauth_response_notfound_template(cli->buff_out);
  }
  
}

void client_http_tokens(char *tmpl, shmap_t *fields)
{
  char fld[1024];
  char val[1024];
  char *tok;
  char *str;
  int idx;
  
  tok = strtok(tmpl, "?");
  while (tok) {
    memset(fld, 0, sizeof(fld));
    memset(val, 0, sizeof(val));

    idx = stridx(tok, '=');
    if (idx != -1) {
      strncpy(fld, tok, MIN(sizeof(fld) - 1, idx));
      strncpy(val, tok + idx + 1, sizeof(val) - 1);
      str = http_token_decode(val);
      if (str) {
        shmap_set_astr(fields, ashkey_str(fld), str);
        free(str);
      }
    }

    tok = strtok(NULL, "&");
  }

}

void cycle_client_http_request(shd_t *cli)
{
  char ebuf[1024];
  char *data;
  int err;
  int idx;

  data = shbuf_data(cli->buff_in);
  if (!data)
    return;

  idx = stridx(data, '\n'); 
  if (idx == -1)
    return;

  data[idx] = '\000'; /* \n */
  if (idx && data[idx-1] == '\r')
    data[idx-1] = '\000'; /* \r */
  if (0 == strncmp(data, "GET ", strlen("GET "))) {
    data += 4;
    strtok(data, " ");
    strncpy(cli->cli.net.tmpl, data, sizeof(cli->cli.net.tmpl) - 1);

    if (!cli->cli.net.fields)
      cli->cli.net.fields = shmap_init();
    client_http_tokens(cli->cli.net.tmpl, cli->cli.net.fields);
  } else if (*data) {
    char tok[1024], *val;

    memset(tok, '\000', sizeof(tok));
    strncpy(tok, data, sizeof(tok)-1);
    val = strstr(tok, ": ");
    if (val) {
      *val = '\000';
      val += 2;
      shmap_set_astr(cli->cli.net.fields, ashkey_str(tok), val); 
    }
  } else {
    client_http_response(cli);
  }

  shbuf_trim(cli->buff_in, idx + 1);

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
    /* check usb devices */
    cycle_usb_device();

    /* check message queue */
    cycle_msg_queue();

    /* check udp broadcast */
    sharedaemon_bcast_recv(); 

    /* handle socket & poll 10ms */
    ms = 10;
    FD_ZERO(&read_fd);
    FD_SET(listen_sk, &read_fd);
    if (http_listen_sk)
      FD_SET(http_listen_sk, &read_fd);
    FD_ZERO(&write_fd);

    /* process socket IO */
    for (cli = sharedaemon_client_list; cli; cli = cli->next) {
      if (cli->cli.net.fd == 0)
        continue;
      if (!(cli->flags & SHD_CLIENT_NET) &&
          !(cli->flags & SHD_CLIENT_HTTP))
        continue;

      FD_SET(cli->cli.net.fd, &read_fd);
      if (shbuf_size(cli->buff_out) != 0)
        FD_SET(cli->cli.net.fd, &write_fd);
    }
    err = shnet_verify(&read_fd, &write_fd, &ms);
    if (err >= 1)
      cycle_socket(&read_fd, &write_fd);

    /* verify socket state */ 
    cycle_socket_verify();

    /* process incoming shared requests */
    for (cli = sharedaemon_client_list; cli; cli = cli->next) {
      if (!(cli->flags & SHD_CLIENT_NET))
        continue;

      cycle_client_request(cli);
    }

    /* process incoming http requests */
    for (cli = sharedaemon_client_list; cli; cli = cli->next) {
      if (!(cli->flags & SHD_CLIENT_HTTP))
        continue;

      cycle_client_http_request(cli);
    }

  }

  sharedaemon_term();

}



