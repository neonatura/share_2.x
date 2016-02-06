
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

int local_broadcast_metric(tx_metric_t *metric)
{
  sched_tx(metric, sizeof(tx_metric_t));
  return (0);
}

int local_confirm_metric(tx_metric_t *metric, uint64_t sig)
{
  return (0);
}

int local_metric_generate(int type, void *data, size_t data_len, tx_metric_t **metric_p)
{
  shcard_t *card_data;
  shkey_t *key;
  tx_metric_t *met;
  shkey_t *peer_key;
  shkey_t sig_key;
  shtime_t sig_expire;
  char sig_hash[MAX_SHARE_HASH_LENGTH];
  uint64_t sig_csum;
  int err;

  peer_key = NULL;
  sig_csum = 0;
  switch (type) {
    case SHMETRIC_CARD:
      card_data = (shcard_t *)data;
      peer_key = shpeer_kpub(&card_data->card_issuer);
      sig_csum = card_data->card_id;
      sig_expire = card_data->card_expire;
      break;
  }

  /* generate signature */
  key = shkey_cert(peer_key, sig_csum, sig_expire);
  memcpy(&sig_key, key, sizeof(shkey_t));
  shkey_free(&key);

  strcpy(sig_hash, shkey_print(&sig_key));
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

  /* generate permanent transaction reference. */
  local_transid_generate(TX_METRIC, &met->met_tx);
  met->met_type = type;
  met->met_expire = sig_expire;
  memcpy(&met->met_sig, &sig_key, sizeof(shkey_t));

  switch (type) {
    case SHMETRIC_CARD:
      /* fill base data */
      memcpy(&met->met_acc, &card_data->card_acc, sizeof(uint64_t));
      met->met_flags = card_data->card_flags;
      strncpy(met->met_name, card_data->card_type, sizeof(card_data->card_type));

      /* verify integrity and assign/inform network transaction */
      err = local_confirm_metric(met, card_data->card_id);
      if (err) {
        free(met);
        return (err);
      }
      break;
  }


  /* generate unique id */
  key = shkey_bin((char *)data, data_len);
  memcpy(&met->met_id, key, sizeof(shkey_t));
  shkey_free(&key);

  pstore_save(met, sizeof(tx_metric_t));

  if (metric_p) {
    /* assign pointer */
    *metric_p = met;
  } else {
    free(met);
  }

  return (0);
}


int process_metric_event(shd_t *cli, tx_metric_t *metric)
{
fprintf(stderr, "DEBUG: NO-OP: process_metric_event()\n");
  return (0);
}
