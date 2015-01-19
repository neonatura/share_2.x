
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


int confirm_signature(shsig_t *sig, char *tx_hash)
{
  uint64_t crc;
  int err;

  crc = (uint64_t)strtoll(tx_hash, NULL, 16);
  err = shkey_verify(&sig->sig_key, crc, &sig->sig_peer, sig->sig_stamp);
  if (err)
    return (err);

  return (0);
}

void generate_signature(shsig_t *sig, shpeer_t *peer, tx_t *tx)
{
  uint64_t crc;
  shkey_t *sig_key;
  shkey_t *key;
  int err;

  memset(sig, 0, sizeof(shsig_t));

  /* assign origin timestamp */
  sig->sig_stamp = shtime64();

  if (peer)
    memcpy(&sig->sig_peer, shpeer_kpriv(peer), sizeof(shkey_t));

  /* convert hash to checksum */
  crc = (uint64_t)strtoll(tx->hash, NULL, 16);

  sig_key = shkey_cert(&sig->sig_peer, crc, sig->sig_stamp);
  memcpy(&sig->sig_key, sig_key, sizeof(shkey_t));
  shkey_free(&sig_key);

  key = shkey_bin((char *)&sig, sizeof(sig));
  memcpy(&sig->sig_id, key, sizeof(shkey_t));
  shkey_free(&key);


}


#if 0
int process_signature_tx(tx_app_t *cli, tx_sig_t *sig)
{
  tx_sig_t *ent;
  int err;

  ent = (tx_sig_t *)pstore_load(TX_SIGNATURE, sig->sig_tx.hash);
  if (!ent) {
    err = confirm_signature(sig, &sig->sig_tx);
    if (err)
      return (err);

    pstore_save(sig, sizeof(tx_sig_t));
  }

  return (0);
}
#endif




