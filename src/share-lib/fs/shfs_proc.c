
/*
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
*/  

#include "share.h"


char *shfs_app_name(char *app_name)
{
  char *ptr;

  if (!app_name)
    return (PACKAGE);

  ptr = strrchr(app_name, '/'); 
  if (ptr)
    app_name = ptr + 1;

  if (0 == strncmp(app_name, "lt-", 3))
    app_name += 3;

  return (app_name);
}

_TEST(shfs_app_name)
{
  char buf[256];
  char *path;
  
  strcpy(buf, "a/a/a/a/a/a/a/a/a/a/a/a/a/a"); 
  _TRUEPTR(path = shfs_app_name(buf));
  if (!path)
    return;
  _TRUE(0 == strcmp(path, "a"));
}

#if 0
int shfs_app_certify(char *exec_path)
{
  shfs_t *fs;
  SHFL *file;
  shpeer_t *peer;
  struct stat st;
  char path[PATH_MAX+1];
  char *app_name;
  int err;

  err = stat(exec_path, &st);
  if (!err && S_ISDIR(st.st_mode)) {
    PRINT_ERROR(SHERR_ISDIR, exec_path);
    return (SHERR_ISDIR);
  }
  if (err) {
    err = -errno;
    PRINT_ERROR(err, exec_path);
    return (err);
  }

  app_name = shfs_app_name(exec_path);
  sprintf(path, "/app/%s/exec", app_name);

  peer = shpeer();
  fs = shfs_init(peer);
  file = shfs_file_find(fs, path);
  err = shfs_sig_verify(file, shpeer_kpub(peer));
  if (err) {
    PRINT_ERROR(err, "shfs_sig_verify");
    return (err);
  }

  return (0);
}
#endif

int shfs_proc_lock(char *process_path, char *runtime_mode)
{
  pid_t pid = getpid();
  pid_t cur_pid;
  pid_t *pid_p;
  shfs_t *tree;
  shfs_ino_t *root;
  shfs_ino_t *ent;
  shmap_t *h;
  shmap_value_t *val;
  shmap_value_t new_val;
  char buf[256];
  int err;

  if (!runtime_mode) {
    memset(buf, 0, sizeof(buf));
    runtime_mode = buf;
  }

  process_path = shfs_app_name(process_path);

  tree = shfs_init(NULL);

  ent = shfs_inode(tree->base_ino, process_path, SHINODE_APP);
  if (runtime_mode)
    ent = shfs_inode(ent, runtime_mode, 0);

  err = shfs_meta(tree, ent, &h); 
  if (err) {
    return (err);
  }

  cur_pid = 0;
  pid_p = (pid_t *)shmap_get_void(h, ashkey_str("shfs_proc"));
  if (pid_p)
    cur_pid = *pid_p;
  if (cur_pid) {
    if (kill(cur_pid, 0) != 0) {
      int err = -errno;
      if (err != SHERR_SRCH) {
        sprintf(buf, "shfs_proc_lock [signal verify (pid %d)]", (unsigned int)cur_pid);
        PRINT_ERROR(-errno, buf); 
      }
      cur_pid = 0;
    }
  }
  if (cur_pid && cur_pid != pid) {
    /* lock is not available. */
    shmap_free(&h);
    return (SHERR_ADDRINUSE);
  }
  shmap_set_void(h, ashkey_str("shfs_proc"), &pid, sizeof(pid)); 

  shfs_meta_save(tree, ent, h);
  shmap_free(&h);

  shfs_free(&tree);

  return (0);
}

_TEST(shfs_proc_lock)
{
  _TRUE(0 == shfs_proc_lock("test", NULL));
}

