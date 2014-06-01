/*
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
 */  

#include "share.h"
#include "sharetool.h"

void share_file_list(char *path)
{
  shfs_t *tree;
  shfs_ino_t *dir;
  shfs_ino_t *file;
  char *ptr;

  tree = shfs_init(NULL);
  if (!tree) {
    perror("shfs_init");
    return;
  }

  dir = shfs_dir_find(tree, path);


#if 0
  dir = tree->cur_ino;
  file = NULL;
  if (*path) {
    if (path[strlen(path)-1] == '/') {
      dir = shfs_dir_find(tree, path);
    } else {
      ptr = strrchr(path, '/');
      if (!ptr) {
        file = shfs_inode(dir, path, SHINODE_FILE); 
      } else {
        *ptr++ = '\000';
        dir = shfs_dir_find(tree, path);
        file = shfs_inode(dir, ptr, SHINODE_FILE);
      }
    }
  }
#endif


  if (file) {
    printf ("%s\n", shfs_inode_print(file));
  }

  shfs_free(&tree);
}

void share_file_import(char *path)
{
  shfs_t *tree;
  shfs_ino_t *file;
  shbuf_t *buf;
  char fpath[PATH_MAX+1];
  char *data;
  size_t data_len;
  int err;

  err = shfs_read_mem(path, &data, &data_len);
  if (err) {
    perror(path);
    return;
  }

  tree = shfs_init(NULL);
  if (!tree) {
    perror("shfs_init");
    return;
  }

  file = shfs_file_find(tree, path);
  if (!file) {
    perror(path);
    return;
  }

  if (file->blk.hdr.type != SHINODE_FILE) {
    if (!IS_INODE_CONTAINER(file->blk.hdr.type)) {
      fprintf(stderr, "%s: %s\n", path, strerror(ENOTDIR));
      return;
    }
    memset(fpath, 0, sizeof(fpath));
    strncpy(fpath, path, PATH_MAX);
    file = shfs_inode(file, basename(fpath), SHINODE_FILE);
    if (!file) {
      perror(basename(fpath));
      return;
    }
  }

  buf = shbuf_init();
  shbuf_cat(buf, data, data_len);
  shfs_inode_write(file, buf);
  shbuf_free(&buf);

  printf ("Wrote %d bytes to inode:\n%s", data_len, shfs_inode_print(file));

  free(data);

}

void share_file_cat(char *path)
{
  shfs_t *tree;
  shfs_ino_t *file;
  shbuf_t *buf;
  char fpath[PATH_MAX+1];
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

  if (file->blk.hdr.type == SHINODE_FILE) {
    buf = shbuf_init();
    shfs_inode_read(file, buf);
    if (buf->data_of)
      printf("%-*.*s", buf->data_of, buf->data_of, buf->data);
    shbuf_free(&buf);
  }

  shfs_free(&tree);

}

void share_file(char *subcmd, char *path)
{
  char *data;
  char hpath[PATH_MAX+1];
  char sub[4096];
  size_t data_len;
  int err;

  memset(sub, 0, sizeof(sub));
  strncpy(sub, subcmd, sizeof(sub) - 1);

  if (0 == strcmp(sub, "list")) {
    return (share_file_list(path));
  }

  if (0 == strcmp(sub, "import")) {
    return (share_file_import(path));
  }

  if (0 == strcmp(sub, "cat")) {
    return (share_file_cat(path));
  }

  if (0 == strncmp(sub, "set:", 4)) {
    shmeta_t *h = shmeta_init(); 
    char *tok = subcmd + 4;
    char *str_val = strchr(tok, '=');
    shbuf_t *buff;
    shkey_t *key;
    if (str_val) {
      *str_val++ = '\0'; 
      
      key = ashkey_str(tok);
      shmeta_set_str(h, key, str_val);
fprintf(stderr, "DEBUG: set hashmap %x using key %x to value '%s'.\n", h, key, str_val);
    }
    sprintf(hpath, ".%s.hmap", path);

    buff = shbuf_init();
    shmeta_print(h, buff);
    err = shfs_write_mem(hpath, buff->data, buff->data_of);
    shbuf_free(&buff);
fprintf(stderr, "DEBUG: saved hmap '%s' (error %d)\n", hpath, err);
    return;
  }

 err = shfs_read_mem(path, &data, &data_len);
  if (err) {
    perror(path);
    return;
  }

  printf ("%-*.*s", data_len, data_len, data);

  free(data);

  
}

