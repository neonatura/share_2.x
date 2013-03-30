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

#ifndef __BUFFER__SOCKBUFF_H__
#define __BUFFER__SOCKBUFF_H__

typedef struct skbuf_s {
  unsigned char *data;
  size_t data_of;
  size_t data_max;
} skbuf_s;

skbuf_s *skbuf_init(void);
void skbuf_catstr(skbuf_s *buf, char *data, size_t data_len);
void skbuf_cat(skbuf_s *buf, unsigned char *data, size_t data_len);
void skbuf_free(skbuf_s **buf_p);

#endif /* ndef __BUFFER__SOCKBUFF_H__ */

