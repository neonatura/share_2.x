
/*
 * INET		An implementation of the TCP/IP protocol suite for the LINUX
 *		operating system.  INET is implemented using the  BSD Socket
 *		interface as the means of communication with the user level.
 *
 *    The Encoded Stream Protocol (ESP).
 *
 * Version:	$Id: esp_ipv4.c,v 2302.24 2002/07/17 23:21:59 root cut root $
 *
 * Author: Brian Burrell, <bburrell@el-i.net>
 *
 * Fixes:
 *
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 */

#include <linux/config.h>
#if defined (CONFIG_INET_ENC) || defined(CONFIG_INET_ENC_MODULE)
 
/* RFC1122 Status:
   4.1.3.1 (Ports):
     SHOULD send ICMP_PORT_UNREACHABLE in response to datagrams to an un-listened port. (YES)
   4.1.3.2 (IP Options)
     MUST pass IP options from IP -> application (OK)
     MUST allow application to specify IP options (OK)
   4.1.3.3 (ICMP Messages)
     MUST pass ICMP error messages to application. (OK)

	 ( ESP CHECKSUMS )
     MUST provide atleast one facility for checksumming (OK)
     MUST allow application to control checksumming behavior (OK)
     MUST default to checksumming ON (OK)
		 MAY attempt retransmission of packets with bad csums. (YES)

	 ( ?? )
     MUST disregard packets with packet parameters errors (OK)
		 MAY disregard packets with errors concerning access violations (YES)
		 MAY disregard packets with errors concerning retransmission (YES)
		 MUST disregard packets with unknown errors (OK)

   4.1.3.5 (esp Multihoming)
     MUST allow application to specify source address (OK)
     SHOULD be able to communicate the chosen src addr up to application
       when application doesn't choose (DOES - use recvmsg cmsgs)

   4.1.3.6 (Invalid Addresses)
     MUST discard invalid source addresses (OK -- done in the new routing code)
     MUST only send data with one of our addresses (OK)
*/

#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/socket.h>
#include <linux/sockios.h>
#include <linux/in.h>
#include <linux/errno.h>
#include <linux/timer.h>
#include <linux/mm.h>
#include <linux/config.h>
#include <linux/inet.h>
#include <linux/netdevice.h>
#include <net/snmp.h>
#include <net/ip.h>
#include <net/protocol.h>
#include <linux/skbuff.h>
#include <net/sock.h>
#include <net/enc.h>
#include <net/icmp.h>
#include <net/route.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/enc.h>
#include <net/encver.h>
#include <net/checksum.h>
#ifdef CONFIG_ES_PROTOCOL_MODULE
#include <linux/module.h>
#endif
#ifdef CONFIG_KMOD
#include <linux/kmod.h>
#endif

#undef NETDEBUG
#define NETDEBUG(_a)

#if 0
int esp_v4_verify_bind(struct sock *sk, unsigned short snum)
{
	struct sock *sk2;
	int reuse1 = sk->sk_reuse;
	int retval = 0;

	sk_for_each (sk, 
	write_lock_bh(&es_hash_lock);
	for(sk2 = es_hash[snum & (ESP_HTABLE_SIZE - 1)]; sk2 != NULL; sk2 = sk_next(sk2)) {
		if((sk2->num == snum) && (sk2 != sk)) {
			unsigned char state = sk2->state;
			int reuse2 = sk2->reuse;

			/* Two sockets can be bound to the same port if they're
			 * bound to different interfaces.
			 */
			if(sk2->bound_dev_if != sk->bound_dev_if)
				continue;

			/* Multiple protocol sets may exist in the same hash table. */
			if (sk2->protocol != sk->protocol)
				continue;

			if(!sk2->rcv_saddr || !sk->rcv_saddr) {
				if( (!reuse2)		||
						(!reuse1)			||
						(state == ESP_LISTEN)) {
					retval = 1;
					break;
				}
			} else if(sk2->rcv_saddr == sk->rcv_saddr) {
				if((!reuse1)			||
				   (!reuse2)			||
				   (state == ESP_LISTEN)) {
					retval = 1;
					break;
				}
			}
		}
	}
	write_unlock_bh(&es_hash_lock);

	NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "NET4: ESP: esp_v4_verify_bind: port %d ret'd %d.\n", (int)snum, retval)));

	return (retval);
}
#endif

struct sock *esp_v4_rcv_lookup(u32 saddr, u16 sport, u32 daddr, u16 dport, int dif, __u32 stream_id)
{
	struct sock *sk, *result = NULL;
  struct hlist_node *node;
  unsigned short hnum = ntohs(dport);
  int badness = -1;

