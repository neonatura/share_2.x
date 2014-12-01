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
#include "sharelog.h"

#if 0
int share_file_list(char *path)
{
  shfs_t *tree;
  shfs_ino_t *file;
  int err;

  tree = shfs_init(NULL);
  if (!tree) {
    perror("shfs_init");
    return (SHERR_IO);
  }

  file = shfs_file_find(tree, path);
  if (!file) {
    perror("shfs_file_find");
    shfs_free(&tree);
    return (SHERR_NOENT);
  }

  if (file->blk.hdr.type == SHINODE_DIRECTORY) {
    shbuf_t *buff;
    buff = shbuf_init();
    err = shfs_link_list(file, buff);
    if (shbuf_size(buff) != 0)
      printf ("%s", shbuf_data(buff));
    shbuf_free(&buff);
    if (err) {
      perror("shfs_link_list");
      shfs_free(&tree);
      return (err);
    }
  } else {
    printf ("%s", shfs_inode_print(file));
  }

  shfs_free(&tree);
  return (0);
}

int share_file_import_file(char *path)
{
  struct stat st;
  shfs_t *tree;
  shfs_ino_t *file;
  shbuf_t *buf;
  char fpath[PATH_MAX+1];
  char *data;
  size_t data_len;
  int err;

  data = NULL;
  tree = NULL;

  err = shfs_read_mem(path, &data, &data_len);
  if (err) {
    err = -errno;
    perror(path);
    goto done;
  }

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
  if (file->blk.hdr.type != SHINODE_FILE) {
    err = SHERR_ISDIR;
    goto done;
  }

#if 0
  if (file->blk.hdr.type != SHINODE_FILE) {
    if (!IS_INODE_CONTAINER(file->blk.hdr.type)) {
      fprintf(stderr, "%s: %s\n", path, strerror(ENOTDIR));
      err = SHERR_NOENT;
      goto done;
    }
    memset(fpath, 0, sizeof(fpath));
    strncpy(fpath, path, PATH_MAX);
    file = shfs_inode(file, basename(fpath), SHINODE_FILE);
    if (!file) {
      err = SHERR_NOENT;
      perror(basename(fpath));
      goto done;
    }
  }
#endif

#if 0
  buf = shbuf_init();
  shbuf_cat(buf, data, data_len);
  shfs_inode_write(file, buf);
  shbuf_free(&buf);
#endif
  err = shfs_file_write(file, data, data_len);
  if (err)
    goto done;

  err = 0;
  printf ("Wrote %d bytes to inode:\n%s", data_len, shfs_inode_print(file));

done:
  if (data)
    free(data);
  shfs_free(&tree);

  return (err);
}

int share_file_import(char *in_path)
{
  struct stat st;
  char path[PATH_MAX+1];
  char cur_path[PATH_MAX+1];
  DIR *dir;
  struct dirent *ent;
  int err;


  err = stat(in_path, &st);
  if (err) {
    perror(path);
    return (err);
  }

  if (S_ISDIR(st.st_mode)) { 
    memset(path, 0, sizeof(path));
    strncpy(path, in_path, sizeof(path) - 1);
    if (path[strlen(path) - 1] == '/')
      path[strlen(path) - 1] = '\0';

    dir = opendir(path);
    while ((ent = readdir(dir))) {
      if (0 == strcmp(ent->d_name, ".") ||
          0 == strcmp(ent->d_name, ".."))
        continue;

      sprintf(cur_path, "%s/%s", path, ent->d_name);
      err = share_file_import(cur_path);
      if (err) {
        perror(path);
        return (err);
      }
    }
    closedir(dir);
  } else {
    memset(path, 0, sizeof(path));
    strncpy(path, in_path, sizeof(path) - 1);
    err = share_file_import_file(path);
    if (err) {
      perror(path);
      return (err);
    }
  }

  return (0);
}

int share_file_cat(char *path)
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

int share_file_mkdir(char *path)
{
  shfs_t *tree;
  shfs_ino_t *dir;

  tree = shfs_init(NULL);
  if (!tree)
    return (SHERR_IO);

  dir = shfs_dir_find(tree, path);
  if (!dir)
    return (SHERR_NOENT);

  shfs_free(&tree);

  return (0);
}

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
    return (share_file_list(path));
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
#endif
