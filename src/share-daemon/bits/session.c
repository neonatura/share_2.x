

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

#include "sharedaemon.h"


int remote_session_inform(tx_session_t *sess)
{
  generate_transaction_id(TX_SESSION, &sess->tx, NULL);
  sched_tx(sess, sizeof(tx_session_t));
}
int global_session_confirm(tx_session_t *sess)
{
  tx_account_t *acc;
  tx_id_t *id;
  char buf[MAX_SHARE_HASH_LENGTH];
  int err;

  strcpy(buf, shkey_hex(&sess->sess_id));
  id = (tx_id_t *)pstore_load(TX_IDENT, buf);
  if (!id)
    return (SHERR_INVAL);

  acc = (tx_account_t *)pstore_load(TX_ACCOUNT, shcrcstr(id->id_uid));
  if (!acc)
    return (SHERR_INVAL);

  err = shpam_sess_verify(&sess->sess_key, &acc->pam_seed.seed_key, sess->sess_stamp, &id->id_key); 
  if (err)
    return (err);

  return (0);
}

static int _generate_session_shadow(tx_id_t *id, tx_session_t *sess)
{
  shfs_t *fs;
  shfs_ino_t *shadow_file;
  int err;

  fs = shfs_init(&id->id_peer);
  shadow_file = shpam_shadow_file(fs);
  err = shpam_shadow_session_set(shadow_file, id->id_uid, &id->id_key, sess->sess_stamp, &sess->sess_key);
  shfs_free(&fs);
  if (err)
    return (err);

  return (0);
}

int local_session_generate(tx_id_t *id, shtime_t sess_stamp, tx_session_t **sess_p)
{
  shkey_t *id_key = &id->id_key;
  tx_account_t *acc;
  tx_session_t *l_sess;
  tx_session_t *sess;
  shkey_t *key;
  shtime_t now;
  int err;

  now = shtime();
  if (now > sess_stamp ||
      shtime_adj(now, MAX_SHARE_SESSION_TIME) < sess_stamp)
    return (SHERR_INVAL);

  acc = (tx_account_t *)pstore_load(TX_ACCOUNT, shcrcstr(id->id_uid));
  if (!acc)
    return (SHERR_INVAL);

  key = shpam_sess_gen(&acc->pam_seed.seed_key, sess_stamp, id_key);
  if (!key)
    return (SHERR_INVAL);

  l_sess = (tx_session_t *)pstore_load(TX_SESSION, (char *)shkey_hex(id_key));
  if (l_sess) {
    if (shkey_cmp(&l_sess->sess_key, key) &&
        l_sess->sess_stamp == sess_stamp) {
      err = global_session_confirm(l_sess); /* already exists */
      if (err) {
        pstore_free(l_sess);
        return (err);
      }

      *sess_p = l_sess;
      return (0);
    }

    /* prep for new identity session */
    pstore_remove(TX_SESSION, l_sess->sess_tx.hash, l_sess);
    pstore_free(l_sess);
  }

  sess = (tx_session_t *)calloc(1, sizeof(tx_session_t));
  if (!sess) {
    err = SHERR_NOMEM;
    goto error;
  } 

  memset(sess, 0, sizeof(tx_session_t));
  sess->sess_stamp = sess_stamp;
  memcpy(&sess->sess_id, id_key, sizeof(shkey_t));
  memcpy(&sess->sess_key, key, sizeof(shkey_t));
  shkey_free(&key);

  err = generate_transaction_id(TX_SESSION, &sess->sess_tx, NULL);
  if (err)
    goto error;
 

  err = _generate_session_shadow(id, sess);
  if (err)
    goto error;

  generate_transaction_id(TX_SESSION, &sess->sess_tx, NULL);

  err = global_session_confirm(sess);
  if (err) {
    goto error;
}

  remote_session_inform(sess);
  pstore_save(sess, sizeof(tx_session_t));

  if (sess_p) {
    *sess_p = sess;
  } else {
    pstore_free(sess);
  }

  return (0);

error:
  sherr(err, "generate session");
  pstore_free(sess);
  shkey_free(&key);
  return (err);
}

int local_session_inform(tx_app_t *cli, tx_session_t *session)
{
  tx_session_t *ent;
  int err;

  ent = (tx_session_t *)pstore_load(TX_SESSION, session->sess_tx.hash);
  if (!ent) {
    err = global_session_confirm(session);
    if (err)
      return (err);

    pstore_save(session, sizeof(tx_session_t));
  }

  return (0);
}


