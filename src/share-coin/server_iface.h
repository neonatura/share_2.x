

#ifndef __SERVER_IFACE_H__
#define __SERVER_IFACE_H__

/* net.cpp */
void start_node(void);

/* rpcmining.cpp */
const char *c_getblocktemplate(void);

/* init.cpp */
int load_wallet(void);

/* init.cpp */
void server_shutdown(void);

/* bitcoinrpc.cpp */
const char *getblocktemplate(void);

#endif /* ndef __SERVER_IFACE_H__ */

