
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
#include <assert.h>


shfs_ino_t *shfs_inode(shfs_ino_t *parent, char *name, int mode)
{
  struct shfs_ino_t *ent = NULL;
  shfs_block_t blk;
  shkey_t ino_key;
  shkey_t *key;
  int err;

  if (!mode)
    mode = SHINODE_FILE;

  /* generate inode token key */
  key = shfs_inode_token(parent, mode, name);

  /* check parent's cache */
  ent = shfs_cache_get(parent, key);
  if (ent) 
    return (ent);

  /* find inode entry. */
  memset(&blk, 0, sizeof(blk));
  err = shfs_link_find(parent, key, &blk);
  if (err && err != SHERR_NOENT) {
    PRINT_ERROR(err, "shfs_inode: shfs_link_find");
    return (NULL);
  }

  ent = (shfs_ino_t *)calloc(1, sizeof(shfs_ino_t));
  if (!err) {
    memcpy(&ent->blk, &blk, sizeof(shfs_block_t));
  } else {
    ent->blk.hdr.type = mode;
    memcpy(&ent->blk.hdr.name, key, sizeof(shkey_t));
    if (name && IS_INODE_CONTAINER(mode)) {
      strncpy(ent->blk.raw, name, SHFS_PATH_MAX - 1);
    }

    if (parent) { /* link inode to parent */
      err = shfs_link(parent, ent);
      if (err) {
        PRINT_ERROR(err, "shfs_inode: shfs_inode_link");
        return (NULL);
      }
    }
  }

  if (parent) {
    ent->parent = parent;
    ent->base = parent->base;
    ent->tree = parent->tree;
  } else {
    ent->base = ent;
  }

  ent->cmeta = NULL;
  ent->meta = NULL;

  shfs_cache_set(parent, ent);

  return (ent);
}

_TEST(shfs_inode)
{
  shfs_t *tree;
  shfs_ino_t *root;
  shfs_ino_t *dir;
  shfs_ino_t *file;
  shfs_ino_t *ref;

  _TRUEPTR(tree = shfs_init(NULL));

  /* verify partition's root. */
  _TRUE(tree->base_ino->blk.hdr.type == SHINODE_DIRECTORY);
  _TRUE(tree->base_ino->blk.hdr.pos.jno);
  _TRUE(tree->base_ino->blk.hdr.pos.ino);

  /* verify directory */
  _TRUEPTR(dir = shfs_inode(tree->base_ino, "shfs_inode", SHINODE_DIRECTORY));
  _TRUE(dir->blk.hdr.type == SHINODE_DIRECTORY);
  _TRUE(dir->blk.hdr.pos.jno);
  _TRUE(dir->blk.hdr.pos.ino);

  shfs_free(&tree);
}

shfs_t *shfs_inode_tree(shfs_ino_t *inode)
{
  if (!inode)
    return (NULL);
  return (inode->tree);
}

_TEST(shfs_inode_tree)
{
  shfs_t *tree;

  _TRUEPTR(tree = shfs_init(NULL));
  if (tree)
    _TRUEPTR(shfs_inode_tree(tree->base_ino));
  shfs_free(&tree);
}

shfs_ino_t *shfs_inode_parent(shfs_ino_t *inode)
{
  if (!inode)
    return (NULL);
  return (inode->parent);
}

shfs_ino_t *shfs_inode_root(shfs_ino_t *inode)
{
  if (!inode)
    return (NULL);
  return (inode->base);
}

int shfs_inode_write_entity(shfs_ino_t *ent)
{
  return (shfs_inode_write_block(ent->tree, &ent->blk));
}

int shfs_inode_write_block(shfs_t *tree, shfs_block_t *blk)
{
  shfs_idx_t *pos = &blk->hdr.pos;
  shfs_journal_t *jrnl;
  shfs_block_t *jblk;
  char *seg;
  int err;

  if (!tree)
    return (-1);

  jrnl = shfs_journal_open(tree, (int)pos->jno);
  if (!jrnl) {
    return (SHERR_IO);
  }

  jblk = shfs_journal_block(jrnl, (int)pos->ino);
  if (!jblk) {
    return (SHERR_IO);
  }

  /* fill block */
  blk->hdr.crc = 0;
  blk->hdr.crc = shcrc(&blk, sizeof(shfs_block_t));
  memcpy(jblk, blk, sizeof(shfs_block_t));

  err = shfs_journal_close(&jrnl);
  if (err) {
    PRINT_RUSAGE("shfs_inode_write_block: error closing journal.");
    return (err);
  }

  return (0);
}

