
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
    default:
      strcpy(prefix, "default");
      break;
  }

  sprintf(path, "/tx/%s/%s", prefix, name);
  fl = shfs_file_find(_pstore_fs, path);
  err = shfs_file_read(fl, &data, &data_len);
fprintf(stderr, "DEBUG: pstore_read[op %d]: read '%s' <%d bytes>\n", tx_op, path, data_len);
  if (err || data_len == 0)
    return (NULL);


  return (data);
}

int pstore_write(int tx_op, char *name, unsigned char *data, size_t data_len)
{
  SHFL *fl;
  char prefix[256];
  char path[PATH_MAX+1];
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
    default:
      strcpy(prefix, "default");
      break;
  }

  sprintf(path, "/tx/%s/%s", prefix, name);
  fl = shfs_file_find(_pstore_fs, path);
  err = shfs_file_write(fl, data, data_len);
  if (err)
    return (err);

fprintf(stderr, "DEBUG: pstore_write[op %d]: wrote '%s' <%d bytes>\n", tx_op, path, data_len);

  return (0);
}

int pstore_save(void *data, size_t data_len)
{
  tx_account_t *acc;
  tx_id_t *id;
  tx_t *tx;
  unsigned char *raw_data = (unsigned char *)data;
  char path[PATH_MAX+1];

  tx = (tx_t *)raw_data;
  pstore_write(tx->tx_op, tx->hash, raw_data, data_len);

  switch (tx->tx_op) {
    case TX_ACCOUNT:
      acc = (tx_account_t *)raw_data;
      pstore_write(tx->tx_op, (char *)shkey_hex(&acc->acc_name), raw_data, data_len);
      break;
    case TX_IDENT:
      id = (tx_id_t *)raw_data;
      pstore_write(tx->tx_op, (char *)shkey_hex(&id->id_name), raw_data, data_len);
      break;
  }

  return (0);
}

void *pstore_load(int tx_op, char *hash)
{
  return (pstore_read(tx_op, hash));
}

void pstore_free(void *tx)
{

  if (tx)
    free(tx);

}


