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

#include "share.h"
#include "sharetool.h"

void share_file(char *subcmd, char *path)
{
  char *data;
  char hpath[PATH_MAX+1];
  char sub[4096];
  size_t data_len;
  int err;

  memset(sub, 0, sizeof(sub));
  strncpy(sub, subcmd, sizeof(sub) - 1);

  if (0 == strncmp(sub, "set:", 4)) {
    shmeta_t *h = shmeta_init(); 
    char *tok = subcmd + 4;
    char *str_val = strchr(tok, '=');
    shbuf_t *buff;
    shkey_t *key;
    if (str_val) {
      *str_val++ = '\0'; 
      
      key = ashkey_str(tok);
      shmeta_set_str(h, key, str_val);
fprintf(stderr, "DEBUG: set hashmap %x using key %x to value '%s'.\n", h, key, str_val);
    }
    sprintf(hpath, ".%s.hmap", path);

    buff = shbuf_init();
    shmeta_print(h, buff);
    err = shfs_write_mem(hpath, buff->data, buff->data_of);
    shbuf_free(&buff);
fprintf(stderr, "DEBUG: saved hmap '%s' (error %d)\n", hpath, err);
    return;
  }

 err = shfs_read_mem(path, &data, &data_len);
  if (err) {
    perror(path);
    return;
  }

  printf ("%-*.*s", data_len, data_len, data);

  free(data);

  
}

