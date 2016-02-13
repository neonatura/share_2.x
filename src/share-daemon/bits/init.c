
/*
 * @copyright
 *
 *  Copyright 2016 Brian Burrell 
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
 *
 *  @endcopyright
 */
#include "sharedaemon.h"
#include <stddef.h>

#define INIT_TX_KEY(_ini) \
  shkey_bin((char *)(_ini) + offsetof(struct tx_init_t, ini_peer), \
      sizeof(struct tx_init_t) - offsetof(struct tx_init_t, ini_peer))

int prep_init_tx(tx_init_t *ini)
{
  shkey_t *key;

  memcpy(&ini->ini_peer, sharedaemon_peer(), sizeof(ini->ini_peer));
  ini->ini_ver = SHARENET_PROTOCOL_VERSION;
  ini->ini_endian = SHMEM_MAGIC;

  ini->ini_stamp = shtime();

  key = INIT_TX_KEY(ini);
  if (!key)
    return (SHERR_NOMEM);

  memset(ini->ini_hash, '\000', sizeof(ini->ini_hash));
  strncpy(ini->ini_hash, shkey_print(key), sizeof(ini->ini_hash)-1);
  shkey_free(&key);
fprintf(stderr, "DEBUG: prep_init_tx: hash '%s'\n", ini->ini_hash);

  return (0);
}

int confirm_init_tx(tx_init_t *ini)
{
  shkey_t *key;
  char hash[MAX_SHARE_HASH_LENGTH];
  int ok;

fprintf(stderr, "DEBUG: confirm_init_tx: peer '%s'\n", shpeer_print(&ini->ini_peer));
fprintf(stderr, "DEBUG: confirm_init_tx: endian %d\n", (int)ini->ini_endian);
fprintf(stderr, "DEBUG: confirm_init_tx: ver %d\n", (int)ini->ini_ver);
fprintf(stderr, "DEBUG: confirm_init_tx: seq %d\n", (int)ini->ini_seq);
fprintf(stderr, "DEBUG: confirm_init_tx: __reserved__ %d\n", (int)ini->__reserved_1__);
fprintf(stderr, "DEBUG: confirm_init_tx: init_stamp %llu\n", (unsigned long long)ini->ini_stamp);


  key = INIT_TX_KEY(ini);
  if (!key)
    return (SHERR_NOMEM);

  memset(hash, 0, sizeof(hash));
  strncpy(hash, shkey_print(key), sizeof(hash) - 1); 
  ok = (0 == strcmp(hash, ini->ini_hash));
  if (!ok) {
fprintf(stderr, "DEBUG: confirm_init_tx: !HASH: gen'd key: '%s'\n", shkey_print(key));
fprintf(stderr, "DEBUG: confirm_init_tx: !HASH: tx key: '%s'\n", ini->ini_hash);
}
  shkey_free(&key);
  if (!ok) {
    return (SHERR_ACCESS);
}

  return (0);
}



/**
 * subscribe to all recent tx keys from recent server.
 */
