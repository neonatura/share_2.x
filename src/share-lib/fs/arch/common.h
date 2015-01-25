


#ifndef __ARCH_COMMON_H__
#define __ARCH_COMMON_H__


#include <inttostr.h>
#include <quotearg.h>
#include <fnmatch.h>
//#include <hash.h>

#include "share.h"
#include "dirname.h"
#include "shfs_arch.h"
#include "tar.h"
#include "system.h"


/* The checksum field is filled with this while the checksum is computed.  */
#define CHKBLANKS	"        "	/* 8 blanks, no null */

/* Some constants from POSIX are given names.  */
#define NAME_FIELD_SIZE   100
#define PREFIX_FIELD_SIZE 155
#define UNAME_FIELD_SIZE   32
#define GNAME_FIELD_SIZE   32

#define   SAFE_READ_ERROR   ((size_t) -1)
#define ROOT_UID 0


/* Some various global definitions.  */

/* Name of file to use for interacting with user.  */

/* GLOBAL is defined to empty in tar.c only, and left alone in other *.c
   modules.  Here, we merely set it to "extern" if it is not already set.
   GNU tar does depend on the system loader to preset all GLOBAL variables to
   neutral (or zero) values, explicit initialization is usually not done.  */
#ifndef GLOBAL
# define GLOBAL extern
#endif


#include "arith.h"
#include <exclude.h>
#include <full-write.h>
#include <modechange.h>
#include <quote.h>
#include <stat-time.h>
#include <timespec.h>
#define obstack_chunk_alloc xmalloc
#define obstack_chunk_free free
#include <obstack.h>
#include <progname.h>
#include "paxlib.h"


/* Log base 2 of common values.  */
#define LG_8 3
#define LG_64 6
#define LG_256 8

_GL_INLINE_HEADER_BEGIN
#ifndef COMMON_INLINE
# define COMMON_INLINE _GL_INLINE
#endif



extern union block *record_start;      /* start of record of archive */

/* Main command option.  */

enum subcommand
{
  UNKNOWN_SUBCOMMAND,		/* none of the following */
  APPEND_SUBCOMMAND,		/* -r */
  CAT_SUBCOMMAND,		/* -A */
  CREATE_SUBCOMMAND,		/* -c */
  DELETE_SUBCOMMAND,		/* -D */
  DIFF_SUBCOMMAND,		/* -d */
  EXTRACT_SUBCOMMAND,		/* -x */
  LIST_SUBCOMMAND,		/* -t */
  UPDATE_SUBCOMMAND,		/* -u */
  TEST_LABEL_SUBCOMMAND,        /* --test-label */
};

GLOBAL enum subcommand subcommand_option;

/* Selected format for output archive.  */
GLOBAL enum archive_format archive_format;

/* Size of each record, once in blocks, once in bytes.  Those two variables
   are always related, the second being BLOCKSIZE times the first.  They do
   not have _option in their name, even if their values is derived from
   option decoding, as these are especially important in tar.  */
GLOBAL int blocking_factor;
GLOBAL size_t record_size;

GLOBAL bool absolute_names_option;

/* Display file times in UTC */
GLOBAL bool utc_option;
/* Output file timestamps to the full resolution */
GLOBAL bool full_time_option;

/* This variable tells how to interpret newer_mtime_option, below.  If zero,
   files get archived if their mtime is not less than newer_mtime_option.
   If nonzero, files get archived if *either* their ctime or mtime is not less
   than newer_mtime_option.  */
GLOBAL int after_date_option;

enum atime_preserve
{
  no_atime_preserve,
  replace_atime_preserve,
  system_atime_preserve
};
GLOBAL enum atime_preserve atime_preserve_option;

GLOBAL bool block_number_option;

GLOBAL unsigned checkpoint_option;
#define DEFAULT_CHECKPOINT 10

/* Specified name of compression program, or "gzip" as implied by -z.  */
GLOBAL const char *use_compress_program_option;

GLOBAL bool dereference_option;
GLOBAL bool hard_dereference_option;

