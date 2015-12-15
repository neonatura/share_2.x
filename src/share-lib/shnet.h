
/*
 * @copyright
 *
 *  Copyright 2013 Neo Natura
 *
 *  This file is part of the Share Library.
 *  (https://github.com/neonatura/share)
 *        
 *  The Share Library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version. 
 *
 *  The Share Library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with The Share Library.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  @endcopyright
 *
 */

#ifndef __NET__SHNET_H__ 
#define __NET__SHNET_H__

#ifndef SHARELIB
#include <sys/socket.h>
#include <netinet/in.h>
#endif

/* use native fcntl() on linux */
#if (defined(linux) && defined(fcntl))
#undef fcntl
#endif

/**
 * The libshare Socket Handling provides access to regular socket operations with posix or convienence functions in addition to access to the ESP network protocol.
 * The ESP protocol performs streaming compression and provides more security than TCP.
 * @brief The libshare network socket operations. 
 * @ingroup libshare
 * @defgroup libshare_net Network socket operations.
 * @{
 */

#ifndef USHORT_MAX
#define USHORT_MAX 65536
#endif

#ifndef IPPROTO_ESTP
#define IPPROTO ESTP 121 /* Encoded Stream Transport Protocol */
#endif


/** socket is not closed */
#define SHNET_ALIVE             (1 << 0)
/** perform asynchronous connection */
#define SHNET_CONNECT           (1 << 1) 
/** do not block client calls */
#define SHNET_ASYNC             (1 << 2) 
/** socket is bound to listen on port. */
#define SHNET_LISTEN            (1 << 3)
/** user-level emulation of network protocol */
#define SHNET_EMULATE           (1 << 4)
/** terminate socket after processing */
#define SHNET_SHUTDOWN          (1 << 5)
/** record connection fail/success in sharefs database */
#define SHNET_TRACK             (1 << 6)


#define SHNET_DEFAULT_DEVICE "eth0"

/*
 * A socket stream with a remote host peer.
 */
typedef struct shnet_t shnet_t;

/*
 * A socket stream with a remote host peer.
 */
struct shnet_t 
{
	int fd;
	int flags;
  int protocol; /* IPPROTO_UDP | IPPROTO_TCP | IPPROTO_SHNET */

	int rcvbuf_len;
	int sndbuf_len;
  shbuf_t *recv_buff;
  shbuf_t *send_buff; /* not used */

  shpeer_t src_addr;
  shpeer_t dst_addr;
};

/**
 * A generic type referencing an invalid transaction.
 */
#define TX_NONE 0
/**
 * A transaction mode to request broadcasts of a transaction type.
 * @note Only applicable for client message-queue communication.
 */
#define TX_LISTEN 1
/**
 * A libshare application identifier.
 */
#define TX_APP 2
/**
 * A share-fs partition file transaction.
 */
#define TX_FILE 3
/**
 * A USDe currency transaction.
 */
#define TX_BOND 4
/**
 * A ward can be placed on another transaction to prohibit from being used.
 */
#define TX_WARD 5
/**
 * A collection of transactions.
 */
#define TX_LEDGER 6
/**
 * A specfic application's reference to an account.
 */
#define TX_IDENT 7
/**
 * An identifier of user credentials.
 */
#define TX_ACCOUNT 8
/**
 * A task performing sexe execution thread(s).
 */
#define TX_TASK 9
/**
 * A sexe runtime execution thread.
 */
#define TX_THREAD 10
/**
 * A secondary confirmation of a transaction.
 */
#define TX_TRUST 11
/**
 * An event specifies when a transaction will become valid.
 * @see shicald
 */
#define TX_EVENT 12
/**
 * An active application session for an identity.
 */
#define TX_SESSION 13
/**
 * A license granting access permission.
 */
#define TX_LICENSE 14
/**
 * A wallet owned by an identity.
 */
#define TX_WALLET 15

/**
 * An account metric of identification.
 */
#define TX_METRIC 16

/** The maximum number of transaction operations supported. */
#define MAX_TX 17


#define TX_BOND_NONE 0
#define TX_BOND_PREPARE 1
#define TX_BOND_SCHEDULE 2
#define TX_BOND_TRANSMIT 3
#define TX_BOND_CONFIRM 4
#define TX_BOND_PENDING 5
#define TX_BOND_COMPLETE 6
#define TX_BONDERR_ADDR 10
#define TX_BONDERR_ACCOUNT 11
#define TX_BONDERR_DEBIT 12
#define TX_BONDERR_NET 13
#define TX_BONDERR_SESS 14

#define NET_DB_NAME "net"

#define TRACK_TABLE_NAME "track"



/**
 * Application identifying information returned to a message-queue client.
 */
