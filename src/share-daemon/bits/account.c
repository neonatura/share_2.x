

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


tx_account_t *sharedaemon_account(void)
{
  tx_account_t *ret_account;
  shseed_t seed;

  memset(&seed, 0, sizeof(shseed_t));
  ret_account = alloc_account(&seed);

	return (ret_account);
}

int inittx_account(tx_account_t *acc, shseed_t *seed)
{
  int err;

  if (!seed)
    return (SHERR_INVAL);

#if 0
  if (seed->seed_uid == 0)
    return (SHERR_INVAL);

  acc = (tx_account_t *)pstore_load(TX_ACCOUNT, shcrcstr(seed->seed_uid));
  if (acc) {
    PRINT_ERROR(SHERR_EXIST, "inittx_account [generation error]");
		return (NULL);
  }
#endif

  memcpy(&acc->pam_sig, &seed->seed_sig, sizeof(acc->pam_sig)); /* doh */

  memcpy(&acc->pam_seed, seed, sizeof(shseed_t));

  err = tx_init(NULL, (tx_t *)acc, TX_ACCOUNT);
  if (err) { 
    PRINT_ERROR(err, "inittx_account [initialization error]");
    return (err);
  }

  return (0);
}

tx_account_t *alloc_account(shseed_t *seed)
{
	tx_account_t *acc;
  int err;

  acc = (tx_account_t *)calloc(1, sizeof(tx_account_t));
  if (!acc)
    return (NULL);

  err = inittx_account(acc, seed);
  if (err) {
    PRINT_ERROR(err, "alloc_account [initialization]");
    free(acc);
    return (NULL);
  }

  return (acc);
}

tx_account_t *alloc_account_user(char *username, char *passphrase, uint64_t salt)
{
  tx_account_t *acc;

  acc = alloc_account(shpam_pass_gen(username, passphrase, salt));
  if (!acc)
    return (NULL);

  return (acc);
}




int txop_account_init(shpeer_t *cli_peer, tx_account_t *acc)
{
  return (0);
}

int txop_account_confirm(shpeer_t *cli_peer, tx_account_t *acc)
{
  shfs_t *fs;
  SHFL *shadow_file;
  shseed_t seed;
  int err;

  /* verify proposed account signature integrity */
  err = shkey_verify(&acc->pam_seed.seed_sig, acc->pam_seed.seed_salt,
      &acc->pam_seed.seed_key, acc->pam_seed.seed_stamp);
  if (err)
    return (err);

  fs = NULL;
  shadow_file = shpam_shadow_file(&fs);
  err = shpam_pshadow_load(shadow_file, acc->pam_seed.seed_uid, &seed);
  shfs_free(&fs);
  if (err)
    return (0); /* cannot access account */


  /* verify integrity if account already exists */
  if (!shkey_cmp(&seed.seed_sig, &acc->pam_seed.seed_sig) &&
      !shkey_cmp(&seed.seed_sig, &acc->pam_sig))
    return (SHERR_ACCESS); /* not current or previous signature */

  if (acc->pam_seed.seed_stamp != seed.seed_stamp ||
      acc->pam_seed.seed_salt != seed.seed_salt) {
    /* all references to same account on sharenet use same salt */
    return (SHERR_INVAL);
  }

  return (0);
}

int txop_account_send(shpeer_t *cli_peer, tx_account_t *acc)
{

  if (!cli_peer) {
    /* broadcast not allowed for TX_ACCOUNT. */
    return (SHERR_OPNOTSUPP);
  }

  return (0);
}

int txop_account_recv(shpeer_t *cli_peer, tx_account_t *acc)
{
  shfs_t *fs;
  SHFL *shadow_file;
  int err;

  if (!acc)
    return (SHERR_INVAL);

  fs = NULL;
  shadow_file = shpam_shadow_file(&fs);

  /* verify uid (TX_IDENT) already exists. */
  err = shpam_shadow_load(shadow_file, acc->pam_seed.seed_uid, NULL);
  if (err) {
    shfs_free(&fs);
    return (err);
  }

  /* attempt to add to local pam db */
  err = shpam_pshadow_store(shadow_file, &acc->pam_seed);
  shfs_free(&fs);
  if (err)
    return (err);

  return (0);
}

