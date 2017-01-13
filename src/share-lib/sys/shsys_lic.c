
/*
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
*/  

#include "share.h"



#if 0
void shlic_sig(shcert_t *cert, shkey_t *key_p)
{
  uint64_t ser_crc;
  shkey_t *key;

  ser_crc = shcrc(cert->cert_sub.ent_ser, 16);
  key = shkey_cert(shcert_sub_sig(cert), ser_crc, shcert_sub_expire(cert));  
  memcpy(key_p, key, sizeof(shkey_t));
  shkey_free(&key);
}
#endif

int shlic_sig_verify(shcert_t *cert, shkey_t *sig_key)
{
  uint64_t ser_crc;

  ser_crc = shcrc(cert->cert_sub.ent_ser, 16);
  return (shkey_verify(&sig_key, ser_crc, shcert_sub_sig(cert), shcert_sub_expire(cert)));
}

int shlic_load_sig(shkey_t *sig_key, shcert_t *lic_p)
{
  shbuf_t *buff;
  shfs_t *tree;
  SHFL *lic_fl;
  shpeer_t *peer;
  char path[PATH_MAX+1];
  int err;

  peer = shpeer_init(NULL, NULL);
  tree = shfs_init(peer);
  shpeer_free(&peer);

  /* obtain derived license certificate */
  strcpy(path, shfs_sys_dir(SHFS_DIR_LICENSE, shkey_hex(sig_key)));
  lic_fl = shfs_file_find(tree, path);
  buff = shbuf_init();
  err = shfs_read(lic_fl, buff);
  if (err) {
    shbuf_free(&buff);
    return (err);
  }

  memset(lic_p, 0, sizeof(shcert_t));
  memcpy(lic_p, shbuf_data(buff), MIN(shbuf_size(buff), sizeof(shcert_t)));
  shbuf_free(&buff);

  return (0);
}

int shlic_load(shcert_t *cert, shcert_t *lic_p)
{
  shkey_t *sig_key;
  int err;

  sig_key = shfs_cert_sig(cert);
  if (!sig_key)
    return (SHERR_INVAL);

  err = shlic_load_sig(sig_key, lic_p);
  shkey_free(&sig_key);

  return (err);
}

int shlic_save_sig(shkey_t *sig_key, shcert_t *lic)
{
  SHFL *lic_fl;
  shfs_t *tree;
  shbuf_t *buff;
  shpeer_t *peer;
  char path[PATH_MAX+1];
  int err;

  peer = shpeer_init(NULL, NULL);
  tree = shfs_init(peer);
  shpeer_free(&peer);

  /* save license using 'licensing certificate signature'. */
  strcpy(path, shfs_sys_dir(SHFS_DIR_LICENSE, (char *)shkey_hex(sig_key)));
  lic_fl = shfs_file_find(tree, path);

  /* write license contents */
  buff = shbuf_map((unsigned char *)lic, sizeof(shcert_t));
  err = shfs_write(lic_fl, buff);
  free(buff);
  if (err)
    return (err);

  return (0);
}

int shlic_save(shcert_t *cert, shcert_t *lic)
{
  shkey_t *sig_key;
  int err;

  sig_key = shfs_cert_sig(cert);
  if (!sig_key)
    return (SHERR_INVAL);

  err = shlic_save_sig(sig_key, lic);
  shkey_free(&sig_key);

  return (err);
}

int shlic_set(SHFL *file, shcert_t *cert)
{
  shkey_t sig_key;
  int err;

  if (!(cert->cert_flag & SHCERT_CERT_LICENSE))
    return (SHERR_INVAL);

#if 0
  /* "digital signature" */
  memset(&sig_key, 0, sizeof(sig_key));
  shlic_sig(cert, &sig_key);
  err = shfs_sig_set(file, &sig_key);
#endif

  /* add certificate as credentials for file */
  err = shfs_cert_apply(file, cert);
  if (err)
    return (err);

  return (0);
}

int shlic_set_name(SHFL *file, char *serial_no)
{
  shcert_t *cert;
  int err;

  cert = shfs_cert_load(serial_no);
  if (!cert)
    return (SHERR_NOENT);

  err = shlic_set(file, cert);

  shcert_free(&cert);
  return (err);
}

