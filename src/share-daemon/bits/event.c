

/*
 * @copyright
 *
 *  Copyright 2013 Brian Burrell 
 *
 *  This file is part of the Share Library.
 *  (https://github.com/briburrell/share)
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


#if 0
int confirm_event(sh_event_t *event, shpeer_t *peer)
{
  sh_sig_t *sig;
  int err;

  sig = find_transaction_signature(&event->event_tx);
  if (!sig)
    return (SHERR_NOENT);

  err = verify_signature(sig, peer, &event->event_tx, &event->event_id);
  if (err)
    return (err);

  sched_tx(peer, &event, sizeof(sh_event_t));
  return (0);
}

/**
 * A trusted client is requesting a transaction be performed in the future.
 * @param duration The number of ms before the transaction will occur.
 */
int generate_event(sh_event_t *event, sh_tx_t *tx, sh_id_t *id, time_t duration, int step)
{
  sh_event_t event;

  memset(&event, 0, sizeof(event));
  generate_transaction_id(&event.tx, step);
  memcpy(&event,event_tx, tx, sizeof(sh_tx_t));
  memcpy(&event,event_id, id, sizeof(sh_id_t));
  event->event_stamp = shtime() + duration;

  return (confirm_event(event, sharedaemon_peer()));
}
#endif


