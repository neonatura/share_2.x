
/*
 * @copyright
 *
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
 *
 *  @endcopyright
 *
*/  

#include "share.h"


int shfs_meta(shfs_t *tree, shfs_ino_t *ent, shmeta_t **val_p)
{
  shfs_ino_t *meta_ent;
  shmeta_value_t *hdr;
  shmeta_t *h;
  shbuf_t *buff;
  unsigned char *data;
  size_t data_len;
  size_t of;
  int err;

  meta_ent = shfs_inode(ent, NULL, SHINODE_META);
  if (!meta_ent)
    return (-1);

  h = shmeta_init();
  if (!h)
    return (-1);

  buff = shbuf_init();
  err = shfs_aux_read(meta_ent, buff);
  if (err < 0) {
    PRINT_ERROR(err, "shfs_meta");
    return (err);
  }

  data = shbuf_data(buff);
  data_len = shbuf_size(buff);

  if (meta_ent->pool) {
    free(meta_ent->pool);
  }
  meta_ent->pool = (unsigned char *)calloc(data_len, sizeof(char));
  if (!meta_ent->pool)
    return (SHERR_NOMEM);
  memcpy(meta_ent->pool, data, data_len);
  shbuf_free(&buff);

  for (of = 0; of < data_len; of += sizeof(shmeta_value_t)) {
    hdr = (shmeta_value_t *)(meta_ent->pool + of); 
    shmeta_set(h, &hdr->name, hdr);
    of += hdr->sz;
  }

  if (val_p)
    *val_p = h; 
  else
    shmeta_free(&h);

  return (0);
}

_TEST(shfs_meta)
{
  shfs_t *tree;
  shfs_ino_t *file;
  shmeta_t *val = NULL;

  _TRUEPTR(tree = shfs_init(NULL)); 
  _TRUEPTR(file = shfs_inode(tree->base_ino, "shfs_meta", SHINODE_FILE));
  _TRUE(!shfs_meta(tree, file, &val));
  _TRUEPTR(val);
  shfs_meta_free(&val);
}

int shfs_meta_save(shfs_t *tree, shfs_ino_t *ent, shmeta_t *h)
{
  shfs_ino_t *meta_ent;
  shmeta_value_t *hdr;
  shsize_t data_len;
  shbuf_t *buff;
  char *data;
  char *map;
  int err;

  meta_ent = shfs_inode(ent, NULL, SHINODE_META);
  if (!meta_ent)
    return (SHERR_IO);

  buff = shbuf_init();
  if (!buff)
    return (SHERR_IO);

  if (h)
    shmeta_print(h, buff);

  err = shfs_aux_write(meta_ent, buff);
  if (err)
    return (err);

  shbuf_free(&buff);

  return (0);
}

_TEST(shfs_meta_save)
{
  shfs_t *tree;
  shfs_ino_t *dir;
  shmeta_t *h = NULL;
  shmeta_value_t *val_p;
  shmeta_value_t val;
  shkey_t *key;


  _TRUEPTR(tree = shfs_init(NULL)); 
  _TRUEPTR(dir = shfs_inode(tree->base_ino, "shfs_meta_save", SHINODE_DIRECTORY));
  if (!h)
    return;

  key = shkey_uniq();

  /* save a definition to disk. */
  memset(&val, 0, sizeof(val));
  shmeta_set(h, key, &val); 
  _TRUEPTR(val_p = shmeta_get(h, key));
  _TRUE(!shfs_meta_save(tree, dir, h));
  shfs_meta_free(&h);

  _TRUE(!shfs_meta(tree, dir, &h));
  _TRUEPTR(h);

  _TRUEPTR(val_p = shmeta_get(h, key)); 
  if (val_p)
    _TRUE(0 == memcmp(&val, val_p, sizeof(val)));
  shfs_meta_free(&h);

  shkey_free(&key);
}


int shfs_meta_set(shfs_ino_t *file, char *def, char *value)
{
  int err;

  if (!file->meta) {
    err = shfs_meta(file->tree, file, &file->meta);  
    if (err)
      return (err);
  }

  shmeta_set_str(file->meta, ashkey_str(def), value);

  err = shfs_meta_save(file->tree, file, file->meta);
  if (err)
    return (err);

  return (0);
}

const char *shfs_meta_get(shfs_ino_t *file, char *def)
{
  static char blank_str[256];
  shkey_t *key;
  char *str;
  int err;
 
  if (!file->meta) {
    err = shfs_meta(file->tree, file, &file->meta);  
    if (err) {
      PRINT_ERROR(err, "shfs_meta_get");
      return ((const char *)blank_str);
    }
  }

  key = shkey_str(def);
  str = shmeta_get_str(file->meta, key);
  shkey_free(&key);

  if (!str)
    return ((const char *)blank_str);

  return ((const char *)str);
}

