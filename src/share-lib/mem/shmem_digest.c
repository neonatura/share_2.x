

/*
 * @copyright
 *
 *  Copyright 2014 Neo Natura
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

#define __MEM__SHMEM_DIGEST_C__
#include "share.h"


#ifdef WORDS_BIGENDIAN
#  define swap32tole(out, in, sz)  swap32yes(out, in, sz)
#else
#  define swap32tole(out, in, sz)  ((out == in) ? (void)0 : memmove(out, in, sz))
#endif

#define SHFR(x, n)    (x >> n)
#define ROTR(x, n)   ((x >> n) | (x << ((sizeof(x) << 3) - n)))
#define CH(x, y, z)  ((x & y) ^ (~x & z))
#define _SH_MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))

#define SHA256_F1(x) (ROTR(x,  2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define SHA256_F2(x) (ROTR(x,  6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SHA256_F3(x) (ROTR(x,  7) ^ ROTR(x, 18) ^ SHFR(x,  3))
#define SHA256_F4(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ SHFR(x, 10))



#define UNPACK32(x, str)                      \
{                                             \
    *((str) + 3) = (uint8_t) ((x)      );       \
    *((str) + 2) = (uint8_t) ((x) >>  8);       \
    *((str) + 1) = (uint8_t) ((x) >> 16);       \
    *((str) + 0) = (uint8_t) ((x) >> 24);       \
}

#define PACK32(str, x)                        \
{                                             \
    *(x) =   ((uint32_t) *((str) + 3)      )    \
           | ((uint32_t) *((str) + 2) <<  8)    \
           | ((uint32_t) *((str) + 1) << 16)    \
           | ((uint32_t) *((str) + 0) << 24);   \
}

#define SHA256_SCR(i)                         \
{                                             \
    w[i] =  SHA256_F4(w[i -  2]) + w[i -  7]  \
          + SHA256_F3(w[i - 15]) + w[i - 16]; \
}

uint32_t sha256_h0[8] =
            {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
             0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

uint32_t sha256_k[64] =
            {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
             0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
             0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
             0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
             0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
             0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
             0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
             0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
             0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
             0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
             0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
             0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
             0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
             0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
             0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
             0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

/* SHA-256 functions */

static void _sh_sha256_transf(sh_sha256_t *ctx, const unsigned char *message, unsigned int block_nb)
{
    uint32_t w[64];
    uint32_t wv[8];
    uint32_t t1, t2;
    const unsigned char *sub_block;
    int i;
    int j;

    for (i = 0; i < 64; i++)
      w[i] = 0;
    for (i = 0; i < 8; i++)
      wv[i] = 0;
    t1 = t2 = 0;

    for (i = 0; i < (int) block_nb; i++) {
        sub_block = message + (i << 6);

        for (j = 0; j < 16; j++) {
            PACK32(&sub_block[j << 2], &w[j]);
        }

        for (j = 16; j < 64; j++) {
            SHA256_SCR(j);
        }

        for (j = 0; j < 8; j++) {
            wv[j] = ctx->h[j];
        }

        for (j = 0; j < 64; j++) {
            t1 = wv[7] + SHA256_F2(wv[4]) + CH(wv[4], wv[5], wv[6])
                + sha256_k[j] + w[j];
            t2 = SHA256_F1(wv[0]) + _SH_MAJ(wv[0], wv[1], wv[2]);
            wv[7] = wv[6];
            wv[6] = wv[5];
            wv[5] = wv[4];
            wv[4] = wv[3] + t1;
            wv[3] = wv[2];
            wv[2] = wv[1];
            wv[1] = wv[0];
            wv[0] = t1 + t2;
        }

        for (j = 0; j < 8; j++) {
            ctx->h[j] += wv[j];
        }
    }
}


void sh_sha256_init(sh_sha256_t *ctx)
{
    int i;
    for (i = 0; i < 8; i++) {
        ctx->h[i] = sha256_h0[i];
    }

    ctx->len = 0;
    ctx->tot_len = 0;
}