int shfs_inode_write(shfs_ino_t *inode, shbuf_t *buff)
{
  shfs_block_t blk;
  shfs_block_t nblk;
  shfs_idx_t *idx;
  shkey_t *key;
  size_t b_len;
  size_t b_of;
  int err;
  int jno;

  if (!buff)
    return (0);

  b_of = 0;
  idx = &inode->blk.hdr.fpos;


  memset(&blk, 0, sizeof(blk));
  if (!idx->ino) {
    /* create first block. */
    err = shfs_journal_scan(inode->tree, &inode->blk.hdr.name, idx);
    if (err)
      return (err);

    blk.hdr.type = SHINODE_AUX;
    memcpy(&blk.hdr.pos, idx, sizeof(shfs_idx_t));

    key = shkey_bin((char *)&inode->blk, sizeof(shfs_block_t));
    memcpy(&blk.hdr.name, key, sizeof(shkey_t)); 
  }

  while (b_of < buff->data_of) {
    b_len = MIN(SHFS_BLOCK_DATA_SIZE, buff->data_of - b_of);
    blk.hdr.size = b_len;

    idx = &blk.hdr.npos;
    memset(&nblk, 0, sizeof(nblk));
    if (b_len == SHFS_BLOCK_DATA_SIZE) {
      if (!idx->ino) {
        err = shfs_journal_scan(inode->tree, &blk.hdr.name, idx);
        if (err)  
          return (err);

        nblk.hdr.type = SHINODE_AUX;
        memcpy(&nblk.hdr.pos, idx, sizeof(shfs_idx_t));

        key = shkey_bin((char *)&blk, sizeof(shfs_block_t));
        memcpy(&nblk.hdr.name, key, sizeof(shkey_t)); 
      } else {
        err = shfs_inode_read_block(inode->tree, idx, &nblk);
        if (err)
          return (err);
      }
    }

    memset(blk.raw, 0, SHFS_BLOCK_DATA_SIZE);
    memcpy(blk.raw, buff->data + b_of, b_len);
    err = shfs_inode_write_block(inode->tree, &blk);
    if (err)
      return (err);

    b_of += b_len;
    memcpy(&blk, &nblk, sizeof(shfs_block_t));
  }

  /* write the inode to the parent directory */
  inode->blk.hdr.mtime = shtime64();
  inode->blk.hdr.size = buff->data_of;
  err = shfs_inode_write_entity(inode); 
  if (err) {
    PRINT_RUSAGE("shfs_inode_write: error writing entity.");
    return (err);
  }

  return (0);
}

int shfs_inode_read_block(shfs_t *tree, shfs_idx_t *pos, shfs_block_t *ret_blk)
{
  shfs_journal_t *jrnl;
  shfs_block_t *jblk;
  int err;

  jrnl = shfs_journal_open(tree, (int)pos->jno);
  if (!jrnl) {
    PRINT_ERROR(SHERR_IO, "shfs_inode_read_block [shfs_journal_open]");
    return (SHERR_IO);
  }

  jblk = shfs_journal_block(jrnl, (int)pos->ino);
  if (!jblk) {
    PRINT_ERROR(SHERR_IO, "shfs_inode_read_block [shfs_journal_block]");
    return (SHERR_IO);
  }

  if (ret_blk)
    memcpy(ret_blk, jblk, sizeof(shfs_block_t));

  err = shfs_journal_close(&jrnl);
  if (err) {
    PRINT_ERROR(err, "shfs_inode_read_block [shfs_journal_close]");
    return (err);
  }

  return (0);
}

