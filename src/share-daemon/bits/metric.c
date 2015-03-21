
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
#include "../sharedaemon.h"

int local_metric_generate(int type, void *data, size_t data_len, tx_metric_t **metric_p)
{
#if 0
  shcard_t *card_data;
  shkey_t *key;
  shsig_t sig;
  tx_metric_t *met;
  uint64_t sig_csum;

  switch (type) {
    case SHMETRIC_CARD:
      memset(&sig, 0, sizeof(sig));
      sig.sig_stamp = shtime64();
      memcpy(&sig.sig_peer, &card_data.card_issuer, sizeof(shpeer_t));
      sig.sig_expire = card_data.card_expire;
      sig_csum = card_data.card_id;
      break;
  }

  /* generate signature */
  key = shkey_cert(&sig.sig_peer, sig_csum, sig.sig_expire);
  memcpy(&sig.sig_id, key, sizeof(shkey_t));
  shkey_free(&key);

  strcpy(sig_hash, shkey_print(&sig->sig_id));
  met = (tx_metric_t *)pstore_load(TX_METRIC, sig_hash);
  if (met) {
    /* verify integrity and assign/inform network transaction */
    err = local_confirm_metric(met, sig_csum);
    if (err)
      return (err);

    if (!metric_p) {
      pstore_free(met);
    } else {
      *metric_p = met;
    }
    return (0);
  }


  met = (tx_metric_t *)calloc(1, sizeof(tx_metric_t));
  if (!met)
    return (SHERR_NOMEM);

  /* reference metric's "self-identifying identification number". */
  key = shkey_bin((char *)&sig_csum, sizeof(sig_csum));
  memcpy(&sig->sig_key, key, sizeof(shkey_t));
  shkey_free(&key);
  /* assign signature to metric */
  memcpy(&met->met_sig, &sig, sizeof(shsig_t));

  /* generate permanent transaction reference. */
  generate_transaction_id(TX_METRIC, &met->met_tx, sizeof(tx_t));

  /* generate unique id */
  key = shkey_bin((char *)&sig, sizeof(shkey_t));
  memcpy(&sig->sig_id, key, sizeof(shkey_t));
  shkey_free(&key);

  /* verify integrity and assign/inform network transaction */
  err = local_confirm_metric(met, card_data.card_id);
  if (err) {
    free(met);
    return (err);
  }

  pstore_save(met, sizeof(tx_metric_t));

  if (metric_p) {
    /* assign pointer */
    *metric_p = met;
  } else {
    free(met);
  }

#endif
  return (0);
}

