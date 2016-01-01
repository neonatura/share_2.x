
/*
 *  Copyright 2015 Neo Natura 
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



shdb_t *shnet_track_db(void)
{
  shdb_t *db;

  db = shdb_open(NET_DB_NAME);
  if (!db)
    return (NULL);

  if (0 == shdb_table_new(db, TRACK_TABLE_NAME)) {
    shdb_col_new(db, TRACK_TABLE_NAME, "host");
    shdb_col_new(db, TRACK_TABLE_NAME, "label");
    shdb_col_new(db, TRACK_TABLE_NAME, "key");
    shdb_col_new(db, TRACK_TABLE_NAME, "trust");
    shdb_col_new(db, TRACK_TABLE_NAME, "ctime"); /* creation */
    shdb_col_new(db, TRACK_TABLE_NAME, "mtime"); /* last attempt */
    shdb_col_new(db, TRACK_TABLE_NAME, "ltime"); /* last connect */
    shdb_col_new(db, TRACK_TABLE_NAME, "cert");
  }

  return (db);
}

/**
 * Adds a new peer entity to the track db
 */
int shnet_track_add(shpeer_t *peer)
{
  shdb_t *db;
  char hostname[MAXHOSTNAMELEN+1];
  char id_str[512];
  char buf[512];
  shdb_idx_t rowid;
  int port;
  int err;

  db = shnet_track_db();
  if (!db)
    return (SHERR_IO);

  shpeer_host(peer, hostname, &port);
  sprintf(id_str, "%s %d", hostname, port);
  err = shdb_row_find(db, TRACK_TABLE_NAME, &rowid, "host", id_str, 0);
  if (!err) {
    /* record already exists -- all done */
    shdb_close(db);
    return (0);
  }

  err = shdb_row_new(db, TRACK_TABLE_NAME, &rowid);
  if (err)
    goto done;

  err = shdb_row_set(db, TRACK_TABLE_NAME, rowid, "host", id_str);
  if (err)
    goto done;

  err = shdb_row_set_time(db, TRACK_TABLE_NAME, rowid, "ctime");
  if (err)
    goto done;

#if 0
  err = shdb_row_set(db, TRACK_TABLE_NAME, rowid, "trust", "0");
  if (err)
    goto done;
#endif

  err = shdb_row_set(db, TRACK_TABLE_NAME, rowid,
      "label", shpeer_get_app(peer));
  if (err)
    goto done;


#if 0
  strcpy(buf, shkey_print(shpeer_kpriv(peer)));
  err = shdb_row_set(db, TRACK_TABLE_NAME, rowid, "key", buf);
  if (err)
    goto done;
#endif

#if 0
  err = shdb_row_set_time(db, TRACK_TABLE_NAME, rowid, "mtime");
  if (err)
    goto done;
#endif

done:
  shdb_close(db);
  return (err);
}

int shnet_track_remove(shpeer_t *peer)
{
  shdb_t *db;
  char hostname[MAXHOSTNAMELEN+1];
  char id_str[512];
  char buf[512];
  shdb_idx_t rowid;
  int port;
  int err;

  if (!peer)
    return (0); /* all done */

  db = shnet_track_db();
  if (!db)
    return (SHERR_IO);

  shpeer_host(peer, hostname, &port);
  sprintf(id_str, "%s %d", hostname, port);
  err = shdb_row_find(db, TRACK_TABLE_NAME, &rowid, "host", id_str, 0);
  if (err) {
    shdb_close(db);
    return (err);
  }

  err = shdb_row_delete(db, TRACK_TABLE_NAME, rowid);
  if (err) {
    shdb_close(db);
    return (err);
  }

  shdb_close(db);
  return (0);
}

/**
 * @returns TRUE or FALSE whether record is active.
 */
int shnet_track_fresh(time_t ctime, int cond)
{
  double diff;
  double deg;

  if (cond >= 0)
    return (TRUE);

  diff = MAX(0, time(NULL) - ctime);
  deg = (diff / 2592000) * fabs(cond);
  if (deg < 1.0)
    return (TRUE);

  return (FALSE);
}


