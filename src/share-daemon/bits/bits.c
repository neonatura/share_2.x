
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


static txop_t _txop_table[MAX_TX] = {
  { "none" },
  { "init", sizeof(tx_init_t), 0,
    txop_init_init, txop_init_confirm, txop_init_recv, txop_init_send },
  { "subscribe", sizeof(tx_subscribe_t) },
  { "ident", sizeof(tx_id_t), 0,
    txop_ident_init, txop_ident_confirm, txop_ident_recv, txop_ident_send },
  { "account", sizeof(tx_account_t), offsetof(tx_account_t, pam_seed),
    txop_account_init, txop_account_confirm, txop_account_recv, txop_account_send },
  { "session", sizeof(tx_session_t), 0,
    txop_session_init, txop_session_confirm, txop_session_recv, txop_session_send },
  { "app", sizeof(tx_app_t) },
  { "file", sizeof(struct tx_file_t), offsetof(struct tx_file_t, ino_crc), 
    txop_file_init, txop_file_confirm, txop_file_recv, txop_file_send },
  { "ward", sizeof(tx_ward_t), 0,
    txop_ward_init, txop_ward_confirm, txop_ward_recv, txop_ward_send },
  { "trust", sizeof(tx_trust_t), 0, txop_trust_init, txop_trust_confirm },
  { "ledger", sizeof(tx_ledger_t) },
  { "license", sizeof(tx_license_t) },
  { "metric", sizeof(tx_metric_t), 0,
    txop_metric_init, txop_metric_confirm, txop_metric_recv, txop_metric_send },
  { "reserved_01" },
  { "task", sizeof(tx_task_t) },
  { "thread", sizeof(tx_thread_t) },
  { "wallet", sizeof(tx_wallet_t) },
  { "bond", sizeof(tx_bond_t) },
  { "asset", sizeof(tx_asset_t) },
  { "reserved_02" },
  { "event", sizeof(tx_event_t) },
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
  int confirm;

  if (!op || !tx)
    return (SHERR_INVAL);

  buff = shbuf_init();
  shbuf_cat(buff, (char *)tx, op->op_keylen ? op->op_keylen : op->op_size);
  shbuf_free(&buff);

  k_tx = (tx_t *)shbuf_data(buff);
  memset(&k_tx->tx_key, '\000', sizeof(k_tx->tx_key));

  key = shkey_bin(shbuf_data(buff), shbuf_size(buff));
  confirm = shkey_cmp(c_key, key);
  shkey_free(&key);

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

  err = op->op_conf(cli_peer, tx);
  if (err)
    return (err);

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

/* check for dup in ledger */

  /* verify tx key reflect transaction received. */
  err = confirm_tx_key(op, tx);
  if (err)
    return (err);

  /* transaction-level integrity verification */
  err = op->op_conf(cli_peer, tx);
  if (err)
    return (err);

  rec_tx = (tx_t *)pstore_load(tx->tx_op, tx->hash);
  if (!rec_tx) {
    rec_tx = (tx_t *)calloc(1, op->op_size);
    if (!rec_tx)
      return (SHERR_NOMEM);

    memcpy(rec_tx, tx, op->op_size);
    err = tx_init(cli_peer, rec_tx);
    if (err) {
      pstore_free(rec_tx);
      return (err);
    }

    err = pstore_save(rec_tx, op->op_size);
    if (err) {
      pstore_free(rec_tx);
      return (err);
    }
  }

  err = op->op_recv(cli_peer, tx);
  if (err) {
    pstore_free(rec_tx);
    return (err);
  }

  pstore_free(rec_tx);

  return (0);
}

int tx_init(shpeer_t *cli_peer, tx_t *tx)
{
  shkey_t *key;
  txop_t *op;
  int tx_len;
  int err;

  if (!tx)
    return (SHERR_INVAL);

  op = get_tx_op(tx->tx_op);
  if (!op)
    return (SHERR_INVAL);

  err = op->op_init(cli_peer, tx);
  if (err)
    return (err);

  err = local_transid_generate(tx->tx_op, &tx);
  if (err)
    return (err); /* scrypt error */

  memcpy(&tx->tx_key, ashkey_blank(), sizeof(shkey_t));
  key = shkey_bin((char *)tx, op->op_keylen ? op->op_keylen : op->op_size);
  memcpy(&tx->tx_key, key, sizeof(shkey_t));
  shkey_free(&key);
 
  return (0);
}

int tx_send(shpeer_t *cli_peer, tx_t *tx, size_t data_len)
{
  unsigned char *data = (unsigned char *)tx;
  int err;
  txop_t *op;

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


