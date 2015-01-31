

/*
 * @copyright
 *
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
 *
 *  @endcopyright
 *
 *  @file pub_scan.c
 */

#include "pub_server.h"

void pubd_scan_path(pubuser_t *u, char *path)
{
  struct stat st;
  int err;

  err = stat(path, &st);
  if (err) {
    u->err = errno;
    return;
  }
  
  if (S_ISDIR(st.st_mode)) {
    pubd_scan_dir(u, path);
    return;
  }

  pubd_file_verify(u, path);
}

void pubd_scan_dir(pubuser_t *u, char *dir_path)
{
  DIR *dir;
  struct dirent *ent;
  char path[PATH_MAX+1];

  dir = opendir(dir_path);
  if (!dir) {
    u->err = errno;
    return;
  }

  while ((ent = readdir(dir))) {
    if (0 == strcmp(ent->d_name, ".") ||
        0 == strcmp(ent->d_name, ".."))
      continue;
    sprintf(path, "%s/%s", u->root_path, ent->d_name);
    pubd_scan_path(u, path);
  }

  closedir(dir);

}

void pubd_scan(void)
{
  pubuser_t *u;
  DIR *dir;
  struct dirent *ent;

  for (u = _pubd_users; u; u = u->next) {
    pubd_scan_dir(u, u->root_path);
  }

}

void pubd_scan_init(void)
{
  struct passwd raw_pw, *pw;
  struct stat st;
  char path[PATH_MAX+1];
  char buf[4096];
char uname[4096];
  struct spwd *spwd; 
  uid_t uid;
  int err;

fprintf(stderr, "DEBUG: pubd_scan_init/start");

  MAX_PUBUSER_NAME_LENGTH = sysconf(_SC_LOGIN_NAME_MAX);
fprintf(stderr, "%d = sysconf(_SC_LOGIN_NAME_MAX)\n", MAX_PUBUSER_NAME_LENGTH);
  if (MAX_PUBUSER_NAME_LENGTH == -1)
    MAX_PUBUSER_NAME_LENGTH = 256;
  MAX_PUBUSER_NAME_LENGTH = MIN(256, MAX_PUBUSER_NAME_LENGTH);


 // setpwent();

memset(&raw_pw, 0, sizeof(raw_pw));
memset(buf, 0, sizeof(buf));
  while (0 == getpwent_r(&raw_pw, buf, sizeof(buf), &pw)) {
    sprintf(path, "%s/%s", pw->pw_dir, PUB_SYNC_PATH);
    err = stat(path, &st);
    if (err)
      continue;
    if (!S_ISDIR(st.st_mode))
      continue;

fprintf(stderr, "DEBUG: found %s's md5 pass '%s'\n", pw->pw_name, pw->pw_passwd);

    pubd_user_add(pw->pw_uid, pw->pw_name, pw->pw_passwd, path);
  }
//  endpwent();

fprintf(stderr, "DEBUG: pubd_scan_init/end\n");
}

void pubd_scan_free(void)
{
  pubuser_t *u;
  pubuser_t *u_next;

  for (u = _pubd_users; u; u = u_next) {
    u_next = u->next;
    free(u);
  }
  _pubd_users = NULL;

}

