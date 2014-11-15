
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



void generate_signature(shsig_t *sig, shpeer_t *peer, sh_tx_t *tx, sh_id_t *id)
{
  shkey_t *key;

  sig->sig_stamp = (uint64_t)shtime();
  strcpy(sig->sig_tx, tx->hash);
  memcpy(&sig->sig_peer, &peer->name, sizeof(shkey_t));

  key = shkey_bin(&id, sizeof(sh_id_t));
  memcpy(&sig->sig_id, key, sizeof(shkey_t));
  shkey_free(&key);

  memset(&sig->sig_key, 0, sizeof(shkey_t));
  key = shkey_bin(&sig, sizeof(shsig_t));
  memcpy(&sig->sig_key, key, sizeof(shkey_t));
  shkey_free(&key);

}

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

int verify_signature_tx(shpeer_t *peer, shsig_t *sig, sh_tx_t *tx, sh_id_t *id)
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
shsig_t *find_transaction_signature(sh_tx_t *tx)
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