/* Patterns that match file names to be excluded.  */
GLOBAL struct exclude *excluded;

enum exclusion_tag_type
  {
    exclusion_tag_none,
     /* Exclude the directory contents, but preserve the directory
	itself and the exclusion tag file */
    exclusion_tag_contents,
    /* Exclude everything below the directory, preserving the directory
       itself */
    exclusion_tag_under,
    /* Exclude entire directory  */
    exclusion_tag_all,
  };

/* Specified value to be put into tar file in place of stat () results, or
   just null and -1 if such an override should not take place.  */
GLOBAL char const *group_name_option;
GLOBAL gid_t group_option;

GLOBAL bool ignore_failed_read_option;

GLOBAL bool ignore_zeros_option;

GLOBAL bool incremental_option;

/* Specified name of script to run at end of each tape change.  */
GLOBAL const char *info_script_option;

GLOBAL bool interactive_option;

/* If nonzero, extract only Nth occurrence of each named file */
GLOBAL uintmax_t occurrence_option;

enum old_files
{
  DEFAULT_OLD_FILES,          /* default */
  NO_OVERWRITE_DIR_OLD_FILES, /* --no-overwrite-dir */
  OVERWRITE_OLD_FILES,        /* --overwrite */
  UNLINK_FIRST_OLD_FILES,     /* --unlink-first */
  KEEP_OLD_FILES,             /* --keep-old-files */
  SKIP_OLD_FILES,             /* --skip-old-files */
  KEEP_NEWER_FILES	      /* --keep-newer-files */
};
GLOBAL enum old_files old_files_option;

GLOBAL bool keep_directory_symlink_option;

/* Specified file name for incremental list.  */
GLOBAL const char *listed_incremental_option;
/* Incremental dump level */
GLOBAL int incremental_level;
/* Check device numbers when doing incremental dumps. */
GLOBAL bool check_device_option;

/* Specified mode change string.  */
GLOBAL struct mode_change *mode_option;

/* Initial umask, if needed for mode change string.  */
GLOBAL mode_t initial_umask;

GLOBAL bool multi_volume_option;

/* Specified threshold date and time.  Files having an older time stamp
   do not get archived (also see after_date_option above).  */
GLOBAL struct timespec newer_mtime_option;

/* If true, override actual mtime (see below) */
GLOBAL bool set_mtime_option;
/* Value to be put in mtime header field instead of the actual mtime */
GLOBAL struct timespec mtime_option;

/* Return true if newer_mtime_option is initialized.  */
#define NEWER_OPTION_INITIALIZED(opt) (0 <= (opt).tv_nsec)

/* Return true if the struct stat ST's M time is less than
   newer_mtime_option.  */
