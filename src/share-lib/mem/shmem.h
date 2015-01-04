
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
*/  

#ifndef __MEM__SHMEM_H__
#define __MEM__SHMEM_H__

#if defined(HAVE_PTHREAD_MUTEX_LOCK) && defined(HAVE_PTHREAD_MUTEX_UNLOCK)
#include <pthread.h>
#endif

/**
 * Memory manipulation routines.
 * @ingroup libshare
 * @defgroup libshare_mem Encryption, memory pools, and hashmaps.
 * @{
 */


/**
 * A hard-coded value stored in memory segments in order to validate a current state or integrity.
 * @note Specifies a hard-coded value that identifies a @c shmeta_value_t data segment.
 * @see shmeta_value_t shencode()
 */
#define SHMEM_MAGIC 0x87654321

/**
 * The byte padding size when allocating a stored value.
 * @see shencode() shmeta_set()
 */
#define SHMEM_PAD_SIZE 32 






/**
 * Dynamic memory buffer.
 * @ingroup libshare_mem
 * @defgroup libshare_membuf Dynamic memory buffer allocation utilities.
 * @{
 */

/*
 * A memory buffer.
 */
typedef struct shbuf_t shbuf_t;

/**
 * A memory buffer that utilizes that re-uses available memory to reduce OS overhead and dynamically grows to a user specific need.
 * @see shbuf_init shbuf_free
 *
 */
struct shbuf_t {
  unsigned char *data;
  size_t data_of;
  size_t data_max;
  int fd;
};

/**
 * Initialize a memory buffer for use.
 * @note A @c shbuf_t memory buffer handles automatic allocation of memory.
 */
shbuf_t *shbuf_init(void);

/**
 * Inserts a string into a @c shbuf_t memory pool.
 */
void shbuf_catstr(shbuf_t *buf, char *data);

/**
 * Inserts a binary data segment into a @c shbuf_t memory pool.
 */
void shbuf_cat(shbuf_t *buf, void *data, size_t data_len);

/**
 * The current size of the data segement stored in the memory buffer.
 */
size_t shbuf_size(shbuf_t *buf);

unsigned char *shbuf_data(shbuf_t *buf);

/**
 * Clear the contents of a @c shbuf_t libshare memory buffer.
 */
void shbuf_clear(shbuf_t *buf);

/**
 * Reduce the data size of a memory buffer.
 * @param buf The memory buffer.
 * @param len The size of bytes to reduce by.
 * @note This removes data from the beginning of the data segment. 
 */
void shbuf_trim(shbuf_t *buf, size_t len);

/**
 * Frees the resources utilizited by the memory buffer.
 */
void shbuf_free(shbuf_t **buf_p);

/**
 * Grow the memory buffer to atleast the size specified.
 * @param buf The @ref shbuf_t memory buffer.
 * @param data_len The minimum byte size the memory buffer should be allocated.
 */
int shbuf_grow(shbuf_t *buf, size_t data_len);

/**
 * Map a file into a memory buffer.
 */
shbuf_t *shbuf_file(char *path);

int shbuf_growmap(shbuf_t *buf, size_t data_len);

shbuf_t *shbuf_map(unsigned char *data, size_t data_len);
shbuf_t *ashbuf_map(unsigned char *data, size_t data_len);
unsigned char *shbuf_unmap(shbuf_t *buf);
 
/**
 * @}
 */





/**
 * Key token generator.
 * @ingroup libshare_mem
 * @defgroup libshare_memkey Utility functions to generate unique checksums of data.
 * @{
 */


/**
 * A key used to represent a hash code of an object.
 */
typedef struct shkey_t shkey_t;

/**
 * The number of "word size" segments the key code is composed of.
 */
#define SHKEY_WORDS 6

/**
 * A key used to represent a hash code of an object.
 */
struct shkey_t 
{

  /** A reserved space for future 'libshare key' versions. */
  uint32_t __reserved__;

  /**
   * The checksum values comprimising the key token.
   * @note This variable must be the initial variable in the structure.
   */
  uint32_t code[SHKEY_WORDS];

};

/**
 * Generates a 128-bit key based on the data segment's checksum based on major and minor variants.
 */
shkey_t *shkey_bin(char *data, size_t data_len);

/**
 * Create a @c shkey_t hashmap key reference from @c kvalue
 * @a kvalue The string to generate into a @c shkey_t
 * @returns A @c shkey_t referencing #a kvalue
 */
