

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

#include "sharedaemon.h"

int inittx_ward(tx_ward_t *ward, tx_t *tx, tx_context_t *ctx)
{
  shkey_t *tx_key;
  int err;

  tx_key = get_tx_key(tx);
  if (!tx_key)
    return (SHERR_INVAL);

  memcpy(&ward->ward_ref, tx_key, sizeof(ward->ward_ref));

  txward_context_sign(ward, ctx);

  err = tx_init(NULL, (tx_t *)ward, TX_WARD);
  if (err)
    return (err);

  return (0);
}

tx_ward_t *alloc_ward(tx_t *tx, tx_context_t *ctx)
{
  tx_ward_t *ward;
  int err;

  ward = (tx_ward_t *)calloc(1, sizeof(tx_ward_t));
  if (!ward)
    return (NULL);

  err = inittx_ward(ward, tx, ctx);
  if (err)
    return (err);

  return (0);
}


/** Associated a particular context with releasing a ward. */
void txward_context_sign(tx_ward_t *ward, tx_context_t *ctx)
{
  shkey_t *sig_key;

  if (!ward || !ctx)
    return;

  if (!shkey_cmp(&ward->ward_ref, &ctx->ctx_ref))
    return (SHERR_INVAL);

  if (ward->ward_tx.tx_stamp == SHTIME_UNDEFINED)
    ward->ward_tx.tx_stamp = shtime();

  sig_key = shkey_cert(&ctx->ctx_sig,
      shkey_crc(&ctx->ctx_ref), ward->ward_tx.tx_stamp);
  memcpy(&ward->ward_sig, sig_key, sizeof(ward->ward_sig));
  shkey_free(&sig_key);

}

/** Determine whether the appropriate context is availale. */
int txward_context_confirm(tx_ward_t *ward, tx_context_t *ctx)
{
  int err;

  if (!shkey_cmp(&ward->ward_ref, &ctx->ctx_ref))
    return (SHERR_INVAL);

  err = shkey_verify(&ward->ward_sig, shkey_crc(&ctx->ctx_ref), 
    &ctx->ctx_sig, ward->ward_tx.tx_stamp);
  if (err)
    return (err);

  return (0);
}

int txop_ward_init(shpeer_t *cli_peer, tx_ward_t *ward)
{
  unsigned char context[128];
  shkey_t sig_key;
  tx_context_t ctx;
  tx_t *tx;
  int err;

  if (ward->ward_stamp == SHTIME_UNDEFINED)
    ward->ward_stamp = shtime();
  if (ward->ward_expire == SHTIME_UNDEFINED)
    ward->ward_expire = shtime_adj(shtime(), MAX_SHARE_SESSION_TIME);

  memcpy(&ward->ward_tx.tx_peer, shpeer_kpriv(sharedaemon_peer()), sizeof(shpeer_t));


  memset(&ctx, 0, sizeof(ctx));
  err = inittx_context_ref(&ctx, ward, ashkey_uniq());
  if (err)
    return (err);

  err = tx_save(&ctx);
  if (err)
    return (err);

  /* store key reference to generated context */
  memcpy(&ward->ward_ctx, &ctx.ctx_tx.tx_key, sizeof(ward->ward_ctx));


  return (0);
}

int txop_ward_confirm(shpeer_t *peer, tx_ward_t *ward)
{
  tx_context_t *ctx;
  tx_t *tx;
  int err;

#if 0
  /* verify identity exists */
  ctx = (tx_context_t *)tx_load(TX_CONTEXT, &ward->ward_ctx);
  if (!ctx)
    return (SHERR_NOKEY);
/* .. */
  pstore_free(ctx);

  err = confirm_signature(&ward->ward_sig, 
      shpeer_kpriv(&ward->ward_peer), shkey_hex(&ward->ward_key));
  pstore_free(tx);
  if (err)
    return (err);

  err = inittx_context(ctx, get_tx_key(ward), ashkey_uniq());
  if (err)
    return (err);
#endif

  return (0);
}

int txop_ward_send(shpeer_t *peer, tx_ward_t *ward)
{
  return (0);
}

int txop_ward_recv(shpeer_t *peer, tx_ward_t *ward)
{
  return (0);
}

/**
 * Apply a ward to a transaction as it is being initialized.
 */
int txward_init(tx_t *tx)
{
  tx_context_t ctx;
  tx_ward_t ward;
  int err;

  memset(&ctx, 0, sizeof(ctx));
  err = inittx_context_ref(&ctx, tx, NULL);
  if (err)
    return (err);

  err = tx_save(&ctx);
  if (err)
    return (err);

  memset(&ward, 0, sizeof(ward));
  err = inittx_ward(&ward, tx, &ctx);
  if (err)
    return (err);

  err = tx_send(NULL, (tx_t *)&ward);
  if (err)
    return (err);

  err = tx_save(&ward);
  if (err)
    return (err);

  tx->tx_flag |= TXF_WARD;

  return (0);
}

/**
 * Confirm a ward's application on a transaction.
 */
int txward_confirm(tx_t *tx)
{
  tx_ward_t *ward;
  tx_context_t *ctx;
  shkey_t *tx_key;
  int err;

  tx_key = get_tx_key(tx);
  if (!tx_key)
    return (SHERR_INVAL);

  ward = (tx_ward_t *)tx_load(TX_WARD, tx_key);
  if (!ward)
    return (SHERR_NOKEY); /* incomplete scope */

  ctx = (tx_context_t *)tx_load(TX_CONTEXT, tx_key);
  if (!ctx)
    return (SHERR_AGAIN); /* transaction ward is applied. */

  /* validate context to invalidate ward. */
  err = txward_context_confirm(ward, ctx);
  pstore_free(ctx);
  if (err)
    return (err);

  return (0);
}


