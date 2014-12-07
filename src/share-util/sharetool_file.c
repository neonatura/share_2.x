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





int share_file_meta_set(char *path, char *cmd, char *val)
{
  shfs_t *tree;
  shfs_ino_t *file;
  int def;
  int err;

  tree = shfs_init(NULL);
  if (!tree) {
    fprintf(stderr, "%s: %s\n", path, strerror(EIO));
    return (SHERR_IO);
  }

  file = shfs_file_find(tree, path);
  if (!file) {
    fprintf(stderr, "%s: %s\n", path, strerror(ENOENT));
    shfs_free(&tree);
    return (SHERR_NOENT);
  }

  err = shfs_meta_set(file, cmd, val);
  shfs_free(&tree);
  if (err)
    return (err);

  printf("Set meta definition '%s' to '%s'.\n", cmd, val);

  return (0);
}

int share_file_meta_get(char *path, char *cmd)
{
  shfs_t *tree;
  shfs_ino_t *file;
  char *str;
  int def;
  int err;

  tree = shfs_init(NULL);
  if (!tree) {
    fprintf(stderr, "%s: %s\n", path, strerror(EIO));
    return (SHERR_IO);
  }

  file = shfs_file_find(tree, path);
  if (!file) {
    fprintf(stderr, "%s: %s\n", path, strerror(ENOENT));
    shfs_free(&tree);
    return (SHERR_NOENT);
  }

  str = shfs_meta_get(file, cmd);
  if (!str || !*str) {
    printf("Meta definition '%s': <not set>\n", cmd);
    shfs_free(&tree);
    return (0);
  }

  printf ("%s", shfs_inode_print(file));
  printf("Meta definition '%s': %s\n", cmd, str);
  shfs_free(&tree);

  return (0);
}

int share_file(char *subcmd, char *path)
{
  char *data;
  char hpath[PATH_MAX+1];
  char sub[4096];
  size_t data_len;
  int err;

  memset(sub, 0, sizeof(sub));
  strncpy(sub, subcmd, sizeof(sub) - 1);

  if (0 == strcmp(sub, "list")) {
    return (share_file_list(path, 0));
  }

  if (0 == strcmp(sub, "import")) {
    return (share_file_import(path));
  }

  if (0 == strcmp(sub, "cat")) {
    return (share_file_cat(path));
  }
  if (0 == strcmp(sub, "mkdir")) {
    return (share_file_mkdir(path));
  }

  if (0 == strncmp(sub, "set:", 4)) {
    char *tok = subcmd + 4;
    char *str_val = strchr(tok, '=');
    if (str_val)
      *str_val++ = '\0'; 
    return (share_file_meta_set(path, tok, str_val));
  }
  if (0 == strncmp(sub, "get:", 4)) {
    char *tok = subcmd + 4;
    return (share_file_meta_get(path, tok));
  }

  if (0 == strcmp(sub, "remove")) {
    return (share_file_remove(path));
  }

#if 0
 err = shfs_read_mem(path, &data, &data_len);
  if (err) {
    perror(path);
    return;
  }

  printf ("%-*.*s", data_len, data_len, data);

  free(data);
#endif

  
}

int share_file_remove(char *path)
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



