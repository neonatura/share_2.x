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

#include "../share.h"

static shfs_tree *root_tree;

struct shfs_tree *shfs_init(void)
{

  if (!root_tree) {
    root_tree = (char *)calloc(1, sizeof(shfs_tree));
  }


  return (root_tree);
}

struct shfs_node *shfs_node_entry(char *path)
{
  static struct shfs_node ent;

  memset(&ent, 0, sizeof(ent));
  strncpy(ent.d_name, path, sizeof(ent.d_name) - 1);
  ent.d_reclen = strlen(ent.d_name);

  return (&ent);
}
