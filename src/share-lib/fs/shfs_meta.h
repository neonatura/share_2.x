
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
 *  @file shfs_meta.h
 */

#ifndef __FS__META_H__
#define __FS__META_H__

/**
 * @addtogroup libshare_fs
 * @{
 */

typedef struct shfs_def {
  /** last update timestamp. */
  uint64_t stamp;           

  /** inode number */
  long  d_ino;

  /** reference to running process */
  uint32_t pid;

  /** offset to this old_linux_dirent */
  off_t d_off; 
} shfs_def;

int shfs_meta(shfs_ino_t *ent, shfs_def **meta_p);
int shfs_meta_save(shfs_ino_t *ent, shfs_def *def);
int shfs_meta_free(shfs_def **meta_p);

/**
 * @}
 */

#endif /* ndef __FS__META_H__ */

