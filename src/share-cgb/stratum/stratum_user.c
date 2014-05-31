
#define __STRATUM__USER_C__
#include "shcgbd.h"

#define MAX_STRATUM_USERS 32
#define MIN_SHARE_DIFFICULTY 0.1

static user_t *user_list;

user_t *stratum_user_find(char *username)
{
  user_t *user;

  for (user = user_list; user; user = user->next) {
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
  for (t_user = user_list; t_user; t_user = t_user->next) {
    t_addr = (struct sockaddr_in *)shnet_host(t_user->fd);
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
  user_t *user;

  user = (user_t *)calloc(1, sizeof(user_t));
  user->fd = fd;
  shscrypt_peer_gen(&user->peer, MIN_SHARE_DIFFICULTY);

  user->next = user_list;
  user_list = user;

  return (user);
}

void stratum_user_block(user_t *user, task_t *task)
{
  user->block_tot += shscrypt_hash_diff(&task->work);
  user->block_cnt++;
}


int stratum_user_broadcast_task(task_t *task)
{
  user_t *user;
  int clear;
  int err;

  for (user = user_list; user; user = user->next) {
    clear = (user->height != task->height);
    err = stratum_send_task(user, task, clear);
    if (!err)
      user->height = task->height;
  }

  return (0);
}


