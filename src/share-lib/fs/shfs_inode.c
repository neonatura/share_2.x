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
  static struct shfs_ino_t *ent;

  ent = (shfs_ino_t *)calloc(1, sizeof(shfs_ino_t));
  ent->hdr.d_type = mode;
  if (name) {
    strncpy(ent->d_raw.name, name, sizeof(ent->d_raw.name) - 1);
  }

  return (ent);
}

_TEST(shfs_inode)
{
  shfs_t *tree;
  shfs_ino_t *root;
  shfs_ino_t *dir;
  shfs_ino_t *file;
  shfs_ino_t *ref;

  _TRUEPTR(tree = shfs_init(NULL, 0));
  _TRUEPTR(root = tree->base_ino);
  _TRUEPTR(dir = shfs_inode(root, "shfs_inode", SHINODE_DIRECTORY));
  _TRUEPTR(file = shfs_inode(dir, "shfs_inode", 0));
  _TRUEPTR(ref = shfs_inode(file, "ref_shfs_inode", SHINODE_REFERENCE));

  shfs_free(&tree);
}

int shfs_inode_write_block(shfs_t *tree, shfs_hdr_t *scan_hdr, shfs_hdr_t *hdr, char *data, size_t data_len)
{
  shfs_journal_t *jrnl;
  shfs_ino_t *jnode;
  char *seg;

  jrnl = shfs_journal_open(tree, (int)scan_hdr->d_jno);
  if (!jrnl)
    return (-1);

  /* journal inode entry */
  jnode = (shfs_ino_t *)jrnl->data->block[scan_hdr->d_ino];

  /* fill header */
  memcpy(&jnode->hdr, hdr, sizeof(shfs_inode_hdr_t));

  /* fill data */
  data_len = MIN(SHFS_BLOCK_DATA_SIZE, data_len);
  memcpy(jnode->d_raw.bin, data, data_len);

  shfs_journal_close(tree, &jrnl);
  return (data_len);
}

_TEST(shfs_inode_write_block)
{
  shfs_t *tree;
  shfs_ino_t *root;
  shfs_ino_t *file;
  shfs_ino_t blk;

  _TRUEPTR(tree = shfs_init(NULL, 0));
  _TRUEPTR(root = tree->base_ino);
  _TRUEPTR(file = shfs_inode(root, "version", 0));

  memset(&blk, 0, sizeof(blk));
  _TRUE(-1 != shfs_inode_write_block(tree, &file->hdr, &blk.hdr, VERSION, strlen(VERSION)));
  _TRUE(!shfs_inode_read_block(tree, &file->hdr, &blk));
  _TRUE(blk.hdr.d_size == strlen(VERSION));
  if (blk.hdr.d_size == strlen(VERSION)) {
    _TRUE(0 == strncmp(blk.d_raw.bin, VERSION, strlen(VERSION)));
  }
}

ssize_t shfs_inode_write(shfs_t *tree, shfs_ino_t *inode,
    char *data, size_t data_of, size_t data_len)
{
  shfs_hdr_t *ihdr;
  shfs_hdr_t cur_hdr;
  shfs_hdr_t hdr;
  size_t b_of;
  size_t b_len;
  int ino_nr;

  if (!inode->hdr.d_stamp) {
    /* no inode reference to a data segment has been created. */
    inode->hdr.d_jno = shfs_journal_index(inode);
  } 

  memset(&hdr, 0, sizeof(hdr));
  hdr.d_stamp = shtime64();

  b_of = 0;
  ihdr = &hdr;
  memcpy(&hdr, &inode->hdr, sizeof(hdr));
  while (b_of < data_len) {
    memcpy(&cur_hdr, &hdr, sizeof(cur_hdr));
    
    ino_nr = shfs_journal_scan(tree, inode->hdr.d_jno);
    if (ino_nr == -1)
      break;

    /* fill in location of next data entry */
    hdr.d_size = data_len - b_of;
    hdr.d_jno = inode->hdr.d_jno;
    hdr.d_ino = ino_nr;

    b_len = shfs_inode_write_block(tree, &cur_hdr, &hdr,
        data + b_of, (size_t)hdr.d_size);
    if (b_len < 1)
      break;

    b_of += b_len;
  }

  return (b_of);
}

