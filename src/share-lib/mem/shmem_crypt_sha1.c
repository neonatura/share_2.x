
/*
 * @copyright
 *
 *  Copyright 2016 Neo Natura 
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

#define __MEM__SHMEM_CRYPT_C__
#include <stdio.h>
#include <string.h>
#include <endian.h>
#include "share.h"
#include "shmem_crypt_sha1.h"


/* This code is public-domain - it is based on libcrypt
 * placed in the public domain by Wei Dai and other contributors.
 */
// gcc -Wall -DSHA1TEST -o test_sha1 shmem_crypt_sha1.c && ./test_sha1

#if 0
#ifdef __BIG_ENDIAN__
# define SHA_BIG_ENDIAN
#elif defined __LITTLE_ENDIAN__
/* override */
#elif defined __BYTE_ORDER
# if __BYTE_ORDER__ ==  __ORDER_BIG_ENDIAN__
# define SHA_BIG_ENDIAN
# endif
#else // ! defined __LITTLE_ENDIAN__
# include <endian.h> // machine/endian.h
# if __BYTE_ORDER__ ==  __ORDER_BIG_ENDIAN__
#  define SHA_BIG_ENDIAN
# endif
#endif
#endif

#ifdef linux
#if __BYTE_ORDER == __BIG_ENDIAN
#define SHA_BIG_ENDIAN
#endif
#endif


/* header */


/* public API - prototypes - TODO: doxygen*/

/**
 */
void shsha1_initHmac(shsha1_t *s, const uint8_t* key, int keyLength);
/**
 */
uint8_t* shsha1_resultHmac(shsha1_t *s);


/* code */
#define SHA1_K0  0x5a827999
#define SHA1_K20 0x6ed9eba1
#define SHA1_K40 0x8f1bbcdc
#define SHA1_K60 0xca62c1d6

void shsha1_init(shsha1_t *s) {
  s->state[0] = 0x67452301;
  s->state[1] = 0xefcdab89;
  s->state[2] = 0x98badcfe;
  s->state[3] = 0x10325476;
  s->state[4] = 0xc3d2e1f0;
  s->byteCount = 0;
  s->bufferOffset = 0;
}

uint32_t shsha1_rol32(uint32_t number, uint8_t bits) {
  return ((number << bits) | (number >> (32-bits)));
}

void shsha1_hashBlock(shsha1_t *s) {
  uint8_t i;
  uint32_t a,b,c,d,e,t;

  a=s->state[0];
  b=s->state[1];
  c=s->state[2];
  d=s->state[3];
  e=s->state[4];
  for (i=0; i<80; i++) {
    if (i>=16) {
      t = s->buffer[(i+13)&15] ^ s->buffer[(i+8)&15] ^ s->buffer[(i+2)&15] ^ s->buffer[i&15];
      s->buffer[i&15] = shsha1_rol32(t,1);
    }
    if (i<20) {
      t = (d ^ (b & (c ^ d))) + SHA1_K0;
    } else if (i<40) {
      t = (b ^ c ^ d) + SHA1_K20;
    } else if (i<60) {
      t = ((b & c) | (d & (b | c))) + SHA1_K40;
    } else {
      t = (b ^ c ^ d) + SHA1_K60;
    }
    t+=shsha1_rol32(a,5) + e + s->buffer[i&15];
    e=d;
    d=c;
    c=shsha1_rol32(b,30);
    b=a;
    a=t;
  }
  s->state[0] += a;
  s->state[1] += b;
  s->state[2] += c;
  s->state[3] += d;
  s->state[4] += e;
}

void shsha1_addUncounted(shsha1_t *s, uint8_t data) {
  uint8_t * const b = (uint8_t*) s->buffer;
#ifdef SHA_BIG_ENDIAN
  b[s->bufferOffset] = data;
#else
  b[s->bufferOffset ^ 3] = data;
#endif
  s->bufferOffset++;
  if (s->bufferOffset == SHA1_BLOCK_LENGTH) {
    shsha1_hashBlock(s);
    s->bufferOffset = 0;
  }
}

