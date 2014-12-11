
int init_peer(sh_peer_t *peer, tx_t *tx, sh_id_t *id);
int generate_peer_tx(tx_peer_t *tx, shpeer_t *peer);
int confirm_peer(tx_peer_t *tx, shkey_t *peer_key);
