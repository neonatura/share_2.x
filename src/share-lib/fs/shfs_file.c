

/*
 * @copyright
 *
 *  Copyright 2013 Brian Burrell 
 *
 *  This file is part of the Share Library.
 *  (https://github.com/briburrell/share)
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

#include "share.h"


int shfs_file_write(shfs_ino_t *file, void *data, size_t data_len)
{
  shfs_ino_t *aux;
  shbuf_t *buff;
  int err;

  aux = shfs_inode(file, NULL, SHINODE_AUX);
  if (!aux)
    return (SHERR_IO);

  buff = shbuf_init();
  shbuf_cat(buff, data, data_len);
  err = shfs_inode_write(aux, buff);
  if (err)
    return (err);

  return (0);
}

int shfs_file_read(shfs_ino_t *file, void **data_p, size_t *data_len_p)
{
  int err;
  shfs_ino_t *aux;
  shbuf_t *buff;

 aux = shfs_inode(file, NULL, SHINODE_AUX);
  if (!aux)
    return (SHERR_IO);

  buff = shbuf_init();
  err = shfs_inode_read(aux, buff);
  if (err)
    return (err);

  if (data_p)
    *data_p = buff->data;
  if (data_len_p)
    *data_len_p = buff->data_of;
  free(buff);

  return (0);
}

shfs_ino_t *shfs_file_find(shfs_t *tree, char *path)
{
  shfs_ino_t *dir;
  shfs_ino_t *file;
  char fpath[PATH_MAX+1];
  char *ptr;

  file = NULL;

  memset(fpath, 0, sizeof(fpath));
  if (*path == '/') {
    dir = tree->base_ino;
    strncpy(fpath, path + 1, PATH_MAX);
  } else {
    dir = tree->cur_ino;
    strncpy(fpath, path, PATH_MAX);
  }

  if (!*fpath)
    return (dir);
  
  if (fpath[strlen(fpath)-1] == '/') {
    dir = shfs_dir_find(tree, fpath);
    return (dir);
  }

  ptr = strrchr(fpath, '/');
  if (!ptr) {
    file = shfs_inode(dir, fpath, SHINODE_FILE);
    return (file);
  } 

  *ptr++ = '\000';
  dir = shfs_dir_find(tree, fpath);
  file = shfs_inode(dir, ptr, SHINODE_FILE);

  return (file);
}


