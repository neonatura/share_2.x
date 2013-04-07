
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
 */
shfs_ino_t *shfs_inode(shfs_ino_t *parent, char *name, int mode);

/**
 * Writes a single inode block to a sharefs filesystem journal.
 */
int shfs_inode_write_block(shfs_t *tree, shfs_hdr_t *scan_hdr, shfs_hdr_t *hdr, char *data, size_t data_len);

/**
 * Stores a data segment to a sharefs filesystem inode.
 * @param tree The sharefs partition allocated by @c shfs_init().
 * @param inode The inode whose data is being retrieved.
 * @param data The data segment to write to the inode.
 * @param data_of The offset to begin reading data from the inode.
 * @param data_len The length of data to be read.
 * @returns The number of bytes written on success, and a (-1) if the file cannot be written to.
 */
ssize_t shfs_inode_write(shfs_t *tree, shfs_ino_t *inode, char *data, shfs_size_t data_of, shfs_size_t data_len);

/**
 * Retrieve a single data block from a sharefs filesystem inode. 
 * @param tree The sharefs partition allocated by @c shfs_init().
 * @param inode The inode whose data is being retrieved.
 * @param hdr A specification of where the block is location in the sharefs filesystem partition.
 * @param inode The inode block data to be filled in.
 * @returns Returns 0 on success and -1 on failure. Check the errno for additional information.
 */
int shfs_inode_read_block(shfs_t *tree, shfs_hdr_t *hdr, shfs_ino_t *inode);

/**
 * Retrieve a data segment of a sharefs filesystem inode.
 * @param tree The sharefs partition allocated by @c shfs_init().
 * @param inode The inode whose data is being retrieved.
 * @param ret_buff The @c shbuf_t return buffer.
 * @param data_of The offset to begin reading data from the inode.
 * @param data_len The length of data to be read.
 * @returns The number of bytes read on success, and a (-1) if the file does not exist.
 */
ssize_t shfs_inode_read(shfs_t *tree, shfs_ino_t *inode, shbuf_t *ret_buff, size_t data_of, size_t data_len);

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

