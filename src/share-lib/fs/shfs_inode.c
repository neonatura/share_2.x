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
  struct shfs_ino_t *ent = NULL;

  /* check parent's cache */
  if (parent) {
    ent = shmeta_get_void(parent->child, shkey_str(name));
    if (ent) {
#if 0
/*DEBUG: ensure node is correct */
      if (!(mode & ent->hdr.d_type)) {
fprintf(stderr, "DEBUG: invalid mode specified; '%s' is mode %d, but user specified %d\n", name, ent->hdr.flags, mode);
        return (null); 
      }
#endif
    }
  }

  if (!ent) {
    ent = (shfs_ino_t *)calloc(1, sizeof(shfs_ino_t));
    ent->hdr.d_type = mode;
    if (name) {
      strncpy(ent->d_raw.name, name, sizeof(ent->d_raw.name) - 1);
    }

    if (parent) {
      ent->parent = parent;
      ent->base = parent->base;
      ent->tree = parent->tree;
    } else {
      ent->base = ent;
    }

    if (parent)
      shmeta_set_void(parent->child, shkey_str(name), ent, sizeof(shfs_ino_t));
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
  _TRUE(!root->parent);
  _TRUEPTR(root->tree);
  _TRUEPTR(dir = shfs_inode(root, "shfs_inode", SHINODE_DIRECTORY));
  _TRUEPTR(file = shfs_inode(dir, "shfs_inode", 0));
  _TRUEPTR(ref = shfs_inode(file, "ref_shfs_inode", SHINODE_REFERENCE));
  _TRUEPTR(ref->tree);
  _TRUEPTR(ref->base);
  _TRUEPTR(ref->parent);

  shfs_free(&tree);
}

/**
 * Link a child inode inside a parent's directory listing.
 */
int shfs_inode_link(shfs_ino_t *parent, shfs_ino_t *inode)
{
  shfs_ino_t blk;
  shfs_hdr_t last_hdr;
  shfs_hdr_t hdr;
  size_t b_of;
  size_t b_max;
  size_t b_len;
  int err;

  if (!parent) {
    PRINT_RUSAGE("shfs_inode_link: null parent");
    return (SHERR_NOENT);
  }

#if 0
/* inode has no identity */
  if (shfs_partition_id(parent->tree) != shfs_partition_id(inode->tree)) {
    PRINT_RUSAGE("shfs_inode_link: unique partitions.");
    /* Attempting to link two different partitions. */
    /* DEBUG: need special SHINODE to allow this */
    return (SHERR_BADF);
  }
#endif

  if (!(parent->hdr.d_type & SHINODE_DIRECTORY)) {
    PRINT_RUSAGE("shfs_inode_link: non-directory parent.");
    return (SHERR_NOTDIR);
  }

  /* find existing link */
  memset(&last_hdr, 0, sizeof(last_hdr));
  memcpy(&hdr, &parent->hdr, sizeof(hdr));
  memcpy(&blk, parent, sizeof(blk));
  b_max = parent->hdr.d_size / SHFS_BLOCK_SIZE;
  for (b_of = 0; b_of < b_max; b_of++) {
    memcpy(&last_hdr, &hdr, sizeof(last_hdr));

    memset(&blk, 0, sizeof(blk));
    err = shfs_inode_read_block(inode->tree, &hdr, &blk);
    if (err)
      return (err);

    if (0 == strncmp(inode->d_raw.name, blk.d_raw.name, sizeof(inode->d_raw.name))) {
      /* found existing link */
      return (0);
    } 

    memcpy(&hdr, &blk.hdr, sizeof(hdr));
  }

  /* mark as last entry. */
  inode->hdr.d_jno = 0;
  inode->hdr.d_ino = 0;

  /* add to directory list. */
  err = shfs_inode_write_block(parent->tree, &blk.hdr, &inode->hdr, inode->d_raw.name, SHFS_BLOCK_DATA_SIZE);
  if (err) {
fprintf(stderr, "DEBUG: %d = shfs_inode_write_block( [%d:%d], [%d:%d], %x )\n", blk.hdr.d_jno, blk.hdr.d_ino, inode->hdr.d_jno, inode->hdr.d_ino);
    PRINT_RUSAGE("shfs_inode_link: error on directory append.");
    return (err);
  }

  return (0);
}

_TEST(shfs_inode_link)
{
  shfs_t *tree;
  shfs_ino_t *file;

  _TRUEPTR(tree = shfs_init(NULL, 0));
  _TRUEPTR(tree->base_ino);
  if (!tree || !tree->base_ino)
    return;

  _TRUEPTR(file = shfs_inode(NULL, "sfs_inode_link", 0));
  _TRUE(!shfs_inode_link(tree->base_ino, file));

  shfs_free(&tree);
}

shfs_t *shfs_inode_tree(shfs_ino_t *inode)
{
  if (!inode)
    return (NULL);
  return (inode->tree);
}

