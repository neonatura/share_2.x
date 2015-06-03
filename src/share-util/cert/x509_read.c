
#include "share.h"
#include "sharetool.h"
#include "asn1.h"
#include "x509.h"
#include "x509_crt.h"

int x509_cert_parse(x509_crt *crt, unsigned char *buf, size_t buflen)
{
  x509_buf sig_params1;
  x509_buf sig_params2;
  unsigned char *p, *end, *crt_end;
  size_t len;
  int err;

int i;
for (i = 0; i < 16; i++) {
printf ("x509_cert_parse: IDX#%-2.2d: '%c' (%d)\n", i, buf[i], buf[i]);
}

  memset( &sig_params1, 0, sizeof( x509_buf ) );
  memset( &sig_params2, 0, sizeof( x509_buf ) );

  p = buf;
  len = buflen;

  crt->raw.p = p;
  crt->raw.len = len;
  end = p + len;

  /*
   * Certificate  ::=  SEQUENCE  {
   *      tbsCertificate       TBSCertificate,
   *      signatureAlgorithm   AlgorithmIdentifier,
   *      signatureValue       BIT STRING  }
   */
  err = asn1_get_tag(&p, end, &len, ASN1_CONSTRUCTED | ASN1_SEQUENCE);
  if (err) { 
    return (err);
  }

  if( len > (size_t) ( end - p ) ) {
    return (SHERR_INVAL);
  }

  crt_end = p + len;

  /*
   * TBSCertificate  ::=  SEQUENCE [ 
   */
  crt->tbs.p = p;
  err = asn1_get_tag(&p, end, &len, ASN1_CONSTRUCTED | ASN1_SEQUENCE);
  if (err) {
    return (err);
}

  end = p + len;
  crt->tbs.len = end - crt->tbs.p;

  /*
   * Version  ::=  INTEGER  {  v1(0), v2(1), v3(2)  }
   *
   * CertificateSerialNumber  ::=  INTEGER
   *
   * signature            AlgorithmIdentifier
   */
fprintf(stderr, "DEBUG: version offset: %d, %x %x %x %x %x %x\n", (buf - p), p[0], p[1], p[2], p[3], p[4], p[5]);
  if( ( err = x509_get_version(  &p, end, &crt->version  ) ) != 0) {
    return (SHERR_INVAL);
  }
fprintf(stderr, "DEBUG: serial offset: %d, %x %x %x %x %x %x\n", (buf - p), p[0], p[1], p[2], p[3], p[4], p[5]);
  if (( err = x509_get_serial(   &p, end, &crt->serial   ) ) != 0 ||
      ( err = x509_get_alg(      &p, end, &crt->sig_oid1,
                                 &sig_params1 ) ) != 0 ) {
    return (SHERR_INVAL);
  }

  crt->version++;

  if (crt->version > 3) {
    return (SHERR_OPNOTSUPP);
  }

  if( ( err = x509_get_sig_alg( &crt->sig_oid1, &sig_params1,
          &crt->sig_md, &crt->sig_pk,
          &crt->sig_opts ) ) != 0 ) {
    return (SHERR_INVAL);
  }

  /*
   * issuer               Name
   */
  crt->issuer_raw.p = p;

  if( ( err = asn1_get_tag( &p, end, &len,
          ASN1_CONSTRUCTED | ASN1_SEQUENCE ) ) != 0 ) {
    return (SHERR_INVAL);
  }

  if( ( err = x509_get_name( &p, p + len, &crt->issuer ) ) != 0 ) {
    return (SHERR_INVAL);
  }

  crt->issuer_raw.len = p - crt->issuer_raw.p;

  /*
   * Validity ::= SEQUENCE {
   *      notBefore      Time,
   *      notAfter       Time }
   *
   */
  if( ( err = x509_get_dates( &p, end, &crt->valid_from,
          &crt->valid_to ) ) != 0 ) {
    return (SHERR_INVAL);
  }

  /*
   * subject              Name
   */
  crt->subject_raw.p = p;

  if( ( err = asn1_get_tag( &p, end, &len,
          ASN1_CONSTRUCTED | ASN1_SEQUENCE ) ) != 0 ) {
    return (SHERR_INVAL);
  }

  if( len && ( err = x509_get_name( &p, p + len, &crt->subject ) ) != 0 )
  {
    return (SHERR_INVAL);
  }

  crt->subject_raw.len = p - crt->subject_raw.p;

  /*
   * SubjectPublicKeyInfo
   */
  if( ( err = pk_parse_subpubkey( &p, end, &crt->pk ) ) != 0 ) {
    return (SHERR_INVAL);
  }

  /*
   *  issuerUniqueID  [1]  IMPLICIT UniqueIdentifier OPTIONAL,
   *                       -- If present, version shall be v2 or v3
   *  subjectUniqueID [2]  IMPLICIT UniqueIdentifier OPTIONAL,
   *                       -- If present, version shall be v2 or v3
   *  extensions      [3]  EXPLICIT Extensions OPTIONAL
   *                       -- If present, version shall be v3
   */
  if( crt->version == 2 || crt->version == 3 ) {
    err = x509_get_uid( &p, end, &crt->issuer_id,  1 );
    if (err) {
      return (SHERR_INVAL);
  }
  }

  if( crt->version == 2 || crt->version == 3 )
  {
    err = x509_get_uid( &p, end, &crt->subject_id,  2 );
    if (err) {
      return (SHERR_INVAL);
    }
  }
 
  if( crt->version == 3 ) {
     err = x509_get_crt_ext( &p, end, crt ); 
  if (err) {
return (SHERR_INVAL);
}
  }

  if( p != end ) {
    return (SHERR_INVAL);
}

  end = crt_end;

  /*
   *  ]
   *  -- end of TBSCertificate
   *
   *  signatureAlgorithm   AlgorithmIdentifier,
   *  signatureValue       BIT STRING
   */
  if( ( err = x509_get_alg( &p, end, &crt->sig_oid2, &sig_params2 ) ) != 0 ) {
    return (SHERR_INVAL);
  }

  if( crt->sig_oid1.len != crt->sig_oid2.len ||
      memcmp( crt->sig_oid1.p, crt->sig_oid2.p, crt->sig_oid1.len ) != 0 ||
      sig_params1.len != sig_params2.len ||
      ( sig_params1.len != 0 &&
        memcmp( sig_params1.p, sig_params2.p, sig_params1.len ) != 0 ) )
  {
    return (SHERR_INVAL);
  }
 
  if( ( err = x509_get_sig( &p, end, &crt->sig ) ) != 0 ) {
    return (SHERR_INVAL);
  }


  if( p != end ) {
    return (SHERR_INVAL);
  }


  return (0);
}