	sk_for_each(sk, node, &es_hash[hnum & (ESP_HTABLE_SIZE - 1)]) {

  sk_for_each(sk, node, &es_hash[hnum & (ESP_HTABLE_SIZE - 1)]) {
    struct inet_sock *inet = inet_sk(sk);
		struct es_sock *af2 = es_sk(sk);

		if (af2->r_id != stream_id)
			continue;

		if (sk->sk_protocol != IPPROTO_ESP)
			continue;

		if ((inet->num == hnum) && 
				!(sock_flag(sk, SOCK_DEAD) && (sk->sk_state == ESP_CLOSE)) &&
				!ipv6_only_sock(sk)) {
			int score = (sk->sk_family == PF_INET ? 1 : 0);
			if (inet->rcv_saddr) {
				if (inet->rcv_saddr != daddr)
					continue;
				score+=2;
			}
			if (inet->daddr) {
				if (inet->daddr != saddr)
					continue;
				score+=2;
			}
			if (inet->dport) {
				if (inet->dport != sport)
					continue;
				score+=2;
			}
			if (sk->sk_bound_dev_if) {
				if (sk->sk_bound_dev_if != dif)
					continue;
				score+=2;
			}
			if(score == 9) {
				result = sk;
				break;
			} else if(score > badness) {
				result = sk;
				badness = score;
			}
		}
	}
	return result;
}

/*
 * This routine is called by the ICMP module when it gets some
 * sort of error condition.  If err < 0 then the socket should
 * be closed and the error returned to the user.  If err > 0
 * it's just the icmp type << 8 | icmp code.  
 * Header points to the ip header of the error packet. We move
 * on past this. Then (as it used to claim before adjustment)
 * header points to the first 8 bytes of the esp header.  
 */
void es_v4_err(struct sk_buff *skb, u32 info)
{
	struct iphdr *iph = (struct iphdr*)skb->data;
	struct esphdr *eh;
	struct inet_sock *inet;
	int type = skb->h.icmph->type;
	int code = skb->h.icmph->code;
	struct sock *sk;
	int err;

	err = 0;

	printk (KERN_DEBUG "NET4: ESP: esp_v4_err: type=%d, code=%d.\n", type, code);

	if (skb->len < (iph->ihl<<2) + sizeof(struct esphdr)) {
		printk (KERN_DEBUG "NET4: ESP: esp_v4_err: icmp packet too small to processi (len=%d).\n", skb->len);
		ICMP_INC_STATS_BH(ICMP_MIB_INERRORS);
		return;
	}

	eh = (struct esphdr*)(skb->data+(iph->ihl<<2));
	sk = esp_v4_rcv_lookup(iph->daddr, eh->dport, iph->saddr, eh->sport, skb->dev ? skb->dev->ifindex : 0, eh->stream_id);
	if (sk == NULL) {
		ICMP_INC_STATS_BH(ICMP_MIB_INERRORS);
		return;	/* No socket for error */
	}

	bh_lock_sock(sk);

	/* If too many ICMPs get dropped on busy
	 * servers this needs to be solved differently.
	 */
	if (sock_owned_by_user(sk))
		NET_INC_STATS_BH(LINUX_MIB_LOCKDROPPEDICMPS);
	
	if (sk->sk_state == ESP_LISTEN) {
		/* socket in error was a listen socket. */
		ICMP_INC_STATS_BH(ICMP_MIB_INERRORS);
		goto out; /* invalid socket for error. */
	}

	if (sk->sk_state == ESP_CLOSE) {
		/* socket in error has already been closed. */
		ICMP_INC_STATS_BH(ICMP_MIB_INERRORS);
		goto out; /* invalid socket for error */
	}

	switch (type) {
		case ICMP_SOURCE_QUENCH: 
			/* icmp request discarded. */
			goto out;
		case ICMP_PARAMETERPROB:
			err = EPROTO;
			break;
		case ICMP_DEST_UNREACH:
			if (code > NR_ICMP_UNREACH)
				goto out;

			if (code == ICMP_FRAG_NEEDED) { /* PMTU Discovery (RFC #1191) */
				if (!sock_owned_by_user(sk))
					do_pmtu_discovery(sk, iph, info);
				goto out;
			}

			err = icmp_err_convert[code].errno;
			break;
		case ICMP_TIME_EXCEEDED:
			err = EHOSTUNREACH;
			break;
		case ICMP_INFO_REQUEST:
			/* obsolete ICMP_INFO_REQUESTS discarded. */
			goto out;
		case ICMP_INFO_REPLY:
			/* obsolete ICMP_INFO_REPLY discarded. */
			goto out;
		case ICMP_ADDRESS:
			/* obsolete ICMP_ADDRESS discarded. */
			goto out;
		case ICMP_ADDRESSREPLY:
			/* obsolete ICMP_ADDRESSREPLY discarded. */
			goto out;
	}

	inet = inet_sk(sk);
	if (!sock_owned_by_user(sk) && inet->recverr) {
		sk->sk_err = err;
		sk->sk_error_report(sk);
	} else  { /* only an error on timeout */
		sk->sk_err_soft = err;
	}

out:
	bh_unlock_sock(sk);
	sock_put(sk);
}

#if 0 /* TODO */
static void __esp_v4_xmit_timer(struct sock *sk)
{
	struct es_sock *af = es_sk(sk);
	struct sk_buff *skb = NULL;
	struct esphdr *eh_p;
	struct esp_queue_s *q = NULL;
	struct iphdr *iph;
	struct esphdr eh;
	ushort pktdata_len, pktdata_del;
	u_char *pktdata = NULL;
	int next_ss, asl, length;
	int pm, flags;
	int msg_flags;
	int xmit_cnt;
	int nextq_size;
	int hh_len; 
	int df, err;

	if (!sk->dst_cache) {
		/* 
		 * [!sk->dst_cache] conditions:
		 * 1. The socket has never initialized a route destination.
		 * 2. The socket is temporarily without a route destination.
		 *
		 * BUG: listen socket should maybe remove the queue timer,
		 *      as it does not need xmits or probes performed. 
		 */
NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "NET4: ESP: DEBUG: esp_v4_xmit_timer: !sk->dst_cache\n")));
		return;
	}

