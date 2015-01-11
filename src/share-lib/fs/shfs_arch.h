

#ifndef __SHFS_ARCH_H__
#define __SHFS_ARCH_H__

enum archive_format
{
  DEFAULT_FORMAT,		/* format to be decided later */
  V7_FORMAT,			/* old V7 tar format */
  OLDGNU_FORMAT,		/* GNU format as per before tar 1.12 */
  USTAR_FORMAT,                 /* POSIX.1-1988 (ustar) format */
  POSIX_FORMAT,			/* POSIX.1-2001 format */
  STAR_FORMAT,                  /* Star format defined in 1994 */
  GNU_FORMAT			/* Same as OLDGNU_FORMAT with one exception:
                                   see FIXME note for to_chars() function
                                   (create.c:189) */
};

#endif
