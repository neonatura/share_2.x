/* Buffer management for tar.

   Copyright 1988, 1992-1994, 1996-1997, 1999-2010, 2013-2014 Free
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

   Written by John Gilmore, on 1985-08-25.  */

#include <system.h>
#include <system-ioctl.h>

#include <signal.h>

#include <closeout.h>
#include <fnmatch.h>
#include <human.h>
#include <quotearg.h>

#include "common.h"

/* Number of retries before giving up on read.  */
#define READ_ERROR_MAX 10

/* Variables.  */

static tarlong prev_written;    /* bytes written on previous volumes */
static tarlong bytes_written;   /* bytes written on this volume */
void *arch_record_buffer[2];  /* allocated memory */
int arch_record_index;

/* FIXME: The following variables should ideally be static to this
   module.  However, this cannot be done yet.  The cleanup continues!  */

union block *record_start;      /* start of record of archive */
union block *record_end;        /* last+1 block of archive record */
union block *current_block;     /* current block of archive */
enum access_mode access_mode;   /* how do we handle the archive */
off_t records_read;             /* number of records read from this archive */
off_t records_written;          /* likewise, for records written */
off_t records_skipped;   /* number of records skipped at the start
                                   of the archive, defined in delete.c */

static off_t record_start_block; /* block ordinal at record_start */

/* Where we write list messages (not errors, not interactions) to.  */
FILE *stdlis;

static void backspace_output (void);

/* Have we hit EOF yet?  */
static bool hit_eof;

static bool read_full_records = false;

/* We're reading, but we just read the last block and it's time to update.
   Declared in update.c

   FIXME: Either eliminate it or move it to common.h.
*/
extern bool time_to_start_writing;

bool write_archive_to_stdout;

static void (*flush_write_ptr) (size_t);
static void (*flush_read_ptr) (void);


char *volume_label;
char *continued_file_name;
uintmax_t continued_file_size;
uintmax_t continued_file_offset;


static int volno = 1;           /* which volume of a multi-volume tape we're
                                   on */
bool write_archive_to_stdout;


/* Multi-volume tracking support */

/* When creating a multi-volume archive, each 'bufmap' represents
   a member stored (perhaps partly) in the current record buffer.
   After flushing the record to the output media, all bufmaps that
   represent fully written members are removed from the list, then
   the sizeleft and start numbers in the remaining bufmaps are updated.

   When reading from a multi-volume archive, the list degrades to a
   single element, which keeps information about the member currently
   being read.
*/

struct bufmap
{
  struct bufmap *next;          /* Pointer to the next map entry */
  size_t start;                 /* Offset of the first data block */
  char *file_name;              /* Name of the stored file */
  off_t sizetotal;              /* Size of the stored file */
  off_t sizeleft;               /* Size left to read/write */
};
static struct bufmap *bufmap_head, *bufmap_tail;

/* This variable, when set, inhibits updating the bufmap chain after
   a write.  This is necessary when writing extended POSIX headers. */
static int inhibit_map;

void
mv_begin_write (const char *file_name, off_t totsize, off_t sizeleft)
{
  if (multi_volume_option)
    {
      struct bufmap *bp = xmalloc (sizeof bp[0]);
      if (bufmap_tail)
	bufmap_tail->next = bp;
      else
	bufmap_head = bp;
      bufmap_tail = bp;

      bp->next = NULL;
      bp->start = current_block - record_start;
      bp->file_name = xstrdup (file_name);
      bp->sizetotal = totsize;
      bp->sizeleft = sizeleft;
    }
}

static struct bufmap *
bufmap_locate (size_t off)
{
  struct bufmap *map;

  for (map = bufmap_head; map; map = map->next)
    {
      if (!map->next
	  || off < map->next->start * BLOCKSIZE)
	break;
    }
  return map;
}

static void
bufmap_free (struct bufmap *mark)
{
  struct bufmap *map;
  for (map = bufmap_head; map && map != mark; )
    {
      struct bufmap *next = map->next;
      free (map->file_name);
      free (map);
      map = next;
    }
  bufmap_head = map;
  if (!bufmap_head)
    bufmap_tail = bufmap_head;
}