/**
 * Marks a network adderss in a positive or negative manner.
 * @param cond a negative or positive number indicating connection failure or success.
 * @note a positive condition updates the 'ltime' db col
 */
int shnet_track_mark(shpeer_t *peer, int cond)
{
  shdb_t *db;
  char hostname[MAXHOSTNAMELEN+1];
  char id_str[512];
  char buf[128];
  char *str;
  uint64_t rowid;
  long trust;
  int port;
  int err;

  db = shnet_track_db();
  if (!db)
    return (SHERR_IO);

  shpeer_host(peer, hostname, &port);
  sprintf(id_str, "%s %d", hostname, port);
  err = shdb_row_find(db, TRACK_TABLE_NAME, &rowid, "host", id_str, 0);
  if (err)
    goto done;

  str = shdb_row_value(db, TRACK_TABLE_NAME, rowid, "trust");
  if (!str)
    str = strdup("");

  trust = (long)atoll(str);
  free(str);

  if (cond < 0) {
    if (cond < -100) cond = -256;
  } else if (cond > 0) {
    if (cond > 100) cond = 256;
  }
  trust += cond;

  if (cond < 0) {
    time_t ctime = shdb_row_time(db, TRACK_TABLE_NAME, rowid, "ctime");
    if (!shnet_track_fresh(ctime, cond)) {
      err = shdb_row_delete(db, TRACK_TABLE_NAME, rowid);
      goto done;
    }
  }

  sprintf(buf, "%ld", trust);
  err = shdb_row_set(db, TRACK_TABLE_NAME, rowid, "trust", buf);
  if (err)
    goto done;

  err = shdb_row_set_time(db, TRACK_TABLE_NAME, rowid, "mtime");
  if (err)
    goto done;

  if (cond > 0) {
    err = shdb_row_set_time(db, TRACK_TABLE_NAME, rowid, "ltime");
    if (err)
      goto done;
  }

  err = 0;

done:
  shdb_close(db);
  return (err);
}

int shnet_track_incr(shpeer_t *peer)
{
  return (shnet_track_mark(peer, 1));
}
int shnet_track_decr(shpeer_t *peer)
{
  return (shnet_track_mark(peer, -1));
}

/**
 * Obtains a remote connection for the same service associated with the peer specified.
 * @param the peer to search for a matching service.
 * @param speer_p a reference to the scanned peer.
 */
int shnet_track_scan(shpeer_t *peer, shpeer_t **speer_p)
{
  shdb_t *db;
  shpeer_t *speer;
  char sql_str[512];
  char app_name[MAX_SHARE_NAME_LENGTH];
  char *ret_val;
  int err;

  if (!speer_p)
    return (SHERR_INVAL);

  db = shnet_track_db();
  if (!db)
    return (SHERR_IO);

  memset(app_name, 0, sizeof(app_name));
  strncpy(app_name, shpeer_get_app(peer), sizeof(app_name)-1);

  ret_val = NULL;
  sprintf(sql_str, "select host from %s where label = '%s' order by mtime limit 1", TRACK_TABLE_NAME, app_name);
  err = shdb_exec_cb(db, sql_str, shdb_col_value_cb, &ret_val);
  shdb_close(db);
  if (err) {
    PRINT_ERROR(err, shdb_exec_cb);
    return (err);
  }

  if (!ret_val)
    return (SHERR_AGAIN);

  *speer_p = shpeer_init(NULL, ret_val);
  free(ret_val);

  return (0);
}


static int shdb_peer_list_cb(void *p, int arg_nr, char **args, char **cols)
{
  shpeer_t **peer_list = (shpeer_t **)p;
  int idx;

  for (idx = 0; peer_list[idx]; idx++);

  if (arg_nr >= 2) {
    peer_list[idx] = shpeer_init(args[1], args[2]);
  }

  return (0);
}

/**
 * Provides a list of the most trusted peers.
 */
