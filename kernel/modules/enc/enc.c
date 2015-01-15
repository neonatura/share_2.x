
/*
 * INET	
 *    An protocol implementation for the IPv4 protocol suite for the 
 *    LINUX operating system. INET is implemented using the  BSD Socket
 *		interface as the means of communication with the user level. ESP
 *    was implemented by Brian Burrell.
 *
 *    The Encoded Stream Protocol (ESP).
 *
 * Version:	$Id: esp.c,v 2302.78 2002/07/17 23:21:57 root cut root $
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
#if defined (CONFIG_ES_PROTOCOL) || defined (CONFIG_ES_PROTOCOL_MODULE) 

#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/socket.h>
#include <linux/sockios.h>
#include <linux/in.h>
#include <linux/errno.h>
#include <linux/timer.h>
#include <linux/mm.h>
#include <linux/config.h>
#include <linux/inet.h>
#include <linux/netdevice.h>
#include <net/inet_common.h>
#include <net/snmp.h>
#include <net/ip.h>
#include <net/protocol.h>
#include <linux/skbuff.h>
#include <net/sock.h>
#include <net/enc.h>
#include <net/encver.h>
#include <net/icmp.h>
#include <net/route.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/enc.h>
#include <net/checksum.h>
#if defined(CONFIG_IPV6) || defined (CONFIG_IPV6_MODULE)
#include <net/flow.h>
#include <net/ipv6.h>
#include <net/transp_v6.h>
#include <net/ip6_route.h>
#endif

#undef NETDEBUG
#define NETDEBUG(_a)

struct sock *es_hash[ESP_HTABLE_SIZE];
DEFINE_RWLOCK(es_hash_lock);

struct esp_mib esp_statistics;
struct esp_mib esp_stat6;

__u32 esp_sock_id(long salt)
{
	static unsigned long id = 0xF;

	/* create a unique identifier for socket stream. */
	id = (id + salt) % 0xFFFFFFFF;

	return ((__u32)id);
}

/* perform a ESP request. */
int esp_request(struct sock *sk, struct sk_buff *skb, int request, u_long value)
{
	u_long value_ar[4];
	int err;

	memset(value_ar, 0, sizeof(u_long) * 4);

	/* packet mode */
	if (skb) {
		value_ar[0] = (u_long)skb->h.eh->pm; //ntohs(skb->h.eh->pm);
	}

	switch (request) {
		case EREQ_PACKET:
			value_ar[1] = skb->h.eh->seq; /* seq */
			value_ar[2] = skb->h.eh->asl; /* asl */
			value_ar[3] = value;
			break;
		case EREQ_SOCKET_BPS:
			value_ar[2] = value; /* size */
			break;
	}
															
	err = espsend(sk, ESP_REQUEST, request, &value_ar, skb);
	if (err)
		return (err);

	return (0);
}

/* declares a stream error */
int esp_error(struct sock *sk, struct sk_buff *skb, int err_type, int err_code)
{
	struct esp_sock *af = es_sk(sk);
	int pm = skb->h.eh->pm; //ntohs(skb->h.eh->pm);
	int err;

	/* declare error locally. */
	af->lcl_error = err_code;

	/* declare error remotely. */
	err = espsend(sk, ESP_ERROR, err_type, &err_code, skb);
	if (err)
		return (err);

	/* some stream errors require a packet retransmit. */
	switch (err_code) {
		case ESE_INVALSEQ:  /* invalid packet sequence */
		case ESE_INVALCRC:  /* invalid packet checksum */
		case ESE_TRUNCDATA: /* truncated data packet   */
			/* increment "packet loss rate" for missing packet. */
			af->pck_loss_rate++;
			/* request packet retransmission. */
			err = esp_request(sk, skb, EREQ_PACKET, err_code);
			if (err)
				return (err);
			break;
	}

	return (0);
}

/* Create a esp queue. */
int kmalloc_queue(unsigned char *data, size_t len, struct esp_queue_s **queue)
{
	struct esp_queue_s *q;

	if (!data || !len)
		return (-EINVAL);

	q = (struct esp_queue_s *)kmalloc(sizeof(struct esp_queue_s), GFP_ATOMIC);
	if (!q)
		return (-ENOMEM);
	memset(q, 0, sizeof(struct esp_queue_s));

	q->data_raw = data;
	q->data_len = len;

	*queue = q;
	return (0);
}

/* Queue a accepting socket. */ 
int push_accept_sock(struct sock *listen_sk, struct sock *accept_sk)
{
	struct esp_opt *af = &listen_sk->tp_pinfo.af_esp;
	int i;

	for (i=0; i<listen_sk->ack_backlog; i++)
		if (!af->listen.queue[i])
			break;

	if (listen_sk->ack_backlog == i) {
		if (listen_sk->ack_backlog + 1 >= listen_sk->max_ack_backlog)
			return (-EAGAIN);
		listen_sk->ack_backlog++;
	}

	af->listen.queue[i] = accept_sk;
	return (0);
}

/* Remove an accept sock. */
void remove_accept_sock(struct sock *sk, struct sock *remove_sock)
{
	struct esp_sock *af = es_sk(sk);
	int i;

	if (!af->listen.queue) {
		NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: remove_accept_sock: listen queue is 0x0.\n", ESP_SKADDR(sk))));
		return;
	}

	if (sk->sk_state != ESP_LISTEN) {
		/* listen sock state's is not ESP_LISTEN. */
		NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: DEBUG: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: remove_accept_sock: listen state is %s, not ESP_LISTEN.\n", ESP_SKADDR(sk), ESP_STATE_STRING(sk->sk_state))));
		return;
	}

	lock_sock(sk);
	for (i=0; i<sk->ack_backlog; i++) {
		if (af->listen.queue[i] == remove_sock) {
			af->listen.queue[i] = NULL;
			break;
		}
	}
	release_sock(sk);

}

/* Dequeue a accepting socket. */
struct sock *pull_accept_sock(struct sock *sk)
{
	struct esp_sock *af = es_sk(sk);
	struct sock *ret_sock = NULL;
	int i;

	if (!sk->max_ack_backlog) {
		NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: DEBUG: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: pull_accept_sock: max ack backlog is zero.\n", ESP_SKADDR(sk))));
		return (NULL);
	}

#if 0
	if (sk->sk_state != ESP_LISTEN) {
		NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: DEBUG: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: pull_accept_sock: listen state is %s, not ESP_LISTEN.\n", ESP_SKADDR(sk), ESP_STATE_STRING(sk->sk_state))));
		return (NULL);
	}
#endif

	if (!af->listen.queue) {
		NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: pull_accept_sock: listen queue is 0x0, max_ack_backlog is %d.\n", ESP_SKADDR(sk), sk->max_ack_backlog)));
		return (NULL);
	}

	lock_sock(sk);
	for (i=0; i<sk->ack_backlog; i++)
		if (af->listen.queue[i] != NULL &&
				af->listen.queue[i]->state == ESP_ESTABLISHED) {
			ret_sock = af->listen.queue[i];
			break;
		}
	release_sock(sk);

	if (!ret_sock)
		return (NULL); /* no sock found. */

	af->listen.queue[i] = NULL;
	return (ret_sock);
}

