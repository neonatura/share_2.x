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

#include "../share.h"


static apr_pool_t *svn_mempool;

void shsvnhash_print(apr_hash_t *h, apr_pool_t *p, char *str, size_t str_max)
{
    apr_hash_index_t *hi;
    char *val, *key;
    apr_ssize_t len;
    int i = 0;

    str[0] = '\0';

    for (hi = apr_hash_first(p, h); hi; hi = apr_hash_next(hi)) {
        apr_hash_this(hi,(void*) &key, &len, (void*) &val);
        apr_snprintf(str, str_max,
            "%s# Key %s (%" APR_SSIZE_T_FMT ") Value %s\n",
            "%s=%s\n",
            str, key, len, val,
            key, val);
        i++;
    }
    apr_snprintf(str, str_max, "%s# Total entries: %d\n", str, i);
}

void shsvnhash_sum(apr_pool_t *p, apr_hash_t *h,
    int *pcount, int *keySum, int *valSum)
{
    apr_hash_index_t *hi;
    void *val, *key;
    int count = 0;

    *keySum = 0;
    *valSum = 0;
    *pcount = 0;
    for (hi = apr_hash_first(p, h); hi; hi = apr_hash_next(hi)) {
        apr_hash_this(hi, (void*)&key, NULL, &val);
        *valSum += *(int *)val;
        *keySum += *(int *)key;
        count++;
    }
    *pcount=count;
}

/**
 * Creates a new APR hashmap.
 * @note All hashmaps generated per process session will use the same memory pool.
 * @returns Returns a @c apr_hash_t hashmap reference.
 */
apr_hash_t *shsvnhash_init(void)
{
    apr_hash_t *h = NULL;

    if (!svn_mempool) {
      apr_pool_initialize();
      apr_pool_create(&svn_mempool, NULL);
    }

    h = apr_hash_make(svn_mempool);
    return (h);
}

void shsvnhash_free(apr_hash_t *h)
{
 
}

/**
 * Called when program terminates.
 */
void shsvnhash_destroy(void)
{

  if (svn_mempool) {
    apr_pool_destroy(svn_mempool);
    svn_mempool = NULL;
  }

}

void shsvnhash_set(apr_hash_t *h, shkey_t key, void *value)
{
  apr_hash_set(h, &key, sizeof(shkey_t), value); 
}

//void shsvnhash_unset(apr_hash_t *h, shkey_t key)

void *shsvnhash_get(apr_hash_t *h, shkey_t key)
{
  void *value;

  value = apr_hash_get(h, &key, sizeof(shkey_t));

  return (value);
} 


/*** Dumping and loading hash files. */



