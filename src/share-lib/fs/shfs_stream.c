
/*
 * @copyright
 *
 *  Copyright 2014 Neo Natura
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

static shfs_t *_shfs_stream_fs;
static int _shfs_stream_refs;

#define SHFS_STREAM_OPEN (1 << 0)
#define SHFS_STREAM_READ (1 << 1) /* unused */
#define SHFS_STREAM_WRITE (1 << 2) /* unused */
#define SHFS_STREAM_FSALLOC (1 << 3)
#define SHFS_STREAM_DIRTY (1 << 4)
#define SHFS_STREAM_MEMORY (1 << 5)

static int _shfs_stream_init(SHFL *stream)
{
  struct stat st;
  int err;

  if (!(stream->stream.flags & SHFS_STREAM_OPEN)) {
    err = shfs_fstat(stream, &st);
    if (err)
      return (err);

    stream->stream.buff = shbuf_init();
    stream->stream.buff_max = st.st_size;
    stream->stream.flags |= SHFS_STREAM_OPEN;
  }

  return (err);
}

static void _shfs_stream_pos_set(SHFL *stream, size_t pos)
{
  pos = MIN(stream->stream.buff_max, pos);
  stream->stream.buff_pos = pos;
}

static void _shfs_stream_alloc(SHFL *stream, size_t size)
{
  size_t len;
  unsigned char *data;

  len = MAX(0, (ssize_t)stream->stream.buff_pos + 
      (ssize_t)size - (ssize_t)shbuf_size(stream->stream.buff));
  if (!len)
    return;

  data = (unsigned char *)calloc(len, sizeof(char));
  shbuf_cat(stream->stream.buff, data, len);
  free(data);
}

static _shfs_stream_flush(SHFL *fp)
{
  int err;

  err = 0;
  if (fp->stream.flags & SHFS_STREAM_DIRTY) {
    /* do actual write operation */
    err = shfs_write(fp, fp->stream.buff);
    fp->stream.flags &= ~SHFS_STREAM_DIRTY;
  }

  return (err);
}

ssize_t shfread(void *ptr, size_t size, size_t nmemb, SHFL *stream)
{
  size_t len = (size * nmemb);
  unsigned char *data;
  struct stat st;
  int err;

  err = _shfs_stream_init(stream);
  if (err)
    return (err);

//  len = 0;
  if (len != 0) {
    _shfs_stream_alloc(stream, len);
    data = shbuf_data(stream->stream.buff) + stream->stream.buff_pos;
    len = MIN(len, stream->stream.buff_max);
    memcpy(ptr, data, len);
    _shfs_stream_pos_set(stream, stream->stream.buff_pos + len);
  }
 
  return (len);
}

size_t shfwrite(const void *ptr, size_t size, size_t nmemb, SHFL *stream)
{
  size_t len = (size * nmemb);
  unsigned char *data;
  struct stat st;
  size_t w_len;
  int err;

  err = _shfs_stream_init(stream);
  if (err)
    return (err);

//  len = 0;
  if (len != 0) {
    _shfs_stream_alloc(stream, len);
    data = shbuf_data(stream->stream.buff) + stream->stream.buff_pos;
    w_len = MIN(len, stream->stream.buff_pos);
    if (w_len)
      memcpy(data, ptr, w_len);
    if (w_len < len)
      shbuf_cat(stream->stream.buff, ptr + w_len, len - w_len);
    _shfs_stream_pos_set(stream, stream->stream.buff_pos + len);
    stream->stream.flags |= SHFS_STREAM_DIRTY;
  }
 
  return (len);
}

/**
 * Obtain the current position of a file stream.
 */
size_t shftell(SHFL *stream)
{
  return (stream->stream.buff_pos);
}

int shfseek(SHFL *stream, size_t offset, int whence)
{
  int err;

  err = _shfs_stream_init(stream);
  if (err)
    return (err);

  if (whence == SEEK_END)
    offset = stream->blk.hdr.size;
  else if (whence == SEEK_CUR)
    offset += shftell(stream);

  _shfs_stream_pos_set(stream, offset);

  return (err);
}

