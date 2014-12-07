
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



int generate_signature(shsig_t *sig, shpeer_t *peer, tx_t *tx)
{
  uint64_t crc;
  shkey_t *sig_key;
  int err;

  memset(sig, 0, sizeof(shsig_t));
  sig->sig_stamp = shtime64();
  strncpy(sig->sig_tx, tx->hash, sizeof(sig->sig_tx) - 1);
  memcpy(&sig->sig_peer, &peer->name, sizeof(shkey_t));

  crc = (uint64_t)strtoll(sig->sig_tx, NULL, 16);
  sig_key = shkey_cert(&sig->sig_peer, crc, sig->sig_stamp);
  if (err)
    return (err);

  memcpy(&sig->sig_key, sig_key, sizeof(shkey_t));
  shkey_free(&sig_key);
fprintf(stderr, "DEBUG: generate_signature: (tx %s + stamp %llu + crc %llu) = sig %s\n", sig->sig_tx, sig->sig_stamp, crc, shkey_print(&sig->sig_key));

  return (0);
}

int verify_signature(shkey_t *sig_key, char *tx_hash, shpeer_t *peer, shtime_t sig_stamp)
{
  uint64_t crc;
  int err;

  crc = (uint64_t)strtoll(tx_hash, NULL, 16);
  err = shkey_verify(sig_key, crc, &peer->name, sig_stamp);
fprintf(stderr, "DEBUG: verify_signature: %d = shkey_verify(sig %s, crc %llu, peer %s, stamp %llu)\n", err, shkey_print(sig_key), crc, shkey_print(&peer->name), sig_stamp);
  if (err)
    return (err);

  return (0);
}

#if 0
int verify_signature(shsig_t *sig)
{
  SHFS *fs = sharedaemon_fs();
  SHFL *fl;
  char path[PATH_MAX+1];
  int err;

  sig->sig_ref++;
  sprintf(path, "/shnet/signature/%s", sig->sig_tx);
  fl = shfs_file_find(fs, path);
  err = shfs_file_write(fl, sig, sizeof(shsig_t));
  if (err)
    return (err);
  
  return (0);
}

int verify_signature_tx(shpeer_t *peer, shsig_t *sig, tx_t *tx, sh_id_t *id)
{
	int err;

  err = verify_signature(sig);
	if (err)
		return (err);

	return (0);
}

/**
 * @returns An allocated transactio signature or NULL if none exists.
 */
shsig_t *find_signature(char *tx_hash)
{
  SHFS *fs = sharedaemon_fs();
  SHFL *fl; 
  shsig_t *sig;
  char path[PATH_MAX+1];
  size_t sig_len;
  int err;

  sprintf(path, "/shnet/signature/%s", tx_hash);
  fl = shfs_file_find(fs, path);
  err = shfs_file_read(fl, &sig, &sig_len);
  if (err)
    return (NULL);

  /* sanity check */
  if (sig_len != sizeof(shsig_t)) {
    PRINT_ERROR(SHERR_INVAL, "invalid signature size");
    free(sig);
    return (NULL);
  }
  
  return (sig);
}
shsig_t *find_transaction_signature(tx_t *tx)
{
  return (find_signature(tx->hash));
}

/**
 * Processed from server peer preceeding actual transaction operation.
 */
int confirm_signature(shsig_t *sig)
{
  SHFS *fs = sharedaemon_fs();
  SHFL *fl;
  char path[PATH_MAX+1];
  int err;

  sig->sig_ref++;
  sprintf(path, "/shnet/signature/%s", sig->sig_tx);
  fl = shfs_file_find(fs, path);
  err = shfs_file_write(fl, sig, sizeof(shsig_t));
  if (err)
    return (err);
  
  return (0);
}
#endif



