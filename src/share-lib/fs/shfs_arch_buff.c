

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

#include "shfs_arch.h"

/* Number of retries before giving up on read.  */
#define READ_ERROR_MAX 10

/* Variables.  */

tarlong prev_written;    /* bytes written on previous volumes */
tarlong bytes_written;   /* bytes written on this volume */
void *arch_record_buffer[2];  /* allocated memory */

/* FIXME: The following variables should ideally be static to this
   module.  However, this cannot be done yet.  The cleanup continues!  */

union block *record_start;      /* start of record of archive */
union block *record_end;        /* last+1 block of archive record */
union block *current_block;     /* current block of archive */
off_t records_skipped;   /* number of records skipped at the start
                                   of the archive, defined in delete.c */

static off_t record_start_block; /* block ordinal at record_start */

/* Where we write list messages (not errors, not interactions) to.  */
FILE *stdlis;

/* Have we hit EOF yet?  */
static bool hit_eof;

static bool read_full_records = false;

bool write_archive_to_stdout;

//void (*flush_write_ptr) (shfs_arch_t *, size_t);
//void (*flush_read_ptr) (shfs_arch_t *arch);


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
int inhibit_map;

void
mv_begin_write (const char *file_name, off_t totsize, off_t sizeleft)
{
  if (multi_volume_option)
    {
      struct bufmap *bp = malloc (sizeof bp[0]);
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

static double duration;

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


/* Open an archive named archive_name_array[0]. Detect if it is
   a compressed archive of known type and use corresponding decompression
   program if so */
static void open_compressed_archive(shfs_arch_t *arch)
{

  archive_compression_type = ct_tar;
  hit_eof = false; /* It might have been set by arch_buffer_next in
                      check_compressed_archive */
  /* Open compressed archive */
  read_full_records = true;
  arch->records_read = 0;
  record_end = record_start; /* set up for 1st record = # 0 */

}

/* Compute and return the block ordinal at current_block.  */
off_t
current_block_ordinal (void)
{
  return record_start_block + (current_block - record_start);
}

/* Indicate that we have used all blocks up thru BLOCK. */
int shfs_arch_set_next_block_after(shfs_arch_t *arch, union block *block)
{

  while (block >= current_block) {
    current_block++;
  }

  if (current_block > record_end)
    return (SHERR_INVAL);

  return (0);
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
static void _open_archive(shfs_arch_t *arch, int wanted_access)
{

  if (arch->record_size == 0)
return;

  if (archive_names == 0)
return;

  tar_stat_destroy (&current_stat_info);

  arch->arch_record_index = 0;
  shfs_arch_init_buffer(arch);

  /* When updating the archive, we start with reading.  */
  arch->access_mode = wanted_access == ACCESS_UPDATE ? ACCESS_READ : wanted_access;
  check_tty (arch->access_mode);

  read_full_records = read_full_records_option;

  arch->records_read = 0;

  switch (wanted_access)
  {
    case ACCESS_READ:
      open_compressed_archive(arch);
      break;

    case ACCESS_WRITE:
      break;

    case ACCESS_UPDATE:
      break;
  }

  switch (wanted_access)
  {
    case ACCESS_READ:
      shfs_arch_buffer_next(arch);       /* read it in, check for EOF */
      break;

    case ACCESS_UPDATE:
    case ACCESS_WRITE:
      arch->records_written = 0;
      break;
  }
}

static void short_read(shfs_arch_t *arch, size_t status)
{
  size_t left;                  /* bytes left */
  char *more;                   /* pointer to next byte to read */

  more = record_start->buffer + status;
  left = arch->record_size - status;

  while (left % BLOCKSIZE != 0
         || (left && status && read_full_records))
    {
      if (status)
        while ((status = shfs_arch_buffer_read(arch->archive, more, left)) == SAFE_READ_ERROR);

      if (status == 0)
        break;

      if (! read_full_records)
        {
          return;
        }

      left -= status;
      more += status;
    }

  record_end = record_start + (arch->record_size - left) / BLOCKSIZE;
  arch->records_read++;
}

static int arch_buffer_seek(shbuf_t *buff, size_t offset, int whence)
{

  switch (whence) {
    case SEEK_SET:
      shbuf_pos_set(buff, offset);
      break;
    case SEEK_CUR:
      shbuf_pos_incr(buff, offset);
      break;
    case SEEK_END:
      shbuf_pos_set(buff, shbuf_size(buff) + offset);
      break;
  }

  return (shbuf_pos(buff));
}

/* Backspace the archive descriptor by one record worth.  If it's a
   tape, MTIOCTOP will work.  If it's something else, try to seek on
   it.  If we can't seek, we lose!  */
static void backspace_output(shfs_arch_t *arch)
{
  off_t position = arch_buffer_seek(arch->archive, 0, SEEK_CUR);

  /* Seek back to the beginning of this record and start writing there.  */

  position -= arch->record_size;
  if (position < 0)
    position = 0;
  arch_buffer_seek(arch->archive, position, SEEK_SET);

}

/*  Flush the current buffer to/from the archive.  */
static int shfs_flush_archive(shfs_arch_t *arch)
{
  size_t buffer_level = current_block->buffer - record_start->buffer;
  record_start_block += record_end - record_start;
  current_block = record_start;
  record_end = record_start + BLOCKING_FACTOR;

  if (arch->access_mode == ACCESS_READ && arch->time_to_start_writing)
    {
      arch->access_mode = ACCESS_WRITE;
      arch->time_to_start_writing = false;
      backspace_output(arch);
    }

  switch (arch->access_mode)
    {
    case ACCESS_READ:
      shfs_arch_read_flush(arch);//flush_read ();
      break;

    case ACCESS_WRITE:
      shfs_arch_write_flush(arch, buffer_level);
      break;

    case ACCESS_UPDATE:
      return (SHERR_INVAL);
    }
return (0);
}

off_t shfs_arch_seek_archive(shfs_arch_t *arch, off_t size)
{
  off_t start = current_block_ordinal ();
  off_t offset;
  off_t nrec, nblk;
  off_t skipped = (BLOCKING_FACTOR - (current_block - record_start))
                  * BLOCKSIZE;

  if (size <= skipped)
    return 0;

  /* Compute number of records to skip */
  nrec = (size - skipped) / arch->record_size;
  if (nrec == 0)
    return 0;

  offset = arch_buffer_seek(arch->archive, (nrec * arch->record_size), SEEK_CUR);
  if (offset < 0)
    return offset;

  if (offset % arch->record_size)
    return (0);

  /* Convert to number of records */
  offset /= BLOCKSIZE;
  /* Compute number of skipped blocks */
  nblk = offset - start;

  /* Update buffering info */
  arch->records_read += nblk / BLOCKING_FACTOR;
  record_start_block = offset - BLOCKING_FACTOR;
  current_block = record_end;

  return nblk;
}

static double _compute_duration(shfs_arch_t *arch)
{
  struct timespec now;
  gettime (&now);
  duration += ((now.tv_sec - arch->last_stat_time.tv_sec)
               + (now.tv_nsec - arch->last_stat_time.tv_nsec) / 1e9);
  gettime (&arch->last_stat_time);
  return duration;
}

/** Close the archive file.  */
void shfs_arch_close_archive(shfs_arch_t *arch)
{
  if (arch->time_to_start_writing || arch->access_mode == ACCESS_WRITE)
    {
      shfs_flush_archive(arch);
      if (current_block > record_start)
        shfs_flush_archive(arch);
    }

  _compute_duration(arch);

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
	  char *s = malloc ((len = p - label) + 1);
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
static void match_volume_label(shfs_arch_t *arch)
{
  if (!volume_label)
  {
    union block *label = shfs_arch_buffer_next(arch);

    if (!label)
      return;
    if (label->header.typeflag == GNUTYPE_VOLHDR)
    {
      if (memchr (label->header.name, '\0', sizeof label->header.name))
        assign_string (&volume_label, label->header.name);
      else
      {
        volume_label = malloc (sizeof (label->header.name) + 1);
        memcpy (volume_label, label->header.name,
            sizeof (label->header.name));
        volume_label[sizeof (label->header.name)] = 0;
      }
    }
    else if (label->header.typeflag == XGLTYPE)
    {
      /* .. */
    }
  }

  if (!volume_label)
    return;

  if (!check_label_pattern (volume_label))
    return;
}

/* Mark the archive with volume label STR. */
static void _write_volume_label(shfs_arch_t *arch, const char *str)
{
  union block *label = shfs_arch_buffer_next(arch);

  memset (label, 0, BLOCKSIZE);

  strcpy (label->header.name, str);
  assign_string (&current_stat_info.file_name,
      label->header.name);
  current_stat_info.had_trailing_slash =
    strip_trailing_slashes (current_stat_info.file_name);

  label->header.typeflag = GNUTYPE_VOLHDR;
  TIME_TO_CHARS(arch, arch->start_time.tv_sec, label->header.mtime);
  shfs_arch_finish_header(arch, &current_stat_info, label, -1);
  shfs_arch_set_next_block_after(arch, label);
}

#define VOL_SUFFIX "Volume"

/* Add a volume label to a part of multi-volume archive */
static void add_volume_label(shfs_arch_t *arch)
{
  char buf[UINTMAX_STRSIZE_BOUND];
  char *p = STRINGIFY_BIGINT (volno, buf);
  char *s = malloc (strlen (volume_label_option) + sizeof VOL_SUFFIX
                     + strlen (p) + 2);
  sprintf (s, "%s %s %s", volume_label_option, VOL_SUFFIX, p);
  _write_volume_label(arch, s);
  free (s);
}

/* Add a volume label to the current archive */
static void write_volume_label(shfs_arch_t *arch)
{
  if (multi_volume_option)
    add_volume_label(arch);
  else
    _write_volume_label(arch, volume_label_option);
}

/* Simple flush read (no multi-volume or label extensions) */
static void simple_flush_read(shfs_arch_t *arch)
{
  size_t status;                /* result from system call */

  /* Clear the count of errors.  This only applies to a single call to
     flush_read.  */

  for (;;)
    {
      status = shfs_arch_buffer_read(arch->archive, record_start->buffer, arch->record_size);
      if (status == arch->record_size)
        {
          arch->records_read++;
          return;
        }
      if (status == SAFE_READ_ERROR)
        {
          continue;             /* try again */
        }
      break;
    }
  short_read(arch, status);
}


/* GNU flush functions. These support multi-volume and archive labels in
   GNU and PAX archive formats. */

static void _gnu_flush_read(shfs_arch_t *arch)
{
  size_t status;                /* result from system call */

  /* Clear the count of errors.  This only applies to a single call to
     flush_read.  */

  for (;;)
    {
      status = shfs_arch_buffer_read(arch->archive, record_start->buffer, arch->record_size);
      if (status == arch->record_size)
        {
          arch->records_read++;
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
  short_read(arch, status);
}

void shfs_arch_read_flush(shfs_arch_t *arch)
{
  size_t status;                /* result from system call */

  /* Clear the count of errors.  This only applies to a single call to flush_read.  */

  while (1) {
    status = shfs_arch_buffer_read(arch->archive, record_start->buffer, arch->record_size);
    if (status == arch->record_size)
    {
      arch->records_read++;
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

  short_read(arch, status);
}

static int shfs_arch_buffer_write(shbuf_t *buff, void *data, size_t data_len)
{
  size_t len;

  len = 0;
  shbuf_cat(buff, data + len, data_len - len);
  shbuf_pos_set(buff, shbuf_size(buff));

  return (data_len);
}

int shfs_arch_write_flush(shfs_arch_t *arch, size_t buffer_level)
{
  ssize_t status;
  union block *header;
  char *copy_ptr;
  size_t copy_size;
  size_t bufsize;
  struct bufmap *map;

  status = shfs_arch_buffer_write(arch->archive, record_start->buffer, arch->record_size);
  if (status != arch->record_size && !multi_volume_option)
{}
  else
    {
      if (status)
        arch->records_written++;
      bytes_written += status;
    }

  if (status == arch->record_size)
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
  arch->arch_record_index = !arch->arch_record_index;
  shfs_arch_init_buffer(arch);

  inhibit_map = 1;

  tar_stat_destroy (&dummy);

  header = shfs_arch_buffer_next(arch);
  bufmap_reset (map, header - record_start);
  bufsize = available_space_after (header);
  inhibit_map = 0;
  while (bufsize < copy_size)
    {
      memcpy (header->buffer, copy_ptr, bufsize);
      copy_ptr += bufsize;
      copy_size -= bufsize;
      shfs_arch_set_next_block_after(arch, header + (bufsize - 1) / BLOCKSIZE);
      header = shfs_arch_buffer_next(arch);
      bufsize = available_space_after (header);
    }
  memcpy (header->buffer, copy_ptr, copy_size);
  memset (header->buffer + copy_size, 0, bufsize - copy_size);
  shfs_arch_set_next_block_after(arch, header + (copy_size - 1) / BLOCKSIZE);
  shfs_arch_buffer_next(arch);

  return (0);
}

void shfs_arch_open_archive(shfs_arch_t *arch, int mode)
{

  //flush_read_ptr = shfs_arch_read_flush; //gnu_flush_read;
  //flush_write_ptr = shfs_arch_write_flush; //gnu_flush_write;

  _open_archive(arch, mode);
  switch (mode)
    {
    case ACCESS_READ:
    case ACCESS_UPDATE:
      if (volume_label_option)
        match_volume_label(arch);
 shbuf_pos_set(arch->archive, 0);

      break;

    case ACCESS_WRITE:
      arch->records_written = 0;
      if (volume_label_option)
        write_volume_label(arch);
      break;
    }
}

union block *shfs_arch_buffer_next(shfs_arch_t *arch)
{

  if (current_block == record_end)
  {
    if (hit_eof)
      return 0;
    shfs_flush_archive(arch);
    if (current_block == record_end)
    {
      hit_eof = true;
      return 0;
    }
  }

  return current_block;
}

