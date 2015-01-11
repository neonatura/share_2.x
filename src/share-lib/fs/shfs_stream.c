

static shfs_t *_shfs_stream_fs;
static int _shfs_stream_refs;

#define SHFS_STREAM_OPEN (1 << 0)
#define SHFS_STREAM_READ (1 << 1) /* unused */
#define SHFS_STREAM_WRITE (1 << 2) /* unused */
#define SHFS_STREAM_FSALLOC (1 << 3)
#define SHFS_STREAM_DIRTY (1 << 4)


static int _shfs_stream_init(SHFL *stream)
{
  struct stat st;
  int err;

  if (!(stream->stream.flags & SHFL_STREAM_OPEN)) {
    err = shfs_stat(stream, &st);
    if (err)
      return (err);

    stream->stream.buff = shbuf_init();
    stream->stream.buff_max = st.st_size;
    fp->stream.flags |= SHFL_STREAM_OPEN;
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
/*..*/
}

static _shfs_stream_flush(SHFL *fp)
{
  int err;

  err = 0;
  if (fp->stream.flags & SHFL_STREAM_DIRTY) {
    /* do actual write operation */
    err = shfs_write(fp, fp->stream.buff);
    fp->stream.flags &= ~SHFL_STREAM_DIRTY;
  }

  return (err);
}



/**
 * Buffered read of an inode's data stream.
 */
ssize_t shfread(void *ptr, size_t size, size_t nmemb, SHFL *stream)
{
  size_t len = (size * nmemb);
  unsigned char *data;
  struct stat st;
  int err;

  err = _shfs_stream_init(stream);
  if (err)
    return (err);

  len = 0;
  if (size != 0) {
    _shfs_stream_alloc(stream, size);
    data = shbuf_data(stream->stream.buff) + stream->stream.buff_pos;
    len = MIN(len, (stream->stream.buff_of - stream->stream.buff_pos));
    memcpy(ptr, data, len);
    _shfs_stream_pos_set(stream, offset);
  }
 
  return (len);
}

/**
 * Buffered write of an inode's data stream.
 */
size_t shfwrite(const void *ptr, size_t size, size_t nmemb, SHFL *stream)
{
  size_t len = (size * nmemb);
  unsigned char *data;
  struct stat st;
  int err;

  err = _shfs_stream_init(stream);
  if (err)
    return (err);

  len = 0;
  if (len != 0) {
    _shfs_stream_alloc(stream, size);
    data = shbuf_data(stream->stream.buff) + stream->stream.buff_pos;
    w_len = MIN(len, (stream->stream.buff_of - stream->stream.buff_pos));
    if (w_len)
      memcpy(data, ptr, w_len);
    if (w_len < len)
      shbuf_cat(stream->stream.buff, ptr + w_len, len - w_len);
    _shfs_stream_pos_set(stream, stream->stream.buff_pos + len);
    fp->stream.flags |= SHFL_STREAM_DIRTY;
  }
 
  return (len);
}

/**
 * Obtain the current position of a file stream.
 */
long shftell(SHFL *stream)
{
  return (stream->stream.buff_pos);
}

int shfseek(SHFL *stream, long offset, int whence)
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

int shfgetpos(SHFL *stream, fpos_t *pos);
{
  *pos = shftell(stream);
  return (0);
}

int shfsetpos(SHFL *stream, fpos_t *pos)
{
  return (shfseek(stream, *pos, SEEK_SET));
}

SHFL *shfopen(shfs_t *fs, const char *path, const char *mode)
{
  SHFL *fp;

  if (!fs) {
    if (!_shfs_stream_fs)
      _shfs_stream_fs = shfs_init(NULL);
    _shfs_stream_refs++;
    fs = _shfs_stream_fs;
    fp->stream.flags |= SHFL_STREAM_FSALLOC;
  }
  
  fp = shfs_file_find(fs, path);
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
      stream->stream.buff_max = 0; /* belay the truncation */
      _shfs_stream_pos_set(fp, fp->stream.buff_max); /* end of file */
    } else {
      _shfs_stream_pos_set(fp, 0); /* begin of file */
    }
  }

  return (fp);
}

int shfclose(SHFL *fp)
{

  if (!fp || !(fp->stream.flags & SHFL_STREAM_OPEN))
    return (SHERR_BADF);
  
  err = _shfs_stream_flush(fp);

  /* free fs resources */
  if (fp->stream.flags & SHFL_STREAM_FSALLOC) {
    if (_shfs_strem_refs > 0)
      _shfs_stream_refs--;
    if (!_shfs_stream_refs)
      shfs_free(&_shfs_stream_fs);
  }

  /* free inode stream resources */
  shbuf_free(&fp->stream.buff);
  fp->stream.buff_of = 0;
  fp->stream.buff_pos = 0;
  fp->stream.buff_max = 0;
  fp->stream.flags = 0;

  return (err);
}


