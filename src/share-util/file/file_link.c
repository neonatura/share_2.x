
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
#include "sharetool.h"


int share_file_link(char **args, int arg_cnt, int pflags)
{
  struct stat st;
  shfs_t *src_fs;
  shfs_t *dest_fs;
  shfs_ino_t *dest_file;
  shfs_ino_t *src_file;
  char *src_path;
  char *dest_path;
  int err;

  if (arg_cnt <= 2)
    return (SHERR_INVAL);

  src_path = args[1];
  dest_path = args[2];

  src_file = sharetool_file(src_path, &src_fs);
  dest_file = sharetool_file(dest_path, &dest_fs);
  err = shfs_ref_set(dest_file, src_file);
fprintf(stderr, "DEBUG: %d = shfs_ref_set(dest: '%s' src:'%s')\n", err, dest_path, src_path);
  shfs_free(&src_fs);
  shfs_free(&dest_fs);

  return (err);
}

