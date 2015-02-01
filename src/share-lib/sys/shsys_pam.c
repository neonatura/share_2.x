
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

#define __MEM__SHSYS_PAM_C__

#include "share.h"

#ifdef HAVE_GETPWNAM
#include <pwd.h>
#endif
#ifdef HAVE_GETSPNAM
#include <shadow.h>
#endif


const char *shpam_sys_username(void)
{
  static char username[MAX_SHARE_NAME_LENGTH];
  char user_buf[1024];
  char host_buf[MAXHOSTNAMELEN+1];
  const char *str;

  memset(username, 0, sizeof(username));
  str = shpref_get(SHPREF_ACC_NAME, "");
#ifdef HAVE_GETLOGIN_R
  if (!*str) {
    memset(user_buf, 0, sizeof(user_buf));
    memset(host_buf, 0, sizeof(host_buf));
    
    /* "<user>" */
    getlogin_r(user_buf, MAX_SHARE_NAME_LENGTH - 2);
    gethostname(host_buf, sizeof(host_buf)-1);
    if (*host_buf) {
      /* "@" */
      strcat(user_buf, "@");
      /* "<host>" */
      strncat(user_buf, host_buf, MAX_SHARE_NAME_LENGTH - strlen(user_buf) - 1);
    }

    str = user_buf;
  }
#endif
  strncpy(username, str, MAX_SHARE_NAME_LENGTH-1);

  return (username);
}

/**
 * A key reference to an account name and pass-phrase.
 */
shkey_t *shpam_seed(char *username, char *passphrase, uint64_t salt)
{
  shkey_t *user_key;
  shkey_t *seed_key;
  char pass_buf[256];
  size_t len;
  uint64_t crc;

  len = MAX_SHARE_NAME_LENGTH - 16;
  memset(pass_buf, 0, sizeof(pass_buf));
  if (passphrase) 
    strncpy(pass_buf, passphrase, len);

  /* encrypt password */
  user_key = shkey_str(username);
  ashencode(pass_buf, &len, user_key);
  crc = shcrc(user_key, sizeof(user_key));
  shkey_free(&user_key);

  /* generate private (pass) key */
  seed_key = shkey_bin(pass_buf, len);  
  if (!seed_key)
    return (NULL);

  /* encase 'public' key in upper bits */
  memcpy(&seed_key->code[4], &crc, sizeof(uint64_t));

  return (seed_key);
}

shkey_t *shpam_sys_pass(char *username)
{
  shkey_t *ret_key;
  char pass_buf[256];
  char user_buf[256];
  char *str;

  if (!username)
    username = get_libshare_account_name();
  memset(user_buf, 0, sizeof(user_buf));
  strncpy(user_buf, username, sizeof(user_buf) - 1);
  strtok(user_buf, "@");

  memset(pass_buf, 0, sizeof(pass_buf));
  strncpy(pass_buf, shpref_get(SHPREF_ACC_PASS, ""), sizeof(pass_buf) - 1);

#ifdef HAVE_GETPWNAM
  if (!*pass_buf) {
    struct passwd *pw = getpwnam(user_buf);
    if (pw) {
      memset(pass_buf, 0, sizeof(pass_buf));
      strncpy(pass_buf, pw->pw_passwd, sizeof(pass_buf) - 1);
      fprintf(stderr, "DEBUG: found %s's md5 pass '%s'\n", pw->pw_name, pass_buf);
#ifdef HAVE_GETSPNAM
      if (*pw->pw_name) {
        struct spwd *sp = getspnam(pw->pw_name);
        if (sp) {
          memset(pass_buf, 0, sizeof(pass_buf));
          strncpy(pass_buf, sp->sp_pwdp, sizeof(pass_buf) - 1);
          fprintf(stderr, "DEBUG: found %s's shadow pass '%s'\n", pw->pw_name, pass_buf);
        }
      }
#endif
    }
  }
#endif

  /* generate pass seed */
  ret_key = shpam_seed(username, pass_buf, 0);
  return (ret_key);
}


