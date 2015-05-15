
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

typedef struct shmime_t
{
  /* standard ISO/IEC 15444 mime-type name */
  char mime_name[MAX_SHARE_NAME_LENGTH];
  /* file-spec pattern */
  char mime_pattern[SHFS_PATH_MAX];
  /** the file's binary header prefix. */
  char mime_header[32];
  /** total byte length of binary header prefix */
  uint32_t mime_header_len;
  /** total number of file content definitions. */
  uint32_t mime_def_len;
  /* file content definition(s). */
  shmime_def_t mime_def[0]; 
} shmime_t;

/** Add a file meta definition to a sharefs file-system */ 
int shmime_add(shfs_t *fs, shmime_t *mime)
{
}
int shpkg_file_add(shpkg_t *pkg, char *mime, SHFL *fl)
{
}
