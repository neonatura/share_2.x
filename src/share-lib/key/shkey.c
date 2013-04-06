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

shkey_t shkey_init_str(char *kvalue)
{
  return ((shkey_t)shfs_adler64(kvalue, strlen(kvalue) + 1));
}

_TEST(shkey_init_str)
{
  shkey_t keys[26];
  char buf[256];
  int i, j;

  memset(buf, 0, sizeof(buf));
  for (i = 0; i < 26; i++) {
    memset(buf, 'a' + i, sizeof(buf) - 1);
    keys[i] = shkey_init_str(buf);
  }

  for (i = 0; i < 26; i++) {
    for (j = (i + 1); j < 26; j++) {
      CuAssertTrue(ct, keys[i] != keys[j]);
    }
  }

}

shkey_t shkey_init_num(long kvalue)
{
  static shkey_t ret_key;
  char buf[256];
  
  memset(buf, 0, sizeof(buf));
  memcpy(buf, &kvalue, sizeof(kvalue));

  return (*((shkey_t *)buf));
}

_TEST(shkey_init_num)
{
  shkey_t keys[1024];
  long num;
  int i, j;

  num = 0;
  for (i = 0; i < 1024; i++) {
    num += (long)(rand() % 1024);
    keys[i] = shkey_init_num(num);
  }

  for (i = 0; i < 1024; i++) {
    for (j = (i + 1); j < 1024; j++) {
      CuAssertTrue(ct, keys[i] != keys[j]);
    }
  }

}

shkey_t shkey_init_unique(void)
{
  static uniq_off;
  return ((shkey_t)(shtime64() + uniq_off++));
}

_TEST(shkey_init_unique)
{
  shkey_t key1 = shkey_init_unique();
  shkey_t key2 = shkey_init_unique();
  CuAssertTrue(ct, key1 != key2);
}


