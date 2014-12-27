
#include "pub_server.h"


pubuser_t *_pubd_users;

pubuser_t *pubd_add_user(char *path)
{
  pubuser_t *u;

  if (!*path)
    return (NULL);

  u = (pubuser_t *)calloc(1, sizeof(pubuser_t));
  if (!u)
    return (NULL);

  strncpy(u->root_path, path, PATH_MAX);
  return (u);
}

void pubd_scan_user(pubuser_t *u)
{
  DIR *dir;

  for (u = _pubd_users; u; u = u->next) {
    dir = opendir(u->root_path);
    if (!dir) {
      u->err = errno;
      continue;
    }

    closedir(dir);
  }

}

void pubd_scan(void)
{
  pubuser_t *u;
  DIR *dir;

  for (u = users; u; u = u->next) {
    dir = opendir(u->root_path);
    if (!dir) {
      u->err = errno;
      continue;
    }

    closedir(dir);
  }

}

void pubd_scan_init(void)
{
  struct passwd *pw;
  char path[PATH_MAX+1];
  uid_t uid;

  for (uid = 0; uid < 1000; uid++) {
    pw = getpwuid(uid);
    if (!pw)
      continue;

    sprintf(path, "%s/share", pw->pw_dir);
    err = stat(path, &st);
    if (err)
      continue;

    pubd_add_user(path);
  }

}

