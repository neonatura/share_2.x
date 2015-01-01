
/*
 *  Copyright 2013 Neo Natura
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


int shfs_ref_read(shfs_ino_t *file, shfs_ref_t *ref_p, shfs_block_t *blk_p)
{
  shfs_ino_t *inode;
  shfs_ref_t ref;
  int err;

  if (!file)
    return (SHERR_INVAL);

  if (shfs_format(file) != SHINODE_REFERENCE)
    return (SHERR_INVAL);

  inode = shfs_inode(file, NULL, SHINODE_REFERENCE);
  if (!inode)
    return (SHERR_IO);

  if (shfs_size(inode) < sizeof(shfs_ref_t))
    return (SHERR_IO);

  memcpy(&ref, (char *)inode->blk.raw, sizeof(shfs_ref_t));

  if (0 != shkey_cmp(shpeer_kpub(&ref.ref_peer), 
        shpeer_kpub(&file->tree->peer))) {
    return (SHERR_OPNOTSUPP);
  }

  if (ref_p) {
    memcpy(ref_p, &ref, sizeof(shfs_ref_t));
  }

  if (blk_p) {
    err = shfs_inode_read_block(file->tree, &ref.ref_pos, blk_p);
    if (err)
      return (err);
  }

  return (0);
}

int shfs_ref_write(shfs_ino_t *file, shfs_ref_t *ref)
{
  shfs_ino_t *inode;
  int err;

  if (!file)
    return (SHERR_INVAL);

  inode = shfs_inode(file, NULL, SHINODE_REFERENCE);
  if (!inode)
    return (SHERR_IO);

  memcpy((char *)inode->blk.raw, ref, sizeof(shfs_ref_t));
  inode->blk.hdr.size = sizeof(shfs_ref_t);
  inode->blk.hdr.crc = shcrc(ref, sizeof(shfs_ref_t));
  err = shfs_inode_write_entity(inode);
  if (err)
    return (err);

  /* copy aux stats to file inode. */
  file->blk.hdr.mtime = inode->blk.hdr.mtime;
  file->blk.hdr.size = inode->blk.hdr.size;
  file->blk.hdr.crc = inode->blk.hdr.crc;
  file->blk.hdr.format = SHINODE_REFERENCE;
  file->blk.hdr.attr |= SHATTR_LINK;

  return (0);
}

int shfs_ref_set(shfs_ino_t *file, char *path)
{
  shfs_ino_t *ref_file;
  shfs_ref_t ref;

  if (!file || !file->tree)
    return (SHERR_INVAL);

  ref_file = shfs_file_find(file->tree, path);
  if (!ref_file)
    return (SHERR_IO);

  memset(&ref, 0, sizeof(ref));
  ref.ref_ver = SHFS_REFERENCE_VERSION;
  memcpy(&ref.ref_peer, &file->tree->peer, sizeof(shpeer_t));
  memcpy(&ref.ref_pos, &file->blk.hdr.pos, sizeof(shfs_idx_t));
  return (shfs_ref_write(file, &ref));  
}


