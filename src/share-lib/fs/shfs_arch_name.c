
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

/* Various processing of names.

   Copyright 1988, 1992, 1994, 1996-2001, 2003-2007, 2009, 2013-2014
   Free Software Foundation, Inc.

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

#include <fnmatch.h>
#include <hash.h>
#include <quotearg.h>
#include <wordsplit.h>
#include <argp.h>
#include <pwd.h>
#include <grp.h>




/* User and group names.  */

/* Make sure you link with the proper libraries if you are running the
   Yellow Peril (thanks for the good laugh, Ian J.!), or, euh... NIS.
   This code should also be modified for non-UNIX systems to do something
   reasonable.  */

static char *cached_uname;
static char *cached_gname;

static uid_t cached_uid;	/* valid only if cached_uname is not empty */
static gid_t cached_gid;	/* valid only if cached_gname is not empty */

/* These variables are valid only if nonempty.  */
static char *cached_no_such_uname;
static char *cached_no_such_gname;

/* These variables are valid only if nonzero.  It's not worth optimizing
   the case for weird systems where 0 is not a valid uid or gid.  */
static uid_t cached_no_such_uid;
static gid_t cached_no_such_gid;

/* Given UID, find the corresponding UNAME.  */
void
uid_to_uname (uid_t uid, char **uname)
{
  struct passwd *passwd;

  if (uid != 0 && uid == cached_no_such_uid)
    {
      *uname = xstrdup ("");
      return;
    }

  if (!cached_uname || uid != cached_uid)
    {
      passwd = getpwuid (uid);
      if (passwd)
	{
	  cached_uid = uid;
	  assign_string (&cached_uname, passwd->pw_name);
	}
      else
	{
	  cached_no_such_uid = uid;
	  *uname = xstrdup ("");
	  return;
	}
    }
  *uname = xstrdup (cached_uname);
}

/* Given GID, find the corresponding GNAME.  */
void
gid_to_gname (gid_t gid, char **gname)
{
  struct group *group;

  if (gid != 0 && gid == cached_no_such_gid)
    {
      *gname = xstrdup ("");
      return;
    }

  if (!cached_gname || gid != cached_gid)
    {
      group = getgrgid (gid);
      if (group)
	{
	  cached_gid = gid;
	  assign_string (&cached_gname, group->gr_name);
	}
      else
	{
	  cached_no_such_gid = gid;
	  *gname = xstrdup ("");
	  return;
	}
    }
  *gname = xstrdup (cached_gname);
}

/* Given UNAME, set the corresponding UID and return 1, or else, return 0.  */
int
uname_to_uid (char const *uname, uid_t *uidp)
{
  struct passwd *passwd;

  if (cached_no_such_uname
      && strcmp (uname, cached_no_such_uname) == 0)
    return 0;

  if (!cached_uname
      || uname[0] != cached_uname[0]
      || strcmp (uname, cached_uname) != 0)
    {
      passwd = getpwnam (uname);
      if (passwd)
	{
	  cached_uid = passwd->pw_uid;
	  assign_string (&cached_uname, passwd->pw_name);
	}
      else
	{
	  assign_string (&cached_no_such_uname, uname);
	  return 0;
	}
    }
  *uidp = cached_uid;
  return 1;
}

/* Given GNAME, set the corresponding GID and return 1, or else, return 0.  */
int
gname_to_gid (char const *gname, gid_t *gidp)
{
  struct group *group;

  if (cached_no_such_gname
      && strcmp (gname, cached_no_such_gname) == 0)
    return 0;

  if (!cached_gname
      || gname[0] != cached_gname[0]
      || strcmp (gname, cached_gname) != 0)
    {
      group = getgrnam (gname);
      if (group)
	{
	  cached_gid = group->gr_gid;
	  assign_string (&cached_gname, gname);
	}
      else
	{
	  assign_string (&cached_no_such_gname, gname);
	  return 0;
	}
    }
  *gidp = cached_gid;
  return 1;
}


static struct name *
make_name (const char *file_name)
{
  struct name *p = xzalloc (sizeof (*p));
  if (!file_name)
    file_name = "";
  p->name = xstrdup (file_name);
  p->length = strlen (p->name);
  return p;
}

