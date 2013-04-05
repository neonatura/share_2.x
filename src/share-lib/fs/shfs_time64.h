
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
 */  



#ifndef __FS__SHFS_TIME32_H__
#define __FS__SHFS_TIME32_H__

/**
 * @addtogroup libshare_fs
 * @{
 */

/**
 * Generate a 64bit representation of the current time with millisecond precision.
 * @ returns an unsigned long repsenting the milliseconds since 1970 UTC.
 */
uint64_t shfs_time64(void);

/**
 * @}
 */

#endif /* ndef __FS__SHFS_TIME32_H__ */


