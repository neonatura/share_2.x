
/* INET         An implementation of the TCP/IP protocol suite for the LINUX
 *              operating system.  INET is implemented using the  BSD Socket
 *              interface as the means of communication with the user level.
 *
 *              Definitions shared by kernel and user-level programs.
 *
 * Version:     @(#)esp.h          1.1b                    01/24/2001
 *
 * Author:      Brian Burrell, <bburrell@el-i.net>
 *
 * Fixes:
 *
 */

#include <linux/enc.h>
#include <linux/poll.h>

#ifndef __NET__ESP_H__
#define __NET__ESP_H__

#define ESP_VERSION 1

#define ESP_HTABLE_SIZE             1024 
#define ESP_DEFAULT_DICTSEG_LENGTH  4
#define ESP_MIN_DICTSEG_LENGTH      3
#define ESP_DICTMAP_SIZE            2
#define ESP_QUEUE_FREQ              (HZ/10)
#define ESP_DEF_VALUE               0xFE
#define ESP_DEF_CHECK               __constant_htons(ESP_DEF_VALUE)
#define ESP_DEF_STREAM_ID           __constant_htonl(ESP_DEF_VALUE)
#define ESPUDP_DEF_CHECK            __constant_htons(0)
#define ESP_MAX_DATA_SIZE           61440
#define ESP_MAX_DATA_WINDOW         62914560
// #define ESP_MAX_BACKLOG_SIZE        262144
// #define ESP_MAX_SOFT_ERRORS         512 
#define ESP_PACKET_BLOCK_SIZE       8 

/* default packets per burst (~100us) */
#define ESP_DEF_PACKETS_PER_BURST   8 

/* maximum number of packets allowed per burst. */
#define ESP_MAX_PACKETS_PER_BURST   1024

/* rate of packets incremented per "full" burst, when applicable. */
#define ESP_INCR_BURST_RATE         1

/* ESP Packet Modes */
enum {
	ESP_DATA,
	ESP_HANDSHAKE, 
	ESP_ERROR,
	ESP_REQUEST,
	ESP_PROBE,
	ESP_PROBE_RETURN,
	ESP_PACKET_MODES,
	ESP_MAX_PACKET_MODES=200 /* modes 200 - 254 are RESERVED. */
};

/* ESP Socket States */
enum {
	ESP_ESTABLISHED = 1,
	ESP_TCP_RESERVED__2,
	ESP_TCP_RESERVED__3,
	ESP_TCP_RESERVED__4,
	ESP_TCP_RESERVED__5,
	ESP_TCP_RESERVED__6,
	ESP_CLOSE,
	ESP_TCP_RESERVED__8,
	ESP_TCP_RESERVED__9,
	ESP_LISTEN,
	ESP_CLOSE_WAIT,
	ESP_CLOSE_ACK,
	ESP_CONN_WAIT,
	ESP_CONN_ACK,
	ESP_SOCKET_STATES
};

/* ESP Socket Options */
#define EOPT_RETRAN               (1 << 0)  /* retransmitted packet. */
#define EOPT_ACK_BLOCK            (1 << 1)  /* acknowledged packet block. */
#define EOPT_RESERVED_02          (1 << 2)
#define EOPT_RESERVED_03          (1 << 3)
#define EOPT_RESERVED_04          (1 << 4)
#define EOPT_RESERVED_05          (1 << 5)
#define EOPT_RESERVED_06          (1 << 6)
#define EOPT_RESERVED_07          (1 << 7)
/* * The following cannot be sent via (eh->opt) header variable. * */
#define EOPT_FAST_CHECKSUM        (1 << 8)  /* esp checksum (v1) */
#define EOPT_FAST_COMPRESS        (1 << 9)  /* esp compression (v1) */
#define EOPT_NONBLOCKING          (1 << 10) /* no blocking in processing. */

/* ESP Error Types */
enum {
	E_UNKNOWN,
	E_QUEUE,      /* Packet Queue Error                            */
	E_LENGTH,     /* Packet Length Mismatch                        */
	E_ACCESS,     /* insufficient permission                       */
	E_STATE,      /* cannot handle data in current state           */
	E_CHECKSUM,   /* Packet Checksum Error                         */
	E_PARAM,      /* Invalid Packet Parameters                     */
	ESP_ERROR_TYPES,
	ESP_MAX_ERROR_TYPES=94
};

