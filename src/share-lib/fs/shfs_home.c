
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

shfs_t *shfs_home_fs(shkey_t *id_key)
{
  shfs_t *fs;
  shfs_ino_t *file;
  shpeer_t *peer;
  char buf[SHFS_PATH_MAX];
  char fs_path[SHFS_PATH_MAX];
  int err;

  if (!id_key)
    return (NULL);

  memset(buf, 0, sizeof(buf));
  snprintf(buf, sizeof(buf) - 1, "home:%s", shkey_hex(id_key));
  peer = shpeer_init(buf, NULL);
  fs = shfs_init(peer);
  shpeer_free(&peer);

  /* initialize home directory */
  sprintf(fs_path, "/%s/", shkey_hex(shpeer_kpriv(&fs->peer)));
  file = shfs_dir_find(fs, fs_path);
  shfs_access_owner_set(file, id_key);
  shfs_attr_set(file, SHATTR_SYNC); /* synchronize. */

  return (fs);
}

shfs_ino_t *shfs_home_file(shfs_t *fs, char *path)
{
  shfile_t *dir;
  shfile_t *file;
  char fs_path[SHFS_PATH_MAX];
  int err;

  /* base dir is group key from user identity */
  sprintf(fs_path, "/%s/", shkey_hex(shpeer_kpriv(&fs->peer)));
  dir = shfs_dir_find(fs, fs_path);
  if (!dir)
    return (NULL);

  /* obtain file reference to home dir path */
  if (*path == '/') path++;
  strncat(fs_path, path, sizeof(fs_path) - strlen(fs_path) - 1);
  file = shfs_file_find(fs, fs_path);
  
  return (file);
}

