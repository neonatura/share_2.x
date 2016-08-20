
/*
 * @copyright
 *
 *  Copyright 2013 Neo Natura 
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

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "share.h"

#ifdef HAVE_LIBGMP
#include "ecdsa/ecdsa_gmp.h"
#include "ecdsa/ecdsa_param.h"
#include "ecdsa/ecdsa_numbertheory.h"
#include "ecdsa/ecdsa_curves.h"
#include "ecdsa/ecdsa_point.h"
#include "ecdsa/ecdsa_signature.h"
#endif


/**
 * Convert a public or private key's hex string to a shkey.
 */
shkey_t *shecdsa_key(char *hex_str)
{
  shkey_t *key;
  char buf[64];
  size_t of;
  int hex_len;
  int nr;
  int i;

  key = (shkey_t *)calloc(1, sizeof(shkey_t));
  if (!key)
    return (NULL);

  nr = 0;
  hex_len = strlen(hex_str);
  for (i = 0; i < hex_len && nr < SHKEY_WORDS; i += 8) {
    memset(buf, 0, sizeof(buf));
    strncpy(buf, (hex_str + i), 8);
#if defined(HAVE_STRTOLL)
    key->code[nr++] = (uint32_t)strtoll(buf, NULL, 16);
#else
    key->code[nr++] = (uint32_t)strtol(buf, NULL, 16);
#endif
  } 
  key->alg = SHKEY_ALG_ECDSA;

  return (key);
}



/**
 * Generate a public key.
 */
shkey_t *shecdsa_key_priv(char *hex_seed)
{
#ifdef HAVE_LIBGMP
  ecdsa_parameters curve;
  mpz_t d;
  shkey_t *ret_key;
  shkey_t *ukey;

#if 0
  /* setup parameters */
  curve = ecdsa_parameters_init();
  ecdsa_parameters_load_curve(curve, secp160r1);

  /* public key */
  ecdsa_point Q = ecdsa_point_init();
  ecdsa_point Q_check = ecdsa_point_init();
#endif

  if (hex_seed) {
    ukey = shkey_hexgen(hex_seed);
  } else {
    ukey = shkey_uniq(); /* generate random */
  }
  /* truncate to "21 bytes" */
  ukey->code[5] = (ukey->code[5] & 0xff);
  ukey->code[6] = 0;
  ukey->alg = SHKEY_ALG_ECDSA;

  ret_key = shecdsa_key(shkey_hex(ukey));

  shkey_free(&ukey);

  return (ret_key);
#else
  return (NULL);
#endif
}

shkey_t *shecdsa_key_pub(shkey_t *priv_key)
{
#ifdef HAVE_LIBGMP
  ecdsa_parameters curve;
  mpz_t temp;
  mpz_t key;
  char pub_key[256];
  char *comp_hex;

  /* setup parameters */
  curve = ecdsa_parameters_init();
  ecdsa_parameters_load_curve(curve, secp160r1);

  /* initialize public key */
  ecdsa_point Q = ecdsa_point_init();
  mpz_init(key);
#if 0
  priv_key_hex = shkey_hex(priv_key);
  str_hash = shsha1_hash(priv_key_hex, strlen(priv_key_hex));
  mpz_set_str(key, str_hash, 16);
#endif
  mpz_set_str(key, shkey_hex(priv_key), 16);

#if 0
  /* key modulo n */
  mpz_init(temp);
  mpz_mod(temp, key, curve->n);
  mpz_set(key, temp);
  mpz_clear(temp);
#endif
 
  /* generate public key */
  memset(pub_key, 0, sizeof(pub_key));
  ecdsa_signature_generate_key(Q, key, curve);


  comp_hex = ecdsa_point_compress(Q); 
  if (!comp_hex) return (NULL);
  strncpy(pub_key, comp_hex, sizeof(pub_key)-1);

  ecdsa_parameters_clear(curve);
  ecdsa_point_clear(Q);
  mpz_clear(key);

  return (shecdsa_key(pub_key));
#else
  return (NULL);
#endif
}

