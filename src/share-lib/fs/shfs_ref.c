
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


int shfs_ref_read(shfs_ino_t *file, shbuf_t *buff)
{
  shfs_ino_t *inode;
  int err;

  if (!file)
    return (SHERR_INVAL);

  if (shfs_format(file) != SHINODE_REFERENCE)
    return (SHERR_INVAL);

  inode = shfs_inode(file, NULL, SHINODE_REFERENCE);
  if (!inode)
    return (SHERR_IO);

  if (inode->blk.hdr.size < (sizeof(shpeer_t) + sizeof(shkey_t)) ||
      inode->blk.hdr.size > SHFS_BLOCK_DATA_SIZE)
    return (SHERR_IO);

  shbuf_cat(buff, (char *)inode->blk.raw, inode->blk.hdr.size);
  return (0);
}

int shfs_ref_write(shfs_ino_t *file, shbuf_t *buff)
{
  shfs_ino_t *inode;
  int err;

  if (!file)
    return (SHERR_INVAL);

  inode = shfs_inode(file, NULL, SHINODE_REFERENCE);
  if (!inode)
    return (SHERR_IO);

  if (shbuf_size(buff) > SHFS_BLOCK_DATA_SIZE) {
    return (SHERR_TOOMANYREFS);
  }

  memset((char *)inode->blk.raw, 0, SHFS_BLOCK_DATA_SIZE);
  memcpy((char *)inode->blk.raw, shbuf_data(buff), shbuf_size(buff));
  inode->blk.hdr.size = shbuf_size(buff);
  inode->blk.hdr.crc = shcrc(shbuf_data(buff), shbuf_size(buff));
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

int shfs_ref_set(shfs_ino_t *file, shfs_ino_t *ref_file)
{
  shfs_ino_t *parent;
  shbuf_t *buff;
  int err;
  int i;

  if (!file || !file->tree)
    return (SHERR_INVAL);

  buff = shbuf_init();
  shbuf_cat(buff, &file->tree->peer, sizeof(shpeer_t));
  
  parent = ref_file;
  for (i = 0; i < SHFS_MAX_REFERENCE_HIERARCHY; i++) {
    if (parent) {
      shbuf_cat(buff, &parent->blk.hdr.name, sizeof(shkey_t));
      parent = shfs_inode_parent(parent);
    }

    if (shkey_cmp(shfs_token(parent), shfs_token(ref_file->tree->base_ino))) {
      parent = NULL;
    }
  }

  err = shfs_ref_write(file, buff);
  shbuf_free(&buff);
  if (err)
    return (err);

  return (0);
}

int shfs_ref_get(shfs_ino_t *file,
    shfs_t **ref_fs_p, shfs_ino_t **ref_p)
{
  shfs_ino_t *ref;
  shfs_ino_t *parent;
  shpeer_t *peer;
  shfs_t *fs;
  shkey_t *hier;
  shbuf_t *buff;
  char path[SHFS_PATH_MAX];
  int hier_cnt;
  int err;
  int i;

  *ref_p = NULL;
  *ref_fs_p = NULL;

  if (!file || !file->tree)
    return (SHERR_INVAL);

  buff = shbuf_init();
  err = shfs_ref_read(file, buff);
  if (err)
    return (err);

  peer = (shpeer_t *)shbuf_data(buff);
  hier = (shkey_t *)(shbuf_data(buff) + sizeof(shpeer_t));

  fs = shfs_init(peer);
  if (!fs)
    return (SHERR_IO);

  memset(path, 0, sizeof(path));
  strcpy(path, "/");
  ref = fs->base_ino;
  for (i = SHFS_MAX_REFERENCE_HIERARCHY - 1; i >= 0; i--) {
    if (shkey_cmp(&hier[i], ashkey_blank()))
      continue;

    ref = shfs_inode_load(ref, &hier[i]);
    if (!ref) {
      shfs_free(&fs);
      return (SHERR_NOENT);
    }

    if (shfs_type(ref) == SHINODE_DIRECTORY)
      strncat(path, "/", SHFS_PATH_MAX - strlen(path) - 1);
    strncat(path, shfs_filename(ref), SHFS_PATH_MAX - strlen(path) - 1);
  }

  shbuf_free(&buff);

  *ref_p = ref;
  *ref_fs_p = fs;

  return (0);
}


