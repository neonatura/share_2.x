
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

#define DEFAULT_SCRYPT_DIFFICULTY 0.0025

static void _fcrypt_generate_transaction_id(tx_t *tx)
{
  shkey_t *key;
	int step = 10240;
  unsigned int idx;
  uint64_t best_crc;
  uint64_t crc;
  int crc_once;
int nonce;

  memset(tx->hash, 0, sizeof(tx->hash));

  if (step < MIN_TX_ONCE)
    step = MIN_TX_ONCE;
  else if (step > (MAX_TX_ONCE / 2))
    step = MAX_TX_ONCE / 2;

  best_crc = 0;
  crc_once = -1;
  nonce = 0;
//  printf("starting..\n");
  tx->tx_stamp = (uint64_t)shtime();
  for (idx = step; idx < MAX_TX_ONCE; idx += step) {
    tx->nonce = idx; 
    crc = shcrc(tx, sizeof(tx_t)); 
    if (!best_crc || crc < best_crc) {
      best_crc = crc;
      crc_once = idx;
      nonce = 0;
    } else {
      nonce++;
      if (nonce > step)
        break;
    }
  }
//printf("ending @ %d (once %d)..\n", idx, crc_once);

  key = shkey_bin((char *)&best_crc, sizeof(best_crc)); 
  sprintf(tx->hash, "%-64.64s", shkey_hex(key));
  shkey_free(&key);

  tx->nonce = crc_once;
	tx->tx_method = TXHASH_FCRYPT;
  //tx->tx_id = best_crc;

}

static int _scrypt_generate_transaction_id(tx_t *tx, char **merkle_list)
{
  const uint32_t *ostate;
	scrypt_peer speer;
	scrypt_work work;
	char nonce1[256];
	char nbit[256];
  char cb1[256];
  time_t now;
  char ntime[64];
  int err;
  int i;

	memset(&work, 0, sizeof(work));

	memset(&speer, 0, sizeof(speer));
	sprintf(nonce1, "%-8.8x", 0);
	shscrypt_peer(&speer, nonce1, DEFAULT_SCRYPT_DIFFICULTY);

  sprintf(work.xnonce2, "%-8.8x", 0x0);


	sprintf(nbit, "%-8.8x", 
			(sizeof(tx_t) * (server_ledger->ledger_height+1)));
  strcpy(cb1, shkey_hex(shpeer_kpub(server_peer)));
  now = time(NULL);
  sprintf(ntime, "%-8.8x", (unsigned int)now); 
  shscrypt_work(&speer, &work, merkle_list, server_ledger->parent_hash, cb1, server_ledger->tx.hash, nbit, ntime);
  err = shscrypt(&work, 10240);
  if (err) {
    PRINT_ERROR(err, "_scrypt_generate_transaction_id");
    return (err);
	}

	ostate = (uint32_t *)work.hash;
  memset(tx->hash, 0, sizeof(tx->hash));
  for (i = 0; i < 8; i++)
    sprintf(tx->hash+strlen(tx->hash), "%-8.8x", ostate[i]);
	tx->nonce = work.hash_nonce;
	tx->tx_method = TXHASH_SCRYPT;

	return (0);
}

int generate_transaction_id(int tx_op, tx_t *tx, char *hash)
{
  shpeer_t *peer;
  char *merkle_list[4];
  char peer_hash[256];
	int err;
  int i;

  peer = sharedaemon_peer();
  sprintf(peer_hash, "%s", shkey_hex(shpeer_kpub(peer)));

  for (i = 0; i < 4; i++)
    merkle_list[i] = NULL;

  merkle_list[0] = peer_hash;
  if (hash) {
    merkle_list[1] = hash;
  }

  /* The time-stamp of when the transaction originated. */
  tx->tx_stamp = (uint64_t)shtime();

  /* operation mode being referenced. */
  tx->tx_op = tx_op;

  /* record originating peer */
  memcpy(&tx->tx_peer, shpeer_kpub(peer), sizeof(shkey_t)); 

	err = _scrypt_generate_transaction_id(tx, merkle_list);
	if (err)
		return (err);

	return (0);
}


int has_tx_access(tx_id_t *id, tx_t *tx)
{
  shpeer_t *lcl_peer;
  tx_id_t lcl_id;

	if (tx->tx_group == TX_GROUP_PRIVATE) {
		lcl_peer = shpeer();
		if (!shkey_cmp(shpeer_kpub(lcl_peer), &tx->tx_peer)) {
			/* transaction did not originate from local node. */
			return (FALSE);
		}
	} else if (tx->tx_group == TX_GROUP_PEER) {
    memset(&lcl_id, 0, sizeof(lcl_id));
#if 0
		get_identity_id(&lcl_id);
#endif
		if (!shkey_cmp(shpeer_kpub(&id->id_peer), shpeer_kpub(&lcl_id.id_peer))) {
			/* transaction did not originate from peer group. */
			return (FALSE);
		}
  }

  return (TRUE);
}


