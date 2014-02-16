

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


static sh_account_t *sharedaemon_account_init()
{
  shpeer_t *peer = sharedaemon_peer();
  shfs_t *fs = sharedaemon_fs();
  shfs_ino_t *fl;
  char path[PATH_MAX+1];
  char *data;
  size_t data_len;
  int err;

  sprintf(path, "/shnet/account/%s", shkey_print(&peer->name));
  fl = shfs_file_find(fs, path);
  err = shfs_file_read(fl, &data, &data_len);
  if (err)
    return (err);

  if (data_len != sizeof(sh_account_t))
    return (SHERR_IO);

  return ((sh_account_t *)data);
}
sh_account_t *sharedaemon_account()
{
  static sh_account_t *ret_account;
  if (!ret_account)
    ret_account = sharedaemon_account_init();
  return (ret_account);
}

/**
 * Generate a new wallet using a seed value.
 */
void generate_account_id(sh_account_t *acc, long seed, int step)
{

  generate_identity_id(&acc->id, seed, step);
  strcpy(acc->hash, shdigest(acc, sizeof(sh_account_t)));


}



/*
sh_id_t *load_account(sh_account_t *acc, size_t *id_len)
{
  shfs_t *fs;
  shfs_ino_t *fl;
  size_t data_len;
  char *data;
  int err;

  fs = shfs_init(NULL);
  if (!fs)
    return (NULL);

  sprintf(path, "/account/%s", acc->hash);
  fl = shfs_file_find(fs, path);
  if (!fl)
    return (NULL);
  
  err = shfs_file_read(fl, &data, &data_len);
  shfs_free(&fs);
  if (err)
    return (NULL);

  *id_len = data_len / sizeof(sh_id_t);
  return ((sh_id_t *)data);
}
*/


int load_account_identity(sh_account_t *acc, sh_id_t *id)
{
  shfs_t *fs = sharedaemon_fs();
  shfs_ino_t *fl;
  char path[PATH_MAX+1];
  char *data;
  size_t data_len;
  int err;

  sprintf(path, "/shnet/account/%s/identity/%s", acc->hash, id->hash);
  fl = shfs_file_find(fs, path);
  err = shfs_file_read(fl, &data, &data_len);
  if (err)
    return (err);

  if (data_len != sizeof(sh_id_t))
    return (SHERR_IO);

  memcpy(id, data, sizeof(sh_id_t));
  free(data);

  return (0);
}

int load_def_account_identity(sh_id_t *id)
{
  sh_account_t *acc = sharedaemon_account();
  return (load_account_identity(acc, id));
}

int add_account_identity(sh_account_t *acc, sh_id_t *id)
{
  shfs_t *fs = sharedaemon_fs();
  shfs_ino_t *fl;
char path[PATH_MAX+1];
  int err;

  sprintf(path, "/shnet/account/%s/identity/%s", acc->hash, id->hash);
  fl = shfs_file_find(fs, path);
  err = shfs_file_write(fl, id, sizeof(sh_id_t));
  if (err)
    return (err);

  return (0);
}

void remove_account_identity(sh_account_t *acc, sh_id_t *id)
{

}

sh_tx_t *load_account_tx(sh_account_t *acc, char *id_hash, size_t *tx_len_p)
{
  shfs_t *fs = sharedaemon_fs();
  shfs_ino_t *fl;
  char path[PATH_MAX+1];
  char *data;
  size_t data_len;
  int err;

  sprintf(path, "/shnet/account/%s/transaction/%s", acc->hash, id_hash);
  fl = shfs_file_find(fs, path);
  if (!fl)
    return (NULL);
  
  err = shfs_file_read(fl, &data, &data_len);
  if (err)
    return (NULL);

  if (tx_len_p)
    *tx_len_p = data_len / sizeof(sh_tx_t);

  return ((sh_tx_t *)data);
}

sh_tx_t *load_def_account_tx(char *id_hash)
{
  sh_account_t *acc = sharedaemon_account();
  return (load_account_tx(acc, id_hash, NULL));
}



