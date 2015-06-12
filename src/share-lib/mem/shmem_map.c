
/*
 * @copyright
 *
 *  Copyright 2013 Neo Natura 
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
 *
 */


#include "share.h"


static shmap_entry_t **find_entry(shmap_t *ht,
                                     shkey_t *key,
                                     const void *val);


static shmap_entry_t **alloc_array(shmap_t *ht, size_t max)
{
  shmap_entry_t **ret_ar;

  ret_ar = (shmap_entry_t **)calloc(max+1, sizeof(shmap_entry_t *));

  return (ret_ar);
}

shmap_t *shmap_init(void)
{
    shmap_t *ht;
    ht = (shmap_t *)calloc(1, sizeof(shmap_t));
    ht->free = NULL;
    ht->count = 0;
    ht->max = INITIAL_MAX;
    ht->array = alloc_array(ht, ht->max);
    //ht->hash_func = shmapfunc_default;
    return ht;
}

_TEST(shmap_init)
{
  shmap_t *meta = shmap_init();

  CuAssertPtrNotNull(ct, meta);
  if (!meta)
    return;

  CuAssertTrue(ct, meta->count == 0);
  CuAssertTrue(ct, meta->max > 0);
  CuAssertPtrNotNull(ct, meta->array);
//  CuAssertPtrNotNull(ct, meta->hash_func);

  shmap_free(&meta);
}

void shmap_free(shmap_t **meta_p)
{
  shmap_t *meta;
  int i;
  
  if (!meta_p)
    return;

  meta = *meta_p;
  *meta_p = NULL;
  if (!meta)
    return;

  for (i = 0; i <= meta->max; i++) {
    shmap_entry_t *entry = meta->array[i];
    if (entry && entry->key)
      free(entry->key);
  }

  free(meta->array);
  free(meta);

}
void shmap_free_values(shmap_t *meta)
{
  int i;
  
  if (!meta)
    return;

  for (i = 0; i <= meta->max; i++) {
    shmap_entry_t *entry = meta->array[i];
    if (entry && entry->key)
      free((void *)entry->val);
  }


}

shmap_t *shmap_init_custom(shmapfunc_t hash_func)
{
    shmap_t *ht = shmap_init();
    ht->hash_func = hash_func;
    return ht;
}


/*
 * Hash iteration functions.
 */

shmap_index_t *shmap_next(shmap_index_t *hi)
{
    hi->tthis = hi->next;
    while (!hi->tthis) {
        if (hi->index > hi->ht->max)
            return NULL;

        hi->tthis = hi->ht->array[hi->index++];
    }
    hi->next = hi->tthis->next;
    return hi;
}

shmap_index_t *shmap_first(shmap_t *ht)
{
  shmap_index_t *hi;

  if (!ht)
    return (NULL);

  hi = &ht->iterator;

  hi->ht = ht;
  hi->index = 0;
  hi->tthis = NULL;
  hi->next = NULL;

  return shmap_next(hi);
}

void shmap_this(shmap_index_t *hi, const void **key, ssize_t *klen, void **val)
{
    if (key)  *key  = hi->tthis->key;
    if (klen) *klen = hi->tthis->klen;
    if (val)  *val  = (void *)hi->tthis->val;
}


/*
 * Expanding a hash table
 */
