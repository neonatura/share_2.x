

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
 *
 *  @file account.h
 */

#ifndef __BITS__ACCOUNT_H__
#define __BITS__ACCOUNT_H__


/**
 * Fills in the contents of an account's identity by providing the hash address.
 * @returns A share error code (SHERR_XXX).
 */
int find_account_identity(tx_account_t *acc, tx_id_t *id);

tx_t *load_account_tx(tx_account_t *acc, char *id_hash, size_t *tx_len_p);
tx_account_t *load_account(const char *hash);
int load_def_account_identity(tx_id_t *id);

tx_account_t *generate_account(shkey_t *peer_key);
tx_account_t *sharedaemon_account_load(void);

/**
 * The default account associated with this server.
 */
tx_account_t *sharedaemon_account(void);

void propose_account(tx_account_t *acc);


int confirm_account(tx_account_t *acc);

#endif /* ndef __BITS__ACCOUNT_H__ */

