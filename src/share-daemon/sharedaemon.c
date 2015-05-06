
/*
 *  Copyright 2013 Neo Natura 
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
#include "sharedaemon.h"
#include <sys/types.h>
#include <signal.h>

shpeer_t *server_peer;
tx_ledger_t *server_ledger;
int shlogd_pid;

void sharedaemon_term(void)
{
  int err;

  if (listen_sk) {
    shnet_close(listen_sk);
    listen_sk = 0;
  }

  cycle_term();
  shpeer_free(&server_peer);
}

void sharedaemon_signal(int sig_num)
{
  signal(sig_num, SIG_DFL);

  sharedaemon_term();
  raise(sig_num);
}

static void sharedaemon_print_usage(void)
{

  printf(
    "shared: A libshare suite transaction daemon.\n"
    "\n"
    "Usage: shared\n"
    "\n"
    "Visit http://docs.sharelib.net for additional documentation.\n"
    "Report bugs to http://bugs.sharelib.net/ or <support@neo-natura.com>.\n"
    );
}

static void sharedaemon_print_version(void)
{

  printf(
    "shared version %s\n"
    "\n"
    "Copyright 2013 Neo Natura\n"
    "Licensed under the GNU GENERAL PUBLIC LICENSE Version 3\n",
    get_libshare_version()
    );
}

int main(int argc, char *argv[])
{
  unsigned int port = (unsigned int)SHARE_DAEMON_PORT;
	shpeer_t *peer;
  char buf[256];
  int err;
  int fd;
  int i;

  for (i = 1; i < argc; i++) {
    if (0 == strcmp(argv[i], "--version") ||
        0 == strcmp(argv[i], "-v")) {
      sharedaemon_print_version();
      return (0);
    }
    if (0 == strcmp(argv[i], "--help") ||
        0 == strcmp(argv[i], "-h")) {
      sharedaemon_print_usage();
      return (0);
    }
  }

#ifndef DEBUG
  /* fork shlogd */
  if (0 == strcmp(shpref_get("daemon.shlogd", SHPREF_TRUE), SHPREF_TRUE)) {
    shlogd_pid = fork();
    switch (shlogd_pid) {
      case 0:
        /* shlogd process */
        memset(argv[0], '\0', strlen(argv[0]));
        strcpy(argv[0], "shlogd");
fprintf(stderr, "DEBUG: fork'd shlogd pid %d\n", getpid());
        return (shlogd_main(argc, argv));
      case -1:
        sherr(-errno, "shlogd spawn");
        break;
      default:
        sprintf(buf, "spawned shlogd daemon (pid %d)", shlogd_pid);
        shinfo(buf);
        break;
    }
  }
#endif

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



  server_ledger = (tx_ledger_t *)calloc(1, sizeof(tx_ledger_t));

	server_peer = shapp_init("shared", NULL, SHAPP_LOCAL);

//	server_account = sharedaemon_account();
//	printf ("Using server account '%s'.\n", server_account->hash);
//	printf ("Using user identity '%s'.\n", server_account->id.hash);

  share_server(argv[0], PROC_SERVE);

  shpeer_free(&server_peer);


  return (0);
}


