
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

#define __BITS_C__

#include "sharedaemon.h"
#include <stddef.h>

#define TXOP(_f) (txop_f)&_f
static txop_t _txop_table[MAX_TX] = {
  { "none" },
  { "init", sizeof(tx_init_t), 0, 
    TXOP(txop_init_init), TXOP(txop_init_confirm), 
    TXOP(txop_init_recv), TXOP(txop_init_send) },
  { "subscribe", sizeof(tx_subscribe_t), 0,
    TXOP(txop_sub_init), TXOP(txop_sub_confirm) },
  { "ident", sizeof(tx_id_t), 0,
    TXOP(txop_ident_init), TXOP(txop_ident_confirm), 
    TXOP(txop_ident_recv), TXOP(txop_ident_send) },
  { "account", sizeof(tx_account_t), offsetof(tx_account_t, pam_seed),
    &txop_account_init, &txop_account_confirm, &txop_account_recv, &txop_account_send },
  { "session", sizeof(tx_session_t), 0,
    &txop_session_init, &txop_session_confirm, &txop_session_recv, &txop_session_send },
  { "app", sizeof(tx_app_t), offsetof(struct tx_app_t, app_stamp),
    &txop_app_init, &txop_app_confirm },
  { "file", sizeof(struct tx_file_t), offsetof(struct tx_file_t, ino_crc), 
    &txop_file_init, &txop_file_confirm, &txop_file_recv, &txop_file_send },
  { "ward", sizeof(tx_ward_t), 0,
    &txop_ward_init, &txop_ward_confirm, &txop_ward_recv, &txop_ward_send },
  { "trust", sizeof(tx_trust_t), 0, &txop_trust_init, &txop_trust_confirm },
  { "ledger", sizeof(tx_ledger_t) },
  { "license", sizeof(tx_license_t), 0,
    &txop_lic_init, &txop_lic_confirm, &txop_lic_recv, &txop_lic_send },
  { "metric", sizeof(tx_metric_t), 0,
    &txop_metric_init, &txop_metric_confirm, &txop_metric_recv, &txop_metric_send },
  { "reserved_01" },
  { "task", sizeof(tx_task_t) },
  { "thread", sizeof(tx_thread_t), offsetof(struct tx_thread_t, th_stamp),
       &txop_thread_init, &txop_thread_confirm, &txop_thread_recv, &txop_thread_send },
  { "wallet", sizeof(tx_wallet_t) },
  { "bond", sizeof(tx_bond_t) },
  { "asset", sizeof(tx_asset_t), 0, &txop_asset_init, &txop_asset_confirm },
  { "reserved_02" },
  { "event", sizeof(tx_event_t), 0,
    &txop_event_init, &txop_event_confirm, &txop_event_recv, &txop_event_send },
  { "vote", sizeof(tx_vote_t) },
};

txop_t *get_tx_op(int tx_op)
{
  if (tx_op < 0 || tx_op >= MAX_TX)
    return (NULL);

  return (&_txop_table[tx_op]);
}

int confirm_tx_key(txop_t *op, tx_t *tx)
{
  shkey_t *c_key = &tx->tx_key;
  shkey_t *key;
  shbuf_t *buff;
  tx_t *k_tx;
  size_t len;
  int confirm;

  if (!op || !tx)
    return (SHERR_INVAL);

  if (op->op_size == 0)
    return (0);

  /* allocate working copy */
  len = MAX(sizeof(tx_t), op->op_keylen ? op->op_keylen : op->op_size);
  buff = shbuf_init();
  shbuf_cat(buff, (char *)tx, len);

  /* blank out tx key */
  k_tx = (tx_t *)shbuf_data(buff);
  memset(&k_tx->tx_key, '\000', sizeof(k_tx->tx_key));

  /* verify generated tx key matches. */
  key = shkey_bin(shbuf_data(buff), shbuf_size(buff));
  confirm = shkey_cmp(c_key, key);
  shkey_free(&key);
  shbuf_free(&buff);

  if (!confirm)
    return (SHERR_INVAL);

  return (0);
}

void *tx_pstore_load(tx_t *tx)
{
  return (pstore_load(tx->tx_op, shkey_hex(&tx->tx_key)));
}

int tx_confirm(shpeer_t *cli_peer, tx_t *tx)
{
  txop_t *op;  
  int err;

  op = get_tx_op(tx->tx_op);
  if (!op)
    return (SHERR_INVAL);

  if (op->op_size == 0)
    return (0);

  /* verify tx key reflect transaction received. */
  err = confirm_tx_key(op, tx);
fprintf(stderr, "DEBUG: tx_confirm: %d (%s) = confirm_tx_key(tx)\n", err, sherrstr(err));
  if (err)
    return (err);

  if (op->op_conf) {
    /* transaction-level integrity verification */
    err = op->op_conf(cli_peer, tx);
    if (err)
      return (err);
  }

  return (0);
}

