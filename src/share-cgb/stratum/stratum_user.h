



#ifndef __STRATUM__USER_H__
#define __STRATUM__USER_H__

typedef struct user_t
{
  char name[256];
  char worker[256];
  char cli_ver[256];
  int fd;
  int dead;
  int difficulty;
  int cli_id;
  int active;
  unsigned int job_id;

  struct user_t *next;
} user_t;

user_t *stratum_user(user_t *user, char *username);

#endif /* __STRATUM__USER_H__ */

