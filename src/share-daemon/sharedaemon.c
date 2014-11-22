
#include "sharedaemon.h"
#include <sys/types.h>
#include <signal.h>

shpeer_t *server_peer;
//sh_account_t *server_account;
sh_ledger_t *server_ledger;

void sharedaemon_term(void)
{
  int err;

  if (listen_sk) {
    shnet_close(listen_sk);
    listen_sk = 0;
  }

}

void sharedaemon_signal(int sig_num)
{
  signal(sig_num, SIG_DFL);

  sharedaemon_term();
}

int main(int argc, char *argv[])
{
  unsigned int port = (unsigned int)SHARE_DAEMON_PORT;
	sh_account_t *acc;
	shpeer_t *peer;
  int err;
  int fd;

  daemon(0, 1);

  signal(SIGHUP, SIG_IGN);
  signal(SIGPIPE, SIG_IGN);
  signal(SIGTERM, sharedaemon_signal);
  signal(SIGQUIT, sharedaemon_signal);
  signal(SIGINT, sharedaemon_signal);

  fd = shnet_sk();
  if (fd == -1) {
    perror("shsk");
    return (-1);
  }
  
  err = shnet_bindsk(fd, NULL, port);
  if (err) {
    perror("shbindport");
    shnet_close(fd);
    return (err);
  }

  listen_sk = fd;



  server_ledger = (sh_ledger_t *)calloc(1, sizeof(sh_ledger_t));

	server_peer = shpeer_init("shared", NULL, 0);
//	printf ("Server regsistered as peer '%x'.\n", shkey_print(&server_peer->name));

//	server_account = sharedaemon_account();
//	printf ("Using server account '%s'.\n", server_account->hash);
//	printf ("Using user identity '%s'.\n", server_account->id.hash);

  share_server(argv[0], PROC_SERVE);




  return (0);
}