void sh_sha256_update(sh_sha256_t *ctx, const unsigned char *message, unsigned int len)
{
    unsigned int block_nb;
    unsigned int new_len, rem_len, tmp_len;
    const unsigned char *shifted_message;

    tmp_len = _SH_SHA256_BLOCK_SIZE - ctx->len;
    rem_len = len < tmp_len ? len : tmp_len;

    memcpy(&ctx->block[ctx->len], message, rem_len);

    if (ctx->len + len < _SH_SHA256_BLOCK_SIZE) {
        ctx->len += len;
        return;
    }

    new_len = len - rem_len;
    block_nb = new_len / _SH_SHA256_BLOCK_SIZE;

    shifted_message = message + rem_len;

    _sh_sha256_transf(ctx, ctx->block, 1);
    _sh_sha256_transf(ctx, shifted_message, block_nb);

    rem_len = new_len % _SH_SHA256_BLOCK_SIZE;

    memcpy(ctx->block, &shifted_message[block_nb << 6],
           rem_len);

    ctx->len = rem_len;
    ctx->tot_len += (block_nb + 1) << 6;
}

void sh_sha256_final(sh_sha256_t *ctx, unsigned char *digest)
{
    unsigned int block_nb;
    unsigned int pm_len;
    unsigned int len_b;

    int i;

    block_nb = (1 + ((_SH_SHA256_BLOCK_SIZE - 9)
                     < (ctx->len % _SH_SHA256_BLOCK_SIZE)));

    len_b = (ctx->tot_len + ctx->len) << 3;
    pm_len = block_nb << 6;

    memset(ctx->block + ctx->len, 0, pm_len - ctx->len);
    ctx->block[ctx->len] = 0x80;
    UNPACK32(len_b, ctx->block + pm_len - 4);

    _sh_sha256_transf(ctx, ctx->block, block_nb);

    for (i = 0 ; i < 8; i++) {
        UNPACK32(ctx->h[i], &digest[i << 2]);
    }
}

/**
 * Generate a sha256 hash checksum from a binary segment.
 * @param message The binary segment to generate a sha2 checksum for.
 * @param len The length of the binary segment to generate a sha2 checksum for.
 * @param digest The binary hash output (64 bytes).
 */
void sh_sha256(const unsigned char *message, unsigned int len, unsigned char *digest)
{
  sh_sha256_t ctx;

  memset(&ctx, 0, sizeof(ctx));
  sh_sha256_init(&ctx);
  sh_sha256_update(&ctx, message, len);
  sh_sha256_final(&ctx, digest);
}

char *shdigest(void *data, int32_t len)
{
  static char ret_buf[256];
  char buf[64];
  int32_t *int_ar;
  int i;

  memset(buf, 0, sizeof(buf));
  sh_sha256((unsigned char *)data, len, buf);
  int_ar = (uint32_t *)buf;

  memset(ret_buf, 0, sizeof(ret_buf));
  for (i = 0; i < 8; i++) {
    sprintf(ret_buf + strlen(ret_buf), "%-8.8x", int_ar[i]);
  }

  return (ret_buf);
}

_TEST(shdigest)
{
  char *data;
  char *ptr;

  data = (char *)calloc(10240, sizeof(char));
  memset(data, 'a', 10240);
  ptr = shdigest(data, 10240);
  free(data);

  _TRUE(strlen(ptr) == 64);

 // ptr = shdigest("test", strlen("test"));
 // ptr = shdigest("test", strlen("test") + 1);

}

void sh_calc_midstate(struct scrypt_work *work)
{
  sh_sha256_t ctx;
  union {
    unsigned char c[64];
    uint32_t i[16];
  } data;

  swap32yes(&data.i[0], work->data, 16);
  sh_sha256_init(&ctx);
  sh_sha256_update(&ctx, data.c, 64);
  memcpy(work->midstate, ctx.h, sizeof(work->midstate));
  swap32tole(work->midstate, work->midstate, 8);
}
















#if 0
#define ROTATE(x,n) (((x) >> (n)) | ((x) << (64 - (n))))
#define S1(x) (ROTATE(x,28) ^ ROTATE(x,34) ^ ROTATE(x,39))
#define S2(x) (ROTATE(x,14) ^ ROTATE(x,18) ^ ROTATE(x,41))
#define S3(x) (ROTATE(x, 1) ^ ROTATE(x, 8) ^ (x >> 7))
#define S4(x) (ROTATE(x,19) ^ ROTATE(x,61) ^ (x >> 6))
#define P(t, a, b, c, d, e, f, g, h, x, i, k) \
  t = (h) + S2((e)) + CH((e),(f),(g)) + (x)[(i)] + (k); \
