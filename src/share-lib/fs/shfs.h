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
 *  @file shfs.h
 */

#ifndef __FS__SHFS_H__
#define __FS__SHFS_H__

/**
 * The sharefs file system overlays ontop of your current filesystem in order to provide extended file operations.
 * @brief The sharefs file system.
 * @ingroup libshare
 * @defgroup libshare_fs The 'sharefs' file-system.
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
 * Disabling caching and asynchronous file operations.
 */
#define SHFS_SYNC           (1 << 3)

/**
 * @}
 */

/**
 * The SHFS_MAX_BLOCK definition specifies the maximum number of sharefs data blocks are contained in a single journal.
 */
#define SHFS_MAX_BLOCK 65536


/**
 * A type defintion for the sharefs filesytem structure.
 */
typedef struct shfs_t shfs_t;




/**
 * @defgroup libshare_fs_inode The 'sharefs' inode sub-system. 
 * @{
 */

/**
 * Inode is in reference to an application-specific directory.
 * @note See also: @c shfs_node.d_type
 */
#define SHINODE_APP          100

/**
 * Inode is the root of an entire sharefs partition.
 * @note See also: @c shfs_node.d_type
 */
#define SHINODE_PARTITION     101

/**
 * Inode is a reference to a remote sharefs partition.
 * @note See also: @c shfs_node.d_type
 */
#define SHINODE_PEER          102

/**
 * Inode is a reference to a binary delta revision.
 * @note See also: @c shfs_node.d_type
 */
#define SHINODE_DELTA         103

/**
 * An archive of files and/or directories.
 * @note See also: @c shfs_node.d_type
 */
#define SHINODE_ARCHIVE       104

/**
 * A reference to another inode.
 * @note The referenced inode may be local or remote.
 */
#define SHINODE_REFERENCE     105

/**
 * A meta definition hashmap (meta map).
 * @note The referenced inode may be local or remote.
 */
#define SHINODE_META          106

/**
 * A directory containing multiple file references.
 */
#define SHINODE_DIRECTORY     107

/**
 * The maximum size a single block can contain.
 * @note Each block segment is 1024 bytes which is equal to the size of @c shfs_ino_t structure. Blocks are kept at 1k in order to reduce overhead on the IP protocol.
 */
#define SHFS_BLOCK_SIZE 1024

/**
 * The size of the data segment each inode contains.
 * @note 992 = (@c SHFS_BLOCK_SIZE - sizeof(@c shfs_hdr_t))
 */
#define SHFS_BLOCK_DATA_SIZE 992

/**
 * The maximum number of blocks in a sharefs journal.
 */
#define SHFS_MAX_BLOCK 65536

/**
 * The number of journals a sharefs filesystem contains.
 */
#define SHFS_MAX_JOURNAL 65536

/**
 * The total byte size of a sharefs filesystem journal.
 */
#define SHFS_MAX_JOURNAL_SIZE (SHFS_MAX_BLOCK * SHFS_BLOCK_SIZE)

/**
 * A sharefs filesystem inode or journal reference.
 */
typedef __uint16_t shfs_inode_off_t;

/**
 * A sharefs inode type definition.
 */
typedef __uint32_t shfs_ino_type_t;

/**
 * A sharefs inode data size definition.
 */
typedef __uint64_t shfs_size_t;

/**
 * A sharefs inode data checksum type definition.
 */
typedef __uint32_t shfs_crc_t;

typedef struct shfs_inode_hdr_t
{
  /**
   * The size of the block that the data segment fills.
   */ 
  shfs_inode_off_t d_size;

  /**
   * The journal that the data segment presides on.
   */ 
  shfs_inode_off_t d_jno;

  /**
   * An inode index in journal to initial data block.
   */ 
  shfs_inode_off_t d_ino;             
} shfs_inode_hdr_t;

/**
 * A sharefs filesystem inode header.
 */
typedef struct shfs_hdr_t {

  /* 64b */

  /**
   * The total size of the data segment being referenced.
   */
  shfs_size_t d_size;

  /**
   * The last time this inode was written to.
   */
  shtime_t d_stamp;

  /* 32b */

  /**
   * Type of inode (SHINODE_XX).
   */
  shfs_ino_type_t d_type;   

  /**
   * A adler32 checksum of the data segment being referenced.
   */
  shfs_crc_t d_crc;

  uint32_t __reserved_2__;

  /* 16b */

  /**
   * The journal number where the inode presides.
   */ 
  shfs_inode_off_t d_jno;             

  /**
   * An inode index in journal to initial data block.
   */ 
  shfs_inode_off_t d_ino;             
} shfs_hdr_t;

/**
 * A sharefs filesystem inode.
 */
typedef struct shfs_ino_t 
{
  /**
   * Inode definition 
   */
  shfs_hdr_t hdr;

  /**
   * The data segment of the inode block.
   */
  union {
    /**
     * The name of the file being referenced by the inode.
     */
    char  name[SHFS_BLOCK_DATA_SIZE];
    /**
     * Binary data content.
     */
    char bin[SHFS_BLOCK_DATA_SIZE];
  } d_raw;

} shfs_ino_t;


/**
 * @}
 */







/**
 * The sharefs filesystem structure.
 * @note See also: @c shfs_ino_t
 */
struct shfs_t {
  /**
   * The flags associated with the sharefs partition.
   */
  int flags;

  /**
   * A remote peer associated with the sharefs file-system.
   */
  shpeer_t peer;

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
 * Strips the absolute parent from @a app_name
 * @note "/test/one/two" becomes "two"
 * @param app_name The running application's executable path
 * @returns Relative filename of executable.
 */
char *shfs_app_name(char *app_name);

/**
 * Creates a reference to a sharefs filesystem.
 * @a app_name The application's executable name.
 * @a flags A combination of SHFS_XXX flags.
 * @returns shfs_t The sharefs filesystem.
 */
shfs_t *shfs_init(char *app_name, int flags);

/**
 * Free a reference to a sharefs partition.
 * @param tree_p A reference to the sharefs partition instance to free.
 */
void shfs_free(shfs_t **tree_p);

/**
 * @}
 */

#endif /* ndef __FS__SHFS_H__ */
