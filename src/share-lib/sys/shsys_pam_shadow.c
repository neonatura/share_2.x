
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


shfs_ino_t *shpam_shadow_file(shfs_t **fs_p)
{
  shfs_ino_t *sys_dir;
  shpeer_t *peer;
  shfs_ino_t *ino;
  shfs_t *fs;

  fs = *fs_p;
  if (!fs) {
    peer = shpeer_init(PACKAGE, NULL);
    fs = shfs_init(peer);
    shpeer_free(&peer);
  }

  sys_dir = shfs_inode(fs->fsbase_ino, "sys", SHINODE_DIRECTORY);
  shfs_access_owner_set(sys_dir, shpam_ident_root(&fs->peer));
// - write attr

  ino = shfs_inode(sys_dir, "shadow", SHINODE_FILE);
  if (!ino)
    return (NULL);

  *fs_p = fs;
  return (ino);
}

int shpam_shadow_create(shfs_ino_t *file, uint64_t uid, shadow_t *ret_shadow)
{
  shadow_t shadow;
  shadow_t *sh_list;
  shbuf_t *buff;
  shkey_t *id_key;
  shkey_t *key;
  int sh_list_max;
  int idx;
  int err;

  if (!file->tree)
    return (SHERR_INVAL);

  err = shpam_shadow_load(file, uid, NULL);
  if (err != SHERR_NOENT)
    return (SHERR_NOTUNIQ);

  memset(&shadow, 0, sizeof(shadow_t));
//  memcpy(&shadow.sh_sess, ashkey_blank(), sizeof(shkey_t));
  shadow.sh_uid = uid; 

  id_key = shpam_ident_gen(uid, &file->tree->peer);
  memcpy(&shadow.sh_id, id_key, sizeof(shadow.sh_id));
  shkey_free(&id_key);

  key = shkey_bin((char *)&uid, sizeof(uid));
  err = shfs_cred_store(file, key, (unsigned char *)&shadow, sizeof(shadow_t));
  shkey_free(&key);
  if (err)
    return (err);

  if (ret_shadow)
    memcpy(ret_shadow, &shadow, sizeof(shadow_t));

  return (0);
}

int shpam_shadow_load(shfs_ino_t *file, uint64_t uid, shadow_t *ret_shadow)
{
  shadow_t shadow;
  shkey_t *key;
  int err;

  memset(&shadow, 0, sizeof(shadow_t));

  key = shkey_bin((char *)&uid, sizeof(uid));
  err = shfs_cred_load(file, key, (unsigned char *)&shadow, sizeof(shadow));
  shkey_free(&key);
  if (err)
    return (err);

  if (ret_shadow)
    memcpy(ret_shadow, &shadow, sizeof(shadow_t));

  return (0);
}

int shpam_shadow_store(shfs_ino_t *file, shadow_t *shadow)
{
  shadow_t save;
  shkey_t *key;
  int err;

  if (!shadow)
    return (SHERR_INVAL);

#if 0
  /* ensure record already exists. */
  err = shpam_shadow_load(file, shadow->sh_uid, NULL);
  if (err)
    return (err);
#endif

  memcpy(&save, shadow, sizeof(shadow_t));
  key = shkey_bin((char *)&save.sh_uid, sizeof(save.sh_uid));
  err = shfs_cred_store(file, key, (unsigned char *)&save, sizeof(shadow_t));
  shkey_free(&key);
  if (err)
    return (err);

  return (0);
}

int shpam_shadow_remove(shfs_ino_t *file, uint64_t uid, shkey_t *sess_key)
{
  shadow_t *ent;
  shadow_t save;
  shkey_t *key;
  int err;

  if (!sess_key)
    return (SHERR_NOKEY);

  err = shpam_shadow_load(file, uid, &save);
  if (err) {
    return (err);
}

  if (shtime_after(shtime(), save.sh_expire))
    return (SHERR_KEYEXPIRED);

  if (!shkey_cmp(&save.sh_sess, sess_key))
    return (SHERR_KEYREJECTED);

  key = shkey_bin((char *)&uid, sizeof(uid));
  err = shfs_cred_remove(file, key);
  shkey_free(&key);
  if (err) {
    return (err);
}

  return (0);
}

int shpam_pshadow_create(shfs_ino_t *file, shseed_t *seed)
{
  int err;

  if (!seed)
    return (SHERR_INVAL);

  err = shpam_pshadow_load(file, seed->seed_uid, NULL); 
  if (err != SHERR_NOENT) {
    return (SHERR_NOTUNIQ);
  }

  err = shpam_pshadow_store(file, seed);
  if (err) {
    return (err);
}

  return (0);
}

