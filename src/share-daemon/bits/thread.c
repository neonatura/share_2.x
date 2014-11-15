

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
#include "bits.h"


void sched_tx(void *data, size_t data_len)
{
  sh_tx_t *tx = (sh_tx_t *)data;
  sh_tx_t sig_tx;
  shsig_t *sig;
  shsig_t new_sig;
  sh_id_t *id;

  memset(&sig_tx, 0, sizeof(sig_tx));
  sig_tx.tx_op = TX_SIGNATURE;
  generate_transaction_id(&sig_tx);
  id = get_account_identity();
  generate_signature(&sig, sharedaemon_peer(), tx, id);

  /* send preceeding server signature for transaction */
  broadcast_raw(&sig_tx, sizeof(sig_tx));
  broadcast_raw(&sig, sizeof(shsig_t));

  /* send entire message with tx header */
  broadcast_raw(data, data_len);

}

int confirm_ward(sh_ward_t *ward, shpeer_t *peer)
{
  shsig_t *sig;
  int err;

  sig = find_transaction_signature(&ward->ward_tx);
  if (!sig)
    return (SHERR_NOENT);

  err = verify_signature(sig, peer, &ward->ward_tx, &ward->ward_id);
  if (err)
    return (err);

  sched_tx(peer, &ward, sizeof(sh_ward_t));
  return (0);
}

/**
 * A trusted client is requesting a ward on a transaction be created.
 */
int generate_ward(sh_ward_t *ward, sh_tx_t *tx, sh_id_t *id, int step)
{
  sh_ward_t ward;

  memset(&ward, 0, sizeof(ward));
  generate_transaction_id(&ward.tx);
  memcpy(&ward,ward_tx, tx, sizeof(sh_tx_t));
  memcpy(&ward,ward_id, id, sizeof(sh_id_t));

  return (confirm_ward(ward, sharedaemon_peer()));
}



