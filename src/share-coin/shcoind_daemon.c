
#include "shcoind.h"
#include <signal.h>

#define DAEMON_PORT 9448

user_t *client_list;
static int server_fd;

void daemon_close_clients(void)
{
  user_t *user;

  for (user = client_list; user; user = user->next) {
    if (user->fd == -1)
      continue;
    close(user->fd);
    user->fd = -1;
  }

}

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
user_t *register_client(int fd)
{
  user_t *user;
  int err;

  err = shnet_fcntl(fd, F_SETFL, O_NONBLOCK);
  if (err) {
    shnet_close(fd);
    return (NULL);
  }

  user = stratum_user_init(fd);
  user->next = client_list;
  client_list = user;

  return (user);
}

int register_client_task(user_t *user, char *json_text)
{
  shjson_t *tree;
  int err;

  if (!*json_text) {
fprintf(stderr, "DEBUG: empty JSON message.\n");
    return (0);
}

  tree = shjson_init(json_text);
  if (tree == NULL) {
fprintf(stderr, "DEBUG: unknown JSON:\n%s\n", json_text);
    return (SHERR_INVAL);
  }

//fprintf(stderr, "DEBUG: stratum_request_message: %s\n", json_text);
  err = stratum_request_message(user, tree);
  shjson_free(&tree);

  return (err);
}

void daemon_server(void)
{
  user_t *peer;
  user_t *peer_last;
  user_t *peer_next;
  fd_set read_set;
  fd_set write_set;
shbuf_t *buff;
  char *data;
  size_t len;
  double work_t;
  double cur_t;
  int fd_max;
  int cli_fd;
  int fd;
  int err;


  signal(SIGHUP, SIG_IGN);
  signal(SIGPIPE, SIG_IGN);
  signal(SIGTERM, daemon_signal);
  signal(SIGQUIT, daemon_signal);
  signal(SIGINT, daemon_signal);
 
  server_fd = shnet_sk();
  if (server_fd == -1) {
    perror("shsk");
    return;
  }

  err = shnet_bindsk(server_fd, NULL, DAEMON_PORT);
  if (err) {
    perror("shbindport");
    shnet_close(server_fd);
    return;
  }

  fprintf (stderr, "Accepting stratum connections on port %d.\n", DAEMON_PORT);

  work_t = shtime();
  while (server_fd != -1) {
    double start_t, diff_t;
    struct timeval to;

    start_t = shtime();

    peer_last = NULL;
    for (peer = client_list; peer; peer = peer_next) {
      peer_next = peer->next;

      if (peer->fd != -1 || (peer->flags & USER_SYSTEM)) {
        peer_last = peer;
        continue;
      }

      if (!peer_last) {
        client_list = peer_next;
      } else {
        peer_last->next = peer_next;
      }
      free(peer);
    }


    cli_fd = shnet_accept_nb(server_fd);
    if (cli_fd < 0 && cli_fd != SHERR_AGAIN) {
      perror("shnet_accept");
    } else if (cli_fd > 0) {
      struct sockaddr_in *addr = shnet_host(cli_fd);
      printf ("Received new connection on port %d (%s).\n", DAEMON_PORT, inet_ntoa(addr->sin_addr));
      register_client(cli_fd);
    }

    for (peer = client_list; peer; peer = peer->next) {
      if (peer->fd == -1)
        continue;

      buff = shnet_read_buf(peer->fd);
      if (!buff) {
        perror("shnet_write");
        shnet_close(peer->fd);
        peer->fd = -1;
        continue;
      }


      len = shbuf_idx(buff, '\n');
      if (len == -1)
        continue;
      data = shbuf_data(buff);
      data[len] = '\0';
      register_client_task(peer, data);
      shbuf_trim(buff, len + 1);
    }

    for (peer = client_list; peer; peer = peer->next) {
      if (peer->fd == -1)
        continue;

      /* flush writes */
      len = shnet_write_flush(peer->fd);
      if (len == -1) {
        perror("shnet_write");
        shnet_close(peer->fd);
        peer->fd = -1;
        continue;
      }
    }

    /* once per x1 seconds */
    cur_t = shtime();
    if (cur_t - 1.0 > work_t) {
      stratum_task_gen();
      work_t = cur_t;
    }

    diff_t = (shtime() - start_t);
    diff_t = MAX(0, 20 - (diff_t * 1000));
    memset(&to, 0, sizeof(to));
    to.tv_usec = (1000 * diff_t);
    if (to.tv_usec > 1000) {
      select(1, NULL, NULL, NULL, &to);
    }
  }

  fprintf (stderr, "Shutting down daemon.\n");

  if (server_fd != -1) {
    shnet_close(server_fd);
    server_fd = -1;
  }


  /* terminate cgb server */
  server_shutdown();

  /* close block fs */
  block_close();

}


