
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
  strncpy(pub_key, ecdsa_point_compress(Q), sizeof(pub_key)-1);

  ecdsa_parameters_clear(curve);
  ecdsa_point_clear(Q);
  mpz_clear(key);

  return (shecdsa_key(pub_key));
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
  strncpy(sig_r, mpz_get_str(NULL, 16, sig->r), sizeof(sig_r)-1);

  memset(sig_s, 0, sizeof(sig_s));
  strncpy(sig_s, mpz_get_str(NULL, 16, sig->s), sizeof(sig_s)-1);

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
  ecdsa_point_decompress(Q, shkey_hex(pub_key), curve);

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