int shpam_pshadow_new(shfs_ino_t *file, char *username, char *passphrase)
{
  shseed_t acc_seed;
  shseed_t *seed;
  uint64_t salt;
  int err;

  salt = shpam_salt();
  seed = shpam_pass_gen((char *)username, passphrase, salt);
  memcpy(&acc_seed, seed, sizeof(shseed_t));
  err = shpam_pshadow_create(file, &acc_seed);
  if (err)
    return (err);

  return (0);
}

int shpam_pshadow_load(shfs_ino_t *file, uint64_t uid, shseed_t *ret_seed)
{
  shbuf_t *buff;
  shseed_t *seeds;
  int total;
  int idx;
  int err;

  if (ret_seed)
    memset(ret_seed, 0, sizeof(shseed_t));

  buff = shbuf_init();
  err = shfs_read(file, buff);
  if (err) {
    shbuf_free(&buff);
    return (err);
  }

  seeds = (shseed_t *)shbuf_data(buff);
  total = shbuf_size(buff) / sizeof(shseed_t);
  if (!total) {
    shbuf_free(&buff);
    return (SHERR_NOENT); /* done */
  }

  for (idx = 0; idx < total; idx++) {
    if (seeds[idx].seed_uid == uid)
      break;
  }

  if (idx == total) {
    shbuf_free(&buff);
    return (SHERR_NOENT);
  }

  if (ret_seed)
    memcpy(ret_seed, &seeds[idx], sizeof(shseed_t));

  shbuf_free(&buff);
  return (0);
}

int shpam_pshadow_store(shfs_ino_t *file, shseed_t *seed)
{
  shbuf_t *buff;
  shseed_t *seeds;
  int total;
  int idx;
  int err;

#if 0
  /* ensure record exists. */
  err = shpam_pshadow_load(file, seed->seed_uid, NULL);
  if (err)
    return (err);
#endif

  buff = shbuf_init();
  shfs_read(file, buff);

  seeds = (shseed_t *)shbuf_data(buff);
  total = shbuf_size(buff) / sizeof(shseed_t);
  for (idx = 0; idx < total; idx++) {
    if (seeds[idx].seed_uid == seed->seed_uid) {
      memcpy(&seeds[idx], seed, sizeof(shseed_t));
      break;
    }
  }
  if (idx == total) {
    shbuf_cat(buff, seed, sizeof(shseed_t));
  }

  err = shfs_write(file, buff);
  shbuf_free(&buff);
  if (err)
    return (err);

  return (0);
}

int shpam_pshadow_set(shfs_ino_t *file, shseed_t *seed, shkey_t *sess_key)
{
  shadow_t save;
  int err;

  err = shpam_shadow_load(file, seed->seed_uid, &save);
  if (err)
    return (err);

  /* validate session key */
  if (shtime_after(shtime(), save.sh_expire))
    return (SHERR_KEYEXPIRED);
  if (!shkey_cmp(&save.sh_sess, sess_key))
    return (SHERR_KEYREJECTED);

  err = shpam_shadow_session_expire(file, seed->seed_uid, sess_key);
  if (err)
    return (err);

  err = shpam_pshadow_store(file, seed);
  if (err)
    return (err);

  return (0);
}

int shpam_pshadow_remove(shfs_ino_t *file, uint64_t rem_uid)
{
  shbuf_t *rbuff;
  shbuf_t *buff;
  shseed_t *seeds;
  int total;
  int idx;
  int err;

  /* ensure record exists. */
  err = shpam_pshadow_load(file, rem_uid, NULL);
  if (err)
    return (err);

  rbuff = shbuf_init();
  shfs_read(file, rbuff);

  buff = shbuf_init();
  seeds = (shseed_t *)shbuf_data(rbuff);
  total = shbuf_size(rbuff) / sizeof(shseed_t);
  for (idx = 0; idx < total; idx++) {
    if (seeds[idx].seed_uid == rem_uid)
      continue;

    shbuf_cat(buff, &seeds[idx], sizeof(shseed_t));
  }
  shbuf_free(&rbuff);

  err = shfs_write(file, buff);
  shbuf_free(&buff);
  if (err)
    return (err);
  
  return (0);
}


