
#define __STRATUM__USER_C__
#include "shcgbd.h"

#define MAX_STRATUM_USERS 32

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
  int diff;

  diff = 0;
  memset(name, 0, sizeof(name));
  ptr = strchr(username, '_');
  if (ptr) {
    strncpy(name, username, 
        MIN(sizeof(name) - 1, strlen(username) - strlen(ptr))); 
    diff = atoi(ptr + 1); 
  } else {
    strncpy(name, username, sizeof(name) - 1);
  }

  if (diff < 1 || diff > 65536) {
    diff = 32;
  }

  if (stratum_user_count(user) > MAX_STRATUM_USERS) {
    /* too many connections. */
    return (NULL);
  }

  strncpy(user->worker, username, sizeof(user->worker) - 1);
  strncpy(user->name, name, sizeof(user->name) - 1);
  user->difficulty = diff;

  return (user);
}