static void
bufmap_reset (struct bufmap *map, ssize_t fixup)
{
  bufmap_free (map);
  if (map)
    {
      for (; map; map = map->next)
	map->start += fixup;
    }
}


static struct tar_stat_info dummy;

void
buffer_write_global_xheader (void)
{
#ifdef DEPENDENCY
  xheader_write_global (&dummy.xhdr);
#endif
}

void
mv_begin_read (struct tar_stat_info *st)
{
  mv_begin_write (st->orig_file_name, st->stat.st_size, st->stat.st_size);
}

void
mv_end (void)
{
  if (multi_volume_option)
    bufmap_free (NULL);
}

void
mv_size_left (off_t size)
{
  if (bufmap_head)
    bufmap_head->sizeleft = size;
}


/* Functions.  */



/* Time-related functions */

static double duration;

void
set_start_time (void)
{
  gettime (&start_time);
  last_stat_time = start_time;
}


double
compute_duration (void)
{
  struct timespec now;
  gettime (&now);
  duration += ((now.tv_sec - last_stat_time.tv_sec)
               + (now.tv_nsec - last_stat_time.tv_nsec) / 1e9);
  gettime (&last_stat_time);
  return duration;
}


/* Compression detection */

enum compress_type {
  ct_none,             /* Unknown compression type */
  ct_tar,              /* Plain tar file */
#if 0
  ct_compress,
  ct_gzip,
  ct_bzip2,
  ct_lzip,
  ct_lzma,
  ct_lzop,
  ct_xz
#endif
};

static enum compress_type archive_compression_type = ct_none;

struct zip_magic
{
  enum compress_type type;
  size_t length;
  char const *magic;
};

struct zip_program
{
  enum compress_type type;
  char const *program;
  char const *option;
};

static struct zip_magic const magic[] = {
  { ct_none,     0, 0 },
  { ct_tar,      0, 0 },
#if 0
  { ct_compress, 2, "\037\235" },
  { ct_gzip,     2, "\037\213" },
  { ct_bzip2,    3, "BZh" },
  { ct_lzip,     4, "LZIP" },
  { ct_lzma,     6, "\xFFLZMA" },
  { ct_lzop,     4, "\211LZO" },
  { ct_xz,       6, "\xFD" "7zXZ" },
#endif
};

#define NMAGIC (sizeof(magic)/sizeof(magic[0]))


const char *
first_decompress_program (int *pstate)
{
#ifdef DEPENDENCY
  struct zip_program const *zp;

  if (use_compress_program_option)
    return use_compress_program_option;

  if (archive_compression_type == ct_none)
    return NULL;

  *pstate = 0;
  zp = find_zip_program (archive_compression_type, pstate);
  return zp ? zp->program : NULL;
#endif
  return NULL;
}

const char *
next_decompress_program (int *pstate)
{
#ifdef DEPENDENCY
  struct zip_program const *zp;

  if (use_compress_program_option)
    return NULL;
  zp = find_zip_program (archive_compression_type, pstate);
  return zp ? zp->program : NULL;
#endif
  return NULL;
}

/* Open an archive named archive_name_array[0]. Detect if it is
   a compressed archive of known type and use corresponding decompression
   program if so */
static void open_compressed_archive (void)
{

  archive_compression_type = ct_tar;
  hit_eof = false; /* It might have been set by arch_buffer_next in
                      check_compressed_archive */
  /* Open compressed archive */
  read_full_records = true;
  records_read = 0;
  record_end = record_start; /* set up for 1st record = # 0 */

}

/* Compute and return the block ordinal at current_block.  */
off_t
current_block_ordinal (void)
{
  return record_start_block + (current_block - record_start);
}

/* If the EOF flag is set, reset it, as well as current_block, etc.  */
void
reset_eof (void)
{
  if (hit_eof)
    {
      hit_eof = false;
      current_block = record_start;
      record_end = record_start + blocking_factor;
      access_mode = ACCESS_WRITE;
    }
}

