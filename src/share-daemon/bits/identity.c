
/*
 * @copyright
 *
 *  Copyright 2013, 2014 Neo Natura
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

#include "sharedaemon.h"


int global_identity_confirm(tx_id_t *id)
{
  shkey_t *key;
  int confirm;
  int err;

  /* ensure id key is derived from identity's base attributes */
  key = shpam_ident_gen(id->id_uid, &id->id_peer);
  confirm = shkey_cmp(&id->id_key, key);
  shkey_free(&key);
  if (!confirm) {
    return (SHERR_INVAL);
}

#if 0
  err = confirm_signature(&id->id_sig, id->id_tx.hash);
  if (err) {
    return (err);
  }
#endif

  return (0);
}

int remote_identity_inform(tx_id_t *id)
{
  int err;

  err = global_identity_confirm(id);
  if (err)
    return (err);

  err = generate_transaction_id(TX_IDENT, &id->tx, NULL);
  if (err)
    return (err);

  sched_tx(id, sizeof(tx_id_t));
  return (0);
}

static int local_identity_shadow_generate(tx_id_t *id)
{
  shfs_t *fs;
  shfs_ino_t *shadow_file;
  int err;

  fs = shfs_init(&id->id_peer);
  shadow_file = shpam_shadow_file(fs);

  err = shpam_shadow_load(shadow_file, id->id_uid, NULL);
  if (err == SHERR_NOKEY) {
    err = shpam_shadow_create(shadow_file, id->id_uid, NULL); 
  }

  shfs_free(&fs);

  return (err);
}

int local_identity_generate(uint64_t uid, shpeer_t *app_peer, tx_id_t **id_p)
{
  tx_id_t *l_id;
  tx_id_t *id;
  shkey_t *key;
  int err;

  /* lookup id key in case of existing record */
  key = shpam_ident_gen(uid, app_peer);
  l_id = (tx_id_t *)pstore_load(TX_IDENT, (char *)shkey_hex(key));
  if (l_id) {
    shkey_free(&key);

    err = global_identity_confirm(l_id);
    if (err)
      return (err);

    *id_p = l_id;
    return (0);
  }

  id = (tx_id_t *)calloc(1, sizeof(tx_id_t));
  if (!id) {
    shkey_free(&key);
    return (SHERR_NOMEM);
  }

  memset(id, 0, sizeof(tx_id_t));
  if (app_peer)
    memcpy(&id->id_peer, app_peer, sizeof(shpeer_t));
  id->id_uid = uid;
  memcpy(&id->id_key, key, sizeof(shkey_t));
  shkey_free(&key);

  err = local_identity_shadow_generate(id);
  if (err)
    return (err);

  generate_transaction_id(TX_IDENT, &id->id_tx, NULL);

  err = remote_identity_inform(id);
  if (err)
    return (err);

  pstore_save(id, sizeof(tx_id_t));

  *id_p = id;
  return (0);
}

int local_identity_inform(tx_app_t *cli, tx_id_t *id)
{
  tx_id_t *ent;
  int err;

  ent = (tx_id_t *)pstore_load(TX_IDENT, id->id_tx.hash);
  if (!ent) {
    /* broadcast to relevant peers */
    err = remote_identity_inform(id);
    if (err)
      return (err);

    pstore_save(id, sizeof(tx_id_t));
  }

  return (0);
}




