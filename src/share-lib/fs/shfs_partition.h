/*
 * @copyright
 *
 *  Copyright 2013 Brian Burrell 
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
 */

#ifndef __FS__SHFS_PARTITION_H__
#define __FS__SHFS_PARTITION_H__

/**
 * @addtogroup libshare_fs
 * @{
 */


/**
 * Creates a reference to a sharefs filesystem.
 * @param peer A local or remote reference to a sharefs partition.
 * @a flags A combination of SHFS_PARTITION_XXX flags.
 * @returns shfs_t A share partition associated with the peer specified or the local default partition if a NULL peer is specified.
 * @todo write local file '/system/version' with current version.
 */
shfs_t *shfs_init(shpeer_t *peer);

/**
 * Free a reference to a sharefs partition.
 * @param tree_p A reference to the sharefs partition instance to free.
 */
void shfs_free(shfs_t **tree_p);

/**
 * Obtain the partition id for a sharefs partition.
 * @note The local parition will always return zero (0).
 */
shkey_t *shfs_partition_id(shfs_t *tree);

/**
 * @}
 */

#endif /* ndef __FS__SHFS_PARTITION_H__ */