shkey_t *shkey_str(char *kvalue);

/**
 * Create a @c shkey_t hashmap key reference from a number.
 * @a kvalue The number to generate into a @c shkey_t
 * @returns A statically allocated version of @kvalue 
 */
shkey_t *shkey_num(long kvalue);

/**
 * Create a unique @c shkey_t hashmap key reference.
 * @returns A @c shkey_t containing a unique key value.
 */
shkey_t *shkey_uniq(void);


void shkey_free(shkey_t **key_p);

int shrand(void);

/**
 * A ascii string representation of a libshare key.
 * @note The string returned will be 36 characters long in a format similar to base64.
 */ 
const char *shkey_print(shkey_t *key);

/**
 * Generates a @ref shkey_t from a string that does not need to be freed.
 * @see shkey_free()
 * @bug psuedo thread-safe
 */
shkey_t *ashkey_str(char *name);

/**
 * Generates a @ref shkey_t from a number that does not need to be freed.
 * @see shkey_free()
 * @bug psuedo thread-safe
 */
shkey_t *ashkey_num(long num);

/**
 * Compare two @ref shkek_t key tokens.
 * @returns A zero is both keys are identical and a -1 if the keys are unique.
 */
int shkey_cmp(shkey_t *key_1, shkey_t *key_2);

/**
 * Generates a blank @ref shkey_t key token.
 * @returns A statically allocated blank key token.
 * @note Do not free the returned value.
 */
#define ashkey_blank() \
  ((shkey_t *)_shkey_blank)

/**
 * Determines whether a @ref shkey_t has been initialized.
 * @returns FALSE is key is not blank, and TRUE is the key is blank.
 * @note It is possible to generate keys which equal a blank key, for example a key generated from a zero-length data segment. This macro should be utilitized only when it is known that the key being compared against has a unique value.
 */
#define shkey_is_blank(_key) \
  (0 == memcmp((_key), (uint32_t *)_shkey_blank, sizeof(uint32_t) * 4))

extern uint32_t _shkey_blank[8];

/**
 * A 64-bit numeric representation of a @ref shkey_t
 */
#define shkey_crc(_key) \
  ((_key) ? shcrc((_key), sizeof(shkey_t)) : 0)

shkey_t *shkey_clone(shkey_t *key);

/**
 * Creates a certificate's signature key based off content attributes.
 */
shkey_t *shkey_cert(shkey_t *key, uint64_t crc, shtime_t stamp);

/**
 * Verifies whether content attributes match a generated certificate signature key.
 */
int shkey_verify(shkey_t *sig, uint64_t crc, shkey_t *key, shtime_t stamp);

/**
 * Converts a hex string into a binary key.
 */
shkey_t *shkey_gen(char *hex_str);

/**
 * A string hexadecimal representation of a libshare key.
 * @note The string returned will be 48 characters long.
 */ 
const char *shkey_hex(shkey_t *key);

/**
 * Generate a libshare key from a 48-character long hexadecimal string.
 */
shkey_t *shkey_hexgen(char *hex_str);


/**
 * @}
 */







/**
 * Memory Buffer Pools
 * @ingroup libshare_mem
 * @defgroup libshare_mempool The libshare memory buffer pool allocation utilities.
 * @{
 */

/**
 * A memory pool.
 */
typedef struct shpool_t shpool_t;

/**
 * A memory pool.
 */
struct shpool_t {
  shbuf_t **pool;
  int max;
};

/**
 * Initializes a new memory pool instance.
 * @returns A @ref shpool_t memory pool of @ref shbuf_t memory buffers.
 */
shpool_t *shpool_init(void);

/**
 * Calculates the number of avaiable @ref shbuf_t memory buffer contained 
 * in the @ref shpool_t memory pool.
 * @see shpool_get_index()
 */
size_t shpool_size(shpool_t *pool);

/**
 * Increases the size of the memory pool.
 * @bug smaller incremental reallocs have been known to fail in glibc
 */
void shpool_grow(shpool_t *pool);

/**
 * Get's the next available memory buffer from a pool.
 * @see shpool_put()
 */
shbuf_t *shpool_get(shpool_t *pool);


/**
 * Get's a specific @ref shbuf_t memory buffer by index number.
 * @param index The index number of the memory buffer.
 * @returns The @ref shbuf_t memory buffer associated with the index or NULL if none exist.
 * @see shpool_size();
 */
