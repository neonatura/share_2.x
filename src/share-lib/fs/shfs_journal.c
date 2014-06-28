
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

char *shfs_journal_path(shfs_t *tree, int index)
{
  static char ret_path[PATH_MAX+1];
  char *base_path;

  if (index < 0 || index >= SHFS_MAX_JOURNAL) {
    return (NULL); /* invalid */
  }

  base_path = shpref_base_dir();
  MKDIR(base_path);

  sprintf(ret_path, "%s/_t%x", base_path,
      shcrc(&tree->peer->name, sizeof(shkey_t)));
  MKDIR(ret_path);

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

  j = (shfs_journal_t *)calloc(1, sizeof(shfs_journal_t));
  if (!j) {
    PRINT_RUSAGE("shfs_journal_open: memory allocation error (1).");
    return (NULL); /* woop woop */
  }

  j->tree = tree;
  j->index = index;

  path = shfs_journal_path(tree, index);
  strncpy(j->path, path, sizeof(j->path) - 1);

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
  shfs_journal_t *jrnl;
  int ret_err;

  if (!jrnl_p)
    return (0); /* all done */

  jrnl = *jrnl_p;
  *jrnl_p = NULL;
  if (!jrnl)
    return (0); /* all done */

  shbuf_free(&jrnl->buff);

  free(jrnl);

  return (0);
}

int shfs_journal_scan(shfs_t *tree, shkey_t *key, shfs_idx_t *idx)
{
  int crc;
  shfs_journal_t *jrnl;
  shfs_block_t *blk;
  ssize_t jlen;
  int ino_max;
  int ino_nr;
  int jno;
  int err;

  jno = shfs_journal_index(key);
fprintf(stderr, "DEBUG: shfs_journal_scan: jno = %d\n", jno);
  jrnl = shfs_journal_open(tree, jno);
  if (!jrnl) {
    return (SHERR_IO);
  }

  jlen = shfs_journal_size(jrnl);
  if (jlen < 0)
    return (jlen);

retry:
  ino_max = MIN(jlen / SHFS_BLOCK_SIZE, SHFS_MAX_BLOCK);
  for (ino_nr = (ino_max - 1); ino_nr >= 0; ino_nr--) {
    blk = (shfs_block_t *)shfs_journal_block(jrnl, ino_nr);
    if (!blk->hdr.type)
      break; /* found empty inode */
fprintf(stderr, "DEBUG: shfs_journal_scan: ino_nr # %d is type %d [blocksize %d]\n", ino_nr, blk->hdr.type, sizeof(shfs_block_t));
  }

  err = shfs_journal_close(&jrnl);
  if (err)
    return (err);
fprintf(stderr, "DEBUG: shfs_journal_scan: ino_nr = %d, ino_max = %d\n", ino_nr, ino_max);

  //if (ino_nr >= SHFS_MAX_BLOCK)
  if (ino_nr < 0) {
    jlen *= 2;
fprintf(stderr, "DEBUG: shfs_journal_scan: growing journal to %d bytes.\n", jlen);
    err = shbuf_growmap(jrnl->buff, jlen);
    if (!err)
      goto retry;
    return (SHERR_IO);
  }

  if (idx) {
    idx->jno = jno;
    idx->ino = ino_nr;
  } 

  return (0);
}

_TEST(shfs_journal_scan)
{
  shfs_t *tree;
  shfs_block_t r_blk;
  shfs_block_t blk;
  shkey_t *key;
  int err;

  _TRUEPTR(tree = shfs_init(NULL));

  /* create [random] new node. */
  key = shkey_uniq();
  memset(&blk, 0, sizeof(blk));
  memcpy(&blk.hdr.name, key, sizeof(shkey_t));
  _TRUE(!shfs_journal_scan(tree, key, &blk.hdr.pos));
  _TRUE(blk.hdr.pos.jno);
  _TRUE(blk.hdr.pos.ino);

  /* write new node. */
  strcpy(blk.raw, "shfs_journal_scan");
  _TRUE(!shfs_inode_write_block(tree, &blk));

  memset(&r_blk, 0, sizeof(r_blk));
  _TRUE(!shfs_inode_read_block(tree, &blk.hdr.pos, &r_blk));
  _TRUE(0 == strcmp(r_blk.raw, "shfs_journal_scan"));

  /* remove node .. */

  shkey_free(&key);
  shfs_free(&tree);

  /* verify change after partition reload */
  _TRUEPTR(tree = shfs_init(NULL));

  memset(&r_blk, 0, sizeof(r_blk));
  _TRUE(!shfs_inode_read_block(tree, &blk.hdr.pos, &r_blk));
  _TRUE(0 == strcmp(r_blk.raw, "shfs_journal_scan"));

  shfs_free(&tree);

  printf("shfs_journal_scan: OK\n");
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
    if (!jrnl->buff) {
fprintf(stderr, "DEBUG: !shbuf_file(%s)\n", jrnl->path);
      return (NULL);
}
fprintf(stderr, "DEBUG: shfs_journal_block: opened block '%s'\n", jrnl->path);
  }

  data_of = (ino * SHFS_BLOCK_SIZE);
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
      return (NULL);
fprintf(stderr, "DEBUG: shfs_journal_size: journal '%s' is %d bytes.\n", jrnl->path, jrnl->buff->data_of);
  }

  return (jrnl->buff->data_of);
}

/* todo: open/close cache up to 16 journals */
