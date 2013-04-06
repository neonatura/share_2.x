
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

double shtime(void)
{
  struct timeval tv;
  double stamp;

  gettimeofday(&tv, NULL);
  tv.tv_sec -= 1325397600; /* 2012 */ 
  stamp = (double)(tv.tv_sec * 1000) + (double)(tv.tv_usec / 1000);

  return (stamp);
}

_TEST(shtime)
{
  CuAssertTrue(ct, shtime() > 31622400); /* > 1 year */
}

shtime_t shtime64(void)
{
  return ((shtime_t)shtime());
}

_TEST(shtime_64)
{
  double ftime = fabs(shtime()); 
  uint64_t ltime = shtime64();
  CuAssertTrue(ct, (uint64_t)ftime == ltime);
}


