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

int shclose(int sk)
{
  unsigned int usk;
  int err;

  if (sk < 1)
    return;

  err = close(sk);

  usk = (unsigned short)sk;
	_sk_table[usk].fd = 0;
	_sk_table[usk].flags = 0;
  
  if (_sk_table[usk].recv_buff)
    skbuf_free(&_sk_table[usk].recv_buff);
  if (_sk_table[usk].send_buff)
    skbuf_free(&_sk_table[usk].send_buff);

  return (err);
}
