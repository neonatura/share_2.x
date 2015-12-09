
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



#define MAX_SHFS_DESCRIPTORS 1024

#define SHFS_DESCRIPTOR_OFFSET (0xFFFF)


static shfs_ino_buf_t _stream_table[MAX_SHFS_DESCRIPTORS];


#if 0
static int _shfs_stream_init(SHFL *stream)
{
  struct stat st;
  int err;

  err = 0;
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
#endif


#if 0
ssize_t shfread(void *ptr, size_t size, size_t nmemb, SHFL *stream)
{
  size_t len = (size * nmemb);
  unsigned char *data;
  struct stat st;
  int err;

  err = _shfs_stream_init(stream);
  if (err)
    return (err);

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
#endif


#if 0
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
#endif

#if 0
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
#endif

#if 0
int shfclose(SHFL *fp)
{
  int err;

  if (!fp || !(fp->stream.flags & SHFS_STREAM_OPEN))
    return (SHERR_BADF);
  
  err = 0;
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
#endif













#if 0
/**
 * Opens a memory stream.
 * @param mode The I/O access level - i.e. "r" read, "w" write, "a" append
 * @param buf The memory segment to perform stream I/O on.  
 * @see fmemopen()
 */
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

_UNUSED TEST(shfmemopen)
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
#endif









static int _shfs_stream_alloc(shfs_ino_buf_t *stream, size_t size)
{
  unsigned char *data;
  size_t tot_len;
  ssize_t len;
  size_t of;
  int err;

  tot_len = size + stream->buff_pos;

  if ((stream->flags & SHFS_STREAM_MEMORY)) {
    return (shbuf_grow(stream->buff, tot_len));
  }

  if (tot_len >= stream->buff->data_max) {
    size_t block_size = sysconf(_SC_PAGE_SIZE);
    size_t alloc_len = ((tot_len / block_size) + 1) * block_size; /* mmap */

    /* allocate enough of file to perform I/O operation. */
    err = shbuf_growmap(stream->buff, alloc_len);
    if (err) {
      sherr(err, "shbuf_growmap");
      return (err);
    }
  }

  len = MIN(tot_len, stream->buff_max) - shbuf_size(stream->buff);
  if (len > 0) {
    /* read supplemental content to fullfill total length requested */
    of = shbuf_size(stream->buff);
    err = shfs_read_of(stream->file, stream->buff, of, len);
    if (err < 0 && err != SHERR_NOENT) {
      return (err);
    }
  }

  return (0);
}

static int _shfs_stream_flush(shfs_ino_buf_t *stream)
{
  shbuf_t *buff;
  size_t len;
  size_t c_len;
  int err;

  if (!(stream->flags & SHFS_STREAM_OPEN)) {
    /* cannot flush closed file */
    return (SHERR_BADF);
  }

  err = 0;
  if (stream->flags & SHFS_STREAM_DIRTY) {
    if (!stream->file) {
      sherr(SHERR_IO, "_shfs_stream_flush: null file");
      return (SHERR_IO);
    }


    /* read in content that hasn't been streamed (until write_of exists) */
    err = 0;
    len = MAX(0, stream->buff_max - shbuf_size(stream->buff));
    if (len) {
      err = _shfs_stream_alloc(stream, len);
    }
    if (!err) {
      /* do actual write operation */
      err = shfs_write(stream->file, stream->buff);
    }

    stream->flags &= ~SHFS_STREAM_DIRTY;
  }

  return (err);
}



shfs_ino_buf_t *shfs_stream_get(int fd)
{

  fd -= SHFS_DESCRIPTOR_OFFSET;
  if (fd < 0 || fd >= MAX_SHFS_DESCRIPTORS)
    return (NULL);

  return (&_stream_table[fd]);
}

int shfs_stream_getfd(void)
{
  shfs_ino_buf_t *stream;
  int max;
  int fd;

  max = SHFS_DESCRIPTOR_OFFSET + MAX_SHFS_DESCRIPTORS;
  for (fd = SHFS_DESCRIPTOR_OFFSET; fd < max; fd++) {
    stream = shfs_stream_get(fd);
    if (!(stream->flags & SHFS_STREAM_OPEN)) {
      return (fd);
    }
  }

  return (-1);
}


int shfs_stream_init(shfs_ino_buf_t *stream, SHFL *file)
{
  struct stat st;
  SHFL *fp;
  shbuf_t *buff;
  int err;

  if ((stream->flags & SHFS_STREAM_OPEN))
    return (SHERR_INVAL);

  err = shfs_fstat(file, &st);
  if (!(stream->flags & SHFS_STREAM_CREATE)) {
    /* file is required to exist */
    if (err)
      return (err);
  }

  buff = shbuf_init();
  stream->buff_max = 0;
  if (!err) {
#if 0
    /* pre-allocate some mapped memory */
    err = shbuf_growmap(buff, st.st_size);
    if (err) {
      shbuf_free(&buff);
      return (err);
    }
#endif

    /* set maximum seek offset */
    stream->buff_max = st.st_size;
  }

  /* initialize stream */
  stream->buff = buff;
  stream->flags |= SHFS_STREAM_OPEN;
  stream->file = file;

  return (0);
}

int shfs_stream_open(shfs_ino_buf_t *stream, const char *path, shfs_t *fs)
{
  SHFL *file;
  int flags;

  flags = 0;
  if (!fs) {
    fs = shfs_init(NULL); 
    stream->fs = fs;
  }

  file = shfs_file_find(fs, (char *)path);
  return (shfs_stream_init(stream, file));
}

int shfs_stream_close(shfs_ino_buf_t *stream)
{
  int err;

  /* set closed state */
  if (!(stream->flags & SHFS_STREAM_OPEN)) {
    return (SHERR_BADF);
  }

  /* flush pending content */
  err = 0;
  if (!(stream->flags & SHFS_STREAM_MEMORY)) {
    err = _shfs_stream_flush(stream);
  }

  /* free partition reference, if allocated */
  shfs_free(&stream->fs);

  /* free mmap buffer */
  shbuf_free(&stream->buff);

  /* reset working variables */
  stream->file = NULL;
  stream->buff_pos = 0;
  stream->buff_max = 0;
  stream->flags = 0;

  return (err);
}

int shfs_stream_setpos(shfs_ino_buf_t *stream, size_t pos)
{

  if (pos < 0 || pos > stream->buff_max)
    return (SHERR_INVAL);

  stream->buff_pos = pos;
  return (0);
}

/**
 * Obtain the current position of a file stream.
 */
size_t shfs_stream_getpos(shfs_ino_buf_t *stream)
{
  return (stream->buff_pos);
}

ssize_t shfs_stream_read(shfs_ino_buf_t *stream, void *ptr, size_t size)
{
  unsigned char *data;
  int err;

  if (!(stream->flags & SHFS_STREAM_OPEN))
    return (SHERR_BADF);

  size = MIN(size, stream->buff_max - stream->buff_pos);
  if (size != 0) {
    /* load file contents in mmap as neccessary */
    err = _shfs_stream_alloc(stream, size);
    if (err)
      return (err);

    if (shbuf_data(stream->buff)) {
      /* copy file segment into user-buffer */
      data = shbuf_data(stream->buff) + stream->buff_pos;
      memcpy(ptr, data, size);
    }

    /* reposition stream offset after data read */
    shfs_stream_setpos(stream, stream->buff_pos + size);
  }
 
  return (size);
}

ssize_t shfs_stream_write(shfs_ino_buf_t *stream, const void *ptr, size_t size)
{
  unsigned char *data;
  struct stat st;
  size_t buff_of;
  size_t w_len;
  int err;

  if (!(stream->flags & SHFS_STREAM_OPEN))
    return (SHERR_BADF);

  if (size != 0) {
    err = _shfs_stream_alloc(stream, size);
    if (err)
      return (err);

    buff_of = stream->buff_pos + size;

    if (shbuf_data(stream->buff)) {
      data = shbuf_data(stream->buff) + stream->buff_pos;
      memcpy(data, ptr, size);

      /* update buffer 'total size' consumed */
      stream->buff->data_of = MAX(stream->buff->data_of, buff_of);

      /* update 'total file size' */
      stream->buff_max = MAX(stream->buff_max, buff_of);
    }

    shfs_stream_setpos(stream, buff_of);

    if (!(stream->flags & SHFS_STREAM_MEMORY))
      stream->flags |= SHFS_STREAM_DIRTY;
  }
 
  return (size);
}

int shfs_stream_sync(shfs_ino_buf_t *stream)
{

  if (!(stream->flags & SHFS_STREAM_DIRTY))
    return (0);

  return (_shfs_stream_flush(stream));
}

int shfs_stream_stat(shfs_ino_buf_t *stream, struct stat *buf)
{

  if (!(stream->flags & SHFS_STREAM_OPEN)) {
    return (SHERR_NOENT);
  }

  memset(buf, 0, sizeof(struct stat));
  shfs_fstat(stream->file, buf);
  buf->st_size = stream->buff_max; /* current max length of stream'd file */ 
/* DEBUG: wont have 'isregfile' mode set on new file */

  return (0);
}

/**
 * @note Does not function as a data length expansion mechanism.
 * @see shfs_inode_truncate()
 */
int shfs_stream_truncate(shfs_ino_buf_t *stream, size_t len)
{
  int err;

  if (len < 0)
    return (SHERR_INVAL);
  
  if (len == stream->buff_max)
    return (0); /* all done */

  if (len > stream->buff_max) {
    /* extend length */
/* DEBUG: */
    return (SHERR_OPNOTSUPP);
  }

  stream->buff_max = len;
  stream->flags |= SHFS_STREAM_DIRTY;

  return (0);
}

_TEST(shfs_stream_write)
{
  shpeer_t *peer;
  SHFL *file;
  shfs_ino_buf_t stream;
  ssize_t len;
  char buf[1024];
  char rbuf[1024];

  memset(&stream, 0, sizeof(stream));
  stream.flags |= SHFS_STREAM_CREATE;

  peer = shpeer_init("test", NULL);
  stream.fs = shfs_init(peer);
  shpeer_free(&peer);
  _TRUEPTR(stream.fs);

  file = shfs_file_find(stream.fs, "/shfs_stream_write");
  _TRUEPTR(file);


  _TRUE(0 == shfs_stream_init(&stream, file));

  memset(buf, '\001', sizeof(buf));
  len = shfs_stream_write(&stream, buf, sizeof(buf));
  _TRUE(len == sizeof(buf)); 

/*
fprintf(stderr, "STREAM WRITE:\n");
fprintf(stderr, "\tSTREAM MAX: %d\n", stream.buff_max);
fprintf(stderr, "\tSTREAM BUFF SIZE: %d\n", shbuf_size(stream.buff));
fprintf(stderr, "\tSTREAM BUFF MAX: %d\n", stream.buff->data_max);
*/

  _TRUE(0 == shfs_stream_setpos(&stream, 0));

  memset(rbuf, 0, sizeof(rbuf));
  len = shfs_stream_read(&stream, rbuf, sizeof(buf));
  _TRUE(len == sizeof(buf)); 
  _TRUE(0 == memcmp(buf, rbuf, sizeof(buf)));

  _TRUE(0 == shfs_stream_close(&stream));


}