int tx_recv(shpeer_t *cli_peer, tx_t *tx)
{
  tx_t *rec_tx;
  txop_t *op;
  int err;

  if (!tx)
    return (SHERR_INVAL);

  op = get_tx_op(tx->tx_op);
  if (!op)
    return (SHERR_INVAL);


/* check for dup in ledger (?) */

  rec_tx = (tx_t *)pstore_load(tx->tx_op, tx->hash);
  if (!rec_tx) {
    rec_tx = (tx_t *)calloc(1, op->op_size);
    if (!rec_tx)
      return (SHERR_NOMEM);

    memcpy(rec_tx, tx, op->op_size);
#if 0 
    err = tx_init(cli_peer, rec_tx);
    if (err) {
      pstore_free(rec_tx);
      return (err);
    }
#endif

    if (tx->tx_op != TX_SUBSCRIBE) {
      err = pstore_save(rec_tx, op->op_size);
      if (err) {
        pstore_free(rec_tx);
        return (err);
      }
    }
  }

  if (op->op_recv) {
    err = op->op_recv(cli_peer, tx);
    if (err) {
      pstore_free(rec_tx);
      return (err);
    }
  }

  pstore_free(rec_tx);

  return (0);
}

int tx_init(shpeer_t *cli_peer, tx_t *tx, int tx_op)
{
  shkey_t *key;
  txop_t *op;
  int tx_len;
  int err;

  if (!tx)
    return (SHERR_INVAL);

  op = get_tx_op(tx_op);
  if (!op)
    return (SHERR_INVAL);

  if (op->op_size == 0)
    return (0);

  tx->tx_op = tx_op;

  if (op->op_init) {
    err = op->op_init(cli_peer, tx);
    if (err)
      return (err);
  }

  err = local_transid_generate(tx_op, tx);
  if (err)
    return (err); /* scrypt error */

  /* generate transaction key */
  memcpy(&tx->tx_key, ashkey_blank(), sizeof(tx->tx_key));
  key = shkey_bin((char *)tx, op->op_keylen ? op->op_keylen : op->op_size);
  memcpy(&tx->tx_key, key, sizeof(tx->tx_key));
  shkey_free(&key);
 
  return (0);
}

int tx_send(shpeer_t *cli_peer, tx_t *tx)
{
  unsigned char *data = (unsigned char *)tx;
  size_t data_len;
  int err;
  txop_t *op;

  if (!data)
    return (SHERR_INVAL);

  data_len = get_tx_size(tx);
  if (!data_len)
    return (0);

  op = get_tx_op(tx->tx_op);
  if (!op)
    return (SHERR_INVAL);

  if (op->op_send) {
    err = op->op_send(cli_peer, data);
    if (err) {
      if (err == SHERR_OPNOTSUPP)
        return (0);

      return (err);
    }
  }

  if (cli_peer) {
    sched_tx_sink(shpeer_kpriv(cli_peer), data, data_len);
  } else {
    sched_tx(data, data_len);
  }

  return (0);
}



shkey_t *get_tx_key(tx_t *tx)
{
  tx_license_t *lic;
  tx_file_t *ino;
  tx_app_t *app;
  tx_ward_t *ward;
  tx_bond_t *bond;
  tx_account_t *acc;
  tx_thread_t *th;
  tx_task_t *task;
  tx_session_t *sess;
  tx_id_t *id;
  shkey_t *ret_key;

  if (!tx)
    return (ashkey_blank());

  ret_key = NULL;
  switch (tx->tx_op) {
    case TX_LICENSE:
      lic = (tx_license_t *)tx;
      ret_key = &lic->lic.lic_sig;
      break;
    case TX_FILE:
      ino = (tx_file_t *)tx;
      ret_key = &ino->ino_name;
      break;
    case TX_APP:
      app = (tx_app_t *)tx;
      ret_key = &app->app_sig;
      break;
    case TX_BOND:
      bond = (tx_bond_t *)tx;
      ret_key = &bond->bond_sig;
      break;
    case TX_WARD:
      ward = (tx_ward_t *)tx;
      ret_key = &ward->ward_sig.sig_key;
      break;
    case TX_IDENT:
      id = (tx_id_t *)tx;
      ret_key = &id->id_key;
      break;
    case TX_ACCOUNT:
      acc = (tx_account_t *)tx;
      ret_key = &acc->pam_seed.seed_sig;
      break;
    case TX_TASK:
      task = (tx_task_t *)tx;
      ret_key = &task->task.task_id;
      break;
    case TX_THREAD:
      th = (tx_thread_t *)tx;
      ret_key = &th->th.th_job;
      break;
    case TX_SESSION:
      sess = (tx_session_t *)tx;
      ret_key = &sess->sess_key;
      break;
    default:
      ret_key = &tx->tx_key;
      break;
  }

  return (ret_key);
}

