

/*
 * @copyright
 *
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
 *
 *  @endcopyright
 */

#include "sharedaemon.h"


int confirm_ward(tx_ward_t *ward)
{
  shsig_t *sig;
  int err;

  err = confirm_signature(&ward->ward_sig, shpeer_kpriv(&ward->ward_peer), ward->ward_tx.hash);
  if (err)
    return (err);

  sched_tx(ward, sizeof(tx_ward_t));
  return (0);
}

/**
 * A trusted client is requesting a ward on a transaction be created.
 */
int generate_ward(tx_ward_t *ward, tx_t *tx, tx_id_t *id)
{
  shpeer_t *self_peer;
  int err;

  ward->ward_op = tx->tx_op;
  memcpy(&ward->ward_key, &tx->tx_key, sizeof(shkey_t));

  if (id)
    ward->ward_id = id->id_uid;

  err = tx_init(NULL, (tx_t *)ward, TX_WARD);
  if (err)
    return (err);

  return (0);
}

#if 0
int generate_ward(tx_ward_t *ward, tx_t *tx, tx_id_t *id)
{
  shpeer_t *self_peer;

  local_transid_generate(TX_WARD, &ward->ward_tx);
  ward->ward_stamp = shtime();
  self_peer = sharedaemon_peer();
  generate_signature(&ward->ward_sig, shpeer_kpub(self_peer), tx); 
  if (id)
    memcpy(&ward->ward_id, id, sizeof(tx_id_t));

  return (confirm_ward(ward));
}
int process_ward_tx(tx_app_t *cli, tx_ward_t *ward)
{
  tx_ward_t *ent;
  int err;

  ent = (tx_ward_t *)pstore_load(TX_WARD, ward->ward_tx.hash);
  if (!ent) {
    err = confirm_ward(ward);
    if (err)
      return (err);

    pstore_save(ward, sizeof(tx_ward_t));
  }

  return (0);
}
#endif





int txop_ward_init(shpeer_t *cli_peer, tx_ward_t *ward)
{
  shpeer_t *self_peer;
  tx_t *tx;

  tx = pstore_load(ward->ward_op, shkey_hex(&ward->ward_key));
  if (!tx)
    return (SHERR_INVAL);

  ward->ward_stamp = shtime();

  self_peer = sharedaemon_peer();
  generate_signature(&ward->ward_sig, shpeer_kpub(self_peer), tx); 

  return (0);
}

int txop_ward_confirm(shpeer_t *peer, tx_ward_t *ward)
{
  tx_t *tx;
  int err;

  /* verify identity exists */
  tx = (tx_t *)pstore_load(TX_IDENT, shcrcstr(ward->ward_id));
  if (!tx) return (SHERR_NOENT);
  pstore_free(tx);

  /* verify ref tx exists */
  tx = (tx_t *)pstore_load(ward->ward_op, shkey_str(&ward->ward_key));
  if (!tx) return (SHERR_NOENT);
  pstore_free(tx);
  
  err = confirm_signature(&ward->ward_sig, 
      shpeer_kpriv(&ward->ward_peer), shkey_hex(&ward->ward_key));
  pstore_free(tx);
  if (err)
    return (err);

  return (0);
}

int txop_ward_send(shpeer_t *peer, tx_ward_t *ward)
{
  return (0);
}

int txop_ward_recv(shpeer_t *peer, tx_ward_t *ward)
{
  return (0);
}

