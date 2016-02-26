
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




int generate_trust(tx_trust_t *trust, shpeer_t *peer, shkey_t *context)
{
  shkey_t *sig_key;
  uint64_t crc;
  int err;

  if (!trust)
    return (SHERR_INVAL);

  memset(trust, 0, sizeof(tx_trust_t));
  memcpy(&trust->trust_context, ashkey_blank(), sizeof(shkey_t));

  memcpy(&trust->trust_peer, shpeer_kpub(peer), sizeof(shkey_t));
  if (context)
    memcpy(&trust->trust_context, context, sizeof(shkey_t));

  err = tx_init(NULL, trust, TX_TRUST);
  if (err)
    return (err);

  return (0);
}
#if 0
int generate_trust(tx_trust_t *trust, shpeer_t *peer, shkey_t *context)
{
  shkey_t *sig_key;
  uint64_t crc;
  int err;

  if (!trust)
    return (SHERR_INVAL);

  memset(trust, 0, sizeof(tx_trust_t));

  local_transid_generate(TX_TRUST, &trust->trust_tx);
  memcpy(&trust->trust_peer, shpeer_kpub(peer), sizeof(shkey_t));

  memset(&trust->trust_context, 0, sizeof(shkey_t));
  if (context)
    memcpy(&trust->trust_context, context, sizeof(shkey_t));
  else
    memcpy(&trust->trust_context, ashkey_blank(), sizeof(shkey_t));

  crc = shcrc(&trust->trust_context, sizeof(shkey_t)); 
  sig_key = shkey_cert(&trust->trust_peer, crc, trust->trust_tx.tx_stamp);
  memcpy(&trust->trust_sig, sig_key, sizeof(trust->trust_sig));
  free(sig_key);

  return (0);
}
int local_broadcast_trust(tx_trust_t *trust)
{
  sched_tx(trust, sizeof(tx_trust_t));
  return (0);
}
int process_trust_tx(tx_trust_t *trust)
{
  tx_trust_t *ent;
  int err;

  ent = (tx_trust_t *)pstore_load(trust->trust_tx.hash);
  if (!ent) {
    err = local_broadcast_trust(trust);
    if (err)
      return (err);

    pstore_save(trust, sizeof(tx_trust_t));
  }

  return (0);
}
int validate_trust(tx_trust_t *trust)
{
  int err;
  uint64_t crc;

  crc = shcrc(&trust->trust_context, sizeof(trust->trust_context));
  err = shkey_verify(&trust->trust_sig, crc, 
      &trust->trust_peer, trust->trust_tx.tx_stamp);
  if (err)
    return (err);

  return (0);
}
int remote_trust_receive(tx_app_t *cli, tx_trust_t *trust)
{
  int err;

  err = validate_trust(trust);
  if (err) {
    decr_app_trust(cli);
    return (err);
  }
  incr_app_trust(cli);
  
  err = process_trust_tx(trust);
  if (err)
    return (err);

  return (0);
}
#endif


int txop_trust_init(shpeer_t *cli_peer, tx_trust_t *trust)
{
  shkey_t *sig_key;
  uint64_t crc;
  int err;

  if (!trust)
    return (SHERR_INVAL);

  crc = shcrc(&trust->trust_context, sizeof(shkey_t)); 
  sig_key = shkey_cert(&trust->trust_peer, crc, trust->trust_tx.tx_stamp);
  memcpy(&trust->trust_sig, sig_key, sizeof(trust->trust_sig));
  free(sig_key);

  return (0);
}

int txop_trust_confirm(shpeer_t *cli_peer, tx_trust_t *trust)
{
  uint64_t crc;
  int err;

  crc = shcrc(&trust->trust_context, sizeof(trust->trust_context));
  err = shkey_verify(&trust->trust_sig, crc, 
      &trust->trust_peer, trust->trust_tx.tx_stamp);
  if (err)
    return (err);
}

