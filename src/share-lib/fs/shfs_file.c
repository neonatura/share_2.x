

/*
 * @copyright
 *
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
 *
 *  @endcopyright
 */

#include "share.h"


int shfs_file_write(shfs_ino_t *file, void *data, size_t data_len)
{
  shfs_ino_t *aux;
  shbuf_t *buff;
  int err;

  aux = shfs_inode(file, NULL, SHINODE_BINARY);
  if (!aux)
    return (SHERR_IO);

  if (!data || data_len == 0)
    return (0); 

  buff = shbuf_init();
  shbuf_cat(buff, data, data_len);
  err = shfs_aux_write(aux, buff);
  shbuf_free(&buff);
  if (err)
    return (err);

  /* copy aux stats to file inode. */
  file->blk.hdr.mtime = aux->blk.hdr.mtime;
  file->blk.hdr.size = aux->blk.hdr.size;
  file->blk.hdr.crc = aux->blk.hdr.crc;
  err = shfs_inode_write_entity(file);
  if (err) {
    PRINT_RUSAGE("shfs_inode_write: error writing entity.");
    return (err);
  }

  return (0);
}


int shfs_file_read(shfs_ino_t *file, unsigned char **data_p, size_t *data_len_p)
{
  int err;
  shfs_ino_t *aux;
  shbuf_t *buff;

	if (file == NULL)
return (SHERR_NOENT);

 aux = shfs_inode(file, NULL, SHINODE_BINARY);
  if (!aux)
    return (SHERR_IO);

  buff = shbuf_init();
  err = shfs_aux_read(aux, buff);
  if (err)
    return (err);

  if (data_p)
    *data_p = buff->data;
  if (data_len_p)
    *data_len_p = buff->data_of;
  free(buff);

  return (0);
}

struct test_shfs_t {
  int val;
  char str[16];
}; 
/**
 * fail case: write 10239, then write 8192, and strlen == 10239.
 */
_TEST(shfs_file_read)
{
  struct test_shfs_t *ar;
  shfs_t *tree;
  shfs_ino_t *fl;
  char binbuf[4096];
  char buf[4096];
  char *data;
  char *bin_data;
  size_t data_len;
  size_t bin_data_len;
  size_t block_len;
  int test_idx;
  int val;
  int i;

  /* ensure multiple writes reflect content change. */
  for (test_idx = 0; test_idx < 3; test_idx++) {
    memset(buf, 0, sizeof(buf));
    memset(buf, '0', 2048);

    block_len = sizeof(binbuf) / sizeof(struct test_shfs_t);
    memset(binbuf, 0, sizeof(binbuf));
    ar = (struct test_shfs_t *)binbuf;
    for (i = 0; i < block_len; i++) {
      val = (test_idx + i);
      ar[i].val = val;
      sprintf(ar[i].str, "%d", val);
    } 

    tree = shfs_init(shpeer());
    _TRUEPTR(tree);
    fl = shfs_file_find(tree, "/test/test"); 
    _TRUE(0 == shfs_file_write(fl, buf, sizeof(buf)));
    fl = shfs_file_find(tree, "/test/test.bin"); 
    _TRUE(0 == shfs_file_write(fl, binbuf, sizeof(binbuf)));
    shfs_free(&tree);

    tree = shfs_init(shpeer());
    _TRUEPTR(tree);
    fl = shfs_file_find(tree, "/test/test"); 
    _TRUE(0 == shfs_file_read(fl, &data, &data_len));
    _TRUEPTR(data);
    fl = shfs_file_find(tree, "/test/test.bin"); 
    _TRUE(0 == shfs_file_read(fl, &bin_data, &bin_data_len));
    _TRUEPTR(bin_data);
    shfs_free(&tree);

    _TRUE(data_len == sizeof(buf));
    _TRUE(strlen(data) == 2048);
    _TRUE(0 == memcmp(buf, data, sizeof(buf)));
    free(data);

    _TRUE(bin_data_len == sizeof(binbuf));
    ar = (struct test_shfs_t *)bin_data;
    for (i = 0; i < block_len; i++) {
      val = (test_idx + i);
      _TRUE(val == ar[i].val);
      _TRUE(val == atoi(ar[i].str));
    } 
    free(bin_data);
  }

}

int shfs_file_pipe(shfs_ino_t *file, int fd)
{
  shfs_ino_t *aux;
  int err;

  if (file == NULL)
    return (SHERR_NOENT);

  aux = shfs_inode(file, NULL, SHINODE_BINARY);
  if (!aux)
    return (SHERR_IO);

  err = shfs_aux_pipe(aux, fd);
  if (err)
    return (err);

  return (0);
}

shfs_ino_t *shfs_file_find(shfs_t *tree, char *path)
{
  shfs_ino_t *dir;
  shfs_ino_t *file;
  char fpath[PATH_MAX+1];
  char *ptr;

  file = NULL;

  if (!path || !*path)
    return (NULL);

  memset(fpath, 0, sizeof(fpath));
  if (*path == '/') {
    strncpy(fpath, path + 1, PATH_MAX);
  } else {
    strncpy(fpath, path, PATH_MAX);
  }
#if 0
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
#endif
  
  if (!*fpath || fpath[strlen(fpath)-1] == '/') {
    dir = shfs_dir_find(tree, fpath);
    return (dir);
  }

  ptr = strrchr(fpath, '/');
  if (!ptr) {
    file = shfs_inode(tree->cur_ino, fpath, SHINODE_FILE);
    return (file);
  }

  *ptr++ = '\000';
  dir = shfs_dir_find(tree, fpath);
  if (!dir) {
    return (NULL);
  }

  file = shfs_inode(dir, ptr, SHINODE_FILE);

  return (file);
}


int shfs_file_remove(shfs_ino_t *file)
{
  shfs_ino_t *aux;
  shbuf_t *buff;
  int err;

  aux = shfs_inode(file, NULL, SHINODE_BINARY);
  if (!aux)
    return (SHERR_IO);

  err = shfs_inode_clear(aux);
  if (err)
    return (err);

  /* reset stats on file inode. */
  file->blk.hdr.mtime = 0;
  file->blk.hdr.size = 0;
  file->blk.hdr.type = SHINODE_NULL;
  err = shfs_inode_write_entity(file);
  if (err) {
    PRINT_RUSAGE("shfs_inode_write: error writing entity.");
    return (err);
  }

  return (0);
}

