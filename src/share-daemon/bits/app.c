

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

#define MINIMUM_APP_TRUST 0
#define MAXIMUM_APP_TRUST 1000000000


int confirm_app(tx_app_t *app)
{
  tx_app_t app_data;
  shsig_t sig;
  uint64_t crc;
  int err;

  memset(&sig, 0, sizeof(sig));
  memcpy(&sig.sig_peer, shpeer_kpriv(&app->app_peer), sizeof(shkey_t));
  memcpy(&sig.sig_key, &app->app_sig, sizeof(shkey_t));
  sig.sig_stamp = app->app_birth;
  err = confirm_signature(&sig, app->app_tx.hash);
  if (err) {
    return (err);
}

  memcpy(&app_data, app, sizeof(tx_app_t));

  app->app_stamp = shtime();
  local_transid_generate(TX_APP, &app_data.app_tx);
  sched_tx(&app_data, sizeof(tx_app_t));

  return (0);
}

static int _generate_app_tx(tx_app_t *app, shpeer_t *peer)
{
  shsig_t sig;

  memcpy(&app->app_peer, peer, sizeof(shpeer_t));
  app->app_arch = peer->arch;

  memset(&app->app_tx, 0, sizeof(app->app_tx));
  generate_transaction_id(TX_APP, &app->app_tx, NULL);

  memset(&sig, 0, sizeof(sig));
  generate_signature(&sig, peer, &app->app_tx);
  app->app_birth = sig.sig_stamp;
  memcpy(&app->app_sig, &sig.sig_key, sizeof(shkey_t));

  return (0);
}

tx_app_t *init_app(shpeer_t *peer)
{
  tx_app_t *app;
  int err;

  app = (tx_app_t *)pstore_load(TX_APP, shpeer_kpriv(peer));
  if (!app) {
    app = (tx_app_t *)calloc(1, sizeof(tx_app_t));
    if (!app)
      return (NULL);

    err = _generate_app_tx(app, peer);
    if (err) {
      free(app);
      return (NULL);
    }
  }

  err = confirm_app(app);
  if (err)
    return (NULL);

  return (app);
}

int process_app_tx(tx_app_t *app)
{
  tx_app_t *ent;
  int err;

  ent = (tx_app_t *)pstore_load(TX_APP, app->app_tx.hash);
  if (!ent) {
    err = confirm_app(app);
    if (err)
      return (err);

    pstore_save(app, sizeof(tx_app_t));
  }

  return (0);
}

void decr_app_trust(tx_app_t *cli)
{
  uint32_t trust = ntohl(cli->app_trust);

  if (trust > MINIMUM_APP_TRUST)
    trust = trust - 1;

  cli->app_trust = trust;
}

void incr_app_trust(tx_app_t *cli)
{
  uint32_t trust = ntohl(cli->app_trust);

  if (trust < MAXIMUM_APP_TRUST)
    trust = trust + 1;

  cli->app_trust = trust;
}

