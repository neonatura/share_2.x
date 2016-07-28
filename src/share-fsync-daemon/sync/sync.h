
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

#ifndef __SYNC__SCAN_H__
#define __SYNC__SCAN_H__



/** 
 * A special type indicating no file-system is being monitored.
 */
#define FS_NONE 0
/** 
 * A directory is being monitored on a share-fs parition.
 */
#define FS_SHARE 1
/** 
 * A directory is being monitored hosted on a linux OS.
 */
#define FS_LINUX 2
#define MAX_SYNC_FS 3

/** 
 * A flag indicating a file-system inode entity is being monitored.
 */
#define FSENT_ACTIVE (1 << 0)


/** A file-system specific sync operation. */
typedef int (*sync_f)(void *, void *);
#define SYNCF(_f) (sync_f)(_f)

typedef struct sync_op_t
{
  sync_f init;
  sync_f term;
  sync_f watch;
  sync_f poll;
  sync_f remove;
  sync_f read;
  sync_f write;
} sync_op_t;

/**
 * An entity that represents a file-system inode.
 */
typedef struct sync_ent_t
{
  /** The current state of the inode entity. */
  int flags;
  /** A unique reference number for this inode entity. */
  int id;
  /** An absolute path to the inode being watched. */
  char path[PATH_MAX+1];

  struct sync_ent_t *next; 
} sync_ent_t;

/**
 * A file-system directory hierarchy.
 */
typedef struct sync_t 
{
  /** The file-system type being monitored. */
  int sync_type;
  int sync_fd;
  /** The root path of the hierarchy being monitored. */
  char sync_path[PATH_MAX+1];

  /** File-system specific operations. */
  sync_op_t *op;

  /** A list of associated inode entities. */
  struct sync_ent_t *ent_list;
} sync_t;


extern sync_op_t sync_op_table[MAX_SYNC_FS];


/**
 * Monitor a directory hierarchy on a file-system.
 */
int sync_init(sync_t *sync, int fs_type, const char *path);

/**
 * Watch a directory in the sync hierarchy.
 */
int sync_watch(sync_t *sync, const char *in_path);

/**
 * Stop monitoring a directory in the sync hierarchy.
 */
void sync_remove(sync_t *sync, sync_ent_t *ent);

/**
 * Process pending sync operations.
 */
int sync_poll(sync_t *sync, double to);

/**
 * De-allocate resources used by sync operations.
 */
int sync_term(sync_t *sync);

/**
 * Obtain a sync entity by it's id number.
 */
sync_ent_t *sync_ent_id(sync_t *sync, int ent_id);

/**
 * Obtain a sync entity by it's absolute path reference.
 */
sync_ent_t *sync_ent_path(sync_t *sync, const char *path);

/**
 * Allocate resources associated with a new sync entity.
 */
sync_ent_t *sync_ent_init(sync_t *sync, int id, const char *path);

#include "sync_share.h"
#include "sync_linux.h"


#endif /* ndef __SYNC__SCAN_H__ */