const char *shecdsa_pub(const char *hex_str)
{
#ifdef HAVE_LIBGMP
  static char pub_key[256];
  ecdsa_parameters curve;
  mpz_t temp;
  mpz_t key;
  char *comp_hex;

  memset(pub_key, 0, sizeof(pub_key));

  /* setup parameters */
  curve = ecdsa_parameters_init();
  ecdsa_parameters_load_curve(curve, secp160r1);

  /* initialize public key */
  ecdsa_point Q = ecdsa_point_init();
  mpz_init(key);
#if 0
  priv_key_hex = shkey_hex(priv_key);
  str_hash = shsha1_hash(priv_key_hex, strlen(priv_key_hex));
  mpz_set_str(key, str_hash, 16);
#endif
  mpz_set_str(key, hex_str, 16);

#if 0
  /* key modulo n */
  mpz_init(temp);
  mpz_mod(temp, key, curve->n);
  mpz_set(key, temp);
  mpz_clear(temp);
#endif
 
  /* generate public key */
  memset(pub_key, 0, sizeof(pub_key));
  ecdsa_signature_generate_key(Q, key, curve);

  comp_hex = ecdsa_point_compress(Q); 
  if (!comp_hex) return (NULL);
  strncpy(pub_key, comp_hex, sizeof(pub_key)-1);

  ecdsa_parameters_clear(curve);
  ecdsa_point_clear(Q);
  mpz_clear(key);

  return (pub_key);
#else
  return (NULL);
#endif
}

int shecdsa_sign(shkey_t *priv_key, char *sig_r, char *sig_s, unsigned char *data, size_t data_len)
{
#ifdef HAVE_LIBGMP
  ecdsa_parameters curve;
  ecdsa_signature sig;
  uint8_t *hash;
  mpz_t temp;
  mpz_t key;
  mpz_t m;

  /* setup parameters */
  curve = ecdsa_parameters_init();
  ecdsa_parameters_load_curve(curve, secp160r1);

  /* generate private key from user-context */
  mpz_init(key);
#if 0
  priv_key_hex = shkey_hex(priv_key);
  hash = shsha1_hash(priv_key_hex, strlen(priv_key_hex));
  mpz_set_str(key, hash, 16);
#endif
  mpz_set_str(key, shkey_hex(priv_key), 16);

#if 0
  /* key modulo n */
  mpz_init(temp);
  mpz_mod(temp, key, curve->n);
  mpz_set(key, temp);
#endif

  /* process message into sha1 hash */
  mpz_init(m);
  hash = shsha1_hash(data, data_len);
  mpz_set_str(m, hash, 16);

  /* msg modulo n */
  mpz_init(temp);
  mpz_mod(temp, m, curve->n);
  mpz_set(m, temp);
  mpz_clear(temp);

  /* generate signature */
  sig = ecdsa_signature_init();
  ecdsa_signature_sign(sig, m, key, curve);

#if 0
  {
    FILE *out = stdout;
    fprintf(out, "Signature:\n\tR:");
    mpz_out_str(out, 16, sig->r);
    fprintf(out, "\n\tS:");
    mpz_out_str(out, 16, sig->s);
    fprintf(out, "\n");
  }
#endif

  memset(sig_r, 0, sizeof(sig_r));
  strcpy(sig_r, mpz_get_str(NULL, 16, sig->r));

  memset(sig_s, 0, sizeof(sig_s));
  strcpy(sig_s, mpz_get_str(NULL, 16, sig->s));

  ecdsa_parameters_clear(curve);
  ecdsa_signature_clear(sig);
  mpz_clear(key);
  mpz_clear(m);

  return (0);
#else
  return (SHERR_OPNOTSUPP);
#endif
}