#ifdef DEPENDENCY
/* Return the location of the next available input or output block.
   Return zero for EOF.  Once we have returned zero, we just keep returning
   it, to avoid accidentally going on to the next file on the tape.  */
union block *
arch_buffer_next (void)
{
fprintf(stderr, "DEBUG: arch_buffer_next: hit_eof %d\n", hit_eof);
  if (current_block == record_end)
    {
      if (hit_eof)
        return 0;
      flush_archive ();
      if (current_block == record_end)
        {
fprintf(stderr, "DEBUG: arch_buffer_next: current_block %d, record_end %d\n", current_block, record_end);
          hit_eof = true;
          return 0;
        }
    }
  return current_block;
}
#endif

/* Indicate that we have used all blocks up thru BLOCK. */
void
set_next_block_after (union block *block)
{

  while (block >= current_block) {
    shbuf_cat(archive, current_block->buffer, BLOCKSIZE);
    current_block++;
  }

  /* Do *not* flush the archive here.  If we do, the same argument to
     set_next_block_after could mean the next block (if the input record
     is exactly one block long), which is not what is intended.  */
#if 0
  if (current_block > record_end)
    abort ();
#endif
}

/* Return the number of bytes comprising the space between POINTER
   through the end of the current buffer of blocks.  This space is
   available for filling with data, or taking data from.  POINTER is
   usually (but not always) the result of previous arch_buffer_next call.  */
size_t
available_space_after (union block *pointer)
{
  return record_end->buffer - pointer->buffer;
}


static void
check_tty (enum access_mode mode)
{
}

/* Open an archive file.  The argument specifies whether we are
   reading or writing, or both.  */
static void
_open_archive (enum access_mode wanted_access)
{

  if (record_size == 0)
return;

  if (archive_names == 0)
return;

  tar_stat_destroy (&current_stat_info);

  arch_record_index = 0;
  arch_init_buffer ();

  /* When updating the archive, we start with reading.  */
  access_mode = wanted_access == ACCESS_UPDATE ? ACCESS_READ : wanted_access;
  check_tty (access_mode);

  read_full_records = read_full_records_option;

  records_read = 0;

  switch (wanted_access)
  {
    case ACCESS_READ:
      open_compressed_archive ();
      break;

    case ACCESS_WRITE:
      break;

    case ACCESS_UPDATE:
      break;
  }

  switch (wanted_access)
  {
    case ACCESS_READ:
      arch_buffer_next ();       /* read it in, check for EOF */
      break;

    case ACCESS_UPDATE:
    case ACCESS_WRITE:
      records_written = 0;
      break;
  }
}

/* Perform a write to flush the buffer.  */
static ssize_t
_flush_write (void)
{
  ssize_t status;

  status = sys_write_archive_buffer ();
  return status;
}

static void
short_read (size_t status)
{
  size_t left;                  /* bytes left */
  char *more;                   /* pointer to next byte to read */

  more = record_start->buffer + status;
  left = record_size - status;

  while (left % BLOCKSIZE != 0
         || (left && status && read_full_records))
    {
      if (status)
        while ((status = arch_buffer_read(archive, more, left)) == SAFE_READ_ERROR);

      if (status == 0)
        break;

      if (! read_full_records)
        {
          return;
        }

      left -= status;
      more += status;
    }

  record_end = record_start + (record_size - left) / BLOCKSIZE;
  records_read++;
}

/*  Flush the current buffer to/from the archive.  */
void
flush_archive (void)
{
  size_t buffer_level = current_block->buffer - record_start->buffer;
  record_start_block += record_end - record_start;
  current_block = record_start;
  record_end = record_start + blocking_factor;

  if (access_mode == ACCESS_READ && time_to_start_writing)
    {
      access_mode = ACCESS_WRITE;
      time_to_start_writing = false;
      backspace_output ();
    }

  switch (access_mode)
    {
    case ACCESS_READ:
      flush_read ();
      break;

    case ACCESS_WRITE:
      flush_write_ptr (buffer_level);
      break;

    case ACCESS_UPDATE:
      abort ();
    }
}

