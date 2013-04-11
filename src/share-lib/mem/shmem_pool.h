
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

#ifndef __MEM__SHMEM_POOL_H__
#define __MEM__SHMEM_POOL_H__

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

#endif /* ndef __MEM__SHMEM_POOL_H__ */

