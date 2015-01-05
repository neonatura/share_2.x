
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

int shfs_obj_set(shfs_ino_t *file, char *obj_name, char *name, shkey_t *key)
{
  shfs_ino_t *obj;
  shfs_obj_t *obj_data;
  int err;

  if (!file || !name)
    return (SHERR_INVAL);

  obj = shfs_inode(file, name, SHINODE_OBJECT);
  if (!obj) return (SHERR_IO);

fprintf(stderr, "DEBUG: shfs_obj_set: SHINODE_OBJECT_KEY: PRE crc %s\n", shcrcstr(shfs_crc(obj)));

  obj_data = (shfs_obj_t *)obj->blk.raw;
  memcpy(&obj_data->key, key, sizeof(shkey_t));
  obj->blk.hdr.size = sizeof(shfs_obj_t);
  obj->blk.hdr.crc = shcrc(obj_data, sizeof(shfs_obj_t));
  obj->blk.hdr.format = SHINODE_OBJECT;
  err = shfs_inode_write_entity(obj);
  if (err)
    return (err);

fprintf(stderr, "DEBUG: shfs_obj_set('%s', key %s) [ino %s]\n", name, shkey_hex(&obj_data->key), shkey_print(shfs_key(obj))); 
fprintf(stderr, "DEBUG: shfs_obj_set: SHINODE_OBJECT_KEY: crc %s\n", shcrcstr(shfs_crc(obj)));

  return (0);
}

int shfs_obj_get(shfs_ino_t *file, char *obj_name, char *name, shkey_t **key_p)
{
  shfs_ino_t *obj;
  shfs_obj_t *obj_data;
  shkey_t *key;

  obj = shfs_inode(file, name, SHINODE_OBJECT);
  if (!obj) return (SHERR_IO);

  obj_data = (shfs_obj_t *)obj->blk.raw;
fprintf(stderr, "DEBUG: shfs_obj_get: SHINODE_OBJECT: name %s\n", shfs_filename(obj));
fprintf(stderr, "DEBUG: shfs_obj_get: SHINODE_OBJECT: crc %s\n", shcrcstr(shfs_crc(obj)));
fprintf(stderr, "DEBUG: shfs_obj_get: SHINODE_OBJECT: type %d\n", shfs_type(obj));
fprintf(stderr, "DEBUG: shfs_obj_get: SHINODE_OBJECT: format %d\n", shfs_format(obj));
fprintf(stderr, "DEBUG: shfs_obj_get: SHINODE_OBJECT: size %d\n", shfs_size(obj));
  if (shkey_cmp(&obj_data->key, ashkey_blank())) {
fprintf(stderr, "DEBUG: shfs_obj_get: NULL key '%s' [ino %s]\n", shkey_hex(&obj_data->key), shkey_print(shfs_key(obj)));
    /* key not set. */
    return (SHERR_NOENT);
  }

  if (key_p) {
    key = (shkey_t *)calloc(1, sizeof(shkey_t));
    memcpy(key, &obj_data->key, sizeof(shkey_t));
    *key_p = key;
  }

  return (0);
}

_TEST(shfs_obj)
{
  shfs_t *fs;
  shfs_ino_t *file;
  shpeer_t *peer;
  shkey_t *key[25];
  shkey_t *obj_key;
  char obj_name[256];
  int err;
  int i;

  peer = shpeer_init("test", NULL);
  fs = shfs_init(peer);
  file = shfs_file_find(fs, "/shfs_obj");

  memset(obj_name, 0, sizeof(obj_name));
  obj_name[0] = 'a';
  for (i = 0; i < 25; i++) {
    key[i] = shkey_uniq();

    memset(obj_name, 'a' + i, 32);
    
    /* shfs_obj_set() */
    err = shfs_obj_set(file, "test", obj_name, key[i]); 
    _TRUE(err == 0);

    /* shfs_obj_get() */
obj_key = NULL;
    err = shfs_obj_get(file, "test", obj_name, &obj_key);
fprintf(stderr, "DEBUG: #%d: %d = shfs_obj_get('%s', & %x)\n", i, err, obj_name, obj_key);
    _TRUE(err == 0);
    _TRUEPTR(obj_key);
    _TRUE(0 == memcmp(key[i], obj_key, sizeof(shkey_t)));
    shkey_free (&obj_key);
  }

  shfs_free(&fs);
  fs = shfs_init(peer);
  file = shfs_file_find(fs, "/shfs_obj");

  for (i = 0; i < 25; i++) {
    memset(obj_name, 'a' + i, 32);

    /* shfs_obj_get() */
    obj_key = NULL;
    err = shfs_obj_get(file, "test", obj_name, &obj_key);
fprintf(stderr, "DEBUG: #%d: %d = shfs_obj_get('%s', & %s)\n", i, err, obj_name, obj_key?shkey_hex(obj_key):"<n/a>");
    _TRUE(err == 0);
    _TRUEPTR(obj_key);
    _TRUE(0 == memcmp(key[i], obj_key, sizeof(shkey_t)));
    shkey_free (&obj_key);
  }

  for (i = 0; i < 25; i++) {
    shkey_free (&key[i]);
  }

  shfs_free(&fs);
  shpeer_free(&peer);
}