/* Backspace the archive descriptor by one record worth.  If it's a
   tape, MTIOCTOP will work.  If it's something else, try to seek on
   it.  If we can't seek, we lose!  */
static void
backspace_output (void)
{
  off_t position = arch_buffer_seek(archive, 0, SEEK_CUR);

  /* Seek back to the beginning of this record and start writing there.  */

  position -= record_size;
  if (position < 0)
    position = 0;
  arch_buffer_seek(archive, position, SEEK_SET);

}

off_t
seek_archive (off_t size)
{
  off_t start = current_block_ordinal ();
  off_t offset;
  off_t nrec, nblk;
  off_t skipped = (blocking_factor - (current_block - record_start))
                  * BLOCKSIZE;

  if (size <= skipped)
    return 0;

  /* Compute number of records to skip */
  nrec = (size - skipped) / record_size;
  if (nrec == 0)
    return 0;

  offset = arch_buffer_seek(archive, (nrec * record_size), SEEK_CUR);
  if (offset < 0)
    return offset;

  if (offset % record_size)
    return (0);

  /* Convert to number of records */
  offset /= BLOCKSIZE;
  /* Compute number of skipped blocks */
  nblk = offset - start;

  /* Update buffering info */
  records_read += nblk / blocking_factor;
  record_start_block = offset - blocking_factor;
  current_block = record_end;

  return nblk;
}

/* Close the archive file.  */
void
close_archive (void)
{
  if (time_to_start_writing || access_mode == ACCESS_WRITE)
    {
      flush_archive ();
      if (current_block > record_start)
        flush_archive ();
    }

  compute_duration ();

  tar_stat_destroy (&current_stat_info);
#if 0
  free (record_buffer[0]);
  free (record_buffer[1]);
#endif
  bufmap_free (NULL);
}


#define VOLUME_TEXT " Volume "
#define VOLUME_TEXT_LEN (sizeof VOLUME_TEXT - 1)

char *
drop_volume_label_suffix (const char *label)
{
  const char *p;
  size_t len = strlen (label);

  if (len < 1)
    return NULL;

  for (p = label + len - 1; p > label && isdigit ((unsigned char) *p); p--)
    ;
  if (p > label && p - (VOLUME_TEXT_LEN - 1) > label)
    {
      p -= VOLUME_TEXT_LEN - 1;
      if (memcmp (p, VOLUME_TEXT, VOLUME_TEXT_LEN) == 0)
	{
	  char *s = xmalloc ((len = p - label) + 1);
	  memcpy (s, label, len);
	  s[len] = 0;
	  return s;
	}
    }

  return NULL;
}

/* Check LABEL against the volume label, seen as a globbing
   pattern.  Return true if the pattern matches.  In case of failure,
   retry matching a volume sequence number before giving up in
   multi-volume mode.  */
static bool
check_label_pattern (const char *label)
{
  char *string;
  bool result = false;

  if (fnmatch (volume_label_option, label, 0) == 0)
    return true;

  if (!multi_volume_option)
    return false;

  string = drop_volume_label_suffix (label);
  if (string)
    {
      result = fnmatch (string, volume_label_option, 0) == 0;
      free (string);
    }
  return result;
}

/* Check if the next block contains a volume label and if this matches
   the one given in the command line */