typedef struct tx_app_msg_t {
  shpeer_t app_peer;
  shkey_t app_context;
  shtime_t app_stamp;
  uint32_t app_hop;
  uint32_t app_trust;
} tx_app_msg_t;

/**
 * Account information returned to a message-queue client.
 * @note This message is only sent from the client.
 */
typedef struct tx_account_msg_t {
  shseed_t pam_seed;
  uint32_t pam_flag;
} tx_account_msg_t;

/**
 * Account identity returned to a message-queue client.
 * @note This message is only sent from the server. Typical use is auxillary applications authorizating actions in an IPC manner. Use the "shinfo" share utility program to display active <i>share identity</i> instances.
 */
typedef struct tx_id_msg_t {
  shpeer_t id_peer;
  shkey_t id_key;
  shtime_t id_stamp;
  uint64_t id_uid;
} tx_id_msg_t;

typedef struct tx_session_msg_t {
  shkey_t sess_id;
  shtime_t sess_stamp;
} tx_session_msg_t;

typedef struct tx_license_msg_t {
  /** The originating peer granting the license. */
  shpeer_t lic_peer;
  /** The digital signature the licence is granting access for. */
  shsig_t lic_sig;
  /** The key reference to the licensing content. */
  shkey_t lic_name;
  shtime_t lic_expire;
} tx_license_msg_t;


typedef struct tx_event_msg_t {
  shpeer_t event_peer; 
  shsig_t event_sig;
  shtime_t event_stamp;
} tx_event_msg_t;

typedef struct tx_bond_msg_t {
  char bond_sink[MAX_SHARE_HASH_LENGTH];
  char bond_label[MAX_SHARE_NAME_LENGTH];
  shsig_t bond_sig;
  shtime_t bond_expire;
  uint64_t bond_credit;
} tx_bond_msg_t;

typedef struct tx_metric_msg_t {
  /** The type of metric (SHMETRIC_XX). */
  uint32_t met_type;

  /** Type specific flags for the metric instance. */
  uint32_t met_flags;

  /** A self-identifying sub-type of the metric. */
  char met_name[8];

  /** The time-stamp of when the metric expires. */
  shtime_t met_expire;

  /** The account being referenced. */
  uint64_t met_acc;
} tx_metric_msg_t;

/** accept */ int shnet_accept(int sockfd);
/** bind */ int shnet_bindsk(int sockfd, char *hostname, unsigned int port);
/** bind */ int shnet_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
/** close */ int shnet_close(int sk);
/** connect */ int shnet_conn(int sk, char *host, unsigned short port, int async);
/** fcntl */ int shnet_fcntl(int fd, int cmd, long arg);
/** gethost */ struct hostent *shnet_gethostbyname(char *name);
/** gethost */ struct hostent *shnet_peer(char *name);
/** read */ ssize_t shnet_read(int fd, const void *buf, size_t count);
/** socket */ extern shnet_t _sk_table[USHORT_MAX];

/** 
 * Creates a standard IPv4 TCP socket.
 */
int shnet_sk(void);

/** 
 * Create a network socket.
 * @param domain Either AF_INET or AF_INET6.
 * @param type Only SOCK_STREAM is supported.
 * @param protocol Either IPPROTO_TCP or IPPROTO_SHNET. 
 */
int shnet_socket(int domain, int type, int protocol);

/** socket */ struct sockaddr *shnet_host(int sockfd);
/** write */ ssize_t shnet_write(int fd, const void *buf, size_t count);

/**
 * Waits on the specified read/write socket streams and marks which are available for an IO operation.
 * @see shnet_select()
 */
int shnet_verify(fd_set *readfds, fd_set *writefds, long *millis);

/**
 * Performs a POSIX select() against a set of @ref shnet_t socket streams.
 */
int shnet_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

/**
 * Initiate a standard posix connection managable by libshare.
 */
int shconnect(int sk, struct sockaddr *skaddr, socklen_t skaddr_len);

/**
 *  * Initiate a connection to the hostname and port specified.
 *   */
int shconnect_host(char *host, unsigned short port, int flags);

/**
 *  * Initiate a socket connection to a libshare peer entity.
 *   */
int shconnect_peer(shpeer_t *peer, int flags);

/**
 * Obtain the internal socket buffer used for a network connection.
 */
shbuf_t *shnet_read_buf(int fd);




int shnet_track_add(shpeer_t *peer);

int shnet_track_mark(shpeer_t *peer, int cond);

int shnet_track_incr(shpeer_t *peer);

int shnet_track_decr(shpeer_t *peer);

int shnet_track_scan(shpeer_t *peer, shpeer_t **speer_p);

int shnet_track_remove(shpeer_t *peer);


/**
 * @}
 */



#endif /* ndef __NET__SHNET_H__ */


