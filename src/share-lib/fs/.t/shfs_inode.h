
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
 *
 *  @file shfs_inode.h
*/  

#ifndef __FS__SHFS_INODE_H__
#define __FS__SHFS_INODE_H__

/**
 * @addtogroup libshare_fs
 * @{
 */


/**
 * Retrieve a sharefs inode directory entry based on a given parent inode and path name.
 * @note Searches for a reference to a sharefs inode labelled "name" in the @a parent inode.
 * @note A new inode is created if a pre-existing one is not found.
 * @param parent The parent inode such as a directory where the file presides.
 * @param name The relational pathname of the file being referenced.
 * @param mode The type of information that this inode is referencing (SHINODE_XX).
 * @returns A @c shfs_node is returned based on the @c parent, @c name, @c and mode specified. If one already exists it will be returned, and otherwise a new entry will be created.
 * @note A new inode will be linked to the sharefs partition if it does not exist.
 */
shfs_ino_t *shfs_inode(shfs_ino_t *parent, char *name, int mode);

/**
 * Obtain the shfs partition associated with a particular inode.
 * @param The inode in reference.
 */
shfs_t *shfs_inode_tree(shfs_ino_t *inode);

/**
 * Obtain the root partition inode associated with a particular inode.
 * @param The inode in reference.
 */
shfs_ino_t *shfs_inode_root(shfs_ino_t *inode);

/**
 * Obtain the parent [directory/container] inode associated with a particular inode.
 * @param The inode in reference.
 */
shfs_ino_t *shfs_inode_parent(shfs_ino_t *inode);

/**
 * Write an entity such as a file inode.
 */
int shfs_inode_write_entity(shfs_ino_t *ent);

/**
 * Writes a single inode block to a sharefs filesystem journal.
 */
int shfs_inode_write_block(shfs_t *tree, shfs_block_t *blk);


/**
 * Retrieve a single data block from a sharefs filesystem inode. 
 * @param tree The sharefs partition allocated by @c shfs_init().
 * @param inode The inode whose data is being retrieved.
 * @param hdr A specification of where the block is location in the sharefs filesystem partition.
 * @param inode The inode block data to be filled in.
 * @returns Returns 0 on success and a SHERR_XXX on failure.
 */
int shfs_inode_read_block(shfs_t *tree, shfs_idx_t *pos, shfs_block_t *blk);


/**
 * Returns a unique key token representing an inode.
 * @param parent The parent inode of the inode being referenced.
 */
shkey_t *shfs_inode_token(shfs_ino_t *parent, int mode, char *fname);

/**
 * Assign an inode a filename.
 */
void shfs_inode_filename_set(shfs_ino_t *inode, char *name);

/**
 * Returns the filename of the inode.
 */
char *shfs_inode_filename_get(shfs_ino_t *inode);

char *shfs_inode_path(shfs_ino_t *inode);

/**
 * A unique hexadecimal string representing a sharefs inode.
 */
char *shfs_inode_id(shfs_ino_t *inode);



char *shfs_inode_print(shfs_ino_t *inode);
char *shfs_inode_block_print(shfs_block_t *jblk);

/** 
 * @example shfs_inode_mkdir.c
 * Example of making a new directory from the base directory of the default local sharefs partition.
 * @example shfs_inode_remote_copy.c 
 * Example of copying a remote file to the local filesystem's current directory.
 * @example shfs_inode_remote_link.c
 * Example of creating a local sym-link to a remote sharefs file.
 */

/**
 * @}
 */

#endif /* ndef __FS__SHFS_INODE_H__ */