static void
free_name (struct name *p)
{
  if (p)
    {
      free (p->name);
      free (p->caname);
      free (p);
    }
}


/* Names from the command call.  */

static struct name *namelist;	/* first name in list, if any */
static struct name *nametail;	/* end of name list */

/* File name arguments are processed in two stages: first a
   name element list (see below) is filled, then the names from it
   are moved into the namelist.

   This awkward process is needed only to implement --same-order option,
   which is meant to help process large archives on machines with
   limited memory.  With this option on, namelist contains at most one
   entry, which diminishes the memory consumption.

   However, I very much doubt if we still need this -- Sergey */

/* A name_list element contains entries of three types: */

#define NELT_NAME  0   /* File name */
#define NELT_CHDIR 1   /* Change directory request */
#define NELT_FMASK 2   /* Change fnmatch options request */
#define NELT_FILE  3   /* Read file names from that file */
#define NELT_NOOP  4   /* No operation */
#define NELT_INODE 5   /* Read directly from share-fs inode */

struct name_elt        /* A name_array element. */
{
  struct name_elt *next, *prev;
  char type;           /* Element type, see NELT_* constants above */
  union
  {
    const char *name;  /* File or directory name */
    int matching_flags;/* fnmatch options if type == NELT_FMASK */
    struct             /* File, if type == NELT_FILE */
    {
      const char *name;/* File name */
      int term;        /* File name terminator in the list */
      FILE *fp;
      SHFL *ino;
    } file;
  } v;
};

static struct name_elt *name_head;  /* store a list of names */
size_t name_count;	 	    /* how many of the entries are names? */

static struct name_elt *
name_elt_alloc (void)
{
  struct name_elt *elt;

  elt = malloc (sizeof (*elt));
  if (!name_head)
    {
      name_head = elt;
      name_head->prev = name_head->next = NULL;
      name_head->type = NELT_NOOP;
      elt = malloc (sizeof (*elt));
    }

  elt->prev = name_head->prev;
  if (name_head->prev)
    name_head->prev->next = elt;
  elt->next = name_head;
  name_head->prev = elt;
  return elt;
}

static void
name_list_adjust (void)
{
  if (name_head)
    while (name_head->prev)
      name_head = name_head->prev;
}

static void
name_list_advance (void)
{
  struct name_elt *elt = name_head;
  name_head = elt->next;
  if (name_head)
    name_head->prev = NULL;
  free (elt);
}

/* Add to name_array the file NAME with fnmatch options MATCHING_FLAGS */
void
name_add_name (const char *name, int matching_flags)
{
  static int prev_flags = 0; /* FIXME: Or EXCLUDE_ANCHORED? */
  struct name_elt *ep = name_elt_alloc ();

  if (prev_flags != matching_flags)
    {
      ep->type = NELT_FMASK;
      ep->v.matching_flags = matching_flags;
      prev_flags = matching_flags;
      ep = name_elt_alloc ();
    }
  ep->type = NELT_NAME;
  ep->v.name = name;
  name_count++;
}

/* Add to name_array a chdir request for the directory NAME */
void name_add_dir(const char *name)
{
  struct name_elt *ep = name_elt_alloc ();
  ep->type = NELT_CHDIR;
  ep->v.name = name;
}

void
name_add_file (const char *name, int term)
{
  struct name_elt *ep = name_elt_alloc ();
  ep->type = NELT_FILE;
  ep->v.file.name = name;
  ep->v.file.term = term;
  ep->v.file.fp = NULL;
}

/** Add a share-fs inode to the name list */
void name_add_inode(const char *name, SHFL *inode)
{
  struct name_elt *ep = name_elt_alloc ();

  ep->type = NELT_INODE;
  ep->v.file.name = name;
  ep->v.file.ino = inode;
}



/* Names from external name file.  */

static char *name_buffer;	/* buffer to hold the current file name */
static size_t name_buffer_length; /* allocated length of name_buffer */

/* Set up to gather file names for tar.  They can either come from a
   file or were saved from decoding arguments.  */