/* Allocate a new accepting socket. */
int esp_create_accept_socket(struct sock *sk, struct sk_buff *skb)
{
	struct rtable *rt = (struct rtable *)skb->dev;
	struct rtable *new_rt;
	struct esp_opt *newaf;
	struct sock *newsk = NULL;
	ushort reqlen;
	int err = 0;
#if defined(CONFIG_IPV6) || defined (CONFIG_IPV6_MODULE)
	struct flowi fl;
#endif

	if (!(newsk = sk_alloc(PF_INET, GFP_ATOMIC, 0))) {
		err = -ENOMEM;
		goto error;
	}
	newaf = &newsk->tp_pinfo.af_esp;

	memcpy(newsk, sk, sizeof(*newsk));
	memset(&newsk->tp_pinfo.af_esp, 0, sizeof(struct esp_opt));
	sock_init_data(NULL, newsk);

	if (newsk->prot->init) {
		err = newsk->prot->init(newsk);
		if (err)
			goto error;
	}

	err = push_accept_sock(sk, newsk);
	if (err) {
		/* Too many pending connections in accept queue. */
		icmp_send(skb, ICMP_DEST_UNREACH, ICMP_PORT_UNREACH, 0); 
		goto error;
	}

	/* reset */
	newsk->done = 0;
	newsk->proc = 0;
	newsk->pair = NULL;
	newsk->protinfo.af_inet.mc_loop=1;
	newsk->protinfo.af_inet.mc_ttl=1;
	newsk->protinfo.af_inet.mc_index=0;
	newsk->protinfo.af_inet.mc_list = NULL;

	/* sanity */
	newsk->pprev = NULL;
	newsk->prev = NULL;

	atomic_set(&newsk->rmem_alloc, 0);
	atomic_set(&newsk->wmem_alloc, 0);
	atomic_set(&newsk->omem_alloc, 0);

#if defined(CONFIG_IPV6) || defined (CONFIG_IPV6_MODULE)
	if (sk->family == PF_INET6) {
		struct ipv6_pinfo *np = &sk->net_pinfo.af_inet6;

		fl.proto = IPPROTO_ESP;
		fl.nl_u.ip6_u.daddr = &skb->nh.ipv6h->daddr;
		if (np->opt && np->opt->srcrt) {
			struct rt0_hdr *rt0 = (struct rt0_hdr *)np->opt->srcrt;
			fl.nl_u.ip6_u.daddr = rt0->addr;
		}
		fl.nl_u.ip6_u.saddr = &skb->nh.ipv6h->saddr;
		fl.fl6_flowlabel = 0;
		fl.oif = sk->bound_dev_if;
		fl.uli_u.ports.dport = skb->h.eh->sport;
		fl.uli_u.ports.sport = skb->h.eh->dport;
		dst = ip6_route_output(sk, &fl);
		if (dst->error) {
			err = -EHOSTUNREACH;
			goto error;
		}
		ip6_dst_store(newsk, dst, NULL);
		newsk->daddr      = LOOPBACK4_IPV6;
		newsk->saddr      = LOOPBACK4_IPV6;
		newsk->rcv_saddr  = LOOPBACK4_IPV6;
		ipv6_addr_copy(&newsk->net_pinfo.af_inet6.daddr, &skb->nh.ipv6h->daddr);
		ipv6_addr_copy(&newsk->net_pinfo.af_inet6.saddr, &skb->nh.ipv6h->saddr);
		ipv6_addr_copy(&newsk->net_pinfo.af_inet6.rcv_saddr, &skb->nh.ipv6h->saddr);
	} else
#endif
	{
		if (!rt)
			return (-EINVAL);
		err = ip_route_connect(&new_rt, rt->rt_dst, 0,
				RT_TOS(newsk->protinfo.af_inet.tos)|newsk->localroute, 
				skb->dev->ifindex);
		if (err)
			goto error;
		newsk->dst_cache = &new_rt->u.dst;
		newsk->route_caps = newsk->dst_cache->dev->features;
		newsk->saddr = skb->nh.iph->daddr;
		newsk->rcv_saddr = skb->nh.iph->daddr;
		newsk->daddr = skb->nh.iph->saddr;
	}

	newsk->sport = skb->h.eh->dport;
	newsk->num = ntohs(newsk->sport);
	newsk->dport = skb->h.eh->sport;
	newsk->bound_dev_if = 0;
	/* pass on EREQ_SOCKET_OPEN sequence. */
	reqlen = (u_short)ntohs(skb->h.eh->len) - (u_short)sizeof(struct esphdr);
	newaf->r_seq[ESP_REQUEST] += reqlen;
	newaf->listen.sock = sk;

	newsk->data_ready(newsk, 0);
	esp_attach_remote_host(newsk);
	newsk->prot->hash(newsk);
#if LINUX_VERSION_CODE <= 0x020301  /* less then or equal to 2.3.x */ 
	add_to_prot_sklist(newsk);
#endif
	esp_set_state(newsk, ESP_CONN_ACK);

	return (0);

error:
	NETDEBUG(printk(KERN_DEBUG "NET4: ESP: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: esp_create_accept_socket error %d.\n", ESP_SKADDR(sk), err));
	if (newsk) {
		esp_set_state(sk, ESP_CLOSE);
	}
	return (err);
}

static int esp_valid_seq(struct sock *sk, struct sk_buff *skb, size_t len)
{
	struct esp_sock *af = es_sk(sk);
	u_short seq, asl, nsl;
	int err;
	int pm;

	pm  = skb->h.eh->pm;
	seq = ntohs(skb->h.eh->seq);
	asl = ntohs(skb->h.eh->asl);
	nsl = ntohs(skb->h.eh->nsl);

	if (af->r_seq[pm] && seq != af->r_seq[pm]) {
		err = ESE_INVALSEQ;
		goto error;
	}
	if (asl != ((af->r_seq[pm]+(__u16)(len-sizeof(esphdr))) % 65536)) {
		err = ESE_INVALSEQ;
		goto error;
	}
	if (af->nr_seq[pm] && af->nr_seq[pm] != asl) {
		err = ESE_INVALSEQ;
		goto error;
	}

	/* update data sequence */
	af->r_seq[pm] = asl;
	af->nr_seq[pm] = nsl;

	return (0);

error:
	esp_error(sk, skb, E_QUEUE, err);
	return (err);
}

/* TODO: ??? */
int es_disconnect(struct sock *sk, int flags)
{
  struct inet_sock *inet = inet_sk(sk);
  /*
   *  1003.1g - break association.
   */

  sk->sk_state = TCP_CLOSE;
  inet->daddr = 0;
  inet->dport = 0;
  sk->sk_bound_dev_if = 0;
  if (!(sk->sk_userlocks & SOCK_BINDADDR_LOCK))
    inet_reset_saddr(sk);

  if (!(sk->sk_userlocks & SOCK_BINDPORT_LOCK)) {
    sk->sk_prot->unhash(sk);
    inet->sport = 0;
  }
  sk_dst_reset(sk);
  return 0;
}


/* Terminate a IPv4/IPv6 ESP socket stream. */
void esp_close(struct sock *sk, long timeout)
{
  struct esp_sock *af = es_sk(sk);
  struct task_struct *tsk = current;
  DECLARE_WAITQUEUE(wait, tsk);

  if (sock_flag(sk, SOCK_DEAD))
    return;

  if (sk->sk_state == ESP_ESTABLISHED && !sk->done) {
    esp_set_state(sk, ESP_CLOSE_WAIT);
    sk->done = 1;

    while (skb_peek(&sk->write_queue) || af->s_queue) {
      if (sock_error(sk))
        break;

      if (signal_pending(tsk))
        break;

      tsk->state = TASK_INTERRUPTIBLE;
      add_wait_queue(sk->sleep, &wait);
      schedule();
      tsk->state = TASK_RUNNING;
      remove_wait_queue(sk->sleep, &wait);
		}
	}

	esp_set_state(sk, ESP_CLOSE);

	NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: esp_close: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: socket closed.\n", ESP_SKADDR(sk))));

}

/*
 * Arguments:
 *   data       - packet data.
 *   data_len   - packet data length.
 *   buff_len   - overflow'd buffer remaining length.
 */
int esp_recvmsg_overflow(struct sock *sk, u_char *data, u_int data_len, u_int buff_len)
{
	struct esp_sock *af = es_sk(sk);
	struct esp_queue_s *q;
	u_char *raw;
	u_int len;
	int err;

	if (buff_len >= data_len)
		return (0);

	len = data_len - buff_len; 
	raw = (u_char *)kmalloc(len, GFP_ATOMIC);
	if (!raw)
		return (-ENOMEM);
	memcpy(raw, data + buff_len, len); 

	err = kmalloc_queue(raw, len, &q);
	if (err)
		return (err);

	esp_push_queue(&af->r_overflow, q);

	NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: esp_recvmsg_overflow: stored %d bytes in overflow receive queue.\n", ESP_SKADDR(sk), (int)len)));

	return (0);
}

/* Receives stream data to the user-level. */
int esp_recvmsg(struct sock *sk, struct msghdr *msg, int len, 
		int noblock, int flags)
{
	struct esp_sock *af = es_sk(sk);
	struct task_struct *tsk = current;
	DECLARE_WAITQUEUE(wait, tsk);
	struct sockaddr_in *sin;
	struct sk_buff *skb = NULL;
	struct esp_queue_s *q;
	u_char *raw = NULL;
	int copied;
	u_int pkthdr_len;
	u_int pktdata_len;
	u_int rawl;
	int err = 0;
	ushort elen = 0;
	ushort del = 0;
	u_char *skb_raw = NULL;

	q = esp_pull_queue(sk, &af->r_overflow);
	if (q) {

		raw = xchg(&q->data_raw, NULL);
		rawl = xchg(&q->data_len, 0);
		kfree_queue(&q);

		NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: esp_recvmsg: read %d bytes from receive overflow queue.\n", ESP_SKADDR(sk), (int)rawl)));

	} else {

restart:
		while (skb_queue_empty(&sk->receive_queue)) {

			err = sock_error(sk);
			if (err)
				goto done;

			if (sk->shutdown & RCV_SHUTDOWN)
				sk->done = 1;
			if (sk->done)
				goto done;

			if (!inet_connected(sk)) {
				err = -ENOTCONN;
				goto done;
			}

			if (signal_pending(tsk)) {
				err = -ERESTARTSYS;
				goto done;
			}

			if (noblock) {
				err = -EAGAIN;
				goto done;
			}

			add_wait_queue(sk->sleep, &wait);
			current->state = TASK_INTERRUPTIBLE;
			if (skb_peek(&sk->receive_queue) == NULL)
				schedule();
			current->state = TASK_RUNNING;
			remove_wait_queue (sk->sleep, &wait);
		}

		skb = skb_dequeue(&sk->receive_queue);
		if (!skb) /* someone beat us to the packet */
			goto restart;
		sk->stamp = skb->stamp;

		if (sk->protocol == IPPROTO_ESP) {
			del = ntohs(skb->h.eh->del);
			elen = ntohs(skb->h.eh->len);
			skb_raw = (u_char *)(skb->h.eh+1);
			pkthdr_len = sizeof(struct esphdr);
#ifdef CONFIG_ES_UDP_SUPPORT
		} else if (sk->protocol == IPPROTO_ESPUDP) {
			elen = ntohs(skb->h.uh->len);
			skb_raw = (u_char *)(skb->h.uh+1);
			pkthdr_len = sizeof(struct udphdr);
#endif
		} else {
			/* error */
			goto done;
		}
		pktdata_len = skb->len - pkthdr_len;
		if (EOPT_ISSET(sk, EOPT_FAST_COMPRESS)) {
			err = esp_decode(sk, skb_raw, pktdata_len, del, &raw, &rawl);
			if (err) {
				NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "NET4: ESP: esp_recvmsg: esp_decode error %d\n", err)));
				goto done;
			}
		} else { /* NO COMPRESS */
			rawl = elen - pkthdr_len;
			raw = (u_char *)kmalloc(rawl, GFP_ATOMIC);
			if (!raw) {
				err = -ENOMEM;
				goto done;
			}
			memcpy(raw, skb_raw, rawl);
		}

	}

		/* "len" is the size of the user's receive buffer.
		 * "rawl" is how many bytes we have prepared for them.
		 */
	if (rawl > len) {
		err = esp_recvmsg_overflow(sk, raw, rawl, len);
		if (err)
			goto done;
	}

	copied = min(rawl, len);
	err = memcpy_toiovec(msg->msg_iov, raw, copied);
	if (err)
		return (err);
	kfree(xchg(&raw, NULL));

	if (msg->msg_name) {
#if defined(CONFIG_IPV6) || defined (CONFIG_IPV6_MODULE)
		if (sk->family == PF_INET6) {
			/* IPv6 */
			struct ipv6_pinfo * np = &sk->net_pinfo.af_inet6;
			struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)msg->msg_name;

			sin6->sin6_family = AF_INET6;
			memcpy(&sin6->sin6_addr, &np->daddr, sizeof(struct in6_addr));
			sin6->sin6_port = sk->dport;
			sin6->sin6_flowinfo = 0;
		} else 
