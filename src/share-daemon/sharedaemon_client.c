
/*
 *  Copyright 2014 Neo Natura
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

shd_t *sharedaemon_client_list;

shd_t *sharedaemon_client_init(void)
{
  shd_t *cli;

  cli = (shd_t *)calloc(1, sizeof(shd_t));
  cli->buff_out = shbuf_init();
  cli->buff_in = shbuf_init();

  cli->next = sharedaemon_client_list;
  sharedaemon_client_list = cli;

  return (cli);
}

int sharedaemon_netclient_add(int fd, shpeer_t *peer, int flags)
{
  shd_t *cli;
  int err;

  cli = sharedaemon_client_init();
  if (!cli)
    return (SHERR_NOMEM);

  cli->flags |= SHD_CLIENT_NET; 
  cli->cli.net.fd = fd;

  cli->birth = shtime();
  if ((flags & SHD_CLIENT_AUTH) || (flags & SHD_CLIENT_SHUTDOWN)) {
    /* wait for client to register */
    cli->flags |= SHD_CLIENT_AUTH;
  }

  err = sharedaemon_app_init(cli, peer);
fprintf(stderr, "DEBUG: sharedaemon_netclient_add: %d = sharedaemon_app_init()\n", err);
  if (err)
    return (err);

  return (0);
}

int sharedaemon_httpclient_add(int fd)
{
  shd_t *cli;
  int err;

  cli = sharedaemon_client_init();
  if (!cli)
    return (SHERR_NOMEM);

  cli->flags |= SHD_CLIENT_HTTP;
  cli->cli.net.fd = fd;

  cli->birth = shtime();

  return (0);
}

int sharedaemon_netclient_init(int fd, struct sockaddr *net_addr)
{
  shpeer_t *peer;
  int err;

  if (!net_addr)
    return (SHERR_INVAL);

  peer = shpeer_init("shared", (char *)shaddr_print(net_addr));
  err = sharedaemon_netclient_add(fd, peer, SHD_CLIENT_AUTH);
  shpeer_free(&peer);
  if (err)
    return (err);

  return (0);
}


int sharedaemon_msgclient_init(shpeer_t *peer)
{
  shkey_t *app_key = shpeer_kpub(peer);
  shd_t *cli;
  int err;

  cli = sharedaemon_client_find(app_key);
  if (!cli) {
    cli = sharedaemon_client_init();
    if (!cli)
      return (SHERR_NOMEM);

    cli->flags |= SHD_CLIENT_MSG;
  }

  err = sharedaemon_app_init(cli, peer);
  if (err)
    return (err);

  if (cli->app) {
    memcpy(&cli->cli.msg.msg_key, shpeer_kpub(&cli->app->app_peer), sizeof(shkey_t));
  }

  return (0);
}

void sharedaemon_client_free(shd_t **cli_p)
{
  shd_t *cli;

  if (!cli_p)
    return;
  cli = *cli_p;
  *cli_p = NULL;

 if (cli->cli.net.fd) {
    shclose(cli->cli.net.fd);
  }
  cli->cli.net.fd = 0;

  shbuf_free(&cli->buff_out);
  shbuf_free(&cli->buff_in);

  free(cli);

}

void sharedaemon_client_term(void)
{
  shd_t *cli_next;
  shd_t *cli;

  for (cli = sharedaemon_client_list; cli; cli = cli_next) {
    cli_next = cli->next;

    sharedaemon_client_free(&cli);
  } 

}

shd_t *sharedaemon_client_find(shkey_t *key)
{
  shd_t *cli;

  for (cli = sharedaemon_client_list; cli; cli = cli->next) {
    if ((cli->flags & SHD_CLIENT_REGISTER) &&
        0 == memcmp(shpeer_kpub(&cli->app->app_peer), key, sizeof(shkey_t))) {
      return (cli);
    }

    if ((cli->flags & SHD_CLIENT_MSG)) {
      if (0 == memcmp(&cli->cli.msg.msg_key, key, sizeof(shkey_t)))
        return (cli);
    }
  } 

  return (NULL);
}


int sharedaemon_netclient_conn(shpeer_t *net_peer, struct sockaddr_in *net_addr)
{
  tx_init_t ini;
  shpeer_t *peer;
  char hostname[MAXHOSTNAMELEN+1];
  int err;
  int fd;

//  memcpy(&net_peer->addr.sin_addr[0], net_addr->sin_addr, sizeof(net_addr->sin_addr));
  sprintf(hostname, "%s:%d", 
      inet_ntoa(net_addr->sin_addr), ntohs(net_peer->addr.sin_port)); 
  peer = shpeer_init(shpeer_get_app(net_peer), hostname);
  fd = shconnect_peer(peer, SHNET_ASYNC);// | SHNET_TRACK);
fprintf(stderr, "DEBUG: %d = shconnect_peer()\n", fd);
  if (fd < 0)
    return (fd); /* refused immediately / error state */
 
  /* add 'er to the list */
  err = sharedaemon_netclient_add(fd, peer, 0);
fprintf(stderr, "DEBUG: %d = sharedaemon_netclient_add\n", err);
  if (err) {
    close(fd);
    return (err);
  }

  err = peer_add(peer);
  shpeer_free(&peer);
  if (err) {
fprintf(stderr, "DEBUG: %d = peer_add()\n", err); 
    return (err);
  }

  /* initialize connection */
  memset(&ini, 0, sizeof(ini));
  memcpy(&ini.ini_peer, net_peer, sizeof(ini.ini_peer));
  sched_tx_sink(shpeer_kpriv(peer), &ini, sizeof(ini));
fprintf(stderr, "DEBUG: sharedaemon_netclient_conn: ini_peer '%s'\n", shpeer_print(&ini.ini_peer)); 

  return (0);
}


int sharedaemon_client_count(struct sockaddr *net_addr)
{
  shd_t *cli;
  char host[MAXHOSTNAMELEN+1];
  char cmp_host[MAXHOSTNAMELEN+1];
  struct sockaddr sa;
  int total;

  memset(host, 0, sizeof(host));
  getnameinfo(net_addr, sizeof(struct sockaddr),
      host, sizeof(host)-1, NULL, NULL, 0);

  total = 0;
  for (cli = sharedaemon_client_list; cli; cli = cli->next) {
    if (cli->cli.net.fd == 0)
      continue;
    if (!(cli->flags & SHD_CLIENT_NET) &&
        !(cli->flags & SHD_CLIENT_HTTP))
      continue;

    memcpy(&sa, shpeer_addr(&cli->peer), sizeof(sa));

    memset(cmp_host, 0, sizeof(cmp_host));
    getnameinfo(&sa, sizeof(sa),
        cmp_host, sizeof(cmp_host)-1, NULL, NULL, 0);
    if (0 == strcmp(host, cmp_host))
      total++;
  } 

  return (total);
}
