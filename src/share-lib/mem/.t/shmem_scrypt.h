
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

#ifndef __MEM__SHMEM_SCRYPT_H__
#define __MEM__SHMEM_SCRYPT_H__

/**
 * Scrypt hash generator
 * @ingroup libshare_mem
 * @defgroup libshare_memscrypt Scrypt hash generator
 * @{
 */

typedef struct scrypt_work 
{
        unsigned char   data[128];
        unsigned char   midstate[32];
        unsigned char   target[32];
        char hash[32];

      char ntime[16];
struct timeval tv_received;
uint32_t hash_nonce;
int restart;

        uint64_t        share_diff;

        int             rolls;
        int             drv_rolllimit; /* How much the driver can roll ntime */

        //dev_blk_ctx     blk;

        //struct thr_info *thr;
        //int             thr_id;
        //struct pool     *pool;
        struct timeval  tv_staged;

#if 0
        bool            mined;
        bool            clone;
        bool            cloned;
        int             rolltime;
        bool            longpoll;
        bool            stale;
        bool            mandatory;
        bool            block;
#endif
	uint32_t nonce;

        bool            stratum;
        char            *job_id;
  char xnonce2[16];
//        bytes_t         nonce2;
        double          sdiff;
//        char            *nonce1;

        unsigned char   work_restart_id;
        int             id;
        int             device_id;
        //UT_hash_handle hh;

        double          work_difficulty;

        char merkle_root[256]; 
#if 0
        // Allow devices to identify work if multiple sub-devices
        // DEPRECATED: New code should be using multiple processors instead
        unsigned char   subid;

        // Allow devices to timestamp work for their own purposes
        struct timeval  tv_stamp;

        //blktemplate_t   *tmpl;
        int             *tmpl_refcount;
        unsigned int    dataid;
        bool            do_foreign_submit;

        struct timeval  tv_getwork;
        time_t          ts_getwork;
        struct timeval  tv_getwork_reply;
        struct timeval  tv_cloned;
        struct timeval  tv_work_start;
        struct timeval  tv_work_found;
        char            getwork_mode;
#endif

  double pool_diff;

} scrypt_work;

typedef struct scrypt_peer
{
  char nonce1[16];
  size_t n1_len;
  size_t n2_len;
  double diff;

} scrypt_peer;


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

#define flip32(dest_p, src_p) \
  (swap32yes(dest_p, src_p, 32 / 4))

static inline uint32_t swab32(uint32_t v)
{
        return bswap_32(v);
}
static inline void swap32yes(void*out, const void*in, size_t sz) {
        size_t swapcounter = 0;
        for (swapcounter = 0; swapcounter < sz; ++swapcounter)
                (((uint32_t*)out)[swapcounter]) = swab32(((uint32_t*)in)[swapcounter]);
}

void shscrypt_peer(scrypt_peer *peer, char *nonce1, double diff);
int shscrypt(scrypt_work *work, int step);
int shscrypt_verify(scrypt_work *work);
void shscrypt_peer_gen(scrypt_peer *peer, double diff);
double shscrypt_hash_diff(scrypt_work *work);

bool scanhash_scrypt(const unsigned char *pmidstate, unsigned char *pdata, unsigned char *phash, const unsigned char *ptarget, uint32_t max_nonce, uint32_t *last_nonce, uint32_t n, double *last_diff);

void shscrypt_work(scrypt_peer *peer, scrypt_work *work, 
    char **merkle_list, char *prev_hash, 
    char *coinbase1, char *coinbase2, char *nbit, char *ntime);

void shscrypt_swap256(void *dest_p, const void *src_p);

/**
 * @}
 */

#endif /* ndef __MEM__SHMEM_SCRYPT_H__ */


