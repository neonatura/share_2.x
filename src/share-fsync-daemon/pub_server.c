
#include "pub_server.h"

shpeer_t *_pubd_peer;

static void pubd_free(void)
{
  pubd_cycle_free();
  shpeer_free(&_pubd_peer);
}

void pubd_terminate(int sig_num)
{

  signal(sig_num, SIG_DFL);

  pubd_free();

  raise(sig_num);

}

void pubd_signal(void)
{
  signal(SIGTERM, pubd_terminate);
}

int main(int argc, char *argv[])
{

  _pubd_peer = shapp_init(argv[0], NULL, 0);
#if 0
{
  uid_t uid = getuid();
  struct passwd *pw = getpwuid(uid);
  struct spwd *spwd;
  pubuser_t *u;
  int err;

  spwd = getspnam(pw->pw_name);
  if (spwd)
    pw->pw_passwd = spwd->sp_pwdp; /* use shadow passwd */

  fprintf(stderr, "DEBUG: user '%s'\n"
      "\tpasswd: %s\n"
      "\tuid: %d\n"
      "\tgid: %d\n"
      "\tgecos: %s\n"
      "\tdir: %s\n"
      "\tshell: %s\n",
      pw->pw_name,
      pw->pw_passwd,
      pw->pw_uid,
      pw->pw_gid,
      pw->pw_gecos,
      pw->pw_dir,
      pw->pw_shell);

  u = pubd_user_add(pw->pw_name, pw->pw_passwd, pw->pw_dir);

  err = pubd_user_validate(u, "..");
fprintf(stderr, "DEBUG: %d = pubd_user_validate(%s, ..)\n", err, u->name);
exit(1);
}
#endif

  daemon(0, 1);

  pubd_signal();

  pubd_cycle();

  shpeer_free(&_pubd_peer);
 
  return (0);
}


