
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


/* 01/01/2000 UTC */
#define SHTIME_EPOCH 946684800 


/** Obtain 60-bit time-stamp value in local machine-byte order. */ 
#define shtime_value(_stamp) \
  (ntohll(_stamp) & 0xFFFFFFFFFFFFFF)

/**
 * Set the precision attribute of a share time-stamp.
 * @note precision is defined in first 4bits. 
 */
#define shtime_prec_set(_stamp, _prec) \
  (*((char *)&(_stamp)) = (char)(_prec % 2))

/**
 * The libshare representation of a particular time.
 */
typedef uint64_t shtime_t;

/** the degree of precision of the time-stamp. */
int shtime_prec(shtime_t stamp);

/**
 * Generate a float-point precision representation of the specified time.
 * @returns an double representing the milliseconds since 2014 UTC.
 */
double shtimef(shtime_t stamp);

/**
 * Generate a 64bit representation integral of the current time with millisecond precision.
 * @returns an unsigned long representing the milliseconds since 2014 UTC.
 * @note 32bit friendly.
 */
shtime_t shtime(void);

/**
 * Generate a 64bit 'libshare time-stamp' given a unix epoch value.
 */
shtime_t shtimeu(time_t unix_t);

/**
 * Obtain the milliseconds portion of the time specified.
 */
int shtimems(shtime_t t);

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

shtime_t shgettime(struct timeval *tv);

int shtime_after(shtime_t stamp, shtime_t cmp_stamp);

int shtime_before(shtime_t stamp, shtime_t cmp_stamp);

/**
 * @}
 */

#endif /* ndef __SHTIME_H__ */