#ifdef CONFIG_ESP_DEVICE_BALANCE
	if (EOPT_ISSET(sk, EOPT_DEVICE_BALANCE) && !sk->bound_dev_if) {
		struct rtable *rt = (struct rtable *)sk->dst_cache;
		struct rtable *new_rt;
		struct device *dev;

		dev = sk->dst_cache->dev->next;
		for (; dev != sk->dst_cache->dev; dev = dev->next) {
			if (!dev)
				dev = dev_base;
			err = ip_route_connect(&new_rt, rt->rt_dst, rt->rt_src, sk->protinfo.af_inet.tos | sk->localroute, dev->ifindex); 
			if (!err) {
				if (new_rt->rt_flags & RTCF_BROADCAST && !sk->broadcast) {
					ip_rt_put(new_rt);
					continue;
				}
				dst_release(xchg(&sk->dst_cache, &new_rt->u.dst));
			}
		}

	}
#endif

	hh_len = (sk->dst_cache->dev->hard_header_len + 15)&~15;
	for (xmit_cnt = 0; xmit_cnt < af->pck_per_burst; xmit_cnt++) {

		/* obtain the next esp queue item for transmission. */
		if (!(q = esp_pull_queue(sk, &af->s_queue)))
			break;

		err = 0;
		pm = q->pm;
		flags = q->esp_flags;
		msg_flags = q->msg_flags;

		if (flags & EOPT_RETRAN) {
			int pkt_seq, pkt_asl;

			memcpy(&eh, &q->eh, sizeof(struct esphdr));
			pkt_seq = ntohs(eh.seq);
			pkt_asl = ntohs(eh.asl);
			next_ss = pkt_asl<pkt_seq ? pkt_asl+65536 : pkt_asl;
			asl = next_ss - pkt_seq;
		} else { /* not a RETRAN packet. */
			if (pm >= ESP_PACKET_MODES) {
				NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "NET4: ESP: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: __esp_v4_xmit_timer: pm >= ESP_PACKET_MODES (%d)\n", ESP_SKADDR(sk), (int)pm)));
				goto error;
			}
			if (!af->s_seq[pm] && !af->ns_seq[pm]) {
				asl = 0; 
			} else {
				next_ss = 
					af->ns_seq[pm]<af->s_seq[pm] ? af->ns_seq[pm]+65536 : af->ns_seq[pm];
				asl = next_ss - af->s_seq[pm]; /* assumed data length */ 
			} 
		}

		if (pm == ESP_DATA) {
			err = esp_encode_packet(sk, &q, asl ? asl : af->msl, 
					&pktdata, &pktdata_len, &pktdata_del);
			if (err) {
				NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "NET4: ESP: esp_encode_packet: error %d.\n", err)));
				goto error;
			}
		} else {
			pktdata_len = q->data_len;
			pktdata = (u_char *)kmalloc(pktdata_len, GFP_ATOMIC);
			if (!pktdata) {
				err = -ENOMEM;
				goto error;
			}
			memcpy(pktdata, q->data_raw, pktdata_len);
			pktdata_del = 0;
		}

		if (!asl)
			asl = pktdata_len;

		if (!(flags & EOPT_RETRAN)) {
			if (pm != ESP_HANDSHAKE) {
				eh.stream_id = af->s_id; /* nbo */
			} else {
				eh.stream_id = ESP_DEF_STREAM_ID; 
			}
			eh.sport      = sk->sport;
			eh.dport      = sk->dport; 
			eh.pm         = pm;            /* packet mode. */
			eh.ver        = ESP_VERSION;
			eh.opt        = (flags & 0xFF);                /* "opt" is 1 byte. */ 
			eh.del        = htons(pktdata_del);
			eh.len        = htons(sizeof(esphdr)+pktdata_len);
			eh.seq        = htons(af->s_seq[pm]);
			af->s_seq[pm] = (__u16)(((int)af->s_seq[pm] + asl) % 65536);
			eh.asl        = htons(af->s_seq[pm]);
			if (pm == ESP_DATA) {
				if (af->s_queue && af->s_queue->pm == ESP_DATA &&
						(nextq_size = min(af->msl, esp_packet_size(sk, af->s_queue))) > 0) {
					af->ns_seq[ESP_DATA] = (__u16)((af->s_seq[pm] + nextq_size) % 65536); 
				} else {
					af->ns_seq[ESP_DATA] = (__u16)((af->s_seq[pm] + af->sl) % 65536);
				}
			} else if (pm == ESP_PROBE) {
				af->ns_seq[ESP_PROBE] = (__u16)(((int)af->s_seq[ESP_PROBE] + af->probe.len) % 65536);
			} else {
				af->ns_seq[pm] = (__u16)(((int)af->s_seq[pm] + pktdata_len) % 65536);
			}
			eh.nsl = (__u16)htons((unsigned short int)af->ns_seq[pm]);
		}

		/* Fast path for ip unfragmented frames without options. */ 
		length = sizeof(struct iphdr) + sizeof(esphdr) + asl;
		skb = sock_alloc_send_skb(sk, length+hh_len+15, 
				0, (msg_flags & MSG_DONTWAIT), &err);
		if (!skb)
			goto error;
		skb_reserve(skb, hh_len);

		skb->priority = sk->priority;
		skb->dst = dst_clone(sk->dst_cache);

		df = 0;
