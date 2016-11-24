
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



shkey_t *shfs_cert_sig(shcert_t *cert)
{
  uint64_t ser_crc;
  shkey_t *key;

  ser_crc = shcrc(cert->cert_sub.ent_ser, 16);
  key = shkey_cert(shcert_sub_sig(cert), ser_crc, shcert_sub_expire(cert));

  return (key);
}

/**
 * Apply a digital certificate on a sharefs file.
 * @param package The application package the file is a part of.
 */
int shfs_cert_apply(SHFL *file, shcert_t *cert)
{
  unsigned char *raw;
  shkey_t *sig_key;
  shkey_t *key;
  shfs_t *tree;
  shlic_t *lic;
  size_t raw_len;
  int err;

  /* assign reference to cert */
  sig_key = shfs_cert_sig(cert);
  if (!sig_key)
    return (SHERR_NOMEM);

  err = shfs_sig_set(file, sig_key);
  if (err)
    return (err);

  raw_len = sizeof(shcert_t) + sizeof(shlic_t);
  raw = (char *)calloc(raw_len, sizeof(char));
  if (!raw) {
    shkey_free(&sig_key);
    return (SHERR_NOMEM);
  }

  tree = shfs_inode_tree(file);
  lic = (shlic_t *)(raw + sizeof(shcert_t));

  /* copy cert */
  memcpy(raw, cert, sizeof(shcert_t));

  /* fill license */
  memcpy(&lic->lic_fs, shpeer_kpub(&tree->peer), sizeof(shkey_t));
  memcpy(&lic->lic_ino, shfs_token(file), sizeof(shkey_t));
  memcpy(&lic->lic_cert, shcert_sub_sig(cert), sizeof(shkey_t));
  lic->lic_expire = shcert_sub_expire(cert);
  lic->lic_crc = shfs_crc(file);

  /* generate key from underlying cert+lic data. */
  key = shkey_bin(raw, raw_len);
  memcpy(&lic->lic_sig, key, sizeof(shkey_t));
  shkey_free(&key);

  /* store certificate + license inside file */
  err = shfs_cred_store(file, sig_key, raw, raw_len);
  shkey_free(&key);
  if (err)
    return (err);

  return (0);
}

int shfs_cert_get(SHFL *fl, shcert_t **cert_p, shlic_t **lic_p)
{
  shkey_t *sig_key;
  unsigned char *raw;
  size_t raw_len;
  int err;

  sig_key = shfs_sig_get(fl);
  if (!sig_key)
    return (SHERR_INVAL);

  raw_len = sizeof(shcert_t) + sizeof(shlic_t);
  raw = (unsigned char *)calloc(raw_len, sizeof(char));
  if (!raw) {
    shkey_free(&sig_key);
    return (SHERR_NOMEM);
  }

  err = shfs_cred_load(fl, sig_key, raw, raw_len);
  shkey_free(&sig_key);
  if (err) {
    free(raw);
    return (err);
  }

  if (cert_p) {
    shcert_t *cert = (shcert_t *)calloc(1, sizeof(shcert_t));
    memcpy(cert, raw, sizeof(shcert_t));
    *cert_p = cert;
  }
  if (lic_p) {
    shlic_t *lic = (shlic_t *)calloc(1, sizeof(shlic_t));
    memcpy(lic, (raw + sizeof(shcert_t)), sizeof(shlic_t)); 
    *lic_p = lic;
  }

  free(raw);
  return (0);
}

#if 0
/* deprec */
int shfs_cert_verify(shfs_ino_t *file, shcert_t *parent)
{
  shcert_t *cert;
  int err;

  err = shfs_cert_get(file, &cert, NULL);
  if (err)
    return (err);

  err = shcert_verify(cert, parent);
  if (err)
    return (err);

  return (0);
}
int shfs_cert_verify_path(char *exec_path)
{
  shfs_t *fs;
  SHFL *file;
  shpeer_t *peer;
  struct stat st;
  char *app_name;
  char path[SHFS_PATH_MAX];
  int err;

  err = stat(exec_path, &st);
  if (!err && S_ISDIR(st.st_mode)) {
    PRINT_ERROR(SHERR_ISDIR, exec_path);
    return (SHERR_ISDIR);
  }
  if (err) {
    err = -errno;
    PRINT_ERROR(err, exec_path);
    return (err);
  }

  app_name = shfs_app_name(exec_path);
  sprintf(path, "%s/%s", app_name, SHFS_FILE_EXECUTABLE);
  fs = shfs_sys_init(SHFS_DIR_APPLICATION, path, &file);
  err = shfs_cert_verify(file, NULL);
  shfs_free(&fs);
  if (err)
    return (err);

  return (0);
}
#endif

