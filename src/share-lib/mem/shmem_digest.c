

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

