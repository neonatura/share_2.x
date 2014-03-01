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
  pid_t *pid_p;
  shfs_t *tree;
  shfs_ino_t *root;
  shfs_ino_t *ent;
  shmeta_t *h;
  shmeta_value_t *val;
  shmeta_value_t new_val;
  shkey_t *key;
  char buf[256];
  int err;

  if (!runtime_mode) {
    memset(buf, 0, sizeof(buf));
    runtime_mode = buf;
  }

  process_path = shfs_app_name(process_path);

  tree = shfs_init(NULL);
/*
  root = shfs_inode(NULL, NULL, SHINODE_PARTITION);
*/

  ent = shfs_inode(tree->base_ino, process_path, SHINODE_APP);
  if (runtime_mode)
    ent = shfs_inode(ent, runtime_mode, 0);

  err = shfs_meta(tree, ent, &h); 
  if (err) {
    printf ("shss_meta[%s]: %s\n", process_path, strerror(errno));
    return (err);
  }

  key = shkey_str("shfs_proc");

  cur_pid = 0;
  pid_p = (pid_t *)shmeta_get_void(h, key); fprintf(stderr, "DEBUG: /2 %d = shmeta_get_void(%x)\n", (unsigned int)cur_pid, key);
  if (pid_p)
    cur_pid = *pid_p;
fprintf(stderr, "DEBUG: %u = shmeta_get_void(%x)\n", (unsigned int)cur_pid, key);
  if (cur_pid) {
    if (kill(cur_pid, 0) != 0) {
      sprintf(buf, "shfs_proc_lock [signal verify (pid %d)]", (unsigned int)cur_pid);
      PRINT_ERROR(-errno, buf); 
      cur_pid = 0;
    }
  }
  if (cur_pid && cur_pid != pid) {
    /* lock is not available. */
    sprintf(buf, "process #%u is running.. lock not available for #%u.\n", (unsigned int)cur_pid, (unsigned int)pid);
    PRINT_ERROR(SHERR_ADDRINUSE, buf);
    shmeta_free(&h);
    return (SHERR_ADDRINUSE);
  }
  shmeta_set_void(h, key, &pid, sizeof(pid)); 
fprintf(stderr, "DEBUG: shmeta_set_void(%x, %u)\n", key, (unsigned int)pid);

  shfs_meta_save(tree, ent, h);
  shmeta_free(&h);

  sprintf(buf, "process #%d is running.. set lock meta definition.\n", pid);
  PRINT_RUSAGE(buf);

  return (0);
}

