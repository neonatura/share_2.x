

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
  shsig_t *sig;
  uint64_t crc;
  int err;

#if 0
  crc = (uint64_t)strtoll(app->app_tx.hash, NULL, 16);
  err = shkey_verify(&app->app_sig, crc, &peer->name, app->app_stamp);
#endif
  err = verify_signature(&app->app_sig, app->app_tx.hash, peer, app->app_stamp);
  if (err)
    return (err);

  memset(&app->tx, 0, sizeof(app->tx));
  generate_transaction_id(&app->tx, NULL);
  sched_tx(app, sizeof(sh_app_t));

  return (0);
}


sh_app_t *init_app(shkey_t *pub_key, shpeer_t *priv_peer)
{
  sh_app_t *app;
  shkey_t *sig_key;
  shsig_t sig;
  uint64_t crc;

  app = (sh_app_t *)calloc(1, sizeof(sh_app_t));
  if (!app)
    return (NULL);

  memcpy(&app->app_name, pub_key, sizeof(shkey_t));

  memset(&app->app_tx, 0, sizeof(app->app_tx));
  generate_transaction_id(&app->app_tx, NULL);

  memset(&sig, 0, sizeof(sig));
  generate_signature(&sig, priv_peer, &app->app_tx);
  app->app_stamp = sig.sig_stamp;
  memcpy(&app->app_sig, &sig.sig_key, sizeof(shkey_t));

  memset(&app->app_id, 0, sizeof(app->app_id));
  generate_identity_id(&app->app_id, &priv_peer->name);

  app->app_arch = priv_peer->arch;

  return (app);
}

