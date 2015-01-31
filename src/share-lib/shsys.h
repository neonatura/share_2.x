
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

#ifdef __cplusplus
extern "C" {
#endif


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

#define SHPAM_DELETE (1 << 0)
#define SHPAM_EXPIRE (1 << 1)
#define SHPAM_LOCK (1 << 2)
#define SHPAM_STATUS (1 << 3)
#define SHPAM_SESSION (1 << 4)
#define SHPAM_UNLOCK (1 << 5)
#define SHPAM_UPDATE (1 << 6)

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


struct shseed_t 
{
  /* account password */
  shkey_t seed_key;
  /* account signature */
  shkey_t seed_sig;
  /* salt generation time-stamp */
  shtime_t seed_stamp;
  /* salt used to generate password */
  uint64_t seed_salt;
  /* a reference to the account name. */
  uint64_t seed_uid;
  /* the encryption method */
  uint32_t seed_type;
  uint32_t _reserved_;
};
typedef struct shseed_t shseed_t;

#define SHSEED_PLAIN 0
#define SHSEED_MD5 1
#define SHSEED_SHA256 3
#define SHSEED_SHA512 2


const char *shpam_sys_username(void);
shkey_t *shpam_sys_pass(char *acc_name);

shkey_t *shpam_seed(char *username, char *passphrase, uint64_t salt);

int shpam_seed_verify(shkey_t *seed_key, char *acc_name, char *passphrase);

shkey_t *shpam_ident_gen(shpeer_t *peer, shkey_t *seed, char *label);

shkey_t *shpam_ident_sys(shpeer_t *peer);

int shpam_ident_verify(shkey_t *id_key, shpeer_t *peer, shkey_t *seed, char *label);

shkey_t *shpam_sess_gen(shkey_t *seed_key, shtime_t stamp, shkey_t *id_key);

int shpam_sess_verify(shkey_t *sess_key, shkey_t *seed_key, shtime_t stamp, shkey_t *id_key);


shfs_ino_t *shpam_shadow_file(shfs_t *fs);

shadow_t *shpam_shadow(shfs_ino_t *file, shkey_t *seed_key);

int shpam_shadow_verify(shfs_ino_t *file, shkey_t *seed_key);

int shpam_shadow_login(shfs_ino_t *file, char *acc_name, char *acc_pass, shkey_t **sess_key_p);

int shpam_shadow_delete(shfs_ino_t *file, shkey_t *seed_key, shkey_t *sess_key);

int shpam_shadow_session_set(shfs_ino_t *file, shkey_t *seed_key, shkey_t *id_key, shkey_t *sess_key, shtime_t sess_stamp);

int shpam_shadow_session(shfs_ino_t *file, shkey_t *seed_key, shkey_t **sess_p, shtime_t *expire_p);

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
 * Strips the absolute parent from @a app_name
 * @note "/test/one/two" becomes "two"
 * @param app_name The running application's executable path
 * @returns Relative filename of executable.
 */
char *shapp_name(char *app_name);

/**
 * Initialize the share library runtime for an application.
 * @param exec_path The process's executable path.
 * @param host The host that the app runs on or NULL for localhost.
 * @param flags application flags
 */
shpeer_t *shapp_init(char *exec_path, char *host, int flags);

int shapp_register(shpeer_t *peer);

int shapp_listen(int tx, shpeer_t *peer);

int shapp_account(const char *username, const char *passphrase, shkey_t **pass_key_p);

int shapp_ident(shkey_t *id_seed, char *id_label, shkey_t **id_key_p);

int shapp_session(shkey_t *seed_key, shkey_t **sess_key_p);

int shapp_account_create(char *acc_name, char *acc_pass, char *id_label);

int shapp_account_login(char *acc_name, char *acc_pass, shkey_t **sess_key_p);

int shapp_account_setpass(char *acc_name, char *opass, char *pass);

int shapp_account_remove(char *acc_name, char *acc_pass);

shadow_t *shapp_account_info(shkey_t *seed_key);

/**
 * @}
 */


/**
 * Perform posix crypt algorythm for md5, sha256, and sha512.
 */
char *shcrypt(const char *passwd, const char *salt);



/**
 * @}
 */

#endif /* ndef __MEM__SHSYS_H__ */


#ifdef __cplusplus
}
#endif

