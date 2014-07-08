
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

#ifndef __FS__SHFS_AUX_H__
#define __FS__SHFS_AUX_H__

/**
 * @addtogroup libshare_fs
 * @{
 */

/*
int shfs_aux_read(shfs_ino_t *file, shbuf_t *buff);
int shfs_aux_write(shfs_ino_t *file, shbuf_t *buff);
uint64_t shfs_aux_crc(shfs_ino_t *file);
*/

/**
 * Stores a data segment to a sharefs filesystem inode.
 * @param inode The inode whose data is being retrieved.
 * @param buff The data segment to write to the inode.
 * @returns The number of bytes written on success, and a (-1) if the file cannot be written to.
 * @note A inode must be linked before it can be written to.
 */
int shfs_aux_write(shfs_ino_t *inode, shbuf_t *buff);
//int shfs_inode_write(shfs_ino_t *inode, shbuf_t *buff);

/**
 * Retrieve a data segment of a sharefs filesystem inode.
 * @param tree The sharefs partition allocated by @c shfs_init().
 * @param inode The inode whose data is being retrieved.
 * @param ret_buff The @c shbuf_t return buffer.
 * @param data_of The offset to begin reading data from the inode.
 * @param data_len The length of data to be read.
 * @returns The number of bytes read on success, and a (-1) if the file does not exist.
 */
int shfs_aux_read(shfs_ino_t *inode, shbuf_t *ret_buff);

/**
 * Writes the auxillary contents of the inode to the file descriptor.
 * @param inode The sharefs filesystem inode to print from.
 * @param fd A posix file descriptor number representing a socket or local filesystem file reference.
 * @returns The size of the bytes written or a SHERR_XX error code on error.
 * On error one of the following error codes will be set:
 *   SHERR_BADF  fd is not a valid file descriptor or is not open for writing.
 */ 
ssize_t shfs_aux_pipe(shfs_ino_t *inode, int fd);

uint64_t shfs_aux_crc(shfs_ino_t *inode);

/**
 * @}
 */

#endif /* ndef __FS__SHFS_AUX_H__ */

