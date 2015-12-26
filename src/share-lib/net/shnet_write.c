
/*
 * @copyright
 *
 *  Copyright 2013 Neo Natura
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
 *
 */

#include "share.h"

ssize_t shnet_write(int fd, const void *buf, size_t count)
{
#if 0
  unsigned int usk = (unsigned int)fd;
  ssize_t w_len;
  size_t len;

  if (usk >= USHORT_MAX)
    return (0);

  if (!_sk_table[usk].send_buff && count < 4096)
    return (write(fd, buf, count));

  if (!_sk_table[usk].send_buff)
    _sk_table[usk].send_buff = shbuf_init();

  if (buf && count)
    shbuf_cat(_sk_table[usk].send_buff, (char *)buf, count);

  if (_sk_table[usk].send_buff->data_of == 0)
    return (0);

  w_len = write(fd, _sk_table[usk].send_buff->data, _sk_table[usk].send_buff->data_of);
  if (w_len >= 1) {
    shbuf_trim(_sk_table[usk].send_buff, w_len);
  }

  /* return bytes read into buffer. */
  return (count);
#endif

  return (write(fd, buf, count));
}

/**
 * @returns 0 upon success
 */
int shnet_write_flush(int fd)
{
  fd_set w_set;
  struct timeval to;
  ssize_t b_len;
  unsigned int usk;
  int err;

  if (!_sk_table[usk].send_buff ||
      shbuf_size(_sk_table[usk].send_buff) == 0)
    return (shnet_write(fd, NULL, 0));

  FD_ZERO(&w_set);
  FD_SET(fd, &w_set);

  /* determine whether data may be written */
  memset(&to, 0, sizeof(to));
  err = shselect(fd+1, NULL, &w_set, NULL, &to);
  if (err < 1)
    return (err);

  /* flush pending socket data */
  b_len = shnet_write(fd, 
      shbuf_data(_sk_table[usk].send_buff), 
      shbuf_size(_sk_table[usk].send_buff)); 
  if (b_len < 0)
    return (b_len);
  shbuf_trim(_sk_table[usk].send_buff, b_len);
  
  return (0);
}