#endif
		{
			/* IPv4 */
			sin = (struct sockaddr_in *)msg->msg_name;
			sin->sin_family = AF_INET;
			sin->sin_addr.s_addr = sk->daddr;
			sin->sin_port = sk->dport;
		}
	}

	if (skb && sk->protinfo.af_inet.cmsg_flags)
    ip_cmsg_recv(msg, skb);
  err = copied;

done:
	if (skb)
		skb_free_datagram(sk, skb);
	if (raw)
		kfree(raw);
	return (err);
}

struct esp_remote_host *esp_remote_host_table = NULL;
int esp_attach_remote_host(struct sock *sk)
{
	struct esp_sock *af = es_sk(sk);
	struct esp_remote_host *rh;

	for (rh=esp_remote_host_table; rh; rh=rh->next)
		if (rh->daddr.sin4.s_addr == sk->daddr)
			break;

	if (!rh) {
		rh = (struct esp_remote_host *)kmalloc(sizeof(struct esp_remote_host), GFP_ATOMIC);
		if (!rh)
			return (-ENOMEM);
		memset(rh, 0, sizeof(struct esp_remote_host));
		rh->daddr.sin4.s_addr = sk->daddr;
		rh->msl = IPV4_MIN_MTU - sizeof(struct esphdr) - sizeof(struct iphdr);
		rh->msw = rh->msl * 1024;
		rh->st = rh->msl;
		rh->next = esp_remote_host_table;
		esp_remote_host_table = rh;
		if (rh->next)
			rh->next->prev = rh;
	}

	rh->users++;
	af->rem_host = rh;
	af->msl = rh->msl;
	af->st = rh->st;
	return (0);
}

void esp_detach_remote_host(struct sock *sk)
{
	struct esp_sock *af = es_sk(sk);

	if (!af->rem_host)
		return;

	af->rem_host->users--;
	if (!af->rem_host->users) {
		if (!af->rem_host->prev)
			esp_remote_host_table = af->rem_host->next;
		else
			af->rem_host->prev->next = af->rem_host->next;
		kfree(af->rem_host);
	}
	af->rem_host = NULL;

}

size_t esp_packet_size(struct sock *sk, struct esp_queue_s *q)
{
	struct esp_sock *af = es_sk(sk);
	size_t pktdata_len;
	int i, s;

	if (!q)
		return (0);

	if (q->pm == ESP_DATA && q->segflg) {
		s = q->data_len / af->dictseg_len;
		/* extended data segment */
		pktdata_len = q->data_len - s*af->dictseg_len;
		/* encode/decode data */
		for (i=0; i<s; i++) {
			if (q->segflg[i] & ESF_SEGMENT)
				pktdata_len += af->dictseg_len;
			if (q->segflg[i] & ESF_MAP)
				pktdata_len += sizeof(__u16);
		}
	} else {
		pktdata_len = q->data_len;
	}

	return (pktdata_len);
}

/* Queue a socket buffer. */
int esp_rcv_skb(struct sock * sk, struct sk_buff *skb)
{
	int err;

	err = sock_queue_rcv_skb(sk, skb);
	if (err < 0) {
		ESP_MIB(sk).EspInErrors++;
#if LINUX_VERSION_CODE <= 0x020301  /* less than or equal to 2.3.x */
		ip_statistics.IpInDiscards++;
		ip_statistics.IpInDelivers--;
#endif
		kfree_skb(skb);
		switch (-err) {
			case ENOMEM:
				esp_error(sk, skb, E_QUEUE, ESE_INSUFMEMORY);
				break;
		}
		ESPDEBUG(NETDEBUG(printk(KERN_DEBUG "NET4: ESP: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: esp_rcv_skb: sock_queue_rcv_skb error %d.\n", ESP_SKADDR(sk), err)));
		return (err);
	}

	return (0);
}

/* Dispences a 'test' packet of a pre-calculated size. */ 
int esp_probe_sendlen(struct sock *sk)
{
	struct esp_opt *af = (struct esp_opt *)&sk->tp_pinfo.af_esp;
	struct dst_entry *dst = sk->dst_cache;
	size_t mtu, maxlen, hh_len;
	int err;

	if (!dst) {
		ESPDEBUG(NETDEBUG(printk(KERN_DEBUG "NET4: ESP: esp_probe_sendlen: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: unable to probe send length, no sk->dst_cache.\n", ESP_SKADDR(sk))));
		return (-EINVAL);
	}

	hh_len = (sk->dst_cache->dev->hard_header_len + 15) & ~15;
	mtu = max(0, dst->pmtu - sizeof(struct esphdr) - sizeof(struct iphdr) - hh_len - 15);
	maxlen = min(ESP_MAX_DATA_SIZE, min(mtu, af->msl+af->st));
	if (maxlen) {
		err = espsend(sk, ESP_PROBE, 0, &maxlen, NULL);
		if (err)
			return (err);
	}

	return (0);
}

void esp_rcv_handshake_opt(struct sock *sk, int *esp_opt, int rem_opt)
{
	struct esp_sock *af = es_sk(sk);
	int loc_opt = af->opt;
	int rem_ver;
	int loc_ver;
	int min_ver;
	int i;

	/* Locate lowest remote version of option. */
	rem_ver = 0;
	for(i=0; esp_opt[i]>=0; i++)
		if (rem_opt & esp_opt[i]) {
			rem_ver = i;
			break;
		}

	/* Locate lowest local version of option. */
	loc_ver = 0;
	for(i=0; esp_opt[i]>=0; i++)
		if (loc_opt & esp_opt[i]) {
			loc_ver = i;
			break;
		}

	/* Unset all versions of option. */
	for(i=0; esp_opt[i]>=0; i++)
		if (EOPT_ISSET(sk, esp_opt[i]))
			EOPT_UNSET(sk, esp_opt[i]);

	min_ver = min(loc_ver, rem_ver);
	if (min_ver > 0) {
		/* Set lowest option version. */
		EOPT_SET(sk, esp_opt[min_ver]);
	}

}

int esp_rcv_handshake(struct sock *sk, struct sk_buff *skb)
{
	struct esp_sock *af = es_sk(sk);
	u_short pktlen = ntohs(skb->h.eh->len) - sizeof(struct esphdr);
	int ver = (int)ntohs((u_short)skb->h.eh->ver);
	struct esp_handshake_p hnd;
	int err;

NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET4: ESP: DEBUG: esp_rcv_handshake()\n")));

#ifdef CONFIG_ES_UDP_SUPPORT
	if (sk->protocol == IPPROTO_ESPUDP)
		return (0); /* no handshake mechanism for ESP-UDP. */
#endif

	if (sk->sk_state == ESP_LISTEN || sk->sk_state == ESP_CLOSE) {
		esp_error(sk, skb, E_STATE, ESE_INVALSTATEHS);
		return (-EINVAL);
	}

	/* if (esp_valid_size) */
	if ((ver <= ESP_VERSION && pktlen != esp_handshake_version_size[ver]) ||
			(ver >  ESP_VERSION && pktlen >  ESP_MAX_HANDSHAKE_SIZE)) {
		esp_error(sk, skb, E_LENGTH, ESE_INVALSIZEHS);
		return (-EINVAL);
	}

	memset(&hnd, 0, sizeof(struct esp_handshake_p));
	memcpy(&hnd, skb->data+sizeof(struct esphdr),
			min(sizeof(struct esp_handshake_p), pktlen));

	hnd.ip4_tos            = (__u32)ntohl((u_long)hnd.ip4_tos);
	hnd.esp_opt            = (__u32)ntohl((u_long)hnd.esp_opt);
	hnd.dictseg_len        = (__u32)ntohl((u_long)hnd.dictseg_len);
	hnd.locked_msw         = (__u32)ntohl((u_long)hnd.locked_msw);
	hnd.ttl                = (__u32)ntohl((u_long)hnd.ttl);

	/* dictionary segment length */
	af->dictseg_len        = max(ESP_MIN_DICTSEG_LENGTH, min(hnd.dictseg_len, af->dictseg_len));

	/* ipv4 time-to-live per packet */
	if (hnd.ttl >= 8 && hnd.ttl < sk->protinfo.af_inet.ttl)
		sk->protinfo.af_inet.ttl = hnd.ttl;

	/* ipv4 type-of-service for stream */
	sk->protinfo.af_inet.tos |= hnd.ip4_tos;

#ifdef CONFIG_ESP_SENDWINDOW
	if (hnd.locked_msw && hnd.locked_msw < ESP_MAX_DATA_WINDOW) {
		if (sk->sk_state == ESP_ESTABLISHED)
			af->locked_msw     = hnd.locked_msw;  
		else
			af->locked_msw     = min(af->locked_msw, hnd.locked_msw);
	}
#endif

	/* esp stream ID number. */
	if (sk->sk_state != ESP_ESTABLISHED) {
		af->r_id             = hnd.stream_id;
	}

	/* Synchronize stream's compression mode. */
	esp_rcv_handshake_opt(sk, (int *)esp_compress_opt, hnd.esp_opt);

	/* Synchronize stream's handshake method. */
	esp_rcv_handshake_opt(sk, (int *)esp_checksum_opt, hnd.esp_opt);

	if (EOPT_ISSET(sk, EOPT_FAST_COMPRESS)) {
		err = esp_dict_init_segs(sk);
		if (err)
			return (err);
	}

	NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "NET4: ESP: esp_rcv_handshake: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: state=%s(%d) dictseg_len=%d tos=%s(%d) opt=%d\n", ESP_SKADDR(sk), ESP_STATE_STRING(sk->sk_state), sk->sk_state, (int)af->dictseg_len, IPTOS_STRING(sk->protinfo.af_inet.tos), (int)sk->protinfo.af_inet.tos, (int)af->opt)));

	if (sk->sk_state == ESP_CONN_ACK) {
		esp_set_state(sk, ESP_ESTABLISHED);
	} else if (sk->sk_state == ESP_CONN_WAIT) {
		err = espsend(sk, ESP_HANDSHAKE, 0, NULL, skb);
		if (err)
			return (err);
	}

	return (0);
}