#ifdef ESP_USE_PMTU_DISC
		/* The "ip_dont_fragment()" function handles ipv4 pmtu discovery. */
		if (ip_dont_fragment(sk, sk->dst_cache))
			df = htons(IP_DF);
#else
		/* 
		 * The IPv4 pmtu discovery process is only performed on 
		 * the ESP Probe Packet. 
		 */
		if (pm == ESP_PROBE) {
			df = htons(IP_DF);
		}
#endif

		skb->nh.iph = iph = (struct iphdr *)skb_put(skb, length);
		memset(iph, 0, length);

		dev_lock_list();
		
		iph->saddr      = sk->saddr;
		iph->daddr      = sk->daddr;
		iph->version    = 4;
		iph->ihl        = 5;
		iph->tos        = sk->protinfo.af_inet.tos;
		iph->tot_len    = htons(length);
		iph->id         = htons(ip_id_count++);
		iph->frag_off   = df;
#if 0
		/* no "rt" */
		if (rt->rt_type == RTN_MULTICAST)
			iph->ttl = sk->protinfo.af_inet.mc_ttl;
		else
#endif
			iph->ttl = sk->protinfo.af_inet.ttl;
		iph->protocol   = sk->protocol;

		/* IPv4 checksum. */
		iph->check = 0;
		iph->check = ip_fast_csum((unsigned char *)iph, iph->ihl);

		/* copy ESP header to socket buffer. */
		eh_p = (struct esphdr *)((char *)iph + iph->ihl*4);
		memcpy(eh_p, &eh, sizeof(esphdr));

		/* copy raw packet data to socket buffer. */
		memcpy((char *)iph + iph->ihl*4 + sizeof(esphdr), pktdata, pktdata_len);

		/* ESP Checksum. */
		eh_p->check = ESP_DEF_CHECK;
#if 0
		if (pm != ESP_HANDSHAKE) {
			eh_p->check = esp_check(sk, af->s_id, 
					(char *)iph + iph->ihl*4,
					pktdata_len + sizeof(struct esphdr));
		} 
#endif

		dev_unlock_list();

		memcpy(&eh, eh_p, sizeof(esphdr)); /* for backlog below. */

#ifdef CONFIG_FIREWALL
		{ 
			int fw_status;

			fw_status = call_out_firewall(PF_INET, 
					sk->dst_cache->dev, iph, NULL, &skb);
			switch (fw_status) {
				case FW_REJECT:
					icmp_send(skb, ICMP_DEST_UNREACH, ICMP_PORT_UNREACH, 0);
				case FW_BLOCK:
					err = -EPERM;
					goto error;
				case FW_QUEUE:
					/* continue without error. */
					kfree_skb(skb);
					goto next;
			}
		}
#endif

		if (af->rem_host) /* CONF */
			af->rem_host->sw = af->rem_host->sw - asl;

NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: DEBUG: xmit %d size packet of mode %d.\n", length, pm)));

		/* transmit socket buffer */
		sk->dst_cache->output(skb);
		esp_statistics.EspOutPackets++;
		if (pm == ESP_DATA)
			esp_statistics.EspOutDataPackets++;

next:
		if (!(flags & EOPT_RETRAN)) {
			/* backlog packet */ 
			err = esp_backlog_packet(sk, &pktdata, pktdata_len, &eh, flags);
			if (err)
				goto error;
		} else {
			/* Free local copy of raw trasmit data. */
			kfree(xchg(&pktdata, NULL));
		}

		kfree_queue(&q);
	}

	if (xmit_cnt == af->pck_per_burst && /* max packets per burst was sent. */
			!af->pck_loss_rate) {            /* no packets were lost "lately". */
		/* increment the number of packets sent per 'burst'. */
		af->pck_per_burst += ESP_INCR_BURST_RATE;
	}

	return;

