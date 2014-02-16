

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


/**
 * Generate a new wallet using a seed value.
 */
static void generate_account_id(sh_account_t *acc)
{
  generate_identity_id(&acc->id);
  strcpy(acc->hash, shdigest(&acc->id, sizeof(acc->id)));

}

sh_account_t *generate_account(void)
{
	struct sh_account_t *acc;

	acc = (sh_account_t *)calloc(1, sizeof(sh_account_t));
	if (!acc)
		return (NULL);

	generate_account_id(acc);
	save_account(acc);

	return (acc);
}

int save_account(sh_account_t *acc)
{
	shfs_t *fs = sharedaemon_fs();
	shfs_ino_t *fl;
	char path[PATH_MAX+1];
	int err;

  sprintf(path, "/shnet/account/%s", acc->hash);
  fl = shfs_file_find(fs, path);
  err = shfs_file_write(fl, (char *)acc, sizeof(sh_account_t));
	if (err)
		return (err);
fprintf(stderr, "DEBUG: info: save_account: saved hash '%s' <%d bytes>\n", acc->hash, sizeof(sh_account_t));

	return (0);
}

sh_account_t *sharedaemon_account_load(void)
{
	char *hash = shpref_get("account_hash", "");
fprintf(stderr, "DEBUG: sharedaemon_account_load: account_hash '%s'\n", hash);
	if (!hash || !*hash)
		return (NULL);

#if 0 
  shpeer_t *peer = sharedaemon_peer();
shkey_print(&peer->name)));
#endif

	return (load_account(hash));
}

sh_account_t *load_account(const char *hash)
{
  shfs_t *fs = sharedaemon_fs();
  shfs_ino_t *fl;
  char path[PATH_MAX+1];
  char *data;
  size_t data_len;
  int err;

  sprintf(path, "/shnet/account/%s", hash);
  fl = shfs_file_find(fs, path);
	err = shfs_file_read(fl, &data, &data_len);
	if (err || data_len < sizeof(sh_account_t)) {
		/* does not exist */
fprintf(stderr, "DEBUG: load_account: error loading '%d'.\n", hash);
		return (NULL);
	}

fprintf(stderr, "DEBUG: load_account: <%d bytes>\n", data_len);
  if (data_len != sizeof(sh_account_t)) {
		PRINT_ERROR(SHERR_IO, "load_account (invalid offset)");
    return (NULL);
	}

  return ((sh_account_t *)data);
}

sh_account_t *sharedaemon_account(void)
{
	static sh_account_t *ret_account;
	if (!ret_account) {
		ret_account = sharedaemon_account_load();
		if (ret_account && ret_account->confirm <= 1) {
			fprintf(stderr, "DEBUG: account '%s' has x%d confirms\n", ret_account->hash, ret_account->confirm);
			confirm_account(ret_account);
		}
	}
	if (!ret_account) {
		ret_account = generate_account();
		shpref_set("account_hash", ret_account->hash);
		fprintf(stderr, "DEBUG: shpref_set('account_hash', '%s')\n", ret_account->hash);
		propose_account(ret_account);
	}
	return (ret_account);
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



void propose_account(sh_account_t *acc)
{

	if (!acc)
		return;

	generate_transaction_id(&acc->tx);
  /* ship 'er off */
  sched_tx(acc, sizeof(sh_account_t));
}

void confirm_account(sh_account_t *acc)
{

	acc->confirm++; /* validate ourselves */

  /* ship 'er off */
  sched_tx(acc, sizeof(sh_account_t));

}

void free_account(sh_account_t **acc_p)
{
	if (acc_p) {
		free(*acc_p);
		*acc_p = NULL;
	}
}