d += (t); \
h = (t) + S1((a)) + _SH_MAJ((a),(b),(c));
#define Pa(t, a, b, c, d, e, f, g, h, x, i, k) \
  (x)[(i)] = htobe64(((uint64_t *)(ctx->block))[(i)]); \
P((t),(a),(b),(c),(d),(e),(f),(g),(h),(x),(i),(k))
#define Pb(t, a, b, c, d, e, f, g, h, x, i,k) \
  (x)[(i)] = S4((x)[(i) - 2]) + (x)[(i) - 7] + S3((x)[(i) - 15]) + (x)[(i) - 16]; \
P((t),(a),(b),(c),(d),(e),(f),(g),(h),(x),(i),(k))
#define CTX_AVAIL(x) ((sizeof((x)->block)) - ((x)->len % (sizeof((x)->block))))
#define CTX_POS(x) ((x)->len % (sizeof((x)->block)))



/**
 * Initialize an sha256 context.
 *
 * @param ctx sha256 hash context
 */
void sh_sha512_init(sh_sha512_t *ctx) 
{
  ctx->hash[0] = 0x6a09e667f3bcc908ULL;
  ctx->hash[1] = 0xbb67ae8584caa73bULL;
  ctx->hash[2] = 0x3c6ef372fe94f82bULL;
  ctx->hash[3] = 0xa54ff53a5f1d36f1ULL;
  ctx->hash[4] = 0x510e527fade682d1ULL;
  ctx->hash[5] = 0x9b05688c2b3e6c1fULL;
  ctx->hash[6] = 0x1f83d9abfb41bd6bULL;
  ctx->hash[7] = 0x5be0cd19137e2179ULL;
  ctx->len = 0;
}

