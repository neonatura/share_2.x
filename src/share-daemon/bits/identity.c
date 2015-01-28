
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


int confirm_identity(tx_id_t *id)
{
  shkey_t *key;
  int confirm;
  int err;

  /* ensure id key is derived from identity's base attributes */
  key = shpam_ident_gen(&id->id_peer, &id->id_seed, id->id_label);
  confirm = shkey_cmp(&id->id_key, key);
  shkey_free(&key);
  if (!confirm) {
fprintf(stderr, "DEBUG: ERROR: confirm_dentity: key compare fail.\n");
    return (SHERR_INVAL);
}

#if 0
  err = confirm_signature(&id->id_sig, id->id_tx.hash);
  if (err) {
fprintf(stderr, "DEBUG: confirm_identity: %d = confirm_signature()\n", err);
    return (err);
  }
#endif

fprintf(stderr, "DEBUG: confirm_identify: SCHED-TX: %s\n", id->id_tx.hash);
  generate_transaction_id(TX_IDENT, &id->tx, NULL);
  sched_tx(id, sizeof(tx_id_t));

  return (0);
}

static int _generate_identity_shadow_create(tx_id_t *id)
{
  shfs_t *fs;
  shfs_ino_t *shadow_file;
  int err;

  fs = shfs_init(&id->id_peer);
  shadow_file = shpam_shadow_file(fs);

  err = shpam_shadow_verify(shadow_file, &id->id_seed);
  if (err == SHERR_NOKEY) {
    err = shpam_shadow_create(shadow_file, &id->id_seed, id->id_label, NULL); 
fprintf(stderr, "DEBUG: _generate_identity_shadow_create: %d = shpam_shadow_create(peer %s, seed %s, label '%s')\n", err, shpeer_print(&id->id_peer), shkey_print(&id->id_seed), id->id_label);
  }

  shfs_free(&fs);

  return (err);
}

#if 0
/**
 * Generate a new identity using a seed value.
 */
int generate_identity_tx(tx_id_t *id, shkey_t *seed_key, shpeer_t *app_peer, char *id_label)
{
  tx_id_t *l_id;
  shkey_t *key;
  int err;

	if (!app_peer)
		app_peer = ashpeer();

  memset(id, 0, sizeof(tx_id_t));
  if (app_peer)
    memcpy(&id->id_peer, app_peer, sizeof(shpeer_t));
  if (seed_key)
    memcpy(&id->id_seed, seed_key, sizeof(shkey_t));
  if (id_label)
    strncpy(id->id_label, id_label, sizeof(id->id_label) - 1);

  key = shpam_ident_gen(app_peer, seed_key, id_label);
  memcpy(&id->id_key, key, sizeof(shkey_t));
  shkey_free(&key);

  l_id = (tx_id_t *)pstore_load(TX_IDENT, (char *)shkey_hex(&id->id_key));
  if (l_id) {
    if (0 == strcmp(id->id_label, l_id->id_label)) {
      err = confirm_identity(l_id);
      if (!err) {
        memcpy(id, l_id, sizeof(tx_id_t));
        pstore_free(l_id);
      }
      return (err);
    }
    pstore_free(l_id);
  }

  err = generate_transaction_id(TX_IDENT, &id->id_tx, NULL);
  if (err)
    return (err);

  err = confirm_identity(id);
  if (err)
    return (err);

  err = _generate_identity_shadow_create(id); 
  if (err)
    return (err);

  pstore_save(id, sizeof(tx_id_t));
  return (0);
}
#endif

tx_id_t *generate_identity(shkey_t *seed_key, shpeer_t *app_peer, char *id_label)
{
  tx_id_t *l_id;
  tx_id_t *id;
  shkey_t *key;
  int err;

fprintf(stderr, "DEBUG: generate_identity: seed_key %s\n", shkey_print(seed_key));
fprintf(stderr, "DEBUG: generate_identity: app_peer %s\n", shpeer_print(app_peer));
fprintf(stderr, "DEBUG: generate_identity: id_label '%s'\n", id_label);

  /* lookup id key in case of existing record */
  key = shpam_ident_gen(app_peer, seed_key, id_label);
fprintf(stderr, "DEBUG: generate_identity: id_key %s\n", shkey_print(key));
  l_id = (tx_id_t *)pstore_load(TX_IDENT, (char *)shkey_hex(key));
  if (l_id) {
    shkey_free(&key);

    err = confirm_identity(l_id);
    if (err) {
      return (NULL);
    }

    return (l_id);
  }

  id = (tx_id_t *)calloc(1, sizeof(tx_id_t));
  if (!id) {
    shkey_free(&key);
    return (NULL);
  }

  memset(id, 0, sizeof(tx_id_t));
  if (app_peer)
    memcpy(&id->id_peer, app_peer, sizeof(shpeer_t));
  if (seed_key)
    memcpy(&id->id_seed, seed_key, sizeof(shkey_t));
  if (id_label)
    strncpy(id->id_label, id_label, sizeof(id->id_label) - 1);
  memcpy(&id->id_key, key, sizeof(shkey_t));
  shkey_free(&key);

  err = confirm_identity(id);
  if (err)
    return (err);

  err = _generate_identity_shadow_create(id); 
  if (err)
    return (err);

  generate_transaction_id(TX_IDENT, &id->id_tx, NULL);
  pstore_save(id, sizeof(tx_id_t));

  return (id);
}

int process_identity_tx(tx_app_t *cli, tx_id_t *id)
{
  tx_id_t *ent;
  int err;

  ent = (tx_id_t *)pstore_load(TX_IDENT, id->id_tx.hash);
  if (!ent) {
    err = confirm_identity(id);
    if (err)
      return (err);

    pstore_save(id, sizeof(tx_id_t));
  }

  return (0);
}