error:
	NETDEBUG(printk(KERN_DEBUG "NET4: ESP: __esp_queue_timer: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: discarded an IP packet (err=%d).\n", ESP_SKADDR(sk), err));
	sk->err = -err;
	if (pktdata)
		kfree(pktdata);
	if (skb)
		kfree_skb(skb);
	kfree_queue(&q);
	ip_statistics.IpOutDiscards++;
}
#endif

#define TENTH_HZ (HZ/10)
static inline void __esp_v4_queue_timer(struct sock *sk)
{
	struct es_sock *af = es_sk(sk);

	__esp_v4_xmit_timer(sk);
	mod_timer(&af->queue_timer, jiffies + TENTH_HZ);
}
#undef TENTH_HZ

#if 0 /* TODO */
static void __esp_v4_default_timer(struct sock *sk)
{
	struct es_sock *af = es_sk(sk);
	unsigned long int now = jiffies;

	if (sock_flag(sk, SOCK_DEAD) || 
			sk->done || sk->sk_state != ESP_ESTABLISHED)
		goto done;

	/* Probe Timer */
	if (af->probe.timeout > 0)
		af->probe.timeout = af->probe.timeout - 1;
	if (af->probe.timeout == 0) {
		esp_probe_sendlen(sk);
		af->probe.timeout = af->probe.tinterval;
	}

done:
	if (now + HZ > jiffies) {
		mod_timer(&af->timer, now + HZ);
	} else {
		mod_timer(&af->timer, jiffies + HZ);
	}
}
#endif

static inline int esp_v4_ioctl(struct sock *sk, int cmd, unsigned long arg)
{
	return (esp_ioctl(sk, cmd, arg));
}

static int esp_v4_recvmsg(struct sock *sk, struct msghdr *msg, int len,
		int noblock, int flags, int *addr_len)
{
	int err;

	if (addr_len) {
		*addr_len = sizeof(struct sockaddr_in);
	}

	if (flags & MSG_ERRQUEUE) {
		return (ip_recv_error(sk, msg, len));
	}

	err = esp_recvmsg(sk, msg, len, noblock, flags);
	if (err)
		return (err);

	return (0);
}

static inline int esp_v4_sendmsg(struct sock *sk, struct msghdr *msg, int len)
{
	return (esp_sendmsg(sk, msg, len));
}

/*
 * todo: may need to est socket to ESP_CLOSE upon failure.
 */
int esp_v4_connect(struct sock *sk, struct sockaddr *uaddr, int addr_len)
{
	struct inet_sock *inet = inet_sk(sk);
	struct sockaddr_in *usin = (struct sockaddr_in *)uaddr;
	struct rtable *rt = NULL;
	u32 daddr, nexthop;
	int err;

	if (addr_len < sizeof(struct sockaddr_in))
		return -EINVAL;

	if (usin->sin_family != AF_INET)
		return -EAFNOSUPPORT;

	nexthop = daddr = usin->sin_addr.s_addr;
	if (inet->opt && inet->opt->srr) {
		if (!daddr)
			return -EINVAL;
		nexthop = inet->opt->faddr;
	}

	err = ip_route_connect(&rt, nexthop, inet->saddr,
			RT_CONN_FLAGS(sk), sk->sk_bound_dev_if,
			IPPROTO_ESP, inet->sport, usin->sin_port, sk);
	if (err < 0)
		return err;

	if (rt->rt_flags & (RTCF_MULTICAST | RTCF_BROADCAST)) {
		ip_rt_put(rt);
		return -ENETUNREACH;
	}

	if (!inet->opt || !inet->opt->srr)
		daddr = rt->rt_dst;
	if (!inet->saddr)
		inet->saddr = rt->rt_src;		/* Update source address */
	inet->rcv_saddr = inet->saddr;
	inet->daddr = daddr;
	inet->dport = usin->sin_port;

	err = ip_route_newports(&rt, inet->sport, inet->dport, sk);
	if (err) {
		ip_rt_put(rt);
		return (err);
	}

	/* commit destination to socket. */
	sk_setup_caps(sk, &rt->u.dst);

	esp_attach_remote_host(sk);
	esp_set_state(sk, ESP_CONN_WAIT);

	return (0);
}

#ifdef CONFIG_IP_TRANSPARENT_PROXY
/*
 *	Check whether a received esp packet might be for one of our
 *	sockets.
 */

int esp_chkaddr(struct sk_buff *skb)
{
	struct iphdr *iph = skb->nh.iph;
	struct esphdr *eh = (struct esphdr *)(skb->nh.raw + iph->ihl*4);
	struct sock *sk;

	sk = esp_v4_rcv_lookup(iph->saddr, eh->sport, iph->daddr, eh->dport, skb->dev ? skb->dev->ifindex : 0, eh->stream_id);
	if (!sk)
		return 0;

	/* 0 means accept all LOCAL addresses here, not all the world... */
	if (sk->rcv_saddr == 0)
		return 0;

	return 1;
}
#endif