/**
* Update a sha512 hash.
*
* @param ctx sha512 hash context
* @param data data
* @param dlen data length
*/
void sh_sha512_update(sh_sha512_t *ctx, void *data, size_t dlen) 
{
  uint16_t avail; /* space available in the context message block */
  uint16_t pos; /* position to write data into the message block */
  uint64_t x[80];
  while (dlen) {
    pos = CTX_POS(ctx);
    avail = CTX_AVAIL(ctx);
    if (avail > dlen)
      avail = dlen;
    /* Copy input data into the context's message block. */
    memcpy(ctx->block + pos, data, avail);
    data += avail;
    dlen -= avail;
    ctx->len += avail;
    if ((ctx->len % 128) == 0) {
      uint64_t a = ctx->hash[0];
      uint64_t b = ctx->hash[1];
      uint64_t c = ctx->hash[2];
      uint64_t d = ctx->hash[3];
      uint64_t e = ctx->hash[4];
      uint64_t f = ctx->hash[5];
      uint64_t g = ctx->hash[6];
      uint64_t h = ctx->hash[7];
      uint64_t t;
      Pa(t, a, b, c, d, e, f, g, h, x, 0, 0x428a2f98d728ae22);
      Pa(t, h, a, b, c, d, e, f, g, x, 1, 0x7137449123ef65cd);
      Pa(t, g, h, a, b, c, d, e, f, x, 2, 0xb5c0fbcfec4d3b2f);
      Pa(t, f, g, h, a, b, c, d, e, x, 3, 0xe9b5dba58189dbbc);
      Pa(t, e, f, g, h, a, b, c, d, x, 4, 0x3956c25bf348b538);
      Pa(t, d, e, f, g, h, a, b, c, x, 5, 0x59f111f1b605d019);
      Pa(t, c, d, e, f, g, h, a, b, x, 6, 0x923f82a4af194f9b);
      Pa(t, b, c, d, e, f, g, h, a, x, 7, 0xab1c5ed5da6d8118);
      Pa(t, a, b, c, d, e, f, g, h, x, 8, 0xd807aa98a3030242);
      Pa(t, h, a, b, c, d, e, f, g, x, 9, 0x12835b0145706fbe);
      Pa(t, g, h, a, b, c, d, e, f, x, 10, 0x243185be4ee4b28c);
      Pa(t, f, g, h, a, b, c, d, e, x, 11, 0x550c7dc3d5ffb4e2);
      Pa(t, e, f, g, h, a, b, c, d, x, 12, 0x72be5d74f27b896f);
      Pa(t, d, e, f, g, h, a, b, c, x, 13, 0x80deb1fe3b1696b1);
      Pa(t, c, d, e, f, g, h, a, b, x, 14, 0x9bdc06a725c71235);
      Pa(t, b, c, d, e, f, g, h, a, x, 15, 0xc19bf174cf692694);
      Pb(t, a, b, c, d, e, f, g, h, x, 16, 0xe49b69c19ef14ad2);
      Pb(t, h, a, b, c, d, e, f, g, x, 17, 0xefbe4786384f25e3);
      Pb(t, g, h, a, b, c, d, e, f, x, 18, 0x0fc19dc68b8cd5b5);
      Pb(t, f, g, h, a, b, c, d, e, x, 19, 0x240ca1cc77ac9c65);
      Pb(t, e, f, g, h, a, b, c, d, x, 20, 0x2de92c6f592b0275);
      Pb(t, d, e, f, g, h, a, b, c, x, 21, 0x4a7484aa6ea6e483);
      Pb(t, c, d, e, f, g, h, a, b, x, 22, 0x5cb0a9dcbd41fbd4);
      Pb(t, b, c, d, e, f, g, h, a, x, 23, 0x76f988da831153b5);
      Pb(t, a, b, c, d, e, f, g, h, x, 24, 0x983e5152ee66dfab);
      Pb(t, h, a, b, c, d, e, f, g, x, 25, 0xa831c66d2db43210);
      Pb(t, g, h, a, b, c, d, e, f, x, 26, 0xb00327c898fb213f);
      Pb(t, f, g, h, a, b, c, d, e, x, 27, 0xbf597fc7beef0ee4);
      Pb(t, e, f, g, h, a, b, c, d, x, 28, 0xc6e00bf33da88fc2);
      Pb(t, d, e, f, g, h, a, b, c, x, 29, 0xd5a79147930aa725);
      Pb(t, c, d, e, f, g, h, a, b, x, 30, 0x06ca6351e003826f);
      Pb(t, b, c, d, e, f, g, h, a, x, 31, 0x142929670a0e6e70);
      Pb(t, a, b, c, d, e, f, g, h, x, 32, 0x27b70a8546d22ffc);
      Pb(t, h, a, b, c, d, e, f, g, x, 33, 0x2e1b21385c26c926);
      Pb(t, g, h, a, b, c, d, e, f, x, 34, 0x4d2c6dfc5ac42aed);
      Pb(t, f, g, h, a, b, c, d, e, x, 35, 0x53380d139d95b3df);
      Pb(t, e, f, g, h, a, b, c, d, x, 36, 0x650a73548baf63de);
      Pb(t, d, e, f, g, h, a, b, c, x, 37, 0x766a0abb3c77b2a8);
      Pb(t, c, d, e, f, g, h, a, b, x, 38, 0x81c2c92e47edaee6);
      Pb(t, b, c, d, e, f, g, h, a, x, 39, 0x92722c851482353b);
      Pb(t, a, b, c, d, e, f, g, h, x, 40, 0xa2bfe8a14cf10364);
      Pb(t, h, a, b, c, d, e, f, g, x, 41, 0xa81a664bbc423001);
      Pb(t, g, h, a, b, c, d, e, f, x, 42, 0xc24b8b70d0f89791);
      Pb(t, f, g, h, a, b, c, d, e, x, 43, 0xc76c51a30654be30);
      Pb(t, e, f, g, h, a, b, c, d, x, 44, 0xd192e819d6ef5218);
      Pb(t, d, e, f, g, h, a, b, c, x, 45, 0xd69906245565a910);
      Pb(t, c, d, e, f, g, h, a, b, x, 46, 0xf40e35855771202a);
      Pb(t, b, c, d, e, f, g, h, a, x, 47, 0x106aa07032bbd1b8);
      Pb(t, a, b, c, d, e, f, g, h, x, 48, 0x19a4c116b8d2d0c8);
      Pb(t, h, a, b, c, d, e, f, g, x, 49, 0x1e376c085141ab53);
      Pb(t, g, h, a, b, c, d, e, f, x, 50, 0x2748774cdf8eeb99);
      Pb(t, f, g, h, a, b, c, d, e, x, 51, 0x34b0bcb5e19b48a8);
      Pb(t, e, f, g, h, a, b, c, d, x, 52, 0x391c0cb3c5c95a63);
      Pb(t, d, e, f, g, h, a, b, c, x, 53, 0x4ed8aa4ae3418acb);
      Pb(t, c, d, e, f, g, h, a, b, x, 54, 0x5b9cca4f7763e373);
      Pb(t, b, c, d, e, f, g, h, a, x, 55, 0x682e6ff3d6b2b8a3);
      Pb(t, a, b, c, d, e, f, g, h, x, 56, 0x748f82ee5defb2fc);
      Pb(t, h, a, b, c, d, e, f, g, x, 57, 0x78a5636f43172f60);
      Pb(t, g, h, a, b, c, d, e, f, x, 58, 0x84c87814a1f0ab72);
      Pb(t, f, g, h, a, b, c, d, e, x, 59, 0x8cc702081a6439ec);
      Pb(t, e, f, g, h, a, b, c, d, x, 60, 0x90befffa23631e28);
      Pb(t, d, e, f, g, h, a, b, c, x, 61, 0xa4506cebde82bde9);
      Pb(t, c, d, e, f, g, h, a, b, x, 62, 0xbef9a3f7b2c67915);
      Pb(t, b, c, d, e, f, g, h, a, x, 63, 0xc67178f2e372532b);
      Pb(t, a, b, c, d, e, f, g, h, x, 64, 0xca273eceea26619c);
      Pb(t, h, a, b, c, d, e, f, g, x, 65, 0xd186b8c721c0c207);
      Pb(t, g, h, a, b, c, d, e, f, x, 66, 0xeada7dd6cde0eb1e);
      Pb(t, f, g, h, a, b, c, d, e, x, 67, 0xf57d4f7fee6ed178);
      Pb(t, e, f, g, h, a, b, c, d, x, 68, 0x06f067aa72176fba);
      Pb(t, d, e, f, g, h, a, b, c, x, 69, 0x0a637dc5a2c898a6);
      Pb(t, c, d, e, f, g, h, a, b, x, 70, 0x113f9804bef90dae);
      Pb(t, b, c, d, e, f, g, h, a, x, 71, 0x1b710b35131c471b);
      Pb(t, a, b, c, d, e, f, g, h, x, 72, 0x28db77f523047d84);
      Pb(t, h, a, b, c, d, e, f, g, x, 73, 0x32caab7b40c72493);
      Pb(t, g, h, a, b, c, d, e, f, x, 74, 0x3c9ebe0a15c9bebc);
      Pb(t, f, g, h, a, b, c, d, e, x, 75, 0x431d67c49c100d4c);
      Pb(t, e, f, g, h, a, b, c, d, x, 76, 0x4cc5d4becb3e42b6);
      Pb(t, d, e, f, g, h, a, b, c, x, 77, 0x597f299cfc657e2a);
      Pb(t, c, d, e, f, g, h, a, b, x, 78, 0x5fcb6fab3ad6faec);
      Pb(t, b, c, d, e, f, g, h, a, x, 79, 0x6c44198c4a475817);
      ctx->hash[0] += a;
      ctx->hash[1] += b;
      ctx->hash[2] += c;
      ctx->hash[3] += d;
      ctx->hash[4] += e;
      ctx->hash[5] += f;
      ctx->hash[6] += g;
      ctx->hash[7] += h;
    }
  }
}

