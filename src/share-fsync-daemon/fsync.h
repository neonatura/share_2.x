
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

#ifndef __FSYNC_H__
#define __FSYNC_H__

#include <sys/types.h>
#include <unistd.h>
#include <utime.h>
#include <dirent.h>
#include <signal.h> 
#include <pwd.h>
#include <shadow.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <share.h>

#include "sync/sync.h"

typedef struct fuser_t
{
  char name[256];
  char pass[256];
  char root_path[PATH_MAX+1];
  int err;

  /* shfs */
  shfs_t *fs;
  shkey_t id;

  /* directory hierarchy monitor (local) */
  sync_t lcl_sync;

  /* directory hierarchy monitor (remote) */
  sync_t rem_sync;

  struct pubuser_t *next;
} fuser_t;


#include "fsync_cycle.h"
#include "fsync_pref.h"
#include "fsync_user.h"
#include "fsync_server.h"

#endif /* ndef __FSYNC_USER_H__ */

