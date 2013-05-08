

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
 *  @file shfs_inode.h
*/  

#ifndef __FS__SHFS_DIR_H__
#define __FS__SHFS_DIR_H__

/**
 * @addtogroup libshare_fs
 * @{
 */

/**
 * The base SHINODE_PARTITION type inode for a sharefs partition.
 */
shfs_ino_t *shfs_dir_base(shfs_t *tree);

/**
 * The current working inode directory for a sharefs partition.
 */
shfs_ino_t *shfs_dir_cwd(shfs_t *tree);

/**
 * @returns The SHINODE_DIRECTORY parent of an inode.
 */
shfs_ino_t *shfs_dir_parent(shfs_ino_t *inode);

/**
 * Return an inode from a directory inode.
 */
shfs_ino_t *shfs_dir_entry(shfs_ino_t *inode, char *fname);

/**
 * Locate a directory inode on a sharefs partition by an absolute pathname. 
 */
shfs_ino_t *shfs_dir_find(shfs_t *tree, char *path);

/**
 * @}
 */

#endif /* ndef __FS__SHFS_DIR_H__ */

