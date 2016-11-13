
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

#include <stdio.h>
#include "share.h"
#undef fnmatch
#include "sharetool.h"
#include "x509.h"
#include "x509_crt.h"
#include "bits.h" /* share-daemon/bits */




int sharetool_cert_save(char *sig_name, shcert_t *cert)
{
  char path[SHFS_PATH_MAX];

  sprintf(path, "alias/%s", sig_name);
  return (shfs_cert_save(cert, path));

#if 0
  SHFL *file;
  shpeer_t *peer;
  shfs_t *fs;
  shbuf_t *buff;
  char path[SHFS_PATH_MAX];
  int err;

  /* store in sharefs sytem hierarchy of 'package' partition. */
  fs = shfs_sys_init(SHFS_DIR_CERTIFICATE, sig_name, &file);
  if (!fs)
    return (SHERR_IO);
/*
 * TODO: "install" or "apply" to existing file
  err = shfs_cert_apply(file, cert);
*/
  buff = shbuf_map((unsigned char *)cert, sizeof(shcert_t));
  err = shfs_write(file, buff);
  free(buff);
  if (err)
    return (err);
  
  shfs_free(&fs);
  if (err)
    return (err);

  return (0);
#endif
}

int sharetool_cert_load(char *sig_name, shcert_t **cert_p)
{
  shcert_t *cert;
  char path[SHFS_PATH_MAX];

  cert = shfs_cert_load(sig_name);
  if (!cert) {
    sprintf(path, "alias/%s", sig_name);
    cert = shfs_cert_load_ref(path);
    if (!cert) {
      return (SHERR_NOENT);
    }
  }

  *cert_p = cert;
  return (0);
} 

int sharetool_cert_list(char *cert_alias)
{
  shpeer_t *peer;
  shfs_dir_t *dir;
  shfs_dirent_t *ent;
  shfs_t *fs;
  char path[SHFS_PATH_MAX];

  fs = shfs_sys_init(NULL, NULL, NULL);
  if (!fs)
    return (SHERR_IO);

  if (!cert_alias)
    cert_alias = "";

  printf ("Certificates:\n");
  dir = shfs_opendir(fs, shfs_sys_dir(SHFS_DIR_CERTIFICATE, cert_alias));
  if (dir) {
    while ((ent = shfs_readdir(dir))) {
      if (ent->d_type != SHINODE_FILE)
        continue;

#if 0
      if (*cert_alias && 0 != fnmatch(cert_alias, ent->d_name, 0))
        continue;
#endif

      printf ("%s [CRC:%s]\n", ent->d_name, shcrcstr(ent->d_crc));
    }
    shfs_closedir(dir);
  }

  shfs_free(&fs);
  return (0);
}

int sharetool_cert_import(char *sig_name, char *parent_name, char *sig_fname)
{
  struct stat st;
  x509_crt *chain;
  shcert_t *cert;
  shcert_t *p_cert;
  shbuf_t *buff;
  shfs_t *fs;
  SHFL *file;
  int err;

  fs = shfs_uri_init(sig_fname, 0, &file);
  if (!fs)
    return (SHERR_NOENT);

  err = shfs_fstat(file, &st);
  if (err) {
    return (err);
  }

  /* read certificate file */
  buff = shbuf_init();
  err = shfs_read(file, buff);
  shfs_free(&fs);
  if (err) {
    printf ("ERROR: %d = shfs_read()\n", err);
    shbuf_free(&buff);
    return (err);
  }

  if (shbuf_size(buff) == 0) {
    return (SHERR_INVAL);
  }

  x509_pem_decode(buff);
  if (shbuf_size(buff) == 0) {
    return (SHERR_INVAL);
  }

  /* parse certificate */
  chain = (x509_crt *)calloc(1, sizeof(x509_crt));
  err = x509_cert_parse(chain, shbuf_data(buff), shbuf_size(buff));
  if (err) {
    free(chain);
    shbuf_free(&buff);
    return (err);
  }

  err = x509_cert_extract(chain, &cert);
  free(chain);
  shbuf_free(&buff);
  if (err)
    return (err);

  if (parent_name && *parent_name) {
    err = sharetool_cert_load(parent_name, &p_cert);
    if (err)
      return (err);

    err = shcert_sign(cert, p_cert);
    free(p_cert);
    if (err)
      return (err);
  }

#if 0
  /* generate a print-out of certificate's underlying info. */
  buff = shbuf_init();
  shcert_print(cert, buff);
  fprintf(sharetool_fout, "%s", shbuf_data(buff));
  shbuf_free(&buff);
#endif

  err = sharetool_cert_save(sig_name, cert);
  free(cert);
  if (err)
    return (err);

  printf("%s: Imported certificate '%s'.\n", process_path, sig_name);

  return (0);
}

