
/*
 * @copyright
 *
 *  Copyright 2015 Neo Natura 
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

/** Obtain a file descriptor referencing a sharefs file stream. */
int shopen(const char *path, const char *mode, shfs_t *fs)
{
  shfs_ino_buf_t *stream;
  int err;
  int fd;

  fd = shfs_stream_getfd();
  if (fd == -1)
    return (SHERR_NFILE);

  stream = shfs_stream_get(fd);
  if (!stream)
    return (SHERR_IO);

  if (strchr(mode, 'w') ||
      strchr(mode, 'a') ||
      strchr(mode, '+')) {
    /* create file if none exists */
    stream->flags |= SHFS_STREAM_CREATE;
  }

  err = shfs_stream_open(stream, path, fs);
  if (err) {
    shfs_stream_close(stream);
    return (err);
  }


  if (strchr(mode, 'a')) {
//    fp->stream.buff_max = 0; /* belay the truncation */
    shfs_stream_setpos(stream, stream->buff_max); /* end of file */
  } else {
    shfs_stream_setpos(stream, 0); /* begin of file */
  }

  return (fd);
}

/** Close a posix or sharefs stream file descriptor. */
int shclose(int fd)
{
  shfs_ino_buf_t *stream;
  int err;

  stream = shfs_stream_get(fd); 
  if (!stream) {
    struct stat st;

    /* process as POSIX file descriptor */
    err = fstat(fd, &st);
    if (!err && S_ISSOCK(st.st_mode)) {
      /* close as posix socket desctriptor */
      return (shnet_close(fd));
    }
    return (close(fd));
  }

  err = shfs_stream_close(stream);
  if (err)
    return (err);

  return (0);
}


int shfsetpos(int fd, size_t pos)
{
  shfs_ino_buf_t *stream;

  stream = shfs_stream_get(fd);
  if (!stream)
    return (SHERR_BADF);

  return (shfs_stream_setpos(stream, pos));
} 

int shfgetpos(int fd, size_t *pos)
{
  shfs_ino_buf_t *stream;

  stream = shfs_stream_get(fd);
  if (!stream)
    return (SHERR_BADF);

  *pos = shfs_stream_getpos(stream);
  return (0);
}

size_t shftell(int fd)
{
  size_t pos;
  int err;
  err = shfgetpos(fd, &pos);
  if (err)
    return (-1);
  return (pos);
}

int shrewind(int fd)
{
  return (shfsetpos(fd, 0));  
}

ssize_t shfseek(int fd, size_t offset, int whence)
{
  shfs_ino_buf_t *stream;
  int err;

  stream = shfs_stream_get(fd);
  if (!stream) { 
    /* process as POSIX file descriptor */
    return ((ssize_t)lseek(fd, offset, whence));
  }

  if (whence == SEEK_END)
    offset += stream->buff_max;
  else if (whence == SEEK_CUR)
    offset += shfs_stream_getpos(stream);

  err = shfs_stream_setpos(stream, offset);
  if (err)
    return (err);

  return ((ssize_t)offset);
}

int shread(int fd, void *ptr, size_t size)
{
  struct stat st;
  shfs_ino_buf_t *stream;
  int err;

  stream = shfs_stream_get(fd);
  if (!stream) {
    /* process as POSIX file descriptor */
    err = fstat(fd, &st);
    if (!err && S_ISSOCK(st.st_mode)) {
      /* read as posix socket desctriptor */
      return (shnet_read(fd, ptr, size));
    }

    /* read as posix file descriptor */
    return (read(fd, ptr, size));
  }

  return (shfs_stream_read(stream, ptr, size));
}

int shwrite(int fd, void *ptr, size_t size)
{
  shfs_ino_buf_t *stream;
  struct stat st;
  int err;

  stream = shfs_stream_get(fd);
  if (!stream) {
    /* process as POSIX file descriptor */
    err = fstat(fd, &st);
    if (!err && S_ISSOCK(st.st_mode)) {
      /* read as posix socket desctriptor */
      return (shnet_write(fd, ptr, size));
    }

    /* read as posix file descriptor */
    return (write(fd, ptr, size));
  }

  return (shfs_stream_write(stream, ptr, size));
}

int shfstat(int fd, struct stat *buf)
{
  shfs_ino_buf_t *stream;

  stream = shfs_stream_get(fd);
  if (!stream)
    return (fstat(fd, buf));

  return (shfs_stream_stat(stream, buf));
}

int shflock_test(int fd)
{
  shfs_ino_buf_t *stream;

  stream = shfs_stream_get(fd);
  if (!stream)
    return (0); /* DEBUG: flock() */

  return (shfs_locked(stream->file));
}

int shflock(int fd)
{
  shfs_ino_buf_t *stream;

  stream = shfs_stream_get(fd);
  if (!stream)
    return (0); /* DEBUG: flock() */

  return (shfs_lock(stream->file, SHLK_IO));
}

int shfunlock(int fd)
{
  shfs_ino_buf_t *stream;

  stream = shfs_stream_get(fd);
  if (!stream)
    return (0); /* DEBUG: flock() */

  return (shfs_unlock(stream->file));
}

/** Flush any pending data to be written from a buffered stream to a file */
int shflush(int fd)
{
  shfs_ino_buf_t *stream;
  struct stat st;
  int err;

  stream = shfs_stream_get(fd);
  if (!stream) {
    /* sync posix file descriptor */
    return (fsync(fd));
  }

  err = shfs_stream_sync(stream);
  if (err)
    return (err);

  return (0);
}

int shftruncate(int fd, size_t len)
{
  shfs_ino_buf_t *stream;
  struct stat st;
  int err;

  stream = shfs_stream_get(fd);
  if (!stream) {
    /* sync posix file descriptor */
    return (ftruncate(fd, len));
  }

  err = shfs_stream_truncate(stream, len);
  if (err)
    return (err);

  return (0);
}

int shfattr(int fd)
{
  shfs_ino_buf_t *stream;
  struct stat st;
  int err;

  stream = shfs_stream_get(fd);
  if (!stream) {
    return (SHERR_NOENT);
  }

  return (shfs_attr(stream->file));
}

int shfattr_set(int fd, int attr)
{
  shfs_ino_buf_t *stream;
  struct stat st;
  int err;

  stream = shfs_stream_get(fd);
  if (!stream) {
    return (SHERR_NOENT);
  }

  return (shfs_attr_set(stream->file, attr));
}