void
name_init (void)
{
  name_buffer = malloc (NAME_FIELD_SIZE + 2);
  name_buffer_length = NAME_FIELD_SIZE;
  name_list_adjust ();
}

void
name_term (void)
{
  free (name_buffer);
}

/* Prevent recursive inclusion of the same file */
struct file_id_list
{
  struct file_id_list *next;
  ino_t ino;
  dev_t dev;
  const char *from_file;
};

static struct file_id_list *file_id_list;

/* Return the name of the file from which the file names and options
   are being read.
*/
static const char *
file_list_name (void)
{
  struct name_elt *elt;

  for (elt = name_head; elt; elt = elt->next)
    if (elt->type == NELT_FILE && elt->v.file.fp)
      return elt->v.file.name;
  return "command line";
}

static int
add_file_id (const char *filename)
{
  struct file_id_list *p;
  struct stat st;
  const char *reading_from;

  reading_from = file_list_name ();
  for (p = file_id_list; p; p = p->next)
    if (p->ino == st.st_ino && p->dev == st.st_dev)
      {
	int oldc = set_char_quoting (NULL, ':', 1);
	set_char_quoting (NULL, ':', oldc);
	return 1;
      }
  p = malloc (sizeof *p);
  p->next = file_id_list;
  p->ino = st.st_ino;
  p->dev = st.st_dev;
  p->from_file = reading_from;
  file_id_list = p;
  return 0;
}

enum read_file_list_state  /* Result of reading file name from the list file */
  {
    file_list_success,     /* OK, name read successfully */
    file_list_end,         /* End of list file */
    file_list_zero,        /* Zero separator encountered where it should not */
    file_list_skip         /* Empty (zero-length) entry encountered, skip it */
  };

/* Read from FP a sequence of characters up to TERM and put them
   into STK.
 */
static enum read_file_list_state
read_name_from_file (struct name_elt *ent)
{
  int c;
  size_t counter = 0;
  FILE *fp = ent->v.file.fp;
  int term = ent->v.file.term;

  for (c = getc (fp); c != EOF && c != term; c = getc (fp))
    {
      if (counter == name_buffer_length)
	name_buffer = x2realloc (name_buffer, &name_buffer_length);
      name_buffer[counter++] = c;
      if (c == 0)
	{
	  /* We have read a zero separator. The file possibly is
	     zero-separated */
	  return file_list_zero;
	}
    }

  if (counter == 0 && c != EOF)
    return file_list_skip;

  if (counter == name_buffer_length)
    name_buffer = x2realloc (name_buffer, &name_buffer_length);
  name_buffer[counter] = 0;

  return (counter == 0 && c == EOF) ? file_list_end : file_list_success;
}

static int
handle_option (const char *str)
{
  struct wordsplit ws;
  int i;

  while (*str && isspace (*str))
    ++str;
  if (*str != '-')
    return 1;

  ws.ws_offs = 1;
  if (wordsplit (str, &ws, WRDSF_DEFFLAGS|WRDSF_DOOFFS))
return -1;
  ws.ws_wordv[0] = program_invocation_short_name;
  //more_options (ws.ws_wordc+ws.ws_offs, ws.ws_wordv);
  for (i = 0; i < ws.ws_wordc+ws.ws_offs; i++)
    ws.ws_wordv[i] = NULL;

  wordsplit_free (&ws);
  return 0;
}

static int
read_next_name (struct name_elt *ent, struct name_elt *ret)
{
  if (!ent->v.file.fp)
    {
	{
	  if (add_file_id (ent->v.file.name))
	    {
	      name_list_advance ();
	      return 1;
	    }
	  if ((ent->v.file.fp = fopen (ent->v.file.name, "r")) == NULL) {
return 1;
}
	}
    }

  while (1)
    {
      switch (read_name_from_file (ent))
	{
	case file_list_skip:
	  continue;

	case file_list_zero:
	  ent->v.file.term = 0;
	  /* fall through */
	case file_list_success:
	  if (unquote_option)
	    unquote_string (name_buffer);
	  if (handle_option (name_buffer) == 0)
	    {
	      name_list_adjust ();
	      return 1;
	    }
	  ret->type = NELT_NAME;
	  ret->v.name = name_buffer;
	  return 0;

	case file_list_end:
	  if (strcmp (ent->v.file.name, "-"))
	    fclose (ent->v.file.fp);
	  ent->v.file.fp = NULL;
	  name_list_advance ();
	  return 1;
	}
    }
}

