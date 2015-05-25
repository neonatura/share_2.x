
/*
 * @copyright
 *
 *  Copyright 2015 Neo Natura
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

#ifndef __SYS__CRYPT_H__
#define __SYS__CRYPT_H__

#include <stdint.h>

/* magic sizes */
#define MD4_SIZE 16
#define MD5_SIZE 16
#define SHA_LBLOCK  16
#define SHA512_CBLOCK  (SHA_LBLOCK*8)  

typedef enum {
  MD_NONE=0,
  MD_MD2,
  MD_MD4,
  MD_MD5,
  MD_SHA1,
  MD_SHA224,
  MD_SHA256,
  MD_SHA384,
  MD_SHA512,
  MD_RIPEMD160,
} md_type_t;


typedef uint32_t UINT4;     /* 32 bits */


char *crypt_md5(const char *pw, const char *salt);
char *crypt_nthash(const char *pw, const char *salt);
char *crypt_sha256 (const char *pw, const char *salt);
char *crypt_sha512 (const char *pw, const char *salt);

//extern void _crypt_to64(char *s, u_long v, int n);
//extern void b64_from_24bit(uint8_t B2, uint8_t B1, uint8_t B0, int n, int *buflen, char **cp);


#endif /* __SYS__CRYPT_H__ */