int shecdsa_verify(shkey_t *pub_key, char *str_r, char *str_s, unsigned char *data, size_t data_len)
{
#ifdef HAVE_LIBGMP
  ecdsa_parameters curve;
  ecdsa_signature sig;
  ecdsa_point Q;
  mpz_t temp;
  mpz_t m;
  uint8_t *hash;
  int ok;


  /* setup parameters */
  curve = ecdsa_parameters_init();
  ecdsa_parameters_load_curve(curve, secp160r1);

  /* decompress public key */
  Q = ecdsa_point_init();
  ecdsa_point_decompress(Q, shkey_hex(pub_key), curve);

  /* process message into sha1 hash */
  mpz_init(m);
  hash = shsha1_hash(data, data_len);
  mpz_set_str(m, hash, 16);

  /* msg modulo n - note standard is bit-length not mod */
  mpz_init(temp);
  mpz_mod(temp, m, curve->n);
  mpz_set(m, temp);
  mpz_clear(temp);

  sig = ecdsa_signature_init();
  ecdsa_signature_set_str(sig, str_r, str_s, 16);

  /* verify signature */
  ok = ecdsa_signature_verify(m, sig, Q, curve);

  ecdsa_parameters_clear(curve);
  ecdsa_signature_clear(sig);
  ecdsa_point_clear(Q);
  mpz_clear(m);

  if (!ok)
    return (SHERR_ACCESS);


  return (0);
#else
  return (SHERR_OPNOTSUPP);
#endif
}
 

_TEST(ecdsa)
{
#ifdef HAVE_LIBGMP
  shkey_t *ukey;
  mpz_t d;
  mpz_t m;

  //Setting up domain parameters
  ecdsa_parameters curve = ecdsa_parameters_init();
  ecdsa_parameters_load_curve(curve, secp160r1);

  //Public key
  ecdsa_point Q = ecdsa_point_init();

  //Message
  mpz_init(m);

  //Private key
  mpz_init(d);

  //Signature
  ecdsa_signature sig = ecdsa_signature_init();

  //Message hash just a random number
  mpz_set_str(m, "2156842181254876268462177895321953219548746516484", 10);

  /* set private key to random "21 byte" integer */
  ukey = shkey_uniq(); /* generate random */
  ukey->code[5] = (ukey->code[5] & 0xff);
  ukey->code[6] = 0;
  mpz_set_str(d, shkey_hex(ukey), 16);
  shkey_free(&ukey);

  //Generate ecdsa_signature
  ecdsa_signature_sign(sig, m, d, curve);

  //Generate public key
  ecdsa_signature_generate_key(Q, d, curve);

  //Verify result
  int result = ecdsa_signature_verify(m, sig, Q, curve);

  //Release memory
  mpz_clear(m);
  mpz_clear(d);
  ecdsa_point_clear(Q);
  ecdsa_signature_clear(sig);
  ecdsa_parameters_clear(curve);

  _TRUE(result);
#endif
}




_TEST(shecdsa)
{
#ifdef HAVE_LIBGMP
  shkey_t *priv_key;
  shkey_t *pub_key;
  char msg[256];
  char sig_r[1024];
  char sig_s[1024];
  int err;

  priv_key = shecdsa_key_priv(NULL);
  _TRUEPTR(priv_key);

  pub_key = shecdsa_key_pub(priv_key);
  _TRUEPTR(pub_key);

  memset(msg, 0xb, sizeof(msg));
  shecdsa_sign(priv_key, sig_r, sig_s, msg, sizeof(msg));

  err = shecdsa_verify(pub_key, sig_r, sig_s, msg, sizeof(msg));
if (err) fprintf(stderr, "DEBUG: %d = shecdsa_verify()\n", err);
  _TRUE(err == 0);

  shkey_free(&priv_key);
  shkey_free(&pub_key);
#endif
}

