


/*
 * @copyright
 *
 *  Copyright 2014 Brian Burrell 
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
 *  @file signature.h
 */

#ifndef __BITS__SIGNATURE_H__
#define __BITS__SIGNATURE_H__



int verify_signature(shkey_t *sig_key, char *tx_hash, shkey_t *peer_key, shtime_t sig_stamp);

int confirm_signature(shsig_t *sig, char *tx_hash);


/**
 * Generate a new signature based off an identity peer and transaction.
 */
void generate_signature(shsig_t *sig, shpeer_t *peer, tx_t *tx);


#if 0
int process_signature_tx(tx_app_t *cli, tx_sig_t *sig);
#endif






#endif /* ndef __BITS__SIGNATURE_H__ */