void shsha1_writebyte(shsha1_t *s, uint8_t data) {
  ++s->byteCount;
  shsha1_addUncounted(s, data);
}

void shsha1_write(shsha1_t *s, const char *data, size_t len) {
  for (;len--;) shsha1_writebyte(s, (uint8_t) *data++);
}

void shsha1_pad(shsha1_t *s) {
  // Implement SHA-1 padding (fips180-2 Â§5.1.1)

  // Pad with 0x80 followed by 0x00 until the end of the block
  shsha1_addUncounted(s, 0x80);
  while (s->bufferOffset != 56) shsha1_addUncounted(s, 0x00);

  // Append length in the last 8 bytes
  shsha1_addUncounted(s, 0); // We're only using 32 bit lengths
  shsha1_addUncounted(s, 0); // But SHA-1 supports 64 bit lengths
  shsha1_addUncounted(s, 0); // So zero pad the top bits
  shsha1_addUncounted(s, s->byteCount >> 29); // Shifting to multiply by 8
  shsha1_addUncounted(s, s->byteCount >> 21); // as SHA-1 supports bitstreams as well as
  shsha1_addUncounted(s, s->byteCount >> 13); // byte.
  shsha1_addUncounted(s, s->byteCount >> 5);
  shsha1_addUncounted(s, s->byteCount << 3);
}

uint8_t* shsha1_result(shsha1_t *s) {
  // Pad to complete the last block
  shsha1_pad(s);

#ifndef SHA_BIG_ENDIAN
  // Swap byte order back
  int i;
  for (i=0; i<5; i++) {
    s->state[i]=
        (((s->state[i])<<24)& 0xff000000)
      | (((s->state[i])<<8) & 0x00ff0000)
      | (((s->state[i])>>8) & 0x0000ff00)
      | (((s->state[i])>>24)& 0x000000ff);
  }
#endif

  // Return pointer to hash (20 characters)
  return (uint8_t*) s->state;
}

#define HMAC_IPAD 0x36
#define HMAC_OPAD 0x5c

void shsha1_initHmac(shsha1_t *s, const uint8_t* key, int keyLength) 
{
  uint8_t i;
  memset(s->keyBuffer, 0, SHA1_BLOCK_LENGTH);
  if (keyLength > SHA1_BLOCK_LENGTH) {
    // Hash long keys
    shsha1_init(s);
    for (;keyLength--;) shsha1_writebyte(s, *key++);
    memcpy(s->keyBuffer, shsha1_result(s), SHA1_HASH_LENGTH);
  } else {
    // Block length keys are used as is
    memcpy(s->keyBuffer, key, keyLength);
  }
  // Start inner hash
  shsha1_init(s);
  for (i=0; i<SHA1_BLOCK_LENGTH; i++) {
    shsha1_writebyte(s, s->keyBuffer[i] ^ HMAC_IPAD);
  }
}

uint8_t* shsha1_resultHmac(shsha1_t *s) 
{
  uint8_t i;
  // Complete inner hash
  memcpy(s->innerHash,shsha1_result(s),SHA1_HASH_LENGTH);
  // Calculate outer hash
  shsha1_init(s);
  for (i=0; i<SHA1_BLOCK_LENGTH; i++) shsha1_writebyte(s, s->keyBuffer[i] ^ HMAC_OPAD);
  for (i=0; i<SHA1_HASH_LENGTH; i++) shsha1_writebyte(s, s->innerHash[i]);
  return shsha1_result(s);
}


/* self-test */
static uint8_t hmacKey1[]={
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
  0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
  0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f
};
static uint8_t hmacKey2[]={
  0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
  0x40,0x41,0x42,0x43
};
static uint8_t hmacKey3[]={
  0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
  0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
  0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
  0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
  0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
  0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
  0xb0,0xb1,0xb2,0xb3
};
static uint8_t hmacKey4[]={
  0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
  0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
  0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
  0xa0
};

