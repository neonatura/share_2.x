/*
 *  Copyright 2013 Brian Burrell 
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




int share_file_remove(char *path, int pflags)
{
  struct stat st;
  shfs_t *tree;
  shfs_ino_t *file;
  shbuf_t *buf;
  char fpath[PATH_MAX+1];
  char *data;
  size_t data_len;
  int err;

  if (!*path || 0 == strcmp(path, "/"))
    return (SHERR_ISDIR);

  tree = shfs_init(NULL);
  if (!tree) {
    err = SHERR_IO;
    perror("shfs_init");
    goto done;
  }

  file = shfs_file_find(tree, path);
  if (!file) {
    err = SHERR_NOENT;
    perror(path);
    goto done;
  }
  if (file->blk.hdr.type == SHINODE_DIRECTORY) {
    err = SHERR_ISDIR;
    goto done;
  }

  err = shfs_file_remove(file);
  if (err)
    goto done;

  err = 0;
  printf ("Removed inode:\n%s", shfs_inode_print(file));

done:
  shfs_free(&tree);

  return (err);
}

