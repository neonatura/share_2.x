

#ifndef __SHAREDAEMON_CLIENT_H__
#define __SHAREDAEMON_CLIENT_H__


#define SHD_CLIENT_MSG (1 << 0)
#define SHD_CLIENT_NET (1 << 1)
#define SHD_CLIENT_REGISTER (1 << 2)

/** Request feedback on a particular operation mode. */
#define SHOP_LISTEN (1 << 0)


/* deprec sock_t & move to..*/
typedef struct shd_net_t {
  int fd;
} shd_net_t;

typedef struct shd_msg_t {
  shkey_t msg_key;
  shpeer_t app_peer;
} shd_msg_t;

#if 0
/**
 * Specifies which peer operations that the client has requested to be informed on.
 */
typedef struct shd_listen_t {
  shtime_t li_stamp;
  shkey_t li_key;
  int li_op;

  struct shd_listen_t *next;
} shd_listen_t;
#endif

typedef struct shd_t {
  int flags;
  tx_app_t *app;
  tx_peer_t peer;

  union {
    shd_net_t net;
    shd_msg_t msg;
  } cli;

  /** Behaviour specification for particular operation modes. */
  uint8_t op_flags[MAX_TX];

  /* incoming & outgoing data buffers. */
  shbuf_t *buff_out;
  shbuf_t *buff_in;

  struct shd_t *next;
} shd_t;

extern shd_t *sharedaemon_client_list;

int sharedaemon_netclient_init(int fd, struct sockaddr_in *net_addr);
shd_t *sharedaemon_client_find(shkey_t *key);
int sharedaemon_msgclient_init(shpeer_t *peer);

#endif /* ndef __SHAREDAEMON_CLIENT_H__ */


