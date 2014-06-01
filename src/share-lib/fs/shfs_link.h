
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
*/  

#ifndef __FS__SHFS_LINK_H__
#define __FS__SHFS_LINK_H__

/**
 * @addtogroup libshare_fs
 * @{
 */



/**
 * Link a child inode inside a parent's directory listing.
 * @note The birth timestamp and token key is assigned on link.
 */
int shfs_link(shfs_ino_t *parent, shfs_ino_t *inode);

/**
 * Unlink an inode from a sharefs partition.
 * @note This effectively deletes the inode.
 */
int shfs_unlink(shfs_ino_t *inode);

/**
 * Find an inode in it's parent using it's key name.
 */
int shfs_link_find(shfs_ino_t *parent, shkey_t *key, shfs_block_t *ret_blk);

/**
 * Print all entries in a directory.
 */
int shfs_link_list(shfs_ino_t *parent, shbuf_t *buff);

/**
 * @}
 */

#endif /* ndef __FS__SHFS_LINK_H__ */