_TEST(shecdsa_integrity)
{
#ifdef HAVE_LIBGMP
  ecdsa_parameters curve;
  ecdsa_point Q;
  shkey_t *priv_key;
  shkey_t *pub_key;
  int idx;

  /* setup parameters */
  curve = ecdsa_parameters_init();
  ecdsa_parameters_load_curve(curve, secp160r1);

  priv_key = shecdsa_key_priv(NULL);
  _TRUEPTR(priv_key);

  pub_key = shecdsa_key_pub(priv_key);
  _TRUEPTR(pub_key);

  /* decompress public key */
  Q = ecdsa_point_init();
  ecdsa_point_decompress(Q, (char *)shkey_hex(pub_key), curve);

  //A guessing ecdsa_point
  ecdsa_point guess = ecdsa_point_init();
  ecdsa_point_set(guess, curve->G);

  //A second guessing ecdsa_point explanation follows
  ecdsa_point guess2 = ecdsa_point_init();

  //Initialize two counter
  mpz_t i; mpz_init(i);
  mpz_t i2; mpz_init(i2);
  mpz_set_ui(i, 0);

  /* attempt a high-volume of guesses */
  for (idx = 0; idx < 100000; idx++) {
    //Break the loop if our guess matches the public key
    if(ecdsa_point_cmp(guess,Q))
      break;

    //Add G to the guess, store it in guess two (it can't be stored in guess)
    ecdsa_point_addition(guess2, guess, curve->G, curve);

    //Add one to our counter
    mpz_add_ui(i2, i, 1);

    //Break the loop if our guess matches the public key
    if(ecdsa_point_cmp(guess2,Q))
      break;

    //Add G to the guess2, store it in guess (it can't be stored in guess2)
    ecdsa_point_addition(guess, guess2, curve->G, curve);

    //Add one to our counter
    mpz_add_ui(i, i2, 1);
  }
  _TRUE(idx == 100000);

  if(mpz_cmp(i,i2) < 0)
    mpz_set(i, i2);
#if 0
  {
    FILE *out = stdout;
    fprintf(out, "Hashsum of the private key is:\n");
    mpz_out_str(out, 16, i);
    fprintf(out, "\n");
  }
#endif

  shkey_free(&priv_key);
  shkey_free(&pub_key);

  ecdsa_parameters_clear(curve);
  ecdsa_point_clear(Q);
  ecdsa_point_clear(guess);
  mpz_clear(i);

#endif /* HAVE_LIBGMP */
}




static void *_memxor (void *restrict dest, const void *restrict src, size_t n)
{
  char const *s = (const char *)src;
  char *d = (char *)dest;

  for (; n > 0; n--)
    *d++ ^= *s++;

  return dest;
}

#define IPAD 0x36
#define OPAD 0x5c
static int _sha512_hmac(const unsigned char *key, size_t keylen,
    const unsigned char *in, size_t inlen, unsigned char *resbuf)
{
  sh_sha512_t inner;
  sh_sha512_t outer;
  unsigned char optkeybuf[64];
  unsigned char block[128];
  unsigned char innerhash[64];

  /* Reduce the key's size, so that it becomes <= 64 bytes large.  */

  if (keylen > 128)
  {
    sh_sha512_t keyhash;

    sh_sha512_init (&keyhash);
    sh_sha512_update (&keyhash, key, keylen);
    sh_sha512_final (&keyhash, optkeybuf);

    key = optkeybuf;
    keylen = 64;
  }

  /* Compute INNERHASH from KEY and IN.  */

  sh_sha512_init (&inner);

  memset (block, IPAD, sizeof (block));
  _memxor (block, key, keylen);

  sh_sha512_process (&inner, block); /* 128 bytes */
  sh_sha512_update (&inner, in, inlen);

  sh_sha512_final (&inner, innerhash);

  /* Compute result from KEY and INNERHASH.  */

  sh_sha512_init (&outer);

  memset (block, OPAD, sizeof (block));
  _memxor (block, key, keylen);

  sh_sha512_process (&outer, block); /* 128 bytes */
  sh_sha512_update (&outer, innerhash, 64);

  sh_sha512_final (&outer, resbuf);

  return 0;
}

bool hex2bin(unsigned char *p, const char *hexstr, size_t len);

void bin2hex(char *str, unsigned char *bin, size_t bin_len);

/**
 * @param secret The secret key in hexadecimal format.
 */
char *shecdsa_hd_point_hex(char *secret)
{
#ifdef HAVE_LIBGMP
  static char ret_buf[256];
  ecdsa_parameters curve = ecdsa_parameters_init();
  ecdsa_point Q = ecdsa_point_init();
  mpz_t key;
  char *hex;
  char ret_x[256];
  char ret_y[256];

  ecdsa_parameters_load_curve(curve, secp256k1);

  mpz_init(key);
  mpz_set_str(key, secret, 16);

/* TRY?: load secret as decompressed point */

  /* generate public key */
  ecdsa_signature_generate_key(Q, key, curve);

  mpz_get_str(ret_y, 16, Q->y);
  mpz_get_str(ret_x, 16, Q->x);

  memset(ret_buf, 0, sizeof(ret_buf));
  memset(ret_buf, '0', 130);
  strncpy(ret_buf, "04", 2);
  strncpy(ret_buf + 2, ret_y, 64); 
  strncpy(ret_buf + 66, ret_x, 64); 

  mpz_clear(key);
  ecdsa_point_clear(Q);
  ecdsa_parameters_clear(curve);

  return (ret_buf);
#else
  return (NULL);
#endif /* HAVE_LIBGMP */
}