shpeer_t **shnet_track_list(shpeer_t *peer, int list_max)
{
  shdb_t *db;
  shpeer_t **peer_list;
  char sql_str[512];
  char app_name[MAX_SHARE_NAME_LENGTH];
  char *ret_val;
  int err;

  db = shnet_track_db();
  if (!db)
    return (NULL);

  memset(app_name, 0, sizeof(app_name));
  strncpy(app_name, shpeer_get_app(peer), sizeof(app_name)-1);

  peer_list = (shpeer_t **)calloc(list_max + 1, sizeof(shpeer_t *));
  if (!peer_list) {
    shdb_close(db);
    return (NULL);
  }

  list_max = MAX(1, MIN(1000, list_max));
  sprintf(sql_str, "select label,host from %s where label = '%s' order by trust limit %d", TRACK_TABLE_NAME, app_name, list_max);
  shdb_exec_cb(db, sql_str, shdb_peer_list_cb, peer_list);
  shdb_close(db);

  return (peer_list);
}

/**
 * @param sk_p Must be set to zero on initial call.
 */
int shnet_track_verify(shpeer_t *peer, int *sk_p)
{
  static char buf[32];
  struct timeval to;
  fd_set w_set;
  socklen_t ret_size;
  int ret;
  int err;
  int sk;

  if (!sk_p)
    return (SHERR_INVAL);
  
  sk = *sk_p;
  if (sk == 0) {
    /* initiate async connection to remote host for verification */
    sk = shconnect_peer(peer, SHNET_CONNECT | SHNET_ASYNC);
    if (sk < 0) {
      /* immediate error state */
      return (0);
    }

    /* async connection has begun. */
    *sk_p = sk;
  }

  FD_ZERO(&w_set);
  FD_SET(sk, &w_set);
  memset(&to, 0, sizeof(to));
  err = shnet_select(sk + 1, NULL, &w_set, NULL, &to);
  if (err < 0)
    return (-errno);
  if (err == 0)
    return (SHERR_INPROGRESS);

  ret = 0;
  ret_size = sizeof(ret);
  err = getsockopt(sk, SOL_SOCKET, SO_ERROR, &ret, &ret_size);
  if (err) {
    ret = -errno;
  } else {
    ret = -ret;
  }

  *sk_p = 0;
  shnet_close(sk);

  return (ret);
}


_TEST(shnet_track)
{
  shpeer_t *scan_peer;
  shpeer_t *peer;
  int err;
  int sk;

  /* create a new peer */
  peer = shpeer_init("", "127.0.0.1:111");
  err = shnet_track_add(peer);
  _TRUE(err == 0);

  /* scan db for fresh peer */
  err = shnet_track_scan(peer, &scan_peer);
  _TRUE(err == 0);
  _TRUE(shkey_cmp(shpeer_kpriv(peer), shpeer_kpriv(scan_peer)));
  shpeer_free(&scan_peer);

  /* verify peer */
  sk = 0;
  while ((err = shnet_track_verify(peer, &sk)) == SHERR_INPROGRESS) {
    usleep(10000); /* 10ms */
}
  _TRUE(err == 0);

  /* increment status */
  err = shnet_track_incr(peer);
  _TRUE(err == 0);

  /* remove peer */
  err = shnet_track_remove(peer);
  _TRUE(err == 0);

  shpeer_free(&peer);
}

int shnet_track_find(shpeer_t *peer)
{
  shdb_t *db;
  char hostname[MAXHOSTNAMELEN+1];
  char id_str[512];
  char buf[512];
  shdb_idx_t rowid;
  int port;
  int err;

  db = shnet_track_db();
  if (!db)
    return (SHERR_IO);

  shpeer_host(peer, hostname, &port);
  sprintf(id_str, "%s %d", hostname, port);
  err = shdb_row_find(db, TRACK_TABLE_NAME, &rowid, "host", id_str, 0);
  if (err) {
    shdb_close(db);
    return (err);
  }

  shdb_close(db);
  return (0);
}

