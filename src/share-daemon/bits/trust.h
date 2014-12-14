


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
 *  @file trust.h
 */

#ifndef __BITS__TRUST_H__
#define __BITS__TRUST_H__


/**
 * Verify a trust's integrity against it's origin peer.
 */
int confirm_trust(tx_trust_t *trust);


/**
 * Generate a new trust based off a peer, transaction, and optional context.
 */
int generate_trust(tx_trust_t *trust, shpeer_t *peer, shkey_t *context);


int process_trust_tx(tx_app_t *cli, tx_trust_t *trust);

#if 0
/**
 * Search for the last known trust with the given hash digest.
 * @returns An allocated transaction trust or NULL.
 */
sh_trust_t *find_trust(char *tx_hash);
#endif

#endif /* ndef __BITS__TRUST_H__ */