char *shecdsa_hd_pubkey(char *pubkey, char *chain, uint32_t idx)
{
  static char ret_buf[256];
  ecdsa_parameters curve;
  mpz_t pk;
  mpz_t hl;
  mpz_t temp;
  mpz_t r;
  char buf[256];
  unsigned char data[256];
  unsigned char chain_data[256];
char hmac_l_hex[256];
  char hmac[256];
  char hmac_l[256];
  char hmac_r[256];
  uint32_t val;
  int i;
ecdsa_point Q;
char t_buf[256];
int of;
  
  hex2bin(data, pubkey, 65);

  idx = htonl(idx); 
  memcpy(data + 65, &idx, sizeof(idx));

  hex2bin(chain_data, chain, 32);
 
  memset(hmac, 0, sizeof(hmac));
  memset(hmac_l, 0, sizeof(hmac_l));
  memset(hmac_r, 0, sizeof(hmac_r));
  _sha512_hmac(chain_data, 32, data, 69, hmac);
  bin2hex(hmac_l, hmac, 32);
  bin2hex(hmac_r, hmac + 32, 32);

  curve = ecdsa_parameters_init();
  ecdsa_parameters_load_curve(curve, secp256k1);

  Q = ecdsa_point_init();

  mpz_t s;
  mpz_init(s);
  mpz_set_str(s, hmac_l, 16);
  ecdsa_signature_generate_key(Q, s, curve);
  mpz_clear(s);
  ecdsa_parameters_clear(curve);

char ret_x[256], ret_y[256];
  mpz_get_str(ret_x, 16, Q->x);
  mpz_get_str(ret_y, 16, Q->y);

memset(t_buf, 0, sizeof(t_buf));
memset(t_buf, '0', 130);
  strcpy(t_buf, "04");
  strncpy(t_buf+2, ret_x, strlen(ret_x));
  strncpy(t_buf+66, ret_y, strlen(ret_y));

  mpz_init(pk);
  mpz_init(hl);
  mpz_init(r);


  mpz_set_str(pk, t_buf, 16);
  mpz_set_str(hl, hmac_l, 16);
  mpz_add(r, hl, pk);

  memset(buf, 0, sizeof(buf));
  mpz_get_str(buf, 16, r);
  memset(ret_buf, 0, sizeof(ret_buf));
  memset(ret_buf, '0', 130);
  of = MAX(0, 130 - strlen(buf));
  strcpy(ret_buf + of, buf); 

  mpz_clear(pk);
  mpz_clear(hl);
  mpz_clear(r);

  ecdsa_point_clear(Q);

  /* return new chain sequence */
  strncpy(chain, hmac_r, 64);

  return (ret_buf);
}