static uint8_t sha512_padding[136] = {
0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0
};

/**
 * Finalize a sha512 hash.
 *
 * @param ctx sha512 hash context
 */
void sh_sha512_final(sh_sha512_t *ctx) 
{
  uint16_t avail = CTX_AVAIL(ctx);
  uint16_t pad;
  uint64_t len;
  /* Shift the length to obtain the total number of bits in the message and
   * * pack it into the buffer so it can be used for padding. */
  len = htobe64(ctx->len << 3);
  pad = (avail > 16) ? (avail - 8) : (avail + 120);
  sh_sha512_update(ctx, sha512_padding, pad);
  sh_sha512_update(ctx, &len, sizeof(len));
  if ((ctx->len % sizeof(ctx->block)) != 0)
    return;
  ctx->hash[0] = htobe64(ctx->hash[0]);
  ctx->hash[1] = htobe64(ctx->hash[1]);
  ctx->hash[2] = htobe64(ctx->hash[2]);
  ctx->hash[3] = htobe64(ctx->hash[3]);
  ctx->hash[4] = htobe64(ctx->hash[4]);
  ctx->hash[5] = htobe64(ctx->hash[5]);
  ctx->hash[6] = htobe64(ctx->hash[6]);
  ctx->hash[7] = htobe64(ctx->hash[7]);
}
#endif









