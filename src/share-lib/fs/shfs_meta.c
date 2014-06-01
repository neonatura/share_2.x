
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
  unsigned char *data;
  size_t data_len;
  size_t of;
  int err;

  meta_ent = shfs_inode(ent, NULL, SHINODE_META);
  if (!meta_ent)
    return (-1);

  h = shmeta_init();
  if (!h)
    return (-1);

  buff = shbuf_init();
  err = shfs_aux_read(meta_ent, buff);
  if (err < 0) {
    PRINT_ERROR(err, "shfs_meta");
    return (err);
  }

  data = shbuf_data(buff);
  data_len = shbuf_size(buff);

  if (meta_ent->pool)
    free(meta_ent->pool);
  meta_ent->pool = (unsigned char *)calloc(data_len, sizeof(char));
  if (!meta_ent->pool)
    return (SHERR_NOMEM);
  memcpy(meta_ent->pool, data, data_len);
  shbuf_free(&buff);

  for (of = 0; of < data_len; of += sizeof(shmeta_value_t)) {
    hdr = (shmeta_value_t *)(meta_ent->pool + of); 
    shmeta_set(h, &hdr->name, hdr);
    of += hdr->sz;
  }

  if (val_p)
    *val_p = h; 
  else
    shmeta_free(&h);

  return (0);
}

_TEST(shfs_meta)
{
  shfs_t *tree;
  shfs_ino_t *file;
  shmeta_t *val = NULL;

  _TRUEPTR(tree = shfs_init(NULL)); 
  _TRUEPTR(file = shfs_inode(tree->base_ino, "shfs_meta", SHINODE_FILE));
  _TRUE(!shfs_meta(tree, file, &val));
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

  if (!h) {
    return (0); /* all done. */
  }

  meta_ent = shfs_inode(ent, NULL, SHINODE_META);
  if (!meta_ent)
    return (SHERR_IO);

  buff = shbuf_init();
  if (!buff)
    return (SHERR_IO);

  shmeta_print(h, buff);

  err = shfs_aux_write(meta_ent, buff);
  if (err)
    return (err);

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


  _TRUEPTR(tree = shfs_init(NULL)); 
//  _TRUEPTR(dir = tree->base_ino);
  _TRUEPTR(dir = shfs_inode(tree->base_ino, "shfs_meta_save", SHINODE_DIRECTORY));
//  _TRUE(!shfs_meta(tree, dir, &h)); /* DEBUG: */
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


int shfs_meta_set(shfs_ino_t *file, int def, char *value)
{
  int err;
  shmeta_t *h;

  err = shfs_meta(file->tree, file, &h);  
  if (err)
    return (err);

  shmeta_set_str(h, ashkey_num(def), value);

  err = shfs_meta_save(file->tree, file, h);
  if (err)
    return (err);

  return (0);
}

char *shfs_meta_get(shfs_ino_t *file, int def)
{
  static char ret_blank[1024];
  char *str;
  int err;
 
  memset(ret_blank, 0, sizeof(ret_blank));
  if (!file->meta) {
    err = shfs_meta(file->tree, file, &file->meta);  
    if (err) {
      return (ret_blank);
    }
  }

  str = shmeta_get_str(file->meta, ashkey_num(def));
  if (!str)
    return (ret_blank);

  return (str);
}

int shfs_meta_perm(shfs_ino_t *file, int def, shkey_t *user)
{
  char *str;
 
  str = shfs_meta_get(file, def);
  if (0 == strcmp(str, shkey_print(user)))
    return (0);

  return (SHERR_ACCESS);
}