/* Esp Segment Flags */
#define ESF_SEGMENT (1 << 0)
#define ESF_MAP     (1 << 1)

#define ESP_DICTSEGS(_seglen)        (ESP_DICTBLOCK_SIZE/(_seglen))

#define EOPT_SET(_socket, _option) \
        ((_socket)->tp_pinfo.af_esp.opt |= (_option))
#define EOPT_UNSET(_socket, _option) \
        ((_socket)->tp_pinfo.af_esp.opt &= ~(_option))
#define EOPT_ISSET(_socket, _option) \
        ((_socket)->tp_pinfo.af_esp.opt & (_option))

#define esp_v4_skb_rcv_lookup(_skbuff)\
	esp_v4_rcv_lookup(\
			(_skbuff)->nh.iph->saddr, (_skbuff)->h.eh->sport,\
			(_skbuff)->nh.iph->daddr, (_skbuff)->h.eh->dport,\
			(_skbuff)->dev->ifindex,  (_skbuff)->h.eh->stream_id)

#define espudp_v4_skb_rcv_lookup(_skbuff)\
	espudp_v4_rcv_lookup(\
			(_skbuff)->nh.iph->saddr, (_skbuff)->h.uh->source,\
			(_skbuff)->nh.iph->daddr, (_skbuff)->h.uh->dest,\
			(_skbuff)->dev->ifindex,  ESP_DEF_STREAM_ID)

#if 0
#define esp_v4_skb_proxy_lookup(_skbuff)\
	esp_v4_proxy_lookup(\
			(_skbuff)->h.eh->dport,\
			(_skbuff)->nh.iph->saddr, (_skbuff)->h.eh->sport,\
			(_skbuff)->nh.iph->daddr, (_skbuff)->dev,\
			IPCB(_skbuff)->redirport, (_skbuff)->dev->ifindex,
			(_skbuff)->h.eh.stream_id)
#endif

/* esp_valid_size() */
#define esp_valid_size(_length, _version, _version_size, _max_size)\
	(((_version) >  ESP_VERSION  &&  ((_length) < (_version_size)[ESP_VERSION] || (_length) >= (_max_size))) ||\
	 ((_version) <= ESP_VERSION  &&  (_length) != (_version_size)[(_version)]) ?\
	 -EINVAL : 0)

#define ESP_DEC_MSL(_sock) \
	(((_sock)->tp_pinfo.af_esp.msl = max(IPV4_MIN_MTU, \
																			 (_sock)->tp_pinfo.af_esp.msl - \
																			 (_sock)->tp_pinfo.af_esp.st)))
#define ESP_INC_MSL(_sock) \
	(((_sock)->tp_pinfo.af_esp.msl = min(ESP_MAX_DATA_SIZE, \
																			 (_sock)->tp_pinfo.af_esp.msl + \
																			 (_sock)->tp_pinfo.af_esp.st)))

#define ESP_STATE_STRING(_state) \
	((_state) == ESP_CLOSE         ? "CLOSE"       : \
	 (_state) == ESP_CLOSE_WAIT    ? "CLOSE_WAIT"  : \
	 (_state) == ESP_CLOSE_ACK     ? "CLOSE_ACK"   : \
	 (_state) == ESP_LISTEN        ? "LISTEN"      : \
	 (_state) == ESP_CONN_WAIT     ? "CONN_WAIT"   : \
	 (_state) == ESP_CONN_ACK      ? "CONN_ACK"    : \
	 (_state) == ESP_ESTABLISHED   ? "ESTABLISHED" : \
	 (_state) <  ESP_SOCKET_STATES ? "OTHER"       : \
	 "ILLEGAL")

#define IPTOS_STRING(_tos) \
	(((_tos) == 0)                ? "NONE"        : \
	 ((_tos) & IPTOS_LOWDELAY)    ? "LOWDELAY"    : \
	 ((_tos) & IPTOS_THROUGHPUT)  ? "THROUGHPUT"  : \
	 ((_tos) & IPTOS_RELIABILITY) ? "RELIABILITY" : \
	 ((_tos) & IPTOS_MINCOST)     ? "MINCOST"     : "OTHER")

#define ESP_SKADDR(_sk) \
	NIPQUAD((_sk)->rcv_saddr), (int)ntohs((_sk)->sport), \
	NIPQUAD((_sk)->daddr),     (int)ntohs((_sk)->dport)

#define ESP_MIB(_sk) \
	((_sk)->family == PF_INET ? esp_statistics : esp_stat6)

