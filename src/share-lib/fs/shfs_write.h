
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
 *  @file shfs_write.h
 *  @ingroup libshare_fs
*/  

#ifndef __FS__SHFS_WRITE_H__
#define __FS__SHFS_WRITE_H__

int shfs_write_mem(char *path, void *data, size_t data_len);

/**
 * Writes the file contents of the inode to the file stream.
 * @param tree The sharefs filesystem partition allocation by @c shfs_init().
 * @param inode The sharefs filesystem inode to print from.
 * @param fd A posix file descriptor number representing a socket or local filesystem file reference.
 * @returns A zero (0) on success and a negative one (-1) on error.
 * On error one of the following error codes will be set:
 *   EBADF  fd is not a valid file descriptor or is not open for writing.
 */ 
int shfs_write_print(shfs_t *tree, shfs_ino_t *inode, int fd); 

#endif /* ndef __FS__SHFS_WRITE_H__ */


