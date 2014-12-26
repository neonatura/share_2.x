

/*
 * @copyright
 *
 *  Copyright 2013 Neo Natura
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


tx_account_t *generate_account(shkey_t *app_key, char *username, shkey_t *pass_key)
{
  tx_account_t *l_acc;
	tx_account_t *acc;
  shkey_t *name_key;
  int err;

	acc = (tx_account_t *)calloc(1, sizeof(tx_account_t));
	if (!acc)
		return (NULL);

  if (app_key)
    memcpy(&acc->acc_app, app_key, sizeof(shkey_t)); 

  if (username)
    strncpy(acc->acc_label, username, sizeof(acc->acc_label) - 1);

  if (pass_key)
    memcpy(&acc->acc_key, pass_key, sizeof(shkey_t));

  name_key = shkey_bin((char *)acc, sizeof(tx_account_t));
  memcpy(&acc->acc_name, name_key, sizeof(shkey_t));
  shkey_free(&name_key);

  l_acc = (tx_account_t *)pstore_load(TX_ACCOUNT,
      (char *)shkey_hex(&acc->acc_name));
  if (l_acc && 0 == strcmp(acc->acc_label, l_acc->acc_label)) {
    /* verify pre-existing account */
    err = confirm_account(l_acc);
    if (!err) {
      free(acc);
      return (l_acc);
    }

    pstore_free(l_acc);
  } 

  /* generate a permanent transaction reference */
  generate_transaction_id(TX_ACCOUNT, &acc->acc_tx, NULL); 

  /* validate new account */
  err = confirm_account(acc);
  if (err) {
    free(acc);
    return (NULL);
  }

  pstore_save(acc, sizeof(tx_account_t));

	return (acc);
}

tx_account_t *generate_account_str(shkey_t *app_key, char *username, char *password)
{
  return (generate_account(app_key, username, ashkey_str(password)));
}

tx_account_t *sharedaemon_account(void)
{
  tx_account_t *ret_account;

  ret_account = generate_account(shpeer_kpub(server_peer), NULL, NULL);

//  shpref_set("account_hash", ret_account->acc_tx.hash);
	return (ret_account);
}

tx_t *load_def_account_tx(char *id_hash)
{
  tx_account_t *acc = sharedaemon_account();
  return (acc);
}

int confirm_account(tx_account_t *acc)
{
  int err;

fprintf(stderr, "DEBUG: confirm_account: SCHED-TX: %s\n", acc->acc_tx.hash);
  generate_transaction_id(TX_ACCOUNT, &acc->tx, NULL);
  sched_tx(acc, sizeof(tx_account_t));

  return (0);
}

void free_account(tx_account_t **acc_p)
{
	if (acc_p) {
		free(*acc_p);
		*acc_p = NULL;
	}
}

int process_account_tx(tx_account_t *acc)
{
  tx_account_t *ent;
  int err;

  ent = (tx_account_t *)pstore_load(TX_ACCOUNT, acc->acc_tx.hash);
  if (!ent) {
    err = confirm_account(acc);
    if (err)
      return (err);

    pstore_save(acc, sizeof(tx_account_t));
  }

  return (0);
}
