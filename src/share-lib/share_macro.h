
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
 *  @file share.h 
 *  @brief The Share Library
 *
 *  Provides: Optimized file system, IPC, and network operations.
 *  Used By: Client programs.
*/  


#ifndef __SHARE_MACRO_H__
#define __SHARE_MACRO_H__





#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif

#ifndef MIN
#define MIN(a,b) \
  (a < b ? a : b)
#endif

#ifndef MAX
#define MAX(a,b) \
  (a > b ? a : b)
#endif

#ifdef HAVE_MKDIR
#define MKDIR(_path) (mkdir(_path, 0777))
#else
#define MKDIR(_path)
#endif

#ifndef STAT 
#define STAT(_path, _info) (-1) /* no-op */
#endif

#ifndef FCNTL
#define FCNTL(_fd, _mode, _opt) (-1)
#endif

#endif /* ndef __SHARE_MACRO_H__ */
