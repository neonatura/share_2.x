
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

/**
 * Obtain the public and peer keys by supplying the transaction and private key.
 * @note The "key_peer" field is filled in on identity confirmation via peers.
 */
void get_identity_id(tx_id_t *id)
{
  struct tx_id_t gen_id;
  shkey_t *pub_key;
  shkey_t *id_key;
  char hash[256];

//  memcpy(&id->key_peer, shkey_bin(&id, sizeof(tx_id_t)), sizeof(shkey_t));
  pub_key = shkey_bin((char *)id, sizeof(tx_id_t));
  memcpy(&id->key_pub, pub_key, sizeof(shkey_t));
  shkey_free(&pub_key);

  memset(&gen_id, 0, sizeof(gen_id));
  memcpy(&gen_id.key_pub, &id->key_pub, sizeof(shkey_t)); 
  //memcpy(&gen_id.key_priv, &id->key_priv, sizeof(shkey_t)); 
  memcpy(&gen_id.tx, &id->tx, sizeof(tx_t));

  id_key = shkey_bin((char *)&gen_id, sizeof(tx_id_t));
  sprintf(hash, "%s", shkey_hex(id_key));
fprintf(stderr, "DEBUG: get_identity_id: generaet_transaction_id f/ '%s'\n", hash);
  generate_transaction_id(TX_IDENT, &gen_id.id_tx, hash); 
  shkey_free(&id_key);

}

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
    seed = &peer->name; 
	}

//  generate_transaction_id(&id->tx, NULL);

  memset(&id->key_pub, 0, sizeof(shkey_t));
  memset(&id->key_peer, 0, sizeof(shkey_t));
  memcpy(&id->key_priv, seed, sizeof(id->key_priv)); 
  memcpy(&id->key_priv, shkey_bin(&id, sizeof(tx_id_t)), sizeof(shkey_t));

  get_identity_id(id);

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
