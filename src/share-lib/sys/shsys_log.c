
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

#define SHLOG_INFO 1
#define SHLOG_WARNING 2
#define SHLOG_ERROR 3
#define SHLOG_RUSAGE 4

static int _log_queue_id;

static size_t shlog_mem_size(void)
{
  size_t mem_size;

  mem_size = 0;

#ifdef linux
  {
    FILE *fl = fopen("/proc/self/status", "r");
    if (fl) {
      char buf[256];

      while (fgets(buf, sizeof(buf) - 1, fl) != NULL) {
        if (0 == strncmp(buf, "VmSize:", strlen("VmSize:"))) {
          mem_size = (size_t)atol(buf + strlen("VmSize:"));
          break;
        }
      }
      fclose(fl);
    }
  }
#endif

  return (mem_size);
}

#if 0
int shlog(int level, int err_code, char *log_str)
{
  shbuf_t *buff;
  char line[1024];
  char *data;
  size_t mem_size;
  uint32_t type;
  time_t now;
  int err;

#ifndef DEBUG
  if (_log_queue_id <= 0) {
    shpeer_t *log_peer;

    /* friendly local log daemon */
    log_peer = shpeer_init("shlogd", NULL);
    _log_queue_id = shmsgget(log_peer);
    shpeer_free(&log_peer);
  }
  if (_log_queue_id < 0)
    return;
#endif

  err = 0;
  now = time(NULL);
  buff = shbuf_init();
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

  mem_size = shlog_mem_size();
  if (mem_size >= 1000) {
    sprintf(line, " (mem:%dk)", (mem_size / 1000)); 
    shbuf_catstr(buff, line);
  }


  data = shbuf_data(buff);
#ifndef DEBUG
  err = shmsgsnd(_log_queue_id, data, shbuf_size(buff));
#else
  fprintf(stderr, "%s %s\n", shstrtime(shtime(), "[%x %T]"), data);
#endif

  shbuf_free(&buff);
  if (err)
    return (err);

  return (0);
}
#endif

static FILE *_shlog_file;

#if 0
static shbuf_t *_shlog_buff;
int shlog_init(shbuf_t **buff_p, size_t data_len)
{
  struct stat st;
  char path[PATH_MAX+1];
  char log_path[PATH_MAX+1];
  char bin_path[PATH_MAX+1];
  shbuf_t *buff;
  shpeer_t peer;
  size_t req_len;
  time_t now;
  int err;
  int fd;

  if (!buff_p)
    return (SHERR_INVAL);

  buff = *buff_p;
  *buff_p = NULL;

  if (!buff) {
    memcpy(&peer, ashpeer(), sizeof(peer));
  #ifdef WINDOWS
    sprintf(path, "%s\\share\\log\\", getenv("APPDATA"));
  #else
    sprintf(path, "/var/log/share/");
  #endif
    mkdir(path, 0777);

    if (!*peer.label)
      strcat(path, PACKAGE_NAME);
    else
      strcat(path, peer.label);

    now = time(NULL);
    strcpy(bin_path, path);
    strftime(bin_path+strlen(bin_path), 
        sizeof(bin_path)-strlen(bin_path)-1, ".%y.%W", localtime(&now));

    sprintf(log_path, "%s.log", path);
    _shlog_file = fopen(log_path, "wb");

    fd = open(bin_path, O_RDWR | O_CREAT, S_IRWXU);
    if (fd == -1)
      return (-errno);

    buff = shbuf_init();
    buff->fd = fd;

    fstat(fd, &st);
    req_len = MAX(SHARE_PAGE_SIZE, st.st_size);
    err = shbuf_growmap(buff, req_len);
    if (err) {
      shbuf_free(&buff);
      return (err);
    }

    buff->data_of = strlen(shbuf_data(buff));
  }
  req_len = shbuf_size(buff) + data_len + SHARE_PAGE_SIZE + 1;

  if (req_len >= buff->data_max) {
    req_len = (req_len / SHARE_PAGE_SIZE);
    req_len *= SHARE_PAGE_SIZE;
    if (req_len != buff->data_max) {
      err = shbuf_growmap(buff, req_len);
      if (err) {
        shbuf_free(&buff);
        return (err);
      }
    }
    buff->data_of = strlen(shbuf_data(buff));
  }

  *buff_p = buff;

  return (0);
}
#endif