int shlic_get(SHFL *file, shcert_t *lic_cert_p, shcert_t *cert_p, shlic_t *lic_p)
{
  shcert_t *cert;
  shcert_t lic_cert;
  shlic_t *lic;
  shkey_t sig_key;
  shfs_t *tree;
  SHFL *lic_fl;
  char path[PATH_MAX+1];
  int err;

  /* obtain licensing certificate */
  err = shfs_cert_get(file, &cert, &lic);
  if (err)
    return (err);

  if (!(cert->cert_flag & SHCERT_CERT_LICENSE))
    return (SHERR_INVAL);

  memset(&lic_cert, 0, sizeof(lic_cert));
  err = shlic_load(cert, &lic_cert);
  if (err)
    return (err);

  err = shcert_verify(&lic_cert, cert);
  if (err)
    return (err);

  if (cert_p)
    memcpy(cert_p, cert, sizeof(shcert_t));
  if (lic_cert_p)
    memcpy(lic_cert_p, &lic_cert, sizeof(shcert_t));
  if (lic_p) {
    memcpy(lic_p, lic, sizeof(shlic_t));
  }
  free(cert);
  free(lic);

  return (0);
}

int shlic_gen(shcert_t *cert, shcert_t *lic_p)
{
  shcert_t *lic;
  int err;

  if (!(cert->cert_flag & SHCERT_CERT_LICENSE)) {
    return (SHERR_INVAL);
  }

  if (cert->cert_fee != 0) {
    return (SHERR_OPNOTSUPP);
  }

  lic = (shcert_t *)calloc(1, sizeof(shcert_t));
  err = shcert_init(lic, cert->cert_sub.ent_name, 0,
      shcert_sub_alg(cert), SHCERT_CERT_DIGITAL | SHCERT_CERT_LICENSE);
  if (err) {
    free(lic);
    return (err);
  }

  err = shcert_sign(lic, cert); 
  if (err)
    return (err);

  shlic_save(cert, lic);

  if (lic_p)
    memcpy(lic_p, lic, sizeof(shcert_t));

  return (0);
}

int shlic_gen_name(const char *serial_no, shcert_t *lic_p)
{
  shcert_t *cert;
  int err;

  cert = shfs_cert_load(serial_no);
  if (!cert)
    return (SHERR_NOENT);

  err = shlic_gen(cert, lic_p);

  shcert_free(&cert);
  return (err);
}

int shlic_cert_verify_ecdsa(shcert_t *lic)
{
  shkey_t *priv_key;
  shkey_t *pub_key;
  shkey_t *seed_key;
  shpeer_t *peer;
  int err;

  peer = shpeer_init(NULL, NULL);
  seed_key = shpeer_kpriv(peer);
  priv_key = shecdsa_key_priv((char *)shkey_hex(seed_key));
  shpeer_free(&peer);

  err = 0;
  pub_key = shecdsa_key_pub(priv_key);
  if (!shkey_cmp(pub_key, shcert_sub_sig(lic))) {
    err = SHERR_ACCESS; 
  }

  shkey_free(&priv_key);
  shkey_free(&pub_key);

  return (err);
}

int shlic_cert_verify_shr(SHFL *file, shcert_t *cert, shlic_t *lic)
{
  shfs_t *tree;

#if 0
  if (shfs_crc(file) != lic->lic_crc) {
fprintf(stderr, "DEBUG: shlic_cert_verify_shr: invalid crc (%llu)\n", (unsigned long long)lic->lic_crc); 
    return (SHERR_ILSEQ);
  }
#endif

  /* ensure signature has not expired. */
  if (shtime_before(lic->lic_expire, shtime())) {
    return (SHERR_KEYEXPIRED);
  }

#if 0
  /* verify license's signature key reference. */
/* .. */
  key = shkey_bin(raw, raw_len);
  memcpy(&lic->lic_sig, key, sizeof(shkey_t));
  shkey_free(&key);
#endif

  /* verify local file-system partition and inode token */
  tree = shfs_inode_tree(file); 
  if (!shkey_cmp(&lic->lic_fs, shpeer_kpub(&tree->peer))) {
    return (SHERR_INVAL); /* ~ NOMEDIUM */
  } 
  if (!shkey_cmp(&lic->lic_ino, shfs_token(file))) {
fprintf(stderr, "DEBUG: shlic_verify_shr: lic->lic_ino != shfs_token(file)\n");
    return (SHERR_INVAL);
  } 

  return (0);
}

/**
 * Validates authorized licensing of a file.
 */