#if 0
static shfs_t *_shadow_fs;
static shfs_ino_t *_shadow_file;
int shpam_shadow_open(uint64_t uid, shadow_t **shadow_p, shseed_t **seed_p)
{
  shadow_t ret_shadow;

  if (!_shadow_fs) {
    _shadow_fs = shfs_init(NULL);
    _shadow_file = shpam_shadow_file(_shadow_fs);
  }

  if (shadow_p) {
    shadow_t *sh;

    memset(&ret_shadow, 0, sizeof(ret_shadow));
    err = shfs_cred_load(file, seed_key, (unsigned char *)&ret_shadow, sizeof(ret_shadow));
    if (err)
      return (err);

    sh = (shadow_t *)calloc(1, sizeof(shadow_t));
    memcpy(sh, &ret_shadow, sizeof(ret_shadow));
    *shadow_p = sh;
  }

  if (seed_p) {
    shseed_t ret_seed;
    shseed_t *seed;

    err = shpam_pshadow_load(_shadow_file, uid, &ret_seed); 
    if (err)
      return (err);

    seed = (shseed_t *)calloc(1, sizeof(shseed_t));
    memcpy(seed, &ret_seed, sizeof(ret_seed));
    *seed_p = seed;
  }

  return (0);
}

int shpam_shadow_close(uint64_t uid, shadow_t **shadow_p, shseed_t **seed_p)
{
  if (sh_p) {
    shadow_t *shadow = *shadow_p;
    *shadow_p = NULL;
    if (shadow) free(shadow);
  }
  if (seed_p) {
    shseed_t *seed = *seed_p;
    *seed_p = NULL;
    if (seed) free(seed);
  }
  shfs_free(&_shadow_fs);
}
#endif

#if 0
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
#endif

#if 0
const shseed_t *shpam_shadow_pass(shfs_ino_t *file, uint64_t uid)
{
  static shseed_t ret_seed;
  shbuf_t *buff;
  shseed_t *seeds;
  int total;
  int idx;
  int err;

  memset(&ret_seed, 0, sizeof(ret_seed));

  buff = shbuf_init();
  err = shfs_read(file, buff);
  if (err) {
    shbuf_free(&buff);
    return (NULL);
  }

  seeds = (shseed_t *)shbuf_data(buff);
  total = shbuf_size(buff) / sizeof(shseed_t);
  for (idx = 0; idx < total; idx++) {
    if (seeds[idx].seed_uid == uid) {
      memcpy(&ret_seed, &seeds[idx], sizeof(shseed_t));
      break;
    }
  }
  shbuf_free(&buff);

  if (idx != total) {
    return (&ret_seed);
  }

  return (NULL);
}
#endif

#if 0
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
#endif

#if 0
int shpam_shadow_pass_append(shfs_ino_t *file, shseed_t *seed)
{
  shbuf_t *buff;
  shseed_t *seeds;
  int total;
  int idx;
  int err;

  buff = shbuf_init();
  shfs_read(file, buff);

  seeds = (shseed_t *)shbuf_data(buff);
  total = shbuf_size(buff) / sizeof(shseed_t);
  for (idx = 0; idx < total; idx++) {
    if (seeds[idx].seed_uid == seed->seed_uid) {
      memcpy(&seeds[idx], seed, sizeof(shseed_t));
      break;
    }
  }
  if (idx == total) {
    shbuf_cat(buff, seed, sizeof(shseed_t));
  }

  err = shfs_write(file, buff);
  shbuf_free(&buff);
  if (err)
    return (err);
  
  return (0);
}
#endif

#if 0
int shpam_shadow_pass_remove(shfs_ino_t *file, uint64_t rem_uid)
{
  shbuf_t *rbuff;
  shbuf_t *buff;
  shseed_t *seeds;
  int total;
  int idx;
  int err;

  rbuff = shbuf_init();
  shfs_read(file, rbuff);

  buff = shbuf_init();
  seeds = (shseed_t *)shbuf_data(rbuff);
  total = shbuf_size(rbuff) / sizeof(shseed_t);
  for (idx = 0; idx < total; idx++) {
    if (seeds[idx].seed_uid == rem_uid)
      continue;

    shbuf_cat(buff, &seeds[idx], sizeof(shseed_t));
  }
  shbuf_free(&rbuff);

  err = shfs_write(file, buff);
  shbuf_free(&buff);
  if (err)
    return (err);
  
  return (0);
}
#endif

#if 0
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
#endif

