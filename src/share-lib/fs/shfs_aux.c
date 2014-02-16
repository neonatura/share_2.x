

/*
 * @copyright
 *
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
 *
 *  @endcopyright
 */

#include "share.h"

int shfs_aux_write(shfs_ino_t *file, shbuf_t *buff)
{
  shfs_ino_t *aux;
  int err;

  if (!IS_INODE_CONTAINER(file->blk.hdr.type)) {
    PRINT_RUSAGE("shfs_aux_write: non-container parent.");
    return (SHERR_NOTDIR);
  }

  aux = shfs_inode(file, NULL, SHINODE_AUX);
  if (!aux)
    return (SHERR_IO);

  err = shfs_inode_write(aux, buff);
  if (err)
    return (err);

  return (0);
}

int shfs_aux_read(shfs_ino_t *file, shbuf_t *buff)
{
  shfs_ino_t *aux;
  int err;

  if (!IS_INODE_CONTAINER(file->blk.hdr.type)) {
    PRINT_RUSAGE("shfs_aux_write: non-container parent.");
    return (SHERR_NOTDIR);
  }

 aux = shfs_inode(file, NULL, SHINODE_AUX);
  if (!aux)
    return (SHERR_IO);

  err = shfs_inode_read(aux, buff);
  if (err)
    return (err);

  return (0);
}

uint64_t shfs_aux_crc(shfs_ino_t *file)
{
  shfs_ino_t *aux;
  int err;

 aux = shfs_inode(file, NULL, SHINODE_AUX);
  if (!aux)
    return (SHERR_IO);

  err = shfs_inode_read_crc(aux);
  if (err)
    return (err);

  return (0);
}


