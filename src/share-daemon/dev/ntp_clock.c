
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

#include "sharedaemon.h"

/*
 * Operations for jitter calculations (these use doubles).
 *
 * Note that we carefully separate the jitter component from the
 * dispersion component (frequency error plus precision). The frequency
 * error component is computed as CLOCK_PHI times the difference between
 * the epoch of the time measurement and the reference time. The
 * precision component is computed as the square root of the mean of the
 * squares of a zero-mean, uniform distribution of unit maximum
 * amplitude. Whether this makes statistical sense may be arguable.
 */
#define SQUARE(x) ((x) * (x))
#define SQRT(x) (sqrt(x))
#define DIFF(x, y) (SQUARE((x) - (y)))
#define LOGTOD(a) ldexp(1., (int)(a)) /* log2 to double */
#define UNIVAR(x) (SQUARE(.28867513 * LOGTOD(x))) /* std uniform distr */
#define ULOGTOD(a)  ldexp(1., (int)(a)) /* ulog2 to double */

#define FRIC    65536.0     /* 2^16 as a double */
#define DTOFP(r)  ((s_fp)((r) * FRIC))
#define DTOLFP(d, v)  M_DTOLFP((d), (v)->Ul_i.Xl_ui, (v)->l_uf)

/* Local Clock (Client) Variables */

typedef struct Local_Clock_Info {
    double local_time;    /* Client disciplined time */
    double adj;     /* Remaining time correction */
    double slew;    /* Correction Slew Rate */
    double last_read_time;  /* Last time the clock was read */
} local_clock_info;

local_clock_info simclock;  /* Local Clock Variables */



u_long current_time;
u_char sys_leap;
u_char sys_stratum;
char sys_precision;


/**
 * Compare two doubles - used with qsort()
 */
static int refclock_cmpl_fp( const void *p1, const void *p2)
{
  const double *dp1 = (const double *)p1;
  const double *dp2 = (const double *)p2;

  if (*dp1 < *dp2)
    return -1;
  if (*dp1 > *dp2)
    return 1;
  return 0;
}

/*
 * refclock_sample - process a pile of samples from the clock
 *
 * This routine implements a recursive median filter to suppress spikes
 * in the data, as well as determine a performance statistic. It
 * calculates the mean offset and RMS jitter. A time adjustment
 * fudgetime1 can be added to the final offset to compensate for various
 * systematic errors. The routine returns the number of samples
 * processed, which could be zero.
 *
 * @param pp refclock structure pointer
 */
static int refclock_sample(struct refclockproc *pp)
{
  size_t  i, j, k, m, n;
  double  off[MAXSTAGE];
  double  offset;

  /*
   * Copy the raw offsets and sort into ascending order. Don't do
   * anything if the buffer is empty.
   */
  n = 0;
  while (pp->codeproc != pp->coderecv) {
    pp->codeproc = (pp->codeproc + 1) % MAXSTAGE;
    off[n] = pp->filter[pp->codeproc];
    n++;
  }
  if (n == 0)
    return (0);

  if (n > 1)
    qsort(off, n, sizeof(off[0]), refclock_cmpl_fp);

  /*
   * Reject the furthest from the median of the samples until
   * approximately 60 percent of the samples remain.
   */
  i = 0; j = n;
  m = n - (n * 4) / 10;
  while ((j - i) > m) {
    offset = off[(j + i) / 2];
    if (off[j - 1] - offset < offset - off[i])
      i++;  /* reject low end */
    else
      j--;  /* reject high end */
  }

  /*
   * Determine the offset and jitter.
   */
  pp->offset = 0;
  pp->jitter = 0;
  for (k = i; k < j; k++) {
    pp->offset += off[k];
    if (k > i)
      pp->jitter += SQUARE(off[k] - off[k - 1]);
  }
  pp->offset /= m;
  pp->jitter = MAX(SQRT(pp->jitter / m), LOGTOD(sys_precision));
#ifdef DEBUG
  if (debug)
    printf(
        "refclock_sample: n %d offset %.6f disp %.6f jitter %.6f\n",
        (int)n, pp->offset, pp->disp, pp->jitter);
#endif
  return (int)n;
}

void refclock_receive(devclock_t *peer)
{
  struct refclockproc *pp;

  /*
   * Do a little sanity dance and update the peer structure. Groom
   * the median filter samples and give the data to the clock
   * filter.
   */
  pp = peer->procptr;
  peer->leap = pp->leap;
  if (peer->leap == LEAP_NOTINSYNC)
    return;

  peer->received++;
  peer->timereceived = current_time;
  if (!peer->reach) {
    report_event(PEVNT_REACH, peer, NULL);
    peer->timereachable = current_time;
  }
  peer->reach |= 1;
  peer->reftime = pp->lastref;
  peer->aorg = pp->lastrec;
  peer->rootdisp = pp->disp;
  get_systime(&peer->dst);
  if (!refclock_sample(pp))
    return;

  clock_filter(peer, pp->offset, 0., pp->jitter);
}

void report_event(int err, devclock_t *peer, const char *str)
{
}

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
	)
{
}


/**
 * get_systime - return the system time in NTP timestamp format 
 * @param now current system time in l_fp
 */
void get_systime(l_fp *now)
{
  /*
   * To fool the code that determines the local clock precision,
   * we advance the clock a minimum of 200 nanoseconds on every
   * clock read. This is appropriate for a typical modern machine
   * with nanosecond clocks. Note we make no attempt here to
   * simulate reading error, since the error is so small. This may
   * change when the need comes to implement picosecond clocks.
   */
  if (simclock.local_time == simclock.last_read_time)
    simclock.local_time += 200e-9;

  simclock.last_read_time = simclock.local_time;
  DTOLFP(simclock.local_time, now);
}

