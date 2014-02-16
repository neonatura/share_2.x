
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


void generate_transaction_id(sh_tx_t *tx, int step)
{
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
  printf("starting..\n");
  tx->tx_stamp = (uint64_t)shtime();
  for (idx = step; idx < MAX_TX_ONCE; idx += step) {
    tx->once = idx; 
    crc = shcrc(tx, sizeof(sh_tx_t)); 
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
printf("ending @ %d (once %d)..\n", idx, crc_once);

  strcpy(tx->hash, shdigest((char *)tx, sizeof(sh_tx_t)));
  tx->once = crc_once;
  tx->tx_id = best_crc;

}


int has_tx_access(sh_id_t *id, sh_tx_t *tx)
{
  shpeer_t *lcl_peer;
  sh_id_t lcl_id;

	if (tx->tx_group == TX_GROUP_PRIVATE) {
		lcl_peer = shpeer();
		if (!shkey_cmp(&lcl_peer->name, &tx->tx_peer)) {
			/* transaction did not originate from local node. */
			return (FALSE);
		}
	} else if (tx->tx_group == TX_GROUP_PEER) {
		get_identity_id(&lcl_id);
		if (!shkey_cmp(&id->key_peer, &lcl_id.key_peer)) {
			/* transaction did not originate from peer group. */
			return (FALSE);
		}
  }

  return (TRUE);
}