char *shecdsa_hd_privkey(char *pubkey, char *chain, char *seed, uint32_t idx)
{
  const static char ret_buf[256];
  mpz_t pk;
  mpz_t hl;
  mpz_t r;
  mpz_t temp;
  char buf[256];
  unsigned char data[256];
  unsigned char chain_data[256];
  char hmac[256];
  char hmac_l[256];
  char hmac_r[256];
  uint32_t val;
  int i;
mpz_t y;
char hex_x[256];
char hex_y[256];
  ecdsa_point Q;
char *hex;

  if (strlen(pubkey) != 130) {
fprintf(stderr, "DEBUG: ERROR: shecdsa_hd_privkey: pubkey is not len 130 (%d): '%s'\n", strlen(pubkey), pubkey); 
return (NULL);
}

  ecdsa_parameters curve;
  curve = ecdsa_parameters_init();
  ecdsa_parameters_load_curve(curve, secp256k1);

  memset(data, 0, sizeof(data));

  memset(hex_y, 0, sizeof(hex_y));
  memset(hex_x, 0, sizeof(hex_x));
  strncpy(hex_y, pubkey + 2, 64);
  strncpy(hex_x, pubkey + 66, 64);

  Q = ecdsa_point_init();
  ecdsa_point_set_hex(Q, hex_x, hex_y);

  /* compress point */
  hex = ecdsa_point_compress(Q);
if (strlen(hex) < 4) fprintf(stderr, "DEBUG: shecdsa_hd_privkey: ecdsa_point_compress: '%s' [x '%s', y '%s']\n", hex, hex_x, hex_y);
  hex2bin(data, hex, 33);

  ecdsa_point_clear(Q);
  
  idx = htonl(idx); 
  memcpy(data + 65, &idx, sizeof(idx));

  hex2bin(chain_data, chain, 32);
 
  memset(hmac, 0, sizeof(hmac));
  memset(hmac_l, 0, sizeof(hmac_l));
  memset(hmac_r, 0, sizeof(hmac_r));
  _sha512_hmac(chain_data, 32, data, 69, hmac);
  bin2hex(hmac_l, hmac, 32);
  bin2hex(hmac_r, hmac + 32, 32);

  mpz_init(pk);
  mpz_init(hl);
  mpz_init(r);

  mpz_set_str(pk, seed, 16);
  mpz_set_str(hl, hmac_l, 16);
  mpz_add(r, hl, pk);

  /* key modulo n */
  mpz_init(temp);
  mpz_mod(temp, r, curve->n);
  mpz_set(r, temp);
  mpz_clear(temp);

  mpz_get_str(ret_buf, 16, r);

  mpz_clear(pk);
  mpz_clear(hl);
  mpz_clear(r);

  ecdsa_parameters_clear(curve);

  /* return new chain sequence */
  strncpy(chain, hmac_r, 64);

  return (ret_buf);
}

char *shecdsa_hd_priv2pub(char *secret, char *chain, uint32_t self_idx)
{
  char *pub_hex;
  char *hex;

  pub_hex = shecdsa_hd_point_hex(secret); 
  hex = shecdsa_hd_pubkey(pub_hex, chain, self_idx);

  return (hex);
}

char *shecdsa_hd_seed(char *seed_hex, char *chain)
{
  static char ret_buf[256];
  uint32_t magic = SHMEM32_MAGIC;
  unsigned char *raw_magic = (unsigned char *)&magic;
  unsigned char hmac[256];
  size_t seed_len;
  char hmac_l[256];
  char hmac_r[256];
  unsigned char seed[256];

  memset(hmac, 0, sizeof(hmac));

  seed_len = MIN(sizeof(seed)*2, strlen(seed_hex)/2);
  hex2bin(seed, seed_hex, seed_len); 

  memset(hmac_l, 0, sizeof(hmac_l));
  memset(hmac_r, 0, sizeof(hmac_r));
  _sha512_hmac(seed, seed_len, raw_magic, sizeof(uint32_t), hmac);
  bin2hex(hmac_l, hmac, 32);
  bin2hex(hmac_r, hmac + 32, 32);

  if (chain) {
    /* return new chain sequence */
    strcpy(chain, hmac_r);
  }
  
  memset(ret_buf, 0, sizeof(ret_buf));
  strncpy(ret_buf, hmac_l, sizeof(ret_buf)-1);

  return (ret_buf);

}




static int _test_shecdsa_hd_derive(char *master_secret, char *master_pubkey, char *m_chain, int idx)
{
  static char last_privkey[256];
  char *pubkey_hex;
  char *privkey_hex;
  char chain[256];
  char *pubkey2_hex;

//fprintf(stderr, "\n-- #%-3.3d --\n", idx);

  /* derive private key */
  memset(chain, 0, sizeof(chain));
  strcpy(chain, m_chain);
  privkey_hex = shecdsa_hd_privkey(master_pubkey, chain, master_secret, idx);
//fprintf(stderr, "DEBUG: PRIV: derived privkey hex '%s'\n", privkey_hex);

  /* derive public key */
  memset(chain, 0, sizeof(chain));
  strcpy(chain, m_chain);
  pubkey_hex = shecdsa_hd_pubkey(master_pubkey, chain, idx);
//fprintf(stderr, "DEBUG: PUB: derived pubkey hex '%s'\n", pubkey_hex);

  memset(chain, 0, sizeof(chain));
  strcpy(chain, m_chain);
  pubkey2_hex = shecdsa_hd_priv2pub(master_secret, chain, idx);
//fprintf(stderr, "DEBUG: PRIV2PUB: child pubkey from master secret '%s'\n", pubkey2_hex);

  if (0 == strcmp(last_privkey, privkey_hex))
    return (-1);
  memset(last_privkey, 0, sizeof(last_privkey));
  strncpy(last_privkey, privkey_hex, sizeof(last_privkey)-1);

  if (0 != strcmp(pubkey_hex, pubkey2_hex))
    return (-1);

  return (0);
}



