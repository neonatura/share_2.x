

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

#define __MEM__SHMEM_SCRYPT_C__
#include "share.h"
#include "shmem_scrypt_gen.h"

static const char *version = "00000bb8"; /* 3000 */

static const char *workpadding_bin = "\0\0\0\x80\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\x80\x02\0\0";


static inline int _hex2bin_char(const char c)
{
        if (c >= '0' && c <= '9')
                return c - '0';
        if (c >= 'a' && c <= 'f')
                return (c - 'a') + 10;
        if (c >= 'A' && c <= 'F')
                return (c - 'A') + 10;
        return -1;
}

/* Does the reverse of bin2hex but does not allocate any ram */
static bool hex2bin(unsigned char *p, const char *hexstr, size_t len)
{
        int n, o;

        while (len--)
        {
                n = _hex2bin_char((hexstr++)[0]);
                if (n == -1)
                {
badchar:
                        if (!hexstr[-1])
                                fprintf(stderr, "DEBUG: hex2bin: str truncated\n");
                        else
                                fprintf(stderr, "DEBUG: hex2bin: invalid character 0x%02x\n", (int)hexstr[-1]);
                        return false;
                }
                o = _hex2bin_char((hexstr++)[0]);
                if (o == -1)
                        goto badchar;
                (p++)[0] = (n << 4) | o;
        }

        return (!hexstr[0]);
}

void shscrypt_peer(scrypt_peer *peer, char *nonce1, double diff)
{ 

  strncpy(peer->nonce1, nonce1, 8);
  peer->n1_len = strlen(peer->nonce1) / 2;
  peer->n2_len = 4; /* int */

  peer->diff = diff;

}

static inline int _sh_timer_elapsed(struct timeval *begin_t, struct timeval *end_t)
{
  struct timeval now;
  if (!end_t) {
    gettimeofday(&now, NULL);
    return (now.tv_sec - begin_t->tv_sec);
  }
  return (end_t->tv_sec - begin_t->tv_sec);
}



/*
administrativo@ltcmining1:~$ ./Litecoin/litecoin/src/litecoind getwork
{   
    "midstate" : "f2cfc038d83389f1e4e44eb67b4967f4e9c0352a9eb0cacb7cc01e04c84fa039",
    "data" : "00000001d8632caadf2260bf42d2067f60d987d6b790562b272653ae611ba0fa67fe1945e3c77cc0b144c2b408614956f4bdcff4ecf1c7f2818beb3e5c6cfd61083be20c4eb747fd1d019db000000000000000800000000000000000000000000000000000000000000000000000000000000000000000000000000080020000",
    "hash1" : "00000000000000000000000000000000000000000000000000000000000000000000008000000000000000000000000000000000000000000000000000010000",
    "target" : "0000000000000000000000000000000000000000000000000000b09d01000000"
}

~/litecoind getwork
{
    "midstate" : "40fd268321efcf60e625707d4e31f9deadd13157e228985de8a10a057b98ed4d",
    "data" : "0000000105e9a54b7f65b46864bc90f55d67cccd8b6404a02f5e064a6df69282adf6e2e5f7f953b0632b25b099858b717bb7b24084148cfa841a89f106bc6b655b18d2ed4ebb191a1d018ea700000000000000800000000000000000000000000000000000000000000000000000000000000000000000000000000080020000",
    "hash1" : "00000000000000000000000000000000000000000000000000000000000000000000008000000000000000000000000000000000000000000000000000010000",
    "target" : "0000000000000000000000000000000000000000000000000000a78e01000000"
}
./netcoind getwork
{
    "midstate" : "86f4b9f4fa8f5cf5827fbc836d20d872d3ef4002453f501fc9e645ddde813834",
    "data" : "000000011328094d532943d9c65defebbbc86a121d3a08e430bd30d38580dd24a7bf6c597afc1fc1e11baf0770762963b01b77c004f40da8d8c695365ca6c381eb652645530e8a521c0f5e6f00000000000000800000000000000000000000000000000000000000000000000000000000000000000000000000000080020000",
    "hash1" : "00000000000000000000000000000000000000000000000000000000000000000000008000000000000000000000000000000000000000000000000000010000",
    "target" : "000000000000000000000000000000000000000000000000006f5e0f00000000",
    "algorithm" : "scrypt:1024,1,1"
}

The data field is stored in big-endian format. We need to cover that to little-endian for each of the fields in the data because we can pass it to the hashing function.
Data is broken down to:
Version - 00000001 (4 bytes)
Previous hash - 05e9a54b7f65b46864bc90f55d67cccd8b6404a02f5e064a6df69282adf6e2e5 (32 bytes)
Merkle root - f7f953b0632b25b099858b717bb7b24084148cfa841a89f106bc6b655b18d2ed (32 bytes)
Timestamp - 4ebb191a (4 bytes)
Bits (target in compact form) - 1d018ea7 (4 bytes)
Nonce - 00000000 (4 bytes)
You need covert these from big-endian to little-endian. This is done 2 characters at a time because each byte is represented by 2 hex chars. (each hex char is 4 bits)
Version becomes 01000000
Previous hash becomes e5e2f6.....a5e905
Merkle root becomes edd218...53f9f7
Timestamp becomes 1a19bb4e
Bits becomes a78e011d
And Nonce is a 32-bit integer you choose that will make the scrypt hash be less than the target.
Remember that you will need to convert the 32-bit nonce to hex and little-endian also. So if you are trying the nonce 2504433986. The hex version is 9546a142 in big-endian and 42a14695 in little-endian.
You then concatenate these little-endian hex strings together to get the header string (80 bytes) you input into scrypt
01000000 e5e2f6.....a5e905 edd218...53f9f7 1a19bb4e a78e011d 42a14695
*/


