
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



#ifndef __SHTIME_H__
#define __SHTIME_H__

/**
 * Time operations are performed in order to store and compare timestamps in the sharefs file system and for network operations. 
 * @brief libshare_time Time calculating operations.
 * @addtogroup libshare
 * @{
 */

/**
 * The libshare representation of a particular time.
 */
typedef uint64_t shtime_t;

/**
 * Generate a float-point precision representation of the current time.
 * @returns an double representing the milliseconds since 2012 UTC.
 */
double shtime(void);

/**
 * Generate a 64bit representation integral of the current time with millisecond precision.
 * @returns an unsigned long representing the milliseconds since 2012 UTC.
 */
shtime_t shtime64(void);
/* shtime_t shtime(), double shtimef() */
/**
 * @}
 */

#endif /* ndef __SHTIME_H__ */


