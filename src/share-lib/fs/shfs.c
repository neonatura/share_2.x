/*
 *  Copyright 2013 Brian Burrell 
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

shpeer_t *shapp_init(char *exec_path, char *host, int flags)
{
  shpeer_t *peer;
  char *app_name;
  char hostbuf[MAXHOSTNAMELEN+1];
  char ebuf[1024];

  app_name = shfs_app_name(exec_path);
  peer = shpeer_init(app_name, NULL, 0);
  if (!peer)
    return (NULL);
  shpeer_set_default(peer);

  if (!(flags & PEERF_LOCAL)) {
    if (!host) {
      memset(hostbuf, 0, sizeof(hostbuf));
      gethostname(hostbuf, sizeof(hostbuf) - 1);
      if (gethostbyname(hostbuf) != NULL)
        host = hostbuf;
    }

    shpeer_t *priv_peer = shpeer_init(app_name, host, PEERF_PRIVATE); 
    shapp_register(priv_peer);
    shpeer_free(&priv_peer);
  }

  sprintf(ebuf, "initialized '%s' as peer %s", exec_path, shpeer_print(peer));
  shinfo(ebuf);

  return (peer);
}

int shapp_register(shpeer_t *peer)
{
  shbuf_t *buff;
  char data[256];
  size_t data_len;
  uint32_t mode;
  int qid;
  int err;

  if (!peer)
    peer = ashpeer();

  /* open message queue to share daemon */
  qid = shmsgget(NULL);
  if (qid < 0)
    return (qid);

  /* send a 'peer transaction' operation request. */
  mode = TX_APP;
  buff = shbuf_init();
  shbuf_cat(buff, &mode, sizeof(mode));
  shbuf_cat(buff, peer, sizeof(shpeer_t));
  err = shmsg_write(qid, buff, NULL);
  shbuf_free(&buff);
  if (err)
    return (err);

  /* close message queue */
  shmsgctl(qid, SHMSGF_RMID, TRUE);

  return (0);
}

char *shfs_app_name(char *app_name)
{
  char *ptr;

  if (!app_name)
    return (NULL);

  ptr = strrchr(app_name, '/'); 
  if (ptr)
    app_name = ptr + 1;

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
  sprintf(path, "/app/%s", app_name);

  peer = shpeer();
  fs = shfs_init(peer);
  file = shfs_file_find(fs, path);
  err = shfs_sig_verify(file, &peer->name);
  if (err) {
    PRINT_ERROR(err, "shfs_sig_verify");
    return (err);
  }

  return (0);
}

