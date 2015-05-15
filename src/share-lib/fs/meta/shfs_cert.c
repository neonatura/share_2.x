
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

typedef struct shcert_t
{
  /** The type of file content. */
  char cert_mime[MAX_SHARE_NAME_LENGTH];
  /** The package name the file is associated with. */
  char cert_pkg[MAX_SHARE_NAME_LENGTH];
  /** The priveleged key of the peer that signed the certificate. */
  shkey_t cert_peer;
  /** The certificate's signature key. */
  shkey_t cert_sig;
  /** The time-stamp of when the certificate was generated. */
  shtime_t cert_stamp;
} shcert_t;

int shfs_cert_verify(shfs_ino_t *file)
{
  int err;

  err = shfs_sig_verify(file, shpeer_kpriv(peer));
  if (err) {
    PRINT_ERROR(err, "shfs_sig_verify");
    return (err);
  }

  return (0);
}
int shfs_cert_verify_path(char *exec_path)
{
  shfs_t *fs;
  SHFL *file;
  shpeer_t *peer;
  struct stat st;
  char path[PATH_MAX+1];
  char *app_name;
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
  sprintf(path, "/app/%s/exec", app_name);

  peer = shpeer();
  fs = shfs_init(peer);
  file = shfs_file_find(fs, path);
  err = shfs_certify(file);
  shfs_free(&fs);

  return (err);
}

#if 0
/**
 * @param peer The peer of the remote file-system partition where the certificate will be stored.
 */
int shfs_cert_export(SHFL *file, SHFS *out_file, shpeer_t *peer)
{
  shsig_t sig;
  int err;

  memset(&sig, 0, sizeof(sig));
  err = shfs_sig_gen(file, &sig, peer);
  if (err)
    return (err);

  return (0);
int shfs_sig_gen(shfs_ino_t *file, shsig_t *sig)
}
#endif

int shfs_cert_sign(SHFS *file, char *package)
{
  char path[SHFS_PATH_MAX];

  sprintf(path, "/app/%s/exec", app_name);

}

int shfs_app_import(char *exec_path, char *package)
{



}


/**
 * Generate a digital signature on a sharefs file.
 * @param mime The file's content type.
 * @param package The application package the file is a part of.
 */
int shfs_cert_sign(SHFL *file, shcert_t *cert)
{
  shpeer_t *peer;

  /* certificate is signed for local machine. */
  peer = shpeer_init();
}

