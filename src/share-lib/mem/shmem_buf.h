
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

#ifndef __MEM__SHMEM_BUF_H__
#define __MEM__SHMEM_BUF_H__

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
 
/**
 * @}
 */

#endif /* ndef __MEM__SHMEM_BUF_H__ */

