
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

  /* .. */

  generate_transaction_id(TX_IDENT, &id->tx, NULL);
  sched_tx(id, sizeof(tx_id_t));

  return (0);
}

#if 0
/**
 * Obtain the public and peer keys by supplying the transaction and private key.
 * @note The "key_peer" field is filled in on identity confirmation via peers.
 */
void get_identity_id(tx_id_t *id)
{
  struct tx_id_t gen_id;
  shkey_t *id_key;
  char hash[256];

  id_key = shkey_bin((char *)&gen_id, sizeof(tx_id_t));
  sprintf(hash, "%s", shkey_hex(id_key));
fprintf(stderr, "DEBUG: get_identity_id: generaet_transaction_id f/ '%s'\n", hash);
  shkey_free(&id_key);
  generate_transaction_id(TX_IDENT, &gen_id.id_tx, hash); 

}
#endif

/**
 * Generate a new identity using a seed value.
 */
int generate_identity_id(tx_id_t *id, shkey_t *seed)
{
	shpeer_t *peer;
  unsigned int idx;
  uint64_t best_crc;
  uint64_t crc;
  shkey_t key;
  int crc_once;
  int nonce;

	if (!seed) {
		peer = ashpeer();
    seed = shpeer_kpub(peer);
	}

#if 0
  get_identity_id(id);
#endif

  return (confirm_identity(id));
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