static void
copy_name (struct name_elt *ep)
{
  const char *source;
  size_t source_len;
  char *cursor;

  source = ep->v.name;
  source_len = strlen (source);
  if (name_buffer_length < source_len)
    {
      do
	{
	  name_buffer_length *= 2;
	  if (! name_buffer_length)
	    xalloc_die ();
	}
      while (name_buffer_length < source_len);

      free (name_buffer);
      name_buffer = malloc(name_buffer_length + 2);
    }
  strcpy (name_buffer, source);

  /* Zap trailing slashes.  */
  cursor = name_buffer + strlen (name_buffer) - 1;
  while (cursor > name_buffer && ISSLASH (*cursor))
    *cursor-- = '\0';
}


static int matching_flags; /* exclude_fnmatch options */

/* Get the next NELT_NAME element from name_array.  Result is in
   static storage and can't be relied upon across two calls.

   If CHANGE_DIRS is true, treat any entries of type NELT_CHDIR as
   the request to change to the given directory.

   Entries of type NELT_FMASK cause updates of the matching_flags
   value. */
static struct name_elt *
name_next_elt (int change_dirs)
{
  static struct name_elt entry;
  struct name_elt *ep;

  while ((ep = name_head) != NULL)
    {
      switch (ep->type)
	{
	case NELT_NOOP:
	  name_list_advance ();
	  break;

	case NELT_FMASK:
	  matching_flags = ep->v.matching_flags;
	  recursion_option = matching_flags & FNM_LEADING_DIR;
	  name_list_advance ();
	  continue;

	case NELT_FILE:
	  if (read_next_name (ep, &entry) == 0)
	    return &entry;
	  continue;


	case NELT_CHDIR:
	  if (change_dirs)
	    {
#if 0
	      chdir_do (chdir_arg (xstrdup (ep->v.name)));
#endif
	      name_list_advance ();
	      break;
	    }
	  /* fall through */
	case NELT_NAME:
	  copy_name (ep);
	  if (unquote_option)
	    unquote_string (name_buffer);
	  entry.type = ep->type;
	  entry.v.name = name_buffer;
	  name_list_advance ();
	  return &entry;

	case NELT_INODE:
	  copy_name (ep);
	  if (unquote_option)
	    unquote_string (name_buffer);
	  entry.type = ep->type;
	  entry.v.name = name_buffer;
	  name_list_advance ();
	  return &entry;

	}
    }

  return NULL;
}

const char *
name_next (int change_dirs)
{
  struct name_elt *nelt = name_next_elt (change_dirs);
  return nelt ? nelt->v.name : NULL;
}

/* Gather names in a list for scanning.  Could hash them later if we
   really care.

   If the names are already sorted to match the archive, we just read
   them one by one.  name_gather reads the first one, and it is called
   by name_match as appropriate to read the next ones.  At EOF, the
   last name read is just left in the buffer.  This option lets users
   of small machines extract an arbitrary number of files by doing
   "tar t" and editing down the list of files.  */

void name_gather(void)
{
  /* Buffer able to hold a single name.  */
  static struct name *buffer = NULL;

  struct name_elt *ep;

  if (same_order_option)
  {
    static int change_dir;

    while ((ep = name_next_elt (0)) && ep->type == NELT_CHDIR)
      change_dir = chdir_arg (xstrdup (ep->v.name));

    if (ep)
    {
      free_name (buffer);
      buffer = make_name (ep->v.name);
      buffer->change_dir = change_dir;
      buffer->next = 0;
      buffer->found_count = 0;
      buffer->matching_flags = matching_flags;
      buffer->directory = NULL;
      buffer->parent = NULL;
      buffer->cmdline = true;

      namelist = nametail = buffer;
    }
    else if (change_dir)
      addname (0, change_dir, false, NULL);
  }
  else
  {
    /* Non sorted names -- read them all in.  */
    int change_dir = 0;

    for (;;)
    {
      int change_dir0 = change_dir;
      while ((ep = name_next_elt (0)) && ep->type == NELT_CHDIR)
        change_dir = chdir_arg (xstrdup (ep->v.name));

      if (ep)
        addname (ep->v.name, change_dir, true, NULL);
      else
      {
        if (change_dir != change_dir0)
          addname (NULL, change_dir, false, NULL);
        break;
      }
    }
  }
}