shbuf_t *shpool_get_index(shpool_t *pool, int index);

/**
 * Put's a memory buffer into a pool.
 */
void shpool_put(shpool_t *pool, shbuf_t *buff);

/**
 * Free's the resources associated with a memory pool.
 * @param pool_p A reference to an allocated @ref pool_t memory pool.
 */
void shpool_free(shpool_t **pool_p);

/**
 * @}
 */






/**
 * Meta definition hashmaps.
 * @ingroup libshare_mem
 * @defgroup libshare_memmeta Store object-related information by key token.
 * @{
 */

/**
 * The initial number of hashmap indexes to create.
 */
#define INITIAL_MAX 15 /* tunable == 2^n - 1 */

/**
 * The network byte order representation of @c SHMEM_MAGIC.
 */
#define SHMETA_VALUE_NET_MAGIC htons(SHMEM_MAGIC)

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
#define SHPF_BINARY 2

/**
 * A @c shmeta_value_t parameter specific to a memory reference (void *).
 */
#define SHPF_REFERENCE 3

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
struct shmeta_value_t 
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
typedef struct shmeta_value_t shmeta_value_t;

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

/**
 * Obtain a non-specific binary data segment from a meta definition hash map.
 * @param h The meta definition hash map.
 * @param name The name of the meta definition.
 */
void *shmeta_get_void(shmeta_t *h, shkey_t *key);

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

/** */
void shbuf_append(shbuf_t *from_buff, shbuf_t *to_buff);

/** */
shbuf_t *shbuf_clone(shbuf_t *buff);

/**
 * @returns the number of characters appended to the memory buffer.
 * @note passes arguments through vsnprintf().
 */
int shbuf_sprintf(shbuf_t *buff, char *fmt, ...);

unsigned int shmeta_count(shmeta_t *ht);

void shmeta_unset_ptr(shmeta_t *h, shkey_t *key);

void shmeta_set_ptr(shmeta_t *ht, shkey_t *key, void *ptr);

void shmeta_unset_ptr(shmeta_t *h, shkey_t *key);

void *shmeta_get_ptr(shmeta_t *h, shkey_t *key);

/**
 * @}
 */




/**
 * Encryption routines.
 * @ingroup libshare_crypt
 * @defgroup libshare_memcrypt Encryption and decryption of memory segments.
 * @{
 */

/**
 * Encrypt a data segment without allocating additional memory.
 * @param data - A segment of data.
 * @paramlen - The length of the data segment.
 * @param key - Pointer to a libshare @ref shkey_t token key.
 * @returns A zero on success and negative one (-1) when the string is already encrypted with the same key.
 * @note Fills @c data with encrypted data and @c len with the size of the new data array
 * @note data size must be equal to or larger than ((len + 7) / 8) * 8 + 8 + 4
 * TEA encrypts in 8 byte blocks, so it must include enough space to 
 * hold the entire data to pad out to an 8 byte boundary, plus another
 * 8 bytes at the end to give the length to the decrypt algorithm, plus
 * another 4 bytes to signify that it has been encrypted.
 * @note You must use the same key passed into this function in order to decrypt the segment.
 * @bug The data segment must be allocated 20 bytes larger than data_len. If possible this should return the same data length even if up to 16 bytes of the segment suffix is not encrypted.
 * @bug Both parameters will be modified.
 * @bug Specifying a different key will not prevent the data segment from being re-encrypted. The magic number @ref SHMEM_MAGIC should be used instead. 
 */
int ashencode(char *data, size_t *data_len_p, shkey_t *key);

/**
 * Encrypts byte array data of length len with key key using TEA.
 * @param data - A segment of data.
 * @paramlen - The length of the data segment.
 * @param key - Pointer to a libshare @ref shkey_t token key.
 * @returns A zero on success and negative one (-1) when the string is already encrypted with the same key.
 * @note Fills @c data with encrypted data and @c len with the size of the new data array
 * @note data size must be equal to or larger than ((len + 7) / 8) * 8 + 8 + 4
 * TEA encrypts in 8 byte blocks, so it must include enough space to 
 * hold the entire data to pad out to an 8 byte boundary, plus another
 * 8 bytes at the end to give the length to the decrypt algorithm, plus
 * another 4 bytes to signify that it has been encrypted.
 * @bug The data segment must be allocated 20 bytes larger than data_len. If possible this should return the same data length even if up to 16 bytes of the segment suffix is not encrypted.
 * @bug Both parameters will be modified.
 * @bug Specifying a different key will not prevent the data segment from being re-encrypted. The magic number @ref SHMEM_MAGIC should be used instead. 
 */