int shfs_meta_perm(shfs_ino_t *file, char *def, shkey_t *user)
{
  const char *str;
 
  str = shfs_meta_get(file, def);
  if (0 == strcmp(str, shkey_print(user)))
    return (0);

  return (SHERR_ACCESS);
}

int shfs_sig_gen(shfs_ino_t *file, shsig_t *sig)
{
  static shsig_t raw_sig;
  shkey_t *key;
  time_t stamp;
  char *key_str;
  unsigned char *data;
  size_t data_len;
  int err;

  if (!sig) {
    memset(&raw_sig, 0, sizeof(raw_sig));
    sig = &raw_sig;
  }

  /* peer key */
  if (file->tree) {
    shpeer_t *peer = file->tree->peer;
    if (peer)
      memcpy(&sig->sig_peer, &peer->name, sizeof(shkey_t));
  }

  sig->sig_stamp = file->blk.hdr.ctime;
  key = shkey_cert(shfs_crc(file), &sig->sig_peer, sig->sig_stamp);
  memcpy(&sig->sig_key, key, sizeof(shkey_t));
  shkey_free(&key);

  sig->sig_ref = 0;
  memset(&sig->sig_id, 0, sizeof(shkey_t));
  key = shkey_bin((char *)&sig, sizeof(sig)); 
  memcpy(&sig->sig_id, key, sizeof(shkey_t));
  shkey_free(&key);

  key_str = (char *)shkey_print(&sig->sig_key);
  err = shfs_meta_set(file, SHMETA_SIGNATURE, key_str);
  if (err)
    return (err);

  if (file->tree) {
    char idx_path[PATH_MAX+1];
    shfs_ino_t *idx_file;

    /* index signature */
    sprintf(idx_path, "/%s/%s/%s", BASE_SHMETA_PATH, SHMETA_SIGNATURE, shkey_print(&sig->sig_id));
    idx_file = shfs_file_find(file->tree, idx_path);
    err = shfs_file_write(idx_file, &sig, sizeof(shsig_t)); 
    if (err)
      PRINT_ERROR(err, idx_path);
  }

  return (0);
}

int shfs_sig_get(shfs_ino_t *file, shsig_t *sig)
{
  int err;
  const char *key_str;

  if (!file || !sig)
    return (SHERR_INVAL);

  memset(&sig->sig_key, 0, sizeof(sig->sig_key));
  key_str = shfs_meta_get(file, SHMETA_SIGNATURE);
  if (key_str && *key_str) {
    shkey_t *key = shkey_gen((char *)key_str);
    memcpy(&sig->sig_key, key, sizeof(shkey_t));
    shkey_free(&key);
  }

  sig->sig_stamp = file->blk.hdr.ctime;

  return (0);
}

int shfs_sig_verify(shfs_ino_t *file, shkey_t *peer_key)
{
  shsig_t sig;
  unsigned char *data;
  size_t data_len;
  int err;

  memset(&sig, 0, sizeof(sig));
  memcpy(&sig.sig_peer, peer_key, sizeof(sig.sig_peer));

  err = shfs_sig_get(file, &sig);
  if (err) {
    PRINT_ERROR(err, "shfs_sig_verify [shfs_sig_get]");
    return (err);
  }

  err = shfs_file_read(file, &data, &data_len);
  if (err) {
    PRINT_ERROR(err, "shfs_sig_verify [shfs_file_read]");
    return (err);
  }

  err = shkey_verify(&sig.sig_key, shfs_crc(file), peer_key, sig.sig_stamp);
  if (err) {
    return (err);
  }

  return (0);
}

_TEST(shfs_sig_verify)
{
  shfs_t *tree;
  SHFL *file;
  shpeer_t *peer;
  shkey_t fake_key;
  char path[PATH_MAX+1];
  char buf[256];
  int err;

  peer = shpeer_app(NULL);

  _TRUEPTR(tree = shfs_init(peer)); 

  strcpy(path, "/shfs_sig_gen");
  _TRUEPTR(file = shfs_file_find(tree, path));

  memset(buf, 'a', sizeof(buf));
  _TRUE(0 == shfs_file_write(file, buf, sizeof(buf)));

  _TRUE(0 == shfs_sig_gen(file, NULL));

  _TRUE(0 == shfs_sig_verify(file, &peer->name));

  memset(&fake_key, 0, sizeof(fake_key));
  _TRUE(0 != shfs_sig_verify(file, &fake_key));

  shfs_free(&tree);


  _TRUEPTR(tree = shfs_init(peer)); 
  strcpy(path, "/shfs_sig_gen");
  _TRUEPTR(file = shfs_file_find(tree, path));
  _TRUE(0 == shfs_sig_verify(file, &peer->name));
  _TRUE(0 == shfs_sig_verify(file, &peer->name));


  shfs_free(&tree);

}
