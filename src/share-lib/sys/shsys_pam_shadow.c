
/*
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
*/  

#include "share.h"

#define SHPAM_SHADOW_PATH "/sys/shadow"


shfs_ino_t *shpam_shadow_file(shfs_t *fs)
{
  shfs_ino_t *sys_dir;

  sys_dir = shfs_inode(fs->fsbase_ino, "sys", SHINODE_DIRECTORY);
  shfs_access_owner_set(sys_dir, shpam_ident_sys(&fs->peer));

  return (shfs_inode(sys_dir, "shadow", SHINODE_FILE));
}

shadow_t *shpam_shadow(shfs_ino_t *file, shkey_t *seed_key)
{
  static shadow_t ret_shadow;
  int err;

  memset(&ret_shadow, 0, sizeof(ret_shadow));
  err = shfs_cred_load(file, seed_key, (unsigned char *)&ret_shadow, sizeof(ret_shadow));
  if (err)
   return (NULL);

  if (!shkey_cmp(seed_key, &ret_shadow.sh_seed))
    return (NULL);

  return (&ret_shadow);
}

int shpam_shadow_verify(shfs_ino_t *file, shkey_t *seed_key)
{
  shadow_t shadow;
  int err;

  memset(&shadow, 0, sizeof(shadow));
  err = shfs_cred_load(file, seed_key, (unsigned char *)&shadow, sizeof(shadow));
  if (err)
    return (SHERR_NOKEY);

  if (!shkey_cmp(seed_key, &shadow.sh_seed))
    return (SHERR_ACCESS);

  return (0);
}


#if 0
shadow_t *shpam_shadow_ent(shfs_ino_t *file, char *id_label)
{
  static shadow_t ret_shadow;
  shadow_t *sh_list;
  shbuf_t *buff;
  int sh_list_max;
  int err;
  int i;

  memset(&ret_shadow, 0, sizeof(ret_shadow));

  buff = shbuf_init();
  err = shfs_read(file, buff);
  if (err) {
    shbuf_free(&buff);
    return (NULL);
  }
 
  sh_list_max = shbuf_size(buff) / sizeof(shadow_t);
  sh_list = (shadow_t *)shbuf_data(buff);
  for (i = 0; i < sh_list_max; i++) {
    if (0 == strcasecmp(id_label, sh_list[i].sh_label)) {
      memcpy(&ret_shadow, &sh_list[i], sizeof(shadow_t));
      shbuf_free(&buff);
      return (&ret_shadow);
    }
  }

  shbuf_free(&buff);
  return (NULL);
}
#endif

int shpam_shadow_append(shfs_ino_t *file, shadow_t *shadow)
{
  shadow_t save;
  shadow_t *ent;
  int err;

  if (!shadow)
    return (SHERR_INVAL);

  ent = shpam_shadow(file, &shadow->sh_seed);
  if (!ent)
    return (SHERR_NOKEY);

  if (ent->sh_flag & SHPAM_LOCK)
    return (SHERR_ACCESS);

  memcpy(&save, shadow, sizeof(shadow_t));
  memcpy(&save.sh_id, &ent->sh_id, sizeof(shkey_t));
  err = shfs_cred_store(file, &save.sh_seed,
      (unsigned char *)&save, sizeof(shadow_t));
  if (err)
    return (err);

  return (0);
}

#if 0
int shpam_shadow_append(shfs_ino_t *file, shadow_t *shadow)
{
  shadow_t *sh_list;
  shbuf_t *buff;
  int sh_list_max;
  int idx;
  int err;

  if (!shadow || !*shadow->sh_label)
    return (SHERR_INVAL);

  buff = shbuf_init();
  err = shfs_read(file, buff);
  if (err) {
    shbuf_free(&buff);
    return (err);
  }

  sh_list_max = shbuf_size(buff) / sizeof(shadow_t);
  sh_list = (shadow_t *)shbuf_data(buff);
  for (idx = 0; idx < sh_list_max; idx++) {
    if (shkey_cmp(&shadow->sh_seed, &sh_list[idx].sh_seed))
      break;
  }
  if (idx == sh_list_max) {
    shbuf_free(&buff);
    return (SHERR_NOENT);
  }

  /* requires 'setpass' operation for seed modification */
  if (!shkey_cmp(&sh_list[idx].sh_seed, &shadow->sh_seed)) {
    shbuf_free(&buff);
    return (SHERR_OPNOTSUPP); 
  }

  if (idx == sh_list_max) {
    shbuf_cat(buff, &shadow, sizeof(shadow_t));
  } else {
    memcpy(&sh_list[idx], &shadow, sizeof(shadow_t));
  }

  /* attempt to write - otherwise server will update shadow file */
  err = shfs_write(file, buff);
  shbuf_free(&buff);
  if (err && err != SHERR_ACCESS)
    return (err);

  return (0);
}
#endif

