
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

  return (user);
}

void stratum_user_block(user_t *user, task_t *task)
{
  user->block_tot += shscrypt_hash_diff(&task->work);
  user->block_cnt++;
  user->block_stamp = time(NULL);
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