#if 0 
int esp_version_size(int pm, int ver)
{
	int ver_size;

	if (ver > ESP_VERSION)
		return (-EINVAL);

	switch (pm) {
		case ESP_REQUEST:
			ver_size = esp_request_version_size[ver];
			break;
		case ESP_ERROR:
			ver_size = esp_error_version_size[ver];
			break;
		case ESP_HANDSHAKE:
			ver_size = esp_handshake_version_size[ver];
			break;
		case ESP_PROBE:
		case ESP_PROBE_RETURN:
			ver_size = esp_probe_version_size[ver];
			break;
		default:
			return (-EINVAL);
	}

	return (ver_size);
}
#endif

/* Process a incoming error packet. */
int esp_rcv_error(struct sock *sk, struct sk_buff *skb)
{
	struct esp_sock *af = es_sk(sk);
	u_short pktlen = ntohs(skb->h.eh->len) - sizeof(struct esphdr);
	struct esp_error_p error;
	int err;

	err = esp_valid_size(pktlen, skb->h.eh->ver, //ntohs((u_short)skb->h.eh->ver),
			esp_error_version_size, ESP_MAX_ERROR_SIZE);
	if (err) {
		esp_error(sk, skb, E_LENGTH, ESE_INVALSIZEERR);
		return (err);
	}

	memset(&error, 0, sizeof(struct esp_error_p));
	memcpy(&error, skb->data+sizeof(struct esphdr), 
			min(sizeof(struct esp_error_p), pktlen));

	error.err_type  = (__u32)ntohl((unsigned long int)error.err_type);
	error.err_code  = (__u32)ntohl((unsigned long int)error.err_code);
	error.state     = (__u32)ntohl((unsigned long int)error.state);

	NETDEBUG(printk(KERN_DEBUG "NET4: ESP: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: esp stream error (ESE #%d) [class %d] received: state=%s(%d).\n", ESP_SKADDR(sk), error.err_code, error.err_type, ESP_STATE_STRING(error.state), error.state));

	if (error.err_type >= ESP_MAX_ERROR_TYPES) {
		esp_error(sk, skb, E_PARAM, ESE_INVALERRTYPE);
		return (-EINVAL);
	}
	af->rem_error = error.err_code;

	switch (error.err_type) {
		case E_CHECKSUM:
			switch (error.err_code) {
				case ESE_INVALCRC:
					NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: The remote endpoint received a packet with an invalid checksum.\n", ESP_SKADDR(sk))));
					ESP_DEC_MSL(sk);
					break;
			}
			break;
		case E_LENGTH:
			{
#if 0
				static int soft_error;
				soft_error++;
				if (soft_error == ESP_MAX_SOFT_ERRORS) {
					sk->done = 1;
					sk->state_change(sk);
					break;
				}
#endif
				switch (error.err_code) {
					case ESE_INVALDEL:
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: Data packet received with an invalid dictionary entry length.\n")));
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: Possible Causes:\n")));
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: - Network degradation.\n")));
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: - Packet was encoded in unknown format.\n")));

						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: Corrective Measures:\n")));
						ESP_DEC_MSL(sk);
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: - The stream's maximum send length (mtu) has been lowered to %d.\n", (int)af->msl)));
						break;
					case ESE_INVALSIZEDATA: /* Received a data packet with invalid size parameter */
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: Packet received with an invalid size at remote endpoint. Possible Causes: \n")));
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: - Network degradation.\n")));
						break;
					case ESE_TRUNCDATA: /* Received truncated data packet */
						NETDEBUG(printk(KERN_DEBUG "INET: ESP: Remote endpoint received truncated data packet, Possible Causes: \n"));
						NETDEBUG(printk(KERN_DEBUG "INET: ESP: - The endpoint's receive buffer overflowed.\n"));
						NETDEBUG(printk(KERN_DEBUG "INET: ESP: - The endpoint received data with a reported size larger then the packet's data area.\n"));
						ESP_DEC_MSL(sk);
						break;
					case ESE_INVALSIZEHS:
					case ESE_INVALSIZEREQ:
					case ESE_INVALSIZEERR:
					case ESE_INVALSIZEPROBE:
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: Stream endpoint received a invalid sized packet [mode: %d], Possible Causes:\n", ESP_SKADDR(sk), (int)skb->h.eh->pm)));
						//(int)ntohs(skb->h.eh->pm))));
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: - Network degregation.\n")));
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: - The local or remote endpoint has an incorrect packet size per version table.\n")));
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: Corrective Measures:\n")));
						ESP_DEC_MSL(sk);
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: - The stream's maximum send length has been lowered: %d bytes.\n", (int)af->msl)));
						break;
				}
			}
			break;
		case E_QUEUE:
			{ 
#if 0
				static int soft_error;
				soft_error++;
				if (soft_error == ESP_MAX_SOFT_ERRORS) {
					sk->done = 1;
					sk->state_change(sk);
					break;
				}
#endif
				switch (error.err_code) {
					case ESE_RETRANTIMEOUT:
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: Packet retransmit timeout. Possible Causes:\n")));
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: - A packet was purged from the backlog queue before the remote endpoint received it.\n"))); 
						sk->done = 1;
						sk->state_change(sk);
						break;
					case ESE_INVALSEQ:
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: Packet received with an invalid sequence number. Possible Causes:\n")));
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: - Network degradation.\n")));
						if (sk->sk_state != ESP_ESTABLISHED) {
							sk->done = 1;
							sk->state_change(sk);
						}
						break;
				}
			}
			break;
		case E_STATE:
			{
#if 0
				static int soft_error;
				soft_error++;
				if (soft_error == ESP_MAX_SOFT_ERRORS) {
					sk->done = 1;
					sk->state_change(sk);
					break;
				}
#endif
				switch (error.err_code) {
					case ESE_INVALSTATEREQ:
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: An invalid socket received a ESP request packet. Possible Causes:\n")));
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: - A non-bound remote endpoint received a create socket request.\n"))); 	
						break;
					case ESE_INVALSTATEHS:
						if (sk->sk_state != ESP_ESTABLISHED) {
							NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: An invalid socket received a handshake packet at remote endpoint. Local endpoint was not connected. Possible Causes:\n")));
							NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: - A socket closed before initiating the handshake sequence.\n")));
							NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: - An error occurred creating a socket at remote endpoint.\n")));
							sk->done = 1;
							sk->state_change(sk);
						}
						break;
					case ESE_INVALSTATEDATA: /* Data received on stream in an invalid state */
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: Received data on a unestablished socket. Possible Causes:\n")));
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: - The stream's handshake mechanism was unsuccessfull.\n")));
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: - The remote endpoint is in the process of closing it's stream connection.\n")));
						sk->done = 1;
						sk->state_change(sk);
						break;
				}
				break;
			case E_PARAM: /* ESP Packet Parameter Error
										 * 
										 * Note: No reponding packet will ever be sent.
										 */
				switch (error.err_code) {
					case ESE_INVALERRTYPE: /* Error packet included invalid parameters */
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: Error packet included invalid parameters. Possible Causes:\n")));
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: - The remote endpoint received an error packet with an invalid error type.\n")));
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: - The remote endpoint received an error packet that was not a real error.\n")));
						break;
					case ESE_INVALPCKMODE: /* Unable to process invalid packet mode */
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: An invalid type of packet was received. Possible Causes:\n")));
						NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: - The remote endpoint received an packet with an invalid packet mode.\n")));
						break;
				}
			}
			break;
	}

	return (0);
}

