

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
#include "shfs_int.h"

#define SHFS_PUBLIC_DIR "pub/"

static int _shfs_block_notify(shfs_t *tree, shfs_block_t *blk)
{
  shbuf_t *buff;
  uint32_t mode; 
  int qid;
  int err;

  if (!tree || !blk)
    return (0); /* done */

  qid = _shfs_file_qid();
  if (qid == -1)
    return (SHERR_IO);

  buff = shbuf_init();
  mode = TX_FILE;
  shbuf_cat(buff, &mode, sizeof(mode));
  shbuf_cat(buff, &tree->peer, sizeof(shpeer_t));
  shbuf_cat(buff, &blk->hdr, sizeof(shfs_hdr_t));
  err = shmsg_write(qid, buff, NULL);
  shbuf_free(&buff);
  if (err)
    return (err);

  return (0);
}

int shfs_file_notify(shfs_ino_t *file)
{
  return (_shfs_block_notify(file->tree, &file->blk));
}

int shfs_read(shfs_ino_t *file, shbuf_t *buff)
{
  shfs_ino_t *aux;
  int format;
  int err;

	if (file == NULL)
    return (SHERR_INVAL);

  if (shfs_format(file) == SHINODE_NULL)
    return (SHERR_NOENT); /* no data content */

  if (!buff)
    return (0);

  err = 0;
  format = shfs_format(file);
  if (format == SHINODE_EXTERNAL) {
    err = shfs_ext_read(file, buff);
  } else if (format == SHINODE_COMPRESS) {
    err = shfs_zlib_read(file, buff); 
  } else if (format == SHINODE_BINARY) {
    err = shfs_bin_read(file, buff);
  }

  return (err);
}

int shfs_write(shfs_ino_t *file, shbuf_t *buff)
{
  shfs_ino_t *aux;
  int format;
  int err;

  if (!file)
    return (SHERR_INVAL);

  err = 0;
  format = shfs_format(file);
  if (!buff) {
    /* presume user wants to erase content. */
    if (format) {
      aux = shfs_inode(file, NULL, format);
      err = shfs_inode_clear(aux);
      if (!err)
        file->blk.hdr.format = SHINODE_NULL;
    }
  } else {
    if (format == SHINODE_EXTERNAL) {
      err = shfs_ext_write(file, buff);
    } else if (format == SHINODE_COMPRESS) {
      err = shfs_zlib_write(file, buff);
    } else {
      err = shfs_bin_write(file, buff);
    }
  }

  if (!err)
    err = shfs_inode_write_entity(file);

  if (!err && (file->blk.hdr.attr & SHATTR_SYNC))
    shfs_file_notify(file);

  return (err);
}


int shfs_file_pipe(shfs_ino_t *file, int fd)
{
  shbuf_t *buff;
  ssize_t b_of;
  int b_len;
  int err;

  if (file == NULL)
    return (SHERR_NOENT);

  buff = shbuf_init();
  err = shfs_read(file, buff);
  if (err) {
    shbuf_free(&buff);
    return (err);
  }

  for (b_of = 0; b_of < buff->data_of; b_of++) {
    b_len = write(fd, buff->data + b_of, buff->data_of - b_of);
    if (b_len < 0) {
      shbuf_free(&buff);
      return (-errno);
    }

    b_of += b_len;
  }

  shbuf_free(&buff);
  return (0);
}

shfs_ino_t *shfs_file_find(shfs_t *tree, char *path)
{
  shfs_ino_t *dir;
  shfs_ino_t *file;
  int is_remote;
  char fpath[PATH_MAX+1];
  char curpath[PATH_MAX+1];
  char *filename;
  char *dirname;
  char *ptr;

  file = NULL;

  if (!path || !*path)
    return (NULL);

  memset(fpath, 0, sizeof(fpath));
  if (*path == '/') {
    strncpy(fpath, path + 1, PATH_MAX);
  } else {
    strncpy(fpath, path, PATH_MAX);
  }
#if 0
  memset(fpath, 0, sizeof(fpath));
  if (*path == '/') {
    dir = tree->base_ino;
    strncpy(fpath, path + 1, PATH_MAX);
  } else {
    dir = tree->cur_ino;
    strncpy(fpath, path, PATH_MAX);
  }

  if (!*fpath)
    return (dir);
#endif
  
  if (!*fpath || fpath[strlen(fpath)-1] == '/') {
    dir = shfs_dir_find(tree, fpath);
    return (dir);
  }

  dirname = NULL;
  filename = NULL;

  ptr = strrchr(fpath, '/');
  if (!ptr) {
    //file = shfs_inode(tree->cur_ino, fpath, SHINODE_FILE);
    filename = fpath;
    memset(curpath, 0, sizeof(curpath));
    getcwd(curpath, sizeof(curpath) - 2);
    if (*curpath && curpath[strlen(curpath)-1] == '/')
      curpath[strlen(curpath) - 1] = '\0';
    dirname = curpath;
    if (*dirname == '/')
      dirname++;
  } else {
    *ptr++ = '\000';
    dirname = fpath;
    filename = ptr;
  }

  dir = shfs_dir_find(tree, dirname);
  if (!dir)
    return (NULL);

  file = shfs_inode(dir, filename, SHINODE_FILE);

  is_remote = FALSE;
  if (0 == strcmp(dirname, "pub") || 0 == strncmp(dirname, "pub/", 4))
    is_remote = TRUE; /* base '/pub/' dir of all fs is sync'd. */
  else if (file->parent && (file->parent->blk.hdr.attr & SHATTR_SYNC))
    is_remote = TRUE; /* parent is remote sync'd */

  if (is_remote) {
    /* set file as remote */
    shfs_attr_set(file, SHATTR_SYNC);
  }

  return (file);
}


