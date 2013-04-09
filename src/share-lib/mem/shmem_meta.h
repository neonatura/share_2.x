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
 */  

#ifndef __MEM__SHMEM_META_H__
#define __MEM__SHMEM_META_H__

/**
 * @ingroup libshare_mem
 * @defgroup libshare_memmeta Meta Definition Hashmaps
 * @{
 */

/**
 * The byte padding size when allocating a meta definition value.
 */
#define SHMETA_PAD_SIZE 16

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
 * A @c shmeta_value_t parameter specific to a non-specific binary memory segment.
 */
#define SHPF_BINARY 1

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
 * A meta definition is part of a @c shmeta_t hashmap.
 *
 * The share library meta definitions can be used to hash header information from a socket stream, retaining access to the meta information by a token, and allowing for efficient redelivery or caching.
 *
 * @note A @c shfs_tree sharefs file system associates meta definition information with every @c shfs_node inode entry.
 * @note The size of the array is always a power of two. We use the maximum index rather than the size so that we can use bitwise-AND for modular arithmetic. The count of hash entries may be greater depending on the chosen collision rate.
 * @note The table is an array indexed by the hash of the key; collisions are resolved by hanging a linked list of hash entries off each element of the array. Although this is a really simple design it isn't too bad given that pools have a low allocation overhead.
 */
struct shmeta_t {
    shmeta_entry_t   **array;
    shmeta_index_t     iterator;  /* For shmeta_first(...) */
    unsigned int         count, max;
    shmetafunc_t       hash_func;
    shmeta_entry_t    *free;  /* List of recycled entries */
};

/**
 * The base of a version 1 shmeta hashmap entry value.
 */
struct shmeta_value_v1_t 
{

  /**
   * The hard-coded value @c SHMETA_VALUE_MAGIC set to ensure validity.
   * @note Used for recycling pools of objects and to ensure network integrity.
   */ 
  uint32_t magic;

  /**
   * An adler32 checksum of this @c shmeta_value_t hashmap value header and the data segment following.
   */
  uint32_t crc;

  /**
   * A unix-time timestamp referencing when this hashmap value was generated.
   */
  uint32_t stamp;

  /**
   * A unique index number directly related to this @c shmeta_value_t hashmap value.
   */
  uint32_t gl;

  /**
   * A adler32 reference to a function pertaining to this hashmap value.
   */ 
  uint32_t func;

  /**
   * A paramater format which describes the data segment following the @c shmeta_value_t header.
   */
  uint32_t pf;  

  /**
   * A sequence number in an order of operations.
   */
  uint32_t seq;

  /**
   * An adler64 reference to the name of this value.
   */
  shkey_t name;

  /**
   * The total size of data segment not including the @c shmeta_value_t header 
   */
  shsize_t sz;  

  /**
   * blind reference to additional an data segment. 
   * */
  char raw[0];
};

/**
 * Specifies a reference to the current version of a shmeta hashmap entry value.
 */
typedef struct shmeta_value_v1_t shmeta_value_t;

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
unsigned int shmetafunc_default(const char *char_key, ssize_t *klen_p);

/**
 * Set a meta definition to a particular value
 * @param ht The meta definition hashmap to retrieve from.
 * @param sh_k The key of the meta definition value.
 * @param val The meta definition value using a @c shmeta_value_t as a header.
 */
void shmeta_set(shmeta_t *ht, shkey_t *key, const void *val);

/**
 * Set a meta definition to a string value.
 * @param h The meta definition hash map.
 * @param name A string name identifying the meta definition.
 * @param value A string value to be assigned.
 */
void shmeta_set_str(shmeta_t *h, shkey_t *key, char *value);

/**
 * Unset a string value from a meta definition.
 */
void shmeta_unset_str(shmeta_t *h, shkey_t *name);

/**
 * Set an object value in a meta definition hash map.
 * @param h The meta definition hash map.
 * @param name The name of the meta definition.
 * @param data The binary data to assign.
 * @param data_len The size of the bindary data.
 */
void shmeta_set_void(shmeta_t *ht, shkey_t *key, void *data, size_t data_len);

/**
 * Unset an object value from a meta definition hash map.
 */
void shmeta_unset_void(shmeta_t *h, shkey_t *key);

/**
 * Get a string meta from a meta definition.
 * @returns A string reference to the hashmap value.
 */
char *shmeta_get_str(shmeta_t *h, shkey_t *key);
#if 0
#define shmeta_get_str(_map, _key) \
  (shmeta_get(_map, _key) ?  (shmeta_get(_map, _key) + sizeof(shmeta_value_t)) : NULL)
#endif

/**
 * Obtain a non-specific binary data segment from a meta definition hash map.
 * @param h The meta definition hash map.
 * @param name The name of the meta definition.
 */
void *shmeta_get_void(shmeta_t *h, shkey_t *key);
#if 0
#define shmeta_get_void(_map, _key) \
  (shmeta_get(_map, _key) ?  (shmeta_get(_map, _key) + sizeof(shmeta_value_t)) : NULL)
#endif

/**
 * Get a meta definition value
 * @param ht The meta definition hashmap to retrieve from.
 * @param sh_k The key of the meta definition value.
 * @returns A @c shmeta_value_t containing the hashmap value.
 */
void *shmeta_get(shmeta_t *ht, shkey_t *key);

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

#endif /* ndef __MEM__SHMEM_META_H__ */


