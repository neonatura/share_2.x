
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

#if 0
int shfs_access_user(shfs_ino_t *inode, shkey_t *user, int flag)
{
#if 0
  uint64_t user_uid = shkey_crc(user);
  uint64_t ino_uid;

  ino_uid = shfs_uid(inode, flag);
  if (!ino_uid)
    return (0); /* public */
  
  if (user_uid == ino_uid)
    return (0); /* owner */

  return (SHERR_ACCESS);
#endif
return (0);
}

int shfs_access_group(shfs_ino_t *inode, shkey_t *user, int flag)
{
#if 0
  uint64_t uid;
  uint64_t gid;

  gid = shfs_gid(inode, flag);

  if (gid == 0)
    return (0);

  uid = shcrc(user, sizeof(shkey_t)) % MAX_SHAREFS_GROUPS;
  if ((uid % gid) == (gid % uid))
    return (0);

  return (SHERR_ACCESS);
#endif
return (0);
}

int shfs_access(shfs_ino_t *inode, shkey_t *user, int flags)
{
  int err;

  err = shfs_access_user(inode, user, flags);
  if (!err)
    return (0);

  err = shfs_access_group(inode, user, flags);
  if (!err)
    return (0);

  return (SHERR_ACCESS);
}
#endif





int shfs_access_read(shfs_ino_t *file, shkey_t *id_key)
{
  shkey_t *key;
  int is_owner;

  if (shfs_attr(file) & SHATTR_READ)
    return (0); /* global read access */

  is_owner = FALSE;
  if (id_key) {
    key = shfs_access_owner_get(file);
    if (!key)
      return (0);

    is_owner = shkey_cmp(id_key, key);
    shkey_free(&key);
  }
  if (!is_owner)
    return (SHERR_ACCESS);

  return (0);
}
int shfs_access_write(shfs_ino_t *file, shkey_t *id_key)
{
  shkey_t *key;
  int is_owner;

  if (shfs_attr(file) & SHATTR_WRITE)
    return (0); /* global write access */

  is_owner = FALSE;
  if (id_key) {
    key = shfs_access_owner_get(file);
    if (!key)
      return (0);

    is_owner = shkey_cmp(id_key, key);
    shkey_free(&key);
  }
  if (!is_owner)
    return (SHERR_ACCESS);

  return (0);
}
int shfs_access_exec(shfs_ino_t *file, shkey_t *id_key)
{
  shkey_t *key;
  int is_owner;

  if (shfs_attr(file) & SHATTR_EXE)
    return (0); /* global execute access */

  is_owner = FALSE;
  if (id_key) {
    key = shfs_access_owner_get(file);
    if (!key)
      return (0);

    is_owner = shkey_cmp(id_key, key);
    shkey_free(&key);
  }
  if (!is_owner)
    return (SHERR_ACCESS);

  return (0);
}

int shfs_access_owner_set(shfs_ino_t *file, shkey_t *id_key)
{
  if (!id_key)
    id_key = ashkey_blank();
  memcpy(&file->blk.hdr.owner, id_key, sizeof(shkey_t));
}

shkey_t *shfs_access_owner_get(shfs_ino_t *file)
{
  static shkey_t ret_key;

  if (shkey_cmp(&file->blk.hdr.owner, ashkey_blank()))
    return (NULL); /* public */

  memcpy(&ret_key, &file->blk.hdr.owner, sizeof(shkey_t));
  return (&ret_key);
}

#if 0
int shfs_access_owner_set(shfs_ino_t *file, shkey_t *id_key)
{
  int err;

  if (!id_key) {
    /* public ownership */
    shfs_attr_set(file, SHATTR_READ);
    shfs_attr_set(file, SHATTR_WRITE);
    shfs_attr_set(file, SHATTR_EXE);
    return (0);
  }

  err = shfs_obj_set(file, "access/owner", id_key);
  if (err)
    return (err);

  return (0);
}

shkey_t *shfs_access_owner_get(shfs_ino_t *file)
{
  int err;
  shkey_t *key;

  err = shfs_obj_get(file, "access/owner", &key);
  if (err)
    return (NULL);

  return (key);
}
#endif