/*  Add a name to the namelist.  */
struct name *
addname (char const *string, int change_dir, bool cmdline, struct name *parent)
{
  struct name *name = make_name (string);

  name->prev = nametail;
  name->next = NULL;
  name->found_count = 0;
  name->matching_flags = matching_flags;
  name->change_dir = change_dir;
  name->directory = NULL;
  name->parent = parent;
  name->cmdline = cmdline;

  if (nametail)
    nametail->next = name;
  else
    namelist = name;
  nametail = name;
  return name;
}

/* Find a match for FILE_NAME (whose string length is LENGTH) in the name
   list.  */
static struct name *
namelist_match (char const *file_name, size_t length)
{
  struct name *p;

  for (p = namelist; p; p = p->next)
    {
      if (p->name[0]
	  && exclude_fnmatch (p->name, file_name, p->matching_flags))
	return p;
    }

  return NULL;
}

void
remname (struct name *name)
{
  struct name *p;

  if ((p = name->prev) != NULL)
    p->next = name->next;
  else
    namelist = name->next;

  if ((p = name->next) != NULL)
    p->prev = name->prev;
  else
    nametail = name->prev;
}

/* Return true if and only if name FILE_NAME (from an archive) matches any
   name from the namelist.  */
bool name_match(const char *file_name)
{
  size_t length = strlen (file_name);

  while (1)
  {
    struct name *cursor = namelist;

    if (!cursor)
      return true;

    if (cursor->name[0] == 0)
    {
#if 0
      chdir_do (cursor->change_dir);
#endif
      namelist = NULL;
      nametail = NULL;
      return true;
    }

    cursor = namelist_match (file_name, length);
    if (cursor)
    {
      if (!(ISSLASH (file_name[cursor->length]) && recursion_option)
          || cursor->found_count == 0)
        cursor->found_count++; /* remember it matched */
      if (starting_file_option)
      {
        free (namelist);
        namelist = NULL;
        nametail = NULL;
      }
#if 0
      chdir_do (cursor->change_dir);
#endif

      /* We got a match.  */
      return ISFOUND (cursor);
    }

    /* Filename from archive not found in namelist.  If we have the whole
       namelist here, just return 0.  Otherwise, read the next name in and
       compare it.  If this was the last name, namelist->found_count will
       remain on.  If not, we loop to compare the newly read name.  */

    if (same_order_option && namelist->found_count)
    {
      name_gather ();	/* read one more */
      if (namelist->found_count)
        return false;
    }
    else
      return false;
  }
}

/* Returns true if all names from the namelist were processed.
   P is the stat_info of the most recently processed entry.
   The decision is postponed until the next entry is read if:

   1) P ended with a slash (i.e. it was a directory)
   2) P matches any entry from the namelist *and* represents a subdirectory
   or a file lying under this entry (in the terms of directory structure).

   This is necessary to handle contents of directories. */
bool
all_names_found (struct tar_stat_info *p)
{
  struct name const *cursor;
  size_t len;

  if (!p->file_name || occurrence_option == 0 || p->had_trailing_slash)
    return false;
  len = strlen (p->file_name);
  for (cursor = namelist; cursor; cursor = cursor->next)
    {
      if ((cursor->name[0] && !WASFOUND (cursor))
	  || (len >= cursor->length && ISSLASH (p->file_name[cursor->length])))
	return false;
    }
  return true;
}


