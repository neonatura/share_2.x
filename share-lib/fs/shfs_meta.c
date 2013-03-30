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

char *shfs_meta_fname(shfs_node *ent)
{
  static char path[NAME_MAX+1];
  sprintf(path, ".meta/_%lu", shfs_adler32(ent, sizeof(shfs_node)));
  return (path);
}

int shfs_meta(shfs_node *ent, shfs_def **meta_p)
{
  static shfs_def ret_meta;
  shfs_tree *tree = shfs_init();
  char *path;
  char *data;
  size_t data_len;
  int err;

  memset(&ret_meta, 0, sizeof(ret_meta));

  path = shfs_meta_fname(ent);
  err = shfs_read_mem(path, &data, &data_len);
  if (!err) {
    memcpy(&ret_meta, data, MIN(data_len, sizeof(shfs_def)));
    printf ("Read %d byte meta definition file '%s'\n", data_len, path);
  }
  free(data);

  if (meta_p)
    *meta_p = &ret_meta;

  return (0);
} 


int shfs_meta_save(shfs_node *ent, shfs_def *def)
{
  char *path;
  int err;

  path = shfs_meta_fname(ent);
  err = shfs_write_mem(path, def, sizeof(shfs_def)); 
  if (err)
    return (err);

  return (0);
}

int shfs_meta_free(shfs_def **meta_p)
{
  shfs_def *meta;

  if (!meta_p)
    return (0);

  meta = *meta_p;
  if (!meta)
   return (0);

  *meta_p = NULL;
   free(meta);

   return (0);
}


