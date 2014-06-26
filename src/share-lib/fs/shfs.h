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

#ifndef __MEM__SHMEM_H__
#include "shmem.h"
#endif

/**
 * @ingroup libshare
 * @defgroup libshare_fs The share library 'sharefs' file system.
 * @brief The sharefs file system.
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
 *  @xxxdefgroup libshare_fs_mode The sharefs file system modes. 
 *  @addtogroup libshare_fs
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
 * The partition is located on a remote machine.
 */
#define SHFS_REMOTE         (1 << 4)

/**
 * @}
 */


/**
 * A type defintion for the sharefs filesytem structure.
 */
typedef struct shfs_t shfs_t;



/**
 * A sharefs filesystem inode.
 */
typedef struct shfs_ino_t shfs_ino_t;


/**
 * @xxxdefgroup libshare_fs_inode The 'sharefs' inode sub-system. 
 * @addtogroup libshare_fs
 * @{
 */

/**
 * An inode reference to nothing.
 */
#define SHINODE_NULL          0

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
 * An auxillary unparseable data segment stored in the sharefs sub-system.
 */
#define SHINODE_AUX           108

/**
 * A generic reference to a path which contains further references to data.
 * @see SHINODE_AUX SHINODE_META SHINODE_DELTA
 */
#define SHINODE_FILE          109 


#define IS_INODE_CONTAINER(_type) \
  (_type != SHINODE_AUX && _type != SHINODE_DELTA && _type != SHINODE_ARCHIVE)

/**
 * The maximum size a single block can contain.
 * @note Each block segment is 1024 bytes which is equal to the size of @c shfs_ino_t structure. Blocks are kept at 1k in order to reduce overhead on the IP protocol.
 */
#define SHFS_BLOCK_SIZE 1024

/**
 * The size of the data segment each inode contains.
 */
#define SHFS_BLOCK_DATA_SIZE (SHFS_BLOCK_SIZE - sizeof(shfs_hdr_t))

/**
 * The maximum number of blocks in a sharefs journal.
 */
#define SHFS_MAX_BLOCK 57344

/**
 * The maximum length of a sharefs file name.
 * @note The length is subtracted by 16 bytes of a hash tag incase to track longer filenames and 1 byte for a null-terminator.
 */
#define SHFS_PATH_MAX (SHFS_BLOCK_DATA_SIZE - 34)

/**
 * A sharefs filesystem inode or journal reference.
 */
typedef __uint16_t shfs_inode_off_t;

/**
 * A sharefs inode type definition.
 */
typedef __uint32_t shfs_ino_type_t;


/**
 * A sharefs filesystem inode position header.
 */
typedef struct shfs_idx_t shfs_idx_t;

struct shfs_idx_t 
{

  /**
   * The journal number where the inode presides.
   */ 
  shfs_inode_off_t jno;             

  /**
   * An inode index in journal to initial data block.
   */ 
  shfs_inode_off_t ino;             

}; /* 4b */

typedef struct shfs_hdr_t shfs_hdr_t;

/**
 * A sharefs filesystem inode.
 * @see shfs_meta() SHINODE_DIRECTORY
 */
struct shfs_hdr_t 
{

  /**
   * A adler32 checksum reference of the inode.
   */
  shkey_t name;

  /**
   * The total size of the data segment being referenced.
   */
  shsize_t size;

  /**
   * The last time this inode was written to.
   */
  shtime_t time;

  /**
   * The last time this inode was written to.
   */
  shtime_t mtime;

  /**
   * Type of inode (SHINODE_XX).
   */
  shfs_ino_type_t type;

  uint64_t crc;

  /**
   * Inode position in the partition.
   */
  shfs_idx_t pos;

  /**
   * The position of the next inode in a chain.
   */
  shfs_idx_t npos;

  /**
   * The position of the first inode in a chain.
   */
  shfs_idx_t fpos;

}; /* 64b */

typedef struct shfs_block_t shfs_block_t;


/**
 * A convienence macro for accessing a sharefs file partition.
 */
typedef struct shfs_t SHFS;
/**
 * A convienence macro for accessing a sharefs file node.
 */
typedef struct shfs_ino_t SHFL;


struct shfs_block_t 
{

  /**
   * The definition header of the inode block. */
  shfs_hdr_t hdr;

  /**
   * The data segment of the inode block.
   */
  unsigned char raw[SHFS_BLOCK_DATA_SIZE];

};

struct shfs_ino_t 
{

  /**
   * A 1k block of data stored on the sharefs partition.
   */
  shfs_block_t blk;

  /**
   * The sharefs partition this inode is a part of.
   */
  shfs_t *tree;

  /**
   * The parent inode containing this inode.
   * @note The root inode will have a parent of NULL.
   * @note This variable is not saved as part of the fileystem inode.
   */
  struct shfs_ino_t *parent;

  /**
   * The root directory inode of the partition.
   * @note The root inode is self-circular for the root inode.
   * @note This variable is not saved as part of the fileystem inode.
   */
  struct shfs_ino_t *base;

  /**
   * Inode entities that are contained inside this [directory] inode.
   */
  shmeta_t *cmeta;

  /**
   * Primary meta definitions associated with the inode.
   */
  shmeta_t *meta;

  /**
   * Type-specific allocated memory pool for inode.
   */
  unsigned char *pool;

};


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
   * The machine related to the sharefs inode's partition.
   * @note This variable is not saved as part of the fileystem inode.
   */
  shpeer_t *peer;

  /**
   * Root directory.
   */
  shfs_ino_t *base_ino;

  /**
   * Application's current working directory.
   */
  shfs_ino_t *cur_ino; 

  /**
   * Root partition inode (supernode).
   * @note This inode references the root directory of a partition.
   */
  shfs_ino_t p_node;

  /**
   * Application's package title
   */
  char app_name[NAME_MAX+1];
};

/**
 * Strips the absolute parent from @a app_name
 * @note "/test/one/two" becomes "two"
 * @param app_name The running application's executable path
 * @returns Relative filename of executable.
 */
char *shfs_app_name(char *app_name);


/* supplemental includes */
#include "fs/shfs_partition.h"
#include "fs/shfs_journal.h"
#include "fs/shfs_inode.h"
#include "fs/shfs_link.h"
#include "fs/shfs_dir.h"
#include "fs/shfs_meta.h"
#include "fs/shfs_read.h"
#include "fs/shfs_write.h"
#include "fs/shfs_file.h"
#include "fs/shfs_rev.h"
#include "fs/shfs_access.h"
#include "fs/shfs_cache.h"
#include "fs/shfs_aux.h"

/**
 * @}
 */

#endif /* ndef __FS__SHFS_H__ */


