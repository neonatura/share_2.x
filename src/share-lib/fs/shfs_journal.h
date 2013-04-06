
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
 */

#ifndef __FS__SHFS_JOURNAL_H__
#define __FS__SHFS_JOURNAL_H__

/**
 * @addtogroup libshare_fs
 * @{
 */

/**
 * The number of journals a sharefs filesystem contains.
 * @seealso shfs_journal_t.index
 */
#define SHFS_MAX_JOURNAL 65536

/**
 * A single block of data inside a journal.
 * @seealso shfs_journal_t.data
 */
typedef uint8_t shfs_block_t[SHFS_BLOCK_SIZE];

/**
 * A memory segment containing a journal's data.
 */
typedef struct shfs_journal_data_t {
  /**
   * The journal's memory segment augmented into blocks.
   */
  shfs_block_t block[SHFS_MAX_BLOCK];  
} shfs_journal_data_t;

/**
 * A sharefs filesystem journal.
 * Each partition is composed of @c SHFS_MAX_JOURNAL journals.
 */
typedef struct shfs_journal_t {

  /**
   * The index number of the journal. 
   * This value ranges from 0 to ( @c SHFS_MAX_JOURNAL - 1 ).
   */
  int index;

  /**
   * The data segment of the journaled sharefs file system.
   */
  shfs_journal_data_t *data;

  /**
   * The currently allocated size of the journal.
   */
  size_t data_max;

  /**
   * The file descriptor used to map the journal's memory segment to the local filesystem.
   */
  int data_fd;

} shfs_journal_t;

/**
 * Initializes a sharefs filesystem journal for use.
 */
shfs_journal_t *shfs_journal(shfs_t *tree, int index);

/**
 * @}
 */

#endif /* ndef __FS__SHFS_JOURNAL_H__ */