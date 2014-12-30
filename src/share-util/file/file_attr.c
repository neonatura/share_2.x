
/*
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
 */  

#include "share.h"
#include "sharetool.h"

#define MOD_MINUS -1
#define MOD_NONE 0
#define MOD_PLUS 1

int share_file_attr(char *path, int pflags)
{
  struct stat st;
  shfs_ino_t *file;
  shfs_attr_t attr_flag;
  char attr[PATH_MAX+1];
  char *ptr;
  int mode;
  int idx;
  int err;
  int i;

fprintf(stderr, "DEBUG: share_file_attr '%s' pflags %d\n", path, pflags);


  memset(attr, 0, sizeof(attr));
  if (*path == '+' || *path == '-') {
    strncpy(attr, path, sizeof(attr)-1);
    ptr = strchr(attr, ' ');
    if (ptr) {
      *ptr++ = '\0'; 
      path = ptr;
    }
  }

  file = sharetool_file(path, &sharetool_fs);
  //file = shfs_file_find(sharetool_fs, path);
  if (!file) {
    fprintf(stderr, "%s: %s\n", path, strerror(ENOENT));
    return;
  }

  err = shfs_fstat(file, &st);
  if (err) {
    fprintf(stderr, "%s: cannot access %s: %s\n", 
        process_path, path, sherr_str(err));
    return (err);
  }

  printf("[%s]\n", shfs_inode_print(file));

  if (!*attr) {
    char buf[4096];

    memset(buf, 0, sizeof(buf));
    for (i = 0; i < strlen(SHFS_ATTR_BITS); i++) {
      if ((attr_flag & (1 << i) || (pflags & PFLAG_VERBOSE))) {
        printf("\t%s: %s\n", shfs_attr_label(i), 
            (attr_flag & (1 << i)) ? "True" : "False");
      }
    }

    return (0);
  }

  mode = MOD_NONE;
  attr_flag = shfs_attr(file);
  for (i = 0; i < strlen(attr); i++) {
    if (attr[i] == '-') {
      mode = MOD_MINUS;
      continue;
    }
    if (attr[i] == '+') {
      mode = MOD_PLUS;
      continue;
    }
  
    attr[i] = tolower(attr[i]);
    switch (mode) {
      case MOD_PLUS:
fprintf(stderr, "DEBUG: MODE_PLUS[%c]\n", attr[i]);
        break;
      case MOD_MINUS:
fprintf(stderr, "DEBUG: MODE_MINUS[%c]\n", attr[i]);
        break;
    }
  }

  shfs_free(&sharetool_fs);

  return (0);
}