int es_v4_rcv(struct sk_buff *skb, unsigned short len)
{
	struct esp_opt *af = NULL;
	struct sock *sk = NULL;
	int pm, seq, asl, nsl;
	u_short elen;
	ushort csum;
	int err = 0;

NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: ip packet received.\n")));
	__skb_pull(skb, skb->h.raw - skb->data);

	if (len < sizeof(struct esphdr)) {
		err = -EBADMSG;
		goto done;
	}

	elen = ntohs(skb->h.eh->len);
	pm   = skb->h.eh->pm;
	seq  = ntohs(skb->h.eh->seq);
	asl  = ntohs(skb->h.eh->asl);
	nsl  = ntohs(skb->h.eh->nsl);
	csum = skb->h.eh->check; /* nbo */

#if 0
#ifdef CONFIG_IP_TRANSPARENT_PROXY
	if (IPCB(skb)->redirport) 
		sk = esp_v4_skb_proxy_lookup(skb);
	else
#endif
#endif
	sk = esp_v4_skb_rcv_lookup(skb);
	if (!sk) {

	NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "NET4: ESP: received packet from %d.%d.%d.%d:%d to %d.%d.%d.%d:%d on device %d with pm %d while in state %s that could not be delivered.\n", NIPQUAD(skb->nh.iph->saddr), ntohs((u_short)skb->h.eh->sport), NIPQUAD(skb->nh.iph->daddr), ntohs(skb->h.eh->dport), skb->dev ? skb->dev->ifindex : 0, pm, ESP_STATE_STRING(sk->sk_state))));

		esp_statistics.EspNoPorts++;
		icmp_send(skb, ICMP_DEST_UNREACH, ICMP_PORT_UNREACH, 0);
		err = -EHOSTUNREACH;
		goto done;
	}
	af = (struct esp_opt *)&sk->tp_pinfo.af_esp;

#if 0
	NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "NET4: ESP: received packet from %d.%d.%d.%d:%d to %d.%d.%d.%d:%d on device %d with pm %d while in state %s.\n", NIPQUAD(skb->nh.iph->saddr), ntohs((u_short)skb->h.eh->sport), NIPQUAD(skb->nh.iph->daddr), ntohs(skb->h.eh->dport), skb->dev ? skb->dev->ifindex : 0, pm, ESP_STATE_STRING(sk->sk_state))));
#endif

	if (elen < sizeof(esphdr) || elen > ESP_MAX_DATA_SIZE) {
		err = ESE_INVALSIZEDATA;
		esp_error(sk, skb, E_LENGTH, err);
		goto done;
	} 

	/* Validate the packet and the esp length. */
	if (elen > len) {
		NETDEBUG(printk(KERN_DEBUG "NET4: ESP: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: short packet: eh=%d/ip=%d\n", ESP_SKADDR(sk), elen, len));
		err = ESE_TRUNCDATA;
		esp_error(sk, skb, E_LENGTH, err);
		goto done;
	}

#if 0
	if (pm != ESP_HANDSHAKE) {
		skb->h.eh->check = ESP_DEF_CHECK;
		skb->csum = esp_check(sk, af->r_id, (u_char *)skb->h.eh, elen);
	} else {
		skb->csum = ESP_DEF_CHECK;
	}
#else
	skb->csum = ESP_DEF_CHECK;
#endif
	if (skb->csum != csum) {
		err = ESE_INVALCRC;
		esp_error(sk, skb, E_CHECKSUM, err);
		goto done;
	} 

	if (sk->sk_state != ESP_LISTEN) {
		/* process a incoming IPv4/IPv6 packet and free the skb. */
		err = esp_rcv(sk, &skb, len, pm);
	} else {
		err = esp_listen_rcv(sk, &skb, pm);
	}

done:
	if (err > 0) {
		esp_statistics.EspInErrors++;
		if (sk) {
			NETDEBUG(printk(KERN_DEBUG "NET4: ESP: esp_v4_rcv: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: esp error %d occurred.\n", ESP_SKADDR(sk), err));
		} else {
			NETDEBUG(printk(KERN_DEBUG "NET4: ESP: esp_v4_rcv: esp error %d occurred.\n", err));
		}
	} else if (err < 0) {
		esp_statistics.EspInErrors++;
		if (sk) {
			NETDEBUG(printk(KERN_DEBUG "NET4: ESP: esp_v4_rcv: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: error %d occurred.\n", ESP_SKADDR(sk), err));
		} else {
			NETDEBUG(printk(KERN_DEBUG "NET4: ESP: esp_v4_rcv: error %d occurred.\n", err));
		}
	}
	if (skb)
		kfree_skb(skb);
	return (0);
}

static inline void esp_v4_close(struct sock *sk, long timeout)
{
	return (esp_close(sk, timeout));
}

