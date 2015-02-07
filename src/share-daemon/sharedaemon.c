
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

int main(int argc, char *argv[])
{
  unsigned int port = (unsigned int)SHARE_DAEMON_PORT;
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



  server_ledger = (tx_ledger_t *)calloc(1, sizeof(tx_ledger_t));

	server_peer = shapp_init("shared", NULL, SHAPP_LOCAL);

//	server_account = sharedaemon_account();
//	printf ("Using server account '%s'.\n", server_account->hash);
//	printf ("Using user identity '%s'.\n", server_account->id.hash);

  share_server(argv[0], PROC_SERVE);

  shpeer_free(&server_peer);


  return (0);
}


