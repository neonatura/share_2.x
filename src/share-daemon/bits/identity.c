
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


/**
 * Obtain the public and peer keys by supplying the transaction and private key.
 * @note The "key_peer" field is filled in on identify confirmation via peers.
 */
void get_identity_id(sh_id_t *id)
{
  struct sh_id_t gen_id;
  char *sha;

  memset(id->hash, 0, sizeof(id->hash));
  memset(&id->key_pub, 0, sizeof(shkey_t));
  memset(&id->key_peer, 0, sizeof(shkey_t));

//  memcpy(&id->key_peer, shkey_bin(&id, sizeof(sh_id_t)), sizeof(shkey_t));
  memcpy(&id->key_pub, shkey_bin(id, sizeof(sh_id_t)), sizeof(shkey_t));

  memset(&gen_id, 0, sizeof(gen_id));
  memcpy(&gen_id.key_pub, &id->key_pub, sizeof(shkey_t)); 
  //memcpy(&gen_id.key_priv, &id->key_priv, sizeof(shkey_t)); 
  memcpy(&gen_id.tx, &id->tx, sizeof(sh_tx_t));

  sha = shdigest(&gen_id, sizeof(sh_id_t));
  strncpy(id->hash, sha, sizeof(id->hash) - 1);
}

/**
 * Generate a new identity using a seed value.
 */
void generate_identity_id(sh_id_t *id)
{
	static uint64_t seed;
	shpeer_t *peer;
  unsigned int idx;
  uint64_t best_crc;
  uint64_t crc;
  shkey_t key;
  int crc_once;
  int nonce;

	if (!seed) {
		peer = sharedaemon_peer();
		seed = shcrc(peer->hwaddr, sizeof(peer->hwaddr)); 
	}
	seed++;

  generate_transaction_id(&id->tx);

  memset(&id->key_pub, 0, sizeof(shkey_t));
  memset(&id->key_peer, 0, sizeof(shkey_t));
  memcpy(&id->key_priv, shkey_num(seed), sizeof(id->key_priv)); 
  memcpy(&id->key_priv, shkey_bin(&id, sizeof(sh_id_t)), sizeof(shkey_t));

  get_identity_id(id);

}

