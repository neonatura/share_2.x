
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

#include "share.h"

static void shalloc_pool_incr(shpool_t *pool, shbuf_t *buff)
{

  if (!buff)
    buff = shbuf_init();

  if (!pool->pool) {
    pool->max = 1;
    pool->pool = (shbuf_t **)calloc(1, sizeof(shbuf_t *));
    pool->pool[0] = buff;
  } else {
    size_t len = sizeof(shbuf_t *) * (pool->max + 1);
    pool->pool = (shbuf_t **)realloc(pool->pool, len);
    pool->pool[pool->max] = buff;
    pool->max++;
  }

}

shpool_t *shpool_init(void)
{
  shpool_t *pool;

  pool = (shpool_t *)calloc(1, sizeof(shpool_t));
  return (pool);
}

/**
 * Calculates the number of avaiable @ref shbuf_t memory buffer contained 
 * in the @ref shpool_t memory pool.
 * @see shpool_get_index()
 */
size_t shpool_size(shpool_t *pool)
{
  return (pool->max);
}

/**
 * Get's the next available memory buffer from a pool.
 */
shbuf_t *shpool_get(shpool_t *pool)
{
  static int pool_idx;

  if (pool->max == 0)
    shalloc_pool_incr(pool, NULL);
  
  return (pool->pool[pool_idx++ % pool->max]);
}

void shpool_grow(shpool_t *pool)
{
  shalloc_pool_incr(pool, NULL);
}

shbuf_t *shpool_get_index(shpool_t *pool, int index)
{

  if (index < 0 || index >= pool->max)
    return (NULL); /* invalid param */

  return (pool->pool[index % pool->max]);
}

void shpool_put(shpool_t *pool, shbuf_t *buff)
{
  shalloc_pool_incr(pool, buff);
}

void shpool_free(shpool_t **pool_p)
{
  int i;
  shpool_t *pool;

  if (!pool_p || !*pool_p)
    return;
  pool = *pool_p;
  *pool_p = NULL;

  for (i = 0; i < pool->max; i++)
    shbuf_free(&pool->pool[i]);
  
  free(pool);

}