/* Force NETDEBUG on if CONFIG_ESP_DEBUG is defined. */
#ifdef CONFIG_ESP_DEBUG
#undef NETDEBUG
#define NETDEBUG(x) do { x; } while (0)
#endif

#ifdef CONFIG_ESP_VERBOSE_DEBUG
	#define ESPDEBUG(x) do { x; } while (0)
#else
	#define ESPDEBUG(x) do { if (sk->debug) x; } while (0)
#endif

typedef struct esp_error_p {
        __u32 err_type;
        __u32 err_code;
				__u32 err_sys;
        __u32 state;
				struct esphdr eh;
} esp_error_p; /* 40b */

typedef struct esp_handshake_p {
        __u32 ip4_tos;
        __u32 esp_opt;
        __u32 dictseg_len; /* <= 255 (v1.1b) */
				__u32 locked_msw;
				__u32 ttl; /* <= 255 (v1.1b) */
				__u32 stream_id;
} esp_handshake_p; /* 24b */

enum { /* esp request types */
	EREQ_UNKNOWN,
	EREQ_SOCKET_CREATE,
	EREQ_SOCKET_CLOSE,
	EREQ_SOCKET_BPS,
	EREQ_SOCKET_OPEN,
	EREQ_PACKET,
	EREQ_SOCKET_PORT,
	ESP_REQUEST_TYPES,
	ESP_MAX_REQUEST_TYPES=200
};

typedef struct esp_request_p {
	__u32 req_type;
	__u32 req_opt1;
	struct esphdr eh;
} esp_request_p; /* 32b */

/* ESP Packet Mode: ESP_ROUTE */
typedef struct esp_route_p {

#define ERF_RETURN (1 << 0)
	__u32             flg;              /* ESP Route Flags (ERF_XXX)       */

#define ERH_SOURCE 0
#define ERH_DEST   25 
	struct esp_route_host {
		struct in_addr  rh_addr;          /* Route Host IPv4 Address         */ 
		__u32           rh_tos;           /* Route Host IPv4 Type-Of-Service */

		struct esp_route_dest {
			__u8          rh_link;          /* Next Route Host                 */
			__u8          rh_ttl;           /* Time To Live                    */
			__u32         rh_stamp;         /* Route Host Time Stamp           */
			__u32         rh_mtu;           /* Maximum Transmission Unit       */
		} dest;

	} host[26];

} esp_route_p;

/* ESP Packet Mode: ESP_PROBE */
typedef struct esp_probe_p {
	__u32 length;
	__u32 rcv_length;
} esp_probe_p; /* 8b */

#if 0
typedef struct esp_accept_head {
	struct esp_accept_node   *list;
	struct sock              *lsk;
} esp_accept_head;
typedef struct esp_accept_node {
	union {
		esp_accept_head        *head_ptr;
		esp_accept_head        head_raw;
	} head;
	struct sock              *sock;
	struct esp_accept_node   *next;
	struct esp_accept_node   *prev;
} esp_accept_node;
#endif

struct esp_dict_map {
	u_char    c_id;
	u_char    seg_nr;
};
typedef struct esp_dict_hash {
	u_char    *seg[256];
	u_char    seg_idx[256];
} esp_dict_hash;

#if 0
struct espq_dict {

	int mode;
#define EQD_HASH 0
#define EQD_MAP  1
#define EQD_EXT  2

	u_int length;
	union {
		struct espq_dict_hash {
			u_char *seg;	
		} hash;
		struct espq_dict_map {
			u_char c_id;
			u_char seg_nr;
		} map;
		struct espq_dict_ext {
			u_char *data;
		} ext;
	} dict;
};
#endif

typedef struct esp_queue_s {
	struct esphdr         eh;
	u_short               pm;        /* packet mode of queue data.             */
	u_char                *data_raw; /* raw shared data segment.               */
	u_long                data_len;  /* shared data segment length.            */
	u_char                *segflg;   /* dictionary segment bit-flags           */

	int                   msg_flags; /* MSG_XXX flags.                         */
	int                   esp_flags; /* EOPT_XXXX flags.                       */

	struct esp_queue_s    *next;     /* next queue in list.                    */
} esp_queue_s;

struct esp_remote_host {
	union {
		struct in_addr sin4;
		struct in6_addr sin6;
	} daddr;
	int users;