/**
 * @returns An allocated parent transaction or NULL.
 */
tx_t *get_tx_parent(tx_t *tx)
{
  tx_t *p_tx;

  if (0 == shkey_cmp(&tx->tx_pkey, ashkey_blank()))
    return (NULL);

  return ((tx_t *)pstore_load(tx->tx_ptype, shkey_hex(&tx->tx_pkey)));
}

/**
 * Obtain the root transaction key of the transaction hierarchy.
 * @returns An unallocated tx key or NULL if tx is the root tx.
 */
shkey_t *get_tx_key_root(tx_t *tx)
{
  static shkey_t ret_key;
  tx_t *t_tx;
  tx_t *p_tx;

  t_tx = tx;
  memcpy(&ret_key, ashkey_blank(), sizeof(ret_key));
  while ((p_tx = get_tx_parent(t_tx))) {
    if (t_tx != tx)
      pstore_free(t_tx);

    t_tx = p_tx;
    memcpy(&ret_key, get_tx_key(t_tx), sizeof(ret_key));
  }

  return (&ret_key);
}

const char *get_tx_label(int tx_op)
{
  static const char *unknown_str = "unknown";
  txop_t *op;

  op = get_tx_op(tx_op);
  if (!op)
    return (unknown_str);

  return (op->op_name);
}

size_t get_tx_size(tx_t *tx)
{
  tx_asset_t *ass;
  tx_file_t *ino;
  tx_task_t *task;
  txop_t *op;
  size_t ret_size;
  
  if (!tx)
    return (0);

  ret_size = 0;
  op = get_tx_op(tx->tx_op);
  if (!op)
    return (0);

  switch (tx->tx_op) {
    case TX_FILE:
      ino = (tx_file_t *)tx;
      ret_size = op->op_size + ino->seg_len;
      break;
    case TX_ASSET:
      ass = (tx_asset_t *)tx;
      ret_size = op->op_size + ass->ass_size;
      break;
#if 0
    case TX_TASK:
      task = (tx_task_t *)tx;
      ret_size = op->op_size + (task->task.max_mod * sizeof(sexe_mod_t));
      break;
#endif
    default:
      ret_size = op->op_size;
      break;
  }
fprintf(stderr, "DEBUG: get_tx_size; tx_op %d, size %d\n", (int)tx->tx_op, (int)ret_size);

  return (ret_size);
}


void wrap_bytes(void *data, size_t data_len)
{
  uint32_t sw_data[1024];
  uint32_t *i_data;
  size_t sw_len;
  int sw_data_idx;
  int tot;
  int i;

  if (data_len == sizeof(uint16_t)) {
    uint16_t *sh_data = (uint16_t)(data + sw_len);
    uint16_t val = htons(*sh_data);
    memcpy(data, &val, sizeof(uint16_t)); 
    return;
  }

  data_len = MIN(data_len, 4096);

  tot = data_len / 4;
  if (tot == 0)
    return;

  sw_data_idx = 0;
  i_data = (uint32_t *)data;
  for (i = (tot-1); i >= 0; i--) {
    sw_data[sw_data_idx++] = (uint32_t)htonl(i_data[tot]);
    sw_len += sizeof(uint32_t);
  }
  memcpy(data, sw_data, sw_len);


}

void unwrap_bytes(void *data, size_t data_len)
{
  uint32_t sw_data[1024];
  uint32_t *i_data;
  size_t sw_len;
  int sw_data_idx;
  int tot;
  int i;

  if (data_len == sizeof(uint16_t)) {
    uint16_t *sh_data = (uint16_t)(data + sw_len);
    uint16_t val = ntohs(*sh_data);
    memcpy(data, &val, sizeof(uint16_t)); 
    return;
  }

  data_len = MIN(data_len, 4096);

  tot = data_len / 4;
  if (tot == 0)
    return;

  sw_data_idx = 0;
  i_data = (uint32_t *)data;
  for (i = (tot-1); i >= 0; i--) {
    sw_data[sw_data_idx++] = (uint32_t)ntohl(i_data[tot]);
    sw_len += sizeof(uint32_t);
  }
  memcpy(data, sw_data, sw_len);


}


