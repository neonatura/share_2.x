
#include "share.h"
#include "sharedaemon.h"

static shfs_t *_pstore_fs;

void pstore_init(void)
{
  if (!_pstore_fs)
    _pstore_fs = shfs_init(NULL);
}

void *pstore_read(int tx_op, char *name)
{
  SHFL *fl;
  shbuf_t *buff;
  char path[PATH_MAX+1];
  char prefix[256];
  unsigned char *data;
  size_t data_len;
  int err;

  pstore_init(); 

  memset(prefix, 0, sizeof(prefix));
  switch (tx_op) {
    case TX_ACCOUNT:
      strcpy(prefix, "account");
      break;
    case TX_IDENT:
      strcpy(prefix, "id");
      break;
    case TX_SESSION:
      strcpy(prefix, "session");
      break;
    case TX_APP:
      strcpy(prefix, "app");
      break;
    case TX_LEDGER:
      strcpy(prefix, "ledger");
      break;
    default:
      strcpy(prefix, "default");
      break;
  }

  buff = shbuf_init();
  sprintf(path, "/tx/%s/%s", prefix, name);
  fl = shfs_file_find(_pstore_fs, path);
  err = shfs_read(fl, buff);
  if (err) {
    shbuf_free(&buff);
    return (NULL);
  }

  return (shbuf_unmap(buff));
}

int pstore_write(int tx_op, char *name, unsigned char *data, size_t data_len)
{
  SHFL *fl;
  char prefix[256];
  char path[PATH_MAX+1];
  shbuf_t *buff;
  int err;

  pstore_init(); 

  memset(prefix, 0, sizeof(prefix));
  switch (tx_op) {
    case TX_ACCOUNT:
      strcpy(prefix, "account");
      break;
    case TX_IDENT:
      strcpy(prefix, "id");
      break;
    case TX_SESSION:
      strcpy(prefix, "session");
      break;
    case TX_APP:
      strcpy(prefix, "app");
      break;
    case TX_LEDGER:
      strcpy(prefix, "ledger");
      break;
    default:
      strcpy(prefix, "default");
      break;
  }

  sprintf(path, "/tx/%s/%s", prefix, name);
  fl = shfs_file_find(_pstore_fs, path);
buff = shbuf_init();
shbuf_cat(buff, data, data_len);
  err = shfs_write(fl, buff);
shbuf_free(&buff);
  if (err)
    return (err);

  return (0);
}

int pstore_save(void *data, size_t data_len)
{
  tx_account_t *acc;
  tx_id_t *id;
  tx_session_t *sess;
  tx_app_t *app;
  tx_ledger_t *ledger;
  tx_asset_t *asset;
  tx_t *tx;
  shkey_t *key;
  unsigned char *raw_data = (unsigned char *)data;
  char path[PATH_MAX+1];

  tx = (tx_t *)raw_data;
  pstore_write(tx->tx_op, tx->hash, raw_data, data_len);

  switch (tx->tx_op) {
    case TX_ACCOUNT:
      /* account uses 'account seed key' as lookup field. */
      acc = (tx_account_t *)raw_data;
      pstore_write(tx->tx_op, shcrcstr(acc->pam_seed.seed_uid), 
          raw_data, data_len);
      break;

    case TX_IDENT:
      /* identity uses 'identity key' as lookup field. */
      id = (tx_id_t *)raw_data;
      pstore_write(tx->tx_op, (char *)shkey_hex(&id->id_key), raw_data, data_len);
      break;
    case TX_SESSION:
      /* session uses 'identity key' as lookup field. */
      sess = (tx_session_t *)raw_data;
      pstore_write(tx->tx_op, (char *)shkey_hex(&sess->sess_id), raw_data, data_len);
      break;
    case TX_APP:
      /* app uses 'private peer key' as lookup field. */
      app = (tx_app_t *)raw_data;
      pstore_write(tx->tx_op, (char *)shkey_hex(shpeer_kpriv(&app->app_peer)), raw_data, data_len);
      break;
    case TX_LEDGER:
      /* ledger uses 'public peer key' as lookup field. */
      ledger = (tx_ledger_t *)raw_data;
      pstore_write(tx->tx_op, (char *)shkey_hex(shpeer_kpub(&ledger->ledger_peer)), raw_data, data_len);
      break;
    case TX_ASSET:
      /* asset uses 'asset signature' as lookup field. */
      asset = (tx_asset_t *)raw_data;
      pstore_write(tx->tx_op, 
          (char *)shkey_hex(&asset->ass_sig), raw_data, data_len);
      break;
  }

  return (0);
}

void *pstore_load(int tx_op, char *hash)
{
  return (pstore_read(tx_op, hash));
}

int pstore_delete(int tx_op, char *hash)
{
  char path[PATH_MAX+1];
  char prefix[256];
  shfs_ino_t *fl;
  int err;

  pstore_init(); 

  memset(prefix, 0, sizeof(prefix));
  switch (tx_op) {
    case TX_ACCOUNT:
      strcpy(prefix, "account");
      break;
    case TX_IDENT:
      strcpy(prefix, "id");
      break;
    case TX_SESSION:
      strcpy(prefix, "session");
      break;
    case TX_APP:
      strcpy(prefix, "app");
      break;
    case TX_LEDGER:
      strcpy(prefix, "ledger");
      break;
    default:
      strcpy(prefix, "default");
      break;
  }

  sprintf(path, "/tx/%s/%s", prefix, hash);
  fl = shfs_file_find(_pstore_fs, path);
  err = shfs_file_remove(fl);
  if (err)
    return (err);

  return (0);
}

void pstore_remove(int tx_op, char *hash, void *data)
{
  unsigned char *raw_data = (unsigned char *)data;
  tx_account_t *acc;
  tx_id_t *id;
  tx_session_t *sess;
  tx_app_t *app;
  tx_ledger_t *ledger;
  shkey_t *key;
  char path[PATH_MAX+1];
  char prefix[256];

  pstore_delete(tx_op, hash);

  switch (tx_op) {
    case TX_ACCOUNT:
      /* account uses 'account seed key' as lookup field. */
      acc = (tx_account_t *)raw_data;
      pstore_delete(tx_op, shcrcstr(acc->pam_seed.seed_uid));
      break;
    case TX_IDENT:
      /* identity uses 'identity key' as lookup field. */
      id = (tx_id_t *)raw_data;
      pstore_delete(tx_op, (char *)shkey_hex(&id->id_key));
      break;
    case TX_SESSION:
      /* session uses 'identity key' as lookup field. */
      sess = (tx_session_t *)raw_data;
      pstore_delete(tx_op, (char *)shkey_hex(&sess->sess_id));
      break;
    case TX_APP:
      /* app uses 'private peer key' as lookup field. */
      app = (tx_app_t *)raw_data;
      pstore_delete(tx_op, (char *)shkey_hex(shpeer_kpriv(&app->app_peer)));
      break;
    case TX_LEDGER:
      /* app uses 'private peer key' as lookup field. */
      ledger = (tx_ledger_t *)raw_data;
      pstore_delete(tx_op, (char *)shkey_hex(shpeer_kpub(&ledger->ledger_peer)));
      break;
  }
  
}

void pstore_free(void *tx)
{

  if (tx)
    free(tx);

}


