
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




shkey_t *shpam_user_gen(char *username)
{
  return (shkey_str(username));
}

/**
 * A key reference to an account pass-phrase.
*/
shkey_t *shpam_seed_gen(shkey_t *user_key, char *passphrase)
{
  char pass_buf[256];
  size_t len;
  int err;

  len = MAX_SHARE_NAME_LENGTH - 16;
  memset(pass_buf, 0, sizeof(pass_buf));
  if (passphrase) 
    strncpy(pass_buf, passphrase, len);
  ashencode(pass_buf, &len, user_key);

  return (shkey_bin(pass_buf, len));
}

int shpam_seed_verify(shkey_t *seed_key, shkey_t *user_key, char *passphrase)
{
  shkey_t *ver_seed_key;
  int err;

  ver_seed_key = shpam_seed_gen(user_key, passphrase);
  err = !shkey_cmp(ver_seed_key, seed_key);
  shkey_free(&ver_seed_key);
  if (err)
    return (SHERR_ACCESS);

  return (0);
}

_TEST(shpam_seed_verify)
{
  char passphrase[256];
  shkey_t *user_key;
  shkey_t *seed_key;

  user_key = shpam_user_gen("test");
fprintf(stderr, "DEBUG: shpam_seed_verify: user 'test' -> '%s'\n", shkey_print(user_key));
  seed_key = shpam_seed_gen(user_key, "test");
fprintf(stderr, "DEBUG: shpam_seed_verify: seed 'test' -> '%s'\n", shkey_print(seed_key));

  _TRUE(0 == shpam_seed_verify(seed_key, user_key, "test"));
shkey_free(&user_key);
shkey_free(&seed_key);

}

shkey_t *shpam_ident_gen(shpeer_t *peer, shkey_t *seed, char *label)
{
  shbuf_t *buff;
  shkey_t *key;

  if (!peer)
    return (NULL);

  buff = shbuf_init();
  shbuf_cat(buff, peer, sizeof(shpeer_t));
  if (seed)
    shbuf_cat(buff, seed, sizeof(shkey_t));
  if (label)
    shbuf_cat(buff, label, strlen(label));
  key = shkey_bin(shbuf_data(buff), shbuf_size(buff));
  shbuf_free(&buff);

  return (key);
}

int shpam_ident_verify(shkey_t *id_key, shpeer_t *peer, shkey_t *seed, char *label)
{
  shkey_t *ver_key;

  ver_key = shpam_ident_gen(peer, seed, label);
  if (!shkey_cmp(ver_key, id_key))
    return (SHERR_ACCESS);

  return (0);
}

shkey_t *shpam_sess_gen(shkey_t *seed_key, shtime_t stamp, uint64_t crc)
{
  shtime_t max_stamp;
  shtime_t now;
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

  return (shkey_cert(seed_key, crc, stamp));
}

int shpam_sess_verify(shkey_t *sess_key, shkey_t *seed_key, shtime_t stamp, uint64_t crc)
{
  shkey_t *ver_sess_key;
  int err;

  ver_sess_key = shpam_sess_gen(seed_key, stamp, crc);
  err = !shkey_cmp(ver_sess_key, sess_key);
  shkey_free(&ver_sess_key);
  if (err)
    return (SHERR_ACCESS);

  return (0);
}

#undef __MEM__SHSYS_PAM_C__