/**
 * @param prev_hash a 64 character hex string.
 * @param nbit a 8 character (32bit) specifying the size of the underlying transaction list. 
 * @param ntime a 8 character (32bit) specifying when the tranaction initiated.
 * @param diff the degree of difficulty in generating the scrypt hash.
 * @param merkle_list string array of 64-char merkle root entries.
 * @param coinbase2 contains the hex of the block output (i.e. the pubkey hash which the address can be derived).
 */
void shscrypt_work(scrypt_peer *peer, scrypt_work *work, 
    char *merkle_root_hex, char *prev_hash, 
    char *coinbase1, char *coinbase2, char *nbit)
{
  unsigned char *coinbase[256], merkle_root[32], merkle_sha[64];
  uint8_t merkle_bin[32];
  uint32_t *data32, *swap32;
  uint8_t diffbits[4];
  int cb1_len, cb2_len;
  int nonce2_offset;
  uint32_t ntime = time(NULL);
  char ntime_str[256];
  int cb_len;

  if (!merkle_root_hex || !*merkle_root_hex)
    merkle_root_hex = "0000000000000000000000000000000000000000000000000000000000000000";
  if (!coinbase1 || !*coinbase1)
    coinbase1 = "0000000000000000000000000000000000000000000000000000000000000000";
  if (!coinbase2 || !*coinbase2)
    coinbase2 = "0000000000000000000000000000000000000000000000000000000000000000";
  if (!prev_hash || !*prev_hash)
    prev_hash = "0000000000000000000000000000000000000000000000000000000000000000";

  work->restart = 0;

  work->sdiff = peer->diff;

sprintf(ntime_str, "%-8.8x", ntime);
  hex2bin((void*)&work->ntime, ntime_str, 4);
  work->ntime = be32toh(work->ntime);

  /* Generate coinbase */
  cb1_len = strlen(coinbase1) / 2;
  nonce2_offset = cb1_len + peer->n1_len;
  cb2_len = strlen(coinbase2) / 2;
  cb_len = nonce2_offset + peer->n2_len + cb2_len;
  memset(coinbase, 0, sizeof(coinbase));
  hex2bin(coinbase, coinbase1, cb1_len);
  hex2bin((coinbase + cb1_len), peer->nonce1, peer->n1_len);
  hex2bin((coinbase + (nonce2_offset + peer->n2_len)), coinbase2, cb2_len);

  /* Generate merkle root */
#if 0
  for (merkle_cnt = 0; merkle_list[merkle_cnt]; merkle_cnt++);
  merkle_bin = (char *)calloc((32 * merkle_cnt), sizeof(char));
  for (i = 0; i < merkle_cnt; i++) {
    hex2bin(merkle_bin + (i * 32), merkle_list[i], 32);
  }
  gen_hash(coinbase, merkle_root, cb_len);
  memcpy(merkle_sha, merkle_root, 32);
  for (i = 0; i < merkle_cnt; ++i, merkle_bin += 32) {
    memcpy(merkle_sha + 32, merkle_bin, 32);
    gen_hash(merkle_sha, merkle_root, 64);
    memcpy(merkle_sha, merkle_root, 32);
  }
#endif
  memset(merkle_root, 0, sizeof(merkle_root));
  memset(merkle_bin, 0, sizeof(merkle_bin));
  hex2bin(merkle_bin, merkle_root_hex, 32);
  gen_hash(coinbase, merkle_root, cb_len);
  memcpy(merkle_sha, merkle_root, 32);
  memcpy(merkle_sha + 32, merkle_bin, 32);
  gen_hash(merkle_sha, merkle_root, 64);
  memcpy(merkle_sha, merkle_root, 32);
  data32 = (uint32_t *)merkle_sha;
  swap32 = (uint32_t *)merkle_root;
  flip32(swap32, data32);

  hex2bin(&diffbits[0], nbit, 4);

  hex2bin(work->data, version, 4);
  hex2bin((work->data + 4), prev_hash, 32);
  memcpy (&work->data[36], merkle_root, 32);
  *((uint32_t*)&work->data[68]) = htobe32(work->ntime + _sh_timer_elapsed(&work->tv_received, NULL));
  memcpy(&work->data[72], diffbits, 4);
  memset(&work->data[76], 0, 4);  // nonce
  memcpy(&work->data[80], workpadding_bin, 48);

  calc_midstate(work);
  set_target(work->target, work->sdiff);
}

