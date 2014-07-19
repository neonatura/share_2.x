
#define __STRATUM__USER_C__
#include "shcoind.h"

#define MAX_STRATUM_USERS 32
#define MIN_SHARE_DIFFICULTY 0.125 /* diff 4 */


user_t *stratum_user_find(char *username)
{
  user_t *user;

  for (user = client_list; user; user = user->next) {
    if (0 == strcasecmp(username, user->worker))
      break;
  }

  return (user);
}

int stratum_user_count(user_t *user)
{
  struct sockaddr_in *addr;
  struct sockaddr_in *t_addr;
  user_t *t_user;
  int cnt;

  cnt = 0;
  addr = (struct sockaddr_in *)shnet_host(user->fd);
  for (t_user = client_list; t_user; t_user = t_user->next) {
    if (t_user->fd == -1)
      continue;
    if (t_user->flags & USER_SYSTEM)
      continue;
    t_addr = (struct sockaddr_in *)shnet_host(t_user->fd);
    if (!t_addr)
      continue;
    if (0 == memcmp(&addr->sin_addr, 
          &t_addr->sin_addr, sizeof(struct in_addr)))
      cnt++;
  }

  return (cnt);
}

user_t *stratum_user(user_t *user, char *username)
{
  char name[256]; 
  char *ptr;

  memset(name, 0, sizeof(name));
  strncpy(name, username, sizeof(name) - 1);
  ptr = strchr(name, '_');
  if (ptr)
    *ptr = '\0';

  if (stratum_user_count(user) > MAX_STRATUM_USERS) {
    /* too many connections. */
    return (NULL);
  }


  strncpy(user->worker, username, sizeof(user->worker) - 1);

  return (user);
}



user_t *stratum_user_init(int fd)
{
  struct sockaddr_in *addr;
  user_t *user;
  char nonce1[32];

  user = (user_t *)calloc(1, sizeof(user_t));
  user->fd = fd;
  user->round_stamp = time(NULL);

addr = shnet_host(fd);
if (!addr)
sprintf(nonce1, "%-8.8x", 0);
else
sprintf(nonce1, "%-8.8x", (unsigned int)shcrc(&addr->sin_addr, sizeof(addr->sin_addr)));

  shscrypt_peer(&user->peer, nonce1, MIN_SHARE_DIFFICULTY);
  //shscrypt_peer_gen(&user->peer, MIN_SHARE_DIFFICULTY);

  user->block_freq = 2.0;

  return (user);
}

/**
 * returns the worker's average speed.
 */
double stratum_user_speed(user_t *user)
{
  double speed;
  int speed_cnt;
  int i;

  speed = 0;
  speed_cnt = 0;
  for (i = 0; i < MAX_SPEED_STEP; i++) {
    if (user->speed[i] > 0.000) {
      speed += user->speed[i];
      speed_cnt++;
    }
  }
  if (!speed_cnt)
    return (0.0);

  return (speed / (double)speed_cnt);
}

void stratum_user_block(user_t *user, task_t *task)
{
  double diff;
  double cur_t;
  double speed;
  double span;
  int step;
  
//  diff = shscrypt_hash_diff(&task->work);
  diff = task->work.pool_diff;
  if (diff != INFINITY)
    user->block_tot += (uint64_t)task->work.pool_diff;
  user->block_cnt++;

  cur_t = shtime();
  if (user->block_tm) {
    span = cur_t - user->block_tm;

    step = ((int)cur_t % MAX_SPEED_STEP);
    speed = (double)user->work_diff / span * pow(2, 32) / 0xffff;
    if (span > 1.0) {
      speed /= 1000; /* khs */
      user->speed[step] = (user->speed[step] + speed) / 2;
//      fprintf(stderr, "DEBUG: user->speed[step %d] = %f\n", step, user->speed[step]);
    }
{
double alt_speed = user->block_tot / user->block_cnt / span * pow(2, 32) / 0xffff;
fprintf(stderr, "DEBUG: alt_speed %f\n", alt_speed);
}

  fprintf(stderr, "DEBUG: stratum_user_block: worker '%s' submitted diff %6.6f block with speed %fkh/s (avg %fkh/s) [%lu/x%d]\n", user->worker, diff, speed, stratum_user_speed(user), (unsigned long)user->block_tot, user->block_cnt);

    user->block_freq = (span + user->block_freq) / 2;
    if (user->block_freq < 1.5) { 
      if (user->work_diff < 16384)
        stratum_set_difficulty(user, user->work_diff + 8);
    } else if (user->block_freq > 15) { 
      if (user->work_diff > 32)
        stratum_set_difficulty(user, user->work_diff - 8);
    }
  }
  user->block_tm = cur_t;

}


int stratum_user_broadcast_task(task_t *task)
{
  user_t *user;
  int clear;
  int err;

  if (!task)
    return (0);
  for (user = client_list; user; user = user->next) {
    if (user->fd == -1) {
      continue;
    }

    clear = (user->height < task->height);
    err = stratum_send_task(user, task, clear);
    if (!err)
      user->height = MAX(user->height, task->height);


  }

  return (0);
}


