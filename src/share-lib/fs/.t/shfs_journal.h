
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
 */

#ifndef __FS__SHFS_JOURNAL_H__
#define __FS__SHFS_JOURNAL_H__

/**
 * @addtogroup libshare_fs
 * @{
 */


/**
 * The local file-system path where a sharefs journal is stored.
 */
char *shfs_journal_path(shfs_t *tree, int index);

/**
 * Returns an instance to a sharefs filesystem journal.
 */
shfs_journal_t *shfs_journal_open(shfs_t *tree, int index);


/**
 * Search for the first empty inode entry in a journal.
 * @param tree The sharefs filesystem partition.
 * @param key The token name of the inode being referenced.
 * @param idx The index number of the journal.
 * @returns A inode index number or zero (0) on failure.
 * @note Inode index #0 is reserved for system use.
 */
int shfs_journal_scan(shfs_t *tree, shkey_t *key, shfs_idx_t *idx);

/**
 * Release all resources being used to reference a shared partition journal.
 * @param jrnl_p A reference to the journal.
 * @returns A zero (0) on success and a negative error code on failure.
 */
int shfs_journal_close(shfs_journal_t **jrnl_p);

/**
 * Retrieve an inode block from a journal.
 */
shfs_block_t *shfs_journal_block(shfs_journal_t *jrnl, int ino);

/**
 * Calculates the byte size of a sharefs partition journal.
 */
size_t shfs_journal_size(shfs_journal_t *jrnl);


/**
 * @}
 */

#endif /* ndef __FS__SHFS_JOURNAL_H__ */