_TEST(shecdsa_hd)
{
  const char *m_seed = "7384f492935706bcc8b6a844d90e5c04e0b77907e0fbd3c2da6abc4ba61447da";
  char m_pubkey[256];
  char m_chain[256];
  char m_secret[256];
  int idx;

  memset(m_chain, 0, sizeof(m_chain));
  memset(m_secret, 0, sizeof(m_secret));
  strcpy(m_secret, shecdsa_hd_seed((char *)m_seed, m_chain));
//fprintf(stderr, "DEBUG: MASTER SECRET: '%s'\n", m_secret);

  memset(m_pubkey, 0, sizeof(m_pubkey));
  strcpy(m_pubkey, shecdsa_hd_point_hex(m_secret));
//fprintf(stderr, "DEBUG: MASTER PUB: '%s'\n", m_pubkey);

  for (idx = 1; idx < 10; idx++) {
    _TRUE(_test_shecdsa_hd_derive(m_secret, m_pubkey, m_chain, idx) == 0);
  }
 
}




#if 0
char *shecdsa_hd_priv2pub(char *secret, char *chain)
{
  static char ret_buf[256];
  ecdsa_parameters curve = ecdsa_parameters_init();
  ecdsa_point Q = ecdsa_point_init();
  mpz_t key;
  char *hex;
char ret_x[256];
char ret_y[256];
mpz_t temp;

  ecdsa_parameters_load_curve(curve, secp256k1);

  mpz_init(key);
  mpz_set_str(key, seed, 16);

#if 0
  /* key modulo n */
  mpz_init(temp);
  mpz_mod(temp, key, curve->n);
  mpz_set(key, temp);
  mpz_clear(temp);
#endif

  /* generate public key */
  ecdsa_signature_generate_key(Q, key, curve);


  mpz_get_str(ret_x, 16, Q->x);
  mpz_get_str(ret_y, 16, Q->y);
fprintf(stderr, "DEBUG: shecdsa_hd_priv2pub: ret_x '%s', ret_y '%s'\n", ret_x, ret_y);

memset(ret_buf, 0, sizeof(ret_buf));
memset(ret_buf, '0', 130);
  strcpy(ret_buf, "04");
  strncpy(ret_buf+2, ret_x, strlen(ret_x));
  strncpy(ret_buf+66, ret_y, strlen(ret_y));
/*
  hex = ecdsa_point_compress(Q);
  strcpy(ret_buf, hex);
*/
fprintf(stderr, "DEBUG: shecdsa_hd_priv2pub: '%s' -> '%s'\n", seed, ret_buf);

  mpz_clear(key);
  ecdsa_point_clear(Q);
  ecdsa_parameters_clear(curve);

return (ret_buf);
}
#endif



#if 0
const char *shecsda_point(char *hex_x, char *hex_y)
{
  static char ret_buf[256];
  ecdsa_point Q;

  memset(ret_buf, 0, sizeof(ret_buf));

  Q = ecdsa_point_init();
  ecdsa_point(Q, hex_x, hex_y);
  ecdsa_point_clear(Q);
}
#endif

