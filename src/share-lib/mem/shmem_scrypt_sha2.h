

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

#ifndef __SHARELIB__MEM__SHA2_H__
#define __SHARELIB__MEM__SHA2_H__

/**
 * Scrypt hash generator
 * @ingroup libshare_memscrypt
 */

#include <stdint.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>


#define SHA256_DIGEST_SIZE ( 256 / 8)
#define SHA256_BLOCK_SIZE  ( 512 / 8)

#define SHFR(x, n)    (x >> n)
#define ROTR(x, n)   ((x >> n) | (x << ((sizeof(x) << 3) - n)))
#define CH(x, y, z)  ((x & y) ^ (~x & z))
#define MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))

#define SHA256_F1(x) (ROTR(x,  2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define SHA256_F2(x) (ROTR(x,  6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SHA256_F3(x) (ROTR(x,  7) ^ ROTR(x, 18) ^ SHFR(x,  3))
#define SHA256_F4(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ SHFR(x, 10))

/* Elementary functions used by SHA256 */
#define Ch(x, y, z)	((x & (y ^ z)) ^ z)
#define Maj(x, y, z)	((x & (y | z)) | (y & z))
#define SHR(x, n)	(x >> n)
//#define ROTR(x, n)	((x >> n) | (x << (32 - n)))
#define S0(x)		(ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define S1(x)		(ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define s0(x)		(ROTR(x, 7) ^ ROTR(x, 18) ^ SHR(x, 3))
#define s1(x)		(ROTR(x, 17) ^ ROTR(x, 19) ^ SHR(x, 10))

/* SHA256 round function */
#define RND(a, b, c, d, e, f, g, h, k)			\
	t0 = h + S1(e) + Ch(e, f, g) + k;		\
	t1 = S0(a) + Maj(a, b, c);			\
	d += t0;					\
	h  = t0 + t1;

/* Adjusted round function for rotating state */
#define RNDr(S, W, i, k)			\
	RND(S[(64 - i) % 8], S[(65 - i) % 8],	\
	    S[(66 - i) % 8], S[(67 - i) % 8],	\
	    S[(68 - i) % 8], S[(69 - i) % 8],	\
	    S[(70 - i) % 8], S[(71 - i) % 8],	\
	    W[i] + k)

#ifndef bswap_16
#define bswap_16(value)  \
        ((((value) & 0xff) << 8) | ((value) >> 8))

#define bswap_32(value) \
        (((uint32_t)bswap_16((uint16_t)((value) & 0xffff)) << 16) | \
        (uint32_t)bswap_16((uint16_t)((value) >> 16)))

#define bswap_64(value) \
        (((uint64_t)bswap_32((uint32_t)((value) & 0xffffffff)) \
            << 32) | \
        (uint64_t)bswap_32((uint32_t)((value) >> 32)))
#endif

#ifdef WORDS_BIGENDIAN
#  define swap32tobe(out, in, sz)  ((out == in) ? (void)0 : memmove(out, in, sz))
#  define LOCAL_swap32be(type, var, sz)  ;
#  define swap32tole(out, in, sz)  swap32yes(out, in, sz)
#  define LOCAL_swap32le(type, var, sz)  LOCAL_swap32(type, var, sz)
#else
#  define swap32tobe(out, in, sz)  swap32yes(out, in, sz)
#  define LOCAL_swap32be(type, var, sz)  LOCAL_swap32(type, var, sz)
#  define swap32tole(out, in, sz)  ((out == in) ? (void)0 : memmove(out, in, sz))
#  define LOCAL_swap32le(type, var, sz)  ;
#endif


static inline uint32_t swab32(uint32_t v)
{
        return bswap_32(v);
}
static inline void swap32yes(void*out, const void*in, size_t sz) {
        size_t swapcounter = 0;
        for (swapcounter = 0; swapcounter < sz; ++swapcounter)
                (((uint32_t*)out)[swapcounter]) = swab32(((uint32_t*)in)[swapcounter]);
}
static inline void swab256(void *dest_p, const void *src_p)
{
        uint32_t *dest = dest_p;
        const uint32_t *src = src_p;

        dest[0] = swab32(src[7]);
        dest[1] = swab32(src[6]);
        dest[2] = swab32(src[5]);
        dest[3] = swab32(src[4]);
        dest[4] = swab32(src[3]);
        dest[5] = swab32(src[2]);
        dest[6] = swab32(src[1]);
        dest[7] = swab32(src[0]);
}

static inline
int timer_elapsed(struct timeval *begin_t, struct timeval *end_t)
{
struct timeval now;
if (!end_t) {
gettimeofday(&now, NULL);
return (now.tv_sec - begin_t->tv_sec);
}
        return (end_t->tv_sec - begin_t->tv_sec);
}




typedef struct {
    unsigned int tot_len;
    unsigned int len;
    unsigned char block[2 * SHA256_BLOCK_SIZE];
    uint32_t h[8];
} sha256_ctx;

extern uint32_t sha256_k[64];

void sha256_init(sha256_ctx * ctx);
void sha256_update(sha256_ctx *ctx, const unsigned char *message,
                   unsigned int len);
void sha256_final(sha256_ctx *ctx, unsigned char *digest);
void sha256(const unsigned char *message, unsigned int len,
            unsigned char *digest);

#endif /*ndef __SHARELIB__MEM__SHA2_H__ */
