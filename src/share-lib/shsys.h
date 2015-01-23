
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

struct shadow_t 
{
  char sh_label[MAX_SHARE_NAME_LENGTH];
  shkey_t sh_seed;
  shkey_t sh_sess; 
  shkey_t sh_id;
  shtime_t sh_expire;
  uint32_t sh_flag;
};
typedef struct shadow_t shadow_t;


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
 * Application state management.
 * @ingroup libshare_sys
 * @defgroup libshare_sysapp
 * @{
 */

#define SHAPP_LOCAL (1 << 0)
#define SHAPP_LOCK (1 << 1)







/**
 * Initialize the share library runtime for an application.
 * @param exec_path The process's executable path.
 * @param host The host that the app runs on or NULL for localhost.
 * @param flags application flags
 */
shpeer_t *shapp_init(char *exec_path, char *host, int flags);

/**
 * Request a peer transaction operation.
 */
int shapp_register(shpeer_t *peer);

int shapp_listen(int tx, shpeer_t *peer);

int shapp_account(const char *username, const char *passphrase, shkey_t **user_key_p, shkey_t **pass_key_p);

int shapp_ident(shkey_t *id_seed, char *id_label, char *id_hash, shkey_t **id_key_p);

/**
 * Strips the absolute parent from @a app_name
 * @note "/test/one/two" becomes "two"
 * @param app_name The running application's executable path
 * @returns Relative filename of executable.
 */
char *shapp_name(char *app_name);

/**
 * @}
 */





/**
 * @}
 */

#endif /* ndef __MEM__SHSYS_H__ */

