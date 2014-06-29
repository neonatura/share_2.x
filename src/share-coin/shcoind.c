

#include "shcoind.h"

shpeer_t *server_peer;


int main(int argc, char *argv[])
{

  block_init();
  load_wallet();
  start_node();
  daemon_server();

  return (0);
}