static void esp_v4_init_timers(struct sock *sk)
{
	struct esp_opt *af       = &sk->tp_pinfo.af_esp;

	/* ipv4 timer */
	sk->timer.data           = (unsigned long) sk;
	sk->timer.function       = &net_timer;

	/* esp queue timer */
	init_timer(&af->queue_timer);
	af->queue_timer.expires   = jiffies + HZ/10;
	af->queue_timer.function  = (void (*)(unsigned long)) __esp_v4_queue_timer;
	af->queue_timer.data      = (unsigned long) sk;
	add_timer(&af->queue_timer);

	/* esp default timer */
	init_timer(&af->timer);
	af->timer.expires        = jiffies + HZ;
	af->timer.function       = (void (*)(unsigned long)) __esp_v4_default_timer;
	af->timer.data           = (unsigned long) sk;
	add_timer(&af->timer);

	/* Probe Timer */
	af->probe.timeout        =
	af->probe.tinterval      = 0;
}

static int esp_v4_init_sock(struct sock *sk)
{
	struct es_sock *af = es_sk(sk);
	int err;
	
	esp_v4_init_timers(sk);

	err = esp_init_sock(sk);
	if (err)
		return (err);

	sk->protinfo.af_inet.ttl = MAXTTL;

	return (0);
}

static inline int esp_v4_destroy_sock(struct sock *sk)
{
	return (esp_destroy_sock(sk));
}

static int esp_v4_setsockopt(struct sock *sk, int level, int optname, char *optval, int optlen)
{
	struct es_sock *af = es_sk(sk);
	unsigned long int val=0;
	struct device *dev;
	struct rtable *rt = (struct rtable *)sk->dst_cache, *newrt;
	int err;
	int i;

	if (level != SOL_ESP)
		return (ip_setsockopt(sk, level, optname, optval, optlen));

	if (optlen >= sizeof(unsigned long int)) {
		if(get_user(val, (unsigned long int *) optval))
			return (-EFAULT);
	} else if (optlen >= sizeof(char)) {
		unsigned char ucval;
		if (get_user(ucval, (unsigned char *) optval))
			return (-EFAULT);
		val = (int)ucval;
	}
	/* If optlen==0, it is equivalent to val == 0 */

	switch (optname) {
		case IPESP_CHECKSUM:
			if (sk->sk_state == ESP_ESTABLISHED)
				return (-EISCONN);
			for (i=1; esp_checksum_opt[i] >= 0; i++)
				EOPT_UNSET(sk, esp_checksum_opt[i]);
			if (val) {
				val = min(i - 1, val);
				EOPT_SET(sk, esp_checksum_opt[val]);
			}
			break;
		case IPESP_NETDEVICE:
			if (!val) {
				/* 0 = No device be bound to socket stream. */
				sk->bound_dev_if = 0;
			} else {
				if (!rt) 
					return (-ENOTCONN);
				dev = dev_get_by_index((int)val);
				if (!dev)
					return (-ENODEV);
				err = ip_route_connect(&newrt, rt->rt_dst, rt->rt_src,
						sk->protinfo.af_inet.tos|sk->localroute, dev->ifindex);
				if (err)
					return (err);
				if ((newrt->rt_flags&RTCF_BROADCAST) && !sk->broadcast) {
					ip_rt_put(newrt);
					return (-EACCES);
				}
				dst_release(xchg(&sk->dst_cache, &newrt->u.dst));
				sk->bound_dev_if = dev->ifindex; 
			}
			break;
#if 0
		case IPESP_SOURCEADDR:
			if (!rt)
				return (-ENOTCONN);
			dev = ip_dev_find(val);
			if (!dev || (sk->bound_dev_if && sk->bound_dev_if != dev->ifindex))
				return (-EADDRNOTAVAIL);
			err = ip_route_connect(&newrt, rt->rt_dst, val, 
					sk->protinfo.af_inet.tos|sk->localroute, dev->ifindex);
			if (err)
				return (err);
			if ((newrt->rt_flags&RTCF_BROADCAST) && !sk->broadcast) {
				ip_rt_put(newrt);
				return (-EACCES);
			}
			dst_release(&newrt->u.dst);
			err = esp_request(sk, NULL, EREQ_SOCKET_DESTIPV4, &val);
			if (err)
				return (err);
			break;
#endif
		case IPESP_GATEWAY:
			if (!rt) 
				return (-ENOTCONN);
			dev = ip_dev_find(val);
			if (!dev || (sk->bound_dev_if && sk->bound_dev_if != dev->ifindex))
				return (-EADDRNOTAVAIL);
			err = ip_route_connect(&newrt, rt->rt_dst, rt->rt_src,
					sk->protinfo.af_inet.tos|sk->localroute, dev->ifindex);
			if (err)
				return (err);
			if ((newrt->rt_flags&RTCF_BROADCAST) && !sk->broadcast) {
				ip_rt_put(newrt);
				return (-EACCES);
			}
			dst_release(xchg(&sk->dst_cache, &newrt->u.dst));
			break;
		case IPESP_LOCAL_ERROR:
			if (val >= ESP_MAX_ERROR_CODES)
				return (-EINVAL);
			af->lcl_error = val;
			break;
		case IPESP_REMOTE_ERROR:
			if (val >= ESP_MAX_ERROR_CODES)
				return (-EINVAL);
			af->rem_error = val;
			break;
		case IPESP_COMPRESS:
			if (sk->sk_state != ESP_CLOSE)
				return (-EINVAL);
			for (i=1; esp_compress_opt[i] > 0; i++)
				EOPT_UNSET(sk, esp_compress_opt[i]);
			val = min(i - 1, val);
			if (val)
				EOPT_SET(sk, esp_compress_opt[val]);
			break;
		case IPESP_VERBOSE_DEBUG:
			sk->debug = !!val;
			break;
#ifndef CONFIG_ESP_SENDWINDOW
		case IPESP_MAXBPS:
			if (val > ESP_MAX_DATA_WINDOW)
				return (-EINVAL);
			if (sk->sk_state == ESP_ESTABLISHED) {
				err = esp_request(sk, NULL, EREQ_SOCKET_BPS, val);
				if (err)
					return (err);
			}
			af->locked_msw = val;
			break;
#endif
		default:
			return (-EINVAL);
	}

	return (0);
}		

