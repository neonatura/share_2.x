
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

#ifndef __MEM__SHMEM_KEY_H__
#define __MEM__SHMEM_KEY_H__

/**
 * Key token generator.
 * @ingroup libshare_mem
 * @defgroup libshare_memkey Utility functions to generate unique checksums of data.
 * @{
 */

/**
 * Generates a 64-bit key based on the data segment's checksum based on major and minor variants.
 * @param _data The data segment to generate a key from.
 * @param _data_len The size of the data segment.
 * @param _key A pointer to the @ref shkey_t to fill in.
 */
#define shkey_bin_r(_bindata, _binlen, _key) \
  (_key)->data_len = (_binlen); \
  (_key)->code[3] = (uint32_t)shcrc((_bindata), (_binlen)); \
  (_key)->code[0] = (uint32_t)shcrc((_bindata), (_binlen) / 4); \
  (_key)->code[1] = (uint32_t)shcrc((_bindata) + ((_binlen) / 3), (_binlen)/2); \
  (_key)->code[2] = (uint32_t)shcrc((_bindata), (_binlen) / 3);

/**
 * A key used to represent a hash code of an object.
 */
typedef struct shkey_t shkey_t;

/**
 * A key used to represent a hash code of an object.
 */
struct shkey_t 
{

  /**
   * The checksum values comprimising the key token.
   * @note This variable must be the initial variable in the structure.
   */
  uint32_t code[4];

  /**
   * The optional length of the data segment the key applies to.
   * @note Will automatically be set to the length of the data segment used to generate the key.
   */
  uint64_t data_len;

};

/**
 * Create a @ref shkey_t hashmap key reference from a binary memory segment.
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

/**
 * A string hexadecimal representation of a @c shkey_t.
 * @note The string returned will be 32 characters long.
 * @see shkey_t
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
 * @}
 */

#endif /* ndef __MEM__SHMEM_KEY_H__ */


