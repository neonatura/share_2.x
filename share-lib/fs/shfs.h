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

/* Filesystem Modes */

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
 * The 'sharefs' inode subsystem.
 * @defgroup sh_fs_inode The 'sharefs' inode subsystem. 
 * @{
 */

/**
 * Inode is in reference to an application-specific directory.
 */
#define SHINO_APP_ID        100

/**
 * Inode is the root of an entire sharefs partition.
 */
#define SHINO_PARTITION     101

/**
 * Inode is a reference to a remote sharefs partition.
 */
#define SHINO_PEER          102

/**
 * A sharefs inode reference definition.
 */
typedef __uint64_t shfs_ino;

/**
 * A sharefs inode type definition.
 */
typedef __uint32_t shfs_ino_type;

/**
 * A sharefs filesystem inode.
 */
typedef struct shfs_node {
  shfs_ino d_ino;              /* inode number */
  shfs_ino_type d_type;     /* inode type */
  off_t d_off;              /* offset to this old_linux_dirent */
  unsigned short d_reclen;  /* length of this d_name */
  union {
    struct in_addr addr;
    struct in6_addr addr6;
  } peer;
  char  d_name[NAME_MAX+1]; /* filename (null-terminated) */
} shfs_node;

/**
 * @}
 */

/**
 * The sharefs filesystem structure.
 * @seealso shfs_node
 */
typedef struct shfs_tree {
  shfs_ino d_ino; /* root directory */
  shfs_ino d_cwd_ino; /* current work directory */
  svn_repos_t *svn_repo;
  apr_pool_t *svn_pool;
} shfs_tree;
#define shfs_public_root root[SHFS_LEVEL_PUBLIC]


/**
 * Creates a reference to a sharefs filesystem.
 * @a app_name The application's executable name.
 * @a flags A combination of SHFS_XXX flags.
 * @returns shfs_tree The sharefs filesystem.
 */
shfs_tree *shfs_init(char *app_name, int flags);

/**
 * Search for a reference to a sharefs inode labelled "name" in the @a parent inode.
 * @note A new inode is created if a pre-existing one is not found.
 * @a parent The location where the inode presides.
 * @a name The label name of the inode.
 * @a mode A particular inode mode (SHINO_XXX).
 * @returns A sharefs inode reference.
 */
shfs_node *shfs_node_entry(shfs_node *parent, char *name, int mode);

/**
 * Strips the absolute parent from @a app_name
 * @note "/test/one/two" becomes "two"
 * @param app_name The running application's executable path
 * @returns Relative filename of executable.
 */
char *shfs_app_name(char *app_name);

#endif /* ndef __FS__SHFS_H__ */
