
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
    FILE *fl = fopen("/proc/self/stautus", "r");
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

int shlog(int level, int err_code, char *log_str)
{
  shbuf_t *buff;
  char line[1024];
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

  shbuf_catstr(buff, "\n");
#ifndef DEBUG
  err = shmsgsnd(_log_queue_id, shbuf_data(buff), shbuf_size(buff));
#else
  fprintf(stderr, "%s %s\n", 
      shstrtime(shtime(), "[%x %T]"), shbuf_data(buff));
#endif
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

