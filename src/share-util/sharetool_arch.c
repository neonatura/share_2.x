
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


int sharetool_archive(char **args, int arg_cnt)
{
  SHFL *file;
  shfs_t *fs;
  char tar_fname[PATH_MAX+1];
  char **tar_files;
  char **ar;
  int file_cnt;
  int err;
  int i;

  memset(tar_fname, 0, sizeof(tar_fname));

  tar_files = (char **)calloc(arg_cnt+1, sizeof(char *));

  file_cnt = 0;
  for (i = 1; i < arg_cnt; i++) {
    if (!*tar_fname) {
      strncpy(tar_fname, args[i], sizeof(tar_fname)-1);
    } else {
      tar_files[file_cnt] = strdup(args[i]);
      if (!(run_flags & PFLAG_QUIET))
        fprintf(sharetool_fout, "%s: added \"%s\".\n", 
            tar_fname, tar_files[file_cnt]);
      file_cnt++;
    }
  }

  if (!*tar_fname || 0 == strcmp(tar_fname, "/")) {
    return (SHERR_INVAL);
  }

  memset(tar_fname, 0, sizeof(tar_fname));
  strncat(tar_fname, "/", SHFS_PATH_MAX-strlen(tar_fname)-1);

  fs = NULL;
  file = sharetool_file(tar_fname, &fs);
err = SHERR_OPNOTSUPP;
/* DEBUG:  err = shfs_arch_PATH(file, tar_files); */

  for (i = 0; i < file_cnt; i++) {
    free(tar_files[i]);
  }
  free(tar_files);

  shfs_free(&fs);

  return (err);
}


