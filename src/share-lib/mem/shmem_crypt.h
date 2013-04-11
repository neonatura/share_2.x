

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

#ifndef __MEM__SHMEM_TEA_H__
#define __MEM__SHMEM_TEA_H__

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
 * @bug The data segment must be allocated 20 bytes larger than data_len. If possible this should return the same data length even if up to 16 bytes of the segment suffix is not encrypted.
 * @bug Both parameters will be modified.
 * @bug Specifying a different key will not prevent the data segment from being re-encrypted. The magic number @ref SHMEM_MAGIC should be used instead. 
 */
int ashencode(uint8_t *data, uint32_t *data_len, shkey_t *key);

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
int ashdecode(uint8_t *data, uint32_t *data_len, shkey_t *key);

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
int shdecode(uint8_t *data, uint32_t data_len, char **data_p, long **data_len_p, shkey_t *key);

/**
 * Decrypt a string into it's original format using an assigned key.
 * @param key The key returned by @ref shencode_str()
 * @returns A zero on success and negative one (-1) when the string is not encrypted.
 */
int shdecode_str(char *data, shkey_t *key);


/**
 * @}
 */

#endif /* ndef __MEM__SHMEM_TEA_H__ */

