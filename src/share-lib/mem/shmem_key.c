
/*
 * @copyright
 *
 *  Copyright 2013, 2014 Neo Natura
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

#define __MEM__SHMEM_KEY_C__
#include "share.h"

uint32_t _shkey_blank[8];

static void shkey_bin_r(void *data, size_t data_len, shkey_t *key)
{
  uint64_t crc;
  size_t step;
  int i;

  crc = shcrc(data, data_len);

  step = data_len / SHKEY_WORDS;
  for (i = 0; i < SHKEY_WORDS; i++) {
    crc += shcrc(data + (step*i), step);
    key->code[i] = (uint32_t)htonl(crc);
  }

}

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
    ret_key->code[i] = (uint32_t)htonl(++uniq_of[i]);
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
  static shkey_t key;

  memset(&key, 0, sizeof(key));
  if (name && strlen(name))
    shkey_bin_r(name, strlen(name), &key);

  return (&key);
}

shkey_t *ashkey_num(long num)
{
  static shkey_t key;
  char buf[256];

  memset(buf, 0, sizeof(buf));
  memcpy(buf, &num, sizeof(num)); 
  memset(&key, 0, sizeof(key));
  shkey_bin_r(buf, strlen(buf), &key);

  return (&key);
}

int shkey_cmp(shkey_t *key_1, shkey_t *key_2)
{
  return (0 == memcmp(key_1, key_2, sizeof(uint32_t) * 4));
}

#if 0
const char *shkey_print(shkey_t *key)
{
  static char ret_str[1024];
  int i;

  memset(ret_str, 0, sizeof(ret_str));
  for (i = 0; i < SHKEY_WORDS; i++) {
    strcat(ret_str, shcrcstr((uint64_t)key->code[i]));
  }

  return (ret_str);
}
#endif

const char *shkey_print(shkey_t *key)
{
  static char ret_str[256];
  int i;

  memset(ret_str, 0, sizeof(ret_str));
  for (i = 0; i < SHKEY_WORDS; i++) {
    sprintf(ret_str+strlen(ret_str), "%6.6s",
        shcrcstr((uint64_t)key->code[i]));
  }
  for (i = 0; i < 256; i++)
    if (ret_str[i] == ' ')
      ret_str[i] = '.';
  return (ret_str);
}

shkey_t *shkey_clone(shkey_t *key)
{
  shkey_t *ret_key;

  if (!key)
    return (NULL);

  ret_key = (shkey_t *)calloc(1, sizeof(shkey_t));
  memcpy(ret_key, key, sizeof(shkey_t));
  return (ret_key);
}

shkey_t *shkey_cert(shkey_t *key, uint64_t crc, shtime_t stamp)
{
  unsigned char shabuf[64];
  unsigned char keybuf[64];
  char *ptr;
  uint32_t *sha_ar;
  int i;

  if (!key)
    return (NULL);

  memset(shabuf, 0, 64);

  memcpy(shabuf, &crc, sizeof(uint64_t));
  memcpy(shabuf + 16, &stamp, sizeof(shtime_t));
  memcpy(shabuf + 32, key, sizeof(shkey_t));
  sha_ar = (uint32_t *)shabuf;
  for (i = 0; i < 16; i++) {
    sha_ar[i] = htonl(sha_ar[i]);
  }

  memset(keybuf, 0, sizeof(keybuf));
  sh_sha256(shabuf, sizeof(shabuf), keybuf);

  return (shkey_bin(keybuf, sizeof(keybuf)));
}

_TEST(shkey_cert)
{
  shpeer_t *peer;
  shkey_t *key;
  shkey_t *peer_key;
  uint64_t crc = 1;

  peer = shpeer_init(NULL, NULL);
  _TRUEPTR(peer);
  peer_key = shpeer_kpub(peer);
  key = shkey_cert(peer_key, crc, 0);
  _TRUEPTR(key);
  shkey_free(&key);
  shpeer_free(&peer);
}

int shkey_verify(shkey_t *sig, uint64_t crc, shkey_t *key, shtime_t stamp)
{
  shkey_t *sha_key;
  char *ptr;
  int valid;

  sha_key = shkey_cert(key, crc, stamp);

  valid = shkey_cmp(sha_key, sig);
  shkey_free(&sha_key);
  if (!valid)
    return (SHERR_INVAL);

  return (0);
}

_TEST(shkey_verify)
{
  shpeer_t *peer;
  shkey_t *key;
  shkey_t *peer_key;
  shbuf_t *data;
  uint64_t crc = 1;

  peer = shpeer_init(NULL, NULL);
  data = shbuf_init();
  shbuf_catstr(data, "shkey_verify");
  peer_key = shpeer_kpub(peer);
  key = shkey_cert(peer_key, crc, 0);
  _TRUEPTR(key);
  _TRUE(0 == shkey_verify(key, crc, peer_key, 0));
  shkey_free(&key);
  shpeer_free(&peer);
}

shkey_t *shkey_gen(char *str)
{
  shkey_t *ret_key;
  char buf[256];
  char *ptr;
  int i;

  if (!str || strlen(str) != 36)
    return (NULL);

  ret_key = (shkey_t *)calloc(1, sizeof(shkey_t));

  for (i = 0; i < SHKEY_WORDS; i++) {
    memset(buf, 0, sizeof(buf));
    strncpy(buf, str + (i * 6), 6);
    ptr = buf;
    while (*ptr && *ptr == '.')
      ptr++;
    ret_key->code[i] = (uint32_t)shcrcgen(ptr);
  }

  return (ret_key);
}

_TEST(shkey_gen)
{
  shkey_t *key;
  shkey_t *cmp_key;
  char buf[256];

  key = shkey_str("shkey_gen");

  memset(buf, 0, sizeof(buf));
  strncpy(buf, shkey_print(key), sizeof(buf)-1);

  cmp_key = shkey_gen(buf);
  _TRUE(shkey_cmp(cmp_key, key));
  shkey_free(&cmp_key);

  shkey_free(&key);
}

const char *shkey_hex(shkey_t *key)
{
  static char ret_buf[256];
  int i;

  memset(ret_buf, 0, sizeof(ret_buf));

  if (key) {
    for (i = 0; i < SHKEY_WORDS; i++) {
      sprintf(ret_buf + strlen(ret_buf), "%-8.8x", key->code[i]);
    }
  }
  
  return (ret_buf);
}

_TEST(shkey_hex)
{
  shkey_t *key;
  char *ptr;

  key = shkey_uniq();
  _TRUEPTR(key);

  ptr = (char *)shkey_hex(key);
  _TRUEPTR(ptr);
  _TRUE(strlen(ptr) == 48);
  _TRUE(strtoll(ptr, NULL, 16));

  shkey_free(&key);
}

shkey_t *shkey_hexgen(char *hex_str)
{
  shkey_t *ret_key;
  char buf[256];
  int i;

  ret_key = (shkey_t *)calloc(1, sizeof(shkey_t));
  if (hex_str && *hex_str) {
    for (i = 0; i < SHKEY_WORDS; i++) {
      memset(buf, 0, sizeof(buf));
      strncpy(buf, hex_str + (8 * i), 8);
      ret_key->code[i] = (uint32_t)strtol(buf, NULL, 16);
    }
  }

  return (ret_key);
}

_TEST(shkey_hexgen)
{
  shkey_t *key;
  shkey_t *cmp_key;
  char buf[256];

  key = shkey_str("shkey_hexgen");

  memset(buf, 0, sizeof(buf));
  strncpy(buf, shkey_hex(key), sizeof(buf)-1);
  _TRUE(strlen(buf) == 48);
  cmp_key = shkey_hexgen(buf);
  _TRUE(shkey_cmp(cmp_key, key));
  shkey_free(&cmp_key);

  shkey_free(&key);
}

int shrand(void)
{
  shkey_t *key;
  int val;
  int i;

  val = 0;
  key = shkey_uniq();
  for (i = 0; i < SHKEY_WORDS; i++)
    val += (int)key->code[i];
  shkey_free(&key);

  return (val);
}


#undef __MEM__SHMEM_KEY_C__



