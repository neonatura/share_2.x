
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

shfs_t *shfs_journal_tree(shfs_journal_t *jrnl)
{
  if (!jrnl)
    return (NULL);
  return (jrnl->tree);
}

char *shfs_journal_path(shfs_t *tree, int index)
{
  static char ret_path[PATH_MAX+1];
  char *base_path;

  if (index < 0 || index >= SHFS_MAX_JOURNAL) {
    fprintf(stderr, "DEBUG: shfs_journal_path: error: index (%d) [out of range].\n", index);
    return (NULL); /* invalid */
  }

  base_path = shpref_base_dir();
  MKDIR(base_path);

  sprintf(ret_path, "%s/_t%u", base_path,
      shcrc(&tree->peer, sizeof(tree->peer)));
  MKDIR(ret_path);

  sprintf(ret_path + strlen(ret_path), "/_j%d", index);
  return (ret_path);
}

_TEST(shfs_journal_path)
{
  shfs_t *tree;
  char *path;
  int i;

  _TRUEPTR(tree = shfs_init(NULL, 0));
  for (i = 0; i < SHFS_MAX_JOURNAL; i += 33) {
    _TRUEPTR(path = shfs_journal_path(tree, i));
    if (path)
      _TRUE(0 != strlen(path));
  }
  shfs_free(&tree);

}

shfs_journal_t *shfs_journal_open(shfs_t *tree, int index)
{
  shfs_journal_t *j;
  struct stat st;
  char *path;
  char *data;
  int err;

  if (!tree) {
    PRINT_RUSAGE("shfs_journal_open: null partition.");
    return (NULL); /* all done */
  }

  j = (shfs_journal_t *)calloc(1, sizeof(shfs_journal_t));
  if (!j) {
    PRINT_RUSAGE("shfs_journal_open: memory allocation error (1).");
    return (NULL); /* woop woop */
  }

  j->tree = tree;
  j->index = index;

  path = shfs_journal_path(tree, index);
  memset(&st, 0, sizeof(st));
  err = stat(path, &st);
  if (!err && st.st_size != 0) { 
    err = shfs_read_mem(path, &data, &j->data_max); 
    if (err) {
      PRINT_RUSAGE("shfs_journal_open: memory allocation error (2).");
      free(j); 
      return (NULL);
    }
    j->data = (shfs_journal_data_t *)data;
  } else {
    j->data_max = SHFS_BLOCK_SIZE;
    j->data = (shfs_journal_data_t *)calloc(j->data_max, sizeof(char));
  }
  if (!j->data) {
    PRINT_RUSAGE("shfs_journal_open: memory allocation error (3).");
    free(j);
    return (NULL); /* woop woop */
  }

  return (j);
}

_TEST(shfs_journal_open)
{
  shfs_t *tree;
  shfs_journal_t *jrnl;
  int jno;

  _TRUEPTR(tree = shfs_init(NULL, 0));
  if (!tree)
    return;

  for (jno = 0; jno < SHFS_MAX_JOURNAL; jno += 33) {
    jrnl = shfs_journal_open(tree, jno);    
    _TRUEPTR(jrnl);
    _TRUEPTR(jrnl->data);
    _TRUE(jrnl->data_max != 0);
    _TRUE(jrnl->index == jno);
    shfs_journal_close(&jrnl);
  }

  shfs_free(&tree);

}

int shfs_journal_write(shfs_journal_t *jrnl)
{
  shfs_ino_t blank_inode;
  FILE *fl;
  char *path;
  char *data;
  ssize_t b_len;
  int err;

  if (!jrnl)
    return (0); /* all done */

  path = shfs_journal_path(jrnl->tree, jrnl->index);
  fl = fopen(path, "wb+");
  if (!fl)
    return (-1);

  if (jrnl->data && jrnl->data_max >= SHFS_BLOCK_SIZE) {
    data = (char *)jrnl->data;
  } else {
    /* journal base marker. */
    memset(&blank_inode, 0, sizeof(blank_inode));
    data = (char *)&blank_inode;
  }

  /* sanity check */
  jrnl->data_max = MAX(SHFS_BLOCK_SIZE, jrnl->data_max);
  jrnl->data_max = MIN(jrnl->data_max, SHFS_MAX_JOURNAL_SIZE);

  b_len = fwrite(data, sizeof(char), jrnl->data_max, fl);
  if (b_len == -1) {
    fclose(fl);
    return (-1);
  }

  err = fclose(fl);
  if (err) 
    return (-1);

  return (0);
}

_TEST(shfs_journal_write)
{
  shfs_t *tree;
  shfs_journal_t *jrnl;
  int index;

  _TRUEPTR(tree = shfs_init(NULL, 0));
  if (!tree)
    return;

  for (index = 0; index < SHFS_MAX_JOURNAL; index += 33) {
    _TRUEPTR(jrnl = shfs_journal_open(tree, index));
    if (!jrnl)
      continue;

    _TRUE(!shfs_journal_write(jrnl));
    shfs_journal_close(&jrnl);
  }
}

