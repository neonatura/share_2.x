
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


/** write package info to disk */
int shpkg_info_write(shpkg_t *pkg)
{
  SHFL *file;
  shbuf_t *buff;
  size_t data_len;
  unsigned char *data;
  char path[SHFS_PATH_MAX];
  int err;

  err = shencode((char *)pkg, sizeof(pkg),
      &data, &data_len, shcert_sub_sig(&pkg->pkg.pkg_cert));
  if (err)
    return (err);

  sprintf(path, "/sys/pkg/%s/info", pkg->pkg.pkg_name);
  file = shfs_file_find(pkg->pkg_fs, path);
  buff = shbuf_map(data, data_len);
  err = shfs_write(file, buff); 
  free(buff);
  free(data);
  if (err)
    return (err);

  return (0);
}

int shpkg_info_read(shpkg_t *pkg)
{
  SHFL *file;
  shbuf_t *buff;
  char *data;
  char path[SHFS_PATH_MAX];
  size_t data_len;
  int err;
  
  /* read package info from disk */
  buff = shbuf_init();
  sprintf(path, "/sys/pkg/%s/info", pkg->pkg.pkg_name);
  file = shfs_file_find(pkg->pkg_fs, path);
  err = shfs_read(file, buff);
  if (err) {
    shbuf_free(&buff);
    return (err);
  }

  err = shdecode(shbuf_data(buff), shbuf_size(buff),
    &data, &data_len, shcert_sub_sig(&pkg->pkg.pkg_cert));
  shbuf_free(&buff);
  if (err) {
    free(data);
    return (err);
  }

  /* fill supplied content as package data. */
  memcpy(&pkg->pkg, data, MIN(data_len, sizeof(shpkg_info_t)));
  free(data);

  return (0);
}

/**
 * @param file The file being defined contained inside the share package.
 */
int shpkg_def_set(shpkg_t *pkg, SHFL *file, char *mime_type, char *sys_dir)
{
  SHFL *def_file;
  shmime_t *mime;
  shpkg_def_t def;
  shbuf_t *buff;
  char path[SHFS_PATH_MAX];
  int err;

  mime = shmime(mime_type);
  if (!mime)
    return (SHERR_INVAL);

  err = shmime_file_set(file, mime->mime_name);
  if (err)
    return (err);

  memset(&def, 0, sizeof(def));
  if (sys_dir)
    strncpy(def.def_dir, sys_dir, sizeof(def.def_dir)-1);
  else
    strcpy(def.def_dir, mime->mime_dir);

  buff = shbuf_map((unsigned char *)&def, sizeof(shpkg_def_t));
  sprintf(path, "/sys/pkg/%s/def/%s",
      pkg->pkg.pkg_name, shkey_print(shfs_token(file)));
  def_file = shfs_file_find(shfs_inode_tree(file), path);
  err = shfs_write(def_file, buff);
  free(buff);
  if (err)
    return (err);

  return (0); 
}

shpkg_def_t *shpkg_def(shpkg_t *pkg, SHFL *file)
{
  SHFL *def_file;
  shpkg_def_t *def;
  shmime_t *mime;
  shbuf_t *buff;
  char path[SHFS_PATH_MAX];
  int err;

  def = (shpkg_def_t *)calloc(1, sizeof(shpkg_def_t));
  if (!def)
    return (NULL);

  buff = shbuf_init();
  sprintf(path, "/sys/pkg/%s/def/%s",
      pkg->pkg.pkg_name, shkey_print(shfs_token(file)));
  def_file = shfs_file_find(shfs_inode_tree(file), path);
  err = shfs_read(def_file, buff);
  if (!err) {
    memcpy(def, shbuf_data(buff), MIN(sizeof(shpkg_def_t), shbuf_size(buff)));
  } else {
    mime = shmime_file(file);
    if (!mime)
      return (NULL);

    memcpy(&def->def_mime, mime, sizeof(def->def_mime));
    strcpy(def->def_dir, mime->mime_dir);
  }

  shbuf_free(&buff);
  return (def);
}


/**
 * Initialize a new package.
 */
shpkg_t *shpkg_init(char *pkg_name, shcert_t *pkg_cert)
{
  shpeer_t *pkg_peer;
  shpkg_t *pkg;
  char path[SHFS_PATH_MAX];
  int err;

  memset(path, 0, sizeof(path));
  snprintf(path, sizeof(path)-1, "/sys/pkg/%s", pkg_name);

  pkg = (shpkg_t *)calloc(1, sizeof(shpkg_t));
  if (!pkg)
    return (NULL);

  /* fill package info */
  strncpy(pkg->pkg.pkg_name, pkg_name, sizeof(pkg->pkg.pkg_name) - 1);
  if (pkg_cert)
    memcpy(&pkg->pkg.pkg_cert, pkg_cert, sizeof(pkg->pkg.pkg_cert));

  /* fill peer responsible for generating package */
  pkg_peer = shpeer_init("package", NULL);
  memcpy(&pkg->pkg.pkg_peer, pkg_peer, sizeof(pkg->pkg.pkg_peer));
  
  /* initizize sharefs package file */
  pkg->pkg_fs = shfs_init(pkg_peer);
  shpeer_free(&pkg_peer);
  pkg->pkg_file = shfs_dir_find(pkg->pkg_fs, path);
  err = shfs_attr_set(pkg->pkg_file, SHATTR_ARCH); 
  if (err) {
    shpkg_free(&pkg);
    return (NULL);
  }

  err = shpkg_info_write(pkg);
  if (err) {
    shpkg_free(&pkg);
    return (NULL);
  }

  return (pkg);
}

