
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

#define __MEM__SHMEM_CRYPT_C__
#include "share.h"

static uint32_t _crypt_magic = SHMEM_MAGIC;
#define CRYPT_MAGIC_SIZE sizeof(_crypt_magic)
#define IS_CRYPT_MAGIC(_data) \
  (0 == memcmp(_data, &_crypt_magic, CRYPT_MAGIC_SIZE))
#define SET_CRYPT_MAGIC(_data) \
  (memcpy(_data, &_crypt_magic, CRYPT_MAGIC_SIZE))

/**
 *   Decrypt 64 bits with a 128 bit key using TEA
 *   From http://en.wikipedia.org/wiki/Tiny_Encryption_Algorithm
 * Arguments:
 *   v - array of two 32 bit uints to be decoded in place
 *   k - array of four 32 bit uints to act as key
 * Returns:
 *   v - decrypted result
 * Side effects:
 *   None
 */
static void TEA_decrypt (uint32_t* v, uint32_t* k) 
{
    uint32_t v0=v[0], v1=v[1], sum=0xC6EF3720, i;  /* set up */
    uint32_t delta=0x9e3779b9;                     /* a key schedule constant */
    uint32_t k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
    for (i=0; i<32; i++) {                         /* basic cycle start */
        v1 -= ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
        v0 -= ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
        sum -= delta;                                   
    }                                              /* end cycle */
    v[0]=v0; v[1]=v1;
}

/**
 *   Encrypt 64 bits with a 128 bit key using TEA
 *   From http://en.wikipedia.org/wiki/Tiny_Encryption_Algorithm
 * Arguments:
 *   v - array of two 32 bit uints to be encoded in place
 *   k - array of four 32 bit uints to act as key
 * Returns:
 *   v - encrypted result
 * Side effects:
 *   None
 */
void TEA_encrypt (uint32_t* v, uint32_t* k) 
{
    uint32_t v0=v[0], v1=v[1], sum=0, i;           /* set up */
    uint32_t delta=0x9e3779b9;                     /* a key schedule constant */
    uint32_t k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
    for (i=0; i < 32; i++) {                       /* basic cycle start */
        sum += delta;
        v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
        v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);  
    }                                              /* end cycle */
    v[0]=v0; v[1]=v1;
}

static void TEA_encryptBlock(uint8_t *data, uint32_t * len, uint32_t * key)
{
   uint32_t blocks, i;
   uint32_t * data32;

   // treat the data as 32 bit unsigned integers
   data32 = (uint32_t *) data;

   // Find the number of 8 byte blocks, add one for the length
   blocks = (((*len) + 7) / 8) + 1;

   // Set the last block to the original data length
   data32[(blocks*2) - 1] = *len;

   // Set the encrypted data length
   *len = blocks * 8;

   for(i = 0; i< blocks; i++)
   {
      TEA_encrypt(&data32[i*2], key);
   }
}

/* inheriently reduced to 128bit encryption. ideal for internationalization. */
static void TEA_decryptBlock(uint8_t *data, uint32_t * len, uint32_t * key)
{
   uint32_t blocks, i;
   uint32_t * data32;

   // treat the data as 32 bit unsigned integers
   data32 = (uint32_t *) data;

   // Find the number of 8 byte blocks
   blocks = (*len)/8;

   for(i = 0; i< blocks; i++)
   {
      TEA_decrypt(&data32[i*2], key);
   }

   // Return the length of the original data
   *len = data32[(blocks*2) - 1];
}

int ashencode(char *data, size_t *data_len_p, shkey_t *key)
{
  uint32_t l = (uint32_t)(*data_len_p);

  /* sanity checks */
  if (l < 4)
    return (0); /* all done */

  if (IS_CRYPT_MAGIC(data)) {
    /* this is already encrypted. */
    return (0);
  }

  /* add encryption identifier */
  memmove(data + CRYPT_MAGIC_SIZE, data, l);
  SET_CRYPT_MAGIC(data);

  /* encrypt segment */
  TEA_encryptBlock(data + CRYPT_MAGIC_SIZE, &l, (uint32_t *)key->code);

  /* add encryption identifier. */
  *data_len_p = (size_t)(l + CRYPT_MAGIC_SIZE);

  return (0);
}

_TEST(ashencode)
{
  shkey_t *key = shkey_uniq();
  shbuf_t *buff = shbuf_init();
  char str[1024];

  _TRUEPTR(key);
  _TRUEPTR(buff);

  memset(str, 0, sizeof(str));
  memset(str, 'a', sizeof(str) - 1);
  shbuf_catstr(buff, str);

  _TRUE(!ashencode(buff->data, &buff->data_of, key));
  _TRUE(!ashdecode(buff->data, &buff->data_of,  key));

  memset(str, 0, sizeof(str));
  memset(str, 'a', sizeof(str) - 64);
  _TRUE(0 != strcmp(buff->data, str));

  _TRUE(!ashencode(buff->data, &buff->data_of, key));
  /* re-encrypt */
  ashencode(buff->data, &buff->data_of, key);
  _TRUE(!ashdecode(buff->data, &buff->data_of,  key));

  memset(str, 0, sizeof(str));
  memset(str, 'a', sizeof(str) - 64);
  _TRUE(0 != strcmp(buff->data, str));

  shbuf_free(&buff);
  shkey_free(&key);

}

