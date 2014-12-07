

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



void generate_trust(sh_trust_t *trust, shpeer_t *peer, tx_t *tx, sh_id_t *id)
{
  shkey_t *key;

  generate_transaction_id(&trust->trust_tx, NULL);

  trust->trust_stamp = (uint64_t)shtime();
  memcpy(&trust->trust_peer, &peer->name, sizeof(shkey_t));

  key = shkey_bin((char *)&id, sizeof(sh_id_t));
  memcpy(&trust->trust_id, key, sizeof(shkey_t));
  shkey_free(&key);

  memset(&trust->trust_key, 0, sizeof(shkey_t));
  key = shkey_bin((char *)&trust, sizeof(sh_trust_t));
  memcpy(&trust->trust_key, key, sizeof(shkey_t));
  shkey_free(&key);

}

int verify_trust(sh_trust_t *trust, shpeer_t *peer, tx_t *tx, sh_id_t *id)
{
  SHFS *fs = sharedaemon_fs();
  SHFL *fl;
  char path[PATH_MAX+1];
  int err;

  trust->trust_ref++;
  sprintf(path, "/shnet/trust/%s", trust->trust_tx);
  fl = shfs_file_find(fs, path);
  err = shfs_file_write(fl, trust, sizeof(sh_trust_t));
  if (err)
    return (err);
  
  return (0);
}

sh_trust_t *find_trust(char *tx_hash)
{
  SHFS *fs = sharedaemon_fs();
  SHFL *fl;
  sh_trust_t *trust;
  char path[PATH_MAX+1];
  size_t trust_len;
  int err;

  sprintf(path, "/shnet/trust/%s", tx_hash);
  fl = shfs_file_find(fs, path);
  err = shfs_file_read(fl, (unsigned char *)&trust, &trust_len);
  if (err)
    return (NULL);

  /* sanity check */
  if (trust_len != sizeof(sh_trust_t)) {
    PRINT_ERROR(SHERR_INVAL, "invalid trust size");
    free(trust);
    return (NULL);
  }
  
  return (trust);
}

/**
 * Processed from server peer preceeding actual transaction operation.
 */
int confirm_trust(sh_trust_t *trust)
{
  SHFS *fs = sharedaemon_fs();
  SHFL *fl;
  char path[PATH_MAX+1];
  int err;

  trust->trust_ref++;
  sprintf(path, "/shnet/trust/%s", trust->trust_tx);
  fl = shfs_file_find(fs, path);
  err = shfs_file_write(fl, trust, sizeof(sh_trust_t));
  if (err)
    return (err);
  
  return (0);
}