int shfs_journal_close(shfs_journal_t **jrnl_p)
{
  shfs_journal_t *jrnl;
  int ret_err;

  if (!jrnl_p)
    return (0); /* all done */

  jrnl = *jrnl_p;
  *jrnl_p = NULL;
  if (!jrnl)
    return (0); /* all done */

  ret_err = shfs_journal_write(jrnl);

  free(jrnl->data);
  free(jrnl);

  return (ret_err);
}

int shfs_journal_grow(shfs_journal_t **jrnl_p)
{
  struct stat st;
  FILE *fl;
  shfs_t *tree;
  shfs_journal_t *jrnl;
  shfs_ino_t blk;
  char *path;
  size_t b_of;
  size_t b_len;
  size_t b_max;
  int jno;
  int err;

  PRINT_RUSAGE("shfs_journal_grow:begin");

  if (!jrnl_p || !*jrnl_p) {
fprintf(stderr, "DEBUG: shfs_journal_grow: no journal specified.\n");
    return (0);
  }

  jrnl = *jrnl_p;
  path = shfs_journal_path(jrnl->tree, jrnl->index);
  
  jno = jrnl->index;
  err = shfs_journal_close(&jrnl);
  if (err) {
    fprintf(stderr, "DEBUG: shfs_journal_grow: error: journal '%s' close [%s]\n", path, strerror(errno));
    return (err);
  }
  *jrnl_p = NULL;

  fl = fopen(path, "wb+");
  if (!fl) {
    fprintf(stderr, "DEBUG: shfs_journal_grow: error: journal '%s' open [%s]\n", path, strerror(errno));
    return (-1);
  }

  memset(&st, 0, sizeof(st));
  err = FSTAT(fl, &st);
  if (err) {
    fprintf(stderr, "DEBUG: shfs_journal_grow: error: journal '%s' stat [%s]\n", path, strerror(errno));
    return (-1);
  }

  err = FSEEK(fl, st.st_size, SEEK_SET);
  if (err) {
    fprintf(stderr, "DEBUG: shfs_journal_grow: error: journal '%s' seek [%s]\n", path, strerror(errno));
    return (err);
  }

  memset(&blk, 0, sizeof(blk));
  b_max = MIN(SHFS_MAX_JOURNAL_SIZE, st.st_size * 4);
  for (b_of = st.st_size; b_of < b_max; b_of += SHFS_BLOCK_SIZE) {
    b_len = fwrite(&blk, SHFS_BLOCK_SIZE, 1, fl);
    if (b_len != SHFS_BLOCK_SIZE) {
      fprintf(stderr, "DEBUG: shfs_journal_grow: error: write [%s]\n", strerror(errno));
      return (-1);
     }
  }

  err = fclose(fl);
  if (err) {
    fprintf(stderr, "DEBUG: shfs_journal_grow: error: journal '%s' close [%s]\n", path, strerror(errno));
    return (err);
  }

  /* journal will be re-opened, if needed */
  PRINT_RUSAGE("shfs_journal_grow:end");
  
  return (0);
}

_TEST(shfs_journal_grow)
{
  shfs_t *tree;
  shfs_journal_t *jrnl;

  _TRUEPTR(tree = shfs_init(NULL, 0));
  _TRUEPTR(jrnl = shfs_journal_open(tree, 0));
  _TRUE(!shfs_journal_grow(&jrnl));
  
  shfs_journal_close(&jrnl);
  shfs_free(&tree);
}

int shfs_journal_index(shfs_ino_t *inode)
{
  int crc = (int)(shcrc(inode->d_raw.name, SHFS_BLOCK_DATA_SIZE) % SHFS_MAX_JOURNAL);
  return ((shfs_inode_off_t)crc);
}

int shfs_journal_scan(shfs_t *tree, int jno)
{
  shfs_journal_t *jrnl;
  shfs_ino_t *ino;
  int ino_max;
  int ino_nr;
  int err;

  jrnl = shfs_journal_open(tree, jno);
  if (!jrnl) {
  fprintf(stderr, "DEBUG: shfs_journal_scan: error: journal open [%s]\n", strerror(errno));
    return (0);
  }

  ino_max = jrnl->data_max / SHFS_BLOCK_SIZE;
  for (ino_nr = 1; ino_nr < ino_max; ino_nr++) {
    ino = (shfs_ino_t *)jrnl->data->block[ino_nr];
    if (!ino->hdr.d_size)
      break; /* found empty inode */
  }

  if (ino_nr >= ino_max && ino_nr < SHFS_MAX_BLOCK) {
    err = shfs_journal_grow(&jrnl);
    if (err) {
      PRINT_RUSAGE("ERROR: shfs_journal_scan: journal grow");
      shfs_journal_close(&jrnl);
      return (0);
    }
    ino_nr = ino_max;
  }

  err = shfs_journal_close(&jrnl);
  if (err) {
    return (0);
  }

  if (ino_nr == SHFS_MAX_BLOCK)
    return (0); /* none avail */

  return (ino_nr);
}

_TEST(shfs_journal_scan)
{

  shfs_t *tree;

  _TRUEPTR(tree = shfs_init(NULL, 0));
  _TRUE(shfs_journal_scan(tree, 0));
  shfs_free(&tree);
}


