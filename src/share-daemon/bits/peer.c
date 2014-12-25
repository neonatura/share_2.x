

/*
 * @copyright
 *
 *  Copyright 2014 Neo Natura
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


int confirm_peer(tx_peer_t *peer)
{
  int err;

/* DEBUG: TODO: sk_connect to peer addr:host to verify */

#if 0
  err = confirm_trust(&peer->peer_trust);
  if (err)
    return (err);
#endif

fprintf(stderr, "DEBUG: confirm_peer: SCHED-TX: %s\n", peer->peer_tx.hash);
  generate_transaction_id(TX_PEER, &peer->tx, NULL);
  sched_tx(peer, sizeof(tx_peer_t));

  return (0);
}

/**
 * Generate a peer transaction.
 */
int generate_peer_tx(tx_peer_t *tx, shpeer_t *peer)
{
  shkey_t *key;
  char hash[256];
  int err;

  if (!peer)
    return (SHERR_INVAL);

  sprintf(hash, "%s", shkey_hex(shpeer_kpub(peer)));
  err = generate_transaction_id(TX_PEER, &tx->peer_tx, hash);
  if (err)
    return (err);

#if 0
  key = shkey_bin((unsigned char *)tx, sizeof(tx_peer_t));
  err = generate_trust(&tx->peer_trust, peer, key);
fprintf(stderr, "DEBUG: generate_peer_tx: %d = generate_trust()\n", err);
  shkey_free(&key);
  if (err)
    return (err);
#endif

  memcpy(&tx->peer, peer, sizeof(shpeer_t));

  return (confirm_peer(tx));
}


int process_peer_tx(shkey_t *src_peer, tx_peer_t *tx)
{
  int err;

  err = confirm_peer(tx);
  if (err)
    return (err);

  return (0);
}


