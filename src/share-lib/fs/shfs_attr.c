


/*
 * @copyright
 *
 *  Copyright 2013 Brian Burrell 
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
 *
 *  @endcopyright
 */

#include "share.h"


#define MAX_INODE_ATTRIBUTES 16 
static char *_shfs_inode_attr_labels[MAX_INODE_ATTRIBUTES] = 
{
  "Arch",
  "Block",
  "Compress",
  "DB",
  "Encode",
  "FLock",
  "Link",
  "Meta",
  "Owner",
  "Read",
  "Sync",
  "Temp",
  "User",
  "Version",
  "Write",
  "Exe"
};

char *shfs_attr_label(int attr_idx)
{

  if (attr_idx < 0 || attr_idx >= MAX_INODE_ATTRIBUTES)
    return ("Unknown");

  return (_shfs_inode_attr_labels[attr_idx]);
}

shfs_attr_t shfs_block_attr(shfs_block_t *blk)
{
  if (!blk)
    return (0);
  return (blk->hdr.attr);
}

shfs_attr_t shfs_attr(shfs_ino_t *inode)
{
  if (!inode)
    return (0);
  return (shfs_block_attr(&inode->blk));
}

char *shfs_attr_str(shfs_attr_t attr)
{
  static char ret_str[256];
  const char *bits = (const char *)SHFS_ATTR_BITS;
  int i;

  memset(ret_str, 0, sizeof(ret_str));
  for (i = 0; i < 16; i++) {
    if (attr & (1 << i)) {
      if (!*ret_str)
        strcat(ret_str, "+");
      sprintf(ret_str + strlen(ret_str), "%c", bits[i]);
    }
  }

  while (strlen(ret_str) < 8)
    strcat(ret_str, "-");

  return (ret_str);
}

int shfs_attr_set(shfs_ino_t *file, int attr)
{
  shfs_attr_t cur_flag;
  int err_code;

  if (!file || !attr)
    return (SHERR_INVAL);

  cur_flag = shfs_attr(file);
  if (cur_flag & attr)
    return (0); /* already set */

  err_code = SHERR_OPNOTSUPP;
  switch (attr) {
    case SHATTR_COMP:
      err_code = shfs_format_set(file, SHINODE_COMPRESS);
      break;
    case SHATTR_SYNC:
      err_code = shfs_file_notify(file);
      break;
    case SHATTR_TEMP:
      err_code = 0;
      break;
    case SHATTR_VER:
      err_code = shfs_rev_init(file);
      break;
  }

  if (!err_code) {
    file->blk.hdr.attr |= attr;
    err_code = shfs_inode_write_entity(file);
  }

  if (!err_code && (file->blk.hdr.attr & SHATTR_SYNC))
    shfs_file_notify(file);

  return (err_code);
}

int shfs_attr_unset(shfs_ino_t *file, int attr)
{
  shfs_attr_t cur_attr;
  shfs_attr_t new_attr;
  int err_code;
  int format;

  if (!file || !attr)
    return (SHERR_INVAL);

  cur_attr = shfs_attr(file);
  if (!(cur_attr & attr))
    return (0); /* already unset */

  new_attr = cur_attr;
  new_attr &= ~attr;

  err_code = SHERR_OPNOTSUPP;
  switch (attr) {
    case SHATTR_COMP:
      err_code = 0;
      format = SHINODE_DEFAULT_ATTR_FORMAT(new_attr);
      if (format != shfs_format(file))
        err_code = shfs_format_set(file, format); 
      break;
    case SHATTR_SYNC:
      err_code = 0;
      break;
    case SHATTR_TEMP:
      err_code = 0;
      break;
    case SHATTR_VER:
      err_code = shfs_rev_clear(file);
      break;
    case SHATTR_LINK:
      /* unimplemented. */
      err_code = SHERR_OPNOTSUPP;
      /* this is now a local copy */
      file->blk.hdr.attr &= ~SHATTR_SYNC; 
      break;
  }

  if (!err_code) {
    file->blk.hdr.attr = new_attr;
    err_code = shfs_inode_write_entity(file);
  }

  if (!err_code && (file->blk.hdr.attr & SHATTR_SYNC))
    shfs_file_notify(file);

  return (err_code);
}


