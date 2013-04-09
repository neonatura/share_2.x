/*
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
*/  

#ifndef __SHKEY_H__
#define __SHKEY_H__

/**
 * Utility functions to generate unique checksums of data.
 * @brief Hash code token operations.
 * @addtogroup libshare
 * @{
 */

/**
 * A key used to represent a hash code of an object.
 */
typedef uint64_t shkey_t; 

/**
 * Create a @c shkey_t hashmap key reference from a binary memory segment.
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
 * @}
 */

#endif /* ndef __SHKEY_H__ */

