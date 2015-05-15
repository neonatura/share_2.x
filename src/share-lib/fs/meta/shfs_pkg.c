
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

typedef struct shpkg_t
{
  /** The unique name of the package */
  char pkg_name[MAX_SHARE_NAME_LENGTH];
  /** The certificate used to license extracted files. */
  shcert_t pkg_cert;
  /** The originating peer which generated the package. */
  shpeer_t pkg_peer;
  /** The time-stamp of when the package was updated. */
  shtime_t pkg_stamp;
} shpkg_t;

/** Add a file meta definition to a sharefs file-system */ 
int shmime_add(shfs_t *fs, shmime_t *mime)
{
}
int shpkg_file_add(shpkg_t *pkg, char *mime, SHFL *fl)
{
}