#define OLDER_STAT_TIME(st, m) \
  (timespec_cmp (get_stat_##m##time (&(st)), newer_mtime_option) < 0)

/* Likewise, for struct tar_stat_info ST.  */
#define OLDER_TAR_STAT_TIME(st, m) \
  (timespec_cmp ((st).m##time, newer_mtime_option) < 0)

/* Zero if there is no recursion, otherwise FNM_LEADING_DIR.  */
GLOBAL int recursion_option;

GLOBAL bool numeric_owner_option;

GLOBAL bool one_file_system_option;

/* Create a top-level directory for extracting based on the archive name.  */
GLOBAL bool one_top_level_option;
GLOBAL char *one_top_level_dir;

/* Specified value to be put into tar file in place of stat () results, or
   just null and -1 if such an override should not take place.  */
GLOBAL char const *owner_name_option;
GLOBAL uid_t owner_option;

GLOBAL bool recursive_unlink_option;

GLOBAL bool read_full_records_option;

/* Specified remote shell command.  */
GLOBAL const char *rsh_command_option;

GLOBAL bool same_order_option;

/* If positive, preserve ownership when extracting.  */
GLOBAL int same_owner_option;

/* If positive, preserve permissions when extracting.  */
GLOBAL int same_permissions_option;

/* If positive, save the SELinux context.  */
GLOBAL int selinux_context_option;

/* If positive, save the ACLs.  */
GLOBAL int acls_option;

/* When set, strip the given number of file name components from the file name
   before extracting */
GLOBAL size_t strip_name_components;

GLOBAL bool show_omitted_dirs_option;

GLOBAL bool sparse_option;
GLOBAL unsigned tar_sparse_major;
GLOBAL unsigned tar_sparse_minor;

GLOBAL bool starting_file_option;

/* Specified maximum byte length of each tape volume (multiple of 1024).  */
GLOBAL tarlong tape_length_option;

GLOBAL bool to_stdout_option;

GLOBAL bool totals_option;

GLOBAL bool touch_option;

GLOBAL char *to_command_option;
GLOBAL bool ignore_command_error_option;

/* Restrict some potentially harmful tar options */
GLOBAL bool restrict_option;

/* Return true if the extracted files are not being written to disk */
#define EXTRACT_OVER_PIPE (to_stdout_option || to_command_option)

/* Count how many times the option has been set, multiple setting yields
   more verbose behavior.  Value 0 means no verbosity, 1 means file name
   only, 2 means file name and all attributes.  More than 2 is just like 2.  */
GLOBAL int verbose_option;

/* Specified name of file containing the volume number.  */
GLOBAL const char *volno_file_option;

/* Specified value or pattern.  */
GLOBAL const char *volume_label_option;

/* Other global variables.  */

/* File descriptor for archive file.  */
extern shbuf_t *archive;

/* Nonzero when outputting to /dev/null.  */
GLOBAL bool dev_null_output;

/* Timestamps: */
GLOBAL struct timespec start_time;        /* when we started execution */
GLOBAL struct timespec volume_start_time; /* when the current volume was
					     opened*/
GLOBAL struct timespec last_stat_time;    /* when the statistics was last
					     computed */

GLOBAL struct tar_stat_info current_stat_info;

/* List of tape drive names, number of such tape drives,
   and current cursor in list.  */
GLOBAL const char **archive_name_array;
GLOBAL size_t archive_names;
GLOBAL const char **archive_name_cursor;

/* Output index file name.  */
GLOBAL char const *index_file_name;

/* Opaque structure for keeping directory meta-data */
struct directory;

/* Structure for keeping track of filenames and lists thereof.  */
struct name
  {
    struct name *next;          /* Link to the next element */
    struct name *prev;          /* Link to the previous element */

    char *name;                 /* File name or globbing pattern */
    size_t length;		/* cached strlen (name) */
    int matching_flags;         /* wildcard flags if name is a pattern */
    bool cmdline;               /* true if this name was given in the
				   command line */

    int change_dir;		/* Number of the directory to change to.
				   Set with the -C option. */
    uintmax_t found_count;	/* number of times a matching file has
				   been found */

    /* The following members are used for incremental dumps only,
       if this struct name represents a directory;
       see incremen.c */
    struct directory *directory;/* directory meta-data and contents */
    struct name *parent;        /* pointer to the parent hierarchy */
    struct name *child;         /* pointer to the first child */
    struct name *sibling;       /* pointer to the next sibling */
    char *caname;               /* canonical name */
  };

/* Obnoxious test to see if dimwit is trying to dump the archive.  */
GLOBAL dev_t ar_dev;
GLOBAL ino_t ar_ino;

/* Flags for reading, searching, and fstatatting files.  */
GLOBAL int open_read_flags;
GLOBAL int open_searchdir_flags;
GLOBAL int fstatat_flags;

GLOBAL int seek_option;
GLOBAL bool seekable_archive;

GLOBAL dev_t root_device;

/* Unquote filenames */
GLOBAL bool unquote_option;

GLOBAL int savedir_sort_order;

/* Show file or archive names after transformation.
   In particular, when creating archive in verbose mode, list member names
   as stored in the archive */
GLOBAL bool show_transformed_names_option;

/* Delay setting modification times and permissions of extracted directories
   until the end of extraction. This variable helps correctly restore directory
   timestamps from archives with an unusual member order. It is automatically
   set for incremental archives. */
GLOBAL bool delay_directory_restore_option;

/* Warn about implicit use of the wildcards in command line arguments.
   (Default for tar prior to 1.15.91, but changed afterwards */
GLOBAL bool warn_regex_usage;

/* Declarations for each module.  */

/* FIXME: compare.c should not directly handle the following variable,
   instead, this should be done in buffer.c only.  */

extern void *arch_record_buffer[2];  /* allocated memory */

enum access_mode
{
  ACCESS_READ,
  ACCESS_WRITE,
  ACCESS_UPDATE
};
extern enum access_mode access_mode;

/* Module buffer.c.  */
extern union block *record_end;        /* last+1 block of archive record */
extern union block *current_block;     /* current block of archive */
extern off_t records_read;             /* number of records read from this archive */
extern int arch_record_index;

extern FILE *stdlis;
extern bool write_archive_to_stdout;
extern char *volume_label;
extern size_t volume_label_count;
extern char *continued_file_name;
extern uintmax_t continued_file_size;
extern uintmax_t continued_file_offset;
extern off_t records_written;

char *drop_volume_label_suffix (const char *label);

size_t available_space_after (union block *pointer);
off_t current_block_ordinal (void);
void close_archive (void);
void closeout_volume_number (void);
double compute_duration (void);
void flush_read (void);
void flush_write (void);
void flush_archive (void);
void init_volume_number (void);
void open_archive (enum access_mode mode);
void print_total_stats (void);
void reset_eof (void);
void set_next_block_after (union block *block);
void clear_read_error_count (void);
void xclose (int fd);
void archive_write_error (ssize_t status) __attribute__ ((noreturn));
void archive_read_error (void);
off_t seek_archive (off_t size);
void set_start_time (void);

#define TF_READ    0
#define TF_WRITE   1
#define TF_DELETED 2
int format_total_stats (FILE *fp, const char **formats, int eor, int eol);
void print_total_stats (void);

void mv_begin_write (const char *file_name, off_t totsize, off_t sizeleft);

void mv_begin_read (struct tar_stat_info *st);
void mv_end (void);
void mv_size_left (off_t size);

void buffer_write_global_xheader (void);

const char *first_decompress_program (int *pstate);
const char *next_decompress_program (int *pstate);

/* Module create.c.  */

enum dump_status
  {
    dump_status_ok,
    dump_status_short,
    dump_status_fail,
    dump_status_not_implemented
  };

void add_exclusion_tag (const char *name, enum exclusion_tag_type type,
			bool (*predicate) (int));
bool cachedir_file_p (int fd);
char *get_directory_entries (struct tar_stat_info *st);

void create_archive (void);
void pad_archive (off_t size_left);
void dump_file (struct tar_stat_info *parent, char const *name,
		char const *fullname);
union block *start_header (struct tar_stat_info *st);
void finish_header (struct tar_stat_info *st, union block *header,
		    off_t block_ordinal);
void simple_finish_header (union block *header);
union block * write_extended (bool global, struct tar_stat_info *st,
			      union block *old_header);
union block *start_private_header (const char *name, size_t size, time_t t);
void write_eot (void);
void check_links (void);
int subfile_open (struct tar_stat_info const *dir, char const *file, int flags);
void restore_parent_fd (struct tar_stat_info const *st);
void exclusion_tag_warning (const char *dirname, const char *tagname,
			    const char *message);
enum exclusion_tag_type check_exclusion_tags (struct tar_stat_info const *st,
					      const char **tag_file_name);

#define OFF_TO_CHARS(val, where) off_to_chars (val, where, sizeof (where))
#define TIME_TO_CHARS(val, where) time_to_chars (val, where, sizeof (where))

bool off_to_chars (off_t off, char *buf, size_t size);
bool time_to_chars (time_t t, char *buf, size_t size);

/* Module diffarch.c.  */

extern bool now_verifying;

void diff_archive (void);
void diff_init (void);
void verify_volume (void);

/* Module extract.c.  */

void extr_init (void);
void extract_archive (void);
void extract_finish (void);
bool rename_directory (char *src, char *dst);

/* Module delete.c.  */

void delete_archive_members (void);

/* Module incremen.c.  */

struct directory *scan_directory (struct tar_stat_info *st);
const char *directory_contents (struct directory *dir);
const char *safe_directory_contents (struct directory *dir);

void rebase_directory (struct directory *dir,
		       const char *samp, size_t slen,
		       const char *repl, size_t rlen);

void append_incremental_renames (struct directory *dir);
void show_snapshot_field_ranges (void);
void read_directory_file (void);
void write_directory_file (void);
void purge_directory (char const *directory_name);
void list_dumpdir (char *buffer, size_t size);
void update_parent_directory (struct tar_stat_info *st);

size_t dumpdir_size (const char *p);
bool is_dumpdir (struct tar_stat_info *stat_info);
void clear_directory_table (void);

/* Module list.c.  */

enum read_header
{
  HEADER_STILL_UNREAD,		/* for when read_header has not been called */
  HEADER_SUCCESS,		/* header successfully read and checksummed */
  HEADER_SUCCESS_EXTENDED,	/* likewise, but we got an extended header */
  HEADER_ZERO_BLOCK,		/* zero block where header expected */
  HEADER_END_OF_FILE,		/* true end of file while header expected */
  HEADER_FAILURE		/* ill-formed header, or bad checksum */
};

/* Operation mode for read_header: */

enum read_header_mode
{
  read_header_auto,             /* process extended headers automatically */
  read_header_x_raw,            /* return raw extended headers (return
				   HEADER_SUCCESS_EXTENDED) */
  read_header_x_global          /* when POSIX global extended header is read,
				   decode it and return
				   HEADER_SUCCESS_EXTENDED */
};
extern union block *current_header;
extern enum archive_format current_format;
extern size_t recent_long_name_blocks;
extern size_t recent_long_link_blocks;

void decode_header (union block *header, struct tar_stat_info *stat_info,
		    enum archive_format *format_pointer, int do_user_group);
void transform_stat_info (int typeflag, struct tar_stat_info *stat_info);
char const *tartime (struct timespec t, bool full_time);

#define OFF_FROM_HEADER(where) off_from_header (where, sizeof (where))
#define UINTMAX_FROM_HEADER(where) uintmax_from_header (where, sizeof (where))

off_t off_from_header (const char *buf, size_t size);
uintmax_t uintmax_from_header (const char *buf, size_t size);

void list_archive (void);
void test_archive_label (void);
void print_for_mkdir (char *dirname, int length, mode_t mode);
void print_header (struct tar_stat_info *st, union block *blk,
	           off_t block_ordinal);
void read_and (void (*do_something) (void));
enum read_header read_header (union block **return_block,
			      struct tar_stat_info *info,
			      enum read_header_mode m);
enum read_header tar_checksum (union block *header, bool silent);
void skip_file (off_t size);
void skip_member (void);

/* Module misc.c.  */

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) < (b) ? (b) : (a))
void assign_string (char **dest, const char *src);
int unquote_string (char *str);
char *zap_slashes (char *name);
char *normalize_filename (int cdidx, const char *name);
void normalize_filename_x (char *name);
void replace_prefix (char **pname, const char *samp, size_t slen,
		     const char *repl, size_t rlen);
char *tar_savedir (const char *name, int must_exist);

typedef struct namebuf *namebuf_t;
namebuf_t namebuf_create (const char *dir);
void namebuf_free (namebuf_t buf);
char *namebuf_name (namebuf_t buf, const char *name);
void namebuf_add_dir (namebuf_t buf, const char *name);
char *namebuf_finish (namebuf_t buf);

const char *tar_dirname (void);

/* Represent N using a signed integer I such that (uintmax_t) I == N.
   With a good optimizing compiler, this is equivalent to (intmax_t) i
   and requires zero machine instructions.  */
#if ! (UINTMAX_MAX / 2 <= INTMAX_MAX)
# error "represent_uintmax returns intmax_t to represent uintmax_t"
#endif
extern intmax_t represent_uintmax (uintmax_t n);

enum { SYSINT_BUFSIZE =
	 max (UINTMAX_STRSIZE_BOUND, INT_BUFSIZE_BOUND (intmax_t)) };
char *sysinttostr (uintmax_t, intmax_t, uintmax_t, char buf[SYSINT_BUFSIZE]);
intmax_t strtosysint (char const *, char **, intmax_t, uintmax_t);
void code_ns_fraction (int ns, char *p);
char const *code_timespec (struct timespec ts, char *sbuf);
enum { BILLION = 1000000000, LOG10_BILLION = 9 };
enum { TIMESPEC_STRSIZE_BOUND =
         UINTMAX_STRSIZE_BOUND + LOG10_BILLION + sizeof "-." - 1 };
struct timespec decode_timespec (char const *, char **, bool);

/* Return true if T does not represent an out-of-range or invalid value.  */
bool valid_timespec (struct timespec t);

bool must_be_dot_or_slash (char const *);

enum remove_option
{
  ORDINARY_REMOVE_OPTION,
  RECURSIVE_REMOVE_OPTION,

  /* FIXME: The following value is never used. It seems to be intended
     as a placeholder for a hypothetical option that should instruct tar
     to recursively remove subdirectories in purge_directory(),
     as opposed to the functionality of --recursive-unlink
     (RECURSIVE_REMOVE_OPTION value), which removes them in
     prepare_to_extract() phase. However, with the addition of more
     meta-info to the incremental dumps, this should become unnecessary */
  WANT_DIRECTORY_REMOVE_OPTION
};
int remove_any_file (const char *file_name, enum remove_option option);

int deref_stat (char const *name, struct stat *buf);

size_t blocking_read (int fd, void *buf, size_t count);
size_t blocking_write (int fd, void const *buf, size_t count);

extern int chdir_current;
extern int chdir_fd;
int chdir_arg (char const *dir);
void chdir_do (int dir);
int chdir_count (void);

void close_diag (char const *name);
void open_diag (char const *name);
void read_diag_details (char const *name, off_t offset, size_t size);
void readlink_diag (char const *name);
void savedir_diag (char const *name);
void seek_diag_details (char const *name, off_t offset);
void stat_diag (char const *name);
void file_removed_diag (const char *name, bool top_level,
			void (*diagfn) (char const *name));
void write_error_details (char const *name, size_t status, size_t size);
void write_fatal (char const *name) __attribute__ ((noreturn));
void write_fatal_details (char const *name, ssize_t status, size_t size)
     __attribute__ ((noreturn));

pid_t xfork (void);
void xpipe (int fd[2]);

void *page_aligned_alloc (void **ptr, size_t size);
int set_file_atime (int fd, int parentfd, char const *file,
		    struct timespec atime);

/* Module names.c.  */

extern size_t name_count;
extern struct name *gnu_list_name;

void gid_to_gname (gid_t gid, char **gname);
int gname_to_gid (char const *gname, gid_t *pgid);
void uid_to_uname (uid_t uid, char **uname);
int uname_to_uid (char const *uname, uid_t *puid);

void name_init (void);
void name_add_name (const char *name, int matching_flags);
void name_add_dir (const char *name);
void name_add_file (const char *name, int term);
void name_term (void);
const char *name_next (int change_dirs);
void name_gather (void);
struct name *addname (char const *string, int change_dir,
		      bool cmdline, struct name *parent);
void remname (struct name *name);
bool name_match (const char *name);
void names_notfound (void);
void label_notfound (void);
void collect_and_sort_names (void);
struct name *name_scan (const char *name);
struct name const *name_from_list (void);
void blank_name_list (void);
char *new_name (const char *dir_name, const char *name);
size_t stripped_prefix_len (char const *file_name, size_t num);
bool all_names_found (struct tar_stat_info *st);

void add_avoided_name (char const *name);
bool is_avoided_name (char const *name);

bool contains_dot_dot (char const *name);

#define ISFOUND(c) ((occurrence_option == 0) ? (c)->found_count : \
                    (c)->found_count == occurrence_option)
#define WASFOUND(c) ((occurrence_option == 0) ? (c)->found_count : \
                     (c)->found_count >= occurrence_option)

