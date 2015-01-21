
/*
 * @copyright
 *
 *  Copyright 2015 Neo Natura 
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

#ifndef __MEM__SHSYS_H__
#define __MEM__SHSYS_H__



/**
 * System-level routines.
 * @ingroup libshare
 * @defgroup libshare_sys
 * @{
 */



/**
 * Permission access management.
 * @ingroup libshare_sys
 * @defgroup libshare_syspam
 * @{
 */

shkey_t *shpam_user_gen(char *username);

char *shpam_seed_sys(char *username);

shkey_t *shpam_seed_gen(shkey_t *user_key, char *passphrase);

shkey_t *shpam_seed_def(char *username);

int shpam_seed_verify(shkey_t *seed_key, shkey_t *user_key, char *passphrase);

shkey_t *shpam_ident_gen(shpeer_t *peer, shkey_t *seed, char *label);

int shpam_ident_verify(shkey_t *id_key, shpeer_t *peer, shkey_t *seed, char *label);

shkey_t *shpam_sess_gen(shkey_t *seed_key, shtime_t stamp, uint64_t crc);

int shpam_sess_verify(shkey_t *sess_key, shkey_t *seed_key, shtime_t stamp, uint64_t crc);

/**
 * @}
 */







/**
 * @}
 */

#endif /* ndef __MEM__SHSYS_H__ */