int shencode(char *data, size_t data_len, uint8_t **data_p, uint32_t *data_len_p, shkey_t *key);

/**
 * @see shdecode_str()
 */
shkey_t *shencode_str(char *data);

/**
 * Decrypt a data segment without allocating additional memory.
 * @param data - pointer to 8 bit data array to be decrypted - SEE NOTES
 * @param len - length of array
 * @param key - Pointer to four integer array (16 bytes) holding TEA key
 * @returns A zero on success and negative one (-1) when the string is not encrypted.
 * @note Modifies data and len
 * @note Fills @ref data with decrypted data and @ref len with the size of the new data 
 * @bug Using magic numbers in encrypt and decrypt routines - use #defines instead - Kyle
 * @bug If the 64 bit encoding functions aren't used outside this module, their prototypes should be in the code, not header - Simon
 * @bug Add sanity checking to input - Rob
 * @bug Require that input len is a multiple of 8 bytes - making a requirement we can't enforce or check is a recipe for corruption - Rob
 */
int ashdecode(uint8_t *data, size_t *data_len_p, shkey_t *key);

/**
 * Decrypts byte array data of length len with a @shkey_t key token.
 * @param data pointer to 8 bit data array to be decrypted
 * @param len length of array
 * @param data_p A reference to the decrypted data segment. 
 * @param data_len_p The length of the decrypted data segment.
 * @param key - Pointer to four integer array (16 bytes).
 * @returns A zero on success and negative one (-1) when the string is not encrypted.
 * @note Modifies data and len
 * @bug Using magic numbers in encrypt and decrypt routines - use #defines instead - Kyle
 * @bug If the 64 bit encoding functions aren't used outside this module, their prototypes should be in the code, not header - Simon
 * @bug Add sanity checking to input - Rob
 * @bug Require that input len is a multiple of 8 bytes - making a requirement we can't enforce or check is a recipe for corruption - Rob
 */
int shdecode(uint8_t *data, uint32_t data_len, char **data_p, size_t *data_len_p, shkey_t *key);

/**
 * Decrypt a string into it's original format using an assigned key.
 * @param key The key returned by @ref shencode_str()
 * @returns A zero on success and negative one (-1) when the string is not encrypted.
 */
int shdecode_str(char *data, shkey_t *key);



int shencode_b64(char *data, size_t data_len, uint8_t **data_p, uint32_t *data_len_p, shkey_t *key);

int shdecode_b64(char *data, size_t data_len, uint8_t **data_p, uint32_t *data_len_p, shkey_t *key);

/**
 * @}
 */





/**
 * Mutex and semaphore locks.
 * @ingroup libshare_mem
 * @defgroup libshare_memlock Utility functions to provide mutex thread locks and semaphores.
 * @{
 */


/**
 * A lock used by the share library in order to ensure no two threads perform a system call at the same instance.
 */
#define SHLOCK_SYSTEM -9999

/**
 * Prevents the mutex from allowing the same thread to access the lock.
 * @note Similar to a semaphore as the lock is not based on thread conditions.
 */
#define SHLK_PRIVATE (1 << 0)

/**
 * The share library lock structure is used primarily in order to prevent multiple threads from performing system calls at the same instance.
 * The lock includes both re-entrant and private access styles.
 * The functionality can be extended in order to provide custom locks.
 * @see shlock_open() shlock_close()
 */
typedef struct shlock_t shlock_t;

/**
 * The share library lock structure.
 */
struct shlock_t {
  /**
   * The number of references being made to this lock.
   */
  int ref;

  /**
   * The process thread identification number
   * @see gettid()
   */
  pid_t tid;

#if defined(HAVE_PTHREAD_MUTEX_LOCK) && defined(HAVE_PTHREAD_MUTEX_UNLOCK)
  /**
   * A linux-style mutex reference.
   */
  pthread_mutex_t mutex;
#endif

};


/**
 * Create a new lock on a mutex waiting if needed.
 * @param num A positive number identifying the lock.
 * @param flags A set of modifiers to configure the lock. (SHLK_XXX)
 * @note The libshare uses negative numbers for internal locks.
 * @see SHLOCK_SYSTEM SHLK_PRIVATE
 * @bug flags should be stored in @ref shkey_t instead of a paramter 
 */
