
/*
 * @copyright
 *
 *  Copyright 2016 Neo Natura
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



int inittx_ref(tx_ref_t *ref, tx_t *tx, char *name, char *hash, int type)
{
  shkey_t *tx_key;
  int err;

  tx_key = get_tx_key(tx);
  if (!tx_key)
    return (SHERR_INVAL);

  memset(ref->ref.ref_name, 0, sizeof(ref->ref.ref_name));
  memset(ref->ref.ref_hash, 0, sizeof(ref->ref.ref_hash));

  if (name)
    strncpy(ref->ref.ref_name, name, sizeof(ref->ref.ref_name)-1); 
  if (hash)
    strncpy(ref->ref.ref_hash, hash, sizeof(ref->ref.ref_hash)-1); 
  ref->ref_type = type;

  memcpy(&ref->ref.ref_peer, &tx->tx_peer, sizeof(ref->ref.ref_peer));
  memcpy(&ref->ref_txkey, tx_key, sizeof(ref->ref_txkey));
  ref->ref_op = tx->tx_op;

  err = tx_init(NULL, (tx_t *)ref, TX_REFERENCE);
  if (err)
    return (err);

  return (0);
}

tx_ref_t *alloc_ref(tx_t *tx, char *name, char *hash, int type)
{
  tx_ref_t *ref;
  int err;

  ref = (tx_ref_t *)calloc(1, sizeof(tx_ref_t));
  if (!ref)
    return (NULL);

  err = inittx_ref(ref, tx, name, hash, type);
  if (err)
    return (NULL);

  return (ref);
}


int txop_ref_init(shpeer_t *cli_peer, tx_ref_t *ref)
{


  return (0);
}

int txop_ref_confirm(shpeer_t *peer, tx_ref_t *ref)
{

  return (0);
}

int txop_ref_send(shpeer_t *peer, tx_ref_t *ref)
{
  return (0);
}

int txop_ref_recv(shpeer_t *peer, tx_ref_t *ref)
{
  return (0);
}

int txop_ref_wrap(shpeer_t *peer, tx_ref_t *ref)
{
  wrap_bytes(&ref->ref_op, sizeof(ref->ref_op));
  wrap_bytes(&ref->ref_type, sizeof(ref->ref_type));
  return (0);
}


