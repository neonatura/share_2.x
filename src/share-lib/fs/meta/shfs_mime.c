
/*
 *  Copyright 2015 Neo Natura 
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


static shmime_t _share_default_mime_types[MAX_DEFAULT_SHARE_MIME_TYPES] = {
  { SHMIME_TEXT_PLAIN, "\.txt", "data/txt", "", 0 },
  { SHMIME_APP_LINUX, "\.bin", "bin/elf", "\177ELF\002", 5 },
  { SHMIME_APP_LINUX_32, "\.bin", "bin/elf32", "\177ELF\001", 5 },
  { SHMIME_APP_GZIP, "\.[t]gz", "arch/gz", "\037\213", 2 }
};
#define BLANK_MIME_TYPE (&_share_default_mime_types[0])


/** Add a file meta definition to a sharefs file-system */ 
int shmime_add(shmime_t *mime)
{
  shfs_t *fs;
  SHFL *file;
  shbuf_t *buff;
  char path[SHFS_PATH_MAX];
  int err;

  if (!mime)
    return (SHERR_INVAL);

  memset(path, 0, sizeof(path));
  snprintf(path, sizeof(path)-1, "/sys/mime/%s", mime->mime_name);

  fs = shfs_init(NULL);
  file = shfs_file_find(fs, path);
  buff = shbuf_map((unsigned char *)mime, sizeof(shmime_t));
  err = shfs_write(file, buff);
  free(buff);
  shfs_free(&fs);
  if (err)
    return (err);

  return (0);
}

const shmime_t *shmime_default(char *type)
{
  int i;

  if (!type)
    return (NULL);

  for (i = 0; i < MAX_DEFAULT_SHARE_MIME_TYPES; i++) {
    if (0 == strcasecmp(type, _share_default_mime_types[i].mime_name))
      return (&_share_default_mime_types[i]);
  }
 
  return (NULL);
}

shmime_t *shmime(char *type)
{
  shfs_t *fs;
  SHFL *file;
  shbuf_t *buff;
  shmime_t *ret_mime;
  char path[SHFS_PATH_MAX];
  int err;

  if (!type || !*type) {
    return (BLANK_MIME_TYPE);
  }

  memset(path, 0, sizeof(path));
  snprintf(path, sizeof(path)-1, "/sys/mime/%s", type);

  fs = shfs_init(NULL);
  file = shfs_file_find(fs, path);

  buff = shbuf_init();
  err = shfs_read(file, buff);
  shfs_free(&fs);
  if (err) {
    shbuf_free(&buff);
    return (shmime_default(type));
  }

  ret_mime = (shmime_t *)calloc(1, sizeof(shmime_t));
  if (!ret_mime) {
    shbuf_free(&buff);
    return (NULL);
  }

  memcpy(ret_mime, shbuf_data(buff), MIN(sizeof(shmime_t), shbuf_size(buff)));
  shbuf_free(&buff);

  return (ret_mime);
}

shmime_t *shmime_file(shfs_ino_t *file)
{
  const char *type;

  type = shfs_meta_get(file, SHMETA_MIME_TYPE);
  return (shmime((char *)type));
}

int shmime_file_set(shfs_ino_t *file, char *type)
{
  int err;

  err = shfs_meta_set(file, SHMETA_MIME_TYPE, type);
  if (err)
    return (err);

  return (0);
}


