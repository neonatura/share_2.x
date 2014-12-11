
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

#include <share.h>
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

int sharedaemon_netclient_init(int fd, struct sockaddr_in *net_addr)
{
  shpeer_t *app_peer;
  shpeer_t *net_peer;
  shd_t *cli;
  char hostname[MAXHOSTNAMELEN+1];
  int err;

fprintf(stderr, "DEBUG: sharedaemon_netclient_init: fd:%d net_addr:%s\n", fd, inet_ntoa(net_addr->sin_addr));

  cli = sharedaemon_client_init();
  if (!cli)
    return (SHERR_NOMEM);

  cli->flags |= SHD_CLIENT_NET; 
  cli->cli.net.fd = fd;

  app_peer = shpeer_init("shared", FALSE, 0);
  sprintf(hostname, "%s:%d", inet_ntoa(net_addr->sin_addr), 32080);  
  net_peer = shpeer_init("shared", hostname, 0);
  err = sharedaemon_app_init(cli, &app_peer->name, net_peer);
  shpeer_free(&app_peer);
  shpeer_free(&net_peer);
  if (err)
    return (err);

  return (0);
}

int sharedaemon_msgclient_init(shkey_t *app_key, shpeer_t *priv_peer)
{
  shd_t *cli;
  int err;

  cli = sharedaemon_client_init();
  if (!cli)
    return (SHERR_NOMEM);

  cli->flags |= SHD_CLIENT_MSG;

  err = sharedaemon_app_init(cli, app_key, priv_peer);
  if (err)
    return (err);


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


