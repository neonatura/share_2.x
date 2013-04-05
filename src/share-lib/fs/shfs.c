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

char *shfs_app_name(char *app_name)
{
  char *ptr;

  ptr = strrchr(app_name, '/'); 
  if (ptr)
    app_name = ptr + 1;

  return (app_name);
}

struct shfs_t *shfs_init(char *app_name, int flags)
{
  shfs_t *root_tree;
  shfs_ino_t *root_node;
  shfs_ino_t *node;
  shfs_ino_t *cwd;
  char path[PATH_MAX + 1];
  char *ptr;

  app_name = shfs_app_name(app_name);

  root_tree = (shfs_t *)calloc(1, sizeof(shfs_t));
  if (!root_tree)
    return (NULL);

  root_node = shfs_inode(NULL, NULL, SHINODE_PARTITION);
  root_tree->base_ino = root_node;
 
  cwd = NULL;
  if (flags & SHFS_OVERLAY) {
    /* use "real" current working directory. */
    memset(path, 0, sizeof(path));
    getcwd(path, PATH_MAX);
    cwd = shfs_inode(root_node, path, 0);
  } else {
    /* use application's working directory. */
    cwd = shfs_inode(root_node, app_name, SHINODE_APP);
  }
  if (cwd)
    root_tree->cur_ino = cwd;

#if 0
  if (flags & SHFS_TRACK) {
    apr_pool_t pool;
    root_tree->svn_repo = shsvn_init(.., &pool); 
    if (root_tree->svn_repo)
      root_tree->svn_pool = pool;
  }
#endif

  return (root_tree);
}

void shfs_free(shfs_t *root_tree)
{
#if 0
  if (root_tree->svn_pool)
    apr_pool_destroy(root_tree->svn_pool);
#endif
  free(root_tree);
}

