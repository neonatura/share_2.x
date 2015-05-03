
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
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif

#include <fcntl.h>
#undef fcntl

static shproc_t *child_proc;
static shproc_pool_t *_proc_pool;

static void shproc_state_set(shproc_t *proc, int state)
{
  int ostate = proc->proc_state;

  if (state < 0 || state >= MAX_SHPROC_STATES)
    return;

  if (proc->proc_state == state)
    return;

  if (proc->proc_stamp) {
    proc->stat.span_tot[ostate] += (shtimef(shtime()) - shtimef(proc->proc_stamp));
    proc->stat.span_cnt[ostate]++;
  }

  proc->proc_stamp = shtime();
  proc->proc_state = state;
}

double shproc_stat_avg(shproc_t *proc)
{
  int type = proc->proc_state;
  if (proc->stat.span_cnt[type] == 0)
    return (0);
  return (proc->stat.span_tot[type] / (double)proc->stat.span_cnt[type]);
}

shproc_pool_t *shproc_init()
{
  struct rlimit rlim;

  if (!_proc_pool) {
    /* soft -> hard fd max / process */
    memset(&rlim, 0, sizeof(rlim));
    getrlimit(RLIMIT_NOFILE, &rlim);
    rlim.rlim_cur = MAX(rlim.rlim_cur, rlim.rlim_max);
    if (rlim.rlim_cur > 0)
      setrlimit(RLIMIT_NOFILE, &rlim);

    _proc_pool = (shproc_pool_t *)calloc(1, sizeof(shproc_pool_t));
    _proc_pool->proc = (shproc_t *)calloc(SHPROC_POOL_DEFAULT_SIZE, sizeof(shproc_pool_t));
    _proc_pool->pool_max = SHPROC_POOL_DEFAULT_SIZE; /* default */

    getrlimit(RLIMIT_NOFILE, &rlim);
    rlim.rlim_cur = MAX(rlim.rlim_cur, 1024);
    _proc_pool->pool_lim = rlim.rlim_cur;
  }

  return (_proc_pool);
}

int shproc_conf(int type, int val)
{
  shproc_pool_t *pool = shproc_init();

  if (type == SHPROC_MAX) {
    if (!val) {
      /* get */
      return (_proc_pool->pool_max);
    }

    /* set */
    _proc_pool->pool_max = MAX(1, MIN(_proc_pool->pool_lim, val));
    /* note: realloc is 'allowed' to return NULL albiet not handled here. */
    _proc_pool->proc = (shproc_t *)realloc(_proc_pool->proc,
        (size_t)(_proc_pool->pool_max * sizeof(shproc_t)));
  }

  return (0);
}

shproc_t *shproc_get(int state)
{
  shproc_pool_t *pool = shproc_init();
  shproc_t *proc;
  int i;

  if (child_proc)
    return (NULL);

  for (i = 0; i < pool->pool_max; i++) {
    if (pool->proc[i].proc_state == state) {
#if 0
      if (pool->proc[i].proc_pid != 0 &&
          0 != kill(pool->proc[i].proc_pid, 0)) {
        shproc_stop(pool->proc + i);
        continue;
      }
#endif
      proc = (pool->proc + i);
      return (proc);
    }
  }

  return (NULL);
}

static void shproc_worker_signal(int sig_num)
{
  shbuf_free(&child_proc->proc_buff);
  close(child_proc->proc_readfd);
  close(child_proc->proc_writefd);
  child_proc = NULL;
  exit(0);
}

static int shproc_worker_main(shproc_t *proc)
{
  
  while (1) {
    shproc_child_poll(proc); 
  }

}

static void shproc_nonblock(int fd)
{
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}
static int shproc_fork(shproc_t *proc)
{
  int pa_fds[2];
  int sp_fds[2];
  int fds[2];
  int err;

  if (proc->proc_state != SHPROC_NONE)
    return (0);

//socketpair

  socketpair(PF_LOCAL, SOCK_STREAM, 0, fds);
 
#if 0
  pipe2(pa_fds, O_NONBLOCK); /* parent write fds */
  pipe2(sp_fds, O_NONBLOCK); /* spawn write fds */
#endif

  err = fork();
  switch (err) {
    case 0:
      /* spawned worker */
      child_proc = proc;
#if 0
      close(pa_fds[1]); /* parent write stream */
      close(sp_fds[0]); /* child read stream */
      proc->proc_readfd = pa_fds[0];
      proc->proc_writefd = sp_fds[1];
#endif
      close(fds[0]);
      proc->proc_readfd = proc->proc_writefd = fds[1];
      shproc_nonblock(fds[1]);
      shproc_state_set(proc, SHPROC_IDLE);
      signal(SIGQUIT, shproc_worker_signal);

      /* process worker requests */
      shproc_worker_main(proc);
      exit (0); /* never returns */

    case -1:
      /* fork failure */
      return (-errno);

    default:
      /* parent process */
#if 0
      close(pa_fds[0]); /* parent read stream */
      close(sp_fds[1]); /* spawn write stream */
      proc->proc_readfd = sp_fds[0];
      proc->proc_writefd = pa_fds[1];
#endif
      close(fds[1]);
      proc->proc_readfd = proc->proc_writefd = fds[0];
      shproc_nonblock(fds[0]);
      proc->proc_pid = err;
      shproc_state_set(proc, SHPROC_IDLE);
      break;
  }

  return (0);
}

