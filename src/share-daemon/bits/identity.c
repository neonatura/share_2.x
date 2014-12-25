
/*
 * @copyright
 *
 *  Copyright 2013, 2014 Neo Natura
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


int confirm_identity(tx_id_t *id)
{
  int err;

  err = confirm_signature(&id->id_sig, id->id_tx.hash);
  if (err)
    return (err);

fprintf(stderr, "DEBUG: confirm_identify: SCHED-TX: %s\n", id->id_tx.hash);
  generate_transaction_id(TX_IDENT, &id->tx, NULL);
  sched_tx(id, sizeof(tx_id_t));

  return (0);
}

/**
 * Generate a new identity using a seed value.
 */
int generate_identity_tx(tx_id_t *id, tx_account_t *acc, shpeer_t *app_peer, char *acc_name, char *acc_hash)
{
  shkey_t *key;
  int err;

	if (!app_peer)
		app_peer = ashpeer();

  memset(id, 0, sizeof(tx_id_t));
  if (acc)
    memcpy(&id->id_app, shpeer_kpub(app_peer), sizeof(shkey_t));
  if (acc)
    memcpy(&id->id_acc, &acc->acc_name, sizeof(shkey_t));
  if (acc_name)
    strncpy(id->id_label, acc_name, sizeof(id->id_label) - 1);

  key = shkey_bin(id, sizeof(tx_id_t));
  memcpy(&id->id_name, key, sizeof(shkey_t));
  shkey_free(&key);

  err = generate_transaction_id(TX_IDENT, &id->id_tx, NULL);
  if (err)
    return (err);

  generate_signature(&id->id_sig, app_peer, &id->id_tx);

  if (acc_hash)
    strncpy(id->id_hash, acc_hash, sizeof(id->id_hash) - 1);

  return (confirm_identity(id));
}

tx_id_t *generate_identity(tx_account_t *acc, shpeer_t *app_peer, char *acc_user, char *acc_hash)
{
  tx_id_t *id;
  int err;

  id = (tx_id_t *)calloc(1, sizeof(tx_id_t));
  if (!id)
    return (NULL);

  err = generate_identity_tx(id, acc, app_peer, acc_user, acc_hash);
  if (err) {
    free(id);
    return (NULL);
  }

  return (id);
}

int process_identity_tx(tx_app_t *cli, tx_id_t *id)
{
  tx_id_t *ent;
  int err;

  ent = (tx_id_t *)pstore_load(TX_IDENT, id->id_tx.hash);
  if (!ent) {
    err = confirm_identity(id);
    if (err)
      return (err);

    pstore_save(id, sizeof(tx_id_t));
  }

  return (0);
}