int shpam_shadow_delete(shfs_ino_t *file, shkey_t *seed_key, shkey_t *sess_key)
{
  shadow_t *ent;
  shadow_t save;
  int err;

  if (!sess_key)
    return (SHERR_INVAL);

  ent = shpam_shadow(file, seed_key);
  if (!ent)
    return (SHERR_NOKEY);

  memcpy(&save, ent, sizeof(shadow_t));

  if (shtime64() >= save.sh_expire)
    return (SHERR_KEYEXPIRED);

  if (!shkey_cmp(&save.sh_sess, sess_key))
    return (SHERR_KEYREJECTED);

  if (save.sh_flag & SHPAM_LOCK)
    return (SHERR_ACCESS);

  err = shfs_cred_remove(file, &save.sh_seed);
  if (err)
    return (err);

  return (0);
}

#if 0
int shpam_shadow_delete(shfs_ino_t *file, char *id_label)
{
  shadow_t *sh_list;
  shbuf_t *buff;
  int sh_list_max;
  int idx;
  int err;

  buff = shbuf_init();
  err = shfs_read(file, buff);
  if (err) {
    shbuf_free(&buff);
    return (err);
  }

  sh_list_max = shbuf_size(buff) / sizeof(shadow_t);
  sh_list = (shadow_t *)shbuf_data(buff);
  for (idx = 0; idx < sh_list_max; idx++) {
    if (0 == strcasecmp(id_label, sh_list[idx].sh_label))
      break;
  }
  if (idx == sh_list_max) {
    shbuf_free(&buff);
    return (SHERR_NOENT);
  }

  memset(&sh_list[idx], 0, sizeof(shadow_t));

  /* attempt to write - otherwise server will update shadow file */
  err = shfs_write(file, buff);
  if (err && err != SHERR_ACCESS)
    return (err);

  return (0);
}
#endif

#if 0
int shpam_shadow_lock(shfs_ino_t *file, char *id_label)
{
  shadow_t *ent;

  ent = shpam_shadow_ent(file, id_label);
  if (!ent)
    return (SHERR_NOENT);

  ent->sh_flag |= SHAPP_LOCK;
  return (shpam_shadow_append(file, ent));
}

int shpam_shadow_unlock(shfs_ino_t *file, char *id_label)
{
  shadow_t *ent;

  ent = shpam_shadow_ent(file, id_label);
  if (!ent)
    return (SHERR_NOENT);

  ent->sh_flag &= ~SHAPP_LOCK;
  return (shpam_shadow_append(file, ent));
}
#endif

int shpam_shadow_create(shfs_ino_t *file, shkey_t *seed_key, char *id_label, shadow_t **shadow_p)
{
  static shadow_t shadow;
  shadow_t *sh_list;
  shbuf_t *buff;
  shkey_t *id_key;
  int sh_list_max;
  int idx;
  int err;

  if (!file->tree)
    return (SHERR_INVAL);

  if (shpam_shadow(file, seed_key))
    return (SHERR_NOTUNIQ);

  memset(&shadow, 0, sizeof(shadow));
  memcpy(&shadow.sh_seed, seed_key, sizeof(shkey_t));
  if (id_label)
    strncpy(shadow.sh_label, id_label, sizeof(shadow.sh_label));

  id_key = shpam_ident_gen(&file->tree->peer, seed_key, id_label);
  memcpy(&shadow.sh_id, id_key, sizeof(shkey_t));
  shkey_free(&id_key);

  err = shfs_cred_store(file, seed_key, 
      (unsigned char *)&shadow, sizeof(shadow));
  if (err)
    return (err);

  if (shadow_p)
    *shadow_p = &shadow;

  return (0);
}

int shpam_shadow_setpass(shfs_ino_t *file, shkey_t *oseed_key, shkey_t *seed_key, shkey_t *sess_key)
{
  shadow_t *ent;
  shadow_t save;
  int err;

  ent = shpam_shadow(file, oseed_key);
  if (!ent)
    return (SHERR_ACCESS);

  memcpy(&save, ent, sizeof(save));

  if (!shkey_cmp(&save.sh_sess, sess_key))
    return (SHERR_KEYREJECTED);
  if (shtime64() >= save.sh_expire)
    return (SHERR_KEYEXPIRED);

  /* set new seed key. */
  memcpy(&save.sh_seed, seed_key, sizeof(shkey_t));

  /* expire token */
  save.sh_expire = 0;

  err = shfs_cred_store(file, seed_key, 
      (unsigned char *)&save, sizeof(shadow_t));
  if (err)
    return (err);

#if 0
  /* cred is based on pub key of account. only remove if based on priv key. */
  err = shfs_cred_remove(file, oseed_key); /* skip error state */
fprintf(stderr, "DEBUG: %d = shfs_cred_remove(oseed %s)\n", err, shkey_print(oseed_key));
#endif

  return (0);
}

int shpam_shadow_new(shfs_ino_t *file, char *acc_name, char *acc_pass, char *id_label)
{
  shkey_t *seed_key;
  int err;

  seed_key = shpam_seed(acc_name, acc_pass);  
  err = shpam_shadow_create(file, seed_key, id_label, NULL);
  shkey_free(&seed_key);

  return (err);
}

