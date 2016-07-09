
/*
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
*/  

#include "share.h"





int shcert_init(shcert_t *cert, char *entity, uint64_t fee, int flags)
{
  shpeer_t *peer;
  shkey_t *key;
  int i;

  /* the relevant name or entity subject */
  memset(cert->cert_sub.ent_name, '\0', sizeof(cert->cert_sub.ent_name));
  if (entity)
    strncpy(cert->cert_sub.ent_name, entity, sizeof(cert->cert_sub.ent_name) - 1);

  /* certificate version */
  cert->cert_ver = 3;

  /* certificate algorythm */
  cert->cert_sub.ent_sig.sig_key.alg = SHKEY_ALG_SHR;

  /* set birth and expiration time-stamps */
  cert->cert_sub.ent_sig.sig_stamp = shtime_adj(shtime(), -1);
  cert->cert_sub.ent_sig.sig_expire = 
    shtime_adj(shtime(), SHARE_DEFAULT_EXPIRE_TIME);

  /* coin cost to license certificate. */
  cert->cert_fee = fee;

  /* certificate attributes */
  cert->cert_flag = flags;

  /* certificate's issuer peer entity */
  peer = shpeer();
  memcpy(&cert->cert_sub.ent_peer, peer, sizeof(cert->cert_sub.ent_peer));
  shpeer_free(&peer);

  /* fill with random serial number */
  for (i = 0; i < 16; i += sizeof(int))
    *( (int *) (cert->cert_ser+i) ) = rand(); 

  /* specify key length */
  shcert_sub_len(cert) = 24; /* 192-bit key */

  /* generate public key */
  memset(shcert_sub_sig(cert), '\000', sizeof(shkey_t));
  key = shkey_bin(cert, sizeof(shcert_t));
  memcpy(shcert_sub_sig(cert), key, sizeof(shkey_t));
  shkey_free(&key);

  return (0);
}

int shcert_ca_init(shcert_t *cert, char *entity, uint64_t fee, int flags)
{
  return (shcert_init(cert, entity, fee, 
        flags | SHCERT_CERT_SIGN | /* !CHAIN & can sign certs */ 
        SHCERT_AUTH_WEB_CLIENT | SHCERT_AUTH_WEB_CLIENT | /* web-ssl */
        SHCERT_CERT_LICENSE | SHCERT_CERT_DIGITAL)); /* shpkg */
}

void shcert_free(shcert_t **cert_p)
{
  shcert_t *cert;

  if (!cert_p)
    return;

  cert = *cert_p;
  *cert_p = NULL;

  free(cert);
}

/**
 * @see shsig_shr_sign()
 */
int shcert_sign(shcert_t *cert, shcert_t *parent)
{
  shkey_t *key;
  unsigned char *enc_data;
  size_t enc_len;
  int err;

  if (!(parent->cert_flag & SHCERT_CERT_SIGN)) {
    /* parent certificate lacks ability to sign. */
    return (SHERR_INVAL);
  }

  err = shencode((char *)&parent->cert_sub.ent_sig.sig_key, sizeof(shkey_t),
    &enc_data, &enc_len, &parent->cert_iss.ent_sig.sig_key);
  if (err)
    return (err);

  key = shkey_bin(enc_data, enc_len);
  free(enc_data);
  memcpy(&cert->cert_sub.ent_sig.sig_key, key, sizeof(shkey_t));
  cert->cert_sub.ent_len = enc_len;
  shkey_free(&key);

  cert->cert_flag |= SHCERT_CERT_CHAIN;
  cert->cert_flag |= parent->cert_flag; /* inherit parent's capabilities */
  strcpy(cert->cert_iss.ent_name, parent->cert_sub.ent_name); 
  cert->cert_iss.ent_sig.sig_stamp = parent->cert_sub.ent_sig.sig_stamp;
  cert->cert_iss.ent_sig.sig_expire = parent->cert_sub.ent_sig.sig_expire;
  cert->cert_iss.ent_len = parent->cert_sub.ent_len;

  return (0);
}

/**
 * @see shsig_shr_verify()
 */
int shcert_sign_verify(shcert_t *cert, shcert_t *parent)
{
  shkey_t *key;
  unsigned char *enc_data;
  size_t enc_len;
  int err;

  err = shencode((char *)&parent->cert_sub.ent_sig.sig_key, sizeof(shkey_t),
    &enc_data, &enc_len, &parent->cert_iss.ent_sig.sig_key);
  if (err)
    return (err);

  key = shkey_bin(enc_data, enc_len);
  free(enc_data);
  if (!shkey_cmp(key, &cert->cert_sub.ent_sig.sig_key)) {
    /* encrypted key is not validated. */
    shkey_free(&key);
    return (SHERR_KEYREJECTED);
  }

  shkey_free(&key);
  return (0); 
}