shlock_t *shlock_open(shkey_t *key, int flags);

/**
 * Opens a lock based on a number.
 * @see shlock_open()
 */
#define shlock_open_num(_num, _flags) \
  shlock_open(ashkey_num(_num), (_flags))

/**
 * Opens a lock based on a string.
 * @see shlock_open()
 */
#define shlock_open_str(_str, _flags) \
  shlock_open(ashkey_str(_str), (_flags))

/**
 * Create a new lock on a mutex unless one has already been created.
 * @param num A positive number identifying the lock.
 * @param flags A set of modifiers to configure the lock. (SHLK_XXX)
 * @note The libshare uses negative numbers for internal locks.
 * @returns A 0 on success, a 1 when the mutex is already locked, and a -1 on error.
 * @see SHLOCK_SYSTEM SHLK_PRIVATE
 */
int shlock_tryopen(shkey_t *key, int flags, shlock_t **lock_p);

/**
 * Opens a lock based on a number.
 * @see shlock_tryopen()
 */
#define shlock_tryopen_num(_num, _flags, _keyp) \
  shlock_tryopen(ashkey_num(_num), (_flags), (_keyp))

/**
 * Opens a lock based on a string.
 * @see shlock_tryopen()
 */
#define shlock_tryopen_str(_str, _flags, _keyp) \
  shlock_tryopen(ashkey_str(_str), (_flags), (_keyp))

/**
 * Unlock a mutex.
 */ 
int shlock_close(shkey_t *key);

/**
 * Closes a lock based on a number.
 * @see shlock_close()
 */
#define shlock_close_num(_num, _flags) \
  shlock_close(ashkey_num(_num), (_flags))

/**
 * Closes a lock based on a string. 
 * @see shlock_close()
 */
#define shlock_close_str(_str, _flags) \
  shlock_close(ashkey_str(_str), (_flags))


/**
 * @}
 */







/**
 * JSON text parser and generator.
 * @ingroup libshare_mem
 * @defgroup libshare_memjson JSON text parser and generator.
 * @{
 */


#ifndef DBL_EPSILON
#define DBL_EPSILON 2.2204460492503131E-16
#endif
#ifndef INT_MAX
#define INT_MAX 0x7FFF/0x7FFFFFFF
#endif
#ifndef INT_MIN
#define INT_MIN ((int) 0x8000/0x80000000)
#endif





/* shjson_ Types: */
#define shjson_False 0
#define shjson_True 1
#define shjson_NULL 2
#define shjson_Number 3
#define shjson_String 4
#define shjson_Array 5
#define shjson_Object 6
	
#define shjson_IsReference 256

/* The shjson structure: */
typedef struct shjson_t {
/** next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
	struct shjson_t *next,*prev;	

		/** An array or object item will have a child pointer pointing to a chain of the items in the array/object. */
	struct shjson_t *child;

			/** The type of the item, as above. */
	int type;		

		/** The item's string, if type==shjson_String */
	char *valuestring;	
			/** The item's number, if type==shjson_Number */
	int valueint;	
		/** The item's number, if type==shjson_Number */
	double valuedouble;	

	/** The item's name string, if this item is the child of, or is in the list of subitems of an object. */
	char *string;			
} shjson_t;



/**
 * Obtain a text representation of the @c json hiearchy in JSON format.
 * @returns An allocated string in JSON format.
 * @see shjson_init()
 */
char *shjson_print(shjson_t *json);

/**
 * Obtain an allocated string value from a JSON object.
 * @param tree The JSON object containing the string value.
 * @param name The name of the string JSON node.
 * @param def_str The default string value if the JSON node does not exist.
 * @returns The string value contained in the JSON node.
 */
char *shjson_str(shjson_t *json, char *name, char *def_str);

/**
 * Obtain an un-allocated string value from a JSON object.
 * @param tree The JSON object containing the string value.
 * @param name The name of the string JSON node.
 * @param def_str The default string value if the JSON node does not exist.
 * @returns The string value contained in the JSON node.
 */
char *shjson_astr(shjson_t *json, char *name, char *def_str);

/**
 * Add a string value to a JSON object or array.
 * @param tree The JSON object containing the string value.
 * @param name The name of the string JSON node.
 * @param val The string value to store in the new JSON node.
 * @returns The new JSON node containing the string value.
 */
shjson_t *shjson_str_add(shjson_t *tree, char *name, char *val);

