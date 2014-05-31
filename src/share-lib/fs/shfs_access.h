
/*
 * @copyright
 *
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
 *
 *  @endcopyright
 *
 */

#ifndef __FS__SHFS_GROUP_H__
#define __FS__SHFS_GROUP_H__

/**
 * @addtogroup libshare_fs
 * @{
 */

#define SHFS_MAX_GROUPS 57344


/**
 * A 64bit user id associated with a read, write, or exec inode permission.
 */
#define shfs_uid(_inode, _flag) \
  (strtoll(shfs_meta_get((_inode), (_flag) | SHMETA_USER)))

/**
 * A 64bit group id associated with a read, write, or exec inode permission.
 */
#define shfs_gid(_inode) \
  (strtoll(shfs_meta_get((_inode), (_flag) | SHMETA_GROUP)))

/**
 * Performs a check to see whether a user has a particular permission to an inode.
 */
int shfs_access(shfs_ino_t *inode, shkey_t *user, int flag);

int shfs_access_user(shfs_ino_t *inode, shkey_t *user, int flag);

int shfs_access_group(shfs_ino_t *inode, shkey_t *user, int flag);

/**
 * @}
 */

#endif /* ndef __FS__SHFS_GROUP_H__ */

