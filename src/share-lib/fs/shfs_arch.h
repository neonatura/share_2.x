

/*
 * @copyright
 *
 *  Copyright 2015 Neo Natura
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
 *
 */  

#include "share.h"


#ifndef __SHFS_ARCH_H__
#define __SHFS_ARCH_H__

#include "arch/common.h"

/* Size of each record in blocks */
#define BLOCKING_FACTOR 20

#define TIME_TO_CHARS(arch, val, where) \
  shfs_arch_time_to_chars(arch, val, where, sizeof (where))
#define OFF_TO_CHARS(_arch, val, where) off_to_chars((_arch), val, where, sizeof (where))
#define GID_TO_CHARS(_arch, val, where) gid_to_chars((_arch), val, where, sizeof (where))
#define MAJOR_TO_CHARS(_arch, val, where) major_to_chars ((_arch), val, where, sizeof (where))
#define MINOR_TO_CHARS(_arch, val, where) minor_to_chars ((_arch), val, where, sizeof (where))
#define MODE_TO_CHARS(_arch, val, where) mode_to_chars ((_arch), val, where, sizeof (where))

#define UID_TO_CHARS(_arch, val, where) \
  uid_to_chars ((_arch), val, where, sizeof (where))

#define UNAME_TO_CHARS(_arch, name,buf) string_to_chars(name, buf, sizeof(buf))
#define GNAME_TO_CHARS(_arch, name,buf) string_to_chars(name, buf, sizeof(buf))

#define MAX_VAL_WITH_DIGITS(digits, bits_per_digit) \
   ((digits) * (bits_per_digit) < sizeof (uintmax_t) * CHAR_BIT \
    ? ((uintmax_t) 1 << ((digits) * (bits_per_digit))) - 1 \
    : (uintmax_t) -1)

/* The maximum uintmax_t value that can be represented with octal
   digits and a trailing NUL in BUFFER.  */
#define MAX_OCTAL_VAL(buffer) MAX_VAL_WITH_DIGITS (sizeof (buffer) - 1, LG_8)

#define MAKE_INCL_OPTIONS(args) \
    ( recursion_option )

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


typedef struct tar_stat_info sharch_ent_t;

/* Number of links a file can have without having to be entered into
   the link table.  Typically this is 1, but in trickier circumstances
   it is 0.  */
extern nlink_t trivial_link_count;


int shfs_arch(char *tar_fname, char **tar_files);

int tar_timespec_cmp (struct timespec a, struct timespec b);

bool shfs_arch_time_to_chars(shfs_arch_t *arch, time_t v, char *p, size_t s);

int shfs_arch_buffer_read(shbuf_t *buff, void *data, size_t data_len);

union block *shfs_arch_start_header(shfs_arch_t *arch, struct tar_stat_info *st);

void shfs_arch_print_header(shfs_arch_t *arch, struct tar_stat_info *st, union block *blk, off_t block_ordinal);

void shfs_arch_skip_member(shfs_arch_t *arch);

void shfs_arch_init_buffer(shfs_arch_t *arch);

void shfs_arch_open_archive(shfs_arch_t *arch, int mode);

int shfs_arch_set_next_block_after(shfs_arch_t *arch, union block *block);

union block *shfs_arch_buffer_next(shfs_arch_t *arch);


#endif