/* queues valid data packets on a socket. */
int esp_rcv_data(struct sock *sk, struct sk_buff **skb_ptr)
{
	struct sk_buff *skb = *skb_ptr;
	ushort len = 0;
	ushort del;
	int err;

	if (sk->protocol == IPPROTO_ESP) {
		len = ntohs(skb->h.eh->len); /* offset (iphdr) */
		del = ntohs(skb->h.eh->del); /* offset (iphdr) + (esphdr) */
		
		if (sk->sk_state != ESP_ESTABLISHED) {
			err = esp_error(sk, skb, E_STATE, ESE_INVALSTATEDATA);
			if (err)
				return (err);
			return (0); /* error processed successfully. */
		}

		if (del + sizeof(struct esphdr) > len) {
			NETDEBUG(ESPDEBUG(printk (KERN_DEBUG "NET4: ESP: esp_rcv_data: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: invalid packet, discarding. (del(%d) > len(%d))\n", ESP_SKADDR(sk), del, len)));
			esp_error(sk, skb, E_LENGTH, ESE_INVALDEL);
			return (-EBADMSG);
		}

#ifdef CONFIG_ES_UDP_SUPPORT
	} else if (sk->protocol == IPPROTO_ESPUDP) {
		len = ntohs(skb->h.uh->len);
#endif
	}

	if (!len)
		return (0);

	skb_trim(skb, len);
	err = esp_rcv_skb(sk, xchg(skb_ptr, NULL));
	if (err)
		return (err);

	return (0);
}

int esp_rcv_request(struct sock *sk, struct sk_buff *skb)
{
	struct esp_sock *af = es_sk(sk);
	u_short pktlen = ntohs(skb->h.eh->len) - sizeof(struct esphdr);
	struct esp_request_p req;
	u_short seq;
	u_short pm;
	u_long size;
	int type;
	int err;

	err = esp_valid_size(pktlen, skb->h.eh->ver, //ntohs((u_short)skb->h.eh->ver),
			esp_request_version_size, ESP_MAX_REQUEST_SIZE);
	if (err) {
		esp_error(sk, skb, E_LENGTH, ESE_INVALSIZEREQ);
		return (err);
	}

	memset(&req, 0, sizeof(struct esp_request_p));
	memcpy(&req, skb->data+sizeof(struct esphdr),
			min(sizeof(struct esp_request_p), pktlen));

	type    = (int)ntohl((u_long)req.req_type);

	/* Maximum allowed ESP request types. */
	if (type >= ESP_MAX_REQUEST_TYPES) {
		return esp_error(sk, skb, E_PARAM, ESE_INVALREQTYPE); 
	}

	/* Local version support of ESP request types. */
	if (type >= ESP_REQUEST_TYPES) {
		/* action not supported. */
		return (0);
	}

	if (sk->sk_state == ESP_LISTEN) {
		if (type == EREQ_SOCKET_CREATE) {
			err = esp_create_accept_socket(sk, skb);
			if (err)
				return (err);
		} else { 
			err = esp_error(sk, skb, E_STATE, ESE_INVALSTATEREQ);
			if (err)
				return (err);
		}
		return (0);
	}

	/* check crc? */

	seq     = (__u16)ntohs((u_short)req.eh.seq);
	pm      = (__u16)req.eh.pm; //ntohs((u_short)req.eh.pm);
	size    = (__u32)ntohl((u_long)req.req_opt1);

	switch (type) {
		case EREQ_SOCKET_OPEN:
			if (sk->sk_state != ESP_CONN_WAIT) {
				err = esp_error(sk, skb, E_STATE, ESE_INVALSTATEREQ);
				if (err)
					return (err);
				return (0); /* error processed successfully */
			}
			err = esp_set_state(sk, ESP_ESTABLISHED);
			if (err)
				return (err);
			break;
		case EREQ_SOCKET_CLOSE:
			if (!sk->done) {
				sk->done = 1;
				sk->state_change(sk);
			}
			break;
		case EREQ_SOCKET_BPS:
			if (sk->sk_state == ESP_ESTABLISHED) {
				err = esp_error(sk, skb, E_STATE, ESE_INVALSTATEREQ);
				if (err)
					return (err);
				return (0);
			}
			if (size < IPV4_MIN_MTU || size >= ESP_MAX_DATA_WINDOW) {
				err = esp_error(sk, skb, E_PARAM, ESE_INVALREQSIZE);
				if (err)
					return (err);
				return (0);
			}
			af->locked_msw = size;
			break;
		case EREQ_PACKET:
			if (sk->sk_state != ESP_ESTABLISHED) {
				err = esp_error(sk, skb, E_STATE, ESE_INVALSTATEREQ);
				if (err)
					return (err);
				return (0);
			}
			err = esp_retransmit(sk, skb, 
					req.eh.pm, req.eh.seq, req.eh.asl);
			if (err)
				return (err);
			break;
	}

	return (0);
}

int esp_rcv_probe(struct sock *sk, struct sk_buff *skb, size_t len)
{
	struct esp_sock *af = es_sk(sk);
	struct esp_remote_host *rh = af->rem_host;
	ushort pktlen = ntohs(skb->h.eh->len) - sizeof(struct esphdr);
	int pm = skb->h.eh->pm; //ntohs((u_short)skb->h.eh->pm);
	struct esp_probe_p p;
	int err;

	if (!rh)
		return (-EPROTO);

	err = esp_valid_size(pktlen, skb->h.eh->ver, //ntohs((u_short)skb->h.eh->ver), 
			esp_probe_version_size, ESP_MAX_PROBE_SIZE);
	if (err) {
		esp_error(sk, skb, E_LENGTH, ESE_INVALSIZEPROBE);
		return (err);
	}

	if (pm == ESP_PROBE) {
		u_long sendval = len - sizeof(struct esphdr);
    err = espsend(sk, ESP_PROBE_RETURN, 0, &sendval, skb);
    if (err)
      return (err);
    return (0);
  }

  memset(&p, 0, sizeof(struct esp_probe_p));
  memcpy(&p, skb->data+sizeof(struct esphdr),
      min(sizeof(struct esp_probe_p), pktlen));
  p.length            =  (__u32)ntohl((u_long)p.length);
  p.rcv_length        =  (__u32)ntohl((u_long)p.rcv_length);

  if (p.rcv_length > rh->msl) {
    rh->dsl = max(rh->dsl, p.rcv_length - rh->msl);
    if (rh->dsl >= rh->st) {
      rh->dsl = rh->dsl - rh->st;
      rh->msl = rh->msl + rh->st;
    } 
		/* increase send threshold by 1.5x */
		rh->st = min(rh->msl, rh->st + rh->st/2);
		if (rh->dsl < rh->st) {
			af->probe.tinterval = min(40, af->probe.tinterval + 2);
		}
  } else if (p.rcv_length < p.length) {
		NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "NET4: ESP: esp_rcv: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: stream send-length probe incomplete. (%d of %d bytes)\n", ESP_SKADDR(sk), p.rcv_length, p.length)));
		rh->psl = max(rh->psl, p.length - p.rcv_length);
		if (rh->psl >= rh->st) {
			rh->psl = rh->psl - rh->st;
			rh->msl = rh->msl - rh->st;
		}
		/* decrease send threshold by 1.5x */
		rh->st = max(rh->msl/8, rh->st - rh->st/2);
		if (rh->psl < rh->st) {
			af->probe.tinterval = max(1, af->probe.tinterval - 2);
		}
	}

	rh->msw = (rh->msw + rh->msl*1024) / 2;
	af->msl = (af->msl + rh->msl)      / 2;
	af->st  = (af->st  + af->msl/4)    / 2;

	/* successfull processing of a probe decrements the "packet loss rate". */
	if (af->pck_loss_rate) {
		af->pck_loss_rate--;
	}

	return (0);
}

/* Process a incoming IPv4/IPv6 packet. */
int esp_listen_rcv(struct sock *sk, struct sk_buff **skb_ptr, int pm)
{
	struct sk_buff *skb = *skb_ptr;
	int err;

	switch (pm) {
		case ESP_REQUEST:
			err = esp_rcv_request(sk, skb);
			if (err)
				return (err);
			break;
	}

	return (0);
}

void esp_trim_overflow_queue(struct sock *sk, int pm)
{
	struct esp_sock *af = es_sk(sk);
	struct esp_queue_s *q;
	int i;

	for (i = 0; i < ESP_PACKET_BLOCK_SIZE; i++) {
		q = esp_pull_queue(sk, &af->s_backlog[pm]);
		if (!q)
			break;
		kfree_queue(&q);
	}
}

/* Process a incoming IPv4/IPv6 packet. */
int esp_rcv(struct sock *sk, struct sk_buff **skb_ptr, size_t len, int pm)
{
	struct esp_sock *af = es_sk(sk);
	struct sk_buff *skb = *skb_ptr;
	int err;

	/* specified packet mode is not correct. */
	if (pm >= ESP_MAX_PACKET_MODES) {
		err = ESE_INVALPCKMODE;
		esp_error(sk, skb, E_PARAM, err);
		return (err);
	} 

	/* specified packet mode is not supported. */
	if (pm >= ESP_PACKET_MODES) {
		/* action not supported. */
		return (0);
	}

	err = esp_valid_seq(sk, skb, len); /* increments stream seq numbers. */
	if (err) {
		if (skb->h.eh->opt & EOPT_RETRAN) {
			/* junk out-of-order retransmitted packet. */
			return (0);
		}
		/* ** BUG: should have an out-of-order bitbucket here. ** */
		return (err);
	}

	/* increment non-ack'd packet receive count. */
	af->r_noack_pck[pm]++;

	if (skb->h.eh->opt & EOPT_ACK_BLOCK) {
		/* Flush backlogged packets that have already been received. */
		esp_trim_overflow_queue(sk, skb->h.eh->pm); 
	}

	ESP_MIB(sk).EspInPackets++;

	/* Process packet mode data. */
	switch (pm) {
		case ESP_DATA:
			ESP_MIB(sk).EspInDataPackets++;
			err = esp_rcv_data(sk, skb_ptr);
			if (err) 
				return (err);
			break;
		case ESP_HANDSHAKE: 
			err = esp_rcv_handshake(sk, skb);
			if (err)
				return (err);
			break;
		case ESP_REQUEST:
			err = esp_rcv_request(sk, skb);
			if (err)
				return (err);
			break;
		case ESP_ERROR:
			err = esp_rcv_error(sk, skb);
			if (err)
				return (err);
			break;
		case ESP_PROBE_RETURN:
		case ESP_PROBE:
			err = esp_rcv_probe(sk, skb, (u_int)len);
			if (err)
				return (err);
			break;
	}

	return (0);
}