int shcert_verify(shcert_t *cert, shcert_t *parent)
{
  shtime_t now;
  int err;

  if (!(cert->cert_flag & SHCERT_CERT_CHAIN)) {
    /* initial (CA) chain entity */
    if (parent)
      return (SHERR_INVAL);

    return (0);
  }

  /* supplemental chain entity */
  if (!parent)
    return (SHERR_INVAL);

  /* The Issuer of each certificate (except the last one) matches the Subject of the next (parent) certificate in the list. */
  if (0 != strcasecmp(cert->cert_iss.ent_name, 
        parent->cert_sub.ent_name)) {
    return (SHERR_ACCESS); 
  }

  now = shtime();
  if (!shtime_after(now, shcert_sub_stamp(cert))) {
    return (SHERR_ACCESS);
  }
  if (!shtime_before(now, shcert_sub_expire(cert))) {
    return (SHERR_KEYEXPIRED);
  }

  /* The signature of one certificate can be verified using the public key contained in the following certificate. */
  err = shcert_sign_verify(cert, parent); 
  if (err)
    return (err);

  return (0);
}

_TEST(shcert_sign)
{
  shcert_t ca_cert;
  shcert_t cert;
  int err;

  memset(&ca_cert, 0, sizeof(ca_cert));
  err = shcert_ca_init(&ca_cert, "test server", 0, SHCERT_ENT_ORGANIZATION);
  _TRUE(0 == err);

  memset(&cert, 0, sizeof(cert));
  err = shcert_init(&cert, "test client", 0, SHCERT_ENT_ORGANIZATION);
  _TRUE(0 == err);

  err = shcert_sign(&cert, &ca_cert);
  _TRUE(0 == err);

  err = shcert_verify(&cert, &ca_cert);
  _TRUE(0 == err);

}

char *shcert_flag_str(int flags)
{
  static char ret_buf[1024];

  if (flags & SHCERT_ENT_INDIVIDUAL)
    strcat(ret_buf, "INDIVIDUAL ");
  if (flags & SHCERT_ENT_ORGANIZATION)
    strcat(ret_buf, "ORGANIZATION ");
  if (flags & SHCERT_ENT_COMPANY)
    strcat(ret_buf, "COMPANY ");
  if (flags & SHCERT_ENT_PRIVATE)
    strcat(ret_buf, "PRIVATE ");

  if (flags & SHCERT_CERT_CHAIN) 
    strcat(ret_buf, "CHAIN ");
  if (flags & SHCERT_CERT_SIGN)
    strcat(ret_buf, "SIGN ");
  if (flags & SHCERT_CERT_CRL)
    strcat(ret_buf, "CRL ");
  if (flags & SHCERT_CERT_KEY)
    strcat(ret_buf, "KEY ");
  if (flags & SHCERT_CERT_ENCIPHER)
    strcat(ret_buf, "ENCIPHER ");
  if (flags & SHCERT_CERT_NONREPUDIATION)
    strcat(ret_buf, "NON-REPUDIATION ");
  if (flags & SHCERT_CERT_LICENSE)
    strcat(ret_buf, "LICENSE ");

  if (flags & SHCERT_AUTH_WEB_CLIENT)
    strcat(ret_buf, "WEB-CLIENT-AUTH ");
  if (flags & SHCERT_AUTH_WEB_SERVER)
    strcat(ret_buf, "WEB-SERVER-AUTH ");
  if (flags & SHCERT_AUTH_FILE)
    strcat(ret_buf, "WEB-FILE ");

  if (*ret_buf)
    ret_buf[strlen(ret_buf)-1] = '\000';

  return (ret_buf);
}

void shcert_hex_print(shbuf_t *buff, unsigned char *data, size_t data_len, char *line_prefix)
{
  char text[256];
  unsigned int val;
  int i;

  if (!data_len)
    return;

  for (i = 0; i < data_len; i++) {
    if (i != 0) {
      shbuf_catstr(buff, ":");
      if (0 == (i % 22))
        shbuf_catstr(buff, "\n");
    }
    if (0 == (i % 22))
      shbuf_catstr(buff, line_prefix);

    val = (unsigned int)data[i];
    if (val || i < data_len - 3) {
      sprintf(text, "%-2.2x", val);
      shbuf_catstr(buff, text);
    }
  }
  shbuf_catstr(buff, "\n");

}
void shcert_hex_print_reverse(shbuf_t *buff, unsigned char *data, size_t data_len, char *line_prefix)
{
  char *text;
  int i, j;

  text = (char *)calloc(data_len, sizeof(char));
  if (!text)
    return;

  j = -1;
  for (i = data_len - 1; i >= 0; i--) {
    text[++j] = data[i];
  }

  shcert_hex_print(buff, text, data_len, line_prefix);
  free(text);

}

