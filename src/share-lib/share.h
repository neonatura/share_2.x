
/*
 * @copyright
 *
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
 *
 *  @endcopyright
 */  


#ifndef __SHARE_H__
#define __SHARE_H__


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef SHARELIB
#include <string.h>
#include <math.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/if_arp.h>

/* gnulib includes */
#include <alloca.h>
#include <dirent.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <wchar.h>
#include <pthread.h>

/* more gnulib includes */
/*
#include "argmatch.h"
#include "argp-version-etc.h"
#include "argp.h"
#include "backupfile.h"
#include "closeout.h"
#include "configmake.h"
#include "dirname.h"
#include "error.h"
#include "exclude.h"
#include "exitfail.h"
#include "fnmatch.h"
#include "full-write.h"
#include "gettext.h"
#include "hash.h"
#include "human.h"
#include "inttostr.h"
#include "localcharset.h"
#include "modechange.h"
#include "obstack.h"
#include "parse-datetime.h"
#include "priv-set.h"
#include "progname.h"
#include "quote.h"
#include "quotearg.h"
#include "safe-read.h"
#include "savedir.h"
#include "stat-time.h"
#include "timespec.h"
#include "unlinkdir.h"
#include "unlocked-io.h"
#include "utimens.h"
#include "xalloc.h"
#include "xgetcwd.h"
#include "xstrtol.h"
#include "xvasprintf.h"
*/

#endif

#include <stdint.h>



/**
 *  @mainpage The Share Library
 *
 *  The libshare API reference manual.
 *
 *  This manual is divided in the following sections:
 *  - \subpage intro
 *  - \subpage intro_fs "The sharefs filesystem"
 *  - \subpage libshare_net "Networking and the ESP protocol."
 *  - \subpage libshare_mem "Hashmaps and memory pools."
 *  - \subpage libshare_pref "Library Configuration"
 */
/**
 *  @page intro Introduction
 *  This documentation covers the public API provided by the Share library. The documentation is for developers of 3rd-party applications intending to use this API. 
 *
 *  The following modules are available:
 *  - \subpage intro_fs "The sharefs filesystem"
 *  - \subpage libshare_net "Networking and the ESP protocol."
 *  - \subpage libshare_mem "Hashmaps and memory pools."
 *  - \subpage libshare_pref "Library Configuration"
 */

/**
 * @page intro_fs Accessing the share-fs filesystem.
 * Goto: @ref libshare_fs "The sharefs reference manual."
 *
 * The filesystem stores introduces new inode types in order to reference additional information relating to a file or directory.
 * @see shfs_ino_t
 *
 * The sharefs filesystem can overlay on top of the base filesystem by linking directories and files into a sharefs partition. The sharefs file may only link to the file, as needed, to conserve duplicate data.
 *
 * Revisions of files can be tracked and reverted. The sharefs uses compressed deltas in order to store each supplemental revision to a file. This design allows for small changes to large files with little overhead. 
 *
 * Permissions may be applied to which file revisions are imported or exported from remote machines by link files or directories to a remote sharefs partition. 
 *
 * Symbolic links can be used to reference local or remote paths. Multiple clients can link to a single directory or file in order to share revisions.
 *
 */

/**
 *
 *  @brief The Share Library
 *  @note The Share Library is hosted at https://github.com/neonatura/share
 *  @defgroup libshare 
 *  @{
 */

// See the libshare_meta.3 API man page for meta definition hash maps.

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif

#ifndef MIN
#define MIN(a,b) \
  (a < b ? a : b)
#endif

#ifndef MAX
#define MAX(a,b) \
  (a > b ? a : b)
#endif

#ifdef DEBUG
#define PRINT_RUSAGE(_msg) \
  shinfo(_msg); shlog_rinfo()
#else
#define PRINT_RUSAGE(_msg)
#endif

#define PRINT_ERROR(_err, _msg) \
  sherr(_err, _msg)

#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK 0x7f000001
#endif

#define MAX_SHARE_NAME_LENGTH 136
#define MAX_SHARE_PASS_LENGTH 136
#define MAX_SHARE_HASH_LENGTH 136

#define SHARE_PAGE_SIZE 8192

/**
 * A shtime_t representation of 01/01/60 UTC
 */
#define SHARE_DEFAULT_EXPIRE_TIME 15147432000 

/**
 * A specification of byte size.
 * @manonly
 * See the libshare_net.3 API man page for ESP protocol network operations.
 * @endmanonly
 * @seealso shmeta_value_t.sz
 * @note This type is typically only used for disk storage or socket communications. A regular @ref size_t is used when the bitsize of a number being reference is not restricted.
 */
typedef uint64_t shsize_t;

/**
 * An email address where bug reports can be submitted.
 */
char *get_libshare_email(void);

/**
 * The current libshare library version.
 */
char *get_libshare_version(void);

/**
 * The libshare library package name.
 */
char *get_libshare_title(void);

/**
 * Unix: /var/lib/share or ~/.share
 * Windows: C:\Users\Username\AppData\Roaming\.share
 * Mac: ~/Library/Application Support/.share
 * @returns The directory where share library persistent data is stored.
 * @note This value can be overwritten with a shared preference.
 */
const char *get_libshare_path(void);

const char *get_libshare_account_name(void);

const char *get_libshare_account_email(void);


#include "sherr.h"
#include "shtime.h"
#include "shcrc.h"
#include "shmem.h"
#include "shpeer.h"
#include "shpref.h"
#include "shfs.h"
#include "shnet.h"

#ifdef SHARELIB
#include "test/shtest.h"
#endif


/**
 * @}
 *
 *  @page libshare_net Networking
 *  Sub-topics:
 *    - @subpage libshare_net_esp
 *
 *  @page libshare_net_esp Encoded Stream Protocol
 *
 *  @page libshare_meta Meta Definition Hashmaps
 *
 *    Metadef hashmaps are used by the Share Library \ref libshare_fs "sharefs" and \ref libshare_net "networking" modules.
 *
 *  @page libshare_mem Hashmaps and memory pools.
 *  Sub-topics:
 *    - @subpage libshare_memmeta "Meta definition hashmaps."
 *    - @subpage libshare_mempool "Memory buffer pools."
 *    - @subpage libshare_membuf "Dynamic memory buffers."
 *
 *  @page libshare_memmeta Meta Definition Hashmaps
 *
 *    Metadef hashmaps are used by the Share Library libshare_fs "sharefs" and libshare_net "networking" modules.
 *    \ref libshare_fs
 *    \ref libshare_net
 *
 *  @page libshare_mempool Memory Buffer Pools
 *
 *  @page libshare_membuf Dynamic Memory Buffers
 *
 *
 *  @page libshare_key Hash/Digest Keys
 *
 *  @page libshare_pref Library configuration.
 */

#endif /* ndef __SHARE_H__ */


