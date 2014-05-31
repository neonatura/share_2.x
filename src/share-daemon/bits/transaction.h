

/*
 * @copyright
 *
 *  Copyright 2013 Brian Burrell 
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
 *  @file transaction.h
 */

#ifndef __BITS__TRANSACTION_H__
#define __BITS__TRANSACTION_H__



/**
*
*/
int generate_transaction_id(sh_tx_t *tx);

/**
 * Determines if the local node has access to process the transaction based on the originating entity.
 * @param id The identity associated with the transaction.
 * @param tx The transaction to process.
 * @returns TRUE if transaction is accessible or FALSE if prohibited.
 */
int has_tx_access(sh_id_t *id, sh_tx_t *tx);

#endif /* ndef __BITS__TRANSACTION_H__ */
