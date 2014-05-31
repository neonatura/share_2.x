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

#define MIN_READ_BUFF_SIZE 4096

ssize_t shnet_read(int fd, const void *buf, size_t count)
{
  unsigned int usk = (unsigned int)fd;
  struct timeval to;
  ssize_t r_len;
  size_t len;
  fd_set read_set;
  fd_set exc_set;
  int err;

#if 0
  if (!_sk_table[usk].recv_buff && count < MIN_READ_BUFF_SIZE)
    return (read(fd, buf, count));
#endif

  if (!_sk_table[usk].recv_buff)
    _sk_table[usk].recv_buff = shbuf_init();
  shbuf_grow(_sk_table[usk].recv_buff, count);

retry_select:
  FD_ZERO(&read_set);
  FD_SET(fd, &read_set);
  FD_ZERO(&exc_set);
  FD_SET(fd, &exc_set);
  if (!(_sk_table[usk].flags & SHNET_ASYNC)) {
    /* block for data */
fprintf(stderr, "DEBUG: data block.. read\n");
    err = select(fd+1, &read_set, NULL, &exc_set, NULL);
  } else {
    memset(&to, 0, sizeof(to));
    err = select(fd+1, &read_set, NULL, &exc_set, &to);
  }
  if (err == -1 && errno == EINTR)
    goto retry_select;
  if (err == -1) 
    return (-1);

  if (err == 1) {
    if (FD_ISSET(fd, &exc_set)) {
      /* no longer connected. */
      return (0);
    }

    /* data available for read. */
    fprintf(stderr, "DEBUG: _read(%d)\n", fd);
    r_len = read(fd, _sk_table[usk].recv_buff->data + _sk_table[usk].recv_buff->data_of, _sk_table[usk].recv_buff->data_max - _sk_table[usk].recv_buff->data_of);
    if (r_len < 1)
      return (r_len);
    _sk_table[usk].recv_buff->data_of += r_len;
fprintf(stderr, "DEBUG: r_len = %d\n", r_len);
  }

  if (buf && count) {
    r_len = MIN(count, _sk_table[usk].recv_buff->data_of);
    memcpy((char *)buf, (char *)_sk_table[usk].recv_buff->data, r_len);
    shbuf_trim(_sk_table[usk].recv_buff, r_len);
  }

  return (r_len);
}

shbuf_t *shnet_read_buf(int fd)
{
  unsigned int usk = (unsigned int)fd;

  if (-1 == shnet_read(fd, NULL, MIN_READ_BUFF_SIZE))
    return (NULL);

  return (_sk_table[usk].recv_buff);
}