int shlic_validate(SHFL *file)
{
  shcert_t lic_cert;
  shcert_t cert;
  shlic_t lic;
  int err;

  memset(&lic, 0, sizeof(lic));
  memset(&cert, 0, sizeof(cert));
  memset(&lic_cert, 0, sizeof(lic_cert));
  err = shlic_get(file, &lic_cert, &cert, &lic);
  if (err) {
    return (err);
  }

  err = SHERR_OPNOTSUPP;
  if (shcert_sub_alg(&lic_cert) == SHKEY_ALG_ECDSA) {
    err = shlic_cert_verify_ecdsa(&lic_cert); 
  } else if (shcert_sub_alg(&lic_cert) == SHKEY_ALG_SHR) {
    err = shlic_cert_verify_shr(file, &cert, &lic); 
  } else {
fprintf(stderr, "DEBUG: shlic_validate: unknown alg %d (%s), pcert alg %d\n", shcert_sub_alg(&lic_cert), lic_cert.cert_sub.ent_name, shcert_sub_alg(&cert));
  }

  return (err);
}



_TEST(ecdsa_shlic)
{
  shpeer_t *peer;
  SHFL *file;
  shfs_t *fs;
  shcert_t cert;
  shcert_t lic_cert;
  shcert_t cmp_cert;
  shcert_t cmp_lic;
  shbuf_t *buff;
  int err;


  peer = shpeer_init("test", NULL);
  fs = shfs_init(peer);
  _TRUEPTR(fs);
  shpeer_free(&peer);

  file = shfs_file_find(fs, "/test/shlic_ecdsa");
  _TRUEPTR(file);


  buff = shbuf_init();
  shbuf_catstr(buff, "test shlic_ecdsa");
  err = shfs_write(file, buff);
  shbuf_free(&buff);
  _TRUE(0 == err);


  /* create cert */
  memset(&cert, 0, sizeof(cert));
  err = shcert_init(&cert,
      "test_libshare: test licensing certificate (ecdsa)",
      0, SHKEY_ALG_ECDSA,
      SHCERT_ENT_ORGANIZATION | SHCERT_CERT_LICENSE | SHCERT_CERT_SIGN);
  _TRUE(0 == err);

   /* apply cert onto test file */
  err = shlic_set(file, &cert);
  _TRUE(0 == err);

  /* negative-proof */
  _TRUE(0 != shlic_get(file, NULL, NULL, NULL));

  /* obtain new license */ 
  err = shlic_gen(&cert, &lic_cert);

  _TRUE(0 == err);

  /* verify */
  _TRUE(0 == shlic_get(file, &cmp_lic, &cmp_cert, NULL));
  _TRUE(shkey_cmp(shcert_sub_sig(&cert), shcert_sub_sig(&cmp_cert)));
  _TRUE(shkey_cmp(shcert_sub_sig(&lic_cert), shcert_sub_sig(&cmp_lic)));
 
   /* verify license ownership */
  err = shlic_validate(file);
  _TRUE(0 == err);

  shfs_free(&fs);

}

_TEST(shr_shlic)
{
  shpeer_t *peer;
  SHFL *file;
  shfs_t *fs;
  shcert_t cert;
  shcert_t lic_cert;
  shcert_t cmp_cert;
  shcert_t cmp_lic;
  shlic_t lic;
  shbuf_t *buff;
  int err;


  peer = shpeer_init("test", NULL);
  fs = shfs_init(peer);
  _TRUEPTR(fs);
  shpeer_free(&peer);

  file = shfs_file_find(fs, "/test/shlic_shr");
  _TRUEPTR(file);


  buff = shbuf_init();
  shbuf_catstr(buff, "test shlic_shr");
  err = shfs_write(file, buff);
  shbuf_free(&buff);
  _TRUE(0 == err);


  /* create cert */
  memset(&cert, 0, sizeof(cert));
  err = shcert_init(&cert,
      "test_libshare: test licensing certificate (shr)",
      0, SHKEY_ALG_SHR,
      SHCERT_ENT_ORGANIZATION | SHCERT_CERT_LICENSE | SHCERT_CERT_SIGN);
  _TRUE(0 == err);

   /* apply cert onto test file */
  err = shlic_set(file, &cert);
  _TRUE(0 == err);

  /* negative-proof */
  _TRUE(0 != shlic_get(file, NULL, NULL, NULL));

  /* obtain new license */ 
  err = shlic_gen(&cert, &lic_cert);

  _TRUE(0 == err);

  /* verify */
  _TRUE(0 == shlic_get(file, &cmp_lic, &cmp_cert, &lic));
  _TRUE(shkey_cmp(shcert_sub_sig(&cert), shcert_sub_sig(&cmp_cert)));
  _TRUE(shkey_cmp(shcert_sub_sig(&lic_cert), shcert_sub_sig(&cmp_lic)));

   /* verify license ownership */
  err = shlic_validate(file);
  _TRUE(0 == err);

  shfs_free(&fs);

}


