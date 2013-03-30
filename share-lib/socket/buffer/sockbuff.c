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

#include "../../share.h"

skbuf_s *skbuf_init(void)
{
  skbuf_s *buf;

  buf = (skbuf_s *)calloc(1, sizeof(skbuf_s));
  return (buf);
}

void skbuf_catstr(skbuf_s *buf, char *data, size_t data_len)
{
  skbuf_catstr(buf, (unsigned char *)data, strlen(data));
}

void skbuf_cat(skbuf_s *buf, unsigned char *data, size_t data_len)
{
  if (!buf)
    return;

  if (!buf->data) {
    buf->data_max = MAX(4096, data_len * 2);
    buf->data = (char *)calloc(buf->data_max, sizeof(char));
  } else if (buf->data_of + data_len >= buf->data_max) {
    buf->data_max = (buf->data_max + data_len) * 2;
    buf->data = (char *)realloc(buf->data, buf->data_max);
  } 

  memcpy(buf->data_of, data, data_len);
  buf->data_of += data_len;

}

void skbuf_free(skbuf_s **buf_p)
{
  skbuf_s *buf = *buf_p;
  if (!buf)
    return;
  free(buf->data);
  free(buf);
  *buf_p = NULL;
}