static void _expand_array(shmap_t *ht)
{
  shmap_index_t *hi;
  shmap_entry_t **new_array;
  unsigned int new_max;

  new_max = ht->max * 2;
  new_array = alloc_array(ht, new_max);
  for (hi = shmap_first(ht); hi; hi = shmap_next(hi)) {
    unsigned int i = hi->tthis->hash & new_max;
    hi->tthis->next = new_array[i];
    new_array[i] = hi->tthis;
  }
  if (ht->array)
    free(ht->array);
  ht->array = new_array;
  ht->max = new_max;
}



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
static unsigned int shmap_hash_num(unsigned char *key)
{
  ssize_t klen = sizeof(shkey_t);
  unsigned int hash = 0;
  const unsigned char *p;
  ssize_t i;

  for (p = key, i = klen; i; i--, p++) {
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
static shmap_entry_t **find_entry(shmap_t *ht, shkey_t *key, const void *val)
{
  ssize_t klen = sizeof(shkey_t);
  shmap_entry_t **hep, *he;
  unsigned int hash;

  if (!key) {
    PRINT_ERROR(SHERR_INVAL, "shemta_get_void [SHPF_REFERENCE]");
    return (NULL);
  }

  hash = shmap_hash_num((unsigned char *)key);

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
    he = (shmap_entry_t *)calloc(1, sizeof(shmap_entry_t));
  he->next = NULL;
  he->hash = hash;
  he->key  = shkey_clone(key);
  he->klen = klen;
  he->val  = val;
  *hep = he;
  ht->count++;
  return hep;
}



/*
APR_DECLARE(shmap_t *) shmap_copy(apr_pool_t *pool,
                                        const shmap_t *orig)
{
    shmap_t *ht;
    shmap_entry_t *new_vals;
    unsigned int i, j;

    ht = apr_palloc(pool, sizeof(shmap_t) +
                    sizeof(*ht->array) * (orig->max + 1) +
                    sizeof(shmap_entry_t) * orig->count);
    ht->pool = pool;
    ht->free = NULL;
    ht->count = orig->count;
    ht->max = orig->max;
    ht->hash_func = orig->hash_func;
    ht->array = (shmap_entry_t **)((char *)ht + sizeof(shmap_t));

    new_vals = (shmap_entry_t *)((char *)(ht) + sizeof(shmap_t) +
                                    sizeof(*ht->array) * (orig->max + 1));
    j = 0;
    for (i = 0; i <= ht->max; i++) {
        shmap_entry_t **new_entry = &(ht->array[i]);
        shmap_entry_t *orig_entry = orig->array[i];
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

char *shmap_get_str(shmap_t *h, shkey_t *key)
{
  unsigned char *data;

  data = (unsigned char *)shmap_get(h, key);
  if (!data)
    return (NULL);

  return (data + sizeof(shmap_value_t));
}

void *shmap_get_ptr(shmap_t *h, shkey_t *key)
{
  shmap_value_t *hdr;
  unsigned char *data;
  void *ptr;

  if (!h)
    return (NULL);

  data = (unsigned char *)shmap_get(h, key);
  if (!data)
    return (NULL);

  hdr = (shmap_value_t *)data;
  if (hdr->pf != SHPF_REFERENCE) {
    PRINT_ERROR(SHERR_ILSEQ, "shemta_get_void [SHPF_REFERENCE]");
    return (NULL);
  }

  memcpy(&ptr, (data + sizeof(shmap_value_t)), sizeof(void *));
  return (ptr);
}

void **shmap_get_ptr_list(shmap_t *h)
{
  shmap_index_t *hi;
  shmap_value_t *hdr;
  void **ret_list;
  ssize_t len;
  char *key;
  void *ptr;
  char *val;
  int idx;

  if (!h)
    return (NULL);

  ret_list = (void *)calloc(shmap_count(h) + 1, sizeof(void *));
  if (!ret_list)
    return (NULL);

  idx = 0;
  for (hi = shmap_first(h); hi; hi = shmap_next(hi)) {
    shmap_this(hi,(void*) &key, &len, (void*) &val);
    hdr = (shmap_value_t *)val;
    if (hdr->pf != SHPF_REFERENCE)
      continue;

    memcpy(&ptr, (val + sizeof(shmap_value_t)), sizeof(void *));
    ret_list[idx] = ptr;
    idx++;
  }

  return (ret_list);
}

void *shmap_get_void(shmap_t *h, shkey_t *key)
{
  shmap_value_t *hdr;
  unsigned char *data;

  if (!h)
    return (NULL);

  data = (unsigned char *)shmap_get(h, key);
  if (!data)
    return (NULL);

  hdr = (shmap_value_t *)data;
  if (hdr->pf != SHPF_BINARY) {
    PRINT_ERROR(SHERR_ILSEQ, "shemta_get_void [SHPF_BINARY]");
    return (NULL);
  }

  return (data + sizeof(shmap_value_t));
}

void *shmap_get(shmap_t *ht, shkey_t *key)
{
  shmap_entry_t *he;

  if (!ht)
    return (NULL);

  he = *find_entry(ht, key, NULL);
  if (he) {
    return (void *)he->val;
  } else {
    return NULL;
  }
}

void shmap_set(shmap_t *ht, shkey_t *key, const void *val)
{
  shmap_entry_t **hep;

  if (!ht || !key)
    return; /* all done */

  hep = find_entry(ht, key, val);
  if (!hep)
    return;

  if (*hep) {
    if (!val) {
      /* delete entry */
      shmap_entry_t *old = *hep;
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
        _expand_array(ht);
      }
    }
  }
  /* else key not present and val==NULL */
}

