
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
    shdb_col_new(db, TRACK_TABLE_NAME, "ctime");
    shdb_col_new(db, TRACK_TABLE_NAME, "mtime");
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
  if (err) {
    err = shdb_row_new(db, TRACK_TABLE_NAME, &rowid);
    if (err)
      goto done;

    err = shdb_row_set(db, TRACK_TABLE_NAME, rowid, "host", id_str);
    if (err)
      goto done;

    err = shdb_row_set_time(db, TRACK_TABLE_NAME, rowid, "ctime");
    if (err)
      goto done;

    err = shdb_row_set(db, TRACK_TABLE_NAME, rowid, "trust", "0");
    if (err)
      goto done;
  }

  err = shdb_row_set(db, TRACK_TABLE_NAME, rowid,
      "label", shpeer_get_app(peer));
  if (err)
    goto done;

  strcpy(buf, shkey_print(shpeer_kpriv(peer)));
  err = shdb_row_set(db, TRACK_TABLE_NAME, rowid, "key", buf);
  if (err)
    goto done;

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

  return (0);
}


/**
 * Marks a network adderss in a positive or negative manner.
 * @param cond a negative or positive number indicating connection failure or success.
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
    return (SHERR_IO);

  trust = (long)atoll(str);
  free(str);

  if (cond < 0) {
    if (cond < -100) cond = -100;
  } else if (cond > 0) {
    if (cond > 100) cond = 100;
  }
  trust += cond;

  sprintf(buf, "%ld", trust);
  err = shdb_row_set(db, TRACK_TABLE_NAME, rowid, "trust", buf);
  if (err)
    goto done;

  err = shdb_row_set_time(db, TRACK_TABLE_NAME, rowid, "mtime");
  if (err)
    goto done;

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

  sprintf(sql_str, "select host from %s where label = '%s' order by mtime limit 1", TRACK_TABLE_NAME, app_name);
  err = shdb_exec_cb(db, sql_str, shdb_col_value_cb, &ret_val);
  shdb_close(db);
  if (err)
    return (err);

  *speer_p = shpeer_init(NULL, ret_val);
  free(ret_val);

  return (0);
}

/**
 * @param sk_p Must be set to zero on initial call.
 */
int shnet_track_verify(shpeer_t *peer, int *sk_p)
{
  struct timeval to;
  fd_set w_set;
  socklen_t ret_size;
  int ret;
  int err;
  int sk;

  if (!sk_p)
    return (SHERR_INVAL);
  
  sk = *sk_p;
  if (sk <= 0) {
    sk = shconnect_peer(peer, SHNET_CONNECT | SHNET_ASYNC);
    if (sk < 0)
      return (sk);

    *sk_p = sk;
  }

  memset(&to, 0, sizeof(to));
  FD_ZERO(&w_set);
  FD_SET(sk, &w_set);
  err = select(sk + 1, NULL, &w_set, NULL, &to);
  if (err < 0)
    return (-errno);

  if (err == 0) /* not ready */
    return (SHERR_AGAIN);

  ret = 0;
  ret_size = sizeof(ret);
  err = getsockopt(sk, SOL_SOCKET, SO_ERROR, &ret, &ret_size);

  *sk_p = 0;
  close(sk);

  return (-ret);
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
  while ((err = shnet_track_verify(peer, &sk)) == SHERR_AGAIN) {
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


