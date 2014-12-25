

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
 *  @file identity.h
 */

#ifndef __BITS__IDENTITY_H__
#define __BITS__IDENTITY_H__


/** The time-stamp of when the identity was generated. */
#define identity_stamp(_id) \
  ((id)->id_sig.sig_stamp)



int process_identity_tx(tx_app_t *cli, tx_id_t *id);


int generate_identity_tx(tx_id_t *id, tx_account_t *acc, shpeer_t *app_peer, char *acc_name, char *acc_hash);

tx_id_t *generate_identity(tx_account_t *acc, shpeer_t *app_peer, char *acc_user, char *acc_hash);


#endif /* ndef __BITS__IDENTITY_H__ */

