
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



int sharetool_archive_write(shz_t *z, shz_idx f_idx, char *f_path, shbuf_t *buff)
{
  char pwd_path[PATH_MAX+1];
  char *rel_path;
  int err;

  memset(pwd_path, 0, sizeof(pwd_path));
  getcwd(pwd_path, sizeof(pwd_path)-1);
  if (0 == strncmp(f_path, pwd_path, strlen(pwd_path))) {
    rel_path = f_path + (strlen(pwd_path) + 1);
  } else {
    rel_path = f_path;
  }


  if (run_flags & PFLAG_DECODE) {
    err = shz_list_write(z, f_idx, f_path, buff);
    if (err) {
      fprintf(sharetool_fout, "error: %s: %s\n", f_path, sherrstr(err));
      return (err);
    }
  } else if (run_flags & PFLAG_VERIFY) {
    /* .. compare crc / size .. */
  }

  if (run_flags & PFLAG_QUIET)
    return (0);

  /* report file status */
  if (run_flags & PFLAG_VERBOSE) {
    time_t stamp;

    stamp = shz_mod_mtime(z, f_idx);
    fprintf(sharetool_fout, "\t%s (page #%u [%s] %-20.20s)\n",
        rel_path, (unsigned int)f_idx,
        shz_mod_label(z, f_idx), (ctime(&stamp) + 4));
  } else {
    fprintf(sharetool_fout, "\t%s\n", rel_path);
  }

  return (0);
}

int sharetool_archive_extract(char *path)
{
  shz_t *z;
  shbuf_t *buff;
  struct stat st;
  int err;

  z = shz_fopen(path, 0);
  if (!z)
    return (SHERR_NOENT);

  err = shz_list(z, NULL, NULL, sharetool_archive_write);  
  shz_free(&z);
  if (err)
    return (err);

  return (0);
}

int sharetool_archive(char **args, int arg_cnt)
{
  shz_t *z;
  sharch_t *arch;
  struct stat st;
  shbuf_t *buff;
  char path[PATH_MAX+1];
  int idx;
  int err;
  int i;

  if (arg_cnt < 2) {
    fprintf(sharetool_fout, "error: no archive specified.\n");
    return (SHERR_INVAL);
  }

  if (!(run_flags & PFLAG_DECODE) &&
      !(run_flags & PFLAG_VERIFY)) { /* create archive */

    memset(path, 0, sizeof(path));
    strncpy(path, args[1], sizeof(path)-1);

    z = shz_fopen(path, SHZ_TRUNC | SHZ_CREATE);
    if (!z) {
      return (SHERR_ACCESS);
    }

    for (idx = 2; idx < arg_cnt; idx++) {
      err = shz_file_add(z, args[idx]);
      if (!err) {
        if (!(run_flags & PFLAG_QUIET)) {
          fprintf(sharetool_fout, "\t%s\n", args[idx]);
        }
      } else {
        fprintf(sharetool_fout, "error: %s: %s\n", args[idx], sherrstr(err));

        if (!(run_flags & PFLAG_IGNORE)) {
          /* abort upon first error */
          shz_free(&z);
          unlink(path);
          return (err);
        }
      }
    }

    shz_free(&z);

  } else { /* extract archive(s) */

    /* treat each filename specified as an SHZ archive. */
    for (idx = 1; idx < arg_cnt; idx++) {
      err = sharetool_archive_extract(args[idx]);
      if (err) {
        if (!(run_flags & PFLAG_IGNORE)) {
          /* abort upon first error */
          return (err);
        }
      }
    }

  }


  return (0);
}


