
/**
 * @copyright
 *
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
 *
 *  @endcopyright
 */

#include "share.h"

shfs_ino_t *shfs_cache_get(shfs_ino_t *parent, shkey_t *name)
{
#ifdef WITH_FSCACHE
  shfs_ino_t *ent;

  if (!parent)
    return (NULL); 

  ent = (shfs_ino_t *)shmeta_get_ptr(parent->cmeta, name);
  if (!ent)
    return (NULL);

  return (ent);
#else
  return (NULL);
#endif
}

void shfs_cache_set(shfs_ino_t *parent, shfs_ino_t *inode)
{
#ifdef WITH_FSCACHE
  if (!parent)
    return;

  shmeta_set_ptr(parent->cmeta, &inode->blk.hdr.name, inode);
#endif
}

_TEST(shfs_cache_get)
{
#ifdef WITH_FSCACHE
  shfs_t *tree;
  shfs_ino_t *file;
  shfs_ino_t *t_file;

  tree = shfs_init(NULL);
  file = shfs_file_find(tree, "/test/shfs_cache_get");
  _TRUEPTR(file);
  _TRUEPTR(file->parent);

  t_file = (shfs_ino_t *)shfs_cache_get(file->parent, shfs_file_key(file));
  _TRUEPTR(t_file);
  _TRUE(file == t_file);

  shfs_free(&tree);
#endif
}