int shfgetpos(SHFL *stream, size_t *pos)
{
  *pos = shftell(stream);
  return (0);
}

int shfsetpos(SHFL *stream, size_t *pos)
{
  return (shfseek(stream, *pos, SEEK_SET));
}

SHFL *shfopen(const char *path, const char *mode, shfs_t *fs)
{
  SHFL *fp;
  int err;

  if (!fs) {
    if (!_shfs_stream_fs)
      _shfs_stream_fs = shfs_init(NULL);
    _shfs_stream_refs++;
    fs = _shfs_stream_fs;
    fp->stream.flags |= SHFS_STREAM_FSALLOC;
  }
  
  fp = shfs_file_find(fs, (char *)path);
  if (fp) {
#if 0
    if (strchr(mode, 'w') && !strchr(mode, 'a')) {
      shfs_inode_clear(fp); /* truncate file */
    }
#endif

    err = _shfs_stream_init(fp);
    if (err)
      return (NULL);

    if (strchr(mode, 'a')) {
      fp->stream.buff_max = 0; /* belay the truncation */
      _shfs_stream_pos_set(fp, fp->stream.buff_max); /* end of file */
    } else {
      _shfs_stream_pos_set(fp, 0); /* begin of file */
    }
  }

  return (fp);
}

int shfclose(SHFL *fp)
{
  int err;

  if (!fp || !(fp->stream.flags & SHFS_STREAM_OPEN))
    return (SHERR_BADF);
  
  if (!(fp->stream.flags & SHFS_STREAM_MEMORY)) {
    err = _shfs_stream_flush(fp);

    /* free fs resources */
    if (fp->stream.flags & SHFS_STREAM_FSALLOC) {
      if (_shfs_stream_refs > 0)
        _shfs_stream_refs--;
      if (!_shfs_stream_refs)
        shfs_free(&_shfs_stream_fs);
    }
  }

  /* free inode stream resources */
  shbuf_free(&fp->stream.buff);
  fp->stream.buff_of = 0;
  fp->stream.buff_pos = 0;
  fp->stream.buff_max = 0;
  fp->stream.flags = 0;

  return (err);
}

SHFL *shfmemopen(void *buf, size_t size, const char *mode)
{
  SHFL *fp;
  struct stat st;
  int err;

  fp = (shfs_ino_t *)calloc(1, sizeof(shfs_ino_t));
  if (!fp)
    return (NULL); /* SHERR_NOMEM */

  fp->blk.hdr.type = SHINODE_FILE;
  //fp->blk.hdr.format = SHINODE_MEMORY;
  fp->blk.hdr.format = SHINODE_BINARY;
  fp->blk.hdr.size = size;
  if (buf && size)
    fp->blk.hdr.crc = shcrc(buf, size);

  fp->stream.buff = shbuf_init();
  if (buf && size)
    shbuf_cat(fp->stream.buff, buf, size);
  fp->stream.buff_max = size;
  fp->stream.flags |= SHFS_STREAM_OPEN;
  fp->stream.flags |= SHFS_STREAM_MEMORY;

  if (strchr(mode, 'a')) {
    fp->stream.buff_max = 0; /* belay the truncation */
    _shfs_stream_pos_set(fp, fp->stream.buff_max); /* end of file */
  } else {
    _shfs_stream_pos_set(fp, 0); /* begin of file */
  }

  return (fp);
}

_TEST(shfmemopen)
{
  shpeer_t *peer;
  shfs_t *fs;
  shfs_ino_t *file;
  char rbuf[4096];
  char buf[4096];
  ssize_t rlen;

  memset(rbuf, 0, sizeof(rbuf));

  peer = shpeer_init("test", NULL);
  fs = shfs_init(peer);

  memset(buf, 'T', sizeof(buf));
  file = shfmemopen(buf, sizeof(buf), "rw+");

  rlen = shfread(rbuf, 1, sizeof(rbuf), file);
  _TRUE(rlen == 4096);
  _TRUE(0 == memcmp(buf, rbuf, 4096));

  _TRUE(0 == shfclose(file));

  shfs_free(&fs);
  shpeer_free(&peer);
}


