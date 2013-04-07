
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
 */


#include "share.h"

static shmeta_entry_t **alloc_array(shmeta_t *ht, unsigned int max)
{
  shmeta_entry_t **ret_ar;

  if (!ht->array) {
    ret_ar = (shmeta_entry_t **)calloc(max+1, sizeof(shmeta_entry_t *));
  } else {
    ret_ar = (shmeta_entry_t **)realloc(ht->array, max+1 * sizeof(shmeta_entry_t *));
  }
  return (ret_ar);
}

shmeta_t *shmeta_init(void)
{
    shmeta_t *ht;
    ht = (shmeta_t *)calloc(1, sizeof(shmeta_t));
    ht->free = NULL;
    ht->count = 0;
    ht->max = INITIAL_MAX;
    ht->array = alloc_array(ht, ht->max);
    ht->hash_func = shmetafunc_default;
    return ht;
}

_TEST(shmeta_init)
{
  shmeta_t *meta = shmeta_init();

  CuAssertPtrNotNull(ct, meta);
  if (!meta)
    return;

  CuAssertTrue(ct, meta->count == 0);
  CuAssertTrue(ct, meta->max > 0);
  CuAssertPtrNotNull(ct, meta->array);
  CuAssertPtrNotNull(ct, meta->hash_func);

  shmeta_free(&meta);
}

void shmeta_free(shmeta_t **meta_p)
{
  shmeta_t *meta;
  
  if (!meta_p)
    return;

  meta = *meta_p;
  *meta_p = NULL;
  if (!meta)
    return;

  free(meta->array);
  free(meta);
}

shmeta_t *shmeta_init_custom(shmetafunc_t hash_func)
{
    shmeta_t *ht = shmeta_init();
    ht->hash_func = hash_func;
    return ht;
}


/*
 * Hash iteration functions.
 */

shmeta_index_t *shmeta_next(shmeta_index_t *hi)
{
    hi->this = hi->next;
    while (!hi->this) {
        if (hi->index > hi->ht->max)
            return NULL;

        hi->this = hi->ht->array[hi->index++];
    }
    hi->next = hi->this->next;
    return hi;
}

shmeta_index_t *shmeta_first(shmeta_t *ht)
{
    shmeta_index_t *hi;

    hi = &ht->iterator;

    hi->ht = ht;
    hi->index = 0;
    hi->this = NULL;
    hi->next = NULL;
    return shmeta_next(hi);
}

void shmeta_this(shmeta_index_t *hi, void **key, ssize_t *klen, void **val)
{
    if (key)  *key  = hi->this->key;
    if (klen) *klen = hi->this->klen;
    if (val)  *val  = (void *)hi->this->val;
}


/*
 * Expanding a hash table
 */

static void expand_array(shmeta_t *ht)
{
    shmeta_index_t *hi;
    shmeta_entry_t **new_array;
    unsigned int new_max;

    new_max = ht->max * 2 + 1;
    new_array = alloc_array(ht, new_max);
    for (hi = shmeta_first(ht); hi; hi = shmeta_next(hi)) {
        unsigned int i = hi->this->hash & new_max;
        hi->this->next = new_array[i];
        new_array[i] = hi->this;
    }
    ht->array = new_array;
    ht->max = new_max;
}