int shpam_seed_verify(shkey_t *seed_key, char *acc_name, char *passphrase)
{
  shkey_t *ver_seed_key;
  int err;

  ver_seed_key = shpam_seed(acc_name, passphrase, 0);
  err = !shkey_cmp(ver_seed_key, seed_key);
  shkey_free(&ver_seed_key);
  if (err)
    return (SHERR_ACCESS);

  return (0);
}

_TEST(shpam_seed_verify)
{
  char passphrase[256];
  shkey_t *seed_key;

  seed_key = shpam_seed("test", "test", 0);
  _TRUE(0 == shpam_seed_verify(seed_key, "test", "test"));
  shkey_free(&seed_key);

}

#if 0
const char *shpam_ident_name(char *acc_label)
{
  static char ret_buf[256];
  int idx;
  int i;
  int f;
  char ch;

  memset(ret_buf, 0, sizeof(ret_buf));
  strncpy(ret_buf, acc_label, sizeof(ret_buf) - 1);

  for (i = 0; i < strlen(ret_buf); i++) {
    idx = i % 9;
    f = (int)tolower(ret_buf[i]) + (int)tolower(ret_buf[idx]);
    ch = (char)(f/2);
    if (isalpha(ch)) {
      if ((i % 2) == 0) {
        ret_buf[idx] = ch;
      } else {
        /* odd - vowel */
        if (ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u')
          ret_buf[idx] = ch;
      }
    }
  }
  ret_buf[8] = '0' + (char)(shcrc(acc_label, strlen(acc_label)) % 10);
  memset(ret_buf + 9, 0, sizeof(ret_buf) - 9);

  return (ret_buf);
}
#endif

shkey_t *shpam_ident_gen(shpeer_t *peer, shkey_t *seed, char *label)
{
  shbuf_t *buff;
  shkey_t *key;
  char hex_buf[MAX_SHARE_NAME_LENGTH];

  if (!peer || !seed)
    return (NULL);

  buff = shbuf_init();
  /* application identifier */
  shbuf_cat(buff, shpeer_kpub(peer), sizeof(shkey_t));
  /* public portion of account seed from 'upper' bits */ 
  shbuf_cat(buff, &seed->code[4], sizeof(uint64_t));
  /* user data */
  if (label)
    shbuf_cat(buff, label, strlen(label));
  key = shkey_bin(shbuf_data(buff), shbuf_size(buff));
  shbuf_free(&buff);

  return (key);
}

shkey_t *shpam_ident_sys(shpeer_t *peer)
{
  shkey_t *seed_key;
  shkey_t *id_key;
  char hostname[256];
  char ubuf[256];
  char pbuf[256];

  memset(hostname, 0, sizeof(hostname));
  gethostname(hostname, sizeof(hostname) - 1);

  sprintf(ubuf, "host:%s", hostname);
  sprintf(pbuf, "%8.8x", SHMEM_MAGIC);
  seed_key = shpam_seed(ubuf, pbuf, 0);
  id_key = shpam_ident_gen(peer, seed_key, NULL);
  shkey_free(&seed_key);

  return (id_key);
}

int shpam_ident_verify(shkey_t *id_key, shpeer_t *peer, shkey_t *seed, char *label)
{
  shkey_t *ver_key;

  ver_key = shpam_ident_gen(peer, seed, label);
  if (!shkey_cmp(ver_key, id_key))
    return (SHERR_ACCESS);

  return (0);
}

shkey_t *shpam_sess_gen(shkey_t *seed_key, shtime_t stamp, shkey_t *id_key)
{
  shtime_t max_stamp;
  shtime_t now;
  shkey_t *key;
  uint64_t crc;
  int err;

  if (!seed_key)
    return (NULL);

  now = shtime64();
  max_stamp = shtime64_adj(now, MAX_SHARE_SESSION_TIME);

  if (!stamp) {
    stamp = max_stamp;
  } else {
    stamp = MAX(now, stamp);
    stamp = MIN(max_stamp, stamp);
  }

  crc = shcrc(id_key, sizeof(shkey_t));
  key = shkey_cert(seed_key, crc, stamp);
  return (key);
}

int shpam_sess_verify(shkey_t *sess_key, shkey_t *seed_key, shtime_t stamp, shkey_t *id_key)
{
  shkey_t *ver_sess_key;
  int valid;

  ver_sess_key = shpam_sess_gen(seed_key, stamp, id_key);
  valid = shkey_cmp(ver_sess_key, sess_key);
  shkey_free(&ver_sess_key);
  if (!valid)
    return (SHERR_ACCESS);

  return (0);
}

/**
 * Generate a random 'salt' number used perturb the account's password key.
 */
uint64_t shpam_salt(void)
{
  shkey_t *key;
  uint64_t crc;

  key = shkey_uniq();
  crc = shcrc(key, sizeof(shkey_t));
  shkey_free(&key);

  return (crc);
}

/**
 * Generate a 'salt' number to perturb the password from a data segment.
 */
uint64_t shpam_salt_gen(unsigned char *data, size_t data_len)
{
  shkey_t *key;
  uint64_t crc;

  key = shkey_bin(data, data_len);
  crc = shcrc(key, sizeof(shkey_t));
  shkey_free(&key);

  return (crc);
}

/**
 * Generate a password key for an account.
 */
shseed_t *shpam_pass_gen(char *username, char *passphrase, uint64_t salt)
{
  static shseed_t ret_seed;
  shkey_t *key;
  char pass_buf[MAX_SHARE_PASS_LENGTH];
  size_t len;

  if (!username)
    username = "";
  if (!passphrase)
    passphrase = "";

  memset(pass_buf, 0, sizeof(pass_buf));

  memset(&ret_seed, 0, sizeof(ret_seed));
  ret_seed.seed_uid = shcrc(username, strlen(username));
  ret_seed.seed_stamp = shtime64();

  /* password salt */
  ret_seed.seed_salt = salt;

  /* crypt password */
  ret_seed.seed_type = SHSEED_PLAIN;
  strncpy(pass_buf, passphrase, MAX_SHARE_PASS_LENGTH - 32);
#ifdef HAVE_CRYPT
  {
    char salt_buf[17];
    char *enc_str;

    memset(salt_buf, 0, sizeof(salt_buf));
    sprintf(salt_buf, "$6$%s", shcrcstr(salt));
    enc_str = crypt(passphrase, salt_buf);
    if (enc_str) {
      memset(pass_buf, 0, sizeof(pass_buf));
      strncpy(pass_buf, enc_str, MAX_SHARE_PASS_LENGTH - 32);
      ret_seed.seed_type = SHSEED_SHA512;
    } else {
      sprintf(salt_buf, "$1$%s", shcrcstr(salt));
      enc_str = crypt(passphrase, salt_buf);
      if (enc_str) {
        memset(pass_buf, 0, sizeof(pass_buf));
        strncpy(pass_buf, enc_str, MAX_SHARE_PASS_LENGTH - 32);
        ret_seed.seed_type = SHSEED_MD5;
      }
    }
  }
#endif

  /* encode password */
  key = shkey_num(salt);
  len = MAX_SHARE_NAME_LENGTH - 32;
  ashencode(pass_buf, &len, key);
  shkey_free(&key);

  /* password key */
  key = shkey_bin(pass_buf, len);
  memcpy(&ret_seed.seed_key, key, sizeof(shkey_t));
  shkey_free(&key);

  /* password signature */
  key = shkey_cert(&ret_seed.seed_key, ret_seed.seed_salt, ret_seed.seed_stamp);
  memcpy(&ret_seed.seed_sig, key, sizeof(shkey_t));
  shkey_free(&key); 

  return (&ret_seed);
}

int shpam_pass_sys(char *username)
{
  static shseed_t ret_seed;
  shkey_t *key;
  size_t len;
  char cr_salt[256];
  char cr_pass[256];

  if (!username)
    username = "";

  memset(cr_salt, 0, sizeof(cr_salt));
  memset(cr_pass, 0, sizeof(cr_pass));

  memset(&ret_seed, 0, sizeof(ret_seed));
  ret_seed.seed_uid = shcrc(username, strlen(username));
  ret_seed.seed_stamp = shtime64();

#ifdef HAVE_GETSPNAM
  {
    struct spwd *sp = getspnam(username);
    if (sp) {
      char *str = sp->sp_pwdp; /* use shadow passwd */
      if (str && 0 == strncmp(str, "$6$", 3)) {
        str += 3;
        int idx = stridx(str, '$');
        if (idx != -1) {
          strncpy(cr_salt, str, MIN(idx, sizeof(cr_salt) - 1));
          strncpy(cr_pass, str + idx + 1, sizeof(cr_pass) - 1);
        }
        ret_seed.seed_type = SHSEED_SHA512;
      } else if (str && 0 == strncmp(str, "$1$", 3)) {
        str += 3;
        int idx = stridx(str, '$');
        if (idx != -1) {
          strncpy(cr_salt, str, MIN(idx, sizeof(cr_salt) - 1));
          strncpy(cr_pass, str + idx + 1, sizeof(cr_pass) - 1);
        }
        ret_seed.seed_type = SHSEED_MD5;
      }
    }
  }
#endif
  ret_seed.seed_salt = shcrcgen(cr_salt);

  /* encode password */
  key = shkey_num(ret_seed.seed_salt);
  len = MAX_SHARE_NAME_LENGTH - 32;
  ashencode(cr_pass, &len, key);
  shkey_free(&key);

  /* password key */
  key = shkey_bin(cr_pass, len);
  memcpy(&ret_seed.seed_key, key, sizeof(shkey_t));
  shkey_free(&key);

  /* password signature */
  key = shkey_cert(&ret_seed.seed_key, ret_seed.seed_salt, ret_seed.seed_stamp);
  memcpy(&ret_seed.seed_sig, key, sizeof(shkey_t));
  shkey_free(&key); 

  return (&ret_seed);
}

int shpam_pass_verify(shseed_t *seed, char *username, char *passphrase, uint64_t salt)
{
  shseed_t *v_seed;
  int err;

  v_seed = shpam_pass_gen(username, passphrase, salt);

  if (seed->seed_uid != v_seed->seed_uid) {
fprintf(stderr, "DEBUG: shpam_pass_verify: fail uid\n");
    return (SHERR_INVAL);
  }
  if (seed->seed_type != v_seed->seed_type) {
fprintf(stderr, "DEBUG: shpam_pass_verify: fail type\n");
    return (SHERR_INVAL);
  }

  if (seed->seed_salt != v_seed->seed_salt) {
fprintf(stderr, "DEBUG: shpam_pass_verify: fail salt\n");
    return (SHERR_INVAL);
  }

  if (!shkey_cmp(&seed->seed_key, &v_seed->seed_key)) {
fprintf(stderr, "DEBUG: shpam_pass_verify: fail key\n");
    return (SHERR_INVAL);
  }

  err = shkey_verify(&seed->seed_sig, 
      seed->seed_salt, &seed->seed_key, seed->seed_stamp); 
  if (err)
    return (err);

  return (0);
}

_TEST(shpam_pass_verify)
{
  shseed_t *raw_seed;
  shseed_t seed;
  uint64_t salt;

  salt = shpam_salt();
  raw_seed = shpam_pass_gen("test", "test", salt);
  memcpy(&seed, raw_seed, sizeof(shseed_t));
  _TRUE(0 == shpam_pass_verify(&seed, "test", "test", salt));

}

#undef __MEM__SHSYS_PAM_C__
