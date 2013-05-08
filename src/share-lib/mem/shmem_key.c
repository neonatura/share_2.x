
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

#define __MEM__SHMEM_KEY_C__
#include "share.h"

shkey_t *shkey_bin(char *data, size_t data_len)
{
  shkey_t *ret_key = (shkey_t *)calloc(1, sizeof(shkey_t));
  if (data && data_len)
    shkey_bin_r(data, data_len, ret_key);
  return (ret_key);
}

shkey_t *shkey_str(char *kvalue)
{
  if (!kvalue)
    return (shkey_bin(NULL, 0));
  return (shkey_bin(kvalue, strlen(kvalue)));
}

_TEST(shkey_str)
{
  shkey_t *key1;
  shkey_t *key2;

  /* compare unique keys */
  key1 = shkey_str("a");
  key2 = shkey_str("b");
  _TRUE(0 != memcmp(key1, key2, sizeof(shkey_t)));
  shkey_free(&key1);
  shkey_free(&key2);

  /* compare identical keys */
  key1 = shkey_str("a");
  key2 = shkey_str("a");
  _TRUE(0 == memcmp(key1, key2, sizeof(shkey_t)));
  shkey_free(&key1);
  shkey_free(&key2);

}

shkey_t *shkey_num(long kvalue)
{
  shkey_t *ret_key = (shkey_t *)calloc(1, sizeof(shkey_t));
  size_t len = sizeof(kvalue);
  shkey_bin_r(&kvalue, len, ret_key);
  return (ret_key);
}

_TEST(shkey_num)
{
  shkey_t *key1;
  shkey_t *key2;
  int num;

  /* compare unique numbers. */
  num = rand();
  key1 = shkey_num(num);
  key2 = shkey_num(num + 1);
  _TRUE(0 != memcmp(key1, key2, sizeof(shkey_t)));
  shkey_free(&key1);
  shkey_free(&key2);

  /* compare identical numbers. */
  num = rand();
  key1 = shkey_num(num);
  key2 = shkey_num(num);
  _TRUE(0 == memcmp(key1, key2, sizeof(shkey_t)));
  shkey_free(&key1);
  shkey_free(&key2);
}

shkey_t *shkey_uniq(void)
{
  static uint32_t uniq_of[SHKEY_WORDS];
  shkey_t *ret_key;
  int i;

  ret_key = (shkey_t *)calloc(1, sizeof(shkey_t));
  for (i = 0; i < SHKEY_WORDS; i++) {
    if (!uniq_of[i])
      uniq_of[i] = (uint32_t)rand();
    ret_key->code[i] = ++uniq_of[i];
  }

  return (ret_key);
}

_TEST(shkey_uniq)
{
  shkey_t *key1 = shkey_uniq();
  shkey_t *key2 = shkey_uniq();
  _TRUE(0 != memcmp(key1, key2, sizeof(shkey_t)));
  shkey_free(&key1);
  shkey_free(&key2);
}

shkey_t *ashkey_uniq(void)
{
  static uint32_t uniq_of[SHKEY_WORDS];
  static shkey_t ret_key;
  int i;

  memset(&ret_key, 0, sizeof(ret_key));
  for (i = 0; i < SHKEY_WORDS; i++) {
    if (!uniq_of[i])
      uniq_of[i] = (uint32_t)rand();
    ret_key.code[i] = ++uniq_of[i];
  }

  return (&ret_key);
}

_TEST(ashkey_uniq)
{
  shkey_t *key1 = shkey_uniq();
  shkey_t *key2 = shkey_uniq();
  _TRUE(0 != memcmp(key1, key2, sizeof(shkey_t)));
}

void shkey_free(shkey_t **key_p)
{
  if (key_p && *key_p) {
    free(*key_p);
    *key_p = NULL;
  }
}


shkey_t *ashkey_str(char *name)
{
  static shkey_t key[MAX_SHARE_THREADS];
  static int key_idx;
  int idx = ++key_idx;

  shkey_bin_r(name, strlen(name), &key[idx]);
  return (&key[idx]);
}

shkey_t *ashkey_num(long num)
{
  static shkey_t key[MAX_SHARE_THREADS];
  static int key_idx;
  int idx = ++key_idx;
  char buf[256];

  memset(buf, 0, sizeof(buf));
  memcpy(buf, &num, sizeof(num)); 
  shkey_bin_r(buf, strlen(buf), &key[idx]);
  return (&key[idx]);
}

int shkey_cmp(shkey_t *key_1, shkey_t *key_2)
{
  return (0 == memcmp(key_1, key_2, sizeof(uint32_t) * 4));
}

const char *shkey_print(shkey_t *key)
{
  static char ret_buf[4096];
  int i;

  memset(ret_buf, 0, sizeof(ret_buf));
  for (i = 0; i < SHKEY_WORDS; i++) {
    sprintf(ret_buf + strlen(ret_buf), "%-8.8x", key->code[i]);
  }
  
  return (ret_buf);
}

_TEST(shkey_print)
{
  shkey_t *key;
  char *ptr;

  key = shkey_uniq();
  _TRUEPTR(key);

  ptr = (char *)shkey_print(key);
  _TRUEPTR(ptr);
  _TRUE(strlen(ptr) == 32);
  _TRUE(strtoll(ptr, NULL, 16));

  shkey_free(&key);
}

#undef __MEM__SHMEM_KEY_C__



