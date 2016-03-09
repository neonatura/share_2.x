
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

int inittx_context(tx_context_t *tx, tx_t *ref_tx, shkey_t *ctx_key)
{
  shkey_t *ref_key;

  if (!ctx_key)
    ctx_key = ashkey_uniq();

  ref_key = get_tx_key(ref_tx);
  if (!ref_tx)
    return (SHERR_INVAL);

  tx->ctx_refop = ref_tx->tx_op;
  memcpy(&tx->ctx_ref, ref_key, sizeof(tx->ctx_tx));
  memcpy(&tx->ctx_data, ctx_key, sizeof(tx->ctx_data));

  return (0);
}

tx_context_t *alloc_context(shkey_t *ctx_key, tx_t *ref_tx)
{
  tx_context_t *tx;
  int err;

  tx = (tx_context_t *)calloc(1, sizeof(tx_context_t));
  if (!tx)
    return (NULL);
  
  err = inittx_context(tx, ref_tx, ctx_key);
  if (err)
    return (NULL);

  return (0);
}

tx_context_t *alloc_context_data(shkey_t *ref_tx, void *data, size_t data_len)
{
  tx_context_t *ctx;
  shkey_t *key;

  key = shkey_bin(data, data_len);
  ctx = alloc_context(ref_tx, key);
  shkey_free(&key);

  return (ctx);
}

int txop_context_init(shpeer_t *cli_peer, tx_context_t *ctx)
{

  tx_sign(ctx, &ctx->ctx_sig, &ctx->ctx_data);

  return (0);
}

int txop_context_confirm(shpeer_t *cli_peer, tx_context_t *ctx)
{
  int err;

  err = tx_sign_confirm(ctx, &ctx->ctx_sig, &ctx->ctx_data);
  if (err)
    return (err);

  return (0);
}

int txop_context_recv(shpeer_t *cli_peer, tx_context_t *ctx)
{
  tx_ward_t *ward;
  tx_t *tx;
  int err;

  if (!ctx)
    return (SHERR_INVAL);

  /* load transaction in reference. */
  tx = tx_load(ctx->ctx_refop, &ctx->ctx_ref);
  if (!tx)
    return (SHERR_INVAL);

  ward = NULL;
  if (tx->tx_flag & TXF_WARD) {
    /* load ward transaction */
    ward = tx_load(TX_WARD, &ctx->ctx_ref);
    if (!ward) {
      err = SHERR_INVAL;
      goto done;
    }

    /* verify context to suppress ward */
    err = txward_context_confirm(ward, ctx); 
    if (err)
      goto done;

    /* process original transaction. */
    err = tx_recv(cli_peer, tx);
    if (err)
      goto done;
  }

done:
  pstore_free(tx);
  if (ward) pstore_free(ward);
  return (0);
}

int txop_context_send(shpeer_t *cli_peer, tx_context_t *ctx)
{

  if (cli_peer)
    return (SHERR_OPNOTSUPP); /* must be broadcasted. */

  return (0);
}