/*
 *  * 64-bit integer manipulation macros (big endian)
 *   */
#ifndef GET_UINT64_BE
#define GET_UINT64_BE(n,b,i)                            \
{                                                       \
    (n) = ( (uint64_t) (b)[(i)    ] << 56 )       \
        | ( (uint64_t) (b)[(i) + 1] << 48 )       \
        | ( (uint64_t) (b)[(i) + 2] << 40 )       \
        | ( (uint64_t) (b)[(i) + 3] << 32 )       \
        | ( (uint64_t) (b)[(i) + 4] << 24 )       \
        | ( (uint64_t) (b)[(i) + 5] << 16 )       \
        | ( (uint64_t) (b)[(i) + 6] <<  8 )       \
        | ( (uint64_t) (b)[(i) + 7]       );      \
}
#endif /* GET_UINT64_BE */

#ifndef PUT_UINT64_BE
#define PUT_UINT64_BE(n,b,i)                            \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 56 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 48 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >> 40 );       \
    (b)[(i) + 3] = (unsigned char) ( (n) >> 32 );       \
    (b)[(i) + 4] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 5] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 6] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 7] = (unsigned char) ( (n)       );       \
}
#endif /* PUT_UINT64_BE */

#if defined(_MSC_VER) || defined(__WATCOMC__)
  #define UL64(x) x##ui64
  typedef unsigned __int64 uint64_t;
#else
  #include <inttypes.h>
  #define UL64(x) x##ULL
#endif

/*
 *  * Round constants
 *   */
static const uint64_t K[80] =
{
    UL64(0x428A2F98D728AE22),  UL64(0x7137449123EF65CD),
    UL64(0xB5C0FBCFEC4D3B2F),  UL64(0xE9B5DBA58189DBBC),
    UL64(0x3956C25BF348B538),  UL64(0x59F111F1B605D019),
    UL64(0x923F82A4AF194F9B),  UL64(0xAB1C5ED5DA6D8118),
    UL64(0xD807AA98A3030242),  UL64(0x12835B0145706FBE),
    UL64(0x243185BE4EE4B28C),  UL64(0x550C7DC3D5FFB4E2),
    UL64(0x72BE5D74F27B896F),  UL64(0x80DEB1FE3B1696B1),
    UL64(0x9BDC06A725C71235),  UL64(0xC19BF174CF692694),
    UL64(0xE49B69C19EF14AD2),  UL64(0xEFBE4786384F25E3),
    UL64(0x0FC19DC68B8CD5B5),  UL64(0x240CA1CC77AC9C65),
    UL64(0x2DE92C6F592B0275),  UL64(0x4A7484AA6EA6E483),
    UL64(0x5CB0A9DCBD41FBD4),  UL64(0x76F988DA831153B5),
    UL64(0x983E5152EE66DFAB),  UL64(0xA831C66D2DB43210),
    UL64(0xB00327C898FB213F),  UL64(0xBF597FC7BEEF0EE4),
    UL64(0xC6E00BF33DA88FC2),  UL64(0xD5A79147930AA725),
    UL64(0x06CA6351E003826F),  UL64(0x142929670A0E6E70),
    UL64(0x27B70A8546D22FFC),  UL64(0x2E1B21385C26C926),
    UL64(0x4D2C6DFC5AC42AED),  UL64(0x53380D139D95B3DF),
    UL64(0x650A73548BAF63DE),  UL64(0x766A0ABB3C77B2A8),
    UL64(0x81C2C92E47EDAEE6),  UL64(0x92722C851482353B),
    UL64(0xA2BFE8A14CF10364),  UL64(0xA81A664BBC423001),
    UL64(0xC24B8B70D0F89791),  UL64(0xC76C51A30654BE30),
    UL64(0xD192E819D6EF5218),  UL64(0xD69906245565A910),
    UL64(0xF40E35855771202A),  UL64(0x106AA07032BBD1B8),
    UL64(0x19A4C116B8D2D0C8),  UL64(0x1E376C085141AB53),
    UL64(0x2748774CDF8EEB99),  UL64(0x34B0BCB5E19B48A8),
    UL64(0x391C0CB3C5C95A63),  UL64(0x4ED8AA4AE3418ACB),
    UL64(0x5B9CCA4F7763E373),  UL64(0x682E6FF3D6B2B8A3),
    UL64(0x748F82EE5DEFB2FC),  UL64(0x78A5636F43172F60),
    UL64(0x84C87814A1F0AB72),  UL64(0x8CC702081A6439EC),
    UL64(0x90BEFFFA23631E28),  UL64(0xA4506CEBDE82BDE9),
    UL64(0xBEF9A3F7B2C67915),  UL64(0xC67178F2E372532B),
    UL64(0xCA273ECEEA26619C),  UL64(0xD186B8C721C0C207),
    UL64(0xEADA7DD6CDE0EB1E),  UL64(0xF57D4F7FEE6ED178),
    UL64(0x06F067AA72176FBA),  UL64(0x0A637DC5A2C898A6),
    UL64(0x113F9804BEF90DAE),  UL64(0x1B710B35131C471B),
    UL64(0x28DB77F523047D84),  UL64(0x32CAAB7B40C72493),
    UL64(0x3C9EBE0A15C9BEBC),  UL64(0x431D67C49C100D4C),
    UL64(0x4CC5D4BECB3E42B6),  UL64(0x597F299CFC657E2A),
    UL64(0x5FCB6FAB3AD6FAEC),  UL64(0x6C44198C4A475817)
};