#if 0
int shpam_shadow_delete(shfs_ino_t *file, char *acc_name, shkey_t *sess_key)
{
  shadow_t *ent;
  shadow_t save;
  shkey_t *seed_key;
  int ret_err;
  int err;

  if (!sess_key)
    return (SHERR_INVAL);

  seed_key = shpam_seed(acc_name);
  ent = shpam_shadow(file, seed_key);
  shkey_free(&seed_key);
  if (!ent)
    return (SHERR_NOKEY);

  memcpy(&save, ent, sizeof(shadow_t));

  if (shtime64() >= save.sh_expire)
    return (SHERR_KEYEXPIRED);

  if (!shkey_cmp(&save.sh_sess, sess_key))
    return (SHERR_KEYREJECTED);

  if (save.sh_flag & SHPAM_LOCK)
    return (SHERR_ACCESS);

  ret_err = 0;

  err = shfs_cred_remove(file, &save.sh_seed);
  if (err)
    ret_err = err;

  err = shpam_shadow_pass_remove(file, shpam_uid(acc_name));
  if (err)
    ret_err = err;

  return (ret_err);
}
#endif

#if 0
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
#endif

#if 0
int shpam_shadow_pass_gen(shfs_ino_t *file, shseed_t *seed)
{
  shseed_t acc_seed;
  int err;

  if (!seed)
    return (SHERR_INVAL);

  if (shpam_shadow_pass(file, seed->seed_uid)) {
    return (SHERR_NOTUNIQ);
  }

  memcpy(&acc_seed, seed, sizeof(shseed_t));
  err = shpam_shadow_pass_append(file, &acc_seed);
  if (err)
    return (err);

  return (0);
}
#endif

#if 0
int shpam_shadow_pass_new(shfs_ino_t *file, char *username, char *passphrase)
{
  shseed_t acc_seed;
  shseed_t *seed;
  uint64_t salt;
  int err;

  salt = shpam_salt();
  seed = shpam_pass_gen((char *)username, passphrase, salt);
  memcpy(&acc_seed, seed, sizeof(shseed_t));
  err = shpam_shadow_pass_gen(file, &acc_seed);
  if (err)
    return (err);

  return (0);
}
#endif

#if 0
int shpam_shadow_setpass(shfs_ino_t *file, shseed_t *seed, shkey_t *sess_key)
{
  shadow_t *ent;
  shadow_t save;
  shkey_t *seed_key;
  int err;

  seed_key = shkey_bin((char *)&seed->seed_uid, sizeof(seed->seed_uid));
  ent = shpam_shadow(file, seed_key);
  shkey_free(&seed_key);
  if (!ent)
    return (SHERR_NOENT);

  memcpy(&save, ent, sizeof(shadow_t));
  if (shtime64() >= save.sh_expire)
    return (SHERR_KEYEXPIRED);
  if (!shkey_cmp(&save.sh_sess, sess_key))
    return (SHERR_KEYREJECTED);

  seed_key = shkey_bin((char *)&seed->seed_uid, sizeof(seed->seed_uid));
  err = shpam_shadow_session_expire(file, seed_key, sess_key);
  shkey_free(&seed_key);
  if (err)
    return (err);

  err = shpam_shadow_pass_append(file, seed);
  if (err)
    return (err);

  return (0);
}
#endif




int shpam_shadow_session_new(shfs_ino_t *file, char *acc_name, char *passphrase)
{
  uint64_t uid;
  int err;

  uid = shpam_uid(acc_name);
  err = shpam_shadow_create(file, uid, NULL);
  if (err)
    return (err);

  err = shpam_pshadow_new(file, acc_name, passphrase);
  if (err)
    return (err); 


{
shadow_t shadow;
memset(&shadow, 0, sizeof(shadow));
err = shpam_shadow_load(file, uid, &shadow);
}

  return (0);
}

static shkey_t *_shpam_shadow_session_gen(shseed_t *seed, shkey_t *id_key, shtime_t stamp)
{
  /* generate new session */
  shkey_t *sess_key;

  sess_key = shpam_sess_gen(&seed->seed_key, stamp, id_key);
  if (!sess_key)
    return (NULL);

  return (sess_key);
}

