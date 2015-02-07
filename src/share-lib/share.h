
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
#define SHARE_DEFAULT_EXPIRE_TIME 15147432000 /* 48 years */ 

#define MAX_SHARE_SESSION_TIME 2073600 /* 24 days */

/**
 * A specification of byte size.
 * @manonly
 * See the libshare_net.3 API man page for ESTP protocol network operations.
 * @endmanonly
 * @seealso shmeta_value_t.sz
 * @note This type is typically only used for disk storage or socket communications. A regular @ref size_t is used when the bitsize of a number being reference is not restricted.
 */
typedef uint64_t shsize_t;


#include "sherr.h"
#include "shtime.h"
#include "shcrc.h"
#include "shmem.h"
#include "shpeer.h"
#include "shpref.h"
#include "shfs.h"
#include "shnet.h"
#include "shsys.h"


//const char *get_libshare_account_email(void);

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

/**
 * @}
 */

#ifdef SHARELIB
#include "test/shtest.h"
#endif





/**
 *  @mainpage The Share Library
 *
 *  <h3>The libshare API reference manual.</h3>
 *
 *  This documentation covers the public API provided by the Share library. The documentation is for developers of 3rd-party applications intending to use this API. 
 *
 *  This manual is divided in the following sections:
 *  - \subpage libshare "Core Programming Interface"
 *  <dl>The libshare core programming interface provides basic routines used through-out the remaining sections of the library. These include methods to track error status, generate checksum verification, application end-point referencing, the tracking of time, and user-specific library configuration settings.</dl>
 *
 *  - \subpage libshare_mem "Memory manipulation routines."
 *  <dl>The libshare memory manipulation routines provide methods to buffer and encode various types of data.</dl>
 *
 *  - \subpage libshare_net "Networking and the ESTP protocol."
 *  <dl>The libshare networking interface provides extensions to the IP protocol / system network handling. The ESTP protocol is an alternative to , or as a layered tunnel upon, the TCP IP protocol.</dl>
 *
 *  - \subpage libshare_fs "The sharefs filesystem"
 *  <dl>The libshare sharefs file-system provides multiple partitions based on the underlying application context. The file-system has extended attributes which allow for alternate storage methods such as archival, compress, version revision, licensing, The filesystem stores introduces new inode types in order to reference additional information relating to a file or directory.</dl>
 *
 *  - \subpage libshare_sys "System-level process management."
 *  <dl>System-level access to libshare account permission and process-level locks.</dl>
 */

/**
 *  @page libshare Core Programming Interface
 *
 *  The libshare library's core functionality is the first layer of the API. This functionality is utilized by the memory, networking, filesytem, and system-level areas of the libshare library in order to provide additional layers of functionality.
 *
 *  In turn, the libshare suite daemons and utility programs are based upon the library layer to provide general access and distribution of information. Developers utilize the libshare library, or an alternate such as the SEXE runtime library, in order to access and distribute additional information, and/or make use of the conveinence functions provided to perform common C routes such as dynamic memory management and simplified socket handling.
 *
 *
 *  The core of the libshare library is can be broken down into the follow sections:
 *
 *  - \subpage libshare_crc "CRC checksum verification."
 *  <dl>A checksum algorithm that computes a 64-bit number from a segment of data.</dl>
 *
 *  - \subpage libshare_err "Error and status codes."
 *  <dl>Error codes returned from libshare API functions.</dl>
 *
 *  - \subpage libshare_peer "Applicatin peer identification."
 *  <dl>Information referencing application information</dl>
 *
 *  - \subpage libshare_time "System time and duration tracking."
 *  <dl>System time functions with extended precision.</dl>
 *
 *  - \subpage libshare_pref "Library configuration settings."
 *  <dl>User-defined settings which control the behaviour of core libshare functionality.</dl>
 */

/**
 *  @page libshare_crc CRC Checksum Verification
 *
 *  The libshare library provides a method to generate and print CRC checksums derived from segments of binary information. The algorithm computes a 64-bit number from the data segment.
 *
 *  The algorythm is a modified form of adler32 suitable for 64bit generation. The checksum is not compatible with standard adler based algorythms. 
 *
 *  The checksum is used to generate a libshare "Key" (shkey_t) used through-out the libshare library suite.
 *
 *  References:
 *    - <a href="http://whttp://en.wikipedia.org/wiki/Checksum">Wikipedia: Checksum</a>
 *    - \subpage libshare_memkey
 *
 */

/**
 *  @page libshare_err Error Codes
 *
 *
 */

/**
 *  @page libshare_peer Application Peer Identification
 *
 *
 */

/**
 *  @page libshare_time Time Tracking 
 *
 *
 */

/**
 *  @page libshare_pref Configuration Settings
 *
 *
 */

/**
 * @page libshare_fs Accessing the share-fs filesystem.
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
 * @page libshare_net Networking
 * Goto: @ref libshare_net "The libshare networking reference manual."
 *
 * The libshare networking layer provides extended support to existing network protocol socket management in addition to providing access to the ESTP protocol. 
 *
 * The libshare networking layer stores additional information about sockets in order to provide additional identification and features such as buffered input.
 * @see shnet_t
 *
 * The ESTP protocol requires compiling a kernel (OS) module. The module provides a new internet IPv4 protocol named "IPPROTO_ESTP". An RFC is included in the source code's documentation directory for additional details. The kernel module may be used independently of the libshare library. 
 *
 */
/**
 *  @page libshare_netestp Encoded Stream Transfer Protocol
 *
 *
 */
 
/**
 *  @page libshare_mem Memory Buffer and Encoding
 *
 *
 *  The memory buffer and encoding section of the libshare library provides method to manage dynamic sized, file memory-mapped, and encoding memory segments.
 *
 *   The section can be broken down into the following groups:
 *
 *    - @subpage libshare_memmeta "Meta definition hashmaps."
 *    <dl>Store and retrieve information from hash-maps.</dl>
 
 *    - @subpage libshare_mempool "Memory buffer pools."
 *    <dl>Manage a set of memory buffers.</dl>
 *
 *    - @subpage libshare_membuf "Dynamic memory buffers."
 *    <dl>Dynamic memory management for memory buffers.</dl>
 *
 *    - @subpage libshare_memkey "Hash/Digest Keys"
 *    <dl>Generate and print 192-bit encoded keys referencing data segments.</dl>
 */

/**
 *  @page libshare_memmeta Meta Definition Hashmaps
 *
 *    Metadef hashmaps are used by the Share Librarys "sharefs" and networking modules.
 *
 *    Metadef hashmaps are used by the Share Library libshare_fs "sharefs" and libshare_net "networking" modules.
 *
 *
 *  References:
 *    - \ref libshare_fs "The libshare file-system programming api."
 *    - \ref libshare_net "The libshare network programming api."
 */
 
/**
 *  @page libshare_mempool Memory Buffer Pools
 */
/**
 *  @page libshare_membuf Dynamic Memory Buffers
 */
/**
 *  @page libshare_memkey Hash/Digest Keys
 *
 */

/**
 *  @page libshare_sys System-level Process Management
 *
 *  The libshare sytem-level process management section provides:
 *  - Access to control process-level control such as threads and mutexes.
 *  - Account Permission Access Management (PAM) identity management and verification.
 *
 */

#endif /* ndef __SHARE_H__ */