shproc_t *shproc_start(shproc_op_t req_f, shproc_op_t resp_f)
{
  shproc_t *proc;
  int err;

  proc = shproc_get(SHPROC_NONE); 
  if (!proc) {
    return (NULL);
}

  /* used by parent and spawn */
  proc->proc_buff = shbuf_init();
  proc->proc_req = req_f;
  proc->proc_resp = resp_f;

  err = shproc_fork(proc);
  if (err)
    return (NULL);
 

  return (proc);
}

int shproc_stop(shproc_t *proc)
{
  int err;

  if (child_proc)
    return (SHERR_INVAL);

  if (proc->proc_pid == 0)
    return (SHERR_INVAL);

  if (proc->proc_state == SHPROC_NONE)
    return (0); /* all done */

  err = kill(proc->proc_pid, SIGQUIT);  
  if (err)
    return (err);

  shproc_state_set(proc, SHPROC_NONE);

  shbuf_free(&proc->proc_buff);

  close(proc->proc_readfd);
  close(proc->proc_writefd);

  proc->proc_readfd =  proc->proc_writefd = proc->proc_pid = 0;

  return (0);
}

/**
 * @param wait_t milliseconds to wait for process to send a message.
 */ 
int shproc_wait(shproc_t *proc, int wait_t)
{
  struct timeval to;
  fd_set in_set;
  int err;

  if (!proc)
    return (SHERR_INVAL);

  if (wait_t) {
    to.tv_sec = wait_t / 1000; 
    to.tv_usec = (wait_t % 1000) * 1000; 
  }

  /* full-blocking poll */
  FD_ZERO(&in_set);
  FD_SET(proc->proc_readfd, &in_set);
  err = select(proc->proc_readfd+1, &in_set, NULL, NULL, 
      !wait_t ? NULL /* blocking poll */ : &to /* semi-blocking */);
  if (err < 0)
    return (-errno);

  return (0);
}

static int shproc_write(shproc_t *proc, shproc_req_t *req)
{
  int err;

  req->data_len = shbuf_size(proc->proc_buff);
  req->crc = shcrc(shbuf_data(proc->proc_buff), shbuf_size(proc->proc_buff));
  err = write(proc->proc_writefd, req, sizeof(shproc_req_t));
  if (err == -1) 
    return (-errno);
  if (err == 0)
    return (SHERR_AGAIN);

  if (shbuf_size(proc->proc_buff)) {
/* todo: write buffer is only 4k on i386 */
    err = write(proc->proc_writefd, shbuf_data(proc->proc_buff), shbuf_size(proc->proc_buff));
    if (err == -1)
      return (-errno);
    if (err == 0)
      return (SHERR_AGAIN);
  }

  return (0);
}

int shproc_schedule(shproc_t *proc, unsigned char *data, size_t data_len)
{
  shproc_req_t req;
  int err;

  if (!proc)
    proc = shproc_get(SHPROC_IDLE);
  if (!proc)
    return (SHERR_AGAIN);

  shbuf_clear(proc->proc_buff);
  if (data && data_len)
    shbuf_cat(proc->proc_buff, data, data_len);

  memset(&req, 0, sizeof(req));
  req.state = SHPROC_RUN;
  err = shproc_write(proc, &req);
  if (err) {
    return (err);
  }

  /* set process to pending state */
  shproc_state_set(proc, SHPROC_PEND);
  proc->stat.out_tot++;
  
  return (0);
}