void shcert_print(shcert_t *cert, shbuf_t *pr_buff)
{
  char tbuf1[256];
  char tbuf2[256];
  char buf[4096];

  if (!cert || !pr_buff)
    return;

  shbuf_catstr(pr_buff, "Certificate:\n");
  shbuf_catstr(pr_buff, "  Data:\n");

  sprintf(buf, "    Version: %d\n", cert->cert_ver);
  shbuf_catstr(pr_buff, buf);

  shbuf_catstr(pr_buff, "    Serial Number: ");
  shcert_hex_print(pr_buff, cert->cert_ser, sizeof(cert->cert_ser), "");

  sprintf(buf, "  Signature Algorithm: %s\n", 
      shsig_alg_str(shcert_iss_alg(cert) | shcert_sub_alg(cert)));
  shbuf_catstr(pr_buff, buf);

  sprintf(buf, "    Issuer: %s\n", cert->cert_iss.ent_name);
  shbuf_catstr(pr_buff, buf);

  strcpy(tbuf1, shctime(shcert_sub_stamp(cert))+4);
  strcpy(tbuf2, shctime(shcert_sub_expire(cert))+4);
  sprintf(buf, "    Validity: %-20.20s - %-20.20s\n", tbuf1, tbuf2); 
  shbuf_catstr(pr_buff, buf);

  sprintf(buf, "    Subject: %s\n", cert->cert_sub.ent_name);
  shbuf_catstr(pr_buff, buf);

  sprintf(buf, "    Public Key Algorithm: (%d bit) %s\n",
      shcert_sub_len(cert) * 8, shsig_alg_str(shcert_sub_alg(cert)));
  shbuf_catstr(pr_buff, buf);

  sprintf(buf, "      Checksum: %llu\n", shkey_crc(shcert_sub_sig(cert)));
  sprintf(buf, "      192-Bit: %s\n", shkey_hex(shcert_sub_sig(cert)));
  shbuf_catstr(pr_buff, buf);

  if (shcert_sub_alg(cert) & SHKEY_ALG_RSA) {
    shbuf_catstr(pr_buff, "      Modulus:\n");
    shcert_hex_print_reverse(pr_buff, cert->cert_sub.ent_sig.key.rsa.mod, 
        cert->cert_sub.ent_sig.key.rsa.mod_len, "        ");
  }

  shbuf_catstr(pr_buff, "    X509v3 extensions:\n");
  sprintf(buf, "      Basic Constraints: CA=%s\n", 
      (cert->cert_flag & SHCERT_CERT_CHAIN) ? "false" : "true");
  shbuf_catstr(pr_buff, buf);

  if (!shpeer_localhost(&cert->cert_sub.ent_peer)) {
    sprintf(buf, "      Alternate Subject: %s\n", 
        shpeer_print(&cert->cert_sub.ent_peer));
    shbuf_catstr(pr_buff, buf);
  }

  sprintf(buf, "      Extended Usage: %s\n", shcert_flag_str(cert->cert_flag));
  shbuf_catstr(pr_buff, buf);

  sprintf(buf, "  Private Signature: %s (%d bytes)\n",
      shsig_alg_str(shcert_iss_alg(cert)), shcert_iss_len(cert));
  shbuf_catstr(pr_buff, buf);

  if (shcert_iss_alg(cert) & SHKEY_ALG_MD5) {
    shcert_hex_print(pr_buff, cert->cert_iss.ent_sig.key.md.md, 
        cert->cert_iss.ent_sig.key.md.md_len, "    ");
  } else if (shcert_iss_alg(cert) & SHKEY_ALG_SHA1) {
    shcert_hex_print(pr_buff, cert->cert_iss.ent_sig.key.sha.sha, 
        cert->cert_iss.ent_sig.key.sha.sha_len, "    ");
  } else if (shcert_iss_alg(cert) & SHKEY_ALG_SHA256) {
    shcert_hex_print(pr_buff, cert->cert_iss.ent_sig.key.sha.sha, 
        cert->cert_iss.ent_sig.key.sha.sha_len, "    ");
  } else {
    sprintf(buf, "    Checksum: %llu\n", shkey_crc(shcert_iss_sig(cert)));
    shbuf_catstr(pr_buff, buf);
    sprintf(buf, "    192-Bit: %s\n", shkey_hex(shcert_iss_sig(cert)));
    shbuf_catstr(pr_buff, buf);
  }


}