/* Module tar.c.  */

void usage (int);

int confirm (const char *message_action, const char *name);

void tar_stat_init (struct tar_stat_info *st);
bool tar_stat_close (struct tar_stat_info *st);
void tar_stat_destroy (struct tar_stat_info *st);
void usage (int) __attribute__ ((noreturn));
int tar_timespec_cmp (struct timespec a, struct timespec b);
const char *archive_format_string (enum archive_format fmt);
const char *subcommand_string (enum subcommand c);
void set_exit_status (int val);

void request_stdin (const char *option);
void more_options (int argc, char **argv);

/* Module update.c.  */

extern char *output_start;

void update_archive (void);

/* Module system.c */

size_t sys_write_archive_buffer (void);

/* Module compare.c */
void report_difference (struct tar_stat_info *st, const char *message, ...)
  __attribute__ ((format (printf, 2, 3)));

/* Module sparse.c */
bool sparse_member_p (struct tar_stat_info *st);
bool sparse_fixup_header (struct tar_stat_info *st);
enum dump_status sparse_dump_file (int, struct tar_stat_info *st);
enum dump_status sparse_extract_file (int fd, struct tar_stat_info *st,
				      off_t *size);
enum dump_status sparse_skip_file (struct tar_stat_info *st);
bool sparse_diff_file (int, struct tar_stat_info *st);

