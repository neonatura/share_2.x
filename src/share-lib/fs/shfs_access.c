
/*
 *  Copyright 2013 Brian Burrell 
 *
 *  This file is part of the Share Library.
 *  (https://github.com/briburrell/share)
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

int shfs_access_user(shfs_ino_t *inode, shkey_t *user, int flag)
{
  uint64_t user_uid = shkey_crc(user);
  uint64_t ino_uid;

  ino_uid = shfs_uid(inode, flag);
  if (!ino_uid)
    return (0); /* public */
  
  if (user_uid == ino_uid)
    return (0); /* owner */

  return (SHERR_ACCESS);
}

int shfs_access_group(shfs_ino_t *inode, shkey_t *user, int flag)
{
  uint64_t uid;
  uint64_t gid;

  gid = shfs_gid(inode, flag);

  if (gid == 0)
    return (0);

  uid = shcrc(user, sizeof(shkey_t)) % MAX_SHAREFS_GROUPS;
  if ((uid % gid) == (gid % uid))
    return (0);

  return (SHERR_ACCESS);
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


