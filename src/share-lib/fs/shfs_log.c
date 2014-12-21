
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

#if defined(HAVE_SYS_RESOURCE_H)
#include <sys/resource.h>
#endif


#if 0
static shpeer_t flog_peer;
static shflog_t *_flog_data;
static int _flog_level;
static int _flog_index;

static char *_flog_level_label[MAX_LOG_LEVELS] = {
  "INFO",
  "VERBOSE",
  "TIMING",
  "DEBUG",
  "WARNING",
  "ERROR",
  "FATAL"
};

int shflog_init(shpeer_t *peer, int min_level)
{

  if (peer) {
    shflog_free();
  } else {
    peer = shpeer();
  }

  _flog_level = min_level;

  if (!_flog_data) {
    char path[PATH_MAX+1];
    shfs_t *tree;
    shfs_ino_t *fl;
    void *data;
    size_t data_len;
    shtime_t stamp;
    int err;
    int i;

    /* establish global-scope peer */
    memcpy(&flog_peer, peer, sizeof(shpeer_t));

    sprintf(path, "%s/flog/", get_libshare_path());
    mkdir(path, 0777);
    strcat(path, shkey_print(&flog_peer.name));
    shfs_read_mem(path, (char **)&_flog_data, &data_len);

    /* load rotating flog into memory */
/*
    tree = shfs_init(&flog_peer); 
    sprintf(path, "/flog/%s", shkey_print(&flog_peer.name));
    fl = shfs_file_find(tree, path); 
    err = shfs_file_read(fl, &data, &data_len);
    if (!err) {
      _flog_data = (shflog_t *)data;
    }
    shfs_free(&tree);
*/

    _flog_index = 0;
    if (!_flog_data) {
      _flog_data = (shflog_t *)calloc(MAX_LOG_SIZE, sizeof(shflog_t));
    } else {
      /* find last offset */
      stamp = 0;
      for (i = 0; i < MAX_LOG_SIZE; i++) {
        if (_flog_data[i].flog_stamp >= stamp) {
          stamp = _flog_data[i].flog_stamp; 
          _flog_index = i;
        }
      }
    }
  }

  return (0);
}

void shflog_free(void)
{
  if (_flog_data)
    free(_flog_data);
  _flog_data = NULL;
}


int shflog_flush(void)
{
  shfs_t *tree;
  shfs_ino_t *fl;
  char path[PATH_MAX+1];
  int err;

  if (!_flog_data)
    return (0);

    sprintf(path, "%s/flog/", get_libshare_path());
    mkdir(path, 0777);
    strcat(path, shkey_print(&flog_peer.name));
    shfs_write_mem(path, (char *)_flog_data, MAX_LOG_SIZE * sizeof(shflog_t));

  /* save rotating flog to disk. */
/*
  tree = shfs_init(&flog_peer); 
  sprintf(path, "/flog/%s", shkey_print(&flog_peer.name));
  fl = shfs_file_find(tree, path); 
  err = shfs_file_write(fl, _flog_data, MAX_LOG_SIZE * sizeof(shflog_t));
  shfs_free(&tree);
*/
 
  return (err);
}

int shflog(int level, char *msg)
{
  static shtime_t last_stamp;
  shtime_t stamp;
  int index;
  int err;

  if (level < _flog_level)
    return (0);

  err = shflog_init(NULL, 0);
  if (err)
    return (err);

  stamp = shtime64();
  index = (_flog_index % MAX_LOG_SIZE);
  _flog_index++;

  _flog_data[index].flog_level = level;
  strncpy(_flog_data[index].flog_text, msg,
    sizeof(_flog_data[index].flog_text) - 1);
  _flog_data[index].flog_stamp = stamp;

  if (0 == (_flog_index % MAX_LOG_SIZE) ||
      last_stamp < (stamp - 300000)) { /* 5min */
    shflog_flush();
    shflog_free();
  }
  last_stamp = stamp;

  //PRINT_RUSAGE(msg);

  return (0);
}

TEST(shflog)
{
shbuf_t *buff;
  char buf[256];
  int max;
  int i;

  max = MAX_LOG_SIZE * 2;
  for (i = 0; i < max; i++) {
    sprintf(buf, "%x", i);
    shflog((i % MAX_LOG_LEVELS), buf);
  }

//  shflog_free();
 
}

char *shflog_level_label(int level)
{
  if (level < 0 || level >= MAX_LOG_LEVELS)
    return ("N/A");
  return (_flog_level_label[level]);
}