/* Module utf8.c */
bool string_ascii_p (const char *str);
bool utf8_convert (bool to_utf, char const *input, char **output);

/* Module transform.c */
#define XFORM_REGFILE  0x01
#define XFORM_LINK     0x02
#define XFORM_SYMLINK  0x04
#define XFORM_ALL      (XFORM_REGFILE|XFORM_LINK|XFORM_SYMLINK)

void set_transform_expr (const char *expr);
bool transform_name (char **pinput, int type);
bool transform_name_fp (char **pinput, int type,
			char *(*fun)(char *, void *), void *);
bool transform_program_p (void);

/* Module suffix.c */
void set_compression_program_by_suffix (const char *name, const char *defprog);
char *strip_compression_suffix (const char *name);

/* Module checkpoint.c */
void checkpoint_compile_action (const char *str);
void checkpoint_finish_compile (void);
void checkpoint_run (bool do_write);
void checkpoint_finish (void);
void checkpoint_flush_actions (void);


/* Module unlink.c */

void queue_deferred_unlink (const char *name, bool is_dir);
void finish_deferred_unlinks (void);


/* Module exclist.c */
#define EXCL_DEFAULT       0x00
#define EXCL_RECURSIVE     0x01
#define EXCL_NON_RECURSIVE 0x02

void excfile_add (const char *name, int flags);
void info_attach_exclist (struct tar_stat_info *dir);
void info_cleanup_exclist (struct tar_stat_info *dir);
void info_free_exclist (struct tar_stat_info *dir);
bool excluded_name (char const *name, struct tar_stat_info *st);
void exclude_vcs_ignores (void);

_GL_INLINE_HEADER_END

#define major_t int

/* Define to a type if <wchar.h> does not define. */
/* #undef mbstate_t */

/* Type of minor device numbers. */
#define minor_t int

/* Define to the type of elements in the array set by `getgroups'. Usually
   this is either `int' or `gid_t'. */
#define GETGROUPS_T gid_t


int arch_buffer_seek(shbuf_t *buff, size_t offset, int whence);
int arch_buffer_write(shbuf_t *buff, void *data, size_t data_len);
int arch_buffer_read(shbuf_t *buff, void *data, size_t data_len);
union block *arch_buffer_next(void);
void arch_buffer_eot(void);


#endif /* ndef __ARCH_COMMON_H__ */
