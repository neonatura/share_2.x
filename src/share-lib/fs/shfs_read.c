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

int shfs_read_mem(char *path, char **data_p, size_t *data_len_p)
{
  FILE *fl;
  struct stat st;
  size_t data_len;
  ssize_t r_len;
  char *data;
  int err;

  *data_p = NULL;
  if (data_len_p)
    *data_len_p = 0;

  memset(&st, 0, sizeof(st));
  err = stat(path, &st);
  if (err)
    return (err);

  if (st.st_size == 0) {
    *data_p = strdup("");
    if (data_len_p)
      *data_len_p = 0;
    return (0);
  }

  data_len = MAX(4096, st.st_size);
  data = (char *)calloc(data_len, sizeof(char));
  if (!data)
    return (-1);

  fl = fopen(path, "rb");
  if (!fl) {
    free(data);
    return (-1);
  }

  r_len = fread(data, sizeof(char), data_len, fl);
  if (r_len < 1) {
    free(data);
    return (-1);
  }

  err = fclose(fl);
  if (err) {  
    free(data);
    return (err);
  }

  *data_p = data;
  if (data_len_p)
    *data_len_p = (size_t)r_len;
  
  return (0);
}