_TEST(shmap_set)
{
  shmap_t *h;
  shmap_value_t *hdr;
  shkey_t *key;
  char *data;

  _TRUEPTR(h = shmap_init());
  if (!h)
    return;

  key = shkey_str("shmap_set");
  data = (char *)calloc(256, sizeof(char));
  hdr = (shmap_value_t *)data;
  hdr->pf = SHPF_BINARY;
  memcpy(data + sizeof(shmap_value_t *), VERSION, strlen(VERSION));
  hdr->sz = strlen(VERSION) + 1;
  shmap_set(h, key, data);
  _TRUEPTR(shmap_get(h, key));

  shmap_free(&h);
}

void shmap_set_str(shmap_t *h, shkey_t *key, char *value)
{
  shmap_value_t *def;
  shmap_value_t *hdr;
  char *data;
  size_t data_len;

  if (!h)
    return;

  if (!value) {
    PRINT_RUSAGE("shmap_set_str: null value");
    shmap_set(h, key, NULL);
    return;
  }

  data = (char *)shmap_get(h, key);
  if (data) {
    free(data);
  }

  data_len = MAX(1024, 
      sizeof(shmap_value_t) + strlen(value) + SHMEM_PAD_SIZE);
  data = (char *)calloc(data_len, sizeof(char));

  hdr = (shmap_value_t *)data;
  hdr->pf = SHPF_STRING;
  hdr->sz = strlen(value) + 1;

  strncpy(data + sizeof(shmap_value_t), value, strlen(value));

  shmap_set(h, key, data);

}

_TEST(shmap_set_str)
{
  shmap_t *h;
  shkey_t *key;
  char buf[256];
  char *ptr = NULL;
  char *ptr2 = NULL;

  _TRUEPTR(h = shmap_init());
  if (!h)
    return;

  memset(buf, 0, sizeof(buf));
  memset(buf, 'a', sizeof(buf) - 1);

  key = shkey_str("shmap_set_str");
  _TRUE(!shmap_get_str(h, key));

  shmap_set_str(h, key, buf);
  _TRUEPTR(shmap_get_str(h, key));
  _TRUEPTR(ptr = shmap_get_str(h, key));
  if (ptr)
    _TRUE(0 == strcmp(buf, ptr));
  
  shkey_free(&key);
  shmap_free(&h);
}

void shmap_unset_str(shmap_t *h, shkey_t *name)
{
  shmap_set(h, name, NULL);
}

void shmap_set_ptr(shmap_t *ht, shkey_t *key, void *ptr)
{
  shmap_value_t *hdr;
  unsigned char *meta_data;
  size_t data_len;

  if (!ht)
    return;

  data_len = sizeof(void *);
  meta_data = (unsigned char *)calloc(data_len + sizeof(shmap_value_t) + SHMEM_PAD_SIZE, sizeof(unsigned char)); 
  hdr = (shmap_value_t *)meta_data;
  hdr->pf = SHPF_REFERENCE;
  hdr->sz = data_len;
  memcpy(meta_data + sizeof(shmap_value_t), &ptr, data_len);

  shmap_set(ht, key, meta_data);
}

void shmap_set_void(shmap_t *ht, shkey_t *key, void *data, size_t data_len)
{
  shmap_value_t *hdr;
  unsigned char *meta_data;

  if (!ht)
    return;

  meta_data = (unsigned char *)calloc(data_len + sizeof(shmap_value_t) + SHMEM_PAD_SIZE, sizeof(unsigned char)); 
  hdr = (shmap_value_t *)meta_data;
  hdr->pf = SHPF_BINARY;
  hdr->sz = data_len;
  memcpy(meta_data + sizeof(shmap_value_t), data, data_len);

  shmap_set(ht, key, meta_data);
}