void x509_pem_decode(shbuf_t *buff)
{
  shbuf_t *enc_buff;
  char *text;
  char *ptr;
  char *e_ptr;
  unsigned char *data;
  char *tok;
  size_t data_len;
  int cert_ok;
  int err;


  /* clone & clear mem buffer. */
  text = strdup(shbuf_data(buff));

  cert_ok = FALSE;
  enc_buff = shbuf_init();
  tok = strtok(text, "\r\n");
  while (tok) {
    if (0 == strcmp(tok, "-----END CERTIFICATE-----")) {
      break;
    }
    if (0 == strcmp(tok, "-----BEGIN CERTIFICATE-----")) {
      cert_ok = TRUE;
    } else if (cert_ok) {
      shbuf_cat(enc_buff, tok, strlen(tok));
    }

    tok = strtok(NULL, "\r\n");
  }
  if (!shbuf_size(enc_buff)) {
    return (SHERR_INVAL);
}
  
  data_len = shbase64_decode_len(shbuf_data(enc_buff));
  err = shbase64_decode(shbuf_data(enc_buff), &data, &data_len);
  shbuf_free(&enc_buff);
  if (err) {
    printf ("ERROR: %d = shbase64_decode()\n", err);
    return (err);
  }

  shbuf_clear(buff);
  shbuf_cat(buff, data, data_len); 
  free(data);

  return (0);
}


