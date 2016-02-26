

/*
 * @copyright
 *
 *  Copyright 2013 Neo Natura
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


#define __BITS__EVENT_C__



#if 0
int confirm_event(tx_event_t *event)
{
  int err;

  /* verify event's signature integrity */
  err = confirm_signature(&event->event_sig, shpeer_kpriv(&event->event_peer), event->event_tx.hash);
  if (err)
    return (err);

  /* inform network */
  sched_tx(event, sizeof(tx_event_t));

  return (0);
}

/**
 * A trusted client is requesting a transaction be performed in the future.
 * @param duration The number of ms before the transaction will occur.
 */
int generate_event(tx_event_t *event, shpeer_t *peer, time_t duration)
{
  shsig_t sig;

  local_transid_generate(TX_EVENT, &event->event_tx);
  event->event_stamp = shtime() + duration;
  generate_signature(&event->event_sig, shpeer_kpub(peer), &event->event_tx);
  memcpy(&event->event_peer, peer, sizeof(shpeer_t));

  return (confirm_event(event));
}

int process_event_tx(tx_event_t *event)
{
  tx_event_t *ent;
  int err;

  ent = (tx_event_t *)pstore_load(TX_EVENT, event->event_tx.hash);
  if (!ent) {
    err = confirm_event(event);
    if (err)
      return (err);

    pstore_save(&event, sizeof(tx_event_t));
  }

  return (0);
} 

#endif


int txop_event_init(shpeer_t *cli_peer, tx_event_t *event)
{
  return (0);
}
int txop_event_confirm(shpeer_t *cli_peer, tx_event_t *event)
{
  int err;

  /* verify event's signature integrity */
  err = confirm_signature(&event->event_sig, shpeer_kpriv(&event->event_peer), event->event_tx.hash);
  if (err)
    return (err);

  return (0);
}
int txop_event_send(shpeer_t *cli_peer, tx_event_t *event)
{
  return (0);
}
int txop_event_recv(shpeer_t *cli_peer, tx_event_t *event)
{
  return (0);
}
