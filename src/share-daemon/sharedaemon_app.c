

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

tx_app_t *sharedaemon_app_load(shkey_t *app_key)
{
  return (NULL);
}
int sharedaemon_app_save(tx_app_t *app)
{
  return (0);
}

/**
 * initializes a libshare runtime enabled process application
 */
int sharedaemon_app_init(shd_t *cli, shpeer_t *peer)
{
  tx_t tx;
  tx_id_t id;
  shkey_t *app_key;
  shsig_t sig;
  tx_app_t *app;
  int flags = cli->flags;
  int err;

fprintf(stderr, "DEBUG: sharedaemon_app_init: peer '%s'\n", shpeer_print(peer));

  app_key = shpeer_kpub(peer);
  app = sharedaemon_app_load(app_key);
  if (!app) {
    app = init_app(peer);
    if (!app)
      return (SHERR_NOMEM);

    err = sharedaemon_app_save(app);
    if (err)
      return (err);
  }

  if (!(flags & SHD_CLIENT_AUTH) && !(flags & SHD_CLIENT_SHUTDOWN)) {
    err = confirm_app(app);
    if (err) {
fprintf(stderr, "DEBUG: %d = confirm_app()\n", err);
      return (err);
}

    cli->flags |= SHD_CLIENT_REGISTER;
  }

  cli->app = app;
  memcpy(&cli->peer, peer, sizeof(shpeer_t));

  return (0);
}