unsigned int shmetafunc_default(const char *char_key, ssize_t *klen)
{
    unsigned int hash = 0;
    const unsigned char *key = (const unsigned char *)char_key;
    const unsigned char *p;
    ssize_t i;
    
    /*
     * This is the popular `times 33' hash algorithm which is used by
     * perl and also appears in Berkeley DB. This is one of the best
     * known hash functions for strings because it is both computed
     * very fast and distributes very well.
     *
     * The originator may be Dan Bernstein but the code in Berkeley DB
     * cites Chris Torek as the source. The best citation I have found
     * is "Chris Torek, Hash function for text in C, Usenet message
     * <27038@mimsy.umd.edu> in comp.lang.c , October, 1990." in Rich
     * Salz's USENIX 1992 paper about INN which can be found at
     * <http://citeseer.nj.nec.com/salz92internetnews.html>.
     *
     * The magic of number 33, i.e. why it works better than many other
     * constants, prime or not, has never been adequately explained by
     * anyone. So I try an explanation: if one experimentally tests all
     * multipliers between 1 and 256 (as I did while writing a low-level
     * data structure library some time ago) one detects that even
     * numbers are not useable at all. The remaining 128 odd numbers
     * (except for the number 1) work more or less all equally well.
     * They all distribute in an acceptable way and this way fill a hash
     * table with an average percent of approx. 86%.
     *
     * If one compares the chi^2 values of the variants (see
     * Bob Jenkins ``Hashing Frequently Asked Questions'' at
     * http://burtleburtle.net/bob/hash/hashfaq.html for a description
     * of chi^2), the number 33 not even has the best value. But the
     * number 33 and a few other equally good numbers like 17, 31, 63,
     * 127 and 129 have nevertheless a great advantage to the remaining
     * numbers in the large set of possible multipliers: their multiply
     * operation can be replaced by a faster operation based on just one
     * shift plus either a single addition or subtraction operation. And
     * because a hash function has to both distribute good _and_ has to
     * be very fast to compute, those few numbers should be preferred.
     *
     *                  -- Ralf S. Engelschall <rse@engelschall.com>
     */
     
/*
    if (*klen == APR_HASH_KEY_STRING) {
        for (p = key; *p; p++) {
            hash = hash * 33 + *p;
        }
        *klen = p - key;
    }
    else {
        for (p = key, i = *klen; i; i--, p++) {
            hash = hash * 33 + *p;
        }
    }
*/
        for (p = key, i = *klen; i; i--, p++) {
            hash = hash * 33 + *p;
        }

    return hash;
}


/*
 * This is where we keep the details of the hash function and control
 * the maximum collision rate.
 *
 * If val is non-NULL it creates and initializes a new hash entry if
 * there isn't already one there; it returns an updatable pointer so
 * that hash entries can be removed.
 */

static shmeta_entry_t **find_entry(shmeta_t *ht,
                                     const void *key,
                                     ssize_t klen,
                                     const void *val)
{
    shmeta_entry_t **hep, *he;
    unsigned int hash;

    hash = ht->hash_func(key, &klen);

    /* scan linked list */
    for (hep = &ht->array[hash & ht->max], he = *hep;
         he; hep = &he->next, he = *hep) {
        if (he->hash == hash
            && he->klen == klen
            && memcmp(he->key, key, klen) == 0)
            break;
    }
    if (he || !val)
        return hep;

    /* add a new entry for non-NULL values */
    if ((he = ht->free) != NULL)
        ht->free = he->next;
    else
        he = (shmeta_entry_t *)calloc(1, sizeof(shmeta_entry_t));
    he->next = NULL;
    he->hash = hash;
    he->key  = key;
    he->klen = klen;
    he->val  = val;
    *hep = he;
    ht->count++;
    return hep;
}

/*
APR_DECLARE(shmeta_t *) shmeta_copy(apr_pool_t *pool,
                                        const shmeta_t *orig)
{
    shmeta_t *ht;
    shmeta_entry_t *new_vals;
    unsigned int i, j;

    ht = apr_palloc(pool, sizeof(shmeta_t) +
                    sizeof(*ht->array) * (orig->max + 1) +
                    sizeof(shmeta_entry_t) * orig->count);
    ht->pool = pool;
    ht->free = NULL;
    ht->count = orig->count;
    ht->max = orig->max;
    ht->hash_func = orig->hash_func;
    ht->array = (shmeta_entry_t **)((char *)ht + sizeof(shmeta_t));

    new_vals = (shmeta_entry_t *)((char *)(ht) + sizeof(shmeta_t) +
                                    sizeof(*ht->array) * (orig->max + 1));
    j = 0;
    for (i = 0; i <= ht->max; i++) {
        shmeta_entry_t **new_entry = &(ht->array[i]);
        shmeta_entry_t *orig_entry = orig->array[i];
        while (orig_entry) {
            *new_entry = &new_vals[j++];
            (*new_entry)->hash = orig_entry->hash;
            (*new_entry)->key = orig_entry->key;
            (*new_entry)->klen = orig_entry->klen;
            (*new_entry)->val = orig_entry->val;
            new_entry = &((*new_entry)->next);
            orig_entry = orig_entry->next;
        }
        *new_entry = NULL;
    }
    return ht;
}
*/