static void
match_volume_label (void)
{
  if (!volume_label)
    {
      union block *label = arch_buffer_next ();

      if (!label)
return;
      if (label->header.typeflag == GNUTYPE_VOLHDR)
	{
	  if (memchr (label->header.name, '\0', sizeof label->header.name))
	    assign_string (&volume_label, label->header.name);
	  else
	    {
	      volume_label = xmalloc (sizeof (label->header.name) + 1);
	      memcpy (volume_label, label->header.name,
		      sizeof (label->header.name));
	      volume_label[sizeof (label->header.name)] = 0;
	    }
	}
      else if (label->header.typeflag == XGLTYPE)
	{
#ifdef DEPENDENCY
	  struct tar_stat_info st;
	  tar_stat_init (&st);
	  xheader_read (&st.xhdr, label,
			OFF_FROM_HEADER (label->header.size));
	  xheader_decode (&st);
	  tar_stat_destroy (&st);
#endif
	}
    }

  if (!volume_label)
return;

  if (!check_label_pattern (volume_label))
return;
}

/* Mark the archive with volume label STR. */
static void
_write_volume_label (const char *str)
{
#ifdef DEPENDENCY
  if (archive_format == POSIX_FORMAT)
    xheader_store ("GNU.volume.label", &dummy, str);
  else
#endif
    {
      union block *label = arch_buffer_next ();

      memset (label, 0, BLOCKSIZE);

      strcpy (label->header.name, str);
      assign_string (&current_stat_info.file_name,
                     label->header.name);
      current_stat_info.had_trailing_slash =
        strip_trailing_slashes (current_stat_info.file_name);

      label->header.typeflag = GNUTYPE_VOLHDR;
      TIME_TO_CHARS (start_time.tv_sec, label->header.mtime);
      finish_header (&current_stat_info, label, -1);
      set_next_block_after (label);
    }
}

#define VOL_SUFFIX "Volume"

/* Add a volume label to a part of multi-volume archive */
static void
add_volume_label (void)
{
  char buf[UINTMAX_STRSIZE_BOUND];
  char *p = STRINGIFY_BIGINT (volno, buf);
  char *s = xmalloc (strlen (volume_label_option) + sizeof VOL_SUFFIX
                     + strlen (p) + 2);
  sprintf (s, "%s %s %s", volume_label_option, VOL_SUFFIX, p);
  _write_volume_label (s);
  free (s);
}

static void
add_chunk_header (struct bufmap *map)
{
#ifdef DEPENDENCY
  if (archive_format == POSIX_FORMAT)
    {
      off_t block_ordinal;
      union block *blk;
      struct tar_stat_info st;

      memset (&st, 0, sizeof st);
      st.orig_file_name = st.file_name = map->file_name;
      st.stat.st_mode = S_IFREG|S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;
      st.stat.st_uid = getuid ();
      st.stat.st_gid = getgid ();
      st.orig_file_name = xheader_format_name (&st,
                                               "%d/GNUFileParts.%p/%f.%n",
                                               volno);
      st.file_name = st.orig_file_name;
      st.archive_file_size = st.stat.st_size = map->sizeleft;

      block_ordinal = current_block_ordinal ();
      blk = start_header (&st);
      if (!blk)
        abort (); /* FIXME */
      finish_header (&st, blk, block_ordinal);
      free (st.orig_file_name);
    }
#endif
}


/* Add a volume label to the current archive */
static void
write_volume_label (void)
{
  if (multi_volume_option)
    add_volume_label ();
  else
    _write_volume_label (volume_label_option);
}

/* Simple flush read (no multi-volume or label extensions) */
static void
simple_flush_read (void)
{
  size_t status;                /* result from system call */

  /* Clear the count of errors.  This only applies to a single call to
     flush_read.  */

fprintf(stderr, "DEBUG: simple_flush_read()\n");

  for (;;)
    {
      status = arch_buffer_read(archive, record_start->buffer, record_size);
      if (status == record_size)
        {
          records_read++;
          return;
        }
      if (status == SAFE_READ_ERROR)
        {
          continue;             /* try again */
        }
      break;
    }
  short_read (status);
}

/* Simple flush write (no multi-volume or label extensions) */
static void
simple_flush_write (size_t level __attribute__((unused)))
{
  ssize_t status;

  status = _flush_write ();
  if (status != record_size)
{}
  else
    {
      records_written++;
      bytes_written += status;
    }
}


/* GNU flush functions. These support multi-volume and archive labels in
   GNU and PAX archive formats. */

