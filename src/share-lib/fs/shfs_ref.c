
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


shfs_ino_t *shfs_ref_base(shfs_t *tree)
{
  shfs_ino_t *pub_root;

  pub_root = shfs_inode(tree->base_ino, NULL, SHINODE_REFERENCE);

  return (pub_root);
}

shfs_ino_t *shfs_ref_parent(shfs_ino_t *inode)
{
  return (shfs_inode_parent(inode));
}

shfs_ino_t *shfs_ref_entry(shfs_ino_t *inode, char *fname)
{
  shfs_ino_t *ent;

  ent = shfs_inode(inode, fname, 0);
  if (!ent)
    return (NULL);

  return (ent);
}

shfs_ino_t *shfs_ref_find(shfs_t *tree, char *path)
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

  cur_ino = shfs_ref_base(tree);

  save_ptr = NULL;
  tok = strtok_r(fname, "/", &save_ptr);
  while (tok) {
    cur_ino = shfs_inode(cur_ino, tok, SHINODE_REFERENCE);
    if (!cur_ino)
      return (NULL);
//      break;

    tok = strtok_r(NULL, "/", &save_ptr);
  }

  return (cur_ino);
}

