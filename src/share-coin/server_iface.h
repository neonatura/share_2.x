

#ifndef __SERVER_IFACE_H__
#define __SERVER_IFACE_H__

#define BLKERR_BAD_SESSION 61
#define BLKERR_INVALID_JOB 62
#define BLKERR_DUPLICATE_BLOCK 63
#define BLKERR_LOW_DIFFICULTY 23
#define BLKERR_UNKNOWN 20
#define BLKERR_INVALID_BLOCK 71
#define BLKERR_INVALID_FORMAT 72

#ifdef __cplusplus
extern "C" {
#endif



/* net.cpp */
void start_node(void);

/* init.cpp */
int load_wallet(void);

/* init.cpp */
void server_shutdown(void);

const char *getblocktemplate(void);

int submitblock(unsigned int workId, unsigned int nTime, unsigned int nNonce, char *xn_hex);

#ifdef __cplusplus
}
#endif

#endif /* ndef __SERVER_IFACE_H__ */

