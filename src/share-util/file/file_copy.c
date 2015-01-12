
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
#include "sharetool.h"


int share_file_copy(char **args, int arg_cnt, int pflags)
{
  struct stat st;
  shfs_t *dest_fs;
  shfs_t **src_fs;
  shfs_ino_t *dest_file;
  shfs_ino_t **src_file;
  shbuf_t *buff;
  char fpath[PATH_MAX+1];
  unsigned char *data;
  size_t data_len;
  size_t of;
  int src_cnt;
  int w_len;
  int err;
  int i;

  if (arg_cnt < 1)
    return (SHERR_INVAL);

  src_file = NULL;
  src_fs = NULL;

  arg_cnt--;
  src_cnt = 0;
  if (!arg_cnt) {
    /* create faux substitute. */
/*DEBUG: */return (SHERR_INVAL);
  } else {
    dest_file = sharetool_file(args[arg_cnt], &dest_fs);

    src_file = (shfs_ino_t **)calloc(arg_cnt + 1, sizeof(shfs_ino_t *));
    src_fs = (shfs_t **)calloc(arg_cnt + 1, sizeof(shfs_t *));

    for (i = 1; i < arg_cnt; i++) {
      src_file[src_cnt] = sharetool_file(args[i], &src_fs[src_cnt]);
      err = shfs_fstat(src_file[src_cnt], &st);
      src_cnt++;
      if (err)
        goto done;
    }
  }

  for (i = 0; i < src_cnt; i++) {
    shfs_t *s_fs = src_fs[i];
    err = shfs_file_copy(src_file[i], dest_file);
    shfs_free(&s_fs);
    if (err)
      goto done;
  }

  err = 0;
done:
  if (src_file) free(src_file);
  if (src_fs) free(src_fs);

  return (err);
}

