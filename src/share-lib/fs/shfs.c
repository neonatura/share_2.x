/*
 *  Copyright 2013 Brian Burrell 
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
*/  

#include "share.h"

char *shfs_app_name(char *app_name)
{
  char *ptr;

  if (!app_name)
    return (NULL);

  ptr = strrchr(app_name, '/'); 
  if (ptr)
    app_name = ptr + 1;

  return (app_name);
}

_TEST(shfs_app_name)
{
  char buf[256];
  char *path;
  
  strcpy(buf, "a/a/a/a/a/a/a/a/a/a/a/a/a/a"); 
  _TRUEPTR(path = shfs_app_name(buf));
  if (!path)
    return;
  _TRUE(0 == strcmp(path, "a"));
}