int shencode(char *data, size_t data_len, uint8_t **data_p, uint32_t *data_len_p, shkey_t *key)
{
  uint32_t l = (uint32_t)data_len;
  shbuf_t *buf;

  /* sanity checks */
  if (data_len < 4) {
    buf = shbuf_init();
    shbuf_cat(buf, data, data_len);
    *data_p = (uint8_t *)buf->data;
    *data_len_p = data_len;
    free(buf);
    return (0);
  }

  if (IS_CRYPT_MAGIC(data)) {
    /* this is already encrypted. */
    return (0);
  }

  buf = shbuf_init();

  /* add encryption identifier */
  shbuf_cat(buf, &_crypt_magic, CRYPT_MAGIC_SIZE);

  /* encrypt segment */
  shbuf_cat(buf, data, l);
  shbuf_grow(buf, CRYPT_MAGIC_SIZE + l + SHMEM_PAD_SIZE);

  l = (uint32_t)buf->data_of - CRYPT_MAGIC_SIZE;
  TEA_encryptBlock(buf->data + CRYPT_MAGIC_SIZE, &l, (uint32_t *)key->code);
  buf->data_of = (size_t)(l + CRYPT_MAGIC_SIZE);

  /* add encryption identifier. */
  memcpy(buf->data + l, &_crypt_magic, sizeof(uint32_t));
  buf->data_of += sizeof(uint32_t);

  /* return encrypted segment. */
  *data_len_p = buf->data_of;
  *data_p = buf->data;
  free(buf);

  return (0);
}


shkey_t *shencode_str(char *data)
{
  shkey_t *key = shkey_str(data);
  size_t len = strlen(data);
  uint32_t klen;
  char *key_p = (char *)key;
  int err;

  klen = (((len / 8) + 1) * 8) + 4;
  memcpy(key_p + sizeof(shkey_t) - sizeof(len), &klen, sizeof(len));
  err = ashencode(data, &len, key); 
  if (err) {
    shkey_free(&key);
    return (NULL);
  }

 // key->data_len = len;
  return (key);
}

int ashdecode(uint8_t *data, size_t *data_len_p, shkey_t *key)
{
  uint32_t data_len = (uint32_t)*data_len_p;

  if (data_len < 4)
    return (0);

  if (!IS_CRYPT_MAGIC(data))
    return (0); /* not encrypted. */

  data_len -= CRYPT_MAGIC_SIZE;
  memmove(data, data + CRYPT_MAGIC_SIZE, data_len);

  TEA_decryptBlock(data, &data_len, (uint32_t *)key->code);
  *data_len_p = (size_t)data_len;

//  data[data_len] = '\0';

  return (0);
}

_TEST(ashdecode)
{
  shkey_t *key;
  shbuf_t *buf;
  char str[1024];
  size_t len;

  key = shkey_uniq();
  _TRUEPTR(key);

  buf = shbuf_init();
  _TRUEPTR(buf);

  memset(str, 0, sizeof(str));
  memset(str, 'a', sizeof(str) - 1);
  shbuf_catstr(buf, str);

  _TRUE(!ashencode(buf->data, &buf->data_of, key));
  _TRUE(!ashdecode(buf->data, &buf->data_of, key));

  memset(str, 0, sizeof(str));
  memset(str, 'a', sizeof(str) - 1);
  _TRUE(0 == strcmp(buf->data, str));

  _TRUE(!ashencode(buf->data, &buf->data_of, key));
  _TRUE(!ashdecode(buf->data, &buf->data_of, key));
//  ashdecode(buf->data, &buf->data_of, key);

  memset(str, 0, sizeof(str));
  memset(str, 'a', sizeof(str) - 1);
  _TRUE(0 == strcmp(buf->data, str));

  shbuf_free(&buf);
  shkey_free(&key);
}

int shdecode(uint8_t *data, uint32_t data_len, char **data_p, size_t *data_len_p, shkey_t *key)
{
  uint32_t l = data_len;
  shbuf_t *buf;

  if (l < 4) {
    buf = shbuf_init();
    shbuf_cat(buf, data, l);
    *data_p = buf->data;
    *data_len_p = buf->data_of;
    free(buf);
    return (0);
  }

  if (!IS_CRYPT_MAGIC(data))
    return (0); /* not encrypted */

  buf = shbuf_init();
  if (!buf)
    return (-1);
  shbuf_cat(buf, data + CRYPT_MAGIC_SIZE, l - CRYPT_MAGIC_SIZE);

  l = (uint32_t)buf->data_of;
  TEA_decryptBlock(buf->data, &l, (uint32_t *)key->code);
  buf->data_of = (size_t)l;

  *data_p = buf->data;
  *data_len_p = buf->data_of;
  free(buf);

  return (0);
}


int shdecode_str(char *data, shkey_t *key)
{
  size_t len;
  char *key_p = (char *)key;
  int err;

  memcpy(&len, key_p + sizeof(shkey_t) - sizeof(len), sizeof(len));
  err = ashdecode(data, &len, key); 
  if (err)
    return (err);

  return (0);
}

_TEST(shdecode_str)
{
  shkey_t *key;
  shbuf_t *buff;
  char str[1024];
  size_t len;

  key = shkey_uniq();
  buff = shbuf_init();
  _TRUEPTR(key);
  _TRUEPTR(buff);

  memset(str, 0, sizeof(str));
  memset(str, 'a', sizeof(str) - 1);
  shbuf_catstr(buff, str);

  key = shencode_str(buff->data);
  _TRUEPTR(key);
  if (key) {
    _TRUE(!shdecode_str(buff->data, key));
    memset(str, 0, sizeof(str));
    memset(str, 'a', sizeof(str) - 1);
    _TRUE(0 == strcmp(buff->data, str));
  }

  shbuf_free(&buff);
  shkey_free(&key);
}


