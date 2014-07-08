

/*
 * @copyright
 *
 *  Copyright 2013 Brian Burrell 
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
 *
 *  @endcopyright
 */
#include "bits.h"



int confirm_app(sh_app_t *app, shpeer_t *peer)
{
  sh_sig_t *sig;
  int err;

  sig = find_transaction_signature(&app->app_tx);
  if (!sig)
    return (SHERR_NOENT);

  err = verify_signature(sig, peer, &app->app_tx, &app->app_id);
  if (err)
    return (err);

  sched_tx(&app, sizeof(sh_app_t));
  return (0);
}

/**
 * A trusted client is requesting a app on a transaction be created.
 */
int generate_app(sh_app_t *app, sh_tx_t *tx, sh_id_t *id)
{
  sh_app_t app;

  memset(&app, 0, sizeof(app));
  generate_transaction_id(&app.tx);
  memcpy(&app,app_tx, tx, sizeof(sh_tx_t));
  memcpy(&app,app_id, id, sizeof(sh_id_t));

  return (confirm_app(app, sharedaemon_peer()));
}



