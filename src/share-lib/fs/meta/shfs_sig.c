
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

int shfs_sig_gen(shfs_ino_t *file, shsig_t *sig)
{
  static shsig_t raw_sig;
  shfs_t *tree;
  shkey_t *key;
  shkey_t *peer_key;
  shbuf_t *buff;
  shpeer_t *peer;
  time_t stamp;
  char key_str[MAX_SHARE_HASH_LENGTH];
  unsigned char *data;
  size_t data_len;
  int err;

  if (!file || !file->tree)
    return (SHERR_INVAL);

  if (!sig) {
    sig = &raw_sig;
  }
  memset(sig, 0, sizeof(shsig_t));

  /* peer key */
  tree = shfs_inode_tree(file);
  if (!tree)
    return (SHERR_IO);
  peer = &tree->peer;
//  memcpy(&sig->sig_peer, shpeer_kpub(peer), sizeof(shkey_t));

  sig->sig_stamp = file->blk.hdr.ctime;
  sig->sig_expire = shtime_adj(sig->sig_stamp, SHARE_DEFAULT_EXPIRE_TIME);
  key = shkey_cert(shpeer_kpub(peer), shfs_crc(file), sig->sig_stamp);
  memcpy(&sig->sig_key, key, sizeof(shkey_t));
  shkey_free(&key);

#if 0
  key = shkey_bin((char *)sig, sizeof(shsig_t));
  memcpy(&sig->sig_id, key, sizeof(shkey_t));
  shkey_free(&key);
#endif

  memset(key_str, 0, sizeof(key_str));
  strncpy(key_str, shkey_print(&sig->sig_key), sizeof(key_str) - 1);
  err = shfs_meta_set(file, SHMETA_SIGNATURE, key_str);
  if (err)
    return (err);

#if 0
  {
    char idx_path[PATH_MAX+1];
    shfs_ino_t *idx_file;

    /* index signature */
    sprintf(idx_path, "/%s/%s/%s", BASE_SHMETA_PATH, SHMETA_SIGNATURE, shkey_print(&sig->sig_id));
    idx_file = shfs_file_find(file->tree, idx_path);
    buff = shbuf_init();
    shbuf_cat(buff, &sig, sizeof(shsig_t));
    err = shfs_write(idx_file, buff);
    shbuf_free(&buff);
    if (err)
      PRINT_ERROR(err, idx_path);
  }
#endif

  return (0);
}

int shfs_sig_get(shfs_ino_t *file, shsig_t *sig)
{
  struct stat st;
  const char *key_str;
  int err;

  if (!file || !sig)
    return (SHERR_INVAL);

  err = shfs_fstat(file, &st);
  if (err)
    return (err);

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

int shfs_sig_verify(shfs_ino_t *file)
{
  shpeer_t *peer;
  shsig_t sig;
  uint64_t crc;
  int err;

  memset(&sig, 0, sizeof(sig));
//  memcpy(&sig.sig_peer, peer_key, sizeof(sig.sig_peer));

  err = shfs_sig_get(file, &sig);
  if (err) {
    return (err);
  }

  peer = shfs_inode_peer(file);
  if (!peer)
    return (SHERR_IO);

  crc = shfs_crc(file);
  err = shkey_verify(&sig.sig_key, crc, shpeer_kpub(peer), sig.sig_stamp);
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
  shbuf_t *buff;
  char path[PATH_MAX+1];
  char buf[256];
  int err;

  peer = shpeer_init("test", NULL);

  _TRUEPTR(tree = shfs_init(peer)); 

  strcpy(path, "/shfs_sig_gen");
  _TRUEPTR(file = shfs_file_find(tree, path));

  memset(buf, 'T', sizeof(buf));
  buff = shbuf_init();
  shbuf_cat(buff, buf, sizeof(buf));
  _TRUE(0 == shfs_write(file, buff));
  shbuf_free(&buff);

  _TRUE(0 == shfs_sig_gen(file, NULL));

  err = shfs_sig_verify(file);
  _TRUE(0 == err);






  shfs_free(&tree);


  _TRUEPTR(tree = shfs_init(peer)); 
  strcpy(path, "/shfs_sig_gen");
  _TRUEPTR(file = shfs_file_find(tree, path));

  err = shfs_sig_verify(file);
  _TRUE(0 == err);

  shfs_free(&tree);
  shpeer_free(&peer);

}

shkey_t *shfs_sig_id(shsig_t *sig)
{
  return (shkey_bin((char *)sig, sizeof(shsig_t)));
}




char *shsig_alg_str(int alg)
{
  static char ret_buf[4096];

  memset(ret_buf, 0, sizeof(ret_buf));

  if (alg & SHKEY_ALG_SHR)
    strcat(ret_buf, "SHR ");

  if (alg & SHKEY_ALG_MD5)
    strcat(ret_buf, "MD5 ");
  else if (alg & SHKEY_ALG_MD4)
    strcat(ret_buf, "MD4 ");
  else if (alg & SHKEY_ALG_MD2)
    strcat(ret_buf, "MD2 ");

  if (alg & SHKEY_ALG_SHA512)
    strcat(ret_buf, "SHA512 ");
  else if (alg & SHKEY_ALG_SHA384)
    strcat(ret_buf, "SHA384 ");
  else if (alg & SHKEY_ALG_SHA256)
    strcat(ret_buf, "SHA256 ");
  else if (alg & SHKEY_ALG_SHA224)
    strcat(ret_buf, "SHA224 ");
  else if (alg & SHKEY_ALG_SHA1)
    strcat(ret_buf, "SHA1 ");

  if (alg & SHKEY_ALG_RSA) 
    strcat(ret_buf, "RSA ");

  if (*ret_buf)
    ret_buf[strlen(ret_buf)-1] = '\000';
  
  return (ret_buf);
}