static char *printHash(uint8_t* hash) 
{
  static char ret_str[256];
  int i;

  memset(ret_str, 0, sizeof(ret_str));
  for (i=0; i<20; i++) {
    sprintf(ret_str+strlen(ret_str), "%02x", hash[i]);
  }

  return (ret_str);
}

_TEST(shmem_crypt_sha1)
{
  uint32_t a;
  shsha1_t s;

  memset(&s, 0, sizeof(s));

  /* Test: FIPS 180-2 C.1 and RFC3174 7.3 TEST1 */
  shsha1_init(&s);
  shsha1_write(&s, "abc", 3);
  _TRUE(0 == strcmp(printHash(shsha1_result(&s)), 
        "a9993e364706816aba3e25717850c26c9cd0d89d"));

  /* Test: FIPS 180-2 C.2 and RFC3174 7.3 TEST2 */
  shsha1_init(&s);
  shsha1_write(&s, "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56);
  _TRUE(0 == strcmp(printHash(shsha1_result(&s)),
        "84983e441c3bd26ebaae4aa1f95129e5e54670f1"));

  /* Test: RFC3174 7.3 TEST4 */
  shsha1_init(&s);
  for (a=0; a<80; a++) shsha1_write(&s, "01234567", 8);
  _TRUE(0 == strcmp(printHash(shsha1_result(&s)),
        "dea356a2cddd90c7a7ecedc5ebb563934f460452"));

  /* Test: FIPS 198a A.1 */
  shsha1_initHmac(&s, hmacKey1, 64);
  shsha1_write(&s, "Sample #1",9);
  _TRUE(0 == strcmp(printHash(shsha1_resultHmac(&s)),
        "4f4ca3d5d68ba7cc0a1208c9c61e9c5da0403c0a"));

  /* Test: FIPS 198a A.2 */
  shsha1_initHmac(&s, hmacKey2, 20);
  shsha1_write(&s, "Sample #2", 9);
  _TRUE(0 == strcmp(printHash(shsha1_resultHmac(&s)),
        "0922d3405faa3d194f82a45830737d5cc6c75d24"));

  /* Test: FIPS 198a A.3 */
  shsha1_initHmac(&s, hmacKey3,100);
  shsha1_write(&s, "Sample #3", 9);
  _TRUE(0 == strcmp(printHash(shsha1_resultHmac(&s)),
        "bcf41eab8bb2d802f3d05caf7cb092ecf8d1a3aa"));

  /* Test: FIPS 198a A.4 */
  shsha1_initHmac(&s, hmacKey4,49);
  shsha1_write(&s, "Sample #4", 9);
  _TRUE(0 == strcmp(printHash(shsha1_resultHmac(&s)),
        "9ea886efe268dbecce420c7524df32e0751a2a26"));

  /* Test: FIPS 180-2 C.3 and RFC3174 7.3 TEST3 */
  shsha1_init(&s);
  for (a=0; a<1000000; a++) shsha1_writebyte(&s, 'a');
  _TRUE(0 == strcmp(printHash(shsha1_result(&s)),
        "34aa973cd4c4daa4f61eeb2bdbad27316534016f"));

}


char *shsha1_hash_print(uint8_t* hash) 
{
  static char ret_str[256];
  int i;

  memset(ret_str, 0, sizeof(ret_str));
  for (i=0; i<20; i++) {
    sprintf(ret_str+strlen(ret_str), "%02x", hash[i]);
  }

  return (ret_str);
}

char *shsha1_hash(unsigned char *data, size_t data_len)
{
  shsha1_t s;
  uint8_t *result;
  int err;

  if (!data || !data_len)
    return (NULL);

  shsha1_init(&s);
  shsha1_write(&s, data, data_len);

  result = shsha1_result(&s);
  if (!result)
    return (NULL);

  return (shsha1_hash_print(result));
}

void shsha1(unsigned char *data, size_t data_len, char *result)
{
  shsha1_t s;
  uint8_t *hash;

  memset(result, '\000', 20);

  shsha1_init(&s);
  if (data && data_len)
    shsha1_write(&s, data, data_len);

  hash = shsha1_result(&s);
  if (!hash)
    return;

  memset(result, hash, 20);
}