/**
 * Create a new certificate.
 * @param sig_fname Specifies the certificate alias.
 * @param sig_fname Specifies a parent certificate alias, or NULL if not applicable.
 */
int sharetool_cert_create(char *sig_name, char *parent_name)
{
  SHFL *file;
  shcert_t *p_cert;
  shcert_t cert;
  shfs_t *fs;
  char path[SHFS_PATH_MAX];
  char type_str[64];
  char entity[MAX_SHARE_NAME_LENGTH]; 
  int flags;
  int err;

  /* generate certificate */
  memset(&cert, 0, sizeof(cert));
  flags = 0;
  printf ("Enter certificate type (O=Org/C=Com/P=Person): ");
  fflush(stdout);
  memset(type_str, 0, sizeof(type_str));
  fgets(type_str, MAX_SHARE_NAME_LENGTH-1, stdin);
  switch (tolower(type_str[0])) {
    case 'o': flags |= SHCERT_ENT_ORGANIZATION; break;
    case 'c': flags |= SHCERT_ENT_COMPANY; break;
    case 'p': flags |= SHCERT_ENT_INDIVIDUAL; break;
  }

  printf ("Enter the entity name (real/company name): ");
  fflush(stdout);
  memset(entity, 0, sizeof(entity));
  fgets(entity, MAX_SHARE_NAME_LENGTH-1, stdin);

  err = shcert_init(&cert, entity, 0, SHKEY_ALG_DEFAULT, flags);
  if (err)
    return (err);

  if (parent_name && *parent_name) {
    err = sharetool_cert_load(parent_name, &p_cert);
    if (err)
      return (err);
  
    err = shcert_sign(&cert, p_cert);
    free(p_cert);
    if (err)
      return (err);
  }

  return (0);
}

int sharetool_cert_remove(char *sig_name)
{
  struct stat st;
  SHFL *file;
  shfs_t *fs;
  shbuf_t *buff;
  shcert_t *cert;
  int err;

  /* store in sharefs sytem hierarchy of 'package' partition. */
  fs = shfs_sys_init(SHFS_DIR_CERTIFICATE, sig_name, &file);
  if (!fs)
    return (SHERR_IO);

  err = shfs_fstat(file, &st);
  if (err) {
    shfs_free(&fs);
    return (err);
  }

  err = shfs_file_remove(file);
  shfs_free(&fs);
  if (err)
    return (err);

  printf("%s: Removed certificate '%s'.\n", process_path, sig_name);

  return (0);
}

int sharetool_cert_verify(char *cert_alias, char *parent_alias)
{
  shcert_t *cert;
  shcert_t *pcert;
  int valid;
  int ret_err;
  int err;

  /* load the certificate from the system hierarchy. */
  err = sharetool_cert_load(cert_alias, &cert);
  if (err)
    return (err);

  valid = TRUE;
  pcert = NULL;
  if ((cert->cert_flag & SHCERT_CERT_CHAIN)) {
    /* load the certificate from the system hierarchy. */
    err = sharetool_cert_load(parent_alias, &pcert);
    if (err)
      return (err);
  }

  ret_err = shcert_verify(cert, pcert);
  if (ret_err == 0) {
    /* successful verification -- return something parseable. */
    fprintf(sharetool_fout, "Public Key: %s\n", shkey_hex(shcert_sub_sig(cert)));
  }

  shcert_free(&pcert);
  shcert_free(&cert);

  return (ret_err);
}

