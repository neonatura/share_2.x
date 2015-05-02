
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

#include "share.h"
#include "sharetool.h"


#define PMODE_NONE 0
#define PMODE_PREFIX 1
#define PMODE_GROUP 2
#define PMODE_PASS 3
#define PMODE_HOST 4
#define PMODE_PORT 5
#define PMODE_PATH 6



shfs_ino_t *sharetool_file(char *path, shfs_t **fs_p)
{
  shfs_t *fs;
  shfs_ino_t *dir;
  shfs_ino_t *file;
  shpeer_t *peer;
  char p_prefix[PATH_MAX+1];
  char p_group[PATH_MAX+1];
  char p_pass[PATH_MAX+1];
  char p_host[PATH_MAX+1];
  char p_dir[PATH_MAX+1];
  char p_path[PATH_MAX+1];
  char f_path[PATH_MAX+1];
  char *peer_name;
  char *peer_host;
  char *cptr;
  char *ptr;
  int p_port;
  int pmode;
  int idx;
  int err;

  memset(p_prefix, 0, sizeof(p_prefix));
  memset(p_group, 0, sizeof(p_group));
  memset(p_pass, 0, sizeof(p_pass));
  memset(p_host, 0, sizeof(p_host));
  memset(p_dir, 0, sizeof(p_dir));
  memset(p_path, 0, sizeof(p_path));
  p_port = 0;

#ifdef PACKAGE
  strncpy(p_prefix, PACKAGE, sizeof(p_prefix) - 1);
#endif

  if (0 == strncmp(path, "home:", 5)) {
    shkey_t *id_key;

    id_key = shpam_ident_gen(shpam_uid(get_libshare_account_name()), ashpeer());
    fs = shfs_home_fs(id_key);
    shkey_free(&id_key);

    *fs_p = fs;
    return (shfs_home_file(fs, path + 5));
  }
  
  if (!strchr(path, '/') || 0 == strncmp(path, "./", 2)) {
    if (0 == strncmp(path, "./", 2))
      path += 2;
    strcpy(p_prefix, "file");
    getcwd(p_dir, sizeof(p_dir) - 1);
    strncpy(p_path, path, sizeof(p_path) - 1);
  } else {
    pmode = PMODE_NONE;
    ptr = path;
    while (*ptr) {
      idx = strcspn(ptr, ":/@");
      cptr = ptr;
      ptr += idx;

      if (pmode == PMODE_NONE) {
        if (0 == strncmp(ptr, ":/", 2)) {
          pmode = PMODE_GROUP;
          memset(p_prefix, 0, sizeof(p_prefix));
          strncpy(p_prefix, cptr, idx);
          ptr += 2;
        } else {
          pmode = PMODE_PATH;
        }
      } else if (pmode == PMODE_GROUP) {
        if (*ptr == ':') {
          pmode = PMODE_PASS;
          ptr++;
        } else if (*ptr == '@') {
          pmode = PMODE_HOST;
          ptr++;
        } else {
          pmode = PMODE_PATH;
        }
        strncpy(p_group, cptr, idx);
      } else if (pmode == PMODE_PASS) {
        if (*ptr == '@') {
          pmode = PMODE_HOST;
          ptr++;
        } else {
          pmode = PMODE_PATH;
        }
        strncpy(p_pass, cptr, idx);
      } else if (pmode == PMODE_HOST) {
        if (*ptr == ':') {
          pmode = PMODE_PORT;
          ptr++;
        } else {
          pmode = PMODE_PATH;
        }
        strncpy(p_host, cptr, idx);
      } else if (pmode == PMODE_PORT) {
        pmode = PMODE_PATH;
        p_port = atoi(cptr);
      } else if (pmode == PMODE_PATH) {
        strncpy(p_dir, cptr, sizeof(p_dir) - 1);

        if (*p_dir && p_dir[strlen(p_dir)-1] != '/') {
          ptr = strrchr(p_dir, '/');
          if (ptr) {
            *ptr++ = '\0';
            strncpy(p_path, ptr, sizeof(p_path) - 1); 
          }
        }
        break;
      }

    }
  }

  sprintf(f_path, "%s/%s", p_dir, p_path);

  peer_name = NULL;
  if (*p_prefix) {
    peer_name = p_prefix;
    if (*p_pass) {
      strcat(peer_name, ":");
      strcat(peer_name, p_pass);
    }
  }
  peer_host = NULL;
  if (*p_host) {
    peer_host = p_host; 
    if (p_port)
      sprintf(peer_host+strlen(peer_host), ":%d", p_port);
  }
  peer = shpeer_init(peer_name, peer_host);
  fs = shfs_init(peer);
  shpeer_free(&peer);

  if (fs_p)
    *fs_p = fs;

  if (!*p_path) {
    /* no file specified. */
    dir = shfs_dir_find(fs, p_dir);
    return (dir);
  }

  /* regular shfs file */
  file = shfs_file_find(fs, f_path);

  if (0 == strcmp(p_prefix, "file")) {
    /* set link to local-disk path. */
    err = shfs_ext_set(file, f_path);
    if (err) {
      fprintf(stderr, "%s: cannot access %s: %s.\n", 
          process_path, f_path, sherrstr(err)); 
      return (NULL);
    }

    err = shfs_inode_write_entity(file);
    if (err)
      return (err);
  }

  return (file);
}


