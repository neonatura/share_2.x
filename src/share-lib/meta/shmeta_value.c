/*
 * @copyright
 *
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
 *
 *  @endcopyright
 *
 */

#include "share.h"



shmeta_value_t *shmeta_str(char *str)
{
  static shbuf_t *buff;
  shmeta_value_t meta;
  char *data;

  if (!str)
    return (NULL);

  if (!buff) {
    buff = shbuf_init();
    if (!buff)
      return (NULL);
  }

  memset(&meta, 0, sizeof(meta));
  meta.pf = SHPF_STRING;

  shbuf_clear(buff);
  shbuf_cat(buff, &meta, sizeof(meta));
  shbuf_catstr(buff, str);

  return (buff->data);
}

_TEST(shmeta_str)
{
  shmeta_value_t *val;
  char buf[1024];

  memset(buf, 0, sizeof(buf));
  memset(buf, 'a', 1023);
  val = (shmeta_value_t *)shmeta_str(buf);
  CuAssertPtrNotNull(ct, val);
  if (!val)
    return;
  CuAssertTrue(ct, val->pf == SHPF_STRING);
  CuAssertTrue(ct, 0 == memcmp((char *)val->raw, buf, 1023));
  free(val);
}