_TEST(shfs_inode_tree)
{
  shfs_t *tree;

  _TRUEPTR(tree = shfs_init(NULL, 0));
  if (tree)
    _TRUEPTR(shfs_inode_tree(tree->base_ino));
  shfs_free(&tree);
}

shfs_ino_t *shfs_inode_parent(shfs_ino_t *inode)
{
  if (!inode)
    return (NULL);
  return (inode->parent);
}

shfs_ino_t *shfs_inode_root(shfs_ino_t *inode)
{
  if (!inode)
    return (NULL);
  return (inode->base);
}

int shfs_inode_write_entity(shfs_t *tree, shfs_ino_t *ent)
{
  return (shfs_inode_write_block(tree, &ent->parent->hdr, &ent->hdr, (char *)ent->d_raw.bin, SHFS_BLOCK_DATA_SIZE));
}

int shfs_inode_write_block(shfs_t *tree, shfs_hdr_t *scan_hdr, shfs_hdr_t *hdr, char *data, size_t data_len)
{
  shfs_journal_t *jrnl;
  shfs_ino_t *jnode;
  char *seg;
  int err;

  jrnl = shfs_journal_open(tree, (int)scan_hdr->d_jno);
  if (!jrnl) {
fprintf(stderr, "DEBUG: shfs_inode_write_block: error opening journal %d\n", scan_hdr->d_jno);
    PRINT_RUSAGE("shfs_inode_write_block: error opening journal");
    return (-1);
  }

  if (scan_hdr->d_ino > jrnl->data_max / SHFS_BLOCK_SIZE) {
    /* DEBUG: add unit test for this condition. */
    PRINT_RUSAGE("WARNING: shfs_inode_write_block: inode has not been allocated");
    return (-1);
  }

  /* journal inode entry */
  jnode = (shfs_ino_t *)jrnl->data->block[scan_hdr->d_ino];

  /* fill header */
  memcpy(&jnode->hdr, hdr, sizeof(shfs_inode_hdr_t));

  /* fill data */
  data_len = MIN(SHFS_BLOCK_DATA_SIZE, data_len);
  if (data)
    memcpy(jnode->d_raw.bin, data, data_len);

  err = shfs_journal_close(&jrnl);
  if (err) {
    PRINT_RUSAGE("shfs_inode_write_block: error closing journal.");
    return (err);
  }

  return (0);
}

ssize_t shfs_inode_write(shfs_t *tree, shfs_ino_t *inode, char *data, size_t data_of, size_t data_len)
{
  shfs_hdr_t last_hdr;
  shfs_hdr_t hdr;
  size_t b_of;
size_t b_len;
  int jno_nr;
  int ino_nr;
  int err;

  memset(&hdr, 0, sizeof(hdr));
  hdr.d_stamp = shtime64();

  data_len = MIN(SHFS_MAX_JOURNAL_SIZE, data_of + data_len);

  if (inode->hdr.d_ino == 0) {
    /* no inode reference to a data segment has been created. */
    jno_nr = shfs_journal_index(inode);
    ino_nr = shfs_journal_scan(tree, jno_nr);
    if (!ino_nr) {
      PRINT_RUSAGE("shfs_inode_write: no space available");
      return (-SHERR_FBIG);
    }

    /* assign first inode */
    memset(&last_hdr, 0, sizeof(last_hdr));
    inode->hdr.d_jno = jno_nr;
    inode->hdr.d_ino = ino_nr;
fprintf(stderr, "DEBUG: shfs_inode_write: initial inode '%d:%d' is %d bytes.\n", jno_nr, ino_nr, data_len);
  }

  b_of = 0;
  memcpy(&last_hdr, &inode->hdr, sizeof(hdr));
  while (b_of < data_len) {
    ino_nr = shfs_journal_scan(tree, last_hdr.d_jno);
    if (ino_nr == 0)
      return (-1);

    /* fill in location of next data entry */
    memset(&hdr, 0, sizeof(hdr));
    hdr.d_size = data_len - b_of;
    hdr.d_jno = last_hdr.d_jno;
    hdr.d_ino = ino_nr;

    err = shfs_inode_write_block(tree, &last_hdr, &hdr,
        data + b_of, (size_t)hdr.d_size);
    if (err)
      return (-1);


    b_len = MIN(hdr.d_size, SHFS_BLOCK_DATA_SIZE);
    b_of += b_len;
    memcpy(&last_hdr, &hdr, sizeof(last_hdr));
  }

  /* write the inode to the parent directory */
  inode->hdr.d_size = b_of;
  err = shfs_inode_write_entity(tree, inode); 
  if (err) {
    PRINT_RUSAGE("shfs_inode_write: error writing entity.");
    return (err);
  }
  
fprintf(stderr, "DEBUG: shfs_inode_write: wrote %lu bytes to inode %d:%d (header reports %lu bytes).\n", (unsigned long int )b_of, (int)inode->hdr.d_jno, (int)inode->hdr.d_ino, inode->hdr.d_size); 


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

  shfs_free(&tree);
}

