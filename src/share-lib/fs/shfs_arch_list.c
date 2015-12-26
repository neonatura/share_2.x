
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
#include "shfs_arch.h"

union block *current_header;	/* points to current archive header */
enum archive_format current_format; /* recognized format */
union block *recent_long_name;	/* recent long name header and contents */
union block *recent_long_link;	/* likewise, for long link */
size_t recent_long_name_blocks;	/* number of blocks in recent_long_name */
size_t recent_long_link_blocks;	/* likewise, for long link */
static union block *recent_global_header; /* Recent global header block */

#define GID_FROM_HEADER(where) gid_from_header (where, sizeof (where))
#define MAJOR_FROM_HEADER(where) major_from_header (where, sizeof (where))
#define MINOR_FROM_HEADER(where) minor_from_header (where, sizeof (where))
#define MODE_FROM_HEADER(where, hbits) \
  mode_from_header (where, sizeof (where), hbits)
#define TIME_FROM_HEADER(where) time_from_header (where, sizeof (where))
#define UID_FROM_HEADER(where) uid_from_header (where, sizeof (where))

static gid_t gid_from_header (const char *buf, size_t size);
static major_t major_from_header (const char *buf, size_t size);
static minor_t minor_from_header (const char *buf, size_t size);
static mode_t mode_from_header (const char *buf, size_t size, bool *hbits);
static time_t time_from_header (const char *buf, size_t size);
static uid_t uid_from_header (const char *buf, size_t size);
static intmax_t from_header (const char *, size_t, const char *,
			     intmax_t, uintmax_t, bool, bool);

