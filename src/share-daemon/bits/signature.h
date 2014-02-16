


/*
 * @copyright
 *
 *  Copyright 2014 Brian Burrell 
 *
 *  This file is part of the Share Library.
 *  (https://github.com/briburrell/share)
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



/**
 * Generate a new signature based off an identity, peer, and transaction.
 */
void generate_signature(sh_sig_t *sig, shpeer_t *peer, sh_tx_t *tx, sh_id_t *id);

/**
 * Search for the last known signature with the given hash digest.
 * @returns An allocated transaction signature or NULL.
 */
sh_sig_t *find_signature(char *tx_hash);

sh_sig_t *find_transaction_signature(sh_tx_t *tx);

#endif /* ndef __BITS__SIGNATURE_H__ */