	u_long msw, sw;              /* Max. / Cur. Send Window (per sec).      */
	int msl, st;                 /* Max / current sent length.              */
	int psl;                     /* Penalty send length.                    */
	int dsl;                     /* Discovery send length.                  */

	time_t  r_trip_time[10];     /* Receive packet trip time.               */
	time_t  s_trip_time[10];     /* Send packet trip time.                  */
	int     doff_time[10];       /* Destination Offset time value.          */
	int     r_trip_idx;
	int     s_trip_idx;
	int     doff_idx;

	struct esp_remote_host *next, *prev;
};

struct es_sock {
  /* inet_sock has to be the first member */
  struct inet_sock inet;

  int    opt;                         /* ESP Options (EOPT_XXXX)            */
  int    lcl_error;                   /* IPESP_LOCAL_ERROR                  */
  int    rem_error;                   /* IPESP_REMOVE_ERROR                 */
  int    msl, sl, st;                 /* max, cur., and thresh of send pkt. */
  u_long locked_msw;                  /* max sending window per sec (0=none)*/
  int    pck_per_burst;               /* max packets per data burst (flow?) */
  int    pck_loss_rate;               /* nr of packets lost "recently".     */
  
  __u16  s_seq[ESP_PACKET_MODES];     /* packet sending SEQuence index      */
  __u16  r_seq[ESP_PACKET_MODES];     /* packet receiving SEQuence index    */
  __u16  ns_seq[ESP_PACKET_MODES];    /* next send SEQuence expected        */
  __u16  nr_seq[ESP_PACKET_MODES];    /* next receiving SEQuence expected   */
  __u32  s_id, r_id;                  /* packet identification              */
  
  struct esp_queue_s     *r_overflow; /* ESP Packet Receive Overflow Queue  */
  struct esp_queue_s     *s_queue;    /* ESP Packet Send Queue              */
  u_short                r_noack_pck[ESP_PACKET_MODES];
  struct esp_queue_s     *s_backlog[ESP_PACKET_MODES];

  struct timer_list      timer;       /* ESP Default Timer                  */
  struct timer_list      queue_timer; /* ESP Queue Timer                    */

#if 0
  struct esp_accept_node accept_node; /* accept socket list node            */
#endif
  struct esp_remote_host *rem_host;   /* remote host                        */

  struct esp_dict_hash   *s_dict;     /* Send Segment Hash Table            */
  struct esp_dict_hash   *r_dict;     /* Receive Segment Hash Table         */
  int                    dictseg_len; /* Dictionary Segment Length          */

  union {
    struct sock            **queue;   /* listen socket's accept queue       */
    struct sock            *sock;     /* listen socket                      */
  } listen;

  struct esp_opt_probe_s {
    int timeout;                      /* sec. before next Probe             */
    int tinterval;                    /* sec. between Probes                */
    int len;                          /* sug. probe length                  */
  } probe;

};              

static inline struct es_sock *es_sk(const struct sock *sk)
{ 
  return (struct es_sock *)sk;
}

struct esp_mib {
	int		EspInPackets,
				EspOutPackets,
				EspInDataPackets,
				EspOutDataPackets,
				EspInErrors,
				EspOutErrors,
				EspNoPorts,
				EspRetransmits;
	int   UdpOutPackets,
	      UdpOutDataPackets,
				UdpNoPorts,
				UdpInErrors,
				UdpInPackets,
				UdpInDataPackets;
};

#if defined(CONFIG_ESP_PROTOCOL_MODULE)
extern struct proto *esp_module_prot;
#endif
struct hlist_head es_hash[ESP_HTABLE_SIZE];
extern rwlock_t es_hash_lock; 

/* esp.c */
extern void esp_hash_sock(struct sock *sk);
extern void esp_unhash_sock(struct sock *sk);
extern void esp_rehash_sock(struct sock *sk);
extern void esp_close(struct sock *sk, long timeout);
extern int esp_recvmsg(struct sock *sk, struct msghdr *msg, int len, 
		int noblock, int flags);
