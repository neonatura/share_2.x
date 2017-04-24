
/*
 *  Copyright 2013 Neo Natura
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
*/  

#include "share.h"


const char *shuser_self(void)
{
  return ((const char *)shpam_username_sys());
}

uint64_t shuser_id(char *acc_name)
{
  return (shpam_uid(acc_name));
}

uint64_t shuser_self_id(void)
{
  return (shuser_id((char *)shuser_self()));
}

int shuser_inform(uint64_t uid)
{
  static int mode = TX_ACCOUNT;
  shbuf_t *buff;
  int qid;
  int err;

  /* notify server */
  buff = shbuf_init();
  shbuf_cat(buff, &mode, sizeof(uint32_t));
  shbuf_cat(buff, &uid, sizeof(uid));
  qid = shmsgget(NULL);
  err = shmsg_write(qid, buff, NULL);
  shbuf_free(&buff);
  if (err)
    return (err);

  return (0);
}

int shuser_create_priv(char *acc_name, shpriv_t *priv, shpriv_t **priv_p)
{
  shfs_t *fs;
  shfs_ino_t *shadow_file;
  shbuf_t *buff;
  shkey_t *user_key;
  uint32_t mode;
  uint64_t salt;
  int qid;
  int err;

  fs = NULL;
  shadow_file = shpam_shadow_file(&fs);

  err = shpam_shadow_create(shadow_file, acc_name, priv, priv_p);
  shfs_free(&fs);
  if (err)
    return (err);

  shuser_inform(shpam_uid(acc_name));

  return (0);
}

int shuser_create(char *acc_name, shpriv_t **priv_p)
{
  return (shuser_create_priv(acc_name, NULL, priv_p));
}

int shuser_login_2fa(char *acc_name, char *passphrase, uint32_t code_2fa, shpriv_t **priv_p)
{
  shfs_ino_t *shadow_file;
  shfs_t *fs;
  int err;

  if (!acc_name)
    return (SHERR_INVAL);

  if (!passphrase)
    passphrase = "";

  fs = NULL;
  shadow_file = shpam_shadow_file(&fs);
  if (!shadow_file)
    return (SHERR_IO);

  err = shpam_shadow_login(shadow_file, acc_name, code_2fa, (unsigned char *)passphrase, strlen(passphrase), priv_p);
  shfs_free(&fs);
  if (err)
    return (err);

  return (0);
}

int shuser_login(char *acc_name, char *passphrase, shpriv_t **priv_p)
{
  return (shuser_login_2fa(acc_name, passphrase, 0, priv_p));
}

int shuser_pass_set(char *acc_name, shpriv_t *priv, char *passphrase)
{
  shfs_t *fs;
  shfs_ino_t *shadow_file;
  uint64_t uid;
  int err;

  uid = shpam_uid(acc_name);

  fs = NULL;
  shadow_file = shpam_shadow_file(&fs);
  if (!shadow_file)
    return (SHERR_IO);

  err = shpam_shadow_pass_set(shadow_file, acc_name, priv,
      (unsigned char *)passphrase, strlen(passphrase));
  shfs_free(&fs);
  if (err)
    return (err);

  shuser_inform(uid);

  return (0);
}

int shuser_remove(char *acc_name, shpriv_t *priv)
{
  shfs_ino_t *shadow_file;
  shfs_t *fs;
  uint64_t uid;
  int err;

  fs = NULL;
  shadow_file = shpam_shadow_file(&fs);
  if (!shadow_file)
    return (SHERR_IO);

  uid = shpam_uid(acc_name);
  err = shpam_shadow_remove(shadow_file, uid, priv);
  shfs_free(&fs);
  if (err)
    return (err);

//  shuser_inform(shpam_uid(acc_name));

  return (0);
}


int shuser_info(char *acc_name, int cmd, unsigned char *ret_data, size_t *ret_len_p)
{
  shfs_t *fs;
  shfs_ino_t *shadow_file;
  uint64_t uid;
  int err;

  fs = NULL;
  shadow_file = shpam_shadow_file(&fs);
  if (!shadow_file)
    return (SHERR_IO);

  uid = shpam_uid(acc_name);
  err = shpam_shadow_get(shadow_file, uid, cmd, ret_data, ret_len_p);
  shfs_free(&fs);
  if (err)
    return (err);

  return (0);
}

int shuser_info_set(char *acc_name, shpriv_t *priv, int cmd, unsigned char *data, size_t data_len)
{
  shfs_t *fs;
  shfs_ino_t *shadow_file;
  uint64_t uid;
  int err;

  fs = NULL;
  shadow_file = shpam_shadow_file(&fs);
  if (!shadow_file)
    return (SHERR_IO);

  uid = shpam_uid(acc_name);
  err = shpam_shadow_set(shadow_file, uid, priv, cmd, data, data_len); 
  shfs_free(&fs);
  if (err)
    return (err);
  
  return (0);
}

shjson_t *shuser_json(char *acc_name)
{
  shfs_t *fs;
  shfs_ino_t *shadow_file;
  shjson_t *ret_json;
  uint64_t uid;
  int err;

  fs = NULL;
  shadow_file = shpam_shadow_file(&fs);
  if (!shadow_file)
    return (NULL);

  uid = shpam_uid(acc_name);
  ret_json = shpam_shadow_json(shadow_file, uid);
  shfs_free(&fs);

  return (ret_json);
}

int shuser_verify(char *acc_name)
{
  shfs_t *fs;
  shfs_ino_t *shadow_file;
  uint64_t uid;
  int err;

  fs = NULL;
  shadow_file = shpam_shadow_file(&fs);
  if (!shadow_file)
    return (SHERR_IO);

  uid = shpam_uid(acc_name);
  err = shpam_shadow_uid_verify(shadow_file, uid);
  shfs_free(&fs);
  if (err)
    return (err);

  return (0);
}

int shuser_admin_default(shpriv_t **priv_p)
{
  shfs_t *fs;
  shfs_ino_t *shadow_file;
  shpriv_t *priv;
  int err;

  if (priv_p)
    *priv_p = NULL;

  fs = NULL;
  shadow_file = shpam_shadow_file(&fs);
  if (!shadow_file)
    return (SHERR_IO);

  priv = shpam_shadow_admin_default(shadow_file);
  shfs_free(&fs);
  if (!priv)
    return (SHERR_ACCESS);

  if (priv_p) {
    *priv_p = priv;
  } else {
    free(priv);
  }

  return (0);
}

_TEST(shuser_login)
{
  shpriv_t *priv;
  shpriv_t *test_priv;
  int err;

  /* get admin account in order to create test account. */
  err = shuser_admin_default(&priv);
  _TRUE(err == 0);

  /* create test account */
  err = shuser_create_priv("test", priv, &test_priv);
  _TRUE(err == 0);

  /* set password on behalf of user */
  err = shuser_pass_set("test", test_priv, "test");
  _TRUE(err == 0);

  /* set password on behalf of admin */
  err = shuser_pass_set("test", priv, "test");
  _TRUE(err == 0);

  err = shuser_login("test", "test", NULL);
  _TRUE(err == 0);

  err = shuser_login("test", "wrong test", NULL);
  _TRUE(err == SHERR_ACCESS);

  err = shuser_remove("test", priv);
  _TRUE(err == 0);

  free(test_priv);
  free(priv);

  return (0);

}