void *shmeta_get(shmeta_t *ht, shkey_t sh_k)
{
  void *key = &sh_k;
  ssize_t klen = sizeof(sh_k);
  shmeta_entry_t *he;

  he = *find_entry(ht, key, klen, NULL);
  if (he)
    return (void *)he->val;
  else
    return NULL;
}

void shmeta_set(shmeta_t *ht, shkey_t sh_k, shmeta_value_t *val)
{
  const void *key = &sh_k;
  ssize_t klen = sizeof(sh_k);
  shmeta_entry_t **hep;

  hep = find_entry(ht, key, klen, val);
  if (*hep) {
    if (!val) {
      /* delete entry */
      shmeta_entry_t *old = *hep;
      *hep = (*hep)->next;
      old->next = ht->free;
      ht->free = old;
      --ht->count;
    }
    else {
      /* replace entry */
      (*hep)->val = val;
      /* check that the collision rate isn't too high */
      if (ht->count > ht->max) {
        expand_array(ht);
      }
    }
  }
  /* else key not present and val==NULL */
}

_TEST(shmeta_set)
{
  shmeta_t *meta = shmeta_init();
  shmeta_value_t *meta_val;
  shmeta_value_t val;
  shkey_t key;
  int i;

  CuAssertPtrNotNull(ct, meta);
  if (!meta)
    return;

  for (i = 0; i < 1024; i++) {
    memset(&val, (char)(i % 255), sizeof(val));
    memset(&key, (char *)&val, sizeof(key));
    shmeta_set(meta, key, &val); 
  }

  for (i = 0; i < 1024; i++) {
    memset(&val, (char)(i % 255), sizeof(val));
    memset(&key, (char *)&val, sizeof(key));
    meta_val = shmeta_get(meta, key);
    CuAssertTrue(ct, 0 == memcmp(meta_val, &val, sizeof(shmeta_value_t)));
  }

  shmeta_free(&meta);
}


unsigned int shmeta_count(shmeta_t *ht)
{
    return ht->count;
}

_TEST(shmeta_count)
{
  shmeta_t *meta = shmeta_init();
  shmeta_value_t val;

  CuAssertPtrNotNull(ct, meta);
  if (!meta)
    return;

  CuAssertTrue(ct, shmeta_count(meta) == 0);

  memset(&val, 0, sizeof(val));
  shmeta_set(meta, 1, &val); 
  CuAssertTrue(ct, shmeta_count(meta) != 0);

  shmeta_free(&meta);
}

void shmeta_print(shmeta_t *h, shbuf_t *ret_buff)
{
  shmeta_entry_t *ent;
  char buf[256];
  int idx;
  int i;
  shmeta_index_t *hi;
  char *val, *key;
  ssize_t len;
  char str[4096];

  if (!h || !ret_buff)
    return; /* all done */

  i = 0;

  shbuf_cat(ret_buff, "[", 1);
  for (hi = shmeta_first(h); hi; hi = shmeta_next(hi)) {
    shmeta_this(hi,(void*) &key, &len, (void*) &val);

    if (ret_buff->data_of > 1)
      shbuf_cat(ret_buff, ",", 1);

    memset(str, 0, sizeof(str));
    snprintf(str, sizeof(str) - 1, "\"%lx\"=\"%lx\"", key, val);
    shbuf_cat(ret_buff, str, strlen(str));
    i++;
  }
  shbuf_cat(ret_buff, "]", 1);

}

_TEST(shmeta_print)
{
  shmeta_t *meta = shmeta_init();
  shbuf_t *buff;

  CuAssertPtrNotNull(ct, meta);
  if (!meta)
    return;

  _TRUEPTR(buff = shbuf_init());

  if (buff) {
    shmeta_print(meta, buff);
    CuAssertPtrNotNull(ct, buff->data);
    CuAssertPtrNotNull(ct, buff->data_of > 0);
    CuAssertPtrNotNull(ct, buff->data_max > 0);
  }

  shbuf_free(&buff);
} 




