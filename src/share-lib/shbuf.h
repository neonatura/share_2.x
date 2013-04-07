
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
 *
 *  Provides: Dynamic allocation of memory pools.
 *  Used By: Client programs.
*/  

#ifndef __BUFFER__SHBUF_H__
#define __BUFFER__SHBUF_H__

/**
 * @addtogroup libshare
 * @{
 */

/**
 * A memory buffer.
 */
typedef struct shbuf_t {
  unsigned char *data;
  size_t data_of;
  size_t data_max;
} shbuf_t;

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

/**
 * Clear the contents of a @c shbuf_t libshare memory buffer.
 */
void shbuf_clear(shbuf_t *buf);

/**
 * Frees the resources utilizited by the memory buffer.
 */
void shbuf_free(shbuf_t **buf_p);
 
/**
 * @}
 */

#endif /* ndef __BUFFER__SHBUF_H__ */

