

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
  int err;

  app_key = shpeer_kpub(peer);
  app = sharedaemon_app_load(app_key);
  if (!app) {
    app = init_app(app_key, peer);
    if (!app)
      return (SHERR_NOMEM);

    err = sharedaemon_app_save(app);
    if (err)
      return (err);
  }

  err = confirm_app(app);
  if (err)
    return (err);

  cli->app = app;
  cli->flags |= SHD_CLIENT_REGISTER;
  memcpy(&cli->peer, peer, sizeof(shpeer_t));

  return (0);
}


