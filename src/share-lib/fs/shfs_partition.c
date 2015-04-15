
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
#include "shfs_int.h"

static int _file_queue_id = -1;

shfs_t *shfs_init(shpeer_t *peer)
{
  shfs_t *tree;
  shfs_block_t p_node;
  shfs_block_t base_blk;
  shfs_ino_t *root;
  shfs_ino_t blk;
  shfs_idx_t idx;
  shkey_t *key;
  int flags;
  char path[PATH_MAX + 1];
  char *ptr;
  int err;

  tree = (shfs_t *)calloc(1, sizeof(shfs_t));
  if (!tree)
    return (NULL);

  /* establish peer */
  flags = 0;
  if (!peer) {
    /* default peer's partition. */
    peer = shpeer();
    memcpy(&tree->peer, peer, sizeof(shpeer_t));
    shpeer_free(&peer);
  } else {
    memcpy(&tree->peer, peer, sizeof(shpeer_t));
  }

  /* read partition (supernode) block */
  memset(&idx, 0, sizeof(idx));
  memset(&p_node, 0, sizeof(p_node));
  err = shfs_inode_read_block(tree, &idx, &p_node);
  if (err) {
    PRINT_ERROR(err, "shfs_init [shfs_inode_read_block]");
    return (NULL);
  }

  if (p_node.hdr.type != SHINODE_PARTITION) {
    PRINT_RUSAGE("shfs_init [fresh supernode]");

    /* unitialized partition inode */
    memset(&p_node, 0, sizeof(p_node));
    p_node.hdr.type = SHINODE_PARTITION;
    memcpy(&p_node.hdr.name, shpeer_kpub(&tree->peer), sizeof(shkey_t));
    p_node.hdr.crc = shfs_crc_init(&p_node);
    p_node.hdr.ctime = shtime();

    /* establish directory tree */
    err = shfs_journal_scan(tree, &p_node.hdr.name, &p_node.hdr.fpos);
    if (err) {
      PRINT_ERROR(err, "shfs_init [shfs_journal_scan]");
      return (NULL);
    }

    /* default full public access */
    p_node.hdr.attr |= SHATTR_READ;
    p_node.hdr.attr |= SHATTR_WRITE;
    p_node.hdr.attr |= SHATTR_EXE;

    err = shfs_inode_write_block(tree, &p_node);
    if (err) {
      PRINT_ERROR(err, "shfs_init [shfs_inode_write_block]");
      return (NULL);
    }
  }

  err = shfs_inode_read_block(tree, &p_node.hdr.fpos, &base_blk);
  if (err) { 
    PRINT_ERROR(err, "shfs_init [shfs_inode]");
    return (NULL);
  }

  root = shfs_inode(NULL, NULL, SHINODE_DIRECTORY);
  if (!root) {
    PRINT_ERROR(err, "shfs_init [shfs_inode error]");
    return (NULL);
  }

  if (base_blk.hdr.type == SHINODE_DIRECTORY) {
    memcpy(&root->blk, &base_blk, sizeof(shfs_block_t));
  } else {
    memcpy(&root->blk.hdr.pos, &p_node.hdr.fpos, sizeof(shfs_idx_t));

    root->blk.hdr.crc = shfs_crc_init(&root->blk);
    err = shfs_inode_write_block(tree, &root->blk);
    if (err) {
      PRINT_ERROR(err, "shfs_init [shfs_inode error]");
      return (NULL);
    }
  }

  tree->base_ino = root;
  tree->fsbase_ino = root;
  root->tree = tree;
  root->base = root;

  return (tree);
}

/**
 * @todo needs to free inode's cache'd in it's hierarchy
 */
void shfs_free(shfs_t **tree_p)
{
  shfs_t *tree;

  if (!tree_p)
    return;
  
  tree = *tree_p;
  *tree_p = NULL;
  if (!tree)
    return;

  if (tree->fsbase_ino)
    shfs_inode_free(&tree->fsbase_ino);

  shfs_journal_cache_free(tree);

  free(tree);

  if (_file_queue_id != -1) {
    shmsgctl(_file_queue_id, SHMSGF_RMID, TRUE);
    _file_queue_id = -1;
  }

}

_TEST(shfs_init)
{
  shfs_t *tree;
  shfs_idx_t pos;

  _TRUEPTR(tree = shfs_init(NULL));
  memcpy(&pos, &tree->base_ino->blk.hdr.pos, sizeof(shfs_idx_t));
  shfs_free(&tree);

  _TRUEPTR(tree = shfs_init(NULL));
  _TRUE(0 == memcmp(&pos, &tree->base_ino->blk.hdr.pos, sizeof(shfs_idx_t)));
  shfs_free(&tree);
}

shkey_t *shfs_partition_id(shfs_t *tree)
{
  return (shpeer_kpub(&tree->peer));
}



int _shfs_file_qid(void)
{
  int err;

  if (_file_queue_id = -1) {
    /* initialize queue to share daemon */
    err = shmsgget(NULL);
    if (err < 0)
      return (err);

    _file_queue_id = err;
  }

  return (_file_queue_id);
}

