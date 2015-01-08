
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


shfs_ino_t *shfs_dir_base(shfs_t *tree)
{
  return (tree->base_ino);
}

_TEST(shfs_dir_base)
{
  shfs_t *tree;
  shfs_ino_t *root;

  _TRUEPTR(tree = shfs_init(NULL));
  _TRUEPTR(root = tree->base_ino);
  if (root)
    _TRUE(!root->parent);
  shfs_free(&tree);
}

shfs_ino_t *shfs_dir_parent(shfs_ino_t *inode)
{

  if (shfs_type(inode) != SHINODE_DIRECTORY)
    return (SHERR_INVAL);

  return (shfs_inode_parent(inode));
}

_TEST(shfs_dir_parent)
{
  shfs_t *tree;
  shfs_ino_t *dir;

  _TRUEPTR(tree = shfs_init(NULL));
  _TRUEPTR(dir = shfs_inode(tree->base_ino, "shfs_dir_parent", SHINODE_DIRECTORY));
  _TRUE(dir->parent == tree->base_ino);
  shfs_free(&tree); 
}

shfs_ino_t *shfs_dir_entry(shfs_ino_t *inode, char *fname)
{
  shfs_ino_t *ent;

  ent = shfs_inode(inode, fname, 0);
  if (!ent)
    return (NULL);

  return (ent);
}

shfs_ino_t *shfs_dir_find(shfs_t *tree, char *path)
{
  shfs_ino_t *cur_ino;
  char fname[PATH_MAX+1];
  char *save_ptr;
  char *tok;

  if (!tree)
    return (NULL); /* all done */

  memset(fname, 0, sizeof(fname));
  if (path)
    strncpy(fname, path, PATH_MAX - 1);

  cur_ino = tree->base_ino;

  save_ptr = NULL;
  tok = strtok_r(fname, "/", &save_ptr);
  while (tok) {
    cur_ino = shfs_inode(cur_ino, tok, SHINODE_DIRECTORY);
    if (!cur_ino)
      return (NULL);
//      break;

    tok = strtok_r(NULL, "/", &save_ptr);
  }

  return (cur_ino);
}


shfs_dir_t *shfs_opendir(shfs_t *fs, char *dir_path)
{
  shfs_dirent_t *dir_list;
  shfs_dir_t *dir;
  shfs_ino_t *file;
  int tot;

  dir = (shfs_dir_t *)calloc(1, sizeof(shfs_dir_t));
  if (!dir)
    return (NULL);

  strncpy(dir->path, dir_path, sizeof(dir->path) - 1);
  if (!fs)
    dir->alloc_fs = fs = shfs_init(NULL);
  dir->fs = fs;

  file = shfs_dir_find(dir->fs, dir_path);
  tot = shfs_list(file, &dir->ino);
  if (tot < 0)
    return (NULL);

  dir->ino_tot = tot;
  return (dir);
}

shfs_dirent_t *shfs_readdir(shfs_dir_t *dir)
{
  shfs_dirent_t *ent;
  int idx;

  if (dir->ino_tot <= 0)
    return (NULL);

  idx = dir->ino_idx++;
  if (idx >= dir->ino_tot)
    return (NULL);

  return (dir->ino + idx);
}

int shfs_closedir(shfs_dir_t *dir)
{

  if (dir) { 
    shfs_list_free(&dir->ino);
    shfs_free(&dir->alloc_fs);
    free(dir);
  }

  return (0);
}

