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

int shfs_write_mem(char *path, void *data, size_t data_len)
{
  FILE *fl;
  char hier[NAME_MAX + 1];
  char dir[NAME_MAX + 1];
  char *n_tok;
  char *tok;
  ssize_t b_len;
  int err;

  if (*path != '/') {
    /* recursive dir generation for relative paths. */
    memset(hier, 0, sizeof(hier));
    strncpy(hier, path, sizeof(hier) - 1); 
    tok = strtok(hier, "/");
    while (tok) {
      n_tok = strtok(NULL, "/");
      if (!n_tok)
        break;

      strcat(dir, tok);
      strcat(dir, "/");
      mkdir(dir, 0777);
      tok = n_tok;
    }
  }

  fl = fopen(path, "wb");
  if (!fl)
    return (-1);

  b_len = fwrite(data, sizeof(char), data_len, fl);
  if (b_len < 1)
    return (b_len);

  err = fclose(fl);
  if (err)
    return (err);

//  printf ("Wrote %d bytes to meta definition file '%s'.\n", b_len, path);

  return (0);
}


