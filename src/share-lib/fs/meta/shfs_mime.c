
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
#include <regex.h>

static shmime_t _share_default_mime_types[MAX_DEFAULT_SHARE_MIME_TYPES] = {
  { SHMIME_TEXT_PLAIN, "\\.txt", "data/txt", "[\\x02-\\x7F]+", MAX_MIME_HEADER_SIZE },
  { SHMIME_APP_LINUX, "\\.bin", "bin/elf", "\177ELF\002", 5 },
  { SHMIME_APP_LINUX_32, "\\.bin", "bin/elf32", "\177ELF\001", 5 },
  { SHMIME_APP_GZIP, "\\.[t]gz", "arch/gz", "\037\213", 2 },
  { SHMIME_APP_TAR, "\\.tar", "arch/tar", "\x01ustar", 263 },
  { SHMIME_APP_PEM, "\\.pem", "cert/pem", "\x03\x02\x01\x02", 13 }, 
  { SHMIME_APP_PEM, "\\.pem", "cert/pem", "-----BEGIN", 10 }, /* base64 */
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

  fs = shfs_sys_init(SHFS_DIR_MIME, mime->mime_name, &file);
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

  fs = shfs_sys_init(SHFS_DIR_MIME, type, &file);
  buff = shbuf_init();
  err = shfs_read(file, buff);
  shfs_free(&fs);
  if (err) {
    shbuf_free(&buff);
    return ((shmime_t *)shmime_default(type));
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

static int shmime_file_set_default(SHFL *file, shmime_t **mime_p)
{
  shfs_ino_t *aux;
  shmime_t *mime;
  regex_t reg;
  shbuf_t *buff;
  char header[MAX_MIME_HEADER_SIZE+1];
  size_t header_len;
  int format;
  int err;
  int idx;
  int i;

  if (mime_p)
    *mime_p = NULL;
  
  /* read header from file. */
  buff = shbuf_init();
  err = shfs_read_of(file, buff, 0, MAX_MIME_HEADER_SIZE);
  if (err) {
    shbuf_free(&buff);
    return (err);
  }

  /* establish header */
  header_len = MIN(MAX_MIME_HEADER_SIZE, shbuf_size(buff)); 

  for (idx = (MAX_DEFAULT_SHARE_MIME_TYPES-1); idx >= 0; idx--) {
    mime = &(_share_default_mime_types[idx]);
    err = regcomp(&reg, mime->mime_header, REG_NOSUB);
//fprintf(stderr, "DEBUG: [%s] %s (%d) = regcomp('%s')\n", mime->mime_name, strerror(errno), err, mime->mime_header);
    if (err) {
      sherr(-err, "regcomp");
      continue;
    }

    if (header_len < mime->mime_header_len)
      continue; /* too small */
 
    memset(header, 0, sizeof(header));
    memcpy(header, shbuf_data(buff), mime->mime_header_len);
    /* convert '\0' to unprintable to be 'text-friendly' */
    for (i = 0; i < mime->mime_header_len; i++)
      if (header[i] == '\x00') header[i] = '\x01';

    err = regexec(&reg, header, 0, NULL, 0); 
//fprintf(stderr, "DEBUG: %s (%d) = regexec('%s')\n", strerror(errno), err, header);
    regfree(&reg);
    if (err)
      continue; /* not a match */

    if (mime_p)
      *mime_p = mime;

    shbuf_free(&buff);
    return (0);
  }
  shbuf_free(&buff);

  return (SHERR_INVAL); /* match not found */
}

shmime_t *shmime_file(shfs_ino_t *file)
{
  shmime_t *mime;
  const char *type;
  int err;

  type = shfs_meta_get(file, SHMETA_MIME_TYPE);
  if (!type || !*type) {
    /* establish a mime-type based on the file's content. */
    err = shmime_file_set_default(file, &mime); 
    if (!err) {
      return (mime);
    }
  }

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

char *shmime_print(shmime_t *mime)
{
  static char ret_buf[1024];

  memset(ret_buf, 0, sizeof(ret_buf));

  strncpy(ret_buf, mime->mime_name, sizeof(ret_buf) - 1);

  return (ret_buf);
}

char **shmime_default_dirs(void)
{
  static char **ret_list;
  int i, j, k;

  if (!ret_list) {
    ret_list = (char **)calloc(MAX_DEFAULT_SHARE_MIME_TYPES+1, sizeof(char *));
    if (!ret_list)
      return (NULL);

    j = -1;
    for (i = 0; i < MAX_DEFAULT_SHARE_MIME_TYPES; i++) {
      /* weed out dups */
      for (k = 0; k < i; k++) {
        if (0 == strcmp(_share_default_mime_types[i].mime_dir,
              _share_default_mime_types[k].mime_dir))
          break;
      }
      if (k != i) continue;

      ret_list[++j] = _share_default_mime_types[i].mime_dir;
    }
  }

  return (ret_list);
}