_TEST(shfs_inode_write)
{
  shfs_t *tree;
  shfs_ino_t *root;
  shfs_ino_t *file;
  shbuf_t *buff;

  _TRUEPTR(tree = shfs_init(NULL, 0));
  _TRUEPTR(root = tree->base_ino);
  _TRUEPTR(file = shfs_inode(root, "version", 0));
  _TRUE(strlen(VERSION) == shfs_inode_write(tree, file, VERSION, 0, strlen(VERSION)));
  _TRUE(file->hdr.d_size == strlen(VERSION));

  _TRUEPTR(buff = shbuf_init());
  _TRUE(strlen(VERSION) == shfs_inode_read(tree, file, buff, 0, file->hdr.d_size));

  shbuf_free(&buff);
  shfs_free(&tree);
}

int shfs_inode_read_block(shfs_t *tree, shfs_hdr_t *hdr, shfs_ino_t *inode)
{
  shfs_journal_t *jrnl;

  jrnl = shfs_journal_open(tree, (int)hdr->d_jno);
  if (!jrnl)
    return (-1);

  memcpy(inode, (char *)jrnl->data->block[hdr->d_ino], sizeof(shfs_ino_t));
  shfs_journal_close(tree, &jrnl);

  return (0);
}

_TEST(shfs_inode_read_block)
{
  shfs_t *tree;
  shfs_ino_t *root;
  shfs_ino_t *file;
  shfs_ino_t blk;

  /* obtain file reference. */
  _TRUEPTR(tree = shfs_init(NULL, 0));
  _TRUEPTR(root = tree->base_ino);
  _TRUEPTR(file = shfs_inode(root, "version", 0));

  _TRUE(strlen(VERSION) == shfs_inode_write(tree, file, VERSION, 0, strlen(VERSION)));
  _TRUE(!shfs_inode_read_block(tree, &file->hdr, &blk));
  _TRUE(blk.hdr.d_size == strlen(VERSION));
  if (blk.hdr.d_size == strlen(VERSION)) {
    _TRUE(0 == strncmp(blk.d_raw.bin, VERSION, strlen(VERSION)));
  }

}

ssize_t shfs_inode_read(shfs_t *tree, shfs_ino_t *inode, 
    shbuf_t *ret_buff, size_t data_of, size_t data_len)
{
  shfs_hdr_t hdr;
  shfs_ino_t blk;
  size_t blk_max;
  size_t blk_nr;
  size_t b_of;
  size_t b_len;
  size_t data_max;
  int err;

  memcpy(&hdr, &inode->hdr, sizeof(hdr));
  data_max = (data_len / SHFS_BLOCK_DATA_SIZE);
  while (b_of < data_max) {
    b_len = shfs_inode_read_block(tree, &hdr, &blk); 
    if (b_len < 1)
      break;

//    if (data_of < DEBUG: 
    shbuf_cat(ret_buff, &blk.d_raw.bin, MIN(SHFS_BLOCK_DATA_SIZE, blk.hdr.d_size));

    b_of += b_len;
  }

  return (b_of);
}

_TEST(shfs_inode_read)
{
  shfs_t *tree;
  shfs_ino_t *root;
  shfs_ino_t *file;
  shbuf_t *buff;
  char *data;

  /* obtain file reference. */
  _TRUEPTR(tree = shfs_init(NULL, 0));
  _TRUEPTR(root = tree->base_ino);
  _TRUEPTR(file = shfs_inode(root, "version", 0));

  /* read file data. */
  _TRUEPTR(buff = shbuf_init());
  _TRUE(strlen(VERSION) == shfs_inode_write(tree, file, VERSION, 0, strlen(VERSION)));
  _TRUE(strlen(VERSION) == shfs_inode_read(tree, file, buff, 0, file->hdr.d_size));
  _TRUEPTR(data);

  shbuf_free(&buff);
  shfs_free(&tree);
}