int shscrypt(scrypt_work *work, int step)
{
  uint32_t *hash_nonce = (uint32_t *)(work->data + 76);
  uint64_t ret;
  int err;

  ret = cpu_scanhash(work, step);
  work->hash_nonce = *hash_nonce;

  return (0);
}

int shscrypt_verify(scrypt_work *work)
{
  int err;

  err = test_nonce(work, work->hash_nonce);
  if (err != 1)
    return (SHERR_INVAL);
  if (err == 0)
    return (SHERR_FBIG);

  return (0);
}


_TEST(shscrypt)
{
  int err;
  char merkle_root[256];
  char prev_hash[256];
  char cb1[256];
  char cb2[256];
  char nbit[256];
  char buf[256];
  char nonce1[256];
  scrypt_peer speer;
  scrypt_work work;
  memset(&work, 0, sizeof(work));

  memset(&speer, 0, sizeof(speer));
  sprintf(nonce1, "%-8.8x", 1);
  shscrypt_peer(&speer, nonce1, 0.001);

  sprintf(merkle_root, "%-64.64x", 0x0);
  sprintf(cb1, "%-64.64x", 0x0);
  sprintf(cb2, "%-64.64x", 0x0);
  sprintf(nbit, "%-8.8x", 0x0);
  sprintf(prev_hash, "%-64.64x", 0x0);

  shscrypt_work(&speer, &work, merkle_root, prev_hash, cb1, cb2, nbit);
  _TRUE(0 == shscrypt(&work, 10240));
  _TRUE(0 == shscrypt_verify(&work));

}

