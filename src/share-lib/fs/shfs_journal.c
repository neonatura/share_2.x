
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


static void _shfs_journal_free(shfs_journal_t *jrnl)
{
  if (!jrnl)
    return;
  shbuf_free(&jrnl->buff);
  free(jrnl);
}

static shfs_journal_t *_shfs_journal_cache_get(shfs_t *tree, int index)
{
  shfs_journal_t *jrnl;
  int cidx;

  cidx = shcrc(&index, sizeof(index)) % MAX_JOURNAL_CACHE_SIZE;
  jrnl = (shfs_journal_t *)tree->jcache[cidx];
  if (jrnl && jrnl->index == index) {
    jrnl->stamp = shtime();
    return (jrnl);
  }

  return (NULL);
}

static void _shfs_journal_cache_set(shfs_t *tree, int index, shfs_journal_t *jrnl)
{
  int cidx;

  if (!jrnl)
    return;

  cidx = shcrc(&index, sizeof(index)) % MAX_JOURNAL_CACHE_SIZE;

  if (tree->jcache[cidx]) {
    if (tree->jcache[cidx] == jrnl)
      return;
    _shfs_journal_free((shfs_journal_t *)tree->jcache[cidx]);
  }

  tree->jcache[cidx] = jrnl;
}

void shfs_journal_cache_free(shfs_t *tree)
{
  int idx;

  for (idx = 0; idx < MAX_JOURNAL_CACHE_SIZE; idx++) {
    _shfs_journal_free(tree->jcache[idx]);
    tree->jcache[idx] = NULL;
  }

}

char *shfs_journal_path(shfs_t *tree, int index)
{
  static char ret_path[PATH_MAX+1];
  const char *base_path;

  if (index < 0 || index >= SHFS_MAX_JOURNAL) {
    return (NULL); /* invalid */
  }

  base_path = get_libshare_path();
  sprintf(ret_path, "%s/_t%x", base_path,
      shcrc(shpeer_kpub(&tree->peer), sizeof(shkey_t)));
  mkdir(ret_path, 0777);
  chmod(ret_path, 0777);

  sprintf(ret_path + strlen(ret_path), "/_j%d", index);
  return (ret_path);
}

_TEST(shfs_journal_path)
{
  shfs_t *tree;
  char *path;
  int i;

  tree = shfs_init(NULL);
  _TRUEPTR(tree);
  for (i = 0; i < SHFS_MAX_JOURNAL; i += 333) {
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
  ssize_t len;
  char *path;
  char *data;
  int err;

  if (!tree) {
    PRINT_RUSAGE("shfs_journal_open: null partition.");
    return (NULL); /* all done */
  }

  j = _shfs_journal_cache_get(tree, index);
  if (j)
    return (j);

  j = (shfs_journal_t *)calloc(1, sizeof(shfs_journal_t));
  if (!j) {
    PRINT_RUSAGE("shfs_journal_open: memory allocation error (1).");
    return (NULL); /* woop woop */
  }

//  j->tree = tree;
  j->index = index;

  path = shfs_journal_path(tree, index);
  if (!path)
    return (NULL); /* invalid index */

  strncpy(j->path, path, sizeof(j->path) - 1);
  _shfs_journal_cache_set(tree, index, j);

  return (j);
}

_TEST(shfs_journal_open)
{
  shfs_t *tree;
  shfs_journal_t *jrnl;
  int jno;

  _TRUEPTR(tree = shfs_init(NULL));
  if (!tree)
    return;

  for (jno = 0; jno < SHFS_MAX_JOURNAL; jno += 333) {
    jrnl = shfs_journal_open(tree, jno);    
    _TRUEPTR(jrnl);
    _TRUE(jrnl->index == jno);
    shfs_journal_close(&jrnl);
  }

  shfs_free(&tree);

}

int shfs_journal_close(shfs_journal_t **jrnl_p)
{
  return (0);
}

int _shfs_journal_scan(shfs_t *tree, int jno, shfs_idx_t *idx)
{
  int crc;
  shfs_journal_t *jrnl;
  shfs_block_t *blk;
  ssize_t jlen;
  int ino_max;
  int ino_min;
  int ino_nr;
  int err;

  jrnl = shfs_journal_open(tree, jno);
  if (!jrnl) {
    return (SHERR_IO);
  }

  jlen = shfs_journal_size(jrnl);
  if (jlen <= 0)
    return (jlen);

retry:
  //ino_max = MIN(jlen / SHFS_MAX_BLOCK_SIZE, SHFS_MAX_BLOCK);
  ino_max = jlen / SHFS_MAX_BLOCK_SIZE;
  ino_min = jno ? 0 : 1; 
  for (ino_nr = (ino_max - 1); ino_nr >= ino_min; ino_nr--) {
    blk = (shfs_block_t *)shfs_journal_block(jrnl, ino_nr);
    if (!blk->hdr.type)
      break; /* found empty inode */
  }

  if (ino_nr < ino_min) {
    jlen = MAX(SHARE_PAGE_SIZE, jlen) * 2;
    err = shbuf_growmap(jrnl->buff, jlen);
    if (!err)
      goto retry;

    return (err);
  }

  err = shfs_journal_close(&jrnl);
  if (err)
    return (err);

  if (idx) {
    idx->jno = jno;
    idx->ino = ino_nr;
  } 

  return (0);
}

int shfs_journal_scan(shfs_t *tree, shkey_t *key, shfs_idx_t *idx)
{
  return (_shfs_journal_scan(tree, key ? shfs_journal_index(key) : 0, idx));
}

shfs_block_t *shfs_journal_block(shfs_journal_t *jrnl, int ino)
{
  size_t data_of;
  int err;

  if (ino < 0)
    return (NULL);

  /* establish memory map */
  if (!jrnl->buff) {
    jrnl->buff = shbuf_file(jrnl->path);
    if (!jrnl->buff)
      return (NULL);
    chmod(jrnl->path, 0777);
  }

  data_of = (ino * SHFS_MAX_BLOCK_SIZE);
  if (data_of >= SHFS_MAX_JOURNAL_SIZE)
    return (NULL);

  /* expand journal as neccessary */
  if (data_of > jrnl->buff->data_max) {
    err = shbuf_grow(jrnl->buff, data_of); 
    if (err)
      return (NULL);
  }

  return ((shfs_block_t *)(jrnl->buff->data + data_of));
}

size_t shfs_journal_size(shfs_journal_t *jrnl)
{

  /* establish memory map */
  if (!jrnl->buff) {
    jrnl->buff = shbuf_file(jrnl->path);
    if (!jrnl->buff)
      return (0);
    chmod(jrnl->path, 0777);
  }

  return (jrnl->buff->data_of);
}

int shfs_journal_index(shkey_t *key)
{
  shfs_inode_off_t of;

  of = shcrc(key, sizeof(shkey_t));
  of = (of % (SHFS_MAX_JOURNAL - 1)) + 1;

  return (of);
}


