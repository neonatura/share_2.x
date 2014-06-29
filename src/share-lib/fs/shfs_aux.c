

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



int shfs_aux_write(shfs_ino_t *inode, shbuf_t *buff)
{
  shfs_block_t blk;
  shfs_block_t nblk;
  shfs_idx_t *idx;
  shkey_t *key;
  size_t b_len;
  size_t b_of;
  uint64_t seg_crc;
  int err;
  int jno;

  if (!buff)
    return (0);

  b_of = 0;
  idx = &inode->blk.hdr.fpos; /* first data segment of inode */

  memset(&blk, 0, sizeof(blk));
  if (!idx->ino) {
    /* create first block. */
    err = shfs_journal_scan(inode->tree, &inode->blk.hdr.name, idx);
    if (err)
      return (err);

    blk.hdr.type = SHINODE_AUX;
    memcpy(&blk.hdr.pos, idx, sizeof(shfs_idx_t));
    blk.hdr.ctime = shtime64();

    key = shkey_bin((char *)&inode->blk, sizeof(shfs_block_t));
    memcpy(&blk.hdr.name, key, sizeof(shkey_t)); 
    shkey_free(&key);
  } else {
    /* read in existing initial data segment. */
    err = shfs_inode_read_block(inode->tree, idx, &blk);
    if (err)
      return (err);
  }

  seg_crc = 0;
  while (blk.hdr.pos.ino) {
    b_len = MIN(SHFS_BLOCK_DATA_SIZE, buff->data_of - b_of);

    idx = &blk.hdr.npos;
    memset(&nblk, 0, sizeof(nblk));

    /* retrieve next block reference. */
    if (!idx->ino) {
      /* create new block if data pending */
      if ((b_of + b_len) < buff->data_of) {
        err = shfs_journal_scan(inode->tree, &blk.hdr.name, idx);
        if (err)  
          return (err);

        nblk.hdr.type = SHINODE_AUX;
        memcpy(&nblk.hdr.pos, idx, sizeof(shfs_idx_t));
        nblk.hdr.ctime = shtime64();

        key = shkey_bin((char *)&blk, sizeof(shfs_block_t));
        memcpy(&nblk.hdr.name, key, sizeof(shkey_t)); 
        shkey_free(&key);
      }
    } else {
      err = shfs_inode_read_block(inode->tree, idx, &nblk);
      if (err)
        return (err);
    }

    memset(blk.raw, 0, SHFS_BLOCK_DATA_SIZE);
    if (b_len) {
      memcpy(blk.raw, buff->data + b_of, b_len);
    }

    blk.hdr.size = b_len;
    blk.hdr.crc = shfs_inode_crc(&blk);
    seg_crc += blk.hdr.crc;

    err = shfs_inode_write_block(inode->tree, &blk);
    if (err)
      return (err);

    b_of += b_len;
    memcpy(&blk, &nblk, sizeof(shfs_block_t));
  }

  /* write the inode to the parent directory */
  inode->blk.hdr.crc = seg_crc;
  inode->blk.hdr.size = buff->data_of;
  err = shfs_inode_write_entity(inode); 
  if (err) {
    PRINT_RUSAGE("shfs_inode_write: error writing entity.");
    return (err);
  }

  return (0);
}


int shfs_aux_read(shfs_ino_t *inode, shbuf_t *ret_buff)
{
  shfs_hdr_t hdr;
  shfs_block_t blk;
  shfs_idx_t idx;
  size_t blk_max;
  size_t blk_nr;
  size_t b_of;
  size_t b_len;
  size_t data_len;
  size_t data_max;
  int err;

  data_len = inode->blk.hdr.size;

  b_of = 0;
  memcpy(&idx, &inode->blk.hdr.fpos, sizeof(shfs_idx_t));
  while (idx.ino) {
    memset(&blk, 0, sizeof(blk)); 
    err = shfs_inode_read_block(inode->tree, &idx, &blk);
    if (err)
      return (err);

    b_len = MIN(SHFS_BLOCK_DATA_SIZE, data_len - b_of);
    shbuf_cat(ret_buff, blk.raw, b_len);

    b_of += b_len;
    memcpy(&idx, &blk.hdr.npos, sizeof(shfs_idx_t));
  }

  return (0);
}

_TEST(shfs_aux_read)
{
  shfs_t *tree;
  shfs_ino_t *inode;
  shfs_hdr_t hdr;
  shfs_block_t blk;
  shfs_idx_t idx;
  size_t blk_max;
  size_t blk_nr;
  size_t b_of;
  size_t b_len;
  size_t data_len;
  size_t data_max;
  char buf[1024];
  int err;

  /* write test file */
  memset(buf, 0, sizeof(buf));
  memset(buf, '0', 512);
  tree = shfs_init(shpeer());
  _TRUEPTR(tree);
  inode = shfs_file_find(tree, "/test/aux"); 
  _TRUE(0 == shfs_file_write(inode, buf, sizeof(buf)));
  shfs_free(&tree);

  /* mimic file read */
  tree = shfs_init(shpeer());
  inode = shfs_file_find(tree, "/test/aux"); 
  data_len = inode->blk.hdr.size;

  b_of = 0;
  memcpy(&idx, &inode->blk.hdr.fpos, sizeof(shfs_idx_t));
  while (idx.ino) {
    memset(&blk, 0, sizeof(blk)); 
    _TRUE(0 == shfs_inode_read_block(inode->tree, &idx, &blk));
    _TRUE(blk.hdr.pos.ino != 0);
    _TRUE(!(blk.hdr.npos.jno == idx.jno && blk.hdr.npos.ino == idx.ino));

    b_len = MIN(SHFS_BLOCK_DATA_SIZE, data_len - b_of);

    b_of += b_len;
    memcpy(&idx, &blk.hdr.npos, sizeof(shfs_idx_t));
  }

  shfs_free(&tree);
}



ssize_t shfs_aux_pipe(shfs_ino_t *inode, int fd)
{
  shfs_t *tree = inode->tree;
  char hier[NAME_MAX + 1];
  char dir[NAME_MAX + 1];
  char *n_tok;
  char *tok;
  char *data;
  size_t data_len;
  shbuf_t *buff;
  ssize_t b_len;
  ssize_t b_of;
  int err;

  buff = shbuf_init();
  err = shfs_aux_read(inode, buff);
  if (err == -1) {
    shbuf_free(&buff);
    return (err);
  }

  for (b_of = 0; b_of < buff->data_of; b_of++) {
    b_len = write(fd, buff->data + b_of, buff->data_of - b_of);
    if (b_len < 1)
      return (b_len);
    b_of += b_len;
  }

  shbuf_free(&buff);

//  printf ("Wrote %lu bytes to file descriptor %d.\n", (unsigned long)data_len, fd);

  return (0);
}

uint64_t shfs_aux_crc(shfs_ino_t *inode)
{
  shfs_hdr_t hdr;
  shfs_block_t blk;
  shfs_idx_t idx;
  uint64_t crc;
  int err;
  int i;

  crc = 0;
  memcpy(&idx, &inode->blk.hdr.fpos, sizeof(shfs_idx_t));
  while (idx.ino) {
    memset(&blk, 0, sizeof(blk)); 
    err = shfs_inode_read_block(inode->tree, &idx, &blk);
    if (err)
      return (err);

    crc += blk.hdr.crc;

    memcpy(&idx, &blk.hdr.npos, sizeof(shfs_idx_t));
  }

  return (crc);
}


//uint64_t shfs_aux_crc(shfs_ino_t *file) {}