static int esp_v4_getsockopt(struct sock *sk, int level, int optname, char *optval, int *optlen)
{
	struct es_sock *af = es_sk(sk);
	struct esp_remote_host *rh = af->rem_host;
	struct rtable *rt = (struct rtable *)sk->dst_cache;
	unsigned long int val;
	int len;
	int i;

	if (level != SOL_ESP)
		return (ip_getsockopt(sk, level, optname, optval, optlen));

	switch (optname) {
		case IPESP_CHECKSUM:
			val = 0;
			for (i=1; esp_checksum_opt[i] >= 0; i++) {
				if (EOPT_ISSET(sk, esp_checksum_opt[i])) {
					val = i;
					break;
				}
			}
			break;
#ifdef CONFIG_ESP_EXPERIMENTAL
		case IPESP_SOURCEADDR:
			if (!rt)
				return (-ENOTCONN);
			val = rt->rt_src;
			break;
#endif
		case IPESP_GATEWAY:
			if (!rt)
				return (-ENOTCONN);
			val = rt->rt_gateway;
			break;
		case IPESP_NETDEVICE:
			val = sk->bound_dev_if;
			break;
		case IPESP_LOCAL_ERROR:
			val = af->lcl_error;
			break;
		case IPESP_REMOTE_ERROR:
			val = af->rem_error; 
			break;
		case IPESP_COMPRESS:
			val = 0;
			for (i=1; esp_compress_opt[i] >= 0; i++) {
				if (EOPT_ISSET(sk, esp_compress_opt[i])) {
					val = i;
					break;
				}
			}
			break;
		case IPESP_VERBOSE_DEBUG:
			val = sk->debug;
			break;
		case IPESP_VERSION:
			val = ESP_VERSION;
			break;
#ifdef CONFIG_ESP_SENDWINDOW
		case IPESP_MAXBPS:
			val = af->locked_msw;
			break;
#endif
		default:
			return (-EINVAL);
	}

	if (get_user(len, optlen))
		return (-EFAULT);

	if (len < sizeof(unsigned long int) && len > 0 && val>=0 && val<255) {
		unsigned char ucval = (unsigned char)val;
		len = 1;
		if (put_user(len, optlen))
			return (-EFAULT);
		if (copy_to_user(optval, &ucval, 1))
			return (-EFAULT);
	} else {
		len = min(sizeof(unsigned long int), len);
		if (put_user(len, optlen))
			return (-EFAULT);
		if (copy_to_user(optval, &val, len))
			return (-EFAULT);
	}

	return (0);
}

static inline struct sock *esp_v4_accept(struct sock *sk, int flags)
{
	return (esp_accept(sk, flags));
}

static inline unsigned int esp_v4_poll(struct file *file, struct socket *sock, poll_table *wait)
{
	return (esp_poll(file, sock, wait));
}

struct proto enc_prot = {
	.name = "ENC",
	.owner = THIS_MODULE,
	.close = esp_v4_close,
	.connect = esp_v4_connect,
	.disconnect = es_disonnect, 
	.accept = esp_v4_accept,
	.ioctl = esp_v4_ioctl,
	.init = esp_v4_init_sock,
	.destroy = esp_v4_destroy_sock,
	.setsockopt = esp_v4_setsockopt,
	.getsockopt = esp_v4_getsockopt,
	.sendmsg = esp_v4_sendmsg,
	.recvmsg = esp_v4_recvmsg,
	.backlog_rcv = esp_rcv_skb,
	.hash = esp_hash_sock,
	.unhash = esp_unhash_sock,
	.get_port = esp_get_port, /* good_socknum */
	.max_header = 128,
	.obj_size = sizeof(struct es_sock)
};


#endif
