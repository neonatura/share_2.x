

#include "shcgbd.h"

shpeer_t *server_peer;


int main(int argc, char *argv[])
{
	shpeer_t *peer;
int err; 

	server_peer = shpeer();
	printf ("Server registered as peer '%s'.\n", shkey_print(&server_peer->name));

  err = shfs_proc_lock(argv[0], "daemon");
  if (err) {
    printf ("Terminating.. server '%s' is already running.\n", argv[0]);
    return;
  }


  load_wallet();
  printf ("Loaded wallet..\n");

  start_node();
  printf ("Started coin daemon..\n");

  daemon_server();

  return (0);
}


