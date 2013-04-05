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


shfs_ino_t *shfs_inode(shfs_ino_t *parent, char *name, int mode)
{
  static struct shfs_ino_t ent;

  memset(&ent, 0, sizeof(ent));
  ent.d_type = mode;
  strncpy(ent.d_raw.name, name, sizeof(ent.d_raw.name) - 1);

  return (&ent);
}

int shfs_inode_data(shfs_t *tree, shfs_ino_t *inode, 
    char **data_p, shfs_size_t data_of, shfs_size_t data_len)
{
  shfs_size_t of;
  shfs_inode_hdr_t hdr;
  shfs_ino_t raw;
  shfs_journal_t *jrnl;
  shbuf_t *ret_data;
  char *data_ptr;
  char *blk;
  int err;

  data_len = MIN(inode->d_size, data_len);

  ret_data = NULL;
  memset(&hdr, 0, sizeof(hdr));
  hdr.d_jno = inode->d_jno;
  hdr.d_ino = inode->d_ino;
  for (of = 0; of < inode->d_size; of++) {
    jrnl = shfs_journal(tree, hdr.d_jno);
    data_ptr = (jrnl->data + (hdr.d_ino * SHFS_BLOCK_SIZE));
    shbuf_cat(ret_data, data_ptr + sizeof(shfs_inode_hdr_t), 
        MAX(inode->d_size - of, SHFS_BLOCK_SIZE - sizeof(shfs_inode_hdr_t)));
  }

  if (!ret_data)
    return (-1); /* does not exist */

  *data_p = ret_data->data;
  free(ret_data);

  return (0);
}