/**
 * De-allocates memory associated with a JSON hiearchy.
 * @param tree_p A reference to the JSON hierarchy.
 * @see shjson_init()
 */
void shjson_free(shjson_t **tree_p);

/**
 * Obtain a number value from a JSON object.
 * @param tree The JSON object containing the number value.
 * @param name The name of the number JSON node.
 * @param def_d The default number value if the JSON node does not exist.
 * @returns The number value contained in the JSON node.
 */
double shjson_num(shjson_t *json, char *name, double def_d);

/**
 * Add a number value to a JSON object or array.
 * @param tree The JSON object or array to add the number value.
 * @param name The name of the number JSON node or NULL if @c tree is an array.
 * @param idx The number value to store in the new JSON node.
 * @returns The new JSON node containing the number value.
 */
shjson_t *shjson_num_add(shjson_t *tree, char *name, double num);

/**
 * Create a new JSON tree hierarchy.
 * @param json_str A JSON formatted text string or NULL.
 * @returns A new JSON object if @c json_str is null or a full JSON node hierarchy otherwise.
 * @see shjson_print
 * @see shjson_free
 */
shjson_t *shjson_init(char *json_str);

/**
 * Create a new JSON node at the end of an array.
 * @param json The JSON object containing the array.
 * @param name The name of the array in the JSON object.
 * @returns A new JSON node attachd to the array.
 */
shjson_t *shjson_array_add(shjson_t *tree, char *name);

/**
 * Obtain an allocated string value from an array.
 * @param json The JSON object containing the array.
 * @param name The name of the array in the JSON object.
 * @param idx The index number of the JSON node in the array.
 * @returns The string value contained in the array's node index.
 * @note The string pointer returned must be de-allocated.
 */
char *shjson_array_str(shjson_t *json, char *name, int idx);

/**
 * Obtain an un-allocated string value from an array.
 * @param json The JSON object containing the array.
 * @param name The name of the array in the JSON object.
 * @param idx The index number of the JSON node in the array.
 * @returns The string value contained in the array's node index.
 * @note Do not free the string pointer returned.
 */
char *shjson_array_astr(shjson_t *json, char *name, int idx);

/**
 * Obtain a number value from an array.
 * @param json The JSON object containing the array.
 * @param name The name of the array in the JSON object.
 * @param idx The index number of the JSON node in the array.
 * @returns The numeric value contained in the array's node index.
 */
double shjson_array_num(shjson_t *json, char *name, int idx);

/**
 * @returns A JSON object contained inside another object.
 */
shjson_t *shjson_obj(shjson_t *json, char *name);

/**
 * @returns The string length of a JSON object node.
 */
size_t shjson_strlen(shjson_t *json, char *name);


/**
 * @}
 */



/**
 * Scrypt hash generator
 * @ingroup libshare_mem
 * @defgroup libshare_memscrypt Scrypt hash generator
 * @{
 */

typedef struct scrypt_work 
{
        unsigned char   data[128];
        unsigned char   midstate[32];
        unsigned char   target[32];
        char hash[32];

      char ntime[16];
struct timeval tv_received;
uint32_t hash_nonce;
int restart;

        uint64_t        share_diff;

        int             rolls;
        int             drv_rolllimit; /* How much the driver can roll ntime */

        //dev_blk_ctx     blk;

        //struct thr_info *thr;
        //int             thr_id;
        //struct pool     *pool;
        struct timeval  tv_staged;

#if 0
        bool            mined;
        bool            clone;
        bool            cloned;
        int             rolltime;
        bool            longpoll;
        bool            stale;
        bool            mandatory;
        bool            block;
#endif
	uint32_t nonce;

//        bool            stratum;
        char            *job_id;
  char xnonce2[16];
//        bytes_t         nonce2;
        double          sdiff;
//        char            *nonce1;

        unsigned char   work_restart_id;
        int             id;
        int             device_id;
        //UT_hash_handle hh;

        double          work_difficulty;

        char merkle_root[256]; 
#if 0
        // Allow devices to identify work if multiple sub-devices
        // DEPRECATED: New code should be using multiple processors instead
        unsigned char   subid;

        // Allow devices to timestamp work for their own purposes
        struct timeval  tv_stamp;

        //blktemplate_t   *tmpl;
        int             *tmpl_refcount;
        unsigned int    dataid;
        bool            do_foreign_submit;

        struct timeval  tv_getwork;
        time_t          ts_getwork;
        struct timeval  tv_getwork_reply;
        struct timeval  tv_cloned;
        struct timeval  tv_work_start;
        struct timeval  tv_work_found;
        char            getwork_mode;
#endif

  double pool_diff;

} scrypt_work;