/* Base 64 digits; see Internet RFC 2045 Table 1.  */
static char const base_64_digits[64] =
{
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

/* Table of base-64 digit values indexed by unsigned chars.
   The value is 64 for unsigned chars that are not base-64 digits.  */
static char base64_map[UCHAR_MAX + 1];

static void
base64_init (void)
{
  int i;
  memset (base64_map, 64, sizeof base64_map);
  for (i = 0; i < 64; i++)
    base64_map[(int) base_64_digits[i]] = i;
}

static int _extract_file(shfs_arch_t *arch, shfs_t *fs, char *file_name, int typeflag)
{
  off_t size;
  union block *data_block;
  int status;
  size_t count;
  size_t written;
int file_created;
  //bool interdir_made = false;
  mode_t mode = (current_stat_info.stat.st_mode & MODE_RWX
		 & ~ (0 < same_owner_option ? S_IRWXG | S_IRWXO : 0));
  //mode_t invert_permissions = 0 < same_owner_option ? mode & (S_IRWXG | S_IRWXO) : 0;
  mode_t current_mode = 0;
  mode_t current_mode_mask = 0;
  int fd;

file_created = 0;
  fd = shopen(file_name, "wb", fs);
  if (fd < 0)
    return -1;

  mv_begin_read (&current_stat_info);
  for (size = current_stat_info.stat.st_size; size > 0; ) {
    mv_size_left (size);

    /* Locate data, determine max length writeable, write it,
       block that we have used the data, then check if the write
       worked.  */

    data_block = shfs_arch_buffer_next(arch);
    if (! data_block)
    {
      //	    ERROR ((0, 0, _("Unexpected EOF in archive")));
      break;		/* FIXME: What happens, then?  */
    }

    written = available_space_after (data_block);

    if (written > size)
      written = size;
    errno = 0;
    count = shwrite(fd, data_block->buffer, written);//blocking_write (fd, data_block->buffer, written);
    size -= written;

    shfs_arch_set_next_block_after(arch, (union block *)(data_block->buffer + written - 1));
    if (count != written)
    {
      if (!to_command_option)
      {}
      /* FIXME: shouldn't we restore from backup? */
      break;
    }
  }

  shfs_arch_skip_file (arch, size);

  mv_end ();

  status = shclose (fd);
  if (status < 0)
return status;


  return status;
}

/* Skip the current member in the archive.
   NOTE: Current header must be decoded before calling this function. */
void shfs_arch_skip_member(shfs_arch_t *arch)
{

  if (!current_stat_info.skipped) {
    shfs_arch_set_next_block_after(arch, current_header);
    mv_begin_read (&current_stat_info);
    mv_end ();
  }

}

static int _shfs_arch_extract_file(shfs_arch_t *arch, shfs_ino_t *file)
{
  shfs_t *fs;
  char path[PATH_MAX+1];
  char typeflag;
//  tar_extractor_t fun;

  shfs_arch_set_next_block_after(arch, current_header);

  if (!current_stat_info.file_name[0])
  {
    shfs_arch_skip_member(arch);
    return;
  }

  typeflag = 0;
  fs = shfs_inode_tree(file);
  snprintf(path, sizeof(path)-1, "%s%s",
      shfs_inode_path(file), current_stat_info.file_name);
  _extract_file(arch, fs, path, typeflag);

}

#define ISOCTAL(c) ((c)>='0'&&(c)<='7')

/* Decode things from a file HEADER block into STAT_INFO, also setting
   *FORMAT_POINTER depending on the header block format.  If
   DO_USER_GROUP, decode the user/group information (this is useful
   for extraction, but waste time when merely listing).

   read_header() has already decoded the checksum and length, so we don't.

   This routine should *not* be called twice for the same block, since
   the two calls might use different DO_USER_GROUP values and thus
   might end up with different uid/gid for the two calls.  If anybody
   wants the uid/gid they should decode it first, and other callers
   should decode it without uid/gid before calling a routine,
   e.g. print_header, that assumes decoded data.  */
static void shfs_arch_decode_header(shfs_arch_t *arch, union block *header, struct tar_stat_info *stat_info, enum archive_format *format_pointer, int do_user_group)
{
  enum archive_format format;
  bool hbits;
  mode_t mode = MODE_FROM_HEADER (header->header.mode, &hbits);

  if (strcmp (header->header.magic, TMAGIC) == 0)
    {
      if (header->star_header.prefix[130] == 0
	  && ISOCTAL (header->star_header.atime[0])
	  && header->star_header.atime[11] == ' '
	  && ISOCTAL (header->star_header.ctime[0])
	  && header->star_header.ctime[11] == ' ')
	format = STAR_FORMAT;
      else if (stat_info->xhdr.size)
	format = POSIX_FORMAT;
      else
	format = USTAR_FORMAT;
    }
  else if (strcmp (header->buffer + offsetof (struct posix_header, magic),
		   OLDGNU_MAGIC)
	   == 0)
    format = hbits ? OLDGNU_FORMAT : GNU_FORMAT;
  else
    format = V7_FORMAT;
  *format_pointer = format;

  stat_info->stat.st_mode = mode;
  stat_info->mtime.tv_sec = TIME_FROM_HEADER (header->header.mtime);
  stat_info->mtime.tv_nsec = 0;
  assign_string (&stat_info->uname,
		 header->header.uname[0] ? header->header.uname : NULL);
  assign_string (&stat_info->gname,
		 header->header.gname[0] ? header->header.gname : NULL);


  if (format == OLDGNU_FORMAT && incremental_option)
    {
      stat_info->atime.tv_sec = TIME_FROM_HEADER (header->oldgnu_header.atime);
      stat_info->ctime.tv_sec = TIME_FROM_HEADER (header->oldgnu_header.ctime);
      stat_info->atime.tv_nsec = stat_info->ctime.tv_nsec = 0;
    }
  else if (format == STAR_FORMAT)
    {
      stat_info->atime.tv_sec = TIME_FROM_HEADER (header->star_header.atime);
      stat_info->ctime.tv_sec = TIME_FROM_HEADER (header->star_header.ctime);
      stat_info->atime.tv_nsec = stat_info->ctime.tv_nsec = 0;
    }
  else
    stat_info->atime = stat_info->ctime = arch->start_time;

  if (format == V7_FORMAT)
    {
      stat_info->stat.st_uid = UID_FROM_HEADER (header->header.uid);
      stat_info->stat.st_gid = GID_FROM_HEADER (header->header.gid);
      stat_info->stat.st_rdev = 0;
    }
  else
    {
      if (do_user_group)
	{
	  /* FIXME: Decide if this should somewhat depend on -p.  */

	  if (numeric_owner_option
	      || !*header->header.uname
	      || !uname_to_uid (header->header.uname, &stat_info->stat.st_uid))
	    stat_info->stat.st_uid = UID_FROM_HEADER (header->header.uid);

	  if (numeric_owner_option
	      || !*header->header.gname
	      || !gname_to_gid (header->header.gname, &stat_info->stat.st_gid))
	    stat_info->stat.st_gid = GID_FROM_HEADER (header->header.gid);
	}

      switch (header->header.typeflag)
	{
	case BLKTYPE:
	case CHRTYPE:
	  stat_info->stat.st_rdev =
	    makedev (MAJOR_FROM_HEADER (header->header.devmajor),
		     MINOR_FROM_HEADER (header->header.devminor));
	  break;

	default:
	  stat_info->stat.st_rdev = 0;
	}
    }

  stat_info->archive_file_size = stat_info->stat.st_size;

  stat_info->is_sparse = false;
  if (((current_format == GNU_FORMAT
          || current_format == OLDGNU_FORMAT)
        && current_header->header.typeflag == GNUTYPE_DUMPDIR)
      || stat_info->dumpdir)
    stat_info->is_dumpdir = true;
}

int shfs_arch_extract_archive(shfs_arch_t *arch, shfs_ino_t *file)
{
  enum read_header status = HEADER_STILL_UNREAD;
  enum read_header prev_status;
  struct timespec mtime;

  base64_init ();
  name_gather ();

  shfs_arch_open_archive(arch, ACCESS_READ);//open_archive (ACCESS_READ);
  do
  {
    prev_status = status;
    tar_stat_destroy (&current_stat_info);

    status = shfs_arch_read_header(arch, &current_header, &current_stat_info, read_header_auto);
    switch (status)
    {
      case HEADER_STILL_UNREAD:
      case HEADER_SUCCESS_EXTENDED:
        abort ();

      case HEADER_SUCCESS:

        /* Valid header.  We should decode next field (mode) first.
           Ensure incoming names are null terminated.  */
        shfs_arch_decode_header(arch, current_header, &current_stat_info, &current_format, 1);
        if (! name_match (current_stat_info.file_name)
            || (NEWER_OPTION_INITIALIZED (newer_mtime_option)
              /* FIXME: We get mtime now, and again later; this causes
                 duplicate diagnostics if header.mtime is bogus.  */
              && ((mtime.tv_sec
                  = TIME_FROM_HEADER (current_header->header.mtime)),
                /* FIXME: Grab fractional time stamps from
                   extended header.  */
                mtime.tv_nsec = 0,
                current_stat_info.mtime = mtime,
                OLDER_TAR_STAT_TIME (current_stat_info, m)))
           )
        {
          switch (current_header->header.typeflag)
          {
            case GNUTYPE_VOLHDR:
            case GNUTYPE_MULTIVOL:
              break;

            case DIRTYPE:
              /* Fall through.  */
            default:
              shfs_arch_skip_member(arch);
              continue;
          }
        }

        transform_stat_info (current_header->header.typeflag,
            &current_stat_info);
        _shfs_arch_extract_file(arch, file);
        continue;

      case HEADER_ZERO_BLOCK:
        if (block_number_option)
        {
        }

        shfs_arch_set_next_block_after(arch, current_header);

        if (!ignore_zeros_option)
        {
          //char buf[UINTMAX_STRSIZE_BOUND];

          status = shfs_arch_read_header(arch, &current_header, &current_stat_info, read_header_auto);
          if (status == HEADER_ZERO_BLOCK)
            break;
          break;
        }
        status = prev_status;
        continue;

      case HEADER_END_OF_FILE:
        if (block_number_option)
        {
        }
        break;

      case HEADER_FAILURE:
        /* If the previous header was good, tell them that we are
           skipping bad ones.  */
        shfs_arch_set_next_block_after(arch, current_header);
        switch (prev_status)
        {
          case HEADER_STILL_UNREAD:
            /* Fall through.  */

          case HEADER_ZERO_BLOCK:
          case HEADER_SUCCESS:
            if (block_number_option)
            {
              //char buf[UINTMAX_STRSIZE_BOUND];
              off_t block_ordinal = current_block_ordinal ();
              block_ordinal -= recent_long_name_blocks;
              block_ordinal -= recent_long_link_blocks;
            }
            break;

          case HEADER_END_OF_FILE:
          case HEADER_FAILURE:
            /* We are in the middle of a cascade of errors.  */
            break;

          case HEADER_SUCCESS_EXTENDED:
            abort ();
        }
        continue;
    }
    break;
  }
  while (!all_names_found (&current_stat_info));

  shfs_arch_close_archive(arch);
  names_notfound ();		/* print names not found */
}

/* List a tar archive, with support routines for reading a tar archive.

   Copyright 1988, 1992-1994, 1996-2001, 2003-2007, 2010, 2012-2014 Free
   Software Foundation, Inc.

   This file is part of GNU tar.

   GNU tar is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   GNU tar is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Written by John Gilmore, on 1985-08-26.  */





static char *
decode_xform (char *file_name, void *data)
{
  int type = *(int*)data;

  switch (type)
    {
    case XFORM_SYMLINK:
      /* FIXME: It is not quite clear how and to which extent are the symbolic
	 links subject to filename transformation.  In the absence of another
	 solution, symbolic links are exempt from component stripping and
	 name suffix normalization, but subject to filename transformation
	 proper. */
      return file_name;

    case XFORM_LINK:
      file_name = safer_name_suffix (file_name, true, absolute_names_option);
      break;

    case XFORM_REGFILE:
      file_name = safer_name_suffix (file_name, false, absolute_names_option);
      break;
    }

  if (strip_name_components)
    {
      size_t prefix_len = stripped_prefix_len (file_name,
					       strip_name_components);
      if (prefix_len == (size_t) -1)
	prefix_len = strlen (file_name);
      file_name += prefix_len;
    }
  return file_name;
}

static bool
transform_member_name (char **pinput, int type)
{
  return transform_name_fp (pinput, type, decode_xform, &type);
}

static void
enforce_one_top_level (char **pfile_name)
{
  char *file_name = *pfile_name;
  char *p;
  
  for (p = file_name; *p && (ISSLASH (*p) || *p == '.'); p++)
    ;

  if (!*p)
    return;

  if (strncmp (p, one_top_level_dir, strlen (one_top_level_dir)) == 0)
    {
      int pos = strlen (one_top_level_dir);
      if (ISSLASH (p[pos]) || p[pos] == 0)
	return;
    }

  *pfile_name = new_name (one_top_level_dir, file_name);
  normalize_filename_x (*pfile_name);
  free (file_name);
}

void
transform_stat_info (int typeflag, struct tar_stat_info *stat_info)
{
  if (typeflag == GNUTYPE_VOLHDR)
    /* Name transformations don't apply to volume headers. */
    return;

  transform_member_name (&stat_info->file_name, XFORM_REGFILE);
  switch (typeflag)
    {
    case SYMTYPE:
      transform_member_name (&stat_info->link_name, XFORM_SYMLINK);
      break;

    case LNKTYPE:
      transform_member_name (&stat_info->link_name, XFORM_LINK);
    }

  if (one_top_level_option)
    enforce_one_top_level (&current_stat_info.file_name);
}

/* Check header checksum */
/* The standard BSD tar sources create the checksum by adding up the
   bytes in the header as type char.  I think the type char was unsigned
   on the PDP-11, but it's signed on the Next and Sun.  It looks like the
   sources to BSD tar were never changed to compute the checksum
   correctly, so both the Sun and Next add the bytes of the header as
   signed chars.  This doesn't cause a problem until you get a file with
   a name containing characters with the high bit set.  So tar_checksum
   computes two checksums -- signed and unsigned.  */

static enum read_header tar_checksum(union block *header, bool silent)
{
  size_t i;
  int unsigned_sum = 0;		/* the POSIX one :-) */
  int signed_sum = 0;		/* the Sun one :-( */
  int recorded_sum;
  int parsed_sum;
  char *p;

  p = header->buffer;
  for (i = sizeof *header; i-- != 0;)
    {
      unsigned_sum += (unsigned char) *p;
      signed_sum += (signed char) (*p++);
    }

  if (unsigned_sum == 0)
    return HEADER_ZERO_BLOCK;

  /* Adjust checksum to count the "chksum" field as blanks.  */

  for (i = sizeof header->header.chksum; i-- != 0;)
    {
      unsigned_sum -= (unsigned char) header->header.chksum[i];
      signed_sum -= (signed char) (header->header.chksum[i]);
    }
  unsigned_sum += ' ' * sizeof header->header.chksum;
  signed_sum += ' ' * sizeof header->header.chksum;

  parsed_sum = from_header (header->header.chksum,
			    sizeof header->header.chksum, 0,
			    0, INT_MAX, true, silent);
  if (parsed_sum < 0)
    return HEADER_FAILURE;

  recorded_sum = parsed_sum;

  if (unsigned_sum != recorded_sum && signed_sum != recorded_sum)
    return HEADER_FAILURE;

  return HEADER_SUCCESS;
}

/* Read a block that's supposed to be a header block.  Return its
   address in *RETURN_BLOCK, and if it is good, the file's size
   and names (file name, link name) in *INFO.

   Return one of enum read_header describing the status of the
   operation.

   The MODE parameter instructs read_header what to do with special
   header blocks, i.e.: extended POSIX, GNU long name or long link,
   etc.:

     read_header_auto        process them automatically,
     read_header_x_raw       when a special header is read, return
                             HEADER_SUCCESS_EXTENDED without actually
			     processing the header,
     read_header_x_global    when a POSIX global header is read,
                             decode it and return HEADER_SUCCESS_EXTENDED.

   You must always set_next_block_after(*return_block) to skip past
   the header which this routine reads.  */

int shfs_arch_read_header(shfs_arch_t *arch, union block **return_block, struct tar_stat_info *info, int mode)
{
  union block *header;
  union block *header_copy;
  char *bp;
  union block *data_block;
  size_t size, written;
  union block *next_long_name = 0;
  union block *next_long_link = 0;
  size_t next_long_name_blocks = 0;
  size_t next_long_link_blocks = 0;

  while (1)
  {
    enum read_header status;

    header = shfs_arch_buffer_next(arch);
    *return_block = header;
    if (!header) {
      return HEADER_END_OF_FILE;
}

    if ((status = tar_checksum (header, false)) != HEADER_SUCCESS) {
      return status;
}

    /* Good block.  Decode file size and return.  */

    if (header->header.typeflag == LNKTYPE)
      info->stat.st_size = 0;	/* links 0 size on tape */
    else
    {
      info->stat.st_size = OFF_FROM_HEADER (header->header.size);
      if (info->stat.st_size < 0)
        return HEADER_FAILURE;
    }

    if (header->header.typeflag == GNUTYPE_LONGNAME
        || header->header.typeflag == GNUTYPE_LONGLINK
        || header->header.typeflag == XHDTYPE
        || header->header.typeflag == XGLTYPE
        || header->header.typeflag == SOLARIS_XHDTYPE)
    {
      if (mode == read_header_x_raw)
        return HEADER_SUCCESS_EXTENDED;
      else if (header->header.typeflag == GNUTYPE_LONGNAME
          || header->header.typeflag == GNUTYPE_LONGLINK)
      {
        size_t name_size = info->stat.st_size;
        size_t n = name_size % BLOCKSIZE;
        size = name_size + BLOCKSIZE;
        if (n)
          size += BLOCKSIZE - n;

        if (name_size != info->stat.st_size || size < name_size)
          xalloc_die ();

        header_copy = malloc (size + 1);

        if (header->header.typeflag == GNUTYPE_LONGNAME)
        {
          free (next_long_name);
          next_long_name = header_copy;
          next_long_name_blocks = size / BLOCKSIZE;
        }
        else
        {
          free (next_long_link);
          next_long_link = header_copy;
          next_long_link_blocks = size / BLOCKSIZE;
        }

        shfs_arch_set_next_block_after(arch, header);
        *header_copy = *header;
        bp = header_copy->buffer + BLOCKSIZE;

        for (size -= BLOCKSIZE; size > 0; size -= written)
        {
          data_block = shfs_arch_buffer_next(arch);
          if (! data_block)
          {
            break;
          }
          written = available_space_after (data_block);
          if (written > size)
            written = size;

          memcpy (bp, data_block->buffer, written);
          bp += written;
          shfs_arch_set_next_block_after(arch, (union block *)(data_block->buffer + written - 1));
        }

        *bp = '\0';
      }
      else if (header->header.typeflag == XHDTYPE
          || header->header.typeflag == SOLARIS_XHDTYPE)
      {
        /* .. */
      }
      else if (header->header.typeflag == XGLTYPE)
      {
        //	      struct xheader xhdr;

        if (!recent_global_header)
          recent_global_header = malloc (sizeof *recent_global_header);
        memcpy (recent_global_header, header,
            sizeof *recent_global_header);
        if (mode == read_header_x_global)
          return HEADER_SUCCESS_EXTENDED;
      }

    }
    else
    {
      char const *name;
      struct posix_header const *h = &header->header;
      char namebuf[sizeof h->prefix + 1 + NAME_FIELD_SIZE + 1];

      free (recent_long_name);

      if (next_long_name)
      {
        name = next_long_name->buffer + BLOCKSIZE;
        recent_long_name = next_long_name;
        recent_long_name_blocks = next_long_name_blocks;
      }
      else
      {
        /* Accept file names as specified by POSIX.1-1996
           section 10.1.1.  */
        char *np = namebuf;

        if (h->prefix[0] && strcmp (h->magic, TMAGIC) == 0)
        {
          memcpy (np, h->prefix, sizeof h->prefix);
          np[sizeof h->prefix] = '\0';
          np += strlen (np);
          *np++ = '/';
        }
        memcpy (np, h->name, sizeof h->name);
        np[sizeof h->name] = '\0';
        name = namebuf;
        recent_long_name = 0;
        recent_long_name_blocks = 0;
      }
      assign_string (&info->orig_file_name, name);
      assign_string (&info->file_name, name);
      info->had_trailing_slash = strip_trailing_slashes (info->file_name);

      free (recent_long_link);

      if (next_long_link)
      {
        name = next_long_link->buffer + BLOCKSIZE;
        recent_long_link = next_long_link;
        recent_long_link_blocks = next_long_link_blocks;
      }
      else
      {
        memcpy (namebuf, h->linkname, sizeof h->linkname);
        namebuf[sizeof h->linkname] = '\0';
        name = namebuf;
        recent_long_link = 0;
        recent_long_link_blocks = 0;
      }
      assign_string (&info->link_name, name);

      return HEADER_SUCCESS;
    }
  }
}



/* Convert buffer at WHERE0 of size DIGS from external format to
   intmax_t.  DIGS must be positive.  If TYPE is nonnull, the data are
   of type TYPE.  The buffer must represent a value in the range
   MINVAL through MAXVAL; if the mathematically correct result V would
   be greater than INTMAX_MAX, return a negative integer V such that
   (uintmax_t) V yields the correct result.  If OCTAL_ONLY, allow only octal
   numbers instead of the other GNU extensions.  Return -1 on error,
   diagnosing the error if TYPE is nonnull and if !SILENT.  */
#if ! (INTMAX_MAX <= UINTMAX_MAX && - (INTMAX_MIN + 1) <= UINTMAX_MAX)
# error "from_header internally represents intmax_t as uintmax_t + sign"
#endif
#if ! (UINTMAX_MAX / 2 <= INTMAX_MAX)
# error "from_header returns intmax_t to represent uintmax_t"
#endif
static intmax_t
from_header (char const *where0, size_t digs, char const *type,
	     intmax_t minval, uintmax_t maxval,
	     bool octal_only, bool silent)
{
  uintmax_t value;
  uintmax_t uminval = minval;
  uintmax_t minus_minval = - uminval;
  char const *where = where0;
  char const *lim = where + digs;
  bool negative = false;

  /* Accommodate buggy tar of unknown vintage, which outputs leading
     NUL if the previous field overflows.  */
  where += !*where;

  /* Accommodate older tars, which output leading spaces.  */
  for (;;)
    {
      if (where == lim)
	{
	  return -1;
	}
      if (!isspace ((unsigned char) *where))
	break;
      where++;
    }

  value = 0;
  if (ISODIGIT (*where))
    {
      char const *where1 = where;
      bool overflow = false;

      for (;;)
	{
	  value += *where++ - '0';
	  if (where == lim || ! ISODIGIT (*where))
	    break;
	  overflow |= value != (value << LG_8 >> LG_8);
	  value <<= LG_8;
	}

      /* Parse the output of older, unportable tars, which generate
         negative values in two's complement octal.  If the leading
         nonzero digit is 1, we can't recover the original value
         reliably; so do this only if the digit is 2 or more.  This
         catches the common case of 32-bit negative time stamps.  */
      if ((overflow || maxval < value) && '2' <= *where1 && type)
	{
	  /* Compute the negative of the input value, assuming two's
	     complement.  */
	  int digit = (*where1 - '0') | 4;
	  overflow = 0;
	  value = 0;
	  where = where1;
	  for (;;)
	    {
	      value += 7 - digit;
	      where++;
	      if (where == lim || ! ISODIGIT (*where))
		break;
	      digit = *where - '0';
	      overflow |= value != (value << LG_8 >> LG_8);
	      value <<= LG_8;
	    }
	  value++;
	  overflow |= !value;

	  if (!overflow && value <= minus_minval)
	    {
	      negative = true;
	    }
	}

      if (overflow)
	{
	  return -1;
	}
    }
  else if (octal_only)
    {
      /* Suppress the following extensions.  */
    }
  else if (*where == '-' || *where == '+')
    {
      /* Parse base-64 output produced only by tar test versions
	 1.13.6 (1999-08-11) through 1.13.11 (1999-08-23).
	 Support for this will be withdrawn in future releases.  */
      int dig;
      negative = *where++ == '-';
      while (where != lim
	     && (dig = base64_map[(unsigned char) *where]) < 64)
	{
	  if (value << LG_64 >> LG_64 != value)
	    {
	      char *string = alloca (digs + 1);
	      memcpy (string, where0, digs);
	      string[digs] = '\0';
	      if (type && !silent)
	      return -1;
	    }
	  value = (value << LG_64) | dig;
	  where++;
	}
    }
  else if (*where == '\200' /* positive base-256 */
	   || *where == '\377' /* negative base-256 */)
    {
      /* Parse base-256 output.  A nonnegative number N is
	 represented as (256**DIGS)/2 + N; a negative number -N is
	 represented as (256**DIGS) - N, i.e. as two's complement.
	 The representation guarantees that the leading bit is
	 always on, so that we don't confuse this format with the
	 others (assuming ASCII bytes of 8 bits or more).  */
      int signbit = *where & (1 << (LG_256 - 2));
      uintmax_t topbits = (((uintmax_t) - signbit)
			   << (CHAR_BIT * sizeof (uintmax_t)
			       - LG_256 - (LG_256 - 2)));
      value = (*where++ & ((1 << (LG_256 - 2)) - 1)) - signbit;
      for (;;)
	{
	  value = (value << LG_256) + (unsigned char) *where++;
	  if (where == lim)
	    break;
	  if (((value << LG_256 >> LG_256) | topbits) != value)
	    {
	      if (type && !silent)
	      return -1;
	    }
	}
      negative = signbit != 0;
      if (negative)
	value = -value;
    }

  if (where != lim && *where && !isspace ((unsigned char) *where))
    {
      if (type)
	{
	  char buf[1000]; /* Big enough to represent any header.  */
	  static struct quoting_options *o;

	  if (!o)
	    {
	      o = clone_quoting_options (0);
	      set_quoting_style (o, locale_quoting_style);
	    }

	  while (where0 != lim && ! lim[-1])
	    lim--;
	  quotearg_buffer (buf, sizeof buf, where0, lim - where0, o);
	}

      return -1;
    }

  if (value <= (negative ? minus_minval : maxval))
    return represent_uintmax (negative ? -value : value);

  if (type && !silent)
    {
      char minval_buf[UINTMAX_STRSIZE_BOUND + 1];
      //char maxval_buf[UINTMAX_STRSIZE_BOUND];
      char value_buf[UINTMAX_STRSIZE_BOUND + 1];
      char *minval_string = STRINGIFY_BIGINT (minus_minval, minval_buf + 1);
      char *value_string = STRINGIFY_BIGINT (value, value_buf + 1);
      if (negative)
	*--value_string = '-';
      if (minus_minval)
	*--minval_string = '-';
      /* TRANSLATORS: Second %s is type name (gid_t,uid_t,etc.) */
    }

  return -1;
}

static gid_t
gid_from_header (const char *p, size_t s)
{
  return from_header (p, s, "gid_t",
		      TYPE_MINIMUM (gid_t), TYPE_MAXIMUM (gid_t),
		      false, false);
}

static major_t
major_from_header (const char *p, size_t s)
{
  return from_header (p, s, "major_t",
		      TYPE_MINIMUM (major_t), TYPE_MAXIMUM (major_t),
		      false, false);
}

static minor_t
minor_from_header (const char *p, size_t s)
{
  return from_header (p, s, "minor_t",
		      TYPE_MINIMUM (minor_t), TYPE_MAXIMUM (minor_t),
		      false, false);
}

/* Convert P to the file mode, as understood by tar.
   Set *HBITS if there are any unrecognized bits.  */
static mode_t
mode_from_header (const char *p, size_t s, bool *hbits)
{
  intmax_t u = from_header (p, s, "mode_t",
			    INTMAX_MIN, UINTMAX_MAX,
			    false, false);
  mode_t mode = ((u & TSUID ? S_ISUID : 0)
		 | (u & TSGID ? S_ISGID : 0)
		 | (u & TSVTX ? S_ISVTX : 0)
		 | (u & TUREAD ? S_IRUSR : 0)
		 | (u & TUWRITE ? S_IWUSR : 0)
		 | (u & TUEXEC ? S_IXUSR : 0)
		 | (u & TGREAD ? S_IRGRP : 0)
		 | (u & TGWRITE ? S_IWGRP : 0)
		 | (u & TGEXEC ? S_IXGRP : 0)
		 | (u & TOREAD ? S_IROTH : 0)
		 | (u & TOWRITE ? S_IWOTH : 0)
		 | (u & TOEXEC ? S_IXOTH : 0));
  *hbits = (u & ~07777) != 0;
  return mode;
}

off_t
off_from_header (const char *p, size_t s)
{
  /* Negative offsets are not allowed in tar files, so invoke
     from_header with minimum value 0, not TYPE_MINIMUM (off_t).  */
  return from_header (p, s, "off_t",
		      0, TYPE_MAXIMUM (off_t),
		      false, false);
}

static time_t
time_from_header (const char *p, size_t s)
{
  return from_header (p, s, "time_t",
		      TYPE_MINIMUM (time_t), TYPE_MAXIMUM (time_t),
		      false, false);
}

static uid_t
uid_from_header (const char *p, size_t s)
{
  return from_header (p, s, "uid_t",
		      TYPE_MINIMUM (uid_t), TYPE_MAXIMUM (uid_t),
		      false, false);
}

uintmax_t
uintmax_from_header (const char *p, size_t s)
{
  return from_header (p, s, "uintmax_t", 0, UINTMAX_MAX, false, false);
}


/* Return a printable representation of T.  The result points to
   static storage that can be reused in the next call to this
   function, to ctime, or to asctime.  If FULL_TIME, then output the
   time stamp to its full resolution; otherwise, just output it to
   1-minute resolution.  */
char const *
tartime (struct timespec t, bool full_time)
{
  enum { fraclen = sizeof ".FFFFFFFFF" - 1 };
  static char buffer[max (UINTMAX_STRSIZE_BOUND + 1,
			  INT_STRLEN_BOUND (int) + 16)
		     + fraclen];
  struct tm *tm;
  time_t s = t.tv_sec;
  int ns = t.tv_nsec;
  bool negative = s < 0;
  char *p;

  if (negative && ns != 0)
    {
      s++;
      ns = 1000000000 - ns;
    }

  tm = utc_option ? gmtime (&s) : localtime (&s);
  if (tm)
    {
      if (full_time)
	{
	  sprintf (buffer, "%04ld-%02d-%02d %02d:%02d:%02d",
		   tm->tm_year + 1900L, tm->tm_mon + 1, tm->tm_mday,
		   tm->tm_hour, tm->tm_min, tm->tm_sec);
	  code_ns_fraction (ns, buffer + strlen (buffer));
	}
      else
	sprintf (buffer, "%04ld-%02d-%02d %02d:%02d",
		 tm->tm_year + 1900L, tm->tm_mon + 1, tm->tm_mday,
		 tm->tm_hour, tm->tm_min);
      return buffer;
    }

  /* The time stamp cannot be broken down, most likely because it
     is out of range.  Convert it as an integer,
     right-adjusted in a field with the same width as the usual
     4-year ISO time format.  */
  p = umaxtostr (negative ? - (uintmax_t) s : s,
		 buffer + sizeof buffer - UINTMAX_STRSIZE_BOUND - fraclen);
  if (negative)
    *--p = '-';
  while ((buffer + sizeof buffer - sizeof "YYYY-MM-DD HH:MM"
	  + (full_time ? sizeof ":SS.FFFFFFFFF" - 1 : 0))
	 < p)
    *--p = ' ';
  if (full_time)
    code_ns_fraction (ns, buffer + sizeof buffer - 1 - fraclen);
  return p;
}

/* Actually print it.

   Plain and fancy file header block logging.  Non-verbose just prints
   the name, e.g. for "tar t" or "tar x".  This should just contain
   file names, so it can be fed back into tar with xargs or the "-T"
   option.  The verbose option can give a bunch of info, one line per
   file.  I doubt anybody tries to parse its format, or if they do,
   they shouldn't.  Unix tar is pretty random here anyway.  */


/* Width of "user/group size", with initial value chosen
   heuristically.  This grows as needed, though this may cause some
   stairstepping in the output.  Make it too small and the output will
   almost always look ragged.  Make it too large and the output will
   be spaced out too far.  */
static int ugswidth = 19;

/* Width of printed time stamps.  It grows if longer time stamps are
   found (typically, those with nanosecond resolution).  Like
   USGWIDTH, some stairstepping may occur.  */
static int datewidth = sizeof "YYYY-MM-DD HH:MM" - 1;

static bool volume_label_printed = false;

/* Skip over SIZE bytes of data in blocks in the archive.  */
void shfs_arch_skip_file(shfs_arch_t *arch, off_t size)
{
  union block *x;

  /* FIXME: Make sure mv_begin_read is always called before it */

  if (seekable_archive)
    {
      off_t nblk = shfs_arch_seek_archive(arch, size);
      if (nblk >= 0)
	size -= nblk * BLOCKSIZE;
      else
	seekable_archive = false;
    }

  mv_size_left (size);

  while (size > 0)
    {
      x = shfs_arch_buffer_next(arch);
      if (! x)
return;

      shfs_arch_set_next_block_after(arch, x);
      size -= BLOCKSIZE;
      mv_size_left (size);
    }
}