/* Print the names of things in the namelist that were not matched.  */
void
names_notfound (void)
{
  struct name const *cursor;

  for (cursor = namelist; cursor; cursor = cursor->next)
    if (!WASFOUND (cursor) && cursor->name[0])
      {
      }

  /* Don't bother freeing the name list; we're about to exit.  */
  namelist = NULL;
  nametail = NULL;

  if (same_order_option)
    {
      const char *name;

      while ((name = name_next (1)) != NULL)
	{
	}
    }
}

void
label_notfound (void)
{
  struct name const *cursor;

  if (!namelist)
    return;

  for (cursor = namelist; cursor; cursor = cursor->next)
    if (WASFOUND (cursor))
      return;

  set_exit_status (2);


  /* Don't bother freeing the name list; we're about to exit.  */
  namelist = NULL;
  nametail = NULL;

}

/* Sorting name lists.  */

/* Sort *singly* linked LIST of names, of given LENGTH, using COMPARE
   to order names.  Return the sorted list.  Note that after calling
   this function, the 'prev' links in list elements are messed up.

   Apart from the type 'struct name' and the definition of SUCCESSOR,
   this is a generic list-sorting function, but it's too painful to
   make it both generic and portable
   in C.  */

static struct name *
merge_sort_sll (struct name *list, int length,
		int (*compare) (struct name const*, struct name const*))
{
  struct name *first_list;
  struct name *second_list;
  int first_length;
  int second_length;
  struct name *result;
  struct name **merge_point;
  struct name *cursor;
  int counter;

# define SUCCESSOR(name) ((name)->next)

  if (length == 1)
    return list;

  if (length == 2)
    {
      if ((*compare) (list, SUCCESSOR (list)) > 0)
	{
	  result = SUCCESSOR (list);
	  SUCCESSOR (result) = list;
	  SUCCESSOR (list) = 0;
	  return result;
	}
      return list;
    }

  first_list = list;
  first_length = (length + 1) / 2;
  second_length = length / 2;
  for (cursor = list, counter = first_length - 1;
       counter;
       cursor = SUCCESSOR (cursor), counter--)
    continue;
  second_list = SUCCESSOR (cursor);
  SUCCESSOR (cursor) = 0;

  first_list = merge_sort_sll (first_list, first_length, compare);
  second_list = merge_sort_sll (second_list, second_length, compare);

  merge_point = &result;
  while (first_list && second_list)
    if ((*compare) (first_list, second_list) < 0)
      {
	cursor = SUCCESSOR (first_list);
	*merge_point = first_list;
	merge_point = &SUCCESSOR (first_list);
	first_list = cursor;
      }
    else
      {
	cursor = SUCCESSOR (second_list);
	*merge_point = second_list;
	merge_point = &SUCCESSOR (second_list);
	second_list = cursor;
      }
  if (first_list)
    *merge_point = first_list;
  else
    *merge_point = second_list;

  return result;

#undef SUCCESSOR
}

/* Sort doubly linked LIST of names, of given LENGTH, using COMPARE
   to order names.  Return the sorted list.  */
static struct name *
merge_sort (struct name *list, int length,
	    int (*compare) (struct name const*, struct name const*))
{
  struct name *head, *p, *prev;
  head = merge_sort_sll (list, length, compare);
  /* Fixup prev pointers */
  for (prev = NULL, p = head; p; prev = p, p = p->next)
    p->prev = prev;
  return head;
}

/* A comparison function for sorting names.  Put found names last;
   break ties by string comparison.  */

static int
compare_names_found (struct name const *n1, struct name const *n2)
{
  int found_diff = WASFOUND (n2) - WASFOUND (n1);
  return found_diff ? found_diff : strcmp (n1->name, n2->name);
}

/* Simple comparison by names. */
static int
compare_names (struct name const *n1, struct name const *n2)
{
  return strcmp (n1->name, n2->name);
}

/* Auxiliary functions for hashed table of struct name's. */
static size_t
name_hash (void const *entry, size_t n_buckets)
{
  struct name const *name = entry;
  return hash_string (name->caname, n_buckets);
}

/* Compare two directories for equality of their names. */
static bool
name_compare (void const *entry1, void const *entry2)
{
  struct name const *name1 = entry1;
  struct name const *name2 = entry2;
  return strcmp (name1->caname, name2->caname) == 0;
}


