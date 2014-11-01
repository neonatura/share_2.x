
/*
 * @copyright
 *
 *  Copyright 2013 Brian Burrell 
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
 *  @file share.h 
 *  @brief The Share Library
 *  @author Brian Burrell
 *  @date 2013
 *
 *  Provides: Optimized file system, IPC, and network operations.
 *  Used By: Client programs.
 */  


#ifndef __SHARE_H__
#define __SHARE_H__


#ifdef SHARELIB

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif

#include <fcntl.h>
#include <math.h>
#include <sys/types.h>

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#define STAT stat
#endif

#include <sys/socket.h>
#include <netinet/in.h>


#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#define FCNTL(_fd, _mode, _opt) fcntl(_fd, _mode, _opt)
#endif

#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>

/* gnulib includes */
#include <alloca.h>
#include <dirent.h>
#include <fcntl.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
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

#include "share_base.h"

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
 *  This following modules are available:
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
 *  @page libshare_mempool Memory Buffer Pools
 *
 *  @page libshare_membuf Dynamic Memory Buffers
 *
 *    Metadef hashmaps are used by the Share Library \ref libshare_fs "sharefs" and \ref libshare_net "networking" modules.
 *
 *  @page libshare_key Hash/Digest Keys
 *
 *  @page libshare_pref Hash/Digest Keys
 *
 *  @brief The Share Library
 *  @note The Share Library is hosted at https://github.com/neonatura/share
 *  @defgroup libshare 
 *  @{
 */

// See the libshare_meta.3 API man page for meta definition hash maps.

/**
 * An arbitrary number specifying the theoretical maximum thread count of a process.
 * @see ashkey_num() ashkey_str()
 */
#define MAX_SHARE_THREADS 256

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
 * Unix: ~/.shlib
 * Windows: C:\Users\Username\AppData\Roaming\.shlib
 * Mac: ~/Library/Application Support/.shlib
 * @returns The directory where share library persistent data is stored.
 * @note This value can be overwritten with a shared preference.
 */
const char *get_libshare_path(void);


#include "sherr.h"
#include "shtime.h"
#include "shcrc.h"
#include "shfile.h"
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
 */

#endif /* ndef __SHARE_H__ */


