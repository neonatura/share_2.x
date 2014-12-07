


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
 *  @file transaction.h
 */

#ifndef __BITS__LEDGER_H__
#define __BITS__LEDGER_H__




/**
 * @returns Allocated memory that must be free'd.
 * @todo free mem properly on errors
 */
int load_ledger(char *hash, char *type, sh_ledger_t **ledger_p, tx_t **payload_p);

/**
 */
int save_ledger(sh_ledger_t *ledger, tx_t *payload, char *type);

int confirm_ledger(sh_ledger_t *led, tx_t *payload);

void propose_ledger(sh_ledger_t *led, tx_t *payload, size_t size);

void free_ledger(sh_ledger_t **ledger_p, tx_t **tx_p);

int remove_ledger(sh_ledger_t *ledger, char *type);

#endif /* ndef __BITS__LEDGER_H__ */

