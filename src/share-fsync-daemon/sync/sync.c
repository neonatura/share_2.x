
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

#include "fsync.h"


sync_op_t sync_op_table[MAX_SYNC_FS] = {
  { }, /* FS_NONE */

  { SYNCF(share_sync_init), SYNCF(share_sync_term), SYNCF(share_sync_watch), SYNCF(share_sync_poll), SYNCF(share_sync_remove), SYNCF(share_sync_read), SYNCF(share_sync_write) },  /* FS_SHARE */

#ifdef linux
  { SYNCF(linux_sync_init), SYNCF(linux_sync_term), SYNCF(linux_sync_watch), SYNCF(linux_sync_poll), SYNCF(linux_sync_remove), SYNCF(linux_sync_read), SYNCF(linux_sync_write) },  /* FS_LINUX */
#else
  { }, /* FS_LINUX n/a */
#endif
};

sync_ent_t *sync_ent_id(sync_t *sync, int ent_id)
{
  sync_ent_t *ent;

  for (ent = sync->ent_list; ent; ent = ent->next) {
    if (!(ent->flags & FSENT_ACTIVE))
      continue;
    if (ent->id == ent_id)
      return (ent);
  }

  return (NULL);
}

sync_ent_t *sync_ent_path(sync_t *sync, const char *path)
{
  sync_ent_t *ent;

  for (ent = sync->ent_list; ent; ent = ent->next) {
    if (0 == strcmp(ent->path, path))
      return (ent);
  }

  return (NULL);
}

sync_ent_t *sync_ent_init(sync_t *sync, int id, const char *path)
{
  sync_ent_t *ent;

/* TODO: check for non-active unused */

  /* allocate entity */
  ent = (sync_ent_t *)calloc(1, sizeof(sync_ent_t));
  if (!ent)
    return (NULL);

  /* set attributes */
  strcpy(ent->path, path);
  ent->id = id;
  ent->flags |= FSENT_ACTIVE;

  /* add to entity list */
  ent->next = sync->ent_list;
  sync->ent_list = ent;

  return (ent);
}


int sync_watch(sync_t *sync, const char *in_path)
{
  sync_ent_t *ent;
  char path[PATH_MAX+1];
  int err;
  int id;

  memset(path, 0, sizeof(path));
  strncpy(path, in_path, sizeof(path)-1);

  if (strlen(path) && path[strlen(path)-1] == '/')
    path[strlen(path)-1] = '\000';

  if (!sync->op->watch)
    return (SHERR_OPNOTSUPP);

  /* register path */
  id = sync->op->watch(sync, path);
  if (id < 0)
    return (err);

  /* allocate entity */
  ent = sync_ent_init(sync, id, path);
  if (!ent)
    return (SHERR_NOMEM);

  return (0);
} 

int sync_init(sync_t *sync, int fs_type, const char *path)
{
  int err;

  if (fs_type <= 0 || fs_type >= MAX_SYNC_FS)
    return (SHERR_INVAL);

  memset(sync, 0, sizeof(sync_t));
  sync->sync_type = fs_type;
  sync->op = &sync_op_table[fs_type];
  strncpy(sync->sync_path, path, sizeof(sync->sync_path)-1);
 
  if (sync->op->init) {
    err = sync->op->init(sync, NULL);
    if (err) {
      sync_term(sync);
      return (err);
    }
  }

#if 0
  err = sync_watch(sync, path);
  if (err) {
    sync_term(sync);
    return (err);
  }
#endif
fprintf(stderr, "DEBUG: sync_init: path '%s'\n", sync->sync_path);  

  return (0);
}


void sync_remove(sync_t *sync, sync_ent_t *ent)
{
  int err;

  if (sync->op->remove) {
    sync->op->remove(sync, ent);
  }

  ent->id = 0;
  ent->flags &= ~FSENT_ACTIVE;
}

int sync_poll(sync_t *sync, double to)
{
  int err;

  if (sync->op->poll) {
    err = sync->op->poll(sync, &to);
    if (err)
      return (err);
  }

  return (0);
}

int sync_term(sync_t *sync)
{
  sync_ent_t *ent_next;
  sync_ent_t *ent;

  if (sync->op->term)
    sync->op->term(sync, NULL);

  for (ent = sync->ent_list; ent; ent = ent_next) {
    ent_next = ent->next;
    free(ent);
  }
  sync->ent_list = NULL;

  sync->sync_fd = 0;
}


