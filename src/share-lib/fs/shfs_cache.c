

/**
 * @copyright
 *
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
 *
 *  @endcopyright
 */

#include "share.h"

shfs_ino_t *shfs_cache_get(shfs_ino_t *parent, shkey_t *name)
{
  shfs_ino_t *ent;

  if (!parent)
    return (NULL); 

  ent = (shfs_ino_t *)shmeta_get_void(parent->child, name);
  if (!ent)
    return (NULL);

  {
    char buf[1024];
    sprintf(buf, "shfs_inode: cache: retrieved inode [%d:%s] from parent [%d:%s].\n", 
        ent->blk.hdr.type, ent->blk.raw, 
        parent->blk.hdr.type, parent->blk.raw);
    PRINT_RUSAGE(buf);
  }

  return (ent);

}

void shfs_cache_set(shfs_ino_t *parent, shfs_ino_t *inode)
{
  int err;

  if (!parent)
    return;

  shmeta_set_void(parent->child, &inode->blk.hdr.name, inode, sizeof(shfs_ino_t *));

  return (0);
}


