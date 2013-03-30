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

#include "../share.h"

ssize_t shread(int fd, const void *buf, size_t count)
{
  unsigned int usk = (unsigned int)fd;
  ssize_t r_len;
  size_t len;

  if (!_sk_table[usk].recv_buff && count < 4096)
    return (read(fd, buf, count));
  
  if (!_sk_table[usk].recv_buff)
    _sk_table[usk].recv_buff = skbuf_init();

  skbuf_grow(_sk_table[usk].recv_buff, count);
  r_len = read(fd, _sk_table[usk].recv_buff->data + _sk_table[usk].recv_buff->data_of, _sk_table[usk].recv_buff->data_max - _sk_table[usk].recv_buff->data_of);
  if (r_len < 1)
    return (r_len);
  _sk_table[usk].recv_buff->data_of += r_len;

  if (buf && count) {
    r_len = MIN(count, _sk_table[usk].recv_buff->data_of);
    memcpy(buf, _sk_table[usk].recv_buff->data, r_len);
    skbuf_trim(_sk_table[usk].recv_buff, r_len);
  }

  return (r_len);
}


