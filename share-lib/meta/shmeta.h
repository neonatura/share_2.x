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
 *  @brief The Share Library
 *  Provides: Hashmap suitable for high-volume memory sharing, disk storage and network transfer.
 *  Used By: Client programs.
 *
 */  

#ifndef __META__SHMETA_H__
#define __META__SHMETA_H__

#include "shmeta_value.h"

/**
 *  A meta definition is part of a @c shmeta_t hashmap.
 *  @note A @c shfs_tree sharefs file system associates meta definition information with every @c shfs_node inode entry.
 * @defgroup sh_meta A meta definition.
 * @{
 */

/**
 * The initial number of hashmap indexes to create.
 */
#define INITIAL_MAX 15 /* tunable == 2^n - 1 */

/**
 * Specifies a hard-coded value that identifies a @c shmeta_value_t data segment.
 */
#define SHMETA_VALUE_MAGIC 0x12345678

/**
 * A @c shmeta_value_t parameter specific to a indeterminate data segment.
 */
#define SHPF_NONE 0

/**
 * A @c shmeta_value_t parameter specific to a null-terminated string value.
 */
#define SHPF_STRING 1

/**
 * A hashmap table.
 */
typedef struct shmeta_t shmeta_t;

/**
 * A hashmap index.
 */
typedef struct shmeta_index_t shmeta_index_t;

/**
 * Callback functions for calculating hash values.
 * @param key The key.
 * @param klen The length of the key.
 */
typedef unsigned int (*shmetafunc_t)(const char *key, ssize_t *klen);

/**
 * A hashmap entry.
 */
typedef struct shmeta_entry_t shmeta_entry_t;
struct shmeta_entry_t {
    shmeta_entry_t *next;
    unsigned int      hash;
    const void       *key;
    ssize_t       klen;
    const void       *val;
};

/**
 * Data structure for iterating through a hash table.
 *
 * We keep a pointer to the next hash entry here to allow the current
 * hash entry to be freed or otherwise mangled between calls to
 * shmeta_next().
 */
struct shmeta_index_t {
    shmeta_t         *ht;
    shmeta_entry_t   *this, *next;
    unsigned int        index;
};

/**
 * The size of the array is always a power of two. We use the maximum
 * index rather than the size so that we can use bitwise-AND for
 * modular arithmetic.
 * The count of hash entries may be greater depending on the chosen
 * collision rate.
 */
struct shmeta_t {
    shmeta_entry_t   **array;
    shmeta_index_t     iterator;  /* For shmeta_first(...) */
    unsigned int         count, max;
    shmetafunc_t       hash_func;
    shmeta_entry_t    *free;  /* List of recycled entries */
};

/**
 * The default hashmap indexing function.
 */
unsigned int shmetafunc_default(const char *char_key, ssize_t *klen);

/**
 * Set a meta definition to a particular value
 * @param ht The meta definition hashmap to retrieve from.
 * @param sh_k The key of the meta definition value.
 * @param val The meta definition value using a @c shmeta_value_t as a header.
 */
void shmeta_set(shmeta_t *ht, shkey_t sh_k, shmeta_value_t *val);

/**
 * Get a meta definition value
 * @param ht The meta definition hashmap to retrieve from.
 * @param sh_k The key of the meta definition value.
 */
void *shmeta_get(shmeta_t *ht, shkey_t sh_k);


/**
 * @}
 */

#endif /* ndef __META__SHMETA_H__ */


