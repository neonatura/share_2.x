
/*
 *  Copyright 2016 Neo Natura 
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

#include "sharedaemon.h"
#include "txtest.h"

#define TXTEST_USERNAME "txtest"

void sched_tx_payload(shkey_t *dest_key, void *data, size_t data_len, void *payload, size_t payload_len)
{
/* .. insert_tail */
}

void sched_tx(void *data, size_t data_len)
{
  sched_tx_payload(NULL, data, data_len, NULL, 0);
}

void sched_tx_sink(shkey_t *dest_key, void *data, size_t data_len)
{
  sched_tx_payload(dest_key, data, data_len, NULL, 0);
}


shd_t *sharedaemon_client_list;
shd_t *sharedaemon_client_find(shkey_t *key)
{
return (NULL);
}
int peer_add(shpeer_t *peer)
{
}

#define MAX_TX_TABLE_SIZE 10240
int tx_table_idx;
tx_t *tx_table[MAX_TX_TABLE_SIZE];

void tx_table_add(tx_t *tx)
{

  if (!tx)
    return;

  tx_table[tx_table_idx++] = tx;
fprintf(stderr, "DEBUG: tx_table_add: tx_op %d\n", tx->tx_op);
}

shseed_t *get_test_account_seed(void)
{
  static shseed_t ret_seed;
  shfs_t *fs;
  SHFL *shadow_file;
  uint64_t uid;
  int err;

  memset(&ret_seed, 0, sizeof(ret_seed));
  uid = shpam_uid(TXTEST_USERNAME);

  fs = NULL;
  shadow_file = shpam_shadow_file(&fs);

  err = shpam_pshadow_load(shadow_file, uid, &ret_seed);
  if (err == SHERR_NOENT) {
    /* create new one */
    uint64_t salt = shpam_salt();
    shseed_t *seed = shpam_pass_gen(TXTEST_USERNAME, TXTEST_USERNAME, salt); 
    err = shpam_pshadow_store(shadow_file, seed); 
    if (!err) {
      memset(&ret_seed, 0, sizeof(ret_seed));
      err = shpam_pshadow_load(shadow_file, uid, &ret_seed);
}
  }
  shfs_free(&fs);
  if (err)
    return (NULL);

  return (&ret_seed);
}

void get_test_account_ident(uint64_t *uid_p, shpeer_t **peer_p)
{
  static shpeer_t ret_peer;
  shfs_t *fs;
  SHFL *shadow_file;

  fs = NULL;
  shadow_file = shpam_shadow_file(&fs);
  memcpy(&ret_peer, shfs_inode_peer(shadow_file), sizeof(shpeer_t));
  shfs_free(&fs);

  *uid_p = shpam_uid(TXTEST_USERNAME);
  *peer_p = &ret_peer;
}

int txtest_gen_tx(int op_type)
{
  shseed_t *seed;
  shpeer_t *peer;
  shkey_t *key;
  tx_t *tx;
  uint64_t uid;
  int ret_err;

  tx = NULL;
  ret_err = 0;
  switch (op_type) {
    case TX_SUBSCRIBE:
      /* subscribe to our own shared APP reference. */
      key = shpeer_kpriv(sharedaemon_peer());
      tx = (tx_t *)alloc_subscribe(key, TX_APP, SHOP_LISTEN);
      if (!tx)
        ret_err = SHERR_INVAL;
      break;
    case TX_IDENT:
      get_test_account_ident(&uid, &peer);
      tx = (tx_t *)alloc_ident(uid, peer); 
      if (!tx)
        ret_err = SHERR_INVAL;
      break;
    case TX_ACCOUNT:
      seed = get_test_account_seed();
      tx = (tx_t *)alloc_account(seed);
      if (!tx)
        ret_err = SHERR_INVAL;
      break;
  }

  tx_table_add(tx);

  return (ret_err);
}

int txtest_verify_tx(tx_t *tx)
{
  tx_subscribe_t *sub;
  tx_account_t *acc;
  tx_id_t *id;
  shpeer_t *peer;
  shseed_t *seed;
  shkey_t *key;
  uint64_t uid;

  if (!tx)
    return (SHERR_INVAL);

fprintf(stderr, "DEBUG: txtest_verify_tx: tx_op %d\n", tx->tx_op);
  switch (tx->tx_op) {
    case TX_SUBSCRIBE:
      sub = (tx_subscribe_t *)tx;
      key = shpeer_kpriv(sharedaemon_peer());
      if (sub->sub_op != TX_APP)
        return (SHERR_INVAL);
      if (!shkey_cmp(&sub->sub_key, key))
        return (SHERR_INVAL); 
      break;
    case TX_IDENT:
      id = (tx_id_t *)tx;
      get_test_account_ident(&uid, &peer);
      if (uid != id->id_uid)
        return (SHERR_INVAL);
      if (0 != memcmp(&id->id_peer, peer, sizeof(shpeer_t)))
        return (SHERR_INVAL);
      break;
    case TX_ACCOUNT:
      acc = (tx_account_t *)tx;
      seed = get_test_account_seed();
      if (0 != memcmp(seed, &acc->pam_seed, sizeof(shseed_t)))
        return (SHERR_INVAL);
      break;
  }

  return (0);
}

int sharedaemon_client_listen(shpeer_t *cli_peer, tx_subscribe_t *sub)
{
  return (0);
}


_TEST(txtest)
{
  tx_t *tx;
  int op_type;
  int idx;
  int err;

  for (op_type = 1; op_type < MAX_TX; op_type++) {
    err = txtest_gen_tx(op_type);
    _TRUE(0 == err);
  }

  for (idx = 0; idx < tx_table_idx; idx++) {
    tx = tx_table[idx];
    if (!tx) continue;

    err = tx_confirm(sharedaemon_peer(), tx);
    _TRUE(err == 0);

    err = tx_send(sharedaemon_peer(), tx);
    _TRUE(err == 0);

    err = tx_recv(sharedaemon_peer(), tx);
    _TRUE(err == 0);

    err = tx_confirm(sharedaemon_peer(), tx);
    _TRUE(err == 0);

    err = txtest_verify_tx(tx);
    _TRUE(err == 0);
  }

}

