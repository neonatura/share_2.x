

/*
 * @copyright
 *
 *  Copyright 2012 Neo Natura
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

#ifndef __SHARELIB__MEM__SHMEM_DIGEST_H__
#define __SHARELIB__MEM__SHMEM_DIGEST_H__

/**
 * Generate hash checksums.
 * @ingroup libshare_mem
 * @defgroup libshare_memdigest Utility functions to generate unique checksums of data in SHA256 format.
 * @{
 */

#define _SH_SHA256_BLOCK_SIZE  ( 512 / 8)
#define SHA256_DIGEST_SIZE ( 256 / 8)

#define SHFR(x, n)    (x >> n)
#define ROTR(x, n)   ((x >> n) | (x << ((sizeof(x) << 3) - n)))
#define CH(x, y, z)  ((x & y) ^ (~x & z))
#define _SH_MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))

#define SHA256_F1(x) (ROTR(x,  2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define SHA256_F2(x) (ROTR(x,  6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SHA256_F3(x) (ROTR(x,  7) ^ ROTR(x, 18) ^ SHFR(x,  3))
#define SHA256_F4(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ SHFR(x, 10))

typedef struct {
    unsigned int tot_len;
    unsigned int len;
    unsigned char block[2 * _SH_SHA256_BLOCK_SIZE];
    uint32_t h[8];
} _sh_sha256_ctx;

void _sh_sha256_init(_sh_sha256_ctx * ctx);
void _sh_sha256_update(_sh_sha256_ctx *ctx, const unsigned char *message, unsigned int len);
void _sh_sha256_final(_sh_sha256_ctx *ctx, unsigned char *digest);
void sh_sha256(const unsigned char *message, unsigned int len, unsigned char *digest);

char *shdigest(void *data, int32_t len);

/**
 * @}
 */

#endif /* ndef__SHARELIB__MEM__SHMEM_DIGEST_H__ */