/**
 * Initialize a package with a dynamically generated certificate.
 */
shpkg_t *shpkg_create(char *pkg_name, char *entity, uint64_t fee, int flags)
{
  shcert_t cert;
  int err;

  memset(&cert, 0, sizeof(cert));
  err = shcert_init(&cert, entity, fee, flags);
  if (err)
    return (err);

  return (shpkg_init(pkg_name, &cert));
}

/**
 * Initialize a public-access package.
 */
shpkg_t *shpkg_create_public(char *pkg_name, char *entity)
{
  return (shpkg_create(pkg_name, entity, 0, SHCERT_ENT_ORGANIZATION));
}

/**
 * Load the resources from a share package.
 */
shpkg_t *shpkg_load(char *pkg_name, shkey_t *cert_sig)
{
  SHFL *pkg_file;
  SHFL *file;
  shpeer_t *pkg_peer;
  shpkg_t *pkg;
  shfs_t *fs;
  struct stat st;
  char path[SHFS_PATH_MAX];
  int err;

  pkg_peer = shpeer_init("package", NULL);
  fs = shfs_init(pkg_peer);
  shpeer_free(&pkg_peer);
  sprintf(path, "/sys/pkg/%s/", pkg_name);
  pkg_file = shfs_dir_find(fs, path);
  err = shfs_fstat(pkg_file, &st);
  if (err)
    return (NULL); /* no exist */

  pkg = (shpkg_t *)calloc(1, sizeof(shpkg_t)); 
  if (pkg)
    return (NULL);

  pkg->pkg_fs = fs;
  pkg->pkg_file = pkg_file;
  err = shpkg_info_read(pkg);
  if (err) {
    shpkg_free(&pkg);
    return (NULL);
  }

  if (!cert_sig)
    cert_sig = ashkey_blank(); 
  if (0 == memcmp(cert_sig,
        shcert_sub_sig(&pkg->pkg.pkg_cert), sizeof(shkey_t))) {
    shpkg_free(&pkg);
    return (NULL);
  }

  return (pkg);
}

void shpkg_free(shpkg_t **pkg_p)
{
  shpkg_t *pkg;

  if (!pkg_p)
    return;
  pkg = *pkg_p;
  if (!pkg)
    return;
  *pkg_p = NULL;

  shfs_free(&pkg->pkg_fs);

  free(pkg);
}

/**
 * Obtain the package certificate's signature key.
 */
shkey_t *shpkg_sig(shpkg_t *pkg)
{
  return (shcert_sub_sig(&pkg->pkg.pkg_cert));
}

/**
 * Add a fresh copy of a file to a share package.
 */
int shpkg_file_add(shpkg_t *pkg, char *mime_type, SHFL *file, char *sys_dir)
{
  SHFL *pkg_file;
  shmime_t *mime;
  char path[SHFS_PATH_MAX];
  char *dir;
  int err;

  mime = shmime(mime_type);
  if (!mime)
    return (SHERR_INVAL);

  sprintf(path, "/sys/pkg/%s/%s/%s", 
    pkg->pkg.pkg_name, mime->mime_dir, shfs_filename(file));
  pkg_file = shfs_file_find(pkg->pkg_fs, path);
  err = shfs_file_copy(file, pkg_file); 
  if (err)
    return (err);

  err = shpkg_def_set(pkg, file, mime->mime_name, sys_dir);
  if (err)
    return (err);

  return (0);
}

/**
 * Apply a pakage license to an extracted file.
 */
int shpkg_file_license(shpkg_t *pkg, SHFL *file)
{
  int err;

  err = shlic_sign(file, &pkg->pkg.pkg_cert);
  if (err)
    return (err);

  return (0);
}

/**
 * Extract a file from a packge into the sharefs sytem diretory hierarchy.
 * @param The sharefs file being extracted from the package.
 * @note Erases pre-existing files that cannot be licensed.
 */
int shpkg_file_extract(shpkg_t *pkg, SHFL *file)
{
  SHFL *sys_file;
  shmime_t *mime;
  shpkg_def_t *def;
  char path[SHFS_PATH_MAX];
  int err;

  def = shpkg_def(pkg, file);
  if (!def)
    return (SHERR_INVAL);

  sprintf(path, "/sys/%s/%s", def->def_dir, shfs_filename(file));
  sys_file = shfs_file_find(pkg->pkg_fs, path);
  err = shfs_file_copy(file, sys_file); 
  if (err)
    return (err);

  err = shpkg_file_license(pkg, file);
  if (err) {
    shfs_inode_remove(sys_file);
    return (err);
  }

  return (0);
}

