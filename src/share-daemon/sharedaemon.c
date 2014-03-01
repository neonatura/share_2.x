
#include "sharedaemon.h"

shpeer_t *server_peer;
sh_account_t *server_account;
sh_ledger_t *server_ledger;


int main(int argc, char *argv[])
{
	sh_account_t *acc;
	shpeer_t *peer;

server_ledger = (sh_ledger_t *)calloc(1, sizeof(sh_ledger_t));
	server_peer = sharedaemon_peer();
	printf ("Server regsistered as peer '%x'.\n", shkey_print(&server_peer->name));

	server_account = sharedaemon_account();
	printf ("Using server account '%s'.\n", server_account->hash);
	printf ("Using user identity '%s'.\n", server_account->id.hash);

  share_server(argv[0], PROC_SERVE);

  return (0);
}


