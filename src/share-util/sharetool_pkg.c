
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
#include "bits.h"



int sharetool_package_list(char *pkg_name)
{
  shpeer_t *peer;
  shfs_dir_t *dir;
  shfs_dirent_t *ent;
  shfs_t *fs;
  char path[SHFS_PATH_MAX];

  peer = shpeer_init("package", NULL);
  fs = shfs_init(peer);
  shpeer_free(&peer);

  sprintf(path, "/sys/pkg/");
  dir = shfs_opendir(fs, path);
  if (!dir)
    return (0); /* nothing to list */

  while ((ent = shfs_readdir(dir))) {
    if (ent->d_type != SHINODE_DIRECTORY)
      continue;

    if (*pkg_name && 0 != fnmatch(pkg_name, ent->d_name, 0))
      continue;

    printf ("%s [%s]\n", ent->d_name, shcrcstr(ent->d_crc));
  }
  shfs_closedir(dir);

  return (0);
}

int sharetool_package_create(char *pkg_name, char *sig_fname)
{

  return (0);
}

int sharetool_certificate_create(char *sig_name, char *sig_fname)
{
  SHFL *file;
  shcert_t cert;
  shfs_t *fs;
  char type_str[64];
  char entity[MAX_SHARE_NAME_LENGTH]; 
  int flags;
  int err;

#if 0
  } else (*sig_fname) {
    /* load certificate from file */
    file = sharetool_file(sig_fname, &fs);
    if (!file)
      return (SHERR_INVAL);
#endif

  if (!*sig_fname) {
    /* generate certificate */
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

    memset(&cert, 0, sizeof(cert));
    err = shcert_init(&cert, entity, 0, flags);
    if (err)
      return (err);
  } else {
    /* load certificate from file */
    file = sharetool_file(sig_fname, &fs);
    if (!file)
      return (SHERR_INVAL);

    /* .. */

    shfs_free(&fs);
  }

  return (0);
}

int sharetool_package(char **args, int arg_cnt, int pflags)
{
  char pkg_name[MAX_SHARE_NAME_LENGTH];
  char sig_fname[SHFS_PATH_MAX];
  char pkg_cmd[256];
  int arg_of;
  int err;
  int i;

  if (arg_cnt <= 1)
    return (SHERR_INVAL);

  memset(sig_fname, 0, sizeof(sig_fname));

  arg_of = 0;
  for (i = 1; i < arg_cnt; i++) {
    if (args[i][0] == '-') {
      /* command argument */
      if (0 == strcmp(args[i], "-c") ||
          0 == strncmp(args[i], "--cert", 5)) {
        i++;
        if (i < arg_cnt)
          strncpy(sig_fname, args[i], sizeof(sig_fname)-1);
      }
      continue;
    }
  }

  memset(pkg_cmd, 0, sizeof(pkg_cmd));
  strncpy(pkg_cmd, args[1], sizeof(pkg_cmd)-1);

  memset(pkg_name, 0, sizeof(pkg_name));
  if (arg_cnt >= 3) {
    strncpy(pkg_name, args[2], sizeof(pkg_name)-1);
  }
fprintf(stderr, "DEBUG: share_package: cmd(%s) name(%s)\n", pkg_cmd, pkg_name);

  err = SHERR_INVAL;
  if (0 == strcasecmp(pkg_cmd, "list")) {
    err = sharetool_package_list(pkg_name);
  } else if (0 == strcasecmp(pkg_cmd, "install")) {
  } else if (0 == strcasecmp(pkg_cmd, "create")) {
    err = sharetool_package_create(pkg_name, sig_fname);
  } else if (0 == strcasecmp(pkg_cmd, "update")) {
  } else if (0 == strcasecmp(pkg_cmd, "remove")) {
  } else if (0 == strcasecmp(pkg_cmd, "list-cert")) {
  } else if (0 == strcasecmp(pkg_cmd, "cert")) {
    err = sharetool_certificate_create(pkg_name, sig_fname);
  }

  return (err);
}