#if 0
void shecdsa_point_key(char *hex_x, char *hex_y, char *hex_z, char *ret_x, char *ret_y)
{
  static char ret_buf[256];
  ecdsa_parameters curve;
  ecdsa_point Q;
  ecdsa_point R;
  mpz_t mul;

  curve = ecdsa_parameters_init();
  ecdsa_parameters_load_curve(curve, secp256k1);

  Q = ecdsa_point_init();
  ecdsa_point_set_hex(Q, hex_x, hex_y);

  mpz_init(mul);
  mpz_set_str(mul, hex_z, 16);

  R = ecdsa_point_init();
  ecdsa_point_multiplication(R, mul, Q, curve);

  mpz_get_str(ret_x, 16, R->x);
  mpz_get_str(ret_y, 16, R->y);

  mpz_clear(mul);
  ecdsa_point_clear(Q);
  ecdsa_point_clear(R);
  ecdsa_parameters_clear(curve);
}
#endif

#if 0
void shecdsa_append_key(char *seed_hex, char *seq_hex, char *ret_hex)
{
  static char ret_buf[256];
  ecdsa_parameters curve;
  mpz_t seed;
  mpz_t seq;
  mpz_t temp;

  curve = ecdsa_parameters_init();
  ecdsa_parameters_load_curve(curve, secp256k1);

  mpz_init(seed);
  mpz_set_str(seed, seed_hex, 16);

  mpz_init(seq);
  mpz_set_str(seq, seq_hex, 16);

  mpz_init(temp);
  mpz_add(temp, seed, seq);
  mpz_set(seed, temp);
  mpz_clear(temp);

  mpz_init(temp);
  mpz_mod(temp, seed, curve->n);
  mpz_set(seed, temp);
  mpz_clear(temp);

  memset(ret_buf, 0, sizeof(ret_buf));
  mpz_get_str(ret_buf, 16, seed);

  mpz_clear(seed);
  mpz_clear(seq);
  ecdsa_parameters_clear(curve);

  strcpy(ret_hex, ret_buf);
}
#endif

#if 0
void shecdsa_point_pubkey(const char *seed_hex, char *ret_x, char *ret_y)
{
#ifdef HAVE_LIBGMP
  ecdsa_parameters curve;
  mpz_t temp;
  mpz_t key;
  char pub_key[256];
  char *comp_hex;

  /* setup parameters */
  curve = ecdsa_parameters_init();
  ecdsa_parameters_load_curve(curve, secp256k1);

  /* initialize public key */
  ecdsa_point Q = ecdsa_point_init();
  mpz_init(key);

  mpz_set_str(key, seed_hex, 16);

  /* generate public key */
  memset(pub_key, 0, sizeof(pub_key));
  ecdsa_signature_generate_key(Q, key, curve);

  mpz_get_str(ret_x, 16, Q->x);
  mpz_get_str(ret_y, 16, Q->y);

#if 0
  comp_hex = ecdsa_point_compress(Q); 
  if (!comp_hex) return (NULL);
  strncpy(ret_hex, comp_hex, sizeof(pub_key)-1);
#endif

  ecdsa_parameters_clear(curve);
  ecdsa_point_clear(Q);
  mpz_clear(key);
#else
  return (NULL);
#endif
}
#endif


#if 0
unsigned char *ecdsa_point_compress_hex(char *point_hex)
{
static char ret_buf[256];
  ecdsa_parameters curve;
  ecdsa_point Q;
  char hex_x[256];
  char hex_y[256];
char *comp_hex;

  if (!point_hex || strlen(point_hex) != 130)
    return (NULL);

  if (0 != strncmp(point_hex, "04", 2)) {
fprintf(stderr, "DEBUG: ecdsa_point_compress_hex: invalid uncompress format '%s'\n", point_hex);
    return (NULL);
}

  memset(hex_x, 0, sizeof(hex_x));
  memset(hex_y, 0, sizeof(hex_y));
  strncpy(hex_y, point_hex + 2, 64);
  strncpy(hex_x, point_hex + 66, 64);

  curve = ecdsa_parameters_init();
  ecdsa_parameters_load_curve(curve, secp256k1);
  Q = ecdsa_point_init();

  ecdsa_point_set_hex(Q, hex_x, hex_y);

  comp_hex = ecdsa_point_compress(Q);
  memset(ret_buf, 0, sizeof(ret_buf));
  strncpy(ret_buf, comp_hex, sizeof(ret_buf)-1);

  ecdsa_point_clear(Q);
  ecdsa_parameters_clear(curve);

  return (ret_buf);
}
#endif
