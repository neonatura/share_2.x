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



int share_file_cat(char *path, int pflags)
{
  shfs_t *tree;
  shfs_ino_t *file;
  shbuf_t *buf;
  char fpath[PATH_MAX+1];
  unsigned char *data;
  size_t data_len;
  int err;

  tree = shfs_init(NULL);
  if (!tree) {
    fprintf(stderr, "%s: %s\n", path, strerror(EIO));
    return;
  }

  file = shfs_file_find(tree, path);
  if (!file) {
    fprintf(stderr, "%s: %s\n", path, strerror(ENOENT));
    shfs_free(&tree);
    return;
  }

  err = shfs_file_read(file, &data, &data_len);
  if (err) {
    fprintf(stderr, "%d = shsf_file_read('%s')\n", err, path);
    shfs_free(&tree);
    return;
  }

/* TODO: may need to cycle through data for large writes to stdout */
  if (data)
    fwrite(data, sizeof(char), data_len, stdout);
  free(data);
#if 0
  if (file->blk.hdr.type == SHINODE_FILE) {
    buf = shbuf_init();
    shfs_inode_read(file, buf);
    if (buf->data_of)
      fwrite(buf->data, sizeof(char), buf->data_of, stdout);
/*
    if (buf->data_of)
      printf("%-*.*s", buf->data_of, buf->data_of, buf->data);
*/
    shbuf_free(&buf);
  }
#endif

  shfs_free(&tree);

}

