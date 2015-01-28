

int pstore_save(void *data, size_t data_len);

void *pstore_load(int op, char *hash);

void pstore_free(void *tx);

void pstore_remove(int tx_op, char *hash, void *data);

