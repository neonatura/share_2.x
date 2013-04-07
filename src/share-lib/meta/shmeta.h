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
 *  @file shmeta.h
 *
 */  

#ifndef __META__SHMETA_H__
#define __META__SHMETA_H__

#include "shmeta_value.h"

/**
 * A meta definition is part of a @c shmeta_t hashmap.
 *
 * The share library meta definitions can be used to hash header information from a socket stream, retaining access to the meta information by a token, and allowing for efficient redelivery or caching.
 *
 * In reference to the internal form of a hash table:
 *
 * The table is an array indexed by the hash of the key; collisions
 * are resolved by hanging a linked list of hash entries off each
 * element of the array. Although this is a really simple design it
 * isn't too bad given that pools have a low allocation overhead.
 *
 *  @note A @c shfs_tree sharefs file system associates meta definition information with every @c shfs_node inode entry.
 *  @ingroup libshare
 * @defgroup libshare_meta Meta definition hash maps (meta maps).
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
 * The network byte order representation of @c SHMETA_VALUE_MAGIC.
 */
#define SHMETA_VALUE_NET_MAGIC htons(0x12345678)

/**
 * Specifies that a machine has a big endian architecture.
 * @see SHMETA_VALUE_ENDIAN
 */
#define SHMETA_BIG_ENDIAN 0

/**
 * Specifies that a machine has a small endian architecture.
 * @see SHMETA_VALUE_ENDIAN
 */
#define SHMETA_SMALL_ENDIAN 1

/**
 * Determines whether the meta value originated from a big or small endian architecture.
 * @returns SHMETA_BIG_ENDIAN or SHMETA_SMALL_ENDIAN based on the meta value.
 */
#define SHMETA_VALUE_ENDIAN(_val) \
  (_val->magic == SHMETA_VALUE_NET_MAGIC ? \
   SHMETA_BIG_ENDIAN : SHMETA_SMALL_ENDIAN)

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
 * Create an instance of a meta definition hashmap.
 * @returns A @c shmeta_t meta definition hashmap.
 */
shmeta_t *shmeta_init(void);

/**
 * Free an instance of a meta definition hashmap.
 * @param meta_p A reference to the meta definition hashmap to be free'd.
 */
void shmeta_free(shmeta_t **meta_p);

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
 * Prints out a JSON representation of a meta definition hashmap.
 * @note The text buffer must be allocated by @c shbuf_init() first.
 * @param h The meta map to print.
 * @param ret_buff The text buffer to return the JSON string representation.
 */
void shmeta_print(shmeta_t *h, shbuf_t *ret_buff);

/**
 * @}
 */

#endif /* ndef __META__SHMETA_H__ */


