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


struct shfs_t *shfs_init(char *app_name, int flags)
{
  shfs_t *root_tree;
  shfs_ino_t *root_node;
  shfs_ino_t *node;
  shfs_ino_t *cwd;
  shfs_ino_t blk;
  shfs_ino_t root;
  char path[PATH_MAX + 1];
  char *ptr;
  int err;

  app_name = shfs_app_name(app_name);

  root_tree = (shfs_t *)calloc(1, sizeof(shfs_t));
  if (!root_tree)
    return (NULL);


  root_node = shfs_inode(NULL, NULL, SHINODE_PARTITION);
  if (!root_node)
    return (NULL);

  root_tree->base_ino = root_node;
  root_tree->cur_ino = root_node;

  root_node->tree = root_tree;
  root_node->base = root_node;
  root_node->hdr.d_type |= SHINODE_PARTITION;

  memset(&blk, 0, sizeof(blk));
  shfs_inode_read_block(root_tree, &blk.hdr, &root);
  root_node->hdr.d_jno = root.hdr.d_jno;
  root_node->hdr.d_ino = root.hdr.d_ino;

  if (!root_node->hdr.d_ino) {
    root_node->hdr.d_jno = shfs_journal_index(root_node);
    root_node->hdr.d_ino = 
      shfs_journal_scan(root_tree, (int)root_node->hdr.d_jno);
    if (!root_node->hdr.d_ino)
      return (NULL);

    err = shfs_inode_write_block(root_tree, &blk.hdr, &root_node->hdr, NULL, 0);
    if (err)
      return (NULL);
    PRINT_RUSAGE("shfs_init: wrote new partition root.");
  }

/* app_name -> cur_ino */

  return (root_tree);
}

/**
 * @todo needs to free inode's cache'd in it's hierarchy
 */
void shfs_free(shfs_t **tree_p)
{
  if (!tree_p)
    return;
  free(*tree_p);
  *tree_p = NULL;
}

_TEST(shfs_init)
{
  shfs_t *tree;

  _TRUEPTR(tree = shfs_init(NULL, 0));
  shfs_free(&tree);
}

shkey_t shfs_partition_id(shfs_t *tree)
{
  if (!tree)
    return ((shkey_t)0);
  return ((shkey_t)shcrc(&tree->peer, sizeof(tree->peer)));
}


