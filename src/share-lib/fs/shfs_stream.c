

static shfs_t *_fs;
static int _fs_refs;

int _shfs_stream_pos_set(stream, *pos)
{
  return (SHERR_INVAL);
}


/**
 * Buffered read of an inode's data stream.
 */
size_t shfread(void *ptr, size_t size, size_t nmemb, SHFL *stream)
{
}

/**
 * Buffered write of an inode's data stream.
 */
size_t shfwrite(const void *ptr, size_t size, size_t nmemb, SHFL *stream)
{
}

long shftell(SHFL *stream)
{
  return (stream->stream.buff_of + stream->stream.buff_pos);
}

int shfseek(SHFL *stream, long offset, int whence)
{
  int err;

  if (whence == SEEK_END)
    offset = stream->blk.hdr.size;
  else if (whence == SEEK_CUR)
    offset += shftell(stream);

  err = _shfs_stream_pos_set(stream, offset);

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

SHFL *shfopen(const char *path, const char *mode)
{
  SHFL *fp;

  _fs_refs++;
  if (!_fs)
    _fs = shfs_init(NULL);
  
  fp = shfs_file_find(_fs, path);

  if (fp) {
    /* initialize stream buffer */
    _shfs_stream_pos_set(fp, 0);
  }

  return (fp);
}

int shfclose(SHFL *fp)
{

  /* free inode stream resources */
  shbuf_free(&fp->stream.buff);
  fp->stream.buff_of = 0;
  fp->stream.buff_pos = 0;

  /* free fs resources */
  _fs_refs--;
  if (!_fs_refs && _fs) {
    shfs_free(&_fs);
  }
  
}


