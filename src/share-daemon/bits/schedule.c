
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

//static sh_task_t schedule[MAX_SCHEDULE_TASKS];

void sched_tx_payload(void *data, size_t data_len, void *payload, size_t payload_len)
{
  tx_t *tx = (tx_t *)data;
  tx_t sig_tx;
  shsig_t sig;
  shsig_t new_sig;
  shbuf_t *buff;
  shpeer_t *self_peer;

  if (!data || !data_len)
    return;

#if 0
  memset(&sig_tx, 0, sizeof(sig_tx));
  generate_transaction_id(TX_SIGNATURE, &sig_tx, NULL);

  memset(&sig, 0, sizeof(sig));
self_peer = sharedaemon_peer();
  generate_signature(&sig, &self_peer->name, tx);

  /* send preceeding server signature for transaction */
  broadcast_raw(&sig_tx, sizeof(sig_tx));
  broadcast_raw(&sig, sizeof(sig));
#endif


  buff = shbuf_init();
  shbuf_cat(buff, data, data_len);
  if (payload && payload_len)
    shbuf_cat(buff, payload, payload_len);
  broadcast_raw(shbuf_data(buff), shbuf_size(buff));
  shbuf_free(&buff);


}

void sched_tx(void *data, size_t data_len)
{
  prep_transaction((tx_t *)data);
  sched_tx_payload(data, data_len, NULL, NULL);
}

#if 0
int sched_rx(shpeer_t *peer, void *data, size_t data_len)
{
	tx_t *tx = (tx_t *)data;
  shsig_t *sig;
  uint64_t crc;
	int err;

	switch (tx->tx_op) {
		case TX_SIGNATURE:
			/* validating a sub-sequent request */
			if (data_len < sizeof(shsig_t))
				return (SHERR_INVAL);

      sig = (shsig_t *)data;
      crc = (uint64_t)strtoll(sig->sig_tx, NULL, 16);
      err = shkey_verify(&sig->sig_key, crc, &peer->name, sig->sig_stamp);
			if (err)
				return (err);
			break;
	}

	return (0);
}
#endif