static shkey_t *_shpam_shadow_session_gen(shkey_t *seed_key, shkey_t *id_key, shtime_t stamp)
{
  /* generate new session */
  shkey_t *sess_key;

  sess_key = shpam_sess_gen(seed_key, stamp, id_key);
  if (!sess_key)
    return (NULL);

  return (sess_key);
}

int shpam_shadow_session(shfs_ino_t *file, shkey_t *seed_key, shkey_t **sess_p, shtime_t *expire_p)
{
  shadow_t *ent;
  shadow_t save;
  shkey_t *sess_key;
  shkey_t *ret_key;
  shtime_t stamp;
  shtime_t now;
  uint64_t crc;
  int err;

  if (!file->tree)
    return (SHERR_INVAL);

  ent = shpam_shadow(file, seed_key);
  if (!ent)
    return (SHERR_ACCESS);

  memcpy(&save, ent, sizeof(shadow_t));

  now = shtime64();
  if (now >= save.sh_expire) {
    stamp = shtime64_adj(now, MAX_SHARE_SESSION_TIME);
    sess_key = _shpam_shadow_session_gen(seed_key, &save.sh_id, stamp); 
    if (!sess_key)
      return (SHERR_KEYREVOKED);

    save.sh_expire = stamp;
    memcpy(&save.sh_sess, sess_key, sizeof(shkey_t));
    err = shpam_shadow_append(file, &save);
    shkey_free(&sess_key);
    if (err)
      return (err);
  }

  if (expire_p)
    *expire_p = save.sh_expire;

  if (sess_p) {
    ret_key = (shkey_t *)calloc(1, sizeof(shkey_t));
    memcpy(ret_key, &save.sh_sess, sizeof(shkey_t));
    *sess_p = ret_key;
  }

  return (0);
}

int shpam_shadow_session_set(shfs_ino_t *file, shkey_t *seed_key, shkey_t *id_key, shkey_t *sess_key, shtime_t sess_stamp)
{
  shadow_t *ent;
  shadow_t save;
  int err;

  ent = shpam_shadow(file, seed_key);
  if (!ent) {
    return (SHERR_ACCESS);
}

  if (shkey_cmp(&ent->sh_sess, sess_key) &&
      ent->sh_expire == sess_stamp)
    return (0); /* done */

  err = shpam_sess_verify(sess_key, seed_key, sess_stamp, id_key);
  if (err)
    return (err);

  memcpy(&save, ent, sizeof(shadow_t));
  memcpy(&save.sh_sess, sess_key, sizeof(shkey_t));
  save.sh_expire = sess_stamp;
  err = shpam_shadow_append(file, &save);
  if (err)
    return (err);

  return (0);
}

int shpam_shadow_session_expire(shfs_ino_t *file, shkey_t *seed_key, shkey_t *sess_key)
{
  shadow_t *ent;
  shadow_t save;
  int err;

  ent = shpam_shadow(file, seed_key);
  if (!ent)
    return (SHERR_ACCESS);

  memcpy(&save, ent, sizeof(shadow_t));

  if (shtime64() >= save.sh_expire)
    return (SHERR_KEYEXPIRED);
  if (!shkey_cmp(&save.sh_sess, sess_key))
    return (SHERR_KEYREJECTED);

  save.sh_expire = 0;
  err = shpam_shadow_append(file, &save);
  if (err)
    return (err);

  return (0);
}

int shpam_shadow_login(shfs_ino_t *file, char *acc_name, char *acc_pass, shkey_t **sess_key_p)
{
  shadow_t *ent;
  shkey_t *seed_key;
  shkey_t *sess_key;
  int err;

  seed_key = shpam_seed(acc_name, acc_pass);  
  if (!seed_key)
    return (SHERR_INVAL);

  ent = shpam_shadow(file, seed_key);
  if (!ent)
    return (SHERR_ACCESS);

  err = shpam_shadow_session(file, seed_key, &sess_key, NULL);
  shkey_free(&seed_key);
  if (err)
    return (err);

  if (sess_key_p)
    *sess_key_p = sess_key;
  else
    shkey_free(&sess_key);

  return (0);
}


_TEST(shpam_shadow_login)
{
  shfs_t *fs;
  shfs_ino_t *file;
  shkey_t *sess_key;
  shkey_t *seed_key;
  shpeer_t *peer;

  peer = shpeer_init("test", NULL);
  fs = shfs_init(peer);
  shpeer_free(&peer);

  file = shfs_file_find(fs, "/shpam_shadow_login");
  _TRUEPTR(file);

  /* test new account generation */
  _TRUE(0 == shpam_shadow_new(file, "test", "test", "test"));

  /* test account validation */
  sess_key = NULL;
  _TRUE(0 == shpam_shadow_login(file, "test", "test", &sess_key));
  _TRUEPTR(sess_key);

  /* test account deletion */
  seed_key = shpam_seed("test", "test");
  _TRUE(0 == shpam_shadow_delete(file, seed_key, sess_key));
  shkey_free(&seed_key);
  shkey_free(&sess_key);

}

