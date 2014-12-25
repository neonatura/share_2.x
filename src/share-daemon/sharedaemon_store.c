
#include <share.h>

int pstore_save(void *data, size_t data_len)
{
  return (0);
}

void *pstore_load(int op, char *hash)
{
  return (NULL);
}

void pstore_free(void **tx_p)
{
  void *tx;

  if (!tx_p)
    return;

  tx = *tx_p;
  *tx_p = NULL;

  if (tx)
    free(tx);

}


