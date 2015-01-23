

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


int confirm_session(tx_session_t *sess)
{
  tx_id_t *id;
  char buf[MAX_SHARE_HASH_LENGTH];
  int err;

  strcpy(buf, shkey_hex(&sess->sess_id));
  id = (tx_id_t *)pstore_load(TX_IDENT, buf);
  if (!id)
    return (SHERR_INVAL);

  err = shpam_sess_verify(&sess->sess_key, &id->id_seed, sess->sess_stamp, 
      shcrc(id->id_label, strlen(id->id_label)));
  if (err)
    return (err);

fprintf(stderr, "DEBUG: confirm_session; SCHED-TX: %s\n", sess->sess_tx.hash);
  generate_transaction_id(TX_SESSION, &sess->tx, NULL);
  sched_tx(sess, sizeof(tx_session_t));

  return (0);
}


/**
 * Create an session for an identity.
 * @param secs The number of seconds before the session expires.
 */
int generate_session_tx(tx_session_t *sess, tx_id_t *id, double secs)
{
  tx_session_t *l_sess;
  shkey_t *id_key = &id->id_key;
  shkey_t *key;
  int err;

  if (secs > MAX_SHARE_SESSION_TIME)
    return (SHERR_INVAL);

  memset(sess, 0, sizeof(tx_session_t));
  generate_transaction_id(TX_SESSION, &sess->sess_tx, NULL);
  sess->sess_stamp = shtime64_adj(shtime64(), secs);
  memcpy(&sess->sess_id, id_key, sizeof(shkey_t));
  key = shpam_sess_gen(&id->id_seed, sess->sess_stamp,
      shcrc(id->id_label, strlen(id->id_label)));
  if (key) {
    memcpy(&sess->sess_key, key, sizeof(shkey_t));
    shkey_free(&key);
  }
 
  err = confirm_session(sess);
  if (err)
    return (err);

  return (0);
}

tx_session_t *generate_session(tx_id_t *id, double secs)
{
  tx_session_t *sess;

  sess = (tx_session_t *)calloc(1, sizeof(tx_session_t));
  if (!sess)  
    return (NULL);
  generate_session_tx(sess, id, secs);
  return (sess);
}

int process_session_tx(tx_app_t *cli, tx_session_t *session)
{
  tx_session_t *ent;
  int err;

  ent = (tx_session_t *)pstore_load(TX_SESSION, session->sess_tx.hash);
  if (!ent) {
    err = confirm_session(session);
    if (err)
      return (err);

    pstore_save(session, sizeof(tx_session_t));
  }

  return (0);
}