int shfs_file_remove(shfs_ino_t *file)
{
  shfs_ino_t *aux;
  shbuf_t *buff;
  int err;

  aux = shfs_inode(file, NULL, SHINODE_BINARY);
  if (!aux)
    return (SHERR_IO);

  err = shfs_inode_clear(aux);
  if (err)
    return (err);

  /* reset stats on file inode. */
  file->blk.hdr.mtime = 0;
  file->blk.hdr.size = 0;
  file->blk.hdr.type = SHINODE_NULL;
  err = shfs_inode_write_entity(file);
  if (err) {
    PRINT_RUSAGE("shfs_inode_write: error writing entity.");
    return (err);
  }

  return (0);
}

shkey_t *shfs_file_key(shfs_ino_t *file)
{
  if (!file)
    return (NULL);
  return (&file->blk.hdr.name);
}

uint64_t shfs_crc(shfs_ino_t *file)
{
  
  if (!file)
    return (0);

  return (file->blk.hdr.crc);
}

_TEST(shfs_crc)
{
  shfs_t *fs;
  SHFL *file;

  fs = shfs_init(NULL);
  file = shfs_file_find(fs, "/test/shfs_crc");
  _TRUE(shfs_crc(file));
  shfs_free(&fs);

}

shsize_t shfs_size(shfs_ino_t *file)
{
  
  if (!file)
    return (0);

  return (file->blk.hdr.size);
}

/* todo: read/write chunks of ashkey_uniq() to 'test' peer fs. */
struct test_shfs_t {
  int val;
  char str[16];
}; 
_TEST(shfs_read)
{
  struct test_shfs_t *ar;
  shfs_t *tree;
  shfs_ino_t *fl;
  shbuf_t *rtbuff;
  shbuf_t *rbbuff;
  shbuf_t *wtbuff;
  shbuf_t *wbbuff;
  char binbuf[4096];
  char buf[4096];
  unsigned char *bin_data;
  size_t bin_data_len;
  size_t block_len;
  int test_idx;
  int val;
  int i;

  /* ensure multiple writes reflect content change. */
  for (test_idx = 0; test_idx < 3; test_idx++) {
    memset(buf, 0, sizeof(buf));
    memset(buf, '0', 2048);

    block_len = sizeof(binbuf) / sizeof(struct test_shfs_t);
    memset(binbuf, 0, sizeof(binbuf));
    ar = (struct test_shfs_t *)binbuf;
    for (i = 0; i < block_len; i++) {
      val = (test_idx + i);
      ar[i].val = val;
      sprintf(ar[i].str, "%d", val);
    } 



    /* write */
    tree = shfs_init(NULL);
    _TRUEPTR(tree);

    wtbuff = shbuf_init();
    shbuf_cat(wtbuff, buf, sizeof(buf));
    fl = shfs_file_find(tree, "/test/test"); 
    _TRUE(0 == shfs_write(fl, wtbuff));
    shbuf_free(&wtbuff);

    wbbuff = shbuf_init();
    shbuf_cat(wbbuff, binbuf, sizeof(binbuf));
    fl = shfs_file_find(tree, "/test/test.bin"); 
    _TRUE(0 == shfs_write(fl, wbbuff));
    shbuf_free(&wbbuff);

    shfs_free(&tree);

    /* read */
    tree = shfs_init(NULL);
    _TRUEPTR(tree);

    rtbuff = shbuf_init();
    fl = shfs_file_find(tree, "/test/test"); 
    _TRUE(0 == shfs_read(fl, rtbuff));
    _TRUEPTR(shbuf_data(rtbuff));
    _TRUE(shbuf_size(rtbuff) == sizeof(buf));
    _TRUE(strlen(shbuf_data(rtbuff)) == 2048); 
    _TRUE(0 == strcmp(buf, shbuf_data(rtbuff)));
    shbuf_free(&rtbuff);

    rbbuff = shbuf_init();
    fl = shfs_file_find(tree, "/test/test.bin"); 
    _TRUE(fl->blk.hdr.crc);
    _TRUE(0 == shfs_read(fl, rbbuff));
    _TRUEPTR(shbuf_data(rbbuff));
    _TRUE(shbuf_size(rbbuff) == sizeof(binbuf));
    _TRUE(0 == memcmp(shbuf_data(rbbuff), binbuf, sizeof(binbuf)));
    ar = (struct test_shfs_t *)shbuf_data(rbbuff);
    for (i = 0; i < block_len; i++) {
      val = (test_idx + i);
      _TRUE(val == ar[i].val);
      _TRUE(val == atoi(ar[i].str));
    } 
    shbuf_free(&rbbuff);

    shfs_free(&tree);
  }

}