/* Pass data from the user-level to the transmission queue. */
int esp_sendmsg(struct sock *sk, struct msghdr *msg, int len)
{
	struct esp_opt *af = (struct esp_opt *)&sk->tp_pinfo.af_esp;
	esp_queue_s *q = NULL;
	u_char *raw;
	int err;

	if (len < 0 || len > ESP_MAX_DATA_SIZE) {
		err = -EMSGSIZE;
		goto error;
	}

	/* check bsd flags */
	if (msg->msg_flags & MSG_OOB) {
		err = -EOPNOTSUPP;
		goto error;
	}

	if (msg->msg_flags&~(MSG_DONTROUTE|MSG_DONTWAIT|MSG_NOSIGNAL))
		return -EINVAL;

	if (sk->sk_state != ESP_ESTABLISHED)
		return (-ENOTCONN);

	/*
	 *  Get and verify the address.
	 */
	if (msg->msg_name) {
#if defined(CONFIG_IPV6) || defined (CONFIG_IPV6_MODULE)
		if (sk->family == PF_INET6) {
			struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)msg->msg_name;
			struct ipv6_pinfo *np = &sk->net_pinfo.af_inet6;

			if (msg->msg_namelen < sizeof(*sin6))
				return(-EINVAL);

			if (sin6->sin6_family && sin6->sin6_family != AF_INET6)
				return (-EINVAL);

			if (ipv6_addr_cmp(&sin6->sin6_addr, &np->daddr) != 0)
				return (-EINVAL);

			if (np->sndflow && np->flow_label !=
					(sin6->sin6_flowinfo & IPV6_FLOWINFO_MASK))
				return (-EINVAL);
		} else 
#endif
		{
			struct sockaddr_in *sin = (struct sockaddr_in*)msg->msg_name;

			if (msg->msg_namelen < sizeof(*sin))
				return (-EINVAL);
			if (sin->sin_family && sin->sin_family != AF_INET)
				return (-EINVAL);
			if (sin->sin_addr.s_addr != sk->daddr)
				return (-EINVAL);
			if (sin->sin_port != sk->dport)
				return (-EINVAL);
		}
	}

#if defined(CONFIG_IPV6) || defined (CONFIG_IPV6_MODULE)
	if (sk->family == PF_INET6) {
		struct ipv6_pinfo *np = &sk->net_pinfo.af_inet6;
		if (ipv6_addr_type(&np->daddr) == IPV6_ADDR_MAPPED) {
			struct sockaddr_in sin;

			sin.sin_family = AF_INET;
			sin.sin_addr.s_addr = np->daddr.s6_addr32[3];
			sin.sin_port = sk->dport;
			msg->msg_name = (struct sockaddr *)(&sin);
			msg->msg_namelen = sizeof(sin);

			return (esp_sendmsg(sk, msg, len));
		}
	}
#endif

	raw = (u_char *)kmalloc(len, GFP_ATOMIC);
	memcpy_fromiovecend(raw, msg->msg_iov, 0, len);
	err = kmalloc_queue(raw, len, &q);
	if (err)
		goto error;
	q->pm = ESP_DATA;
	q->msg_flags = msg->msg_flags;
	err = esp_encode_queue(sk, q);
	if (err)
		goto error;
	esp_push_queue(&af->s_queue, q);
	af->sl = min(af->msl, esp_packet_size(sk, q));
	return (len);

error:
	kfree_queue(&q);
	return (err);
}

/* Return the next outstanding connection or NULL. */
struct sock *esp_accept(struct sock *sk, int flags)
{
	struct task_struct *tsk = current;
	DECLARE_WAITQUEUE(wait, tsk);
	struct sock *newsk;
	int err;

	NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: esp_accept: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: esp_accept: accepting sock using flags %d.\n", ESP_SKADDR(sk), flags))); 

	if (sk->sk_state != ESP_LISTEN) {
		NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: esp_accept: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: esp_accept: sock is in state %s, not ESP_LISTEN", ESP_SKADDR(sk), ESP_STATE_STRING(sk->sk_state)))); 
		err = -EINVAL;
		goto error;
	}

	while (!(newsk = pull_accept_sock(sk))) {

		if (signal_pending(tsk)) {
			err = -ERESTARTSYS;
			goto error;
		}

		err = sock_error(sk);
		if (err)
			goto error;

		if (flags & MSG_DONTWAIT) {
			err = -EAGAIN;
			goto error;
		}

		add_wait_queue(sk->sleep, &wait);
		current->state = TASK_INTERRUPTIBLE;
		schedule();
		current->state = TASK_RUNNING;
		remove_wait_queue(sk->sleep, &wait);
	}

	return (newsk);

error:
	sk->err = -err;
	return (NULL);
}

inline void esp_clear_timers(struct sock *sk)
{
	if (timer_pending(&sk->tp_pinfo.af_esp.timer)) {
		del_timer(&sk->tp_pinfo.af_esp.timer);
	}

	if (timer_pending(&sk->tp_pinfo.af_esp.queue_timer)) {
		del_timer(&sk->tp_pinfo.af_esp.queue_timer);
	}
}

int esp_destroy_sock(struct sock *sk)
{
	struct esp_sock *af = es_sk(sk);
	struct esp_queue_s *que;
	struct sock *accept_sk;
	struct sk_buff *skb;
	int i;

	NETDEBUG(ESPDEBUG(printk (KERN_DEBUG "INET: ESP: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: esp_destroy_sock: destroying sock.\n", ESP_SKADDR(sk))));

	esp_clear_timers(sk);
	esp_detach_remote_host(sk);
	esp_dict_free_segs(sk);

	while ((skb = __skb_dequeue(&sk->write_queue)) != NULL)
		kfree_skb(skb);

	while ((skb = __skb_dequeue(&sk->receive_queue)) != NULL)
		kfree_skb(skb);

	while ((que = esp_pull_queue(sk, &af->r_overflow)) != NULL)
		kfree_queue(&que);

	while ((que = esp_pull_queue(sk, &af->s_queue)) != NULL)
		kfree_queue(&que);

	for (i = 0; i < ESP_PACKET_MODES; i++) {
		while ((que = esp_pull_queue(sk, &af->s_backlog[i])) != NULL)
			kfree_queue(&que);
	}

	if (sk->max_ack_backlog != 0) {
		/* listen sock. */
		if (af->listen.queue) {
			/* remove un-accepted nodes from listen list. */
			while ((accept_sk = pull_accept_sock(sk))) {
				esp_set_state(accept_sk, ESP_CLOSE);
				accept_sk->prot->close(accept_sk, 0);
			}
			kfree(af->listen.queue);
			af->listen.queue = NULL;
			sk->max_ack_backlog = 0;
			sk->ack_backlog = 0;
		}
	} else if (af->listen.sock) {
		/* may be a listen or normal sock. */
		remove_accept_sock(af->listen.sock, sk);
		af->listen.sock = NULL;
	}

	return (0);
}

/* Compute a checksum from the data and length given. */ 
inline u_short esp_check(struct sock *sk, __u32 seed, u_char *data, size_t len)
{
	int err;

	err = verify_area(VERIFY_READ, data, len);
	if (!err) {
		/* ESP Fast Checksum (Level 1 Checksum) */
		if (EOPT_ISSET(sk, EOPT_FAST_CHECKSUM)) { 
			return (esp_fast_csum(seed, data, len));
		}
	} 

	return (ESP_DEF_CHECK);
}

void init_espsend_handshake(struct sock *sk, esp_handshake_p *h)
{
	struct esp_sock *af = es_sk(sk);

	/* Stream ID initialization. */
	if (sk->sk_state != ESP_ESTABLISHED) {
		af->s_id      = esp_sock_id(sk->num);
	} 

	h->stream_id    = af->s_id;
	h->ip4_tos      = (__u32)htonl((u_long)IPTOS_TOS(sk->protinfo.af_inet.tos));
	h->esp_opt      = (__u32)htonl((u_long)af->opt);
	h->dictseg_len  = (__u32)htonl((u_long)af->dictseg_len);
	h->locked_msw   = (__u32)htonl((u_long)af->locked_msw);
	h->ttl          = (__u32)htonl((u_long)sk->protinfo.af_inet.ttl);

}

/*
 * Send a non-data packet. 
 *
 * note: arguments skb and optdata are optional.
 */
