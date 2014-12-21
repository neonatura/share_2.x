

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
#include "../sharedaemon_file.h"



/**
 * Processed from server peer preceeding actual transaction operation.
 */
int confirm_trust(tx_trust_t *trust)
{
  tx_trust_t cmp_trust;
  char path[PATH_MAX+1];

  if (!trust->trust_stamp)
    return (SHERR_INVAL);

  memcpy(&cmp_trust, trust, sizeof(tx_trust_t));
  memset(&cmp_trust.tx, 0, sizeof(tx_t));
  cmp_trust.trust_ref = 0;

  trust->trust_ref++;

#if 0
fprintf(stderr, "DEBUG: confirm_trust: SCHED-TX: %s\n", trust->trust_tx.hash); 
  generate_transaction_id(TX_TRUST, &trust->tx, NULL);
  sched_tx(trust, sizeof(tx_trust_t));
#endif

  return (0);
}


int generate_trust(tx_trust_t *trust, shpeer_t *peer, shkey_t *context)
{
  int err;

  if (!trust)
    return (SHERR_INVAL);

  memset(trust, 0, sizeof(tx_trust_t));

  err = generate_transaction_id(TX_TRUST, &trust->trust_tx, NULL);
  if (err)
    return (err);

  memset(&trust->tx, 0, sizeof(tx_t));
  memset(&trust->trust_id, 0, sizeof(shkey_t));
  trust->trust_ref = 0;

  trust->trust_stamp = (uint64_t)shtime();
  memcpy(&trust->trust_peer, shpeer_kpub(peer), sizeof(shkey_t));

  if (context)
    memcpy(&trust->trust_id, context, sizeof(shkey_t));

  return (confirm_trust(trust));
}

int process_trust_tx(tx_app_t *cli, tx_trust_t *trust)
{
  tx_trust_t *ent;
  int err;

  ent = (tx_trust_t *)pstore_load(trust->trust_tx.hash);
  if (!ent) {
    err = confirm_trust(trust);
    if (err)
      return (err);

    pstore_save(trust, sizeof(tx_trust_t));
  }

  return (0);
}

