

#include "shcoind.h"
#include <signal.h>

shpeer_t *server_peer;
int server_fd;

void daemon_signal(int sig_num)
{
  signal(sig_num, SIG_DFL);

  block_close();
  daemon_close_clients();
  if (server_fd != -1) {
    shnet_close(server_fd);
    server_fd = -1;
  }
}

int main(int argc, char *argv[])
{
  int fd;
  int err;

  daemon(0, 1);

  signal(SIGHUP, SIG_IGN);
  signal(SIGPIPE, SIG_IGN);
  signal(SIGTERM, daemon_signal);
  signal(SIGQUIT, daemon_signal);
  signal(SIGINT, daemon_signal);
 
  fd = shnet_sk();
  if (fd == -1) {
    perror("shnet_sk");
    return (-1);
  }

  err = shnet_bindsk(fd, NULL, DAEMON_PORT);
  if (err) {
    perror("shbindport");
    shnet_close(fd);
    return (err);
  }

  server_fd = fd;
  block_init();
  load_wallet();
  start_node();
  daemon_server();

  return (0);
}