_TEST(shmap_set_void)
{
  shkey_t *key;
  shmap_t *ht;
  char buf[256];
  char *ptr;
  char *ptr2;


  _TRUEPTR(ht = shmap_init());
  if (!ht)
    return;

  key = shkey_str("shmap_set_void");
  _TRUE(!shmap_get_void(ht, key));

  memset(buf, 0, sizeof(buf));
  strcpy(buf, VERSION);

  shmap_set_void(ht, key, buf, strlen(VERSION));
  ptr = shmap_get_void(ht, key);
  _TRUEPTR(ptr);
  if (ptr)
    _TRUE(0 == memcmp(ptr, VERSION, strlen(VERSION)));

  shkey_free(&key);
  shmap_free(&ht);
}

void shmap_unset_ptr(shmap_t *h, shkey_t *key)
{
  shmap_set(h, key, NULL);
}

_TEST(shmap_unset_ptr)
{
  shmap_t *meta;
  shkey_t *key;
  int i;
  char *str;

  meta = shmap_init();
  _TRUEPTR(meta);
  
  for (i = 0; i < 64; i++) {
    key = shkey_num(i);
    str = strdup("blah");
    shmap_set_ptr(meta, key, str);
    shkey_free(&key);
  }

  for (i = 0; i < 64; i++) {
    key = shkey_num(i);
    str = shmap_get_ptr(meta, key);
    shmap_unset_ptr(meta, key);
    free(str);
    shkey_free(&key);
  }

  for (i = 0; i < 64; i++) {
    key = shkey_num(i);
    _TRUE(!shmap_get_ptr(meta, key));
    shkey_free(&key);
  }

  shmap_free(&meta);
}

void shmap_unset_void(shmap_t *h, shkey_t *key)
{
  shmap_set(h, key, NULL);
}

unsigned int shmap_count(shmap_t *ht)
{
  if (!ht)
    return (0);
  return ht->count;
}

_TEST(shmap_count)
{
  shmap_t *meta = shmap_init();
  shmap_value_t val;
  shkey_t *key;

  _TRUEPTR(meta = shmap_init());
  if (!meta)
    return;

  key = shkey_str("shmap_count");
  _TRUE(shmap_count(meta) == 0);
  shmap_set_str(meta, key, VERSION);
   _TRUE(shmap_count(meta) != 0);

  shkey_free(&key);
  shmap_free(&meta);
}

void shmap_print(shmap_t *h, shbuf_t *ret_buff)
{
  shmap_entry_t *ent;
  char buf[256];
  int idx;
  int i;
  shmap_index_t *hi;
  shmap_value_t *hdr;
  char *val, *key;
  ssize_t len;
  char str[4096];

  if (!h || !ret_buff)
    return; /* all done */

  i = 0;

  for (hi = shmap_first(h); hi; hi = shmap_next(hi)) {
    shmap_this(hi,(void*) &key, &len, (void*) &val);

    hdr = (shmap_value_t *)val;
    memcpy(&hdr->name, key, sizeof(shkey_t));
    shbuf_cat(ret_buff, hdr, sizeof(shmap_value_t));
    shbuf_cat(ret_buff, ((char *)val + sizeof(shmap_value_t)), hdr->sz);

    i++;
  }

}

_TEST(shmap_print)
{
  shmap_t *meta = shmap_init();
  shbuf_t *buff;
  shkey_t *key;
  char *value;

  CuAssertPtrNotNull(ct, meta);
  if (!meta)
    return;

  _TRUEPTR(buff = shbuf_init());

  key = shkey_uniq(); 
  value = strdup("value");
  shmap_set_str(meta, key, value);
  shmap_print(meta, buff);
  _TRUEPTR(buff->data);
  _TRUE(buff->data_of > 0);
  _TRUE(buff->data_max > 0);

  shbuf_free(&buff);
  shkey_free(&key);
  free(value);
}




