
int get_bond_state(tx_bond_t *bond);

void set_bond_state(tx_bond_t *bond, int state);

shkey_t *get_bond_key(shkey_t *sender, shkey_t *receiver, shkey_t *ref);

tx_bond_t *load_bond(shkey_t *bond_key);

tx_bond_t *load_bond_peer(shpeer_t *sender, shpeer_t *receiver, shpeer_t *ref);

tx_bond_t *create_bond(shkey_t *bond_key, double duration, double fee, double basis);

tx_bond_t *create_bond_peer(shpeer_t *receiver, shpeer_t *ref, double duration, double fee, double basis);

void save_bond(tx_bond_t *bond);

void free_bond(tx_bond_t **bond_p);

int confirm_bond_value(tx_bond_t *bond, double fee);

int complete_bond(tx_bond_t *bond); 

