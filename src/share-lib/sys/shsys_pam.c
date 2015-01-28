
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


/**
 * A key reference to an account name and pass-phrase.
 * @note passphrase is typically ran through crypt() by this point.
 */
shkey_t *shpam_seed(char *username, char *passphrase)
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

int shpam_seed_verify(shkey_t *seed_key, char *acc_name, char *passphrase)
{
  shkey_t *ver_seed_key;
  int err;

  ver_seed_key = shpam_seed(acc_name, passphrase);
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

  seed_key = shpam_seed("test", "test");
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
  seed_key = shpam_seed(ubuf, pbuf);
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


#undef __MEM__SHSYS_PAM_C__