extern int esp_rcv_skb(struct sock * sk, struct sk_buff *skb);
extern u_short esp_check(struct sock *sk, __u32 seed, u_char *data, size_t len);
extern int espsend(struct sock *sk, int pm, int level, void *optdata, struct sk_buff *skb);
extern int esp_rcv_handshake(struct sock *sk, struct sk_buff *skb);
extern int esp_create_accept_socket(struct sock *sk, struct sk_buff *skb);
extern struct sock *pull_accept_sock(struct sock *sk);
extern int create_accept_socket(struct sock *sk, struct sk_buff *skb);
extern int esp_rcv_error(struct sock *sk, struct sk_buff *skb);
extern int esp_attach_remote_host(struct sock *sk);
extern void esp_detach_remote_host(struct sock *sk);
extern int esp_rcv_data(struct sock *sk, struct sk_buff **skb_ptr);
extern int esp_rcv(struct sock *sk, struct sk_buff **skb_ptr, size_t len, int pm);
extern int esp_sendmsg(struct sock *sk, struct msghdr *msg, int len);
extern struct sock *esp_accept(struct sock *sk, int flags);
extern int esp_destroy_sock(struct sock *sk);
extern unsigned short esp_good_socknum(void);
extern int esp_set_state(struct sock *sk, int state);
extern size_t esp_packet_size(struct sock *sk, struct esp_queue_s *q);
extern int esp_error(struct sock *sk, struct sk_buff *skb, int err_type, int err_code);	
extern int esp_init_sock(struct sock *sk);
extern int esp_ioctl(struct sock *sk, int cmd, unsigned long arg);
extern int esp_probe_sendlen(struct sock *sk);
extern int kmalloc_queue(unsigned char *data, size_t len, struct esp_queue_s **queue);
extern void kfree_queue(struct esp_queue_s **que);
extern void esp_push_queue(struct esp_queue_s **head, struct esp_queue_s *new);
extern struct esp_queue_s *esp_pull_queue(struct sock *sk, struct esp_queue_s **head);
extern int esp_backlog_packet(struct sock *sk, u_char **pktdata, size_t pktdata_len, struct esphdr *eh, int msg_flags);
int esp_retransmit(struct sock *sk, struct sk_buff *skb, u_char pm, __u32 nbo_seq, __u32 nbo_asl);
extern int esp_listen_rcv(struct sock *sk, struct sk_buff **skb_ptr, int pm);
extern unsigned int esp_poll(struct file *file, struct socket *sock, poll_table *wait);
extern int esp_request(struct sock *sk, struct sk_buff *skb, int request, u_long value);
extern inline void esp_clear_timers(struct sock *sk);


/* esp_v4.c */
extern struct esp_mib esp_statistics;
extern int esp_v4_rcv(struct sk_buff *skb, unsigned short len);
extern void esp_v4_err(struct sk_buff *skb, unsigned char *dp, int len);
extern int esp_v4_connect(struct sock *sk, struct sockaddr *uaddr, int addr_len);
extern int esp_v4_verify_bind(struct sock *sk, unsigned short snum);
extern struct sock *esp_v4_rcv_lookup(u32 saddr, u16 sport, u32 daddr, u16 dport, int dif, __u32 stream_id);
extern struct proto enc_prot;

/* esp_v6.c */
extern struct esp_mib esp_stat6;
extern int esp_v6_rcv_skb(struct sock *sk, struct sk_buff *skb);
extern int esp_v6_deliver(struct sock *sk, struct sk_buff *skb);
extern inline int esp_v6_deliver(struct sock *sk, struct sk_buff *skb);
extern void espv6_init(void);
extern struct proto espv6_prot;

/* espudp_ipv4.c */
extern int espudp_v4_rcv(struct sk_buff *skb, unsigned short len);
extern void espudp_v4_err(struct sk_buff *skb, unsigned char *dp, int len);
extern struct proto espudp_prot;

/* esptcp_v4.c */
extern void esptcp_v4_err(struct sk_buff *skb, unsigned char *dp, int len);
extern int esptcp_v4_rcv(struct sk_buff *skb, u_short len);

/* esp_dict.c */
extern int esp_encode_queue(struct sock *sk, struct esp_queue_s *q);
extern int esp_encode_packet(struct sock *sk, struct esp_queue_s **queue_ptr,
		ushort asl, u_char **retdata, ushort *retlen, ushort *retdel);
extern int esp_decode(struct sock *sk, u_char *data,      u_int len,      ushort del, u_char **retdata,  u_int *retlen);
extern inline int esp_dict_init_segs(struct sock *sk);
extern inline void esp_dict_free_segs(struct sock *sk);

/* esp_csum.c */
extern inline u_short esp_fast_csum(__u32 seed, char *data, int len);

/* CONFIG_IP_TRANSPARENT_PROXY */
extern int	esp_chkaddr(struct sk_buff *skb);

#endif
