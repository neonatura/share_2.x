
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

void sched_tx(void *data, size_t data_len)
{
  sched_tx_payload(data, data_len, NULL, NULL);
}
void sched_tx_payload(void *data, size_t data_len, char *payload, size_t payload_len)
{
  sh_account_t *account = sharedaemon_account();
  sh_id_t *id = &account->id;
  sh_tx_t *tx = (sh_tx_t *)data;
  sh_tx_t sig_tx;
  sh_sig_t sig;
  sh_sig_t new_sig;

  memset(&sig_tx, 0, sizeof(sig_tx));
  sig_tx.tx_op = TX_SIGNATURE;
  generate_transaction_id(&sig_tx);
  generate_signature(&sig, sharedaemon_peer(), tx, id);

  /* send preceeding server signature for transaction */
  broadcast_raw(&sig_tx, sizeof(sig_tx));
  broadcast_raw(&sig, sizeof(sh_sig_t));

  /* send entire message with tx header */
  broadcast_raw(data, data_len);

  if (payload && payload_len)
    broadcast_raw(payload, payload_len);

}

int sched_rx(shpeer_t *peer, void *data, size_t data_len)
{
	sh_tx_t *tx = (sh_tx_t *)data;
	int err;

	switch (tx->tx_op) {
		case TX_SIGNATURE:
			/* validating a sub-sequent request */
			if (data_len < sizeof(sh_sig_t))
				return (SHERR_INVAL);
			err = verify_signature((sh_sig_t *)data);
			if (err)
				return (err);
			break;
	}

	return (0);
}





