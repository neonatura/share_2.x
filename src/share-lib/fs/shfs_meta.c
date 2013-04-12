
/*
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
 *
*/  

#include "share.h"


int shfs_meta(shfs_t *tree, shfs_ino_t *ent, shmeta_t **val_p)
{
  shfs_ino_t *meta_ent;
  shmeta_value_t *hdr;
  shmeta_t *h;
  shbuf_t *buff;
  size_t of;
  int err;

  meta_ent = shfs_inode(ent, NULL, SHINODE_META);
  if (!meta_ent)
    return (-1);

  h = shmeta_init();
  if (!h)
    return (-1);

  buff = shbuf_init();
  err = shfs_inode_read(tree, meta_ent, buff, 0, meta_ent->hdr.d_size);
  if (err == -1) {
    shbuf_free(&buff);
    return (-1);
  }

  for (of = 0; of < buff->data_of; of += sizeof(shmeta_value_t)) {
    hdr = (shmeta_value_t *)(buff->data + of); 
    shmeta_set(h, &hdr->name, hdr);
  }

  printf ("Read %d byte meta definition file '%s'\n", meta_ent->hdr.d_size, meta_ent->d_raw.name);
  shbuf_free(&buff);

  if (val_p)
    *val_p = h; 
  else
    shmeta_free(&h);

  return (0);
}

_TEST(shfs_meta)
{
  shfs_t *tree;
  shfs_ino_t *dir;
  shmeta_t *val = NULL;

  _TRUEPTR(tree = shfs_init(NULL, 0)); 
  _TRUEPTR(dir = tree->base_ino);
  _TRUE(!shfs_meta(tree, dir, &val));
  _TRUEPTR(val);
  shfs_meta_free(&val);
}

int shfs_meta_save(shfs_t *tree, shfs_ino_t *ent, shmeta_t *h)
{
  shfs_ino_t *meta_ent;
  shmeta_value_t *hdr;
  shsize_t data_len;
  shbuf_t *buff;
  char *data;
  char *map;
  int err;

  if (!h)
    return (0); /* all done. */

  meta_ent = shfs_inode(ent, NULL, SHINODE_META);
  if (!meta_ent)
    return (-1);

  buff = shbuf_init();
  if (!buff)
    return (-1);

  shmeta_print(h, buff);
  err = shfs_inode_write(tree, meta_ent, buff->data, 0, buff->data_of);
  if (err == -1)
    return (-1);

  shbuf_free(&buff);

  return (0);
}

_TEST(shfs_meta_save)
{
  shfs_t *tree;
  shfs_ino_t *dir;
  shmeta_t *h = NULL;
  shmeta_value_t *val_p;
  shmeta_value_t val;
  shkey_t *key;

  _TRUEPTR(tree = shfs_init(NULL, 0)); 
  _TRUEPTR(dir = tree->base_ino);
  _TRUE(!shfs_meta(tree, dir, &h));
  if (!h)
    return;

  key = shkey_uniq();

  /* save a definition to disk. */
  memset(&val, 0, sizeof(val));
  shmeta_set(h, key, &val); 
  _TRUEPTR(val_p = shmeta_get(h, key));
  _TRUE(!shfs_meta_save(tree, dir, h));
  shfs_meta_free(&h);

  _TRUE(!shfs_meta(tree, dir, &h));
  _TRUEPTR(h);

  _TRUEPTR(val_p = shmeta_get(h, key)); 
  if (val_p)
    _TRUE(0 == memcmp(&val, val_p, sizeof(val)));
  shfs_meta_free(&h);

  shkey_free(&key);
}



