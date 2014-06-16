
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

#ifndef __FS__SHFS_FILE_H__
#define __FS__SHFS_FILE_H__

/**
 * @addtogroup libshare_fs
 * @{
 */

int shfs_file_write(shfs_ino_t *file, void *data, size_t data_len);

int shfs_file_read(shfs_ino_t *file, void **data_p, size_t *data_len_p);

shfs_ino_t *shfs_file_find(shfs_t *tree, char *path);

/**
 * @}
 */

#endif /* ndef __FS__SHFS_FILE_H__ */