int espsend(struct sock *sk, int pm, int level, void *optdata, struct sk_buff *skb)
{
	struct esp_sock *af = es_sk(sk);
	struct esp_queue_s *q = NULL;
	u_long optval = optdata ? *((u_long *)optdata) : 0UL;
	u_long *optval_ar = (u_long *)optdata;
#if 0
	struct in6_addr *addr6 = (struct in6_addr *)optdata;
	struct in_addr *addr = (struct in_addr *)optdata;
#endif
	unsigned char *data = NULL;
	size_t length = 0;
	int err, next_ss;

	esp_request_p *req;
	esp_probe_p *p;

	switch (pm) {
		case ESP_ERROR:
			{
				struct esp_error_p *e;

				length = sizeof(esp_error_p);
				e = (esp_error_p *)data = (unsigned char *)kmalloc(length, GFP_ATOMIC);
				if (!e) {
					err = -ENOMEM;
					goto error;
				}
				memset(e, 0, length);
				e->err_type  = (__u32)htonl((u_long)level);
				e->err_code  = (__u32)htonl((u_long)optval);
				e->err_sys   = (__u32)htonl((u_long)sk->err);
				e->state     = (__u32)htonl((u_long)sk->sk_state);
				if (skb) {
					memcpy(&e->eh, skb->h.eh, sizeof(struct esphdr)); 
				}
			}
			break;
		case ESP_HANDSHAKE:
			{
				esp_handshake_p *h;

				length = sizeof(esp_handshake_p);
				h = (esp_handshake_p *)data =
					(u_char *)kmalloc(length, GFP_ATOMIC);
				if (!h) {
					err = -ENOMEM;
					goto error;
				}
				init_espsend_handshake(sk, h);
			}
			break;
		case ESP_REQUEST:
			if (!optdata)
				return (-EINVAL);
			length = sizeof(esp_request_p);
			req = (esp_request_p *)data = (u_char *)kmalloc(length, GFP_ATOMIC);
			if (!req) {
				err = -ENOMEM;
				goto error;
			}
			req->req_type    = (__u32)htonl((u_long)level);
			req->eh.pm       = (__u8)optval_ar[0];
			req->eh.seq      = (__u16)optval_ar[1]; /* nbo */
			req->eh.asl      = (__u16)optval_ar[2]; /* nbo */
			req->req_opt1    = (__u32)htonl(optval_ar[3]);
			break;
		case ESP_PROBE_RETURN:
			if (!skb)
				return (-EINVAL);
			length = sizeof(struct esp_probe_p);
			p = (struct esp_probe_p *)data = (u_char *)kmalloc(length, GFP_ATOMIC);
			if (!p) {
				err = -ENOMEM;
				goto error;
			}
			if (skb->len < length + sizeof(struct esphdr))
				return (-EINVAL);
			memcpy(p, skb->data+sizeof(struct esphdr), sizeof(struct esp_probe_p));
			p->rcv_length = (__u32)htonl(optval);
			break;
		case ESP_PROBE:
			/* set next probe length. */
			af->probe.len = max(sizeof(struct esp_probe_p), optval);

			length = sizeof(struct esp_probe_p);
			p = (struct esp_probe_p *)data = (u_char *)kmalloc(length, GFP_ATOMIC);
			if (!p) {
				err = -ENOMEM;
				goto error;
			}
			memset(p, 0, sizeof(struct esp_probe_p));
			if (af->ns_seq[ESP_PROBE] == 0 && af->s_seq[ESP_PROBE] == 0) {
				p->length = htonl(length);
			} else {
				next_ss = af->ns_seq[ESP_PROBE] < af->s_seq[ESP_PROBE] ?
					af->ns_seq[ESP_PROBE] + 65536 : af->ns_seq[ESP_PROBE];
				p->length = htonl(next_ss - af->s_seq[ESP_PROBE]);
			}
			break;
		default:
			err = -EINVAL;
			goto error;
	}

	if (data) {
		err = kmalloc_queue(data, length, &q);
		if (err)
			goto error;
		q->pm = pm;
		esp_push_queue(&af->s_queue, q);
	}

	return (0);

error:
	kfree_queue(&q);
	if (data)
		kfree(data);
	return (err);
}

int esp_set_state(struct sock *sk, int state)
{
	struct esp_sock *af = es_sk(sk);
	int oldstate = sk->sk_state;
	int err;

	if (state >= ESP_SOCKET_STATES)
		return (-EINVAL);

	NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "NET4: ESP: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: socket state %s -> %s.\n", ESP_SKADDR(sk), ESP_STATE_STRING(sk->sk_state), ESP_STATE_STRING(state))));
	sk->sk_state = state;

	switch (state) {
		case ESP_CONN_WAIT:
			if (oldstate == ESP_CLOSE) {
				err = esp_request(sk, NULL, EREQ_SOCKET_CREATE, 0);
				if (err)
					return (err);
			}
			break;
		case ESP_CONN_ACK:
			if (oldstate == ESP_CLOSE) {
				err = espsend(sk, ESP_HANDSHAKE, 0, NULL, NULL);
				if (err)
					return (err);
			}
			break;
		case ESP_ESTABLISHED:
			if (!af->rem_host)
				return (-EINVAL); /* no established sockets lack a remote host. */
			if (oldstate == ESP_CONN_ACK) {
				err = esp_request(sk, NULL, EREQ_SOCKET_OPEN, 0);
				if (err)
					return (err);
			}
			if (af->listen.sock)
				wake_up_interruptible(af->listen.sock->sleep);
#if 0
			if (af->accept_node.head.head_ptr)
				wake_up_interruptible(af->accept_node.head.head_ptr->lsk->sleep);
#endif
			break;
		case ESP_CLOSE_WAIT:
			if (oldstate == ESP_ESTABLISHED) {
				err = esp_request(sk, NULL, EREQ_SOCKET_CLOSE, 0);
				if (err)
					return (err);
			}
			break;
		case ESP_CLOSE: 
			if (sock_flag(sk, SOCK_DEAD))
				return (-ENOTCONN);
			lock_sock(sk);
			esp_unhash_sock(sk);
			set_sock_flag(sk, SOCK_DEAD);
			release_sock(sk);
#if defined(CONFIG_IPV6) || defined (CONFIG_IPV6_MODULE)
			if (sk->family == PF_INET6) {
				esp_destroy_sock(sk);
				inet6_destroy_sock(sk);
			} else 
#endif
				sk->prot->destroy(sk);
			break;
	}

	sk->state_change(sk);

	return (0);
}

/* appends (new) to (*head) list. */
void esp_push_queue(struct esp_queue_s **head, struct esp_queue_s *new)
{
	struct esp_queue_s *que;

	if (!new)
		return;

	if (!*head) {
		*head = new;
		return;
	}

	for(que=*head; que->next; que=que->next);
	que->next = new;
}

void kfree_queue(struct esp_queue_s **que)
{
	if (!*que)
		return;

	if ((*que)->data_raw)
		kfree((*que)->data_raw);

	if ((*que)->segflg)
		kfree((*que)->segflg);

	kfree(xchg(que, NULL));
}

#ifdef CONFIG_ESP_SENDWINDOW
/*
 * Allows the user to configure how many bytes is sent 
 * to a remote host per second.
 */
u_long esp_sendwindow(struct sock *sk)
{
	struct esp_sock *af = es_sk(sk);
	struct esp_remote_host *rh = af->rem_host;
	float tfrac;
	u_long msw;
	u_long now = jiffies;
	u_long diff;

	if (!rh) {
		printk(KERN_DEBUG "NET4: ESP: DEBUG: esp_sendwindow: no remote host.\n");
		return (0UL);
	}

	tfrac = (float)(now - rh->last_msw_calib / HZ);
	rh->last_msw_calib = now;
	if (tfrac >= 1.0) {
		rh->sw = rh->msw;
	} else {
		rh->sw += (u_long)((float)rh->msw * tfrac);
	}

	diff = rh->msw - rh->sw;
	if (diff < af->locked_msw)
		return (min(af->locked_msw, rh->msw) - diff);
	else 
		return (0UL);
}
#endif

struct esp_queue_s *esp_pull_queue(struct sock *sk, struct esp_queue_s **head)
{
	struct esp_queue_s *que;

#ifdef CONFIG_ESP_SENDWINDOW
	if (esp_sendwindow(sk) < IPV4_MIN_MTU) {
		/* BUG: If we can't send then this will
		 *      prohibit receiving aswell.
		 */
		return (NULL);
	}
#endif

	while ((que = *head)) {
		*head = xchg(&que->next, NULL);
		if (que->data_raw && que->data_len)
			break;
		kfree_queue(&que);
	}

	return (que);
}


/* NOTE: A lot of things set to zero explicitly by call to
 *       sk_alloc() so need not be done here.
 */
int esp_init_sock(struct sock *sk)
{
	struct esp_opt *af = &(sk->tp_pinfo.af_esp);
	int i;

	sk->zapped             = 0;
	sk->saddr              = 0;
	sk->rcv_saddr          = 0;
	sk->num                = 0;
	sk->sport              = 0;
	sk->daddr              = 0;
	sk->dport              = 0;
	sk->ack_backlog        = 0; /* f/ listen socket. */
	sk->max_ack_backlog    = 0; /* f/ listen socket. */
	af->s_id               = ESP_DEF_STREAM_ID;
	af->r_id               = ESP_DEF_STREAM_ID;
#if 0
	/* These are handled by inet_create_sock(). */
	sk->sk_state           = ESP_CLOSE;
	sk->protocol           = IPPROTO_ESP;
#endif
	sk->allocation         = GFP_ATOMIC;
	af->dictseg_len        = ESP_DEFAULT_DICTSEG_LENGTH;
	af->sl                 = IPV4_MIN_MTU - sizeof(struct esphdr) - sizeof(struct iphdr);
	af->r_overflow         = NULL;
	af->s_queue            = NULL;
	af->s_dict             = NULL;
	af->r_dict             = NULL;
	af->pck_per_burst      = 1;

	for (i = 0; i < ESP_PACKET_MODES; i++) {
		af->s_backlog[i]     = NULL;
	}

	/* reset listen components. */
	memset(&af->listen, 0, sizeof(af->listen));


#ifdef CONFIG_ESP_SENDWINDOW
	af->locked_msw         = ESP_MAX_DATA_WINDOW - 1;
#endif

	af->opt = 0;

	/* Set highest ESP checksum version. */
	af->opt |= esp_checksum_opt[ESP_VERSION];

	/* Set highest ESP compression version. */
	af->opt |= esp_compress_opt[ESP_VERSION];

	return (0);
}