static void _sh_sha512_starts(sh_sha512_t *ctx, int is384 )
{
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    if( is384 == 0 )
    {
        /* SHA-512 */
        ctx->state[0] = UL64(0x6A09E667F3BCC908);
        ctx->state[1] = UL64(0xBB67AE8584CAA73B);
        ctx->state[2] = UL64(0x3C6EF372FE94F82B);
        ctx->state[3] = UL64(0xA54FF53A5F1D36F1);
        ctx->state[4] = UL64(0x510E527FADE682D1);
        ctx->state[5] = UL64(0x9B05688C2B3E6C1F);
        ctx->state[6] = UL64(0x1F83D9ABFB41BD6B);
        ctx->state[7] = UL64(0x5BE0CD19137E2179);
    }
    else
    {
        /* SHA-384 */
        ctx->state[0] = UL64(0xCBBB9D5DC1059ED8);
        ctx->state[1] = UL64(0x629A292A367CD507);
        ctx->state[2] = UL64(0x9159015A3070DD17);
        ctx->state[3] = UL64(0x152FECD8F70E5939);
        ctx->state[4] = UL64(0x67332667FFC00B31);
        ctx->state[5] = UL64(0x8EB44A8768581511);
        ctx->state[6] = UL64(0xDB0C2E0D64F98FA7);
        ctx->state[7] = UL64(0x47B5481DBEFA4FA4);
    }

    ctx->is384 = is384;
}

/*
 * SHA-512 context setup
 */
void sh_sha512_init(sh_sha512_t *ctx)
{
  memset(ctx, 0, sizeof(sh_sha512_t));
  _sh_sha512_starts(ctx, 0);
}