int shfs_inode_read(shfs_ino_t *inode, shbuf_t *ret_buff)
{
  shfs_hdr_t hdr;
  shfs_block_t blk;
  shfs_idx_t idx;
  size_t blk_max;
  size_t blk_nr;
  size_t b_of;
  size_t b_len;
  size_t data_len;
  size_t data_max;
  int err;

  data_len = inode->blk.hdr.size;

  b_of = 0;
  memcpy(&idx, &inode->blk.hdr.fpos, sizeof(shfs_idx_t));
  while (idx.ino) {
    memset(&blk, 0, sizeof(blk)); 
    err = shfs_inode_read_block(inode->tree, &idx, &blk);
    if (err)
      return (err);

    assert(blk.hdr.pos.ino);
    assert(!(blk.hdr.npos.jno == idx.jno && blk.hdr.npos.ino == idx.ino));

    b_len = MIN(SHFS_BLOCK_DATA_SIZE, data_len - b_of);
    shbuf_cat(ret_buff, blk.raw, b_len);

    b_of += b_len;
    memcpy(&idx, &blk.hdr.npos, sizeof(shfs_idx_t));
  }

  return (0);
}

void shfs_inode_free(shfs_ino_t **inode_p)
{
  shfs_ino_t *inode;

  if (!inode_p)
    return;
  
  inode = *inode_p;
  if (!inode)
    return;

  if (inode->tree) {
    if (inode->tree->base_ino == inode || inode->tree->cur_ino == inode)
      return; /* required for additional reference. */
  }

  *inode_p = NULL;

  if (inode->parent)
    shmeta_unset_void(inode->parent->cmeta, &inode->blk.hdr.name);

  shmeta_free(&inode->cmeta);
  shmeta_free(&inode->meta);
  free(inode);
}

_TEST(shfs_inode_free)
{
  shfs_t *tree;
  shfs_ino_t *file;

  _TRUEPTR(tree = shfs_init(NULL));
  if (!tree)
     return;

  /* ensure we cannot free root node of partition. */
  shfs_inode_free(&tree->base_ino);
  _TRUEPTR(tree->base_ino);

  /* ensure we can free newly created file. */
  _TRUEPTR(file = shfs_inode(tree->base_ino, "shfs_inode_free", SHINODE_FILE));
  if (file) {
    shfs_inode_free(&file);
    _TRUE(!file);
  }

  shfs_free(&tree);
}

char *shfs_inode_path(shfs_ino_t *inode)
{
  static char path[PATH_MAX+1];
  char buf[PATH_MAX+1];
  shfs_ino_t *node;

  memset(path, 0, sizeof(path));
  for (node = inode; node; node = node->parent) {
    char *fname = node->blk.raw;
    if (!fname)
      continue;
    strcpy(buf, path);

    strcpy(path, fname);
    if (!*buf) {
      strncat(path, "/", PATH_MAX - strlen(path));
      strncpy(path, buf, PATH_MAX - strlen(path));
    }
  }

  return (path);
}

void shfs_inode_filename_set(shfs_ino_t *inode, char *name)
{
  static char fname[SHFS_BLOCK_SIZE];
  shkey_t *key;

  if (!inode)
    return;

  if (!IS_INODE_CONTAINER(inode->blk.hdr.type)) {
    shfs_meta_set(inode, SHMETA_DESC, name);
    return;
  }


  if (!name || !name[0]) {
    return;
  }

  memset(fname, 0, sizeof(fname));
  strncpy(fname, name, SHFS_PATH_MAX);
  if (strlen(name) > SHFS_PATH_MAX) {
    // suffix identifier to track all size names
    strcat(fname, "$");
    strcat(fname, shkey_print(ashkey_str(name)));
  }

  strcpy(inode->blk.raw, fname);

}

char *shfs_inode_filename_get(shfs_ino_t *inode)
{

  if (!inode)
    return ("");

  if (!IS_INODE_CONTAINER(inode->blk.hdr.type))
    return ((char *)shkey_print(&inode->blk.hdr.name));

  return (inode->blk.raw);
}