/**
 * Copy a sharefs file's certificate into another file's binary content.
 */
int shfs_cert_export(SHFL *file, SHFS *out_file)
{
  shcert_t *cert;
  shbuf_t *buff;
  int err;

  err = shfs_cert_get(file, &cert, NULL);
  if (err)
    return (err);

  buff = shbuf_map((unsigned char *)cert, sizeof(shcert_t));
  err = shfs_write(file, buff);
  free(buff);
  free(cert);
  if (err)
    return (err);

  return (0);
}


/**
 * Save a certificate to the system-level certificate directory.
 * @param cert The certificate to store.
 * @param ref_path A optional relative path [to the sys cert dir] to reference the certificate.
 * @note Overwites original certificate.
 */
int shfs_cert_save(shcert_t *cert, char *ref_path)
{
  SHFL *file;
  SHFL *l_file;
  shpeer_t *peer;
  shfs_t *fs;
  shbuf_t *buff;
  char path[SHFS_PATH_MAX];
  char sig_name[MAX_SHARE_HASH_LENGTH];
  int err;

/* DEBUG: TODO: only allow over-write when owner id is same */

  /* store in sharefs sytem hierarchy of 'package' partition. */
  memset(sig_name, 0, sizeof(sig_name));
  strncpy(sig_name, shcert_serialno(cert), sizeof(sig_name)-1);
  fs = shfs_sys_init(SHFS_DIR_CERTIFICATE, sig_name, &file);
  if (!fs)
    return (SHERR_IO);

  buff = shbuf_map((unsigned char *)cert, sizeof(shcert_t));
  err = shfs_write(file, buff);
  free(buff);
  if (err)
    return (err);

  if (ref_path) {
    /* an alias's link reference. */
    l_file = shfs_file_find(fs, shfs_sys_dir(SHFS_DIR_CERTIFICATE, ref_path));
    shfs_ref_set(l_file, file);
  }

  shfs_free(&fs);
  if (err)
    return (err);

  return (0);
}

/**
 * Load a system-level certificate by it's serial number.
 */
shcert_t *shfs_cert_load(char *serial_no)
{
  SHFL *file;
  shcert_t *cert;
  shbuf_t *buff;
  shfs_t *fs;
  int err;

  buff = shbuf_init();
  fs = shfs_sys_init(SHFS_DIR_CERTIFICATE, serial_no, &file);
  err = shfs_read(file, buff);
  if (err) {
    shbuf_free(&buff);
    return (NULL);
  }

  cert = (shcert_t *)calloc(1, sizeof(shcert_t));
  if (!cert) {
    shbuf_free(&buff);
    return (NULL);
  }
  memcpy(cert, shbuf_data(buff), MIN(sizeof(shcert_t), shbuf_size(buff)));
  shbuf_free(&buff);

  return (cert);

}

/**
 * Load a system-level certificate by an alias reference.
 * @ref_path The relative [to the sys cert dir] path of the file reference.
 */
shcert_t *shfs_cert_load_ref(char *ref_path)
{
  SHFL *file;
  shcert_t *cert;
  shbuf_t *buff;
  shfs_t *fs;
  int err;

  buff = shbuf_init();
  fs = shfs_sys_init(SHFS_DIR_CERTIFICATE, ref_path, &file);
  err = shfs_read(file, buff);
  if (err) {
    shbuf_free(&buff);
    return (NULL);
  }

  cert = (shcert_t *)calloc(1, sizeof(shcert_t));
  if (!cert) {
    shbuf_free(&buff);
    return (NULL);
  }
  memcpy(cert, shbuf_data(buff), MIN(sizeof(shcert_t), shbuf_size(buff)));
  shbuf_free(&buff);

  return (cert);
}

int shfs_cert_remove_ref(char *ref_path)
{
  SHFL *file;
  shcert_t *cert;
  shbuf_t *buff;
  shfs_t *fs;
  int err;

  fs = shfs_sys_init(SHFS_DIR_CERTIFICATE, ref_path, &file);
  err = shfs_file_remove(file);
  shfs_free(&fs);
  if (err) 
    return (err);

  return (0);
}