int sharetool_cert_print(char *cert_alias)
{
  shcert_t *cert;
  shbuf_t *buff;
  int err;

  /* load the certificate from the system hierarchy. */
  err = sharetool_cert_load(cert_alias, &cert);
  if (err)
    return (err);

  /* generate a print-out of certificate's underlying info. */
  buff = shbuf_init();
  shcert_print(cert, buff);
  free(cert);

  /* flush data to output file pointer */
  fprintf(sharetool_fout, "%s", shbuf_data(buff));
shbuf_free(&buff);

  return (0);
}

int sharetool_cert_print_file(char *sig_name, char *sig_fname)
{
  struct stat st;
  x509_crt *chain;
  shbuf_t *buff;
  shfs_t *fs;
  SHFL *file;
  int err;

  fs = shfs_uri_init(sig_fname, 0, &file);
  if (!fs)
    return (SHERR_NOENT);

  err = shfs_fstat(file, &st);
  if (err) {
    fprintf(stderr, "ERROR: fstat '%s': %s\n", sig_fname, sherrstr(err));
    return (err);
  }

  /* read certificate file */
  buff = shbuf_init();
  err = shfs_read(file, buff);
  shfs_free(&fs);
  if (err) {
    printf ("ERROR: %d = shfs_read()\n", err);
    shbuf_free(&buff);
    return (err);
  }

  if (shbuf_size(buff) == 0) {
    return (SHERR_INVAL);
  }

  x509_pem_decode(buff);
  if (shbuf_size(buff) == 0) {
    return (SHERR_INVAL);
  }

  /* parse certificate */
  chain = (x509_crt *)calloc(1, sizeof(x509_crt));
  err = x509_cert_parse(chain, shbuf_data(buff), shbuf_size(buff));
  if (err) {
    free(chain);
    shbuf_free(&buff);
    return (err);
  }

  /* print contents. */
  sharetool_cert_print_crt(chain);
  shbuf_free(&buff);

  free(chain);
  return (0);
}

int sharetool_certificate(char **args, int arg_cnt, int pflags)
{
  char cert_alias[MAX_SHARE_NAME_LENGTH];
  char parent_alias[MAX_SHARE_NAME_LENGTH];
  char x509_fname[SHFS_PATH_MAX];
  char cert_cmd[256];
  int err;
  int i;

  if (arg_cnt <= 1)
    return (SHERR_INVAL);

  memset(x509_fname, 0, sizeof(x509_fname));
  memset(parent_alias, 0, sizeof(parent_alias));
  memset(cert_cmd, 0, sizeof(cert_cmd));
  memset(cert_alias, 0, sizeof(cert_alias));

  for (i = 1; i < arg_cnt; i++) {
    if (args[i][0] == '-') {
      /* command argument */
      if (0 == strcmp(args[i], "-c") ||
          0 == strncmp(args[i], "--cert", 5)) {
        i++;
        if (i < arg_cnt)
          strncpy(x509_fname, args[i], sizeof(x509_fname)-1);
      }
      continue;
    }
    if (!*cert_cmd) {
      strncpy(cert_cmd, args[i], sizeof(cert_cmd)-1);
    } else if (!*cert_alias) {
      strncpy(cert_alias, args[i], sizeof(cert_alias)-1);
    } else if (!*parent_alias) {
      strncpy(parent_alias, args[i], sizeof(parent_alias)-1);
    }
  }


  err = SHERR_INVAL;
  if (0 == strcasecmp(cert_cmd, "list")) {
    err = sharetool_cert_list(cert_alias);
  } else if (0 == strcasecmp(cert_cmd, "create")) {
    if (!*x509_fname) {
      err = sharetool_cert_create(cert_alias, parent_alias);
    } else {
      err = sharetool_cert_import(cert_alias, parent_alias, x509_fname);
    }
  } else if (0 == strcasecmp(cert_cmd, "remove")) {
    err = sharetool_cert_remove(cert_alias);
  } else if (0 == strcasecmp(cert_cmd, "verify")) {
    err = sharetool_cert_verify(cert_alias, parent_alias);
  } else if (0 == strcasecmp(cert_cmd, "print")) {
    if (!*x509_fname) {
      err = sharetool_cert_print(cert_alias);
    } else {
      /* print a X509 certificate stored in a file. */
      err = sharetool_cert_print_file(cert_alias, x509_fname);
    }
  }

  return (err);
}

