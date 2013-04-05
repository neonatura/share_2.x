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
 *  @file share.h 
 *  @brief The sharefs file system operations. 
 */

#ifndef __FS__SHFS_H__
#define __FS__SHFS_H__

/**
 * @defgroup sh_fs The 'sharefs' file-system.
 * @{
 */


#define SHFS_LEVEL_PUBLIC 0
#define SHFS_MAX_LEVELS 1

#ifndef NAME_MAX
#define NAME_MAX 4095
#endif

#ifndef PATH_MAX
#define PATH_MAX NAME_MAX
#endif

/**
 *  Filesystem Modes 
 *  @defgroup libshare_fs_mode The sharefs file system modes. 
 *  @{
 */

/**
 * Overlay sharefs on top of current filesystem.
 * @note Use 'shnet --nosync' for example behavior of this flag.
 */ 
#define SHFS_OVERLAY        (1 << 0)

/**
 * Track all revisions of file modifications.
 * @note Use 'shnet --track' for example behavior of this flag.
 */
#define SHFS_TRACK          (1 << 1)

/**
 * A sharefs filesystem that is externally unaccessible beyond 
 * the scope of this application.
 * @note Use 'shnet --hidden' for example behavior of this flag.
 */
#define SHFS_PRIVATE        (1 << 2)

/**
 * @}
 */

/**
 * The SHFS_MAX_BLOCK definition specifies the maximum number of sharefs data blocks are contained in a single journal.
 */
#define SHFS_MAX_BLOCK 65536

/**
 * The size of a single sharefs inode data block segment.
 * @note The size of a @c shfs_ino_t.
 */
#define SHFS_BLOCK_SIZE 4128

/**
 * A type defintion for the sharefs filesytem structure.
 */
typedef struct shfs_t shfs_t;

#include "shfs_inode.h"

/**
 * The sharefs filesystem structure.
 * @seealso shfs_ino_t
 */
struct shfs_t {
  /**
   * The flags associated with the sharefs partition.
   */
  int flags;

  /**
   * Root directory.
   */
  shfs_ino_t *base_ino;

  /**
   * Application's current working directory.
   */
  shfs_ino_t *cur_ino; 
};

/**
 * Creates a reference to a sharefs filesystem.
 * @a app_name The application's executable name.
 * @a flags A combination of SHFS_XXX flags.
 * @returns shfs_t The sharefs filesystem.
 */
shfs_t *shfs_init(char *app_name, int flags);

/**
 * Search for a reference to a sharefs inode labelled "name" in the @a parent inode.
 * @note A new inode is created if a pre-existing one is not found.
 * @a parent The location where the inode presides.
 * @a name The label name of the inode.
 * @a mode A particular inode mode (SHINO_XXX).
 * @returns A sharefs inode reference.
 */
shfs_ino_t *shfs_ino_t_entry(shfs_ino_t *parent, char *name, int mode);

/**
 * Strips the absolute parent from @a app_name
 * @note "/test/one/two" becomes "two"
 * @param app_name The running application's executable path
 * @returns Relative filename of executable.
 */
char *shfs_app_name(char *app_name);

#endif /* ndef __FS__SHFS_H__ */