int shfs_inode_read_block(shfs_t *tree, shfs_hdr_t *hdr, shfs_ino_t *inode)
{
  shfs_journal_t *jrnl;
  int err;

  jrnl = shfs_journal_open(tree, (int)hdr->d_jno);
  if (!jrnl) {
    PRINT_RUSAGE("shfs_inode_read_block: error opening journal.");
    return (-1);
  }

  memset(inode, 0, sizeof(shfs_ino_t));
  memcpy(inode, (char *)jrnl->data->block[hdr->d_ino], SHFS_BLOCK_SIZE);

  err = shfs_journal_close(&jrnl);
  if (err) {
    PRINT_RUSAGE("shfs_inode_read_block: error closing journal.");
    return (err);
  }

  return (0);
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

  if (inode->hdr.d_ino == 0) {
    PRINT_RUSAGE("shfs_inode_read: inode has null data.");
    return (0);
  }

  b_of = 0;
  memcpy(&hdr, &inode->hdr, sizeof(hdr));
  while (b_of < inode->hdr.d_size) {
fprintf(stderr, "DEBUG: shfs_inode_read: read offset %lu..\n", b_of);
    memset(&blk, 0, sizeof(blk));
    err = shfs_inode_read_block(tree, &hdr, &blk); 
    if (err) {
fprintf(stderr, "DEBUG: shfs_inode_read: block read from %d:%d (size %lu) returned %d, bail'n..\n", hdr.d_jno, hdr.d_ino, hdr.d_size, b_len);
      return (err);
    }

    if (data_of > b_of)
      continue;

//    if (data_of < DEBUG: 
    shbuf_cat(ret_buff, &blk.d_raw.bin, MIN(SHFS_BLOCK_DATA_SIZE, blk.hdr.d_size));

    b_of += b_len;
    memcpy(&hdr, &blk.hdr, sizeof(hdr));
  }
fprintf(stderr, "DEBUG: shfs_inode_read: read %lu bytes from ref %d:%d.. ret_buff.size = %d, last segment \"%-5.5s\".\n", b_of, inode->hdr.d_jno, inode->hdr.d_ino, ret_buff->data_of, blk.d_raw.bin);

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


void shfs_inode_free(shfs_ino_t **inode_p)
{
  shfs_ino_t *inode;

  if (!inode_p)
    return;
  
  inode = *inode_p;
  if (!inode)
    return;

  if (inode->tree) {
    if (inode->tree->base_ino == inode || inode->tree->cur_ino == inode)
      return; /* required for additional reference. */
  }

  *inode_p = NULL;

  if (inode->parent) {
    shmeta_unset_void(inode->parent->child, shkey_str(inode->d_raw.name));
  }

  shmeta_free(&inode->child);
  free(inode);
}

_TEST(shfs_inode_free)
{
  shfs_t *tree;
  shfs_ino_t *file;

  _TRUEPTR(tree = shfs_init(NULL, 0));
  if (!tree)
     return;

  /* ensure we cannot free root node of partition. */
  shfs_inode_free(&tree->base_ino);
  _TRUEPTR(tree->base_ino);

  /* ensure we can free newly created file. */
  _TRUEPTR(file = shfs_inode(tree->base_ino, "shfs_inode_free", 0));
  if (file) {
    shfs_inode_free(&file);
    _TRUE(!file);
  }

  shfs_free(&tree);
}

char *shfs_inode_path(shfs_ino_t *inode)
{
  char path[PATH_MAX+1];
  char buf[PATH_MAX+1];
  shfs_ino_t *node;

  memset(path, 0, sizeof(path));
  for (node = inode; node; node = node->parent) {
    char *fname = shfs_inode_name(node);
    if (!fname)
      continue;
    strcpy(buf, path);

    strcpy(path, fname);
    if (!*buf) {
      strncat(path, "/", PATH_MAX - strlen(path));
      strncpy(path, buf, PATH_MAX - strlen(path));
    }
  }

  return (path);
}

char *shfs_inode_name(shfs_ino_t *inode)
{
  if (!inode)
    return ("");
  return (inode->d_raw.name);
}

char *shfs_inode_filename(char *name)
{
  static char fname[SHFS_BLOCK_SIZE];
  shkey_t *key;

  memset(fname, 0, sizeof(fname));
  strncpy(fname, name, SHFS_PATH_MAX);
  if (strlen(name) > SHFS_PATH_MAX) {
    key = shkey_str(name);
    strcat(fname, shkey_print(key));
    shkey_free(&key);
  }

}

void shfs_inode_filename_set(shfs_ino_t *inode, char *name)
{
  strncpy(inode->d_raw.name, shfs_inode_filename(name), SHFS_PATH_MAX);
}


