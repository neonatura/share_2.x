
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

int listen_tx(int tx_op, shd_t *cli, shkey_t *peer_key)
{

  if (!cli)
    return (0);

  cli->op_flags[tx_op] |= SHOP_LISTEN;

  return (0);
}

void proc_msg(int type, shkey_t *key, unsigned char *data, size_t data_len)
{
  tx_account_msg_t m_acc;
  tx_id_msg_t m_id;
  tx_session_msg_t m_sess;
  tx_account_t *acc;
  tx_id_t *id;
  tx_session_t *sess;
  shfs_hdr_t *fhdr;
  shd_t *cli;
  shpeer_t *peer;
  shkey_t *seed_key;
  char ebuf[512];
  int tx_op;
  int err;

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
        acc = generate_account(&m_acc.pam_seed);
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

      err = local_identity_generate(acc->pam_seed.seed_uid, &cli->peer, &id);
      pstore_free(acc);
      if (err) {
        sprintf(ebuf, "proc_msg[TX_IDENT]: error generating identity (peer '%s').", shpeer_print(&cli->peer)); 
        sherr(err, ebuf);
        break;
      }

      pstore_free(id);
      break;

    case TX_SESSION:
      if (data_len < sizeof(tx_session_msg_t))
        break;
      
      memcpy(&m_sess, data, sizeof(m_sess));

      /* identity must be established to process a session */
      id = (tx_id_t *)pstore_load(TX_IDENT,
          (char *)shkey_hex(&m_sess.sess_id));
      if (!id) {
        break;
}

      err = local_session_generate(id, m_sess.sess_stamp, &sess);
      pstore_free(id);
      if (err) {
        sprintf(ebuf, "proc_msg: generating session (id '%s', stamp '%llu')", shkey_print(&id->id_key), (unsigned long long)m_sess.sess_stamp);
        sherr(err, ebuf);
        break;
      }

      pstore_free(sess);
      break;

    case TX_FILE: /* remote file notification */
      peer = (shpeer_t *)data;
      fhdr = (shfs_hdr_t *)(data + sizeof(shpeer_t));
      fprintf(stderr, "DEBUG: proc_msg[TX_FILE]: key %s, peer %s, file "
          " %s %-4.4x:%-4.4x size(%lu) crc(%lx) mtime(%-20.20s)",
          shkey_print(key), shpeer_print(peer), 
          shfs_type_str(fhdr->type),
          fhdr->pos.jno, fhdr->pos.ino,
          (unsigned long)fhdr->size,
          (unsigned long)fhdr->crc,
          shctime(fhdr->mtime)+4);
      break;

    case TX_WALLET:
      break;

    case TX_BOND:
      break;

    case TX_LISTEN:
      if (data_len < sizeof(uint32_t))
        break;

      tx_op = *((uint32_t *)data); 
      peer = (shpeer_t *)(data + sizeof(uint32_t));
      if (tx_op > 0 && tx_op < MAX_TX)
        cli->op_flags[tx_op] |= SHOP_LISTEN;

      /* track associated application */
      if (*peer->label)
        init_app(peer);
/* DEBUG: TODO: add outgoing connection to port, verify, and then 'init_app' */
      break;

    case TX_METRIC:
      if (data_len < sizeof(tx_metric_msg_t))
        break;

      break;

    default:
      err = SHERR_OPNOTSUPP;
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
        memcpy(&m_app.app_context, &app->app_context, sizeof(shkey_t));
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
        memcpy(&m_lic.lic_peer, &lic->lic_peer, sizeof(shpeer_t));
        memcpy(&m_lic.lic_sig, &lic->lic_sig, sizeof(shsig_t));
        memcpy(&m_lic.lic_name, &lic->lic_name, sizeof(shkey_t));
        memcpy(&m_lic.lic_expire, &lic->lic_expire, sizeof(shtime_t));

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
          shbuf_cat(buff, &ledger->ledger_peer, sizeof(shpeer_t));
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
              local_broadcast_metric(met);
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


int broadcast_filter(shd_t *user, tx_t *tx)
{

#if 0
  if ((user->flags & SHD_CLIENT_REGISTER) &&
      0 == memcmp(&user->app->app_name, &tx->tx_peer, sizeof(shkey_t))) {
    /* supress broadcast to originating user */
    return (SHERR_INVAL);
  }
#endif

  if (user->flags & SHD_CLIENT_MSG) { /* ipc msg */
    if (user->op_flags[tx->tx_op] & SHOP_LISTEN)
      return (0); /* user is listening to op mode */

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
    if (0 != broadcast_filter(user, tx)) {
      continue;
    }

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
  switch (tx->tx_op) {
    case TX_IDENT:
      if (shbuf_size(cli->buff_in) < sizeof(tx_id_t))
        break; 
      err = local_identity_inform(cli->app, (tx_id_t *)shbuf_data(cli->buff_in));
      shbuf_trim(cli->buff_in, sizeof(tx_id_t));
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
#if 0
    case TX_SIGNATURE:
      if (shbuf_size(cli->buff_in) < sizeof(tx_sig_t))
        break; 
      err = process_signature_tx(cli->app, (tx_sig_t *)shbuf_data(cli->buff_in));
      shbuf_trim(cli->buff_in, sizeof(tx_sig_t));
      break;
#endif
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
      err = remote_trust_receive(cli->app, (tx_trust_t *)shbuf_data(cli->buff_in));
      shbuf_trim(cli->buff_in, sizeof(tx_trust_t));
      break;
    case TX_EVENT:
      if (shbuf_size(cli->buff_in) < sizeof(tx_event_t))
        break;
      err = process_event_tx((tx_event_t *)shbuf_data(cli->buff_in));
      shbuf_trim(cli->buff_in, sizeof(tx_event_t));
      break;
    case TX_METRIC:
      if (shbuf_size(cli->buff_in) < sizeof(tx_metric_t))
        break;

      shbuf_trim(cli->buff_in, sizeof(tx_metric_t));
      break;
  }

  if (err) {
    sprintf(ebuf, "proc_msg: TX %d: %s [sherr %d].",
        tx->tx_op, sherrstr(err), err);
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
    /* check usb devices */
    cycle_usb_device();

    /* check message queue */
    cycle_msg_queue();

    /* handle socket & poll 10ms */
    ms = 10;
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