static void _sh_sha512_process(sh_sha512_t *ctx, const unsigned char data[128])
{
  int i;
  uint64_t temp1, temp2, W[80];
  uint64_t A, B, C, D, E, F, G, H;

#ifdef ROTR
#undef ROTR
#endif

#define  SHR(x,n) (x >> n)
#define ROTR(x,n) (SHR(x,n) | (x << (64 - n)))

#define S0(x) (ROTR(x, 1) ^ ROTR(x, 8) ^  SHR(x, 7))
#define S1(x) (ROTR(x,19) ^ ROTR(x,61) ^  SHR(x, 6))

#define S2(x) (ROTR(x,28) ^ ROTR(x,34) ^ ROTR(x,39))
#define S3(x) (ROTR(x,14) ^ ROTR(x,18) ^ ROTR(x,41))

#define F0(x,y,z) ((x & y) | (z & (x | y)))
#define F1(x,y,z) (z ^ (x & (y ^ z)))

#define P(a,b,c,d,e,f,g,h,x,K)                  \
  {                                               \
    temp1 = h + S3(e) + F1(e,f,g) + K + x;      \
    temp2 = S2(a) + F0(a,b,c);                  \
    d += temp1; h = temp1 + temp2;              \
  }

  for( i = 0; i < 16; i++ )
  {
    GET_UINT64_BE( W[i], data, i << 3 );
  }

  for( ; i < 80; i++ )
  {
    W[i] = S1(W[i -  2]) + W[i -  7] +
      S0(W[i - 15]) + W[i - 16];
  }

  A = ctx->state[0];
  B = ctx->state[1];
  C = ctx->state[2];
  D = ctx->state[3];
  E = ctx->state[4];
  F = ctx->state[5];
  G = ctx->state[6];
  H = ctx->state[7];
  i = 0;

  do
  {
    P( A, B, C, D, E, F, G, H, W[i], K[i] ); i++;
    P( H, A, B, C, D, E, F, G, W[i], K[i] ); i++;
    P( G, H, A, B, C, D, E, F, W[i], K[i] ); i++;
    P( F, G, H, A, B, C, D, E, W[i], K[i] ); i++;
    P( E, F, G, H, A, B, C, D, W[i], K[i] ); i++;
    P( D, E, F, G, H, A, B, C, W[i], K[i] ); i++;
    P( C, D, E, F, G, H, A, B, W[i], K[i] ); i++;
    P( B, C, D, E, F, G, H, A, W[i], K[i] ); i++;
  }
  while( i < 80 );

  ctx->state[0] += A;
  ctx->state[1] += B;
  ctx->state[2] += C;
  ctx->state[3] += D;
  ctx->state[4] += E;
  ctx->state[5] += F;
  ctx->state[6] += G;
  ctx->state[7] += H;
}

/*
 *  * SHA-512 process buffer
 *   */
void sh_sha512_update(sh_sha512_t *ctx, const unsigned char *input, size_t ilen )
{
  size_t fill;
  unsigned int left;

  if( ilen == 0 )
    return;

  left = (unsigned int) (ctx->total[0] & 0x7F);
  fill = 128 - left;

  ctx->total[0] += (uint64_t) ilen;

  if( ctx->total[0] < (uint64_t) ilen )
    ctx->total[1]++;

  if( left && ilen >= fill )
  {
    memcpy( (void *) (ctx->buffer + left), input, fill );
    _sh_sha512_process( ctx, ctx->buffer );
    input += fill;
    ilen  -= fill;
    left = 0;
  }

  while( ilen >= 128 )
  {
    _sh_sha512_process( ctx, input );
    input += 128;
    ilen  -= 128;
  }

  if( ilen > 0 )
    memcpy( (void *) (ctx->buffer + left), input, ilen );
}

static const unsigned char sha512_padding[128] =
{
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * SHA-512 final digest
 */
void sh_sha512_final(sh_sha512_t *ctx, unsigned char output[64])
{
  size_t last, padn;
  uint64_t high, low;
  unsigned char msglen[16];

  high = ( ctx->total[0] >> 61 )
    | ( ctx->total[1] <<  3 );
  low  = ( ctx->total[0] <<  3 );

  PUT_UINT64_BE( high, msglen, 0 );
  PUT_UINT64_BE( low,  msglen, 8 );

  last = (size_t)( ctx->total[0] & 0x7F );
  padn = ( last < 112 ) ? ( 112 - last ) : ( 240 - last );

  sh_sha512_update( ctx, sha512_padding, padn );
  sh_sha512_update( ctx, msglen, 16 );

  PUT_UINT64_BE( ctx->state[0], output,  0 );
  PUT_UINT64_BE( ctx->state[1], output,  8 );
  PUT_UINT64_BE( ctx->state[2], output, 16 );
  PUT_UINT64_BE( ctx->state[3], output, 24 );
  PUT_UINT64_BE( ctx->state[4], output, 32 );
  PUT_UINT64_BE( ctx->state[5], output, 40 );

  if( ctx->is384 == 0 )
  {
    PUT_UINT64_BE( ctx->state[6], output, 48 );
    PUT_UINT64_BE( ctx->state[7], output, 56 );
  }
}