shkey_t *shfs_inode_token(shfs_ino_t *parent, int mode, char *fname)
{
  static shkey_t ret_key;
  shbuf_t *buff;
  shkey_t *key;

  /* create unique key token. */
  buff = shbuf_init();
  if (parent)
    shbuf_cat(buff, &parent->blk.hdr.name, sizeof(shkey_t)); 
  shbuf_cat(buff, &mode, sizeof(mode));
  if (fname)
    shbuf_cat(buff, fname, strlen(fname) + 1);
  key = shkey_bin(buff->data, shbuf_size(buff));
  shbuf_free(&buff);

  memcpy(&ret_key, key, sizeof(ret_key));
  shkey_free(&key);

  return (&ret_key);
}

_TEST(shfs_inode_token)
{
  shfs_t *tree;
  shkey_t *key;

  tree = shfs_init(NULL);
  key = shfs_inode_token(tree->base_ino, 0, NULL);
  _TRUE(0 != memcmp(key, &tree->base_ino->blk.hdr.name, sizeof(shkey_t)));

  shfs_free(&tree);
}

char *shfs_inode_id(shfs_ino_t *inode)
{
  return ((char *)shkey_print(&inode->blk.hdr.name));
}

uint64_t shfs_inode_read_crc(shfs_ino_t *inode)
{
  shfs_hdr_t hdr;
  shfs_block_t blk;
  shfs_idx_t idx;
  uint64_t crc;
  int err;
  int i;

  crc = 0;
  memcpy(&idx, &inode->blk.hdr.fpos, sizeof(shfs_idx_t));
  while (idx.ino) {
    memset(&blk, 0, sizeof(blk)); 
    err = shfs_inode_read_block(inode->tree, &idx, &blk);
    if (err)
      return (err);

    for (i = 0; i < SHKEY_WORDS; i++) 
      crc += blk.hdr.name.code[i];

    memcpy(&idx, &blk.hdr.npos, sizeof(shfs_idx_t));
  }

  return (crc);
}

char *shfs_inode_print(shfs_ino_t *inode)
{
  static char ret_buf[4096];
  char buf[256];

  memset(ret_buf, 0, sizeof(ret_buf));
  if (!inode)
    return (ret_buf);

  memset(buf, 0, sizeof(buf));
  if (inode->tree)
    strcpy(buf, shkey_print(&inode->tree->p_node.blk.hdr.name));

  if (inode->parent) {
    sprintf(ret_buf + strlen(ret_buf), "%s of ",
        shfs_inode_block_print(&inode->parent->blk));
  }

  sprintf(ret_buf + strlen(ret_buf), "Peer #%s\n", buf);

  sprintf(ret_buf + strlen(ret_buf), "%s\n",
      shfs_inode_block_print(&inode->blk));

  return (ret_buf);
}

char *shfs_inode_type(int type)
{
  static char ret_buf[1024];

  memset(ret_buf, 0, sizeof(ret_buf));

  switch (type) {
    case SHINODE_PARTITION:
      strcpy(ret_buf, "FS");
      break;
    case SHINODE_FILE:
      strcpy(ret_buf, "File");
      break;
    case SHINODE_DIRECTORY:
      strcpy(ret_buf, "Dir");
      break;
    default:
      strcpy(ret_buf, "Aux");
      break;
  }

  return (ret_buf);
}

char *shfs_inode_block_print(shfs_block_t *jblk)
{
  static char ret_buf[4096];

  memset(ret_buf, 0, sizeof(ret_buf));

  if (!jblk)
    return (ret_buf);

  sprintf(ret_buf, "%s", shfs_inode_type(jblk->hdr.type));

  if (IS_INODE_CONTAINER(jblk->hdr.type)) {
    sprintf(ret_buf + strlen(ret_buf), " \"%s\"", jblk->raw);
  }

  sprintf(ret_buf + strlen(ret_buf),
    " %s:%d:%d:%d size(%lu) crc(%lx)",
    shkey_print(&jblk->hdr.name), 
    jblk->hdr.pos.jno, jblk->hdr.pos.ino, jblk->hdr.type,
    (unsigned long)jblk->hdr.size,
    (unsigned long)jblk->hdr.crc);

  return (ret_buf);
}


