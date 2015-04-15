
/*
 * @copyright
 *
 *  Copyright 2013 Neo Natura 
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



#ifndef __SHTIME_H__
#define __SHTIME_H__

#include <time.h>

/**
 * Time operations are performed in order to store and compare timestamps in the sharefs file system and for network operations. 
 * @brief libshare_time Time calculating operations.
 * @addtogroup libshare
 * @{
 */

/**
 * One second in shtime64() format.
 */
#define SHTIME64_ONE_SECOND 10

/**
 * The libshare representation of a particular time.
 */
typedef uint64_t shtime_t;

/**
 * Generate a float-point precision representation of the current time.
 * @returns an double representing the milliseconds since 2014 UTC.
 */
double shtimef(void);

/**
 * Generate a 64bit representation integral of the current time with millisecond precision.
 * @returns an unsigned long representing the milliseconds since 2014 UTC.
 * @note 32bit friendly.
 */
shtime_t shtime64(void);
/* shtime_t shtime(), double shtimef() */

/**
 * Generate a 64bit 'libshare time-stamp' given a unix epoch value.
 */
shtime_t shtimeu(time_t unix_t);

/**
 * Display full ISO 8601 format of date and time.
 */
char *shctime(shtime_t t);



/**
 * Convert a share library timestamp into a unix timestamp.
 */
time_t shutime(shtime_t t);

/**
 * Convert a libshare time-stamp into a string format.
 * @param fmt If NULL then "%x %X" will be used.
 * @note This function utilizes the same time tokens as strftime().
 */
char *shstrtime(shtime_t t, char *fmt);

shtime_t shtime_adj(shtime_t stamp, double secs);

/**
 * @}
 */

#endif /* ndef __SHTIME_H__ */


