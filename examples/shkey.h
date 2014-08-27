
/*
 * @copyright
 *
 *  Copyright 2014 Neo Natura
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
 *
 *  @endcopyright
*/  

#ifndef __EXAMPLES__SHKEY_H__
#define __EXAMPLES__SHKEY_H__


#define RUN_NONE 0
#define RUN_LIST 1
#define RUN_EXPORT 2
#define RUN_IMPORT 3
#define RUN_GENERATE 4
#define RUN_VERIFY 5

#define PROGRAM_NAME "shkey"


typedef struct keystore_t 
{
  shtime_t stamp;
  shkey_t context;
} keystore_t;


#endif /* ndef __EXAMPLES__SHKEY_H__ */