/*
 *	IOCTL requests applicable to the esp protocol
 */
int esp_ioctl(struct sock *sk, int cmd, unsigned long arg)
{
	switch(cmd) 
	{
		case TIOCOUTQ:
		{
			unsigned long amount;

			if (sk->sk_state == ESP_LISTEN) return(-EINVAL);
			amount = sock_wspace(sk);
			return put_user(amount, (int *)arg);
		}

		case TIOCINQ:
		{
			struct sk_buff *skb;
			unsigned long amount;

			if (sk->sk_state == ESP_LISTEN)
				return(-EINVAL);
			amount = 0;
			/* N.B. Is this interrupt safe??
			   -> Yes. Interrupts do not remove skbs. --ANK (980725)
			 */
			skb = skb_peek(&sk->receive_queue);
			if (skb != NULL) {
				/*
				 * We will only return the amount
				 * of this packet since that is all
				 * that will be read.
				 */
				amount = skb->len - sizeof(struct esphdr);
			}
			return put_user(amount, (int *)arg);
		}

		default:
			return(-ENOIOCTLCMD);
	}
	return(0);
}

int esp_backlog_packet(struct sock *sk, u_char **pktdata, size_t pktdata_len, struct esphdr *eh, int esp_flags)
{
	struct esp_sock *af = es_sk(sk);
	struct esp_queue_s *q;
	u_char *data = *pktdata;
	int err;

	err = kmalloc_queue(data, pktdata_len, &q);
	if (err)
		return (err);
	*pktdata = NULL;

	memcpy(&q->eh, eh, sizeof(struct esphdr));
	q->esp_flags = esp_flags;
	esp_push_queue(&af->s_backlog[q->pm], q);

#if 0
	/* EXPERIMENTAL: NEEDS TO ENCOMPASS ALL PACKET MODES AT ONCE. */
	if (af->backlog_size >= ESP_MAX_BACKLOG_SIZE) {
		NETDEBUG(ESPDEBUG(long overflow = af->backlog_size));
		NETDEBUG(ESPDEBUG(int purges = 0));

		/* Flush backlog queue until < "ESP_MAX_BACKLOG_SIZE". */
		while (af->backlog_size >= ESP_MAX_BACKLOG_SIZE) {
			q = esp_pull_queue(sk, &af->s_backlog[q->pm]);
			if (!q)
				break;
			if (af->backlog_size > q->data_len)
				af->backlog_size -= q->data_len; 
			if (af->backlog_pck)
				af->backlog_pck--;
			kfree_queue(&q);
			NETDEBUG(ESPDEBUG(purges++));
		}

		NETDEBUG(ESPDEBUG(printk(KERN_DEBUG "INET: ESP: %d.%d.%d.%d:%d - %d.%d.%d.%d:%d: esp_backlog_packet: retran overflow (%ld/%d), purged %d queue(s).\n", ESP_SKADDR(sk), overflow, ESP_MAX_BACKLOG_SIZE, purges)));
	}
#endif

	return (0);
}

/* chaka-laka-boom */
int esp_retransmit(struct sock *sk, struct sk_buff *skb, 
		u_char pm, __u32 nbo_seq, __u32 nbo_asl)
{
	struct esp_sock *af = es_sk(sk);
	struct esp_queue_s *q;
	int err;

	if (pm >= ESP_MAX_PACKET_MODES) {
		err = esp_error(sk, skb, E_PARAM, ESE_INVALPCKMODE);
		if (err)
			return (err);
	}

	if (pm >= ESP_PACKET_MODES) {
		/* unsupported packet mode. */
		return (0);
	}

	/* trim/scan backlog queue */
	while ((q = esp_pull_queue(sk, &af->r_overflow))) {
		if (q->eh.seq == nbo_seq &&
				q->eh.asl == nbo_asl)
			break;
		kfree_queue(&q);
	}
	if (!q) { 
		esp_error(sk, skb, E_QUEUE, ESE_RETRANTIMEOUT);
		return (-ENODATA);
	}

	/* mark data as a "retransmit" packet. */
	q->esp_flags |= EOPT_RETRAN;
//	q->esp_flags |= EOPT_IP4_FRAG;

	/* assign backlog queue to next queue node */
	af->s_backlog[pm] = q->next;

	/* attach to send queue */
	q->next = af->s_queue;
	af->s_queue = q;

	return (0);
}

/* Attach socket to ESP socket hash table. */
void es_hash_sock(struct sock *sk)
{
	struct sock **skp;
	int num = sk->num;

	num &= (ESP_HTABLE_SIZE - 1);
	skp = &es_hash[num].sk_list;

	write_lock_bh(&es_hash_lock);
	sk->next = *skp;
	*skp = sk;
	sk->hashent = num;
	write_unlock_bh(&es_hash_lock);
}

void esp_rehash_sock(struct sock *sk)
{
	struct sock **skp;
	int num = sk->num;
	int oldnum = sk->hashent;

	num &= (ESP_HTABLE_SIZE - 1);
	skp = &es_hash[oldnum].sk_list;

	write_lock_bh(&es_hash_lock);
	while(*skp != NULL) {
		if(*skp == sk) {
			*skp = sk->next;
			break;
		}
		skp = &((*skp)->next);
	}
	sk->next = es_hash[num].sk_list;
	es_hash[num].sk_list = sk;
	sk->hashent = num;
	write_unlock_bh(&es_hash_lock);
}

static unsigned int esp_listen_poll(struct sock *sk, poll_table *wait)
{
	struct esp_sock *af = es_sk(sk);
	int flg = 0;
	int i;

	if (!af->listen.queue)
		return (-EINVAL);

	lock_sock(sk);
	for (i=0; i<sk->ack_backlog; i++) {
		if (af->listen.queue[i] && 
				af->listen.queue[i]->state == ESP_ESTABLISHED) {
			flg |= POLLIN;
			break;
		}
	}
	release_sock(sk);

	return (flg);
}

/*
 *  Wait for a ESP event.
 *
 *  Note that we don't need to lock the socket, as the upper poll layers
 *  take care of normal races (between the test and the event) and we don't
 *  go look at any of the socket buffers directly.
 */
unsigned int esp_poll(struct file *file, struct socket *sock, poll_table *wait)
{
	unsigned int mask;
	struct sock *sk = sock->sk;
	struct esp_sock *af = es_sk(sk);

	poll_wait(file, sk->sleep, wait);

	if (sk->sk_state == ESP_LISTEN)
		return (esp_listen_poll(sk, wait));

	mask = 0;
	if (sk->err)
		mask = POLLERR;
	if (sk->shutdown & RCV_SHUTDOWN) {
		mask |= POLLHUP;
	}

	if (sk->done) {
		mask |= POLLIN;
	}

	if (sk->sk_state == ESP_ESTABLISHED) {
		if (!skb_queue_empty(&sk->receive_queue)) {
			mask |= POLLIN | POLLRDNORM;
		}

		if (sock_writeable(sk))
			mask |= POLLOUT | POLLWRNORM | POLLWRBAND;
		else {
			set_bit (SOCK_ASYNC_NOSPACE, &sk->socket->flags);
			set_bit (SOCK_NOSPACE, &sk->socket->flags);
			//sk->socket->flags |= SO_NOSPACE;
		}

#if 0
		/* no concept of priority data in esp. */
		if (af->opt & EOPT_PRIORITY_DATA)
			mask |= POLLPRI;
#endif
	}
	
	return (mask);
}

/* Dettach socket from ESP socket hash table. */
void esp_unhash_sock(struct sock *sk)
{
	struct sock **skp;
	int num = sk->num;

	num &= (ESP_HTABLE_SIZE - 1);
	skp = &es_hash[num].sk_list;

	write_lock_bh(&es_hash_lock);
	while(*skp != NULL) {
		if(*skp == sk) {
			*skp = sk->next;
			sk->next = NULL;
			break;
		}
		skp = &((*skp)->next);
	}
	write_unlock_bh(&es_hash_lock);
}

/* Returns a unused socket port number. */
unsigned short esp_get_port(void)
{
	static int start = 0;
	struct sock *sk;
	int best, best_size_so_far;
	int result;
	int i;

	if (start > sysctl_local_port_range[1] || start <
			sysctl_local_port_range[0])
		start = sysctl_local_port_range[0];

	best_size_so_far = 32767; /* "big" num */
	best = result = start;

	write_lock_bh(&es_hash_lock);
	for(i = 0; i < ESP_HTABLE_SIZE; i++, result++) {
		struct sock *sk;
		int size;

		sk = es_hash[result & (ESP_HTABLE_SIZE - 1)].sk_list;
		if(!sk) {
			if (result > sysctl_local_port_range[1])
				result = sysctl_local_port_range[0] + ((result -
							sysctl_local_port_range[0]) & (ESP_HTABLE_SIZE - 1));
			goto out;
		}

		/* Is this one better than our best so far? */
		size = 0;
		do {
			if(++size >= best_size_so_far)
				goto next;
		} while((sk = sk->next) != NULL);
		best_size_so_far = size;
		best = result;

next:
	}

	result = best;

	for(;; result += ESP_HTABLE_SIZE) {
		/* Get into range (but preserve hash bin)... */
		if (result > sysctl_local_port_range[1])
			result = sysctl_local_port_range[0]
				+ ((result - sysctl_local_port_range[0]) &
						(ESP_HTABLE_SIZE - 1));

		sk = es_hash[result & (ESP_HTABLE_SIZE-1)].sk_list;
		for (; sk != NULL && sk->num != result; sk = sk->next); 
		if (sk != NULL) 
			break;
	}
	write_unlock_bh(&es_hash_lock);

out:
	start = result;
	return (result);
}

#endif