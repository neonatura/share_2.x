
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
  char path[SHFS_PATH_MAX];
  int err;


  memset(path, 0, sizeof(path));
  if (name)
    strncpy(path, name, sizeof(path) - 1);
  if (*path && path[strlen(path) - 1] == '/') {
    path[strlen(path) - 1] = '\0';
    if (!mode)
      mode = SHINODE_DIRECTORY;
  } else {
    if (!mode)
      mode = SHINODE_FILE;
  }

  /* generate inode token key */
  key = shfs_inode_token(parent, mode, path);
  if (!key) {
    PRINT_ERROR(err, "shfs_inode: shfs_inode_token");
    return (NULL);
  }

  /* check parent's cache */
  ent = shfs_cache_get(parent, key);
  if (ent) { 
    return (ent);
  }

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
    if (IS_INODE_CONTAINER(mode))
      strcpy(ent->blk.raw, path);

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

/*
  blk->hdr.crc = shcrc(&blk->hdr.name, sizeof(shkey_t));
  blk->hdr.crc += shcrc((char *)blk->raw, SHFS_BLOCK_DATA_SIZE);
*/

  /* fill block */
  blk->hdr.mtime = shtime64();
  memcpy(jblk, blk, sizeof(shfs_block_t));

/*
fprintf(stderr, "DEBUG: --\n");
fprintf(stderr, "DEBUG: shfs_inode_write_block: [name %s]\n", shkey_print(&jblk->hdr.name));
fprintf(stderr, "DEBUG: shfs_inode_write_block: [pos %d:%d]\n", jblk->hdr.pos.jno, blk->hdr.pos.ino);
fprintf(stderr, "DEBUG: shfs_inode_write_block: [npos %d:%d]\n", jblk->hdr.npos.jno, blk->hdr.npos.ino);
fprintf(stderr, "DEBUG: shfs_inode_write_block: [fpos %d:%d]\n", jblk->hdr.fpos.jno, blk->hdr.fpos.ino);
fprintf(stderr, "DEBUG: shfs_inode_write_block: [size %lu]\n", jblk->hdr.size);
fprintf(stderr, "DEBUG: shfs_inode_write_block: [crc %lu]\n", jblk->hdr.crc);
fprintf(stderr, "DEBUG: shfs_inode_write_block: [type %lu]\n", jblk->hdr.type);
*/

  err = shfs_journal_close(&jrnl);
  if (err) {
    PRINT_RUSAGE("shfs_inode_write_block: error closing journal.");
    return (err);
  }

  return (0);
}

int shfs_inode_clear_block(shfs_t *tree, shfs_idx_t *pos)
{
  shfs_journal_t *jrnl;
  shfs_block_t *jblk;
  char *seg;
  int err;

  if (!tree)
    return (-1);

  jrnl = shfs_journal_open(tree, (int)pos->jno);
  if (!jrnl) {
    PRINT_RUSAGE("shfs_inode_clear_block: error opening journal");
    return (SHERR_IO);
  }

  jblk = shfs_journal_block(jrnl, (int)pos->ino);
  if (!jblk) {
    PRINT_RUSAGE("shfs_inode_clear_block: error referencing block.");
    return (SHERR_IO);
  }

  /* clear block */
  memset(jblk, 0, sizeof(shfs_block_t));

  err = shfs_journal_close(&jrnl);
  if (err) {
    PRINT_RUSAGE("shfs_inode_clear_block: error closing journal.");
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
  _TRUEPTR(tree);
  key = shfs_inode_token(tree->base_ino, 0, NULL);
  _TRUE(0 != memcmp(key, &tree->base_ino->blk.hdr.name, sizeof(shkey_t)));

  shfs_free(&tree);
}

char *shfs_inode_id(shfs_ino_t *inode)
{
  return ((char *)shkey_print(&inode->blk.hdr.name));
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
    sprintf(ret_buf + strlen(ret_buf), "%s\n",
        shfs_inode_block_print(&inode->parent->blk));
  }

  sprintf(ret_buf + strlen(ret_buf), "[Inode %s @ %s]\n", 
      shkey_print(&inode->blk.hdr.name), buf);

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
    case SHINODE_BINARY:
      strcpy(ret_buf, "Bin");
      break;
    case SHINODE_AUX:
      strcpy(ret_buf, "Aux");
      break;
    default:
      strcpy(ret_buf, "Unknown");
      break;
  }

  return (ret_buf);
}

int shfs_inode_clear(shfs_ino_t *inode)
{
  shfs_block_t blk;
  shfs_block_t nblk;
  shfs_idx_t *idx;
  shkey_t *key;
  size_t b_len;
  size_t b_of;
  int err;
  int jno;

  if (!inode)
    return (0);

  b_of = 0;
  idx = &inode->blk.hdr.fpos;

  memcpy(&blk, &inode->blk, sizeof(blk));
  while (idx->ino) {
    /* wipe current position */
    err = shfs_inode_clear_block(inode->tree, idx);
    if (err)
      return (err);

    /* read in next block. */
    idx = &blk.hdr.npos;
    if (idx->ino) {
      memset(&nblk, 0, sizeof(nblk));
      if (idx->ino) {
        err = shfs_inode_read_block(inode->tree, idx, &nblk);
        if (err)
          return (err);
      }
    }

    memcpy(&blk, &nblk, sizeof(shfs_block_t));
  }

  /* write the inode to the parent directory */
  inode->blk.hdr.mtime = 0;
  inode->blk.hdr.size = 0;
  memset(&inode->blk.hdr.fpos, 0, sizeof(shfs_idx_t));
  err = shfs_inode_write_entity(inode); 
  if (err) {
    PRINT_RUSAGE("shfs_inode_write: error writing entity.");
    return (err);
  }

  return (0);
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
    " %-4.4x:%-4.4x size(%lu) crc(%lx) mtime(%-20.20s)",
    jblk->hdr.pos.jno, jblk->hdr.pos.ino,
    (unsigned long)jblk->hdr.size,
    (unsigned long)jblk->hdr.crc,
    shctime64(jblk->hdr.mtime)+4);

  return (ret_buf);
}

uint64_t shfs_inode_crc(shfs_block_t *blk)
{
  uint64_t crc;

  crc = 0;
  crc += shcrc(&blk->hdr.name, sizeof(shkey_t));
  crc += shcrc((char *)blk->raw, SHFS_BLOCK_DATA_SIZE);

  return (crc);
}