int x509_cert_extract(x509_crt *chain, shcert_t **cert_p)
{
  struct tm tm;
  shpeer_t *peer;
  shcert_t *cert;
  shkey_t *key;

  cert = (shcert_t *)calloc(1, sizeof(shcert_t));
  if (!cert)
    return (SHERR_NOMEM);

  cert->cert_ver = chain->version;

  /* fill serial number (verbatim 128-bit) from x509 certificate. */

  memcpy(cert->cert_ser, chain->serial.p+1,
      MIN(sizeof(cert->cert_ser), chain->serial.len-1));

  if (!chain->ca_istrue) {
    cert->cert_flag |= SHCERT_CERT_CHAIN;
  }

  x509_dn_gets(cert->cert_sub.ent_name,
      sizeof(cert->cert_iss.ent_name)-1, &chain->subject );
  x509_dn_gets(cert->cert_iss.ent_name,
      sizeof(cert->cert_iss.ent_name)-1, &chain->issuer );

  if (chain->sig_pk == POLARSSL_PK_RSA) {
    shcert_sub_alg(cert) = SHKEY_ALG_RSA; 
}

  memset(&tm, 0, sizeof(tm));
  tm.tm_year = chain->valid_from.year - 1900;
  tm.tm_mon = chain->valid_from.mon;
  tm.tm_mday = chain->valid_from.day;
  tm.tm_hour = chain->valid_from.min;
  tm.tm_min = chain->valid_from.min;
  tm.tm_sec = chain->valid_from.sec;
  cert->cert_sub.ent_sig.sig_stamp = shmktime(&tm);

  memset(&tm, 0, sizeof(tm));
  tm.tm_year = chain->valid_to.year - 1900;
  tm.tm_mon = chain->valid_to.mon;
  tm.tm_mday = chain->valid_to.day;
  tm.tm_hour = chain->valid_to.min;
  tm.tm_min = chain->valid_to.min;
  tm.tm_sec = chain->valid_to.sec;
  cert->cert_sub.ent_sig.sig_expire = shmktime(&tm);

  if( chain->ext_types & EXT_KEY_USAGE ) {
    if (chain->key_usage & KU_DIGITAL_SIGNATURE) {
      cert->cert_flag |= SHCERT_CERT_DIGITAL;
    } else if (chain->key_usage & KU_KEY_CERT_SIGN) {
      cert->cert_flag |= SHCERT_CERT_SIGN;
    } else if (chain->key_usage & KU_KEY_AGREEMENT) {
      cert->cert_flag |= SHCERT_CERT_KEY;
    } else if (chain->key_usage & KU_KEY_ENCIPHERMENT) {
      cert->cert_flag |= SHCERT_CERT_ENCIPHER;
    }
  }

  if( chain->ext_types & EXT_EXTENDED_KEY_USAGE ) {
    const x509_sequence *cur = &chain->ext_key_usage;
    while (cur) {
      char *desc;

      oid_get_extended_key_usage( &cur->buf, &desc );

      if (0 == strcmp(desc, "TLS Web Client Authentication")) {
        cert->cert_flag |= SHCERT_AUTH_WEB_CLIENT;
      }
      if (0 == strcmp(desc, "TLS Web Server Authentication")) {
        cert->cert_flag |= SHCERT_AUTH_WEB_SERVER;
      }

      cur = cur->next;
    }
  }

  if (chain->ext_types & EXT_SUBJECT_ALT_NAME) {
    size_t i;
    char buf[1024];
    size_t n = sizeof(buf)-1;
    char *p;
    const x509_sequence *cur = &chain->subject_alt_names;
    const char *sep = "";
    size_t sep_len = 0;

    while( cur != NULL )
    {
      memset(buf, 0, sizeof(buf));
      p= buf;

      if( cur->buf.len + sep_len >= n )
      {
        *p = '\0';
        break;
      }

      n -= cur->buf.len + sep_len;
      for( i = 0; i < cur->buf.len; i++ )
        *p++ = cur->buf.p[i];

      if (shnet_gethostbyname(buf)) {
        /* found suitable hostname */
        if (cert->cert_flag & SHCERT_AUTH_WEB_SERVER)
          strcat(buf, ":443");
        peer = shpeer_init("ssl", buf); 
        memcpy(&cert->cert_sub.ent_peer,
            peer, sizeof(cert->cert_sub.ent_peer));
        shpeer_free(&peer);
        break;
      }

      cur = cur->next;
    }
  }


  /* public key - generated by subject. */
  shcert_sub_len(cert) = pk_get_size(&chain->pk)/8;
  if (chain->sig_pk == POLARSSL_PK_RSA) {
    shrsa_context *rsa;
    shbuf_t *buff;

    rsa = pk_rsa(chain->pk);

    buff = shbuf_init();
    shbuf_cat(buff, rsa->N.p, rsa->N.n*sizeof(t_uint));
    shbuf_cat(buff, rsa->E.p, rsa->E.n*sizeof(t_uint));

    key = shkey_bin(shbuf_data(buff), shbuf_size(buff));
    memcpy(shcert_sub_sig(cert), key, sizeof(shkey_t));
    shkey_free(&key);
    shcert_sub_alg(cert) = SHKEY_ALG_RSA;

    cert->cert_sub.ent_sig.key.rsa.mod_len = rsa->N.n * sizeof(t_uint);
    memcpy(cert->cert_sub.ent_sig.key.rsa.mod, 
        rsa->N.p, cert->cert_sub.ent_sig.key.rsa.mod_len);
    if (rsa->E.n == 1) {
      cert->cert_sub.ent_sig.key.rsa.exp = *rsa->E.p;
    }
fprintf(stderr, "DEBUG: x509_cert_extract: len %d\n", cert->cert_sub.ent_sig.key.rsa.mod_len); 
fprintf(stderr, "DEBUG: x509_cert_extract: exp %llu\n", cert->cert_sub.ent_sig.key.rsa.exp); 

    shbuf_free(&buff);
  }

  /* signature key - parent's public key encrypted with subject's local key */
  shcert_iss_len(cert) = chain->sig.len;
  key = shkey_bin(chain->sig.p, shcert_iss_len(cert));
  memcpy(shcert_iss_sig(cert), key, sizeof(shkey_t));
  shkey_free(&key);
  if (chain->sig_md == MD_MD5)
    shcert_iss_alg(cert) = SHKEY_ALG_MD5;
  else if (chain->sig_md == MD_SHA1) {
    shcert_iss_alg(cert) = SHKEY_ALG_SHA1;
    
    cert->cert_iss.ent_sig.key.sha.sha_len = chain->sig.len;
    memcpy(cert->cert_iss.ent_sig.key.sha.sha,
      chain->sig.p, cert->cert_iss.ent_sig.key.sha.sha_len);
  } else if (chain->sig_md == MD_SHA256)
    shcert_iss_alg(cert) = SHKEY_ALG_SHA256;
  else if (chain->sig_md == MD_SHA512)
    shcert_iss_alg(cert) = SHKEY_ALG_SHA512;

  *cert_p = cert;

  return (0);
}


int x509_cert_insert(shcert_t cert, x509_crt **chain_p)
{

  return (0);
}
