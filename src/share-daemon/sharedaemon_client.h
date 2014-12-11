

#ifndef __SHAREDAEMON_CLIENT_H__
#define __SHAREDAEMON_CLIENT_H__


#define SHD_CLIENT_MSG (1 << 0)
#define SHD_CLIENT_NET (1 << 1)

/* deprec sock_t & move to..*/
typedef struct shd_net_t {
  int fd;
} shd_net_t;

typedef struct shd_msg_t {
  shkey_t msg_key;
  shpeer_t app_peer;
} shd_msg_t;

/**
 * Specifies which peer operations that the client has requested to be informed on.
 */
typedef struct shd_listen_t {
  shtime_t li_stamp;
  shkey_t li_key;
  int li_op;

  struct shd_listen_t *next;
} shd_listen_t;

typedef struct shd_t {
  int flags;
  tx_app_t *app;

  union {
    shd_net_t net;
    shd_msg_t msg;
  } cli;

  /* incoming & outgoing data buffers. */
  shbuf_t *buff_out;
  shbuf_t *buff_in;

  struct shd_t *next;
} shd_t;

extern shd_t *sharedaemon_client_list;

int sharedaemon_msgclient_init(shkey_t *app_key, shpeer_t *priv_peer);
int sharedaemon_netclient_init(int fd, struct sockaddr_in *net_addr);

#endif /* ndef __SHAREDAEMON_CLIENT_H__ */