typedef struct scrypt_peer
{
  char nonce1[16];
  size_t n1_len;
  size_t n2_len;
  double diff;

} scrypt_peer;


#ifndef bswap_16
#define bswap_16(value)  \
        ((((value) & 0xff) << 8) | ((value) >> 8))

#define bswap_32(value) \
        (((uint32_t)bswap_16((uint16_t)((value) & 0xffff)) << 16) | \
        (uint32_t)bswap_16((uint16_t)((value) >> 16)))

#define bswap_64(value) \
        (((uint64_t)bswap_32((uint32_t)((value) & 0xffffffff)) \
            << 32) | \
        (uint64_t)bswap_32((uint32_t)((value) >> 32)))
#endif

#define flip32(dest_p, src_p) \
  (swap32yes(dest_p, src_p, 32 / 4))

static inline uint32_t swab32(uint32_t v)
{
        return bswap_32(v);
}
static inline void swap32yes(void*out, const void*in, size_t sz) {
        size_t swapcounter = 0;
        for (swapcounter = 0; swapcounter < sz; ++swapcounter)
                (((uint32_t*)out)[swapcounter]) = swab32(((uint32_t*)in)[swapcounter]);
}

void shscrypt_peer(scrypt_peer *peer, char *nonce1, double diff);
int shscrypt(scrypt_work *work, int step);
int shscrypt_verify(scrypt_work *work);
void shscrypt_peer_gen(scrypt_peer *peer, double diff);
double shscrypt_hash_diff(scrypt_work *work);


void shscrypt_work(scrypt_peer *peer, scrypt_work *work, 
    char **merkle_list, char *prev_hash, 
    char *coinbase1, char *coinbase2, char *nbit, char *ntime);

void shscrypt_swap256(void *dest_p, const void *src_p);


/**
 * @}
 */










/**
 * Generate hash checksums.
 * @ingroup libshare_mem
 * @defgroup libshare_memdigest Utility functions to generate unique checksums of data in SHA256 format.
 * @{
 */

#define _SH_SHA256_BLOCK_SIZE  ( 512 / 8)
#define SHA256_DIGEST_SIZE ( 256 / 8)

#define SHFR(x, n)    (x >> n)
#define ROTR(x, n)   ((x >> n) | (x << ((sizeof(x) << 3) - n)))
#define CH(x, y, z)  ((x & y) ^ (~x & z))
#define _SH_MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))

#define SHA256_F1(x) (ROTR(x,  2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define SHA256_F2(x) (ROTR(x,  6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SHA256_F3(x) (ROTR(x,  7) ^ ROTR(x, 18) ^ SHFR(x,  3))
#define SHA256_F4(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ SHFR(x, 10))

void sh_sha256(const unsigned char *message, unsigned int len, unsigned char *digest);

char *shdigest(void *data, int32_t len);

void sh_calc_midstate(struct scrypt_work *work);

/**
 * @}
 */




/**
 * Compress and decompress data segments.
 * @ingroup libshare_mem
 * @defgroup libshare_memzlib
 * @{
 */

/**
 * Compress data into a data buffer.
 * @buff The data buffer to store the compressed data.
 * @data The data segment to compress.
 */
int shzenc(shbuf_t *buff, void *data, size_t data_len);

/**
 * Decompress a data segment into a data buffer.
 * @buff The data buffer to store the decompressed data.
 * @data The data segment to decompress.
 */
int shzdec(shbuf_t *buff, unsigned char *data, size_t data_len);

/**
 * @}
 */





/**
 * Generate xd3 diff comparisons.
 * @ingroup libshare_mem
 * @defgroup libshare_memdelta
 * @{
 */

int shdelta(shbuf_t *src_buff, shbuf_t *in_buff, shbuf_t *out_buff);

int shpatch(shbuf_t *src_buff, shbuf_t *in_buff, shbuf_t *out_buff);


/**
 * @}
 */

int shdiff(shbuf_t *buff, char *str_1, char *str_2);




/** 
 * @example shkeystore.c
 * Example of storing, verifying, and retrieiving arbitrary keys.
 */


/**
 * @}
 */


#endif /* ndef __MEM__SHMEM_H__ */

