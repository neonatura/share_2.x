/* System-dependent calls for tar.

   Copyright 2003-2008, 2010, 2013-2014 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 3, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
   Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <system.h>

#include "common.h"
#include <signal.h>
#include <wordsplit.h>



bool
sys_get_archive_stat (void)
{
  return 0;
}

bool
sys_file_is_archive (struct tar_stat_info *p)
{
  return false;
}

void
sys_detect_dev_null_output (void)
{
}


void
sys_spawn_shell (void)
{
}

/* stat() in djgpp's C library gives a constant number of 42 as the
   uid and gid of a file.  So, comparing an FTP'ed archive just after
   unpack would fail on MSDOS.  */

bool
sys_compare_uid (struct stat *a, struct stat *b)
{
  return true;
}

bool
sys_compare_gid (struct stat *a, struct stat *b)
{
  return true;
}

bool
sys_compare_links (struct stat *link_data, struct stat *stat_data)
{
  return true;
}

int
sys_truncate (int fd)
{
  return write (fd, "", 0);
}

size_t
sys_write_archive_buffer (void)
{
  return arch_buffer_write(archive, record_start->buffer, record_size);
}

/* Set ARCHIVE for writing, then compressing an archive.  */
void
sys_child_open_for_compress (void)
{
//  FATAL_ERROR ((0, 0, _("Cannot use compressed or remote archives")));
}

/* Set ARCHIVE for uncompressing, then reading an archive.  */
void
sys_child_open_for_uncompress (void)
{
  //FATAL_ERROR ((0, 0, _("Cannot use compressed or remote archives")));
}

