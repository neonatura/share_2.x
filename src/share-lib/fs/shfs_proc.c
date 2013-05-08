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

#include "share.h"

int shfs_proc_lock(char *process_path, char *runtime_mode)
{
  pid_t pid = getpid();
  pid_t cur_pid;
  shfs_t *tree;
  shfs_ino_t *root;
  shfs_ino_t *ent;
  shmeta_t *h;
  shmeta_value_t *val;
  shkey_t *key;
  char buf[256];
  int err;

  if (!runtime_mode) {
    memset(buf, 0, sizeof(buf));
    runtime_mode = buf;
  }

  process_path = shfs_app_name(process_path);

  tree = shfs_init(NULL);
  root = shfs_inode(NULL, NULL, SHINODE_PARTITION);
  ent = shfs_inode(root, process_path, SHINODE_APP);
  ent = shfs_inode(ent, runtime_mode, 0);

  err = shfs_meta(tree, ent, &h); 
  if (err) {
    printf ("shss_meta[%s]: %s\n", process_path, strerror(errno));
    return (err);
  }

  key = shkey_str("shfs_proc");
  val = shmeta_get(h, key);
  memcpy(&cur_pid, &val->name, sizeof(pid));
  if (cur_pid) {
    if (kill(cur_pid, 0) != 0) {
      memset(&val->name, 0, sizeof(val->name));
      printf ("Process #%d is not running.. cleared lock.\n", val->name.code[0]);
    }
  } else if (!val->stamp) {
    printf ("Process #%d has ran for the first time.\n", pid);
  }

  if (shkey_is_blank(&val->name) && cur_pid != pid) {
    /* lock is not available. */
    printf ("Process #%d is running.. lock not available.\n", (int)cur_pid);
    return (-EAGAIN);
  }

  val->stamp = shtime64();
  memcpy(&val->name, &pid, sizeof(pid));
  shfs_meta_save(tree, ent, h);
  shmeta_free(&h);

  printf ("Process #%d is running.. set lock meta definition.\n", pid);

  return (0);
}

