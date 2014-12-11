

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
static void generate_account_id(tx_account_t *acc)
{

  memset(&acc->acc_id, 0, sizeof(acc->acc_id));
  generate_identity_id(&acc->acc_id, NULL);
  //strcpy(acc->hash, shdigest(&acc->id, sizeof(acc->id)));

}

tx_account_t *generate_account(shkey_t *peer_key)
{
	struct tx_account_t *acc;

	acc = (tx_account_t *)calloc(1, sizeof(tx_account_t));
	if (!acc)
		return (NULL);

	generate_account_id(acc);
	save_account(acc);

	return (acc);
}

int save_account(tx_account_t *acc)
{
#if 0
	shfs_t *fs = sharedaemon_fs();
	shfs_ino_t *fl;
	char path[PATH_MAX+1];
	int err;

  sprintf(path, "/shnet/account/%s", acc->acc_tx.hash);
  fl = shfs_file_find(fs, path);
  err = shfs_file_write(fl, (char *)acc, sizeof(tx_account_t));
	if (err)
		return (err);
//fprintf(stderr, "DEBUG: info: save_account: saved hash '%s' <%d bytes>\n", acc->hash, sizeof(tx_account_t));
#endif

	return (0);
}

tx_account_t *sharedaemon_account_load(void)
{
	char *hash = (char *)shpref_get("account_hash", "");
//fprintf(stderr, "DEBUG: sharedaemon_account_load: account_hash '%s'\n", hash);
	if (!hash || !*hash)
		return (NULL);

#if 0 
  shpeer_t *peer = sharedaemon_peer();
shkey_print(&peer->name)));
#endif

	return (load_account(hash));
}

tx_account_t *load_account(const char *hash)
{
#if 0
  shfs_t *fs = sharedaemon_fs();
  shfs_ino_t *fl;
  char path[PATH_MAX+1];
  unsigned char *data;
  size_t data_len;
  int err;

  sprintf(path, "/shnet/account/%s", hash);
  fl = shfs_file_find(fs, path);
	err = shfs_file_read(fl, &data, &data_len);
	if (err || data_len < sizeof(tx_account_t)) {
		/* does not exist */
//fprintf(stderr, "DEBUG: load_account: error loading '%d'.\n", hash);
		return (NULL);
	}

//fprintf(stderr, "DEBUG: load_account: <%d bytes>\n", data_len);
  if (data_len != sizeof(tx_account_t)) {
		PRINT_ERROR(SHERR_IO, "load_account (invalid offset)");
    return (NULL);
	}

  return ((tx_account_t *)data);
#endif
  return (NULL);
}

tx_account_t *sharedaemon_account(void)
{
	static tx_account_t *ret_account;
	if (!ret_account) {
		ret_account = sharedaemon_account_load();
		if (ret_account && ret_account->acc_confirm <= 1) {
//			fprintf(stderr, "DEBUG: account '%s' has x%d confirms\n", ret_account->hash, ret_account->confirm);
			confirm_account(ret_account);
		}
	}
	if (!ret_account) {
		ret_account = generate_account(&server_peer->name);
		shpref_set("account_hash", ret_account->acc_tx.hash);
		fprintf(stderr, "DEBUG: shpref_set('account_hash', '%s')\n", ret_account->acc_tx.hash);
		propose_account(ret_account);
	}
	return (ret_account);
}




/*
tx_id_t *load_account(tx_account_t *acc, size_t *id_len)
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

  *id_len = data_len / sizeof(tx_id_t);
  return ((tx_id_t *)data);
}
*/


int load_account_identity(tx_account_t *acc, tx_id_t *id)
{
#if 0
  shfs_t *fs = sharedaemon_fs();
  shfs_ino_t *fl;
  char path[PATH_MAX+1];
  unsigned char *data;
  size_t data_len;
  int err;

  sprintf(path, "/shnet/account/%s/identity/%s", acc->acc_tx.hash, id->id_tx.hash);
  fl = shfs_file_find(fs, path);
  err = shfs_file_read(fl, &data, &data_len);
  if (err)
    return (err);

  if (data_len != sizeof(tx_id_t))
    return (SHERR_IO);

  memcpy(id, data, sizeof(tx_id_t));
  free(data);
#endif

  return (0);
}

int load_def_account_identity(tx_id_t *id)
{
  tx_account_t *acc = sharedaemon_account();
  return (load_account_identity(acc, id));
}

int add_account_identity(tx_account_t *acc, tx_id_t *id)
{
#if 0
  shfs_t *fs = sharedaemon_fs();
  shfs_ino_t *fl;
char path[PATH_MAX+1];
  int err;

  sprintf(path, "/shnet/account/%s/identity/%s", 
      acc->acc_tx.hash, id->id_tx.hash);
  fl = shfs_file_find(fs, path);
  err = shfs_file_write(fl, id, sizeof(tx_id_t));
  if (err)
    return (err);
#endif

  return (0);
}

void remove_account_identity(tx_account_t *acc, tx_id_t *id)
{

}

tx_t *load_account_tx(tx_account_t *acc, char *id_hash, size_t *tx_len_p)
{
#if 0 
  shfs_t *fs = sharedaemon_fs();
  shfs_ino_t *fl;
  char path[PATH_MAX+1];
  unsigned char *data;
  size_t data_len;
  int err;

  sprintf(path, "/shnet/account/%s/transaction/%s", acc->acc_tx.hash, id_hash);
  fl = shfs_file_find(fs, path);
  if (!fl)
    return (NULL);
  
  err = shfs_file_read(fl, &data, &data_len);
  if (err)
    return (NULL);

  if (tx_len_p)
    *tx_len_p = data_len / sizeof(tx_t);

  return ((tx_t *)data);
#endif
  return (NULL);
}

tx_t *load_def_account_tx(char *id_hash)
{
  tx_account_t *acc = sharedaemon_account();
  return (load_account_tx(acc, id_hash, NULL));
}



void propose_account(tx_account_t *acc)
{

	if (!acc)
		return;

 // acc->tx.tx_op = TX_ACCOUNT;
//	generate_transaction_id(&acc->tx, NULL);
  /* ship 'er off */
  //sched_tx(acc, sizeof(tx_account_t));
}

void confirm_account(tx_account_t *acc)
{

	acc->acc_confirm++; /* validate ourselves */

  /* ship 'er off */
  //sched_tx(acc, sizeof(tx_account_t));

}

void free_account(tx_account_t **acc_p)
{
	if (acc_p) {
		free(*acc_p);
		*acc_p = NULL;
	}
}


