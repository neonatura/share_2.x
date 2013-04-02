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

shkey_t shsvn_init_keystr(char *kvalue)
{
  return ((shkey_t)shfs_adler64(kvalue, strlen(kvalue) + 1));
}

shkey_t shsvn_init_keynum(long kvalue)
{
  static shkey_t ret_key;
  char buf[256];
  
  memset(buf, 0, sizeof(buf));
  memcpy(buf, kvalue, sizeof(kvalue));

  return (*((shkey_t *)buf));
}

shkey_t shsvn_init_key(void)
{
  return ((shkey_t)shfs_time64());
}
