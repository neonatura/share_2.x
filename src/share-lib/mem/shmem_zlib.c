

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
#include "zlib.h"


#define CHUNK 0x4000
#define windowBits 15
#define GZIP_ENCODING 16

static int _shz_err(int code)
{
  int ret_code;

  ret_code = -1;

  if (code == Z_MEM_ERROR) {
    ret_code = SHERR_NOMEM;
  } else if (code == Z_STREAM_ERROR) {
    ret_code = SHERR_IO;
  } else if (code == Z_ERRNO) {
    ret_code = -errno;
  } else if (code == Z_DATA_ERROR) {
    ret_code = SHERR_INVAL;
  } else if (code == Z_VERSION_ERROR) {
    ret_code = SHERR_INVAL;
  } else if (code == Z_NEED_DICT) {
    ret_code = SHERR_INVAL;
  }

  return (ret_code);
}

int shzenc(shbuf_t *buff, void *data, size_t data_len)
{
  unsigned char out[CHUNK];
  z_stream strm;
  int err;

  memset(&strm, 0, sizeof(z_stream));
  err = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
  if (err) {
    return -1;
  }

  strm.next_in = (unsigned char *)data;
  strm.avail_in = data_len;

  do {
    int have;

    strm.avail_out = CHUNK;
    strm.next_out = out;
    err = z_deflate(&strm, Z_FINISH);
    if (err < 0) {
      deflateEnd (& strm);
      return -1;
    }

    have = CHUNK - strm.avail_out;
    shbuf_cat(buff, out, have); 
  }
  while (strm.avail_out == 0);

  z_deflateEnd(&strm);
  return (0);
}

int shzdec(shbuf_t *buff, unsigned char *data, size_t data_len)
{
  z_stream strm;
  unsigned char out[CHUNK];
  unsigned have;
  int ret;

  if (data_len == 0)
    return (0);

  /* allocate inflate state */
  memset(&strm, 0, sizeof(strm));
  ret = inflateInit(&strm);
  if (ret != Z_OK)
    return ret;

  strm.next_in = data;
  strm.avail_in = data_len;

  /* run inflate() on input until output buffer not full */
  do {
    strm.avail_out = CHUNK;
    strm.next_out = out;
    ret = z_inflate(&strm, Z_NO_FLUSH);
    switch (ret) {
      case Z_NEED_DICT:
      case Z_DATA_ERROR:
      case Z_MEM_ERROR:
        (void)inflateEnd(&strm);
        return _shz_err(ret);
    }

    have = CHUNK - strm.avail_out;
    shbuf_cat(buff, out, have);
  } while (strm.avail_out == 0);

  /* clean up and return */
  (void)z_inflateEnd(&strm);
  return ret == Z_STREAM_END ? 0 : SHERR_INVAL;
}


_TEST(shzenc)
{
  const char *data = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec sed hendrerit sapien.";
  shbuf_t *raw_buff;
  shbuf_t *buff;
  int err;

  buff = shbuf_init();
  err = shzenc(buff, data, strlen(data) + 1);
  _TRUE(err == 0);
  
  raw_buff = shbuf_init();
  err = shzdec(raw_buff, shbuf_data(buff), shbuf_size(buff));
  _TRUE(err == 0);

  _TRUE(0 == strcmp(data, shbuf_data(raw_buff)));

  shbuf_free(&raw_buff);
  shbuf_free(&buff);
}