/* Rebase 'name' member of CHILD and all its siblings to
   the new PARENT. */
static void
rebase_child_list (struct name *child, struct name *parent)
{
  size_t old_prefix_len = child->parent->length;
  size_t new_prefix_len = parent->length;
  char *new_prefix = parent->name;

  for (; child; child = child->sibling)
    {
      size_t size = child->length - old_prefix_len + new_prefix_len;
      char *newp = malloc (size + 1);
      strcpy (newp, new_prefix);
      strcat (newp, child->name + old_prefix_len);
      free (child->name);
      child->name = newp;
      child->length = size;

      rebase_directory (child->directory,
			child->parent->name, old_prefix_len,
			new_prefix, new_prefix_len);
    }
}

/* This is like name_match, except that
    1. It returns a pointer to the name it matched, and doesn't set FOUND
    in structure. The caller will have to do that if it wants to.
    2. If the namelist is empty, it returns null, unlike name_match, which
    returns TRUE. */
struct name *
name_scan (const char *file_name)
{
  size_t length = strlen (file_name);

  while (1)
    {
      struct name *cursor = namelist_match (file_name, length);
      if (cursor)
	return cursor;

      /* Filename from archive not found in namelist.  If we have the whole
	 namelist here, just return 0.  Otherwise, read the next name in and
	 compare it.  If this was the last name, namelist->found_count will
	 remain on.  If not, we loop to compare the newly read name.  */

      if (same_order_option && namelist && namelist->found_count)
	{
	  name_gather ();	/* read one more */
	  if (namelist->found_count)
	    return 0;
	}
      else
	return 0;
    }
}

/* This returns a name from the namelist which doesn't have ->found
   set.  It sets ->found before returning, so successive calls will
   find and return all the non-found names in the namelist.  */
struct name *gnu_list_name;

struct name const *
name_from_list (void)
{
  if (!gnu_list_name)
    gnu_list_name = namelist;
  while (gnu_list_name
	 && (gnu_list_name->found_count || gnu_list_name->name[0] == 0))
    gnu_list_name = gnu_list_name->next;
  if (gnu_list_name)
    {
      gnu_list_name->found_count++;
//      chdir_do (gnu_list_name->change_dir);
      return gnu_list_name;
    }
  return NULL;
}

void
blank_name_list (void)
{
  struct name *name;

  gnu_list_name = 0;
  for (name = namelist; name; name = name->next)
    name->found_count = 0;
}

/* Yield a newly allocated file name consisting of FILE_NAME concatenated to
   NAME, with an intervening slash if FILE_NAME does not already end in one. */
char *
new_name (const char *file_name, const char *name)
{
  size_t file_name_len = strlen (file_name);
  size_t namesize = strlen (name) + 1;
  int slash = file_name_len && ! ISSLASH (file_name[file_name_len - 1]);
  char *buffer = malloc (file_name_len + slash + namesize);
  memcpy (buffer, file_name, file_name_len);
  buffer[file_name_len] = '/';
  memcpy (buffer + file_name_len + slash, name, namesize);
  return buffer;
}



/* Return the size of the prefix of FILE_NAME that is removed after
   stripping NUM leading file name components.  NUM must be
   positive.  */

size_t
stripped_prefix_len (char const *file_name, size_t num)
{
  char const *p = file_name + FILE_SYSTEM_PREFIX_LEN (file_name);
  while (ISSLASH (*p))
    p++;
  while (*p)
    {
      bool slash = ISSLASH (*p);
      p++;
      if (slash)
	{
	  if (--num == 0)
	    return p - file_name;
	  while (ISSLASH (*p))
	    p++;
	}
    }
  return -1;
}

/* Return nonzero if NAME contains ".." as a file name component.  */
bool
contains_dot_dot (char const *name)
{
  char const *p = name + FILE_SYSTEM_PREFIX_LEN (name);

  for (;; p++)
    {
      if (p[0] == '.' && p[1] == '.' && (ISSLASH (p[2]) || !p[2]))
	return 1;

      while (! ISSLASH (*p))
	{
	  if (! *p++)
	    return 0;
	}
    }
}