static int shproc_read(shproc_t *proc)
{
  struct shproc_req_t req;
  struct timeval to;
  fd_set in_set;
  char buf[4096];
  int r_len;
  int err;
  int of;

  err = shproc_wait(proc, child_proc ? 1000 : 1);

  memset(&req, 0, sizeof(req));
  r_len = read(proc->proc_readfd, &req, sizeof(req));
  if (r_len == -1 && errno != EAGAIN) {
    return (-errno);
  }
  if (r_len != sizeof(req))
    return (1); /* nothing to read */

  if (!child_proc) {
    /* parent process */
    if (req.state == SHPROC_RUN || 
        req.state == SHPROC_IDLE) {
      shproc_state_set(proc, req.state);
    }
    proc->stat.in_tot++;
  }

  of = 0;
  shbuf_clear(proc->proc_buff);
  for (of = 0; of < req.data_len; of += 4096) {
    r_len = read(proc->proc_readfd, buf, 4096);
    if (r_len == -1) {
      return (-errno);
    }
    shbuf_cat(proc->proc_buff, buf, r_len);
  }

  proc->proc_error = req.error;

  return (0);
}

int shproc_parent_poll(shproc_t *proc)
{
  struct timeval to;
  shbuf_t *sp_buf;
  fd_set in_set;
  char buf[4096];
  int r_len;
  int err;

  if (proc->proc_pid == 0 || proc->proc_state == SHPROC_NONE)
    return (SHERR_INVAL);

  if (0 != kill(proc->proc_pid, 0)) {
    err = -errno;
    shproc_stop(proc);
    return (err);
  }

  if (proc->proc_state == SHPROC_IDLE)
    return (0); /* spawn process is idle -- nothing to read */ 

  while ((err = shproc_read(proc)) == 0) {
    if (proc->proc_state != SHPROC_IDLE) {
      continue; /* not a response */
    }
    if (proc->proc_resp) {
      /* return response data to callback */
      (*proc->proc_resp)(proc->proc_error, proc->proc_buff);
    }
    proc->proc_error = 0;
  }
  if (err == 1)
    return (0); /* nothing to do */
  if (err)
    return (err);

  return (0);
}

int shproc_child_poll(shproc_t *proc)
{
  struct shproc_req_t req;
  struct timeval to;
  shbuf_t *sp_buf;
  fd_set in_set;
  char buf[4096];
  int r_len;
  int err;

  err = shproc_read(proc);
  if (err == 1)
    return (0); /* nothing to do */
  if (err)
    return (err);

  memset(&req, 0, sizeof(req));
  /* spawned worker - data receieved as request */
  sp_buf = shbuf_clone(proc->proc_buff);

  memset(&req, 0, sizeof(req));
  req.state = SHPROC_RUN;
  shbuf_clear(proc->proc_buff);
  err = shproc_write(proc, &req);

  err = 0;
  if (proc->proc_req) {
    err = (*proc->proc_req)((int)proc->proc_idx, sp_buf);
    /* user-result data */
    shbuf_append(sp_buf, proc->proc_buff);
  }
  shbuf_free(&sp_buf);

  memset(&req, 0, sizeof(req));
  req.state = SHPROC_IDLE;
  req.error = err;
  err = shproc_write(proc, &req);

  proc->proc_idx++;
  shproc_state_set(proc, SHPROC_IDLE);
  shbuf_clear(proc->proc_buff);

  return (0);
}

static int _test_shproc_value[256];
static int _test_shproc_req(int idx, shbuf_t *buff)
{
  int val;


  if (shbuf_size(buff) != sizeof(val)) {
    
    return (-1);
  }
  val = *((int *)shbuf_data(buff));

  return (0);
}

static int _test_shproc_resp(int err_code, shbuf_t *buff)
{
  if (err_code == 0) {
    int val = *((int *)shbuf_data(buff));
    _test_shproc_value[val] = val+1;
  }
  return (0);
}

_TEST(shproc_schedule)
{
  shproc_t *proc_list[256];
  shproc_t *proc;
  int val;
  int t_val;
  int err;
  int i;

  shproc_conf(SHPROC_MAX, 2);
  for (i = 0; i < 2; i++) {
    proc = shproc_start(&_test_shproc_req, &_test_shproc_resp);
    _TRUEPTR(proc);
    proc_list[i] = proc;

    val = i;
    err = shproc_schedule(proc, (unsigned char *)&val, sizeof(val));
    _TRUE(0 == err);
  }
sleep(1);

  /* handle ACK response */
  for (i = 0; i < 2; i++) {
    err = shproc_parent_poll(proc_list[i]);
    _TRUE(0 == err);
  }

#if 0
  for (i = 0; i < 2; i++) {
    /* wait for work to be finished. */
    err = shproc_wait(proc_list[i], 0);
  }
#endif

  for (i = 0; i < 2; i++) {
    _TRUE(0 == shproc_stop(proc_list[i]));
  }

  for (i = 0; i < 2; i++) {
    /* verify response */
    _TRUE(_test_shproc_value[i]-1 == i);
  }

}

