
/*
 * @copyright
 *
 *  Copyright 2014 Neo Natura
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

int shfs_obj_set(shfs_ino_t *file, char *name, shkey_t *key)
{
  shfs_ino_t *obj;
  shfs_obj_t *obj_data;
  int err;

  obj = shfs_inode(file, name, SHINODE_OBJECT);
  if (!obj)
    return (NULL);

  obj_data = (shfs_obj_t *)obj->blk.raw;
  memcpy(&obj_data->key, key, sizeof(shkey_t));

  err = shfs_inode_write_entity(file);
  if (err)
    return (err);

  return (0);
}

shkey_t *shfs_obj_get(shfs_ino_t *file, char *name)
{
  shfs_ino_t *obj;
  shfs_obj_t *obj_data;
  shkey_t *key;

  obj = shfs_inode(file, name, SHINODE_OBJECT);
  if (!obj)
    return (NULL);

  obj_data = (shfs_obj_t *)obj->blk.raw;

  key = (shkey_t *)calloc(1, sizeof(shkey_t));
  memcpy(key, &obj_data->key, sizeof(shkey_t));
  return (key);
}