void shflog_print_line(shbuf_t *buff, shflog_t *flog, shtime_t *stamp_p)
{
  char line[256];
  char *tptr;

if (!buff || !flog || !stamp_p)
return;

  tptr = shctime64(flog->flog_stamp);

  sprintf(line, "[%-20.20s (%lums) %s] ",
      (strlen(tptr)>4) ? (tptr+4) : "",
      *stamp_p ? (unsigned long)(flog->flog_stamp - *stamp_p) : 0,
      shflog_level_label(flog->flog_level));
  shbuf_catstr(buff, line);
  shbuf_catstr(buff, flog->flog_text); 
  shbuf_catstr(buff, "\n");

  *stamp_p = flog->flog_stamp;
}

int shflog_print(int lines, shbuf_t *buff)
{
  shtime_t last_t;
  int line_cnt;
  int err;
  int i;

  err = shflog_init(NULL, 0);
  if (err)
    return (err);

  last_t = 0;
  line_cnt = 0;
  for (i = _flog_index; i < MAX_LOG_SIZE; i++) {
    if (_flog_data[i].flog_stamp == 0)
      continue;
    if (line_cnt < lines)
      shflog_print_line(buff, &_flog_data[i], &last_t);
    line_cnt++;
  }
  for (i = 0; i < _flog_index; i++) {
    if (_flog_data[i].flog_stamp == 0)
      continue;
    if (line_cnt < lines)
      shflog_print_line(buff, &_flog_data[i], &last_t);
    line_cnt++;
  }

  shflog_free();

  return (0);
}

TEST(shflog_print)
{
  shbuf_t *buff;
  int nl_cnt;
  int j;

  buff = shbuf_init();
  _TRUEPTR(buff);
  shflog_print(MAX_LOG_SIZE, buff);

  nl_cnt = 0;
  for (j = 0; j < buff->data_of; j++) {
    if (buff->data[j] == '\n')
      nl_cnt++;
  }
  _TRUE(nl_cnt == MAX_LOG_SIZE);

  shbuf_free(&buff);

}
#endif




#define SHLOG_INFO 1
#define SHLOG_WARNING 2
#define SHLOG_ERROR 3
#define SHLOG_RUSAGE 4

static int _log_queue_id;

int shlog(int level, int err_code, char *log_str)
{
  shbuf_t *buff;
  char line[1024];
  uint32_t type;
  time_t now;
  int err;

  if (_log_queue_id <= 0) {
    shpeer_t *log_peer;

    /* friendly local log daemon */
    log_peer = shpeer_init("shlogd", NULL);
    _log_queue_id = shmsgget(log_peer);
    shpeer_free(&log_peer);
  }
  if (_log_queue_id < 0)
    return;

 // type = TX_LOG;
  now = time(NULL);
  
  buff = shbuf_init();

/*
  shbuf_cat(buff, &type, sizeof(type));
  strftime(line, sizeof(line) - 1, "%D %T", localtime(&now));
  shbuf_catstr(buff, line);
*/

  if (level == SHLOG_ERROR) {
    shbuf_catstr(buff, "Error: ");
  } else if (level == SHLOG_WARNING) {
    shbuf_catstr(buff, "Warning: ");
  }
  if (err_code) {
    sprintf(line, "%s [code %d]: ", strerror(-(err_code)), (err_code));
    shbuf_catstr(buff, line);
  }
  shbuf_catstr(buff, log_str);
  shbuf_catstr(buff, "\n");
  err = shmsgsnd(_log_queue_id, shbuf_data(buff), shbuf_size(buff));
  shbuf_free(&buff);
  if (err)
    return (err);

  return (0);
}

void shlog_free(void)
{

  if (_log_queue_id > 0) {
    shmsgctl(_log_queue_id, SHMSGF_RMID, 1);
    _log_queue_id = 0;
  }

}

void sherr(int err_code, char *log_str)
{
  shlog(SHLOG_ERROR, err_code, log_str);
}

void shwarn(char *log_str)
{
  shlog(SHLOG_WARNING, 0, log_str);
}

void shinfo(char *log_str)
{
  shlog(SHLOG_INFO, 0, log_str);
}

void shlog_rinfo(void)
{
#if defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_GETRUSAGE)
  struct rusage rusage;
  char rinfo_buf[256];

  memset(&rusage, 0, sizeof(rusage));
  getrusage(RUSAGE_SELF, &rusage);

  sprintf(rinfo_buf,
      "PROCESS [cpu(user:%d.%-6.6ds sys:%d.%-6.6ds maxrss(%uk) ixrss(%uk) idrss(%uk) flt(%uk) swaps(%uk) in-ops(%uk) out-ops(%uk)]",
      rusage.ru_utime.tv_sec, rusage.ru_utime.tv_usec, \
      rusage.ru_stime.tv_sec, rusage.ru_stime.tv_usec, \
      rusage.ru_maxrss, rusage.ru_ixrss, rusage.ru_idrss, \
      rusage.ru_majflt, rusage.ru_nswap, \
      rusage.ru_inblock, rusage.ru_oublock);

  shinfo(rinfo_buf);
#endif
}

