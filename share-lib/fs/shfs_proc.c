/*
 *  Copyright 2013 Brian Burrell 
 *
 *  This file is part of the Share Library.
 *  (https://github.com/briburrell/share)
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

#include "../share.h"

int shfs_proc_lock(char *process_path, char *runtime_mode)
{
  pid_t pid = getpid();
  shfs_tree *tree;
  shfs_node *root;
  shfs_node *ent;
  shfs_def *def;
  char buf[256];
  int err;

  if (!runtime_mode) {
    memset(buf, 0, sizeof(buf));
    runtime_mode = buf;
  }

  process_path = shfs_app_name(process_path);

  tree = shfs_init(process_path, SHFS_OVERLAY | SHFS_TRACK);
  root = shfs_node_entry(NULL, NULL, SHINO_PARTITION);
  ent = shfs_node_entry(root, process_path, SHINO_APP_ID);
  ent = shfs_node_entry(ent, runtime_mode, 0);

  err = shfs_meta(ent, &def); 
  if (err) {
    printf ("shss_meta[%s]: %s\n", process_path, strerror(errno));
    return (err);
  }

  if (def->pid) {
    if (kill(def->pid, 0) != 0) {
      def->pid = 0;
      printf ("Process #%d is not running.. cleared lock.\n", def->pid);
    }
  } else if (!def->stamp) {
    printf ("Process #%d has ran for the first time.\n", pid);
  }

  if (def->pid != 0 && def->pid != pid) {
    /* lock is not available. */
    printf ("Process #%d is running.. lock not available.\n", def->pid);
    return (-EAGAIN);
  }

  def->stamp = shfs_time64();
  def->pid = pid;
  shfs_meta_save(ent, def);
  //shfs_meta_free(&def);

  printf ("Process #%d is running.. set lock meta definition.\n", pid);

  return (0);
}