static void
_gnu_flush_read (void)
{
  size_t status;                /* result from system call */

  /* Clear the count of errors.  This only applies to a single call to
     flush_read.  */

fprintf(stderr, "DEBUG: _glu_flush_read()\n");

  for (;;)
    {
      status = arch_buffer_read(archive, record_start->buffer, record_size);
fprintf(stderr, "DEBUG: gnu_flush_read: %d = rmtread( record_size:%d )\n", status, record_size);
      if (status == record_size)
        {
          records_read++;
          return;
        }

      /* The condition below used to include
              || (status > 0 && !read_full_records)
         This is incorrect since even if new_volume() succeeds, the
         subsequent call to rmtread will overwrite the chunk of data
         already read in the buffer, so the processing will fail */
       if (status == SAFE_READ_ERROR)
        {
          continue;
        }
      break;
    }
  short_read (status);
}

static void
gnu_flush_read (void)
{
  flush_read_ptr = simple_flush_read; /* Avoid recursion */
  _gnu_flush_read ();
  flush_read_ptr = gnu_flush_read;
}

static void
_gnu_flush_write (size_t buffer_level)
{
  ssize_t status;
  union block *header;
  char *copy_ptr;
  size_t copy_size;
  size_t bufsize;
  struct bufmap *map;

  status = _flush_write ();
fprintf(stderr, "DEBUG: _gnu_flush_write: status %d\n", status);
  if (status != record_size && !multi_volume_option)
{}
  else
    {
      if (status)
        records_written++;
      bytes_written += status;
    }

  if (status == record_size)
    {
      return;
    }

  map = bufmap_locate (status);



  tar_stat_destroy (&dummy);

  //increase_volume_number ();
  prev_written += bytes_written;
  bytes_written = 0;

  copy_ptr = record_start->buffer + status;
  copy_size = buffer_level - status;

  /* Switch to the next buffer */
  arch_record_index = !arch_record_index;
  arch_init_buffer ();

  inhibit_map = 1;

#ifdef DEPEDENCY
  if (volume_label_option)
    add_volume_label ();

  if (map)
    add_multi_volume_header (map);
#endif

  write_extended (true, &dummy, arch_buffer_next ());
  tar_stat_destroy (&dummy);

  if (map)
    add_chunk_header (map);
  header = arch_buffer_next ();
  bufmap_reset (map, header - record_start);
  bufsize = available_space_after (header);
  inhibit_map = 0;
  while (bufsize < copy_size)
    {
      memcpy (header->buffer, copy_ptr, bufsize);
      copy_ptr += bufsize;
      copy_size -= bufsize;
      set_next_block_after (header + (bufsize - 1) / BLOCKSIZE);
      header = arch_buffer_next ();
      bufsize = available_space_after (header);
    }
  memcpy (header->buffer, copy_ptr, copy_size);
  memset (header->buffer + copy_size, 0, bufsize - copy_size);
  set_next_block_after (header + (copy_size - 1) / BLOCKSIZE);
  arch_buffer_next ();
}

static void
gnu_flush_write (size_t buffer_level)
{
  flush_write_ptr = simple_flush_write; /* Avoid recursion */
  _gnu_flush_write (buffer_level);
  flush_write_ptr = gnu_flush_write;
}

void
flush_read (void)
{
fprintf(stderr, "DEBUG: flush_read()\n");
  flush_read_ptr ();
}

void
flush_write (void)
{
  flush_write_ptr (record_size);
}

void
open_archive (enum access_mode wanted_access)
{
  flush_read_ptr = gnu_flush_read;
  flush_write_ptr = gnu_flush_write;

  _open_archive (wanted_access);
  switch (wanted_access)
    {
    case ACCESS_READ:
    case ACCESS_UPDATE:
      if (volume_label_option)
        match_volume_label ();
      break;

    case ACCESS_WRITE:
      records_written = 0;
      if (volume_label_option)
        write_volume_label ();
      break;
    }
}