int shpam_shadow_session(shfs_ino_t *file, shseed_t *seed, shkey_t **sess_p, shtime_t *expire_p)
{
  shadow_t *ent;
  shadow_t save;
  shkey_t *sess_key;
  shkey_t *ret_key;
  shkey_t *seed_key;
  shtime_t stamp;
  shtime_t now;
  uint64_t crc;
  int err;

  if (!file->tree)
    return (SHERR_INVAL);

  err = shpam_shadow_load(file, seed->seed_uid, &save);
  if (err) {
    return (err);
}

  now = shtime();
  if (shtime_after(now, save.sh_expire)) {
    /* generate new session key with default expiration */
    stamp = shtime_adj(now, MAX_SHARE_SESSION_TIME);
    sess_key = _shpam_shadow_session_gen(seed, &save.sh_id, stamp); 
    if (!sess_key)
      return (SHERR_KEYREVOKED);

    save.sh_expire = stamp;
    memcpy(&save.sh_sess, sess_key, sizeof(save.sh_sess));
    err = shpam_shadow_store(file, &save);
    shkey_free(&sess_key);
    if (err) {
      return (err);
}
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

int shpam_shadow_session_verify(shfs_ino_t *file, uint64_t uid, shkey_t *sess_key)
{
  shseed_t seed;
  shadow_t shadow;
  shkey_t sess_id;
  shkey_t pass_key;
  shtime_t sess_expire;
  int err;

  memset(&shadow, 0, sizeof(shadow));
  err = shpam_shadow_load(file, uid, &shadow);
  if (err)
    return (err);

  sess_expire = shadow.sh_expire;
  memcpy(&sess_id, &shadow.sh_id, sizeof(sess_id));

  err = shpam_pshadow_load(file, uid, &seed);
  if (err)
    return (err);

  memcpy(&pass_key, &seed.seed_key, sizeof(shkey_t));
  err = shpam_sess_verify(sess_key, &pass_key, sess_expire, &sess_id);
  if (err)
    return (err);

  return (0);
}

int shpam_shadow_session_set(shfs_ino_t *file, uint64_t uid, shkey_t *id_key, uint64_t sess_stamp, shkey_t *sess_key)
{
  shadow_t shadow;
  shseed_t save;
  int err;

  err = shpam_pshadow_load(file, uid, &save);
  if (err)
    return (err);

  err = shpam_sess_verify(sess_key, &save.seed_key, sess_stamp, id_key);
  if (err)
    return (err);

  err = shpam_shadow_load(file, uid, &shadow);
  if (err)
    return (err);

  if (!shkey_cmp(id_key, &shadow.sh_id))
    return (SHERR_INVAL);

  shadow.sh_expire = sess_stamp;
  memcpy(&shadow.sh_sess, sess_key, sizeof(shadow.sh_sess));
  err = shpam_shadow_store(file, &shadow);
  if (err)
    return (err);

  return (0);
}

int shpam_shadow_session_expire(shfs_ino_t *file, uint64_t uid, shkey_t *sess_key)
{
  shadow_t *ent;
  shadow_t save;
  int err;

  err = shpam_shadow_load(file, uid, &save);
  if (err)
    return (err);

  if (shtime_after(shtime(), save.sh_expire))
    return (SHERR_KEYEXPIRED);
  if (!shkey_cmp(&save.sh_sess, sess_key))
    return (SHERR_KEYREJECTED);

  save.sh_expire = 0;
  err = shpam_shadow_store(file, &save);
  if (err)
    return (err);

  return (0);
}

int shpam_shadow_login(shfs_ino_t *file, char *acc_name, char *acc_pass, shkey_t **sess_key_p)
{
  shadow_t v_shadow;
  shseed_t v_seed;
  uint64_t uid;
  int err;

  if (!file->tree)
    return (SHERR_INVAL);

  uid = shpam_uid(acc_name);

  err = shpam_shadow_load(file, uid, &v_shadow);
  if (err)
    return (err);

  err = shpam_ident_verify(&v_shadow.sh_id, uid, &file->tree->peer); 
  if (err)
    return (err);

  err = shpam_pshadow_load(file, uid, &v_seed);
  if (err)
    return (err);

  err = shpam_pass_verify(&v_seed, acc_name, acc_pass);
  if (err) { 
    return (err);
}

  err = shpam_shadow_session(file, &v_seed, sess_key_p, NULL);
  if (err) {
    return (err);
}

  return (0);
}

_TEST(shpam_shadow_login)
{
  shfs_t *fs;
  shfs_ino_t *file;
  shkey_t *sess_key;
  shpeer_t *peer;
  int err;

  peer = shpeer_init("test", NULL);
  fs = shfs_init(peer);
  shpeer_free(&peer);

  file = shfs_file_find(fs, "/shpam_shadow_login");
  _TRUEPTR(file);

  /* test new identity generation */
  _TRUE(0 == shpam_shadow_session_new(file, "test", "test"));

  /* test account validation */
  sess_key = NULL;
  _TRUE(0 == shpam_shadow_login(file, "test", "test", &sess_key));
  _TRUEPTR(sess_key);

  /* test account deletion */
  err = shpam_shadow_remove(file, shpam_uid("test"), sess_key);
  _TRUE(0 == err);
  shkey_free(&sess_key);

  /* test shadow pass entry deletion */
  _TRUE(0 == shpam_pshadow_remove(file, shpam_uid("test")));
  
}

