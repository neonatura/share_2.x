
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

static shpeer_t log_peer;
static shlog_t *_log_data;
static int _log_level;
static int _log_index;

static char *_log_level_label[MAX_LOG_LEVELS] = {
  "INFO",
  "VERBOSE",
  "TIMING",
  "DEBUG",
  "WARNING",
  "ERROR",
  "FATAL"
};

int shlog_init(char *app, int min_level)
{

  if (!_log_level)
    _log_level = min_level;

  if (!_log_data) {
    char path[PATH_MAX+1];
    shfs_t *tree;
    shfs_ino_t *fl;
    void *data;
    size_t data_len;
    shpeer_t *peer;
    shtime_t stamp;
    int err;
    int i;

    /* retrieve unique peer for app name */
    peer = shpeer_app(app);
    if (!peer)
      return (SHERR_INVAL);

    /* establish global-scope peer */
    memcpy(&log_peer, peer, sizeof(shpeer_t));

    sprintf(path, "%s/.share/log/", getenv("HOME"));
    mkdir(path, 0777);
    strcat(path, shkey_print(&log_peer.name));
    shfs_read_mem(path, (char **)&_log_data, &data_len);

    /* load rotating log into memory */
/*
    tree = shfs_init(&log_peer); 
    sprintf(path, "/log/%s", shkey_print(&log_peer.name));
    fl = shfs_file_find(tree, path); 
    err = shfs_file_read(fl, &data, &data_len);
    if (!err) {
      _log_data = (shlog_t *)data;
    }
    shfs_free(&tree);
*/

    _log_index = 0;
    if (!_log_data) {
      _log_data = (shlog_t *)calloc(MAX_LOG_SIZE, sizeof(shlog_t));
    } else {
      /* find last offset */
      stamp = 0;
      for (i = 0; i < MAX_LOG_SIZE; i++) {
        if (_log_data[i].log_stamp >= stamp) {
          stamp = _log_data[i].log_stamp; 
          _log_index = i;
        }
      }
    }
  }

  return (0);
}

void shlog_free(void)
{
  if (_log_data)
    free(_log_data);
  _log_data = NULL;
}


int shlog_flush(void)
{
  shfs_t *tree;
  shfs_ino_t *fl;
  char path[PATH_MAX+1];
  int err;

  if (!_log_data)
    return (0);

    sprintf(path, "%s/.share/log/", getenv("HOME"));
    mkdir(path, 0777);
    strcat(path, shkey_print(&log_peer.name));
    shfs_write_mem(path, (char *)_log_data, MAX_LOG_SIZE * sizeof(shlog_t));

  /* save rotating log to disk. */
/*
  tree = shfs_init(&log_peer); 
  sprintf(path, "/log/%s", shkey_print(&log_peer.name));
  fl = shfs_file_find(tree, path); 
  err = shfs_file_write(fl, _log_data, MAX_LOG_SIZE * sizeof(shlog_t));
  shfs_free(&tree);
*/
 
  return (err);
}

int shlog(int level, char *msg)
{
  static shtime_t last_stamp;
  shtime_t stamp;
  int index;
  int err;

  if (level < _log_level)
    return (0);

  err = shlog_init(NULL, 0);
  if (err)
    return (err);

  stamp = shtime64();
  index = (_log_index % MAX_LOG_SIZE);
  _log_index++;

  _log_data[index].log_level = level;
  strncpy(_log_data[index].log_text, msg,
    sizeof(_log_data[index].log_text) - 1);
  _log_data[index].log_stamp = stamp;

  if (0 == (_log_index % MAX_LOG_SIZE) ||
      last_stamp < (stamp - 300000)) { /* 5min */
    shlog_flush();
    shlog_free();
  }
  last_stamp = stamp;

  //PRINT_RUSAGE(msg);

  return (0);
}

_TEST(shlog)
{
shbuf_t *buff;
  char buf[256];
  int max;
  int i;

  max = MAX_LOG_SIZE * 2;
  for (i = 0; i < max; i++) {
    sprintf(buf, "%x", i);
    shlog((i % MAX_LOG_LEVELS), buf);
  }

//  shlog_free();
 
}

char *shlog_level_label(int level)
{
  if (level < 0 || level >= MAX_LOG_LEVELS)
    return ("N/A");
  return (_log_level_label[level]);
}

void shlog_print_line(shbuf_t *buff, shlog_t *log, shtime_t *stamp_p)
{
  char line[256];

  sprintf(line, "[%-20.20s (%lums) %s] ",
      shctime64(log->log_stamp) + 4,
      *stamp_p ? (log->log_stamp - *stamp_p) : 0,
      shlog_level_label(log->log_level));
  shbuf_catstr(buff, line);
  shbuf_catstr(buff, log->log_text); 
  shbuf_catstr(buff, "\n");

  *stamp_p = log->log_stamp;
}

int shlog_print(int lines, shbuf_t *buff)
{
  shtime_t last_t;
  int line_cnt;
  int err;
  int i;

  err = shlog_init(NULL, 0);
  if (err)
    return (err);

  last_t = 0;
  line_cnt = 0;
  for (i = _log_index; i < MAX_LOG_SIZE; i++) {
    if (_log_data[i].log_stamp == 0)
      continue;
    if (line_cnt < lines)
      shlog_print_line(buff, &_log_data[i], &last_t);
    line_cnt++;
  }
  for (i = 0; i < _log_index; i++) {
    if (_log_data[i].log_stamp == 0)
      continue;
    if (line_cnt < lines)
      shlog_print_line(buff, &_log_data[i], &last_t);
    line_cnt++;
  }

  shlog_free();

  return (0);
}

_TEST(shlog_print)
{
  shbuf_t *buff;
  int nl_cnt;
  int j;

  buff = shbuf_init();
  _TRUEPTR(buff);
  shlog_print(MAX_LOG_SIZE, buff);

  nl_cnt = 0;
  for (j = 0; j < buff->data_of; j++) {
    if (buff->data[j] == '\n')
      nl_cnt++;
  }
  _TRUE(nl_cnt == MAX_LOG_SIZE);

  shbuf_free(&buff);

}


