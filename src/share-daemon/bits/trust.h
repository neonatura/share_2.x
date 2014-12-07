


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
 * Generate a new trust based off an identity, peer, and transaction.
 */
void generate_trust(sh_trust_t *trust, shpeer_t *peer, tx_t *tx, sh_id_t *id);

/**
 * Search for the last known trust with the given hash digest.
 * @returns An allocated transaction trust or NULL.
 */
sh_trust_t *find_trust(char *tx_hash);

#endif /* ndef __BITS__TRUST_H__ */

