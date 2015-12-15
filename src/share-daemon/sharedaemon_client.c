
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

int sharedaemon_netclient_add(int fd, shpeer_t *peer)
{
  shd_t *cli;
  int err;

  cli = sharedaemon_client_init();
  if (!cli)
    return (SHERR_NOMEM);

  cli->flags |= SHD_CLIENT_NET; 
  cli->cli.net.fd = fd;

  err = sharedaemon_app_init(cli, peer);
  if (err)
    return (err);

  return (0);
}

int sharedaemon_netclient_init(int fd, struct sockaddr_in *net_addr)
{
  shpeer_t *peer;
  char hostname[MAXHOSTNAMELEN+1];
  int err;

  if (!net_addr)
    return (SHERR_INVAL);

  sprintf(hostname, "%s:%d",
      inet_ntoa(net_addr->sin_addr), ntohs(net_addr->sin_port));
  peer = shpeer_init("shared", hostname);
  err = sharedaemon_netclient_add(fd, peer);
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

 if (cli->cli.net.fd)
    close(cli->cli.net.fd);
  cli->cli.net.fd = 0;

  shbuf_free(&cli->buff_out);
  shbuf_free(&cli->buff_in);

  free(cli);

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
  shpeer_t *peer;
  char hostname[MAXHOSTNAMELEN+1];
  int err;
  int fd;

fprintf(stderr, "DEBUG: sharedaemon_netclient_init: net_addr(%s) net_peer(%s)\n", inet_ntoa(net_addr->sin_addr), shpeer_print(net_peer));

//  memcpy(&net_peer->addr.sin_addr[0], net_addr->sin_addr, sizeof(net_addr->sin_addr));
  sprintf(hostname, "%s:%d", 
      inet_ntoa(net_addr->sin_addr), ntohs(net_peer->addr.sin_port)); 
  peer = shpeer_init(shpeer_get_app(net_peer), hostname);
#if 0
  fd = shconnect_peer(peer, SHNET_ASYNC | SHNET_TRACK);
  if (fd < 0)
    return (-errno); /* refused immediately / error state */
 
  /* add 'er to the list */
  err = sharedaemon_netclient_add(fd, peer);
  if (err) {
    close(fd);
    return (err);
  }
#endif

  err = peer_add(peer);
  shpeer_free(&peer);
  if (err)
    return (err);

  return (0);
}