void shlog_write(shbuf_t *buff, int level, int err_code, char *log_str)
{
  static char log_path[PATH_MAX+1];
  char line[640];
  char *beg_line;
  size_t mem_size;

  if (!buff)
    return;

  if (!*log_path) {
    shpeer_t peer;
    char path[PATH_MAX+1];

    memcpy(&peer, ashpeer(), sizeof(peer));
  #ifdef WINDOWS
    sprintf(path, "%s\\share\\log\\", getenv("APPDATA"));
  #else
    sprintf(path, "/var/log/share/");
  #endif
    mkdir(path, 0777);

    if (!*peer.label)
      strcat(path, PACKAGE_NAME);
    else
      strcat(path, peer.label);
    sprintf(log_path, "%s.log", path);
  }
  if (*log_path && !_shlog_file) {
    _shlog_file = fopen(log_path, "ab");
  }

  beg_line = shbuf_data(buff) + shbuf_size(buff);

  sprintf(line, "%s", shstrtime(shtime(), "[%x %T] "));
  shbuf_catstr(buff, line);

  if (level == SHLOG_ERROR) {
    shbuf_catstr(buff, "error");
  } else if (level == SHLOG_WARNING) {
    shbuf_catstr(buff, "warning");
  } else {
    shbuf_catstr(buff, "info");
  }

  if (err_code) {
    memset(line, 0, sizeof(line));
    snprintf(line, sizeof(line) - 1,
        ": %s [code %d]", strerror(-(err_code)), (err_code));
    shbuf_catstr(buff, line);
  }

  if (log_str) {
    shbuf_catstr(buff, ": ");
    shbuf_catstr(buff, log_str);
  }

  mem_size = shlog_mem_size();
  if (mem_size > 100000) {
    sprintf(line, " (mem:%dk)", (mem_size / 1000)); 
    shbuf_catstr(buff, line);
  }

  shbuf_catstr(buff, "\n");

}

void shlog_free(void)
{

  if (_shlog_file) {
    fclose(_shlog_file);
    _shlog_file = NULL;
  }

}

#define MAX_FLUSH_SPAN 5

int shlog(int level, int err_code, char *log_str)
{
  static time_t last_day;
  static time_t last_flush;
  static shbuf_t *buff;
  time_t day;
  time_t now;
  int err;

  now = time(NULL);
  day = now / 86400; 
  if (day != last_day) {
    // shlog_zcompr();  /* compress .YY.WW bin log file, removing prev zip */
    shlog_free();
  }
  last_day = day;

  if (!buff)
    buff = shbuf_init();
  shbuf_clear(buff);

  shlog_write(buff, level, err_code, log_str);

  if (shbuf_data(buff) && _shlog_file) {
    fprintf(_shlog_file, "%s", shbuf_data(buff));
    if (last_flush < (now - MAX_FLUSH_SPAN)) {
      fflush(_shlog_file);
      last_flush = now;
    }
  }

  return (0);
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
      "PROCESS [cpu(user:%d.%-6.6ds sys:%d.%-6.6ds) maxrss(%uk) flt(%uk) swaps(%uk) in-ops(%uk) out-ops(%uk)]",
      rusage.ru_utime.tv_sec, rusage.ru_utime.tv_usec,
      rusage.ru_stime.tv_sec, rusage.ru_stime.tv_usec,
      rusage.ru_maxrss, rusage.ru_majflt, rusage.ru_nswap,
      rusage.ru_inblock, rusage.ru_oublock);

  shinfo(rinfo_buf);
#endif
}

