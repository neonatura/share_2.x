


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

#ifndef __MEM__SHMEM_SCRYPT_GEN_H__
#define __MEM__SHMEM_SCRYPT_GEN_H__

/**
 * Scrypt hash generator
 * @ingroup libshare_memscrypt
 */


#include <stdint.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


#define TNR_BAD -1
#define TNR_GOOD 1
#define TNR_HIGH 0

#define flip32(dest_p, src_p) swap32yes(dest_p, src_p, 32 / 4)




typedef struct bytes_t {
        uint8_t *buf;
        size_t sz;
        size_t allocsz;
} bytes_t;



int scrypt_test(unsigned char *pdata, const unsigned char *ptarget, uint32_t nonce);
void scrypt_regenhash(struct scrypt_work *work);
void calc_midstate(struct scrypt_work *work);
void set_target(unsigned char *dest_target, double diff);
int64_t cpu_scanhash(struct scrypt_work *work, int64_t max_nonce);



#endif /* __MEM__SHMEM_SCRYPT_GEN_H__ */
