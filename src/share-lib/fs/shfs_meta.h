
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
 *  @file shfs_meta.h
 */

#ifndef __FS__META_H__
#define __FS__META_H__

/**
 * @addtogroup libshare_fs
 * @{
 */

#define SHMETA_READ   (1 << 0)
#define SHMETA_WRITE  (1 << 1)
#define SHMETA_EXEC   (1 << 2)
/**
 * The read-access group assigned to the inode.
 */
#define SHMETA_USER   (1 << 10)
#define SHMETA_GROUP  (1 << 11)

/**
 * A textual description of the inode.
 */
#define SHMETA_DESC   (1 << 20)


/**
 * Obtain a reference to the meta definition hashmap associated with the inode entry.
 * @note The @c shfs_ino_t inode will cache the hashmap reference.
 * @param ent The inode entry.
 * @param val_p A memory reference to the meta definition hashmap being filled in.
 */
int shfs_meta(shfs_t *tree, shfs_ino_t *ent, shmeta_t **val_p);

/**
 * Flush the inode's meta map to disk.
 * @param The inode associated with the meta map.
 * @param val The meta map to store to disk.
 * @returns A zero (0) on success and a negative one (-1) on failure.
 */
int shfs_meta_save(shfs_t *tree, shfs_ino_t *ent, shmeta_t *h);

/**
 * Free an instance to a sharedfs meta definition hashmap.
 * @note Directly calls @c shmeta_free().
  */
#define shfs_meta_free(_meta_p) shmeta_free(_meta_p)

int shfs_meta_set(shfs_ino_t *file, int def, char *value);
char *shfs_meta_get(shfs_ino_t *file, int def);

int shfs_meta_perm(shfs_ino_t *file, int def, shkey_t *user);

/**
 * @}
 */

#endif /* ndef __FS__META_H__ */

