

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

  err = verify_signature(&ward->ward_sig.sig_key, ward->ward_tx.hash, &ward->ward_sig.sig_peer, ward->ward_sig.sig_stamp);
  if (err)
    return (err);

fprintf(stderr, "DEBUG: confirm_ward; SCHED-TX: %s\n", ward->ward_tx.hash);
  generate_transaction_id(TX_WARD, &ward->tx, NULL);
  sched_tx(ward, sizeof(tx_ward_t));

  return (0);
}

/**
 * A trusted client is requesting a ward on a transaction be created.
 */
int generate_ward(tx_ward_t *ward, tx_t *tx, tx_id_t *id)
{
  shpeer_t *self_peer;

  memcpy(&ward->ward_tx, tx, sizeof(tx_t));
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