void process_init_ledger_notify(shd_t *cli, tx_init_t *ini)
{
  tx_subscribe_t sub;
  ledger_t *l;
  shd_t *n_cli;
  tx_t *tx;
  shpeer_t *peer;
  shkey_t *key;
  void *tx_data;
  shtime_t now;
  shtime_t t;
  int tot;
  int idx;

fprintf(stderr, "DEBUG: process_init_ledger_notify()\n");

  for (n_cli = sharedaemon_client_list; n_cli; n_cli->next) {
    init_subscribe_tx(&sub, &cli->peer, shpeer_kpub(&cli->peer), TX_APP, SHOP_LISTEN);
    sched_tx_sink(shpeer_kpriv(&cli->peer), &sub, sizeof(sub));
  }

  now = shtime();
  if (!cli->app || cli->app->app_stamp == SHTIME_UNDEFINED) {
    if (cli->app)
      cli->app->app_stamp = now;
    t = now;
  } else {
    t = cli->app->app_stamp;
  }

  peer = sharedaemon_peer();
  while (t < now) {
    l = ledger_load(peer, t);
    t = shtime_adj(t, ONE_HOUR);

    if (l) {
      tot = ledger_height(l);
      for (idx = 0; idx < tot; idx++) {
        tx = l->ledger + idx;
        if (tx->tx_op == TX_LEDGER)
          continue;

        tx_data = pstore_load(tx->tx_op, tx->hash);
        if (!tx_data)
          continue; /* nerp */

        key = NULL;
        switch (tx->tx_op) {
          case TX_APP:
            key = get_app_key((tx_app_t *)tx_data);
            break;      
          case TX_FILE:
            key = get_file_key((tx_file_t *)tx_data);
            break;
          case TX_BOND:
            key = get_bond_key((tx_bond_t *)tx_data);
            break;
          case TX_WARD:
            key = get_ward_key((tx_ward_t *)tx_data);
            break;
          case TX_IDENT:
            key = get_ident_key((tx_id_t *)tx_data);
            break;
          case TX_ACCOUNT:
            key = get_account_key((tx_account_t *)tx_data);
            break;
          case TX_TASK:
            key = get_task_key((tx_task_t *)tx_data);
            break;
          case TX_THREAD:
            key = get_thread_key((tx_thread_t *)tx_data);
            break;
          case TX_TRUST:
            key = get_trust_key((tx_trust_t *)tx_data);
            break;
          case TX_EVENT:
            key = get_event_key((tx_event_t *)tx_data);
            break;
          case TX_SESSION:
            key = get_session_key((tx_session_t *)tx_data);
            break;
          case TX_LICENSE:
            key = get_license_key((tx_license_t *)tx_data);
            break;
          case TX_WALLET:
            key = get_wallet_key((tx_wallet_t *)tx_data);
            break;
          case TX_METRIC:
            key = get_metric_key((tx_metric_t *)tx_data);
            break;
          case TX_ASSET:
            key = get_asset_key((tx_asset_t *)tx_data);
            break;
        }
        if (key)
          continue;

        init_subscribe_tx(&sub, &tx->tx_peer, key, tx->tx_op, SHOP_LISTEN);
        sched_tx_sink(shpeer_kpriv(&cli->peer), &sub, sizeof(sub));
      }
      ledger_close(l);
    }
  }

}

/**
 * Informs a client about all actively known registered apps.
 */
void process_init_app_notify(shd_t *cli, tx_init_t *ini)
{
  shkey_t c_key;
  shd_t *n_cli;

fprintf(stderr, "DEBUG: process_init_app_notify()\n");

  memcpy(&c_key, shpeer_kpriv(&cli->peer), sizeof(c_key));

  for (n_cli = sharedaemon_client_list; n_cli; n_cli->next) {
    if (!n_cli->app)
      continue; /* has no app info */
    if (shkey_cmp(&c_key, shpeer_kpriv(&n_cli->peer)))
      continue; /* don't notify app of themselves */
    if (cli->flags & SHD_CLIENT_AUTH)
      continue; /* not registered */

    sched_tx_sink(shpeer_kpriv(&cli->peer), n_cli->app, sizeof(tx_app_t));
  }

}

int process_init_tx(shd_t *cli, tx_init_t *ini)
{
  tx_app_t *ent;
  shtime_t stamp;
  int err;

  err = confirm_init_tx(ini);
fprintf(stderr, "DEBUG: process_init_tx: %d = confirm_init_tx()\n", err);
  if (err)
    return (err);

  ini->ini_seq++;

  err = prep_init_tx(ini);
fprintf(stderr, "DEBUG: process_init_tx: %d = prep_init_tx()\n", err);
  if (err)
    return (err);

fprintf(stderr, "DEBUG: ini_seq %d\n", ini->ini_seq);
  stamp = ini->ini_stamp;
  switch (ini->ini_seq) {
    case 1:
    case 2:
      /* public peer notification */
      break;
    case 3:
    case 4:
      /* private peer notification */
      break;
    case 5:
    case 6:
      /* ledger notification */
      process_init_ledger_notify(cli, ini);
    case 7:
    case 8:
      /* app notification */
      process_init_app_notify(cli, ini);
      break;
    case 9:
    case 10:
fprintf(stderr, "DEBUG: ini->ini_time diff %f\n", shtimef(stamp) - shtimef(ini->ini_stamp));
      /* time sync */
      break;
    case 11:
    case 12:
      /* remove 'authorization required' flag for client */
      cli->flags &= ~SHD_CLIENT_AUTH;
      break;
    default:
      /* term init sequence */
      return (0);
  }

  sched_tx_sink(shpeer_kpriv(&cli->peer), ini, sizeof(tx_init_t));

  return (0);
}
