
/*
 *  Copyright 2014 Neo Natura
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
 */  

#ifndef __SHARETOOL_H__
#define __SHARETOOL_H__

#undef __STRICT_ANSI__ 

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

#include "share.h"
#include "sharetool_pref.h"
#include "sharetool_file.h"
#include "sharetool_rev.h"
#include "file/file_cat.h"
#include "file/file_import.h"
#include "file/file_list.h"
#include "file/file_mkdir.h"
#include "file/file_unlink.h"
#include "file/file_attr.h"
#include "file/file_rev.h"


#define SHM_NONE 0
#define SHM_PREF 1
#define SHM_FILE_LIST 10
#define SHM_FILE_CAT 12
#define SHM_FILE_COPY 14
#define SHM_FILE_MKDIR 16
#define SHM_FILE_REMOVE 18
#define SHM_FILE_ATTR 20
#define SHM_FILE_DIFF 22
#define SHM_FILE_MERGE 24
#define SHM_FILE_PATCH 26
#define SHM_FILE_REV 28

#define PFLAG_VERBOSE (1 << 0)
#define PFLAG_SYNTAX (1 << 1)
#define PFLAG_VERSION (1 << 2)
#define PFLAG_CHECKSUM (1 << 3)
#define PFLAG_INODE (1 << 4)
#define PFLAG_LOCAL (1 << 5)
#define PFLAG_RECURSIVE (1 << 6)

/* psuedo-standard ports for shnet operations */
#define SHARE_PING_PORT 32200

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE !FALSE
#endif

#ifndef MIN
#define MIN(a,b) \
  (a < b ? a : b)
#endif

#ifndef MAX
#define MAX(a,b) \
  (a > b ? a : b)
#endif

extern char process_path[PATH_MAX + 1];
extern char process_file_path[PATH_MAX + 1];
extern char process_socket_host[PATH_MAX + 1];
extern unsigned int process_socket_port;
extern FILE *sharetool_fout;


#endif /* __SHARETOOL_H__ */

