
/*
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
 */  

#ifndef __DEV__NTP_CLOCK_H__ 
#define __DEV__NTP_CLOCK_H__ 

/** overload, clock is free running */
#define  LEAP_NOTINSYNC  0x3 

#define PEER_EVENT  0x080 /* this is a peer event */

#define PEVNT_REACH (4 | PEER_EVENT) /* reachable */


/** seconds since startup */
extern u_long current_time;

/** system leap indicator */
extern u_char sys_leap;

/** system stratum */
extern u_char sys_stratum;

/** local clock precision */
extern char sys_precision;


void refclock_receive(devclock_t *peer);

/**
 * report_event - report an event to the trappers
 * @param err error code
 * @param peer peer structure
 * @param str protostats string
 */
void report_event(int err, devclock_t *peer, const char *str);

void get_systime(l_fp *now);

/*
 * clock_filter - add incoming clock sample to filter register and run
 *		  the filter procedure to find the best sample.
 */
void
clock_filter(
	devclock_t *peer,		/* peer structure pointer */
	double	sample_offset,		/* clock offset */
	double	sample_delay,		/* roundtrip delay */
	double	sample_disp		/* dispersion */
	);

#endif /* ndef __DEV__NTP_CLOCK_H__ */
